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
**      Xcode 5.1.1 (Apple LLVM 5.1)
**
**      TGSPhysicsDatabase.cpp
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

#include "TGSPhysicsDatabase.h"
#include <map>
using namespace std;
#include "cTLE.h"

namespace tgs {

#define TABLE_PHYSICS       ("_physics_")
#define TIME_FORMAT         ("%YYYY-%MM-%DD %hh:%mm:%ss")

static  char const*         g_table[][2] = {
    {"norad",               "INTEGER NOT NULL PRIMARY KEY"},
    {"name",                "TEXT"},
    {"callsign",            "TEXT"},
    {"beacon_mode",         "TEXT"},
    {"beacon_frequency",    "INTEGER"},
    {"beacon_drift",        "INTEGER"},
    {"sender_mode",         "TEXT"},
    {"sender_frequency",    "INTEGER"},
    {"sender_drift",        "INTEGER"},
    {"receiver_mode",       "TEXT"},
    {"receiver_frequency",  "INTEGER"},
    {"receiver_drift",      "INTEGER"},
    {"time",                "TEXT NOT NULL CHECK(time like '____-__-__ __:__:__') DEFAULT '0000-00-00 00:00:00'"},
    {"epoch",               "TEXT NOT NULL CHECK(epoch like '_______.________') DEFAULT '0000000.00000000'"},
    {"tle_one",             "TEXT"},
    {"tle_two",             "TEXT"},
    {"\"update\"",          "INTEGER NOT NULL DEFAULT 0"}
};
static  char const*         g_name[][2] = {
    {"norad",               "%d"},
    {"name",                "%Q"},
    {"\"update\"",          "%d"}
};
static  char const*         g_callsign[][2] = {
    {"norad",               "%d"},
    {"callsign",            "%Q"},
    {"\"update\"",          "%d"}
};
static  char const*         g_radioBeacon[][2] = {
    {"norad",               "%d"},
    {"beacon_mode",         "%Q"},
    {"beacon_frequency",    "%d"},
    {"beacon_drift",        "%d"},
    {"\"update\"",          "%d"}
};
static  char const*         g_radioSender[][2] = {
    {"norad",               "%d"},
    {"sender_mode",         "%Q"},
    {"sender_frequency",    "%d"},
    {"sender_drift",        "%d"},
    {"\"update\"",          "%d"}
};
static  char const*         g_radioReceiver[][2] = {
    {"norad",               "%d"},
    {"receiver_mode",       "%Q"},
    {"receiver_frequency",  "%d"},
    {"receiver_drift",      "%d"},
    {"\"update\"",          "%d"}
};
static  char const*         g_setOrbitData[][2] = {
    {"norad",               "%s"},
    {"name",                "%Q"},
    {"epoch",               "%s"},
    {"tle_one",             "%Q"},
    {"tle_two",             "%Q"},
    {"\"update\"",          "%d"}
};
static  char const*         g_setOrbitTime[][2] = {
    {"norad",               "%s"},
    {"time",                "%Q"}
};
static  char const*         g_getOrbitData[][2] = {
    {"norad",               "%d"},
    {"name",                "%Q"},
    {"tle_one",             "%Q"},
    {"tle_two",             "%Q"},
    {"time",                "%Q"}
};
static  char const*         g_field[][2] = {
    {"norad",               "%d"},
    {"name",                "%Q"},
    {"callsign",            "%Q"},
    {"beacon_mode",         "%Q"},
    {"beacon_frequency",    "%d"},
    {"beacon_drift",        "%d"},
    {"sender_mode",         "%Q"},
    {"sender_frequency",    "%d"},
    {"sender_drift",        "%d"},
    {"receiver_mode",       "%Q"},
    {"receiver_frequency",  "%d"},
    {"receiver_drift",      "%d"},
    {"time",                "%Q"},
    {"tle_one",             "%Q"},
    {"tle_two",             "%Q"}
};
static  char const*         g_hasUpdate[][2] = {
    {"norad",               "%d"},
    {"\"update\"",          "%d"}
};

/*public */TGSPhysicsDatabase::TGSPhysicsDatabase(void)
{
}

/*public virtual */TGSPhysicsDatabase::~TGSPhysicsDatabase(void)
{
    close();
}

/*public */TGSError TGSPhysicsDatabase::setName(int norad, std::string const& param)
{
    return setText(norad, g_name, param);
}

/*public */TGSError TGSPhysicsDatabase::getName(int norad, std::string* result)
{
    return getText(norad, g_name, result);
}

/*public */TGSError TGSPhysicsDatabase::setCallsign(int norad, std::string const& param)
{
    return setText(norad, g_callsign, param);
}

/*public */TGSError TGSPhysicsDatabase::getCallsign(int norad, std::string* result)
{
    return getText(norad, g_callsign, result);
}

/*public */TGSError TGSPhysicsDatabase::setRadioBeacon(int norad, RadioRec const& param)
{
    return setRadio(norad, g_radioBeacon, param);
}

/*public */TGSError TGSPhysicsDatabase::getRadioBeacon(int norad, RadioRec* result)
{
    return getRadio(norad, g_radioBeacon, result);
}

/*public */TGSError TGSPhysicsDatabase::setRadioSender(int norad, RadioRec const& param)
{
    return setRadio(norad, g_radioSender, param);
}

/*public */TGSError TGSPhysicsDatabase::getRadioSender(int norad, RadioRec* result)
{
    return getRadio(norad, g_radioSender, result);
}

/*public */TGSError TGSPhysicsDatabase::setRadioReceiver(int norad, RadioRec const& param)
{
    return setRadio(norad, g_radioReceiver, param);
}

/*public */TGSError TGSPhysicsDatabase::getRadioReceiver(int norad, RadioRec* result)
{
    return getRadio(norad, g_radioReceiver, result);
}

/*public */TGSError TGSPhysicsDatabase::setOrbitData(TLERec const& param, ir::IRXTime const& time)
{
    std::string name;
    std::string one;
    std::string two;
    std::string epoch;
    std::string norad;
    ir::IRXTime date;
    int i;
    TGSError error(TGSERROR_OK);
    
    if ((error = checkFlow()) == TGSERROR_OK) {
        name = param.name;
        if (Zeptomoby::OrbitTools::cTle::IsValidLine(name, Zeptomoby::OrbitTools::cTle::LINE_ZERO)) {
            one = param.one;
            if (Zeptomoby::OrbitTools::cTle::IsValidLine(one, Zeptomoby::OrbitTools::cTle::LINE_ONE)) {
                two = param.two;
                if (Zeptomoby::OrbitTools::cTle::IsValidLine(two, Zeptomoby::OrbitTools::cTle::LINE_TWO)) {
                    Zeptomoby::OrbitTools::cTle tle(name, one, two);
                    tle.GetField(Zeptomoby::OrbitTools::cTle::FLD_EPOCHYEAR, Zeptomoby::OrbitTools::cTle::U_NATIVE, &epoch);
                    for (i = 0; i < epoch.size(); ++i) {
                        if (epoch[i] == ' ') {
                            epoch[i] = '0';
                        }
                    }
                    if ((error = date.parse("%YY", epoch)) == TGSERROR_OK) {
                        tle.GetField(Zeptomoby::OrbitTools::cTle::FLD_EPOCHDAY, Zeptomoby::OrbitTools::cTle::U_NATIVE, &epoch);
                        for (i = 0; i < epoch.size(); ++i) {
                            if (epoch[i] == ' ') {
                                epoch[i] = '0';
                            }
                        }
                        epoch = date.format("%YYYY") + epoch;
                        if ((error = print("%Q", &epoch, epoch.c_str())) == TGSERROR_OK) {
                            tle.GetField(Zeptomoby::OrbitTools::cTle::FLD_NORADNUM, Zeptomoby::OrbitTools::cTle::U_NATIVE, &norad);
                            if ((error = update(TABLE_PHYSICS, g_setOrbitData, lengthof(g_setOrbitData), 0, "AND epoch<" + epoch, norad.c_str(), name.c_str(), epoch.c_str(), one.c_str(), two.c_str(), 1)) == TGSERROR_OK) {
                                error = update(TABLE_PHYSICS, g_setOrbitTime, lengthof(g_setOrbitTime), 0, std::string(""), norad.c_str(), time.format(TIME_FORMAT).c_str());
                            }
                        }
                    }
                }
                else {
                    error = TGSERROR_INVALID_FORMAT;
                }
            }
            else {
                error = TGSERROR_INVALID_FORMAT;
            }
        }
        else {
            error = TGSERROR_INVALID_FORMAT;
        }
    }
    return error;
}

/*public */TGSError TGSPhysicsDatabase::getOrbitData(int norad, TLERec* result, ir::IRXTime* time)
{
    std::string string;
    std::string name;
    std::string one;
    std::string two;
    ir::IRXTime date;
    TGSError error(TGSERROR_OK);
    
    if (result != NULL) {
        if ((error = checkFlow()) == TGSERROR_OK) {
            if ((error = buildQuery("", &g_getOrbitData[1], 3, &string)) == TGSERROR_OK) {
                if (time != NULL) {
                    error = buildQuery(string, &g_getOrbitData[4], 1, &string);
                }
                if (error == TGSERROR_OK) {
                    if ((error = select(TABLE_PHYSICS, string, &g_getOrbitData[0], std::string(""), norad)) == TGSERROR_OK) {
                        if ((error = step()) == TGSERROR_WAIT_RESULT) {
                            if ((error = readText(0, &name)) == TGSERROR_OK) {
                                if ((error = readText(1, &one)) == TGSERROR_OK) {
                                    if ((error = readText(2, &two)) == TGSERROR_OK) {
                                        if (time != NULL) {
                                            if ((error = readText(3, &string)) == TGSERROR_OK) {
                                                if ((error = date.parse(TIME_FORMAT, string)) == TGSERROR_OK) {
                                                    *time = date;
                                                }
                                            }
                                        }
                                        if (error == TGSERROR_OK) {
                                            error = convert(name, one, two, result);
                                        }
                                    }
                                }
                            }
                            while (step() == TGSERROR_WAIT_RESULT);
                        }
                        else if (error == TGSERROR_OK) {
                            error = TGSERROR_NO_RESULT;
                        }
                    }
                }
            }
        }
    }
    else {
        error = TGSERROR_INVALID_PARAM;
    }
    return error;
}

/*public */TGSError TGSPhysicsDatabase::getCount(int* result)
{
    return countRow(TABLE_PHYSICS, NULL, "", result);
}

/*public */TGSError TGSPhysicsDatabase::getField(int norad, FieldRec* result)
{
    std::string string;
    FieldRec field;
    TGSError error(TGSERROR_OK);
    
    if (result != NULL) {
        if ((error = checkFlow()) == TGSERROR_OK) {
            if ((error = buildQuery("", &g_field[0], 15, &string)) == TGSERROR_OK) {
                if ((error = select(TABLE_PHYSICS, string, &g_field[0], std::string(""), norad)) == TGSERROR_OK) {
                    if ((error = step()) == TGSERROR_WAIT_RESULT) {
                        if ((error = readField(0, &field)) == TGSERROR_OK) {
                            *result = field;
                        }
                        while (step() == TGSERROR_WAIT_RESULT);
                    }
                    else if (error == TGSERROR_OK) {
                        error = TGSERROR_NO_RESULT;
                    }
                }
            }
        }
    }
    else {
        error = TGSERROR_INVALID_PARAM;
    }
    return error;
}

/*public */TGSError TGSPhysicsDatabase::getField(int limit, int offset, std::vector<FieldRec>* result)
{
    std::string string;
    std::string condition;
    std::vector<FieldRec> vector;
    FieldRec field;
    TGSError error(TGSERROR_OK);
    
    if (result != NULL) {
        if ((error = checkFlow()) == TGSERROR_OK) {
            if ((error = buildQuery("", &g_field[0], 15, &string)) == TGSERROR_OK) {
                if ((error = buildOrder("", &g_field[0], true, limit, offset, &condition)) == TGSERROR_OK) {
                    if ((error = select(TABLE_PHYSICS, string, NULL, condition)) == TGSERROR_OK) {
                        while ((error = step()) == TGSERROR_WAIT_RESULT) {
                            if ((error = readField(0, &field)) == TGSERROR_OK) {
                                vector.push_back(field);
                            }
                            if (error != TGSERROR_OK) {
                                while (step() == TGSERROR_WAIT_RESULT);
                                break;
                            }
                        }
                        if (error == TGSERROR_OK) {
                            *result = vector;
                        }
                    }
                }
            }
        }
    }
    else {
        error = TGSERROR_INVALID_PARAM;
    }
    return error;
}

/*public */TGSError TGSPhysicsDatabase::getFieldByName(std::string const& name, std::vector<FieldRec>* result)
{
    return getField("%" + name + "%", 1, result);
}

/*public */TGSError TGSPhysicsDatabase::getFieldByCallsign(std::string const& callsign, std::vector<FieldRec>* result)
{
    return getField("%" + callsign + "%", 2, result);
}

/*public */TGSError TGSPhysicsDatabase::getNORADByName(std::string const& name, std::vector<int>* result)
{
    return getNORAD("%" + name + "%", 1, result);
}

/*public */TGSError TGSPhysicsDatabase::getNORADByCallsign(std::string const& callsign, std::vector<int>* result)
{
    return getNORAD("%" + callsign + "%", 2, result);
}

/*public */bool TGSPhysicsDatabase::hasUpdate(int norad)
{
    int value;
    TGSError error;
    bool result(false);
    
    if (select(TABLE_PHYSICS, g_hasUpdate[1][0], &g_hasUpdate[0], std::string(""), norad) == TGSERROR_OK) {
        if (step() == TGSERROR_WAIT_RESULT) {
            if ((error = readInteger(0, &value)) == TGSERROR_OK) {
                result = !!value;
            }
            while (step() == TGSERROR_WAIT_RESULT);
            if (error == TGSERROR_OK) {
                if (result) {
                    update(TABLE_PHYSICS, g_hasUpdate, lengthof(g_hasUpdate), 0, std::string(""), norad, 0);
                }
            }
        }
    }
    return result;
}

/*public virtual */TGSError TGSPhysicsDatabase::open(std::string const& file, int timeout)
{
    TGSError error(TGSERROR_OK);
    
    if ((error = super::open(file, timeout)) == TGSERROR_OK) {
        error = create(TABLE_PHYSICS, g_table, lengthof(g_table));
        if (error != TGSERROR_OK) {
            self::close();
        }
    }
    return error;
}

/*public virtual */void TGSPhysicsDatabase::close(void)
{
    super::close();
    return;
}

/*private */TGSError TGSPhysicsDatabase::setText(int norad, char const* format[3][2], std::string const& param)
{
    return update(TABLE_PHYSICS, format, 3, 0, std::string(""), norad, param.c_str(), 1);
}

/*private */TGSError TGSPhysicsDatabase::getText(int norad, char const* format[2][2], std::string* result)
{
    TGSError error(TGSERROR_OK);
    
    if (result != NULL) {
        if ((error = select(TABLE_PHYSICS, format[1][0], &format[0], std::string(""), norad)) == TGSERROR_OK) {
            if ((error = step()) == TGSERROR_WAIT_RESULT) {
                error = readText(0, result);
                while (step() == TGSERROR_WAIT_RESULT);
            }
            else if (error == TGSERROR_OK) {
                error = TGSERROR_NO_RESULT;
            }
        }
    }
    else {
        error = TGSERROR_INVALID_PARAM;
    }
    return error;
}

/*private */TGSError TGSPhysicsDatabase::setRadio(int norad, char const* format[5][2], RadioRec const& param)
{
    return update(TABLE_PHYSICS, format, 5, 0, std::string(""), norad, param.mode.c_str(), param.frequency, param.drift, 1);
}

/*private */TGSError TGSPhysicsDatabase::getRadio(int norad, char const* format[4][2], RadioRec* result)
{
    std::string string;
    RadioRec radio;
    TGSError error(TGSERROR_OK);
    
    if (result != NULL) {
        if ((error = checkFlow()) == TGSERROR_OK) {
            if ((error = buildQuery("", &format[1], 3, &string)) == TGSERROR_OK) {
                if ((error = select(TABLE_PHYSICS, string, &format[0], std::string(""), norad)) == TGSERROR_OK) {
                    if ((error = step()) == TGSERROR_WAIT_RESULT) {
                        if ((error = readText(0, &radio.mode)) == TGSERROR_OK) {
                            if ((error = readInteger(1, &radio.frequency)) == TGSERROR_OK) {
                                if ((error = readInteger(2, &radio.drift)) == TGSERROR_OK) {
                                    *result = radio;
                                }
                            }
                        }
                        while (step() == TGSERROR_WAIT_RESULT);
                    }
                    else if (error == TGSERROR_OK) {
                        error = TGSERROR_NO_RESULT;
                    }
                }
            }
        }
    }
    else {
        error = TGSERROR_INVALID_PARAM;
    }
    return error;
}

/*private */TGSError TGSPhysicsDatabase::getField(std::string const& key, int index, std::vector<FieldRec>* result)
{
    std::string string;
    std::string condition;
    std::vector<FieldRec> vector;
    FieldRec field;
    TGSError error(TGSERROR_OK);
    
    if (result != NULL) {
        if ((error = checkFlow()) == TGSERROR_OK) {
            if ((error = buildQuery("", &g_field[0], 15, &string)) == TGSERROR_OK) {
                if ((error = buildOrder("", &g_field[0], true, -1, -1, &condition)) == TGSERROR_OK) {
                    if ((error = select(TABLE_PHYSICS, string, &g_field[index], condition, key.c_str())) == TGSERROR_OK) {
                        while ((error = step()) == TGSERROR_WAIT_RESULT) {
                            if ((error = readField(0, &field)) == TGSERROR_OK) {
                                vector.push_back(field);
                            }
                            if (error != TGSERROR_OK) {
                                while (step() == TGSERROR_WAIT_RESULT);
                                break;
                            }
                        }
                        if (error == TGSERROR_OK) {
                            *result = vector;
                        }
                    }
                }
            }
        }
    }
    else {
        error = TGSERROR_INVALID_PARAM;
    }
    return error;
}

/*private */TGSError TGSPhysicsDatabase::getNORAD(std::string const& key, int index, std::vector<int>* result)
{
    std::string condition;
    std::vector<int> vector;
    int norad;
    TGSError error(TGSERROR_OK);
    
    if (result != NULL) {
        if ((error = checkFlow()) == TGSERROR_OK) {
            if ((error = buildOrder("", &g_field[0], true, -1, -1, &condition)) == TGSERROR_OK) {
                if ((error = select(TABLE_PHYSICS, g_field[0][0], &g_field[index], condition, key.c_str())) == TGSERROR_OK) {
                    while ((error = step()) == TGSERROR_WAIT_RESULT) {
                        if ((error = readInteger(0, &norad)) == TGSERROR_OK) {
                            vector.push_back(norad);
                        }
                        if (error != TGSERROR_OK) {
                            while (step() == TGSERROR_WAIT_RESULT);
                            break;
                        }
                    }
                    if (error == TGSERROR_OK) {
                        *result = vector;
                    }
                }
            }
        }
    }
    else {
        error = TGSERROR_INVALID_PARAM;
    }
    return error;
}

/*private */TGSError TGSPhysicsDatabase::readField(int column, FieldRec* result)
{
    std::string string;
    std::string one;
    std::string two;
    TGSError error(TGSERROR_OK);
    
    if ((error = readInteger(column, &result->norad)) == TGSERROR_OK) {
        if ((error = readText(++column, &result->name)) == TGSERROR_OK) {
            if ((error = readText(++column, &result->callsign)) == TGSERROR_OK) {
                if ((error = readText(++column, &result->beacon.mode)) == TGSERROR_OK) {
                    if ((error = readInteger(++column, &result->beacon.frequency)) == TGSERROR_OK) {
                        if ((error = readInteger(++column, &result->beacon.drift)) == TGSERROR_OK) {
                            if ((error = readText(++column, &result->sender.mode)) == TGSERROR_OK) {
                                if ((error = readInteger(++column, &result->sender.frequency)) == TGSERROR_OK) {
                                    if ((error = readInteger(++column, &result->sender.drift)) == TGSERROR_OK) {
                                        if ((error = readText(++column, &result->receiver.mode)) == TGSERROR_OK) {
                                            if ((error = readInteger(++column, &result->receiver.frequency)) == TGSERROR_OK) {
                                                if ((error = readInteger(++column, &result->receiver.drift)) == TGSERROR_OK) {
                                                    if ((error = readText(++column, &string)) == TGSERROR_OK) {
                                                        if ((error = result->time.parse(TIME_FORMAT, string)) == TGSERROR_OK) {
                                                            if ((error = readText(++column, &one)) == TGSERROR_OK) {
                                                                if ((error = readText(++column, &two)) == TGSERROR_OK) {
                                                                    error = convert(result->name, one, two, &result->tle);
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return error;
}

}// end of namespace
