/*
**      ARTSAT Project
**
**      Original Copyright (C) 2013 - 2013 HORIGUCHI Junshi.
**                                          http://iridium.jp/
**                                          zap00365@nifty.com
**      Portions Copyright (C) <year> <author>
**                                          <website>
**                                          <e-mail>
**      Version     POSIX / C++03
**      Website     http://artsat.jp/
**      E-mail      info@artsat.jp
**
**      This source code is for Xcode.
**      Xcode 4.6.2 (Apple LLVM compiler 4.2, LLVM GCC 4.2)
**
**      TGSDeviceInterface.cpp
**
**      ------------------------------------------------------------------------
**
**      The MIT License (MIT)
**
**      Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
**      associated documentation files (the "Software"), to deal in the Software without restriction,
**      including without limitation the rights to use, copy, modify, merge, publish, distribute,
**      sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
**      furnished to do so, subject to the following conditions:
**      The above copyright notice and this permission notice shall be included in all copies or
**      substantial portions of the Software.
**      THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
**      BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
**      IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
**      WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
**      OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
**
**      以下に定める条件に従い、本ソフトウェアおよび関連文書のファイル（以下「ソフトウェア」）の複製を
**      取得するすべての人に対し、ソフトウェアを無制限に扱うことを無償で許可します。
**      これには、ソフトウェアの複製を使用、複写、変更、結合、掲載、頒布、サブライセンス、および、または販売する権利、
**      およびソフトウェアを提供する相手に同じことを許可する権利も無制限に含まれます。
**      上記の著作権表示および本許諾表示を、ソフトウェアのすべての複製または重要な部分に記載するものとします。
**      ソフトウェアは「現状のまま」で、明示であるか暗黙であるかを問わず、何らの保証もなく提供されます。
**      ここでいう保証とは、商品性、特定の目的への適合性、および権利非侵害についての保証も含みますが、それに限定されるものではありません。
**      作者または著作権者は、契約行為、不法行為、またはそれ以外であろうと、ソフトウェアに起因または関連し、
**      あるいはソフトウェアの使用またはその他の扱いによって生じる一切の請求、損害、その他の義務について何らの責任も負わないものとします。
*/

#include "TGSDeviceInterface.h"

