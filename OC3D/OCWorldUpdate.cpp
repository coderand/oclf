// This file is distributed under a MIT license. See LICENSE.txt for details.
//
// OCWorldUpdate.cpp
//

#include "common.h"

//
// Update
//
bool OCWorld::Update
    (
    )
{
    m_pPlayer->UpdateInput( ocinput );

    return true;
}
