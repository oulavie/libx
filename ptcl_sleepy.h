#ifndef PTCL_SLEEPY_H
#define PTCL_SLEEPY_H

#include <string>
#include <string.h>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>

namespace sleepy 
{

struct atom
{
  const char *_tag;
  uint16_t _taglen;
  const char *_val;
  uint16_t _vallen;

  atom( const char *tag_, uint16_t taglen_, const char *val_=nullptr, uint16_t vallen_=0 ) : _tag(tag_), _taglen(taglen_), _val(val_),
  _vallen(vallen_) {}

  friend std::ostream& operator<<( std::ostream& out_, const atom& t_ )
  {
    out_ << std::string( t_._tag, t_._taglen) << "=";
    if( t_._val != nullptr && t_._vallen!=0 )
      out_ << std::string( t_._val, t_._vallen);
    return out_;
  }
};

std::string tokens_to_string( const std::vector< atom >& tokens_ )
{
  std::ostringstream oss;
  for( auto& it : tokens_ )
    oss << it << ";";
  return oss.str();
}

// sr 1 ,ty 1 ,id status ,e but ,dstt 12.34 ,; 
template< typename OnTag>
int tokenize_message( const char* data_, int length_, OnTag&& onTag_ )
{
  int tagPos = 0;
  int valPos = 0;
  for( int i(0); i<length_; ++i )
  {
    if( data_[i] == ' ' )
    {
      valPos = ++i;
      if( data_[i] != ',' )
      {
        for( ; i < length_; ++i )
        {
          if( data_[i] == ',' )
          {
            if( data_[i-1] == ' ' )
            {
              onTag_( &data_[tagPos], valPos - tagPos - 1, &data_[valPos], i - valPos - 1 );
            }
            else
            {
              onTag_( &data_[tagPos], valPos - tagPos - 1, &data_[valPos], i - valPos );
            }
            tagPos = i + 1;
            if( data_[ tagPos ] == ';' )
            {
              return tagPos + 1;
            }
            break;
          }
        }
      }
      else
      {
         onTag_( &data_[tagPos], valPos - tagPos - 1, nullptr, 0 );
         tagPos = i + 1;
         if( data_[ tagPos ] == ';' ) 
         {
           return tagPos + 1;
         }
      }
    }
  }
  return length_;
}

template< typename OnMessage>
int parse_messages( const char* data_, int length_, OnMessage&& onMessage_ )
{
  int offset( 0 );
  while( length_ > 0 )
  {
    std::vector< atom > tokens;
    int rtn = tokenize_message( data_ + offset, length_,
      [&]( const char* tag_, uint16_t taglen_, const char *val_, uint16_t vallen_ ) -> void 
      {
        tokens.push_back( atom( tag_, taglen_, val_, vallen_ ) );
      } );
    onMessage_( tokens, data_ + offset, rtn );
    offset += rtn;
    length_ -= rtn;
  }
  return length_;
}

}

//--------------------------------------------------------------------------------------------------

namespace sleepy2 
{

template< typename IT >
std::pair< IT, IT > extract_token( IT& first, IT last, char delim=',')
{
  auto it = std::find( first, last, delim );
  std::pair< IT, IT > res{ first, it };
  for( first = it; (first != last) && (*first != delim); ++first )
  {}
  return res;
}

template< typename back_inserter >
inline int tokenize_message( const std::string& line_, back_inserter it, char delim_=',' )
{
  if( line_.begin() == line_.end() )
    return 0;
  int rc = 0;
  auto l = line_.begin();
  for( ;; )
  {
    auto p = extract_token( l, line_.cend(), delim_ );
    *it++ = std::string{ &*(p.first), static_cast<std::size_t>( std::distance( p.first, p.second )) };
    ++rc;
    if( l != line_.end() )
      ++l;
    else
      break;
  }
  return rc;
}

template< typename OnMessage>
int parse_messages( std::istringstream& istr_, OnMessage onMessage_ )
{
  std::vector< std::string > tokens;
  //for( LineArray line_array; istr_.getline( &line_array[0], line_array.size() ); )
  {
    //std::string line( &line_array[0] );
    std::string line( istr_.str() );
    if( !tokenize_message( line, std::back_inserter(tokens) ) )
    {
      //std::cerr << "ERROR could not tokenize input stream" << std::endl;
      return 0;
    }
    onMessage_( tokens );
    tokens.clear();
  }
  return 0;
}

}

#endif

// vim: set expandtab tabstop=2 shiftwidth=2 autoindent smartindent:

