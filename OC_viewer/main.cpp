// This file is distributed under a MIT license. See LICENSE.txt for details.
//
// Outcast land viewer in 3D
//

#include "common.h"
#include "CRenderer.h"
#include "oc3d.h"
#include "ocinput.h"

bool            g_bRun = false;
HWND            g_hWnd;
HINSTANCE       g_hInstance;
unsigned int    dwFogColor;

XFileSystem     *xfs;
X3DRender       *x3d;

OCInput         *ocinput = NULL;
OCWorld         *ocworld;
OCGUI           *ocgui = NULL;

OCDrivenPlayer  *player;

void main_LoadMap
    (
    int nMap
    )
{
    nMap = 1 + ( ( nMap - 1 ) % 7 );
    char *pMapNames[ 7 ] =
    {
        "11/temples",
        "12/marches",
        "13/montagne",
        "14/ville",
        "15/foret",
        "16/neige",
        "17/burned"
    };
    unsigned int dwFogColors[ 7 ] =
    {
        0x979B94,
        0x696B64,
        0xA38050,
        0xB1A073,
        0x837753,
        0x95A594,
        0x95A594
    };
    float fFogUp[ 7 ] = { 470,  430, 700, 360, 790, 500, 500 };
    float fFogDn[ 7 ] = {   0,  -60, 100, -160, 160,  50,  50 };

    ocgui->Enable( false );

    ocworld->ShutDown();
    ocworld->LoadMap( pMapNames[ nMap - 1 ] );
    dwFogColor = dwFogColors[ nMap - 1 ];
    ocworld->m_dwFogColor = dwFogColor;
    ocworld->m_fFogUp = fFogUp[ nMap - 1 ];
    ocworld->m_fFogDown = fFogDn[ nMap - 1 ];

    player->m_xvPos.x = (float)ocworld->m_nMapWidth * 64.f;
    player->m_xvPos.z = (float)ocworld->m_nMapHeight * 64.f;
    player->m_xvPos.y = ocworld->m_pTilesMap[ ocworld->m_nMapWidth / 2 + ocworld->m_nMapWidth * ( ocworld->m_nMapHeight / 2 ) ].m_xvPos.y + 128.f;

    ocgui->Enable( true );
}

LRESULT WINAPI rend_WndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{

    switch ( msg )
    {
    case WM_CREATE :
        g_hWnd = hWnd;

        SendMessage( hWnd, WM_SETICON, ICON_BIG, (LPARAM)LoadIcon( g_hInstance, MAKEINTRESOURCE( IDI_ICON1 ) ) );

        SetTimer( hWnd, 0, 15, NULL );
        break;

    case WM_TIMER :
        if ( ocgui && ocinput && !ocinput->IsEnabled() )
            PostMessage( hWnd, WM_SETCURSOR, 0, 0 );

        break;

    case WM_SETCURSOR :
        if ( ocgui )
        {
            if ( ocinput && !ocinput->IsEnabled() )
            {
                int arr[] = { IDC_CURSOR2, IDC_CURSOR1, IDC_CURSOR3, IDC_CURSOR4, IDC_CURSOR4, IDC_CURSOR3, IDC_CURSOR1, IDC_CURSOR2 };
                float t = fmodf( 0.5f + 5.f * sys_timeCounter, 1 ); 
                int fr = (int)( t * ( sizeof( arr ) / 4 ) ) % ( sizeof( arr ) / 4 );

                if ( !ocgui->m_bCursorOver )
                    SetCursor( LoadCursor( g_hInstance, MAKEINTRESOURCE( IDC_CURSOR1 ) ) );
                else
                    SetCursor( LoadCursor( g_hInstance, MAKEINTRESOURCE( arr[ fr ] ) ) );
            }
            else
                SetCursor( NULL );
        }
        else
            SetCursor( NULL );
        break;
    case WM_LBUTTONUP :
    case WM_LBUTTONDOWN :
    case WM_RBUTTONUP :
    case WM_MOUSEMOVE :
        if ( ocgui )
            ocgui->ProcessMessage( hWnd, msg, wParam, lParam );
        break;
    case WM_KEYDOWN:

        if ( ocgui )
            ocgui->ProcessMessage( hWnd, msg, wParam, lParam );

        if ( (int)wParam == VK_F12 )
            PostMessage( hWnd, WM_QUIT, 0, 0 );
        break;
    case WM_CLOSE:
    case WM_QUIT:
        g_bRun = false;
        break;
    default:
        return DefWindowProc( hWnd, msg, wParam, lParam );
    }

    return 0;
}

bool CreateInterfaces
    (
    )
{
    xfs = NEW XFileSystem;
    x3d = NEW X3DRender;

    xfs->Initialize();
    XFS_EXT_Register_OCPak();
    xfs->ScaningCntFiles();

    x3d->dev = pDev;
    x3d->m_hWnd = g_hWnd;
    if ( !x3d->Initialize() )
        return false;

    return true;
}

void DeleteInterfaces
    (
    )
{
    DEL( x3d );
    DEL( xfs );
}

int __stdcall WinMain
    ( 
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nShowCmd
    )
{
    g_hInstance = hInstance;

    DINIT();
    rend_Build();

    if ( !rend_Init( hInstance ) )
        return false;
    
    rend_Start();
    rend_CreateFontBitmap();

    //
    // Create base OC classes
    //
    if ( CreateInterfaces() )
    {

    ocinput = NEW OCInput;
    ocworld = NEW OCWorld;
    ocgui = NEW OCGUI;
    ocgui->Initialize();

    player = NEW OCDrivenPlayer;
    ocworld->m_pPlayer = player;

    // load a map
    ocinput->Init( hInstance, GetForegroundWindow() );
    
    ocgui->SetMenu();
    ocgui->Activate( true );
    ocgui->Enable( true );

    g_bRun = true;
    while( g_bRun )
    {
        ocinput->Update();
        rend_NewFrame( dwFogColor );

        //
        // do OC world rendering
        //
        ocworld->Update();

        pDev->BeginScene();
        ocworld->Render();

        ocgui->Update();

        //
        // End a scene and update a screen
        //
        pDev->EndScene();
        rend_UpdateScreen();
    }

    ocgui->Enable( false );
    ocinput->Release();
    rend_Kill();

    DEL( ocinput );
    DEL( ocgui );
    DEL( ocworld );
    DEL( player );

    }

    DeleteInterfaces();

    return 0;
}
