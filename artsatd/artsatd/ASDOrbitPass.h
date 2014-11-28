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
**      ASDOrbitPass.h
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

#ifndef __ASD_ORBITPASS_H
#define __ASD_ORBITPASS_H

#include "TGSType.h"
#include "TGSOrbitInterface.h"

class ASDRotationSolver;

class ASDOrbitPass {
public:
    struct State {
        ir::IRXTime time;
        double latitude;
        double longitude;
        double altitude;
        double elevation;
        double azimuth;
        double sender;
        double receiver;
    };
    
    struct RotatorState {
        ir::IRXTime time;
        double elevation;
        double azimuth;
    };
    
public:
    ASDOrbitPass();
    ASDOrbitPass(const std::vector<State>& sequence, const ASDRotationSolver& solver);
    
    tgs::TGSError getAOSTime(ir::IRXTime* result) const;
    tgs::TGSError getLOSTime(ir::IRXTime* result) const;
    tgs::TGSError getRotationStartTime(ir::IRXTime* result) const;
    tgs::TGSError getMEL(double* result) const;
    tgs::TGSError getSpacecraftPosition(const ir::IRXTime& time, double* latitude, double* longitude, double* altitude) const;
    tgs::TGSError getSpacecraftDirection(const ir::IRXTime& time, double* azimuth, double* elevation) const;
    tgs::TGSError getRotatorDirection(const ir::IRXTime& time, double* azimuth, double* elevation) const;
    tgs::TGSError getDopplerRatio(const ir::IRXTime& time, double* sender, double* receiver) const;
    
private:
    std::vector<State> _state_sequence;
    std::vector<RotatorState> _rotator_state_sequence;
    double _mel;
    ir::IRXTime _rotation_start;
};

#endif
