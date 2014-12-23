// This file is distributed under a MIT license. See LICENSE.txt for details.
//
// XMath clases ( header )
//

#ifndef __XMATH_H__
#define __XMATH_H__

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <float.h>

#ifdef _WIN32
#pragma warning( disable : 4244 ) // double to float conversion
#endif

#ifndef M_PI
#define M_PI        3.14159265358979323846f
#endif // M_PI

#define FP_BITS( fp )       ( *( DWORD * ) & ( fp ) )
#define FP_ABS_BITS( fp )   ( FP_BITS( fp ) & 0x7FFFFFFF )
#define FP_SIGN_BIT( fp )   ( FP_BITS( fp ) & 0x80000000 )
#define FP_ONE_BITS         0x3F800000

// square root

inline float XMATH_InvSqrt( float number )
{
    return 1.0f / sqrt( number );
}

// randoms

inline int XMATH_rand( int *seed )
{
    *seed = ( 69069 * *seed + 1 );
    return *seed;
}

inline float XMATH_random( int *seed )
{
    return ( XMATH_rand( seed ) & 0xffff ) / (float)0x10000;
}

inline float XMATH_crandom( int *seed )
{
    return 2.0 * ( XMATH_random( seed ) - 0.5 );
}

template <class T> inline unsigned int &xint_cast( T &x )
{
    return *reinterpret_cast< unsigned * >( &x );
}

inline float Fabs( float x )
{
    xint_cast( x ) &= 0x7fffffff;
    return x;
}

inline int Fsgn( float x )
{
    if ( x > 0 )
    {
        return 1;
    }
    else
    if ( x < 0 )
    {
        return -1;
    }
    else
    {
        return 0;
    }
}

inline float Deg2Rad( float x )
{
    return ( x * M_PI ) / 180.f;
}

inline float Rad2Deg( float x )
{
    return ( x * 180.f ) / M_PI;
}

inline float Sin( float x )
{
    return float( sin( x ) );
}

inline float Cos( float x )
{
    return float( cos( x ) );
}

inline float Asin( float x )
{
    return float( asin( x ) );
}

inline float Acos( float x )
{
    return float( acos( x ) );
}

inline float Sqrt( float x )
{
    return float( sqrt( x ) );
}

inline float Atan2( float x, float y )
{
    return float( atan2( x, y ) );
}

#endif __XMATH_H__
