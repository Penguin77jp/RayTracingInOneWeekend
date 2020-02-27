#include <GLFW/glfw3.h>
#include <iostream>
#include <time.h>
#include <vector>
#include <array>
#include <chrono>
#include <random>

#include "vec3.h"
#include "ray.h"
#include "color.h"
#include "hittablelist.h"
#include "sphere.h"
#include "camera.h"
#include "random.h"
#include "bvh.h"

constexpr int WINDOW_WIDTH = 1920*0.5;
constexpr int WINDOW_HEIGHT = 1080*0.5;
constexpr double g_size = 0.05;
constexpr int fullScreen = false;
const double screen_distance = 0.2f;

constexpr float FLOAT_INFINITY = std::numeric_limits<float>::max();

int InitWindow(std::array<RGBA, (int)(WINDOW_WIDTH * WINDOW_HEIGHT)>& pix_data, GLFWwindow** window) {
	// GLFWの初期化
	if (!glfwInit()) {
		return 0;
	}

	// ウインドウの作成
	GLFWmonitor* monitorTarget;
	if (!fullScreen)
		monitorTarget = NULL;
	else
		monitorTarget = glfwGetPrimaryMonitor();


	*window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "RayTracing", monitorTarget, NULL);
	if (!window) {
		glfwTerminate();
		return 0;
	}
	glfwMakeContextCurrent(*window);
	glfwSwapInterval(1);

	return 1;
}

void UpdateWindow(std::array<RGBA, (int)(WINDOW_WIDTH * WINDOW_HEIGHT) >& pix_data, GLFWwindow** window) {
	glClear(GL_COLOR_BUFFER_BIT);
	glDrawPixels(WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, &pix_data[0]);
	glfwSwapBuffers(*window);
	glfwPollEvents();
}

float hit_sphere(const vec3& center, float radius, const ray& r) {
	vec3 oc = r.origin() - center;
	float a = dot(r.direction(), r.direction());
	float b = 2.0 * dot(oc, r.direction());
	float c = dot(oc, oc) - radius * radius;
	float discriminant = b * b - 4 * a * c;
	if (discriminant < 0) {
		return -1.0;
	} else {
		return (-b - sqrt(discriminant)) / (2.0 * a);
	}
}

vec3 color(const ray& r, hittable* world, int depth) {
	hit_record rec;
	if (world->hit(r, 0.001, std::numeric_limits<float>::max(), rec)) {
		ray scattered;
		vec3 attenuation;
		if (depth < 50 && rec.mat_ptr->scatter(r, rec, attenuation, scattered)) {
			return attenuation * color(scattered, world, depth + 1);
		} else {
			return vec3(0, 0, 0);
		}
	} else {
		vec3 unit_direction = unit_vector(r.direction());
		float t = 0.5 * (unit_direction.y() + 1.0);
		return (1.0 - t) * vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0);
	}
}

hittable* random_scene() {
	int n = 50000;
	hittable** list = new hittable * [n + 1];
	//texture* checker = new checker_texture(new constant_texture(vec3(0.2, 0.3, 0.1)), new constant_texture(vec3(0.9, 0.9, 0.9)));
	list[0] = new sphere(vec3(0, -1000, 0), 1000, new lambertian(vec3(0.2, 0.3, 0.1)));
	int i = 1;
	for (int a = -10; a < 10; a++) {
		for (int b = -10; b < 10; b++) {
			float choose_mat = random_double();
			vec3 center(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());
			if ((center - vec3(4, 0.2, 0)).length() > 0.9) {
				if (choose_mat < 0.8) {  // diffuse
					list[i++] = new moving_sphere(center, center + vec3(0, 0.5 * random_double(), 0), 0.0, 1.0, 0.2, new lambertian(vec3(random_double() * random_double(), random_double() * random_double(), random_double() * random_double())));
				} else if (choose_mat < 0.95) { // metal
					list[i++] = new sphere(center, 0.2,
										   new metal(vec3(0.5 * (1 + random_double()), 0.5 * (1 + random_double()), 0.5 * (1 + random_double())), 0.5 * random_double()));
				} else {  // glass
					//list[i++] = new sphere(center, 0.2, new dielectric(1.5));
				}
			}
		}
	}

	//list[i++] = new sphere(vec3(0, 1, 0), 1.0, new dielectric(1.5));
	list[i++] = new sphere(vec3(-4, 1, 0), 1.0, new lambertian(vec3(0.4, 0.2, 0.1)));
	list[i++] = new sphere(vec3(4, 1, 0), 1.0, new metal(vec3(0.7, 0.6, 0.5), 0.0));

	//return new hittable_list(list,i);
	return new bvh_node(list, i, 0.0, 1.0);
}

int main() {
	const int sample_width = WINDOW_WIDTH * g_size;
	const int sample_height = WINDOW_HEIGHT * g_size;
	std::array<vec3, sample_width * sample_height> pix_data{};
	std::array<RGBA, sample_width * sample_height> pix_data_sqrt{};
	std::array<RGBA, WINDOW_WIDTH * WINDOW_HEIGHT> pix_data_expand{};
	GLFWwindow* glfWindowp = nullptr;
	GLFWwindow** glfwWindowpp = &glfWindowp;
	InitWindow(pix_data_expand, glfwWindowpp);

	std::chrono::system_clock::time_point p = std::chrono::system_clock::now();

	const int ns = 1;
	int samples = 0;
	vec3 lookfrom(13, 3, 2);
	vec3 lookat(0, 0, 0);
	float dist_to_focus = 10;
	float aperture = 0.0;

	camera cam(
		lookfrom, lookat, vec3(0, 1, 0), 20, float(WINDOW_WIDTH) / float(WINDOW_HEIGHT), aperture,
		dist_to_focus, 0.0, 1.0);
	hittable* world = random_scene();

	while (!glfwWindowShouldClose(*glfwWindowpp)) {
		p = std::chrono::system_clock::now();
		samples += ns;
		std::cout << samples << "samples" << std::endl;
		for (int y = 0; y < sample_height; ++y) {
#pragma omp parallel for
			for (int x = 0; x < sample_width; ++x) {
				int index = x + y * sample_width;
				vec3 col(0, 0, 0);
				for (int s = 0; s < ns; s++) {
					float u = float(x + random_double()) / float(sample_width);
					float v = float(y + random_double()) / float(sample_height);
					ray r = cam.get_ray(u, v);
					col += color(r, world, 0);
				}
				const int samples_n = samples / ns;
				pix_data[index][0] = 1.0 * (samples_n - 1) / samples_n * pix_data[index][0] + 1.0 / samples_n / ns * col[0];
				pix_data[index][1] = 1.0 * (samples_n - 1) / samples_n * pix_data[index][1] + 1.0 / samples_n / ns * col[1];
				pix_data[index][2] = 1.0 * (samples_n - 1) / samples_n * pix_data[index][2] + 1.0 / samples_n / ns * col[2];

				pix_data_sqrt[index].r = (int)(255.99 * sqrt(pix_data[index][0]));
				pix_data_sqrt[index].g = (int)(255.99 * sqrt(pix_data[index][1]));
				pix_data_sqrt[index].b = (int)(255.99 * sqrt(pix_data[index][2]));
			}
		}

		//expand texture
		for (int y = 0; y < WINDOW_HEIGHT; ++y) {
			for (int x = 0; x < WINDOW_WIDTH; ++x) {
				pix_data_expand[x + y * WINDOW_WIDTH] = pix_data_sqrt[(int)(x * g_size) + (int)(y * g_size) * sample_width];
			}
		}

		UpdateWindow(pix_data_expand, glfwWindowpp);

		const float dtMS = static_cast<float>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - p).count());
		std::cout << dtMS << "ms (" << (int)(1000.0 / dtMS) << "FPS)" << std::endl;
	}

	return 0;
}
