// This file is distributed under a MIT license. See LICENSE.txt for details.

#ifndef __OCDATA_H__
#define __OCDATA_H__

#pragma warning( disable : 4200 ) // zero-sized array in struct/union

//#define OC_SUPPORT_ENTITIES
#include "OCMath.h"

//
// Outcast data classes
//
#define OC_APPEAL_SIGNATURE	0x00006E71

#define OC_FILEID_FLT	0x0289FCF5
#define OC_FILEID_TVT	0x0770FCF5
#define OC_FILEID_VTL	0x0EC4F345
#define OC_FILEID_LND	0x04BCCAE5
#define OC_FILEID_WRD	0x055E6924
#define OC_FILEID_PAL	0x0E614355
#define OC_FILEID_BTM	0x04023750
#define OC_FILEID_RBT	0x07960AA0
#define OC_FILEID_MSH	0x00553C98
#define OC_FILEID_AMH	0x05463C98
#define OC_FILEID_FNT	0x0054D654
#define OC_FILEID_PAK	0x097DD4D3

typedef unsigned int    dword;
typedef unsigned short  word;
typedef unsigned char   byte;

//////////////////////////////////////////////////////////////////////
//
// File headers
//
//////////////////////////////////////////////////////////////////////

//
// BTM file header structure
//
typedef struct
{
    dword       dwSignature;
    dword       dwTypeID;
    dword       dwUnknown01;
    dword       dwUnknown02;
    dword       dwUnknown03;
    dword       dwUnknown04;
    dword       dwBTMFileSize;
    dword       dwUnknown05;
    dword       dwUnknown06;
    int         dwWidth;
    int         dwHeight;
    dword       dwNumLayers;
    dword       dwImageSize;
    dword       dwUnknown08;
} TBTMHeader;

typedef struct
{
    dword       dwSignature;
    dword       dwTypeID;
    dword       dwUnknown01;
    dword       dwUnknown02;
    dword       dwUnknown03;
    dword       dwUnknown04;
    dword       dwUnknown05;
    dword       dwUnknown06;
    dword       dwUnknown07;
    dword       dwUnknown08;
    dword       dwUnknown09;
    dword       dwUnknown10;
    dword       dwColorSize;
    dword       dwNumOfColors;
} TPLTHeader;

typedef struct
{
    byte        dwUnknown01;
    byte        dwUnknown02;
    byte        dwSignature;
    byte        dwUnknown03;
    byte        dwUnknown04;
    byte        dwUnknown05;
    byte        dwUnknown06;
    byte        dwUnknown07;
    byte        dwUnknown08;
    byte        dwUnknown09;
    byte        dwUnknown10;
    byte        dwUnknown11;
    word        wWidth;
    word        wHeight;
    word        wBPP;
} TTGAHeader;

typedef struct
{
    dword       dwSignature;
    dword       dwTypeID;
    dword       dwMajVer;
    dword       dwMinVer;

    dword       dwUnknown01;
    dword       dwUnknown02;
    dword       dwRBTFileSize;
    dword       dwUnknown03;
    dword       dwUnknown04;
    dword       dwWidth;
    dword       dwHeight;
    dword       dwUnknown05;
    dword       dwEndOffset;
    dword       dwUnknown07;
} TRBTHeader;

//////////////////////////////////////////////////////////////////////
//
// Common data classes
//
//////////////////////////////////////////////////////////////////////
typedef struct
{
    dword       dwUnknown;
    dword       dwOffset;
} TOCChunk;

typedef struct
{
    dword       dwSignature;
    dword       dwTypeID;
    dword       dwMajVer;
    dword       dwMinVer;

    dword       dwChunkCount;
    TOCChunk    pChunks[]; // ( dwChunkCount + 1 ) chunks, last of them points onto end of file
} TOCMainHeader;

typedef struct
{
    dword       dwUnknown01;
    dword       dwUnknown02;
    dword       dwRGB;
} TPLTColor;

//
// Palette class
//
class CPLTSurface
{
public :

    TPLTHeader  m_Header;
    TPLTColor   *m_pColors;

    dword       LoadFromMemory          ( const void *pData );
    dword       GetColor                ( dword dwIndex );

                CPLTSurface             ();
                ~CPLTSurface        ();
};

//
// Bitmap class
//
class CBTMSurface
{
public :

    TBTMHeader  m_Header;
    byte        *m_pArray;
    CPLTSurface *m_pPalette;

    bool        LoadFromFile            ( const char *pFileName );
    dword       LoadFromMemory          ( const void *pData );
    dword       GetPixel                ( int nX, int nY, byte nLayer, byte nMipLevel = 0 );
    dword       GetRGBAPixel            ( int nX, int nY, byte nLayer, byte nMipLevel = 0 );

    void        DumpLayer               ( const char *pTGAFileName, int nLayer = 0 );

                CBTMSurface             ();
                ~CBTMSurface            ();
};

//
// Rlc Bitmap class
//
class CRBTSurface
{
private :

    void        UnpackScanline          ( char *src, char *dest );

public :

    TRBTHeader  *m_pHeader;             // base data for file
    int         *m_pOffs;
    char        *m_bUnpacked;
    CPLTSurface	*m_pPalette;

    dword       GetPixel                ( int nX, int nY );

    dword       LoadFromMemory          ( const void *pData );
    void        Unpack                  ();

    void        DumpLayer               ( const char *pTGAFileName, int nLayer = 0 );
};

//
// TGA class
//
class CTGASurface
{
public :

    TTGAHeader  m_Header;
    byte        *m_pArray;

    dword       Create                  ( int nWidth, int nHeight, int nBPP = 32 );
    dword       LoadFromMemory          ( const void *pData );
    void        SetRGBAPixel            ( int nX, int nY, dword dwColor );

    void        Dump                    ( const char *pTGAFileName );

                CTGASurface             ();
                ~CTGASurface            ();
};

//////////////////////////////////////////////////////////////////////
//
// Landscape file ( *.lnd )
//
//////////////////////////////////////////////////////////////////////

class COCStr
{
public :

    int         m_nLength;

private :

    char        m_String[];

public :

    bool        Cmp                     ( const char *pStr );
    void        CopyTo                  ( char *pStr );
};

typedef struct
{
    dword       dwSignature;
    dword       dwTypeID;
    dword       dwMajVer;
    dword       dwMinVer;
} TOCBaseHeader;

#define LND_REC_NAME    0
#define LND_REC_XORIGIN 2
#define LND_REC_YORIGIN 3
#define LND_REC_SKYNAME 4

#define REC_VAL_PTR( x )    (COCStr *)( (byte *)x + x->m_nLength + sizeof( int ) )

//
// lnd header
//
typedef struct
{
    dword       dwSignature;
    dword       dwTypeID;
    dword       dwMajVer;
    dword       dwMinVer;

    dword       dwChunkCount;
    dword       dwUnkID;
} TOCLndHeader;

typedef struct
{
    dword       dwOffset;
    dword       dwUnknown;
} TOCLHChunk;

typedef struct
{
    dword       dwChunkCount;
    TOCLHChunk  chunks[];
} TOCLndProp;

typedef struct
{
    dword       dwUnknown;
    dword       dwWidth;
    dword       dwHeight;
} TOCLndSize;

//
// lnd record
//
typedef struct
{
    dword           dwOffsRecName;
    dword           dwUnknown[ 12 ];
    dword           dwOffsRecNext;
} TOCLndRecHeader;

//
// Hi-level voxel tiles
//
class CBaseVoxelTile
{
public :

    void            *m_pFileData;
    TOCLndHeader    *m_pHeader;
    TOCLHChunk      *m_pChunks;

    char            *m_pBasePath;
    char            m_pTileName[ 32 ];
    CBTMSurface     m_Texture;

    // parsed properties
    dword           m_dwHeightAmp;
    dword           m_dwHeightMax;
    
    // additional properties for OCLF
    float           m_fMinEdgeHeight[ 4 ];  // left, down, right, up, 
    float           m_fMaxEdgeHeight[ 4 ];  // --//--
    float           m_fEdgeDelta[ 4 ];

    virtual void    LoadData                ( byte *pBase );
};

class CVoxelTile : public CBaseVoxelTile
{
public :

    virtual void    LoadData                ( byte *pBase );
};

class CFlatVoxelTile : public CVoxelTile
{
};

class CTexturedVoxelTile : public CBaseVoxelTile
{
public :

    virtual void    LoadData                ( byte *pBase );
};

class CMapTile
{
public :

    CBaseVoxelTile  *m_pVoxelTile;

    bool            m_bUniq;
    int             m_nTileRefNum;

    // unique properties
    dword           m_dwTileHeight;
    char            m_bTileHasWater;
    float           *m_pfWaterHeights;

    CMapTile
        (
        )
    {
        m_pVoxelTile = NULL;
        m_dwTileHeight = 0;
        m_bTileHasWater = 0;
        m_bUniq = false;
    }

    ~CMapTile
        (
        )
    {
        if ( m_bUniq )
        {
            DEL( m_pVoxelTile );
        }
        else
        {
            m_pVoxelTile = NULL;
        }
    }
};

//
// landscape data class
//
class COCLandscapeData
{
private :

    byte            *m_pLND;

    inline byte     *GetLNDPtr              ( dword dwOffset ) { return m_pLND + dwOffset; };

public :

    char            *m_pBasePath;

    // base info
    TOCLndHeader    *m_pHeader;
    TOCLHChunk      *m_pChunks;
    COCStr          *m_pRecName;
    COCStr          *m_pRecVal;

    // sky for this land
    COCStr          *m_pSkyRecName;
    COCStr          *m_pSkyRecVal;

    // LND properties
    TOCLndProp      *m_pLndProp;
    TOCLndSize      *m_pLndSize;
    dword           m_dwXOrigin;
    dword           m_dwYOrigin;

    // first information record
    TOCLndRecHeader *m_pFirstRec;

    int             m_nNumTileContainers;
    CMapTile        **m_ppMapTiles;
    dword           m_dwMinHeight;
    dword           m_dwMaxHeight;

    float           m_fWaterHeights[ 8 ];

    dword           LoadFromMemory          ( const void *pData );
    CBaseVoxelTile  *FindVoxelTile          ( const char *pName, int &num );
    void            CalculateTileEdges      ( CMapTile *pMapTile );
    void            ClaculateEdgeDeltas     ( int nX, int nY );

    void            Dump                    ( const char *pTGAFileName );

                    COCLandscapeData        ();
                    ~COCLandscapeData       ();
};

//
// animation ( *.anm file )
//
typedef struct
{
    dword       dwSignature;
    dword       dwUnknown01;
    dword       dwUnknown02;
    word        wNumFrames;
    word        wNumJoints;
    dword       dwLength;
} TANMHeader;

typedef struct CANMJoint
{
    word        wIndex;
    word        wMask;
    short       *pwO[ 3 ];
    float       *pfD[ 3 ];

    word        fo[ 3 ];
    word        os[ 3 ];

    word        fd[ 3 ];
    word        ds[ 3 ];

    CANMJoint()
    {
        for ( int i = 0; i < 3; i++ )
        {
            pwO[ i ] = NULL;
            pfD[ i ] = NULL;
        }
    }

    ~CANMJoint()
    {
        for ( int i = 0; i < 3; i++ )
        {
            VDEL( pwO[ i ] );
            VDEL( pfD[ i ] );
        }
    }

} TANMJoint;

class COCAnimation
{
public :

    TANMHeader      m_Header;
    TANMJoint       *m_pJoints;

    dword           LoadFromMemory          ( const void *pData, int size );

                    COCAnimation            ();
                    ~COCAnimation           ();
};

//
// static mesh ( *.msh file )
//
typedef struct
{
    dword       dwSignature;
    dword       dwTypeID;
    dword       dwUnknown01;
    dword       dwUnknown02;
    dword       dwUnknown03;
    dword       dwObjectOffset;
    dword       dwMSHFileSize;
} TMSHHeader;

typedef struct
{
    dword       dwSignature;
    dword       dwTypeID;
    dword       dwUnknown01;
    dword       dwUnknown02;
    dword       dwNumLOD;
} TMSHObjHeader;

typedef struct
{
    float       x, y, z, w;
} TMSHVertex;

typedef struct
{
    word        m_wVertIdx[ 3 ];
    byte        m_bTextureID;
    byte        m_bFlag;
} TMSHPolygon;

typedef struct
{
    short       edgevt[ 3 ];
} TMSHPolyInfo;

typedef struct
{
    int         edgevt[ 3 ];
} TMSHPolyInfo2;

typedef struct
{
    float       u, v;
} TMSHTextureCoord;

typedef struct
{
    float       x, y, z;
} TMSHNormal;

typedef struct
{
    dword       dwUnknown01;
    char        sUnknown02[ 12 ];

    COCStr      *pFileNames[ 4 ];
} TMSHTextureInfo;

typedef struct
{
    dword       dwPolyCount;
    dword       dwPolyIndex;
    dword       dwVertCount;
    dword       dwVertIndex;
    float       fEnvelope;
} TAMHCluster;

typedef struct
{
    dword       dwKind;
    dword       dwClusterCount;
    dword       dwClusterIndex;
    dword       dwPolyIndex;
    XVec3       xvPos;

    // xform
    XMat3       mRot;
    XVec3       xvPivot;
} TAMHJoint;

typedef struct
{
    COCStr      *pName;
    word        index;
    XVec3       v, u;
    short       ox, oy, oz;
} TAMHModelJoint;

//
// MSH arrays
//
typedef struct
{
    int         m_nNumVerts;
    TMSHVertex  m_Verts[];
} TMSHVertexArray;

typedef struct
{
    int         m_nNumPolygons;
    TMSHPolygon m_Polygons[];
} TMSHPolygonArray;

typedef struct
{
    int         m_nNumPolygons;
    TMSHPolyInfo m_PolyInfo[];
} TMSHPolyInfoArray;

typedef struct
{
    int         m_nNumPolygons;
    TMSHPolyInfo2 m_PolyInfo[];
} TMSHPolyInfoArray2;

typedef struct
{
    int         m_nNumVerts;
    TMSHNormal  m_Normals[];
} TMSHVNormalArray;

typedef struct
{
    int         m_nNumPolygons;
    TMSHNormal  m_Normals[];
} TMSHFNormalArray;

typedef struct
{
    int         m_nNumPolygons;
    TMSHTextureCoord m_Coords[][ 3 ];
} TMSHTexCoordArray;

class COCMesh
{
public :

    TMSHHeader      *m_pHeader;
    TMSHObjHeader   *m_pObjHeader;

    TMSHVertexArray *m_pVerts;
    TMSHPolygonArray *m_pPolygons;

    TMSHPolyInfoArray *m_pPolyInfo;
    TMSHPolyInfoArray2 *m_pPolyInfo2;
    TMSHVNormalArray *m_pVertNormals;
    TMSHFNormalArray *m_pPolyNormals;
    TMSHTexCoordArray *m_pTexCoords;

    TMSHTextureInfo *m_pTexinfos;
    int             m_nNumTexInfos;

    //
    // AMH section
    //
    int             m_nNumClusters;
    TAMHCluster     *m_pClusters;

    int             m_nNumLimbs;
    TAMHJoint       *m_pLimbs;

    int             m_nNumJoints;
    TAMHModelJoint  *m_pJoints;

    dword           LoadFromMemory          ( const void *pData );
    
                    COCMesh                 ();
                    ~COCMesh                ();
};

//////////////////////////////////////////////////////////////////////
//
// Outcast world class ( *.wrd file )
//
//////////////////////////////////////////////////////////////////////
typedef struct
{
    dword           dwOffsetRecName;
    dword           dwOffsetPosition;
    dword           dwOffsetRotataion;
    dword           dwOffsetUknown01;
    dword           dwOffsetStr;
    dword           dwOffsetUknown02;
    dword           dwOffsetUknown03;

    dword           dwOffsetNextRec;
} TOCDecorRec;

typedef struct
{
    dword           dwOffsetPosition;
    dword           dwOffsetRotation;
    dword           dwOffsetScaling;
    dword           dwOffsetUnknown01;
    dword           dwOffsetStr;
    dword           dwOffsetType;
    dword           dwOffsetUnknown02;
    dword           dwOffsetUnknown03;

    dword           dwOffsetNextRec;
} TOCMarkRec;

typedef struct
{
    unsigned short  sAngle1;
    unsigned short  sAngle2;
    unsigned short  sAngle3;
} TOCRotation;

#define MRK_REC_TYPE    0

class COCMark
{
    friend class COCWorldMarks;

    void            *m_pMarkData;

public :

    char            m_pFileName[ 256 ];

    TOCMainHeader   *m_pHeader;

    inline byte     *GetMRKPtr              ( dword dwOffset ) { return (byte *)m_pMarkData + dwOffset; };
    dword           LoadFromMemory          ( const void *pData );

                    COCMark                 ();
                    ~COCMark                ();
};

class COCMarkEntity
{
public :

    int             m_nOCMarkID;

    float           m_Pos[ 3 ];
    TOCRotation     m_Rot;
};

class COCWorldMarks
{
private :

    byte            *m_pWRD;

    void            AddDecoration           ( TOCDecorRec *pRec );

public :

    inline byte     *GetWRDPtr              ( dword dwOffset ) { return m_pWRD + dwOffset; };

    TOCLndHeader    *m_pHeader;
    TOCLHChunk      *m_pChunks;
    TOCLndProp      *m_pWrdProp;
    TOCLndProp      *m_pMrkProp;

    dword           m_dwNumUnknown01;
    dword           m_dwNumDecorations;
    dword           m_dwNumUnknown02;
    dword           m_dwNumMarks;

    TOCDecorRec     *m_pStartRec;
    TOCMarkRec      *m_pStartMark;

    //
    // Marks and entities
    //
    COCMark         *m_pEMarks;
    int             m_nNumEMarks;
    COCMark         *GetEMark               ( int markID ) { return &m_pEMarks[ markID ]; };
    
    COCMarkEntity   *m_pEntities;
    int             m_nNumEntities;

    int             RegisterEMark           ( const char *pMarkName );
    dword           LoadFromMemory          ( const void *pData );

                    COCWorldMarks           ();
                    ~COCWorldMarks          ();
};

//////////////////////////////////////////////////////////////////////
//
// Outcast font class ( *.fnt file )
//
//////////////////////////////////////////////////////////////////////

typedef struct
{
    dword       dwChunkCount;

    // chunks, it has variable size I think. But all OC fonts has only 2 chunks.
    dword       dwUnknown01;
    dword       dwUnknown02;
    dword       dwUnknown03;
    dword       dwUnknown04;

    dword       dwUnknown05;
    
    dword       dwNumRecords;
} TOCFNTHeader;

typedef struct
{
    dword       dwOffsetRecName;    // type of resource ( in most cases it is PRlcBitmap )
    dword       dwOffsetCharLength;
    
    dword       dwOffsetNextRec;
} TOCFNTRecord;

class OCFNTChar
{
public :

    char            m_pBitmapName[ 256 ];
    CRBTSurface     *m_pBitmap;
    int             m_nLength;
};

class COCFont
{
private :

    byte            *m_pFNT;

public :

    TOCMainHeader   *m_pHeader;
    TOCFNTHeader    *m_pFNTHeader;

    OCFNTChar       *m_pChars; // set of compressed bitmaps (256)
    TOCFNTRecord    *m_pStartRec;

    inline byte     *GetFNTPtr              ( dword dwOffset ) { return m_pFNT + dwOffset; };
    dword           LoadFromMemory          ( const void *pData );
};

//////////////////////////////////////////////////////////////////////
//
// Outcast PAK file ( *.pak file )
//
//////////////////////////////////////////////////////////////////////

typedef struct
{
    dword       dwSignature;
    dword       dwTypeID;
    dword       dwMajVer;
    dword       dwMinVer;

    int         dwNumFiles;
} TOCPAKHeader;

typedef struct
{
    dword       dwOffsetData;
    dword       dwPackedSize;
    dword       dwNormalSize;
} TOCPAKFileInfo;

#endif // __OCDATA_H__
