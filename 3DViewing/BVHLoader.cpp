//
//  BVHLoader.cpp
//  3DViewing
//
//  Created by Fernando Birra on 08/06/14.
//  Copyright (c) 2014 FCT/UNL. All rights reserved.
//
#include <fstream>

#include "BVHLoader.h"


BVHLoader::BVHLoader(const std::string &bvhFile)
{
    bHierarchyLoaded = false;
    currFrame = 0;
    
    std::ifstream bvhStream(bvhFile.c_str());
    if(!bvhStream) {
        std::cerr << "File not found: " << bvhFile << std::endl;
        throw -1;
    }
    
    std::istream_iterator<std::string> bvhIt(bvhStream);
    std::istream_iterator<std::string> sentinel;
    
    lines = new std::vector<std::string>(bvhIt, sentinel);
}

BVHLoader::~BVHLoader()
{
    delete lines;
}

void BVHLoader::collectNodes(BVH &node, std::vector<BVH *> &nodes) {
    nodes.push_back(&node);
    for(int i=0; i<node.children.size(); i++) {
        collectNodes(node.children[i], nodes);
    }
}

BVH BVHLoader::loadHierarchy()
{
    offset = 0;
    root.process(*lines, offset, mode::None);
    
    std::cout << lines->at(offset++) << std::endl;  // "MOTION"
    std::cout << lines->at(offset++) << std::endl;  // "Frames:"
    nFrames = atoi(lines->at(offset++).c_str());
    std::cout << nFrames << std::endl;
    std::cout << lines->at(offset++) << std::endl;
    std::cout << lines->at(offset++) << std::endl;
    fTime = atof(lines->at(offset++).c_str());
    std::cout << fTime << std::endl;
    
    bHierarchyLoaded = true;
    
    motionOffset = offset;  // Record offset for motion data
    
    collectNodes(root, nodes);
    
    return root;
}

void BVHLoader::parseMotionData()
{
    root.parseMotionFrame(*lines, offset);
    currFrame++;
}

bool BVHLoader::hasMoreMotionData() const
{
    return currFrame < nFrames;
}

void BVHLoader::rewind()
{
    offset = motionOffset;
    currFrame = 0;
}