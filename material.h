#pragma once

#include "ray.h"
#include "random.h"
#include "texture.h"
#include "aabb.h"

class hit_record;

class material {
public:
	virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const = 0;
	virtual vec3 emitted(const vec3& p) const;
};

class lambertian : public material {
public:
	lambertian(texture* a) : albedo(a) {}
	virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const;

	texture* albedo;
};

class metal : public material {
public:
	vec3 albedo;
	float fuzz;

	metal(const vec3& a, float f);
	virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const;
};

inline vec3 reflect(const vec3& v, const vec3& n) {
	return v - 2 * dot(v, n) * n;
}

class diffuse_light : public material {
public:
	diffuse_light(texture* a) : emit(a) {}
	bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const override;
	vec3 emitted(const vec3& p) const override;
	texture* emit;
};
