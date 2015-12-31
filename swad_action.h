// swad_action.h: actions

#ifndef _SWAD_ACT
#define _SWAD_ACT
/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2015 Antonio Cañas Vargas

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

#define Act_NUM_ACTIONS	(7+52+15+90+72+67+205+183+143+151+36+27+82)

#define Act_MAX_ACTION_COD 1494

#define Act_MAX_OPTIONS_IN_MENU_PER_TAB 20

/*****************************************************************************/
/************************* Not asociates with tabs ***************************/
/*****************************************************************************/
#define ActAll		 0
#define ActUnk		 1
#define ActHom		 2
#define ActMnu		 3
#define ActRefCon	 4
#define ActRefLstClk	 5
#define ActWebSvc	 6

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
#define ActChgDegTypLog		(ActWebSvc+ 21)

#define ActRemOldCrs		(ActWebSvc+ 22)
#define ActEdiMai		(ActWebSvc+ 23)
#define ActNewMai		(ActWebSvc+ 24)
#define ActRemMai		(ActWebSvc+ 25)
#define ActRenMaiSho		(ActWebSvc+ 26)
#define ActRenMaiFul		(ActWebSvc+ 27)

#define ActEdiBan		(ActWebSvc+ 28)
#define ActNewBan		(ActWebSvc+ 29)
#define ActRemBan		(ActWebSvc+ 30)
#define ActShoBan		(ActWebSvc+ 31)
#define ActHidBan		(ActWebSvc+ 32)
#define ActRenBanSho		(ActWebSvc+ 33)
#define ActRenBanFul		(ActWebSvc+ 34)
#define ActChgBanImg		(ActWebSvc+ 35)
#define ActChgBanWWW		(ActWebSvc+ 36)
#define ActClkBan		(ActWebSvc+ 37)

#define ActEdiLnk		(ActWebSvc+ 38)
#define ActNewLnk		(ActWebSvc+ 39)
#define ActRemLnk		(ActWebSvc+ 40)
#define ActRenLnkSho		(ActWebSvc+ 41)
#define ActRenLnkFul		(ActWebSvc+ 42)
#define ActChgLnkWWW		(ActWebSvc+ 43)

#define ActEdiPlg		(ActWebSvc+ 44)
#define ActNewPlg		(ActWebSvc+ 45)
#define ActRemPlg		(ActWebSvc+ 46)
#define ActRenPlg		(ActWebSvc+ 47)
#define ActChgPlgDes		(ActWebSvc+ 48)
#define ActChgPlgLog		(ActWebSvc+ 49)
#define ActChgPlgAppKey		(ActWebSvc+ 50)
#define ActChgPlgURL		(ActWebSvc+ 51)
#define ActChgPlgIP		(ActWebSvc+ 52)

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
#define ActAdmComIns		(ActChgInsSta+  8)

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
#define ActAdmComCtr		(ActDowShaIns+  5)

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
#define ActChgDegFstYea		(ActDowShaIns+ 22)
#define ActChgDegLstYea		(ActDowShaIns+ 23)
// #define ActChgDegOptYea		(ActDowShaIns+ 24)
#define ActChgDegWWW		(ActDowShaIns+ 24)
#define ActChgDegSta		(ActDowShaIns+ 25)

#define ActChgToSeeDocCtr	(ActDowShaIns+ 26)
#define ActSeeDocCtr		(ActDowShaIns+ 27)
#define ActExpSeeDocCtr		(ActDowShaIns+ 28)
#define ActConSeeDocCtr		(ActDowShaIns+ 29)
#define ActZIPSeeDocCtr		(ActDowShaIns+ 30)
#define ActReqDatSeeDocCtr	(ActDowShaIns+ 31)
#define ActDowSeeDocCtr		(ActDowShaIns+ 32)
#define ActChgToAdmDocCtr	(ActDowShaIns+ 33)
#define ActAdmDocCtr		(ActDowShaIns+ 34)
#define ActReqRemFilDocCtr	(ActDowShaIns+ 35)
#define ActRemFilDocCtr		(ActDowShaIns+ 36)
#define ActRemFolDocCtr		(ActDowShaIns+ 37)
#define ActCopDocCtr		(ActDowShaIns+ 38)
#define ActPasDocCtr		(ActDowShaIns+ 39)
#define ActRemTreDocCtr		(ActDowShaIns+ 40)
#define ActFrmCreDocCtr		(ActDowShaIns+ 41)
#define ActCreFolDocCtr		(ActDowShaIns+ 42)
#define ActCreLnkDocCtr		(ActDowShaIns+ 43)
#define ActRenFolDocCtr		(ActDowShaIns+ 44)
#define ActRcvFilDocCtrDZ	(ActDowShaIns+ 45)
#define ActRcvFilDocCtrCla	(ActDowShaIns+ 46)
#define ActExpAdmDocCtr		(ActDowShaIns+ 47)
#define ActConAdmDocCtr		(ActDowShaIns+ 48)
#define ActZIPAdmDocCtr		(ActDowShaIns+ 49)
#define ActShoDocCtr		(ActDowShaIns+ 50)
#define ActHidDocCtr		(ActDowShaIns+ 51)
#define ActReqDatAdmDocCtr	(ActDowShaIns+ 52)
#define ActChgDatAdmDocCtr	(ActDowShaIns+ 53)
#define ActDowAdmDocCtr		(ActDowShaIns+ 54)

#define ActReqRemFilShaCtr	(ActDowShaIns+ 55)
#define ActRemFilShaCtr		(ActDowShaIns+ 56)
#define ActRemFolShaCtr		(ActDowShaIns+ 57)
#define ActCopShaCtr		(ActDowShaIns+ 58)
#define ActPasShaCtr		(ActDowShaIns+ 59)
#define ActRemTreShaCtr		(ActDowShaIns+ 60)
#define ActFrmCreShaCtr		(ActDowShaIns+ 61)
#define ActCreFolShaCtr		(ActDowShaIns+ 62)
#define ActCreLnkShaCtr		(ActDowShaIns+ 63)
#define ActRenFolShaCtr		(ActDowShaIns+ 64)
#define ActRcvFilShaCtrDZ	(ActDowShaIns+ 65)
#define ActRcvFilShaCtrCla	(ActDowShaIns+ 66)
#define ActExpShaCtr		(ActDowShaIns+ 67)
#define ActConShaCtr		(ActDowShaIns+ 68)
#define ActZIPShaCtr		(ActDowShaIns+ 69)
#define ActReqDatShaCtr		(ActDowShaIns+ 70)
#define ActChgDatShaCtr		(ActDowShaIns+ 71)
#define ActDowShaCtr		(ActDowShaIns+ 72)

/*****************************************************************************/
/********************************* Degree tab ********************************/
/*****************************************************************************/
// Actions in menu
#define ActDegReqSch		(ActDowShaCtr+  1)
#define ActSeeDegInf		(ActDowShaCtr+  2)
#define ActSeeCrs		(ActDowShaCtr+  3)
#define ActSeeAdmDocDeg		(ActDowShaCtr+  4)
#define ActAdmComDeg		(ActDowShaCtr+  5)

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
#define ActAdmShaCrsGrp		(ActDowShaDeg+  6)
#define ActSeeCrsTT		(ActDowShaDeg+  7)
#define ActSeeBib		(ActDowShaDeg+  8)
#define ActSeeFAQ		(ActDowShaDeg+  9)
#define ActSeeCrsLnk		(ActDowShaDeg+ 10)
// Secondary actions
#define ActCrsSch		(ActDowShaDeg+ 11)
#define ActPrnCrsInf		(ActDowShaDeg+ 12)
#define ActChgCrsLog		(ActDowShaDeg+ 13)
#define ActEdiCrsInf		(ActDowShaDeg+ 14)
#define ActEdiTchGui		(ActDowShaDeg+ 15)
#define ActPrnCrsTT		(ActDowShaDeg+ 16)
#define ActEdiCrsTT		(ActDowShaDeg+ 17)
#define ActChgCrsTT		(ActDowShaDeg+ 18)
#define ActChgCrsTT1stDay	(ActDowShaDeg+ 19)
#define ActSeeSylLec		(ActDowShaDeg+ 20)
#define ActSeeSylPra		(ActDowShaDeg+ 21)
#define ActEdiSylLec		(ActDowShaDeg+ 22)
#define ActEdiSylPra		(ActDowShaDeg+ 23)
#define ActDelItmSylLec		(ActDowShaDeg+ 24)
#define ActDelItmSylPra		(ActDowShaDeg+ 25)
#define ActUp_IteSylLec		(ActDowShaDeg+ 26)
#define ActUp_IteSylPra		(ActDowShaDeg+ 27)
#define ActDwnIteSylLec		(ActDowShaDeg+ 28)
#define ActDwnIteSylPra		(ActDowShaDeg+ 29)
#define ActRgtIteSylLec		(ActDowShaDeg+ 30)
#define ActRgtIteSylPra		(ActDowShaDeg+ 31)
#define ActLftIteSylLec		(ActDowShaDeg+ 32)
#define ActLftIteSylPra		(ActDowShaDeg+ 33)
#define ActInsIteSylLec		(ActDowShaDeg+ 34)
#define ActInsIteSylPra		(ActDowShaDeg+ 35)
#define ActModIteSylLec		(ActDowShaDeg+ 36)
#define ActModIteSylPra		(ActDowShaDeg+ 37)

#define ActChgToSeeDocCrs	(ActDowShaDeg+ 38)
#define ActSeeDocCrs		(ActDowShaDeg+ 39)
#define ActExpSeeDocCrs		(ActDowShaDeg+ 40)
#define ActConSeeDocCrs		(ActDowShaDeg+ 41)
#define ActZIPSeeDocCrs		(ActDowShaDeg+ 42)
#define ActReqDatSeeDocCrs	(ActDowShaDeg+ 43)
#define ActDowSeeDocCrs		(ActDowShaDeg+ 44)

#define ActSeeDocGrp		(ActDowShaDeg+ 45)
#define ActExpSeeDocGrp		(ActDowShaDeg+ 46)
#define ActConSeeDocGrp		(ActDowShaDeg+ 47)
#define ActZIPSeeDocGrp		(ActDowShaDeg+ 48)
#define ActReqDatSeeDocGrp	(ActDowShaDeg+ 49)
#define ActDowSeeDocGrp		(ActDowShaDeg+ 50)

#define ActChgToAdmDocCrs	(ActDowShaDeg+ 51)
#define ActAdmDocCrs		(ActDowShaDeg+ 52)
#define ActReqRemFilDocCrs	(ActDowShaDeg+ 53)
#define ActRemFilDocCrs		(ActDowShaDeg+ 54)
#define ActRemFolDocCrs		(ActDowShaDeg+ 55)
#define ActCopDocCrs		(ActDowShaDeg+ 56)
#define ActPasDocCrs		(ActDowShaDeg+ 57)
#define ActRemTreDocCrs		(ActDowShaDeg+ 58)
#define ActFrmCreDocCrs		(ActDowShaDeg+ 59)
#define ActCreFolDocCrs		(ActDowShaDeg+ 60)
#define ActCreLnkDocCrs		(ActDowShaDeg+ 61)
#define ActRenFolDocCrs		(ActDowShaDeg+ 62)
#define ActRcvFilDocCrsDZ	(ActDowShaDeg+ 63)
#define ActRcvFilDocCrsCla	(ActDowShaDeg+ 64)
#define ActExpAdmDocCrs		(ActDowShaDeg+ 65)
#define ActConAdmDocCrs		(ActDowShaDeg+ 66)
#define ActZIPAdmDocCrs		(ActDowShaDeg+ 67)
#define ActShoDocCrs		(ActDowShaDeg+ 68)
#define ActHidDocCrs		(ActDowShaDeg+ 69)
#define ActReqDatAdmDocCrs	(ActDowShaDeg+ 70)
#define ActChgDatAdmDocCrs	(ActDowShaDeg+ 71)
#define ActDowAdmDocCrs		(ActDowShaDeg+ 72)

#define ActAdmDocGrp		(ActDowShaDeg+ 73)
#define ActReqRemFilDocGrp	(ActDowShaDeg+ 74)
#define ActRemFilDocGrp		(ActDowShaDeg+ 75)
#define ActRemFolDocGrp		(ActDowShaDeg+ 76)
#define ActCopDocGrp		(ActDowShaDeg+ 77)
#define ActPasDocGrp		(ActDowShaDeg+ 78)
#define ActRemTreDocGrp		(ActDowShaDeg+ 79)
#define ActFrmCreDocGrp		(ActDowShaDeg+ 80)
#define ActCreFolDocGrp		(ActDowShaDeg+ 81)
#define ActCreLnkDocGrp		(ActDowShaDeg+ 82)
#define ActRenFolDocGrp		(ActDowShaDeg+ 83)
#define ActRcvFilDocGrpDZ	(ActDowShaDeg+ 84)
#define ActRcvFilDocGrpCla	(ActDowShaDeg+ 85)
#define ActExpAdmDocGrp		(ActDowShaDeg+ 86)
#define ActConAdmDocGrp		(ActDowShaDeg+ 87)
#define ActZIPAdmDocGrp		(ActDowShaDeg+ 88)
#define ActShoDocGrp		(ActDowShaDeg+ 89)
#define ActHidDocGrp		(ActDowShaDeg+ 90)
#define ActReqDatAdmDocGrp	(ActDowShaDeg+ 91)
#define ActChgDatAdmDocGrp	(ActDowShaDeg+ 92)
#define ActDowAdmDocGrp		(ActDowShaDeg+ 93)

#define ActChgToAdmSha		(ActDowShaDeg+ 94)

#define ActAdmShaCrs		(ActDowShaDeg+ 95)
#define ActReqRemFilShaCrs	(ActDowShaDeg+ 96)
#define ActRemFilShaCrs		(ActDowShaDeg+ 97)
#define ActRemFolShaCrs		(ActDowShaDeg+ 98)
#define ActCopShaCrs		(ActDowShaDeg+ 99)
#define ActPasShaCrs		(ActDowShaDeg+100)
#define ActRemTreShaCrs		(ActDowShaDeg+101)
#define ActFrmCreShaCrs		(ActDowShaDeg+102)
#define ActCreFolShaCrs		(ActDowShaDeg+103)
#define ActCreLnkShaCrs		(ActDowShaDeg+104)
#define ActRenFolShaCrs		(ActDowShaDeg+105)
#define ActRcvFilShaCrsDZ	(ActDowShaDeg+106)
#define ActRcvFilShaCrsCla	(ActDowShaDeg+107)
#define ActExpShaCrs		(ActDowShaDeg+108)
#define ActConShaCrs		(ActDowShaDeg+109)
#define ActZIPShaCrs		(ActDowShaDeg+110)
#define ActReqDatShaCrs		(ActDowShaDeg+111)
#define ActChgDatShaCrs		(ActDowShaDeg+112)
#define ActDowShaCrs		(ActDowShaDeg+113)

#define ActAdmShaGrp		(ActDowShaDeg+114)
#define ActReqRemFilShaGrp	(ActDowShaDeg+115)
#define ActRemFilShaGrp		(ActDowShaDeg+116)
#define ActRemFolShaGrp		(ActDowShaDeg+117)
#define ActCopShaGrp		(ActDowShaDeg+118)
#define ActPasShaGrp		(ActDowShaDeg+119)
#define ActRemTreShaGrp		(ActDowShaDeg+120)
#define ActFrmCreShaGrp		(ActDowShaDeg+121)
#define ActCreFolShaGrp		(ActDowShaDeg+122)
#define ActCreLnkShaGrp		(ActDowShaDeg+123)
#define ActRenFolShaGrp		(ActDowShaDeg+124)
#define ActRcvFilShaGrpDZ	(ActDowShaDeg+125)
#define ActRcvFilShaGrpCla	(ActDowShaDeg+126)
#define ActExpShaGrp		(ActDowShaDeg+127)
#define ActConShaGrp		(ActDowShaDeg+128)
#define ActZIPShaGrp		(ActDowShaDeg+129)
#define ActReqDatShaGrp		(ActDowShaDeg+130)
#define ActChgDatShaGrp		(ActDowShaDeg+131)
#define ActDowShaGrp		(ActDowShaDeg+132)

#define ActEdiBib		(ActDowShaDeg+133)
#define ActEdiFAQ		(ActDowShaDeg+134)
#define ActEdiCrsLnk		(ActDowShaDeg+135)

#define ActChgFrcReaCrsInf	(ActDowShaDeg+136)
#define ActChgFrcReaTchGui	(ActDowShaDeg+137)
#define ActChgFrcReaSylLec	(ActDowShaDeg+138)
#define ActChgFrcReaSylPra	(ActDowShaDeg+139)
#define ActChgFrcReaBib		(ActDowShaDeg+140)
#define ActChgFrcReaFAQ		(ActDowShaDeg+141)
#define ActChgFrcReaCrsLnk	(ActDowShaDeg+142)

#define ActChgHavReaCrsInf	(ActDowShaDeg+143)
#define ActChgHavReaTchGui	(ActDowShaDeg+144)
#define ActChgHavReaSylLec	(ActDowShaDeg+145)
#define ActChgHavReaSylPra	(ActDowShaDeg+146)
#define ActChgHavReaBib		(ActDowShaDeg+147)
#define ActChgHavReaFAQ		(ActDowShaDeg+148)
#define ActChgHavReaCrsLnk	(ActDowShaDeg+149)

#define ActSelInfSrcCrsInf	(ActDowShaDeg+150)
#define ActSelInfSrcTchGui	(ActDowShaDeg+151)
#define ActSelInfSrcSylLec	(ActDowShaDeg+152)
#define ActSelInfSrcSylPra	(ActDowShaDeg+153)
#define ActSelInfSrcBib		(ActDowShaDeg+154)
#define ActSelInfSrcFAQ		(ActDowShaDeg+155)
#define ActSelInfSrcCrsLnk	(ActDowShaDeg+156)
#define ActRcvURLCrsInf		(ActDowShaDeg+157)
#define ActRcvURLTchGui		(ActDowShaDeg+158)
#define ActRcvURLSylLec		(ActDowShaDeg+159)
#define ActRcvURLSylPra		(ActDowShaDeg+160)
#define ActRcvURLBib		(ActDowShaDeg+161)
#define ActRcvURLFAQ		(ActDowShaDeg+162)
#define ActRcvURLCrsLnk		(ActDowShaDeg+163)
#define ActRcvPagCrsInf		(ActDowShaDeg+164)
#define ActRcvPagTchGui		(ActDowShaDeg+165)
#define ActRcvPagSylLec		(ActDowShaDeg+166)
#define ActRcvPagSylPra		(ActDowShaDeg+167)
#define ActRcvPagBib		(ActDowShaDeg+168)
#define ActRcvPagFAQ		(ActDowShaDeg+169)
#define ActRcvPagCrsLnk		(ActDowShaDeg+170)
#define ActEditorCrsInf		(ActDowShaDeg+171)
#define ActEditorTchGui		(ActDowShaDeg+172)
#define ActEditorSylLec		(ActDowShaDeg+173)
#define ActEditorSylPra		(ActDowShaDeg+174)
#define ActEditorBib		(ActDowShaDeg+175)
#define ActEditorFAQ		(ActDowShaDeg+176)
#define ActEditorCrsLnk		(ActDowShaDeg+177)
#define ActPlaTxtEdiCrsInf	(ActDowShaDeg+178)
#define ActPlaTxtEdiTchGui	(ActDowShaDeg+179)
#define ActPlaTxtEdiSylLec	(ActDowShaDeg+180)
#define ActPlaTxtEdiSylPra	(ActDowShaDeg+181)
#define ActPlaTxtEdiBib		(ActDowShaDeg+182)
#define ActPlaTxtEdiFAQ		(ActDowShaDeg+183)
#define ActPlaTxtEdiCrsLnk	(ActDowShaDeg+184)
#define ActRchTxtEdiCrsInf	(ActDowShaDeg+185)
#define ActRchTxtEdiTchGui	(ActDowShaDeg+186)
#define ActRchTxtEdiSylLec	(ActDowShaDeg+187)
#define ActRchTxtEdiSylPra	(ActDowShaDeg+188)
#define ActRchTxtEdiBib		(ActDowShaDeg+189)
#define ActRchTxtEdiFAQ		(ActDowShaDeg+190)
#define ActRchTxtEdiCrsLnk	(ActDowShaDeg+191)
#define ActRcvPlaTxtCrsInf	(ActDowShaDeg+192)
#define ActRcvPlaTxtTchGui	(ActDowShaDeg+193)
#define ActRcvPlaTxtSylLec	(ActDowShaDeg+194)
#define ActRcvPlaTxtSylPra	(ActDowShaDeg+195)
#define ActRcvPlaTxtBib		(ActDowShaDeg+196)
#define ActRcvPlaTxtFAQ		(ActDowShaDeg+197)
#define ActRcvPlaTxtCrsLnk	(ActDowShaDeg+198)
#define ActRcvRchTxtCrsInf	(ActDowShaDeg+199)
#define ActRcvRchTxtTchGui	(ActDowShaDeg+200)
#define ActRcvRchTxtSylLec	(ActDowShaDeg+201)
#define ActRcvRchTxtSylPra	(ActDowShaDeg+202)
#define ActRcvRchTxtBib		(ActDowShaDeg+203)
#define ActRcvRchTxtFAQ		(ActDowShaDeg+204)
#define ActRcvRchTxtCrsLnk	(ActDowShaDeg+205)

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
#define ActRemTstQst		(ActRcvRchTxtCrsLnk+109)
#define ActShfTstQst		(ActRcvRchTxtCrsLnk+110)
#define ActCfgTst		(ActRcvRchTxtCrsLnk+111)
#define ActEnableTag		(ActRcvRchTxtCrsLnk+112)
#define ActDisableTag		(ActRcvRchTxtCrsLnk+113)
#define ActRenTag		(ActRcvRchTxtCrsLnk+114)
#define ActRcvCfgTst		(ActRcvRchTxtCrsLnk+115)

#define ActReqSeeMyTstExa	(ActRcvRchTxtCrsLnk+116)
#define ActSeeMyTstExa		(ActRcvRchTxtCrsLnk+117)
#define ActSeeOneTstExaMe	(ActRcvRchTxtCrsLnk+118)
#define ActReqSeeUsrTstExa	(ActRcvRchTxtCrsLnk+119)
#define ActSeeUsrTstExa		(ActRcvRchTxtCrsLnk+120)
#define ActSeeOneTstExaOth	(ActRcvRchTxtCrsLnk+121)

#define ActPrnCal		(ActRcvRchTxtCrsLnk+122)
#define ActChgCal1stDay		(ActRcvRchTxtCrsLnk+123)

#define ActEdiExaAnn		(ActRcvRchTxtCrsLnk+124)
#define ActRcvExaAnn		(ActRcvRchTxtCrsLnk+125)
#define ActPrnExaAnn		(ActRcvRchTxtCrsLnk+126)
#define ActRemExaAnn		(ActRcvRchTxtCrsLnk+127)

#define ActChgToSeeMrk		(ActRcvRchTxtCrsLnk+128)

#define ActSeeMrkCrs		(ActRcvRchTxtCrsLnk+129)
#define ActExpSeeMrkCrs		(ActRcvRchTxtCrsLnk+130)
#define ActConSeeMrkCrs		(ActRcvRchTxtCrsLnk+131)
#define ActReqDatSeeMrkCrs	(ActRcvRchTxtCrsLnk+132)
#define ActSeeMyMrkCrs		(ActRcvRchTxtCrsLnk+133)

#define ActSeeMrkGrp		(ActRcvRchTxtCrsLnk+134)
#define ActExpSeeMrkGrp		(ActRcvRchTxtCrsLnk+135)
#define ActConSeeMrkGrp		(ActRcvRchTxtCrsLnk+136)
#define ActReqDatSeeMrkGrp	(ActRcvRchTxtCrsLnk+137)
#define ActSeeMyMrkGrp		(ActRcvRchTxtCrsLnk+138)

#define ActChgToAdmMrk		(ActRcvRchTxtCrsLnk+139)

#define ActAdmMrkCrs		(ActRcvRchTxtCrsLnk+140)
#define ActReqRemFilMrkCrs	(ActRcvRchTxtCrsLnk+141)
#define ActRemFilMrkCrs		(ActRcvRchTxtCrsLnk+142)
#define ActRemFolMrkCrs		(ActRcvRchTxtCrsLnk+143)
#define ActCopMrkCrs		(ActRcvRchTxtCrsLnk+144)
#define ActPasMrkCrs		(ActRcvRchTxtCrsLnk+145)
#define ActRemTreMrkCrs		(ActRcvRchTxtCrsLnk+146)
#define ActFrmCreMrkCrs		(ActRcvRchTxtCrsLnk+147)
#define ActCreFolMrkCrs		(ActRcvRchTxtCrsLnk+148)
#define ActRenFolMrkCrs		(ActRcvRchTxtCrsLnk+149)
#define ActRcvFilMrkCrsDZ	(ActRcvRchTxtCrsLnk+150)
#define ActRcvFilMrkCrsCla	(ActRcvRchTxtCrsLnk+151)
#define ActExpAdmMrkCrs		(ActRcvRchTxtCrsLnk+152)
#define ActConAdmMrkCrs		(ActRcvRchTxtCrsLnk+153)
#define ActZIPAdmMrkCrs		(ActRcvRchTxtCrsLnk+154)
#define ActShoMrkCrs		(ActRcvRchTxtCrsLnk+155)
#define ActHidMrkCrs		(ActRcvRchTxtCrsLnk+156)
#define ActReqDatAdmMrkCrs	(ActRcvRchTxtCrsLnk+157)
#define ActChgDatAdmMrkCrs	(ActRcvRchTxtCrsLnk+158)
#define ActDowAdmMrkCrs		(ActRcvRchTxtCrsLnk+159)
#define ActChgNumRowHeaCrs	(ActRcvRchTxtCrsLnk+160)
#define ActChgNumRowFooCrs	(ActRcvRchTxtCrsLnk+161)

#define ActAdmMrkGrp		(ActRcvRchTxtCrsLnk+162)
#define ActReqRemFilMrkGrp	(ActRcvRchTxtCrsLnk+163)
#define ActRemFilMrkGrp		(ActRcvRchTxtCrsLnk+164)
#define ActRemFolMrkGrp		(ActRcvRchTxtCrsLnk+165)
#define ActCopMrkGrp		(ActRcvRchTxtCrsLnk+166)
#define ActPasMrkGrp		(ActRcvRchTxtCrsLnk+167)
#define ActRemTreMrkGrp		(ActRcvRchTxtCrsLnk+168)
#define ActFrmCreMrkGrp		(ActRcvRchTxtCrsLnk+169)
#define ActCreFolMrkGrp		(ActRcvRchTxtCrsLnk+170)
#define ActRenFolMrkGrp		(ActRcvRchTxtCrsLnk+171)
#define ActRcvFilMrkGrpDZ	(ActRcvRchTxtCrsLnk+172)
#define ActRcvFilMrkGrpCla	(ActRcvRchTxtCrsLnk+173)
#define ActExpAdmMrkGrp		(ActRcvRchTxtCrsLnk+174)
#define ActConAdmMrkGrp		(ActRcvRchTxtCrsLnk+175)
#define ActZIPAdmMrkGrp		(ActRcvRchTxtCrsLnk+176)
#define ActShoMrkGrp		(ActRcvRchTxtCrsLnk+177)
#define ActHidMrkGrp		(ActRcvRchTxtCrsLnk+178)
#define ActReqDatAdmMrkGrp	(ActRcvRchTxtCrsLnk+179)
#define ActChgDatAdmMrkGrp	(ActRcvRchTxtCrsLnk+180)
#define ActDowAdmMrkGrp		(ActRcvRchTxtCrsLnk+181)
#define ActChgNumRowHeaGrp	(ActRcvRchTxtCrsLnk+182)
#define ActChgNumRowFooGrp	(ActRcvRchTxtCrsLnk+183)

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
#define ActReqRejSignUp		(ActChgNumRowFooGrp+ 72)
#define ActRejSignUp		(ActChgNumRowFooGrp+ 73)

#define ActReqMdfOneOth		(ActChgNumRowFooGrp+ 74)
#define ActReqMdfOneStd		(ActChgNumRowFooGrp+ 75)
#define ActReqMdfOneTch		(ActChgNumRowFooGrp+ 76)

#define ActReqMdfOth		(ActChgNumRowFooGrp+ 77)
#define ActReqMdfStd		(ActChgNumRowFooGrp+ 78)
#define ActReqMdfTch		(ActChgNumRowFooGrp+ 79)

#define ActReqOthPho		(ActChgNumRowFooGrp+ 80)
#define ActReqStdPho		(ActChgNumRowFooGrp+ 81)
#define ActReqTchPho		(ActChgNumRowFooGrp+ 82)
#define ActDetOthPho		(ActChgNumRowFooGrp+ 83)
#define ActDetStdPho		(ActChgNumRowFooGrp+ 84)
#define ActDetTchPho		(ActChgNumRowFooGrp+ 85)
#define ActUpdOthPho		(ActChgNumRowFooGrp+ 86)
#define ActUpdStdPho		(ActChgNumRowFooGrp+ 87)
#define ActUpdTchPho		(ActChgNumRowFooGrp+ 88)
#define ActRemOthPho		(ActChgNumRowFooGrp+ 89)
#define ActRemStdPho		(ActChgNumRowFooGrp+ 90)
#define ActRemTchPho		(ActChgNumRowFooGrp+ 91)
#define ActCreOth		(ActChgNumRowFooGrp+ 92)
#define ActCreStd		(ActChgNumRowFooGrp+ 93)
#define ActCreTch		(ActChgNumRowFooGrp+ 94)
#define ActUpdOth		(ActChgNumRowFooGrp+ 95)
#define ActUpdStd		(ActChgNumRowFooGrp+ 96)
#define ActUpdTch		(ActChgNumRowFooGrp+ 97)

#define ActReqAccEnrStd		(ActChgNumRowFooGrp+ 98)
#define ActReqAccEnrTch		(ActChgNumRowFooGrp+ 99)
#define ActAccEnrStd		(ActChgNumRowFooGrp+100)
#define ActAccEnrTch		(ActChgNumRowFooGrp+101)
#define ActRemMe_Std		(ActChgNumRowFooGrp+102)
#define ActRemMe_Tch		(ActChgNumRowFooGrp+103)

#define ActNewAdmIns		(ActChgNumRowFooGrp+104)
#define ActRemAdmIns		(ActChgNumRowFooGrp+105)
#define ActNewAdmCtr		(ActChgNumRowFooGrp+106)
#define ActRemAdmCtr		(ActChgNumRowFooGrp+107)
#define ActNewAdmDeg		(ActChgNumRowFooGrp+108)
#define ActRemAdmDeg		(ActChgNumRowFooGrp+109)

#define ActRcvFrmEnrSevStd	(ActChgNumRowFooGrp+110)
#define ActRcvFrmEnrSevTch	(ActChgNumRowFooGrp+111)

#define ActFrmIDsOth		(ActChgNumRowFooGrp+112)
#define ActFrmIDsStd		(ActChgNumRowFooGrp+113)
#define ActFrmIDsTch		(ActChgNumRowFooGrp+114)
#define ActRemID_Oth		(ActChgNumRowFooGrp+115)
#define ActRemID_Std		(ActChgNumRowFooGrp+116)
#define ActRemID_Tch		(ActChgNumRowFooGrp+117)
#define ActNewID_Oth		(ActChgNumRowFooGrp+118)
#define ActNewID_Std		(ActChgNumRowFooGrp+119)
#define ActNewID_Tch		(ActChgNumRowFooGrp+120)
#define ActFrmPwdOth		(ActChgNumRowFooGrp+121)
#define ActFrmPwdStd		(ActChgNumRowFooGrp+122)
#define ActFrmPwdTch		(ActChgNumRowFooGrp+123)
#define ActChgPwdOth		(ActChgNumRowFooGrp+124)
#define ActChgPwdStd		(ActChgNumRowFooGrp+125)
#define ActChgPwdTch		(ActChgNumRowFooGrp+126)
#define ActFrmMaiOth		(ActChgNumRowFooGrp+127)
#define ActFrmMaiStd		(ActChgNumRowFooGrp+128)
#define ActFrmMaiTch		(ActChgNumRowFooGrp+129)
#define ActRemMaiOth		(ActChgNumRowFooGrp+130)
#define ActRemMaiStd		(ActChgNumRowFooGrp+131)
#define ActRemMaiTch		(ActChgNumRowFooGrp+132)
#define ActNewMaiOth		(ActChgNumRowFooGrp+133)
#define ActNewMaiStd		(ActChgNumRowFooGrp+134)
#define ActNewMaiTch		(ActChgNumRowFooGrp+135)

#define ActRemStdCrs		(ActChgNumRowFooGrp+136)
#define ActRemTchCrs		(ActChgNumRowFooGrp+137)
#define ActRemUsrGbl		(ActChgNumRowFooGrp+138)

#define ActReqRemAllStdCrs	(ActChgNumRowFooGrp+139)
#define ActRemAllStdCrs		(ActChgNumRowFooGrp+140)

#define ActReqRemOldUsr		(ActChgNumRowFooGrp+141)
#define ActRemOldUsr		(ActChgNumRowFooGrp+142)

#define ActLstClk		(ActChgNumRowFooGrp+143)

/*****************************************************************************/
/******************************** Social tab *********************************/
/*****************************************************************************/
// Actions in menu
#define ActSeeSocAct		(ActLstClk+  1)
#define ActReqPubPrf		(ActLstClk+  2)
#define ActSeeFor		(ActLstClk+  3)
#define ActSeeChtRms		(ActLstClk+  4)
// Secondary actions
#define ActReqSocPst		(ActLstClk+  5)
#define ActRcvSocPst		(ActLstClk+  6)
#define ActReqRemSocPub		(ActLstClk+  7)
#define ActRemSocPub		(ActLstClk+  8)

#define ActSeePubPrf		(ActLstClk+  9)
#define ActCal1stClkTim		(ActLstClk+ 10)
#define ActCalNumClk		(ActLstClk+ 11)
#define ActCalNumFilVie		(ActLstClk+ 12)
#define ActCalNumForPst		(ActLstClk+ 13)
#define ActCalNumMsgSnt		(ActLstClk+ 14)

#define ActFolUsr		(ActLstClk+ 15)
#define ActUnfUsr		(ActLstClk+ 16)
#define ActSeeFlg		(ActLstClk+ 17)
#define ActSeeFlr		(ActLstClk+ 18)

#define ActSeeForCrsUsr		(ActLstClk+ 19)
#define ActSeeForCrsTch		(ActLstClk+ 20)
#define ActSeeForDegUsr		(ActLstClk+ 21)
#define ActSeeForDegTch		(ActLstClk+ 22)
#define ActSeeForCtrUsr		(ActLstClk+ 23)
#define ActSeeForCtrTch		(ActLstClk+ 24)
#define ActSeeForInsUsr		(ActLstClk+ 25)
#define ActSeeForInsTch		(ActLstClk+ 26)
#define ActSeeForGenUsr		(ActLstClk+ 27)
#define ActSeeForGenTch		(ActLstClk+ 28)
#define ActSeeForSWAUsr		(ActLstClk+ 29)
#define ActSeeForSWATch		(ActLstClk+ 30)
#define ActSeePstForCrsUsr	(ActLstClk+ 31)
#define ActSeePstForCrsTch	(ActLstClk+ 32)
#define ActSeePstForDegUsr	(ActLstClk+ 33)
#define ActSeePstForDegTch	(ActLstClk+ 34)
#define ActSeePstForCtrUsr	(ActLstClk+ 35)
#define ActSeePstForCtrTch	(ActLstClk+ 36)
#define ActSeePstForInsUsr	(ActLstClk+ 37)
#define ActSeePstForInsTch	(ActLstClk+ 38)
#define ActSeePstForGenUsr	(ActLstClk+ 39)
#define ActSeePstForGenTch	(ActLstClk+ 40)
#define ActSeePstForSWAUsr	(ActLstClk+ 41)
#define ActSeePstForSWATch	(ActLstClk+ 42)
#define ActRcvThrForCrsUsr	(ActLstClk+ 43)
#define ActRcvThrForCrsTch	(ActLstClk+ 44)
#define ActRcvThrForDegUsr	(ActLstClk+ 45)
#define ActRcvThrForDegTch	(ActLstClk+ 46)
#define ActRcvThrForCtrUsr	(ActLstClk+ 47)
#define ActRcvThrForCtrTch	(ActLstClk+ 48)
#define ActRcvThrForInsUsr	(ActLstClk+ 49)
#define ActRcvThrForInsTch	(ActLstClk+ 50)
#define ActRcvThrForGenUsr	(ActLstClk+ 51)
#define ActRcvThrForGenTch	(ActLstClk+ 52)
#define ActRcvThrForSWAUsr	(ActLstClk+ 53)
#define ActRcvThrForSWATch	(ActLstClk+ 54)
#define ActRcvRepForCrsUsr	(ActLstClk+ 55)
#define ActRcvRepForCrsTch	(ActLstClk+ 56)
#define ActRcvRepForDegUsr	(ActLstClk+ 57)
#define ActRcvRepForDegTch	(ActLstClk+ 58)
#define ActRcvRepForCtrUsr	(ActLstClk+ 59)
#define ActRcvRepForCtrTch	(ActLstClk+ 60)
#define ActRcvRepForInsUsr	(ActLstClk+ 61)
#define ActRcvRepForInsTch	(ActLstClk+ 62)
#define ActRcvRepForGenUsr	(ActLstClk+ 63)
#define ActRcvRepForGenTch	(ActLstClk+ 64)
#define ActRcvRepForSWAUsr	(ActLstClk+ 65)
#define ActRcvRepForSWATch	(ActLstClk+ 66)
#define ActReqDelThrCrsUsr	(ActLstClk+ 67)
#define ActReqDelThrCrsTch	(ActLstClk+ 68)
#define ActReqDelThrDegUsr	(ActLstClk+ 69)
#define ActReqDelThrDegTch	(ActLstClk+ 70)
#define ActReqDelThrCtrUsr	(ActLstClk+ 71)
#define ActReqDelThrCtrTch	(ActLstClk+ 72)
#define ActReqDelThrInsUsr	(ActLstClk+ 73)
#define ActReqDelThrInsTch	(ActLstClk+ 74)
#define ActReqDelThrGenUsr	(ActLstClk+ 75)
#define ActReqDelThrGenTch	(ActLstClk+ 76)
#define ActReqDelThrSWAUsr	(ActLstClk+ 77)
#define ActReqDelThrSWATch	(ActLstClk+ 78)
#define ActDelThrForCrsUsr	(ActLstClk+ 79)
#define ActDelThrForCrsTch	(ActLstClk+ 80)
#define ActDelThrForDegUsr	(ActLstClk+ 81)
#define ActDelThrForDegTch	(ActLstClk+ 82)
#define ActDelThrForCtrUsr	(ActLstClk+ 83)
#define ActDelThrForCtrTch	(ActLstClk+ 84)
#define ActDelThrForInsUsr	(ActLstClk+ 85)
#define ActDelThrForInsTch	(ActLstClk+ 86)
#define ActDelThrForGenUsr	(ActLstClk+ 87)
#define ActDelThrForGenTch	(ActLstClk+ 88)
#define ActDelThrForSWAUsr	(ActLstClk+ 89)
#define ActDelThrForSWATch	(ActLstClk+ 90)
#define ActCutThrForCrsUsr	(ActLstClk+ 91)
#define ActCutThrForCrsTch	(ActLstClk+ 92)
#define ActCutThrForDegUsr	(ActLstClk+ 93)
#define ActCutThrForDegTch	(ActLstClk+ 94)
#define ActCutThrForCtrUsr	(ActLstClk+ 95)
#define ActCutThrForCtrTch	(ActLstClk+ 96)
#define ActCutThrForInsUsr	(ActLstClk+ 97)
#define ActCutThrForInsTch	(ActLstClk+ 98)
#define ActCutThrForGenUsr	(ActLstClk+ 99)
#define ActCutThrForGenTch	(ActLstClk+100)
#define ActCutThrForSWAUsr	(ActLstClk+101)
#define ActCutThrForSWATch	(ActLstClk+102)
#define ActPasThrForCrsUsr	(ActLstClk+103)
#define ActPasThrForCrsTch	(ActLstClk+104)
#define ActPasThrForDegUsr	(ActLstClk+105)
#define ActPasThrForDegTch	(ActLstClk+106)
#define ActPasThrForCtrUsr	(ActLstClk+107)
#define ActPasThrForCtrTch	(ActLstClk+108)
#define ActPasThrForInsUsr	(ActLstClk+109)
#define ActPasThrForInsTch	(ActLstClk+110)
#define ActPasThrForGenUsr	(ActLstClk+111)
#define ActPasThrForGenTch	(ActLstClk+112)
#define ActPasThrForSWAUsr	(ActLstClk+113)
#define ActPasThrForSWATch	(ActLstClk+114)
#define ActDelPstForCrsUsr	(ActLstClk+115)
#define ActDelPstForCrsTch	(ActLstClk+116)
#define ActDelPstForDegUsr	(ActLstClk+117)
#define ActDelPstForDegTch	(ActLstClk+118)
#define ActDelPstForCtrUsr	(ActLstClk+119)
#define ActDelPstForCtrTch	(ActLstClk+120)
#define ActDelPstForInsUsr	(ActLstClk+121)
#define ActDelPstForInsTch	(ActLstClk+122)
#define ActDelPstForGenUsr	(ActLstClk+123)
#define ActDelPstForGenTch	(ActLstClk+124)
#define ActDelPstForSWAUsr	(ActLstClk+125)
#define ActDelPstForSWATch	(ActLstClk+126)
#define ActEnbPstForCrsUsr	(ActLstClk+127)
#define ActEnbPstForCrsTch	(ActLstClk+128)
#define ActEnbPstForDegUsr	(ActLstClk+129)
#define ActEnbPstForDegTch	(ActLstClk+130)
#define ActEnbPstForCtrUsr	(ActLstClk+131)
#define ActEnbPstForCtrTch	(ActLstClk+132)
#define ActEnbPstForInsUsr	(ActLstClk+133)
#define ActEnbPstForInsTch	(ActLstClk+134)
#define ActEnbPstForGenUsr	(ActLstClk+135)
#define ActEnbPstForGenTch	(ActLstClk+136)
#define ActEnbPstForSWAUsr	(ActLstClk+137)
#define ActEnbPstForSWATch	(ActLstClk+138)
#define ActDisPstForCrsUsr	(ActLstClk+139)
#define ActDisPstForCrsTch	(ActLstClk+140)
#define ActDisPstForDegUsr	(ActLstClk+141)
#define ActDisPstForDegTch	(ActLstClk+142)
#define ActDisPstForCtrUsr	(ActLstClk+143)
#define ActDisPstForCtrTch	(ActLstClk+144)
#define ActDisPstForInsUsr	(ActLstClk+145)
#define ActDisPstForInsTch	(ActLstClk+146)
#define ActDisPstForGenUsr	(ActLstClk+147)
#define ActDisPstForGenTch	(ActLstClk+148)
#define ActDisPstForSWAUsr	(ActLstClk+149)
#define ActDisPstForSWATch	(ActLstClk+150)

#define ActCht			(ActLstClk+151)

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
#define ActRemSvyQst		(ActUnbUsrLst+ 20)

#define ActSeeUseGbl		(ActUnbUsrLst+ 21)
#define ActPrnPhoDeg		(ActUnbUsrLst+ 22)
#define ActCalPhoDeg		(ActUnbUsrLst+ 23)
#define ActSeeAccGbl		(ActUnbUsrLst+ 24)
#define ActReqAccCrs		(ActUnbUsrLst+ 25)
#define ActSeeAccCrs		(ActUnbUsrLst+ 26)
#define ActSeeAllStaCrs		(ActUnbUsrLst+ 27)

/*****************************************************************************/
/******************************** Profile tab ********************************/
/*****************************************************************************/
// Actions in menu
#define ActFrmLogIn		(ActSeeAllStaCrs+ 1)
#define ActMyCrs		(ActSeeAllStaCrs+ 2)
#define ActSeeMyTT		(ActSeeAllStaCrs+ 3)
#define ActFrmUsrAcc		(ActSeeAllStaCrs+ 4)
#define ActReqEdiRecCom		(ActSeeAllStaCrs+ 5)
#define ActEdiPrf		(ActSeeAllStaCrs+ 6)
#define ActAdmBrf		(ActSeeAllStaCrs+ 7)
#define ActMFUAct		(ActSeeAllStaCrs+ 8)
// Secondary actions
#define ActAutUsrInt		(ActSeeAllStaCrs+ 9)
#define ActAutUsrExt		(ActSeeAllStaCrs+10)
#define ActAutUsrChgLan		(ActSeeAllStaCrs+11)
#define ActLogOut		(ActSeeAllStaCrs+12)
#define ActAnnSee		(ActSeeAllStaCrs+13)
#define ActReqSndNewPwd		(ActSeeAllStaCrs+14)
#define ActSndNewPwd		(ActSeeAllStaCrs+15)
#define ActChgMyRol		(ActSeeAllStaCrs+16)
#define ActCreUsrAcc		(ActSeeAllStaCrs+17)
#define ActRemID_Me		(ActSeeAllStaCrs+18)
#define ActNewIDMe		(ActSeeAllStaCrs+19)
#define ActRemOldNic		(ActSeeAllStaCrs+20)
#define ActChgNic		(ActSeeAllStaCrs+21)
#define ActRemMaiMe		(ActSeeAllStaCrs+22)
#define ActNewMaiMe		(ActSeeAllStaCrs+23)
#define ActCnfMai		(ActSeeAllStaCrs+24)
#define ActFrmChgMyPwd		(ActSeeAllStaCrs+25)
#define ActChgPwd		(ActSeeAllStaCrs+26)
#define ActReqRemMyAcc		(ActSeeAllStaCrs+27)
#define ActRemMyAcc		(ActSeeAllStaCrs+28)

#define ActChgMyData		(ActSeeAllStaCrs+29)

#define ActReqMyPho		(ActSeeAllStaCrs+30)
#define ActDetMyPho		(ActSeeAllStaCrs+31)
#define ActUpdMyPho		(ActSeeAllStaCrs+32)
#define ActRemMyPho		(ActSeeAllStaCrs+33)

#define ActEdiPri		(ActSeeAllStaCrs+34)
#define ActChgPriPho		(ActSeeAllStaCrs+35)
#define ActChgPriPrf		(ActSeeAllStaCrs+36)

#define ActReqEdiMyIns		(ActSeeAllStaCrs+37)
#define ActChgCtyMyIns		(ActSeeAllStaCrs+38)
#define ActChgMyIns		(ActSeeAllStaCrs+39)
#define ActChgMyCtr		(ActSeeAllStaCrs+40)
#define ActChgMyDpt		(ActSeeAllStaCrs+41)
#define ActChgMyOff		(ActSeeAllStaCrs+42)
#define ActChgMyOffPho		(ActSeeAllStaCrs+43)

#define ActReqEdiMyNet		(ActSeeAllStaCrs+44)
#define ActChgMyNet		(ActSeeAllStaCrs+45)

#define ActChgThe		(ActSeeAllStaCrs+46)
#define ActReqChgLan		(ActSeeAllStaCrs+47)
#define ActChgLan		(ActSeeAllStaCrs+48)
#define ActChg1stDay		(ActSeeAllStaCrs+49)
#define ActChgCol		(ActSeeAllStaCrs+50)
#define ActHidLftCol		(ActSeeAllStaCrs+51)
#define ActHidRgtCol		(ActSeeAllStaCrs+52)
#define ActShoLftCol		(ActSeeAllStaCrs+53)
#define ActShoRgtCol		(ActSeeAllStaCrs+54)
#define ActChgIco		(ActSeeAllStaCrs+55)
#define ActChgMnu		(ActSeeAllStaCrs+56)
#define ActChgNtfPrf		(ActSeeAllStaCrs+57)

#define ActPrnUsrQR		(ActSeeAllStaCrs+58)

#define ActPrnMyTT		(ActSeeAllStaCrs+59)
#define ActEdiTut		(ActSeeAllStaCrs+60)
#define ActChgTut		(ActSeeAllStaCrs+61)
#define ActChgMyTT1stDay	(ActSeeAllStaCrs+62)

#define ActReqRemFilBrf		(ActSeeAllStaCrs+63)
#define ActRemFilBrf		(ActSeeAllStaCrs+64)
#define ActRemFolBrf		(ActSeeAllStaCrs+65)
#define ActCopBrf		(ActSeeAllStaCrs+66)
#define ActPasBrf		(ActSeeAllStaCrs+67)
#define ActRemTreBrf		(ActSeeAllStaCrs+68)
#define ActFrmCreBrf		(ActSeeAllStaCrs+69)
#define ActCreFolBrf		(ActSeeAllStaCrs+70)
#define ActCreLnkBrf		(ActSeeAllStaCrs+71)
#define ActRenFolBrf		(ActSeeAllStaCrs+72)
#define ActRcvFilBrfDZ		(ActSeeAllStaCrs+73)
#define ActRcvFilBrfCla		(ActSeeAllStaCrs+74)
#define ActExpBrf		(ActSeeAllStaCrs+75)
#define ActConBrf		(ActSeeAllStaCrs+76)
#define ActZIPBrf		(ActSeeAllStaCrs+77)
#define ActReqDatBrf		(ActSeeAllStaCrs+78)
#define ActChgDatBrf		(ActSeeAllStaCrs+79)
#define ActDowBrf		(ActSeeAllStaCrs+80)

#define ActAskRemOldBrf		(ActSeeAllStaCrs+81)
#define ActRemOldBrf		(ActSeeAllStaCrs+82)

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

void Act_FormStart (Act_Action_t NextAction);
void Act_FormGoToStart (Act_Action_t NextAction);
void Act_FormStartAnchor (Act_Action_t NextAction,const char *Anchor);
void Act_FormStartId (Act_Action_t NextAction,const char *Id);
void Act_SetParamsForm (char *Params,Act_Action_t NextAction,bool PutParameterLocationIfNoSesion);
void Act_FormEnd (void);
void Act_LinkFormSubmit (const char *Title,const char *LinkStyle);
void Act_LinkFormSubmitId (const char *Title,const char *LinkStyle,const char *Id);
void Act_LinkFormSubmitAnimated (const char *Title,const char *LinkStyle);
void Act_PutLinkToUpdateAction (Act_Action_t Action);

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
