#ifndef _TEST_APP
#define _TEST_APP

#define USE_MIDI_IN

#include "ofMain.h"
#include "ofxOsc.h"

#ifdef USE_MIDI_IN
#include "ofxMidiIn.h"
#endif

#include "ImageSource.h"

#include "fftw3.h"

#include "Controller.h"
#include "ControllableParameter.h"
#include "IntervalTimer.h"
#include "BeatDetector.h"

class Mesh;
class GLShader;
class Framebuffer;

class testApp : public ofBaseApp
#ifdef USE_MIDI_IN
, ofxMidiListener
#endif
{
public:
	testApp();
	~testApp() {}
    void setup();
    void exit();
    void update();
    void draw();
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void resized(int w, int h);
    void audioReceived(float * input, int bufferSize, int nChannels);
//	void newMessage(int port, int id, int value, double timestamp);
    void newMidiMessage(ofxMidiMessage& msg);

private:
	void updateWindowParameters();
	void fadeParameters(float fade);
	void updateOscMessage();
	void updateControllableParameters();
	void resetControllableParameters();
	bool loadControllableParameters(int index);
	void saveControllableParameters(int index);
	void setNextControllableParameters(int index);
#ifdef USE_MIDI_IN
	void setMidiValue(ControllableParameter* cp, int id, int value);
	void parseMidiData();
private:
	struct MIDIData {
		int port;
		int id;
		int value;
		double timestamp;
	};
	
	list<MIDIData> midiDatas;
    ofxMidiIn midiIn;
#endif
	
	enum { IN_PORT = 8000, OUT_PORT = 9000 };
	ofxOscReceiver	receiver;
	ofxOscSender sender;
	
	vector<ImageSource*> imageSources;
	vector<ImageSource*> secondaryImageSources;
    vector<ImageSource*> noiseImageSources;
    vector<ImageSource*> pointSpriteImageSources;	

	ImageSource* imageSourceA;
	ImageSource* imageSourceB;
	ImageSource* imageSourceCamera;
	ImageSource* imageSourceNoise;
	ImageSource* imageSourcePointSprite;
	
	int imageSourceIndexA;
	int imageSourceIndexB;
	int imageSourceIndexNoise;
	int imageSourceIndexPointSprite;
	
    bool frameByframe;
	
    GLuint* textureIds;
	GLint textureIdsNum;
    Mesh* mesh;

    float * left;
    float * right;
    float peakLeft;
    float peakRight;
    float peakLeftRight;
    static const float PEAK_DECAY_MAX;
    static const float PEAK_DECAY_MIN;
    static const float PEAK_DECAY_DEFAULT;

    static const int SAMPLES;
    fftwf_complex *outLeft;
    fftwf_complex *outRight;
    fftwf_plan planLeft;
    fftwf_plan planRight;
    float* fftPeakLeft;
    float* fftPeakRight;
	
	int meshNumXOld;
	int meshNumYOld;

	int width;
	int height;
    int pmouseX;
    int pmouseY;

	Framebuffer* fbo;
	Framebuffer* fboFeedback;
	
	GLShader* shader;
	GLShader* shaderDof;
	Controller controller;
	
	enum { KEYS_SIZE = 512 };
	bool keys[KEYS_SIZE];
	
	enum { PRESET_PARAMETER_COUNT = 15 };
	enum
	{
		PARAMETER_POINT_SIZE,
		PARAMETER_SCALE_X,
		PARAMETER_SCALE_Y,
		PARAMETER_ROTATION_X,
		PARAMETER_ROTATION_Y,
		PARAMETER_ROTATION_CENTER_X,
		PARAMETER_ROTATION_CENTER_Y,
		PARAMETER_DISTANCE,
		PARAMETER_DISPLACEMENT_GAIN,
		PARAMETER_Z_CLIP,
		PARAMETER_Z_OFFSET,
		PARAMETER_COLOR_GAIN,
		PARAMETER_TEXTURE_BLEND,
		PARAMETER_MESH_NUM_X,
		PARAMETER_MESH_NUM_Y,
		PARAMETER_POINT_SMOOTH_MODE,
		PARAMETER_NOISE_GAIN,
		PARAMETER_DRAW_MODE,
		PARAMETER_SOUND_GAIN,
		PARAMETER_PEAK_DECAY,
		PARAMETER_PEAK_LUMI_GAIN,
		PARAMETER_PEAK_POINT_SIZE_GAIN,
		PARAMETER_TEXTURE_FILTER,
		PARAMETER_BLENDING_MODE,
		PARAMETER_LAYER_COUNT,
		PARAMETER_AUTO_ROTATION,
		PARAMETER_FOCAL_OFFSET,
		PARAMETER_FEEDBACK_GAIN,
		PARAMETER_COUNT
	};
	struct PresetEntry
	{
		PresetEntry() : isAvailable(false) {}
		bool isAvailable;
		ControllableParameter params[PARAMETER_COUNT];
	};
	PresetEntry presetParams[PRESET_PARAMETER_COUNT];

	int activePresetParamIndexA;
	int activePresetParamIndexB;
	ControllableParameter paramPointSize;
	
	ControllableParameter scaleX;
	ControllableParameter scaleY;
	
	ControllableParameter rotationX;
	ControllableParameter rotationY;
	ControllableParameter rotationCenterX;
	ControllableParameter rotationCenterY;
	ControllableParameter distance;
	ControllableParameter displacementGain;
	ControllableParameter zClip;
	ControllableParameter zOffset;
	ControllableParameter colorGain;
	ControllableParameter textureBlend;
	ControllableParameter meshNumX;
	ControllableParameter meshNumY;
	ControllableParameter pointSmoothMode;
	ControllableParameter noiseGain;
	ControllableParameter drawMode;
	ControllableParameter soundGain;
	ControllableParameter peakDecay;
	ControllableParameter peakLumiGain;
	ControllableParameter peakPointSizeGain;
	ControllableParameter textureFilter;	
	ControllableParameter blendingMode;
	ControllableParameter layerCount;
	ControllableParameter autoRotation;
	ControllableParameter focalOffset;
	ControllableParameter feedbackGain;
	
	static const int MIDI_UNINITIALIZED_VALUE = -1;
	enum { MIDI_PARAMS_SIZE = 256 };
	int midiValueOffset[MIDI_PARAMS_SIZE];
	
	IntervalTimer autoRotationTimer;
	
	float overlayImageAlpha;
	BeatDetector beatDetector;
	IntervalTimer beatTimer;
	bool forceBeat;
	
	GLuint* noiseImage;

	float noiseFeedback;
	
	float lastParameterFade;
    
    bool isFullscreen;
public:
    enum {
        WINDOWED_WIDTH = 800,
        WINDOWED_HEIGHT = 600
    };
};

#endif
