//
//  Phigs3DView.cpp
//  3DViewing
//
//  Created by Fernando Birra on 24/04/14.
//  Copyright (c) 2014 FCT/UNL. All rights reserved.
//

#include "Phigs3DView.h"

Phigs3DView::Phigs3DView():vrp(0,0,0), u(1,0,0), v(0,1,0), n(0,0,1), umin(-1), umax(1), vmin(-1), vmax(1), f(0.5), b(-1), prp(0,0,1), isPerspective(false)
{
}

Phigs3DView::~Phigs3DView()
{
}

void Phigs3DView::setVRC(const glm::vec3 &VRP, const glm::vec3 &VUP, const glm::vec3 &VPN)
{
    vrp = VRP;
    
    u = glm::normalize(glm::cross(VUP, VPN));
    n = glm::normalize(VPN);
    v = glm:: cross(n, u);
    
}

void Phigs3DView::setVolume(const glm::vec3 &PRP, float umin, float umax, float vmin, float vmax, float front, float back)
{
    prp = PRP;
    f = front; b = back;
    this->umin = umin; this->umax = umax; this->vmin = vmin; this->vmax = vmax;
    
    buildViewMatrix();
    buildProjectionMatrix();
}


glm::mat4 Phigs3DView::getViewMatrix() const
{
    return viewMatrix;
}

glm::mat4 Phigs3DView::getProjectionMatrix() const
{
    return projectionMatrix;
}

glm::vec3 Phigs3DView::getVRP() const
{
    return vrp;
}

glm::vec3 Phigs3DView::getPRP() const
{
    return prp;
}

float Phigs3DView::getUmax() const
{
    return umax;
}

float Phigs3DView::getUmin() const
{
    return umin;
}

float Phigs3DView::getVmin() const
{
    return vmin;
}

float Phigs3DView::getVmax() const
{
    return vmax;
}

float Phigs3DView::getF() const
{
    return f;
}

float Phigs3DView::getB() const
{
    return b;
}

void Phigs3DView::setPerspective(bool perspective)
{
    isPerspective = perspective;
    
    buildProjectionMatrix();
}

void Phigs3DView::buildViewMatrix()
{
    // T(-PRP).R.T(-VRP)
    
    glm::mat4 T = glm::translate(glm::mat4 (1.0), -vrp);
    
    glm::mat4 R(glm::mat3x3(u,v,n));
    
    R = glm::transpose(R);

    glm::mat4 Tp = glm::translate(glm::mat4(1.0), -prp);
 
    viewMatrix = Tp * R * T;
}

/*
void Phigs3DView::buildViewMatrix()
{
    glm::mat4 T = glm::translate(glm::mat4 (1.0), vrp);
    glm::mat4 R(glm::mat3x3(u,v,n));
    glm::vec4 eye = T * R * glm::vec4(prp,1);

    viewMatrix = glm::lookAt(glm::vec3(eye), vrp, v);
}
*/

void Phigs3DView::buildProjectionMatrix()
{
    float fovy = 2*atan(0.5*(vmax-vmin)/prp[2]);
    float aspect = (umax-umin)/(vmax-vmin);
    float near = prp[2]-f;
    float far = prp[2]-b;
    projectionMatrix = glm::perspective(fovy, aspect, near, far);
}

void Phigs3DView::rotateAroundUBy(float ammount)
{
    Quaternion ru(u, ammount);
    glm::mat3 mru = ru.matrix3();
    
    v = mru * v;
    n = mru * n;
    
    buildViewMatrix();
}

void Phigs3DView::rotateUp()
{
    rotateAroundUBy(-2);
}

void Phigs3DView::rotateDown()
{
    rotateAroundUBy(2);
}

void Phigs3DView::rotateAroundVBy(float ammount)
{
    Quaternion rv(v, ammount);
    glm::mat3 mrv = rv.matrix3();
    
    u = mrv * u;
    n = mrv * n;
    
    buildViewMatrix();
}

void Phigs3DView::rotateLeft()
{
    rotateAroundVBy(-2);
}

void Phigs3DView::rotateRight()
{
    rotateAroundVBy(2);
}

void Phigs3DView::zoomIn()
{
    umin *= 1.05;   umax *= 1.05;
    vmin *= 1.05;   vmax *= 1.05;
    buildProjectionMatrix();
}

void Phigs3DView::zoomOut()
{
    umin /= 1.05;   umax /= 1.05;
    vmin /= 1.05;   vmax /= 1.05;
    buildProjectionMatrix();
}