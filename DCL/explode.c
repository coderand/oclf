// This file is distributed under a MIT license. See LICENSE.txt for details.

#include "DCLib.h"
#include <string.h>

extern UInt32 dcl_explode(read_data_proc read_data, write_data_proc write_data,
      UInt8 *work_buff, void *param);

int g_pos, g_rpos;
int g_srcsize;
char *g_src;
char *g_dest;

UInt32 read_data
    (
    UInt8 *buffer, 
    UInt16 *psize, 
    void *param
    )
{
    int size = *psize;

    if ( g_rpos + size > *(int *)param )
        size = *(int *)param - g_rpos;

    memcpy( buffer, g_src, size );
    g_src += size;

    g_rpos += size;

    return size;
}

void write_data
    (
    UInt8 *buffer, 
    UInt16 *psize, 
    void *param
    )
{
    int size = *psize;

    memcpy( g_dest, buffer, size );

    g_dest += size;

    g_pos += size;
}

void DCLUnpack
    ( 
    void *pdest, 
    void *psrc, 
    unsigned int *fnsize, 
    int normalsize
    )
{
    int p;
    int work_buff[ CMP_BUFFER_SIZE ];

    p = normalsize;
    g_pos = 0;
    g_rpos = 0;
    g_srcsize = normalsize;
    g_src = psrc;
    g_dest = pdest;
    memset( work_buff, 0, sizeof( work_buff ) );

    dcl_explode( read_data, write_data, (unsigned char *)work_buff, &p );

    *fnsize = g_pos;
}
