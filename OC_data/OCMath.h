// This file is distributed under a MIT license. See LICENSE.txt for details.
//
// Outcast like math
//

#ifndef __OCMATH_H__
#define __OCMATH_H__

class OCVec
{
public:
    float x, y, z;
    
    OCVec ():x(0.f),y(0.f),z(0.f){}
    OCVec (float _x, float _y, float _z):x(_x),y(_y),z(_z){}
    OCVec (float * v):x(v[0]),y(v[1]),z(v[2]){}
    OCVec operator - () const { return OCVec (- x, - y, - z); }
    OCVec operator + (const OCVec& v) const {
        return OCVec (x + v.x, y + v.y, z + v.z);
    }
    void operator-=( const OCVec &a ) { x-=a.x; y-=a.y; z-=a.z;}
};

inline OCVec operator*( const OCVec &a, float f ) 
{ 
    return OCVec(a.x * f, a.y * f, a.z * f);
}

class OCMtx
{
public:
    float xx, xy, xz, xt;
    float yx, yy, yz, yt;
    float zx, zy, zz, zt;

    OCVec apply (const OCVec&) const;
    OCMtx operator * (const OCMtx&) const;
    void scale( float f ) 
    { 
        xx *= f; xy *= f; xz *= f; xt *= f;
        yx *= f; yy *= f; yz *= f; yt *= f;
        zx *= f; zy *= f; zz *= f; zt *= f;
    }
    void add( const OCMtx &a ) 
    {
        xx += a.xx; xy += a.xy; xz += a.xz; xt += a.xt;
        yx += a.yx; yy += a.yy; yz += a.yz; yt += a.yt;
        zx += a.zx; zy += a.zy; zz += a.zz; zt += a.zt;
    }
    OCMtx transposed () const;
    static OCMtx from_euler (float, float, float);
    static OCMtx from_vector (const OCVec&);
    static OCMtx from_float9 (const float *);
    void setid ();
};

class OCQuat
{
public:

    float i, j, k, s;

    OCQuat ():i(0.f),j(0.f),k(0.f),s(0.f){}
    OCQuat (float s):i(0.f),j(0.f),k(0.f),s(s){}
    OCMtx to_matrix () const;
    OCQuat operator * (const OCQuat&) const; // spherical scale (slerp ID q)
    OCQuat operator * (const float) const;
    OCQuat sscale (const float) const;
    void conjugate ();
    float magnitude_squared () const;
    float magnitude () const;
    void to_axis_angle (OCVec&, float&);
    static OCQuat from_matrix (const OCMtx&);
    static OCQuat from_euler (float, float, float);
};

#endif // __OCMATH_H__
