// This file is distributed under a MIT license. See LICENSE.txt for details.
//
// Adaptive tesselator
//

#include "common.h"

//
// Constructor
//
CATess::CATess
    (
    )
{
    m_pVerts = NULL;
}

//
// Destructor
//
CATess::~CATess
    (
    )
{
    int i;

    for ( i = 0; i < 4; i++ )
    {
        TATessTri *ct = m_pTriSet[ i ];
        TATessTri *next;

        while ( ct )
        {
            next = ct->pNext;
            DEL( ct );
            ct = next;
        }
    }

    VDEL( m_pVerts );
}

//
// Generate polygonal mesh from BTM
//
bool CATess::Tesselate
    (
    CBTMSurface *pBTM,
    float fEdgeDeltas[ 4 ],
    float l_fAmp,
    int nLODSize
    )
{
    m_pBTM = pBTM;
    m_pTriSet[ 0 ] = NULL;
    m_pTriSet[ 1 ] = NULL;
    m_pTriSet[ 2 ] = NULL;
    m_pTriSet[ 3 ] = NULL;
    m_nNumTris[ 0 ] = 0;
    m_nNumTris[ 1 ] = 0;
    m_nNumTris[ 2 ] = 0;
    m_nNumTris[ 3 ] = 0;
    m_fAmp = l_fAmp;
    m_nNumVerts = 0;

    m_pVerts = NEW XVec3[ 256 * 256 ];
    m_nVertexMap = NEW short[ 128 * 128 ];
    memset( m_nVertexMap, 0, 128 * 128 * sizeof( short ) );

    m_bIncludeMap = NEW bool[ 128 * 128 ];

    for ( int lod = 0; lod < nLODSize; lod++ )
    {
        SetCurrentLOD( lod );

        memset( m_bIncludeMap, 0, 128 * 128 );

        DoQuadInclude_r( 64, 64, 64 );
        DoQuadFix_r( 64, 64, 64 );
        DoQuadTesselate_r( 64, 64, 64 );

        ExtrudeEdges( fEdgeDeltas );

        /**
        if ( 0 )
        {
            CTGASurface tga;
            int i, j;

            tga.Create( 128, 128 );

            for ( i = 0; i < 128; i++ )
            {
                for ( j = 0; j < 128; j++ )
                {
                    if ( GetIncMap( i, j ) )
                        tga.SetRGBAPixel( i, j, 0xffffff );
                    else
                        tga.SetRGBAPixel( i, j, 0x606060 );
                }
            }

            tga.Dump( "includ.tga" );
        }
        /**/
    }

    VDEL( m_bIncludeMap );
    VDEL( m_nVertexMap );

    return true;
}

bool CATess::DoQuadInclude_r
    ( 
    int nX, 
    int nY, 
    int nR
    )
{
    if ( nR == 0 )
        return false;

    bool bRet = false;

    {
        XVec3 vNull( 0, 0, 0 );
        XVec3 v[ 4 ];
        XVec3 e[ 5 ];
        float fLevel = 30.f * powf( ( (float)nR / 64.f ), 0.9f );

        v[ 0 ] = MakeUVVertex( nX - nR, nY - nR );
        v[ 1 ] = MakeUVVertex( nX + nR, nY - nR );
        v[ 2 ] = MakeUVVertex( nX + nR, nY + nR );
        v[ 3 ] = MakeUVVertex( nX - nR, nY + nR );

        e[ 0 ] = MakeUVVertex( nX, nY - nR );
        e[ 1 ] = MakeUVVertex( nX + nR, nY );
        e[ 2 ] = MakeUVVertex( nX, nY + nR );
        e[ 3 ] = MakeUVVertex( nX - nR, nY );
        e[ 4 ] = MakeUVVertex( nX, nY );

        // test edges

        if ( TestEdgeArea( v[ 0 ], e[ 0 ], v[ 1 ], vNull ) )
        {
            bRet = bRet || true;
            SetIncMap( nX, nY - nR, true );
        }

        if ( TestEdgeArea( v[ 1 ], e[ 1 ], v[ 2 ], vNull ) )
        {
            bRet = bRet || true;
            SetIncMap( nX + nR, nY, true );
        }

        if ( TestEdgeArea( v[ 2 ], e[ 2 ], v[ 3 ], vNull ) )
        {
            bRet = bRet || true;
            SetIncMap( nX, nY + nR, true );
        }

        if ( TestEdgeArea( v[ 3 ], e[ 3 ], v[ 0 ], vNull ) )
        {
            bRet = bRet || true;
            SetIncMap( nX - nR, nY, true );
        }
    }

        int newR = nR >> 1;
        bool bi[ 4 ];

        bi[ 0 ] = DoQuadInclude_r( nX - newR, nY - newR, newR  );
        bi[ 1 ] = DoQuadInclude_r( nX + newR, nY - newR, newR  );
        bi[ 2 ] = DoQuadInclude_r( nX - newR, nY + newR, newR  );
        bi[ 3 ] = DoQuadInclude_r( nX + newR, nY + newR, newR  );

        // make minimal detalization
        if ( nR > 32 )
        {
            bi[ 0 ] = true;
            bi[ 1 ] = true;
            bi[ 2 ] = true;
            bi[ 3 ] = true;
        }
        
        if ( bi[ 0 ] )
        {
            SetIncMap( nX - nR, nY - nR, true );
            SetIncMap( nX, nY - nR, true );
            SetIncMap( nX, nY, true );
            SetIncMap( nX - nR, nY, true );
        }

        if ( bi[ 1 ] )
        {
            SetIncMap( nX + nR, nY - nR, true );
            SetIncMap( nX, nY - nR, true );
            SetIncMap( nX, nY, true );
            SetIncMap( nX + nR, nY, true );
        }

        if ( bi[ 2 ] )
        {
            SetIncMap( nX - nR, nY + nR, true );
            SetIncMap( nX, nY + nR, true );
            SetIncMap( nX, nY, true );
            SetIncMap( nX - nR, nY, true );
        }

        if ( bi[ 3 ] )
        {
            SetIncMap( nX + nR, nY + nR, true );
            SetIncMap( nX, nY + nR, true );
            SetIncMap( nX, nY, true );
            SetIncMap( nX + nR, nY, true );
        }

    return bRet | bi[ 0 ] || bi[ 1 ] || bi[ 2 ] || bi[ 3 ];
}

bool CATess::DoQuadFix_r
    ( 
    int nX, 
    int nY, 
    int nR
    )
{
    // if we have central point in quad, then go to sub-quads
    if ( GetIncMap( nX, nY ) && ( nR > 1 ) )
    {
        int newR = nR >> 1;

        SetIncMap( nX - nR, nY - nR, true );
        SetIncMap( nX, nY - nR, true );
        SetIncMap( nX + nR, nY - nR, true );
        SetIncMap( nX - nR, nY + nR, true );
        SetIncMap( nX, nY + nR, true );
        SetIncMap( nX + nR, nY + nR, true );
        SetIncMap( nX - nR, nY, true );
        SetIncMap( nX + nR, nY, true );

        DoQuadFix_r( nX - newR, nY - newR, newR  );
        DoQuadFix_r( nX + newR, nY - newR, newR  );
        DoQuadFix_r( nX - newR, nY + newR, newR  );
        DoQuadFix_r( nX + newR, nY + newR, newR  );

        return true;
    }

    return true;
}

//
//
//
bool CATess::DoQuadTesselate_r
    ( 
    int nX, 
    int nY, 
    int nR
    )
{
    // if we have central point in quad, then go to sub-quads
    if ( GetIncMap( nX, nY ) && ( nR > 1 ) )
    {
        int newR = nR >> 1;

        DoQuadTesselate_r( nX - newR, nY - newR, newR  );
        DoQuadTesselate_r( nX + newR, nY - newR, newR  );
        DoQuadTesselate_r( nX - newR, nY + newR, newR  );
        DoQuadTesselate_r( nX + newR, nY + newR, newR  );

        return true;
    }

    // build triangle fan from central point
    XVec3 cp = MakeUVVertex( nX, nY );
    XVec3 lp, np, sp;
    int i, nr[ 4 ];

    // initialize last point as left-top corner
    lp = MakeUVVertex( nX - nR, nY - nR );
    sp = lp;

    nr[ 0 ] = (nY + nR) > 127 ? nR - 1 : nR;
    nr[ 1 ] = (nX + nR) > 127 ? nR - 1 : nR;
    nr[ 2 ] = (nY + nR) > 127 ? nR - 1 : nR;
    nr[ 3 ] = (nX + nR) > 127 ? nR - 1 : nR;

    // go down by left edge and build triangles
    for ( i = -nR + 1; i < nr[ 0 ]; i++ )
        if ( GetIncMap( nX - nR, nY + i ) )
        {
            np = MakeUVVertex( nX - nR, nY + i );
            AddTriangle( cp, lp, np );
            lp = np;
        }
    // go right
    for ( i = -nR; i < nr[ 1 ]; i++ )
        if ( GetIncMap( nX + i, nY + nr[ 0 ] ) )
        {
            np = MakeUVVertex( nX + i, nY + nr[ 0 ] );
            AddTriangle( cp, lp, np );
            lp = np;
        }
    // go up
    for ( i = nr[ 2 ]; i > -nR; i-- )
        if ( GetIncMap( nX + nr[ 1 ], nY + i ) )
        {
            np = MakeUVVertex( nX + nr[ 1 ], nY + i );
            AddTriangle( cp, lp, np );
            lp = np;
        }
    // go left
    for ( i = nr[ 3 ]; i > -nR; i-- )
        if ( GetIncMap( nX + i, nY - nR ) )
        {
            np = MakeUVVertex( nX + i, nY - nR );
            AddTriangle( cp, lp, np );
            lp = np;
        }

    AddTriangle( cp, lp, sp );

    return true;
}

//
// Add triangle into a list
//
void CATess::AddTriangle
    ( 
    XVec3 &v0, 
    XVec3 &v1, 
    XVec3 &v2 
    )
{
    TATessTri *t = NEW TATessTri;

    if ( m_pTriSet )
    {
        t->pNext = m_pTriSet[ m_nCurLOD ];
        m_pTriSet[ m_nCurLOD ] = t;
    }
    else
    {
        m_pTriSet[ m_nCurLOD ] = t;
        t->pNext = NULL;
    }

    t->vidx[ 0 ] = AddVertex( v0 );
    t->vidx[ 2 ] = AddVertex( v1 );
    t->vidx[ 1 ] = AddVertex( v2 );

    t->v[ 0 ] = v0;
    t->v[ 1 ] = v2;
    t->v[ 2 ] = v1;

    m_nNumTris[ m_nCurLOD ]++;
}

//
// Save DEBUG mesh into STL file
//
void CATess::Dump
    ( 
    const char *pSTLFileName
    )
{
    FILE *fp;
    TATessTri *tri = m_pTriSet[ m_nCurLOD ];

    fp = fopen( pSTLFileName, "wb" );
    
    fprintf( fp, "solid Object\n" );
    while ( tri )
    {
        fprintf( fp, "facet normal 0 0 0\n" );
        fprintf( fp, "outer loop\n" );

        fprintf( fp, "vertex %f %f %f\n", tri->v[ 0 ].x, tri->v[ 0 ].z, tri->v[ 0 ].y );
        fprintf( fp, "vertex %f %f %f\n", tri->v[ 1 ].x, tri->v[ 1 ].z, tri->v[ 1 ].y );
        fprintf( fp, "vertex %f %f %f\n", tri->v[ 2 ].x, tri->v[ 2 ].z, tri->v[ 2 ].y );

        fprintf( fp, "endloop\n" );
        fprintf( fp, "endfacet\n" );

        tri = tri->pNext;
    }
    fprintf( fp, "endsolid Object\n" );

    fclose( fp );
}

//
// Build vertex shifted from planar surface along height map
//
XVec3 CATess::MakeUVVertex
    ( 
    int nU, 
    int nV 
    )
{
    return XVec3( nU, GetUVHeight( nU, nV ), nV );
}

//
// Get height from map
//
float CATess::GetUVHeight
    (
    int nU,
    int nV
    )
{
    if ( nU < 0 ) nU = 0;
    if ( nV < 0 ) nV = 0;

    int cu = nU < (int)m_pBTM->m_Header.dwWidth ? nU : m_pBTM->m_Header.dwWidth - 1;
    int cv = nV < (int)m_pBTM->m_Header.dwWidth ? nV : m_pBTM->m_Header.dwWidth - 1;

    cv = m_pBTM->m_Header.dwWidth - cv - 1;

    if ( m_pBTM->m_Header.dwNumLayers > 1 )
        return m_fAmp * (float)m_pBTM->GetPixel( cu, cv, 1 );
    else
        return 0.f;
}

//
// Recursive surface tesselation
//
bool CATess::Tesselate_r
    ( 
    TATessTri *pTri
    )
{
    TATessTri *curr = pTri->pNext;
    TATessTri *tr[ 4 ];
    XVec3 ev[ 3 ], dv;
    XVec3 eh[ 3 ];
    int i;
    byte edge;

    float r;

    int q = 0;
    for ( i = 0; i < 3; i++ )
    {
        dv = curr->v[ i ] - curr->v[ ( i + 1 ) % 3 ];
        r = Fabs( dv.x - dv.z );
        if ( r < 1.1f && r > 0.1f )
            q++;
    }
    if ( q == 3 ) return true;

    ev[ 0 ] = 0.5f * ( curr->v[ 0 ] + curr->v[ 1 ] );
    ev[ 1 ] = 0.5f * ( curr->v[ 1 ] + curr->v[ 2 ] );
    ev[ 2 ] = 0.5f * ( curr->v[ 2 ] + curr->v[ 0 ] );
    
    eh[ 0 ] = MakeUVVertex( ev[ 0 ].x, ev[ 0 ].z );
    eh[ 1 ] = MakeUVVertex( ev[ 1 ].x, ev[ 1 ].z );
    eh[ 2 ] = MakeUVVertex( ev[ 2 ].x, ev[ 2 ].z );

    eh[ 0 ].x = ev[ 0 ].x;
    eh[ 0 ].z = ev[ 0 ].z;
    eh[ 1 ].x = ev[ 1 ].x;
    eh[ 1 ].z = ev[ 1 ].z;
    eh[ 2 ].x = ev[ 2 ].x;
    eh[ 2 ].z = ev[ 2 ].z;

    edge = 0;
    for ( i = 0; i < 3; i++ )
    {
        if ( TestEdgeArea( curr->v[ i ], eh[ i ], curr->v[ ( i + 1 ) % 3 ], curr->v[ ( i + 2 ) % 3 ] ) )
        {
            edge |= 1 << i;
        }
    }

    // show edge cases
    switch ( edge )
    {
    case 0 :
        // no tesselation
        return true;

    case 1 : case 2 : case 4 :
        // build 2 triangles
        edge = edge >> 1;

        tr[ 0 ] = NEW TATessTri;
        tr[ 1 ] = NEW TATessTri;
        tr[ 0 ]->pNext = tr[ 1 ];
        tr[ 1 ]->pNext = curr->pNext;
        pTri->pNext = tr[ 0 ];
        tr[ 0 ]->nLevel = curr->nLevel + 1;
        tr[ 1 ]->nLevel = curr->nLevel + 1;

        tr[ 0 ]->v[ 0 ] = curr->v[ ( edge + 2 ) % 3 ];
        tr[ 0 ]->v[ 1 ] = curr->v[ ( edge + 0 ) % 3 ];
        tr[ 0 ]->v[ 2 ] = eh[ edge ];
        tr[ 1 ]->v[ 0 ] = curr->v[ ( edge + 2 ) % 3 ];
        tr[ 1 ]->v[ 1 ] = eh[ edge ];
        tr[ 1 ]->v[ 2 ] = curr->v[ ( edge + 1 ) % 3 ];

        return false;

    case 3 : case 5 : case 6 :
        // build 3 triangles
        edge = ~edge & 3;

        tr[ 0 ] = NEW TATessTri;
        tr[ 1 ] = NEW TATessTri;
        tr[ 2 ] = NEW TATessTri;
        tr[ 0 ]->pNext = tr[ 1 ];
        tr[ 1 ]->pNext = tr[ 2 ];
        tr[ 2 ]->pNext = curr->pNext;
        pTri->pNext = tr[ 0 ];
        tr[ 0 ]->nLevel = curr->nLevel + 1;
        tr[ 1 ]->nLevel = curr->nLevel + 1;
        tr[ 2 ]->nLevel = curr->nLevel + 1;

        tr[ 0 ]->v[ 0 ] = curr->v[ ( edge + 2 ) % 3 ];
        tr[ 0 ]->v[ 1 ] = curr->v[ ( edge + 0 ) % 3 ];
        tr[ 0 ]->v[ 2 ] = eh[ edge ];
        tr[ 1 ]->v[ 0 ] = curr->v[ ( edge + 2 ) % 3 ];
        tr[ 1 ]->v[ 1 ] = eh[ edge ];
        tr[ 1 ]->v[ 2 ] = eh[ ( edge + 1 ) % 3 ];
        tr[ 2 ]->v[ 0 ] = eh[ ( edge + 1 ) % 3 ];
        tr[ 2 ]->v[ 1 ] = eh[ edge ];
        tr[ 2 ]->v[ 2 ] = curr->v[ ( edge + 1 ) % 3 ];

        return false;

    case 7 :
        // build 4 triangles
        tr[ 0 ] = NEW TATessTri;
        tr[ 1 ] = NEW TATessTri;
        tr[ 2 ] = NEW TATessTri;
        tr[ 3 ] = NEW TATessTri;		
        tr[ 0 ]->pNext = tr[ 1 ];
        tr[ 1 ]->pNext = tr[ 2 ];
        tr[ 2 ]->pNext = tr[ 3 ];
        tr[ 3 ]->pNext = curr->pNext;
        pTri->pNext = tr[ 0 ];
        tr[ 0 ]->nLevel = curr->nLevel + 1;
        tr[ 1 ]->nLevel = curr->nLevel + 1;
        tr[ 2 ]->nLevel = curr->nLevel + 1;
        tr[ 3 ]->nLevel = curr->nLevel + 1;

        tr[ 0 ]->v[ 0 ] = curr->v[ 0 ];
        tr[ 0 ]->v[ 1 ] = eh[ 0 ];
        tr[ 0 ]->v[ 2 ] = eh[ 2 ];
        tr[ 1 ]->v[ 0 ] = eh[ 0 ];
        tr[ 1 ]->v[ 1 ] = eh[ 1 ];
        tr[ 1 ]->v[ 2 ] = eh[ 2 ];
        tr[ 2 ]->v[ 0 ] = eh[ 1 ];
        tr[ 2 ]->v[ 1 ] = curr->v[ 2 ];
        tr[ 2 ]->v[ 2 ] = eh[ 2 ];
        tr[ 3 ]->v[ 0 ] = eh[ 0 ];
        tr[ 3 ]->v[ 1 ] = curr->v[ 1 ];
        tr[ 3 ]->v[ 2 ] = eh[ 1 ];
        return false;

  }

    return true;
}

