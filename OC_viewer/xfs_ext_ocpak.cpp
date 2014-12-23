// This file is distributed under a MIT license. See LICENSE.txt for details.
//
// XFileSystem extention ( Outcast's pak file )
//

#include "common.h"
#include "DCLib.h"

class XFS_OCPak : public XFSContainer
{
public :

    virtual bool    Initialize          ( XFSCntFile *pc );
    virtual void    Release             ( XFSCntFile *pc );
    virtual int     GetNumFiles         ( const XFSCntFile *pc );
    virtual int     FindFile            ( const XFSCntFile *pc, const char *pFileName );
    virtual void    *ExtractFile        ( const XFSCntFile *pc, int nFileID, int &nFileSize ); // nFileID takes from FindFile

    virtual bool    IsContainerExt      ( const char * pExt )
    {
        return !strcmp(m_pContainerExt, pExt)
               || !strcmp("opk", pExt); // for Outcast 1.1 convenience
    }

    XFS_OCPak
        (
        )
    {
        strcpy( m_pContainerExt, "pak" );
        strcpy( m_pInternalName, "%s/" );
        strcpy( m_pDescription, "Outcast's PAK file" );
    }

} ocpak;

typedef struct
{
    char            pFileName[ XFS_MAX_FILENAME - 4 ];
    int             offset;
    TOCPAKFileInfo  info;
} TOCPKRec;

void XFS_EXT_Register_OCPak
    (
    )
{
    xfs->RegisterContainer( &ocpak );
}

//
// Release container
//
void XFS_OCPak::Release
    (
    XFSCntFile *pc
    )
{
    VDEL( pc->m_pData );
}

//
// Initialize container
//
bool XFS_OCPak::Initialize
    ( 
    XFSCntFile *pc 
    )
{
    int i;
    FILE *fp;
    int fnsize;
    static char filename[ XFS_MAX_FILENAME ];
    static char pFullNamePrefix[ XFS_MAX_FILENAME ];
    static char pFullName[ XFS_MAX_FILENAME ];
    TOCPAKHeader header;
    TOCPAKFileInfo info;
    TOCPKRec *pRec;

    fp = fopen( pc->m_pFileName, "rb" );
    if ( !fp )
        return 0;

    strcpy( pFullName, pc->m_pFileName );
    char * ext = (char *)xfs->GetFileExtention( pFullName );
    if ( ext > pFullName )
        *( ext - 1 ) = 0;

    sprintf( pFullNamePrefix, m_pInternalName, pFullName );
    fread( &header, 1, sizeof( header ), fp );
    if ( header.dwSignature != OC_APPEAL_SIGNATURE || header.dwTypeID != OC_FILEID_PAK )
    {
        fclose( fp );
        return 0;
    }

    pc->m_nNumFiles = header.dwNumFiles;
    pRec = NEW TOCPKRec[ pc->m_nNumFiles ];
    pc->m_pData = (TOCPKRec *)pRec;

    for ( i = 0; i < header.dwNumFiles; i++ )
    {
        fread( &fnsize, 1, 4, fp );
        fread( filename, 1, fnsize, fp );
        filename[ fnsize ] = 0;
        strcpy( pFullName, pFullNamePrefix );
        strcat( pFullName, filename );
        xfs->FixFileName( pFullName );

        fread( &info, 1, sizeof( info ), fp );

        strcpy( pRec[ i ].pFileName, pFullName );
        pRec[ i ].offset = ftell( fp ) - fnsize - 4;
        pRec[ i ].info = info;
    }

    fclose( fp );
    return true;
}

int XFS_OCPak::GetNumFiles
    ( 
    const XFSCntFile *pc
    )
{
    return pc->m_nNumFiles;
}

int XFS_OCPak::FindFile
    ( 
    const XFSCntFile *pc, 
    const char *pFileName
    )
{
    int i;

    for ( i = 0; i < pc->m_nNumFiles; i++ )
    {
        TOCPKRec *rc = (TOCPKRec *)pc->m_pData + i;

        if ( !strcmp( pFileName, rc->pFileName ) )
            return i + 1;
    }

    return 0;
}

void *XFS_OCPak::ExtractFile
    ( 
    const XFSCntFile *pc, 
    int nFileID,
    int &nFileSize
    )
{
    FILE *fp;
    int fnsize;
    static char filename[ XFS_MAX_FILENAME ];
    TOCPKRec *rec = (TOCPKRec *)pc->m_pData + nFileID - 1;
    void *psrc, *pdest;

    fp = fopen( pc->m_pFileName, "rb" );
    fseek( fp, rec->info.dwOffsetData, SEEK_SET );

    psrc = (void *)ALLOCATE( rec->info.dwPackedSize );
    fread( psrc, 1, rec->info.dwPackedSize, fp );
    fclose( fp );

    if ( rec->info.dwPackedSize == rec->info.dwNormalSize )
    {
        nFileSize = rec->info.dwNormalSize;
        return psrc;
    }

    pdest = (void *)ALLOCATE( rec->info.dwNormalSize );
    memset( pdest, 0, rec->info.dwNormalSize );

    DCLUnpack( pdest, psrc, (unsigned int *)&fnsize, rec->info.dwPackedSize );
    nFileSize = fnsize;

    assert( fnsize == rec->info.dwNormalSize );

    FREE( psrc );

    return pdest;
}
