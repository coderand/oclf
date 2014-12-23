// This file is distributed under a MIT license. See LICENSE.txt for details.
//
// X3D engine header file
//

#ifndef __X3D_H__
#define __X3D_H__

#include <d3d9.h>
#include <d3dx9.h>
#include <windows.h>

//
// Common X3D types
//
#define	X3D_MAX_TEXTURECACHE    1024
#define X3D_MAX_FONTS           32
#define X3D_MAX_2DSPRITES       2000
#define X3D_MAX_DXSHADERS       256

typedef int HX3DTexture;
typedef int HX3DFont;
typedef int HXDXShader;

enum
{
    ERTF_DEFAULT        =   0x00000000,

    ERTF_CLAMPSQUAREX   =   0x00000001, // if texture height more than width, then height = width
    ERTF_SQUARESWAPXY   =   0x00000002, // in square textures swaps x and y
    ERTF_NOMIP          =   0x00000004, // do only one mip-map level
    ERTF_BUMPMAP        =   0x00000008, // load bump map in 2nd layer

    ERTF_FORCE_DWORD    =   0x7fffffff,
};

enum
{
    EPDF_DEFAULT        =   0x00000000,

    EPDF_NOCENTER       =   0x00000001, // draw sprite from left up corner

    EPDF_FORCE_DWORD    =   0x7fffffff,
};

#define FVF_TX3DSptireVtx ( D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 )
typedef struct
{
    float           fX, fY, fZ, fRHW;
    unsigned int    dwColor;
    float           fU, fV;
} TX3DSptireVtx;

//
// X3D D3D shader manager
//
class XD3DShader
{
public :

    bool            m_bIsVertexShader;	// in other case that is a pixel shader
    union
    {
    IDirect3DVertexShader9 *m_pVS;
    IDirect3DPixelShader9 *m_pPS;
    };
};

//
// X3D texture manager
//
class X3DTexture
{
public :
    int             m_nRef;
    char            m_pFileName[ 256 ];
    IDirect3DTexture9 *m_pTexture[ 2 ];
};

//
// X3D font
//
typedef struct
{
    float   fU, fV; // left-top corner
    float   fU2, fV2; // right-down corner
    float   fDx;
    float   fDy;
} TX3DCharInfo;

class X3DFont
{
public :

    char            m_pFontName[ XFS_MAX_FILENAME ];
    TX3DCharInfo    m_pChars[ 256 ];
    HX3DTexture     m_hTexture;

    bool            LoadFromFile        ( const char *pFontName );

                    X3DFont             ();
};

//
// X3D rendering device
//
class X3DRender
{
private :

    //
    // D3D Shader manager
    //
    XD3DShader      m_DXShaders[ X3D_MAX_DXSHADERS ];
    int             m_nNumDXShaders;

    //
    // Texture manager
    //
    X3DTexture      *m_pTextures[ X3D_MAX_TEXTURECACHE ];
    int             m_nNumTextures;

    HX3DTexture     AddTextureSlot      ();
    X3DTexture      *GetTextureSlot     ( HX3DTexture hTexture );

    //
    // Font manager
    //
    X3DFont         *m_pFonts[ X3D_MAX_FONTS ];
    int             m_nNumFonts;
    
    X3DFont         *GetFontSlot        ( HX3DFont hFont );

    //
    // Sprite drawing syste,
    //
    CIBuffer        *m_pSpriteIB;
    CVBuffer        *m_pSpriteVB;
    TX3DSptireVtx   *m_pSpriteVerts;
    int             m_nNumSprites;

    bool            Initialize2DDraw    ();

public :

    //
    // Shared devices's interfaces
    //
    HWND            m_hWnd;
    IDirect3DDevice9 *dev;

    HX3DFont        m_hFontCommon;

    //
    // Working with textures
    //
    void            SetTexture          ( int nStage, HX3DTexture hTexture, int nLayer = 0 );
    HX3DTexture     RegisterTexture     ( const char *pTexName, int nRegFlag = ERTF_DEFAULT );
    HX3DTexture     FindExistTexture    ( const char *pTexName );
    void            UnregisterTexture   ( HX3DTexture hTexture );

    HX3DFont        RegisterFont        ( const char *pFontName );
    int             PushDraw2DText      ( HX3DFont hFont, float fX, float fY, float fScale, unsigned int dwColor, int nPDFlag, const char *pFormat, ... );
    void            PushDraw2DSprite    ( HX3DTexture hTexture, float fX, float fY, float fWidth, float fHeight, unsigned int dwColor, int nPDFlag, float fAngle, float *pTexCoords );
    void            FlushSpriteBuffer   ();

    HXDXShader      RegisterDXShader    ( const char *pFileName, const DWORD *pDeclaration = NULL );
    void            SetDXShader         ( HXDXShader hDXShader );

    //
    // System methods
    //
    bool            Initialize          ();
                    
                    ~X3DRender          ();
};

#endif // __X3D_H__
