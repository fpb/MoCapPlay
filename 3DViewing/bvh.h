//
//  bvh.h
//  3DViewing
//
//  Created by Fernando Birra on 28/05/14.
//  Copyright (c) 2014 FCT/UNL. All rights reserved.
//

#ifndef ___DViewing__bvh__
#define ___DViewing__bvh__

#include <iostream>
#include <vector>

#include <glm/glm.hpp>

#include "quaternion.h"

enum class BVHChannel {Xposition, Yposition, Zposition, Zrotation, Xrotation, Yrotation };
enum class mode { None, Hierarchy, Root, Name, Offset, Channels, Joint, JointList, EndSite };

class BVH {
public:
    std::string name;
    glm::vec3 offset;
    float rx, ry, rz;
    std::vector<BVH> children;
    std::vector<BVHChannel> channels;
    BVH *parent;
    Quaternion preRot;

public:
    BVH();
    virtual ~BVH();
    
    virtual void dumpTree(const std::string &prefix="") const;
    virtual void dumpPointTo() const;
    virtual void process(const std::vector<std::string> &lines, int &offset, mode currMode);
    BVH init(const std::string &bvhFile);
    int parseMotionFrame(std::vector<std::string> &lines, int &offset);
    BVHChannel channelId(std::string str) const;
    std::string channelStr(BVHChannel ch) const;
    int motionDataSize() const;
    Quaternion getLocalTransform() const;
    void dumpState(const Quaternion &q, std::ostream &output) const;
    void setPreRot(const glm::vec3 &from);
    glm::vec3 pointTo() const;
};

#endif /* defined(___DViewing__bvh__) */
