/*
**      IridiumFrameworks
**
**      Original Copyright (C) 2013 - 2014 HORIGUCHI Junshi.
**                                          http://iridium.jp/
**                                          zap00365@nifty.com
**      Portions Copyright (C) <year> <author>
**                                          <website>
**                                          <e-mail>
**      Version     POSIX / C++03
**      Website     http://iridium.jp/
**      E-mail      zap00365@nifty.com
**
**      This source code is for Xcode.
**      Xcode 5.1.1 (Apple LLVM 5.1)
**
**      artsatd.cpp
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

#include "artsatd.h"
#include "ASDPluginINVADER.h"
#include "TGSRotatorGS232B.h"
#include "TGSTransceiverIC9100.h"
#include "TGSTNCTNC555.h"
#include "ASDTLEClientCelestrak.h"

#define VERSION_STRING                          ("4.4.2")
#define PATH_WORKSPACE                          ("/etc")
#define PATH_SERVER                             ("server")
#define PATH_PLUGIN                             ("plugin")
#define XML_CONFIG                              ("config.xml")
#define XML_PLUGIN                              ("plugin.xml")
#define XML_DEVICE                              ("device.xml")
#define XML_NETWORK                             ("network.xml")
#define DATABASE_PHYSICS                        ("physics.db")
#define DEFAULT_SERVER_DATABASE_PORT            ("16781")
#define DEFAULT_SERVER_DATABASE_LISTEN          (1)
#define DEFAULT_SERVER_OPERATION_PORT           ("16780")
#define DEFAULT_SERVER_OPERATION_LISTEN         (4)
#define DEFAULT_SERVER_RPC_PORT                 ("16782")
#define DEFAULT_SERVER_RPC_LISTEN               (4)
#define DEFAULT_SESSION_MAXIMUM                 (16)
#define DEFAULT_SESSION_TIMEOUT                 (300)
#define DEFAULT_SESSION_LOCALONLY               (true)
#define DEFAULT_OBSERVER_CALLSIGN               ("JQ1ZKL")
#define DEFAULT_OBSERVER_LATITUDE               (35.610603)
#define DEFAULT_OBSERVER_LONGITUDE              (139.351124)
#define DEFAULT_OBSERVER_ALTITUDE               (0.160)
#define DEFAULT_CW_TEST_AZIMUTH                 (0)
#define DEFAULT_CW_TEST_ELEVATION               (90)
#define DEFAULT_FM_TEST_AZIMUTH                 (0)
#define DEFAULT_FM_TEST_ELEVATION               (90)
#define DEFAULT_ALGORITHM_LOOKAHEAD             (1)
#define DEFAULT_INTERVAL_SESSION                (1)
#define DEFAULT_INTERVAL_ROTATOR                (1)
#define DEFAULT_INTERVAL_TRANSCEIVER            (1)
#define DEFAULT_INTERVAL_TNC                    (1)
#define DEFAULT_INTERVAL_LOG                    (10)
#define LOG_SEPARATOR                           ("================================================================")
#define LOOP_INTERVAL                           (250000)
//<<<
#define ROTATOR_MIN_AZIMUTH                     (0.0)
#define ROTATOR_MAX_AZIMUTH                     (450.0)
#define ROTATOR_DEG_PER_SEC                     (6.0)
//>>>

IRXDAEMON_STATIC(&artsatd::getInstance())

/*private */artsatd::artsatd(void)
{
}

/*private virtual */artsatd::~artsatd(void)
{
    terminate();
}

/*public static */std::string const& artsatd::getVersion(void)
{
    static std::string const s_version(VERSION_STRING);
    
    return s_version;
}

/*public static */artsatd& artsatd::getInstance(void)
{
    static artsatd s_instance;
    
    return s_instance;
}

/*public */void artsatd::getSession(std::string const& session, int* owner, bool* exclusive, std::string* host, int* online) const
{
    int rstown(0);
    bool rstecl(false);
    std::string rsthst;
    
    boost::shared_lock<boost::shared_mutex> rlock(_mutex_session);
    if (!_session.owner.empty()) {
        rstown = (_session.owner == session) ? (+1) : (-1);
        rstecl = _session.exclusive;
        rsthst = _session.host;
    }
    if (online != NULL) {
        *online = _session.id.size();
    }
    rlock.unlock();
    if (owner != NULL) {
        *owner = rstown;
    }
    if (exclusive != NULL) {
        *exclusive = rstecl;
    }
    if (host != NULL) {
        *host = rsthst;
    }
    return;
}

/*public */tgs::TGSError artsatd::setManualRotator(std::string const& session, bool manual)
{
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    if (!session.empty()) {
        boost::shared_lock<boost::shared_mutex> rlock(_mutex_session);
        if (_session.owner == session) {
            boost::unique_lock<boost::shared_mutex> wlock(_mutex_control);
            _control.manualRotator = manual;
        }
        else {
            error = tgs::TGSERROR_INVALID_SESSION;
        }
    }
    else {
        error = tgs::TGSERROR_INVALID_PARAM;
    }
    return error;
}

/*public */bool artsatd::getManualRotator(void) const
{
    boost::shared_lock<boost::shared_mutex> rlock(_mutex_control);
    return _control.manualRotator;
}

/*public */tgs::TGSError artsatd::setManualTransceiver(std::string const& session, bool manual)
{
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    if (!session.empty()) {
        boost::shared_lock<boost::shared_mutex> rlock(_mutex_session);
        if (_session.owner == session) {
            boost::unique_lock<boost::shared_mutex> wlock(_mutex_control);
            _control.manualTransceiver = manual;
        }
        else {
            error = tgs::TGSERROR_INVALID_SESSION;
        }
    }
    else {
        error = tgs::TGSERROR_INVALID_PARAM;
    }
    return error;
}

/*public */bool artsatd::getManualTransceiver(void) const
{
    boost::shared_lock<boost::shared_mutex> rlock(_mutex_control);
    return _control.manualTransceiver;
}

/*public */tgs::TGSError artsatd::setManualTNC(std::string const& session, bool manual)
{
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    if (!session.empty()) {
        boost::shared_lock<boost::shared_mutex> rlock(_mutex_session);
        if (_session.owner == session) {
            boost::unique_lock<boost::shared_mutex> wlock(_mutex_control);
            _control.manualTNC = manual;
        }
        else {
            error = tgs::TGSERROR_INVALID_SESSION;
        }
    }
    else {
        error = tgs::TGSERROR_INVALID_PARAM;
    }
    return error;
}

/*public */bool artsatd::getManualTNC(void) const
{
    boost::shared_lock<boost::shared_mutex> rlock(_mutex_control);
    return _control.manualTNC;
}

/*public */tgs::TGSError artsatd::setNORAD(std::string const& session, std::string const& query)
{
    tgs::TGSPhysicsDatabase database;
    std::vector<tgs::TGSPhysicsDatabase::FieldRec> field;
    std::vector<tgs::TGSPhysicsDatabase::FieldRec>::const_iterator it;
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    if (query.empty()) {
        error = setNORAD(session, -1);
    }
    else if (query.size() <= 5 && boost::all(query, boost::is_digit())) {
        error = setNORAD(session, boost::lexical_cast<int>(query));
    }
    else if ((error = database.open(DATABASE_PHYSICS)) == tgs::TGSERROR_OK) {
        if ((error = database.getFieldByName(query, &field)) == tgs::TGSERROR_OK) {
            if (field.size() > 0) {
                for (it = field.begin(); it != field.end(); ++it) {
                    if (it->name == query) {
                        break;
                    }
                }
                error = setNORAD(session, (it != field.end()) ? (it->norad) : (field.front().norad));
            }
            else {
                error = tgs::TGSERROR_NO_RESULT;
            }
        }
    }
    return error;
}

/*public */tgs::TGSError artsatd::setNORAD(std::string const& session, int norad)
{
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    if (!session.empty()) {
        boost::shared_lock<boost::shared_mutex> rlock(_mutex_session);
        if (_session.owner == session) {
            boost::unique_lock<boost::shared_mutex> wlock(_mutex_control);
            _control.norad = (norad >= 0) ? (norad) : (-1);
        }
        else {
            error = tgs::TGSERROR_INVALID_SESSION;
        }
    }
    else {
        error = tgs::TGSERROR_INVALID_PARAM;
    }
    return error;
}

/*public */int artsatd::getNORAD(void) const
{
    boost::shared_lock<boost::shared_mutex> rlock(_mutex_control);
    return _control.norad;
}

/*public */tgs::TGSError artsatd::setMode(std::string const& session, std::string const& mode)
{
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    if (!session.empty()) {
        boost::shared_lock<boost::shared_mutex> rlock(_mutex_session);
        if (_session.owner == session) {
            boost::unique_lock<boost::shared_mutex> wlock(_mutex_control);
            if (mode == "CW") {
                _control.mode = MODE_CW;
            }
            else if (mode == "CW_TEST") {
                _control.mode = MODE_CW_TEST;
            }
            else if (mode == "FM") {
                _control.mode = MODE_FM;
            }
            else if (mode == "FM_TEST") {
                _control.mode = MODE_FM_TEST;
            }
            else {
                _control.mode = MODE_LIMIT;
            }
        }
        else {
            error = tgs::TGSERROR_INVALID_SESSION;
        }
    }
    else {
        error = tgs::TGSERROR_INVALID_PARAM;
    }
    return error;
}

/*public */std::string artsatd::getMode(void) const
{
    std::string result;
    
    boost::shared_lock<boost::shared_mutex> rlock(_mutex_control);
    switch (_control.mode) {
        case MODE_CW:
            result = "CW";
            break;
        case MODE_CW_TEST:
            result = "CW_TEST";
            break;
        case MODE_FM:
            result = "FM";
            break;
        case MODE_FM_TEST:
            result = "FM_TEST";
            break;
        default:
            // nop
            break;
    }
    return result;
}

/*public */ir::IRXTime artsatd::getTime(void) const
{
    boost::shared_lock<boost::shared_mutex> rlock(_mutex_current);
    return _current.time;
}

/*public */boost::shared_ptr<ASDPluginInterface> artsatd::getPlugin(int norad) const
{
    std::map<int, boost::shared_ptr<ASDPluginInterface> >::const_iterator it;
    boost::shared_ptr<ASDPluginInterface> result;
    
    if (norad >= 0) {
        if ((it = _plugin.find(norad)) != _plugin.end()) {
            result = it->second;
        }
    }
    return result;
}

/*public */void artsatd::getObserverCallsign(std::string* callsign) const
{
    if (callsign != NULL) {
        *callsign = _config.observerCallsign;
    }
    return;
}

/*public */void artsatd::getObserverPosition(double* latitude, double* longitude, double* altitude) const
{
    if (latitude != NULL) {
        *latitude = _config.observerLatitude;
    }
    if (longitude != NULL) {
        *longitude = _config.observerLongitude;
    }
    if (altitude != NULL) {
        *altitude = _config.observerAltitude;
    }
    return;
}

/*public */void artsatd::getSatellitePosition(double* latitude, double* longitude, double* altitude) const
{
    boost::shared_lock<boost::shared_mutex> rlock(_mutex_monitor);
    if (latitude != NULL) {
        *latitude = _monitor.latitude;
    }
    if (longitude != NULL) {
        *longitude = _monitor.longitude;
    }
    if (altitude != NULL) {
        *altitude = _monitor.altitude;
    }
    return;
}

/*public */void artsatd::getSatelliteDirection(double* azimuth, double* elevation) const
{
    boost::shared_lock<boost::shared_mutex> rlock(_mutex_monitor);
    if (azimuth != NULL) {
        *azimuth = _monitor.azimuth;
    }
    if (elevation != NULL) {
        *elevation = _monitor.elevation;
    }
    return;
}

/*public */void artsatd::getSatelliteFrequency(double* beacon, double* sender, double* receiver) const
{
    boost::shared_lock<boost::shared_mutex> rlock(_mutex_monitor);
    if (beacon != NULL) {
        *beacon = _monitor.beacon;
    }
    if (sender != NULL) {
        *sender = _monitor.sender;
    }
    if (receiver != NULL) {
        *receiver = _monitor.receiver;
    }
    return;
}

/*public */void artsatd::getSatelliteDopplerShift(double* sender, double* receiver) const
{
    boost::shared_lock<boost::shared_mutex> rlock(_mutex_monitor);
    if (sender != NULL) {
        *sender = _monitor.dopplerSender;
    }
    if (receiver != NULL) {
        *receiver = _monitor.dopplerReceiver;
    }
    return;
}

/*public */void artsatd::getSatelliteAOSLOS(ir::IRXTime* aos, ir::IRXTime* los) const
{
    boost::shared_lock<boost::shared_mutex> rlock(_mutex_monitor);
    if (aos != NULL) {
        *aos = _monitor.aos;
    }
    if (los != NULL) {
        *los = _monitor.los;
    }
    return;
}

/*public */void artsatd::getSatelliteMEL(double* mel) const
{
    if (mel != NULL) {
        boost::shared_lock<boost::shared_mutex> rlock(_mutex_monitor);
        *mel = _monitor.mel;
    }
    return;
}

/*public */void artsatd::getRotatorStart(ir::IRXTime* start) const
{
    if (start != NULL) {
        boost::shared_lock<boost::shared_mutex> rlock(_mutex_monitor);
        *start = _monitor.start;
    }
    return;
}

/*public */tgs::TGSError artsatd::getError(void) const
{
    boost::shared_lock<boost::shared_mutex> rlock(_mutex_monitor);
    return _monitor.error;
}

/*public */tgs::TGSError artsatd::requestSession(std::string* session, bool* update)
{
    ir::IRXTime time;
    std::map<std::string, ir::IRXTime>::const_iterator it;
    std::string string;
    bool flag;
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    if (session != NULL) {
        boost::shared_lock<boost::shared_mutex> rlock(_mutex_current);
        time = _current.time;
        rlock.unlock();
        flag = false;
        boost::upgrade_lock<boost::shared_mutex> ulock(_mutex_session);
        if ((it = _session.id.find(*session)) != _session.id.end()) {
            string = it->first;
        }
        else if (_session.id.size() < _config.sessionMaximum) {
            do {
                string = (boost::format("%08x%08x") % time.asTime_t() % _session.random()).str();
            } while (_session.id.find(string) != _session.id.end());
            flag = true;
        }
        else {
            error = tgs::TGSERROR_INVALID_STATE;
        }
        if (error == tgs::TGSERROR_OK) {
            boost::upgrade_to_unique_lock<boost::shared_mutex> wlock(ulock);
            _session.id[string] = time;
        }
        ulock.unlock();
        if (error == tgs::TGSERROR_OK) {
            *session = string;
            if (update != NULL) {
                *update = flag;
            }
        }
    }
    else {
        error = tgs::TGSERROR_INVALID_PARAM;
    }
    return error;
}

/*public */tgs::TGSError artsatd::controlSession(std::string const& session, bool owner, std::string const& host)
{
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    if (!session.empty()) {
        boost::upgrade_lock<boost::shared_mutex> ulock(_mutex_session);
        if (_session.owner == session) {
            boost::upgrade_to_unique_lock<boost::shared_mutex> wlock(ulock);
            if (owner) {
                _session.host = host;
            }
            else {
                _session.owner.clear();
            }
        }
        else if (_session.owner.empty() || !_session.exclusive) {
            if (owner) {
                if (_config.sessionLocalonly) {
                    if (host != "127.0.0.1") {
                        error = tgs::TGSERROR_INVALID_SESSION;
                    }
                }
                if (error == tgs::TGSERROR_OK) {
                    boost::upgrade_to_unique_lock<boost::shared_mutex> wlock(ulock);
                    _session.owner = session;
                    _session.exclusive = false;
                    _session.host = host;
                }
            }
        }
        else {
            error = tgs::TGSERROR_INVALID_SESSION;
        }
    }
    else {
        error = tgs::TGSERROR_INVALID_PARAM;
    }
    return error;
}

/*public */tgs::TGSError artsatd::excludeSession(std::string const& session, bool exclusive)
{
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    if (!session.empty()) {
        boost::upgrade_lock<boost::shared_mutex> ulock(_mutex_session);
        if (_session.owner == session) {
            boost::upgrade_to_unique_lock<boost::shared_mutex> wlock(ulock);
            _session.exclusive = exclusive;
        }
        else {
            error = tgs::TGSERROR_INVALID_SESSION;
        }
    }
    else {
        error = tgs::TGSERROR_INVALID_PARAM;
    }
    return error;
}

/*public */tgs::TGSError artsatd::controlManualRotator(std::string const& session, tgs::TGSError (*function)(ASDDeviceRotator& rotator, void const* info), void const* info)
{
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    if (!session.empty() && function != NULL) {
        boost::shared_lock<boost::shared_mutex> slock(_mutex_session);
        if (_session.owner == session) {
            boost::shared_lock<boost::shared_mutex> clock(_mutex_control);
            if (_control.manualRotator) {
                error = (*function)(_rotator, info);
            }
            else {
                error = tgs::TGSERROR_INVALID_STATE;
            }
        }
        else {
            error = tgs::TGSERROR_INVALID_SESSION;
        }
    }
    else {
        error = tgs::TGSERROR_INVALID_PARAM;
    }
    return error;
}

/*public */tgs::TGSError artsatd::controlManualTransceiver(std::string const& session, tgs::TGSError (*function)(ASDDeviceTransceiver& transceiver, void const* info), void const* info)
{
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    if (!session.empty() && function != NULL) {
        boost::shared_lock<boost::shared_mutex> slock(_mutex_session);
        if (_session.owner == session) {
            boost::shared_lock<boost::shared_mutex> clock(_mutex_control);
            if (_control.manualTransceiver) {
                error = (*function)(_transceiver, info);
            }
            else {
                error = tgs::TGSERROR_INVALID_STATE;
            }
        }
        else {
            error = tgs::TGSERROR_INVALID_SESSION;
        }
    }
    else {
        error = tgs::TGSERROR_INVALID_PARAM;
    }
    return error;
}

/*public */tgs::TGSError artsatd::controlManualTNC(std::string const& session, tgs::TGSError (*function)(ASDDeviceTNC& tnc, void const* info), void const* info)
{
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    if (!session.empty() && function != NULL) {
        boost::shared_lock<boost::shared_mutex> slock(_mutex_session);
        if (_session.owner == session) {
            boost::shared_lock<boost::shared_mutex> clock(_mutex_control);
            if (_control.manualTNC) {
                error = (*function)(_tnc, info);
            }
            else {
                error = tgs::TGSERROR_INVALID_STATE;
            }
        }
        else {
            error = tgs::TGSERROR_INVALID_SESSION;
        }
    }
    else {
        error = tgs::TGSERROR_INVALID_PARAM;
    }
    return error;
}

/*public */tgs::TGSError artsatd::requestCommand(std::string const& session, std::string const& command)
{
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    if (!session.empty() && !command.empty()) {
        boost::shared_lock<boost::shared_mutex> rlock(_mutex_session);
        if (_session.owner == session) {
            boost::unique_lock<boost::shared_mutex> wlock(_mutex_command);
            _command.queue.push_back(command);
        }
        else {
            error = tgs::TGSERROR_INVALID_SESSION;
        }
    }
    else {
        error = tgs::TGSERROR_INVALID_PARAM;
    }
    return error;
}

/*public static */tgs::TGSError artsatd::controlRotatorAzimuth(ASDDeviceRotator& rotator, void const* info)
{
    return rotator->rotateAzimuthTo(*static_cast<int const*>(info));
}

/*public static */tgs::TGSError artsatd::controlRotatorElevation(ASDDeviceRotator& rotator, void const* info)
{
    return rotator->rotateElevationTo(*static_cast<int const*>(info));
}

/*public static */tgs::TGSError artsatd::controlTransceiverModeCW(ASDDeviceTransceiver& transceiver, void const* info)
{
    return transceiver->selectModeBeacon();
}

/*public static */tgs::TGSError artsatd::controlTransceiverModeFM(ASDDeviceTransceiver& transceiver, void const* info)
{
    return transceiver->selectModeCommunication();
}

/*public static */tgs::TGSError artsatd::controlTransceiverSender(ASDDeviceTransceiver& transceiver, void const* info)
{
    return transceiver->setFrequencySender(*static_cast<int const*>(info));
}

/*public static */tgs::TGSError artsatd::controlTransceiverReceiver(ASDDeviceTransceiver& transceiver, void const* info)
{
    return transceiver->setFrequencyReceiver(*static_cast<int const*>(info));
}

/*public static */tgs::TGSError artsatd::controlTNCModeCommand(ASDDeviceTNC& tnc, void const* info)
{
    return tnc->selectModeCommand();
}

/*public static */tgs::TGSError artsatd::controlTNCModeConverse(ASDDeviceTNC& tnc, void const* info)
{
    return tnc->selectModeConverse();
}

/*public static */tgs::TGSError artsatd::controlTNCPacket(ASDDeviceTNC& tnc, void const* info)
{
    return tnc->sendPacket(*static_cast<std::string const*>(info));
}

/*private virtual */int artsatd::usage(int argc, char const* argv[])
{
    return EXIT_SUCCESS;
}

/*private virtual */int artsatd::initialize(void)
{
    tgs::TGSError error;
    int result(EXIT_SUCCESS);
    
    if ((result = setWorkspace()) == EXIT_SUCCESS) {
        log(LOG_NOTICE, LOG_SEPARATOR);
        log(LOG_NOTICE, (std::string("artsatd ") + VERSION_STRING).c_str());
        log(LOG_NOTICE, " ");
        log(LOG_NOTICE, "Copyright (C) 2013 - 2014 HORIGUCHI Junshi.");
        log(LOG_NOTICE, "                           http://iridium.jp/");
        log(LOG_NOTICE, "                           zap00365@nifty.com");
        log(LOG_NOTICE, "Copyright (C) 2014 - 2014 Ron Hashimoto.");
        log(LOG_NOTICE, "                           http://h2so5.net/");
        log(LOG_NOTICE, "                           mail@h2so5.net");
        log(LOG_NOTICE, " ");
        log(LOG_NOTICE, "The MIT License (MIT)");
        log(LOG_NOTICE, " ");
        log(LOG_NOTICE, "Permission is hereby granted, free of charge, to any person obtaining a copy of this software and");
        log(LOG_NOTICE, "associated documentation files (the \"Software\"), to deal in the Software without restriction,");
        log(LOG_NOTICE, "including without limitation the rights to use, copy, modify, merge, publish, distribute,");
        log(LOG_NOTICE, "sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is");
        log(LOG_NOTICE, "furnished to do so, subject to the following conditions:");
        log(LOG_NOTICE, "The above copyright notice and this permission notice shall be included in all copies or");
        log(LOG_NOTICE, "substantial portions of the Software.");
        log(LOG_NOTICE, "THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING");
        log(LOG_NOTICE, "BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.");
        log(LOG_NOTICE, "IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,");
        log(LOG_NOTICE, "WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,");
        log(LOG_NOTICE, "OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.");
        log(LOG_NOTICE, LOG_SEPARATOR);
        if ((error = openConfig()) == tgs::TGSERROR_OK) {
            log(LOG_NOTICE, LOG_SEPARATOR);
            if ((error = openDatabase()) == tgs::TGSERROR_OK) {
                log(LOG_NOTICE, LOG_SEPARATOR);
                if ((error = openPlugin()) == tgs::TGSERROR_OK) {
                    log(LOG_NOTICE, LOG_SEPARATOR);
                    if ((error = openDevice()) == tgs::TGSERROR_OK) {
                        log(LOG_NOTICE, LOG_SEPARATOR);
                        //<<<
                        if ((error = _passFactory.setObserverPosition(_config.observerLatitude, _config.observerLongitude, _config.observerAltitude)) == tgs::TGSERROR_OK) {
                            ASDRotationSolver::RotatorSpec rotator = {
                                ROTATOR_MIN_AZIMUTH,
                                ROTATOR_MAX_AZIMUTH,
                                ROTATOR_DEG_PER_SEC
                            };
                            error = _rotationSolver.setRotaterSpec(rotator);
                        }
                        if (error != tgs::TGSERROR_OK) {
                            log(LOG_ERR, "can't initialize pass simulator [%s]", error.print().c_str());
                            return EX_UNAVAILABLE;
                        }
                        //>>>
                        if ((error = _state.orbit.setObserverPosition(_config.observerLatitude, _config.observerLongitude, _config.observerAltitude)) == tgs::TGSERROR_OK) {
                            resetSession(&_session);
                            resetControl(&_control);
                            resetCurrent(_control, &_current);
                            resetState(_control, &_state);
                            resetMonitor(&_monitor);
                            resetCommand(&_command);
                            if ((error = openNetwork()) == tgs::TGSERROR_OK) {
                                log(LOG_NOTICE, LOG_SEPARATOR);
                            }
                            else {
                                log(LOG_ERR, "can't initialize network [%s]", error.print().c_str());
                                result = EX_UNAVAILABLE;
                            }
                        }
                        else {
                            log(LOG_ERR, "can't initialize orbit simulator [%s]", error.print().c_str());
                            result = EX_UNAVAILABLE;
                        }
                    }
                    else {
                        log(LOG_ERR, "can't initialize device [%s]", error.print().c_str());
                        result = EX_UNAVAILABLE;
                    }
                }
                else {
                    log(LOG_ERR, "can't initialize plugin [%s]", error.print().c_str());
                    result = EX_UNAVAILABLE;
                }
            }
            else {
                log(LOG_ERR, "can't initialize database [%s]", error.print().c_str());
                result = EX_UNAVAILABLE;
            }
        }
        else {
            log(LOG_ERR, "can't initialize config [%s]", error.print().c_str());
            result = EX_UNAVAILABLE;
        }
    }
    else {
        log(LOG_ERR, "can't initialize workspace [%m]");
    }
    return result;
}

/*private virtual */void artsatd::terminate(void)
{
    closeNetwork();
    closeDevice();
    closePlugin();
    closeDatabase();
    closeConfig();
    return;
}

/*private virtual */void artsatd::loop(void)
{
    ControlRec control;
    CurrentRec current;
    MonitorRec monitor;
    bool expire;
    double azimuth;
    double elevation;
    double beacon;
    double sender;
    double receiver;
    
    getControl(&control);
    resetCurrent(control, &current);
    resetMonitor(&monitor);
    operateSession(current.time);
    if (control.manualRotator != _state.manualRotator) {
        resetRotator();
        _state.manualRotator = control.manualRotator;
    }
    if (control.manualTransceiver != _state.manualTransceiver) {
        switchTransceiver(MODETRANSCEIVER_LIMIT);
        resetTransceiver();
        _state.manualTransceiver = control.manualTransceiver;
    }
    if (control.manualTNC != _state.manualTNC) {
        switchTNC(MODETNC_LIMIT, "");
        resetTNC();
        _state.manualTNC = control.manualTNC;
    }
    expire = false;
    if (control.norad != _state.field.norad) {
        expire = true;
        _state.field.norad = control.norad;
    }
    if (_state.field.norad >= 0) {
        if (_database.hasUpdate(_state.field.norad)) {
            expire = true;
        }
        if (expire) {
            if ((monitor.error = _database.getField(_state.field.norad, &_state.field)) == tgs::TGSERROR_OK) {
                if (_state.field.beacon.drift == INT_MIN) {
                    _state.field.beacon.drift = 0;
                }
                if (_state.field.sender.drift == INT_MIN) {
                    _state.field.sender.drift = 0;
                }
                if (_state.field.receiver.drift == INT_MIN) {
                    _state.field.receiver.drift = 0;
                }
                if ((monitor.error = _state.orbit.setOrbitData(_state.field.tle)) == tgs::TGSERROR_OK) {
                    if ((monitor.error = _passFactory.setOrbitData(_state.field.tle)) == tgs::TGSERROR_OK) {
                        resetRotator();
                        resetTransceiver();
                        resetTNC();
                    }
                }
            }
            if (monitor.error != tgs::TGSERROR_OK) {
                _state.field.norad = -1;
            }
        }
    }
    if (control.mode != _state.mode) {
        resetTransceiver();
        resetTNC();
        _state.mode = control.mode;
    }
    switch (_state.mode) {
        case MODE_CW:
        case MODE_CW_TEST:
            if (!_state.manualTransceiver) {
                switchTransceiver(MODETRANSCEIVER_BEACON);
            }
            if (!_state.manualTNC) {
                switchTNC(MODETNC_COMMAND, "");
            }
            break;
        case MODE_FM:
        case MODE_FM_TEST:
            if (!_state.manualTransceiver) {
                switchTransceiver(MODETRANSCEIVER_COMMUNICATION);
            }
            if (!_state.manualTNC) {
                if (_state.field.norad < 0) {
                    switchTNC(MODETNC_CONVERSE, _config.observerCallsign);
                }
                else if (!_state.field.callsign.empty()) {
                    switchTNC(MODETNC_CONVERSE, _state.field.callsign);
                }
                else {
                    switchTNC(MODETNC_CONVERSE, "CQ");
                }
            }
            break;
        default:
            // nop
            break;
    }
    if (_state.field.norad >= 0) {
        azimuth = NAN;
        elevation = NAN;
        beacon = NAN;
        sender = NAN;
        receiver = NAN;
        if ((monitor.error = _state.orbit.setTargetTime(current.time + ir::IRXTimeDiff(_config.algorithmLookahead))) == tgs::TGSERROR_OK) {
            if ((monitor.error = _state.orbit.getSatellitePosition(&monitor.latitude, &monitor.longitude, &monitor.altitude)) == tgs::TGSERROR_OK) {
                if ((monitor.error = _state.orbit.getSatelliteDirection(&monitor.azimuth, &monitor.elevation)) == tgs::TGSERROR_OK) {
                    if ((monitor.error = _state.orbit.getDopplerRatio(&monitor.dopplerSender, &monitor.dopplerReceiver)) == tgs::TGSERROR_OK) {
                        if (_state.field.beacon.frequency >= 0) {
                            monitor.beacon = (_state.field.beacon.frequency + _state.field.beacon.drift) * monitor.dopplerReceiver;
                        }
                        if (_state.field.sender.frequency >= 0) {
                            monitor.sender = (_state.field.sender.frequency + _state.field.sender.drift) * monitor.dopplerSender;
                        }
                        if (_state.field.receiver.frequency >= 0) {
                            monitor.receiver = (_state.field.receiver.frequency + _state.field.receiver.drift) * monitor.dopplerReceiver;
                        }
                        switch (_state.mode) {
                            case MODE_CW_TEST:
                            case MODE_FM_TEST:
                                if (_state.field.beacon.frequency >= 0) {
                                    beacon = _state.field.beacon.frequency;
                                }
                                if (_state.field.sender.frequency >= 0) {
                                    sender = _state.field.sender.frequency;
                                }
                                if (_state.field.receiver.frequency >= 0) {
                                    receiver = _state.field.receiver.frequency;
                                }
                                break;
                            default:
                                beacon = monitor.beacon;
                                sender = monitor.sender;
                                receiver = monitor.receiver;
                                break;
                        }
                        if ((monitor.error = _passFactory.getNearestPass(&_pass, current.time, _rotationSolver)) == tgs::TGSERROR_OK) {
                            if ((monitor.error = _pass.getAOSTime(&monitor.aos)) == tgs::TGSERROR_OK) {
                                if ((monitor.error = _pass.getLOSTime(&monitor.los)) == tgs::TGSERROR_OK) {
                                    if ((monitor.error = _pass.getMEL(&monitor.mel)) == tgs::TGSERROR_OK) {
                                        if ((monitor.error = _pass.getRotationStartTime(&monitor.start)) == tgs::TGSERROR_OK) {
                                            switch (_state.mode) {
                                                case MODE_CW_TEST:
                                                    azimuth = _config.cwTestAzimuth;
                                                    elevation = _config.cwTestElevation;
                                                    break;
                                                case MODE_FM_TEST:
                                                    azimuth = _config.fmTestAzimuth;
                                                    elevation = _config.fmTestElevation;
                                                    break;
                                                default:
                                                    monitor.error = _pass.getRotatorDirection((monitor.aos <= current.time && current.time <= monitor.los - ir::IRXTimeDiff(_config.algorithmLookahead)) ? (current.time + ir::IRXTimeDiff(_config.algorithmLookahead)) : (monitor.aos), &azimuth, &elevation);
                                                    break;
                                            }
                                            if (monitor.error == tgs::TGSERROR_OK) {
                                                switch (_state.mode) {
                                                    case MODE_CW:
                                                    case MODE_CW_TEST:
                                                        if (!_state.manualRotator) {
                                                            operateRotator(current.time, azimuth, elevation);
                                                        }
                                                        if (!_state.manualTransceiver) {
                                                            operateTransceiver(current.time, NAN, beacon);
                                                        }
                                                        break;
                                                    case MODE_FM:
                                                    case MODE_FM_TEST:
                                                        if (!_state.manualRotator) {
                                                            operateRotator(current.time, azimuth, elevation);
                                                        }
                                                        if (!_state.manualTransceiver) {
                                                            operateTransceiver(current.time, sender, receiver);
                                                        }
                                                        if (!_state.manualTNC) {
                                                            boost::upgrade_lock<boost::shared_mutex> ulock(_mutex_command);
                                                            if (_command.queue.size() > 0) {
                                                                if (operateTNC(current.time, _command.queue.front()) == tgs::TGSERROR_OK) {
                                                                    boost::upgrade_to_unique_lock<boost::shared_mutex> wlock(ulock);
                                                                    _command.queue.pop_front();
                                                                }
                                                            }
                                                        }
                                                        break;
                                                    default:
                                                        boost::unique_lock<boost::shared_mutex> wlock(_mutex_command);
                                                        _command.queue.clear();
                                                        break;
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
        operateLog(current.time, _state.field.norad, _state.mode, azimuth, elevation, beacon, sender, receiver, monitor.error);
    }
    setCurrent(current);
    setMonitor(monitor);
    usleep(LOOP_INTERVAL);
    return;
}

/*private virtual */void artsatd::syslog(int priority, char const* format, va_list ap)
{
    static boost::mutex s_mutex;
    
    boost::unique_lock<boost::mutex> lock(s_mutex);
    super::syslog(priority, format, ap);
    return;
}

/*private virtual */void artsatd::onNotifyReceivePacket(std::string const& packet)
{
    bool handle;
    std::map<int, boost::shared_ptr<ASDPluginInterface> >::const_iterator it;
    
    log(LOG_NOTICE, "TNC: packet receive [%s]", packet.c_str());
    handle = false;
    boost::shared_lock<boost::shared_mutex> rlock(_mutex_current);
    if (_current.norad >= 0) {
        if ((it = _plugin.find(_current.norad)) != _plugin.end()) {
            handle = it->second->receive(packet);
        }
    }
    for (it = _plugin.begin(); it != _plugin.end() && !handle; ++it) {
        if (_current.norad < 0 || it->first != _current.norad) {
            handle = it->second->receive(packet);
        }
    }
    return;
}

/*private */void artsatd::getControl(ControlRec* control) const
{
    boost::shared_lock<boost::shared_mutex> rlock(_mutex_control);
    *control = _control;
    return;
}

/*private */void artsatd::setCurrent(CurrentRec const& current)
{
    boost::unique_lock<boost::shared_mutex> wlock(_mutex_current);
    _current = current;
    return;
}

/*private */void artsatd::setMonitor(MonitorRec const& monitor)
{
    boost::unique_lock<boost::shared_mutex> wlock(_mutex_monitor);
    _monitor = monitor;
    return;
}

/*private */void artsatd::cleanSession(ir::IRXTime const& time)
{
    std::map<std::string, ir::IRXTime>::const_iterator it;
    
    boost::upgrade_lock<boost::shared_mutex> ulock(_mutex_session);
    for (it = _session.id.begin(); it != _session.id.end(); ) {
        if (time >= it->second + ir::IRXTimeDiff(_config.sessionTimeout)) {
            boost::upgrade_to_unique_lock<boost::shared_mutex> wlock(ulock);
            if (_session.owner == it->first) {
                _session.owner.clear();
            }
            _session.id.erase(it++);
        }
        else {
            ++it;
        }
    }
    return;
}

/*private */tgs::TGSError artsatd::openConfig(void)
{
    tinyxml2::XMLDocument xml;
    tinyxml2::XMLElement const* root;
    tinyxml2::XMLElement const* type;
    tinyxml2::XMLElement const* element;
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    closeConfig();
    _config.serverDatabasePort = DEFAULT_SERVER_DATABASE_PORT;
    _config.serverDatabaseListen = DEFAULT_SERVER_DATABASE_LISTEN;
    _config.serverOperationPort = DEFAULT_SERVER_OPERATION_PORT;
    _config.serverOperationListen = DEFAULT_SERVER_OPERATION_LISTEN;
    _config.serverRPCPort = DEFAULT_SERVER_RPC_PORT;
    _config.serverRPCListen = DEFAULT_SERVER_RPC_LISTEN;
    _config.sessionMaximum = DEFAULT_SESSION_MAXIMUM;
    _config.sessionTimeout = DEFAULT_SESSION_TIMEOUT;
    _config.sessionLocalonly = DEFAULT_SESSION_LOCALONLY;
    _config.observerCallsign = DEFAULT_OBSERVER_CALLSIGN;
    _config.observerLatitude = DEFAULT_OBSERVER_LATITUDE;
    _config.observerLongitude = DEFAULT_OBSERVER_LONGITUDE;
    _config.observerAltitude = DEFAULT_OBSERVER_ALTITUDE;
    _config.cwTestAzimuth = DEFAULT_CW_TEST_AZIMUTH;
    _config.cwTestElevation = DEFAULT_CW_TEST_ELEVATION;
    _config.fmTestAzimuth = DEFAULT_FM_TEST_AZIMUTH;
    _config.fmTestElevation = DEFAULT_FM_TEST_ELEVATION;
    _config.algorithmLookahead = DEFAULT_ALGORITHM_LOOKAHEAD;
    _config.intervalSession = DEFAULT_INTERVAL_SESSION;
    _config.intervalRotator = DEFAULT_INTERVAL_ROTATOR;
    _config.intervalTransceiver = DEFAULT_INTERVAL_TRANSCEIVER;
    _config.intervalTNC = DEFAULT_INTERVAL_TNC;
    _config.intervalLog = DEFAULT_INTERVAL_LOG;
    switch (xml.LoadFile(XML_CONFIG)) {
        case tinyxml2::XML_NO_ERROR:
            if ((root = xml.FirstChildElement("config")) != NULL) {
                if ((type = root->FirstChildElement("server")) != NULL) {
                    if ((element = type->FirstChildElement("database")) != NULL) {
                        xmlReadText(element, "port", &_config.serverDatabasePort);
                        xmlReadInteger(element, "listen", &_config.serverDatabaseListen);
                    }
                    if ((element = type->FirstChildElement("operation")) != NULL) {
                        xmlReadText(element, "port", &_config.serverOperationPort);
                        xmlReadInteger(element, "listen", &_config.serverOperationListen);
                    }
                    if ((element = type->FirstChildElement("rpc")) != NULL) {
                        xmlReadText(element, "port", &_config.serverRPCPort);
                        xmlReadInteger(element, "listen", &_config.serverRPCListen);
                    }
                }
                if ((type = root->FirstChildElement("session")) != NULL) {
                    xmlReadInteger(type, "maximum", &_config.sessionMaximum);
                    xmlReadInteger(type, "timeout", &_config.sessionTimeout);
                    xmlReadBool(type, "localonly", &_config.sessionLocalonly);
                }
                if ((type = root->FirstChildElement("observer")) != NULL) {
                    xmlReadText(type, "callsign", &_config.observerCallsign);
                    xmlReadDouble(type, "latitude", &_config.observerLatitude);
                    xmlReadDouble(type, "longitude", &_config.observerLongitude);
                    xmlReadDouble(type, "altitude", &_config.observerAltitude);
                }
                if ((type = root->FirstChildElement("cw")) != NULL) {
                    if ((element = type->FirstChildElement("test")) != NULL) {
                        xmlReadInteger(element, "azimuth", &_config.cwTestAzimuth);
                        xmlReadInteger(element, "elevation", &_config.cwTestElevation);
                    }
                }
                if ((type = root->FirstChildElement("fm")) != NULL) {
                    if ((element = type->FirstChildElement("test")) != NULL) {
                        xmlReadInteger(element, "azimuth", &_config.fmTestAzimuth);
                        xmlReadInteger(element, "elevation", &_config.fmTestElevation);
                    }
                }
                if ((type = root->FirstChildElement("algorithm")) != NULL) {
                    xmlReadInteger(type, "lookahead", &_config.algorithmLookahead);
                }
                if ((type = root->FirstChildElement("interval")) != NULL) {
                    xmlReadInteger(type, "session", &_config.intervalSession);
                    xmlReadInteger(type, "rotator", &_config.intervalRotator);
                    xmlReadInteger(type, "transceiver", &_config.intervalTransceiver);
                    xmlReadInteger(type, "tnc", &_config.intervalTNC);
                    xmlReadInteger(type, "log", &_config.intervalLog);
                }
            }
            break;
        case tinyxml2::XML_ERROR_FILE_NOT_FOUND:
            // nop
            break;
        default:
            error = tgs::TGSERROR_FAILED;
            break;
    }
    log(LOG_NOTICE, " CONFIG: Server Database Port    [%s]", _config.serverDatabasePort.c_str());
    log(LOG_NOTICE, " CONFIG: Server Database Listen  [%d]", _config.serverDatabaseListen);
    log(LOG_NOTICE, " CONFIG: Server Operation Port   [%s]", _config.serverOperationPort.c_str());
    log(LOG_NOTICE, " CONFIG: Server Operation Listen [%d]", _config.serverOperationListen);
    log(LOG_NOTICE, " CONFIG: Server RPC Port         [%s]", _config.serverRPCPort.c_str());
    log(LOG_NOTICE, " CONFIG: Server RPC Listen       [%d]", _config.serverRPCListen);
    log(LOG_NOTICE, " CONFIG: Session Maximum         [%d]", _config.sessionMaximum);
    log(LOG_NOTICE, " CONFIG: Session Timeout         [%d]", _config.sessionTimeout);
    log(LOG_NOTICE, " CONFIG: Session Localonly       [%s]", (_config.sessionLocalonly) ? ("true") : ("false"));
    log(LOG_NOTICE, " CONFIG: Observer Callsign       [%s]", _config.observerCallsign.c_str());
    log(LOG_NOTICE, " CONFIG: Observer Latitude       [%lf]", _config.observerLatitude);
    log(LOG_NOTICE, " CONFIG: Observer Longitude      [%lf]", _config.observerLongitude);
    log(LOG_NOTICE, " CONFIG: Observer Altitude       [%lf]", _config.observerAltitude);
    log(LOG_NOTICE, " CONFIG: CW Test Azimuth         [%d]", _config.cwTestAzimuth);
    log(LOG_NOTICE, " CONFIG: CW Test Elevation       [%d]", _config.cwTestElevation);
    log(LOG_NOTICE, " CONFIG: FM Test Azimuth         [%d]", _config.fmTestAzimuth);
    log(LOG_NOTICE, " CONFIG: FM Test Elevation       [%d]", _config.fmTestElevation);
    log(LOG_NOTICE, " CONFIG: Algorithm Lookahead     [%d]", _config.algorithmLookahead);
    log(LOG_NOTICE, " CONFIG: Interval Session        [%d]", _config.intervalSession);
    log(LOG_NOTICE, " CONFIG: Interval Rotator        [%d]", _config.intervalRotator);
    log(LOG_NOTICE, " CONFIG: Interval Transceiver    [%d]", _config.intervalTransceiver);
    log(LOG_NOTICE, " CONFIG: Interval TNC            [%d]", _config.intervalTNC);
    log(LOG_NOTICE, " CONFIG: Interval Log            [%d]", _config.intervalLog);
    if (error != tgs::TGSERROR_OK) {
        closeConfig();
    }
    return error;
}

/*private */tgs::TGSError artsatd::openDatabase(void)
{
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    closeDatabase();
    error = _database.open(DATABASE_PHYSICS);
    if (error != tgs::TGSERROR_OK) {
        closeDatabase();
    }
    return error;
}

/*private */tgs::TGSError artsatd::openPlugin(void)
{
    tinyxml2::XMLDocument xml;
    tinyxml2::XMLElement const* root;
    tinyxml2::XMLElement const* type;
    std::string name;
    boost::shared_ptr<ASDPluginInterface> plugin;
    std::string file;
    int norad;
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    closePlugin();
    switch (xml.LoadFile(XML_PLUGIN)) {
        case tinyxml2::XML_NO_ERROR:
            if ((root = xml.FirstChildElement("plugin")) != NULL) {
                for (type = root->FirstChildElement(); type != NULL; type = type->NextSiblingElement()) {
                    name = type->Name();
                    if (name == "ASDPluginINVADER") {
                        plugin.reset(new(std::nothrow) ASDPluginINVADER);
                    }
                    else {
                        name.clear();
                    }
                    if (!name.empty()) {
                        if (plugin != NULL) {
                            if ((error = xmlReadInteger(type, "norad", &norad)) == tgs::TGSERROR_OK) {
                                xmlReadText(type, "file", &file);
                                log(LOG_NOTICE, " PLUGIN: Type                    [%s]", name.c_str());
                                log(LOG_NOTICE, " PLUGIN: File                    [%s]", file.c_str());
                                log(LOG_NOTICE, " PLUGIN: NORAD                   [%d]", norad);
                                if (!file.empty()) {
                                    name  = PATH_SERVER;
                                    name += "/";
                                    name += PATH_PLUGIN;
                                    name += "/";
                                    file = name + file;
                                }
                                if ((error = plugin->open(file)) == tgs::TGSERROR_OK) {
                                    _plugin[norad] = plugin;
                                }
                            }
                        }
                        else {
                            error = tgs::TGSERROR_NO_MEMORY;
                        }
                    }
                }
            }
            break;
        case tinyxml2::XML_ERROR_FILE_NOT_FOUND:
            // nop
            break;
        default:
            error = tgs::TGSERROR_FAILED;
            break;
    }
    if (error != tgs::TGSERROR_OK) {
        closePlugin();
    }
    return error;
}

/*private */tgs::TGSError artsatd::openDevice(void)
{
    boost::shared_ptr<tgs::TGSRotatorGS232B> rotator;
    boost::shared_ptr<tgs::TGSTransceiverIC9100> transceiver;
    boost::shared_ptr<tgs::TGSTNCTNC555> tnc;
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    closeDevice();
    rotator.reset(new(std::nothrow) tgs::TGSRotatorGS232B);
    if (rotator != NULL) {
        transceiver.reset(new(std::nothrow) tgs::TGSTransceiverIC9100);
        if (transceiver != NULL) {
            if ((error = transceiver->connect(&_civ)) == tgs::TGSERROR_OK) {
                tnc.reset(new(std::nothrow) tgs::TGSTNCTNC555);
                if (tnc != NULL) {
                    tnc->setNotifier(this);
                    if ((error = _loader.append(rotator.get(), "rotator")) == tgs::TGSERROR_OK) {
                        if ((error = _loader.append(&_civ, "civ")) == tgs::TGSERROR_OK) {
                            if ((error = _loader.append(transceiver.get(), "transceiver")) == tgs::TGSERROR_OK) {
                                if ((error = _loader.append(tnc.get(), "tnc")) == tgs::TGSERROR_OK) {
                                    if ((error = _loader.open(XML_DEVICE)) == tgs::TGSERROR_OK) {
                                        if ((error = _rotator.open(rotator)) == tgs::TGSERROR_OK) {
                                            if ((error = _transceiver.open(transceiver)) == tgs::TGSERROR_OK) {
                                                error = _tnc.open(tnc);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                else {
                    error = tgs::TGSERROR_NO_MEMORY;
                }
            }
        }
        else {
            error = tgs::TGSERROR_NO_MEMORY;
        }
    }
    else {
        error = tgs::TGSERROR_NO_MEMORY;
    }
    if (error != tgs::TGSERROR_OK) {
        closeDevice();
    }
    return error;
}

/*private */tgs::TGSError artsatd::openNetwork(void)
{
    tinyxml2::XMLDocument xml;
    tinyxml2::XMLElement const* root;
    tinyxml2::XMLElement const* type;
    tinyxml2::XMLElement const* element;
    std::string name;
    boost::shared_ptr<ASDTLEClientInterface> client;
    std::vector<std::string> url;
    int interval;
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    closeNetwork();
    switch (xml.LoadFile(XML_NETWORK)) {
        case tinyxml2::XML_NO_ERROR:
            if ((root = xml.FirstChildElement("network")) != NULL) {
                for (type = root->FirstChildElement(); type != NULL; type = type->NextSiblingElement()) {
                    name = type->Name();
                    if (name == "ASDTLEClientCelestrak") {
                        client.reset(new(std::nothrow) ASDTLEClientCelestrak);
                    }
                    else {
                        name.clear();
                    }
                    if (!name.empty()) {
                        if (client != NULL) {
                            if ((error = xmlReadInteger(type, "interval", &interval)) == tgs::TGSERROR_OK) {
                                log(LOG_NOTICE, "NETWORK: Site                    [%s]", name.c_str());
                                url.clear();
                                for (element = type->FirstChildElement("url"); element != NULL; element = element->NextSiblingElement("url")) {
                                    if (!element->NoChildren()) {
                                        url.push_back(element->GetText());
                                        log(LOG_NOTICE, "NETWORK: URL                     [%s]", url.back().c_str());
                                    }
                                }
                                log(LOG_NOTICE, "NETWORK: Interval                [%d]", interval);
                                if ((error = client->open(DATABASE_PHYSICS, url, interval)) == tgs::TGSERROR_OK) {
                                    _clientTle.push_back(client);
                                }
                            }
                        }
                        else {
                            error = tgs::TGSERROR_NO_MEMORY;
                        }
                    }
                }
            }
            break;
        case tinyxml2::XML_ERROR_FILE_NOT_FOUND:
            // nop
            break;
        default:
            error = tgs::TGSERROR_FAILED;
            break;
    }
    if (error == tgs::TGSERROR_OK) {
        if ((error = _replierOperation.open(PATH_SERVER, DATABASE_PHYSICS)) == tgs::TGSERROR_OK) {
            if ((error = _replierRPC.open(DATABASE_PHYSICS)) == tgs::TGSERROR_OK) {
                _serverDatabase.setNotifier(&_replierDatabase);
                if ((error = _serverDatabase.open(_config.serverDatabasePort, _config.serverDatabaseListen)) == tgs::TGSERROR_OK) {
                    _serverOperation.setNotifier(&_replierOperation);
                    if ((error = _serverOperation.open(_config.serverOperationPort, _config.serverOperationListen)) == tgs::TGSERROR_OK) {
                        _serverRPC.setNotifier(&_replierRPC);
                        error = _serverRPC.open(_config.serverRPCPort, _config.serverRPCListen);
                    }
                }
            }
        }
    }
    if (error != tgs::TGSERROR_OK) {
        closeNetwork();
    }
    return error;
}

/*private */void artsatd::closeConfig(void)
{
    return;
}

/*private */void artsatd::closeDatabase(void)
{
    _database.close();
    return;
}

/*private */void artsatd::closePlugin(void)
{
    _plugin.clear();
    return;
}

/*private */void artsatd::closeDevice(void)
{
    _tnc.close();
    _transceiver.close();
    _civ.close();
    _rotator.close();
    _loader.clear();
    return;
}

/*private */void artsatd::closeNetwork(void)
{
    _serverRPC.close();
    _serverOperation.close();
    _serverDatabase.close();
    _replierRPC.close();
    _replierOperation.close();
    _clientTle.clear();
    return;
}

/*private */void artsatd::switchTransceiver(ModeTransceiverEnum mode)
{
    tgs::TGSError error;
    
    if (mode != _state.modeTransceiver) {
        error = tgs::TGSERROR_OK;
        if (_transceiver.isValid()) {
            switch (mode) {
                case MODETRANSCEIVER_BEACON:
                    error = _transceiver->selectModeBeacon();
                    break;
                case MODETRANSCEIVER_COMMUNICATION:
                    error = _transceiver->selectModeCommunication();
                    break;
                default:
                    // nop
                    break;
            }
            if (error != tgs::TGSERROR_OK) {
                log(LOG_WARNING, "can't switch transceiver mode [%s]", error.print().c_str());
            }
        }
        if (error == tgs::TGSERROR_OK) {
            _state.modeTransceiver = mode;
        }
    }
    return;
}

/*private */void artsatd::switchTNC(ModeTNCEnum mode, std::string const& callsign)
{
    tgs::TGSError error;
    
    if (mode != _state.modeTNC || callsign != _state.callsign) {
        error = tgs::TGSERROR_OK;
        if (_tnc.isValid()) {
            switch (mode) {
                case MODETNC_COMMAND:
                    error = _tnc->selectModeCommand();
                    break;
                case MODETNC_CONVERSE:
                    if ((error = _tnc->selectModeCommand()) == tgs::TGSERROR_OK) {
                        if ((error = _tnc->setPacketMode(_config.observerCallsign, callsign)) == tgs::TGSERROR_OK) {
                            error = _tnc->selectModeConverse();
                        }
                    }
                    break;
                default:
                    // nop
                    break;
            }
            if (error != tgs::TGSERROR_OK) {
                log(LOG_WARNING, "can't switch tnc mode [%s]", error.print().c_str());
            }
        }
        if (error == tgs::TGSERROR_OK) {
            _state.modeTNC = mode;
            _state.callsign = callsign;
        }
    }
    return;
}

/*private */void artsatd::operateSession(ir::IRXTime const& time)
{
    if (time >= _state.timeSession + ir::IRXTimeDiff(_config.intervalSession)) {
        cleanSession(time);
        _state.timeSession = time;
    }
    return;
}

/*private */tgs::TGSError artsatd::operateRotator(ir::IRXTime const& time, double azimuth, double elevation)
{
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    if (time >= _state.timeRotator + ir::IRXTimeDiff(_config.intervalRotator)) {
        if (!std::isnan(azimuth) && !std::isnan(elevation)) {
            if (std::round(azimuth) != std::round(_state.azimuth) || std::round(elevation) != std::round(_state.elevation)) {
                if (_rotator.isValid()) {
                    if ((error = _rotator->rotateTo(std::round(azimuth), std::round(elevation))) != tgs::TGSERROR_OK) {
                        log(LOG_WARNING, "can't operate rotator angle [%s]", error.print().c_str());
                    }
                }
                if (error == tgs::TGSERROR_OK) {
                    _state.azimuth = azimuth;
                    _state.elevation = elevation;
                }
            }
        }
        if (error == tgs::TGSERROR_OK) {
            _state.timeRotator = time;
        }
    }
    else {
        error = tgs::TGSERROR_WAIT_RESULT;
    }
    return error;
}

/*private */tgs::TGSError artsatd::operateTransceiver(ir::IRXTime const& time, double sender, double receiver)
{
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    if (time >= _state.timeTransceiver + ir::IRXTimeDiff(_config.intervalTransceiver)) {
        if (!std::isnan(sender)) {
            if (std::round(sender) != std::round(_state.sender)) {
                if (_transceiver.isValid()) {
                    if ((error = _transceiver->setFrequencySender(std::round(sender))) != tgs::TGSERROR_OK) {
                        log(LOG_WARNING, "can't operate transceiver sender frequency [%s]", error.print().c_str());
                    }
                }
                if (error == tgs::TGSERROR_OK) {
                    _state.sender = sender;
                }
            }
        }
        if (error == tgs::TGSERROR_OK) {
            if (!std::isnan(receiver)) {
                if (std::round(receiver) != std::round(_state.receiver)) {
                    if (_transceiver.isValid()) {
                        if ((error = _transceiver->setFrequencyReceiver(std::round(receiver))) != tgs::TGSERROR_OK) {
                            log(LOG_WARNING, "can't operate transceiver receiver frequency [%s]", error.print().c_str());
                        }
                    }
                    if (error == tgs::TGSERROR_OK) {
                        _state.receiver = receiver;
                    }
                }
            }
        }
        if (error == tgs::TGSERROR_OK) {
            _state.timeTransceiver = time;
        }
    }
    else {
        error = tgs::TGSERROR_WAIT_RESULT;
    }
    return error;
}

/*private */tgs::TGSError artsatd::operateTNC(ir::IRXTime const& time, std::string const& command)
{
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    if (time >= _state.timeTNC + ir::IRXTimeDiff(_config.intervalTNC)) {
        if (!command.empty()) {
            log(LOG_NOTICE, "TNC: packet send [%s]", command.c_str());
            if (_tnc.isValid()) {
                if ((error = _tnc->sendPacket(command + " ")) != tgs::TGSERROR_OK) {
                    log(LOG_WARNING, "can't operate tnc packet [%s]", error.print().c_str());
                }
            }
        }
        if (error == tgs::TGSERROR_OK) {
            _state.timeTNC = time;
        }
    }
    else {
        error = tgs::TGSERROR_WAIT_RESULT;
    }
    return error;
}

/*private */void artsatd::operateLog(ir::IRXTime const& time, int norad, ModeEnum mode, double azimuth, double elevation, double beacon, double sender, double receiver, tgs::TGSError info)
{
    std::string string;
    
    if (time >= _state.timeLog + ir::IRXTimeDiff(_config.intervalLog)) {
        if (!_rotator.isValid()) {
            log(LOG_NOTICE, "rotator is offline");
        }
        if (!_transceiver.isValid()) {
            log(LOG_NOTICE, "transceiver is offline");
        }
        if (!_tnc.isValid()) {
            log(LOG_NOTICE, "tnc is offline");
        }
        switch (mode) {
            case MODE_CW:
                string = "CW";
                break;
            case MODE_CW_TEST:
                string = "CW_TEST";
                break;
            case MODE_FM:
                string = "FM";
                break;
            case MODE_FM_TEST:
                string = "FM_TEST";
                break;
            default:
                string = "NOOP";
                break;
        }
        log(LOG_INFO, "NORAD: %05d, Mode: %-7s [A %7.3lf, E %+7.3lf, B %10.6lf, S %10.6lf, R %10.6lf], Error: %s", norad, string.c_str(), azimuth, elevation, beacon / 1000000.0, sender / 1000000.0, receiver / 1000000.0, info.print().c_str());
        _state.timeLog = time;
    }
    return;
}

/*private */void artsatd::resetRotator(void)
{
    _state.timeRotator.set(0);
    _state.azimuth = NAN;
    _state.elevation = NAN;
    return;
}

/*private */void artsatd::resetTransceiver(void)
{
    _state.timeTransceiver.set(0);
    _state.sender = NAN;
    _state.receiver = NAN;
    return;
}

/*private */void artsatd::resetTNC(void)
{
    _state.timeTNC.set(0);
    return;
}

/*private static */void artsatd::resetSession(SessionRec* session)
{
    session->id.clear();
    session->owner.clear();
    session->random.seed(ir::IRXTime::currentUTCTime().asTime_t());
    return;
}

/*private static */void artsatd::resetControl(ControlRec* control)
{
    control->manualRotator = false;
    control->manualTransceiver = false;
    control->manualTNC = false;
    control->norad = -1;
    control->mode = MODE_LIMIT;
    return;
}

/*private static */void artsatd::resetCurrent(ControlRec const& control, CurrentRec* current)
{
    current->norad = control.norad;
    current->time = ir::IRXTime::currentTime();
    return;
}

/*private static */void artsatd::resetState(ControlRec const& control, StateRec* state)
{
    state->manualRotator = control.manualRotator;
    state->manualTransceiver = control.manualTransceiver;
    state->manualTNC = control.manualTNC;
    state->field.norad = control.norad;
    state->mode = control.mode;
    state->modeTransceiver = MODETRANSCEIVER_LIMIT;
    state->modeTNC = MODETNC_LIMIT;
    state->callsign.clear();
    state->timeSession.set(0);
    state->timeRotator.set(0);
    state->azimuth = NAN;
    state->elevation = NAN;
    state->timeTransceiver.set(0);
    state->sender = NAN;
    state->receiver = NAN;
    state->timeTNC.set(0);
    state->timeLog.set(0);
    return;
}

/*private static */void artsatd::resetMonitor(MonitorRec* monitor)
{
    monitor->latitude = NAN;
    monitor->longitude = NAN;
    monitor->altitude = NAN;
    monitor->azimuth = NAN;
    monitor->elevation = NAN;
    monitor->beacon = NAN;
    monitor->sender = NAN;
    monitor->receiver = NAN;
    monitor->dopplerSender = NAN;
    monitor->dopplerReceiver = NAN;
    //<<<
    monitor->aos.set(0);
    monitor->los.set(0);
    monitor->mel = NAN;
    monitor->start.set(0);
    //>>>
    monitor->error = tgs::TGSERROR_OK;
    return;
}

/*private static */void artsatd::resetCommand(CommandRec* command)
{
    command->queue.clear();
    return;
}

/*private static */int artsatd::setWorkspace(void)
{
    std::string path;
    int result(EXIT_SUCCESS);
    
    path  = PATH_WORKSPACE;
    path += "/";
    path += getprogname();
    if (mkdir(path.c_str(), 0755) != 0) {
        if (errno != EEXIST) {
            result = EX_OSFILE;
        }
    }
    if (result == EXIT_SUCCESS) {
        if (chdir(path.c_str()) != 0) {
            result = EX_OSFILE;
        }
    }
    return result;
}

/*private static */tgs::TGSError artsatd::xmlReadText(tinyxml2::XMLElement const* parent, std::string const& tag, std::string* result)
{
    tinyxml2::XMLElement const* element;
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    if ((element = parent->FirstChildElement(tag.c_str())) != NULL) {
        if (!element->NoChildren()) {
            *result = element->GetText();
        }
        else {
            error = tgs::TGSERROR_NO_RESULT;
        }
    }
    else {
        error = tgs::TGSERROR_NO_RESULT;
    }
    return error;
}

/*private static */tgs::TGSError artsatd::xmlReadBool(tinyxml2::XMLElement const* parent, std::string const& tag, bool* result)
{
    tinyxml2::XMLElement const* element;
    bool value;
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    if ((element = parent->FirstChildElement(tag.c_str())) != NULL) {
        if (!element->NoChildren()) {
            if (element->QueryBoolText(&value) == tinyxml2::XML_NO_ERROR) {
                *result = value;
            }
            else {
                error = tgs::TGSERROR_INVALID_FORMAT;
            }
        }
        else {
            error = tgs::TGSERROR_NO_RESULT;
        }
    }
    else {
        error = tgs::TGSERROR_NO_RESULT;
    }
    return error;
}

/*private static */tgs::TGSError artsatd::xmlReadInteger(tinyxml2::XMLElement const* parent, std::string const& tag, int* result)
{
    tinyxml2::XMLElement const* element;
    int value;
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    if ((element = parent->FirstChildElement(tag.c_str())) != NULL) {
        if (!element->NoChildren()) {
            if (element->QueryIntText(&value) == tinyxml2::XML_NO_ERROR) {
                *result = value;
            }
            else {
                error = tgs::TGSERROR_INVALID_FORMAT;
            }
        }
        else {
            error = tgs::TGSERROR_NO_RESULT;
        }
    }
    else {
        error = tgs::TGSERROR_NO_RESULT;
    }
    return error;
}

/*private static */tgs::TGSError artsatd::xmlReadDouble(tinyxml2::XMLElement const* parent, std::string const& tag, double* result)
{
    tinyxml2::XMLElement const* element;
    double value;
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    if ((element = parent->FirstChildElement(tag.c_str())) != NULL) {
        if (!element->NoChildren()) {
            if (element->QueryDoubleText(&value) == tinyxml2::XML_NO_ERROR) {
                *result = value;
            }
            else {
                error = tgs::TGSERROR_INVALID_FORMAT;
            }
        }
        else {
            error = tgs::TGSERROR_NO_RESULT;
        }
    }
    else {
        error = tgs::TGSERROR_NO_RESULT;
    }
    return error;
}
