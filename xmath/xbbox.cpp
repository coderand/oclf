// This file is distributed under a MIT license. See LICENSE.txt for details.
//
// XMath clases
//
// X Bound Box class
//

#include "common.h"

#include "xbbox.h"
#include "xmatrix.h"

void XBBox::AddPoint( const XVec3 &point )
{
    if ( point.x > max.x ) max.x = point.x;
    if ( point.y > max.y ) max.y = point.y;
    if ( point.z > max.z ) max.z = point.z;

    if ( point.x < min.x ) min.x = point.x;
    if ( point.y < min.y ) min.y = point.y;
    if ( point.z < min.z ) min.z = point.z;
}

bool XBBox::TestPoint( const XVec3 &point )
{
    if ( ( point.x <= max.x ) &&
         ( point.y <= max.y ) &&
         ( point.z <= max.z ) &&
         ( point.x >= min.x ) &&
         ( point.y >= min.y ) &&
         ( point.z >= min.z ) )
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool XBBox::TestBBox( const XBBox &xbbox )
{

    for ( int i = 0; i < 3; i++ )
    if ( ( max[ i ] < xbbox.min[ i ] ) ||
         ( xbbox.max[ i ] < min[ i ] ) )
         return false;

    return true;
}
