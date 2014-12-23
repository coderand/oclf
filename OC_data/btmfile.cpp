// This file is distributed under a MIT license. See LICENSE.txt for details.
//
// BTM file
//

#include "common.h"

//
// Constructor
//
CBTMSurface::CBTMSurface
    (
    )
{
    memset( &m_Header, 0, sizeof( m_Header ) );
    m_pArray = NULL;
    m_pPalette = NULL;
}

//
// Destructor
//
CBTMSurface::~CBTMSurface
    (
    )
{
    DEL( m_pPalette );
    FREE( m_pArray );
}

//
// Load BTM surface from file
//
bool CBTMSurface::LoadFromFile
    ( 
    const char *pFileName 
    )
{
    void *pFile;
    //FILE *fp;
    HXFSFile fp;
    int size;

    //fp = fopen( pFileName, "rb" );
    fp = xfs->Open( pFileName );
    if ( !fp )
        return false;

    //fseek( fp, 0, SEEK_END );
    //size = ftell( fp );
    //fseek( fp, 0, SEEK_SET );
    size = xfs->SizeOf( fp );
    pFile = ALLOCATE( size );
    //fread( pFile, 1, size, fp );
    //fclose( fp );
    xfs->Read( fp, pFile, size );
    xfs->Close( fp );

    LoadFromMemory( pFile );
    FREE( pFile );

    return true;
}

//
// Load BTM surface from memory
//
dword CBTMSurface::LoadFromMemory
    ( 
    const void *l_pData
    )
{
    const byte *pData = (byte *)l_pData;
    const void *pStartData = pData;

    memcpy( &m_Header, pData, sizeof( m_Header ) );
    pData += sizeof( m_Header );

    m_pArray = (byte *)ALLOCATE( m_Header.dwImageSize );
    memcpy( m_pArray, pData, m_Header.dwImageSize );
    pData += m_Header.dwImageSize;

    // ok, try to load palette
    m_pPalette = NULL;
    if ( (int)( (byte *)pData - (byte *)pStartData ) < (int)m_Header.dwBTMFileSize )
    {
        m_pPalette = NEW CPLTSurface;

        pData += m_pPalette->LoadFromMemory( pData );
    }


    return (byte *)pData - (byte *)pStartData;
}

//
// Get pixel from BTM surface
//
dword CBTMSurface::GetPixel
    (
    int nX,
    int nY,
    byte nLayer,
    byte nMipLevel
    )
{
    if ( nLayer >= m_Header.dwNumLayers )
        return 0; // FIXME : this is incorrect image

    if ( nMipLevel )
    {
        nX = nX >> nMipLevel;

        // 1 : w / 2                                    =  1 * w / 2
        // 2 : w / 2 + w / 4                            =  3 * w / 4
        // 3 : w / 2 + w / 4 + w / 8                    =  7 * w / 8
        // 4 : w / 2 + w / 4 + w / 8 + w / 16           = 15 * w / 16
        // 5 : w / 2 + w / 4 + w / 8 + w / 16 + w / 32  = 31 * w / 32

        nX += ( ( m_Header.dwWidth << ( nMipLevel - 1 ) ) - m_Header.dwWidth ) >> ( nMipLevel - 1 );

        nY = nY >> nMipLevel;
        nY += m_Header.dwWidth;
    }

    byte * ptr = m_pArray + m_Header.dwNumLayers * ( nX + nY * m_Header.dwWidth ) + nLayer;

    if ( ptr >= ( m_pArray + m_Header.dwImageSize ) )
    {
        // FIXME : out of pixels array
        //DPrint( "\n**********\n" );
        //DPrint( "x = %d, y = %d, lay = %d, mip = %d\n", nX, nY, nLayer, nMipLevel );
        //DPrint( "w = %d, h = %d, nl = %d\n", m_Header.dwWidth, m_Header.dwHeight, m_Header.dwNumLayers );
        return 0;
    }

    return *ptr;
}

//
// Get RGBA pixel from BTM surface
//
dword CBTMSurface::GetRGBAPixel
    ( 
    int x, 
    int y, 
    byte nLayer,
    byte nMipLevel
    )
{
    dword color;

    if ( m_pPalette )
    {
        color = GetPixel( x, y, nLayer, nMipLevel );

        if ( nLayer )
            color = 0xff000000 | ( color << 16 ) | ( color << 8 ) | color;
        else
        {
            color = m_pPalette->GetColor( color );
        }
    }
    else
    {
        color = GetPixel( x, y, nLayer, nMipLevel );
        color = 0xff000000 | ( color << 16 ) | ( color << 8 ) | color;
    }

    return color;
}

//
// Debug dump layer into TGA file
//
void CBTMSurface::DumpLayer
    ( 
    const char *pTGAFileName, 
    int nLayer
    )
{
    CTGASurface tga;
    int x, y;
    dword color;

    /*if ( nLayer )
    {
        int ml = 5;
        tga.Create( m_Header.dwWidth, m_Header.dwWidth );
        for ( y = 0; y < m_Header.dwWidth; y++ )
            for ( x = 0; x < m_Header.dwWidth; x++ )
            {
                color = GetRGBAPixel( x, y, nLayer, ml );
                tga.SetRGBAPixel( x, m_Header.dwWidth - y - 1, color );
            }
    }
    else
    {*/
    tga.Create( m_Header.dwWidth, m_Header.dwHeight );

    for ( y = 0; y < m_Header.dwHeight; y++ )
        for ( x = 0; x < m_Header.dwWidth; x++ )
        {
            color = GetRGBAPixel( x, y, nLayer );

            tga.SetRGBAPixel( x, m_Header.dwHeight - y - 1, color );
        }
//    }

    tga.Dump( pTGAFileName );
}
