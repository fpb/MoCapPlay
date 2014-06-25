//
//  BVHLoader.h
//  3DViewing
//
//  Created by Fernando Birra on 08/06/14.
//  Copyright (c) 2014 FCT/UNL. All rights reserved.
//

#ifndef ___DViewing__BVHLoader__
#define ___DViewing__BVHLoader__

#include <iostream>
#include <vector>

#include "bvh.h"

class BVHLoader {
private:
    std::vector<BVH *> nodes;
    bool bHierarchyLoaded;
    std::vector<std::string> *lines;
    int offset;
    int nFrames;
    float fTime;
    int currFrame;
    int motionOffset;
 
    void collectNodes(BVH &node, std::vector<BVH *> &nodes);

public:
    BVH root;
    
    BVHLoader(const std::string &filename);
    virtual ~BVHLoader();
    
    BVH loadHierarchy();
    void parseMotionData();
    bool hasMoreMotionData() const;
    void rewind();
};

#endif /* defined(___DViewing__BVHLoader__) */
