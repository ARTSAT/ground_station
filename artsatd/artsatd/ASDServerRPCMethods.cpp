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
**      Xcode 5.1.1 (Apple LLVM 5.1)
**
**      ASDServerRPCMethods.cpp
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

#include "ASDServerRPCMethods.h"
#include "artsatd.h"

namespace {
    template <class T>
    bool getParam(const std::string& name, T *result, const ASDServerRPC::Params& args) {
        ASDServerRPC::Params::const_iterator it = args.find(name);
        if (it != args.end() && it->second.type() == typeid(T)) {
            *result = boost::get<T>(it->second);
            return true;
        }
        else {
            return false;
        }
    }
    
    template <>
    bool getParam(const std::string& name, int *result, const ASDServerRPC::Params& args) {
        ASDServerRPC::Params::const_iterator it = args.find(name);
        if (it != args.end() && it->second.type() == typeid(double)) {
            *result = boost::get<double>(it->second);
            return true;
        }
        else {
            return false;
        }
    }
}

namespace ASDServerRPC {
    namespace sys {
        Result rpcEcho(const Params& args, Params *result)
        {
            (*result)["args"] = args;
            return RPC_OK;
        }
    }
    
    namespace trans {
        
        Result setManualRotator(const Params& args, Params *result)
        {
            bool flag;
            if (getParam("manual", &flag, args)) {
                tgs::TGSError error = artsatd::getInstance().setManualRotator("!!TODO!!", flag);
                if (error == tgs::TGSERROR_OK) {
                    (*result)["manual"] = artsatd::getInstance().getManualRotator();
                    return RPC_OK;
                }
                else {
                    (*result)["message"] = error.print();
                    return RPC_INTERNAL_ERR;
                }
            }
            else {
                return RPC_WRONG_ARGS;
            }
        }

        Result getManualRotator(const Params& args, Params *result)
        {
            (*result)["manual"] = artsatd::getInstance().getManualRotator();
            return RPC_OK;
        }

        Result setManualTransceiver(const Params& args, Params *result)
        {
            bool flag;
            if (getParam("manual", &flag, args)) {
                tgs::TGSError error = artsatd::getInstance().setManualTransceiver("!!TODO!!", flag);
                if (error == tgs::TGSERROR_OK) {
                    (*result)["manual"] = artsatd::getInstance().getManualTransceiver();
                    return RPC_OK;
                }
                else {
                    (*result)["message"] = error.print();
                    return RPC_INTERNAL_ERR;
                }
            }
            else {
                return RPC_WRONG_ARGS;
            }
        }

        Result getManualTransceiver(const Params& args, Params *result)
        {
            (*result)["manual"] = artsatd::getInstance().getManualTransceiver();
            return RPC_OK;
        }
        
        Result setManualTNC(const Params& args, Params *result)
        {
            bool flag;
            if (getParam("manual", &flag, args)) {
                tgs::TGSError error = artsatd::getInstance().setManualTNC("!!TODO!!", flag);
                if (error == tgs::TGSERROR_OK) {
                    (*result)["manual"] = artsatd::getInstance().getManualTNC();
                    return RPC_OK;
                }
                else {
                    (*result)["message"] = error.print();
                    return RPC_INTERNAL_ERR;
                }
            }
            else {
                return RPC_WRONG_ARGS;
            }
        }
        
        Result getManualTNC(const Params& args, Params *result)
        {
            (*result)["manual"] = artsatd::getInstance().getManualTNC();
            return RPC_OK;
        }
        
        Result getStateRotator(const Params& args, Params *result)
        {
            (*result)["valid"] = artsatd::getInstance().getRotator().isValid();
            return RPC_OK;
        }
        
        Result getStateTransceiver(const Params& args, Params *result)
        {
            (*result)["valid"] = artsatd::getInstance().getTransceiver().isValid();
            return RPC_OK;
        }
        
        Result getStateTNC(const Params& args, Params *result)
        {
            (*result)["valid"] = artsatd::getInstance().getTNC().isValid();
            return RPC_OK;
        }
        
        Result setMode(const Params& args, Params *result)
        {
            std::string mode;
            if (getParam("mode", &mode, args)) {
                tgs::TGSError error = artsatd::getInstance().setMode("!!TODO!!", mode);
                if (error == tgs::TGSERROR_OK) {
                    (*result)["mode"] = artsatd::getInstance().getMode();
                    return RPC_OK;
                }
                else {
                    (*result)["message"] = error.print();
                    return RPC_INTERNAL_ERR;
                }
            }
            else {
                return RPC_WRONG_ARGS;
            }
        }
        
        Result getMode(const Params& args, Params *result)
        {
            (*result)["mode"] = artsatd::getInstance().getMode();
            return RPC_OK;
        }
        
        Result setNorad(const Params& args, Params *result)
        {
            double norad = 0;
            if (getParam("norad", &norad, args)) {
                tgs::TGSError error = artsatd::getInstance().setNORAD("!!TODO!!", norad);
                if (error == tgs::TGSERROR_OK) {
                    (*result)["norad"] = static_cast<double>(artsatd::getInstance().getNORAD());
                    return RPC_OK;
                }
                else {
                    (*result)["message"] = error.print();
                    return RPC_INTERNAL_ERR;
                }
            }
            else {
                return RPC_WRONG_ARGS;
            }
        }

        Result getNorad(const Params& args, Params *result)
        {
            (*result)["norad"] = static_cast<double>(artsatd::getInstance().getNORAD());
            return RPC_OK;
        }
        
        Result getAngleAzimuth(const Params& args, Params *result)
        {
            double value;
            artsatd::getInstance().getSatelliteDirection(&value, NULL);
            (*result)["azimuth"] = value;
            return RPC_OK;
        }
        
        Result getAngleElevation(const Params& args, Params *result)
        {
            double value;
            artsatd::getInstance().getSatelliteDirection(NULL, &value);
            (*result)["elevation"] = value;
            return RPC_OK;
        }
        
        Result getFrequencyBeacon(const Params& args, Params *result)
        {
            double value;
            artsatd::getInstance().getSatelliteFrequency(&value, NULL, NULL);
            (*result)["beacon"] = value;
            return RPC_OK;
        }
        
        Result getFrequencySender(const Params& args, Params *result)
        {
            double value;
            artsatd::getInstance().getSatelliteFrequency(NULL, &value, NULL);
            (*result)["sender"] = value;
            return RPC_OK;
        }
        
        Result getFrequencyReceiver(const Params& args, Params *result)
        {
            double value;
            artsatd::getInstance().getSatelliteFrequency(NULL, NULL, &value);
            (*result)["receiver"] = value;
            return RPC_OK;
        }
        
        Result sendSafeCommand(const Params& args, Params *result)
        {
            static const std::string safe_patterns[] = {
                "c-c-g-sta",
                "c-p-g-mrr",
                "c-c-g-pti",
                "c-c-g-ptr-[0-9a-f]+"
            };
            
            std::string command;
            if (getParam("command", &command, args)) {
                for (int i = 0; i < sizeof(safe_patterns) / sizeof(std::string); ++i) {
                    using namespace boost::xpressive;
                    sregex r = sregex::compile(safe_patterns[i]);
                    if (regex_match(command, r)) {
                        artsatd::getInstance().requestCommand("!!TODO!!", command);
                        (*result)["command"] = command;
                        return RPC_OK;
                    }
                }
                (*result)["message"] = std::string("unsafe command");
                return RPC_WRONG_ARGS;
            }
            else {
                return RPC_WRONG_ARGS;
            }
        }
    }
    
    namespace pass {
        Result getStateNearest(const Params& args, Params *result)
        {
            ir::IRXTime aos, los;
            double mel;
            artsatd::getInstance().getSatelliteAOSLOS(&aos, &los);
            artsatd::getInstance().getSatelliteMEL(&mel);
            (*result)["aos"] = aos.formatISO8601();
            (*result)["los"] = los.formatISO8601();
            (*result)["mel"] = mel;
            return RPC_OK;
        }
    }
    
    namespace db {
        
        Result setName(const Params& args, Params *result)
        {
            double norad;
            std::string name;
            if (getParam("norad", &norad, args) && getParam("name", &name, args)) {
                tgs::TGSPhysicsDatabase db;
                db.open("physics.db");
                
                tgs::TGSError error = db.setName(norad, name);
                if (error == tgs::TGSERROR_OK) {
                    std::string result_name;
                    error = db.getName(norad, &result_name);
                    
                    if (error == tgs::TGSERROR_OK) {
                        (*result)["name"] = result_name;
                        return RPC_OK;
                    }
                    else {
                        (*result)["message"] = error.print();
                        return RPC_INTERNAL_ERR;
                    }
                }
                else {
                    (*result)["message"] = error.print();
                    return RPC_INTERNAL_ERR;
                }
            }
            else {
                return RPC_WRONG_ARGS;
            }
        }
        
        Result getName(const Params& args, Params *result)
        {
            double norad;
            if (getParam("norad", &norad, args)) {
                tgs::TGSPhysicsDatabase db;
                db.open("physics.db");
                
                std::string result_name;
                tgs::TGSError error = db.getName(norad, &result_name);
                
                if (error == tgs::TGSERROR_OK) {
                    (*result)["name"] = result_name;
                    return RPC_OK;
                }
                else {
                    (*result)["message"] = error.print();
                    return RPC_INTERNAL_ERR;
                }
            }
            else {
                return RPC_WRONG_ARGS;
            }
        }
        
        Result setCallsign(const Params& args, Params *result)
        {
            double norad;
            std::string callsign;
            if (getParam("norad", &norad, args) && getParam("callsign", &callsign, args)) {
                tgs::TGSPhysicsDatabase db;
                db.open("physics.db");
                
                tgs::TGSError error = db.setCallsign(norad, callsign);
                if (error == tgs::TGSERROR_OK) {
                    std::string result_callsign;
                    error = db.getCallsign(norad, &result_callsign);
                    
                    if (error == tgs::TGSERROR_OK) {
                        (*result)["callsign"] = result_callsign;
                        return RPC_OK;
                    }
                    else {
                        (*result)["message"] = error.print();
                        return RPC_INTERNAL_ERR;
                    }
                }
                else {
                    (*result)["message"] = error.print();
                    return RPC_INTERNAL_ERR;
                }
            }
            else {
                return RPC_WRONG_ARGS;
            }
        }
        
        Result getCallsign(const Params& args, Params *result)
        {
            double norad;
            if (getParam("norad", &norad, args)) {
                tgs::TGSPhysicsDatabase db;
                db.open("physics.db");
                
                std::string result_callsign;
                tgs::TGSError error = db.getCallsign(norad, &result_callsign);
                
                if (error == tgs::TGSERROR_OK) {
                    (*result)["callsign"] = result_callsign;
                    return RPC_OK;
                }
                else {
                    (*result)["message"] = error.print();
                    return RPC_INTERNAL_ERR;
                }
            }
            else {
                return RPC_WRONG_ARGS;
            }
        }
  
        Result setRadioBeacon(const Params& args, Params *result)
        {
            double norad;
            if (getParam("norad", &norad, args)) {
                tgs::TGSPhysicsDatabase db;
                db.open("physics.db");
                
                tgs::TGSPhysicsDatabase::RadioRec rec;
                db.getRadioBeacon(norad, &rec);
                
                getParam("mode", &rec.mode, args);
                getParam("frequency", &rec.frequency, args);
                getParam("drift", &rec.drift, args);
                
                tgs::TGSError error = db.setRadioBeacon(norad, rec);
                if (error == tgs::TGSERROR_OK) {
                    error = db.getRadioBeacon(norad, &rec);
                    if (error == tgs::TGSERROR_OK) {
                        (*result)["mode"] = rec.mode;
                        (*result)["frequency"] = static_cast<double>(rec.frequency);
                        (*result)["drift"] = static_cast<double>(rec.drift);
                        return RPC_OK;
                    }
                    else {
                        (*result)["message"] = error.print();
                        return RPC_INTERNAL_ERR;
                    }
                }
                else {
                    (*result)["message"] = error.print();
                    return RPC_INTERNAL_ERR;
                }
            }
            else {
                return RPC_WRONG_ARGS;
            }
        }
        
        Result getRadioBeacon(const Params& args, Params *result)
        {
            double norad;
            if (getParam("norad", &norad, args)) {
                tgs::TGSPhysicsDatabase::RadioRec rec;
                tgs::TGSPhysicsDatabase db;
                db.open("physics.db");
                tgs::TGSError error = db.getRadioBeacon(norad, &rec);
                if (error == tgs::TGSERROR_OK) {
                    (*result)["mode"] = rec.mode;
                    (*result)["frequency"] = static_cast<double>(rec.frequency);
                    (*result)["drift"] = static_cast<double>(rec.drift);
                    return RPC_OK;
                }
                else {
                    (*result)["message"] = error.print();
                    return RPC_INTERNAL_ERR;
                }
            }
            else {
                return RPC_WRONG_ARGS;
            }
        }
        
