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

#include "swad_bool.h"
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
   TabMsg =  9,
   TabSta = 10,
   TabPrf = 11,
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

#define Act_NUM_ACTIONS	(7+52+15+70+53+48+204+184+94+168+28+75)

#define Act_MAX_ACTION_COD 1336

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

// Secondary actions
#define ActInsSch		(ActChgInsSta+  8)
#define ActPrnInsInf		(ActChgInsSta+  9)
#define ActReqInsLog		(ActChgInsSta+ 10)
#define ActRecInsLog		(ActChgInsSta+ 11)

#define ActEdiCtr		(ActChgInsSta+ 12)
#define ActReqCtr		(ActChgInsSta+ 13)
#define ActNewCtr		(ActChgInsSta+ 14)
#define ActRemCtr		(ActChgInsSta+ 15)
#define ActChgCtrIns		(ActChgInsSta+ 16)
#define ActChgCtrPlc		(ActChgInsSta+ 17)
#define ActRenCtrSho		(ActChgInsSta+ 18)
#define ActRenCtrFul		(ActChgInsSta+ 19)
#define ActChgCtrWWW		(ActChgInsSta+ 20)
#define ActChgCtrSta		(ActChgInsSta+ 21)

#define ActEdiDpt		(ActChgInsSta+ 22)
#define ActNewDpt		(ActChgInsSta+ 23)
#define ActRemDpt		(ActChgInsSta+ 24)
#define ActChgDptIns		(ActChgInsSta+ 25)
#define ActRenDptSho		(ActChgInsSta+ 26)
#define ActRenDptFul		(ActChgInsSta+ 27)
#define ActChgDptWWW		(ActChgInsSta+ 28)

#define ActEdiPlc		(ActChgInsSta+ 29)
#define ActNewPlc		(ActChgInsSta+ 30)
#define ActRemPlc		(ActChgInsSta+ 31)
#define ActRenPlcSho		(ActChgInsSta+ 32)
#define ActRenPlcFul		(ActChgInsSta+ 33)

#define ActEdiHld		(ActChgInsSta+ 34)
#define ActNewHld		(ActChgInsSta+ 35)
#define ActRemHld		(ActChgInsSta+ 36)
#define ActChgHldPlc		(ActChgInsSta+ 37)
#define ActChgHldTyp		(ActChgInsSta+ 38)
#define ActChgHldStrDat		(ActChgInsSta+ 39)
#define ActChgHldEndDat		(ActChgInsSta+ 40)
#define ActRenHld		(ActChgInsSta+ 41)

#define ActChgToSeeDocIns	(ActChgInsSta+ 42)
#define ActSeeDocIns		(ActChgInsSta+ 43)
#define ActExpSeeDocIns		(ActChgInsSta+ 44)
#define ActConSeeDocIns		(ActChgInsSta+ 45)
#define ActZIPSeeDocIns		(ActChgInsSta+ 46)
#define ActReqDatSeeDocIns	(ActChgInsSta+ 47)
#define ActDowSeeDocIns		(ActChgInsSta+ 48)
#define ActChgToAdmDocIns	(ActChgInsSta+ 49)
#define ActAdmDocIns		(ActChgInsSta+ 50)
#define ActReqRemFilDocIns	(ActChgInsSta+ 51)
#define ActRemFilDocIns		(ActChgInsSta+ 52)
#define ActRemFolDocIns		(ActChgInsSta+ 53)
#define ActCopDocIns		(ActChgInsSta+ 54)
#define ActPasDocIns		(ActChgInsSta+ 55)
#define ActRemTreDocIns		(ActChgInsSta+ 56)
#define ActFrmCreDocIns		(ActChgInsSta+ 57)
#define ActCreFolDocIns		(ActChgInsSta+ 58)
#define ActCreLnkDocIns		(ActChgInsSta+ 59)
#define ActRenFolDocIns		(ActChgInsSta+ 60)
#define ActRcvFilDocInsDZ	(ActChgInsSta+ 61)
#define ActRcvFilDocInsCla	(ActChgInsSta+ 62)
#define ActExpAdmDocIns		(ActChgInsSta+ 63)
#define ActConAdmDocIns		(ActChgInsSta+ 64)
#define ActZIPAdmDocIns		(ActChgInsSta+ 65)
#define ActShoDocIns		(ActChgInsSta+ 66)
#define ActHidDocIns		(ActChgInsSta+ 67)
#define ActReqDatAdmDocIns	(ActChgInsSta+ 68)
#define ActChgDatAdmDocIns	(ActChgInsSta+ 69)
#define ActDowAdmDocIns		(ActChgInsSta+ 70)

/*****************************************************************************/
/********************************* Centre tab ********************************/
/*****************************************************************************/
// Actions in menu
#define ActCtrReqSch		(ActDowAdmDocIns+  1)
#define ActSeeCtrInf		(ActDowAdmDocIns+  2)
#define ActSeeDeg		(ActDowAdmDocIns+  3)
#define ActSeeAdmDocCtr		(ActDowAdmDocIns+  4)

// Secondary actions
#define ActCtrSch		(ActDowAdmDocIns+  5)
#define ActPrnCtrInf		(ActDowAdmDocIns+  6)
#define ActReqCtrLog		(ActDowAdmDocIns+  7)
#define ActRecCtrLog		(ActDowAdmDocIns+  8)
#define ActReqCtrPho		(ActDowAdmDocIns+  9)
#define ActRecCtrPho		(ActDowAdmDocIns+ 10)
#define ActChgCtrPhoAtt		(ActDowAdmDocIns+ 11)

#define ActEdiDeg		(ActDowAdmDocIns+ 12)
#define ActReqDeg		(ActDowAdmDocIns+ 13)
#define ActNewDeg		(ActDowAdmDocIns+ 14)
#define ActRemDeg		(ActDowAdmDocIns+ 15)
#define ActRenDegSho		(ActDowAdmDocIns+ 16)
#define ActRenDegFul		(ActDowAdmDocIns+ 17)
#define ActChgDegTyp		(ActDowAdmDocIns+ 18)
#define ActChgDegCtr		(ActDowAdmDocIns+ 19)
#define ActChgDegFstYea		(ActDowAdmDocIns+ 20)
#define ActChgDegLstYea		(ActDowAdmDocIns+ 21)
#define ActChgDegOptYea		(ActDowAdmDocIns+ 22)
#define ActChgDegWWW		(ActDowAdmDocIns+ 23)
#define ActChgDegSta		(ActDowAdmDocIns+ 24)

#define ActChgToSeeDocCtr	(ActDowAdmDocIns+ 25)
#define ActSeeDocCtr		(ActDowAdmDocIns+ 26)
#define ActExpSeeDocCtr		(ActDowAdmDocIns+ 27)
#define ActConSeeDocCtr		(ActDowAdmDocIns+ 28)
#define ActZIPSeeDocCtr		(ActDowAdmDocIns+ 29)
#define ActReqDatSeeDocCtr	(ActDowAdmDocIns+ 30)
#define ActDowSeeDocCtr		(ActDowAdmDocIns+ 31)
#define ActChgToAdmDocCtr	(ActDowAdmDocIns+ 32)
#define ActAdmDocCtr		(ActDowAdmDocIns+ 33)
#define ActReqRemFilDocCtr	(ActDowAdmDocIns+ 34)
#define ActRemFilDocCtr		(ActDowAdmDocIns+ 35)
#define ActRemFolDocCtr		(ActDowAdmDocIns+ 36)
#define ActCopDocCtr		(ActDowAdmDocIns+ 37)
#define ActPasDocCtr		(ActDowAdmDocIns+ 38)
#define ActRemTreDocCtr		(ActDowAdmDocIns+ 39)
#define ActFrmCreDocCtr		(ActDowAdmDocIns+ 40)
#define ActCreFolDocCtr		(ActDowAdmDocIns+ 41)
#define ActCreLnkDocCtr		(ActDowAdmDocIns+ 42)
#define ActRenFolDocCtr		(ActDowAdmDocIns+ 43)
#define ActRcvFilDocCtrDZ	(ActDowAdmDocIns+ 44)
#define ActRcvFilDocCtrCla	(ActDowAdmDocIns+ 45)
#define ActExpAdmDocCtr		(ActDowAdmDocIns+ 46)
#define ActConAdmDocCtr		(ActDowAdmDocIns+ 47)
#define ActZIPAdmDocCtr		(ActDowAdmDocIns+ 48)
#define ActShoDocCtr		(ActDowAdmDocIns+ 49)
#define ActHidDocCtr		(ActDowAdmDocIns+ 50)
#define ActReqDatAdmDocCtr	(ActDowAdmDocIns+ 51)
#define ActChgDatAdmDocCtr	(ActDowAdmDocIns+ 52)
#define ActDowAdmDocCtr		(ActDowAdmDocIns+ 53)

/*****************************************************************************/
/********************************* Degree tab ********************************/
/*****************************************************************************/
// Actions in menu
#define ActDegReqSch		(ActDowAdmDocCtr+  1)
#define ActSeeDegInf		(ActDowAdmDocCtr+  2)
#define ActSeeCrs		(ActDowAdmDocCtr+  3)
#define ActSeeAdmDocDeg		(ActDowAdmDocCtr+  4)

// Secondary actions
#define ActDegSch		(ActDowAdmDocCtr+  5)
#define ActPrnDegInf		(ActDowAdmDocCtr+  6)
#define ActReqDegLog		(ActDowAdmDocCtr+  7)
#define ActRecDegLog		(ActDowAdmDocCtr+  8)

#define ActEdiCrs		(ActDowAdmDocCtr+  9)
#define ActReqCrs		(ActDowAdmDocCtr+ 10)
#define ActNewCrs		(ActDowAdmDocCtr+ 11)
#define ActRemCrs		(ActDowAdmDocCtr+ 12)
#define ActChgInsCrsCod		(ActDowAdmDocCtr+ 13)
#define ActChgCrsDeg		(ActDowAdmDocCtr+ 14)
#define ActChgCrsYea		(ActDowAdmDocCtr+ 15)
#define ActChgCrsSem		(ActDowAdmDocCtr+ 16)
#define ActRenCrsSho		(ActDowAdmDocCtr+ 17)
#define ActRenCrsFul		(ActDowAdmDocCtr+ 18)
#define ActChgCrsSta		(ActDowAdmDocCtr+ 19)

#define ActChgToSeeDocDeg	(ActDowAdmDocCtr+ 20)
#define ActSeeDocDeg		(ActDowAdmDocCtr+ 21)
#define ActExpSeeDocDeg		(ActDowAdmDocCtr+ 22)
#define ActConSeeDocDeg		(ActDowAdmDocCtr+ 23)
#define ActZIPSeeDocDeg		(ActDowAdmDocCtr+ 24)
#define ActReqDatSeeDocDeg	(ActDowAdmDocCtr+ 25)
#define ActDowSeeDocDeg		(ActDowAdmDocCtr+ 26)
#define ActChgToAdmDocDeg	(ActDowAdmDocCtr+ 27)
#define ActAdmDocDeg		(ActDowAdmDocCtr+ 28)
#define ActReqRemFilDocDeg	(ActDowAdmDocCtr+ 29)
#define ActRemFilDocDeg		(ActDowAdmDocCtr+ 30)
#define ActRemFolDocDeg		(ActDowAdmDocCtr+ 31)
#define ActCopDocDeg		(ActDowAdmDocCtr+ 32)
#define ActPasDocDeg		(ActDowAdmDocCtr+ 33)
#define ActRemTreDocDeg		(ActDowAdmDocCtr+ 34)
#define ActFrmCreDocDeg		(ActDowAdmDocCtr+ 35)
#define ActCreFolDocDeg		(ActDowAdmDocCtr+ 36)
#define ActCreLnkDocDeg		(ActDowAdmDocCtr+ 37)
#define ActRenFolDocDeg		(ActDowAdmDocCtr+ 38)
#define ActRcvFilDocDegDZ	(ActDowAdmDocCtr+ 39)
#define ActRcvFilDocDegCla	(ActDowAdmDocCtr+ 40)
#define ActExpAdmDocDeg		(ActDowAdmDocCtr+ 41)
#define ActConAdmDocDeg		(ActDowAdmDocCtr+ 42)
#define ActZIPAdmDocDeg		(ActDowAdmDocCtr+ 43)
#define ActShoDocDeg		(ActDowAdmDocCtr+ 44)
#define ActHidDocDeg		(ActDowAdmDocCtr+ 45)
#define ActReqDatAdmDocDeg	(ActDowAdmDocCtr+ 46)
#define ActChgDatAdmDocDeg	(ActDowAdmDocCtr+ 47)
#define ActDowAdmDocDeg		(ActDowAdmDocCtr+ 48)

/*****************************************************************************/
/******************************** Course tab *********************************/
/*****************************************************************************/
// Actions in menu
#define ActCrsReqSch		(ActDowAdmDocDeg+  1)
#define ActSeeCrsInf		(ActDowAdmDocDeg+  2)
#define ActSeeTchGui		(ActDowAdmDocDeg+  3)
#define ActSeeSyl		(ActDowAdmDocDeg+  4)
#define ActSeeAdmDocCrs		(ActDowAdmDocDeg+  5)
#define ActAdmCom		(ActDowAdmDocDeg+  6)
#define ActSeeCrsTimTbl		(ActDowAdmDocDeg+  7)
#define ActSeeBib		(ActDowAdmDocDeg+  8)
#define ActSeeFAQ		(ActDowAdmDocDeg+  9)
#define ActSeeCrsLnk		(ActDowAdmDocDeg+ 10)
// Secondary actions
#define ActCrsSch		(ActDowAdmDocDeg+ 11)
#define ActPrnCrsInf		(ActDowAdmDocDeg+ 12)
#define ActChgCrsLog		(ActDowAdmDocDeg+ 13)
#define ActEdiCrsInf		(ActDowAdmDocDeg+ 14)
#define ActEdiTchGui		(ActDowAdmDocDeg+ 15)
#define ActPrnCrsTimTbl		(ActDowAdmDocDeg+ 16)
#define ActEdiCrsTimTbl		(ActDowAdmDocDeg+ 17)
#define ActChgCrsTimTbl		(ActDowAdmDocDeg+ 18)
#define ActSeeSylLec		(ActDowAdmDocDeg+ 19)
#define ActSeeSylPra		(ActDowAdmDocDeg+ 20)
#define ActEdiSylLec		(ActDowAdmDocDeg+ 21)
#define ActEdiSylPra		(ActDowAdmDocDeg+ 22)
#define ActDelItmSylLec		(ActDowAdmDocDeg+ 23)
#define ActDelItmSylPra		(ActDowAdmDocDeg+ 24)
#define ActUp_IteSylLec		(ActDowAdmDocDeg+ 25)
#define ActUp_IteSylPra		(ActDowAdmDocDeg+ 26)
#define ActDwnIteSylLec		(ActDowAdmDocDeg+ 27)
#define ActDwnIteSylPra		(ActDowAdmDocDeg+ 28)
#define ActRgtIteSylLec		(ActDowAdmDocDeg+ 29)
#define ActRgtIteSylPra		(ActDowAdmDocDeg+ 30)
#define ActLftIteSylLec		(ActDowAdmDocDeg+ 31)
#define ActLftIteSylPra		(ActDowAdmDocDeg+ 32)
#define ActInsIteSylLec		(ActDowAdmDocDeg+ 33)
#define ActInsIteSylPra		(ActDowAdmDocDeg+ 34)
#define ActModIteSylLec		(ActDowAdmDocDeg+ 35)
#define ActModIteSylPra		(ActDowAdmDocDeg+ 36)

#define ActChgToSeeDocCrs	(ActDowAdmDocDeg+ 37)
#define ActSeeDocCrs		(ActDowAdmDocDeg+ 38)
#define ActExpSeeDocCrs		(ActDowAdmDocDeg+ 39)
#define ActConSeeDocCrs		(ActDowAdmDocDeg+ 40)
#define ActZIPSeeDocCrs		(ActDowAdmDocDeg+ 41)
#define ActReqDatSeeDocCrs	(ActDowAdmDocDeg+ 42)
#define ActDowSeeDocCrs		(ActDowAdmDocDeg+ 43)

#define ActSeeDocGrp		(ActDowAdmDocDeg+ 44)
#define ActExpSeeDocGrp		(ActDowAdmDocDeg+ 45)
#define ActConSeeDocGrp		(ActDowAdmDocDeg+ 46)
#define ActZIPSeeDocGrp		(ActDowAdmDocDeg+ 47)
#define ActReqDatSeeDocGrp	(ActDowAdmDocDeg+ 48)
#define ActDowSeeDocGrp		(ActDowAdmDocDeg+ 49)

#define ActChgToAdmDocCrs	(ActDowAdmDocDeg+ 50)
#define ActAdmDocCrs		(ActDowAdmDocDeg+ 51)
#define ActReqRemFilDocCrs	(ActDowAdmDocDeg+ 52)
#define ActRemFilDocCrs		(ActDowAdmDocDeg+ 53)
#define ActRemFolDocCrs		(ActDowAdmDocDeg+ 54)
#define ActCopDocCrs		(ActDowAdmDocDeg+ 55)
#define ActPasDocCrs		(ActDowAdmDocDeg+ 56)
#define ActRemTreDocCrs		(ActDowAdmDocDeg+ 57)
#define ActFrmCreDocCrs		(ActDowAdmDocDeg+ 58)
#define ActCreFolDocCrs		(ActDowAdmDocDeg+ 59)
#define ActCreLnkDocCrs		(ActDowAdmDocDeg+ 60)
#define ActRenFolDocCrs		(ActDowAdmDocDeg+ 61)
#define ActRcvFilDocCrsDZ	(ActDowAdmDocDeg+ 62)
#define ActRcvFilDocCrsCla	(ActDowAdmDocDeg+ 63)
#define ActExpAdmDocCrs		(ActDowAdmDocDeg+ 64)
#define ActConAdmDocCrs		(ActDowAdmDocDeg+ 65)
#define ActZIPAdmDocCrs		(ActDowAdmDocDeg+ 66)
#define ActShoDocCrs		(ActDowAdmDocDeg+ 67)
#define ActHidDocCrs		(ActDowAdmDocDeg+ 68)
#define ActReqDatAdmDocCrs	(ActDowAdmDocDeg+ 69)
#define ActChgDatAdmDocCrs	(ActDowAdmDocDeg+ 70)
#define ActDowAdmDocCrs		(ActDowAdmDocDeg+ 71)

#define ActAdmDocGrp		(ActDowAdmDocDeg+ 72)
#define ActReqRemFilDocGrp	(ActDowAdmDocDeg+ 73)
#define ActRemFilDocGrp		(ActDowAdmDocDeg+ 74)
#define ActRemFolDocGrp		(ActDowAdmDocDeg+ 75)
#define ActCopDocGrp		(ActDowAdmDocDeg+ 76)
#define ActPasDocGrp		(ActDowAdmDocDeg+ 77)
#define ActRemTreDocGrp		(ActDowAdmDocDeg+ 78)
#define ActFrmCreDocGrp		(ActDowAdmDocDeg+ 79)
#define ActCreFolDocGrp		(ActDowAdmDocDeg+ 80)
#define ActCreLnkDocGrp		(ActDowAdmDocDeg+ 81)
#define ActRenFolDocGrp		(ActDowAdmDocDeg+ 82)
#define ActRcvFilDocGrpDZ	(ActDowAdmDocDeg+ 83)
#define ActRcvFilDocGrpCla	(ActDowAdmDocDeg+ 84)
#define ActExpAdmDocGrp		(ActDowAdmDocDeg+ 85)
#define ActConAdmDocGrp		(ActDowAdmDocDeg+ 86)
#define ActZIPAdmDocGrp		(ActDowAdmDocDeg+ 87)
#define ActShoDocGrp		(ActDowAdmDocDeg+ 88)
#define ActHidDocGrp		(ActDowAdmDocDeg+ 89)
#define ActReqDatAdmDocGrp	(ActDowAdmDocDeg+ 90)
#define ActChgDatAdmDocGrp	(ActDowAdmDocDeg+ 91)
#define ActDowAdmDocGrp		(ActDowAdmDocDeg+ 92)

#define ActChgToAdmCom		(ActDowAdmDocDeg+ 93)

#define ActAdmComCrs		(ActDowAdmDocDeg+ 94)
#define ActReqRemFilComCrs	(ActDowAdmDocDeg+ 95)
#define ActRemFilComCrs		(ActDowAdmDocDeg+ 96)
#define ActRemFolComCrs		(ActDowAdmDocDeg+ 97)
#define ActCopComCrs		(ActDowAdmDocDeg+ 98)
#define ActPasComCrs		(ActDowAdmDocDeg+ 99)
#define ActRemTreComCrs		(ActDowAdmDocDeg+100)
#define ActFrmCreComCrs		(ActDowAdmDocDeg+101)
#define ActCreFolComCrs		(ActDowAdmDocDeg+102)
#define ActCreLnkComCrs		(ActDowAdmDocDeg+103)
#define ActRenFolComCrs		(ActDowAdmDocDeg+104)
#define ActRcvFilComCrsDZ	(ActDowAdmDocDeg+105)
#define ActRcvFilComCrsCla	(ActDowAdmDocDeg+106)
#define ActExpComCrs		(ActDowAdmDocDeg+107)
#define ActConComCrs		(ActDowAdmDocDeg+108)
#define ActZIPComCrs		(ActDowAdmDocDeg+109)
#define ActReqDatComCrs		(ActDowAdmDocDeg+110)
#define ActChgDatComCrs		(ActDowAdmDocDeg+111)
#define ActDowComCrs		(ActDowAdmDocDeg+112)

#define ActAdmComGrp		(ActDowAdmDocDeg+113)
#define ActReqRemFilComGrp	(ActDowAdmDocDeg+114)
#define ActRemFilComGrp		(ActDowAdmDocDeg+115)
#define ActRemFolComGrp		(ActDowAdmDocDeg+116)
#define ActCopComGrp		(ActDowAdmDocDeg+117)
#define ActPasComGrp		(ActDowAdmDocDeg+118)
#define ActRemTreComGrp		(ActDowAdmDocDeg+119)
#define ActFrmCreComGrp		(ActDowAdmDocDeg+120)
#define ActCreFolComGrp		(ActDowAdmDocDeg+121)
#define ActCreLnkComGrp		(ActDowAdmDocDeg+122)
#define ActRenFolComGrp		(ActDowAdmDocDeg+123)
#define ActRcvFilComGrpDZ	(ActDowAdmDocDeg+124)
#define ActRcvFilComGrpCla	(ActDowAdmDocDeg+125)
#define ActExpComGrp		(ActDowAdmDocDeg+126)
#define ActConComGrp		(ActDowAdmDocDeg+127)
#define ActZIPComGrp		(ActDowAdmDocDeg+128)
#define ActReqDatComGrp		(ActDowAdmDocDeg+129)
#define ActChgDatComGrp		(ActDowAdmDocDeg+130)
#define ActDowComGrp		(ActDowAdmDocDeg+131)

#define ActEdiBib		(ActDowAdmDocDeg+132)
#define ActEdiFAQ		(ActDowAdmDocDeg+133)
#define ActEdiCrsLnk		(ActDowAdmDocDeg+134)

#define ActChgFrcReaCrsInf	(ActDowAdmDocDeg+135)
#define ActChgFrcReaTchGui	(ActDowAdmDocDeg+136)
#define ActChgFrcReaSylLec	(ActDowAdmDocDeg+137)
#define ActChgFrcReaSylPra	(ActDowAdmDocDeg+138)
#define ActChgFrcReaBib		(ActDowAdmDocDeg+139)
#define ActChgFrcReaFAQ		(ActDowAdmDocDeg+140)
#define ActChgFrcReaCrsLnk	(ActDowAdmDocDeg+141)

#define ActChgHavReaCrsInf	(ActDowAdmDocDeg+142)
#define ActChgHavReaTchGui	(ActDowAdmDocDeg+143)
#define ActChgHavReaSylLec	(ActDowAdmDocDeg+144)
#define ActChgHavReaSylPra	(ActDowAdmDocDeg+145)
#define ActChgHavReaBib		(ActDowAdmDocDeg+146)
#define ActChgHavReaFAQ		(ActDowAdmDocDeg+147)
#define ActChgHavReaCrsLnk	(ActDowAdmDocDeg+148)

#define ActSelInfSrcCrsInf	(ActDowAdmDocDeg+149)
#define ActSelInfSrcTchGui	(ActDowAdmDocDeg+150)
#define ActSelInfSrcSylLec	(ActDowAdmDocDeg+151)
#define ActSelInfSrcSylPra	(ActDowAdmDocDeg+152)
#define ActSelInfSrcBib		(ActDowAdmDocDeg+153)
#define ActSelInfSrcFAQ		(ActDowAdmDocDeg+154)
#define ActSelInfSrcCrsLnk	(ActDowAdmDocDeg+155)
#define ActRcvURLCrsInf		(ActDowAdmDocDeg+156)
#define ActRcvURLTchGui		(ActDowAdmDocDeg+157)
#define ActRcvURLSylLec		(ActDowAdmDocDeg+158)
#define ActRcvURLSylPra		(ActDowAdmDocDeg+159)
#define ActRcvURLBib		(ActDowAdmDocDeg+160)
#define ActRcvURLFAQ		(ActDowAdmDocDeg+161)
#define ActRcvURLCrsLnk		(ActDowAdmDocDeg+162)
#define ActRcvPagCrsInf		(ActDowAdmDocDeg+163)
#define ActRcvPagTchGui		(ActDowAdmDocDeg+164)
#define ActRcvPagSylLec		(ActDowAdmDocDeg+165)
#define ActRcvPagSylPra		(ActDowAdmDocDeg+166)
#define ActRcvPagBib		(ActDowAdmDocDeg+167)
#define ActRcvPagFAQ		(ActDowAdmDocDeg+168)
#define ActRcvPagCrsLnk		(ActDowAdmDocDeg+169)
#define ActEditorCrsInf		(ActDowAdmDocDeg+170)
#define ActEditorTchGui		(ActDowAdmDocDeg+171)
#define ActEditorSylLec		(ActDowAdmDocDeg+172)
#define ActEditorSylPra		(ActDowAdmDocDeg+173)
#define ActEditorBib		(ActDowAdmDocDeg+174)
#define ActEditorFAQ		(ActDowAdmDocDeg+175)
#define ActEditorCrsLnk		(ActDowAdmDocDeg+176)
#define ActPlaTxtEdiCrsInf	(ActDowAdmDocDeg+177)
#define ActPlaTxtEdiTchGui	(ActDowAdmDocDeg+178)
#define ActPlaTxtEdiSylLec	(ActDowAdmDocDeg+179)
#define ActPlaTxtEdiSylPra	(ActDowAdmDocDeg+180)
#define ActPlaTxtEdiBib		(ActDowAdmDocDeg+181)
#define ActPlaTxtEdiFAQ		(ActDowAdmDocDeg+182)
#define ActPlaTxtEdiCrsLnk	(ActDowAdmDocDeg+183)
#define ActRchTxtEdiCrsInf	(ActDowAdmDocDeg+184)
#define ActRchTxtEdiTchGui	(ActDowAdmDocDeg+185)
#define ActRchTxtEdiSylLec	(ActDowAdmDocDeg+186)
#define ActRchTxtEdiSylPra	(ActDowAdmDocDeg+187)
#define ActRchTxtEdiBib		(ActDowAdmDocDeg+188)
#define ActRchTxtEdiFAQ		(ActDowAdmDocDeg+189)
#define ActRchTxtEdiCrsLnk	(ActDowAdmDocDeg+190)
#define ActRcvPlaTxtCrsInf	(ActDowAdmDocDeg+191)
#define ActRcvPlaTxtTchGui	(ActDowAdmDocDeg+192)
#define ActRcvPlaTxtSylLec	(ActDowAdmDocDeg+193)
#define ActRcvPlaTxtSylPra	(ActDowAdmDocDeg+194)
#define ActRcvPlaTxtBib		(ActDowAdmDocDeg+195)
#define ActRcvPlaTxtFAQ		(ActDowAdmDocDeg+196)
#define ActRcvPlaTxtCrsLnk	(ActDowAdmDocDeg+197)
#define ActRcvRchTxtCrsInf	(ActDowAdmDocDeg+198)
#define ActRcvRchTxtTchGui	(ActDowAdmDocDeg+199)
#define ActRcvRchTxtSylLec	(ActDowAdmDocDeg+200)
#define ActRcvRchTxtSylPra	(ActDowAdmDocDeg+201)
#define ActRcvRchTxtBib		(ActDowAdmDocDeg+202)
#define ActRcvRchTxtFAQ		(ActDowAdmDocDeg+203)
#define ActRcvRchTxtCrsLnk	(ActDowAdmDocDeg+204)

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
#define ActSeeRecCrs		(ActRcvRchTxtCrsLnk+  9)
// Secondary actions
#define ActEdiAss		(ActRcvRchTxtCrsLnk+ 10)
#define ActChgFrcReaAss		(ActRcvRchTxtCrsLnk+ 11)
#define ActChgHavReaAss		(ActRcvRchTxtCrsLnk+ 12)
#define ActSelInfSrcAss		(ActRcvRchTxtCrsLnk+ 13)
#define ActRcvURLAss		(ActRcvRchTxtCrsLnk+ 14)
#define ActRcvPagAss		(ActRcvRchTxtCrsLnk+ 15)
#define ActEditorAss		(ActRcvRchTxtCrsLnk+ 16)
#define ActPlaTxtEdiAss		(ActRcvRchTxtCrsLnk+ 17)
#define ActRchTxtEdiAss		(ActRcvRchTxtCrsLnk+ 18)
#define ActRcvPlaTxtAss		(ActRcvRchTxtCrsLnk+ 19)
#define ActRcvRchTxtAss		(ActRcvRchTxtCrsLnk+ 20)

#define ActFrmNewAsg		(ActRcvRchTxtCrsLnk+ 21)
#define ActEdiOneAsg		(ActRcvRchTxtCrsLnk+ 22)
#define ActNewAsg		(ActRcvRchTxtCrsLnk+ 23)
#define ActChgAsg		(ActRcvRchTxtCrsLnk+ 24)
#define ActReqRemAsg		(ActRcvRchTxtCrsLnk+ 25)
#define ActRemAsg		(ActRcvRchTxtCrsLnk+ 26)
#define ActHidAsg		(ActRcvRchTxtCrsLnk+ 27)
#define ActShoAsg		(ActRcvRchTxtCrsLnk+ 28)

#define ActRcvRecCrs		(ActRcvRchTxtCrsLnk+ 29)
#define ActAdmAsgWrkCrs		(ActRcvRchTxtCrsLnk+ 30)

#define ActReqRemFilAsgUsr	(ActRcvRchTxtCrsLnk+ 31)
#define ActRemFilAsgUsr		(ActRcvRchTxtCrsLnk+ 32)
#define ActRemFolAsgUsr		(ActRcvRchTxtCrsLnk+ 33)
#define ActCopAsgUsr		(ActRcvRchTxtCrsLnk+ 34)
#define ActPasAsgUsr		(ActRcvRchTxtCrsLnk+ 35)
#define ActRemTreAsgUsr		(ActRcvRchTxtCrsLnk+ 36)
#define ActFrmCreAsgUsr		(ActRcvRchTxtCrsLnk+ 37)
#define ActCreFolAsgUsr		(ActRcvRchTxtCrsLnk+ 38)
#define ActCreLnkAsgUsr		(ActRcvRchTxtCrsLnk+ 39)
#define ActRenFolAsgUsr		(ActRcvRchTxtCrsLnk+ 40)
#define ActRcvFilAsgUsrDZ	(ActRcvRchTxtCrsLnk+ 41)
#define ActRcvFilAsgUsrCla	(ActRcvRchTxtCrsLnk+ 42)
#define ActExpAsgUsr		(ActRcvRchTxtCrsLnk+ 43)
#define ActConAsgUsr		(ActRcvRchTxtCrsLnk+ 44)
#define ActZIPAsgUsr		(ActRcvRchTxtCrsLnk+ 45)
#define ActReqDatAsgUsr		(ActRcvRchTxtCrsLnk+ 46)
#define ActChgDatAsgUsr		(ActRcvRchTxtCrsLnk+ 47)
#define ActDowAsgUsr		(ActRcvRchTxtCrsLnk+ 48)

#define ActReqRemFilWrkUsr	(ActRcvRchTxtCrsLnk+ 49)
#define ActRemFilWrkUsr		(ActRcvRchTxtCrsLnk+ 50)
#define ActRemFolWrkUsr		(ActRcvRchTxtCrsLnk+ 51)
#define ActCopWrkUsr		(ActRcvRchTxtCrsLnk+ 52)
#define ActPasWrkUsr		(ActRcvRchTxtCrsLnk+ 53)
#define ActRemTreWrkUsr		(ActRcvRchTxtCrsLnk+ 54)
#define ActFrmCreWrkUsr		(ActRcvRchTxtCrsLnk+ 55)
#define ActCreFolWrkUsr		(ActRcvRchTxtCrsLnk+ 56)
#define ActCreLnkWrkUsr		(ActRcvRchTxtCrsLnk+ 57)
#define ActRenFolWrkUsr		(ActRcvRchTxtCrsLnk+ 58)
#define ActRcvFilWrkUsrDZ	(ActRcvRchTxtCrsLnk+ 59)
#define ActRcvFilWrkUsrCla	(ActRcvRchTxtCrsLnk+ 60)
#define ActExpWrkUsr		(ActRcvRchTxtCrsLnk+ 61)
#define ActConWrkUsr		(ActRcvRchTxtCrsLnk+ 62)
#define ActZIPWrkUsr		(ActRcvRchTxtCrsLnk+ 63)
#define ActReqDatWrkUsr		(ActRcvRchTxtCrsLnk+ 64)
#define ActChgDatWrkUsr		(ActRcvRchTxtCrsLnk+ 65)
#define ActDowWrkUsr		(ActRcvRchTxtCrsLnk+ 66)

#define ActReqRemFilAsgCrs	(ActRcvRchTxtCrsLnk+ 67)
#define ActRemFilAsgCrs		(ActRcvRchTxtCrsLnk+ 68)
#define ActRemFolAsgCrs		(ActRcvRchTxtCrsLnk+ 69)
#define ActCopAsgCrs		(ActRcvRchTxtCrsLnk+ 70)
#define ActPasAsgCrs		(ActRcvRchTxtCrsLnk+ 71)
#define ActRemTreAsgCrs		(ActRcvRchTxtCrsLnk+ 72)
#define ActFrmCreAsgCrs		(ActRcvRchTxtCrsLnk+ 73)
#define ActCreFolAsgCrs		(ActRcvRchTxtCrsLnk+ 74)
#define ActCreLnkAsgCrs		(ActRcvRchTxtCrsLnk+ 75)
#define ActRenFolAsgCrs		(ActRcvRchTxtCrsLnk+ 76)
#define ActRcvFilAsgCrsDZ	(ActRcvRchTxtCrsLnk+ 77)
#define ActRcvFilAsgCrsCla	(ActRcvRchTxtCrsLnk+ 78)
#define ActExpAsgCrs		(ActRcvRchTxtCrsLnk+ 79)
#define ActConAsgCrs		(ActRcvRchTxtCrsLnk+ 80)
#define ActZIPAsgCrs		(ActRcvRchTxtCrsLnk+ 81)
#define ActReqDatAsgCrs		(ActRcvRchTxtCrsLnk+ 82)
#define ActChgDatAsgCrs		(ActRcvRchTxtCrsLnk+ 83)
#define ActDowAsgCrs		(ActRcvRchTxtCrsLnk+ 84)

#define ActReqRemFilWrkCrs	(ActRcvRchTxtCrsLnk+ 85)
#define ActRemFilWrkCrs		(ActRcvRchTxtCrsLnk+ 86)
#define ActRemFolWrkCrs		(ActRcvRchTxtCrsLnk+ 87)
#define ActCopWrkCrs		(ActRcvRchTxtCrsLnk+ 88)
#define ActPasWrkCrs		(ActRcvRchTxtCrsLnk+ 89)
#define ActRemTreWrkCrs		(ActRcvRchTxtCrsLnk+ 90)
#define ActFrmCreWrkCrs		(ActRcvRchTxtCrsLnk+ 91)
#define ActCreFolWrkCrs		(ActRcvRchTxtCrsLnk+ 92)
#define ActCreLnkWrkCrs		(ActRcvRchTxtCrsLnk+ 93)
#define ActRenFolWrkCrs		(ActRcvRchTxtCrsLnk+ 94)
#define ActRcvFilWrkCrsDZ	(ActRcvRchTxtCrsLnk+ 95)
#define ActRcvFilWrkCrsCla	(ActRcvRchTxtCrsLnk+ 96)
#define ActExpWrkCrs		(ActRcvRchTxtCrsLnk+ 97)
#define ActConWrkCrs		(ActRcvRchTxtCrsLnk+ 98)
#define ActZIPWrkCrs		(ActRcvRchTxtCrsLnk+ 99)
#define ActReqDatWrkCrs		(ActRcvRchTxtCrsLnk+100)
#define ActChgDatWrkCrs		(ActRcvRchTxtCrsLnk+101)
#define ActDowWrkCrs		(ActRcvRchTxtCrsLnk+102)

#define ActSeeTst		(ActRcvRchTxtCrsLnk+103)
#define ActAssTst		(ActRcvRchTxtCrsLnk+104)
#define ActEdiTstQst		(ActRcvRchTxtCrsLnk+105)
#define ActEdiOneTstQst		(ActRcvRchTxtCrsLnk+106)
#define ActReqImpTstQst		(ActRcvRchTxtCrsLnk+107)
#define ActImpTstQst		(ActRcvRchTxtCrsLnk+108)
#define ActLstTstQst		(ActRcvRchTxtCrsLnk+109)
#define ActRcvTstQst		(ActRcvRchTxtCrsLnk+110)
#define ActRemTstQst		(ActRcvRchTxtCrsLnk+111)
#define ActShfTstQst		(ActRcvRchTxtCrsLnk+112)
#define ActCfgTst		(ActRcvRchTxtCrsLnk+113)
#define ActEnableTag		(ActRcvRchTxtCrsLnk+114)
#define ActDisableTag		(ActRcvRchTxtCrsLnk+115)
#define ActRenTag		(ActRcvRchTxtCrsLnk+116)
#define ActRcvCfgTst		(ActRcvRchTxtCrsLnk+117)

#define ActReqSeeMyTstExa	(ActRcvRchTxtCrsLnk+118)
#define ActSeeMyTstExa		(ActRcvRchTxtCrsLnk+119)
#define ActSeeOneTstExaMe	(ActRcvRchTxtCrsLnk+120)
#define ActReqSeeUsrTstExa	(ActRcvRchTxtCrsLnk+121)
#define ActSeeUsrTstExa		(ActRcvRchTxtCrsLnk+122)
#define ActSeeOneTstExaOth	(ActRcvRchTxtCrsLnk+123)

#define ActPrnCal		(ActRcvRchTxtCrsLnk+124)

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
#define ActReqSelGrp		(ActChgNumRowFooGrp+ 1)
#define ActLstInv		(ActChgNumRowFooGrp+ 2)
#define ActLstStd		(ActChgNumRowFooGrp+ 3)
#define ActSeeAtt		(ActChgNumRowFooGrp+ 4)
#define ActLstTch		(ActChgNumRowFooGrp+ 5)
#define ActLstAdm		(ActChgNumRowFooGrp+ 6)
#define ActReqSignUp		(ActChgNumRowFooGrp+ 7)
#define ActSeeSignUpReq		(ActChgNumRowFooGrp+ 8)
#define ActReqMdfSevUsr		(ActChgNumRowFooGrp+ 9)
#define ActLstCon		(ActChgNumRowFooGrp+10)

// Secondary actions
#define ActChgGrp		(ActChgNumRowFooGrp+11)
#define ActReqEdiGrp		(ActChgNumRowFooGrp+12)
#define ActNewGrpTyp		(ActChgNumRowFooGrp+13)
#define ActReqRemGrpTyp		(ActChgNumRowFooGrp+14)
#define ActRemGrpTyp		(ActChgNumRowFooGrp+15)
#define ActRenGrpTyp		(ActChgNumRowFooGrp+16)
#define ActChgMdtGrpTyp		(ActChgNumRowFooGrp+17)
#define ActChgMulGrpTyp		(ActChgNumRowFooGrp+18)
#define ActChgTimGrpTyp		(ActChgNumRowFooGrp+19)
#define ActNewGrp		(ActChgNumRowFooGrp+20)
#define ActReqRemGrp		(ActChgNumRowFooGrp+21)
#define ActRemGrp		(ActChgNumRowFooGrp+22)
#define ActOpeGrp		(ActChgNumRowFooGrp+23)
#define ActCloGrp		(ActChgNumRowFooGrp+24)
#define ActEnaFilZonGrp		(ActChgNumRowFooGrp+25)
#define ActDisFilZonGrp		(ActChgNumRowFooGrp+26)
#define ActChgGrpTyp		(ActChgNumRowFooGrp+27)
#define ActRenGrp		(ActChgNumRowFooGrp+28)
#define ActChgMaxStdGrp		(ActChgNumRowFooGrp+29)

#define ActGetExtLstStd		(ActChgNumRowFooGrp+30)
#define ActPrnInvPho		(ActChgNumRowFooGrp+31)
#define ActPrnStdPho		(ActChgNumRowFooGrp+32)
#define ActPrnTchPho		(ActChgNumRowFooGrp+33)
#define ActLstInvAll		(ActChgNumRowFooGrp+34)
#define ActLstStdAll		(ActChgNumRowFooGrp+35)
#define ActLstTchAll		(ActChgNumRowFooGrp+36)
#define ActSeeRecOneStd		(ActChgNumRowFooGrp+37)
#define ActSeeRecOneTch		(ActChgNumRowFooGrp+38)
#define ActSeeRecSevInv		(ActChgNumRowFooGrp+39)
#define ActSeeRecSevStd		(ActChgNumRowFooGrp+40)
#define ActSeeRecSevTch		(ActChgNumRowFooGrp+41)
#define ActPrnRecSevInv		(ActChgNumRowFooGrp+42)
#define ActPrnRecSevStd		(ActChgNumRowFooGrp+43)
#define ActPrnRecSevTch		(ActChgNumRowFooGrp+44)
#define ActRcvRecOthUsr		(ActChgNumRowFooGrp+45)
#define ActEdiRecFie		(ActChgNumRowFooGrp+46)
#define ActNewFie		(ActChgNumRowFooGrp+47)
#define ActReqRemFie		(ActChgNumRowFooGrp+48)
#define ActRemFie		(ActChgNumRowFooGrp+49)
#define ActRenFie		(ActChgNumRowFooGrp+50)
#define ActChgRowFie		(ActChgNumRowFooGrp+51)
#define ActChgVisFie		(ActChgNumRowFooGrp+52)

#define ActReqLstAttStd		(ActChgNumRowFooGrp+53)
#define ActSeeLstAttStd		(ActChgNumRowFooGrp+54)
#define ActPrnLstAttStd		(ActChgNumRowFooGrp+55)
#define ActFrmNewAtt		(ActChgNumRowFooGrp+56)
#define ActEdiOneAtt		(ActChgNumRowFooGrp+57)
#define ActNewAtt		(ActChgNumRowFooGrp+58)
#define ActChgAtt		(ActChgNumRowFooGrp+59)
#define ActReqRemAtt		(ActChgNumRowFooGrp+60)
#define ActRemAtt		(ActChgNumRowFooGrp+61)
#define ActHidAtt		(ActChgNumRowFooGrp+62)
#define ActShoAtt		(ActChgNumRowFooGrp+63)
#define ActSeeOneAtt		(ActChgNumRowFooGrp+64)
#define ActRecAttStd		(ActChgNumRowFooGrp+65)
#define ActRecAttMe		(ActChgNumRowFooGrp+66)

#define ActSignUp		(ActChgNumRowFooGrp+67)
#define ActReqRejSignUp		(ActChgNumRowFooGrp+68)
#define ActRejSignUp		(ActChgNumRowFooGrp+69)

#define ActReqMdfOneUsr		(ActChgNumRowFooGrp+70)
#define ActReqMdfUsr		(ActChgNumRowFooGrp+71)

#define ActReqUsrPho		(ActChgNumRowFooGrp+72)
#define ActDetUsrPho		(ActChgNumRowFooGrp+73)
#define ActUpdUsrPho		(ActChgNumRowFooGrp+74)
#define ActRemUsrPho		(ActChgNumRowFooGrp+75)
#define ActCreOthUsrDat		(ActChgNumRowFooGrp+76)
#define ActUpdOthUsrDat		(ActChgNumRowFooGrp+77)

#define ActReqAccEnrCrs		(ActChgNumRowFooGrp+78)
#define ActAccEnrCrs		(ActChgNumRowFooGrp+79)
#define ActRemMeCrs		(ActChgNumRowFooGrp+80)

#define ActNewAdmDeg		(ActChgNumRowFooGrp+81)
#define ActRemAdmDeg		(ActChgNumRowFooGrp+82)

#define ActRcvFrmMdfUsrCrs	(ActChgNumRowFooGrp+83)
#define ActFrmIDsOthUsr		(ActChgNumRowFooGrp+84)
#define ActRemIDOth		(ActChgNumRowFooGrp+85)
#define ActNewIDOth		(ActChgNumRowFooGrp+86)
#define ActFrmPwdOthUsr		(ActChgNumRowFooGrp+87)
#define ActChgPwdOthUsr		(ActChgNumRowFooGrp+88)
#define ActRemUsrCrs		(ActChgNumRowFooGrp+89)
#define ActRemUsrGbl		(ActChgNumRowFooGrp+90)

#define ActReqRemAllStdCrs	(ActChgNumRowFooGrp+91)
#define ActRemAllStdCrs		(ActChgNumRowFooGrp+92)

#define ActReqRemOldUsr		(ActChgNumRowFooGrp+93)
#define ActRemOldUsr		(ActChgNumRowFooGrp+94)

/*****************************************************************************/
/******************************* Messages tab ********************************/
/*****************************************************************************/
// Actions in menu
#define ActSeeNtf		(ActRemOldUsr+  1)
#define ActSeeAnn		(ActRemOldUsr+  2)
#define ActSeeNot		(ActRemOldUsr+  3)
#define ActSeeFor		(ActRemOldUsr+  4)
#define ActSeeChtRms		(ActRemOldUsr+  5)
#define ActReqMsgUsr		(ActRemOldUsr+  6)
#define ActSeeRcvMsg		(ActRemOldUsr+  7)
#define ActSeeSntMsg		(ActRemOldUsr+  8)
#define ActMaiStd		(ActRemOldUsr+  9)
// Secondary actions
#define ActWriAnn		(ActRemOldUsr+ 10)
#define ActRcvAnn		(ActRemOldUsr+ 11)
#define ActRemAnn		(ActRemOldUsr+ 12)
#define ActShoNot		(ActRemOldUsr+ 13)
#define ActWriNot		(ActRemOldUsr+ 14)
#define ActRcvNot		(ActRemOldUsr+ 15)
#define ActHidNot		(ActRemOldUsr+ 16)
#define ActRevNot		(ActRemOldUsr+ 17)
#define ActRemNot		(ActRemOldUsr+ 18)
#define ActSeeNewNtf		(ActRemOldUsr+ 19)
#define ActMrkNtfSee		(ActRemOldUsr+ 20)
#define ActSeeForCrsUsr		(ActRemOldUsr+ 21)
#define ActSeeForCrsTch		(ActRemOldUsr+ 22)
#define ActSeeForDegUsr		(ActRemOldUsr+ 23)
#define ActSeeForDegTch		(ActRemOldUsr+ 24)
#define ActSeeForCtrUsr		(ActRemOldUsr+ 25)
#define ActSeeForCtrTch		(ActRemOldUsr+ 26)
#define ActSeeForInsUsr		(ActRemOldUsr+ 27)
#define ActSeeForInsTch		(ActRemOldUsr+ 28)
#define ActSeeForGenUsr		(ActRemOldUsr+ 29)
#define ActSeeForGenTch		(ActRemOldUsr+ 30)
#define ActSeeForSWAUsr		(ActRemOldUsr+ 31)
#define ActSeeForSWATch		(ActRemOldUsr+ 32)
#define ActSeePstForCrsUsr	(ActRemOldUsr+ 33)
#define ActSeePstForCrsTch	(ActRemOldUsr+ 34)
#define ActSeePstForDegUsr	(ActRemOldUsr+ 35)
#define ActSeePstForDegTch	(ActRemOldUsr+ 36)
#define ActSeePstForCtrUsr	(ActRemOldUsr+ 37)
#define ActSeePstForCtrTch	(ActRemOldUsr+ 38)
#define ActSeePstForInsUsr	(ActRemOldUsr+ 39)
#define ActSeePstForInsTch	(ActRemOldUsr+ 40)
#define ActSeePstForGenUsr	(ActRemOldUsr+ 41)
#define ActSeePstForGenTch	(ActRemOldUsr+ 42)
#define ActSeePstForSWAUsr	(ActRemOldUsr+ 43)
#define ActSeePstForSWATch	(ActRemOldUsr+ 44)
#define ActRcvThrForCrsUsr	(ActRemOldUsr+ 45)
#define ActRcvThrForCrsTch	(ActRemOldUsr+ 46)
#define ActRcvThrForDegUsr	(ActRemOldUsr+ 47)
#define ActRcvThrForDegTch	(ActRemOldUsr+ 48)
#define ActRcvThrForCtrUsr	(ActRemOldUsr+ 49)
#define ActRcvThrForCtrTch	(ActRemOldUsr+ 50)
#define ActRcvThrForInsUsr	(ActRemOldUsr+ 51)
#define ActRcvThrForInsTch	(ActRemOldUsr+ 52)
#define ActRcvThrForGenUsr	(ActRemOldUsr+ 53)
#define ActRcvThrForGenTch	(ActRemOldUsr+ 54)
#define ActRcvThrForSWAUsr	(ActRemOldUsr+ 55)
#define ActRcvThrForSWATch	(ActRemOldUsr+ 56)
#define ActRcvRepForCrsUsr	(ActRemOldUsr+ 57)
#define ActRcvRepForCrsTch	(ActRemOldUsr+ 58)
#define ActRcvRepForDegUsr	(ActRemOldUsr+ 59)
#define ActRcvRepForDegTch	(ActRemOldUsr+ 60)
#define ActRcvRepForCtrUsr	(ActRemOldUsr+ 61)
#define ActRcvRepForCtrTch	(ActRemOldUsr+ 62)
#define ActRcvRepForInsUsr	(ActRemOldUsr+ 63)
#define ActRcvRepForInsTch	(ActRemOldUsr+ 64)
#define ActRcvRepForGenUsr	(ActRemOldUsr+ 65)
#define ActRcvRepForGenTch	(ActRemOldUsr+ 66)
#define ActRcvRepForSWAUsr	(ActRemOldUsr+ 67)
#define ActRcvRepForSWATch	(ActRemOldUsr+ 68)
#define ActReqDelThrCrsUsr	(ActRemOldUsr+ 69)
#define ActReqDelThrCrsTch	(ActRemOldUsr+ 70)
#define ActReqDelThrDegUsr	(ActRemOldUsr+ 71)
#define ActReqDelThrDegTch	(ActRemOldUsr+ 72)
#define ActReqDelThrCtrUsr	(ActRemOldUsr+ 73)
#define ActReqDelThrCtrTch	(ActRemOldUsr+ 74)
#define ActReqDelThrInsUsr	(ActRemOldUsr+ 75)
#define ActReqDelThrInsTch	(ActRemOldUsr+ 76)
#define ActReqDelThrGenUsr	(ActRemOldUsr+ 77)
#define ActReqDelThrGenTch	(ActRemOldUsr+ 78)
#define ActReqDelThrSWAUsr	(ActRemOldUsr+ 79)
#define ActReqDelThrSWATch	(ActRemOldUsr+ 80)
#define ActDelThrForCrsUsr	(ActRemOldUsr+ 81)
#define ActDelThrForCrsTch	(ActRemOldUsr+ 82)
#define ActDelThrForDegUsr	(ActRemOldUsr+ 83)
#define ActDelThrForDegTch	(ActRemOldUsr+ 84)
#define ActDelThrForCtrUsr	(ActRemOldUsr+ 85)
#define ActDelThrForCtrTch	(ActRemOldUsr+ 86)
#define ActDelThrForInsUsr	(ActRemOldUsr+ 87)
#define ActDelThrForInsTch	(ActRemOldUsr+ 88)
#define ActDelThrForGenUsr	(ActRemOldUsr+ 89)
#define ActDelThrForGenTch	(ActRemOldUsr+ 90)
#define ActDelThrForSWAUsr	(ActRemOldUsr+ 91)
#define ActDelThrForSWATch	(ActRemOldUsr+ 92)
#define ActCutThrForCrsUsr	(ActRemOldUsr+ 93)
#define ActCutThrForCrsTch	(ActRemOldUsr+ 94)
#define ActCutThrForDegUsr	(ActRemOldUsr+ 95)
#define ActCutThrForDegTch	(ActRemOldUsr+ 96)
#define ActCutThrForCtrUsr	(ActRemOldUsr+ 97)
#define ActCutThrForCtrTch	(ActRemOldUsr+ 98)
#define ActCutThrForInsUsr	(ActRemOldUsr+ 99)
#define ActCutThrForInsTch	(ActRemOldUsr+100)
#define ActCutThrForGenUsr	(ActRemOldUsr+101)
#define ActCutThrForGenTch	(ActRemOldUsr+102)
#define ActCutThrForSWAUsr	(ActRemOldUsr+103)
#define ActCutThrForSWATch	(ActRemOldUsr+104)
#define ActPasThrForCrsUsr	(ActRemOldUsr+105)
#define ActPasThrForCrsTch	(ActRemOldUsr+106)
#define ActPasThrForDegUsr	(ActRemOldUsr+107)
#define ActPasThrForDegTch	(ActRemOldUsr+108)
#define ActPasThrForCtrUsr	(ActRemOldUsr+109)
#define ActPasThrForCtrTch	(ActRemOldUsr+110)
#define ActPasThrForInsUsr	(ActRemOldUsr+111)
#define ActPasThrForInsTch	(ActRemOldUsr+112)
#define ActPasThrForGenUsr	(ActRemOldUsr+113)
#define ActPasThrForGenTch	(ActRemOldUsr+114)
#define ActPasThrForSWAUsr	(ActRemOldUsr+115)
#define ActPasThrForSWATch	(ActRemOldUsr+116)
#define ActDelPstForCrsUsr	(ActRemOldUsr+117)
#define ActDelPstForCrsTch	(ActRemOldUsr+118)
#define ActDelPstForDegUsr	(ActRemOldUsr+119)
#define ActDelPstForDegTch	(ActRemOldUsr+120)
#define ActDelPstForCtrUsr	(ActRemOldUsr+121)
#define ActDelPstForCtrTch	(ActRemOldUsr+122)
#define ActDelPstForInsUsr	(ActRemOldUsr+123)
#define ActDelPstForInsTch	(ActRemOldUsr+124)
#define ActDelPstForGenUsr	(ActRemOldUsr+125)
#define ActDelPstForGenTch	(ActRemOldUsr+126)
#define ActDelPstForSWAUsr	(ActRemOldUsr+127)
#define ActDelPstForSWATch	(ActRemOldUsr+128)
#define ActEnbPstForCrsUsr	(ActRemOldUsr+129)
#define ActEnbPstForCrsTch	(ActRemOldUsr+130)
#define ActEnbPstForDegUsr	(ActRemOldUsr+131)
#define ActEnbPstForDegTch	(ActRemOldUsr+132)
#define ActEnbPstForCtrUsr	(ActRemOldUsr+133)
#define ActEnbPstForCtrTch	(ActRemOldUsr+134)
#define ActEnbPstForInsUsr	(ActRemOldUsr+135)
#define ActEnbPstForInsTch	(ActRemOldUsr+136)
#define ActEnbPstForGenUsr	(ActRemOldUsr+137)
#define ActEnbPstForGenTch	(ActRemOldUsr+138)
#define ActEnbPstForSWAUsr	(ActRemOldUsr+139)
#define ActEnbPstForSWATch	(ActRemOldUsr+140)
#define ActDisPstForCrsUsr	(ActRemOldUsr+141)
#define ActDisPstForCrsTch	(ActRemOldUsr+142)
#define ActDisPstForDegUsr	(ActRemOldUsr+143)
#define ActDisPstForDegTch	(ActRemOldUsr+144)
#define ActDisPstForCtrUsr	(ActRemOldUsr+145)
#define ActDisPstForCtrTch	(ActRemOldUsr+146)
#define ActDisPstForInsUsr	(ActRemOldUsr+147)
#define ActDisPstForInsTch	(ActRemOldUsr+148)
#define ActDisPstForGenUsr	(ActRemOldUsr+149)
#define ActDisPstForGenTch	(ActRemOldUsr+150)
#define ActDisPstForSWAUsr	(ActRemOldUsr+151)
#define ActDisPstForSWATch	(ActRemOldUsr+152)
#define ActRcvMsgUsr		(ActRemOldUsr+153)
#define ActReqDelAllSntMsg	(ActRemOldUsr+154)
#define ActReqDelAllRcvMsg	(ActRemOldUsr+155)
#define ActDelAllSntMsg		(ActRemOldUsr+156)
#define ActDelAllRcvMsg		(ActRemOldUsr+157)
#define ActDelSntMsg		(ActRemOldUsr+158)
#define ActDelRcvMsg		(ActRemOldUsr+159)
#define ActExpSntMsg		(ActRemOldUsr+160)
#define ActExpRcvMsg		(ActRemOldUsr+161)
#define ActConSntMsg		(ActRemOldUsr+162)
#define ActConRcvMsg		(ActRemOldUsr+163)
#define ActLstBanUsr		(ActRemOldUsr+164)
#define ActBanUsrMsg		(ActRemOldUsr+165)
#define ActUnbUsrMsg		(ActRemOldUsr+166)
#define ActUnbUsrLst		(ActRemOldUsr+167)
#define ActCht			(ActRemOldUsr+168)

