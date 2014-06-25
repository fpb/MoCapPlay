//
//  bvh.cpp
//  3DViewing
//
//  Created by Fernando Birra on 28/05/14.
//  Copyright (c) 2014 FCT/UNL. All rights reserved.
//
#include <fstream>
#include <iomanip>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "bvh.h"

BVH::BVH(): name("unnamed"), offset(0,0,0), rx(0), ry(0), rz(0)
{
}

BVH::~BVH()
{
}

void BVH::dumpTree(const std::string &prefix) const
{
    std::string newprefix = prefix + "\t";

    if(children.size()==0) {
        std::cout << prefix << "End Site" << std::endl;
        std::cout << prefix << "{" << std::endl;
        std::cout << newprefix << "OFFSET " << offset.x << " " << offset.y << " " << offset.z << std::endl;
        std::cout << prefix << "}" << std::endl;
    }
    else {
        std::cout << prefix << (prefix == "" ? "ROOT " : "JOINT ") << name << std::endl;
        std::cout << prefix << "{" << std::endl;
        std::cout << newprefix << "OFFSET " << offset.x << " " << offset.y << " " << offset.z << std::endl;
        std::cout << newprefix << "CHANNELS " << channels.size();
        for(int i=0; i<channels.size(); i++) {
            std::cout << " " << channelStr(channels[i]);
        }
        std::cout << std::endl;
        for(int i=0; i<children.size(); i++)
            children[i].dumpTree(newprefix);
        std::cout << prefix << "}" << std::endl;
    }
}


BVHChannel BVH::channelId(std::string str) const
{
    if(str=="Xposition") return BVHChannel::Xposition;
    if(str=="Yposition") return BVHChannel::Yposition;
    if(str=="Zposition") return BVHChannel::Zposition;
    if(str=="Xrotation") return BVHChannel::Xrotation;
    if(str=="Yrotation") return BVHChannel::Yrotation;
    if(str=="Zrotation") return BVHChannel::Zrotation;
    
    throw -1;
}

std::string BVH::channelStr(BVHChannel ch) const
{
    switch(ch) {
        case BVHChannel::Xposition:
            return "Xposition";
        case BVHChannel::Yposition:
            return "Yposition";
        case BVHChannel::Zposition:
            return "Zposition";
        case BVHChannel::Xrotation:
            return "Xrotation";
        case BVHChannel::Yrotation:
            return "Yrotation";
        case BVHChannel::Zrotation:
            return "Zrotation";
        default:
            throw -1;
    }
}

void BVH::process(const std::vector<std::string> &lines, int &offset, mode currMode)
{
    std::string line = lines[offset++];
    
    if(line == "HIERARCHY") {
        if(currMode != mode::None)
            throw -1;
        process(lines, offset, mode::Hierarchy);
    }
    else if(line == "ROOT") {
        if(currMode != mode::Hierarchy) {
            throw -1;
        }
        process(lines, offset, mode::Root);
    }
    else if(line == "JOINT") {
        if(currMode != mode::JointList) {
            throw -1;
        }
        BVH joint;
        joint.parent = this;
        joint.process(lines, offset, mode::Joint);
        children.push_back(joint);
        process(lines, offset, mode::JointList);
    }
    else if(line == "OFFSET") {
        if(currMode!=mode::Name && currMode!=mode::EndSite) {
            throw -1;
        }
        if(currMode == mode::EndSite) {
            process(lines, offset, mode::Offset);
        }
        else process(lines, offset, mode::Offset);
    }
    else if(line == "CHANNELS") {
        if(currMode!=mode::None)
            throw -1;
        process(lines, offset, mode::Channels);
    }
    else if(line == "End") {
        if(lines[offset++] == "Site") {
            BVH end;
            end.parent = this;
            end.process(lines, offset, mode::EndSite);
            children.push_back(end);
            process(lines, offset, mode::JointList);
        }
        else throw -1;
    }
    else if(line == "{") {
        if(currMode != mode::Name && currMode != mode::EndSite)
            throw -1;
        // nothing to do
        process(lines, offset, currMode);
    }
    else if(line == "}") {
        if(currMode == mode::JointList) { // ?????
            //
        }
        return ;
    }
    else {
        switch(currMode) {
            case mode::Root:
                name = line;
                process(lines, offset, mode::Name);
                break;
            case mode::Joint:
                name = line;
                process(lines, offset, mode::Name);
                break;
            case mode::Offset:
            {
                
                float x = atof(line.c_str());
                float y = atof(lines[offset++].c_str());
                float z = atof(lines[offset++].c_str());
                this->offset = glm::vec3(x,y,z);
                //std::cout << root.offset.x << ":" << root.offset.y << ":" << root.offset.z << std::endl;
                process(lines, offset, mode::None);
                //std::cout << this->offset.x << ":" << this->offset.y << ":" << this->offset.z << std::endl;
            }
                break;
            case mode::None:
                break;
            case mode::Hierarchy:
                break;
            case mode::Channels:
            {
                float n = atoi(line.c_str());
                for(int i=0; i<n; i++) {
                    BVHChannel channel=channelId(lines[offset++]);
                    channels.push_back(channel);
                }
                process(lines, offset, mode::JointList);
            }
                break;
            case mode::EndSite:
            {
                throw -1;
            }
                
        }
    }
}

