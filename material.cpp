#include "material.h"
#include "hittable.h"

vec3 material::emitted(const vec3& p) const {
	return vec3(0, 0, 0);
}

bool lambertian::scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const {
	vec3 target = rec.p + rec.normal + random_in_unit_sphere();
	scattered = ray(rec.p, target - rec.p, r_in.time());
	attenuation = albedo->value(rec.p);
	return true;
}

metal::metal(const vec3& a, float f) {
	this->albedo = a;
	if (f < 1)
		this->fuzz = f;
	else
		this->fuzz = 1;
}

bool metal::scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const {
	vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
	scattered = ray(rec.p, reflected + fuzz * random_in_unit_sphere(), r_in.time());
	attenuation = albedo;
	return (dot(scattered.direction(), rec.normal) > 0);
}

bool diffuse_light::scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const {
	return false;
}

vec3 diffuse_light::emitted(const vec3& p) const {
	return emit->value(p);
}
