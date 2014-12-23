// This file is distributed under a MIT license. See LICENSE.txt for details.
//
// FNT file
//

#include "common.h"

//
// Load FNT file from memory
//
dword COCFont::LoadFromMemory
    ( 
    const void *pData 
    )
{
    TOCFNTRecord *pRec;
    COCStr  *pRecName;
    COCStr  *pRecVal;
    int i;

    m_pFNT = (byte *)pData;

    m_pHeader = (TOCMainHeader *)m_pFNT;
    m_pFNTHeader = (TOCFNTHeader *)GetFNTPtr( m_pHeader->pChunks[ 0 ].dwOffset );

    m_pStartRec = (TOCFNTRecord *)( m_pFNTHeader + 1 );
    m_pChars = NEW OCFNTChar[ 256 ];

    pRec = m_pStartRec;
    for ( i = 0; i < m_pFNTHeader->dwNumRecords; i++ )
    {
        pRecName = (COCStr *)GetFNTPtr( pRec->dwOffsetRecName );
        pRecVal = REC_VAL_PTR( pRecName );
        pRecVal->CopyTo( m_pChars[ i ].m_pBitmapName );
        m_pChars[ i ].m_nLength = *(int *)GetFNTPtr( pRec->dwOffsetCharLength );
        m_pChars[ i ].m_pBitmap = NULL;

        if ( m_pChars[ i ].m_pBitmapName[ 0 ] )
        {
            m_pChars[ i ].m_pBitmap = NEW CRBTSurface;
            
            strcat( m_pChars[ i ].m_pBitmapName, ".rbt" );
            FILE *fp = fopen( m_pChars[ i ].m_pBitmapName, "rb" );
            fseek( fp, 0, SEEK_END );
            int size = ftell( fp );
            fseek( fp, 0, SEEK_SET );
            byte *data = (byte *)malloc( size );
            fread( data, 1, size, fp );
            fclose( fp );
            
            m_pChars[ i ].m_pBitmap->LoadFromMemory( data );
            m_pChars[ i ].m_pBitmap->Unpack();
        }

        pRec = (TOCFNTRecord *)GetFNTPtr( pRec->dwOffsetNextRec );
    }

    return 0;
}
