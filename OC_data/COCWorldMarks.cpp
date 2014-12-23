// This file is distributed under a MIT license. See LICENSE.txt for details.
//
// World marks class
//

#include "common.h"
#include "xfs.h"

extern "C"
{
extern  class XFileSystem       *xfs;
}

//
// Constructor
//
COCWorldMarks::COCWorldMarks
    (
    )
{
    m_pEMarks = NULL;
    m_pEntities = NULL;
    m_nNumEMarks = 0;
    m_nNumEntities = 0;
}

//
// Destructor
//
COCWorldMarks::~COCWorldMarks
    (
    )
{
    VDEL( m_pEMarks );
    VDEL( m_pEntities );
}

//
// Load data from memory
//
dword COCWorldMarks::LoadFromMemory
    ( 
    const void *l_pData 
    )
{
    int rec;
    TOCDecorRec *pRec;
    TOCMarkRec *pMark;

    m_pWRD = (byte *)l_pData;

    m_pHeader = (TOCLndHeader *)m_pWRD;
    m_pChunks = (TOCLHChunk *)( m_pHeader + 1 );

    m_pWrdProp = (TOCLndProp *)GetWRDPtr( m_pChunks[ 1 ].dwOffset );
    m_dwNumUnknown01 = *(dword *)( (byte *)m_pWrdProp + 4 + m_pWrdProp->dwChunkCount * sizeof( TOCLHChunk ) );
    m_dwNumDecorations = *(dword *)( (byte *)m_pWrdProp + 4 + m_pWrdProp->dwChunkCount * sizeof( TOCLHChunk ) + 4 );
    m_pStartRec = (TOCDecorRec *)( (byte *)m_pWrdProp + 4 + m_pWrdProp->dwChunkCount * sizeof( TOCLHChunk ) + 4 + 4 );

    m_pMrkProp = (TOCLndProp *)GetWRDPtr( m_pChunks[ 2 ].dwOffset );
    m_dwNumUnknown02 = *(dword *)( (byte *)m_pMrkProp + 4 + m_pMrkProp->dwChunkCount * sizeof( TOCLHChunk ) );
    m_dwNumMarks = *(dword *)( (byte *)m_pMrkProp + 4 + m_pMrkProp->dwChunkCount * sizeof( TOCLHChunk ) + 4 );
    m_pStartMark = (TOCMarkRec *)( (byte *)m_pMrkProp + 4 + m_pMrkProp->dwChunkCount * sizeof( TOCLHChunk ) + 4 + 4 );

    /*
    pRec = m_pStartRec;
    for ( rec = 0; rec < m_dwNumDecorations; rec++ )
    {
        AddDecoration( pRec );

        pRec = (TOCDecorRec *)GetWRDPtr( pRec->dwOffsetNextRec );
    }
    */

    m_pEMarks = NEW COCMark[ m_dwNumMarks ];
    m_nNumEMarks = 0;

    m_pEntities = NEW COCMarkEntity[ m_dwNumMarks ];
    m_nNumEntities = 0;

#ifdef OC_SUPPORT_ENTITIES
    // load object like entities
    pMark = m_pStartMark;
    for ( rec = 0; rec < m_dwNumMarks; rec++ )
    {
        {
            COCStr *m_pRecName;
            COCStr *m_pRecVal;
            XVec3 xvPos;
            TOCRotation *pRot;
            COCStr *m_pTypName;
            COCStr *m_pTypVal;

            m_pRecName = (COCStr *)GetWRDPtr( pMark->dwOffsetStr );
            m_pRecVal = REC_VAL_PTR( m_pRecName );

            xvPos = *(XVec3 *)GetWRDPtr( pMark->dwOffsetPosition );
            pRot = (TOCRotation *)GetWRDPtr( pMark->dwOffsetRotation );

            // add entity
            if ( m_pRecName->Cmp( "PMark" ) )
            {
                static char markname[ 256 ];

                m_pRecVal->CopyTo( markname );
                m_pEntities[ m_nNumEntities ].m_nOCMarkID = RegisterEMark( markname );
                m_pEntities[ m_nNumEntities ].m_Pos[ 0 ] = xvPos.x;
                m_pEntities[ m_nNumEntities ].m_Pos[ 1 ] = xvPos.y;
                m_pEntities[ m_nNumEntities ].m_Pos[ 2 ] = xvPos.z;
                m_pEntities[ m_nNumEntities ].m_Rot.sAngle1 = pRot->sAngle1;
                m_pEntities[ m_nNumEntities ].m_Rot.sAngle2 = pRot->sAngle2;
                m_pEntities[ m_nNumEntities ].m_Rot.sAngle3 = pRot->sAngle3;
                m_nNumEntities++;
            }
        }

        pMark = (TOCMarkRec *)GetWRDPtr( pMark->dwOffsetNextRec );
    }
#endif // OC_SUPPORT_ENTITIES
    return 0;
}

//
// Register entity mark
//
int COCWorldMarks::RegisterEMark
    (
    const char *pMarkName
    )
{
    static char filename[ 256 ];
    int i;

    strcpy( filename, "marks/" );
    strcat( filename, pMarkName );
    strcat( filename, ".mrk" );

    xfs->FixFileName( filename );

    for ( i = 0; i < m_nNumEMarks; i++ )
    {
        if ( !strcmp( m_pEMarks[ i ].m_pFileName, filename ) )
            return i;
    }

    strcpy( m_pEMarks[ m_nNumEMarks ].m_pFileName, filename );
    HXFSFile fp;

    fp = xfs->Open( filename );
    if ( fp )
    {
        void *data = (void *)ALLOCATE( xfs->SizeOf( fp ) );
        xfs->Read( fp, data, xfs->SizeOf( fp ) );
        xfs->Close( fp );

        m_pEMarks[ m_nNumEMarks ].LoadFromMemory( data );
    }
    else
        return -1;

    m_nNumEMarks++;
    return m_nNumEMarks - 1;
}

//
// Add decoration into the world
//
void COCWorldMarks::AddDecoration
    ( 
    TOCDecorRec *pRec 
    )
{
    COCStr *m_pRecName;
    COCStr *m_pRecVal;
    XVec3 xvPos;
    TOCRotation *pRot;
    COCStr *m_pTypName;
    COCStr *m_pTypVal;

    m_pRecName = (COCStr *)GetWRDPtr( pRec->dwOffsetRecName );
    m_pRecVal = REC_VAL_PTR( m_pRecName );

    xvPos = *(XVec3 *)GetWRDPtr( pRec->dwOffsetPosition );
    pRot = (TOCRotation *)GetWRDPtr( pRec->dwOffsetRotataion );

    m_pTypName = (COCStr *)GetWRDPtr( pRec->dwOffsetStr );
    m_pTypVal = REC_VAL_PTR( m_pTypName );
}

//
// Constructor
//
COCMark::COCMark
    (
    )
{
    m_pMarkData = NULL;
}

//
// Destructor
//
COCMark::~COCMark
    (
    )
{
    FREE( m_pMarkData );
}

//
// OCMark loading
//
dword COCMark::LoadFromMemory
    ( 
    const void *pData 
    )
{
    m_pMarkData = (void *)pData;

    m_pHeader = (TOCMainHeader *)pData;

    return 0;
}
