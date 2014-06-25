//
//  Phigs3DView.h
//  3DViewing
//
//  Created by Fernando Birra on 24/04/14.
//  Copyright (c) 2014 FCT/UNL. All rights reserved.
//

#ifndef ___DViewing__Phigs3DView__
#define ___DViewing__Phigs3DView__

#include <iostream>

#define GLM_FORCE_RADIANS

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "quaternion.h"

class Phigs3DView {
public:
    Phigs3DView();
    virtual ~Phigs3DView();
    
    void setVRC(const glm::vec3 &VRP, const glm::vec3 &VUP, const glm::vec3 &VPN);
    void setVolume(const glm::vec3 &PRP, float umin, float umax, float vmin, float vmax, float front, float back);
    void setPerspective(bool perspective);
    
    glm::vec3 getVRP() const;
    glm::vec3 getPRP() const;
    
    float getUmin() const;
    float getUmax() const;
    float getVmin() const;
    float getVmax() const;
    float getF() const;
    float getB() const;
    
    glm::mat4 getViewMatrix() const;
    glm::mat4 getProjectionMatrix() const;
    
    void zoomIn();
    void zoomOut();
    void rotateLeft();
    void rotateRight();
    void rotateUp();
    void rotateDown();
    
protected:
    glm::vec3 u;
    glm::vec3 v;
    glm::vec3 n;
    glm::vec3 vrp;
    glm::vec3 prp;
    
    float f, b;
    float umin, umax, vmin, vmax;
    bool isPerspective;
    
    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;
    
private:
    void buildViewMatrix();
    void buildProjectionMatrix();
    
    void rotateAroundUBy(float ammount);
    void rotateAroundVBy(float ammount);
};



#endif /* defined(___DViewing__Phigs3DView__) */
