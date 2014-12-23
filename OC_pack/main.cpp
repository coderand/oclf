// This file is distributed under a MIT license. See LICENSE.txt for details.
//
// OC Packer
//

#include <windows.h>
#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string.h>

#define OC_APPEAL_SIGNATURE 0x00006E71
#define OC_FILEID_PAK       0x097DD4D3

typedef unsigned int dword;

#pragma pack( 1 )

typedef struct
{
    dword       dwSignature;
    dword       dwTypeID;
    dword       dwMajVer;
    dword       dwMinVer;

    int         dwNumFiles;
} TOCPAKHeader;

typedef struct
{
    dword       dwOffsetData;
    dword       dwPackedSize;
    dword       dwNormalSize;
} TOCPAKFileInfo;

#pragma pack()

char g_pSearchMask[ 1024 ];

void main
    (
    int argv,
    char * argc[]
    )
{
    printf( "\n" );
    printf( "#\n" );
    printf( "# OUTCAST PAK Builder by AND 2005\n" );
    printf( "#\n" );
    if ( argv <= 1 )
    {
        printf( "# USAGE : oc_pack.exe directoryname\n" );
        return;
    }

    int handle;
    struct _finddata_t fileinfo;
    int ret;
    int i;
    int offset = sizeof( TOCPAKHeader );

    std::vector< _finddata_t > files;

    printf( "\n" );

    sprintf( g_pSearchMask, "%s/*.*", argc[ 1 ] );

    handle = _findfirst( g_pSearchMask, &fileinfo );
    ret = 0;
    if ( handle == -1 )
        ret = -1;

    while ( ret == 0 )
    {
        if ( !( fileinfo.attrib & _A_SUBDIR ) )
        {
            files.push_back( fileinfo );

            offset += 4 + sizeof( TOCPAKFileInfo );
            offset += strlen( fileinfo.name );
        }

        ret = _findnext( handle, &fileinfo );
        if ( ret != 0 )
            _findclose( handle );
    }

    char str[ 256 ];
    strcpy( str, argc[ 1 ] );
    strcat( str, ".pak" );
    FILE * fp = fopen( str, "wb" );

    TOCPAKHeader header;

    header.dwSignature = OC_APPEAL_SIGNATURE;
    header.dwTypeID = OC_FILEID_PAK;
    header.dwNumFiles = files.size();
    header.dwMajVer = 1;
    header.dwMinVer = 3; // unpacked data

    fwrite( &header, 1, sizeof( header ), fp );

    for ( i = 0; i < files.size(); i++ )
    {
        int len = strlen( files.at( i ).name );

        fwrite( &len, 1, 4, fp );
        fwrite( files.at( i ).name, 1, len, fp );

        TOCPAKFileInfo fi;
        fi.dwOffsetData = offset;
        fi.dwNormalSize = files.at( i ).size;
        fi.dwPackedSize = fi.dwNormalSize;

        fwrite( &fi, 1, sizeof( fi ), fp );

        offset += fi.dwNormalSize;
    }

    for ( i = 0; i < files.size(); i++ )
    {
        printf( "pack : %s\n", files.at( i ).name );

        sprintf( str, "%s/%s", argc[ 1 ], files.at( i ).name );
        FILE * in = fopen( str, "rb" );
        void * data = malloc( files.at( i ).size );
        fread( data, 1, files.at( i ).size, in );
        fwrite( data, 1, files.at( i ).size, fp );
        free( data );
        fclose( in );
    }

    fclose( fp );

    printf( "\nDone\n\n" );
}
