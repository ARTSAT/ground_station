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
**      ASDRotationSolver.cpp
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

#include "ASDRotationSolver.h"

ASDRotationSolver::ASDRotationSolver() :
    _current_azimuth(0.0),
    _target_azimuth(0.0),
    _max_elevation(0.0)
{
}

tgs::TGSError ASDRotationSolver::setRotaterSpec(const RotatorSpec& spec)
{
    _spec = spec;
    return tgs::TGSERROR_OK;
}

tgs::TGSError ASDRotationSolver::getRotaterSpec(RotatorSpec* spec) const
{
    *spec = _spec;
    return tgs::TGSERROR_OK;
}

double ASDRotationSolver::process(const std::vector<ASDOrbitPass::RotatorState>& input, std::vector<ASDOrbitPass::RotatorState> *bestseq) const
{
    bestseq->clear();
    if (input.empty()) return 0;
    
    std::vector<int> flip_points;
    flip_points.push_back(0);
    
    _max_elevation = 0.0;
    for (int i = 1; i < input.size(); ++i) {
        if (std::abs(input[i].azimuth - input[i-1].azimuth) > 90.0) {
            flip_points.push_back(i);
        }
        _max_elevation = std::max(_max_elevation, input[i].elevation);
    }
    
    int max_elevation_intg = -1;
    std::vector<ASDOrbitPass::RotatorState> result;
    for (std::vector<int>::const_iterator it = flip_points.begin(); it != flip_points.end(); ++it) {
        int active_time = processWithOffset(input, &result, *it);
        if (active_time > max_elevation_intg) {
            max_elevation_intg = 0;
            max_elevation_intg = active_time;
            bestseq->assign(result.begin(), result.end());
        }
    }
    
    return max_elevation_intg;
}

double ASDRotationSolver::processWithOffset(const std::vector<ASDOrbitPass::RotatorState>& input, std::vector<ASDOrbitPass::RotatorState> *bestseq, int offset) const
{
    bestseq->clear();
    if (input.empty()) return 0;
    
    double elevation_intg = 0.0;
    
    _current_azimuth = 0.0;
    _target_azimuth = 0.0;
    
    std::vector<ASDOrbitPass::RotatorState>::const_iterator it = input.begin();
    
    if (offset > 0) {
        it += offset;
        double first_azimuth = it->azimuth;
        if (first_azimuth > 180.0) {
            _current_azimuth = 360.0;
            _target_azimuth = 360.0;
        }
        for (int i = 0; i < offset; ++i) {
            ASDOrbitPass::RotatorState d = {input[i].time, input[i].elevation, _target_azimuth};
            bestseq->push_back(d);
        }
    }
    
    time_t prev_time = it->time.asTime_t();
    _target_azimuth = it->azimuth;
    
    for (; it != input.end(); ++it) {
        
        double physical_target_azimuth = _target_azimuth;
        
        // simulate rotation
        for (int i = 0; i < it->time.asTime_t() - prev_time; ++i) {
            double azimuth_diff = physical_target_azimuth - _current_azimuth;
            if (azimuth_diff != 0.0) {
                if (std::abs(azimuth_diff) > _spec.degPerSec) {
                    if (azimuth_diff > 0) {
                        _current_azimuth += _spec.degPerSec;
                    }
                    else {
                        _current_azimuth -= _spec.degPerSec;
                    }
                } else {
                    _current_azimuth += azimuth_diff;
                }
            }
            if (_current_azimuth == _target_azimuth) elevation_intg += it->elevation;
        }
        
        prev_time = it->time.asTime_t();
        
        // rotate over 360deg
        double alt_azimuth = it->azimuth + 360.0;
        if (alt_azimuth <= _spec.maxAzimuth && std::abs(_current_azimuth - alt_azimuth) < std::abs(_current_azimuth - it->azimuth)) {
            _target_azimuth = alt_azimuth;
        }
        else {
            _target_azimuth = it->azimuth;
        }
        
        ASDOrbitPass::RotatorState d = {it->time, it->elevation, _target_azimuth};
        bestseq->push_back(d);
    }
    
    return elevation_intg;
}
