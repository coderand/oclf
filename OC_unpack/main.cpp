// This file is distributed under a MIT license. See LICENSE.txt for details.
//
// Outcast PAK unpacker
//

#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <process.h>
#include <dos.h>
#include <io.h>

extern "C"
{
    extern char p_tool1;
    extern char p_tool2;
    extern char p_tool3;
};

void Unpack
    (
    void *ptr,
    int size,
    const char *pName
    )
{
    FILE *fp;

    fp = fopen( pName, "wb" );

    fwrite( ptr, 1, size, fp );

    fclose( fp );
}

void main
    (
    int argv, 
    char *argc[] 
    )
{
    char pCommandU[ 256 ];
    char pFileName[ 256 ];
    char pDir[ 256 ];

    printf( "\n" );
    printf( "#\n" );
    printf( "# OUTCAST Unpacker by AND 2002\n" );
    printf( "#\n" );
    if ( argv <= 1 )
    {
        printf( "# USAGE : oc_unpack.exe filename.pak\n" );
        return;
    }

    strcpy( pFileName, argc[ 1 ] );
    strcpy( pDir, pFileName );
    strcat( pDir, ".dir" );

    CreateDirectory( pDir, 0 );
    SetCurrentDirectory( pDir );

    Unpack( &p_tool1, &p_tool2 - &p_tool1, "tool1.exe" );
    Unpack( &p_tool2, &p_tool3 - &p_tool2, "tool2.exe" );

    sprintf( pCommandU, "copy ..\\%s %s", pFileName, pFileName );
    system( pCommandU );

    sprintf( pCommandU, "tool1.exe -e %s", pFileName );
    system( pCommandU );
    system( "del tool1.exe" );

    int rc;
    int ret = 0;
    struct _finddata_t fileinfo;

    rc = _findfirst( "*.zip", &fileinfo );
    
    if ( rc == -1 )
        ret = -1;

    while ( ret == 0 )
    {
        sprintf( pCommandU, "tool2.exe -extract %s", fileinfo.name );
        system( pCommandU );
        sprintf( pCommandU, "del %s", fileinfo.name );
        system( pCommandU );

        ret = _findnext( rc, &fileinfo );
        if ( ret != 0 )
        {
            _findclose( rc );
        }
    }

    system( "del tool2.exe" );
    sprintf( pCommandU, "del %s", pFileName );
    system( pCommandU );
}
