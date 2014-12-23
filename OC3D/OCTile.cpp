// This file is distributed under a MIT license. See LICENSE.txt for details.
//
// Outcast tile container by AND 2002
//

#include "common.h"
#include <string.h>
#include "oc3d.h"

//
// Initialize tile object
//
void OCTile::Init
    (
    int nX,
    int nY
    )
{
    m_nLODLevel = 0;

    nX *= 128;
    nY *= 128;

    m_xvPos.xyz( nX, m_pMapTile->m_dwTileHeight, nY );

    // calculate visibility bbox
    m_xbBox.min.xyz( nX, m_pMapTile->m_dwTileHeight + m_pTileContainer->m_fLowLevel, nY );
    m_xbBox.max.xyz( nX + 128, m_pMapTile->m_dwTileHeight + m_pTileContainer->m_fHighLevel, nY + 128 );

    m_nFlags |= EOBJF_RP_PROJSHADOW;

    if ( m_pMapTile->m_bTileHasWater )
    {
        m_nFlags |= EOBJF_RP_WATER;
        m_xbBox.AddPoint( XVec3( nX, m_pMapTile->m_pfWaterHeights[ m_pMapTile->m_bTileHasWater ], nY ) );

        if ( !m_pTileContainer->m_pWaterVB )
        {
            // create vertex buffer for water
            float fWaterLevel = m_pMapTile->m_pfWaterHeights[ m_pMapTile->m_bTileHasWater ];

            m_pTileContainer->m_pWaterVB = NEW CVBuffer;
            m_pTileContainer->m_pWaterVB->Create( pDev, 256, FVF_OCWVertex, sizeof( OCWVertex ) );
            OCWVertex *v = (OCWVertex *)m_pTileContainer->m_pWaterVB->Lock( 256 );
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

                    float h = GetHeight( x, 127 - y );
                    h = fWaterLevel - h;
                    h *= 40.f;
                    if ( h < 0 )
                        h = 0;
                    if ( h > 255 )
                        h = 255;

                    h = 70.f + h * ( 224.f - 70.f ) * ( 1.f / 255.f );

                    v[ pos ].color = (((int)h) << 24) | 0x00ffffff;

                    pos++;
                }
            }
            m_pTileContainer->m_pWaterVB->Unlock();
        }
    }
}

//
// Calculate LOD of object for current render frame
//
void OCTile::SetLODLevel
    ( 
    float from0to1 
    )
{
    m_nLODLevel = 0;
}

//
// Get number of faces for current LOD
//
int OCTile::GetNumFaces
    (
    )
{
    return m_pTileContainer->m_nNumFaces[ m_nLODLevel ];
}

//
// Render current tile
//
void OCTile::Render
    (
    int nPassType
    )
{
    if ( nPassType & EOBJF_RP_WATER )
    {
        PreRenderWater();
        return;
    }

    XMat4 mWorld;

    mWorld.setIdentity();
    mWorld._41 = m_xvPos.x;
    mWorld._42 = m_xvPos.y;
    mWorld._43 = m_xvPos.z;
    pDev->SetTransform( D3DTS_WORLD, (D3DMATRIX *)&mWorld );

    pDev->SetVertexShader( NULL );
    pDev->SetFVF( FVF_OCVertex );
    pDev->SetStreamSource( 0, m_pTileContainer->m_pVB->GetInterface(), 0, sizeof( OCVertex ) );
    pDev->SetIndices( m_pTileContainer->m_pIB->GetInterface() );

    rend_SetRenderState( D3DRS_LIGHTING, false );

    if ( nPassType & EOBJF_RP_PROJSHADOW )
    {
        pDev->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
        pDev->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );
        
        rend_SetBlend( D3DBLEND_SRCALPHA, D3DBLEND_INVSRCALPHA );
        pDev->SetRenderState( D3DRS_ALPHATESTENABLE, true );
        pDev->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );
        pDev->SetRenderState( D3DRS_ALPHAREF, 10 );

        pDev->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE );
        pDev->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR );
        pDev->SetRenderState( D3DRS_TEXTUREFACTOR, 0x60ffffff );
        
        pDev->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, m_pTileContainer->m_nNumVerts, m_pTileContainer->m_nStartIBIdx[ m_nLODLevel ], m_pTileContainer->m_nNumFaces[ m_nLODLevel ] );
        pDev->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP );
        pDev->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP );

        pDev->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE );
        pDev->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_CURRENT );
        pDev->SetRenderState( D3DRS_ALPHATESTENABLE, false );
    }
    else
    {
        rend_SetTexture( 0, m_pTileContainer->m_pTexture );
        rend_SetBlend( D3DBLEND_ONE, D3DBLEND_ZERO );
        
        pDev->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
        pDev->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );

        pDev->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, m_pTileContainer->m_nNumVerts, m_pTileContainer->m_nStartIBIdx[ m_nLODLevel ], m_pTileContainer->m_nNumFaces[ m_nLODLevel ] );

        pDev->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP );
        pDev->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP );
    }
}

//
// Pre Render water
//
void OCTile::PreRenderWater
    (
    )
{
    XMat4 mWorld;
    mWorld.setIdentity();
    mWorld._41 = m_xvPos.x;
    mWorld._42 = m_pMapTile->m_pfWaterHeights[ m_pMapTile->m_bTileHasWater ];
    mWorld._43 = m_xvPos.z;
    pDev->SetTransform( D3DTS_WORLD, (D3DMATRIX *)&mWorld );

    pDev->SetStreamSource( 0, m_pTileContainer->m_pWaterVB->GetInterface(), 0, sizeof( OCWVertex ) );

    pDev->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, 81, 0, 128 );
}

//
// Get height
//
float OCTile::GetHeight
    (
    int nx,
    int ny
    )
{
    if ( nx < 0 ) nx = 0;
    else
    if ( nx > 127 ) nx = 127;
    if ( ny < 0 ) ny = 0;
    else
    if ( ny > 127 ) ny = 127;

    float fAmp = (float)( 32768 >> m_pMapTile->m_pVoxelTile->m_dwHeightAmp ) / 32768.f;
    
    float h = 0;
    if ( m_pMapTile->m_pVoxelTile->m_Texture.m_Header.dwNumLayers > 1 )
        h = m_pMapTile->m_pVoxelTile->m_Texture.GetPixel( nx, ny, 1 );
    h = m_pMapTile->m_dwTileHeight + h * fAmp;

    return h;
}
