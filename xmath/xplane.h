// This file is distributed under a MIT license. See LICENSE.txt for details.
//
// XMath clases
//
// X Plane class ( header )
//

#ifndef __XPLANE_H__
#define __XPLANE_H__

#include "xmath.h"
#include "xvec3.h"

class XPlane 
{
public:

    // constructors
    XPlane();
    XPlane( const XVec3 &vec, float dist = 0.f );
    XPlane( const XPlane & );
    XPlane( const XVec3 &point, const XVec3 &normal );

    XVec3 n;
    float d;

    // methods
    float   DistanceFromPlane( const XVec3 &in ) const;
    void    ProjectToPlane( const XVec3 &in, XVec3 &out ) const;
    XVec3   IntersectRay( const XVec3 &p1, const XVec3 &p2) const;
    float   getIntersectTime( const XVec3 &p1, const XVec3 &p2_minus_p1 ) const;
    void    ReorderBoundBox( XVec3 &min, XVec3 &max );
};

inline XPlane::XPlane( void )
{   
}

inline XPlane::XPlane( const XVec3 &vec, float dist )
{
    n = vec;
    d = dist;
}

inline XPlane::XPlane( const XPlane &copy )
{
    n = copy.n;
    d = copy.d;
}

inline XPlane::XPlane( const XVec3 &point, const XVec3 &normal )
{
    n = normal;
    d = - ( normal.x * point.x + normal.y * point.y + normal.z * point.z );
}

inline float XPlane::DistanceFromPlane( const XVec3 &in ) const
{
    return ( in DOT n ) + d;
}

inline void XPlane::ProjectToPlane( const XVec3 &in, XVec3 &out ) const
{
    out = in + ( n * DistanceFromPlane( in ) );
}

#endif __XPLANE_H__
