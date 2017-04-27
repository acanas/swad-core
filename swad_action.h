// swad_action.h: actions

#ifndef _SWAD_ACT
#define _SWAD_ACT
/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2017 Antonio Cañas Vargas

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
#include "swad_string.h"
#include "swad_tab.h"
#include "swad_text.h"

/*****************************************************************************/
/****************************** Public constants *****************************/
/*****************************************************************************/

#define Act_MAX_CHARS_ACTION_TXT	(256 - 1)			// 255
#define Act_MAX_BYTES_ACTION_TXT	Act_MAX_CHARS_ACTION_TXT	// 255

typedef enum
  {
   Act_CONT_NORM,
   Act_CONT_DATA,
  } Act_Content_t;

typedef enum
  {
   Act_THIS_WINDOW,	// The current tab in the browser
   Act_BLNK_WINDOW,	// A new blank tab in the browser
   Act_UPLOAD_FILE,	// Upload a file. Do not write HTML content. Write Status code instead for Dropzone.js
   Act_DOWNLD_FILE,	// Download a file in a new window. Do not write HTML content.
  } Act_BrowserWindow_t;

typedef signed int Act_Action_t;	// Must be a signed type, because -1 is used to indicate obsolete action

#define Act_NUM_ACTIONS	(1 + 9 + 43 + 17 + 47 + 32 + 24 + 115 + 71 + 416 + 159 + 172 + 42 + 14 + 96)

#define Act_MAX_ACTION_COD 1636

#define Act_MAX_OPTIONS_IN_MENU_PER_TAB 12

#define Act_MAX_BYTES_ID (32 + Cry_BYTES_ENCRYPTED_STR_SHA256_BASE64 + 10 + 1)

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
#define ActSysReqSch		(ActWebSvc +  1)
#define ActSeeCty		(ActWebSvc +  2)
#define ActSeePen		(ActWebSvc +  3)
#define ActSeeLnk		(ActWebSvc +  4)
#define ActLstPlg		(ActWebSvc +  5)
#define ActSetUp		(ActWebSvc +  6)
#define ActSeeCalSys		(ActWebSvc +  7)

// Secondary actions
#define ActSysSch		(ActWebSvc +  8)
#define ActEdiCty		(ActWebSvc +  9)
#define ActNewCty		(ActWebSvc + 10)
#define ActRemCty		(ActWebSvc + 11)
#define ActRenCty		(ActWebSvc + 12)
#define ActChgCtyWWW		(ActWebSvc + 13)

#define ActReqRemOldCrs		(ActWebSvc + 14)
#define ActRemOldCrs		(ActWebSvc + 15)

#define ActSeeBan		(ActWebSvc + 16)
#define ActEdiBan		(ActWebSvc + 17)
#define ActNewBan		(ActWebSvc + 18)
#define ActRemBan		(ActWebSvc + 19)
#define ActShoBan		(ActWebSvc + 20)
#define ActHidBan		(ActWebSvc + 21)
#define ActRenBanSho		(ActWebSvc + 22)
#define ActRenBanFul		(ActWebSvc + 23)
#define ActChgBanImg		(ActWebSvc + 24)
#define ActChgBanWWW		(ActWebSvc + 25)
#define ActClkBan		(ActWebSvc + 26)

#define ActEdiLnk		(ActWebSvc + 27)
#define ActNewLnk		(ActWebSvc + 28)
#define ActRemLnk		(ActWebSvc + 29)
#define ActRenLnkSho		(ActWebSvc + 30)
#define ActRenLnkFul		(ActWebSvc + 31)
#define ActChgLnkWWW		(ActWebSvc + 32)

#define ActEdiPlg		(ActWebSvc + 33)
#define ActNewPlg		(ActWebSvc + 34)
#define ActRemPlg		(ActWebSvc + 35)
#define ActRenPlg		(ActWebSvc + 36)
#define ActChgPlgDes		(ActWebSvc + 37)
#define ActChgPlgLog		(ActWebSvc + 38)
#define ActChgPlgAppKey		(ActWebSvc + 39)
#define ActChgPlgURL		(ActWebSvc + 40)
#define ActChgPlgIP		(ActWebSvc + 41)

#define ActPrnCalSys		(ActWebSvc + 42)
#define ActChgCalSys1stDay	(ActWebSvc + 43)

/*****************************************************************************/
/******************************** Country tab ********************************/
/*****************************************************************************/
// Actions in menu
#define ActCtyReqSch		(ActChgCalSys1stDay +  1)
#define ActSeeCtyInf		(ActChgCalSys1stDay +  2)
#define ActSeeIns		(ActChgCalSys1stDay +  3)
#define ActSeeCalCty		(ActChgCalSys1stDay +  4)

// Secondary actions
#define ActCtySch		(ActChgCalSys1stDay +  5)
#define ActPrnCtyInf		(ActChgCalSys1stDay +  6)
#define ActChgCtyMapAtt		(ActChgCalSys1stDay +  7)

#define ActEdiIns		(ActChgCalSys1stDay +  8)
#define ActReqIns		(ActChgCalSys1stDay +  9)
#define ActNewIns		(ActChgCalSys1stDay + 10)
#define ActRemIns		(ActChgCalSys1stDay + 11)
#define ActRenInsSho		(ActChgCalSys1stDay + 12)
#define ActRenInsFul		(ActChgCalSys1stDay + 13)
#define ActChgInsWWW		(ActChgCalSys1stDay + 14)
#define ActChgInsSta		(ActChgCalSys1stDay + 15)

#define ActPrnCalCty		(ActChgCalSys1stDay + 16)
#define ActChgCalCty1stDay	(ActChgCalSys1stDay + 17)

/*****************************************************************************/
/****************************** Institution tab ******************************/
/*****************************************************************************/
// Actions in menu
#define ActInsReqSch		(ActChgCalCty1stDay +  1)
#define ActSeeInsInf		(ActChgCalCty1stDay +  2)
#define ActSeeCtr		(ActChgCalCty1stDay +  3)
#define ActSeeDpt		(ActChgCalCty1stDay +  4)
#define ActSeeCalIns		(ActChgCalCty1stDay +  5)

// Secondary actions
#define ActInsSch		(ActChgCalCty1stDay +  6)
#define ActPrnInsInf		(ActChgCalCty1stDay +  7)
#define ActChgInsCtyCfg		(ActChgCalCty1stDay +  8)
#define ActRenInsShoCfg		(ActChgCalCty1stDay +  9)
#define ActRenInsFulCfg		(ActChgCalCty1stDay + 10)
#define ActChgInsWWWCfg		(ActChgCalCty1stDay + 11)
#define ActReqInsLog		(ActChgCalCty1stDay + 12)
#define ActRecInsLog		(ActChgCalCty1stDay + 13)
#define ActRemInsLog		(ActChgCalCty1stDay + 14)

#define ActEdiCtr		(ActChgCalCty1stDay + 15)
#define ActReqCtr		(ActChgCalCty1stDay + 16)
#define ActNewCtr		(ActChgCalCty1stDay + 17)
#define ActRemCtr		(ActChgCalCty1stDay + 18)
#define ActChgCtrPlc		(ActChgCalCty1stDay + 19)
#define ActRenCtrSho		(ActChgCalCty1stDay + 20)
#define ActRenCtrFul		(ActChgCalCty1stDay + 21)
#define ActChgCtrWWW		(ActChgCalCty1stDay + 22)
#define ActChgCtrSta		(ActChgCalCty1stDay + 23)

#define ActSeePlc		(ActChgCalCty1stDay + 24)
#define ActEdiPlc		(ActChgCalCty1stDay + 25)
#define ActNewPlc		(ActChgCalCty1stDay + 26)
#define ActRemPlc		(ActChgCalCty1stDay + 27)
#define ActRenPlcSho		(ActChgCalCty1stDay + 28)
#define ActRenPlcFul		(ActChgCalCty1stDay + 29)

#define ActEdiDpt		(ActChgCalCty1stDay + 30)
#define ActNewDpt		(ActChgCalCty1stDay + 31)
#define ActRemDpt		(ActChgCalCty1stDay + 32)
#define ActChgDptIns		(ActChgCalCty1stDay + 33)
#define ActRenDptSho		(ActChgCalCty1stDay + 34)
#define ActRenDptFul		(ActChgCalCty1stDay + 35)
#define ActChgDptWWW		(ActChgCalCty1stDay + 36)

#define ActSeeHld		(ActChgCalCty1stDay + 37)
#define ActEdiHld		(ActChgCalCty1stDay + 38)
#define ActNewHld		(ActChgCalCty1stDay + 39)
#define ActRemHld		(ActChgCalCty1stDay + 40)
#define ActChgHldPlc		(ActChgCalCty1stDay + 41)
#define ActChgHldTyp		(ActChgCalCty1stDay + 42)
#define ActChgHldStrDat		(ActChgCalCty1stDay + 43)
#define ActChgHldEndDat		(ActChgCalCty1stDay + 44)
#define ActRenHld		(ActChgCalCty1stDay + 45)

#define ActPrnCalIns		(ActChgCalCty1stDay + 46)
#define ActChgCalIns1stDay	(ActChgCalCty1stDay + 47)

/*****************************************************************************/
/********************************* Centre tab ********************************/
/*****************************************************************************/
// Actions in menu
#define ActCtrReqSch		(ActChgCalIns1stDay +  1)
#define ActSeeCtrInf		(ActChgCalIns1stDay +  2)
#define ActSeeDeg		(ActChgCalIns1stDay +  3)
#define ActSeeCalCtr		(ActChgCalIns1stDay +  4)

// Secondary actions
#define ActCtrSch		(ActChgCalIns1stDay +  5)
#define ActPrnCtrInf		(ActChgCalIns1stDay +  6)
#define ActChgCtrInsCfg		(ActChgCalIns1stDay +  7)
#define ActRenCtrShoCfg		(ActChgCalIns1stDay +  8)
#define ActRenCtrFulCfg		(ActChgCalIns1stDay +  9)
#define ActChgCtrWWWCfg		(ActChgCalIns1stDay + 10)
#define ActReqCtrLog		(ActChgCalIns1stDay + 11)
#define ActRecCtrLog		(ActChgCalIns1stDay + 12)
#define ActRemCtrLog		(ActChgCalIns1stDay + 13)
#define ActReqCtrPho		(ActChgCalIns1stDay + 14)
#define ActRecCtrPho		(ActChgCalIns1stDay + 15)
#define ActChgCtrPhoAtt		(ActChgCalIns1stDay + 16)

#define ActSeeDegTyp		(ActChgCalIns1stDay + 17)
#define ActEdiDegTyp		(ActChgCalIns1stDay + 18)
#define ActNewDegTyp		(ActChgCalIns1stDay + 19)
#define ActRemDegTyp		(ActChgCalIns1stDay + 20)
#define ActRenDegTyp		(ActChgCalIns1stDay + 21)

#define ActEdiDeg		(ActChgCalIns1stDay + 22)
#define ActReqDeg		(ActChgCalIns1stDay + 23)
#define ActNewDeg		(ActChgCalIns1stDay + 24)
#define ActRemDeg		(ActChgCalIns1stDay + 25)
#define ActRenDegSho		(ActChgCalIns1stDay + 26)
#define ActRenDegFul		(ActChgCalIns1stDay + 27)
#define ActChgDegTyp		(ActChgCalIns1stDay + 28)
#define ActChgDegWWW		(ActChgCalIns1stDay + 29)
#define ActChgDegSta		(ActChgCalIns1stDay + 30)

#define ActPrnCalCtr		(ActChgCalIns1stDay + 31)
#define ActChgCalCtr1stDay	(ActChgCalIns1stDay + 32)

/*****************************************************************************/
/********************************* Degree tab ********************************/
/*****************************************************************************/
// Actions in menu
#define ActDegReqSch		(ActChgCalCtr1stDay +  1)
#define ActSeeDegInf		(ActChgCalCtr1stDay +  2)
#define ActSeeCrs		(ActChgCalCtr1stDay +  3)
#define ActSeeCalDeg		(ActChgCalCtr1stDay +  4)

// Secondary actions
#define ActDegSch		(ActChgCalCtr1stDay +  5)
#define ActPrnDegInf		(ActChgCalCtr1stDay +  6)
#define ActChgDegCtrCfg		(ActChgCalCtr1stDay +  7)
#define ActRenDegShoCfg		(ActChgCalCtr1stDay +  8)
#define ActRenDegFulCfg		(ActChgCalCtr1stDay +  9)
#define ActChgDegWWWCfg		(ActChgCalCtr1stDay + 10)
#define ActReqDegLog		(ActChgCalCtr1stDay + 11)
#define ActRecDegLog		(ActChgCalCtr1stDay + 12)
#define ActRemDegLog		(ActChgCalCtr1stDay + 13)

#define ActEdiCrs		(ActChgCalCtr1stDay + 14)
#define ActReqCrs		(ActChgCalCtr1stDay + 15)
#define ActNewCrs		(ActChgCalCtr1stDay + 16)
#define ActRemCrs		(ActChgCalCtr1stDay + 17)
#define ActChgInsCrsCod		(ActChgCalCtr1stDay + 18)
#define ActChgCrsYea		(ActChgCalCtr1stDay + 19)
#define ActRenCrsSho		(ActChgCalCtr1stDay + 20)
#define ActRenCrsFul		(ActChgCalCtr1stDay + 21)
#define ActChgCrsSta		(ActChgCalCtr1stDay + 22)

#define ActPrnCalDeg		(ActChgCalCtr1stDay + 23)
#define ActChgCalDeg1stDay	(ActChgCalCtr1stDay + 24)

/*****************************************************************************/
/******************************** Course tab *********************************/
/*****************************************************************************/
// Actions in menu
#define ActCrsReqSch		(ActChgCalDeg1stDay +   1)
#define ActSeeCrsInf		(ActChgCalDeg1stDay +   2)
#define ActSeeTchGui		(ActChgCalDeg1stDay +   3)
#define ActSeeSyl		(ActChgCalDeg1stDay +   4)
#define ActSeeBib		(ActChgCalDeg1stDay +   5)
#define ActSeeFAQ		(ActChgCalDeg1stDay +   6)
#define ActSeeCrsLnk		(ActChgCalDeg1stDay +   7)
#define ActSeeCrsTT		(ActChgCalDeg1stDay +   8)
#define ActSeeCalCrs		(ActChgCalDeg1stDay +   9)

// Secondary actions
#define ActCrsSch		(ActChgCalDeg1stDay +  10)
#define ActPrnCrsInf		(ActChgCalDeg1stDay +  11)
#define ActChgCrsDegCfg		(ActChgCalDeg1stDay +  12)
#define ActRenCrsShoCfg		(ActChgCalDeg1stDay +  13)
#define ActRenCrsFulCfg		(ActChgCalDeg1stDay +  14)
#define ActChgInsCrsCodCfg	(ActChgCalDeg1stDay +  15)
#define ActChgCrsYeaCfg		(ActChgCalDeg1stDay +  16)
#define ActEdiCrsInf		(ActChgCalDeg1stDay +  17)
#define ActEdiTchGui		(ActChgCalDeg1stDay +  18)
#define ActSeeSylLec		(ActChgCalDeg1stDay +  19)
#define ActSeeSylPra		(ActChgCalDeg1stDay +  20)
#define ActEdiSylLec		(ActChgCalDeg1stDay +  21)
#define ActEdiSylPra		(ActChgCalDeg1stDay +  22)
#define ActDelItmSylLec		(ActChgCalDeg1stDay +  23)
#define ActDelItmSylPra		(ActChgCalDeg1stDay +  24)
#define ActUp_IteSylLec		(ActChgCalDeg1stDay +  25)
#define ActUp_IteSylPra		(ActChgCalDeg1stDay +  26)
#define ActDwnIteSylLec		(ActChgCalDeg1stDay +  27)
#define ActDwnIteSylPra		(ActChgCalDeg1stDay +  28)
#define ActRgtIteSylLec		(ActChgCalDeg1stDay +  29)
#define ActRgtIteSylPra		(ActChgCalDeg1stDay +  30)
#define ActLftIteSylLec		(ActChgCalDeg1stDay +  31)
#define ActLftIteSylPra		(ActChgCalDeg1stDay +  32)
#define ActInsIteSylLec		(ActChgCalDeg1stDay +  33)
#define ActInsIteSylPra		(ActChgCalDeg1stDay +  34)
#define ActModIteSylLec		(ActChgCalDeg1stDay +  35)
#define ActModIteSylPra		(ActChgCalDeg1stDay +  36)

#define ActEdiBib		(ActChgCalDeg1stDay +  37)
#define ActEdiFAQ		(ActChgCalDeg1stDay +  38)
#define ActEdiCrsLnk		(ActChgCalDeg1stDay +  39)

#define ActChgFrcReaCrsInf	(ActChgCalDeg1stDay +  40)
#define ActChgFrcReaTchGui	(ActChgCalDeg1stDay +  41)
#define ActChgFrcReaSylLec	(ActChgCalDeg1stDay +  42)
#define ActChgFrcReaSylPra	(ActChgCalDeg1stDay +  43)
#define ActChgFrcReaBib		(ActChgCalDeg1stDay +  44)
#define ActChgFrcReaFAQ		(ActChgCalDeg1stDay +  45)
#define ActChgFrcReaCrsLnk	(ActChgCalDeg1stDay +  46)

#define ActChgHavReaCrsInf	(ActChgCalDeg1stDay +  47)
#define ActChgHavReaTchGui	(ActChgCalDeg1stDay +  48)
#define ActChgHavReaSylLec	(ActChgCalDeg1stDay +  49)
#define ActChgHavReaSylPra	(ActChgCalDeg1stDay +  50)
#define ActChgHavReaBib		(ActChgCalDeg1stDay +  51)
#define ActChgHavReaFAQ		(ActChgCalDeg1stDay +  52)
#define ActChgHavReaCrsLnk	(ActChgCalDeg1stDay +  53)

#define ActSelInfSrcCrsInf	(ActChgCalDeg1stDay +  54)
#define ActSelInfSrcTchGui	(ActChgCalDeg1stDay +  55)
#define ActSelInfSrcSylLec	(ActChgCalDeg1stDay +  56)
#define ActSelInfSrcSylPra	(ActChgCalDeg1stDay +  57)
#define ActSelInfSrcBib		(ActChgCalDeg1stDay +  58)
#define ActSelInfSrcFAQ		(ActChgCalDeg1stDay +  59)
#define ActSelInfSrcCrsLnk	(ActChgCalDeg1stDay +  60)
#define ActRcvURLCrsInf		(ActChgCalDeg1stDay +  61)
#define ActRcvURLTchGui		(ActChgCalDeg1stDay +  62)
#define ActRcvURLSylLec		(ActChgCalDeg1stDay +  63)
#define ActRcvURLSylPra		(ActChgCalDeg1stDay +  64)
#define ActRcvURLBib		(ActChgCalDeg1stDay +  65)
#define ActRcvURLFAQ		(ActChgCalDeg1stDay +  66)
#define ActRcvURLCrsLnk		(ActChgCalDeg1stDay +  67)
#define ActRcvPagCrsInf		(ActChgCalDeg1stDay +  68)
#define ActRcvPagTchGui		(ActChgCalDeg1stDay +  69)
#define ActRcvPagSylLec		(ActChgCalDeg1stDay +  70)
#define ActRcvPagSylPra		(ActChgCalDeg1stDay +  71)
#define ActRcvPagBib		(ActChgCalDeg1stDay +  72)
#define ActRcvPagFAQ		(ActChgCalDeg1stDay +  73)
#define ActRcvPagCrsLnk		(ActChgCalDeg1stDay +  74)
#define ActEditorCrsInf		(ActChgCalDeg1stDay +  75)
#define ActEditorTchGui		(ActChgCalDeg1stDay +  76)
#define ActEditorSylLec		(ActChgCalDeg1stDay +  77)
#define ActEditorSylPra		(ActChgCalDeg1stDay +  78)
#define ActEditorBib		(ActChgCalDeg1stDay +  79)
#define ActEditorFAQ		(ActChgCalDeg1stDay +  80)
#define ActEditorCrsLnk		(ActChgCalDeg1stDay +  81)
#define ActPlaTxtEdiCrsInf	(ActChgCalDeg1stDay +  82)
#define ActPlaTxtEdiTchGui	(ActChgCalDeg1stDay +  83)
#define ActPlaTxtEdiSylLec	(ActChgCalDeg1stDay +  84)
#define ActPlaTxtEdiSylPra	(ActChgCalDeg1stDay +  85)
#define ActPlaTxtEdiBib		(ActChgCalDeg1stDay +  86)
#define ActPlaTxtEdiFAQ		(ActChgCalDeg1stDay +  87)
#define ActPlaTxtEdiCrsLnk	(ActChgCalDeg1stDay +  88)
#define ActRchTxtEdiCrsInf	(ActChgCalDeg1stDay +  89)
#define ActRchTxtEdiTchGui	(ActChgCalDeg1stDay +  90)
#define ActRchTxtEdiSylLec	(ActChgCalDeg1stDay +  91)
#define ActRchTxtEdiSylPra	(ActChgCalDeg1stDay +  92)
#define ActRchTxtEdiBib		(ActChgCalDeg1stDay +  93)
#define ActRchTxtEdiFAQ		(ActChgCalDeg1stDay +  94)
#define ActRchTxtEdiCrsLnk	(ActChgCalDeg1stDay +  95)
#define ActRcvPlaTxtCrsInf	(ActChgCalDeg1stDay +  96)
#define ActRcvPlaTxtTchGui	(ActChgCalDeg1stDay +  97)
#define ActRcvPlaTxtSylLec	(ActChgCalDeg1stDay +  98)
#define ActRcvPlaTxtSylPra	(ActChgCalDeg1stDay +  99)
#define ActRcvPlaTxtBib		(ActChgCalDeg1stDay + 100)
#define ActRcvPlaTxtFAQ		(ActChgCalDeg1stDay + 101)
#define ActRcvPlaTxtCrsLnk	(ActChgCalDeg1stDay + 102)
#define ActRcvRchTxtCrsInf	(ActChgCalDeg1stDay + 103)
#define ActRcvRchTxtTchGui	(ActChgCalDeg1stDay + 104)
#define ActRcvRchTxtSylLec	(ActChgCalDeg1stDay + 105)
#define ActRcvRchTxtSylPra	(ActChgCalDeg1stDay + 106)
#define ActRcvRchTxtBib		(ActChgCalDeg1stDay + 107)
#define ActRcvRchTxtFAQ		(ActChgCalDeg1stDay + 108)
#define ActRcvRchTxtCrsLnk	(ActChgCalDeg1stDay + 109)

#define ActPrnCrsTT		(ActChgCalDeg1stDay + 110)
#define ActEdiCrsTT		(ActChgCalDeg1stDay + 111)
#define ActChgCrsTT		(ActChgCalDeg1stDay + 112)
#define ActChgCrsTT1stDay	(ActChgCalDeg1stDay + 113)

#define ActPrnCalCrs		(ActChgCalDeg1stDay + 114)
#define ActChgCalCrs1stDay	(ActChgCalDeg1stDay + 115)

/*****************************************************************************/
/***************************** Assessment tab ********************************/
/*****************************************************************************/
// Actions in menu
#define ActSeeAss		(ActChgCalCrs1stDay +  1)
#define ActSeeAsg		(ActChgCalCrs1stDay +  2)
#define ActReqTst		(ActChgCalCrs1stDay +  3)
#define ActSeeAllExaAnn		(ActChgCalCrs1stDay +  4)
#define ActSeeAllSvy		(ActChgCalCrs1stDay +  5)

// Secondary actions
#define ActEdiAss		(ActChgCalCrs1stDay +  6)
#define ActChgFrcReaAss		(ActChgCalCrs1stDay +  7)
#define ActChgHavReaAss		(ActChgCalCrs1stDay +  8)
#define ActSelInfSrcAss		(ActChgCalCrs1stDay +  9)
#define ActRcvURLAss		(ActChgCalCrs1stDay + 10)
#define ActRcvPagAss		(ActChgCalCrs1stDay + 11)
#define ActEditorAss		(ActChgCalCrs1stDay + 12)
#define ActPlaTxtEdiAss		(ActChgCalCrs1stDay + 13)
#define ActRchTxtEdiAss		(ActChgCalCrs1stDay + 14)
#define ActRcvPlaTxtAss		(ActChgCalCrs1stDay + 15)
#define ActRcvRchTxtAss		(ActChgCalCrs1stDay + 16)

#define ActFrmNewAsg		(ActChgCalCrs1stDay + 17)
#define ActEdiOneAsg		(ActChgCalCrs1stDay + 18)
#define ActNewAsg		(ActChgCalCrs1stDay + 19)
#define ActChgAsg		(ActChgCalCrs1stDay + 20)
#define ActReqRemAsg		(ActChgCalCrs1stDay + 21)
#define ActRemAsg		(ActChgCalCrs1stDay + 22)
#define ActHidAsg		(ActChgCalCrs1stDay + 23)
#define ActShoAsg		(ActChgCalCrs1stDay + 24)

#define ActSeeTst		(ActChgCalCrs1stDay + 25)
#define ActAssTst		(ActChgCalCrs1stDay + 26)
#define ActEdiTstQst		(ActChgCalCrs1stDay + 27)
#define ActEdiOneTstQst		(ActChgCalCrs1stDay + 28)
#define ActReqImpTstQst		(ActChgCalCrs1stDay + 29)
#define ActImpTstQst		(ActChgCalCrs1stDay + 30)
#define ActLstTstQst		(ActChgCalCrs1stDay + 31)
#define ActRcvTstQst		(ActChgCalCrs1stDay + 32)
#define ActReqRemTstQst		(ActChgCalCrs1stDay + 33)
#define ActRemTstQst		(ActChgCalCrs1stDay + 34)
#define ActShfTstQst		(ActChgCalCrs1stDay + 35)
#define ActCfgTst		(ActChgCalCrs1stDay + 36)
#define ActEnableTag		(ActChgCalCrs1stDay + 37)
#define ActDisableTag		(ActChgCalCrs1stDay + 38)
#define ActRenTag		(ActChgCalCrs1stDay + 39)
#define ActRcvCfgTst		(ActChgCalCrs1stDay + 40)

#define ActReqSeeMyTstRes	(ActChgCalCrs1stDay + 41)
#define ActSeeMyTstRes		(ActChgCalCrs1stDay + 42)
#define ActSeeOneTstResMe	(ActChgCalCrs1stDay + 43)
#define ActReqSeeUsrTstRes	(ActChgCalCrs1stDay + 44)
#define ActSeeUsrTstRes		(ActChgCalCrs1stDay + 45)
#define ActSeeOneTstResOth	(ActChgCalCrs1stDay + 46)

#define ActSeeOneExaAnn		(ActChgCalCrs1stDay + 47)
#define ActSeeDatExaAnn		(ActChgCalCrs1stDay + 48)
#define ActEdiExaAnn		(ActChgCalCrs1stDay + 49)
#define ActRcvExaAnn		(ActChgCalCrs1stDay + 50)
#define ActPrnExaAnn		(ActChgCalCrs1stDay + 51)
#define ActReqRemExaAnn		(ActChgCalCrs1stDay + 52)
#define ActRemExaAnn		(ActChgCalCrs1stDay + 53)
#define ActHidExaAnn		(ActChgCalCrs1stDay + 54)
#define ActShoExaAnn		(ActChgCalCrs1stDay + 55)

#define ActSeeOneSvy		(ActChgCalCrs1stDay + 56)
#define ActAnsSvy		(ActChgCalCrs1stDay + 57)
#define ActFrmNewSvy		(ActChgCalCrs1stDay + 58)
#define ActEdiOneSvy		(ActChgCalCrs1stDay + 59)
#define ActNewSvy		(ActChgCalCrs1stDay + 60)
#define ActChgSvy		(ActChgCalCrs1stDay + 61)
#define ActReqRemSvy		(ActChgCalCrs1stDay + 62)
#define ActRemSvy		(ActChgCalCrs1stDay + 63)
#define ActReqRstSvy		(ActChgCalCrs1stDay + 64)
#define ActRstSvy		(ActChgCalCrs1stDay + 65)
#define ActHidSvy		(ActChgCalCrs1stDay + 66)
#define ActShoSvy		(ActChgCalCrs1stDay + 67)
#define ActEdiOneSvyQst		(ActChgCalCrs1stDay + 68)
#define ActRcvSvyQst		(ActChgCalCrs1stDay + 69)
#define ActReqRemSvyQst		(ActChgCalCrs1stDay + 70)
#define ActRemSvyQst		(ActChgCalCrs1stDay + 71)

/*****************************************************************************/
/******************************** Files tab **********************************/
/*****************************************************************************/
// Actions in menu
#define ActSeeAdmDocIns		(ActRemSvyQst +   1)
#define ActAdmShaIns		(ActRemSvyQst +   2)
#define ActSeeAdmDocCtr		(ActRemSvyQst +   3)
#define ActAdmShaCtr		(ActRemSvyQst +   4)
#define ActSeeAdmDocDeg		(ActRemSvyQst +   5)
#define ActAdmShaDeg		(ActRemSvyQst +   6)
#define ActSeeAdmDocCrsGrp	(ActRemSvyQst +   7)
#define ActAdmTchCrsGrp		(ActRemSvyQst +   8)
#define ActAdmShaCrsGrp		(ActRemSvyQst +   9)
#define ActAdmAsgWrkUsr		(ActRemSvyQst +  10)
#define ActReqAsgWrkCrs		(ActRemSvyQst +  11)
#define ActSeeAdmMrk		(ActRemSvyQst +  12)
// Secondary actions
#define ActChgToSeeDocIns	(ActRemSvyQst +  13)
#define ActSeeDocIns		(ActRemSvyQst +  14)
#define ActExpSeeDocIns		(ActRemSvyQst +  15)
#define ActConSeeDocIns		(ActRemSvyQst +  16)
#define ActZIPSeeDocIns		(ActRemSvyQst +  17)
#define ActReqDatSeeDocIns	(ActRemSvyQst +  18)
#define ActDowSeeDocIns		(ActRemSvyQst +  19)
#define ActChgToAdmDocIns	(ActRemSvyQst +  20)
#define ActAdmDocIns		(ActRemSvyQst +  21)
#define ActReqRemFilDocIns	(ActRemSvyQst +  22)
#define ActRemFilDocIns		(ActRemSvyQst +  23)
#define ActRemFolDocIns		(ActRemSvyQst +  24)
#define ActCopDocIns		(ActRemSvyQst +  25)
#define ActPasDocIns		(ActRemSvyQst +  26)
#define ActRemTreDocIns		(ActRemSvyQst +  27)
#define ActFrmCreDocIns		(ActRemSvyQst +  28)
#define ActCreFolDocIns		(ActRemSvyQst +  29)
#define ActCreLnkDocIns		(ActRemSvyQst +  30)
#define ActRenFolDocIns		(ActRemSvyQst +  31)
#define ActRcvFilDocInsDZ	(ActRemSvyQst +  32)
#define ActRcvFilDocInsCla	(ActRemSvyQst +  33)
#define ActExpAdmDocIns		(ActRemSvyQst +  34)
#define ActConAdmDocIns		(ActRemSvyQst +  35)
#define ActZIPAdmDocIns		(ActRemSvyQst +  36)
#define ActShoDocIns		(ActRemSvyQst +  37)
#define ActHidDocIns		(ActRemSvyQst +  38)
#define ActReqDatAdmDocIns	(ActRemSvyQst +  39)
#define ActChgDatAdmDocIns	(ActRemSvyQst +  40)
#define ActDowAdmDocIns		(ActRemSvyQst +  41)

#define ActReqRemFilShaIns	(ActRemSvyQst +  42)
#define ActRemFilShaIns		(ActRemSvyQst +  43)
#define ActRemFolShaIns		(ActRemSvyQst +  44)
#define ActCopShaIns		(ActRemSvyQst +  45)
#define ActPasShaIns		(ActRemSvyQst +  46)
#define ActRemTreShaIns		(ActRemSvyQst +  47)
#define ActFrmCreShaIns		(ActRemSvyQst +  48)
#define ActCreFolShaIns		(ActRemSvyQst +  49)
#define ActCreLnkShaIns		(ActRemSvyQst +  50)
#define ActRenFolShaIns		(ActRemSvyQst +  51)
#define ActRcvFilShaInsDZ	(ActRemSvyQst +  52)
#define ActRcvFilShaInsCla	(ActRemSvyQst +  53)
#define ActExpShaIns		(ActRemSvyQst +  54)
#define ActConShaIns		(ActRemSvyQst +  55)
#define ActZIPShaIns		(ActRemSvyQst +  56)
#define ActReqDatShaIns		(ActRemSvyQst +  57)
#define ActChgDatShaIns		(ActRemSvyQst +  58)
#define ActDowShaIns		(ActRemSvyQst +  59)

#define ActChgToSeeDocCtr	(ActRemSvyQst +  60)
#define ActSeeDocCtr		(ActRemSvyQst +  61)
#define ActExpSeeDocCtr		(ActRemSvyQst +  62)
#define ActConSeeDocCtr		(ActRemSvyQst +  63)
#define ActZIPSeeDocCtr		(ActRemSvyQst +  64)
#define ActReqDatSeeDocCtr	(ActRemSvyQst +  65)
#define ActDowSeeDocCtr		(ActRemSvyQst +  66)
#define ActChgToAdmDocCtr	(ActRemSvyQst +  67)
#define ActAdmDocCtr		(ActRemSvyQst +  68)
#define ActReqRemFilDocCtr	(ActRemSvyQst +  69)
#define ActRemFilDocCtr		(ActRemSvyQst +  70)
#define ActRemFolDocCtr		(ActRemSvyQst +  71)
#define ActCopDocCtr		(ActRemSvyQst +  72)
#define ActPasDocCtr		(ActRemSvyQst +  73)
#define ActRemTreDocCtr		(ActRemSvyQst +  74)
#define ActFrmCreDocCtr		(ActRemSvyQst +  75)
#define ActCreFolDocCtr		(ActRemSvyQst +  76)
#define ActCreLnkDocCtr		(ActRemSvyQst +  77)
#define ActRenFolDocCtr		(ActRemSvyQst +  78)
#define ActRcvFilDocCtrDZ	(ActRemSvyQst +  79)
#define ActRcvFilDocCtrCla	(ActRemSvyQst +  80)
#define ActExpAdmDocCtr		(ActRemSvyQst +  81)
#define ActConAdmDocCtr		(ActRemSvyQst +  82)
#define ActZIPAdmDocCtr		(ActRemSvyQst +  83)
#define ActShoDocCtr		(ActRemSvyQst +  84)
#define ActHidDocCtr		(ActRemSvyQst +  85)
#define ActReqDatAdmDocCtr	(ActRemSvyQst +  86)
#define ActChgDatAdmDocCtr	(ActRemSvyQst +  87)
#define ActDowAdmDocCtr		(ActRemSvyQst +  88)

#define ActReqRemFilShaCtr	(ActRemSvyQst +  89)
#define ActRemFilShaCtr		(ActRemSvyQst +  90)
#define ActRemFolShaCtr		(ActRemSvyQst +  91)
#define ActCopShaCtr		(ActRemSvyQst +  92)
#define ActPasShaCtr		(ActRemSvyQst +  93)
#define ActRemTreShaCtr		(ActRemSvyQst +  94)
#define ActFrmCreShaCtr		(ActRemSvyQst +  95)
#define ActCreFolShaCtr		(ActRemSvyQst +  96)
#define ActCreLnkShaCtr		(ActRemSvyQst +  97)
#define ActRenFolShaCtr		(ActRemSvyQst +  98)
#define ActRcvFilShaCtrDZ	(ActRemSvyQst +  99)
#define ActRcvFilShaCtrCla	(ActRemSvyQst + 100)
#define ActExpShaCtr		(ActRemSvyQst + 101)
#define ActConShaCtr		(ActRemSvyQst + 102)
#define ActZIPShaCtr		(ActRemSvyQst + 103)
#define ActReqDatShaCtr		(ActRemSvyQst + 104)
#define ActChgDatShaCtr		(ActRemSvyQst + 105)
#define ActDowShaCtr		(ActRemSvyQst + 106)

#define ActChgToSeeDocDeg	(ActRemSvyQst + 107)
#define ActSeeDocDeg		(ActRemSvyQst + 108)
#define ActExpSeeDocDeg		(ActRemSvyQst + 109)
#define ActConSeeDocDeg		(ActRemSvyQst + 110)
#define ActZIPSeeDocDeg		(ActRemSvyQst + 111)
#define ActReqDatSeeDocDeg	(ActRemSvyQst + 112)
#define ActDowSeeDocDeg		(ActRemSvyQst + 113)
#define ActChgToAdmDocDeg	(ActRemSvyQst + 114)
#define ActAdmDocDeg		(ActRemSvyQst + 115)
#define ActReqRemFilDocDeg	(ActRemSvyQst + 116)
#define ActRemFilDocDeg		(ActRemSvyQst + 117)
#define ActRemFolDocDeg		(ActRemSvyQst + 118)
#define ActCopDocDeg		(ActRemSvyQst + 119)
#define ActPasDocDeg		(ActRemSvyQst + 120)
#define ActRemTreDocDeg		(ActRemSvyQst + 121)
#define ActFrmCreDocDeg		(ActRemSvyQst + 122)
#define ActCreFolDocDeg		(ActRemSvyQst + 123)
#define ActCreLnkDocDeg		(ActRemSvyQst + 124)
#define ActRenFolDocDeg		(ActRemSvyQst + 125)
#define ActRcvFilDocDegDZ	(ActRemSvyQst + 126)
#define ActRcvFilDocDegCla	(ActRemSvyQst + 127)
#define ActExpAdmDocDeg		(ActRemSvyQst + 128)
#define ActConAdmDocDeg		(ActRemSvyQst + 129)
#define ActZIPAdmDocDeg		(ActRemSvyQst + 130)
#define ActShoDocDeg		(ActRemSvyQst + 131)
#define ActHidDocDeg		(ActRemSvyQst + 132)
#define ActReqDatAdmDocDeg	(ActRemSvyQst + 133)
#define ActChgDatAdmDocDeg	(ActRemSvyQst + 134)
#define ActDowAdmDocDeg		(ActRemSvyQst + 135)

#define ActReqRemFilShaDeg	(ActRemSvyQst + 136)
#define ActRemFilShaDeg		(ActRemSvyQst + 137)
#define ActRemFolShaDeg		(ActRemSvyQst + 138)
#define ActCopShaDeg		(ActRemSvyQst + 139)
#define ActPasShaDeg		(ActRemSvyQst + 140)
#define ActRemTreShaDeg		(ActRemSvyQst + 141)
#define ActFrmCreShaDeg		(ActRemSvyQst + 142)
#define ActCreFolShaDeg		(ActRemSvyQst + 143)
#define ActCreLnkShaDeg		(ActRemSvyQst + 144)
#define ActRenFolShaDeg		(ActRemSvyQst + 145)
#define ActRcvFilShaDegDZ	(ActRemSvyQst + 146)
#define ActRcvFilShaDegCla	(ActRemSvyQst + 147)
#define ActExpShaDeg		(ActRemSvyQst + 148)
#define ActConShaDeg		(ActRemSvyQst + 149)
#define ActZIPShaDeg		(ActRemSvyQst + 150)
#define ActReqDatShaDeg		(ActRemSvyQst + 151)
#define ActChgDatShaDeg		(ActRemSvyQst + 152)
#define ActDowShaDeg		(ActRemSvyQst + 153)

#define ActChgToSeeDocCrs	(ActRemSvyQst + 154)
#define ActSeeDocCrs		(ActRemSvyQst + 155)
#define ActExpSeeDocCrs		(ActRemSvyQst + 156)
#define ActConSeeDocCrs		(ActRemSvyQst + 157)
#define ActZIPSeeDocCrs		(ActRemSvyQst + 158)
#define ActReqDatSeeDocCrs	(ActRemSvyQst + 159)
#define ActDowSeeDocCrs		(ActRemSvyQst + 160)

#define ActSeeDocGrp		(ActRemSvyQst + 161)
#define ActExpSeeDocGrp		(ActRemSvyQst + 162)
#define ActConSeeDocGrp		(ActRemSvyQst + 163)
#define ActZIPSeeDocGrp		(ActRemSvyQst + 164)
#define ActReqDatSeeDocGrp	(ActRemSvyQst + 165)
#define ActDowSeeDocGrp		(ActRemSvyQst + 166)

#define ActChgToAdmDocCrs	(ActRemSvyQst + 167)
#define ActAdmDocCrs		(ActRemSvyQst + 168)
#define ActReqRemFilDocCrs	(ActRemSvyQst + 169)
#define ActRemFilDocCrs		(ActRemSvyQst + 170)
#define ActRemFolDocCrs		(ActRemSvyQst + 171)
#define ActCopDocCrs		(ActRemSvyQst + 172)
#define ActPasDocCrs		(ActRemSvyQst + 173)
#define ActRemTreDocCrs		(ActRemSvyQst + 174)
#define ActFrmCreDocCrs		(ActRemSvyQst + 175)
#define ActCreFolDocCrs		(ActRemSvyQst + 176)
#define ActCreLnkDocCrs		(ActRemSvyQst + 177)
#define ActRenFolDocCrs		(ActRemSvyQst + 178)
#define ActRcvFilDocCrsDZ	(ActRemSvyQst + 179)
#define ActRcvFilDocCrsCla	(ActRemSvyQst + 180)
#define ActExpAdmDocCrs		(ActRemSvyQst + 181)
#define ActConAdmDocCrs		(ActRemSvyQst + 182)
#define ActZIPAdmDocCrs		(ActRemSvyQst + 183)
#define ActShoDocCrs		(ActRemSvyQst + 184)
#define ActHidDocCrs		(ActRemSvyQst + 185)
#define ActReqDatAdmDocCrs	(ActRemSvyQst + 186)
#define ActChgDatAdmDocCrs	(ActRemSvyQst + 187)
#define ActDowAdmDocCrs		(ActRemSvyQst + 188)

#define ActAdmDocGrp		(ActRemSvyQst + 189)
#define ActReqRemFilDocGrp	(ActRemSvyQst + 190)
#define ActRemFilDocGrp		(ActRemSvyQst + 191)
#define ActRemFolDocGrp		(ActRemSvyQst + 192)
#define ActCopDocGrp		(ActRemSvyQst + 193)
#define ActPasDocGrp		(ActRemSvyQst + 194)
#define ActRemTreDocGrp		(ActRemSvyQst + 195)
#define ActFrmCreDocGrp		(ActRemSvyQst + 196)
#define ActCreFolDocGrp		(ActRemSvyQst + 197)
#define ActCreLnkDocGrp		(ActRemSvyQst + 198)
#define ActRenFolDocGrp		(ActRemSvyQst + 199)
#define ActRcvFilDocGrpDZ	(ActRemSvyQst + 200)
#define ActRcvFilDocGrpCla	(ActRemSvyQst + 201)
#define ActExpAdmDocGrp		(ActRemSvyQst + 202)
#define ActConAdmDocGrp		(ActRemSvyQst + 203)
#define ActZIPAdmDocGrp		(ActRemSvyQst + 204)
#define ActShoDocGrp		(ActRemSvyQst + 205)
#define ActHidDocGrp		(ActRemSvyQst + 206)
#define ActReqDatAdmDocGrp	(ActRemSvyQst + 207)
#define ActChgDatAdmDocGrp	(ActRemSvyQst + 208)
#define ActDowAdmDocGrp		(ActRemSvyQst + 209)

#define ActChgToAdmTch		(ActRemSvyQst + 210)

#define ActAdmTchCrs		(ActRemSvyQst + 211)
#define ActReqRemFilTchCrs	(ActRemSvyQst + 212)
#define ActRemFilTchCrs		(ActRemSvyQst + 213)
#define ActRemFolTchCrs		(ActRemSvyQst + 214)
#define ActCopTchCrs		(ActRemSvyQst + 215)
#define ActPasTchCrs		(ActRemSvyQst + 216)
#define ActRemTreTchCrs		(ActRemSvyQst + 217)
#define ActFrmCreTchCrs		(ActRemSvyQst + 218)
#define ActCreFolTchCrs		(ActRemSvyQst + 219)
#define ActCreLnkTchCrs		(ActRemSvyQst + 220)
#define ActRenFolTchCrs		(ActRemSvyQst + 221)
#define ActRcvFilTchCrsDZ	(ActRemSvyQst + 222)
#define ActRcvFilTchCrsCla	(ActRemSvyQst + 223)
#define ActExpTchCrs		(ActRemSvyQst + 224)
#define ActConTchCrs		(ActRemSvyQst + 225)
#define ActZIPTchCrs		(ActRemSvyQst + 226)
#define ActReqDatTchCrs		(ActRemSvyQst + 227)
#define ActChgDatTchCrs		(ActRemSvyQst + 228)
#define ActDowTchCrs		(ActRemSvyQst + 229)

#define ActAdmTchGrp		(ActRemSvyQst + 230)
#define ActReqRemFilTchGrp	(ActRemSvyQst + 231)
#define ActRemFilTchGrp		(ActRemSvyQst + 232)
#define ActRemFolTchGrp		(ActRemSvyQst + 233)
#define ActCopTchGrp		(ActRemSvyQst + 234)
#define ActPasTchGrp		(ActRemSvyQst + 235)
#define ActRemTreTchGrp		(ActRemSvyQst + 236)
#define ActFrmCreTchGrp		(ActRemSvyQst + 237)
#define ActCreFolTchGrp		(ActRemSvyQst + 238)
#define ActCreLnkTchGrp		(ActRemSvyQst + 239)
#define ActRenFolTchGrp		(ActRemSvyQst + 240)
#define ActRcvFilTchGrpDZ	(ActRemSvyQst + 241)
#define ActRcvFilTchGrpCla	(ActRemSvyQst + 242)
#define ActExpTchGrp		(ActRemSvyQst + 243)
#define ActConTchGrp		(ActRemSvyQst + 244)
#define ActZIPTchGrp		(ActRemSvyQst + 245)
#define ActReqDatTchGrp		(ActRemSvyQst + 246)
#define ActChgDatTchGrp		(ActRemSvyQst + 247)
#define ActDowTchGrp		(ActRemSvyQst + 248)

#define ActChgToAdmSha		(ActRemSvyQst + 249)

#define ActAdmShaCrs		(ActRemSvyQst + 250)
#define ActReqRemFilShaCrs	(ActRemSvyQst + 251)
#define ActRemFilShaCrs		(ActRemSvyQst + 252)
#define ActRemFolShaCrs		(ActRemSvyQst + 253)
#define ActCopShaCrs		(ActRemSvyQst + 254)
#define ActPasShaCrs		(ActRemSvyQst + 255)
#define ActRemTreShaCrs		(ActRemSvyQst + 256)
#define ActFrmCreShaCrs		(ActRemSvyQst + 257)
#define ActCreFolShaCrs		(ActRemSvyQst + 258)
#define ActCreLnkShaCrs		(ActRemSvyQst + 259)
#define ActRenFolShaCrs		(ActRemSvyQst + 260)
#define ActRcvFilShaCrsDZ	(ActRemSvyQst + 261)
#define ActRcvFilShaCrsCla	(ActRemSvyQst + 262)
#define ActExpShaCrs		(ActRemSvyQst + 263)
#define ActConShaCrs		(ActRemSvyQst + 264)
#define ActZIPShaCrs		(ActRemSvyQst + 265)
#define ActReqDatShaCrs		(ActRemSvyQst + 266)
#define ActChgDatShaCrs		(ActRemSvyQst + 267)
#define ActDowShaCrs		(ActRemSvyQst + 268)

#define ActAdmShaGrp		(ActRemSvyQst + 269)
#define ActReqRemFilShaGrp	(ActRemSvyQst + 270)
#define ActRemFilShaGrp		(ActRemSvyQst + 271)
#define ActRemFolShaGrp		(ActRemSvyQst + 272)
#define ActCopShaGrp		(ActRemSvyQst + 273)
#define ActPasShaGrp		(ActRemSvyQst + 274)
#define ActRemTreShaGrp		(ActRemSvyQst + 275)
#define ActFrmCreShaGrp		(ActRemSvyQst + 276)
#define ActCreFolShaGrp		(ActRemSvyQst + 277)
#define ActCreLnkShaGrp		(ActRemSvyQst + 278)
#define ActRenFolShaGrp		(ActRemSvyQst + 279)
#define ActRcvFilShaGrpDZ	(ActRemSvyQst + 280)
#define ActRcvFilShaGrpCla	(ActRemSvyQst + 281)
#define ActExpShaGrp		(ActRemSvyQst + 282)
#define ActConShaGrp		(ActRemSvyQst + 283)
#define ActZIPShaGrp		(ActRemSvyQst + 284)
#define ActReqDatShaGrp		(ActRemSvyQst + 285)
#define ActChgDatShaGrp		(ActRemSvyQst + 286)
#define ActDowShaGrp		(ActRemSvyQst + 287)

#define ActAdmAsgWrkCrs		(ActRemSvyQst + 288)

#define ActReqRemFilAsgUsr	(ActRemSvyQst + 289)
#define ActRemFilAsgUsr		(ActRemSvyQst + 290)
#define ActRemFolAsgUsr		(ActRemSvyQst + 291)
#define ActCopAsgUsr		(ActRemSvyQst + 292)
#define ActPasAsgUsr		(ActRemSvyQst + 293)
#define ActRemTreAsgUsr		(ActRemSvyQst + 294)
#define ActFrmCreAsgUsr		(ActRemSvyQst + 295)
#define ActCreFolAsgUsr		(ActRemSvyQst + 296)
#define ActCreLnkAsgUsr		(ActRemSvyQst + 297)
#define ActRenFolAsgUsr		(ActRemSvyQst + 298)
#define ActRcvFilAsgUsrDZ	(ActRemSvyQst + 299)
#define ActRcvFilAsgUsrCla	(ActRemSvyQst + 300)
#define ActExpAsgUsr		(ActRemSvyQst + 301)
#define ActConAsgUsr		(ActRemSvyQst + 302)
#define ActZIPAsgUsr		(ActRemSvyQst + 303)
#define ActReqDatAsgUsr		(ActRemSvyQst + 304)
#define ActChgDatAsgUsr		(ActRemSvyQst + 305)
#define ActDowAsgUsr		(ActRemSvyQst + 306)

#define ActReqRemFilWrkUsr	(ActRemSvyQst + 307)
#define ActRemFilWrkUsr		(ActRemSvyQst + 308)
#define ActRemFolWrkUsr		(ActRemSvyQst + 309)
#define ActCopWrkUsr		(ActRemSvyQst + 310)
#define ActPasWrkUsr		(ActRemSvyQst + 311)
#define ActRemTreWrkUsr		(ActRemSvyQst + 312)
#define ActFrmCreWrkUsr		(ActRemSvyQst + 313)
#define ActCreFolWrkUsr		(ActRemSvyQst + 314)
#define ActCreLnkWrkUsr		(ActRemSvyQst + 315)
#define ActRenFolWrkUsr		(ActRemSvyQst + 316)
#define ActRcvFilWrkUsrDZ	(ActRemSvyQst + 317)
#define ActRcvFilWrkUsrCla	(ActRemSvyQst + 318)
#define ActExpWrkUsr		(ActRemSvyQst + 319)
#define ActConWrkUsr		(ActRemSvyQst + 320)
#define ActZIPWrkUsr		(ActRemSvyQst + 321)
#define ActReqDatWrkUsr		(ActRemSvyQst + 322)
#define ActChgDatWrkUsr		(ActRemSvyQst + 323)
#define ActDowWrkUsr		(ActRemSvyQst + 324)

#define ActReqRemFilAsgCrs	(ActRemSvyQst + 325)
#define ActRemFilAsgCrs		(ActRemSvyQst + 326)
#define ActRemFolAsgCrs		(ActRemSvyQst + 327)
#define ActCopAsgCrs		(ActRemSvyQst + 328)
#define ActPasAsgCrs		(ActRemSvyQst + 329)
#define ActRemTreAsgCrs		(ActRemSvyQst + 330)
#define ActFrmCreAsgCrs		(ActRemSvyQst + 331)
#define ActCreFolAsgCrs		(ActRemSvyQst + 332)
#define ActCreLnkAsgCrs		(ActRemSvyQst + 333)
#define ActRenFolAsgCrs		(ActRemSvyQst + 334)
#define ActRcvFilAsgCrsDZ	(ActRemSvyQst + 335)
#define ActRcvFilAsgCrsCla	(ActRemSvyQst + 336)
#define ActExpAsgCrs		(ActRemSvyQst + 337)
#define ActConAsgCrs		(ActRemSvyQst + 338)
#define ActZIPAsgCrs		(ActRemSvyQst + 339)
#define ActReqDatAsgCrs		(ActRemSvyQst + 340)
#define ActChgDatAsgCrs		(ActRemSvyQst + 341)
#define ActDowAsgCrs		(ActRemSvyQst + 342)

#define ActReqRemFilWrkCrs	(ActRemSvyQst + 343)
#define ActRemFilWrkCrs		(ActRemSvyQst + 344)
#define ActRemFolWrkCrs		(ActRemSvyQst + 345)
#define ActCopWrkCrs		(ActRemSvyQst + 346)
#define ActPasWrkCrs		(ActRemSvyQst + 347)
#define ActRemTreWrkCrs		(ActRemSvyQst + 348)
#define ActFrmCreWrkCrs		(ActRemSvyQst + 349)
#define ActCreFolWrkCrs		(ActRemSvyQst + 350)
#define ActCreLnkWrkCrs		(ActRemSvyQst + 351)
#define ActRenFolWrkCrs		(ActRemSvyQst + 352)
#define ActRcvFilWrkCrsDZ	(ActRemSvyQst + 353)
#define ActRcvFilWrkCrsCla	(ActRemSvyQst + 354)
#define ActExpWrkCrs		(ActRemSvyQst + 355)
#define ActConWrkCrs		(ActRemSvyQst + 356)
#define ActZIPWrkCrs		(ActRemSvyQst + 357)
#define ActReqDatWrkCrs		(ActRemSvyQst + 358)
#define ActChgDatWrkCrs		(ActRemSvyQst + 359)
#define ActDowWrkCrs		(ActRemSvyQst + 360)

#define ActChgToSeeMrk		(ActRemSvyQst + 361)

#define ActSeeMrkCrs		(ActRemSvyQst + 362)
#define ActExpSeeMrkCrs		(ActRemSvyQst + 363)
#define ActConSeeMrkCrs		(ActRemSvyQst + 364)
#define ActReqDatSeeMrkCrs	(ActRemSvyQst + 365)
#define ActSeeMyMrkCrs		(ActRemSvyQst + 366)

#define ActSeeMrkGrp		(ActRemSvyQst + 367)
#define ActExpSeeMrkGrp		(ActRemSvyQst + 368)
#define ActConSeeMrkGrp		(ActRemSvyQst + 369)
#define ActReqDatSeeMrkGrp	(ActRemSvyQst + 370)
#define ActSeeMyMrkGrp		(ActRemSvyQst + 371)

#define ActChgToAdmMrk		(ActRemSvyQst + 372)

#define ActAdmMrkCrs		(ActRemSvyQst + 373)
#define ActReqRemFilMrkCrs	(ActRemSvyQst + 374)
#define ActRemFilMrkCrs		(ActRemSvyQst + 375)
#define ActRemFolMrkCrs		(ActRemSvyQst + 376)
#define ActCopMrkCrs		(ActRemSvyQst + 377)
#define ActPasMrkCrs		(ActRemSvyQst + 378)
#define ActRemTreMrkCrs		(ActRemSvyQst + 379)
#define ActFrmCreMrkCrs		(ActRemSvyQst + 380)
#define ActCreFolMrkCrs		(ActRemSvyQst + 381)
#define ActRenFolMrkCrs		(ActRemSvyQst + 382)
#define ActRcvFilMrkCrsDZ	(ActRemSvyQst + 383)
#define ActRcvFilMrkCrsCla	(ActRemSvyQst + 384)
#define ActExpAdmMrkCrs		(ActRemSvyQst + 385)
#define ActConAdmMrkCrs		(ActRemSvyQst + 386)
#define ActZIPAdmMrkCrs		(ActRemSvyQst + 387)
#define ActShoMrkCrs		(ActRemSvyQst + 388)
#define ActHidMrkCrs		(ActRemSvyQst + 389)
#define ActReqDatAdmMrkCrs	(ActRemSvyQst + 390)
#define ActChgDatAdmMrkCrs	(ActRemSvyQst + 391)
#define ActDowAdmMrkCrs		(ActRemSvyQst + 392)
#define ActChgNumRowHeaCrs	(ActRemSvyQst + 393)
#define ActChgNumRowFooCrs	(ActRemSvyQst + 394)

#define ActAdmMrkGrp		(ActRemSvyQst + 395)
#define ActReqRemFilMrkGrp	(ActRemSvyQst + 396)
#define ActRemFilMrkGrp		(ActRemSvyQst + 397)
#define ActRemFolMrkGrp		(ActRemSvyQst + 398)
#define ActCopMrkGrp		(ActRemSvyQst + 399)
#define ActPasMrkGrp		(ActRemSvyQst + 400)
#define ActRemTreMrkGrp		(ActRemSvyQst + 401)
#define ActFrmCreMrkGrp		(ActRemSvyQst + 402)
#define ActCreFolMrkGrp		(ActRemSvyQst + 403)
#define ActRenFolMrkGrp		(ActRemSvyQst + 404)
#define ActRcvFilMrkGrpDZ	(ActRemSvyQst + 405)
#define ActRcvFilMrkGrpCla	(ActRemSvyQst + 406)
#define ActExpAdmMrkGrp		(ActRemSvyQst + 407)
#define ActConAdmMrkGrp		(ActRemSvyQst + 408)
#define ActZIPAdmMrkGrp		(ActRemSvyQst + 409)
#define ActShoMrkGrp		(ActRemSvyQst + 410)
#define ActHidMrkGrp		(ActRemSvyQst + 411)
#define ActReqDatAdmMrkGrp	(ActRemSvyQst + 412)
#define ActChgDatAdmMrkGrp	(ActRemSvyQst + 413)
#define ActDowAdmMrkGrp		(ActRemSvyQst + 414)
#define ActChgNumRowHeaGrp	(ActRemSvyQst + 415)
#define ActChgNumRowFooGrp	(ActRemSvyQst + 416)

/*****************************************************************************/
/******************************* Users tab ***********************************/
/*****************************************************************************/
// Actions in menu
#define ActReqSelGrp		(ActChgNumRowFooGrp +   1)
#define ActLstStd		(ActChgNumRowFooGrp +   2)
#define ActLstTch		(ActChgNumRowFooGrp +   3)
#define ActLstOth		(ActChgNumRowFooGrp +   4)
#define ActSeeAtt		(ActChgNumRowFooGrp +   5)
#define ActReqSignUp		(ActChgNumRowFooGrp +   6)
#define ActSeeSignUpReq		(ActChgNumRowFooGrp +   7)
#define ActLstCon		(ActChgNumRowFooGrp +   8)

// Secondary actions
#define ActChgGrp		(ActChgNumRowFooGrp +   9)
#define ActReqEdiGrp		(ActChgNumRowFooGrp +  10)
#define ActNewGrpTyp		(ActChgNumRowFooGrp +  11)
#define ActReqRemGrpTyp		(ActChgNumRowFooGrp +  12)
#define ActRemGrpTyp		(ActChgNumRowFooGrp +  13)
#define ActRenGrpTyp		(ActChgNumRowFooGrp +  14)
#define ActChgMdtGrpTyp		(ActChgNumRowFooGrp +  15)
#define ActChgMulGrpTyp		(ActChgNumRowFooGrp +  16)
#define ActChgTimGrpTyp		(ActChgNumRowFooGrp +  17)
#define ActNewGrp		(ActChgNumRowFooGrp +  18)
#define ActReqRemGrp		(ActChgNumRowFooGrp +  19)
#define ActRemGrp		(ActChgNumRowFooGrp +  20)
#define ActOpeGrp		(ActChgNumRowFooGrp +  21)
#define ActCloGrp		(ActChgNumRowFooGrp +  22)
#define ActEnaFilZonGrp		(ActChgNumRowFooGrp +  23)
#define ActDisFilZonGrp		(ActChgNumRowFooGrp +  24)
#define ActChgGrpTyp		(ActChgNumRowFooGrp +  25)
#define ActRenGrp		(ActChgNumRowFooGrp +  26)
#define ActChgMaxStdGrp		(ActChgNumRowFooGrp +  27)

#define ActLstGst		(ActChgNumRowFooGrp +  28)

#define ActPrnGstPho		(ActChgNumRowFooGrp +  29)
#define ActPrnStdPho		(ActChgNumRowFooGrp +  30)
#define ActPrnTchPho		(ActChgNumRowFooGrp +  31)
#define ActLstGstAll		(ActChgNumRowFooGrp +  32)
#define ActLstStdAll		(ActChgNumRowFooGrp +  33)
#define ActLstTchAll		(ActChgNumRowFooGrp +  34)

#define ActSeeRecOneStd		(ActChgNumRowFooGrp +  35)
#define ActSeeRecOneTch		(ActChgNumRowFooGrp +  36)
#define ActSeeRecSevGst		(ActChgNumRowFooGrp +  37)
#define ActSeeRecSevStd		(ActChgNumRowFooGrp +  38)
#define ActSeeRecSevTch		(ActChgNumRowFooGrp +  39)
#define ActPrnRecSevGst		(ActChgNumRowFooGrp +  40)
#define ActPrnRecSevStd		(ActChgNumRowFooGrp +  41)
#define ActPrnRecSevTch		(ActChgNumRowFooGrp +  42)
#define ActRcvRecOthUsr		(ActChgNumRowFooGrp +  43)
#define ActEdiRecFie		(ActChgNumRowFooGrp +  44)
#define ActNewFie		(ActChgNumRowFooGrp +  45)
#define ActReqRemFie		(ActChgNumRowFooGrp +  46)
#define ActRemFie		(ActChgNumRowFooGrp +  47)
#define ActRenFie		(ActChgNumRowFooGrp +  48)
#define ActChgRowFie		(ActChgNumRowFooGrp +  49)
#define ActChgVisFie		(ActChgNumRowFooGrp +  50)
#define ActRcvRecCrs		(ActChgNumRowFooGrp +  51)

#define ActFrmLogInUsrAgd	(ActChgNumRowFooGrp +  52)
#define ActLogInUsrAgd		(ActChgNumRowFooGrp +  53)
#define ActLogInUsrAgdLan	(ActChgNumRowFooGrp +  54)
#define ActSeeUsrAgd		(ActChgNumRowFooGrp +  55)

#define ActReqEnrSevStd		(ActChgNumRowFooGrp +  56)
#define ActReqEnrSevTch		(ActChgNumRowFooGrp +  57)

#define ActReqLstStdAtt		(ActChgNumRowFooGrp +  58)
#define ActSeeLstMyAtt		(ActChgNumRowFooGrp +  59)
#define ActPrnLstMyAtt		(ActChgNumRowFooGrp +  60)
#define ActSeeLstStdAtt		(ActChgNumRowFooGrp +  61)
#define ActPrnLstStdAtt		(ActChgNumRowFooGrp +  62)
#define ActFrmNewAtt		(ActChgNumRowFooGrp +  63)
#define ActEdiOneAtt		(ActChgNumRowFooGrp +  64)
#define ActNewAtt		(ActChgNumRowFooGrp +  65)
#define ActChgAtt		(ActChgNumRowFooGrp +  66)
#define ActReqRemAtt		(ActChgNumRowFooGrp +  67)
#define ActRemAtt		(ActChgNumRowFooGrp +  68)
#define ActHidAtt		(ActChgNumRowFooGrp +  69)
#define ActShoAtt		(ActChgNumRowFooGrp +  70)
#define ActSeeOneAtt		(ActChgNumRowFooGrp +  71)
#define ActRecAttStd		(ActChgNumRowFooGrp +  72)
#define ActRecAttMe		(ActChgNumRowFooGrp +  73)

#define ActSignUp		(ActChgNumRowFooGrp +  74)
#define ActUpdSignUpReq		(ActChgNumRowFooGrp +  75)
#define ActReqRejSignUp		(ActChgNumRowFooGrp +  76)
#define ActRejSignUp		(ActChgNumRowFooGrp +  77)

#define ActReqMdfOneOth		(ActChgNumRowFooGrp +  78)
#define ActReqMdfOneStd		(ActChgNumRowFooGrp +  79)
#define ActReqMdfOneTch		(ActChgNumRowFooGrp +  80)

#define ActReqMdfOth		(ActChgNumRowFooGrp +  81)
#define ActReqMdfStd		(ActChgNumRowFooGrp +  82)
#define ActReqMdfTch		(ActChgNumRowFooGrp +  83)

#define ActReqOthPho		(ActChgNumRowFooGrp +  84)
#define ActReqStdPho		(ActChgNumRowFooGrp +  85)
#define ActReqTchPho		(ActChgNumRowFooGrp +  86)
#define ActDetOthPho		(ActChgNumRowFooGrp +  87)
#define ActDetStdPho		(ActChgNumRowFooGrp +  88)
#define ActDetTchPho		(ActChgNumRowFooGrp +  89)
#define ActUpdOthPho		(ActChgNumRowFooGrp +  90)
#define ActUpdStdPho		(ActChgNumRowFooGrp +  91)
#define ActUpdTchPho		(ActChgNumRowFooGrp +  92)
#define ActReqRemOthPho		(ActChgNumRowFooGrp +  93)
#define ActReqRemStdPho		(ActChgNumRowFooGrp +  94)
#define ActReqRemTchPho		(ActChgNumRowFooGrp +  95)
#define ActRemOthPho		(ActChgNumRowFooGrp +  96)
#define ActRemStdPho		(ActChgNumRowFooGrp +  97)
#define ActRemTchPho		(ActChgNumRowFooGrp +  98)
#define ActCreOth		(ActChgNumRowFooGrp +  99)
#define ActCreStd		(ActChgNumRowFooGrp + 100)
#define ActCreTch		(ActChgNumRowFooGrp + 101)
#define ActUpdOth		(ActChgNumRowFooGrp + 102)
#define ActUpdStd		(ActChgNumRowFooGrp + 103)
#define ActUpdTch		(ActChgNumRowFooGrp + 104)

#define ActReqAccEnrStd		(ActChgNumRowFooGrp + 105)
#define ActReqAccEnrTch		(ActChgNumRowFooGrp + 106)
#define ActAccEnrStd		(ActChgNumRowFooGrp + 107)
#define ActAccEnrTch		(ActChgNumRowFooGrp + 108)
#define ActRemMe_Std		(ActChgNumRowFooGrp + 109)
#define ActRemMe_Tch		(ActChgNumRowFooGrp + 110)

#define ActNewAdmIns		(ActChgNumRowFooGrp + 111)
#define ActRemAdmIns		(ActChgNumRowFooGrp + 112)
#define ActNewAdmCtr		(ActChgNumRowFooGrp + 113)
#define ActRemAdmCtr		(ActChgNumRowFooGrp + 114)
#define ActNewAdmDeg		(ActChgNumRowFooGrp + 115)
#define ActRemAdmDeg		(ActChgNumRowFooGrp + 116)

#define ActRcvFrmEnrSevStd	(ActChgNumRowFooGrp + 117)
#define ActRcvFrmEnrSevTch	(ActChgNumRowFooGrp + 118)

#define ActReqCnfID_Oth		(ActChgNumRowFooGrp + 119)
#define ActReqCnfID_Std		(ActChgNumRowFooGrp + 120)
#define ActReqCnfID_Tch		(ActChgNumRowFooGrp + 121)
#define ActCnfID_Oth		(ActChgNumRowFooGrp + 122)
#define ActCnfID_Std		(ActChgNumRowFooGrp + 123)
#define ActCnfID_Tch		(ActChgNumRowFooGrp + 124)

#define ActFrmIDsOth		(ActChgNumRowFooGrp + 125)
#define ActFrmIDsStd		(ActChgNumRowFooGrp + 126)
#define ActFrmIDsTch		(ActChgNumRowFooGrp + 127)
#define ActRemID_Oth		(ActChgNumRowFooGrp + 128)
#define ActRemID_Std		(ActChgNumRowFooGrp + 129)
#define ActRemID_Tch		(ActChgNumRowFooGrp + 130)
#define ActNewID_Oth		(ActChgNumRowFooGrp + 131)
#define ActNewID_Std		(ActChgNumRowFooGrp + 132)
#define ActNewID_Tch		(ActChgNumRowFooGrp + 133)
#define ActFrmPwdOth		(ActChgNumRowFooGrp + 134)
#define ActFrmPwdStd		(ActChgNumRowFooGrp + 135)
#define ActFrmPwdTch		(ActChgNumRowFooGrp + 136)
#define ActChgPwdOth		(ActChgNumRowFooGrp + 137)
#define ActChgPwdStd		(ActChgNumRowFooGrp + 138)
#define ActChgPwdTch		(ActChgNumRowFooGrp + 139)
#define ActFrmMaiOth		(ActChgNumRowFooGrp + 140)
#define ActFrmMaiStd		(ActChgNumRowFooGrp + 141)
#define ActFrmMaiTch		(ActChgNumRowFooGrp + 142)
#define ActRemMaiOth		(ActChgNumRowFooGrp + 143)
#define ActRemMaiStd		(ActChgNumRowFooGrp + 144)
#define ActRemMaiTch		(ActChgNumRowFooGrp + 145)
#define ActNewMaiOth		(ActChgNumRowFooGrp + 146)
#define ActNewMaiStd		(ActChgNumRowFooGrp + 147)
#define ActNewMaiTch		(ActChgNumRowFooGrp + 148)

#define ActRemStdCrs		(ActChgNumRowFooGrp + 149)
#define ActRemTchCrs		(ActChgNumRowFooGrp + 150)
#define ActRemUsrGbl		(ActChgNumRowFooGrp + 151)

#define ActReqRemAllStdCrs	(ActChgNumRowFooGrp + 152)
#define ActRemAllStdCrs		(ActChgNumRowFooGrp + 153)

#define ActReqRemOldUsr		(ActChgNumRowFooGrp + 154)
#define ActRemOldUsr		(ActChgNumRowFooGrp + 155)

#define ActLstDupUsr		(ActChgNumRowFooGrp + 156)
#define ActLstSimUsr		(ActChgNumRowFooGrp + 157)
#define ActRemDupUsr		(ActChgNumRowFooGrp + 158)

#define ActLstClk		(ActChgNumRowFooGrp + 159)

/*****************************************************************************/
/******************************** Social tab *********************************/
/*****************************************************************************/
// Actions in menu
#define ActSeeSocTmlGbl		(ActLstClk +   1)
#define ActSeeSocPrf		(ActLstClk +   2)
#define ActSeeFor		(ActLstClk +   3)
#define ActSeeChtRms		(ActLstClk +   4)
// Secondary actions
#define ActRcvSocPstGbl		(ActLstClk +   5)
#define ActRcvSocComGbl		(ActLstClk +   6)
#define ActShaSocNotGbl		(ActLstClk +   7)
#define ActUnsSocNotGbl		(ActLstClk +   8)
#define ActFavSocNotGbl		(ActLstClk +   9)
#define ActUnfSocNotGbl		(ActLstClk +  10)
#define ActFavSocComGbl		(ActLstClk +  11)
#define ActUnfSocComGbl		(ActLstClk +  12)
#define ActReqRemSocPubGbl	(ActLstClk +  13)
#define ActRemSocPubGbl		(ActLstClk +  14)
#define ActReqRemSocComGbl	(ActLstClk +  15)
#define ActRemSocComGbl		(ActLstClk +  16)

#define ActReqOthPubPrf		(ActLstClk +  17)

#define ActRcvSocPstUsr		(ActLstClk +  18)
#define ActRcvSocComUsr		(ActLstClk +  19)
#define ActShaSocNotUsr		(ActLstClk +  20)
#define ActUnsSocNotUsr		(ActLstClk +  21)
#define ActFavSocNotUsr		(ActLstClk +  22)
#define ActUnfSocNotUsr		(ActLstClk +  23)
#define ActFavSocComUsr		(ActLstClk +  24)
#define ActUnfSocComUsr		(ActLstClk +  25)
#define ActReqRemSocPubUsr	(ActLstClk +  26)
#define ActRemSocPubUsr		(ActLstClk +  27)
#define ActReqRemSocComUsr	(ActLstClk +  28)
#define ActRemSocComUsr		(ActLstClk +  29)

#define ActSeeOthPubPrf		(ActLstClk +  30)
#define ActCal1stClkTim		(ActLstClk +  31)
#define ActCalNumClk		(ActLstClk +  32)
#define ActCalNumFilVie		(ActLstClk +  33)
#define ActCalNumForPst		(ActLstClk +  34)
#define ActCalNumMsgSnt		(ActLstClk +  35)

#define ActFolUsr		(ActLstClk +  36)
#define ActUnfUsr		(ActLstClk +  37)
#define ActSeeFlg		(ActLstClk +  38)
#define ActSeeFlr		(ActLstClk +  39)

#define ActSeeForCrsUsr		(ActLstClk +  40)
#define ActSeeForCrsTch		(ActLstClk +  41)
#define ActSeeForDegUsr		(ActLstClk +  42)
#define ActSeeForDegTch		(ActLstClk +  43)
#define ActSeeForCtrUsr		(ActLstClk +  44)
#define ActSeeForCtrTch		(ActLstClk +  45)
#define ActSeeForInsUsr		(ActLstClk +  46)
#define ActSeeForInsTch		(ActLstClk +  47)
#define ActSeeForGenUsr		(ActLstClk +  48)
#define ActSeeForGenTch		(ActLstClk +  49)
#define ActSeeForSWAUsr		(ActLstClk +  50)
#define ActSeeForSWATch		(ActLstClk +  51)
#define ActSeePstForCrsUsr	(ActLstClk +  52)
#define ActSeePstForCrsTch	(ActLstClk +  53)
#define ActSeePstForDegUsr	(ActLstClk +  54)
#define ActSeePstForDegTch	(ActLstClk +  55)
#define ActSeePstForCtrUsr	(ActLstClk +  56)
#define ActSeePstForCtrTch	(ActLstClk +  57)
#define ActSeePstForInsUsr	(ActLstClk +  58)
#define ActSeePstForInsTch	(ActLstClk +  59)
#define ActSeePstForGenUsr	(ActLstClk +  60)
#define ActSeePstForGenTch	(ActLstClk +  61)
#define ActSeePstForSWAUsr	(ActLstClk +  62)
#define ActSeePstForSWATch	(ActLstClk +  63)
#define ActRcvThrForCrsUsr	(ActLstClk +  64)
#define ActRcvThrForCrsTch	(ActLstClk +  65)
#define ActRcvThrForDegUsr	(ActLstClk +  66)
#define ActRcvThrForDegTch	(ActLstClk +  67)
#define ActRcvThrForCtrUsr	(ActLstClk +  68)
#define ActRcvThrForCtrTch	(ActLstClk +  69)
#define ActRcvThrForInsUsr	(ActLstClk +  70)
#define ActRcvThrForInsTch	(ActLstClk +  71)
#define ActRcvThrForGenUsr	(ActLstClk +  72)
#define ActRcvThrForGenTch	(ActLstClk +  73)
#define ActRcvThrForSWAUsr	(ActLstClk +  74)
#define ActRcvThrForSWATch	(ActLstClk +  75)
#define ActRcvRepForCrsUsr	(ActLstClk +  76)
#define ActRcvRepForCrsTch	(ActLstClk +  77)
#define ActRcvRepForDegUsr	(ActLstClk +  78)
#define ActRcvRepForDegTch	(ActLstClk +  79)
#define ActRcvRepForCtrUsr	(ActLstClk +  80)
#define ActRcvRepForCtrTch	(ActLstClk +  81)
#define ActRcvRepForInsUsr	(ActLstClk +  82)
#define ActRcvRepForInsTch	(ActLstClk +  83)
#define ActRcvRepForGenUsr	(ActLstClk +  84)
#define ActRcvRepForGenTch	(ActLstClk +  85)
#define ActRcvRepForSWAUsr	(ActLstClk +  86)
#define ActRcvRepForSWATch	(ActLstClk +  87)
#define ActReqDelThrCrsUsr	(ActLstClk +  88)
#define ActReqDelThrCrsTch	(ActLstClk +  89)
#define ActReqDelThrDegUsr	(ActLstClk +  90)
#define ActReqDelThrDegTch	(ActLstClk +  91)
#define ActReqDelThrCtrUsr	(ActLstClk +  92)
#define ActReqDelThrCtrTch	(ActLstClk +  93)
#define ActReqDelThrInsUsr	(ActLstClk +  94)
#define ActReqDelThrInsTch	(ActLstClk +  95)
#define ActReqDelThrGenUsr	(ActLstClk +  96)
#define ActReqDelThrGenTch	(ActLstClk +  97)
#define ActReqDelThrSWAUsr	(ActLstClk +  98)
#define ActReqDelThrSWATch	(ActLstClk +  99)
#define ActDelThrForCrsUsr	(ActLstClk + 100)
#define ActDelThrForCrsTch	(ActLstClk + 101)
#define ActDelThrForDegUsr	(ActLstClk + 102)
#define ActDelThrForDegTch	(ActLstClk + 103)
#define ActDelThrForCtrUsr	(ActLstClk + 104)
#define ActDelThrForCtrTch	(ActLstClk + 105)
#define ActDelThrForInsUsr	(ActLstClk + 106)
#define ActDelThrForInsTch	(ActLstClk + 107)
#define ActDelThrForGenUsr	(ActLstClk + 108)
#define ActDelThrForGenTch	(ActLstClk + 109)
#define ActDelThrForSWAUsr	(ActLstClk + 110)
#define ActDelThrForSWATch	(ActLstClk + 111)
#define ActCutThrForCrsUsr	(ActLstClk + 112)
#define ActCutThrForCrsTch	(ActLstClk + 113)
#define ActCutThrForDegUsr	(ActLstClk + 114)
#define ActCutThrForDegTch	(ActLstClk + 115)
#define ActCutThrForCtrUsr	(ActLstClk + 116)
#define ActCutThrForCtrTch	(ActLstClk + 117)
#define ActCutThrForInsUsr	(ActLstClk + 118)
#define ActCutThrForInsTch	(ActLstClk + 119)
#define ActCutThrForGenUsr	(ActLstClk + 120)
#define ActCutThrForGenTch	(ActLstClk + 121)
#define ActCutThrForSWAUsr	(ActLstClk + 122)
#define ActCutThrForSWATch	(ActLstClk + 123)
#define ActPasThrForCrsUsr	(ActLstClk + 124)
#define ActPasThrForCrsTch	(ActLstClk + 125)
#define ActPasThrForDegUsr	(ActLstClk + 126)
#define ActPasThrForDegTch	(ActLstClk + 127)
#define ActPasThrForCtrUsr	(ActLstClk + 128)
#define ActPasThrForCtrTch	(ActLstClk + 129)
#define ActPasThrForInsUsr	(ActLstClk + 130)
#define ActPasThrForInsTch	(ActLstClk + 131)
#define ActPasThrForGenUsr	(ActLstClk + 132)
#define ActPasThrForGenTch	(ActLstClk + 133)
#define ActPasThrForSWAUsr	(ActLstClk + 134)
#define ActPasThrForSWATch	(ActLstClk + 135)
#define ActDelPstForCrsUsr	(ActLstClk + 136)
#define ActDelPstForCrsTch	(ActLstClk + 137)
#define ActDelPstForDegUsr	(ActLstClk + 138)
#define ActDelPstForDegTch	(ActLstClk + 139)
#define ActDelPstForCtrUsr	(ActLstClk + 140)
#define ActDelPstForCtrTch	(ActLstClk + 141)
#define ActDelPstForInsUsr	(ActLstClk + 142)
#define ActDelPstForInsTch	(ActLstClk + 143)
#define ActDelPstForGenUsr	(ActLstClk + 144)
#define ActDelPstForGenTch	(ActLstClk + 145)
#define ActDelPstForSWAUsr	(ActLstClk + 146)
#define ActDelPstForSWATch	(ActLstClk + 147)
#define ActEnbPstForCrsUsr	(ActLstClk + 148)
#define ActEnbPstForCrsTch	(ActLstClk + 149)
#define ActEnbPstForDegUsr	(ActLstClk + 150)
#define ActEnbPstForDegTch	(ActLstClk + 151)
#define ActEnbPstForCtrUsr	(ActLstClk + 152)
#define ActEnbPstForCtrTch	(ActLstClk + 153)
#define ActEnbPstForInsUsr	(ActLstClk + 154)
#define ActEnbPstForInsTch	(ActLstClk + 155)
#define ActEnbPstForGenUsr	(ActLstClk + 156)
#define ActEnbPstForGenTch	(ActLstClk + 157)
#define ActEnbPstForSWAUsr	(ActLstClk + 158)
#define ActEnbPstForSWATch	(ActLstClk + 159)
#define ActDisPstForCrsUsr	(ActLstClk + 160)
#define ActDisPstForCrsTch	(ActLstClk + 161)
#define ActDisPstForDegUsr	(ActLstClk + 162)
#define ActDisPstForDegTch	(ActLstClk + 163)
#define ActDisPstForCtrUsr	(ActLstClk + 164)
#define ActDisPstForCtrTch	(ActLstClk + 165)
#define ActDisPstForInsUsr	(ActLstClk + 166)
#define ActDisPstForInsTch	(ActLstClk + 167)
#define ActDisPstForGenUsr	(ActLstClk + 168)
#define ActDisPstForGenTch	(ActLstClk + 169)
#define ActDisPstForSWAUsr	(ActLstClk + 170)
#define ActDisPstForSWATch	(ActLstClk + 171)

#define ActCht			(ActLstClk + 172)

/*****************************************************************************/
/******************************* Messages tab ********************************/
/*****************************************************************************/
// Actions in menu
#define ActSeeNtf		(ActCht +   1)
#define ActSeeAnn		(ActCht +   2)
#define ActSeeAllNot		(ActCht +   3)
#define ActReqMsgUsr		(ActCht +   4)
#define ActSeeRcvMsg		(ActCht +   5)
#define ActSeeSntMsg		(ActCht +   6)
#define ActMaiStd		(ActCht +   7)
// Secondary actions
#define ActWriAnn		(ActCht +   8)
#define ActRcvAnn		(ActCht +   9)
#define ActHidAnn		(ActCht +  10)
#define ActRevAnn		(ActCht +  11)
#define ActRemAnn		(ActCht +  12)
#define ActSeeOneNot		(ActCht +  13)
#define ActWriNot		(ActCht +  14)
#define ActRcvNot		(ActCht +  15)
#define ActHidNot		(ActCht +  16)
#define ActRevNot		(ActCht +  17)
#define ActReqRemNot		(ActCht +  18)
#define ActRemNot		(ActCht +  19)

#define ActSeeNewNtf		(ActCht +  20)
#define ActMrkNtfSee		(ActCht +  21)
#define ActSeeMai		(ActCht +  22)
#define ActEdiMai		(ActCht +  23)
#define ActNewMai		(ActCht +  24)
#define ActRemMai		(ActCht +  25)
#define ActRenMaiSho		(ActCht +  26)
#define ActRenMaiFul		(ActCht +  27)

#define ActRcvMsgUsr		(ActCht +  28)
#define ActReqDelAllSntMsg	(ActCht +  29)
#define ActReqDelAllRcvMsg	(ActCht +  30)
#define ActDelAllSntMsg		(ActCht +  31)
#define ActDelAllRcvMsg		(ActCht +  32)
#define ActDelSntMsg		(ActCht +  33)
#define ActDelRcvMsg		(ActCht +  34)
#define ActExpSntMsg		(ActCht +  35)
#define ActExpRcvMsg		(ActCht +  36)
#define ActConSntMsg		(ActCht +  37)
#define ActConRcvMsg		(ActCht +  38)
#define ActLstBanUsr		(ActCht +  39)
#define ActBanUsrMsg		(ActCht +  40)
#define ActUnbUsrMsg		(ActCht +  41)
#define ActUnbUsrLst		(ActCht +  42)

/*****************************************************************************/
/****************************** Statistics tab *******************************/
/*****************************************************************************/
// Actions in menu
#define ActReqUseGbl		(ActUnbUsrLst +  1)
#define ActSeePhoDeg		(ActUnbUsrLst +  2)
#define ActReqStaCrs		(ActUnbUsrLst +  3)
#define ActReqAccGbl		(ActUnbUsrLst +  4)
#define ActReqMyUsgRep		(ActUnbUsrLst +  5)
#define ActMFUAct		(ActUnbUsrLst +  6)

// Secondary actions
#define ActSeeUseGbl		(ActUnbUsrLst +  7)
#define ActPrnPhoDeg		(ActUnbUsrLst +  8)
#define ActCalPhoDeg		(ActUnbUsrLst +  9)
#define ActSeeAccGbl		(ActUnbUsrLst + 10)
#define ActReqAccCrs		(ActUnbUsrLst + 11)
#define ActSeeAccCrs		(ActUnbUsrLst + 12)
#define ActSeeAllStaCrs		(ActUnbUsrLst + 13)

#define ActSeeMyUsgRep		(ActUnbUsrLst + 14)

/*****************************************************************************/
/******************************** Profile tab ********************************/
/*****************************************************************************/
// Actions in menu
#define ActFrmLogIn		(ActSeeMyUsgRep +  1)
#define ActFrmRolSes		(ActSeeMyUsgRep +  2)
#define ActMyCrs		(ActSeeMyUsgRep +  3)
#define ActSeeMyTT		(ActSeeMyUsgRep +  4)
#define ActSeeMyAgd		(ActSeeMyUsgRep +  5)
#define ActFrmMyAcc		(ActSeeMyUsgRep +  6)
#define ActReqEdiRecCom		(ActSeeMyUsgRep +  7)
#define ActEdiPrf		(ActSeeMyUsgRep +  8)
#define ActAdmBrf		(ActSeeMyUsgRep +  9)
// Secondary actions
#define ActReqSndNewPwd		(ActSeeMyUsgRep + 10)
#define ActSndNewPwd		(ActSeeMyUsgRep + 11)
#define ActLogOut		(ActSeeMyUsgRep + 12)
#define ActLogIn		(ActSeeMyUsgRep + 13)
#define ActLogInNew		(ActSeeMyUsgRep + 14)
#define ActLogInLan		(ActSeeMyUsgRep + 15)
#define ActAnnSee		(ActSeeMyUsgRep + 16)
#define ActChgMyRol		(ActSeeMyUsgRep + 17)
#define ActFrmNewEvtMyAgd	(ActSeeMyUsgRep + 18)
#define ActEdiOneEvtMyAgd	(ActSeeMyUsgRep + 19)
#define ActNewEvtMyAgd		(ActSeeMyUsgRep + 20)
#define ActChgEvtMyAgd		(ActSeeMyUsgRep + 21)
#define ActReqRemEvtMyAgd	(ActSeeMyUsgRep + 22)
#define ActRemEvtMyAgd		(ActSeeMyUsgRep + 23)
#define ActHidEvtMyAgd		(ActSeeMyUsgRep + 24)
#define ActShoEvtMyAgd		(ActSeeMyUsgRep + 25)
#define ActPrvEvtMyAgd		(ActSeeMyUsgRep + 26)
#define ActPubEvtMyAgd		(ActSeeMyUsgRep + 27)
#define ActPrnAgdQR		(ActSeeMyUsgRep + 28)

#define ActChkUsrAcc		(ActSeeMyUsgRep + 29)
#define ActCreUsrAcc		(ActSeeMyUsgRep + 30)
#define ActRemID_Me		(ActSeeMyUsgRep + 31)
#define ActNewIDMe		(ActSeeMyUsgRep + 32)
#define ActRemOldNic		(ActSeeMyUsgRep + 33)
#define ActChgNic		(ActSeeMyUsgRep + 34)
#define ActRemMaiMe		(ActSeeMyUsgRep + 35)
#define ActNewMaiMe		(ActSeeMyUsgRep + 36)
#define ActCnfMai		(ActSeeMyUsgRep + 37)
#define ActFrmChgMyPwd		(ActSeeMyUsgRep + 38)
#define ActChgPwd		(ActSeeMyUsgRep + 39)
#define ActReqRemMyAcc		(ActSeeMyUsgRep + 40)
#define ActRemMyAcc		(ActSeeMyUsgRep + 41)

#define ActChgMyData		(ActSeeMyUsgRep + 42)

#define ActReqMyPho		(ActSeeMyUsgRep + 43)
#define ActDetMyPho		(ActSeeMyUsgRep + 44)
#define ActUpdMyPho		(ActSeeMyUsgRep + 45)
#define ActReqRemMyPho		(ActSeeMyUsgRep + 46)
#define ActRemMyPho		(ActSeeMyUsgRep + 47)

#define ActEdiPri		(ActSeeMyUsgRep + 48)
#define ActChgPriPho		(ActSeeMyUsgRep + 49)
#define ActChgPriPrf		(ActSeeMyUsgRep + 50)

#define ActReqEdiMyIns		(ActSeeMyUsgRep + 51)
#define ActChgCtyMyIns		(ActSeeMyUsgRep + 52)
#define ActChgMyIns		(ActSeeMyUsgRep + 53)
#define ActChgMyCtr		(ActSeeMyUsgRep + 54)
#define ActChgMyDpt		(ActSeeMyUsgRep + 55)
#define ActChgMyOff		(ActSeeMyUsgRep + 56)
#define ActChgMyOffPho		(ActSeeMyUsgRep + 57)

#define ActReqEdiMyNet		(ActSeeMyUsgRep + 58)
#define ActChgMyNet		(ActSeeMyUsgRep + 59)

#define ActChgThe		(ActSeeMyUsgRep + 60)
#define ActReqChgLan		(ActSeeMyUsgRep + 61)
#define ActChgLan		(ActSeeMyUsgRep + 62)
#define ActChg1stDay		(ActSeeMyUsgRep + 63)
#define ActChgCol		(ActSeeMyUsgRep + 64)
#define ActHidLftCol		(ActSeeMyUsgRep + 65)
#define ActHidRgtCol		(ActSeeMyUsgRep + 66)
#define ActShoLftCol		(ActSeeMyUsgRep + 67)
#define ActShoRgtCol		(ActSeeMyUsgRep + 68)
#define ActChgIco		(ActSeeMyUsgRep + 69)
#define ActChgMnu		(ActSeeMyUsgRep + 70)
#define ActChgNtfPrf		(ActSeeMyUsgRep + 71)

#define ActPrnUsrQR		(ActSeeMyUsgRep + 72)

#define ActPrnMyTT		(ActSeeMyUsgRep + 73)
#define ActEdiTut		(ActSeeMyUsgRep + 74)
#define ActChgTut		(ActSeeMyUsgRep + 75)
#define ActChgMyTT1stDay	(ActSeeMyUsgRep + 76)

#define ActReqRemFilBrf		(ActSeeMyUsgRep + 77)
#define ActRemFilBrf		(ActSeeMyUsgRep + 78)
#define ActRemFolBrf		(ActSeeMyUsgRep + 79)
#define ActCopBrf		(ActSeeMyUsgRep + 80)
#define ActPasBrf		(ActSeeMyUsgRep + 81)
#define ActRemTreBrf		(ActSeeMyUsgRep + 82)
#define ActFrmCreBrf		(ActSeeMyUsgRep + 83)
#define ActCreFolBrf		(ActSeeMyUsgRep + 84)
#define ActCreLnkBrf		(ActSeeMyUsgRep + 85)
#define ActRenFolBrf		(ActSeeMyUsgRep + 86)
#define ActRcvFilBrfDZ		(ActSeeMyUsgRep + 87)
#define ActRcvFilBrfCla		(ActSeeMyUsgRep + 88)
#define ActExpBrf		(ActSeeMyUsgRep + 89)
#define ActConBrf		(ActSeeMyUsgRep + 90)
#define ActZIPBrf		(ActSeeMyUsgRep + 91)
#define ActReqDatBrf		(ActSeeMyUsgRep + 92)
#define ActChgDatBrf		(ActSeeMyUsgRep + 93)
#define ActDowBrf		(ActSeeMyUsgRep + 94)

#define ActReqRemOldBrf		(ActSeeMyUsgRep + 95)
#define ActRemOldBrf		(ActSeeMyUsgRep + 96)

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
   Tab_Tab_t Tab;
   Act_Action_t SuperAction;
   unsigned PermissionCrsIfIBelong;
   unsigned PermissionCrsIfIDontBelong;
   unsigned PermissionDeg;
   unsigned PermissionCtr;
   unsigned PermissionIns;
   unsigned PermissionCty;
   unsigned PermissionSys;
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
char *Act_GetActionTextFromDB (long ActCod,
                               char ActTxt[Act_MAX_BYTES_ACTION_TXT + 1]);

void Act_FormGoToStart (Act_Action_t NextAction);
void Act_FormStart (Act_Action_t NextAction);
void Act_FormStartOnSubmit (Act_Action_t NextAction,const char *OnSubmit);
void Act_FormStartUnique (Act_Action_t NextAction);
void Act_FormStartAnchor (Act_Action_t NextAction,const char *Anchor);
void Act_FormStartUniqueAnchor (Act_Action_t NextAction,const char *Anchor);
void Act_FormStartId (Act_Action_t NextAction,const char *Id);
void Act_SetParamsForm (char *ParamsStr,Act_Action_t NextAction,
                        bool PutParameterLocationIfNoSesion);
void Act_FormEnd (void);
void Act_LinkFormSubmit (const char *Title,const char *LinkStyle,
                         const char *OnSubmit);
void Act_LinkFormSubmitUnique (const char *Title,const char *LinkStyle);
void Act_LinkFormSubmitId (const char *Title,const char *LinkStyle,
                           const char *Id,const char *OnSubmit);
void Act_LinkFormSubmitAnimated (const char *Title,const char *LinkStyle,
                                 const char *OnSubmit);

void Act_SetUniqueId (char UniqueId[Act_MAX_BYTES_ID]);

void Act_AdjustActionWhenNoUsrLogged (void);
void Act_AdjustCurrentAction (void);

#endif
