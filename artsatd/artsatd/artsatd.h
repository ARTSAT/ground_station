/*
**      IridiumFrameworks
**
**      Original Copyright (C) 2013 - 2015 HORIGUCHI Junshi.
**                                          http://iridium.jp/
**                                          zap00365@nifty.com
**      Portions Copyright (C) <year> <author>
**                                          <website>
**                                          <e-mail>
**      Version     POSIX / C++11
**      Website     http://iridium.jp/
**      E-mail      zap00365@nifty.com
**
**      This source code is for Xcode.
**      Xcode 6.1 (Apple LLVM 6.0)
**
**      artsatd.h
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

#ifndef __ARTSATD_H
#define __ARTSATD_H

#include "IRXDaemon.h"
#include "ASDPluginInterface.h"
#include "TGSDeviceLoader.h"
#include "ASDDeviceRotator.h"
#include "TGSTransceiverCIV.h"
#include "ASDDeviceTransceiver.h"
#include "ASDDeviceTNC.h"
#include "ASDHTTPClientInterface.h"
#include "ASDServerOperation.h"
#include "ASDServerRPC.h"
//<<<
#include "ASDOrbitPassFactory.h"
#include "ASDRotationSolver.h"
//>>>

class artsatd : public ir::IRXDaemon, private tgs::TGSTNCInterface::Notifier {
    public:
        typedef artsatd                         self;
        typedef ir::IRXDaemon                   super;
    
    private:
        enum ModeEnum {
            MODE_CW,
            MODE_CW_TEST,
            MODE_FM,
            MODE_FM_TEST,
            MODE_LIMIT
        };
        enum ModeTransceiverEnum {
            MODETRANSCEIVER_BEACON,
            MODETRANSCEIVER_COMMUNICATION,
            MODETRANSCEIVER_LIMIT
        };
        enum ModeTNCEnum {
            MODETNC_COMMAND,
            MODETNC_CONVERSE,
            MODETNC_LIMIT
        };
    
    private:
        struct ConfigRec {
            std::string                         serverOperationPort;
            int                                 serverOperationListen;
            std::string                         serverRPCPort;
            int                                 serverRPCListen;
            int                                 sessionMaximum;
            int                                 sessionTimeout;
            bool                                sessionLocalonly;
            std::string                         observerCallsign;
            double                              observerLatitude;
            double                              observerLongitude;
            double                              observerAltitude;
            int                                 cwTestAzimuth;
            int                                 cwTestElevation;
            int                                 fmTestAzimuth;
            int                                 fmTestElevation;
            int                                 algorithmLookahead;
            int                                 intervalSession;
            int                                 intervalRotator;
            int                                 intervalTransceiver;
            int                                 intervalTNC;
            int                                 intervalLog;
            int                                 waitBoot;
            int                                 waitLoop;
        };
        struct SessionRec {
            std::map<std::string, ir::IRXTime>  id;
            std::string                         owner;
            bool                                exclusive;
            std::string                         host;
            boost::random::mt19937              random;
        };
        struct ControlRec {
            bool                                manualRotator;
            bool                                manualTransceiver;
            bool                                manualTNC;
            int                                 exnorad;
            ModeEnum                            mode;
        };
        struct CurrentRec {
            int                                 exnorad;
            ir::IRXTime                         time;
        };
        struct StateRec {
            bool                                manualRotator;
            bool                                manualTransceiver;
            bool                                manualTNC;
            tgs::TGSPhysicsDatabase::FieldRec   field;
            boost::shared_ptr<tgs::TGSOrbitInterface>
                                                orbit;
            ModeEnum                            mode;
            ModeTransceiverEnum                 modeTransceiver;
            ModeTNCEnum                         modeTNC;
            std::string                         callsign;
            ir::IRXTime                         timeSession;
            ir::IRXTime                         timeRotator;
            double                              azimuth;
            double                              elevation;
            ir::IRXTime                         timeTransceiver;
            double                              sender;
            double                              receiver;
            ir::IRXTime                         timeTNC;
            ir::IRXTime                         timeLog;
        };
        struct MonitorRec {
            double                              latitude;
            double                              longitude;
            double                              altitude;
            double                              azimuth;
            double                              elevation;
            double                              distance;
            double                              speed;
            double                              beacon;
            double                              sender;
            double                              receiver;
            double                              dopplerSender;
            double                              dopplerReceiver;
            //<<<
            ir::IRXTime                         aos;
            ir::IRXTime                         los;
            double                              mel;
            ir::IRXTime                         start;
            //>>>
            tgs::TGSError                       error;
        };
        struct CommandRec {
            std::deque<std::string>             queue;
        };
    
    private:
                ConfigRec                       _config;
                tgs::TGSPhysicsDatabase         _database;
                std::map<tgs::OrbitData::TypeEnum, boost::shared_ptr<tgs::TGSOrbitInterface> >
                                                _orbit;
                std::map<int, boost::shared_ptr<ASDPluginInterface> >
                                                _plugin;
                tgs::TGSDeviceLoader            _loader;
                ASDDeviceRotator                _rotator;
                tgs::TGSTransceiverCIV          _civ;
                ASDDeviceTransceiver            _transceiver;
                ASDDeviceTNC                    _tnc;
                std::vector<boost::shared_ptr<ASDHTTPClientInterface> >
                                                _clientHTTP;
                ASDNetworkServer                _serverOperation;
                ASDNetworkServer                _serverRPC;
                ASDServerOperation              _replierOperation;
                ASDServerRPC                    _replierRPC;
                //<<<
                ASDOrbitPassFactory             _passFactory;
                ASDOrbitPass                    _pass;
                ASDRotationSolver               _rotationSolver;
                //>>>
                SessionRec                      _session;
        mutable boost::shared_mutex             _mutex_session;
                ControlRec                      _control;
        mutable boost::shared_mutex             _mutex_control;
                CurrentRec                      _current;
        mutable boost::shared_mutex             _mutex_current;
                StateRec                        _state;
                MonitorRec                      _monitor;
        mutable boost::shared_mutex             _mutex_monitor;
                CommandRec                      _command;
        mutable boost::shared_mutex             _mutex_command;
    
    public:
        static  std::string const&              getVersion                  (void);
        static  artsatd&                        getInstance                 (void);
        static  ASDDeviceRotator&               getRotator                  (void);
        static  ASDDeviceTransceiver&           getTransceiver              (void);
        static  ASDDeviceTNC&                   getTNC                      (void);
                void                            getSession                  (std::string const& session, int* owner, bool* exclusive, std::string* host, int* online) const;
                tgs::TGSError                   setManualRotator            (std::string const& session, bool manual);
                bool                            getManualRotator            (void) const;
                tgs::TGSError                   setManualTransceiver        (std::string const& session, bool manual);
                bool                            getManualTransceiver        (void) const;
                tgs::TGSError                   setManualTNC                (std::string const& session, bool manual);
                bool                            getManualTNC                (void) const;
                tgs::TGSError                   setEXNORAD                  (std::string const& session, std::string const& query);
                tgs::TGSError                   setEXNORAD                  (std::string const& session, int exnorad);
                int                             getEXNORAD                  (void) const;
                tgs::TGSError                   setMode                     (std::string const& session, std::string const& mode);
                std::string                     getMode                     (void) const;
                ir::IRXTime                     getTime                     (void) const;
                boost::shared_ptr<ASDPluginInterface>
                                                getPlugin                   (int exnorad) const;
                void                            getObserverCallsign         (std::string* callsign) const;
                void                            getObserverPosition         (double* latitude, double* longitude, double* altitude) const;
                void                            getSpacecraftPosition       (double* latitude, double* longitude, double* altitude) const;
                void                            getSpacecraftDirection      (double* azimuth, double* elevation) const;
                void                            getSpacecraftDistance       (double* distance) const;
                void                            getSpacecraftSpeed          (double* speed) const;
                void                            getSpacecraftFrequency      (double* beacon, double* sender, double* receiver) const;
                void                            getSpacecraftDopplerShift   (double* sender, double* receiver) const;
                //<<<
                void                            getSpacecraftAOSLOS         (ir::IRXTime* aos, ir::IRXTime* los) const;
                void                            getSpacecraftMEL            (double* mel) const;
                void                            getRotatorStart             (ir::IRXTime* start) const;
                //>>>
                tgs::TGSError                   getError                    (void) const;
                tgs::TGSError                   requestSession              (std::string const& host, std::string* session, bool* update);
                tgs::TGSError                   controlSession              (std::string const& session, bool owner, std::string const& host);
                tgs::TGSError                   excludeSession              (std::string const& session, bool exclusive);
                tgs::TGSError                   controlManualRotator        (std::string const& session, tgs::TGSError (*function)(ASDDeviceRotator& rotator, void const* info), void const* info);
                tgs::TGSError                   controlManualTransceiver    (std::string const& session, tgs::TGSError (*function)(ASDDeviceTransceiver& transceiver, void const* info), void const* info);
                tgs::TGSError                   controlManualTNC            (std::string const& session, tgs::TGSError (*function)(ASDDeviceTNC& tnc, void const* info), void const* info);
                tgs::TGSError                   requestCommand              (std::string const& session, std::string const& command);
        static  tgs::TGSError                   controlRotatorAzimuth       (ASDDeviceRotator& rotator, void const* info);
        static  tgs::TGSError                   controlRotatorElevation     (ASDDeviceRotator& rotator, void const* info);
        static  tgs::TGSError                   controlTransceiverModeCW    (ASDDeviceTransceiver& transceiver, void const* info);
        static  tgs::TGSError                   controlTransceiverModeFM    (ASDDeviceTransceiver& transceiver, void const* info);
        static  tgs::TGSError                   controlTransceiverSender    (ASDDeviceTransceiver& transceiver, void const* info);
        static  tgs::TGSError                   controlTransceiverReceiver  (ASDDeviceTransceiver& transceiver, void const* info);
        static  tgs::TGSError                   controlTNCModeCommand       (ASDDeviceTNC& tnc, void const* info);
        static  tgs::TGSError                   controlTNCModeConverse      (ASDDeviceTNC& tnc, void const* info);
        static  tgs::TGSError                   controlTNCPacket            (ASDDeviceTNC& tnc, void const* info);
    private:
        explicit                                artsatd                     (void);
        virtual                                 ~artsatd                    (void);
        virtual int                             usage                       (int argc, char const* argv[]);
        virtual int                             initialize                  (void);
        virtual void                            terminate                   (void);
        virtual void                            loop                        (void);
        virtual void                            syslog                      (int priority, char const* format, va_list ap);
        virtual void                            onNotifyReceivePacket       (std::string const& packet);
                void                            getControl                  (ControlRec* control) const;
                void                            setCurrent                  (CurrentRec const& current);
                void                            setMonitor                  (MonitorRec const& monitor);
                void                            cleanSession                (ir::IRXTime const& time);
                tgs::TGSError                   openConfig                  (void);
                tgs::TGSError                   openDatabase                (void);
                tgs::TGSError                   openOrbit                   (void);
                tgs::TGSError                   openPlugin                  (void);
                tgs::TGSError                   openDevice                  (void);
                tgs::TGSError                   openNetwork                 (void);
                void                            closeConfig                 (void);
                void                            closeDatabase               (void);
                void                            closeOrbit                  (void);
                void                            closePlugin                 (void);
                void                            closeDevice                 (void);
                void                            closeNetwork                (void);
                void                            switchTransceiver           (ModeTransceiverEnum mode);
                void                            switchTNC                   (ModeTNCEnum mode, std::string const& callsign);
                void                            operateSession              (ir::IRXTime const& time);
                void                            operateRotator              (ir::IRXTime const& time, double azimuth, double elevation);
                void                            operateTransceiver          (ir::IRXTime const& time, double sender, double receiver);
                tgs::TGSError                   operateTNC                  (ir::IRXTime const& time, std::string const& command);
                void                            operateLog                  (ir::IRXTime const& time, int exnorad, ModeEnum mode, double azimuth, double elevation, double beacon, double sender, double receiver, tgs::TGSError info);
                void                            resetRotator                (void);
                void                            resetTransceiver            (void);
                void                            resetTNC                    (void);
        static  void                            resetSession                (SessionRec* session);
        static  void                            resetControl                (ControlRec* control);
        static  void                            resetCurrent                (ControlRec const& control, CurrentRec* current);
        static  void                            resetState                  (ControlRec const& control, StateRec* state);
        static  void                            resetMonitor                (MonitorRec* monitor);
        static  void                            resetCommand                (CommandRec* command);
        static  int                             setWorkspace                (void);
        static  tgs::TGSError                   xmlReadText                 (tinyxml2::XMLElement const* parent, std::string const& tag, std::string* result);
        static  tgs::TGSError                   xmlReadBool                 (tinyxml2::XMLElement const* parent, std::string const& tag, bool* result);
        static  tgs::TGSError                   xmlReadInteger              (tinyxml2::XMLElement const* parent, std::string const& tag, int* result);
        static  tgs::TGSError                   xmlReadDouble               (tinyxml2::XMLElement const* parent, std::string const& tag, double* result);
    private:
                                                artsatd                     (artsatd const&);
                artsatd&                        operator=                   (artsatd const&);
};

/*public static */inline ASDDeviceRotator& artsatd::getRotator(void)
{
    return getInstance()._rotator;
}

/*public static */inline ASDDeviceTransceiver& artsatd::getTransceiver(void)
{
    return getInstance()._transceiver;
}

/*public static */inline ASDDeviceTNC& artsatd::getTNC(void)
{
    return getInstance()._tnc;
}

#endif
