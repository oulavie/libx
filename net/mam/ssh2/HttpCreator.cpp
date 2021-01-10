#include "HttpCreator.h"
#include "LibHt/HttpHandies.h"
#include "LibHt/HtmlWriter.h"
#include "LibX/xBuffer.h"
#include "../Http/HttpCLient.h"
#include "LibX/xConsoleColor.h"
#include "GatewayLib/BaseGateway.h"

// http://en.wikipedia.org/wiki/List_of_HTTP_header_fields
// X-Requested-With: XMLHttpRequest

// Internet Engineering Task Force (IETF) 
// http://www.w3.org/Protocols/HTTP/1.1/draft-ietf-http-v11-spec-01.html
// http://tools.ietf.org/html/rfc2109 "HTTP State Management Mechanism"
// http://tools.ietf.org/html/rfc2965 "HTTP State Management Mechanism"
// http://tools.ietf.org/html/rfc6265 "HTTP State Management Mechanism"
// http://fr.wikipedia.org/wiki/Cookie_(informatique)
// http://www.w3.org/Protocols/
// http://www.w3schools.com/media/media_mimeref.asp
// http://www.rfc-editor.org/rfc/rfc2045.txt         // MIME
// http://www.rfc-editor.org/rfc/rfc2046.txt         // MIME
// http://www.rfc-editor.org/rfc/rfc2048.txt         // MIME

/*
   HTTP/1.1 400 Bad Request
   Content-Type: text/html
   Content-Length: 111

   <html><body>
   <h2>No Host: header received</h2>
   HTTP 1.1 requests must include the Host: header.
   </body></html>
*/
// Set-Cookie: name=value
// Cookie: name=value

// -----------------------------------------------------------------------------
bool Http404::GetHtmlPage( std::string      &rHttpHeader,
                           HtmlWriter       &w,
                           std::string const& rUserIP)
{
   w = "Document not found";
   w.process( false);

   rHttpHeader = "HTTP/1.1 404 Error 404 document not found\r\n";
   rHttpHeader += "Content-Type: text/html;charset=UTF-8\r\n";
   char buffer[256];
   _snprintf( buffer, sizeof( buffer), "Content-Length: %ld\r\n", w.length());
   rHttpHeader += buffer;
   //rHttpPage += "Connection: Keep-Alive\r\n";
   //rHttpPage += "Keep-Alive: timeout=10, max=1\r\n"; // deprecated( iron expire tjrs au bout de 5mn)
   //rHttpPage += "Cache-Control: max-age=3600\r\n"; // max-age=3600, must-revalidate, no-cache, no-store

   rHttpHeader += "\r\n";

   return true;
}

// -----------------------------------------------------------------------------
void CreateHttpHeader( std::string       &rHttpHeader,
                       size_t             htmlLength,
                       std::string const& rUsername)
{
   // --- rfc2616 ---

   static char buffer[1000];
   rHttpHeader = "HTTP/1.1 200 OK\r\n";  // CRLF
   rHttpHeader += "Content-Type: text/html;charset=UTF-8\r\n"; // CRLF
   
   // --- message body included ---
   // The presence of a message-body in a request is signaled by the
   // inclusion of a Content-Length or Transfer-Encoding header field in
   // the request’s message-headers.
   // All 1xx (informational), 204 (no content), and 304 (not modified) responses
   // MUST NOT include a message-body. All other responses do include a
   // message-body, although it MAY be of zero length.
   // Messages MUST NOT include both a Content-Length header field and a
   // non-identity transfer-coding. If the message does include a nonidentity
   // transfer-coding, the Content-Length MUST be ignored.
   // When a Content-Length is given in a message where a message-body is
   // allowed, its field value MUST exactly match the number of OCTETs in
   // the message-body. HTTP/1.1 user agents MUST notify the user when an
   // invalid length is received and detected.
   sprintf( buffer, "Content-Length: %ld\r\n", htmlLength);
   rHttpHeader += buffer;
   //rHttpHeader += "Transfer-Encoding: ";

 //rHttpHeader += "Connection: Keep-Alive\r\n";
 //rHttpHeader += "Keep-Alive: timeout=10, max=1\r\n"; // deprecated( iron expire tjrs au bout de 5mn)
 //rHttpHeader += "Cache-Control: max-age=3600\r\n";
   rHttpHeader += "Expires: Thu, 01-Jan-1970 00:00:01 GMT\r\n";                // Date in the past
   rHttpHeader += "Cache-Control: must-revalidate, no-cache, no-store\r\n";    // HTTP 1.1
   rHttpHeader += "Cache-Control: post-check=0, pre-check=0\r\n";
   rHttpHeader += "Pragma: no-cache\r\n";                                      // HTTP 1.0
//rHttpHeader: Tue, 17 Dec 2002 16:52:50 GMT 
//Cache-Control: max-age=86400
//Cache-Control: no-store, no-cache, must-revalidate, post-check=0, pre-check=0
//Pragma: no-cache

   // --- The Set-Cookie Header ---
   HttpClient* pClient = HttpClients::Instance().GetClient( rUsername);
   if( pClient == NULL)
   {
      // Clearing a Cookie Value 
      // 1/ Set the cookie's value to null.
      // 2/ Set the cookie's expiration date to some time in the past.
      //sprintf( buffer, "Set-Cookie: 34=deleted; Expires=Thu, 01-Jan-1970 00:00:01 GMT; Path=/; Domain=touit.dyndns.com");
      sprintf( buffer, "Set-Cookie: 34=0; Expires=Thu, 01-Jan-1970 00:00:01 GMT; Path=/");
   }
   else
      //sprintf( buffer, "Set-Cookie: 34=%ld; Path=/; Domain=touit.dyndns.com", pClient->mUserSeqNum);
      //sprintf( buffer, "Set-Cookie: 34=%ld; Path=/", pClient->mUserSeqNum);
      sprintf( buffer, "Set-Cookie: 49=%s; Path=/", rUsername.data());
   rHttpHeader += buffer;
   rHttpHeader += "\r\n";
   //rHttpHeader += "; Secure; HttpOnly";
   //::verbose( __FILE__, __LINE__, V_INFO, "Set-Cookie: %s", buffer);

   rHttpHeader += "\r\n"; // [blank line here] CRLF
}

// -----------------------------------------------------------------------------
/*
void create_http_page_from_html_page( std::string &rHttpPage, 
                                      std::vector<std::string> const& rHtmlVect,
                                      std::string const& rUserIP)
{
   //HttpClient* pClient = HttpClients::Instance().GetClient( rUserIP);

   size_t i,htmlLength = 0;
   for( i=0; i<rHtmlVect.size(); ++i)
      htmlLength += rHtmlVect[i].length();

   //htmlLength += 2;

   rHttpPage.reserve( htmlLength+500);
   rHttpPage = "HTTP/1.1 200 OK\r\n";
   rHttpPage += "Content-Type: text/html;charset=UTF-8\r\n";
   char buffer[256];
   _snprintf( buffer, sizeof( buffer), "Content-Length: %ld\r\n",htmlLength);
   rHttpPage += buffer;
 //rHttpPage += "Connection: Keep-Alive\r\n";
 //rHttpPage += "Keep-Alive: timeout=10, max=1\r\n"; // deprecated( iron expire tjrs au bout de 5mn)
 //rHttpPage += "Cache-Control: max-age=3600\r\n";
   rHttpPage += "Expires: Mon, 14 Oct 2002 05:00:00 GMT\r\n";                // Date in the past
   rHttpPage += "Cache-Control: must-revalidate, no-cache, no-store\r\n";    // HTTP 1.1
   rHttpPage += "Cache-Control: post-check=0, pre-check=0\r\n";
   rHttpPage += "Pragma: no-cache\r\n";                                      // HTTP 1.0
//Expires: Tue, 17 Dec 2002 16:52:50 GMT 
//Cache-Control: max-age=86400
//Cache-Control: no-store, no-cache, must-revalidate, post-check=0, pre-check=0
//Pragma: no-cache
   //if( pClient!=NULL && pClient->mAddCookie)
   //{
   //   rHttpPage += "Set-Cookie: client=";
   //   rHttpPage += rUserIP;
   //   rHttpPage += "\r\n";
   //   pClient->mAddCookie = false;
   //}
   rHttpPage += "\r\n";

   for( i=0; i<rHtmlVect.size(); ++i)
      rHttpPage += rHtmlVect[i];

   //rHttpPage += "\r\n";
}
*/

// -----------------------------------------------------------------------------
/*
void create_http_multipart_page_from_html_page( std::string &rHttpPage,
                                                std::vector<std::string> const& rHtmlVect,
                                                std::string const& rUserIP)
{
   size_t i,htmlLength = 0;
   for( i=0; i<rHtmlVect.size(); ++i)
      htmlLength += rHtmlVect[i].length();

   static std::string httpStr;
   httpStr.reserve( htmlLength+500);

   httpStr = "HTTP/1.1 200 OK\r\n";
   httpStr += "Content-Type: text/html;charset=UTF-8\r\n";
 //httpStr += "Transfer-Encoding: chunked\r\n";
 //httpStr += "\r\n";

   //std::string lenHexa = IToH::GetString( htmlLength);
   //httpStr += lenHexa;
   char buffer[256];
   _snprintf( buffer, sizeof( buffer), "Content-Length: %ld\r\n",htmlLength);
   httpStr += buffer;
   httpStr += "Connection: Keep-Alive\r\n";
   httpStr += "Keep-Alive: timeout=10, max=1\r\n";
   httpStr += "\r\n";

   for( i=0; i<rHtmlVect.size(); ++i)
      httpStr += rHtmlVect[i];

   rHttpPage.assign( httpStr.c_str(),httpStr.length());
}
*/

// -----------------------------------------------------------------------------
bool read_fileA( HtmlWriter &w,
                 const char*  pFilePath,
                 const char*  pImage)
{
   FILE* pFile = fopen( pImage, "rb");
   if( pFile == NULL)
   {
      BaseGateway *pEngine = BaseGateway::GetInstance();
      Configurator const& rConfig = pEngine->GetConfigurator();
      std::string lPathString = rConfig.GetValue( pFilePath);
      if( lPathString.empty())
         return false;
      const char* p = lPathString.c_str();
      const char  c = p[lPathString.length()-1];
      if( (c=='/' || c=='\\') && (*pImage=='/' || *pImage=='\\'))
         lPathString += pImage+1;
      else
         lPathString += pImage;
      pFile = fopen( lPathString.c_str(), "rb");
   }
   if( pFile != NULL)
   {
      /*
      static const size_t len = 99999;
      static char line[len];
      char* read = NULL;
      while(( read = fgets( line, len, pFile))!=NULL)
      {
         httpStr += line;
         httpStr += "\n";
      }
      fclose( pFile);
      return true;
      */

      // obtain file size:
      fseek( pFile, 0, SEEK_END);
      size_t size2read = ftell( pFile);
      rewind( pFile);

      size_t result = fread( (void*)w.body_ptr(), 1, w.body_size(), pFile);
      w.set_body_length( result);
      fclose( pFile);
      if( result == size2read)
         return true;
   }
   return false;
}

// -----------------------------------------------------------------------------
bool GetHtmlPageFileA( std::string &rHttpHeader, 
                       HtmlWriter &w,
                       const char*  pFilePath,
                       const char*  pImage,
                       const char*  htmlContentType,
                       const char*  extra)
{
   if( !::read_fileA( w, pFilePath, pImage))
      return false;

   char buffer[256];
   rHttpHeader = "HTTP/1.1 200 OK\r\n";
   _snprintf( buffer, sizeof( buffer), "Content-Type: %s\r\n", htmlContentType);
   rHttpHeader += buffer;
   _snprintf( buffer, sizeof( buffer), "Content-Length: %ld\r\n", (long)w.length());
   rHttpHeader += buffer;
   if( extra!=NULL)
      rHttpHeader += extra;
   rHttpHeader += "\r\n";
   return true;
}

// -----------------------------------------------------------------------------
bool read_fileB( HtmlWriter &w, 
                 const char*  pFilePath,
                 const char*  pImage)
{
   FILE* pFile = fopen( pImage, "rb");
   if( pFile == NULL)
   {
      BaseGateway *pEngine = BaseGateway::GetInstance();
      Configurator const& rConfig = pEngine->GetConfigurator();
      std::string lPathString = rConfig.GetValue( pFilePath);
      if( lPathString.empty())
         return false;
      const char* p = lPathString.c_str();
      const char  c = p[lPathString.length()-1];
      if( (c=='/' || c=='\\') && (*pImage=='/' || *pImage=='\\'))
         lPathString += pImage+1;
      else
         lPathString += pImage;
      pFile = fopen( lPathString.c_str(), "rb");
   }
   if( pFile != NULL)
   {
      // obtain file size:
      fseek( pFile, 0, SEEK_END);
      size_t size2read = ftell( pFile);
      rewind( pFile);

      size_t result = fread( (void*)w.body_ptr(), 1, w.body_size(), pFile);
      w.set_body_length( result);
      fclose( pFile);
      if( result == size2read)
         return true;
   }
   return false;
/*
   char buf[1024];
   FILE* pFile = fopen( lPath.GetString().c_str(),"rb");
   if( pFile!=NULL)
   {
      rHtmlPage = "";

      fgets( buf, sizeof( buf), pFile);
      while( !feof( pFile))
      {
         rHtmlPage += std::string( buf, strlen( buf));
         fgets( buf, sizeof( buf), pFile);
      }
   }
*/
}

// -----------------------------------------------------------------------------
bool GetHtmlPageFileB( std::string &rHttpHeader,
                       HtmlWriter  &w,
                       const char*  pFilePath,
                       const char*  pImage,
                       const char*  htmlContentType,
                       const char*  extra)
{
   if( !::read_fileB( w, pFilePath, pImage))
      return false;

   char buffer[256];
   rHttpHeader = "HTTP/1.1 200 OK\r\n";
   _snprintf( buffer, sizeof( buffer), "Content-Type: %s\r\n", htmlContentType);
   rHttpHeader += buffer;
   _snprintf( buffer, sizeof( buffer), "Content-Length: %ld\r\n", (long)w.length());
   rHttpHeader += buffer;
   if( extra != NULL)
      rHttpHeader += extra;
   rHttpHeader += "\r\n";
   return true;
}
