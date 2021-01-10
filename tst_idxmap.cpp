
#include "pbx_idxmap.h"
#include <iostream>

struct toto_t
{
  int _int;

  friend std::ostream& operator<<( std::ostream& os_, const toto_t& t_ )
  {
    return os_ << "_int=" << t_._int << ", ";
  }
};

//--------------------------------------------------------------------------------------------------
void test1()
{
  std::cout << "---------------------------------------------" << std::endl;
  std::cout << __FUNCTION__ << std::endl;
  std::cout << "---------------------------------------------" << std::endl;

  pbx::idxmap< toto_t> m; 

  toto_t t1 { ._int = 1, };
  m.insert( t1, 1, "first" );
  m.insert( { ._int = 2, }, 2, "second" );
  m.insert( { ._int = 3, }, 3, "third" );
  m.insert( { ._int = 4, }, 4, "quatre" );
  m.insert( { ._int = 5, }, 5, "cinq" );
  m.insert( { ._int = 6, }, 6, "six" );
  m.insert( { ._int = 7, }, 7, "sept" );
  m.insert( { ._int = 8, }, 8, "huit" );
  m.insert( { ._int = 9, }, 9, "neuf" );

  std::cout << m << std::endl;
  
  std::cout << "---------------------------------------------" << std::endl;

  toto_t* p = m.at( 5 );
  if( p != nullptr )
    std::cout << *p << std::endl;

  p = m.find( "sept" );
  if( p != nullptr )
    std::cout << *p << std::endl;
}

//--------------------------------------------------------------------------------------------------
int main()
{
  test1();
  return 0;
}




