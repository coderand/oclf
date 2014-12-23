// This file is distributed under a MIT license. See LICENSE.txt for details.
//
// XMath clases
//
// X Plane class
//

#include "common.h"

#include "xplane.h"

float XPlane::getIntersectTime( const XVec3 &p1, const XVec3 &p2_minus_p1 ) const
{
    return - ( n DOT p1 + d ) / ( n DOT p2_minus_p1 );
}

XVec3 XPlane::IntersectRay( const XVec3 &p1, const XVec3 &p2 ) const
{
    XVec3 p2_minus_p1 = p2;

    p2_minus_p1 -= p1;

    p2_minus_p1 *= getIntersectTime( p1, p2_minus_p1 );
    p2_minus_p1 += p1;

    return p2_minus_p1;
}

void XPlane::ReorderBoundBox( XVec3 &min, XVec3 &max )
{
    XVec3 vmin = min;
    XVec3 vmax = max;

    if ( ( ( n.x >= 0 ) && ( n.y >= 0 ) && ( n.z >= 0 ) ) ||
         ( ( n.x < 0 ) && ( n.y < 0 ) && ( n.z < 0 ) ) )
    {
        //min = vmin;
        //max = vmax;
    }
    else
    if ( ( ( n.x < 0 ) && ( n.y >= 0 ) && ( n.z >= 0 ) ) ||
         ( ( n.x >= 0 ) && ( n.y < 0 ) && ( n.z < 0 ) ) )
    {
        min.x = vmax.x;
        min.y = vmin.y;
        min.z = vmin.z;
        max.x = vmin.x;
        max.y = vmax.y;
        max.z = vmax.z;
    }
    else
    if ( ( ( n.x >= 0 ) && ( n.y >= 0 ) && ( n.z < 0 ) ) ||
         ( ( n.x < 0 ) && ( n.y < 0 ) && ( n.z >= 0 ) ) )
    {
        min.x = vmin.x;
        min.y = vmin.y;
        min.z = vmax.z;
        max.x = vmax.x;
        max.y = vmax.y;
        max.z = vmin.z;
    }
    else
    if ( ( ( n.x < 0 ) && ( n.y >= 0 ) && ( n.z < 0 ) ) ||
         ( ( n.x >= 0 ) && ( n.y < 0 ) && ( n.z >= 0 ) ) )
    {
        min.x = vmax.x;
        min.y = vmin.y;
        min.z = vmax.z;
        max.x = vmin.x;
        max.y = vmax.y;
        max.z = vmin.z;
    }
}
