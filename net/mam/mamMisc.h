
#pragma once
#ifndef mam_misc_h
#define mam_misc_h

#include <sstream>
#include <string>

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define AT " (" __FILE__ ":" TOSTRING(__LINE__) ")"

//#if 0
#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)
//#endif

namespace mam
{

//--------------------------------------------------------------------------------------------------
#define rdtsc()                                                                                                        \
  ({                                                                                                                   \
    uint32_t lo, hi;                                                                                                   \
    __asm__ __volatile__("rdtsc" : "=a"(lo), "=d"(hi));                                                                \
  (uint64_t)(hi << 32 | lo;                                                                                            \
  })

uint32_t now_as_sec(uint32_t *nanosec_)
{
  struct timespec tms;
  clock_gettime(CLOCK_REALTIME, &tms);
  *nanosec_ = tms.tv_nsec;
  return tms.tv_sec;
}

uint64_t now_as_nsec()
{
  struct timespec t;
  clock_gettime(CLOCK_REALTIME, &t);
  return t.tv_sec * 1'000'000'000ULL + t.tv_nsec;
}

std::string date()
{
  struct timespec t;
  clock_gettime(CLOCK_REALTIME, &t);
  char now[256];
  // strftime( now, 256, "%Y%m%d%H%M%S", gmtime( &t.tv_sec ) );
  strftime(now, 256, "%Y%m%d:%H%M%S", localtime(&t.tv_sec));
  std::ostringstream oss;
  oss << now;
  oss << ".";
  oss << t.tv_nsec;
  return oss.str();
}

//--------------------------------------------------------------------------------------------------
template <class Vector, class InputIterator> void append(Vector &vector_, InputIterator first_, InputIterator last_)
{
  vector_.reserve(vector_.size() + last_ - first_ + 1);
  vector_.insert(vector_.end(), first_, last_);
}

template <class Vector> void pop_front(Vector &vector_)
{
  vector_.erase(vector_.begin());
}

template <class Vector> void pop_front(Vector &vector_, int n_)
{
  vector_.erase(vector_.begin(), vector_.begin() + n_);
}

/*
https://stackoverflow.com/questions/4713131/removing-item-from-vector-while-iterating/13102374

int myInts[] = {1, 7, 8, 4, 5, 10, 15, 22, 50. 29};
std::vector v(myInts, myInts + sizeof(myInts) / sizeof(int));
v.erase(std::remove_if(v.begin(), v.end(), [](int i) { return i < 10; }), v.end());

vector< string >::iterator it = curFiles.begin();
while(it != curFiles.end())
{
  if(aConditionIsMet)
    it = curFiles.erase(it);
  else
    ++it;
}

*/

} // namespace mam

#endif

// vim: set expandtab tabstop=2 shiftwidth=2 autoindent smartindent:
