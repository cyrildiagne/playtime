/*
 *  UserPointCloud.h
 *  skin
 *
 *  Created by kikko on 2/24/11.
 *  Copyright 2011 Cyril Diagne. All rights reserved.
 *
 */

#pragma once

#include "ofMain.h"
#include "ofxOpenNI.h"
#include "Bone.h"
#include "Global.h"

using namespace xn;

class UserSkeleton {
	
public:
	
	UserSkeleton();
    ~UserSkeleton();
	
	void init(ofxONI & ni, XnUserID userId);
	bool update();
	void draw();
    void drawBones();
    
    XnUserID userId;
	ofxUserGenerator * user;
    
    bool bUpdated;
    int lastPhotoDelay;
	bool bBonesInit;
    
    float initWaistY;
    vector<int> waistYHistory;
    
private:
	
	void initBones();
	
	void setBonesLengths();
	void setBoneLength(int boneId, XnSkeletonJoint endBone);
	
	void updateBones();
	
	Bone	bones[kNumTestNodes];
	
	int numBones;
	
};