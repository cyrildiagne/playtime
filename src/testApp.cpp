#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	
	ofSetFrameRate(30);
	ofBackgroundHex(0xf0f0ee);
    
    initGui();
    
#ifdef PLAYBACK
    oni.initWithRecording(ofToDataPath("record.oni"));
#else
    oni.initWithXML();
    oni.depth.getXnDepthGenerator().GetAlternativeViewPointCap().SetViewPoint(oni.image.getXnImageGenerator());
    recorder.setup(&oni.context, &oni.depth, &oni.image);
#endif
	
	scenePointCloud.init(oni, POINTCLOUD_SCENE);
    
	ofSetLogLevel(OF_LOG_WARNING);
	ofSetVerticalSync(true);
	
	bRecord = false;
	bOrbit = true;
    bDebug = false;
	
	camFOV = 68.3;
	distance = 2500;
	
	camera.setFov(camFOV);
	camera.setFarClip(10000.f);
    
    screen.loadImage("assets/ecran_beforejumping.jpg");
    newImageShownSeconds = 0.0;
    
    ofAddListener(eos.photoDownloadedEvent, this, &testApp::newPhotoDownloaded);
}

void testApp::newPhotoDownloaded(int &code) {
    
    newImageShownSeconds = ofGetElapsedTimef();
}

void testApp::initGui() {
    
    // -_-...
    
    // gui
    
    //myGui.addSlider("graphYAmp", graphYAmp, 0.1, 2);
    //gui.addSlider("minY", minY, 0, 1000);
    //gui.addSlider("maxY", maxY, 0, 1000);
    
    //gui.loadFromXML();
    //gui.show();
    
    //bGuiInit = true;
}

void testApp::exit() {
    
    //eos.closeSession();
}

//--------------------------------------------------------------
void testApp::update(){
    
    if(!eos.isAvailable && ofGetElapsedTimef()>3.0f) {
        eos.init();
        return;
    }
    
    std::vector<UserSkeleton*>::iterator it = users.begin();
    while(it != users.end()) {
        (*it)->bUpdated = false;
        ++it;
    }
    
	oni.update();
	
    vector<ofxTrackedUser*> trackedUsers = oni.user.getTrackedUsers();
    
    if(trackedUsers.size() > 0) {
        
        int i;
        
        UserSkeleton* userSkeleton;
        ofxTrackedUser* trackedUser;
        
        for(i=0; i<trackedUsers.size(); i++) {
            
            trackedUser = trackedUsers[i];
            userSkeleton = getUserId(trackedUser->id);
            
            if(userSkeleton) {
                if(userSkeleton->lastPhotoDelay>0) userSkeleton->lastPhotoDelay--;
                if(userSkeleton->update() && userSkeleton->lastPhotoDelay == 0) {
                    eos.exportImage();
                    userSkeleton->lastPhotoDelay = 30;
                }
            }
            else {
                userSkeleton = new UserSkeleton();
                userSkeleton->init(oni, trackedUser->id);
                users.push_back(userSkeleton);
            }
         }
	}
    
    if(bDebug) scenePointCloud.update();
    
    // erase users that haven't been tracked
    it = users.begin();
    while(it != users.end()) {
        if( !(*it)->bUpdated) {
            delete (*it);
            it = users.erase(it);
            cout << "User removed" << endl;
        }
        else ++it;
    }
	
	if(bOrbit) {
		camera.orbit((float)mouseX/ofGetWidth()*360, (float)mouseY/ofGetHeight()*180-90, distance, ofVec3f(0,0,distance));
	}
	else {
		camera.orbit(180, 0, distance, ofVec3f(0,0,distance));
	}
}

UserSkeleton* testApp::getUserId(int id) {
    
	std::vector<UserSkeleton*>::iterator it = users.begin();
	while(it != users.end()) {
		if( (*it)->userId == id) {
			return(*it);
		}			
		++it;
	}
	return NULL;
}

//--------------------------------------------------------------


void testApp::draw(){
	
	ofSetColor(255, 255, 255);
	
    if(newImageShownSeconds>0.1 && newImageShownSeconds+10 > ofGetElapsedTimef()) {
        
        ofBackground(0, 0, 0);
        
        float w = eos.lastPhoto.getWidth();
        float h = eos.lastPhoto.getHeight();
        if(ofGetWidth()>ofGetHeight()) {
            float newWidth = ofGetHeight()*w/h;
            eos.lastPhoto.draw((ofGetWidth()-newWidth)*0.5, 0, newWidth, ofGetHeight());
        }
        else {
            float newHeight = ofGetWidth()*h/w;
            eos.lastPhoto.draw(0, (ofGetHeight()-newHeight)*0.5, ofGetWidth(), newHeight);
        }
        
        return;
    }
    
    oni.image.generateTexture();
    
	if (bDebug) {
        
        ofBackground(0, 0, 0);
        
        drawPointCloud();
		drawGraph();
        
        if (bRecord) {
            ofSetColor(255, 0, 0);
            ofCircle(15, 15, 10);
            ofSetColor(255, 255, 255);
        }
        
        ofDrawBitmapString("min jump Y : " + ofToString(minJumpY), 10, 10);
        ofDrawBitmapString("max jump Y : " + ofToString(maxJumpY), 10, 25);
        
	}
    else  {
        
        ofBackgroundHex(0xf0f0ee);
        
        if(users.size()) {
            float ratio;
            
            // we don't know yet how the screen will be oriented
            
            if(ofGetWidth()>ofGetHeight()) {
                ratio = 320.0/240.0;
                float newWidth = ofGetHeight()*ratio;
                oni.image.draw((ofGetWidth()-newWidth)*0.5, 0, newWidth, ofGetHeight());
            }
            else {
                ratio = 240.0/320.0;
                float newHeight = ofGetWidth()*ratio;
                oni.image.draw(0, (ofGetHeight()-newHeight)*0.5, ofGetWidth(), newHeight);
            }
        }
		else {
            screen.draw( (ofGetWidth()-screen.getWidth())*0.5, (ofGetHeight()-screen.getHeight())*0.5 );
        }
    }  
}

void testApp::drawGraph(){
    
    UserSkeleton* user;
    
    ofPushMatrix();
    ofTranslate(0, 40);
    
    int rectHeight = 80;
    int margin = 10;
    
    for (int i=0; i<users.size(); i++) {
        
        user = users[i];
        if(!user->waistYHistory.size()) return;
        
        float rectWidth = (user->waistYHistory.size()-1)*10;
        
        ofPushMatrix();
        ofTranslate(margin, i*(rectHeight+margin));
        
        ofSetHexColor(0x333333);
        ofRect(0, 0, rectWidth, rectHeight);
        
        ofSetHexColor(0xFFFFFF);
        
        float minY = user->initWaistY+minJumpY;
        float maxY = user->initWaistY+maxJumpY;
        
        float lastY = (user->waistYHistory[0]-minY) / (maxY-minY) * rectHeight;
        float newY;
        
        for (int j=1; j<user->waistYHistory.size(); j++) {
            
            newY = (user->waistYHistory[j]-minY) / (maxY-minY) * rectHeight;
            ofLine(10*(j-1), lastY, 10*j, newY);
            lastY = newY;
        }
        
        float initY = (user->initWaistY-minY)/(maxY-minY)*rectHeight;
        
        ofSetHexColor(0xFF0000);
        ofLine(0, initY, rectWidth, initY);
        ofDrawBitmapString(ofToString(user->initWaistY, 0), ofPoint(5, initY-5)); 
        
        ofSetHexColor(0xcccccc);
        ofDrawBitmapString(ofToString(minY, 0), ofPoint(5, 10));
        ofDrawBitmapString(ofToString(maxY, 0), ofPoint(5, rectHeight-5));
        
        ofEndShape();
        
        ofPopMatrix();
    }
    
    ofPopMatrix();
}

void testApp::drawPointCloud() {
    
    camera.begin();
    
    glEnable(GL_DEPTH_TEST);
    scenePointCloud.draw();
    glDisable(GL_DEPTH_TEST);
    
    if(bDebug) for (int i=0; i<users.size(); i++) {
        users[i]->drawBones();
    }
    camera.end();
    
    glDisable(GL_DEPTH_TEST);
}

void testApp::keyPressed(int key){
	
	switch (key) {
		case 'o': bOrbit=!bOrbit; break;
		case 's': bDebug=!bDebug; break;
		case 'f': ofToggleFullscreen(); break;
		case 'p':
            //if(users.size()) {
                eos.exportImage();
            //    users[0]->lastPhotoDelay = 30;
            //}
            break;
        case OF_KEY_UP: minJumpY+=1; break;
		case OF_KEY_DOWN: minJumpY-=1; break;
        case OF_KEY_RIGHT: maxJumpY+=1; break;
		case OF_KEY_LEFT: maxJumpY-=1; break;
		/*case OF_KEY_UP: distance+=10; break;
		case OF_KEY_DOWN: distance-=10; break;
#ifndef PLAYBACK
		case 'r': 
			if( (bRecord=!bRecord) ) {
				recorder.startRecord("record.oni");
			} else {
                recorder.stopRecord();
            }
			break;
#endif
        */
		default : break;
	}
}