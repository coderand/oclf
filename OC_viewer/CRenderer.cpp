// This file is distributed under a MIT license. See LICENSE.txt for details.
//
// CRenderer
//

#include <d3d9.h>
#include <d3dx9.h>
#include <ddraw.h>
#include <windows.h>
#include "CRenderer.h"
#include "common.h"
#include <conio.h>
#include "xmath.h"
#include <commctrl.h>
#include "resource.h"

extern bool g_bRun;

#ifdef SQUISH_DEBUG
char FRAMENAME[] = "OUTCAST Lives Forever by AND 2002-2005";
#else
char FRAMENAME[] = "OUTCAST Lives Forever by AND 2002-2005";
#endif // SQUISH_DEBUG

    DWORD               BasedWidth;
    DWORD               BasedHeight;
    float               BasedWidth_K;
    float               BasedHeight_K;
    float               OriginX;
    float               OriginY;
    float               BasedOriginX;
    float               BasedOriginY;
    bool                bWindowed;
    DWORD               dwWidth;
    DWORD               dwHeight;
    DWORD               dwBpp;
    HINSTANCE           hInstance;
    HWND                m_hWnd;
    bool                bShowWindow;
    DWORD               m_startTime;
    D3DFORMAT           m_d3dfmtBackBuffer;
    D3DFORMAT           m_d3dfmtTextureAlpha;
    D3DFORMAT           m_d3dfmtZBuffer;
    LPDIRECT3D9         m_pD3D;
    LPDIRECT3DSURFACE9  m_pRenderSurface;
    LPDIRECT3DSURFACE9  m_pZRenderSurface;
    D3DVIEWPORT9        m_d3dViewPort;
    XMat4               m_mView;
    XMat4               m_mProj;
    XMat4               m_mWorld;
    XMat4               m_mTransform;
    CVBuffer            m_SpriteVB;
    CVBuffer            m_LineVB;
    
    LPDIRECT3DDEVICE9   pDev;
    int                 m_numTextures;
    LPDIRECT3DTEXTURE9  m_pTexture[ 128 ];
    LPDIRECT3DTEXTURE9  m_pFontTexture;
    int                 m_fontSizes[ 256 ];
    float               sys_timeCounter;
    DWORD               sys_dwtimeCounter;
    float               sys_timeDelta;
    XVec3               g_xvEyePoint;
    XVec3               g_xvEyeDirection;

    IDirect3DTexture9   *pLFTexture, *pLFTexture2;
    IDirect3DSurface9   *pLFSurface, *pLFSurface2;
    IDirect3DSurface9   *pLFZSurface;

void rend_Build()
{
    BasedWidth	= 640;
    BasedHeight = 480;
    BasedOriginX = BasedWidth * 0.5;
    BasedOriginY = BasedHeight * 0.5;

    dwWidth = 800;
    dwHeight = 600;

    bWindowed = 0;
    
    dwBpp = 32;
    m_d3dfmtBackBuffer = D3DFMT_UNKNOWN;
    m_d3dfmtTextureAlpha = D3DFMT_UNKNOWN;
    m_d3dfmtZBuffer = D3DFMT_UNKNOWN;
    bShowWindow = true;

    m_pD3D = NULL;
    pDev = NULL;
    m_pFontTexture = NULL;
}

D3DADAPTER_IDENTIFIER9 idf;

D3DDISPLAYMODE *pModes = NULL;
int m_nCurrentMode = 0;

void rend_Kill()
{
    pLFZSurface->Release();
    pLFTexture->Release();
    pLFTexture2->Release();
    VDEL( pModes );
}

bool g_bFullScreen = true;
bool g_bVSync = false;
bool g_bGlow = true;
bool g_bComplexWater = true;
bool g_bBumpMapping = true;
bool g_bShadows = true;
bool g_bSoftShadows = true;
bool g_bShowFPS = true;

BOOL FAR PASCAL AboutProc
    (
    HWND hWnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam
    )
{
    switch( message )
    {
    case WM_CLOSE :
        EndDialog( hWnd, FALSE );
        return TRUE;
    case WM_COMMAND:
        switch( LOWORD( wParam ) )
        {
        case IDCANCEL :
            EndDialog( hWnd, FALSE );
            return TRUE;

        case IDOK :
            EndDialog( hWnd, TRUE );
            return TRUE;
        }
    }

    return FALSE;
}

BOOL FAR PASCAL SetupProc
    (
    HWND hWnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam
    )
{
    switch( message )
    {
    case WM_INITDIALOG :
        {
            SendMessage( hWnd, WM_SETICON, ICON_BIG, (LPARAM)LoadIcon( g_hInstance, MAKEINTRESOURCE( IDI_ICON1 ) ) );

            SetWindowText( GetDlgItem( hWnd, IDC_VIDEOCARD ), idf.Description );

            int numModes = m_pD3D->GetAdapterModeCount( D3DADAPTER_DEFAULT, D3DFMT_X8R8G8B8 );
            int i, cm;
            pModes = NEW D3DDISPLAYMODE[ numModes ];
            static char str[ 256 ];
            int lastW = -1;
            int lastH = -1;
            int lastHZ = -1;

            cm = 0;
            for ( i = 0; i < numModes; i++ )
            {
                if ( !FAILED( m_pD3D->EnumAdapterModes( D3DADAPTER_DEFAULT, D3DFMT_X8R8G8B8, i, &pModes[ cm ] ) ) )
                {
                    {
                        sprintf( str, "%d x %d   (%d Hz)", pModes[ cm ].Width, pModes[ cm ].Height, (int)pModes[ cm ].RefreshRate );
                        SendDlgItemMessage( hWnd, IDC_RMODES, CB_ADDSTRING, 0, (LPARAM)str );
                        lastW = pModes[ cm ].Width;
                        lastH = pModes[ cm ].Height;
                        if ( lastW == 800 && lastH == 600 && lastHZ < (int)pModes[ cm ].RefreshRate )
                        {
                            m_nCurrentMode = cm;
                            lastHZ = pModes[ cm ].RefreshRate;
                        }
                        
                        cm++;
                    }
                }
            }

            // get abilities of the card
            {
                bool bIsGF3 = false;

                D3DCAPS9 caps;
                memset( &caps, 0, sizeof( caps ) );
                m_pD3D->GetDeviceCaps( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps );

                if ( ( ( caps.VertexShaderVersion & 0xffff ) >= 0x0101 ) &&
                     ( caps.MaxVertexShaderConst >= 96 ) &&
                     ( ( caps.PixelShaderVersion & 0xffff ) >= 0x0101 ) )
                {
                    bIsGF3 = true;
                }

                if ( !bIsGF3 )
                {
                    g_bGlow = false;
                    g_bComplexWater = false;
                    g_bBumpMapping = false;
                    g_bSoftShadows = false;

                    EnableWindow( GetDlgItem( hWnd, IDC_GLOW ), FALSE );
                    EnableWindow( GetDlgItem( hWnd, IDC_COMPLEXWATER ), FALSE );
                    EnableWindow( GetDlgItem( hWnd, IDC_BUMPMAPPING ), FALSE );
                    EnableWindow( GetDlgItem( hWnd, IDC_SOFTSHADOWS ), FALSE );
                }
            }

            SendDlgItemMessage( hWnd, IDC_RMODES, CB_SETCURSEL, m_nCurrentMode, 0);
            SendDlgItemMessage( hWnd, IDC_FULLSCREEN, BM_SETCHECK, g_bFullScreen, 0 );
            SendDlgItemMessage( hWnd, IDC_VSYNC, BM_SETCHECK, g_bVSync, 0 );
            SendDlgItemMessage( hWnd, IDC_GLOW, BM_SETCHECK, g_bGlow, 0 );
            SendDlgItemMessage( hWnd, IDC_COMPLEXWATER, BM_SETCHECK, g_bComplexWater, 0 );
            SendDlgItemMessage( hWnd, IDC_BUMPMAPPING, BM_SETCHECK, g_bBumpMapping, 0 );
            SendDlgItemMessage( hWnd, IDC_SHADOWS, BM_SETCHECK, g_bShadows, 0 );
            SendDlgItemMessage( hWnd, IDC_SOFTSHADOWS, BM_SETCHECK, g_bSoftShadows, 0 );
            SendDlgItemMessage( hWnd, IDC_SHOWFPS, BM_SETCHECK, g_bShowFPS, 0 );
        }
        return FALSE;

    case WM_DESTROY :
        g_bFullScreen = SendDlgItemMessage( hWnd, IDC_FULLSCREEN, BM_GETCHECK, 0, 0 ) != 0;
        g_bVSync = SendDlgItemMessage( hWnd, IDC_VSYNC, BM_GETCHECK, 0, 0 ) != 0;
        m_nCurrentMode = SendDlgItemMessage( hWnd, IDC_RMODES, CB_GETCURSEL, 0, 0 );
        g_bGlow = SendDlgItemMessage( hWnd, IDC_GLOW, BM_GETCHECK, 0, 0 ) != 0;
        g_bComplexWater = SendDlgItemMessage( hWnd, IDC_COMPLEXWATER, BM_GETCHECK, 0, 0 ) != 0;
        g_bBumpMapping = SendDlgItemMessage( hWnd, IDC_BUMPMAPPING, BM_GETCHECK, 0, 0 ) != 0;
        g_bShadows = SendDlgItemMessage( hWnd, IDC_SHADOWS, BM_GETCHECK, 0, 0 ) != 0;
        g_bSoftShadows = SendDlgItemMessage( hWnd, IDC_SOFTSHADOWS, BM_GETCHECK, 0, 0 ) != 0;
        g_bShowFPS = SendDlgItemMessage( hWnd, IDC_SHOWFPS, BM_GETCHECK, 0, 0 ) != 0;
        return TRUE;

    case WM_CLOSE :
        EndDialog( hWnd, FALSE );
        return TRUE;

    case WM_COMMAND:
        switch( LOWORD( wParam ) )
        {
        case IDC_ABOUT :
            DialogBox( hInstance, MAKEINTRESOURCE( IDD_DIALOG2 ), hWnd, (DLGPROC)AboutProc );
            return TRUE;

        case IDC_SHADOWS :
            g_bShadows = 0 != SendDlgItemMessage( hWnd, IDC_SHADOWS, BM_GETCHECK, 0, 0 );
            EnableWindow( GetDlgItem( hWnd, IDC_SOFTSHADOWS ), g_bShadows );
            return TRUE;

        case IDCANCEL :
            EndDialog( hWnd, FALSE );
            return TRUE;

        case IDOK :
            EndDialog( hWnd, TRUE );
            return TRUE;
        }
    }

    return FALSE;
}


bool rend_Init( HINSTANCE hInstance, D3DFORMAT d3dfmtRender, D3DFORMAT d3dfmtZBuffer )
{
    D3DPRESENT_PARAMETERS d3dpp;
    HRESULT hr;
    ::hInstance = hInstance;

    DPrint( ".Initialize rendering system\n" );

    m_pD3D = Direct3DCreate9( D3D_SDK_VERSION );
    if ( m_pD3D )
    {
        DPRINT( ".direct3D9 device found\n" );

        if ( !FAILED( m_pD3D->GetAdapterIdentifier( D3DADAPTER_DEFAULT, 0, &idf ) ) )
        {
            typedef struct
            {
                WORD	w[ 4 ];
            } TLI;
            TLI *ptli = (TLI *)&idf.DriverVersion;

            DPrint( "-Driver      : %s, version %d.%d.%d.%d\n", idf.Driver, ptli->w[ 3 ], ptli->w[ 2 ], ptli->w[ 1 ], ptli->w[ 0 ] );
            DPrint( "-Description : %s\n", idf.Description );
        }
    }
    else
    {
        DPRINT( "! Error : direct3D9 device not found\n" );
        return false;
    }

    InitCommonControls();
    if ( !DialogBox( hInstance, MAKEINTRESOURCE( IDD_DIALOG1 ), NULL, (DLGPROC)SetupProc ) )
        return false;

    dwWidth = pModes[ m_nCurrentMode ].Width;
    dwHeight = pModes[ m_nCurrentMode ].Height;
    bWindowed = !g_bFullScreen;

    BasedWidth_K = (float)dwWidth / (float)BasedWidth;
    BasedHeight_K = (float)dwHeight / (float)BasedHeight;

    if ( !rend_OpenWindow( hInstance ) ) return false;

    m_d3dfmtBackBuffer = d3dfmtRender;
    m_d3dfmtZBuffer = d3dfmtZBuffer;
    ZeroMemory( &d3dpp, sizeof( d3dpp ) );
    d3dpp.Windowed = bWindowed;
    d3dpp.BackBufferWidth = dwWidth;
    d3dpp.BackBufferHeight = dwHeight;

    DPrint( ".try %dx%d, ", dwWidth, dwHeight );

    if ( bWindowed )
    {
        d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
        d3dpp.BackBufferCount = 1;

        DPrint( "windowed\n" );
    }
    else
    {
        d3dpp.SwapEffect = D3DSWAPEFFECT_FLIP;
        d3dpp.BackBufferCount = 2;

        if ( g_bVSync )
            d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
        else
            d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

        DPrint( "fullscreen\n" );
    }

    d3dpp.BackBufferFormat = m_d3dfmtBackBuffer;
    d3dpp.hDeviceWindow = m_hWnd;
    d3dpp.EnableAutoDepthStencil = true;
    d3dpp.AutoDepthStencilFormat = m_d3dfmtZBuffer;

    DPRINT( ".create device\n" );
    if ( FAILED( hr = m_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_hWnd,
                                       D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp, &pDev ) ) )
    {
        DPRINT( "! Error : can't create device : " );
        DXERROR( hr );
        return false;
    }

    if ( FAILED( hr = pDev->GetRenderTarget( 0, &m_pRenderSurface ) ) )
    {
        DPRINT( "! Error : can't get access to target surface : " );
        DXERROR( hr );
        return false;
    }

    if ( FAILED( hr = pDev->GetDepthStencilSurface( &m_pZRenderSurface ) ) )
    {
        DPRINT( "! Error : can't get access to Z target surface : " );
        DXERROR( hr );
        return false;
    }

    DPrint( "# total texture memory : %d\n", pDev->GetAvailableTextureMem() );


    rend_CreateBuffers();
    m_d3dViewPort.X = 0;
    m_d3dViewPort.Y = 0;
    m_d3dViewPort.Width = dwWidth;
    m_d3dViewPort.Height = dwHeight;
    m_d3dViewPort.MinZ = 0; 
    m_d3dViewPort.MaxZ = 1.0;
    pDev->SetViewport( &m_d3dViewPort );

    for ( int i = 0; i < 7; i++ )
    {
        pDev->SetSamplerState( i, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
        pDev->SetSamplerState( i, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
        pDev->SetSamplerState( i, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );
        pDev->SetTextureStageState( i, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2 );
    }

    D3DMATERIAL9 mat;
    mat.Diffuse.r = mat.Diffuse.g = mat.Diffuse.b = 
    mat.Specular.r = mat.Specular.g = mat.Specular.b = 1.0;
    mat.Emissive.r = mat.Emissive.g = mat.Emissive.b = mat.Ambient.r = mat.Ambient.g = mat.Ambient.b = 0.0;
    mat.Emissive.a = mat.Diffuse.a = mat.Specular.a = mat.Ambient.a = 1.0;
    mat.Power = 10;
    pDev->SetMaterial( &mat );

    DXFAIL( pDev->CreateTexture( 256, 256, 1, D3DUSAGE_RENDERTARGET, m_d3dfmtBackBuffer, D3DPOOL_DEFAULT, &pLFTexture, NULL ) );
    DXFAIL( pDev->CreateTexture( 256, 256, 1, D3DUSAGE_RENDERTARGET, m_d3dfmtBackBuffer, D3DPOOL_DEFAULT, &pLFTexture2, NULL ) );
    DXFAIL( pLFTexture->GetSurfaceLevel( 0, &pLFSurface ) );
    DXFAIL( pLFTexture2->GetSurfaceLevel( 0, &pLFSurface2 ) );
    DXFAIL( pDev->CreateDepthStencilSurface( 256, 256, m_d3dfmtZBuffer, D3DMULTISAMPLE_NONE, 0, true, &pLFZSurface, NULL ) );

    return true;
}

LRESULT WINAPI rend_WndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );

bool rend_OpenWindow
    (
    HINSTANCE hInstance
    )
{
    WNDCLASS wc;

    hInstance = 0;

    DPrint( ".create Window : \"%s\"\n", FRAMENAME );

    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = rend_WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = 0;
    wc.hCursor = LoadCursor( hInstance, MAKEINTRESOURCE( IDC_CURSOR1 ) );
    wc.hbrBackground = (HBRUSH)BLACK_PEN;
    wc.lpszMenuName = NULL;
    wc.lpszClassName = CLASSNAME;
    RegisterClass( &wc );

    int flags;
    if ( bWindowed ) flags = WS_POPUP | WS_CAPTION | WS_OVERLAPPEDWINDOW | WS_SYSMENU;
    else flags = WS_POPUP;

    m_hWnd = CreateWindowEx( WS_EX_APPWINDOW, CLASSNAME, FRAMENAME,
                             flags, 0, 0, dwWidth, dwHeight, NULL, NULL, hInstance, NULL );
    if ( !m_hWnd )
    {
        DPRINT( "! Error : can't create window\n" );
        return false;
    }

    if ( bShowWindow )
    {
        ShowWindow( m_hWnd, SW_SHOW );
    }

    return true;
}

void rend_UpdateScreen()
{
    MSG umsg;

    pDev->TestCooperativeLevel();

    pDev->Present( NULL, NULL, NULL, NULL );

    sys_dwtimeCounter = timeGetTime();

    float sys_lastTimeCounter = sys_timeCounter;
    sys_timeCounter = ((float)( sys_dwtimeCounter - m_startTime ) ) * 0.001f;
    sys_timeDelta = sys_timeCounter - sys_lastTimeCounter;

    memset( &umsg, 0, sizeof( umsg ) );
    if ( PeekMessage( &umsg, m_hWnd, 0, 0, PM_REMOVE ) )
    {
        TranslateMessage( &umsg );
        DispatchMessage( &umsg );
    }
}

void rend_NewFrame
    (
    unsigned int color
    )
{
    pDev->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, color, 1.0, 0 );
}

bool rend_CreateBuffers()
{
    DPrint( ".create internal rendering buffers\n" );

    m_SpriteVB.Create( pDev, 4000, TSpriteVertex_FVF, sizeof( TSpriteVertex ) );
    m_LineVB.Create( pDev, 2048, TSpriteVertex_FVF, sizeof( TSpriteVertex ) );

    return true;
}

void rend_Start()
{
    BasedWidth_K = (float)dwWidth / (float)BasedWidth;
    BasedHeight_K = (float)dwHeight / (float)BasedHeight;
    OriginX     = dwWidth * 0.5;
    OriginY     = dwHeight * 0.5;

    m_startTime = timeGetTime();
}

XVec3 xvUpVector( 0, 1, 0 );

void rend_SetViewProj( const XVec3 camPos, const XVec3 camTgt, float camRoll,
                             float camFov, float nearP, float farP, float aspect )
{
    D3DXMatrixLookAtLH( (D3DXMATRIX *)&m_mView, (D3DXVECTOR3 *)&camPos, (D3DXVECTOR3 *)&camTgt, (D3DXVECTOR3 *)&xvUpVector );
    static XMat4 mm;
    XEuler eul( camRoll, 0, 0 );

    mm.setIdentity();
    mm = (XMat3)eul;

    m_mView *= mm;
    pDev->SetTransform( D3DTS_VIEW, (D3DMATRIX *)&m_mView );

    if ( aspect == 0 ) aspect = (float)BasedHeight / (float)BasedWidth;

    float fov = 0.5 * Deg2Rad( camFov );
    float fCos = Cos( fov );
    float fSin = Sin( fov );
    float Q = ( farP * fSin ) / ( farP - nearP );

    memset( &m_mProj, 0, sizeof( m_mProj ) );
    m_mProj.floats[ 0 ][ 0 ] = fCos * aspect;
    m_mProj.floats[ 1 ][ 1 ] = fCos;
    m_mProj.floats[ 2 ][ 2 ] = Q;
    m_mProj.floats[ 2 ][ 3 ] = fSin;
    m_mProj.floats[ 3 ][ 2 ] = -Q * nearP;

    pDev->SetTransform( D3DTS_PROJECTION, (D3DMATRIX *)&m_mProj );

    m_mTransform = m_mView * m_mProj;
}

//
// DrawSprite methods
//

int                 g_numDrawSprites = 0;
TSpriteVertex       g_drawSpriteVB[ 5000 ];

void rend_FlushSprites()
{
    unsigned int        numV = 4 * g_numDrawSprites, startV = 0;
    TSpriteVertex       *pVtx;

    pVtx = (TSpriteVertex *)m_SpriteVB.Lock( numV );
    memcpy( pVtx, g_drawSpriteVB, numV * sizeof( TSpriteVertex ) );
    m_SpriteVB.Unlock();

    pDev->SetStreamSource( 0, m_SpriteVB.GetInterface(), 0, sizeof( TSpriteVertex ) );
    pDev->SetVertexShader( NULL );
    pDev->SetFVF( TSpriteVertex_FVF );
    for ( ; startV < numV; startV += 4 )
        pDev->DrawPrimitive( D3DPT_TRIANGLEFAN, startV, 2 );

    g_numDrawSprites = 0;
}

void rend_PushDrawSprite
    (
    int nx,
    int ny,
    float z,
    int nw,
    int nh,
    float angle, DWORD color, float rhw, float texmul
    )
{
    float x = nx;
    float y = ny;
    float w = nw;
    float h = nh;

    x *= BasedWidth_K;
    y *= BasedHeight_K;
    w *= BasedWidth_K;
    h *= BasedHeight_K;
    TSpriteVertex *vtx = &g_drawSpriteVB[ 4 * g_numDrawSprites ];

    vtx[ 0 ].u = vtx[ 0 ].v = vtx[ 1 ].v = vtx[ 3 ].u = 0.f - texmul;
    vtx[ 1 ].u = vtx[ 2 ].u = vtx[ 2 ].v = vtx[ 3 ].v = 1.f + texmul;
    for ( int i = 0; i < 4; i++ )
    {
        vtx[ i ].rhw = rhw;
        vtx[ i ].sz = z;
        vtx[ i ].color = color;
    }

    float a = Atan2( h, w );
    float r = 0.5f * w / Cos( a );
    float c1 = r * Cos( a - angle );
    float c2 = r * Cos( a + angle );
    float s1 = r * Sin( a - angle );
    float s2 = r * Sin( a + angle );

    vtx[ 0 ].sx = x - c1; vtx[ 0 ].sy = y - s1;
    vtx[ 1 ].sx = x + c2; vtx[ 1 ].sy = y - s2;
    vtx[ 2 ].sx = x + c1; vtx[ 2 ].sy = y + s1;
    vtx[ 3 ].sx = x - c2; vtx[ 3 ].sy = y + s2;
    
    g_numDrawSprites++;
}

void rend_PushDraw3DSprite
    ( 
    XVec3 pos,
    short w, short h, 
    DWORD color, 
    float angle,
    char chFX )
{
    static float sx, sy, sz, sw, sh, rhw, fx, fy;

    rhw = 1.0 / ( ( pos[ 0 ] * m_mTransform.floats[ 0 ][ 3 ] ) + ( pos[ 1 ] * m_mTransform.floats[ 1 ][ 3 ] ) + ( pos[ 2 ] * m_mTransform.floats[ 2 ][ 3 ] ) + m_mTransform.floats[ 3 ][ 3 ] );
    sz = rhw * ( ( pos[ 0 ] * m_mTransform.floats[ 0 ][ 2 ] ) + ( pos[ 1 ] * m_mTransform.floats[ 1 ][ 2 ] ) + ( pos[ 2 ] * m_mTransform.floats[ 2 ][ 2 ] ) + m_mTransform.floats[ 3 ][ 2 ] );
    if ( rhw < 0 )
    {
        return;
    }
    sx = rhw * ((pos[ 0 ] * m_mTransform.floats[ 0 ][ 0 ]) + (pos[ 1 ] * m_mTransform.floats[ 1 ][ 0 ]) + (pos[ 2 ] * m_mTransform.floats[ 2 ][ 0 ]) + m_mTransform.floats[ 3 ][ 0 ]);
    sy = rhw * ((pos[ 0 ] * m_mTransform.floats[ 0 ][ 1 ]) + (pos[ 1 ] * m_mTransform.floats[ 1 ][ 1 ]) + (pos[ 2 ] * m_mTransform.floats[ 2 ][ 1 ]) + m_mTransform.floats[ 3 ][ 1 ]);
    sx = BasedOriginX * ( sx + 1 );
    sy = BasedOriginY * ( -sy + 1 );
    sw = 1 + fabs( BasedOriginX * w * rhw );
    sh = 1 + fabs( BasedOriginX * h * rhw );

    if ( !chFX )
        rend_PushDrawSprite( sx, sy, sz, sw, sh, angle, color, rhw );
    else
    {
        // do lens-flare effect

        fx = sx - BasedOriginX;
        fy = sy - BasedOriginY;

        if ( ( Fabs( fx ) > 320 ) || ( Fabs( fy ) > 240 ) )
            return;

        float rad = sqrt( fx * fx + fy * fy ) / 200;
        if ( rad > 1 ) rad = 1;
        rad = 1.f - rad;
        XVec3 col( 0.8f * rad, 0.3f * rad, 0.1f * rad );
        unsigned int mcol = 0xff000000 | ( ((int)( 255.f * col[ 0 ] ) << 16) | ((int)( 255.f * col[ 1 ] ) << 8) | (int)( 255.f * col[ 2 ] ) );

        rend_PushDrawSprite( BasedOriginX + fx * 0.6f, BasedOriginY + fy * 0.6f, 0, 90, 68, 0, mcol, 1.0f );
        rend_PushDrawSprite( BasedOriginX - fx * 0.2f, BasedOriginY - fy * 0.2f, 0, 60, 45, 0, mcol, 1.0f );
        rend_PushDrawSprite( BasedOriginX - fx * 0.6f, BasedOriginY - fy * 0.6f, 0, 110, 83, 0, mcol, 1.0f );
        rend_PushDrawSprite( BasedOriginX - fx * 1.0f, BasedOriginY - fy * 1.0f, 0, 160, 120, 0, mcol, 1.0f );

        col[ 0 ] = 0.4f * rad;
        col[ 1 ] = 0.4f * rad;
        col[ 2 ] = 0.5f * rad;
        mcol = 0xff000000 | ( ((int)( 255.f * col[ 0 ] ) << 16) | ((int)( 255.f * col[ 1 ] ) << 8) | (int)( 255.f * col[ 2 ] ) );
        
        rend_SetTexture( 0, m_pTexture[ 9 ] );
        rend_FlushSprites();

        rend_PushDrawSprite( sx, sy, 0, 260 * rad, 195 * rad, 0, mcol, 1.0f );

        rend_PushDrawSprite( sx, sy, 0, 1000 * rad, 750 * rad, 0, mcol, 1.0f );
        rend_PushDrawSprite( sx, sy, 0, 1000 * rad, 10, 0, mcol, 1.0f );
        rend_PushDrawSprite( sx, sy, 0, 1000 * rad, 10, 0.9f, mcol, 1.0f );
        rend_PushDrawSprite( sx, sy, 0, 1000 * rad, 10, -0.9f, mcol, 1.0f );
        rend_SetTexture( 0, m_pTexture[ 0 ] );
        rend_FlushSprites();
    }
}

//
// DrawLine methods
//

int                 g_numDrawLines = 0;
TSpriteVertex       g_drawLineVB[ 2048 ];

void rend_FlushLines()
{
    unsigned int        numV = 6 * g_numDrawLines, startV = 0;
    TSpriteVertex       *pVtx;

    pVtx = (TSpriteVertex *)m_LineVB.Lock( numV );
    memcpy( pVtx, g_drawLineVB, numV * sizeof( TSpriteVertex ) );
    m_LineVB.Unlock();

    pDev->SetStreamSource( 0, m_LineVB.GetInterface(), 0, sizeof( TSpriteVertex ) );
    pDev->SetVertexShader( NULL );
    pDev->SetFVF( TSpriteVertex_FVF );
    for ( ; startV < numV; startV += 6 )
        pDev->DrawPrimitive( D3DPT_TRIANGLESTRIP, startV, 4 );

    g_numDrawLines = 0;
}

void rend_PushDrawLine
    ( 
    short x1, 
    short y1, 
    short x2, 
    short y2, 
    float width, 
    DWORD color 
    )
{
    TSpriteVertex *pVtx = &g_drawLineVB[ 6 * g_numDrawLines ];
    float rx, ry, r;

    x1 *= BasedWidth_K;
    y1 *= BasedHeight_K;
    x2 *= BasedWidth_K;
    y2 *= BasedHeight_K;

    rx = y2 - y1;
    ry = x2 - x1;
    r = width / sqrtf( rx * rx + ry * ry );
    rx *= r * BasedWidth_K;
    ry *= r * BasedHeight_K;

    pVtx[ 0 ].color = pVtx[ 1 ].color = pVtx[ 4 ].color = pVtx[ 5 ].color = color & 0x00ffffff;
    pVtx[ 2 ].color = pVtx[ 3 ].color = color;
    pVtx[ 0 ].sz = pVtx[ 1 ].sz = pVtx[ 2 ].sz = pVtx[ 3 ].sz = pVtx[ 4 ].sz = pVtx[ 5 ].sz = 0;
    pVtx[ 0 ].rhw = pVtx[ 1 ].rhw = pVtx[ 2 ].rhw = pVtx[ 3 ].rhw = pVtx[ 4 ].rhw = pVtx[ 5 ].rhw = 1.0;
    
    // vtx 0
    pVtx[ 0 ].sx = x1 + rx;
    pVtx[ 0 ].sy = y1 - ry;
    // vtx 1
    pVtx[ 1 ].sx = x2 + rx;
    pVtx[ 1 ].sy = y2 - ry;
    // vtx 2
    pVtx[ 2 ].sx = x1;
    pVtx[ 2 ].sy = y1;
    // vtx 3
    pVtx[ 3 ].sx = x2;
    pVtx[ 3 ].sy = y2;
    // vtx 4
    pVtx[ 4 ].sx = x1 - rx;
    pVtx[ 4 ].sy = y1 + ry;
    // vtx 5
    pVtx[ 5 ].sx = x2 - rx;
    pVtx[ 5 ].sy = y2 + ry;

    g_numDrawLines++;
}

//
// rend_FlushLines() needed
//
void rend_PushDrawRect
    (
    short x,
    short y,
    short width, 
    short height, 
    float angle, 
    DWORD color 
    )
{
    float r;
    float a = Atan2( height, width );
    float c1, c2, s1, s2;

    r = 0.5 * width / ( Cos( a ) );
    c1 = r * Cos( a - angle );
    c2 = r * Cos( a + angle );
    s1 = r * Sin( a - angle );
    s2 = r * Sin( a + angle );

    rend_PushDrawLine( x - c1, y - s1, x + c2, y - s2, 1.5f, color );
    rend_PushDrawLine( x + c2, y - s2, x + c1, y + s1, 1.5f, color );
    rend_PushDrawLine( x + c1, y + s1, x - c2, y + s2, 1.5f, color );
    rend_PushDrawLine( x - c2, y + s2, x - c1, y - s1, 1.5f, color );
}



void rend_CreateFontBitmap()
{
    int fontSize = 60;
    char *fontWinName = "Arial";
    HFONT hf = 0;
    BITMAPINFO bmi;
    BITMAPINFOHEADER &head = bmi.bmiHeader;
    HDC dc = 0;
    HBITMAP dib = 0, oldbitmap =0;
    unsigned char *bmbits = NULL;

    DPrint( ".create system Font\n" );

    hf = 
        CreateFont ( fontSize, 0, 0, 0, 
        FW_NORMAL,
        FALSE,//( fontStyle & FONT_STYLE_ITALIC ) ? TRUE : FALSE, 
        FALSE,//( fontStyle & FONT_STYLE_UNDERLINE ) ? TRUE : FALSE,
        FALSE,//( fontStyle & FONT_STYLE_STRIKEOUT ) ? TRUE : FALSE,
        DEFAULT_CHARSET,
        OUT_TT_PRECIS,
        CLIP_DEFAULT_PRECIS,
        ANTIALIASED_QUALITY,//PROOF_QUALITY,
        DEFAULT_PITCH | FF_DONTCARE,
        fontWinName );
      

    dc = CreateCompatibleDC ( NULL );
    SetTextColor( dc, RGB ( 0xff, 0xff, 0xff ) );
    SetBkColor ( dc, 0 );
    SetBkMode( dc, TRANSPARENT );
    SelectObject ( dc, hf );

    SIZE theSize;
    GetTextExtentPoint32 ( dc, " ", 1, &theSize );

    memset( &bmi, 0, sizeof( bmi ) );
    head.biSize = sizeof ( BITMAPINFOHEADER );
    head.biWidth = 512;
    head.biHeight = 512;
    head.biPlanes = 1;
    head.biBitCount = 24;
    head.biCompression = BI_RGB;

    dib = CreateDIBSection ( dc, &bmi, DIB_RGB_COLORS, reinterpret_cast<void **>(&bmbits), NULL, 0 );
    SelectObject( dc, dib );

    RECT fmtRect;
    char s[ 2 ];
    s[ 1 ] = 0;
    int ascii = 32;
    for ( int j = 0; j < 10; j++ )
    {
        for ( int i = 0; i < 10; i++ )
        {
            fmtRect.left = i * 51;
            fmtRect.right = fmtRect.left + 51;
            fmtRect.top = j * 51 - 5;
            fmtRect.bottom = fmtRect.top + 61;
            s[ 0 ] = ascii++;
            GetTextExtentPoint32 ( dc, s, 1, &theSize );
            m_fontSizes[ ascii - 1 ] = theSize.cx;
            ::DrawText ( dc, s, 1, &fmtRect, DT_LEFT | DT_NOPREFIX | DT_TOP );
        }
    }

    // create and setup font texture
    D3DLOCKED_RECT rect;
    int offs = 0;

    pDev->CreateTexture( 512, 512, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &m_pFontTexture, NULL );
    m_pFontTexture->LockRect( 0, &rect, NULL, D3DLOCK_NOSYSLOCK );
    for ( int j = 511; j >= 0; j-- )
    {
        for ( int i = 0; i < 512; i++ )
        {
            int color = RGBA_MAKE( *( bmbits + offs++ ), *( bmbits + offs++ ), *( bmbits + offs++ ), 255 );
            *(DWORD *)((char *)rect.pBits + 4 * i + rect.Pitch * j ) = color;
        }
    }
    m_pFontTexture->UnlockRect( 0 );

    GdiFlush();

    if ( dc )
    {
        SelectObject ( dc, GetStockObject ( DEFAULT_GUI_FONT ) );
        if ( oldbitmap )
            SelectObject ( dc, oldbitmap );
    }

   if ( hf )
      DeleteObject ( hf );
   if ( dib )
      DeleteObject ( dib );
   if ( dc )
      DeleteDC ( dc );
}

float rend_PrintCharXY( int nX, int nY, char sym, int color, float zoom )
{
    float x = nX;
    float y = nY;
    x -= 20;
    zoom *= 0.5f;
    float dx = 1 + m_fontSizes[ sym ];
    float l_dx = dx;
    int dy = 51;
    static float ddu = 0.0 / 512.f;
    static float ddv = 3.0 / 512.f;
    float du = (float)( 0 + dx ) / 512.f;
    static float dv = 1.0f / 10.0f;
    sym -= 32;
    float u = ddu + ( (float)( sym % 10 ) / 10.0 );
    float v = ddv + (float)( sym / 10 ) / 10.0;
    x *= BasedWidth_K;
    y *= BasedHeight_K;
    dx *= BasedWidth_K * zoom;
    dy *= BasedHeight_K * zoom;
    TTextVertex vtx[ 4 ] = {
        { x, y, 0, 1.0, color, u, v },
        { x + dx, y, 0, 1.0, color, u + du, v },
        { x + dx, y + dy, 0, 1.0, color, u + du, v + dv },
        { x, y + dy, 0, 1.0, color, u, v + dv } };

    pDev->DrawPrimitiveUP( D3DPT_TRIANGLEFAN, 2, vtx, sizeof( TTextVertex ) );

    return ( l_dx - 1.0f ) * zoom;
}

void rend_DrawText( float x, float y, char *pStr, int color, float zoom, BYTE rfx, float localTime )
{
    float dxf, dyf, dx;

    pDev->SetTexture( 0, m_pFontTexture );
    pDev->SetVertexShader( NULL );
    pDev->SetFVF( FVF_TTextVertex );

    for ( int i = 0; i < (int)strlen( pStr ); i++ )
    {
        if ( rfx & ( RDTFX_BLURALL | RDTFX_BLURCHARS ) )
        {
            for ( int j = 0; j < 16; j++ )
            {
                if ( !j || ( rfx & RDTFX_BLURCHARS ) )
                {
                    dxf = 4 * powf( sinf( 4 * localTime ), 15 );
                    dyf = 4 * powf( cosf( 5 * localTime ), 15 );
                }
                dx = rend_PrintCharXY( x + dxf * ( j - 7.5f ), y + dyf * ( j - 7.5f ), pStr[ i ], color, zoom );
            }
            x += dx;
            if ( rfx & RDTFX_BLURCHARS )
                localTime += 1;
        }
        else
        if ( rfx & RDTFX_SINELINE )
        {
            for ( int j = 0; j < 8; j++ )
            {
                dxf = 3 * sinf( 4 * localTime + j + i );
                dyf = 3 * cosf( 4 * localTime + j + i );
                dx = rend_PrintCharXY( x + dxf, y + dyf, pStr[ i ], color, zoom  );
            }

            x += dx;
        }
        else
            x += rend_PrintCharXY( x, y, pStr[ i ], color, zoom );
    }
}

void rend_SetBlend( D3DBLEND d3drs_SRC, D3DBLEND dwdrs_DEST )
{
    pDev->SetRenderState( D3DRS_SRCBLEND, d3drs_SRC );
    pDev->SetRenderState( D3DRS_DESTBLEND, dwdrs_DEST );
}

void rend_SetRenderState( D3DRENDERSTATETYPE State, DWORD Value )
{
    pDev->SetRenderState( State, Value );
}

void rend_SetTextureStageState( DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD Value )
{
    pDev->SetTextureStageState( Stage, Type, Value );
}

void rend_SetTexture( DWORD Stage, IDirect3DBaseTexture9* pTexture )
{
    pDev->SetTexture( Stage, pTexture );
}

void rend_SetFog( float fFogStart, float fFogEnd, DWORD dwFogColor )
{
    rend_SetRenderState( D3DRS_FOGENABLE, true );
    rend_SetRenderState( D3DRS_FOGCOLOR, dwFogColor );

    rend_SetRenderState( D3DRS_FOGTABLEMODE, D3DFOG_LINEAR );
    rend_SetRenderState( D3DRS_FOGSTART, *((DWORD*) (&fFogStart)));
    rend_SetRenderState( D3DRS_FOGEND, *((DWORD*) (&fFogEnd)));
    rend_SetRenderState( D3DRS_FOGVERTEXMODE, D3DFOG_NONE );
    rend_SetRenderState( D3DRS_RANGEFOGENABLE, false );
}

void rend_SetRenderFX()
{
    BasedWidth_K = 256.f / 640.f;
    BasedHeight_K = 256.f / 480.f;
    pDev->SetRenderTarget( 0, pLFSurface );
    pDev->SetDepthStencilSurface( pLFZSurface );
    pDev->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, 0x0, 1.0, 0 );
}

void rend_RestoreRenderFX( int dwType, float fBlurPower )
{
    int i;

    BasedWidth_K = (float)dwWidth / (float)BasedWidth;
    BasedHeight_K = (float)dwHeight / (float)BasedHeight;
    pDev->SetRenderTarget( 0, m_pRenderSurface );
    pDev->SetDepthStencilSurface( m_pZRenderSurface );

    rend_SetBlend( D3DBLEND_ONE, D3DBLEND_ONE );
    rend_SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
    rend_SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE );
    rend_SetTexture( 0, pLFTexture );

    switch ( dwType )
    {
    case 1 :
        for ( i = 0; i < 8; i++ )
            rend_PushDrawSprite( 320, 240, 0, 640 + i * 16 * fBlurPower, 480 + i * 16 * fBlurPower, 0, 0xff202020 );
        break;
        
    case 2 :
        for ( i = 0; i < 8; i++ )
            rend_PushDrawSprite( 320, 240, 0, 640, 480, i * 0.025 * fBlurPower, 0xff202020 );
        break;

    case 3 :
        for ( i = 0; i < 8; i++ )
            rend_PushDrawSprite( 320 + ( i - 4.5 ) * 3 * fBlurPower, 240, 0, 640, 480, 0, 0xff202020 );
        break;

    case 4 :
        {

        char chf = 255.f - 255.f * fBlurPower;
        unsigned int color = 0xff000000 | ( chf << 16 ) | ( chf << 8 ) | chf;
        rend_PushDrawSprite( 320, 240, 0, 640, 480, 0, color, 1, 20 * pow( fBlurPower, 3 ) );
        color ^= 0xffffff;
        float ff = -0.5 * ( 1 - pow( fBlurPower, 8 ) );
        rend_PushDrawSprite( 320, 240, 0, 640, 480, 0, color, 1, ff );
        }
        break;

    case 5 :
        {
            float po = 2.f + 32.f * sinf( 0.65f * 3.14f * fBlurPower );
            float pi = 2.f * sinf( 2.f * 0.65f * 3.14f * fBlurPower );

            for ( i = 0; i < 32; i++ )
                rend_PushDrawSprite( 320 + 0.3f * po * i * ( 0.5f - fBlurPower ), 240 + pi * i, 0, 640 + i * po, 480 + 1.5f * i * po, 0, 0xff101010 );
        }
        break;

    case 6 :
        {
            pDev->Clear( 0, NULL, D3DCLEAR_TARGET, 0x0, 1.0, 0 );

            rend_SetBlend( D3DBLEND_SRCALPHA, D3DBLEND_ONE );
            int alpha = 255 - 255 * powf( fBlurPower, 1e4 );
            rend_PushDrawSprite( 
                320 + fBlurPower * ( 160 - 320 ), 
                240,
                0, 
                640 + fBlurPower * ( 260 - 640 ),
                480 + fBlurPower * ( 130 - 480 ),
                0, 0xffffff | ( alpha << 24 ) );
        }
        break;
    }

    rend_FlushSprites();

    // swap
    IDirect3DTexture9 *tmpTex;
    IDirect3DSurface9 *tmpSurf;

    tmpTex = pLFTexture;
    pLFTexture = pLFTexture2;
    pLFTexture2 = tmpTex;

    tmpSurf = pLFSurface;
    pLFSurface = pLFSurface2;
    pLFSurface2 = tmpSurf;
}

void rend_SetEnv
    (
    int nStage, 
    bool bState 
    )
{
    static XMat4 mtx;
    mtx.setIdentity();

    if ( bState )
    {
        mtx.floats[ 0 ][ 0 ] = mtx.floats[ 1 ][ 1 ] = 0.5;
        mtx.floats[ 3 ][ 0 ] = mtx.floats[ 3 ][ 1 ] = 0.5;
        rend_SetTextureStageState( nStage, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2 );
        rend_SetTextureStageState( nStage, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR );
    }
    else
    {
        rend_SetTextureStageState( nStage, D3DTSS_TEXTURETRANSFORMFLAGS, 0 );
        rend_SetTextureStageState( nStage, D3DTSS_TEXCOORDINDEX, 0 );
    }

    pDev->SetTransform( (D3DTRANSFORMSTATETYPE)(D3DTS_TEXTURE0 + nStage), (D3DMATRIX *)&mtx );
}

void rend_SetLight
    ( 
    int num, 
    const XVec3 pos, 
    const XVec3 diffuse, 
    const XVec3 specular, 
    float atten,
    float atten0
    )
{
    D3DLIGHT9 d3dLight;
    memset( &d3dLight, 0, sizeof( d3dLight ) );

    d3dLight.Type = D3DLIGHT_POINT;
    d3dLight.Diffuse.r  = diffuse[ 0 ];
    d3dLight.Diffuse.g  = diffuse[ 1 ];
    d3dLight.Diffuse.b  = diffuse[ 2 ];
    
    d3dLight.Specular.r = specular[ 0 ];
    d3dLight.Specular.g = specular[ 1 ];
    d3dLight.Specular.b = specular[ 2 ];

    d3dLight.Diffuse.a  = 1.0f;
    d3dLight.Ambient.r  = 1.0f;
    d3dLight.Ambient.g  = 1.0f;
    d3dLight.Ambient.b  = 1.0f;
    d3dLight.Ambient.a  = 1.0f;
    d3dLight.Specular.a = 1.0f;

    d3dLight.Position.x = pos[ 0 ];
    d3dLight.Position.y = pos[ 1 ];
    d3dLight.Position.z = pos[ 2 ];

    d3dLight.Attenuation0 = atten0;
    d3dLight.Attenuation2 = atten;
    d3dLight.Range      = 100000.0f;
    
    pDev->SetLight( num, &d3dLight );
    pDev->LightEnable( num, true );
}

//
// Build dword from four bytes
//
unsigned int rend_RGBA
    ( 
    register int r, 
    register int g, 
    register int b, 
    register int a 
    )
{
    return ( a << 24 ) | ( r << 16 ) | ( g << 8 ) | b;
}
