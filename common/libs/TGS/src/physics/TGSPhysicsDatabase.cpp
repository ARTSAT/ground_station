/*
**      ARTSAT Project
**
**      Original Copyright (C) 2013 - 2015 HORIGUCHI Junshi.
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
#include "TGSOrbitTLE.h"
#include "TGSOrbitSCD.h"

namespace tgs {

#define TABLE_PHYSICS       ("_physics6_")
#define ORBIT_TLE           ("TLE")
#define ORBIT_SCD           ("SCD")
#define TIME_FORMAT         ("%YYYY-%MM-%DD %hh:%mm:%ss")

static  char const*         g_table[][2] = {
    {"exnorad",             "INTEGER NOT NULL PRIMARY KEY"},
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
    {"epoch",               "REAL NOT NULL DEFAULT '0.0'"},
    {"orbit_type",          "TEXT NOT NULL DEFAULT 'NONE'"},
    {"orbit_data1",         "TEXT"},
    {"orbit_data2",         "TEXT"},
    {"\"update\"",          "INTEGER NOT NULL DEFAULT 0"}
};
static  char const*         g_name[][2] = {
    {"exnorad",             "%d"},
    {"name",                "%Q"},
    {"\"update\"",          "%d"}
};
static  char const*         g_callsign[][2] = {
    {"exnorad",             "%d"},
    {"callsign",            "%Q"},
    {"\"update\"",          "%d"}
};
static  char const*         g_radioBeacon[][2] = {
    {"exnorad",             "%d"},
    {"beacon_mode",         "%Q"},
    {"beacon_frequency",    "%d"},
    {"beacon_drift",        "%d"},
    {"\"update\"",          "%d"}
};
static  char const*         g_radioSender[][2] = {
    {"exnorad",             "%d"},
    {"sender_mode",         "%Q"},
    {"sender_frequency",    "%d"},
    {"sender_drift",        "%d"},
    {"\"update\"",          "%d"}
};
static  char const*         g_radioReceiver[][2] = {
    {"exnorad",             "%d"},
    {"receiver_mode",       "%Q"},
    {"receiver_frequency",  "%d"},
    {"receiver_drift",      "%d"},
    {"\"update\"",          "%d"}
};
static  char const*         g_setOrbitData[][2] = {
    {"exnorad",             "%d"},
    {"name",                "%Q"},
    {"epoch",               "%lf"},
    {"orbit_type",          "%Q"},
    {"orbit_data1",         "%Q"},
    {"orbit_data2",         "%Q"},
    {"\"update\"",          "%d"}
};
static  char const*         g_setOrbitTime[][2] = {
    {"exnorad",             "%d"},
    {"time",                "%Q"}
};
static  char const*         g_getOrbitData[][2] = {
    {"exnorad",             "%d"},
    {"name",                "%Q"},
    {"time",                "%Q"},
    {"orbit_type",          "%Q"},
    {"orbit_data1",         "%Q"},
    {"orbit_data2",         "%Q"}
};
static  char const*         g_field[][2] = {
    {"exnorad",             "%d"},
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
    {"orbit_type",          "%Q"},
    {"orbit_data1",         "%Q"},
    {"orbit_data2",         "%Q"}
};
static  char const*         g_hasUpdate[][2] = {
    {"exnorad",             "%d"},
    {"\"update\"",          "%d"}
};

/*public */TGSPhysicsDatabase::TGSPhysicsDatabase(void)
{
}

/*public virtual */TGSPhysicsDatabase::~TGSPhysicsDatabase(void)
{
    close();
}

/*public */TGSError TGSPhysicsDatabase::setName(int exnorad, std::string const& param)
{
    return setText(exnorad, g_name, param);
}

/*public */TGSError TGSPhysicsDatabase::getName(int exnorad, std::string* result)
{
    return getText(exnorad, g_name, result);
}

/*public */TGSError TGSPhysicsDatabase::setCallsign(int exnorad, std::string const& param)
{
    return setText(exnorad, g_callsign, param);
}

/*public */TGSError TGSPhysicsDatabase::getCallsign(int exnorad, std::string* result)
{
    return getText(exnorad, g_callsign, result);
}

/*public */TGSError TGSPhysicsDatabase::setRadioBeacon(int exnorad, RadioRec const& param)
{
    return setRadio(exnorad, g_radioBeacon, param);
}

/*public */TGSError TGSPhysicsDatabase::getRadioBeacon(int exnorad, RadioRec* result)
{
    return getRadio(exnorad, g_radioBeacon, result);
}

/*public */TGSError TGSPhysicsDatabase::setRadioSender(int exnorad, RadioRec const& param)
{
    return setRadio(exnorad, g_radioSender, param);
}

/*public */TGSError TGSPhysicsDatabase::getRadioSender(int exnorad, RadioRec* result)
{
    return getRadio(exnorad, g_radioSender, result);
}

/*public */TGSError TGSPhysicsDatabase::setRadioReceiver(int exnorad, RadioRec const& param)
{
    return setRadio(exnorad, g_radioReceiver, param);
}

/*public */TGSError TGSPhysicsDatabase::getRadioReceiver(int exnorad, RadioRec* result)
{
    return getRadio(exnorad, g_radioReceiver, result);
}

/*public */TGSError TGSPhysicsDatabase::setOrbitData(OrbitData const& param, ir::IRXTime const& time)
{
    TGSOrbitInterface* orbit;
    std::string name;
    std::string data1;
    std::string data2;
    std::string type;
    int exnorad;
    double epoch;
    std::string condition;
    TGSError error(TGSERROR_OK);
    
    if ((error = checkFlow()) == TGSERROR_OK) {
        orbit = NULL;
        switch (param.getType()) {
            case OrbitData::TYPE_TLE:
                if ((orbit = new(std::nothrow) TGSOrbitTLE) != NULL) {
                    if ((error = convertTLE(param, &name, &data1, &data2)) == TGSERROR_OK) {
                        type = ORBIT_TLE;
                    }
                }
                else {
                    error = TGSERROR_NO_MEMORY;
                }
                break;
            case OrbitData::TYPE_SCD:
                if ((orbit = new(std::nothrow) TGSOrbitSCD) != NULL) {
                    if ((error = convertSCD(param, &name, &data1, &data2)) == TGSERROR_OK) {
                        type = ORBIT_SCD;
                    }
                }
                else {
                    error = TGSERROR_NO_MEMORY;
                }
                break;
            default:
                error = TGSERROR_INVALID_FORMAT;
                break;
        }
        if (error == TGSERROR_OK) {
            if ((error = orbit->setOrbitData(param)) == TGSERROR_OK) {
                if ((error = orbit->getID(&exnorad)) == TGSERROR_OK) {
                    if ((error = orbit->getEpochTime(&epoch)) == TGSERROR_OK) {
                        if ((error = print("AND epoch<%lf", &condition, epoch)) == TGSERROR_OK) {
                            if ((error = update(TABLE_PHYSICS, g_setOrbitData, lengthof(g_setOrbitData), 0, condition, exnorad, name.c_str(), epoch, type.c_str(), data1.c_str(), data2.c_str(), 1)) == TGSERROR_OK) {
                                error = update(TABLE_PHYSICS, g_setOrbitTime, lengthof(g_setOrbitTime), 0, std::string(""), exnorad, time.format(TIME_FORMAT).c_str());
                            }
                        }
                    }
                }
            }
        }
        if (orbit != NULL) {
            delete orbit;
        }
    }
    return error;
}

/*public */TGSError TGSPhysicsDatabase::getOrbitData(int exnorad, OrbitData* result, ir::IRXTime* time)
{
    std::string string;
    std::string name;
    std::string data1;
    std::string data2;
    OrbitData orbit;
    ir::IRXTime date;
    TGSError error(TGSERROR_OK);
    
    if (result != NULL) {
        if ((error = checkFlow()) == TGSERROR_OK) {
            if ((error = buildQuery("", &g_getOrbitData[1], lengthof(g_getOrbitData) - 1, &string)) == TGSERROR_OK) {
                if ((error = select(TABLE_PHYSICS, string, &g_getOrbitData[0], std::string(""), exnorad)) == TGSERROR_OK) {
                    if ((error = step()) == TGSERROR_WAIT_RESULT) {
                        if ((error = readText(0, true, &name)) == TGSERROR_OK) {
                            if ((error = readText(1, false, &string)) == TGSERROR_OK) {
                                if ((error = date.parse(TIME_FORMAT, string)) == TGSERROR_OK) {
                                    if ((error = readText(2, false, &string)) == TGSERROR_OK) {
                                        if ((error = readText(3, true, &data1)) == TGSERROR_OK) {
                                            if ((error = readText(4, true, &data2)) == TGSERROR_OK) {
                                                if (string == ORBIT_TLE) {
                                                    error = convertTLE(name, data1, data2, &orbit);
                                                }
                                                else if (string == ORBIT_SCD) {
                                                    error = convertSCD(name, data1, data2, &orbit);
                                                }
                                                if (error == TGSERROR_OK) {
                                                    *result = orbit;
                                                    if (time != NULL) {
                                                        *time = date;
                                                    }
                                                }
                                            }
                                        }
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
    else {
        error = TGSERROR_INVALID_PARAM;
    }
    return error;
}

/*public */TGSError TGSPhysicsDatabase::getCount(int* result)
{
    return countRow(TABLE_PHYSICS, NULL, "", result);
}

/*public */TGSError TGSPhysicsDatabase::getField(int exnorad, FieldRec* result)
{
    std::string string;
    FieldRec field;
    TGSError error(TGSERROR_OK);
    
    if (result != NULL) {
        if ((error = checkFlow()) == TGSERROR_OK) {
            if ((error = buildQuery("", &g_field[0], lengthof(g_field), &string)) == TGSERROR_OK) {
                if ((error = select(TABLE_PHYSICS, string, &g_field[0], std::string(""), exnorad)) == TGSERROR_OK) {
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
            if ((error = buildQuery("", &g_field[0], lengthof(g_field), &string)) == TGSERROR_OK) {
                if ((error = buildOrder("", &g_field[0], true, limit, offset, &condition)) == TGSERROR_OK) {
                    if ((error = select(TABLE_PHYSICS, string, NULL, condition)) == TGSERROR_OK) {
                        while ((error = step()) == TGSERROR_WAIT_RESULT) {
                            if ((error = readField(0, &field)) == TGSERROR_OK) {
                                vector.push_back(field);
                            }
                            else {
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

/*public */TGSError TGSPhysicsDatabase::getEXNORADByName(std::string const& name, std::vector<int>* result)
{
    return getEXNORAD("%" + name + "%", 1, result);
}

/*public */TGSError TGSPhysicsDatabase::getEXNORADByCallsign(std::string const& callsign, std::vector<int>* result)
{
    return getEXNORAD("%" + callsign + "%", 2, result);
}

/*public */bool TGSPhysicsDatabase::hasUpdate(int exnorad)
{
    int value;
    bool result(false);
    
    if (select(TABLE_PHYSICS, g_hasUpdate[1][0], &g_hasUpdate[0], std::string(""), exnorad) == TGSERROR_OK) {
        if (step() == TGSERROR_WAIT_RESULT) {
            if (readInteger(0, false, &value) == TGSERROR_OK) {
                result = !!value;
            }
            while (step() == TGSERROR_WAIT_RESULT);
            if (result) {
                update(TABLE_PHYSICS, g_hasUpdate, lengthof(g_hasUpdate), 0, std::string(""), exnorad, 0);
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

/*private */TGSError TGSPhysicsDatabase::setText(int exnorad, char const* format[3][2], std::string const& param)
{
    return update(TABLE_PHYSICS, format, 3, 0, std::string(""), exnorad, param.c_str(), 1);
}

/*private */TGSError TGSPhysicsDatabase::getText(int exnorad, char const* format[2][2], std::string* result)
{
    std::string text;
    TGSError error(TGSERROR_OK);
    
    if (result != NULL) {
        if ((error = select(TABLE_PHYSICS, format[1][0], &format[0], std::string(""), exnorad)) == TGSERROR_OK) {
            if ((error = step()) == TGSERROR_WAIT_RESULT) {
                if ((error = readText(0, true, &text)) == TGSERROR_OK) {
                    *result = text;
                }
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

/*private */TGSError TGSPhysicsDatabase::setRadio(int exnorad, char const* format[5][2], RadioRec const& param)
{
    return update(TABLE_PHYSICS, format, 5, 0, std::string(""), exnorad, param.mode.c_str(), param.frequency, param.drift, 1);
}

/*private */TGSError TGSPhysicsDatabase::getRadio(int exnorad, char const* format[4][2], RadioRec* result)
{
    std::string string;
    RadioRec radio;
    TGSError error(TGSERROR_OK);
    
    if (result != NULL) {
        if ((error = checkFlow()) == TGSERROR_OK) {
            if ((error = buildQuery("", &format[1], 3, &string)) == TGSERROR_OK) {
                if ((error = select(TABLE_PHYSICS, string, &format[0], std::string(""), exnorad)) == TGSERROR_OK) {
                    if ((error = step()) == TGSERROR_WAIT_RESULT) {
                        if ((error = readText(0, true, &radio.mode)) == TGSERROR_OK) {
                            radio.frequency = -1;
                            if ((error = readInteger(1, true, &radio.frequency)) == TGSERROR_OK) {
                                radio.drift = INT_MIN;
                                if ((error = readInteger(2, true, &radio.drift)) == TGSERROR_OK) {
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
            if ((error = buildQuery("", &g_field[0], lengthof(g_field), &string)) == TGSERROR_OK) {
                if ((error = buildOrder("", &g_field[0], true, -1, -1, &condition)) == TGSERROR_OK) {
                    if ((error = select(TABLE_PHYSICS, string, &g_field[index], condition, key.c_str())) == TGSERROR_OK) {
                        while ((error = step()) == TGSERROR_WAIT_RESULT) {
                            if ((error = readField(0, &field)) == TGSERROR_OK) {
                                vector.push_back(field);
                            }
                            else {
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

/*private */TGSError TGSPhysicsDatabase::getEXNORAD(std::string const& key, int index, std::vector<int>* result)
{
    std::string condition;
    std::vector<int> vector;
    int exnorad;
    TGSError error(TGSERROR_OK);
    
    if (result != NULL) {
        if ((error = checkFlow()) == TGSERROR_OK) {
            if ((error = buildOrder("", &g_field[0], true, -1, -1, &condition)) == TGSERROR_OK) {
                if ((error = select(TABLE_PHYSICS, g_field[0][0], &g_field[index], condition, key.c_str())) == TGSERROR_OK) {
                    while ((error = step()) == TGSERROR_WAIT_RESULT) {
                        if ((error = readInteger(0, false, &exnorad)) == TGSERROR_OK) {
                            vector.push_back(exnorad);
                        }
                        else {
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

/*private */TGSError TGSPhysicsDatabase::readField(int column, FieldRec* result) const
{
    FieldRec field;
    std::string string;
    std::string data1;
    std::string data2;
    TGSError error(TGSERROR_OK);
    
    if ((error = readInteger(column, false, &field.exnorad)) == TGSERROR_OK) {
        if ((error = readText(++column, true, &field.name)) == TGSERROR_OK) {
            if ((error = readText(++column, true, &field.callsign)) == TGSERROR_OK) {
                if ((error = readText(++column, true, &field.beacon.mode)) == TGSERROR_OK) {
                    field.beacon.frequency = -1;
                    if ((error = readInteger(++column, true, &field.beacon.frequency)) == TGSERROR_OK) {
                        field.beacon.drift = INT_MIN;
                        if ((error = readInteger(++column, true, &field.beacon.drift)) == TGSERROR_OK) {
                            if ((error = readText(++column, true, &field.sender.mode)) == TGSERROR_OK) {
                                field.sender.frequency = -1;
                                if ((error = readInteger(++column, true, &field.sender.frequency)) == TGSERROR_OK) {
                                    field.sender.drift = INT_MIN;
                                    if ((error = readInteger(++column, true, &field.sender.drift)) == TGSERROR_OK) {
                                        if ((error = readText(++column, true, &field.receiver.mode)) == TGSERROR_OK) {
                                            field.receiver.frequency = -1;
                                            if ((error = readInteger(++column, true, &field.receiver.frequency)) == TGSERROR_OK) {
                                                field.receiver.drift = INT_MIN;
                                                if ((error = readInteger(++column, true, &field.receiver.drift)) == TGSERROR_OK) {
                                                    if ((error = readText(++column, false, &string)) == TGSERROR_OK) {
                                                        if ((error = field.time.parse(TIME_FORMAT, string)) == TGSERROR_OK) {
                                                            if ((error = readText(++column, false, &string)) == TGSERROR_OK) {
                                                                if ((error = readText(++column, true, &data1)) == TGSERROR_OK) {
                                                                    if ((error = readText(++column, true, &data2)) == TGSERROR_OK) {
                                                                        if (string == ORBIT_TLE) {
                                                                            error = convertTLE(field.name, data1, data2, &field.orbit);
                                                                        }
                                                                        else if (string == ORBIT_SCD) {
                                                                            error = convertSCD(field.name, data1, data2, &field.orbit);
                                                                        }
                                                                        if (error == TGSERROR_OK) {
                                                                            *result = field;
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
        }
    }
    return error;
}

/*private */TGSError TGSPhysicsDatabase::readInteger(int column, bool implicit, int* result) const
{
    TGSError error(TGSERROR_OK);
    
    if ((error = super::readInteger(column, result)) == TGSERROR_NO_RESULT) {
        error = (implicit) ? (TGSERROR_OK) : (TGSERROR_INVALID_FORMAT);
    }
    return error;
}

/*private */TGSError TGSPhysicsDatabase::readText(int column, bool implicit, std::string* result) const
{
    TGSError error(TGSERROR_OK);
    
    if ((error = super::readText(column, result)) == TGSERROR_NO_RESULT) {
        error = (implicit) ? (TGSERROR_OK) : (TGSERROR_INVALID_FORMAT);
    }
    return error;
}

}// end of namespace
