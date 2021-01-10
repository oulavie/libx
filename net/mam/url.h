
// https://stackoverflow.com/questions/154536/encode-decode-urls-in-c

#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

namespace
{

char from_hex(char ch)
{
  return isdigit(ch) ? ch - '0' : tolower(ch) - 'a' + 10;
}

} // namespace

namespace pbx
{
std::string url_encode(const std::string &value)
{
  std::ostringstream escaped;
  escaped.fill('0');
  escaped << std::hex;

  for (std::string::const_iterator i = value.begin(), n = value.end(); i != n; ++i)
  {
    std::string::value_type c = (*i);

    // Keep alphanumeric and other accepted characters intact
    if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~')
    {
      escaped << c;
      continue;
    }

    // Any other characters are percent-encoded
    escaped << std::uppercase;
    escaped << '%' << std::setw(2) << int((unsigned char)c);
    escaped << std::nouppercase;
  }

  return escaped.str();
}

std::string url_decode(const char* const begin_, const char* const end_)
{
  std::ostringstream escaped;
  escaped.fill('0');

  const char* current = beging_;

  for ( ; current != end_; ++current )
  {
    switch ( *current )
    {
    case '%': {
      if (*(current+1) && *(current+2))
      {
        char h = from_hex(*(current+1)) << 4 | from_hex(*(current+2));
        escaped << h;
        current += 2;
      }
    }
    break;
    case '+': {
      escaped << ' ';
    }
    break;
    default: {
      escaped << *current;
    }
    break;
    }
  }
  return escaped.str();
}


std::string url_decode(const std::string &text)
{
  char h;
  std::ostringstream escaped;
  escaped.fill('0');

  for (auto i = text.begin(), n = text.end(); i != n; ++i)
  {
    std::string::value_type c = (*i);

    switch (c)
    {
    case '%': {
      if (i[1] && i[2])
      {
        h = from_hex(i[1]) << 4 | from_hex(i[2]);
        escaped << h;
        i += 2;
      }
    }
    break;
    case '+': {
      escaped << ' ';
    }
    break;
    default: {
      escaped << c;
    }
    break;
    }
  }
  return escaped.str();
}


} // namespace pbx
  /*
    int main(int argc, char **argv)
    {
      std::string msg = "J%C3%B8rn!";
      std::cout << msg << std::endl;
      std::string decodemsg = pbx::url_decode(msg);
      std::cout << decodemsg << std::endl;

      return 0;
    }
  */
  // clang-format off
// clang-format-9 -i url.cpp
// g++ -Wall -W -pedantic -fno-default-inline -Weffc++ -std=c++2a url.cpp
// g++ -std=c++17 -Wall -Wextra -Wpedantic -O3 url.cpp -L/usr/lib/x86_64-linux-gnu -lssl -lcrypto
// g++ -std=c++17 -Wall -Wextra -Wpedantic -Wno-deprecated-declarations url.cpp -L/usr/lib/x86_64-linux-gnu -lssl -lcrypto

// vim: set expandtab tabstop=2 shiftwidth=2 autoindent smartindent:

