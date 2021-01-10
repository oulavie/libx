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
#include "HttpHandies.h"

#include "GatewayLib/Vector/MComVectorMessageBase.h"
// http://www.w3.org/Protocols/rfc2616/rfc2616-sec6.html
// http://www.faqs.org/rfcs/rfc2616.html
// http://tools.ietf.org/html/rfc2616
// http://web-sniffer.net/rfc/rfc2616.html

/*
5 Request
5.1.1 Method
"OPTIONS"                ; Section 9.2
"GET"                    ; Section 9.3
"HEAD"                   ; Section 9.4
"POST"                   ; Section 9.5
"PUT"                    ; Section 9.6
"DELETE"                 ; Section 9.7
"TRACE"                  ; Section 9.8
"CONNECT"                ; Section 9.9
*/
// -----------------------------------------------------------------------------
void extract_document( std::string       &document,
                       std::string const& strHttp)
{
   if( strHttp.c_str()[0]=='G' &&
       strHttp.c_str()[1]=='E' &&
       strHttp.c_str()[2]=='T' &&
       strHttp.c_str()[3]==' ')
   {
      size_t pos = strHttp.find( ' ', 4);
      document = strHttp.substr( 4, pos-4);
   }
   else if( strHttp.c_str()[0]=='P' &&
            strHttp.c_str()[1]=='O' &&
            strHttp.c_str()[2]=='S' &&
            strHttp.c_str()[3]=='T' &&
            strHttp.c_str()[4]==' ')
   {
      size_t pos = strHttp.find( ' ', 5);
      document = strHttp.substr( 5, pos-5);
   }
}

// -----------------------------------------------------------------------------
static void extract_to_CRLF( std::string       &rExtract, 
                             std::string const& what,
                             std::string const& strHttp)
{
   std::string::size_type pos = strHttp.find( what.c_str());
   if( pos != std::string::npos)
   {
      std::string::size_type pos2 = strHttp.find_first_of( "\r\n", pos+what.length());
      if( pos2 != std::string::npos)
         rExtract = strHttp.substr( pos+what.length(), pos2-pos-what.length());
   }
}

// -----------------------------------------------------------------------------
//  http_URL = "http:" "//" host [ ":" port ] [ abs_path [ "?" query ]]
//
void extract_document( std::string       &rHttpRequest,
                       std::string       &rHttpPath,
                       std::string       &rHttpQuery,
                       std::string       &rUserIP,
                       std::string       &rSeqNum,
                       HttpCmds          &rHttpCmds,
                       HttpNameValues    &rNameValues,
                       std::string const& strHttp)
{
   if( strHttp.data()[0] == 'G' &&
       strHttp.data()[1] == 'E' &&
       strHttp.data()[2] == 'T' &&
       strHttp.data()[3] == ' ')
   {
      size_t pos = strHttp.find( ' ', 4);
      rHttpRequest = strHttp.substr( 4, pos-4);
   }
   else if( strHttp.data()[0] == 'P' &&
            strHttp.data()[1] == 'O' &&
            strHttp.data()[2] == 'S' &&
            strHttp.data()[3] == 'T' && 
            strHttp.data()[4] == ' ')
   {
      size_t pos = strHttp.find( ' ', 5);
      rHttpRequest = strHttp.substr( 5, pos-5);
   }
   else if( strHttp.data()[0] == 'H' &&
            strHttp.data()[1] == 'E' &&
            strHttp.data()[2] == 'A' &&
            strHttp.data()[3] == 'D' && 
            strHttp.data()[4] == ' ')
   {
      size_t pos = strHttp.find( ' ', 5);
      rHttpRequest = strHttp.substr( 5, pos-5);
   }
   else if( strHttp.data()[0] == 'T' &&
            strHttp.data()[1] == 'R' &&
            strHttp.data()[2] == 'A' &&
            strHttp.data()[3] == 'C' && 
            strHttp.data()[4] == 'E' && 
            strHttp.data()[5] == ' ')
   {
      size_t pos = strHttp.find( ' ', 5);
      rHttpRequest = strHttp.substr( 5, pos-5);
   }
   else
   {
      rHttpRequest = strHttp;
   }

   std::string lStrHttp = rHttpRequest;
   //lStrHttp = UriDecode( lStrHttp);

   // --- rHttpPath ? rHttpQuery ---
   {
      std::string::size_type pos = rHttpRequest.find_first_of( "?", 0);
      if( pos != std::string::npos)
      {
         rHttpPath  = rHttpRequest.substr( 0, pos);
         rHttpQuery = rHttpRequest.substr( pos, std::string::npos);
      }
      else
      {
         rHttpPath = rHttpRequest;
      }
   }

   // --- rUserIP ---
   std::string::size_type pos = rHttpPath.find( "/49=");
   if( pos != std::string::npos)
   {
      std::string::size_type eomatch = rHttpPath.find_first_of( " /", pos+1);
      rUserIP = rHttpPath.substr( pos+4, eomatch-( pos+4));
      if( eomatch == std::string::npos)
         lStrHttp = "/";
      else
         lStrHttp = rHttpPath.substr( eomatch, std::string::npos);
   }
   // --- rSeqNum ---
   pos = rHttpPath.find( "/34=");
   if( pos != std::string::npos)
   {
      std::string::size_type eomatch = rHttpPath.find_first_of( " /", pos+1);
      rSeqNum = rHttpPath.substr( pos+4, eomatch-( pos+4));
      if( eomatch == std::string::npos)
         lStrHttp = "/";
      else
         lStrHttp = rHttpPath.substr( eomatch, std::string::npos);
   }

   // --- rHttpCmds ---
   std::string::size_type posSep = lStrHttp.find_first_of( " /?", 1);
   while( posSep != std::string::npos)
   {
      std::string str = lStrHttp.substr( 0, posSep);
      rHttpCmds.push_back( str);
      lStrHttp = lStrHttp.substr( posSep);

      if( lStrHttp.data()[0] == '?')
         break;

      posSep = lStrHttp.find_first_of( " /?", 1);
   }

   // --- rNameValues ---
   if( lStrHttp.data()[0] != '?')
      rHttpCmds.push_back( lStrHttp);

   if( !rHttpQuery.empty())
      HttpNameValue::ParseHTTP( rNameValues, rHttpQuery.data()+1, rHttpQuery.length()-1);

   // --- Host: ---
   std::string lHost;
   {
      static const std::string what = "Host: ";
      ::extract_to_CRLF( lHost, what, strHttp);
   }
   // --- Connection: ---
   std::string lConnection;
   {
      static const std::string what = "Connection: ";
      ::extract_to_CRLF( lConnection, what, strHttp);
   }
   // --- Referer: ---
   std::string lReferer;
   {
      static const std::string what = "Referer: ";
      ::extract_to_CRLF( lReferer, what, strHttp);
   }
   // --- Accept: ---
   // --- User-Agent: ---
   // --- Accept-Encoding: ---
   // --- Accept-Language: ---
   // --- Accept-Charset: ---
}

// -----------------------------------------------------------------------------
void push_back( MComVectorMessageBase *pCom,
                const char*            pMsg,
                size_t                 lSizeLeft,
                size_t                 chunck)
{
   while( lSizeLeft >= chunck)
   {
      pCom->push_back( pMsg, chunck);
      lSizeLeft -= chunck;
      pMsg += chunck;
   }
   if( lSizeLeft != 0)
      pCom->push_back( pMsg, lSizeLeft);
}

// -----------------------------------------------------------------------------
// Uri encode and decode.
// RFC1630, RFC1738, RFC2396

// Only alphanum is safe.
const char SAFE[256] =
{
    /*      0 1 2 3  4 5 6 7  8 9 A B  C D E F */
    /* 0 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
    /* 1 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
    /* 2 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
    /* 3 */ 1,1,1,1, 1,1,1,1, 1,1,0,0, 0,0,0,0,
    
    /* 4 */ 0,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1,
    /* 5 */ 1,1,1,1, 1,1,1,1, 1,1,1,0, 0,0,0,0,
    /* 6 */ 0,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1,
    /* 7 */ 1,1,1,1, 1,1,1,1, 1,1,1,0, 0,0,0,0,
    
    /* 8 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
    /* 9 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
    /* A */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
    /* B */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
    
    /* C */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
    /* D */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
    /* E */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
    /* F */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0
};

// -----------------------------------------------------------------------------
const char HEX2DEC[256] = 
{
    /*       0  1  2  3   4  5  6  7   8  9  A  B   C  D  E  F */
    /* 0 */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    /* 1 */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    /* 2 */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    /* 3 */  0, 1, 2, 3,  4, 5, 6, 7,  8, 9,-1,-1, -1,-1,-1,-1,
    
    /* 4 */ -1,10,11,12, 13,14,15,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    /* 5 */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    /* 6 */ -1,10,11,12, 13,14,15,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    /* 7 */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    
    /* 8 */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    /* 9 */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    /* A */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    /* B */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    
    /* C */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    /* D */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    /* E */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    /* F */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1
};

// -----------------------------------------------------------------------------
std::string UriEncode( std::string const& rSrc)
{
   const char DEC2HEX[16 + 1] = "0123456789ABCDEF";
   const unsigned char * pSrc = (const unsigned char *)rSrc.c_str();
   size_t SRC_LEN = rSrc.length();
   unsigned char * const pStart = new unsigned char[SRC_LEN * 3];
   unsigned char * pEnd = pStart;
   const unsigned char * const SRC_END = pSrc + SRC_LEN;

   for (; pSrc < SRC_END; ++pSrc)
   {
      if (SAFE[*pSrc]) 
         *pEnd++ = *pSrc;
      else
      {
         // escape this char
         *pEnd++ = '%';
         *pEnd++ = DEC2HEX[*pSrc >> 4];
         *pEnd++ = DEC2HEX[*pSrc & 0x0F];
      }
   }

   std::string sResult((char *)pStart, (char *)pEnd);
   delete [] pStart;
   return sResult;
}

// -----------------------------------------------------------------------------
std::string UriDecode( std::string const& rSrc)
{
   // Note from RFC1630: "Sequences which start with a percent
   // sign but are not followed by two hexadecimal characters
   // (0-9, A-F) are reserved for future extension"

   const unsigned char * pSrc = (const unsigned char *)rSrc.c_str();
   size_t SRC_LEN = rSrc.length();
   const unsigned char * const SRC_END = pSrc + SRC_LEN;
   // last decodable '%' 
   const unsigned char * const SRC_LAST_DEC = SRC_END - 2;

   char * const pStart = new char[SRC_LEN];
   char * pEnd = pStart;

   while (pSrc < SRC_LAST_DEC)
   {
      if (*pSrc == '%')
      {
         char dec1, dec2;
         if (-1 != (dec1 = HEX2DEC[*(pSrc + 1)])
            && -1 != (dec2 = HEX2DEC[*(pSrc + 2)]))
         {
            *pEnd++ = (dec1 << 4) + dec2;
            pSrc += 3;
            continue;
         }
      }
      else if (*pSrc == '+') // modif perso: remplacer les '+' par des espaces
      {                      // commentaire: trouver une fonction off the shelf qui fait ca
         *pEnd = ' ';
         pEnd++;
         pSrc++;
         continue;
      }
      
      *pEnd++ = *pSrc++;
   }

   // the last 2- chars
   //while (pSrc < SRC_END) // code original
   //   *pEnd++ = *pSrc++;  // code original - ce qui suit est la version modifiée:
   while (pSrc < SRC_END)
   {
      if (*pSrc == '+')
      {
         *pEnd = ' ';
         pEnd++;
         pSrc++;
      }
      else
         *pEnd++ = *pSrc++;
   }

   std::string sResult(pStart, pEnd);
   delete [] pStart;
   return sResult;
}

// --- mongoose ----------------------------------------------------------------
// URL-decode input buffer into destination buffer.
// 0-terminate the destination buffer. Return the length of decoded data.
// form-url-encoded data differs from URI encoding in a way that it
// uses '+' as character for space, see RFC 1866 section 8.2.1
// http://ftp.ics.uci.edu/pub/ietf/html/rfc1866.txt
static size_t url_decode( const char* src,
                          size_t      src_len,
                          char*       dst,
                          size_t      dst_len,
                          int is_form_url_encoded) 
{
   size_t i, j;
   int a, b;
#define HEXTOI(x) (isdigit(x) ? x - '0' : x - 'W')

   for( i = j = 0; i < src_len && j < dst_len - 1; i++, j++)
   {
      if( src[i] == '%' &&
          isxdigit(* (unsigned char *) (src + i + 1)) &&
          isxdigit(* (unsigned char *) (src + i + 2))) 
      {
         a = tolower(* (unsigned char *) (src + i + 1));
         b = tolower(* (unsigned char *) (src + i + 2));
         dst[j] = (char) ((HEXTOI(a) << 4) | HEXTOI(b));
         i += 2;
      }
      else if( is_form_url_encoded && src[i] == '+')
      {
         dst[j] = ' ';
      }
      else 
      {
         dst[j] = src[i];
      }
   }
   dst[j] = '\0'; /* Null-terminate the destination */
   return j;
}

// --- mongoose ----------------------------------------------------------------
static void url_encode(const char *src, char *dst, size_t dst_len)
{
   static const char *dont_escape = "._-$,;~()";
   static const char *hex = "0123456789abcdef";
   const char *end = dst + dst_len - 1;

   for( ; *src != '\0' && dst < end; src++, dst++)
   {
      if( isalnum(*(unsigned char *) src) ||
          strchr(dont_escape, * (unsigned char *) src) != NULL)
      {
         *dst = *src;
      }
      else if (dst + 2 < end)
      {
         dst[0] = '%';
         dst[1] = hex[(* (unsigned char *) src) >> 4];
         dst[2] = hex[(* (unsigned char *) src) & 0xf];
         dst += 2;
      }
   }
   *dst = '\0';
}

// --- mongoose ----------------------------------------------------------------
// Protect against directory disclosure attack by removing '..',
// excessive '/' and '\' characters
static void remove_double_dots_and_double_slashes( char *s)
{
   char *p = s;

   while (*s != '\0')
   {
      *p++ = *s++;
      if( s[-1] == '/' || s[-1] == '\\')
      {
         // Skip all following slashes and backslashes
         while( *s == '/' || *s == '\\')
         {
            s++;
         }
         // Skip all double-dots
         while (*s == '.' && s[1] == '.')
         {
            s += 2;
         }
      }
   }
   *p = '\0';
}

// -----------------------------------------------------------------------------
/*
 *  urldecode  --  filter from URL-encoded text to plain text
 *  
 *  Description: this program reads from standard input and decodes %HH
 *  sequences in their byte equivalent according to the RFC 3986.
 *  
 *  Moreover, the '+' char is translated into space, as this is the common
 *  convention still in use on the WEB.
 *  
 *  Invalid and incomplete sequences, for example "%0q" or "abc%", are
 *  left untoched.  The result is sent to the standard output.
 *  
 *  Example: "a%20b" becomes "a b".
 *  
 *  Author: Umberto Salsi <salsi@icosaedro.it>
 *  
 *  Version: 2008-04-27
 *  
 *  Updates: www.icosaedro.it/apache/urldecode.c
 *  
 *  References: Uniform Resource Identifier (URI): Generic Syntax, RFC 3986
 */
/*
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>


int c, c1, c2;
// c is the current char being examined while c1,c2 are the next two following chars.


void next()
{
	if( c == EOF )
		return;

	c = c1;
	c1 = c2;
	c2 = getchar();
}


int main()
{
	// Initialize c,c1,c2:
	c = getchar();
	c1 = EOF;
	c2 = EOF;
	if( c != EOF ){
		c1 = getchar();
		if( c1 != EOF ){
			c2 = getchar();
		}
	}

	while( c != EOF ){

		if( c == '%' ){

			if( isxdigit(c1) && isxdigit(c2) ){

				// Valid %HH sequence found.

				c1 = tolower(c1);
				c2 = tolower(c2);

				if( c1 <= '9' )
					c1 = c1 - '0';
				else
					c1 = c1 - 'a' + 10;
				if( c2 <= '9' )
					c2 = c2 - '0';
				else
					c2 = c2 - 'a' + 10;

				putchar( 16 * c1 + c2 );

				next();
				next();
				next();

			} else {

				// Invalid or incomplete sequence.

				putchar('%');
				next();
			}

		} else if( c == '+' ){

			putchar(' ');
			next();

		} else {

			putchar(c);
			next();

		}
	}
	
	return 0;
}
*/


/*
// -----------------------------------------------------------------------------
// micro_httpd.c

static void
strdecode( char* to, char* from )
    {
    for ( ; *from != '\0'; ++to, ++from )
	{
	if ( from[0] == '%' && isxdigit( from[1] ) && isxdigit( from[2] ) )
	    {
	    *to = hexit( from[1] ) * 16 + hexit( from[2] );
	    from += 2;
	    }
	else
	    *to = *from;
	}
    *to = '\0';
    }


static int
hexit( char c )
    {
    if ( c >= '0' && c <= '9' )
	return c - '0';
    if ( c >= 'a' && c <= 'f' )
	return c - 'a' + 10;
    if ( c >= 'A' && c <= 'F' )
	return c - 'A' + 10;
    return 0;		// shouldn't happen, we're guarded by isxdigit()
    }


static void
strencode( char* to, size_t tosize, const char* from )
    {
    int tolen;

    for ( tolen = 0; *from != '\0' && tolen + 4 < tosize; ++from )
	{
	if ( isalnum(*from) || strchr( "/_.-~", *from ) != (char*) 0 )
	    {
	    *to = *from;
	    ++to;
	    ++tolen;
	    }
	else
	    {
	    (void) sprintf( to, "%%%02x", (int) *from & 0xff );
	    to += 3;
	    tolen += 3;
	    }
	}
    *to = '\0';
    }
*/

static const char *HTTPStatus2text(int code) {
	static const struct {
		int code;
		const char *msg;
	} translation[] = {
		{100, "Continue"},
		{101, "Switching Protocols"},
		{102, "Processing"},
		{200, "OK"},
		{201, "Created"},
		{202, "Accepted"},
		{203, "Non-Authoritative Information"},
		{204, "No Content"},
		{205, "Reset Content"},
		{206, "Partial Content"},
		{207, "Multi-Status"},
		{300, "Multiple Choices"},
		{301, "Moved Permanently"},
		{302, "Found"},
		{303, "See Other"},
		{304, "Not Modified"},
		{305, "Use Proxy"},
		{307, "Temporary Redirect"},
		{400, "Bad Request"},
		{401, "Unauthorized"},
		{402, "Payment Required"},
		{403, "Forbidden"},
		{404, "Not Found"},
		{405, "Method Not Allowed"},
		{406, "Not Acceptable"},
		{407, "Proxy Authentication Required"},
		{408, "Request Timeout"},
		{409, "Conflict"},
		{410, "Gone"},
		{411, "Length Required"},
		{412, "Precondition Failed"},
		{413, "Request Entity Too Large"},
		{414, "URI too long"},
		{415, "Unsupported Media Type"},
		{416, "Requested Range Not Satisfiable"},
		{417, "Expectation Failed"},
		{422, "Unprocessable Entity"},
		{423, "Locked"},
		{424, "Failed Dependency"},
		{426, "Upgrade Required"},
		{500, "Internal Server Error"},
		{501, "Not Implemented"},
		{502, "Bad Gateway"},
		{503, "Service Unavailable"},
		{503, "Gateway Timeout"},
		{504, "HTTP Version Not Supported"},
		{507, "Insufficient Storage"},
		{0,""}
	};
	int i;
	for(i=0; translation[i].code!=0; i++)
		if(translation[i].code==code)
			return translation[i].msg;
	return "unknown error code";
}

// -*- modeline for vim, do not remove please :-) -*-
// vim: set expandtab tabstop=3 shiftwidth=3 autoindent smartindent:
