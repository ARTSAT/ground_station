#include "tgsARAntennaMonitor.h"

#define IMAGE_WIDTH     (480)
#define IMAGE_HEIGHT    (360)

/*public */tgsARAnttenaMonitor::tgsARAnttenaMonitor(void) : _state(false)
{
    _camera.setPosition(0, 0, 350);
    _camera.lookAt(ofVec3f(0, 0, 0));
    _camera.setFov(45);
    _viewport.set(0, 0, -1, -1);
    _mode = MODE_AR_COMPOSITION;
    _azimuth = 0.0f;
    _elevation = 0.0f;
}

/*public */tgsARAnttenaMonitor::~tgsARAnttenaMonitor(void)
{
    close();
}

/*public */tgs::TGSError tgsARAnttenaMonitor::getCaptureError(void) const
{
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    if (_state) {
        _mutex.lock();
        error = _error;
        _mutex.unlock();
        if (error != tgs::TGSERROR_FAILED) {
            if (const_cast<ofImage*>(&_image)->isAllocated()) {
                error = tgs::TGSERROR_OK;
            }
            else {
                error = tgs::TGSERROR_WAIT_RESULT;
            }
        }
        else {
            error = tgs::TGSERROR_NETWORK_DOWN;
        }
    }
    else {
        error = tgs::TGSERROR_INVALID_STATE;
    }
    return error;
}

/*public */tgs::TGSError tgsARAnttenaMonitor::open(std::string const& url, int reload, std::string const& font, int size, float radius, ofPoint const& offset, float slope, float direction)
{
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    if (!_state) {
        _state = true;
        if (_font.loadFont(font, size, true, true)) {
            _radius = radius;
            _offset = offset;
            _slope = slope;
            _direction = direction;
            _url = url;
            _reload = reload;
            _error = tgs::TGSERROR_WAIT_RESULT;
            startThread();
        }
        else {
            error = tgs::TGSERROR_FAILED;
        }
        if (error != tgs::TGSERROR_OK) {
            close();
        }
    }
    else {
        error = tgs::TGSERROR_INVALID_STATE;
    }
    return error;
}

/*public */void tgsARAnttenaMonitor::close(void)
{
    waitForThread();
    _image.clear();
    _pixel.clear();
    _webcam.clear();
    _state = false;
    return;
}

/*public */void tgsARAnttenaMonitor::update(void)
{
    if (_state) {
        // NOP
    }
    return;
}

/*public */void tgsARAnttenaMonitor::draw(void)
{
    tgs::TGSError error;
    ofRectangle viewport;
    float angle;
    ofVec3f axis;
    ofRectangle rect;
    
    if (_state) {
        _mutex.lock();
        if ((error = _error) == tgs::TGSERROR_OK) {
            _image = _pixel;
            _image.setUseTexture(true);
            _image.reloadTexture();
            _error = tgs::TGSERROR_WAIT_RESULT;
        }
        _mutex.unlock();
        viewport.set((_viewport.width >= 0 && _viewport.height >= 0) ? (_viewport) : (ofGetCurrentViewport()));
        switch (_mode) {
            case MODE_WEBCAM:
                drawImage(error, ofPoint(0, 0), 0, viewport, 255);
                break;
            default:
                drawDummy(ofPoint(0, 0), 0, viewport, 255);
                _rotation.getRotate(angle, axis);
                _camera.begin(viewport);
                {
                    ofPushMatrix();
                    ofRotate(angle, axis.x, axis.y, axis.z);
                    ofRotateY(5);
                    glEnable(GL_DEPTH_TEST);
                    glEnable(GL_BLEND);
                    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                    glDepthFunc(GL_LEQUAL);
                    glClear(GL_DEPTH_BUFFER_BIT);
                    switch (_mode) {
                        case MODE_3D_COMPOSITION:
                            drawGuide(error, viewport);
                            break;
                        default:
                            rect.set(-IMAGE_WIDTH / 2, -IMAGE_HEIGHT / 2, IMAGE_WIDTH, IMAGE_HEIGHT);
                            drawDummy(_offset, 180, rect, 0);
                            glDepthFunc(GL_GREATER);
                            drawGuide(error, viewport);
                            glDepthFunc(GL_LEQUAL);
                            glClear(GL_DEPTH_BUFFER_BIT);
                            drawImage(error, _offset, 180, rect, 191);
                            drawGuide(error, viewport);
                            break;
                    }
                    glDisable(GL_BLEND);
                    glDisable(GL_DEPTH_TEST);
                    ofPopMatrix();
                }
                _camera.end();
                break;
        }
    }
    return;
}

/*private virtual */void tgsARAnttenaMonitor::threadedFunction(void)
{
    while (isThreadRunning()) {
        switch (_mode) {
            case MODE_WEBCAM:
            case MODE_AR_MONITOR:
            case MODE_AR_COMPOSITION:
                _webcam.setUseTexture(false);
                if (_webcam.loadImage(_url)) {
                    _webcam.resize(IMAGE_WIDTH, IMAGE_HEIGHT);
                    _mutex.lock();
                    _pixel = _webcam;
                    _error = tgs::TGSERROR_OK;
                    _mutex.unlock();
                }
                else {
                    _mutex.lock();
                    _error = tgs::TGSERROR_FAILED;
                    _mutex.unlock();
                }
                break;
            default:
                break;
        }
        sleep(_reload);
    }
    return;
}

/*private */void tgsARAnttenaMonitor::drawDummy(ofPoint const& offset, float rotate, ofRectangle const& rect, int alpha)
{
    ofPushMatrix();
    ofTranslate(offset);
    ofRotateX(rotate);
    ofFill();
    ofSetColor(255, 255, 255, alpha);
    ofRect(rect);
    ofPopMatrix();
    return;
}

/*private */void tgsARAnttenaMonitor::drawImage(tgs::TGSError error, ofPoint const& offset, float rotate, ofRectangle const& rect, int alpha)
{
    ofPushMatrix();
    ofTranslate(offset);
    ofRotateX(rotate);
    ofFill();
    ofSetColor(0, 0, 255, alpha);
    if (_image.isAllocated()) {
        if (error != tgs::TGSERROR_FAILED) {
            ofSetColor(255, 255, 255, alpha);
        }
        _image.draw(rect);
    }
    else {
        ofRect(rect);
    }
    ofPopMatrix();
    return;
}

/*private */void tgsARAnttenaMonitor::drawGuide(tgs::TGSError error, ofRectangle const& viewport)
{
    float scale;
    ofPath path;
    int i;
    
    ofPushMatrix();
    ofTranslate(ofPoint(0, 0, 0.1f));
    switch (_mode) {
        case MODE_3D_COMPOSITION:
            ofRotateX(-70);
            break;
        default:
            ofRotateX(-(90 + _slope));
            ofRotateZ(-_direction);
            break;
    }
    drawCompass();
    glDepthMask(GL_FALSE);
    switch (_mode) {
        case MODE_3D_COMPOSITION:
            drawGrid();
            break;
        default:
            break;
    }
    scale = max(viewport.width / 1024.0f, viewport.height / 768.0f);
    path.setArcResolution(96);
    path.setStrokeWidth(2.0f * scale);
    path.setStrokeColor(ofColor(63, 255, 63));
    path.setFillColor(ofColor(63, 255, 63, 63));
    path.clear();
    path.arc(0, 0, _radius, _radius, 0, 360);
    path.moveTo(0, 0, 0);
    path.lineTo(0, 0, _radius);
    path.setFilled(true);
    path.draw();
    {
        ofPushMatrix();
        ofRotateX(90);
        path.clear();
        path.arc(0, 0, _radius, _radius, 0, 90);
        path.moveTo(0, 0, 0);
        path.lineTo(_radius, 0, 0);
        path.setFilled(false);
        for (i = 0; i < 3; ++i) {
            path.draw();
            ofRotateY(-90);
        }
        path.setStrokeColor(ofColor(255, 63, 63));
        path.draw();
        switch (_mode) {
            case MODE_AR_COMPOSITION:
            case MODE_3D_COMPOSITION:
                switch (_mode) {
                    case MODE_3D_COMPOSITION:
                        path.setStrokeColor(ofColor(63, 63, 255));
                        path.setFillColor(ofColor(63, 63, 255, 95));
                        break;
                    default:
                        path.setStrokeColor(ofColor(255, 255, 63));
                        path.setFillColor(ofColor(255, 255, 63, 95));
                        if (_image.isAllocated()) {
                            if (error != tgs::TGSERROR_FAILED) {
                                path.setStrokeColor(ofColor(63, 63, 255));
                                path.setFillColor(ofColor(63, 63, 255, 95));
                            }
                        }
                        break;
                }
                ofRotateY(-_azimuth);
                if (_elevation > 0) {
                    path.setStrokeWidth(0);
                    path.clear();
                    path.arc(0, 0, _radius, _radius, 0, _elevation);
                    path.lineTo(0, 0, 0);
                    path.close();
                    path.setFilled(true);
                    path.draw();
                }
                ofRotateZ(_elevation);
                if (ofGetElapsedTimeMillis() / 500 % 2 == 0) {
                    path.setStrokeWidth(3.0f * scale);
                    path.clear();
                    path.moveTo(0, 0, 0);
                    path.lineTo(_radius * 1.5, 0, 0);
                    path.setFilled(false);
                    path.draw();
                }
                break;
            default:
                break;
        }
        ofPopMatrix();
    }
    glDepthMask(GL_TRUE);
    ofPopMatrix();
    return;
}

/*private */void tgsARAnttenaMonitor::drawCompass(void)
{
    static char const table[][7] = {"東", "南東", "南", "南西", "西", "北西", "北", "北東"};
    int i;
    
    ofPushMatrix();
    ofFill();
    ofSetColor(0, 0, 0);
    for (i = 0; i < lengthof(table); ++i) {
        {
            ofPushMatrix();
            ofTranslate(_radius + 5, 0);
            ofRotateZ(90);
            switch (_mode) {
                case MODE_3D_COMPOSITION:
                    ofRotateY(180);
                    break;
                default:
                    break;
            }
            _font.drawStringAsShapes(table[i], -_font.stringWidth(table[i]) / 2, 0);
            ofPopMatrix();
        }
        ofRotateZ(-360.0f / lengthof(table));
    }
    ofPopMatrix();
    return;
}

/*private */void tgsARAnttenaMonitor::drawGrid(void)
{
    ofPushMatrix();
    ofRotateX(90);
    ofRotateY(90);
    ofNoFill();
    ofSetColor(127, 127, 127, 7);
    ofSphere(0, 0, 0, _radius);
    ofPopMatrix();
    return;
}
