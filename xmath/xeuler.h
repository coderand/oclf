// This file is distributed under a MIT license. See LICENSE.txt for details.
//
// XMath clases
//
// X Euler class ( header )
//

#ifndef __XEULER_H__
#define __XEULER_H__

#include "xmath.h"

class XVec3;
class XMat3;

class XEuler
{
public:

    float yaw;
    float pitch;
    float roll;

    // constructors
    inline XEuler() {}
    XEuler( float, float, float );
    void setYPR( float, float, float );

    // operators
    friend XEuler operator + ( const XEuler &, const XEuler & );
    friend XEuler operator - ( const XEuler &, const XEuler & );
    friend XEuler operator * ( const XEuler &, float );
    inline friend XEuler operator * ( float a, XEuler &b )
    {
        return b * a;
    }
    
    XEuler &operator += ( const XEuler & );
    XEuler &operator -= ( const XEuler & );
    XEuler &operator *= ( float );
    bool operator == ( const XEuler & ) const;
    bool operator != ( const XEuler &a ) const
    {
        return ! ( *this == a );
    }
    
    void operator = ( const XMat3 & );

    void AngleVectors( XVec3 *right, XVec3 *up, XVec3 *forward ) const;
};

// implementation

inline      XEuler::XEuler( float yy, float pp, float rr )
{
    yaw = yy;
    pitch = pp;
    roll = rr;
}

inline void XEuler::setYPR( float yy, float pp, float rr )
{
    yaw = yy;
    pitch = pp;
    roll = rr;
}

inline XEuler &XEuler::operator += ( const XEuler &a )
{
    yaw += a.yaw;
    pitch += a.pitch;
    roll += a.roll;

    return *this;
}

inline XEuler &XEuler::operator -= ( const XEuler &a )
{
    yaw -= a.yaw;
    pitch -= a.pitch;
    roll -= a.roll;

    return *this;
}

inline XEuler &XEuler::operator *= ( float f )
{
    yaw *= f;
    pitch *= f;
    roll *= f;
    return *this;
}

inline bool XEuler::operator == ( const XEuler &a ) const
{
    return ( pitch == a.pitch && roll == a.roll && yaw == a.yaw );
}

inline XEuler operator + ( const XEuler &a, const XEuler &b )
{
    XEuler c;

    c.yaw = a.yaw + b.yaw;
    c.pitch = a.pitch + b.pitch;
    c.roll = a.roll + b.roll;

    return c;
}

inline XEuler operator - ( const XEuler &a, const XEuler &b )
{
    XEuler c;

    c.yaw = a.yaw - b.yaw;
    c.pitch = a.pitch - b.pitch;
    c.roll = a.roll - b.roll;

    return c;
}

inline XEuler operator * ( const XEuler &a, float b )
{
    XEuler c;

    c.yaw = a.yaw * b;
    c.pitch = a.pitch * b;
    c.roll = a.roll * b;

    return c;
}

#endif __XEULER_H__
