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
**      MBSMainBoard.cpp
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

#include "MBSMainBoard.h"

/*public */MBSMainBoard::MBSMainBoard(void)
{
    _notifier = NULL;
}

/*public virtual */MBSMainBoard::~MBSMainBoard(void)
{
    close();
}

/*public virtual */TGSError MBSMainBoard::open(std::string const& port, int baud, bool verbose)
{
    std::string response;
    TGSError error(TGSERROR_OK);
    
    if (baud == 4800) {
        if ((error = super::open(port, baud, false, verbose)) == TGSERROR_OK) {
            flushRead();
            if ((error = write("c-m-c-eco")) == TGSERROR_OK) {
                if ((error = read(&response)) == TGSERROR_OK) {
                    if (response == "r-c-m-eco") {
                        if (verbose) {
                            std::cout << "MBSMainBoard::open [port : " << port << ", baud : " << baud << "]" << std::endl;
                        }
                    }
                    else {
                        error = TGSERROR_INVALID_STATE;
                    }
                }
            }
            if (error != TGSERROR_OK) {
                self::close();
            }
        }
    }
    else {
        error = TGSERROR_INVALID_PARAM;
    }
    return error;
}

/*public virtual */void MBSMainBoard::close(void)
{
    _queue.clear();
    super::close();
    return;
}

/*public virtual */TGSError MBSMainBoard::update(void)
{
    std::vector<std::string>::iterator it;
    TGSError error(TGSERROR_OK);
    
    if ((error = super::update()) == TGSERROR_OK) {
        poll();
        for (it = _queue.begin(); it != _queue.end(); ++it) {
            if (*it == "c-c-m-nsd") {
                if (_notifier != NULL) {
                    _notifier->onNotifyNSD();
                }
            }
            else if (*it == "c-c-m-don") {
                if (_notifier != NULL) {
                    _notifier->onNotifyDON();
                }
            }
            else if (*it == "c-c-m-dof") {
                if (_notifier != NULL) {
                    _notifier->onNotifyDOF();
                }
            }
        }
        _queue.clear();
    }
    return error;
}

/*public */TGSError MBSMainBoard::pushTelemetry(unsigned char const* data, int length)
{
    std::string request;
    TGSError error(TGSERROR_OK);
    
    if (isValid()) {
        request = "c-m-c-smm";
        if (data != NULL && length > 0) {
            request += "-";
            request.append(reinterpret_cast<char const*>(data), length);
        }
        error = write(request);
    }
    else {
        error = TGSERROR_INVALID_STATE;
    }
    return error;
}

/*public */TGSError MBSMainBoard::shutdown(void)
{
    TGSError error(TGSERROR_OK);
    
    if (isValid()) {
        error = write("c-m-c-asd");
    }
    else {
        error = TGSERROR_INVALID_STATE;
    }
    return error;
}

/*private */TGSError MBSMainBoard::write(std::string const& param)
{
    return super::write("", param, " \x0D\x0A", false);
}

/*private */TGSError MBSMainBoard::read(std::string* result)
{
    std::string string;
    TGSError error(TGSERROR_OK);
    
    while (true) {
        if ((error = super::read("", &string, " \x0D\x0A", false)) == TGSERROR_OK) {
            if (string.find("r-c-") == 0) {
                *result = string;
                break;
            }
            else if (string.find("c-c-") == 0) {
                _queue.push_back(string);
            }
        }
        else {
            break;
        }
    }
    return error;
}

/*private */void MBSMainBoard::poll(void)
{
    std::string string;
    
    while (available()) {
        if (super::read("", &string, " \x0D\x0A", true) == TGSERROR_OK) {
            if (string.find("c-c-") == 0) {
                _queue.push_back(string);
            }
        }
    }
    return;
}

/*protected virtual */void MBSMainBoard::Notifier::onNotifyNSD(void)
{
    return;
}

/*protected virtual */void MBSMainBoard::Notifier::onNotifyDON(void)
{
    return;
}

/*protected virtual */void MBSMainBoard::Notifier::onNotifyDOF(void)
{
    return;
}
