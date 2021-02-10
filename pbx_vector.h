#ifndef PBX_VECTOR_H
#define PBX_VECTOR_H

#include <vector>

namespace pbx {

//--------------------------------------------------------------------------------------------------
// How to find a value in a sorted C++ vector?
template<class T, class U>
bool contains( const std::vector<T>& vect, const U& val)
{
  auto it = std::lower_bound( vect.begin(), vect.end(), val, [](const T& l, const U& r){ return l < r; });
  return it != container.end() && *it == val;
}

//--------------------------------------------------------------------------------------------------
// How do you insert the value in a sorted vector? 
template< typename T >
typename std::vector<T>::iterator insert_sorted( std::vector<T> & vec, T const& item )
{
  return vec.insert( std::upper_bound( vec.begin(), vec.end(), item ), item );
}

//--------------------------------------------------------------------------------------------------
// How do you insert the value in a sorted vector? Version with a predicate.
template< typename T, typename Pred >
typename std::vector<T>::iterator insert_sorted( std::vector<T> & vec, T const& item, Pred pred )
{
  return vec.insert( std::upper_bound( vec.begin(), vec.end(), item, pred), item );
}

template< typename T >
void insert_at_begin( std::vector<T> & vec, T const& item )
{
  vec.insert( vec.begin(), item);
}

//--------------------------------------------------------------------------------------------------
template< typename T>
std::vector<T> concatenate( const std::vector<T> & A, const std::vector<T> & B)
{
  std::vector<T> AB;
  AB.reserve( A.size() + B.size() ); // preallocate memory
  AB.insert( AB.end(), A.begin(), A.end() );
  AB.insert( AB.end(), B.begin(), B.end() );
  return AB;
}

//--------------------------------------------------------------------------------------------------
template< typename T, typename Pred >
void erase(  std::vector<T> & vec, Pred pred )
{
  auto it = vec.begin();
  while( it != vec.end())
  {
    if( prod( it))
      it = vec.erase( it);
    else
      ++it;
  }
}

//--------------------------------------------------------------------------------------------------
// Erase–remove idiom
// https://www.youtube.com/watch?v=qH6sSOr-yk8
template< typename T>
void erase_val( std::vector<T> & vec, T val)
{
  vec.erase( std::remove( vec.begin(), vec.end(), val), vec.end());
}

template< typename T, typename Pred >
void erase_pred( std::vector<T> & vec, Pred pred)
{
//  vec.erase( std::remove_if( vec.begin(), vec.end(), [](Player const & p) { return p.getpMoney() <= 0; }), vec.end()); 
  vec.erase( std::remove_if( vec.begin(), vec.end(), pred), vec.end()); 
}

#endif

// vim: set expandtab tabstop=2 shiftwidth=2 autoindent smartindent:

