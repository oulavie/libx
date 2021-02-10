#include "HttpClient.h"

// -----------------------------------------------------------------------------
HttpClient::HttpClient( std::string const& rUsername)
   : mUsername ( "")
{
}

// -----------------------------------------------------------------------------
HttpClient::~HttpClient()
{
}

// -----------------------------------------------------------------------------
HttpClients::~HttpClients()
{
   tHttpClients::iterator iter = mHttpClients.begin();
   for( ; iter != mHttpClients.end(); ++iter)
      delete iter->second;
}

// -----------------------------------------------------------------------------
HttpClient* HttpClients::AddHttpClient( std::string const& rUsername)
{
   tHttpClients::const_iterator iter = mHttpClients.find( rUsername);
   if( iter == mHttpClients.end())
   {
      mHttpClients.insert( tHttpClients::value_type( rUsername, new HttpClient( rUsername)));
      iter = mHttpClients.find( rUsername);
   }
   return GetClient( rUsername);
}

// -----------------------------------------------------------------------------
HttpClient* HttpClients::GetClient( std::string const& rUsername)
{
   tHttpClients::const_iterator iter = mHttpClients.find( rUsername);
   if( iter == mHttpClients.end())
      return NULL;
   return iter->second;
}


