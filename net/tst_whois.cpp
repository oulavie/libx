
#include "pbx_whois.h"

#include <iostream>

int main()
{
  pbx::whois_t whois;
  pbx::execute_whois( "34.237.73.95", whois);
  std::cout << whois << std::endl;
  return 0;
}

// g++ -Wall -fno-default-inline -Weffc++ -std=c++17 -I../libpbx tst_whois.cpp
// g++ -Wall -fno-default-inline -Weffc++ -std=c++17 tst_whois.cpp

