/*
**      ARTSAT Project
**
**      Original Copyright (C) 2013 - 2014 HORIGUCHI Junshi.
**                                          http://iridium.jp/
**                                          zap00365@nifty.com
**      Portions Copyright (C) <year> <author>
**                                          <website>
**                                          <e-mail>
**      Version     POSIX / C++11
**      Website     http://artsat.jp/
**      E-mail      info@artsat.jp
**
**      This source code is for Xcode.
**      Xcode 6.1 (Apple LLVM 6.0)
**
**      TGSDeviceManager.cpp
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

#include "TGSDeviceManager.h"

namespace tgs {

/*public */TGSError TGSDeviceManager::getByIndex(int index, TGSDeviceInterface** device, std::string* port, int* baud) const
{
    std::vector<RecordRec>::const_iterator it;
    TGSError error(TGSERROR_OK);
    
    if (0 <= index && index < _record.size()) {
        it = _record.begin() + index;
        if (device != NULL) {
            *device = it->device;
        }
        if (port != NULL) {
            *port = it->port;
        }
        if (baud != NULL) {
            *baud = it->baud;
        }
    }
    else {
        error = TGSERROR_INVALID_PARAM;
    }
    return error;
}

/*public */TGSError TGSDeviceManager::getByDevice(TGSDeviceInterface* device, std::string* port, int* baud) const
{
    std::vector<RecordRec>::const_iterator it;
    TGSError error(TGSERROR_OK);
    
    if (device != NULL) {
        for (it = _record.begin(); it != _record.end(); ++it) {
            if (it->device == device) {
                if (port != NULL) {
                    *port = it->port;
                }
                if (baud != NULL) {
                    *baud = it->baud;
                }
                break;
            }
        }
        if (it == _record.end()) {
            error = TGSERROR_NO_RESULT;
        }
    }
    else {
        error = TGSERROR_INVALID_PARAM;
    }
    return error;
}

/*public */TGSError TGSDeviceManager::append(TGSDeviceInterface* device, std::string const& port, int baud)
{
    std::vector<RecordRec>::const_iterator it;
    RecordRec record;
    TGSError error(TGSERROR_OK);
    
    if (device != NULL) {
        for (it = _record.begin(); it != _record.end(); ++it) {
            if (it->device == device) {
                break;
            }
        }
        if (it == _record.end()) {
            record.device = device;
            record.port = port;
            record.baud = baud;
            _record.push_back(record);
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

/*public */void TGSDeviceManager::removeByIndex(int index)
{
    std::vector<RecordRec>::iterator it;
    
    if (0 <= index && index < _record.size()) {
        it = _record.begin() + index;
        _record.erase(it);
    }
    return;
}

/*public */void TGSDeviceManager::removeByDevice(TGSDeviceInterface* device)
{
    std::vector<RecordRec>::iterator it;
    
    if (device != NULL) {
        for (it = _record.begin(); it != _record.end(); ++it) {
            if (it->device == device) {
                it = _record.erase(it);
                --it;
            }
        }
    }
    return;
}

/*public */TGSError TGSDeviceManager::open(bool verbose)
{
    TGSSerial serial;
    std::vector<TGSSerialDeviceInfo> info;
    std::vector<TGSSerialDeviceInfo>::iterator iit;
    std::vector<RecordRec>::iterator rit;
    int i;
    TGSError error(TGSERROR_OK);
    
    if (verbose) {
        std::cout << "--- found serial devices ---" << std::endl;
    }
    info = serial.getDeviceList();
    for (iit = info.begin(); iit != info.end(); ++iit) {
        if (verbose) {
            std::cout << "[ID " << iit->getDeviceID() << "] " << iit->getDeviceName();
        }
        if (iit->getDeviceName().find("tty.usbserial") != 0) {
            iit = info.erase(iit);
            --iit;
        }
        else if (verbose) {
            std::cout << " (detected)";
        }
        if (verbose) {
            std::cout << std::endl;
        }
    }
    for (rit = _record.begin(); rit != _record.end(); ++rit) {
        rit->support = true;
        if (!rit->device->isValid()) {
            if (!rit->port.empty()) {
                switch (rit->device->open(rit->port, rit->baud, verbose)) {
                    case TGSERROR_NO_SUPPORT:
                        rit->support = false;
                        break;
                    default:
                        // nop
                        break;
                }
            }
        }
    }
    if (verbose) {
        std::cout << "--- fixed serial devices ---" << std::endl;
    }
    for (rit = _record.begin(), i = 0; rit != _record.end(); ++rit, ++i) {
        if (rit->device->isValid()) {
            if (verbose) {
                std::cout << "[Index " << i << "] " << rit->port;
            }
            for (iit = info.begin(); iit != info.end(); ++iit) {
                if (iit->getDeviceName() == rit->port) {
                    if (verbose) {
                        std::cout << " (ID " << iit->getDeviceID() << ")";
                    }
                    iit = info.erase(iit);
                    --iit;
                }
            }
            if (verbose) {
                std::cout << std::endl;
            }
        }
    }
    for (iit = info.begin(); iit != info.end(); ++iit) {
        for (rit = _record.begin(); rit != _record.end(); ++rit) {
            if (!rit->device->isValid()) {
                switch (rit->device->open(iit->getDeviceName(), rit->baud, verbose)) {
                    case TGSERROR_OK:
                        rit->port = iit->getDeviceName();
                        break;
                    case TGSERROR_NO_SUPPORT:
                        rit->support = false;
                        break;
                    default:
                        // nop
                        break;
                }
            }
        }
    }
    if (verbose) {
        std::cout << "--- wired serial devices ---" << std::endl;
        for (rit = _record.begin(), i = 0; rit != _record.end(); ++rit, ++i) {
            if (rit->device->isValid()) {
                for (iit = info.begin(); iit != info.end(); ++iit) {
                    if (iit->getDeviceName() == rit->port) {
                        std::cout << "[Index " << i << "] " << rit->port << " (ID " << iit->getDeviceID() << ")" << std::endl;
                    }
                }
            }
        }
        std::cout << "----------------------------" << std::endl;
    }
    for (rit = _record.begin(); rit != _record.end(); ++rit) {
        if (rit->support) {
            if (!rit->device->isValid()) {
                switch (rit->device->open("", 0, false)) {
                    case TGSERROR_NO_SUPPORT:
                        rit->support = false;
                        break;
                    case TGSERROR_OK:
                        rit->device->close();
                    default:
                        error = TGSERROR_FAILED;
                        break;
                }
            }
        }
    }
    return error;
}

/*public */void TGSDeviceManager::close(void)
{
    std::vector<RecordRec>::const_iterator it;
    
    for (it = _record.begin(); it != _record.end(); ++it) {
        it->device->close();
    }
    return;
}

/*public */void TGSDeviceManager::update(void)
{
    std::vector<RecordRec>::const_iterator it;
    
    for (it = _record.begin(); it != _record.end(); ++it) {
        it->device->update();
    }
    return;
}

}// end of namespace
