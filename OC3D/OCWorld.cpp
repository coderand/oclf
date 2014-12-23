// This file is distributed under a MIT license. See LICENSE.txt for details.
//
// Outcast world by AND 2002,2003
//

#include "common.h"
#include <string.h>
#include "oc3d.h"

int tl = 0;

inline DWORD F2DW( FLOAT f ) { return *((DWORD*)&f); }

void RemoveFileName
    (
    char *path
    )
{
    int pos = strlen( path ) - 1;

    while ( pos >= 0 )
    {
        if ( ( path[ pos ] == '/' ) || ( path[ pos ] == '\\' ) )
        {
            path[ pos + 1 ] = 0;
            return;
        }

        pos--;
    }
}

//
// Common constructor
//
OCWorld::OCWorld
    (
    )
{
    m_nActorNum = 0;
    m_b1STPerson = true;
    m_pWaterVB = NULL;
    m_pWaterIB = NULL;
    m_pLNDFile = NULL;
    m_pLND = NULL;
    m_pSkyVB = NULL;
    m_pStaticObjects = NULL;
    m_nNumStaticObjects = 0;
    m_pMeshObjects = NULL;
    m_nNumMeshObjects = 0;
    m_hSkyTexture = 0;
    m_hFogTexture = 0;

    DPrint( "OCWorld::OCWorld()\n" );

    // create vertex buffer for water
    m_pWaterVB = NEW CVBuffer;
    m_pWaterVB->Create( pDev, 256, FVF_OCWVertex, sizeof( OCWVertex ) );
    OCWVertex *v = (OCWVertex *)m_pWaterVB->Lock( 256 );
    int x, y, pos = 0;
    for ( y = 0; y <= 128; y += 16 )
    {
        for ( x = 0; x <= 128; x += 16 )
        {
            v[ pos ].x = x;
            v[ pos ].y = 0;
            v[ pos ].z = y;
            v[ pos ].u = (float)x * ( 1.f / 128.f );
            v[ pos ].v = (float)y * ( 1.f / 128.f );
            v[ pos ].color = 0xe0ffffff;

            pos++;
        }
    }
    m_pWaterVB->Unlock();

    // create index buffer for water
    m_pWaterIB = NEW CIBuffer;
    m_pWaterIB->Create( pDev, 675 );
    short *idx = (short *)m_pWaterIB->Lock( 675 );
    pos = 0;
    for ( y = 0; y < 8; y++ )
    {
        for ( x = 0; x < 8; x++ )
        {
            idx[ pos++ ] = ( x + 0 ) + ( y + 0 ) * 9;
            idx[ pos++ ] = ( x + 1 ) + ( y + 1 ) * 9;
            idx[ pos++ ] = ( x + 1 ) + ( y + 0 ) * 9;

            idx[ pos++ ] = ( x + 0 ) + ( y + 0 ) * 9;
            idx[ pos++ ] = ( x + 0 ) + ( y + 1 ) * 9;
            idx[ pos++ ] = ( x + 1 ) + ( y + 1 ) * 9;
        }
    }
    m_pWaterIB->Unlock();

    DPrint( "OCWorld::OCWorld : Create wave map for water surface\n" );

    //
    // Create wave map for water surface
    //
    int dwWidth = 256;
    int dwHeight = 256;
    D3DFORMAT d3dBumpFormat = D3DFMT_V8U8;
    m_psBumpMap = NULL;
    m_psWaterDecalMap = NULL;
    if( FAILED( x3d->dev->CreateTexture( dwWidth, dwHeight, 1, 0 /* Usage */,
                                             d3dBumpFormat, D3DPOOL_MANAGED,
                                             &m_psBumpMap, NULL ) ) )
    {
        DPrint( "ERR : Can't create bump map\n" );
    }

    if( FAILED( x3d->dev->CreateTexture( dwWidth, dwHeight, 1, 0 /* Usage */,
                                             D3DFMT_A8R8G8B8, D3DPOOL_MANAGED,
                                             &m_psWaterDecalMap, NULL ) ) )
    {
        DPrint( "ERR : Can't create bump decal map\n" );
    }

    // Lock the surface and write in some bumps for the waves
    D3DLOCKED_RECT d3dlr, d3dlr2;
    m_psBumpMap->LockRect( 0, &d3dlr, 0, 0 );
    m_psWaterDecalMap->LockRect( 0, &d3dlr2, 0, 0 );
    CHAR *pDst = (CHAR *)d3dlr.pBits;
    CHAR *pDst2 = (CHAR *)d3dlr2.pBits;
    CHAR  iDu, iDv;

    for ( y = 0; y < dwHeight; y++ )
    {
        CHAR *pPixel = pDst;
        CHAR *pPixel2 = pDst2;

        for ( x = 0; x < dwWidth; x++ )
        {
            FLOAT fx = x / (FLOAT)dwWidth - 0.5f;
            FLOAT fy = y / (FLOAT)dwHeight - 0.5f;

            iDu  = (char)( 16 * cosf( 64.f * fy + 3 * sinf( 6.28f * fx ) ) );
            iDu += (char)( 64 * cosf( 64.f * fy ) );
            iDv  = (char)( 16 * sinf( 64.f * fy + 3 * cosf( 6.28f * fx ) ) );
            iDv += (char)( 64 * sinf( 64.f * fy ) );

            *pPixel++ = iDu;
            *pPixel++ = iDv;

            CHAR amp = 230 + (char)( 16 * cosf( 64.f * fy ) ) +
        (char)( 8 * cosf( 64.f * fy + 3 * sinf( 6.28f * fx ) ) );

            *pPixel2++ = amp;
            *pPixel2++ = amp;
            *pPixel2++ = amp;
            *pPixel2++ = (CHAR)255;
        }

        pDst += d3dlr.Pitch;
        pDst2 += d3dlr2.Pitch;
    }
    m_psWaterDecalMap->UnlockRect( 0 );
    m_psBumpMap->UnlockRect( 0 );

    CreateDXShaders();
}

//
// Common destructor
//
OCWorld::~OCWorld
    (
    )
{
    DEL( m_pWaterVB );
    DEL( m_pWaterIB );
    
    ShutDown();
}

//
// Create DX shaders
//
D3DVERTEXELEMENT9 dwSpriteVtxDecl[] =
{
    { 0, 0,  D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
    { 0, 16, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
    { 0, 20, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
    D3DDECL_END()
};

D3DVERTEXELEMENT9 OCWorld::m_dwAMHVtxDecl[] =
{
    { 0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
    { 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
    { 0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
    { 0, 32, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1 },
    D3DDECL_END()
};

void OCWorld::CreateDXShaders
    (
    )
{
    pDev->CreateVertexDeclaration( dwSpriteVtxDecl, &m_pSpriteVtxDecl );
    pDev->CreateVertexDeclaration( m_dwAMHVtxDecl, &m_pAMHVtxDecl );

    m_hOCBlur_vso = x3d->RegisterDXShader( "luxsys/ocblur.vso" );
    m_hOCBlur_pso = x3d->RegisterDXShader( "luxsys/ocblur.pso" );
    m_hOCGlowOut_pso = x3d->RegisterDXShader( "luxsys/ocglowout.pso" );
    m_hAMHBump_vso = x3d->RegisterDXShader( "luxsys/amhbump.vso" );
    m_hAMHBump_pso = x3d->RegisterDXShader( "luxsys/amhbump.pso" );
}

//
// Load a map
//
bool OCWorld::LoadMap
    (
    const char *pMapName
    )
{
    HXFSFile fp;
    int fileSize;

    DPrint( "OCWorld::LoadMap : Load WRD : \"" );
    DPrint( pMapName );
    DPrint( "\"\n" );

    DrawLoadProgress( -1 );

    {
        strcpy( m_pLNDFileName, pMapName );
        strcat( m_pLNDFileName, ".wrd" );

        fp = xfs->Open( m_pLNDFileName );
        fileSize = xfs->SizeOf( fp );
        
        m_pWRDFile = ALLOCATE( fileSize );
        xfs->Read( fp, m_pWRDFile, fileSize );
        xfs->Close( fp );

        m_pWRD = NEW COCWorldMarks;
        m_pWRD->LoadFromMemory( m_pWRDFile );
    }

    DPrint( "OCWorld::LoadMap : Load LND\n" );
    strcpy( m_pLNDFileName, pMapName );
    strcat( m_pLNDFileName, ".lnd" );
    char chBaseLNDPath[ 256 ];
    strcpy( chBaseLNDPath, m_pLNDFileName ); 
    RemoveFileName( chBaseLNDPath );

    fp = xfs->Open( m_pLNDFileName );
    fileSize = xfs->SizeOf( fp );
    
    m_pLNDFile = ALLOCATE( fileSize );
    xfs->Read( fp, m_pLNDFile, fileSize );
    xfs->Close( fp );

    m_pLND = NEW COCLandscapeData;
    m_pLND->m_pBasePath = chBaseLNDPath;
    m_pLND->LoadFromMemory( m_pLNDFile );

    // well, let's create 3D tiles
    m_nMapWidth = m_pLND->m_pLndSize->dwWidth;
    m_nMapHeight = m_pLND->m_pLndSize->dwHeight;
    m_nNumTileContainers = m_pLND->m_nNumTileContainers;

    m_pTilesMap = NEW OCTile[ m_nMapWidth * m_nMapHeight ];
    m_pTileContainers = NEW OCTileContainer[ m_nNumTileContainers ];

    DPrint( "OCWorld::LoadMap : Convert and initialize tiles\n" );
    // create tile containers
    m_nNumTileContainers = 0;
    for ( int pass = 0; pass < 2; pass++ )
        for ( int pp = 0, y = 0; y < m_nMapHeight; y++ )
            for ( int x = 0; x < m_nMapWidth; x++, pp++ )
            {
                DrawLoadProgress( 0.75f * ( (float)( pass * m_nMapWidth * m_nMapHeight + y * m_nMapWidth + x ) / (float)( 2 * m_nMapHeight * m_nMapWidth ) ) );

                if ( !pass )
                {
                    if ( m_pLND->m_ppMapTiles[ pp ]->m_bUniq )
                    {
                        m_pTileContainers[ m_nNumTileContainers ].CreateFromCMapTile( m_pLND->m_ppMapTiles[ pp ] );
                        m_nNumTileContainers++;
                    }
                }
                else
                {
                    m_pTilesMap[ pp ].m_pTileContainer = &m_pTileContainers[ m_pLND->m_ppMapTiles[ pp ]->m_nTileRefNum ];
                    m_pTilesMap[ pp ].m_pMapTile = m_pLND->m_ppMapTiles[ pp ];
                    m_pTilesMap[ pp ].Init( x, m_nMapHeight - y - 1 );
                }
            }

    m_pRefObjects = (OCRefObject *)NEW OCRefObject[ m_nMapWidth * m_nMapHeight + 1000 ];
    m_pRefRenderSet = (OCRefObject **)ALLOCATE( ( m_nMapWidth * m_nMapHeight + 1000 ) * sizeof( OCRefObject * ) );

    DPrint( "OCWorld::LoadMap : Load and register decorations\n" );

    m_pMeshObjects = (OCMeshCache **)ALLOCATE( 512 * sizeof( OCMeshCache * ) );
    m_nNumMeshObjects = 0;

    LoadAndRegisterDecorations();

    // load sky texture
    char pSkyFileName[ 256 ];
    char chtmp[ 256 ];
    m_pLND->m_pSkyRecVal->CopyTo( chtmp );
    strcpy( pSkyFileName, chBaseLNDPath );
    strcat( pSkyFileName, chtmp );
    strcat( pSkyFileName, ".btm" );
    m_hSkyTexture = x3d->RegisterTexture( pSkyFileName, ERTF_NOMIP );
    m_hFogTexture = x3d->RegisterTexture( "luxsys/fog.tga", ERTF_NOMIP );

    DPrint( "OCWorld::LoadMap : Build sky and water effects\n" );
    BuildSkyAndWaterEffects();

    DPrint( "OCWorld::LoadMap : Ok, OC world has been loaded\n" );

    return true;
}

//
// Load and register decoration meshes from OC world
//
void OCWorld::LoadAndRegisterDecorations
    (
    )
{
    unsigned int rec;
    TOCDecorRec *pRec;
    COCStr *m_pRecName;
    COCStr *m_pRecVal;
    XVec3 xvPos;
    TOCRotation *pRot;
    COCStr *m_pTypName;
    COCStr *m_pTypVal;
    static char chMeshName[ 256 ];
    OCMeshCache *pMesh;
    OCMeshObject *pObj;

    m_pStaticObjects = (OCMeshObject **)ALLOCATE( ( m_pWRD->m_dwNumDecorations + m_pWRD->m_dwNumMarks ) * sizeof( OCMeshObject * ) );
    m_nNumStaticObjects = 0;

    pRec = m_pWRD->m_pStartRec;
    for ( rec = 0; rec < m_pWRD->m_dwNumDecorations; rec++ )
    {
        DrawLoadProgress( 0.75f + 0.25f * ( (float)rec / (float)( 1 + m_pWRD->m_dwNumDecorations ) ) );

        m_pRecName = (COCStr *)m_pWRD->GetWRDPtr( pRec->dwOffsetRecName );
        m_pRecVal = REC_VAL_PTR( m_pRecName );

        xvPos = *(XVec3 *)m_pWRD->GetWRDPtr( pRec->dwOffsetPosition );
        pRot = (TOCRotation *)m_pWRD->GetWRDPtr( pRec->dwOffsetRotataion );

        m_pTypName = (COCStr *)m_pWRD->GetWRDPtr( pRec->dwOffsetStr );
        m_pTypVal = REC_VAL_PTR( m_pTypName );

        m_pRecVal->CopyTo( chMeshName );
        DPrint( "OCWorld : create reference to \"" );
        DPrint( chMeshName );
        DPrint( "\"\n" );
        pMesh = AddMeshObject( chMeshName );

        pObj = AddStaticObject( pMesh );
        pObj->m_xvPos.xyz( xvPos.x + 8.f * m_pLND->m_dwXOrigin, xvPos.y, xvPos.z + 8.f * m_pLND->m_dwYOrigin );
        pObj->m_xeRot.yaw = -360.f * (float)pRot->sAngle2 / 65536.f;
        pObj->Init();

        pRec = (TOCDecorRec *)m_pWRD->GetWRDPtr( pRec->dwOffsetNextRec );
    }

#ifdef OC_SUPPORT_ENTITIES

    DPrint( "OCWorld::LoadAndRegisterDecorations : Create entities\n" );

    // create all entities as static meshes ;)
    for ( rec = 0; rec < m_pWRD->m_nNumEntities; rec++ )
    {
        COCMarkEntity *e = &m_pWRD->m_pEntities[ rec ];
        COCMark *m = m_pWRD->GetEMark( m_pWRD->m_pEntities[ rec ].m_nOCMarkID );

        m_pRecName = (COCStr *)m->GetMRKPtr( m->m_pHeader->pChunks[ MRK_REC_TYPE ].dwOffset );
        m_pRecVal = REC_VAL_PTR( m_pRecName );

        if ( m_pRecName->Cmp( "PMesh" ) )
        {
            m_pRecVal->CopyTo( chMeshName );
            pMesh = AddMeshObject( chMeshName );

            pObj = AddStaticObject( pMesh );
            pObj->m_xvPos.xyz( e->m_Pos[ 0 ] + 8.f * m_pLND->m_dwXOrigin, e->m_Pos[ 1 ], e->m_Pos[ 2 ] + 8.f * m_pLND->m_dwYOrigin );
            pObj->m_xeRot.yaw = -360.f * (float)e->m_Rot.sAngle2 / 65536.f;
            pObj->m_xeRot.roll = -360.f * (float)e->m_Rot.sAngle3 / 65536.f;
            pObj->Init();
        }
    }
#else

    DPrint( "OCWorld::LoadAndRegisterDecorations : Entities disabled\n" );

#endif // OC_SUPPORT_ENTITIES
}

//
// Add cached mesh object
//
OCMeshCache *OCWorld::AddMeshObject
    (
    const char *pFileName
    )
{
    OCMeshCache *mesh;
    static char fileName[ 256 ];
    int i, size;
    HXFSFile fp;

    strcpy( fileName, "Mesh/" );
    strcat( fileName, pFileName );
    bool bHasExt = false;
    for ( i = 0; i < (int)strlen( pFileName ); i++ )
        if ( pFileName[ i ] == '.' )
        {
            bHasExt = true;
            break;
        }
    
    if ( !bHasExt )
        strcat( fileName, ".msh" );
    xfs->FixFileName( fileName );

    // try find the same
    for ( i = 0; i < m_nNumMeshObjects; i++ )
    {
        if ( !strcmp( fileName, m_pMeshObjects[ i ]->m_chName ) )
            return m_pMeshObjects[ i ];
    }

    mesh = NEW OCMeshCache;
    strcpy( mesh->m_chName, fileName );

    fp = xfs->Open( mesh->m_chName );
    if ( fp )
    {
        size = xfs->SizeOf( fp );
        mesh->m_pMESHData = ALLOCATE( size );
        xfs->Read( fp, mesh->m_pMESHData, size );
        xfs->Close( fp );
        mesh->m_OCMesh.LoadFromMemory( mesh->m_pMESHData );

        mesh->Init();
    }
    else
        DPrint( "MESH file not found" );

    m_pMeshObjects[ m_nNumMeshObjects++ ] = mesh;

    return mesh;
}

//
// OC mesh object constructor
//
OCMeshObject::OCMeshObject
    (
    )
{
    m_pMesh = NULL;
    m_pAnim = NULL;
}

//
// Add static mesh object
//
OCMeshObject *OCWorld::AddStaticObject
    (
    OCMeshCache *pMesh
    )
{
    OCMeshObject *po;

    po = NEW OCMeshObject;
    po->m_pMesh = pMesh;

    m_pStaticObjects[ m_nNumStaticObjects++ ] = po;

    return po;
}

//
// Setup current viewer
//
void OCWorld::SetViewer
    (
    OCViewer *pView
    )
{
    m_pCurrView = pView;
}

//
// Add reference to object for render
//
void OCWorld::AddRefObject
    ( 
    OCObject *pObj 
    )
{
    float fLOD;
    XVec3 bbpos;

    m_pRefObjects[ m_nNumRefObjects ].m_pObj = pObj;

    bbpos = 0.5f * ( pObj->m_xbBox.min + pObj->m_xbBox.max );
    fLOD = ( pObj->m_xvPos - m_pCurrView->m_xvPos ) * ( bbpos - m_pCurrView->m_xvPos );
    m_pRefObjects[ m_nNumRefObjects ].m_fDist2 = ( bbpos - m_pCurrView->m_xvPos ) * ( bbpos - m_pCurrView->m_xvPos );
    fLOD = sqrtf( fLOD ) / 800.f;
    if ( fLOD > 1.f ) fLOD = 1.f;
    if ( pObj ) pObj->SetLODLevel( fLOD );

    m_pRefRenderSet[ m_nNumRefObjects ] = &m_pRefObjects[ m_nNumRefObjects ];

    m_nNumRefObjects++;
}

//
// Sort ref object by distance to view
//
int OCWorld_RefSort_Cmp
    (
    const void *a,
    const void *b
    )
{
    return ( ((OCRefObject *)(*(OCRefObject **)a))->m_fDist2 - ((OCRefObject *)(*(OCRefObject **)b))->m_fDist2 ) > 0 ? 1 : -1;
}

void OCWorld::SortRefObjects
    (
    )
{
    qsort( m_pRefRenderSet, m_nNumRefObjects, sizeof( OCRefObject * ), OCWorld_RefSort_Cmp );
}

//
// Render a world
//
bool OCWorld::Render
    (
    )
{
    if ( !m_pLNDFile )
        return false;

    // setup current viewer
    SetViewer( m_pPlayer->m_pViewer );
    m_pCurrView->SetRender();
    m_pCurrView->CalculateFrustum();

    int x, y, idx;
    int nTotalFaces, nStaticFaces = 0;
    int ref;

    nTotalFaces = 0;

    if ( !ocinput->TestKey( DIK_V ) )
    {
        m_nNumRefObjects = 0;

        for ( y = 0; y < m_nMapHeight; y++ )
        {
            idx = y * m_nMapWidth;
            for ( x = 0; x < m_nMapWidth; x++ )
            {
                if ( m_pCurrView->TestWithBBox( m_pTilesMap[ idx ].m_xbBox ) )
                {
                    AddRefObject( (OCObject *)&m_pTilesMap[ idx ] );
                }

                idx++;
            }
        }

        for ( int i = 0; i < m_nNumStaticObjects; i++ )
        {
            if ( m_pCurrView->TestWithBBox( m_pStaticObjects[ i ]->m_xbBox ) )
            {
                AddRefObject( (OCObject *)m_pStaticObjects[ i ] );
                nStaticFaces += ((OCObject *)m_pStaticObjects[ i ])->GetNumFaces();
            }
        }

        // render referenced objects
        SortRefObjects();
    }

    pDev->SetTransform( D3DTS_VIEW, (D3DMATRIX *)&m_pCurrView->m_mView );
    pDev->SetTransform( D3DTS_PROJECTION, (D3DMATRIX *)&m_pCurrView->m_mProj );

    rend_SetRenderState( D3DRS_ZENABLE, true );
    rend_SetRenderState( D3DRS_ALPHABLENDENABLE, false );
    rend_SetFog( 0, 860.f, m_dwFogColor );

    bool bVisWater = false;
    for ( ref = 0; ref < m_nNumRefObjects; ref++ )
    {
        bVisWater = bVisWater || ( 0 != ( m_pRefRenderSet[ ref ]->m_pObj->GetFlags() & EOBJF_RP_WATER ) );
        m_pRefRenderSet[ ref ]->m_pObj->Render();
        nTotalFaces += m_pRefRenderSet[ ref ]->m_pObj->GetNumFaces();
    }

    m_pPlayer->Render( EOBJF_RP_COMMON );

    // do water pass
    if ( bVisWater )
        RenderWater();

    RenderSky();

    // print debug info
    pDev->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
    pDev->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
    pDev->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_NONE );
    rend_SetRenderState( D3DRS_ALPHABLENDENABLE, true );
    rend_SetRenderState( D3DRS_LIGHTING, false );

    static float glowRadius = 0.006f;
    static float s = 0.34f;
    static float m = 0.90f;

    // post processing - GLOW
    if ( g_bGlow )
    {
        rend_SetRenderState( D3DRS_ZENABLE, false );
        rend_SetRenderState( D3DRS_ALPHABLENDENABLE, false );
        rend_SetBlend( D3DBLEND_ONE, D3DBLEND_ZERO );
        pDev->StretchRect( m_pRenderSurface, NULL, pLFSurface, NULL, D3DTEXF_LINEAR );

        IDirect3DSurface9 *surf[ 2 ] = { pLFSurface, pLFSurface2 };
        IDirect3DTexture9 *tex[ 2 ] = { pLFTexture, pLFTexture2 };
        float ct = 0.5f / 256.f;
        TSpriteVertex vtx[ 4 ] = {
            { -1, 1, 0, 1, 0xffffffff, ct, ct },
            { 1, 1, 0, 1, 0xffffffff, 1 + ct, ct },
            { -1,  -1, 0, 1, 0xffffffff, ct, 1 + ct },
            { 1, -1, 0, 1, 0xffffffff, 1 + ct, 1 + ct },
        };

        x3d->SetDXShader( m_hOCBlur_vso );
        x3d->SetDXShader( m_hOCBlur_pso );
        pDev->SetVertexDeclaration( m_pSpriteVtxDecl );

        float float4[] = { glowRadius, glowRadius * 640.f / 480.f, 0, 0 };
        float float40[] = { 0, 0, 0, 0 };
        pDev->SetVertexShaderConstantF( 0, float4, 1 );

        int pass, i;

        for ( i = 0; i < 4; i++ )
        {
            pDev->SetTextureStageState( i, D3DTSS_TEXCOORDINDEX, i );
            pDev->SetTextureStageState( i, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
            pDev->SetSamplerState( i, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
            pDev->SetSamplerState( i, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );
        }

        pDev->SetDepthStencilSurface( pLFZSurface );

        for ( pass = 0; pass < 5; pass++ )
        {
            for ( i = 0; i < 4; i++ )
                rend_SetTexture( i, tex[ pass & 1 ] );

            pDev->SetRenderTarget( 0, surf[ ~pass & 1 ] );
            pDev->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, vtx, sizeof( TSpriteVertex ) );
        }

        rend_SetRenderState( D3DRS_ALPHABLENDENABLE, true );
        rend_SetBlend( D3DBLEND_ONE, D3DBLEND_ONE );
        if ( ocinput->TestKey( DIK_H ) )
            rend_SetBlend( D3DBLEND_ONE, D3DBLEND_ZERO );

        pDev->SetRenderTarget( 0, m_pRenderSurface );
        pDev->SetDepthStencilSurface( m_pZRenderSurface );
        
        pDev->SetVertexShaderConstantF( 0, float40, 1 );

        if ( ocinput->TestKey( DIK_1 ) )
            s -= 0.001f;
        if ( ocinput->TestKey( DIK_2 ) )
            s += 0.001f;
        if ( ocinput->TestKey( DIK_3 ) )
            m -= 0.001f;
        if ( ocinput->TestKey( DIK_4 ) )
            m += 0.001f;
        if ( ocinput->TestKey( DIK_5 ) )
            glowRadius -= 0.0001f;
        if ( ocinput->TestKey( DIK_6 ) )
            glowRadius += 0.0001f;

        float contrast[] = { s, s, s, s, m, m, m, m };
        pDev->SetPixelShaderConstantF( 0, contrast, 2 );
        
        x3d->SetDXShader( m_hOCGlowOut_pso );
        rend_SetTexture( 0, tex[ pass & 1 ] );
        pDev->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, vtx, sizeof( TSpriteVertex ) );

        pDev->SetPixelShader( NULL );
        rend_SetRenderState( D3DRS_ZENABLE, true );

        for ( i = 0; i < 4; i++ )
        {
            pDev->SetSamplerState( i, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP );
            pDev->SetSamplerState( i, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP );
        }
    }

    char str[ 256 ];

    static float fFPS = 0.f;
    static float lastTime = sys_timeCounter;
    static int frame = 0;

    frame++;
    if ( sys_timeCounter - lastTime > 1.f )
    {
        fFPS = (float)frame / ( sys_timeCounter - lastTime );
        lastTime = sys_timeCounter;
        frame = 0;
    }

    rend_SetRenderState( D3DRS_FOGENABLE, false );
    rend_SetBlend( D3DBLEND_SRCALPHA, D3DBLEND_ONE );
    rend_SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE );
    float ly = 15.f;
    if ( g_bShowFPS )
    {
        x3d->PushDraw2DText( x3d->m_hFontCommon, 15, ly + 5, 1.2f, 0xC000FF00, 0, "%.2f fps", fFPS );
        x3d->PushDraw2DText( x3d->m_hFontCommon, 15, ly + 20, 1.2f, 0xC000FF00, 0, "%d tris", nTotalFaces );
        x3d->PushDraw2DText( x3d->m_hFontCommon, 15, ly + 35, 1.2f, 0xC000FF00, 0, "%d refobj", m_nNumRefObjects );
        x3d->PushDraw2DText( x3d->m_hFontCommon, 15, ly + 50, 1.2f, 0xC000FF00, 0, "%d stris", nStaticFaces );

        ly = 430.f;
        x3d->PushDraw2DText( x3d->m_hFontCommon, 15, ly + 00, 1.2f, 0xC000FF00, 0, "glow.sub=%.3f", s );
        x3d->PushDraw2DText( x3d->m_hFontCommon, 15, ly + 15, 1.2f, 0xC000FF00, 0, "glow.mul=%.3f", m );
        x3d->PushDraw2DText( x3d->m_hFontCommon, 15, ly + 30, 1.2f, 0xC000FF00, 0, "glow.size=%.4f", glowRadius );
    }

    x3d->FlushSpriteBuffer();

    rend_SetRenderState( D3DRS_ALPHABLENDENABLE, false );
    pDev->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );

    return true;
}

//
// Render water surface
//
void OCWorld::RenderWater
    (
    )
{
    int ref;

    // setup specific render states
    rend_SetRenderState( D3DRS_ZWRITEENABLE, false );
    rend_SetRenderState( D3DRS_ALPHABLENDENABLE, true );
    rend_SetBlend( D3DBLEND_SRCALPHA, D3DBLEND_INVSRCALPHA );
    pDev->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2 );
    pDev->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP );
    pDev->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP );

    pDev->SetVertexShader( NULL );
    pDev->SetFVF( FVF_OCWVertex );
    pDev->SetPixelShader( NULL );
    pDev->SetStreamSource( 0, m_pWaterVB->GetInterface(), 0, sizeof( OCWVertex ) );
    pDev->SetIndices( m_pWaterIB->GetInterface() );

    //
    // Do bump frame
    //
    XMat4 m_matBumpMat;
    float r = 0.01f;
    float m_fTime = sys_timeCounter;
    m_matBumpMat._11 =  r * cosf( m_fTime * 4.0f );
    m_matBumpMat._12 = -r * sinf( m_fTime * 4.0f );
    m_matBumpMat._21 =  r * sinf( m_fTime * 4.0f );
    m_matBumpMat._22 =  r * cosf( m_fTime * 4.0f );

    //
    // Setup projection parameters
    //
    XMat4 mproj;
    mproj.setIdentity();
    XVec3 dir = m_pCurrView->m_xvTgt - m_pCurrView->m_xvPos;
    float rot = 0.6f + -atan2( dir.z, dir.x ) / 6.2831853f;
    rot = rot - (int)rot;
    dir.Normalize();
    mproj._11 = 420.f / 1920.f;
    mproj._22 = -2.0f;
    mproj._32 = -2.f * dir.y;
    mproj._31 = rot;

    XMat4 mw;
    mw.setIdentity();
    mw._32 = -m_fTime * 0.05f;

    if ( g_bComplexWater )
    {
        pDev->SetTransform( D3DTS_TEXTURE1, (D3DMATRIX *)&mproj );
        pDev->SetTransform( D3DTS_TEXTURE2, (D3DMATRIX *)&mw );

        //
        // Setup textures
        //
        x3d->dev->SetTexture( 0, m_psBumpMap );
        x3d->dev->SetTextureStageState( 0, D3DTSS_BUMPENVMAT00,   F2DW( m_matBumpMat._11 ) );
        x3d->dev->SetTextureStageState( 0, D3DTSS_BUMPENVMAT01,   F2DW( m_matBumpMat._12 ) );
        x3d->dev->SetTextureStageState( 0, D3DTSS_BUMPENVMAT10,   F2DW( m_matBumpMat._21 ) );
        x3d->dev->SetTextureStageState( 0, D3DTSS_BUMPENVMAT11,   F2DW( m_matBumpMat._22 ) );
        x3d->dev->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_BUMPENVMAP );
        x3d->dev->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
        x3d->dev->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_CURRENT );

        x3d->SetTexture( 1, ocworld->m_hSkyTexture );
        x3d->dev->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEPOSITION );
        x3d->dev->SetTextureStageState( 1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_PROJECTED | D3DTTFF_COUNT3 );
        x3d->dev->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
        x3d->dev->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
        pDev->SetSamplerState( 1, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );

        x3d->dev->SetTexture( 2, m_psWaterDecalMap );
        x3d->dev->SetTextureStageState( 2, D3DTSS_TEXCOORDINDEX, 0 );
        x3d->dev->SetTextureStageState( 2, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2 );
        x3d->dev->SetTextureStageState( 2, D3DTSS_COLOROP,   D3DTOP_MODULATE );
    }
    else
    {
        pDev->SetTransform( D3DTS_TEXTURE0, (D3DMATRIX *)&mproj );
        pDev->SetTransform( D3DTS_TEXTURE1, (D3DMATRIX *)&mw );

        x3d->SetTexture( 0, ocworld->m_hSkyTexture );
        x3d->dev->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEPOSITION );
        x3d->dev->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_PROJECTED | D3DTTFF_COUNT3 );
        pDev->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );

        x3d->dev->SetTexture( 1, m_psWaterDecalMap );
        x3d->dev->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 0 );
        x3d->dev->SetTextureStageState( 1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2 );
        x3d->dev->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_MODULATE );

        x3d->dev->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    }

    // render all watered objects
    for ( ref = 0; ref < m_nNumRefObjects; ref++ )
    {
        if ( m_pRefRenderSet[ ref ]->m_pObj->GetFlags() & EOBJF_RP_WATER )
            m_pRefRenderSet[ ref ]->m_pObj->Render( EOBJF_RP_WATER );
    }

    //
    // Reset texture states
    //
    pDev->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );
    pDev->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
    pDev->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 0 );
    pDev->SetTextureStageState( 1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
    pDev->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP );
    pDev->SetSamplerState( 1, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP );
    pDev->SetTextureStageState( 2, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );

    x3d->dev->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
    x3d->dev->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE );
    x3d->dev->SetTextureStageState( 2, D3DTSS_COLOROP, D3DTOP_DISABLE );

    // reset render states
    rend_SetRenderState( D3DRS_ALPHABLENDENABLE, false );
    rend_SetRenderState( D3DRS_ZWRITEENABLE, true );
}

//
// Build sky and water effects
//
void OCWorld::BuildSkyAndWaterEffects
    (
    )
{
    int nNumVerts = 6;
    m_pSkyVB = NEW CVBuffer;
    m_pSkyVB->Create( x3d->dev, nNumVerts, FVF_OCVertex, sizeof( OCVertex ) );
    OCVertex *v = (OCVertex *)m_pSkyVB->Lock( nNumVerts );
    
    float sx = 1920.f;
    float sy = 420.f;
    float z = 860.f;

    v[ 0 ].xyz( sx, 0, z );
    v[ 0 ].u = 1.f;
    v[ 0 ].v = 0.f;
    v[ 1 ].xyz( -sx, 0, z );
    v[ 1 ].u = 0.f;
    v[ 1 ].v = 0.f;
    v[ 2 ].xyz( sx, sy, z );
    v[ 2 ].u = 1.f;
    v[ 2 ].v = 1.f;
    v[ 3 ].xyz( -sx, sy, z );
    v[ 3 ].u = 0.f;
    v[ 3 ].v = 1.f;
    v[ 4 ].xyz( sx, sy, -z * 0.5f );
    v[ 4 ].u = 1.f;
    v[ 4 ].v = 2.f;
    v[ 5 ].xyz( -sx, sy, -z * 0.5f );
    v[ 5 ].u = 0.f;
    v[ 5 ].v = 2.f;

    m_pSkyVB->Unlock();
}

//
// Render skyes on background
//
void OCWorld::RenderSky
    (
    )
{
    XMat4 mWorld, mView, m;
    XVec3 dir, camPos, camTgt;
    XVec3 xvUpVector( 0, 1.f, 0 );

    mWorld.setIdentity();
    mWorld._41 = m_pCurrView->m_xvPos.x;
    mWorld._42 = m_pCurrView->m_xvPos.y;
    mWorld._43 = m_pCurrView->m_xvPos.z;
    camPos = m_pCurrView->m_xvPos;
    camTgt = m_pCurrView->m_xvTgt;
    camPos.y = 0;
    camTgt.y = 0;
    D3DXMatrixLookAtLH( (D3DXMATRIX *)&mView, (D3DXVECTOR3 *)&camPos, (D3DXVECTOR3 *)&camTgt, (D3DXVECTOR3 *)&xvUpVector );
    D3DXMatrixInverse( (D3DXMATRIX *)&mView, NULL, (D3DXMATRIX *)&mView );
    mView._41 = mView._42 = mView._43 = 0;
    mWorld = mView * mWorld;

    dir = camTgt - camPos;
    float rot = 0.1f + -atan2( dir.z, dir.x ) / 6.2831853f;
    m.setIdentity();
    m._31 = rot - (int)rot;
    rend_SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2 );
    x3d->dev->SetTransform( D3DTS_TEXTURE0, (D3DMATRIX *)&m );

    //
    // Setup fog parameters
    //
    x3d->SetTexture( 0, m_hSkyTexture );

    x3d->SetTexture( 1, m_hFogTexture );
    x3d->dev->SetRenderState( D3DRS_TEXTUREFACTOR, m_dwFogColor );
    x3d->dev->SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_BLENDTEXTUREALPHA );
    x3d->dev->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TFACTOR );
    x3d->dev->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_CURRENT );
    x3d->dev->SetTextureStageState( 1, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1 );
    x3d->dev->SetTextureStageState( 1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
    rend_SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 0 );
    rend_SetTextureStageState( 1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2 );
    pDev->SetSamplerState( 1, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
    float fh = m_pCurrView->m_xvPos.y;
    if ( fh > m_fFogUp ) fh = m_fFogUp;
    if ( fh < m_fFogDown ) fh = m_fFogDown;
    fh = ( fh - m_fFogDown ) / ( m_fFogUp - m_fFogDown );
    m.setIdentity();
    m._11 = 0;
    m._21 = -0.5f;
    m._12 = 1;
    m._22 = 0;
    m._31 = 2 - 2 * fh;
    x3d->dev->SetTransform( D3DTS_TEXTURE1, (D3DMATRIX *)&m );

    x3d->dev->SetTransform( D3DTS_WORLD, (D3DMATRIX *)&mWorld );

    x3d->dev->SetVertexShader( NULL );
    x3d->dev->SetFVF( FVF_OCVertex );
    x3d->dev->SetStreamSource( 0, m_pSkyVB->GetInterface(), 0, sizeof( OCVertex ) );
    rend_SetRenderState( D3DRS_FOGENABLE, false );
    rend_SetRenderState( D3DRS_ZWRITEENABLE, false );
    pDev->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );
    
    x3d->dev->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 4 );

    //
    // Reset all parameters
    //
    rend_SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
    pDev->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP );
    rend_SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE );
    pDev->SetSamplerState( 1, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP );
    x3d->dev->SetTextureStageState( 1, D3DTSS_ALPHAOP, D3DTOP_MODULATE );
    x3d->dev->SetTextureStageState( 1, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );
    rend_SetRenderState( D3DRS_FOGENABLE, true );
    rend_SetRenderState( D3DRS_ZWRITEENABLE, true );
}

//
// Draw loading progress
//
void OCWorld::DrawLoadProgress
    (
    float fVal
    )
{
    static float fOld = 0;

    if ( fVal < fOld )
        fOld = 0;

    if ( fVal >= 0 )
    {
        if ( ( fVal - fOld ) < 0.05f )
            return;
    }
    else
    {
        fVal = 0;
        fOld = 0;
    }

    fOld = fVal;

    pDev->BeginScene();

    x3d->PushDraw2DSprite( ocgui->m_hBackgroundTexture, 0, 0, 640, 480, 0xffffffff, EPDF_NOCENTER, 0, NULL );
    x3d->FlushSpriteBuffer();

    ocgui->PushDrawHilight( 320, 450, 540, 30, 0xffc08040 );
    ocgui->PushDrawHilight( 320 - 245 + 245 * fVal, 450, 50 + 490 * fVal, 15, 0xffffc060 );
    x3d->FlushSpriteBuffer();

    pDev->EndScene();
    rend_UpdateScreen();
}

//
// Get height
//
float OCWorld::GetHeight
    (
    float x,
    float y
    )
{
    int tx = x / 128;
    int ty = y / 128;

    if ( tx < 0 )
        tx = 0;
    else
    if ( tx > m_nMapWidth )
        tx = m_nMapWidth - 1;
    else
    if ( ty < 0 )
        ty = 0;
    else
    if ( ty > m_nMapHeight )
        ty = m_nMapHeight - 1;
    
    int tile = tx + m_nMapWidth * ( m_nMapHeight - ty - 1 );
    int nx = x - 128 * tx;
    int ny = 127 - ( y - 128 * ty );

    return m_pTilesMap[ tile ].GetHeight( nx, ny );
}

//
// Shut down map
//
void OCWorld::ShutDown
    (
    )
{
    int i;

    DPrint( "OCWorld::ShutDown\n" );

    if ( !IsLoaded() )
        return;

    x3d->UnregisterTexture( m_hSkyTexture );
    x3d->UnregisterTexture( m_hFogTexture );

    DEL( m_pSkyVB );

    DEL( m_pWRD );
    FREE( m_pWRDFile );
    DEL( m_pLND );
    FREE( m_pLNDFile );

    VDEL( m_pTilesMap );
    VDEL( m_pTileContainers );

    VDEL( m_pRefObjects );
    FREE( m_pRefRenderSet );

    for ( i = 0; i < 20; i++ )
        g_pActors[ i ].Release();

    for ( i = 0; i < m_nNumStaticObjects; i++ )
        DEL( m_pStaticObjects[ i ] );

    FREE( m_pStaticObjects );

    for ( i = 0; i < m_nNumMeshObjects; i++ )
        DEL( m_pMeshObjects[ i ] );

    FREE( m_pMeshObjects );
}