        Result setRadioSender(const Params& args, Params *result)
        {
            double norad;
            if (getParam("norad", &norad, args)) {
                tgs::TGSPhysicsDatabase db;
                db.open("physics.db");
                
                tgs::TGSPhysicsDatabase::RadioRec rec;
                db.getRadioSender(norad, &rec);
                
                getParam("mode", &rec.mode, args);
                getParam("frequency", &rec.frequency, args);
                getParam("drift", &rec.drift, args);
                
                tgs::TGSError error = db.setRadioSender(norad, rec);
                if (error == tgs::TGSERROR_OK) {
                    error = db.getRadioSender(norad, &rec);
                    if (error == tgs::TGSERROR_OK) {
                        (*result)["mode"] = rec.mode;
                        (*result)["frequency"] = static_cast<double>(rec.frequency);
                        (*result)["drift"] = static_cast<double>(rec.drift);
                        return RPC_OK;
                    }
                    else {
                        (*result)["message"] = error.print();
                        return RPC_INTERNAL_ERR;
                    }
                }
                else {
                    (*result)["message"] = error.print();
                    return RPC_INTERNAL_ERR;
                }
            }
            else {
                return RPC_WRONG_ARGS;
            }
        }
        
        Result getRadioSender(const Params& args, Params *result)
        {
            double norad;
            if (getParam("norad", &norad, args)) {
                tgs::TGSPhysicsDatabase::RadioRec rec;
                tgs::TGSPhysicsDatabase db;
                db.open("physics.db");
                tgs::TGSError error = db.getRadioSender(norad, &rec);
                if (error == tgs::TGSERROR_OK) {
                    (*result)["mode"] = rec.mode;
                    (*result)["frequency"] = static_cast<double>(rec.frequency);
                    (*result)["drift"] = static_cast<double>(rec.drift);
                    return RPC_OK;
                }
                else {
                    (*result)["message"] = error.print();
                    return RPC_INTERNAL_ERR;
                }
            }
            else {
                return RPC_WRONG_ARGS;
            }
        }
        
        Result setRadioReceiver(const Params& args, Params *result)
        {
            double norad;
            if (getParam("norad", &norad, args)) {
                tgs::TGSPhysicsDatabase db;
                db.open("physics.db");
                
                tgs::TGSPhysicsDatabase::RadioRec rec;
                db.getRadioReceiver(norad, &rec);
                
                getParam("mode", &rec.mode, args);
                getParam("frequency", &rec.frequency, args);
                getParam("drift", &rec.drift, args);
                
                tgs::TGSError error = db.setRadioReceiver(norad, rec);
                if (error == tgs::TGSERROR_OK) {
                    error = db.getRadioReceiver(norad, &rec);
                    if (error == tgs::TGSERROR_OK) {
                        (*result)["mode"] = rec.mode;
                        (*result)["frequency"] = static_cast<double>(rec.frequency);
                        (*result)["drift"] = static_cast<double>(rec.drift);
                        return RPC_OK;
                    }
                    else {
                        (*result)["message"] = error.print();
                        return RPC_INTERNAL_ERR;
                    }
                }
                else {
                    (*result)["message"] = error.print();
                    return RPC_INTERNAL_ERR;
                }
            }
            else {
                return RPC_WRONG_ARGS;
            }
        }
        
        Result getRadioReceiver(const Params& args, Params *result)
        {
            double norad;
            if (getParam("norad", &norad, args)) {
                tgs::TGSPhysicsDatabase::RadioRec rec;
                tgs::TGSPhysicsDatabase db;
                db.open("physics.db");
                tgs::TGSError error = db.getRadioReceiver(norad, &rec);
                if (error == tgs::TGSERROR_OK) {
                    (*result)["mode"] = rec.mode;
                    (*result)["frequency"] = static_cast<double>(rec.frequency);
                    (*result)["drift"] = static_cast<double>(rec.drift);
                    return RPC_OK;
                }
                else {
                    (*result)["message"] = error.print();
                    return RPC_INTERNAL_ERR;
                }
            }
            else {
                return RPC_WRONG_ARGS;
            }
        }
        
        Result setOrbitData(const Params& args, Params *result)
        {
            std::string time_str, name, one, two;
            Params tle;
            if (getParam("time", &time_str, args) &&
                getParam("tle",  &tle,  args) &&
                getParam("name", &name, tle)  &&
                getParam("one",  &one,  tle)  &&
                getParam("two",  &two,  tle)) {
                
                ir::IRXTime time;
                tgs::TLERec tle;
                
                if (time.parseISO8601(time_str)) {
                    (*result)["message"] = "time parse error";
                    return RPC_WRONG_ARGS;
                }
                
                name.copy(tle.name, sizeof(tle.name));
                name.copy(tle.one, sizeof(tle.one));
                name.copy(tle.two, sizeof(tle.two));
                
                tgs::TGSPhysicsDatabase db;
                db.open("physics.db");
                tgs::TGSError error = db.setOrbitData(tle, time);
                if (error == tgs::TGSERROR_OK) {
                    // TODO: return value
                    return RPC_OK;
                }
                else {
                    (*result)["message"] = error.print();
                    return RPC_INTERNAL_ERR;
                }
            }
            else {
                return RPC_WRONG_ARGS;
            }
        }
        
        Result getOrbitData(const Params& args, Params *result)
        {
            double norad;
            if (getParam("norad", &norad, args)) {
                tgs::TGSPhysicsDatabase::RadioRec rec;
                tgs::TGSPhysicsDatabase db;
                db.open("physics.db");
                
                ir::IRXTime time;
                tgs::TLERec tle;
                tgs::TGSError error = db.getOrbitData(norad, &tle, &time);
                if (error == tgs::TGSERROR_OK) {
                    (*result)["time"] = time.formatISO8601();
                    Params tle_res;
                    tle_res["name"] = std::string(tle.name);
                    tle_res["one"] = std::string(tle.one);
                    tle_res["two"] = std::string(tle.two);
                    (*result)["tle"] = tle_res;
                    return RPC_OK;
                }
                else {
                    (*result)["message"] = error.print();
                    return RPC_INTERNAL_ERR;
                }
            }
            else {
                return RPC_WRONG_ARGS;
            }
        }
        
        Result getCount(const Params& args, Params *result)
        {
            tgs::TGSPhysicsDatabase::RadioRec rec;
            tgs::TGSPhysicsDatabase db;
            db.open("physics.db");
            
            int count;
            tgs::TGSError error = db.getCount(&count);
            if (error == tgs::TGSERROR_OK) {
                (*result)["count"] = static_cast<double>(count);
                return RPC_OK;
            }
            else {
                (*result)["message"] = error.print();
                return RPC_INTERNAL_ERR;
            }
        }
        
        namespace {
            void fieldToVariant(const tgs::TGSPhysicsDatabase::FieldRec &field, Params *result)
            {
                (*result)["name"] = field.name;
                (*result)["callsign"] = field.callsign;
                
                Params beacon;
                beacon["mode"] = field.beacon.mode;
                beacon["frequency"] = static_cast<double>(field.beacon.frequency);
                beacon["drift"] = static_cast<double>(field.beacon.drift);
                (*result)["beacon"] = beacon;
                
                Params sender;
                sender["mode"] = field.sender.mode;
                sender["frequency"] = static_cast<double>(field.sender.frequency);
                sender["drift"] = static_cast<double>(field.sender.drift);
                (*result)["sender"] = sender;
                
                Params receiver;
                receiver["mode"] = field.receiver.mode;
                receiver["frequency"] = static_cast<double>(field.receiver.frequency);
                receiver["drift"] = static_cast<double>(field.receiver.drift);
                (*result)["receiver"] = receiver;
                
                (*result)["time"] = field.time.formatISO8601();
                Params tle;
                tle["name"] = std::string(field.tle.name);
                tle["one"] = std::string(field.tle.one);
                tle["two"] = std::string(field.tle.two);
                (*result)["tle"] = tle;
            }
            
        }
        
        Result getField(const Params& args, Params *result)
        {
            tgs::TGSPhysicsDatabase::RadioRec rec;
            tgs::TGSPhysicsDatabase db;
            db.open("physics.db");
            
            double norad, limit, offset = 0.0;
            if (getParam("norad", &norad, args)) {
                tgs::TGSPhysicsDatabase::FieldRec field;
                tgs::TGSError error = db.getField(norad, &field);
                if (error == tgs::TGSERROR_OK) {
                    fieldToVariant(field, result);
                    return RPC_OK;
                }
                else {
                    (*result)["message"] = error.print();
                    return RPC_INTERNAL_ERR;
                }
            }
            else if (getParam("limit", &limit, args)) {
                getParam("offset", &offset, args);
                typedef std::vector<tgs::TGSPhysicsDatabase::FieldRec> FieldVec;
                FieldVec fields;
                tgs::TGSError error = db.getField(limit, offset, &fields);
                if (error == tgs::TGSERROR_OK) {
                    std::list<Variant> list;
                    for (FieldVec::iterator it = fields.begin(); it != fields.end(); ++it) {
                        Params field;
                        fieldToVariant(*it, &field);
                        list.push_back(field);
                    }
                    (*result)["fields"] = list;
                    
                    return RPC_OK;
                }
                else {
                    (*result)["message"] = error.print();
                    return RPC_INTERNAL_ERR;
                }
            }
            else {
                return RPC_WRONG_ARGS;
            }
        }
        
        Result getFieldByName(const Params& args, Params *result)
        {
            tgs::TGSPhysicsDatabase::RadioRec rec;
            tgs::TGSPhysicsDatabase db;
            db.open("physics.db");
            
            std::string name;
            if (getParam("name", &name, args)) {
                typedef std::vector<tgs::TGSPhysicsDatabase::FieldRec> FieldVec;
                FieldVec fields;
                tgs::TGSError error = db.getFieldByName(name, &fields);
                if (error == tgs::TGSERROR_OK) {
                    std::list<Variant> list;
                    for (FieldVec::iterator it = fields.begin(); it != fields.end(); ++it) {
                        Params field;
                        fieldToVariant(*it, &field);
                        list.push_back(field);
                    }
                    (*result)["fields"] = list;
                    
                    return RPC_OK;
                }
                else {
                    (*result)["message"] = error.print();
                    return RPC_INTERNAL_ERR;
                }
            }
            else {
                return RPC_WRONG_ARGS;
            }
        }
        
        Result getFieldByCallsign(const Params& args, Params *result)
        {
            tgs::TGSPhysicsDatabase::RadioRec rec;
            tgs::TGSPhysicsDatabase db;
            db.open("physics.db");
            
            std::string callsign;
            if (getParam("callsign", &callsign, args)) {
                typedef std::vector<tgs::TGSPhysicsDatabase::FieldRec> FieldVec;
                FieldVec fields;
                tgs::TGSError error = db.getFieldByCallsign(callsign, &fields);
                if (error == tgs::TGSERROR_OK) {
                    std::list<Variant> list;
                    for (FieldVec::iterator it = fields.begin(); it != fields.end(); ++it) {
                        Params field;
                        fieldToVariant(*it, &field);
                        list.push_back(field);
                    }
                    (*result)["fields"] = list;
                    
                    return RPC_OK;
                }
                else {
                    (*result)["message"] = error.print();
                    return RPC_INTERNAL_ERR;
                }
            }
            else {
                return RPC_WRONG_ARGS;
            }
        }
        
        Result getNoradByName(const Params& args, Params *result)
        {
            tgs::TGSPhysicsDatabase::RadioRec rec;
            tgs::TGSPhysicsDatabase db;
            db.open("physics.db");
            
            std::string name;
            if (getParam("name", &name, args)) {
                typedef std::vector<int> NoradVec;
                NoradVec norads;
                tgs::TGSError error = db.getNORADByName(name, &norads);
                if (error == tgs::TGSERROR_OK) {
                    std::list<Variant> list;
                    for (NoradVec::iterator it = norads.begin(); it != norads.end(); ++it) {
                        list.push_back(static_cast<double>(*it));
                    }
                    (*result)["norads"] = list;
                    
                    return RPC_OK;
                }
                else {
                    (*result)["message"] = error.print();
                    return RPC_INTERNAL_ERR;
                }
            }
            else {
                return RPC_WRONG_ARGS;
            }
        }
        
        Result getNoradByCallsign(const Params& args, Params *result)
        {
            tgs::TGSPhysicsDatabase::RadioRec rec;
            tgs::TGSPhysicsDatabase db;
            db.open("physics.db");
            
            std::string callsign;
            if (getParam("callsign", &callsign, args)) {
                typedef std::vector<int> NoradVec;
                NoradVec norads;
                tgs::TGSError error = db.getNORADByCallsign(callsign, &norads);
                if (error == tgs::TGSERROR_OK) {
                    std::list<Variant> list;
                    for (NoradVec::iterator it = norads.begin(); it != norads.end(); ++it) {
                        list.push_back(static_cast<double>(*it));
                    }
                    (*result)["norads"] = list;
                    
                    return RPC_OK;
                }
                else {
                    (*result)["message"] = error.print();
                    return RPC_INTERNAL_ERR;
                }
            }
            else {
                return RPC_WRONG_ARGS;
            }
        }
    }
};
