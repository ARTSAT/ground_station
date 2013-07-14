/*
**      ARTSAT Project
**
**      Original Copyright (C) 2013 - 2013 HORIGUCHI Junshi.
**                                          http://iridium.jp/
**                                          zap00365@nifty.com
**      Portions Copyright (C) <year> <author>
**                                          <website>
**                                          <e-mail>
**      Version     openFrameworks
**      Website     http://artsat.jp/
**      E-mail      info@artsat.jp
**
**      This source code is for Xcode.
**      Xcode 4.6.2 (Apple LLVM compiler 4.2, LLVM GCC 4.2)
**
**      tgsARAntennaMonitor.h
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

#ifndef __TGS_ARANTENNAMONITOR_H
#define __TGS_ARANTENNAMONITOR_H

#include "ofMain.h"
#include "ofxTrueTypeFontUC.h"
#include "TGSType.h"

class tgsARAnttenaMonitor : public ofThread {
    public:
        enum ModeEnum {
            MODE_WEBCAM,
            MODE_AR_MONITOR,
            MODE_AR_COMPOSITION,
            MODE_3D_COMPOSITION
        };
    
    private:
                bool                    _state;
                ofCamera                _camera;
                ofRectangle             _viewport;
                ofQuaternion            _rotation;
                ofxTrueTypeFontUC       _font;
                double                  _radius;
                ofPoint                 _offset;
                double                  _slope;
                double                  _direction;
                std::string             _url;
                int                     _reload;
                tgs::TGSError           _error;
                ofImage                 _webcam;
                ofImage                 _pixel;
                ofImage                 _image;
                ModeEnum                _mode;
                double                  _azimuth;
                double                  _elevation;
        mutable ofMutex                 _mutex;
    
    public:
        explicit                        tgsARAnttenaMonitor             (void);
                                        ~tgsARAnttenaMonitor            (void);
                void                    setViewport                     (ofRectangle const& param);
                ofRectangle const&      getViewport                     (void) const;
                void                    setRotation                     (ofQuaternion const& param);
                ofQuaternion const&     getRotation                     (void) const;
                void                    setMode                         (ModeEnum param);
                ModeEnum                getMode                         (void) const;
                void                    setAzimuth                      (double param);
                double                  getAzimuth                      (void) const;
                void                    setElevation                    (double param);
                double                  getElevation                    (void) const;
                tgs::TGSError           getCaptureError                 (void) const;
                bool                    isValid                         (void) const;
                tgs::TGSError           open                            (std::string const& url, int reload, std::string const& font, int size, double radius, ofPoint const& offset, double slope, double direction);
                void                    close                           (void);
                void                    update                          (void);
                void                    draw                            (void);
    private:
        virtual void                    threadedFunction                (void);
                void                    drawDummy                       (ofPoint const& offset, double rotate, ofRectangle const& rect, int alpha);
                void                    drawImage                       (tgs::TGSError error, ofPoint const& offset, double rotate, ofRectangle const& rect, int alpha);
                void                    drawGuide                       (tgs::TGSError error, ofRectangle const& viewport);
                void                    drawCompass                     (void);
                void                    drawGrid                        (void);
};

/*public */inline void tgsARAnttenaMonitor::setViewport(ofRectangle const& param)
{
    _viewport = param;
    return;
}

/*public */inline ofRectangle const& tgsARAnttenaMonitor::getViewport(void) const
{
    return _viewport;
}

/*public */inline void tgsARAnttenaMonitor::setRotation(ofQuaternion const& param)
{
    _rotation = param;
    return;
}

/*public */inline ofQuaternion const& tgsARAnttenaMonitor::getRotation(void) const
{
    return _rotation;
}

/*public */inline void tgsARAnttenaMonitor::setMode(ModeEnum param)
{
    _mode = param;
    return;
}

/*public */inline tgsARAnttenaMonitor::ModeEnum tgsARAnttenaMonitor::getMode(void) const
{
    return _mode;
}

/*public */inline void tgsARAnttenaMonitor::setAzimuth(double param)
{
    _azimuth = param;
    return;
}

/*public */inline double tgsARAnttenaMonitor::getAzimuth(void) const
{
    return _azimuth;
}

/*public */inline void tgsARAnttenaMonitor::setElevation(double param)
{
    _elevation = param;
    return;
}

/*public */inline double tgsARAnttenaMonitor::getElevation(void) const
{
    return _elevation;
}

/*public */inline bool tgsARAnttenaMonitor::isValid(void) const
{
    return _state;
}

#endif
