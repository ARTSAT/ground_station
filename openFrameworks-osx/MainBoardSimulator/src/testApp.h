#pragma once

#include "ofMain.h"
#include "TGSDeviceLoader.h"
#include "MBSMainBoard.h"
#include "ofxUI.h"

using namespace std;
using namespace tgs;

class testApp : public ofBaseApp, private ofThread , private MBSMainBoard::Notifier {
    private:
        TGSDeviceLoader _loader;
        MBSMainBoard _main;
        ofMutex _mutexMain;
        unsigned char _telemetry[63];
        ofMutex _mutexTelemetry;
        int _beat;
        ofMutex _mutexBeat;
        ofxUICanvas* _ui;
        ofxUILabel* _state;
        ofxUILabel* _memory;
        ofxUILabelToggle* _connect;
        ofxUILabelButton* _ledState;
        ofxUILabelButton* _ledTelemetry;
    
	public:
		void setup();
        void exit();
		void update();
        void draw();
        
        void threadedFunction(void);
        void onNotifyNSD(int address, TGSError error);
        void guiEvent(ofxUIEventArgs& e);
		
		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y);
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
    private:
        TGSError open(void);
        void close(void);
        void setLabelState(string const& param);
        void setLabelMemory(string const& param);
        void setLabelMemory(int param);
        void setToggleConnect(bool param);
};
