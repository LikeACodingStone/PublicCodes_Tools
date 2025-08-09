/***************************************************************************
 *  HMI-common.h
 *
 *	Health Gene Technologies, All Rights Reserved
 *
 *  Created on  : Aug. 27, 2021 09:21:21 AM
 *  Author      : 
 *
 *  Version Info: 1.0
 *
 ***************************************************************************/
#pragma once

#include <vector>
#include <list>
#include <map>
#include <string>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <iterator>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <time.h>
#include <assert.h>
#include <memory.h>

#ifndef __cplusplus
 #ifndef TRUE
  #define TRUE  1
  #define FALSE 0
 #endif

 #ifndef YES
  #define YES 1
  #define NO  0
 #endif

 #ifndef ON
  #define ON  1
  #define OFF 0
 #endif

#else

//Below #defines for TRUE & FALSE needs to be commented to avoid compilation warnings 
//of the NextGen applications (in RELEASE build) as the ACE library uses these
//defines to a different values (TRUE with 1 & FALSE with 0) and redefining
//these entries in the application are leading to compilation inconsistencies 
//But these defines are not commented as these defines are used extensively in the nextgen applications!!

 #ifndef TRUE
  #define TRUE  true
  #define FALSE false
 #endif

 #ifndef YES
  #define YES true
  #define NO  false
 #endif

 #ifndef ON
  #define ON  true
  #define OFF false
 #endif

#endif

#ifndef NULL
 #define NULL 0L
#endif

#ifndef CLEAR
 #define CLEAR 0
#endif

#ifndef CLEAR_ALL
 #define CLEAR_ALL 0
#endif

/* C++ Already defines bool.  For that matter, so does C99.  When we get a */
/* new compiler, this will have to be revisited. */
#ifndef __cplusplus
 typedef short bool;
#endif

typedef char cbool;

#define FOREVER   for(;;)

#define ABS(x)    ( ((x) >= 0) ? (x) : -(x) )
#define HGT_MIN(a,b)  ( ((a) <= (b)) ? (a) : (b) )

// I'm guessing I could take this out and there wouldn't be any compile errors.
// But that's just a guess.  Shouldn't the system headers define this somewhere?
// 
#ifdef __gnu_linux__
#include <sys/param.h>
#else
#define MAX(a,b)  ( ((a) >= (b)) ? (a) : (b) )
#endif

// Changed to avoid accidental use of NUM_ELEMENTS with class types 
// that support operator[].  As array[0], it can be misused by passing
// a vector<> for array (this would divide the sizeof the vector class 
// by the sizeof one element -- definitely wrong!).  Note that this
// still incorrectly allows array to be a pointer.
#define NUM_ELEMENTS(array)   (sizeof(array)/sizeof(0[array]))

// This has been redefined to evaluate to -1 or 1, as some C++
// compilers *do* allow an array size of zero, but do not allow
// an array size of -1.
//
#define compile_assert( test )  extern char  compile_assert_fail[(int)((test) != 0)*2 - 1]


// define primary data types
typedef signed   char  schar;
typedef unsigned char  uchar;
typedef unsigned short ushort;
typedef unsigned long  ulong;
typedef unsigned int   uint;
typedef long           ltime_t;

#if defined(_MSC_VER) // Microsoft compiler
  typedef __int64 int64;
  typedef unsigned __int64 uint64;
#else
  #include <stdint.h>
  typedef int64_t  int64;
  typedef uint64_t uint64;
#endif

