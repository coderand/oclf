// This file is distributed under a MIT license. See LICENSE.txt for details.
//
// LUX font generator
//

#include <windows.h>
#include <stdio.h>
#include <math.h>
#include "ocdata.h"

typedef struct
{
    float   fU, fV; // left-top corner
    float   fU2, fV2; // right-down corner
    float   fDx;
    float   fDy;
} TLUXChar;

int     g_nTexWidth = 512;
int     g_nTexHeight = 256;
dword   *g_pTexture;
TLUXChar g_pLUX[ 256 ];

char    pFileName[ 256 ];

//
// Convert OUTCAST font into LUX font
//
void ConvertFromOCFNT
    (
    )
{
    FILE *fp;
    int size;
    byte *data;
    COCFont fnt;

    fp = fopen( "sf.fnt", "rb" );
    fseek( fp, 0, SEEK_END );
    size = ftell( fp );
    fseek( fp, 0, SEEK_SET );
    data = (byte *)malloc( size );
    fread( data, 1, size, fp );
    fclose( fp );

    fnt.LoadFromMemory( data );

    // map it onto texture
    int x, y;
    int tx = 1, ty = 1;
    int idx;
    int min = 10000, max = -10000;
    int minh = 10000, maxh = -10000;
    int miny = 10000, maxy = -10000;
    int height;

    for ( idx = 0; idx < 256; idx++ )
    {
        if ( fnt.m_pChars[ idx ].m_pBitmap )
        {
            int x = fnt.m_pChars[ idx ].m_pBitmap->m_pHeader->dwWidth;
            int y = fnt.m_pChars[ idx ].m_pBitmap->m_pHeader->dwHeight;
            int h = fnt.m_pChars[ idx ].m_nLength;

            if ( -h < miny ) miny = -h;
            if ( y - h > maxy ) maxy = y - h;

            if ( h > maxh ) maxh = h;
        }
    }

    height = maxy - miny;
    
    for ( idx = 0; idx < 256; idx++ )
    {
        if ( fnt.m_pChars[ idx ].m_pBitmap && ( tx + fnt.m_pChars[ idx ].m_pBitmap->m_pHeader->dwWidth >= g_nTexWidth ) )
        {
            tx = 1;
            ty += height + 1;
        }

        if ( fnt.m_pChars[ idx ].m_pBitmap )
        {
            int sy = maxh - fnt.m_pChars[ idx ].m_nLength;

            for ( y = 0; y < fnt.m_pChars[ idx ].m_pBitmap->m_pHeader->dwHeight; y++ )
                for ( x = 0; x < fnt.m_pChars[ idx ].m_pBitmap->m_pHeader->dwWidth; x++ )
                {
                    dword color = fnt.m_pChars[ idx ].m_pBitmap->GetPixel( x, y );
                    g_pTexture[ x + tx + g_nTexWidth * ( sy + y + ty ) ] = ( color << 24 ) | 0x00ffffff;
                }

            g_pLUX[ idx ].fU = tx - 0.5f;
            g_pLUX[ idx ].fV = ty - 0.5f;
            g_pLUX[ idx ].fU2 = 0.5f + tx + fnt.m_pChars[ idx ].m_pBitmap->m_pHeader->dwWidth;
            g_pLUX[ idx ].fV2 = 0.5f + ty + height;
            g_pLUX[ idx ].fDy = height;
            g_pLUX[ idx ].fDx = fnt.m_pChars[ idx ].m_pBitmap->m_pHeader->dwWidth;

            tx += 1 + fnt.m_pChars[ idx ].m_pBitmap->m_pHeader->dwWidth;
        }
    }

    // hack
    g_pLUX[ ' ' ].fDx = g_pLUX[ '_' ].fDx * 0.75f;

    ty += height + 1;
    float po = ty;

    po = log( po ) / log( 2 );
    po = 1 + (int)po;

    g_nTexHeight = 1 << (int)po;

    for ( idx = 0; idx < 256; idx++ )
    {
        g_pLUX[ idx ].fU /= g_nTexWidth;
        g_pLUX[ idx ].fV /= g_nTexHeight;
        g_pLUX[ idx ].fU2 /= g_nTexWidth;
        g_pLUX[ idx ].fV2 /= g_nTexHeight;
    }
}

//
// Main procedure
//
int __stdcall WinMain
    ( 
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nShowCmd
    )
{
    char *pExt;
    strcpy( pFileName, "sf.fnt" );
    pExt = pFileName + strlen( pFileName ) - 3;

    g_pTexture = (dword *)new dword[ g_nTexWidth * g_nTexHeight ];
    for ( int i = 0; i < g_nTexWidth * g_nTexHeight; i++ )
        g_pTexture[ i ] = 0x00ffffff;
    memset( g_pLUX, 0, sizeof( g_pLUX ) );

    ConvertFromOCFNT();

    // save tga
    CTGASurface tga;
    int x, y;
    tga.Create( g_nTexWidth, g_nTexHeight );
    for ( y = 0; y < g_nTexHeight; y++ )
        for ( x = 0; x < g_nTexWidth; x++ )
            tga.SetRGBAPixel( x, g_nTexHeight - 1 - y, g_pTexture[ x + g_nTexWidth * y ] );
    
    pExt[ 0 ] = 't';
    pExt[ 1 ] = 'g';
    pExt[ 2 ] = 'a';
    tga.Dump( pFileName );

    // save LUX font
    pExt[ 0 ] = 'x';
    pExt[ 1 ] = 'f';
    pExt[ 2 ] = 'n';
    FILE *fp = fopen( pFileName, "wb" );
    fwrite( g_pLUX, 1, sizeof( g_pLUX ), fp );
    fclose( fp );

    delete [] g_pTexture;

    MessageBox( 0, "Conversion complete", "XFont compiler", MB_OK );

    return 0;
}
