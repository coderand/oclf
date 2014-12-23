// This file is distributed under a MIT license. See LICENSE.txt for details.
//
// Adaptive tesselator ( aka Adaptive Displacement Mapping )
//

#ifndef __CATESS_H__
#define __CATESS_H__

class CBTMSurface;

typedef struct SATessTri
{
    int             nLevel;
    XVec3           v[ 3 ];
    int             vidx[ 3 ];
    XVec3           n;

    SATessTri       *pNext;

} TATessTri;

class TATessRect
{
public :

    int             x0, y0;
    int             x1, y1;

    TATessRect      ()
    {
    }

    TATessRect      ( int _x0, int _y0, int _x1, int _y1 )
    {
        x0 = _x0;
        y0 = _y0;
        x1 = _x1;
        y1 = _y1;
    }

    void set        ( int _x0, int _y0, int _x1, int _y1 )
    {
        x0 = _x0;
        y0 = _y0;
        x1 = _x1;
        y1 = _y1;
    }

    void AddPoint   ( int _x0, int _y0 )
    {
        if ( _x0 < x0 ) x0 = _x0;
        if ( _y0 < y0 ) y0 = _y0;
        if ( _x0 > x1 ) x1 = _x0;
        if ( _y0 > y1 ) y1 = _y0;
    }
};

class CATess
{
private :

    int             m_nCurLOD; // 0     1    2    3
    float           m_fCurLOD; // 0.5f, 1.f, 2.f, 4.f
    float           m_fAmp;
    CBTMSurface     *m_pBTM;
    bool            *m_bIncludeMap;
    short           *m_nVertexMap;

    TATessTri       *m_pTriSet[ 4 ];
    int             m_nNumTris[ 4 ];

    float           GetUVHeight             ( int nU, int nV );
    XVec3           MakeUVVertex            ( int nU, int nV );
    bool            TestEdgeArea            ( const XVec3 &v0, const XVec3 &e, const XVec3 &v1, const XVec3 &v2 );
    float           GetAvarageHeight        ( const TATessRect &rect );
    bool            Tesselate_r             ( TATessTri *pTri );

    // new approach
    void            SetCurrentLOD           ( int nLod );
    bool            DoQuadInclude_r         ( int nX, int nY, int nR );
    bool            DoQuadTesselate_r       ( int nX, int nY, int nR );
    bool            DoQuadFix_r             ( int nX, int nY, int nR );
    void            ExtrudeEdges            ( float fEdgeDeltas[ 4 ] );
    void            AddTriangle             ( XVec3 &v0, XVec3 &v1, XVec3 &v2 );
    int             AddVertex               ( XVec3 &v, bool bTestByTable = true );
    inline void     SetIncMap               ( int nX, int nY, bool bState )
    {
        if ( nX > 127 ) nX = 127;
        if ( nY > 127 ) nY = 127;
        if ( nX < 0 ) nX = 0;
        if ( nY < 0 ) nY = 0;

        m_bIncludeMap[ nY * 128 + nX ] = bState;
    }
    inline bool     GetIncMap               ( int nX, int nY )
    {
        if ( nX > 127 ) nX = 127;
        if ( nY > 127 ) nY = 127;
        if ( nX < 0 ) nX = 0;
        if ( nY < 0 ) nY = 0;

        return m_bIncludeMap[ nY * 128 + nX ];
    }

public :

    XVec3           *m_pVerts;
    int             m_nNumVerts;
    float           m_fMinHeight;
    float           m_fMaxHeight;

    bool            Tesselate               ( CBTMSurface *pBTM, float fEdgeDeltas[ 4 ], float fAmp = 0.5f, int nLODSize = 1 );
    void            Dump                    ( const char *pSTLFileName );

    int             GetNumTris              ( int nLOD ) { return m_nNumTris[ nLOD ]; };
    TATessTri       *GetTriSet              ( int nLOD ) { return m_pTriSet[ nLOD ]; };

                    CATess                  ();
                    ~CATess                 ();
};

#endif // __CATESS_H__
