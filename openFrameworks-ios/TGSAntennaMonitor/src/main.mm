#include "ofMain.h"
#include "testApp.h"

int main(){
    ofAppiPhoneWindow* window = new ofAppiPhoneWindow();
    window->enableRetina();
    window->enableAntiAliasing(2);
    window->enableDepthBuffer();
	ofSetupOpenGL(window, 1024, 768, OF_FULLSCREEN);			// <-------- setup the GL context
	ofRunApp(new testApp);
}
