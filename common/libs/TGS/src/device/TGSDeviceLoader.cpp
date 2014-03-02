/*
**      ARTSAT Project
**
**      Original Copyright (C) 2013 - 2014 HORIGUCHI Junshi.
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
**      TGSDeviceLoader.cpp
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

#include "TGSDeviceLoader.h"

namespace tgs {

#define DEFAULT_PORT        ("")
#define DEFAULT_BAUD        (9600)

/*public */TGSError TGSDeviceLoader::get(TGSDeviceInterface* device, std::string* result) const
{
    std::map<std::string, TGSDeviceInterface*>::const_iterator it;
    TGSError error(TGSERROR_OK);
    
    if (device != NULL && result != NULL) {
        for (it = _record.begin(); it != _record.end(); ++it) {
            if (it->second == device) {
                *result = it->first;
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

/*public */TGSError TGSDeviceLoader::append(TGSDeviceInterface* device, std::string const& param)
{
    TGSError error(TGSERROR_OK);
    
    if (device != NULL && !param.empty()) {
        if (_record.find(param) == _record.end()) {
            _record.insert(std::map<std::string, TGSDeviceInterface*>::value_type(param, device));
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

/*public */void TGSDeviceLoader::remove(TGSDeviceInterface* device)
{
    std::map<std::string, TGSDeviceInterface*>::iterator it;
    
    if (device != NULL) {
        for (it = _record.begin(); it != _record.end(); ++it) {
            if (it->second == device) {
                _record.erase(std::map<std::string, TGSDeviceInterface*>::iterator(it));
            }
        }
    }
    return;
}

/*public */TGSError TGSDeviceLoader::open(std::string const& file, bool verbose)
{
    std::map<std::string, TGSDeviceInterface*>::const_iterator it;
    TGSError error(TGSERROR_OK);
    
    close();
    _manager.clear();
    if ((error = load(file, verbose)) != TGSERROR_OK) {
        _manager.clear();
        error = load();
    }
    if (error == TGSERROR_OK) {
        error = _manager.open(verbose);
        if (verbose) {
            std::cout << "TGSDeviceLoader::open [result : " << error.print() << "]" << std::endl;
        }
        if ((error = save(file)) != TGSERROR_OK) {
            error = TGSERROR_FILE_NOTSAVED;
        }
        _verbose = verbose;
    }
    else {
        _manager.clear();
    }
    return error;
}

/*private */TGSError TGSDeviceLoader::save(std::string const& file) const
{
    static char const table[] = ""
        "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
        "<device>\n"
        "</device>\n"
    "";
    tinyxml2::XMLDocument xml;
    tinyxml2::XMLElement* root;
    tinyxml2::XMLElement* type;
    TGSDeviceInterface* device;
    std::string tag;
    std::string port;
    int baud;
    int i;
    TGSError error(TGSERROR_OK);
    
    if (xml.Parse(table) == tinyxml2::XML_NO_ERROR) {
        if ((root = xml.FirstChildElement("device")) != NULL) {
            for (i = 0; i < _manager.getSize(); ++i) {
                if ((error = _manager.getByIndex(i, &device, &port, &baud)) == TGSERROR_OK) {
                    if ((error = get(device, &tag)) == TGSERROR_OK) {
                        if ((type = xml.NewElement(tag.c_str())) != NULL) {
                            if ((error = write(&xml, type, "port", port)) == TGSERROR_OK) {
                                if ((error = write(&xml, type, "baud", baud)) == TGSERROR_OK) {
                                    if (root->InsertEndChild(type) == NULL) {
                                        error = TGSERROR_FAILED;
                                        break;
                                    }
                                }
                                else {
                                    break;
                                }
                            }
                            else {
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
                else {
                    break;
                }
            }
            if (error == TGSERROR_OK) {
                if (xml.SaveFile(file.c_str()) != tinyxml2::XML_NO_ERROR) {
                    error = TGSERROR_FILE_NOTSAVED;
                }
            }
        }
        else {
            error = TGSERROR_FAILED;
        }
    }
    else {
        error = TGSERROR_FAILED;
    }
    return error;
}

/*private */TGSError TGSDeviceLoader::load(std::string const& file, bool verbose)
{
    tinyxml2::XMLDocument xml;
    tinyxml2::XMLElement const* root;
    tinyxml2::XMLElement const* type;
    std::string port;
    int baud;
    std::map<std::string, TGSDeviceInterface*>::const_iterator it;
    TGSError error(TGSERROR_OK);
    
    switch (xml.LoadFile(file.c_str())) {
        case tinyxml2::XML_NO_ERROR:
            if ((root = xml.FirstChildElement("device")) != NULL) {
                for (type = root->FirstChildElement(); type != NULL; type = type->NextSiblingElement()) {
                    port = DEFAULT_PORT;
                    if ((error = read(type, "port", &port)) == TGSERROR_OK) {
                        baud = DEFAULT_BAUD;
                        if ((error = read(type, "baud", &baud)) == TGSERROR_OK) {
                            if ((it = _record.find(type->Name())) != _record.end()) {
                                if ((error = _manager.append(it->second, port, baud)) != TGSERROR_OK) {
                                    break;
                                }
                            }
                        }
                        else {
                            break;
                        }
                    }
                    else {
                        break;
                    }
                }
            }
            else {
                error = TGSERROR_INVALID_FORMAT;
            }
            break;
        case tinyxml2::XML_ERROR_FILE_NOT_FOUND:
            error = TGSERROR_FILE_NOTFOUND;
            break;
        default:
            error = TGSERROR_FAILED;
            break;
    }
    return error;
}

/*private */TGSError TGSDeviceLoader::load(void)
{
    std::map<std::string, TGSDeviceInterface*>::const_iterator it;
    TGSError error(TGSERROR_OK);
    
    for (it = _record.begin(); it != _record.end(); ++it) {
        if ((error = _manager.append(it->second, DEFAULT_PORT, DEFAULT_BAUD)) != TGSERROR_OK) {
            break;
        }
    }
    return error;
}

/*private static */TGSError TGSDeviceLoader::write(tinyxml2::XMLDocument* xml, tinyxml2::XMLElement* parent, std::string const& tag, int param)
{
    char temp[32];
    TGSError error(TGSERROR_OK);
    
    if (snprintf(temp, sizeof(temp), "%d", param) >= 0) {
        error = write(xml, parent, tag, temp);
    }
    else {
        error = TGSERROR_FAILED;
    }
    return error;
}

/*private static */TGSError TGSDeviceLoader::write(tinyxml2::XMLDocument* xml, tinyxml2::XMLElement* parent, std::string const& tag, std::string const& param)
{
    tinyxml2::XMLElement* element;
    tinyxml2::XMLText* text;
    TGSError error(TGSERROR_OK);
    
    if ((element = xml->NewElement(tag.c_str())) != NULL) {
        if ((text = xml->NewText(param.c_str())) != NULL) {
            if (element->InsertEndChild(text) != NULL) {
                if (parent->InsertEndChild(element) == NULL) {
                    error = TGSERROR_FAILED;
                }
            }
            else {
                error = TGSERROR_FAILED;
            }
        }
        else {
            error = TGSERROR_FAILED;
        }
    }
    else {
        error = TGSERROR_FAILED;
    }
    return error;
}

/*private static */TGSError TGSDeviceLoader::read(tinyxml2::XMLElement const* parent, std::string const& tag, int* result)
{
    tinyxml2::XMLElement const* element;
    TGSError error(TGSERROR_OK);
    
    if ((element = parent->FirstChildElement(tag.c_str())) != NULL) {
        if (!element->NoChildren()) {
            if (element->QueryIntText(result) != tinyxml2::XML_NO_ERROR) {
                error = TGSERROR_INVALID_FORMAT;
            }
        }
    }
    else {
        error = TGSERROR_INVALID_FORMAT;
    }
    return error;
}

/*private static */TGSError TGSDeviceLoader::read(tinyxml2::XMLElement const* parent, std::string const& tag, std::string* result)
{
    tinyxml2::XMLElement const* element;
    TGSError error(TGSERROR_OK);
    
    if ((element = parent->FirstChildElement(tag.c_str())) != NULL) {
        if (!element->NoChildren()) {
            *result = element->GetText();
        }
    }
    else {
        error = TGSERROR_INVALID_FORMAT;
    }
    return error;
}

}// end of namespace
