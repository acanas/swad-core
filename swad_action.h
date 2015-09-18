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

#define Act_NUM_ACTIONS	(7+52+15+90+73+68+204+182+142+168+28+79)

#define Act_MAX_ACTION_COD 1469

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
#define ActChgDegOptYea		(ActDowComIns+ 24)
#define ActChgDegWWW		(ActDowComIns+ 25)
#define ActChgDegSta		(ActDowComIns+ 26)

#define ActChgToSeeDocCtr	(ActDowComIns+ 27)
#define ActSeeDocCtr		(ActDowComIns+ 28)
#define ActExpSeeDocCtr		(ActDowComIns+ 29)
#define ActConSeeDocCtr		(ActDowComIns+ 30)
#define ActZIPSeeDocCtr		(ActDowComIns+ 31)
#define ActReqDatSeeDocCtr	(ActDowComIns+ 32)
#define ActDowSeeDocCtr		(ActDowComIns+ 33)
#define ActChgToAdmDocCtr	(ActDowComIns+ 34)
#define ActAdmDocCtr		(ActDowComIns+ 35)
#define ActReqRemFilDocCtr	(ActDowComIns+ 36)
#define ActRemFilDocCtr		(ActDowComIns+ 37)
#define ActRemFolDocCtr		(ActDowComIns+ 38)
#define ActCopDocCtr		(ActDowComIns+ 39)
#define ActPasDocCtr		(ActDowComIns+ 40)
#define ActRemTreDocCtr		(ActDowComIns+ 41)
#define ActFrmCreDocCtr		(ActDowComIns+ 42)
#define ActCreFolDocCtr		(ActDowComIns+ 43)
#define ActCreLnkDocCtr		(ActDowComIns+ 44)
#define ActRenFolDocCtr		(ActDowComIns+ 45)
#define ActRcvFilDocCtrDZ	(ActDowComIns+ 46)
#define ActRcvFilDocCtrCla	(ActDowComIns+ 47)
#define ActExpAdmDocCtr		(ActDowComIns+ 48)
#define ActConAdmDocCtr		(ActDowComIns+ 49)
#define ActZIPAdmDocCtr		(ActDowComIns+ 50)
#define ActShoDocCtr		(ActDowComIns+ 51)
#define ActHidDocCtr		(ActDowComIns+ 52)
#define ActReqDatAdmDocCtr	(ActDowComIns+ 53)
#define ActChgDatAdmDocCtr	(ActDowComIns+ 54)
#define ActDowAdmDocCtr		(ActDowComIns+ 55)

#define ActReqRemFilComCtr	(ActDowComIns+ 56)
#define ActRemFilComCtr		(ActDowComIns+ 57)
#define ActRemFolComCtr		(ActDowComIns+ 58)
#define ActCopComCtr		(ActDowComIns+ 59)
#define ActPasComCtr		(ActDowComIns+ 60)
#define ActRemTreComCtr		(ActDowComIns+ 61)
#define ActFrmCreComCtr		(ActDowComIns+ 62)
#define ActCreFolComCtr		(ActDowComIns+ 63)
#define ActCreLnkComCtr		(ActDowComIns+ 64)
#define ActRenFolComCtr		(ActDowComIns+ 65)
#define ActRcvFilComCtrDZ	(ActDowComIns+ 66)
#define ActRcvFilComCtrCla	(ActDowComIns+ 67)
#define ActExpComCtr		(ActDowComIns+ 68)
#define ActConComCtr		(ActDowComIns+ 69)
#define ActZIPComCtr		(ActDowComIns+ 70)
#define ActReqDatComCtr		(ActDowComIns+ 71)
#define ActChgDatComCtr		(ActDowComIns+ 72)
#define ActDowComCtr		(ActDowComIns+ 73)

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
#define ActChgCrsSem		(ActDowComCtr+ 18)
#define ActRenCrsSho		(ActDowComCtr+ 19)
#define ActRenCrsFul		(ActDowComCtr+ 20)
#define ActChgCrsSta		(ActDowComCtr+ 21)

#define ActChgToSeeDocDeg	(ActDowComCtr+ 22)
#define ActSeeDocDeg		(ActDowComCtr+ 23)
#define ActExpSeeDocDeg		(ActDowComCtr+ 24)
#define ActConSeeDocDeg		(ActDowComCtr+ 25)
#define ActZIPSeeDocDeg		(ActDowComCtr+ 26)
#define ActReqDatSeeDocDeg	(ActDowComCtr+ 27)
#define ActDowSeeDocDeg		(ActDowComCtr+ 28)
#define ActChgToAdmDocDeg	(ActDowComCtr+ 29)
#define ActAdmDocDeg		(ActDowComCtr+ 30)
#define ActReqRemFilDocDeg	(ActDowComCtr+ 31)
#define ActRemFilDocDeg		(ActDowComCtr+ 32)
#define ActRemFolDocDeg		(ActDowComCtr+ 33)
#define ActCopDocDeg		(ActDowComCtr+ 34)
#define ActPasDocDeg		(ActDowComCtr+ 35)
#define ActRemTreDocDeg		(ActDowComCtr+ 36)
#define ActFrmCreDocDeg		(ActDowComCtr+ 37)
#define ActCreFolDocDeg		(ActDowComCtr+ 38)
#define ActCreLnkDocDeg		(ActDowComCtr+ 39)
#define ActRenFolDocDeg		(ActDowComCtr+ 40)
#define ActRcvFilDocDegDZ	(ActDowComCtr+ 41)
#define ActRcvFilDocDegCla	(ActDowComCtr+ 42)
#define ActExpAdmDocDeg		(ActDowComCtr+ 43)
#define ActConAdmDocDeg		(ActDowComCtr+ 44)
#define ActZIPAdmDocDeg		(ActDowComCtr+ 45)
#define ActShoDocDeg		(ActDowComCtr+ 46)
#define ActHidDocDeg		(ActDowComCtr+ 47)
#define ActReqDatAdmDocDeg	(ActDowComCtr+ 48)
#define ActChgDatAdmDocDeg	(ActDowComCtr+ 49)
#define ActDowAdmDocDeg		(ActDowComCtr+ 50)

