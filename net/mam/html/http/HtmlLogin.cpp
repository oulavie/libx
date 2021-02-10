#include "HtmlLogin.h"
#include "LibHt/HttpHandies.h"
#include "LibX/xBuffer.h"
#include "LibHt/HtmlWriter.h"
#include "HttpClient.h"
#include <assert.h>
#include "../db/Users.h"

// -----------------------------------------------------------------------------
static void get_css( HtmlWriter &w)
{
   w.head().add( "<style type='text/css'>");
 //w.head().add( "fieldset { -moz-border-radius:10px; -webkit-border-radius:10px; border-radius:10px; border: 5px solid #FF0000; }");
   w.head().add( "fieldset { border-radius:10px; border: 3px solid #FF0000; }");
   w.head().add( "</style>");
}

// -----------------------------------------------------------------------------
void Login::GetHtmlPage( HtmlWriter &w)
{
   // http://validator.w3.org/
   // --- HTML 4.01 Transitional ---
   //w.doctype().append( "<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\"\n");
   //w.doctype().append( "   \"http://www.w3.org/TR/html4/loose.dtd\">\n");
   // --- XHTML 1.0 Transitional ---
   w.doctype().append( "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\"\n");
   w.doctype().append( "   \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">\n");
   w.html_attributes().append( "xmlns=\"http://www.w3.org/1999/xhtml\"");

   // --- <head> ---
   w.head().append( "<meta http-equiv=\"Content-type\" content=\"text/html;charset=UTF-8\" />");
   ::get_css( w);
   w.head().append( "<title>Logon</title>");

   // --- <body> ---
   w.append( "<center>");
   w.append( "<table border='0' cellpadding='0' cellspacing='0' width='400'><tr><td>");
   w.append( "<fieldset><legend><b>Login</b></legend>");

      // -------------------------------
      w.append( "<form action='LoginRequest' method='get'>");
      w.append( "<table border='0' cellpadding='2' cellspacing='1'>");

      w.append( "<tr>");
      w.append( "<td align='right'><label>Username<strong>*</strong></label></td>");
      w.append( "<td><input required type='text' name='Username' value='' size='30' placeholder='username' autocomplete='on'/></td>");
      w.append( "</tr>");

      w.append( "<tr>");
      w.append( "<td align='right'><label>Password<strong>*</strong></label></td>");
      w.append( "<td><input required type='password' name='Password' value='' size='30' placeholder='password'/></td>");
      w.append( "</tr>");

      w.append( "<tr>");
      w.append( "<td></td>");
      w.append( "<td align='left'><input type='submit' name='Action' size='15' value='Logon' /></td>");
      w.append( "</tr>");

      w.append( "</table>");
      w.append( "</form>");

      // -------------------------------
      w.append( "<form action='LoginRequest' method='get'>");
      w.append( "<table border='0' cellpadding='2' cellspacing='1'>");

      w.append( "<tr>");
      w.append( "<td>Not registered, please register!</td>");
      w.append( "<td align='left'><input type='submit' name='Action' size='50' value='Sign Up' /></td>");
      w.append( "</tr>");

      w.append( "</table>");
      w.append( "</form>");

   w.append( "</fieldset>");
   w.append( "</td></tr></table>");
   w.append( "</center>");
}

// -----------------------------------------------------------------------------
static const char* const LoginRequestAction = "/LoginRequest";

// -----------------------------------------------------------------------------
bool Login::IsLoginRequestAction( std::string const& rMsg)
{
   const char* pch = ::strstr( rMsg.data(), LoginRequestAction);
   return pch != NULL;
}

// -----------------------------------------------------------------------------
tUSR* Login::IsLoginRequestActionValid( HttpNameValues const& rHttpNameValues)
{
   // <FORM action=LoginRequest method=GET accept_char=UTF-8>
   // GET /LoginRequest?Username=dasdas&Password=dasdasdas&Action=Logon
   std::string username, password;
   if( !rHttpNameValues.GetValueOf( username, "Username") ||
       !rHttpNameValues.GetValueOf( password, "Password"))
       return NULL;
   return UsersHolder::Instance().CheckingLogin( username, password);
}

// -----------------------------------------------------------------------------
void SignUpRequest::GetHtmlPage( HtmlWriter &w)
{
   // http://validator.w3.org/
   // --- HTML 4.01 Transitional ---
   //w.append( "<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\"\n");
   //w.append( "   \"http://www.w3.org/TR/html4/loose.dtd\">\n");
   // --- XHTML 1.0 Transitional ---
   w.doctype().append( "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\"\n");
   w.doctype().append( "   \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">\n");
   w.html_attributes().append( "xmlns=\"http://www.w3.org/1999/xhtml\"");

   // --- <head> ---
   w.head().append( "<meta http-equiv=\"Content-type\" content=\"text/html;charset=UTF-8\" />");
   ::get_css( w);
   w.head().append( "<title>SignUp</title>");

   // --- <body> ---
   w.append( "<center>");
   w.append( "<table border='0' cellpadding='0' cellspacing='0' width='400'><tr><td>");
   w.append( "<fieldset><legend><b>Signing Up</b></legend>");

      w.append( "<form action='SignUpRequest' method='get'>");
      w.append( "<table border='0' cellpadding='2' cellspacing='1'>");

      w.append( "<tr>");
      w.append( "<td align='right'>Username</td>");
      w.append( "<td><input type='text' name='Username' value='' size='30' placeholder='username'/></td>");
      w.append( "</tr>");

      w.append( "<tr>");
      w.append( "<td align='right'>Email/Courriel</td>");
      w.append( "<td><input type='text' name='Email' value='' size='30' placeholder='valid email address'/></td>");
      w.append( "</tr>");

      w.append( "<tr>");
      w.append( "<td align='right' colspan='2'>(A password will be sent to your Email address)</td>");
      w.append( "</tr>");

      w.append( "<tr>");
      w.append( "<td></td>");
      w.append( "<td align='left'><input type='submit' name='Action' size='15' value='Sign Up' /></td>");
      w.append( "</tr>");

      w.append( "</table>");
      w.append( "</form>");

   w.append( "</fieldset>");
   w.append( "</td></tr></table>");
   w.append( "</center>");
}

// -----------------------------------------------------------------------------
bool SignUpRequest::IsSignUpRequestAction( std::string const& rMsg)
{
   char *action = "/SignUpRequest";
   const char *pch = strstr(rMsg.c_str(),action);
   return pch == rMsg.c_str();
}

// -----------------------------------------------------------------------------
void SignUpOk::GetHtmlPage( HtmlWriter &w, std::string const& rUsername, std::string const& rEmail)
{
   // http://validator.w3.org/
   // --- HTML 4.01 Transitional ---
   //w.doctype().append( "<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\"\n");
   //w.doctype().append( "   \"http://www.w3.org/TR/html4/loose.dtd\">\n");
   // --- XHTML 1.0 Transitional ---
   w.doctype().append( "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\"\n");
   w.doctype().append( "   \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">\n");
   w.html_attributes().append( "xmlns=\"http://www.w3.org/1999/xhtml\"");

   // --- <head> ---
   w.head().append( "<meta http-equiv=\"Content-type\" content=\"text/html;charset=UTF-8\" />");
   w.head().append( "<title>Logon</title>");

   // --- <body> ---
   w.append( "<center>");
   w.append( "<table border='0' cellpadding='0' cellspacing='0' width='400'><tr><td>");
   w.append( "<fieldset><legend><b>Thank you!</b></legend>");
   w.add( "Dear %s,<br /><br />", rUsername.c_str());
   w.add( "An email is being sent to: %s.<br /><br />", rEmail.c_str());
   w.append( "We thank you for your interest.<br /><br />");
   w.append( "The Quiz Tiger Company");
   w.append( "</fieldset>");
   w.append( "</td></tr></table>");
   w.append( "</center>");
}

// -----------------------------------------------------------------------------
void SignUpNok::GetHtmlPage( HtmlWriter &w, std::string const& rUsername, std::string const& rEmail)
{
   // http://validator.w3.org/
   // --- HTML 4.01 Transitional ---
   //w.doctype().append( "<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\"\n");
   //w.doctype().append( "   \"http://www.w3.org/TR/html4/loose.dtd\">\n");
   // --- XHTML 1.0 Transitional ---
   w.doctype().append( "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\"\n");
   w.doctype().append( "   \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">\n");
   w.html_attributes().append( "<html xmlns=\"http://www.w3.org/1999/xhtml\">\n");

   // --- <head> ---
   w.append( "<head>");
   w.append( "<meta http-equiv=\"Content-type\" content=\"text/html;charset=UTF-8\" />");
   w.append( "<title>Logon</title>");
   w.append( "</head>");

   // --- <body> ---
   w.append( "<body>");
   w.append( "<p>");
   w.append( "<br />");
   w.add( "Dear %s,<br />", rUsername.c_str());
   w.add( "The email [%s] is not valid. A valid email is requested.", rEmail.c_str());
   w.append( "</p>");
}

// -----------------------------------------------------------------------------
bool Logout::IsLogoutAction( std::string const& rMsg)
{
   char *action = "/Rcm?Action=Logout";
   const char *pch = strstr( rMsg.c_str(), action);
   return pch == rMsg.c_str();
}
