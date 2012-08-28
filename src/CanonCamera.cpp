/*
 *  CanonCamera.cpp
 *  PopAdidas
 *
 *  Created by kikko on 9/28/10.
 *  Copyright 2010 lab212. All rights reserved.
 *
 */

#include "CanonCamera.h"

CanonCamera::CanonCamera() {
	
    isAvailable = false;
	wrapper.setup(0);
};

CanonCamera::~CanonCamera() {
	
	wrapper.closeSession();
    cout << "Canon Camera - close session" << endl;
}

void CanonCamera::closeSession()
{
    wrapper.closeSession();
}

void CanonCamera::init() {
    
    wrapper.closeSession();
    isAvailable = wrapper.openSession();
    
	ofAddListener(wrapper.photoDownloadedEvent, this, &CanonCamera::photoDownloadedHandler);
	
	wrapper.setDeleteFromCameraAfterDownload(true);
    
    //photoShutterSpeed = 100;
    //photoApperture = 26;
    
    //wrapper.setShutterSpeed(photoShutterSpeed);
	//wrapper.setAperture(photoApperture);
    
    wrapper.setDownloadPath("photos/");
    
	//ofAddListener(ofEvents.keyPressed, this, &CanonCamera::keyPressed);
}

void CanonCamera::exportImage() {
	
    cout << "taking picture..." << endl;
	wrapper.takePicture();
}
/*
void CanonCamera::keyPressed(ofKeyEventArgs &args) {
	
    if(args.key=='p') {
        exportImage();
    }
		
	if(args.key!=OF_KEY_DOWN && args.key!=OF_KEY_UP && args.key!=OF_KEY_LEFT && args.key!=OF_KEY_RIGHT) return;
	
	switch(args.key){
		
        case OF_KEY_DOWN:
            photoShutterSpeed--;
            break;
        case OF_KEY_UP:
            photoShutterSpeed++;
            break;
        case OF_KEY_LEFT:
            photoApperture--;
            break;
        case OF_KEY_RIGHT:
            photoApperture++;
            break;
    }
    
    wrapper.setShutterSpeed(photoShutterSpeed);
    wrapper.setAperture(photoApperture);

}
*/

string CanonCamera::toStr(int v) {
    return (v<10?"0":"") + ofToString(v);
}

void CanonCamera::photoDownloadedHandler(int &code) {
	
    string imgName = wrapper.getLastImageName();
    
    int mth = ofGetMonth();
    
    string newName = "";
    newName += ofToString(ofGetYear()) + "-" + toStr(ofGetMonth()) + "-" + toStr(ofGetDay());
    newName += "_" + toStr(ofGetHours()) + "-" + toStr(ofGetMinutes()) +"-" + toStr(ofGetSeconds());
    newName += ".jpg";
    ofFile("photos/"+imgName).renameTo("photos/"+newName);
    
    lastPhoto.loadImage( "photos/"+newName );
    lastPhoto.rotate90(3);
    lastPhoto.resize(1536, 1536*lastPhoto.height/lastPhoto.width);
    lastPhoto.saveImage("export/"+newName, OF_IMAGE_QUALITY_HIGH );
    
	cout << "photo downloaded" << endl;
	
    ofNotifyEvent(photoDownloadedEvent, code, this);
}

string CanonCamera::getLastImageName(){
	return wrapper.getLastImageName();
}