namespace tgs {

#define INTERVAL_POLLING    (1000)
#define INTERVAL_RETRY      (50000)

/*protected */TGSDeviceInterface::TGSDeviceInterface(void) : _state(false)
{
    _timeout = TIMEOUT_DEFAULT;
}

/*public virtual */TGSDeviceInterface::~TGSDeviceInterface(void)
{
    close();
}

/*public virtual */bool TGSDeviceInterface::isValid(void) const
{
    return _state;
}

/*public virtual */TGSError TGSDeviceInterface::open(std::string const& port, int baud, bool verbose)
{
    return open(port, baud, false, verbose);
}

/*public virtual */void TGSDeviceInterface::close(void)
{
    _serial.close();
    _state = false;
    return;
}

/*public virtual */TGSError TGSDeviceInterface::update(void)
{
    TGSError error(TGSERROR_OK);
    
    if (!_state) {
        error = TGSERROR_INVALID_STATE;
    }
    return error;
}

/*protected */bool TGSDeviceInterface::available(void)
{
    bool result(false);
    
    if (_state) {
        if (_serial.available() > 0) {
            result = true;
        }
    }
    return result;
}

/*protected */TGSError TGSDeviceInterface::open(std::string const& port, int baud, bool flow, bool verbose)
{
    TGSError error(TGSERROR_OK);
    
    if (!port.empty() && baud > 0) {
        if (!_state) {
            _state = true;
            if (_serial.setup(port, baud, flow)) {
                _serial.flush();
            }
            else {
                error = TGSERROR_FAILED;
            }
            if (error != TGSERROR_OK) {
                self::close();
            }
        }
        else {
            error = TGSERROR_INVALID_STATE;
        }
    }
    else {
        error = TGSERROR_INVALID_PARAM;
    }
    return error;
}

/*protected */TGSError TGSDeviceInterface::write(std::string const& prefix, std::string const& param, std::string const& postfix, bool verify)
{
    int count;
    TGSError error(TGSERROR_OK);
    
    if (_state) {
        count = 0;
        while (true) {
            if (verify) {
                _serial.flush(true, false);
            }
            if ((error = write(prefix, verify)) == TGSERROR_OK) {
                if ((error = write(param, verify)) == TGSERROR_OK) {
                    error = write(postfix, verify);
                }
            }
            if (error == TGSERROR_INVALID_STATE) {
                error = TGSERROR_OK;
                while (true) {
                    if (_timeout >= 0) {
                        if (++count > _timeout * 1000 / INTERVAL_RETRY) {
                            error = TGSERROR_TIMEOUT;
                            break;
                        }
                    }
                    if (verify) {
                        _serial.flush(true, false);
                    }
                    usleep(INTERVAL_RETRY);
                    if (!verify) {
                        break;
                    }
                    else if (_serial.available() <= 0) {
                        break;
                    }
                }
                if (error != TGSERROR_OK) {
                    break;
                }
            }
            else {
                break;
            }
        }
        _serial.drain();
    }
    else {
        error = TGSERROR_INVALID_STATE;
    }
    return error;
}

/*protected */TGSError TGSDeviceInterface::read(std::string const& prefix, std::string* result, std::string const& postfix, bool immediate)
{
    int temp;
    std::string string;
    std::string::size_type pos;
    TGSError error(TGSERROR_OK);
    
    if (result != NULL && !postfix.empty()) {
        if (_state) {
            _serial.drain();
            if ((error = read(prefix, immediate)) == TGSERROR_OK) {
                while (true) {
                    if ((error = wait()) == TGSERROR_OK) {
                        if ((temp = _serial.readByte()) >= 0) {
                            string += static_cast<unsigned char>(temp);
                            if ((pos = string.find(prefix)) != std::string::npos) {
                                string = string.substr(pos + prefix.size());
                            }
                            if ((pos = string.find(postfix)) != std::string::npos) {
                                *result = string.substr(0, pos);
                                break;
                            }
                        }
                        else {
                            error = TGSERROR_FAILED;
                            break;
                        }
                    }
                    else {
                        break;
                    }
                }
            }
        }
        else {
            error = TGSERROR_INVALID_STATE;
        }
    }
    else {
        error = TGSERROR_INVALID_PARAM;
    }
    return error;
}

/*protected */void TGSDeviceInterface::flushWrite(void)
{
    if (_state) {
        _serial.flush(false, true);
    }
    return;
}

/*protected */void TGSDeviceInterface::flushRead(void)
{
    if (_state) {
        _serial.flush(true, false);
    }
    return;
}

/*private */TGSError TGSDeviceInterface::write(std::string const& string, bool verify)
{
    int temp;
    int i;
    TGSError error(TGSERROR_OK);
    
    for (i = 0; i < string.size(); ++i) {
        if (_serial.writeByte(string[i])) {
            if (verify) {
                if ((error = wait()) == TGSERROR_OK) {
                    if ((temp = _serial.readByte()) >= 0) {
                        if (static_cast<unsigned char>(temp) != static_cast<unsigned char>(string[i])) {
                            error = TGSERROR_INVALID_STATE;
                            break;
                        }
                    }
                    else {
                        error = TGSERROR_FAILED;
                        break;
                    }
                }
                else {
                    break;
                }
            }
        }
        else {
            error = TGSERROR_FAILED;
            break;
        }
    }
    return error;
}

/*private */TGSError TGSDeviceInterface::read(std::string const& string, bool immediate)
{
    int temp;
    int i;
    TGSError error(TGSERROR_OK);
    
    while (true) {
        for (i = 0; i < string.size(); ++i) {
            if ((error = wait()) == TGSERROR_OK) {
                if ((temp = _serial.readByte()) >= 0) {
                    if (static_cast<unsigned char>(temp) != static_cast<unsigned char>(string[i])) {
                        if (_serial.available() <= 0 && immediate) {
                            error = TGSERROR_NO_RESULT;
                        }
                        else {
                            error = TGSERROR_INVALID_STATE;
                        }
                        break;
                    }
                }
                else {
                    error = TGSERROR_FAILED;
                    break;
                }
            }
            else {
                break;
            }
        }
        if (error == TGSERROR_INVALID_STATE) {
            error = TGSERROR_OK;
        }
        else {
            break;
        }
    }
    return error;
}

/*private */TGSError TGSDeviceInterface::wait(void)
{
    int count;
    TGSError error(TGSERROR_OK);
    
    count = 0;
    while (_serial.available() <= 0) {
        if (_timeout >= 0) {
            if (++count > _timeout * 1000 / INTERVAL_POLLING) {
                error = TGSERROR_TIMEOUT;
                break;
            }
        }
        usleep(INTERVAL_POLLING);
    }
    return error;
}

}// end of namespace
