// This file is distributed under a MIT license. See LICENSE.txt for details.
//
// RBT file
//

#include "common.h"

//
// Load RBT file from memory
//
dword CRBTSurface::LoadFromMemory
    ( 
    const void *pData 
    )
{
    m_pHeader = (TRBTHeader *)pData;
    m_pOffs = (int *)( (char *)pData + sizeof( TRBTHeader ) );
    m_bUnpacked = NULL;
    m_pPalette = NULL;

    if ( ( m_pHeader->dwEndOffset + sizeof( TRBTHeader ) ) != m_pHeader->dwRBTFileSize )
    {
        // load palette
        m_pPalette = NEW CPLTSurface;
        m_pPalette->LoadFromMemory( (char *)m_pHeader + m_pHeader->dwEndOffset + sizeof( TRBTHeader ) );
    }

    return 0;
}

//
// Unpack image into internal memory
//
void CRBTSurface::Unpack
    (
    )
{
    dword i;

    m_bUnpacked = NEW char[ m_pHeader->dwWidth * m_pHeader->dwHeight ];
    memset( m_bUnpacked, 0, m_pHeader->dwWidth * m_pHeader->dwHeight );

    for ( i = 0; i < m_pHeader->dwHeight; i++ )
    {
        UnpackScanline( (char *)m_pHeader + sizeof( TRBTHeader ) + m_pOffs[ i ], m_bUnpacked + i * m_pHeader->dwWidth );
    }
}

//
// Unpack given scanline
//
void CRBTSurface::UnpackScanline
    ( 
    char *src,
    char *dest
    )
{
    char *sdest = dest;

    while ( dest - sdest < (int)m_pHeader->dwWidth )
    {
        if ( !(*src) )
            return;

        if ( *src & 0x80 ) // compressed
        {
            int len = 1 + ( *src & 0x3F );

            if ( *src & 0x40 ) // zeroes
            {
                src++;
                dest += len;
            }
            else
            {
                src++;
                while ( len-- )
                    *dest++ = *src++;
            }
        }
    }
}

//
// Debug dump layer into TGA file
//
void CRBTSurface::DumpLayer
    ( 
    const char *pTGAFileName, 
    int nLayer
    )
{
    CTGASurface tga;
    dword x, y;
    dword color;

    Unpack();

    tga.Create( m_pHeader->dwWidth, m_pHeader->dwHeight );

    for ( y = 0; y < m_pHeader->dwHeight; y++ )
        for ( x = 0; x < m_pHeader->dwWidth; x++ )
        {
            color = (unsigned char)m_bUnpacked[ x + y * m_pHeader->dwWidth ];

            if ( m_pPalette )
                color = m_pPalette->GetColor( color );
            else
                color = ( color << 16 ) | ( color << 8 ) | color;

            tga.SetRGBAPixel( x, m_pHeader->dwHeight - y - 1, color );
        }

    tga.Dump( pTGAFileName );
}

//
// Get pixel
//
dword CRBTSurface::GetPixel
    ( 
    int nX, 
    int nY
    )
{
    return (unsigned char)m_bUnpacked[ nX + nY * m_pHeader->dwWidth ];
}
