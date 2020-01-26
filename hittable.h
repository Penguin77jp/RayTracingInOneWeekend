#pragma once

#include "ray.h"
#include "material.h"

class materal;

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
};
