// swad.js: javascript functions

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.
    Copyright (C) 1999-2015 Antonio Cañas-Vargas
    University of Granada (SPAIN) (acanas@ugr.es)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as
    published by the Free Software Foundation, either version 3 of the
    License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
"use strict";

// Global variable (string) used to write HTML
var Gbl_HTMLContent;

// Global variable used to call SWAD via AJAX
var ActionAJAX;

// Global variables used in writeLocalClock()
var secondsSince1970UTC;

// Global variables used in writeClockConnected()
var NumUsrsCon;
var ListSeconds = [];
var countClockConnected = 0;

/****************** Write a date in client local time ************************/
//id is the id of the HTML element in which date will be written
//TimeUTC is the date-time to write in UTC UNIX time format

function writeLocalDateFromUTC (id,TimeUTC,StrToday) {
	var today = new Date();
	var todayYea = today.getFullYear();
	var todayMon = today.getMonth()+1;
	var todayDay = today.getDate();
	var d = new Date();
	var Yea;
	var Mon;
	var Day;
	var StrMon;
	var StrDay;

	d.setTime(TimeUTC * 1000);
	Yea = d.getFullYear();
	Mon = d.getMonth() + 1;
	Day = d.getDate();
	StrMon = ((Mon < 10) ? '0' : '') + Mon;
	StrDay = ((Day < 10) ? '0' : '') + Day;

	if (Yea == todayYea && Mon == todayMon && Day == todayDay &&	// Today
		StrToday.length)
		document.getElementById(id).innerHTML = StrToday;
	else
		document.getElementById(id).innerHTML = Yea + '-' + StrMon + '-' + StrDay;

}

/*************** Write a date-time in client local time **********************/
// id is the id of the HTML element in which date-time will be written
// TimeUTC is the date-time to write in UTC UNIX time format
// separator is HTML code to write between date and time

function writeLocalDateHMSFromUTC (id,TimeUTC,separator,StrToday) {
	// HMS: Hour, Minutes, Seconds
	var today = new Date();
	var todayYea = today.getFullYear();
	var todayMon = today.getMonth()+1;
	var todayDay = today.getDate();
	var d = new Date();
	var Yea;
	var Mon;
	var Day;
	var Hou;
	var Min;
	var Sec;
	var StrMon;
	var StrDay;
	var StrHou;
	var StrMin;
	var StrSec;

	d.setTime(TimeUTC * 1000);
	Yea = d.getFullYear();
	Mon = d.getMonth() + 1;
	Day = d.getDate();
	Hou = d.getHours();
	Min = d.getMinutes();
	Sec = d.getSeconds();
	StrMon = ((Mon < 10) ? '0' : '') + Mon;
	StrDay = ((Day < 10) ? '0' : '') + Day;
	StrHou = ((Hou < 10) ? '0' : '') + Hou;
	StrMin = ((Min < 10) ? '0' : '') + Min;
	StrSec = ((Sec < 10) ? '0' : '') + Sec;
	if (Yea == todayYea && Mon == todayMon && Day == todayDay &&	// Today
		StrToday.length)
		document.getElementById(id).innerHTML = StrToday +
												separator +
												StrHou + ':' + StrMin + ':' + StrSec;
	else
		document.getElementById(id).innerHTML = Yea    + '-' + StrMon + '-' + StrDay +
												separator +
												StrHou + ':' + StrMin + ':' + StrSec;
}

function writeLocalDateHMFromUTC (id,TimeUTC,separator,StrToday) {
	// HM: Hour, Minutes
	var today = new Date();
	var todayYea = today.getFullYear();
	var todayMon = today.getMonth()+1;
	var todayDay = today.getDate();
	var d = new Date();
	var Yea;
	var Mon;
	var Day;
	var Hou;
	var Min;
	var StrMon;
	var StrDay;
	var StrHou;
	var StrMin;

	d.setTime(TimeUTC * 1000);
	Yea = d.getFullYear();
	Mon = d.getMonth() + 1;
	Day = d.getDate();
	Hou = d.getHours();
	Min = d.getMinutes();
	StrMon = ((Mon < 10) ? '0' : '') + Mon;
	StrDay = ((Day < 10) ? '0' : '') + Day;
	StrHou = ((Hou < 10) ? '0' : '') + Hou;
	StrMin = ((Min < 10) ? '0' : '') + Min;
	if (Yea == todayYea && Mon == todayMon && Day == todayDay &&	// Today
		StrToday.length)
		document.getElementById(id).innerHTML = StrToday +
												separator +
												StrHou + ':' + StrMin;
	else
		document.getElementById(id).innerHTML = Yea    + '-' + StrMon + '-' + StrDay +
												separator +
												StrHou + ':' + StrMin;
}

// Set local date-time form fields from UTC time
function setLocalDateTimeFormFromUTC (id,TimeUTC) {
	var FormYea = document.getElementById(id+'Year'  );
	var FormMon = document.getElementById(id+'Month' );
	var FormDay = document.getElementById(id+'Day'   );
	var FormHou = document.getElementById(id+'Hour'  );
	var FormMin = document.getElementById(id+'Minute');
	var FormSec = document.getElementById(id+'Second');
	var d;
	var Year;
	var YearIsValid = false;

	if (TimeUTC) {
		d = new Date();
		d.setTime(TimeUTC * 1000);
		Year = d.getFullYear();
		for (var i=0; i<FormYea.options.length && !YearIsValid; i++)
			if (FormYea.options[i].value == Year) {
				FormYea.options[i].selected = true;
				YearIsValid = true;
			}
		if (YearIsValid) {
			FormMon.options[d.getMonth()  ].selected = true;
			FormDay.options[d.getDate()-1 ].selected = true;
			FormHou.options[d.getHours()  ].selected = true;
			FormMin.options[d.getMinutes()].selected = true;
			FormSec.options[d.getSeconds()].selected = true;
		}
	}
	
	if (!YearIsValid) {
		FormYea.options[0].selected = true;
		FormMon.options[0].selected = true;
		FormDay.options[0].selected = true;
		FormHou.options[0].selected = true;
		FormMin.options[0].selected = true;
		FormSec.options[0].selected = true;
	}
}

// Set UTC time from local date-time form fields 
function setUTCFromLocalDateTimeForm (id) {
	var d = new Date();

	// Important: set year first in order to work properly with leap years
	d.setFullYear(document.getElementById(id+'Year'  ).value);
	d.setMonth   (document.getElementById(id+'Month' ).value-1);
	d.setDate    (document.getElementById(id+'Day'   ).value);
	d.setHours   (document.getElementById(id+'Hour'  ).value);
	d.setMinutes (document.getElementById(id+'Minute').value);
	d.setSeconds (document.getElementById(id+'Second').value);
	d.setMilliseconds(0);

	document.getElementById(id+'TimeUTC').value = d.getTime() / 1000;
}

// Set form param with time difference between UTC time and client local time, in minutes
// For example, if your time zone is GMT+2, -120 will be returned
function setTZ (id) {
	var FormTZ = document.getElementById(id);
	var d = new Date();

	FormTZ.value = d.getTimezoneOffset();
}

// Set form param with time difference between UTC time and client local time, in minutes
// For example, if your time zone is GMT+2, -120 will be returned
function setTZname (id) {
	var FormTZname = document.getElementById(id);
	var tz = jstz.determine();	// Determines the time zone of the browser client
	FormTZname.value = tz.name();	// Returns the name of the time zone eg "Europe/Berlin"
}

// Adjust a date form correcting days in the month
function adjustDateForm (id) {
	var FormYea = document.getElementById(id+'Year' );
	var FormMon = document.getElementById(id+'Month');
	var FormDay = document.getElementById(id+'Day'  );
	var Yea = FormYea.options[FormYea.selectedIndex].value;
	var Days;

	if (FormMon.options[1].selected)			// February
		Days = ((((Yea % 4) == 0) && ((Yea % 100) != 0)) || ((Yea % 400) == 0)) ? 29 : 28;
	else if (FormMon.options[ 3].selected ||	// April
			 FormMon.options[ 5].selected ||	// June
			 FormMon.options[ 8].selected ||	// September
			 FormMon.options[10].selected)		// November
		Days = 30;
	else
		Days = 31;

	if (FormDay.selectedIndex >= Days)
		FormDay.options[Days-1].selected = true;			// Select last day in month

	for (var i=FormDay.options.length; i<Days; i++) {		// Create new days at the end
		FormDay.options[i] = new Option(String(i+1),i+1);
	}
	for (var i=FormDay.options.length-1; i>=Days; i--)		// Remove days from the end
		FormDay.options[i] = null;
}

// Set a date range form to yesterday
function setDateToYesterday () {
	var d = new Date();

	d.setTime (d.getTime () - 24*60*60*1000);	// Today - 1 day
	setDateRange(d);
}

// Set a date range form to today
function setDateToToday () {
	var d = new Date();

	setDateRange(d);
}

// Set a date range form to a specific day
function setDateRange (d) {
	var FormYea;
	var Yea = d.getFullYear();
	var Mon = d.getMonth()+1;
	var Day = d.getDate();

	FormYea = document.getElementById('StartYear');
	for (var i=0; i<FormYea.options.length; i++)
		if (FormYea.options[i].value == Yea) {
			FormYea.options[i].selected = true;
			break;
		}
	document.getElementById('StartMonth' ).options[Mon-1].selected = true;
	document.getElementById('StartDay'   ).options[Day-1].selected = true;
	document.getElementById('StartHour'  ).options[0    ].selected = true;
	document.getElementById('StartMinute').options[0    ].selected = true;
	document.getElementById('StartSecond').options[0    ].selected = true;
	setUTCFromLocalDateTimeForm('Start');

	FormYea = document.getElementById('EndYear');
	for (var i=0; i<FormYea.options.length; i++)
		if (FormYea.options[i].value == Yea) {
			FormYea.options[i].selected = true;
			break;
		}
	document.getElementById('EndMonth' ).options[Mon-1].selected = true;
	document.getElementById('EndDay'   ).options[Day-1].selected = true;
	document.getElementById('EndHour'  ).options[23   ].selected = true;
	document.getElementById('EndMinute').options[59   ].selected = true;
	document.getElementById('EndSecond').options[59   ].selected = true;
	setUTCFromLocalDateTimeForm('End');
}

// Write clock in client local time updated every minute
function writeLocalClock () {
	var d;
	var Mon;
	var Day;
	var Hou;
	var Min;
	var StrMin;

	setTimeout('writeLocalClock()',60000);

	d = new Date();
	d.setTime(secondsSince1970UTC * 1000);
	secondsSince1970UTC += 60;	// For next call

	Mon = d.getMonth();
	Day = d.getDate();
	Hou = d.getHours();
	Min = d.getMinutes();
	StrMin = ((Min < 10) ? '0' : '') + Min;
	document.getElementById('hm').innerHTML =	'<span id="current_date">' +
												Months[Mon] + ' ' + Day + ', ' +
												'</span><span id="current_time">' +
												Hou + ':' + StrMin +
												'</span>';
}
      
function writeClockConnected () {
	var BoxClock;
	var H;
	var M;
	var S;
	var StrM;
	var StrS;
	var PrintableClock;

	countClockConnected++;
	countClockConnected %= 10;
	for (var i=0; i<NumUsrsCon; i++) {
		BoxClock = document.getElementById('hm'+i);
		if (BoxClock) {
			ListSeconds[i] += 1;
			if (!countClockConnected) {	// Print after 10 seconds
				M = Math.floor(ListSeconds[i] / 60);
				if (M >= 60) {
					H = Math.floor(M / 60);
					M %= 60;
				} else
					H = 0;
				S = ListSeconds[i] % 60;
				if (H != 0) {
					StrM = ((M < 10) ? '0' : '') + M;
					StrS = ((S < 10) ? '0' : '') + S;
					PrintableClock = H + ':' + StrM + '&#39;' + StrS + '&quot;';
				} else if (M != 0) {
					StrS = ((S < 10) ? '0' : '') + S;
					PrintableClock = M + '&#39;' + StrS + '&quot;';
				} else
					PrintableClock = S + '&quot;';
				BoxClock.innerHTML = PrintableClock;
			}
		}
	}
	setTimeout('writeClockConnected()',1000);	// refresh after 1 second
}

// Automatic refresh of connected users using AJAX. This function must be called from time to time
var objXMLHttpReqCon = false;
function refreshConnected () {
	objXMLHttpReqCon = AJAXCreateObject();
	if (objXMLHttpReqCon) {
		var RefreshParams = RefreshParamNxtActCon + '&' +
							RefreshParamIdSes + '&' +
							RefreshParamCrsCod;

		objXMLHttpReqCon.onreadystatechange = readConnUsrsData;	// onreadystatechange must be lowercase
		objXMLHttpReqCon.open('POST',ActionAJAX,true);
		objXMLHttpReqCon.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');
		objXMLHttpReqCon.send(RefreshParams);
	}
}

// Automatic refresh of last clicks using AJAX. This function must be called from time to time
var objXMLHttpReqLog = false;
function refreshLastClicks () {
	objXMLHttpReqLog = AJAXCreateObject();
	if (objXMLHttpReqLog) {
		var RefreshParams = RefreshParamNxtActLog + '&' +
							RefreshParamIdSes + '&' +
							RefreshParamCrsCod;

		objXMLHttpReqLog.onreadystatechange = readLastClicksData;	// onreadystatechange must be lowercase
		objXMLHttpReqLog.open('POST',ActionAJAX,true);
		objXMLHttpReqLog.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');
		objXMLHttpReqLog.send(RefreshParams);
	}
}

// Automatic refresh of new publishings in social timeline using AJAX. This function must be called from time to time
var objXMLHttpReqSoc = false;
function refreshNewTimeline () {
	objXMLHttpReqSoc = AJAXCreateObject();
	if (objXMLHttpReqSoc) {
		var RefreshParams = RefreshParamNxtActNewPub + '&' +
							RefreshParamIdSes;

		objXMLHttpReqSoc.onreadystatechange = readNewTimelineData;	// onreadystatechange must be lowercase
		objXMLHttpReqSoc.open('POST',ActionAJAX,true);
		objXMLHttpReqSoc.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');
		objXMLHttpReqSoc.send(RefreshParams);
	}
}

// Refresh of old publishings in social timeline using AJAX. This function is called when user clicks in link
var objXMLHttpReqSoc = false;
function refreshOldTimeline () {
	objXMLHttpReqSoc = AJAXCreateObject ();
	if (objXMLHttpReqSoc) {
		var RefreshParams = RefreshParamNxtActOldPub + '&' + RefreshParamIdSes;
		if (RefreshParamUsr)
			if (RefreshParamUsr.length)
				RefreshParams += '&' + RefreshParamUsr;

		objXMLHttpReqSoc.onreadystatechange = readOldTimelineData;	// onreadystatechange must be lowercase
		objXMLHttpReqSoc.open('POST',ActionAJAX,true);
		objXMLHttpReqSoc.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');
		objXMLHttpReqSoc.send(RefreshParams);
	}
}

// Create AJAX object	(try is unknown in earlier versions of Netscape, but works in IE5)
function AJAXCreateObject () {
	var obj = false;
	if (window.XMLHttpRequest) {	// Mozilla, Safari,...
		obj = new XMLHttpRequest();
	} else if (window.ActiveXObject) {	// IE
		try {
			obj = new ActiveXObject('Msxml2.XMLHTTP');
		} catch (e) {
			try {
				obj = new ActiveXObject('Microsoft.XMLHTTP');
			} catch (e) {}
		}
	}
	return obj;
}

// Receives and show connected users data
function readConnUsrsData () {
	if (objXMLHttpReqCon.readyState == 4) {	// Check if data have been received
		if (objXMLHttpReqCon.status == 200) {
			var endOfDelay   = objXMLHttpReqCon.responseText.indexOf('|',0);				// Get separator position
			var endOfNotif   = objXMLHttpReqCon.responseText.indexOf('|',endOfDelay+1);		// Get separator position
			var endOfGblCon  = objXMLHttpReqCon.responseText.indexOf('|',endOfNotif+1);		// Get separator position
			var endOfCrsCon  = objXMLHttpReqCon.responseText.indexOf('|',endOfGblCon+1);	// Get separator position
			var endOfNumUsrs = objXMLHttpReqCon.responseText.indexOf('|',endOfCrsCon+1);	// Get separator position

			var delay = parseInt(objXMLHttpReqCon.responseText.substring(0,endOfDelay));			// Get refresh delay
			var htmlNotif  = objXMLHttpReqCon.responseText.substring(endOfDelay +1,endOfNotif);		// Get HTML code for new notifications
			var htmlGblCon = objXMLHttpReqCon.responseText.substring(endOfNotif +1,endOfGblCon);	// Get HTML code for connected
			var htmlCrsCon = objXMLHttpReqCon.responseText.substring(endOfGblCon+1,endOfCrsCon);	// Get HTML code for course connected
			var NumUsrsStr = objXMLHttpReqCon.responseText.substring(endOfCrsCon+1,endOfNumUsrs);	// Get number of users
			var startOfUsr;
			var endOfUsr;

			NumUsrsCon = (NumUsrsStr.length ? parseInt(NumUsrsStr) : 0);

			var divNewNotif = document.getElementById('msg');			// Access to new notifications DIV
			if (divNewNotif)
				divNewNotif.innerHTML = (htmlNotif.length) ? htmlNotif : '';	// Update new notifications DIV

			var divGblConnected = document.getElementById('globalconnected');	// Access to global connected DIV
			if (divGblConnected)
				divGblConnected.innerHTML = htmlGblCon;				// Update global connected DIV
			if (htmlCrsCon.length) {
				var divCrsConnected = document.getElementById('courseconnected');	// Access to course connected DIV
				if (divCrsConnected) {
					divCrsConnected.innerHTML = htmlCrsCon;				// Update course connected DIV
					countClockConnected = 0;	// Don't refresh again using writeClockConnected until past 10 seconds
					startOfUsr = endOfNumUsrs + 1;
					for (var NumUsr=0; NumUsr<NumUsrsCon; NumUsr++) {
						endOfUsr = objXMLHttpReqCon.responseText.indexOf('|',startOfUsr+1);
						ListSeconds[NumUsr] = parseInt(objXMLHttpReqCon.responseText.substring(startOfUsr,endOfUsr));
						startOfUsr = endOfUsr + 1;
					}
				}
			}

			if (delay >= 60000)	// If refresh slower than 1 time each 60 seconds, do refresh; else abort
				setTimeout('refreshConnected()',delay);
		}
	}
}

// Receives and show last clicks data
function readLastClicksData () {
	if (objXMLHttpReqLog.readyState == 4) {	// Check if data have been received
		if (objXMLHttpReqLog.status == 200) {
			var endOfDelay = objXMLHttpReqLog.responseText.indexOf('|',0);	// Get separator position

			var delay = parseInt(objXMLHttpReqLog.responseText.substring(0,endOfDelay));	// Get refresh delay
			var htmlLastClicks = objXMLHttpReqLog.responseText.substring(endOfDelay+1);		// Get HTML code for last clicks

			var divLastClicks = document.getElementById('lastclicks');			// Access to last click DIV
			if (divLastClicks)
				divLastClicks.innerHTML = htmlLastClicks;				// Update global connected DIV
			if (delay > 200)	// If refresh slower than 1 time each 0.2 seconds, do refresh; else abort
				setTimeout('refreshLastClicks()',delay);
		}
	}
}

// Receives and show new social timeline data
function readNewTimelineData () {
	if (objXMLHttpReqSoc.readyState == 4) {	// Check if data have been received
		if (objXMLHttpReqSoc.status == 200) {
			var endOfDelay = objXMLHttpReqSoc.responseText.indexOf('|',0);					// Get separator position
			var delay = parseInt(objXMLHttpReqSoc.responseText.substring(0,endOfDelay));	// Get refresh delay
			var htmlJustNowTimeline = objXMLHttpReqSoc.responseText.substring(endOfDelay+1);// Get HTML code for social timeline

			var justNowTimeline = document.getElementById('just_now_timeline_list');// Access to UL for the just received timeline
			if (justNowTimeline) {
				justNowTimeline.innerHTML = htmlJustNowTimeline;	// Update list of publishings in just now timeline		
				var countJustNowTimeline = justNowTimeline.childNodes.length;

				if (countJustNowTimeline) {	// New pubs just retrieved
					// Scripts in timeline got via AJAX are not executed ==> execute them
					evalScriptsInElem (justNowTimeline);

					// Process mathematics; see http://docs.mathjax.org/en/latest/advanced/typeset.html
					MathJax.Hub.Queue(["Typeset",MathJax.Hub,justNowTimeline]);

					// Move just received timeline to top of new timeline
					var newTimeline = document.getElementById('new_timeline_list');		// Access to UL with the new timeline

					// Move all the LI elements in UL 'just_now_timeline_list' to the top of UL 'new_timeline_list'
					for (var i=0; i<countJustNowTimeline; i++)
						newTimeline.insertBefore(justNowTimeline.lastChild, newTimeline.childNodes[0]);

					// Update number of new posts
					var viewNewPostsCount = document.getElementById('view_new_posts_count');
					viewNewPostsCount.innerHTML = newTimeline.childNodes.length;

					// Display message with new posts if hidden
					var viewNewPostsContainer = document.getElementById('view_new_posts_container');
					viewNewPostsContainer.style.display = '';
				}
			}

			if (delay >= 10000)	// If refresh slower than 1 time each 10 seconds, do refresh; else abort
				setTimeout('refreshNewTimeline()',delay);
		}
	}
}

// Receives and show old social timeline data
function readOldTimelineData () {
	if (objXMLHttpReqSoc.readyState == 4) {	// Check if data have been received
		if (objXMLHttpReqSoc.status == 200) {
			var endOfDelay = objXMLHttpReqSoc.responseText.indexOf('|',0);					// Get separator position
			var delay = parseInt(objXMLHttpReqSoc.responseText.substring(0,endOfDelay));	// Get refresh delay
			var htmlOldTimeline = objXMLHttpReqSoc.responseText.substring(endOfDelay+1);	// Get HTML code for social timeline

			var oldTimeline = document.getElementById('old_timeline_list');	// Access to UL with the recent timeline
			if (oldTimeline) {
				oldTimeline.innerHTML = htmlOldTimeline;	// Fill list of publishings in old timeline
				var countOldTimeline = oldTimeline.childNodes.length;
				
				if (countOldTimeline) {
					// Scripts in timeline got via AJAX are not executed ==> execute them
					evalScriptsInElem (oldTimeline);

					// Process mathematics; see http://docs.mathjax.org/en/latest/advanced/typeset.html
					MathJax.Hub.Queue(["Typeset",MathJax.Hub,oldTimeline]);

					// Move all the LI elements in UL 'old_timeline_list' to the bottom of UL 'timeline_list'
					var timeline = document.getElementById("timeline_list");
					for (var i=0; i<countOldTimeline; i++)
							timeline.appendChild(oldTimeline.firstChild);
				}
				
				if (countOldTimeline < 20)	// Set to Soc_MAX_OLD_PUBS_TO_GET_AND_SHOW
					// No more old publishings
					document.getElementById("view_old_posts_container").style.display = 'none';
				else {
					// There may be more publishings
					document.getElementById('get_old_timeline').style.display='';			// Show icon to be hidden on click
					document.getElementById('getting_old_timeline').style.display='none';	// Hide icon to be shown on click
				}
			}
		}
	}
}

// Move new timeline to top of timeline
function moveNewTimelineToTimeline () {
	// Move all the LI elements in UL 'new_timeline_list' to the top of UL 'timeline_list'
	var newTimeline = document.getElementById('new_timeline_list');
	var countNewTimeline = newTimeline.childNodes.length;
	if (countNewTimeline) {
		var timeline = document.getElementById("timeline_list");
		for (var i=0; i<countNewTimeline; i++) {
			timeline.insertBefore(newTimeline.lastChild, timeline.childNodes[0]);
			timeline.childNodes[0].className += " SOCIAL_NEW_PUB";
		}
    }

	// Reset number of new posts after moving
	var viewNewPostsCount = document.getElementById('view_new_posts_count');
	viewNewPostsCount.innerHTML = 0;

	// Hide link to view new posts after moving
	var viewNewPostsContainer = document.getElementById('view_new_posts_container');
	viewNewPostsContainer.style.display = 'none';
}

// Scripts got via AJAX are not executed ==> execute them
function evalScriptsInElem (elem) {
	var scrs = elem.getElementsByTagName("script");
	var s;
	for (var i=0; i<scrs.length; i++) {
		s = scrs[i].childNodes[0].nodeValue;
		if (s != null) eval(s);
	}
}

// Expand textarea when focus. Called from a textarea onfocus
function expandTextarea (textareaElem,idButton,rows) {
	textareaElem.rows = rows;
	document.getElementById(idButton).style.display = '';
}

// Contract textarea when focus. Called from a textarea onblur
/*
function contractTextarea (textareaElem,idButton,rows) {
	if (textareaElem.value == '') {
		document.getElementById(idButton).style.display = 'none';
		textareaElem.rows = rows;
	}
}
*/
// Change display of a test answer
function toggleAnswer (option) {
	toggleDisplay('ans_' + option);
	toggleDisplay('con_' + option);
	toggleDisplay('exp_' + option);
}

// Change display of a element (hidden or visible)
function toggleDisplay (elementID) {
	var stl = document.getElementById (elementID).style;
	stl.display = (stl.display === 'none') ? '' : 'none';
}

// Zoom a user's photograph
function zoom (img,urlPhoto,shortName) {
	var zoomImgWidth  = 186;	// big photo
	var zoomImgHeight = 248;	// big photo
	var padding = 7;			// padding around big photo including border
	var xPos = img.offsetLeft;
	var yPos = img.offsetTop;
	var tempEl = img.offsetParent;

	while (tempEl != null) {
		xPos += tempEl.offsetLeft;
		yPos += tempEl.offsetTop;
		tempEl = tempEl.offsetParent;
	}
	if (xPos - (zoomImgWidth + padding*2) >= 0)
	   xPos -= (zoomImgWidth + padding*2);	// zoom at left of the photo
	else
	   xPos += img.width + 1;				// zoom at right of the photo
	yPos = yPos + img.height - zoomImgHeight - padding;
	if (yPos < 0)
	   yPos = 0;
	document.getElementById('zoomLyr').style.left = xPos + 'px';
	document.getElementById('zoomLyr').style.top  = yPos + 'px';
	document.getElementById('zoomImg').src = urlPhoto;
	document.getElementById('zoomTxt').innerHTML = '<span class="ZOOM_TXT">' + shortName + '</span>';
}

// Exit from zooming a user's photograph
function noZoom () {
	var xPos = -(187+15);
	var yPos = -(250+15+110);
	document.getElementById('zoomTxt').innerHTML = '';
	document.getElementById('zoomImg').src='/swad/icon/usr_bl.jpg';
	document.getElementById('zoomLyr').style.left = xPos + 'px';
	document.getElementById('zoomLyr').style.top = yPos + 'px';
}

// Select or unselect a radio element in a form
function selectUnselectRadio (radio,groupRadios,numRadiosInGroup){
	if (radio.IsChecked) radio.checked = false;
	radio.IsChecked = !radio.IsChecked;
	for (var i=0; i<numRadiosInGroup; i++)
		if (groupRadios[i] != radio) groupRadios[i].IsChecked = 0;
}

// Activate a parent checkbox when all children checkboxes are activated
// Deactivate a parent checkbox when any child checkbox is deactivated
function checkParent (CheckBox, MainCheckbox) {
	var IsChecked = true, i, Formul = CheckBox.form;
	for (i=0; i<Formul.elements.length; i++)
		if (Formul.elements[i].name == CheckBox.name)
			if (!(Formul.elements[i].checked)) { IsChecked = false; break; }
	Formul[MainCheckbox].checked = IsChecked;
}
// Activate all children checkboxes when parent checkbox is activated
// Deactivate all children checkboxes when parent checkbox is deactivated
function togglecheckChildren (MainCheckbox, GroupCheckboxes) {
	var i, Formul = MainCheckbox.form;
	for (i=0; i<Formul.elements.length; i++)
		if (Formul.elements[i].name == GroupCheckboxes) Formul.elements[i].checked = MainCheckbox.checked;
}

// Deactivate a parent checkbox when any child checkbox is activated
// Activate a parent checkbox when all children checkboxes are deactivated
function uncheckParent (CheckBox, MainCheckbox) {
	var IsChecked = false, i, Formul = CheckBox.form;
	for (i=0; i<Formul.elements.length; i++)
		if (Formul.elements[i].name == CheckBox.name)
			if (Formul.elements[i].checked) { IsChecked = true; break; }
	Formul[MainCheckbox].checked = !IsChecked;
}
// Deactivate all children checkboxes when parent checkbox is activated
function uncheckChildren (MainCheckbox, GroupCheckboxes) {
	var i, Formul = MainCheckbox.form;
	if (MainCheckbox.checked)
		for (i=0; i<Formul.elements.length; i++)
			if (Formul.elements[i].name == GroupCheckboxes) Formul.elements[i].checked = false;
}

// Change text of a test descriptor
function changeTxtTag (NumTag){
	var Sel = document.getElementById('SelDesc' + NumTag);

	document.getElementById('TagTxt' + NumTag).value = Sel.options[Sel.selectedIndex].value;
}

// Change selectors of test descriptors
function changeSelTag (NumTag) {
	var Sel = document.getElementById('SelDesc'+NumTag);
	var Txt = document.getElementById('TagTxt' +NumTag);

	for (var i=0; i<Sel.options.length - 1; i++)
		if (Sel.options[i].value.toUpperCase() == Txt.value.toUpperCase()) {
			Sel.options[i].selected = true;
			Txt.value = Sel.options[i].value;
			break;
		}
	if (i == Sel.options.length - 1) // End reached without matching
		Sel.options[i].selected = true;
}

// Activate or deactivate answer types of a test question
function enableDisableAns (Formul) {
	var Tst_ANS_INT				= 0;
	var Tst_ANS_FLOAT			= 1;
	var Tst_ANS_TRUE_FALSE		= 2;
	var Tst_ANS_UNIQUE_CHOICE	= 3;
	var Tst_ANS_MULTIPLE_CHOICE	= 4;
	var Tst_ANS_TEXT			= 5;

	if (Formul.AnswerType[Tst_ANS_INT].checked){
		for (var i=0; i<Formul.elements.length; i++)
			if (Formul.elements[i].name == 'AnsInt')
				Formul.elements[i].disabled = false;
			else if (Formul.elements[i].name == 'AnsMulti'	||
				Formul.elements[i].name == 'AnsFloatMin'	||
				Formul.elements[i].name == 'AnsFloatMax'	||
				Formul.elements[i].name == 'AnsTF'			||
				Formul.elements[i].name == 'AnsUni'			||
				Formul.elements[i].name == 'Shuffle')
				Formul.elements[i].disabled = true;
			else {
				enableDisableStrAns(Formul.elements[i],true);
				enableDisableImgAns(Formul.elements[i],true);
			}
	}
	else if (Formul.AnswerType[Tst_ANS_FLOAT].checked){
		for (var i=0; i<Formul.elements.length; i++)
			if (Formul.elements[i].name == 'AnsFloatMin'	||
				Formul.elements[i].name == 'AnsFloatMax')
				Formul.elements[i].disabled = false;
			else if (Formul.elements[i].name == 'AnsInt'	||
				Formul.elements[i].name == 'AnsTF'			||
				Formul.elements[i].name == 'AnsUni'			||
				Formul.elements[i].name == 'AnsMulti'		||
				Formul.elements[i].name == 'Shuffle')
				Formul.elements[i].disabled = true;
			else {
				enableDisableStrAns(Formul.elements[i],true);
				enableDisableImgAns(Formul.elements[i],true);
			}
	}
	else if (Formul.AnswerType[Tst_ANS_TRUE_FALSE].checked){
		for (var i=0; i<Formul.elements.length; i++)
			if (Formul.elements[i].name == 'AnsTF')
				Formul.elements[i].disabled = false;
			else if (Formul.elements[i].name == 'AnsInt'	||
				Formul.elements[i].name == 'AnsFloatMin'	||
				Formul.elements[i].name == 'AnsFloatMax'	||
				Formul.elements[i].name == 'AnsUni'			||
				Formul.elements[i].name == 'AnsMulti'		||
				Formul.elements[i].name == 'Shuffle')
				Formul.elements[i].disabled = true;
			else {
				enableDisableStrAns(Formul.elements[i],true);
				enableDisableImgAns(Formul.elements[i],true);
			}
	}
	else if (Formul.AnswerType[Tst_ANS_UNIQUE_CHOICE].checked){
		for (var i=0; i<Formul.elements.length; i++)
			if (Formul.elements[i].name == 'AnsUni'	||
				Formul.elements[i].name == 'Shuffle')
				Formul.elements[i].disabled = false;
			else if (Formul.elements[i].name == 'AnsInt'		||
					 Formul.elements[i].name == 'AnsFloatMin'	||
					 Formul.elements[i].name == 'AnsFloatMax'	||
					 Formul.elements[i].name == 'AnsTF'			||
					 Formul.elements[i].name == 'AnsMulti')
				Formul.elements[i].disabled = true;
			else {
				enableDisableStrAns(Formul.elements[i],false);
				enableDisableImgAns(Formul.elements[i],false);
			}
	}
	else if (Formul.AnswerType[Tst_ANS_MULTIPLE_CHOICE].checked){
		for (var i=0; i<Formul.elements.length; i++)
			if (Formul.elements[i].name == 'AnsMulti'	||
				Formul.elements[i].name == 'Shuffle')
				Formul.elements[i].disabled = false;
			else if (Formul.elements[i].name == 'AnsInt'		||
					 Formul.elements[i].name == 'AnsFloatMin'	||
					 Formul.elements[i].name == 'AnsFloatMax'	||
					 Formul.elements[i].name == 'AnsTF'			||
					 Formul.elements[i].name == 'AnsUni')
				Formul.elements[i].disabled = true;
			else {
				enableDisableStrAns(Formul.elements[i],false);
				enableDisableImgAns(Formul.elements[i],false);
			}
	}
	else if (Formul.AnswerType[Tst_ANS_TEXT].checked){
		for (var i=0; i<Formul.elements.length; i++)
			if (Formul.elements[i].name == 'AnsInt'			||
				Formul.elements[i].name == 'AnsFloatMin'	||
				Formul.elements[i].name == 'AnsFloatMax'	||
				Formul.elements[i].name == 'AnsTF'			||
				Formul.elements[i].name == 'AnsUni'			||
				Formul.elements[i].name == 'AnsMulti')
				Formul.elements[i].disabled = true;
			else {
				enableDisableStrAns(Formul.elements[i],false);
				enableDisableImgAns(Formul.elements[i],true);
			}
	}
}

// Activate or deactivate answer strings and feedback of a test question
function enableDisableStrAns (elem, isDisabled) {
	var Tst_MAX_OPTIONS_PER_QUESTION = 10;

	for (var i = 0; i < Tst_MAX_OPTIONS_PER_QUESTION; i++)
		if (elem.name == ('AnsStr' + i) ||
			elem.name == ('FbStr'  + i))
			elem.disabled = isDisabled;
}

//Activate or deactivate answer images of a test question
function enableDisableImgAns (elem, isDisabled) {
	var Tst_MAX_OPTIONS_PER_QUESTION = 10;

	for (var i = 0; i < Tst_MAX_OPTIONS_PER_QUESTION; i++)
		if (elem.name == ('ImgAct' + i) ||
			elem.name == ('ImgFil' + i) ||
			elem.name == ('ImgTit' + i))
			elem.disabled = isDisabled;
}

// Selection of statistics of current course ****/
function enableDetailedClicks () {
	document.getElementById('CountType').disabled = true;
	document.getElementById('GroupedBy').disabled = true;
	document.getElementById('RowsPage').disabled = false;
}

function disableDetailedClicks () {
	document.getElementById('CountType').disabled = false;
	document.getElementById('GroupedBy').disabled = false;
	document.getElementById('RowsPage').disabled = true;
}

/*****************************************************************************/
/************************ Draw an academic calendar **************************/
/*****************************************************************************/

