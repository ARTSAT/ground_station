/*
**      ARTSAT Project
**
**      Original Copyright (C) 2014 - 2014 HORIGUCHI Junshi.
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
**      ASDPluginINVADER.cpp
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

#include "ASDPluginINVADER.h"
#include "artsatd.h"

enum NoteEnum {
    NOTE_END                = -1,
    NOTE_REST               = 0,
    NOTE_B0                 = 31,
    NOTE_C1                 = 33,
    NOTE_CS1                = 35,
    NOTE_D1                 = 37,
    NOTE_DS1                = 39,
    NOTE_E1                 = 41,
    NOTE_F1                 = 44,
    NOTE_FS1                = 46,
    NOTE_G1                 = 49,
    NOTE_GS1                = 52,
    NOTE_A1                 = 55,
    NOTE_AS1                = 58,
    NOTE_B1                 = 62,
    NOTE_C2                 = 65,
    NOTE_CS2                = 69,
    NOTE_D2                 = 73,
    NOTE_DS2                = 78,
    NOTE_E2                 = 82,
    NOTE_F2                 = 87,
    NOTE_FS2                = 93,
    NOTE_G2                 = 98,
    NOTE_GS2                = 104,
    NOTE_A2                 = 110,
    NOTE_AS2                = 117,
    NOTE_B2                 = 123,
    NOTE_C3                 = 131,
    NOTE_CS3                = 139,
    NOTE_D3                 = 147,
    NOTE_DS3                = 156,
    NOTE_E3                 = 165,
    NOTE_F3                 = 175,
    NOTE_FS3                = 185,
    NOTE_G3                 = 196,
    NOTE_GS3                = 208,
    NOTE_A3                 = 220,
    NOTE_AS3                = 233,
    NOTE_B3                 = 247,
    NOTE_C4                 = 262,
    NOTE_CS4                = 277,
    NOTE_D4                 = 294,
    NOTE_DS4                = 311,
    NOTE_E4                 = 330,
    NOTE_F4                 = 349,
    NOTE_FS4                = 370,
    NOTE_G4                 = 392,
    NOTE_GS4                = 415,
    NOTE_A4                 = 440,
    NOTE_AS4                = 466,
    NOTE_B4                 = 494,
    NOTE_C5                 = 523,
    NOTE_CS5                = 554,
    NOTE_D5                 = 587,
    NOTE_DS5                = 622,
    NOTE_E5                 = 659,
    NOTE_F5                 = 698,
    NOTE_FS5                = 740,
    NOTE_G5                 = 784,
    NOTE_GS5                = 831,
    NOTE_A5                 = 880,
    NOTE_AS5                = 932,
    NOTE_B5                 = 988,
    NOTE_C6                 = 1047,
    NOTE_CS6                = 1109,
    NOTE_D6                 = 1175,
    NOTE_DS6                = 1245,
    NOTE_E6                 = 1319,
    NOTE_F6                 = 1397,
    NOTE_FS6                = 1480,
    NOTE_G6                 = 1568,
    NOTE_GS6                = 1661,
    NOTE_A6                 = 1760,
    NOTE_AS6                = 1865,
    NOTE_B6                 = 1976,
    NOTE_C7                 = 2093,
    NOTE_CS7                = 2217,
    NOTE_D7                 = 2349,
    NOTE_DS7                = 2489,
    NOTE_E7                 = 2637,
    NOTE_F7                 = 2794,
    NOTE_FS7                = 2960,
    NOTE_G7                 = 3136,
    NOTE_GS7                = 3322,
    NOTE_A7                 = 3520,
    NOTE_AS7                = 3729,
    NOTE_B7                 = 3951,
    NOTE_C8                 = 4186,
    NOTE_CS8                = 4435,
    NOTE_D8                 = 4699,
    NOTE_DS8                = 4978
};
enum DurationEnum {
    DURATION_01             = 1,
    DURATION_02             = 2,
    DURATION_04             = 4,
    DURATION_08             = 8,
    DURATION_16             = 16,
    DURATION_32             = 32
};
enum QualifierEnum {
    QUALIFIER_NONE,
    QUALIFIER_DOT,
    QUALIFIER_DOTDOT,
    QUALIFIER_TRIPLE,
    QUALIFIER_QUINTUPLE,
    QUALIFIER_SEXTUPLE,
    QUALIFIER_SEPTUPLE,
    QUALIFIER_NONUPLE,
    QUALIFIER_LIMIT
};

struct NoteRec {
    char const* name;
    NoteEnum note;
};
struct DurationRec {
    char const* name;
    DurationEnum duration;
    QualifierEnum qualifier;
};

static NoteRec const g_note[] = {
    {"-",   NOTE_REST},
    {"b0",  NOTE_B0},
    {"c1",  NOTE_C1},
    {"c#1", NOTE_CS1},
    {"d1",  NOTE_D1},
    {"d#1", NOTE_DS1},
    {"e1",  NOTE_E1},
    {"f1",  NOTE_F1},
    {"f#1", NOTE_FS1},
    {"g1",  NOTE_G1},
    {"g#1", NOTE_GS1},
    {"a1",  NOTE_A1},
    {"a#1", NOTE_AS1},
    {"b1",  NOTE_B1},
    {"c2",  NOTE_C2},
    {"c#2", NOTE_CS2},
    {"d2",  NOTE_D2},
    {"d#2", NOTE_DS2},
    {"e2",  NOTE_E2},
    {"f2",  NOTE_F2},
    {"f#2", NOTE_FS2},
    {"g2",  NOTE_G2},
    {"g#2", NOTE_GS2},
    {"a2",  NOTE_A2},
    {"a#2", NOTE_AS2},
    {"b2",  NOTE_B2},
    {"c3",  NOTE_C3},
    {"c#3", NOTE_CS3},
    {"d3",  NOTE_D3},
    {"d#3", NOTE_DS3},
    {"e3",  NOTE_E3},
    {"f3",  NOTE_F3},
    {"f#3", NOTE_FS3},
    {"g3",  NOTE_G3},
    {"g#3", NOTE_GS3},
    {"a3",  NOTE_A3},
    {"a#3", NOTE_AS3},
    {"b3",  NOTE_B3},
    {"c4",  NOTE_C4},
    {"c#4", NOTE_CS4},
    {"d4",  NOTE_D4},
    {"d#4", NOTE_DS4},
    {"e4",  NOTE_E4},
    {"f4",  NOTE_F4},
    {"f#4", NOTE_FS4},
    {"g4",  NOTE_G4},
    {"g#4", NOTE_GS4},
    {"a4",  NOTE_A4},
    {"a#4", NOTE_AS4},
    {"b4",  NOTE_B4},
    {"c5",  NOTE_C5},
    {"c#5", NOTE_CS5},
    {"d5",  NOTE_D5},
    {"d#5", NOTE_DS5},
    {"e5",  NOTE_E5},
    {"f5",  NOTE_F5},
    {"f#5", NOTE_FS5},
    {"g5",  NOTE_G5},
    {"g#5", NOTE_GS5},
    {"a5",  NOTE_A5},
    {"a#5", NOTE_AS5},
    {"b5",  NOTE_B5},
    {"c6",  NOTE_C6},
    {"c#6", NOTE_CS6},
    {"d6",  NOTE_D6},
    {"d#6", NOTE_DS6},
    {"e6",  NOTE_E6},
    {"f6",  NOTE_F6},
    {"f#6", NOTE_FS6},
    {"g6",  NOTE_G6},
    {"g#6", NOTE_GS6},
    {"a6",  NOTE_A6},
    {"a#6", NOTE_AS6},
    {"b6",  NOTE_B6},
    {"c7",  NOTE_C7},
    {"c#7", NOTE_CS7},
    {"d7",  NOTE_D7},
    {"d#7", NOTE_DS7},
    {"e7",  NOTE_E7},
    {"f7",  NOTE_F7},
    {"f#7", NOTE_FS7},
    {"g7",  NOTE_G7},
    {"g#7", NOTE_GS7},
    {"a7",  NOTE_A7},
    {"a#7", NOTE_AS7},
    {"b7",  NOTE_B7},
    {"c8",  NOTE_C8},
    {"c#8", NOTE_CS8},
    {"d8",  NOTE_D8},
    {"d#8", NOTE_DS8}
};
static DurationRec const g_duration[] = {
    { "1",   DURATION_01, QUALIFIER_NONE},
    { "2",   DURATION_02, QUALIFIER_NONE},
    { "4",   DURATION_04, QUALIFIER_NONE},
    { "8",   DURATION_08, QUALIFIER_NONE},
    {"16",   DURATION_16, QUALIFIER_NONE},
    {"32",   DURATION_32, QUALIFIER_NONE},
    { "1.",  DURATION_01, QUALIFIER_DOT},
    { "2.",  DURATION_02, QUALIFIER_DOT},
    { "4.",  DURATION_04, QUALIFIER_DOT},
    { "8.",  DURATION_08, QUALIFIER_DOT},
    {"16.",  DURATION_16, QUALIFIER_DOT},
    {"32.",  DURATION_32, QUALIFIER_DOT},
    { "1..", DURATION_01, QUALIFIER_DOTDOT},
    { "2..", DURATION_02, QUALIFIER_DOTDOT},
    { "4..", DURATION_04, QUALIFIER_DOTDOT},
    { "8..", DURATION_08, QUALIFIER_DOTDOT},
    {"16..", DURATION_16, QUALIFIER_DOTDOT},
    {"32..", DURATION_32, QUALIFIER_DOTDOT},
    { "1~3", DURATION_01, QUALIFIER_TRIPLE},
    { "2~3", DURATION_02, QUALIFIER_TRIPLE},
    { "4~3", DURATION_04, QUALIFIER_TRIPLE},
    { "8~3", DURATION_08, QUALIFIER_TRIPLE},
    {"16~3", DURATION_16, QUALIFIER_TRIPLE},
    {"32~3", DURATION_32, QUALIFIER_TRIPLE},
    { "1~5", DURATION_01, QUALIFIER_QUINTUPLE},
    { "2~5", DURATION_02, QUALIFIER_QUINTUPLE},
    { "4~5", DURATION_04, QUALIFIER_QUINTUPLE},
    { "8~5", DURATION_08, QUALIFIER_QUINTUPLE},
    {"16~5", DURATION_16, QUALIFIER_QUINTUPLE},
    {"32~5", DURATION_32, QUALIFIER_QUINTUPLE},
    { "1~6", DURATION_01, QUALIFIER_SEXTUPLE},
    { "2~6", DURATION_02, QUALIFIER_SEXTUPLE},
    { "4~6", DURATION_04, QUALIFIER_SEXTUPLE},
    { "8~6", DURATION_08, QUALIFIER_SEXTUPLE},
    {"16~6", DURATION_16, QUALIFIER_SEXTUPLE},
    {"32~6", DURATION_32, QUALIFIER_SEXTUPLE},
    { "1~7", DURATION_01, QUALIFIER_SEPTUPLE},
    { "2~7", DURATION_02, QUALIFIER_SEPTUPLE},
    { "4~7", DURATION_04, QUALIFIER_SEPTUPLE},
    { "8~7", DURATION_08, QUALIFIER_SEPTUPLE},
    {"16~7", DURATION_16, QUALIFIER_SEPTUPLE},
    {"32~7", DURATION_32, QUALIFIER_SEPTUPLE},
    { "1~9", DURATION_01, QUALIFIER_NONUPLE},
    { "2~9", DURATION_02, QUALIFIER_NONUPLE},
    { "4~9", DURATION_04, QUALIFIER_NONUPLE},
    { "8~9", DURATION_08, QUALIFIER_NONUPLE},
    {"16~9", DURATION_16, QUALIFIER_NONUPLE},
    {"32~9", DURATION_32, QUALIFIER_NONUPLE}
};

/*public */ASDPluginINVADER::ASDPluginINVADER(void)
{
}

/*public virtual */ASDPluginINVADER::~ASDPluginINVADER(void)
{
}

/*public virtual */void ASDPluginINVADER::execute(std::string const& session, insensitive::map<std::string, std::string> const& query, std::string* category, std::string* message)
{
    CommandRec context;
    insensitive::map<std::string, std::string>::const_iterator it;
    std::string type;
    std::string command;
    std::string text;
    int value;
    
    if ((it = query.find("category")) != query.end()) {
        type = it->second;
        if ((it = query.find("command")) != query.end()) {
            command = it->second;
        }
        _mutex.lock();
        context = _command;
        _mutex.unlock();
        if (type == "normal_operation") {
            if (command == "STA") {
                queueSTA(session);
            }
            else if (command == "MRR") {
                queueMRR(session);
            }
        }
        else if (type == "telemetry_index") {
            if (command == "PTI") {
                queuePTI(session);
            }
        }
        else if (type == "telemetry_data") {
            if (command == "PTR") {
                if ((it = query.find("value")) != query.end()) {
                    queuePTR(session, it->second);
                    context.address = it->second;
                }
            }
            else if ((it = query.find("edit")) != query.end()) {
                if (it->second == "remove") {
                }
                else if (it->second == "clear") {
                }
                else if (it->second == "append") {
                    if ((it = query.find("offset")) != query.end()) {
                        context.offset = it->second;
                    }
                    if ((it = query.find("count")) != query.end()) {
                        context.count = it->second;
                    }
                }
            }
        }
        else if (type == "boot_mode") {
            if (command == "GRS") {
                queueGRSBootMode(session);
            }
            else if (command == "GWA") {
                if ((it = query.find("value")) != query.end()) {
                    queueGWABootMode(session, it->second);
                    context.mode = it->second;
                }
            }
        }
        else if (type == "text_param") {
            if (command == "GRS") {
                if ((it = query.find("text")) != query.end()) {
                    queueGRSText(session, it->second);
                    context.text = it->second;
                }
            }
            else if (command == "GWA") {
                if ((it = query.find("text")) != query.end()) {
                    text = it->second;
                    if ((it = query.find("value")) != query.end()) {
                        queueGWAText(session, text, it->second);
                        context.text = text;
                        context.param = it->second;
                    }
                }
            }
        }
        else if (type == "vm_param") {
            if (command == "GRS") {
                queueGRSVM(session);
            }
            else if (command == "GWA") {
                if ((it = query.find("value")) != query.end()) {
                    queueGWAVM(session, it->second);
                    context.vm = it->second;
                }
            }
        }
        else if (type == "note_param") {
            if (command == "GRS") {
                queueGRSNote(session);
            }
            else if (command == "GWA") {
                if ((it = query.find("value")) != query.end()) {
                    queueGWANote(session, it->second);
                    context.note = it->second;
                }
            }
        }
        else if (type == "morse_param") {
            if (command == "GRS") {
                queueGRSMorse(session);
            }
            else if (command == "GWA") {
                if ((it = query.find("value")) != query.end()) {
                    queueGWAMorse(session, it->second);
                    context.morse = it->second;
                }
            }
        }
        else if (type == "digitalker_param") {
            if (command == "GRS") {
                queueGRSDigitalker(session);
            }
            else if (command == "GWA") {
                if ((it = query.find("value")) != query.end()) {
                    queueGWADigitalker(session, it->second);
                    context.digitalker = it->second;
                }
            }
        }
        else if (type == "camera_param") {
            if (command == "GRS") {
                queueGRSCamera(session);
            }
            else if (command == "GWA") {
                if ((it = query.find("value")) != query.end()) {
                    queueGWACamera(session, it->second);
                    context.camera = it->second;
                }
            }
        }
        else if (type == "camera_data") {
            if (command == "GRS") {
                if ((it = query.find("value")) != query.end()) {
                    if (!it->second.empty()) {
                        if (it->second.size() <= 3 && boost::all(it->second, boost::is_digit())) {
                            value = boost::lexical_cast<int>(it->second);
                            if (0 <= value && value <= 119) {
                                artsatd::getInstance().requestCommand(session, (boost::format("c-c-g-grs-%06d.320") % (value * 320)).str());
                                context.line = it->second;
                            }
                        }
                    }
                }
            }
        }
        else if (type == "mission_power") {
            if (command == "MSR") {
                queueMSR(session);
            }
            else if (command == "MSD") {
                queueMSD(session);
            }
            else if (command == "SMS") {
                long long obc(-1);
                long long tmp;
                int ohour(-1);
                int ominute(-1);
                int osecond(-1);
                int dhour(-1);
                int dminute(-1);
                int dsecond(-1);
                int i;
                
                if ((it = query.find("obc")) != query.end()) {
                    if (it->second.size() == 8 && boost::all(it->second, boost::is_xdigit())) {
                        tmp = 0;
                        for (i = 0; i < 8; i += 2) {
                            tmp <<= 8;
                            tmp |= (fromHex(it->second[i + 0]) << 4) | (fromHex(it->second[i + 1]) << 0);
                        }
                        obc = tmp;
                        context.obc = it->second;
                    }
                }
                if ((it = query.find("ohour")) != query.end()) {
                    if (1 <= it->second.size() && it->second.size() <= 2 && boost::all(it->second, boost::is_digit())) {
                        value = boost::lexical_cast<int>(it->second);
                        if (0 <= value && value <= 99) {
                            ohour = value;
                            context.ohour = it->second;
                        }
                    }
                }
                if ((it = query.find("ominute")) != query.end()) {
                    if (1 <= it->second.size() && it->second.size() <= 2 && boost::all(it->second, boost::is_digit())) {
                        value = boost::lexical_cast<int>(it->second);
                        if (0 <= value && value <= 59) {
                            ominute = value;
                            context.ominute = it->second;
                        }
                    }
                }
                if ((it = query.find("osecond")) != query.end()) {
                    if (1 <= it->second.size() && it->second.size() <= 2 && boost::all(it->second, boost::is_digit())) {
                        value = boost::lexical_cast<int>(it->second);
                        if (0 <= value && value <= 59) {
                            osecond = value;
                            context.osecond = it->second;
                        }
                    }
                }
                if ((it = query.find("dhour")) != query.end()) {
                    if (1 <= it->second.size() && it->second.size() <= 2 && boost::all(it->second, boost::is_digit())) {
                        value = boost::lexical_cast<int>(it->second);
                        if (0 <= value && value <= 99) {
                            dhour = value;
                            context.dhour = it->second;
                        }
                    }
                }
                if ((it = query.find("dminute")) != query.end()) {
                    if (1 <= it->second.size() && it->second.size() <= 2 && boost::all(it->second, boost::is_digit())) {
                        value = boost::lexical_cast<int>(it->second);
                        if (0 <= value && value <= 59) {
                            dminute = value;
                            context.dminute = it->second;
                        }
                    }
                }
                if ((it = query.find("dsecond")) != query.end()) {
                    if (1 <= it->second.size() && it->second.size() <= 2 && boost::all(it->second, boost::is_digit())) {
                        value = boost::lexical_cast<int>(it->second);
                        if (0 <= value && value <= 59) {
                            dsecond = value;
                            context.dsecond = it->second;
                        }
                    }
                }
                if (obc >= 0 && ohour >= 0 && ominute >= 0 && osecond >= 0 && dhour >= 0 && dminute >= 0 && dsecond >= 0) {
                    ir::IRXTimeDiff offset;
                    ir::IRXTimeDiff duration;
                    
                    offset.set(ohour, ominute, osecond);
                    duration.set(dhour, dminute, dsecond);
                    artsatd::getInstance().requestCommand(session, "c-c-g-sms-" + (boost::format("%d.%d") % (duration.asTime_t() * 100) % (obc + offset.asTime_t() * 100)).str());
                }
            }
        }
        else if (type == "digitalker_power") {
            if (command == "DON") {
                queueDON(session);
            }
            else if (command == "DOF") {
                queueDOF(session);
            }
        }
        _mutex.lock();
        _command = context;
        _mutex.unlock();
    }
    return;
}

/*public virtual */void ASDPluginINVADER::process(std::string const& session, std::string const& category, std::string const& message, std::string* response) const
{
    CommandRec context;
    std::string string;
    std::vector<std::string> preset;
    std::vector<std::string>::const_iterator it;
    int offset;
    int count;
    
    *response = getSkeleton();
    _mutex.lock();
    context = _command;
    _mutex.unlock();
    if (0 < context.offset.size() && context.offset.size() <= 7 && boost::all(context.offset, boost::is_digit())) {
        if (0 < context.count.size() && boost::all(context.count, boost::is_digit())) {
            offset = boost::lexical_cast<int>(context.offset);
            count = boost::lexical_cast<int>(context.count);
            for (; count > 0; --count, offset += 50) {
                preset.push_back((boost::format("%07d") % offset).str());
            }
        }
    }
    string.clear();
    for (it = preset.begin(); it != preset.end(); ++it) {
        string += "<option value='" + *it + "'";
        if (*it == context.address) {
            string += " selected";
        }
        string += ">" + *it + "</option>";
    }
    boost::replace_first(*response, "<!TA />", string);
    
    boost::replace_first(*response, "<!OF />", context.offset);
    boost::replace_first(*response, "<!_C" + context.count + " />", "selected");
    boost::replace_first(*response, "<!_M" + context.mode + " />", "selected");
    boost::replace_first(*response, "<!_T" + context.text + " />", "selected");
    boost::replace_first(*response, "<!TP />", context.param);
    boost::replace_first(*response, "<!VP />", context.vm);
    boost::replace_first(*response, "<!NP />", context.note);
    boost::replace_first(*response, "<!MP />", context.morse);
    boost::replace_first(*response, "<!DP />", context.digitalker);
    boost::replace_first(*response, "<!CP />", context.camera);
    boost::replace_first(*response, "<!LN />", context.line);
    boost::replace_first(*response, "<!OT />", context.obc);
    boost::replace_first(*response, "<!OH />", context.ohour);
    boost::replace_first(*response, "<!OM />", context.ominute);
    boost::replace_first(*response, "<!OS />", context.osecond);
    boost::replace_first(*response, "<!DH />", context.dhour);
    boost::replace_first(*response, "<!DM />", context.dminute);
    boost::replace_first(*response, "<!DS />", context.dsecond);
    
    string = artsatd::getInstance().getMode();
    if (!(string == "FM" || string == "FM_TEST")) {
        boost::replace_all(*response, "<!_DF />", "disabled");
        boost::replace_all(*response, "<!_RO />", "readonly");
    }
    return;
}

/*private */tgs::TGSError ASDPluginINVADER::queueSTA(std::string const& session)
{
    return artsatd::getInstance().requestCommand(session, "c-c-g-sta");
}

/*private */tgs::TGSError ASDPluginINVADER::queueMRR(std::string const& session)
{
    return artsatd::getInstance().requestCommand(session, "c-p-g-mrr");
}

/*private */tgs::TGSError ASDPluginINVADER::queuePTI(std::string const& session)
{
    return artsatd::getInstance().requestCommand(session, "c-c-g-pti");
}

/*private */tgs::TGSError ASDPluginINVADER::queuePTR(std::string const& session, std::string const& address)
{
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    if (1 <= address.size() && address.size() <= 7 && boost::all(address, boost::is_digit())) {
        error = queuePTR(session, boost::lexical_cast<int>(address));
    }
    else {
        error = tgs::TGSERROR_INVALID_PARAM;
    }
    return error;
}

/*private */tgs::TGSError ASDPluginINVADER::queuePTR(std::string const& session, int address)
{
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    if (0 <= address && address <= 9999999) {
        error = artsatd::getInstance().requestCommand(session, "c-c-g-ptr-" + (boost::format("%07d") % address).str());
    }
    else {
        error = tgs::TGSERROR_INVALID_PARAM;
    }
    return error;
}

/*private */tgs::TGSError ASDPluginINVADER::queueGWABootMode(std::string const& session, std::string const& mode)
{
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    if (1 <= mode.size() && mode.size() <= 3 && boost::all(mode, boost::is_digit())) {
        error = queueGWABootMode(session, boost::lexical_cast<int>(mode));
    }
    else {
        error = tgs::TGSERROR_INVALID_PARAM;
    }
    return error;
}

/*private */tgs::TGSError ASDPluginINVADER::queueGWABootMode(std::string const& session, int mode)
{
    std::string ascii;
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    if (0 <= mode && mode <= 255) {
        ascii = (boost::format("%c") % static_cast<unsigned char>(mode)).str();
        if ((error = queueGWA(session, 524165, ascii, false)) == tgs::TGSERROR_OK) {
            if ((error = queueGWA(session, 524203, ascii, false)) == tgs::TGSERROR_OK) {
                error = queueGWA(session, 524241, ascii, false);
            }
        }
    }
    else {
        error = tgs::TGSERROR_INVALID_PARAM;
    }
    return error;
}

/*private */tgs::TGSError ASDPluginINVADER::queueGRSBootMode(std::string const& session)
{
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    if ((error = queueGRS(session, 524165, 1)) == tgs::TGSERROR_OK) {
        if ((error = queueGRS(session, 524203, 1)) == tgs::TGSERROR_OK) {
            error = queueGRS(session, 524241, 1);
        }
    }
    return error;
}

/*private */tgs::TGSError ASDPluginINVADER::queueGWAText(std::string const& session, std::string const& text, std::string const& data)
{
    TextEnum value;
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    if (text == "x") {
        value = TEXT_X;
    }
    else if (text == "y") {
        value = TEXT_Y;
    }
    else if (text == "z") {
        value = TEXT_Z;
    }
    else if (text == "debug") {
        value = TEXT_DEBUG;
    }
    else {
        error = tgs::TGSERROR_INVALID_PARAM;
    }
    if (error == tgs::TGSERROR_OK) {
        error = queueGWAText(session, value, data);
    }
    return error;
}

/*private */tgs::TGSError ASDPluginINVADER::queueGWAText(std::string const& session, TextEnum text, std::string const& data)
{
    static int const s_address[TEXT_LIMIT] = {
        523904,
        523776,
        523648,
        523520
    };
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    if (text < TEXT_LIMIT && data.size() <= 127) {
        error = queueGWA(session, s_address[text], data, true);
    }
    else {
        error = tgs::TGSERROR_INVALID_PARAM;
    }
    return error;
}

/*private */tgs::TGSError ASDPluginINVADER::queueGRSText(std::string const& session, std::string const& text)
{
    TextEnum value;
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    if (text == "x") {
        value = TEXT_X;
    }
    else if (text == "y") {
        value = TEXT_Y;
    }
    else if (text == "z") {
        value = TEXT_Z;
    }
    else if (text == "debug") {
        value = TEXT_DEBUG;
    }
    else {
        error = tgs::TGSERROR_INVALID_PARAM;
    }
    if (error == tgs::TGSERROR_OK) {
        error = queueGRSText(session, value);
    }
    return error;
}

/*private */tgs::TGSError ASDPluginINVADER::queueGRSText(std::string const& session, TextEnum text)
{
    static int const s_address[TEXT_LIMIT] = {
        523904,
        523776,
        523648,
        523520
    };
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    if (text < TEXT_LIMIT) {
        error = queueGRS(session, s_address[text], 128);
    }
    else {
        error = tgs::TGSERROR_INVALID_PARAM;
    }
    return error;
}

/*private */tgs::TGSError ASDPluginINVADER::queueGWAVM(std::string const& session, std::string const& data)
{
    static boost::xpressive::sregex const s_space(boost::xpressive::sregex::compile(" +"));
    std::string string;
    std::string ascii;
    int i;
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    string = boost::xpressive::regex_replace(data, s_space, "");
    if (string.size() % 2 == 0 && boost::all(string, boost::is_xdigit())) {
        for (i = 0; i < string.size(); i += 2) {
            ascii += (fromHex(string[i + 0]) << 4) | (fromHex(string[i + 1]) << 0);
        }
        if (ascii.size() <= 127) {
            error = queueGWA(session, 523648, ascii, true);
        }
        else {
            error = tgs::TGSERROR_INVALID_PARAM;
        }
    }
    else {
        error = tgs::TGSERROR_INVALID_PARAM;
    }
    return error;
}

/*private */tgs::TGSError ASDPluginINVADER::queueGRSVM(std::string const& session)
{
    return queueGRS(session, 523648, 128);
}

/*private */tgs::TGSError ASDPluginINVADER::queueGWANote(std::string const& session, std::string const& data)
{
    static boost::xpressive::sregex const s_space(boost::xpressive::sregex::compile(" +"));
    std::string string;
    std::vector<std::string> pair;
    NoteEnum note;
    DurationEnum duration;
    QualifierEnum qualifier;
    int i;
    int j;
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    string = boost::xpressive::regex_replace(data, s_space, " ");
    boost::to_lower(string);
    boost::split(pair, string, boost::is_space());
    if (pair.size() % 2 == 0) {
        string.clear();
        for (i = 0; i < pair.size(); i += 2) {
            for (j = 0; j < lengthof(g_note); ++j) {
                if (pair[i + 0] == g_note[j].name) {
                    note = g_note[j].note;
                    break;
                }
            }
            if (j < lengthof(g_note)) {
                for (j = 0; j < lengthof(g_duration); ++j) {
                    if (pair[i + 1] == g_duration[j].name) {
                        duration = g_duration[j].duration;
                        qualifier = g_duration[j].qualifier;
                        break;
                    }
                }
                if (j < lengthof(g_duration)) {
                    string += (boost::format("%c%c%c%c") %
                               static_cast<unsigned char>((note >> 0) & 0xFF) %
                               static_cast<unsigned char>((note >> 8) & 0xFF) %
                               static_cast<unsigned char>(duration) %
                               static_cast<unsigned char>(qualifier)).str();
                }
                else {
                    error = tgs::TGSERROR_INVALID_FORMAT;
                    break;
                }
            }
            else {
                error = tgs::TGSERROR_INVALID_FORMAT;
                break;
            }
        }
        if (error == tgs::TGSERROR_OK) {
            string += (boost::format("%c%c%c%c") %
                       static_cast<unsigned char>((NOTE_END >> 0) & 0xFF) %
                       static_cast<unsigned char>((NOTE_END >> 8) & 0xFF) %
                       static_cast<unsigned char>(0) %
                       static_cast<unsigned char>(0)).str();
            if (string.size() <= 255) {
                error = queueGWA(session, 523264, string, true);
            }
            else {
                error = tgs::TGSERROR_INVALID_FORMAT;
            }
        }
    }
    else {
        error = tgs::TGSERROR_INVALID_PARAM;
    }
    return error;
}

