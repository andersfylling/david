#pragma once
#ifdef DAVID_NOT_SUPPORTED

#include <string>
#include <functional>
#include <iostream>

namespace utils {
namespace logger {
namespace type {
const uint8_t DEBUG = 1;
const uint8_t TEST = 2;
const uint8_t BENCHMARK = 3;
const uint8_t DEVELOPMENT = 4;
}

inline void save(const std::string& info) {
  
}

inline const std::string format(const std::string& event, const std::string& info) {
  return "[" + event + "] >> " + info;
}

inline const std::string logTypeTitle(const int t) {
#ifdef DAVID_FLAG_DEVELOPMENT
  std::string env = "";
  if (t == ::utils::logger::type::DEBUG) {
    env = "DEBUG";
  }
  else if(t == ::utils::logger::type::TEST) {
    env += (env == "" ? "TEST" : "+TEST");
  }
  else if (t == ::utils::logger::type::BENCHMARK) {
    env += (env == "" ? "BENCHMARK" : "+BENCHMARK");
  }
  else if (t == ::utils::logger::type::DEVELOPMENT) {
    env += (env == "" ? "DEVELOPMENT" : "+DEVELOPMENT");
  }
  return env;
#else
  return "";
#endif
}

inline const std::string logTypeTitle() {
#ifdef DAVID_FLAG_DEVELOPMENT
  std::vector<std::string> envs;

#ifdef DAVID_DEBUG
  envs.push_back("DEBUG");
#endif

#ifdef DAVID_TEST
  envs.push_back("TEST");
#endif

#ifdef DAVID_BENCHMARK
  envs.push_back("BENCHMARK");
#endif

#ifdef DAVID_DEVELOPMENT
  envs.push_back("DEVELOPMENT");
#endif

  std::string env = envs.at(0);
  for (int i = 1; i < envs.size(); i++) {
    env += "+";
    env += envs.at(i);
  }

  return env;

#else
  return "";
#endif
}

inline void print(const std::string& event, const std::string& info) {
  std::cout << format(event, info) << std::endl;
}

inline void print(const std::string& info) {
  std::cout << info << std::endl;
}

inline bool validateRequest(const int t) {
#ifdef DAVID_DEBUG
  return t == ::utils::logger::type::DEBUG;
#elif DAVID_TEST
  return t == ::utils::logger::type::TEST;
#elif DAVID_BENCHMARK
  return t == ::utils::logger::type::BENCHMARK;
#elif DAVID_DEVELOPMENT
  return t == ::utils::logger::type::DEVELOPMENT;
#else
  return false;
#endif
}

inline void log(const int t, const std::string info) {
#ifdef DAVID_FLAG_DEVELOPMENT
  if (validateRequest(t)) {
    save(format(logTypeTitle(t), info));
    print(logTypeTitle(t), info);
  }
#endif
}

inline void log(const std::string info) {
#ifdef DAVID_FLAG_DEVELOPMENT
    save(format(logTypeTitle(), info));
    print(logTypeTitle(), info);
#endif
}

inline void logdebug(const std::string info){
#ifdef DAVID_DEBUG
  save(format("DEBUG", info));
  print("DEBUG", info);
#endif
}
inline void logdebug(const std::function<void()>& action) {
#ifdef DAVID_DEBUG
  action();
#endif
}
inline void logdebug(const std::function<const std::string()>& actionInfo) {
#ifdef DAVID_DEBUG
  logdebug(actionInfo());
#endif
}


inline void logtest(const std::string info){
#ifdef DAVID_TEST
  save(format("TEST", info));
  print("TEST", info);
#endif
}
inline void logtest(const std::function<void()>& action) {
#ifdef DAVID_TEST
  action();
#endif
}
inline void logtest(const std::function<const std::string()>& actionInfo) {
#ifdef DAVID_TEST
  logtest(actionInfo());
#endif
}

inline void logbenchmark(const std::string info){
#ifdef DAVID_BENCHMARK
  save(format(type::BENCHMARK, info));
  print(type::BENCHMARK, info);
#endif
}
inline void logbenchmark(const std::function<void()>& action) {
#ifdef DAVID_BENCHMARK
  action();
#endif
}
inline void logbenchmark(const std::function<const std::string()>& actionInfo) {
#ifdef DAVID_BENCHMARK
  logbenchmark(actionInfo());
#endif
}


} // logger
} // utils

#endif