#pragma once

#include "ofMain.h"
#include "tgsARAntennaMonitor.h"

#include "TGSDeviceLoader.h"
#include "TGSRotatorGS232B.h"
#include "TGSTransceiverCIV.h"
#include "TGSTransceiverIC9100.h"
#include "TGSTNCTNC555.h"

class testApp : public ofBaseApp, public tgs::TGSTransceiverCIV::Notifier {
private:
    tgsARAnttenaMonitor _ar;
    tgsARAnttenaMonitor _ar2;
    ofVec2f _mouse;
    
    int _time;
    int _azimuth;
    int _elevation;
    int _frequency_main;
    int _frequency_sub;
    
    tgs::TGSDeviceLoader _loader;
    tgs::TGSRotatorGS232B _rotator;
    tgs::TGSTransceiverCIV _civ;
    tgs::TGSTransceiverIC9100 _transceiver;
    tgs::TGSTNCTNC555 _tnc;
    
	public:
		void setup();
        void exit();
		void update();
		void draw();
    
        void onNotifyOperationFrequency(int frequency);
        void onNotifyOperationMode(tgs::TGSTransceiverCIV::OperationModeEnum mode, tgs::TGSTransceiverCIV::FilterEnum filter);
		
		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y);
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
};
