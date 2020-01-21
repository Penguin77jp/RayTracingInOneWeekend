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

constexpr double g_size = 1.0; // = 960x540
constexpr int WINDOW_WIDTH = 960 * g_size;
constexpr int WINDOW_HEIGHT = 540 * g_size;
constexpr int fullScreen = false;
const double screen_distance = 0.2f;

constexpr float FLOAT_INFINITY = std::numeric_limits<float>::max();

int InitWindow(std::array<RGBA, WINDOW_WIDTH * WINDOW_HEIGHT >& pix_data, GLFWwindow** window) {
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


	*window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Tetris", monitorTarget, NULL);
	if (!window) {
		glfwTerminate();
		return 0;
	}
	glfwMakeContextCurrent(*window);
	glfwSwapInterval(1);

	return 1;
}

void UpdateWindow(std::array<RGBA, WINDOW_WIDTH * WINDOW_HEIGHT >& pix_data, GLFWwindow** window) {
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

vec3 color(const ray& r, hittable* world) {
	hit_record rec;
	if (world->hit(r, 0.0, std::numeric_limits<float>::max(), rec)) {
		return 0.5 * vec3(rec.normal.x() + 1, rec.normal.y() + 1, rec.normal.z() + 1);
	} else {
		vec3 unit_direction = unit_vector(r.direction());
		float t = 0.5 * (unit_direction.y() + 1.0);
		return (1.0 - t) * vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0);
	}
}

int main() {
	std::array<RGBA, WINDOW_WIDTH * WINDOW_HEIGHT> pix_data{};
	GLFWwindow* glfWindowp = nullptr;
	GLFWwindow** glfwWindowpp = &glfWindowp;
	InitWindow(pix_data, glfwWindowpp);

	std::chrono::system_clock::time_point p = std::chrono::system_clock::now();

	vec3 lower_left_corner(-2.0, -1.0, -1.0);
	vec3 horizontal(4.0, 0.0, 0.0);
	vec3 vertical(0.0, 2.0, 0.0);
	vec3 origin(0.0, 0.0, 0.0);

	hittable* list[2];
	list[0] = new sphere(vec3(0, 0, -1), 0.5);
	list[1] = new sphere(vec3(0, -100.5, -1), 100);
	hittable* world = new hittable_list(list, 2);

	while (!glfwWindowShouldClose(*glfwWindowpp)) {
		p = std::chrono::system_clock::now();

		for (int y = 0; y < WINDOW_HEIGHT; ++y) {
			for (int x = 0; x < WINDOW_WIDTH; ++x) {
				int index = x + y * WINDOW_WIDTH;

				float u = float(x) / float(WINDOW_WIDTH);
				float v = float(y) / float(WINDOW_HEIGHT);
				ray r(origin, lower_left_corner + u * horizontal + v * vertical);
				vec3 p = r.point_at_parameter(2.0);
				vec3 col = color(r, world);
				pix_data[index].r = int(255.99 * col[0]);
				pix_data[index].g = int(255.99 * col[1]);
				pix_data[index].b = int(255.99 * col[2]);
			}
		}

		UpdateWindow(pix_data, glfwWindowpp);

		const float dtMS = static_cast<float>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - p).count());
		std::cout << dtMS << "ms (" << (int)(1000.0 / dtMS) << "FPS)" << std::endl;
	}

	return 0;
}