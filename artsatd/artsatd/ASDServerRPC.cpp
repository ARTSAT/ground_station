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
#include "writer.h"
#include "stringbuffer.h"
#include "artsatd.h"

#define JSON_VERSION                            ("2.0")

struct MethodTableRec {
    char const*                 name;
    ASDServerRPC::Method        method;
};
template <class T>
class Writer : public rapidjson::Writer<T> {
    public:
        explicit                Writer          (T& stream) : rapidjson::Writer<T>(stream)
                {
                }
    
                Writer&         Double          (double param)
                {
                    char buffer[100];
                    int ret;
                    int i;
                    
#if _MSC_VER
                    ret = sprintf_s(buffer, sizeof(buffer), "%.9f", param);
#else
                    ret = snprintf(buffer, sizeof(buffer), "%.9f", param);
#endif
                    RAPIDJSON_ASSERT(ret >= 1);
                    if (!isnan(param) && !isinf(param)) {
                        this->Prefix(rapidjson::kNumberType);
                        for (i = 0; i < ret; ++i) {
                            this->stream_.Put(buffer[i]);
                        }
                    }
                    else {
                        this->Prefix(rapidjson::kStringType);
                        this->WriteString(buffer, strlen(buffer));
                    }
                    return *this;
                }
};

static  MethodTableRec const                    g_method[] = {
    {"system.rpcEcho",                      &ASDServerRPC::rpcEcho},
    {"observer.getVersion",                 &ASDServerRPC::getVersion},
    {"observer.getSession",                 &ASDServerRPC::getSession},
    {"observer.setManualRotator",           &ASDServerRPC::setManualRotator},
    {"observer.getManualRotator",           &ASDServerRPC::getManualRotator},
    {"observer.setManualTransceiver",       &ASDServerRPC::setManualTransceiver},
    {"observer.getManualTransceiver",       &ASDServerRPC::getManualTransceiver},
    {"observer.setManualTNC",               &ASDServerRPC::setManualTNC},
    {"observer.getManualTNC",               &ASDServerRPC::getManualTNC},
    {"observer.setNORAD",                   &ASDServerRPC::setNORAD},
    {"observer.getNORAD",                   &ASDServerRPC::getNORAD},
    {"observer.setMode",                    &ASDServerRPC::setMode},
    {"observer.getMode",                    &ASDServerRPC::getMode},
    {"observer.getTime",                    &ASDServerRPC::getTime},
    {"observer.getObserverCallsign",        &ASDServerRPC::getObserverCallsign},
    {"observer.getObserverPosition",        &ASDServerRPC::getObserverPosition},
    {"observer.getObserverDirection",       &ASDServerRPC::getObserverDirection},
    {"observer.getObserverFrequency",       &ASDServerRPC::getObserverFrequency},
    {"observer.getSatellitePosition",       &ASDServerRPC::getSatellitePosition},
    {"observer.getSatelliteDirection",      &ASDServerRPC::getSatelliteDirection},
    {"observer.getSatelliteFrequency",      &ASDServerRPC::getSatelliteFrequency},
    {"observer.getSatelliteDopplerShift",   &ASDServerRPC::getSatelliteDopplerShift},
    {"observer.getSatelliteAOSLOS",         &ASDServerRPC::getSatelliteAOSLOS},
    {"observer.getSatelliteMEL",            &ASDServerRPC::getSatelliteMEL},
    {"observer.getRotatorStart",            &ASDServerRPC::getRotatorStart},
    {"observer.getError",                   &ASDServerRPC::getError},
    {"observer.isValidRotator",             &ASDServerRPC::isValidRotator},
    {"observer.isValidTransceiver",         &ASDServerRPC::isValidTransceiver},
    {"observer.isValidTNC",                 &ASDServerRPC::isValidTNC},
    {"observer.controlSession",             &ASDServerRPC::controlSession},
    {"observer.excludeSession",             &ASDServerRPC::excludeSession},
    {"observer.setRotatorAzimuth",          &ASDServerRPC::setRotatorAzimuth},
    {"observer.setRotatorElevation",        &ASDServerRPC::setRotatorElevation},
    {"observer.setTransceiverMode",         &ASDServerRPC::setTransceiverMode},
    {"observer.setTransceiverSender",       &ASDServerRPC::setTransceiverSender},
    {"observer.setTransceiverReceiver",     &ASDServerRPC::setTransceiverReceiver},
    {"observer.setTNCMode",                 &ASDServerRPC::setTNCMode},
    {"observer.sendTNCPacket",              &ASDServerRPC::sendTNCPacket},
    {"observer.requestCommand",             &ASDServerRPC::requestCommand}
};

/*public */ASDServerRPC::ASDServerRPC(void)
{
}

/*public virtual */ASDServerRPC::~ASDServerRPC(void)
{
    close();
}

/*public */tgs::TGSError ASDServerRPC::open(std::string const& database)
{
    int i;
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    close();
    for (i = 0; i < lengthof(g_method); ++i) {
        _method[g_method[i].name] = g_method[i].method;
    }
    _database = database;
    if (error != tgs::TGSERROR_OK) {
        close();
    }
    return error;
}

/*public */void ASDServerRPC::close(void)
{
    _method.clear();
    return;
}

/*private virtual */tgs::TGSError ASDServerRPC::onRequest(RequestRec const& request, ResponseRec* response)
{
    if (boost::iequals(request.path, "/rpc.json")) {
        replyJSON(request, response);
    }
    else {
        replyStatus(Server::response::not_found, response);
    }
    return tgs::TGSERROR_OK;
}

/*private */void ASDServerRPC::replyJSON(RequestRec const& request, ResponseRec* response) const
{
    rapidjson::Document idoc;
    rapidjson::Document odoc;
    rapidjson::Value::ValueIterator it;
    rapidjson::Value result;
    rapidjson::StringBuffer buffer;
    Writer<rapidjson::StringBuffer> writer(buffer);
    
    if (!idoc.Parse<0>(request.content.c_str()).HasParseError()) {
        if (idoc.IsArray()) {
            odoc.SetArray();
            for (it = idoc.Begin(); it != idoc.End(); ++it) {
                processJSON(request.host, *it, &result, odoc.GetAllocator());
                if (!result.IsNull()) {
                    odoc.PushBack(result, odoc.GetAllocator());
                }
            }
            if (odoc.Empty()) {
                odoc.SetNull();
            }
        }
        else {
            processJSON(request.host, idoc, &odoc, odoc.GetAllocator());
        }
    }
    else {
        returnJSON(JSONCODE_PARSEERROR, result, result, &odoc, odoc.GetAllocator());
    }
    if (!odoc.IsNull()) {
        odoc.Accept(writer);
        response->content = buffer.GetString();
    }
    else {
        response->status = Server::response::no_content;
        response->content.clear();
    }
    response->header["Content-Type"] = "application/json";
    response->header["Cache-Control"] = "no-cache";
    return;
}

/*private */void ASDServerRPC::processJSON(std::string const& host, rapidjson::Value& request, rapidjson::Value* response, rapidjson::Document::AllocatorType& allocator) const
{
    std::map<std::string, Method>::const_iterator it;
    rapidjson::Value jsonrpc;
    rapidjson::Value method;
    rapidjson::Value params;
    rapidjson::Value iid;
    rapidjson::Value result;
    rapidjson::Value oid;
    Variant variant;
    Param mparam;
    Param mresult;
    bool reply;
    JSONCodeEnum code;
    tgs::TGSError error;
    
    reply = true;
    code = JSONCODE_OK;
    if (request.IsObject()) {
        if (request.HasMember("jsonrpc")) {
            jsonrpc = request["jsonrpc"];
            if (jsonrpc.IsString()) {
                if (boost::iequals(jsonrpc.GetString(), JSON_VERSION)) {
                    if (request.HasMember("id")) {
                        iid = request["id"];
                        if (iid.IsInt() || iid.IsString()) {
                            oid = iid;
                        }
                        else if (!iid.IsNull()) {
                            code = JSONCODE_INVALIDREQUEST;
                        }
                    }
                    else {
                        reply = false;
                    }
                    if (code == JSONCODE_OK) {
                        if (request.HasMember("method")) {
                            method = request["method"];
                            if (method.IsString()) {
                                if ((it = _method.find(method.GetString())) != _method.end()) {
                                    if (request.HasMember("params")) {
                                        params = request["params"];
                                        if (params.IsObject()) {
                                            toVariant(params, &variant);
                                            mparam = boost::get<Param>(variant);
                                        }
                                        else if (params.IsArray()) {
                                            code = JSONCODE_INVALIDPARAMS;
                                        }
                                        else if (!params.IsNull()) {
                                            code = JSONCODE_INVALIDREQUEST;
                                        }
                                    }
                                    if (code == JSONCODE_OK) {
                                        error = it->second(this, host, mparam, &mresult);
                                        toJSON(mresult, &result, allocator);
                                        switch (error) {
                                            case tgs::TGSERROR_OK:
                                                // nop
                                                break;
                                            case tgs::TGSERROR_INVALID_PARAM:
                                            case tgs::TGSERROR_INVALID_FORMAT:
                                            case tgs::TGSERROR_NO_RESULT:
                                                code = JSONCODE_INVALIDPARAMS;
                                                break;
                                            default:
                                                code = JSONCODE_INTERNALERROR;
                                                break;
                                        }
                                    }
                                }
                                else {
                                    code = JSONCODE_METHODNOTFOUND;
                                }
                            }
                            else {
                                code = JSONCODE_INVALIDREQUEST;
                            }
                        }
                        else {
                            code = JSONCODE_INVALIDREQUEST;
                        }
                    }
                }
                else {
                    code = JSONCODE_INVALIDREQUEST;
                }
            }
            else {
                code = JSONCODE_INVALIDREQUEST;
            }
        }
        else {
            code = JSONCODE_INVALIDREQUEST;
        }
    }
    else {
        code = JSONCODE_INVALIDREQUEST;
    }
    if (reply) {
        returnJSON(code, result, oid, response, allocator);
    }
    else {
        response->SetNull();
    }
    return;
}

/*private static */void ASDServerRPC::returnJSON(JSONCodeEnum code, rapidjson::Value& result, rapidjson::Value& id, rapidjson::Value* response, rapidjson::Document::AllocatorType& allocator)
{
    rapidjson::Value error;
    
    response->SetObject();
    response->AddMember("jsonrpc", JSON_VERSION, allocator);
    if (code == JSONCODE_OK) {
        response->AddMember("result", result, allocator);
    }
    else {
        error.SetObject();
        error.AddMember("code", code, allocator);
        switch (code) {
            case JSONCODE_PARSEERROR:
                error.AddMember("message", "Parse error", allocator);
                break;
            case JSONCODE_INVALIDREQUEST:
                error.AddMember("message", "Invalid Request", allocator);
                break;
            case JSONCODE_METHODNOTFOUND:
                error.AddMember("message", "Method not found", allocator);
                break;
            case JSONCODE_INVALIDPARAMS:
                error.AddMember("message", "Invalid params", allocator);
                break;
            case JSONCODE_INTERNALERROR:
                error.AddMember("message", "Internal error", allocator);
                break;
            default:
                error.AddMember("message", "Server error", allocator);
                break;
        }
        if (!result.IsNull()) {
            error.AddMember("data", result, allocator);
        }
        response->AddMember("error", error, allocator);
    }
    response->AddMember("id", id, allocator);
    return;
}

/*private static */void ASDServerRPC::toVariant(rapidjson::Value const& param, Variant* result)
{
    Variant value;
    
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
            *result = static_cast<std::string>(param.GetString());
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
    return;
}

/*private static */void ASDServerRPC::toJSON(Variant const& param, rapidjson::Value* result, rapidjson::Document::AllocatorType& allocator)
{
    rapidjson::Value value;
    
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
    return;
}

/*public */tgs::TGSError ASDServerRPC::rpcEcho(std::string const& host, Param const& param, Param* result) const
{
    *result = param;
    return tgs::TGSERROR_OK;
}

/*public */tgs::TGSError ASDServerRPC::getVersion(std::string const& host, Param const& param, Param* result) const
{
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    if ((error = updateSession(param, NULL, result)) == tgs::TGSERROR_OK) {
        setResult(artsatd::getInstance().getVersion(), "version", result);
    }
    return error;
}

/*public */tgs::TGSError ASDServerRPC::getSession(std::string const& host, Param const& param, Param* result) const
{
    std::string session;
    int owner;
    bool exclusive;
    std::string current;
    int online;
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    if ((error = updateSession(param, &session, result)) == tgs::TGSERROR_OK) {
        artsatd::getInstance().getSession(session, &owner, &exclusive, &current, &online);
        setResult(owner, "owner", result);
        setResult(exclusive, "exclusive", result);
        setResult(current, "host", result);
        setResult(online, "online", result);
    }
    return error;
}

/*public */tgs::TGSError ASDServerRPC::setManualRotator(std::string const& host, Param const& param, Param* result) const
{
    std::string session;
    bool manual;
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    if ((error = updateSession(param, &session, result)) == tgs::TGSERROR_OK) {
        if ((error = getParam(param, "manual", &manual)) == tgs::TGSERROR_OK) {
            if ((error = artsatd::getInstance().setManualRotator(session, manual)) != tgs::TGSERROR_OK) {
                error = setError(error, result);
            }
        }
    }
    return error;
}

/*public */tgs::TGSError ASDServerRPC::getManualRotator(std::string const& host, Param const& param, Param* result) const
{
    std::string session;
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    if ((error = updateSession(param, &session, result)) == tgs::TGSERROR_OK) {
        setResult(artsatd::getInstance().getManualRotator(), "manual", result);
    }
    return error;
}

/*public */tgs::TGSError ASDServerRPC::setManualTransceiver(std::string const& host, Param const& param, Param* result) const
{
    std::string session;
    bool manual;
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    if ((error = updateSession(param, &session, result)) == tgs::TGSERROR_OK) {
        if ((error = getParam(param, "manual", &manual)) == tgs::TGSERROR_OK) {
            if ((error = artsatd::getInstance().setManualTransceiver(session, manual)) != tgs::TGSERROR_OK) {
                error = setError(error, result);
            }
        }
    }
    return error;
}

/*public */tgs::TGSError ASDServerRPC::getManualTransceiver(std::string const& host, Param const& param, Param* result) const
{
    std::string session;
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    if ((error = updateSession(param, &session, result)) == tgs::TGSERROR_OK) {
        setResult(artsatd::getInstance().getManualTransceiver(), "manual", result);
    }
    return error;
}

/*public */tgs::TGSError ASDServerRPC::setManualTNC(std::string const& host, Param const& param, Param* result) const
{
    std::string session;
    bool manual;
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    if ((error = updateSession(param, &session, result)) == tgs::TGSERROR_OK) {
        if ((error = getParam(param, "manual", &manual)) == tgs::TGSERROR_OK) {
            if ((error = artsatd::getInstance().setManualTNC(session, manual)) != tgs::TGSERROR_OK) {
                error = setError(error, result);
            }
        }
    }
    return error;
}

/*public */tgs::TGSError ASDServerRPC::getManualTNC(std::string const& host, Param const& param, Param* result) const
{
    std::string session;
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    if ((error = updateSession(param, &session, result)) == tgs::TGSERROR_OK) {
        setResult(artsatd::getInstance().getManualTNC(), "manual", result);
    }
    return error;
}

/*public */tgs::TGSError ASDServerRPC::setNORAD(std::string const& host, Param const& param, Param* result) const
{
    std::string session;
    std::string query;
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    if ((error = updateSession(param, &session, result)) == tgs::TGSERROR_OK) {
        if ((error = getParam(param, "query", &query)) == tgs::TGSERROR_OK) {
            if ((error = artsatd::getInstance().setNORAD(session, query)) != tgs::TGSERROR_OK) {
                error = setError(error, result);
            }
        }
    }
    return error;
}

/*public */tgs::TGSError ASDServerRPC::getNORAD(std::string const& host, Param const& param, Param* result) const
{
    std::string session;
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    if ((error = updateSession(param, &session, result)) == tgs::TGSERROR_OK) {
        setResult(artsatd::getInstance().getNORAD(), "norad", result);
    }
    return error;
}

/*public */tgs::TGSError ASDServerRPC::setMode(std::string const& host, Param const& param, Param* result) const
{
    std::string session;
    std::string mode;
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    if ((error = updateSession(param, &session, result)) == tgs::TGSERROR_OK) {
        if ((error = getParam(param, "mode", &mode)) == tgs::TGSERROR_OK) {
            if ((error = artsatd::getInstance().setMode(session, mode)) != tgs::TGSERROR_OK) {
                error = setError(error, result);
            }
        }
    }
    return error;
}

/*public */tgs::TGSError ASDServerRPC::getMode(std::string const& host, Param const& param, Param* result) const
{
    std::string session;
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    if ((error = updateSession(param, &session, result)) == tgs::TGSERROR_OK) {
        setResult(artsatd::getInstance().getMode(), "mode", result);
    }
    return error;
}

/*public */tgs::TGSError ASDServerRPC::getTime(std::string const& host, Param const& param, Param* result) const
{
    std::string session;
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    if ((error = updateSession(param, &session, result)) == tgs::TGSERROR_OK) {
        setResult((artsatd::getInstance().getTime() - ir::IRXTimeDiff::localTimeOffset()).format("%YYYY/%MM/%DD %hh:%mm:%ss UTC"), "time", result);
    }
    return error;
}

/*public */tgs::TGSError ASDServerRPC::getObserverCallsign(std::string const& host, Param const& param, Param* result) const
{
    std::string session;
    std::string callsign;
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    if ((error = updateSession(param, &session, result)) == tgs::TGSERROR_OK) {
        artsatd::getInstance().getObserverCallsign(&callsign);
        setResult(callsign, "callsign", result);
    }
    return error;
}

/*public */tgs::TGSError ASDServerRPC::getObserverPosition(std::string const& host, Param const& param, Param* result) const
{
    std::string session;
    double latitude;
    double longitude;
    double altitude;
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    if ((error = updateSession(param, &session, result)) == tgs::TGSERROR_OK) {
        artsatd::getInstance().getObserverPosition(&latitude, &longitude, &altitude);
        setResult(latitude, "latitude", result);
        setResult(longitude, "longitude", result);
        setResult(altitude, "altitude", result);
    }
    return error;
}

/*public */tgs::TGSError ASDServerRPC::getObserverDirection(std::string const& host, Param const& param, Param* result) const
{
    std::string session;
    ASDDeviceRotator::DataRec rotator;
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    if ((error = updateSession(param, &session, result)) == tgs::TGSERROR_OK) {
        artsatd::getRotator().getData(&rotator);
        setResult(rotator.azimuth, "azimuth", result);
        setResult(rotator.elevation, "elevation", result);
    }
    return error;
}

/*public */tgs::TGSError ASDServerRPC::getObserverFrequency(std::string const& host, Param const& param, Param* result) const
{
    std::string session;
    ASDDeviceTransceiver::DataRec transceiver;
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    if ((error = updateSession(param, &session, result)) == tgs::TGSERROR_OK) {
        artsatd::getTransceiver().getData(&transceiver);
        setResult(transceiver.frequencySender, "sender", result);
        setResult(transceiver.frequencyReceiver, "receiver", result);
    }
    return error;
}

/*public */tgs::TGSError ASDServerRPC::getSatellitePosition(std::string const& host, Param const& param, Param* result) const
{
    std::string session;
    double latitude;
    double longitude;
    double altitude;
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    if ((error = updateSession(param, &session, result)) == tgs::TGSERROR_OK) {
        artsatd::getInstance().getSatellitePosition(&latitude, &longitude, &altitude);
        setResult(latitude, "latitude", result);
        setResult(longitude, "longitude", result);
        setResult(altitude, "altitude", result);
    }
    return error;
}

/*public */tgs::TGSError ASDServerRPC::getSatelliteDirection(std::string const& host, Param const& param, Param* result) const
{
    std::string session;
    double azimuth;
    double elevation;
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    if ((error = updateSession(param, &session, result)) == tgs::TGSERROR_OK) {
        artsatd::getInstance().getSatelliteDirection(&azimuth, &elevation);
        setResult(azimuth, "azimuth", result);
        setResult(elevation, "elevation", result);
    }
    return error;
}

/*public */tgs::TGSError ASDServerRPC::getSatelliteFrequency(std::string const& host, Param const& param, Param* result) const
{
    std::string session;
    double beacon;
    double sender;
    double receiver;
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    if ((error = updateSession(param, &session, result)) == tgs::TGSERROR_OK) {
        artsatd::getInstance().getSatelliteFrequency(&beacon, &sender, &receiver);
        setResult(beacon, "beacon", result);
        setResult(sender, "sender", result);
        setResult(receiver, "receiver", result);
    }
    return error;
}

/*public */tgs::TGSError ASDServerRPC::getSatelliteDopplerShift(std::string const& host, Param const& param, Param* result) const
{
    std::string session;
    double sender;
    double receiver;
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    if ((error = updateSession(param, &session, result)) == tgs::TGSERROR_OK) {
        artsatd::getInstance().getSatelliteDopplerShift(&sender, &receiver);
        setResult(sender, "sender", result);
        setResult(receiver, "receiver", result);
    }
    return error;
}

/*public */tgs::TGSError ASDServerRPC::getSatelliteAOSLOS(std::string const& host, Param const& param, Param* result) const
{
    std::string session;
    ir::IRXTime aos;
    ir::IRXTime los;
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    if ((error = updateSession(param, &session, result)) == tgs::TGSERROR_OK) {
        artsatd::getInstance().getSatelliteAOSLOS(&aos, &los);
        setResult((aos - ir::IRXTimeDiff::localTimeOffset()).format("%YYYY/%MM/%DD %hh:%mm:%ss UTC"), "aos", result);
        setResult((los - ir::IRXTimeDiff::localTimeOffset()).format("%YYYY/%MM/%DD %hh:%mm:%ss UTC"), "los", result);
    }
    return error;
}

/*public */tgs::TGSError ASDServerRPC::getSatelliteMEL(std::string const& host, Param const& param, Param* result) const
{
    std::string session;
    double mel;
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    if ((error = updateSession(param, &session, result)) == tgs::TGSERROR_OK) {
        artsatd::getInstance().getSatelliteMEL(&mel);
        setResult(mel, "mel", result);
    }
    return error;
}

/*public */tgs::TGSError ASDServerRPC::getRotatorStart(std::string const& host, Param const& param, Param* result) const
{
    std::string session;
    ir::IRXTime start;
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    if ((error = updateSession(param, &session, result)) == tgs::TGSERROR_OK) {
        artsatd::getInstance().getRotatorStart(&start);
        setResult((start - ir::IRXTimeDiff::localTimeOffset()).format("%YYYY/%MM/%DD %hh:%mm:%ss UTC"), "start", result);
    }
    return error;
}

/*public */tgs::TGSError ASDServerRPC::getError(std::string const& host, Param const& param, Param* result) const
{
    std::string session;
    tgs::TGSError state;
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    if ((error = updateSession(param, &session, result)) == tgs::TGSERROR_OK) {
        state = artsatd::getInstance().getError();
        setResult(state.get(), "code", result);
        setResult(state.print(), "message", result);
    }
    return error;
}

/*public */tgs::TGSError ASDServerRPC::isValidRotator(std::string const& host, Param const& param, Param* result) const
{
    std::string session;
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    if ((error = updateSession(param, &session, result)) == tgs::TGSERROR_OK) {
        setResult(artsatd::getRotator().isValid(), "valid", result);
    }
    return error;
}

/*public */tgs::TGSError ASDServerRPC::isValidTransceiver(std::string const& host, Param const& param, Param* result) const
{
    std::string session;
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    if ((error = updateSession(param, &session, result)) == tgs::TGSERROR_OK) {
        setResult(artsatd::getTransceiver().isValid(), "valid", result);
    }
    return error;
}

/*public */tgs::TGSError ASDServerRPC::isValidTNC(std::string const& host, Param const& param, Param* result) const
{
    std::string session;
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    if ((error = updateSession(param, &session, result)) == tgs::TGSERROR_OK) {
        setResult(artsatd::getTNC().isValid(), "valid", result);
    }
    return error;
}

/*public */tgs::TGSError ASDServerRPC::controlSession(std::string const& host, Param const& param, Param* result) const
{
    std::string session;
    bool owner;
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    if ((error = updateSession(param, &session, result)) == tgs::TGSERROR_OK) {
        if ((error = getParam(param, "owner", &owner)) == tgs::TGSERROR_OK) {
            if ((error = artsatd::getInstance().controlSession(session, owner, host)) != tgs::TGSERROR_OK) {
                error = setError(error, result);
            }
        }
    }
    return error;
}

/*public */tgs::TGSError ASDServerRPC::excludeSession(std::string const& host, Param const& param, Param* result) const
{
    std::string session;
    bool exclusive;
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    if ((error = updateSession(param, &session, result)) == tgs::TGSERROR_OK) {
        if ((error = getParam(param, "exclusive", &exclusive)) == tgs::TGSERROR_OK) {
            if ((error = artsatd::getInstance().excludeSession(session, exclusive)) != tgs::TGSERROR_OK) {
                error = setError(error, result);
            }
        }
    }
    return error;
}

/*public */tgs::TGSError ASDServerRPC::setRotatorAzimuth(std::string const& host, Param const& param, Param* result) const
{
    std::string session;
    int azimuth;
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    if ((error = updateSession(param, &session, result)) == tgs::TGSERROR_OK) {
        if ((error = getParam(param, "azimuth", &azimuth)) == tgs::TGSERROR_OK) {
            if ((error = artsatd::getInstance().controlManualRotator(session, &artsatd::controlRotatorAzimuth, &azimuth)) != tgs::TGSERROR_OK) {
                error = setError(error, result);
            }
        }
    }
    return error;
}

/*public */tgs::TGSError ASDServerRPC::setRotatorElevation(std::string const& host, Param const& param, Param* result) const
{
    std::string session;
    int elevation;
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    if ((error = updateSession(param, &session, result)) == tgs::TGSERROR_OK) {
        if ((error = getParam(param, "elevation", &elevation)) == tgs::TGSERROR_OK) {
            if ((error = artsatd::getInstance().controlManualRotator(session, &artsatd::controlRotatorElevation, &elevation)) != tgs::TGSERROR_OK) {
                error = setError(error, result);
            }
        }
    }
    return error;
}

/*public */tgs::TGSError ASDServerRPC::setTransceiverMode(std::string const& host, Param const& param, Param* result) const
{
    std::string session;
    std::string mode;
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    if ((error = updateSession(param, &session, result)) == tgs::TGSERROR_OK) {
        if ((error = getParam(param, "mode", &mode)) == tgs::TGSERROR_OK) {
            if (mode == "CW") {
                error = artsatd::getInstance().controlManualTransceiver(session, &artsatd::controlTransceiverModeCW, NULL);
            }
            else if (mode == "FM") {
                error = artsatd::getInstance().controlManualTransceiver(session, &artsatd::controlTransceiverModeFM, NULL);
            }
            else {
                error = tgs::TGSERROR_INVALID_PARAM;
            }
            if (error != tgs::TGSERROR_OK) {
                error = setError(error, result);
            }
        }
    }
    return error;
}

/*public */tgs::TGSError ASDServerRPC::setTransceiverSender(std::string const& host, Param const& param, Param* result) const
{
    std::string session;
    int sender;
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    if ((error = updateSession(param, &session, result)) == tgs::TGSERROR_OK) {
        if ((error = getParam(param, "sender", &sender)) == tgs::TGSERROR_OK) {
            if ((error = artsatd::getInstance().controlManualTransceiver(session, &artsatd::controlTransceiverSender, &sender)) != tgs::TGSERROR_OK) {
                error = setError(error, result);
            }
        }
    }
    return error;
}

/*public */tgs::TGSError ASDServerRPC::setTransceiverReceiver(std::string const& host, Param const& param, Param* result) const
{
    std::string session;
    int receiver;
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    if ((error = updateSession(param, &session, result)) == tgs::TGSERROR_OK) {
        if ((error = getParam(param, "receiver", &receiver)) == tgs::TGSERROR_OK) {
            if ((error = artsatd::getInstance().controlManualTransceiver(session, &artsatd::controlTransceiverReceiver, &receiver)) != tgs::TGSERROR_OK) {
                error = setError(error, result);
            }
        }
    }
    return error;
}

/*public */tgs::TGSError ASDServerRPC::setTNCMode(std::string const& host, Param const& param, Param* result) const
{
    std::string session;
    std::string mode;
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    if ((error = updateSession(param, &session, result)) == tgs::TGSERROR_OK) {
        if ((error = getParam(param, "mode", &mode)) == tgs::TGSERROR_OK) {
            if (mode == "Command") {
                error = artsatd::getInstance().controlManualTNC(session, &artsatd::controlTNCModeCommand, NULL);
            }
            else if (mode == "Converse") {
                error = artsatd::getInstance().controlManualTNC(session, &artsatd::controlTNCModeConverse, NULL);
            }
            else {
                error = tgs::TGSERROR_INVALID_PARAM;
            }
            if (error != tgs::TGSERROR_OK) {
                error = setError(error, result);
            }
        }
    }
    return error;
}

/*public */tgs::TGSError ASDServerRPC::sendTNCPacket(std::string const& host, Param const& param, Param* result) const
{
    std::string session;
    std::string packet;
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    if ((error = updateSession(param, &session, result)) == tgs::TGSERROR_OK) {
        if ((error = getParam(param, "packet", &packet)) == tgs::TGSERROR_OK) {
            if ((error = artsatd::getInstance().controlManualTNC(session, &artsatd::controlTNCPacket, &packet)) != tgs::TGSERROR_OK) {
                error = setError(error, result);
            }
        }
    }
    return error;
}

/*public */tgs::TGSError ASDServerRPC::requestCommand(std::string const& host, Param const& param, Param* result) const
{
    std::string session;
    std::string command;
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    if ((error = updateSession(param, &session, result)) == tgs::TGSERROR_OK) {
        if ((error = getParam(param, "command", &command)) == tgs::TGSERROR_OK) {
            if ((error = artsatd::getInstance().requestCommand(session, command)) != tgs::TGSERROR_OK) {
                error = setError(error, result);
            }
        }
    }
    return error;
}

/*public */tgs::TGSError ASDServerRPC::setName(std::string const& host, Param const& param, Param* result) const
{
    std::string session;
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    if ((error = updateSession(param, &session, result)) == tgs::TGSERROR_OK) {
    }
    return error;
}

/*public */tgs::TGSError ASDServerRPC::getName(std::string const& host, Param const& param, Param* result) const
{
    std::string session;
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    if ((error = updateSession(param, &session, result)) == tgs::TGSERROR_OK) {
    }
    return error;
}

/*public */tgs::TGSError ASDServerRPC::setCallsign(std::string const& host, Param const& param, Param* result) const
{
    std::string session;
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    if ((error = updateSession(param, &session, result)) == tgs::TGSERROR_OK) {
    }
    return error;
}

/*public */tgs::TGSError ASDServerRPC::getCallsign(std::string const& host, Param const& param, Param* result) const
{
    std::string session;
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    if ((error = updateSession(param, &session, result)) == tgs::TGSERROR_OK) {
    }
    return error;
}

/*public */tgs::TGSError ASDServerRPC::setRadioBeacon(std::string const& host, Param const& param, Param* result) const
{
    std::string session;
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    if ((error = updateSession(param, &session, result)) == tgs::TGSERROR_OK) {
    }
    return error;
}

/*public */tgs::TGSError ASDServerRPC::getRadioBeacon(std::string const& host, Param const& param, Param* result) const
{
    std::string session;
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    if ((error = updateSession(param, &session, result)) == tgs::TGSERROR_OK) {
    }
    return error;
}

/*public */tgs::TGSError ASDServerRPC::setRadioSender(std::string const& host, Param const& param, Param* result) const
{
    std::string session;
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    if ((error = updateSession(param, &session, result)) == tgs::TGSERROR_OK) {
    }
    return error;
}

/*public */tgs::TGSError ASDServerRPC::getRadioSender(std::string const& host, Param const& param, Param* result) const
{
    std::string session;
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    if ((error = updateSession(param, &session, result)) == tgs::TGSERROR_OK) {
    }
    return error;
}

/*public */tgs::TGSError ASDServerRPC::setRadioReceiver(std::string const& host, Param const& param, Param* result) const
{
    std::string session;
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    if ((error = updateSession(param, &session, result)) == tgs::TGSERROR_OK) {
    }
    return error;
}

/*public */tgs::TGSError ASDServerRPC::getRadioReceiver(std::string const& host, Param const& param, Param* result) const
{
    std::string session;
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    if ((error = updateSession(param, &session, result)) == tgs::TGSERROR_OK) {
    }
    return error;
}

/*public */tgs::TGSError ASDServerRPC::setOrbitData(std::string const& host, Param const& param, Param* result) const
{
    std::string session;
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    if ((error = updateSession(param, &session, result)) == tgs::TGSERROR_OK) {
    }
    return error;
}

/*public */tgs::TGSError ASDServerRPC::getOrbitData(std::string const& host, Param const& param, Param* result) const
{
    std::string session;
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    if ((error = updateSession(param, &session, result)) == tgs::TGSERROR_OK) {
    }
    return error;
}

/*public */tgs::TGSError ASDServerRPC::getCount(std::string const& host, Param const& param, Param* result) const
{
    std::string session;
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    if ((error = updateSession(param, &session, result)) == tgs::TGSERROR_OK) {
    }
    return error;
}

/*public */tgs::TGSError ASDServerRPC::getField(std::string const& host, Param const& param, Param* result) const
{
    std::string session;
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    if ((error = updateSession(param, &session, result)) == tgs::TGSERROR_OK) {
    }
    return error;
}

/*public */tgs::TGSError ASDServerRPC::getFieldByName(std::string const& host, Param const& param, Param* result) const
{
    std::string session;
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    if ((error = updateSession(param, &session, result)) == tgs::TGSERROR_OK) {
    }
    return error;
}

/*public */tgs::TGSError ASDServerRPC::getFieldByCallsign(std::string const& host, Param const& param, Param* result) const
{
    std::string session;
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    if ((error = updateSession(param, &session, result)) == tgs::TGSERROR_OK) {
    }
    return error;
}

/*public */tgs::TGSError ASDServerRPC::getNORADByName(std::string const& host, Param const& param, Param* result) const
{
    std::string session;
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    if ((error = updateSession(param, &session, result)) == tgs::TGSERROR_OK) {
    }
    return error;
}

/*public */tgs::TGSError ASDServerRPC::getNORADByCallsign(std::string const& host, Param const& param, Param* result) const
{
    std::string session;
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    if ((error = updateSession(param, &session, result)) == tgs::TGSERROR_OK) {
    }
    return error;
}

/*private static */tgs::TGSError ASDServerRPC::updateSession(Param const& param, std::string* session, Param* result)
{
    std::string string;
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    error = getParam(param, "session", &string);
    switch (error) {
        case tgs::TGSERROR_OK:
        case tgs::TGSERROR_NO_RESULT:
            if ((error = artsatd::getInstance().requestSession(&string, NULL)) == tgs::TGSERROR_OK) {
                setResult(string, "session", result);
                if (session != NULL) {
                    *session = string;
                }
            }
            else {
                error = setError(error, result);
            }
            break;
        default:
            // nop
            break;
    }
    return error;
}

template <typename T>
/*private static */tgs::TGSError ASDServerRPC::getParam(Param const& param, std::string const& key, T* result)
{
    Param::const_iterator it;
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    if ((it = param.find(key)) != param.end()) {
        if (it->second.type() == typeid(T)) {
            *result = boost::get<T>(it->second);
        }
        else {
            error = tgs::TGSERROR_INVALID_FORMAT;
        }
    }
    else {
        error = tgs::TGSERROR_NO_RESULT;
    }
    return error;
}

/*private static */tgs::TGSError ASDServerRPC::getParam(Param const& param, std::string const& key, int* result)
{
    Param::const_iterator it;
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    if ((it = param.find(key)) != param.end()) {
        if (it->second.type() == typeid(double)) {
            *result = boost::get<double>(it->second);
        }
        else {
            error = tgs::TGSERROR_INVALID_FORMAT;
        }
    }
    else {
        error = tgs::TGSERROR_NO_RESULT;
    }
    return error;
}

template <typename T>
/*private static */void ASDServerRPC::setResult(T const& param, std::string const& key, Param* result)
{
    (*result)[key] = param;
    return;
}

/*private static */tgs::TGSError ASDServerRPC::setError(tgs::TGSError error, Param* result)
{
    (*result)["message"] = error.print();
    return tgs::TGSERROR_FAILED;
}
