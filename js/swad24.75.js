// swad.js: javascript functions

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.
    Copyright (C) 1999-2023 Antonio Cañas-Vargas
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
var actionAJAX;

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
	const Form = document.getElementById(FormId);
	
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
// Separator is HTML code to write between date and time
// Write: a bit set ==> true

const Dat_WRITE_TODAY			= (1 << 5);
const Dat_WRITE_DATE_ON_SAME_DAY	= (1 << 4);
const Dat_WRITE_WEEK_DAY		= (1 << 3);
const Dat_WRITE_HOUR			= (1 << 2);
const Dat_WRITE_MINUTE			= (1 << 1);
const Dat_WRITE_SECOND			= (1 << 0);

const txtToday = [
	"",			// Unknown
	"Avui",			// CA
	"Heute",		// DE
	"Today",		// EN
	"Hoy",			// ES
	"Aujourd'hui",		// FR
	"Ko &aacute;ra",	// GN
	"Oggi",			// IT
	"Dzisiaj",		// PL
	"Hoje",			// PT
	"Bug&uuml;n",		// TR
];

function writeLocalDateHMSFromUTC (id,TimeUTC,DateFormat,Separator,Language,WhatToWrite) {
	var today = new Date();
	var todayYea = today.getFullYear ();
	var todayMon = today.getMonth () + 1;
	var todayDay = today.getDate ();
	var d = new Date();
	var WriteDate;
	var Yea,Mon,Day;
	var DayOfWeek;
	var Hou,Min,Sec;
	var StrDat;
	var StrMon;
	var StrDay;
	var StrHou;
	var StrMin;
	var StrSec;

	d.setTime (TimeUTC * 1000);
	Yea = d.getFullYear ();
	Mon = d.getMonth () + 1;
	Day = d.getDate ();
	
	if ((WhatToWrite & Dat_WRITE_DATE_ON_SAME_DAY) != 0)
		WriteDate = true;
	// Check to see if the last date has been initialized
	else if (typeof writeLocalDateHMSFromUTC.lastd == 'undefined')
		// lastd: static variable to remember current date for the next call
		// Not initialized
		WriteDate = true;
	else
		WriteDate = (Yea != writeLocalDateHMSFromUTC.lastd.getFullYear ()	||
					 Mon != writeLocalDateHMSFromUTC.lastd.getMonth () + 1	||
					 Day != writeLocalDateHMSFromUTC.lastd.getDate ());

	writeLocalDateHMSFromUTC.lastd = d;	// Update last date for the next call

	/* Set date */
	StrDat = '';
	if (WriteDate) {
		if ((WhatToWrite & Dat_WRITE_TODAY) != 0 &&
			Yea == todayYea &&
			Mon == todayMon &&
			Day == todayDay)	// Date is today
			StrDat = txtToday[Language];
		else
			switch (DateFormat) {
				case 0:	// Dat_FORMAT_YYYY_MM_DD
					StrMon = ((Mon < 10) ? '0' : '') + Mon;
					StrDay = ((Day < 10) ? '0' : '') + Day;
					StrDat = Yea.toString () + '-' + StrMon + '-' + StrDay;
					break;
				case 1:	// Dat_FORMAT_DD_MONTH_YYYY
					StrDat = Day.toString () + ' ' +
							 MonthsShort[Mon - 1] + ' ' +
							 Yea.toString ();
					break;
				case 2:	// Dat_FORMAT_MONTH_DD_YYYY
					StrDat = MonthsShort[Mon - 1] + ' ' +
							 Day.toString() + ', ' +
							 Yea.toString ();
					break;
				default:
					break;
			}
		
		if ((WhatToWrite & Dat_WRITE_WEEK_DAY) != 0) {
			DayOfWeek = d.getDay();
			DayOfWeek = (DayOfWeek == 0) ? 6 : DayOfWeek - 1;
			StrDat += Separator + DAYS[DayOfWeek];
		}
		StrDat += Separator;
	}

	/* Set HH:MM:SS */
	StrHou = '';
	StrMin = '';
	StrSec = '';
	if ((WhatToWrite & Dat_WRITE_HOUR) != 0) {
		// Bit 2 on => Write hour
		Hou = d.getHours();
		StrHou = ((Hou < 10) ?  '0' :  '') + Hou;
		if ((WhatToWrite & Dat_WRITE_MINUTE) != 0) {
			// Bits 2,1 on => Write minutes
			Min = d.getMinutes ();
			StrMin = ((Min < 10) ? ':0' : ':') + Min;
			if ((WhatToWrite & Dat_WRITE_SECOND) != 0) {
				// Bits 2,1,0 on => Write seconds
				Sec = d.getSeconds ();
				StrSec = ((Sec < 10) ? ':0' : ':') + Sec;
			}
		}
	}

	/* Write date and time */
	document.getElementById (id).innerHTML = StrDat + StrHou + StrMin + StrSec;
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
		var refreshParams = refreshParamNxtActCon + '&' +
							refreshParamIdSes + '&' +
							refreshParamCrsCod;

		// onreadystatechange must be lowercase
		objXMLHttpReqCon.onreadystatechange = readConnUsrsData;
		objXMLHttpReqCon.open('POST',actionAJAX,true);
		objXMLHttpReqCon.setRequestHeader('Content-Type','application/x-www-form-urlencoded');
		objXMLHttpReqCon.send(refreshParams);
	}
}

// Receive and show connected users data
function readConnUsrsData () {
	if (objXMLHttpReqCon.readyState == 4)	// Check if data have been received
		if (objXMLHttpReqCon.status == 200) {
			var endOfDelay   = objXMLHttpReqCon.responseText.indexOf('|',0);		// Get separator position
			var endOfNotif   = objXMLHttpReqCon.responseText.indexOf('|',endOfDelay  + 1);	// Get separator position
			var endOfGblCon  = objXMLHttpReqCon.responseText.indexOf('|',endOfNotif  + 1);	// Get separator position
			var endOfCrsCon  = objXMLHttpReqCon.responseText.indexOf('|',endOfGblCon + 1);	// Get separator position
			var endOfNumUsrs = objXMLHttpReqCon.responseText.indexOf('|',endOfCrsCon + 1);	// Get separator position

			var delay = parseInt(objXMLHttpReqCon.responseText.substring(0,endOfDelay));		// Get refresh delay
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

/*****************************************************************************/
/***************** Update exam print main area using AJAX ********************/
/*****************************************************************************/

var txtConnectionIssues = [
	"",													// Unknown
	"Problemes de connexi&oacute;. Els canvis no s&apos;han desat.",					// CA
	"Verbindungsprobleme. Die &Auml;nderungen wurden nicht gespeichert.",					// DE
	"Connection issues. The changes have not been saved.",							// EN
	"Problema de conexi&oacute;n. Los cambios no se han guardado.",						// ES
	"Probl&egrave;mes de connexion. Les modifications n'ont pas &eacute;t&eacute; enregistr&eacute;es.",	// FR
	"Problema de conexi&oacute;n. Los cambios no se han guardado.",						// GN Okoteve traducción
	"Problemi di connessione. Le modifiche non sono state salvate.",					// IT
	"Problemy z po&lstrok;&aogon;czeniem. Zmiany nie zosta&lstrok;y zapisane.",				// PL
	"Problemas de conex&atilde;o. As altera&ccedil;&otilde;es n&atilde;o foram salvas."			// PT
];

var txtSaving = [
	"",				// Unknown
	"Desant&hellip;",		// CA
	"Speichern&hellip;",		// DE
	"Saving&hellip;",		// EN
	"Guardando&hellip;",		// ES
	"Enregistrement&hellip;",	// FR
	"Guardando&hellip;",		// GN Okoteve traducción
	"Salvataggio&hellip;",		// IT
	"Zapisywanie&hellip;",		// PL
	"Salvando&hellip;",		// PT
];

var IHaveFinishedTxt;

// This function is called when user changes an answer in an exam print
function updateExamPrint (idDiv,idInput,nameInput,Params,Language) {
    var objXMLHttp = false;

	objXMLHttp = AJAXCreateObject ();
	if (objXMLHttp) {
		/* Send request to server */
		// onreadystatechange must be lowercase
		objXMLHttp.onreadystatechange = function() {
			if (objXMLHttp.readyState == 4)	// Check if data have been received
				if (objXMLHttp.status == 200) {
					// Response received
					clearTimeout (xmlHttpTimeout);		// Clear timeout
					if (idDiv) {
						var div = document.getElementById(idDiv);		// Access to DIV
						if (div) {
							div.innerHTML = objXMLHttp.responseText;	// Update DIV content
							
							// Scripts in div got via AJAX are not executed ==> execute them
							evalScriptsInElem (div);
	
							// Process maths; see http://docs.mathjax.org/en/latest/advanced/typeset.html
							MathJax.typeset();
						}
					}
				}
		};

		var inputElem = document.getElementById(idInput);

		if (inputElem)
			if (inputElem.type) {
				if (inputElem.type === 'radio') {
					if (inputElem.checked)
						Params += '&' + nameInput + '=' + inputElem.value;
				}
				else if (inputElem.type === 'checkbox') {
					var inputElems = inputElem.form.elements;
					var i = 0;

					// First checkbox checked
					for (; i<inputElems.length; i++)
						if (inputElems[i].checked) {
							Params += '&' + nameInput + '=' + inputElems[i].value;
							i++;
							break;
						}

					// Other checked checkboxes
					for (; i<inputElems.length; i++)
						if (inputElems[i].checked)
							Params += ',' + inputElems[i].value;
				}
				else {
					// form value is always UTF-8 codified, even if other coding is specified in form				
					var val = inputElem.value;

					// Params += '&' + nameInput + '=' + encodeURIComponent(val);	// UTF-8 escaped
					// Params += '&' + nameInput + '=' + escape(val);		// ISO-8859-1 escaped (deprecated)
					Params += '&' + nameInput + '=' + getEscapedString(val);	// ISO-8859-1 escaped, replacement for deprecated escape()
					inputElem.value = '?';	// Reset while waiting response. If connection is broken ==> user will see ? input
				}
			}
		
		IHaveFinishedTxt = disableFinished (txtSaving[Language]);	// Disable finished button on sending. When answer is saved and response received ==> the button will be reloaded

		objXMLHttp.open('POST',actionAJAX,true);
		objXMLHttp.setRequestHeader('Content-Type','application/x-www-form-urlencoded');
		objXMLHttp.send(Params);

		/* Timeout to abort in 5 seconds.
		   See https://stackoverflow.com/questions/1018705/how-to-detect-timeout-on-an-ajax-xmlhttprequest-call-in-the-browser
		   and http://geekswithblogs.net/lorint/archive/2006/03/07/71625.aspx
		   "ontimeout" based solutions does not work when network disconnects */
		var xmlHttpTimeout = setTimeout (ajaxTimeout,5000);	// 5 s
		function ajaxTimeout () {
			objXMLHttp.abort ();
			alert (txtConnectionIssues[Language]);
			disableFinished (IHaveFinishedTxt);	// Sending aborted ==> change "Saving..." to original "I have finished"
		};
	}
}

// Escape the same chars as encodeURIComponent(), but in ISO-8859-1
function getEscapedString (str) {
	const len = str.length;
	var escaped = '';

	for (var i = 0; i < len; i++) {
        const code = str.charCodeAt(i);
        switch (true) {	// Escape punctuation marks except - _ . ! ~ * ' ( )
        	case (code <= 0x09):						// Spec.
        		escaped += '%0' + code.toString(16).toUpperCase();	// %0X
        		break;
        	case (code >= 0x10 && code <= 0x20):				// Spec.   0x20 space
        	case (code >= 0x22 && code <= 0x26):				// 0x22 "  0x23 #  0x24 $  0x25 %  0x26 &
        	case (code == 0x2B):						// 0x2B +
        	case (code == 0x2C):						// 0x2C ,
        	case (code == 0x2F):						// 0x2F /
        	case (code >= 0x3A && code <= 0x40):				// 0x3A :  0x3B ;  0x3C <  0x3D =  0x3E >  0x3F ?  0x40 @
        	case (code >= 0x5B && code <= 0x5E):				// 0x5B [  0x5C \  0x5D ]  0x5E ^
        	case (code == 0x60):						// 0x60 `
        	case (code >= 0x7B && code <= 0x7D):				// 0x7B {  0x7C |  0x7D }
        	case (code >= 0x7F && code <= 0xFF):				// ISO-8859-1 or windows-1252
        		escaped += '%' + code.toString(16).toUpperCase();	// %XX
        		break;
        	case (code >= 0x100):
        		escaped += '%26%23' + code.toString(10) + '%3B';	// &#code; instead of %uXXXX
        		break;
	        default:	// 0x21 !
       				// 0x27 '  0x28 (  0x29 )  0x2A *
	        		// 0x2D -  0x2E .
	        		// 0x30 0   ...    0x39 9
	        		// 0x41 A   ...    0x5A Z
	        		// 0x5F _
	        		// 0x61 a   ...    0x7A z
	        		// 0x7E ~
	        	escaped += str.charAt(i);
	        	break;
        }
	}
	return escaped;
}

/*
// To generate a table with the codification generated by encodeURIComponent()
// and escape(), copy the following code in https://playcode.io/
//-----------------------------------------------------------------------------
var arr = [];
for(var i=0;i<256;i++) {
  var char=String.fromCharCode(i);
    arr.push({
      character:char,
      encodeURIComponent:encodeURIComponent(char),
      escape:escape(char)
    });
}
console.table(arr);
//-----------------------------------------------------------------------------
*/

/*****************************************************************************/
/********* Disable button to finish exam when focus on a input text **********/
/*****************************************************************************/

function disableFinished (buttonNewTxt) {
	var f = document.getElementById('finished');		// Access to form
	var buttonOldTxt = '';
	
	if (f)
		for (var i = 0; i < f.elements.length; i++) {
			var b = f.elements[i];
			if (b.type == 'submit') {
				b.disabled = true;
				b.style.opacity = 0.5;
				buttonOldTxt = b.innerHTML;
				b.innerHTML = buttonNewTxt;
			}
		}
	
	return buttonOldTxt;
}

/*****************************************************************************/
/********** Automatic refresh of current match question using AJAX ***********/
/*****************************************************************************/

//  This function must be called from time to time
var objXMLHttpReqMchStd = false;
function refreshMatchStd () {
	objXMLHttpReqMchStd = AJAXCreateObject();
	if (objXMLHttpReqMchStd) {
		var refreshParams = refreshParamNxtActMch + '&' +
							refreshParamMchCod + '&' +
							refreshParamIdSes;

		// onreadystatechange must be lowercase
		objXMLHttpReqMchStd.onreadystatechange = readMatchStdData;
		objXMLHttpReqMchStd.open('POST',actionAJAX,true);
		objXMLHttpReqMchStd.setRequestHeader('Content-Type','application/x-www-form-urlencoded');
		objXMLHttpReqMchStd.send(refreshParams);
	}
}

function readMatchStdData () {
	if (objXMLHttpReqMchStd.readyState == 4)	// Check if data have been received
		if (objXMLHttpReqMchStd.status == 200) {
			var htmlMatch = objXMLHttpReqMchStd.responseText;	// Get HTML code

			var div = document.getElementById('match');	// Access to refreshable DIV
			if (div)
				div.innerHTML = htmlMatch;				// Update DIV content
			// Global delay variable is set initially in swad-core
			setTimeout('refreshMatchStd()',delayMatch);
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
		var refreshParams = refreshParamNxtActMch + '&' +
							refreshParamMchCod + '&' +
							refreshParamIdSes;

		// onreadystatechange must be lowercase
		objXMLHttpReqMchTch.onreadystatechange = readMatchTchData;
		objXMLHttpReqMchTch.open('POST',actionAJAX,true);
		objXMLHttpReqMchTch.setRequestHeader('Content-Type','application/x-www-form-urlencoded');
		objXMLHttpReqMchTch.send(refreshParams);
	}
}

function readMatchTchData () {
	if (objXMLHttpReqMchTch.readyState == 4)	// Check if data have been received
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

					// Process maths; see http://docs.mathjax.org/en/latest/advanced/typeset.html
					MathJax.typeset();
				}
			}
			
			// Global delay variable is set initially in swad-core
			setTimeout('refreshMatchTch()',delayMatch);
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
		// onreadystatechange must be lowercase
		objXMLHttp.onreadystatechange = function() {
			if (objXMLHttp.readyState == 4)	// Check if data have been received
				if (objXMLHttp.status == 200)
					if (id) {
						var div = document.getElementById(id);			// Access to DIV
						if (div)
							div.innerHTML = objXMLHttp.responseText;	// Update DIV content
					}
		};
		objXMLHttp.open('POST',actionAJAX,true);
		objXMLHttp.setRequestHeader('Content-Type','application/x-www-form-urlencoded');
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
		var refreshParams = refreshParamNxtActLstClk + '&' +
							refreshParamIdSes + '&' +
							refreshParamCrsCod;

		// onreadystatechange must be lowercase
		objXMLHttpReqLog.onreadystatechange = readLastClicksData;
		objXMLHttpReqLog.open('POST',actionAJAX,true);
		objXMLHttpReqLog.setRequestHeader('Content-Type','application/x-www-form-urlencoded');
		objXMLHttpReqLog.send(refreshParams);
	}
}

function readLastClicksData () {
	if (objXMLHttpReqLog.readyState == 4)	// Check if data have been received
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

/*****************************************************************************/
/******* Automatically refresh new publications in timeline using AJAX *******/
/*****************************************************************************/

// This function must be called from time to time

var objXMLHttpReqNewTml = false;

function refreshNewTimeline () {
	objXMLHttpReqNewTml = AJAXCreateObject();
	if (objXMLHttpReqNewTml) {
		var refreshParams = refreshParamNxtActNewPub + '&' +
							refreshParamIdSes + '&' +
							refreshParamWho;

		// onreadystatechange must be lowercase
		objXMLHttpReqNewTml.onreadystatechange = readNewTimelineData;
		objXMLHttpReqNewTml.open('POST',actionAJAX,true);
		objXMLHttpReqNewTml.setRequestHeader('Content-Type','application/x-www-form-urlencoded');
		objXMLHttpReqNewTml.send(refreshParams);
	}
}

function readNewTimelineData () {
	if (objXMLHttpReqNewTml.readyState == 4)	// Check if data have been received
		if (objXMLHttpReqNewTml.status == 200) {

			// Access to UL for just now timeline
			var justNowTimeline = document.getElementById('just_now_timeline_list');
			if (justNowTimeline) {
				// Update list of publications in just now timeline		
				justNowTimeline.innerHTML = objXMLHttpReqNewTml.responseText;

				var numNotesJustGot = justNowTimeline.childNodes.length;
				if (numNotesJustGot) {	// New notes received
					// Scripts in timeline got via AJAX not executed ==> execute them
					evalScriptsInElem (justNowTimeline);

					// Process maths; see http://docs.mathjax.org/en/latest/advanced/typeset.html
					MathJax.typeset();

					// Move all the LI elements (notes) in UL 'just_now_timeline_list'...
					// ...to the top of UL 'new_timeline_list'
					var newTimeline = document.getElementById('new_timeline_list');

					for (var i=0; i<numNotesJustGot; i++)
						// Move node from just now timeline to new timeline
						newTimeline.insertBefore(justNowTimeline.lastChild,newTimeline.firstChild);

					// Update number of notes in new timeline
					var viewNewCount = document.getElementById('view_new_count');
					viewNewCount.innerHTML = newTimeline.childNodes.length;

					// Unhide message with number of notes if hidden
					var viewNewContainer = document.getElementById('view_new_container');
					viewNewContainer.style.display = '';
				}
			}
			
			// Global delay variable is set initially in swad-core
			delayNewTml += 1000;	// Increase one second on each call
			setTimeout('refreshNewTimeline()',delayNewTml);
		}
}

/*****************************************************************************/
/*** View new pubs. in timeline by moving new timeline to top of timeline ****/
/*****************************************************************************/

// This function is called when user clicks in 'See new activity (n)'

function moveNewTimelineToTimeline () {
	// Move the LI elements (notes) in UL 'new_timeline_list'...
	// ...to the top of UL 'timeline_list', only if not repeated before
	var newTimeline = document.getElementById('new_timeline_list');
	var numNewNotes = newTimeline.childNodes.length;

	if (numNewNotes) {
		var timeline = document.getElementById("timeline_list");

		for (var i=1; i<=numNewNotes; i++) {
			// Check if the last child (the oldest) in the new timeline...
			// ...is the last ocurrence of the note
			var mostRecentOcurrenceOfNote = true;
			var lastChildIndex = numNewNotes - i;
			var noteCode = newTimeline.lastChild.dataset.noteCode;
			for (var j=0; j<lastChildIndex; j++)
				if (newTimeline.childNodes[j].dataset.noteCode == noteCode) {
					mostRecentOcurrenceOfNote = false;
					break;
				}

			// Move or remove node from new timeline
			if (mostRecentOcurrenceOfNote)
				// Move node from new timeline to timeline
				timeline.insertBefore(newTimeline.lastChild,timeline.firstChild);
			else
				// Remove last child (because is repeated in more recent pubs)
				newTimeline.removeChild(newTimeline.lastChild);
		}
	}

	// Reset number of new publications after moving
	var viewNewCount = document.getElementById('view_new_count');
	viewNewCount.innerHTML = 0;

	// Hide link to view new publications after moving
	var viewNewContainer = document.getElementById('view_new_container');
	viewNewContainer.style.display = 'none';
}

/*****************************************************************************/
/************* Refresh old publications in timeline using AJAX ***************/
/*****************************************************************************/

// See https://webdesign.tutsplus.com/how-to-implement-infinite-scrolling-with-javascript--cms-37055t
// See https://www.educative.io/answers/how-to-implement-infinite-scrolling-in-javascript

var refreshingOldTimeline = false;

function handleInfiniteScroll () {
	if (refreshingOldTimeline) return;	// Don't check scroll while refreshing

	if (window.scrollY + window.innerHeight >= document.documentElement.scrollHeight - 360) {
		// Near the end of the page (subtract at least 360 pixels so it works well on mobile)
		refreshingOldTimeline = true;
		refreshOldTimeline ();
	}
};

// This function is called when user clicks in 'See more'

var objXMLHttpReqOldTml = false;

function refreshOldTimeline () {
	objXMLHttpReqOldTml = AJAXCreateObject ();
	if (objXMLHttpReqOldTml) {
		var refreshParams = refreshParamNxtActOldPub + '&' +
							refreshParamIdSes;
		if (typeof refreshParamUsr !== 'undefined') {
			if (refreshParamUsr.length)
				refreshParams += '&' + refreshParamUsr;
		}
		if (typeof refreshParamWho !== 'undefined') {
			if (refreshParamWho.length)
				refreshParams += '&' + refreshParamWho;
		}

		// onreadystatechange must be lowercase
		objXMLHttpReqOldTml.onreadystatechange = readOldTimelineData;
		objXMLHttpReqOldTml.open('POST',actionAJAX,true);
		objXMLHttpReqOldTml.setRequestHeader('Content-Type','application/x-www-form-urlencoded');
		objXMLHttpReqOldTml.send(refreshParams);
	}
}

function readOldTimelineData () {
	// Hide view more button, unhide waiting image
	document.getElementById("view_more").style.display = 'none';
	document.getElementById("loading_old_timeline").style.display = '';

	if (objXMLHttpReqOldTml.readyState == 4)	// Check if data have been received
		if (objXMLHttpReqOldTml.status == 200) {
			// Access to UL with the old timeline
			var oldTimeline = document.getElementById('old_timeline_list');
			if (oldTimeline) {
				// Fill list of publications in old timeline
				oldTimeline.innerHTML = objXMLHttpReqOldTml.responseText;

				var countOldTimeline = oldTimeline.childNodes.length;
				if (countOldTimeline) {
					// Scripts in timeline got via AJAX not executed ==> execute them
					evalScriptsInElem (oldTimeline);

					// Process maths; see http://docs.mathjax.org/en/latest/advanced/typeset.html
					MathJax.typeset();

					// Move all the LI elements in UL 'old_timeline_list'
					// to the bottom of UL 'timeline_list'
					var timeline = document.getElementById("timeline_list");
					for (var i=0; i<countOldTimeline; i++)
						timeline.appendChild(oldTimeline.firstChild);

					// Unhide view more button only if oldest pub not reached
					document.getElementById("view_more").style.display = '';
				}
				else {	// No old publications retrieved, so we have reached the oldest pub.
					// Remove event listener
					window.removeEventListener("scroll", handleInfiniteScroll);
					// Hide container with link to get old publications
					document.getElementById("view_old_pubs_container").style.display = 'none';
				}

				// Hide waiting image
				document.getElementById("loading_old_timeline").style.display = 'none';

				refreshingOldTimeline = false;	// Refreshing is finished
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
		s = scrs[i].firstChild.nodeValue;
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
		// onreadystatechange must be lowercase
		objXMLHttp.onreadystatechange = function() {
			if (objXMLHttp.readyState == 4)	// Check if data have been received
				if (objXMLHttp.status == 200)
					if (id) {
						var div = document.getElementById(id);			// Access to DIV
						if (div) {
							div.innerHTML = objXMLHttp.responseText;	// Update DIV content
						
							// Scripts in div got via AJAX are not executed ==> execute them
							evalScriptsInElem (div);
	
							// Process maths; see http://docs.mathjax.org/en/latest/advanced/typeset.html
							MathJax.typeset();
						}
					}
		};
		objXMLHttp.open('POST',actionAJAX,true);
		objXMLHttp.setRequestHeader('Content-Type','application/x-www-form-urlencoded');
		objXMLHttp.send(Params);
	}
}

/*****************************************************************************/
/************** Update lock/unlock area in project using AJAX ****************/
/*****************************************************************************/

// This function is called when user submit a form just inside a parent div
function updateDivLockUnlockProject (form,Params) {
	var id = form.parentNode.id;

	var objXMLHttp = AJAXCreateObject ();
	if (objXMLHttp) {
		/* Send request to server */
		// onreadystatechange must be lowercase
		objXMLHttp.onreadystatechange = function() {
			if (objXMLHttp.readyState == 4)	// Check if data have been received
				if (objXMLHttp.status == 200)
					if (id) {
						var div = document.getElementById(id);			// Access to DIV
						if (div)
							div.innerHTML = objXMLHttp.responseText;	// Update DIV content
					}
		};
		objXMLHttp.open('POST',actionAJAX,true);
		objXMLHttp.setRequestHeader('Content-Type','application/x-www-form-urlencoded');
		objXMLHttp.send(Params);
	}
}

/*****************************************************************************/
/********** Update fav or share area in social timeline using AJAX ***********/
/*****************************************************************************/

// This function is called when user submit a form inside two parent divs
function updateDivFaversSharers (form,Params) {
	var id = form.parentNode.parentNode.id;

	var objXMLHttp = AJAXCreateObject ();
	if (objXMLHttp) {
		/* Send request to server */
		// onreadystatechange must be lowercase
		objXMLHttp.onreadystatechange = function() {
			if (objXMLHttp.readyState == 4)	// Check if data have been received
				if (objXMLHttp.status == 200)
					if (id) {
						var div = document.getElementById(id);			// Access to DIV
						if (div)
							div.innerHTML = objXMLHttp.responseText;	// Update DIV content
					}
		};
		objXMLHttp.open('POST',actionAJAX,true);
		objXMLHttp.setRequestHeader('Content-Type','application/x-www-form-urlencoded');
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

		// Enable upload, disable others
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

	iconDiv.className = (iconDiv.className == 'Tml_ICO_COM_OFF') ? 'Tml_ICO_COM_ON' :
																   'Tml_ICO_COM_OFF';
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

function toggleDisplay (id) {
	var element = document.getElementById (id);
	var stl;

	if (element) {
		stl = element.style;
		stl.display = (stl.display === 'none') ? '' : 'none';
	}
}

/*****************************************************************************/
/****************************** Unhide element *******************************/
/*****************************************************************************/

function unhideElement (elementID) {
	var element = document.getElementById (elementID);

	if (element)
		element.style.display = '';
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
// The second parameter, wasCheckedInitially, provides information about the initial state of the radio
function selectUnselectRadio (radio,wasCheckedInitially,groupRadios,numRadiosInGroup){
	if (radio.wasChecked == undefined) radio.wasChecked = wasCheckedInitially;
	if (radio.wasChecked) radio.checked = false;	// uncheck when clicking on a checked radio
	radio.wasChecked = !radio.wasChecked;
	for (var i=0; i<numRadiosInGroup; i++)
		if (groupRadios[i] != radio) groupRadios[i].wasChecked = false;
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
	var Sel = document.getElementById('SelTag' + NumTag);

	document.getElementById('TagTxt' + NumTag).value = Sel.options[Sel.selectedIndex].value;
}

// Change selectors of test tags
function changeSelTag (NumTag) {
	var Sel = document.getElementById('SelTag'+NumTag);
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

function Cal_DrawCalendar (id,FirstDayOfWeek,
							TimeUTC,CurrentPlcCod,PrintView,ColorSuffix,
							CGI,FormGoToCalendarParams,FormEventParams) {
	/* var StartingMonth = [	// Calendar starts one row before current month
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
	]; */
	var StartingMonth = [	// Calendar starts one row before current month
		 2,	// January   --> February
		 3,	// February  --> Mars
		 4,	// Mars      --> April
		 5,	// April     --> May
		 6,	// May       --> June
		 7,	// June      --> July
		 8,	// July      --> August
		 9,	// August    --> September
		10,	// September --> October
		11,	// October   --> November
		12,	// November  --> December
		 1	// December  --> January
	];
	var d = new Date();
	d.setTime(TimeUTC * 1000);
	var CurrentYear = d.getFullYear();
	var CurrentMonth = d.getMonth() + 1;
	var CurrentDay = d.getDate();
	var Month = StartingMonth[CurrentMonth - 1];
	var Year = (Month < CurrentMonth) ? CurrentYear :
										CurrentYear - 1;
	var i;
	var MonthIdNum = 0;
	var MonthId;

	/***** Draw several months *****/
	for (i = 0;
		 i < 24;
		 i++) {
		MonthIdNum++;
		MonthId = id + '_month_' + MonthIdNum;

		Gbl_HTMLContent += '<div class="MONTH_CONT">';
		DrawMonth (MonthId,FirstDayOfWeek,
					Year,Month,
					CurrentYear,CurrentMonth,CurrentDay,
					CurrentPlcCod,true,PrintView,ColorSuffix,
					CGI,FormGoToCalendarParams,FormEventParams);
		Gbl_HTMLContent += '</div>';
		if (++Month == 13) {
			Month = 1;
			Year++;
		}
	}

	document.getElementById(id).innerHTML = Gbl_HTMLContent;
}

/*****************************************************************************/
/***************************** Draw current month ****************************/
/*****************************************************************************/

function DrawCurrentMonth (id,FirstDayOfWeek,TimeUTC,CurrentPlcCod,ColorSuffix,
						   CGI,FormGoToCalendarParams,FormEventParams) {
	var d = new Date();
	d.setTime(TimeUTC * 1000);
	var Year = d.getFullYear();
	var Month = d.getMonth() + 1;
	var CurrentDay = d.getDate();

	DrawMonth (id,FirstDayOfWeek,
				Year,Month,
				Year,Month,CurrentDay,
				CurrentPlcCod,false,false,ColorSuffix,
				CGI,FormGoToCalendarParams,FormEventParams);
	document.getElementById(id).innerHTML = Gbl_HTMLContent;
}

/*****************************************************************************/
/******************************* Draw a month ********************************/
/*****************************************************************************/
// FirstDayOfWeek == 0 ==> Weeks from Monday to Sunday
// FirstDayOfWeek == 6 ==> Weeks from Sunday to Saturday

function DrawMonth (id,FirstDayOfWeek,
					YearToDraw,MonthToDraw,
					CurrentYear,CurrentMonth,CurrentDay,
					CurrentPlcCod,DrawingCalendar,PrintView,ColorSuffix,
					CGI,FormGoToCalendarParams,FormEventParams) {
	var Hld_HOLIDAY = 0;
	var Hld_NON_SCHOOL_PERIOD = 1;
	var Yea = YearToDraw;
	var Mon = MonthToDraw;
	var Day;
	var ExaCod;
	var StrMon;
	var StrDay;
	var StrDat;
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
	Gbl_HTMLContent += '<div class="MONTH">';

	/***** Month name *****/
	if (DrawingCalendar)
		Gbl_HTMLContent += '<div class="MONTH_NAME MONTH_NAME_' + ColorSuffix + '">';
	else {
		FormId = id + '_show_calendar';
		Gbl_HTMLContent += '<form method="post" action="' + CGI + '" id="' + FormId + '">' +
							FormGoToCalendarParams +
							'<div class="MONTH_NAME">' +
							'<a href="" class="MONTH_NAME_' + ColorSuffix +
							'" onclick="document.getElementById(\'' + FormId +
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
						   ((DayOfWeek == 6 - FirstDayOfWeek) ? 'DAY DAY_NO_WRK_HEAD_' + ColorSuffix :
													            'DAY DAY_WRK_HEAD_' + ColorSuffix) +
						   '">' +
						   DAYS2[(DayOfWeek + FirstDayOfWeek) % 7] +
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
			ClassForDay = ((Mon == MonthToDraw) ? 'DAY_WRK_' + ColorSuffix :
												  'DAY_WRK_LIGHT_' + ColorSuffix);
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
									ClassForDay = ((Mon == MonthToDraw) ? 'DAY_HLD_' + ColorSuffix :
																		  'DAY_HLD_LIGHT_' + ColorSuffix);
									TextForDay = Hlds[NumHld].Name;
									ContinueSearching = false;
								}
								break;
							case Hld_NON_SCHOOL_PERIOD:
								if (Hlds[NumHld].EndDate >= YYYYMMDD) {	// If start date <= date being drawn <= end date
									ClassForDay = ((Mon == MonthToDraw) ? 'DAY_NO_WORK_' + ColorSuffix :
																		  'DAY_NO_WORK_LIGHT_' + ColorSuffix);
									TextForDay = Hlds[NumHld].Name;
								}
								break;
						}
				}

			/* Day being drawn is sunday? */
			if (DayOfWeek == 6 - FirstDayOfWeek) // All the sundays are holidays
				ClassForDay = (Mon == MonthToDraw) ? 'DAY_HLD_' + ColorSuffix :
													 'DAY_HLD_LIGHT_' + ColorSuffix;

			/* Date being drawn is today? */
			IsToday = (Yea == YearToDraw   &&
					   Mon == MonthToDraw  &&
					   Yea == CurrentYear  &&
					   Mon == CurrentMonth &&
					   Day == CurrentDay);

			/* Check if day has a call for exam */
			ThisDayHasEvent = false;
			if (!DrawingCalendar || Mon == MonthToDraw) // If drawing calendar and the month is not the real one, don't draw call for exams
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
							StrDat = Yea + StrMon + StrDay;	// YYYYMMDD
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
									StrDat +
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
			if (idRow.indexOf(idParent + '_') == 0) {		// row.id starts by idParent_, so it's a child
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
	var idRow;

	for (i = 0; i < rows.length; i++) {
		row = rows[i];
		if (row != parent) {
			idRow = row.id;
			if (idRow.indexOf(idParent + '_') == 0)	// row.id starts by idParent_, so it's a child
				/* Hide row */
				row.style.display = 'none';			// hide
		}
	}
}
