#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
    ofRectangle rect;
    
	// initialize the accelerometer
	ofxAccelerometer.setup();
	
	//If you want a landscape oreintation 
	//iPhoneSetOrientation(OFXIPHONE_ORIENTATION_LANDSCAPE_RIGHT);
	
    ofxTrueTypeFontUC::setGlobalDpi(72);
    ofSetFrameRate(60);
    ofSetCircleResolution(96);
    ofEnableSmoothing();
    ofEnableAlphaBlending();
    ofEnableNormalizedTexCoords();
	ofBackground(0, 0, 0);
    
    rect.x = 0;
    rect.y = 60;
    rect.width = ofGetWidth();
    rect.height = 2 * rect.width / 3;
    _monitor.setViewport(rect);
    _monitor.setMode(tgsARAnttenaMonitor::MODE_AR_MONITOR);
    if (_monitor.open("http://ascam.idd.tamabi.ac.jp:10088/snapshot.cgi?user=artsatadmin&pwd=invader", 100, "hiragino_marugo_pro_w4.otf", 14, 100, ofPoint(32, -20), 30, -90) == tgs::TGSERROR_OK) {
        _time = ofxSATTime::currentTime();
    }
    else {
        ofExit();
    }
    return;
}

//--------------------------------------------------------------
void testApp::update(){
    _monitor.update();
    return;
}

//--------------------------------------------------------------
void testApp::draw(){
    int offset;
    string info;
    
	_monitor.draw();
    switch (_monitor.getCaptureError()) {
        case tgs::TGSERROR_OK:
            ofSetColor(127, 255, 127);
            info = "monitoring";
            _time = ofxSATTime::currentTime();
            break;
        case tgs::TGSERROR_WAIT_RESULT:
            ofSetColor(255, 255, 127);
            info = "connecting...";
            _time = ofxSATTime::currentTime();
            break;
        case tgs::TGSERROR_NETWORK_DOWN:
            ofSetColor(255, 127, 127);
            info = "network down";
            break;
        default:
            ofSetColor(255, 127, 127);
            info = "unknown";
            break;
    }
    offset = 27;
    ofDrawBitmapString("Capture: " + _time.format("%YYYY/%MM/%DD (%WEK) %hh:%mm:%ss"), 6, offset);
    offset += 15;
    ofDrawBitmapString("  State: " + info, 6, offset);
    offset += 15;
    return;
}

//--------------------------------------------------------------
void testApp::exit(){
    _monitor.close();
}

//--------------------------------------------------------------
void testApp::touchDown(ofTouchEventArgs & touch){

}

//--------------------------------------------------------------
void testApp::touchMoved(ofTouchEventArgs & touch){

}

//--------------------------------------------------------------
void testApp::touchUp(ofTouchEventArgs & touch){
    if (_monitor.getMode() == tgsARAnttenaMonitor::MODE_AR_MONITOR) {
        _monitor.setMode(tgsARAnttenaMonitor::MODE_WEBCAM);
    }
    else {
        _monitor.setMode(tgsARAnttenaMonitor::MODE_AR_MONITOR);
    }
    return;
}

//--------------------------------------------------------------
void testApp::touchDoubleTap(ofTouchEventArgs & touch){

}

//--------------------------------------------------------------
void testApp::touchCancelled(ofTouchEventArgs & touch){
    
}

//--------------------------------------------------------------
void testApp::lostFocus(){

}

//--------------------------------------------------------------
void testApp::gotFocus(){

}

//--------------------------------------------------------------
void testApp::gotMemoryWarning(){

}

//--------------------------------------------------------------
void testApp::deviceOrientationChanged(int newOrientation){

}

