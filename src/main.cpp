#define TARGET_OSX

#include "ofMain.h"
#include "ofAppGlutWindow.h"
#include "testApp.h"


//========================================================================

int main(int argc, char* argv[])
{

    ofAppGlutWindow window;
    ofSetupOpenGL(&window, testApp::WINDOWED_WIDTH, testApp::WINDOWED_HEIGHT,
#if 0
				  //OF_GAME_MODE
                  OF_FULLSCREEN
#else //1
				  OF_WINDOW
#endif //1
				  );
	
    ofRunApp( new testApp());

}
