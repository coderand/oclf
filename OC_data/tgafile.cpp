// This file is distributed under a MIT license. See LICENSE.txt for details.
//
// TGA file
//

#include "common.h"

//
// Constructor
//
CTGASurface::CTGASurface
    (
    )
{
    memset( &m_Header, 0, sizeof( m_Header ) );
    m_Header.dwSignature = 2;
    m_pArray = NULL;
}

//
// Destructor
//
CTGASurface::~CTGASurface
    (
    )
{
}

//
// Create TGA surface
//
dword CTGASurface::Create
    ( 
    int nWidth, 
    int nHeight, 
    int nBPP
    )
{
    int nBTPP = 4;

    m_Header.wBPP = 32;
    m_Header.wWidth = nWidth;
    m_Header.wHeight = nHeight;

    m_pArray = (byte *)malloc( m_Header.wWidth * m_Header.wHeight * nBTPP );

    return sizeof( m_Header ) + ( m_Header.wWidth * m_Header.wHeight * nBTPP );
}

//
// Set pixel into TGA surface
//
void CTGASurface::SetRGBAPixel
    ( 
    int nX, 
    int nY, 
    dword dwColor
    )
{
    *(dword *)( m_pArray + 4 * ( nX + nY * m_Header.wWidth ) ) = dwColor;
}

//
// Debug dump TGA file into disk
//
void CTGASurface::Dump
    ( 
    const char *pTGAFileName 
    )
{
    FILE *fp;

    fp = fopen( pTGAFileName, "wb" );
    fwrite( &m_Header, 1, sizeof( m_Header ), fp );
    fwrite( m_pArray, 1, m_Header.wWidth * m_Header.wHeight * 4, fp );
    fclose( fp );
}
