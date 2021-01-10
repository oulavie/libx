#ifndef PBX_IDXMAP_H
#define PBX_IDXMAP_H

#include <map>
#include <string>
#include <vector>
#include <unordered_map>
#include <ostream>
#include <sstream>

namespace pbx {

//--------------------------------------------------------------------------------------------------
template< typename T >
class idxmap
{
  std::vector< std::pair< T, std::pair<size_t,std::string> > > _data;
  std::vector< size_t > _index;
  std::unordered_map< std::string, size_t > _map;

public:
  idxmap() = default;
  ~idxmap() = default;

  idxmap( const idxmap& ) = delete;
  idxmap& operator=( const idxmap& ) = delete;
  idxmap( idxmap&& ) = delete;
  idxmap& operator=( idxmap&& ) = delete;

  void insert( const T& t_, size_t idx_, const std::string& key_ )
  {
    size_t currpos = _data.size();

    if( _index.size() <= idx_ )
      _index.resize( 2*idx_, -1 );
    _index[ idx_ ] = currpos;

    auto found = _map.find( key_ );
    if( found == _map.end() )
      _map.insert( { key_, currpos } );
    else
      found->second = currpos;

    _data.push_back( { t_, { idx_, key_}} );
  }
  constexpr T* at( size_t idx_ )
  {
    T* rtn = nullptr;
    if( idx_ < _index.size() && _index[ idx_ ] != -1 )
      rtn = &( _data[ _index[ idx_ ] ].first);
    return rtn;
  }
  constexpr T* find( const std::string& key_ )
  {
    T* rtn = nullptr;
    auto found = _map.find( key_ );
    if( found != _map.end() && found->second != -1 )
      rtn = &( _data[ found->second ].first );
    return rtn;
  }
  friend std::ostream& operator<<( std::ostream& os_, const idxmap<T>& t_ )
  {
    for( auto& it : t_._data )
      os_ << "[" << it.second.first << "," << it.second.second << "] " << it.first << std::endl;
    return os_;
  }
  /*
  friend std::ostringstream& operator<<( std::ostringstream& os_, const idxmap<T>& t_ )
  {
    return os_;
  }
  */
};

}

#endif

// vim: set expandtab tabstop=2 shiftwidth=2 autoindent smartindent:

