// This file is distributed under a MIT license. See LICENSE.txt for details.
//
// Landscape data class
//

#include "common.h"

//
// Constructor
//
COCLandscapeData::COCLandscapeData
    (
    )
{
    m_ppMapTiles = NULL;
}

//
// Destructor
//
COCLandscapeData::~COCLandscapeData
    (
    )
{
    unsigned int x, y;
    int     nTile = 0;

    for ( y = 0; y < m_pLndSize->dwHeight; y++ )
    {
        for ( x = 0; x < m_pLndSize->dwWidth; x++ )
        {
            DEL( m_ppMapTiles[ nTile ] );

            nTile++;
        }
    }

    VDEL( m_ppMapTiles );
}

//
// Load data from memory
//
dword COCLandscapeData::LoadFromMemory
    ( 
    const void *l_pData 
    )
{
    m_pLND = (byte *)l_pData;

    m_pHeader = (TOCLndHeader *)m_pLND;
    m_pChunks = (TOCLHChunk *)( m_pLND + sizeof( TOCLndHeader ) );

    for ( int i = 0; i < 8; i++ )
    {
        *((dword *)&m_fWaterHeights[ i ]) = *(dword *)(m_pLND + m_pChunks[ 10 + i ].dwOffset);
        m_fWaterHeights[ i ] *= 0.125f;
    }
    
    m_pRecName = (COCStr *)GetLNDPtr( m_pChunks[ LND_REC_NAME ].dwOffset );
    m_pRecVal = REC_VAL_PTR( m_pRecName );

    m_pSkyRecName = (COCStr *)GetLNDPtr( m_pChunks[ LND_REC_SKYNAME ].dwOffset );
    m_pSkyRecVal = REC_VAL_PTR( m_pSkyRecName );

    m_pLndProp = (TOCLndProp *)( (byte *)m_pRecVal + m_pRecVal->m_nLength + sizeof( int ) );
    m_pLndSize = (TOCLndSize *)( (byte *)m_pLndProp + m_pLndProp->dwChunkCount * sizeof( TOCLHChunk ) + sizeof( int ) );

    m_dwXOrigin = *(dword *)GetLNDPtr( m_pChunks[ LND_REC_XORIGIN ].dwOffset );
    m_dwYOrigin = 128 * m_pLndSize->dwHeight - *(dword *)GetLNDPtr( m_pChunks[ LND_REC_YORIGIN ].dwOffset );

    m_pFirstRec = (TOCLndRecHeader *)( (byte *)m_pLndSize + sizeof( TOCLndSize ) );

    m_dwMinHeight = 99999; // big value
    m_dwMaxHeight = 0;
    m_nNumTileContainers = 0;
    int numvt;

    //
    // Create voxel tiles
    //
    m_ppMapTiles = (CMapTile **) NEW int[ m_pLndSize->dwWidth * m_pLndSize->dwHeight ];
    memset( m_ppMapTiles, 0, sizeof( int ) * m_pLndSize->dwWidth * m_pLndSize->dwHeight );
    {
        dword   x, y;
        TOCLndRecHeader *pRec = m_pFirstRec;
        int     nTile = 0;
        COCStr  *pRecName;
        COCStr  *pRecVal;
        char    string[ 256 ];

        for ( y = 0; y < m_pLndSize->dwHeight; y++ )
        {
            for ( x = 0; x < m_pLndSize->dwWidth; x++ )
            {
                pRecName = (COCStr *)GetLNDPtr( pRec->dwOffsRecName );
                pRecVal = REC_VAL_PTR( pRecName );

                pRecVal->CopyTo( string );
                m_ppMapTiles[ nTile ] = NEW CMapTile;

                if ( !( m_ppMapTiles[ nTile ]->m_pVoxelTile = FindVoxelTile( string, numvt ) ) )
                {
                    if ( pRecName->Cmp( "PVoxelTile" ) )
                        m_ppMapTiles[ nTile ]->m_pVoxelTile = NEW CVoxelTile;
                    else
                    if ( pRecName->Cmp( "PTexturedVoxelTile" ) )
                        m_ppMapTiles[ nTile ]->m_pVoxelTile = NEW CTexturedVoxelTile;
                    else
                    if ( pRecName->Cmp( "PFlatTile" ) )
                        m_ppMapTiles[ nTile ]->m_pVoxelTile = NEW CFlatVoxelTile;
                    else
                        printf( "ERR : incorrect tile type\n" );

                    pRecVal->CopyTo( m_ppMapTiles[ nTile ]->m_pVoxelTile->m_pTileName );
                    m_ppMapTiles[ nTile ]->m_pVoxelTile->m_pFileData = NULL;

                    // ok, file has inline data
                    //byte *a = ((byte *)REC_VAL_PTR( pRecVal ) );
                    //byte *b = (byte *)GetLNDPtr( pRec->dwOffsRecNext );

                    if ( ( (byte *)REC_VAL_PTR( pRecVal ) ) != (byte *)GetLNDPtr( pRec->dwOffsRecNext ) )
                    {
                        m_ppMapTiles[ nTile ]->m_pVoxelTile->m_pFileData = (byte *)REC_VAL_PTR( pRecVal );
                    }

                    m_ppMapTiles[ nTile ]->m_pVoxelTile->m_pBasePath = m_pBasePath;
                    m_ppMapTiles[ nTile ]->m_pVoxelTile->LoadData( m_pLND );
                    m_ppMapTiles[ nTile ]->m_bUniq = true;
                    m_ppMapTiles[ nTile ]->m_nTileRefNum = m_nNumTileContainers;
#ifdef OC_DO_TILE_CONNECTION
                    CalculateTileEdges( m_ppMapTiles[ nTile ] );
#endif // OC_DO_TILE_CONNECTION
                    m_nNumTileContainers++;
                }
                else
                {
                    m_ppMapTiles[ nTile ]->m_nTileRefNum = m_ppMapTiles[ numvt ]->m_nTileRefNum;
                }

                dword dwHeight = *(dword *)GetLNDPtr( pRec->dwUnknown[ 0 ] );
                m_ppMapTiles[ nTile ]->m_dwTileHeight = dwHeight;
                m_ppMapTiles[ nTile ]->m_bTileHasWater = (*(byte *)GetLNDPtr( pRec->dwUnknown[ 11 ] ));
                m_ppMapTiles[ nTile ]->m_pfWaterHeights = m_fWaterHeights;

                if ( dwHeight < m_dwMinHeight )
                    m_dwMinHeight = dwHeight;
                if ( dwHeight > m_dwMaxHeight )
                    m_dwMaxHeight = dwHeight;
                /**
                printf( "tile : %10s ", m_ppMapTiles[ nTile ]->m_pVoxelTile->m_pTileName );
                //printf( "y=%5d ", *(dword *)GetLNDPtr( pRec->dwUnknown[ 0 ] ) );
                printf( "l=%4d h=%4d ", m_ppMapTiles[ nTile ]->m_pVoxelTile->m_dwHeightAmp, m_ppMapTiles[ nTile ]->m_pVoxelTile->m_dwHeightMax );
                printf( "%5d", *(dword *)GetLNDPtr( pRec->dwUnknown[ 1 ] ) );
                printf( "%5d", *(dword *)GetLNDPtr( pRec->dwUnknown[ 2 ] ) );
                printf( "%5d", *(dword *)GetLNDPtr( pRec->dwUnknown[ 3 ] ) );
                printf( "%5d", *(dword *)GetLNDPtr( pRec->dwUnknown[ 4 ] ) );
                printf( "%5d", *(dword *)GetLNDPtr( pRec->dwUnknown[ 5 ] ) );
                printf( "%5d", *(dword *)GetLNDPtr( pRec->dwUnknown[ 6 ] ) );
                printf( "%5d", (dword)(*(byte *)GetLNDPtr( pRec->dwUnknown[ 7 ] )) );
                printf( "%5d", (dword)(*(byte *)GetLNDPtr( pRec->dwUnknown[ 8 ] )) );
                printf( "%5d", (dword)(*(byte *)GetLNDPtr( pRec->dwUnknown[ 9 ] )) );
                printf( "%5d", (dword)(*(byte *)GetLNDPtr( pRec->dwUnknown[ 10 ] )) );
                printf( "%5d\n", (dword)(*(byte *)GetLNDPtr( pRec->dwUnknown[ 11 ] )) );
                /**/

                nTile++;
                pRec = (TOCLndRecHeader *)GetLNDPtr( pRec->dwOffsRecNext );
            }
        }
    }

#ifdef OC_DO_TILE_CONNECTION
    //
    // Calculate deltas on edges between tiles
    //
    for ( int y = 0; y < m_pLndSize->dwHeight; y++ )
        for ( int x = 0; x < m_pLndSize->dwWidth; x++ )
            ClaculateEdgeDeltas( x, y );
#endif // OC_DO_TILE_CONNECTION

    m_dwMaxHeight += 256; // plus maximal local height of voxel

    return 0;
}

//
// Add unique tile into world
//
CBaseVoxelTile	*COCLandscapeData::FindVoxelTile
    ( 
    const char *pName,
    int &num
    )
{
    CMapTile **pTile = m_ppMapTiles;
    int i;
    int numTiles = m_pLndSize->dwWidth * m_pLndSize->dwHeight;

    for ( i = 0; i < numTiles; i++, pTile++ )
    {
        num = i;

        if ( !( *pTile ) || !( (*pTile)->m_pVoxelTile ) )
            return NULL;

        if ( !strcmp( pName, (*pTile)->m_pVoxelTile->m_pTileName ) )
            return (*pTile)->m_pVoxelTile;
    }

    num = -1;

    return NULL;
}

//
// Build level's map
//
void COCLandscapeData::Dump
    ( 
    const char *pTGAFileName
    )
{
    CTGASurface tga;
    dword x, y;
    dword color, color0;
    int lWidth, lHeight;
    dword tx, ty;
    float fHeightRange = 255.f / (float)(m_dwMaxHeight - m_dwMinHeight);

    lWidth = 64 * m_pLndSize->dwWidth;
    lHeight = 64 * m_pLndSize->dwHeight;

    tga.Create( lWidth, lHeight );

    for ( ty = 0; ty < m_pLndSize->dwHeight; ty++ )
    {
        for ( tx = 0; tx < m_pLndSize->dwWidth; tx++ )
        {
            int lx = 64 * tx;
            int ly = 64 * ty;
            CMapTile *pTile = m_ppMapTiles[ tx + ty * m_pLndSize->dwWidth ];

            for ( y = 0; y < 64; y++ )
            {
                for ( x = 0; x < 64; x++ )
                {
                    color0 = pTile->m_pVoxelTile->m_Texture.GetRGBAPixel( x, y + 128, 0 );

                    if ( pTile->m_pVoxelTile->m_Texture.m_Header.dwNumLayers >= 2 )
                    {
                        color = pTile->m_pVoxelTile->m_Texture.GetRGBAPixel( x, y + 128, 1 );
                        color &= 0xff;
                        color = color >> pTile->m_pVoxelTile->m_dwHeightAmp;
                        color += pTile->m_dwTileHeight;					
                        color = (float)( color - m_dwMinHeight ) * fHeightRange;
                    }
                    else
                    {
                        color = (float)( pTile->m_dwTileHeight - m_dwMinHeight ) * fHeightRange;
                    }
                    if ( color > 255 ) color = 255;

                    color = ( color0 & 0x00ffffff ) | ( color << 24 );
                    
                    tga.SetRGBAPixel( lx + x, lHeight - ( ly + y ) - 1, color );
                }
            }
        }
    }

    tga.Dump( pTGAFileName );
}

//
// Well, it is not a flat voxel tile, lets calculate
// maximal and minimal heights along each of 4 edges
//
void COCLandscapeData::CalculateTileEdges
    ( 
    CMapTile *pMapTile
    )
{
    return;

    /*
    CBaseVoxelTile *m = pMapTile->m_pVoxelTile;

    int w;
    float fAmp = (float)( 32768 >> m->m_dwHeightAmp ) / 32768.f;
    float fH = fAmp * m->m_Texture.GetPixel( 0, 0, 1 );

    m->m_fMinEdgeHeight[ 0 ] = m->m_fMinEdgeHeight[ 1 ] = 
        m->m_fMinEdgeHeight[ 2 ] = m->m_fMinEdgeHeight[ 3 ] = 
        m->m_fMaxEdgeHeight[ 0 ] = m->m_fMaxEdgeHeight[ 1 ] = 
        m->m_fMaxEdgeHeight[ 2 ] = m->m_fMaxEdgeHeight[ 3 ] = fH;

    w = m->m_Texture.m_Header.dwWidth;
    for ( int y = 0; y < w; y++ )
    {
        for ( int x = 0; x < w; x++ )
        {
            if ( x && ( x < w - 1 ) &&
                 y && ( y < w - 1 ) )
                 continue;

            fH = fAmp * m->m_Texture.GetPixel( x, y, 1 );

            if ( x == 0 )
            {
                if ( m->m_fMinEdgeHeight[ 0 ] > fH ) m->m_fMinEdgeHeight[ 0 ] = fH;
                if ( m->m_fMaxEdgeHeight[ 0 ] < fH ) m->m_fMaxEdgeHeight[ 0 ] = fH;
            }

            if ( x == ( w - 1 ) )
            {
                if ( m->m_fMinEdgeHeight[ 2 ] > fH ) m->m_fMinEdgeHeight[ 2 ] = fH;
                if ( m->m_fMaxEdgeHeight[ 2 ] < fH ) m->m_fMaxEdgeHeight[ 2 ] = fH;
            }

            if ( y == 0 )
            {
                if ( m->m_fMinEdgeHeight[ 3 ] > fH ) m->m_fMinEdgeHeight[ 3 ] = fH;
                if ( m->m_fMaxEdgeHeight[ 3 ] < fH ) m->m_fMaxEdgeHeight[ 3 ] = fH;
            }

            if ( y == ( w - 1 ) )
            {
                if ( m->m_fMinEdgeHeight[ 1 ] > fH ) m->m_fMinEdgeHeight[ 1 ] = fH;
                if ( m->m_fMaxEdgeHeight[ 1 ] < fH ) m->m_fMaxEdgeHeight[ 1 ] = fH;
            }
        }
    }
    */
}

//
// Calculate maximal delta on edges for tile container
//
void COCLandscapeData::ClaculateEdgeDeltas
    (
    int nX,
    int nY
    )
{
    CMapTile *m = m_ppMapTiles[ nX + nY * m_pLndSize->dwWidth ];
    CMapTile *mt[ 4 ];
    int i, ii;
    float yc, yn, ydelta;

    mt[ 0 ] = mt[ 1 ] = mt[ 2 ] = mt[ 3 ] = NULL;

    if ( nX )
        mt[ 0 ] = m_ppMapTiles[ nX - 1 + nY * m_pLndSize->dwWidth ];

    if ( nX < ( (int)m_pLndSize->dwWidth - 1 ) )
        mt[ 2 ] = m_ppMapTiles[ nX + 1 + nY * m_pLndSize->dwWidth ];

    if ( nY )
        mt[ 3 ] = m_ppMapTiles[ nX + ( nY - 1 ) * m_pLndSize->dwWidth ];

    if ( nY < ( (int)m_pLndSize->dwHeight - 1 ) )
        mt[ 1 ] = m_ppMapTiles[ nX + ( nY + 1 ) * m_pLndSize->dwWidth ];

    for ( i = 0; i < 4; i++ )
    {
        if ( !mt[ i ] )
            continue;

        m->m_pVoxelTile->m_fEdgeDelta[ i ] = 10;
    }
}
