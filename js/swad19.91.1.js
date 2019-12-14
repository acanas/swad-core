// swad.js: javascript functions

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.
    Copyright (C) 1999-2019 Antonio Cañas-Vargas
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

/*****************************************************************************/
/************************** Handle match keystrokes **************************/
/*****************************************************************************/

function handleMatchKeys(event) {
	switch (event.key) {
		case 'PageUp':
		case 'ArrowLeft':
		case 'ArrowUp':
			submitForm('backward');
			break;
		case ' ':	// Space
			submitForm('play_pause');
			break;
		case 'PageDown':
		case 'ArrowRight':
		case 'ArrowDown':
			submitForm('forward');
			break;
	}
}

/*****************************************************************************/
/************************ Submit a form given its id *************************/
/*****************************************************************************/

function submitForm(FormId) {
	var Form = document.getElementById(FormId);
	
	if (Form)
		Form.submit();

	return false;
}

/*****************************************************************************/
/*************** Write a date-time in client local time **********************/
/*****************************************************************************/
// id is the id of the HTML element in which date-time will be written
// TimeUTC is the date-time to write in UTC UNIX time format
// DateFormat:
// 	Dat_FORMAT_YYYY_MM_DD		= 0
// 	Dat_FORMAT_DD_MONTH_YYYY	= 1
// 	Dat_FORMAT_MONTH_DD_YYYY	= 2
// separator is HTML code to write between date and time
// StrToday is a string in current language ('today', 'hoy'...)
// WriteDateOnSameDay = false ==> don't write date if it's the same day than the last call
// WriteWeekDay = true ==> write day of the week ('monday', 'tuesday'...)
// WriteHMS = 3 least significant bits for hour, minute and second

function writeLocalDateHMSFromUTC (id,TimeUTC,DateFormat,Separator,StrToday,
									WriteDateOnSameDay,WriteWeekDay,WriteHMS) {
	// HMS: Hour, Minutes, Seconds
	var today = new Date();
	var todayYea = today.getFullYear();
	var todayMon = today.getMonth() + 1;
	var todayDay = today.getDate();
	var d = new Date();
	var WriteDate;
	var WriteTodayStr;
	var Yea;
	var Mon;
	var Day;
	var DayOfWeek;
	var Hou;
	var Min;
	var Sec;
	var StrDate;
	var StrMon;
	var StrDay;
	var StrHou;
	var StrMin;
	var StrSec;

	d.setTime(TimeUTC * 1000);
	Yea = d.getFullYear();
	Mon = d.getMonth() + 1;
	Day = d.getDate();
	
	if (WriteDateOnSameDay)
		WriteDate = true;
	// Check to see if the last date has been initialized
	else if (typeof writeLocalDateHMSFromUTC.lastd == 'undefined')	// Static variable to remember current date for the next call
		// Not initialized
		WriteDate = true;
	else
		WriteDate = (Yea != writeLocalDateHMSFromUTC.lastd.getFullYear()	||
					 Mon != writeLocalDateHMSFromUTC.lastd.getMonth() + 1	||
					 Day != writeLocalDateHMSFromUTC.lastd.getDate())

	writeLocalDateHMSFromUTC.lastd = d;	// Update last date for the next call

	/* Set date */
	StrDate = '';
	if (WriteDate) {
        WriteTodayStr = false;
        if (StrToday != null)
        	if (StrToday.length &&
        		Yea == todayYea &&
        		Mon == todayMon &&
        		Day == todayDay)	// Today
        		WriteTodayStr = true;
        
        if (WriteTodayStr)
        	StrDate = StrToday;
		else
			switch (DateFormat) {
				case 0:	// Dat_FORMAT_YYYY_MM_DD
					StrMon = ((Mon < 10) ? '0' : '') + Mon;
					StrDay = ((Day < 10) ? '0' : '') + Day;
					StrDate = Yea.toString() + '-' + StrMon + '-' + StrDay;
					break;
				case 1:	// Dat_FORMAT_DD_MONTH_YYYY
					StrDate = Day.toString() + '&nbsp;' + MonthsShort[Mon - 1] + '&nbsp;' + Yea.toString();
					break;
				case 2:	// Dat_FORMAT_MONTH_DD_YYYY
					StrDate = MonthsShort[Mon - 1] + '&nbsp;' + Day.toString() + ',&nbsp;' + Yea.toString();
					break;
				default:
					break;
			}
		
		if (WriteWeekDay) {
			DayOfWeek = d.getDay();
			DayOfWeek = (DayOfWeek == 0) ? 6 : DayOfWeek - 1;
			StrDate = StrDate + Separator + DAYS[DayOfWeek];
		}
	}
	else if (WriteWeekDay)
		StrDate = Separator;
	StrDate = StrDate + Separator;

	/* Set HH:MM:SS */
	StrHou = '';
	StrMin = '';
	StrSec = '';
	if (WriteHMS & (1<<2)) {
		// Bit 2 on => Write hour
		Hou = d.getHours();
		StrHou = ((Hou < 10) ?  '0' :  '') + Hou;
		if (WriteHMS & (1<<1)) {
			// Bits 2,1 on => Write minutes
			Min = d.getMinutes();
			StrMin = ((Min < 10) ? ':0' : ':') + Min;
			if (WriteHMS & 1) {
				// Bits 2,1,0 on => Write seconds
				Sec = d.getSeconds();
				StrSec = ((Sec < 10) ? ':0' : ':') + Sec;
			}
		}
	}

	/* Write date and time */
	document.getElementById(id).innerHTML = StrDate + StrHou + StrMin + StrSec;
}

/*****************************************************************************/
/************** Set local date-time form fields from UTC time ****************/
/*****************************************************************************/

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
	var Hou;
	var Min;
	var Sec;

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
			FormMon.options[d.getMonth()].selected = true;
			FormDay.options[d.getDate()-1].selected = true;
			
			Hou = d.getHours();
			FormHou.options[Hou].selected = true;
			
			Min = d.getMinutes();
			for (var i=FormMin.options.length-1; i>=0; i--)
				if (FormMin.options[i].value <= Min) {
					FormMin.options[i].selected = true;
					break;
				}

			if (FormSec) {
				Sec = d.getSeconds();
				FormSec.options[d.getSeconds()].selected = true;
			}
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

/*****************************************************************************/
/************** Set UTC time from local date-time form fields ****************/
/*****************************************************************************/

function setUTCFromLocalDateTimeForm (id) {
	var Seconds = 0;
	var idSecond = document.getElementById(id+'Second');
	if (idSecond)	// id+'Second' present
		Seconds = idSecond.value;
	// Important: set date all at once to avoid problems with different length of months
	var d = new Date(document.getElementById(id+'Year'  ).value,
			         document.getElementById(id+'Month' ).value-1,
			         document.getElementById(id+'Day'   ).value,
			         document.getElementById(id+'Hour'  ).value,
			         document.getElementById(id+'Minute').value,
			         Seconds,
			         0);
	document.getElementById(id+'TimeUTC').value = d.getTime() / 1000;
}

/*****************************************************************************/
/******************* Set form params related to time zones *******************/
/*****************************************************************************/

// Set form param with time difference between UTC time and client local time, in minutes
// For example, if your time zone is GMT+2, -120 will be returned
function setTZ (id) {
	var FormTZ = document.getElementById(id);
	var d = new Date();

	FormTZ.value = d.getTimezoneOffset();
}

// Set form param with the name of the time zone
function setTZname (id) {
	var FormTZname = document.getElementById(id);
	var tz = jstz.determine();		// Determines the time zone of the browser client
	FormTZname.value = tz.name();	// Returns the name of the time zone eg "Europe/Berlin"
}

/*****************************************************************************/
/********* Adjust a date form correcting selector days in the month **********/
/*****************************************************************************/

// The selector of days can start by 1, 2, 3... or by -, 1, 2, 3...
function adjustDateForm (id) {
	var FormYea = document.getElementById(id+'Year' );
	var FormMon = document.getElementById(id+'Month');
	var FormDay = document.getElementById(id+'Day'  );
	var Yea = Number(FormYea.options[FormYea.selectedIndex].value);
	var Mon = Number(FormMon.options[FormMon.selectedIndex].value);
	
	if (Yea != 0 && Mon != 0) {
		var LastDayIndex = FormDay.options.length - 1;
		var LastDayValue = Number(FormDay.options[LastDayIndex].value);
		var SelectedDay = Number(FormDay.options[FormDay.selectedIndex].value);
		var Days = daysInMonth (Mon,Yea);
	
		// If current selected day is out of range...
		if (SelectedDay > Days)
			// Select last day in month
			for (var i=LastDayIndex; i>=0; i--)
				if (FormDay.options[i].value == Days) {
					FormDay.options[i].selected = true;
					break;
				}
	
		// Create new days at the end if necessary
		for (var Day = Number(LastDayValue) + 1; Day <= Days; Day++) {
			var opt = document.createElement('option');
			opt.value = opt.text = Day;
			FormDay.add(opt);
		}
	
		// Remove days from the end if necessary
		for (var i=LastDayIndex; i>=0; i--)
			if (FormDay.options[i].value > Days)
				FormDay.options[i] = null;
			else
				break;
	}
}

/*****************************************************************************/
/*********************** Get number of days in a month ***********************/
/*****************************************************************************/

function daysInMonth (month, year) {	// Month is 1 based
	  return new Date(year, month, 0).getDate();	// 0 is the last day of previous month
}

/*****************************************************************************/
/****************** Set a date range form to a specific day ******************/
/*****************************************************************************/

// Set a date range form to yesterday
function setDateToYesterday (idStart,idEnd) {
	var d = new Date();

	d.setTime (d.getTime () - 24 * 60 * 60 * 1000);	// Today - 1 day
	setDateRange(idStart,idEnd,d);
}

// Set a date range form to today
function setDateToToday (idStart,idEnd) {
	var d = new Date();

	setDateRange(idStart,idEnd,d);
}

function setDateRange (idStart,idEnd,d) {
	var FormYea;
	var Yea = d.getFullYear();
	var Mon = d.getMonth() + 1;
	var Day = d.getDate();

	FormYea = document.getElementById(idStart+'Year');
	for (var i=0; i<FormYea.options.length; i++)
		if (FormYea.options[i].value == Yea) {
			FormYea.options[i].selected = true;
			break;
		}
	document.getElementById(idStart+'Month').options[Mon-1].selected = true;
	adjustDateForm (idStart);	// Adjust date form correcting days in the month
	document.getElementById(idStart+'Day').options[Day-1].selected = true;
	setHMSTo000000(idStart);

	FormYea = document.getElementById(idEnd+'Year');
	for (var i=0; i<FormYea.options.length; i++)
		if (FormYea.options[i].value == Yea) {
			FormYea.options[i].selected = true;
			break;
		}
	document.getElementById(idEnd+'Month').options[Mon-1].selected = true;
	adjustDateForm (idEnd);		// Adjust date form correcting days in the month
	document.getElementById(idEnd+'Day').options[Day-1].selected = true;
	setHMSTo235959(idEnd);
}

/*****************************************************************************/
/************ Set hour, minute and second in a form to 00:00:00 **************/
/*****************************************************************************/

function setHMSTo000000 (id) {
	document.getElementById(id+'Hour'  ).options[0].selected = true;
	document.getElementById(id+'Minute').options[0].selected = true;
	document.getElementById(id+'Second').options[0].selected = true;
	setUTCFromLocalDateTimeForm(id);
}

/*****************************************************************************/
/************ Set hour, minute and second in a form to 23:59:59 **************/
/*****************************************************************************/

function setHMSTo235959 (id) {
	document.getElementById(id+'Hour'  ).options[23].selected = true;
	document.getElementById(id+'Minute').options[59].selected = true;
	document.getElementById(id+'Second').options[59].selected = true;
	setUTCFromLocalDateTimeForm(id);
}

/*****************************************************************************/
/********** Write clock in client local time updated every minute ************/
/*****************************************************************************/

// Global variables used in writeLocalClock()
var secondsSince1970UTC;

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

	document.getElementById('current_month_txt').innerHTML = Months[Mon];
	document.getElementById('current_day_txt').innerHTML = Day;
	document.getElementById('current_time').innerHTML = Hou + ':' + StrMin;
}

// Global variables used in writeClockConnected()
var NumUsrsCon;
var ListSeconds = [];
var countClockConnected = 0;
   
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
					PrintableClock = H + ':' + StrM + '&prime;' + StrS + '&Prime;';
				} else if (M != 0) {
					StrS = ((S < 10) ? '0' : '') + S;
					PrintableClock = M + '&prime;' + StrS + '&Prime;';
				} else
					PrintableClock = S + '&Prime;';
				BoxClock.innerHTML = PrintableClock;
			}
		}
	}
	setTimeout('writeClockConnected()',1000);	// refresh after 1 second
}

/*****************************************************************************/
/************* Automatic refresh of connected users using AJAX ***************/
/*****************************************************************************/

// This function must be called from time to time
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

// Receive and show connected users data
function readConnUsrsData () {
	if (objXMLHttpReqCon.readyState == 4) {	// Check if data have been received
		if (objXMLHttpReqCon.status == 200) {
			var endOfDelay   = objXMLHttpReqCon.responseText.indexOf('|',0);				// Get separator position
			var endOfNotif   = objXMLHttpReqCon.responseText.indexOf('|',endOfDelay  + 1);	// Get separator position
			var endOfGblCon  = objXMLHttpReqCon.responseText.indexOf('|',endOfNotif  + 1);	// Get separator position
			var endOfCrsCon  = objXMLHttpReqCon.responseText.indexOf('|',endOfGblCon + 1);	// Get separator position
			var endOfNumUsrs = objXMLHttpReqCon.responseText.indexOf('|',endOfCrsCon + 1);	// Get separator position

			var delay = parseInt(objXMLHttpReqCon.responseText.substring(0,endOfDelay));			// Get refresh delay
			var htmlNotif  = objXMLHttpReqCon.responseText.substring(endOfDelay  + 1,endOfNotif);	// Get HTML code for new notifications
			var htmlGblCon = objXMLHttpReqCon.responseText.substring(endOfNotif  + 1,endOfGblCon);	// Get HTML code for connected
			var htmlCrsCon = objXMLHttpReqCon.responseText.substring(endOfGblCon + 1,endOfCrsCon);	// Get HTML code for course connected
			var NumUsrsStr = objXMLHttpReqCon.responseText.substring(endOfCrsCon + 1,endOfNumUsrs);	// Get number of users
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
						endOfUsr = objXMLHttpReqCon.responseText.indexOf('|',startOfUsr + 1);
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

/*****************************************************************************/
/********** Automatic refresh of current match question using AJAX ***********/
/*****************************************************************************/

//  This function must be called from time to time
var objXMLHttpReqMchStd = false;
function refreshMatchStd () {
	objXMLHttpReqMchStd = AJAXCreateObject();
	if (objXMLHttpReqMchStd) {
		var RefreshParams = RefreshParamNxtActMch + '&' +
							RefreshParamMchCod + '&' +
							RefreshParamIdSes;

		objXMLHttpReqMchStd.onreadystatechange = readMatchStdData;	// onreadystatechange must be lowercase
		objXMLHttpReqMchStd.open('POST',ActionAJAX,true);
		objXMLHttpReqMchStd.setRequestHeader('Content-Type','application/x-www-form-urlencoded');
		objXMLHttpReqMchStd.send(RefreshParams);
	}
}

function readMatchStdData () {
	if (objXMLHttpReqMchStd.readyState == 4) {	// Check if data have been received
		if (objXMLHttpReqMchStd.status == 200) {
			var htmlMatch = objXMLHttpReqMchStd.responseText;	// Get HTML code

			var div = document.getElementById('match');	// Access to refreshable DIV
			if (div)
				div.innerHTML = htmlMatch;				// Update DIV content
			// Global delay variable is set initially in swad-core
			setTimeout('refreshMatchStd()',delayMatch);
		}
	}
}

/*****************************************************************************/
/**** Automatic refresh of left part of current match question using AJAX ****/
/*****************************************************************************/

//This function must be called from time to time
var objXMLHttpReqMchTch = false;
function refreshMatchTch () {
	objXMLHttpReqMchTch = AJAXCreateObject();
	if (objXMLHttpReqMchTch) {
		var RefreshParams = RefreshParamNxtActMch + '&' +
							RefreshParamMchCod + '&' +
							RefreshParamIdSes;

		objXMLHttpReqMchTch.onreadystatechange = readMatchTchData;	// onreadystatechange must be lowercase
		objXMLHttpReqMchTch.open('POST',ActionAJAX,true);
		objXMLHttpReqMchTch.setRequestHeader('Content-Type','application/x-www-form-urlencoded');
		objXMLHttpReqMchTch.send(RefreshParams);
	}
}

function readMatchTchData () {
	if (objXMLHttpReqMchTch.readyState == 4) {	// Check if data have been received
		if (objXMLHttpReqMchTch.status == 200) {
			var endOfId = objXMLHttpReqMchTch.responseText.indexOf('|',0           );	// Get separator position
			var endOfEv = objXMLHttpReqMchTch.responseText.indexOf('|',endOfId  + 1);	// Get separator position

			var id   = objXMLHttpReqMchTch.responseText.substring(0          ,endOfId);	// Get id
			var ev   = objXMLHttpReqMchTch.responseText.substring(endOfId + 1,endOfEv);	// Get ev ('0' / '1')
			var html = objXMLHttpReqMchTch.responseText.substring(endOfEv + 1);			// Get HTML code

			var div = document.getElementById(id);	// Access to refreshable DIV
			if (div) {
				div.innerHTML = html;				// Update DIV content
			
				if (parseInt(ev)) {	// 0 / 1
					// Scripts in div got via AJAX are not executed ==> execute them
					evalScriptsInElem (div);

					// Process mathematics; see http://docs.mathjax.org/en/latest/advanced/typeset.html
					MathJax.Hub.Queue(["Typeset",MathJax.Hub,div]);
				}
			}
			
			// Global delay variable is set initially in swad-core
			setTimeout('refreshMatchTch()',delayMatch);
		}
	}
}

/*****************************************************************************/
/****************** Update match control area using AJAX *********************/
/*****************************************************************************/

// This function is called when user submit a form inside two parent divs
function updateMatchTch (id,Params) {
    var objXMLHttp = false;

	objXMLHttp = AJAXCreateObject ();
	if (objXMLHttp) {
		/* Send request to server */
		objXMLHttp.onreadystatechange = function() {	// onreadystatechange must be lowercase
			if (objXMLHttp.readyState == 4) {			// Check if data have been received
				if (objXMLHttp.status == 200)
					if (id) {
						var div = document.getElementById(id);			// Access to DIV
						if (div)
							div.innerHTML = objXMLHttp.responseText;	// Update DIV content
					}
			}
		};
		objXMLHttp.open('POST',ActionAJAX,true);
		objXMLHttp.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');
		objXMLHttp.send(Params);
	}
}

/*****************************************************************************/
/**************** Automatic refresh of last clicks using AJAX ****************/
/*****************************************************************************/

//  This function must be called from time to time
var objXMLHttpReqLog = false;
function refreshLastClicks () {
	objXMLHttpReqLog = AJAXCreateObject();
	if (objXMLHttpReqLog) {
		var RefreshParams = RefreshParamNxtActLstClk + '&' +
							RefreshParamIdSes + '&' +
							RefreshParamCrsCod;

		objXMLHttpReqLog.onreadystatechange = readLastClicksData;	// onreadystatechange must be lowercase
		objXMLHttpReqLog.open('POST',ActionAJAX,true);
		objXMLHttpReqLog.setRequestHeader('Content-Type','application/x-www-form-urlencoded');
		objXMLHttpReqLog.send(RefreshParams);
	}
}

function readLastClicksData () {
	if (objXMLHttpReqLog.readyState == 4) {	// Check if data have been received
		if (objXMLHttpReqLog.status == 200) {
			var endOfDelay = objXMLHttpReqLog.responseText.indexOf('|',0);	// Get separator position

			var delay = parseInt(objXMLHttpReqLog.responseText.substring(0,endOfDelay));	// Get refresh delay
			var htmlLastClicks = objXMLHttpReqLog.responseText.substring(endOfDelay + 1);	// Get HTML code for last clicks

			var divLastClicks = document.getElementById('lastclicks');			// Access to last click DIV
			if (divLastClicks)
				divLastClicks.innerHTML = htmlLastClicks;				// Update global connected DIV
			if (delay > 200)	// If refresh slower than 1 time each 0.2 seconds, do refresh; else abort
				setTimeout('refreshLastClicks()',delay);
		}
	}
}

/*****************************************************************************/
/*** Automatically refresh new publications in social timeline using AJAX ****/
/*****************************************************************************/

// This function must be called from time to time
var objXMLHttpReqNewTL = false;
function refreshNewTL () {
	objXMLHttpReqNewTL = AJAXCreateObject();
	if (objXMLHttpReqNewTL) {
		var RefreshParams = RefreshParamNxtActNewPub + '&' +
							RefreshParamIdSes + '&' +
							RefreshParamWho;

		objXMLHttpReqNewTL.onreadystatechange = readNewTimelineData;	// onreadystatechange must be lowercase
		objXMLHttpReqNewTL.open('POST',ActionAJAX,true);
		objXMLHttpReqNewTL.setRequestHeader('Content-Type','application/x-www-form-urlencoded');
		objXMLHttpReqNewTL.send(RefreshParams);
	}
}

// Receive and show new social timeline data
function readNewTimelineData () {
	if (objXMLHttpReqNewTL.readyState == 4) {	// Check if data have been received
		if (objXMLHttpReqNewTL.status == 200) {

			var justNowTimeline = document.getElementById('just_now_timeline_list');// Access to UL for the just received timeline
			if (justNowTimeline) {
				justNowTimeline.innerHTML = objXMLHttpReqNewTL.responseText;	// Update list of publications in just now timeline		

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
			
			// Global delay variable is set initially in swad-core
			delayNewTL += 1000;	// Increase 1 second
			setTimeout('refreshNewTL()',delayNewTL);
		}
	}
}

/*****************************************************************************/
/* View new publication in timeline by moving new timeline to top of timeline */
/*****************************************************************************/

function moveNewTimelineToTimeline () {
	// Move all the LI elements in UL 'new_timeline_list' to the top of UL 'timeline_list'
	var newTimeline = document.getElementById('new_timeline_list');
	var countNewTimeline = newTimeline.childNodes.length;
	if (countNewTimeline) {
		var timeline = document.getElementById("timeline_list");
		for (var i=0; i<countNewTimeline; i++) {
			timeline.insertBefore(newTimeline.lastChild, timeline.childNodes[0]);
			timeline.childNodes[0].className += " TL_NEW_PUB";
		}
    }

	// Reset number of new posts after moving
	var viewNewPostsCount = document.getElementById('view_new_posts_count');
	viewNewPostsCount.innerHTML = 0;

	// Hide link to view new posts after moving
	var viewNewPostsContainer = document.getElementById('view_new_posts_container');
	viewNewPostsContainer.style.display = 'none';
}

/*****************************************************************************/
/********** Refresh old publications in social timeline using AJAX ***********/
/*****************************************************************************/

// This function is called when user clicks in link
var objXMLHttpReqOldTL = false;
function refreshOldTimeline () {
	objXMLHttpReqOldTL = AJAXCreateObject ();
	if (objXMLHttpReqOldTL) {
		var RefreshParams = RefreshParamNxtActOldPub + '&' +
							RefreshParamIdSes;
		if (typeof RefreshParamUsr !== 'undefined') {
			if (RefreshParamUsr.length)
				RefreshParams += '&' + RefreshParamUsr;
		}
		if (typeof RefreshParamWho !== 'undefined') {
			if (RefreshParamWho.length)
				RefreshParams += '&' + RefreshParamWho;
		}

		objXMLHttpReqOldTL.onreadystatechange = readOldTimelineData;	// onreadystatechange must be lowercase
		objXMLHttpReqOldTL.open('POST',ActionAJAX,true);
		objXMLHttpReqOldTL.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');
		objXMLHttpReqOldTL.send(RefreshParams);
	}
}

// Receive and show old social timeline data
function readOldTimelineData () {
	if (objXMLHttpReqOldTL.readyState == 4) {	// Check if data have been received
		if (objXMLHttpReqOldTL.status == 200) {
			var oldTimeline = document.getElementById('old_timeline_list');	// Access to UL with the old timeline
			if (oldTimeline) {
				oldTimeline.innerHTML = objXMLHttpReqOldTL.responseText;		// Fill list of publications in old timeline

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
						
					// There may be more publications
					document.getElementById('get_old_timeline').style.display='';			// Show icon to be hidden on click
					document.getElementById('getting_old_timeline').style.display='none';	// Hide icon to be shown on click
				}
				else	// No old publications retrieved, so we have reached the oldest publication
						// Hide container with link to get old publications
					document.getElementById("view_old_posts_container").style.display = 'none';
			}
		}
	}
}

/*****************************************************************************/
/********** Scripts got via AJAX are not executed ==> execute them ***********/
/*****************************************************************************/

function evalScriptsInElem (elem) {
	var scrs = elem.getElementsByTagName("script");
	var s;
	for (var i=0; i<scrs.length; i++) {
		s = scrs[i].childNodes[0].nodeValue;
		if (s != null) eval(s);
	}
}

/*****************************************************************************/
/************* Show hidden comments social timeline using AJAX ***************/
/*****************************************************************************/

//This function is called when user submit a form inside two parent divs
function updateDivHiddenComments (form,Params) {
    var objXMLHttp = false;
	var id = form.parentNode.parentNode.id;

	objXMLHttp = AJAXCreateObject ();
	if (objXMLHttp) {
		/* Send request to server */
		objXMLHttp.onreadystatechange = function() {	// onreadystatechange must be lowercase
			if (objXMLHttp.readyState == 4) {			// Check if data have been received
				if (objXMLHttp.status == 200)
					if (id) {
						var div = document.getElementById(id);			// Access to DIV
						if (div) {
							div.innerHTML = objXMLHttp.responseText;	// Update DIV content
						
							// Scripts in div got via AJAX are not executed ==> execute them
							evalScriptsInElem (div);
	
							// Process mathematics; see http://docs.mathjax.org/en/latest/advanced/typeset.html
							MathJax.Hub.Queue(["Typeset",MathJax.Hub,div]);
						}
					}
			}
		};
		objXMLHttp.open('POST',ActionAJAX,true);
		objXMLHttp.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');
		objXMLHttp.send(Params);
	}
}

/*****************************************************************************/
/********** Update fav or share area in social timeline using AJAX ***********/
/*****************************************************************************/

// This function is called when user submit a form just inside a parent div
function updateDivLockUnlockProject (form,Params) {
    var objXMLHttp = false;
	var id = form.parentNode.id;

	objXMLHttp = AJAXCreateObject ();
	if (objXMLHttp) {
		/* Send request to server */
		objXMLHttp.onreadystatechange = function() {	// onreadystatechange must be lowercase
			if (objXMLHttp.readyState == 4) {			// Check if data have been received
				if (objXMLHttp.status == 200)
					if (id) {
						var div = document.getElementById(id);			// Access to DIV
						if (div)
							div.innerHTML = objXMLHttp.responseText;	// Update DIV content
					}
			}
		};
		objXMLHttp.open('POST',ActionAJAX,true);
		objXMLHttp.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');
		objXMLHttp.send(Params);
	}
}

/*****************************************************************************/
/************** Update lock/unlock area in project using AJAX ****************/
/*****************************************************************************/

// This function is called when user submit a form inside two parent divs
function updateDivFaversSharers (form,Params) {
    var objXMLHttp = false;
	var id = form.parentNode.parentNode.id;

	objXMLHttp = AJAXCreateObject ();
	if (objXMLHttp) {
		/* Send request to server */
		objXMLHttp.onreadystatechange = function() {	// onreadystatechange must be lowercase
			if (objXMLHttp.readyState == 4) {			// Check if data have been received
				if (objXMLHttp.status == 200)
					if (id) {
						var div = document.getElementById(id);			// Access to DIV
						if (div)
							div.innerHTML = objXMLHttp.responseText;	// Update DIV content
					}
			}
		};
		objXMLHttp.open('POST',ActionAJAX,true);
		objXMLHttp.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');
		objXMLHttp.send(Params);
	}
}

/*****************************************************************************/
/*********************** Expand textarea when focus **************************/
/*****************************************************************************/

// Called from a textarea onfocus
function expandTextarea (textareaElem,idButton,rows) {
	textareaElem.rows = rows;
	document.getElementById(idButton).style.display = '';
}

/*****************************************************************************/
/**************************** Create AJAX object *****************************/
/*****************************************************************************/

// try is unknown in earlier versions of Netscape, but works in IE5
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

/*****************************************************************************/
/****************************** Media uploader *******************************/
/*****************************************************************************/

function mediaActivateMediaUploader (id) {
	document.getElementById (id + '_med_ico').style.display = 'none';
	document.getElementById (id + '_med_upl').style.display = '';
}

function mediaClickOnActivateUpload (id) {
	var par_upl	= document.getElementById (id + '_par_upl');

	if (par_upl.disabled) {				// Click on highlighted icon
		//  par_upl already got
		var par_you = document.getElementById (id + '_par_you');
		var par_emb = document.getElementById (id + '_par_emb');

		var ico_upl	= document.getElementById (id + '_ico_upl');
		var ico_you	= document.getElementById (id + '_ico_you');
		var ico_emb	= document.getElementById (id + '_ico_emb');

		var fil		= document.getElementById (id + '_fil');
		var url		= document.getElementById (id + '_url');
		var tit		= document.getElementById (id + '_tit');

		// Enable embed, disable others
	    par_upl.disabled = false;		// Enable upload
	    par_you.disabled = true;		// Disable youtube
	    par_emb.disabled = true;		// Disable embed

		ico_upl.className = 'PREF_ON';	// Highlighted upload icon
		ico_you.className = 'PREF_OFF';	// Normal youtube icon
		ico_emb.className = 'PREF_OFF';	// Normal embed icon

		fil.style.display  = '';		// Show file input
		fil.disabled = false;			// Enable file input

		url.style.display = '';			// Show URL input
		url.disabled = false;			// Enable URL input

		tit.style.display = '';			// Show title input
		tit.disabled = false;			// Enable title input
	}
	else								// Click on shadowed icon
	    mediaDisableAll (id);
}

function mediaClickOnActivateYoutube (id) {
	var par_you	= document.getElementById (id + '_par_you');

	if (par_you.disabled) {				// Click on highlighted icon
		var par_upl = document.getElementById (id + '_par_upl');
		//  par_you already got
		var par_emb = document.getElementById (id + '_par_emb');

		var ico_upl	= document.getElementById (id + '_ico_upl');
		var ico_you	= document.getElementById (id + '_ico_you');
		var ico_emb	= document.getElementById (id + '_ico_emb');

		var fil		= document.getElementById (id + '_fil');
		var url		= document.getElementById (id + '_url');
		var tit		= document.getElementById (id + '_tit');

		// Enable youtube, disable others
	    par_upl.disabled = true;		// Disable upload
	    par_you.disabled = false;		// Enable youtube
	    par_emb.disabled = true;		// Disable embed

		ico_upl.className = 'PREF_OFF';	// Normal upload icon
		ico_you.className = 'PREF_ON';	// Highlighted youtube icon
		ico_emb.className = 'PREF_OFF';	// Normal embed icon

		fil.style.display  = 'none';	// Hide file input
		fil.disabled = true;			// Disable file input

		url.style.display = '';			// Show URL input
		url.disabled = false;			// Enable URL input

		tit.style.display = 'none';		// Hide title input
		tit.disabled = true;			// Disable title input
	}
	else								// Click on shadowed icon
	    mediaDisableAll (id);
}

function mediaClickOnActivateEmbed (id) {
	var par_emb	= document.getElementById (id + '_par_emb');

	if (par_emb.disabled) {				// Click on highlighted icon
		var par_upl = document.getElementById (id + '_par_upl');
		var par_you = document.getElementById (id + '_par_you');
		//  par_emb already got

		var ico_upl	= document.getElementById (id + '_ico_upl');
		var ico_you	= document.getElementById (id + '_ico_you');
		var ico_emb	= document.getElementById (id + '_ico_emb');

		var fil		= document.getElementById (id + '_fil');
		var url		= document.getElementById (id + '_url');
		var tit		= document.getElementById (id + '_tit');

		// Enable embed, disable others
	    par_upl.disabled = true;		// Disable upload
	    par_you.disabled = true;		// Disable youtube
	    par_emb.disabled = false;		// Enable embed

		ico_upl.className = 'PREF_OFF';	// Normal upload icon
		ico_you.className = 'PREF_OFF';	// Normal youtube icon
		ico_emb.className = 'PREF_ON';	// Highlighted embed icon

		fil.style.display  = 'none';	// Hide file input
		fil.disabled = true;			// Disable file input

		url.style.display = '';			// Show URL input
		url.disabled = false;			// Enable URL input

		tit.style.display = 'none';		// Hide title input
		tit.disabled = true;			// Disable title input
	}
	else								// Click on shadowed icon
	    mediaDisableAll (id);
}

function mediaDisableAll (id) {
	var par_upl = document.getElementById (id + '_par_upl');
	var par_you	= document.getElementById (id + '_par_you');
	var par_emb	= document.getElementById (id + '_par_emb');

	var ico_upl	= document.getElementById (id + '_ico_upl');
	var ico_you	= document.getElementById (id + '_ico_you');
	var ico_emb	= document.getElementById (id + '_ico_emb');

	var fil		= document.getElementById (id + '_fil');
	var url		= document.getElementById (id + '_url');
	var tit		= document.getElementById (id + '_tit');

    par_upl.disabled = true;		// Disable upload
    par_you.disabled = true;		// Disable youtube
    par_emb.disabled = true;		// Disable embed

	ico_upl.className = 'PREF_OFF';	// Normal upload icon
	ico_you.className = 'PREF_OFF';	// Normal youtube icon
	ico_emb.className = 'PREF_OFF';	// Normal embed icon

	fil.style.display = 'none';		// Hide file input
	fil.disabled = true;			// Disable file input

	url.style.display = 'none';		// Hide URL input
	url.disabled = true;			// Disable URL input

	tit.style.display = 'none';		// Hide title input
	tit.disabled = true;			// Disable title input
}

/*****************************************************************************/
/********************** Change display of animated GIF ***********************/
/*****************************************************************************/
// elem should be a DIV with two children: IMG and SPAN

function toggleOnGIF (elem, animatedURL) {
	var c = elem.children;

	for (var i = 0; i < c.length; i++) {
		var child = c[i];

		if (child.nodeName == 'IMG')
			/* Display animated image */
			child.src = animatedURL;
		else if (child.nodeName == 'SPAN')
			/* Hide the span element showing "GIF" */
			child.style.display = 'none';
	}
}

function toggleOffGIF (elem, staticURL) {
	var c = elem.children;

	for (var i = 0; i < c.length; i++) {
		var child = c[i];

		if (child.nodeName == 'IMG')
			/* Display static image */
			child.src = staticURL;
		else if (child.nodeName == 'SPAN')
			/* Make visible the span element showing "GIF" */
			child.style.display = '';
	}
}

/*****************************************************************************/
/******* Change display of edition of a new comment in a social note *********/
/*****************************************************************************/

function toggleNewComment (id) {
	var iconDiv = document.getElementById (id + '_ico');

	if (iconDiv.className == 'TL_ICO_COM_OFF')
		iconDiv.className = 'TL_ICO_COM_ON';
	else
		iconDiv.className = 'TL_ICO_COM_OFF';
	toggleDisplay(id);
}

/*****************************************************************************/
/*************** Change display of comments in a social note *****************/
/*****************************************************************************/

function toggleComments (notID) {
	toggleDisplay('con_' + notID);
	toggleDisplay('com_' + notID);
	toggleDisplay('exp_' + notID);
}

/*****************************************************************************/
/************************ Change display of a project ************************/
/*****************************************************************************/

function toggleProject (prjID) {
	toggleDisplay('prj_exp_' + prjID);
	toggleDisplay('prj_con_' + prjID);
	toggleDisplay('prj_pro_' + prjID);
	toggleDisplay('prj_dsc_' + prjID);
	toggleDisplay('prj_knw_' + prjID);
	toggleDisplay('prj_mtr_' + prjID);
	toggleDisplay('prj_url_' + prjID);
}

/*****************************************************************************/
/********************** Change display of a test answer **********************/
/*****************************************************************************/

function toggleAnswer (option) {
	toggleDisplay('ans_' + option);
	toggleDisplay('con_' + option);
	toggleDisplay('exp_' + option);
}

/*****************************************************************************/
/************* Change display of an element (hidden or visible) **************/
/*****************************************************************************/

function toggleDisplay (elementID) {
	var element = document.getElementById (elementID);
	var stl;

	if (element) {
		stl = element.style;
		stl.display = (stl.display === 'none') ? '' : 'none';
	}
}

/*****************************************************************************/
/************************ Zoom a user's photograph ***************************/
/*****************************************************************************/

// idCaption must be the id of a hidden div with the caption in innerHTML
// (this allows showing &prime; and &Prime;)
function zoom (img,urlPhoto,idCaption) {
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
	document.getElementById('zoomTxt').innerHTML = document.getElementById(idCaption).innerHTML;
}

// Exit from zooming a user's photograph
function noZoom () {
	var xPos = -(187 + 15);
	var yPos = -(250 + 15 + 110);
	document.getElementById('zoomTxt').innerHTML = '';
	document.getElementById('zoomImg').src='/swad/icon/usr_bl.jpg';
	document.getElementById('zoomLyr').style.left = xPos + 'px';
	document.getElementById('zoomLyr').style.top = yPos + 'px';
}

/*****************************************************************************/
/****************** Related to radio and checkbox elements *******************/
/*****************************************************************************/

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

/*****************************************************************************/
/******************************* Related to tests ****************************/
/*****************************************************************************/

// Change text of a test tag
function changeTxtTag (NumTag) {
	var Sel = document.getElementById('SelDesc' + NumTag);

	document.getElementById('TagTxt' + NumTag).value = Sel.options[Sel.selectedIndex].value;
}

// Change selectors of test tags
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
		if (elem.name == ('MedAct' + i) ||
			elem.name == ('MedFil' + i) ||
			elem.name == ('MedTit' + i) ||
			elem.name == ('MedURL' + i))
			elem.disabled = isDisabled;
}

/*****************************************************************************/
/********** Selection of statistics about hits in current course *************/
/*****************************************************************************/

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
/************************* Draw an academic calendar *************************/
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
	Gbl_HTMLContent += '<table class="CALENDAR">';

	for (Row = 0;
		 Row < 5;
		 Row++) {
		Gbl_HTMLContent += '<tr>';
		for (Col = 0;
			 Col < 3;
			 Col++) {
			MonthIdNum++;
			MonthId = id + '_month_' + MonthIdNum;

			Gbl_HTMLContent += '<td class="CT" style="width:150px;">';
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
	var Hld_HOLIDAY = 0;
	var Hld_NON_SCHOOL_PERIOD = 1;
	var Yea = YearToDraw;
	var Mon = MonthToDraw;
	var Day;
	var ExaCod;
	var StrMon;
	var StrDay;
	var StrDate;
	var Week;
	var DayOfWeek; /* 0, 1, 2, 3, 4, 5, 6 */
	var NumDaysInMonth;
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
		NumDaysInMonth = daysInMonth (Mon,Yea);
		Day = NumDaysInMonth - DayOfWeek + 1;
	}

	/***** Start of month *****/
	Gbl_HTMLContent += '<div class="MONTH_CONT">';

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
	Gbl_HTMLContent += Months[MonthToDraw - 1] + ' ' + YearToDraw;
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
						if (!PrintView) {
							ExaCod = LstExamAnnouncements[NumExamAnnouncement].ExaCod;
							StrMon = ((Mon < 10) ? '0' : '') + Mon;
							StrDay = ((Day < 10) ? '0' : '') + Day;
							StrDate = Yea + StrMon + StrDay;	// YYYYMMDD
							TextForDay = STR_EXAM;
						}
						break;	// Only last exam in the day (list is ordered from more recent to older)
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
				Gbl_HTMLContent +=	'<form method="post" action="' +
									CGI + '#' + 'cod_' +
									ExaCod +
									'" id="' + FormId + '">' +
									FormEventParams +
									'<input type="hidden" name=\"Date\" value="' +
									StrDate +
									'" /><div class="' + ClassForDay + '"';
				if (TextForDay.length)
					Gbl_HTMLContent += ' title="' + TextForDay + '"';
				Gbl_HTMLContent +=	'><a href="" class="' + ClassForDay +
									'" onclick="document.getElementById(\'' + FormId +
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
			NumDaysInMonth = daysInMonth (Mon,Yea);
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
/** Compute day of the week from a given date (first day of a week: monday) **/
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
/************* Copy message subject and content to hidden fields *************/
/*****************************************************************************/

function CopyMessageToHiddenFields () {
	var Subject = document.getElementById('MsgSubject');
	var Content = document.getElementById('MsgContent');
	var Subjects = document.getElementsByName("HiddenSubject");
	var Contents = document.getElementsByName("HiddenContent");
	var i;

	if (Subject)
		for (i = 0; i < Subjects.length; i++)
			if (Subjects[i].type == "hidden")
				Subjects[i].value = Subject.value;

	if (Content)
		for (i = 0; i < Contents.length; i++)
			if (Contents[i].type == "hidden")
				Contents[i].value = Content.value;
}

/*****************************************************************************/
/**************** Animate icon "recycle" when click on a link ****************/
/*****************************************************************************/

function AnimateIcon (NumForm) {
	document.getElementById('update_'+NumForm).style.display='none';	// Icon to be hidden on click
	document.getElementById('updating_'+NumForm).style.display='';		// Icon to be shown on click
}

/*****************************************************************************/
/************************ Expand / contract a folder *************************/
/*****************************************************************************/
/*
Example of rows:
	idParent	= 'file_browser_1_1_2_1'
Children ids:
	idRow		= 'file_browser_1_1_2_1_1'
	idRow		= 'file_browser_1_1_2_1_2'
	idRow		= 'file_browser_1_1_2_1_2_1'
*/
function ExpandFolder (idParent) {
	var parent				= document.getElementById(idParent);
	var iconToExpand		= document.getElementById('expand_'			+ idParent);
	var iconToContract		= document.getElementById('contract_'		+ idParent);
	var iconFolderClosed	= document.getElementById('folder_closed_'	+ idParent);
	var iconFolderOpen		= document.getElementById('folder_open_'	+ idParent);

	if (parent.dataset.folder == 'contracted') {
		ExpandChildren (idParent);
		parent.dataset.folder = 'expanded';
		iconToExpand.style.display		= 'none';
		iconToContract.style.display	= '';
		iconFolderClosed.style.display	= 'none';
		iconFolderOpen.style.display	= '';
	}
}

function ExpandChildren (idParent) {
	var parent = document.getElementById(idParent);
	var rows = document.getElementsByTagName('tr');
	var lengthIdParent = idParent.length;
	var i;
	var row;
	var idRow;
	var filename;

	for (i = 0; i < rows.length; i++) {
		row = rows[i];
		if (row != parent) {
			idRow = row.id;
			if (idRow.indexOf(idParent) == 0) {				// row.id starts by idParent, so it's a child
				/* Get filename */
				filename = idRow.substring(lengthIdParent + 1);

				if (filename.indexOf("_") < 0) {			// this child hangs directly from parent
					/* Unhide row */
					row.style.display = '';					// unhide

					/* Expand children */
					if (row.dataset.folder == 'expanded')	// this child is an expanded folder
						ExpandChildren (idRow);				// recursive call
				}
			}
		}
	}
}

function ContractFolder (idParent) {
	var parent				= document.getElementById(idParent);
	var iconToExpand		= document.getElementById('expand_'			+ idParent);
	var iconToContract		= document.getElementById('contract_'		+ idParent);
	var iconFolderClosed	= document.getElementById('folder_closed_'	+ idParent);
	var iconFolderOpen		= document.getElementById('folder_open_'	+ idParent);

	if (parent.dataset.folder == 'expanded') {
		ContractChildren (idParent);
		parent.dataset.folder = 'contracted';
		iconToContract.style.display	= 'none';
		iconToExpand.style.display		= '';
		iconFolderOpen.style.display	= 'none';
		iconFolderClosed.style.display	= '';
	}
}

function ContractChildren (idParent) {
	var parent = document.getElementById(idParent);
	var rows = document.getElementsByTagName('tr');
	var i;
	var row;

	for (i = 0; i < rows.length; i++) {
		row = rows[i];
		if (row != parent)
			if (row.id.indexOf(idParent) == 0)		// row.id starts by idParent, so it's a child
				/* Unhide row */
				row.style.display = 'none';			// hide
	}
}