#define ActReqRemFilComDeg	(ActDowComCtr+ 51)
#define ActRemFilComDeg		(ActDowComCtr+ 52)
#define ActRemFolComDeg		(ActDowComCtr+ 53)
#define ActCopComDeg		(ActDowComCtr+ 54)
#define ActPasComDeg		(ActDowComCtr+ 55)
#define ActRemTreComDeg		(ActDowComCtr+ 56)
#define ActFrmCreComDeg		(ActDowComCtr+ 57)
#define ActCreFolComDeg		(ActDowComCtr+ 58)
#define ActCreLnkComDeg		(ActDowComCtr+ 59)
#define ActRenFolComDeg		(ActDowComCtr+ 60)
#define ActRcvFilComDegDZ	(ActDowComCtr+ 61)
#define ActRcvFilComDegCla	(ActDowComCtr+ 62)
#define ActExpComDeg		(ActDowComCtr+ 63)
#define ActConComDeg		(ActDowComCtr+ 64)
#define ActZIPComDeg		(ActDowComCtr+ 65)
#define ActReqDatComDeg		(ActDowComCtr+ 66)
#define ActChgDatComDeg		(ActDowComCtr+ 67)
#define ActDowComDeg		(ActDowComCtr+ 68)

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
#define ActSeeCrsTimTbl		(ActDowComDeg+  7)
#define ActSeeBib		(ActDowComDeg+  8)
#define ActSeeFAQ		(ActDowComDeg+  9)
#define ActSeeCrsLnk		(ActDowComDeg+ 10)
// Secondary actions
#define ActCrsSch		(ActDowComDeg+ 11)
#define ActPrnCrsInf		(ActDowComDeg+ 12)
#define ActChgCrsLog		(ActDowComDeg+ 13)
#define ActEdiCrsInf		(ActDowComDeg+ 14)
#define ActEdiTchGui		(ActDowComDeg+ 15)
#define ActPrnCrsTimTbl		(ActDowComDeg+ 16)
#define ActEdiCrsTimTbl		(ActDowComDeg+ 17)
#define ActChgCrsTimTbl		(ActDowComDeg+ 18)
#define ActSeeSylLec		(ActDowComDeg+ 19)
#define ActSeeSylPra		(ActDowComDeg+ 20)
#define ActEdiSylLec		(ActDowComDeg+ 21)
#define ActEdiSylPra		(ActDowComDeg+ 22)
#define ActDelItmSylLec		(ActDowComDeg+ 23)
#define ActDelItmSylPra		(ActDowComDeg+ 24)
#define ActUp_IteSylLec		(ActDowComDeg+ 25)
#define ActUp_IteSylPra		(ActDowComDeg+ 26)
#define ActDwnIteSylLec		(ActDowComDeg+ 27)
#define ActDwnIteSylPra		(ActDowComDeg+ 28)
#define ActRgtIteSylLec		(ActDowComDeg+ 29)
#define ActRgtIteSylPra		(ActDowComDeg+ 30)
#define ActLftIteSylLec		(ActDowComDeg+ 31)
#define ActLftIteSylPra		(ActDowComDeg+ 32)
#define ActInsIteSylLec		(ActDowComDeg+ 33)
#define ActInsIteSylPra		(ActDowComDeg+ 34)
#define ActModIteSylLec		(ActDowComDeg+ 35)
#define ActModIteSylPra		(ActDowComDeg+ 36)

#define ActChgToSeeDocCrs	(ActDowComDeg+ 37)
#define ActSeeDocCrs		(ActDowComDeg+ 38)
#define ActExpSeeDocCrs		(ActDowComDeg+ 39)
#define ActConSeeDocCrs		(ActDowComDeg+ 40)
#define ActZIPSeeDocCrs		(ActDowComDeg+ 41)
#define ActReqDatSeeDocCrs	(ActDowComDeg+ 42)
#define ActDowSeeDocCrs		(ActDowComDeg+ 43)

#define ActSeeDocGrp		(ActDowComDeg+ 44)
#define ActExpSeeDocGrp		(ActDowComDeg+ 45)
#define ActConSeeDocGrp		(ActDowComDeg+ 46)
#define ActZIPSeeDocGrp		(ActDowComDeg+ 47)
#define ActReqDatSeeDocGrp	(ActDowComDeg+ 48)
#define ActDowSeeDocGrp		(ActDowComDeg+ 49)

#define ActChgToAdmDocCrs	(ActDowComDeg+ 50)
#define ActAdmDocCrs		(ActDowComDeg+ 51)
#define ActReqRemFilDocCrs	(ActDowComDeg+ 52)
#define ActRemFilDocCrs		(ActDowComDeg+ 53)
#define ActRemFolDocCrs		(ActDowComDeg+ 54)
#define ActCopDocCrs		(ActDowComDeg+ 55)
#define ActPasDocCrs		(ActDowComDeg+ 56)
#define ActRemTreDocCrs		(ActDowComDeg+ 57)
#define ActFrmCreDocCrs		(ActDowComDeg+ 58)
#define ActCreFolDocCrs		(ActDowComDeg+ 59)
#define ActCreLnkDocCrs		(ActDowComDeg+ 60)
#define ActRenFolDocCrs		(ActDowComDeg+ 61)
#define ActRcvFilDocCrsDZ	(ActDowComDeg+ 62)
#define ActRcvFilDocCrsCla	(ActDowComDeg+ 63)
#define ActExpAdmDocCrs		(ActDowComDeg+ 64)
#define ActConAdmDocCrs		(ActDowComDeg+ 65)
#define ActZIPAdmDocCrs		(ActDowComDeg+ 66)
#define ActShoDocCrs		(ActDowComDeg+ 67)
#define ActHidDocCrs		(ActDowComDeg+ 68)
#define ActReqDatAdmDocCrs	(ActDowComDeg+ 69)
#define ActChgDatAdmDocCrs	(ActDowComDeg+ 70)
#define ActDowAdmDocCrs		(ActDowComDeg+ 71)

#define ActAdmDocGrp		(ActDowComDeg+ 72)
#define ActReqRemFilDocGrp	(ActDowComDeg+ 73)
#define ActRemFilDocGrp		(ActDowComDeg+ 74)
#define ActRemFolDocGrp		(ActDowComDeg+ 75)
#define ActCopDocGrp		(ActDowComDeg+ 76)
#define ActPasDocGrp		(ActDowComDeg+ 77)
#define ActRemTreDocGrp		(ActDowComDeg+ 78)
#define ActFrmCreDocGrp		(ActDowComDeg+ 79)
#define ActCreFolDocGrp		(ActDowComDeg+ 80)
#define ActCreLnkDocGrp		(ActDowComDeg+ 81)
#define ActRenFolDocGrp		(ActDowComDeg+ 82)
#define ActRcvFilDocGrpDZ	(ActDowComDeg+ 83)
#define ActRcvFilDocGrpCla	(ActDowComDeg+ 84)
#define ActExpAdmDocGrp		(ActDowComDeg+ 85)
#define ActConAdmDocGrp		(ActDowComDeg+ 86)
#define ActZIPAdmDocGrp		(ActDowComDeg+ 87)
#define ActShoDocGrp		(ActDowComDeg+ 88)
#define ActHidDocGrp		(ActDowComDeg+ 89)
#define ActReqDatAdmDocGrp	(ActDowComDeg+ 90)
#define ActChgDatAdmDocGrp	(ActDowComDeg+ 91)
#define ActDowAdmDocGrp		(ActDowComDeg+ 92)

#define ActChgToAdmCom		(ActDowComDeg+ 93)

#define ActAdmComCrs		(ActDowComDeg+ 94)
#define ActReqRemFilComCrs	(ActDowComDeg+ 95)
#define ActRemFilComCrs		(ActDowComDeg+ 96)
#define ActRemFolComCrs		(ActDowComDeg+ 97)
#define ActCopComCrs		(ActDowComDeg+ 98)
#define ActPasComCrs		(ActDowComDeg+ 99)
#define ActRemTreComCrs		(ActDowComDeg+100)
#define ActFrmCreComCrs		(ActDowComDeg+101)
#define ActCreFolComCrs		(ActDowComDeg+102)
#define ActCreLnkComCrs		(ActDowComDeg+103)
#define ActRenFolComCrs		(ActDowComDeg+104)
#define ActRcvFilComCrsDZ	(ActDowComDeg+105)
#define ActRcvFilComCrsCla	(ActDowComDeg+106)
#define ActExpComCrs		(ActDowComDeg+107)
#define ActConComCrs		(ActDowComDeg+108)
#define ActZIPComCrs		(ActDowComDeg+109)
#define ActReqDatComCrs		(ActDowComDeg+110)
#define ActChgDatComCrs		(ActDowComDeg+111)
#define ActDowComCrs		(ActDowComDeg+112)

#define ActAdmComGrp		(ActDowComDeg+113)
#define ActReqRemFilComGrp	(ActDowComDeg+114)
#define ActRemFilComGrp		(ActDowComDeg+115)
#define ActRemFolComGrp		(ActDowComDeg+116)
#define ActCopComGrp		(ActDowComDeg+117)
#define ActPasComGrp		(ActDowComDeg+118)
#define ActRemTreComGrp		(ActDowComDeg+119)
#define ActFrmCreComGrp		(ActDowComDeg+120)
#define ActCreFolComGrp		(ActDowComDeg+121)
#define ActCreLnkComGrp		(ActDowComDeg+122)
#define ActRenFolComGrp		(ActDowComDeg+123)
#define ActRcvFilComGrpDZ	(ActDowComDeg+124)
#define ActRcvFilComGrpCla	(ActDowComDeg+125)
#define ActExpComGrp		(ActDowComDeg+126)
#define ActConComGrp		(ActDowComDeg+127)
#define ActZIPComGrp		(ActDowComDeg+128)
#define ActReqDatComGrp		(ActDowComDeg+129)
#define ActChgDatComGrp		(ActDowComDeg+130)
#define ActDowComGrp		(ActDowComDeg+131)

#define ActEdiBib		(ActDowComDeg+132)
#define ActEdiFAQ		(ActDowComDeg+133)
#define ActEdiCrsLnk		(ActDowComDeg+134)

#define ActChgFrcReaCrsInf	(ActDowComDeg+135)
#define ActChgFrcReaTchGui	(ActDowComDeg+136)
#define ActChgFrcReaSylLec	(ActDowComDeg+137)
#define ActChgFrcReaSylPra	(ActDowComDeg+138)
#define ActChgFrcReaBib		(ActDowComDeg+139)
#define ActChgFrcReaFAQ		(ActDowComDeg+140)
#define ActChgFrcReaCrsLnk	(ActDowComDeg+141)

#define ActChgHavReaCrsInf	(ActDowComDeg+142)
#define ActChgHavReaTchGui	(ActDowComDeg+143)
#define ActChgHavReaSylLec	(ActDowComDeg+144)
#define ActChgHavReaSylPra	(ActDowComDeg+145)
#define ActChgHavReaBib		(ActDowComDeg+146)
#define ActChgHavReaFAQ		(ActDowComDeg+147)
#define ActChgHavReaCrsLnk	(ActDowComDeg+148)

#define ActSelInfSrcCrsInf	(ActDowComDeg+149)
#define ActSelInfSrcTchGui	(ActDowComDeg+150)
#define ActSelInfSrcSylLec	(ActDowComDeg+151)
#define ActSelInfSrcSylPra	(ActDowComDeg+152)
#define ActSelInfSrcBib		(ActDowComDeg+153)
#define ActSelInfSrcFAQ		(ActDowComDeg+154)
#define ActSelInfSrcCrsLnk	(ActDowComDeg+155)
#define ActRcvURLCrsInf		(ActDowComDeg+156)
#define ActRcvURLTchGui		(ActDowComDeg+157)
#define ActRcvURLSylLec		(ActDowComDeg+158)
#define ActRcvURLSylPra		(ActDowComDeg+159)
#define ActRcvURLBib		(ActDowComDeg+160)
#define ActRcvURLFAQ		(ActDowComDeg+161)
#define ActRcvURLCrsLnk		(ActDowComDeg+162)
#define ActRcvPagCrsInf		(ActDowComDeg+163)
#define ActRcvPagTchGui		(ActDowComDeg+164)
#define ActRcvPagSylLec		(ActDowComDeg+165)
#define ActRcvPagSylPra		(ActDowComDeg+166)
#define ActRcvPagBib		(ActDowComDeg+167)
#define ActRcvPagFAQ		(ActDowComDeg+168)
#define ActRcvPagCrsLnk		(ActDowComDeg+169)
#define ActEditorCrsInf		(ActDowComDeg+170)
#define ActEditorTchGui		(ActDowComDeg+171)
#define ActEditorSylLec		(ActDowComDeg+172)
#define ActEditorSylPra		(ActDowComDeg+173)
#define ActEditorBib		(ActDowComDeg+174)
#define ActEditorFAQ		(ActDowComDeg+175)
#define ActEditorCrsLnk		(ActDowComDeg+176)
#define ActPlaTxtEdiCrsInf	(ActDowComDeg+177)
#define ActPlaTxtEdiTchGui	(ActDowComDeg+178)
#define ActPlaTxtEdiSylLec	(ActDowComDeg+179)
#define ActPlaTxtEdiSylPra	(ActDowComDeg+180)
#define ActPlaTxtEdiBib		(ActDowComDeg+181)
#define ActPlaTxtEdiFAQ		(ActDowComDeg+182)
#define ActPlaTxtEdiCrsLnk	(ActDowComDeg+183)
#define ActRchTxtEdiCrsInf	(ActDowComDeg+184)
#define ActRchTxtEdiTchGui	(ActDowComDeg+185)
#define ActRchTxtEdiSylLec	(ActDowComDeg+186)
#define ActRchTxtEdiSylPra	(ActDowComDeg+187)
#define ActRchTxtEdiBib		(ActDowComDeg+188)
#define ActRchTxtEdiFAQ		(ActDowComDeg+189)
#define ActRchTxtEdiCrsLnk	(ActDowComDeg+190)
#define ActRcvPlaTxtCrsInf	(ActDowComDeg+191)
#define ActRcvPlaTxtTchGui	(ActDowComDeg+192)
#define ActRcvPlaTxtSylLec	(ActDowComDeg+193)
#define ActRcvPlaTxtSylPra	(ActDowComDeg+194)
#define ActRcvPlaTxtBib		(ActDowComDeg+195)
#define ActRcvPlaTxtFAQ		(ActDowComDeg+196)
#define ActRcvPlaTxtCrsLnk	(ActDowComDeg+197)
#define ActRcvRchTxtCrsInf	(ActDowComDeg+198)
#define ActRcvRchTxtTchGui	(ActDowComDeg+199)
#define ActRcvRchTxtSylLec	(ActDowComDeg+200)
#define ActRcvRchTxtSylPra	(ActDowComDeg+201)
#define ActRcvRchTxtBib		(ActDowComDeg+202)
#define ActRcvRchTxtFAQ		(ActDowComDeg+203)
#define ActRcvRchTxtCrsLnk	(ActDowComDeg+204)

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

#define ActEdiExaAnn		(ActRcvRchTxtCrsLnk+123)
#define ActRcvExaAnn		(ActRcvRchTxtCrsLnk+124)
#define ActPrnExaAnn		(ActRcvRchTxtCrsLnk+125)
#define ActRemExaAnn		(ActRcvRchTxtCrsLnk+126)

#define ActChgToSeeMrk		(ActRcvRchTxtCrsLnk+127)

#define ActSeeMrkCrs		(ActRcvRchTxtCrsLnk+128)
#define ActExpSeeMrkCrs		(ActRcvRchTxtCrsLnk+129)
#define ActConSeeMrkCrs		(ActRcvRchTxtCrsLnk+130)
#define ActReqDatSeeMrkCrs	(ActRcvRchTxtCrsLnk+131)
#define ActSeeMyMrkCrs		(ActRcvRchTxtCrsLnk+132)

#define ActSeeMrkGrp		(ActRcvRchTxtCrsLnk+133)
#define ActExpSeeMrkGrp		(ActRcvRchTxtCrsLnk+134)
#define ActConSeeMrkGrp		(ActRcvRchTxtCrsLnk+135)
#define ActReqDatSeeMrkGrp	(ActRcvRchTxtCrsLnk+136)
#define ActSeeMyMrkGrp		(ActRcvRchTxtCrsLnk+137)

#define ActChgToAdmMrk		(ActRcvRchTxtCrsLnk+138)

#define ActAdmMrkCrs		(ActRcvRchTxtCrsLnk+139)
#define ActReqRemFilMrkCrs	(ActRcvRchTxtCrsLnk+140)
#define ActRemFilMrkCrs		(ActRcvRchTxtCrsLnk+141)
#define ActRemFolMrkCrs		(ActRcvRchTxtCrsLnk+142)
#define ActCopMrkCrs		(ActRcvRchTxtCrsLnk+143)
#define ActPasMrkCrs		(ActRcvRchTxtCrsLnk+144)
#define ActRemTreMrkCrs		(ActRcvRchTxtCrsLnk+145)
#define ActFrmCreMrkCrs		(ActRcvRchTxtCrsLnk+146)
#define ActCreFolMrkCrs		(ActRcvRchTxtCrsLnk+147)
#define ActRenFolMrkCrs		(ActRcvRchTxtCrsLnk+148)
#define ActRcvFilMrkCrsDZ	(ActRcvRchTxtCrsLnk+149)
#define ActRcvFilMrkCrsCla	(ActRcvRchTxtCrsLnk+150)
#define ActExpAdmMrkCrs		(ActRcvRchTxtCrsLnk+151)
#define ActConAdmMrkCrs		(ActRcvRchTxtCrsLnk+152)
#define ActZIPAdmMrkCrs		(ActRcvRchTxtCrsLnk+153)
#define ActShoMrkCrs		(ActRcvRchTxtCrsLnk+154)
#define ActHidMrkCrs		(ActRcvRchTxtCrsLnk+155)
#define ActReqDatAdmMrkCrs	(ActRcvRchTxtCrsLnk+156)
#define ActChgDatAdmMrkCrs	(ActRcvRchTxtCrsLnk+157)
#define ActDowAdmMrkCrs		(ActRcvRchTxtCrsLnk+158)
#define ActChgNumRowHeaCrs	(ActRcvRchTxtCrsLnk+159)
#define ActChgNumRowFooCrs	(ActRcvRchTxtCrsLnk+160)

#define ActAdmMrkGrp		(ActRcvRchTxtCrsLnk+161)
#define ActReqRemFilMrkGrp	(ActRcvRchTxtCrsLnk+162)
#define ActRemFilMrkGrp		(ActRcvRchTxtCrsLnk+163)
#define ActRemFolMrkGrp		(ActRcvRchTxtCrsLnk+164)
#define ActCopMrkGrp		(ActRcvRchTxtCrsLnk+165)
#define ActPasMrkGrp		(ActRcvRchTxtCrsLnk+166)
#define ActRemTreMrkGrp		(ActRcvRchTxtCrsLnk+167)
#define ActFrmCreMrkGrp		(ActRcvRchTxtCrsLnk+168)
#define ActCreFolMrkGrp		(ActRcvRchTxtCrsLnk+169)
#define ActRenFolMrkGrp		(ActRcvRchTxtCrsLnk+170)
#define ActRcvFilMrkGrpDZ	(ActRcvRchTxtCrsLnk+171)
#define ActRcvFilMrkGrpCla	(ActRcvRchTxtCrsLnk+172)
#define ActExpAdmMrkGrp		(ActRcvRchTxtCrsLnk+173)
#define ActConAdmMrkGrp		(ActRcvRchTxtCrsLnk+174)
#define ActZIPAdmMrkGrp		(ActRcvRchTxtCrsLnk+175)
#define ActShoMrkGrp		(ActRcvRchTxtCrsLnk+176)
#define ActHidMrkGrp		(ActRcvRchTxtCrsLnk+177)
#define ActReqDatAdmMrkGrp	(ActRcvRchTxtCrsLnk+178)
#define ActChgDatAdmMrkGrp	(ActRcvRchTxtCrsLnk+179)
#define ActDowAdmMrkGrp		(ActRcvRchTxtCrsLnk+180)
#define ActChgNumRowHeaGrp	(ActRcvRchTxtCrsLnk+181)
#define ActChgNumRowFooGrp	(ActRcvRchTxtCrsLnk+182)

/*****************************************************************************/
/******************************* Users tab ***********************************/
/*****************************************************************************/
// Actions in menu
#define ActReqSelGrp		(ActChgNumRowFooGrp+  1)
#define ActLstStd		(ActChgNumRowFooGrp+  2)
#define ActSeeAtt		(ActChgNumRowFooGrp+  3)
#define ActLstTch		(ActChgNumRowFooGrp+  4)
#define ActLstOth		(ActChgNumRowFooGrp+  5)
#define ActReqSignUp		(ActChgNumRowFooGrp+  6)
#define ActSeeSignUpReq		(ActChgNumRowFooGrp+  7)
#define ActLstCon		(ActChgNumRowFooGrp+  8)
#define ActReqPubPrf		(ActChgNumRowFooGrp+  9)

// Secondary actions
#define ActChgGrp		(ActChgNumRowFooGrp+ 10)
#define ActReqEdiGrp		(ActChgNumRowFooGrp+ 11)
#define ActNewGrpTyp		(ActChgNumRowFooGrp+ 12)
#define ActReqRemGrpTyp		(ActChgNumRowFooGrp+ 13)
#define ActRemGrpTyp		(ActChgNumRowFooGrp+ 14)
#define ActRenGrpTyp		(ActChgNumRowFooGrp+ 15)
#define ActChgMdtGrpTyp		(ActChgNumRowFooGrp+ 16)
#define ActChgMulGrpTyp		(ActChgNumRowFooGrp+ 17)
#define ActChgTimGrpTyp		(ActChgNumRowFooGrp+ 18)
#define ActNewGrp		(ActChgNumRowFooGrp+ 19)
#define ActReqRemGrp		(ActChgNumRowFooGrp+ 20)
#define ActRemGrp		(ActChgNumRowFooGrp+ 21)
#define ActOpeGrp		(ActChgNumRowFooGrp+ 22)
#define ActCloGrp		(ActChgNumRowFooGrp+ 23)
#define ActEnaFilZonGrp		(ActChgNumRowFooGrp+ 24)
#define ActDisFilZonGrp		(ActChgNumRowFooGrp+ 25)
#define ActChgGrpTyp		(ActChgNumRowFooGrp+ 26)
#define ActRenGrp		(ActChgNumRowFooGrp+ 27)
#define ActChgMaxStdGrp		(ActChgNumRowFooGrp+ 28)

#define ActGetExtLstStd		(ActChgNumRowFooGrp+ 29)

#define ActLstGst		(ActChgNumRowFooGrp+ 30)

#define ActPrnGstPho		(ActChgNumRowFooGrp+ 31)
#define ActPrnStdPho		(ActChgNumRowFooGrp+ 32)
#define ActPrnTchPho		(ActChgNumRowFooGrp+ 33)
#define ActLstGstAll		(ActChgNumRowFooGrp+ 34)
#define ActLstStdAll		(ActChgNumRowFooGrp+ 35)
#define ActLstTchAll		(ActChgNumRowFooGrp+ 36)

#define ActSeeRecOneStd		(ActChgNumRowFooGrp+ 37)
#define ActSeeRecOneTch		(ActChgNumRowFooGrp+ 38)
#define ActSeeRecSevGst		(ActChgNumRowFooGrp+ 39)
#define ActSeeRecSevStd		(ActChgNumRowFooGrp+ 40)
#define ActSeeRecSevTch		(ActChgNumRowFooGrp+ 41)
#define ActPrnRecSevGst		(ActChgNumRowFooGrp+ 42)
#define ActPrnRecSevStd		(ActChgNumRowFooGrp+ 43)
#define ActPrnRecSevTch		(ActChgNumRowFooGrp+ 44)
#define ActRcvRecOthUsr		(ActChgNumRowFooGrp+ 45)
#define ActEdiRecFie		(ActChgNumRowFooGrp+ 46)
#define ActNewFie		(ActChgNumRowFooGrp+ 47)
#define ActReqRemFie		(ActChgNumRowFooGrp+ 48)
#define ActRemFie		(ActChgNumRowFooGrp+ 49)
#define ActRenFie		(ActChgNumRowFooGrp+ 50)
#define ActChgRowFie		(ActChgNumRowFooGrp+ 51)
#define ActChgVisFie		(ActChgNumRowFooGrp+ 52)
#define ActRcvRecCrs		(ActChgNumRowFooGrp+ 53)

#define ActReqEnrSevStd		(ActChgNumRowFooGrp+ 54)
#define ActReqEnrSevTch		(ActChgNumRowFooGrp+ 55)

#define ActReqLstAttStd		(ActChgNumRowFooGrp+ 56)
#define ActSeeLstAttStd		(ActChgNumRowFooGrp+ 57)
#define ActPrnLstAttStd		(ActChgNumRowFooGrp+ 58)
#define ActFrmNewAtt		(ActChgNumRowFooGrp+ 59)
#define ActEdiOneAtt		(ActChgNumRowFooGrp+ 60)
#define ActNewAtt		(ActChgNumRowFooGrp+ 61)
#define ActChgAtt		(ActChgNumRowFooGrp+ 62)
#define ActReqRemAtt		(ActChgNumRowFooGrp+ 63)
#define ActRemAtt		(ActChgNumRowFooGrp+ 64)
#define ActHidAtt		(ActChgNumRowFooGrp+ 65)
#define ActShoAtt		(ActChgNumRowFooGrp+ 66)
#define ActSeeOneAtt		(ActChgNumRowFooGrp+ 67)
#define ActRecAttStd		(ActChgNumRowFooGrp+ 68)
#define ActRecAttMe		(ActChgNumRowFooGrp+ 69)

#define ActSignUp		(ActChgNumRowFooGrp+ 70)
#define ActReqRejSignUp		(ActChgNumRowFooGrp+ 71)
#define ActRejSignUp		(ActChgNumRowFooGrp+ 72)

#define ActReqMdfOneOth		(ActChgNumRowFooGrp+ 73)
#define ActReqMdfOneStd		(ActChgNumRowFooGrp+ 74)
#define ActReqMdfOneTch		(ActChgNumRowFooGrp+ 75)

#define ActReqMdfOth		(ActChgNumRowFooGrp+ 76)
#define ActReqMdfStd		(ActChgNumRowFooGrp+ 77)
#define ActReqMdfTch		(ActChgNumRowFooGrp+ 78)

#define ActReqOthPho		(ActChgNumRowFooGrp+ 79)
#define ActReqStdPho		(ActChgNumRowFooGrp+ 80)
#define ActReqTchPho		(ActChgNumRowFooGrp+ 81)
#define ActDetOthPho		(ActChgNumRowFooGrp+ 82)
#define ActDetStdPho		(ActChgNumRowFooGrp+ 83)
#define ActDetTchPho		(ActChgNumRowFooGrp+ 84)
#define ActUpdOthPho		(ActChgNumRowFooGrp+ 85)
#define ActUpdStdPho		(ActChgNumRowFooGrp+ 86)
#define ActUpdTchPho		(ActChgNumRowFooGrp+ 87)
#define ActRemOthPho		(ActChgNumRowFooGrp+ 88)
#define ActRemStdPho		(ActChgNumRowFooGrp+ 89)
#define ActRemTchPho		(ActChgNumRowFooGrp+ 90)
#define ActCreOth		(ActChgNumRowFooGrp+ 91)
#define ActCreStd		(ActChgNumRowFooGrp+ 92)
#define ActCreTch		(ActChgNumRowFooGrp+ 93)
#define ActUpdOth		(ActChgNumRowFooGrp+ 94)
#define ActUpdStd		(ActChgNumRowFooGrp+ 95)
#define ActUpdTch		(ActChgNumRowFooGrp+ 96)

#define ActReqAccEnrStd		(ActChgNumRowFooGrp+ 97)
#define ActReqAccEnrTch		(ActChgNumRowFooGrp+ 98)
#define ActAccEnrStd		(ActChgNumRowFooGrp+ 99)
#define ActAccEnrTch		(ActChgNumRowFooGrp+100)
#define ActRemMe_Std		(ActChgNumRowFooGrp+101)
#define ActRemMe_Tch		(ActChgNumRowFooGrp+102)

#define ActNewAdmIns		(ActChgNumRowFooGrp+103)
#define ActRemAdmIns		(ActChgNumRowFooGrp+104)
#define ActNewAdmCtr		(ActChgNumRowFooGrp+105)
#define ActRemAdmCtr		(ActChgNumRowFooGrp+106)
#define ActNewAdmDeg		(ActChgNumRowFooGrp+107)
#define ActRemAdmDeg		(ActChgNumRowFooGrp+108)

#define ActRcvFrmEnrSevStd	(ActChgNumRowFooGrp+109)
#define ActRcvFrmEnrSevTch	(ActChgNumRowFooGrp+110)

#define ActFrmIDsOth		(ActChgNumRowFooGrp+111)
#define ActFrmIDsStd		(ActChgNumRowFooGrp+112)
#define ActFrmIDsTch		(ActChgNumRowFooGrp+113)
#define ActRemID_Oth		(ActChgNumRowFooGrp+114)
#define ActRemID_Std		(ActChgNumRowFooGrp+115)
#define ActRemID_Tch		(ActChgNumRowFooGrp+116)
#define ActNewID_Oth		(ActChgNumRowFooGrp+117)
#define ActNewID_Std		(ActChgNumRowFooGrp+118)
#define ActNewID_Tch		(ActChgNumRowFooGrp+119)
#define ActFrmPwdOth		(ActChgNumRowFooGrp+120)
#define ActFrmPwdStd		(ActChgNumRowFooGrp+121)
#define ActFrmPwdTch		(ActChgNumRowFooGrp+122)
#define ActChgPwdOth		(ActChgNumRowFooGrp+123)
#define ActChgPwdStd		(ActChgNumRowFooGrp+124)
#define ActChgPwdTch		(ActChgNumRowFooGrp+125)
#define ActRemStdCrs		(ActChgNumRowFooGrp+126)
#define ActRemTchCrs		(ActChgNumRowFooGrp+127)
#define ActRemUsrGbl		(ActChgNumRowFooGrp+128)

#define ActReqRemAllStdCrs	(ActChgNumRowFooGrp+129)
#define ActRemAllStdCrs		(ActChgNumRowFooGrp+130)

#define ActReqRemOldUsr		(ActChgNumRowFooGrp+131)
#define ActRemOldUsr		(ActChgNumRowFooGrp+132)

#define ActSeePubPrf		(ActChgNumRowFooGrp+133)
#define ActCal1stClkTim		(ActChgNumRowFooGrp+134)
#define ActCalNumClk		(ActChgNumRowFooGrp+135)
#define ActCalNumFilVie		(ActChgNumRowFooGrp+136)
#define ActCalNumForPst		(ActChgNumRowFooGrp+137)
#define ActCalNumMsgSnt		(ActChgNumRowFooGrp+138)

#define ActFolUsr		(ActChgNumRowFooGrp+139)
#define ActUnfUsr		(ActChgNumRowFooGrp+140)
#define ActSeeFlg		(ActChgNumRowFooGrp+141)
#define ActSeeFlr		(ActChgNumRowFooGrp+142)

/*****************************************************************************/
/******************************* Messages tab ********************************/
/*****************************************************************************/
// Actions in menu
#define ActSeeNtf		(ActSeeFlr+  1)
#define ActSeeAnn		(ActSeeFlr+  2)
#define ActSeeNot		(ActSeeFlr+  3)
#define ActSeeFor		(ActSeeFlr+  4)
#define ActSeeChtRms		(ActSeeFlr+  5)
#define ActReqMsgUsr		(ActSeeFlr+  6)
#define ActSeeRcvMsg		(ActSeeFlr+  7)
#define ActSeeSntMsg		(ActSeeFlr+  8)
#define ActMaiStd		(ActSeeFlr+  9)
// Secondary actions
#define ActWriAnn		(ActSeeFlr+ 10)
#define ActRcvAnn		(ActSeeFlr+ 11)
#define ActRemAnn		(ActSeeFlr+ 12)
#define ActShoNot		(ActSeeFlr+ 13)
#define ActWriNot		(ActSeeFlr+ 14)
#define ActRcvNot		(ActSeeFlr+ 15)
#define ActHidNot		(ActSeeFlr+ 16)
#define ActRevNot		(ActSeeFlr+ 17)
#define ActRemNot		(ActSeeFlr+ 18)
#define ActSeeNewNtf		(ActSeeFlr+ 19)
#define ActMrkNtfSee		(ActSeeFlr+ 20)
#define ActSeeForCrsUsr		(ActSeeFlr+ 21)
#define ActSeeForCrsTch		(ActSeeFlr+ 22)
#define ActSeeForDegUsr		(ActSeeFlr+ 23)
#define ActSeeForDegTch		(ActSeeFlr+ 24)
#define ActSeeForCtrUsr		(ActSeeFlr+ 25)
#define ActSeeForCtrTch		(ActSeeFlr+ 26)
#define ActSeeForInsUsr		(ActSeeFlr+ 27)
#define ActSeeForInsTch		(ActSeeFlr+ 28)
#define ActSeeForGenUsr		(ActSeeFlr+ 29)
#define ActSeeForGenTch		(ActSeeFlr+ 30)
#define ActSeeForSWAUsr		(ActSeeFlr+ 31)
#define ActSeeForSWATch		(ActSeeFlr+ 32)
#define ActSeePstForCrsUsr	(ActSeeFlr+ 33)
#define ActSeePstForCrsTch	(ActSeeFlr+ 34)
#define ActSeePstForDegUsr	(ActSeeFlr+ 35)
#define ActSeePstForDegTch	(ActSeeFlr+ 36)
#define ActSeePstForCtrUsr	(ActSeeFlr+ 37)
#define ActSeePstForCtrTch	(ActSeeFlr+ 38)
#define ActSeePstForInsUsr	(ActSeeFlr+ 39)
#define ActSeePstForInsTch	(ActSeeFlr+ 40)
#define ActSeePstForGenUsr	(ActSeeFlr+ 41)
#define ActSeePstForGenTch	(ActSeeFlr+ 42)
#define ActSeePstForSWAUsr	(ActSeeFlr+ 43)
#define ActSeePstForSWATch	(ActSeeFlr+ 44)
#define ActRcvThrForCrsUsr	(ActSeeFlr+ 45)
#define ActRcvThrForCrsTch	(ActSeeFlr+ 46)
#define ActRcvThrForDegUsr	(ActSeeFlr+ 47)
#define ActRcvThrForDegTch	(ActSeeFlr+ 48)
#define ActRcvThrForCtrUsr	(ActSeeFlr+ 49)
#define ActRcvThrForCtrTch	(ActSeeFlr+ 50)
#define ActRcvThrForInsUsr	(ActSeeFlr+ 51)
#define ActRcvThrForInsTch	(ActSeeFlr+ 52)
#define ActRcvThrForGenUsr	(ActSeeFlr+ 53)
#define ActRcvThrForGenTch	(ActSeeFlr+ 54)
#define ActRcvThrForSWAUsr	(ActSeeFlr+ 55)
#define ActRcvThrForSWATch	(ActSeeFlr+ 56)
#define ActRcvRepForCrsUsr	(ActSeeFlr+ 57)
#define ActRcvRepForCrsTch	(ActSeeFlr+ 58)
#define ActRcvRepForDegUsr	(ActSeeFlr+ 59)
#define ActRcvRepForDegTch	(ActSeeFlr+ 60)
#define ActRcvRepForCtrUsr	(ActSeeFlr+ 61)
#define ActRcvRepForCtrTch	(ActSeeFlr+ 62)
#define ActRcvRepForInsUsr	(ActSeeFlr+ 63)
#define ActRcvRepForInsTch	(ActSeeFlr+ 64)
#define ActRcvRepForGenUsr	(ActSeeFlr+ 65)
#define ActRcvRepForGenTch	(ActSeeFlr+ 66)
#define ActRcvRepForSWAUsr	(ActSeeFlr+ 67)
#define ActRcvRepForSWATch	(ActSeeFlr+ 68)
#define ActReqDelThrCrsUsr	(ActSeeFlr+ 69)
#define ActReqDelThrCrsTch	(ActSeeFlr+ 70)
#define ActReqDelThrDegUsr	(ActSeeFlr+ 71)
#define ActReqDelThrDegTch	(ActSeeFlr+ 72)
#define ActReqDelThrCtrUsr	(ActSeeFlr+ 73)
#define ActReqDelThrCtrTch	(ActSeeFlr+ 74)
#define ActReqDelThrInsUsr	(ActSeeFlr+ 75)
#define ActReqDelThrInsTch	(ActSeeFlr+ 76)
#define ActReqDelThrGenUsr	(ActSeeFlr+ 77)
#define ActReqDelThrGenTch	(ActSeeFlr+ 78)
#define ActReqDelThrSWAUsr	(ActSeeFlr+ 79)
#define ActReqDelThrSWATch	(ActSeeFlr+ 80)
#define ActDelThrForCrsUsr	(ActSeeFlr+ 81)
#define ActDelThrForCrsTch	(ActSeeFlr+ 82)
#define ActDelThrForDegUsr	(ActSeeFlr+ 83)
#define ActDelThrForDegTch	(ActSeeFlr+ 84)
#define ActDelThrForCtrUsr	(ActSeeFlr+ 85)
#define ActDelThrForCtrTch	(ActSeeFlr+ 86)
#define ActDelThrForInsUsr	(ActSeeFlr+ 87)
#define ActDelThrForInsTch	(ActSeeFlr+ 88)
#define ActDelThrForGenUsr	(ActSeeFlr+ 89)
#define ActDelThrForGenTch	(ActSeeFlr+ 90)
#define ActDelThrForSWAUsr	(ActSeeFlr+ 91)
#define ActDelThrForSWATch	(ActSeeFlr+ 92)
#define ActCutThrForCrsUsr	(ActSeeFlr+ 93)
#define ActCutThrForCrsTch	(ActSeeFlr+ 94)
#define ActCutThrForDegUsr	(ActSeeFlr+ 95)
#define ActCutThrForDegTch	(ActSeeFlr+ 96)
#define ActCutThrForCtrUsr	(ActSeeFlr+ 97)
#define ActCutThrForCtrTch	(ActSeeFlr+ 98)
#define ActCutThrForInsUsr	(ActSeeFlr+ 99)
#define ActCutThrForInsTch	(ActSeeFlr+100)
#define ActCutThrForGenUsr	(ActSeeFlr+101)
#define ActCutThrForGenTch	(ActSeeFlr+102)
#define ActCutThrForSWAUsr	(ActSeeFlr+103)
#define ActCutThrForSWATch	(ActSeeFlr+104)
#define ActPasThrForCrsUsr	(ActSeeFlr+105)
#define ActPasThrForCrsTch	(ActSeeFlr+106)
#define ActPasThrForDegUsr	(ActSeeFlr+107)
#define ActPasThrForDegTch	(ActSeeFlr+108)
#define ActPasThrForCtrUsr	(ActSeeFlr+109)
#define ActPasThrForCtrTch	(ActSeeFlr+110)
#define ActPasThrForInsUsr	(ActSeeFlr+111)
#define ActPasThrForInsTch	(ActSeeFlr+112)
#define ActPasThrForGenUsr	(ActSeeFlr+113)
#define ActPasThrForGenTch	(ActSeeFlr+114)
#define ActPasThrForSWAUsr	(ActSeeFlr+115)
#define ActPasThrForSWATch	(ActSeeFlr+116)
#define ActDelPstForCrsUsr	(ActSeeFlr+117)
#define ActDelPstForCrsTch	(ActSeeFlr+118)
#define ActDelPstForDegUsr	(ActSeeFlr+119)
#define ActDelPstForDegTch	(ActSeeFlr+120)
#define ActDelPstForCtrUsr	(ActSeeFlr+121)
#define ActDelPstForCtrTch	(ActSeeFlr+122)
#define ActDelPstForInsUsr	(ActSeeFlr+123)
#define ActDelPstForInsTch	(ActSeeFlr+124)
#define ActDelPstForGenUsr	(ActSeeFlr+125)
#define ActDelPstForGenTch	(ActSeeFlr+126)
#define ActDelPstForSWAUsr	(ActSeeFlr+127)
#define ActDelPstForSWATch	(ActSeeFlr+128)
#define ActEnbPstForCrsUsr	(ActSeeFlr+129)
#define ActEnbPstForCrsTch	(ActSeeFlr+130)
#define ActEnbPstForDegUsr	(ActSeeFlr+131)
#define ActEnbPstForDegTch	(ActSeeFlr+132)
#define ActEnbPstForCtrUsr	(ActSeeFlr+133)
#define ActEnbPstForCtrTch	(ActSeeFlr+134)
#define ActEnbPstForInsUsr	(ActSeeFlr+135)
#define ActEnbPstForInsTch	(ActSeeFlr+136)
#define ActEnbPstForGenUsr	(ActSeeFlr+137)
#define ActEnbPstForGenTch	(ActSeeFlr+138)
#define ActEnbPstForSWAUsr	(ActSeeFlr+139)
#define ActEnbPstForSWATch	(ActSeeFlr+140)
#define ActDisPstForCrsUsr	(ActSeeFlr+141)
#define ActDisPstForCrsTch	(ActSeeFlr+142)
#define ActDisPstForDegUsr	(ActSeeFlr+143)
#define ActDisPstForDegTch	(ActSeeFlr+144)
#define ActDisPstForCtrUsr	(ActSeeFlr+145)
#define ActDisPstForCtrTch	(ActSeeFlr+146)
#define ActDisPstForInsUsr	(ActSeeFlr+147)
#define ActDisPstForInsTch	(ActSeeFlr+148)
#define ActDisPstForGenUsr	(ActSeeFlr+149)
#define ActDisPstForGenTch	(ActSeeFlr+150)
#define ActDisPstForSWAUsr	(ActSeeFlr+151)
#define ActDisPstForSWATch	(ActSeeFlr+152)
#define ActRcvMsgUsr		(ActSeeFlr+153)
#define ActReqDelAllSntMsg	(ActSeeFlr+154)
#define ActReqDelAllRcvMsg	(ActSeeFlr+155)
#define ActDelAllSntMsg		(ActSeeFlr+156)
#define ActDelAllRcvMsg		(ActSeeFlr+157)
#define ActDelSntMsg		(ActSeeFlr+158)
#define ActDelRcvMsg		(ActSeeFlr+159)
#define ActExpSntMsg		(ActSeeFlr+160)
#define ActExpRcvMsg		(ActSeeFlr+161)
#define ActConSntMsg		(ActSeeFlr+162)
#define ActConRcvMsg		(ActSeeFlr+163)
#define ActLstBanUsr		(ActSeeFlr+164)
#define ActBanUsrMsg		(ActSeeFlr+165)
#define ActUnbUsrMsg		(ActSeeFlr+166)
#define ActUnbUsrLst		(ActSeeFlr+167)
#define ActCht			(ActSeeFlr+168)

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

#define ActChgLay		(ActSeeAllStaCrs+46)
#define ActChgThe		(ActSeeAllStaCrs+47)
#define ActReqChgLan		(ActSeeAllStaCrs+48)
#define ActChgLan		(ActSeeAllStaCrs+49)
#define ActChgCol		(ActSeeAllStaCrs+50)
#define ActHidLftCol		(ActSeeAllStaCrs+51)
#define ActHidRgtCol		(ActSeeAllStaCrs+52)
#define ActShoLftCol		(ActSeeAllStaCrs+53)
#define ActShoRgtCol		(ActSeeAllStaCrs+54)
#define ActChgIco		(ActSeeAllStaCrs+55)
#define ActChgMnu		(ActSeeAllStaCrs+56)
#define ActChgNtfPrf		(ActSeeAllStaCrs+57)

#define ActPrnUsrQR		(ActSeeAllStaCrs+58)

#define ActPrnMyTimTbl		(ActSeeAllStaCrs+59)
#define ActEdiTut		(ActSeeAllStaCrs+60)
#define ActChgTut		(ActSeeAllStaCrs+61)
#define ActReqRemFilBrf		(ActSeeAllStaCrs+62)
#define ActRemFilBrf		(ActSeeAllStaCrs+63)
#define ActRemFolBrf		(ActSeeAllStaCrs+64)
#define ActCopBrf		(ActSeeAllStaCrs+65)
#define ActPasBrf		(ActSeeAllStaCrs+66)
#define ActRemTreBrf		(ActSeeAllStaCrs+67)
#define ActFrmCreBrf		(ActSeeAllStaCrs+68)
#define ActCreFolBrf		(ActSeeAllStaCrs+69)
#define ActCreLnkBrf		(ActSeeAllStaCrs+70)
#define ActRenFolBrf		(ActSeeAllStaCrs+71)
#define ActRcvFilBrfDZ		(ActSeeAllStaCrs+72)
#define ActRcvFilBrfCla		(ActSeeAllStaCrs+73)
#define ActExpBrf		(ActSeeAllStaCrs+74)
#define ActConBrf		(ActSeeAllStaCrs+75)
#define ActZIPBrf		(ActSeeAllStaCrs+76)
#define ActReqDatBrf		(ActSeeAllStaCrs+77)
#define ActChgDatBrf		(ActSeeAllStaCrs+78)
#define ActDowBrf		(ActSeeAllStaCrs+79)

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
void Act_FormEnd (void);
void Act_LinkFormSubmit (const char *Title,const char *LinkStyle);
void Act_LinkFormSubmitId (const char *Title,const char *LinkStyle,const char *Id);
void Act_LinkFormSubmitAnimated (const char *Title,const char *LinkStyle);

void Act_PutContextualLink (Act_Action_t NextAction,void (*FuncParams) (),
                            const char *Icon,const char *Title);
void Act_PutIconLink (const char *Icon,const char *Title);

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
