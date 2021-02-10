#include "MapHttpServer.h"
#include "GatewayLib/MEventActiveTcpSocket.h"
#include "LibEvt/EvtHttp.h"
#include "Com/SHServerComMgr.h"
#include "Http/HttpCreator.h"
#include "Http/HtmlLogin.h"
#include "husers/HttpUsers.h"
#include "hdsk/HttpDsk.h"
#include "hshs/HttpShs.h"
#include "hftf/HttpFtf.h"
#include "HttpMain.h"
#include "Http/HttpClient.h"
#include "libht/HtmlWriter.h"
#include "libht/HttpCmds.h"
#include "libht/HttpNameValues.h"
#include "libht/HttpHandies.h"
#include "libx/xBuffer.h"
#include "libtype/xDateTime.h"

// -----------------------------------------------------------------------------
typedef std::map< std::string, HttpCreator*> tDocHttpMap;
static tDocHttpMap sDocumentToHttp;
static tDocHttpMap sHicToHttp;

static const size_t HTTP_CHUNK_SIZE = 8192; // 1024 2048 4096 8192 16384 32768 65536

// -----------------------------------------------------------------------------
class HttpIndex : public HttpCreator
{
public:
   virtual bool HExecute( HttpCmds const& rHttpCmds, HttpNameValues const& rNameValues, std::string const& rUserIP) { return true; }
   virtual bool GetHtmlPage( std::string &rHttpHeader, HtmlWriter &w, std::string const& rUserIP)
   {
      Login::Instance().GetHtmlPage( w);
      ::CreateHttpHeader( rHttpHeader, w.length(), rUserIP);
      return true;
   }
};

// -----------------------------------------------------------------------------
class HttpTime : public HttpCreator
{
public:
   virtual bool HExecute( HttpCmds const& rHttpCmds, HttpNameValues const& rNameValues, std::string const& rUserIP) { return true; }
   virtual bool GetHtmlPage( std::string &rHttpHeader, HtmlWriter &w, std::string const& rUserIP)
   {
      w.append( "<html>");
      w.append( "<head>");
      w.append( "<meta http-equiv='Refresh' content='10'>");
      //w.append( "<title>Time</title>");
      w.append( "</head>");
      w.append( "<body>");

      w.append( "<span style='font-size:18.0pt;font-family:Arial;color:#AAAAAA'>");
      xDateTime lDateTime;
      w.add( "<center><b>%.*s:%.*s:%.*s</b></center>",
         2, lDateTime.hour_data(),
         2, lDateTime.minute_data(),
         2, lDateTime.second_data());
      w.append( "</span>");

      w.append( "</body>");
      w.append( "</html>");

      ::CreateHttpHeader( rHttpHeader, w.length(), rUserIP);
      return true;
   }
};

static const size_t HTML_HEAD_SIZE = 2*1024*1024;
static const size_t HTML_BODY_SIZE = 10*1024*1024;

// -----------------------------------------------------------------------------
class MapHttpServer::Impl
{
   MapHttpServer     *parent_;
   EvtHttpServer     &mrEvtHttpServer;
   SHServerComMgr &mrSHServerComMgr;
   HtmlWriter         html_writer_;

public:
   Impl( MapHttpServer     *p,
         EvtHttpServer     &rEventServer,
         SHServerComMgr &rSHServerComMgr);
   ~Impl();

   void ConsumeTechnical( const TechnicalInfoData *pData, unsigned int Len);
   void Consume( const void *pMessage, unsigned int lLen, UserSequence& rUserInSequence);
   bool ConsumeHIC( HttpCmds const& rHttpCmds);
};

// -----------------------------------------------------------------------------
MapHttpServer::Impl::Impl( MapHttpServer     *p,
                           EvtHttpServer     &rEventServer, 
                           SHServerComMgr &rSHServerComMgr)
   : parent_         ( p)
   , mrEvtHttpServer ( rEventServer)
   , mrSHServerComMgr( rSHServerComMgr)
   , html_writer_    ( new xCharBuffer( HTML_HEAD_SIZE),
                       new xCharBuffer( HTML_BODY_SIZE),
                       new xCharBuffer( HTML_HEAD_SIZE+HTML_BODY_SIZE))
{
   sDocumentToHttp.insert( tDocHttpMap::value_type( std::string( "/time.html"),   new HttpTime()));
   sDocumentToHttp.insert( tDocHttpMap::value_type( std::string( "/favicon.ico"), new Http404()));

   sDocumentToHttp.insert( tDocHttpMap::value_type( HttpUsers::GetMenuItem(1),  HttpUsers::Instance()));
   sDocumentToHttp.insert( tDocHttpMap::value_type( HttpDsk::GetMenuItem(1),    HttpDsk::Instance()));
   sDocumentToHttp.insert( tDocHttpMap::value_type( HttpShs::GetMenuItem(1),    HttpShs::Instance()));
   sDocumentToHttp.insert( tDocHttpMap::value_type( HttpFtf::GetMenuItem(1),    HttpFtf::Instance()));
   //sDocumentToHttp.insert( tDocHttpMap::value_type( std::string( "/Rcm"),         new HttpRcm()));

   sHicToHttp.insert( tDocHttpMap::value_type( std::string( ".html"), new HttpHtml())); // H..
   sHicToHttp.insert( tDocHttpMap::value_type( std::string( ".jpg"),  new HttpJpg()));  // .I.
   sHicToHttp.insert( tDocHttpMap::value_type( std::string( ".png"),  new HttpPng()));  // .I.
   sHicToHttp.insert( tDocHttpMap::value_type( std::string( ".gif"),  new HttpGif()));  // .I.
   sHicToHttp.insert( tDocHttpMap::value_type( std::string( ".css"),  new HttpCss()));  // ..C
   sHicToHttp.insert( tDocHttpMap::value_type( std::string( ".js"),   new HttpJs()));   // ..C
   sHicToHttp.insert( tDocHttpMap::value_type( std::string( ".jsp"),  new HttpJs()));   // ..C
}

// -----------------------------------------------------------------------------
MapHttpServer::Impl::~Impl()
{
   tDocHttpMap::iterator iter;

   //for( iter = sDocumentToHttp.begin(); iter != sDocumentToHttp.end(); ++iter)
   //   delete iter->second; attention aux singletons!!!!
   sDocumentToHttp.erase( sDocumentToHttp.begin(), sDocumentToHttp.end());

   for( iter = sHicToHttp.begin(); iter != sHicToHttp.end(); ++iter)
   {
      delete iter->second;
      iter->second = NULL;
   }
   sHicToHttp.erase( sHicToHttp.begin(), sHicToHttp.end());
}

// -----------------------------------------------------------------------------
void MapHttpServer::Impl::ConsumeTechnical( const TechnicalInfoData *pData,
                                            unsigned int Len)
{
}

// -----------------------------------------------------------------------------
void MapHttpServer::Impl::Consume( const void   *pMessage,
                                   unsigned int  lLen,
                                   UserSequence &rUserInSequence)
{
   std::string lMessage( ( const char*)pMessage, lLen);

   size_t posHost = lMessage.find( "\nHost");
   std::string lMessageHeader = lMessage.substr( 0, (posHost<256) ? posHost : 256);
   while( lMessageHeader[ lMessageHeader.length()-1] == '\r') // 13 = CR
      lMessageHeader = lMessageHeader.substr( 0, lMessageHeader.length()-1);
   if( posHost >= 256)
      lMessageHeader += " ...";
   ::verbose( __FILE__, __LINE__, V_WARNING, lMessageHeader);

   //std::string lMessageFooter = lMessage.substr( posHost+1);
   //::verbose( __FILE__, __LINE__, V_WARNING, lMessageFooter);

   std::string     lHttpHeader;
   std::string     lHttpRequest;
   std::string     lHttpPath;
   std::string     lHttpQuery;
   std::string     lUserIP;
   std::string     lSeqNum;
   HttpCmds        lHttpCmds;
   HttpNameValues  lHttpNameValues;
   ::extract_document( lHttpRequest, lHttpPath, lHttpQuery, lUserIP, lSeqNum, lHttpCmds, lHttpNameValues, lMessage);

   char HttpId[20];
   sprintf( HttpId, "HTTP%X", (unsigned int)parent_->GetCurrentIdentifier());
   //for( size_t i(0); i < lHttpCmds.size(); ++i)
   //   ::verbose( __FILE__, __LINE__, V_INFO, "   doc[%ld]=%s", i, lHttpCmds[i].data());
   //for( size_t i(0); i < lHttpNameValues.size(); ++i)
   //   ::verbose( __FILE__, __LINE__, V_INFO, "   name[%ld]=%s value=%s", i, lHttpNameValues[i].name().data(), lHttpNameValues[i].value().data());

   html_writer_.reset();

   // extraction du client:
   if( lUserIP.length() != 0)
   {
      std::string lUserIPID = lUserIP + std::string( HttpId);
      mrSHServerComMgr.AddLC3onPC1( lUserIPID, parent_->GetCurrentIdentifier());
      mrSHServerComMgr.SetAtEnd(    lUserIPID, parent_->GetCurrentIdentifier());
      mrSHServerComMgr.Authorize(   lUserIPID, true);

      // les pages/actions:
      {
         tDocHttpMap::iterator iter = sDocumentToHttp.find( lHttpCmds[0]);
         if( iter != sDocumentToHttp.end())
         {
            iter->second->HExecute( lHttpCmds, lHttpNameValues, lUserIP);
            iter->second->GetHtmlPage( lHttpHeader, html_writer_, lUserIP);
            mrSHServerComMgr.push_back2_business( lUserIPID, lHttpHeader.data(), lHttpHeader.length(), 
                                                  html_writer_.data(), html_writer_.length(), eHTTP, HTTP_CHUNK_SIZE);
            return;
         }
      }
   }

   // HIC, les pieces jointes:
   std::string fullCmds;
   for( size_t i(0); i < lHttpCmds.size(); ++i)
      fullCmds += lHttpCmds[i];
      
   std::string::size_type posDot = fullCmds.find_last_of( ".");
   if( posDot != std::string::npos)
   {
      std::string extension = fullCmds.substr( posDot, std::string::npos);
      tDocHttpMap::iterator iter = sHicToHttp.find( extension);
      if( iter != sHicToHttp.end())
      {
         iter->second->GetHtmlPage( lHttpHeader, html_writer_, fullCmds);
         mrSHServerComMgr.push_back_technic( parent_->GetCurrentIdentifier(), lHttpHeader.data(), lHttpHeader.length());
         mrSHServerComMgr.push_back_technic( parent_->GetCurrentIdentifier(), html_writer_.data(), html_writer_.length());
         return;
      }
   }

   Http404().GetHtmlPage( lHttpHeader, html_writer_, "");
   lHttpHeader += html_writer_.data();
   mrSHServerComMgr.push_back_technic( parent_->GetCurrentIdentifier(), lHttpHeader.data(), lHttpHeader.length());

   return;
}

/*
// -----------------------------------------------------------------------------
void MapHttpServer::Impl::Consume( const void   *pMessage,
                                   unsigned int  lLen,
                                   UserSequence &rUserInSequence)
{
   std::string lHttpHeader;
   std::string lMessage( ( const char*)pMessage, lLen);

   std::string     lHttpRequest;
   std::string     lHttpPath;
   std::string     lHttpQuery;
   std::string     lUserIP;
   std::string     lSeqNum;
   HttpCmds        lHttpCmds;
   HttpNameValues  lHttpNameValues;
   ::extract_document( lHttpRequest, lHttpPath, lHttpQuery, lUserIP, lSeqNum, lHttpCmds, lHttpNameValues, lMessage);

   char HttpId[20];
   sprintf( HttpId, "HTTP%X", (unsigned int)parent_->GetCurrentIdentifier());
   for( size_t i(0); i < lHttpCmds.size(); ++i)
      ::verbose( __FILE__, __LINE__, V_INFO, "   doc[%ld]=%s", i, lHttpCmds[i].data());
   for( size_t i(0); i < lHttpNameValues.size(); ++i)
      ::verbose( __FILE__, __LINE__, V_INFO, "   name[%ld]=%s value=%s", i, lHttpNameValues[i].mName.data(), lHttpNameValues[i].mValue.data());

   html_writer_.reset();

   // extraction du client:
   if( lUserIP.length() != 0)
   {
      std::string lUserIPID = lUserIP + std::string( HttpId);
      mrSHServerComMgr.AddLC3onPC1( lUserIPID, parent_->GetCurrentIdentifier());
      mrSHServerComMgr.SetAtEnd(    lUserIPID, parent_->GetCurrentIdentifier());
      mrSHServerComMgr.Authorize(   lUserIPID, true);

      // les pages/actions:
      {
         tDocHttpMap::iterator iter = sDocumentToHttp.find( lHttpCmds[0]);
         if( iter != sDocumentToHttp.end())
         {
            iter->second->HExecute( lHttpCmds, lHttpNameValues, lUserIP);
            iter->second->GetHtmlPage( lHttpHeader, html_writer_, lUserIP);
            mrSHServerComMgr.push_back2_business( lUserIPID, lHttpHeader.data(), lHttpHeader.length(), 
                                                    html_writer_.data(), html_writer_.length(), eHTTP, HTTP_CHUNK_SIZE);
            return;
         }
      }
   }
   else
   {
      // presenter la fenetre de login:
      if( lHttpCmds[0] == "/")
      {
         HttpIndex().GetHtmlPage( lHttpHeader, html_writer_, lUserIP);
         mrSHServerComMgr.push_back_technic( parent_->GetCurrentIdentifier(), lHttpHeader.data(), lHttpHeader.length(), HTTP_CHUNK_SIZE);
         mrSHServerComMgr.push_back_technic( parent_->GetCurrentIdentifier(), html_writer_.data(), html_writer_.length(), HTTP_CHUNK_SIZE);
         return;
      }
      // reponse a la fenetre de login:
      else if( Login::Instance().IsLoginRequestAction( lHttpCmds[0]))
      {
         tUSR* ok = Login::Instance().IsLoginRequestActionValid( lHttpNameValues);
         if( ok)
         {
            std::string username;
            lHttpNameValues.GetValueOf( username, "Username");
            std::string lUserIP = username;
            HttpClients::Instance().AddHttpClient( lUserIP);

            //TRACE_PRINT( 1,( TRACE_OUTPUT << "Login of: " << lUserIP.c_str() <<" ( "<< __FILE__<<":"<<__LINE__<<")"));
            
            HttpTexts::Instance()->HExecute( lHttpCmds, lHttpNameValues, lUserIP);
            HttpTexts::Instance()->GetHtmlPage( lHttpHeader, html_writer_, lUserIP);

            //HttpMain2::Instance()->GetHtmlPage( lHttpHeader, html_writer_, lUserIP);

            std::string lUserIPID = lUserIP + std::string( HttpId);
            mrSHServerComMgr.AddLC3onPC1( lUserIPID, parent_->GetCurrentIdentifier());
            mrSHServerComMgr.Authorize(   lUserIPID, true);
            mrSHServerComMgr.SetAtEnd(    lUserIPID, parent_->GetCurrentIdentifier());
            mrSHServerComMgr.push_back2_business( lUserIPID, lHttpHeader.data(), lHttpHeader.length(), 
                                                    html_writer_.data(), html_writer_.length(), eHTTP, HTTP_CHUNK_SIZE);
            return;
         }
         // presenter la fenetre de signup:
         else if( lHttpCmds[0].find( "Action=Sign Up") != std::string::npos)
         {
            SignUpRequest::Instance().GetHtmlPage( html_writer_);
            ::CreateHttpHeader( lHttpHeader, html_writer_.length(), lUserIP);
            mrSHServerComMgr.push_back_technic( parent_->GetCurrentIdentifier(), lHttpHeader.data(), lHttpHeader.length());
            mrSHServerComMgr.push_back_technic( parent_->GetCurrentIdentifier(), html_writer_.data(), html_writer_.length());
            return;
         }
         else
         {
            HttpIndex().GetHtmlPage( lHttpHeader, html_writer_, lUserIP);
            mrSHServerComMgr.push_back_technic( parent_->GetCurrentIdentifier(), lHttpHeader.data(), lHttpHeader.length());
            mrSHServerComMgr.push_back_technic( parent_->GetCurrentIdentifier(), html_writer_.data(), html_writer_.length());
            return;
         }
      }
      // reponse a la fenetre de signup:
      else if( SignUpRequest::Instance().IsSignUpRequestAction( lHttpCmds[0]))
      {
         std::string lUsername = lHttpNameValues[0].mValue;
         std::string lEmail    = lHttpNameValues[1].mValue;
         std::string lLevel    = lHttpNameValues[2].mValue;
         if( strcmp( lHttpNameValues[3].mValue.data(), "Sign Up") == 0)
         {
            SignUpOk::Instance().GetHtmlPage( html_writer_, lUsername, lEmail);
            ::CreateHttpHeader( lHttpHeader, html_writer_.length(), lUserIP);
            mrSHServerComMgr.push_back_technic( parent_->GetCurrentIdentifier(), lHttpHeader.data(), lHttpHeader.length());
            mrSHServerComMgr.push_back_technic( parent_->GetCurrentIdentifier(), html_writer_.data(), html_writer_.length());
            //aussi envoyer un email....
            return;
         }
         else
         {
            SignUpNok::Instance().GetHtmlPage( html_writer_,lUsername,lEmail);
            ::CreateHttpHeader( lHttpHeader, html_writer_.length(), lUserIP);
            mrSHServerComMgr.push_back_technic( parent_->GetCurrentIdentifier(), lHttpHeader.data(), lHttpHeader.length());
            mrSHServerComMgr.push_back_technic( parent_->GetCurrentIdentifier(), html_writer_.data(), html_writer_.length());
            return;
         }
      }
   }

   // HIC, les pieces jointes:
   std::string fullCmds;
   for( int i(0); i < lHttpCmds.size(); ++i)
      fullCmds += lHttpCmds[i];
      
   std::string::size_type posDot = fullCmds.find_last_of( ".");
   if( posDot != std::string::npos)
   {
      std::string extension = fullCmds.substr( posDot, std::string::npos);
      tDocHttpMap::iterator iter = sHicToHttp.find( extension);
      if( iter != sHicToHttp.end())
      {
         iter->second->GetHtmlPage( lHttpHeader, html_writer_, fullCmds);
         mrSHServerComMgr.push_back_technic( parent_->GetCurrentIdentifier(), lHttpHeader.data(), lHttpHeader.length());
         mrSHServerComMgr.push_back_technic( parent_->GetCurrentIdentifier(), html_writer_.data(), html_writer_.length());
         return;
      }
   }

   Http404().GetHtmlPage( lHttpHeader, html_writer_, "");
   lHttpHeader += html_writer_.data();
   mrSHServerComMgr.push_back_technic( parent_->GetCurrentIdentifier(), lHttpHeader.data(), lHttpHeader.length());

   return;
}
*/

/*
// -----------------------------------------------------------------------------
class http_tableau : public http_html_page
{
public:
   void HConsume( std::string &rHttpString, std::string const& rUserIP)
   {
      std::vector<std::string> lHtmlVect;

      lHtmlVect.append( "<HTML>");
      lHtmlVect.append( "<html xmlns='http://www.w3.org/1999/xhtml' xml:lang='en-US'>");
      lHtmlVect.append( "<head>");
      lHtmlVect.append( "<link rel=icon href=PhyENS.jpg type=image/jpg />");
      lHtmlVect.append( "<link rel=stylesheet href=style-paper.css type=text/css />");
 
      lHtmlVect.append( "<title>TITLE</title>");
      lHtmlVect.append( "<link rel=alternate type=application/atom+xml");
         lHtmlVect.append( "href=atom-log title='Atom feed for TITI' />"); 
      lHtmlVect.append( "<link rel=alternate type=application/rss+xml");
         lHtmlVect.append( "href=rss-log title='RSS feed for TOTO' />");
      lHtmlVect.append( "</head>");
      lHtmlVect.append( "<body>");
      // -------------------------------
      lHtmlVect.append( "<div class=container>");
         // - 1 ------------------------
         lHtmlVect.append( "<div class=menu>");
            lHtmlVect.append( "<div class=logo>"); 
               lHtmlVect.append( "<img src=PhyENS.jpg />");
            lHtmlVect.append( "</div>");
            lHtmlVect.append( "<ul>");
            lHtmlVect.append( "<li class=active>log</li>");
            lHtmlVect.append( "<li><a href=/graph>graph</a></li>");
            lHtmlVect.append( "<li><a href=/tags>tags</a></li>");
            lHtmlVect.append( "<li><a href=/branches>branches</a></li>");
            lHtmlVect.append( "</ul>");
            lHtmlVect.append( "<ul>");
            lHtmlVect.append( "<li><a href=/rev>changeset</a></li>");
            lHtmlVect.append( "<li><a href=/file>browse</a></li>");
            lHtmlVect.append( "</ul>");
            lHtmlVect.append( "<ul>");
            lHtmlVect.append( "</ul>");
         lHtmlVect.append( "</div>"); // - 1 -

         // - 2 ------------------------
         lHtmlVect.append( "<div class=main>");
            lHtmlVect.append( "<h2><a href='/'>Bienvenue </a></h2>");
            lHtmlVect.append( "<h3>log</h3>");
            lHtmlVect.append( "<form class=search action='/log'>");
               lHtmlVect.append( "<p><input name=rev id=search1 type=text size=30 /></p>");
               lHtmlVect.append( "<div id=hint>find changesets by author, revision, files, or words in the commit message</div>"); // tooltip
            lHtmlVect.append( "</form");

            lHtmlVect.append( "<div class=navigate>");
               lHtmlVect.append( "<a href='/shortlog/2?revcount=30'>less</a>");
               lHtmlVect.append( "<a href='/shortlog/2?revcount=120'>more</a>");
               lHtmlVect.append( "| rev 2: <a href='/shortlog/05cde3da0a6c'>( 0)</a> <a href='/shortlog/tip'>tip</a>");
            lHtmlVect.append( "</div>");

            lHtmlVect.append( "<table class=bigtable>");
               lHtmlVect.append( "<tr>");
                  lHtmlVect.append( "<th class=age>age</th>");
                  lHtmlVect.append( "<th class=author>author</th>");
                  lHtmlVect.append( "<th class=description>description</th>");
               lHtmlVect.append( "</tr>");
               lHtmlVect.append( "<tr class=parity0>");
                  lHtmlVect.append( "<td class=age>2 days ago</td>");
                  lHtmlVect.append( "<td class=author>boolern</td>");
                  lHtmlVect.append( "<td class=description><a href='/rev/351f61faf418'>Ajout des niveaux pour le signup</a><span class=branchhead>default</span> <span class=tag>tip</span> </td>");
               lHtmlVect.append( "</tr>");
               lHtmlVect.append( "<tr class=parity1>");
                  lHtmlVect.append( "<td class=age>2 days ago</td>");
                  lHtmlVect.append( "<td class=author>boolern</td>");
                  lHtmlVect.append( "<td class=description><a href='/rev/d6adc1be06d6'>Ajout de la barre de progression</a></td>");
               lHtmlVect.append( "</tr>");
               lHtmlVect.append( "<tr class=parity0>");
                  lHtmlVect.append( "<td class=age>3 days ago</td>");
                  lHtmlVect.append( "<td class=author>boolern</td>");
                  lHtmlVect.append( "<td class=description><a href=/rev/05cde3da0a6c>Initial commit</a></td>");
               lHtmlVect.append( "</tr>");
            lHtmlVect.append( "</table>");
 
            lHtmlVect.append( "<div class=navigate>");
               lHtmlVect.append( "<a href='/shortlog/2?revcount=30'>less</a>");
               lHtmlVect.append( "<a href='/shortlog/2?revcount=120'>more</a>");
               lHtmlVect.append( "| rev 2: <a href='/shortlog/05cde3da0a6c'>( 0)</a> <a href='/shortlog/tip'>tip</a>");
            lHtmlVect.append( "</div>");

         lHtmlVect.append( "</div>"); // - 2 -

      lHtmlVect.append( "</div>");
      lHtmlVect.append( "</body>");
      lHtmlVect.append( "</html>");

      create_http_page_from_html_page( rHttpString, lHtmlVect, rUserIP);
   }
};
*/

// -----------------------------------------------------------------------------
MapHttpServer::MapHttpServer( EvtHttpServer     &rEventServer,
                              SHServerComMgr &rSHServerComMgr)
   : pimpl_( NULL)
{
   pimpl_ = new MapHttpServer::Impl( this, rEventServer, rSHServerComMgr);
}

// -----------------------------------------------------------------------------
MapHttpServer::~MapHttpServer()
{
   delete pimpl_;
}

// -----------------------------------------------------------------------------
void MapHttpServer::ExecuteTechnical( const TechnicalInfoData *pData,
                                      unsigned int             Len)
{
   pimpl_->ConsumeTechnical( pData, Len);
}

// -----------------------------------------------------------------------------
void MapHttpServer::Execute( const void*   pMessage,
                             unsigned int  lLen,
                             UserSequence& rUserInSequence)
{
   pimpl_->Consume( pMessage, lLen, rUserInSequence);
}
