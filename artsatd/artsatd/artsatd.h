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
**      Xcode 4.6.2 (Apple LLVM compiler 4.2, LLVM GCC 4.2)
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
#include "TGSDeviceLoader.h"
#include "ASDDeviceSatellite.h"
#include "ASDDeviceRotator.h"
#include "TGSTransceiverCIV.h"
#include "ASDDeviceTransceiver.h"
#include "ASDDeviceTNC.h"
#include "ASDTLEClientInterface.h"
#include "ASDNetworkServer.h"
#include "ASDServerDatabase.h"
#include "ASDServerOperation.h"
#include "TGSOrbitTLE.h"

class artsatd : public ir::IRXDaemon, private tgs::TGSTNCInterface::Notifier {
    public:
        typedef artsatd                         self;
        typedef ir::IRXDaemon                   super;
    
    private:
        struct SessionRec {
            unsigned int                        id;
            bool                                exclusive;
        };
        struct StateRec {
            bool                                active;
            bool                                manualSatellite;
            bool                                manualRotator;
            bool                                manualTransceiver;
            bool                                manualTnc;
            std::string                         mode;
            int                                 norad;
        };
        struct ControlRec {
            std::string                         mode;
            tgs::TGSPhysicsDatabase::FieldRec   field;
            tgs::TGSOrbitTLE                    orbit;
            double                              azimuth;
            double                              elevation;
            double                              beacon;
            double                              sender;
            double                              receiver;
        };
        struct MonitorRec {
            tgs::TGSPhysicsDatabase::FieldRec   field;
            double                              azimuth;
            double                              elevation;
            double                              beacon;
            double                              sender;
            double                              receiver;
            std::string                         error;
        };
    
    private:
                tgs::TGSPhysicsDatabase         _physics;
                tgs::TGSDeviceLoader            _loader;
                ASDDeviceSatellite              _satellite;
                ASDDeviceRotator                _rotator;
                tgs::TGSTransceiverCIV          _civ;
                ASDDeviceTransceiver            _transceiver;
                ASDDeviceTNC                    _tnc;
                std::vector<boost::shared_ptr<ASDTLEClientInterface> >
                                                _clientTle;
                ASDNetworkServer                _serverDatabase;
                ASDNetworkServer                _serverOperation;
                ASDServerDatabase               _replierDatabase;
                ASDServerOperation              _replierOperation;
                SessionRec                      _session;
        mutable boost::mutex                    _mutex_session;
                StateRec                        _state;
        mutable boost::mutex                    _mutex_state;
                ControlRec                      _control;
                MonitorRec                      _monitor;
        mutable boost::mutex                    _mutex_monitor;
                std::deque<std::string>         _queue;
        mutable boost::mutex                    _mutex_queue;
    
    public:
        static  artsatd&                        getInstance                 (void);
        static  ASDDeviceSatellite&             getSatellite                (void);
        static  ASDDeviceRotator&               getRotator                  (void);
        static  ASDDeviceTransceiver&           getTransceiver              (void);
        static  ASDDeviceTNC&                   getTNC                      (void);
                tgs::TGSError                   setActive                   (bool param);
                bool                            getActive                   (void) const;
                tgs::TGSError                   setManualSatellite          (bool param);
                bool                            getManualSatellite          (void) const;
                tgs::TGSError                   setManualRotator            (bool param);
                bool                            getManualRotator            (void) const;
                tgs::TGSError                   setManualTransceiver        (bool param);
                bool                            getManualTransceiver        (void) const;
                tgs::TGSError                   setManualTNC                (bool param);
                bool                            getManualTNC                (void) const;
                tgs::TGSError                   setMode                     (std::string const& param);
                std::string                     getMode                     (void) const;
                tgs::TGSError                   setNorad                    (int param);
                int                             getNorad                    (void) const;
                double                          getAngleAzimuth             (void) const;
                double                          getAngleElevation           (void) const;
                double                          getFrequencyBeacon          (void) const;
                double                          getFrequencySender          (void) const;
                double                          getFrequencyReceiver        (void) const;
                std::string                     getError                    (void) const;
                void                            queueCommand                (std::string const& param);
    private:
        explicit                                artsatd                     (void);
        virtual                                 ~artsatd                    (void);
        virtual int                             usage                       (int argc, char const* argv[]);
        virtual int                             initialize                  (void);
        virtual void                            terminate                   (void);
        virtual void                            loop                        (void);
        virtual void                            syslog                      (int priority, char const* format, va_list ap);
        virtual void                            onNotifyReceivePacket       (std::string const& packet);
                tgs::TGSError                   openDatabase                (void);
                tgs::TGSError                   openDevice                  (void);
                tgs::TGSError                   openNetwork                 (void);
                void                            closeDatabase               (void);
                void                            closeDevice                 (void);
                void                            closeNetwork                (void);
        static  int                             setWorkspace                (void);
    private:
                                                artsatd                     (artsatd const&);
                artsatd&                        operator=                   (artsatd const&);
};

/*public static */inline ASDDeviceSatellite& artsatd::getSatellite(void)
{
    return getInstance()._satellite;
}

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
