// This file is distributed under a MIT license. See LICENSE.txt for details.
//
// Index and Vertex buffer wrappers
//

#include "common.h"

void CIBuffer::Create
    ( 
    IDirect3DDevice9 *pDev, 
    const unsigned int dwCount 
    )
{
    m_dwCount = dwCount;
    memset( &m_Desc, 0, sizeof( m_Desc ) );
    m_Desc.Format = D3DFMT_INDEX16;
    m_Desc.Size = 2 * m_dwCount;
    m_Desc.Type = D3DRTYPE_INDEXBUFFER;
    m_Desc.Pool = D3DPOOL_DEFAULT;
    m_Desc.Usage = D3DUSAGE_WRITEONLY;

    HRESULT hr = pDev->CreateIndexBuffer( m_dwCount * 2, m_Desc.Usage, m_Desc.Format, m_Desc.Pool, &m_pInterface, NULL );

    if ( hr != D3D_OK )
    {
    }
}

void *CIBuffer::Lock
    ( 
    const unsigned int dwCount 
    )
{
    unsigned short *pLockedData;

    HRESULT hr = m_pInterface->Lock( 0, dwCount * 2, reinterpret_cast< void ** >( &pLockedData ), 0 );

    if ( hr != D3D_OK )
    {
    }

    return pLockedData;
}

void CVBuffer::Create
    ( 
    IDirect3DDevice9 *pDev, 
    const unsigned int dwCount, 
    const unsigned int dwFVF,
    const unsigned int dwSize,
    bool bIsDynamic
    )
{
    m_bDynamic = bIsDynamic;
    m_dwCount = dwCount;
    m_dwSize = dwSize;
    memset( &m_Desc, 0, sizeof( m_Desc ) );
    m_Desc.Format = D3DFMT_VERTEXDATA;
    m_Desc.Size = m_dwCount * m_dwSize;
    m_Desc.Type = D3DRTYPE_VERTEXBUFFER;
    m_Desc.Pool = D3DPOOL_DEFAULT;
    m_Desc.FVF = dwFVF;
    m_Desc.Usage = D3DUSAGE_WRITEONLY | ( m_bDynamic ? D3DUSAGE_DYNAMIC : 0 );

    if ( m_bDynamic )
    {
        m_Desc.Pool = D3DPOOL_SYSTEMMEM;
    }

    HRESULT hr = pDev->CreateVertexBuffer( dwCount * m_dwSize, m_Desc.Usage, m_Desc.FVF, m_Desc.Pool, &m_pInterface, NULL );

    if ( hr != D3D_OK )
    {
    }
}

void *CVBuffer::Lock
    ( 
    const unsigned int dwCount 
    )
{
    unsigned short *pLockedData;
    unsigned int dwFlags = D3DLOCK_NOSYSLOCK;

    if ( m_bDynamic )
        dwFlags = D3DLOCK_DISCARD;

    HRESULT hr = m_pInterface->Lock( 0, dwCount * m_dwSize, reinterpret_cast< void ** >( &pLockedData ), dwFlags );

    if ( hr != D3D_OK )
    {
    }

    return pLockedData;
}
