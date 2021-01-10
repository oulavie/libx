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
#include "HtmlWriter.h"
#include "libx/xBuffer.h"
#include <stdarg.h>
#include <stdio.h>

/*
HTML5 Document:
The following tags have been introduced for better structure:
- section: This tag represents a generic document or application section. It can be used together with h1-h6 to indicate the document structure.
- article: This tag represents an independent piece of content of a document, such as a blog entry or newspaper article.
- aside: This tag represents a piece of content that is only slightly related to the rest of the page.
- header: This tag represents the header of a section.
- footer: This tag represents a footer for a section and can contain information about the author, copyright information, et cetera.
- nav: This tag represents a section of the document intended for navigation.
- dialog: This tag can be used to mark up a conversation.
- figure: This tag can be used to associate a caption together with some embedded content, such as a graphic or video.

<!DOCTYPE html> 
<html> 
   <head>
      <meta charset="utf-8" /> 
      <title>...</title> 
   </head> 
   <body> 
      <header>...</header> 
      <nav>...</nav>
      <article>
      <section> ... </section>
      </article>
      <aside>...</aside>
      <footer>...</footer>
      <h2>HTML5 SVG Circle</h2> 
      <svg id="svgelem" height="200" xmlns="http://www.w3.org/2000/svg"> 
         <circle id="redcircle" cx="50" cy="50" r="50" fill="red" /> 
         <rect id="redrect" width="300" height="100" fill="red" />
         <line x1="0" y1="0" x2="200" y2="100" style="stroke:red;stroke-width:2"/>
         <ellipse cx="100" cy="50" rx="100" ry="50" fill="red" />
         <polyline points="0,0 0,20 20,20 20,40 40,40 40,60" fill="red" />
         <defs>
            <radialGradient id="gradient" cx="50%" cy="50%" r="50%" fx="50%" fy="50%">
            <stop offset="0%" style="stop-color:rgb(200,200,200); stop-opacity:0"/>
            <stop offset="100%" style="stop-color:rgb(0,0,255); stop-opacity:1"/>
            </radialGradient>
         </defs>
         <ellipse cx="100" cy="50" rx="100" ry="50" style="fill:url(#gradient)" />
      </svg>
   </body>
</html> 

http://validator.w3.org/

--- HTML 4.01 Transitional ---
<!DOCTYPE html PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN"
   "http://www.w3.org/TR/html4/loose.dtd">
<html>
   <head>
   ...
   </head>
   <body>
   ...
   </body>
</html>


--- XHTML 1.0 Transitional ---
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"
   "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">

<html xmlns="http://www.w3.org/1999/xhtml"

<meta http-equiv=\"Content-type\" content=\"text/html;charset=UTF-8\" 

*/

// -----------------------------------------------------------------------------
HtmlWriter& HtmlWriter::reset()
{
   doctype_         = "";
   html_attributes_ = "";
   head_attributes_ = "";
   head_->erase();
   body_attributes_ = "";
   body_->erase();
   ready_ = false; 
   return *this;
}

// -----------------------------------------------------------------------------
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

void HtmlWriter::process( bool header)
{
   if( ready_)
      return;
   buffer_->erase();

   // --- doctype ---
   if( header)
      buffer_->append( doctype_.data(), doctype_.length());

   // --- html ---
   if( html_attributes_.empty())
   {
      *buffer_ += "<html>";
   }
   else
   {
      *buffer_ += "<html ";
      buffer_->append( html_attributes_.data(), html_attributes_.length());
      *buffer_ += ">";
   }

   // --- head ---
   if( head_attributes_.empty())
   {
      *buffer_ += "<head>";
   }
   else
   {
      *buffer_ += "<head ";
      buffer_->append( head_attributes_.data(), head_attributes_.length());
      *buffer_ += ">";
   }
   buffer_->append( head_->data(), head_->length());
   *buffer_ += "</head>";

   // --- body ---
   if( body_attributes_.empty())
   {
      *buffer_ += "<body>";
   }
   else
   {
      *buffer_ += "<body ";
      buffer_->append( body_attributes_.data(), body_attributes_.length());
      *buffer_ += ">";
   }
   buffer_->append( body_->data(), body_->length());
   *buffer_ += "</body>";

   // --- /html ---
   *buffer_ += "</html>";

   ready_ = true;
}

// -----------------------------------------------------------------------------
const char* HtmlWriter::data()
{
   return buffer_->data();
}

// -----------------------------------------------------------------------------
size_t HtmlWriter::length()
{
   return buffer_->length();
}

// -----------------------------------------------------------------------------
HtmlWriter& HtmlWriter::operator=( const char* data)
{
   ready_ = false; 
   body_->operator=( data);
   return *this;
}

// -----------------------------------------------------------------------------
HtmlWriter& HtmlWriter::operator+=( const char* data)
{
   ready_ = false; 
   body_->operator+=( data);
   return *this;
}

// -----------------------------------------------------------------------------
HtmlWriter& HtmlWriter::operator+=( std::string const& text)
{
   ready_ = false; 
   body_->operator+=( text);
   return *this;
}

// -----------------------------------------------------------------------------
HtmlWriter& HtmlWriter::append( const char* data)
{
   ready_ = false; 
   body_->append( data);
   return *this;
}

// -----------------------------------------------------------------------------
HtmlWriter& HtmlWriter::append( std::string const& text)
{
   ready_ = false; 
   body_->append( text);
   return *this;
}

// -----------------------------------------------------------------------------
HtmlWriter& HtmlWriter::add( const char* pBuffer, ...)
{
   ready_ = false; 
   if( pBuffer == NULL)
      return *this;
   static char sBuffer[ 1024*1024];
   va_list argptr;
   va_start( argptr, pBuffer);
#if defined(WIN32) && _MSC_VER < 1300
   size_t length = _vsnprintf( sBuffer, sizeof(sBuffer)-1, pBuffer, argptr);
#else
   size_t length = vsnprintf( sBuffer, sizeof(sBuffer)-1, pBuffer, argptr);
#endif
   va_end( argptr);
   sBuffer[ length] = '\0';
   body_->append( sBuffer, length);
   return *this;
}

// -----------------------------------------------------------------------------
void* HtmlWriter::body_ptr()
{
   ready_ = true; 
   return buffer_->data_ptr();
}

// -----------------------------------------------------------------------------
size_t HtmlWriter::body_size()
{
   ready_ = true; 
   return buffer_->size();
}

// -----------------------------------------------------------------------------
void HtmlWriter::set_body_length( size_t len)
{
   ready_ = true; 
   buffer_->set_length( len);
}

// -*- modeline for vim, do not remove please :-) -*-
// vim: set expandtab tabstop=3 shiftwidth=3 autoindent smartindent:
