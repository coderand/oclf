// This file is distributed under a MIT license. See LICENSE.txt for details.
//
// XFileSystem
//

#ifndef __XFS_H__
#define __XFS_H__

#include "stdio.h"

//
// General types and defines
//
#define XFS_MAX_OPENFILES   16
#define XFS_MAX_FILENAME    256
#define XFS_MAX_CONTTYPES   16
#define XFS_MAX_CONTAINERS  256

typedef int HXFSFile;

//
// Find and open files flags
//
enum
{
    EFFI_DISK               =   0x10000000,
    EFFI_CONTAINER          =   0x20000000,
    EFFI_BOTH               =   ( EFFI_DISK | EFFI_CONTAINER ),

    EFFI_DEFAULT            =   EFFI_BOTH,

    EFFI_FORCE_DWORD        =   0x7fffffff,
};

typedef struct
{
    int             m_nFileID; // internal ID in container
    int             m_nContainerID;
    int             m_nCntFileID;
} TXFSFindInfo;

//
// Extended file container like packs, etc
//
class XFSCntFile
{
public :

    int             m_nContainerID; // -1 - unused
    char            m_pFileName[ XFS_MAX_FILENAME ];
    int             m_nNumFiles;
    void            *m_pData;

                    XFSCntFile          ();
};

class XFSContainer
{
public :

    char            m_pContainerExt[ XFS_MAX_FILENAME ]; // extention of container file
    char            m_pInternalName[ XFS_MAX_FILENAME ]; // internal representation
    char            m_pDescription[ XFS_MAX_FILENAME ];  // description of a container

    virtual bool    Initialize          ( XFSCntFile *pc );
    virtual void    Release             ( XFSCntFile *pc );
    virtual int     GetNumFiles         ( const XFSCntFile *pc );
    virtual int     FindFile            ( const XFSCntFile *pc, const char *pFileName );
    virtual void    *ExtractFile        ( const XFSCntFile *pc, int nFileID, int &nFileSize ); // nFileID takes from FindFile
    virtual bool    IsContainerExt      ( const char * pExt ) { return !strcmp(m_pContainerExt, pExt); }

                    XFSContainer        ();
    virtual         ~XFSContainer       ();
};

//
// Common xfs file
//
class XFSFile
{
public :
    void            *m_pData;
    FILE            *m_fp;
    int             m_nContainerType; // 0 = disk

    bool            m_bOpened;
    char            m_pFileName[ XFS_MAX_FILENAME ];
    int             m_nPos;
    int             m_nFileSize;
};

//
// Main file system class
//
class XFileSystem
{
private :

    XFSFile         m_Files[ XFS_MAX_OPENFILES ];

    int             m_nNumContainers;
    XFSContainer    *m_pContainers[ XFS_MAX_CONTTYPES ];

    int             m_nNumCntFiles;
    XFSCntFile      m_CntFiles[ XFS_MAX_CONTAINERS ];

public :

    //
    // External tools
    //
    static void     CutFileName         ( char *pFullFilename );
    static void     FixFileName         ( char *pFileName );
    static const char*GetFileExtention  ( const char *pFileName );

    //
    // Register methods
    //
    bool            RegisterContainer   ( XFSContainer *pContainer );
    void            ScaningCntFiles     ();

    //
    // Common operation with files
    //

    HXFSFile        Open                ( const char *pFileName );
    int             Find                ( const char *pFileName, unsigned int nFindIn = EFFI_DEFAULT, TXFSFindInfo *pInfo = NULL );
    void            Close               ( HXFSFile hFile );
    int             Read                ( HXFSFile hFile, void *pDest, int nLength );
    int             SizeOf              ( HXFSFile hFile );

    bool            Initialize          ();

                    ~XFileSystem        ();
};

#endif // __XFS_H__
