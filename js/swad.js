// swad.js: javascript functions

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.
    Copyright (C) 1999-2012 Antonio Cañas-Vargas
    & Daniel J. Calandria-Hernández,
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

// Global variables used in writeClock()
var IsToday;
var StrToday;
var Hour;
var Minute;

// Global variables used in writeClockConnected()
var NumUsrsCon;
var ListSeconds = new Array();
var countClockConnected = 0;

// Write a clock updated every minute
// IsToday, StrToday, Hour and Minute are global variables initialized before call this function
function writeClock() {
	var StrHour = Hour;
	var StrMinute = Minute;
	var MidnightExceeded = false;
        var PrintableClock;
	if (Minute < 10)
		StrMinute = "0" + StrMinute;
	if (++Minute == 60) {
		Minute = 0;
		if (++Hour == 24) {
			Hour = 0;
			MidnightExceeded = true;
		}
	}
        if (IsToday)
		PrintableClock = StrToday + ", " + StrHour + ":" + StrMinute;
        else
		PrintableClock = StrHour + ":" + StrMinute;
        if (MidnightExceeded)
		IsToday = false;	// For next call
	document.getElementById('hm').innerHTML = PrintableClock;
	setTimeout("writeClock()",60000);
}
      
function writeClockConnected() {
        var BoxClock;
	var Hours;
	var Minutes;
	var Seconds;
	var StrMinutes;
	var StrSeconds;
        var PrintableClock;

        countClockConnected++;
        countClockConnected %= 10;
	for (var i=0; i<NumUsrsCon; i++) {
		BoxClock = document.getElementById('hm'+i);
		if (BoxClock) {
			ListSeconds[i] += 1;
			if (!countClockConnected) {	// Print after 10 seconds
				Minutes = Math.floor(ListSeconds[i] / 60);
				if (Minutes >= 60) {
					Hours = Math.floor(Minutes / 60);
					Minutes %= 60;
				} else
					Hours = 0;
				Seconds = ListSeconds[i] % 60;
				if (Hours != 0) {
					StrMinutes = ((Minutes < 10) ? "0" : "") + Minutes;
					StrSeconds = ((Seconds < 10) ? "0" : "") + Seconds;
					PrintableClock = Hours + ":" + StrMinutes + "'" + StrSeconds + "&quot;";
				} else if (Minutes != 0) {
					StrSeconds = ((Seconds < 10) ? "0" : "") + Seconds;
					PrintableClock = Minutes + "'" + StrSeconds + "&quot;";
				} else
					PrintableClock = Seconds + "&quot;";
				BoxClock.innerHTML = PrintableClock;
			}
		}
	}
	setTimeout("writeClockConnected()",1000);	// refresh after 1 second
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
			obj = new ActiveXObject("Msxml2.XMLHTTP");
		} catch (e) {
			try {
				obj = new ActiveXObject("Microsoft.XMLHTTP");
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

			if (delay < 10000) delay = 10000;	// Don't refresh faster than 1 time each 10 seconds
			setTimeout("refreshConnected()",delay);
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
			if (delay < 200) delay = 200;	// Don't refresh faster than 1 time each 0.2 seconds
			setTimeout("refreshLastClicks()",delay);
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
        xPos -= (150+12);
        yPos -= ((200+12)/2);
        if (yPos < 0)
           yPos = 0;
	document.getElementById('zoomLyr').style.left = xPos + 'px';
	document.getElementById('zoomLyr').style.top = yPos + 'px';
	document.getElementById('zoomImg').src = urlPhoto;
	document.getElementById('zoomTxt').innerHTML = '<span class="ZOOM_TXT">' + shortName + '</span>';
}

// Exit from zooming a user's photograph
function noZoom(imagen) {
	var xPos = -(150+12);
	var yPos = -(200+12+88);
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

// Adjust a date form correcting days in the month
function adjustDateForm (DayForm,MonthForm,YearForm) {
	var Days = 31;
	var Year = YearForm.options[YearForm.selectedIndex].value;
	if (MonthForm.options[2].selected)			// Adjust days of february
		{ if ((((Year % 4) == 0) && ((Year % 100) != 0)) || ((Year % 400) == 0)) Days = 29; else Days = 28; }
	else if (MonthForm.options[4].selected || MonthForm.options[6].selected || MonthForm.options[9].selected || MonthForm.options[11].selected) Days = 30;
	if (DayForm.selectedIndex > Days) DayForm.options[Days].selected = true;
	for (var i=DayForm.options.length; i<=Days ; i++)	// Create new days at start
		{ var x = String (i); DayForm.options[i] = new Option(x,x); }
	for (var i=DayForm.options.length-1; i>Days; i--)	// Remove days at the end
		DayForm.options[i] = null;
}

// Set a the date in a date form to a specified date  
function setDateTo (elem,Day,Month,Year) {
	document.getElementById('StartYear').options[Year].selected = true;
	document.getElementById('StartMonth').options[Month].selected = true;
	adjustDateForm (elem.form.StartDay,elem.form.StartMonth,elem.form.StartYear)
	document.getElementById('StartDay').options[Day].selected = true;
	document.getElementById('EndYear').options[Year].selected = true;
	document.getElementById('EndMonth').options[Month].selected = true;
	adjustDateForm (elem.form.EndDay,elem.form.EndMonth,elem.form.EndYear)
	document.getElementById('EndDay').options[Day].selected = true;
}

// Adjust iFrame height to height of browser
function iFrameHeight () {
	var h;
	if (document.getElementById) {
		if (self.innerHeight) // all except Explorer
			h = self.innerHeight;
		else if (document.documentElement && document.documentElement.clientHeight)
			// Explorer 6 Strict Mode
			h = document.documentElement.clientHeight;
		else if (document.body) // other Explorers
			h = document.body.clientHeight;
		if (h == 0) h = 480;
		else {
			h = h - 210;
			if (h < 200) h = 200;
		}
		document.getElementById('iframe_central').style.height = h;
	}
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
function enableRowsPage () {
	document.getElementById('CountType').disabled = true;
	document.getElementById('RowsPage').disabled = false;
}
function disableRowsPage () {
	document.getElementById('CountType').disabled = false;
	document.getElementById('RowsPage').disabled = true;
}
