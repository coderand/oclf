// This file is distributed under a MIT license. See LICENSE.txt for details.
//
// XFileSystem implementation
//

#include "common.h"

//
// Initialize file system
//
bool XFileSystem::Initialize
    (
    )
{
    DPrint( ".Initialize file system ..." );

    memset( m_Files, 0, sizeof ( m_Files ) );

    m_nNumContainers = 0;
    memset( m_pContainers, 0, sizeof( m_pContainers ) );
    m_nNumCntFiles = 0;
    memset( m_CntFiles, 0, sizeof( m_CntFiles ) );

    DPrint( "ok\n" );

    return true;
}

//
// Destructor
//
XFileSystem::~XFileSystem
    (
    )
{
    int i;

    for ( i = 0; i < m_nNumCntFiles; i++ )
        m_pContainers[ m_CntFiles[ i ].m_nContainerID ]->Release( &m_CntFiles[ i ] );
}

//
// Find given file
//
int XFileSystem::Find
    ( 
    const char *l_pFileName,
    unsigned int nFindIn,
    TXFSFindInfo *pInfo
    )
{
    static char pFileName[ XFS_MAX_FILENAME ];
    int i;

    strcpy( pFileName, l_pFileName );
    FixFileName( pFileName );

    // find in containers at first
    if ( nFindIn & EFFI_CONTAINER )
    {
        for ( i = 0; i < m_nNumCntFiles; i++ )
        {
            XFSContainer *c = m_pContainers[ m_CntFiles[ i ].m_nContainerID ];

            int handle = c->FindFile( &m_CntFiles[ i ], pFileName );

            if ( handle )
            {
                if ( pInfo )
                {
                    pInfo->m_nCntFileID = i;
                    pInfo->m_nContainerID = m_CntFiles[ i ].m_nContainerID;
                    pInfo->m_nFileID = handle;
                }

                return EFFI_CONTAINER;
            }
        }
    }

    // and that on disk
    if ( nFindIn & EFFI_DISK )
    {
        FILE *fp = fopen( pFileName, "rb" );

        if ( fp )
        {
            fclose( fp );
            return EFFI_DISK;
        }
    }

    return 0;
}

//
// Open a file
//
HXFSFile XFileSystem::Open
    ( 
    const char *l_pFileName
    )
{
    static char pFileName[ XFS_MAX_FILENAME ];
    HXFSFile hCurr = 0;
    FILE *fp;
    int i;

    strcpy( pFileName, l_pFileName );
    FixFileName( pFileName );

    for ( i = 0; i < XFS_MAX_OPENFILES; i++ )
    {
        if ( !m_Files[ i ].m_bOpened )
        {

            // open from pack at first
            {
                TXFSFindInfo info;

                if ( Find( pFileName, EFFI_CONTAINER, &info ) )
                {
                    XFSContainer *c = m_pContainers[ info.m_nContainerID ];
                    int size;

                    m_Files[ i ].m_pData = c->ExtractFile( &m_CntFiles[ info.m_nCntFileID ], info.m_nFileID, size );
                    m_Files[ i ].m_fp = NULL;
                    m_Files[ i ].m_nContainerType = 1;
                    m_Files[ i ].m_bOpened = true;
                    strcpy( m_Files[ i ].m_pFileName, pFileName );
                    m_Files[ i ].m_nPos = 0;
                    m_Files[ i ].m_nFileSize = size;

                    hCurr = i + 1;
                    break;
                }
            }

            // let's open it from disk
            fp = fopen( pFileName, "rb" );
            if ( fp )
            {
                m_Files[ i ].m_pData = NULL;
                m_Files[ i ].m_fp = fp;
                m_Files[ i ].m_nContainerType = 0;
                m_Files[ i ].m_bOpened = true;
                strcpy( m_Files[ i ].m_pFileName, pFileName );
                m_Files[ i ].m_nPos = 0;
                fseek( fp, 0, SEEK_END );
                m_Files[ i ].m_nFileSize = ftell( fp );
                fseek( fp, 0, SEEK_SET );

                hCurr = i + 1;
                break;
            }
        }
    }

    if ( !hCurr )
    {
        DPrint( "ERR : file not found :\"" );
        DPrint( pFileName );
        DPrint( "\"\n" );
    }

    return hCurr;
}

//
// Close a file
//
void XFileSystem::Close
    ( 
    HXFSFile hFile 
    )
{
    if ( hFile && hFile <= XFS_MAX_OPENFILES && m_Files[ hFile - 1 ].m_bOpened )
    {
        if ( !m_Files[ hFile - 1 ].m_nContainerType )
            fclose( m_Files[ hFile - 1 ].m_fp );

        if ( m_Files[ hFile - 1 ].m_pData )
            FREE( m_Files[ hFile - 1 ].m_pData );

        m_Files[ hFile - 1 ].m_bOpened = false;
    }
}

//
// Read data from file
//
int XFileSystem::Read
    ( 
    HXFSFile hFile, 
    void *pDest, 
    int nLength 
    )
{
    int nReal = 0;

    if ( hFile && hFile <= XFS_MAX_OPENFILES && m_Files[ hFile - 1 ].m_bOpened )
    {
        if ( !m_Files[ hFile - 1 ].m_nContainerType )
            nReal = fread( pDest, 1, nLength, m_Files[ hFile - 1 ].m_fp );
        else
        {
            nReal = m_Files[ hFile - 1 ].m_nPos + nLength <= m_Files[ hFile - 1 ].m_nFileSize ? nLength : m_Files[ hFile - 1 ].m_nFileSize - m_Files[ hFile - 1 ].m_nPos;
            memcpy( pDest, (char *)m_Files[ hFile - 1 ].m_pData + m_Files[ hFile - 1 ].m_nPos, nReal );
        }

        m_Files[ hFile - 1 ].m_nPos += nReal;
    }

    return nReal;
}

//
// Get size of file
//
int XFileSystem::SizeOf
    ( 
    HXFSFile hFile 
    )
{
    if ( hFile && hFile <= XFS_MAX_OPENFILES && m_Files[ hFile - 1 ].m_bOpened )
        return m_Files[ hFile - 1 ].m_nFileSize;
    else
        return 0;
}

//
// Cut name of file from full file name
//
void XFileSystem::CutFileName
    ( 
    char *pFullFilename
    )
{
    int pos = strlen( pFullFilename ) - 1;

    while ( pos >= 0 )
    {
        if ( pFullFilename[ pos ] == '/' || pFullFilename[ pos ] == '\\' )
        {
            pFullFilename[ pos + 1 ] = 0;
            return;
        }

        pos--;
    }
}

//
// Fix name of file
//
void XFileSystem::FixFileName
    ( 
    char *pFileName
    )
{
    int pos = 0;
    int pos2 = 0;
    char lastChar = '\0';

    while ( pFileName[ pos ] != '\0' )
    {
        if ( pFileName[ pos ] == '\\' )
            pFileName[ pos ] = '/';

        if ( pFileName[ pos ] == lastChar && lastChar == '/' )
        {
            pos++;
            continue;
        }

        lastChar = pFileName[ pos ];
        pFileName[ pos ] = tolower( pFileName[ pos ] );
        pFileName[ pos2++ ] = pFileName[ pos++ ];
    }
    pFileName[ pos2 ] = '\0';
}

//
// Get pointer to file extention in a given string
//
const char *XFileSystem::GetFileExtention
    ( 
    const char *pFileName 
    )
{
    int pos = strlen( pFileName ) - 1;
    int end = pos;

    while ( pos >= 0 )
    {
        if ( pFileName[ pos ] == '.' )
            return &pFileName[ pos + 1 ];

        pos--;
    }

    return &pFileName[ end ];
}
