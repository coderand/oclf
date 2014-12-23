// This file is distributed under a MIT license. See LICENSE.txt for details.
//
// ANM file
//

#include "common.h"

//
// Constructor
//
COCAnimation::COCAnimation
    (
    )
{
    memset( &m_Header, 0, sizeof( m_Header ) );
    m_pJoints = NULL;
}

//
// Destructor
//
COCAnimation::~COCAnimation	
    (
    )
{
    if ( m_pJoints )
    {
        VDEL( m_pJoints );
        m_pJoints = NULL;
    }
}

//
// Load file from memory
//
dword COCAnimation::LoadFromMemory
    (
    const void *pData,
    int size
    )
{
    int i, j;
    int crd;
    dword offs;
    const char *ptr = (const char *)pData;

    m_Header = *(TANMHeader *)ptr;
    ptr += sizeof( TANMHeader );

    m_pJoints = NEW TANMJoint[ m_Header.wNumJoints ];

    for ( i = 0; i < m_Header.wNumJoints; i++ )
    {
        m_pJoints[ i ].wIndex = *(word *)ptr; ptr += 2;
        m_pJoints[ i ].wMask = *(word *)ptr; ptr += 2;

        for ( crd = 0; crd < 3; crd++ )
        {
            word fox = *(word *)ptr; ptr += 2;
            word oxs = *(word *)ptr; ptr += 2;
            offs = *(dword *)ptr; ptr += 4;

            m_pJoints[ i ].fo[ crd ] = fox;
            m_pJoints[ i ].os[ crd ] = oxs;

            m_pJoints[ i ].pwO[ crd ] = NEW short[ oxs ];
            const char *oldptr = ptr;
            ptr = (const char *)pData + offs + size - m_Header.dwLength;
            for ( j = 0; j < oxs; j++ )
            {
                m_pJoints[ i ].pwO[ crd ][ j ] = *(short *)ptr; ptr += 2;
            }
            ptr = oldptr;
        }

        for ( crd = 0; crd < 3; crd++ )
        {
            word fox = *(word *)ptr; ptr += 2;
            word oxs = *(word *)ptr; ptr += 2;
            offs = *(dword *)ptr; ptr += 4;

            m_pJoints[ i ].fd[ crd ] = fox;
            m_pJoints[ i ].ds[ crd ] = oxs;

            m_pJoints[ i ].pfD[ crd ] = NEW float[ oxs ];
            const char *oldptr = ptr;
            ptr = (const char *)pData + offs + size - m_Header.dwLength;
            for ( j = 0; j < oxs; j++ )
            {
                m_pJoints[ i ].pfD[ crd ][ j ] = *(float *)ptr; ptr += 4;
            }
            ptr = oldptr;
        }
    }

    return 0;
}
