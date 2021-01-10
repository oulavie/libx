#ifndef PBX_MAP_H
#define PBX_MAP_H

// alternative :
// https://en.wikipedia.org/wiki/Interval_tree

#include <map>

namespace pbx {

//--------------------------------------------------------------------------------------------------
template< typename T >
class range
{
  T min_, max_;
public:
  typedef T value_type;

  range( T const & center ) : min_( center ), max_( center ) {}
  range( T const & min, T const & max ) : min_( min ), max_( max ) {}
  T min() const { return min_; }
  T max() const { return max_; }
};

//--------------------------------------------------------------------------------------------------
// Detection of outside of range to the left (smaller values):
// a range lhs is left (smaller) of another range if both lhs.min() and lhs.max() 
// are smaller than rhs.min().
template <typename T>
struct left_of_range : public std::binary_function< range<T>, range<T>, bool >
{
  bool operator()( range<T> const & lhs, range<T> const & rhs ) const
  {
    return lhs.min() < rhs.min() && lhs.max() <= rhs.min(); // quand on peut avoir le max=min du range suivant
    //return lhs.max() < rhs.min();
  }
};

}

#endif

// vim: set expandtab tabstop=2 shiftwidth=2 autoindent smartindent:

