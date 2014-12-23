// This file is distributed under a MIT license. See LICENSE.txt for details.

#include "common.h"
#include <math.h>
#include <io.h>

XFileSystem *xfs;
void DPrint(const char *pStr, ... ) {}

void SurfaceProperties
    (
    CBTMSurface &btm,
    CTGASurface &tga
    )
{
    int x, y;
    int h[ 9 ], r;
    dword color;
    int t = 0;

    for ( y = 1; y < 127; y++ )
    {
        for ( x = 1; x < 127; x++ )
        {
            h[ 0 ] = btm.GetPixel( x - 1, y - 1, 1 );
            h[ 1 ] = btm.GetPixel( x + 0, y - 1, 1 );
            h[ 2 ] = btm.GetPixel( x + 1, y - 1, 1 );

            h[ 3 ] = btm.GetPixel( x - 1, y + 0, 1 );
            h[ 4 ] = btm.GetPixel( x + 0, y + 0, 1 );
            h[ 5 ] = btm.GetPixel( x + 1, y + 0, 1 );

            h[ 6 ] = btm.GetPixel( x - 1, y + 1, 1 );
            h[ 7 ] = btm.GetPixel( x + 0, y + 1, 1 );
            h[ 8 ] = btm.GetPixel( x + 1, y + 1, 1 );

            r = -40 * h[ 4 ]
                +10 * h[ 1 ]
                +10 * h[ 3 ]
                +10 * h[ 5 ]
                +10 * h[ 7 ];
            if ( r < 0 ) r = 0;
            if ( r > 255 ) r = 255;

            r = 255 - r;

            color = r | ( r << 8 ) | ( r << 16 );

            tga.SetRGBAPixel( x, 127 - y, color );
        }
    }

    printf( "numFT = %d\n", t );
}

bool ConvertFile
    (
    const char chFileName[]
    )
{
    FILE *fp;
    byte *pFile, *pFileTmp;
    int nFileSize;
    char chOutName[ 256 ];

    fp = fopen( chFileName, "rb" );
    if ( !fp )
    {
        printf( "# I/O error\n" );
        return false;
    }

    fseek( fp, 0, SEEK_END );
    nFileSize = ftell( fp );
    fseek( fp, 0, SEEK_SET );
    pFile = (byte *)malloc( nFileSize );
    fread( pFile, 1, nFileSize, fp );
    fclose( fp );

    // start converting
    CBTMSurface btm;
    pFileTmp = pFile;

    pFile += btm.LoadFromMemory( pFile );
    free( pFileTmp );

    strcpy( chOutName, chFileName );
    strcat( chOutName, ".tga" );
    btm.DumpLayer( chOutName );

    // save height map, if we have it
    if ( btm.m_Header.dwNumLayers > 1 )
    {
        strcpy( chOutName, chFileName );
        strcat( chOutName, "_h.tga" );
        btm.DumpLayer( chOutName, 1 );
    }

    return true;
}

bool ConvertRBTFile
    (
    const char chFileName[]
    )
{
    FILE *fp;
    byte *pFile, *pFileTmp;
    int nFileSize;
    char chOutName[ 256 ];

    fp = fopen( chFileName, "rb" );
    if ( !fp )
    {
        printf( "# I/O error\n" );
        return false;
    }

    fseek( fp, 0, SEEK_END );
    nFileSize = ftell( fp );
    fseek( fp, 0, SEEK_SET );
    pFile = (byte *)malloc( nFileSize );
    fread( pFile, 1, nFileSize, fp );
    fclose( fp );

    // start converting
    CRBTSurface rbt;
    pFileTmp = pFile;

    pFile += rbt.LoadFromMemory( pFile );
    //free( pFileTmp );

    strcpy( chOutName, chFileName );
    strcat( chOutName, ".tga" );
    rbt.DumpLayer( chOutName );

    return true;
}

const char *GetFileExtention
    (
    const char *pFileName
    )
{
    int pos = strlen( pFileName ) - 1;

    while ( pos >= 0 )
    {
        if ( pFileName[ pos ] == '.' )
            return pFileName + pos;

        pos--;
    }

    return pFileName;
}

void main
    (
    int argv,
    char *argc[]
    )
{
    char chFileName[ 256 ];
    char chFileMask[ 256 ];
    const char *pFileExt;

    printf( "\n" );
    printf( "#\n" );
    printf( "# OUTCAST Data ripper by AND 2002. Build ( %s )\n", __DATE__ );
    printf( "# web : www.and.intercon.ru\n" );
    printf( "#\n" );
    if ( argv <= 1 )
    {
        printf( "# USAGE : oc_data.exe <filename.lnd>\n" );
        printf( "#         oc_data.exe <mask.btm>\n" );
        printf( "#         oc_data.exe <mask.rbt>\n" );
        return;
    }

    strcpy( chFileMask, argc[ 1 ] );
    strlwr( chFileMask );
    pFileExt = GetFileExtention( chFileMask );

    if ( !strcmp( pFileExt, ".lnd" ) )
    {
        COCLandscapeData lnd;

        lnd.m_pBasePath = 0;

        FILE *fp = fopen( chFileMask, "rb" );
        fseek( fp, 0, SEEK_END );
        int size = ftell( fp );
        fseek( fp, 0, SEEK_SET );
        byte *data = (byte *)malloc( size );
        fread( data, 1, size, fp );
        fclose( fp );

        lnd.LoadFromMemory( data );
        //lnd.Dump( "map.tga" );
        char str[ 256 ];
        strcpy( str, chFileMask );
        strcat( str, ".tga" );

        printf( "convert : %s\n", chFileMask );
        lnd.Dump( str );
        printf( "ok... %s consist image of map in RGB and height of map in Alpha components\n", str );
        //free( data );

        return;
    }

    int handle;
    int ret = 0;
    struct _finddata_t fileinfo;

    handle = _findfirst( chFileMask, &fileinfo );
    if ( handle == -1 )
        ret = -1;

    while ( ret == 0 )
    {

        strlwr( fileinfo.name );
        pFileExt = GetFileExtention( fileinfo.name );
        if ( !strcmp( pFileExt, ".btm" ) )
        {
            printf( "convert : %s\n", fileinfo.name );
            ConvertFile( fileinfo.name );
        }
        else
        if ( !strcmp( pFileExt, ".rbt" ) )
        {
            printf( "convert : %s\n", fileinfo.name );
            ConvertRBTFile( fileinfo.name );
        }

        ret = _findnext( handle, &fileinfo );
        if ( ret != 0 )
        {
            _findclose( handle );
        }
    }
}
