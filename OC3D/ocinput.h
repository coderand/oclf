// This file is distributed under a MIT license. See LICENSE.txt for details.
//
// Outcast input system by AND 2002
//

#ifndef __OCINPUT_H__
#define __OCINPUT_H__

#include <dinput.h>

#define OCINPUT_KETBUFFERSIZE   256
#define OC_LBUTTON              0
#define OC_MBUTTON              1
#define OC_RBUTTON              2

//
// DirectX base input system
//
class OCInput
{
protected :

    bool            m_bState;

IDirectInput8       *m_pDI;
IDirectInputDevice8 *m_pMouse;
IDirectInputDevice8 *m_pKeyboard;

    bool            m_bButton[ 3 ];
    XVec3           m_xvMouse;
    char            m_bKeyBuf[ OCINPUT_KETBUFFERSIZE ];

public :

    bool            Init                ( HINSTANCE hInstance, HWND hWnd );
    void            Release             ();
    void            Update              ();
    void            Enable              ( bool bState );
    bool            IsEnabled           () { return m_bState; };

    bool            GetMouseButton      ( int nButtonNum ) { return m_bButton[ nButtonNum ]; };
    XVec3           GetMouseDelta       () { return m_xvMouse; };
    bool            TestKey             ( int nKey );

                    OCInput             ();
                    ~OCInput            ();
};

#endif // __OCINPUT_H__