BVH BVH::init(const std::string &bvhFile) {
    
    std::ifstream bvhStream(bvhFile.c_str());
    if(!bvhStream) {
        std::cerr << "File not found: " << bvhFile << std::endl;
        throw -1;
    }
    
    std::istream_iterator<std::string> bvhIt(bvhStream);
    std::istream_iterator<std::string> sentinel;
    
    std::vector<std::string> lines(bvhIt,sentinel);
    
    int offset = 0;
    process(lines, offset, mode::None);
    
    std::cout << lines[offset++] << std::endl;  // "MOTION"
    std::cout << lines[offset++] << std::endl;  // "Frames:"
    int nFrames = atoi(lines[offset++].c_str());
    std::cout << nFrames << std::endl;
    std::cout << lines[offset++] << std::endl;
    std::cout << lines[offset++] << std::endl;
    float fTime = atof(lines[offset++].c_str());
    std::cout << fTime << std::endl;
    for(int i=0; i<nFrames; i++) {
        parseMotionFrame(lines, offset);
    }

    return *this;
}

int BVH::parseMotionFrame(std::vector<std::string> &lines, int &offset)
{
    float v;
    for(int i=0; i<this->channels.size(); i++) {
        v = atof(lines[offset++].c_str());
        switch(channels[i]) {
            case BVHChannel::Xposition:
                this->offset.x = v;
                break;
            case BVHChannel::Yposition:
                this->offset.y = v;
                break;
            case BVHChannel::Zposition:
                this->offset.z = v;
                break;
            case BVHChannel::Xrotation:
                rx = v;
                break;
            case BVHChannel::Yrotation:
                ry = v;
                break;
            case BVHChannel::Zrotation:
                rz = v;
                break;
        }
    }
    for(int i=0; i<children.size(); i++) {
        children[i].parseMotionFrame(lines, offset);
    }
    
    return offset;
}

int BVH::motionDataSize() const
{
    int s = channels.size();
    
    for(int i=0; i<children.size(); i++) {
        s += children[i].motionDataSize();
    }
    
    return s;
}

Quaternion BVH::getLocalTransform() const
{
    Quaternion qx(glm::vec3(1,0,0),rx), qy(glm::vec3(0,1,0),ry), qz(glm::vec3(0,0,1),rz);

    Quaternion q(glm::vec3(1,0,0), 0);

    for(int i=0; i<channels.size(); i++)
    {
                if(channels[i] == BVHChannel::Xrotation) q = q * qx;
                if(channels[i] == BVHChannel::Yrotation) q = q * qy;
                if(channels[i] == BVHChannel::Zrotation) q = q * qz;
//                if(channels[i] == BVHChannel::Xrotation) q = qx * q;
//                if(channels[i] == BVHChannel::Yrotation) q = qy * q;
//                if(channels[i] == BVHChannel::Zrotation) q = qz * q;
    }
    return q;
}

void BVH::dumpState(const Quaternion &q, std::ostream &output) const
{
    // qz * qx * qy
    Quaternion qql = getLocalTransform();
    Quaternion qqg = q * qql;
    output << "\"" << std::setw(16) << std::left << name << "\"";
    output << std::setprecision(3) << std::fixed;
    output << "\t(w,x,y,z)=" << "(" << std::setw(6) << std::right << qqg.w << "," << std::setw(6) << qqg.x << "," << std::setw(6) << qqg.y << "," << std::setw(6) << qqg.z << ")";
    float ex, ey, ez;
    qql.getHeadingAttitudeBank(ex, ey, ez);
    output << "\t rx=" << ex << " ry=" << ey << " rz=" << ez;
    output << std::endl;
    //output << "Local (w,x,y,z)=" << "(" << qql.w << "," << qql.x << "," << qql.y << "," << qql.z << ")" << std::endl;
    //output << "(rx,ry,rz)=" << "(" << rx << "," << ry << "," << rz << ")" << std::endl;

    for(int i=0; i<children.size(); i++) {
        children[i].dumpState(q, output);
    }
}

