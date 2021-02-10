#ifndef PBX_CSV_H
#define PBX_CSV_H

#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <memory>
#include <algorithm>

/*****************************************************************************************
 *  *** With an external function 'convert' ***
  struct struct_t
  {
    std::string _first, _second;
  };
  void convert( pbx::Tokens& tokens_, struct_t& data_ )
  {
    data_._first  = tokens.at(0);
    data_._second = tokens.at(1);
  }
  std::vector< struct_t> get( const char* filename_ )
  {
    std::vector<struct_t> rtn;
    auto str = std::ifstream( filename_, std::ios::in);
    auto tokens = pbx::parse_csv_header( str);
    auto push_struct_t = [&]( auto m)
    {
      rtn.push_back( m);
    };
    pbx::parse_csv_file< struct_t>( push_struct_t, str);
    return rtn;
  }
*/

/*****************************************************************************************
 *  *** Without the external function 'convert' ***
    std::vector<ipscsv_t> rtn;
    auto str = std::ifstream( csv_file_, std::ios::in );
    auto tokens = pbx::parse_csv_header( str );
    auto push_ipscsv_t = [&]( pbx::Tokens& tokens_ )
    {
      ipscsv_t data
      {
        ._date = tokens_.at(0),
        ._ip   = tokens_.at(1),
      };
      rtn.push_back( data );
    };
    pbx::parse_csv_file( push_ipscsv_t, str );
*/
namespace pbx {

using Tokens = std::vector<std::string>;
constexpr std::size_t line_size = 1 << 14;
using LineArray = std::array< char, line_size>;

template< typename IT>
std::pair<IT,IT> extract_token( IT& first, IT last, char delim )
{
  char search_for = (*first=='\'' || *first=='\"') ? *first ++ : delim;
  auto it = std::find( first, last, search_for );
  std::pair< IT, IT> res{ first, it };
  for( first = it; (first != last) && (*first != delim); ++first ) {}
  return res;
}

template< typename back_inserter>
inline uint32_t tokenize_csv( std::string const& line_, back_inserter it, char delim=',' )
{
  if( line_.begin() == line_.end() )
    return 0;
  uint32_t rc = 0;
  auto l = line_.begin();
  for( ; ; )
  {
    auto p = extract_token( l, line_.cend(), delim );
    *it++ = std::string{ &*(p.first), static_cast<std::size_t>( std::distance( p.first, p.second ))};
    ++rc;
    if( l != line_.cend() )
      l++;
    else
      break;
  }
  return rc;
}

Tokens parse_csv_header( std::istream& istr_ )
{
  LineArray line_array;
  if( !istr_.getline( &line_array[0], line_array.size() ) )
  {
    std::cerr << "could not read input stream" << AT << std::endl;
    return {};
  }
  Tokens tokens;
  std::string line( &line_array[0] );
  if( !tokenize_csv( line, std::back_inserter(tokens) ) )
  {
    std::cerr << "could not tokenize input stream" << AT << std::endl;
    return {};
  }
  return tokens;
}

// *** With an external function 'convert' ***
template< typename MsgType, typename F >
void parse_csv_file( F f, std::istream& istr_ )
{
  Tokens tokens = {};
  MsgType msg_data = {};
  for( LineArray line_array; istr_.getline( &line_array[0], line_array.size() ); )
  {
    std::string line( &line_array[0] );
    if( !tokenize_csv( line, std::back_inserter(tokens) ) )
    {
      std::cerr << "could not tokenize input stream" << AT << std::endl;
      return;
    }
    convert( tokens, msg_data);
    f( msg_data);
    tokens.clear();
  }
}

// *** Without the external function 'convert' ***
template< typename T >
void parse_csv_file( T t, std::istream& istr_, char delim_ )
{
  Tokens tokens = {};
  for( LineArray line_array; istr_.getline( &line_array[0], line_array.size() ); )
  {
    std::string line( &line_array[0] );
    if( !tokenize_csv( line, std::back_inserter(tokens), delim_ ) )
    {
      std::cerr << "could not tokenize input stream" << AT << std::endl;
      return;
    }
    t( tokens, line );
    tokens.clear();
  }
}

}

#endif

// vim: set expandtab tabstop=2 shiftwidth=2 autoindent smartindent:

