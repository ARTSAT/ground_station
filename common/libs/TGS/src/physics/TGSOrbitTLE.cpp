/*
**      ARTSAT Project
**
**      Original Copyright (C) 2013 - 2014 HORIGUCHI Junshi.
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
**      TGSOrbitTLE.cpp
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

#include "TGSOrbitTLE.h"
#include <map>
using namespace std;
#include "cOrbit.h"
#include "cSite.h"

namespace tgs {

#define C_km_sec            (299792.458)

/*public */TGSOrbitTLE::TGSOrbitTLE(void) : _orbit(NULL), _julian(NULL), _site(NULL), _seci(NULL), _oeci(NULL)
{
    _sflag = false;
    _oflag = false;
}

/*public virtual */TGSOrbitTLE::~TGSOrbitTLE(void)
{
    if (_oeci != NULL) {
        delete _oeci;
    }
    _oeci = NULL;
    if (_seci != NULL) {
        delete _seci;
    }
    _seci = NULL;
    if (_site != NULL) {
        delete _site;
    }
    _site = NULL;
    if (_julian != NULL) {
        delete _julian;
    }
    _julian = NULL;
    if (_orbit != NULL) {
        delete _orbit;
    }
    _orbit = NULL;
}

/*public virtual */TGSError TGSOrbitTLE::setOrbitData(TLERec const& param)
{
    std::string name;
    std::string one;
    std::string two;
    TGSError error(TGSERROR_OK);
    
    name = param.name;
    if (Zeptomoby::OrbitTools::cTle::IsValidLine(name, Zeptomoby::OrbitTools::cTle::LINE_ZERO)) {
        one = param.one;
        if (Zeptomoby::OrbitTools::cTle::IsValidLine(one, Zeptomoby::OrbitTools::cTle::LINE_ONE)) {
            two = param.two;
            if (Zeptomoby::OrbitTools::cTle::IsValidLine(two, Zeptomoby::OrbitTools::cTle::LINE_TWO)) {
                Zeptomoby::OrbitTools::cTle tle(name, one, two);
                Zeptomoby::OrbitTools::cOrbit orbit(tle);
                if (_orbit != NULL) {
                    *_orbit = orbit;
                }
                else if ((_orbit = new(std::nothrow) Zeptomoby::OrbitTools::cOrbit(orbit)) == NULL) {
                    error = TGSERROR_NO_MEMORY;
                }
                if (error == TGSERROR_OK) {
                    _tle = param;
                    _sflag = false;
                    _oflag = false;
                }
            }
            else {
                error = TGSERROR_INVALID_FORMAT;
            }
        }
        else {
            error = TGSERROR_INVALID_FORMAT;
        }
    }
    else {
        error = TGSERROR_INVALID_FORMAT;
    }
    return error;
}

/*public virtual */TGSError TGSOrbitTLE::getOrbitData(TLERec* result) const
{
    TGSError error(TGSERROR_OK);
    
    if (_orbit != NULL) {
        if (result != NULL) {
            *result = _tle;
        }
    }
    else {
        error = TGSERROR_INVALID_STATE;
    }
    return error;
}

/*public virtual */TGSError TGSOrbitTLE::getEpochTime(ir::IRXTime* result) const
{
    TGSError error(TGSERROR_OK);
    
    if (_orbit != NULL) {
        if (result != NULL) {
            result->set(_orbit->Epoch().ToTime());
        }
    }
    else {
        error = TGSERROR_INVALID_STATE;
    }
    return error;
}

/*public virtual */TGSError TGSOrbitTLE::setTargetTime(ir::IRXTime const& param)
{
    TGSError error(TGSERROR_OK);
    
    if ((error = super::setTargetTime(param)) == TGSERROR_NO_SUPPORT) {
        error = TGSERROR_OK;
        Zeptomoby::OrbitTools::cJulian julian(param.asTime_t());
        if (_julian != NULL) {
            *_julian = julian;
        }
        else if ((_julian = new(std::nothrow) Zeptomoby::OrbitTools::cJulian(julian)) == NULL) {
            error = TGSERROR_NO_MEMORY;
        }
        if (error == TGSERROR_OK) {
            _sflag = false;
            _oflag = false;
        }
    }
    return error;
}

/*public virtual */TGSError TGSOrbitTLE::getTargetTime(ir::IRXTime* result) const
{
    TGSError error(TGSERROR_OK);
    
    if ((error = super::getTargetTime(result)) == TGSERROR_NO_SUPPORT) {
        error = TGSERROR_OK;
        if (_julian != NULL) {
            if (result != NULL) {
                result->set(_julian->ToTime());
            }
        }
        else {
            error = TGSERROR_INVALID_STATE;
        }
    }
    return error;
}

/*public virtual */TGSError TGSOrbitTLE::setObserverPosition(double latitude, double longitude, double altitude)
{
    TGSError error(TGSERROR_OK);
    
    if ((error = super::setObserverPosition(latitude, longitude, altitude)) == TGSERROR_NO_SUPPORT) {
        error = TGSERROR_OK;
        Zeptomoby::OrbitTools::cSite site(latitude, longitude, altitude);
        if (_site != NULL) {
            *_site = site;
        }
        else if ((_site = new(std::nothrow) Zeptomoby::OrbitTools::cSite(site)) == NULL) {
            error = TGSERROR_NO_MEMORY;
        }
        if (error == TGSERROR_OK) {
            _oflag = false;
        }
    }
    return error;
}

/*public virtual */TGSError TGSOrbitTLE::getObserverPosition(double* latitude, double* longitude, double* altitude) const
{
    TGSError error(TGSERROR_OK);
    
    if ((error = super::getObserverPosition(latitude, longitude, altitude)) == TGSERROR_NO_SUPPORT) {
        error = TGSERROR_OK;
        if (_site != NULL) {
            if (latitude != NULL) {
                *latitude = _site->LatitudeDeg();
            }
            if (longitude != NULL) {
                *longitude = _site->LongitudeDeg();
            }
            if (altitude != NULL) {
                *altitude = _site->AltitudeKm();
            }
        }
        else {
            error = TGSERROR_INVALID_STATE;
        }
    }
    return error;
}

/*public virtual */TGSError TGSOrbitTLE::getSatellitePosition(double* latitude, double* longitude, double* altitude) const
{
    TGSError error(TGSERROR_OK);
    
    if ((error = super::getSatellitePosition(latitude, longitude, altitude)) == TGSERROR_NO_SUPPORT) {
        if ((error = cacheSatellite()) == TGSERROR_OK) {
            Zeptomoby::OrbitTools::cGeoTime geo(*_seci);
            if (latitude != NULL) {
                *latitude = geo.LatitudeDeg();
            }
            if (longitude != NULL) {
                *longitude = geo.LongitudeDeg();
            }
            if (altitude != NULL) {
                *altitude = geo.AltitudeKm();
            }
        }
    }
    return error;
}

/*public virtual */TGSError TGSOrbitTLE::getSatelliteDirection(double* azimuth, double* elevation) const
{
    TGSError error(TGSERROR_OK);
    
    if ((error = super::getSatelliteDirection(azimuth, elevation)) == TGSERROR_NO_SUPPORT) {
        if ((error = cacheSatellite()) == TGSERROR_OK) {
            if (_site != NULL) {
                Zeptomoby::OrbitTools::cTopo topo(_site->GetLookAngle(*_seci));
                if (azimuth != NULL) {
                    *azimuth = topo.AzimuthDeg();
                }
                if (elevation != NULL) {
                    *elevation = topo.ElevationDeg();
                }
            }
            else {
                error = TGSERROR_INVALID_STATE;
            }
        }
    }
    return error;
}

/*public virtual */TGSError TGSOrbitTLE::getDopplerRatio(double* sender, double* receiver) const
{
    double factor;
    TGSError error(TGSERROR_OK);
    
    if ((error = super::getDopplerRatio(sender, receiver)) == TGSERROR_NO_SUPPORT) {
        if ((error = cacheSatellite()) == TGSERROR_OK) {
            if ((error = cacheObserver()) == TGSERROR_OK) {
                Zeptomoby::OrbitTools::cVector position(_oeci->Position());
                position.Sub(_seci->Position());
                Zeptomoby::OrbitTools::cVector velocity(_seci->Velocity());
                velocity.Sub(_oeci->Velocity());
                factor = velocity.Magnitude() * cos(velocity.Angle(position));
                if (sender != NULL) {
                    *sender = (C_km_sec - factor) / C_km_sec;
                }
                if (receiver != NULL) {
                    *receiver = C_km_sec / (C_km_sec - factor);
                }
            }
        }
    }
    return error;
}

/*private */TGSError TGSOrbitTLE::cacheSatellite(void) const
{
    TGSError error(TGSERROR_OK);
    
    if (_orbit != NULL && _julian != NULL) {
        if (!_sflag) {
            Zeptomoby::OrbitTools::cEciTime eci(_orbit->GetPosition(_orbit->TPlusEpoch(*_julian) / 60.0));
            if (_seci != NULL) {
                *_seci = eci;
            }
            else if ((_seci = new(std::nothrow) Zeptomoby::OrbitTools::cEciTime(eci)) == NULL) {
                error = TGSERROR_NO_MEMORY;
            }
            if (error == TGSERROR_OK) {
                _sflag = true;
            }
        }
    }
    else {
        error = TGSERROR_INVALID_STATE;
    }
    return error;
}

/*private */TGSError TGSOrbitTLE::cacheObserver(void) const
{
    TGSError error(TGSERROR_OK);
    
    if (_site != NULL && _seci != NULL) {
        if (!_oflag) {
            Zeptomoby::OrbitTools::cEciTime eci(_site->GetPosition(_seci->Date()));
            if (_oeci != NULL) {
                *_oeci = eci;
            }
            else if ((_oeci = new(std::nothrow) Zeptomoby::OrbitTools::cEciTime(eci)) == NULL) {
                error = TGSERROR_NO_MEMORY;
            }
            if (error == TGSERROR_OK) {
                _oflag = true;
            }
        }
    }
    else {
        error = TGSERROR_INVALID_STATE;
    }
    return error;
}

}// end of namespace
