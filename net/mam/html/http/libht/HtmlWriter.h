/*
 * copyright (c) 2011, Paulo Pereira <ppi@coding-raccoons.org>
 * copyright (c) 2011, Philippe Boulerne <boolern666@gmail.com>
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
#ifndef _HtmlWriter_H_
#define _HtmlWriter_H_

#include <string>

/*
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN"
   "http://www.w3.org/TR/html4/strict.dtd">
<html>
   <head>
      <title>My first HTML document</title>
   </head>
   <body>
      <P>Hello world!
   </body>
</html>
*/

class xBuffer;

// -----------------------------------------------------------------------------
class HtmlWriter
{
   std::string   doctype_;         // <html [html_attributes_]><head [head_attributes_]>
   std::string   html_attributes_;
   std::string   head_attributes_;
   xBuffer      *head_;            // </head><body [body_attributes_]>
   std::string   body_attributes_;
   xBuffer      *body_;            // </body>

   xBuffer      *buffer_;
   bool          ready_;

   HtmlWriter( HtmlWriter const&);
   HtmlWriter& operator=( HtmlWriter const&);

public:
   HtmlWriter( xBuffer *head, xBuffer *body, xBuffer *buffer)
      : head_   (head)
      , body_   (body)
      , buffer_ (buffer)
      , ready_  (false)
   {
   }
   ~HtmlWriter() {}

   void process( bool header = true);
   const char* data();
   size_t length();

   size_t body_size();
   void* body_ptr();
   void set_body_length( size_t len);

   std::string& doctype()         { ready_=false; return doctype_; }
   std::string& html_attributes() { ready_=false; return html_attributes_; }
   std::string& head_attributes() { ready_=false; return head_attributes_; }
   xBuffer&     head()            { ready_=false; return *head_; }
   std::string& body_attributes() { ready_=false; return body_attributes_; }
   xBuffer&     body()            { ready_=false; return *body_; }

   HtmlWriter& reset();
   HtmlWriter& operator=( const char* data);
   HtmlWriter& operator+=( const char* data);
   HtmlWriter& operator+=( std::string const& text);
   HtmlWriter& append( const char* data);
   HtmlWriter& append( std::string const& text);
   HtmlWriter& add( const char* pBuffer, ...);
};

#endif

// -*- modeline for vim, do not remove please :-) -*-
// vim: set expandtab tabstop=3 shiftwidth=3 autoindent smartindent:
