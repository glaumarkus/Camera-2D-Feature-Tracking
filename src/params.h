#ifndef PARAMS_H
#define PARAMS_H

#include <string>
#include <iostream>
#include "Eigen/Core"
#include <fstream>

struct PipelineParams {

  int kernel_size;
  float sigma_x;
  int block_size;
  int aperture_size;
  int min_reponse;
  double harris_k;



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
      } else if (paramName == "block_size:") {
        check1 >> block_size;
      } else if (paramName == "aperture_size:") {
        check1 >> aperture_size;
      } else if (paramName == "min_reponse:") {
        check1 >> min_reponse;
      } else if (paramName == "harris_k:") {
        check1 >> harris_k;
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
