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
**      ASDServerRPC.h
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

#ifndef __ASD_SERVERRPC_H
#define __ASD_SERVERRPC_H

#include "ASDNetworkServer.h"
#include "document.h"

class ASDServerRPC : public ASDNetworkServer::Notifier {
    private:
        enum VariantTypeEnum {
            VARIANTTYPE_BOOL,
            VARIANTTYPE_INT,
            VARIANTTYPE_DOUBLE,
            VARIANTTYPE_STRING,
            VARIANTTYPE_LIST,
            VARIANTTYPE_MAP,
            VARIANTTYPE_BLANK,
            VARIANTTYPE_LIMIT
        };
        enum JSONCodeEnum {
            JSONCODE_OK                         = 0,
            JSONCODE_PARSEERROR                 = -32700,
            JSONCODE_INVALIDREQUEST             = -32600,
            JSONCODE_METHODNOTFOUND             = -32601,
            JSONCODE_INVALIDPARAMS              = -32602,
            JSONCODE_INTERNALERROR              = -32603
        };
    
    private:
        typedef boost::make_recursive_variant<
            bool,
            int,
            double,
            std::string,
            std::list<boost::recursive_variant_>,
            std::map<std::string, boost::recursive_variant_>,
            boost::blank
        >::type                                 Variant;
        typedef std::map<std::string, Variant>  Param;
    public:
        typedef boost::function<
            tgs::TGSError(ASDServerRPC const*, std::string const& host, Param const& param, Param* result)
        >                                       Method;
    
    private:
                std::map<std::string, Method>   _method;
                std::string                     _database;
    
    public:
        explicit                                ASDServerRPC                (void);
        virtual                                 ~ASDServerRPC               (void);
                tgs::TGSError                   open                        (std::string const& database);
                void                            close                       (void);
                /*
                 * params:
                 *   [any]
                 * result:
                 *   [any]
                 */
                tgs::TGSError                   rpcEcho                     (std::string const& host, Param const& param, Param* result) const;
                /*
                 * params:
                 *   [session <std::string>: session ID]
                 * result:
                 *   session <std::string>: session ID
                 *   version <std::string>: version string
                 */
                tgs::TGSError                   getVersion                  (std::string const& host, Param const& param, Param* result) const;
                /*
                 * params:
                 *   [session <std::string>: session ID]
                 * result:
                 *   session <std::string>: session ID
                 *   owner <int>: current owner
                 *                -1 not you
                 *                 0 nobody owning
                 *                +1 you
                 *   exclusive <bool>: exclusively owning
                 *   host <std::string>: owner's host
                 *   online <int>: online count
                 */
                tgs::TGSError                   getSession                  (std::string const& host, Param const& param, Param* result) const;
                /*
                 * params:
                 *   [session <std::string>: session ID]
                 *   manual <bool>: state flag
                 * result:
                 *   session <std::string>: session ID
                 */
                tgs::TGSError                   setManualRotator            (std::string const& host, Param const& param, Param* result) const;
                /*
                 * params:
                 *   [session <std::string>: session ID]
                 * result:
                 *   session <std::string>: session ID
                 *   manual <bool>: state flag
                 */
                tgs::TGSError                   getManualRotator            (std::string const& host, Param const& param, Param* result) const;
                /*
                 * params:
                 *   [session <std::string>: session ID]
                 *   manual <bool>: state flag
                 * result:
                 *   session <std::string>: session ID
                 */
                tgs::TGSError                   setManualTransceiver        (std::string const& host, Param const& param, Param* result) const;
                /*
                 * params:
                 *   [session <std::string>: session ID]
                 * result:
                 *   session <std::string>: session ID
                 *   manual <bool>: state flag
                 */
                tgs::TGSError                   getManualTransceiver        (std::string const& host, Param const& param, Param* result) const;
                /*
                 * params:
                 *   [session <std::string>: session ID]
                 *   manual <bool>: state flag
                 * result:
                 *   session <std::string>: session ID
                 */
                tgs::TGSError                   setManualTNC                (std::string const& host, Param const& param, Param* result) const;
                /*
                 * params:
                 *   [session <std::string>: session ID]
                 * result:
                 *   session <std::string>: session ID
                 *   manual <bool>: state flag
                 */
                tgs::TGSError                   getManualTNC                (std::string const& host, Param const& param, Param* result) const;
                /*
                 * params:
                 *   [session <std::string>: session ID]
                 *   query <std::string>: NORAD number or satellite's name
                 * result:
                 *   session <std::string>: session ID
                 */
                tgs::TGSError                   setNORAD                    (std::string const& host, Param const& param, Param* result) const;
                /*
                 * params:
                 *   [session <std::string>: session ID]
                 * result:
                 *   session <std::string>: session ID
                 *   norad <int>: current NORAD number
                 *                -1 not set yet
                 */
                tgs::TGSError                   getNORAD                    (std::string const& host, Param const& param, Param* result) const;
                /*
                 * params:
                 *   [session <std::string>: session ID]
                 *   mode <std::string>: mode name
                 *                       "" idle mode
                 * result:
                 *   session <std::string>: session ID
                 */
                tgs::TGSError                   setMode                     (std::string const& host, Param const& param, Param* result) const;
                /*
                 * params:
                 *   [session <std::string>: session ID]
                 * result:
                 *   session <std::string>: session ID
                 *   mode <std::string>: mode name
                 *                       "" idle mode
                 */
                tgs::TGSError                   getMode                     (std::string const& host, Param const& param, Param* result) const;
                /*
                 * params:
                 *   [session <std::string>: session ID]
                 * result:
                 *   session <std::string>: session ID
                 *   time <std::string>: current time
                 */
                tgs::TGSError                   getTime                     (std::string const& host, Param const& param, Param* result) const;
                /*
                 * params:
                 *   [session <std::string>: session ID]
                 * result:
                 *   session <std::string>: session ID
                 *   callsign <std::string>: observer's callsign
                 */
                tgs::TGSError                   getObserverCallsign         (std::string const& host, Param const& param, Param* result) const;
                /*
                 * params:
                 *   [session <std::string>: session ID]
                 * result:
                 *   session <std::string>: session ID
                 *   latitude <double>: observer's latitude
                 *   longitude <double>: observer's longitude
                 *   altitude <double>: observer's altitude
                 */
                tgs::TGSError                   getObserverPosition         (std::string const& host, Param const& param, Param* result) const;
                /*
                 * params:
                 *   [session <std::string>: session ID]
                 * result:
                 *   session <std::string>: session ID
                 *   azimuth <int>: observer's rotator azimuth
                 *                  -1 invalid state
                 *   elevation <int>: observer's rotator elevation
                 *                  -1 invalid state
                 */
                tgs::TGSError                   getObserverDirection        (std::string const& host, Param const& param, Param* result) const;
                /*
                 * params:
                 *   [session <std::string>: session ID]
                 * result:
                 *   session <std::string>: session ID
                 *   sender <int>: observer's transceiver sender frequency
                 *                  -1 invalid state
                 *   receiver <int>: observer's transceiver receiver frequency
                 *                  -1 invalid state
                 */
                tgs::TGSError                   getObserverFrequency        (std::string const& host, Param const& param, Param* result) const;
                /*
                 * params:
                 *   [session <std::string>: session ID]
                 * result:
                 *   session <std::string>: session ID
                 *   latitude <double>: satellite's latitude
                 *   longitude <double>: satellite's longitude
                 *   altitude <double>: satellite's altitude
                 */
                tgs::TGSError                   getSatellitePosition        (std::string const& host, Param const& param, Param* result) const;
                /*
                 * params:
                 *   [session <std::string>: session ID]
                 * result:
                 *   session <std::string>: session ID
                 *   azimuth <double>: satellite's azimuth
                 *   elevation <double>: satellite's elevation
                 */
                tgs::TGSError                   getSatelliteDirection       (std::string const& host, Param const& param, Param* result) const;
                /*
                 * params:
                 *   [session <std::string>: session ID]
                 * result:
                 *   session <std::string>: session ID
                 *   beacon <double>: satellite's beacon frequency
                 *   sender <double>: satellite's sender frequency
                 *   receiver <double>: satellite's receiver frequency
                 */
                tgs::TGSError                   getSatelliteFrequency       (std::string const& host, Param const& param, Param* result) const;
                /*
                 * params:
                 *   [session <std::string>: session ID]
                 * result:
                 *   session <std::string>: session ID
                 *   sender <double>: satellite's sender doppler shift ratio
                 *   receiver <double>: satellite's receiver doppler shift ratio
                 */
                tgs::TGSError                   getSatelliteDopplerShift    (std::string const& host, Param const& param, Param* result) const;
                /*
                 * params:
                 *   [session <std::string>: session ID]
                 * result:
                 *   session <std::string>: session ID
                 *   aos <std::string>: satellite's next AOS time
                 *   los <std::string>: satellite's next LOS time
                 */
                tgs::TGSError                   getSatelliteAOSLOS          (std::string const& host, Param const& param, Param* result) const;
                /*
                 * params:
                 *   [session <std::string>: session ID]
                 * result:
                 *   session <std::string>: session ID
                 *   mel <double>: satellite's next MEL
                 */
                tgs::TGSError                   getSatelliteMEL             (std::string const& host, Param const& param, Param* result) const;
                /*
                 * params:
                 *   [session <std::string>: session ID]
                 * result:
                 *   session <std::string>: session ID
                 *   start <std::string>: next rotation start time
                 */
                tgs::TGSError                   getRotatorStart             (std::string const& host, Param const& param, Param* result) const;
                /*
                 * params:
                 *   [session <std::string>: session ID]
                 * result:
                 *   session <std::string>: session ID
                 *   code <int>: current error code
                 *   message <std::string>: current error message
                 */
                tgs::TGSError                   getError                    (std::string const& host, Param const& param, Param* result) const;
                /*
                 * params:
                 *   [session <std::string>: session ID]
                 * result:
                 *   session <std::string>: session ID
                 *   valid <bool>: valid flag
                 */
                tgs::TGSError                   isValidRotator              (std::string const& host, Param const& param, Param* result) const;
                /*
                 * params:
                 *   [session <std::string>: session ID]
                 * result:
                 *   session <std::string>: session ID
                 *   valid <bool>: valid flag
                 */
                tgs::TGSError                   isValidTransceiver          (std::string const& host, Param const& param, Param* result) const;
                /*
                 * params:
                 *   [session <std::string>: session ID]
                 * result:
                 *   session <std::string>: session ID
                 *   valid <bool>: valid flag
                 */
                tgs::TGSError                   isValidTNC                  (std::string const& host, Param const& param, Param* result) const;
                /*
                 * params:
                 *   [session <std::string>: session ID]
                 *   owner <bool>: own flag
                 * result:
                 *   session <std::string>: session ID
                 */
                tgs::TGSError                   controlSession              (std::string const& host, Param const& param, Param* result) const;
                /*
                 * params:
                 *   [session <std::string>: session ID]
                 *   exclusive <bool>: exclusive flag
                 * result:
                 *   session <std::string>: session ID
                 */
                tgs::TGSError                   excludeSession              (std::string const& host, Param const& param, Param* result) const;
                /*
                 * params:
                 *   [session <std::string>: session ID]
                 *   azimuth <int>: rotator's azimuth
                 * result:
                 *   session <std::string>: session ID
                 */
                tgs::TGSError                   setRotatorAzimuth           (std::string const& host, Param const& param, Param* result) const;
                /*
                 * params:
                 *   [session <std::string>: session ID]
                 *   elevation <int>: rotator's elevation
                 * result:
                 *   session <std::string>: session ID
                 */
                tgs::TGSError                   setRotatorElevation         (std::string const& host, Param const& param, Param* result) const;
                /*
                 * params:
                 *   [session <std::string>: session ID]
                 *   mode <std::string>: transceiver's mode
                 *                       "CW"
                 *                       "FM"
                 * result:
                 *   session <std::string>: session ID
                 */
                tgs::TGSError                   setTransceiverMode          (std::string const& host, Param const& param, Param* result) const;
                /*
                 * params:
                 *   [session <std::string>: session ID]
                 *   sender <int>: transceiver's sender frequency
                 * result:
                 *   session <std::string>: session ID
                 */
                tgs::TGSError                   setTransceiverSender        (std::string const& host, Param const& param, Param* result) const;
                /*
                 * params:
                 *   [session <std::string>: session ID]
                 *   receiver <int>: transceiver's receiver frequency
                 * result:
                 *   session <std::string>: session ID
                 */
                tgs::TGSError                   setTransceiverReceiver      (std::string const& host, Param const& param, Param* result) const;
                /*
                 * params:
                 *   [session <std::string>: session ID]
                 *   mode <std::string>: TNC's mode
                 *                       "Command"
                 *                       "Converse"
                 * result:
                 *   session <std::string>: session ID
                 */
                tgs::TGSError                   setTNCMode                  (std::string const& host, Param const& param, Param* result) const;
                /*
                 * params:
                 *   [session <std::string>: session ID]
                 *   packet <std::string>: sending packet data
                 * result:
                 *   session <std::string>: session ID
                 */
                tgs::TGSError                   sendTNCPacket               (std::string const& host, Param const& param, Param* result) const;
                /*
                 * params:
                 *   [session <std::string>: session ID]
                 *   command <std::string>: requesting command data
                 * result:
                 *   session <std::string>: session ID
                 */
                tgs::TGSError                   requestCommand              (std::string const& host, Param const& param, Param* result) const;
    private:
        virtual tgs::TGSError                   onRequest                   (RequestRec const& request, ResponseRec* response);
                void                            replyJSON                   (RequestRec const& request, ResponseRec* response) const;
                void                            processJSON                 (std::string const& host, rapidjson::Value& request, rapidjson::Value* response, rapidjson::Document::AllocatorType& allocator) const;
        static  void                            returnJSON                  (JSONCodeEnum code, rapidjson::Value& result, rapidjson::Value& id, rapidjson::Value* response, rapidjson::Document::AllocatorType& allocator);
        static  void                            toVariant                   (rapidjson::Value const& param, Variant* result);
        static  void                            toJSON                      (Variant const& param, rapidjson::Value* result, rapidjson::Document::AllocatorType& allocator);
        static  tgs::TGSError                   updateSession               (Param const& param, std::string* session, Param* result);
        template <typename T>
        static  tgs::TGSError                   getParam                    (Param const& param, std::string const& key, T* result);
        static  tgs::TGSError                   getParam                    (Param const& param, std::string const& key, int* result);
        template <typename T>
        static  void                            setResult                   (T const& param, std::string const& key, Param* result);
        static  tgs::TGSError                   setError                    (tgs::TGSError error, Param* result);
    private:
                                                ASDServerRPC                (ASDServerRPC const&);
                ASDServerRPC&                   operator=                   (ASDServerRPC const&);
};

#endif
