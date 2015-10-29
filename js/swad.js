// swad.js: javascript functions

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.
    Copyright (C) 1999-2015 Antonio Ca�as-Vargas
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

// Global variable used in refreshConnected()
var ActionAJAX;

// Global variables used in writeLocalClock()
var secondsSince1970UTC;

// Global variables used in writeClockConnected()
var NumUsrsCon;
var ListSeconds = [];
var countClockConnected = 0;

// Write a date in client local time
function writeLocalDateFromUTC(id,TimeUTC) {
	var d = new Date;
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
	document.getElementById(id).innerHTML = Yea + '/' + StrMon + '/' + StrDay;
}

/*************** Write a date-time in client local time **********************/
// - id is the id of the HTML element in which date-time will be written
// - TimeUTC is the date-time to write in UTC UNIX time format
// - separator is HTML code to write between date and time

function writeLocalDateTimeFromUTC(id,TimeUTC,separator) {
	var d = new Date;
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
	document.getElementById(id).innerHTML = Yea    + '/' + StrMon + '/' + StrDay +
						separator +
						StrHou + ':' + StrMin + ':' + StrSec;
}

// Set local date-time form fields from UTC time
function setLocalDateTimeFormFromUTC(id,TimeUTC) {
	var FormYea = document.getElementById(id+'Year');
	var FormMon = document.getElementById(id+'Month');
	var FormDay = document.getElementById(id+'Day');
	var FormHou = document.getElementById(id+'Hour');
	var FormMin = document.getElementById(id+'Minute');
	var FormSec = document.getElementById(id+'Second');
	var d;
	var Year;
	var YearIsValid = false;

	if (TimeUTC) {
		d = new Date;
		d.setTime(TimeUTC * 1000);
		Year = d.getFullYear();
		for (var i=0; i<FormYea.options.length && !YearIsValid; i++)
			if (FormYea.options[i].value == Year) {
				FormYea.options[i].selected = true;
				YearIsValid = true;
			}
		FormMon.disabled = false;
		FormDay.disabled = false;
		FormHou.disabled = false;
		FormMin.disabled = false;
		FormSec.disabled = false;
		FormMon.options[d.getMonth()+1].selected = true;
		FormDay.options[d.getDate()   ].selected = true;
		FormHou.options[d.getHours()  ].selected = true;
		FormMin.options[d.getMinutes()].selected = true;
		FormSec.options[d.getSeconds()].selected = true;
	}
	
	if (!YearIsValid) {
		FormYea.options[0].selected = true;
		FormMon.options[0].selected = true;
		FormDay.options[0].selected = true;
		FormHou.options[0].selected = true;
		FormMin.options[0].selected = true;
		FormSec.options[0].selected = true;

		FormMon.disabled = true;
		FormDay.disabled = true;
		FormHou.disabled = true;
		FormMin.disabled = true;
		FormSec.disabled = true;
	}
}

// Set UTC time from local date-time form fields 
function setUTCFromLocalDateTimeForm(id) {
	var FormYea = document.getElementById(id+'Year');
	var FormMon = document.getElementById(id+'Month');
	var FormDay = document.getElementById(id+'Day');
	var FormHou = document.getElementById(id+'Hour');
	var FormMin = document.getElementById(id+'Minute');
	var FormSec = document.getElementById(id+'Second');
	var FormTimeUTC = document.getElementById(id+'TimeUTC');
	var d;
	var Yea = FormYea.value;
	var Mon;
	var Day;
	
	if (Yea == 0) {
		FormYea.options[0].selected = true;
		FormMon.options[0].selected = true;
		FormDay.options[0].selected = true;
		FormHou.options[0].selected = true;
		FormMin.options[0].selected = true;
		FormSec.options[0].selected = true;

		FormMon.disabled = true;
		FormDay.disabled = true;
		FormHou.disabled = true;
		FormMin.disabled = true;
		FormSec.disabled = true;

		FormTimeUTC.value = 0;
	}
	else {
		FormMon.disabled = false;
		FormDay.disabled = false;
		FormHou.disabled = false;
		FormMin.disabled = false;
		FormSec.disabled = false;

		Mon = FormMon.value;
		if (Mon == 0)
			Mon = 1;
		Day = FormDay.value;
		if (Day == 0)
			Day = 1;

		// Important: set year first in order to work properly with leap years
		d = new (Date);
		d.setFullYear(Yea);
		d.setMonth   (Mon-1);
		d.setDate    (Day);
		d.setHours   (FormHou.value);
		d.setMinutes (FormMin.value);
		d.setSeconds (FormSec.value);
		d.setMilliseconds(0);
	
		FormTimeUTC.value = d.getTime() / 1000;
	}
}

// Set form param with time difference between UTC time and client local time, in minutes
// For example, if your time zone is GMT+2, -120 will be returned
function setTZ(id) {
	var FormTZ = document.getElementById(id);
	var d = new (Date);

	FormTZ.value = d.getTimezoneOffset();
}

// Set form param with time difference between UTC time and client local time, in minutes
// For example, if your time zone is GMT+2, -120 will be returned
function setTZname(id) {
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

	if (FormYea.selectedIndex > 0) {
		if (FormMon.options[0].selected) {			// No month selected, set to january
			FormMon.options[1].selected = true;
			Days = 31;
		}
		else if (FormMon.options[2].selected)			// Adjust days of february
			Days = ((((Yea % 4) == 0) && ((Yea % 100) != 0)) || ((Yea % 400) == 0)) ? 29 : 28;
		else if (FormMon.options[ 4].selected ||
			 FormMon.options[ 6].selected ||
			 FormMon.options[ 9].selected ||
			 FormMon.options[11].selected)
			Days = 30;
		else
			Days = 31;
	
		if (FormDay.options[0].selected)			// No day selected, set to 1
			FormDay.options[1].selected = true;
		else if (FormDay.selectedIndex > Days)
			FormDay.options[Days].selected = true;
	
		for (var i=FormDay.options.length; i<=Days ; i++) {	// Create new days
			var x = String (i);
			FormDay.options[i] = new Option(x,x);
		}
		for (var i=FormDay.options.length-1; i>Days; i--)	// Remove days
			FormDay.options[i] = null;
	}
}

// Set a date range form to yesterday
function setDateToYesterday() {
	var d = new (Date);

	d.setTime(d.getTime() - 24*60*60*1000);	// Today - 1 day
	setDateRange(d);
}

// Set a date range form to today
function setDateToToday() {
	var d = new (Date);

	setDateRange(d);
}

// Set a date range form to a specific day
function setDateRange(d) {
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
	document.getElementById('StartMonth' ).options[Mon].selected = true;
	document.getElementById('StartDay'   ).options[Day].selected = true;
	document.getElementById('StartHour'  ).options[0  ].selected = true;
	document.getElementById('StartMinute').options[0  ].selected = true;
	document.getElementById('StartSecond').options[0  ].selected = true;
	setUTCFromLocalDateTimeForm('Start');

	FormYea = document.getElementById('EndYear');
	for (var i=0; i<FormYea.options.length; i++)
		if (FormYea.options[i].value == Yea) {
			FormYea.options[i].selected = true;
			break;
		}
	document.getElementById('EndMonth' ).options[Mon].selected = true;
	document.getElementById('EndDay'   ).options[Day].selected = true;
	document.getElementById('EndHour'  ).options[23 ].selected = true;
	document.getElementById('EndMinute').options[59 ].selected = true;
	document.getElementById('EndSecond').options[59 ].selected = true;
	setUTCFromLocalDateTimeForm('End');
}

// Write clock in client local time updated every minute
function writeLocalClock() {
	var d;
	var Mon;
	var Day;
        var Hou;
        var Min;
	var StrMin;

	setTimeout('writeLocalClock()',60000);

	d = new Date;
	d.setTime(secondsSince1970UTC * 1000);
	secondsSince1970UTC += 60;	// For next call

	Mon = d.getMonth();
	Day = d.getDate();
	Hou = d.getHours();
	Min = d.getMinutes();
	StrMin = ((Min < 10) ? '0' : '') + Min;
	document.getElementById('hm').innerHTML = Day + ' ' + Months[Mon] + ', ' + Hou + ':' + StrMin;
}
      
function writeClockConnected() {
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
function refreshConnected() {
	objXMLHttpReqCon = AJAXCreateObject();
	if (objXMLHttpReqCon) {
      		var RefreshParams = RefreshParamNxtActCon + '&' + RefreshParamIdSes + '&' + RefreshParamCrsCod;
		objXMLHttpReqCon.onreadystatechange = readConnUsrsData;	// onreadystatechange must be lowercase
		objXMLHttpReqCon.open('POST',ActionAJAX,true);
		objXMLHttpReqCon.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');
		objXMLHttpReqCon.send(RefreshParams);
	}
}

// Automatic refresh of last clicks using AJAX. This function must be called from time to time
var objXMLHttpReqLog = false;
function refreshLastClicks() {
	objXMLHttpReqLog = AJAXCreateObject();
	if (objXMLHttpReqLog) {
      		var RefreshParams = RefreshParamNxtActLog + '&' + RefreshParamIdSes + '&' + RefreshParamCrsCod;
		objXMLHttpReqLog.onreadystatechange = readLastClicksData;	// onreadystatechange must be lowercase
		objXMLHttpReqLog.open('POST',ActionAJAX,true);
		objXMLHttpReqLog.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');
		objXMLHttpReqLog.send(RefreshParams);
	}
}

// Create AJAX object	(try is unknown in earlier versions of Netscape, but works in IE5)
function AJAXCreateObject() {
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
function readConnUsrsData() {
	if (objXMLHttpReqCon.readyState == 4) {	// Check if data have been received
		if (objXMLHttpReqCon.status == 200) {
			var endOfDelay   = objXMLHttpReqCon.responseText.indexOf('|',0);		// Get separator position
			var endOfNotif   = objXMLHttpReqCon.responseText.indexOf('|',endOfDelay+1);	// Get separator position
			var endOfGblCon  = objXMLHttpReqCon.responseText.indexOf('|',endOfNotif+1);	// Get separator position
			var endOfCrsCon  = objXMLHttpReqCon.responseText.indexOf('|',endOfGblCon+1);	// Get separator position
			var endOfNumUsrs = objXMLHttpReqCon.responseText.indexOf('|',endOfCrsCon+1);	// Get separator position

			var delay = parseInt(objXMLHttpReqCon.responseText.substring(0,endOfDelay));		// Get refresh delay
			var htmlNotif  = objXMLHttpReqCon.responseText.substring(endOfDelay +1,endOfNotif);	// Get HTML code for new notifications
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
function readLastClicksData() {
	if (objXMLHttpReqLog.readyState == 4) {	// Check if data have been received
		if (objXMLHttpReqLog.status == 200) {
			var endOfDelay = objXMLHttpReqLog.responseText.indexOf('|',0);			// Get separator position
			var endOfLastClicks = objXMLHttpReqLog.responseText.indexOf('|',endOfDelay+1);	// Get separator position

			var delay = parseInt(objXMLHttpReqLog.responseText.substring(0,endOfDelay));	// Get refresh delay
			var htmlLastClicks = objXMLHttpReqLog.responseText.substring(endOfDelay+1);	// Get HTML code for last clicks

			var divLastClicks = document.getElementById('lastclicks');			// Access to last click DIV
			if (divLastClicks)
				divLastClicks.innerHTML = htmlLastClicks;				// Update global connected DIV
			if (delay > 200)	// If refresh slower than 1 time each 0.2 seconds, do refresh; else abort
				setTimeout('refreshLastClicks()',delay);
		}
	}
}

// Zoom a user's photograph
function zoom(imagen,urlPhoto,shortName) {
	var xPos = imagen.offsetLeft;
	var yPos = imagen.offsetTop;
	var tempEl = imagen.offsetParent;
	while (tempEl != null) {
		xPos += tempEl.offsetLeft;
		yPos += tempEl.offsetTop;
		tempEl = tempEl.offsetParent;
	}
        xPos -= (187+15);
        yPos -= ((250+15)/2);
        if (yPos < 0)
           yPos = 0;
	document.getElementById('zoomLyr').style.left = xPos + 'px';
	document.getElementById('zoomLyr').style.top = yPos + 'px';
	document.getElementById('zoomImg').src = urlPhoto;
	document.getElementById('zoomTxt').innerHTML = '<span class="ZOOM_TXT">' + shortName + '</span>';
}

// Exit from zooming a user's photograph
function noZoom(imagen) {
	var xPos = -(187+15);
	var yPos = -(250+15+110);
	document.getElementById('zoomTxt').innerHTML = '';
	document.getElementById('zoomImg').src='/icon/_.gif';
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
function checkParent(CheckBox, MainCheckbox) {
	var IsChecked = true, i, Formul = CheckBox.form;
	for (i=0; i<Formul.elements.length; i++)
		if (Formul.elements[i].name == CheckBox.name)
			if (!(Formul.elements[i].checked)) { IsChecked = false; break; }
	Formul[MainCheckbox].checked = IsChecked;
}
// Activate all children checkboxes when parent checkbox is activated
// Deactivate all children checkboxes when parent checkbox is deactivated
function togglecheckChildren(MainCheckbox, GroupCheckboxes) {
	var i, Formul = MainCheckbox.form;
	for (i=0; i<Formul.elements.length; i++)
		if (Formul.elements[i].name == GroupCheckboxes) Formul.elements[i].checked = MainCheckbox.checked;
}

// Deactivate a parent checkbox when any child checkbox is activated
// Activate a parent checkbox when all children checkboxes are deactivated
function uncheckParent(CheckBox, MainCheckbox) {
	var IsChecked = false, i, Formul = CheckBox.form;
	for (i=0; i<Formul.elements.length; i++)
		if (Formul.elements[i].name == CheckBox.name)
			if (Formul.elements[i].checked) { IsChecked = true; break; }
	Formul[MainCheckbox].checked = !IsChecked;
}
// Deactivate all children checkboxes when parent checkbox is activated
function uncheckChildren(MainCheckbox, GroupCheckboxes) {
	var i, Formul = MainCheckbox.form;
        if (MainCheckbox.checked)
		for (i=0; i<Formul.elements.length; i++)
			if (Formul.elements[i].name == GroupCheckboxes) Formul.elements[i].checked = false;
}

// Change text of a test descriptor
function changeTxtTag(NumTag){
	var Sel = document.getElementById('SelDesc'+NumTag);
	document.getElementById('TagTxt'+NumTag).value = Sel.options[Sel.selectedIndex].value;
}

// Change selectors of test descriptors
function changeSelTag(NumTag){
	var Sel = document.getElementById('SelDesc'+NumTag);
	var Txt = document.getElementById('TagTxt'+NumTag);
	for (var i=0; i<Sel.options.length-1 ; i++)
		if (Sel.options[i].value.toUpperCase() == Txt.value.toUpperCase()){
			Sel.options[i].selected = true;
			Txt.value = Sel.options[i].value;
			break;
		}
	if (i == Sel.options.length-1) // End reached without matching
		Sel.options[i].selected = true;
}

// Activate or deactivate answer types of a test question
function enableDisableAns(Formul) {
	var Tst_ANS_INT			= 0;
	var Tst_ANS_FLOAT		= 1;
	var Tst_ANS_TRUE_FALSE		= 2;
	var Tst_ANS_UNIQUE_CHOICE	= 3;
	var Tst_ANS_MULTIPLE_CHOICE	= 4;
	var Tst_ANS_TEXT		= 5;

	if (Formul.AnswerType[Tst_ANS_INT].checked){
		for (var i=0; i<Formul.elements.length; i++)
			if (Formul.elements[i].name == 'AnsInt') Formul.elements[i].disabled = false;
			else if (Formul.elements[i].name == 'AnsMulti' ||
			         Formul.elements[i].name == 'AnsFloatMin' ||
			         Formul.elements[i].name == 'AnsFloatMax' ||
			         Formul.elements[i].name == 'AnsTF' ||
			         Formul.elements[i].name == 'AnsUni' ||
			         Formul.elements[i].name == 'Shuffle') Formul.elements[i].disabled = true;
			else enableDisableContAns(Formul.elements[i],true);
	}
	else if (Formul.AnswerType[Tst_ANS_FLOAT].checked){
		for (var i=0; i<Formul.elements.length; i++)
			if (Formul.elements[i].name == 'AnsFloatMin' ||
			    Formul.elements[i].name == 'AnsFloatMax') Formul.elements[i].disabled = false;
			else if (Formul.elements[i].name == 'AnsInt' ||
			         Formul.elements[i].name == 'AnsTF' ||
			         Formul.elements[i].name == 'AnsUni' ||
			         Formul.elements[i].name == 'AnsMulti' ||
			         Formul.elements[i].name == 'Shuffle') Formul.elements[i].disabled = true;
			else enableDisableContAns(Formul.elements[i],true);
	}
	else if (Formul.AnswerType[Tst_ANS_TRUE_FALSE].checked){
		for (var i=0; i<Formul.elements.length; i++)
			if (Formul.elements[i].name == 'AnsTF') Formul.elements[i].disabled = false;
			else if (Formul.elements[i].name == 'AnsInt' ||
			         Formul.elements[i].name == 'AnsFloatMin' ||
			         Formul.elements[i].name == 'AnsFloatMax' ||
			         Formul.elements[i].name == 'AnsUni' ||
			         Formul.elements[i].name == 'AnsMulti' ||
			         Formul.elements[i].name == 'Shuffle') Formul.elements[i].disabled = true;
			else enableDisableContAns(Formul.elements[i],true);
	}
	else if (Formul.AnswerType[Tst_ANS_UNIQUE_CHOICE].checked){
		for (var i=0; i<Formul.elements.length; i++)
			if (Formul.elements[i].name == 'AnsUni' ||
			    Formul.elements[i].name == 'Shuffle') Formul.elements[i].disabled = false;
			else if (Formul.elements[i].name == 'AnsInt' ||
			         Formul.elements[i].name == 'AnsFloatMin' ||
			         Formul.elements[i].name == 'AnsFloatMax' ||
			         Formul.elements[i].name == 'AnsTF' ||
			         Formul.elements[i].name == 'AnsMulti') Formul.elements[i].disabled = true;
			else enableDisableContAns(Formul.elements[i],false);
	}
	else if (Formul.AnswerType[Tst_ANS_MULTIPLE_CHOICE].checked){
		for (var i=0; i<Formul.elements.length; i++)
			if (Formul.elements[i].name == 'AnsMulti' ||
			    Formul.elements[i].name == 'Shuffle') Formul.elements[i].disabled = false;
			else if (Formul.elements[i].name == 'AnsInt' ||
			         Formul.elements[i].name == 'AnsFloatMin' ||
			         Formul.elements[i].name == 'AnsFloatMax' ||
			         Formul.elements[i].name == 'AnsTF' ||
			         Formul.elements[i].name == 'AnsUni') Formul.elements[i].disabled = true;
			else enableDisableContAns(Formul.elements[i],false);
	}
	else if (Formul.AnswerType[Tst_ANS_TEXT].checked){
		for (var i=0; i<Formul.elements.length; i++)
			if (Formul.elements[i].name == 'AnsInt' ||
			    Formul.elements[i].name == 'AnsFloatMin' ||
			    Formul.elements[i].name == 'AnsFloatMax' ||
			    Formul.elements[i].name == 'AnsTF' ||
			    Formul.elements[i].name == 'AnsUni' ||
			    Formul.elements[i].name == 'AnsMulti') Formul.elements[i].disabled = true;
			else enableDisableContAns(Formul.elements[i],false);
	}
}

// Activate or deactivate response contents of a test question
function enableDisableContAns(Elem,IsDisabled) {
	var Tst_MAX_OPTIONS_PER_QUESTION = 10;
	for (var i=0; i<Tst_MAX_OPTIONS_PER_QUESTION; i++)
		if (Elem.name == ('AnsStr'+i) || Elem.name == ('FbStr'+i))
			Elem.disabled = IsDisabled;
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
/******************************** Draw a month *******************************/
/*****************************************************************************/

function DrawCurrentMonth (id,TimeUTC) {
	var d = new Date;

	d.setTime(TimeUTC * 1000);
	DrawMonth (id,d.getFullYear(),d.getMonth() + 1,d.getDate());
}

/*****************************************************************************/
/******************************** Draw a month *******************************/
/*****************************************************************************/

function DrawMonth (id,Year,Month,Today)
  {
   var NumDaysMonth = [
	 0,
	31,	//  1: January
	28,	//  2: February
	31,	//  3: Mars
	30,	//  4: April
	31,	//  5: May
	30,	//  6: June
	31,	//  7: July
	31,	//  8: Agoust
	30,	//  9: September
	31,	// 10: October
	30,	// 11: November
	31,	// 12: December
   ];
   // var StrExamOfX;
   var Week;
   var DayOfWeek; /* 0, 1, 2, 3, 4, 5, 6 */
   var Day;
   var NumDaysInMonth;
   var Yea = Year;
   var Mon = Month;
   // var YYYYMMDD;
   // var NumHld;
   var ClassForDay;		// Class of day depending on type of day
   // var TextForDay;		// Text associated to a day, for example the name of the holiday
   // var NumExamAnnouncement;	// Number of exam announcement
   // var ResultOfCmpStartDate;
   // var ContinueSearching;
   var ThisDayHasEvent = false;
   var IsToday;

   /***** Compute number of day of month for the first box *****/
   /* The initial day of month can be -5, -4, -3, -2, -1, 0, or 1
      If it's -5 then write 6 boxes of the previous month.
      If it's -4 then write 5 boxes of the previous month.
      If it's -3 then write 4 boxes of the previous month.
      If it's -2 then write 3 boxes of the previous month.
      If it's -1 then write 2 boxes of the previous month.
      If it's  0 then write 1 box   of the previous month.
      If it's  1 then write 0 boxes of the previous month. */

   if ((DayOfWeek = GetDayOfWeek (Yea,Mon,1)) == 0)
      Day = 1;
   else
     {
      if (Mon <= 1)
	{
	 Mon = 12;
	 Yea--;
	}
      else
	 Mon--;
      NumDaysInMonth = (Mon == 2) ? GetNumDaysFebruary (Yea) :
	                            NumDaysMonth[Mon];
      Day = NumDaysInMonth - DayOfWeek + 1;
     }

   /***** Start of month *****/
   HTMLContent = '<div class="MONTH_CONTAINER">';

   /***** Month name *****/
   HTMLContent += '<div class="MONTH">' +
                  MONTHS_CAPS[Month-1] + ' ' + Year +
                  '</div>';

   /***** Month head: first letter for each day of week *****/
   HTMLContent += '<table class="MONTH_TABLE_DAYS">'
   HTMLContent += '<tr>';
   for (DayOfWeek = 0;
	DayOfWeek < 7;
	DayOfWeek++)
      HTMLContent += '<td class="' +
                     ((DayOfWeek == 6) ? 'DAY_NO_WRK_HEAD' :
        	                         'DAY_WRK_HEAD') +
                     '">' +
                     DAYS_CAPS[DayOfWeek] +
                     '</td>';
   HTMLContent += '</tr>';

   /***** Draw every week of the month *****/
   for (Week = 0;
	Week < 6;
	Week++)
     {
      HTMLContent += '<tr>';

      /***** Draw every day of the week *****/
      for (DayOfWeek = 0;
	   DayOfWeek < 7;
	   DayOfWeek++)
	{
         /***** Set class for day being drawn *****/
         ClassForDay = (Mon == Month) ? 'DAY_WRK' :
                                        'DAY_WRK_LIGHT';
         /* Day being drawn is sunday? */
	 if (DayOfWeek == 6) // All the sundays are holidays
	    ClassForDay = (Mon == Month) ? 'DAY_HLD' :
		                           'DAY_HLD_LIGHT';

         /* Date being drawn is today? */
         /*
	 IsToday = (Gbl.CurrentAct != ActPrnCal && Mon == Month &&
                    Yea == Gbl.Now.Date.Yea &&
                    Mon == Gbl.Now.Date.Month &&
                    Day == Gbl.Now.Date.Day);
         */
         IsToday = (Yea == Year  &&
                    Mon == Month &&
                    Day == Today);

         /* Check if day has an exam announcement */
         /*
         ThisDayHasEvent = false;
	 if (!DrawingCalendar || Mon == Month)	// If drawing calendar and the month is not the real one, don't draw exam announcements
	    for (NumExamAnnouncement = 0;
		 NumExamAnnouncement < Gbl.LstExamAnnouncements.NumExamAnnounc;
		 NumExamAnnouncement++)
               if (Yea == Gbl.LstExamAnnouncements.Lst[NumExamAnnouncement].Year &&
                   Mon == Gbl.LstExamAnnouncements.Lst[NumExamAnnouncement].Month &&
                   Day == Gbl.LstExamAnnouncements.Lst[NumExamAnnouncement].Day)
                 {
		  ThisDayHasEvent = true;
		  if (PutLinkToEvents)
                    {
                     sprintf (StrExamOfX,Txt_Exam_of_X,Gbl.CurrentCrs.Crs.FullName);
   	             sprintf (Gbl.Title,"%s: %02u/%02u/%04u",
                              StrExamOfX,
                              Gbl.LstExamAnnouncements.Lst[NumExamAnnouncement].Day,
                              Gbl.LstExamAnnouncements.Lst[NumExamAnnouncement].Month,
                              Gbl.LstExamAnnouncements.Lst[NumExamAnnouncement].Yea);
                    }
                  break;
                 }
         */

         /***** Write the box with the day *****/
         HTMLContent += '<td class="' +
                        (IsToday ? (ThisDayHasEvent ? 'TODAY_EVENT' :
                	                              'TODAY') :
                                   (ThisDayHasEvent ? 'DAY_EVENT' :
                        	                      'DAY'  )) +
                        '">';

         /* If day has an exam announcement */
	 /* if (PutLinkToEvents && ThisDayHasEvent)
           {
            Act_FormStart (ActSeeExaAnn);
            fprintf (Gbl.F.Out,"<table style=\"width:100%%;\">"
                               "<tr>"
                               "<td class=\"%s\">",
                     ClassForDay);
            Act_LinkFormSubmit (Gbl.Title,ClassForDay);
           }
         else
           {
         */
            HTMLContent += '<div class="' + ClassForDay + '"';
            /*
            if (TextForDay)
	       fprintf (Gbl.F.Out," title=\"%s\"",TextForDay);
	    */
	    HTMLContent += '>';
	 /*
           }
         */

	 /* Write the day of month */
	 HTMLContent += Day;

         /* If day has an exam announcement */
         /*
	 if (PutLinkToEvents && ThisDayHasEvent)
	   {
            fprintf (Gbl.F.Out,"</a>"
        	               "</td>"
        	               "</tr>"
        	               "</table>");
	    Act_FormEnd ();
	   }
         else
         */
            HTMLContent += '</div>';

	 HTMLContent += '</td>';

         /***** Set the next day *****/
	 NumDaysInMonth = (Mon == 2) ? GetNumDaysFebruary (Yea) :
	                               NumDaysMonth[Mon];
	 if (++Day > NumDaysInMonth)
	   {
	    if (++Mon > 12)
	      {
	       Yea++;
	       Mon = 1;
	      }
	    Day = 1;
	   }
	}
      HTMLContent += '</tr>';
     }

   /***** End of month *****/
   HTMLContent += '</table></div>';
   
   document.getElementById(id).innerHTML = HTMLContent;
  }

/*****************************************************************************/
/***************** Compute day of the week from a given date *****************/
/*****************************************************************************/
// Return 0 for monday, 1 for tuesday,... 6 for sunday

function GetDayOfWeek (Year,Month,Day)
  {
   if (Month <= 2)
     {
      Month += 12;
      Year--;
     }
   return (
          	(
          		(
          			Day+
          			(Month*2)+
          			Math.floor(((Month+1)*3)/5)+
          			Year+
          			(
          				Math.floor(Year/4)-
          			 	Math.floor(Year/100)+
          			 	Math.floor(Year/400)
          			 )
          			+2
          		) % 7
          	) + 5
          ) % 7;
  }
  
/*****************************************************************************/
/****************** Return the number of days of february ********************/
/*****************************************************************************/

function GetNumDaysFebruary (Year)
  {
   return (GetIfLeapYear (Year) ? 29 :
	                          28);
  }

/*****************************************************************************/
/************************* Return true if year is leap ***********************/
/*****************************************************************************/

function GetIfLeapYear (Year)
  {
   return (Year % 4 == 0) && ((Year % 100 != 0) || (Year % 400 == 0));
  }
