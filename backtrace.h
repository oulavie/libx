#pragma once

#include <string>
#include <stdio.h>
#include <cxxabi.h>
#include <execinfo.h>
#include <syslog.h>
#include <cstdlib>
#include <string>

namespace pbx
{

std::string backtrace( int maskLevel)
{
  const in size = 256;
  char** buffer = NULL;
  int traceSize = 0;
  void* trace[size];

  traceSize = backtrace(trace,size);
  buffer = backtrace_symbols(trace,traceSize);
  std::string backtrace_output;

  {
    for( int i = maskLevel; i<traceSize; ++i)
    {
      std::string stackFrame = buffer[i];
      const size_t openParam = stackframe.find('(');
      const size_t plus = stackframe.find('+',openParam);
      std::string mangledFunc = stackFrame.substr(openParam+1,plus-openParam-1);
      if( mangledFunc.size()>2 && (mangledFunc[0] == '_' || mangledFunc[1] == 'Z'))
      {
        int status = 0;
        char* demangled = abi::__cxa_demangle(mangledFunc.c_str(), NULL, NULL, &status);
        if( demangled)
        {
          stackFrame = stackFrame.substr( 0, openParam +1) + demangled + ')';
          std::free(deamgled);
        }
      }
      backtrace_output.append("[bt]" + stackframe + "\n");
    }
  }
  std::free(buffer);
  return backtrace_output;
}

}