/*****************************************************************************/
/****************************** Statistics tab *******************************/
/*****************************************************************************/
// Actions in menu
#define ActSeeAllSvy		(ActCht+ 1)
#define ActReqUseGbl		(ActCht+ 2)
#define ActSeePhoDeg		(ActCht+ 3)
#define ActReqStaCrs		(ActCht+ 4)
#define ActReqAccGbl		(ActCht+ 5)
#define ActLstClk		(ActCht+ 6)
// Secondary actions
#define ActSeeOneSvy		(ActCht+ 7)
#define ActAnsSvy		(ActCht+ 8)
#define ActFrmNewSvy		(ActCht+ 9)
#define ActEdiOneSvy		(ActCht+10)
#define ActNewSvy		(ActCht+11)
#define ActChgSvy		(ActCht+12)
#define ActReqRemSvy		(ActCht+13)
#define ActRemSvy		(ActCht+14)
#define ActReqRstSvy		(ActCht+15)
#define ActRstSvy		(ActCht+16)
#define ActHidSvy		(ActCht+17)
#define ActShoSvy		(ActCht+18)
#define ActEdiOneSvyQst		(ActCht+19)
#define ActRcvSvyQst		(ActCht+20)
#define ActRemSvyQst		(ActCht+21)

#define ActSeeUseGbl		(ActCht+22)
#define ActPrnPhoDeg		(ActCht+23)
#define ActCalPhoDeg		(ActCht+24)
#define ActSeeAccGbl		(ActCht+25)
#define ActReqAccCrs		(ActCht+26)
#define ActSeeAccCrs		(ActCht+27)
#define ActSeeAllStaCrs		(ActCht+28)

/*****************************************************************************/
/******************************** Profile tab ********************************/
/*****************************************************************************/
// Actions in menu
#define ActFrmLogIn		(ActSeeAllStaCrs+ 1)
#define ActMyCrs		(ActSeeAllStaCrs+ 2)
#define ActSeeMyTimTbl		(ActSeeAllStaCrs+ 3)
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
#define ActRemIDMe		(ActSeeAllStaCrs+18)
#define ActNewIDMe		(ActSeeAllStaCrs+19)
#define ActRemOldNic		(ActSeeAllStaCrs+20)
#define ActChgNic		(ActSeeAllStaCrs+21)
#define ActRemOldMai		(ActSeeAllStaCrs+22)
#define ActChgMai		(ActSeeAllStaCrs+23)
#define ActCnfMai		(ActSeeAllStaCrs+24)
#define ActFrmChgMyPwd		(ActSeeAllStaCrs+25)
#define ActChgPwd		(ActSeeAllStaCrs+26)

#define ActChgMyData		(ActSeeAllStaCrs+27)

#define ActReqMyPho		(ActSeeAllStaCrs+28)
#define ActDetMyPho		(ActSeeAllStaCrs+29)
#define ActUpdMyPho		(ActSeeAllStaCrs+30)
#define ActRemMyPho		(ActSeeAllStaCrs+31)
#define ActChgPubPho		(ActSeeAllStaCrs+32)

#define ActReqEdiMyIns		(ActSeeAllStaCrs+33)
#define ActChgCtyMyIns		(ActSeeAllStaCrs+34)
#define ActChgMyIns		(ActSeeAllStaCrs+35)
#define ActChgMyCtr		(ActSeeAllStaCrs+36)
#define ActChgMyDpt		(ActSeeAllStaCrs+37)
#define ActChgMyOff		(ActSeeAllStaCrs+38)
#define ActChgMyOffPho		(ActSeeAllStaCrs+39)

#define ActReqEdiMyNet		(ActSeeAllStaCrs+40)
#define ActChgMyNet		(ActSeeAllStaCrs+41)

#define ActChgLay		(ActSeeAllStaCrs+42)
#define ActChgThe		(ActSeeAllStaCrs+43)
#define ActReqChgLan		(ActSeeAllStaCrs+44)
#define ActChgLan		(ActSeeAllStaCrs+45)
#define ActChgCol		(ActSeeAllStaCrs+46)
#define ActHidLftCol		(ActSeeAllStaCrs+47)
#define ActHidRgtCol		(ActSeeAllStaCrs+48)
#define ActShoLftCol		(ActSeeAllStaCrs+49)
#define ActShoRgtCol		(ActSeeAllStaCrs+50)
#define ActChgIco		(ActSeeAllStaCrs+51)
#define ActChgMnu		(ActSeeAllStaCrs+52)
#define ActChgNtfPrf		(ActSeeAllStaCrs+53)

#define ActPrnUsrQR		(ActSeeAllStaCrs+54)

#define ActPrnMyTimTbl		(ActSeeAllStaCrs+55)
#define ActEdiTut		(ActSeeAllStaCrs+56)
#define ActChgTut		(ActSeeAllStaCrs+57)
#define ActReqRemFilBrf		(ActSeeAllStaCrs+58)
#define ActRemFilBrf		(ActSeeAllStaCrs+59)
#define ActRemFolBrf		(ActSeeAllStaCrs+60)
#define ActCopBrf		(ActSeeAllStaCrs+61)
#define ActPasBrf		(ActSeeAllStaCrs+62)
#define ActRemTreBrf		(ActSeeAllStaCrs+63)
#define ActFrmCreBrf		(ActSeeAllStaCrs+64)
#define ActCreFolBrf		(ActSeeAllStaCrs+65)
#define ActCreLnkBrf		(ActSeeAllStaCrs+66)
#define ActRenFolBrf		(ActSeeAllStaCrs+67)
#define ActRcvFilBrfDZ		(ActSeeAllStaCrs+68)
#define ActRcvFilBrfCla		(ActSeeAllStaCrs+69)
#define ActExpBrf		(ActSeeAllStaCrs+70)
#define ActConBrf		(ActSeeAllStaCrs+71)
#define ActZIPBrf		(ActSeeAllStaCrs+72)
#define ActReqDatBrf		(ActSeeAllStaCrs+73)
#define ActChgDatBrf		(ActSeeAllStaCrs+74)
#define ActDowBrf		(ActSeeAllStaCrs+75)

/*****************************************************************************/
/******************************** Public types *******************************/
/*****************************************************************************/

struct Act_Tabs
  {
   char *Icon;
  };
struct Act_Menu
  {
   Act_Action_t Action;
   bool SubsequentSeparation;
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
void Act_FormStartId (Act_Action_t NextAction,const char *Id);
void Act_LinkFormSubmit (const char *Title,const char *LinkStyle);
void Act_LinkFormSubmitId (const char *Title,const char *LinkStyle,const char *Id);
void Act_AdjustActionWhenNoUsrLogged (void);
void Act_AdjustCurrentAction (void);
Act_Action_t Act_GetFirstActionAvailableInCurrentTab (void);

void Act_AllocateMFUActions (struct Act_ListMFUActions *ListMFUActions,unsigned MaxActionsShown);
void Act_FreeMFUActions (struct Act_ListMFUActions *ListMFUActions);
void Act_GetMFUActions (struct Act_ListMFUActions *ListMFUActions,unsigned MaxActionsShown);
Act_Action_t Act_GetMyLastActionInCurrentTab (void);
void Act_ShowMyMFUActions (void);
void Act_WriteBigMFUActions (struct Act_ListMFUActions *ListMFUActions);
void Act_WriteSmallMFUActions (struct Act_ListMFUActions *ListMFUActions);
void Act_UpdateMFUActions (void);

#endif
