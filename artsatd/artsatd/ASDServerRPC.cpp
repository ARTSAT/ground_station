/*
**      ARTSAT Project
**
**      Original Copyright (C) 2014 - 2014 Ron Hashimoto.
**                                          http://h2so5.net/
**                                          mail@h2so5.net
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
    
    Variant toVariant(const rapidjson::Value& value)
    {
        switch (value.GetType()) {
            case 1:
                return false;
            case 2:
                return true;
            case 3:
            {
                std::map<std::string, Variant> map;
                for (rapidjson::Value::ConstMemberIterator it = value.MemberBegin(); it != value.MemberEnd(); ++it) {
                    std::string name = it->name.GetString();
                    map[name] = toVariant(it->value);
                }
                return map;
            }
            case 4:
            {
                std::list<Variant> list;
                for (rapidjson::Value::ConstValueIterator it = value.Begin(); it != value.End(); ++it) {
                    list.push_back(toVariant(*it));
                }
                return list;
            }
            case 5:
                return std::string(value.GetString());
            case 6:
                return value.GetDouble();
            default:
                return boost::blank();
        }
    }
    
    void toJson(const Variant& variant, rapidjson::Value* value, rapidjson::Document::AllocatorType& alloc)
    {
        switch (variant.which()) {
            case 0:
                value->SetBool(boost::get<bool>(value));
                break;
            case 1:
                value->SetDouble(boost::get<double>(variant));
                break;
            case 2:
                value->SetString(boost::get<std::string>(variant).c_str(), alloc);
                break;
            case 3:
            {
                value->SetObject();
                const std::map<std::string, Variant>& map = boost::get<std::map<std::string, Variant> >(variant);
                for (std::map<std::string, Variant>::const_iterator it = map.begin(); it != map.end(); ++it) {
                    rapidjson::Value child_val;
                    toJson(it->second, &child_val, alloc);
                    value->AddMember(it->first.c_str(), alloc, child_val, alloc);
                }
            }
                break;
            case 4:
            {
                value->SetArray();
                const std::list<Variant>& list = boost::get<std::list<Variant> >(variant);
                for (std::list<Variant>::const_iterator it = list.begin(); it != list.end(); ++it) {
                    rapidjson::Value child_val;
                    toJson(*it, &child_val, alloc);
                    value->PushBack(child_val, alloc);
                }
            }
                break;
            default:
                value->SetNull();
        }
    }

}