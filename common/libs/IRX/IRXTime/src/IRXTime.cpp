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
**      IRXTime.cpp
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

#include "IRXTime.h"
#include <iostream>
#include "IRXTimeType.h"

namespace ir {

#define FORMAT_YMD(param)       (std::string("%YYYY") + (param) + "%MM" + (param) + "%DD %hh:%mm:%ss")
#define FORMAT_MDY(param)       (std::string("%MM") + (param) + "%DD" + (param) + "%YYYY %hh:%mm:%ss")
#define FORMAT_DMY(param)       (std::string("%DD") + (param) + "%MM" + (param) + "%YYYY %hh:%mm:%ss")
#define FORMAT_ISO8601_BASIC    ("%YYYY%MM%DDT%hh%mm%ss")
#define FORMAT_ISO8601_EXTENDED ("%YYYY-%MM-%DDT%hh:%mm:%ss")

static  char const              irxtime_month[][13][10] = {
    {"MONTH", "JANUARY", "FEBRUARY", "MARCH", "APRIL", "MAY", "JUNE", "JULY", "AUGUST", "SEPTEMBER", "OCTOBER", "NOVEMBER", "DECEMBER"},
    {"Month", "January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"},
    {"month", "january", "february", "march", "april", "may", "june", "july", "august", "september", "october", "november", "december"}
};
static  char const              irxtime_mth[][13][4] = {
    {"MTH", "JAN", "FEB", "MAR", "APR", "MAY", "JUN", "JUL", "AUG", "SEP", "OCT", "NOV", "DEC"},
    {"Mth", "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"},
    {"mth", "jan", "feb", "mar", "apr", "may", "jun", "jul", "aug", "sep", "oct", "nov", "dec"}
};
static  char const              irxtime_week[][8][10] = {
    {"WEEK", "SUNDAY", "MONDAY", "TUESDAY", "WEDNESDAY", "THURSDAY", "FRIDAY", "SATURDAY"},
    {"Week", "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"},
    {"week", "sunday", "monday", "tuesday", "wednesday", "thursday", "friday", "saturday"}
};
static  char const              irxtime_wek[][8][4] = {
    {"WEK", "SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"},
    {"Wek", "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"},
    {"wek", "sun", "mon", "tue", "wed", "thu", "fri", "sat"}
};
static  char const              irxtime_youbi[][8][4] = {
    {"JY", "日", "月", "火", "水", "木", "金", "土"}
};
static  char const              irxtime_ampm[][3][5] = {
    {"AN", "AM", "PM"},
    {"an", "am", "pm"},
    {"AD", "A.M.", "P.M."},
    {"ad", "a.m.", "p.m."}
};
static  char const              irxtime_gozengogo[][3][7] = {
    {"JG", "午前", "午後"}
};
static  char const              irxtime_formatint[][5] = {
    "%d", "%01d", "%02d", "%03d", "%04d"
};
static  char const              irxtime_formatlong[][6] = {
    "%ld"
};

/*public */int IRXTime::operator[](std::string const& param) const
{
    std::string key;
    int i;
    int result(-1);
    
    key = param;
    for (i = 0; i < key.size(); ++i) {
        key[i] = tolower(key[i]);
    }
    if (key == "year") {
        result = _year;
    }
    else if (key == "month") {
        result = _month;
    }
    else if (key == "day") {
        result = _day;
    }
    else if (key == "hour") {
        result = _hour;
    }
    else if (key == "minute") {
        result = _minute;
    }
    else if (key == "second") {
        result = _second;
    }
    return result;
}

/*public */IRXTime& IRXTime::set(IRXTime const& param)
{
    _year = param._year;
    _month = param._month;
    _day = param._day;
    _hour = param._hour;
    _minute = param._minute;
    _second = param._second;
    _days = param._days;
    _week = param._week;
    return *this;
}

/*public */IRXTime& IRXTime::set(int year, int month, int day, int hour, int minute, int second)
{
    from(year, month, day, hour, minute, second, false);
    return *this;
}

/*public */void IRXTime::get(int* year, int* month, int* day, int* hour, int* minute, int* second, DayOfWeekEnum* week) const
{
    if (year != NULL) {
        *year = _year;
    }
    if (month != NULL) {
        *month = _month;
    }
    if (day != NULL) {
        *day = _day;
    }
    if (hour != NULL) {
        *hour = _hour;
    }
    if (minute != NULL) {
        *minute = _minute;
    }
    if (second != NULL) {
        *second = _second;
    }
    if (week != NULL) {
        *week = _week;
    }
    return;
}

/*public */IRXTime& IRXTime::set(int year, int day, int hour, int minute, int second)
{
    from(year, 1, day, hour, minute, second, false);
    return *this;
}

/*public */void IRXTime::get(int* year, int* day, int* hour, int* minute, int* second, DayOfWeekEnum* week) const
{
    if (year != NULL) {
        *year = _year;
    }
    if (day != NULL) {
        *day = _days;
    }
    if (hour != NULL) {
        *hour = _hour;
    }
    if (minute != NULL) {
        *minute = _minute;
    }
    if (second != NULL) {
        *second = _second;
    }
    if (week != NULL) {
        *week = _week;
    }
    return;
}

/*public */IRXTime& IRXTime::set(int year, int hour, int minute, int second)
{
    from(year, 1, 1, hour, minute, second, false);
    return *this;
}

/*public */void IRXTime::get(int* year, int* hour, int* minute, int* second) const
{
    if (year != NULL) {
        *year = _year;
    }
    if (hour != NULL) {
        *hour = 24 * (_days - 1) + _hour;
    }
    if (minute != NULL) {
        *minute = _minute;
    }
    if (second != NULL) {
        *second = _second;
    }
    return;
}

/*public */IRXTime& IRXTime::set(int year, int minute, int second)
{
    from(year, 1, 1, 0, minute, second, false);
    return *this;
}

/*public */void IRXTime::get(int* year, int* minute, int* second) const
{
    if (year != NULL) {
        *year = _year;
    }
    if (minute != NULL) {
        *minute = 1440 * (_days - 1) + 60 * _hour + _minute;
    }
    if (second != NULL) {
        *second = _second;
    }
    return;
}

/*public */IRXTime& IRXTime::set(int year, int second)
{
    from(year, 1, 1, 0, 0, second, false);
    return *this;
}

/*public */void IRXTime::get(int* year, int* second) const
{
    if (year != NULL) {
        *year = _year;
    }
    if (second != NULL) {
        *second = 86400 * (_days - 1) + 3600 * _hour + 60 * _minute + _second;
    }
    return;
}

/*public */IRXTime& IRXTime::set(time_t param)
{
    from(param, true);
    return *this;
}

/*public */IRXTime& IRXTime::setDate(IRXTime const& param)
{
    from(param._year, param._month, param._day, _hour, _minute, _second, false);
    return *this;
}

/*public */IRXTime& IRXTime::setDate(int year, int month, int day)
{
    from(year, month, day, _hour, _minute, _second, false);
    return *this;
}

/*public */void IRXTime::getDate(int* year, int* month, int* day, DayOfWeekEnum* week) const
{
    if (year != NULL) {
        *year = _year;
    }
    if (month != NULL) {
        *month = _month;
    }
    if (day != NULL) {
        *day = _day;
    }
    if (week != NULL) {
        *week = _week;
    }
    return;
}

/*public */IRXTime& IRXTime::setDate(int year, int day)
{
    from(year, 1, day, _hour, _minute, _second, false);
    return *this;
}

/*public */void IRXTime::getDate(int* year, int* day, DayOfWeekEnum* week) const
{
    if (year != NULL) {
        *year = _year;
    }
    if (day != NULL) {
        *day = _days;
    }
    if (week != NULL) {
        *week = _week;
    }
    return;
}

/*public */IRXTime& IRXTime::setTime(IRXTime const& param)
{
    from(_year, _month, _day, param._hour, param._minute, param._second, false);
    return *this;
}

/*public */IRXTime& IRXTime::setTime(int hour, int minute, int second)
{
    from(_year, _month, _day, hour, minute, second, false);
    return *this;
}

/*public */int IRXTime::getTime(int* hour, int* minute, int* second) const
{
    int result(0);
    
    result = 3600 * _hour + 60 * _minute + _second;
    if (hour != NULL) {
        *hour = _hour;
    }
    if (minute != NULL) {
        *minute = _minute;
    }
    if (second != NULL) {
        *second = _second;
    }
    return result;
}

/*public */IRXTime& IRXTime::setTime(int minute, int second)
{
    from(_year, _month, _day, 0, minute, second, false);
    return *this;
}

/*public */int IRXTime::getTime(int* minute, int* second) const
{
    int result(0);
    
    result = 3600 * _hour + 60 * _minute + _second;
    if (minute != NULL) {
        *minute = 60 * _hour + _minute;
    }
    if (second != NULL) {
        *second = _second;
    }
    return result;
}

/*public */IRXTime& IRXTime::setTime(int second)
{
    from(_year, _month, _day, 0, 0, second, false);
    return *this;
}

/*public */int IRXTime::getTime(int* second) const
{
    int result(0);
    
    result = 3600 * _hour + 60 * _minute + _second;
    if (second != NULL) {
        *second = result;
    }
    return result;
}

/*public */IRXTime& IRXTime::setYear(int param)
{
    from(param, _month, _day, _hour, _minute, _second, true);
    return *this;
}

/*public */IRXTime& IRXTime::setMonth(int param)
{
    from(_year, param, _day, _hour, _minute, _second, true);
    return *this;
}

/*public */IRXTime& IRXTime::setDay(int param)
{
    from(_year, _month, param, _hour, _minute, _second, false);
    return *this;
}

/*public */IRXTime& IRXTime::setHour(int param)
{
    from(_year, _month, _day, param, _minute, _second, false);
    return *this;
}

/*public */IRXTime& IRXTime::setMinute(int param)
{
    from(_year, _month, _day, _hour, param, _second, false);
    return *this;
}

/*public */IRXTime& IRXTime::setSecond(int param)
{
    from(_year, _month, _day, _hour, _minute, param, false);
    return *this;
}

/*public */IRXTime& IRXTime::add(IRXTimeDiff const& param)
{
    long day;
    long limit;
    long temp;
    int hour;
    int minute;
    int second;
    
    param.get(&day, &hour, &minute, &second);
    from(_year, _month, _day, _hour + hour, _minute + minute, _second + second, false);
    while (day != 0) {
        limit = day;
        if (limit < (temp = static_cast<long>(INT_MIN) - _day)) {
            limit = temp;
        }
        if (limit > (temp = static_cast<long>(INT_MAX) - _day)) {
            limit = temp;
        }
        from(_year, _month, static_cast<int>(_day + limit), _hour, _minute, _second, false);
        day -= limit;
    }
    return *this;
}

/*public */IRXTime& IRXTime::addYear(int param)
{
    long limit;
    long temp;
    
    while (param != 0) {
        limit = param;
        if (limit < (temp = static_cast<long>(INT_MIN) - _year)) {
            limit = temp;
        }
        if (limit > (temp = static_cast<long>(INT_MAX) - _year)) {
            limit = temp;
        }
        from(static_cast<int>(_year + limit), _month, _day, _hour, _minute, _second, true);
        param -= limit;
    }
    return *this;
}

/*public */IRXTime& IRXTime::addMonth(int param)
{
    long limit;
    long temp;
    
    while (param != 0) {
        limit = param;
        if (limit < (temp = static_cast<long>(INT_MIN) - _month)) {
            limit = temp;
        }
        if (limit > (temp = static_cast<long>(INT_MAX) - _month)) {
            limit = temp;
        }
        from(_year, static_cast<int>(_month + limit), _day, _hour, _minute, _second, true);
        param -= limit;
    }
    return *this;
}

/*public */IRXTime& IRXTime::addDay(int param)
{
    long limit;
    long temp;
    
    while (param != 0) {
        limit = param;
        if (limit < (temp = static_cast<long>(INT_MIN) - _day)) {
            limit = temp;
        }
        if (limit > (temp = static_cast<long>(INT_MAX) - _day)) {
            limit = temp;
        }
        from(_year, _month, static_cast<int>(_day + limit), _hour, _minute, _second, false);
        param -= limit;
    }
    return *this;
}

/*public */IRXTime& IRXTime::addHour(int param)
{
    long limit;
    long temp;
    
    while (param != 0) {
        limit = param;
        if (limit < (temp = static_cast<long>(INT_MIN) - _hour)) {
            limit = temp;
        }
        if (limit > (temp = static_cast<long>(INT_MAX) - _hour)) {
            limit = temp;
        }
        from(_year, _month, _day, static_cast<int>(_hour + limit), _minute, _second, false);
        param -= limit;
    }
    return *this;
}

/*public */IRXTime& IRXTime::addMinute(int param)
{
    long limit;
    long temp;
    
    while (param != 0) {
        limit = param;
        if (limit < (temp = static_cast<long>(INT_MIN) - _minute)) {
            limit = temp;
        }
        if (limit > (temp = static_cast<long>(INT_MAX) - _minute)) {
            limit = temp;
        }
        from(_year, _month, _day, _hour, static_cast<int>(_minute + limit), _second, false);
        param -= limit;
    }
    return *this;
}

/*public */IRXTime& IRXTime::addSecond(int param)
{
    long limit;
    long temp;
    
    while (param != 0) {
        limit = param;
        if (limit < (temp = static_cast<long>(INT_MIN) - _second)) {
            limit = temp;
        }
        if (limit > (temp = static_cast<long>(INT_MAX) - _second)) {
            limit = temp;
        }
        from(_year, _month, _day, _hour, _minute, static_cast<int>(_second + limit), false);
        param -= limit;
    }
    return *this;
}

/*public */IRXTime& IRXTime::sub(IRXTimeDiff const& param)
{
    long day;
    long limit;
    long temp;
    int hour;
    int minute;
    int second;
    
    param.get(&day, &hour, &minute, &second);
    from(_year, _month, _day, _hour - hour, _minute - minute, _second - second, false);
    while (day != 0) {
        limit = day;
        if (limit < (temp = _day - static_cast<long>(INT_MAX))) {
            limit = temp;
        }
        if (limit > (temp = _day - static_cast<long>(INT_MIN))) {
            limit = temp;
        }
        from(_year, _month, static_cast<int>(_day - limit), _hour, _minute, _second, false);
        day -= limit;
    }
    return *this;
}

/*public */IRXTime& IRXTime::subYear(int param)
{
    long limit;
    long temp;
    
    while (param != 0) {
        limit = param;
        if (limit < (temp = _year - static_cast<long>(INT_MAX))) {
            limit = temp;
        }
        if (limit > (temp = _year - static_cast<long>(INT_MIN))) {
            limit = temp;
        }
        from(static_cast<int>(_year - limit), _month, _day, _hour, _minute, _second, true);
        param -= limit;
    }
    return *this;
}

/*public */IRXTime& IRXTime::subMonth(int param)
{
    long limit;
    long temp;
    
    while (param != 0) {
        limit = param;
        if (limit < (temp = _month - static_cast<long>(INT_MAX))) {
            limit = temp;
        }
        if (limit > (temp = _month - static_cast<long>(INT_MIN))) {
            limit = temp;
        }
        from(_year, static_cast<int>(_month - limit), _day, _hour, _minute, _second, true);
        param -= limit;
    }
    return *this;
}

/*public */IRXTime& IRXTime::subDay(int param)
{
    long limit;
    long temp;
    
    while (param != 0) {
        limit = param;
        if (limit < (temp = _day - static_cast<long>(INT_MAX))) {
            limit = temp;
        }
        if (limit > (temp = _day - static_cast<long>(INT_MIN))) {
            limit = temp;
        }
        from(_year, _month, static_cast<int>(_day - limit), _hour, _minute, _second, false);
        param -= limit;
    }
    return *this;
}

/*public */IRXTime& IRXTime::subHour(int param)
{
    long limit;
    long temp;
    
    while (param != 0) {
        limit = param;
        if (limit < (temp = _hour - static_cast<long>(INT_MAX))) {
            limit = temp;
        }
        if (limit > (temp = _hour - static_cast<long>(INT_MIN))) {
            limit = temp;
        }
        from(_year, _month, _day, static_cast<int>(_hour - limit), _minute, _second, false);
        param -= limit;
    }
    return *this;
}

/*public */IRXTime& IRXTime::subMinute(int param)
{
    long limit;
    long temp;
    
    while (param != 0) {
        limit = param;
        if (limit < (temp = _minute - static_cast<long>(INT_MAX))) {
            limit = temp;
        }
        if (limit > (temp = _minute - static_cast<long>(INT_MIN))) {
            limit = temp;
        }
        from(_year, _month, _day, _hour, static_cast<int>(_minute - limit), _second, false);
        param -= limit;
    }
    return *this;
}

/*public */IRXTime& IRXTime::subSecond(int param)
{
    long limit;
    long temp;
    
    while (param != 0) {
        limit = param;
        if (limit < (temp = _second - static_cast<long>(INT_MAX))) {
            limit = temp;
        }
        if (limit > (temp = _second - static_cast<long>(INT_MIN))) {
            limit = temp;
        }
        from(_year, _month, _day, _hour, _minute, static_cast<int>(_second - limit), false);
        param -= limit;
    }
    return *this;
}

/*public */bool IRXTime::equals(IRXTime const& param) const
{
    return (_year == param._year &&
            _month == param._month &&
            _day == param._day &&
            _hour == param._hour &&
            _minute == param._minute &&
            _second == param._second);
}

/*public */bool IRXTime::equalsDate(IRXTime const& param) const
{
    return (_year == param._year &&
            _month == param._month &&
            _day == param._day);
}

/*public */bool IRXTime::equalsTime(IRXTime const& param) const
{
    return (_hour == param._hour &&
            _minute == param._minute &&
            _second == param._second);
}

/*public */int IRXTime::compare(IRXTime const& param) const
{
    int result(0);
    
    if (_year > param._year) {
        result = +1;
    }
    else if (_year < param._year) {
        result = -1;
    }
    else if (_month > param._month) {
        result = +1;
    }
    else if (_month < param._month) {
        result = -1;
    }
    else if (_day > param._day) {
        result = +1;
    }
    else if (_day < param._day) {
        result = -1;
    }
    else if (_hour > param._hour) {
        result = +1;
    }
    else if (_hour < param._hour) {
        result = -1;
    }
    else if (_minute > param._minute) {
        result = +1;
    }
    else if (_minute < param._minute) {
        result = -1;
    }
    else if (_second > param._second) {
        result = +1;
    }
    else if (_second < param._second) {
        result = -1;
    }
    return result;
}

/*public */int IRXTime::compareDate(IRXTime const& param) const
{
    int result(0);
    
    if (_year > param._year) {
        result = +1;
    }
    else if (_year < param._year) {
        result = -1;
    }
    else if (_month > param._month) {
        result = +1;
    }
    else if (_month < param._month) {
        result = -1;
    }
    else if (_day > param._day) {
        result = +1;
    }
    else if (_day < param._day) {
        result = -1;
    }
    return result;
}

/*public */int IRXTime::compareTime(IRXTime const& param) const
{
    int result(0);
    
    if (_hour > param._hour) {
        result = +1;
    }
    else if (_hour < param._hour) {
        result = -1;
    }
    else if (_minute > param._minute) {
        result = +1;
    }
    else if (_minute < param._minute) {
        result = -1;
    }
    else if (_second > param._second) {
        result = +1;
    }
    else if (_second < param._second) {
        result = -1;
    }
    return result;
}

/*public */IRXTimeDiff IRXTime::difference(IRXTime const& param) const
{
    return IRXTimeDiff(difftime(to(), param.to()));
}

/*public */IRXTime::ErrorEnum IRXTime::parse(std::string const& format, std::string const& string)
{
    IRXTime initial;
    int year;
    int month;
    int day;
    int hour;
    int minute;
    int second;
    int days;
    time_t posix;
    bool isampm;
    bool ispm;
    bool isdays;
    bool isposix;
    char fkey;
    char skey;
    int value;
    int i;
    int j;
    int k;
    int x;
    ErrorEnum error(ERROR_OK);
    
    year = initial.getYear();
    month = initial.getMonth();
    day = initial.getDay();
    hour = initial.getHour();
    minute = initial.getMinute();
    second = initial.getSecond();
    isampm = false;
    ispm = false;
    isdays = false;
    isposix = false;
    for (i = 0, x = 0; i < format.size() && x < string.size() && error == ERROR_OK; ) {
        switch (search(format, &i, &fkey)) {
            case SEARCH_PERCENT:
                if (i < format.size()) {
                    switch (search(format, &i, &fkey, &j)) {
                        case SEARCH_PERCENT:
                            step(string, &x, &skey);
                            if (fkey != skey) {
                                error = ERROR_INVALID_FORMAT;
                            }
                            break;
                        case SEARCH_MONTH:
                            for (k = 1; k < lengthof(irxtime_month[j]); ++k) {
                                if (compare(string, &x, irxtime_month[j][k])) {
                                    month = k;
                                    isdays = false;
                                    isposix = false;
                                    break;
                                }
                            }
                            if (k >= lengthof(irxtime_month[j])) {
                                error = ERROR_INVALID_FORMAT;
                            }
                            break;
                        case SEARCH_MTH:
                            for (k = 1; k < lengthof(irxtime_mth[j]); ++k) {
                                if (compare(string, &x, irxtime_mth[j][k])) {
                                    month = k;
                                    isdays = false;
                                    isposix = false;
                                    break;
                                }
                            }
                            if (k >= lengthof(irxtime_mth[j])) {
                                error = ERROR_INVALID_FORMAT;
                            }
                            break;
                        case SEARCH_WEEK:
                            for (k = 1; k < lengthof(irxtime_week[j]); ++k) {
                                if (compare(string, &x, irxtime_week[j][k])) {
                                    break;
                                }
                            }
                            if (k >= lengthof(irxtime_week[j])) {
                                error = ERROR_INVALID_FORMAT;
                            }
                            break;
                        case SEARCH_WEK:
                            for (k = 1; k < lengthof(irxtime_wek[j]); ++k) {
                                if (compare(string, &x, irxtime_wek[j][k])) {
                                    break;
                                }
                            }
                            if (k >= lengthof(irxtime_wek[j])) {
                                error = ERROR_INVALID_FORMAT;
                            }
                            break;
                        case SEARCH_YOUBI:
                            for (k = 1; k < lengthof(irxtime_youbi[j]); ++k) {
                                if (compare(string, &x, irxtime_youbi[j][k])) {
                                    break;
                                }
                            }
                            if (k >= lengthof(irxtime_youbi[j])) {
                                error = ERROR_INVALID_FORMAT;
                            }
                            break;
                        case SEARCH_AMPM:
                            for (k = 1; k < lengthof(irxtime_ampm[j]); ++k) {
                                if (compare(string, &x, irxtime_ampm[j][k])) {
                                    ispm = (k == 2);
                                    break;
                                }
                            }
                            if (k >= lengthof(irxtime_ampm[j])) {
                                error = ERROR_INVALID_FORMAT;
                            }
                            break;
                        case SEARCH_GOZENGOGO:
                            for (k = 1; k < lengthof(irxtime_gozengogo[j]); ++k) {
                                if (compare(string, &x, irxtime_gozengogo[j][k])) {
                                    ispm = (k == 2);
                                    break;
                                }
                            }
                            if (k >= lengthof(irxtime_gozengogo[j])) {
                                error = ERROR_INVALID_FORMAT;
                            }
                            break;
                        default:
                            switch (fkey) {
                                case ' ':
                                    // nop
                                    break;
                                case '*':
                                    for (k = 0; k < j && x < string.size(); ++k, ++x) {
                                        // nop
                                    }
                                    if (k < j) {
                                        error = ERROR_INVALID_FORMAT;
                                    }
                                    break;
                                case 'Y':
                                case 'M':
                                case 'D':
                                case 'H':
                                case 'h':
                                case 'm':
                                case 's':
                                    if (j > 1) {
                                        for (k = 0; k < j && x < string.size(); ++k, ++x) {
                                            if (!isdigit(string[x])) {
                                                break;
                                            }
                                        }
                                        if (k >= j) {
                                            if (j >= 4) {
                                                k = 4;
                                            }
                                            else if (j >= 3) {
                                                k = 3;
                                            }
                                            else if (j >= 2) {
                                                k = 2;
                                            }
                                            else {
                                                k = 0;
                                            }
                                        }
                                        else {
                                            error = ERROR_INVALID_FORMAT;
                                        }
                                    }
                                    else {
                                        for (j = 0; x < string.size(); ++x, ++j) {
                                            if (!isdigit(string[x])) {
                                                break;
                                            }
                                        }
                                        if (j > 0) {
                                            k = 0;
                                        }
                                        else {
                                            error = ERROR_INVALID_FORMAT;
                                        }
                                    }
                                    if (error == ERROR_OK) {
                                        if (sscanf(&string[x - j], irxtime_formatint[k], &value) > 0) {
                                            switch (fkey) {
                                                case 'Y':
                                                    switch (k) {
                                                        case 2: // %YY
                                                            value += (value < 70) ? (2000) : (1900);
                                                        case 0: // %Y
                                                        case 4: // %YYYY
                                                            year = value;
                                                            break;
                                                        default:
                                                            error = ERROR_INVALID_FORMAT;
                                                            break;
                                                    }
                                                    break;
                                                case 'M':
                                                    switch (k) {
                                                        case 0: // %M
                                                        case 2: // %MM
                                                            month = value;
                                                            isdays = false;
                                                            break;
                                                        default:
                                                            error = ERROR_INVALID_FORMAT;
                                                            break;
                                                    }
                                                    break;
                                                case 'D':
                                                    switch (k) {
                                                        case 0: // %D
                                                        case 2: // %DD
                                                            day = value;
                                                            isdays = false;
                                                            break;
                                                        case 3: // %DDD
                                                            days = value;
                                                            isdays = true;
                                                            break;
                                                        default:
                                                            error = ERROR_INVALID_FORMAT;
                                                            break;
                                                    }
                                                    break;
                                                case 'H':
                                                    switch (k) {
                                                        case 0: // %H
                                                        case 2: // %HH
                                                            hour = value;
                                                            isampm = true;
                                                            break;
                                                        default:
                                                            error = ERROR_INVALID_FORMAT;
                                                            break;
                                                    }
                                                    break;
                                                case 'h':
                                                    switch (k) {
                                                        case 0: // %h
                                                        case 2: // %hh
                                                            hour = value;
                                                            isampm = false;
                                                            break;
                                                        default:
                                                            error = ERROR_INVALID_FORMAT;
                                                            break;
                                                    }
                                                    break;
                                                case 'm':
                                                    switch (k) {
                                                        case 0: // %m
                                                        case 2: // %mm
                                                            minute = value;
                                                            break;
                                                        default:
                                                            error = ERROR_INVALID_FORMAT;
                                                            break;
                                                    }
                                                    break;
                                                case 's':
                                                    switch (k) {
                                                        case 0: // %s
                                                        case 2: // %ss
                                                            second = value;
                                                            break;
                                                        default:
                                                            error = ERROR_INVALID_FORMAT;
                                                            break;
                                                    }
                                                    break;
                                                default:
                                                    break;
                                            }
                                            if (error == ERROR_OK) {
                                                isposix = false;
                                            }
                                        }
                                        else {
                                            error = ERROR_INVALID_FORMAT;
                                        }
                                    }
                                    break;
                                case 'p':
                                    if (j <= 1) {
                                        for (j = 0; x < string.size(); ++x, ++j) {
                                            if (!isdigit(string[x])) {
                                                break;
                                            }
                                        }
                                        if (j > 0) {
                                            if (sscanf(&string[x - j], irxtime_formatlong[0], &posix) > 0) {
                                                isposix = true;
                                            }
                                            else {
                                                error = ERROR_INVALID_FORMAT;
                                            }
                                        }
                                        else {
                                            error = ERROR_INVALID_FORMAT;
                                        }
                                    }
                                    else {
                                        error = ERROR_INVALID_FORMAT;
                                    }
                                    break;
                                default:
                                    break;
                            }
                            break;
                    }
                }
                break;
            default:
                step(string, &x, &skey);
                if (isspace(fkey) && isspace(skey)) {
                    for (j = 0; i < format.size(); ++i, ++j) {
                        if (!isspace(format[i])) {
                            break;
                        }
                    }
                    for (; x < string.size(); ++x, --j) {
                        if (!isspace(string[x])) {
                            break;
                        }
                    }
                    if (j > 0) {
                        error = ERROR_INVALID_FORMAT;
                    }
                }
                else if (fkey != skey) {
                    error = ERROR_INVALID_FORMAT;
                }
                break;
        }
    }
    if (error == ERROR_OK) {
        if (i >= format.size()) {
            if (!isposix) {
                if (isdays) {
                    month = 1;
                    day = days;
                }
                if (isampm) {
                    if (0 <= hour && hour <= 12) {
                        hour %= 12;
                        if (ispm) {
                            hour += 12;
                        }
                    }
                    else {
                        error = ERROR_INVALID_FORMAT;
                    }
                }
                if (error == ERROR_OK) {
                    from(year, month, day, hour, minute, second, false);
                }
            }
            else {
                from(posix, true);
            }
        }
        else {
            error = ERROR_INVALID_FORMAT;
        }
    }
    return error;
}

/*public */IRXTime::ErrorEnum IRXTime::parseYMD(std::string const& string, char delimiter)
{
    return parse(FORMAT_YMD(delimiter), string);
}

/*public */IRXTime::ErrorEnum IRXTime::parseMDY(std::string const& string, char delimiter)
{
    return parse(FORMAT_MDY(delimiter), string);
}

/*public */IRXTime::ErrorEnum IRXTime::parseDMY(std::string const& string, char delimiter)
{
    return parse(FORMAT_DMY(delimiter), string);
}

/*public */IRXTime::ErrorEnum IRXTime::parseISO8601(std::string const& string, bool extended)
{
    return parse((extended) ? (FORMAT_ISO8601_EXTENDED) : (FORMAT_ISO8601_BASIC), string);
}

/*public */std::string IRXTime::format(std::string const& format) const
{
    char temp[64];
    char fkey;
    bool valid;
    int value;
    int i;
    int j;
    std::string result;
    
    for (i = 0; i < format.size(); ) {
        switch (search(format, &i, &fkey)) {
            case SEARCH_PERCENT:
                if (i < format.size()) {
                    switch (search(format, &i, &fkey, &j)) {
                        case SEARCH_PERCENT:
                            result.append(1, fkey);
                            break;
                        case SEARCH_MONTH:
                            result.append(irxtime_month[j][_month]);
                            break;
                        case SEARCH_MTH:
                            result.append(irxtime_mth[j][_month]);
                            break;
                        case SEARCH_WEEK:
                            result.append(irxtime_week[j][_week + 1]);
                            break;
                        case SEARCH_WEK:
                            result.append(irxtime_wek[j][_week + 1]);
                            break;
                        case SEARCH_YOUBI:
                            result.append(irxtime_youbi[j][_week + 1]);
                            break;
                        case SEARCH_AMPM:
                            result.append(irxtime_ampm[j][(_hour < 12) ? (1) : (2)]);
                            break;
                        case SEARCH_GOZENGOGO:
                            result.append(irxtime_gozengogo[j][(_hour < 12) ? (1) : (2)]);
                            break;
                        default:
                            switch (fkey) {
                                case ' ':
                                    // nop
                                    break;
                                case '*':
                                    result.append(j, fkey);
                                    break;
                                case 'Y':
                                case 'M':
                                case 'D':
                                case 'H':
                                case 'h':
                                case 'm':
                                case 's':
                                    valid = true;
                                    if (j >= 4) {
                                        j = 4;
                                    }
                                    else if (j >= 3) {
                                        j = 3;
                                    }
                                    else if (j >= 2) {
                                        j = 2;
                                    }
                                    else {
                                        j = 0;
                                    }
                                    value = 0;
                                    switch (fkey) {
                                        case 'Y':
                                            switch (j) {
                                                case 0: // %Y
                                                case 2: // %YY
                                                case 4: // %YYYY
                                                    value = _year;
                                                    break;
                                                default:
                                                    valid = false;
                                                    break;
                                            }
                                            break;
                                        case 'M':
                                            switch (j) {
                                                case 0: // %M
                                                case 2: // %MM
                                                    value = _month;
                                                    break;
                                                default:
                                                    valid = false;
                                                    break;
                                            }
                                            break;
                                        case 'D':
                                            switch (j) {
                                                case 0: // %D
                                                case 2: // %DD
                                                    value = _day;
                                                    break;
                                                case 3: // %DDD
                                                    value = _days;
                                                    break;
                                                default:
                                                    valid = false;
                                                    break;
                                            }
                                            break;
                                        case 'H':
                                            switch (j) {
                                                case 0: // %H
                                                case 2: // %HH
                                                    if ((value = _hour % 12) == 0) {
                                                        value = 12;
                                                    }
                                                    break;
                                                default:
                                                    valid = false;
                                                    break;
                                            }
                                            break;
                                        case 'h':
                                            switch (j) {
                                                case 0: // %h
                                                case 2: // %hh
                                                    value = _hour;
                                                    break;
                                                default:
                                                    valid = false;
                                                    break;
                                            }
                                            break;
                                        case 'm':
                                            switch (j) {
                                                case 0: // %m
                                                case 2: // %mm
                                                    value = _minute;
                                                    break;
                                                default:
                                                    valid = false;
                                                    break;
                                            }
                                            break;
                                        case 's':
                                            switch (j) {
                                                case 0: // %s
                                                case 2: // %ss
                                                    value = _second;
                                                    break;
                                                default:
                                                    valid = false;
                                                    break;
                                            }
                                            break;
                                        default:
                                            break;
                                    }
                                    if (valid) {
                                        if (j >= 4) {
                                            value %= 10000;
                                        }
                                        else if (j >= 3) {
                                            value %= 1000;
                                        }
                                        else if (j >= 2) {
                                            value %= 100;
                                        }
                                        else if (j >= 1) {
                                            value %= 10;
                                        }
                                        snprintf(temp, sizeof(temp), irxtime_formatint[j], value);
                                        result.append(temp);
                                    }
                                    break;
                                case 'p':
                                    if (j <= 1) {
                                        snprintf(temp, sizeof(temp), irxtime_formatlong[0], to());
                                        result.append(temp);
                                    }
                                    break;
                                default:
                                    break;
                            }
                            break;
                    }
                }
                break;
            default:
                result.append(1, fkey);
                break;
        }
    }
    return result;
}

/*public */std::string IRXTime::formatYMD(char delimiter) const
{
    return format(FORMAT_YMD(delimiter));
}

/*public */std::string IRXTime::formatMDY(char delimiter) const
{
    return format(FORMAT_MDY(delimiter));
}

/*public */std::string IRXTime::formatDMY(char delimiter) const
{
    return format(FORMAT_DMY(delimiter));
}

/*public */std::string IRXTime::formatISO8601(bool extended) const
{
    return format((extended) ? (FORMAT_ISO8601_EXTENDED) : (FORMAT_ISO8601_BASIC));
}

/*public static */IRXTime IRXTime::epochTime(void)
{
    IRXTime result;
    
    result.from(0, true);
    return result;
}

/*public static */IRXTime IRXTime::epochUTCTime(void)
{
    IRXTime result;
    
    result.from(0, false);
    return result;
}

/*public static */IRXTime IRXTime::currentTime(void)
{
    time_t utc;
    IRXTime result;
    
    if ((utc = time(NULL)) != -1) {
        result.from(utc, true);
    }
    return result;
}

/*public static */IRXTime IRXTime::currentUTCTime(void)
{
    time_t utc;
    IRXTime result;
    
    if ((utc = time(NULL)) != -1) {
        result.from(utc, false);
    }
    return result;
}

/*private */IRXTime::ErrorEnum IRXTime::from(int year, int month, int day, int hour, int minute, int second, bool truncate)
{
    tm temp = {
        0
    };
    ErrorEnum error(ERROR_OK);
    
    temp.tm_year = year - 1900;
    temp.tm_mon = month - 1;
    temp.tm_mday = day;
    temp.tm_hour = hour;
    temp.tm_min = minute;
    temp.tm_sec = second;
    if ((error = from(mktime(&temp), true)) == ERROR_OK) {
        if (truncate) {
            if (_day != day) {
                error = from(year, month, day - _day, hour, minute, second, false);
            }
        }
    }
    return error;
}

/*private */IRXTime::ErrorEnum IRXTime::from(time_t utc, bool local)
{
    tm const* addr;
    ErrorEnum error(ERROR_OK);
    
    if ((addr = (local) ? (localtime(&utc)) : (gmtime(&utc))) == NULL) {
        error = ERROR_FAILED;
        std::cerr << "IRXTime conversion error at IRXTime::from()" << std::endl;
        utc = 0;
        if ((addr = gmtime(&utc)) == NULL) {
            std::cerr << "IRXTime internal fault at IRXTime::from()" << std::endl;
        }
    }
    if (addr != NULL) {
        _year = addr->tm_year + 1900;
        _month = addr->tm_mon + 1;
        _day = addr->tm_mday;
        _hour = addr->tm_hour;
        _minute = addr->tm_min;
        _second = addr->tm_sec;
        _days = addr->tm_yday + 1;
        _week = static_cast<DayOfWeekEnum>(addr->tm_wday);
    }
    else {
        _year = 1970;
        _month = 1;
        _day = 1;
        _hour = 0;
        _minute = 0;
        _second = 0;
        _days = 1;
        _week = DAYOFWEEK_THURSDAY;
    }
    return error;
}

/*private */time_t IRXTime::to(void) const
{
    tm temp = {
        0
    };
    
    temp.tm_year = _year - 1900;
    temp.tm_mon = _month - 1;
    temp.tm_mday = _day;
    temp.tm_hour = _hour;
    temp.tm_min = _minute;
    temp.tm_sec = _second;
    return mktime(&temp);
}

/*private static */IRXTime::SearchEnum IRXTime::search(std::string const& string, int* index, char* key)
{
    SearchEnum result(SEARCH_LIMIT);
    
    *key = string[*index];
    ++*index;
    switch (*key) {
        case '%':
            result = SEARCH_PERCENT;
            break;
        default:
            break;
    }
    return result;
}

/*private static */IRXTime::SearchEnum IRXTime::search(std::string const& string, int* index, char* key, int* hint)
{
    int i;
    SearchEnum result(SEARCH_LIMIT);
    
    *key = string[*index];
    switch (*key) {
        case '%':
            ++*index;
            result = SEARCH_PERCENT;
            break;
        case 'M': // MONTH, Month, MTH, Mth
        case 'm': // month, mth
            for (i = 0; i < lengthof(irxtime_month); ++i) {
                if (compare(string, index, irxtime_month[i][0])) {
                    *hint = i;
                    result = SEARCH_MONTH;
                    break;
                }
            }
            if (i >= lengthof(irxtime_month)) {
                for (i = 0; i < lengthof(irxtime_mth); ++i) {
                    if (compare(string, index, irxtime_mth[i][0])) {
                        *hint = i;
                        result = SEARCH_MTH;
                        break;
                    }
                }
            }
            break;
        case 'W': // WEEK, Week, WEK, Wek
        case 'w': // week, wek
            for (i = 0; i < lengthof(irxtime_week); ++i) {
                if (compare(string, index, irxtime_week[i][0])) {
                    *hint = i;
                    result = SEARCH_WEEK;
                    break;
                }
            }
            if (i >= lengthof(irxtime_week)) {
                for (i = 0; i < lengthof(irxtime_wek); ++i) {
                    if (compare(string, index, irxtime_wek[i][0])) {
                        *hint = i;
                        result = SEARCH_WEK;
                        break;
                    }
                }
            }
            break;
        case 'J': // JP, JG
            for (i = 0; i < lengthof(irxtime_youbi); ++i) {
                if (compare(string, index, irxtime_youbi[i][0])) {
                    *hint = i;
                    result = SEARCH_YOUBI;
                    break;
                }
            }
            if (i >= lengthof(irxtime_youbi)) {
                for (i = 0; i < lengthof(irxtime_gozengogo); ++i) {
                    if (compare(string, index, irxtime_gozengogo[i][0])) {
                        *hint = i;
                        result = SEARCH_GOZENGOGO;
                        break;
                    }
                }
            }
            break;
        case 'A': // AN, AD
        case 'a': // an, ad
            for (i = 0; i < lengthof(irxtime_ampm); ++i) {
                if (compare(string, index, irxtime_ampm[i][0])) {
                    *hint = i;
                    result = SEARCH_AMPM;
                    break;
                }
            }
            break;
        default:
            break;
    }
    if (result == SEARCH_LIMIT) {
        for (i = 0; *index < string.size(); ++*index, ++i) {
            if (string[*index] != *key) {
                break;
            }
        }
        switch (*key) {
            case '*':
            case 'Y':
            case 'M':
            case 'D':
            case 'H':
            case 'h':
            case 'm':
            case 's':
            case 'p':
                // nop
                break;
            default:
                if (i >= 1) {
                    i = 1;
                }
                break;
        }
        *hint = i;
    }
    return result;
}

/*private static */void IRXTime::step(std::string const& string, int* index, char* key)
{
    *key = string[*index];
    ++*index;
    return;
}

/*private static */bool IRXTime::compare(std::string const& string, int* index, char const* compare)
{
    int length;
    bool result(false);
    
    length = static_cast<int>(strlen(compare));
    if (string.compare(*index, length, compare) == 0) {
        *index += length;
        result = true;
    }
    return result;
}

}// end of namespace
