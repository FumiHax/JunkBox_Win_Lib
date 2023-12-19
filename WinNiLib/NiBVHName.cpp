
#include "NiBVHName.h"

using namespace jbxl;
using namespace jbxwl;


/**
@see llfloatermodelpreview.cpp
*/
tList*   jbxwl::setBVHJointName(void)
{
    tList* lt = add_bvh_name(NULL, -1, "BVH Name List");
    tList* pp = lt; 

    // NI: PELVIS, WAIST, TORSO, ....., R_TOE   see NiJointsTool.cpp
    for (int j=0; j<NI_NORML_JOINT_NUM; j++) {
        pp = add_bvh_name(pp, j, (char*)NiJointName(j).c_str());
    }

    // SL/OS
    pp = add_bvh_name(pp, NI_PELVIS,   "mPelvis");
    pp = add_bvh_name(pp, NI_TORSO,    "mTorso");
    pp = add_bvh_name(pp, NI_CHEST,    "mChest");
    pp = add_bvh_name(pp, NI_NECK,     "mNeck");
    pp = add_bvh_name(pp, NI_HEAD,     "mHead");
    pp = add_bvh_name(pp, NI_SKULL,    "mSkull");
    pp = add_bvh_name(pp, NI_L_EYE,    "mEyeLeft");
    pp = add_bvh_name(pp, NI_R_EYE,    "mEyeRight");
    pp = add_bvh_name(pp, NI_L_COLLAR, "mCollarLeft");
    pp = add_bvh_name(pp, NI_L_SHLDR,  "mShoulderLeft");
    pp = add_bvh_name(pp, NI_L_ELBOW,  "mElbowLeft");
    pp = add_bvh_name(pp, NI_L_WRIST,  "mWristLeft");
    pp = add_bvh_name(pp, NI_R_COLLAR, "mCollarRight");
    pp = add_bvh_name(pp, NI_R_SHLDR,  "mShoulderRight");
    pp = add_bvh_name(pp, NI_R_ELBOW,  "mElbowRight");
    pp = add_bvh_name(pp, NI_R_WRIST,  "mWristRight");
    pp = add_bvh_name(pp, NI_L_HIP,    "mHipLeft");
    pp = add_bvh_name(pp, NI_L_KNEE,   "mKneeLeft");
    pp = add_bvh_name(pp, NI_L_ANKLE,  "mAnkleLeft");
    pp = add_bvh_name(pp, NI_L_FOOT,   "mFootLeft");
    pp = add_bvh_name(pp, NI_L_TOE,    "mToeLeft");
    pp = add_bvh_name(pp, NI_R_HIP,    "mHipRight");
    pp = add_bvh_name(pp, NI_R_KNEE,   "mKneeRight");
    pp = add_bvh_name(pp, NI_R_ANKLE,  "mAnkleRight");
    pp = add_bvh_name(pp, NI_R_FOOT,   "mFootRight");
    pp = add_bvh_name(pp, NI_R_TOE,    "mToeRight");

    //
    pp = add_bvh_name(pp, NI_PELVIS,   "avatar_mPelvis");
    pp = add_bvh_name(pp, NI_TORSO,    "avatar_mTorso");
    pp = add_bvh_name(pp, NI_CHEST,    "avatar_mChest");
    pp = add_bvh_name(pp, NI_NECK,     "avatar_mNeck");
    pp = add_bvh_name(pp, NI_HEAD,     "avatar_mHead");
    pp = add_bvh_name(pp, NI_SKULL,    "avatar_mSkull");
    pp = add_bvh_name(pp, NI_L_EYE,    "avatar_mEyeLeft");
    pp = add_bvh_name(pp, NI_R_EYE,    "avatar_mEyeRight");
    pp = add_bvh_name(pp, NI_L_COLLAR, "avatar_mCollarLeft");
    pp = add_bvh_name(pp, NI_L_SHLDR,  "avatar_mShoulderLeft");
    pp = add_bvh_name(pp, NI_L_ELBOW,  "avatar_mElbowLeft");
    pp = add_bvh_name(pp, NI_L_WRIST,  "avatar_mWristLeft");
    pp = add_bvh_name(pp, NI_R_COLLAR, "avatar_mCollarRight");
    pp = add_bvh_name(pp, NI_R_SHLDR,  "avatar_mShoulderRight");
    pp = add_bvh_name(pp, NI_R_ELBOW,  "avatar_mElbowRight");
    pp = add_bvh_name(pp, NI_R_WRIST,  "avatar_mWristRight");
    pp = add_bvh_name(pp, NI_L_HIP,    "avatar_mHipLeft");
    pp = add_bvh_name(pp, NI_L_KNEE,   "avatar_mKneeLeft");
    pp = add_bvh_name(pp, NI_L_ANKLE,  "avatar_mAnkleLeft");
    pp = add_bvh_name(pp, NI_L_FOOT,   "avatar_mFootLeft");
    pp = add_bvh_name(pp, NI_L_TOE,    "avatar_mToeLeft");
    pp = add_bvh_name(pp, NI_R_HIP,    "avatar_mHipRight");
    pp = add_bvh_name(pp, NI_R_KNEE,   "avatar_mKneeRight");
    pp = add_bvh_name(pp, NI_R_ANKLE,  "avatar_mAnkleRight");
    pp = add_bvh_name(pp, NI_R_FOOT,   "avatar_mFootRight");
    pp = add_bvh_name(pp, NI_R_TOE,    "avatar_mToeRight");
    
    // QAv
    pp = add_bvh_name(pp, NI_PELVIS,   "hip");
    pp = add_bvh_name(pp, NI_TORSO,    "abdomen");
    pp = add_bvh_name(pp, NI_CHEST,    "chest");
    pp = add_bvh_name(pp, NI_NECK,     "neck");
    pp = add_bvh_name(pp, NI_HEAD,     "head");
    pp = add_bvh_name(pp, NI_L_COLLAR, "lCollar");
    pp = add_bvh_name(pp, NI_L_SHLDR,  "lShldr");
    pp = add_bvh_name(pp, NI_L_ELBOW,  "lForeArm");
    pp = add_bvh_name(pp, NI_L_WRIST,  "lHand");
    pp = add_bvh_name(pp, NI_R_COLLAR, "rCollar");
    pp = add_bvh_name(pp, NI_R_SHLDR,  "rShldr");
    pp = add_bvh_name(pp, NI_R_ELBOW,  "rForeArm");
    pp = add_bvh_name(pp, NI_R_WRIST,  "rHand");
    pp = add_bvh_name(pp, NI_L_HIP,    "lThigh");
    pp = add_bvh_name(pp, NI_L_KNEE,   "lShin");
    pp = add_bvh_name(pp, NI_L_FOOT,   "lFoot");
    pp = add_bvh_name(pp, NI_R_HIP,    "rThigh");
    pp = add_bvh_name(pp, NI_R_KNEE,   "rShin");
    pp = add_bvh_name(pp, NI_R_FOOT,   "rFoot");

    return lt;
}



tList*  jbxwl::addBVHJointName(char* fname, tList* nl)
{
    tList* pp = dup_tList(nl);
    tList* lt = pp;

    tList* tl = read_index_tList_file(fname, ' ');
    if (tl==NULL) return pp;

    //
    // PELVIS, WAIST, TORSO, ....., R_TOE   see NiJointsTool.cpp
    for (int j=0; j<NI_NORML_JOINT_NUM; j++) {
        char* joint_name = get_str_param_tList(tl, (char*)NiJointName(j).c_str(), NULL);
        if (joint_name!=NULL) {
            pp = add_bvh_name(pp, j, joint_name);
            free(joint_name);
        }
    }

    del_tList(&tl);

    return lt;
}



int  jbxwl::BVHJoint2NiJointNum(tList* lt, Buffer joint_name)
{
    tList* pp = lt;

    if (pp==NULL) return -1;

    while(pp!=NULL) {
        if (!strcasecmp((char*)joint_name.buf, (char*)pp->ldat.key.buf)) {
            return pp->ldat.id;
        }
        pp = pp->next;
    }

    return -1;
}




//////////////////////////////////////////////////////////////////////////////////////////////////////
// Hierarchy

tTree*  jbxwl::makeBVH_SLHierarchy(void)
{
    tTree* root = NULL;
    tTree* chst = NULL;
    tTree* head = NULL;
    tTree* tree = NULL;

    root = add_tTree_node_bystr(NULL, BVH_NODE_ROOT,  6, "hip",        "PXPYPZRYRXRZ", NULL, 0);

    tree = add_tTree_node_bystr(root, BVH_NODE_JOINT, 3, "abdomen",    "RYRXRZ", NULL, 0);
    chst = add_tTree_node_bystr(tree, BVH_NODE_JOINT, 3, "chest",      "RYRXRZ", NULL, 0);

    tree = add_tTree_node_bystr(chst, BVH_NODE_JOINT, 3, "neck",       "RYRXRZ", NULL, 0);
    head = add_tTree_node_bystr(tree, BVH_NODE_JOINT, 3, "head",       "RYRXRZ", NULL, 0);

    tree = add_tTree_node_bystr(head, BVH_NODE_JOINT, 3, "mSkull",     "RYRXRZ", NULL, 0);
    tree = add_tTree_node_bystr(tree, BVH_NODE_END,   0, "Site",        NULL,    NULL, 0);

    tree = add_tTree_node_bystr(head, BVH_NODE_JOINT, 3, "mEyeLeft",   "RYRXRZ", NULL, 0);
    tree = add_tTree_node_bystr(tree, BVH_NODE_END,   0, "Site",        NULL,    NULL, 0);
    
    tree = add_tTree_node_bystr(head, BVH_NODE_JOINT, 3, "mEyeRight",  "RYRXRZ", NULL, 0);
    tree = add_tTree_node_bystr(tree, BVH_NODE_END,   0, "Site",        NULL,    NULL, 0);

    tree = add_tTree_node_bystr(chst, BVH_NODE_JOINT, 3, "lCollar",    "RYRXRZ", NULL, 0);
    tree = add_tTree_node_bystr(tree, BVH_NODE_JOINT, 3, "lShldr",     "RYRXRZ", NULL, 0);
    tree = add_tTree_node_bystr(tree, BVH_NODE_JOINT, 3, "lForeArm",   "RYRXRZ", NULL, 0);
    tree = add_tTree_node_bystr(tree, BVH_NODE_JOINT, 3, "lHand",      "RYRXRZ", NULL, 0);
    tree = add_tTree_node_bystr(tree, BVH_NODE_END,  0, "Site",         NULL,    NULL, 0);

    tree = add_tTree_node_bystr(chst, BVH_NODE_JOINT, 3, "rCollar",    "RYRXRZ", NULL, 0);
    tree = add_tTree_node_bystr(tree, BVH_NODE_JOINT, 3, "rShldr",     "RYRXRZ", NULL, 0);
    tree = add_tTree_node_bystr(tree, BVH_NODE_JOINT, 3, "rForeArm",   "RYRXRZ", NULL, 0);
    tree = add_tTree_node_bystr(tree, BVH_NODE_JOINT, 3, "rHand",      "RYRXRZ", NULL, 0);
    tree = add_tTree_node_bystr(tree, BVH_NODE_END,   0, "Site",        NULL,    NULL, 0);

    tree = add_tTree_node_bystr(root, BVH_NODE_JOINT, 3, "lThigh",     "RYRXRZ", NULL, 0);
    tree = add_tTree_node_bystr(tree, BVH_NODE_JOINT, 3, "lShin",      "RYRXRZ", NULL, 0);
    tree = add_tTree_node_bystr(tree, BVH_NODE_JOINT, 3, "mAnkleLeft", "RYRXRZ", NULL, 0);
    tree = add_tTree_node_bystr(tree, BVH_NODE_JOINT, 3, "lFoot",      "RYRXRZ", NULL, 0);
    tree = add_tTree_node_bystr(tree, BVH_NODE_JOINT, 3, "mToeLeft",   "RYRXRZ", NULL, 0);
    tree = add_tTree_node_bystr(tree, BVH_NODE_END,   0, "Site",        NULL,    NULL, 0);

    tree = add_tTree_node_bystr(root, BVH_NODE_JOINT, 3, "rThigh",     "RYRXRZ", NULL, 0);
    tree = add_tTree_node_bystr(tree, BVH_NODE_JOINT, 3, "rShin",      "RYRXRZ", NULL, 0);
    tree = add_tTree_node_bystr(tree, BVH_NODE_JOINT, 3, "mAnkleRight","RYRXRZ", NULL, 0);
    tree = add_tTree_node_bystr(tree, BVH_NODE_JOINT, 3, "rFoot",      "RYRXRZ", NULL, 0);
    tree = add_tTree_node_bystr(tree, BVH_NODE_JOINT, 3, "mToeRight",  "RYRXRZ", NULL, 0);
    tree = add_tTree_node_bystr(tree, BVH_NODE_END,   0, "Site",        NULL,    NULL, 0);

    return root;
}



tTree*  jbxwl::makeBVH_QAvHierarchy(void)
{
    tTree* root = NULL;
    tTree* chst = NULL;
    tTree* tree = NULL;

    root = add_tTree_node_bystr(NULL, BVH_NODE_ROOT,  6, "hip", "PXPYPZRYRXRZ", NULL, 0);

    tree = add_tTree_node_bystr(root, BVH_NODE_JOINT, 3, "abdomen",   "RYRXRZ", NULL, 0);
    chst = add_tTree_node_bystr(tree, BVH_NODE_JOINT, 3, "chest",     "RYRXRZ", NULL, 0);

    tree = add_tTree_node_bystr(chst, BVH_NODE_JOINT, 3, "neck",      "RYRXRZ", NULL, 0);
    tree = add_tTree_node_bystr(tree, BVH_NODE_JOINT, 3, "head",      "RYRXRZ", NULL, 0);
    tree = add_tTree_node_bystr(tree, BVH_NODE_END,   0, "Site",       NULL,    NULL, 0);

    tree = add_tTree_node_bystr(chst, BVH_NODE_JOINT, 3, "lCollar",   "RYRXRZ", NULL, 0);
    tree = add_tTree_node_bystr(tree, BVH_NODE_JOINT, 3, "lShldr",    "RYRXRZ", NULL, 0);
    tree = add_tTree_node_bystr(tree, BVH_NODE_JOINT, 3, "lForeArm",  "RYRXRZ", NULL, 0);
    tree = add_tTree_node_bystr(tree, BVH_NODE_JOINT, 3, "lHand",     "RYRXRZ", NULL, 0);
    tree = add_tTree_node_bystr(tree, BVH_NODE_END,   0, "Site",       NULL,    NULL, 0);
    
    tree = add_tTree_node_bystr(chst, BVH_NODE_JOINT, 3, "rCollar",   "RYRXRZ", NULL, 0);
    tree = add_tTree_node_bystr(tree, BVH_NODE_JOINT, 3, "rShldr",    "RYRXRZ", NULL, 0);
    tree = add_tTree_node_bystr(tree, BVH_NODE_JOINT, 3, "rForeArm",  "RYRXRZ", NULL, 0);
    tree = add_tTree_node_bystr(tree, BVH_NODE_JOINT, 3, "rHand",     "RYRXRZ", NULL, 0);
    tree = add_tTree_node_bystr(tree, BVH_NODE_END,   0, "Site",       NULL,    NULL, 0);
    
    tree = add_tTree_node_bystr(root, BVH_NODE_JOINT, 3, "lThigh",    "RYRXRZ", NULL, 0);
    tree = add_tTree_node_bystr(tree, BVH_NODE_JOINT, 3, "lShin",     "RYRXRZ", NULL, 0);
    tree = add_tTree_node_bystr(tree, BVH_NODE_JOINT, 3, "lFoot",     "RYRXRZ", NULL, 0);
    tree = add_tTree_node_bystr(tree, BVH_NODE_END,   0, "Site",       NULL,    NULL, 0);

    tree = add_tTree_node_bystr(root, BVH_NODE_JOINT, 3, "rThigh",    "RYRXRZ", NULL, 0);
    tree = add_tTree_node_bystr(tree, BVH_NODE_JOINT, 3, "rShin",     "RYRXRZ", NULL, 0);
    tree = add_tTree_node_bystr(tree, BVH_NODE_JOINT, 3, "rFoot",     "RYRXRZ", NULL, 0);
    tree = add_tTree_node_bystr(tree, BVH_NODE_END,   0, "Site",       NULL,    NULL, 0);

    return root;
}



tTree*  jbxwl::makeBVH_NiHierarchy(void)
{
    tTree* root = NULL;
    tTree* chst = NULL;
    tTree* head = NULL;
    tTree* tree = NULL;

    root = add_tTree_node_bystr(NULL, BVH_NODE_ROOT,  6, "PELVIS", "PXPYPZRYRXRZ", NULL, 0);

    tree = add_tTree_node_bystr(root, BVH_NODE_JOINT, 3, "WAIST",    "RYRXRZ", NULL, 0);
    tree = add_tTree_node_bystr(tree, BVH_NODE_JOINT, 3, "TORSO",    "RYRXRZ", NULL, 0);
    chst = add_tTree_node_bystr(tree, BVH_NODE_JOINT, 3, "CHEST",    "RYRXRZ", NULL, 0);

    tree = add_tTree_node_bystr(chst, BVH_NODE_JOINT, 3, "NECK",     "RYRXRZ", NULL, 0);
    head = add_tTree_node_bystr(tree, BVH_NODE_JOINT, 3, "HEAD",     "RYRXRZ", NULL, 0);

    tree = add_tTree_node_bystr(head, BVH_NODE_JOINT, 3, "SKULL",    "RYRXRZ", NULL, 0);
    tree = add_tTree_node_bystr(tree, BVH_NODE_END,   0, "Site",      NULL,    NULL, 0);

    tree = add_tTree_node_bystr(head, BVH_NODE_JOINT, 3, "L_EYE",    "RYRXRZ", NULL, 0);
    tree = add_tTree_node_bystr(tree, BVH_NODE_END,   0, "Site",      NULL,    NULL, 0);
    tree = add_tTree_node_bystr(head, BVH_NODE_JOINT, 3, "R_EYE",    "RYRXRZ", NULL, 0);
    tree = add_tTree_node_bystr(tree, BVH_NODE_END,   0, "Site",      NULL,    NULL, 0);

    tree = add_tTree_node_bystr(chst, BVH_NODE_JOINT, 3, "L_BUST",   "RYRXRZ", NULL, 0);
    tree = add_tTree_node_bystr(tree, BVH_NODE_END,   0, "Site",      NULL,    NULL, 0);
    tree = add_tTree_node_bystr(chst, BVH_NODE_JOINT, 3, "R_BUST",   "RYRXRZ", NULL, 0);
    tree = add_tTree_node_bystr(tree, BVH_NODE_END,   0, "Site",      NULL,    NULL, 0);

    tree = add_tTree_node_bystr(chst, BVH_NODE_JOINT, 3, "L_COLLAR", "RYRXRZ", NULL, 0);
    tree = add_tTree_node_bystr(tree, BVH_NODE_JOINT, 3, "L_SHLDR",  "RYRXRZ", NULL, 0);
    tree = add_tTree_node_bystr(tree, BVH_NODE_JOINT, 3, "L_ELBOW",  "RYRXRZ", NULL, 0);
    tree = add_tTree_node_bystr(tree, BVH_NODE_JOINT, 3, "L_WRIST",  "RYRXRZ", NULL, 0);
    tree = add_tTree_node_bystr(tree, BVH_NODE_JOINT, 3, "L_HAND",   "RYRXRZ", NULL, 0);
    tree = add_tTree_node_bystr(tree, BVH_NODE_JOINT, 3, "L_FNGRTIP","RYRXRZ", NULL, 0);
    tree = add_tTree_node_bystr(tree, BVH_NODE_END,   0, "Site",      NULL,    NULL, 0);

    tree = add_tTree_node_bystr(chst, BVH_NODE_JOINT, 3, "R_COLLAR", "RYRXRZ", NULL, 0);
    tree = add_tTree_node_bystr(tree, BVH_NODE_JOINT, 3, "R_SHLDR",  "RYRXRZ", NULL, 0);
    tree = add_tTree_node_bystr(tree, BVH_NODE_JOINT, 3, "R_ELBOW",  "RYRXRZ", NULL, 0);
    tree = add_tTree_node_bystr(tree, BVH_NODE_JOINT, 3, "R_WRIST",  "RYRXRZ", NULL, 0);
    tree = add_tTree_node_bystr(tree, BVH_NODE_JOINT, 3, "R_HAND",   "RYRXRZ", NULL, 0);
    tree = add_tTree_node_bystr(tree, BVH_NODE_JOINT, 3, "R_FNGRTIP","RYRXRZ", NULL, 0);
    tree = add_tTree_node_bystr(tree, BVH_NODE_END,   0, "Site",      NULL,    NULL, 0);

    tree = add_tTree_node_bystr(root, BVH_NODE_JOINT, 3, "L_HIP",    "RYRXRZ", NULL, 0);
    tree = add_tTree_node_bystr(tree, BVH_NODE_JOINT, 3, "L_KNEE",   "RYRXRZ", NULL, 0);
    tree = add_tTree_node_bystr(tree, BVH_NODE_JOINT, 3, "L_ANKLE",  "RYRXRZ", NULL, 0);
    tree = add_tTree_node_bystr(tree, BVH_NODE_JOINT, 3, "L_FOOT",   "RYRXRZ", NULL, 0);
    tree = add_tTree_node_bystr(tree, BVH_NODE_JOINT, 3, "L_TOE",    "RYRXRZ", NULL, 0);
    tree = add_tTree_node_bystr(tree, BVH_NODE_END,   0, "Site",      NULL,    NULL, 0);

    tree = add_tTree_node_bystr(root, BVH_NODE_JOINT, 3, "R_HIP",    "RYRXRZ", NULL, 0);
    tree = add_tTree_node_bystr(tree, BVH_NODE_JOINT, 3, "R_KNEE",   "RYRXRZ", NULL, 0);
    tree = add_tTree_node_bystr(tree, BVH_NODE_JOINT, 3, "R_ANKLE",  "RYRXRZ", NULL, 0);
    tree = add_tTree_node_bystr(tree, BVH_NODE_JOINT, 3, "R_FOOT",   "RYRXRZ", NULL, 0);
    tree = add_tTree_node_bystr(tree, BVH_NODE_JOINT, 3, "R_TOE",    "RYRXRZ", NULL, 0);
    tree = add_tTree_node_bystr(tree, BVH_NODE_END,   0, "Site",      NULL,    NULL, 0);

    return root;
}



double _QAV_Offset[] = 
{
      0.000000,   0.000000,   0.000000,     // ROOT hip
      0.000000,   3.422050,   0.000000,     // JOINT abdomen
      0.000000,   8.486693,  -0.684411,     // JOINT chest
      0.000000,  10.266162,  -0.273764,     // JOINT neck
      0.000000,   3.148285,   0.000000,     // JOINT head
      0.000000,   3.148289,   0.000000,     // End Site
      3.422053,   6.707223,  -0.821293,     // JOINT lCollar
      3.285171,   0.000000,   0.000000,     // JOINT lShldr
     10.129278,   0.000000,   0.000000,     // JOINT lForeArm
      8.486692,   0.000000,   0.000000,     // JOINT lHand
      4.106464,   0.000000,   0.000000,     // End Site
     -3.558935,   6.707223,  -0.821293,     // JOINT rCollar
     -3.148289,   0.000000,   0.000000,     // JOINT rShldr
    -10.266159,   0.000000,   0.000000,     // JOINT rForeArm
     -8.349810,   0.000000,   0.000000,     // JOINT rHand
     -4.106464,   0.000000,   0.000000,     // End Site
      5.338403,  -1.642589,   1.368821,     // JOINT lThigh
     -2.053232, -20.121670,   0.000000,     // JOINT lShin
      0.000000, -19.300380,  -1.231939,     // JOINT lFoot
      0.000000,  -2.463878,   4.653993,     // End Site
     -5.338403,  -1.642589,   1.368821,     // OINT rThigh
      2.053232,  -20.121670,  0.000000,     // JOINT rShin
      0.000000,  -19.300380, -1.231939,     // JOINT rFoot
      0.000000,  -2.463878,   4.653993      // End Site
};



vector  jbxwl::getQAvBVHOffset(int joint)
{
    vector vect;
    memset(&vect, 0, sizeof(vect));

    if (joint<0) return vect;
    
    vect.x = _QAV_Offset[joint*3];
    vect.y = _QAV_Offset[joint*3+1];
    vect.z = _QAV_Offset[joint*3+2];

    return vect;
}



