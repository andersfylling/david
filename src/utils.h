//
// Created by anders on 5/4/17.
//

#ifndef PROJECT_UTILS_H
#define PROJECT_UTILS_H

// universal
#include <iostream>
#include <climits>
#include "engineInformation.h"

#ifdef __linux__
//linux code goes here
#include <unistd.h>

namespace utils {
std::string getAbsoluteProjectPath() {
  char buffer[PATH_MAX];
  std::string path = getcwd(buffer, PATH_MAX) ? std::string(buffer) : std::string("");

  if (path == "") {
    return "";
  }

  // find string until folder project name
  std::string projectPath = path.substr(0, path.find(::engineInformation::PROJECT_NAME)); //hackz

  return projectPath + ::engineInformation::PROJECT_NAME;
}
}


#elif _WIN32
// windows code goes here
#else
// idk
std::cerr << "UNSUPPORTED PLATFORM!!!" << std::endl;
#endif

#endif //PROJECT_UTILS_H
