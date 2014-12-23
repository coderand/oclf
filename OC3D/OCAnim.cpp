// This file is distributed under a MIT license. See LICENSE.txt for details.
//
// Outcast ANM animation object by AND 2003
//

#include "common.h"
#include <string.h>
#include "oc3d.h"

//
// Constructor
//
OCAnim::OCAnim
    (
    )
{
    m_mStack = NULL;
    m_xvBoneStack = NULL;
}

//
// Destructor
//
OCAnim::~OCAnim
    (
    )
{
    if ( m_mStack ) { VDEL( m_mStack ); m_mStack = NULL; };
    if ( m_xvBoneStack ) { VDEL( m_xvBoneStack ); m_xvBoneStack = NULL; };
}

//
// Load animation from file
//
void OCAnim::Load
    (
    const char *pFileName
    )
{
    static char fileName[ 256 ];
    int size;
    HXFSFile fp;

    strcpy( fileName, "Anim/" );
    strcat( fileName, pFileName );
    xfs->FixFileName( fileName );

    fp = xfs->Open( fileName );
    if ( fp )
    {
        size = xfs->SizeOf( fp );
        void *data = ALLOCATE( size );
        xfs->Read( fp, data, size );
        m_ANM.LoadFromMemory( data, size );
        xfs->Close( fp );
        FREE( data );
    }
    else
        DPrint( "ANIM file not found" );

    //
    // Create stacks
    //
    m_mStack = NEW OCMtx[ m_ANM.m_Header.wNumJoints ];
    m_xvBoneStack = NEW OCVec[ m_ANM.m_Header.wNumJoints ];
}
