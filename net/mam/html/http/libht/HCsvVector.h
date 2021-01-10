/*
 * Copyright (c) 2011, Paulo Pereira <ppi@coding-raccoons.org>
 * Copyright (c) 2011, Philippe Boulerne <boolern666@gmail.com>
 * All rights reserved.
 * 
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
#ifndef _HCsvVector_H_
#define _HCsvVector_H_

#include "HttpHandies.h"
#include "libx/fatoi.h"
#include "libx/xStaticBuffer.h"
#include <stdio.h>
#include <string>
#include <vector>

// -----------------------------------------------------------------------------
template< typename T>
class HCsvVector : public std::vector< T>
{
   std::string mAction;
   std::string mName;
   long        mSelection;

   HCsvVector() {}
public:
   HCsvVector( std::string const &action,
               std::string const &name,
               bool addDefault  = true)
      : mAction    ( action)
      , mName      ( name)
      , mSelection ( 0)
   {
      if( addDefault)
      {
         T lDefault( "Default");
         push_back( lDefault);
      }
   }
   HCsvVector( const char* name, bool addDefault = true)
      : mName      ( name)
      , mSelection ( 0)
   {
      if( addDefault)
      {
         T lDefault( "Default");
         push_back( lDefault);
      }
   }
   void FileRead( FILE *pFILE)
   {
      if( pFILE == NULL)
         return;
      while( std::fgets( xSB, xSBL, pFILE) != 0)
      {
         xCsvReader lFormat( xSB, xSBL);
         T lT;
         lT.IteratorDD( lFormat);
         if( std::strncmp( lT.mName.GetStringValue().c_str(), "Default", 7) == 0)
            operator[](0) = lT; // une place a ete reservee lors de la construction
         else
            push_back( lT);
      }
   }
   void FileWrite( FILE *pFILE)
   {
      if( pFILE == NULL)
         return;
      std::string lBuffer;
      xCsvWriter lFormat( lBuffer);
      for( int i(0); i<(int)size(); ++i)
      {
         lFormat.Initialise();
         T &rT = operator[](i);
         rT.IteratorDD( lFormat);
         
         std::string str( lBuffer.c_str(), lBuffer.length());
         str += "\n";
         std::fwrite( str.c_str(), sizeof(char), str.length(), pFILE);
      }
   }

   long GetSelection()
   {
      return mSelection;
   }
   T& GetSelected()
   {
      return operator[](mSelection);
   }
   std::string const& GetName() const
   {
      return mName;
   }
   template< typename TWriter>
   void GetHtmlSelection( TWriter &w)
   {
      if( size()==0)
      {
         w.add( "%s is empty", mName.c_str());
         w.add( "<form action='%s' method='get' accept-charset='UTF-8'>", mAction.c_str());
         w.append( "<input type='submit' name='Action' value='Adding'></input>");
         w.append( "</form>");
         return;
      }

      w.add( "<form action='%s' method='get' accept-charset='UTF-8'>", mAction.c_str());

      w.append( "<select name='Selecting' style='width: 280px' onchange='this.form.submit()'>"); // --- Selecting
      for( int i(0); i<(int)size(); ++i)
      {
         w.add( "<option value='%d'", i);

         if( i==mSelection)
            w.add( " selected='selected'>%s</option>", operator[](i).mName.GetStringValue().c_str());
         else
            w.add( ">%s</option>", operator[](i).mName.GetStringValue().c_str());
      }
      w.append( "</select>");

      w.append( "<input type='submit' name='Action' value='Removing'></input>"); // --- Removing
      w.append( "<input type='submit' name='Action' value='Adding'></input>");   // --- Adding
      w.append( "</form>");
   }
   template< typename TWriter>
   void GetHtmlSelected( TWriter &w)
   {
      if( size()==0)
         return;

      w.append( "<fieldset><legend><b>Fields</b></legend>");
      w.add( "<form action='%s' method='get' accept-charset='UTF-8'>", mAction.c_str());
      w.append( "<center><input type='submit' name='Action' value='Submitting'></input></center>");

      w.append( "<table border='0' cellpadding='1' cellspacing='1' align='center' width='95%' style='white-space:nowrap'>");
      GetHtmlRaw< TWriter> lHtmlRaw( w);
      T &rT = operator[]( mSelection);
      rT.IteratorDD( lHtmlRaw);
      w.append( "</table>");

      w.append( "</form>");
      w.append( "</fieldset>");
   }
   bool SelectingT( long selection)
   {
      mSelection = selection;
      return true;
   }
   bool RemovingT( long selection)
   {
      erase( begin() + selection);
      mSelection = 0;
      return true;
   }
   bool SelectingT( HttpNameValue const& rHttpNameValue)
   {
      if( strcmp( mName.data(), rHttpNameValue.mName.data()) == 0)
      {
         std::string const& rValue = rHttpNameValue.mValue;
         mSelection = ::fatoi( rValue.data(), rValue.length());
         return true;
      }
      return false;
   }
   bool RemovingT( HttpNameValue const& rHttpNameValue)
   {
      if( strcmp( mName.data(), rHttpNameValue.mName.data()) == 0)
      {
         std::string const& rValue = rHttpNameValue.mValue;
         long idx_remove = ::fatoi( rValue.data(), rValue.length());
         erase( begin() + idx_remove);
         return true;
      }
   }
   bool AddingT()
   {
      T lT;
      lT.mName.set_data( "New", (size_t)3);
      push_back( lT);
      mSelection = size()-1;
      return true;
   }
   bool SubmittingT( HttpNameValues const& rHttpNameValues)
   {
      T &rT = operator[]( mSelection);
      rT.IteratorDD( const_cast<HttpNameValues&>( rHttpNameValues));
      return true;
   }
};

#endif

// -*- modeline for vim, do not remove please :-) -*-
// vim: set expandtab tabstop=3 shiftwidth=3 autoindent smartindent:
