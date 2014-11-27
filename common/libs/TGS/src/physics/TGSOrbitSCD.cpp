/*
**      ARTSAT Project
**
**      Original Copyright (C) 2014 - 2014 HORIGUCHI Junshi.
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
    _valid = false;
}

/*public virtual */TGSOrbitSCD::~TGSOrbitSCD(void)
{
    if (_tracker != NULL) {
        delete _tracker;
    }
    _tracker = NULL;
}

/*public virtual */TGSError TGSOrbitSCD::setOrbitData(OrbitData const& param)
{
    SpacecraftTracker::SerializedSCDRec scd;
    TGSError error(TGSERROR_OK);
    
    if ((error = super::setOrbitData(param)) == TGSERROR_NO_SUPPORT) {
        if ((error = cacheTracker()) == TGSERROR_OK) {
            if (param.getType() == OrbitData::TYPE_SCD) {
                strncpy(scd.name, static_cast<SCDRec const&>(param).name, sizeof(scd.name));
                scd.info = static_cast<SCDRec const&>(param).info;
                scd.param = static_cast<SCDRec const&>(param).param;
                if (_tracker->setSpacecraftInfo(scd) == 0) {
                    _data = param;
                    _valid = true;
                }
                else {
                    error = TGSERROR_FAILED;
                }
            }
            else {
                error = TGSERROR_INVALID_FORMAT;
            }
        }
    }
    return error;
}

/*public virtual */TGSError TGSOrbitSCD::getOrbitData(OrbitData* result) const
{
    TGSError error(TGSERROR_OK);
    
    if ((error = super::getOrbitData(result)) == TGSERROR_NO_SUPPORT) {
        if ((error = cacheTracker()) == TGSERROR_OK) {
            if (_valid) {
                if (result != NULL) {
                    *result = _data;
                }
            }
            else {
                error = TGSERROR_INVALID_STATE;
            }
        }
    }
    return error;
}

/*public virtual */TGSError TGSOrbitSCD::getID(int* result) const
{
    SpacecraftTracker::SCDRec scd;
    int id;
    TGSError error(TGSERROR_OK);
    
    if ((error = super::getID(result)) == TGSERROR_NO_SUPPORT) {
        if ((error = cacheTracker()) == TGSERROR_OK) {
            _tracker->getSpacecraftInfo(&scd);
            if (sscanf(scd.orbitInfo.id, "SCD%5d", &id) == 1) {
                if (result != NULL) {
                    *result = id + ID_BASE_SCD;
                }
            }
            else {
                error = TGSERROR_INVALID_FORMAT;
            }
        }
    }
    return error;
}

/*public virtual */TGSError TGSOrbitSCD::getEpochTime(double* result) const
{
    double value;
    TGSError error(TGSERROR_OK);
    
    if ((error = super::getEpochTime(result)) == TGSERROR_NO_SUPPORT) {
        if ((error = cacheTracker()) == TGSERROR_OK) {
            if (result != NULL) {
                _tracker->getEpochTime(&value);
                tf::convertMjdToJd(&value, value);
                *result = value;
            }
        }
    }
    return error;
}

/*public virtual */TGSError TGSOrbitSCD::getEpochTime(ir::IRXTime* result) const
{
    double value;
    TGSError error(TGSERROR_OK);
    
    if ((error = super::getEpochTime(result)) == TGSERROR_NO_SUPPORT) {
        if ((error = cacheTracker()) == TGSERROR_OK) {
            if (result != NULL) {
                _tracker->getEpochTime(&value);
                tf::convertMjdToUnixtime(&value, value);
                result->set(value);
            }
        }
    }
    return error;
}

/*public virtual */TGSError TGSOrbitSCD::setTargetTime(ir::IRXTime const& param)
{
    TGSError error(TGSERROR_OK);
    
    if ((error = super::setTargetTime(param)) == TGSERROR_NO_SUPPORT) {
        if ((error = cacheTracker()) == TGSERROR_OK) {
            if (_tracker->setTargetTime(param.asTime_t()) != 0) {
                error = TGSERROR_FAILED;
            }
        }
    }
    return error;
}

/*public virtual */TGSError TGSOrbitSCD::getTargetTime(ir::IRXTime* result) const
{
    double value;
    TGSError error(TGSERROR_OK);
    
    if ((error = super::getTargetTime(result)) == TGSERROR_NO_SUPPORT) {
        if ((error = cacheTracker()) == TGSERROR_OK) {
            if (result != NULL) {
                _tracker->getTargetTime(&value);
                result->set(value);
            }
        }
    }
    return error;
}

/*public virtual */TGSError TGSOrbitSCD::setObserverPosition(double latitude, double longitude, double altitude)
{
    TGSError error(TGSERROR_OK);
    
    if ((error = super::setObserverPosition(latitude, longitude, altitude)) == TGSERROR_NO_SUPPORT) {
        if ((error = cacheTracker()) == TGSERROR_OK) {
            _tracker->setObserverGeoCoord(degToRad(fmod(latitude, 360.0)), degToRad(fmod(longitude, 360.0)), altitude * 1000.0);
        }
    }
    return error;
}

/*public virtual */TGSError TGSOrbitSCD::getObserverPosition(double* latitude, double* longitude, double* altitude) const
{
    double value[3];
    TGSError error(TGSERROR_OK);
    
    if ((error = super::getObserverPosition(latitude, longitude, altitude)) == TGSERROR_NO_SUPPORT) {
        if ((error = cacheTracker()) == TGSERROR_OK) {
            _tracker->getObserverGeoCoord(&value[0], &value[1], &value[2]);
            if (latitude != NULL) {
                *latitude = fmod(radToDeg(value[0]), 360.0);
            }
            if (longitude != NULL) {
                if ((value[1] = fmod(radToDeg(value[1]), 360.0)) < -180.0) {
                    value[1] += 360.0;
                }
                else if (value[1] >= 180.0) {
                    value[1] -= 360.0;
                }
                *longitude = value[1];
            }
            if (altitude != NULL) {
                *altitude = value[2] / 1000.0;
            }
        }
    }
    return error;
}

/*public virtual */TGSError TGSOrbitSCD::getSpacecraftPosition(double* latitude, double* longitude, double* altitude) const
{
    double value[3];
    TGSError error(TGSERROR_OK);
    
    if ((error = super::getSpacecraftPosition(latitude, longitude, altitude)) == TGSERROR_NO_SUPPORT) {
        if ((error = cacheTracker()) == TGSERROR_OK) {
            _tracker->getSpacecraftGeoCoord(&value[0], &value[1], &value[2]);
            if (latitude != NULL) {
                *latitude = fmod(radToDeg(value[0]), 360.0);
            }
            if (longitude != NULL) {
                if ((value[1] = fmod(radToDeg(value[1]), 360.0)) < -180.0) {
                    value[1] += 360.0;
                }
                else if (value[1] >= 180.0) {
                    value[1] -= 360.0;
                }
                *longitude = value[1];
            }
            if (altitude != NULL) {
                *altitude = value[2] / 1000.0;
            }
        }
    }
    return error;
}

/*public virtual */TGSError TGSOrbitSCD::getSpacecraftDirection(double* azimuth, double* elevation) const
{
    double value[2];
    TGSError error(TGSERROR_OK);
    
    if ((error = super::getSpacecraftDirection(azimuth, elevation)) == TGSERROR_NO_SUPPORT) {
        if ((error = cacheTracker()) == TGSERROR_OK) {
            _tracker->getSpacecraftDirection(&value[0], &value[1]);
            if (azimuth != NULL) {
                *azimuth = radToDeg(value[1]);
            }
            if (elevation != NULL) {
                *elevation = radToDeg(value[0]);
            }
        }
    }
    return error;
}

/*public virtual */TGSError TGSOrbitSCD::getSpacecraftDistance(double* distance) const
{
    double value;
    TGSError error(TGSERROR_OK);
    
    if ((error = super::getSpacecraftDistance(distance)) == TGSERROR_NO_SUPPORT) {
        if ((error = cacheTracker()) == TGSERROR_OK) {
            if (distance != NULL) {
                // TODO: need to change to observer centered value
                _tracker->getDistanceEarthCentered(&value);
                *distance = value / 1000.0;
            }
        }
    }
    return error;
}

/*public virtual */TGSError TGSOrbitSCD::getSpacecraftSpeed(double* speed) const
{
    Vector3d value[2];
    TGSError error(TGSERROR_OK);
    
    if ((error = super::getSpacecraftSpeed(speed)) == TGSERROR_NO_SUPPORT) {
        if ((error = cacheTracker()) == TGSERROR_OK) {
            if (speed != NULL) {
                _tracker->getSpacecraftState(&value[0], &value[1]);
                *speed = value[1].norm() / 1000.0;
            }
        }
    }
    return error;
}

/*public virtual */TGSError TGSOrbitSCD::getDopplerRatio(double* sender, double* receiver) const
{
    double value[2];
    TGSError error(TGSERROR_OK);
    
    if ((error = super::getDopplerRatio(sender, receiver)) == TGSERROR_NO_SUPPORT) {
        if ((error = cacheTracker()) == TGSERROR_OK) {
            _tracker->getDopplerRatio(&value[0], &value[1]);
            if (sender != NULL) {
                *sender = value[1];
            }
            if (receiver != NULL) {
                *receiver = value[0];
            }
        }
    }
    return error;
}

/*private */TGSError TGSOrbitSCD::cacheTracker(void) const
{
    TGSError error(TGSERROR_OK);
    
    if (_tracker == NULL) {
        if ((_tracker = new(std::nothrow) SpacecraftTracker) == NULL) {
            error = TGSERROR_NO_MEMORY;
        }
    }
    return error;
}

}// end of namespace
