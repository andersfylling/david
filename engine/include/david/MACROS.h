#pragma once

// check if any of the development flags are used
#if defined(DAVID_DEBUG) || defined(DAVID_TEST) || defined(DAVID_BENCHMARK) || defined(DAVID_DEVELOPMENT)
#define DAVID_FLAG_DEVELOPMENT
#endif

// get a hold of what cpu type is used
// no idea how to actually do this
#if defined(__INTEL__)
#define DAVID_CPU_INTEL
#endif

//
// Supported compilers
// * GCC 5.1 and higher
//
#if defined(__GNUC__) && __GNUC__ > 5
#define DAVID_SUPPORTED_COMPILER
#endif

// if the compiler is not officially supported, create an error
// This can be removed, but isn't recommended. Just remove the comments below:
//#define I_DONT_CARE_LET_ME_THROUGH
#if !defined(DAVID_SUPPORTED_COMPILER) && !defined(I_DONT_CARE_LET_ME_THROUGH)
#error Compiler type or version is not supported.
#endif