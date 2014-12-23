// This file is distributed under a MIT license. See LICENSE.txt for details.
//
// Outcast MSH cache object by AND 2002
//

#include "common.h"
#include <string.h>
#include "oc3d.h"

inline DWORD F2DW( FLOAT f ) { return *((DWORD*)&f); }

//
// Constructor
//
OCMeshCache::OCMeshCache
    (
    )
{
    m_pVerts = NULL;
    m_pFaces = NULL;
    m_pVertsMesh = NULL;
    m_pVB = NULL;
    m_pIB = NULL;
    m_pMESHData = NULL;
    memset( m_phTexture, 0, sizeof( m_phTexture ) );
}

//
// Destructor
//
OCMeshCache::~OCMeshCache
    (
    )
{
    int i, j;

    for ( i = 0; i < m_OCMesh.m_nNumTexInfos; i++ )
        for ( j = 0; j < 4; j++ )
            x3d->UnregisterTexture( m_phTexture[ i ][ j ] );

    VDEL( m_pVerts );
    VDEL( m_pFaces );
    VDEL( m_pVertsMesh );
    DEL( m_pVB );
    DEL( m_pIB );
    FREE( m_pMESHData );
}

//
// Initialize
//
void OCMeshCache::Init
    (
    )
{
    int i, j;

    DPrint( "OCMeshCache::Init() : " );
    DPrint( m_chName );
    DPrint( "\n" );

    m_nNumVerts = m_OCMesh.m_pVerts->m_nNumVerts;
    m_nNumFaces = m_OCMesh.m_pPolygons->m_nNumPolygons;

    memset( m_pStartIndex, 0, sizeof( m_pStartIndex ) );
    memset( m_pStripeLength, 0, sizeof( m_pStripeLength ) );

    m_pVerts = NULL;
    m_pFaces = NULL;
    m_pVertsMesh = NULL;

    if ( m_nNumFaces == 0 || m_nNumVerts == 0 )
        return;

    m_pVertsMesh = NEW TAMHProcVertex[ m_nNumVerts ];
    m_pVerts = NEW TMSHVertex_opt[ m_nNumFaces * 3 ];
    m_pFaces = NEW TMSHFace_opt[ m_nNumFaces ];

    bool bDynamic = m_OCMesh.m_pHeader->dwTypeID == OC_FILEID_AMH;
    ReconstructMeshData( !bDynamic );

    // create vertex buffer
    m_pVB = NEW CVBuffer;
    if ( bDynamic )
    {
        m_pVB->Create( pDev, m_nNumVerts, FVF_OCAMHVertex, sizeof( OCAMHVertex ), bDynamic );
    }
    else
    {
        m_pVB->Create( pDev, m_nNumVerts, FVF_OCMSHVertex, sizeof( OCMSHVertex ), bDynamic );

        OCMSHVertex *v = (OCMSHVertex *)m_pVB->Lock( m_nNumVerts );
        for ( i = 0; i < m_nNumVerts; i++ )
        {
            v[ i ].x = m_pVerts[ i ].x;
            v[ i ].y = m_pVerts[ i ].y;
            v[ i ].z = m_pVerts[ i ].z;
            v[ i ].u = m_pVerts[ i ].u;
            v[ i ].v = m_pVerts[ i ].v;
        }
        m_pVB->Unlock();
    }

    // create index buffer
    m_pIB = NEW CIBuffer;
    m_pIB->Create( pDev, 3 * m_nNumFaces );
    if ( m_nNumFaces )
    {
        short *idx = (short *)m_pIB->Lock( 3 * m_nNumFaces );
        int pos = 0;
        for ( i = 0; i < m_nNumFaces; i++ )
        {
            idx[ pos++ ] = m_pFaces[ i ].idx[ 0 ];
            idx[ pos++ ] = m_pFaces[ i ].idx[ 1 ];
            idx[ pos++ ] = m_pFaces[ i ].idx[ 2 ];
        }
        m_pIB->Unlock();
    }

    // load textures
    char texname[ 256 ];
    char texname2[ 256 ];

    for ( i = 0; i < m_OCMesh.m_nNumTexInfos; i++ )
        for ( j = 0; j < 4; j++ )
        if ( m_OCMesh.m_pTexinfos[ i ].pFileNames[ j ]->m_nLength )
        {
            strcpy( texname, m_chName );
            m_OCMesh.m_pTexinfos[ i ].pFileNames[ j ]->CopyTo( texname2 );

            xfs->CutFileName( texname );
            strcat( texname, texname2 );
            strcat( texname, ".btm" );

            // try find this file in mesh directory
            if ( xfs->Find( texname ) )
            {
                m_phTexture[ i ][ j ] = x3d->RegisterTexture( texname, bDynamic ? ERTF_BUMPMAP : ERTF_DEFAULT );
            }
            else
            {
                // so, let's try find this texture in landscape directory

                if ( ocworld->IsLoaded() )
                {
                    strcpy( texname, ocworld->GetMapName() );
                    xfs->CutFileName( texname );
                    strcat( texname, texname2 );
                    strcat( texname, ".btm" );

                    if ( xfs->Find( texname ) )
                        m_phTexture[ i ][ j ] = x3d->RegisterTexture( texname, ERTF_CLAMPSQUAREX | ERTF_SQUARESWAPXY );
                    else
                        m_phTexture[ i ][ j ] = 0;
                }
                else
                {
                    m_phTexture[ i ][ j ] = 0;
                }
            }
        }
}

//
// Reconstruct mesh data for best optimization
//
#define UNIQ_TEX_COORD 65536.f

int OCMeshCache_FaceSort_Cmp
    (
    const void *a,
    const void *b
    )
{
    return ((TMSHFace_opt *)a)->textureID - ((TMSHFace_opt *)b)->textureID;
}

void OCMeshCache::ReconstructMeshData
    (
    bool bDoSort
    )
{
    int i, j, fac, vi, index, dx;
    int nNumVerts;
    bool bTestResult;

#if 1
    
    {
        m_nNumVerts = 3 * m_nNumFaces;
        int nv = 0;
        for ( i = 0; i < m_nNumFaces; i++ )
        {
            m_pFaces[ i ].textureID = m_OCMesh.m_pPolygons->m_Polygons[ i ].m_bTextureID;
            
            for ( j = 0; j < 3; j++ )
            {
                int idx = m_OCMesh.m_pPolygons->m_Polygons[ i ].m_wVertIdx[ j ];
                m_pVerts[ nv ].x = m_OCMesh.m_pVerts->m_Verts[ idx ].x;
                m_pVerts[ nv ].y = m_OCMesh.m_pVerts->m_Verts[ idx ].y;
                m_pVerts[ nv ].z = m_OCMesh.m_pVerts->m_Verts[ idx ].z;
                m_pVerts[ nv ].nx = m_OCMesh.m_pVertNormals->m_Normals[ idx ].x;
                m_pVerts[ nv ].ny = m_OCMesh.m_pVertNormals->m_Normals[ idx ].y;
                m_pVerts[ nv ].nz = m_OCMesh.m_pVertNormals->m_Normals[ idx ].z;
                m_pVerts[ nv ].u = m_OCMesh.m_pTexCoords->m_Coords[ i ][ j ].u;
                m_pVerts[ nv ].v = m_OCMesh.m_pTexCoords->m_Coords[ i ][ j ].v;
                m_pFaces[ i ].idx[ j ] = nv++;
            }
        }
    }

#else

    // create initial vertex list
    nNumVerts = m_nNumVerts;
    for ( i = 0; i < nNumVerts; i++ )
    {
        m_pVerts[ i ].x = m_OCMesh.m_pVerts->m_Verts[ i ].x;
        m_pVerts[ i ].y = m_OCMesh.m_pVerts->m_Verts[ i ].y;
        m_pVerts[ i ].z = m_OCMesh.m_pVerts->m_Verts[ i ].z;
        m_pVerts[ i ].u = UNIQ_TEX_COORD;
        m_pVerts[ i ].next = -1;
    }

    // create initial face list
    for ( i = 0; i < m_nNumFaces; i++ )
    {
        m_pFaces[ i ].textureID = m_OCMesh.m_pPolygons->m_Polygons[ i ].m_bTextureID;
        m_pFaces[ i ].idx[ 0 ] = m_OCMesh.m_pPolygons->m_Polygons[ i ].m_wVertIdx[ 0 ];
        m_pFaces[ i ].idx[ 1 ] = m_OCMesh.m_pPolygons->m_Polygons[ i ].m_wVertIdx[ 1 ];
        m_pFaces[ i ].idx[ 2 ] = m_OCMesh.m_pPolygons->m_Polygons[ i ].m_wVertIdx[ 2 ];
    }

    for ( fac = 0; fac < m_nNumFaces; fac++ )
    {
        for ( vi = 0; vi < 3; vi++ )
        {
            index = m_pFaces[ fac ].idx[ vi ];

            bTestResult = false;
            while ( 1 )
            {
                bTestResult = ( ( m_pVerts[ index ].u == m_OCMesh.m_pTexCoords->m_Coords[ fac ][ vi ].u ) &&
                                ( m_pVerts[ index ].v == m_OCMesh.m_pTexCoords->m_Coords[ fac ][ vi ].v ) );

                if ( bTestResult )
                {
                    m_pFaces[ fac ].idx[ vi ] = index;
                    break;
                }

                if ( m_pVerts[ index ].next == -1 )
                    break;

                index = m_pVerts[ index ].next;
            }

            if ( !bTestResult )
            {
                dx = 0;
                if ( m_pVerts[ index ].u != UNIQ_TEX_COORD )
                {
                    m_pVerts[ nNumVerts ] = m_pVerts[ index ];
                    m_pVerts[ nNumVerts ].next = -1;
                    dx = nNumVerts - index;
                    m_pFaces[ fac ].idx[ vi ] = nNumVerts;
                    m_pVerts[ index ].next = nNumVerts;
                    nNumVerts++;
                }
                else
                    m_pFaces[ fac ].idx[ vi ] = index;

                m_pVerts[ index + dx ].u = m_OCMesh.m_pTexCoords->m_Coords[ fac ][ vi ].u;
                m_pVerts[ index + dx ].v = m_OCMesh.m_pTexCoords->m_Coords[ fac ][ vi ].v;
            }
        }
    }

    m_nNumVerts = nNumVerts;

#endif

    if ( !bDoSort )
        return;

    //
    // Sort faces by textures into strips
    //
    if ( bDoSort )
        qsort( m_pFaces, m_nNumFaces, sizeof( TMSHFace_opt ), OCMeshCache_FaceSort_Cmp );

    index = 0;
    fac = 0;
    vi = 0;
    while ( fac < m_nNumFaces )
    {
        dx = m_pFaces[ fac ].textureID;

        while ( fac < m_nNumFaces && m_pFaces[ fac ].textureID == dx )
            fac++;

        m_pStartIndex[ index ] = vi * 3;
        m_pStripeLength[ index ] = ( fac - vi ) | ( dx << 16 );

        vi = fac;
        index++;
    }

    m_nNumSurfaces = index;
}


//
// Get number of faces
//
int	OCMeshObject::GetNumFaces
    (
    )
{
    if ( m_pMesh )
        return m_pMesh->m_nNumFaces;
    else
        return 0;
}

//
// Update transformation
//
void OCMeshObject::UpdateTransform
    (
    )
{
    XMat4 mRot;

    mRot = (XMat4)m_xeRot;

    m_mWorld.setIdentity();
    m_mWorld._41 = m_xvPos.x * 0.125f;
    m_mWorld._42 = m_xvPos.y * 0.125f;
    m_mWorld._43 = m_xvPos.z * 0.125f;
    m_mWorld._11 = m_mWorld._22 = m_mWorld._33 = 0.125f;
    m_mWorld = mRot * m_mWorld;
}

//
// Initialize mesh object after creation
//
void OCMeshObject::Init
    (
    )
{
    UpdateTransform();

    int i;
    XVec3 v, tv;

    // calculate transformed bbox
    for ( i = 0; i < m_pMesh->m_OCMesh.m_pVerts->m_nNumVerts; i++ )
    {
        v.xyz( m_pMesh->m_OCMesh.m_pVerts->m_Verts[ i ].x, m_pMesh->m_OCMesh.m_pVerts->m_Verts[ i ].y, m_pMesh->m_OCMesh.m_pVerts->m_Verts[ i ].z );
        m_mWorld.MultiplyVector( v, tv );

        if ( !i )
        {
            m_xbBox.min = tv;
            m_xbBox.max = tv;
        }
        else
            m_xbBox.AddPoint( tv );
    }
}

//
// Render MSH object
//
void OCMeshObject::Render
    (
    int nPassType
    )
{
    if ( m_pMesh->m_nNumFaces == 0 || m_pMesh->m_nNumVerts == 0 )
        return;

    pDev->SetTransform( D3DTS_WORLD, (D3DMATRIX *)&m_mWorld );

    if ( m_pMesh->m_OCMesh.m_pHeader->dwTypeID == OC_FILEID_AMH )
    {
        m_pMesh->RenderAMH( this, nPassType, m_pAnim, m_fFrame, m_pAnim2, m_fFrame2, m_fBlend );
        return;
    }

    pDev->SetVertexShader( NULL );
    pDev->SetFVF( FVF_OCMSHVertex );
    pDev->SetPixelShader( NULL );
    pDev->SetStreamSource( 0, m_pMesh->m_pVB->GetInterface(), 0, sizeof( OCMSHVertex ) );
    pDev->SetIndices( m_pMesh->m_pIB->GetInterface() );

    rend_SetRenderState( D3DRS_LIGHTING, false );
    rend_SetRenderState( D3DRS_FOGTABLEMODE, D3DFOG_LINEAR );

    int nMod = 2;
    for ( int i = 0; i < m_pMesh->m_nNumSurfaces; i++ )
    {
        x3d->SetTexture( 0, m_pMesh->m_phTexture[ m_pMesh->m_pStripeLength[ i ] >> 16 ][ nMod ] );
        pDev->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, m_pMesh->m_nNumVerts, m_pMesh->m_pStartIndex[ i ], m_pMesh->m_pStripeLength[ i ] & 0xffff );
    }
}

//
// Render AMH object
//

#define ANIM_SHORT_TO_RAD(s) ((float)((float)(s) * 9.58752621833e-05))

XBBox g_BBox;

void OCMeshCache::ApplyXForm
    (
    int startV,
    int endV,
    OCMtx mat
    )
{
    for ( int i = startV; i < endV; i++ )
    {
        OCVec v( m_OCMesh.m_pVerts->m_Verts[ i ].x, m_OCMesh.m_pVerts->m_Verts[ i ].y, m_OCMesh.m_pVerts->m_Verts[ i ].z );
        OCVec u = mat.apply( v );

        OCVec n( m_OCMesh.m_pVertNormals->m_Normals[ i ].x, m_OCMesh.m_pVertNormals->m_Normals[ i ].y, m_OCMesh.m_pVertNormals->m_Normals[ i ].z );
        OCVec t;

        t.x = mat.xx * n.x + mat.xy * n.y + mat.xz * n.z;
        t.y = mat.yx * n.x + mat.yy * n.y + mat.yz * n.z;
        t.z = mat.zx * n.x + mat.zy * n.y + mat.zz * n.z;

        m_pVertsMesh[ i ].xyz( u.x, u.y, u.z );
        m_pVertsMesh[ i ].n.xyz( t.x, t.y, t.z );

        g_BBox.AddPoint( XVec3( u.x, u.y, u.z ) );
    }
}

void OCMeshCache::RenderAMH
    (
    OCMeshObject *pObj,
    int nPassType,
    OCAnim *pAnim,
    float fFrame,
    OCAnim *pAnim2,
    float fFrame2,
    float fBlend
    )
{
    pDev->SetStreamSource( 0, m_pVB->GetInterface(), 0, sizeof( OCAMHVertex ) );
    pDev->SetIndices( m_pIB->GetInterface() );

    rend_SetRenderState( D3DRS_LIGHTING, false );
    rend_SetRenderState( D3DRS_FOGTABLEMODE, D3DFOG_LINEAR );

    //
    // Process animation
    //
    XVec3 delta( 0, 0, 0 );
    XVec3 delta2( 0, 0, 0 );

    g_BBox.min.xyz( 1e30f, 1e30f, 1e30f );
    g_BBox.max.xyz( -1e30f, -1e30f, -1e30f );

    {
        int i;

        int frame = fmodf( fFrame, pAnim->m_ANM.m_Header.wNumFrames );
        int frame2 = fmodf( fFrame2, pAnim2->m_ANM.m_Header.wNumFrames );
        float fFrameLT = fFrame - (int)fFrame;
        float fFrameLT2 = fFrame2 - (int)fFrame2;

        assert( pAnim );

        pAnim->m_mStack[ 0 ].setid();
        pAnim2->m_mStack[ 0 ].setid();
        pAnim->m_xvBoneStack[ 0 ] = OCVec( 0, 0, 0 );
        pAnim2->m_xvBoneStack[ 0 ] = OCVec( 0, 0, 0 );

        static bool bDebug = false;

        if ( bDebug )
        {
            DPrint( "Debug AMH\n" );
            DPrint( "Mesh  : %s\n", m_chName );
            DPrint( "Anim  : numJoints = %d / %d\n", (dword)pAnim->m_ANM.m_Header.wNumJoints, m_OCMesh.m_nNumLimbs );
            DPrint( "Anim2 : numJoints = %d / %d\n", (dword)pAnim2->m_ANM.m_Header.wNumJoints, m_OCMesh.m_nNumLimbs );
        }

        for ( i = 0; i < m_OCMesh.m_nNumLimbs; i++ )
        {
            const TAMHModelJoint &mj = m_OCMesh.m_pJoints[ i ];
            TAMHJoint &joint = m_OCMesh.m_pLimbs[ i ];

            OCMtx parentM = pAnim->m_mStack[ mj.index - 1 ];
            OCMtx parentM2 = pAnim2->m_mStack[ mj.index - 1 ];

            assert( mj.index - 1 >= 0 );

            OCVec bone( joint.xvPos.x, joint.xvPos.y, joint.xvPos.z );
            OCVec bone2( joint.xvPos.x, joint.xvPos.y, joint.xvPos.z );
            
            pAnim->m_xvBoneStack[ mj.index ] = bone;
            pAnim2->m_xvBoneStack[ mj.index ] = bone2;
            bone -= pAnim->m_xvBoneStack[ mj.index - 1 ];
            bone2 -= pAnim2->m_xvBoneStack[ mj.index - 1 ];

            XVec3 o( 0, 0, 0 );
            XVec3 o2( 0, 0, 0 );
            const TANMJoint &aj = pAnim->m_ANM.m_pJoints[ i ];
            const TANMJoint &aj2 = pAnim2->m_ANM.m_pJoints[ i ];

            if ( 0 != (aj.wMask & 7) )
                for ( int crd = 0; crd < 3; crd++ )
                    if ( 0 != (aj.wMask & ( 1 << crd ) ) )
                    {
                        float curr = ANIM_SHORT_TO_RAD( aj.os[ crd ] > frame ? aj.pwO[ crd ][ frame ] : aj.pwO[ crd ][ 0 ] );
                        float next = ANIM_SHORT_TO_RAD( aj.os[ crd ] > ( frame + 1 ) ? aj.pwO[ crd ][ frame + 1 ] : aj.pwO[ crd ][ 0 ] );
                        o[ crd ] = curr + fFrameLT * ( next - curr );

                        if ( i == 0 )
                        {
                            curr = aj.ds[ crd ] > frame ? aj.pfD[ crd ][ frame ] : aj.pfD[ crd ][ 0 ];
                            next = aj.ds[ crd ] > ( frame + 1 ) ? aj.pfD[ crd ][ frame + 1 ] : aj.pfD[ crd ][ 0 ];
                            delta[ crd ] = curr + fFrameLT * ( next - curr );
                        }
                    }

            if ( 0 != (aj2.wMask & 7) )
                for ( int crd = 0; crd < 3; crd++ )
                    if ( 0 != (aj2.wMask & ( 1 << crd ) ) )
                    {
                        float curr = ANIM_SHORT_TO_RAD( aj2.os[ crd ] > frame2 ? aj2.pwO[ crd ][ frame2 ] : aj2.pwO[ crd ][ 0 ] );
                        float next = ANIM_SHORT_TO_RAD( aj2.os[ crd ] > ( frame2 + 1 ) ? aj2.pwO[ crd ][ frame2 + 1 ] : aj2.pwO[ crd ][ 0 ] );
                        o2[ crd ] = curr + fFrameLT2 * ( next - curr );

                        if ( i == 0 )
                        {
                            curr = aj2.ds[ crd ] > frame2 ? aj2.pfD[ crd ][ frame2 ] : aj2.pfD[ crd ][ 0 ];
                            next = aj2.ds[ crd ] > ( frame2 + 1 ) ? aj2.pfD[ crd ][ frame2 + 1 ] : aj2.pfD[ crd ][ 0 ];
                            delta2[ crd ] = curr + fFrameLT2 * ( next - curr );
                        }
                    }

            OCQuat q = OCQuat::from_euler( o.x, o.y, o.z );
            OCMtx rotation = OCMtx::from_euler( o.x, o.y, o.z );
            pAnim->m_mStack[ mj.index ] = parentM * OCMtx::from_vector( bone ) * rotation;

            OCQuat q2 = OCQuat::from_euler( o2.x, o2.y, o2.z );
            OCMtx rotation2 = OCMtx::from_euler( o2.x, o2.y, o2.z );
            pAnim2->m_mStack[ mj.index ] = parentM2 * OCMtx::from_vector( bone2 ) * rotation2;

            if ( bDebug )
            {
                char name[ 256 ];
                mj.pName->CopyTo( name );
                DPrint( "joint #%d, kind %x, %d:\"%s\"\n", i, (dword)joint.dwKind, mj.index - 1, name );

                DPrint( "o %f %f %f, o2 %f %f %f\n", o.x, o.y, o.z, o2.x, o2.y, o2.z );
                DPrint( "aj.pwO  %x %x %x\n", aj.pwO[ 0 ][ 0 ], aj.pwO[ 1 ][ 0 ], aj.pwO[ 2 ][ 0 ] );
                DPrint( "aj2.pwO %x %x %x\n", aj2.pwO[ 0 ][ 0 ], aj2.pwO[ 1 ][ 0 ], aj2.pwO[ 2 ][ 0 ] );
            }

            if ( 1 == joint.dwKind >> 1 )
            {
                OCMtx m = (OCMtx::from_float9( joint.mRot )).transposed();
                q = OCQuat::from_matrix( m ) * q;
                q2 = OCQuat::from_matrix( m ) * q2;
                
                bone = bone + OCVec( joint.xvPivot.x, joint.xvPivot.y, joint.xvPivot.z );
                rotation = rotation * m;

                bone2 = bone2 + OCVec( joint.xvPivot.x, joint.xvPivot.y, joint.xvPivot.z );
                rotation2 = rotation2 * m;
            }

            OCMtx cmin = parentM * OCMtx::from_vector( bone );
            OCMtx env1 = cmin * rotation;

            OCMtx cmin2 = parentM2 * OCMtx::from_vector( bone2 );
            OCMtx env12 = cmin2 * rotation2;

            for ( int j = joint.dwClusterIndex; j < (int)( joint.dwClusterIndex + joint.dwClusterCount ); j++ )
            {
                TAMHCluster &c = m_OCMesh.m_pClusters[ j ];

                if ( c.fEnvelope < 0.8f )
                {
                    OCQuat cq = q.sscale( c.fEnvelope );
                    OCMtx cm = cq.to_matrix();

                    OCQuat cq2 = q2.sscale( c.fEnvelope );
                    OCMtx cm2 = cq2.to_matrix();
                    
                    OCMtx m1 = cmin * cm;
                    OCMtx m2 = cmin2 * cm2;

                    m1.scale( fBlend );
                    m2.scale( 1 - fBlend );
                    OCMtx m = m1;
                    m.add( m2 );

                    ApplyXForm( c.dwVertIndex, c.dwVertIndex + c.dwVertCount, m );
                }
                else
                {
                    OCMtx m1 = env1;
                    OCMtx m2 = env12;
                    m1.scale( fBlend );
                    m2.scale( 1 - fBlend );
                    OCMtx m = m1;
                    m.add( m2 );

                    ApplyXForm( c.dwVertIndex, c.dwVertIndex + c.dwVertCount, m );//env1 );
                }
            }
        }

        bDebug = false;
    }

    delta = delta2 + fBlend * ( delta - delta2 );

    int nv = 0;
    int i;
    for ( i = 0; i < m_nNumFaces; i++ )
        for ( int j = 0; j < 3; j++ )
        {
            int idx = m_OCMesh.m_pPolygons->m_Polygons[ i ].m_wVertIdx[ j ];
            
            m_pVerts[ nv ].x = m_pVertsMesh[ idx ].x;
            m_pVerts[ nv ].y = m_pVertsMesh[ idx ].y;
            m_pVerts[ nv ].z = m_pVertsMesh[ idx ].z;
            m_pVerts[ nv ].nx = m_pVertsMesh[ idx ].n.x;
            m_pVerts[ nv ].ny = m_pVertsMesh[ idx ].n.y;
            m_pVerts[ nv ].nz = m_pVertsMesh[ idx ].n.z;

            nv++;
        }

    XVec3 dir1( 0, 0, -1 );
    XVec3 dir;
    ((XMat3)pObj->m_mWorld).ProjectVector( dir1, dir );
    dir.Normalize();

    OCAMHVertex *v = (OCAMHVertex *)m_pVB->Lock( m_nNumVerts );
    for ( i = 0; i < m_nNumVerts; i++ )
    {
        v[ i ].x = m_pVerts[ i ].x + delta.x;
        v[ i ].y = m_pVerts[ i ].y + delta.y;
        v[ i ].z = m_pVerts[ i ].z + delta.z;
        v[ i ].n.x = m_pVerts[ i ].nx;
        v[ i ].n.y = m_pVerts[ i ].ny;
        v[ i ].n.z = m_pVerts[ i ].nz;
        v[ i ].u = m_pVerts[ i ].u;
        v[ i ].v = m_pVerts[ i ].v;

        v[ i ].n.Normalize();
        v[ i ].u2 = 0.7f + 0.4f * ( v[ i ].n.x * dir.x + v[ i ].n.z * dir.z );
    }
    m_pVB->Unlock();

    bool bDoBump = g_bBumpMapping;

    if ( !bDoBump )
    {
        pDev->SetRenderState( D3DRS_LIGHTING, true );
        pDev->SetRenderState( D3DRS_SPECULARENABLE, true );
        pDev->SetRenderState( D3DRS_NORMALIZENORMALS, true );
        D3DLIGHT9 lt, lt2;
        memset( &lt, 0, sizeof( lt ) );
        lt.Type = D3DLIGHT_DIRECTIONAL;
        lt.Direction.x = 0;
        lt.Direction.y = 0;
        lt.Direction.z = -1;
        lt.Diffuse.r = .85f;
        lt.Diffuse.g = .82f;
        lt.Diffuse.b = .8f;
        lt.Ambient.r = .15f;
        lt.Ambient.g = .15f;
        lt.Ambient.b = .15f;
        lt.Specular.r = .2f;
        lt.Specular.g = .15f;
        lt.Specular.b = .1f;
        lt.Attenuation0 = 1;
        lt.Range = 100000;
        pDev->SetLight( 0, &lt );
        pDev->LightEnable( 0, true );
        lt2 = lt;
        lt2.Direction.z = 1;
        lt2.Diffuse.r = .5f;
        lt2.Diffuse.g = .45f;
        lt2.Diffuse.b = .4f;
        pDev->SetLight( 1, &lt2 );
        pDev->LightEnable( 1, true );

        D3DMATERIAL9 mat;
        memset( &mat, 0, sizeof( mat ) );
        mat.Diffuse.r = 1;
        mat.Diffuse.g = 1;
        mat.Diffuse.b = 1;
        mat.Ambient.r = 1;
        mat.Ambient.g = 1;
        mat.Ambient.b = 1;
        mat.Specular.r = 1;
        mat.Specular.g = 1;
        mat.Specular.b = 1;
        mat.Power = 4;
        pDev->SetMaterial( &mat );

        pDev->SetVertexShader( NULL );
        pDev->SetFVF( FVF_OCAMHVertex );
        pDev->SetPixelShader( NULL );

        int nMod = 2;
        for ( i = 0; i < m_nNumFaces; i++ )
        {
            x3d->SetTexture( 0, m_phTexture[ m_pFaces[ i ].textureID ][ nMod ] );
            
            pDev->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, m_nNumVerts, i * 3, 1 );
        }
        
        pDev->SetRenderState( D3DRS_LIGHTING, false );
    }
    else
    {
        x3d->SetDXShader( ocworld->m_hAMHBump_vso );
        x3d->SetDXShader( ocworld->m_hAMHBump_pso );
        pDev->SetVertexDeclaration( ocworld->m_pAMHVtxDecl );

        XMat4 mW = pObj->m_mWorld;
        XMat4 mVP = ocworld->GetViewer()->m_mTransform;
        mW.Transpose();
        mVP.Transpose();
        pDev->SetVertexShaderConstantF( 0, mW, 4 );
        pDev->SetVertexShaderConstantF( 4, mVP, 4 );

        static HX3DTexture m_hLUT = x3d->RegisterTexture( "luxsys/lut.tga", ERTF_NOMIP );
        x3d->SetTexture( 1, m_hLUT );
        pDev->SetSamplerState( 1, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
        pDev->SetSamplerState( 1, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP );
        pDev->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 1 );
        pDev->SetTextureStageState( 1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );

        x3d->dev->SetTextureStageState( 1, D3DTSS_BUMPENVMAT00, F2DW( -0.4f ) );
        x3d->dev->SetTextureStageState( 1, D3DTSS_BUMPENVMAT01, F2DW( 0 ) );
        x3d->dev->SetTextureStageState( 1, D3DTSS_BUMPENVMAT10, F2DW( 0 ) );
        x3d->dev->SetTextureStageState( 1, D3DTSS_BUMPENVMAT11, F2DW( 0 ) );

        int nMod = 2;

        for ( i = 0; i < m_nNumFaces; i++ )
        {
            x3d->SetTexture( 0, m_phTexture[ m_pFaces[ i ].textureID ][ nMod ], 1 );
            x3d->SetTexture( 2, m_phTexture[ m_pFaces[ i ].textureID ][ nMod ] );
            
            pDev->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, m_nNumVerts, i * 3, 1 );
        }

        pDev->SetPixelShader( NULL );
        pDev->SetSamplerState( 1, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP );
        pDev->SetSamplerState( 1, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP );
    }

    //
    // Render shadow
    //
    if ( g_bShadows )
    {
        static IDirect3DTexture9 *pShadowTex[ 2 ] = { NULL, NULL };
        static IDirect3DSurface9 *pShadowSurf[ 2 ] = { NULL, NULL };

        if ( !pShadowTex[ 0 ] )
        {
            DXFAIL( pDev->CreateTexture( 128, 128, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &pShadowTex[ 0 ], NULL ) );
            DXFAIL( pShadowTex[ 0 ]->GetSurfaceLevel( 0, &pShadowSurf[ 0 ] ) );
            DXFAIL( pDev->CreateTexture( 128, 128, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &pShadowTex[ 1 ], NULL ) );
            DXFAIL( pShadowTex[ 1 ]->GetSurfaceLevel( 0, &pShadowSurf[ 1 ] ) );
        }

        int currSTex = 0;

        pDev->SetRenderTarget( 0, pShadowSurf[ currSTex ] );
        pDev->Clear( 0, NULL, D3DCLEAR_TARGET, 0x00000000, 0, 0 );
        pDev->SetRenderState( D3DRS_LIGHTING, false );
        pDev->SetRenderState( D3DRS_ZENABLE, false );
        pDev->SetVertexShader( NULL );
        pDev->SetFVF( FVF_OCAMHVertex );
        pDev->SetPixelShader( NULL );
        pDev->SetTexture( 0, NULL );
        rend_SetRenderState( D3DRS_ALPHABLENDENABLE, false );
        rend_SetBlend( D3DBLEND_ONE, D3DBLEND_ZERO );

        // create camera
        XVec3 xvUpVector( 0, 1, 0 );
        XVec3 origin = 0.125f * 0.5f * ( g_BBox.min + g_BBox.max ) + 0.125f * pObj->m_xvPos;
        XVec3 from = origin + XVec3( 0, 100, 75 );
        XMat4 mView, mProj;
        float size = 0.0001f * 0.125f * ( g_BBox.max - g_BBox.min ).Length();
        D3DXMatrixLookAtLH( (D3DXMATRIX *)&mView, (D3DXVECTOR3 *)&from, (D3DXVECTOR3 *)&origin, (D3DXVECTOR3 *)&xvUpVector );
        D3DXMatrixPerspectiveLH( (D3DXMATRIX *)&mProj, size, size, 0.01f, 200.f );
        pDev->SetTransform( D3DTS_VIEW, (D3DMATRIX *)&mView );
        pDev->SetTransform( D3DTS_PROJECTION, (D3DMATRIX *)&mProj );

        rend_SetRenderState( D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_ALPHA );
        pDev->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, m_nNumVerts, 0, m_nNumFaces );
        rend_SetRenderState( D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_ALPHA | D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE );

        // blur shadow
        if ( g_bSoftShadows )
        {
            float ct = 0.5f / 256.f;
            TSpriteVertex vtx[ 4 ] = {
                { -1, 1, 0, 1, 0xffffffff, ct, ct },
                { 1, 1, 0, 1, 0xffffffff, 1 + ct, ct },
                { -1,  -1, 0, 1, 0xffffffff, ct, 1 + ct },
                { 1, -1, 0, 1, 0xffffffff, 1 + ct, 1 + ct },
            };

            x3d->SetDXShader( ocworld->m_hOCBlur_vso );
            x3d->SetDXShader( ocworld->m_hOCBlur_pso );
            pDev->SetVertexDeclaration( ocworld->m_pSpriteVtxDecl );

            float blurRadius = 0.008f;
            float float4[] = { blurRadius, blurRadius, 0, 0 };
            pDev->SetVertexShaderConstantF( 0, float4, 1 );

            int pass, i;
            for ( i = 0; i < 4; i++ )
            {
                pDev->SetTextureStageState( i, D3DTSS_TEXCOORDINDEX, i );
                pDev->SetTextureStageState( i, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
                pDev->SetSamplerState( i, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
                pDev->SetSamplerState( i, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );
            }

            for ( pass = 0; pass < 1; pass++ )
            {
                for ( i = 0; i < 4; i++ )
                    rend_SetTexture( i, pShadowTex[ pass & 1 ] );

                pDev->SetRenderTarget( 0, pShadowSurf[ ~pass & 1 ] );
                pDev->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, vtx, sizeof( TSpriteVertex ) );
            }

            currSTex = pass & 1;

            pDev->SetPixelShader( NULL );
            pDev->SetVertexShader( NULL );
            for ( i = 0; i < 4; i++ )
            {
                pDev->SetSamplerState( i, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP );
                pDev->SetSamplerState( i, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP );
            }
        }

        // restore
        pDev->SetRenderTarget( 0, m_pRenderSurface );

        rend_SetTexture( 0, pShadowTex[ currSTex ] );
        rend_SetBlend( D3DBLEND_SRCALPHA, D3DBLEND_INVSRCALPHA );
        pDev->SetRenderState( D3DRS_ZENABLE, true );
        pDev->SetTransform( D3DTS_VIEW, (D3DMATRIX *)&ocworld->GetViewer()->m_mView );
        pDev->SetTransform( D3DTS_PROJECTION, (D3DMATRIX *)&ocworld->GetViewer()->m_mProj );

        {
            pDev->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT3 | D3DTTFF_PROJECTED );
            pDev->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEPOSITION );

            XMat4 mv;
            D3DXMatrixInverse( (D3DXMATRIX *)&mv, NULL, (D3DXMATRIX *)&ocworld->GetViewer()->m_mView );

            XMat4 scale;
            scale.setIdentity();
            scale._11 = 0.5f;
            scale._22 = -0.5f;
            scale._41 = scale._42 = 0.5f;

            XMat4 vp = mv * mView * mProj * scale;
            pDev->SetTransform( D3DTS_TEXTURE0, (D3DMATRIX *)&vp );

            XBBox rbox = g_BBox;
            rbox.AddPoint( XVec3( rbox.min.x, rbox.min.y, -( rbox.max.y - rbox.min.y ) ) );
            rbox.AddPoint( XVec3( g_BBox.min.x, g_BBox.min.y - ( g_BBox.max.y - g_BBox.min.y ), g_BBox.min.z ) );
            rbox.AddPoint( XVec3( g_BBox.min.z, g_BBox.min.y, g_BBox.min.x ) );
            rbox.AddPoint( XVec3( g_BBox.max.z, g_BBox.max.y, g_BBox.max.x ) );
            rbox.min = 0.125f * ( rbox.min + pObj->m_xvPos );
            rbox.max = 0.125f * ( rbox.max + pObj->m_xvPos );

            rend_SetRenderState( D3DRS_ALPHABLENDENABLE, true );
            ocworld->RenderShadow( rbox );

            pDev->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
            pDev->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );
            pDev->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE );
            rend_SetRenderState( D3DRS_ALPHABLENDENABLE, false );
        }
    }
}

//
// Render projected shadow to visible surfaces
//
void OCWorld::RenderShadow
    ( 
    const XBBox &bbox 
    )
{
    for ( int ref = 0; ref < ocworld->m_nNumRefObjects; ref++ )
    {
        if ( m_pRefRenderSet[ ref ]->m_pObj->GetFlags() & EOBJF_RP_PROJSHADOW )
        if ( m_pRefRenderSet[ ref ]->m_pObj->m_xbBox.TestBBox( bbox ) )
        {
            m_pRefRenderSet[ ref ]->m_pObj->Render( EOBJF_RP_PROJSHADOW );
        }
    }
}