function Cal_DrawCalendar (id,FirstDayOfWeek,TimeUTC,CurrentPlcCod,PrintView,
						   CGI,FormGoToCalendarParams,FormEventParams) {
	var StartingMonth = [	// Calendar starts one row before current month
		10,	// January   --> October
		10,	// February  --> October
		10,	// Mars      --> October
		 1,	// April     --> January
		 1,	// May       --> January
		 1,	// June      --> January
		 4,	// July      --> April
		 4,	// August    --> April
		 4,	// September --> April
		 7,	// October   --> July
		 7,	// November  --> July
		 7	// December  --> July
	];
	var d = new Date();
	d.setTime(TimeUTC * 1000);
	var CurrentMonth = d.getMonth() + 1;
	var CurrentYear = d.getFullYear();
	var CurrentDay = d.getDate();
	var Month = StartingMonth[CurrentMonth - 1];
	var Year = (Month < CurrentMonth) ? CurrentYear :
										CurrentYear - 1;
	var Row;
	var Col;
	var MonthIdNum = 0;
	var MonthId;

	/***** Draw several months *****/
	Gbl_HTMLContent += '<table style="margin:0 auto; border-spacing:6px;">';

	for (Row = 0;
		 Row < 5;
		 Row++) {
		Gbl_HTMLContent += '<tr>';
		for (Col = 0;
			 Col < 3;
			 Col++) {
			MonthIdNum++;
			MonthId = id + '_month_' + MonthIdNum;

			Gbl_HTMLContent += '<td class="CENTER_TOP" style="width:150px;">';
			DrawMonth (MonthId,FirstDayOfWeek,Year,Month,CurrentMonth,CurrentDay,
						CurrentPlcCod,true,PrintView,CGI,
						FormGoToCalendarParams,FormEventParams);
			Gbl_HTMLContent += '</td>';
			if (++Month == 13) {
				Month = 1;
				Year++;
			}
		}
		Gbl_HTMLContent += '</tr>';
	}
	Gbl_HTMLContent += '</table>';

	document.getElementById(id).innerHTML = Gbl_HTMLContent;
}

/*****************************************************************************/
/***************************** Draw current month ****************************/
/*****************************************************************************/

function DrawCurrentMonth (id,FirstDayOfWeek,TimeUTC,CurrentPlcCod,
						   CGI,FormGoToCalendarParams,FormEventParams) {
	var d = new Date();
	d.setTime(TimeUTC * 1000);
	var Year = d.getFullYear();
	var Month = d.getMonth() + 1;
	var CurrentDay = d.getDate();

	DrawMonth (id,FirstDayOfWeek,Year,Month,Month,CurrentDay,
				CurrentPlcCod,false,false,
				CGI,FormGoToCalendarParams,FormEventParams);
	document.getElementById(id).innerHTML = Gbl_HTMLContent;
}

/*****************************************************************************/
/******************************* Draw a month ********************************/
/*****************************************************************************/
// FirstDayOfWeek == 0 ==> Weeks from Monday to Sunday
// FirstDayOfWeek == 6 ==> Weeks from Sunday to Saturday

function DrawMonth (id,FirstDayOfWeek,YearToDraw,MonthToDraw,CurrentMonth,CurrentDay,
					CurrentPlcCod,DrawingCalendar,PrintView,
					CGI,FormGoToCalendarParams,FormEventParams) {
	var NumDaysMonth = [
		  0,
		 31, //  1: January
		 28, //  2: February
		 31, //  3: Mars
		 30, //  4: April
		 31, //  5: May
		 30, //  6: June
		 31, //  7: July
		 31, //  8: Agoust
		 30, //  9: September
		 31, // 10: October
		 30, // 11: November
		 31, // 12: December
	];
	var Hld_HOLIDAY = 0;
	var Hld_NON_SCHOOL_PERIOD = 1;
	var Week;
	var DayOfWeek; /* 0, 1, 2, 3, 4, 5, 6 */
	var Day;
	var NumDaysInMonth;
	var Yea = YearToDraw;
	var Mon = MonthToDraw;
	var YYYYMMDD;
	var NumHld;
	var ClassForDay; // Class of day depending on type of day
	var TextForDay; // Text associated to a day, for example the name of the holiday
	var NumExamAnnouncement; // Number of exam announcement
	var ResultOfCmpStartDate;
	var ContinueSearching;
	var ThisDayHasEvent;
	var IsToday;
	var FormIdNum = 0;
	var FormId;

	/***** Compute number of day of month for the first box *****/
	/* The initial day of month can be -5, -4, -3, -2, -1, 0, or 1
	   If it's -5 then write 6 boxes of the previous month.
	   If it's -4 then write 5 boxes of the previous month.
	   If it's -3 then write 4 boxes of the previous month.
	   If it's -2 then write 3 boxes of the previous month.
	   If it's -1 then write 2 boxes of the previous month.
	   If it's  0 then write 1 box   of the previous month.
	   If it's  1 then write 0 boxes of the previous month. */

	if ((DayOfWeek = (GetDayOfWeekMondayFirst (Yea,Mon,1) + 7 - FirstDayOfWeek) % 7) == 0)
		Day = 1;
	else {
		if (Mon <= 1) {
			Mon = 12;
			Yea--;
		} else
			Mon--;
		NumDaysInMonth = (Mon == 2) ? GetNumDaysFebruary(Yea) :
									  NumDaysMonth[Mon];
		Day = NumDaysInMonth - DayOfWeek + 1;
	}

	/***** Start of month *****/
	Gbl_HTMLContent += '<div class="MONTH_CONTAINER">';

	/***** Month name *****/
	if (DrawingCalendar)
		Gbl_HTMLContent += '<div class="MONTH">';
	else {
		FormId = id + '_show_calendar';
		Gbl_HTMLContent += '<form method="post" action="' + CGI + '" id="' + FormId + '">' +
							FormGoToCalendarParams +
							'<div class="MONTH">' +
							'<a href="" class="MONTH" onclick="document.getElementById(\'' + FormId +
							'\').submit();return false;">';
	}
	Gbl_HTMLContent += MONTHS_CAPS[MonthToDraw - 1] + ' ' + YearToDraw;
	if (DrawingCalendar)
		Gbl_HTMLContent += '</div>';
	else
		Gbl_HTMLContent += '</a></div></form>';

	/***** Month head: first letter for each day of week *****/
	Gbl_HTMLContent += '<table class="MONTH_TABLE_DAYS">' + '<tr>';
	for (DayOfWeek = 0; DayOfWeek < 7; DayOfWeek++)
		Gbl_HTMLContent += '<td class="' +
						   ((DayOfWeek == 6 - FirstDayOfWeek) ? 'DAY_NO_WRK_HEAD' :
													            'DAY_WRK_HEAD') +
						   '">' +
						   DAYS_CAPS[(DayOfWeek + FirstDayOfWeek) % 7] +
						   '</td>';
	Gbl_HTMLContent += '</tr>';

	/***** Draw every week of the month *****/
	for (Week = 0;
		 Week < 6;
		 Week++) {
		Gbl_HTMLContent += '<tr>';

		/***** Draw every day of the week *****/
		for (DayOfWeek = 0;
			 DayOfWeek < 7;
			 DayOfWeek++) {
			/***** Set class for day being drawn *****/
			ClassForDay = ((Mon == MonthToDraw) ? 'DAY_WRK' :
												  'DAY_WRK_LIGHT');
			TextForDay = '';

			/* Check if day is a holiday or a school day */
			YYYYMMDD = Yea * 10000 + Mon * 100 + Day;
			for (NumHld = 0, ContinueSearching = true;
				 NumHld < Hlds.length && ContinueSearching;
				 NumHld++)
				if (Hlds[NumHld].PlcCod <= 0 ||
					Hlds[NumHld].PlcCod == CurrentPlcCod) {
					if (Hlds[NumHld].StartDate > YYYYMMDD) // List is ordered by start date. If start date is greater than date being drawn, don't continue searching
						ContinueSearching = false;
					else
						// start date <= date being drawn
						switch (Hlds[NumHld].HldTyp) {
							case Hld_HOLIDAY:
								if (Hlds[NumHld].StartDate == YYYYMMDD) {	// If start date == date being drawn
									ClassForDay = ((Mon == MonthToDraw) ? 'DAY_HLD' :
																		  'DAY_HLD_LIGHT');
									TextForDay = Hlds[NumHld].Name;
									ContinueSearching = false;
								}
								break;
							case Hld_NON_SCHOOL_PERIOD:
								if (Hlds[NumHld].EndDate >= YYYYMMDD) {	// If start date <= date being drawn <= end date
									ClassForDay = ((Mon == MonthToDraw) ? 'DAY_NO_WORK' :
																		  'DAY_NO_WORK_LIGHT');
									TextForDay = Hlds[NumHld].Name;
								}
								break;
						}
				}

			/* Day being drawn is sunday? */
			if (DayOfWeek == 6 - FirstDayOfWeek) // All the sundays are holidays
				ClassForDay = (Mon == MonthToDraw) ? 'DAY_HLD' :
													 'DAY_HLD_LIGHT';

			/* Date being drawn is today? */
			IsToday = (Yea == YearToDraw   &&
					   Mon == MonthToDraw  &&
					   Mon == CurrentMonth &&
					   Day == CurrentDay);

			/* Check if day has an exam announcement */
			ThisDayHasEvent = false;
			if (!DrawingCalendar || Mon == MonthToDraw) // If drawing calendar and the month is not the real one, don't draw exam announcements
				for (NumExamAnnouncement = 0;
					NumExamAnnouncement < LstExamAnnouncements.length;
					NumExamAnnouncement++)
					if (Yea == LstExamAnnouncements[NumExamAnnouncement].Year  &&
						Mon == LstExamAnnouncements[NumExamAnnouncement].Month &&
						Day == LstExamAnnouncements[NumExamAnnouncement].Day) {
						ThisDayHasEvent = true;
						if (!PrintView)
							TextForDay = STR_EXAM + ': ' +
										 LstExamAnnouncements[NumExamAnnouncement].Year  + '-' +
										 LstExamAnnouncements[NumExamAnnouncement].Month + '-' +
										 LstExamAnnouncements[NumExamAnnouncement].Day;
						break;
					}

			/***** Write the box with the day *****/
			Gbl_HTMLContent += '<td class="' +
							   ((IsToday && !PrintView) ? (ThisDayHasEvent ? 'TODAY_EVENT' :
																			 'TODAY') :
														  (ThisDayHasEvent ? 'DAY_EVENT' :
																			 'DAY')) +
							   '">';

			/* If day has an exam announcement */
			if (!PrintView && ThisDayHasEvent) {
				FormIdNum++;
				FormId = id + '_event_' + FormIdNum;
				Gbl_HTMLContent +=	'<form method="post" action="' + CGI + '" id="' + FormId + '">' +
									FormEventParams +
									'<div class="' + ClassForDay + '"';
				if (TextForDay.length)
					Gbl_HTMLContent += ' title="' + TextForDay + '"';
				Gbl_HTMLContent +=	'><a href="" class="' + ClassForDay + '"' +
									' onclick="document.getElementById(\'' + FormId +
									'\').submit();return false;">';
			} else {
				Gbl_HTMLContent += '<div class="' + ClassForDay + '"';
				if (TextForDay.length)
					Gbl_HTMLContent += ' title="' + TextForDay + '"';
				Gbl_HTMLContent += '>';
			}

			/* Write the day of month */
			Gbl_HTMLContent += Day;

			/* If day has an exam announcement */
			if (!PrintView && ThisDayHasEvent)
				Gbl_HTMLContent += '</a></div></form>';
			else
				Gbl_HTMLContent += '</div>';

			Gbl_HTMLContent += '</td>';

			/***** Set the next day *****/
			NumDaysInMonth = (Mon == 2) ? GetNumDaysFebruary (Yea) :
										  NumDaysMonth[Mon];
			if (++Day > NumDaysInMonth) {
				if (++Mon > 12) {
					Yea++;
					Mon = 1;
				}
				Day = 1;
			}
		}
		Gbl_HTMLContent += '</tr>';
	}

	/***** End of month *****/
	Gbl_HTMLContent += '</table></div>';
}

/*****************************************************************************/
/* Compute day of the week from a given date (monday as first day of a week) */
/*****************************************************************************/
// Return 0 for monday, 1 for tuesday,... 6 for sunday

function GetDayOfWeekMondayFirst (Year,Month,Day) {
	if (Month <= 2) {
		Month += 12;
		Year--;
	}
	return (((Day +
			(Month * 2) +
			Math.floor (((Month + 1) * 3) / 5) +
			Year +
			Math.floor (Year / 4) -
			Math.floor (Year/100) +
			Math.floor (Year/400) +
			2) % 7) + 5) % 7;
}

/*****************************************************************************/
/****************** Return the number of days of february ********************/
/*****************************************************************************/

function GetNumDaysFebruary (Year) {
	return (GetIfLeapYear (Year) ? 29 : 28);
}

/*****************************************************************************/
/************************* Return true if year is leap ***********************/
/*****************************************************************************/

function GetIfLeapYear (Year) {
	return (Year % 4 == 0) && ((Year % 100 != 0) || (Year % 400 == 0));
}
