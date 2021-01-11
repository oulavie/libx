
#pragma once
#ifndef mam_http_h
#define mam_http_h

#include <sstream>
#include <string>

namespace http
{

//--------------------------------------------------------------------------------------------------
// auto html = http::get_html( "<html><body><h1>Hello world!!</h1></body></html>" );
//std::string get_http(const char *html_, bool keep_alive_ = true)
std::string get_http(const char *html_)
{
  std::string html(html_);

  std::stringstream rtn;
  rtn << "HTTP/1.1 200 OK\n";
//rtn << "Content-Type: text/html\n";
  rtn << "Content-Type: text/html;charset=UTF-8\n";
  rtn << "Date: Mon, 27 Jul 2009 12:28:53 GMT\n";
  rtn << "Server: Apache/2.2.14 (Win32)\n";
  rtn << "Last-Modified: Wed, 22 Jul 2009 19:15:56 GMT\n";
  rtn << "Content-Length: " << html.size() << "\n";
/*
  if (keep_alive_)
    rtn << "Connection: keep-alive\n";
  else
    rtn << "Connection: Closed\n";
*/
  rtn << "Expires: Thu, 01-Jan-1970 00:00:01 GMT\n"; // date in past
  rtn << "Cache-Control: must-revalidate, no-cache, no-store\n";
  rtn << "Cache-Control: post-check=0, pre-check=0\n";
//  rtn << "Cache-Control: max-age=0\n";
  rtn << "Pragma: no-cache\n";


  // HTTP headers are used to pass additional information with HTTP response or HTTP requests.
  // A cookie is an HTTP request header i.e. used in the requests sent by the user to the server.
  // It contains the cookies previously sent by the server using set-cookies. It is an optional header.
  // Cookie: name=value
  // Set-Cookie: <cookie-name>=<cookie-value> | Expires=<date> | Max-Age=<non-zero-digit> | Domain=<domain-value> | Path=<path-value> | SameSite=Strict|Lax|none
  // Set-Cookie: sessionId=38afes7a8

  rtn<< " Set-Cookie: 34=deleted; path=/; expires=Thu, 01 Jan 1970 00:00:00 GMT\n";
  rtn<< " Set-Cookie: 49=deleted; path=/; expires=Thu, 01 Jan 1970 00:00:00 GMT\n";
//  rtn << "Set-Cookie: 49=tutu1; path=/\n";
//  rtn << "Set-Cookie: 34=2; path=/\n";
  //rtn << "Set-Cookie: 34=0; Expires=Thu, 01-Jan-1970 00:00:01 GMT; Path=/\n";
//  rtn << "Set-Cookie: 49=%s; Path=/", rUsername.data()

  rtn << "\n"; // [blank line here] CRLF \r\n
  rtn << html;
  return rtn.str();
}

} // namespace http

#endif

// vim: set expandtab tabstop=2 shiftwidth=2 autoindent smartindent:
