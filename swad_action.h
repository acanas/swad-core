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

#define Act_NUM_ACTIONS	(7+52+15+90+73+68+204+184+100+168+28+75)

#define Act_MAX_ACTION_COD 1402

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
#define ActReqSelGrp		(ActChgNumRowFooGrp+  1)
#define ActLstInv		(ActChgNumRowFooGrp+  2)
#define ActLstStd		(ActChgNumRowFooGrp+  3)
#define ActSeeAtt		(ActChgNumRowFooGrp+  4)
#define ActLstTch		(ActChgNumRowFooGrp+  5)
#define ActLstAdm		(ActChgNumRowFooGrp+  6)
#define ActReqSignUp		(ActChgNumRowFooGrp+  7)
#define ActSeeSignUpReq		(ActChgNumRowFooGrp+  8)
#define ActReqMdfSevUsr		(ActChgNumRowFooGrp+  9)
#define ActLstCon		(ActChgNumRowFooGrp+ 10)
#define ActReqPubPrf		(ActChgNumRowFooGrp+ 11)

// Secondary actions
#define ActChgGrp		(ActChgNumRowFooGrp+ 12)
#define ActReqEdiGrp		(ActChgNumRowFooGrp+ 13)
#define ActNewGrpTyp		(ActChgNumRowFooGrp+ 14)
#define ActReqRemGrpTyp		(ActChgNumRowFooGrp+ 15)
#define ActRemGrpTyp		(ActChgNumRowFooGrp+ 16)
#define ActRenGrpTyp		(ActChgNumRowFooGrp+ 17)
#define ActChgMdtGrpTyp		(ActChgNumRowFooGrp+ 18)
#define ActChgMulGrpTyp		(ActChgNumRowFooGrp+ 19)
#define ActChgTimGrpTyp		(ActChgNumRowFooGrp+ 20)
#define ActNewGrp		(ActChgNumRowFooGrp+ 21)
#define ActReqRemGrp		(ActChgNumRowFooGrp+ 22)
#define ActRemGrp		(ActChgNumRowFooGrp+ 23)
#define ActOpeGrp		(ActChgNumRowFooGrp+ 24)
#define ActCloGrp		(ActChgNumRowFooGrp+ 25)
#define ActEnaFilZonGrp		(ActChgNumRowFooGrp+ 26)
#define ActDisFilZonGrp		(ActChgNumRowFooGrp+ 27)
#define ActChgGrpTyp		(ActChgNumRowFooGrp+ 28)
#define ActRenGrp		(ActChgNumRowFooGrp+ 29)
#define ActChgMaxStdGrp		(ActChgNumRowFooGrp+ 30)

#define ActGetExtLstStd		(ActChgNumRowFooGrp+ 31)
#define ActPrnInvPho		(ActChgNumRowFooGrp+ 32)
#define ActPrnStdPho		(ActChgNumRowFooGrp+ 33)
#define ActPrnTchPho		(ActChgNumRowFooGrp+ 34)
#define ActLstInvAll		(ActChgNumRowFooGrp+ 35)
#define ActLstStdAll		(ActChgNumRowFooGrp+ 36)
#define ActLstTchAll		(ActChgNumRowFooGrp+ 37)
#define ActSeeRecOneStd		(ActChgNumRowFooGrp+ 38)
#define ActSeeRecOneTch		(ActChgNumRowFooGrp+ 39)
#define ActSeeRecSevInv		(ActChgNumRowFooGrp+ 40)
#define ActSeeRecSevStd		(ActChgNumRowFooGrp+ 41)
#define ActSeeRecSevTch		(ActChgNumRowFooGrp+ 42)
#define ActPrnRecSevInv		(ActChgNumRowFooGrp+ 43)
#define ActPrnRecSevStd		(ActChgNumRowFooGrp+ 44)
#define ActPrnRecSevTch		(ActChgNumRowFooGrp+ 45)
#define ActRcvRecOthUsr		(ActChgNumRowFooGrp+ 46)
#define ActEdiRecFie		(ActChgNumRowFooGrp+ 47)
#define ActNewFie		(ActChgNumRowFooGrp+ 48)
#define ActReqRemFie		(ActChgNumRowFooGrp+ 49)
#define ActRemFie		(ActChgNumRowFooGrp+ 50)
#define ActRenFie		(ActChgNumRowFooGrp+ 51)
#define ActChgRowFie		(ActChgNumRowFooGrp+ 52)
#define ActChgVisFie		(ActChgNumRowFooGrp+ 53)

