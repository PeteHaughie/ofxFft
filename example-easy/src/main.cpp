#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main()
{

	// Use ofGLFWWindowSettings for more options like multi-monitor fullscreen
	ofGLWindowSettings settings;
	settings.setSize(640, 480);
#ifdef linux
	settings.windowMode = OF_GAME_MODE; // can also be OF_FULLSCREEN
#else
	settings.windowMode = OF_WINDOW; // can also be OF_FULLSCREEN
#endif

	auto window = ofCreateWindow(settings);

	ofRunApp(window, std::make_shared<ofApp>());
	ofRunMainLoop();
}
