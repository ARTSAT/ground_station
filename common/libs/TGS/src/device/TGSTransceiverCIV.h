/*
**      ARTSAT Project
**
**      Original Copyright (C) 2013 - 2013 HORIGUCHI Junshi.
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
**      TGSTransceiverCIV.h
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

#ifndef __TGS_TRANSCEIVERCIV_H
#define __TGS_TRANSCEIVERCIV_H

#include "TGSType.h"
#include "TGSDeviceInterface.h"

namespace tgs {

class TGSTransceiverCIV : public TGSDeviceInterface {
    public:
        typedef TGSTransceiverCIV           self;
        typedef TGSDeviceInterface          super;
    
    public:
        enum OperationModeEnum {
            OPERATIONMODE_LSB               = 0x00,
            OPERATIONMODE_USB               = 0x01,
            OPERATIONMODE_AM                = 0x02,
            OPERATIONMODE_CW                = 0x03,
            OPERATIONMODE_RTTY              = 0x04,
            OPERATIONMODE_FM                = 0x05,
            OPERATIONMODE_CW_R              = 0x07,
            OPERATIONMODE_RTTY_R            = 0x08,
            OPERATIONMODE_DV                = 0x17,
            OPERATIONMODE_UNKNOWN           = 0xFF
        };
        enum FilterEnum {
            FILTER_FIL1                     = 0x01,
            FILTER_FIL2                     = 0x02,
            FILTER_FIL3                     = 0x03,
            FILTER_UNKNOWN                  = 0xFF
        };
    
    public:
        class Notifier {
            protected:
                virtual void                onNotifyOperationFrequency      (int frequency);
                virtual void                onNotifyOperationMode           (OperationModeEnum mode, FilterEnum filter);
            friend      class               TGSTransceiverCIV;
        };
        class Transceiver {
            private:
                        unsigned char       _id;
                        TGSTransceiverCIV*  _civ;
                        Notifier*           _notifier;
            
            public:
                        void                setID                           (unsigned char param);
                        unsigned char       getID                           (void) const;
                        void                setNotifier                     (Notifier* param);
                        Notifier*           getNotifier                     (void) const;
                virtual TGSError            setOperationFrequency           (int param);
                virtual TGSError            getOperationFrequency           (int* result);
                virtual TGSError            setOperationMode                (OperationModeEnum mode, FilterEnum filter = FILTER_UNKNOWN);
                virtual TGSError            getOperationMode                (OperationModeEnum* mode, FilterEnum* filter = NULL);
                virtual TGSError            setSubBandMode                  (bool param);
                virtual TGSError            getSubBandMode                  (bool* result);
                virtual TGSError            setSatelliteMode                (bool param);
                virtual TGSError            getSatelliteMode                (bool* result);
                virtual TGSError            setTransceiveMode               (bool param);
                virtual TGSError            getTransceiveMode               (bool* result);
                virtual TGSError            setDataMode                     (bool mode, FilterEnum filter = FILTER_UNKNOWN);
                virtual TGSError            getDataMode                     (bool* mode, FilterEnum* filter = NULL);
                virtual bool                isConnected                     (void) const;
                virtual TGSError            connect                         (TGSTransceiverCIV* param);
                virtual void                disconnect                      (void);
                virtual TGSError            selectVFOMode                   (void);
                virtual TGSError            selectVFOA                      (void);
                virtual TGSError            selectVFOB                      (void);
                virtual TGSError            equalizeVFO                     (void);
                virtual TGSError            exchangeBand                    (void);
                virtual TGSError            selectMainBand                  (void);
                virtual TGSError            selectSubBand                   (void);
            protected:
                explicit                    Transceiver                     (unsigned char id);
                virtual                     ~Transceiver                    (void);
            private:
                                            Transceiver                     (Transceiver const&);
                        Transceiver&        operator=                       (Transceiver const&);
            friend      class               TGSTransceiverCIV;
        };
    
    private:
                unsigned char               _id;
                std::vector<Transceiver*>   _transceiver;
                std::vector<std::pair<unsigned char, std::string> >
                                            _queue;
                Notifier*                   _notifier;
    
    public:
        explicit                            TGSTransceiverCIV               (void);
        virtual                             ~TGSTransceiverCIV              (void);
                void                        setID                           (unsigned char param);
                unsigned char               getID                           (void) const;
                void                        setNotifier                     (Notifier* param);
                Notifier*                   getNotifier                     (void) const;
        virtual TGSError                    setOperationFrequency           (unsigned char id, int param);
        virtual TGSError                    getOperationFrequency           (unsigned char id, int* result);
        virtual TGSError                    setOperationMode                (unsigned char id, OperationModeEnum mode, FilterEnum filter = FILTER_UNKNOWN);
        virtual TGSError                    getOperationMode                (unsigned char id, OperationModeEnum* mode, FilterEnum* filter = NULL);
        virtual TGSError                    setSubBandMode                  (unsigned char id, bool param);
        virtual TGSError                    getSubBandMode                  (unsigned char id, bool* result);
        virtual TGSError                    setSatelliteMode                (unsigned char id, bool param);
        virtual TGSError                    getSatelliteMode                (unsigned char id, bool* result);
        virtual TGSError                    setTransceiveMode               (unsigned char id, bool param);
        virtual TGSError                    getTransceiveMode               (unsigned char id, bool* result);
        virtual TGSError                    setDataMode                     (unsigned char id, bool mode, FilterEnum filter = FILTER_UNKNOWN);
        virtual TGSError                    getDataMode                     (unsigned char id, bool* mode, FilterEnum* filter = NULL);
        virtual TGSError                    open                            (std::string const& port, int baud, bool verbose = true);
        virtual void                        close                           (void);
        virtual TGSError                    update                          (void);
        virtual TGSError                    selectVFOMode                   (unsigned char id);
        virtual TGSError                    selectVFOA                      (unsigned char id);
        virtual TGSError                    selectVFOB                      (unsigned char id);
        virtual TGSError                    equalizeVFO                     (unsigned char id);
        virtual TGSError                    exchangeBand                    (unsigned char id);
        virtual TGSError                    selectMainBand                  (unsigned char id);
        virtual TGSError                    selectSubBand                   (unsigned char id);
    private:
                TGSError                    connect                         (Transceiver* param);
                void                        disconnect                      (Transceiver* param);
                void                        disconnect                      (void);
                TGSError                    setParamBool                    (unsigned char id, std::string const& request, bool param);
                TGSError                    getParamBool                    (unsigned char id, std::string const& request, bool* result);
                TGSError                    selectParamVoid                 (unsigned char id, std::string const& request);
                TGSError                    set                             (unsigned char id, std::string const& request, std::string const& param);
                TGSError                    get                             (unsigned char id, std::string const& request, std::string* result);
                TGSError                    write                           (unsigned char id, std::string const& param);
                TGSError                    read                            (unsigned char id, std::string* result);
                void                        transceive                      (void);
                void                        notifyOperationFrequency        (unsigned char id, int param) const;
                void                        notifyOperationMode             (unsigned char id, OperationModeEnum mode, FilterEnum filter) const;
        static  TGSError                    test                            (std::string const& request, std::string const& response, std::string* result);
        static  TGSError                    formatOperationFrequency        (int param, std::string* result);
        static  TGSError                    parseOperationFrequency         (std::string const& param, int* result);
        static  TGSError                    formatOperationMode             (OperationModeEnum mode, FilterEnum filter, std::string* result);
        static  TGSError                    parseOperationMode              (std::string const& param, OperationModeEnum* mode, FilterEnum* filter);
    private:
                                            TGSTransceiverCIV               (TGSTransceiverCIV const&);
                TGSTransceiverCIV&          operator=                       (TGSTransceiverCIV const&);
};

/*public */inline void TGSTransceiverCIV::setID(unsigned char param)
{
    _id = param;
    return;
}

/*public */inline unsigned char TGSTransceiverCIV::getID(void) const
{
    return _id;
}

/*public */inline void TGSTransceiverCIV::setNotifier(Notifier* param)
{
    _notifier = param;
    return;
}

/*public */inline TGSTransceiverCIV::Notifier* TGSTransceiverCIV::getNotifier(void) const
{
    return _notifier;
}

/*public */inline void TGSTransceiverCIV::Transceiver::setID(unsigned char param)
{
    _id = param;
    return;
}

/*public */inline unsigned char TGSTransceiverCIV::Transceiver::getID(void) const
{
    return _id;
}

/*public */inline void TGSTransceiverCIV::Transceiver::setNotifier(Notifier* param)
{
    _notifier = param;
    return;
}

/*public */inline TGSTransceiverCIV::Notifier* TGSTransceiverCIV::Transceiver::getNotifier(void) const
{
    return _notifier;
}

}// end of namespace

#endif
