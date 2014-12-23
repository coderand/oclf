// This file is distributed under a MIT license. See LICENSE.txt for details.
//
// PLT file
//

#include "common.h"

//
// Constructor
//
CPLTSurface::CPLTSurface
    (
    )
{
    memset( &m_Header, 0, sizeof( m_Header ) );
    m_pColors = NULL;
}

//
// Destructor
//
CPLTSurface::~CPLTSurface
    (
    )
{
    VDEL( m_pColors );
}

//
// Load PLT surface from memory
//
dword CPLTSurface::LoadFromMemory
    ( 
    const void *l_pData
    )
{
    const byte *pData = (byte *)l_pData;
    const void *pStartData = pData;
    dword i;
    dword color;

    memcpy( &m_Header, pData, sizeof( m_Header ) );
    pData += sizeof( m_Header );

    m_pColors = NEW TPLTColor[ m_Header.dwNumOfColors ];

    memcpy( m_pColors, pData, m_Header.dwNumOfColors * sizeof( TPLTColor ) );
    for ( i = 0; i < m_Header.dwNumOfColors; i++ )
    {
        color = m_pColors[ i ].dwRGB;
        m_pColors[ i ].dwRGB = ( ( color & 0x00ff0000 ) >> 16 ) | ( ( color & 0x000000ff ) << 16 ) | ( color & 0xff00ff00 );
    }
    pData += m_Header.dwNumOfColors * sizeof( TPLTColor );

    return (byte *)pData - (byte *)pStartData;
}

//
// Get color from index
//
dword CPLTSurface::GetColor
    ( 
    dword dwIndex 
    )
{
    return m_pColors[ dwIndex ].dwRGB;
}
