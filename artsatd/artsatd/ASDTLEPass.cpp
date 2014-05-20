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
**      ASDTLEPass.cpp
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

#include "ASDTLEPass.h"
#include "ASDRotationSolver.h"

ASDTLEPass::ASDTLEPass() :
    _mel(0.0)
{
    
}

ASDTLEPass::ASDTLEPass(const std::vector<State>& sequence, const ASDRotationSolver& solver) :
    _state_sequence(sequence),
    _mel(0.0)
{
    std::vector<RotatorState> rotator_sequence;
    for (std::vector<State>::iterator it = _state_sequence.begin(); it != _state_sequence.end(); ++it) {
        if (_mel < it->elevation) _mel = it->elevation;
        RotatorState state = {it->time, it->elevation, it->azimuth};
        rotator_sequence.push_back(state);
    }
    
    solver.process(rotator_sequence, &_rotator_state_sequence);
    
    if (!_rotator_state_sequence.empty()) {
        std::vector<RotatorState>::iterator it = _rotator_state_sequence.begin();
        double azimuth = it->azimuth;
        ++it;
        
        for (; it != _rotator_state_sequence.end(); ++it) {
            if (azimuth != it->azimuth) {
                _rotation_start = (it - 1)->time;
                break;
            }
            azimuth = it->azimuth;
        }
    }
}

tgs::TGSError ASDTLEPass::getAOSTime(ir::IRXTime* result) const
{
    if (_state_sequence.empty()) {
        return tgs::TGSERROR_INVALID_STATE;
    }
    else {
        *result = _state_sequence.front().time;
        return tgs::TGSERROR_OK;
    }
}

tgs::TGSError ASDTLEPass::getLOSTime(ir::IRXTime* result) const
{
    if (_state_sequence.empty()) {
        return tgs::TGSERROR_INVALID_STATE;
    }
    else {
        *result = _state_sequence.back().time;
        return tgs::TGSERROR_OK;
    }
}

tgs::TGSError ASDTLEPass::getRotationStartTime(ir::IRXTime* result) const
{
    ir::IRXTime aos;
    if (getAOSTime(&aos) == tgs::TGSERROR_OK && _rotation_start >= aos) {
        *result = _rotation_start;
        return tgs::TGSERROR_OK;
    }
    else {
        return tgs::TGSERROR_INVALID_STATE;
    }
}

tgs::TGSError ASDTLEPass::getMEL(double* result) const
{
    if (_mel > 0.0) {
        *result = _mel;
        return tgs::TGSERROR_OK;
    }
    else {
        return tgs::TGSERROR_INVALID_STATE;
    }
}

tgs::TGSError ASDTLEPass::getSatellitePosition(const ir::IRXTime& time, double* latitude, double* longitude, double* altitude) const
{
    ir::IRXTime aos, los;
    if (getAOSTime(&aos) == tgs::TGSERROR_OK && getLOSTime(&los) == tgs::TGSERROR_OK) {
        if (aos <= time && time <= los) {
            int index = 0;
            for (; time > _state_sequence[index].time; ++index);
            const State& nearest_state = _state_sequence[index];
            *latitude = nearest_state.latitude;
            *longitude = nearest_state.longitude;
            *altitude = nearest_state.altitude;
            return tgs::TGSERROR_OK;
        }
        else {
            return tgs::TGSERROR_NO_RESULT;
        }
    }
    else {
        return tgs::TGSERROR_INVALID_STATE;
    }
}

tgs::TGSError ASDTLEPass::getSatelliteDirection(const ir::IRXTime& time, double* azimuth, double* elevation) const
{
    ir::IRXTime aos, los;
    if (getAOSTime(&aos) == tgs::TGSERROR_OK && getLOSTime(&los) == tgs::TGSERROR_OK) {
        if (aos <= time && time <= los) {
            int index = 0;
            for (; time > _state_sequence[index].time; ++index);
            const State& nearest_state = _state_sequence[index];
            *azimuth = nearest_state.azimuth;
            *elevation = nearest_state.elevation;
            return tgs::TGSERROR_OK;
        }
        else {
            return tgs::TGSERROR_NO_RESULT;
        }
    }
    else {
        return tgs::TGSERROR_INVALID_STATE;
    }
}

tgs::TGSError ASDTLEPass::getRotatorDirection(const ir::IRXTime& time, double* azimuth, double* elevation) const
{
    ir::IRXTime aos, los;
    if (getAOSTime(&aos) == tgs::TGSERROR_OK && getLOSTime(&los) == tgs::TGSERROR_OK) {
        if (aos <= time && time <= los) {
            int index = 0;
            for (; time > _rotator_state_sequence[index].time; ++index);
            const RotatorState& nearest_state = _rotator_state_sequence[index];
            *azimuth = nearest_state.azimuth;
            *elevation = nearest_state.elevation;
            return tgs::TGSERROR_OK;
        }
        else {
            return tgs::TGSERROR_NO_RESULT;
        }
    }
    else {
        return tgs::TGSERROR_INVALID_STATE;
    }
}

tgs::TGSError ASDTLEPass::getDopplerRatio(const ir::IRXTime& time, double* sender, double* receiver) const
{
    ir::IRXTime aos, los;
    if (getAOSTime(&aos) == tgs::TGSERROR_OK && getLOSTime(&los) == tgs::TGSERROR_OK) {
        if (aos <= time && time <= los) {
            int index = 0;
            for (; time > _state_sequence[index].time; ++index);
            const State& nearest_state = _state_sequence[index];
            *sender = nearest_state.sender;
            *receiver = nearest_state.receiver;
            return tgs::TGSERROR_OK;
        }
        else {
            return tgs::TGSERROR_NO_RESULT;
        }
    }
    else {
        return tgs::TGSERROR_INVALID_STATE;
    }
}