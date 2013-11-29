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
**      testApp.cpp
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

#include "testApp.h"

static unsigned char const g_table[59] = {
    0x00, 0x00, 0x00, 0x00,
    0x04, 0x05, 0x00,
    0xFF, 0xFF, 0xFF,   // reserved
    0x09, 0x38, 0x25, 0x00, 0x00, 0x01, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x05, 0x00, 0x05, 0x18, 0x0A,
    0xFF,               // reserved
    0x4C, 0x4C, 0x4C, 0x4C, 0x4C, 0x4C, 0x4C, 0x4C, 0x4C, 0x4C, 0x4C, 0x4C, 0x4C, 0x4C, 0x4C, 0x4C, 0x4C, 0x4C,
    0xFF, 0xFF,         // reserved
    0x85, 0x86, 0x80,
    0xA5, 0x65, 0x23,
    0xFF, 0xFF, 0xFF    // reserved
};

void testApp::setup()
{
    bool value(false);
    
    ofSetFrameRate(30);
    ofEnableSmoothing();
    ofEnableAlphaBlending();
    ofBackground(127);
    
    _main.setNotifier(this);
    memcpy(_telemetry, g_table, sizeof(_telemetry));
    _beat = 0;
    
    _ui = new ofxUICanvas(0, 0, ofGetWidth(), ofGetHeight());
    _ui->addWidgetDown(new ofxUILabel("INVADER MainBoard Simulator", OFX_UI_FONT_MEDIUM));
    _ui->addSpacer();
    _state = new ofxUILabel("", OFX_UI_FONT_MEDIUM);
    _ui->addWidgetDown(_state);
    _audio = new ofxUILabel("", OFX_UI_FONT_MEDIUM);
    _ui->addWidgetDown(_audio);
    _connect = new ofxUILabelToggle("", value, 150);
    _ui->addWidgetDown(_connect);
    _ui->addWidgetRight(new ofxUILabelButton("asd", value, 150));
    _ledTelemetry = new ofxUILabelButton("", value, 50);
    _ledTelemetry->setVisible(false);
    _ui->addWidgetRight(_ledTelemetry, OFX_UI_ALIGN_RIGHT);
    _ledState = new ofxUILabelButton("", value, 50);
    _ledState->setVisible(false);
    _ui->addWidgetLeft(_ledState);
    ofAddListener(_ui->newGUIEvent, this, &testApp::guiEvent);
    
    setLabelState("disconnect");
    setLabelAudio("off");
    setToggleConnect(false);
    return;
}

void testApp::exit()
{
    close();
    delete _ui;
    return;
}

void testApp::update()
{
    _mutexMain.lock();
    _loader.update();
    _mutexMain.unlock();
    return;
}

void testApp::threadedFunction(void)
{
    unsigned int time;
    int i;
    
    while (isThreadRunning()) {
        time = ofGetUnixTime();
        _telemetry[0] = (time >> 24) & 0xFF;
        _telemetry[1] = (time >> 16) & 0xFF;
        _telemetry[2] = (time >>  8) & 0xFF;
        _telemetry[3] = (time >>  0) & 0xFF;
        _mutexMain.lock();
        if (_main.pushTelemetry(_telemetry, sizeof(_telemetry)) == TGSERROR_OK) {
            _mutexBeat.lock();
            _beat = 255;
            _mutexBeat.unlock();
        }
        else {
            _mutexBeat.lock();
            _beat = -255;
            _mutexBeat.unlock();
        }
        _mutexMain.unlock();
        sleep(900);
    }
    return;
}

void testApp::onNotifyNSD(void)
{
    setLabelState("shutdown");
    return;
}

void testApp::onNotifyDON(void)
{
    setLabelAudio("on");
    return;
}

void testApp::onNotifyDOF(void)
{
    setLabelAudio("off");
    return;
}

void testApp::guiEvent(ofxUIEventArgs& e)
{
    switch (e.widget->getID()) {
        case 5:
        {
            if (static_cast<ofxUILabelToggle*>(e.widget)->getValue()) {
                open();
            }
            else {
                close();
            }
            break;
        }
        case 7:
        {
            TGSError error;
            
            if (!static_cast<ofxUILabelButton*>(e.widget)->getValue()) {
                if (!_asd) {
                    _mutexMain.lock();
                    error = _main.shutdown();
                    _mutexMain.unlock();
                    if (error == TGSERROR_OK) {
                        setLabelState("abnormal");
                        _asd = true;
                    }
                }
            }
            break;
        }
    }
    return;
}

TGSError testApp::open(void)
{
    TGSError error(TGSERROR_OK);
    
    close();
    if ((error = _loader.append(&_main, "mainboard")) == TGSERROR_OK) {
        if ((error = _loader.open(ofToDataPath("usbserial.xml"))) == TGSERROR_OK) {
            if (_main.isValid()) {
                setLabelState("start");
                setLabelAudio("on");
                setToggleConnect(true);
                _asd = false;
                startThread();
            }
            else {
                error = TGSERROR_FAILED;
            }
        }
    }
    if (error != TGSERROR_OK) {
        close();
    }
    return error;
}

void testApp::close(void)
{
    waitForThread();
    _loader.close();
    _loader.clear();
    setLabelAudio("off");
    setLabelState("disconnect");
    setToggleConnect(false);
    return;
}

void testApp::setLabelState(string const& param)
{
    _state->setLabel("current state : " + param);
    return;
}

void testApp::setLabelAudio(string const& param)
{
    _audio->setLabel("audio bus : " + param);
    return;
}

void testApp::setToggleConnect(bool param)
{
    if (param) {
        _connect->setValue(true);
        _connect->setLabelText("disconnect");
    }
    else {
        _connect->setValue(false);
        _connect->setLabelText("connect");
    }
    return;
}

void testApp::draw()
{
    ofFill();
    ofSetColor(107, 107, 107);
    _ledState->getRect()->draw();
    _ledTelemetry->getRect()->draw();
    if (_state->getLabel().find("disconnect") != string::npos) {
        ofSetColor(255, 127, 127);
        _ledState->getRect()->draw();
    }
    else if (_state->getLabel().find("start") != string::npos) {
        ofSetColor(127, 255, 127);
        _ledState->getRect()->draw();
    }
    else if (_state->getLabel().find("abnormal") != string::npos) {
        ofSetColor(255, 255, 127);
        _ledState->getRect()->draw();
    }
    else if (_state->getLabel().find("shutdown") != string::npos) {
        if (ofGetElapsedTimeMillis() / 500 % 2 == 0) {
            ofSetColor(255, 127, 127);
        }
        else {
            ofSetColor(127, 255, 127);
        }
        _ledState->getRect()->draw();
    }
    _mutexBeat.lock();
    if (_beat > 0) {
        ofSetColor(127, 255, 127, _beat);
        _ledTelemetry->getRect()->draw();
        if ((_beat -= 10) < 0) {
            _beat = 0;
        }
    }
    else if (_beat < 0) {
        ofSetColor(255, 127, 127, -_beat);
        _ledTelemetry->getRect()->draw();
        if ((_beat += 10) > 0) {
            _beat = 0;
        }
    }
    _mutexBeat.unlock();
    return;
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){
    
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y){
    
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
    
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
    
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