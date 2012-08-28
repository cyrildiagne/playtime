/*
 *  UserSkeleton.cpp
 *  skin
 *
 *  Created by kikko on 2/24/11.
 *  Copyright 2011 Cyril Diagne. All rights reserved.
 *
 */

#include "UserSkeleton.h"

UserSkeleton::UserSkeleton() {
	
	bBonesInit = false;
    bUpdated = false;
    lastPhotoDelay = 0;
}

UserSkeleton::~UserSkeleton() {
	
	
}

void UserSkeleton::init(ofxONI &oni, XnUserID userId) {
	
    this->user = &oni.user;
    this->userId = userId;
    update();
}

bool UserSkeleton::update() {
	
    if(!bBonesInit) initBones();
    else updateBones();
    
    float minY=initWaistY, maxY=initWaistY;
    int minYpos, maxYpos;
    
    for(int i=0; i<waistYHistory.size(); i++) {
        
        if(waistYHistory[i] < minY) {
            minY = waistYHistory[i];
            minYpos = i;
        }
        else if(waistYHistory[i]>maxY) {
            maxY = waistYHistory[i];
            maxYpos = i;
        }
    }
    
    bUpdated = true;
    
    bool bJumpGesture = minY < initWaistY+minJumpY
                        && maxY > initWaistY+maxJumpY
                        && minYpos < maxYpos;
    
    return bJumpGesture;
}

void UserSkeleton::initBones() {
	
	// First retrieve the points of the user
	
	ofVec3f zAxis = ofVec3f(0.0f, 0.0f, 1.0f);
	
	ofQuaternion qDefault	= ofQuaternion( 0	 , zAxis);
	ofQuaternion qLeft		= ofQuaternion(-90.0 , zAxis);
	ofQuaternion qRight		= ofQuaternion( 90.0 , zAxis);
	ofQuaternion qNeck		= ofQuaternion( 180.0, zAxis);
	
	bones[TORSO]		 .init( XN_SKEL_TORSO,			qDefault);
	bones[WAIST]		 .init( XN_SKEL_WAIST,			qDefault);
	bones[LEFT_SHOULDER] .init( XN_SKEL_LEFT_SHOULDER,	qLeft);
	bones[LEFT_ELBOW]	 .init( XN_SKEL_LEFT_ELBOW,		qLeft);
	bones[RIGHT_SHOULDER].init( XN_SKEL_RIGHT_SHOULDER,	qRight);
	bones[RIGHT_ELBOW]	 .init( XN_SKEL_RIGHT_ELBOW,	qRight);
	bones[LEFT_HIP]		 .init( XN_SKEL_LEFT_HIP,		qDefault);
	bones[RIGHT_HIP]	 .init( XN_SKEL_RIGHT_HIP,		qDefault);
	bones[LEFT_KNEE]	 .init( XN_SKEL_LEFT_KNEE,		qDefault);
	bones[RIGHT_KNEE]	 .init( XN_SKEL_RIGHT_KNEE,		qDefault);
	bones[NECK]			 .init( XN_SKEL_NECK,			qNeck);
	
	// Update the bones
	updateBones();
	
    initWaistY = bones[WAIST].getPosition().y + 15;
    
	for(int i=0; i<kNumTestNodes; i++) bones[i].updateCalibPose();
	
	setBonesLengths();
    
	bBonesInit = true;
}


void UserSkeleton::setBonesLengths() {
	
	setBoneLength(NECK,			 XN_SKEL_TORSO);
	setBoneLength(LEFT_SHOULDER, XN_SKEL_LEFT_ELBOW);
	setBoneLength(LEFT_ELBOW,	 XN_SKEL_LEFT_HAND);
	setBoneLength(RIGHT_SHOULDER,XN_SKEL_RIGHT_ELBOW);
	setBoneLength(RIGHT_ELBOW,	 XN_SKEL_RIGHT_HAND);	
	setBoneLength(LEFT_HIP,		 XN_SKEL_LEFT_KNEE);
	setBoneLength(LEFT_KNEE,	 XN_SKEL_LEFT_FOOT);
	setBoneLength(RIGHT_HIP,	 XN_SKEL_RIGHT_KNEE);
	setBoneLength(RIGHT_KNEE,	 XN_SKEL_RIGHT_FOOT);
}

void UserSkeleton::setBoneLength(int boneId, XnSkeletonJoint endBone) {
	
	XnSkeletonJointPosition endPos;
	//user->getXnUserGenerator().GetSkeletonCap().GetSkeletonJointPosition(user->getTrackedUser(0)->id, endBone, endPos);
	user->getXnUserGenerator().GetSkeletonCap().GetSkeletonJointPosition(userId, endBone, endPos);
	
	ofPoint dest(endPos.position.X, endPos.position.Y, endPos.position.Z);
	ofPoint origin(bones[boneId].getPosition());
	
	bones[boneId].length = 300;//origin.distance(dest);
}

void UserSkeleton::updateBones() {
	
	for (int i=0; i<kNumTestNodes; i++) {
		
		//XnUserID userId = user->getTrackedUser(0)->id;
		
		// Get the openNI bone info	
		xn::SkeletonCapability pUserSkel = user->getXnUserGenerator().GetSkeletonCap();		
		
		XnSkeletonJointOrientation jointOri;
		pUserSkel.GetSkeletonJointOrientation(userId, bones[i].xnJointId, jointOri);
		
		XnSkeletonJointPosition jointPos;
		pUserSkel.GetSkeletonJointPosition(userId, bones[i].xnJointId, jointPos);
		
		if( jointOri.fConfidence > 0 )
		 {
			float * oriM = jointOri.orientation.elements;
			
			ofMatrix4x4 rotMatrix;
			
			// Create a 4x4 rotation matrix (converting row to column-major)
			rotMatrix.set(oriM[0], oriM[3], oriM[6], 0.0f,
						  oriM[1], oriM[4], oriM[7], 0.0f,
						  oriM[2], oriM[5], oriM[8], 0.0f,
						  0.0f, 0.0f, 0.0f, 1.0f);
			
			ofQuaternion q = rotMatrix.getRotate();
			
			bones[i].setPosition(jointPos.position.X, jointPos.position.Y, jointPos.position.Z);
			
			// apply skeleton pose relatively to the bone bind pose
			// /!\ WARNING the order of the quat' multiplication does mater!!
			bones[i].setOrientation(bones[i].bindPose.getRotate()*q);
		 }
	}
    
    waistYHistory.push_back(bones[WAIST].getPosition().y);
    
    if(waistYHistory.size()>15) {
        waistYHistory.erase(waistYHistory.begin());
    }
}

void UserSkeleton::drawBones() {
    
    for(int i=0; i<kNumTestNodes; i++) {
        bones[i].draw();
    }
}