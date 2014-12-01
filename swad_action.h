// swad_action.h: actions

#ifndef _SWAD_ACT
#define _SWAD_ACT
/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2014 Antonio Cañas Vargas

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

/* Related with tabs, menus, etc. */
#define Act_NUM_TABS	(1+11)
#define Act_NUM_ACTIONS	(7+52+15+39+22+17+205+182+94+168+28+74)

#define Act_MAX_ACTION_COD 1241

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

// Secondary actions
#define ActInsSch		(ActChgInsSta+  7)
#define ActPrnInsInf		(ActChgInsSta+  8)
#define ActChgInsLog		(ActChgInsSta+  9)

#define ActEdiCtr		(ActChgInsSta+ 10)
#define ActReqCtr		(ActChgInsSta+ 11)
#define ActNewCtr		(ActChgInsSta+ 12)
#define ActRemCtr		(ActChgInsSta+ 13)
#define ActChgCtrIns		(ActChgInsSta+ 14)
#define ActChgCtrPlc		(ActChgInsSta+ 15)
#define ActRenCtrSho		(ActChgInsSta+ 16)
#define ActRenCtrFul		(ActChgInsSta+ 17)
#define ActChgCtrWWW		(ActChgInsSta+ 18)
#define ActChgCtrSta		(ActChgInsSta+ 19)

#define ActEdiDpt		(ActChgInsSta+ 20)
#define ActNewDpt		(ActChgInsSta+ 21)
#define ActRemDpt		(ActChgInsSta+ 22)
#define ActChgDptIns		(ActChgInsSta+ 23)
#define ActRenDptSho		(ActChgInsSta+ 24)
#define ActRenDptFul		(ActChgInsSta+ 25)
#define ActChgDptWWW		(ActChgInsSta+ 26)

#define ActEdiPlc		(ActChgInsSta+ 27)
#define ActNewPlc		(ActChgInsSta+ 28)
#define ActRemPlc		(ActChgInsSta+ 29)
#define ActRenPlcSho		(ActChgInsSta+ 30)
#define ActRenPlcFul		(ActChgInsSta+ 31)

#define ActEdiHld		(ActChgInsSta+ 32)
#define ActNewHld		(ActChgInsSta+ 33)
#define ActRemHld		(ActChgInsSta+ 34)
#define ActChgHldPlc		(ActChgInsSta+ 35)
#define ActChgHldTyp		(ActChgInsSta+ 36)
#define ActChgHldStrDat		(ActChgInsSta+ 37)
#define ActChgHldEndDat		(ActChgInsSta+ 38)
#define ActRenHld		(ActChgInsSta+ 39)

/*****************************************************************************/
/********************************* Centre tab ********************************/
/*****************************************************************************/
// Actions in menu
#define ActCtrReqSch		(ActRenHld+  1)
#define ActSeeCtrInf		(ActRenHld+  2)
#define ActSeeDeg		(ActRenHld+  3)

// Secondary actions
#define ActCtrSch		(ActRenHld+  4)
#define ActPrnCtrInf		(ActRenHld+  5)
#define ActReqCtrPho		(ActRenHld+  6)
#define ActRecCtrPho		(ActRenHld+  7)
#define ActChgCtrPhoAtt		(ActRenHld+  8)
#define ActChgCtrLog		(ActRenHld+  9)

#define ActEdiDeg		(ActRenHld+ 10)
#define ActReqDeg		(ActRenHld+ 11)
#define ActNewDeg		(ActRenHld+ 12)
#define ActRemDeg		(ActRenHld+ 13)
#define ActRenDegSho		(ActRenHld+ 14)
#define ActRenDegFul		(ActRenHld+ 15)
#define ActChgDegTyp		(ActRenHld+ 16)
#define ActChgDegCtr		(ActRenHld+ 17)
#define ActChgDegFstYea		(ActRenHld+ 18)
#define ActChgDegLstYea		(ActRenHld+ 19)
#define ActChgDegOptYea		(ActRenHld+ 20)
#define ActChgDegWWW		(ActRenHld+ 21)
#define ActChgDegSta		(ActRenHld+ 22)

/*****************************************************************************/
/********************************* Degree tab ********************************/
/*****************************************************************************/
// Actions in menu
#define ActDegReqSch		(ActChgDegSta+  1)
#define ActSeeDegInf		(ActChgDegSta+  2)
#define ActSeeCrs		(ActChgDegSta+  3)

// Secondary actions
#define ActDegSch		(ActChgDegSta+  4)
#define ActPrnDegInf		(ActChgDegSta+  5)
#define ActChgDegLog		(ActChgDegSta+  6)

#define ActEdiCrs		(ActChgDegSta+  7)
#define ActReqCrs		(ActChgDegSta+  8)
#define ActNewCrs		(ActChgDegSta+  9)
#define ActRemCrs		(ActChgDegSta+ 10)
#define ActChgInsCrsCod		(ActChgDegSta+ 11)
#define ActChgCrsDeg		(ActChgDegSta+ 12)
#define ActChgCrsYea		(ActChgDegSta+ 13)
#define ActChgCrsSem		(ActChgDegSta+ 14)
#define ActRenCrsSho		(ActChgDegSta+ 15)
#define ActRenCrsFul		(ActChgDegSta+ 16)
#define ActChgCrsSta		(ActChgDegSta+ 17)

/*****************************************************************************/
/******************************** Course tab *********************************/
/*****************************************************************************/
// Actions in menu
#define ActCrsReqSch		(ActChgCrsSta+  1)
#define ActSeeCrsInf		(ActChgCrsSta+  2)
#define ActSeeTchGui		(ActChgCrsSta+  3)
#define ActSeeSylLec		(ActChgCrsSta+  4)
#define ActSeeSylPra		(ActChgCrsSta+  5)
#define ActSeeAdmDoc		(ActChgCrsSta+  6)
#define ActAdmCom		(ActChgCrsSta+  7)
#define ActSeeCal		(ActChgCrsSta+  8)
#define ActSeeCrsTimTbl		(ActChgCrsSta+  9)
#define ActSeeBib		(ActChgCrsSta+ 10)
#define ActSeeFAQ		(ActChgCrsSta+ 11)
#define ActSeeCrsLnk		(ActChgCrsSta+ 12)
// Secondary actions
#define ActCrsSch		(ActChgCrsSta+ 13)
#define ActPrnCrsInf		(ActChgCrsSta+ 14)
#define ActChgCrsLog		(ActChgCrsSta+ 15)
#define ActEdiCrsInf		(ActChgCrsSta+ 16)
#define ActEdiTchGui		(ActChgCrsSta+ 17)
#define ActPrnCal		(ActChgCrsSta+ 18)
#define ActPrnCrsTimTbl		(ActChgCrsSta+ 19)
#define ActEdiCrsTimTbl		(ActChgCrsSta+ 20)
#define ActChgCrsTimTbl		(ActChgCrsSta+ 21)
#define ActEdiSylLec		(ActChgCrsSta+ 22)
#define ActEdiSylPra		(ActChgCrsSta+ 23)
#define ActDelItmSylLec		(ActChgCrsSta+ 24)
#define ActDelItmSylPra		(ActChgCrsSta+ 25)
#define ActUp_IteSylLec		(ActChgCrsSta+ 26)
#define ActUp_IteSylPra		(ActChgCrsSta+ 27)
#define ActDwnIteSylLec		(ActChgCrsSta+ 28)
#define ActDwnIteSylPra		(ActChgCrsSta+ 29)
#define ActRgtIteSylLec		(ActChgCrsSta+ 30)
#define ActRgtIteSylPra		(ActChgCrsSta+ 31)
#define ActLftIteSylLec		(ActChgCrsSta+ 32)
#define ActLftIteSylPra		(ActChgCrsSta+ 33)
#define ActInsIteSylLec		(ActChgCrsSta+ 34)
#define ActInsIteSylPra		(ActChgCrsSta+ 35)
#define ActModIteSylLec		(ActChgCrsSta+ 36)
#define ActModIteSylPra		(ActChgCrsSta+ 37)

#define ActChgToSeeDoc		(ActChgCrsSta+ 38)

#define ActSeeDocCrs		(ActChgCrsSta+ 39)
#define ActExpSeeDocCrs		(ActChgCrsSta+ 40)
#define ActConSeeDocCrs		(ActChgCrsSta+ 41)
#define ActZIPSeeDocCrs		(ActChgCrsSta+ 42)
#define ActReqDatSeeDocCrs	(ActChgCrsSta+ 43)
#define ActDowSeeDocCrs		(ActChgCrsSta+ 44)

#define ActSeeDocGrp		(ActChgCrsSta+ 45)
#define ActExpSeeDocGrp		(ActChgCrsSta+ 46)
#define ActConSeeDocGrp		(ActChgCrsSta+ 47)
#define ActZIPSeeDocGrp		(ActChgCrsSta+ 48)
#define ActReqDatSeeDocGrp	(ActChgCrsSta+ 49)
#define ActDowSeeDocGrp		(ActChgCrsSta+ 50)

#define ActChgToAdmDoc		(ActChgCrsSta+ 51)

#define ActAdmDocCrs		(ActChgCrsSta+ 52)
#define ActReqRemFilDocCrs	(ActChgCrsSta+ 53)
#define ActRemFilDocCrs		(ActChgCrsSta+ 54)
#define ActRemFolDocCrs		(ActChgCrsSta+ 55)
#define ActCopDocCrs		(ActChgCrsSta+ 56)
#define ActPasDocCrs		(ActChgCrsSta+ 57)
#define ActRemTreDocCrs		(ActChgCrsSta+ 58)
#define ActFrmCreDocCrs		(ActChgCrsSta+ 59)
#define ActCreFolDocCrs		(ActChgCrsSta+ 60)
#define ActCreLnkDocCrs		(ActChgCrsSta+ 61)
#define ActRenFolDocCrs		(ActChgCrsSta+ 62)
#define ActRcvFilDocCrsDZ	(ActChgCrsSta+ 63)
#define ActRcvFilDocCrsCla	(ActChgCrsSta+ 64)
#define ActExpAdmDocCrs		(ActChgCrsSta+ 65)
#define ActConAdmDocCrs		(ActChgCrsSta+ 66)
#define ActZIPAdmDocCrs		(ActChgCrsSta+ 67)
#define ActShoDocCrs		(ActChgCrsSta+ 68)
#define ActHidDocCrs		(ActChgCrsSta+ 69)
#define ActReqDatAdmDocCrs	(ActChgCrsSta+ 70)
#define ActChgDatAdmDocCrs	(ActChgCrsSta+ 71)
#define ActDowAdmDocCrs		(ActChgCrsSta+ 72)

#define ActAdmDocGrp		(ActChgCrsSta+ 73)
#define ActReqRemFilDocGrp	(ActChgCrsSta+ 74)
#define ActRemFilDocGrp		(ActChgCrsSta+ 75)
#define ActRemFolDocGrp		(ActChgCrsSta+ 76)
#define ActCopDocGrp		(ActChgCrsSta+ 77)
#define ActPasDocGrp		(ActChgCrsSta+ 78)
#define ActRemTreDocGrp		(ActChgCrsSta+ 79)
#define ActFrmCreDocGrp		(ActChgCrsSta+ 80)
#define ActCreFolDocGrp		(ActChgCrsSta+ 81)
#define ActCreLnkDocGrp		(ActChgCrsSta+ 82)
#define ActRenFolDocGrp		(ActChgCrsSta+ 83)
#define ActRcvFilDocGrpDZ	(ActChgCrsSta+ 84)
#define ActRcvFilDocGrpCla	(ActChgCrsSta+ 85)
#define ActExpAdmDocGrp		(ActChgCrsSta+ 86)
#define ActConAdmDocGrp		(ActChgCrsSta+ 87)
#define ActZIPAdmDocGrp		(ActChgCrsSta+ 88)
#define ActShoDocGrp		(ActChgCrsSta+ 89)
#define ActHidDocGrp		(ActChgCrsSta+ 90)
#define ActReqDatAdmDocGrp	(ActChgCrsSta+ 91)
#define ActChgDatAdmDocGrp	(ActChgCrsSta+ 92)
#define ActDowAdmDocGrp		(ActChgCrsSta+ 93)

#define ActChgToAdmCom		(ActChgCrsSta+ 94)

#define ActAdmComCrs		(ActChgCrsSta+ 95)
#define ActReqRemFilComCrs	(ActChgCrsSta+ 96)
#define ActRemFilComCrs		(ActChgCrsSta+ 97)
#define ActRemFolComCrs		(ActChgCrsSta+ 98)
#define ActCopComCrs		(ActChgCrsSta+ 99)
#define ActPasComCrs		(ActChgCrsSta+100)
#define ActRemTreComCrs		(ActChgCrsSta+101)
#define ActFrmCreComCrs		(ActChgCrsSta+102)
#define ActCreFolComCrs		(ActChgCrsSta+103)
#define ActCreLnkComCrs		(ActChgCrsSta+104)
#define ActRenFolComCrs		(ActChgCrsSta+105)
#define ActRcvFilComCrsDZ	(ActChgCrsSta+106)
#define ActRcvFilComCrsCla	(ActChgCrsSta+107)
#define ActExpComCrs		(ActChgCrsSta+108)
#define ActConComCrs		(ActChgCrsSta+109)
#define ActZIPComCrs		(ActChgCrsSta+110)
#define ActReqDatComCrs		(ActChgCrsSta+111)
#define ActChgDatComCrs		(ActChgCrsSta+112)
#define ActDowComCrs		(ActChgCrsSta+113)

#define ActAdmComGrp		(ActChgCrsSta+114)
#define ActReqRemFilComGrp	(ActChgCrsSta+115)
#define ActRemFilComGrp		(ActChgCrsSta+116)
#define ActRemFolComGrp		(ActChgCrsSta+117)
#define ActCopComGrp		(ActChgCrsSta+118)
#define ActPasComGrp		(ActChgCrsSta+119)
#define ActRemTreComGrp		(ActChgCrsSta+120)
#define ActFrmCreComGrp		(ActChgCrsSta+121)
#define ActCreFolComGrp		(ActChgCrsSta+122)
#define ActCreLnkComGrp		(ActChgCrsSta+123)
#define ActRenFolComGrp		(ActChgCrsSta+124)
#define ActRcvFilComGrpDZ	(ActChgCrsSta+125)
#define ActRcvFilComGrpCla	(ActChgCrsSta+126)
#define ActExpComGrp		(ActChgCrsSta+127)
#define ActConComGrp		(ActChgCrsSta+128)
#define ActZIPComGrp		(ActChgCrsSta+129)
#define ActReqDatComGrp		(ActChgCrsSta+130)
#define ActChgDatComGrp		(ActChgCrsSta+131)
#define ActDowComGrp		(ActChgCrsSta+132)

#define ActEdiBib		(ActChgCrsSta+133)
#define ActEdiFAQ		(ActChgCrsSta+134)
#define ActEdiCrsLnk		(ActChgCrsSta+135)

#define ActChgFrcReaCrsInf	(ActChgCrsSta+136)
#define ActChgFrcReaTchGui	(ActChgCrsSta+137)
#define ActChgFrcReaSylLec	(ActChgCrsSta+138)
#define ActChgFrcReaSylPra	(ActChgCrsSta+139)
#define ActChgFrcReaBib		(ActChgCrsSta+140)
#define ActChgFrcReaFAQ		(ActChgCrsSta+141)
#define ActChgFrcReaCrsLnk	(ActChgCrsSta+142)

#define ActChgHavReaCrsInf	(ActChgCrsSta+143)
#define ActChgHavReaTchGui	(ActChgCrsSta+144)
#define ActChgHavReaSylLec	(ActChgCrsSta+145)
#define ActChgHavReaSylPra	(ActChgCrsSta+146)
#define ActChgHavReaBib		(ActChgCrsSta+147)
#define ActChgHavReaFAQ		(ActChgCrsSta+148)
#define ActChgHavReaCrsLnk	(ActChgCrsSta+149)

#define ActSelInfSrcCrsInf	(ActChgCrsSta+150)
#define ActSelInfSrcTchGui	(ActChgCrsSta+151)
#define ActSelInfSrcSylLec	(ActChgCrsSta+152)
#define ActSelInfSrcSylPra	(ActChgCrsSta+153)
#define ActSelInfSrcBib		(ActChgCrsSta+154)
#define ActSelInfSrcFAQ		(ActChgCrsSta+155)
#define ActSelInfSrcCrsLnk	(ActChgCrsSta+156)
#define ActRcvURLCrsInf		(ActChgCrsSta+157)
#define ActRcvURLTchGui		(ActChgCrsSta+158)
#define ActRcvURLSylLec		(ActChgCrsSta+159)
#define ActRcvURLSylPra		(ActChgCrsSta+160)
#define ActRcvURLBib		(ActChgCrsSta+161)
#define ActRcvURLFAQ		(ActChgCrsSta+162)
#define ActRcvURLCrsLnk		(ActChgCrsSta+163)
#define ActRcvPagCrsInf		(ActChgCrsSta+164)
#define ActRcvPagTchGui		(ActChgCrsSta+165)
#define ActRcvPagSylLec		(ActChgCrsSta+166)
#define ActRcvPagSylPra		(ActChgCrsSta+167)
#define ActRcvPagBib		(ActChgCrsSta+168)
#define ActRcvPagFAQ		(ActChgCrsSta+169)
#define ActRcvPagCrsLnk		(ActChgCrsSta+170)
#define ActEditorCrsInf		(ActChgCrsSta+171)
#define ActEditorTchGui		(ActChgCrsSta+172)
#define ActEditorSylLec		(ActChgCrsSta+173)
#define ActEditorSylPra		(ActChgCrsSta+174)
#define ActEditorBib		(ActChgCrsSta+175)
#define ActEditorFAQ		(ActChgCrsSta+176)
#define ActEditorCrsLnk		(ActChgCrsSta+177)
#define ActPlaTxtEdiCrsInf	(ActChgCrsSta+178)
#define ActPlaTxtEdiTchGui	(ActChgCrsSta+179)
#define ActPlaTxtEdiSylLec	(ActChgCrsSta+180)
#define ActPlaTxtEdiSylPra	(ActChgCrsSta+181)
#define ActPlaTxtEdiBib		(ActChgCrsSta+182)
#define ActPlaTxtEdiFAQ		(ActChgCrsSta+183)
#define ActPlaTxtEdiCrsLnk	(ActChgCrsSta+184)
#define ActRchTxtEdiCrsInf	(ActChgCrsSta+185)
#define ActRchTxtEdiTchGui	(ActChgCrsSta+186)
#define ActRchTxtEdiSylLec	(ActChgCrsSta+187)
#define ActRchTxtEdiSylPra	(ActChgCrsSta+188)
#define ActRchTxtEdiBib		(ActChgCrsSta+189)
#define ActRchTxtEdiFAQ		(ActChgCrsSta+190)
#define ActRchTxtEdiCrsLnk	(ActChgCrsSta+191)
#define ActRcvPlaTxtCrsInf	(ActChgCrsSta+192)
#define ActRcvPlaTxtTchGui	(ActChgCrsSta+193)
#define ActRcvPlaTxtSylLec	(ActChgCrsSta+194)
#define ActRcvPlaTxtSylPra	(ActChgCrsSta+195)
#define ActRcvPlaTxtBib		(ActChgCrsSta+196)
#define ActRcvPlaTxtFAQ		(ActChgCrsSta+197)
#define ActRcvPlaTxtCrsLnk	(ActChgCrsSta+198)
#define ActRcvRchTxtCrsInf	(ActChgCrsSta+199)
#define ActRcvRchTxtTchGui	(ActChgCrsSta+200)
#define ActRcvRchTxtSylLec	(ActChgCrsSta+201)
#define ActRcvRchTxtSylPra	(ActChgCrsSta+202)
#define ActRcvRchTxtBib		(ActChgCrsSta+203)
#define ActRcvRchTxtFAQ		(ActChgCrsSta+204)
#define ActRcvRchTxtCrsLnk	(ActChgCrsSta+205)

/*****************************************************************************/
/***************************** Assessment tab ********************************/
/*****************************************************************************/
// Actions in menu
#define ActSeeAss		(ActRcvRchTxtCrsLnk+  1)
#define ActSeeAsg		(ActRcvRchTxtCrsLnk+  2)
#define ActAdmAsgWrkUsr		(ActRcvRchTxtCrsLnk+  3)
#define ActReqAsgWrkCrs		(ActRcvRchTxtCrsLnk+  4)
#define ActReqTst		(ActRcvRchTxtCrsLnk+  5)
#define ActReqSeeMyTstExa	(ActRcvRchTxtCrsLnk+  6)
#define ActReqSeeUsrTstExa	(ActRcvRchTxtCrsLnk+  7)
#define ActSeeExaAnn		(ActRcvRchTxtCrsLnk+  8)
#define ActSeeAdmMrk		(ActRcvRchTxtCrsLnk+  9)
#define ActSeeRecCrs		(ActRcvRchTxtCrsLnk+ 10)
// Secondary actions
#define ActEdiAss		(ActRcvRchTxtCrsLnk+ 11)
#define ActChgFrcReaAss		(ActRcvRchTxtCrsLnk+ 12)
#define ActChgHavReaAss		(ActRcvRchTxtCrsLnk+ 13)
#define ActSelInfSrcAss		(ActRcvRchTxtCrsLnk+ 14)
#define ActRcvURLAss		(ActRcvRchTxtCrsLnk+ 15)
#define ActRcvPagAss		(ActRcvRchTxtCrsLnk+ 16)
#define ActEditorAss		(ActRcvRchTxtCrsLnk+ 17)
#define ActPlaTxtEdiAss		(ActRcvRchTxtCrsLnk+ 18)
#define ActRchTxtEdiAss		(ActRcvRchTxtCrsLnk+ 19)
#define ActRcvPlaTxtAss		(ActRcvRchTxtCrsLnk+ 20)
#define ActRcvRchTxtAss		(ActRcvRchTxtCrsLnk+ 21)

#define ActFrmNewAsg		(ActRcvRchTxtCrsLnk+ 22)
#define ActEdiOneAsg		(ActRcvRchTxtCrsLnk+ 23)
#define ActNewAsg		(ActRcvRchTxtCrsLnk+ 24)
#define ActChgAsg		(ActRcvRchTxtCrsLnk+ 25)
#define ActReqRemAsg		(ActRcvRchTxtCrsLnk+ 26)
#define ActRemAsg		(ActRcvRchTxtCrsLnk+ 27)
#define ActHidAsg		(ActRcvRchTxtCrsLnk+ 28)
#define ActShoAsg		(ActRcvRchTxtCrsLnk+ 29)

#define ActRcvRecCrs		(ActRcvRchTxtCrsLnk+ 30)
#define ActAdmAsgWrkCrs		(ActRcvRchTxtCrsLnk+ 31)

#define ActReqRemFilAsgUsr	(ActRcvRchTxtCrsLnk+ 32)
#define ActRemFilAsgUsr		(ActRcvRchTxtCrsLnk+ 33)
#define ActRemFolAsgUsr		(ActRcvRchTxtCrsLnk+ 34)
#define ActCopAsgUsr		(ActRcvRchTxtCrsLnk+ 35)
#define ActPasAsgUsr		(ActRcvRchTxtCrsLnk+ 36)
#define ActRemTreAsgUsr		(ActRcvRchTxtCrsLnk+ 37)
#define ActFrmCreAsgUsr		(ActRcvRchTxtCrsLnk+ 38)
#define ActCreFolAsgUsr		(ActRcvRchTxtCrsLnk+ 39)
#define ActCreLnkAsgUsr		(ActRcvRchTxtCrsLnk+ 40)
#define ActRenFolAsgUsr		(ActRcvRchTxtCrsLnk+ 41)
#define ActRcvFilAsgUsrDZ	(ActRcvRchTxtCrsLnk+ 42)
#define ActRcvFilAsgUsrCla	(ActRcvRchTxtCrsLnk+ 43)
#define ActExpAsgUsr		(ActRcvRchTxtCrsLnk+ 44)
#define ActConAsgUsr		(ActRcvRchTxtCrsLnk+ 45)
#define ActZIPAsgUsr		(ActRcvRchTxtCrsLnk+ 46)
#define ActReqDatAsgUsr		(ActRcvRchTxtCrsLnk+ 47)
#define ActChgDatAsgUsr		(ActRcvRchTxtCrsLnk+ 48)
#define ActDowAsgUsr		(ActRcvRchTxtCrsLnk+ 49)

#define ActReqRemFilWrkUsr	(ActRcvRchTxtCrsLnk+ 50)
#define ActRemFilWrkUsr		(ActRcvRchTxtCrsLnk+ 51)
#define ActRemFolWrkUsr		(ActRcvRchTxtCrsLnk+ 52)
#define ActCopWrkUsr		(ActRcvRchTxtCrsLnk+ 53)
#define ActPasWrkUsr		(ActRcvRchTxtCrsLnk+ 54)
#define ActRemTreWrkUsr		(ActRcvRchTxtCrsLnk+ 55)
#define ActFrmCreWrkUsr		(ActRcvRchTxtCrsLnk+ 56)
#define ActCreFolWrkUsr		(ActRcvRchTxtCrsLnk+ 57)
#define ActCreLnkWrkUsr		(ActRcvRchTxtCrsLnk+ 58)
#define ActRenFolWrkUsr		(ActRcvRchTxtCrsLnk+ 59)
#define ActRcvFilWrkUsrDZ	(ActRcvRchTxtCrsLnk+ 60)
#define ActRcvFilWrkUsrCla	(ActRcvRchTxtCrsLnk+ 61)
#define ActExpWrkUsr		(ActRcvRchTxtCrsLnk+ 62)
#define ActConWrkUsr		(ActRcvRchTxtCrsLnk+ 63)
#define ActZIPWrkUsr		(ActRcvRchTxtCrsLnk+ 64)
#define ActReqDatWrkUsr		(ActRcvRchTxtCrsLnk+ 65)
#define ActChgDatWrkUsr		(ActRcvRchTxtCrsLnk+ 66)
#define ActDowWrkUsr		(ActRcvRchTxtCrsLnk+ 67)

#define ActReqRemFilAsgCrs	(ActRcvRchTxtCrsLnk+ 68)
#define ActRemFilAsgCrs		(ActRcvRchTxtCrsLnk+ 69)
#define ActRemFolAsgCrs		(ActRcvRchTxtCrsLnk+ 70)
#define ActCopAsgCrs		(ActRcvRchTxtCrsLnk+ 71)
#define ActPasAsgCrs		(ActRcvRchTxtCrsLnk+ 72)
#define ActRemTreAsgCrs		(ActRcvRchTxtCrsLnk+ 73)
#define ActFrmCreAsgCrs		(ActRcvRchTxtCrsLnk+ 74)
#define ActCreFolAsgCrs		(ActRcvRchTxtCrsLnk+ 75)
#define ActCreLnkAsgCrs		(ActRcvRchTxtCrsLnk+ 76)
#define ActRenFolAsgCrs		(ActRcvRchTxtCrsLnk+ 77)
#define ActRcvFilAsgCrsDZ	(ActRcvRchTxtCrsLnk+ 78)
#define ActRcvFilAsgCrsCla	(ActRcvRchTxtCrsLnk+ 79)
#define ActExpAsgCrs		(ActRcvRchTxtCrsLnk+ 80)
#define ActConAsgCrs		(ActRcvRchTxtCrsLnk+ 81)
#define ActZIPAsgCrs		(ActRcvRchTxtCrsLnk+ 82)
#define ActReqDatAsgCrs		(ActRcvRchTxtCrsLnk+ 83)
#define ActChgDatAsgCrs		(ActRcvRchTxtCrsLnk+ 84)
#define ActDowAsgCrs		(ActRcvRchTxtCrsLnk+ 85)

#define ActReqRemFilWrkCrs	(ActRcvRchTxtCrsLnk+ 86)
#define ActRemFilWrkCrs		(ActRcvRchTxtCrsLnk+ 87)
#define ActRemFolWrkCrs		(ActRcvRchTxtCrsLnk+ 88)
#define ActCopWrkCrs		(ActRcvRchTxtCrsLnk+ 89)
#define ActPasWrkCrs		(ActRcvRchTxtCrsLnk+ 90)
#define ActRemTreWrkCrs		(ActRcvRchTxtCrsLnk+ 91)
#define ActFrmCreWrkCrs		(ActRcvRchTxtCrsLnk+ 92)
#define ActCreFolWrkCrs		(ActRcvRchTxtCrsLnk+ 93)
#define ActCreLnkWrkCrs		(ActRcvRchTxtCrsLnk+ 94)
#define ActRenFolWrkCrs		(ActRcvRchTxtCrsLnk+ 95)
#define ActRcvFilWrkCrsDZ	(ActRcvRchTxtCrsLnk+ 96)
#define ActRcvFilWrkCrsCla	(ActRcvRchTxtCrsLnk+ 97)
#define ActExpWrkCrs		(ActRcvRchTxtCrsLnk+ 98)
#define ActConWrkCrs		(ActRcvRchTxtCrsLnk+ 99)
#define ActZIPWrkCrs		(ActRcvRchTxtCrsLnk+100)
#define ActReqDatWrkCrs		(ActRcvRchTxtCrsLnk+101)
#define ActChgDatWrkCrs		(ActRcvRchTxtCrsLnk+102)
#define ActDowWrkCrs		(ActRcvRchTxtCrsLnk+103)

#define ActSeeTst		(ActRcvRchTxtCrsLnk+104)
#define ActAssTst		(ActRcvRchTxtCrsLnk+105)
#define ActEdiTstQst		(ActRcvRchTxtCrsLnk+106)
#define ActEdiOneTstQst		(ActRcvRchTxtCrsLnk+107)
#define ActReqImpTstQst		(ActRcvRchTxtCrsLnk+108)
#define ActImpTstQst		(ActRcvRchTxtCrsLnk+109)
#define ActLstTstQst		(ActRcvRchTxtCrsLnk+110)
#define ActRcvTstQst		(ActRcvRchTxtCrsLnk+111)
#define ActRemTstQst		(ActRcvRchTxtCrsLnk+112)
#define ActShfTstQst		(ActRcvRchTxtCrsLnk+113)
#define ActCfgTst		(ActRcvRchTxtCrsLnk+114)
#define ActEnableTag		(ActRcvRchTxtCrsLnk+115)
#define ActDisableTag		(ActRcvRchTxtCrsLnk+116)
#define ActRenTag		(ActRcvRchTxtCrsLnk+117)
#define ActRcvCfgTst		(ActRcvRchTxtCrsLnk+118)

#define ActSeeMyTstExa		(ActRcvRchTxtCrsLnk+119)
#define ActSeeUsrTstExa		(ActRcvRchTxtCrsLnk+120)
#define ActSeeOneTstExaMe	(ActRcvRchTxtCrsLnk+121)
#define ActSeeOneTstExaOth	(ActRcvRchTxtCrsLnk+122)

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
#define ActReqSelGrp		(ActChgNumRowFooGrp+ 1)
#define ActLstInv		(ActChgNumRowFooGrp+ 2)
#define ActLstStd		(ActChgNumRowFooGrp+ 3)
#define ActSeeAtt		(ActChgNumRowFooGrp+ 4)
#define ActLstTch		(ActChgNumRowFooGrp+ 5)
#define ActLstAdm		(ActChgNumRowFooGrp+ 6)
#define ActReqSignUp		(ActChgNumRowFooGrp+ 7)
#define ActSeeSignUpReq		(ActChgNumRowFooGrp+ 8)
#define ActReqMdfOneUsr		(ActChgNumRowFooGrp+ 9)
#define ActReqMdfSevUsr		(ActChgNumRowFooGrp+10)
#define ActLstCon		(ActChgNumRowFooGrp+11)

// Secondary actions
#define ActChgGrp		(ActChgNumRowFooGrp+12)
#define ActReqEdiGrp		(ActChgNumRowFooGrp+13)
#define ActNewGrpTyp		(ActChgNumRowFooGrp+14)
#define ActReqRemGrpTyp		(ActChgNumRowFooGrp+15)
#define ActRemGrpTyp		(ActChgNumRowFooGrp+16)
#define ActRenGrpTyp		(ActChgNumRowFooGrp+17)
#define ActChgMdtGrpTyp		(ActChgNumRowFooGrp+18)
#define ActChgMulGrpTyp		(ActChgNumRowFooGrp+19)
#define ActChgTimGrpTyp		(ActChgNumRowFooGrp+20)
#define ActNewGrp		(ActChgNumRowFooGrp+21)
#define ActReqRemGrp		(ActChgNumRowFooGrp+22)
#define ActRemGrp		(ActChgNumRowFooGrp+23)
#define ActOpeGrp		(ActChgNumRowFooGrp+24)
#define ActCloGrp		(ActChgNumRowFooGrp+25)
#define ActEnaFilZonGrp		(ActChgNumRowFooGrp+26)
#define ActDisFilZonGrp		(ActChgNumRowFooGrp+27)
#define ActChgGrpTyp		(ActChgNumRowFooGrp+28)
#define ActRenGrp		(ActChgNumRowFooGrp+29)
#define ActChgMaxStdGrp		(ActChgNumRowFooGrp+30)

#define ActGetExtLstStd		(ActChgNumRowFooGrp+31)
#define ActPrnInvPho		(ActChgNumRowFooGrp+32)
#define ActPrnStdPho		(ActChgNumRowFooGrp+33)
#define ActPrnTchPho		(ActChgNumRowFooGrp+34)
#define ActLstInvAll		(ActChgNumRowFooGrp+35)
#define ActLstStdAll		(ActChgNumRowFooGrp+36)
#define ActLstTchAll		(ActChgNumRowFooGrp+37)
#define ActSeeRecOneStd		(ActChgNumRowFooGrp+38)
#define ActSeeRecOneTch		(ActChgNumRowFooGrp+39)
#define ActSeeRecSevInv		(ActChgNumRowFooGrp+40)
#define ActSeeRecSevStd		(ActChgNumRowFooGrp+41)
#define ActSeeRecSevTch		(ActChgNumRowFooGrp+42)
#define ActPrnRecSevInv		(ActChgNumRowFooGrp+43)
#define ActPrnRecSevStd		(ActChgNumRowFooGrp+44)
#define ActPrnRecSevTch		(ActChgNumRowFooGrp+45)
#define ActRcvRecOthUsr		(ActChgNumRowFooGrp+46)
#define ActEdiRecFie		(ActChgNumRowFooGrp+47)
#define ActNewFie		(ActChgNumRowFooGrp+48)
#define ActReqRemFie		(ActChgNumRowFooGrp+49)
#define ActRemFie		(ActChgNumRowFooGrp+50)
#define ActRenFie		(ActChgNumRowFooGrp+51)
#define ActChgRowFie		(ActChgNumRowFooGrp+52)
#define ActChgVisFie		(ActChgNumRowFooGrp+53)

#define ActReqLstAttStd		(ActChgNumRowFooGrp+54)
#define ActSeeLstAttStd		(ActChgNumRowFooGrp+55)
#define ActPrnLstAttStd		(ActChgNumRowFooGrp+56)
#define ActFrmNewAtt		(ActChgNumRowFooGrp+57)
#define ActEdiOneAtt		(ActChgNumRowFooGrp+58)
#define ActNewAtt		(ActChgNumRowFooGrp+59)
#define ActChgAtt		(ActChgNumRowFooGrp+60)
#define ActReqRemAtt		(ActChgNumRowFooGrp+61)
#define ActRemAtt		(ActChgNumRowFooGrp+62)
#define ActHidAtt		(ActChgNumRowFooGrp+63)
#define ActShoAtt		(ActChgNumRowFooGrp+64)
#define ActSeeOneAtt		(ActChgNumRowFooGrp+65)
#define ActRecAttStd		(ActChgNumRowFooGrp+66)
#define ActRecAttMe		(ActChgNumRowFooGrp+67)

#define ActSignUp		(ActChgNumRowFooGrp+68)
#define ActReqRejSignUp		(ActChgNumRowFooGrp+69)
#define ActRejSignUp		(ActChgNumRowFooGrp+70)
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

#define ActNewAdm		(ActChgNumRowFooGrp+81)
#define ActRemAdm		(ActChgNumRowFooGrp+82)

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
#define ActLstBanUsr		(ActRemOldUsr+  9)
#define ActMaiStd		(ActRemOldUsr+ 10)
// Secondary actions
#define ActWriAnn		(ActRemOldUsr+ 11)
#define ActRcvAnn		(ActRemOldUsr+ 12)
#define ActRemAnn		(ActRemOldUsr+ 13)
#define ActShoNot		(ActRemOldUsr+ 14)
#define ActWriNot		(ActRemOldUsr+ 15)
#define ActRcvNot		(ActRemOldUsr+ 16)
#define ActHidNot		(ActRemOldUsr+ 17)
#define ActRevNot		(ActRemOldUsr+ 18)
#define ActRemNot		(ActRemOldUsr+ 19)
#define ActSeeNewNtf		(ActRemOldUsr+ 20)
#define ActMrkNtfSee		(ActRemOldUsr+ 21)
#define ActSeeForCrsUsr		(ActRemOldUsr+ 22)
#define ActSeeForCrsTch		(ActRemOldUsr+ 23)
#define ActSeeForDegUsr		(ActRemOldUsr+ 24)
#define ActSeeForDegTch		(ActRemOldUsr+ 25)
#define ActSeeForCtrUsr		(ActRemOldUsr+ 26)
#define ActSeeForCtrTch		(ActRemOldUsr+ 27)
#define ActSeeForInsUsr		(ActRemOldUsr+ 28)
#define ActSeeForInsTch		(ActRemOldUsr+ 29)
#define ActSeeForGenUsr		(ActRemOldUsr+ 30)
#define ActSeeForGenTch		(ActRemOldUsr+ 31)
#define ActSeeForSWAUsr		(ActRemOldUsr+ 32)
#define ActSeeForSWATch		(ActRemOldUsr+ 33)
#define ActSeePstForCrsUsr	(ActRemOldUsr+ 34)
#define ActSeePstForCrsTch	(ActRemOldUsr+ 35)
#define ActSeePstForDegUsr	(ActRemOldUsr+ 36)
#define ActSeePstForDegTch	(ActRemOldUsr+ 37)
#define ActSeePstForCtrUsr	(ActRemOldUsr+ 38)
#define ActSeePstForCtrTch	(ActRemOldUsr+ 39)
#define ActSeePstForInsUsr	(ActRemOldUsr+ 40)
#define ActSeePstForInsTch	(ActRemOldUsr+ 41)
#define ActSeePstForGenUsr	(ActRemOldUsr+ 42)
#define ActSeePstForGenTch	(ActRemOldUsr+ 43)
#define ActSeePstForSWAUsr	(ActRemOldUsr+ 44)
#define ActSeePstForSWATch	(ActRemOldUsr+ 45)
#define ActRcvThrForCrsUsr	(ActRemOldUsr+ 46)
#define ActRcvThrForCrsTch	(ActRemOldUsr+ 47)
#define ActRcvThrForDegUsr	(ActRemOldUsr+ 48)
#define ActRcvThrForDegTch	(ActRemOldUsr+ 49)
#define ActRcvThrForCtrUsr	(ActRemOldUsr+ 50)
#define ActRcvThrForCtrTch	(ActRemOldUsr+ 51)
#define ActRcvThrForInsUsr	(ActRemOldUsr+ 52)
#define ActRcvThrForInsTch	(ActRemOldUsr+ 53)
#define ActRcvThrForGenUsr	(ActRemOldUsr+ 54)
#define ActRcvThrForGenTch	(ActRemOldUsr+ 55)
#define ActRcvThrForSWAUsr	(ActRemOldUsr+ 56)
#define ActRcvThrForSWATch	(ActRemOldUsr+ 57)
#define ActRcvRepForCrsUsr	(ActRemOldUsr+ 58)
#define ActRcvRepForCrsTch	(ActRemOldUsr+ 59)
#define ActRcvRepForDegUsr	(ActRemOldUsr+ 60)
#define ActRcvRepForDegTch	(ActRemOldUsr+ 61)
#define ActRcvRepForCtrUsr	(ActRemOldUsr+ 62)
#define ActRcvRepForCtrTch	(ActRemOldUsr+ 63)
#define ActRcvRepForInsUsr	(ActRemOldUsr+ 64)
#define ActRcvRepForInsTch	(ActRemOldUsr+ 65)
#define ActRcvRepForGenUsr	(ActRemOldUsr+ 66)
#define ActRcvRepForGenTch	(ActRemOldUsr+ 67)
#define ActRcvRepForSWAUsr	(ActRemOldUsr+ 68)
#define ActRcvRepForSWATch	(ActRemOldUsr+ 69)
#define ActReqDelThrCrsUsr	(ActRemOldUsr+ 70)
#define ActReqDelThrCrsTch	(ActRemOldUsr+ 71)
#define ActReqDelThrDegUsr	(ActRemOldUsr+ 72)
#define ActReqDelThrDegTch	(ActRemOldUsr+ 73)
#define ActReqDelThrCtrUsr	(ActRemOldUsr+ 74)
#define ActReqDelThrCtrTch	(ActRemOldUsr+ 75)
#define ActReqDelThrInsUsr	(ActRemOldUsr+ 76)
#define ActReqDelThrInsTch	(ActRemOldUsr+ 77)
#define ActReqDelThrGenUsr	(ActRemOldUsr+ 78)
#define ActReqDelThrGenTch	(ActRemOldUsr+ 79)
#define ActReqDelThrSWAUsr	(ActRemOldUsr+ 80)
#define ActReqDelThrSWATch	(ActRemOldUsr+ 81)
#define ActDelThrForCrsUsr	(ActRemOldUsr+ 82)
#define ActDelThrForCrsTch	(ActRemOldUsr+ 83)
#define ActDelThrForDegUsr	(ActRemOldUsr+ 84)
#define ActDelThrForDegTch	(ActRemOldUsr+ 85)
#define ActDelThrForCtrUsr	(ActRemOldUsr+ 86)
#define ActDelThrForCtrTch	(ActRemOldUsr+ 87)
#define ActDelThrForInsUsr	(ActRemOldUsr+ 88)
#define ActDelThrForInsTch	(ActRemOldUsr+ 89)
#define ActDelThrForGenUsr	(ActRemOldUsr+ 90)
#define ActDelThrForGenTch	(ActRemOldUsr+ 91)
#define ActDelThrForSWAUsr	(ActRemOldUsr+ 92)
#define ActDelThrForSWATch	(ActRemOldUsr+ 93)
#define ActCutThrForCrsUsr	(ActRemOldUsr+ 94)
#define ActCutThrForCrsTch	(ActRemOldUsr+ 95)
#define ActCutThrForDegUsr	(ActRemOldUsr+ 96)
#define ActCutThrForDegTch	(ActRemOldUsr+ 97)
#define ActCutThrForCtrUsr	(ActRemOldUsr+ 98)
#define ActCutThrForCtrTch	(ActRemOldUsr+ 99)
#define ActCutThrForInsUsr	(ActRemOldUsr+100)
#define ActCutThrForInsTch	(ActRemOldUsr+101)
#define ActCutThrForGenUsr	(ActRemOldUsr+102)
#define ActCutThrForGenTch	(ActRemOldUsr+103)
#define ActCutThrForSWAUsr	(ActRemOldUsr+104)
#define ActCutThrForSWATch	(ActRemOldUsr+105)
#define ActPasThrForCrsUsr	(ActRemOldUsr+106)
#define ActPasThrForCrsTch	(ActRemOldUsr+107)
#define ActPasThrForDegUsr	(ActRemOldUsr+108)
#define ActPasThrForDegTch	(ActRemOldUsr+109)
#define ActPasThrForCtrUsr	(ActRemOldUsr+110)
#define ActPasThrForCtrTch	(ActRemOldUsr+111)
#define ActPasThrForInsUsr	(ActRemOldUsr+112)
#define ActPasThrForInsTch	(ActRemOldUsr+113)
#define ActPasThrForGenUsr	(ActRemOldUsr+114)
#define ActPasThrForGenTch	(ActRemOldUsr+115)
#define ActPasThrForSWAUsr	(ActRemOldUsr+116)
#define ActPasThrForSWATch	(ActRemOldUsr+117)
#define ActDelPstForCrsUsr	(ActRemOldUsr+118)
#define ActDelPstForCrsTch	(ActRemOldUsr+119)
#define ActDelPstForDegUsr	(ActRemOldUsr+120)
#define ActDelPstForDegTch	(ActRemOldUsr+121)
#define ActDelPstForCtrUsr	(ActRemOldUsr+122)
#define ActDelPstForCtrTch	(ActRemOldUsr+123)
#define ActDelPstForInsUsr	(ActRemOldUsr+124)
#define ActDelPstForInsTch	(ActRemOldUsr+125)
#define ActDelPstForGenUsr	(ActRemOldUsr+126)
#define ActDelPstForGenTch	(ActRemOldUsr+127)
#define ActDelPstForSWAUsr	(ActRemOldUsr+128)
#define ActDelPstForSWATch	(ActRemOldUsr+129)
#define ActEnbPstForCrsUsr	(ActRemOldUsr+130)
#define ActEnbPstForCrsTch	(ActRemOldUsr+131)
#define ActEnbPstForDegUsr	(ActRemOldUsr+132)
#define ActEnbPstForDegTch	(ActRemOldUsr+133)
#define ActEnbPstForCtrUsr	(ActRemOldUsr+134)
#define ActEnbPstForCtrTch	(ActRemOldUsr+135)
#define ActEnbPstForInsUsr	(ActRemOldUsr+136)
#define ActEnbPstForInsTch	(ActRemOldUsr+137)
#define ActEnbPstForGenUsr	(ActRemOldUsr+138)
#define ActEnbPstForGenTch	(ActRemOldUsr+139)
#define ActEnbPstForSWAUsr	(ActRemOldUsr+140)
#define ActEnbPstForSWATch	(ActRemOldUsr+141)
#define ActDisPstForCrsUsr	(ActRemOldUsr+142)
#define ActDisPstForCrsTch	(ActRemOldUsr+143)
#define ActDisPstForDegUsr	(ActRemOldUsr+144)
#define ActDisPstForDegTch	(ActRemOldUsr+145)
#define ActDisPstForCtrUsr	(ActRemOldUsr+146)
#define ActDisPstForCtrTch	(ActRemOldUsr+147)
#define ActDisPstForInsUsr	(ActRemOldUsr+148)
#define ActDisPstForInsTch	(ActRemOldUsr+149)
#define ActDisPstForGenUsr	(ActRemOldUsr+150)
#define ActDisPstForGenTch	(ActRemOldUsr+151)
#define ActDisPstForSWAUsr	(ActRemOldUsr+152)
#define ActDisPstForSWATch	(ActRemOldUsr+153)
#define ActRcvMsgUsr		(ActRemOldUsr+154)
#define ActReqDelAllSntMsg	(ActRemOldUsr+155)
#define ActReqDelAllRcvMsg	(ActRemOldUsr+156)
#define ActDelAllSntMsg		(ActRemOldUsr+157)
#define ActDelAllRcvMsg		(ActRemOldUsr+158)
#define ActDelSntMsg		(ActRemOldUsr+159)
#define ActDelRcvMsg		(ActRemOldUsr+160)
#define ActExpSntMsg		(ActRemOldUsr+161)
#define ActExpRcvMsg		(ActRemOldUsr+162)
#define ActConSntMsg		(ActRemOldUsr+163)
#define ActConRcvMsg		(ActRemOldUsr+164)
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
#define ActReqAccCrs		(ActCht+ 5)
#define ActReqAccGbl		(ActCht+ 6)
#define ActLstClk		(ActCht+ 7)
// Secondary actions
#define ActSeeOneSvy		(ActCht+ 8)
#define ActAnsSvy		(ActCht+ 9)
#define ActFrmNewSvy		(ActCht+10)
#define ActEdiOneSvy		(ActCht+11)
#define ActNewSvy		(ActCht+12)
#define ActChgSvy		(ActCht+13)
#define ActReqRemSvy		(ActCht+14)
#define ActRemSvy		(ActCht+15)
#define ActReqRstSvy		(ActCht+16)
#define ActRstSvy		(ActCht+17)
#define ActHidSvy		(ActCht+18)
#define ActShoSvy		(ActCht+19)
#define ActEdiOneSvyQst		(ActCht+20)
#define ActRcvSvyQst		(ActCht+21)
#define ActRemSvyQst		(ActCht+22)

#define ActSeeUseGbl		(ActCht+23)
#define ActPrnPhoDeg		(ActCht+24)
#define ActCalPhoDeg		(ActCht+25)
#define ActSeeAccGbl		(ActCht+26)
#define ActSeeAccCrs		(ActCht+27)
#define ActSeeAllStaCrs		(ActCht+28)

/*****************************************************************************/
/******************************** Profile tab ********************************/
/*****************************************************************************/
// Actions in menu
#define ActFrmLogIn		(ActSeeAllStaCrs+ 1)
#define ActFrmUsrAcc		(ActSeeAllStaCrs+ 2)
#define ActReqSndNewPwd		(ActSeeAllStaCrs+ 3)
#define ActFrmChgMyPwd		(ActSeeAllStaCrs+ 4)
#define ActReqEdiRecCom		(ActSeeAllStaCrs+ 5)
#define ActReqMyPho		(ActSeeAllStaCrs+ 6)
#define ActReqEdiMyIns		(ActSeeAllStaCrs+ 7)
#define ActReqEdiMyNet		(ActSeeAllStaCrs+ 8)
#define ActEdiPrf		(ActSeeAllStaCrs+ 9)
#define ActMyCrs		(ActSeeAllStaCrs+10)
#define ActSeeMyTimTbl		(ActSeeAllStaCrs+11)
#define ActAdmBrf		(ActSeeAllStaCrs+12)
#define ActMFUAct		(ActSeeAllStaCrs+13)
// Secondary actions
#define ActAutUsrInt		(ActSeeAllStaCrs+14)
#define ActAutUsrExt		(ActSeeAllStaCrs+15)
#define ActAutUsrChgLan		(ActSeeAllStaCrs+16)
#define ActLogOut		(ActSeeAllStaCrs+17)
#define ActAnnSee		(ActSeeAllStaCrs+18)
#define ActChgMyRol		(ActSeeAllStaCrs+19)
#define ActCreUsrAcc		(ActSeeAllStaCrs+20)
#define ActRemIDMe		(ActSeeAllStaCrs+21)
#define ActNewIDMe		(ActSeeAllStaCrs+22)
#define ActRemOldNic		(ActSeeAllStaCrs+23)
#define ActChgNic		(ActSeeAllStaCrs+24)
#define ActRemOldMai		(ActSeeAllStaCrs+25)
#define ActChgMai		(ActSeeAllStaCrs+26)
#define ActCnfMai		(ActSeeAllStaCrs+27)
#define ActSndNewPwd		(ActSeeAllStaCrs+28)
#define ActChgPwd		(ActSeeAllStaCrs+29)

#define ActChgMyData		(ActSeeAllStaCrs+30)

#define ActDetMyPho		(ActSeeAllStaCrs+31)
#define ActUpdMyPho		(ActSeeAllStaCrs+32)
#define ActRemMyPho		(ActSeeAllStaCrs+33)

#define ActChgCtyMyIns		(ActSeeAllStaCrs+34)
#define ActChgMyIns		(ActSeeAllStaCrs+35)
#define ActChgMyCtr		(ActSeeAllStaCrs+36)
#define ActChgMyDpt		(ActSeeAllStaCrs+37)
#define ActChgMyOff		(ActSeeAllStaCrs+38)
#define ActChgMyOffPho		(ActSeeAllStaCrs+39)

#define ActChgMyNet		(ActSeeAllStaCrs+40)

#define ActChgLay		(ActSeeAllStaCrs+41)
#define ActChgThe		(ActSeeAllStaCrs+42)
#define ActReqChgLan		(ActSeeAllStaCrs+43)
#define ActChgLan		(ActSeeAllStaCrs+44)
#define ActChgCol		(ActSeeAllStaCrs+45)
#define ActHidLftCol		(ActSeeAllStaCrs+46)
#define ActHidRgtCol		(ActSeeAllStaCrs+47)
#define ActShoLftCol		(ActSeeAllStaCrs+48)
#define ActShoRgtCol		(ActSeeAllStaCrs+49)
#define ActChgIco		(ActSeeAllStaCrs+50)
#define ActChgPubPho		(ActSeeAllStaCrs+51)
#define ActChgNtfPrf		(ActSeeAllStaCrs+52)

#define ActPrnUsrQR		(ActSeeAllStaCrs+53)

#define ActPrnMyTimTbl		(ActSeeAllStaCrs+54)
#define ActEdiTut		(ActSeeAllStaCrs+55)
#define ActChgTut		(ActSeeAllStaCrs+56)
#define ActReqRemFilBrf		(ActSeeAllStaCrs+57)
#define ActRemFilBrf		(ActSeeAllStaCrs+58)
#define ActRemFolBrf		(ActSeeAllStaCrs+59)
#define ActCopBrf		(ActSeeAllStaCrs+60)
#define ActPasBrf		(ActSeeAllStaCrs+61)
#define ActRemTreBrf		(ActSeeAllStaCrs+62)
#define ActFrmCreBrf		(ActSeeAllStaCrs+63)
#define ActCreFolBrf		(ActSeeAllStaCrs+64)
#define ActCreLnkBrf		(ActSeeAllStaCrs+65)
#define ActRenFolBrf		(ActSeeAllStaCrs+66)
#define ActRcvFilBrfDZ		(ActSeeAllStaCrs+67)
#define ActRcvFilBrfCla		(ActSeeAllStaCrs+68)
#define ActExpBrf		(ActSeeAllStaCrs+69)
#define ActConBrf		(ActSeeAllStaCrs+70)
#define ActZIPBrf		(ActSeeAllStaCrs+71)
#define ActReqDatBrf		(ActSeeAllStaCrs+72)
#define ActChgDatBrf		(ActSeeAllStaCrs+73)
#define ActDowBrf		(ActSeeAllStaCrs+74)

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
const char *Act_GetMenuTxtAction (Act_Action_t Action);
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
