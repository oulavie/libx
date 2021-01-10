#ifndef _HttpCreator_H_
#define _HttpCreator_H_

#include <string>
#include <vector>

class HttpCmds;
class HttpNameValues;
class HtmlWriter;

// -----------------------------------------------------------------------------
void CreateHttpHeader( std::string       &rHttpHeader, 
                       size_t             htmlLength, 
                       std::string const& rUserIP);

bool GetHtmlPageFileA( std::string &rHttpHeader, 
                       HtmlWriter  &rHtmlWriter, 
                       const char* pFilePath, 
                       const char* pImage, 
                       const char* htmlContentType, 
                       const char* extra=0);

bool GetHtmlPageFileB( std::string &rHttpHeader, 
                       HtmlWriter  &rHtmlWriter,
                       const char* pFilePath, 
                       const char* pImage, 
                       const char* htmlContentType, 
                       const char* extra=0);

// -----------------------------------------------------------------------------
class HttpCreator
{
   HttpCreator( HttpCreator const&);
   HttpCreator& operator=( HttpCreator const&);
protected:
   HttpCreator() {}
public:

   virtual ~HttpCreator() {}

   virtual bool HExecute( HttpCmds const& rHttpCmds, HttpNameValues const& rNameValues, std::string const& rUserIP) = 0;
   virtual bool GetHtmlPage( std::string &rHttpHeader, HtmlWriter &rHtmlWriter, std::string const& rUserIP) = 0;
   virtual void GetHtmlBody( HtmlWriter &rHtmlVect, std::string const& rUserIP) {}
};

// -----------------------------------------------------------------------------
class Http404 : public HttpCreator
{
public:
   virtual bool HExecute( HttpCmds const& rHttpCmds, HttpNameValues const& rNameValues, std::string const& rUserIP) { return true; }
   virtual bool GetHtmlPage( std::string &rHttpHeader, HtmlWriter &rHtmlWriter, std::string const& rUserIP);
};

// -----------------------------------------------------------------------------
class HttpFavicon : public HttpCreator
{
   std::string mFile;
public:
   HttpFavicon() {}
   HttpFavicon( std::string const& rFile)
      : mFile( rFile)
   {
   }
   virtual bool HExecute( HttpCmds const& rHttpCmds, HttpNameValues const& rNameValues, std::string const& rUserIP) { return true; }
   virtual bool GetHtmlPage( std::string &rHttpHeader, HtmlWriter &rHtmlWriter, std::string const& rUserIP)
   {
      return ::GetHtmlPageFileB( rHttpHeader, rHtmlWriter, "ICO_DIR", mFile.c_str(), "favicon.ico");
   }
};

// -----------------------------------------------------------------------------
// H..
class HttpHtml : public HttpCreator
{
   std::string mFile;
public:
   HttpHtml() {}
   HttpHtml( std::string const& rFile)
      : mFile( rFile)
   {
   }
   virtual bool HExecute( HttpCmds const& rHttpCmds, HttpNameValues const& rNameValues, std::string const& rUserIP) { return true; }
   virtual bool GetHtmlPage( std::string &rHttpHeader, HtmlWriter &rHtmlWriter, std::string const& rFile)
   {
      return ::GetHtmlPageFileA( rHttpHeader, rHtmlWriter, "HTML_DIR", rFile.c_str(), "text/html");
   }
};

// -----------------------------------------------------------------------------
// .I.
class HttpJpg : public HttpCreator
{
public:
   virtual bool HExecute( HttpCmds const& rHttpCmds, HttpNameValues const& rNameValues, std::string const& rUserIP) { return true; }
   virtual bool GetHtmlPage( std::string &rHttpHeader, HtmlWriter &rHtmlWriter, std::string const& rFile)
   {
      return ::GetHtmlPageFileB( rHttpHeader, rHtmlWriter, "JPG_DIR", rFile.c_str(), "image/jpeg", "Cache-Control: max-age=3600\r\n");
   }
};

// -----------------------------------------------------------------------------
// .I.
class HttpPng : public HttpCreator
{
public:
   virtual bool HExecute( HttpCmds const& rHttpCmds, HttpNameValues const& rNameValues, std::string const& rUserIP) { return true; }
   virtual bool GetHtmlPage( std::string &rHttpHeader, HtmlWriter &rHtmlWriter, std::string const& rFile)
   {
      return ::GetHtmlPageFileB( rHttpHeader, rHtmlWriter, "PNG_DIR", rFile.c_str(), "image/png", "Cache-Control: max-age=3600\r\n");
   }
};

// -----------------------------------------------------------------------------
// .I.
class HttpGif : public HttpCreator
{
public:
   virtual bool HExecute( HttpCmds const& rHttpCmds, HttpNameValues const& rNameValues, std::string const& rUserIP) { return true; }
   virtual bool GetHtmlPage( std::string &rHttpHeader, HtmlWriter &rHtmlWriter, std::string const& rFile)
   {
      return ::GetHtmlPageFileB( rHttpHeader, rHtmlWriter, "GIF_DIR", rFile.c_str(), "image/gif", "Cache-Control: max-age=3600\r\n");
   }
};

// -----------------------------------------------------------------------------
// ..C
class HttpCss : public HttpCreator
{
public:
   virtual bool HExecute( HttpCmds const& rHttpCmds, HttpNameValues const& rNameValues, std::string const& rUserIP) { return true; }
   virtual bool GetHtmlPage( std::string &rHttpHeader, HtmlWriter &rHtmlWriter, std::string const& rFile)
   {
      return ::GetHtmlPageFileA( rHttpHeader, rHtmlWriter, "CSS_DIR", rFile.c_str(), "text/css", "Cache-Control: max-age=3600\r\n");
   }
};

// -----------------------------------------------------------------------------
// ..C
class HttpJs : public HttpCreator
{
public:
   virtual bool HExecute( HttpCmds const& rHttpCmds, HttpNameValues const& rNameValues, std::string const& rUserIP) { return true; }
   virtual bool GetHtmlPage( std::string &rHttpHeader, HtmlWriter &rHtmlWriter, std::string const& rFile)
   {
      return ::GetHtmlPageFileA( rHttpHeader, rHtmlWriter, "JS_DIR", rFile.c_str(), "text/javascript", "Cache-Control: max-age=3600\r\n");
   }
};

// -----------------------------------------------------------------------------
// ..C
class HttpSwf : public HttpCreator
{
public:
   virtual bool HExecute( HttpCmds const& rHttpCmds, HttpNameValues const& rNameValues, std::string const& rUserIP) { return true; }
   virtual bool GetHtmlPage( std::string &rHttpHeader, HtmlWriter &rHtmlWriter, std::string const& rFile)
   {
      return ::GetHtmlPageFileB( rHttpHeader, rHtmlWriter, "SWF_DIR", rFile.c_str(), "application/x-shockwave-flash", "Cache-Control: max-age=3600\r\n");
   }
};

/*
Web Server MIME Type Considerations
http://www.utoronto.ca/web/htmldocs/book/book-3ed/appb/mimetype.html

switch ( fileExtension )
{
    case "pdf":  Response.ContentType = "application/pdf"; break; 
    case "swf":  Response.ContentType = "application/x-shockwave-flash"; break; 

    case "gif":  Response.ContentType = "image/gif"; break; 
    case "jpeg": Response.ContentType = "image/jpg"; break; 
    case "jpg":  Response.ContentType = "image/jpg"; break; 
    case "png":  Response.ContentType = "image/png"; break; 

    case "mp4":  Response.ContentType = "video/mp4"; break; 
    case "mpeg": Response.ContentType = "video/mpeg"; break; 
    case "mov":  Response.ContentType = "video/quicktime"; break; 
    case "wmv":
    case "avi":  Response.ContentType = "video/x-ms-wmv"; break; 

    //and so on          

    default: Response.ContentType = "application/octet-stream"; break; 
}

Case ".asf"         ContentType = "video/x-ms-asf"
Case ".avi"         ContentType = "video/avi"
Case ".doc"         ContentType = "application/msword"
Case ".zip"         ContentType = "application/zip"
Case ".xls"         ContentType = "application/vnd.ms-excel"
Case ".gif"         ContentType = "image/gif"
Case ".jpg", "jpeg" ContentType = "image/jpeg"
Case ".wav"         ContentType = "audio/wav"
Case ".mp3"         ContentType = "audio/mpeg3"
Case ".mpg", "mpeg" ContentType = "video/mpeg"
Case ".rtf"         ContentType = "application/rtf"
Case ".htm", "html" ContentType = "text/html"
Case ".asp"         ContentType = "text/asp"
Case Else           ContentType = "application/octet-stream"

.asf	video/x-ms-asf
.asx	video/x-ms-asf
.wma	audio/x-ms-wma
.wax	audio/x-ms-wax
.wmv	audio/x-ms-wmv
.wvx	video/x-ms-wvx
.wm	video/x-ms-wm
.wmx	video/x-ms-wmx
.wmz	application/x-ms-wmz
.wmd	application/x-ms-wmd
*/

#endif