//
// Test edge for tesselation
//
bool CATess::TestEdgeArea
    ( 
    const XVec3 &v0, 
    const XVec3 &e, 
    const XVec3 &v1, 
    const XVec3 &v2
    )
{
    bool bResult;
    TATessRect rect[ 3 ];
    XVec3 rvec;
    float height[ 3 ];

    rvec = 0.25f * ( v1 - v0 );

    rect[ 0 ].set( v0.x, v0.z, v0.x, v0.z );
    rect[ 0 ].AddPoint( v0.x + rvec.x, v0.z + rvec.x );
    rect[ 0 ].AddPoint( v0.x - rvec.x, v0.z - rvec.x );
    rect[ 0 ].AddPoint( v0.x + rvec.z, v0.z + rvec.z );
    rect[ 0 ].AddPoint( v0.x - rvec.z, v0.z - rvec.z );

    rect[ 1 ].set( e.x, e.z, e.x, e.z );
    rect[ 1 ].AddPoint( e.x + rvec.x, e.z + rvec.x );
    rect[ 1 ].AddPoint( e.x - rvec.x, e.z - rvec.x );
    rect[ 1 ].AddPoint( e.x + rvec.z, e.z + rvec.z );
    rect[ 1 ].AddPoint( e.x - rvec.z, e.z - rvec.z );

    rect[ 2 ].set( v1.x, v1.z, v1.x, v1.z );
    rect[ 2 ].AddPoint( v1.x + rvec.x, v1.z + rvec.x );
    rect[ 2 ].AddPoint( v1.x - rvec.x, v1.z - rvec.x );
    rect[ 2 ].AddPoint( v1.x + rvec.z, v1.z + rvec.z );
    rect[ 2 ].AddPoint( v1.x - rvec.z, v1.z - rvec.z );

    height[ 0 ] = GetAvarageHeight( rect[ 0 ] );
    height[ 1 ] = GetAvarageHeight( rect[ 1 ] );
    height[ 2 ] = GetAvarageHeight( rect[ 2 ] );
    
    float vy = 0.5f * ( height[ 0 ] + height[ 2 ] );

    bResult = Fabs( vy - height[ 1 ] ) > m_fCurLOD;

    return bResult;
}

//
// Get avarage height from map
//
float CATess::GetAvarageHeight
    ( 
    const TATessRect &rect
    )
{
    int area = 0;
    int x, y, h;

    for ( y = rect.y0 + 1; y < rect.y1; y++ )
    {
        for ( x = rect.x0 + 1; x < rect.x1; x++ )
        {
            h = GetUVHeight( x, y );
            area += h;
        }
    }

    return (float)area / ( ( rect.x1 - rect.x0 ) * ( rect.y1 - rect.y0 ) );
}

//
// Add unique vertex into a mesh
//
int CATess::AddVertex
    (
    XVec3 &v,
    bool bTestByTable
    )
{
    int nX = v.x;
    int nY = v.z;

    if ( bTestByTable )
    {
        if ( m_nVertexMap[ nY * 128 + nX ] )
        {
            return m_nVertexMap[ nY * 128 + nX ] - 1;
        }
        else
        {
            m_nVertexMap[ nY * 128 + nX ] = m_nNumVerts + 1;
        }
    }
    else
    {
        int i;

        for ( i = 0; i < m_nNumVerts; i++ )
        {
            if ( v == m_pVerts[ i ] )
                return i;
        }
    }

    m_pVerts[ m_nNumVerts ] = v;
    
    // calculate vertex height range
    if ( m_nCurLOD == 0 )
    {
        if ( !m_nNumVerts )
        {
            m_fMinHeight = v.y;
            m_fMaxHeight = v.y;
        }
        else
        {
            if ( v.y > m_fMaxHeight )
                m_fMaxHeight = v.y;
            if ( v.y < m_fMinHeight )
                m_fMinHeight = v.y;
        }
    }

    m_nNumVerts++;
    return m_nNumVerts - 1;
}

//
// Extrude free edges for tile faces emulation
//
void CATess::ExtrudeEdges
    ( 
    float fEdgeDeltas[ 4 ]
    )
{
    XVec3       *v[ 3 ], *vtmp;
    TATessTri   *tri, *tri_prev, *tri_new;
    int         edg, et;
    XVec3       vtx;
    float       bak_m_fMinHeight = m_fMinHeight;
    float       bak_m_fMaxHeight = m_fMaxHeight;
    float       height = m_fMinHeight - 0.1;

    tri = m_pTriSet[ m_nCurLOD ];
    tri_prev = NULL;

    while ( tri )
    {
        v[ 0 ] = m_pVerts + tri->vidx[ 0 ];
        v[ 1 ] = m_pVerts + tri->vidx[ 1 ];
        v[ 2 ] = m_pVerts + tri->vidx[ 2 ];

        // each triangle may has only two free edges
        for ( edg = 0; edg < 3; edg++ )
        {
            // calc type of edge
            et = 0;
            if ( v[ 0 ]->z == 0 && v[ 1 ]->z == 0 )
                et = 1;
            else
            if ( v[ 0 ]->x == 0 && v[ 1 ]->x == 0 )
                et = 2;
            else
            if ( v[ 0 ]->z == 127 && v[ 1 ]->z == 127 )
                et = 3;
            else
            if ( v[ 0 ]->x == 127 && v[ 1 ]->x == 127 )
                et = 4;

#ifdef OC_DO_TILE_CONNECTION
            static int edgemap[ 5 ] = { 0, 1, 0, 3, 2 };
            int edidx = edgemap[ et ];

            if ( fEdgeDeltas[ edidx ] == 0 )
                et = 0;
#endif // OC_DO_TILE_CONNECTION

            if ( et )
            {
                // given edge is free
                int idx_last;

                for ( int e = 0; e < 2; e++ )
                {
                    tri_new = NEW TATessTri;
                    m_nNumTris[ m_nCurLOD ]++;

                    if ( !tri_prev )
                    {
                        tri_new->pNext = m_pTriSet[ m_nCurLOD ];
                        m_pTriSet[ m_nCurLOD ] = tri_new;
                    }
                    else
                    {
                        tri_new->pNext = tri;
                        tri_prev->pNext = tri_new;
                    }
                    tri_prev = tri_new;

                    if ( !e )
                    {
                        vtx.x = v[ 0 ]->x;
#ifdef OC_DO_TILE_CONNECTION
                        vtx.y = v[ 0 ]->y - fEdgeDeltas[ edidx ];
#else
                        vtx.y = height;
#endif // OC_DO_TILE_CONNECTION
                        vtx.z = v[ 0 ]->z;
                        tri_new->vidx[ 0 ] = tri->vidx[ ( 0 + edg ) % 3 ];
                        idx_last = AddVertex( vtx, false );
                        tri_new->vidx[ 1 ] = idx_last;
                        tri_new->vidx[ 2 ] = tri->vidx[ ( 1 + edg ) % 3 ];
                    }
                    else
                    {
                        vtx.x = v[ 1 ]->x;
#ifdef OC_DO_TILE_CONNECTION
                        vtx.y = v[ 1 ]->y - fEdgeDeltas[ edidx ];
#else
                        vtx.y = height;
#endif // OC_DO_TILE_CONNECTION
                        vtx.z = v[ 1 ]->z;
                        tri_new->vidx[ 0 ] = tri->vidx[ ( 1 + edg ) % 3 ];
                        tri_new->vidx[ 1 ] = idx_last;
                        tri_new->vidx[ 2 ] = AddVertex( vtx, false );
                    }
                }
            }

            // scroll vertex pointers
            vtmp = v[ 0 ];
            v[ 0 ] = v[ 1 ];
            v[ 1 ] = v[ 2 ];
            v[ 2 ] = vtmp;
        }

        tri_prev = tri;
        tri = tri->pNext;
    }

    m_fMinHeight = bak_m_fMinHeight;
    m_fMaxHeight = bak_m_fMaxHeight;
}

//
// Set current LOD for tesselation
//
void CATess::SetCurrentLOD
    ( 
    int nLod 
    )
{
    m_nCurLOD = nLod;
    switch ( nLod )
    {
    case 0 : m_fCurLOD = 0.5f; break;
    case 1 : m_fCurLOD = 1.0f; break;
    case 2 : m_fCurLOD = 2.0f; break;
    case 3 : m_fCurLOD = 3.0f; break;
    default : m_fCurLOD = 128.f; break;
    }
}
