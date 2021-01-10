#ifndef PBX_H
#define PBX_H

#include "pbx_macro.h"
//#include "color.h"
#include "pbx_dirent.h"
//#include "pbx_string.h"
//#include "pbx_execute.h"
#include "pbx_csv.h"
//#include "pbx_time.h"
//#include "pbx_vector.h"
//#include "pbx_file.h"
//#include "pbx_hex.h"

#endif

// git clone https://github.com/poudlar/libpbx
// git remote add origin https://github.com/poudlar/libpbx.git

//  git status
//  git diff pbx.h
//  git add pbx.h
//  git commit -m "first commit"
//  git push -u origin master

static inline uint64_t gettime()
{
  struct timespec t;
  clock_gettime(CLOCK_REALTIME, &t);
  return (t.tv_sec * 1'000'000'000) + t.tv_nsec;
}

// vim: set expandtab tabstop=2 shiftwidth=2 autoindent smartindent:

