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
#define Act_NUM_ACTIONS	(7+52+15+39+22+17+206+182+94+168+28+74)

#define Act_MAX_ACTION_COD 1242

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
#define ActSeeSyl		(ActChgCrsSta+  4)
#define ActSeeAdmDoc		(ActChgCrsSta+  5)
#define ActAdmCom		(ActChgCrsSta+  6)
#define ActSeeCal		(ActChgCrsSta+  7)
#define ActSeeCrsTimTbl		(ActChgCrsSta+  8)
#define ActSeeBib		(ActChgCrsSta+  9)
#define ActSeeFAQ		(ActChgCrsSta+ 10)
#define ActSeeCrsLnk		(ActChgCrsSta+ 11)
// Secondary actions
#define ActCrsSch		(ActChgCrsSta+ 12)
#define ActPrnCrsInf		(ActChgCrsSta+ 13)
#define ActChgCrsLog		(ActChgCrsSta+ 14)
#define ActEdiCrsInf		(ActChgCrsSta+ 15)
#define ActEdiTchGui		(ActChgCrsSta+ 16)
#define ActPrnCal		(ActChgCrsSta+ 17)
#define ActPrnCrsTimTbl		(ActChgCrsSta+ 18)
#define ActEdiCrsTimTbl		(ActChgCrsSta+ 19)
#define ActChgCrsTimTbl		(ActChgCrsSta+ 20)
#define ActSeeSylLec		(ActChgCrsSta+ 21)
#define ActSeeSylPra		(ActChgCrsSta+ 22)
#define ActEdiSylLec		(ActChgCrsSta+ 23)
#define ActEdiSylPra		(ActChgCrsSta+ 24)
#define ActDelItmSylLec		(ActChgCrsSta+ 25)
#define ActDelItmSylPra		(ActChgCrsSta+ 26)
#define ActUp_IteSylLec		(ActChgCrsSta+ 27)
#define ActUp_IteSylPra		(ActChgCrsSta+ 28)
#define ActDwnIteSylLec		(ActChgCrsSta+ 29)
#define ActDwnIteSylPra		(ActChgCrsSta+ 30)
#define ActRgtIteSylLec		(ActChgCrsSta+ 31)
#define ActRgtIteSylPra		(ActChgCrsSta+ 32)
#define ActLftIteSylLec		(ActChgCrsSta+ 33)
#define ActLftIteSylPra		(ActChgCrsSta+ 34)
#define ActInsIteSylLec		(ActChgCrsSta+ 35)
#define ActInsIteSylPra		(ActChgCrsSta+ 36)
#define ActModIteSylLec		(ActChgCrsSta+ 37)
#define ActModIteSylPra		(ActChgCrsSta+ 38)

#define ActChgToSeeDoc		(ActChgCrsSta+ 39)

#define ActSeeDocCrs		(ActChgCrsSta+ 40)
#define ActExpSeeDocCrs		(ActChgCrsSta+ 41)
#define ActConSeeDocCrs		(ActChgCrsSta+ 42)
#define ActZIPSeeDocCrs		(ActChgCrsSta+ 43)
#define ActReqDatSeeDocCrs	(ActChgCrsSta+ 44)
#define ActDowSeeDocCrs		(ActChgCrsSta+ 45)

#define ActSeeDocGrp		(ActChgCrsSta+ 46)
#define ActExpSeeDocGrp		(ActChgCrsSta+ 47)
#define ActConSeeDocGrp		(ActChgCrsSta+ 48)
#define ActZIPSeeDocGrp		(ActChgCrsSta+ 49)
#define ActReqDatSeeDocGrp	(ActChgCrsSta+ 50)
#define ActDowSeeDocGrp		(ActChgCrsSta+ 51)

#define ActChgToAdmDoc		(ActChgCrsSta+ 52)

#define ActAdmDocCrs		(ActChgCrsSta+ 53)
#define ActReqRemFilDocCrs	(ActChgCrsSta+ 54)
#define ActRemFilDocCrs		(ActChgCrsSta+ 55)
#define ActRemFolDocCrs		(ActChgCrsSta+ 56)
#define ActCopDocCrs		(ActChgCrsSta+ 57)
#define ActPasDocCrs		(ActChgCrsSta+ 58)
#define ActRemTreDocCrs		(ActChgCrsSta+ 59)
#define ActFrmCreDocCrs		(ActChgCrsSta+ 60)
#define ActCreFolDocCrs		(ActChgCrsSta+ 61)
#define ActCreLnkDocCrs		(ActChgCrsSta+ 62)
#define ActRenFolDocCrs		(ActChgCrsSta+ 63)
#define ActRcvFilDocCrsDZ	(ActChgCrsSta+ 64)
#define ActRcvFilDocCrsCla	(ActChgCrsSta+ 65)
#define ActExpAdmDocCrs		(ActChgCrsSta+ 66)
#define ActConAdmDocCrs		(ActChgCrsSta+ 67)
#define ActZIPAdmDocCrs		(ActChgCrsSta+ 68)
#define ActShoDocCrs		(ActChgCrsSta+ 69)
#define ActHidDocCrs		(ActChgCrsSta+ 70)
#define ActReqDatAdmDocCrs	(ActChgCrsSta+ 71)
#define ActChgDatAdmDocCrs	(ActChgCrsSta+ 72)
#define ActDowAdmDocCrs		(ActChgCrsSta+ 73)

#define ActAdmDocGrp		(ActChgCrsSta+ 74)
#define ActReqRemFilDocGrp	(ActChgCrsSta+ 75)
#define ActRemFilDocGrp		(ActChgCrsSta+ 76)
#define ActRemFolDocGrp		(ActChgCrsSta+ 77)
#define ActCopDocGrp		(ActChgCrsSta+ 78)
#define ActPasDocGrp		(ActChgCrsSta+ 79)
#define ActRemTreDocGrp		(ActChgCrsSta+ 80)
#define ActFrmCreDocGrp		(ActChgCrsSta+ 81)
#define ActCreFolDocGrp		(ActChgCrsSta+ 82)
#define ActCreLnkDocGrp		(ActChgCrsSta+ 83)
#define ActRenFolDocGrp		(ActChgCrsSta+ 84)
#define ActRcvFilDocGrpDZ	(ActChgCrsSta+ 85)
#define ActRcvFilDocGrpCla	(ActChgCrsSta+ 86)
#define ActExpAdmDocGrp		(ActChgCrsSta+ 87)
#define ActConAdmDocGrp		(ActChgCrsSta+ 88)
#define ActZIPAdmDocGrp		(ActChgCrsSta+ 89)
#define ActShoDocGrp		(ActChgCrsSta+ 90)
#define ActHidDocGrp		(ActChgCrsSta+ 91)
#define ActReqDatAdmDocGrp	(ActChgCrsSta+ 92)
#define ActChgDatAdmDocGrp	(ActChgCrsSta+ 93)
#define ActDowAdmDocGrp		(ActChgCrsSta+ 94)

#define ActChgToAdmCom		(ActChgCrsSta+ 95)

#define ActAdmComCrs		(ActChgCrsSta+ 96)
#define ActReqRemFilComCrs	(ActChgCrsSta+ 97)
#define ActRemFilComCrs		(ActChgCrsSta+ 98)
#define ActRemFolComCrs		(ActChgCrsSta+ 99)
#define ActCopComCrs		(ActChgCrsSta+100)
#define ActPasComCrs		(ActChgCrsSta+101)
#define ActRemTreComCrs		(ActChgCrsSta+102)
#define ActFrmCreComCrs		(ActChgCrsSta+103)
#define ActCreFolComCrs		(ActChgCrsSta+104)
#define ActCreLnkComCrs		(ActChgCrsSta+105)
#define ActRenFolComCrs		(ActChgCrsSta+106)
#define ActRcvFilComCrsDZ	(ActChgCrsSta+107)
#define ActRcvFilComCrsCla	(ActChgCrsSta+108)
#define ActExpComCrs		(ActChgCrsSta+109)
#define ActConComCrs		(ActChgCrsSta+110)
#define ActZIPComCrs		(ActChgCrsSta+111)
#define ActReqDatComCrs		(ActChgCrsSta+112)
#define ActChgDatComCrs		(ActChgCrsSta+113)
#define ActDowComCrs		(ActChgCrsSta+114)

#define ActAdmComGrp		(ActChgCrsSta+115)
#define ActReqRemFilComGrp	(ActChgCrsSta+116)
#define ActRemFilComGrp		(ActChgCrsSta+117)
#define ActRemFolComGrp		(ActChgCrsSta+118)
#define ActCopComGrp		(ActChgCrsSta+119)
#define ActPasComGrp		(ActChgCrsSta+120)
#define ActRemTreComGrp		(ActChgCrsSta+121)
#define ActFrmCreComGrp		(ActChgCrsSta+122)
#define ActCreFolComGrp		(ActChgCrsSta+123)
#define ActCreLnkComGrp		(ActChgCrsSta+124)
#define ActRenFolComGrp		(ActChgCrsSta+125)
#define ActRcvFilComGrpDZ	(ActChgCrsSta+126)
#define ActRcvFilComGrpCla	(ActChgCrsSta+127)
#define ActExpComGrp		(ActChgCrsSta+128)
#define ActConComGrp		(ActChgCrsSta+129)
#define ActZIPComGrp		(ActChgCrsSta+130)
#define ActReqDatComGrp		(ActChgCrsSta+131)
#define ActChgDatComGrp		(ActChgCrsSta+132)
#define ActDowComGrp		(ActChgCrsSta+133)

#define ActEdiBib		(ActChgCrsSta+134)
#define ActEdiFAQ		(ActChgCrsSta+135)
#define ActEdiCrsLnk		(ActChgCrsSta+136)

#define ActChgFrcReaCrsInf	(ActChgCrsSta+137)
#define ActChgFrcReaTchGui	(ActChgCrsSta+138)
#define ActChgFrcReaSylLec	(ActChgCrsSta+139)
#define ActChgFrcReaSylPra	(ActChgCrsSta+140)
#define ActChgFrcReaBib		(ActChgCrsSta+141)
#define ActChgFrcReaFAQ		(ActChgCrsSta+142)
#define ActChgFrcReaCrsLnk	(ActChgCrsSta+143)

#define ActChgHavReaCrsInf	(ActChgCrsSta+144)
#define ActChgHavReaTchGui	(ActChgCrsSta+145)
#define ActChgHavReaSylLec	(ActChgCrsSta+146)
#define ActChgHavReaSylPra	(ActChgCrsSta+147)
#define ActChgHavReaBib		(ActChgCrsSta+148)
#define ActChgHavReaFAQ		(ActChgCrsSta+149)
#define ActChgHavReaCrsLnk	(ActChgCrsSta+150)

#define ActSelInfSrcCrsInf	(ActChgCrsSta+151)
#define ActSelInfSrcTchGui	(ActChgCrsSta+152)
#define ActSelInfSrcSylLec	(ActChgCrsSta+153)
#define ActSelInfSrcSylPra	(ActChgCrsSta+154)
#define ActSelInfSrcBib		(ActChgCrsSta+155)
#define ActSelInfSrcFAQ		(ActChgCrsSta+156)
#define ActSelInfSrcCrsLnk	(ActChgCrsSta+157)
#define ActRcvURLCrsInf		(ActChgCrsSta+158)
#define ActRcvURLTchGui		(ActChgCrsSta+159)
#define ActRcvURLSylLec		(ActChgCrsSta+160)
#define ActRcvURLSylPra		(ActChgCrsSta+161)
#define ActRcvURLBib		(ActChgCrsSta+162)
#define ActRcvURLFAQ		(ActChgCrsSta+163)
#define ActRcvURLCrsLnk		(ActChgCrsSta+164)
#define ActRcvPagCrsInf		(ActChgCrsSta+165)
#define ActRcvPagTchGui		(ActChgCrsSta+166)
#define ActRcvPagSylLec		(ActChgCrsSta+167)
#define ActRcvPagSylPra		(ActChgCrsSta+168)
#define ActRcvPagBib		(ActChgCrsSta+169)
#define ActRcvPagFAQ		(ActChgCrsSta+170)
#define ActRcvPagCrsLnk		(ActChgCrsSta+171)
#define ActEditorCrsInf		(ActChgCrsSta+172)
#define ActEditorTchGui		(ActChgCrsSta+173)
#define ActEditorSylLec		(ActChgCrsSta+174)
#define ActEditorSylPra		(ActChgCrsSta+175)
#define ActEditorBib		(ActChgCrsSta+176)
#define ActEditorFAQ		(ActChgCrsSta+177)
#define ActEditorCrsLnk		(ActChgCrsSta+178)
#define ActPlaTxtEdiCrsInf	(ActChgCrsSta+179)
#define ActPlaTxtEdiTchGui	(ActChgCrsSta+180)
#define ActPlaTxtEdiSylLec	(ActChgCrsSta+181)
#define ActPlaTxtEdiSylPra	(ActChgCrsSta+182)
#define ActPlaTxtEdiBib		(ActChgCrsSta+183)
#define ActPlaTxtEdiFAQ		(ActChgCrsSta+184)
#define ActPlaTxtEdiCrsLnk	(ActChgCrsSta+185)
#define ActRchTxtEdiCrsInf	(ActChgCrsSta+186)
#define ActRchTxtEdiTchGui	(ActChgCrsSta+187)
#define ActRchTxtEdiSylLec	(ActChgCrsSta+188)
#define ActRchTxtEdiSylPra	(ActChgCrsSta+189)
#define ActRchTxtEdiBib		(ActChgCrsSta+190)
#define ActRchTxtEdiFAQ		(ActChgCrsSta+191)
#define ActRchTxtEdiCrsLnk	(ActChgCrsSta+192)
#define ActRcvPlaTxtCrsInf	(ActChgCrsSta+193)
#define ActRcvPlaTxtTchGui	(ActChgCrsSta+194)
#define ActRcvPlaTxtSylLec	(ActChgCrsSta+195)
#define ActRcvPlaTxtSylPra	(ActChgCrsSta+196)
#define ActRcvPlaTxtBib		(ActChgCrsSta+197)
#define ActRcvPlaTxtFAQ		(ActChgCrsSta+198)
#define ActRcvPlaTxtCrsLnk	(ActChgCrsSta+199)
#define ActRcvRchTxtCrsInf	(ActChgCrsSta+200)
#define ActRcvRchTxtTchGui	(ActChgCrsSta+201)
#define ActRcvRchTxtSylLec	(ActChgCrsSta+202)
#define ActRcvRchTxtSylPra	(ActChgCrsSta+203)
#define ActRcvRchTxtBib		(ActChgCrsSta+204)
#define ActRcvRchTxtFAQ		(ActChgCrsSta+205)
#define ActRcvRchTxtCrsLnk	(ActChgCrsSta+206)

/*****************************************************************************/
/***************************** Assessment tab ********************************/
/*****************************************************************************/
// Actions in menu
#define ActSeeAss		(ActRcvRchTxtCrsLnk+  1)
#define ActSeeAsg		(ActRcvRchTxtCrsLnk+  2)
#define ActAdmAsgWrkUsr		(ActRcvRchTxtCrsLnk+  3)
#define ActReqAsgWrkCrs		(ActRcvRchTxtCrsLnk+  4)
#define ActReqTst		(ActRcvRchTxtCrsLnk+  5)
#define ActSeeExaAnn		(ActRcvRchTxtCrsLnk+  6)
#define ActSeeAdmMrk		(ActRcvRchTxtCrsLnk+  7)
#define ActSeeRecCrs		(ActRcvRchTxtCrsLnk+  8)
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

#define ActRcvRecCrs		(ActRcvRchTxtCrsLnk+ 28)
#define ActAdmAsgWrkCrs		(ActRcvRchTxtCrsLnk+ 29)

#define ActReqRemFilAsgUsr	(ActRcvRchTxtCrsLnk+ 30)
#define ActRemFilAsgUsr		(ActRcvRchTxtCrsLnk+ 31)
#define ActRemFolAsgUsr		(ActRcvRchTxtCrsLnk+ 32)
#define ActCopAsgUsr		(ActRcvRchTxtCrsLnk+ 33)
#define ActPasAsgUsr		(ActRcvRchTxtCrsLnk+ 34)
#define ActRemTreAsgUsr		(ActRcvRchTxtCrsLnk+ 35)
#define ActFrmCreAsgUsr		(ActRcvRchTxtCrsLnk+ 36)
#define ActCreFolAsgUsr		(ActRcvRchTxtCrsLnk+ 37)
#define ActCreLnkAsgUsr		(ActRcvRchTxtCrsLnk+ 38)
#define ActRenFolAsgUsr		(ActRcvRchTxtCrsLnk+ 39)
#define ActRcvFilAsgUsrDZ	(ActRcvRchTxtCrsLnk+ 40)
#define ActRcvFilAsgUsrCla	(ActRcvRchTxtCrsLnk+ 41)
#define ActExpAsgUsr		(ActRcvRchTxtCrsLnk+ 42)
#define ActConAsgUsr		(ActRcvRchTxtCrsLnk+ 43)
#define ActZIPAsgUsr		(ActRcvRchTxtCrsLnk+ 44)
#define ActReqDatAsgUsr		(ActRcvRchTxtCrsLnk+ 45)
#define ActChgDatAsgUsr		(ActRcvRchTxtCrsLnk+ 46)
#define ActDowAsgUsr		(ActRcvRchTxtCrsLnk+ 47)

#define ActReqRemFilWrkUsr	(ActRcvRchTxtCrsLnk+ 48)
#define ActRemFilWrkUsr		(ActRcvRchTxtCrsLnk+ 49)
#define ActRemFolWrkUsr		(ActRcvRchTxtCrsLnk+ 50)
#define ActCopWrkUsr		(ActRcvRchTxtCrsLnk+ 51)
#define ActPasWrkUsr		(ActRcvRchTxtCrsLnk+ 52)
#define ActRemTreWrkUsr		(ActRcvRchTxtCrsLnk+ 53)
#define ActFrmCreWrkUsr		(ActRcvRchTxtCrsLnk+ 54)
#define ActCreFolWrkUsr		(ActRcvRchTxtCrsLnk+ 55)
#define ActCreLnkWrkUsr		(ActRcvRchTxtCrsLnk+ 56)
#define ActRenFolWrkUsr		(ActRcvRchTxtCrsLnk+ 57)
#define ActRcvFilWrkUsrDZ	(ActRcvRchTxtCrsLnk+ 58)
#define ActRcvFilWrkUsrCla	(ActRcvRchTxtCrsLnk+ 59)
#define ActExpWrkUsr		(ActRcvRchTxtCrsLnk+ 60)
#define ActConWrkUsr		(ActRcvRchTxtCrsLnk+ 61)
#define ActZIPWrkUsr		(ActRcvRchTxtCrsLnk+ 62)
#define ActReqDatWrkUsr		(ActRcvRchTxtCrsLnk+ 63)
#define ActChgDatWrkUsr		(ActRcvRchTxtCrsLnk+ 64)
#define ActDowWrkUsr		(ActRcvRchTxtCrsLnk+ 65)

#define ActReqRemFilAsgCrs	(ActRcvRchTxtCrsLnk+ 66)
#define ActRemFilAsgCrs		(ActRcvRchTxtCrsLnk+ 67)
#define ActRemFolAsgCrs		(ActRcvRchTxtCrsLnk+ 68)
#define ActCopAsgCrs		(ActRcvRchTxtCrsLnk+ 69)
#define ActPasAsgCrs		(ActRcvRchTxtCrsLnk+ 70)
#define ActRemTreAsgCrs		(ActRcvRchTxtCrsLnk+ 71)
#define ActFrmCreAsgCrs		(ActRcvRchTxtCrsLnk+ 72)
#define ActCreFolAsgCrs		(ActRcvRchTxtCrsLnk+ 73)
#define ActCreLnkAsgCrs		(ActRcvRchTxtCrsLnk+ 74)
#define ActRenFolAsgCrs		(ActRcvRchTxtCrsLnk+ 75)
#define ActRcvFilAsgCrsDZ	(ActRcvRchTxtCrsLnk+ 76)
#define ActRcvFilAsgCrsCla	(ActRcvRchTxtCrsLnk+ 77)
#define ActExpAsgCrs		(ActRcvRchTxtCrsLnk+ 78)
#define ActConAsgCrs		(ActRcvRchTxtCrsLnk+ 79)
#define ActZIPAsgCrs		(ActRcvRchTxtCrsLnk+ 80)
#define ActReqDatAsgCrs		(ActRcvRchTxtCrsLnk+ 81)
#define ActChgDatAsgCrs		(ActRcvRchTxtCrsLnk+ 82)
#define ActDowAsgCrs		(ActRcvRchTxtCrsLnk+ 83)

#define ActReqRemFilWrkCrs	(ActRcvRchTxtCrsLnk+ 84)
#define ActRemFilWrkCrs		(ActRcvRchTxtCrsLnk+ 85)
#define ActRemFolWrkCrs		(ActRcvRchTxtCrsLnk+ 86)
#define ActCopWrkCrs		(ActRcvRchTxtCrsLnk+ 87)
#define ActPasWrkCrs		(ActRcvRchTxtCrsLnk+ 88)
#define ActRemTreWrkCrs		(ActRcvRchTxtCrsLnk+ 89)
#define ActFrmCreWrkCrs		(ActRcvRchTxtCrsLnk+ 90)
#define ActCreFolWrkCrs		(ActRcvRchTxtCrsLnk+ 91)
#define ActCreLnkWrkCrs		(ActRcvRchTxtCrsLnk+ 92)
#define ActRenFolWrkCrs		(ActRcvRchTxtCrsLnk+ 93)
#define ActRcvFilWrkCrsDZ	(ActRcvRchTxtCrsLnk+ 94)
#define ActRcvFilWrkCrsCla	(ActRcvRchTxtCrsLnk+ 95)
#define ActExpWrkCrs		(ActRcvRchTxtCrsLnk+ 96)
#define ActConWrkCrs		(ActRcvRchTxtCrsLnk+ 97)
#define ActZIPWrkCrs		(ActRcvRchTxtCrsLnk+ 98)
#define ActReqDatWrkCrs		(ActRcvRchTxtCrsLnk+ 99)
#define ActChgDatWrkCrs		(ActRcvRchTxtCrsLnk+100)
#define ActDowWrkCrs		(ActRcvRchTxtCrsLnk+101)

#define ActSeeTst		(ActRcvRchTxtCrsLnk+102)
#define ActAssTst		(ActRcvRchTxtCrsLnk+103)
#define ActEdiTstQst		(ActRcvRchTxtCrsLnk+104)
#define ActEdiOneTstQst		(ActRcvRchTxtCrsLnk+105)
#define ActReqImpTstQst		(ActRcvRchTxtCrsLnk+106)
#define ActImpTstQst		(ActRcvRchTxtCrsLnk+107)
#define ActLstTstQst		(ActRcvRchTxtCrsLnk+108)
#define ActRcvTstQst		(ActRcvRchTxtCrsLnk+109)
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

#define ActReqEdiMyIns		(ActSeeAllStaCrs+32)
#define ActChgCtyMyIns		(ActSeeAllStaCrs+33)
#define ActChgMyIns		(ActSeeAllStaCrs+34)
#define ActChgMyCtr		(ActSeeAllStaCrs+35)
#define ActChgMyDpt		(ActSeeAllStaCrs+36)
#define ActChgMyOff		(ActSeeAllStaCrs+37)
#define ActChgMyOffPho		(ActSeeAllStaCrs+38)

#define ActReqEdiMyNet		(ActSeeAllStaCrs+39)
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
