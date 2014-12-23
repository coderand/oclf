// This file is distributed under a MIT license. See LICENSE.txt for details.
//
// Index and Vertex buffer wrappers
//

#ifndef __IDXVTXBUFS_H__
#define __IDXVTXBUFS_H__

#include <d3d9.h>

//
// 16bit Index buffer
//
class CIBuffer
{
private :

    unsigned int            m_dwCount;
    IDirect3DIndexBuffer9   *m_pInterface;
    D3DINDEXBUFFER_DESC     m_Desc;

public :

    inline IDirect3DIndexBuffer9 *GetInterface() { return m_pInterface; };
    void        Create      ( IDirect3DDevice9 *pDev, const unsigned int dwCount );
    void        *Lock       ( const unsigned int dwCount );
    inline void Unlock      () { m_pInterface->Unlock(); };
    inline void Release     () { RELEASE( m_pInterface ); };

                CIBuffer    () : m_pInterface( NULL ) {};
                ~CIBuffer   () { RELEASE( m_pInterface ); };
};

//
// Vertex buffer
//
class CVBuffer
{
private :

    bool                    m_bDynamic;
    unsigned int            m_dwCount;
    IDirect3DVertexBuffer9  *m_pInterface;
    D3DVERTEXBUFFER_DESC    m_Desc;
    unsigned int            m_dwSize;

public :

    inline IDirect3DVertexBuffer9 *GetInterface() { return m_pInterface; };
    void        Create      ( IDirect3DDevice9 *pDev, const unsigned int dwCount, const unsigned int dwFVF, const unsigned int dwSize, bool bIsDynamic = false );
    void        *Lock       ( const unsigned int dwCount );
    inline void Unlock      () { m_pInterface->Unlock(); };
    inline void Release     () { RELEASE( m_pInterface ); };

                CVBuffer    () : m_pInterface( NULL ) {};
                ~CVBuffer   () { RELEASE( m_pInterface ); };
};

#endif // __IDXVTXBUFS_H__
