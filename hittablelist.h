#pragma once
#include "hittable.h"
#include "mesh.h"
#include "sphere.h"

class hittable_list : public hittable {
public:
  hittable_list() {}
  hittable_list(hittable** l, int n) { list = l; list_size = n; }
  virtual bool hit(
    const ray& r, float tmin, float tmax, hit_record& rec) const;
  hittable** list;
  int list_size;
};

bool hittable_list::hit(const ray& r, float t_min, float t_max,
                        hit_record& rec) const {

  hit_record temp_rec;
  bool hit_anything = false;
  double closest_so_far = t_max;
  for (int i = 0; i < list_size; i++) {
    if (list[i]->hit(r, t_min, closest_so_far, temp_rec)) {
      hit_anything = true;
      closest_so_far = temp_rec.t;
      rec = temp_rec;
    }
  }
  return hit_anything;
}

class hittable_vector : public hittable {
public:
  std::vector<hittable*> hittable_vec;

  hittable_vector() {}
  hittable_vector(std::vector<hittable*> vec) :hittable_vec(vec) {}
  void AddHittable(mesh mesh_data) {
    for (int i = 0; i < mesh_data.polygons.size(); ++i) {
      hittable_vec.push_back(mesh_data.polygons[i]);
    }
  }
  virtual bool hit(
    const ray& r, float tmin, float tmax, hit_record& rec) const;
};

bool hittable_vector::hit(const ray& r, float t_min, float t_max,
                          hit_record& rec) const {

  hit_record temp_rec;
  bool hit_anything = false;
  double closest_so_far = t_max;
  for (int i = 0; i < hittable_vec.size(); i++) {
    if (hittable_vec[i]->hit(r, t_min, closest_so_far, temp_rec)) {
      hit_anything = true;
      closest_so_far = temp_rec.t;
      rec = temp_rec;
    }
  }
  return hit_anything;
}
