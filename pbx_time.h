#ifndef PBX_TIME_H
#define PBX_TIME_H

#include <time.h>
#include <sys/timeb.h>
#include <string>
#include <cstring>
#include <iostream>
#include <iomanip>
#include <ctime>

namespace pbx {

/*
struct timeb
{
  time_t time;
  unsigned short millitm;
  short timezone;
  short dstflag;
};
struct tm
{
  int tm_sec;     // seconds after the minute - [0,59]
  int tm_min;     // minutes after the hour - [0,59]
  int tm_hour;    // hours since midnight - [0,23]
  int tm_mday;    // day of the month - [1,31]
  int tm_mon;     // months since January - [0,11]
  int tm_year;    // years since 1900
  int tm_wday;    // days since Sunday - [0,6]
  int tm_yday;    // days since January 1 - [0,365]
  int tm_isdst;   // daylight savings time flag
};
int main()
{
  std::time_t t = std::time(NULL);
  std::cout << "UTC:   " << std::put_time( std::gmtime(&t), "%c %Z") << '\n'
            << "local: " << std::put_time( std::localtime(&t), "%c %Z") << '\n';
}
output:
UTC:   Wed Dec 28 11:44:28 2011 GMT
local: Wed Dec 28 06:44:28 2011 EST
*/

//------------------------------------------------------------------------------
//#define rdtsc() ({ uint32_t lo, hi; \
//  __asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi)); \
//  (uint64_t)(hi << 32 | lo;})

//------------------------------------------------------------------------------
#ifdef __i386
extern __inline__ uint64_t rdtsc(void) {
    uint64_t x;
      __asm__ volatile ("rdtsc" : "=A" (x));
        return x;
}
#elif defined __amd64
extern __inline__ uint64_t rdtsc(void) {
    uint64_t a, d;
      __asm__ volatile ("rdtsc" : "=a" (a), "=d" (d));
        return (d<<32) | a;
}
#endif

//------------------------------------------------------------------------------
uint64_t now_as_nanosec( )
{
  struct timespec tms;
  clock_gettime( CLOCK_REALTIME, &tms);
  return tms.tv_sec*1'000'000'000ULL + tms.tv_nsec;
}
uint32_t now_as_sec( uint32_t* nanosec_ = nullptr )
{
  struct timespec tms;
  clock_gettime( CLOCK_REALTIME, &tms);
  if( nanosec_ != nullptr )
    *nanosec_ = tms.tv_nsec;
  return tms.tv_sec;

}
uint32_t now_as_int( uint32_t* nanosec_ = nullptr )
{
  return now_as_sec( nanosec_ );
}
static inline uint64_t gettime()
{
  return now_as_nanosec();
}
//------------------------------------------------------------------------------
std::string now()
{
  struct timeb timeb;
  ::ftime( &timeb);
  const struct tm *plocaltime = ::localtime( &(timeb.time));
  struct tm stm;
  std::memcpy( &stm, plocaltime, sizeof(struct tm));

  std::ostringstream oss;
  oss << 1900 + stm.tm_year;
  oss << std::setfill('0') << std::setw(2) << stm.tm_mon + 1;
  oss << std::setfill('0') << std::setw(2) << stm.tm_mday;
  oss << ":";
  oss << std::setfill('0') << std::setw(2) << stm.tm_hour;
  oss << std::setfill('0') << std::setw(2) << stm.tm_min;
  oss << std::setfill('0') << std::setw(2) << stm.tm_sec;
  oss << ".";
  oss << std::setfill('0') << std::setw(3) << timeb.millitm;
  return oss.str();
}

//------------------------------------------------------------------------------
std::string utc_to_local( int year_, int month_, int day_, int hour_, int min_, int sec_ )
{
  struct tm in;
  in.tm_year = year_ - 1900;
  in.tm_mon = month_ - 1;
  in.tm_mday = day_;
  in.tm_hour = hour_;
  in.tm_min = min_;
  in.tm_sec = sec_;
  in.tm_isdst = -1;

  time_t in2 = timegm( &in );

  struct tm *out = localtime( &in2 );

  char buf[100];
  sprintf( buf, "%04d%02d%02d%02d%02d%02d", out->tm_year + 1900,
           out->tm_mon + 1, out->tm_mday, out->tm_hour, out->tm_min,
           out->tm_sec );
  return std::string( buf );
}

//------------------------------------------------------------------------------
std::string local_to_utc( int year_, int month_, int day_, int hour_, int min_, int sec_ )
{
  struct tm in;
  in.tm_year = year_ - 1900;
  in.tm_mon = month_ - 1;
  in.tm_mday = day_;
  in.tm_hour = hour_;
  in.tm_min = min_;
  in.tm_sec = sec_;
  in.tm_isdst = -1;

  time_t in2 = timelocal( &in );

  struct tm *out = gmtime( &in2 );

  char buf[100];
  sprintf( buf, "%04d%02d%02d%02d%02d%02d", out->tm_year + 1900,
           out->tm_mon + 1, out->tm_mday, out->tm_hour, out->tm_min,
           out->tm_sec );
  return std::string( buf );
}


}

/*
 ISO 8601
 http://www.w3.org/TR/NOTE-datetime
 http://en.wikipedia.org/wiki/ISO_8601

Formats

Different standards may need different levels of granularity in the date and time, so this profile defines six levels. Standards that reference this profile should specify one or more of these granularities. If a given standard allows more than one granularity, it should specify the meaning of the dates and times with reduced precision, for example, the result of comparing two dates with different precisions.

The formats are as follows. Exactly the components shown here must be present, with exactly this punctuation. Note that the "T" appears literally in the string, to indicate the beginning of the time element, as specified in ISO 8601.

   Year:
      YYYY (eg 1997)
   Year and month:
      YYYY-MM (eg 1997-07)
   Complete date:
      YYYY-MM-DD (eg 1997-07-16)
   Complete date plus hours and minutes:
      YYYY-MM-DDThh:mmTZD (eg 1997-07-16T19:20+01:00)
   Complete date plus hours, minutes and seconds:
      YYYY-MM-DDThh:mm:ssTZD (eg 1997-07-16T19:20:30+01:00)
   Complete date plus hours, minutes, seconds and a decimal fraction of a
second
      YYYY-MM-DDThh:mm:ss.sTZD (eg 1997-07-16T19:20:30.45+01:00)
where:

     YYYY = four-digit year
     MM   = two-digit month (01=January, etc.)
     DD   = two-digit day of month (01 through 31)
     hh   = two digits of hour (00 through 23) (am/pm NOT allowed)
     mm   = two digits of minute (00 through 59)
     ss   = two digits of second (00 through 59)
     s    = one or more digits representing a decimal fraction of a second
     TZD  = time zone designator (Z or +hh:mm or -hh:mm)
This profile does not specify how many digits may be used to represent the decimal fraction of a second. An adopting standard that permits fractions of a second must specify both the minimum number of digits (a number greater than or equal to one) and the maximum number of digits (the maximum may be stated to be "unlimited").

This profile defines two ways of handling time zone offsets:

Times are expressed in UTC (Coordinated Universal Time), with a special UTC designator ("Z").
Times are expressed in local time, together with a time zone offset in hours and minutes. A time zone offset of "+hh:mm" indicates that the date/time uses a local time zone which is "hh" hours and "mm" minutes ahead of UTC. A time zone offset of "-hh:mm" indicates that the date/time uses a local time zone which is "hh" hours and "mm" minutes behind UTC.
A standard referencing this profile should permit one or both of these ways of handling time zone offsets.
*/

#endif

// vim: set expandtab tabstop=2 shiftwidth=2 autoindent smartindent:

