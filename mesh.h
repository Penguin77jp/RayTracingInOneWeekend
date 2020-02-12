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

  mesh(std::string fn) : fileName(fn) {
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

          if (mode == 1 && tmpVec.size() == 3) {
            vertex.push_back(vec3(tmpVec[0], tmpVec[1], tmpVec[2]));
            tmpVec.clear();
            mode = 0;
          } else if (mode == 2 && tmpVec.size() == 4) {
            polygons.push_back(new polygon(vertex[tmpVec[0] - 1], vertex[tmpVec[1] - 1], vertex[tmpVec[2] - 1], new lambertian(vec3(0.8, 0.8, 0.8))));
            polygons.push_back(new polygon(vertex[tmpVec[2] - 1], vertex[tmpVec[3] - 1], vertex[tmpVec[0] - 1], new lambertian(vec3(0.8, 0.8, 0.8))));
            tmpVec.clear();
            mode = 0;
          }
        }
      }
    }
  }
};
