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
**      Xcode 4.6.2 (Apple LLVM compiler 4.2, LLVM GCC 4.2)
**
**      ASDServerRPCMethods.cpp
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

#include "ASDServerRPCMethods.h"
#include "artsatd.h"

namespace {
    template <class T>
    bool getParam(const std::string& name, T *result, const ASDServerRPC::Params& args) {
        ASDServerRPC::Params::const_iterator it = args.find(name);
        if (it != args.end() && it->second.type() == typeid(T)) {
            *result = boost::get<T>(it->second);
            return true;
        }
        else {
            return false;
        }
    }
}

namespace ASDServerRPC {
    namespace sys {
        Result rpcEcho(const Params& args, Params *result)
        {
            (*result)["args"] = args;
            return RPC_OK;
        }
    }
    
    namespace trans {
        
        Result setActive(const Params& args, Params *result)
        {
            bool flag;
            if (getParam("active", &flag, args)) {
                tgs::TGSError error = artsatd::getInstance().setActive(flag);
                if (error == tgs::TGSERROR_OK) {
                    (*result)["active"] = artsatd::getInstance().getActive();
                    return RPC_OK;
                }
                else {
                    (*result)["message"] = error.print();
                    return RPC_INTERNAL_ERR;
                }
            }
            else {
                return RPC_WRONG_ARGS;
            }
        }
   
        Result getActive(const Params& args, Params *result)
        {
            (*result)["active"] = artsatd::getInstance().getActive();
            return RPC_OK;
        }

        Result setManualSatellite(const Params& args, Params *result)
        {
            bool flag;
            if (getParam("manual", &flag, args)) {
                tgs::TGSError error = artsatd::getInstance().setManualSatellite(flag);
                if (error == tgs::TGSERROR_OK) {
                    (*result)["manual"] = artsatd::getInstance().getManualSatellite();
                    return RPC_OK;
                }
                else {
                    (*result)["message"] = error.print();
                    return RPC_INTERNAL_ERR;
                }
            }
            else {
                return RPC_WRONG_ARGS;
            }
        }

        Result getManualSatellite(const Params& args, Params *result)
        {
            (*result)["manual"] = artsatd::getInstance().getManualSatellite();
            return RPC_OK;
        }

        Result setManualRotator(const Params& args, Params *result)
        {
            bool flag;
            if (getParam("manual", &flag, args)) {
                tgs::TGSError error = artsatd::getInstance().setManualRotator(flag);
                if (error == tgs::TGSERROR_OK) {
                    (*result)["manual"] = artsatd::getInstance().getManualRotator();
                    return RPC_OK;
                }
                else {
                    (*result)["message"] = error.print();
                    return RPC_INTERNAL_ERR;
                }
            }
            else {
                return RPC_WRONG_ARGS;
            }
        }

        Result getManualRotator(const Params& args, Params *result)
        {
            (*result)["manual"] = artsatd::getInstance().getManualRotator();
            return RPC_OK;
        }

        Result setManualTransceiver(const Params& args, Params *result)
        {
            bool flag;
            if (getParam("manual", &flag, args)) {
                tgs::TGSError error = artsatd::getInstance().setManualTransceiver(flag);
                if (error == tgs::TGSERROR_OK) {
                    (*result)["manual"] = artsatd::getInstance().getManualTransceiver();
                    return RPC_OK;
                }
                else {
                    (*result)["message"] = error.print();
                    return RPC_INTERNAL_ERR;
                }
            }
            else {
                return RPC_WRONG_ARGS;
            }
        }

        Result getManualTransceiver(const Params& args, Params *result)
        {
            (*result)["manual"] = artsatd::getInstance().getManualTransceiver();
            return RPC_OK;
        }
        
        Result setManualTNC(const Params& args, Params *result)
        {
            bool flag;
            if (getParam("manual", &flag, args)) {
                tgs::TGSError error = artsatd::getInstance().setManualTNC(flag);
                if (error == tgs::TGSERROR_OK) {
                    (*result)["manual"] = artsatd::getInstance().getManualTNC();
                    return RPC_OK;
                }
                else {
                    (*result)["message"] = error.print();
                    return RPC_INTERNAL_ERR;
                }
            }
            else {
                return RPC_WRONG_ARGS;
            }
        }
        
        Result getManualTNC(const Params& args, Params *result)
        {
            (*result)["manual"] = artsatd::getInstance().getManualTNC();
            return RPC_OK;
        }
        
        Result setMode(const Params& args, Params *result)
        {
            std::string mode;
            if (getParam("mode", &mode, args)) {
                tgs::TGSError error = artsatd::getInstance().setMode(mode);
                if (error == tgs::TGSERROR_OK) {
                    (*result)["mode"] = artsatd::getInstance().getMode();
                    return RPC_OK;
                }
                else {
                    (*result)["message"] = error.print();
                    return RPC_INTERNAL_ERR;
                }
            }
            else {
                return RPC_WRONG_ARGS;
            }
        }
        
        Result getMode(const Params& args, Params *result)
        {
            (*result)["mode"] = artsatd::getInstance().getMode();
            return RPC_OK;
        }
        
        Result setNorad(const Params& args, Params *result)
        {
            double norad = 0;
            if (getParam("norad", &norad, args)) {
                tgs::TGSError error = artsatd::getInstance().setNorad(norad);
                if (error == tgs::TGSERROR_OK) {
                    (*result)["norad"] = static_cast<double>(artsatd::getInstance().getNorad());
                    return RPC_OK;
                }
                else {
                    (*result)["message"] = error.print();
                    return RPC_INTERNAL_ERR;
                }
            }
            else {
                return RPC_WRONG_ARGS;
            }
        }

        Result getNorad(const Params& args, Params *result)
        {
            (*result)["norad"] = static_cast<double>(artsatd::getInstance().getNorad());
            return RPC_OK;
        }
        
        Result getAngleAzimuth(const Params& args, Params *result)
        {
            (*result)["azimuth"] = static_cast<double>(artsatd::getInstance().getAngleAzimuth());
            return RPC_OK;
        }
        
        Result getAngleElevation(const Params& args, Params *result)
        {
            (*result)["elevation"] = static_cast<double>(artsatd::getInstance().getAngleElevation());
            return RPC_OK;
        }
        
        Result getFrequencyBeacon(const Params& args, Params *result)
        {
            (*result)["beacon"] = static_cast<double>(artsatd::getInstance().getFrequencyBeacon());
            return RPC_OK;
        }
        
        Result getFrequencySender(const Params& args, Params *result)
        {
            (*result)["sender"] = static_cast<double>(artsatd::getInstance().getFrequencySender());
            return RPC_OK;
        }
        
        Result getFrequencyReceiver(const Params& args, Params *result)
        {
            (*result)["receiver"] = static_cast<double>(artsatd::getInstance().getFrequencyReceiver());
            return RPC_OK;
        }
        
        Result getError(const Params& args, Params *result)
        {
            (*result)["error"] = artsatd::getInstance().getError();
            return RPC_OK;
        }
    }
};
