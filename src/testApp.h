#ifndef _TEST_APP
#define _TEST_APP

#include "ofMain.h"
#include "ofxOpenNI.h"

#include "PointCloud.h"
#include "UserSkeleton.h"
#include "Bone.h"
#include "Global.h"
//#include "ofxSimpleGuiToo.h"

#include "CanonCamera.h"

//#define PLAYBACK

using namespace xn;

class testApp : public ofBaseApp{

public:
	
	void setup();
	void update();
	void draw();
	void exit();
	void keyPressed(int key);
    void initGui();
    
    void newPhotoDownloaded(int&code);
    
    UserSkeleton* getUserId(int id);
    
    ofxONI  oni;
	
#ifndef PLAYBACK
	ofxOpenNIRecorder	recorder;
#endif
	
    CanonCamera eos;
    
	ofCamera	camera;
	
	UserSkeleton	userPointCloud;
    vector<UserSkeleton*> users;
	PointCloud		scenePointCloud;
	
private:
	
    void drawGraph();
    void drawPointCloud();
    
	bool bOrbit, bDebug, bRecord;
	int distance;
    float newImageShownSeconds;
    
	ofImage screen;
	
	float camFOV;
};

#endif
