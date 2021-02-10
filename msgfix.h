/*
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
#ifndef MSGFIX_H
#define MSGFIX_H

#include <map>
#include <set>
#include <vector>
#include <string>
#include <sstream>

// support non int tag [N]
// support un-specified tags in fix repeating group [N]
// support of extended repeating groups start/counter [N]

namespace trd
{

typedef std::map<int, std::set<int>> tGROUPS;
tGROUPS get_fix42_repeating_groups();
static tGROUPS FIX42_GROUPS = get_fix42_repeating_groups();

//------------------------------------------------------------------------------
class MsgFix : public std::vector<MsgFix *>
{
  bool _istag;
  int _inttag;
  std::string _val;

  typedef std::vector<MsgFix *> parent_type;

  MsgFix();

public:
  static int _COUNT;
  MsgFix( int inttag, std::string const &val, bool istag = true )
      : _istag( istag ), _inttag( inttag ), _val( val )
  {
    ++_COUNT;
  }

  ~MsgFix()
  {
    --_COUNT;
    for ( size_t i( 0 ); i < this->size(); ++i )
    {
      delete this->operator[]( i );
      this->operator[]( i ) = NULL;
    }
  }

  int inttag() const { return _inttag; }
  int &set_inttag() { return _inttag; }

  std::string const &val() const { return _val; }
  std::string &set_val() { return _val; }

  void set_istag() { _istag = true; }
  void set_isgrp() { _istag = false; }

  bool is_tag() const { return _istag; }
  bool is_grp() const { return !_istag; }

  friend std::ostream &operator<<( std::ostream &ostr, MsgFix &rMsgFix );
  static MsgFix *make( std::string const &rmsg,
                       tGROUPS const &groups = FIX42_GROUPS,
                       const char *sep = ";" );
};

//------------------------------------------------------------------------------
void MsgFix_printer( MsgFix & );
void MsgFix_debuger( MsgFix &, std::ostringstream & );
}

#endif

// -*- modeline for vim, do not remove please :-) -*-
// vim: set expandtab tabstop=2 shiftwidth=2 autoindent smartindent:

