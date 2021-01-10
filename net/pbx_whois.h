#ifndef PBX_WHOIS_H
#define PBX_WHOIS_H

#include "../pbx_string.h"
#include "../pbx_execute.h"
#include <vector>
#include <string>
#include <regex>
#include <ostream>
#include <sstream>

namespace pbx {

//--------------------------------------------------------------------------------------------------
struct whois_t
{
  std::string _NetRangeLow, _NetRangeHigh;
  std::string _OrgName;
  std::string _Country;
  std::string _Customer;

  whois_t() : _NetRangeLow{}, _NetRangeHigh{}, _OrgName{}, _Country{}, _Customer{} {}

  friend std::ostream& operator<<( std::ostream& os_, const whois_t& w_ )
  {
    return os_ << w_._NetRangeLow << ", " << w_._NetRangeHigh << ", " << w_._OrgName << ", " << w_._Country;
  }
  friend std::ostringstream& operator<<( std::ostringstream& os_, const whois_t& w_ )
  {
    os_ << w_._NetRangeLow << ", " << w_._NetRangeHigh << ", " << w_._OrgName << ", " << w_._Country;
    return os_;
  }
};
 
//--------------------------------------------------------------------------------------------------
std::vector< std::string > regex_search( const std::string& subject_, const std::string& regex_ )
{
  std::vector< std::string > res;
  std::string result;
  try 
  {
    std::regex re( regex_ );
    std::smatch match;
    if( std::regex_search( subject_, match, re ) ) 
    {
      for( size_t i(0); i<match.size(); ++i )
      {
        res.push_back( match.str(i) );
      }
    }
  } 
  catch( std::regex_error& e ) 
  {
  }
  return res;
}

//std::string subject = "NetRange:       99.85.128.0 - 99.87.191.255";
std::pair< std::string, std::string > get_net_range( const std::string& subject_ )
{
  std::pair< std::string, std::string > res;
  std::string regstr( "((?:\\d+\\.){3}\\d+)(.*?)((?:\\d+\\.){3}\\d+)" );
  auto rtn = pbx::regex_search( subject_, regstr );
  if( rtn.size() == 4)
  {
    // To be sure you have an IP address, use :
    std::string regstr2( "(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)");
    auto rtn1 = pbx::regex_search( rtn[1], regstr2 );
    auto rtn2 = pbx::regex_search( rtn[3], regstr2 );
    if( rtn1.size() == 1 and rtn2.size() == 1 )
    {
      res.first = rtn1[0];
      res.second = rtn2[0];
    }
  }
  return res;
}

/*
NetRange:       35.208.0.0 - 35.247.255.255
CIDR:           35.224.0.0/12, 35.208.0.0/12, 35.240.0.0/13
NetName:        GOOGLE-CLOUD                                  
NetHandle:      NET-35-208-0-0-1
Parent:         NET35 (NET-35-0-0-0-0)
NetType:        Direct Allocation
OriginAS:                                                     
Organization:   Google LLC (GOOGL-2)                                                                                           
OrgName:        Google LLC
Country:        US
*/
//--------------------------------------------------------------------------------------------------
void execute_whois( const std::string& ip_address_, whois_t& whois_ )
{
  std::string cmd( "whois ");
  cmd += ip_address_;
  pbx::execute( cmd, [&]( const std::string& s_ )
    {
      if( s_.find( "NetRange:" ) != std::string::npos ||
          s_.find( "inetnum:" ) != std::string::npos )
      {
        auto range = get_net_range( s_ );
        whois_._NetRangeLow = range.first;
        whois_._NetRangeHigh = range.second;
      }
      else if( s_.find( "OrgName:" ) != std::string::npos ||
               s_.find( "NetName:" ) != std::string::npos ||
               s_.find( "netname:" ) != std::string::npos )
      {
        auto s = s_.substr( 8 );
        whois_._OrgName = pbx::trim( s );
      }
      else if( s_.find( "Country:" ) != std::string::npos ||
               s_.find( "country:" ) != std::string::npos )
      {
        auto s = s_.substr( 8 );
        whois_._Country = pbx::trim( s );
      }
      else if( s_.find( "Customer:" ) != std::string::npos )
      {
        auto s = s_.substr( 9 );
        whois_._Customer = pbx::trim( s );
      }

    });
}

}

#endif

// vim: set expandtab tabstop=2 shiftwidth=2 autoindent smartindent:

