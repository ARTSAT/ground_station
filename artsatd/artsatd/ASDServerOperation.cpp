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
**      ASDServerOperation.cpp
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

#include "ASDServerOperation.h"
#include "ASDServerRPCMethods.h"
#include "stringbuffer.h"
#include "writer.h"
#include "artsatd.h"

/*public */ASDServerOperation::ASDServerOperation(void)
{
    registerRpcMethod("sys.rpcEcho",                &ASDServerRPC::sys::rpcEcho);
    
    registerRpcMethod("trans.setActive",            &ASDServerRPC::trans::setActive);
    registerRpcMethod("trans.getActive",            &ASDServerRPC::trans::getActive);
    registerRpcMethod("trans.setManualSatellite",   &ASDServerRPC::trans::setManualSatellite);
    registerRpcMethod("trans.getManualSatellite",   &ASDServerRPC::trans::getManualSatellite);
    registerRpcMethod("trans.setManualRotator",     &ASDServerRPC::trans::setManualRotator);
    registerRpcMethod("trans.getManualRotator",     &ASDServerRPC::trans::getManualRotator);
    registerRpcMethod("trans.setManualTransceiver", &ASDServerRPC::trans::setManualTransceiver);
    registerRpcMethod("trans.getManualTransceiver", &ASDServerRPC::trans::getManualTransceiver);
    registerRpcMethod("trans.setManualTNC",         &ASDServerRPC::trans::setManualTNC);
    registerRpcMethod("trans.getManualTNC",         &ASDServerRPC::trans::getManualTNC);
    registerRpcMethod("trans.setMode",              &ASDServerRPC::trans::setMode);
    registerRpcMethod("trans.getMode",              &ASDServerRPC::trans::getMode);
    registerRpcMethod("trans.setNorad",             &ASDServerRPC::trans::setNorad);
    registerRpcMethod("trans.getNorad",             &ASDServerRPC::trans::getNorad);
    registerRpcMethod("trans.getAngleAzimuth",      &ASDServerRPC::trans::getAngleAzimuth);
    registerRpcMethod("trans.getAngleElevation",    &ASDServerRPC::trans::getAngleElevation);
    registerRpcMethod("trans.getFrequencyBeacon",   &ASDServerRPC::trans::getFrequencyBeacon);
    registerRpcMethod("trans.getFrequencySender",   &ASDServerRPC::trans::getFrequencySender);
    registerRpcMethod("trans.getFrequencyReceiver", &ASDServerRPC::trans::getFrequencyReceiver);
    registerRpcMethod("trans.getError",             &ASDServerRPC::trans::getError);
}

/*public virtual */ASDServerOperation::~ASDServerOperation(void)
{
}

