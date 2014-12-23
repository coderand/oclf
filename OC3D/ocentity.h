// This file is distributed under a MIT license. See LICENSE.txt for details.
//
// Outcast logical system by AND 2003
//

#ifndef __OCENTITY_H__
#define __OCENTITY_H__

class OCObject;

//
// Base logical entity
//
class OCEntity
{
public :

    OCObject        *m_pObj;

    void            Think               ();
    void            Render              ();
};

#endif // __OCENTITY_H__
