
#include "tvp.h"
#include <iostream>
#include <stdio.h>

void test()
{
  std::cout << "----------------------------------------------------------" << std::endl;
  /*
  t to;
  size_t t8 = t::mapTag2Index(8);
  std::cout << "8=" << t::mapTag2Index(8) << std::endl;
  std::cout << "9=" << t::mapTag2Index(9) << std::endl;
  std::cout << "35=" << t::mapTag2Index(35) << std::endl;
  std::cout << "48=" << t::mapTag2Index(48) << std::endl;
  std::cout << "22=" << t::mapTag2Index(22) << std::endl;
  std::cout << "11=" << t::mapTag2Index(11) << std::endl;
  std::cout << "41=" << t::mapTag2Index(41) << std::endl;
  std::cout << "1=" << t::mapTag2Index(1) << std::endl;
  */

  std::string s{"8=toto;9=tata;41=tyty;22=gfgf;35=BN;11=tag11;48=porduit;10=100;"};
  const char *p = s.data();
  size_t l = s.length();
  pbx::tvp_writer pw;

  uint64_t t00 = gettime();
  typedef pbx::intclipper<8, 9, 35, 48, 22, 11, 41, 1> ic;
  pbx::tvp_reader<ic> mc(';');
  mc.set(p, l);

  uint64_t t0 = gettime();
  const auto &tag8 = mc.getPair<8>();

  uint64_t t1 = gettime();
  const boost::string_ref &tag9 = mc.getPair<9>();
  const auto &tag41 = mc.getPair<41>();
  const auto &tag22 = mc.getPair<22>();
  const auto &tag35 = mc.getPair<35>();
  const auto &tag11 = mc.getPair<11>();
  const auto &tag48 = mc.getPair<48>();
  const auto &tag10 = mc.getPair<10>();
  uint64_t t2 = gettime();
  std::string s1;
  pw.write(';', "fix4.2", {{"9", tag9}, {"41", tag41}}, [&](auto &s) { s1 = s.str(); });
  uint64_t t3 = gettime();

  std::cout << "ctor             =" << (t0 - t00) << std::endl;
  std::cout << "1er getpair parse=" << (t1 - t0) << std::endl;
  std::cout << "getpair          =" << (t2 - t1) / 7. << std::endl;
  std::cout << "all reader only  =" << (t2 - t00) << std::endl;
  std::cout << "all avec writer  =" << (t3 - t00) << std::endl;

  {
    std::string tag{tag8.data(), tag8.size()};
    std::cout << tag.data() << std::endl;
  }
  {
    std::string tag{tag9.data(), tag9.size()};
    std::cout << tag.data() << std::endl;
  }
  {
    std::string tag{tag41.data(), tag41.size()};
    std::cout << tag.data() << std::endl;
  }
  {
    std::string tag{tag22.data(), tag22.size()};
    std::cout << tag.data() << std::endl;
  }
  {
    std::string tag{tag35.data(), tag35.size()};
    std::cout << tag.data() << std::endl;
  }
  {
    std::string tag{tag11.data(), tag11.size()};
    std::cout << tag.data() << std::endl;
  }
  {
    std::string tag{tag48.data(), tag48.size()};
    std::cout << tag.data() << std::endl;
  }
  {
    std::string tag{tag10.data(), tag10.size()};
    std::cout << tag.data() << std::endl;
  }
  std::cout << s1 << std::endl;
}

//--------------------------------------------------------------------------------------------------
int main()
{
  test();
  test();
  return 0;
}

/*
1983  mkdir musl
1984  cd musl/
1985  git clone git://git.musl-libc.org/musl
1986  ./configure
1987  ls -lrt
1988  cd musl/
1989  ls
1990  ./configure
1991  make install
1992  sudo make install
1993  which musl-gcc
1994  ls -lrt /usr/local/musl/bin
/usr/local/musl/bin/musl-gcc -static
*/

// clang-format off
// clang-format-9 -i tvp_test.cpp
// g++ -Wall -W -pedantic -fno-default-inline -Weffc++ -std=c++2a tvp_test.cpp
// g++ -std=c++17 -Wall -Wextra -Wpedantic -O3 tvp_test.cpp -L/usr/lib/x86_64-linux-gnu -lssl -lcrypto
// g++ -std=c++17 -Wall -Wextra -Wpedantic -Wno-deprecated-declarations tvp_test.cpp -L/usr/lib/x86_64-linux-gnu -lssl -lcrypto

// vim: set expandtab tabstop=2 shiftwidth=2 autoindent smartindent:
