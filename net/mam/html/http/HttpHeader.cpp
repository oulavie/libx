#include "HttpHeader.h" 
#include "GatewayLib/BaseGateway.h" 
#include "HttpClient.h"

// -----------------------------------------------------------------------------
const char* GetHost()
{
   static BaseGateway *pEngine = BaseGateway::GetInstance();
   static Configurator const& rConfig = pEngine->GetConfigurator();
   static std::string sHttpHost = rConfig.GetValue( "HTTP_HOST");
   return sHttpHost.data();
}

// -----------------------------------------------------------------------------
const char* GetService()
{
   static BaseGateway *pEngine = BaseGateway::GetInstance();
   static Configurator const& rConfig = pEngine->GetConfigurator();
   static std::string sHttpService = rConfig.GetValue( "HTTP_SERVICE");
   return sHttpService.data();
}

// -----------------------------------------------------------------------------
const char* GetHttpHeader( std::string const& rUser)
{
   static char buffer[2000];
   HttpClient* pHttpClient = HttpClients::Instance().GetClient( rUser);
   //sprintf( buffer, "http://%s:%s/49=%s/34=%ld",
   //   ::GetHost(), ::GetService(), rUser.data(), pHttpClient->mUserSeqNum);
   sprintf( buffer, "http://%s:%s",
      ::GetHost(), ::GetService());
   return buffer;
}

// -----------------------------------------------------------------------------
const char* GetHttpShortHeader( std::string const& rUser)
{
   static char buffer[2000];
   HttpClient* pHttpClient = HttpClients::Instance().GetClient( rUser);
   //sprintf( buffer, "/49=%s/34=%ld",
   //   rUser.data(), pHttpClient->mUserSeqNum);
   sprintf( buffer, "");
   return buffer;
}
