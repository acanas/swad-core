// swad_action.h: actions

#ifndef _SWAD_ACT
#define _SWAD_ACT
/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2016 Antonio Cañas Vargas

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
/*****************************************************************************/
/********************************** Headers **********************************/
/*****************************************************************************/

#include <stdbool.h>		// For boolean type

#include "swad_cryptography.h"
#include "swad_text.h"

/*****************************************************************************/
/****************************** Public constants *****************************/
/*****************************************************************************/

#define Act_MAX_LENGTH_ACTION_TXT	255

// Related with tabs and actions
typedef enum
  {
   TabUnk =  0,
   TabSys =  1,
   TabCty =  2,
   TabIns =  3,
   TabCtr =  4,
   TabDeg =  5,
   TabCrs =  6,
   TabAss =  7,
   TabUsr =  8,
   TabSoc =  9,
   TabMsg = 10,
   TabSta = 11,
   TabPrf = 12,
  } Act_Tab_t;

typedef enum
  {
   Act_CONTENT_NORM,
   Act_CONTENT_DATA,
  } Act_Content_t;

typedef enum
  {
   Act_MAIN_WINDOW,
   Act_NEW_WINDOW,
   Act_UPLOAD_FILE,	// Upload a file. Do not write HTML content. Write Status code instead for Dropzone.js
   Act_DOWNLD_FILE,	// Download a file in a new window. Do not write HTML content.
  } Act_BrowserWindow_t;

typedef int Act_Action_t;	// Must be a signed type, because -1 is used to indicate obsolete action

#define Act_NUM_ACTIONS	(1+9+51+15+90+70+67+245+184+144+172+36+28+83)

#define Act_MAX_ACTION_COD 1564

#define Act_MAX_OPTIONS_IN_MENU_PER_TAB 20

#define Act_MAX_LENGTH_ID	(32+Cry_LENGTH_ENCRYPTED_STR_SHA256_BASE64+10+1)

/*****************************************************************************/
/************************* Not asociates with tabs ***************************/
/*****************************************************************************/
#define ActAll			 0

#define ActUnk			 1
#define ActHom			 2
#define ActMnu			 3

// The following 5 actions use AJAX to refresh only a part of the page
#define ActRefCon		 4
#define ActRefLstClk		 5
#define ActRefNewSocPubGbl	 6
#define ActRefOldSocPubGbl	 7
#define ActRefOldSocPubUsr	 8

#define ActWebSvc		 9

/*****************************************************************************/
/******************************** System tab *********************************/
/*****************************************************************************/
// Actions in menu
#define ActSysReqSch		(ActWebSvc+  1)
#define ActSeeCty		(ActWebSvc+  2)
#define ActSeePen		(ActWebSvc+  3)
#define ActReqRemOldCrs		(ActWebSvc+  4)
#define ActSeeDegTyp		(ActWebSvc+  5)
#define ActSeeMai		(ActWebSvc+  6)
#define ActSeeBan		(ActWebSvc+  7)
#define ActSeeLnk		(ActWebSvc+  8)
#define ActLstPlg		(ActWebSvc+  9)
#define ActSetUp		(ActWebSvc+ 10)
// Secondary actions
#define ActSysSch		(ActWebSvc+ 11)
#define ActEdiDegTyp		(ActWebSvc+ 12)
#define ActEdiCty		(ActWebSvc+ 13)
#define ActNewCty		(ActWebSvc+ 14)
#define ActRemCty		(ActWebSvc+ 15)
#define ActRenCty		(ActWebSvc+ 16)
#define ActChgCtyWWW		(ActWebSvc+ 17)

#define ActNewDegTyp		(ActWebSvc+ 18)
#define ActRemDegTyp		(ActWebSvc+ 19)
#define ActRenDegTyp		(ActWebSvc+ 20)

#define ActRemOldCrs		(ActWebSvc+ 21)
#define ActEdiMai		(ActWebSvc+ 22)
#define ActNewMai		(ActWebSvc+ 23)
#define ActRemMai		(ActWebSvc+ 24)
#define ActRenMaiSho		(ActWebSvc+ 25)
#define ActRenMaiFul		(ActWebSvc+ 26)

#define ActEdiBan		(ActWebSvc+ 27)
#define ActNewBan		(ActWebSvc+ 28)
#define ActRemBan		(ActWebSvc+ 29)
#define ActShoBan		(ActWebSvc+ 30)
#define ActHidBan		(ActWebSvc+ 31)
#define ActRenBanSho		(ActWebSvc+ 32)
#define ActRenBanFul		(ActWebSvc+ 33)
#define ActChgBanImg		(ActWebSvc+ 34)
#define ActChgBanWWW		(ActWebSvc+ 35)
#define ActClkBan		(ActWebSvc+ 36)

#define ActEdiLnk		(ActWebSvc+ 37)
#define ActNewLnk		(ActWebSvc+ 38)
#define ActRemLnk		(ActWebSvc+ 39)
#define ActRenLnkSho		(ActWebSvc+ 40)
#define ActRenLnkFul		(ActWebSvc+ 41)
#define ActChgLnkWWW		(ActWebSvc+ 42)

#define ActEdiPlg		(ActWebSvc+ 43)
#define ActNewPlg		(ActWebSvc+ 44)
#define ActRemPlg		(ActWebSvc+ 45)
#define ActRenPlg		(ActWebSvc+ 46)
#define ActChgPlgDes		(ActWebSvc+ 47)
#define ActChgPlgLog		(ActWebSvc+ 48)
#define ActChgPlgAppKey		(ActWebSvc+ 49)
#define ActChgPlgURL		(ActWebSvc+ 50)
#define ActChgPlgIP		(ActWebSvc+ 51)

/*****************************************************************************/
/******************************** Country tab ********************************/
/*****************************************************************************/
// Actions in menu
#define ActCtyReqSch		(ActChgPlgIP+  1)
#define ActSeeCtyInf		(ActChgPlgIP+  2)
#define ActSeeIns		(ActChgPlgIP+  3)

// Secondary actions
#define ActCtySch		(ActChgPlgIP+  4)
#define ActPrnCtyInf		(ActChgPlgIP+  5)
#define ActChgCtyMapAtt		(ActChgPlgIP+  6)

#define ActEdiIns		(ActChgPlgIP+  7)
#define ActReqIns		(ActChgPlgIP+  8)
#define ActNewIns		(ActChgPlgIP+  9)
#define ActRemIns		(ActChgPlgIP+ 10)
#define ActChgInsCty		(ActChgPlgIP+ 11)
#define ActRenInsSho		(ActChgPlgIP+ 12)
#define ActRenInsFul		(ActChgPlgIP+ 13)
#define ActChgInsWWW		(ActChgPlgIP+ 14)
#define ActChgInsSta		(ActChgPlgIP+ 15)

/*****************************************************************************/
/****************************** Institution tab ******************************/
/*****************************************************************************/
// Actions in menu
#define ActInsReqSch		(ActChgInsSta+  1)
#define ActSeeInsInf		(ActChgInsSta+  2)
#define ActSeeCtr		(ActChgInsSta+  3)
#define ActSeeDpt		(ActChgInsSta+  4)
#define ActSeePlc		(ActChgInsSta+  5)
#define ActSeeHld		(ActChgInsSta+  6)
#define ActSeeAdmDocIns		(ActChgInsSta+  7)
#define ActAdmShaIns		(ActChgInsSta+  8)

// Secondary actions
#define ActInsSch		(ActChgInsSta+  9)
#define ActPrnInsInf		(ActChgInsSta+ 10)
#define ActReqInsLog		(ActChgInsSta+ 11)
#define ActRecInsLog		(ActChgInsSta+ 12)
#define ActRemInsLog		(ActChgInsSta+ 13)

#define ActEdiCtr		(ActChgInsSta+ 14)
#define ActReqCtr		(ActChgInsSta+ 15)
#define ActNewCtr		(ActChgInsSta+ 16)
#define ActRemCtr		(ActChgInsSta+ 17)
#define ActChgCtrIns		(ActChgInsSta+ 18)
#define ActChgCtrPlc		(ActChgInsSta+ 19)
#define ActRenCtrSho		(ActChgInsSta+ 20)
#define ActRenCtrFul		(ActChgInsSta+ 21)
#define ActChgCtrWWW		(ActChgInsSta+ 22)
#define ActChgCtrSta		(ActChgInsSta+ 23)

#define ActEdiDpt		(ActChgInsSta+ 24)
#define ActNewDpt		(ActChgInsSta+ 25)
#define ActRemDpt		(ActChgInsSta+ 26)
#define ActChgDptIns		(ActChgInsSta+ 27)
#define ActRenDptSho		(ActChgInsSta+ 28)
#define ActRenDptFul		(ActChgInsSta+ 29)
#define ActChgDptWWW		(ActChgInsSta+ 30)

#define ActEdiPlc		(ActChgInsSta+ 31)
#define ActNewPlc		(ActChgInsSta+ 32)
#define ActRemPlc		(ActChgInsSta+ 33)
#define ActRenPlcSho		(ActChgInsSta+ 34)
#define ActRenPlcFul		(ActChgInsSta+ 35)

#define ActEdiHld		(ActChgInsSta+ 36)
#define ActNewHld		(ActChgInsSta+ 37)
#define ActRemHld		(ActChgInsSta+ 38)
#define ActChgHldPlc		(ActChgInsSta+ 39)
#define ActChgHldTyp		(ActChgInsSta+ 40)
#define ActChgHldStrDat		(ActChgInsSta+ 41)
#define ActChgHldEndDat		(ActChgInsSta+ 42)
#define ActRenHld		(ActChgInsSta+ 43)

#define ActChgToSeeDocIns	(ActChgInsSta+ 44)
#define ActSeeDocIns		(ActChgInsSta+ 45)
#define ActExpSeeDocIns		(ActChgInsSta+ 46)
#define ActConSeeDocIns		(ActChgInsSta+ 47)
#define ActZIPSeeDocIns		(ActChgInsSta+ 48)
#define ActReqDatSeeDocIns	(ActChgInsSta+ 49)
#define ActDowSeeDocIns		(ActChgInsSta+ 50)
#define ActChgToAdmDocIns	(ActChgInsSta+ 51)
#define ActAdmDocIns		(ActChgInsSta+ 52)
#define ActReqRemFilDocIns	(ActChgInsSta+ 53)
#define ActRemFilDocIns		(ActChgInsSta+ 54)
#define ActRemFolDocIns		(ActChgInsSta+ 55)
#define ActCopDocIns		(ActChgInsSta+ 56)
#define ActPasDocIns		(ActChgInsSta+ 57)
#define ActRemTreDocIns		(ActChgInsSta+ 58)
#define ActFrmCreDocIns		(ActChgInsSta+ 59)
#define ActCreFolDocIns		(ActChgInsSta+ 60)
#define ActCreLnkDocIns		(ActChgInsSta+ 61)
#define ActRenFolDocIns		(ActChgInsSta+ 62)
#define ActRcvFilDocInsDZ	(ActChgInsSta+ 63)
#define ActRcvFilDocInsCla	(ActChgInsSta+ 64)
#define ActExpAdmDocIns		(ActChgInsSta+ 65)
#define ActConAdmDocIns		(ActChgInsSta+ 66)
#define ActZIPAdmDocIns		(ActChgInsSta+ 67)
#define ActShoDocIns		(ActChgInsSta+ 68)
#define ActHidDocIns		(ActChgInsSta+ 69)
#define ActReqDatAdmDocIns	(ActChgInsSta+ 70)
#define ActChgDatAdmDocIns	(ActChgInsSta+ 71)
#define ActDowAdmDocIns		(ActChgInsSta+ 72)

#define ActReqRemFilShaIns	(ActChgInsSta+ 73)
#define ActRemFilShaIns		(ActChgInsSta+ 74)
#define ActRemFolShaIns		(ActChgInsSta+ 75)
#define ActCopShaIns		(ActChgInsSta+ 76)
#define ActPasShaIns		(ActChgInsSta+ 77)
#define ActRemTreShaIns		(ActChgInsSta+ 78)
#define ActFrmCreShaIns		(ActChgInsSta+ 79)
#define ActCreFolShaIns		(ActChgInsSta+ 80)
#define ActCreLnkShaIns		(ActChgInsSta+ 81)
#define ActRenFolShaIns		(ActChgInsSta+ 82)
#define ActRcvFilShaInsDZ	(ActChgInsSta+ 83)
#define ActRcvFilShaInsCla	(ActChgInsSta+ 84)
#define ActExpShaIns		(ActChgInsSta+ 85)
#define ActConShaIns		(ActChgInsSta+ 86)
#define ActZIPShaIns		(ActChgInsSta+ 87)
#define ActReqDatShaIns		(ActChgInsSta+ 88)
#define ActChgDatShaIns		(ActChgInsSta+ 89)
#define ActDowShaIns		(ActChgInsSta+ 90)

/*****************************************************************************/
/********************************* Centre tab ********************************/
/*****************************************************************************/
// Actions in menu
#define ActCtrReqSch		(ActDowShaIns+  1)
#define ActSeeCtrInf		(ActDowShaIns+  2)
#define ActSeeDeg		(ActDowShaIns+  3)
#define ActSeeAdmDocCtr		(ActDowShaIns+  4)
#define ActAdmShaCtr		(ActDowShaIns+  5)

// Secondary actions
#define ActCtrSch		(ActDowShaIns+  6)
#define ActPrnCtrInf		(ActDowShaIns+  7)
#define ActReqCtrLog		(ActDowShaIns+  8)
#define ActRecCtrLog		(ActDowShaIns+  9)
#define ActRemCtrLog		(ActDowShaIns+ 10)
#define ActReqCtrPho		(ActDowShaIns+ 11)
#define ActRecCtrPho		(ActDowShaIns+ 12)
#define ActChgCtrPhoAtt		(ActDowShaIns+ 13)

#define ActEdiDeg		(ActDowShaIns+ 14)
#define ActReqDeg		(ActDowShaIns+ 15)
#define ActNewDeg		(ActDowShaIns+ 16)
#define ActRemDeg		(ActDowShaIns+ 17)
#define ActRenDegSho		(ActDowShaIns+ 18)
#define ActRenDegFul		(ActDowShaIns+ 19)
#define ActChgDegTyp		(ActDowShaIns+ 20)
#define ActChgDegCtr		(ActDowShaIns+ 21)
#define ActChgDegWWW		(ActDowShaIns+ 22)
#define ActChgDegSta		(ActDowShaIns+ 23)

#define ActChgToSeeDocCtr	(ActDowShaIns+ 24)
#define ActSeeDocCtr		(ActDowShaIns+ 25)
#define ActExpSeeDocCtr		(ActDowShaIns+ 26)
#define ActConSeeDocCtr		(ActDowShaIns+ 27)
#define ActZIPSeeDocCtr		(ActDowShaIns+ 28)
#define ActReqDatSeeDocCtr	(ActDowShaIns+ 29)
#define ActDowSeeDocCtr		(ActDowShaIns+ 30)
#define ActChgToAdmDocCtr	(ActDowShaIns+ 31)
#define ActAdmDocCtr		(ActDowShaIns+ 32)
#define ActReqRemFilDocCtr	(ActDowShaIns+ 33)
#define ActRemFilDocCtr		(ActDowShaIns+ 34)
#define ActRemFolDocCtr		(ActDowShaIns+ 35)
#define ActCopDocCtr		(ActDowShaIns+ 36)
#define ActPasDocCtr		(ActDowShaIns+ 37)
#define ActRemTreDocCtr		(ActDowShaIns+ 38)
#define ActFrmCreDocCtr		(ActDowShaIns+ 39)
#define ActCreFolDocCtr		(ActDowShaIns+ 40)
#define ActCreLnkDocCtr		(ActDowShaIns+ 41)
#define ActRenFolDocCtr		(ActDowShaIns+ 42)
#define ActRcvFilDocCtrDZ	(ActDowShaIns+ 43)
#define ActRcvFilDocCtrCla	(ActDowShaIns+ 44)
#define ActExpAdmDocCtr		(ActDowShaIns+ 45)
#define ActConAdmDocCtr		(ActDowShaIns+ 46)
#define ActZIPAdmDocCtr		(ActDowShaIns+ 47)
#define ActShoDocCtr		(ActDowShaIns+ 48)
#define ActHidDocCtr		(ActDowShaIns+ 49)
#define ActReqDatAdmDocCtr	(ActDowShaIns+ 50)
#define ActChgDatAdmDocCtr	(ActDowShaIns+ 51)
#define ActDowAdmDocCtr		(ActDowShaIns+ 52)

#define ActReqRemFilShaCtr	(ActDowShaIns+ 53)
#define ActRemFilShaCtr		(ActDowShaIns+ 54)
#define ActRemFolShaCtr		(ActDowShaIns+ 55)
#define ActCopShaCtr		(ActDowShaIns+ 56)
#define ActPasShaCtr		(ActDowShaIns+ 57)
#define ActRemTreShaCtr		(ActDowShaIns+ 58)
#define ActFrmCreShaCtr		(ActDowShaIns+ 59)
#define ActCreFolShaCtr		(ActDowShaIns+ 60)
#define ActCreLnkShaCtr		(ActDowShaIns+ 61)
#define ActRenFolShaCtr		(ActDowShaIns+ 62)
#define ActRcvFilShaCtrDZ	(ActDowShaIns+ 63)
#define ActRcvFilShaCtrCla	(ActDowShaIns+ 64)
#define ActExpShaCtr		(ActDowShaIns+ 65)
#define ActConShaCtr		(ActDowShaIns+ 66)
#define ActZIPShaCtr		(ActDowShaIns+ 67)
#define ActReqDatShaCtr		(ActDowShaIns+ 68)
#define ActChgDatShaCtr		(ActDowShaIns+ 69)
#define ActDowShaCtr		(ActDowShaIns+ 70)

/*****************************************************************************/
/********************************* Degree tab ********************************/
/*****************************************************************************/
// Actions in menu
#define ActDegReqSch		(ActDowShaCtr+  1)
#define ActSeeDegInf		(ActDowShaCtr+  2)
#define ActSeeCrs		(ActDowShaCtr+  3)
#define ActSeeAdmDocDeg		(ActDowShaCtr+  4)
#define ActAdmShaDeg		(ActDowShaCtr+  5)

// Secondary actions
#define ActDegSch		(ActDowShaCtr+  6)
#define ActPrnDegInf		(ActDowShaCtr+  7)
#define ActReqDegLog		(ActDowShaCtr+  8)
#define ActRecDegLog		(ActDowShaCtr+  9)
#define ActRemDegLog		(ActDowShaCtr+ 10)

#define ActEdiCrs		(ActDowShaCtr+ 11)
#define ActReqCrs		(ActDowShaCtr+ 12)
#define ActNewCrs		(ActDowShaCtr+ 13)
#define ActRemCrs		(ActDowShaCtr+ 14)
#define ActChgInsCrsCod		(ActDowShaCtr+ 15)
#define ActChgCrsDeg		(ActDowShaCtr+ 16)
#define ActChgCrsYea		(ActDowShaCtr+ 17)
#define ActRenCrsSho		(ActDowShaCtr+ 18)
#define ActRenCrsFul		(ActDowShaCtr+ 19)
#define ActChgCrsSta		(ActDowShaCtr+ 20)

#define ActChgToSeeDocDeg	(ActDowShaCtr+ 21)
#define ActSeeDocDeg		(ActDowShaCtr+ 22)
#define ActExpSeeDocDeg		(ActDowShaCtr+ 23)
#define ActConSeeDocDeg		(ActDowShaCtr+ 24)
#define ActZIPSeeDocDeg		(ActDowShaCtr+ 25)
#define ActReqDatSeeDocDeg	(ActDowShaCtr+ 26)
#define ActDowSeeDocDeg		(ActDowShaCtr+ 27)
#define ActChgToAdmDocDeg	(ActDowShaCtr+ 28)
#define ActAdmDocDeg		(ActDowShaCtr+ 29)
#define ActReqRemFilDocDeg	(ActDowShaCtr+ 30)
#define ActRemFilDocDeg		(ActDowShaCtr+ 31)
#define ActRemFolDocDeg		(ActDowShaCtr+ 32)
#define ActCopDocDeg		(ActDowShaCtr+ 33)
#define ActPasDocDeg		(ActDowShaCtr+ 34)
#define ActRemTreDocDeg		(ActDowShaCtr+ 35)
#define ActFrmCreDocDeg		(ActDowShaCtr+ 36)
#define ActCreFolDocDeg		(ActDowShaCtr+ 37)
#define ActCreLnkDocDeg		(ActDowShaCtr+ 38)
#define ActRenFolDocDeg		(ActDowShaCtr+ 39)
#define ActRcvFilDocDegDZ	(ActDowShaCtr+ 40)
#define ActRcvFilDocDegCla	(ActDowShaCtr+ 41)
#define ActExpAdmDocDeg		(ActDowShaCtr+ 42)
#define ActConAdmDocDeg		(ActDowShaCtr+ 43)
#define ActZIPAdmDocDeg		(ActDowShaCtr+ 44)
#define ActShoDocDeg		(ActDowShaCtr+ 45)
#define ActHidDocDeg		(ActDowShaCtr+ 46)
#define ActReqDatAdmDocDeg	(ActDowShaCtr+ 47)
#define ActChgDatAdmDocDeg	(ActDowShaCtr+ 48)
#define ActDowAdmDocDeg		(ActDowShaCtr+ 49)

#define ActReqRemFilShaDeg	(ActDowShaCtr+ 50)
#define ActRemFilShaDeg		(ActDowShaCtr+ 51)
#define ActRemFolShaDeg		(ActDowShaCtr+ 52)
#define ActCopShaDeg		(ActDowShaCtr+ 53)
#define ActPasShaDeg		(ActDowShaCtr+ 54)
#define ActRemTreShaDeg		(ActDowShaCtr+ 55)
#define ActFrmCreShaDeg		(ActDowShaCtr+ 56)
#define ActCreFolShaDeg		(ActDowShaCtr+ 57)
#define ActCreLnkShaDeg		(ActDowShaCtr+ 58)
#define ActRenFolShaDeg		(ActDowShaCtr+ 59)
#define ActRcvFilShaDegDZ	(ActDowShaCtr+ 60)
#define ActRcvFilShaDegCla	(ActDowShaCtr+ 61)
#define ActExpShaDeg		(ActDowShaCtr+ 62)
#define ActConShaDeg		(ActDowShaCtr+ 63)
#define ActZIPShaDeg		(ActDowShaCtr+ 64)
#define ActReqDatShaDeg		(ActDowShaCtr+ 65)
#define ActChgDatShaDeg		(ActDowShaCtr+ 66)
#define ActDowShaDeg		(ActDowShaCtr+ 67)

/*****************************************************************************/
/******************************** Course tab *********************************/
/*****************************************************************************/
// Actions in menu
#define ActCrsReqSch		(ActDowShaDeg+  1)
#define ActSeeCrsInf		(ActDowShaDeg+  2)
#define ActSeeTchGui		(ActDowShaDeg+  3)
#define ActSeeSyl		(ActDowShaDeg+  4)
#define ActSeeAdmDocCrsGrp	(ActDowShaDeg+  5)
#define ActAdmTchCrsGrp		(ActDowShaDeg+  6)
#define ActAdmShaCrsGrp		(ActDowShaDeg+  7)
#define ActSeeCrsTT		(ActDowShaDeg+  8)
#define ActSeeBib		(ActDowShaDeg+  9)
#define ActSeeFAQ		(ActDowShaDeg+ 10)
#define ActSeeCrsLnk		(ActDowShaDeg+ 11)
// Secondary actions
#define ActCrsSch		(ActDowShaDeg+ 12)
#define ActPrnCrsInf		(ActDowShaDeg+ 13)
#define ActChgCrsLog		(ActDowShaDeg+ 14)
#define ActEdiCrsInf		(ActDowShaDeg+ 15)
#define ActEdiTchGui		(ActDowShaDeg+ 16)
#define ActPrnCrsTT		(ActDowShaDeg+ 17)
#define ActEdiCrsTT		(ActDowShaDeg+ 18)
#define ActChgCrsTT		(ActDowShaDeg+ 19)
#define ActChgCrsTT1stDay	(ActDowShaDeg+ 20)
#define ActSeeSylLec		(ActDowShaDeg+ 21)
#define ActSeeSylPra		(ActDowShaDeg+ 22)
#define ActEdiSylLec		(ActDowShaDeg+ 23)
#define ActEdiSylPra		(ActDowShaDeg+ 24)
#define ActDelItmSylLec		(ActDowShaDeg+ 25)
#define ActDelItmSylPra		(ActDowShaDeg+ 26)
#define ActUp_IteSylLec		(ActDowShaDeg+ 27)
#define ActUp_IteSylPra		(ActDowShaDeg+ 28)
#define ActDwnIteSylLec		(ActDowShaDeg+ 29)
#define ActDwnIteSylPra		(ActDowShaDeg+ 30)
#define ActRgtIteSylLec		(ActDowShaDeg+ 31)
#define ActRgtIteSylPra		(ActDowShaDeg+ 32)
#define ActLftIteSylLec		(ActDowShaDeg+ 33)
#define ActLftIteSylPra		(ActDowShaDeg+ 34)
#define ActInsIteSylLec		(ActDowShaDeg+ 35)
#define ActInsIteSylPra		(ActDowShaDeg+ 36)
#define ActModIteSylLec		(ActDowShaDeg+ 37)
#define ActModIteSylPra		(ActDowShaDeg+ 38)

#define ActChgToSeeDocCrs	(ActDowShaDeg+ 39)
#define ActSeeDocCrs		(ActDowShaDeg+ 40)
#define ActExpSeeDocCrs		(ActDowShaDeg+ 41)
#define ActConSeeDocCrs		(ActDowShaDeg+ 42)
#define ActZIPSeeDocCrs		(ActDowShaDeg+ 43)
#define ActReqDatSeeDocCrs	(ActDowShaDeg+ 44)
#define ActDowSeeDocCrs		(ActDowShaDeg+ 45)

#define ActSeeDocGrp		(ActDowShaDeg+ 46)
#define ActExpSeeDocGrp		(ActDowShaDeg+ 47)
#define ActConSeeDocGrp		(ActDowShaDeg+ 48)
#define ActZIPSeeDocGrp		(ActDowShaDeg+ 49)
#define ActReqDatSeeDocGrp	(ActDowShaDeg+ 50)
#define ActDowSeeDocGrp		(ActDowShaDeg+ 51)

#define ActChgToAdmDocCrs	(ActDowShaDeg+ 52)
#define ActAdmDocCrs		(ActDowShaDeg+ 53)
#define ActReqRemFilDocCrs	(ActDowShaDeg+ 54)
#define ActRemFilDocCrs		(ActDowShaDeg+ 55)
#define ActRemFolDocCrs		(ActDowShaDeg+ 56)
#define ActCopDocCrs		(ActDowShaDeg+ 57)
#define ActPasDocCrs		(ActDowShaDeg+ 58)
#define ActRemTreDocCrs		(ActDowShaDeg+ 59)
#define ActFrmCreDocCrs		(ActDowShaDeg+ 60)
#define ActCreFolDocCrs		(ActDowShaDeg+ 61)
#define ActCreLnkDocCrs		(ActDowShaDeg+ 62)
#define ActRenFolDocCrs		(ActDowShaDeg+ 63)
#define ActRcvFilDocCrsDZ	(ActDowShaDeg+ 64)
#define ActRcvFilDocCrsCla	(ActDowShaDeg+ 65)
#define ActExpAdmDocCrs		(ActDowShaDeg+ 66)
#define ActConAdmDocCrs		(ActDowShaDeg+ 67)
#define ActZIPAdmDocCrs		(ActDowShaDeg+ 68)
#define ActShoDocCrs		(ActDowShaDeg+ 69)
#define ActHidDocCrs		(ActDowShaDeg+ 70)
#define ActReqDatAdmDocCrs	(ActDowShaDeg+ 71)
#define ActChgDatAdmDocCrs	(ActDowShaDeg+ 72)
#define ActDowAdmDocCrs		(ActDowShaDeg+ 73)

#define ActAdmDocGrp		(ActDowShaDeg+ 74)
#define ActReqRemFilDocGrp	(ActDowShaDeg+ 75)
#define ActRemFilDocGrp		(ActDowShaDeg+ 76)
#define ActRemFolDocGrp		(ActDowShaDeg+ 77)
#define ActCopDocGrp		(ActDowShaDeg+ 78)
#define ActPasDocGrp		(ActDowShaDeg+ 79)
#define ActRemTreDocGrp		(ActDowShaDeg+ 80)
#define ActFrmCreDocGrp		(ActDowShaDeg+ 81)
#define ActCreFolDocGrp		(ActDowShaDeg+ 82)
#define ActCreLnkDocGrp		(ActDowShaDeg+ 83)
#define ActRenFolDocGrp		(ActDowShaDeg+ 84)
#define ActRcvFilDocGrpDZ	(ActDowShaDeg+ 85)
#define ActRcvFilDocGrpCla	(ActDowShaDeg+ 86)
#define ActExpAdmDocGrp		(ActDowShaDeg+ 87)
#define ActConAdmDocGrp		(ActDowShaDeg+ 88)
#define ActZIPAdmDocGrp		(ActDowShaDeg+ 89)
#define ActShoDocGrp		(ActDowShaDeg+ 90)
#define ActHidDocGrp		(ActDowShaDeg+ 91)
#define ActReqDatAdmDocGrp	(ActDowShaDeg+ 92)
#define ActChgDatAdmDocGrp	(ActDowShaDeg+ 93)
#define ActDowAdmDocGrp		(ActDowShaDeg+ 94)

#define ActChgToAdmTch		(ActDowShaDeg+ 95)

#define ActAdmTchCrs		(ActDowShaDeg+ 96)
#define ActReqRemFilTchCrs	(ActDowShaDeg+ 97)
#define ActRemFilTchCrs		(ActDowShaDeg+ 98)
#define ActRemFolTchCrs		(ActDowShaDeg+ 99)
#define ActCopTchCrs		(ActDowShaDeg+100)
#define ActPasTchCrs		(ActDowShaDeg+101)
#define ActRemTreTchCrs		(ActDowShaDeg+102)
#define ActFrmCreTchCrs		(ActDowShaDeg+103)
#define ActCreFolTchCrs		(ActDowShaDeg+104)
#define ActCreLnkTchCrs		(ActDowShaDeg+105)
#define ActRenFolTchCrs		(ActDowShaDeg+106)
#define ActRcvFilTchCrsDZ	(ActDowShaDeg+107)
#define ActRcvFilTchCrsCla	(ActDowShaDeg+108)
#define ActExpTchCrs		(ActDowShaDeg+109)
#define ActConTchCrs		(ActDowShaDeg+110)
#define ActZIPTchCrs		(ActDowShaDeg+111)
#define ActReqDatTchCrs		(ActDowShaDeg+112)
#define ActChgDatTchCrs		(ActDowShaDeg+113)
#define ActDowTchCrs		(ActDowShaDeg+114)

#define ActAdmTchGrp		(ActDowShaDeg+115)
#define ActReqRemFilTchGrp	(ActDowShaDeg+116)
#define ActRemFilTchGrp		(ActDowShaDeg+117)
#define ActRemFolTchGrp		(ActDowShaDeg+118)
#define ActCopTchGrp		(ActDowShaDeg+119)
#define ActPasTchGrp		(ActDowShaDeg+120)
#define ActRemTreTchGrp		(ActDowShaDeg+121)
#define ActFrmCreTchGrp		(ActDowShaDeg+122)
#define ActCreFolTchGrp		(ActDowShaDeg+123)
#define ActCreLnkTchGrp		(ActDowShaDeg+124)
#define ActRenFolTchGrp		(ActDowShaDeg+125)
#define ActRcvFilTchGrpDZ	(ActDowShaDeg+126)
#define ActRcvFilTchGrpCla	(ActDowShaDeg+127)
#define ActExpTchGrp		(ActDowShaDeg+128)
#define ActConTchGrp		(ActDowShaDeg+129)
#define ActZIPTchGrp		(ActDowShaDeg+130)
#define ActReqDatTchGrp		(ActDowShaDeg+131)
#define ActChgDatTchGrp		(ActDowShaDeg+132)
#define ActDowTchGrp		(ActDowShaDeg+133)

#define ActChgToAdmSha		(ActDowShaDeg+134)

#define ActAdmShaCrs		(ActDowShaDeg+135)
#define ActReqRemFilShaCrs	(ActDowShaDeg+136)
#define ActRemFilShaCrs		(ActDowShaDeg+137)
#define ActRemFolShaCrs		(ActDowShaDeg+138)
#define ActCopShaCrs		(ActDowShaDeg+139)
#define ActPasShaCrs		(ActDowShaDeg+140)
#define ActRemTreShaCrs		(ActDowShaDeg+141)
#define ActFrmCreShaCrs		(ActDowShaDeg+142)
#define ActCreFolShaCrs		(ActDowShaDeg+143)
#define ActCreLnkShaCrs		(ActDowShaDeg+144)
#define ActRenFolShaCrs		(ActDowShaDeg+145)
#define ActRcvFilShaCrsDZ	(ActDowShaDeg+146)
#define ActRcvFilShaCrsCla	(ActDowShaDeg+147)
#define ActExpShaCrs		(ActDowShaDeg+148)
#define ActConShaCrs		(ActDowShaDeg+149)
#define ActZIPShaCrs		(ActDowShaDeg+150)
#define ActReqDatShaCrs		(ActDowShaDeg+151)
#define ActChgDatShaCrs		(ActDowShaDeg+152)
#define ActDowShaCrs		(ActDowShaDeg+153)

#define ActAdmShaGrp		(ActDowShaDeg+154)
#define ActReqRemFilShaGrp	(ActDowShaDeg+155)
#define ActRemFilShaGrp		(ActDowShaDeg+156)
#define ActRemFolShaGrp		(ActDowShaDeg+157)
#define ActCopShaGrp		(ActDowShaDeg+158)
#define ActPasShaGrp		(ActDowShaDeg+159)
#define ActRemTreShaGrp		(ActDowShaDeg+160)
#define ActFrmCreShaGrp		(ActDowShaDeg+161)
#define ActCreFolShaGrp		(ActDowShaDeg+162)
#define ActCreLnkShaGrp		(ActDowShaDeg+163)
#define ActRenFolShaGrp		(ActDowShaDeg+164)
#define ActRcvFilShaGrpDZ	(ActDowShaDeg+165)
#define ActRcvFilShaGrpCla	(ActDowShaDeg+166)
#define ActExpShaGrp		(ActDowShaDeg+167)
#define ActConShaGrp		(ActDowShaDeg+168)
#define ActZIPShaGrp		(ActDowShaDeg+169)
#define ActReqDatShaGrp		(ActDowShaDeg+170)
#define ActChgDatShaGrp		(ActDowShaDeg+171)
#define ActDowShaGrp		(ActDowShaDeg+172)

#define ActEdiBib		(ActDowShaDeg+173)
#define ActEdiFAQ		(ActDowShaDeg+174)
#define ActEdiCrsLnk		(ActDowShaDeg+175)

#define ActChgFrcReaCrsInf	(ActDowShaDeg+176)
#define ActChgFrcReaTchGui	(ActDowShaDeg+177)
#define ActChgFrcReaSylLec	(ActDowShaDeg+178)
#define ActChgFrcReaSylPra	(ActDowShaDeg+179)
#define ActChgFrcReaBib		(ActDowShaDeg+180)
#define ActChgFrcReaFAQ		(ActDowShaDeg+181)
#define ActChgFrcReaCrsLnk	(ActDowShaDeg+182)

#define ActChgHavReaCrsInf	(ActDowShaDeg+183)
#define ActChgHavReaTchGui	(ActDowShaDeg+184)
#define ActChgHavReaSylLec	(ActDowShaDeg+185)
#define ActChgHavReaSylPra	(ActDowShaDeg+186)
#define ActChgHavReaBib		(ActDowShaDeg+187)
#define ActChgHavReaFAQ		(ActDowShaDeg+188)
#define ActChgHavReaCrsLnk	(ActDowShaDeg+189)

#define ActSelInfSrcCrsInf	(ActDowShaDeg+190)
#define ActSelInfSrcTchGui	(ActDowShaDeg+191)
#define ActSelInfSrcSylLec	(ActDowShaDeg+192)
#define ActSelInfSrcSylPra	(ActDowShaDeg+193)
#define ActSelInfSrcBib		(ActDowShaDeg+194)
#define ActSelInfSrcFAQ		(ActDowShaDeg+195)
#define ActSelInfSrcCrsLnk	(ActDowShaDeg+196)
#define ActRcvURLCrsInf		(ActDowShaDeg+197)
#define ActRcvURLTchGui		(ActDowShaDeg+198)
#define ActRcvURLSylLec		(ActDowShaDeg+199)
#define ActRcvURLSylPra		(ActDowShaDeg+200)
#define ActRcvURLBib		(ActDowShaDeg+201)
#define ActRcvURLFAQ		(ActDowShaDeg+202)
#define ActRcvURLCrsLnk		(ActDowShaDeg+203)
#define ActRcvPagCrsInf		(ActDowShaDeg+204)
#define ActRcvPagTchGui		(ActDowShaDeg+205)
#define ActRcvPagSylLec		(ActDowShaDeg+206)
#define ActRcvPagSylPra		(ActDowShaDeg+207)
#define ActRcvPagBib		(ActDowShaDeg+208)
#define ActRcvPagFAQ		(ActDowShaDeg+209)
#define ActRcvPagCrsLnk		(ActDowShaDeg+210)
#define ActEditorCrsInf		(ActDowShaDeg+211)
#define ActEditorTchGui		(ActDowShaDeg+212)
#define ActEditorSylLec		(ActDowShaDeg+213)
#define ActEditorSylPra		(ActDowShaDeg+214)
#define ActEditorBib		(ActDowShaDeg+215)
#define ActEditorFAQ		(ActDowShaDeg+216)
#define ActEditorCrsLnk		(ActDowShaDeg+217)
#define ActPlaTxtEdiCrsInf	(ActDowShaDeg+218)
#define ActPlaTxtEdiTchGui	(ActDowShaDeg+219)
#define ActPlaTxtEdiSylLec	(ActDowShaDeg+220)
#define ActPlaTxtEdiSylPra	(ActDowShaDeg+221)
#define ActPlaTxtEdiBib		(ActDowShaDeg+222)
#define ActPlaTxtEdiFAQ		(ActDowShaDeg+223)
#define ActPlaTxtEdiCrsLnk	(ActDowShaDeg+224)
#define ActRchTxtEdiCrsInf	(ActDowShaDeg+225)
#define ActRchTxtEdiTchGui	(ActDowShaDeg+226)
#define ActRchTxtEdiSylLec	(ActDowShaDeg+227)
#define ActRchTxtEdiSylPra	(ActDowShaDeg+228)
#define ActRchTxtEdiBib		(ActDowShaDeg+229)
#define ActRchTxtEdiFAQ		(ActDowShaDeg+230)
#define ActRchTxtEdiCrsLnk	(ActDowShaDeg+231)
#define ActRcvPlaTxtCrsInf	(ActDowShaDeg+232)
#define ActRcvPlaTxtTchGui	(ActDowShaDeg+233)
#define ActRcvPlaTxtSylLec	(ActDowShaDeg+234)
#define ActRcvPlaTxtSylPra	(ActDowShaDeg+235)
#define ActRcvPlaTxtBib		(ActDowShaDeg+236)
#define ActRcvPlaTxtFAQ		(ActDowShaDeg+237)
#define ActRcvPlaTxtCrsLnk	(ActDowShaDeg+238)
#define ActRcvRchTxtCrsInf	(ActDowShaDeg+239)
#define ActRcvRchTxtTchGui	(ActDowShaDeg+240)
#define ActRcvRchTxtSylLec	(ActDowShaDeg+241)
#define ActRcvRchTxtSylPra	(ActDowShaDeg+242)
#define ActRcvRchTxtBib		(ActDowShaDeg+243)
#define ActRcvRchTxtFAQ		(ActDowShaDeg+244)
#define ActRcvRchTxtCrsLnk	(ActDowShaDeg+245)

/*****************************************************************************/
/***************************** Assessment tab ********************************/
/*****************************************************************************/
// Actions in menu
#define ActSeeAss		(ActRcvRchTxtCrsLnk+  1)
#define ActSeeAsg		(ActRcvRchTxtCrsLnk+  2)
#define ActAdmAsgWrkUsr		(ActRcvRchTxtCrsLnk+  3)
#define ActReqAsgWrkCrs		(ActRcvRchTxtCrsLnk+  4)
#define ActReqTst		(ActRcvRchTxtCrsLnk+  5)
#define ActSeeCal		(ActRcvRchTxtCrsLnk+  6)
#define ActSeeExaAnn		(ActRcvRchTxtCrsLnk+  7)
#define ActSeeAdmMrk		(ActRcvRchTxtCrsLnk+  8)
// Secondary actions
#define ActEdiAss		(ActRcvRchTxtCrsLnk+  9)
#define ActChgFrcReaAss		(ActRcvRchTxtCrsLnk+ 10)
#define ActChgHavReaAss		(ActRcvRchTxtCrsLnk+ 11)
#define ActSelInfSrcAss		(ActRcvRchTxtCrsLnk+ 12)
#define ActRcvURLAss		(ActRcvRchTxtCrsLnk+ 13)
#define ActRcvPagAss		(ActRcvRchTxtCrsLnk+ 14)
#define ActEditorAss		(ActRcvRchTxtCrsLnk+ 15)
#define ActPlaTxtEdiAss		(ActRcvRchTxtCrsLnk+ 16)
#define ActRchTxtEdiAss		(ActRcvRchTxtCrsLnk+ 17)
#define ActRcvPlaTxtAss		(ActRcvRchTxtCrsLnk+ 18)
#define ActRcvRchTxtAss		(ActRcvRchTxtCrsLnk+ 19)

#define ActFrmNewAsg		(ActRcvRchTxtCrsLnk+ 20)
#define ActEdiOneAsg		(ActRcvRchTxtCrsLnk+ 21)
#define ActNewAsg		(ActRcvRchTxtCrsLnk+ 22)
#define ActChgAsg		(ActRcvRchTxtCrsLnk+ 23)
#define ActReqRemAsg		(ActRcvRchTxtCrsLnk+ 24)
#define ActRemAsg		(ActRcvRchTxtCrsLnk+ 25)
#define ActHidAsg		(ActRcvRchTxtCrsLnk+ 26)
#define ActShoAsg		(ActRcvRchTxtCrsLnk+ 27)

#define ActAdmAsgWrkCrs		(ActRcvRchTxtCrsLnk+ 28)

#define ActReqRemFilAsgUsr	(ActRcvRchTxtCrsLnk+ 29)
#define ActRemFilAsgUsr		(ActRcvRchTxtCrsLnk+ 30)
#define ActRemFolAsgUsr		(ActRcvRchTxtCrsLnk+ 31)
#define ActCopAsgUsr		(ActRcvRchTxtCrsLnk+ 32)
#define ActPasAsgUsr		(ActRcvRchTxtCrsLnk+ 33)
#define ActRemTreAsgUsr		(ActRcvRchTxtCrsLnk+ 34)
#define ActFrmCreAsgUsr		(ActRcvRchTxtCrsLnk+ 35)
#define ActCreFolAsgUsr		(ActRcvRchTxtCrsLnk+ 36)
#define ActCreLnkAsgUsr		(ActRcvRchTxtCrsLnk+ 37)
#define ActRenFolAsgUsr		(ActRcvRchTxtCrsLnk+ 38)
#define ActRcvFilAsgUsrDZ	(ActRcvRchTxtCrsLnk+ 39)
#define ActRcvFilAsgUsrCla	(ActRcvRchTxtCrsLnk+ 40)
#define ActExpAsgUsr		(ActRcvRchTxtCrsLnk+ 41)
#define ActConAsgUsr		(ActRcvRchTxtCrsLnk+ 42)
#define ActZIPAsgUsr		(ActRcvRchTxtCrsLnk+ 43)
#define ActReqDatAsgUsr		(ActRcvRchTxtCrsLnk+ 44)
#define ActChgDatAsgUsr		(ActRcvRchTxtCrsLnk+ 45)
#define ActDowAsgUsr		(ActRcvRchTxtCrsLnk+ 46)

#define ActReqRemFilWrkUsr	(ActRcvRchTxtCrsLnk+ 47)
#define ActRemFilWrkUsr		(ActRcvRchTxtCrsLnk+ 48)
#define ActRemFolWrkUsr		(ActRcvRchTxtCrsLnk+ 49)
#define ActCopWrkUsr		(ActRcvRchTxtCrsLnk+ 50)
#define ActPasWrkUsr		(ActRcvRchTxtCrsLnk+ 51)
#define ActRemTreWrkUsr		(ActRcvRchTxtCrsLnk+ 52)
#define ActFrmCreWrkUsr		(ActRcvRchTxtCrsLnk+ 53)
#define ActCreFolWrkUsr		(ActRcvRchTxtCrsLnk+ 54)
#define ActCreLnkWrkUsr		(ActRcvRchTxtCrsLnk+ 55)
#define ActRenFolWrkUsr		(ActRcvRchTxtCrsLnk+ 56)
#define ActRcvFilWrkUsrDZ	(ActRcvRchTxtCrsLnk+ 57)
#define ActRcvFilWrkUsrCla	(ActRcvRchTxtCrsLnk+ 58)
#define ActExpWrkUsr		(ActRcvRchTxtCrsLnk+ 59)
#define ActConWrkUsr		(ActRcvRchTxtCrsLnk+ 60)
#define ActZIPWrkUsr		(ActRcvRchTxtCrsLnk+ 61)
#define ActReqDatWrkUsr		(ActRcvRchTxtCrsLnk+ 62)
#define ActChgDatWrkUsr		(ActRcvRchTxtCrsLnk+ 63)
#define ActDowWrkUsr		(ActRcvRchTxtCrsLnk+ 64)

#define ActReqRemFilAsgCrs	(ActRcvRchTxtCrsLnk+ 65)
#define ActRemFilAsgCrs		(ActRcvRchTxtCrsLnk+ 66)
#define ActRemFolAsgCrs		(ActRcvRchTxtCrsLnk+ 67)
#define ActCopAsgCrs		(ActRcvRchTxtCrsLnk+ 68)
#define ActPasAsgCrs		(ActRcvRchTxtCrsLnk+ 69)
#define ActRemTreAsgCrs		(ActRcvRchTxtCrsLnk+ 70)
#define ActFrmCreAsgCrs		(ActRcvRchTxtCrsLnk+ 71)
#define ActCreFolAsgCrs		(ActRcvRchTxtCrsLnk+ 72)
#define ActCreLnkAsgCrs		(ActRcvRchTxtCrsLnk+ 73)
#define ActRenFolAsgCrs		(ActRcvRchTxtCrsLnk+ 74)
#define ActRcvFilAsgCrsDZ	(ActRcvRchTxtCrsLnk+ 75)
#define ActRcvFilAsgCrsCla	(ActRcvRchTxtCrsLnk+ 76)
#define ActExpAsgCrs		(ActRcvRchTxtCrsLnk+ 77)
#define ActConAsgCrs		(ActRcvRchTxtCrsLnk+ 78)
#define ActZIPAsgCrs		(ActRcvRchTxtCrsLnk+ 79)
#define ActReqDatAsgCrs		(ActRcvRchTxtCrsLnk+ 80)
#define ActChgDatAsgCrs		(ActRcvRchTxtCrsLnk+ 81)
#define ActDowAsgCrs		(ActRcvRchTxtCrsLnk+ 82)

#define ActReqRemFilWrkCrs	(ActRcvRchTxtCrsLnk+ 83)
#define ActRemFilWrkCrs		(ActRcvRchTxtCrsLnk+ 84)
#define ActRemFolWrkCrs		(ActRcvRchTxtCrsLnk+ 85)
#define ActCopWrkCrs		(ActRcvRchTxtCrsLnk+ 86)
#define ActPasWrkCrs		(ActRcvRchTxtCrsLnk+ 87)
#define ActRemTreWrkCrs		(ActRcvRchTxtCrsLnk+ 88)
#define ActFrmCreWrkCrs		(ActRcvRchTxtCrsLnk+ 89)
#define ActCreFolWrkCrs		(ActRcvRchTxtCrsLnk+ 90)
#define ActCreLnkWrkCrs		(ActRcvRchTxtCrsLnk+ 91)
#define ActRenFolWrkCrs		(ActRcvRchTxtCrsLnk+ 92)
#define ActRcvFilWrkCrsDZ	(ActRcvRchTxtCrsLnk+ 93)
#define ActRcvFilWrkCrsCla	(ActRcvRchTxtCrsLnk+ 94)
#define ActExpWrkCrs		(ActRcvRchTxtCrsLnk+ 95)
#define ActConWrkCrs		(ActRcvRchTxtCrsLnk+ 96)
#define ActZIPWrkCrs		(ActRcvRchTxtCrsLnk+ 97)
#define ActReqDatWrkCrs		(ActRcvRchTxtCrsLnk+ 98)
#define ActChgDatWrkCrs		(ActRcvRchTxtCrsLnk+ 99)
#define ActDowWrkCrs		(ActRcvRchTxtCrsLnk+100)

#define ActSeeTst		(ActRcvRchTxtCrsLnk+101)
#define ActAssTst		(ActRcvRchTxtCrsLnk+102)
#define ActEdiTstQst		(ActRcvRchTxtCrsLnk+103)
#define ActEdiOneTstQst		(ActRcvRchTxtCrsLnk+104)
#define ActReqImpTstQst		(ActRcvRchTxtCrsLnk+105)
#define ActImpTstQst		(ActRcvRchTxtCrsLnk+106)
#define ActLstTstQst		(ActRcvRchTxtCrsLnk+107)
#define ActRcvTstQst		(ActRcvRchTxtCrsLnk+108)
#define ActReqRemTstQst		(ActRcvRchTxtCrsLnk+109)
#define ActRemTstQst		(ActRcvRchTxtCrsLnk+110)
#define ActShfTstQst		(ActRcvRchTxtCrsLnk+111)
#define ActCfgTst		(ActRcvRchTxtCrsLnk+112)
#define ActEnableTag		(ActRcvRchTxtCrsLnk+113)
#define ActDisableTag		(ActRcvRchTxtCrsLnk+114)
#define ActRenTag		(ActRcvRchTxtCrsLnk+115)
#define ActRcvCfgTst		(ActRcvRchTxtCrsLnk+116)

#define ActReqSeeMyTstExa	(ActRcvRchTxtCrsLnk+117)
#define ActSeeMyTstExa		(ActRcvRchTxtCrsLnk+118)
#define ActSeeOneTstExaMe	(ActRcvRchTxtCrsLnk+119)
#define ActReqSeeUsrTstExa	(ActRcvRchTxtCrsLnk+120)
#define ActSeeUsrTstExa		(ActRcvRchTxtCrsLnk+121)
#define ActSeeOneTstExaOth	(ActRcvRchTxtCrsLnk+122)

#define ActPrnCal		(ActRcvRchTxtCrsLnk+123)
#define ActChgCal1stDay		(ActRcvRchTxtCrsLnk+124)

#define ActEdiExaAnn		(ActRcvRchTxtCrsLnk+125)
#define ActRcvExaAnn		(ActRcvRchTxtCrsLnk+126)
#define ActPrnExaAnn		(ActRcvRchTxtCrsLnk+127)
#define ActRemExaAnn		(ActRcvRchTxtCrsLnk+128)

#define ActChgToSeeMrk		(ActRcvRchTxtCrsLnk+129)

#define ActSeeMrkCrs		(ActRcvRchTxtCrsLnk+130)
#define ActExpSeeMrkCrs		(ActRcvRchTxtCrsLnk+131)
#define ActConSeeMrkCrs		(ActRcvRchTxtCrsLnk+132)
#define ActReqDatSeeMrkCrs	(ActRcvRchTxtCrsLnk+133)
#define ActSeeMyMrkCrs		(ActRcvRchTxtCrsLnk+134)

#define ActSeeMrkGrp		(ActRcvRchTxtCrsLnk+135)
#define ActExpSeeMrkGrp		(ActRcvRchTxtCrsLnk+136)
#define ActConSeeMrkGrp		(ActRcvRchTxtCrsLnk+137)
#define ActReqDatSeeMrkGrp	(ActRcvRchTxtCrsLnk+138)
#define ActSeeMyMrkGrp		(ActRcvRchTxtCrsLnk+139)

#define ActChgToAdmMrk		(ActRcvRchTxtCrsLnk+140)

#define ActAdmMrkCrs		(ActRcvRchTxtCrsLnk+141)
#define ActReqRemFilMrkCrs	(ActRcvRchTxtCrsLnk+142)
#define ActRemFilMrkCrs		(ActRcvRchTxtCrsLnk+143)
#define ActRemFolMrkCrs		(ActRcvRchTxtCrsLnk+144)
#define ActCopMrkCrs		(ActRcvRchTxtCrsLnk+145)
#define ActPasMrkCrs		(ActRcvRchTxtCrsLnk+146)
#define ActRemTreMrkCrs		(ActRcvRchTxtCrsLnk+147)
#define ActFrmCreMrkCrs		(ActRcvRchTxtCrsLnk+148)
#define ActCreFolMrkCrs		(ActRcvRchTxtCrsLnk+149)
#define ActRenFolMrkCrs		(ActRcvRchTxtCrsLnk+150)
#define ActRcvFilMrkCrsDZ	(ActRcvRchTxtCrsLnk+151)
#define ActRcvFilMrkCrsCla	(ActRcvRchTxtCrsLnk+152)
#define ActExpAdmMrkCrs		(ActRcvRchTxtCrsLnk+153)
#define ActConAdmMrkCrs		(ActRcvRchTxtCrsLnk+154)
#define ActZIPAdmMrkCrs		(ActRcvRchTxtCrsLnk+155)
#define ActShoMrkCrs		(ActRcvRchTxtCrsLnk+156)
#define ActHidMrkCrs		(ActRcvRchTxtCrsLnk+157)
#define ActReqDatAdmMrkCrs	(ActRcvRchTxtCrsLnk+158)
#define ActChgDatAdmMrkCrs	(ActRcvRchTxtCrsLnk+159)
#define ActDowAdmMrkCrs		(ActRcvRchTxtCrsLnk+160)
#define ActChgNumRowHeaCrs	(ActRcvRchTxtCrsLnk+161)
#define ActChgNumRowFooCrs	(ActRcvRchTxtCrsLnk+162)

#define ActAdmMrkGrp		(ActRcvRchTxtCrsLnk+163)
#define ActReqRemFilMrkGrp	(ActRcvRchTxtCrsLnk+164)
#define ActRemFilMrkGrp		(ActRcvRchTxtCrsLnk+165)
#define ActRemFolMrkGrp		(ActRcvRchTxtCrsLnk+166)
#define ActCopMrkGrp		(ActRcvRchTxtCrsLnk+167)
#define ActPasMrkGrp		(ActRcvRchTxtCrsLnk+168)
#define ActRemTreMrkGrp		(ActRcvRchTxtCrsLnk+169)
#define ActFrmCreMrkGrp		(ActRcvRchTxtCrsLnk+170)
#define ActCreFolMrkGrp		(ActRcvRchTxtCrsLnk+171)
#define ActRenFolMrkGrp		(ActRcvRchTxtCrsLnk+172)
#define ActRcvFilMrkGrpDZ	(ActRcvRchTxtCrsLnk+173)
#define ActRcvFilMrkGrpCla	(ActRcvRchTxtCrsLnk+174)
#define ActExpAdmMrkGrp		(ActRcvRchTxtCrsLnk+175)
#define ActConAdmMrkGrp		(ActRcvRchTxtCrsLnk+176)
#define ActZIPAdmMrkGrp		(ActRcvRchTxtCrsLnk+177)
#define ActShoMrkGrp		(ActRcvRchTxtCrsLnk+178)
#define ActHidMrkGrp		(ActRcvRchTxtCrsLnk+179)
#define ActReqDatAdmMrkGrp	(ActRcvRchTxtCrsLnk+180)
#define ActChgDatAdmMrkGrp	(ActRcvRchTxtCrsLnk+181)
#define ActDowAdmMrkGrp		(ActRcvRchTxtCrsLnk+182)
#define ActChgNumRowHeaGrp	(ActRcvRchTxtCrsLnk+183)
#define ActChgNumRowFooGrp	(ActRcvRchTxtCrsLnk+184)

/*****************************************************************************/
/******************************* Users tab ***********************************/
/*****************************************************************************/
// Actions in menu
#define ActReqSelGrp		(ActChgNumRowFooGrp+  1)
#define ActLstStd		(ActChgNumRowFooGrp+  2)
#define ActLstTch		(ActChgNumRowFooGrp+  3)
#define ActLstOth		(ActChgNumRowFooGrp+  4)
#define ActSeeAtt		(ActChgNumRowFooGrp+  5)
#define ActReqSignUp		(ActChgNumRowFooGrp+  6)
#define ActSeeSignUpReq		(ActChgNumRowFooGrp+  7)
#define ActLstCon		(ActChgNumRowFooGrp+  8)

// Secondary actions
#define ActChgGrp		(ActChgNumRowFooGrp+  9)
#define ActReqEdiGrp		(ActChgNumRowFooGrp+ 10)
#define ActNewGrpTyp		(ActChgNumRowFooGrp+ 11)
#define ActReqRemGrpTyp		(ActChgNumRowFooGrp+ 12)
#define ActRemGrpTyp		(ActChgNumRowFooGrp+ 13)
#define ActRenGrpTyp		(ActChgNumRowFooGrp+ 14)
#define ActChgMdtGrpTyp		(ActChgNumRowFooGrp+ 15)
#define ActChgMulGrpTyp		(ActChgNumRowFooGrp+ 16)
#define ActChgTimGrpTyp		(ActChgNumRowFooGrp+ 17)
#define ActNewGrp		(ActChgNumRowFooGrp+ 18)
#define ActReqRemGrp		(ActChgNumRowFooGrp+ 19)
#define ActRemGrp		(ActChgNumRowFooGrp+ 20)
#define ActOpeGrp		(ActChgNumRowFooGrp+ 21)
#define ActCloGrp		(ActChgNumRowFooGrp+ 22)
#define ActEnaFilZonGrp		(ActChgNumRowFooGrp+ 23)
#define ActDisFilZonGrp		(ActChgNumRowFooGrp+ 24)
#define ActChgGrpTyp		(ActChgNumRowFooGrp+ 25)
#define ActRenGrp		(ActChgNumRowFooGrp+ 26)
#define ActChgMaxStdGrp		(ActChgNumRowFooGrp+ 27)

#define ActGetExtLstStd		(ActChgNumRowFooGrp+ 28)

#define ActLstGst		(ActChgNumRowFooGrp+ 29)

#define ActPrnGstPho		(ActChgNumRowFooGrp+ 30)
#define ActPrnStdPho		(ActChgNumRowFooGrp+ 31)
#define ActPrnTchPho		(ActChgNumRowFooGrp+ 32)
#define ActLstGstAll		(ActChgNumRowFooGrp+ 33)
#define ActLstStdAll		(ActChgNumRowFooGrp+ 34)
#define ActLstTchAll		(ActChgNumRowFooGrp+ 35)

#define ActSeeRecOneStd		(ActChgNumRowFooGrp+ 36)
#define ActSeeRecOneTch		(ActChgNumRowFooGrp+ 37)
#define ActSeeRecSevGst		(ActChgNumRowFooGrp+ 38)
#define ActSeeRecSevStd		(ActChgNumRowFooGrp+ 39)
#define ActSeeRecSevTch		(ActChgNumRowFooGrp+ 40)
#define ActPrnRecSevGst		(ActChgNumRowFooGrp+ 41)
#define ActPrnRecSevStd		(ActChgNumRowFooGrp+ 42)
#define ActPrnRecSevTch		(ActChgNumRowFooGrp+ 43)
#define ActRcvRecOthUsr		(ActChgNumRowFooGrp+ 44)
#define ActEdiRecFie		(ActChgNumRowFooGrp+ 45)
#define ActNewFie		(ActChgNumRowFooGrp+ 46)
#define ActReqRemFie		(ActChgNumRowFooGrp+ 47)
#define ActRemFie		(ActChgNumRowFooGrp+ 48)
#define ActRenFie		(ActChgNumRowFooGrp+ 49)
#define ActChgRowFie		(ActChgNumRowFooGrp+ 50)
#define ActChgVisFie		(ActChgNumRowFooGrp+ 51)
#define ActRcvRecCrs		(ActChgNumRowFooGrp+ 52)

#define ActReqEnrSevStd		(ActChgNumRowFooGrp+ 53)
#define ActReqEnrSevTch		(ActChgNumRowFooGrp+ 54)

#define ActReqLstStdAtt		(ActChgNumRowFooGrp+ 55)
#define ActSeeLstMyAtt		(ActChgNumRowFooGrp+ 56)
#define ActPrnLstMyAtt		(ActChgNumRowFooGrp+ 57)
#define ActSeeLstStdAtt		(ActChgNumRowFooGrp+ 58)
#define ActPrnLstStdAtt		(ActChgNumRowFooGrp+ 59)
#define ActFrmNewAtt		(ActChgNumRowFooGrp+ 60)
#define ActEdiOneAtt		(ActChgNumRowFooGrp+ 61)
#define ActNewAtt		(ActChgNumRowFooGrp+ 62)
#define ActChgAtt		(ActChgNumRowFooGrp+ 63)
#define ActReqRemAtt		(ActChgNumRowFooGrp+ 64)
#define ActRemAtt		(ActChgNumRowFooGrp+ 65)
#define ActHidAtt		(ActChgNumRowFooGrp+ 66)
#define ActShoAtt		(ActChgNumRowFooGrp+ 67)
#define ActSeeOneAtt		(ActChgNumRowFooGrp+ 68)
#define ActRecAttStd		(ActChgNumRowFooGrp+ 69)
#define ActRecAttMe		(ActChgNumRowFooGrp+ 70)

#define ActSignUp		(ActChgNumRowFooGrp+ 71)
#define ActUpdSignUpReq		(ActChgNumRowFooGrp+ 72)
#define ActReqRejSignUp		(ActChgNumRowFooGrp+ 73)
#define ActRejSignUp		(ActChgNumRowFooGrp+ 74)

#define ActReqMdfOneOth		(ActChgNumRowFooGrp+ 75)
#define ActReqMdfOneStd		(ActChgNumRowFooGrp+ 76)
#define ActReqMdfOneTch		(ActChgNumRowFooGrp+ 77)

#define ActReqMdfOth		(ActChgNumRowFooGrp+ 78)
#define ActReqMdfStd		(ActChgNumRowFooGrp+ 79)
#define ActReqMdfTch		(ActChgNumRowFooGrp+ 80)

#define ActReqOthPho		(ActChgNumRowFooGrp+ 81)
#define ActReqStdPho		(ActChgNumRowFooGrp+ 82)
#define ActReqTchPho		(ActChgNumRowFooGrp+ 83)
#define ActDetOthPho		(ActChgNumRowFooGrp+ 84)
#define ActDetStdPho		(ActChgNumRowFooGrp+ 85)
#define ActDetTchPho		(ActChgNumRowFooGrp+ 86)
#define ActUpdOthPho		(ActChgNumRowFooGrp+ 87)
#define ActUpdStdPho		(ActChgNumRowFooGrp+ 88)
#define ActUpdTchPho		(ActChgNumRowFooGrp+ 89)
#define ActRemOthPho		(ActChgNumRowFooGrp+ 90)
#define ActRemStdPho		(ActChgNumRowFooGrp+ 91)
#define ActRemTchPho		(ActChgNumRowFooGrp+ 92)
#define ActCreOth		(ActChgNumRowFooGrp+ 93)
#define ActCreStd		(ActChgNumRowFooGrp+ 94)
#define ActCreTch		(ActChgNumRowFooGrp+ 95)
#define ActUpdOth		(ActChgNumRowFooGrp+ 96)
#define ActUpdStd		(ActChgNumRowFooGrp+ 97)
#define ActUpdTch		(ActChgNumRowFooGrp+ 98)

#define ActReqAccEnrStd		(ActChgNumRowFooGrp+ 99)
#define ActReqAccEnrTch		(ActChgNumRowFooGrp+100)
#define ActAccEnrStd		(ActChgNumRowFooGrp+101)
#define ActAccEnrTch		(ActChgNumRowFooGrp+102)
#define ActRemMe_Std		(ActChgNumRowFooGrp+103)
#define ActRemMe_Tch		(ActChgNumRowFooGrp+104)

#define ActNewAdmIns		(ActChgNumRowFooGrp+105)
#define ActRemAdmIns		(ActChgNumRowFooGrp+106)
#define ActNewAdmCtr		(ActChgNumRowFooGrp+107)
#define ActRemAdmCtr		(ActChgNumRowFooGrp+108)
#define ActNewAdmDeg		(ActChgNumRowFooGrp+109)
#define ActRemAdmDeg		(ActChgNumRowFooGrp+110)

#define ActRcvFrmEnrSevStd	(ActChgNumRowFooGrp+111)
#define ActRcvFrmEnrSevTch	(ActChgNumRowFooGrp+112)

#define ActFrmIDsOth		(ActChgNumRowFooGrp+113)
#define ActFrmIDsStd		(ActChgNumRowFooGrp+114)
#define ActFrmIDsTch		(ActChgNumRowFooGrp+115)
#define ActRemID_Oth		(ActChgNumRowFooGrp+116)
#define ActRemID_Std		(ActChgNumRowFooGrp+117)
#define ActRemID_Tch		(ActChgNumRowFooGrp+118)
#define ActNewID_Oth		(ActChgNumRowFooGrp+119)
#define ActNewID_Std		(ActChgNumRowFooGrp+120)
#define ActNewID_Tch		(ActChgNumRowFooGrp+121)
#define ActFrmPwdOth		(ActChgNumRowFooGrp+122)
#define ActFrmPwdStd		(ActChgNumRowFooGrp+123)
#define ActFrmPwdTch		(ActChgNumRowFooGrp+124)
#define ActChgPwdOth		(ActChgNumRowFooGrp+125)
#define ActChgPwdStd		(ActChgNumRowFooGrp+126)
#define ActChgPwdTch		(ActChgNumRowFooGrp+127)
#define ActFrmMaiOth		(ActChgNumRowFooGrp+128)
#define ActFrmMaiStd		(ActChgNumRowFooGrp+129)
#define ActFrmMaiTch		(ActChgNumRowFooGrp+130)
#define ActRemMaiOth		(ActChgNumRowFooGrp+131)
#define ActRemMaiStd		(ActChgNumRowFooGrp+132)
#define ActRemMaiTch		(ActChgNumRowFooGrp+133)
#define ActNewMaiOth		(ActChgNumRowFooGrp+134)
#define ActNewMaiStd		(ActChgNumRowFooGrp+135)
#define ActNewMaiTch		(ActChgNumRowFooGrp+136)

#define ActRemStdCrs		(ActChgNumRowFooGrp+137)
#define ActRemTchCrs		(ActChgNumRowFooGrp+138)
#define ActRemUsrGbl		(ActChgNumRowFooGrp+139)

#define ActReqRemAllStdCrs	(ActChgNumRowFooGrp+140)
#define ActRemAllStdCrs		(ActChgNumRowFooGrp+141)

#define ActReqRemOldUsr		(ActChgNumRowFooGrp+142)
#define ActRemOldUsr		(ActChgNumRowFooGrp+143)

#define ActLstClk		(ActChgNumRowFooGrp+144)

/*****************************************************************************/
/******************************** Social tab *********************************/
/*****************************************************************************/
// Actions in menu
#define ActSeeSocTmlGbl		(ActLstClk+  1)
#define ActSeeSocPrf		(ActLstClk+  2)
#define ActSeeFor		(ActLstClk+  3)
#define ActSeeChtRms		(ActLstClk+  4)
// Secondary actions
#define ActRcvSocPstGbl		(ActLstClk+  5)
#define ActRcvSocComGbl		(ActLstClk+  6)
#define ActShaSocNotGbl		(ActLstClk+  7)
#define ActUnsSocNotGbl		(ActLstClk+  8)
#define ActFavSocNotGbl		(ActLstClk+  9)
#define ActUnfSocNotGbl		(ActLstClk+ 10)
#define ActFavSocComGbl		(ActLstClk+ 11)
#define ActUnfSocComGbl		(ActLstClk+ 12)
#define ActReqRemSocPubGbl	(ActLstClk+ 13)
#define ActRemSocPubGbl		(ActLstClk+ 14)
#define ActReqRemSocComGbl	(ActLstClk+ 15)
#define ActRemSocComGbl		(ActLstClk+ 16)

#define ActReqPubPrf		(ActLstClk+ 17)

#define ActRcvSocPstUsr		(ActLstClk+ 18)
#define ActRcvSocComUsr		(ActLstClk+ 19)
#define ActShaSocNotUsr		(ActLstClk+ 20)
#define ActUnsSocNotUsr		(ActLstClk+ 21)
#define ActFavSocNotUsr		(ActLstClk+ 22)
#define ActUnfSocNotUsr		(ActLstClk+ 23)
#define ActFavSocComUsr		(ActLstClk+ 24)
#define ActUnfSocComUsr		(ActLstClk+ 25)
#define ActReqRemSocPubUsr	(ActLstClk+ 26)
#define ActRemSocPubUsr		(ActLstClk+ 27)
#define ActReqRemSocComUsr	(ActLstClk+ 28)
#define ActRemSocComUsr		(ActLstClk+ 29)

#define ActSeePubPrf		(ActLstClk+ 30)
#define ActCal1stClkTim		(ActLstClk+ 31)
#define ActCalNumClk		(ActLstClk+ 32)
#define ActCalNumFilVie		(ActLstClk+ 33)
#define ActCalNumForPst		(ActLstClk+ 34)
#define ActCalNumMsgSnt		(ActLstClk+ 35)

#define ActFolUsr		(ActLstClk+ 36)
#define ActUnfUsr		(ActLstClk+ 37)
#define ActSeeFlg		(ActLstClk+ 38)
#define ActSeeFlr		(ActLstClk+ 39)

#define ActSeeForCrsUsr		(ActLstClk+ 40)
#define ActSeeForCrsTch		(ActLstClk+ 41)
#define ActSeeForDegUsr		(ActLstClk+ 42)
#define ActSeeForDegTch		(ActLstClk+ 43)
#define ActSeeForCtrUsr		(ActLstClk+ 44)
#define ActSeeForCtrTch		(ActLstClk+ 45)
#define ActSeeForInsUsr		(ActLstClk+ 46)
#define ActSeeForInsTch		(ActLstClk+ 47)
#define ActSeeForGenUsr		(ActLstClk+ 48)
#define ActSeeForGenTch		(ActLstClk+ 49)
#define ActSeeForSWAUsr		(ActLstClk+ 50)
#define ActSeeForSWATch		(ActLstClk+ 51)
#define ActSeePstForCrsUsr	(ActLstClk+ 52)
#define ActSeePstForCrsTch	(ActLstClk+ 53)
#define ActSeePstForDegUsr	(ActLstClk+ 54)
#define ActSeePstForDegTch	(ActLstClk+ 55)
#define ActSeePstForCtrUsr	(ActLstClk+ 56)
#define ActSeePstForCtrTch	(ActLstClk+ 57)
#define ActSeePstForInsUsr	(ActLstClk+ 58)
#define ActSeePstForInsTch	(ActLstClk+ 59)
#define ActSeePstForGenUsr	(ActLstClk+ 60)
#define ActSeePstForGenTch	(ActLstClk+ 61)
#define ActSeePstForSWAUsr	(ActLstClk+ 62)
#define ActSeePstForSWATch	(ActLstClk+ 63)
#define ActRcvThrForCrsUsr	(ActLstClk+ 64)
#define ActRcvThrForCrsTch	(ActLstClk+ 65)
#define ActRcvThrForDegUsr	(ActLstClk+ 66)
#define ActRcvThrForDegTch	(ActLstClk+ 67)
#define ActRcvThrForCtrUsr	(ActLstClk+ 68)
#define ActRcvThrForCtrTch	(ActLstClk+ 69)
#define ActRcvThrForInsUsr	(ActLstClk+ 70)
#define ActRcvThrForInsTch	(ActLstClk+ 71)
#define ActRcvThrForGenUsr	(ActLstClk+ 72)
#define ActRcvThrForGenTch	(ActLstClk+ 73)
#define ActRcvThrForSWAUsr	(ActLstClk+ 74)
#define ActRcvThrForSWATch	(ActLstClk+ 75)
#define ActRcvRepForCrsUsr	(ActLstClk+ 76)
#define ActRcvRepForCrsTch	(ActLstClk+ 77)
#define ActRcvRepForDegUsr	(ActLstClk+ 78)
#define ActRcvRepForDegTch	(ActLstClk+ 79)
#define ActRcvRepForCtrUsr	(ActLstClk+ 80)
#define ActRcvRepForCtrTch	(ActLstClk+ 81)
#define ActRcvRepForInsUsr	(ActLstClk+ 82)
#define ActRcvRepForInsTch	(ActLstClk+ 83)
#define ActRcvRepForGenUsr	(ActLstClk+ 84)
#define ActRcvRepForGenTch	(ActLstClk+ 85)
#define ActRcvRepForSWAUsr	(ActLstClk+ 86)
#define ActRcvRepForSWATch	(ActLstClk+ 87)
#define ActReqDelThrCrsUsr	(ActLstClk+ 88)
#define ActReqDelThrCrsTch	(ActLstClk+ 89)
#define ActReqDelThrDegUsr	(ActLstClk+ 90)
#define ActReqDelThrDegTch	(ActLstClk+ 91)
#define ActReqDelThrCtrUsr	(ActLstClk+ 92)
#define ActReqDelThrCtrTch	(ActLstClk+ 93)
#define ActReqDelThrInsUsr	(ActLstClk+ 94)
#define ActReqDelThrInsTch	(ActLstClk+ 95)
#define ActReqDelThrGenUsr	(ActLstClk+ 96)
#define ActReqDelThrGenTch	(ActLstClk+ 97)
#define ActReqDelThrSWAUsr	(ActLstClk+ 98)
#define ActReqDelThrSWATch	(ActLstClk+ 99)
#define ActDelThrForCrsUsr	(ActLstClk+100)
#define ActDelThrForCrsTch	(ActLstClk+101)
#define ActDelThrForDegUsr	(ActLstClk+102)
#define ActDelThrForDegTch	(ActLstClk+103)
#define ActDelThrForCtrUsr	(ActLstClk+104)
#define ActDelThrForCtrTch	(ActLstClk+105)
#define ActDelThrForInsUsr	(ActLstClk+106)
#define ActDelThrForInsTch	(ActLstClk+107)
#define ActDelThrForGenUsr	(ActLstClk+108)
#define ActDelThrForGenTch	(ActLstClk+109)
#define ActDelThrForSWAUsr	(ActLstClk+110)
#define ActDelThrForSWATch	(ActLstClk+111)
#define ActCutThrForCrsUsr	(ActLstClk+112)
#define ActCutThrForCrsTch	(ActLstClk+113)
#define ActCutThrForDegUsr	(ActLstClk+114)
#define ActCutThrForDegTch	(ActLstClk+115)
#define ActCutThrForCtrUsr	(ActLstClk+116)
#define ActCutThrForCtrTch	(ActLstClk+117)
#define ActCutThrForInsUsr	(ActLstClk+118)
#define ActCutThrForInsTch	(ActLstClk+119)
#define ActCutThrForGenUsr	(ActLstClk+120)
#define ActCutThrForGenTch	(ActLstClk+121)
#define ActCutThrForSWAUsr	(ActLstClk+122)
#define ActCutThrForSWATch	(ActLstClk+123)
#define ActPasThrForCrsUsr	(ActLstClk+124)
#define ActPasThrForCrsTch	(ActLstClk+125)
#define ActPasThrForDegUsr	(ActLstClk+126)
#define ActPasThrForDegTch	(ActLstClk+127)
#define ActPasThrForCtrUsr	(ActLstClk+128)
#define ActPasThrForCtrTch	(ActLstClk+129)
#define ActPasThrForInsUsr	(ActLstClk+130)
#define ActPasThrForInsTch	(ActLstClk+131)
#define ActPasThrForGenUsr	(ActLstClk+132)
#define ActPasThrForGenTch	(ActLstClk+133)
#define ActPasThrForSWAUsr	(ActLstClk+134)
#define ActPasThrForSWATch	(ActLstClk+135)
#define ActDelPstForCrsUsr	(ActLstClk+136)
#define ActDelPstForCrsTch	(ActLstClk+137)
#define ActDelPstForDegUsr	(ActLstClk+138)
#define ActDelPstForDegTch	(ActLstClk+139)
#define ActDelPstForCtrUsr	(ActLstClk+140)
#define ActDelPstForCtrTch	(ActLstClk+141)
#define ActDelPstForInsUsr	(ActLstClk+142)
#define ActDelPstForInsTch	(ActLstClk+143)
#define ActDelPstForGenUsr	(ActLstClk+144)
#define ActDelPstForGenTch	(ActLstClk+145)
#define ActDelPstForSWAUsr	(ActLstClk+146)
#define ActDelPstForSWATch	(ActLstClk+147)
#define ActEnbPstForCrsUsr	(ActLstClk+148)
#define ActEnbPstForCrsTch	(ActLstClk+149)
#define ActEnbPstForDegUsr	(ActLstClk+150)
#define ActEnbPstForDegTch	(ActLstClk+151)
#define ActEnbPstForCtrUsr	(ActLstClk+152)
#define ActEnbPstForCtrTch	(ActLstClk+153)
#define ActEnbPstForInsUsr	(ActLstClk+154)
#define ActEnbPstForInsTch	(ActLstClk+155)
#define ActEnbPstForGenUsr	(ActLstClk+156)
#define ActEnbPstForGenTch	(ActLstClk+157)
#define ActEnbPstForSWAUsr	(ActLstClk+158)
#define ActEnbPstForSWATch	(ActLstClk+159)
#define ActDisPstForCrsUsr	(ActLstClk+160)
#define ActDisPstForCrsTch	(ActLstClk+161)
#define ActDisPstForDegUsr	(ActLstClk+162)
#define ActDisPstForDegTch	(ActLstClk+163)
#define ActDisPstForCtrUsr	(ActLstClk+164)
#define ActDisPstForCtrTch	(ActLstClk+165)
#define ActDisPstForInsUsr	(ActLstClk+166)
#define ActDisPstForInsTch	(ActLstClk+167)
#define ActDisPstForGenUsr	(ActLstClk+168)
#define ActDisPstForGenTch	(ActLstClk+169)
#define ActDisPstForSWAUsr	(ActLstClk+170)
#define ActDisPstForSWATch	(ActLstClk+171)

#define ActCht			(ActLstClk+172)

/*****************************************************************************/
/******************************* Messages tab ********************************/
/*****************************************************************************/
// Actions in menu
#define ActSeeNtf		(ActCht+  1)
#define ActSeeAnn		(ActCht+  2)
#define ActSeeNot		(ActCht+  3)
#define ActReqMsgUsr		(ActCht+  4)
#define ActSeeRcvMsg		(ActCht+  5)
#define ActSeeSntMsg		(ActCht+  6)
#define ActMaiStd		(ActCht+  7)
// Secondary actions
#define ActWriAnn		(ActCht+  8)
#define ActRcvAnn		(ActCht+  9)
#define ActHidAnn		(ActCht+ 10)
#define ActRevAnn		(ActCht+ 11)
#define ActRemAnn		(ActCht+ 12)
#define ActShoNot		(ActCht+ 13)
#define ActWriNot		(ActCht+ 14)
#define ActRcvNot		(ActCht+ 15)
#define ActHidNot		(ActCht+ 16)
#define ActRevNot		(ActCht+ 17)
#define ActReqRemNot		(ActCht+ 18)
#define ActRemNot		(ActCht+ 19)
#define ActSeeNewNtf		(ActCht+ 20)
#define ActMrkNtfSee		(ActCht+ 21)
#define ActRcvMsgUsr		(ActCht+ 22)
#define ActReqDelAllSntMsg	(ActCht+ 23)
#define ActReqDelAllRcvMsg	(ActCht+ 24)
#define ActDelAllSntMsg		(ActCht+ 25)
#define ActDelAllRcvMsg		(ActCht+ 26)
#define ActDelSntMsg		(ActCht+ 27)
#define ActDelRcvMsg		(ActCht+ 28)
#define ActExpSntMsg		(ActCht+ 29)
#define ActExpRcvMsg		(ActCht+ 30)
#define ActConSntMsg		(ActCht+ 31)
#define ActConRcvMsg		(ActCht+ 32)
#define ActLstBanUsr		(ActCht+ 33)
#define ActBanUsrMsg		(ActCht+ 34)
#define ActUnbUsrMsg		(ActCht+ 35)
#define ActUnbUsrLst		(ActCht+ 36)

/*****************************************************************************/
/****************************** Statistics tab *******************************/
/*****************************************************************************/
// Actions in menu
#define ActSeeAllSvy		(ActUnbUsrLst+  1)
#define ActReqUseGbl		(ActUnbUsrLst+  2)
#define ActSeePhoDeg		(ActUnbUsrLst+  3)
#define ActReqStaCrs		(ActUnbUsrLst+  4)
#define ActReqAccGbl		(ActUnbUsrLst+  5)
// Secondary actions
#define ActSeeOneSvy		(ActUnbUsrLst+  6)
#define ActAnsSvy		(ActUnbUsrLst+  7)
#define ActFrmNewSvy		(ActUnbUsrLst+  8)
#define ActEdiOneSvy		(ActUnbUsrLst+  9)
#define ActNewSvy		(ActUnbUsrLst+ 10)
#define ActChgSvy		(ActUnbUsrLst+ 11)
#define ActReqRemSvy		(ActUnbUsrLst+ 12)
#define ActRemSvy		(ActUnbUsrLst+ 13)
#define ActReqRstSvy		(ActUnbUsrLst+ 14)
#define ActRstSvy		(ActUnbUsrLst+ 15)
#define ActHidSvy		(ActUnbUsrLst+ 16)
#define ActShoSvy		(ActUnbUsrLst+ 17)
#define ActEdiOneSvyQst		(ActUnbUsrLst+ 18)
#define ActRcvSvyQst		(ActUnbUsrLst+ 19)
#define ActReqRemSvyQst		(ActUnbUsrLst+ 20)
#define ActRemSvyQst		(ActUnbUsrLst+ 21)

#define ActSeeUseGbl		(ActUnbUsrLst+ 22)
#define ActPrnPhoDeg		(ActUnbUsrLst+ 23)
#define ActCalPhoDeg		(ActUnbUsrLst+ 24)
#define ActSeeAccGbl		(ActUnbUsrLst+ 25)
#define ActReqAccCrs		(ActUnbUsrLst+ 26)
#define ActSeeAccCrs		(ActUnbUsrLst+ 27)
#define ActSeeAllStaCrs		(ActUnbUsrLst+ 28)

/*****************************************************************************/
/******************************** Profile tab ********************************/
/*****************************************************************************/
// Actions in menu
#define ActFrmLogIn		(ActSeeAllStaCrs+ 1)
#define ActFrmRolSes		(ActSeeAllStaCrs+ 2)
#define ActMyCrs		(ActSeeAllStaCrs+ 3)
#define ActSeeMyTT		(ActSeeAllStaCrs+ 4)
#define ActFrmUsrAcc		(ActSeeAllStaCrs+ 5)
#define ActReqEdiRecCom		(ActSeeAllStaCrs+ 6)
#define ActEdiPrf		(ActSeeAllStaCrs+ 7)
#define ActAdmBrf		(ActSeeAllStaCrs+ 8)
#define ActMFUAct		(ActSeeAllStaCrs+ 9)
// Secondary actions
#define ActReqSndNewPwd		(ActSeeAllStaCrs+10)
#define ActSndNewPwd		(ActSeeAllStaCrs+11)
#define ActLogOut		(ActSeeAllStaCrs+12)
#define ActAutUsrInt		(ActSeeAllStaCrs+13)
#define ActAutUsrExt		(ActSeeAllStaCrs+14)
#define ActAutUsrChgLan		(ActSeeAllStaCrs+15)
#define ActAnnSee		(ActSeeAllStaCrs+16)
#define ActChgMyRol		(ActSeeAllStaCrs+17)
#define ActCreUsrAcc		(ActSeeAllStaCrs+18)
#define ActRemID_Me		(ActSeeAllStaCrs+19)
#define ActNewIDMe		(ActSeeAllStaCrs+20)
#define ActRemOldNic		(ActSeeAllStaCrs+21)
#define ActChgNic		(ActSeeAllStaCrs+22)
#define ActRemMaiMe		(ActSeeAllStaCrs+23)
#define ActNewMaiMe		(ActSeeAllStaCrs+24)
#define ActCnfMai		(ActSeeAllStaCrs+25)
#define ActFrmChgMyPwd		(ActSeeAllStaCrs+26)
#define ActChgPwd		(ActSeeAllStaCrs+27)
#define ActReqRemMyAcc		(ActSeeAllStaCrs+28)
#define ActRemMyAcc		(ActSeeAllStaCrs+29)

#define ActChgMyData		(ActSeeAllStaCrs+30)

#define ActReqMyPho		(ActSeeAllStaCrs+31)
#define ActDetMyPho		(ActSeeAllStaCrs+32)
#define ActUpdMyPho		(ActSeeAllStaCrs+33)
#define ActRemMyPho		(ActSeeAllStaCrs+34)

#define ActEdiPri		(ActSeeAllStaCrs+35)
#define ActChgPriPho		(ActSeeAllStaCrs+36)
#define ActChgPriPrf		(ActSeeAllStaCrs+37)

#define ActReqEdiMyIns		(ActSeeAllStaCrs+38)
#define ActChgCtyMyIns		(ActSeeAllStaCrs+39)
#define ActChgMyIns		(ActSeeAllStaCrs+40)
#define ActChgMyCtr		(ActSeeAllStaCrs+41)
#define ActChgMyDpt		(ActSeeAllStaCrs+42)
#define ActChgMyOff		(ActSeeAllStaCrs+43)
#define ActChgMyOffPho		(ActSeeAllStaCrs+44)

#define ActReqEdiMyNet		(ActSeeAllStaCrs+45)
#define ActChgMyNet		(ActSeeAllStaCrs+46)

#define ActChgThe		(ActSeeAllStaCrs+47)
#define ActReqChgLan		(ActSeeAllStaCrs+48)
#define ActChgLan		(ActSeeAllStaCrs+49)
#define ActChg1stDay		(ActSeeAllStaCrs+50)
#define ActChgCol		(ActSeeAllStaCrs+51)
#define ActHidLftCol		(ActSeeAllStaCrs+52)
#define ActHidRgtCol		(ActSeeAllStaCrs+53)
#define ActShoLftCol		(ActSeeAllStaCrs+54)
#define ActShoRgtCol		(ActSeeAllStaCrs+55)
#define ActChgIco		(ActSeeAllStaCrs+56)
#define ActChgMnu		(ActSeeAllStaCrs+57)
#define ActChgNtfPrf		(ActSeeAllStaCrs+58)

#define ActPrnUsrQR		(ActSeeAllStaCrs+59)

#define ActPrnMyTT		(ActSeeAllStaCrs+60)
#define ActEdiTut		(ActSeeAllStaCrs+61)
#define ActChgTut		(ActSeeAllStaCrs+62)
#define ActChgMyTT1stDay	(ActSeeAllStaCrs+63)

#define ActReqRemFilBrf		(ActSeeAllStaCrs+64)
#define ActRemFilBrf		(ActSeeAllStaCrs+65)
#define ActRemFolBrf		(ActSeeAllStaCrs+66)
#define ActCopBrf		(ActSeeAllStaCrs+67)
#define ActPasBrf		(ActSeeAllStaCrs+68)
#define ActRemTreBrf		(ActSeeAllStaCrs+69)
#define ActFrmCreBrf		(ActSeeAllStaCrs+70)
#define ActCreFolBrf		(ActSeeAllStaCrs+71)
#define ActCreLnkBrf		(ActSeeAllStaCrs+72)
#define ActRenFolBrf		(ActSeeAllStaCrs+73)
#define ActRcvFilBrfDZ		(ActSeeAllStaCrs+74)
#define ActRcvFilBrfCla		(ActSeeAllStaCrs+75)
#define ActExpBrf		(ActSeeAllStaCrs+76)
#define ActConBrf		(ActSeeAllStaCrs+77)
#define ActZIPBrf		(ActSeeAllStaCrs+78)
#define ActReqDatBrf		(ActSeeAllStaCrs+79)
#define ActChgDatBrf		(ActSeeAllStaCrs+80)
#define ActDowBrf		(ActSeeAllStaCrs+81)

#define ActReqRemOldBrf		(ActSeeAllStaCrs+82)
#define ActRemOldBrf		(ActSeeAllStaCrs+83)

/*****************************************************************************/
/******************************** Public types *******************************/
/*****************************************************************************/

struct Act_Tabs
  {
   char *Icon;
  };

struct Act_Actions
  {
   long ActCod;	// Unique, time-persistent numerical code for the action
   signed int IndexInMenu;
   Act_Tab_t Tab;
   Act_Action_t SuperAction;
   unsigned PermisIfIBelongToCrs;
   unsigned PermisIfIDontBelongToCrs;
   unsigned PermisIfNoCrsSelected;
   Act_Content_t ContentType;
   Act_BrowserWindow_t BrowserWindow;
   void (*FunctionPriori)();
   void (*FunctionPosteriori)();
   const char *Icon;
  };

struct Act_ListMFUActions
  {
   unsigned NumActions;
   Act_Action_t *Actions;
  };

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

bool Act_CheckIfIHavePermissionToExecuteAction (Act_Action_t Action);
const char *Act_GetTitleAction (Act_Action_t Action);
const char *Act_GetSubtitleAction (Act_Action_t Action);
void Act_GetBreadcrumbStrForAction (Act_Action_t Action,bool HTML,char *BreadcrumbStr);
char *Act_GetActionTextFromDB (long ActCod,char *Txt);

void Act_FormGoToStart (Act_Action_t NextAction);
void Act_FormStart (Act_Action_t NextAction);
void Act_FormStartUnique (Act_Action_t NextAction);
void Act_FormStartAnchor (Act_Action_t NextAction,const char *Anchor);
void Act_FormStartUniqueAnchor (Act_Action_t NextAction,const char *Anchor);
void Act_FormStartId (Act_Action_t NextAction,const char *Id);
void Act_SetParamsForm (char *Params,Act_Action_t NextAction,bool PutParameterLocationIfNoSesion);
void Act_FormEnd (void);
void Act_LinkFormSubmit (const char *Title,const char *LinkStyle);
void Act_LinkFormSubmitUnique (const char *Title,const char *LinkStyle);
void Act_LinkFormSubmitId (const char *Title,const char *LinkStyle,const char *Id);
void Act_LinkFormSubmitAnimated (const char *Title,const char *LinkStyle);

void Act_SetUniqueId (char UniqueId[Act_MAX_LENGTH_ID]);

void Act_AdjustActionWhenNoUsrLogged (void);
void Act_AdjustCurrentAction (void);

void Act_AllocateMFUActions (struct Act_ListMFUActions *ListMFUActions,unsigned MaxActionsShown);
void Act_FreeMFUActions (struct Act_ListMFUActions *ListMFUActions);
void Act_GetMFUActions (struct Act_ListMFUActions *ListMFUActions,unsigned MaxActionsShown);
Act_Action_t Act_GetMyLastActionInCurrentTab (void);
void Act_ShowMyMFUActions (void);
void Act_WriteBigMFUActions (struct Act_ListMFUActions *ListMFUActions);
void Act_WriteSmallMFUActions (struct Act_ListMFUActions *ListMFUActions);
void Act_UpdateMFUActions (void);

#endif
