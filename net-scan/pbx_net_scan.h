#ifndef PBX_NET_SCAN_H
#define PBX_NET_SCAN_H

#include "../pbx_execute.h"
#include "../pbx_string.h"
#include <iostream>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

namespace pbx
{

//--------------------------------------------------------------------------------------------------
struct scan_t
{
  std::string _ip = {};

  /*
    friend std::ostream &operator<<(std::ostream &os_, const whois_t &w_)
    {
      return os_ << w_._NetRangeLow << ", " << w_._NetRangeHigh << ", " << w_._OrgName << ", " << w_._Country;
    }
    friend std::ostringstream &operator<<(std::ostringstream &os_, const whois_t &w_)
    {
      os_ << w_._NetRangeLow << ", " << w_._NetRangeHigh << ", " << w_._OrgName << ", " << w_._Country;
      return os_;
    }
    */
};

//--------------------------------------------------------------------------------------------------
std::vector<std::string> regex_search(const std::string &subject_, const std::string &regex_)
{
  std::vector<std::string> res;
  std::string result;
  try
  {
    std::regex re(regex_);
    std::smatch match;
    if (std::regex_search(subject_, match, re))
    {
      for (size_t i(0); i < match.size(); ++i)
      {
        res.push_back(match.str(i));
      }
    }
  }
  catch (std::regex_error &e)
  {
  }
  return res;
}

// std::string subject = "NetRange:       99.85.128.0 - 99.87.191.255";
std::pair<std::string, std::string> get_net_range(const std::string &subject_)
{
  std::pair<std::string, std::string> res;
  const std::string regstr("((?:\\d+\\.){3}\\d+)(.*?)((?:\\d+\\.){3}\\d+)");
  auto rtn = pbx::regex_search(subject_, regstr);
  if (rtn.size() == 4)
  {
    // To be sure you have an IP address, use :
    const std::string regstr2(
        "(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)");
    auto rtn1 = pbx::regex_search(rtn[1], regstr2);
    auto rtn2 = pbx::regex_search(rtn[3], regstr2);
    if (rtn1.size() == 1 and rtn2.size() == 1)
    {
      res.first = rtn1[0];
      res.second = rtn2[0];
    }
  }
  return res;
}

std::tuple<bool, std::string> get_ip_address(const std::string &str_)
{
  // To be sure you have an IP address, use :
  const std::string ipRegex(
      "(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)");
  auto rtn = pbx::regex_search(str_, ipRegex);
  // return (rtn.size() > 0) ? {true, rtn[0]} : {false, std::string()};
  if (rtn.size() > 0)
    return {true, rtn[0]};
  else
    return {false, std::string()};
}
//"(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}
//    (?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)");
std::tuple<bool, std::string> get_mac_address(const std::string &str_)
{
  // "^([0-9A-F]{2}[:-]){5}([0-9A-F]{2})$"; // validate
  const std::string macRegex = "([0-9A-Fa-f]{2}:){5}[0-9A-Fa-f]{2}"; //  extract
  // const std::string hexPair = "[0-9A-Fa-f]{2}";
  // const std::string macRegex = "{hexPair}:{hexPair}:{hexPair}:{hexPair}:{hexPair}:{hexPair}";
  auto rtn = pbx::regex_search(str_, macRegex);
  if (rtn.size() > 0)
    return {true, rtn[0]};
  else
    return {false, std::string()};
}

std::tuple<bool, std::string> get_first_word(const std::string &str_)
{
  const std::string firstwordRegex = "^([\\w\\-]+)";
  auto rtn = pbx::regex_search(str_, firstwordRegex);
  if (rtn.size() > 0)
    return {true, rtn[0]};
  else
    return {false, std::string()};
}

//--------------------------------------------------------------------------------------------------
std::vector<std::string> execute_command(const std::string &cmd_)
{
  std::vector<std::string> out;
  pbx::execute(cmd_, [&](const std::string &s_) { out.push_back(s_); });
  return out;
}

} // namespace pbx

#endif

// vim: set expandtab tabstop=2 shiftwidth=2 autoindent smartindent:
