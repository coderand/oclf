// This file is distributed under a MIT license. See LICENSE.txt for details.

#include "common.h"

void CBaseVoxelTile::LoadData
    ( 
    byte *pBase
    )
{
    char chBTMName[ 256 ];
    char *pN;

    chBTMName[ 0 ] = 0;
    if ( m_pBasePath )
        strcat( chBTMName, m_pBasePath );

    strcat( chBTMName, m_pTileName );
    pN = strchr( chBTMName, '_' );
    pN[ 1 ] = 'v';
    strcat( chBTMName, ".btm" );

    m_Texture.LoadFromFile( chBTMName );

    memset( m_fMinEdgeHeight, 0, sizeof( m_fMinEdgeHeight ) );
    memset( m_fMaxEdgeHeight, 0, sizeof( m_fMaxEdgeHeight ) );
    memset( m_fEdgeDelta, 0, sizeof( m_fEdgeDelta ) );

    m_pHeader = NULL;
    m_pChunks = NULL;

    if ( m_pFileData )
    {
        m_pHeader = (TOCLndHeader *)m_pFileData;
        m_pChunks = (TOCLHChunk *)( (byte *)m_pHeader + sizeof( TOCLndHeader ) );

        // heightamp and heightmax have only TVT and VTL files
        m_dwHeightAmp = 0;
        m_dwHeightMax = 0;

        if ( m_pHeader->dwTypeID == OC_FILEID_TVT || m_pHeader->dwTypeID == OC_FILEID_VTL )
        {
            m_dwHeightAmp = *((dword *)(pBase + m_pChunks[ m_pHeader->dwChunkCount - 3 ].dwOffset));
            m_dwHeightMax = *((dword *)(pBase + m_pChunks[ m_pHeader->dwChunkCount - 2 ].dwOffset));
        }
    }
}

void CVoxelTile::LoadData
    ( 
    byte *pBase
    )
{
    CBaseVoxelTile::LoadData( pBase );

    if ( !m_pFileData )
        return;
}

void CTexturedVoxelTile ::LoadData
    ( 
    byte *pBase
    )
{
    CBaseVoxelTile::LoadData( pBase );

    if ( !m_pFileData )
        return;
}
