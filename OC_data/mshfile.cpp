// This file is distributed under a MIT license. See LICENSE.txt for details.
//
// MSH file
//

#include "common.h"

COCMesh *backup = NULL;

//
// Constructor
//
COCMesh::COCMesh
    (
    )
{
    m_pHeader = NULL;
    m_pObjHeader = NULL;
    m_pVerts = NULL;
    m_pPolygons = NULL;
    m_pPolyInfo = NULL;
    m_pPolyInfo2 = NULL;
    m_pVertNormals = NULL;
    m_pPolyNormals = NULL;
    m_pTexCoords = NULL;
    m_pTexinfos = NULL;
    m_nNumTexInfos = NULL;
    m_pClusters = NULL;
    m_pLimbs = NULL;
    m_pJoints = NULL;
    m_pTexinfos = NULL;
}

//
// Destructor
//
COCMesh::~COCMesh
    (
    )
{
    if ( m_pLimbs )
    {
        VDEL( m_pLimbs );
        m_pLimbs = NULL;
    }

    if ( m_pJoints )
    {
        VDEL( m_pJoints );
        m_pJoints = NULL;
    }

    VDEL( m_pTexinfos );
}

//
// Load mesh from file in memory
//
dword COCMesh::LoadFromMemory
    ( 
    const void *pData 
    )
{
    int i, j;
    char *ptr;
    dword currLod = 0;

    m_pHeader = (TMSHHeader *)pData;
    m_pObjHeader = (TMSHObjHeader *)( (char *)m_pHeader + m_pHeader->dwObjectOffset );

    ptr = (char *)m_pObjHeader + sizeof( TMSHObjHeader );

    if ( m_pHeader->dwTypeID == OC_FILEID_AMH )
    {
        backup = (COCMesh *)ALLOCATE( sizeof( COCMesh ) );
    }

    while ( 1 )
    {
        ptr += 8;/*unknown*/

        m_pVerts = (TMSHVertexArray	*)ptr;
        ptr += sizeof( int ) + m_pVerts->m_nNumVerts * sizeof( TMSHVertex );

        m_pPolygons = (TMSHPolygonArray *)ptr;
        ptr += sizeof( int ) + m_pPolygons->m_nNumPolygons * sizeof( TMSHPolygon );

        m_pPolyInfo = (TMSHPolyInfoArray *)ptr;
        ptr += 4/*lost chunk*/ + sizeof( int ) + m_pPolyInfo->m_nNumPolygons * sizeof( TMSHPolyInfo );

        m_pVertNormals = (TMSHVNormalArray *)ptr;
        ptr += sizeof( int ) + m_pVertNormals->m_nNumVerts * sizeof( TMSHNormal );

        m_pPolyNormals = (TMSHFNormalArray *)ptr;
        ptr += 4/*lost chunk*/ + sizeof( int ) + m_pPolyNormals->m_nNumPolygons * sizeof( TMSHNormal );

        m_pTexCoords = (TMSHTexCoordArray *)ptr;
        ptr += sizeof( int ) + 3/*3 vectors per face*/ * m_pTexCoords->m_nNumPolygons * sizeof( TMSHTextureCoord );

        // convert texcoords
        for ( i = 0; i < m_pTexCoords->m_nNumPolygons; i++ )
            for ( j = 0; j < 3; j++ )
            {
                word *ptr = (word *)&m_pTexCoords->m_Coords[ i ][ j ];
                float us = (float)((short)ptr[ 1 ]);
                float vs = (float)((short)ptr[ 3 ]);
                m_pTexCoords->m_Coords[ i ][ j ].u = ( us + ( (float)ptr[ 0 ] / 65535.f ) );
                m_pTexCoords->m_Coords[ i ][ j ].v = -( vs + ( (float)ptr[ 2 ] / 65535.f ) );
            }

        m_nNumTexInfos = *(int *)ptr;
        ptr += sizeof( int );
        m_pTexinfos = NEW TMSHTextureInfo[ m_nNumTexInfos ];
        for ( i = 0; i < m_nNumTexInfos; i++ )
        {
            m_pTexinfos[ i ].dwUnknown01 = *(dword *)ptr;
            ptr += sizeof( dword );

            for ( j = 0; j < 4; j++ )
            {
                m_pTexinfos[ i ].pFileNames[ j ] = (COCStr *)ptr;
                ptr += sizeof( int ) + m_pTexinfos[ i ].pFileNames[ j ]->m_nLength;
            }

            memcpy( m_pTexinfos[ i ].sUnknown02, ptr, 12 );
            ptr += 12;
        }

        if ( m_pHeader->dwTypeID == OC_FILEID_AMH )
        {
            m_nNumClusters = *(int *)ptr;
            ptr += 4;

            m_pClusters = (TAMHCluster *)ptr;
            ptr += m_nNumClusters * sizeof( TAMHCluster );

            m_nNumLimbs = *(int *)ptr;
            ptr += 4;

            m_pLimbs = NEW TAMHJoint[ m_nNumLimbs ];
            int i;
            for ( i = 0; i < m_nNumLimbs; i++ )
            {
                m_pLimbs[ i ].dwKind = *(dword *)ptr; ptr += 4;
                m_pLimbs[ i ].dwClusterCount = *(dword *)ptr; ptr += 4;
                m_pLimbs[ i ].dwClusterIndex = *(dword *)ptr; ptr += 4;
                m_pLimbs[ i ].dwPolyIndex = *(dword *)ptr; ptr += 4;
                m_pLimbs[ i ].xvPos = *(XVec3 *)ptr; ptr += sizeof( XVec3 );

                if ( 1 == m_pLimbs[ i ].dwKind >> 1 )
                {
                    m_pLimbs[ i ].mRot = *(XMat3 *)ptr; ptr += sizeof( XMat3 );
                    m_pLimbs[ i ].xvPivot = *(XVec3 *)ptr; ptr += sizeof( XVec3 );
                }
            }

            COCStr *pPath = (COCStr *)ptr;
            ptr += 4 + pPath->m_nLength;

            int c = *(int *)ptr;
            ptr += 4;
            ptr += sizeof( float ) * c;

            m_pPolyInfo2 = (TMSHPolyInfoArray2 *)ptr;
            ptr += sizeof( int ) + m_pPolyInfo2->m_nNumPolygons * sizeof( TMSHPolyInfo2 );

            if ( ( currLod == 0 ) && ( m_pObjHeader->dwNumLOD > 1 ) )
            {
                memcpy( backup, this, sizeof( COCMesh ) );
            }
            
            if ( currLod && m_pLimbs )
            {
                VDEL( m_pLimbs );
                m_pLimbs = NULL;
                VDEL( m_pTexinfos );
            }

            currLod++;
            if ( currLod < m_pObjHeader->dwNumLOD )
                continue;
            else
            {
                if ( ( m_pObjHeader->dwNumLOD > 1 ) )
                {
                    memcpy( this, backup, sizeof( COCMesh ) );
                    backup->m_pLimbs = NULL;
                }

                break;
            }
        }

        break;
    }

    if ( m_pHeader->dwTypeID == OC_FILEID_AMH )
    {
        FREE( backup );

        m_nNumJoints = *(int *)ptr;
        ptr += 4;
        int i;

        m_pJoints = NEW TAMHModelJoint[ m_nNumJoints ];
        for ( i = 0; i < m_nNumJoints; i++ )
        {
            m_pJoints[ i ].pName = (COCStr *)ptr; ptr += 4 + m_pJoints[ i ].pName->m_nLength;
            m_pJoints[ i ].index = *(word *)ptr; ptr += 2;
            m_pJoints[ i ].v = *(XVec3 *)ptr; ptr += sizeof( XVec3 );
            m_pJoints[ i ].u = *(XVec3 *)ptr; ptr += sizeof( XVec3 );
            m_pJoints[ i ].ox = *(short *)ptr; ptr += 2;
            m_pJoints[ i ].oy = *(short *)ptr; ptr += 2;
            m_pJoints[ i ].oz = *(short *)ptr; ptr += 2;
        }
    }

    return 0;
}
