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
**      ASDServerRPCMethods.h
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

#ifndef __ASD_SERVERRPCMETHODS_H
#define __ASD_SERVERRPCMETHODS_H

#include "ASDServerRPC.h"

namespace ASDServerRPC {
    namespace sys {
        
        // params: {<ANY>}
        // result: {"args": <ANY>}
        Result rpcEcho(const Params& args, Params *result);
        
    }
    namespace trans {
        
        /* params: {"manual": <bool>}      */
        /* result: {"manual": <bool>}      */
        Result setManualRotator(const Params& args, Params *result);
        
        /* params: {}                      */
        /* result: {"manual": <bool>}      */
        Result getManualRotator(const Params& args, Params *result);
        
        /* params: {"manual": <bool>}      */
        /* result: {"manual": <bool>}      */
        Result setManualTransceiver(const Params& args, Params *result);
        
        /* params: {}                      */
        /* result: {"manual": <bool>}      */
        Result getManualTransceiver(const Params& args, Params *result);
        
        /* params: {"manual": <bool>}      */
        /* result: {"manual": <bool>}      */
        Result setManualTNC(const Params& args, Params *result);
        
        /* params: {}                      */
        /* result: {"manual": <bool>}      */
        Result getManualTNC(const Params& args, Params *result);
        
        /* params: {}                      */
        /* result: {"valid": <bool>}       */
        Result getStateRotator(const Params& args, Params *result);
        
        /* params: {}                      */
        /* result: {"valid": <bool>}       */
        Result getStateTransceiver(const Params& args, Params *result);
        
        /* params: {}                      */
        /* result: {"valid": <bool>}       */
        Result getStateTNC(const Params& args, Params *result);
    
        /* params: {"mode": <string>}      */
        /* result: {"mode": <string>}      */
        Result setMode(const Params& args, Params *result);
        
        /* params: {}                      */
        /* result: {"mode": <string>}      */
        Result getMode(const Params& args, Params *result);
        
        /* params: {"norad": <int>}        */
        /* result: {"norad": <int>}        */
        Result setNorad(const Params& args, Params *result);
        
        /* params: {}                      */
        /* result: {"norad": <int>}        */
        Result getNorad(const Params& args, Params *result);
        
        /* params: {}                      */
        /* result: {"azimuth": <dobule>}   */
        Result getAngleAzimuth(const Params& args, Params *result);
        
        /* params: {}                      */
        /* result: {"elevation": <dobule>} */
        Result getAngleElevation(const Params& args, Params *result);
        
        /* params: {}                      */
        /* result: {"beacon": <dobule>}    */
        Result getFrequencyBeacon(const Params& args, Params *result);
        
        /* params: {}                      */
        /* result: {"sender": <dobule>}    */
        Result getFrequencySender(const Params& args, Params *result);
        
        /* params: {}                      */
        /* result: {"receiver": <dobule>}  */
        Result getFrequencyReceiver(const Params& args, Params *result);
        
        /* params: {"command": <string>}   */
        /* result: {"command": <string>}   */
        Result sendSafeCommand(const Params& args, Params *result);
    }
    namespace pass {
        /* params: {}                      */
        /* result: {"visible": <bool>}     */
        Result getStateNearest(const Params& args, Params *result);
    }
    namespace db {
        
        /* params: {"norad": <int>, 
                    "name":  <string>}                   */
        /* result: {"name":  <string>}                   */
        Result setName(const Params& args, Params *result);
         
        /* params: {"norad": <int>}                      */
        /* result: {"name":  <string>}                   */
        Result getName(const Params& args, Params *result);
        
        /* params: {"norad":    <int>, 
                    "callsign": <string>}                */
        /* result: {"callsign": <string>}                */
        Result setCallsign(const Params& args, Params *result);
        
        /* params: {"norad":    <int>}                   */
        /* result: {"callsign": <string>}                */
        Result getCallsign(const Params& args, Params *result);
        
        /* params: {"norad":     <int>, 
                   ("mode":      <string>), 
                   ("frequency": <double>), 
                   ("drift":     <double>)}              */
        /* result: {"mode":      <string>, 
                    "frequency": <double>, 
                    "drift":     <double>}               */
        Result setRadioBeacon(const Params& args, Params *result);
        
        /* params: {"norad":     <int>}                  */
        /* result: {"mode":      <string>,
                    "frequency": <double>, 
                    "drift":     <double>}               */
        Result getRadioBeacon(const Params& args, Params *result);
        
        /* params: {"norad":     <int>,
                   ("mode":      <string>),
                   ("frequency": <double>),
                   ("drift":     <double>)}              */
        /* result: {"mode":      <string>, 
                    "frequency": <double>, 
                    "drift":     <double>}               */
        Result setRadioSender(const Params& args, Params *result);
        
        /* params: {"norad":     <int>}                  */
        /* result: {"mode":      <string>,
                    "frequency": <double>, 
                    "drift":     <double>}               */
        Result getRadioSender(const Params& args, Params *result);
        
        /* params: {"norad":     <int>,
                   ("mode":      <string>),
                   ("frequency": <double>),
                   ("drift":     <double>)}              */
        /* result: {"mode":      <string>, 
                    "frequency": <double>, 
                    "drift":     <double>}               */
        Result setRadioReceiver(const Params& args, Params *result);
        
        /* params: {"norad":     <int>}                  */
        /* result: {"mode":      <string>, 
                    "frequency": <double>, 
                    "drift":     <double>}               */
        Result getRadioReceiver(const Params& args, Params *result);
        
        /* params: {"time":          <string>,
                    "tle":  {"name": <string>,
                             "one":  <string>,
                             "two":  <string>}}          */
        /* result: {"time":          <string>,
                    "tle":  {"name": <string>,
                             "one":  <string>,
                             "two":  <string>}}          */
        Result setOrbitData(const Params& args, Params *result);
        
        /* params: {"norad":          <int>}             */
        /* result: {"time":           <string>,
                    "tle":   {"name": <string>,
                              "one":  <string>,
                              "two":  <string>}}         */
        Result getOrbitData(const Params& args, Params *result);
        
        /* params: {}                                    */
        /* result: {"count":          <int>}             */
        Result getCount(const Params& args, Params *result);
        
        /* params: {"norad":                  <int>}     */
        /* result: {"norad":                  <int>,
                    "name":                   <string>,
                    "callsign":               <string>,
                    "beacon":   {"mode":      <string>,
                                 "frequency": <double>,
                                 "drift":     <double>},
                    "sender":   {"mode":      <string>,
                                 "frequency": <double>,
                                 "drift":     <double>},
                    "receiver": {"mode":      <string>,
                                 "frequency": <double>,
                                 "drift":     <double>},
                    "time":                   <string>,
                    "tle":      {"name":      <string>,
                                 "one":       <string>,
                                 "two":       <string>}} */
        
        /* params: {"limit":  <int>,
                    "offset": <int>}                     */
        /* result: {"fields": [...]}                     */
        
        Result getField(const Params& args, Params *result);
    
        /* params: {"name":   <string>}                  */
        /* result: {"fields": [...]}                     */
        Result getFieldByName(const Params& args, Params *result);
        
        /* params: {"callsign": <string>}                */
        /* result: {"fields":   [...]}                   */
        Result getFieldByCallsign(const Params& args, Params *result);
        
        /* params: {"name":   <string>}                  */
        /* result: {"norads": [...]}                     */
        Result getNoradByName(const Params& args, Params *result);
        
        /* params: {"callsign": <string>}                */
        /* result: {"norads":   [...]}                   */
        Result getNoradByCallsign(const Params& args, Params *result);
        
    }
};

#endif