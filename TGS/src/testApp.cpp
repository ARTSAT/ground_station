#include "testApp.h"

#define MARGIN  (10)

//--------------------------------------------------------------
void testApp::setup()
{
    ofxTrueTypeFontUC::setGlobalDpi(72);
    ofSetFrameRate(60);
    ofSetCircleResolution(96);
    ofEnableSmoothing();
    ofEnableAlphaBlending();
    ofEnableNormalizedTexCoords();
    ofBackground(191, 191, 191);
    
    _mouse.set(ofGetMouseX(), ofGetMouseY());
    
    _time = 0;
    _azimuth = 0;
    _elevation = 0;
    _frequency_main = 0;
    _frequency_sub = 0;
    
    {
        _loader.append(&_rotator, "rotator");
        _loader.append(&_civ, "civ");
        _loader.append(&_transceiver, "transceiver");
        _loader.append(&_tnc, "tnc");
        cout << _loader.open(ofToDataPath("usbserial.xml")).print() << endl;
        
        _transceiver.setNotifier(this);
        if (_transceiver.connect(&_civ) == tgs::TGSERROR_OK) {
            _transceiver.selectSubBand();
            _transceiver.getOperationFrequency(&_frequency_sub);
            _transceiver.selectMainBand();
            _transceiver.getOperationFrequency(&_frequency_main);
        }
        cout << "TNC : " << _tnc.setupModePacket("JA1TGS", "JA1INV").print() << endl;
    }
    _ar.setMode(tgsARAnttenaMonitor::MODE_3D_COMPOSITION);
    _ar.open("http://ascam.idd.tamabi.ac.jp:10088/snapshot.cgi?user=artsatadmin&pwd=invader", 100, "hiragino_marugo_pro_w4.otf", 14, 100, ofPoint(32, -20), 30, -90);
    _ar2.setMode(tgsARAnttenaMonitor::MODE_WEBCAM);
    _ar2.open("http://ascam.idd.tamabi.ac.jp:10088/snapshot.cgi?user=artsatadmin&pwd=invader", 100, "hiragino_marugo_pro_w4.otf", 14, 100, ofPoint(32, -20), 30, -90);
    return;
}

void testApp::onNotifyOperationFrequency(int frequency)
{
    tgs::TGSTransceiverCIV::Notifier::onNotifyOperationFrequency(frequency);
    _frequency_main = frequency;
    return;
}

void testApp::onNotifyOperationMode(tgs::TGSTransceiverCIV::OperationModeEnum mode, tgs::TGSTransceiverCIV::FilterEnum filter)
{
    tgs::TGSTransceiverCIV::Notifier::onNotifyOperationMode(mode, filter);
    return;
}

void testApp::exit()
{
    _ar2.close();
    _ar.close();
    _loader.close();
    return;
}

//--------------------------------------------------------------
void testApp::update()
{
    int time;
    
    time = ofGetElapsedTimeMillis();
    if (time >= _time + 250) {
        if (_rotator.getAngle(&_azimuth, &_elevation) == tgs::TGSERROR_OK) {
            if (!(5 < _azimuth && _azimuth < 355)) {
                _rotator.stopManuallyAzimuth();
            }
            if (!(5 < _elevation && _elevation < 175)) {
                _rotator.stopManuallyElevation();
            }
            _ar.setAzimuth(_azimuth);
            _ar.setElevation(_elevation);
            _ar2.setAzimuth(_azimuth);
            _ar2.setElevation(_elevation);
        }
        _time = time;
    }
    _loader.update();
    return;
}

//--------------------------------------------------------------
void testApp::draw()
{
    int offset;
    int i;
    ofRectangle rect;
    string info;
    
    rect.x = MARGIN;
    rect.y = MARGIN;
    rect.width = ofGetWidth() - MARGIN * 2;
    rect.height = ofGetHeight() - MARGIN * 2;
    _ar.setViewport(rect);
    _ar.draw();
    
    if (_ar.getMode() == tgsARAnttenaMonitor::MODE_3D_COMPOSITION) {
        rect.width /= 5;
        rect.height /= 5;
        rect.x = ofGetWidth() - MARGIN * 3 - rect.width;
        rect.y = ofGetHeight() - MARGIN * 3 - rect.height;
        _ar2.setViewport(rect);
        _ar2.draw();
    }
    
    ofSetColor(0, 0, 0);
    offset = 15;
    ofDrawBitmapString("       Move azimuth: <Left>, <Right>", 6, offset);
    offset += 15;
    ofDrawBitmapString("     Move elevation: <Up>, <Down>", 6, offset);
    offset += 15;
    ofDrawBitmapString("        Stop moving: <Space>", 6, offset);
    offset += 15;
    ofDrawBitmapString("      Reset 3D View: <X>", 6, offset);
    offset += 15;
    offset += 15;
    ofDrawBitmapString("        WebCam mode: <1>", 6, offset);
    offset += 15;
    ofDrawBitmapString("    AR Monitor mode: <2>", 6, offset);
    offset += 15;
    ofDrawBitmapString("AR Composition mode: <3>", 6, offset);
    offset += 15;
    ofDrawBitmapString("3D Composition mode", 6, offset);
    offset += 15;
    ofDrawBitmapString("           + WebCam: <4>", 6, offset);
    offset += 15;
    ofDrawBitmapString("       + AR Monitor: <5>", 6, offset);
    offset += 15;
    ofDrawBitmapString("   + AR Composition: <6>", 6, offset);
    offset += 15;
    offset += 15;
    ofDrawBitmapString("            Azimuth: " + ofToString(_azimuth, 3) + " deg", 6, offset);
    offset += 15;
    ofDrawBitmapString("          Elevation: " + ofToString(_elevation, 3) + " deg", 6, offset);
    offset += 15;
    offset += 15;
    if (_ar.getMode() != tgsARAnttenaMonitor::MODE_3D_COMPOSITION) {
        switch (_ar.getCaptureError()) {
            case tgs::TGSERROR_OK:
                info = "monitoring";
                break;
            case tgs::TGSERROR_WAIT_RESULT:
                info = "connecting...";
                break;
            case tgs::TGSERROR_NETWORK_DOWN:
                info = "network down";
                break;
            default:
                info = "unknown";
                break;
        }
    }
    else {
        info = "-";
    }
    ofDrawBitmapString("      Main AR state: " + info, 6, offset);
    offset += 15;
    if (_ar.getMode() == tgsARAnttenaMonitor::MODE_3D_COMPOSITION) {
        switch (_ar2.getCaptureError()) {
            case tgs::TGSERROR_OK:
                info = "monitoring";
                break;
            case tgs::TGSERROR_WAIT_RESULT:
                info = "connecting...";
                break;
            case tgs::TGSERROR_NETWORK_DOWN:
                info = "network down";
                break;
            default:
                info = "unknown";
                break;
        }
    }
    else {
        info = "-";
    }
    ofDrawBitmapString("       Sub AR state: " + info, 6, offset);
    offset += 15;
    
    offset = ofGetHeight() - 15;
    ofDrawBitmapString("  TNC Converse Mode: <K>", 6, offset);
    offset -= 15;
    ofDrawBitmapString("   TNC Command Mode: <I>", 6, offset);
    offset -= 15;
    offset -= 15;
    ofDrawBitmapString("   Update Frequency: <U>", 6, offset);
    offset -= 15;
    ofDrawBitmapString(" Communication Mode: <C>", 6, offset);
    offset -= 15;
    ofDrawBitmapString("        Beacon Mode: <B>", 6, offset);
    offset -= 15;
    offset -= 15;
    ofDrawBitmapString(" Sub Band Frequency: " + ofToString(_frequency_sub / 1000000.0, 5) + " MHz", 6, offset);
    offset -= 15;
    ofDrawBitmapString("Main Band Frequency: " + ofToString(_frequency_main / 1000000.0, 5) + " MHz", 6, offset);
    offset -= 15;
    return;
}

//--------------------------------------------------------------
void testApp::keyPressed(int key)
{
    int azi;
    int ele;
    int freq;
    tgs::TGSError error(tgs::TGSERROR_OK);
    
    switch (key) {
        case 'b':
        case 'B':
            error = _transceiver.selectModeBeacon();
            break;
        case 'c':
        case 'C':
            error = _transceiver.selectModeCommunication();
            break;
        case 'u':
        case 'U':
            error = _transceiver.selectSubBand();
            error = _transceiver.getOperationFrequency(&_frequency_sub);
            error = _transceiver.selectMainBand();
            error = _transceiver.getOperationFrequency(&_frequency_main);
            break;
        case 'k':
        case 'K':
            error = _tnc.selectModeConverse();
            break;
        case 'i':
        case 'I':
            error = _tnc.selectModeCommand();
            break;
        case OF_KEY_LEFT:
            if ((error = _rotator.getAngleAzimuth(&azi)) == tgs::TGSERROR_OK) {
                if (5 < azi) {
                    if ((error = _rotator.rotateManuallyLeft()) == tgs::TGSERROR_OK) {
                        ofLogNotice() << "rotateManuallyLeft...";
                    }
                }
            }
            break;
        case OF_KEY_RIGHT:
            if ((error = _rotator.getAngleAzimuth(&azi)) == tgs::TGSERROR_OK) {
                if (azi < 355) {
                    if ((error = _rotator.rotateManuallyRight()) == tgs::TGSERROR_OK) {
                        ofLogNotice() << "rotateManuallyRight...";
                    }
                }
            }
            break;
        case OF_KEY_UP:
            if ((error = _rotator.getAngleElevation(&ele)) == tgs::TGSERROR_OK) {
                if (ele < 175) {
                    if ((error = _rotator.rotateManuallyUp()) == tgs::TGSERROR_OK) {
                        ofLogNotice() << "rotateManuallyUp...";
                    }
                }
            }
            break;
        case OF_KEY_DOWN:
            if ((error = _rotator.getAngleElevation(&ele)) == tgs::TGSERROR_OK) {
                if (5 < ele) {
                    if ((error = _rotator.rotateManuallyDown()) == tgs::TGSERROR_OK) {
                        ofLogNotice() << "rotateManuallyDown...";
                    }
                }
            }
            break;
        case ' ':
            if ((error = _rotator.stopManually()) == tgs::TGSERROR_OK) {
                ofLogNotice() << "stopManually...";
                if ((error = _rotator.getAngle(&azi, &ele)) == tgs::TGSERROR_OK) {
                    ofLogNotice() << "Az = " << azi << ", El = " << ele;
                }
            }
            break;
        case 'x':
        case 'X':
            _ar.setRotation(ofQuaternion());
            break;
        case '1':
            _ar.setMode(tgsARAnttenaMonitor::MODE_WEBCAM);
            _ar.setRotation(ofQuaternion());
            break;
        case '2':
            _ar.setMode(tgsARAnttenaMonitor::MODE_AR_MONITOR);
            _ar.setRotation(ofQuaternion());
            break;
        case '3':
            _ar.setMode(tgsARAnttenaMonitor::MODE_AR_COMPOSITION);
            _ar.setRotation(ofQuaternion());
            break;
        case '4':
            _ar.setMode(tgsARAnttenaMonitor::MODE_3D_COMPOSITION);
            _ar.setRotation(ofQuaternion());
            _ar2.setMode(tgsARAnttenaMonitor::MODE_WEBCAM);
            break;
        case '5':
            _ar.setMode(tgsARAnttenaMonitor::MODE_3D_COMPOSITION);
            _ar.setRotation(ofQuaternion());
            _ar2.setMode(tgsARAnttenaMonitor::MODE_AR_MONITOR);
            break;
        case '6':
            _ar.setMode(tgsARAnttenaMonitor::MODE_3D_COMPOSITION);
            _ar.setRotation(ofQuaternion());
            _ar2.setMode(tgsARAnttenaMonitor::MODE_AR_COMPOSITION);
            break;
    }
    if (error != tgs::TGSERROR_OK) {
        ofLogError() << error.print();
    }
    return;
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button)
{
    ofQuaternion xRot;
    ofQuaternion yRot;
    
    xRot.makeRotate((y - _mouse.y) * 0.4, 1, 0, 0);
    yRot.makeRotate((x - _mouse.x) * 0.4, 0, 1, 0);
    _ar.setRotation(_ar.getRotation() * xRot * yRot);
    _mouse.set(x, y);
    return;
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button)
{
    _mouse.set(x, y);
    return;
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}