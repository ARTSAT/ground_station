/*
**      IridiumFrameworks
**
**      Original Copyright (C) 2012 - 2012 HORIGUCHI Junshi.
**                                          http://iridium.jp/
**                                          zap00365@nifty.com
**      Portions Copyright (C) <year> <author>
**                                          <website>
**                                          <e-mail>
**      Version     C++03
**      Website     http://iridium.jp/
**      E-mail      zap00365@nifty.com
**
**      This source code is for Xcode.
**      Xcode 4.2 (LLVM compiler 3.0)
**
**      IRXTimeDiff.cpp
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

#include "IRXTimeDiff.h"
#include "IRXTimeType.h"
#include "IRXTime.h"

namespace ir {

/*public */long IRXTimeDiff::operator[](std::string const& param) const
{
    std::string key;
    int i;
    long result(-1);
    
    key = param;
    for (i = 0; i < key.size(); ++i) {
        key[i] = tolower(key[i]);
    }
    if (key == "day") {
        result = labs(_second / 86400);
    }
    else if (key == "hour") {
        result = labs(_second / 3600 % 24);
    }
    else if (key == "minute") {
        result = labs(_second / 60 % 60);
    }
    else if (key == "second") {
        result = labs(_second % 60);
    }
    return result;
}

/*public */IRXTimeDiff& IRXTimeDiff::set(long day, int hour, int minute, int second)
{
    _second = 86400 * day + 3600 * hour + 60 * minute + second;
    return *this;
}

/*public */void IRXTimeDiff::get(long* day, int* hour, int* minute, int* second) const
{
    time_t value;
    
    value = _second;
    if (second != NULL) {
        *second = static_cast<int>(value % 60);
    }
    value /= 60;
    if (minute != NULL) {
        *minute = static_cast<int>(value % 60);
    }
    value /= 60;
    if (hour != NULL) {
        *hour = static_cast<int>(value % 24);
    }
    value /= 24;
    if (day != NULL) {
        *day = value;
    }
    return;
}

/*public */IRXTimeDiff& IRXTimeDiff::set(long hour, int minute, int second)
{
    _second = 3600 * hour + 60 * minute + second;
    return *this;
}

/*public */void IRXTimeDiff::get(long* hour, int* minute, int* second) const
{
    time_t value;
    
    value = _second;
    if (second != NULL) {
        *second = static_cast<int>(value % 60);
    }
    value /= 60;
    if (minute != NULL) {
        *minute = static_cast<int>(value % 60);
    }
    value /= 60;
    if (hour != NULL) {
        *hour = value;
    }
    return;
}

/*public */IRXTimeDiff& IRXTimeDiff::set(long minute, int second)
{
    _second = 60 * minute + second;
    return *this;
}

/*public */void IRXTimeDiff::get(long* minute, int* second) const
{
    time_t value;
    
    value = _second;
    if (second != NULL) {
        *second = static_cast<int>(value % 60);
    }
    value /= 60;
    if (minute != NULL) {
        *minute = value;
    }
    return;
}

/*public */int IRXTimeDiff::compare(IRXTimeDiff const& param) const
{
    int result(0);
    
    if (_second > param._second) {
        result = +1;
    }
    else if (_second < param._second) {
        result = -1;
    }
    return result;
}

/*public static */IRXTimeDiff IRXTimeDiff::localTimeOffset(void)
{
    return IRXTimeDiff(-IRXTime::epochUTCTime().asTime_t());
}

}// end of namespace
