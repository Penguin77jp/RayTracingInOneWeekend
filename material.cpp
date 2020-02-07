#include "material.h"
#include "hittable.h"

bool lambertian::scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const {
  vec3 target = rec.p + rec.normal + random_in_unit_sphere();
  scattered = ray(rec.p, target - rec.p,r_in.time());
  attenuation = albedo;
  return true;
}

bool metal::scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const {
  vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
  scattered = ray(rec.p, reflected + fuzz * random_in_unit_sphere(),r_in.time());
  attenuation = albedo;
  return (dot(scattered.direction(), rec.normal) > 0);
}
