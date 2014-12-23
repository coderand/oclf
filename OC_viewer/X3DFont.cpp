// This file is distributed under a MIT license. See LICENSE.txt for details.
//
// X3D font
//

#include "common.h"

//
// Register font in the system
//
HX3DFont X3DRender::RegisterFont
    ( 
    const char *pFontName 
    )
{
    X3DFont *pFont;

    pFont = NEW X3DFont;
    m_pFonts[ m_nNumFonts ] = pFont;

    DPrint( "X3DRender::RegisterFont : register \"" );
    DPrint( pFontName );
    DPrint( "\"\n" );

    if ( !pFont->LoadFromFile( pFontName ) )
    {
        DEL( pFont );
        DPrint( "ERR : X3DRender::RegisterFont : Can't load font\n" );
        return 0;
    }

    m_nNumFonts++;

    return m_nNumFonts;
}

//
// Get X3DFont by HX3DFont
//
X3DFont *X3DRender::GetFontSlot
    ( 
    HX3DFont hFont
    )
{
    return m_pFonts[ hFont - 1 ];
}

//
// X3DFont constructor
//
X3DFont::X3DFont
    (
    )
{
    memset( m_pFontName, 0, sizeof( m_pFontName ) );
    memset( m_pChars, 0, sizeof( m_pChars ) );
    m_hTexture = 0;
}

//
// Load and create font from file
//
bool X3DFont::LoadFromFile
    ( 
    const char *pFontName 
    )
{
    HXFSFile fp;
    char texName[ XFS_MAX_FILENAME ];

    fp = xfs->Open( pFontName );
    if ( !fp )
        return false;

    strcpy( m_pFontName, pFontName );
    xfs->FixFileName( m_pFontName );
    xfs->Read( fp, m_pChars, xfs->SizeOf( fp ) );
    xfs->Close( fp );

    // register texture
    strcpy( texName, m_pFontName );
    char *pExt = (char *)xfs->GetFileExtention( texName );
    pExt[ 0 ] = 't';
    pExt[ 1 ] = 'g';
    pExt[ 2 ] = 'a';
    m_hTexture = x3d->RegisterTexture( texName, ERTF_NOMIP );

    return true;
}
