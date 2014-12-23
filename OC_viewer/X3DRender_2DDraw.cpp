// This file is distributed under a MIT license. See LICENSE.txt for details.
//
// Set of 2D drawing methods
//

#include "common.h"

#define MAX_SPRITES 1024

//
// Initialize 2D drawing subsystem
//
bool X3DRender::Initialize2DDraw
    (
    )
{
    int i;
    int pos, vpos;

    m_nNumSprites = 0;
    m_pSpriteVerts = NEW TX3DSptireVtx[ MAX_SPRITES * 4 ];

    // create index buffer
    m_pSpriteIB = NEW CIBuffer;
    m_pSpriteIB->Create( dev, MAX_SPRITES * 6 );
    unsigned short *idx = (unsigned short *)m_pSpriteIB->Lock( MAX_SPRITES * 6 );
    
    pos = 0;
    vpos = 0;
    for ( i = 0; i < MAX_SPRITES; i++ )
    {
        idx[ pos++ ] = vpos;
        idx[ pos++ ] = vpos + 2;
        idx[ pos++ ] = vpos + 1;

        idx[ pos++ ] = vpos;
        idx[ pos++ ] = vpos + 3;
        idx[ pos++ ] = vpos + 2;

        vpos += 4;
    }
    m_pSpriteIB->Unlock();

    // create vertex buffer
    m_pSpriteVB = NEW CVBuffer;
    m_pSpriteVB->Create( dev, MAX_SPRITES * 4, FVF_TX3DSptireVtx, sizeof( TX3DSptireVtx ), true );

    return true;
}

//
// Push draw 2D text
//
int X3DRender::PushDraw2DText
    ( 
    HX3DFont hFont, 
    float fX, 
    float fY, 
    float fScale,
    unsigned int dwColor,
    int nPDFlag,
    const char *pFormat, 
    ... 
    )
{
    static char buff[ 512 ];
    X3DFont *fnt = GetFontSlot( hFont );
    float texcoords[ 4 ];
    int chidx;
    float w, h;
    va_list arg;
    int i;

    va_start( arg, pFormat );
    vsprintf( buff, pFormat, arg );
    va_end( arg );

    for ( i = 0; i < (int)strlen( buff ); i++ )
    {
        chidx = buff[ i ];
        texcoords[ 0 ] = fnt->m_pChars[ chidx ].fU;
        texcoords[ 1 ] = fnt->m_pChars[ chidx ].fV;
        texcoords[ 2 ] = fnt->m_pChars[ chidx ].fU2;
        texcoords[ 3 ] = fnt->m_pChars[ chidx ].fV2;
        w = fScale * fnt->m_pChars[ chidx ].fDx;
        h = fScale * fnt->m_pChars[ chidx ].fDy;

        PushDraw2DSprite( fnt->m_hTexture, fX, fY, w, h, dwColor, nPDFlag | EPDF_NOCENTER, 0, texcoords );

        fX += w;
    }

    return 0;
}

//
// Push draw 2D sprite
//
void X3DRender::PushDraw2DSprite
    ( 
    HX3DTexture hTexture, 
    float fX, 
    float fY, 
    float fWidth, 
    float fHeight, 
    unsigned int dwColor, 
    int nPDFlag,
    float fAngle, 
    float *pTexCoords
    )
{
    if ( m_nNumSprites >= MAX_SPRITES )
        return;

    fX *= BasedWidth_K;
    fY *= BasedHeight_K;
    fWidth *= BasedWidth_K;
    fHeight *= BasedHeight_K;
    float xc = fX;
    float yc = fY;
    TX3DSptireVtx *vtx = m_pSpriteVerts + m_nNumSprites * 4;
    float fZ = 0;
    float fRHW = 1.f;

    if ( !(nPDFlag & EPDF_NOCENTER) )
    {
        xc -= fWidth * 0.5f;
        yc -= fHeight * 0.5f;
    }

    if ( fAngle == 0 )
    {
        vtx[ 0 ].fX = xc; vtx[ 0 ].fY = yc; vtx[ 0 ].fZ = fZ; vtx[ 0 ].dwColor = dwColor; vtx[ 0 ].fRHW = fRHW;
        vtx[ 1 ].fX = xc; vtx[ 1 ].fY = yc + fHeight; vtx[ 1 ].fZ = fZ; vtx[ 1 ].dwColor = dwColor; vtx[ 1 ].fRHW = fRHW;
        vtx[ 2 ].fX = xc + fWidth; vtx[ 2 ].fY = yc + fHeight; vtx[ 2 ].fZ = fZ; vtx[ 2 ].dwColor = dwColor; vtx[ 2 ].fRHW = fRHW;
        vtx[ 3 ].fX = xc + fWidth; vtx[ 3 ].fY = yc; vtx[ 3 ].fZ = fZ; vtx[ 3 ].dwColor = dwColor; vtx[ 3 ].fRHW = fRHW;

        if ( pTexCoords )
        {
            vtx[ 0 ].fU = pTexCoords[ 0 ]; vtx[ 0 ].fV = pTexCoords[ 1 ];
            vtx[ 1 ].fU = pTexCoords[ 0 ]; vtx[ 1 ].fV = pTexCoords[ 3 ];
            vtx[ 2 ].fU = pTexCoords[ 2 ]; vtx[ 2 ].fV = pTexCoords[ 3 ];
            vtx[ 3 ].fU = pTexCoords[ 2 ]; vtx[ 3 ].fV = pTexCoords[ 1 ];
        }
        else
        {
            vtx[ 0 ].fU = 0; vtx[ 0 ].fV = 0;
            vtx[ 1 ].fU = 0; vtx[ 1 ].fV = 1;
            vtx[ 2 ].fU = 1; vtx[ 2 ].fV = 1;
            vtx[ 3 ].fU = 1; vtx[ 3 ].fV = 0;
        }
    }
    else
    {
    }

    m_nNumSprites++;

    // FIXME : hack
    SetTexture( 0, hTexture );
}

//
// Flush sprite buffer onto a screen
//
void X3DRender::FlushSpriteBuffer
    (
    )
{
    TX3DSptireVtx *vtx = (TX3DSptireVtx *)m_pSpriteVB->Lock( m_nNumSprites * 4 );
    memcpy( vtx, m_pSpriteVerts, sizeof( TX3DSptireVtx ) * m_nNumSprites * 4 );
    m_pSpriteVB->Unlock();

    HRESULT hr;

    hr = dev->SetVertexShader( NULL );
    hr = dev->SetFVF( FVF_TX3DSptireVtx );
    hr = dev->SetStreamSource( 0, m_pSpriteVB->GetInterface(), 0, sizeof( TX3DSptireVtx ) );
    hr = dev->SetIndices( m_pSpriteIB->GetInterface() );

    hr = dev->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, m_nNumSprites * 4, 0, m_nNumSprites * 2 );

    m_nNumSprites = 0;
}
