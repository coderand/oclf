// This file is distributed under a MIT license. See LICENSE.txt for details.
//
// Outcast player by AND 2002,2003
//

#include "common.h"
#include <string.h>
#include "oc3d.h"

OCMeshCache *CActor::GetMesh
    (
    )
{
    if ( !pMesh )
    {
        pMesh = ocworld->AddMeshObject( pFileAMH );
        pAnim1 = NEW OCAnim;
        pAnim2 = NEW OCAnim;
        pAnim1->Load( pFileAnimRun );
        pAnim2->Load( pFileAnimStop );
    }

    return pMesh;
}

CActor OCWorld::g_pActors[] =
{
    CActor( "x_cutter1.amh", "hm_run_fast_nogun.anm", "hm_stop1.anm" ),
    CActor( "x_cutter2.amh", "hm_run_fast_nogun.anm", "hm_stop1.anm" ),
    CActor( "x_marion.amh", "marion_run.anm", "marion_stop.anm", 25.2f ),
    CActor( "x_xue.amh", "hm_run_fast_nogun.anm", "hm_stop1.anm" ),
    CActor( "x_primit.amh", "lz_run.anm", "lz_stop01.anm", 45.f ),
    CActor( "x_old_zork.amh", "lz_run.anm", "lz_stop01.anm", 45.f ),
    CActor( "x_farmer.amh", "lz_run.anm", "lz_stop01.anm", 45.f ),
    CActor( "x_fat_trader.amh", "lz_run.anm", "lz_stop01.anm", 45.f ),
    CActor( "x_fisher.amh", "lz_run.anm", "lz_stop01.anm", 45.f ),
    CActor( "x_slave.amh", "lz_run.anm", "lz_stop01.anm", 45.f ),
    CActor( "x_rchief.amh", "lz_run.anm", "lz_stop01.anm", 45.f ),
    CActor( "x_reb_ch.amh", "lz_run.anm", "lz_stop01.anm", 45.f ),
    CActor( "x_chief1.amh", "lz_run.anm", "lz_stop01.anm", 45.f ),
    CActor( "x_warrior1.amh", "lz_run.anm", "lz_stop01.anm", 45.f ),
    CActor( "x_bipod.amh", "bipod_run.anm", "bipod_stop.anm", 29.f ),
    CActor( "3_aquarex.amh", "gorgor_slow_walk.anm", "gorgor_hurt.anm" ),
    CActor( "x_ventilope.amh", "ventilope_fly.anm", "ventilop_stop2.anm", 60 ),
    CActor( "x_bird.amh", "bird_fly.anm", "bird_stop2.anm", 60 ),
    CActor( "x_gamor.amh", "gamor_run.anm", "gamor_relax1.anm" ),
    CActor( "x_spider.amh", "spider_walk.anm", "spider_stop.anm" ),
};

float LerpAngle
    (
    float a1,
    float a2,
    float time
    )
{
    a1 = fmodf( a1, 360.f );
    a2 = fmodf( a2, 360.f );

    if ( a2 < a1 )
    {
        float temp = a1;
        a1 = a2;
        a2 = temp;
        time = 1 - time;
    }

    if ( ( a2 - a1 ) < ( 360.f + a1 - a2) )
    {
        return a1 + time * ( a2 - a1 );
    }
    else
    {
        return a1 + 360.f + time * ( a2 - a1 - 360.f );
    }
}

//
// Constructor
//
OCDrivenPlayer::OCDrivenPlayer
    (
    )
{
    m_pViewer = NEW OCViewer;

    m_pViewer->m_fFarClip = 1000.f;
    m_pViewer->m_fNearClip = 0.1f;
    m_pViewer->m_fFOV = 52;
    m_xvVelocity.xyz( 0, 0, 0 );

    m_fRotX = 0;
    m_fRotXLast = 0;
    m_fRotY = 0;
    m_fRotYLast = 0;

    m_bInMove = false;
}

//
// Destructor
//
OCDrivenPlayer::~OCDrivenPlayer
    (
    )
{
    DEL( m_pViewer );
}

XVec3 TraceRay
    (
    XVec3 &from,
    XVec3 &to
    )
{
    XVec3 out = from;

    int i;
    int len = ( to - from ).Length();
    XVec3 dir = ( to - from ).getNormalized();

    for ( i = 0; i < len; i++ )
    {
        float h = ocworld->GetHeight( out.x, out.z );
        if ( out.y < h )
            break;

        out += dir;
    }

    if ( i == len )
        return to;

    return out - dir * 10;
}

//
// Update player motion from input
//
void OCDrivenPlayer::UpdateInput
    (
    OCInput *pInput
    )
{
    if ( !ocworld->IsLoaded() )
        return;

    XVec3 xvDir;
    XVec3 xvMouseD;
    XMat3 xe1, xe2;
    XVec3 xvMove, xvLMove;
    XVec3 xvForwardMove, xvSideMove, xvUpMove;

    // get data from input
    xvMouseD = pInput->GetMouseDelta();

    m_fRotXLast += 0.25f * (float)(-xvMouseD.x);
    m_fRotYLast += 0.25f * (float)xvMouseD.y;
    if ( m_fRotYLast < -89.f ) m_fRotYLast = -89.f;
    if ( m_fRotYLast > 89.f ) m_fRotYLast = 89.f;

    if ( !ocworld->m_b1STPerson )
    {
        if ( m_fRotYLast < 0 ) m_fRotYLast = 0;
        else
        if ( m_fRotYLast > 30 ) m_fRotYLast = 30;
    }

    static float dt = 0;
    static dword old_sv_time = 0;
    static dword new_sv_time = 0;
    static dword numframes = 0;

    static dword old_t = timeGetTime();
    dword new_t = timeGetTime();
    numframes++;
    if ( new_t - old_t >= 1 )
    {
        old_sv_time = old_t;
        new_sv_time = new_t;

        dt = 0.001f * (float)( new_t - old_t ) / (float)numframes;

        old_t = new_t;
        numframes = 0;
    }
    else
    {
        dt = 0.001f;
    }

    static dword old = timeGetTime();

    // do mouse smoothing
    double frac = dt * 20;
    if ( frac > 1 ) frac = 1;
    if ( frac < 0 ) frac = 0;
    frac = 1 - frac;
    m_fRotX = m_fRotX * (frac) + m_fRotXLast * ( 1 - frac );
    m_fRotY = m_fRotY * (frac) + m_fRotYLast * ( 1 - frac );

    xe1 = XEuler( 0, m_fRotX, 0 );
    xe2 = XEuler( m_fRotY, 0, 0 );
    xe1 *= xe2;
    xe1.ProjectVector( XVec3( 1, 0, 0 ), xvDir );

    xvMove.xyz( 0, 0, 0 );
    xvLMove.xyz( 0, 0, 0 );
    xvForwardMove = xvDir;
    xvSideMove.CrossProduct( xvDir, XVec3( 0, 1, 0 ) );
    xvUpMove.xyz( 0, 1, 0 );

    if ( !ocworld->m_b1STPerson )
    {
        xvForwardMove.y = 0;
        xvForwardMove.Normalize();
        xvSideMove.y = 0;
        xvSideMove.Normalize();
    }

    if ( pInput->TestKey( DIK_W ) )
        xvLMove.x += 1;

    if ( pInput->TestKey( DIK_S ) )
        xvLMove.x -= 1;

    if ( pInput->TestKey( DIK_A ) )
        xvLMove.z += 1;

    if ( pInput->TestKey( DIK_D ) )
        xvLMove.z -= 1;

    if ( pInput->TestKey( DIK_C ) )
        xvLMove.y -= 1;

    if ( pInput->TestKey( DIK_E ) )
        xvLMove.y += 1;

    xvMove = xvLMove.x * xvForwardMove + xvLMove.y * xvUpMove + xvLMove.z * xvSideMove;

    xvMove.Normalize();
    xvLMove.Normalize();
    if ( ocworld->m_b1STPerson )
    {
        xvMove *= 400.f;
        m_xvVelocity = xvMove;
    }
    else
    {
        xvMove *= ocworld->g_pActors[ ocworld->m_nActorNum ].fSpeed;
        m_xvVelocity = xvMove;
    }

    // post update
    m_xvPos += m_xvVelocity * dt;

    m_bInMove = false;
    
    if ( ocworld->m_b1STPerson )
    {
        m_pViewer->m_xvPos = m_xvPos;
        m_pViewer->m_xvTgt = m_pViewer->m_xvPos + 100.f * xvDir;
    }
    else
    {
        m_xvPos.y = ocworld->GetHeight( m_xvPos.x, m_xvPos.z );

        {
            static float lastH = m_xvPos.y;
            double frac = dt * 10;
            if ( frac > 1 ) frac = 1;
            if ( frac < 0 ) frac = 0;
            lastH = ( 1 - frac ) * lastH + frac * m_xvPos.y;
            m_xvPos.y = lastH;
        }

        XVec3 offs( 0, 20, 0 );

        double frac = dt;
        if ( frac > 1 ) frac = 1;
        if ( frac < 0 ) frac = 0;

        static XVec3 cm = xvMove;
        cm = ( 1 - frac ) * cm + frac * xvMove;
        XVec3 cp;
        cp = m_xvPos + 0.35f * cm;

        static float fCameraDist = 50.f;

        static float cdst = 50.f;
        if ( ocinput->TestKey( DIK_HOME ) ) cdst -= 0.5f;
        if ( ocinput->TestKey( DIK_END ) ) cdst += 0.5f;
        offs.y = 20 + 0.5f * cdst / ( cdst + 50.f );

        XVec3 tp = TraceRay( offs + cp, offs + cp - cdst * xvDir );
        float newdist = ( tp - ( offs + cp ) ).Length();
        double cfrac = dt * ( newdist > fCameraDist ? 1 : 30 );
        if ( cfrac > 1 ) cfrac = 1;
        if ( cfrac < 0 ) cfrac = 0;
        fCameraDist = ( 1 - cfrac ) * fCameraDist + cfrac * newdist;

        static XVec3 lps( 0, 0, 0 );
        XVec3 corg = offs + cp;

        m_pViewer->m_xvPos = corg - fCameraDist * xvDir;
        m_pViewer->m_xvTgt = corg;

        static XVec3 lastMove;
        static float fRot = 0.0f;
        {
            XVec3 vel = xvLMove;
            if ( xvLMove.x == 0 && xvLMove.z == 0 )
                vel = lastMove;
            else
            {
                lastMove = xvLMove;
                m_bInMove = true;
            }

            vel.y = 0;
            vel.Normalize();
            XEuler er = vel.ToAngles();
            XEuler eu = xvForwardMove.ToAngles();

            float fMoveRot = -er.pitch - 90;
            float fCameraRot = - eu.pitch;

            static float fMRot = 0;
            static float fMRotFrom = 0;
            static float fOldMRot = 0;

            if ( m_bInMove )
            {
                float fMTime;
                static dword dwStartMTime = sys_dwtimeCounter;

                dword ct = sys_dwtimeCounter;
                fMTime = 3.5f * 0.001f * float( ct - dwStartMTime );
                if ( fMTime > 1 )
                    fMTime = 1;

                if ( fOldMRot != fMoveRot )
                {
                    fMRotFrom = fMRot;
                    fOldMRot = fMoveRot;
                    dwStartMTime = ct;
                    fMTime = 0;
                }

                fMRot = LerpAngle( fMRotFrom, fMoveRot, fMTime );
            }
            else
            {
                fOldMRot = fMRot;
            }

            fRot = fMRot + fCameraRot;
            
            m_xeRot.setYPR( fRot, 0, 0 );
        }
    }
}

//
// Render a player
//
void OCDrivenPlayer::Render
    (
    int nPassType
    )
{
    if ( ocworld->m_b1STPerson )
        return;

    static OCMeshObject *pObj = NULL;
    static OCAnim *pAnimLast = NULL;
    static dword fStartAnimTime = sys_dwtimeCounter;
    static dword fStartAnimTime2 = sys_dwtimeCounter;
    static dword fStartBlendTime = sys_dwtimeCounter;
    static int lastActor = -1;

    if ( ocworld->m_nActorNum != lastActor )
    {
        lastActor = ocworld->m_nActorNum;
        pAnimLast = NULL;
    }

    float fBlendSpeed = 5.f;

    if ( !pObj )
        pObj = NEW OCMeshObject;

    pObj->m_pMesh = ocworld->g_pActors[ ocworld->m_nActorNum ].GetMesh();
    pObj->m_pAnim = m_bInMove ? ocworld->g_pActors[ ocworld->m_nActorNum ].pAnim1 : ocworld->g_pActors[ ocworld->m_nActorNum ].pAnim2;
    pObj->m_pAnim2 = !m_bInMove ? ocworld->g_pActors[ ocworld->m_nActorNum ].pAnim1 : ocworld->g_pActors[ ocworld->m_nActorNum ].pAnim2;
    if ( pAnimLast != pObj->m_pAnim )
    {
        pAnimLast = pObj->m_pAnim;
        fStartAnimTime2 = fStartAnimTime;
        fStartAnimTime = sys_dwtimeCounter;
        
        fStartBlendTime = sys_dwtimeCounter - ( 1000.f / fBlendSpeed ) * ( 1 - pObj->m_fBlend );
    }
    pObj->m_fFrame = 20.f * 0.001f * float( sys_dwtimeCounter - fStartAnimTime );
    pObj->m_fFrame2 = 20.f * 0.001f * float( sys_dwtimeCounter - fStartAnimTime2 );
    pObj->m_fBlend = fBlendSpeed * 0.001f * float( (int)sys_dwtimeCounter - (int)fStartBlendTime );
    if ( pObj->m_fBlend < 0 )
        pObj->m_fBlend = 0;
    if ( pObj->m_fBlend > 1 )
        pObj->m_fBlend = 1;

    pObj->m_xvPos = m_xvPos * ( 1.f / 0.125f );
    pObj->m_xeRot.setYPR( m_xeRot.yaw, m_xeRot.pitch, m_xeRot.roll );

    pObj->UpdateTransform();

    pObj->Render( nPassType );
}
