// This file is distributed under a MIT license. See LICENSE.txt for details.
//
// class COCStr
//

#include "common.h"

bool COCStr::Cmp
    ( 
    const char *pStr
    )
{
    return !strncmp( m_String, pStr, m_nLength );
}

void COCStr::CopyTo
    ( 
    char *pStr
    )
{
    memcpy( pStr, m_String, m_nLength );
    pStr[ m_nLength ] = 0;
}
