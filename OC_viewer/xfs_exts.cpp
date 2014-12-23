// This file is distributed under a MIT license. See LICENSE.txt for details.
//
// XFileSystem containers implementation
//

#include "common.h"
#include <io.h>

//
// Register container type
//
bool XFileSystem::RegisterContainer
    ( 
    XFSContainer *pContainer
    )
{
    m_pContainers[ m_nNumContainers++ ] = pContainer;

    DPrint( "XFileSystem::RegisterContainer : ext:\"" );
    DPrint( pContainer->m_pContainerExt );
    DPrint( "\", desc:\"" );
    DPrint( pContainer->m_pDescription );
    DPrint( "\" ...ok\n" );

    return true;
}

//
// Looking for file containers
//
void XFileSystem::ScaningCntFiles
    (
    )
{
    DPrint( "XFileSystem::ScaningCntFiles : start process\n" );

    int i;
    int handle;
    int ret = 0;
    struct _finddata_t fileinfo;
    const char *pExt;

    handle = _findfirst( "*.*", &fileinfo );
    if ( handle == -1 )
        ret = -1;

    while ( ret == 0 )
    {
        if ( !( fileinfo.attrib & _A_SUBDIR ) )
        {
            FixFileName( fileinfo.name );
            pExt = GetFileExtention( fileinfo.name );

            // find container type
            for ( i = 0; i < m_nNumContainers; i++ )
                if ( m_pContainers[ i ]->IsContainerExt( pExt ) )
                {
                    m_CntFiles[ m_nNumCntFiles ].m_nContainerID = i;
                    strcpy( m_CntFiles[ m_nNumCntFiles ].m_pFileName, fileinfo.name );

                    m_pContainers[ i ]->Initialize( &m_CntFiles[ m_nNumCntFiles ] );

                    m_nNumCntFiles++;
                    DPrint( ".found \"" ); DPrint( fileinfo.name ); DPrint( "\"\n" );
                    break;
                }
        }

        ret = _findnext( handle, &fileinfo );
        if ( ret != 0 )
        {
            _findclose( handle );
        }
    }
}

//
// Container constructor
//
XFSContainer::XFSContainer
    (
    )
{
    memset( m_pContainerExt, 0, sizeof( m_pContainerExt ) );
    memset( m_pInternalName, 0, sizeof( m_pInternalName ) );
    memset( m_pDescription, 0, sizeof( m_pDescription ) );
}

//
// Container destructor
//
XFSContainer::~XFSContainer
    (
    )
{
}

//
// Initialize
//
bool XFSContainer::Initialize
    ( 
    XFSCntFile *pc 
    )
{
    return false;
}

//
// Release
//
void XFSContainer::Release
    ( 
    XFSCntFile *pc 
    )
{
}

//
// Get number of files in 
//
int XFSContainer::GetNumFiles
    (
    const XFSCntFile *pc
    )
{
    return 0;
}

//
// Find file in given container
//
int XFSContainer::FindFile
    ( 
    const XFSCntFile *pc, 
    const char *pFileName
    )
{
    return 0;
}

//
// nFileID takes from FindFile
//
void *XFSContainer::ExtractFile
    ( 
    const XFSCntFile *pc, 
    int nFileID,
    int &nFileSize
    )
{
    return NULL;
}

//
// Cnt file constructor
//
XFSCntFile::XFSCntFile
    (
    )
{
    m_nContainerID = -1;
    memset( m_pFileName, 0, sizeof( m_pFileName ) );
    m_nNumFiles = 0;
    m_pData = NULL;
}