#define ActReqLstAttStd		(ActChgNumRowFooGrp+ 54)
#define ActSeeLstAttStd		(ActChgNumRowFooGrp+ 55)
#define ActPrnLstAttStd		(ActChgNumRowFooGrp+ 56)
#define ActFrmNewAtt		(ActChgNumRowFooGrp+ 57)
#define ActEdiOneAtt		(ActChgNumRowFooGrp+ 58)
#define ActNewAtt		(ActChgNumRowFooGrp+ 59)
#define ActChgAtt		(ActChgNumRowFooGrp+ 60)
#define ActReqRemAtt		(ActChgNumRowFooGrp+ 61)
#define ActRemAtt		(ActChgNumRowFooGrp+ 62)
#define ActHidAtt		(ActChgNumRowFooGrp+ 63)
#define ActShoAtt		(ActChgNumRowFooGrp+ 64)
#define ActSeeOneAtt		(ActChgNumRowFooGrp+ 65)
#define ActRecAttStd		(ActChgNumRowFooGrp+ 66)
#define ActRecAttMe		(ActChgNumRowFooGrp+ 67)

#define ActSignUp		(ActChgNumRowFooGrp+ 68)
#define ActReqRejSignUp		(ActChgNumRowFooGrp+ 69)
#define ActRejSignUp		(ActChgNumRowFooGrp+ 70)

#define ActReqMdfOneUsr		(ActChgNumRowFooGrp+ 71)
#define ActReqMdfUsr		(ActChgNumRowFooGrp+ 72)

#define ActReqUsrPho		(ActChgNumRowFooGrp+ 73)
#define ActDetUsrPho		(ActChgNumRowFooGrp+ 74)
#define ActUpdUsrPho		(ActChgNumRowFooGrp+ 75)
#define ActRemUsrPho		(ActChgNumRowFooGrp+ 76)
#define ActCreOthUsrDat		(ActChgNumRowFooGrp+ 77)
#define ActUpdOthUsrDat		(ActChgNumRowFooGrp+ 78)

#define ActReqAccEnrCrs		(ActChgNumRowFooGrp+ 79)
#define ActAccEnrCrs		(ActChgNumRowFooGrp+ 80)
#define ActRemMeCrs		(ActChgNumRowFooGrp+ 81)

#define ActNewAdmIns		(ActChgNumRowFooGrp+ 82)
#define ActRemAdmIns		(ActChgNumRowFooGrp+ 83)
#define ActNewAdmCtr		(ActChgNumRowFooGrp+ 84)
#define ActRemAdmCtr		(ActChgNumRowFooGrp+ 85)
#define ActNewAdmDeg		(ActChgNumRowFooGrp+ 86)
#define ActRemAdmDeg		(ActChgNumRowFooGrp+ 87)

#define ActRcvFrmMdfUsrCrs	(ActChgNumRowFooGrp+ 88)
#define ActFrmIDsOthUsr		(ActChgNumRowFooGrp+ 89)
#define ActRemIDOth		(ActChgNumRowFooGrp+ 90)
#define ActNewIDOth		(ActChgNumRowFooGrp+ 91)
#define ActFrmPwdOthUsr		(ActChgNumRowFooGrp+ 92)
#define ActChgPwdOthUsr		(ActChgNumRowFooGrp+ 93)
#define ActRemUsrCrs		(ActChgNumRowFooGrp+ 94)
#define ActRemUsrGbl		(ActChgNumRowFooGrp+ 95)

#define ActReqRemAllStdCrs	(ActChgNumRowFooGrp+ 96)
#define ActRemAllStdCrs		(ActChgNumRowFooGrp+ 97)

#define ActReqRemOldUsr		(ActChgNumRowFooGrp+ 98)
#define ActRemOldUsr		(ActChgNumRowFooGrp+ 99)

#define ActSeePubPrf		(ActChgNumRowFooGrp+100)

/*****************************************************************************/
/******************************* Messages tab ********************************/
/*****************************************************************************/
// Actions in menu
#define ActSeeNtf		(ActSeePubPrf+  1)
#define ActSeeAnn		(ActSeePubPrf+  2)
#define ActSeeNot		(ActSeePubPrf+  3)
#define ActSeeFor		(ActSeePubPrf+  4)
#define ActSeeChtRms		(ActSeePubPrf+  5)
#define ActReqMsgUsr		(ActSeePubPrf+  6)
#define ActSeeRcvMsg		(ActSeePubPrf+  7)
#define ActSeeSntMsg		(ActSeePubPrf+  8)
#define ActMaiStd		(ActSeePubPrf+  9)
// Secondary actions
#define ActWriAnn		(ActSeePubPrf+ 10)
#define ActRcvAnn		(ActSeePubPrf+ 11)
#define ActRemAnn		(ActSeePubPrf+ 12)
#define ActShoNot		(ActSeePubPrf+ 13)
#define ActWriNot		(ActSeePubPrf+ 14)
#define ActRcvNot		(ActSeePubPrf+ 15)
#define ActHidNot		(ActSeePubPrf+ 16)
#define ActRevNot		(ActSeePubPrf+ 17)
#define ActRemNot		(ActSeePubPrf+ 18)
#define ActSeeNewNtf		(ActSeePubPrf+ 19)
#define ActMrkNtfSee		(ActSeePubPrf+ 20)
#define ActSeeForCrsUsr		(ActSeePubPrf+ 21)
#define ActSeeForCrsTch		(ActSeePubPrf+ 22)
#define ActSeeForDegUsr		(ActSeePubPrf+ 23)
#define ActSeeForDegTch		(ActSeePubPrf+ 24)
#define ActSeeForCtrUsr		(ActSeePubPrf+ 25)
#define ActSeeForCtrTch		(ActSeePubPrf+ 26)
#define ActSeeForInsUsr		(ActSeePubPrf+ 27)
#define ActSeeForInsTch		(ActSeePubPrf+ 28)
#define ActSeeForGenUsr		(ActSeePubPrf+ 29)
#define ActSeeForGenTch		(ActSeePubPrf+ 30)
#define ActSeeForSWAUsr		(ActSeePubPrf+ 31)
#define ActSeeForSWATch		(ActSeePubPrf+ 32)
#define ActSeePstForCrsUsr	(ActSeePubPrf+ 33)
#define ActSeePstForCrsTch	(ActSeePubPrf+ 34)
#define ActSeePstForDegUsr	(ActSeePubPrf+ 35)
#define ActSeePstForDegTch	(ActSeePubPrf+ 36)
#define ActSeePstForCtrUsr	(ActSeePubPrf+ 37)
#define ActSeePstForCtrTch	(ActSeePubPrf+ 38)
#define ActSeePstForInsUsr	(ActSeePubPrf+ 39)
#define ActSeePstForInsTch	(ActSeePubPrf+ 40)
#define ActSeePstForGenUsr	(ActSeePubPrf+ 41)
#define ActSeePstForGenTch	(ActSeePubPrf+ 42)
#define ActSeePstForSWAUsr	(ActSeePubPrf+ 43)
#define ActSeePstForSWATch	(ActSeePubPrf+ 44)
#define ActRcvThrForCrsUsr	(ActSeePubPrf+ 45)
#define ActRcvThrForCrsTch	(ActSeePubPrf+ 46)
#define ActRcvThrForDegUsr	(ActSeePubPrf+ 47)
#define ActRcvThrForDegTch	(ActSeePubPrf+ 48)
#define ActRcvThrForCtrUsr	(ActSeePubPrf+ 49)
#define ActRcvThrForCtrTch	(ActSeePubPrf+ 50)
#define ActRcvThrForInsUsr	(ActSeePubPrf+ 51)
#define ActRcvThrForInsTch	(ActSeePubPrf+ 52)
#define ActRcvThrForGenUsr	(ActSeePubPrf+ 53)
#define ActRcvThrForGenTch	(ActSeePubPrf+ 54)
#define ActRcvThrForSWAUsr	(ActSeePubPrf+ 55)
#define ActRcvThrForSWATch	(ActSeePubPrf+ 56)
#define ActRcvRepForCrsUsr	(ActSeePubPrf+ 57)
#define ActRcvRepForCrsTch	(ActSeePubPrf+ 58)
#define ActRcvRepForDegUsr	(ActSeePubPrf+ 59)
#define ActRcvRepForDegTch	(ActSeePubPrf+ 60)
#define ActRcvRepForCtrUsr	(ActSeePubPrf+ 61)
#define ActRcvRepForCtrTch	(ActSeePubPrf+ 62)
#define ActRcvRepForInsUsr	(ActSeePubPrf+ 63)
#define ActRcvRepForInsTch	(ActSeePubPrf+ 64)
#define ActRcvRepForGenUsr	(ActSeePubPrf+ 65)
#define ActRcvRepForGenTch	(ActSeePubPrf+ 66)
#define ActRcvRepForSWAUsr	(ActSeePubPrf+ 67)
#define ActRcvRepForSWATch	(ActSeePubPrf+ 68)
#define ActReqDelThrCrsUsr	(ActSeePubPrf+ 69)
#define ActReqDelThrCrsTch	(ActSeePubPrf+ 70)
#define ActReqDelThrDegUsr	(ActSeePubPrf+ 71)
#define ActReqDelThrDegTch	(ActSeePubPrf+ 72)
#define ActReqDelThrCtrUsr	(ActSeePubPrf+ 73)
#define ActReqDelThrCtrTch	(ActSeePubPrf+ 74)
#define ActReqDelThrInsUsr	(ActSeePubPrf+ 75)
#define ActReqDelThrInsTch	(ActSeePubPrf+ 76)
#define ActReqDelThrGenUsr	(ActSeePubPrf+ 77)
#define ActReqDelThrGenTch	(ActSeePubPrf+ 78)
#define ActReqDelThrSWAUsr	(ActSeePubPrf+ 79)
#define ActReqDelThrSWATch	(ActSeePubPrf+ 80)
#define ActDelThrForCrsUsr	(ActSeePubPrf+ 81)
#define ActDelThrForCrsTch	(ActSeePubPrf+ 82)
#define ActDelThrForDegUsr	(ActSeePubPrf+ 83)
#define ActDelThrForDegTch	(ActSeePubPrf+ 84)
#define ActDelThrForCtrUsr	(ActSeePubPrf+ 85)
#define ActDelThrForCtrTch	(ActSeePubPrf+ 86)
#define ActDelThrForInsUsr	(ActSeePubPrf+ 87)
#define ActDelThrForInsTch	(ActSeePubPrf+ 88)
#define ActDelThrForGenUsr	(ActSeePubPrf+ 89)
#define ActDelThrForGenTch	(ActSeePubPrf+ 90)
#define ActDelThrForSWAUsr	(ActSeePubPrf+ 91)
#define ActDelThrForSWATch	(ActSeePubPrf+ 92)
#define ActCutThrForCrsUsr	(ActSeePubPrf+ 93)
#define ActCutThrForCrsTch	(ActSeePubPrf+ 94)
#define ActCutThrForDegUsr	(ActSeePubPrf+ 95)
#define ActCutThrForDegTch	(ActSeePubPrf+ 96)
#define ActCutThrForCtrUsr	(ActSeePubPrf+ 97)
#define ActCutThrForCtrTch	(ActSeePubPrf+ 98)
#define ActCutThrForInsUsr	(ActSeePubPrf+ 99)
#define ActCutThrForInsTch	(ActSeePubPrf+100)
#define ActCutThrForGenUsr	(ActSeePubPrf+101)
#define ActCutThrForGenTch	(ActSeePubPrf+102)
#define ActCutThrForSWAUsr	(ActSeePubPrf+103)
#define ActCutThrForSWATch	(ActSeePubPrf+104)
#define ActPasThrForCrsUsr	(ActSeePubPrf+105)
#define ActPasThrForCrsTch	(ActSeePubPrf+106)
#define ActPasThrForDegUsr	(ActSeePubPrf+107)
#define ActPasThrForDegTch	(ActSeePubPrf+108)
#define ActPasThrForCtrUsr	(ActSeePubPrf+109)
#define ActPasThrForCtrTch	(ActSeePubPrf+110)
#define ActPasThrForInsUsr	(ActSeePubPrf+111)
#define ActPasThrForInsTch	(ActSeePubPrf+112)
#define ActPasThrForGenUsr	(ActSeePubPrf+113)
#define ActPasThrForGenTch	(ActSeePubPrf+114)
#define ActPasThrForSWAUsr	(ActSeePubPrf+115)
#define ActPasThrForSWATch	(ActSeePubPrf+116)
#define ActDelPstForCrsUsr	(ActSeePubPrf+117)
#define ActDelPstForCrsTch	(ActSeePubPrf+118)
#define ActDelPstForDegUsr	(ActSeePubPrf+119)
#define ActDelPstForDegTch	(ActSeePubPrf+120)
#define ActDelPstForCtrUsr	(ActSeePubPrf+121)
#define ActDelPstForCtrTch	(ActSeePubPrf+122)
#define ActDelPstForInsUsr	(ActSeePubPrf+123)
#define ActDelPstForInsTch	(ActSeePubPrf+124)
#define ActDelPstForGenUsr	(ActSeePubPrf+125)
#define ActDelPstForGenTch	(ActSeePubPrf+126)
#define ActDelPstForSWAUsr	(ActSeePubPrf+127)
#define ActDelPstForSWATch	(ActSeePubPrf+128)
#define ActEnbPstForCrsUsr	(ActSeePubPrf+129)
#define ActEnbPstForCrsTch	(ActSeePubPrf+130)
#define ActEnbPstForDegUsr	(ActSeePubPrf+131)
#define ActEnbPstForDegTch	(ActSeePubPrf+132)
#define ActEnbPstForCtrUsr	(ActSeePubPrf+133)
#define ActEnbPstForCtrTch	(ActSeePubPrf+134)
#define ActEnbPstForInsUsr	(ActSeePubPrf+135)
#define ActEnbPstForInsTch	(ActSeePubPrf+136)
#define ActEnbPstForGenUsr	(ActSeePubPrf+137)
#define ActEnbPstForGenTch	(ActSeePubPrf+138)
#define ActEnbPstForSWAUsr	(ActSeePubPrf+139)
#define ActEnbPstForSWATch	(ActSeePubPrf+140)
#define ActDisPstForCrsUsr	(ActSeePubPrf+141)
#define ActDisPstForCrsTch	(ActSeePubPrf+142)
#define ActDisPstForDegUsr	(ActSeePubPrf+143)
#define ActDisPstForDegTch	(ActSeePubPrf+144)
#define ActDisPstForCtrUsr	(ActSeePubPrf+145)
#define ActDisPstForCtrTch	(ActSeePubPrf+146)
#define ActDisPstForInsUsr	(ActSeePubPrf+147)
#define ActDisPstForInsTch	(ActSeePubPrf+148)
#define ActDisPstForGenUsr	(ActSeePubPrf+149)
#define ActDisPstForGenTch	(ActSeePubPrf+150)
#define ActDisPstForSWAUsr	(ActSeePubPrf+151)
#define ActDisPstForSWATch	(ActSeePubPrf+152)
#define ActRcvMsgUsr		(ActSeePubPrf+153)
#define ActReqDelAllSntMsg	(ActSeePubPrf+154)
#define ActReqDelAllRcvMsg	(ActSeePubPrf+155)
#define ActDelAllSntMsg		(ActSeePubPrf+156)
#define ActDelAllRcvMsg		(ActSeePubPrf+157)
#define ActDelSntMsg		(ActSeePubPrf+158)
#define ActDelRcvMsg		(ActSeePubPrf+159)
#define ActExpSntMsg		(ActSeePubPrf+160)
#define ActExpRcvMsg		(ActSeePubPrf+161)
#define ActConSntMsg		(ActSeePubPrf+162)
#define ActConRcvMsg		(ActSeePubPrf+163)
#define ActLstBanUsr		(ActSeePubPrf+164)
#define ActBanUsrMsg		(ActSeePubPrf+165)
#define ActUnbUsrMsg		(ActSeePubPrf+166)
#define ActUnbUsrLst		(ActSeePubPrf+167)
#define ActCht			(ActSeePubPrf+168)

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
