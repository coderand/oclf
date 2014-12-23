// This file is distributed under a MIT license. See LICENSE.txt for details.
//
// Outcast 3D system by AND 2002
//

#ifndef __OC3D_H__
#define __OC3D_H__

#include "XVec3.h"
#include "XEuler.h"
#include "XBBox.h"
#include "XPlane.h"
#include "ocdata.h"
#include "CATess.h"
#include "CRenderer.h"
#include "ocinput.h"
#include "x3d.h"

//
// Common defines and general types
//

#define MAX_OC_FILENAME     256

class OCWorld;
class OCViewer;
class OCMeshObject;

enum
{
    // common flags
    EOBJF_NONE              = 0x00000000, // flag bits empty
    
    // rendering passes
    EOBJF_RP_COMMON         = 0x00000000, // common pass type
    EOBJF_RP_WATER          = 0x00010000, // object has water effect
    EOBJF_RP_PROJSHADOW     = 0x00020000, // render projected shadow

    // combined flags
    EOBJF_DEFAULT           = EOBJF_NONE,

    EOBJF_FORCE_DWORD       = 0x7fffffff,
};

class OCObject
{
protected :

    int             m_nFlags;

public :

    XVec3           m_xvPos;    // position of object in a world
    XEuler          m_xeRot;    // rotation of object in a world
    XBBox           m_xbBox;    // visible bounding box

    virtual	void    Render              ( int nPassType = EOBJF_RP_COMMON ) {};
    virtual	void    SetLODLevel         ( float from0to1 ) {};
    virtual int     GetNumFaces         () { return 0; };
    virtual int     GetFlags            () { return m_nFlags; };

                    OCObject            ();
    virtual         ~OCObject           ();
};

class OCRefObject
{
public :

    float           m_fDist2;   // square distance to view

    OCObject        *m_pObj;
};

//
// Common triangular object
//
class OCTriObject : public OCObject
{
public :
};

//
// Outcast's mesh objects
//
#define FVF_OCMSHVertex ( D3DFVF_XYZ | D3DFVF_TEX1 )

class OCMSHVertex : public XVec3
{
public :
    
    float           u, v;
};

#define FVF_OCAMHVertex ( D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX2 )
class OCAMHVertex : public XVec3
{
public :

    XVec3           n;
    float           u, v;
    float           u2, v2;
};

typedef struct : public XVec3
{
    XVec3           n;
} TAMHProcVertex;

typedef struct
{
    float           x, y, z;
    float           nx, ny, nz;
    float           u, v;
    short           next;
} TMSHVertex_opt;

typedef struct 
{
    int             textureID;
    int             idx[ 3 ];
} TMSHFace_opt;

class OCAnim
{
public :

    COCAnimation    m_ANM;

    OCMtx           *m_mStack;
    OCVec           *m_xvBoneStack;

    void            Load                ( const char *pFileName );

                    OCAnim              ();
                    ~OCAnim             ();
};

class OCMeshCache
{
    friend OCWorld;
    friend OCMeshObject;

private :
    void            *m_pMESHData;
    TMSHVertex_opt  *m_pVerts;
    TMSHFace_opt    *m_pFaces;

    TAMHProcVertex  *m_pVertsMesh;

    void            ReconstructMeshData ( bool bDoSort = true );
    void            ApplyXForm          ( int startV, int endV, OCMtx mat );

public :

    int             m_nNumSurfaces;
    HX3DTexture     m_phTexture[ 64 ][ 4 ];
    int             m_pStartIndex[ 64 ];
    int             m_pStripeLength[ 64 ];

    CIBuffer        *m_pIB;
    CVBuffer        *m_pVB;
    int             m_nNumVerts;
    int             m_nNumFaces;

    char            m_chName[ 256 ];
    COCMesh         m_OCMesh;

    void            Init                ();

    void            RenderAMH           ( OCMeshObject *pObj, int nPassType, OCAnim *pAnim, float fFrame, OCAnim *pAnim2, float fFrame2, float fBlend );

                    OCMeshCache         ();
                    ~OCMeshCache        ();
};

class OCMeshObject : public OCObject
{
    friend OCWorld;

public :

    OCMeshCache *   m_pMesh;
    XMat4           m_mWorld;

    OCAnim *        m_pAnim;
    float           m_fFrame;
    OCAnim *        m_pAnim2;
    float           m_fFrame2;
    float           m_fBlend;

    void            Init                ();

    virtual	void    Render              ( int nPassType );
    virtual int     GetNumFaces         ();
    void            UpdateTransform     ();

                    OCMeshObject        ();
};

//
// Land scape engine emulation
//

//
// Voxel tile container
//
#define FVF_OCVertex ( D3DFVF_XYZ | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE2(0) )

class OCVertex : public XVec3
{
public :
    
    float           u, v;
};

class OCTileVertex : public XVec3
{
public :
    
    float           u, v;
    DWORD           indexes;
};

class OCFace
{
public :
};

#define FVF_OCWVertex ( D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1 )

class OCWVertex : public XVec3
{
public :

    int         color;
    float       u, v;
};

class OCTileContainer
{
public :

    IDirect3DTexture9 *m_pTexture;
    CIBuffer        *m_pIB;
    CVBuffer        *m_pVB;
    CVBuffer        *m_pWaterVB;
    int             m_nNumVerts;
    int             m_nNumFaces[ 4 ];
    int             m_nStartIBIdx[ 4 ];
    float           m_fLowLevel;
    float           m_fHighLevel;

    void            CreateFromCMapTile  ( const CMapTile *pmt );

                    OCTileContainer     ();
                    ~OCTileContainer    ();
};

//
// Voxel tile represented as set of vertexes and triangles
//
class OCTile : public OCObject
{
    friend OCWorld;

private :

    OCTileContainer *m_pTileContainer;
    const CMapTile  *m_pMapTile;
    int             m_nLODLevel;

    void            PreRenderWater      ();

public :

    float           GetHeight           ( int x, int y );

    void            Init                ( int nX, int nY );

    virtual void    Render              ( int nPassType );
    virtual void    SetLODLevel         ( float from0to1 );
    virtual int     GetNumFaces         ();
};

//
// Outcast's game classes
//

//
// Base player
//
class OCBasePlayer : public OCObject
{
public :

    XVec3           m_xvVelocity;
};

class OCDrivenPlayer : public OCBasePlayer
{
protected :

    float           m_fRotX, m_fRotXLast;
    float           m_fRotY, m_fRotYLast;
    bool            m_bInMove;

public :

    OCViewer        *m_pViewer;

    void            UpdateInput         ( OCInput *pInput );
    virtual void    Render              ( int nPassType );

                    OCDrivenPlayer      ();
                    ~OCDrivenPlayer     ();
};

//
// Generals Outcast's classes
//

//
// Outcast viewer
//
#define OC_FRUSTUM_NEAR     0
#define OC_FRUSTUM_FAR      1
#define OC_FRUSTUM_LEFT     2
#define OC_FRUSTUM_RIGHT    3
#define OC_FRUSTUM_TOP      4
#define OC_FRUSTUM_BUTTOM   5

class OCViewer
{
private :

    XPlane          m_xpPlanes[ 6 ];

public :

    XMat4           m_mView;
    XMat4           m_mProj;
    XMat4           m_mTransform;

    XVec3           m_xvPos;
    XVec3           m_xvTgt;
    float           m_fRoll;
    float           m_fFOV;
    float           m_fNearClip;
    float           m_fFarClip;
    float           m_fAspect;

    void            SetRender           ();
    void            CalculateFrustum    ();
    bool            TestWithBBox        ( const XBBox &bbox );

                    OCViewer            ();
};

//
// Logical actor
//
class CActor
{
public :

    char            *pFileAMH;
    char            *pFileAnimRun;
    char            *pFileAnimStop;

    float           fSpeed;
    OCMeshCache     *pMesh;
    OCAnim          *pAnim1, *pAnim2;

    CActor( char *name, char *run, char *stop, float speed = 38.f )
        : pFileAMH( name ), pFileAnimRun( run ), pFileAnimStop( stop ),
        pMesh( NULL ), pAnim1( NULL ), pAnim2( NULL ), fSpeed( speed )
    {
    }

    OCMeshCache     *GetMesh();

    void Release()
    {
        DEL( pAnim1 );
        DEL( pAnim2 );
        pMesh = NULL;
    }
};

//
// Outcast world
//
class OCWorld
{
    friend class OCTile;

private :

    void            *m_pLNDFile;
    void            *m_pWRDFile;
    CVBuffer        *m_pWaterVB;
    CIBuffer        *m_pWaterIB;

    void            CreateDXShaders();
    void            LoadAndRegisterDecorations();

protected :

    char            m_pLNDFileName[ MAX_OC_FILENAME ]; // name of LND file
    COCLandscapeData *m_pLND;
    COCWorldMarks   *m_pWRD;

    OCTileContainer *m_pTileContainers;
    int             m_nNumTileContainers;

public :

    OCTile          *m_pTilesMap;
    HX3DTexture     m_hSkyTexture;
    HX3DTexture     m_hFogTexture;
    HX3DTexture     m_hDetailTexture;
    IDirect3DTexture9 *m_psBumpMap;

    static CActor   g_pActors[];

protected :

    OCViewer        *m_pCurrView;
    CVBuffer        *m_pSkyVB;
    IDirect3DTexture9 *m_psWaterDecalMap;

    //
    // Tri objects cache wrapped on CObject
    //
    OCMeshCache     **m_pMeshObjects;
    int             m_nNumMeshObjects;

    OCMeshObject    **m_pStaticObjects;
    int             m_nNumStaticObjects;

    //
    // References to realtime rendered objects
    //
friend class OCMeshCache;
    OCRefObject     *m_pRefObjects;
    OCRefObject     **m_pRefRenderSet;
    int             m_nNumRefObjects;

    void            SortRefObjects      ();
    void            RenderWater         ();
    void            RenderSky           ();
    void            BuildSkyAndWaterEffects();

public :

    IDirect3DVertexDeclaration9 *m_pSpriteVtxDecl;
    IDirect3DVertexDeclaration9 *m_pAMHVtxDecl;
    static D3DVERTEXELEMENT9 m_dwAMHVtxDecl[];

    HXDXShader      m_hOCVertex_vso;
    HXDXShader      m_hOCVertex_pso;
    HXDXShader      m_hOCBlur_vso;
    HXDXShader      m_hOCBlur_pso;
    HXDXShader      m_hOCGlowOut_pso;
    HXDXShader      m_hAMHBump_vso;
    HXDXShader      m_hAMHBump_pso;

    OCMeshCache     *AddMeshObject      ( const char *pFileName );
    OCMeshObject    *AddStaticObject    ( OCMeshCache *pMesh );

    void            RenderShadow        ( const XBBox &bbox );

    unsigned int    m_dwFogColor;
    float           m_fFogUp;
    float           m_fFogDown;
    int             m_nMapWidth;
    int             m_nMapHeight;
    bool            m_b1STPerson;
    
    int             m_nActorNum;
    OCDrivenPlayer  *m_pPlayer;

    void            DrawLoadProgress    ( float fVal );

    bool            IsLoaded            () { return (bool)(m_pLND != NULL); };
    bool            LoadMap             ( const char *pMapName );
    void            ShutDown            ();
    bool            Update              ();
    bool            Render              ();
    void            SetViewer           ( OCViewer *pView );
    OCViewer        *GetViewer          () { return m_pCurrView; };
    inline const char *GetMapName       () { return m_pLNDFileName; };
    float           GetHeight           ( float x, float y );

    void            AddRefObject        ( OCObject *pObj );

                    OCWorld             ();
                    ~OCWorld            ();
};

//
// Outcast user interface
//

enum
{
    EOMI_BUTTON     =   0x00000001,
    EOMI_CANCEL     =   0x00000002,
    EOMI_TEXT       =   0x00000004,

    EOMI_FORCE_DWORD=   0x7fffffff,
};

// menu item
class OCGUIItem
{
public :
    
    int             m_nType;

    float           m_fX, m_fY;
    float           m_fWidth, m_fHeight;
    char            m_pCaption[ 256 ];

    void            SetProperties       ( int nType, float fX, float fY, float fWidth, float fHeight, const char *pCaption );
};

// base menu
class OCGUIMenu
{
public :

    OCGUIItem       *m_pItems;
    int             m_nNumItems;
    int             m_nDefaultItem;
    int             m_nCurrItem;

    float           m_fMorphVel;
    float           m_fMorphTime;

    virtual void    Cancel              ();
    virtual void    SelectItem          ();
    virtual void    Start               ();
    virtual void    Render              ();
    virtual void    Update              ();
    virtual void    SelectItem          ( int nDX, int nDY );

                    OCGUIMenu           ();
    virtual         ~OCGUIMenu          ();
};

// main OC GUI
class OCGUI
{
private :

    float           m_fCVel;
    float           m_fCounter;
    bool            m_bActivated;
    bool            m_bState;

    OCGUIMenu       *m_pLastMenu;
    OCGUIMenu       *m_pCurrMenu;

    void            UpdateCursor        ();

public :

    HX3DTexture     m_hBackgroundTexture;
    HX3DTexture     m_hHLightTexture;
    HX3DTexture     m_hGamePanel;

    bool            m_bCursorOver;

    void            PushDrawHilight     ( float x, float y, float w, float h, unsigned int color );

    void            Enable              ( bool bState ) { m_bState = bState; };
    void            Activate            ( bool bState );
    bool            IsActivated         () { return m_bActivated && m_bState; };
    bool            Initialize          ();
    void            Update              ();
    bool            ProcessMessage      ( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam	);

    void            SetMenu             ( OCGUIMenu *pMenu = NULL );

                    OCGUI               ();
                    ~OCGUI              ();
};

#endif __OC3D_H__
