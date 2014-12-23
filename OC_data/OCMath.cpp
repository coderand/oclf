// This file is distributed under a MIT license. See LICENSE.txt for details.
//
// Outcast like math
//

#include "OCMath.h"
#include <math.h>
#include <stdlib.h>

OCMtx OCMtx::from_euler
    (
    float yaw,
    float pitch,
    float roll
    )
{
    OCMtx r;
    float sy = (float) sin (yaw);
    float cy = (float) cos (yaw);
    float sp = (float) sin (pitch);
    float cp = (float) cos (pitch);
    float sr = (float) cos (roll);
    float cr = (float) sin (roll);
    float spsy = sp * sy;
    float spcy = sp * cy;
    r.xx = sr * cp;
    r.xy = sr * spsy - cr *cy;
    r.xz = sr * spcy + cr *sy;
    r.xt = 0.f;
    r.yx = cr * cp;
    r.yy = cr * spsy + sr * cy;
    r.yz = cr * spcy - sr * sy;
    r.yt = 0.f;
    r.zx = - sp;
    r.zy = cp * sy;
    r.zz = cp * cy;
    r.zt = 0.f;

    return r;
}

OCVec OCMtx::apply
    (
    const OCVec& p
    ) const
{
    OCVec v;

    v.x = xx * p.x + xy * p.y + xz * p.z + xt;
    v.y = yx * p.x + yy * p.y + yz * p.z + yt;
    v.z = zx * p.x + zy * p.y + zz * p.z + zt;

    return v;
}

OCMtx OCMtx::operator *
    (
    const OCMtx& n
    ) const
{
    OCMtx r;

    r.xx = xx * n.xx + xy * n.yx + xz * n.zx;
    r.xy = xx * n.xy + xy * n.yy + xz * n.zy;
    r.xz = xx * n.xz + xy * n.yz + xz * n.zz;
    r.xt = xx * n.xt + xy * n.yt + xz * n.zt + xt;
    
    r.yx = yx * n.xx + yy * n.yx + yz * n.zx;
    r.yy = yx * n.xy + yy * n.yy + yz * n.zy;
    r.yz = yx * n.xz + yy * n.yz + yz * n.zz;
    r.yt = yx * n.xt + yy * n.yt + yz * n.zt + yt;
    
    r.zx = zx * n.xx + zy * n.yx + zz * n.zx;
    r.zy = zx * n.xy + zy * n.yy + zz * n.zy;
    r.zz = zx * n.xz + zy * n.yz + zz * n.zz;
    r.zt = zx * n.xt + zy * n.yt + zz * n.zt + zt;

    return r;
}

void OCMtx::setid
    (
    )
{
    xx = 1.f;
    xy = 0.f;
    xz = 0.f;
    xt = 0.f;
    
    yx = 0.f;
    yy = 1.f;
    yz = 0.f;
    yt = 0.f;
    
    zx = 0.f;
    zy = 0.f;
    zz = 1.f;
    zt = 0.f;
}

OCMtx OCMtx::from_vector
    (
    const OCVec& v
    )
{
    OCMtx r;

    r.xx = 1.f;
    r.xy = 0.f;
    r.xz = 0.f;
    r.xt = v.x;
    
    r.yx = 0.f;
    r.yy = 1.f;
    r.yz = 0.f;
    r.yt = v.y;
    
    r.zx = 0.f;
    r.zy = 0.f;
    r.zz = 1.f;
    r.zt = v.z;

    return r;
}

OCMtx OCMtx::from_float9
    (
    const float * f
    )
{
    OCMtx r;

    r.xx = f[0];
    r.xy = f[1];
    r.xz = f[2];
    r.xt = 0.f;
    
    r.yx = f[3];
    r.yy = f[4];
    r.yz = f[5];
    r.yt = 0.f;
    
    r.zx = f[6];
    r.zy = f[7];
    r.zz = f[8];
    r.zt = 0.f;

    return r;
}

OCMtx OCMtx::transposed
    (
    ) const
{
    OCMtx r;

    r.xx = xx;
    r.xy = yx;
    r.xz = zx;
    r.xt = xt;
    r.yx = xy;
    r.yy = yy;
    r.yz = zy;
    r.yt = yt;
    r.zx = xz;
    r.zy = yz;
    r.zz = zz;
    r.zt = zt;

    return r;
}

float OCQuat::magnitude_squared
    (
    ) const
{
    return i * i + j * j + k * k + s * s;
}

float OCQuat::magnitude
    (
    ) const
{
    return (float) sqrt (magnitude_squared ());
}

OCMtx OCQuat::to_matrix
    (
    ) const 
{
    OCMtx r;

    float S = 2.f / magnitude ();
    
    float w = s;
    float x = i;
    float y = j;
    float z = k;
    
    float xs = x * S;
    float ys = y * S;
    float zs = z * S;
    
    float wx = w * xs;
    float wy = w * ys;
    float wz = w * zs;
    
    float xx = x * xs;
    float xy = x * ys;
    float xz = x * zs;
    
    float yy = y * ys;
    float yz = y * zs;
    
    float zz = z * zs;
    
    r.zz = 1.f - yy - zz;
    r.zy = xy + wz;
    r.zx = xz - wy;
    r.xt = 0.f;
    
    r.yz = xy - wz;
    r.yy = 1.f - xx - zz;
    r.yx = yz + wx;
    r.yt = 0.f;
    
    r.xz = xz + wy;
    r.xy = yz - wx;
    r.xx = 1.0f - xx - yy;
    r.zt = 0.f;

    return r;
}

void OCQuat::to_axis_angle
    (
    OCVec& axis,
    float& theta)
{
    float magsq = i * i + j * j + k * k;

    if ( magsq > 0.f )
    {
        float t = 1.f / sqrtf (magsq);
        axis.x = k * t;
        axis.y = j * t;
        axis.z = k * t;
        theta = 2.f * (float) acos (s);
    }
    else
    {
        axis.x = 1.f;
        axis.y = 0.f;
        axis.z = 0.f;
        theta = 0.f;
    }
}

OCQuat OCQuat::operator *
    (
    const OCQuat& p
    ) const
{
    OCQuat r;

    r.s = s * p.s - (i * p.i + j * p.j + k * p.k);
    r.i = s * p.i + p.s * i + j * p.k - k * p.j;
    r.j = s * p.j + p.s * j + k * p.i - i * p.k;
    r.k = s * p.k + p.s * k + i * p.j - j * p.i;

    return r;
}

OCQuat OCQuat::operator *
    (
    const float scale
    ) const
{
    OCQuat r;

    r.s = s * scale;
    r.i = i * scale;
    r.j = j * scale;
    r.k = k * scale;

    return r;
}

OCQuat OCQuat::from_matrix
    (
    const OCMtx& m
    )
{
    OCQuat r;
    
    float qs2 = 0.25f * (m.xx + m.yy + m.zz + 1.0f);
    float qx2 = qs2 - 0.5f * (m.yy + m.xx);
    float qy2 = qs2 - 0.5f * (m.zz + m.xx);
    float qz2 = qs2 - 0.5f * (m.zz + m.yy);
    int n = (qs2 > qx2) 
        ? ((qs2 > qy2) 
        ? ((qs2 > qz2) ? 0 : 3)
        : ((qy2 > qz2) ? 2 : 3))
        : ((qx2 > qy2)
        ? ((qx2 > qz2) ? 1 : 3)
        : ((qy2 > qz2) ? 2 : 3));
    
#define R                                               \
    _mag = sqrtf (_i * _i + _j * _j + _k * _k + _s * _s);  \
    _mag = 1.f / (_s < 0.0 ? - _mag :  _mag);             \
    r.i = _i * _mag;                                      \
    r.j = _j * _mag;                                      \
    r.k = _k * _mag;                                      \
    r.s = _s * _mag;                                      \
    return r; break
    
    float _i, _j, _k, _s, _mag, _tmp;
    
    switch (n) {
    case 0:
        _s = sqrtf (qs2);
        _tmp = 0.25f / _s;
        _i = (m.yx - m.xy) * _tmp;
        _j = (m.xz - m.zx) * _tmp;
        _k = (m.zy - m.yz) * _tmp; 
        R;
        
    case 1:
        _i = sqrtf (qx2);
        _tmp = 0.25f / _i;
        _s = (m.yx - m.xy) * _tmp;
        _j = (m.zy + m.yz) * _tmp;
        _k = (m.zx + m.xz) * _tmp;
        R;
        
    case 2:
        _j = sqrtf (qy2);
        _tmp = 0.25f / _j;
        _s = (m.xz - m.zx) * _tmp;
        _k = (m.yx + m.xy) * _tmp;
        _i = (m.yz + m.zy) * _tmp;
        R;
        
    case 3:
        _k = sqrtf (qz2);
        _tmp = 0.25f / _k;
        _s = (m.zy - m.yz) * _tmp;
        _i = (m.xz + m.zx) * _tmp;
        _j = (m.xy + m.yx) * _tmp;
        R;
        
    default: 
        abort ();
#if defined __BORLANDC__ || defined __ICC
        return r;
#endif
    }
}

void OCQuat::conjugate
    (
    )
{
    i = -i;
    j = -j;
    k = -k;
}

OCQuat OCQuat::sscale
    (
    const float t
    ) const
{
    OCQuat r;

    float cosphi;
    bool flip;
    
    if ( s < 0.f )
    {
        cosphi = - s;
        flip = true;
    }
    else
    {
        cosphi = s;
        flip = false;
    }
    
    if ( 1.f - cosphi < 1e-8 )
    {
        r.i = i;
        r.j = j;
        r.k = k;
        r.s = s;
        return r;
    }
    else
    {
        float phi = (float) acos (cosphi);
        float sphi = 1.f / (float) sin (phi);
        float k0 = (float) sin ((1.f - t) * phi) * sphi;
        float k1 = (float) sin (t * phi) * sphi;
        k1 = flip ? - k1 : k1;
        r.i = i * k1;
        r.j = j * k1;
        r.k = k * k1;
        r.s = s * k1 + k0;
        return r;
    }
}

OCQuat OCQuat::from_euler
    (
    float yaw, 
    float pitch, 
    float roll
    )
{
    OCQuat r;

    float half_yaw = yaw / 2.0f;
    float half_pitch = pitch / 2.0f;
    float half_roll = roll / 2.0f;
    
    float cosYaw = (float) cos (half_yaw);
    float cosRoll = (float) cos (half_roll);
    float cosPitch = (float) cos (half_pitch);
    float sinYaw = (float) sin (half_yaw);
    float sinRoll = (float) sin (half_roll);
    float sinPitch = (float) sin (half_pitch);
    
    r.i = sinRoll * cosPitch * cosYaw - cosRoll * sinPitch * sinYaw;
    r.j = cosRoll * sinPitch * cosYaw + sinRoll * cosPitch * sinYaw;
    r.k = cosRoll * cosPitch * sinYaw - sinRoll * sinPitch * cosYaw;
    r.s = cosRoll * cosPitch * cosYaw + sinRoll * sinPitch * sinYaw;

    return r;
}