/*private virtual */tgs::TGSError ASDServerOperation::onRequest(std::string const& path, std::map<std::string, std::string>& query, int* status, std::string* response)
{
    std::string mode;
    std::string norad;
    char azimuth[64];
    char elevation[64];
    char beacon[64];
    char sender[64];
    char receiver[64];
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    if (!(query["mode"] == "Idle" || query["mode"] == "CW" || query["mode"] == "FM" || query["mode"] == "list")) {
        mode = artsatd::getInstance().getMode();
        if (mode == "CW") {
            query["mode"] = "CW";
        }
        else if (mode == "FM") {
            query["mode"] = "FM";
        }
        else {
            query["mode"] = "Idle";
        }
    }
    if (query["mode"] == "CW") {
        artsatd::getInstance().setMode("CW");
    }
    else if (query["mode"] == "FM") {
        artsatd::getInstance().setMode("FM");
        if (query["command"] == "sta") {
            artsatd::getInstance().queueCommand("c-c-g-sta");
        }
        else if (query["command"] == "aef") {
            artsatd::getInstance().queueCommand("c-p-g-aef");
        }
        else if (query["command"] == "mrr") {
            artsatd::getInstance().queueCommand("c-p-g-mrr");
        }
        else if (query["command"] == "hds") {
            artsatd::getInstance().queueCommand("c-p-g-hds");
        }
        else if (query["command"] == "pti") {
            artsatd::getInstance().queueCommand("c-c-g-pti");
        }
        else if (query["command"] == "pta_0015023") {
            artsatd::getInstance().queueCommand("c-c-g-pta-0015023");
        }
        else if (query["command"] == "pta_0015053") {
            artsatd::getInstance().queueCommand("c-c-g-pta-0015053");
        }
        else if (query["command"] == "pta_0015083") {
            artsatd::getInstance().queueCommand("c-c-g-pta-0015083");
        }
        else if (query["command"] == "pta_0015119") {
            artsatd::getInstance().queueCommand("c-c-g-pta-0015119");
        }
    }
    else {
        artsatd::getInstance().setMode("Idle");
    }
    norad = boost::lexical_cast<std::string>(artsatd::getInstance().getNorad());
    if (query["norad"].empty()) {
        query["norad"] = norad;
    }
    else {
        artsatd::getInstance().setNorad(boost::lexical_cast<int>(query["norad"]));
    }
    *status = 200;
    *response += ""
    "<html>"
        "<head>"
            "<meta http-equiv='refresh' content='" + std::string("5") + ";url=?mode=" + query["mode"] + "&norad=" + query["norad"] + "'/>"
            "<style><!--"
                "th {"
                    "text-align: left;"
                    "background-color: #eeeeff;"
                    "padding: 2px 6px;"
                "}"
                "td {"
                    "background-color: #ccccff;"
                    "padding: 2px 6px;"
                "}"
                "table form {"
                    "display: inline;"
                "}"
                "p {"
                    "font-size: 10pt;"
                "}"
            "--></style>"
        "</head>"
        "<body>"
            "<h1>ARTSAT Tamabi GS Tester 2.0<h1/>";
    ///////////////////////////////////////////////
    if (path == "/") {
        *response += ""
            "<form action='' method='get'>"
                "<input type='submit' name='mode' value='Idle'/>"
                "<input type='submit' name='mode' value='CW'/>"
                "<input type='submit' name='mode' value='FM'/>"
                "<input type='hidden' name='norad' value='" + query["norad"] + "'/>"
            "</form>";
        *response += ""
            "NORAD:"
            "<form action='' method='get'>"
                "<input type='hidden' name='mode' value='" + query["mode"] + "'/>"
                "<input type='text' name='norad' value='" + query["norad"] + "' size='50' maxlength='39'/>"
            "</form>";
        snprintf(azimuth, sizeof(azimuth), "%+.6lf deg", artsatd::getInstance().getAngleAzimuth());
        snprintf(elevation, sizeof(elevation), "%+.6lf deg", artsatd::getInstance().getAngleElevation());
        snprintf(beacon, sizeof(beacon), "%.6lf MHz", artsatd::getInstance().getFrequencyBeacon() / 1000000.0);
        snprintf(sender, sizeof(sender), "%.6lf MHz", artsatd::getInstance().getFrequencySender() / 1000000.0);
        snprintf(receiver, sizeof(receiver), "%.6lf MHz", artsatd::getInstance().getFrequencyReceiver() / 1000000.0);
        if (query["mode"] == "CW") {
            *response += ""
            "<table>"
                "<tr>"
                    "<th>NORAD</th>"
                    "<td>" + norad + "</td>"
                "</tr>"
                "<tr>"
                    "<th>Time</th>"
                    "<td>" + ir::IRXTime::currentTime().formatYMD() + "</td>"
                "</tr>"
                "<tr>"
                    "<th>Mode</th>"
                    "<td>" + query["mode"] + "</td>"
                "</tr>"
                "<tr>"
                    "<th>Angle (Azimuth)</th>"
                    "<td>" + azimuth + "</td>"
                "</tr>"
                "<tr>"
                    "<th>Angle (Elevation)</th>"
                    "<td>" + elevation + "</td>"
                "</tr>"
                "<tr>"
                    "<th>Frequency (Receiver)</th>"
                    "<td>" + beacon + "</td>"
                "</tr>"
            "</table>";
        }
        else if (query["mode"] == "FM") {
            *response += ""
            "<table>"
                "<tr>"
                    "<th>NORAD</th>"
                    "<td>" + norad + "</td>"
                "</tr>"
                "<tr>"
                    "<th>Time</th>"
                    "<td>" + ir::IRXTime::currentTime().formatYMD() + "</td>"
                "</tr>"
                "<tr>"
                    "<th>Mode</th>"
                    "<td>" + query["mode"] + "</td>"
                "</tr>"
                "<tr>"
                    "<th>Angle (Azimuth)</th>"
                    "<td>" + azimuth + "</td>"
                "</tr>"
                "<tr>"
                    "<th>Angle (Elevation)</th>"
                    "<td>" + elevation + "</td>"
                "</tr>"
                "<tr>"
                    "<th>Frequency (Receiver)</th>"
                    "<td>" + receiver + "</td>"
                "</tr>"
                "<tr>"
                    "<th>Frequency (Sender)</th>"
                    "<td>" + sender + "</td>"
                "</tr>"
                "<tr>"
                    "<th rowspan='9'>Command</th>"
                    "<td>"
                        "<form action='' method='get'>"
                            "<input type='hidden' name='mode' value='" + query["mode"] + "'/>"
                            "<input type='submit' name='command' value='sta'/>"
                            "<input type='hidden' name='norad' value='" + query["norad"] + "'/>"
                        "</form>"
                    "</td>"
                "</tr>"
                "<tr>"
                    "<td>"
                        "<form action='' method='get'>"
                            "<input type='hidden' name='mode' value='" + query["mode"] + "'/>"
                            "<input type='submit' name='command' value='aef'/>"
                            "<input type='hidden' name='norad' value='" + query["norad"] + "'/>"
                        "</form>"
                    "</td>"
                "</tr>"
                "<tr>"
                    "<td>"
                        "<form action='' method='get'>"
                            "<input type='hidden' name='mode' value='" + query["mode"] + "'/>"
                            "<input type='submit' name='command' value='mrr'/>"
                            "<input type='hidden' name='norad' value='" + query["norad"] + "'/>"
                        "</form>"
                    "</td>"
                "</tr>"
                "<tr>"
                    "<td>"
                        "<form action='' method='get'>"
                            "<input type='hidden' name='mode' value='" + query["mode"] + "'/>"
                            "<input type='submit' name='command' value='hds'/>"
                            "<input type='hidden' name='norad' value='" + query["norad"] + "'/>"
                        "</form>"
                    "</td>"
                "</tr>"
                "<tr>"
                    "<td>"
                        "<form action='' method='get'>"
                            "<input type='hidden' name='mode' value='" + query["mode"] + "'/>"
                            "<input type='submit' name='command' value='pti'/>"
                            "<input type='hidden' name='norad' value='" + query["norad"] + "'/>"
                        "</form>"
                    "</td>"
                "</tr>"
                "<tr>"
                    "<td>"
                        "<form action='' method='get'>"
                            "<input type='hidden' name='mode' value='" + query["mode"] + "'/>"
                            "<input type='submit' name='command' value='pta_0015023'/>"
                            "<input type='hidden' name='norad' value='" + query["norad"] + "'/>"
                        "</form>"
                    "</td>"
                "</tr>"
                "<tr>"
                    "<td>"
                        "<form action='' method='get'>"
                            "<input type='hidden' name='mode' value='" + query["mode"] + "'/>"
                            "<input type='submit' name='command' value='pta_0015053'/>"
                            "<input type='hidden' name='norad' value='" + query["norad"] + "'/>"
                        "</form>"
                    "</td>"
                "</tr>"
                "<tr>"
                    "<td>"
                        "<form action='' method='get'>"
                            "<input type='hidden' name='mode' value='" + query["mode"] + "'/>"
                            "<input type='submit' name='command' value='pta_0015083'/>"
                            "<input type='hidden' name='norad' value='" + query["norad"] + "'/>"
                        "</form>"
                    "</td>"
                "</tr>"
                "<tr>"
                    "<td>"
                        "<form action='' method='get'>"
                            "<input type='hidden' name='mode' value='" + query["mode"] + "'/>"
                            "<input type='submit' name='command' value='pta_0015119'/>"
                            "<input type='hidden' name='norad' value='" + query["norad"] + "'/>"
                        "</form>"
                    "</td>"
                "</tr>"
            "</table>";
        }
        else {
            *response += ""
            "<table>"
                "<tr>"
                    "<th>NORAD</th>"
                    "<td>" + norad + "</td>"
                "</tr>"
                "<tr>"
                    "<th>Time</th>"
                    "<td>" + ir::IRXTime::currentTime().formatYMD() + "</td>"
                "</tr>"
                "<tr>"
                    "<th>Mode</th>"
                    "<td>" + query["mode"] + "</td>"
                "</tr>"
                "<tr>"
                    "<th>Angle (Azimuth)</th>"
                    "<td>" + azimuth + "</td>"
                "</tr>"
                "<tr>"
                    "<th>Angle (Elevation)</th>"
                    "<td>" + elevation + "</td>"
                "</tr>"
            "</table>";
        }
    }
    else if (path == "/database") {
        *response += "<h1>Database</h1>";
        
        tgs::TGSPhysicsDatabase db;
        db.open("physics.db");
        
        std::vector<int> result;
        tgs::TGSPhysicsDatabase::FieldRec temp;
        std::vector<tgs::TGSPhysicsDatabase::FieldRec> field;
        
        if (query["mode"] == "list") {
            if (query.find("name") != query.end()) {
                db.getNoradByName(query["name"], &result);
                for (std::vector<int>::iterator it = result.begin(); it != result.end(); ++it) {
                    db.getField(*it, &temp);
                    field.push_back(temp);
                }
            }
            else if (query.find("callsign") != query.end()) {
                db.getNoradByCallsign(query["callsign"], &result);
                for (std::vector<int>::iterator it = result.begin(); it != result.end(); ++it) {
                    db.getField(*it, &temp);
                    field.push_back(temp);
                }
            }
            else {
                db.getField(-1, -1, &field);
            }
        }
        else if (query["mode"] == "set") {
            if (query.find("id") != query.end()) {
                int norad = boost::lexical_cast<int>(query["id"]);
                field.resize(1);
                tgs::TGSPhysicsDatabase::FieldRec &frontField = field.front();
                frontField.norad = norad;
                frontField.beacon.frequency = 0;
                frontField.beacon.drift = 0;
                frontField.sender.frequency = 0;
                frontField.sender.drift = 0;
                frontField.receiver.frequency = 0;
                frontField.receiver.drift = 0;
                frontField.time = ir::IRXTime::currentTime();
                
                db.getField(norad, &field.front());
                
                // "Name"
                if (query.find("Name") != query.end()) {
                    db.setName(norad, query["Name"]);
                }
                
                // "CallSign"
                if (query.find("CallSign") != query.end()) {
                    db.setCallsign(norad, query["CallSign"]);
                }
                
                // "Beacon/Mode"
                // "Beacon/Frequency"
                // "Beacon/Drift"
                
                if (query.find("Beacon") != query.end()) {
                    std::vector<std::string> args;
                    boost::split(args, query["Beacon"], boost::is_any_of("_"));
                    if (args.size() == 3) {
                        frontField.beacon.mode = args[0];
                        frontField.beacon.frequency = boost::lexical_cast<int>(args[1]);
                        frontField.beacon.drift = boost::lexical_cast<int>(args[2]);
                        db.setRadioBeacon(norad, frontField.beacon);
                    }
                }
                
                // "Sender/Mode"
                // "Sender/Frequency"
                // "Sender/Drift"
                
                if (query.find("Sender") != query.end()) {
                    std::vector<std::string> args;
                    boost::split(args, query["Sender"], boost::is_any_of("_"));
                    if (args.size() == 3) {
                        frontField.sender.mode = args[0];
                        frontField.sender.frequency = boost::lexical_cast<int>(args[1]);
                        frontField.sender.drift = boost::lexical_cast<int>(args[2]);
                        db.setRadioSender(norad, frontField.sender);
                    }
                }
                
                // "Receiver/Mode"
                // "Receiver/Frequency"
                // "Receiver/Drift"
                
                if (query.find("Receiver") != query.end()) {
                    std::vector<std::string> args;
                    boost::split(args, query["Receiver"], boost::is_any_of("_"));
                    if (args.size() == 3) {
                        frontField.receiver.mode = args[0];
                        frontField.receiver.frequency = boost::lexical_cast<int>(args[1]);
                        frontField.receiver.drift = boost::lexical_cast<int>(args[2]);
                        db.setRadioReceiver(norad, frontField.receiver);
                    }
                }
                
                // "Time"
                if (query.find("Time") != query.end()) {
                    frontField.time.parseISO8601(query["Time"]);
                }
                
                // "TLE"
                if (query.find("TLE") != query.end()) {
                    std::vector<std::string> args;
                    boost::split(args, query["TLE"], boost::is_any_of("_"));
                    std::cout << query["TLE"] << std::endl;
                    if (args.size() == 3) {
                        args[0].copy(frontField.tle.name, sizeof(frontField.tle.name));
                        args[1].copy(frontField.tle.one, sizeof(frontField.tle.one));
                        args[2].copy(frontField.tle.two, sizeof(frontField.tle.two));
                        std::cout << frontField.tle.name << std::endl;
                        std::cout << frontField.tle.one << std::endl;
                        std::cout << frontField.tle.two << std::endl;
                    }
                }
                
                std::cout << db.setOrbitData(frontField.tle, ir::IRXTime::currentTime()).print();
                
                db.getField(norad, &field.front());
            }
        }
        else if (query["mode"] == "get") {
            if (query.find("id") != query.end()) {
                int norad = boost::lexical_cast<int>(query["id"]);
                field.resize(1);
                db.getField(norad, &field.front());
            }
        }
        
        // http://localhost:16780/database?mode=set&id=0&Time=20140227T072917&TLE=INVADER%28PRE02%29_1%2000000U%2000000A%20%20%2014058%2E80299768%20%20%2E00000000%20%2000000%2D0%20%2010000%2D3%200%2000010_2%2000000%20%2065%2E0000%20%2053%2E0000%200021000%20%2061%2E4000%20203%2E1000%2015%2E63328135%2000000
        
        *response += ""
        "<table>"
        "<th>"
        "<td>Norad</td>"
        "<td>CallSign</td>"
        "<td>Beacon/Mode</td>"
        "<td>Beacon/Frequency</td>"
        "<td>Beacon/Drift</td>"
        "<td>Sender/Mode</td>"
        "<td>Sender/Frequency</td>"
        "<td>Sender/Drift</td>"
        "<td>Receiver/Mode</td>"
        "<td>Receiver/Frequency</td>"
        "<td>Receiver/Drift</td>"
        "<td>Time</td>"
        "<td>TLE</td>"
        "</th>";
        
        for (std::vector<tgs::TGSPhysicsDatabase::FieldRec>::iterator it = field.begin(); it != field.end(); ++it) {
            *response += ""
            "<tr>"
            "<td>"
            + it->name +
            "</td>"
            "<td>"
            + boost::lexical_cast<std::string>(it->norad) +
            "</td>"
            "<td>"
            + it->callsign +
            "</td>"
            "<td>"
            + it->beacon.mode +
            "</td>"
            "<td>"
            + boost::lexical_cast<std::string>(it->beacon.frequency) +
            "</td>"
            "<td>"
            + boost::lexical_cast<std::string>(it->beacon.drift) +
            "</td>"
            "<td>"
            + it->sender.mode +
            "</td>"
            "<td>"
            + boost::lexical_cast<std::string>(it->sender.frequency) +
            "</td>"
            "<td>"
            + boost::lexical_cast<std::string>(it->sender.drift) +
            "</td>"
            "<td>"
            + it->receiver.mode +
            "</td>"
            "<td>"
            + boost::lexical_cast<std::string>(it->receiver.frequency) +
            "</td>"
            "<td>"
            + boost::lexical_cast<std::string>(it->receiver.drift) +
            "</td>"
            "<td>"
            + it->time.formatISO8601() +
            "</td>"
            "<td>"
            + it->tle.one + "<BR>" + it->tle.two +
            "</td>"
            "</tr>";
        }
        *response += "</table>";
    }
    /*
    *response += ""
            "<p>" + artsatd::getInstance().getConsole() + "</p>";
     */
    ///////////////////////////////////////////////
    *response += ""
        "</body>"
    "</html>";
    return error;
}

