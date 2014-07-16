//
//  quaternion.h
//  3DViewing
//
//  Created by Fernando Birra on 15/05/14.
//  Copyright (c) 2014 FCT/UNL. All rights reserved.
//

#ifndef ___DViewing__quaternion__
#define ___DViewing__quaternion__

#include <iostream>

#include <glm/glm.hpp>

class Quaternion {
public:
    Quaternion();
    Quaternion(const Quaternion &other);
    Quaternion(const glm::vec3 &v);
    Quaternion(const glm::vec3 &v, float angle);
    Quaternion(float x, float y, float z);
    Quaternion(float x, float y, float z, float angle);
    virtual ~Quaternion();
    
    // Modifiers
    void normalize();
    void invert();
    
    // Selectors
    Quaternion inverse() const;
    Quaternion conjugate() const;
    
    glm::vec3 rotate(const glm::vec3 v) const;
    glm::vec4 rotate(const glm::vec4 v) const;
    
    glm::mat3 matrix3() const;
    glm::mat4 matrix4() const;

    void getHeadingAttitudeBank(float &heading, float &attitude, float &bank) const;
    static Quaternion rotationFromTo(const glm::vec3 &from, const glm::vec3 &to);
    
public:
    float w, x, y, z;
};

// Operators
Quaternion operator +(const Quaternion &a, const Quaternion &b);
Quaternion operator *(const Quaternion &a, const Quaternion &b);
Quaternion operator *(const Quaternion &a, float f);
Quaternion operator *(float f, const Quaternion &a);
Quaternion operator /(const Quaternion &a, float f);


std::ostream& operator <<(std::ostream& os, const Quaternion &q);


// Tests
void quaternion_test_vector_rotation();

#endif /* defined(___DViewing__quaternion__) */
