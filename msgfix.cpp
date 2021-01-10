/*
 * Copyright (c) 2015, Philippe Boulerne <philippe.boulerne@gmail.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS
 * AND CONTRIBUTORS AS IS AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING,
 * BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include "msgfix.h"
#include <map>
#include <set>
#include <iostream> // std::cout
#include <fstream> // std::ifstream std::ofstream
#include <sstream> // std::sringstream
#include "boost/tokenizer.hpp"

namespace trd
{

int MsgFix::_COUNT = 0;

//------------------------------------------------------------------------------
tGROUPS get_fix42_repeating_groups()
{
  // LinesOfText group=TextEntry parent=News/Email
  std::set<int> v33{58, 354, 355};

  // NoOrders
  std::set<int> v73{11, 526, 78};

  // NoAllocs
  std::set<int> v78{79, 467};

  // NoRelatedSym
  std::set<int> v146{15,  22,  48,  55,  65,  106, 107, 146, 167, 200, 201,
                     202, 205, 206, 207, 223, 225, 231, 320, 321, 322, 336,
                     348, 349, 350, 351, 460, 461, 541, 873, 874};

  // NoStipulations group=Stipulations parent=TradeCaptureRpt(35=AE)
  std::set<int> v232{233, 234};

  // NoTradingSessions
  std::set<int> v386 = {336}; // ????????????????????????

  // NoPartyIDs group=PartiesEntry
  // parent=TradeCaptureReport(35=AE)/TradeCapturePrtSideEntry
  std::set<int> v453{448, 447, 452, 802}; // NoPartySubIDs

  std::set<int> v454{455, 456};

  // NoSides group=TradeCaptureRptSideEntry parent=TradeCaptureRpt(35=AE)
  std::set<int> v552{54,  37,  1,   11,  12,  13,  15,  58,
                     66,  78,  118, 157, 159, 198, 355, 381,
                     453, // NoPartyIDs
                     526, 528, 581};

  // NoLegs group=TrdInstrmtLegEntry parent=TradeCaptureRpt(35=AE)
  std::set<int> v555{
      600, 601, 602, 603, 607, 608, 609, 610, 611, 612, 613, 614,
      615, 617, 618, 619, 620, 623, 624, 564, 654, 604}; // NoLegSecurityAltID

  // NoLegSecurityAltID group=TrdInstrmtLegEntry parent=TradeCaptureRpt(35=AE)
  std::set<int> v604{605, 606};

  // NoPartySubIDs group=PartySubIDEntry parent=PartiesEntry
  std::set<int> v802{523, 803};

  std::map<int, std::set<int>> m;
  m[33] = v33;
  m[73] = v73;
  m[78] = v78;
  m[146] = v146;
  m[232] = v232;
  m[386] = v386;
  m[453] = v453;
  m[454] = v454;
  m[552] = v552;
  m[555] = v555;
  m[604] = v604;
  m[802] = v802;
  return m;
}

namespace {

//------------------------------------------------------------------------------
std::vector<std::string> tokenize( std::string const &str, const char *sep )
{
  typedef boost::char_separator<char> separators;
  typedef boost::tokenizer<separators> tokenizer;
  const separators seps( sep );
  tokenizer tok( str, seps );
  // for( tokenizer::const_iterator it=tok.begin(); it!=tok.end(); ++it)
  //  rtokens.push_back( *it);
  std::vector<std::string> ltokens;
  for ( auto &it : tok )
    ltokens.push_back( it );
  return ltokens;
}

struct TagVal
{
  int firstAsInt;
  std::string second;
  //std::string first;
};

void extract_tag_val( struct TagVal &rtn, std::string const &rfixtoken )
{
  std::string::size_type sep = rfixtoken.find( '=' );
  std::string first = rfixtoken.substr( 0, sep );
  rtn.second = rfixtoken.substr( sep + 1 );
  // rtn.firstAsInt = atoi( rtn.first.data());
  rtn.firstAsInt = std::stoi( first );
}
/*
bool extract_tag_val( struct TagVal &rtn, std::string const &rfixtoken )
{
  std::string::size_type sep = rfixtoken.find( '=' );
  if ( sep != std::string::npos )
  {
    rtn.first = rfixtoken.substr( 0, sep );
    rtn.second = rfixtoken.substr( sep + 1 );
  }
  else
  {
    rtn.first = rfixtoken;
    rtn.second = std::string( "" );
    rtn.firstAsInt = -2;
    return false;
  }
  try
  {
    rtn.firstAsInt = atoi( rtn.first.data());
    rtn.firstAsInt = std::stoi( rtn.first );
    return true;
  }
  catch ( ... )
  {
    rtn.firstAsInt = -1;
    return false;
  }
}
*/

}

//------------------------------------------------------------------------------
int MsgFix_rmake( MsgFix &rMsgFix, std::vector<std::string> const &rfixtokens,
                  tGROUPS const &groups_, size_t inc = 0,
                  std::set<int> *pRptGroup = NULL )
{
  struct TagVal pair, npair;
  while ( inc < rfixtokens.size() )
  {
    extract_tag_val( pair, rfixtokens[inc] );
    if ( pRptGroup != NULL )
    {
      std::set<int>::iterator found = pRptGroup->find( pair.firstAsInt );
      if ( found == pRptGroup->end() )
      {
        delete pRptGroup;
        return inc;
      }
      else
      {
        pRptGroup->erase( found );
      }
    }
    tGROUPS::const_iterator foundRptTag = groups_.find( pair.firstAsInt );
    if ( foundRptTag != groups_.end() )
    {
      std::set<int> const &rRptTags = foundRptTag->second;
      size_t pos = inc + 1;

      extract_tag_val( npair, rfixtokens[pos] );
      if ( !( pos < rfixtokens.size() &&
              rRptTags.find( npair.firstAsInt ) != rRptTags.end() ) )
      {
        ++inc;
      }
      else
      {
        while ( pos < rfixtokens.size() &&
                rRptTags.find( npair.firstAsInt ) != rRptTags.end() )
        {
          MsgFix *p = new MsgFix( pair.firstAsInt, pair.second, false );
          rMsgFix.push_back( p );

          std::set<int> *copy_of_rptGrp = new std::set<int>( rRptTags );
          // In order to understand recursion, one has first to understand
          // recursion.
          inc = MsgFix_rmake( *p, rfixtokens, groups_, pos, copy_of_rptGrp );
          pos = inc;

          if ( pos < rfixtokens.size() )
          {
            extract_tag_val( npair, rfixtokens[pos] );
          }
        }
      }
    }
    else
    {
      MsgFix *p = new MsgFix( pair.firstAsInt, pair.second );
      rMsgFix.push_back( p );
      ++inc;
    }
  }
  return inc;
}

//------------------------------------------------------------------------------
MsgFix *MsgFix::make( std::string const &rmsg, tGROUPS const &groups_,
                      const char *sep )
{
  MsgFix *p = new MsgFix( 0, "", false );
  std::vector<std::string> ltokens = tokenize( rmsg, sep );
  MsgFix_rmake( *p, ltokens, groups_ );
  return p;
}

//------------------------------------------------------------------------------
void MsgFix_ostream_imp( std::ostream &ostr, MsgFix &rMsgFix,
                         std::string const &roffset )
{
  for ( size_t i( 0 ); i < rMsgFix.size(); ++i )
  {
    MsgFix *p = rMsgFix[i];
    if ( p->is_tag() )
    {
      ostr << roffset << p->inttag() << " = " << p->val() << std::endl;
    }
    else
    {
      ostr << roffset << p->inttag() << std::endl;
      // In order to understand recursion, one has first to understand
      // recursion.
      MsgFix_ostream_imp( ostr, *p, roffset + "--" );
    }
  }
}

std::ostream &operator<<( std::ostream &ostr, MsgFix &rMsgFix )
{
  MsgFix_ostream_imp( ostr, rMsgFix, "" );
  return ostr;
}

//------------------------------------------------------------------------------
void MsgFix_printer_imp( MsgFix &rMsgFix, std::string const &roffset )
{
  for ( size_t i( 0 ); i < rMsgFix.size(); ++i )
  {
    MsgFix *p = rMsgFix[i];
    if ( p->is_tag() )
    {
      std::cout << roffset << p->inttag() << " = " << p->val() << std::endl;
    }
    else
    {
      std::cout << roffset << p->inttag() << std::endl;
      // In order to understand recursion, one has first to understand
      // recursion.
      MsgFix_printer_imp( *p, roffset + "  " );
    }
  }
}

void MsgFix_printer( MsgFix &rMsgFix ) { MsgFix_printer_imp( rMsgFix, "" ); }

//------------------------------------------------------------------------------
void MsgFix_debuger_imp( MsgFix &rMsgFix_, std::ostringstream &ros_,
                         int level_ )
{
  for ( size_t i( 0 ); i < rMsgFix_.size(); ++i )
  {
    MsgFix *p = rMsgFix_[i];
    if ( p->is_tag() )
    {
      ros_ << p->inttag() << "[" << level_ << "]"
           << "=" << p->val() << ";";
    }
    else
    {
      ros_ << p->inttag() << "[" << level_ << "]"
           << ";";
    }
    if ( p->size() > 0 )
      // In order to understand recursion, one has first to understand
      // recursion.
      MsgFix_debuger_imp( *p, ros_, level_ + 1 );
  }
}

void MsgFix_debuger( MsgFix &rMsgFix_, std::ostringstream &ros_ )
{
  MsgFix_debuger_imp( rMsgFix_, ros_, 0 );
}
}

// vim: set expandtab tabstop=2 shiftwidth=2 autoindent smartindent:

