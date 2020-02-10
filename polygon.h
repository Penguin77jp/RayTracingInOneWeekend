#pragma once

#include "hittable.h"
#include "ray.h"

class polygon : public hittable{
private :
  vec3 v0, v1, v2;
  material* mat_ptr;
  
public :
  polygon(vec3 v0,vec3 v1, vec3 v2, material* m) : v0(v0),v1(v1),v2(v2),mat_ptr(m){}
  virtual bool hit(const ray& r, float tmin, float tmax, hit_record& rec) const;
};

bool polygon::hit(const ray& r, float tmin, float tmax, hit_record& rec) const {
  vec3 a = v0 - r.origin;
  vec3 b = v1 - v0;
  vec3 c = v2 - v0;
  vec3 dir = r.direction();

}
