// This file is distributed under a MIT license. See LICENSE.txt for details.
//
// COMMON
//

#ifndef __COMMON_H__
#define __COMMON_H__

#include <malloc.h>
#include "resource.h"

#define NEW             new
#define DEL( x )        { if ( x ) delete x; x = NULL; }
#define VDEL( x )       { if ( x ) delete [] x; x = NULL; }
#define ALLOCATE( x )   malloc( x )
#define FREE( x )       { if ( x ) free( x ); x = NULL; }
#define RELEASE( x )    { if ( (x) ) (x)->Release(); }

#define RGB_MAKE( r, g, b ) ( ( r << 16 ) | ( g << 8 ) | ( b ) )
#define RGBA_MAKE( r, g, b, a ) ( ( a << 24 ) | ( r << 16 ) | ( g << 8 ) | ( b ) )
#define RGBA_GETR( color ) ( ( color >> 16 ) & 0xff )
#define RGBA_GETG( color ) ( ( color >> 8 ) & 0xff )
#define RGBA_GETB( color ) ( ( color ) & 0xff )

float ShortToFloat( short sh );

#include "idxvtxbufs.h"
#include "xmatrix.h"
#include "xfs.h"
#include "x3d.h"
#include "ocentity.h"
#include "oc3d.h"
#include "ocinput.h"

extern "C"
{
    extern  XFileSystem     *xfs;
    extern  X3DRender       *x3d;

    extern  OCInput         *ocinput;
    extern  OCWorld         *ocworld;
    extern  OCGUI           *ocgui;

    extern  void XFS_EXT_Register_OCPak();
};

#endif __COMMON_H__
