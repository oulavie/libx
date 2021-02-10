#ifndef _HtmlLogin_H_
#define _HtmlLogin_H_

#include <string>

class HttpNameValues;
class HtmlWriter;
class tUSR;

// -----------------------------------------------------------------------------
class Login
{
   Login() {}
public:
   static Login& Instance ()
   {
      static Login sLogin;
      return sLogin;
   }
   bool IsLoginRequestAction( std::string const& rMsg);
   tUSR* IsLoginRequestActionValid( HttpNameValues const& rHttpNameValues);
   void GetHtmlPage( HtmlWriter&);
};

// -----------------------------------------------------------------------------
class SignUpRequest
{
   SignUpRequest() {}
public:
   static SignUpRequest& Instance ()
   {
      static SignUpRequest sSignUpRequest;
      return sSignUpRequest;
   }
   bool IsSignUpRequestAction( std::string const& rMsg);
   void GetHtmlPage( HtmlWriter&);
};

// -----------------------------------------------------------------------------
class SignUpOk
{
   SignUpOk() {}
public:
   static SignUpOk& Instance ()
   {
      static SignUpOk sSignUpOk;
      return sSignUpOk;
   }
   void GetHtmlPage( HtmlWriter &rHtmlWriter, std::string const& rUsername, std::string const& rEmail);
};

// -----------------------------------------------------------------------------
class SignUpNok
{
   SignUpNok() {}
public:
   static SignUpNok& Instance ()
   {
      static SignUpNok sSignUpNok;
      return sSignUpNok;
   }
   void GetHtmlPage( HtmlWriter &rHtmlWriter, std::string const& rUsername, std::string const& rEmail);
};

// -----------------------------------------------------------------------------
class Logout
{
   Logout() {}
public:
   static Logout& Instance ()
   {
      static Logout sLogout;
      return sLogout;
   }
   bool IsLogoutAction( std::string const& rMsg);
};

#endif




