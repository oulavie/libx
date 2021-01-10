
#pragma once
#ifndef mam_html_h
#define mam_html_h

//#include <initializer_list>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

// https://www.w3schools.com/tags/tag_th.asp
// https://developer.mozilla.org/en-US/docs/Web/HTML/Element
// https://validator.w3.org/nu/#textarea

/*
https://stackoverflow.com/questions/391979/how-to-get-clients-ip-address-using-javascript

https://www.cloudflare.com/cdn-cgi/trace          ip=2a01:cb18:7c2:4d00:b0e5:1256:4c11:d6bb
http://gd.geobytes.com/GetCityDetails             "geobytesremoteip":"86.233.161.245","geobytesipaddress":"86.233.161.245"
http://www.geoplugin.net/json.gp                  "geoplugin_request":"86.233.161.245",
https://ipapi.co/json/                            "ip": "2a01:cb18:7c2:4d00:b0e5:1256:4c11:d6bb",
http://ip-api.com/json                            "query":"86.233.161.245"
https://ipinfo.io/json                            "ip": "86.233.161.245",
https://jsonip.com/                               "ip":"2a01:cb18:7c2:4d00:b0e5:1256:4c11:d6bb"
http://ip.jsontest.com/                           {"ip": "2a01:cb18:7c2:4d00:b0e5:1256:4c11:d6bb"}
https://api64.ipify.org/?format=json              {"ip":"2a01:cb18:7c2:4d00:b0e5:1256:4c11:d6bb"}
http://api.ipify.org
https://api.ipify.org/
https://api.ipify.org/?format=json                {"ip":"86.233.161.245"}
https://api.ipify.org/?format=json                {"ip":"86.233.161.245"}
*/

namespace mam
{

class html
{
protected:
  std::string _name = {};
  std::vector<html *> _htmls;

public:
  // html(std::initializer_list<html> l_) : _htmls(l_) { }
  html(const std::string &name_) : _name(name_)
  {
  }
  template <typename... T> html(const std::string &name_, T... ts_) : _name(name_), _htmls({ts_...})
  {
  }
  virtual ~html()
  {
    for (size_t i(0); i < _htmls.size(); ++i)
    {
      delete _htmls[i];
      _htmls[i] = nullptr;
    }
  }
  html() noexcept = default;
  html(const html &) noexcept = delete;
  html(html &&) noexcept = delete;
  // html(std::string_view filename);
  html &operator=(const html &) noexcept = delete;
  html &operator=(html &&) noexcept = delete;
  virtual void gett(std::stringstream &html_)
  {
    // "<link rel='stylesheet' href=\"/style-paper.css\" type='text/css' />"
    // "<link rel='stylesheet' href=\"/chosen.css\" type='text/css' />"

    html_ << "<!DOCTYPE html><html lang='en'><head>";

    html_ << "<title>";
    html_ << _name; // displayed on tab of the browser
    html_ << "</title>";

    html_ << "<script>var ipa; function getIP(json){ ipa=json.ip; } function setIp(){ document.getElementById('00NF000000DAKs6').value=ipa;} </script>";
    html_ << "<script src='https://api.ipify.org?format=jsonp&callback=getIP'></script>";

    //html_ << "<script>function toCelsius(f) { return (5/9) * (f-32); } document.getElementById('demo').innerHTML = toCelsius;</script>";

    html_ << "</head>";
    html_ << "<body onload='setIp()'>";

    for (auto &it : _htmls)
      it->gett(html_);
    html_ << "</body></html>";
  }
};

class textasis : public html
{
public:
  textasis(const std::string &name_) : html(name_)
  {
  }
  virtual void gett(std::stringstream &html_)
  {
    html_ << _name;
  }
};


template <int N> class h : public html
{
public:
  h(const std::string &name_) : html(name_)
  {
  }
  template <typename... T> h(const std::string &name_, T... ts_) : html(name_, std::forward<T>(ts_)...)
  {
  }
  virtual void gett(std::stringstream &html_)
  {
    html_ << "<h" << N << ">";
    html_ << _name;
    for (auto &it : _htmls)
      it->gett(html_);
    html_ << "</h" << N << ">";
  }
};

class div : public html
{
public:
  template <typename... T> div(const std::string &name_, T... ts_) : html(name_, std::forward<T>(ts_)...)
  {
  }
  virtual void gett(std::stringstream &html_)
  {
    html_ << "<div>";
    for (auto &it : _htmls)
      it->gett(html_);
    html_ << "</div>";
  }
};

class tr : public html
{
public:
  tr(const std::string &name_) : html(name_)
  {
  }
  template <typename... T> tr(const std::string &name_, T... ts_) : html(name_, std::forward<T>(ts_)...)
  {
  }
  virtual void gett(std::stringstream &html_)
  {
    html_ << "<tr>";
    for (auto &it : _htmls)
      it->gett(html_);
    html_ << "</tr>";
  }
};

class td : public html
{
public:
  td(const std::string &name_) : html(name_)
  {
  }
  template <typename... T> td(const std::string &name_, T... ts_) : html(name_, std::forward<T>(ts_)...)
  {
  }
  virtual void gett(std::stringstream &html_)
  {
    html_ << "<td>";
    for (auto &it : _htmls)
      it->gett(html_);
    html_ << "</td>";
  }
};

class br : public html
{
public:
  br() : html("")
  {
  }
  virtual void gett(std::stringstream &html_)
  {
    html_ << "<br>";
  }
};


/*
<button>
<datalist>
<fieldset>
<form>
<label>
<legend>
<meter>
<optgroup>
<option>
<output>
<progress>
<select>
<textarea>
<input> types
*/

// https://developer.mozilla.org/en-US/docs/Web/HTML/Element/button

// https://developer.mozilla.org/en-US/docs/Web/HTML/Element/legend

// https://developer.mozilla.org/en-US/docs/Web/HTML/Element/form
class form : public html
{
public:
  form(const std::string &action_) : html(action_)
  {
  }
  template <typename... T> form(const std::string &action_, T... ts_) : html(action_, std::forward<T>(ts_)...)
  {
  }
  virtual void gett(std::stringstream &html_)
  {
    // "<form action='form_action_1' method='get' accept-charset='UTF-8'>";
    html_ << "<form action=\'" << _name << "\' method=\'get\' accept-charset=\'UTF-8\'>";
    for (auto &it : _htmls)
      it->gett(html_);
    html_ << "</form>";
  }
};

// https://developer.mozilla.org/en-US/docs/Web/HTML/Element/label
class label : public html
{
  std::string _for = {};
public:
  label(const std::string &name_) : html(name_)
  {
  }
  label(const std::string &name_, const char *for_) : html(name_), _for(for_)
  {
  }
  template <typename... T> label(const std::string &action_, T... ts_) : html(action_, std::forward<T>(ts_)...)
  {
  }
  virtual void gett(std::stringstream &html_)
  {
    // <label for="username">Username: </label>
    html_ << "<label ";
    if (!_for.empty())
      html_ << "for=\'" << _for << "\'";
    html_ << ">";
    for (auto &it : _htmls)
      it->gett(html_);
    html_ << _name << "</label>";
  }
};
/*
<div>
  <h3>Fruits</h3>
  <label> <input type="checkbox" class="radio" value="1" name="fooby[1][]" />Kiwi</label>
  <label> <input type="checkbox" class="radio" value="1" name="fooby[1][]" />Jackfruit</label>
  <label> <input type="checkbox" class="radio" value="1" name="fooby[1][]" />Mango</label>
</div>
<div>
  <h3>Animals</h3>
  <label> <input type="checkbox" class="radio" value="1" name="fooby[2][]" />Tiger</label>
  <label> <input type="checkbox" class="radio" value="1" name="fooby[2][]" />Sloth</label>
  <label> <input type="checkbox" class="radio" value="1" name="fooby[2][]" />Cheetah</label>
</div>
*/

// clang-format off
/*
<input type="hidden">
<input type="number">
<input type="text">
<input type="button">    <input type="button" /> buttons will not submit a form - they don't do anything by default.
<input type="submit">    <input type="submit"> buttons will submit the form they are in when the user clicks on them, unless you specify otherwise with JavaScript.
<input type="radio">
<input type="checkbox">
<input type="color">
<input type="file">
<input type="image">
<input type="password">
<input type="range">
<input type="reset">
<input type="search">
<input type="tel">
<input type="url">
<input type="email">
<input type="time">
<input type="date">
<input type="datetime">
<input type="datetime-local">
<input type="week">
<input type="month">

accept when type is file
alt when type is image
autocomplete when type is text, search, url, tel, email, password, date, month, week, time, datetime-local, number, range, or color
autofocus
checked when type is checkbox or radio
dirname when type is text or search
disabled
form
formaction when type is submit or image
formenctype when type is submit or image
formmethod when type is submit or image
formnovalidate when type is submit or image
formtarget when type is submit or image
height when type is image
list when type is text, search, url, tel, email, date, month, week, time, datetime-local, number, range, or color
max when type is date, month, week, time, datetime-local, number, or range
maxlength when type is text, search, url, tel, email, or password
min when type is date, month, week, time, datetime-local, number, or range
multiple when type is email or file
name
pattern when type is text, search, url, tel, email, or password
placeholder when type is text, search, url, tel, email, password, or number
readonly when type is text, search, url, tel, email, password, date, month, week, time, datetime-local, or number
required when type is text, search, url, tel, email, password, date, month, week, time, datetime-local, number, checkbox, radio, or file
size when type is text, search, url, tel, email, or password
src when type is image
step when type is date, month, week, time, datetime-local, number, or range
type
value when type is not file or image
width when type is image
*/
// clang-format on
// https://developer.mozilla.org/en-US/docs/Web/HTML/Element/input
// https://www.w3schools.com/tags/att_input_type.asp
class input : public html
{
  std::string _type = {}, _value = {}, _other = {};
  bool _required = {};

public:
  input(const std::string &type_, const std::string &name_, const std::string &value_, bool required_ = false)
      : html(name_), _type(type_), _value(value_), _required(required_)
  {
  }
  input(const std::string &type_, const std::string &name_, const std::string &value_, const std::string& other_, bool required_ = false)
      : html(name_), _type(type_), _value(value_), _other(other_), _required(required_)
  {
  }
  virtual void gett(std::stringstream &html_)
  {
    // "<input type='submit' name='Request' value='Modify'/>";
    // "<td><input required type='password' name='Password' value='' size='30' placeholder='password'/></td>";
    html_ << "<input ";
    if (_required)
      html_ << "required ";
    html_ << "type=\'" << _type << "\' name=\'" << _name << "\'";
    if( !_value.empty())
      html_ << " value=\'" << _value << "\'";
    if( !_other.empty())
      html_ << " " << _other;
    html_ << " />";
    // html_ << "type=\'" << _type << "\' name=\'" << _name << "\' value=\'" << _value << "\' id=\'id\'/>";
  }
};

// "<input type='text' name='%s' value='' size='70' placeholder='%s' list='existing_tags' onkeyup='send_keywords_to_server(this.value)' />"

// https://developer.mozilla.org/en-US/docs/Web/HTML/Element/select
class select : public html
{
  std::vector<std::pair<std::string,std::string>> _values;
public:
  select(const char* name_, const std::vector<std::pair<std::string,std::string>>& values_ ) : html(name_), _values(values_)
  {
  }
  virtual void gett(std::stringstream &html_)
  {
    html_ << "<select name=\'" << _name << "\'>";
    for( auto& it : _values)
      html_ << "<option value=\'" << it.first << "\'>" << it.second << "</option>";
    html_ << "</select>";
  }
};
/*
<select name="pets" id="pet-select">
  <option value="">--Please choose an option--</option>
  <option value="dog">Dog</option>
  <option value="cat">Cat</option>
  <option value="hamster">Hamster</option>
  <option value="parrot">Parrot</option>
  <option value="spider">Spider</option>
  <option value="goldfish">Goldfish</option>
</select>
*/

// https://developer.mozilla.org/en-US/docs/Web/HTML/Element/datalist
/*
<label for="myBrowser">Choose a browser from this list:</label>
<input list="browsers" id="myBrowser" name="myBrowser" />
<datalist id="browsers">
  <option value="Chrome">
  <option value="Firefox">
  <option value="Internet Explorer">
  <option value="Opera">
  <option value="Safari">
  <option value="Microsoft Edge">
</datalist>
*/

// https://www.w3schools.com/tags/tag_fieldset.asp
// https://developer.mozilla.org/en-US/docs/Web/HTML/Element/fieldset
class fieldset : public html
{
public:
  template <typename... T> fieldset(const std::string &legend_, T... ts_) : html(legend_, std::forward<T>(ts_)...)
  {
  }
  virtual void gett(std::stringstream &html_)
  {
    html_ << "<fieldset>";
    html_ << "<legend>" << _name << "</legend>";
    for (auto &it : _htmls)
      it->gett(html_);
    html_ << "</fieldset>";
  }
};

/*
<form>
  <fieldset>
    <legend>Choose your favorite monster</legend>
    <input type="radio" id="kraken" name="monster">    <label for="kraken">Kraken</label><br/>
    <input type="radio" id="sasquatch" name="monster"> <label for="sasquatch">Sasquatch</label><br/>
    <input type="radio" id="mothman" name="monster">   <label for="mothman">Mothman</label>
  </fieldset>
</form>

<form action="/action_page.php">
  <fieldset>
    <legend>Personalia:</legend>
    <label for="fname">First name:</label>  <input type="text" id="fname" name="fname"><br><br>
    <label for="lname">Last name:</label>   <input type="text" id="lname" name="lname"><br><br>
    <label for="email">Email:</label>       <input type="email" id="email" name="email"><br><br>
    <label for="birthday">Birthday:</label> <input type="date" id="birthday" name="birthday"><br><br>
    <input type="submit" value="Submit">
  </fieldset>
</form>
*/

//https://developer.mozilla.org/en-US/docs/Web/HTML/Element/optgroup
/*
<select id="dino-select">
  <optgroup label="Theropods">
    <option>Tyrannosaurus</option>
    <option>Velociraptor</option>
    <option>Deinonychus</option>
  </optgroup>
  <optgroup label="Sauropods">
    <option>Diplodocus</option>
    <option>Saltasaurus</option>
    <option>Apatosaurus</option>
  </optgroup>
</select>
*/

// https://developer.mozilla.org/en-US/docs/Web/HTML/Element/progress

// https://developer.mozilla.org/en-US/docs/Web/HTML/Element/meter

// https://developer.mozilla.org/en-US/docs/Web/HTML/Element/textarea



// https://www.w3schools.com/graphics/canvas_clock.asp

// https://en.cppreference.com/w/cpp/utility/initializer_list
template <class T> struct S
{
  std::vector<T> v;
  S(std::initializer_list<T> l) : v(l)
  {
  }
  void append(std::initializer_list<T> l)
  {
    v.insert(v.end(), l.begin(), l.end());
  }
  std::pair<const T *, std::size_t> c_arr() const
  {
    return {&v[0], v.size()}; // copy list-initialization in return statement
                              // this is NOT a use of std::initializer_list
  }
};

} // namespace mam

#endif

// vim: set expandtab tabstop=2 shiftwidth=2 autoindent smartindent:
