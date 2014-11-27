/*
**      ARTSAT Project
**
**      Original Copyright (C) 2014 - 2014 Ron Hashimoto.
**                                          http://h2so5.net/
**                                          mail@h2so5.net
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
**      ASDTLEPassFactory.cpp
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

#include "ASDTLEPassFactory.h"
#include "ASDTLEPass.h"

ASDTLEPassFactory::ASDTLEPassFactory()
{
    
}

tgs::TGSError ASDTLEPassFactory::getNearestPass(ASDTLEPass* pass, ir::IRXTime const& param, const ASDRotationSolver& solver)
{
    ir::IRXTime cached_los;
    if (_cached_pass && _cached_pass->getLOSTime(&cached_los) == tgs::TGSERROR_OK) {
        if (param > _last_searched_time && param <= cached_los) {
            ir::IRXTime prev_los;
            pass->getLOSTime(&prev_los);
            if (prev_los != cached_los) {
                *pass = *_cached_pass;
            }
            return tgs::TGSERROR_OK;
        }
    }
    
    double azimuth, elevation;
    if (_orbit.setTargetTime(param) == tgs::TGSERROR_OK &&
        _orbit.getSatelliteDirection(&azimuth, &elevation) == tgs::TGSERROR_OK) {
        ir::IRXTime aos = param;

        if (elevation > 0.0) {
            while (elevation >= 0.0) {
                aos.subSecond(1);
                if (_orbit.setTargetTime(aos) != tgs::TGSERROR_OK ||
                    _orbit.getSatelliteDirection(&azimuth, &elevation) != tgs::TGSERROR_OK) {
                    return tgs::TGSERROR_INVALID_STATE;
                }
            }
        }
        
        double max_elevation = -FLT_MAX;
        while (elevation < 0.0) {
            aos.addSecond(1);
            if (_orbit.setTargetTime(aos) != tgs::TGSERROR_OK ||
                _orbit.getSatelliteDirection(&azimuth, &elevation) != tgs::TGSERROR_OK) {
                return tgs::TGSERROR_INVALID_STATE;
            }
            max_elevation = std::max(max_elevation, elevation);
        }
        
        _last_searched_time = param;
        
        std::vector<ASDTLEPass::State> state_sequence;
        
        ASDTLEPass::State state;
        state.elevation = elevation;
        ir::IRXTime t = aos;
        
        while (state.elevation > 0.0) {
            
            if (_orbit.setTargetTime(t.addSecond(1)) != tgs::TGSERROR_OK)
                return tgs::TGSERROR_INVALID_STATE;
            if (_orbit.getSatelliteDirection(&state.azimuth, &state.elevation) != tgs::TGSERROR_OK)
                return tgs::TGSERROR_INVALID_STATE;
            if (_orbit.getSatellitePosition(&state.latitude, &state.longitude, &state.altitude) != tgs::TGSERROR_OK)
                return tgs::TGSERROR_INVALID_STATE;
            if (_orbit.getDopplerRatio(&state.sender, &state.receiver) != tgs::TGSERROR_OK)
                return tgs::TGSERROR_INVALID_STATE;
            
            // low elevation mode (fixed elevation)
            //if (max_elevation < 15.0) {
            //    state.elevation = max_elevation / 2.0;
            //}
            
            state.time = t;
            state_sequence.push_back(state);
        }
        
        _cached_pass.reset(new(std::nothrow) ASDTLEPass(state_sequence, solver));
        *pass = *_cached_pass;
        
        return tgs::TGSERROR_OK;
    }
    else {
        return tgs::TGSERROR_INVALID_STATE;
    }
}

tgs::TGSError ASDTLEPassFactory::setOrbitData(tgs::TLERec const& param)
{
    _cached_pass.reset();
    return _orbit.setOrbitData(param);
}

tgs::TGSError ASDTLEPassFactory::getOrbitData(tgs::TLERec* result) const
{
    return _orbit.getOrbitData(result);
}

tgs::TGSError ASDTLEPassFactory::setObserverPosition(double latitude, double longitude, double altitude)
{
    _cached_pass.reset();
    return _orbit.setObserverPosition(latitude, longitude, altitude);
}

tgs::TGSError ASDTLEPassFactory::getObserverPosition(double* latitude, double* longitude, double* altitude) const
{
    return _orbit.getObserverPosition(latitude, longitude, altitude);
}
