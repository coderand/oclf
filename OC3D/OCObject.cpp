// This file is distributed under a MIT license. See LICENSE.txt for details.
//
// Outcast general object by AND 2002
//

#include "common.h"
#include <string.h>
#include "oc3d.h"

//
// Constructor
//
OCObject::OCObject
    (
    )
{
    m_nFlags = EOBJF_NONE;
    m_xvPos.xyz( 0, 0, 0 );
    m_xeRot.setYPR( 0, 0, 0 );
}

//
// Destructor
//
OCObject::~OCObject
    (
    )
{
}
