// This file is distributed under a MIT license. See LICENSE.txt for details.
//
// Debugging code
//

#include <d3d9.h>
#include <d3dx9.h>
#include <ddraw.h>
#include <windows.h>
#include "CRenderer.h"
#include "common.h"
#include <conio.h>
#include "xmath.h"

#ifdef SQUISH_DEBUG

void DInit
    (
    )
{
    FILE *fp = fopen( "OC_viewer.log", "w" );
    
    if ( !fp )
        return;
    
    fclose( fp );

    DPrint( ".Start OC_viewer\n" );
}

void DPrint
    (
    const char *pStr,
    ...
    )
{
    va_list arg;
    FILE *fp;

    va_start( arg, pStr );

    fp = fopen( "OC_viewer.log", "a+" );
    if ( !fp )
        return;

    vfprintf( fp, pStr, arg );
    va_end( arg );

    fclose( fp );
}

// debug macros
void PrintDXError
    ( 
    HRESULT hr,
    const char *pCallerLine
    )
{
    char text[ 256 ];

    if ( FAILED( hr ) )
    {
        DPrint( text );
        DPrint( ", in line : " );
        DPrint( pCallerLine );
        DPrint( "\n" );
    }
}

#endif // SQUISH_DEBUG
