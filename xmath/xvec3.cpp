// This file is distributed under a MIT license. See LICENSE.txt for details.
//
// XMath clases
//
// X Vector 3 class
//

#include "common.h"

#include "xvec3.h"
#include "xmatrix.h"
#include "xplane.h"
#include "xquat.h"
#include "xeuler.h"

float XVec3::toYaw( void )
{
    float yaw;

    if ( ( y == 0 ) && ( x == 0 ) )
    {
        yaw = 0;
    }
    else
    {
        yaw = ( float )( ( int )( atan2( y, x ) * 180 / M_PI ) );
        if ( yaw < 0 )
        {
            yaw += 360;
        }
    }

    return yaw;
}

float XVec3::toPitch( void )
{
    float forward;
    float pitch;
    
    if ( ( x == 0 ) && ( y == 0 ) )
    {
        if ( z > 0 )
        {
            pitch = 90;
        }
        else
        {
            pitch = 270;
        }
    }
    else
    {
        forward = ( float )sqrt( x * x + y * y );
        pitch = ( float )( ( int )( Atan2( z, forward ) * 180 / M_PI ) );
        if ( pitch < 0 )
        {
            pitch += 360;
        }
    }

    return -pitch;
}

//////////////////////////////////////////////
// don't use this method !!!, use toEuler
XVec3 XVec3::toAngles( void )
{
    float forward;
    float yaw, pitch;
    
    if ( ( x == 0 ) && ( y == 0 ) )
    {
        yaw = 0;
        if ( z > 0 )
        {
            pitch = -90;
        }
        else
        {
            pitch = 90;
        }
    }
    else
    {
        pitch=(Atan2(x, z)-(M_PI/2.0f))*180.0f/M_PI;
        forward=(float)sqrt(x*x+z*z);
        yaw=Atan2(y, forward)*180.0f/M_PI;
    }

    return XVec3( yaw, pitch, 0 );
}

void XVec3::RotatePointAroundVector( const XVec3 &dir, const XVec3 &point, float deg )
{
   XQuat about( dir );
   float thetaover2 = Deg2Rad( deg ) / 2.f;

   about.vec3().Normalize ();
   about.w = Cos ( thetaover2 );
   about.vec3() *= Sin( thetaover2 );

   *this = ( about * point * about.getInverse() ).vec3();
}

void XVec3::LerpVector( XVec3 &v1, XVec3 &v2, float t )
{
    float omega, cosom, sinom, scale0, scale1;

    v1.Normalize();
    v2.Normalize();

    cosom = v1 * v2;
    if ( ( 1.0 - cosom ) > LERP_EPS )
    {
        omega = Acos( cosom );
        sinom = Sin( omega );
        scale0 = Sin( ( 1.0 - t ) * omega ) / sinom;
        scale1 = Sin( t * omega ) / sinom;
    }
    else
    {
        scale0 = 1.0 - t;
        scale1 = t;
    }

    *this = v1 * scale0 + v2 * scale1;
}

XEuler XVec3::TreatAsAngles()
{
    return XEuler( x, z, y );
};

//////////////////////////////////////////////
// don't use this method !!!, use toEuler
XEuler XVec3::ToAngles()
{
    XVec3 vTemp = toAngles();
    return XEuler( vTemp.x, vTemp.y, vTemp.z );
};

float XVec3::DistanceToLine( XVec3 &xvLineStart, XVec3 &xvLineEnd )
{
    float scalar = ( *this - xvLineEnd ) DOT ( xvLineEnd - xvLineStart );

    if ( scalar != 0.f )
    {
        float bc2 = ( *this - xvLineEnd ) DOT ( *this - xvLineEnd );
        float q = bc2 * ( ( xvLineEnd - xvLineStart ) DOT ( xvLineEnd - xvLineStart ) );
        return Sqrt( bc2 * ( 1 - scalar * scalar / q ) );
    }
    else
    {
        return ( *this - xvLineEnd ) DOT ( *this - xvLineEnd );
    }
}


XEuler XVec3::ToEuler() const
{
    float forward;
    float yaw, pitch;
    
    if ( ( x == 0 ) && ( y == 0 ) )
        {
        yaw = 0;
        if ( z > 0 )
            {
            pitch = 90;
            }
        else
            {
            pitch = 270;
            }
        }
    else
    {
        yaw = atan2( y, x ) * 180 / M_PI;
        if ( yaw < 0 )
            {
            yaw += 360;
            }

        forward = ( float )sqrt( x * x + y * y );
        pitch = atan2( z, forward ) * 180 / M_PI;
        if ( pitch < 0 )
            {
            pitch += 360;
            }

    }

    return XEuler( yaw, -pitch, 0 );
}
