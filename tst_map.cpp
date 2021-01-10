
#include "pbx_map.h"
#include <iostream>

//--------------------------------------------------------------------------------------------------
void test1()
{
  std::cout << "---------------------------------------------" << std::endl;
  std::cout << __FUNCTION__ << std::endl;
  std::cout << "---------------------------------------------" << std::endl;

  typedef std::map< pbx::range<int>, std::string, pbx::left_of_range<int> > map_type;

  map_type integer; // integer part of a decimal number:

  integer[ pbx::range<int>( 0, 1 ) ] = "0-1";
  integer[ pbx::range<int>( 1, 3 ) ] = "1-3";
  integer[ pbx::range<int>( 3, 5 ) ] = "3-5";
  integer[ pbx::range<int>( 5, 7 ) ] = "5-7";
  integer[ pbx::range<int>( 7, 10 ) ] = "7-10";
  integer[ pbx::range<int>( 11, 15 ) ] = "11-15";

  std::cout << "0 " << integer[ pbx::range<int>( 0 ) ] << std::endl;
  std::cout << "1 " << integer[ pbx::range<int>( 1 ) ] << std::endl;
  std::cout << "---------------------------------------------" << std::endl;
  std::cout << "0 " << integer[ 0 ] << std::endl;
  std::cout << "1 " << integer[ 1 ] << std::endl;
  std::cout << "2 " << integer[ 2 ] << std::endl;
  std::cout << "3 " << integer[ 3 ] << std::endl;
  std::cout << "4 " << integer[ 4 ] << std::endl;
  std::cout << "5 " << integer[ 5 ] << std::endl;
  std::cout << "6 " << integer[ 6 ] << std::endl;
  std::cout << "7 " << integer[ 7 ] << std::endl;
  std::cout << "8 " << integer[ 8 ] << std::endl;
  std::cout << "9 " << integer[ 9 ] << std::endl;
  std::cout << "10 " << integer[ 10 ] << std::endl;
  std::cout << "11 " << integer[ 11 ] << std::endl;
  std::cout << "12 " << integer[ 12 ] << std::endl;
  std::cout << "13 " << integer[ 13 ] << std::endl;
  std::cout << "14 " << integer[ 14 ] << std::endl;
  std::cout << "15 " << integer[ 15 ] << std::endl;
  std::cout << "16 " << integer[ 16 ] << std::endl;
}

//--------------------------------------------------------------------------------------------------
void test2()
{
  std::cout << "---------------------------------------------" << std::endl;
  std::cout << __FUNCTION__ << std::endl;
  std::cout << "---------------------------------------------" << std::endl;

  typedef std::map< pbx::range<double>, std::string, pbx::left_of_range<double> > map_type;

  map_type integer; // integer part of a decimal number:

  integer[ pbx::range<double>( 0.0, 1.0 ) ] = "zero";
  integer[ pbx::range<double>( 1.0, 2.0 ) ] = "one";
  integer[ pbx::range<double>( 2.0, 3.0 ) ] = "two";

  std::cout << integer[ pbx::range<double>( 0.5 ) ] << std::endl; // zero
  std::cout << integer[ pbx::range<double>( 1.0 ) ] << std::endl; // one
  std::cout << integer[ pbx::range<double>( 1.5 ) ] << std::endl; // one
  std::cout << integer[ pbx::range<double>( 2.0 ) ] << std::endl; // one
  std::cout << integer[ pbx::range<double>( 2.5 ) ] << std::endl; // one
  std::cout << "---------------------------------------------" << std::endl;
  std::cout << integer[ 0.5 ] << std::endl; // implicit conversion kicks in
  std::cout << integer[ 1.0 ] << std::endl; // implicit conversion kicks in
  std::cout << integer[ 1.5 ] << std::endl; // implicit conversion kicks in
  std::cout << integer[ 2.0 ] << std::endl; // implicit conversion kicks in
  std::cout << integer[ 2.5 ] << std::endl; // implicit conversion kicks in
}

//--------------------------------------------------------------------------------------------------
int main()
{
  test1();
  test2();
  return 0;
}

// vim: set expandtab tabstop=2 shiftwidth=2 autoindent smartindent:

