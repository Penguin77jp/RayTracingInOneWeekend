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
#include "SceneData.h"
#include "include/vWriter.h"

constexpr int WINDOW_WIDTH = 1920 * 0.5;
constexpr int WINDOW_HEIGHT = 1080 * 0.5;
constexpr double g_size = 1.0;
constexpr int fullScreen = false;
const double screen_distance = 0.2f;
constexpr int sample_width = WINDOW_WIDTH * g_size;
constexpr int sample_height = WINDOW_HEIGHT * g_size;
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
	if (world->hit(r, 0.001, FLOAT_INFINITY, rec)) {
		ray scattered;
		vec3 attenuation;
		vec3 emitted = rec.mat_ptr->emitted(rec.p);
		if (depth < 50 && rec.mat_ptr->scatter(r, rec, attenuation, scattered)) {
			return emitted + attenuation * color(scattered, world, depth + 1);
		} else {
			return emitted;
		}
	} else {
		return vec3(0, 0, 0);
	}
}

void render(std::array<vec3, sample_width * sample_height>& pix_data, std::array<RGBA, sample_width * sample_height>& pix_data_sqrt, int sample_width, int sample_height, int samplingTime, int samplingTotalTime, camera cam, hittable* world) {
	for (int y = 0; y < sample_height; ++y) {
#pragma omp parallel for
		for (int x = 0; x < sample_width; ++x) {
			int index = x + y * sample_width;
			vec3 col(0, 0, 0);
			for (int s = 0; s < samplingTime; s++) {
				float u = float(x + random_double()) / float(sample_width);
				float v = float(y + random_double()) / float(sample_height);
				float time = 0;
				// TODO: 時間調整
				ray r = cam.get_ray(u, v,time);
				col += color(r, world, 0);
			}
			const int samples_n = samplingTotalTime / samplingTime;
			pix_data[index][0] = 1.0 * (samples_n - 1) / samples_n * pix_data[index][0] + 1.0 / samples_n / samplingTime * col[0];
			pix_data[index][1] = 1.0 * (samples_n - 1) / samples_n * pix_data[index][1] + 1.0 / samples_n / samplingTime * col[1];
			pix_data[index][2] = 1.0 * (samples_n - 1) / samples_n * pix_data[index][2] + 1.0 / samples_n / samplingTime * col[2];

			pix_data_sqrt[index].r = (unsigned char)(255.99 * sqrt(pix_data[index][0]));
			pix_data_sqrt[index].g = (unsigned char)(255.99 * sqrt(pix_data[index][1]));
			pix_data_sqrt[index].b = (unsigned char)(255.99 * sqrt(pix_data[index][2]));
		}
	}

	double tmp = 0;
	for (int i = 0; i < sample_width * sample_height; i++) {
		tmp += (int)pix_data_sqrt[i].r;
		tmp += (int)pix_data_sqrt[i].g;
		tmp += (int)pix_data_sqrt[i].b;
	}
	std::cout << tmp << std::endl;
}

void renderWithGLFW() {
	std::array<vec3, sample_width * sample_height> pix_data{};
	std::array<RGBA, sample_width * sample_height> pix_data_sqrt{};
	std::array<RGBA, WINDOW_WIDTH * WINDOW_HEIGHT> pix_data_expand{};
	GLFWwindow* glfWindowp = nullptr;
	GLFWwindow** glfwWindowpp = &glfWindowp;
	InitWindow(pix_data_expand, glfwWindowpp);

	std::chrono::system_clock::time_point p = std::chrono::system_clock::now();

	const int ns = 1;
	int samples = 0;
	vec3 lookfrom(278, 278, -800);
	vec3 lookat(278, 278, 0);
	float dist_to_focus = 10;
	float aperture = 0.0;
	const float shutterSpeed = 0.5;

	camera cam(
		lookfrom, lookat, vec3(0, 1, 0), 40, float(WINDOW_WIDTH) / float(WINDOW_HEIGHT), aperture,
		dist_to_focus,shutterSpeed);
	hittable* world = cornell_box_motion();

	while (!glfwWindowShouldClose(*glfwWindowpp)) {
		p = std::chrono::system_clock::now();
		samples += ns;
		std::cout << samples << "samples" << std::endl;

		render(pix_data, pix_data_sqrt, sample_width, sample_height, ns, samples, cam, world);

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
}

void renderWithVideo() {
	std::array<unsigned char, sample_width * sample_height * 3> pix_data_sqrt{};

	hittable* world = cornell_box_motion();

	const int fps = 5, sec = 5;
	const int samplePerFrame = 10;
	vec3 lookfrom(278, 278, -800);
	vec3 lookat(278, 278, 0);
	const float camShutterSpeed = 0.5;//sec
	float dist_to_focus = 10;
	float aperture = 0.0;

	camera cam(
		lookfrom, lookat, vec3(0, 1, 0), 40, float(WINDOW_WIDTH) / float(WINDOW_HEIGHT), aperture,
		dist_to_focus,camShutterSpeed);
	vWriter video("output", fps, sample_width, sample_height);
	for (int i = 0; i < fps * sec; ++i) {
		const float progress = 1.0 * i / (fps * sec);
		const float timer = 1.0 * i / fps;
		std::cout << i << "/" << fps * sec << " " << progress << "%" << std::endl;
		for (int y = 0; y < sample_height; ++y) {
#pragma omp parallel for
			for (int x = 0; x < sample_width; ++x) {
				vec3 col(0, 0, 0);
				for (int s = 0; s < samplePerFrame; s++) {
					float u = float(x + random_double()) / float(sample_width);
					float v = float(y + random_double()) / float(sample_height);
					ray r = cam.get_ray(u, v,timer);
					col += color(r, world, 0) / samplePerFrame;
				}
				int index = y * sample_width + x;
				pix_data_sqrt[index * 3] = (unsigned char)(255.99 * sqrt(col[0]));
				pix_data_sqrt[index * 3 + 1] = (unsigned char)(255.99 * sqrt(col[1]));
				pix_data_sqrt[index * 3 + 2] = (unsigned char)(255.99 * sqrt(col[2]));
			}
		}
		video.Write(&pix_data_sqrt[0]);
	}
}

int main() {
	//renderWithGLFW();
	renderWithVideo();

	return 0;
}
