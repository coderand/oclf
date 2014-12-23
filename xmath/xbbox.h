// This file is distributed under a MIT license. See LICENSE.txt for details.
//
// XMath clases
//
// X Bound Box class ( header )
//

#ifndef __XBBOX_H__
#define __XBBOX_H__

#include "xvec3.h"
#include "xmath.h"

class XBBox
{
private:
public:

    XVec3       min, max;

    // constructors
    XBBox();
    XBBox( const XVec3 &min, const XVec3 &max );

    void        set( const XVec3 &min, const XVec3 &max );

    // operators

    XVec3       operator[]( int index ) const;
    XVec3       &operator[]( int index );
    void        operator = ( const XBBox &bbox );
    void        operator = ( const XVec3 *ptr );
    friend XBBox operator + ( const XBBox &box1, const XBBox &box2 );
    XBBox       &operator += ( const XBBox &box );

    void        AddPoint( const XVec3 &xvPoint );
    bool        TestPoint( const XVec3 &xvPoint );
    bool        TestBBox( const XBBox &xbbox );
};

// implementation

inline XBBox::XBBox()
{
}

inline XBBox::XBBox( const XVec3 &minBBox, const XVec3 &maxBBox )
{
    min = minBBox;
    max = maxBBox;
}

inline void XBBox::set( const XVec3 &minBBox, const XVec3 &maxBBox )
{
    min = minBBox;
    max = maxBBox;
}

inline XVec3 XBBox::operator[]( int index ) const
{
    return ( &min )[ index ];
}

inline XVec3 &XBBox::operator[]( int index )
{
    return ( &min )[ index ];
}

inline void XBBox::operator = ( const XBBox &bbox )
{
    min = bbox.min;
    max = bbox.max;
}

inline void XBBox::operator = ( const XVec3 *ptr )
{
    min = ptr[ 0 ];
    max = ptr[ 1 ];
}

inline XBBox operator + ( const XBBox &box1, const XBBox &box2 )
{
    XBBox box;
    
    box = box1;
    box.AddPoint( box2.min );
    box.AddPoint( box2.max );

    return box;
}

inline XBBox &XBBox::operator += ( const XBBox &box )
{
    AddPoint( box.min );
    AddPoint( box.max );

    return *this;
}

#endif __XBBOX_H__
