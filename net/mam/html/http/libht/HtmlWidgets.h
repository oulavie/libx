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
#ifndef _HtmlWidgets_H_
#define _HtmlWidgets_H_

#include "HtmlWriter.h"
#include <string>

/*
<style type="text/css"> p {font-family: fantasy, cursive, Serif;} </style>
<p>CSS font-family</p>

<style type="text/css"> p {font-size: x-large} larger smaller </style>
<p>CSS font-size.</p>

<style type="text/css"> p {font-stretch: ultra-expanded} </style>

<style type="text/css"> p {font-style: italic} normal oblic inherit </style>
<p>CSS font-style</p>

<style type="text/css"> p {font-variant: small-caps} normal inherit </style>

<style type="text/css"> p {font-weight: 900} normal bold bolder lighter inherit </style>

<style type="text/css"> p.wide-tracking { letter-spacing: 0.3em } </style>

<style type="text/css"> p { line-height: 180%; } </style>

<style type="text/css">
h2 { color:olive; }
p.question { color: rgb(30%,25%,60%) }
p.answer { color:#663300; }
</style>

<style type="text/css">
.colorBox {
	font: bold large cursive;
	border:1px solid #000000;
	background-color:#ff9900;
}

<style type="text/css">
div {
 text-align: right;
}
</style>

<style type="text/css">
a:hover { text-decoration:none } underline overline line-through blink inherit
</style>

<style type="text/css">
p { text-indent: 10% }
</style>

<style type="text/css">
h1.drop-shadow { text-shadow: 4px 4px 8px blue }
</style>

<style type="text/css">
h1.uppercase { text-transform: uppercase }
</style>
*/

// -----------------------------------------------------------------------------
class xTextout
{
   std::string mName;
   std::string mValue;
   std::string mSize;

   xTextout();
public:
   xTextout( const char* name)
      : mName  ( name)
      , mValue ( "''")
      , mSize ( "60")
   {}
   xTextout& SetName( const char* pValue)
   {
      mName = std::string( pValue);
      return *this;
   }
   xTextout& SetValue( const char* pValue)
   {
      mValue = std::string( pValue);
      return *this;
   }
   xTextout& SetSize( const char* pValue)
   {
      mSize = std::string( pValue);
      return *this;
   }
   const std::string& GetValue() const { return mValue; }

   xTextout& SetValue( std::string const& rName, std::string const& rValue)
   {
      if( strcmp( rName.c_str(), mName.c_str())==0)
         mValue = rValue;
      return *this;
   }
   void GetHtml( HtmlWriter &rHtmlWriter) const
   {
      std::string str = "<input type='text' name='";
      str += mName;
      str += "' value='";
      str += mValue;
      str += "' size='";
      str += mSize;
      str += "'></input>";
      rHtmlWriter.append( str);
   }
};

// -----------------------------------------------------------------------------
/*
<input type="text" id="startDate" name="startDateString" size="12" value = ""></input> 
<input type="image" src="calendar.gif" align="top" name="setStartDate" value = ""
       onclick="return showCalendar('startDate', 'yyyy-MM-dd');"></input> 
*/
class xHtmlDate
{
   std::string mName;
   std::string mSize;
   std::string mValue;
   std::string mAlign;

   xHtmlDate();
public:
   xHtmlDate( const char* name)
      : mName  ( name)
      , mSize  ( "12")
      , mValue ( "")
      , mAlign ( "top")
   {}
   void GetHtml( HtmlWriter &rHtmlWriter) const
   {
      std::string str = "<input type='text' id='";
      str += mName;
      str += "' name='";
      str += mName;
      str += "' size='";
      str += mSize;
      str += "' value='";
      str += mValue;
      str += "'></input>";

      str += "<input type='image' src='calendar.gif' align='top' name='set";
      str += mName;
      str += "' value='";
      str += mValue;
      str += "' onclick='return showCalendar('";
      str += mName;
      str += "', 'yyyy-MM-dd');'></input>";

      rHtmlWriter.append( str);
   }
};

// -----------------------------------------------------------------------------
class xButton
{
   void GetHtml( HtmlWriter &rHtmlWriter)
   {
   }
};

// -----------------------------------------------------------------------------
class xPassword
{
   void GetHtml( HtmlWriter &rHtmlWriter)
   {
   }
};

// -----------------------------------------------------------------------------
class xCheckbox
{
   void GetHtml( HtmlWriter &rHtmlWriter)
   {
   }
};

// -----------------------------------------------------------------------------
class xRadio
{
   void GetHtml( HtmlWriter &rHtmlWriter)
   {
   }
};

/*
La balise INPUT fait partie des éléments composant un formulaire (avec les tags BUTTON, SELECT et TEXTAREA) et permettant donc d'interagir avec l'utilisateur. 

Ses attributs spécifiques sont :
- type :	 Défini le type d'élément à afficher :
    text : défini une textbox (zone de saisie texte)
    password : défini une zone de texte de type password
    checkbox : défini une case à cocher
    radio : défini un bouton radio
    submit : bouton de validation du formulaire
    reset : permet d'effacer toute saisie effectuer dans le formulaire
    file : permet d'uploader des fichiers
    hidden : champ masqué permettant de transférer des valeurs dans le formulaire
    image : bouton de type image
    button : bouton classique
- value :	Valeur affichée, pour les boutons checkbox et boutons radio
- disabled :	Désactive le contrôle
- src :	Source de l'image pour le type image
- usemap :	Source de l'image MAP pour le type image
- maxlength :	Nombre maximum de caractères pour les zones de saisie

*/

#endif

// -*- modeline for vim, do not remove please :-) -*-
// vim: set expandtab tabstop=3 shiftwidth=3 autoindent smartindent:
