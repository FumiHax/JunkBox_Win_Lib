#pragma once

#include  "common++.h"
#include  "tools++.h"

#include  "BVHTool.h"
#include  "NiJointsTool.h"


#define  BVH_ADD_JOINTNAME_TXT      "bvh_joint_name.txt"

#define  BVH_SAVE_FORMAT_QAV        0
#define  BVH_SAVE_FORMAT_SL         1
#define  BVH_SAVE_FORMAT_NI         2

//
#define add_bvh_name(p, n, j)   add_tList_node_bystr((p), (n), 0, (j), NULL, NULL, 0)


//
namespace jbxwl {

using namespace jbxl;

tList*  setBVHJointName(void);
tList*  addBVHJointName(char* fname, tList* lt);

int     BVHJoint2NiJointNum(tList* lt, Buffer joint_name);
tTree*  makeBVH_SLHierarchy(void);
tTree*  makeBVH_QAvHierarchy(void);
tTree*  makeBVH_NiHierarchy(void);

vector  getQAvBVHOffset(int joint);


}   // namespace




/*
mJointMap["mPelvis"] = "mPelvis";
mJointMap["mTorso"] = "mTorso";
mJointMap["mChest"] = "mChest";
mJointMap["mNeck"] = "mNeck";
mJointMap["mHead"] = "mHead";
mJointMap["mSkull"] = "mSkull";
mJointMap["mEyeRight"] = "mEyeRight";
mJointMap["mEyeLeft"] = "mEyeLeft";
mJointMap["mCollarLeft"] = "mCollarLeft";
mJointMap["mShoulderLeft"] = "mShoulderLeft";
mJointMap["mElbowLeft"] = "mElbowLeft";
mJointMap["mWristLeft"] = "mWristLeft";
mJointMap["mCollarRight"] = "mCollarRight";
mJointMap["mShoulderRight"] = "mShoulderRight";
mJointMap["mElbowRight"] = "mElbowRight";
mJointMap["mWristRight"] = "mWristRight";
mJointMap["mHipRight"] = "mHipRight";
mJointMap["mKneeRight"] = "mKneeRight";
mJointMap["mAnkleRight"] = "mAnkleRight";
mJointMap["mFootRight"] = "mFootRight";
mJointMap["mToeRight"] = "mToeRight";
mJointMap["mHipLeft"] = "mHipLeft";
mJointMap["mKneeLeft"] = "mKneeLeft";
mJointMap["mAnkleLeft"] = "mAnkleLeft";
mJointMap["mFootLeft"] = "mFootLeft";
mJointMap["mToeLeft"] = "mToeLeft";

mJointMap["avatar_mPelvis"] = "mPelvis";
mJointMap["avatar_mTorso"] = "mTorso";
mJointMap["avatar_mChest"] = "mChest";
mJointMap["avatar_mNeck"] = "mNeck";
mJointMap["avatar_mHead"] = "mHead";
mJointMap["avatar_mSkull"] = "mSkull";
mJointMap["avatar_mEyeRight"] = "mEyeRight";
mJointMap["avatar_mEyeLeft"] = "mEyeLeft";
mJointMap["avatar_mCollarLeft"] = "mCollarLeft";
mJointMap["avatar_mShoulderLeft"] = "mShoulderLeft";
mJointMap["avatar_mElbowLeft"] = "mElbowLeft";
mJointMap["avatar_mWristLeft"] = "mWristLeft";
mJointMap["avatar_mCollarRight"] = "mCollarRight";
mJointMap["avatar_mShoulderRight"] = "mShoulderRight";
mJointMap["avatar_mElbowRight"] = "mElbowRight";
mJointMap["avatar_mWristRight"] = "mWristRight";
mJointMap["avatar_mHipRight"] = "mHipRight";
mJointMap["avatar_mKneeRight"] = "mKneeRight";
mJointMap["avatar_mAnkleRight"] = "mAnkleRight";
mJointMap["avatar_mFootRight"] = "mFootRight";
mJointMap["avatar_mToeRight"] = "mToeRight";
mJointMap["avatar_mHipLeft"] = "mHipLeft";
mJointMap["avatar_mKneeLeft"] = "mKneeLeft";
mJointMap["avatar_mAnkleLeft"] = "mAnkleLeft";
mJointMap["avatar_mFootLeft"] = "mFootLeft";
mJointMap["avatar_mToeLeft"] = "mToeLeft";

mJointMap["hip"] = "mPelvis";
mJointMap["abdomen"] = "mTorso";
mJointMap["chest"] = "mChest";
mJointMap["neck"] = "mNeck";
mJointMap["head"] = "mHead";
mJointMap["figureHair"] = "mSkull";
mJointMap["lCollar"] = "mCollarLeft";
mJointMap["lShldr"] = "mShoulderLeft";
mJointMap["lForeArm"] = "mElbowLeft";
mJointMap["lHand"] = "mWristLeft";
mJointMap["rCollar"] = "mCollarRight";
mJointMap["rShldr"] = "mShoulderRight";
mJointMap["rForeArm"] = "mElbowRight";
mJointMap["rHand"] = "mWristRight";
mJointMap["rThigh"] = "mHipRight";
mJointMap["rShin"] = "mKneeRight";
mJointMap["rFoot"] = "mFootRight";
mJointMap["lThigh"] = "mHipLeft";
mJointMap["lShin"] = "mKneeLeft";
mJointMap["lFoot"] = "mFootLeft";
*/
