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
**      TGSTransceiverIC9100.cpp
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

#include "TGSTransceiverIC9100.h"

namespace tgs {

#define DEFAULT_ID          (0x7C)

/*public */TGSTransceiverIC9100::TGSTransceiverIC9100(void) : Transceiver(DEFAULT_ID)
{
}

/*public virtual */TGSTransceiverIC9100::~TGSTransceiverIC9100(void)
{
    close();
}

/*public virtual */TGSError TGSTransceiverIC9100::setFrequencySender(int param)
{
    bool flag;
    TGSError error(TGSERROR_OK);
    
    if ((error = super::setFrequencySender(param)) == TGSERROR_NO_SUPPORT) {
        if ((error = Transceiver::getSubBandMode(&flag)) == TGSERROR_OK) {
            if (flag) {
                if ((error = Transceiver::selectSubBand()) == TGSERROR_OK) {
                    error = Transceiver::setOperationFrequency(param);
                }
            }
            else {
                error = TGSERROR_NO_RESULT;
            }
        }
    }
    return error;
}

/*public virtual */TGSError TGSTransceiverIC9100::getFrequencySender(int* result)
{
    bool flag;
    TGSError error(TGSERROR_OK);
    
    if ((error = super::getFrequencySender(result)) == TGSERROR_NO_SUPPORT) {
        if ((error = Transceiver::getSubBandMode(&flag)) == TGSERROR_OK) {
            if (flag) {
                if ((error = Transceiver::selectSubBand()) == TGSERROR_OK) {
                    error = Transceiver::getOperationFrequency(result);
                }
            }
            else {
                error = TGSERROR_NO_RESULT;
            }
        }
    }
    return error;
}

/*public virtual */TGSError TGSTransceiverIC9100::setFrequencyReceiver(int param)
{
    TGSError error(TGSERROR_OK);
    
    if ((error = super::setFrequencyReceiver(param)) == TGSERROR_NO_SUPPORT) {
        if ((error = Transceiver::selectMainBand()) == TGSERROR_OK) {
            error = Transceiver::setOperationFrequency(param);
        }
    }
    return error;
}

/*public virtual */TGSError TGSTransceiverIC9100::getFrequencyReceiver(int* result)
{
    TGSError error(TGSERROR_OK);
    
    if ((error = super::getFrequencyReceiver(result)) == TGSERROR_NO_SUPPORT) {
        if ((error = Transceiver::selectMainBand()) == TGSERROR_OK) {
            error = Transceiver::getOperationFrequency(result);
        }
    }
    return error;
}

/*public virtual */bool TGSTransceiverIC9100::isValid(void) const
{
    return super::isValid() || Transceiver::isConnected();
}

/*public virtual */TGSError TGSTransceiverIC9100::open(std::string const& port, int baud, bool verbose)
{
    return TGSERROR_NO_SUPPORT;
}

/*public virtual */void TGSTransceiverIC9100::close(void)
{
    return;
}

/*public virtual */TGSError TGSTransceiverIC9100::selectModeBeacon(void)
{
    TGSError error(TGSERROR_OK);
    
    if ((error = super::selectModeBeacon()) == TGSERROR_NO_SUPPORT) {
        if ((error = Transceiver::setSatelliteMode(false)) == TGSERROR_OK) {
            if ((error = Transceiver::setSubBandMode(false)) == TGSERROR_OK) {
                if ((error = Transceiver::selectMainBand()) == TGSERROR_OK) {
                    if ((error = Transceiver::selectVFOA()) == TGSERROR_OK) {
                        error = Transceiver::setOperationMode(TGSTransceiverCIV::OPERATIONMODE_CW, TGSTransceiverCIV::FILTER_FIL2);
                    }
                }
            }
        }
    }
    return error;
}

/*public virtual */TGSError TGSTransceiverIC9100::selectModeCommunication(void)
{
    TGSError error(TGSERROR_OK);
    
    if ((error = super::selectModeCommunication()) == TGSERROR_NO_SUPPORT) {
        if ((error = Transceiver::setSatelliteMode(true)) == TGSERROR_OK) {
            if ((error = Transceiver::setSubBandMode(true)) == TGSERROR_OK) {
                if ((error = Transceiver::selectSubBand()) == TGSERROR_OK) {
                    if ((error = Transceiver::setOperationMode(TGSTransceiverCIV::OPERATIONMODE_FM, TGSTransceiverCIV::FILTER_FIL1)) == TGSERROR_OK) {
                        if ((error = Transceiver::setDataMode(true)) == TGSERROR_OK) {
                            if ((error = Transceiver::selectMainBand()) == TGSERROR_OK) {
                                if ((error = Transceiver::setOperationMode(TGSTransceiverCIV::OPERATIONMODE_FM, TGSTransceiverCIV::FILTER_FIL1)) == TGSERROR_OK) {
                                    error = Transceiver::setDataMode(true);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return error;
}

}// end of namespace
