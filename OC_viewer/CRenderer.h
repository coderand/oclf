// This file is distributed under a MIT license. See LICENSE.txt for details.
//
// CRenderer ( header )
//

#ifndef __CRENDERER_H__
#define __CRENDERER_H__

#include <d3d9.h>
#include <d3dx9.h>
#include <windows.h>
#include "xvec3.h"
#include "xmatrix.h"
#include "xeuler.h"
#include "xquat.h"

extern HINSTANCE g_hInstance;

#define CLASSNAME       "CRenderer"

#define RFVF_XYZNCT2 ( D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1 )
typedef struct _Tfvf_XYZNCT2
{
    XVec3   p;
    XVec3   n;
    DWORD   color;
    float   u, v;
} Tfvf_XYZNCT2;

#define RFVF_XYZ ( D3DFVF_XYZ )
typedef struct _Tfvf_XYZ
{
    XVec3   p;
} Tfvf_XYZ;

#define TSpriteVertex_FVF D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1
typedef struct
{
    float sx, sy, sz, rhw;
    DWORD color;
    float u, v;
} TSpriteVertex;

#define FVF_TTextVertex D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1
typedef struct
{
    float sx, sy, sz, rhw;
    DWORD color;
    float u, v;
} TTextVertex;

//
// CRenderer::DrawText effect
//
#define RDTFX_DEFAULT   0x00
#define RDTFX_BLURALL   0x01
#define RDTFX_BLURCHARS 0x02
#define RDTFX_SINELINE  0x04

extern "C"
{
    extern LPDIRECT3DDEVICE9    pDev;

    extern DWORD                dwWidth;
    extern DWORD                dwHeight;
    extern int                  m_numTextures;
    extern LPDIRECT3DTEXTURE9   m_pTexture[ 128 ];
    extern LPDIRECT3DTEXTURE9   m_pFontTexture;
    extern int                  m_fontSizes[ 256 ];
    extern float                sys_timeCounter;
    extern DWORD                sys_dwtimeCounter;
    extern float                sys_timeDelta;
    extern LPDIRECT3DSURFACE9   m_pRenderSurface;
    extern LPDIRECT3DSURFACE9   m_pZRenderSurface;
    extern float                BasedWidth_K;
    extern float                BasedHeight_K;
    extern IDirect3DTexture9    *pLFTexture, *pLFTexture2;
    extern IDirect3DSurface9    *pLFSurface, *pLFSurface2;
    extern IDirect3DSurface9    *pLFZSurface;

    extern XVec3                g_xvEyePoint;
    extern XVec3                g_xvEyeDirection;

    extern void     rend_Build          ();
    extern void     rend_Kill           ();

    extern bool     rend_OpenWindow     ( HINSTANCE hInstance );
    extern bool     rend_Init           ( HINSTANCE hInstance, D3DFORMAT d3dfmtRender = D3DFMT_X8R8G8B8, D3DFORMAT d3dfmtZBuffer = D3DFMT_D24S8 );
    extern void     rend_UpdateScreen   ();
    extern void     rend_NewFrame       ( unsigned int color );
    extern void     rend_CreateFontBitmap();
    extern bool     rend_CreateBuffers  ();
    extern void     rend_SetViewProj    ( const XVec3 camPos, const XVec3 camTgt, float camRoll = 0, float camFov = 90, float nearP = 0.1, float farP = 10000, float aspect = 0 );
    extern void     rend_Start          ();
    extern void     rend_SetBlend       ( D3DBLEND d3drs_SRC, D3DBLEND dwdrs_DEST );

    extern void     rend_PushDrawSprite ( int, int, float, int, int, float angle = 0.0, DWORD = 0xffffffff, float rhw = 1.0f, float texmul = 0.0f );
    extern void     rend_PushDrawFlare  ( int id, float x, float y, float z,
                                          float w, float h, DWORD color = 0xffffffff, float rhw = 1.0 );
    extern void     rend_PushDraw3DSprite( XVec3 pos,
                                          short w, short h, DWORD color, float angle, char chFX = 0 );
    extern void     rend_FlushSprites   ();

    extern void     rend_DrawText       ( float x, float y, char *pStr, int color = 0xffffffff, float zoom = 1.0f, BYTE rfx = RDTFX_DEFAULT, float localTime = 0.0f );
    extern float    rend_PrintCharXY    ( int x, int y, char sym, int color = 0xffffffff, float zoom = 1.0f );


    extern void     rend_PushDrawLine   ( short x1, short y1, short x2, short y2, float width = 1.f, DWORD color = 0xffffffff );
    extern void     rend_PushDrawRect   ( short cx, short cy, short width, short height, float angle = 0.f, DWORD color = 0xffffffff );
    extern void     rend_FlushLines     ();

    //
    // System methods
    //
    extern void     rend_SetTexture     ( DWORD Stage, IDirect3DBaseTexture9* pTexture );
    extern void     rend_SetRenderState ( D3DRENDERSTATETYPE State, DWORD Value );
    extern void     rend_SetTextureStageState ( DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD Value );

    //
    // Macros like toolz
    //
    extern void     rend_SetFog         ( float fFogStart, float fFogEnd, DWORD dwFogColor );
    extern void     rend_SetEnv         ( int nStage, bool bState );
    extern void     rend_SetLight       ( int num, const XVec3 pos, const XVec3 diffuse, const XVec3 specular, float atten, float atten0 = 0.0f );

    extern void     rend_SetRenderFX    ();
    extern void     rend_RestoreRenderFX( int dwType, float fBlurPower );

    extern unsigned int rend_RGBA       ( register int r, register int g, register int b, register int a );

    extern bool g_bFullScreen;
    extern bool g_bGlow;
    extern bool g_bComplexWater;
    extern bool g_bBumpMapping;
    extern bool g_bShadows;
    extern bool g_bSoftShadows;
    extern bool g_bShowFPS;
};

#define setBlend_ONE_ONE()      rend_SetBlend( D3DBLEND_ONE, D3DBLEND_ONE );
#define setBlend_BOTHSRCALPHA() rend_SetBlend( D3DBLEND_SRCALPHA, D3DBLEND_INVSRCALPHA );
#define setBlend_ONE_ZERO()     rend_SetBlend( D3DBLEND_ONE, D3DBLEND_ZERO );

//
// Debug
//

#define SQUISH_DEBUG

#ifdef SQUISH_DEBUG

void DInit();
void DPrint( const char *pStr, ... );
void PrintDXError( HRESULT hr, const char *pCallerName );

#define DINIT() DInit()
#define DXERROR( x ) PrintDXError( x, "" )
#define DXFAIL( x ) PrintDXError( x, #x )
#define DPRINT( x ) DPrint( x )

#else

#define DINIT()
#define DXERROR( x )
#define DXFAIL( x ) x
#define DPRINT( x )

#endif // SQUISH_DEBUG

#endif __CRENDERER_H__
