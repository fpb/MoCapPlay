//
//  quaternion.cpp
//  3DViewing
//
//  Created by Fernando Birra on 15/05/14.
//  Copyright (c) 2014 FCT/UNL. All rights reserved.
//

#include "quaternion.h"

// Quaternion from quaternion
Quaternion::Quaternion(const Quaternion &other): w(other.w), x(other.x), y(other.y), z(other.z)
{
}

// Pure quaternion out of a vector v
Quaternion::Quaternion(const glm::vec3 &v): w(0), x(v.x), y(v.y), z(v.z)
{
    normalize();
}

Quaternion::Quaternion(const glm::vec3 &v, float angle)
{
    float len = sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
    
    float vx = v.x/len;
    float vy = v.y/len;
    float vz = v.z/len;
    
    angle = angle * M_PI/180;
    float s = sin(angle/2);
    
    w = cos(angle/2);
    x = vx * s;
    y = vy * s;
    z = vz * s;
    
    //normalize();    //???
}


Quaternion::Quaternion(float w, float x, float y, float z):w(w), x(x), y(y), z(z)
{
}


Quaternion::~Quaternion()
{
}

void Quaternion::normalize()
{
    float len = sqrt(x*x + y*y + z*z + w*w);
    
    x/=len;
    y/=len;
    z/=len;
    w/=len;
}

void Quaternion::invert()
{
    x *= -1; y*=-1; z*= -1;
}

Quaternion Quaternion::inverse() const
{
    Quaternion q(*this);
    q.invert();
    
    return q;
}

Quaternion Quaternion::conjugate() const
{
    return inverse();
}

glm::mat3 Quaternion::matrix3() const
{
    float aa = w*w;
    float bb = x*x;
    float cc = y*y;
    float dd = z*z;
    
    float ab2 = 2*w*x;
    float bc2 = 2*x*y;
    float ad2 = 2*w*z;
    float bd2 = 2*x*z;
    float ac2 = 2*w*y;
    float cd2 = 2*y*z;
    
    return glm::mat3( glm::vec3(aa + bb - cc - dd,  bc2+ad2,        bd2 - ac2  ),
                     glm::vec3(bc2-ad2,            aa-bb+cc-dd,    cd2+ab2     ),
                     glm::vec3(bd2+ac2,            cd2-ab2,        aa-bb-cc+dd )
                     );
}

void Quaternion::getHeadingAttitudeBank(float &heading, float &attitude, float &bank) const
{
    float sqw = w*w;
    float sqx = x*x;
    float sqy = y*y;
    float sqz = z*z;
    float unit = sqx + sqy + sqz + sqw; // if normalised is one, otherwise is correction factor
    float test = x*y + z*w;
    if (test > 0.499*unit) { // singularity at north pole
        heading = 2 * atan2(x,w);
        attitude = M_PI/2;
        bank = 0;
        return;
    }
    if (test < -0.499*unit) { // singularity at south pole
        heading = -2 * atan2(x,w);
        attitude = -M_PI/2;
        bank = 0;
        return;
    }
    heading = 180 * atan2(2*y*w-2*x*z , sqx - sqy - sqz + sqw) / M_PI;
    attitude = 180 * asin(2*test/unit) / M_PI;
    bank = 180 * atan2(2*x*w-2*y*z , -sqx + sqy - sqz + sqw) /M_PI;
}

glm::mat4 Quaternion::matrix4() const
{
    float aa = w*w;
    float bb = x*x;
    float cc = y*y;
    float dd = z*z;
    
    float ab2 = 2*w*x;
    float bc2 = 2*x*y;
    float ad2 = 2*w*z;
    float bd2 = 2*x*z;
    float ac2 = 2*w*y;
    float cd2 = 2*y*z;
    
    return glm::mat4( glm::vec4(aa + bb - cc - dd,  bc2+ad2,        bd2 - ac2,      0),
                      glm::vec4(bc2-ad2,            aa-bb+cc-dd,    cd2+ab2,        0),
                      glm::vec4(bd2+ac2,            cd2-ab2,        aa-bb-cc+dd,    0),
                      glm::vec4(0,                  0,              0,              1)
            );
}

glm::vec3 Quaternion::rotate(const glm::vec3 v) const
{
    Quaternion r = (*this) * Quaternion(v) * this->inverse();
    
    return glm::vec3(r.x, r.y, r.z);
}

glm::vec4 Quaternion::rotate(const glm::vec4 v) const
{
    glm::vec3 vv = (v.w != 0.0 ? glm::vec3(v.x/v.w, v.y/v.w, v.z/v.w) : glm::vec3(v.x, v.y, v.z));
    Quaternion r = (*this) * Quaternion(vv) * this->inverse();

    return glm::vec4(r.x, r.y, r.z, v.w != 0.0 ? 1.0 : 0.0);
}

Quaternion operator +(const Quaternion &a, const Quaternion &b)
{
    Quaternion result(a.w+b.w, a.x+b.x, a.y+b.y, a.z+b.z);
    
    return result;
}

Quaternion operator *(const Quaternion &a, const Quaternion &b)
{
    Quaternion result(
        a.w*b.w - a.x*b.x - a.y*b.y - a.z*b.z,      // a1a2-b1b2-c1c2-d1d2,
        a.w*b.x + a.x*b.w + a.y*b.z - a.z*b.y,      // a1b2+b1a2+c1d2-d1c2,
        a.w*b.y - a.x*b.z + a.y*b.w + a.z*b.x,      // a1c2-b1d2+c1a2+d1b2,
        a.w*b.z + a.x*b.y - a.y*b.x + a.z*b.w       // a1d2+b1c2-c1b2+d1a2
    );
    
    return result;
}

Quaternion operator *(const Quaternion &a, float f)
{
    return Quaternion(a.w*f, a.x*f, a.y*f, a.z*f);
}

Quaternion operator *(float f, const Quaternion &a)
{
    return Quaternion(a.w*f, a.x*f, a.y*f, a.z*f);
}

Quaternion operator /(const Quaternion &a, float f)
{
    return Quaternion(a.w/f, a.x/f, a.y/f, a.z/f);
}

std::ostream& operator << (std::ostream& os, const Quaternion &q) {
    return os << q.w << " " << q.x << " " << q.y << " " << q.z;
}

// Tests

void quaternion_test_vector_rotation()
{
    // Generate pure quaternion
    Quaternion x(glm::vec3(1,0,0));
    Quaternion y_axis_rot(glm::vec3(0,1,0), 90);
    Quaternion r = y_axis_rot * x * y_axis_rot.inverse();
    
    std::cout << "X axis quaternion: " << x << std::endl;
    std::cout << "Y axis rotation quaternion: " << y_axis_rot << std::endl;
    std::cout << "Rotated x (manually)" << r << std::endl;
    
    glm::vec3 rr = y_axis_rot.rotate(glm::vec3(1,0,0));
    std::cout << "Rotated x (rotate)" << rr.x << " " << rr.y << " " << rr.z << std::endl;
    
    glm::vec4 rrr = y_axis_rot.rotate(glm::vec4(1,0,0,0));
    std::cout << "Rotated x40 (rotate)" << rrr.x << " " << rrr.y << " " << rrr.z << " " << rrr.w << std::endl;

    rrr = y_axis_rot.rotate(glm::vec4(1,0,0,1));
    std::cout << "Rotated x41 (rotate)" << rrr.x << " " << rrr.y << " " << rrr.z << " " << rrr.w << std::endl;
}