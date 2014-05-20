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
**      TGSOrbitTLE.h
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

#ifndef __TGS_ORBITTLE_H
#define __TGS_ORBITTLE_H

#include "TGSType.h"
#include "TGSOrbitInterface.h"

namespace Zeptomoby { namespace OrbitTools {
class cOrbit;
class cJulian;
class cSite;
class cEciTime;
}}
namespace tgs {

class TGSOrbitTLE : public TGSOrbitInterface {
    public:
        typedef TGSOrbitTLE                 self;
        typedef TGSOrbitInterface           super;
    
    private:
                Zeptomoby::OrbitTools::cOrbit*
                                            _orbit;
                TLERec                      _tle;
                Zeptomoby::OrbitTools::cJulian*
                                            _julian;
                Zeptomoby::OrbitTools::cSite*
                                            _site;
        mutable Zeptomoby::OrbitTools::cEciTime*
                                            _seci;
        mutable bool                        _sflag;
        mutable Zeptomoby::OrbitTools::cEciTime*
                                            _oeci;
        mutable bool                        _oflag;
    
    public:
        explicit                            TGSOrbitTLE                     (void);
        virtual                             ~TGSOrbitTLE                    (void);
        virtual TGSError                    setOrbitData                    (TLERec const& param);
        virtual TGSError                    getOrbitData                    (TLERec* result) const;
        virtual TGSError                    getEpochTime                    (ir::IRXTime* result) const;
        virtual TGSError                    setTargetTime                   (ir::IRXTime const& param);
        virtual TGSError                    getTargetTime                   (ir::IRXTime* result) const;
        virtual TGSError                    setObserverPosition             (double latitude, double longitude, double altitude);
        virtual TGSError                    getObserverPosition             (double* latitude, double* longitude, double* altitude) const;
        virtual TGSError                    getSatellitePosition            (double* latitude, double* longitude, double* altitude) const;
        virtual TGSError                    getSatelliteDirection           (double* azimuth, double* elevation) const;
        virtual TGSError                    getDopplerRatio                 (double* sender, double* receiver) const;
    private:
                TGSError                    cacheSatellite                  (void) const;
                TGSError                    cacheObserver                   (void) const;
    private:
                                            TGSOrbitTLE                     (TGSOrbitTLE const&);
                TGSOrbitTLE&                operator=                       (TGSOrbitTLE const&);
};

}// end of namespace

#endif