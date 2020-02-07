#pragma once

#include "ray.h"
#include "random.h"

class hit_record;

class material {
public:
	virtual bool scatter(const ray& r_int, const hit_record& rec, vec3& attenuation, ray& scatterd) const = 0;
};

class lambertian : public material {
public:
	lambertian(const vec3& a) : albedo(a) {}
	virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const;

	vec3 albedo;
};

class metal : public material {
public:
	vec3 albedo;
	float fuzz;

	metal(const vec3& a, float f) : albedo(a) {
		if (f < 1) fuzz = f; else fuzz = 1;
	}
	virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const;
};

inline vec3 reflect(const vec3& v, const vec3& n) {
	return v - 2 * dot(v, n) * n;
}
