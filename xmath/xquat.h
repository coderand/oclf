// This file is distributed under a MIT license. See LICENSE.txt for details.
//
// XMath clases
//
// X Quaternion class ( header )
//

#ifndef __XQUAT_H__
#define __XQUAT_H__

#include "xmath.h"
#include "xvec3.h"
#include "assert.h"

class XMat3;
class XEuler;
class XQuat 
{
public:
    float     x, y, z, w;

    // constructors
              XQuat();
              XQuat( float xx, float yy, float zz, float ww );
              XQuat( const XVec3 &, float ww = 0.f );
              XQuat( const XEuler & );
    void      set( float xx, float yy, float zz, float ww );
    XVec3     &vec3();
    float     *vec4();

    // operators
    float     operator [] ( int index ) const;
    float     &operator [] ( int index );
    friend XQuat operator + ( const XQuat &, const XQuat & );
    friend XQuat operator - ( const XQuat &, const XQuat & );
    friend XQuat operator * ( const XQuat &, float );
    friend XQuat operator * ( float, const XQuat & );
    friend XQuat operator * ( const XQuat &, const XQuat & );

    inline friend XQuat operator * ( float a, XQuat &b )
    {
       return b * a;
    }

    XQuat     &operator += ( const XQuat & );
    XQuat     &operator -= ( const XQuat & );
    XQuat     &operator *= ( float );
    XQuat     &operator *= ( const XQuat & );

    bool operator == ( const XQuat & ) const;
    bool operator != ( const XQuat &v ) const
    {
        return ! ( *this == v );
    }
   
    void operator = ( const XMat3 & );
    void operator = ( const XQuat & );
    void operator = ( const float [] );

    // methods
    float     Length();
    void      Normalize();
    XQuat     getNormalized();
    void      Invert();
    XQuat     getInverse();
    void      Slerp( const XQuat &from, const XQuat &to, float t );
    void      SetupRotation( float angles );
    void      ConvertFromVectorAngle( const XVec3 &v, float radians );
    void      ConvertToVectorAngle( XVec3 &v, float &angle );

    XVec3     ProjectVector( const XVec3 &v ) const;
    XVec3     UnprojectVector( const XVec3 &v ) const;
};

// inline implementation

// constructors
inline XQuat::XQuat()
{
}

inline XQuat::XQuat( float xx, float yy, float zz, float ww )
{
    x = xx; 
    y = yy; 
    z = zz; 
    w = ww;
}

inline XQuat::XQuat( const XVec3 &v, float ww )
{
    x = v.x; 
    y = v.y; 
    z = v.z; 
    w = ww;
}

inline void XQuat::set( float xx, float yy, float zz, float ww )
{
    x = xx; 
    y = yy; 
    z = zz; 
    w = ww;
}

inline XVec3 &XQuat::vec3()
{
    return *reinterpret_cast< XVec3 * >( &x ); 
}

inline float *XQuat::vec4()
{
    return &x;
}

// operators

inline XQuat &XQuat::operator += ( const XQuat &a )
{
    x += a.x; 
    y += a.y; 
    z += a.z; 
    w += a.w; 
    return *this; 
}

inline XQuat &XQuat::operator -= ( const XQuat &a ) 
{ 
    x -= a.x; 
    y -= a.y; 
    z -= a.z; 
    w -= a.w; 
    return *this; 
}

inline XQuat &XQuat::operator *= ( float f )        
{ 
    x *= f; 
    y *= f; 
    z *= f; 
    w *= f; 
    return *this; 
}

inline XQuat &XQuat::operator *= ( const XQuat &a ) 
{ 
    *this = *this * a; 
    return *this; 
}

inline bool XQuat::operator == ( const XQuat &a ) const
{
    return x == a.x && y == a.y && z == a.z && w == a.w;
}

inline XQuat operator * ( float a, const XQuat &b ) 
{ 
    return b * a; 
}

inline void XQuat::operator = ( const XQuat &q )
{
    x = q.x;
    y = q.y;
    z = q.z;
    w = q.w;
}

inline void XQuat::operator = ( const float q[] )
{
    x = q[ 0 ];
    y = q[ 1 ];
    z = q[ 2 ];
    w = q[ 3 ];
}

// methods

inline void XQuat::SetupRotation( float angles )
{
    float halftheta = Deg2Rad( angles ) * 0.5f;

    w = Cos( halftheta );
    vec3() *= Sin( halftheta );
}

inline void XQuat::ConvertFromVectorAngle( const XVec3 &v, float angle )
{
    float sn = Sin( angle * 0.5 );

    w = Cos( angle * 0.5 );
    x = v.x * sn;
    y = v.y * sn;
    z = v.z * sn;
}

inline void XQuat::ConvertToVectorAngle( XVec3 &v, float &angle )
{
    float sn;

    angle = 2 * Acos( w );
    sn = 1.0 / Sin( 0.5 * angle );
    v.x = x * sn;
    v.y = y * sn;
    v.z = z * sn;
}

inline XQuat operator + ( const XQuat &a, const XQuat &b )
{ 
    XQuat c;

    c.x = a.x + b.x;
    c.y = a.y + b.y;
    c.z = a.z + b.z;
    c.w = a.w + b.w;

    return c;
}

inline XQuat operator - ( const XQuat &a, const XQuat &b )
{ 
    XQuat c;

    c.x = a.x - b.x;
    c.y = a.y - b.y;
    c.z = a.z - b.z;
    c.w = a.w - b.w;

    return c;
}

inline XQuat operator * (const XQuat &a, float b )
{
    XQuat c;

    c.x = a.x * b;
    c.y = a.y * b;
    c.z = a.z * b;
    c.w = a.w * b;

    return c;
}

inline XQuat operator * ( const XQuat &a, const XQuat &b )
{
    XQuat c;

    c.x = a.w * b.x + b.w * a.x + a.y * b.z - a.z * b.y;
    c.y = a.w * b.y + b.w * a.y + a.z * b.x - a.x * b.z;
    c.z = a.w * b.z + b.w * a.z + a.x * b.y - a.y * b.x;
    c.w = a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z;

    return c;
}

inline float XQuat::operator[] ( int index ) const
{
    assert( ( index >= 0 ) && ( index < 4 ) );
    
    return ( &x )[ index ];
}

inline float& XQuat::operator[] ( int index )
{
    assert( ( index >= 0 ) && ( index < 4 ) );

    return ( &x )[ index ];
}

inline float XQuat::Length( void )
{
    return Sqrt ( x * x + y * y + z * z + w * w );
}

inline void XQuat::Normalize( void )
{
    float length, ilength;

    length = Length();
    if ( length )
    {
        ilength = 1.f / length;
        x *= ilength;
        y *= ilength;
        z *= ilength;
        w *= ilength;
    }
}

inline XQuat XQuat::getNormalized( void )
{
    XQuat q = *this;

    q.Normalize();
    return q;
}

inline void XQuat::Invert( void )
{
    vec3 () *= -1.f;
}

inline XQuat XQuat::getInverse( void )
{
    XQuat u ( *this );

    u.vec3() *= -1.f;

    return u;
}

inline XVec3 XQuat::ProjectVector( const XVec3 &b ) const
{
    XQuat c(
         w * b.x + y * b.z - z * b.y,
         w * b.y + z * b.x - x * b.z,
         w * b.z + x * b.y - y * b.x,
        -x * b.x - y * b.y - z * b.z );

    return XVec3(
        c.w * -x - w * c.x - c.y * z + c.z * y,
        c.w * -y - w * c.y - c.z * x + c.x * z,
        c.w * -z - w * c.z - c.x * y + c.y * x );
}

inline XVec3 XQuat::UnprojectVector( const XVec3 &b ) const
{
    XQuat c(
        w * b.x - y * b.z + z * b.y,
        w * b.y - z * b.x + x * b.z,
        w * b.z - x * b.y + y * b.x,
        x * b.x + y * b.y + z * b.z );

    return XVec3(
        c.w * x + w * c.x + c.y * z - c.z * y,
        c.w * y + w * c.y + c.z * x - c.x * z,
        c.w * z + w * c.z + c.x * y - c.y * x );
}

#endif __XQUAT_H__
