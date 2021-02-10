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
#ifndef _HttpHandies_H_
#define _HttpHandies_H_

#include "libx/xStaticBuffer.h"
#include "HttpCmds.h"
#include "HttpNameValues.h"
#include <stdio.h>
#include <string>

class MComVectorMessageBase;

void extract_document( std::string       &rHttpRequest,
                       std::string       &rHttpPath,
                       std::string       &rHttpQuery,
                       std::string       &rUserIP,
                       std::string       &rSeqNum,
                       HttpCmds          &rHttpCmds,
                       HttpNameValues    &rNameValues,
                       std::string const& strHttp);

void push_back( MComVectorMessageBase *pCom, const char* pMsg, size_t lSizeLeft, size_t chunck);

std::string UriEncode( std::string const& rSrc);
std::string UriDecode( std::string const& rSrc);

//-----------------------------------------------------------------------------
template< typename TWriter>
class GetHtmlRaw
{
   TWriter &mTWriter;
   long      mRow;

   GetHtmlRaw();
   GetHtmlRaw( GetHtmlRaw const&);
   GetHtmlRaw operator=( GetHtmlRaw const&);
public:
   GetHtmlRaw( TWriter &rHtmlWriter)
      : mTWriter ( rHtmlWriter)
      , mRow     ( 0)
   {
   }

   template< typename T>
   void operator()( T &rT)
   {
      if( mRow%2==0)
         mTWriter.append( "<tr style='background-color:#f0f0f0'>");
      else
         mTWriter.append( "<tr style='background-color:#f5f5f5'>");

      mTWriter.add( "<td>%s</td>", T::GetAttribut());

      mTWriter.append( "<td>");
      std::string lHtml;
      T::GetHtml( lHtml, T::GetAttribut(), rT.GetStringValue());
      mTWriter.append( lHtml);
      mTWriter.append( "</td>");

      mTWriter.append( "</tr>");
      ++mRow;
   }
};

#endif

// -*- modeline for vim, do not remove please :-) -*-
// vim: set expandtab tabstop=3 shiftwidth=3 autoindent smartindent:
