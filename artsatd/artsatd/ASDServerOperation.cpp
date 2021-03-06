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
#include "artsatd.h"

#define COOKIE_SESSION_ID                       ("session")
#define COOKIE_ERROR_CATEGORY                   ("category")
#define COOKIE_ERROR_MESSAGE                    ("error")
#define COOKIE_SHRINK_HARDWARE                  ("hardware")
#define COOKIE_SHRINK_DATABASE                  ("database")
#define COOKIE_SHRINK_STATUS                    ("status")
#define COOKIE_SHRINK_CONTROL                   ("control")
#define COOKIE_MAXAGE                           (365 * 24 * 60 * 60)
#define CATEGORY_SESSION                        ("session")
#define CATEGORY_HARDWARE                       ("hardware")
#define CATEGORY_DATABASE                       ("database")
#define CATEGORY_CONTROL                        ("control")
#define SHRINK_SHOW                             ("+")
#define SHRINK_HIDE                             ("-")
#define DEVICE_OK                               ("ok")
#define DEVICE_NG                               ("ng")
#define DEFAULT_TIME                            ("----/--/-- --:--:-- JST")
#define DEFAULT_TIMEDIFF                        ("&Delta; --:--:--")
#define FORM_DISABLED                           ("disabled")
#define FORM_READONLY                           ("readonly")

struct CacheTableRec {
    char const*                 path;
    char const*                 mime;
    char const*                 cache;
    char const*                 content;
    void  (ASDServerOperation::*function)       (ASDServerOperation::RequestRec const& request, ASDServerOperation::ResponseRec* response) const;
};

static  CacheTableRec const                     g_cache[] = {
    {"/",               "text/html",        "no-cache",              "/root.html",          &ASDServerOperation::replyRoot},
    {"/hardware.html",  "text/html",        "private, max-age=3600", "/hardware.html",      &ASDServerOperation::replyHardware},
    {"/orbital.html",   "text/html",        "private, max-age=3600", "/orbital.html",       &ASDServerOperation::replyOrbital},
    {"/streaming.html", "text/html",        "private, max-age=3600", "/streaming.html",     NULL},
    {"/webcam.html",    "text/html",        "private, max-age=3600", "/webcam.html",        NULL},
    {"/default.css",    "text/css",         "private, max-age=3600", "/css/default.css",    NULL}
};
static  char const* const                       g_shrink[] = {
    COOKIE_SHRINK_HARDWARE,
    COOKIE_SHRINK_DATABASE,
    COOKIE_SHRINK_STATUS,
    COOKIE_SHRINK_CONTROL
};

/*public */ASDServerOperation::ASDServerOperation(void)
{
}

/*public virtual */ASDServerOperation::~ASDServerOperation(void)
{
    close();
}

/*public */tgs::TGSError ASDServerOperation::open(std::string const& skeleton, std::string const& database)
{
    CacheRec cache;
    int i;
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    close();
    for (i = 0; i < lengthof(g_cache); ++i) {
        cache.content.clear();
        if (strlen(g_cache[i].content) > 0) {
            error = serializeCache(skeleton + g_cache[i].content, &cache.content);
        }
        if (error == tgs::TGSERROR_OK) {
            cache.mime = g_cache[i].mime;
            cache.cache = g_cache[i].cache;
            cache.function = g_cache[i].function;
            _cache[g_cache[i].path] = cache;
        }
        else {
            break;
        }
    }
    if (error == tgs::TGSERROR_OK) {
        _database = database;
    }
    if (error != tgs::TGSERROR_OK) {
        close();
    }
    return error;
}

/*public */void ASDServerOperation::close(void)
{
    _cache.clear();
    return;
}

/*public */void ASDServerOperation::replyRoot(RequestRec const& request, ResponseRec* response) const
{
    artsatd& daemon(artsatd::getInstance());
    insensitive::map<std::string, std::string>::const_iterator it;
    std::string session;
    bool update;
    std::string category;
    std::string message;
    bool shrink[lengthof(g_shrink)];
    int i;
    
    if ((it = request.cookie.find(COOKIE_SESSION_ID)) != request.cookie.end()) {
        session = it->second;
    }
    if (daemon.requestSession(request.host, &session, &update) == tgs::TGSERROR_OK) {
        if (!update) {
            if ((it = request.cookie.find(COOKIE_ERROR_CATEGORY)) != request.cookie.end()) {
                category = it->second;
            }
            if ((it = request.cookie.find(COOKIE_ERROR_MESSAGE)) != request.cookie.end()) {
                message = it->second;
            }
        }
        for (i = 0; i < lengthof(g_shrink); ++i) {
            shrink[i] = false;
            if ((it = request.cookie.find(g_shrink[i])) != request.cookie.end()) {
                if (it->second == "true") {
                    shrink[i] = true;
                }
            }
        }
        if (!request.query.empty()) {
            executeRoot(session, request.host, request.query, &category, &message, shrink);
            replyStatus(Server::response::found, response);
            response->header["Location"] = "/";
        }
        else {
            processRoot(session, request.host, category, message, shrink, &response->content);
        }
        setCookie(COOKIE_SESSION_ID, session, COOKIE_MAXAGE, response);
        setCookie(COOKIE_ERROR_CATEGORY, category, COOKIE_MAXAGE, response);
        setCookie(COOKIE_ERROR_MESSAGE, message, COOKIE_MAXAGE, response);
        for (i = 0; i < lengthof(g_shrink); ++i) {
            setCookie(g_shrink[i], (shrink[i]) ? ("true") : ("false"), COOKIE_MAXAGE, response);
        }
    }
    else {
        replyStatus(Server::response::service_unavailable, response);
    }
    return;
}

/*public */void ASDServerOperation::replyHardware(RequestRec const& request, ResponseRec* response) const
{
    artsatd& daemon(artsatd::getInstance());
    insensitive::map<std::string, std::string>::const_iterator it;
    std::string session;
    int owner;
    bool exclusive;
    int online;
    std::string string;
    ASDDeviceRotator::DataRec rotator;
    ASDDeviceTransceiver::DataRec transceiver;
    double x;
    double y;
    double z;
    
    if ((it = request.cookie.find(COOKIE_SESSION_ID)) != request.cookie.end()) {
        session = it->second;
    }
    daemon.getSession(session, &owner, &exclusive, &session, &online);
    daemon.getObserverCallsign(&string);
    boost::replace_first(response->content, "<!CS />", stringizeCallsign(string));
    daemon.getObserverPosition(&x, &y, &z);
    boost::replace_first(response->content, "<!LT />", stringizeLatitude(x));
    boost::replace_first(response->content, "<!LN />", stringizeLongitude(y));
    boost::replace_first(response->content, "<!AT />", stringizeAltitude(z));
    artsatd::getRotator().getData(&rotator);
    boost::replace_first(response->content, "<!AZ />", colorizeSpan("cyan", stringizeAzimuth(rotator.azimuth)));
    boost::replace_first(response->content, "<!EL />", colorizeSpan("magenta", stringizeElevation(rotator.elevation)));
    artsatd::getTransceiver().getData(&transceiver);
    boost::replace_first(response->content, "<!SF />", stringizeFrequency(transceiver.frequencySender, request.host != "127.0.0.1" && owner <= 0));
    boost::replace_first(response->content, "<!RF />", stringizeFrequency(transceiver.frequencyReceiver, false));
    session = stringizeSession(session);
    if (owner > 0) {
        session = colorizeSpan("green", session);
    }
    else if (exclusive) {
        session = colorizeSpan("red", session);
    }
    boost::replace_first(response->content, "<!SS />", session);
    boost::replace_first(response->content, "<!OS />", stringizeOnline(online));
    return;
}

/*public */void ASDServerOperation::replyOrbital(RequestRec const& request, ResponseRec* response) const
{
    artsatd& daemon(artsatd::getInstance());
    insensitive::map<std::string, std::string>::const_iterator it;
    std::string session;
    int owner;
    std::string string;
    ir::IRXTime time;
    ir::IRXTime aos;
    ir::IRXTime los;
    double x;
    double y;
    double z;
    
    if ((it = request.cookie.find(COOKIE_SESSION_ID)) != request.cookie.end()) {
        session = it->second;
    }
    daemon.getSession(session, &owner, NULL, NULL, NULL);
    daemon.getSpacecraftPosition(&x, &y, &z);
    boost::replace_first(response->content, "<!LT />", stringizeLatitude(x));
    boost::replace_first(response->content, "<!LN />", stringizeLongitude(y));
    boost::replace_first(response->content, "<!AT />", stringizeAltitude(z));
    daemon.getSpacecraftDirection(&x, &y);
    boost::replace_first(response->content, "<!AZ />", colorizeSpan("cyan", stringizeAzimuth(x)));
    boost::replace_first(response->content, "<!EL />", colorizeSpan("magenta", stringizeElevation(y)));
    string = daemon.getMode();
    daemon.getSpacecraftFrequency(&x, &y, &z);
    boost::replace_first(response->content, "<!BF />", colorizeSpan((string == "CW") ? ("green") : (""), stringizeFrequency(x, false)));
    boost::replace_first(response->content, "<!SF />", colorizeSpan((string == "FM") ? ("green") : (""), stringizeFrequency(y, request.host != "127.0.0.1" && owner <= 0)));
    boost::replace_first(response->content, "<!RF />", colorizeSpan((string == "FM") ? ("green") : (""), stringizeFrequency(z, false)));
    daemon.getSpacecraftDopplerShift(&x, &y);
    boost::replace_first(response->content, "<!SD />", stringizeDopplerShift(x));
    boost::replace_first(response->content, "<!RD />", stringizeDopplerShift(y));
    time = daemon.getTime();
    boost::replace_first(response->content, "<!TM />", stringizeTime(time));
    daemon.getSpacecraftAOSLOS(&aos, &los);
    if (aos.asTime_t() > 0 && los.asTime_t() > 0) {
        if (aos <= time && time <= los) {
            boost::replace_first(response->content, "<!DA />", colorizeSpan("red", "<!DA />"));
            boost::replace_first(response->content, "<!DL />", colorizeSpan("green", "<!DL />"));
        }
    }
    if (aos.asTime_t() > 0) {
        boost::replace_first(response->content, "<!AS />", stringizeTime(aos));
        boost::replace_first(response->content, "<!DA />", stringizeTimeDiff(aos - time));
    }
    else {
        boost::replace_first(response->content, "<!AS />", DEFAULT_TIME);
        boost::replace_first(response->content, "<!DA />", DEFAULT_TIMEDIFF);
    }
    if (los.asTime_t() > 0) {
        boost::replace_first(response->content, "<!LS />", stringizeTime(los));
        boost::replace_first(response->content, "<!DL />", stringizeTimeDiff(los - time));
    }
    else {
        boost::replace_first(response->content, "<!LS />", DEFAULT_TIME);
        boost::replace_first(response->content, "<!DL />", DEFAULT_TIMEDIFF);
    }
    daemon.getSpacecraftMEL(&y);
    boost::replace_first(response->content, "<!ML />", colorizeSpan("magenta", stringizeElevation(y)));
    daemon.getRotatorStart(&aos);
    if (aos.asTime_t() > 0) {
        boost::replace_first(response->content, "<!RS />", stringizeTime(aos));
        boost::replace_first(response->content, "<!DR />", stringizeTimeDiff(aos - time));
    }
    else {
        boost::replace_first(response->content, "<!RS />", DEFAULT_TIME);
        boost::replace_first(response->content, "<!DR />", DEFAULT_TIMEDIFF);
    }
    return;
}

/*private virtual */tgs::TGSError ASDServerOperation::onRequest(RequestRec const& request, ResponseRec* response)
{
    insensitive::map<std::string, CacheRec>::const_iterator it;
    
    if ((it = _cache.find(request.path)) != _cache.end()) {
        if (!it->second.mime.empty()) {
            response->header["Content-Type"] = it->second.mime;
        }
        if (!it->second.cache.empty()) {
            response->header["Cache-Control"] = it->second.cache;
        }
        response->content = it->second.content;
        if (it->second.function != NULL) {
            (this->*it->second.function)(request, response);
        }
    }
    else {
        replyStatus(Server::response::not_found, response);
    }
    return tgs::TGSERROR_OK;
}

/*private */void ASDServerOperation::executeRoot(std::string const& session, std::string const& host, insensitive::map<std::string, std::string> const& query, std::string* category, std::string* message, bool shrink[]) const
{
    artsatd& daemon(artsatd::getInstance());
    insensitive::map<std::string, std::string>::const_iterator it;
    bool exclusive;
    ASDDeviceRotator::DataRec rotator;
    boost::shared_ptr<ASDPluginInterface> plugin;
    int value;
    int i;
    tgs::TGSError error;
    
    if ((it = query.find("session")) != query.end()) {
        if (it->second == "reload") {
            bindError(CATEGORY_SESSION, tgs::TGSERROR_OK, category, message);
        }
        else if (it->second == "session") {
            daemon.getSession(session, &value, NULL, NULL, NULL);
            bindError(CATEGORY_SESSION, daemon.controlSession(session, value <= 0, host), category, message);
        }
        else if (it->second == "exclusive") {
            daemon.getSession(session, NULL, &exclusive, NULL, NULL);
            bindError(CATEGORY_SESSION, daemon.excludeSession(session, !exclusive), category, message);
        }
    }
    else if ((it = query.find("shrink")) != query.end()) {
        for (i = 0; i < lengthof(g_shrink); ++i) {
            if (it->second == g_shrink[i]) {
                shrink[i] = !shrink[i];
                break;
            }
        }
    }
    else if ((it = query.find("manual")) != query.end()) {
        if (it->second == "rotator") {
            bindError(CATEGORY_HARDWARE, daemon.setManualRotator(session, !daemon.getManualRotator()), category, message);
        }
        else if (it->second == "transceiver") {
            bindError(CATEGORY_HARDWARE, daemon.setManualTransceiver(session, !daemon.getManualTransceiver()), category, message);
        }
        else if (it->second == "tnc") {
            bindError(CATEGORY_HARDWARE, daemon.setManualTNC(session, !daemon.getManualTNC()), category, message);
        }
    }
    else if ((it = query.find("azimuth")) != query.end()) {
        if ((error = valueizeAzimuth(it->second, &value)) == tgs::TGSERROR_OK) {
            error = daemon.controlManualRotator(session, &artsatd::controlRotatorAzimuth, &value);
        }
        bindError(CATEGORY_HARDWARE, error, category, message);
    }
    else if ((it = query.find("azimuth_offset")) != query.end()) {
        if ((error = valueizeOffset(it->second, &value)) == tgs::TGSERROR_OK) {
            artsatd::getRotator().getData(&rotator);
            if (rotator.azimuth >= 0) {
                value += rotator.azimuth;
                if ((value %= 360) < 0) {
                    value += 360;
                }
                error = daemon.controlManualRotator(session, &artsatd::controlRotatorAzimuth, &value);
            }
            else {
                error = tgs::TGSERROR_INVALID_STATE;
            }
        }
        bindError(CATEGORY_HARDWARE, error, category, message);
    }
    else if ((it = query.find("elevation")) != query.end()) {
        if ((error = valueizeElevation(it->second, &value)) == tgs::TGSERROR_OK) {
            error = daemon.controlManualRotator(session, &artsatd::controlRotatorElevation, &value);
        }
        bindError(CATEGORY_HARDWARE, error, category, message);
    }
    else if ((it = query.find("elevation_offset")) != query.end()) {
        if ((error = valueizeOffset(it->second, &value)) == tgs::TGSERROR_OK) {
            artsatd::getRotator().getData(&rotator);
            if (rotator.elevation >= 0) {
                value += rotator.elevation;
                if (value < 0) {
                    value = 0;
                }
                else if (value > 90) {
                    value = 90;
                }
                error = daemon.controlManualRotator(session, &artsatd::controlRotatorElevation, &value);
            }
            else {
                error = tgs::TGSERROR_INVALID_STATE;
            }
        }
        bindError(CATEGORY_HARDWARE, error, category, message);
    }
    else if ((it = query.find("transceiver")) != query.end()) {
        if (it->second == "CW") {
            bindError(CATEGORY_HARDWARE, daemon.controlManualTransceiver(session, &artsatd::controlTransceiverModeCW, NULL), category, message);
        }
        else if (it->second == "FM") {
            bindError(CATEGORY_HARDWARE, daemon.controlManualTransceiver(session, &artsatd::controlTransceiverModeFM, NULL), category, message);
        }
        else {
            bindError(CATEGORY_HARDWARE, tgs::TGSERROR_INVALID_PARAM, category, message);
        }
    }
    else if ((it = query.find("sender")) != query.end()) {
        if ((error = valueizeFrequency(it->second, &value)) == tgs::TGSERROR_OK) {
            error = daemon.controlManualTransceiver(session, &artsatd::controlTransceiverSender, &value);
        }
        bindError(CATEGORY_HARDWARE, error, category, message);
    }
    else if ((it = query.find("receiver")) != query.end()) {
        if ((error = valueizeFrequency(it->second, &value)) == tgs::TGSERROR_OK) {
            error = daemon.controlManualTransceiver(session, &artsatd::controlTransceiverReceiver, &value);
        }
        bindError(CATEGORY_HARDWARE, error, category, message);
    }
    else if ((it = query.find("tnc")) != query.end()) {
        if (it->second == "Command") {
            bindError(CATEGORY_HARDWARE, daemon.controlManualTNC(session, &artsatd::controlTNCModeCommand, NULL), category, message);
        }
        else if (it->second == "Converse") {
            bindError(CATEGORY_HARDWARE, daemon.controlManualTNC(session, &artsatd::controlTNCModeConverse, NULL), category, message);
        }
        else {
            bindError(CATEGORY_HARDWARE, tgs::TGSERROR_INVALID_PARAM, category, message);
        }
    }
    else if ((it = query.find("packet")) != query.end()) {
        bindError(CATEGORY_HARDWARE, daemon.controlManualTNC(session, &artsatd::controlTNCPacket, &it->second), category, message);
    }
    else if ((it = query.find("exnorad")) != query.end()) {
        bindError(CATEGORY_DATABASE, daemon.setEXNORAD(session, it->second), category, message);
    }
    else if ((it = query.find("mode")) != query.end()) {
        bindError(CATEGORY_CONTROL, daemon.setMode(session, it->second), category, message);
    }
    if ((plugin = daemon.getPlugin(daemon.getEXNORAD())) != NULL) {
        plugin->execute(session, query, category, message);
    }
    return;
}

/*private */void ASDServerOperation::processRoot(std::string const& session, std::string const& host, std::string const& category, std::string const& message, bool const shrink[], std::string* response) const
{
    static boost::xpressive::sregex const s_ESS(boost::xpressive::sregex::compile("<!\\[ESS />.*?<!\\]ESS />"));
    static boost::xpressive::sregex const s_HWT(boost::xpressive::sregex::compile("<!\\[HWF />.*?<!\\]HWF />"));
    static boost::xpressive::sregex const s_HWF(boost::xpressive::sregex::compile("<!\\[HWT />.*?<!\\]HWT />"));
    static boost::xpressive::sregex const s_MNR(boost::xpressive::sregex::compile("<!\\[MNR />.*?<!\\]MNR />"));
    static boost::xpressive::sregex const s_MNT(boost::xpressive::sregex::compile("<!\\[MNT />.*?<!\\]MNT />"));
    static boost::xpressive::sregex const s_MNM(boost::xpressive::sregex::compile("<!\\[MNM />.*?<!\\]MNM />"));
    static boost::xpressive::sregex const s_EHW(boost::xpressive::sregex::compile("<!\\[EHW />.*?<!\\]EHW />"));
    static boost::xpressive::sregex const s_DBT(boost::xpressive::sregex::compile("<!\\[DBF />.*?<!\\]DBF />"));
    static boost::xpressive::sregex const s_DBF(boost::xpressive::sregex::compile("<!\\[DBT />.*?<!\\]DBT />"));
    static boost::xpressive::sregex const s_EDB(boost::xpressive::sregex::compile("<!\\[EDB />.*?<!\\]EDB />"));
    static boost::xpressive::sregex const s_STT(boost::xpressive::sregex::compile("<!\\[STF />.*?<!\\]STF />"));
    static boost::xpressive::sregex const s_STF(boost::xpressive::sregex::compile("<!\\[STT />.*?<!\\]STT />"));
    static boost::xpressive::sregex const s_CNT(boost::xpressive::sregex::compile("<!\\[CNF />.*?<!\\]CNF />"));
    static boost::xpressive::sregex const s_CNF(boost::xpressive::sregex::compile("<!\\[CNT />.*?<!\\]CNT />"));
    static boost::xpressive::sregex const s_ECN(boost::xpressive::sregex::compile("<!\\[ECN />.*?<!\\]ECN />"));
    static boost::xpressive::sregex const s_tag(boost::xpressive::sregex::compile("<![^<>]+? />"));
    artsatd& daemon(artsatd::getInstance());
    int owner;
    bool exclusive;
    int exnorad;
    tgs::TGSPhysicsDatabase database;
    tgs::TGSPhysicsDatabase::FieldRec field;
    boost::shared_ptr<ASDPluginInterface> plugin;
    std::string string;
    tgs::TGSError error;
    
    daemon.getSession(session, &owner, &exclusive, &string, NULL);
    boost::replace_first(*response, "<!VS />", artsatd::getVersion());
    if (owner > 0) {
        boost::replace_first(*response, "<!PM />", "green");
        if (exclusive) {
            boost::replace_first(*response, "<!EX />", "green");
            boost::replace_first(*response, "<!SI />", "You are controlling the ground station exclusively.");
        }
        else {
            boost::replace_first(*response, "<!SI />", "You are controlling the ground station.");
        }
    }
    else if (owner < 0) {
        boost::replace_first(*response, "<!PM />", "red");
        if (exclusive) {
            boost::replace_first(*response, "<!_DP />", FORM_DISABLED);
            boost::replace_first(*response, "<!EX />", "red");
            boost::replace_first(*response, "<!SI />", string + " is controlling the ground station exclusively.");
        }
        else {
            boost::replace_first(*response, "<!SI />", string + " is controlling the ground station.");
        }
    }
    else {
        boost::replace_first(*response, "<!SI />", "The ground station is on standby.");
    }
    if (category == CATEGORY_SESSION && !message.empty()) {
        boost::replace_first(*response, "<!ES />", message);
    }
    else {
        *response = boost::xpressive::regex_replace(*response, s_ESS, "");
    }
    if (!shrink[0]) {
        boost::replace_first(*response, "<!HW />", SHRINK_HIDE);
        *response = boost::xpressive::regex_replace(*response, s_HWT, "");
        if (daemon.getManualRotator()) {
            boost::replace_first(*response, "<!MR />", "red");
        }
        else {
            *response = boost::xpressive::regex_replace(*response, s_MNR, "");
        }
        if (daemon.getManualTransceiver()) {
            boost::replace_first(*response, "<!MT />", "red");
        }
        else {
            *response = boost::xpressive::regex_replace(*response, s_MNT, "");
        }
        if (daemon.getManualTNC()) {
            boost::replace_first(*response, "<!MM />", "red");
        }
        else {
            *response = boost::xpressive::regex_replace(*response, s_MNM, "");
        }
        if (category == CATEGORY_HARDWARE && !message.empty()) {
            boost::replace_first(*response, "<!EH />", message);
        }
        else {
            *response = boost::xpressive::regex_replace(*response, s_EHW, "");
        }
    }
    else {
        boost::replace_first(*response, "<!HW />", SHRINK_SHOW);
        *response = boost::xpressive::regex_replace(*response, s_HWF, "");
    }
    boost::replace_first(*response, "<!HR />", (artsatd::getRotator().isValid()) ? (DEVICE_OK) : (DEVICE_NG));
    boost::replace_first(*response, "<!HT />", (artsatd::getTransceiver().isValid()) ? (DEVICE_OK) : (DEVICE_NG));
    boost::replace_first(*response, "<!HM />", (artsatd::getTNC().isValid()) ? (DEVICE_OK) : (DEVICE_NG));
    exnorad = daemon.getEXNORAD();
    boost::replace_first(*response, "<!ND />", stringizeEXNORAD(exnorad));
    if (category == CATEGORY_DATABASE && !message.empty()) {
        boost::replace_first(*response, "<!ED />", message);
    }
    else {
        *response = boost::xpressive::regex_replace(*response, s_EDB, "");
    }
    string = daemon.getMode();
    if (!shrink[1]) {
        boost::replace_first(*response, "<!DB />", SHRINK_HIDE);
        *response = boost::xpressive::regex_replace(*response, s_DBT, "");
        field.exnorad = -1;
        field.beacon.frequency = -1;
        field.beacon.drift = INT_MIN;
        field.sender.frequency = -1;
        field.sender.drift = INT_MIN;
        field.receiver.frequency = -1;
        field.receiver.drift = INT_MIN;
        if ((error = database.open(_database)) == tgs::TGSERROR_OK) {
            if (database.getField(exnorad, &field) == tgs::TGSERROR_OK) {
                boost::replace_first(*response, "<!NM />", colorizeSpan("green", field.name));
            }
            else {
                boost::replace_first(*response, "<!NM />", "Unknown EXNORAD");
            }
            database.close();
        }
        else {
            boost::replace_first(*response, "<!NM />", colorizeSpan("red", "Database Error " + error.print()));
        }
        boost::replace_first(*response, "<!CS />", stringizeCallsign(field.callsign));
        boost::replace_first(*response, "<!BM />", stringizeMode(field.beacon.mode));
        boost::replace_first(*response, "<!BF />", colorizeSpan((string == "CW_TEST") ? ("green") : (""), stringizeFrequency(field.beacon.frequency, false)));
        boost::replace_first(*response, "<!BD />", stringizeDrift(field.beacon.drift));
        boost::replace_first(*response, "<!SM />", stringizeMode(field.sender.mode));
        boost::replace_first(*response, "<!SF />", colorizeSpan((string == "FM_TEST") ? ("green") : (""), stringizeFrequency(field.sender.frequency, host != "127.0.0.1" && owner <= 0)));
        boost::replace_first(*response, "<!SD />", stringizeDrift(field.sender.drift));
        boost::replace_first(*response, "<!RM />", stringizeMode(field.receiver.mode));
        boost::replace_first(*response, "<!RF />", colorizeSpan((string == "FM_TEST") ? ("green") : (""), stringizeFrequency(field.receiver.frequency, false)));
        boost::replace_first(*response, "<!RD />", stringizeDrift(field.receiver.drift));
        boost::replace_first(*response, "<!OT />", stringizeOrbitType(field.orbit));
        boost::replace_first(*response, "<!OU />", (field.exnorad >= 0) ? (stringizeTime(field.time + ir::IRXTimeDiff::localTimeOffset())) : (DEFAULT_TIME));
        boost::replace_first(*response, "<!OD />", stringizeOrbitData(field.orbit));
    }
    else {
        boost::replace_first(*response, "<!DB />", SHRINK_SHOW);
        *response = boost::xpressive::regex_replace(*response, s_DBF, "");
    }
    if (!shrink[2]) {
        boost::replace_first(*response, "<!ST />", SHRINK_HIDE);
        *response = boost::xpressive::regex_replace(*response, s_STT, "");
    }
    else {
        boost::replace_first(*response, "<!ST />", SHRINK_SHOW);
        *response = boost::xpressive::regex_replace(*response, s_STF, "");
    }
    boost::replace_first(*response, "<!" + ((!string.empty()) ? (string) : ("NO")) + " />", "green");
    if (category == CATEGORY_CONTROL && !message.empty()) {
        boost::replace_first(*response, "<!EC />", message);
    }
    else {
        *response = boost::xpressive::regex_replace(*response, s_ECN, "");
    }
    if (!shrink[3]) {
        boost::replace_first(*response, "<!CN />", SHRINK_HIDE);
        *response = boost::xpressive::regex_replace(*response, s_CNT, "");
        string.clear();
        if ((plugin = daemon.getPlugin(exnorad)) != NULL) {
            plugin->process(session, category, message, &string);
        }
        boost::replace_first(*response, "<!CD />", string);
    }
    else {
        boost::replace_first(*response, "<!CN />", SHRINK_SHOW);
        *response = boost::xpressive::regex_replace(*response, s_CNF, "");
    }
    if (owner <= 0) {
        boost::replace_all(*response, "<!_DF />", FORM_DISABLED);
        boost::replace_all(*response, "<!_RO />", FORM_READONLY);
    }
    *response = boost::xpressive::regex_replace(*response, s_tag, "");
    return;
}

/*private static */tgs::TGSError ASDServerOperation::serializeCache(std::string const& file, std::string* result)
{
    std::ifstream stream;
    std::string line;
    std::string string;
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    stream.open(file.c_str());
    if (stream.is_open()) {
        while (std::getline(stream, line)) {
            boost::trim(line);
            string += line;
        }
        *result = string;
        stream.close();
    }
    else {
        error = tgs::TGSERROR_FILE_NOTFOUND;
    }
    return error;
}

/*private static */tgs::TGSError ASDServerOperation::valueizeAzimuth(std::string const& param, int* result)
{
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    if (param == "N") {
        *result = 0;
    }
    else if (param == "NE") {
        *result = 45;
    }
    else if (param == "E") {
        *result = 90;
    }
    else if (param == "SE") {
        *result = 135;
    }
    else if (param == "S") {
        *result = 180;
    }
    else if (param == "SW") {
        *result = 225;
    }
    else if (param == "W") {
        *result = 270;
    }
    else if (param == "NW") {
        *result = 315;
    }
    else if (1 <= param.size() && param.size() <= 3 && boost::all(param, boost::is_digit())) {
        *result = boost::lexical_cast<int>(param);
    }
    else {
        error = tgs::TGSERROR_INVALID_PARAM;
    }
    return error;
}

/*private static */tgs::TGSError ASDServerOperation::valueizeElevation(std::string const& param, int* result)
{
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    if (param == "0°") {
        *result = 0;
    }
    else if (param == "45°") {
        *result = 45;
    }
    else if (param == "90°") {
        *result = 90;
    }
    else if (1 <= param.size() && param.size() <= 3 && boost::all(param, boost::is_digit())) {
        *result = boost::lexical_cast<int>(param);
    }
    else {
        error = tgs::TGSERROR_INVALID_PARAM;
    }
    return error;
}

/*private static */tgs::TGSError ASDServerOperation::valueizeFrequency(std::string const& param, int* result)
{
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    if (1 <= param.size() && param.size() <= 10 && boost::all(param, boost::is_any_of(".0123456789"))) {
        try {
            *result = static_cast<int>(boost::lexical_cast<double>(param) * 1000000.0);
        }
        catch (...) {
            error = tgs::TGSERROR_INVALID_PARAM;
        }
    }
    else {
        error = tgs::TGSERROR_INVALID_PARAM;
    }
    return error;
}

/*private static */tgs::TGSError ASDServerOperation::valueizeOffset(std::string const& param, int* result)
{
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    if (1 <= param.size() && boost::all(param, boost::is_any_of("+-0123456789"))) {
        try {
            *result = boost::lexical_cast<int>(param);
        }
        catch (...) {
            error = tgs::TGSERROR_INVALID_PARAM;
        }
    }
    else {
        error = tgs::TGSERROR_INVALID_PARAM;
    }
    return error;
}

/*private static */void ASDServerOperation::bindError(std::string const& name, tgs::TGSError error, std::string* category, std::string* message)
{
    *category = name;
    if (error == tgs::TGSERROR_OK) {
        message->clear();
    }
    else {
        *message = error.print();
    }
    return;
}

/*private static */std::string ASDServerOperation::colorizeSpan(std::string const& color, std::string const& string)
{
    return (!color.empty()) ? ("<span class='" + color + "'>" + string + "</span>") : (string);
}

/*private static */std::string ASDServerOperation::stringizeLatitude(double param)
{
    std::string result("-      -");
    
    if (!std::isnan(param)) {
        result = (boost::format("%s %6.3lf") % ((param >= 0.0) ? ("N") : ("S")) % std::abs(param)).str();
    }
    return result + "&deg;";
}

/*private static */std::string ASDServerOperation::stringizeLongitude(double param)
{
    std::string result("-       -");
    
    if (!std::isnan(param)) {
        result = (boost::format("%s %7.3lf") % ((param >= 0.0) ? ("E") : ("W")) % std::abs(param)).str();
    }
    return result + "&deg;";
}

/*private static */std::string ASDServerOperation::stringizeAltitude(double param)
{
    std::string result("      -");
    
    if (!std::isnan(param)) {
        result = (boost::format("%7.3lf") % param).str();
    }
    return result + " km";
}

/*private static */std::string ASDServerOperation::stringizeAzimuth(int param)
{
    std::string result("  -");
    
    if (param >= 0) {
        result = (boost::format("%3d") % param).str();
    }
    return result + "&deg;";
}

/*private static */std::string ASDServerOperation::stringizeAzimuth(double param)
{
    std::string result("      -");
    
    if (!std::isnan(param)) {
        result = (boost::format("%7.3lf") % param).str();
    }
    return result + "&deg;";
}

/*private static */std::string ASDServerOperation::stringizeElevation(int param)
{
    std::string result(" -");
    
    if (param >= 0) {
        result = (boost::format("%2d") % param).str();
    }
    return result + "&deg;";
}

/*private static */std::string ASDServerOperation::stringizeElevation(double param)
{
    std::string result("      -");
    
    if (!std::isnan(param)) {
        result = (boost::format("%+7.3lf") % param).str();
    }
    return result + "&deg;";
}

/*private static */std::string ASDServerOperation::stringizeEXNORAD(int param)
{
    std::string result;
    
    if (param >= 0) {
        result = (boost::format("%05d") % param).str();
    }
    return result;
}

/*private static */std::string ASDServerOperation::stringizeCallsign(std::string const& param)
{
    return (!param.empty()) ? (param) : ("-");
}

/*private static */std::string ASDServerOperation::stringizeMode(std::string const& param)
{
    return (!param.empty()) ? (param) : ("-");
}

/*private static */std::string ASDServerOperation::stringizeFrequency(int param, bool secret)
{
    static boost::xpressive::sregex const s_secret(boost::xpressive::sregex::compile("[-0-9]"));
    std::string result("         -");
    
    if (param >= 0) {
        result = (boost::format("%10.6lf") % (param / 1000000.0)).str();
        if (secret) {
            result = boost::xpressive::regex_replace(result, s_secret, "*");
        }
    }
    return result + " MHz";
}

/*private static */std::string ASDServerOperation::stringizeFrequency(double param, bool secret)
{
    static boost::xpressive::sregex const s_secret(boost::xpressive::sregex::compile("[-0-9]"));
    std::string result("         -");
    
    if (!std::isnan(param)) {
        result = (boost::format("%10.6lf") % (param / 1000000.0)).str();
        if (secret) {
            result = boost::xpressive::regex_replace(result, s_secret, "*");
        }
    }
    return result + " MHz";
}

/*private static */std::string ASDServerOperation::stringizeDrift(int param)
{
    std::string result("         -");
    
    if (param != INT_MIN) {
        result = (boost::format("%+10d") % param).str();
    }
    return result + " Hz";
}

/*private static */std::string ASDServerOperation::stringizeDopplerShift(double param)
{
    std::string result("         -");
    
    if (!std::isnan(param)) {
        result = (boost::format("%10.6lf") % (param * 100.0)).str();
    }
    return result + " %";
}

/*private static */std::string ASDServerOperation::stringizeOrbitType(tgs::OrbitData const& param)
{
    std::string result;
    
    switch (param.getType()) {
        case tgs::OrbitData::TYPE_TLE:
            result = "TLE";
            break;
        case tgs::OrbitData::TYPE_SCD:
            result = "SCD";
            break;
        default:
            result = "Unknown";
            break;
    }
    return result;
}

/*private static */std::string ASDServerOperation::stringizeOrbitData(tgs::OrbitData const& param)
{
    std::string result;
    
    switch (param.getType()) {
        case tgs::OrbitData::TYPE_TLE:
            result  = (strlen(static_cast<tgs::TLERec const&>(param).one) > 0) ? (static_cast<tgs::TLERec const&>(param).one) : ("1                                                                    ");
            result += "<br />";
            result += (strlen(static_cast<tgs::TLERec const&>(param).two) > 0) ? (static_cast<tgs::TLERec const&>(param).two) : ("2                                                                    ");
            break;
        case tgs::OrbitData::TYPE_SCD:
            result  = (!static_cast<tgs::SCDRec const&>(param).info.empty()) ? (static_cast<tgs::SCDRec const&>(param).info) : ("-");
            result += "<br />";
            result += (!static_cast<tgs::SCDRec const&>(param).param.empty()) ? (static_cast<tgs::SCDRec const&>(param).param) : ("-");
            break;
        default:
            result  = "-";
            result += "<br />";
            result += "-";
            break;
    }
    return result;
}

/*private static */std::string ASDServerOperation::stringizeTime(ir::IRXTime const& param)
{
    return param.format("%YYYY/%MM/%DD %hh:%mm:%ss JST");
}

/*private static */std::string ASDServerOperation::stringizeTimeDiff(ir::IRXTimeDiff const& param)
{
    long hour;
    int minute;
    int second;
    std::string result;
    
    param.get(&hour, &minute, &second);
    result  = (param >= ir::IRXTimeDiff(0)) ? (" ") : ("-");
    result += (boost::format("%02d:%02d:%02d") % std::abs(hour) % std::abs(minute) % std::abs(second)).str();
    return "&Delta;" + result;
}

/*private static */std::string ASDServerOperation::stringizeSession(std::string const& param)
{
    std::vector<std::string> fragment;
    std::string result("---.---.---.---");
    
    boost::split(fragment, param, boost::is_any_of("."));
    if (fragment.size() == 4) {
        result = (boost::format("%3s.%3s.%3s.%3s") % fragment[0] % fragment[1] % fragment[2] % fragment[3]).str();
    }
    return result;
}

/*private static */std::string ASDServerOperation::stringizeOnline(int param)
{
    return (param >= 0) ? ((boost::format("%d") % param).str()) : ("-");
}
