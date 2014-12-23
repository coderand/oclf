// This file is distributed under a MIT license. See LICENSE.txt for details.
//
// Outcast viewer by AND 2002
//

#include "common.h"
#include <string.h>
#include "oc3d.h"

//
// Constructor
//
OCViewer::OCViewer
    (
    )
{
    m_xvPos.xyz( 0, 0, 0 );
    m_xvTgt.xyz( 0, 0, 0 );
    m_fRoll = 0;
    m_fFOV = 90.f;
    m_fNearClip = 0.1f;
    m_fFarClip = 10000.f;
    m_fAspect = 0;
}

//
// Set rendering parameters
//
void OCViewer::SetRender
    (
    )
{
    static XMat4 mm;
    static XVec3 xvUpVector( 0, 1, 0 );

    D3DXMatrixLookAtLH( (D3DXMATRIX *)&m_mView, (D3DXVECTOR3 *)&m_xvPos, (D3DXVECTOR3 *)&m_xvTgt, (D3DXVECTOR3 *)&xvUpVector );
    XEuler eul( m_fRoll, 0, 0 );

    mm.setIdentity();
    mm = (XMat3)eul;

    m_mView *= mm;

    if ( m_fAspect == 0 ) m_fAspect = 480.f / 640.f;

    float fov = 0.5 * Deg2Rad( m_fFOV );
    float fCos = Cos( fov );
    float fSin = Sin( fov );
    float Q = ( m_fFarClip * fSin ) / ( m_fFarClip - m_fNearClip );

    memset( &m_mProj, 0, sizeof( m_mProj ) );
    m_mProj.floats[ 0 ][ 0 ] = fCos * m_fAspect / fSin;
    m_mProj.floats[ 1 ][ 1 ] = fCos / fSin;
    m_mProj.floats[ 2 ][ 2 ] = Q / fSin;
    m_mProj.floats[ 2 ][ 3 ] = fSin / fSin;
    m_mProj.floats[ 3 ][ 2 ] = -Q * m_fNearClip / fSin;

    m_mTransform = m_mView * m_mProj;
}

//
// Calculate view frustum for visibility testing
//
void OCViewer::CalculateFrustum
    (
    )
{
    XVec3 v, v1;
    XVec3 vpos, dpos;
    XMat3 mCam;
    float hfov = 0.5f * Deg2Rad( m_fFOV ); 
    float xfov = atan( ( 640.f/480.f ) * tan( hfov ) );

    XMat4 mView;
    mView = m_mView;

    v = m_xvTgt - m_xvPos;
    v.Normalize();

    vpos = m_xvPos;
    m_xpPlanes[ OC_FRUSTUM_NEAR ] = XPlane( vpos, v );

    vpos = m_xvPos + v * ( 860.f );
    m_xpPlanes[ OC_FRUSTUM_FAR ] = XPlane( vpos, -v );

    mCam = (XMat3)mView;
    mCam.Transpose();

    vpos.x = Cos( xfov );
    vpos.z = Sin( xfov );
    vpos.y = 0;
    mCam.UnprojectVector( vpos, v1 );
    v1.Normalize();
    m_xpPlanes[ OC_FRUSTUM_LEFT ] = XPlane( m_xvPos, v1 );

    vpos.x = -vpos.x;
    mCam.UnprojectVector( vpos, v1 );
    v1.Normalize();
    m_xpPlanes[ OC_FRUSTUM_RIGHT ] = XPlane( m_xvPos, v1 );

    vpos.x = 0;
    vpos.z = Sin( hfov );
    vpos.y = -Cos( hfov );
    mCam.UnprojectVector( vpos, v1 );
    v1.Normalize();
    m_xpPlanes[ OC_FRUSTUM_TOP ] = XPlane( m_xvPos, v1 );

    vpos.y = -vpos.y;
    mCam.UnprojectVector( vpos, v1 );
    v1.Normalize();
    m_xpPlanes[ OC_FRUSTUM_BUTTOM ] = XPlane( m_xvPos, v1 );
}

//
// Test frustum with bound box for intersection
//
bool OCViewer::TestWithBBox
    ( 
    const XBBox &bbox 
    )
{
    XVec3 v1, v2;
    float d1, d2;
    XPlane *pPlane;
    int i;

    if ( ((XBBox)bbox).TestPoint( m_xvPos ) )
        return true;

    for ( i = 1; i < 6; i++ )
    {
        pPlane = &m_xpPlanes[ i ];
        
        v1 = bbox.min;
        v2 = bbox.max;
        pPlane->ReorderBoundBox( v1, v2 );

        d1 = pPlane->DistanceFromPlane( v1 );
        if ( d1 < 0 )
        {
            d2 = pPlane->DistanceFromPlane( v2 );
            if ( d2 < 0 )
                return false;
        }
    }

    return true;
}
