// This file is distributed under a MIT license. See LICENSE.txt for details.
//
// XMath clases
//
// X Quaternion class
//

#include "common.h"

#include "xquat.h"
#include "xmatrix.h"
#include "xeuler.h"

static const float s_slerpDelta = float( 1e-6 );

XQuat::XQuat( const XEuler &angles )
{
    XQuat working;

    vec3().xyz( 0.f, 1.f, 0.f );
    SetupRotation( angles.yaw );
    working.vec3().xyz( 1.f, 0.f, 0.f );
    working.SetupRotation( angles.pitch );
    *this *= working;
    working.vec3().xyz( 0.f, 0.f, 1.f );
    working.SetupRotation( angles.roll );
    *this *= working;
}

void XQuat::operator = ( const XMat3 &mat )
{
    float trace, s;

    trace = mat.vecs[ 0 ].x + mat.vecs[ 1 ].y + mat.vecs[ 2 ].z;

    if ( trace > 0.f )
    {
        s = Sqrt ( trace + 1.f );
        XQuat::w = s * 0.5f;
        s = 0.5f / s;

        XQuat::x = ( mat.vecs[ 2 ].y - mat.vecs[ 1 ].z ) * s;
        XQuat::y = ( mat.vecs[ 0 ].z - mat.vecs[ 2 ].x ) * s;
        XQuat::z = ( mat.vecs[ 1 ].x - mat.vecs[ 0 ].y ) * s;
    }
    else
    {
        int i, j, k;
        static int s_next[ 3 ] = { 1, 2, 0 };

        i = 0;
        if ( mat.vecs[ 1 ].y > mat.vecs[ 0 ].x )
        {
            i = 1;
        }
        if ( mat.vecs[ 2 ].z > mat.vecs[ i ][ i ] )
        {
            i = 2;
        }

        j = s_next[ i ];
        k = s_next[ j ];

        s = Sqrt ( ( mat.vecs[ i ][ i ] - ( mat.vecs[ j ][ j ] + mat.vecs[ k ][ k ] ) ) + 1.f );
        XQuat::operator[]( i ) = s * 0.5f;
        s = 0.5f / s;
        XQuat::w =               ( mat.vecs[ k ][ j ] - mat.vecs[ j ][ k ] ) * s;
        XQuat::operator[]( j ) = ( mat.vecs[ j ][ i ] + mat.vecs[ i ][ j ] ) * s;
        XQuat::operator[]( k ) = ( mat.vecs[ k ][ i ] + mat.vecs[ i ][ k ] ) * s;
   }
}

void XQuat::Slerp( const XQuat &from, const XQuat &to, float t )
{
    float omega, cosom, sinom, scale0, scale1;
    float scaleto;

    if ( from == to ) 
    {
        *this = from;
        return;
    }
   
    cosom = from.x * to.x + from.y * to.y + from.z * to.z + from.w * to.w;
    scaleto = 1.f;

    if ( cosom < 0.f )
    {
        cosom = -cosom;
        scaleto = -1.f;
    }

    if ( 1.f - cosom > s_slerpDelta )
    {
        omega = Acos( cosom );
        sinom = Sin( omega );
        scale0 = Sin( ( 1.f - t ) * omega ) / sinom;
        scale1 = Sin( t * omega ) / sinom;
    }
    else
    {
        scale0 = 1.f - t;
        scale1 = t;
    }

   *this = ( scale0 * from ) + ( scaleto * scale1 * to );
}