/*private virtual */tgs::TGSError ASDServerOperation::onJsonRpcRequest(std::string const& body, int* status, std::string* response)
{
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    rapidjson::Document req;
    req.Parse<0>(body.c_str());
    
    rapidjson::Document res;
    res.SetObject();
    res.AddMember("jsonrpc", "2.0", res.GetAllocator());
    
    rapidjson::Value result_obj;
    
    int req_id = -1;
    int error_code = 0;
    std::string error_msg = "";
    
    if (req.HasParseError()) {
        
        error_code = -32700;
        error_msg = "Parse error";
        
        *status = 400;
    }
    else {
        if (req["id"].IsInt()) {
            req_id = req["id"].GetInt();
            
            if (req_id >= 0 && req["method"].IsString()) {
                std::string method = req["method"].GetString();
                if (_methods.find(method) != _methods.end()) {
                    ASDServerRPC::Params args, result;
                    
                    if (req.HasMember("params") && req["params"].IsObject()) {
                        ASDServerRPC::Variant variant = ASDServerRPC::toVariant(req["params"]);
                        args = boost::get<ASDServerRPC::Params>(variant);
                    }
                    
                    ASDServerRPC::Result rpc_result = _methods[method](args, &result);
                    if (rpc_result == ASDServerRPC::RPC_OK) {
                        *status = 200;
                    }
                    else if (rpc_result == ASDServerRPC::RPC_WRONG_ARGS) {
                        error_code = -32602;
                        error_msg = "Invalid params";
                        *status = 400;
                    }
                    else {
                        error_code = -32603;
                        error_msg = "Internal error";
                        *status = 500;
                    }
                    result_obj.SetObject();
                    ASDServerRPC::toJson(result, &result_obj, res.GetAllocator());
                }
                else {
                    error_code = -32601;
                    error_msg = "Method not found";
                    *status = 400;
                }
            }
            else {
                error_code = -32600;
                error_msg = "Invalid Request";
                *status = 400;
            }
        }
        else {
            error_code = -32600;
            error_msg = "Invalid Request";
            *status = 400;
        }
    }
    
    if (req_id == -1) {
        rapidjson::Value null;
        null.SetNull();
        res.AddMember("id", null, res.GetAllocator());
    }
    else {
        res.AddMember("id", req_id, res.GetAllocator());
    }
    
    if (error_code == 0) {
        res.AddMember("result", result_obj, res.GetAllocator());
    }
    else {
        rapidjson::Value res_error;
        res_error.SetObject();
        res_error.AddMember("code", error_code, res.GetAllocator());
        res_error.AddMember("message", error_msg.c_str(), res.GetAllocator());
        if (result_obj.IsObject() && result_obj.MemberBegin() != result_obj.MemberEnd()) {
            res_error.AddMember("data", result_obj, res.GetAllocator());
        }
        res.AddMember("error", res_error, res.GetAllocator());
    }
    
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    res.Accept(writer);
    
    *response += buffer.GetString();
    
    return error;
}

/*private */void ASDServerOperation::registerRpcMethod(std::string const& name, ASDServerRPC::Method const& func)
{
    _methods[name] = func;
    return;
}
