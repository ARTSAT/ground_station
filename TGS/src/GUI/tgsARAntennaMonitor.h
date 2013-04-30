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
                float                   _radius;
                ofPoint                 _offset;
                float                   _slope;
                float                   _direction;
                std::string             _url;
                int                     _reload;
                tgs::TGSError           _error;
                ofImage                 _webcam;
                ofImage                 _pixel;
                ofImage                 _image;
                ModeEnum                _mode;
                float                   _azimuth;
                float                   _elevation;
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
                void                    setAzimuth                      (float param);
                float                   getAzimuth                      (void) const;
                void                    setElevation                    (float param);
                float                   getElevation                    (void) const;
                tgs::TGSError           getCaptureError                 (void) const;
                bool                    isValid                         (void) const;
                tgs::TGSError           open                            (std::string const& url, int reload, std::string const& font, int size, float radius, ofPoint const& offset, float slope, float direction);
                void                    close                           (void);
                void                    update                          (void);
                void                    draw                            (void);
    private:
        virtual void                    threadedFunction                (void);
                void                    drawDummy                       (ofPoint const& offset, float rotate, ofRectangle const& rect, int alpha);
                void                    drawImage                       (tgs::TGSError error, ofPoint const& offset, float rotate, ofRectangle const& rect, int alpha);
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

/*public */inline void tgsARAnttenaMonitor::setAzimuth(float param)
{
    _azimuth = param;
    return;
}

/*public */inline float tgsARAnttenaMonitor::getAzimuth(void) const
{
    return _azimuth;
}

/*public */inline void tgsARAnttenaMonitor::setElevation(float param)
{
    _elevation = param;
    return;
}

/*public */inline float tgsARAnttenaMonitor::getElevation(void) const
{
    return _elevation;
}

/*public */inline bool tgsARAnttenaMonitor::isValid(void) const
{
    return _state;
}

#endif
