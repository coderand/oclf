// This file is distributed under a MIT license. See LICENSE.txt for details.
//
// XMath clases
//
// X Vector 4 class ( header )
//

#ifndef __XVEC4_H__
#define __XVEC4_H__

#include "xmath.h"

#define LERP_EPS 1e-6

#define CROSS ^
#define DOT *

class XVec3;

class XVec4
{
public:
    union
    {
    float       x, r; // union x / r
    };
    union
    {
    float       y, g; // union y / g
    };
    union
    {
    float       z, b; // union z / b
    };
    union
    {
    float       w, a; // union w / a
    };

    // main constructors
    XVec4( void );
    XVec4( float x, float y, float z, float w );
    XVec4( const char *pStr );
    XVec4( const float *pSrc );
    XVec4( const XVec3 & );

    float       *vec4( void );
    const float *vec4( void ) const;

    void        copyTo( float * ) const;
    float       operator[]( int index ) const;

    float       &operator[]( int index );
    void        xyzw( float x, float y, float z, float w );
                  
    // operators

    void        operator = ( const XVec4 &v );
    void        operator = ( const XVec3 &v );
    void        operator = ( const float *v );
   friend XVec4 operator + ( const XVec4 &v1, const XVec4 &v2 );
    XVec4       &operator += ( const XVec4 &v );
   friend XVec4 operator - ( const XVec4 &v1, const XVec4 &v2 );
   friend XVec4 operator - ( const XVec4 &v1 );
    XVec4       &operator -= ( const XVec4 &v1 );
   friend XVec4 operator * ( const XVec4 &v1, float k );
   friend XVec4 operator * ( float k, const XVec4 &v1 );
   friend float operator * ( const XVec4 &v1, const XVec4 &v2 );
    XVec4       &operator *= ( float k );
   friend bool  operator == ( const XVec4 &v1, const XVec4 &v2 );
   friend bool  operator != ( const XVec4 &v1, const XVec4 &v2 );
   friend XVec4 operator ^ ( const XVec4 &v1, const XVec4 &v2 );
    float       Length( void ) const;
    void        Normalize( void );
    void        FastNormalize( void );
    XVec4       getNormalized( void );
    void        ScaleAdd( const XVec4 &mult, float k, const XVec4 &add );
    void        Scale( const XVec4 &mult, float k );

    void        LerpVector( XVec4 &v1, XVec4 &v2, float t );
   friend float MaxValue( XVec4 v );
};

// inlines

inline XVec4::XVec4()
{
}

inline XVec4::XVec4 ( const float *pSrc ) :
    x( pSrc[ 0 ] ),
    y( pSrc[ 1 ] ),
    z( pSrc[ 2 ] ),
    w( pSrc[ 3 ] )
{
}

inline XVec4::XVec4 ( float x1, float y1, float z1, float w1 ) :
    x( x1 ),
    y( y1 ),
    z( z1 ),
    w( w1 )
{
}

inline XVec4::XVec4( const XVec3 &v ) :
    x( ((float *)&v)[ 0 ] ),
    y( ((float *)&v)[ 1 ] ),
    z( ((float *)&v)[ 2 ] ),
    w( 1.0f )
{
}

inline void XVec4::copyTo( float *pVec ) const
{
    pVec[ 0 ] = x;
    pVec[ 1 ] = y;
    pVec[ 2 ] = z;
    pVec[ 3 ] = w;
}

inline void XVec4::xyzw( float x1, float y1, float z1, float w1 )
{
    x = x1;
    y = y1;
    z = z1;
    w = w1;
}

inline float *XVec4::vec4()
{
    return &x;
}

inline const float *XVec4::vec4() const
{
    return &x;
}

// inline operators

inline void XVec4::operator = ( const XVec4 &v )
{
    x = v.x;
    y = v.y;
    z = v.z;
    w = v.w;
}

inline void XVec4::operator = ( const XVec3 &v )
{
    x = ((float *)&v)[ 0 ];
    y = ((float *)&v)[ 1 ];
    z = ((float *)&v)[ 2 ];
    w = 1;
}

inline void XVec4::operator = ( const float *pV )
{
    x = pV[ 0 ];
    y = pV[ 1 ];
    z = pV[ 2 ];
    y = pV[ 3 ];
}

inline XVec4 &XVec4::operator += ( const XVec4 &v )
{
    x += v.x;
    y += v.y;
    z += v.z;
    w += v.w;
    return *this;
}

inline XVec4 &XVec4::operator -= ( const XVec4 &v )
{
    x -= v.x;
    y -= v.y;
    z -= v.z;
    w -= v.w;
    return *this;
}

inline XVec4 operator - ( const XVec4 &v )
{
    XVec4 v1;

    v1.x = -v.x;
    v1.y = -v.y;
    v1.z = -v.z;
    v1.w = -v.w;

    return v1;
}

inline float XVec4::operator[] ( int index ) const
{
    return ( &x )[ index ];
}

inline float &XVec4::operator[] ( int index )
{
    return ( &x )[ index ];
}

inline XVec4::XVec4( const char *pStr )
{
    if ( pStr )
    {
        sscanf( pStr, "%f %f %f %f", &x, &y, &z, &w );
    }
    else
    {
        x = y = z = w = 0;
    }
}

inline XVec4 operator + ( const XVec4 &v1, const XVec4 &v2 )
{
    return XVec4( v1.x + v2.x, v1.y + v2.y, v1.z + v2.z, v1.w + v2.w );
}

inline XVec4 operator - ( const XVec4 &v1, const XVec4 &v2 )
{
    return XVec4( v1.x - v2.x, v1.y - v2.y, v1.z - v2.z, v1.w - v2.w );
}

inline XVec4 operator * ( const XVec4 &v1, float k )
{
    return XVec4( v1.x * k, v1.y * k, v1.z * k, v1.w * k );
}

inline XVec4 operator * ( float k, const XVec4 &v1 )
{
    return v1 * k;
}

inline float operator * ( const XVec4 &v1, const XVec4 &v2 )
{
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z + v1.w * v2.w;
}

inline XVec4& XVec4::operator *= ( float k )
{
    x *= k;
    y *= k;
    z *= k;
    w *= k;
    return *this;
}

inline bool operator == ( const XVec4 &v1, const XVec4 &v2 )
{
    return ( ( v1.x == v2.x ) && ( v1.y == v2.y ) && ( v1.z == v2.z ) && ( v1.w == v2.w ) );
}

inline bool operator != ( const XVec4 &v1, const XVec4 &v2 )
{
    return ( ( v1.x != v2.x ) || ( v1.y != v2.y ) || ( v1.z != v2.z ) || ( v1.w != v2.w ) );
}

inline float XVec4::Length( void ) const
{
    float l;
    
    l = x * x + y * y + z * z + w * w;
    return (float)sqrt( l );
}

inline void XVec4::Normalize( void )
{
    float l;

    l = Length();
    if ( l )
    {
        l = 1.0f / l;
        x *= l;
        y *= l;
        z *= l;
        w *= l;
    }
    else
    {
        x = y = z = w = 0;
    }
}

inline void XVec4::FastNormalize( void )
{
    float l;

    l = x * x + y * y + z * z + w * w;
    if ( l != 0 )
    {
        l = XMATH_InvSqrt( l );
        x *= l;
        y *= l;
        z *= l;
        w *= l;
    }
    else
    {
        x = y = z = w = 0.f;
    }
}

inline XVec4 XVec4::getNormalized( void )
{
    XVec4 v = *this;
    
    v.Normalize();
    return v;
}

inline XVec4 fabs( XVec4 v )
{
    return XVec4( Fabs( v.x ), Fabs( v.y ), Fabs( v.z ), Fabs( v.w ) );
}

inline float MaxValue( XVec4 v )
{
   float maxw;
   float maxy;
   float maxz;
   float max;

   max = Fabs( v.x );
   maxy = Fabs( v.y );
   maxz = Fabs( v.z );
   maxw = Fabs( v.w );
   if ( maxy > max )
   {
       max = maxy;
   }
   if ( maxz > max )
   {
       max = maxz;
   }
   if ( maxw > max )
   {
       max = maxw;
   }
   
   return max;
}

inline void XVec4::ScaleAdd( const XVec4 &mult, float k, const XVec4 &add )
{
   x = mult.x * k + add.x;
   y = mult.y * k + add.y;
   z = mult.z * k + add.z;
   w = mult.w * k + add.w;
}

inline void XVec4::Scale( const XVec4 &mult, float k ) 
{
   x = mult.x * k;
   y = mult.y * k;
   z = mult.z * k;
   w = mult.w * k;
}

#endif __XVEC4_H__
