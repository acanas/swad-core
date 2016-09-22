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

#define Act_NUM_ACTIONS	(1+9+51+15+90+70+67+246+186+156+172+36+30+86)

#define Act_MAX_ACTION_COD 1584

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
#define ActChgInsCrsCodCfg	(ActDowShaDeg+ 14)
#define ActChgCrsYeaCfg		(ActDowShaDeg+ 15)
#define ActEdiCrsInf		(ActDowShaDeg+ 16)
#define ActEdiTchGui		(ActDowShaDeg+ 17)
#define ActPrnCrsTT		(ActDowShaDeg+ 18)
#define ActEdiCrsTT		(ActDowShaDeg+ 19)
#define ActChgCrsTT		(ActDowShaDeg+ 20)
#define ActChgCrsTT1stDay	(ActDowShaDeg+ 21)
#define ActSeeSylLec		(ActDowShaDeg+ 22)
#define ActSeeSylPra		(ActDowShaDeg+ 23)
#define ActEdiSylLec		(ActDowShaDeg+ 24)
#define ActEdiSylPra		(ActDowShaDeg+ 25)
#define ActDelItmSylLec		(ActDowShaDeg+ 26)
#define ActDelItmSylPra		(ActDowShaDeg+ 27)
#define ActUp_IteSylLec		(ActDowShaDeg+ 28)
#define ActUp_IteSylPra		(ActDowShaDeg+ 29)
#define ActDwnIteSylLec		(ActDowShaDeg+ 30)
#define ActDwnIteSylPra		(ActDowShaDeg+ 31)
#define ActRgtIteSylLec		(ActDowShaDeg+ 32)
#define ActRgtIteSylPra		(ActDowShaDeg+ 33)
#define ActLftIteSylLec		(ActDowShaDeg+ 34)
#define ActLftIteSylPra		(ActDowShaDeg+ 35)
#define ActInsIteSylLec		(ActDowShaDeg+ 36)
#define ActInsIteSylPra		(ActDowShaDeg+ 37)
#define ActModIteSylLec		(ActDowShaDeg+ 38)
#define ActModIteSylPra		(ActDowShaDeg+ 39)

#define ActChgToSeeDocCrs	(ActDowShaDeg+ 40)
#define ActSeeDocCrs		(ActDowShaDeg+ 41)
#define ActExpSeeDocCrs		(ActDowShaDeg+ 42)
#define ActConSeeDocCrs		(ActDowShaDeg+ 43)
#define ActZIPSeeDocCrs		(ActDowShaDeg+ 44)
#define ActReqDatSeeDocCrs	(ActDowShaDeg+ 45)
#define ActDowSeeDocCrs		(ActDowShaDeg+ 46)

#define ActSeeDocGrp		(ActDowShaDeg+ 47)
#define ActExpSeeDocGrp		(ActDowShaDeg+ 48)
#define ActConSeeDocGrp		(ActDowShaDeg+ 49)
#define ActZIPSeeDocGrp		(ActDowShaDeg+ 50)
#define ActReqDatSeeDocGrp	(ActDowShaDeg+ 51)
#define ActDowSeeDocGrp		(ActDowShaDeg+ 52)

#define ActChgToAdmDocCrs	(ActDowShaDeg+ 53)
#define ActAdmDocCrs		(ActDowShaDeg+ 54)
#define ActReqRemFilDocCrs	(ActDowShaDeg+ 55)
#define ActRemFilDocCrs		(ActDowShaDeg+ 56)
#define ActRemFolDocCrs		(ActDowShaDeg+ 57)
#define ActCopDocCrs		(ActDowShaDeg+ 58)
#define ActPasDocCrs		(ActDowShaDeg+ 59)
#define ActRemTreDocCrs		(ActDowShaDeg+ 60)
#define ActFrmCreDocCrs		(ActDowShaDeg+ 61)
#define ActCreFolDocCrs		(ActDowShaDeg+ 62)
#define ActCreLnkDocCrs		(ActDowShaDeg+ 63)
#define ActRenFolDocCrs		(ActDowShaDeg+ 64)
#define ActRcvFilDocCrsDZ	(ActDowShaDeg+ 65)
#define ActRcvFilDocCrsCla	(ActDowShaDeg+ 66)
#define ActExpAdmDocCrs		(ActDowShaDeg+ 67)
#define ActConAdmDocCrs		(ActDowShaDeg+ 68)
#define ActZIPAdmDocCrs		(ActDowShaDeg+ 69)
#define ActShoDocCrs		(ActDowShaDeg+ 70)
#define ActHidDocCrs		(ActDowShaDeg+ 71)
#define ActReqDatAdmDocCrs	(ActDowShaDeg+ 72)
#define ActChgDatAdmDocCrs	(ActDowShaDeg+ 73)
#define ActDowAdmDocCrs		(ActDowShaDeg+ 74)

#define ActAdmDocGrp		(ActDowShaDeg+ 75)
#define ActReqRemFilDocGrp	(ActDowShaDeg+ 76)
#define ActRemFilDocGrp		(ActDowShaDeg+ 77)
#define ActRemFolDocGrp		(ActDowShaDeg+ 78)
#define ActCopDocGrp		(ActDowShaDeg+ 79)
#define ActPasDocGrp		(ActDowShaDeg+ 80)
#define ActRemTreDocGrp		(ActDowShaDeg+ 81)
#define ActFrmCreDocGrp		(ActDowShaDeg+ 82)
#define ActCreFolDocGrp		(ActDowShaDeg+ 83)
#define ActCreLnkDocGrp		(ActDowShaDeg+ 84)
#define ActRenFolDocGrp		(ActDowShaDeg+ 85)
#define ActRcvFilDocGrpDZ	(ActDowShaDeg+ 86)
#define ActRcvFilDocGrpCla	(ActDowShaDeg+ 87)
#define ActExpAdmDocGrp		(ActDowShaDeg+ 88)
#define ActConAdmDocGrp		(ActDowShaDeg+ 89)
#define ActZIPAdmDocGrp		(ActDowShaDeg+ 90)
#define ActShoDocGrp		(ActDowShaDeg+ 91)
#define ActHidDocGrp		(ActDowShaDeg+ 92)
#define ActReqDatAdmDocGrp	(ActDowShaDeg+ 93)
#define ActChgDatAdmDocGrp	(ActDowShaDeg+ 94)
#define ActDowAdmDocGrp		(ActDowShaDeg+ 95)

#define ActChgToAdmTch		(ActDowShaDeg+ 96)

#define ActAdmTchCrs		(ActDowShaDeg+ 97)
#define ActReqRemFilTchCrs	(ActDowShaDeg+ 98)
#define ActRemFilTchCrs		(ActDowShaDeg+ 99)
#define ActRemFolTchCrs		(ActDowShaDeg+100)
#define ActCopTchCrs		(ActDowShaDeg+101)
#define ActPasTchCrs		(ActDowShaDeg+102)
#define ActRemTreTchCrs		(ActDowShaDeg+103)
#define ActFrmCreTchCrs		(ActDowShaDeg+104)
#define ActCreFolTchCrs		(ActDowShaDeg+105)
#define ActCreLnkTchCrs		(ActDowShaDeg+106)
#define ActRenFolTchCrs		(ActDowShaDeg+107)
#define ActRcvFilTchCrsDZ	(ActDowShaDeg+108)
#define ActRcvFilTchCrsCla	(ActDowShaDeg+109)
#define ActExpTchCrs		(ActDowShaDeg+110)
#define ActConTchCrs		(ActDowShaDeg+111)
#define ActZIPTchCrs		(ActDowShaDeg+112)
#define ActReqDatTchCrs		(ActDowShaDeg+113)
#define ActChgDatTchCrs		(ActDowShaDeg+114)
#define ActDowTchCrs		(ActDowShaDeg+115)

#define ActAdmTchGrp		(ActDowShaDeg+116)
#define ActReqRemFilTchGrp	(ActDowShaDeg+117)
#define ActRemFilTchGrp		(ActDowShaDeg+118)
#define ActRemFolTchGrp		(ActDowShaDeg+119)
#define ActCopTchGrp		(ActDowShaDeg+120)
#define ActPasTchGrp		(ActDowShaDeg+121)
#define ActRemTreTchGrp		(ActDowShaDeg+122)
#define ActFrmCreTchGrp		(ActDowShaDeg+123)
#define ActCreFolTchGrp		(ActDowShaDeg+124)
#define ActCreLnkTchGrp		(ActDowShaDeg+125)
#define ActRenFolTchGrp		(ActDowShaDeg+126)
#define ActRcvFilTchGrpDZ	(ActDowShaDeg+127)
#define ActRcvFilTchGrpCla	(ActDowShaDeg+128)
#define ActExpTchGrp		(ActDowShaDeg+129)
#define ActConTchGrp		(ActDowShaDeg+130)
#define ActZIPTchGrp		(ActDowShaDeg+131)
#define ActReqDatTchGrp		(ActDowShaDeg+132)
#define ActChgDatTchGrp		(ActDowShaDeg+133)
#define ActDowTchGrp		(ActDowShaDeg+134)

#define ActChgToAdmSha		(ActDowShaDeg+135)

#define ActAdmShaCrs		(ActDowShaDeg+136)
#define ActReqRemFilShaCrs	(ActDowShaDeg+137)
#define ActRemFilShaCrs		(ActDowShaDeg+138)
#define ActRemFolShaCrs		(ActDowShaDeg+139)
#define ActCopShaCrs		(ActDowShaDeg+140)
#define ActPasShaCrs		(ActDowShaDeg+141)
#define ActRemTreShaCrs		(ActDowShaDeg+142)
#define ActFrmCreShaCrs		(ActDowShaDeg+143)
#define ActCreFolShaCrs		(ActDowShaDeg+144)
#define ActCreLnkShaCrs		(ActDowShaDeg+145)
#define ActRenFolShaCrs		(ActDowShaDeg+146)
#define ActRcvFilShaCrsDZ	(ActDowShaDeg+147)
#define ActRcvFilShaCrsCla	(ActDowShaDeg+148)
#define ActExpShaCrs		(ActDowShaDeg+149)
#define ActConShaCrs		(ActDowShaDeg+150)
#define ActZIPShaCrs		(ActDowShaDeg+151)
#define ActReqDatShaCrs		(ActDowShaDeg+152)
#define ActChgDatShaCrs		(ActDowShaDeg+153)
#define ActDowShaCrs		(ActDowShaDeg+154)

#define ActAdmShaGrp		(ActDowShaDeg+155)
#define ActReqRemFilShaGrp	(ActDowShaDeg+156)
#define ActRemFilShaGrp		(ActDowShaDeg+157)
#define ActRemFolShaGrp		(ActDowShaDeg+158)
#define ActCopShaGrp		(ActDowShaDeg+159)
#define ActPasShaGrp		(ActDowShaDeg+160)
#define ActRemTreShaGrp		(ActDowShaDeg+161)
#define ActFrmCreShaGrp		(ActDowShaDeg+162)
#define ActCreFolShaGrp		(ActDowShaDeg+163)
#define ActCreLnkShaGrp		(ActDowShaDeg+164)
#define ActRenFolShaGrp		(ActDowShaDeg+165)
#define ActRcvFilShaGrpDZ	(ActDowShaDeg+166)
#define ActRcvFilShaGrpCla	(ActDowShaDeg+167)
#define ActExpShaGrp		(ActDowShaDeg+168)
#define ActConShaGrp		(ActDowShaDeg+169)
#define ActZIPShaGrp		(ActDowShaDeg+170)
#define ActReqDatShaGrp		(ActDowShaDeg+171)
#define ActChgDatShaGrp		(ActDowShaDeg+172)
#define ActDowShaGrp		(ActDowShaDeg+173)

#define ActEdiBib		(ActDowShaDeg+174)
#define ActEdiFAQ		(ActDowShaDeg+175)
#define ActEdiCrsLnk		(ActDowShaDeg+176)

#define ActChgFrcReaCrsInf	(ActDowShaDeg+177)
#define ActChgFrcReaTchGui	(ActDowShaDeg+178)
#define ActChgFrcReaSylLec	(ActDowShaDeg+179)
#define ActChgFrcReaSylPra	(ActDowShaDeg+180)
#define ActChgFrcReaBib		(ActDowShaDeg+181)
#define ActChgFrcReaFAQ		(ActDowShaDeg+182)
#define ActChgFrcReaCrsLnk	(ActDowShaDeg+183)

#define ActChgHavReaCrsInf	(ActDowShaDeg+184)
#define ActChgHavReaTchGui	(ActDowShaDeg+185)
#define ActChgHavReaSylLec	(ActDowShaDeg+186)
#define ActChgHavReaSylPra	(ActDowShaDeg+187)
#define ActChgHavReaBib		(ActDowShaDeg+188)
#define ActChgHavReaFAQ		(ActDowShaDeg+189)
#define ActChgHavReaCrsLnk	(ActDowShaDeg+190)

#define ActSelInfSrcCrsInf	(ActDowShaDeg+191)
#define ActSelInfSrcTchGui	(ActDowShaDeg+192)
#define ActSelInfSrcSylLec	(ActDowShaDeg+193)
#define ActSelInfSrcSylPra	(ActDowShaDeg+194)
#define ActSelInfSrcBib		(ActDowShaDeg+195)
#define ActSelInfSrcFAQ		(ActDowShaDeg+196)
#define ActSelInfSrcCrsLnk	(ActDowShaDeg+197)
#define ActRcvURLCrsInf		(ActDowShaDeg+198)
#define ActRcvURLTchGui		(ActDowShaDeg+199)
#define ActRcvURLSylLec		(ActDowShaDeg+200)
#define ActRcvURLSylPra		(ActDowShaDeg+201)
#define ActRcvURLBib		(ActDowShaDeg+202)
#define ActRcvURLFAQ		(ActDowShaDeg+203)
#define ActRcvURLCrsLnk		(ActDowShaDeg+204)
#define ActRcvPagCrsInf		(ActDowShaDeg+205)
#define ActRcvPagTchGui		(ActDowShaDeg+206)
#define ActRcvPagSylLec		(ActDowShaDeg+207)
#define ActRcvPagSylPra		(ActDowShaDeg+208)
#define ActRcvPagBib		(ActDowShaDeg+209)
#define ActRcvPagFAQ		(ActDowShaDeg+210)
#define ActRcvPagCrsLnk		(ActDowShaDeg+211)
#define ActEditorCrsInf		(ActDowShaDeg+212)
#define ActEditorTchGui		(ActDowShaDeg+213)
#define ActEditorSylLec		(ActDowShaDeg+214)
#define ActEditorSylPra		(ActDowShaDeg+215)
#define ActEditorBib		(ActDowShaDeg+216)
#define ActEditorFAQ		(ActDowShaDeg+217)
#define ActEditorCrsLnk		(ActDowShaDeg+218)
#define ActPlaTxtEdiCrsInf	(ActDowShaDeg+219)
#define ActPlaTxtEdiTchGui	(ActDowShaDeg+220)
#define ActPlaTxtEdiSylLec	(ActDowShaDeg+221)
#define ActPlaTxtEdiSylPra	(ActDowShaDeg+222)
#define ActPlaTxtEdiBib		(ActDowShaDeg+223)
#define ActPlaTxtEdiFAQ		(ActDowShaDeg+224)
#define ActPlaTxtEdiCrsLnk	(ActDowShaDeg+225)
#define ActRchTxtEdiCrsInf	(ActDowShaDeg+226)
#define ActRchTxtEdiTchGui	(ActDowShaDeg+227)
#define ActRchTxtEdiSylLec	(ActDowShaDeg+228)
#define ActRchTxtEdiSylPra	(ActDowShaDeg+229)
#define ActRchTxtEdiBib		(ActDowShaDeg+230)
#define ActRchTxtEdiFAQ		(ActDowShaDeg+231)
#define ActRchTxtEdiCrsLnk	(ActDowShaDeg+232)
#define ActRcvPlaTxtCrsInf	(ActDowShaDeg+233)
#define ActRcvPlaTxtTchGui	(ActDowShaDeg+234)
#define ActRcvPlaTxtSylLec	(ActDowShaDeg+235)
#define ActRcvPlaTxtSylPra	(ActDowShaDeg+236)
#define ActRcvPlaTxtBib		(ActDowShaDeg+237)
#define ActRcvPlaTxtFAQ		(ActDowShaDeg+238)
#define ActRcvPlaTxtCrsLnk	(ActDowShaDeg+239)
#define ActRcvRchTxtCrsInf	(ActDowShaDeg+240)
#define ActRcvRchTxtTchGui	(ActDowShaDeg+241)
#define ActRcvRchTxtSylLec	(ActDowShaDeg+242)
#define ActRcvRchTxtSylPra	(ActDowShaDeg+243)
#define ActRcvRchTxtBib		(ActDowShaDeg+244)
#define ActRcvRchTxtFAQ		(ActDowShaDeg+245)
#define ActRcvRchTxtCrsLnk	(ActDowShaDeg+246)

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
#define ActSeeAllExaAnn		(ActRcvRchTxtCrsLnk+  7)
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

#define ActSeeOneExaAnn		(ActRcvRchTxtCrsLnk+125)
#define ActSeeDatExaAnn		(ActRcvRchTxtCrsLnk+126)
#define ActEdiExaAnn		(ActRcvRchTxtCrsLnk+127)
#define ActRcvExaAnn		(ActRcvRchTxtCrsLnk+128)
#define ActPrnExaAnn		(ActRcvRchTxtCrsLnk+129)
#define ActRemExaAnn		(ActRcvRchTxtCrsLnk+130)

#define ActChgToSeeMrk		(ActRcvRchTxtCrsLnk+131)

#define ActSeeMrkCrs		(ActRcvRchTxtCrsLnk+132)
#define ActExpSeeMrkCrs		(ActRcvRchTxtCrsLnk+133)
#define ActConSeeMrkCrs		(ActRcvRchTxtCrsLnk+134)
#define ActReqDatSeeMrkCrs	(ActRcvRchTxtCrsLnk+135)
#define ActSeeMyMrkCrs		(ActRcvRchTxtCrsLnk+136)

#define ActSeeMrkGrp		(ActRcvRchTxtCrsLnk+137)
#define ActExpSeeMrkGrp		(ActRcvRchTxtCrsLnk+138)
#define ActConSeeMrkGrp		(ActRcvRchTxtCrsLnk+139)
#define ActReqDatSeeMrkGrp	(ActRcvRchTxtCrsLnk+140)
#define ActSeeMyMrkGrp		(ActRcvRchTxtCrsLnk+141)

#define ActChgToAdmMrk		(ActRcvRchTxtCrsLnk+142)

#define ActAdmMrkCrs		(ActRcvRchTxtCrsLnk+143)
#define ActReqRemFilMrkCrs	(ActRcvRchTxtCrsLnk+144)
#define ActRemFilMrkCrs		(ActRcvRchTxtCrsLnk+145)
#define ActRemFolMrkCrs		(ActRcvRchTxtCrsLnk+146)
#define ActCopMrkCrs		(ActRcvRchTxtCrsLnk+147)
#define ActPasMrkCrs		(ActRcvRchTxtCrsLnk+148)
#define ActRemTreMrkCrs		(ActRcvRchTxtCrsLnk+149)
#define ActFrmCreMrkCrs		(ActRcvRchTxtCrsLnk+150)
#define ActCreFolMrkCrs		(ActRcvRchTxtCrsLnk+151)
#define ActRenFolMrkCrs		(ActRcvRchTxtCrsLnk+152)
#define ActRcvFilMrkCrsDZ	(ActRcvRchTxtCrsLnk+153)
#define ActRcvFilMrkCrsCla	(ActRcvRchTxtCrsLnk+154)
#define ActExpAdmMrkCrs		(ActRcvRchTxtCrsLnk+155)
#define ActConAdmMrkCrs		(ActRcvRchTxtCrsLnk+156)
#define ActZIPAdmMrkCrs		(ActRcvRchTxtCrsLnk+157)
#define ActShoMrkCrs		(ActRcvRchTxtCrsLnk+158)
#define ActHidMrkCrs		(ActRcvRchTxtCrsLnk+159)
#define ActReqDatAdmMrkCrs	(ActRcvRchTxtCrsLnk+160)
#define ActChgDatAdmMrkCrs	(ActRcvRchTxtCrsLnk+161)
#define ActDowAdmMrkCrs		(ActRcvRchTxtCrsLnk+162)
#define ActChgNumRowHeaCrs	(ActRcvRchTxtCrsLnk+163)
#define ActChgNumRowFooCrs	(ActRcvRchTxtCrsLnk+164)

#define ActAdmMrkGrp		(ActRcvRchTxtCrsLnk+165)
#define ActReqRemFilMrkGrp	(ActRcvRchTxtCrsLnk+166)
#define ActRemFilMrkGrp		(ActRcvRchTxtCrsLnk+167)
#define ActRemFolMrkGrp		(ActRcvRchTxtCrsLnk+168)
#define ActCopMrkGrp		(ActRcvRchTxtCrsLnk+169)
#define ActPasMrkGrp		(ActRcvRchTxtCrsLnk+170)
#define ActRemTreMrkGrp		(ActRcvRchTxtCrsLnk+171)
#define ActFrmCreMrkGrp		(ActRcvRchTxtCrsLnk+172)
#define ActCreFolMrkGrp		(ActRcvRchTxtCrsLnk+173)
#define ActRenFolMrkGrp		(ActRcvRchTxtCrsLnk+174)
#define ActRcvFilMrkGrpDZ	(ActRcvRchTxtCrsLnk+175)
#define ActRcvFilMrkGrpCla	(ActRcvRchTxtCrsLnk+176)
#define ActExpAdmMrkGrp		(ActRcvRchTxtCrsLnk+177)
#define ActConAdmMrkGrp		(ActRcvRchTxtCrsLnk+178)
#define ActZIPAdmMrkGrp		(ActRcvRchTxtCrsLnk+179)
#define ActShoMrkGrp		(ActRcvRchTxtCrsLnk+180)
#define ActHidMrkGrp		(ActRcvRchTxtCrsLnk+181)
#define ActReqDatAdmMrkGrp	(ActRcvRchTxtCrsLnk+182)
#define ActChgDatAdmMrkGrp	(ActRcvRchTxtCrsLnk+183)
#define ActDowAdmMrkGrp		(ActRcvRchTxtCrsLnk+184)
#define ActChgNumRowHeaGrp	(ActRcvRchTxtCrsLnk+185)
#define ActChgNumRowFooGrp	(ActRcvRchTxtCrsLnk+186)

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
#define ActReqRemOthPho		(ActChgNumRowFooGrp+ 90)
#define ActReqRemStdPho		(ActChgNumRowFooGrp+ 91)
#define ActReqRemTchPho		(ActChgNumRowFooGrp+ 92)
#define ActRemOthPho		(ActChgNumRowFooGrp+ 93)
#define ActRemStdPho		(ActChgNumRowFooGrp+ 94)
#define ActRemTchPho		(ActChgNumRowFooGrp+ 95)
#define ActCreOth		(ActChgNumRowFooGrp+ 96)
#define ActCreStd		(ActChgNumRowFooGrp+ 97)
#define ActCreTch		(ActChgNumRowFooGrp+ 98)
#define ActUpdOth		(ActChgNumRowFooGrp+ 99)
#define ActUpdStd		(ActChgNumRowFooGrp+100)
#define ActUpdTch		(ActChgNumRowFooGrp+101)

#define ActReqAccEnrStd		(ActChgNumRowFooGrp+102)
#define ActReqAccEnrTch		(ActChgNumRowFooGrp+103)
#define ActAccEnrStd		(ActChgNumRowFooGrp+104)
#define ActAccEnrTch		(ActChgNumRowFooGrp+105)
#define ActRemMe_Std		(ActChgNumRowFooGrp+106)
#define ActRemMe_Tch		(ActChgNumRowFooGrp+107)

#define ActNewAdmIns		(ActChgNumRowFooGrp+108)
#define ActRemAdmIns		(ActChgNumRowFooGrp+109)
#define ActNewAdmCtr		(ActChgNumRowFooGrp+110)
#define ActRemAdmCtr		(ActChgNumRowFooGrp+111)
#define ActNewAdmDeg		(ActChgNumRowFooGrp+112)
#define ActRemAdmDeg		(ActChgNumRowFooGrp+113)

#define ActRcvFrmEnrSevStd	(ActChgNumRowFooGrp+114)
#define ActRcvFrmEnrSevTch	(ActChgNumRowFooGrp+115)

#define ActReqCnfID_Oth		(ActChgNumRowFooGrp+116)
#define ActReqCnfID_Std		(ActChgNumRowFooGrp+117)
#define ActReqCnfID_Tch		(ActChgNumRowFooGrp+118)
#define ActCnfID_Oth		(ActChgNumRowFooGrp+119)
#define ActCnfID_Std		(ActChgNumRowFooGrp+120)
#define ActCnfID_Tch		(ActChgNumRowFooGrp+121)

#define ActFrmIDsOth		(ActChgNumRowFooGrp+122)
#define ActFrmIDsStd		(ActChgNumRowFooGrp+123)
#define ActFrmIDsTch		(ActChgNumRowFooGrp+124)
#define ActRemID_Oth		(ActChgNumRowFooGrp+125)
#define ActRemID_Std		(ActChgNumRowFooGrp+126)
#define ActRemID_Tch		(ActChgNumRowFooGrp+127)
#define ActNewID_Oth		(ActChgNumRowFooGrp+128)
#define ActNewID_Std		(ActChgNumRowFooGrp+129)
#define ActNewID_Tch		(ActChgNumRowFooGrp+130)
#define ActFrmPwdOth		(ActChgNumRowFooGrp+131)
#define ActFrmPwdStd		(ActChgNumRowFooGrp+132)
#define ActFrmPwdTch		(ActChgNumRowFooGrp+133)
#define ActChgPwdOth		(ActChgNumRowFooGrp+134)
#define ActChgPwdStd		(ActChgNumRowFooGrp+135)
#define ActChgPwdTch		(ActChgNumRowFooGrp+136)
#define ActFrmMaiOth		(ActChgNumRowFooGrp+137)
#define ActFrmMaiStd		(ActChgNumRowFooGrp+138)
#define ActFrmMaiTch		(ActChgNumRowFooGrp+139)
#define ActRemMaiOth		(ActChgNumRowFooGrp+140)
#define ActRemMaiStd		(ActChgNumRowFooGrp+141)
#define ActRemMaiTch		(ActChgNumRowFooGrp+142)
#define ActNewMaiOth		(ActChgNumRowFooGrp+143)
#define ActNewMaiStd		(ActChgNumRowFooGrp+144)
#define ActNewMaiTch		(ActChgNumRowFooGrp+145)

#define ActRemStdCrs		(ActChgNumRowFooGrp+146)
#define ActRemTchCrs		(ActChgNumRowFooGrp+147)
#define ActRemUsrGbl		(ActChgNumRowFooGrp+148)

#define ActReqRemAllStdCrs	(ActChgNumRowFooGrp+149)
#define ActRemAllStdCrs		(ActChgNumRowFooGrp+150)

#define ActReqRemOldUsr		(ActChgNumRowFooGrp+151)
#define ActRemOldUsr		(ActChgNumRowFooGrp+152)

#define ActLstDupUsr		(ActChgNumRowFooGrp+153)
#define ActLstSimUsr		(ActChgNumRowFooGrp+154)
#define ActRemDupUsr		(ActChgNumRowFooGrp+155)

#define ActLstClk		(ActChgNumRowFooGrp+156)

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
#define ActSeeAllNot		(ActCht+  3)
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
#define ActSeeOneNot		(ActCht+ 13)
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
#define ActSeeMyUsgRep		(ActUnbUsrLst+  6)

// Secondary actions
#define ActSeeOneSvy		(ActUnbUsrLst+  7)
#define ActAnsSvy		(ActUnbUsrLst+  8)
#define ActFrmNewSvy		(ActUnbUsrLst+  9)
#define ActEdiOneSvy		(ActUnbUsrLst+ 10)
#define ActNewSvy		(ActUnbUsrLst+ 11)
#define ActChgSvy		(ActUnbUsrLst+ 12)
#define ActReqRemSvy		(ActUnbUsrLst+ 13)
#define ActRemSvy		(ActUnbUsrLst+ 14)
#define ActReqRstSvy		(ActUnbUsrLst+ 15)
#define ActRstSvy		(ActUnbUsrLst+ 16)
#define ActHidSvy		(ActUnbUsrLst+ 17)
#define ActShoSvy		(ActUnbUsrLst+ 18)
#define ActEdiOneSvyQst		(ActUnbUsrLst+ 19)
#define ActRcvSvyQst		(ActUnbUsrLst+ 20)
#define ActReqRemSvyQst		(ActUnbUsrLst+ 21)
#define ActRemSvyQst		(ActUnbUsrLst+ 22)

#define ActSeeUseGbl		(ActUnbUsrLst+ 23)
#define ActPrnPhoDeg		(ActUnbUsrLst+ 24)
#define ActCalPhoDeg		(ActUnbUsrLst+ 25)
#define ActSeeAccGbl		(ActUnbUsrLst+ 26)
#define ActReqAccCrs		(ActUnbUsrLst+ 27)
#define ActSeeAccCrs		(ActUnbUsrLst+ 28)
#define ActSeeAllStaCrs		(ActUnbUsrLst+ 29)

#define ActPrnMyUsgRep		(ActUnbUsrLst+ 30)

/*****************************************************************************/
/******************************** Profile tab ********************************/
/*****************************************************************************/
// Actions in menu
#define ActFrmLogIn		(ActPrnMyUsgRep+ 1)
#define ActFrmRolSes		(ActPrnMyUsgRep+ 2)
#define ActMyCrs		(ActPrnMyUsgRep+ 3)
#define ActSeeMyTT		(ActPrnMyUsgRep+ 4)
#define ActSeeMyAgd		(ActPrnMyUsgRep+ 5)
#define ActFrmUsrAcc		(ActPrnMyUsgRep+ 6)
#define ActReqEdiRecCom		(ActPrnMyUsgRep+ 7)
#define ActEdiPrf		(ActPrnMyUsgRep+ 8)
#define ActAdmBrf		(ActPrnMyUsgRep+ 9)
#define ActMFUAct		(ActPrnMyUsgRep+10)
// Secondary actions
#define ActReqSndNewPwd		(ActPrnMyUsgRep+11)
#define ActSndNewPwd		(ActPrnMyUsgRep+12)
#define ActLogOut		(ActPrnMyUsgRep+13)
#define ActAutUsrInt		(ActPrnMyUsgRep+14)
#define ActAutUsrExt		(ActPrnMyUsgRep+15)
#define ActAutUsrChgLan		(ActPrnMyUsgRep+16)
#define ActAnnSee		(ActPrnMyUsgRep+17)
#define ActChgMyRol		(ActPrnMyUsgRep+18)
#define ActChkUsrAcc		(ActPrnMyUsgRep+19)
#define ActCreUsrAcc		(ActPrnMyUsgRep+20)
#define ActRemID_Me		(ActPrnMyUsgRep+21)
#define ActNewIDMe		(ActPrnMyUsgRep+22)
#define ActRemOldNic		(ActPrnMyUsgRep+23)
#define ActChgNic		(ActPrnMyUsgRep+24)
#define ActRemMaiMe		(ActPrnMyUsgRep+25)
#define ActNewMaiMe		(ActPrnMyUsgRep+26)
#define ActCnfMai		(ActPrnMyUsgRep+27)
#define ActFrmChgMyPwd		(ActPrnMyUsgRep+28)
#define ActChgPwd		(ActPrnMyUsgRep+29)
#define ActReqRemMyAcc		(ActPrnMyUsgRep+30)
#define ActRemMyAcc		(ActPrnMyUsgRep+31)

#define ActChgMyData		(ActPrnMyUsgRep+32)

#define ActReqMyPho		(ActPrnMyUsgRep+33)
#define ActDetMyPho		(ActPrnMyUsgRep+34)
#define ActUpdMyPho		(ActPrnMyUsgRep+35)
#define ActReqRemMyPho		(ActPrnMyUsgRep+36)
#define ActRemMyPho		(ActPrnMyUsgRep+37)

#define ActEdiPri		(ActPrnMyUsgRep+38)
#define ActChgPriPho		(ActPrnMyUsgRep+39)
#define ActChgPriPrf		(ActPrnMyUsgRep+40)

#define ActReqEdiMyIns		(ActPrnMyUsgRep+41)
#define ActChgCtyMyIns		(ActPrnMyUsgRep+42)
#define ActChgMyIns		(ActPrnMyUsgRep+43)
#define ActChgMyCtr		(ActPrnMyUsgRep+44)
#define ActChgMyDpt		(ActPrnMyUsgRep+45)
#define ActChgMyOff		(ActPrnMyUsgRep+46)
#define ActChgMyOffPho		(ActPrnMyUsgRep+47)

#define ActReqEdiMyNet		(ActPrnMyUsgRep+48)
#define ActChgMyNet		(ActPrnMyUsgRep+49)

#define ActChgThe		(ActPrnMyUsgRep+50)
#define ActReqChgLan		(ActPrnMyUsgRep+51)
#define ActChgLan		(ActPrnMyUsgRep+52)
#define ActChg1stDay		(ActPrnMyUsgRep+53)
#define ActChgCol		(ActPrnMyUsgRep+54)
#define ActHidLftCol		(ActPrnMyUsgRep+55)
#define ActHidRgtCol		(ActPrnMyUsgRep+56)
#define ActShoLftCol		(ActPrnMyUsgRep+57)
#define ActShoRgtCol		(ActPrnMyUsgRep+58)
#define ActChgIco		(ActPrnMyUsgRep+59)
#define ActChgMnu		(ActPrnMyUsgRep+60)
#define ActChgNtfPrf		(ActPrnMyUsgRep+61)

#define ActPrnUsrQR		(ActPrnMyUsgRep+62)

#define ActPrnMyTT		(ActPrnMyUsgRep+63)
#define ActEdiTut		(ActPrnMyUsgRep+64)
#define ActChgTut		(ActPrnMyUsgRep+65)
#define ActChgMyTT1stDay	(ActPrnMyUsgRep+66)

#define ActReqRemFilBrf		(ActPrnMyUsgRep+67)
#define ActRemFilBrf		(ActPrnMyUsgRep+68)
#define ActRemFolBrf		(ActPrnMyUsgRep+69)
#define ActCopBrf		(ActPrnMyUsgRep+70)
#define ActPasBrf		(ActPrnMyUsgRep+71)
#define ActRemTreBrf		(ActPrnMyUsgRep+72)
#define ActFrmCreBrf		(ActPrnMyUsgRep+73)
#define ActCreFolBrf		(ActPrnMyUsgRep+74)
#define ActCreLnkBrf		(ActPrnMyUsgRep+75)
#define ActRenFolBrf		(ActPrnMyUsgRep+76)
#define ActRcvFilBrfDZ		(ActPrnMyUsgRep+77)
#define ActRcvFilBrfCla		(ActPrnMyUsgRep+78)
#define ActExpBrf		(ActPrnMyUsgRep+79)
#define ActConBrf		(ActPrnMyUsgRep+80)
#define ActZIPBrf		(ActPrnMyUsgRep+81)
#define ActReqDatBrf		(ActPrnMyUsgRep+82)
#define ActChgDatBrf		(ActPrnMyUsgRep+83)
#define ActDowBrf		(ActPrnMyUsgRep+84)

#define ActReqRemOldBrf		(ActPrnMyUsgRep+85)
#define ActRemOldBrf		(ActPrnMyUsgRep+86)

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
void Act_FormStartOnSubmit (Act_Action_t NextAction,const char *OnSubmit);
void Act_FormStartUnique (Act_Action_t NextAction);
void Act_FormStartAnchor (Act_Action_t NextAction,const char *Anchor);
void Act_FormStartUniqueAnchor (Act_Action_t NextAction,const char *Anchor);
void Act_FormStartId (Act_Action_t NextAction,const char *Id);
void Act_SetParamsForm (char *ParamsStr,Act_Action_t NextAction,bool PutParameterLocationIfNoSesion);
void Act_FormEnd (void);
void Act_LinkFormSubmit (const char *Title,const char *LinkStyle,
                         const char *OnSubmit);
void Act_LinkFormSubmitUnique (const char *Title,const char *LinkStyle);
void Act_LinkFormSubmitId (const char *Title,const char *LinkStyle,
                           const char *Id,const char *OnSubmit);
void Act_LinkFormSubmitAnimated (const char *Title,const char *LinkStyle,
                                 const char *OnSubmit);

void Act_SetUniqueId (char UniqueId[Act_MAX_LENGTH_ID]);

void Act_AdjustActionWhenNoUsrLogged (void);
void Act_AdjustCurrentAction (void);

#endif
