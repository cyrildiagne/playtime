/*
 *  CanonCamera.h
 *  PopAdidas
 *
 *  Created by kikko on 9/28/10.
 *  Copyright 2010 lab212. All rights reserved.
 *
 */

#pragma once

#include "ofxCanonCameraWrapper.h"

class CanonCamera {

public:
	
	CanonCamera();
	~CanonCamera();

    void init();
	void exportImage();
    void closeSession();
    
	string getLastImageName();
    
    bool isAvailable;
    
    ofImage lastPhoto;
    
    ofEvent<int> photoDownloadedEvent;
	
private:
	
	CanonCameraWrapper wrapper;
	
	void photoDownloadedHandler(int &code);
	void backgroundDownloaded(int &code);
    
    string toStr(int v);
	
	//void keyPressed(ofKeyEventArgs &args);
	
	int frameNum;
	bool bAllocated;
	string currFileName;
	
	int photoApperture;
	int photoShutterSpeed;
};