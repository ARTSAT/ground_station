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
**      ASDServerRPCMethod.cpp
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

#include "ASDServerRPCMethod.h"
#include "artsatd.h"

namespace ASDServerRPC {

static  tgs::TGSError           updateSession               (Param const& param, std::string* session, Param* result);
template <typename T>
static  tgs::TGSError           getParam                    (Param const& param, std::string const& key, T* result);
template <>
        tgs::TGSError           getParam                    (Param const& param, std::string const& key, int* result);
template <typename T>
static  void                    setResult                   (T const& param, std::string const& key, Param* result);
static  tgs::TGSError           setError                    (tgs::TGSError error, Param* result);
static  tgs::TGSError           controlRotatorAzimuth       (ASDDeviceRotator& rotator, void const* info);
static  tgs::TGSError           controlRotatorElevation     (ASDDeviceRotator& rotator, void const* info);
static  tgs::TGSError           controlTransceiverModeCW    (ASDDeviceTransceiver& transceiver, void const* info);
static  tgs::TGSError           controlTransceiverModeFM    (ASDDeviceTransceiver& transceiver, void const* info);
static  tgs::TGSError           controlTransceiverSender    (ASDDeviceTransceiver& transceiver, void const* info);
static  tgs::TGSError           controlTransceiverReceiver  (ASDDeviceTransceiver& transceiver, void const* info);
static  tgs::TGSError           controlTNCModeCommand       (ASDDeviceTNC& tnc, void const* info);
static  tgs::TGSError           controlTNCModeConverse      (ASDDeviceTNC& tnc, void const* info);
static  tgs::TGSError           controlTNCPacket            (ASDDeviceTNC& tnc, void const* info);

namespace system {

tgs::TGSError rpcEcho(Param const& param, Param* result)
{
    *result = param;
    return tgs::TGSERROR_OK;
}

}// end of namespace
namespace observer {

tgs::TGSError getVersion(Param const& param, Param* result)
{
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    if ((error = updateSession(param, NULL, result)) == tgs::TGSERROR_OK) {
        setResult(artsatd::getInstance().getVersion(), "version", result);
    }
    return error;
}

tgs::TGSError getSession(Param const& param, Param* result)
{
    std::string session;
    int owner;
    bool exclusive;
    std::string host;
    int online;
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    if ((error = updateSession(param, &session, result)) == tgs::TGSERROR_OK) {
        artsatd::getInstance().getSession(session, &owner, &exclusive, &host, &online);
        setResult(owner, "owner", result);
        setResult(exclusive, "exclusive", result);
        setResult(host, "host", result);
        setResult(online, "online", result);
    }
    return error;
}

tgs::TGSError setManualRotator(Param const& param, Param* result)
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

tgs::TGSError getManualRotator(Param const& param, Param* result)
{
    std::string session;
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    if ((error = updateSession(param, &session, result)) == tgs::TGSERROR_OK) {
        setResult(artsatd::getInstance().getManualRotator(), "manual", result);
    }
    return error;
}

tgs::TGSError setManualTransceiver(Param const& param, Param* result)
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

tgs::TGSError getManualTransceiver(Param const& param, Param* result)
{
    std::string session;
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    if ((error = updateSession(param, &session, result)) == tgs::TGSERROR_OK) {
        setResult(artsatd::getInstance().getManualTransceiver(), "manual", result);
    }
    return error;
}

tgs::TGSError setManualTNC(Param const& param, Param* result)
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

tgs::TGSError getManualTNC(Param const& param, Param* result)
{
    std::string session;
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    if ((error = updateSession(param, &session, result)) == tgs::TGSERROR_OK) {
        setResult(artsatd::getInstance().getManualTNC(), "manual", result);
    }
    return error;
}

tgs::TGSError setNORAD(Param const& param, Param* result)
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

tgs::TGSError getNORAD(Param const& param, Param* result)
{
    std::string session;
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    if ((error = updateSession(param, &session, result)) == tgs::TGSERROR_OK) {
        setResult(artsatd::getInstance().getNORAD(), "norad", result);
    }
    return error;
}

tgs::TGSError setMode(Param const& param, Param* result)
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

tgs::TGSError getMode(Param const& param, Param* result)
{
    std::string session;
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    if ((error = updateSession(param, &session, result)) == tgs::TGSERROR_OK) {
        setResult(artsatd::getInstance().getMode(), "mode", result);
    }
    return error;
}

tgs::TGSError getTime(Param const& param, Param* result)
{
    std::string session;
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    if ((error = updateSession(param, &session, result)) == tgs::TGSERROR_OK) {
        setResult((artsatd::getInstance().getTime() - ir::IRXTimeDiff::localTimeOffset()).format("%YYYY/%MM/%DD %hh:%mm:%ss UTC"), "time", result);
    }
    return error;
}

tgs::TGSError getObserverCallsign(Param const& param, Param* result)
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

tgs::TGSError getObserverPosition(Param const& param, Param* result)
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

tgs::TGSError getObserverDirection(Param const& param, Param* result)
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

tgs::TGSError getObserverFrequency(Param const& param, Param* result)
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

tgs::TGSError getSatellitePosition(Param const& param, Param* result)
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

tgs::TGSError getSatelliteDirection(Param const& param, Param* result)
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

tgs::TGSError getSatelliteFrequency(Param const& param, Param* result)
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

tgs::TGSError getSatelliteDopplerShift(Param const& param, Param* result)
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

tgs::TGSError getSatelliteAOSLOS(Param const& param, Param* result)
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

tgs::TGSError getSatelliteMEL(Param const& param, Param* result)
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

tgs::TGSError getRotatorStart(Param const& param, Param* result)
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

tgs::TGSError getError(Param const& param, Param* result)
{
    std::string session;
    tgs::TGSError state;
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    if ((error = updateSession(param, &session, result)) == tgs::TGSERROR_OK) {
        state = artsatd::getInstance().getError();
        setResult(state, "code", result);
        setResult(state.print(), "message", result);
    }
    return error;
}

tgs::TGSError isValidRotator(Param const& param, Param* result)
{
    std::string session;
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    if ((error = updateSession(param, &session, result)) == tgs::TGSERROR_OK) {
        setResult(artsatd::getRotator().isValid(), "valid", result);
    }
    return error;
}

tgs::TGSError isValidTransceiver(Param const& param, Param* result)
{
    std::string session;
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    if ((error = updateSession(param, &session, result)) == tgs::TGSERROR_OK) {
        setResult(artsatd::getTransceiver().isValid(), "valid", result);
    }
    return error;
}

tgs::TGSError isValidTNC(Param const& param, Param* result)
{
    std::string session;
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    if ((error = updateSession(param, &session, result)) == tgs::TGSERROR_OK) {
        setResult(artsatd::getTNC().isValid(), "valid", result);
    }
    return error;
}

tgs::TGSError controlSession(Param const& param, Param* result)
{
    std::string session;
    bool owner;
    std::string host;
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    if ((error = updateSession(param, &session, result)) == tgs::TGSERROR_OK) {
        if ((error = getParam(param, "owner", &owner)) == tgs::TGSERROR_OK) {
            if ((error = getParam(param, "host", &host)) == tgs::TGSERROR_OK) {
                if ((error = artsatd::getInstance().controlSession(session, owner, host)) != tgs::TGSERROR_OK) {
                    error = setError(error, result);
                }
            }
        }
    }
    return error;
}

tgs::TGSError excludeSession(Param const& param, Param* result)
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

tgs::TGSError setRotatorAzimuth(Param const& param, Param* result)
{
    std::string session;
    int azimuth;
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    if ((error = updateSession(param, &session, result)) == tgs::TGSERROR_OK) {
        if ((error = getParam(param, "azimuth", &azimuth)) == tgs::TGSERROR_OK) {
            if ((error = artsatd::getInstance().controlManualRotator(session, &controlRotatorAzimuth, &azimuth)) != tgs::TGSERROR_OK) {
                error = setError(error, result);
            }
        }
    }
    return error;
}

tgs::TGSError setRotatorElevation(Param const& param, Param* result)
{
    std::string session;
    int elevation;
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    if ((error = updateSession(param, &session, result)) == tgs::TGSERROR_OK) {
        if ((error = getParam(param, "elevation", &elevation)) == tgs::TGSERROR_OK) {
            if ((error = artsatd::getInstance().controlManualRotator(session, &controlRotatorElevation, &elevation)) != tgs::TGSERROR_OK) {
                error = setError(error, result);
            }
        }
    }
    return error;
}

tgs::TGSError setTransceiverMode(Param const& param, Param* result)
{
    std::string session;
    std::string mode;
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    if ((error = updateSession(param, &session, result)) == tgs::TGSERROR_OK) {
        if ((error = getParam(param, "mode", &mode)) == tgs::TGSERROR_OK) {
            if (mode == "CW") {
                error = artsatd::getInstance().controlManualTransceiver(session, &controlTransceiverModeCW, NULL);
            }
            else if (mode == "FM") {
                error = artsatd::getInstance().controlManualTransceiver(session, &controlTransceiverModeFM, NULL);
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

tgs::TGSError setTransceiverSender(Param const& param, Param* result)
{
    std::string session;
    int sender;
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    if ((error = updateSession(param, &session, result)) == tgs::TGSERROR_OK) {
        if ((error = getParam(param, "sender", &sender)) == tgs::TGSERROR_OK) {
            if ((error = artsatd::getInstance().controlManualTransceiver(session, &controlTransceiverSender, &sender)) != tgs::TGSERROR_OK) {
                error = setError(error, result);
            }
        }
    }
    return error;
}

tgs::TGSError setTransceiverReceiver(Param const& param, Param* result)
{
    std::string session;
    int receiver;
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    if ((error = updateSession(param, &session, result)) == tgs::TGSERROR_OK) {
        if ((error = getParam(param, "receiver", &receiver)) == tgs::TGSERROR_OK) {
            if ((error = artsatd::getInstance().controlManualTransceiver(session, &controlTransceiverReceiver, &receiver)) != tgs::TGSERROR_OK) {
                error = setError(error, result);
            }
        }
    }
    return error;
}

tgs::TGSError setTNCMode(Param const& param, Param* result)
{
    std::string session;
    std::string mode;
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    if ((error = updateSession(param, &session, result)) == tgs::TGSERROR_OK) {
        if ((error = getParam(param, "mode", &mode)) == tgs::TGSERROR_OK) {
            if (mode == "Command") {
                error = artsatd::getInstance().controlManualTNC(session, &controlTNCModeCommand, NULL);
            }
            else if (mode == "Converse") {
                error = artsatd::getInstance().controlManualTNC(session, &controlTNCModeConverse, NULL);
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

tgs::TGSError sendTNCPacket(Param const& param, Param* result)
{
    std::string session;
    std::string packet;
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    if ((error = updateSession(param, &session, result)) == tgs::TGSERROR_OK) {
        if ((error = getParam(param, "packet", &packet)) == tgs::TGSERROR_OK) {
            if ((error = artsatd::getInstance().controlManualTNC(session, &controlTNCPacket, &packet)) != tgs::TGSERROR_OK) {
                error = setError(error, result);
            }
        }
    }
    return error;
}

tgs::TGSError requestCommand(Param const& param, Param* result)
{
    std::string session;
    std::string command;
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    if ((error = updateSession(param, &session, result)) == tgs::TGSERROR_OK) {
        if ((error = getParam(param, "command", &command)) == tgs::TGSERROR_OK) {
#if 0
            if ((error = artsatd::getInstance().requestCommand(session, command)) != tgs::TGSERROR_OK) {
                error = setError(error, result);
            }
#else
            error = setError(tgs::TGSERROR_NO_SUPPORT, result);
#endif
        }
    }
    return error;
}

}// end of namespace
namespace database {

}// end of namespace

tgs::TGSError updateSession(Param const& param, std::string* session, Param* result)
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
tgs::TGSError getParam(Param const& param, std::string const& key, T* result)
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

template <>
tgs::TGSError getParam(Param const& param, std::string const& key, int* result)
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
void setResult(T const& param, std::string const& key, Param* result)
{
    (*result)[key] = param;
    return;
}

tgs::TGSError setError(tgs::TGSError error, Param* result)
{
    (*result)["message"] = error.print();
    return tgs::TGSERROR_FAILED;
}

tgs::TGSError controlRotatorAzimuth(ASDDeviceRotator& rotator, void const* info)
{
    return rotator->rotateAzimuthTo(*static_cast<int const*>(info));
}

tgs::TGSError controlRotatorElevation(ASDDeviceRotator& rotator, void const* info)
{
    return rotator->rotateElevationTo(*static_cast<int const*>(info));
}

tgs::TGSError controlTransceiverModeCW(ASDDeviceTransceiver& transceiver, void const* info)
{
    return transceiver->selectModeBeacon();
}

tgs::TGSError controlTransceiverModeFM(ASDDeviceTransceiver& transceiver, void const* info)
{
    return transceiver->selectModeCommunication();
}

tgs::TGSError controlTransceiverSender(ASDDeviceTransceiver& transceiver, void const* info)
{
    return transceiver->setFrequencySender(*static_cast<int const*>(info));
}

tgs::TGSError controlTransceiverReceiver(ASDDeviceTransceiver& transceiver, void const* info)
{
    return transceiver->setFrequencyReceiver(*static_cast<int const*>(info));
}

tgs::TGSError controlTNCModeCommand(ASDDeviceTNC& tnc, void const* info)
{
    return tnc->selectModeCommand();
}

tgs::TGSError controlTNCModeConverse(ASDDeviceTNC& tnc, void const* info)
{
    return tnc->selectModeConverse();
}

tgs::TGSError controlTNCPacket(ASDDeviceTNC& tnc, void const* info)
{
    return tnc->sendPacket(*static_cast<std::string const*>(info));
}

}// end of namespace
