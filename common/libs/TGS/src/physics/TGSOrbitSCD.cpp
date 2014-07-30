/*
**      ARTSAT Project
**
**      Original Copyright (C) 2014 - 2014 HORIGUCHI Junshi.
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
**      TGSOrbitSCD.cpp
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

#include "TGSOrbitSCD.h"
#include "tracker.h"

namespace tgs {

/*public */TGSOrbitSCD::TGSOrbitSCD(void) : _tracker(NULL)
{
}

/*public virtual */TGSOrbitSCD::~TGSOrbitSCD(void)
{
    if (_tracker != NULL) {
        delete _tracker;
    }
    _tracker = NULL;
}

/*public virtual */TGSError TGSOrbitSCD::setOrbitData(SCDRec const& param)
{
    TGSError error(TGSERROR_OK);
    
    return error;
}

/*public virtual */TGSError TGSOrbitSCD::getOrbitData(SCDRec* result) const
{
    TGSError error(TGSERROR_OK);
    
    return error;
}

/*public virtual */TGSError TGSOrbitSCD::getEpochTime(ir::IRXTime* result) const
{
    TGSError error(TGSERROR_OK);
    
    return error;
}

/*public virtual */TGSError TGSOrbitSCD::setTargetTime(ir::IRXTime const& param)
{
    TGSError error(TGSERROR_OK);
    
    if ((error = super::setTargetTime(param)) == TGSERROR_NO_SUPPORT) {
        error = TGSERROR_OK;
    }
    return error;
}

/*public virtual */TGSError TGSOrbitSCD::getTargetTime(ir::IRXTime* result) const
{
    TGSError error(TGSERROR_OK);
    
    if ((error = super::getTargetTime(result)) == TGSERROR_NO_SUPPORT) {
        error = TGSERROR_OK;
    }
    return error;
}

/*public virtual */TGSError TGSOrbitSCD::setObserverPosition(double latitude, double longitude, double altitude)
{
    TGSError error(TGSERROR_OK);
    
    if ((error = super::setObserverPosition(latitude, longitude, altitude)) == TGSERROR_NO_SUPPORT) {
        error = TGSERROR_OK;
    }
    return error;
}

/*public virtual */TGSError TGSOrbitSCD::getObserverPosition(double* latitude, double* longitude, double* altitude) const
{
    TGSError error(TGSERROR_OK);
    
    if ((error = super::getObserverPosition(latitude, longitude, altitude)) == TGSERROR_NO_SUPPORT) {
        error = TGSERROR_OK;
    }
    return error;
}

/*public virtual */TGSError TGSOrbitSCD::getSatellitePosition(double* latitude, double* longitude, double* altitude) const
{
    TGSError error(TGSERROR_OK);
    
    if ((error = super::getSatellitePosition(latitude, longitude, altitude)) == TGSERROR_NO_SUPPORT) {
    }
    return error;
}

/*public virtual */TGSError TGSOrbitSCD::getSatelliteDirection(double* azimuth, double* elevation) const
{
    TGSError error(TGSERROR_OK);
    
    if ((error = super::getSatelliteDirection(azimuth, elevation)) == TGSERROR_NO_SUPPORT) {
    }
    return error;
}

/*public virtual */TGSError TGSOrbitSCD::getSatelliteDistance(double* distance) const
{
    TGSError error(TGSERROR_OK);
    
    if ((error = super::getSatelliteDistance(distance)) == TGSERROR_NO_SUPPORT) {
    }
    return error;
}

/*public virtual */TGSError TGSOrbitSCD::getSatelliteSpeed(double* speed) const
{
    TGSError error(TGSERROR_OK);
    
    if ((error = super::getSatelliteSpeed(speed)) == TGSERROR_NO_SUPPORT) {
    }
    return error;
}

/*public virtual */TGSError TGSOrbitSCD::getDopplerRatio(double* sender, double* receiver) const
{
    TGSError error(TGSERROR_OK);
    
    if ((error = super::getDopplerRatio(sender, receiver)) == TGSERROR_NO_SUPPORT) {
    }
    return error;
}

}// end of namespace
