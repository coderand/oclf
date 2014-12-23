// This file is distributed under a MIT license. See LICENSE.txt for details.
//
// XMath clases
//
// X Matrix class
//

#include "common.h"

#include "xmatrix.h"
#include "xquat.h"
#include "xeuler.h"

XMat4::XMat4( const XMat3 &mat3 )
{
    int i;

    for( i = 0; i < 3; i++ )
    {
        floats[ 0 ][ i ] = mat3.vecs[ 0 ][ i ];
        floats[ 1 ][ i ] = mat3.vecs[ 1 ][ i ];
        floats[ 2 ][ i ] = mat3.vecs[ 2 ][ i ];
        floats[ 3 ][ i ] = 0.f;
    }

    floats[ 0 ][ 3 ] = 0.f;
    floats[ 1 ][ 3 ] = 0.f;
    floats[ 2 ][ 3 ] = 0.f;
    floats[ 3 ][ 3 ] = 1.f;
}

void XMat4::setIdentity( void ) 
{
    floats[ 0 ][ 0 ] = 1; floats[ 0 ][ 1 ] = 0; floats[ 0 ][ 2 ] = 0; floats[ 0 ][ 3 ] = 0;
    floats[ 1 ][ 0 ] = 0; floats[ 1 ][ 1 ] = 1; floats[ 1 ][ 2 ] = 0; floats[ 1 ][ 3 ] = 0;
    floats[ 2 ][ 0 ] = 0; floats[ 2 ][ 1 ] = 0; floats[ 2 ][ 2 ] = 1; floats[ 2 ][ 3 ] = 0;
    floats[ 3 ][ 0 ] = 0; floats[ 3 ][ 1 ] = 0; floats[ 3 ][ 2 ] = 0; floats[ 3 ][ 3 ] = 1;
}

void XMat4::Transpose( void )
{
    float f;
    int   i;
    int   i1;

    for( i = 0; i < 4; i++ ) 
    {
        for( i1 = i + 1; i1 < 4; i1++ )
        {
            f = floats[ i ][ i1 ];
            floats[ i ][ i1 ] = floats[ i1 ][ i ];
            floats[ i1 ][ i ] = f;
        }
    }
}

void XMat4::MultiplyMatrix( const XMat4 &in1, const XMat4 &in2 )
{
    int i;
    int i1;

    for( i = 0; i < 4; i++ )
    {
        for( i1 = 0; i1 < 4; i1++ )
        {
            floats[ i ][ i1 ] = 
                in2.floats[ 0 ][ i1 ] * in1.floats[ i ][ 0 ] +
                in2.floats[ 1 ][ i1 ] * in1.floats[ i ][ 1 ] +
                in2.floats[ 2 ][ i1 ] * in1.floats[ i ][ 2 ] +
                in2.floats[ 3 ][ i1 ] * in1.floats[ i ][ 3 ];
        }
    }
}

void XMat4::MultiplyThis( const XMat4 &in ) 
{
    float tmp[ 4 ][ 4 ];
    int i;
    int i1;

    for( i = 0; i < 4; i++ )
    {
        for( i1 = 0; i1 < 4; i1++ )
        {
            tmp[ i ][ i1 ] = 
                floats[ i ][ 0 ] * in.floats[ 0 ][ i1 ] +
                floats[ i ][ 1 ] * in.floats[ 1 ][ i1 ] +
                floats[ i ][ 2 ] * in.floats[ 2 ][ i1 ] +
                floats[ i ][ 3 ] * in.floats[ 3 ][ i1 ];
        }
    }

    memcpy( floats, tmp, sizeof( floats ) );
}

float XMat4::MultiplyVector( const XVec3 &in, XVec3 &out ) const
{
    int   i;
    float w_inv;
    float w;

    for ( i = 0; i < 3; i++ )
    {
        out[ i ] = 
            in[ 0 ] * floats[ 0 ][ i ] + 
            in[ 1 ] * floats[ 1 ][ i ] + 
            in[ 2 ] * floats[ 2 ][ i ] + 
                      floats[ 3 ][ i ]; // w = 1.0
    }

    w = in[ 0 ] * floats[ 0 ][ 3 ] + in[ 1 ] * floats[ 1 ][ 3 ] + 
        in[ 2 ] * floats[ 2 ][ 3 ] + floats[ 3 ][ 3 ];
    w_inv = 1.f / w;

   out *= w_inv;

   return w;
}

XMat4 XMat4::Ortho( float left, float right, float bottom, float top,
                    float nearz, float farz ) 
{
    XMat4 that;

    that.setIdentity();
    that.floats[ 0 ][ 0 ] = 2.f / ( right - left );
    that.floats[ 1 ][ 1 ] = 2.f / ( top - bottom );
    that.floats[ 2 ][ 2 ] = -2.f / ( farz - nearz );
    that.floats[ 3 ][ 3 ] = 1.f;

    that.floats[ 3 ][ 0 ] = ( right + left ) / ( right - left );
    that.floats[ 3 ][ 1 ] = ( top + bottom ) / ( top - bottom );
    that.floats[ 3 ][ 2 ] = ( farz + nearz ) / ( farz - nearz );

    return that;
}

XMat4 XMat4::Frustum( float left, float right, float bottom, float top,
                      float nearz, float farz )
{
    XMat4 that;

    that.setIdentity();
    that.floats[ 0 ][ 0 ] = 2.f * nearz / ( right - left );
    that.floats[ 1 ][ 1 ] = 2.f * nearz / ( top - bottom );
    that.floats[ 2 ][ 2 ] = - ( farz + nearz ) / ( farz - nearz );

    that.floats[ 2 ][ 0 ] = ( right + left ) / ( right - left );
    that.floats[ 2 ][ 1 ] = ( top + bottom ) / ( top - bottom );
    that.floats[ 3 ][ 2 ] = -2.f * farz * nearz / ( farz - nearz );

    return that;
}

XMat4 XMat4::Translate( float x, float y, float z )
{
    XMat4 that;

    that.setIdentity();

    that.floats[ 3 ][ 0 ] = x;
    that.floats[ 3 ][ 1 ] = y;
    that.floats[ 3 ][ 2 ] = z;

    return that;
}

// was moved to xmatrix.h as inline
/*
void XMat3::ProjectVector( const XVec3 &in, XVec3 &out ) const
{
    out.x = in DOT vecs[ 0 ];
    out.y = in DOT vecs[ 1 ];
    out.z = in DOT vecs[ 2 ];
}

void XMat3::UnprojectVector( const XVec3 &in, XVec3 &out ) const
{
    out.Scale   ( vecs[ 0 ], in.x      );
    out.ScaleAdd( vecs[ 1 ], in.y, out );
    out.ScaleAdd( vecs[ 2 ], in.z, out );
}
*/

XMat3::XMat3( const XQuat &q )
{
    float x2 = q.x * 2.0f;
    float y2 = q.y * 2.0f;
    float z2 = q.z * 2.0f;

    float xy = q.x * y2;
    float xz = q.x * z2;
    float yz = q.y * z2;

    float wx = q.w * x2;
    float wy = q.w * y2;
    float wz = q.w * z2;

    float xs = q.x * x2;
    float ys = q.y * y2;
    float zs = q.z * z2;

    vecs[ 0 ].xyz( 1.f - ( ys + zs ), xy - wz, xz + wy );
    vecs[ 1 ].xyz( xy + wz, 1.f - ( xs + zs ), yz - wx );
    vecs[ 2 ].xyz( xz - wy, yz + wx, 1.f - ( xs + ys ) );
}

void XMat3::operator = ( const XEuler &e )
{
    e.AngleVectors ( &vecs[ 0 ], &vecs[ 1 ], &vecs[ 2 ] );
    vecs[ 1 ] *= -1.f;
}

void XMat3::Transpose( void )
{
    float f;
    int   i;
    int   i1;
   
    for( i = 0; i < 3; i++ ) 
    {
        for( i1 = i + 1; i1 < 3; i1++ ) 
        {
            f = vecs[ i ][ i1 ];
            vecs[ i ][ i1 ] = vecs[ i1 ][ i ];
            vecs[ i1 ][ i ] = f;
        }
    }
}

void XMat3::MultiplyMatrix( const XMat3 &in1, const XMat3 &in2 )
{
    int i;
    int i1;

    for( i = 0; i < 3; i++ )
    {
        for( i1 = 0; i1 < 3; i1++ )
        {
            vecs[ i ][ i1 ] = 
                in1.vecs[ i ][ 0 ] * in2.vecs[ 0 ][ i1 ] +
                in1.vecs[ i ][ 1 ] * in2.vecs[ 1 ][ i1 ] +
                in1.vecs[ i ][ 2 ] * in2.vecs[ 2 ][ i1 ];
        }
    }
}

void XMat3::MultiplyThis( const XMat3 &in ) 
{
    XVec3 tmp[ 3 ];
    int i;
    int i1;

    for( i = 0; i < 3; i++ )
    {
        for( i1 = 0; i1 < 3; i1++ )
        {
            tmp[ i ][ i1 ] = 
                vecs[ i ][ 0 ] * in.vecs[ 0 ][ i1 ] +
                vecs[ i ][ 1 ] * in.vecs[ 1 ][ i1 ] +
                vecs[ i ][ 2 ] * in.vecs[ 2 ][ i1 ];
        }
    }

    memcpy( vecs, tmp, sizeof( vecs ) );
}
