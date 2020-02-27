#pragma once

#define _USE_MATH_DEFINES
#include <math.h>

#include "ray.h"
#include "material.h"
#include "aabb.h"

class materal;

inline void get_sphere_uv(const vec3& p, float& u, float& v) {
    float phi = atan2(p.z(), p.x());
    float theta = asin(p.y());
    u = 1 - (phi + M_PI) / (2 * M_PI);
    v = (theta + M_PI / 2) / M_PI;
}

class hit_record {
public :
    float t;
    vec3 p;
    vec3 normal;
    material* mat_ptr;
    hit_record(): t(0),p(),normal(),mat_ptr(0) {}
};

class hittable {
public:
    virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const = 0;
    virtual bool bounding_box(float t0, float t1, aabb& box) const = 0;
};