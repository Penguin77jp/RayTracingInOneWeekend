#pragma once

#include "hittable.h"
#include "ray.h"
#include "matrix.h"

class polygon : public hittable {
private:
	vec3 v0, v1, v2;
	material* mat_ptr;

public:
	polygon(vec3 v0, vec3 v1, vec3 v2, material* m) : v0(v0), v1(v1), v2(v2), mat_ptr(m) {}
	virtual bool hit(const ray& r, float tmin, float tmax, hit_record& rec) const;
};

bool polygon::hit(const ray& r, float tmin, float tmax, hit_record& rec) const {
	vec3 b = r.origin() - v0;
	vec3 dir = r.direction();
	vec3 dir_u = v1 - v0;
	vec3 dir_v = v2 - v0;
	vec3 normal = unit_vector(cross(v1 - v0, v2 - v0));

	//normal check
	if (dot(normal, r.direction()) < 0) {
		return false;
	}

	mat3x3 a(dir_u.x(), dir_v.x(), -dir.x(), dir_u.y(), dir_v.y(), -dir.y(), dir_u.z(), dir_v.z(), -dir.z());
	mat3x3 a1 = a;
	mat3x3 a2 = a;
	mat3x3 a3 = a;
	a1.m11 = a2.m12 = a3.m13 = b.x();
	a1.m21 = a2.m22 = a3.m23 = b.y();
	a1.m31 = a2.m32 = a3.m33 = b.z();
	float u = a1.det() / a.det();
	float v = a2.det() / a.det();
	float t = a3.det() / a.det();

	if (tmin < t && t < tmax) {
		if (0.0 <= v && v <= 1.0 && 0.0 <= u && u <= 1.0 && 0.0 <= u + v && u + v <= 1.0) {
			rec.t = t;
			rec.normal = normal;
			rec.p = r.point_at_parameter(rec.t);
			rec.mat_ptr = mat_ptr;
			return true;
		}
	}
	return false;
}