/*private */tgs::TGSError ASDPluginINVADER::queueGRSNote(std::string const& session)
{
    return queueGRS(session, 523264, 256);
}

/*private */tgs::TGSError ASDPluginINVADER::queueGWAMorse(std::string const& session, std::string const& data)
{
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    if (data.size() <= 63) {
        error = queueGWA(session, 523072, data, true);
    }
    else {
        error = tgs::TGSERROR_INVALID_PARAM;
    }
    return error;
}

/*private */tgs::TGSError ASDPluginINVADER::queueGRSMorse(std::string const& session)
{
    return queueGRS(session, 523072, 64);
}

/*private */tgs::TGSError ASDPluginINVADER::queueGWADigitalker(std::string const& session, std::string const& data)
{
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    if (data.size() <= 127) {
        error = queueGWA(session, 523136, data, true);
    }
    else {
        error = tgs::TGSERROR_INVALID_PARAM;
    }
    return error;
}

/*private */tgs::TGSError ASDPluginINVADER::queueGRSDigitalker(std::string const& session)
{
    return queueGRS(session, 523136, 128);
}

/*private */tgs::TGSError ASDPluginINVADER::queueGWACamera(std::string const& session, std::string const& data)
{
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    if (data.size() <= 63) {
        error = queueGWA(session, 523008, data, true);
    }
    else {
        error = tgs::TGSERROR_INVALID_PARAM;
    }
    return error;
}

/*private */tgs::TGSError ASDPluginINVADER::queueGRSCamera(std::string const& session)
{
    return queueGRS(session, 523008, 64);
}

/*private */tgs::TGSError ASDPluginINVADER::queueGWA(std::string const& session, int address, std::string const& data, bool size)
{
    std::string hex;
    int i;
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    if (0 <= address && address <= 999999) {
        if (size) {
            hex += (boost::format("%02x") % data.size()).str();
        }
        for (i = 0; i < data.size(); ++i) {
            hex += (boost::format("%02x") % static_cast<int>(static_cast<unsigned char>(data[i]))).str();
        }
        while (hex.size() > 0) {
            i = std::min(32, static_cast<int>(hex.size()));
            if ((error = artsatd::getInstance().requestCommand(session, "c-c-g-gwa-" + (boost::format("%06d.%d.") % address % (i / 2)).str() + hex.substr(0, i))) != tgs::TGSERROR_OK) {
                break;
            }
            hex = hex.substr(i);// erase?
            address += i / 2;
        }
    }
    else {
        error = tgs::TGSERROR_INVALID_PARAM;
    }
    return error;
}

/*private */tgs::TGSError ASDPluginINVADER::queueGRS(std::string const& session, int address, int size)
{
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    if (0 <= address && address <= 999999 && size > 0) {
        error = artsatd::getInstance().requestCommand(session, "c-c-g-grs-" + (boost::format("%06d.%d") % address % size).str());
    }
    else {
        error = tgs::TGSERROR_INVALID_PARAM;
    }
    return error;
}

/*private */tgs::TGSError ASDPluginINVADER::queueMSR(std::string const& session)
{
    return artsatd::getInstance().requestCommand(session, "c-c-g-msr");
}

/*private */tgs::TGSError ASDPluginINVADER::queueMSD(std::string const& session)
{
    return artsatd::getInstance().requestCommand(session, "c-c-g-msd");
}

/*private */tgs::TGSError ASDPluginINVADER::queueDON(std::string const& session)
{
    return artsatd::getInstance().requestCommand(session, "c-c-g-don");
}

/*private */tgs::TGSError ASDPluginINVADER::queueDOF(std::string const& session)
{
    return artsatd::getInstance().requestCommand(session, "c-c-g-dof");
}

/*private static */int ASDPluginINVADER::fromHex(char param)
{
    int result(-1);
    
    if ('0' <= param && param <= '9') {
        result = param - '0';
    }
    else if ('a' <= param && param <= 'f') {
        result = param - 'a' + 10;
    }
    else if ('A' <= param && param <= 'F') {
        result = param - 'A' + 10;
    }
    return result;
}
