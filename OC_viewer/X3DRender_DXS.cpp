// This file is distributed under a MIT license. See LICENSE.txt for details.
//
// Working with DX shaders
//

#include "common.h"

//
// Register DX Shader
//
HXDXShader X3DRender::RegisterDXShader
    ( 
    const char *pFileName, 
    const DWORD *pDeclaration 
    )
{
    static char fileName[ XFS_MAX_FILENAME ];
    const char *pExt;
    HXFSFile fp;
    DWORD *pData;
    HRESULT rv;

    strcpy( fileName, pFileName );
    xfs->FixFileName( fileName );
    pExt = xfs->GetFileExtention( fileName );

    fp = xfs->Open( fileName );
    if ( !fp )
        return 0;

    if ( !strcmp( pExt, "vso" ) )
        m_DXShaders[ m_nNumDXShaders ].m_bIsVertexShader = true;
    else
    if ( !strcmp( pExt, "pso" ) )
        m_DXShaders[ m_nNumDXShaders ].m_bIsVertexShader = false;
    else
        return 0;

    pData = (DWORD *)ALLOCATE( xfs->SizeOf( fp ) );
    xfs->Read( fp, pData, xfs->SizeOf( fp ) );
    xfs->Close( fp );

    if ( m_DXShaders[ m_nNumDXShaders ].m_bIsVertexShader )
    {
        rv = dev->CreateVertexShader( pData, &m_DXShaders[ m_nNumDXShaders ].m_pVS );
    }
    else
    {
        rv = dev->CreatePixelShader( pData, &m_DXShaders[ m_nNumDXShaders ].m_pPS );
    }

    if ( FAILED( rv ) )
        return 0;

    FREE( pData );

    m_nNumDXShaders++;
    return m_nNumDXShaders;
}

//
// Setup given shader
//
void X3DRender::SetDXShader	
    ( 
    HXDXShader hDXShader
    )
{
    if ( !hDXShader )
        return;

    hDXShader--;

    if ( m_DXShaders[ hDXShader ].m_bIsVertexShader )
    {
        dev->SetVertexShader( m_DXShaders[ hDXShader ].m_pVS );
    }
    else
    {
        dev->SetPixelShader( m_DXShaders[ hDXShader ].m_pPS );
    }
}
