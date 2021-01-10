#ifndef _HttpHeader_H_
#define _HttpHeader_H_

#include <string>

const char* GetHost();
const char* GetService();
const char* GetHttpHeader( std::string const& rUser);
const char* GetHttpShortHeader( std::string const& rUser);

#endif
