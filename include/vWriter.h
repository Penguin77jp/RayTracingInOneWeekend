#pragma once
#include "opencv2/opencv.hpp"


class vWriter {
private:
	int width, height, fps;

public:
	cv::VideoWriter writer;
	vWriter(std::string fileName, int fps, int width, int height) {
		cv::VideoWriter writer(fileName + ".mp4", cv::VideoWriter::fourcc('X', '2', '6', '4'), fps, cv::Size(width, height));
		this->writer = writer;
		this->width = width;
		this->height = height;
		this->fps = fps;
	}

	void Write(unsigned char color[]) {
		cv::Mat frame(cv::Size(width, height), CV_8UC3);
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				int index = y * width * 3 + x * 3;
				frame.data[index] = (int)(color[index + 2]);
				frame.data[index+ 1] = (int)(color[index + 1]);
				frame.data[index + 2] = (int)(color[index]);
			}
		}
		writer << frame;
	}
};
