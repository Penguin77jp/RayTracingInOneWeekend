#pragma once
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <sstream>

#include "polygon.h"

class mesh {
public:
	std::string fileName;
	std::vector<vec3> vertex;
	std::vector <polygon*> polygons;
	float size;

	mesh(std::string fn, float p_size) : fileName(fn), size(p_size){
		std::ifstream fs(fileName);
		int mode = 0;
		//mode 0 none
		//1 v mode
		//2 f mode
		std::vector<double> tmpVec;
		std::string tmpLine;
		while (std::getline(fs, tmpLine)) {
			std::stringstream ss(tmpLine);
			std::string tmp;
			while (std::getline(ss, tmp, ' ')) {
				if (tmp == "v") {
					mode = 1;
				} else if (tmp == "f") {
					mode = 2;
				} else {
					const char* valChar = tmp.c_str();
					float val = std::stod(valChar);
					tmpVec.push_back(val);
				}
			}
			if (mode == 1) {
				vertex.push_back(vec3(tmpVec[0], tmpVec[1], tmpVec[2]));
				tmpVec.clear();
				mode = 0;
			} else if (mode == 2) {
				std::vector<vec3> posi;
				if (tmpVec.size() == 3) {
					posi.push_back(vertex[tmpVec[0] - 1]);
					posi.push_back(vertex[tmpVec[1] - 1]);
					posi.push_back(vertex[tmpVec[2] - 1]);
				} else if (tmpVec.size() == 4) {
					posi.push_back(vertex[tmpVec[0] - 1]);
					posi.push_back(vertex[tmpVec[1] - 1]);
					posi.push_back(vertex[tmpVec[2] - 1]);
					posi.push_back(vertex[tmpVec[2] - 1]);
					posi.push_back(vertex[tmpVec[3] - 1]);
					posi.push_back(vertex[tmpVec[0] - 1]);
				}
				posi.push_back(vertex[tmpVec[0] - 1]);
				posi.push_back(vertex[tmpVec[1] - 1]);
				posi.push_back(vertex[tmpVec[2] - 1]);
				for (int i = 2; i+1 < tmpVec.size(); ++i) {
					posi.push_back(vertex[tmpVec[i] - 1]);
					posi.push_back(vertex[tmpVec[i+1] - 1]);
					posi.push_back(vertex[tmpVec[0] - 1]);
				}
				//posi
				for (int i = 0; i < posi.size(); ++i) {
					posi[i] *= size;
				}
				for (int i = 0; i < posi.size() / 3; ++i) {
					polygons.push_back(new polygon(posi[i*3], posi[i*3+1], posi[i*3+2], new lambertian(vec3(0.5, 0.5, 0.8))));
				}
				tmpVec.clear();
				mode = 0;
			}
		}
	}
};
