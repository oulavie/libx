/*
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
#ifndef _HttpNameValues_H_
#define _HttpNameValues_H_

#include <string>
#include <vector>

//-----------------------------------------------------------------------------
std::string HttpGetValueOfName( std::string const& rHttpMessage, const char* pName);
std::string HttpGetValueOfName( std::string const& rHttpMessage, std::string const& rName);

//-----------------------------------------------------------------------------
class HttpNameValue
{
   std::string name_;
   std::string value_;
public:
   HttpNameValue()
   {
   }
   HttpNameValue( const char* pName, const char* pValue)
      : name_  ( pName)
      , value_ ( pValue)
   {
   }

   std::string const& name()  const { return name_; }
   std::string const& value() const { return value_; }

   static void ParseHTTP( std::vector< HttpNameValue> &rVect, const char* pMsg, size_t Len)
   {
      const char ASSIGNMENT = '=';
      const char SEPARATOR  = '&';
      bool bAssignmentFound = false;

      const char* Debut = pMsg;
      const char* Tmp   = Debut;
      HttpNameValue lNameValue;

      while( Tmp < &pMsg[Len])
      {
         switch( *Tmp)
         {
         default:
            ++Tmp;
            continue;

         case ASSIGNMENT:
            if( bAssignmentFound == false)
            {
               lNameValue.name_ = std::string( Debut, Tmp - Debut);
               bAssignmentFound = true;
            }
            else
            {
               ++Tmp;
               continue;
            }
            break;

         case SEPARATOR:
            lNameValue.value_ = std::string( Debut, Tmp - Debut);
            rVect.push_back( lNameValue);
            lNameValue.name_ = "";
            lNameValue.value_ = "";
            bAssignmentFound = false;
            break;

         }
         ++Tmp;
         Debut = Tmp;
      }
      //--Tmp;
      lNameValue.value_ = std::string( Debut, Tmp - Debut);
      rVect.push_back( lNameValue);
   }
};

//-----------------------------------------------------------------------------
class HttpNameValues : public std::vector< HttpNameValue>
{
public:
   bool GetValueOf( std::string &rValue, const char* pName) const
   {
      const_iterator iter = begin();
      for( ; iter != end(); ++iter)
      {
         if( std::strcmp( iter->name().data(), pName) == 0)
         {
            rValue = iter->value();
            return true;
         }
      }
      return false;
   }

   template< typename T>
   void operator()( T &rT)
   {
      const_iterator iter = begin();
      for( ; iter != end(); ++iter)
      {
         if( std::strcmp( iter->name().data(), T::GetAttribut()) == 0)
         {
            std::string const& rValue = iter->name();
            rT.set_data( rValue.data(), rValue.length());
            return;
         }
      }
   }
};

#endif

// -*- modeline for vim, do not remove please :-) -*-
// vim: set expandtab tabstop=3 shiftwidth=3 autoindent smartindent:
