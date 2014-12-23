// This file is distributed under a MIT license. See LICENSE.txt for details.
//
// Outcast GUI by AND 2002
//

#include "common.h"

//
// Select player menu
//
class OCGUIMenu_SelectPlayer : public OCGUIMenu
{
public :

    OCGUIMenu_SelectPlayer
        (
        )
    {
        OCGUIMenu();

        float x, y;
        m_nNumItems = 22;
        m_pItems = NEW OCGUIItem[ m_nNumItems ];

        m_pItems[ 0 ].SetProperties( EOMI_CANCEL, 0, -100, 38, 38, "" );
        m_pItems[ 1 ].SetProperties( EOMI_BUTTON, 160, -160, 140, 100, "Spectator" );
        
        x = 0;
        y = 0;
        m_pItems[ 2 ].SetProperties( EOMI_BUTTON, x, y, 140, 45, "Cutter 1" ); y += 50;
        m_pItems[ 3 ].SetProperties( EOMI_BUTTON, x, y, 140, 45, "Cutter 2" ); y += 50;
        m_pItems[ 4 ].SetProperties( EOMI_BUTTON, x, y, 140, 45, "Marion" ); y += 50;
        m_pItems[ 5 ].SetProperties( EOMI_BUTTON, x, y, 140, 45, "Xue" ); y += 50;
        m_pItems[ 6 ].SetProperties( EOMI_BUTTON, x, y, 140, 45, "Primitive" ); y += 50;

        x = -160;
        y = -200;
        m_pItems[ 7 ].SetProperties( EOMI_BUTTON, x, y, 140, 45, "Oru" ); y += 50;
        m_pItems[ 8 ].SetProperties( EOMI_BUTTON, x, y, 140, 45, "Farmer" ); y += 50;
        m_pItems[ 9 ].SetProperties( EOMI_BUTTON, x, y, 140, 45, "Fat trader" ); y += 50;
        m_pItems[ 10 ].SetProperties( EOMI_BUTTON, x, y, 140, 45, "Fisher" ); y += 50;
        m_pItems[ 11 ].SetProperties( EOMI_BUTTON, x, y, 140, 45, "Slave" ); y += 50;
        m_pItems[ 12 ].SetProperties( EOMI_BUTTON, x, y, 140, 45, "Shamaz" ); y += 50;
        m_pItems[ 13 ].SetProperties( EOMI_BUTTON, x, y, 140, 45, "Reb" ); y += 50;
        m_pItems[ 14 ].SetProperties( EOMI_BUTTON, x, y, 140, 45, "Chief" ); y += 50;
        m_pItems[ 15 ].SetProperties( EOMI_BUTTON, x, y, 140, 45, "Soldier" ); y += 50;

        x = 160;
        y = -50;
        m_pItems[ 16 ].SetProperties( EOMI_BUTTON, x, y, 140, 45, "Twonha" ); y += 50;
        m_pItems[ 17 ].SetProperties( EOMI_BUTTON, x, y, 140, 45, "Aquarex" ); y += 50;
        m_pItems[ 18 ].SetProperties( EOMI_BUTTON, x, y, 140, 45, "Ventilope" ); y += 50;
        m_pItems[ 19 ].SetProperties( EOMI_BUTTON, x, y, 140, 45, "Bird" ); y += 50;
        m_pItems[ 20 ].SetProperties( EOMI_BUTTON, x, y, 140, 45, "Gamor" ); y += 50;
        m_pItems[ 21 ].SetProperties( EOMI_BUTTON, x, y, 140, 45, "Spider" ); y += 50;

        m_nDefaultItem = 0;
    }

    virtual void SelectItem();

} ocSelectPlayer;

//
// Map load menu
//
class OCGUIMenu_LoadMap : public OCGUIMenu
{
public :

    OCGUIMenu_LoadMap
        (
        )
    {
        OCGUIMenu();

        m_nNumItems = 8;
        m_pItems = NEW OCGUIItem[ m_nNumItems ];

        m_nDefaultItem = 0;
        float y = 30;
        m_pItems[ 0 ].SetProperties( EOMI_CANCEL, 0, y-200, 38, 38, "" );
        m_pItems[ 1 ].SetProperties( EOMI_BUTTON, 0, y-150, 250, 45, "Shamazaar (Temples 11.pak)" );
        m_pItems[ 2 ].SetProperties( EOMI_BUTTON, 0, y-100, 250, 45, "Okasankaar (Marches 12.pak)" );
        m_pItems[ 3 ].SetProperties( EOMI_BUTTON, 0, y-50, 250, 45, "Motazaar (Montagne 13.pak)" );
        m_pItems[ 4 ].SetProperties( EOMI_BUTTON, 0, y, 250, 45, "Talanzaar (Ville 14.pak)" );
        m_pItems[ 5 ].SetProperties( EOMI_BUTTON, 0, y+50, 250, 45, "Okaar (Foret 15.pak)" );
        m_pItems[ 6 ].SetProperties( EOMI_BUTTON, 0, y+100, 250, 45, "Ranzaar (Neige 16.pak)" );
        m_pItems[ 7 ].SetProperties( EOMI_BUTTON, 0, y+150, 250, 45, "Ranzaar2 (Burned 17.pak)" );
    }

    virtual void SelectItem();

} ocMenuLoadmap;

//
// Main menu
//
class OCGUIMenu_Main : public OCGUIMenu
{
public :

    OCGUIMenu_Main
        (
        )
    {
        OCGUIMenu();

        m_nNumItems = 5;
        m_pItems = NEW OCGUIItem[ m_nNumItems ];

        m_pItems[ 0 ].SetProperties( EOMI_BUTTON, -160, 0, 140, 60, "Quit" );
        m_pItems[ 1 ].SetProperties( EOMI_CANCEL, 0, 0, 38, 38, "" );
        m_pItems[ 2 ].SetProperties( EOMI_BUTTON, 160, 0, 140, 60, "Load map" );
        m_pItems[ 3 ].SetProperties( EOMI_BUTTON, 0, -120, 140, 60, "Select player" );
        m_pItems[ 4 ].SetProperties( EOMI_TEXT,   -240, 220, 0, 60, "Outcast Lives Forever by Dmitry \"AND\" Andreev 2002-2005" );
        m_nDefaultItem = 1;
    }

    virtual void SelectItem();

} ocMenuMain;

//
// Select items
//
extern void main_LoadMap( int nMap );

void OCGUIMenu_LoadMap::SelectItem
    (
    )
{
    OCGUIMenu::SelectItem();

    switch ( m_nCurrItem )
    {
    case 0 :
        ocgui->SetMenu( &ocMenuMain );
        break;

    default :
        ocgui->Activate( false );
        main_LoadMap( m_nCurrItem );
        ocgui->SetMenu( &ocMenuMain );
        break;
    }
}

void OCGUIMenu_Main::SelectItem
    (
    )
{
    OCGUIMenu::SelectItem();

    switch ( m_nCurrItem )
    {
    case 0 : PostMessage( x3d->m_hWnd, WM_QUIT, 0, 0 ); break;
    case 1 : ocgui->Activate( false ); break;
    case 2 : ocgui->SetMenu( &ocMenuLoadmap ); break;
    case 3 : ocgui->SetMenu( &ocSelectPlayer ); break;
    }
}

void OCGUIMenu_SelectPlayer::SelectItem
    (
    )
{
    OCGUIMenu::SelectItem();

    switch ( m_nCurrItem )
    {
    case 0 :
        ocgui->SetMenu( &ocMenuMain );
        break;

    case 1 :
        ocworld->m_b1STPerson = true;
        ocgui->SetMenu( &ocMenuMain );
        ocgui->Activate( false );
        break;

    default :
        ocworld->m_b1STPerson = false;
        ocworld->m_nActorNum = m_nCurrItem - 2;
        ocgui->SetMenu( &ocMenuMain );
        ocgui->Activate( false );
        break;
    }
}

//
// OCItem implementation
//

void OCGUIItem::SetProperties
    ( 
    int nType,
    float fX, 
    float fY, 
    float fWidth, 
    float fHeight, 
    const char *pCaption
    )
{
    m_nType = nType;
    m_fX = fX;
    m_fY = fY;
    m_fWidth = fWidth;
    m_fHeight = fHeight;
    strcpy( m_pCaption, pCaption );
}

//
// OCMenu implementation
//

//
// Constructor
//
OCGUIMenu::OCGUIMenu
    (
    )
{
    m_pItems = NULL;
    m_nNumItems = 0;
    m_nDefaultItem = 0;
    m_fMorphVel = 0;
    m_fMorphTime = 0;
}

//
// Destructor
//
OCGUIMenu::~OCGUIMenu
    (
    )
{
    if ( m_pItems )
        VDEL( m_pItems );
}

//
// Start menu
//
void OCGUIMenu::Start
    (
    )
{
    m_nCurrItem = m_nDefaultItem;
}

//
// Select item
//
void OCGUIMenu::SelectItem
    (
    )
{
}

//
// Select item by direction
//
void OCGUIMenu::SelectItem
    (
    int nDX,
    int nDY
    )
{
    int i;
    float fMinDist = 1e30f;
    OCGUIItem *ci = &m_pItems[ m_nCurrItem ];
    OCGUIItem *it;

    for ( i = 0; i < m_nNumItems; i++ )
    {
        it = &m_pItems[ i ];
        if ( it == ci )
            continue;
        if ( it->m_nType & EOMI_TEXT )
            continue;

        float dist = ( it->m_fX - ci->m_fX ) * ( it->m_fX - ci->m_fX ) + ( it->m_fY - ci->m_fY ) * ( it->m_fY - ci->m_fY );
        float cs = nDX * ( it->m_fX - ci->m_fX ) + nDY * ( it->m_fY - ci->m_fY );

        if ( cs > 0 )
        {
            dist = sqrt( dist );
            if ( dist < fMinDist )
            {
                m_nCurrItem = i;
                fMinDist = dist;
            }
        }
    }
}

//
// Select Cancel
//
void OCGUIMenu::Cancel
    (
    )
{
    int i;

    for ( i = 0; i < m_nNumItems; i++ )
        if ( m_pItems[ i ].m_nType & EOMI_CANCEL )
        {
            m_nCurrItem = i;
            SelectItem();
        }
}

//
// Update menu
//
void OCGUIMenu::Update
    (
    )
{
    m_fMorphTime += m_fMorphVel * 4.f * sys_timeDelta;
    if ( m_fMorphTime > 1 ) m_fMorphTime = 1;
    else
    if ( m_fMorphTime < 0 ) m_fMorphTime = 0;
}

//
// Render menu
//
void OCGUIMenu::Render
    (
    )
{
    int i;
    float fDX, fDY;
    float fSine = 5.f * sys_timeCounter;
    fSine = fSine - (int)fSine;
    fSine = sinf( 3.14f * fSine * fSine * fSine );
    unsigned int dwNormColor = 0x00ff7f | ( (int)( m_fMorphTime * 136.f ) << 24 );
    unsigned int dwSelcColor = 0x00ff7f | ( (int)( m_fMorphTime * ( 192.f + 63.f * fSine ) ) << 24 );

    rend_SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE );
    rend_SetRenderState( D3DRS_ALPHABLENDENABLE, true );
    rend_SetBlend( D3DBLEND_SRCALPHA, D3DBLEND_INVSRCALPHA );
    
    // draw buttons
    for ( i = 0; i < m_nNumItems; i++ )
    {
        unsigned int color;
        float dw;

        if ( m_pItems[ i ].m_nType & EOMI_TEXT )
            continue;

        if ( m_nCurrItem == i ) { color = dwSelcColor; dw = 4 - 3 * fSine; }
        else { color = dwNormColor; dw = 0; }

        fDX = m_fMorphTime * m_pItems[ i ].m_fX;
        fDY = m_fMorphTime * m_pItems[ i ].m_fY;
        ocgui->PushDrawHilight( 320.f + fDX, 240.f + fDY, ( 2.f - m_fMorphTime ) * m_pItems[ i ].m_fWidth + dw, ( 2.f - m_fMorphTime ) * m_pItems[ i ].m_fHeight + dw, color );
    }
    x3d->FlushSpriteBuffer();

    rend_SetBlend( D3DBLEND_SRCALPHA, D3DBLEND_INVSRCALPHA );
    for ( i = 0; i < m_nNumItems; i++ )
    {
        float x = 320.f + m_fMorphTime * ( m_pItems[ i ].m_fX - 0.5f * m_pItems[ i ].m_fWidth ) + 12.f;
        float y = 240.f + m_fMorphTime * ( m_pItems[ i ].m_fY - 0.5f * m_pItems[ i ].m_fHeight ) + 12.f;
        unsigned int color = dwNormColor;

        if ( m_nCurrItem == i )
            color = dwSelcColor;

        x3d->PushDraw2DText( x3d->m_hFontCommon, x, y, 1.3f, color, 0, m_pItems[ i ].m_pCaption );
    }
    x3d->FlushSpriteBuffer();

    rend_SetBlend( D3DBLEND_SRCALPHA, D3DBLEND_INVSRCALPHA );
}

//
// GUI implementation
//

//
// Constructor
//
OCGUI::OCGUI
    (
    )
{
}

//
// Destructor
//
OCGUI::~OCGUI
    (
    )
{
}

//
// Do main initialization
//
bool OCGUI::Initialize
    (
    )
{
    DPrint( ".Initialize OC GUI ..." );

    m_hBackgroundTexture = x3d->RegisterTexture( "luxsys/octitle2.jpg", ERTF_NOMIP );
    m_hHLightTexture = x3d->RegisterTexture( "luxsys/hlight32.tga" );
    m_bActivated = false;
    m_bState = false;
    m_pLastMenu = NULL;
    m_pCurrMenu = NULL;
    m_fCounter = 0.f;
    m_fCVel = 0.f;
    m_bCursorOver = false;

    DPrint( "ok\n" );

    return true;
}

//
// Update current frame
//
void OCGUI::Update
    (
    )
{
    rend_SetRenderState( D3DRS_ZENABLE, false );
    rend_SetRenderState( D3DRS_FOGENABLE, false );
    rend_SetRenderState( D3DRS_ALPHABLENDENABLE, true );
    rend_SetBlend( D3DBLEND_SRCALPHA, D3DBLEND_INVSRCALPHA );
    if ( !ocworld->IsLoaded() )
    {
        x3d->PushDraw2DSprite( m_hBackgroundTexture, 0, 0, 640, 480, 0xffffffff, EPDF_NOCENTER, 0, NULL );
        x3d->FlushSpriteBuffer();
    }

    if ( !IsActivated() )
        return;

    m_fCounter += 8.f * m_fCVel * sys_timeDelta;
    if ( m_fCounter > 1 ) m_fCounter = 1;
    if ( m_fCounter < 0 )
    {
        m_bActivated = false;
        m_fCVel = 0.f;
        m_fCounter = 0;
        return;
    }

    // fade screen
    {
        x3d->dev->SetTexture( 0, NULL );
        x3d->PushDraw2DSprite( 0, 0, 0, 640, 480, 0x002000 | ( (int)(170.f * m_fCounter) << 24 ), EPDF_NOCENTER, 0, NULL );
        x3d->FlushSpriteBuffer();
    }

    if ( m_pLastMenu )
    {
        m_pLastMenu->Update();
        if ( m_pLastMenu->m_fMorphTime > 0 )
            m_pLastMenu->Render();
    }

    if ( m_pCurrMenu )
    {
        m_pCurrMenu->Update();
        if ( m_pCurrMenu->m_fMorphTime > 0 )
            m_pCurrMenu->Render();
    }
}

//
// Process window messages
//
bool OCGUI::ProcessMessage
    (
    HWND hWnd,
    UINT msg, 
    WPARAM wParam, 
    LPARAM lParam
    )
{
    if ( !m_bState )
        return true;

    int nDX = 0;
    int nDY = 0;

    if ( msg == WM_KEYDOWN )
    {
        if ( IsActivated() )
        {
            if ( (int)wParam == VK_LEFT )
                nDX = -1;
            if ( (int)wParam == VK_RIGHT )
                nDX = 1;
            if ( (int)wParam == VK_UP )
                nDY = -1;
            if ( (int)wParam == VK_DOWN )
                nDY = 1;
            
            m_pCurrMenu->SelectItem( nDX, nDY );

            switch ( (int)wParam )
            {
            case VK_LBUTTON :
            case VK_RETURN :
            case VK_CONTROL :
                
                m_pCurrMenu->SelectItem();
            }
        }

        if ( (int)wParam == VK_ESCAPE )
        {
            if ( !IsActivated() )
            {
                Activate( true );
            }
            else
                m_pCurrMenu->Cancel();
        }
    }
    else
    if ( IsActivated() && ( msg == WM_MOUSEMOVE || msg == WM_LBUTTONUP || msg == WM_LBUTTONDOWN ) )
    {
        int mX = LOWORD( lParam );
        int mY = HIWORD( lParam );

        float fX = ( (float)mX / BasedWidth_K ) - 320;
        float fY = ( (float)mY / BasedHeight_K ) - 240;

        m_bCursorOver = false;

        int i;
        for ( i = 0; i < m_pCurrMenu->m_nNumItems; i++ )
        {
            OCGUIItem *ci = &m_pCurrMenu->m_pItems[ i ];

            float ox = ci->m_fX - ci->m_fWidth * 0.5f;
            float oy = ci->m_fY - ci->m_fHeight * 0.5f;

            if ( ( fX > ox && fX < ( ox + ci->m_fWidth ) ) &&
                 ( fY > oy && fY < ( oy + ci->m_fHeight ) ) )
            {
                m_bCursorOver = true;

                if ( hWnd ) // this is the real message
                {
                    m_pCurrMenu->m_nCurrItem = i;

                    if ( msg == WM_LBUTTONUP )
                    {
                        m_pCurrMenu->SelectItem();
                    }
                }

                break;
            }
        }
    }

    return true;
}

//
// Set active menu
//
void OCGUI::SetMenu
    ( 
    OCGUIMenu *pMenu
    )
{
    if ( !pMenu )
        pMenu = &ocMenuLoadmap;

    if ( m_pCurrMenu )
    {
        m_pCurrMenu->m_fMorphVel = -1;
    }

    m_pLastMenu = m_pCurrMenu;
    m_pCurrMenu = pMenu;
    m_pCurrMenu->Start();
    m_pCurrMenu->m_fMorphVel = 1;

    UpdateCursor();
}

void OCGUI::UpdateCursor
    (
    )
{
    POINT pnt;
    RECT rc;
    GetCursorPos( &pnt );
    GetClientRect( x3d->m_hWnd, &rc );
    pnt.x -= rc.left;
    pnt.y -= rc.top;
    OCGUI::ProcessMessage( NULL, WM_MOUSEMOVE, 0, pnt.x | ( pnt.y << 16 ) );
}

//
// Activate GUI
//
void OCGUI::Activate
    ( 
    bool bState 
    )
{
    if ( bState )
    {
        m_fCVel = 1.f;
        m_bActivated = bState;

        if ( m_pCurrMenu )
        {
            m_pCurrMenu->m_fMorphVel = 1;
            m_pCurrMenu->m_fMorphTime = 0;
        }

        // disable OCInput
        ocinput->Enable( false );
        UpdateCursor();
    }
    else
    {
        m_fCVel = -1.f;

        if ( m_pCurrMenu )
        {
            m_pCurrMenu->m_fMorphVel = -1;
        }

        if ( m_pLastMenu )
        {
            m_pLastMenu->m_fMorphTime = 0;
            m_pLastMenu->m_fMorphVel = 0;
        }

        // enable OCInput
        ocinput->Enable( true );
        SetCursor( NULL );
    }
}

//
// Push draw specified rect
//
void OCGUI::PushDrawHilight
    ( 
    float x, 
    float y, 
    float w, 
    float h, 
    unsigned int color
    )
{
    static float tex0[ 4 ] = { 0, 0, 0.5f, 0.5f };
    static float tex1[ 4 ] = { 0.5f, 0, 1.f, 0.5f };
    static float tex2[ 4 ] = { 0, 0.5f, 0.5f, 1.f };
    static float tex3[ 4 ] = { 0.5f, 0.5f, 1.f, 1.f };
    static float tex4[ 4 ] = { 0.5f, 0, 0.5f, 0.5f };
    static float tex5[ 4 ] = { 0.5f, 0.5f, 0.5f, 1.f };
    static float tex6[ 4 ] = { 0, 0.5f, 0.5f, 0.5f };
    static float tex7[ 4 ] = { 0.5f, 0.5f, 1.f, 0.5f };
    float sx, sy, sw;

    sx = x - 0.5f * w + 8.f;
    sy = y - 0.5f * h + 8.f;
    x3d->PushDraw2DSprite( m_hHLightTexture, sx, sy, 16, 16, color, 0, 0, tex0 );

    sx = x + 0.5f * w - 8.f;
    sy = y - 0.5f * h + 8.f;
    x3d->PushDraw2DSprite( m_hHLightTexture, sx, sy, 16, 16, color, 0, 0, tex1 );

    sx = x - 0.5f * w + 8.f;
    sy = y + 0.5f * h - 8.f;
    x3d->PushDraw2DSprite( m_hHLightTexture, sx, sy, 16, 16, color, 0, 0, tex2 );

    sx = x + 0.5f * w - 8.f;
    sy = y + 0.5f * h - 8.f;
    x3d->PushDraw2DSprite( m_hHLightTexture, sx, sy, 16, 16, color, 0, 0, tex3 );

    // draw horizontal
    sy = y - 0.5f * h + 8.f;
    sw = w - 32.f;
    if ( sw > 0 )
    {
        x3d->PushDraw2DSprite( m_hHLightTexture, x, sy, sw, 16, color, 0, 0, tex4 );
        sy = y + 0.5f * h - 8.f;
        x3d->PushDraw2DSprite( m_hHLightTexture, x, sy, sw, 16, color, 0, 0, tex5 );
    }

    // draw vertical
    sx = x - 0.5f * w + 8.f;
    sw = h - 32.f;
    if ( sw > 0 )
    {
        x3d->PushDraw2DSprite( m_hHLightTexture, sx, y, 16, sw, color, 0, 0, tex6 );
        sx = x + 0.5f * w - 8.f;
        x3d->PushDraw2DSprite( m_hHLightTexture, sx, y, 16, sw, color, 0, 0, tex7 );
    }
}
