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
#include "DskProxy.h"
#include "DskHtml.h"

#include "libx/xReaderWriter.h"
#include "libx/xPathOperations.h"
#include "libx/xStringOperations.h"
#include "libx/xBuffer.h"
#include "libx/xCalendar.h"
#include "libx/Verbose.h"

#include "libht/HtmlWriter.h"
#include "libht/HttpHandies.h"

#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <fstream>
#include <iomanip>

/*
<details>
<summary>Maladies</summary>
   <details>
   <summary>Atrhrose</summary>
    Tres malade
   </details>
   <details>
   <summary>Alzeihmer</summary>
  Mouns malade
   </details>
</details>

<p>My favorite color is <del>blue</del> <ins>red</ins>!</p>

Word Break
XML<wbr>Http<wbr>Request 
*/

//------------------------------------------------------------------------------
DskProxy::DskProxy() : state_        ( eSttDisplayDataList)
                     , data_filter_  ( eFltNotes)
                     , data_display_ ( eDspList)
{
   calendar_.current_date_.now();
   calendar_.col_ = 1;
   calendar_.row_ = 7;
}

//------------------------------------------------------------------------------
void DskProxy::initialize()
{
   char eoc = directory_[ directory_.length()-1];
   if( eoc == 34)
      directory_ = directory_.substr( 0, directory_.length()-1);
}

//------------------------------------------------------------------------------
void DskProxy::read_ppi()
{
   ::verbose( __FILE__, __LINE__, V_INFO, "Reading PPI (start)");
   std::vector< std::string> lFiles;
   ::GetFilesInDirectory( lFiles, directory_, "*.ppi");
   size_t sz = lFiles.size();
   int percentage = 0;
   for( size_t i(0); i < sz; ++i)
   {
      percentage = (i * 100) / sz;
      std::string lFile( lFiles[i]);
      printf("\r   percentage to finish = %ld   (%s)       ", percentage, lFile.data());
      ::PathFileMerge( lFile, directory_);
      data_.read_insert_ppi( lFile);
   }
   printf("\r                                    \r");
   ::verbose( __FILE__, __LINE__, V_INFO, "Reading PPI (end)");
}

//------------------------------------------------------------------------------
void DskProxy::save_as_new_text( dsk::DatumExt const& rDatumExt)
{
   dsk::DatumExt &lrDatumExt = const_cast< dsk::DatumExt&>( rDatumExt);
   lrDatumExt.set_t8().set_data( "1", 1);
   lrDatumExt.set_t60().now();
   lrDatumExt.set_t11().assign( GetNextClientID());
   lrDatumExt.set_t41().assign( 0UL);
   lrDatumExt.set_t20().assign( "0", 1); // new

   std::string lFile( file_);
   ::PathFileMerge( lFile, directory_);
   lFile += ".";
   lFile += lrDatumExt.t11().GetString();
   lFile += ".ppi";
   lrDatumExt.set_file() = lFile;

   data_.write_insert_ppi( lFile, lrDatumExt);
}

//------------------------------------------------------------------------------
void DskProxy::save_as_modified_text( dsk::DatumExt const& rDatumExt)
{
   size_t lt11 = data_.selected_t11();

   dsk::DatumExt lDatumExt;
   lDatumExt.set_t11().assign( lt11);
   const dsk::DatumExt* pDatumExtOrig =
      data_.rack_datumext().rack_read( lDatumExt, data_.key11());
   std::string const& lrFile = pDatumExtOrig->file();

   dsk::DatumExt &lrDatumExt = const_cast< dsk::DatumExt&>( rDatumExt);
   lrDatumExt.set_data_index( pDatumExtOrig->get_data_index());
   lrDatumExt.set_t8().set_data( "1", 1);
   lrDatumExt.set_t60().now();
   lrDatumExt.set_t11().assign( lt11);
   lrDatumExt.set_t41().assign( 0UL);
   lrDatumExt.set_t20().assign( "2", 1); // modified
   lrDatumExt.set_file() = lrFile;
   lrDatumExt.set_data_type() = dsk::get_msg_datatype( lrDatumExt.t35());

   if( data_.rack_datumext().rack_update( lrDatumExt))
   {
      data_.insert_tags( lrDatumExt);
      ::remove( lrFile.data());
      lrDatumExt.write_ppi( lrFile);
   }
}

//------------------------------------------------------------------------------
void DskProxy::select_datum( long id)
{
   data_.set_selected_t11() = id;
}

//------------------------------------------------------------------------------
void DskProxy::select_date( const char* date)
{
   calendar_.current_date_.set_data( date, ::strlen(date));
}

//------------------------------------------------------------------------------
void DskProxy::request_delete( std::string const& filename)
{
   data_.request_delete( filename);
}

/*
<!DOCTYPE html>
<html>
<body>

<script>
document.write("Total Height: " + screen.height);
document.write("Available Width: " + screen.availWidth);
document.write("Total Height: " + screen.height);
document.write("Total Width: " + screen.width);
document.write("Color Depth: " + screen.colorDepth);
document.write("Color resolution: " + screen.pixelDepth);
</script>

</body>
</html>
*/
//------------------------------------------------------------------------------
void DskProxy::get_html_body( HtmlWriter &w,
                              std::string const& rUserIP)
{
   // http://www.w3.org/TR/XMLHttpRequest/#text-response-entity-body
   // Return the text response entity body.
   if( !XMLHttpRequest_responseText_.empty())
   {
      w.append( XMLHttpRequest_responseText_);
      XMLHttpRequest_responseText_.clear();
      return;
   }

 //w.head().add( "<link rel='stylesheet' href=\"http://%s/49=%s/style-paper.css\" type='text/css' />",
 //     hostname_.data(),
 //     rUserIP.data());
   w.head().add( "<link rel='stylesheet' href=\"/style-paper.css\" type='text/css' />");
   w.head().add( "<link rel='stylesheet' href=\"/chosen.css\" type='text/css' />");

/*
   w.add( "<td><a href='http://%s/49=%s/%s/%s?11=%d'>%s</a></td>",
            hostname_.data(),
            pUserIP,
            pRepo,
            pSubRepo,
            pText->t11().GetULong(),
            lTitle.c_str());
*/

   switch( state_)
   {
   case eSttDisplayDataList:
      {
         w.append( "<table><tr>");

         w.append( "<td>");
            ::get_datafilter_form( w, data_filter_);
         w.append( "</td>");

         w.append( "<td>");
            ::get_datadisplay_form( w, data_display_);
         w.append( "</td>");

         w.append( "<td>");
            w.append( "<form action='dsk' method='get' accept-charset='UTF-8'>");         
            w.append( "<input type='submit' name='Request' value='Add'/>");
            w.append( "</form>");
         w.append( "</td>");

         w.append( "<td>");
            w.append( "<form action='dsk' method='get' accept-charset='UTF-8'>");         
            w.add( "<input type='text' name='Keywords' value='%s' size='60' placeholder='keywords' autocomplete='on'/>",
               keywords_.c_str());
            w.append( "</form>");
         w.append( "</td>");

         w.append( "</tr></table>");

         switch( data_display_)
         {
         case eDspList:
          //w.append( "<style type='text/css' ><!-- div.scroll_report{height:800px;overflow:auto;border:1px solid #FFF;background-color:#FFF;padding:8px;}--></style>");
            w.head().append(  "<style type='text/css' ><!-- div.scroll_report{height:800px;overflow:auto;border:1px solid #FFF;background-color:#FFF;padding:8px;}--></style>");

            w.append( "<form action='dsk' method='get' accept-charset='UTF-8'>");         
            w.append( "<input type='submit' name='Request' value='Delete'/>");

            w.append( "<div class='scroll_report'>");

            data_.get_data_table( w, hostname_.c_str(), rUserIP.c_str(), 
                                  "dsk", "SelectDatum",
                                  (dsk::DatumExt::DataType)data_filter_);

            w.append( "</form>");

            w.append( "</div>"); // scroll_report
            break;

         default:
         case eDspAgenda:
            {
               w.append( "<table>");
               w.append( "<tr>");
               w.append( "<td>"); // left column
               {
                  w.append( "<table>");

                  w.append( "<tr><td>");
                     ::get_calendar_row_select( w, "dsk", "CalendarRow", calendar_.row_);
                     ::get_calendar_col_select( w, "dsk", "CalendarCol", calendar_.col_);
                  w.append( "</td></tr>");

                  w.append( "<tr><td>");
                     ::get_agenda_table( w, hostname_.c_str(), rUserIP.c_str(), 
                                         "dsk", "SelectDate",
                                         calendar_.current_date_);
                  w.append( "</td></tr>");

                  w.append( "</table>");
               }
               w.append( "</td>");
               w.append( "<td>"); // right column
               {
                  xDateTime lDateTime( calendar_.current_date_);

                  w.head().append(  "<style type='text/css' ><!-- div.scroll_report{height:800px;overflow:auto;border:1px solid #FFF;background-color:#FFF;padding:8px;}--></style>");
                  w.append( "<div class='scroll_report'>");

                  w.append( "<table>");
                  for( size_t i(0); i < calendar_.row_; ++i)
                  {
                     w.append( "<tr>");
                     for( size_t j(0); j < calendar_.col_; ++j)
                     {
                        w.append( "<td>");
                        {
                           w.append( "<table class='bigtable'>");
                           int dow = ::day_of_week2( lDateTime.year().ulong(),
                                                     lDateTime.month().ulong()-1,
                                                     lDateTime.day().ulong()-1);

                           w.add( "<td>%s | %s</td>", lDateTime.date2_str().data(),
                                                      day_of_week_str[0][dow]);

                           data_.get_data_table_at( w, hostname_.c_str(), rUserIP.c_str(), 
                                                    "dsk", "SelectDatum",
                                                    (dsk::DatumExt::DataType)data_filter_,
                                                    &(lDateTime));
                           w.append( "</table>");
                        }
                        w.append( "</td>");
                        lDateTime.add_day(1);
                     }
                     w.append( "</tr>");
                  }
                  w.append( "</table>");

                  w.append( "</div>"); // scroll_report
               }
               w.append( "</td>");
               w.append( "</tr>");
               w.append( "</table>");
            }
            break;
         }
      }
      break;

   case eSttDisplayDatum:
      {
         w.append( "<form action='dsk' method='get' accept-charset='UTF-8'>");
         w.append( "<input type='submit' name='Request' value='Modify'/>");
         w.append( "<input type='submit' name='Request' value='Delete'/>");
         w.append( "<input type='submit' name='Request' value='Add'/>");
         w.append( "<input type='submit' name='Display' value='TextsList'/>");
         w.append( "</form>");

         data_.get_datum_html( w, hostname_.c_str(), rUserIP.c_str(), "dsk");

         w.append( "<form action='dsk' method='get' accept-charset='UTF-8' enctype='text/plain'>");
         //w.append( "<form action='dsk' method='get' accept-charset='UTF-8' enctype='application/x-www-form-urlencoded'>");
         //w.append( "<input type='submit' name='Action' value='SubmitComment'/><br>");
         //w.append( "<textarea rows='100' cols='100' name='What' wrap='soft'></textarea>");
         w.append( "</form>");
      }
      break;

   case eSttAddDatum:
      {
         w.append( "<form action='dsk' method='get' accept-charset='UTF-8'>");
         w.append( "<input type='submit' name='Action' value='Cancel'/>");
         w.append( "<input type='submit' name='Action' value='SubmitText'/><br>");
         ::get_datum_entry( w, data_, data_filter_);
         w.append( "</form>");
      }
      break;

   case eSttModifyDatum:
      {
         w.append( "<form action='dsk' method='get' accept-charset='UTF-8'>");
         w.append( "<input type='submit' name='Action' value='Cancel'/>");
         w.append( "<input type='submit' name='Action' value='SubmitText'/><br>");
         size_t lt11 = selected_t11();
         const dsk::DatumExt *pDatumExt = data_.GetDatumFromClientID( lt11);
         if( pDatumExt)
            ::get_datum_modify( w, *pDatumExt);
         w.append( "</form>");
      }
      break;
   }
}

// -----------------------------------------------------------------------------
static void get_input_for_existing_tags( HtmlWriter &w, 
                                         dsk::Data const& rData)
{
   w.add( "<input type='text' name='%s' value='' size='70' placeholder='%s' list='existing_tags' onkeyup='send_keywords_to_server(this.value)' />",
      dsk::Datum::get_attribut_from_tag(161),
      dsk::Datum::get_attribut_from_tag(161));

   w.head().add( "<script type='text/javascript' language='javascript'>");
   w.head().add( "function send_keywords_to_server(str)");
   w.head().add( "{");
   w.head().add( "if(str.length==0){document.getElementById('KeywordsHintDisplay').innerHTML='';return;}");
   w.head().add( "if(window.XMLHttpRequest){xmlHttp=new XMLHttpRequest();}"); // xhr2
   w.head().add( "else{xmlHttp=new ActiveXObject('Microsoft.XMLHTTP');}"); // for older IE 5/6
   w.head().add( "var url='?XMLHttpRequest_KeywordsTyped='+str;");
   w.head().add( "xmlHttp.open('GET',url,false);"); // async: true (asynchronous) or false (synchronous)
   w.head().add( "xmlHttp.send();"); // send(string) string: Only used for POST requests
   //w.head().add( "if(xmlHttp.readyState==4 && xmlHttp.status==200) {");
   w.head().add( "document.getElementById('KeywordsHintDisplay').innerHTML=xmlHttp.responseText;");
   //w.head().add( }");
   w.head().add( "}");
   w.head().add( "</script>");
}

// -----------------------------------------------------------------------------
void get_datalist_for_existing_tags( HtmlWriter &w, 
                                     dsk::Data const& rData,
                                     DskProxy::tDataFilter data_filter)
{
   std::set< tagOption> const& rtags = rData.tags( (dsk::DatumExt::DataType)data_filter);
   w.append( "<datalist id='existing_tags'>");
   std::set< tagOption>::const_iterator iter = rtags.begin();
   for( ; iter != rtags.end(); ++iter)
   {
      if( iter->label().empty())
      {
         w.add( "<option value='%s'></option>", iter->value().data());
      }
      else
      {
         w.add( "<option value='%s' label='%s'></option>",
            iter->value().data(), iter->label().data());
         //w.add( "<option value='%s'>%s</option>",
         //   iter->value().data(), iter->label().data());
      }
   }
   w.append( "</datalist>");
   
   w.append( "<div id='KeywordsHintDisplay'></div>");
}

// -----------------------------------------------------------------------------
static void get_datum_entry( HtmlWriter &w, 
                            dsk::Data const& rData,
                            DskProxy::tDataFilter data_filter)
{
   w.append( "<fieldset style='width:500'><legend><b>Entry</b></legend>");
   w.append( "<table>");

   // Subject Title
   w.append( "<tr>");
   w.add( "<td>%s</td><td>", dsk::Datum::get_attribut_from_tag(147));
   w.add( "<input type='text' name='%s' value='' size='70' placeholder='%s' />",
      dsk::Datum::get_attribut_from_tag(147),
      dsk::Datum::get_attribut_from_tag(147));
   w.append( "</td>");
   w.append( "</tr>");

   // URL
   w.append( "<tr>");
   w.add( "<td>%s</td><td>", dsk::Datum::get_attribut_from_tag(149));
   w.add( "<input type='text' name='%s' value='' size='70' placeholder='%s' />",
      dsk::Datum::get_attribut_from_tag(149),
      dsk::Datum::get_attribut_from_tag(149));
   w.append( "</td>");
   w.append( "</tr>");

   // Tags
   w.append( "<tr>");
   w.add( "<td>%s</td><td>", dsk::Datum::get_attribut_from_tag(161));
   ::get_input_for_existing_tags( w, rData);
   w.append( "</td>");
   w.append( "</tr>");

   // Text
   w.append( "<tr>");
   w.append( "<td colspan='2'>");
   w.add( "<textarea rows='20' cols='58' name='%s' wrap='soft'></textarea>",
      dsk::Datum::get_attribut_from_tag(58));
  //w.add( "<img src='dropdown_simple.gif' />");
   w.append( "</td>");
   w.append( "</tr>");

   w.append( "</table>");
   w.append( "</fieldset>");

   ::get_datalist_for_existing_tags( w, rData, data_filter);
}


// http://www.xul.fr/XMLHttpRequest.html#text-response-entity-body
// http://www.w3schools.com/xml/xml_http.asp
// http://www.w3schools.com/ajax/ajax_xmlhttprequest_response.asp
/*
<!DOCTYPE html>
<html>
<head>

<script>
function loadXMLDoc()
{
var xmlhttp;
if(window.XMLHttpRequest){xmlhttp=new XMLHttpRequest();}
else{xmlhttp=new ActiveXObject("Microsoft.XMLHTTP");}
xmlhttp.onreadystatechange=function()
{if(xmlhttp.readyState==4 && xmlhttp.status==200)
 {document.getElementById("myDiv").innerHTML = xmlhttp.responseText}}
xmlhttp.open("GET","ajax_info.txt",true);
xmlhttp.send();
}
</script>

</head>
<body>

<div id="myDiv"><h2>Let AJAX change this text</h2></div>
<button type="button" onclick="loadXMLDoc()">Change Content</button>

</body>
</html>
*/




/*
<html>
<head>
<script type="text/javascript" language="javascript">
var xmlhttp;
var user="123456";
var pass="123456";
function loadXMLDoc() 
{ 
xmlhttp=null;
if(window.XMLHttpRequest){xmlhttp=new XMLHttpRequest();}
else if (window.ActiveXObject){xmlhttp=new ActiveXObject("Microsoft.XMLHTTP");}
if (xmlhttp!=null)
  {
  xmlhttp.onreadystatechange=state_Change;
  xmlhttp.open("GET","http://dslstats.eircom.net/commoncgi/dslstats/showstats.cgi?stype=total&username="+user+"&password="+pass,false);
  xmlhttp.send(null);
  }
else
  {
  alert("Your browser does not support XMLHTTP.");
  }
}

function state_Change()
{
if (xmlhttp.readyState==4)
{// 4 = "loaded"

     //alert("readystate ==4");
         
  document.getElementById('response').innerHTML="response = "+xmlhttp.responseText;
  if (xmlhttp.status==200)
  {// 200 = "OK"
       alert("ok");
       //document.getElementById('show').innerHTML= "DONE: <br /><br />"+xmlhttp.responseText;   //last resort display all of the html
  }
  else
  {
    document.getElementById('show').innerHTML="no connection/error ??";
    document.getElementById('response').innerHTML=xmlhttp.responseText;

    }
  }
}
</script>


</head>
<body onload="">

<a href="javascript:loadXMLDoc();"> Click for stats <br /></a>
<div id="show">load stats page</div>
<div id="response">load response here</div>

</body >
</html>
*/
// -----------------------------------------------------------------------------
void get_datum_modify( HtmlWriter &w,
                       dsk::DatumExt const& rDatumExt)
{
   std::string msgtype = rDatumExt.t35().GetString();

   xString const& title = rDatumExt.t147();
   xString const& url   = rDatumExt.t149();
   xString const& tags  = rDatumExt.tags();
   xString const& text  = rDatumExt.t58();

   w.append( "<fieldset style='width:500'><legend><b>Entry</b></legend>");
   w.append( "<table>");

   // Subject Title
   w.append( "<tr>");
   w.add( "<td>%s</td><td>", dsk::Datum::get_attribut_from_tag(147));
   w.add( "<input type='text' name='%s' value='%s' size='70' placeholder='%s' />", 
      dsk::Datum::get_attribut_from_tag(147),
      title.data(),
      dsk::Datum::get_attribut_from_tag(147));
   w.append( "</td>");
   w.append( "</tr>");

   // URL
   w.append( "<tr>");
   w.add( "<td>%s</td><td>", dsk::Datum::get_attribut_from_tag(149));
   w.add( "<input type='text' name='%s' value='%s' size='70' placeholder='%s' />",
      dsk::Datum::get_attribut_from_tag(149),
      url.data(),
      dsk::Datum::get_attribut_from_tag(149));
   w.append( "</td>");
   w.append( "</tr>");

   // Tags
   w.append( "<tr>");
   w.add( "<td>%s</td><td>", dsk::Datum::get_attribut_from_tag(161));
   w.add( "<input type='text' name='%s' value='%s' size='70' placeholder='%s' />",
      dsk::Datum::get_attribut_from_tag(161),
      tags.data(),
      dsk::Datum::get_attribut_from_tag(161));
   w.append( "</td>");
   w.append( "</tr>");

   // Text
   w.append( "<tr>");
   w.append( "<td colspan='2'>");
   w.add( "<textarea rows='20' cols='58' name='%s' wrap='soft'>%s</textarea>",
      dsk::Datum::get_attribut_from_tag(58),
      text.data());
   w.append( "</td>");
   w.append( "</tr>");

   w.append( "</table>");
   w.append( "</fieldset>");
}

//------------------------------------------------------------------------------
void get_datafilter_form( HtmlWriter &w,
                          DskProxy::tDataFilter data_filter)
{
   w.append( "<form action='dsk' method='get' accept-charset='UTF-8'>");         
   w.append( "<select name='DataFilter' onchange='this.form.submit()'>");
      w.add( "<option value='events' %s>Events</option>",
         (data_filter == DskProxy::eFltEvents) ? "selected='selected'" : "");
      w.add( "<option value='bookmarks' %s>Bookmarks</option>",
         (data_filter == DskProxy::eFltBookmarks) ? "selected='selected'" : "");
      w.add( "<option value='notes' %s>Notes</option>",
         (data_filter == DskProxy::eFltNotes) ? "selected='selected'" : "");
      w.add( "<option value='tasks' %s>Tasks</option>",
         (data_filter == DskProxy::eFltTasks) ? "selected='selected'" : "");
      w.add( "<option value='contacts' %s>Contacts</option>",
         (data_filter == DskProxy::eFltContacts) ? "selected='selected'" : "");
   w.append( "</select>");
   w.append( "</form>");
}

//------------------------------------------------------------------------------
void get_datadisplay_form( HtmlWriter &w,
                           DskProxy::tDataDisplay data_display)
{
   w.append( "<form action='dsk' method='get' accept-charset='UTF-8'>");         
   w.append( "<select name='DataDisplay' onchange='this.form.submit()'>");
      w.add( "<option value='list' %s>List</option>",
         (data_display == DskProxy::eDspList) ? "selected='selected'" : "");
      w.add( "<option value='agenda' %s>Agenda</option>",
         (data_display == DskProxy::eDspAgenda) ? "selected='selected'" : "");
   w.append( "</select>");
   w.append( "</form>");
}

// -*- modeline for vim, do not remove please :-) -*-
// vim: set expandtab tabstop=3 shiftwidth=3 autoindent smartindent:
