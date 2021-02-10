/***************************************************************************
 *   Copyright (C) 2008, Paul Lutus                                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

// the function "factor()" borrows from "factor.c"
// written by Paul Rubin <phr@ocf.berkeley.edu>
// part of the GNU utilities distribution
// and released under the GPL

// for an in-depth explanation of this factoring method, see:
// http://primes.utm.edu/glossary/page.php?sort=WheelFactorization

if (typeof document.attachEvent!='undefined') {
  window.attachEvent('onload',compute_factors);
}
else {
  window.addEventListener('load',compute_factors,false);
}

var wheel = new Array(
  1,2,2,4,2,4,2,4,6,2,6,4,2,4,6,6,2,6,4,2,6,4,6,8,4,2,4,2,4,14,4,6,2,10,2,6,6,
  4,2,4,6,2,10,2,4,2,12,10,2,4,2,4,6,2,6,4,6,6,6,2,6,4,2,6,4,6,8,4,2,4,6,8,6,
  10,2,4,6,2,6,6,4,2,4,6,2,6,4,2,6,10,2,10,2,4,2,4,6,8,4,2,4,12,2,6,4,2,6,4,6,
  12,2,4,2,4,8,6,4,6,2,4,6,2,6,10,2,4,6,2,6,4,2,4,2,10,2,10,2,4,6,6,2,6,6,4,6,6,
  2,6,4,2,6,4,6,8,4,2,6,4,8,6,4,6,2,4,6,8,6,4,2,10,2,6,4,2,4,2,10,2,10,2,4,2,4,8,
  6,4,2,4,6,6,2,6,4,8,4,6,8,4,2,4,2,4,8,6,4,6,6,6,2,6,6,4,2,4,6,2,6,4,2,4,2,10,2,
  10,2,6,4,6,2,6,4,2,4,6,6,8,4,2,6,10,8,4,2,4,2,4,8,10,6,2,4,8,6,6,4,2,4,6,2,6,4,6,
  2,10,2,10,2,4,2,4,6,2,6,4,2,4,6,6,2,6,6,6,4,6,8,4,2,4,2,4,8,6,4,8,4,6,2,6,6,4,2,
  4,6,8,4,2,4,2,10,2,10,2,4,2,4,6,2,10,2,4,6,8,6,4,2,6,4,6,8,4,6,2,4,8,6,4,6,2,4,6,
  2,6,6,4,6,6,2,6,6,4,2,10,2,10,2,4,2,4,6,2,6,4,2,10,6,2,6,4,2,6,4,6,8,4,2,4,2,12,6,
  4,6,2,4,6,2,12,4,2,4,8,6,4,2,4,2,10,2,10,6,2,4,6,2,6,4,2,4,6,6,2,6,4,2,10,6,8,6,4,
  2,4,8,6,4,6,2,4,6,2,6,6,6,4,6,2,6,4,2,4,2,10,12,2,4,2,10,2,6,4,2,4,6,6,2,10,2,6,4,
  14,4,2,4,2,4,8,6,4,6,2,4,6,2,6,6,4,2,4,6,2,6,4,2,4,12,2,12
);

function factor(m) {
  // keep a copy of the original number
  var km = m;
  var q,e;
  // wheel dimensions and entry point
  var wheel_max = wheel.length;
  var wheel_start = 5;
  // "s" accumulates the text result
  var s = "";
  var i = 0;
  var n = 2;
  do {
    e = 0;
    q = Math.floor(m/n);
    while(m == n * q) {
      e++;
      m = q;
      q = Math.floor(m/n);
    }
    // if this factor divided m
    if(e > 0) {
      s += n;
      // if more than once, show an exponent
      if(e > 1) s += "<sup>" + e + "</sup>";
      s += " ";
    }
    // next position on the wheel
    n += wheel[i++];
    if(i == wheel_max) i = wheel_start;
  }
  while(n <= q);
  // if there is a remainder
  if(m != 1 || km == 1) s += m;
  // is "m" composite?
  if(m != km) {
    s = "composite: " + s;
  }
  else {
    s = "prime.";
  }
  return s;
}

function show_result(s) {
  document.getElementById("factor_result").innerHTML = s;
}

function factor_then_show(v) {
  var start_time = new Date().getTime();
  s = factor(v);
  var delta = (new Date().getTime() - start_time) / 1000.0;
  if(delta == 0) delta = "< 0.001";
  s +=  " (" + delta + " seconds)";
  show_result(s);
}

function compute_factors() {
  var s;
  var vs = document.form1.factor_entry.value
  // strip whitespace
  vs = vs.replace(/^\s*(.*?)\s*$/,"$1")
  var v = parseInt(vs);
  if("" + v != vs) {
    show_result("number too large, largest is 2<sup>53</sup>-1.");
  }
  else if("" + v == "NaN" || v < 2) {
    show_result("please enter a number >= 2.");
  }
  else {
    show_result("computing ...");
    // this hack allows the above printing to appear
    // before factoring begins
    setTimeout("factor_then_show(" + v + ")",0);
  }
}
