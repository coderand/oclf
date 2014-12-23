// This file is distributed under a MIT license. See LICENSE.txt for details.
//
// XMath clases
//
// X Matrix class ( header )
//

#ifndef __XMATRIX_H__
#define __XMATRIX_H__

#include <string.h>
#include "xvec3.h"
#include "xmath.h"

class XQuat;
class XEuler;
class XMat3;
class xMat4;

class XMat4
{
public:

    // constructors
    XMat4();
    XMat4( const XMat4 &copy );
    XMat4( const XMat3 & );
    XMat4( const XQuat & );

    union
    {
       float floats[ 4 ][ 4 ];

        struct
        {
            float	_11, _12, _13, _14;
            float	_21, _22, _23, _24;
            float	_31, _32, _33, _34;
            float	_41, _42, _43, _44;
        };
    };

    void setIdentity();
    void Transpose();

    static XMat4 Ortho( float left, float right, float bottom, float top, float nearz, float farz );
    static XMat4 Frustum( float left, float right, float bottom, float top, float nearz, float farz );
    static XMat4 Translate( float x, float y, float z );
    inline static XMat4 Translate( const XVec3 &in )
    {
        return Translate( in.x, in.y, in.z );
    }
    
    void MultiplyMatrix( const XMat4 &in1, const XMat4 &in2 );
    void MultiplyThis( const XMat4 &in );
    float MultiplyVector( const XVec3 &in, XVec3 &out ) const;

    friend XMat4 operator * ( const XMat4 &in1, const XMat4 &in2 );
    XMat4 &operator *= ( const XMat4 &in );
    void operator = ( const XMat4 &in );
    operator const float * ();
};

class XMat3
{
public:

    // constructors
    XMat3();
    XMat3( const XQuat & );
    XMat3( const XMat4 & );
    
    XVec3 vecs[ 3 ];

    float *getFloats();
    void setIdentity();
    void operator = ( const XEuler & );

    void MultiplyMatrix( const XMat3 &, const XMat3 & );
    void MultiplyThis( const XMat3 & );
    void ProjectVector( const XVec3 &in, XVec3 &out ) const;
    void UnprojectVector( const XVec3 &in, XVec3 &out ) const;
    void OrthoNormalize();
    void Transpose();

    friend XMat3 operator * ( const XMat3 &in1, const XMat3 &in2 );
    XMat3 &operator *= ( const XMat3 &in );
    operator const float * ();
};

// implementation

inline XMat4::XMat4()
{
}

inline XMat4::XMat4( const XMat4 &copy )
{
    memcpy( floats, copy.floats, sizeof( floats ) );
}

inline XMat4::XMat4( const XQuat &q )
{
    *this = static_cast< XMat3 >( q );
}


inline XMat4 operator * ( const XMat4 &in1, const XMat4 &in2 ) 
{
    XMat4 that;

    that.MultiplyMatrix( in1, in2 );
    return that;
}

inline XMat4 &XMat4::operator *= ( const XMat4 &in ) 
{
    MultiplyThis( in );

    return *this;
}

inline void XMat4::operator = ( const XMat4 &in ) 
{
    memcpy( &floats[ 0 ][ 0 ], &in._11, sizeof( XMat4 ) );
}

inline XMat4::operator const float * ( void )
{
    return &floats[ 0 ][ 0 ];
}

inline XMat3::XMat3( void )
{
}

inline float *XMat3::getFloats ( void )
{
    return &vecs[ 0 ].x;
}

inline void XMat3::setIdentity ( void )
{
    vecs[ 0 ].xyz( 1.f, 0.f, 0.f );
    vecs[ 1 ].xyz( 0.f, 1.f, 0.f );
    vecs[ 2 ].xyz( 0.f, 0.f, 1.f );
}

inline void XMat3::OrthoNormalize( void )
{
    vecs[ 0 ].Normalize();
    vecs[ 2 ].CrossProduct( vecs[ 0 ], vecs[ 1 ] );
    vecs[ 2 ].Normalize();
    vecs[ 1 ].CrossProduct( vecs[ 2 ], vecs[ 0 ] );
    vecs[ 1 ].Normalize();
}

inline XMat3 operator * ( const XMat3 &in1, const XMat3 &in2 ) 
{
    XMat3 that;

    that.MultiplyMatrix( in1, in2 );
    return that;
}

inline XMat3 &XMat3::operator *= ( const XMat3 &in )
{
    MultiplyThis( in );
    return *this;
}

inline XMat3::operator const float * ( void )
{
    return vecs[ 0 ].vec3();
}

inline void XMat3::ProjectVector( const XVec3 &in, XVec3 &out ) const
{
    out.x = in DOT vecs[ 0 ];
    out.y = in DOT vecs[ 1 ];
    out.z = in DOT vecs[ 2 ];
}

inline void XMat3::UnprojectVector( const XVec3 &in, XVec3 &out ) const
{
    out.Scale   ( vecs[ 0 ], in.x      );
    out.ScaleAdd( vecs[ 1 ], in.y, out );
    out.ScaleAdd( vecs[ 2 ], in.z, out );
}

inline XMat3::XMat3( const XMat4 &m )
{
    vecs[ 0 ].x = m.floats[ 0 ][ 0 ];
    vecs[ 0 ].y = m.floats[ 0 ][ 1 ];
    vecs[ 0 ].z = m.floats[ 0 ][ 2 ];
    vecs[ 1 ].x = m.floats[ 1 ][ 0 ];
    vecs[ 1 ].y = m.floats[ 1 ][ 1 ];
    vecs[ 1 ].z = m.floats[ 1 ][ 2 ];
    vecs[ 2 ].x = m.floats[ 2 ][ 0 ];
    vecs[ 2 ].y = m.floats[ 2 ][ 1 ];
    vecs[ 2 ].z = m.floats[ 2 ][ 2 ];
}

#endif __XMATRIX_H__
