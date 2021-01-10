

#include <deque>
#include <algorithm>
#include <cstdint>
#include <time.h>
#include <ostream>

namespace pbx {

uint64_t now_as_int()
{
  struct timespec t;
  clock_gettime( CLOCK_REALTIME, &t );
  return t.tv_sec * 1'000'000'000ULL + t.tv_nsec;
}

std::string date()
{
  struct timespec t;
  clock_gettime( CLOCK_REALTIME, &t );
  char now[256];
  //strftime( now, 256, "%Y%m%d%H%M%S", gmtime( &t.tv_sec ) );
  strftime( now, 256, "%Y%m%d%H%M%S", localtime( &t.tv_sec ) );
  return std::string(now);
}

template< typename T >
class deque_t
{
  struct eT_t
  {
    T _t;
    int _count = 0;
    constexpr bool operator==( const eT_t& t_ ) const
    {
      return _t == t_._t;
    }
  };
  std::deque< eT_t > _deque;

public:
  deque_t() {}
  void add( const T& t_ )
  {
    typename std::deque< eT_t >::iterator found = std::find_if( _deque.begin(), _deque.end(), [&]( const eT_t& val_ ) { return val_._t == t_; } );
    if( found == _deque.end() )
    {
      _deque.push_back( { ._t = t_, ._count = 1, } );
    }
    else
    {
      found->_t = t_;
      ++found->_count;
    }
  }
  eT_t& front() const
  {
    return _deque.front();
  }
  void pop_front()
  {
    return _deque.pop_front();
  }
  friend std::ostream& operator<<( std::ostream& os_, const deque_t& t_ )
  {
    for( auto& it : t_._deque )
      os_ << it._count << " : " << it._t << std::endl;
    return os_;
  }
private:
  deque_t( const deque_t& ) = delete;
  deque_t( deque_t&& ) = delete;
  deque_t& operator=( const deque_t& ) = delete;
  deque_t& operator=( deque_t&& ) = delete;
};

}

#include <string>
#include <iostream>

struct toto_t
{
  std::string _key;
  int _int;

  friend std::ostream& operator<<( std::ostream& os_, const toto_t& t_ )
  {
    return os_ << "_key=" << t_._key << ", _int=" << t_._int << ", ";
  }
  toto_t& operator=( const toto_t& t_ )
  {
    _key = t_._key;
    _int = t_._int;
    return *this;
  }
  bool operator==( const toto_t& t_ ) const
  {
    return _key == t_._key;
  }
};

//--------------------------------------------------------------------------------------------------
void test1()
{
  std::cout << "---------------------------------------------" << std::endl;
  std::cout << __FUNCTION__ << std::endl;
  std::cout << "---------------------------------------------" << std::endl;

  pbx::deque_t< toto_t> m;
  m.add( { ._key = "toto", ._int = 1 } );
  std::cout << m << std::endl;
  m.add( { ._key = "toto", ._int = 2 } );
  std::cout << m << std::endl;
  m.add( { ._key = "titi", ._int = 1 } );
  std::cout << m << std::endl;
  m.add( { ._key = "tata", ._int = 1 } );
  std::cout << m << std::endl;
  m.add( { ._key = "toto", ._int = 3 } );
  std::cout << m << std::endl;
  m.add( { ._key = "tata", ._int = 2 } );
  std::cout << m << std::endl;

  m.pop_front();
  std::cout << m << std::endl;
  
  m.add( { ._key = "toto", ._int = 1 } );
  std::cout << m << std::endl;
  m.add( { ._key = "toto", ._int = 2 } );
  std::cout << m << std::endl;
  m.add( { ._key = "titi", ._int = 2 } );
  std::cout << m << std::endl;

  m.pop_front();
  std::cout << m << std::endl;
  
}

//--------------------------------------------------------------------------------------------------
int main()
{
  std::cout << pbx::date() << std::endl;
  test1();
  return 0;
}


