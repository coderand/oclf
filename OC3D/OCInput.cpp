// This file is distributed under a MIT license. See LICENSE.txt for details.
//
// Outcast input system by AND 2002
//

#include "CRenderer.h"
#include "common.h"
#include "ocinput.h"

//
// Constructor
//
OCInput::OCInput
    (
    )
{
    m_pDI = NULL;
    m_pMouse = NULL;
    m_pKeyboard = NULL;
    m_bState = true;
}

//
// Destructor
//
OCInput::~OCInput
    (
    )
{
}

//
// Initialize input system
//
bool OCInput::Init
    (
    HINSTANCE hInstance,
    HWND hWnd
    )
{
    DPrint( "OCInput : init DirectInput\n" );

    DirectInput8Create( hInstance, DIRECTINPUT_VERSION,
                       IID_IDirectInput8, (void **)&m_pDI, NULL );

    DPrint( "OCInput : create mouse device\n" );
    m_pDI->CreateDevice( GUID_SysMouse, &m_pMouse, NULL );
    m_pMouse->SetDataFormat( &c_dfDIMouse2 );
    m_pMouse->SetCooperativeLevel( hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND );
    m_pMouse->Acquire();

    DPrint( "OCInput : create keyboard device\n" );
    m_pDI->CreateDevice( GUID_SysKeyboard, &m_pKeyboard, NULL );
    m_pKeyboard->SetDataFormat( &c_dfDIKeyboard );
    m_pKeyboard->SetCooperativeLevel( hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND );
    m_pKeyboard->Acquire();

    return true;
}

//
// Release input system
//
void OCInput::Release
    (
    )
{
    m_pKeyboard->Unacquire();
    m_pMouse->Unacquire();
    RELEASE( m_pKeyboard );
    RELEASE( m_pMouse );
    RELEASE( m_pDI );
}

//
// Update input system
//
void OCInput::Update
    (
    )
{
    HRESULT hr;
    DIMOUSESTATE2 dims2;

    memset( &dims2, 0, sizeof( dims2 ) );
    memset( m_bKeyBuf, 0, OCINPUT_KETBUFFERSIZE );
    m_xvMouse.xyz( 0, 0, 0 );

    if ( !m_bState )
        return;

    hr = m_pMouse->GetDeviceState( sizeof( dims2 ), &dims2 );
    if ( FAILED( hr ) )
    {
        hr = m_pMouse->Acquire();
        while ( hr == DIERR_INPUTLOST )
            hr = m_pMouse->Acquire();
    }
    m_xvMouse.x = (float)dims2.lX;
    m_xvMouse.y = (float)dims2.lY;
    m_xvMouse.z = (float)dims2.lZ;
    m_bButton[ 0 ] = dims2.rgbButtons[ 0 ] > 0;
    m_bButton[ 1 ] = dims2.rgbButtons[ 1 ] > 0;
    m_bButton[ 2 ] = dims2.rgbButtons[ 2 ] > 0;

    hr = m_pKeyboard->GetDeviceState( OCINPUT_KETBUFFERSIZE, (void *)m_bKeyBuf );
    if ( FAILED( hr ) )
    {
        hr = m_pKeyboard->Acquire();
        while ( hr == DIERR_INPUTLOST )
            hr = m_pKeyboard->Acquire();
    }
}

//
// Enable/Disable input
//
void OCInput::Enable
    ( 
    bool bState
    )
{
    m_bState = bState;

    if ( bState )
    {
        Update();
    }
    else
    {
    }
}

//
// Test key
//
bool OCInput::TestKey
    ( 
    int nKey 
    )
{
    return ( m_bKeyBuf[ nKey ] & 0x80 ) > 0;
}
