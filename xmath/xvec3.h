// This file is distributed under a MIT license. See LICENSE.txt for details.
//
// XMath clases
//
// X Vector 3 class ( header )
//

#ifndef __XVEC3_H__
#define __XVEC3_H__

#include "xmath.h"

#define LERP_EPS 1e-6

#define CROSS ^
#define DOT *

class XEuler;

class XVec3
{
public:
    float       x;
    float       y;
    float       z;

    // main constructors
    XVec3( void );
    XVec3( float x, float y, float z );
    XVec3( const char *pStr );
    XVec3( const float *pSrc );

    float       *vec3( void );
    const float *vec3( void ) const;

    void        copyTo( float * ) const;
    float       operator[]( int index ) const;

    float       &operator[]( int index );
    void        xyz( float x, float y, float z );

    // operators

    void        operator = ( const XVec3 &v );
    void        operator = ( const float *v );
   friend XVec3 operator + ( const XVec3 &v1, const XVec3 &v2 );
    XVec3       &operator += ( const XVec3 &v );
   friend XVec3 operator - ( const XVec3 &v1, const XVec3 &v2 );
   friend XVec3 operator - ( const XVec3 &v1 );
    XVec3       &operator -= ( const XVec3 &v1 );
   friend XVec3 operator * ( const XVec3 &v1, float k );
   friend XVec3 operator * ( float k, const XVec3 &v1 );
   friend float operator * ( const XVec3 &v1, const XVec3 &v2 );
    XVec3       &operator *= ( float k );
   friend bool  operator == ( const XVec3 &v1, const XVec3 &v2 );
   friend bool  operator != ( const XVec3 &v1, const XVec3 &v2 );
   friend XVec3 operator ^ ( const XVec3 &v1, const XVec3 &v2 );
    void        CrossProduct( const XVec3 &v1, const XVec3 &v2 );
    float       Length( void ) const;
    void        Normalize( void );
    void        FastNormalize( void );
    XVec3       getNormalized( void ) const;
    void        ScaleAdd( const XVec3 &mult, float k, const XVec3 &add );
    void        Scale( const XVec3 &mult, float k );
    void        RotatePointAroundVector( const XVec3 &dir, const XVec3 &point, float deg );
    float       DistanceToLine( XVec3 &xvLineStart, XVec3 &xvLineEnd );

    XEuler      ToEuler() const;
    float       toYaw( void	);
    float       toPitch( void );
    XVec3       toAngles( void );
    XEuler      ToAngles();
    XEuler      TreatAsAngles();

    void        LerpVector( XVec3 &v1, XVec3 &v2, float t );
   friend float MaxValue( XVec3 v );
};

// inlines

inline XVec3::XVec3()
{
}

inline XVec3::XVec3 ( const float *pSrc ) :
    x( pSrc[ 0 ] ),
    y( pSrc[ 1 ] ),
    z( pSrc[ 2 ] )
{
}

inline XVec3::XVec3 ( float x1, float y1, float z1 ) :
    x( x1 ),
    y( y1 ),
    z( z1 )
{
}

inline void XVec3::copyTo( float *pVec ) const
{
    pVec[ 0 ] = x;
    pVec[ 1 ] = y;
    pVec[ 2 ] = z;
}

inline void XVec3::xyz( float x1, float y1, float z1 )
{
    x = x1;
    y = y1;
    z = z1;
}

inline float *XVec3::vec3()
{
    return &x;
}

inline const float *XVec3::vec3() const
{
    return &x;
}

// inline operators

inline void XVec3::operator = ( const XVec3 &v )
{
    x = v.x;
    y = v.y;
    z = v.z;
}

inline void XVec3::operator = ( const float *pV )
{
    x = pV[ 0 ];
    y = pV[ 1 ];
    z = pV[ 2 ];
}

inline XVec3 &XVec3::operator += ( const XVec3 &v )
{
    x += v.x;
    y += v.y;
    z += v.z;
    return *this;
}

inline XVec3 &XVec3::operator -= ( const XVec3 &v )
{
    x -= v.x;
    y -= v.y;
    z -= v.z;
    return *this;
}

inline XVec3 operator - ( const XVec3 &v )
{
    XVec3 v1;

    v1.x = -v.x;
    v1.y = -v.y;
    v1.z = -v.z;

    return v1;
}

inline float XVec3::operator[] ( int index ) const
{
    return ( &x )[ index ];
}

inline float &XVec3::operator[] ( int index )
{
    return ( &x )[ index ];
}

inline XVec3::XVec3( const char *pStr )
{
    if ( pStr )
    {
        sscanf( pStr, "%f %f %f", &x, &y, &z );
    }
    else
    {
        x = y = z = 0;
    }
}

inline XVec3 operator + ( const XVec3 &v1, const XVec3 &v2 )
{
    return XVec3( v1.x + v2.x, v1.y + v2.y, v1.z + v2.z );
}

inline XVec3 operator - ( const XVec3 &v1, const XVec3 &v2 )
{
    return XVec3( v1.x - v2.x, v1.y - v2.y, v1.z - v2.z );
}

inline XVec3 operator * ( const XVec3 &v1, float k )
{
    return XVec3( v1.x * k, v1.y * k, v1.z * k );
}

inline XVec3 operator * ( float k, const XVec3 &v1 )
{
    return v1 * k;
}

inline float operator * ( const XVec3 &v1, const XVec3 &v2 )
{
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

inline XVec3& XVec3::operator *= ( float k )
{
    x *= k;
    y *= k;
    z *= k;
    return *this;
}

inline bool operator == ( const XVec3 &v1, const XVec3 &v2 )
{
    return ( ( v1.x == v2.x ) && ( v1.y == v2.y ) && ( v1.z == v2.z ) );
}

inline bool operator != ( const XVec3 &v1, const XVec3 &v2 )
{
    return ( ( v1.x != v2.x ) || ( v1.y != v2.y ) || ( v1.z != v2.z ) );
}

inline void XVec3::CrossProduct( const XVec3 &v1, const XVec3 &v2 )
{
    x = v1.y * v2.z - v1.z * v2.y;
    y = v1.z * v2.x - v1.x * v2.z;
    z = v1.x * v2.y - v1.y * v2.x;
}

inline XVec3 operator ^ ( const XVec3 &v1, const XVec3 &v2 )
{
   XVec3 v;

   v.CrossProduct( v1, v2 );

   return v;
}   

inline float XVec3::Length( void ) const
{
    float l;
    
    l = x * x + y * y + z * z;
    return (float)sqrt( l );
}

inline void XVec3::Normalize( void )
{
    float l;

    l = Length();
    if ( l > 0.000001)
    {
        l = 1.0f / l;
        x *= l;
        y *= l;
        z *= l;
    }
    else
    {
        x = y = z = 0;
    }
}

inline void XVec3::FastNormalize( void )
{
    float l;

    l = x * x + y * y + z * z;
    if (l > 0.000001)
    {
        l = XMATH_InvSqrt( l );
        x *= l;
        y *= l;
        z *= l;
    }
    else
    {
        x = y = z = 0.f;
    }
}

inline XVec3 XVec3::getNormalized( void ) const
{
    XVec3 v = *this;
    
    v.Normalize();
    return v;
}

inline XVec3 fabs( XVec3 v )
{
    return XVec3( Fabs( v.x ), Fabs( v.y ), Fabs( v.z ) );
}

inline float MaxValue( XVec3 v )
{
   float maxy;
   float maxz;
   float max;

   max = Fabs( v.x );
   maxy = Fabs( v.y );
   maxz = Fabs( v.z );
   if ( maxy > max )
   {
       max = maxy;
   }
   if ( maxz > max )
   {
       max = maxz;
   }
   
   return max;
}

inline void XVec3::ScaleAdd( const XVec3 &mult, float k, const XVec3 &add )
{
   x = mult.x * k + add.x;
   y = mult.y * k + add.y;
   z = mult.z * k + add.z;
}

inline void XVec3::Scale( const XVec3 &mult, float k ) 
{
   x = mult.x * k;
   y = mult.y * k;
   z = mult.z * k;
}

#endif __XVEC3_H__
