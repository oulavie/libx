#ifndef _HttpClient_H_
#define _HttpClient_H_

#include <string>
#include <map>

class HttpCreator;

// -----------------------------------------------------------------------------
class HttpClient
{
   HttpClient();
public:
   std::string   mUsername;
   HttpCreator  *mpHttpCreator;

   HttpClient( std::string const& rUsername);
   // default copy contructor
   // default assignment operator
   ~HttpClient();
};

// -----------------------------------------------------------------------------
class HttpClients
{
   typedef std::map< std::string, HttpClient*> tHttpClients;
   tHttpClients mHttpClients;

   HttpClients() {}
   HttpClients( HttpClients const&);
   HttpClients& operator=( HttpClients const&);
public:
   ~HttpClients();


   static HttpClients& Instance ()
   {
      static HttpClients sHttpClients;
      return sHttpClients;
   }

   HttpClient* AddHttpClient( std::string const& rUsername);
   HttpClient* GetClient( std::string const& rUsername);
};

#endif
