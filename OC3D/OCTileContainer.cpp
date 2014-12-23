// This file is distributed under a MIT license. See LICENSE.txt for details.
//
// Outcast tile container by AND 2002
//

#include "common.h"
#include <string.h>
#include "oc3d.h"

//
// Constructor
//
OCTileContainer::OCTileContainer
    (
    )
{
    m_pTexture = NULL;
    m_pIB = NULL;
    m_pVB = NULL;
    m_pWaterVB = NULL;
}

//
// Destructor
//
OCTileContainer::~OCTileContainer
    (
    )
{
    RELEASE( m_pTexture );
    DEL( m_pIB );
    DEL( m_pVB );
    DEL( m_pWaterVB );
}

//
// Create real 3D tile from voxel base tile
//
void OCTileContainer::CreateFromCMapTile
    ( 
    const CMapTile *pmt 
    )
{
    int i, j, idxoffs;
    int m_nLODSize = 1;

    CATess *ts = NEW CATess;
    float fAmp = (float)( 32768 >> pmt->m_pVoxelTile->m_dwHeightAmp ) / 32768.f;

    ts->Tesselate( &pmt->m_pVoxelTile->m_Texture, pmt->m_pVoxelTile->m_fEdgeDelta, fAmp, m_nLODSize );
    m_nNumVerts = ts->m_nNumVerts;

    idxoffs = 0;
    for ( i = 0; i < m_nLODSize; i++ )
    {
        m_nStartIBIdx[ i ] = idxoffs;
        m_nNumFaces[ i ] = ts->GetNumTris( i );
        idxoffs += 3 * m_nNumFaces[ i ];
    }

    // create vertex buffer
    m_pWaterVB = NULL;
    m_pVB = NEW CVBuffer;
    m_pVB->Create( pDev, m_nNumVerts, NULL, sizeof( OCVertex ) );
    OCVertex *v = (OCVertex *)m_pVB->Lock( m_nNumVerts );
    
    m_fLowLevel = ts->m_pVerts[ 0 ].y;
    m_fHighLevel = ts->m_pVerts[ 0 ].y;

    float fmul = 1.f / 130.f;
    float fofs = 1.f / 130.f;
    const float ddh = 128.f / 127.f;
    for ( i = 0; i < m_nNumVerts; i++ )
    {
        v[ i ].x = ddh * ts->m_pVerts[ i ].x;
        v[ i ].y = ts->m_pVerts[ i ].y;
        v[ i ].z = ddh * ts->m_pVerts[ i ].z;
        v[ i ].u = fofs + v[ i ].x * fmul;
        v[ i ].v = fofs + v[ i ].z * fmul;

        // calculate low and hight level of bounding box
        if ( v[ i ].y > m_fHighLevel )
            m_fHighLevel = v[ i ].y;
        if ( v[ i ].y < m_fLowLevel )
            m_fLowLevel = v[ i ].y;
    }
    m_pVB->Unlock();

    // create index buffer
    m_pIB = NEW CIBuffer;
    m_pIB->Create( pDev, idxoffs );
    short *idx = (short *)m_pIB->Lock( idxoffs );
    int pos = 0;
    TATessTri *f;

    for ( j = 0; j < m_nLODSize; j++ )
    {
        f = ts->GetTriSet( j );
        for ( i = 0; i < m_nNumFaces[ j ]; i++ )
        {
            idx[ pos++ ] = f->vidx[ 0 ];
            idx[ pos++ ] = f->vidx[ 2 ];
            idx[ pos++ ] = f->vidx[ 1 ];

            f = f->pNext;
        }
    }
    m_pIB->Unlock();

    // create texture
    char *pTex = (char *)ALLOCATE( sizeof( TTGAHeader ) + 128 * 128 * 4 );
    memset( pTex, 0, sizeof( TTGAHeader ) + 128 * 128 * 4 );

    ((TTGAHeader *)pTex)->dwSignature = 2;
    ((TTGAHeader *)pTex)->wBPP = 32;
    ((TTGAHeader *)pTex)->wWidth = 128;
    ((TTGAHeader *)pTex)->wHeight = 128;

    for ( int y = 0; y < 128; y++ )
        for ( int x = 0; x < 128; x++ )
        {
            *(int *)(pTex + sizeof( TTGAHeader ) + 4 * ( x + y * 128 )) = pmt->m_pVoxelTile->m_Texture.GetRGBAPixel( x, y, 0 );
        }

    D3DXCreateTextureFromFileInMemoryEx(
        pDev,
        pTex,
        sizeof( TTGAHeader ) + 128 * 128 * 4,
        D3DX_DEFAULT, D3DX_DEFAULT,
        D3DX_DEFAULT,
        0,
        D3DFMT_X8R8G8B8,
        D3DPOOL_MANAGED,
        D3DX_FILTER_POINT | D3DX_FILTER_MIRROR,
        D3DX_FILTER_BOX,
        0,
        NULL,
        NULL,
        &m_pTexture
        );

    FREE( pTex );

    DEL( ts );
}
