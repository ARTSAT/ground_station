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
**      Xcode 4.6 (LLVM compiler 4.2)
**
**      TGSTransceiverCIV.cpp
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

#include "TGSTransceiverCIV.h"

namespace tgs {

#define DEFAULT_ID          (0xE0)

/*public */TGSTransceiverCIV::TGSTransceiverCIV(void)
{
    _id = DEFAULT_ID;
    _notifier = NULL;
}

/*public virtual */TGSTransceiverCIV::~TGSTransceiverCIV(void)
{
    close();
    disconnect();
}

/*public */TGSError TGSTransceiverCIV::setOperationFrequency(unsigned char id, int param)
{
    static std::string const request("\x05", 1);
    std::string content;
    TGSError error(TGSERROR_OK);
    
    if (isValid()) {
        if ((error = formatOperationFrequency(param, &content)) == TGSERROR_OK) {
            error = set(id, request, content);
        }
    }
    else {
        error = TGSERROR_INVALID_STATE;
    }
    return error;
}

/*public */TGSError TGSTransceiverCIV::getOperationFrequency(unsigned char id, int* result)
{
    static std::string const request("\x03", 1);
    std::string content;
    TGSError error(TGSERROR_OK);
    
    if (isValid()) {
        if (result != NULL) {
            if ((error = get(id, request, &content)) == TGSERROR_OK) {
                error = parseOperationFrequency(content, result);
            }
        }
        else {
            error = TGSERROR_INVALID_PARAM;
        }
    }
    else {
        error = TGSERROR_INVALID_STATE;
    }
    return error;
}

/*public */TGSError TGSTransceiverCIV::setOperationMode(unsigned char id, OperationModeEnum mode, FilterEnum filter)
{
    static std::string const request("\x06", 1);
    std::string content;
    TGSError error(TGSERROR_OK);
    
    if (isValid()) {
        if ((error = formatOperationMode(mode, filter, &content)) == TGSERROR_OK) {
            error = set(id, request, content);
        }
    }
    else {
        error = TGSERROR_INVALID_STATE;
    }
    return error;
}

/*public */TGSError TGSTransceiverCIV::getOperationMode(unsigned char id, OperationModeEnum* mode, FilterEnum* filter)
{
    static std::string const request("\x04", 1);
    std::string content;
    FilterEnum dummy;
    TGSError error(TGSERROR_OK);
    
    if (isValid()) {
        if (mode != NULL) {
            if ((error = get(id, request, &content)) == TGSERROR_OK) {
                if ((error = parseOperationMode(content, mode, &dummy)) == TGSERROR_OK) {
                    if (filter != NULL) {
                        *filter = dummy;
                    }
                }
            }
        }
        else {
            error = TGSERROR_INVALID_PARAM;
        }
    }
    else {
        error = TGSERROR_INVALID_STATE;
    }
    return error;
}

/*public */TGSError TGSTransceiverCIV::setSatelliteMode(unsigned char id, bool param)
{
    static std::string const request("\x16\x5A", 2);
    
    return setParamBool(id, request, param);
}

/*public */TGSError TGSTransceiverCIV::getSatelliteMode(unsigned char id, bool* result)
{
    static std::string const request("\x16\x5A", 2);
    
    return getParamBool(id, request, result);
}

/*public */TGSError TGSTransceiverCIV::setTransceiveMode(unsigned char id, bool param)
{
    static std::string const request("\x1A\x05\x00\x58", 4);
    
    return setParamBool(id, request, param);
}

/*public */TGSError TGSTransceiverCIV::getTransceiveMode(unsigned char id, bool* result)
{
    static std::string const request("\x1A\x05\x00\x58", 4);
    
    return getParamBool(id, request, result);
}

/*public */TGSError TGSTransceiverCIV::setDataMode(unsigned char id, bool mode, FilterEnum filter)
{
    static std::string const request("\x1A\x06", 2);
    std::string content;
    TGSError error(TGSERROR_OK);
    
    if (isValid()) {
        content += static_cast<unsigned char>(!!mode);
        if (!mode) {
            filter = static_cast<FilterEnum>(0);
        }
        else if (filter == FILTER_UNKNOWN) {
            filter = FILTER_FIL1;
        }
        content += static_cast<unsigned char>(filter);
        error = set(id, request, content);
    }
    else {
        error = TGSERROR_INVALID_STATE;
    }
    return error;
}

/*public */TGSError TGSTransceiverCIV::getDataMode(unsigned char id, bool* mode, FilterEnum* filter)
{
    static std::string const request("\x1A\x06", 2);
    std::string content;
    TGSError error(TGSERROR_OK);
    
    if (isValid()) {
        if (mode != NULL) {
            if ((error = get(id, request, &content)) == TGSERROR_OK) {
                if (content.size() >= 2) {
                    *mode = !!content[0];
                    if (filter != NULL) {
                        if (!!content[0]) {
                            *filter = static_cast<FilterEnum>(content[1]);
                        }
                        else {
                            *filter = FILTER_UNKNOWN;
                        }
                    }
                }
                else {
                    error = TGSERROR_INVALID_FORMAT;
                }
            }
        }
        else {
            error = TGSERROR_INVALID_PARAM;
        }
    }
    else {
        error = TGSERROR_INVALID_STATE;
    }
    return error;
}

/*public virtual */TGSError TGSTransceiverCIV::open(std::string const& port, int baud, bool verbose)
{
    static std::string const request("\x19\x00", 2);
    TGSError error(TGSERROR_OK);
    
    if (300 <= baud && baud <= 19200) {
        if ((error = super::open(port, baud, true, verbose)) == TGSERROR_OK) {
            if ((error = write(_id, request)) == TGSERROR_OK) {
                if (verbose) {
                    std::cout << "TGSTransceiverCIV::open [port : " << port << ", baud : " << baud << "]" << std::endl;
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

/*public virtual */void TGSTransceiverCIV::close(void)
{
    _queue.clear();
    super::close();
    return;
}

/*public virtual */void TGSTransceiverCIV::update(void)
{
    static std::string const request_frequency("\x00", 1);
    static std::string const request_mode("\x01", 1);
    std::vector<std::pair<unsigned char, std::string> >::iterator it;
    int frequency;
    OperationModeEnum mode;
    FilterEnum filter;
    
    super::update();
    transceive();
    for (it = _queue.begin(); it != _queue.end(); ++it) {
        if (test(request_frequency, it->second, &it->second) == TGSERROR_OK) {
            if (parseOperationFrequency(it->second, &frequency) == TGSERROR_OK) {
                notifyOperationFrequency(it->first, frequency);
            }
        }
        else if (test(request_mode, it->second, &it->second) == TGSERROR_OK) {
            if (parseOperationMode(it->second, &mode, &filter) == TGSERROR_OK) {
                notifyOperationMode(it->first, mode, filter);
            }
        }
    }
    _queue.clear();
    return;
}

/*public */TGSError TGSTransceiverCIV::selectVFOMode(unsigned char id)
{
    static std::string const request("\x07", 1);
    
    return selectParamVoid(id, request);
}

/*public */TGSError TGSTransceiverCIV::selectVFOA(unsigned char id)
{
    static std::string const request("\x07\x00", 2);
    
    return selectParamVoid(id, request);
}

/*public */TGSError TGSTransceiverCIV::selectVFOB(unsigned char id)
{
    static std::string const request("\x07\x01", 2);
    
    return selectParamVoid(id, request);
}

/*public */TGSError TGSTransceiverCIV::equalizeVFO(unsigned char id)
{
    static std::string const request("\x07\xA0", 2);
    
    return selectParamVoid(id, request);
}

/*public */TGSError TGSTransceiverCIV::exchangeBand(unsigned char id)
{
    static std::string const request("\x07\xB0", 2);
    
    return selectParamVoid(id, request);
}

/*public */TGSError TGSTransceiverCIV::selectMainBand(unsigned char id)
{
    static std::string const request("\x07\xD0", 2);
    
    return selectParamVoid(id, request);
}

/*public */TGSError TGSTransceiverCIV::selectSubBand(unsigned char id)
{
    static std::string const request("\x07\xD1", 2);
    
    return selectParamVoid(id, request);
}

/*private */TGSError TGSTransceiverCIV::connect(Transceiver* param)
{
    std::vector<Transceiver*>::const_iterator it;
    TGSError error(TGSERROR_OK);
    
    if (param != NULL) {
        for (it = _transceiver.begin(); it != _transceiver.end(); ++it) {
            if (*it == param) {
                break;
            }
        }
        if (it == _transceiver.end()) {
            _transceiver.push_back(param);
        }
    }
    else {
        error = TGSERROR_INVALID_PARAM;
    }
    return error;
}

/*private */void TGSTransceiverCIV::disconnect(Transceiver* param)
{
    std::vector<Transceiver*>::iterator it;
    
    if (param != NULL) {
        for (it = _transceiver.begin(); it != _transceiver.end(); ++it) {
            if (*it == param) {
                it = _transceiver.erase(it);
                --it;
            }
        }
    }
    return;
}

/*private */void TGSTransceiverCIV::disconnect(void)
{
    std::vector<Transceiver*>::const_iterator it;
    
    for (it = _transceiver.begin(); it != _transceiver.end(); ++it) {
        if (*it != NULL) {
            (*it)->_civ = NULL;
        }
    }
    _transceiver.clear();
    return;
}

/*private */TGSError TGSTransceiverCIV::setParamBool(unsigned char id, std::string const& request, bool param)
{
    std::string content;
    TGSError error(TGSERROR_OK);
    
    if (isValid()) {
        content += static_cast<unsigned char>(!!param);
        error = set(id, request, content);
    }
    else {
        error = TGSERROR_INVALID_STATE;
    }
    return error;
}

/*private */TGSError TGSTransceiverCIV::getParamBool(unsigned char id, std::string const& request, bool* result)
{
    std::string content;
    TGSError error(TGSERROR_OK);
    
    if (isValid()) {
        if (result != NULL) {
            if ((error = get(id, request, &content)) == TGSERROR_OK) {
                if (content.size() >= 1) {
                    *result = !!content[0];
                }
                else {
                    error = TGSERROR_INVALID_FORMAT;
                }
            }
        }
        else {
            error = TGSERROR_INVALID_PARAM;
        }
    }
    else {
        error = TGSERROR_INVALID_STATE;
    }
    return error;
}

/*private */TGSError TGSTransceiverCIV::selectParamVoid(unsigned char id, std::string const& request)
{
    TGSError error(TGSERROR_OK);
    
    if (isValid()) {
        error = set(id, request, "");
    }
    else {
        error = TGSERROR_INVALID_STATE;
    }
    return error;
}

/*private */TGSError TGSTransceiverCIV::set(unsigned char id, std::string const& request, std::string const& param)
{
    static std::string const request_ok("\xFB", 1);
    static std::string const request_ng("\xFA", 1);
    std::string response;
    TGSError error(TGSERROR_OK);
    
    flushRead();
    if ((error = write(id, request + param)) == TGSERROR_OK) {
        if ((error = read(id, &response)) == TGSERROR_OK) {
            if ((error = test(request_ng, response, &response)) == TGSERROR_OK) {
                error = TGSERROR_FAILED;
            }
            else if ((error = test(request_ok, response, &response)) != TGSERROR_OK) {
                error = TGSERROR_INVALID_FORMAT;
            }
        }
    }
    return error;
}

/*private */TGSError TGSTransceiverCIV::get(unsigned char id, std::string const& request, std::string* result)
{
    std::string response;
    TGSError error(TGSERROR_OK);
    
    flushRead();
    if ((error = write(id, request)) == TGSERROR_OK) {
        if ((error = read(id, &response)) == TGSERROR_OK) {
            error = test(request, response, result);
        }
    }
    return error;
}

/*private */TGSError TGSTransceiverCIV::write(unsigned char id, std::string const& param)
{
    std::string string;
    
    string += id;
    string += _id;
    string += param;
    return super::write("\xFE\xFE", string, "\xFD", true);
}

/*private */TGSError TGSTransceiverCIV::read(unsigned char id, std::string* result)
{
    std::string string;
    TGSError error(TGSERROR_OK);
    
    while (true) {
        if ((error = super::read("\xFE\xFE", &string, "\xFD", false)) == TGSERROR_OK) {
            if (string.find("\xFC") == std::string::npos) {
                if (string.size() >= 2) {
                    if (static_cast<unsigned char>(string[0]) == _id && static_cast<unsigned char>(string[1]) == id) {
                        *result = string.substr(2);
                        break;
                    }
                    else if (static_cast<unsigned char>(string[0]) == 0x00) {
                        _queue.push_back(std::pair<unsigned char, std::string>(static_cast<unsigned char>(string[1]), string.substr(2)));
                    }
                }
            }
        }
        else {
            break;
        }
    }
    return error;
}

/*private */void TGSTransceiverCIV::transceive(void)
{
    std::string string;
    
    while (available()) {
        if (super::read("\xFE\xFE", &string, "\xFD", true) == TGSERROR_OK) {
            if (string.find("\xFC") == std::string::npos) {
                if (string.size() >= 2) {
                    if (static_cast<unsigned char>(string[0]) == 0x00) {
                        _queue.push_back(std::pair<unsigned char, std::string>(static_cast<unsigned char>(string[1]), string.substr(2)));
                    }
                }
            }
        }
    }
    return;
}

/*private */void TGSTransceiverCIV::notifyOperationFrequency(unsigned char id, int param) const
{
    std::vector<Transceiver*>::const_iterator it;
    
    if (_notifier != NULL) {
        _notifier->onNotifyOperationFrequency(param);
    }
    for (it = _transceiver.begin(); it != _transceiver.end(); ++it) {
        if (*it != NULL) {
            if ((*it)->_id == id) {
                if ((*it)->_notifier != NULL) {
                    (*it)->_notifier->onNotifyOperationFrequency(param);
                }
            }
        }
    }
    return;
}

/*private */void TGSTransceiverCIV::notifyOperationMode(unsigned char id, OperationModeEnum mode, FilterEnum filter) const
{
    std::vector<Transceiver*>::const_iterator it;
    
    if (_notifier != NULL) {
        _notifier->onNotifyOperationMode(mode, filter);
    }
    for (it = _transceiver.begin(); it != _transceiver.end(); ++it) {
        if (*it != NULL) {
            if ((*it)->_id == id) {
                if ((*it)->_notifier != NULL) {
                    (*it)->_notifier->onNotifyOperationMode(mode, filter);
                }
            }
        }
    }
    return;
}

/*private static */TGSError TGSTransceiverCIV::test(std::string const& request, std::string const& response, std::string* result)
{
    TGSError error(TGSERROR_OK);
    
    if (response.find(request) == 0) {
        *result = response.substr(request.size());
    }
    else {
        error = TGSERROR_INVALID_FORMAT;
    }
    return error;
}

/*private static */TGSError TGSTransceiverCIV::formatOperationFrequency(int param, std::string* result)
{
    std::string string;
    int temp;
    int i;
    TGSError error(TGSERROR_OK);
    
    if (param >= 0) {
        string.resize(5);
        for (i = 0; i < 5; ++i) {
            temp = param % 10;
            param /= 10;
            string[i] = static_cast<unsigned char>(((param % 10) << 4) | (temp << 0));
            param /= 10;
        }
        *result = string;
    }
    else {
        error = TGSERROR_INVALID_PARAM;
    }
    return error;
}

/*private static */TGSError TGSTransceiverCIV::parseOperationFrequency(std::string const& param, int* result)
{
    int frequency;
    int temp;
    int i;
    TGSError error(TGSERROR_OK);
    
    if (param.size() >= 5) {
        frequency = 0;
        for (i = 4; i >= 0; --i) {
            if ((temp = (static_cast<unsigned char>(param[i]) >> 4) & 0x0F) < 10) {
                frequency *= 10;
                frequency += temp;
                if ((temp = (static_cast<unsigned char>(param[i]) >> 0) & 0x0F) < 10) {
                    frequency *= 10;
                    frequency += temp;
                }
                else {
                    error = TGSERROR_INVALID_FORMAT;
                    break;
                }
            }
            else {
                error = TGSERROR_INVALID_FORMAT;
                break;
            }
        }
        if (error == TGSERROR_OK) {
            *result = frequency;
        }
    }
    else {
        error = TGSERROR_INVALID_FORMAT;
    }
    return error;
}

/*private static */TGSError TGSTransceiverCIV::formatOperationMode(OperationModeEnum mode, FilterEnum filter, std::string* result)
{
    std::string string;
    
    string += static_cast<unsigned char>(mode);
    if (filter != FILTER_UNKNOWN) {
        string += static_cast<unsigned char>(filter);
    }
    *result = string;
    return TGSERROR_OK;
}

/*private static */TGSError TGSTransceiverCIV::parseOperationMode(std::string const& param, OperationModeEnum* mode, FilterEnum* filter)
{
    TGSError error(TGSERROR_OK);
    
    if (param.size() >= 1) {
        *mode = static_cast<OperationModeEnum>(param[0]);
        if (param.size() >= 2) {
            *filter = static_cast<FilterEnum>(param[1]);
        }
        else {
            *filter = FILTER_FIL1;
        }
    }
    else {
        error = TGSERROR_INVALID_FORMAT;
    }
    return error;
}

/*protected virtual */void TGSTransceiverCIV::Notifier::onNotifyOperationFrequency(int frequency)
{
    std::cout << "TGSTransceiverCIV::Notifier::onNotifyOperationFrequency [frequency : " << frequency << " Hz]" << std::endl;
    return;
}

/*protected virtual */void TGSTransceiverCIV::Notifier::onNotifyOperationMode(OperationModeEnum mode, FilterEnum filter)
{
    std::cout << "TGSTransceiverCIV::Notifier::onNotifyOperationMode [mode : ";
    switch (mode) {
        case OPERATIONMODE_LSB:
            std::cout << "LSB";
            break;
        case OPERATIONMODE_USB:
            std::cout << "USB";
            break;
        case OPERATIONMODE_AM:
            std::cout << "AM";
            break;
        case OPERATIONMODE_CW:
            std::cout << "CW";
            break;
        case OPERATIONMODE_RTTY:
            std::cout << "RTTY";
            break;
        case OPERATIONMODE_FM:
            std::cout << "FM";
            break;
        case OPERATIONMODE_CW_R:
            std::cout << "CW-R";
            break;
        case OPERATIONMODE_RTTY_R:
            std::cout << "RTTY-R";
            break;
        case OPERATIONMODE_DV:
            std::cout << "DV";
            break;
        default:
            std::cout << "Unknown";
            break;
    }
    std::cout << ", filter : ";
    switch (filter) {
        case FILTER_FIL1:
            std::cout << "FIL1";
            break;
        case FILTER_FIL2:
            std::cout << "FIL2";
            break;
        case FILTER_FIL3:
            std::cout << "FIL3";
            break;
        default:
            std::cout << "Unknown";
            break;
    }
    std::cout << "]" << std::endl;
    return;
}

/*protected */TGSTransceiverCIV::Transceiver::Transceiver(unsigned char id)
{
    _id = id;
    _civ = NULL;
    _notifier = NULL;
}

/*protected */TGSTransceiverCIV::Transceiver::~Transceiver(void)
{
    disconnect();
}

/*public */TGSError TGSTransceiverCIV::Transceiver::setOperationFrequency(int param)
{
    TGSError error(TGSERROR_OK);
    
    if (_civ != NULL) {
        error = _civ->setOperationFrequency(_id, param);
    }
    else {
        error = TGSERROR_INVALID_STATE;
    }
    return error;
}

/*public */TGSError TGSTransceiverCIV::Transceiver::getOperationFrequency(int* result)
{
    TGSError error(TGSERROR_OK);
    
    if (_civ != NULL) {
        error = _civ->getOperationFrequency(_id, result);
    }
    else {
        error = TGSERROR_INVALID_STATE;
    }
    return error;
}

/*public */TGSError TGSTransceiverCIV::Transceiver::setOperationMode(OperationModeEnum mode, FilterEnum filter)
{
    TGSError error(TGSERROR_OK);
    
    if (_civ != NULL) {
        error = _civ->setOperationMode(_id, mode, filter);
    }
    else {
        error = TGSERROR_INVALID_STATE;
    }
    return error;
}

/*public */TGSError TGSTransceiverCIV::Transceiver::getOperationMode(OperationModeEnum* mode, FilterEnum* filter)
{
    TGSError error(TGSERROR_OK);
    
    if (_civ != NULL) {
        error = _civ->getOperationMode(_id, mode, filter);
    }
    else {
        error = TGSERROR_INVALID_STATE;
    }
    return error;
}

/*public */TGSError TGSTransceiverCIV::Transceiver::setSatelliteMode(bool param)
{
    TGSError error(TGSERROR_OK);
    
    if (_civ != NULL) {
        error = _civ->setSatelliteMode(_id, param);
    }
    else {
        error = TGSERROR_INVALID_STATE;
    }
    return error;
}

/*public */TGSError TGSTransceiverCIV::Transceiver::getSatelliteMode(bool* result)
{
    TGSError error(TGSERROR_OK);
    
    if (_civ != NULL) {
        error = _civ->getSatelliteMode(_id, result);
    }
    else {
        error = TGSERROR_INVALID_STATE;
    }
    return error;
}

/*public */TGSError TGSTransceiverCIV::Transceiver::setTransceiveMode(bool param)
{
    TGSError error(TGSERROR_OK);
    
    if (_civ != NULL) {
        error = _civ->setTransceiveMode(_id, param);
    }
    else {
        error = TGSERROR_INVALID_STATE;
    }
    return error;
}

/*public */TGSError TGSTransceiverCIV::Transceiver::getTransceiveMode(bool* result)
{
    TGSError error(TGSERROR_OK);
    
    if (_civ != NULL) {
        error = _civ->getTransceiveMode(_id, result);
    }
    else {
        error = TGSERROR_INVALID_STATE;
    }
    return error;
}

/*public */TGSError TGSTransceiverCIV::Transceiver::setDataMode(bool mode, FilterEnum filter)
{
    TGSError error(TGSERROR_OK);
    
    if (_civ != NULL) {
        error = _civ->setDataMode(_id, mode, filter);
    }
    else {
        error = TGSERROR_INVALID_STATE;
    }
    return error;
}

/*public */TGSError TGSTransceiverCIV::Transceiver::getDataMode(bool* mode, FilterEnum* filter)
{
    TGSError error(TGSERROR_OK);
    
    if (_civ != NULL) {
        error = _civ->getDataMode(_id, mode, filter);
    }
    else {
        error = TGSERROR_INVALID_STATE;
    }
    return error;
}

/*public */TGSError TGSTransceiverCIV::Transceiver::connect(TGSTransceiverCIV* param)
{
    TGSError error(TGSERROR_OK);
    
    if (_civ == NULL) {
        if (param != NULL) {
            if ((error = param->connect(this)) == TGSERROR_OK) {
                _civ = param;
            }
        }
        else {
            error = TGSERROR_INVALID_PARAM;
        }
    }
    else {
        error = TGSERROR_INVALID_STATE;
    }
    return error;
}

/*public */void TGSTransceiverCIV::Transceiver::disconnect(void)
{
    if (_civ != NULL) {
        _civ->disconnect(this);
    }
    _civ = NULL;
    return;
}

/*public */TGSError TGSTransceiverCIV::Transceiver::selectVFOMode(void)
{
    TGSError error(TGSERROR_OK);
    
    if (_civ != NULL) {
        error = _civ->selectVFOMode(_id);
    }
    else {
        error = TGSERROR_INVALID_STATE;
    }
    return error;
}

/*public */TGSError TGSTransceiverCIV::Transceiver::selectVFOA(void)
{
    TGSError error(TGSERROR_OK);
    
    if (_civ != NULL) {
        error = _civ->selectVFOA(_id);
    }
    else {
        error = TGSERROR_INVALID_STATE;
    }
    return error;
}

/*public */TGSError TGSTransceiverCIV::Transceiver::selectVFOB(void)
{
    TGSError error(TGSERROR_OK);
    
    if (_civ != NULL) {
        error = _civ->selectVFOB(_id);
    }
    else {
        error = TGSERROR_INVALID_STATE;
    }
    return error;
}

/*public */TGSError TGSTransceiverCIV::Transceiver::equalizeVFO(void)
{
    TGSError error(TGSERROR_OK);
    
    if (_civ != NULL) {
        error = _civ->equalizeVFO(_id);
    }
    else {
        error = TGSERROR_INVALID_STATE;
    }
    return error;
}

/*public */TGSError TGSTransceiverCIV::Transceiver::exchangeBand(void)
{
    TGSError error(TGSERROR_OK);
    
    if (_civ != NULL) {
        error = _civ->exchangeBand(_id);
    }
    else {
        error = TGSERROR_INVALID_STATE;
    }
    return error;
}

/*public */TGSError TGSTransceiverCIV::Transceiver::selectMainBand(void)
{
    TGSError error(TGSERROR_OK);
    
    if (_civ != NULL) {
        error = _civ->selectMainBand(_id);
    }
    else {
        error = TGSERROR_INVALID_STATE;
    }
    return error;
}

/*public */TGSError TGSTransceiverCIV::Transceiver::selectSubBand(void)
{
    TGSError error(TGSERROR_OK);
    
    if (_civ != NULL) {
        error = _civ->selectSubBand(_id);
    }
    else {
        error = TGSERROR_INVALID_STATE;
    }
    return error;
}

}// end of namespace
