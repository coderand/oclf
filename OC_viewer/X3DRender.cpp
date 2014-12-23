// This file is distributed under a MIT license. See LICENSE.txt for details.
//
// X3D Rendering system
//

#include "common.h"
#include "ocdata.h"
#include "CRenderer.h"

//
// Initialize device
//
bool X3DRender::Initialize
    (
    )
{
    m_nNumTextures = 0;
    memset( m_pTextures, 0, sizeof( X3DTexture * ) * X3D_MAX_TEXTURECACHE );

    m_nNumFonts = 0;
    memset( m_pFonts, 0, sizeof( X3DFont * ) * X3D_MAX_FONTS );

    m_nNumDXShaders = 0;

    //
    // do 3D system intialization
    //

    Initialize2DDraw();
    m_hFontCommon = RegisterFont( "luxsys/sf.xfn" );

    if ( !m_hFontCommon )
    {
        MessageBox( NULL, "luxsys directory or luxsys.pak not found", "OCLF Critical Error", MB_OK );
        return false;
    }

    return true;
}

//
// Destructor
//
X3DRender::~X3DRender
    (
    )
{
    int i;

    for ( i = 0; i < m_nNumFonts; i++ )
        DEL( m_pFonts[ i ] );

    VDEL( m_pSpriteVerts );
    DEL( m_pSpriteIB );
    DEL( m_pSpriteVB );
}

//
// Set texture into given texture stage
//
void X3DRender::SetTexture
    ( 
    int nStage, 
    HX3DTexture hTexture,
    int nLayer
    )
{
    if ( hTexture )
    {
        dev->SetTexture( nStage, m_pTextures[ hTexture - 1 ]->m_pTexture[ nLayer ] );
    }
    else
    {
        dev->SetTexture( nStage, NULL );
    }
}

char currPath[ 1024 ];
char tempPath[ 1024 ];

//
// Register given texture
//
HX3DTexture X3DRender::RegisterTexture
    ( 
    const char *pTexName,
    int nRegFlag
    )
{
    char texname[ XFS_MAX_FILENAME ];
    const char *pext;
    HX3DTexture hCurr;
    X3DTexture *pTex;

    strcpy( texname, pTexName );
    xfs->FixFileName( texname );

    // find it in exists textures
    hCurr = FindExistTexture( texname );

    DPrint( "X3DRender::RegisterTexture : register \"" );
    DPrint( texname );
    DPrint( "\"..." );

    if ( !hCurr )
    {
        // texture not found, let's create a new texture
        pext = xfs->GetFileExtention( texname );

        hCurr = AddTextureSlot();
        pTex = GetTextureSlot( hCurr );

        strcpy( pTex->m_pFileName, texname );
        pTex->m_pTexture[ 0 ] = NULL;
        pTex->m_pTexture[ 1 ] = NULL;

        // FIXME : HACK
        {
            int lay;
            HXFSFile    fp;
            char        *tgafile;
            int         size;
            int         mipCount = nRegFlag & ERTF_NOMIP ? 1 : 8;

            int numLayers = 1;
            if ( nRegFlag & ERTF_BUMPMAP )
                numLayers = 2;

            if ( !strcmp( pext, "btm" ) )
            {
                CBTMSurface btm;
                char *data;

                fp = xfs->Open( texname );
                if ( fp )
                {
                    data = (char *)ALLOCATE( xfs->SizeOf( fp ) );
                    xfs->Read( fp, data, xfs->SizeOf( fp ) );
                    xfs->Close( fp );
                    btm.LoadFromMemory( data );

                    int height = btm.m_Header.dwHeight;
                    if ( ( nRegFlag & ERTF_CLAMPSQUAREX ) && ( height > btm.m_Header.dwWidth ) )
                        height = btm.m_Header.dwWidth;

                    size = sizeof( TTGAHeader ) + 4 * btm.m_Header.dwWidth * height;
                    tgafile = (char *)ALLOCATE( size );
                    memset( tgafile, 0, size );
                    ((TTGAHeader *)tgafile)->dwSignature = 2;
                    ((TTGAHeader *)tgafile)->wBPP = 32;
                    ((TTGAHeader *)tgafile)->wWidth = btm.m_Header.dwWidth;
                    ((TTGAHeader *)tgafile)->wHeight = height;

                    for ( lay = 0; lay < numLayers; lay++ )
                    {

                    unsigned int *ptr = (unsigned int *)( (char *)tgafile + sizeof( TTGAHeader ) );
                    for ( int y = 0; y < height; y++ )
                        for ( int x = 0; x < btm.m_Header.dwWidth; x++ )
                        {
                            if ( nRegFlag & ERTF_SQUARESWAPXY ) 
                                *ptr = btm.GetRGBAPixel( y, x, lay );
                            else
                                *ptr = btm.GetRGBAPixel( x, y, lay );
                            ptr++;
                        }
        
                    D3DXCreateTextureFromFileInMemoryEx( dev, tgafile, size,
                        D3DX_DEFAULT, D3DX_DEFAULT, mipCount, 0, D3DFMT_A8R8G8B8,
                        D3DPOOL_MANAGED,
                        D3DX_FILTER_POINT | D3DX_FILTER_MIRROR,
                        D3DX_FILTER_BOX, 0, NULL, NULL, &pTex->m_pTexture[ lay ] );
                    }

                    FREE( tgafile );
                    FREE( data );
                }
            }
            else
            {
                fp = xfs->Open( texname );
                if ( fp )
                {
                    size = xfs->SizeOf( fp );
                    tgafile = (char *)ALLOCATE( xfs->SizeOf( fp ) );
                    xfs->Read( fp, tgafile, xfs->SizeOf( fp ) );
                    xfs->Close( fp );

                    bool bJPG = !strcmp( pext, "jpg" );
                    
                    if ( bJPG )
                    {
                        ::GetCurrentDirectory( 1024, currPath );
                        ::GetTempPath( 1024, tempPath );
                        ::SetCurrentDirectory( tempPath );
                    }

                    HRESULT hr = D3DXCreateTextureFromFileInMemoryEx( dev, tgafile, size,
                        D3DX_DEFAULT, D3DX_DEFAULT, mipCount, 0, D3DFMT_A8R8G8B8,
                        D3DPOOL_MANAGED,
                        D3DX_FILTER_POINT | D3DX_FILTER_MIRROR,
                        D3DX_FILTER_BOX, 0, NULL, NULL, &pTex->m_pTexture[ 0 ] );

                    if ( bJPG )
                        ::SetCurrentDirectory( currPath );

                    FREE( tgafile );
                }
            }
        }

        m_nNumTextures++;
    }

    pTex = GetTextureSlot( hCurr );
    pTex->m_nRef++;

    DPrint( "Ref %d\n", pTex->m_nRef );

    return hCurr;
}

//
// Unregister texture
//
void X3DRender::UnregisterTexture
    (
    HX3DTexture hTexture
    )
{
    X3DTexture *pTex;

    if ( !hTexture )
        return;

    pTex = GetTextureSlot( hTexture );

    pTex->m_nRef--;
    
    DPrint( "X3DRender::UnregisterTexture : \"%s\"...", pTex->m_pFileName );
    DPrint( "Ref %d\n", pTex->m_nRef );

    if ( pTex->m_nRef <= 0 )
    {
        for ( int i = 0; i < 8; i++ )
            rend_SetTexture( i, NULL );

        RELEASE( pTex->m_pTexture[ 0 ] );
        RELEASE( pTex->m_pTexture[ 1 ] );
        DEL( m_pTextures[ hTexture - 1 ] );
        m_pTextures[ hTexture - 1 ] = NULL;
    }
}

//
// Get internal texture slot
//
X3DTexture *X3DRender::GetTextureSlot
    ( 
    HX3DTexture hTexture 
    )
{
    return m_pTextures[ hTexture - 1 ];
}

//
// Find given texture in exist set
//
HX3DTexture X3DRender::FindExistTexture
    (
    const char *pFileName
    )
{
    int tex, pos;

    tex = m_nNumTextures;
    pos = 0;
    while ( tex && pos < X3D_MAX_TEXTURECACHE )
    {
        if ( m_pTextures[ pos ] )
        {
            if ( !strcmp( pFileName, m_pTextures[ pos ]->m_pFileName ) )
            {
                return pos + 1;
            }
            tex--;
        }
        pos++;
    }

    return 0;
}

//
// Create new texture slot
//
HX3DTexture X3DRender::AddTextureSlot
    (
    )
{
    int pos;

    pos = 0;
    while ( m_pTextures[ pos ] && pos < X3D_MAX_TEXTURECACHE )
        pos++;

    if ( pos >= X3D_MAX_TEXTURECACHE )
        return 0;

    m_pTextures[ pos ] = NEW X3DTexture;
    m_pTextures[ pos ]->m_nRef = 0;

    return pos + 1;
}
