/*
 * Copyright (c) 2011, Paulo Pereira <ppi@coding-raccoons.org>
 * Copyright (c) 2011, Philippe Boulerne <boolern666@gmail.com>
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
 * AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING,
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
#ifndef _HttpHistory_H_
#define _HttpHistory_H_

#include <string>
#include <vector>

//-----------------------------------------------------------------------------
// http_URL = "http:" "//" host [ ":" port ] [ abs_path [ "?" query ]]
class HttpUrl
{
   std::string path_;
   std::string query_;

   HttpUrl();
   
public:
   HttpUrl( std::string const& rPath)
      : path_  ( rPath)
   {
   }
   HttpUrl( std::string const& rPath, std::string const& rQuery)
      : path_  ( rPath)
      , query_ ( rQuery)
   {
   }
   friend class HttpHistory;
};

//-----------------------------------------------------------------------------
class HttpHistory : public std::vector< HttpUrl>
{
   HttpHistory( HttpHistory const&);
   HttpHistory& operator=( HttpHistory const&);
public:
   HttpHistory() {}
   void push_back( std::string const& rPath)
   {
      std::vector< HttpUrl>::push_back( HttpUrl( rPath));
   }
   void push_back( std::string const& rPath, std::string const& rQuery)
   {
      std::vector< HttpUrl>::push_back( HttpUrl( rPath, rQuery));
   }
   bool IsRefresh( std::string const& rPath, std::string const& rQuery)
   {
      if( size()>0)
      {
         HttpUrl const& rHttpUrl = this->operator[]( size()-1);
         return rPath==rHttpUrl.path_ && rQuery==rHttpUrl.query_;
      }
      return false;
   }
   bool IsBackButton( std::string const& rPath, std::string const& rQuery)
   {
      if( size()>1)
      {
         for( int i=size()-2; i>=0; --i)
         {
            HttpUrl const& rHttpUrl = this->operator[]( i);
            if( rPath==rHttpUrl.path_ && rQuery==rHttpUrl.query_)
               return true;
         }
      }
      return false;
   }
};

#endif

// -*- modeline for vim, do not remove please :-) -*-
// vim: set expandtab tabstop=3 shiftwidth=3 autoindent smartindent:
