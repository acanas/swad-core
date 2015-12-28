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

#define Act_NUM_ACTIONS	(7+52+15+90+72+67+205+183+143+14+169+27+82)

#define Act_MAX_ACTION_COD 1490

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

#define ActReqRemFilComIns	(ActChgInsSta+ 73)
#define ActRemFilComIns		(ActChgInsSta+ 74)
#define ActRemFolComIns		(ActChgInsSta+ 75)
#define ActCopComIns		(ActChgInsSta+ 76)
#define ActPasComIns		(ActChgInsSta+ 77)
#define ActRemTreComIns		(ActChgInsSta+ 78)
#define ActFrmCreComIns		(ActChgInsSta+ 79)
#define ActCreFolComIns		(ActChgInsSta+ 80)
#define ActCreLnkComIns		(ActChgInsSta+ 81)
#define ActRenFolComIns		(ActChgInsSta+ 82)
#define ActRcvFilComInsDZ	(ActChgInsSta+ 83)
#define ActRcvFilComInsCla	(ActChgInsSta+ 84)
#define ActExpComIns		(ActChgInsSta+ 85)
#define ActConComIns		(ActChgInsSta+ 86)
#define ActZIPComIns		(ActChgInsSta+ 87)
#define ActReqDatComIns		(ActChgInsSta+ 88)
#define ActChgDatComIns		(ActChgInsSta+ 89)
#define ActDowComIns		(ActChgInsSta+ 90)

/*****************************************************************************/
/********************************* Centre tab ********************************/
/*****************************************************************************/
// Actions in menu
#define ActCtrReqSch		(ActDowComIns+  1)
#define ActSeeCtrInf		(ActDowComIns+  2)
#define ActSeeDeg		(ActDowComIns+  3)
#define ActSeeAdmDocCtr		(ActDowComIns+  4)
#define ActAdmComCtr		(ActDowComIns+  5)

// Secondary actions
#define ActCtrSch		(ActDowComIns+  6)
#define ActPrnCtrInf		(ActDowComIns+  7)
#define ActReqCtrLog		(ActDowComIns+  8)
#define ActRecCtrLog		(ActDowComIns+  9)
#define ActRemCtrLog		(ActDowComIns+ 10)
#define ActReqCtrPho		(ActDowComIns+ 11)
#define ActRecCtrPho		(ActDowComIns+ 12)
#define ActChgCtrPhoAtt		(ActDowComIns+ 13)

#define ActEdiDeg		(ActDowComIns+ 14)
#define ActReqDeg		(ActDowComIns+ 15)
#define ActNewDeg		(ActDowComIns+ 16)
#define ActRemDeg		(ActDowComIns+ 17)
#define ActRenDegSho		(ActDowComIns+ 18)
#define ActRenDegFul		(ActDowComIns+ 19)
#define ActChgDegTyp		(ActDowComIns+ 20)
#define ActChgDegCtr		(ActDowComIns+ 21)
#define ActChgDegFstYea		(ActDowComIns+ 22)
#define ActChgDegLstYea		(ActDowComIns+ 23)
// #define ActChgDegOptYea		(ActDowComIns+ 24)
#define ActChgDegWWW		(ActDowComIns+ 24)
#define ActChgDegSta		(ActDowComIns+ 25)

#define ActChgToSeeDocCtr	(ActDowComIns+ 26)
#define ActSeeDocCtr		(ActDowComIns+ 27)
#define ActExpSeeDocCtr		(ActDowComIns+ 28)
#define ActConSeeDocCtr		(ActDowComIns+ 29)
#define ActZIPSeeDocCtr		(ActDowComIns+ 30)
#define ActReqDatSeeDocCtr	(ActDowComIns+ 31)
#define ActDowSeeDocCtr		(ActDowComIns+ 32)
#define ActChgToAdmDocCtr	(ActDowComIns+ 33)
#define ActAdmDocCtr		(ActDowComIns+ 34)
#define ActReqRemFilDocCtr	(ActDowComIns+ 35)
#define ActRemFilDocCtr		(ActDowComIns+ 36)
#define ActRemFolDocCtr		(ActDowComIns+ 37)
#define ActCopDocCtr		(ActDowComIns+ 38)
#define ActPasDocCtr		(ActDowComIns+ 39)
#define ActRemTreDocCtr		(ActDowComIns+ 40)
#define ActFrmCreDocCtr		(ActDowComIns+ 41)
#define ActCreFolDocCtr		(ActDowComIns+ 42)
#define ActCreLnkDocCtr		(ActDowComIns+ 43)
#define ActRenFolDocCtr		(ActDowComIns+ 44)
#define ActRcvFilDocCtrDZ	(ActDowComIns+ 45)
#define ActRcvFilDocCtrCla	(ActDowComIns+ 46)
#define ActExpAdmDocCtr		(ActDowComIns+ 47)
#define ActConAdmDocCtr		(ActDowComIns+ 48)
#define ActZIPAdmDocCtr		(ActDowComIns+ 49)
#define ActShoDocCtr		(ActDowComIns+ 50)
#define ActHidDocCtr		(ActDowComIns+ 51)
#define ActReqDatAdmDocCtr	(ActDowComIns+ 52)
#define ActChgDatAdmDocCtr	(ActDowComIns+ 53)
#define ActDowAdmDocCtr		(ActDowComIns+ 54)

#define ActReqRemFilComCtr	(ActDowComIns+ 55)
#define ActRemFilComCtr		(ActDowComIns+ 56)
#define ActRemFolComCtr		(ActDowComIns+ 57)
#define ActCopComCtr		(ActDowComIns+ 58)
#define ActPasComCtr		(ActDowComIns+ 59)
#define ActRemTreComCtr		(ActDowComIns+ 60)
#define ActFrmCreComCtr		(ActDowComIns+ 61)
#define ActCreFolComCtr		(ActDowComIns+ 62)
#define ActCreLnkComCtr		(ActDowComIns+ 63)
#define ActRenFolComCtr		(ActDowComIns+ 64)
#define ActRcvFilComCtrDZ	(ActDowComIns+ 65)
#define ActRcvFilComCtrCla	(ActDowComIns+ 66)
#define ActExpComCtr		(ActDowComIns+ 67)
#define ActConComCtr		(ActDowComIns+ 68)
#define ActZIPComCtr		(ActDowComIns+ 69)
#define ActReqDatComCtr		(ActDowComIns+ 70)
#define ActChgDatComCtr		(ActDowComIns+ 71)
#define ActDowComCtr		(ActDowComIns+ 72)

/*****************************************************************************/
/********************************* Degree tab ********************************/
/*****************************************************************************/
// Actions in menu
#define ActDegReqSch		(ActDowComCtr+  1)
#define ActSeeDegInf		(ActDowComCtr+  2)
#define ActSeeCrs		(ActDowComCtr+  3)
#define ActSeeAdmDocDeg		(ActDowComCtr+  4)
#define ActAdmComDeg		(ActDowComCtr+  5)

// Secondary actions
#define ActDegSch		(ActDowComCtr+  6)
#define ActPrnDegInf		(ActDowComCtr+  7)
#define ActReqDegLog		(ActDowComCtr+  8)
#define ActRecDegLog		(ActDowComCtr+  9)
#define ActRemDegLog		(ActDowComCtr+ 10)

#define ActEdiCrs		(ActDowComCtr+ 11)
#define ActReqCrs		(ActDowComCtr+ 12)
#define ActNewCrs		(ActDowComCtr+ 13)
#define ActRemCrs		(ActDowComCtr+ 14)
#define ActChgInsCrsCod		(ActDowComCtr+ 15)
#define ActChgCrsDeg		(ActDowComCtr+ 16)
#define ActChgCrsYea		(ActDowComCtr+ 17)
#define ActRenCrsSho		(ActDowComCtr+ 18)
#define ActRenCrsFul		(ActDowComCtr+ 19)
#define ActChgCrsSta		(ActDowComCtr+ 20)

#define ActChgToSeeDocDeg	(ActDowComCtr+ 21)
#define ActSeeDocDeg		(ActDowComCtr+ 22)
#define ActExpSeeDocDeg		(ActDowComCtr+ 23)
#define ActConSeeDocDeg		(ActDowComCtr+ 24)
#define ActZIPSeeDocDeg		(ActDowComCtr+ 25)
#define ActReqDatSeeDocDeg	(ActDowComCtr+ 26)
#define ActDowSeeDocDeg		(ActDowComCtr+ 27)
#define ActChgToAdmDocDeg	(ActDowComCtr+ 28)
#define ActAdmDocDeg		(ActDowComCtr+ 29)
#define ActReqRemFilDocDeg	(ActDowComCtr+ 30)
#define ActRemFilDocDeg		(ActDowComCtr+ 31)
#define ActRemFolDocDeg		(ActDowComCtr+ 32)
#define ActCopDocDeg		(ActDowComCtr+ 33)
#define ActPasDocDeg		(ActDowComCtr+ 34)
#define ActRemTreDocDeg		(ActDowComCtr+ 35)
#define ActFrmCreDocDeg		(ActDowComCtr+ 36)
#define ActCreFolDocDeg		(ActDowComCtr+ 37)
#define ActCreLnkDocDeg		(ActDowComCtr+ 38)
#define ActRenFolDocDeg		(ActDowComCtr+ 39)
#define ActRcvFilDocDegDZ	(ActDowComCtr+ 40)
#define ActRcvFilDocDegCla	(ActDowComCtr+ 41)
#define ActExpAdmDocDeg		(ActDowComCtr+ 42)
#define ActConAdmDocDeg		(ActDowComCtr+ 43)
#define ActZIPAdmDocDeg		(ActDowComCtr+ 44)
#define ActShoDocDeg		(ActDowComCtr+ 45)
#define ActHidDocDeg		(ActDowComCtr+ 46)
#define ActReqDatAdmDocDeg	(ActDowComCtr+ 47)
#define ActChgDatAdmDocDeg	(ActDowComCtr+ 48)
#define ActDowAdmDocDeg		(ActDowComCtr+ 49)

#define ActReqRemFilComDeg	(ActDowComCtr+ 50)
#define ActRemFilComDeg		(ActDowComCtr+ 51)
#define ActRemFolComDeg		(ActDowComCtr+ 52)
#define ActCopComDeg		(ActDowComCtr+ 53)
#define ActPasComDeg		(ActDowComCtr+ 54)
#define ActRemTreComDeg		(ActDowComCtr+ 55)
#define ActFrmCreComDeg		(ActDowComCtr+ 56)
#define ActCreFolComDeg		(ActDowComCtr+ 57)
#define ActCreLnkComDeg		(ActDowComCtr+ 58)
#define ActRenFolComDeg		(ActDowComCtr+ 59)
#define ActRcvFilComDegDZ	(ActDowComCtr+ 60)
#define ActRcvFilComDegCla	(ActDowComCtr+ 61)
#define ActExpComDeg		(ActDowComCtr+ 62)
#define ActConComDeg		(ActDowComCtr+ 63)
#define ActZIPComDeg		(ActDowComCtr+ 64)
#define ActReqDatComDeg		(ActDowComCtr+ 65)
#define ActChgDatComDeg		(ActDowComCtr+ 66)
#define ActDowComDeg		(ActDowComCtr+ 67)

/*****************************************************************************/
/******************************** Course tab *********************************/
/*****************************************************************************/
// Actions in menu
#define ActCrsReqSch		(ActDowComDeg+  1)
#define ActSeeCrsInf		(ActDowComDeg+  2)
#define ActSeeTchGui		(ActDowComDeg+  3)
#define ActSeeSyl		(ActDowComDeg+  4)
#define ActSeeAdmDocCrs		(ActDowComDeg+  5)
#define ActAdmCom		(ActDowComDeg+  6)
#define ActSeeCrsTT		(ActDowComDeg+  7)
#define ActSeeBib		(ActDowComDeg+  8)
#define ActSeeFAQ		(ActDowComDeg+  9)
#define ActSeeCrsLnk		(ActDowComDeg+ 10)
// Secondary actions
#define ActCrsSch		(ActDowComDeg+ 11)
#define ActPrnCrsInf		(ActDowComDeg+ 12)
#define ActChgCrsLog		(ActDowComDeg+ 13)
#define ActEdiCrsInf		(ActDowComDeg+ 14)
#define ActEdiTchGui		(ActDowComDeg+ 15)
#define ActPrnCrsTT		(ActDowComDeg+ 16)
#define ActEdiCrsTT		(ActDowComDeg+ 17)
#define ActChgCrsTT		(ActDowComDeg+ 18)
#define ActChgCrsTT1stDay	(ActDowComDeg+ 19)
#define ActSeeSylLec		(ActDowComDeg+ 20)
#define ActSeeSylPra		(ActDowComDeg+ 21)
#define ActEdiSylLec		(ActDowComDeg+ 22)
#define ActEdiSylPra		(ActDowComDeg+ 23)
#define ActDelItmSylLec		(ActDowComDeg+ 24)
#define ActDelItmSylPra		(ActDowComDeg+ 25)
#define ActUp_IteSylLec		(ActDowComDeg+ 26)
#define ActUp_IteSylPra		(ActDowComDeg+ 27)
#define ActDwnIteSylLec		(ActDowComDeg+ 28)
#define ActDwnIteSylPra		(ActDowComDeg+ 29)
#define ActRgtIteSylLec		(ActDowComDeg+ 30)
#define ActRgtIteSylPra		(ActDowComDeg+ 31)
#define ActLftIteSylLec		(ActDowComDeg+ 32)
#define ActLftIteSylPra		(ActDowComDeg+ 33)
#define ActInsIteSylLec		(ActDowComDeg+ 34)
#define ActInsIteSylPra		(ActDowComDeg+ 35)
#define ActModIteSylLec		(ActDowComDeg+ 36)
#define ActModIteSylPra		(ActDowComDeg+ 37)

#define ActChgToSeeDocCrs	(ActDowComDeg+ 38)
#define ActSeeDocCrs		(ActDowComDeg+ 39)
#define ActExpSeeDocCrs		(ActDowComDeg+ 40)
#define ActConSeeDocCrs		(ActDowComDeg+ 41)
#define ActZIPSeeDocCrs		(ActDowComDeg+ 42)
#define ActReqDatSeeDocCrs	(ActDowComDeg+ 43)
#define ActDowSeeDocCrs		(ActDowComDeg+ 44)

#define ActSeeDocGrp		(ActDowComDeg+ 45)
#define ActExpSeeDocGrp		(ActDowComDeg+ 46)
#define ActConSeeDocGrp		(ActDowComDeg+ 47)
#define ActZIPSeeDocGrp		(ActDowComDeg+ 48)
#define ActReqDatSeeDocGrp	(ActDowComDeg+ 49)
#define ActDowSeeDocGrp		(ActDowComDeg+ 50)

#define ActChgToAdmDocCrs	(ActDowComDeg+ 51)
#define ActAdmDocCrs		(ActDowComDeg+ 52)
#define ActReqRemFilDocCrs	(ActDowComDeg+ 53)
#define ActRemFilDocCrs		(ActDowComDeg+ 54)
#define ActRemFolDocCrs		(ActDowComDeg+ 55)
#define ActCopDocCrs		(ActDowComDeg+ 56)
#define ActPasDocCrs		(ActDowComDeg+ 57)
#define ActRemTreDocCrs		(ActDowComDeg+ 58)
#define ActFrmCreDocCrs		(ActDowComDeg+ 59)
#define ActCreFolDocCrs		(ActDowComDeg+ 60)
#define ActCreLnkDocCrs		(ActDowComDeg+ 61)
#define ActRenFolDocCrs		(ActDowComDeg+ 62)
#define ActRcvFilDocCrsDZ	(ActDowComDeg+ 63)
#define ActRcvFilDocCrsCla	(ActDowComDeg+ 64)
#define ActExpAdmDocCrs		(ActDowComDeg+ 65)
#define ActConAdmDocCrs		(ActDowComDeg+ 66)
#define ActZIPAdmDocCrs		(ActDowComDeg+ 67)
#define ActShoDocCrs		(ActDowComDeg+ 68)
#define ActHidDocCrs		(ActDowComDeg+ 69)
#define ActReqDatAdmDocCrs	(ActDowComDeg+ 70)
#define ActChgDatAdmDocCrs	(ActDowComDeg+ 71)
#define ActDowAdmDocCrs		(ActDowComDeg+ 72)

#define ActAdmDocGrp		(ActDowComDeg+ 73)
#define ActReqRemFilDocGrp	(ActDowComDeg+ 74)
#define ActRemFilDocGrp		(ActDowComDeg+ 75)
#define ActRemFolDocGrp		(ActDowComDeg+ 76)
#define ActCopDocGrp		(ActDowComDeg+ 77)
#define ActPasDocGrp		(ActDowComDeg+ 78)
#define ActRemTreDocGrp		(ActDowComDeg+ 79)
#define ActFrmCreDocGrp		(ActDowComDeg+ 80)
#define ActCreFolDocGrp		(ActDowComDeg+ 81)
#define ActCreLnkDocGrp		(ActDowComDeg+ 82)
#define ActRenFolDocGrp		(ActDowComDeg+ 83)
#define ActRcvFilDocGrpDZ	(ActDowComDeg+ 84)
#define ActRcvFilDocGrpCla	(ActDowComDeg+ 85)
#define ActExpAdmDocGrp		(ActDowComDeg+ 86)
#define ActConAdmDocGrp		(ActDowComDeg+ 87)
#define ActZIPAdmDocGrp		(ActDowComDeg+ 88)
#define ActShoDocGrp		(ActDowComDeg+ 89)
#define ActHidDocGrp		(ActDowComDeg+ 90)
#define ActReqDatAdmDocGrp	(ActDowComDeg+ 91)
#define ActChgDatAdmDocGrp	(ActDowComDeg+ 92)
#define ActDowAdmDocGrp		(ActDowComDeg+ 93)

#define ActChgToAdmCom		(ActDowComDeg+ 94)

#define ActAdmComCrs		(ActDowComDeg+ 95)
#define ActReqRemFilComCrs	(ActDowComDeg+ 96)
#define ActRemFilComCrs		(ActDowComDeg+ 97)
#define ActRemFolComCrs		(ActDowComDeg+ 98)
#define ActCopComCrs		(ActDowComDeg+ 99)
#define ActPasComCrs		(ActDowComDeg+100)
#define ActRemTreComCrs		(ActDowComDeg+101)
#define ActFrmCreComCrs		(ActDowComDeg+102)
#define ActCreFolComCrs		(ActDowComDeg+103)
#define ActCreLnkComCrs		(ActDowComDeg+104)
#define ActRenFolComCrs		(ActDowComDeg+105)
#define ActRcvFilComCrsDZ	(ActDowComDeg+106)
#define ActRcvFilComCrsCla	(ActDowComDeg+107)
#define ActExpComCrs		(ActDowComDeg+108)
#define ActConComCrs		(ActDowComDeg+109)
#define ActZIPComCrs		(ActDowComDeg+110)
#define ActReqDatComCrs		(ActDowComDeg+111)
#define ActChgDatComCrs		(ActDowComDeg+112)
#define ActDowComCrs		(ActDowComDeg+113)

#define ActAdmComGrp		(ActDowComDeg+114)
#define ActReqRemFilComGrp	(ActDowComDeg+115)
#define ActRemFilComGrp		(ActDowComDeg+116)
#define ActRemFolComGrp		(ActDowComDeg+117)
#define ActCopComGrp		(ActDowComDeg+118)
#define ActPasComGrp		(ActDowComDeg+119)
#define ActRemTreComGrp		(ActDowComDeg+120)
#define ActFrmCreComGrp		(ActDowComDeg+121)
#define ActCreFolComGrp		(ActDowComDeg+122)
#define ActCreLnkComGrp		(ActDowComDeg+123)
#define ActRenFolComGrp		(ActDowComDeg+124)
#define ActRcvFilComGrpDZ	(ActDowComDeg+125)
#define ActRcvFilComGrpCla	(ActDowComDeg+126)
#define ActExpComGrp		(ActDowComDeg+127)
#define ActConComGrp		(ActDowComDeg+128)
#define ActZIPComGrp		(ActDowComDeg+129)
#define ActReqDatComGrp		(ActDowComDeg+130)
#define ActChgDatComGrp		(ActDowComDeg+131)
#define ActDowComGrp		(ActDowComDeg+132)

#define ActEdiBib		(ActDowComDeg+133)
#define ActEdiFAQ		(ActDowComDeg+134)
#define ActEdiCrsLnk		(ActDowComDeg+135)

#define ActChgFrcReaCrsInf	(ActDowComDeg+136)
#define ActChgFrcReaTchGui	(ActDowComDeg+137)
#define ActChgFrcReaSylLec	(ActDowComDeg+138)
#define ActChgFrcReaSylPra	(ActDowComDeg+139)
#define ActChgFrcReaBib		(ActDowComDeg+140)
#define ActChgFrcReaFAQ		(ActDowComDeg+141)
#define ActChgFrcReaCrsLnk	(ActDowComDeg+142)

#define ActChgHavReaCrsInf	(ActDowComDeg+143)
#define ActChgHavReaTchGui	(ActDowComDeg+144)
#define ActChgHavReaSylLec	(ActDowComDeg+145)
#define ActChgHavReaSylPra	(ActDowComDeg+146)
#define ActChgHavReaBib		(ActDowComDeg+147)
#define ActChgHavReaFAQ		(ActDowComDeg+148)
#define ActChgHavReaCrsLnk	(ActDowComDeg+149)

#define ActSelInfSrcCrsInf	(ActDowComDeg+150)
#define ActSelInfSrcTchGui	(ActDowComDeg+151)
#define ActSelInfSrcSylLec	(ActDowComDeg+152)
#define ActSelInfSrcSylPra	(ActDowComDeg+153)
#define ActSelInfSrcBib		(ActDowComDeg+154)
#define ActSelInfSrcFAQ		(ActDowComDeg+155)
#define ActSelInfSrcCrsLnk	(ActDowComDeg+156)
#define ActRcvURLCrsInf		(ActDowComDeg+157)
#define ActRcvURLTchGui		(ActDowComDeg+158)
#define ActRcvURLSylLec		(ActDowComDeg+159)
#define ActRcvURLSylPra		(ActDowComDeg+160)
#define ActRcvURLBib		(ActDowComDeg+161)
#define ActRcvURLFAQ		(ActDowComDeg+162)
#define ActRcvURLCrsLnk		(ActDowComDeg+163)
#define ActRcvPagCrsInf		(ActDowComDeg+164)
#define ActRcvPagTchGui		(ActDowComDeg+165)
#define ActRcvPagSylLec		(ActDowComDeg+166)
#define ActRcvPagSylPra		(ActDowComDeg+167)
#define ActRcvPagBib		(ActDowComDeg+168)
#define ActRcvPagFAQ		(ActDowComDeg+169)
#define ActRcvPagCrsLnk		(ActDowComDeg+170)
#define ActEditorCrsInf		(ActDowComDeg+171)
#define ActEditorTchGui		(ActDowComDeg+172)
#define ActEditorSylLec		(ActDowComDeg+173)
#define ActEditorSylPra		(ActDowComDeg+174)
#define ActEditorBib		(ActDowComDeg+175)
#define ActEditorFAQ		(ActDowComDeg+176)
#define ActEditorCrsLnk		(ActDowComDeg+177)
#define ActPlaTxtEdiCrsInf	(ActDowComDeg+178)
#define ActPlaTxtEdiTchGui	(ActDowComDeg+179)
#define ActPlaTxtEdiSylLec	(ActDowComDeg+180)
#define ActPlaTxtEdiSylPra	(ActDowComDeg+181)
#define ActPlaTxtEdiBib		(ActDowComDeg+182)
#define ActPlaTxtEdiFAQ		(ActDowComDeg+183)
#define ActPlaTxtEdiCrsLnk	(ActDowComDeg+184)
#define ActRchTxtEdiCrsInf	(ActDowComDeg+185)
#define ActRchTxtEdiTchGui	(ActDowComDeg+186)
#define ActRchTxtEdiSylLec	(ActDowComDeg+187)
#define ActRchTxtEdiSylPra	(ActDowComDeg+188)
#define ActRchTxtEdiBib		(ActDowComDeg+189)
#define ActRchTxtEdiFAQ		(ActDowComDeg+190)
#define ActRchTxtEdiCrsLnk	(ActDowComDeg+191)
#define ActRcvPlaTxtCrsInf	(ActDowComDeg+192)
#define ActRcvPlaTxtTchGui	(ActDowComDeg+193)
#define ActRcvPlaTxtSylLec	(ActDowComDeg+194)
#define ActRcvPlaTxtSylPra	(ActDowComDeg+195)
#define ActRcvPlaTxtBib		(ActDowComDeg+196)
#define ActRcvPlaTxtFAQ		(ActDowComDeg+197)
#define ActRcvPlaTxtCrsLnk	(ActDowComDeg+198)
#define ActRcvRchTxtCrsInf	(ActDowComDeg+199)
#define ActRcvRchTxtTchGui	(ActDowComDeg+200)
#define ActRcvRchTxtSylLec	(ActDowComDeg+201)
#define ActRcvRchTxtSylPra	(ActDowComDeg+202)
#define ActRcvRchTxtBib		(ActDowComDeg+203)
#define ActRcvRchTxtFAQ		(ActDowComDeg+204)
#define ActRcvRchTxtCrsLnk	(ActDowComDeg+205)

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
#define ActSeeChtRms		(ActLstClk+  3)
// Secondary actions
#define ActSeePubPrf		(ActLstClk+  4)
#define ActCal1stClkTim		(ActLstClk+  5)
#define ActCalNumClk		(ActLstClk+  6)
#define ActCalNumFilVie		(ActLstClk+  7)
#define ActCalNumForPst		(ActLstClk+  8)
#define ActCalNumMsgSnt		(ActLstClk+  9)

#define ActFolUsr		(ActLstClk+ 10)
#define ActUnfUsr		(ActLstClk+ 11)
#define ActSeeFlg		(ActLstClk+ 12)
#define ActSeeFlr		(ActLstClk+ 13)

#define ActCht			(ActLstClk+ 14)

/*****************************************************************************/
/******************************* Messages tab ********************************/
/*****************************************************************************/
// Actions in menu
#define ActSeeNtf		(ActCht+  1)
#define ActSeeAnn		(ActCht+  2)
#define ActSeeNot		(ActCht+  3)
#define ActSeeFor		(ActCht+  4)
#define ActReqMsgUsr		(ActCht+  5)
#define ActSeeRcvMsg		(ActCht+  6)
#define ActSeeSntMsg		(ActCht+  7)
#define ActMaiStd		(ActCht+  8)
// Secondary actions
#define ActWriAnn		(ActCht+  9)
#define ActRcvAnn		(ActCht+ 10)
#define ActHidAnn		(ActCht+ 11)
#define ActRevAnn		(ActCht+ 12)
#define ActRemAnn		(ActCht+ 13)
#define ActShoNot		(ActCht+ 14)
#define ActWriNot		(ActCht+ 15)
#define ActRcvNot		(ActCht+ 16)
#define ActHidNot		(ActCht+ 17)
#define ActRevNot		(ActCht+ 18)
#define ActReqRemNot		(ActCht+ 19)
#define ActRemNot		(ActCht+ 20)
#define ActSeeNewNtf		(ActCht+ 21)
#define ActMrkNtfSee		(ActCht+ 22)
#define ActSeeForCrsUsr		(ActCht+ 23)
#define ActSeeForCrsTch		(ActCht+ 24)
#define ActSeeForDegUsr		(ActCht+ 25)
#define ActSeeForDegTch		(ActCht+ 26)
#define ActSeeForCtrUsr		(ActCht+ 27)
#define ActSeeForCtrTch		(ActCht+ 28)
#define ActSeeForInsUsr		(ActCht+ 29)
#define ActSeeForInsTch		(ActCht+ 30)
#define ActSeeForGenUsr		(ActCht+ 31)
#define ActSeeForGenTch		(ActCht+ 32)
#define ActSeeForSWAUsr		(ActCht+ 33)
#define ActSeeForSWATch		(ActCht+ 34)
#define ActSeePstForCrsUsr	(ActCht+ 35)
#define ActSeePstForCrsTch	(ActCht+ 36)
#define ActSeePstForDegUsr	(ActCht+ 37)
#define ActSeePstForDegTch	(ActCht+ 38)
#define ActSeePstForCtrUsr	(ActCht+ 39)
#define ActSeePstForCtrTch	(ActCht+ 40)
#define ActSeePstForInsUsr	(ActCht+ 41)
#define ActSeePstForInsTch	(ActCht+ 42)
#define ActSeePstForGenUsr	(ActCht+ 43)
#define ActSeePstForGenTch	(ActCht+ 44)
#define ActSeePstForSWAUsr	(ActCht+ 45)
#define ActSeePstForSWATch	(ActCht+ 46)
#define ActRcvThrForCrsUsr	(ActCht+ 47)
#define ActRcvThrForCrsTch	(ActCht+ 48)
#define ActRcvThrForDegUsr	(ActCht+ 49)
#define ActRcvThrForDegTch	(ActCht+ 50)
#define ActRcvThrForCtrUsr	(ActCht+ 51)
#define ActRcvThrForCtrTch	(ActCht+ 52)
#define ActRcvThrForInsUsr	(ActCht+ 53)
#define ActRcvThrForInsTch	(ActCht+ 54)
#define ActRcvThrForGenUsr	(ActCht+ 55)
#define ActRcvThrForGenTch	(ActCht+ 56)
#define ActRcvThrForSWAUsr	(ActCht+ 57)
#define ActRcvThrForSWATch	(ActCht+ 58)
#define ActRcvRepForCrsUsr	(ActCht+ 59)
#define ActRcvRepForCrsTch	(ActCht+ 60)
#define ActRcvRepForDegUsr	(ActCht+ 61)
#define ActRcvRepForDegTch	(ActCht+ 62)
#define ActRcvRepForCtrUsr	(ActCht+ 63)
#define ActRcvRepForCtrTch	(ActCht+ 64)
#define ActRcvRepForInsUsr	(ActCht+ 65)
#define ActRcvRepForInsTch	(ActCht+ 66)
#define ActRcvRepForGenUsr	(ActCht+ 67)
#define ActRcvRepForGenTch	(ActCht+ 68)
#define ActRcvRepForSWAUsr	(ActCht+ 69)
#define ActRcvRepForSWATch	(ActCht+ 70)
#define ActReqDelThrCrsUsr	(ActCht+ 71)
#define ActReqDelThrCrsTch	(ActCht+ 72)
#define ActReqDelThrDegUsr	(ActCht+ 73)
#define ActReqDelThrDegTch	(ActCht+ 74)
#define ActReqDelThrCtrUsr	(ActCht+ 75)
#define ActReqDelThrCtrTch	(ActCht+ 76)
#define ActReqDelThrInsUsr	(ActCht+ 77)
#define ActReqDelThrInsTch	(ActCht+ 78)
#define ActReqDelThrGenUsr	(ActCht+ 79)
#define ActReqDelThrGenTch	(ActCht+ 80)
#define ActReqDelThrSWAUsr	(ActCht+ 81)
#define ActReqDelThrSWATch	(ActCht+ 82)
#define ActDelThrForCrsUsr	(ActCht+ 83)
#define ActDelThrForCrsTch	(ActCht+ 84)
#define ActDelThrForDegUsr	(ActCht+ 85)
#define ActDelThrForDegTch	(ActCht+ 86)
#define ActDelThrForCtrUsr	(ActCht+ 87)
#define ActDelThrForCtrTch	(ActCht+ 88)
#define ActDelThrForInsUsr	(ActCht+ 89)
#define ActDelThrForInsTch	(ActCht+ 90)
#define ActDelThrForGenUsr	(ActCht+ 91)
#define ActDelThrForGenTch	(ActCht+ 92)
#define ActDelThrForSWAUsr	(ActCht+ 93)
#define ActDelThrForSWATch	(ActCht+ 94)
#define ActCutThrForCrsUsr	(ActCht+ 95)
#define ActCutThrForCrsTch	(ActCht+ 96)
#define ActCutThrForDegUsr	(ActCht+ 97)
#define ActCutThrForDegTch	(ActCht+ 98)
#define ActCutThrForCtrUsr	(ActCht+ 99)
#define ActCutThrForCtrTch	(ActCht+100)
#define ActCutThrForInsUsr	(ActCht+101)
#define ActCutThrForInsTch	(ActCht+102)
#define ActCutThrForGenUsr	(ActCht+103)
#define ActCutThrForGenTch	(ActCht+104)
#define ActCutThrForSWAUsr	(ActCht+105)
#define ActCutThrForSWATch	(ActCht+106)
#define ActPasThrForCrsUsr	(ActCht+107)
#define ActPasThrForCrsTch	(ActCht+108)
#define ActPasThrForDegUsr	(ActCht+109)
#define ActPasThrForDegTch	(ActCht+110)
#define ActPasThrForCtrUsr	(ActCht+111)
#define ActPasThrForCtrTch	(ActCht+112)
#define ActPasThrForInsUsr	(ActCht+113)
#define ActPasThrForInsTch	(ActCht+114)
#define ActPasThrForGenUsr	(ActCht+115)
#define ActPasThrForGenTch	(ActCht+116)
#define ActPasThrForSWAUsr	(ActCht+117)
#define ActPasThrForSWATch	(ActCht+118)
#define ActDelPstForCrsUsr	(ActCht+119)
#define ActDelPstForCrsTch	(ActCht+120)
#define ActDelPstForDegUsr	(ActCht+121)
#define ActDelPstForDegTch	(ActCht+122)
#define ActDelPstForCtrUsr	(ActCht+123)
#define ActDelPstForCtrTch	(ActCht+124)
#define ActDelPstForInsUsr	(ActCht+125)
#define ActDelPstForInsTch	(ActCht+126)
#define ActDelPstForGenUsr	(ActCht+127)
#define ActDelPstForGenTch	(ActCht+128)
#define ActDelPstForSWAUsr	(ActCht+129)
#define ActDelPstForSWATch	(ActCht+130)
#define ActEnbPstForCrsUsr	(ActCht+131)
#define ActEnbPstForCrsTch	(ActCht+132)
#define ActEnbPstForDegUsr	(ActCht+133)
#define ActEnbPstForDegTch	(ActCht+134)
#define ActEnbPstForCtrUsr	(ActCht+135)
#define ActEnbPstForCtrTch	(ActCht+136)
#define ActEnbPstForInsUsr	(ActCht+137)
#define ActEnbPstForInsTch	(ActCht+138)
#define ActEnbPstForGenUsr	(ActCht+139)
#define ActEnbPstForGenTch	(ActCht+140)
#define ActEnbPstForSWAUsr	(ActCht+141)
#define ActEnbPstForSWATch	(ActCht+142)
#define ActDisPstForCrsUsr	(ActCht+143)
#define ActDisPstForCrsTch	(ActCht+144)
#define ActDisPstForDegUsr	(ActCht+145)
#define ActDisPstForDegTch	(ActCht+146)
#define ActDisPstForCtrUsr	(ActCht+147)
#define ActDisPstForCtrTch	(ActCht+148)
#define ActDisPstForInsUsr	(ActCht+149)
#define ActDisPstForInsTch	(ActCht+150)
#define ActDisPstForGenUsr	(ActCht+151)
#define ActDisPstForGenTch	(ActCht+152)
#define ActDisPstForSWAUsr	(ActCht+153)
#define ActDisPstForSWATch	(ActCht+154)
#define ActRcvMsgUsr		(ActCht+155)
#define ActReqDelAllSntMsg	(ActCht+156)
#define ActReqDelAllRcvMsg	(ActCht+157)
#define ActDelAllSntMsg		(ActCht+158)
#define ActDelAllRcvMsg		(ActCht+159)
#define ActDelSntMsg		(ActCht+160)
#define ActDelRcvMsg		(ActCht+161)
#define ActExpSntMsg		(ActCht+162)
#define ActExpRcvMsg		(ActCht+163)
#define ActConSntMsg		(ActCht+164)
#define ActConRcvMsg		(ActCht+165)
#define ActLstBanUsr		(ActCht+166)
#define ActBanUsrMsg		(ActCht+167)
#define ActUnbUsrMsg		(ActCht+168)
#define ActUnbUsrLst		(ActCht+169)

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
