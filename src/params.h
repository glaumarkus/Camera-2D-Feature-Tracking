#ifndef PARAMS_H
#define PARAMS_H

#include <string>
#include <iostream>
#include "Eigen/Core"
#include <fstream>

struct PipelineParams {

  int kernel_size;
  float sigma_x;



  bool fromFile(const std::string &filePath) {
    std::ifstream file_(filePath);

    if (!file_.is_open()) {
      std::cerr << "Params file not found!" << std::endl;
      return false;
    }

    std::string line_;
    int i = 0;
    while (getline(file_, line_)) {
      if (line_[0] == '#') continue;
      if (line_.empty()) continue;

      std::stringstream check1(line_);
      std::string paramName;

      check1 >> paramName;
      if (paramName == "kernel_size:"){
        check1 >> kernel_size;
      } else if (paramName == "sigma_x:") {
        check1 >> sigma_x;

      } else {
        std::cerr << "Unrecognized pipeline parameter: " << paramName << std::endl;
        assert(0);
      }
    }
    file_.close();
    return true;
  }

};

#endif