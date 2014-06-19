/*
**      ARTSAT Project
**
**      Original Copyright (C) 2014 - 2014 Ron Hashimoto.
**                                          http://h2so5.net/
**                                          mail@h2so5.net
**      Portions Copyright (C) 2014 - 2014 HORIGUCHI Junshi.
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
**      Xcode 5.1.1 (Apple LLVM 5.1)
**
**      ASDServerRPC.cpp
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

#include "ASDServerRPC.h"

namespace ASDServerRPC {

void toVariant(rapidjson::Value const& param, Variant* result)
{
    Variant value;
    
    if (result != NULL) {
        switch (param.GetType()) {
            case rapidjson::kFalseType:
                *result = false;
                break;
            case rapidjson::kTrueType:
                *result = true;
                break;
            case rapidjson::kNumberType:
                *result = param.GetDouble();
                break;
            case rapidjson::kStringType:
                *result = param.GetString();
                break;
            case rapidjson::kArrayType:
                {
                    std::list<Variant> list;
                    rapidjson::Value::ConstValueIterator it;
                    for (it = param.Begin(); it != param.End(); ++it) {
                        toVariant(*it, &value);
                        list.push_back(value);
                    }
                    *result = list;
                }
                break;
            case rapidjson::kObjectType:
                {
                    std::map<std::string, Variant> map;
                    rapidjson::Value::ConstMemberIterator it;
                    for (it = param.MemberBegin(); it != param.MemberEnd(); ++it) {
                        toVariant(it->value, &value);
                        map[it->name.GetString()] = value;
                    }
                    *result = map;
                }
                break;
            default:
                *result = boost::blank();
                break;
        }
    }
    return;
}

void toJSON(Variant const& param, rapidjson::Value* result, rapidjson::Document::AllocatorType& allocator)
{
    rapidjson::Value value;
    
    if (result != NULL) {
        switch (param.which()) {
            case VARIANTTYPE_BOOL:
                result->SetBool(boost::get<bool>(param));
                break;
            case VARIANTTYPE_INT:
                result->SetInt(boost::get<int>(param));
                break;
            case VARIANTTYPE_DOUBLE:
                result->SetDouble(boost::get<double>(param));
                break;
            case VARIANTTYPE_STRING:
                result->SetString(boost::get<std::string>(param).c_str(), allocator);
                break;
            case VARIANTTYPE_LIST:
                {
                    result->SetArray();
                    std::list<Variant> const& list(boost::get<std::list<Variant> >(param));
                    std::list<Variant>::const_iterator it;
                    for (it = list.begin(); it != list.end(); ++it) {
                        toJSON(*it, &value, allocator);
                        result->PushBack(value, allocator);
                    }
                }
                break;
            case VARIANTTYPE_MAP:
                {
                    result->SetObject();
                    std::map<std::string, Variant> const& map(boost::get<std::map<std::string, Variant> >(param));
                    std::map<std::string, Variant>::const_iterator it;
                    for (it = map.begin(); it != map.end(); ++it) {
                        toJSON(it->second, &value, allocator);
                        result->AddMember(it->first.c_str(), allocator, value, allocator);
                    }
                }
                break;
            default:
                result->SetNull();
                break;
        }
    }
    return;
}

}// end of namespace
