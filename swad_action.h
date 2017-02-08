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
#include "swad_tab.h"
#include "swad_text.h"

/*****************************************************************************/
/****************************** Public constants *****************************/
/*****************************************************************************/

#define Act_MAX_LENGTH_ACTION_TXT	255

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

#define Act_NUM_ACTIONS	(1 + 9 + 51 + 14 + 44 + 24 + 21 + 112 + 57 + 416 + 159 + 172 + 36 + 31 + 97)

#define Act_MAX_ACTION_COD 1621

#define Act_MAX_OPTIONS_IN_MENU_PER_TAB 12

#define Act_MAX_LENGTH_ID (32 + Cry_LENGTH_ENCRYPTED_STR_SHA256_BASE64 + 10 + 1)

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
#define ActReqRemOldCrs		(ActWebSvc +  4)
#define ActSeeDegTyp		(ActWebSvc +  5)
#define ActSeeMai		(ActWebSvc +  6)
#define ActSeeBan		(ActWebSvc +  7)
#define ActSeeLnk		(ActWebSvc +  8)
#define ActLstPlg		(ActWebSvc +  9)
#define ActSetUp		(ActWebSvc + 10)
// Secondary actions
#define ActSysSch		(ActWebSvc + 11)
#define ActEdiDegTyp		(ActWebSvc + 12)
#define ActEdiCty		(ActWebSvc + 13)
#define ActNewCty		(ActWebSvc + 14)
#define ActRemCty		(ActWebSvc + 15)
#define ActRenCty		(ActWebSvc + 16)
#define ActChgCtyWWW		(ActWebSvc + 17)

#define ActNewDegTyp		(ActWebSvc + 18)
#define ActRemDegTyp		(ActWebSvc + 19)
#define ActRenDegTyp		(ActWebSvc + 20)

#define ActRemOldCrs		(ActWebSvc + 21)
#define ActEdiMai		(ActWebSvc + 22)
#define ActNewMai		(ActWebSvc + 23)
#define ActRemMai		(ActWebSvc + 24)
#define ActRenMaiSho		(ActWebSvc + 25)
#define ActRenMaiFul		(ActWebSvc + 26)

#define ActEdiBan		(ActWebSvc + 27)
#define ActNewBan		(ActWebSvc + 28)
#define ActRemBan		(ActWebSvc + 29)
#define ActShoBan		(ActWebSvc + 30)
#define ActHidBan		(ActWebSvc + 31)
#define ActRenBanSho		(ActWebSvc + 32)
#define ActRenBanFul		(ActWebSvc + 33)
#define ActChgBanImg		(ActWebSvc + 34)
#define ActChgBanWWW		(ActWebSvc + 35)
#define ActClkBan		(ActWebSvc + 36)

#define ActEdiLnk		(ActWebSvc + 37)
#define ActNewLnk		(ActWebSvc + 38)
#define ActRemLnk		(ActWebSvc + 39)
#define ActRenLnkSho		(ActWebSvc + 40)
#define ActRenLnkFul		(ActWebSvc + 41)
#define ActChgLnkWWW		(ActWebSvc + 42)

#define ActEdiPlg		(ActWebSvc + 43)
#define ActNewPlg		(ActWebSvc + 44)
#define ActRemPlg		(ActWebSvc + 45)
#define ActRenPlg		(ActWebSvc + 46)
#define ActChgPlgDes		(ActWebSvc + 47)
#define ActChgPlgLog		(ActWebSvc + 48)
#define ActChgPlgAppKey		(ActWebSvc + 49)
#define ActChgPlgURL		(ActWebSvc + 50)
#define ActChgPlgIP		(ActWebSvc + 51)

/*****************************************************************************/
/******************************** Country tab ********************************/
/*****************************************************************************/
// Actions in menu
#define ActCtyReqSch		(ActChgPlgIP +  1)
#define ActSeeCtyInf		(ActChgPlgIP +  2)
#define ActSeeIns		(ActChgPlgIP +  3)

// Secondary actions
#define ActCtySch		(ActChgPlgIP +  4)
#define ActPrnCtyInf		(ActChgPlgIP +  5)
#define ActChgCtyMapAtt		(ActChgPlgIP +  6)

#define ActEdiIns		(ActChgPlgIP +  7)
#define ActReqIns		(ActChgPlgIP +  8)
#define ActNewIns		(ActChgPlgIP +  9)
#define ActRemIns		(ActChgPlgIP + 10)
#define ActRenInsSho		(ActChgPlgIP + 11)
#define ActRenInsFul		(ActChgPlgIP + 12)
#define ActChgInsWWW		(ActChgPlgIP + 13)
#define ActChgInsSta		(ActChgPlgIP + 14)

/*****************************************************************************/
/****************************** Institution tab ******************************/
/*****************************************************************************/
// Actions in menu
#define ActInsReqSch		(ActChgInsSta +  1)
#define ActSeeInsInf		(ActChgInsSta +  2)
#define ActSeeCtr		(ActChgInsSta +  3)
#define ActSeeDpt		(ActChgInsSta +  4)
#define ActSeePlc		(ActChgInsSta +  5)
#define ActSeeHld		(ActChgInsSta +  6)

// Secondary actions
#define ActInsSch		(ActChgInsSta +  7)
#define ActPrnInsInf		(ActChgInsSta +  8)
#define ActChgInsCtyCfg		(ActChgInsSta +  9)
#define ActRenInsShoCfg		(ActChgInsSta + 10)
#define ActRenInsFulCfg		(ActChgInsSta + 11)
#define ActChgInsWWWCfg		(ActChgInsSta + 12)
#define ActReqInsLog		(ActChgInsSta + 13)
#define ActRecInsLog		(ActChgInsSta + 14)
#define ActRemInsLog		(ActChgInsSta + 15)

#define ActEdiCtr		(ActChgInsSta + 16)
#define ActReqCtr		(ActChgInsSta + 17)
#define ActNewCtr		(ActChgInsSta + 18)
#define ActRemCtr		(ActChgInsSta + 19)
#define ActChgCtrPlc		(ActChgInsSta + 20)
#define ActRenCtrSho		(ActChgInsSta + 21)
#define ActRenCtrFul		(ActChgInsSta + 22)
#define ActChgCtrWWW		(ActChgInsSta + 23)
#define ActChgCtrSta		(ActChgInsSta + 24)

#define ActEdiDpt		(ActChgInsSta + 25)
#define ActNewDpt		(ActChgInsSta + 26)
#define ActRemDpt		(ActChgInsSta + 27)
#define ActChgDptIns		(ActChgInsSta + 28)
#define ActRenDptSho		(ActChgInsSta + 29)
#define ActRenDptFul		(ActChgInsSta + 30)
#define ActChgDptWWW		(ActChgInsSta + 31)

#define ActEdiPlc		(ActChgInsSta + 32)
#define ActNewPlc		(ActChgInsSta + 33)
#define ActRemPlc		(ActChgInsSta + 34)
#define ActRenPlcSho		(ActChgInsSta + 35)
#define ActRenPlcFul		(ActChgInsSta + 36)

#define ActEdiHld		(ActChgInsSta + 37)
#define ActNewHld		(ActChgInsSta + 38)
#define ActRemHld		(ActChgInsSta + 39)
#define ActChgHldPlc		(ActChgInsSta + 40)
#define ActChgHldTyp		(ActChgInsSta + 41)
#define ActChgHldStrDat		(ActChgInsSta + 42)
#define ActChgHldEndDat		(ActChgInsSta + 43)
#define ActRenHld		(ActChgInsSta + 44)

/*****************************************************************************/
/********************************* Centre tab ********************************/
/*****************************************************************************/
// Actions in menu
#define ActCtrReqSch		(ActRenHld +  1)
#define ActSeeCtrInf		(ActRenHld +  2)
#define ActSeeDeg		(ActRenHld +  3)

// Secondary actions
#define ActCtrSch		(ActRenHld +  4)
#define ActPrnCtrInf		(ActRenHld +  5)
#define ActChgCtrInsCfg		(ActRenHld +  6)
#define ActRenCtrShoCfg		(ActRenHld +  7)
#define ActRenCtrFulCfg		(ActRenHld +  8)
#define ActChgCtrWWWCfg		(ActRenHld +  9)
#define ActReqCtrLog		(ActRenHld + 10)
#define ActRecCtrLog		(ActRenHld + 11)
#define ActRemCtrLog		(ActRenHld + 12)
#define ActReqCtrPho		(ActRenHld + 13)
#define ActRecCtrPho		(ActRenHld + 14)
#define ActChgCtrPhoAtt		(ActRenHld + 15)

#define ActEdiDeg		(ActRenHld + 16)
#define ActReqDeg		(ActRenHld + 17)
#define ActNewDeg		(ActRenHld + 18)
#define ActRemDeg		(ActRenHld + 19)
#define ActRenDegSho		(ActRenHld + 20)
#define ActRenDegFul		(ActRenHld + 21)
#define ActChgDegTyp		(ActRenHld + 22)
#define ActChgDegWWW		(ActRenHld + 23)
#define ActChgDegSta		(ActRenHld + 24)

/*****************************************************************************/
/********************************* Degree tab ********************************/
/*****************************************************************************/
// Actions in menu
#define ActDegReqSch		(ActChgDegSta +  1)
#define ActSeeDegInf		(ActChgDegSta +  2)
#define ActSeeCrs		(ActChgDegSta +  3)

// Secondary actions
#define ActDegSch		(ActChgDegSta +  4)
#define ActPrnDegInf		(ActChgDegSta +  5)
#define ActChgDegCtrCfg		(ActChgDegSta +  6)
#define ActRenDegShoCfg		(ActChgDegSta +  7)
#define ActRenDegFulCfg		(ActChgDegSta +  8)
#define ActChgDegWWWCfg		(ActChgDegSta +  9)
#define ActReqDegLog		(ActChgDegSta + 10)
#define ActRecDegLog		(ActChgDegSta + 11)
#define ActRemDegLog		(ActChgDegSta + 12)

#define ActEdiCrs		(ActChgDegSta + 13)
#define ActReqCrs		(ActChgDegSta + 14)
#define ActNewCrs		(ActChgDegSta + 15)
#define ActRemCrs		(ActChgDegSta + 16)
#define ActChgInsCrsCod		(ActChgDegSta + 17)
#define ActChgCrsYea		(ActChgDegSta + 18)
#define ActRenCrsSho		(ActChgDegSta + 19)
#define ActRenCrsFul		(ActChgDegSta + 20)
#define ActChgCrsSta		(ActChgDegSta + 21)

/*****************************************************************************/
/******************************** Course tab *********************************/
/*****************************************************************************/
// Actions in menu
#define ActCrsReqSch		(ActChgCrsSta +   1)
#define ActSeeCrsInf		(ActChgCrsSta +   2)
#define ActSeeTchGui		(ActChgCrsSta +   3)
#define ActSeeSyl		(ActChgCrsSta +   4)
#define ActSeeBib		(ActChgCrsSta +   5)
#define ActSeeFAQ		(ActChgCrsSta +   6)
#define ActSeeCrsLnk		(ActChgCrsSta +   7)
#define ActSeeCrsTT		(ActChgCrsSta +   8)

// Secondary actions
#define ActCrsSch		(ActChgCrsSta +   9)
#define ActPrnCrsInf		(ActChgCrsSta +  10)
#define ActChgCrsDegCfg		(ActChgCrsSta +  11)
#define ActRenCrsShoCfg		(ActChgCrsSta +  12)
#define ActRenCrsFulCfg		(ActChgCrsSta +  13)
#define ActChgInsCrsCodCfg	(ActChgCrsSta +  14)
#define ActChgCrsYeaCfg		(ActChgCrsSta +  15)
#define ActEdiCrsInf		(ActChgCrsSta +  16)
#define ActEdiTchGui		(ActChgCrsSta +  17)
#define ActSeeSylLec		(ActChgCrsSta +  18)
#define ActSeeSylPra		(ActChgCrsSta +  19)
#define ActEdiSylLec		(ActChgCrsSta +  20)
#define ActEdiSylPra		(ActChgCrsSta +  21)
#define ActDelItmSylLec		(ActChgCrsSta +  22)
#define ActDelItmSylPra		(ActChgCrsSta +  23)
#define ActUp_IteSylLec		(ActChgCrsSta +  24)
#define ActUp_IteSylPra		(ActChgCrsSta +  25)
#define ActDwnIteSylLec		(ActChgCrsSta +  26)
#define ActDwnIteSylPra		(ActChgCrsSta +  27)
#define ActRgtIteSylLec		(ActChgCrsSta +  28)
#define ActRgtIteSylPra		(ActChgCrsSta +  29)
#define ActLftIteSylLec		(ActChgCrsSta +  30)
#define ActLftIteSylPra		(ActChgCrsSta +  31)
#define ActInsIteSylLec		(ActChgCrsSta +  32)
#define ActInsIteSylPra		(ActChgCrsSta +  33)
#define ActModIteSylLec		(ActChgCrsSta +  34)
#define ActModIteSylPra		(ActChgCrsSta +  35)

#define ActEdiBib		(ActChgCrsSta +  36)
#define ActEdiFAQ		(ActChgCrsSta +  37)
#define ActEdiCrsLnk		(ActChgCrsSta +  38)

#define ActChgFrcReaCrsInf	(ActChgCrsSta +  39)
#define ActChgFrcReaTchGui	(ActChgCrsSta +  40)
#define ActChgFrcReaSylLec	(ActChgCrsSta +  41)
#define ActChgFrcReaSylPra	(ActChgCrsSta +  42)
#define ActChgFrcReaBib		(ActChgCrsSta +  43)
#define ActChgFrcReaFAQ		(ActChgCrsSta +  44)
#define ActChgFrcReaCrsLnk	(ActChgCrsSta +  45)

#define ActChgHavReaCrsInf	(ActChgCrsSta +  46)
#define ActChgHavReaTchGui	(ActChgCrsSta +  47)
#define ActChgHavReaSylLec	(ActChgCrsSta +  48)
#define ActChgHavReaSylPra	(ActChgCrsSta +  49)
#define ActChgHavReaBib		(ActChgCrsSta +  50)
#define ActChgHavReaFAQ		(ActChgCrsSta +  51)
#define ActChgHavReaCrsLnk	(ActChgCrsSta +  52)

#define ActSelInfSrcCrsInf	(ActChgCrsSta +  53)
#define ActSelInfSrcTchGui	(ActChgCrsSta +  54)
#define ActSelInfSrcSylLec	(ActChgCrsSta +  55)
#define ActSelInfSrcSylPra	(ActChgCrsSta +  56)
#define ActSelInfSrcBib		(ActChgCrsSta +  57)
#define ActSelInfSrcFAQ		(ActChgCrsSta +  58)
#define ActSelInfSrcCrsLnk	(ActChgCrsSta +  59)
#define ActRcvURLCrsInf		(ActChgCrsSta +  60)
#define ActRcvURLTchGui		(ActChgCrsSta +  61)
#define ActRcvURLSylLec		(ActChgCrsSta +  62)
#define ActRcvURLSylPra		(ActChgCrsSta +  63)
#define ActRcvURLBib		(ActChgCrsSta +  64)
#define ActRcvURLFAQ		(ActChgCrsSta +  65)
#define ActRcvURLCrsLnk		(ActChgCrsSta +  66)
#define ActRcvPagCrsInf		(ActChgCrsSta +  67)
#define ActRcvPagTchGui		(ActChgCrsSta +  68)
#define ActRcvPagSylLec		(ActChgCrsSta +  69)
#define ActRcvPagSylPra		(ActChgCrsSta +  70)
#define ActRcvPagBib		(ActChgCrsSta +  71)
#define ActRcvPagFAQ		(ActChgCrsSta +  72)
#define ActRcvPagCrsLnk		(ActChgCrsSta +  73)
#define ActEditorCrsInf		(ActChgCrsSta +  74)
#define ActEditorTchGui		(ActChgCrsSta +  75)
#define ActEditorSylLec		(ActChgCrsSta +  76)
#define ActEditorSylPra		(ActChgCrsSta +  77)
#define ActEditorBib		(ActChgCrsSta +  78)
#define ActEditorFAQ		(ActChgCrsSta +  79)
#define ActEditorCrsLnk		(ActChgCrsSta +  80)
#define ActPlaTxtEdiCrsInf	(ActChgCrsSta +  81)
#define ActPlaTxtEdiTchGui	(ActChgCrsSta +  82)
#define ActPlaTxtEdiSylLec	(ActChgCrsSta +  83)
#define ActPlaTxtEdiSylPra	(ActChgCrsSta +  84)
#define ActPlaTxtEdiBib		(ActChgCrsSta +  85)
#define ActPlaTxtEdiFAQ		(ActChgCrsSta +  86)
#define ActPlaTxtEdiCrsLnk	(ActChgCrsSta +  87)
#define ActRchTxtEdiCrsInf	(ActChgCrsSta +  88)
#define ActRchTxtEdiTchGui	(ActChgCrsSta +  89)
#define ActRchTxtEdiSylLec	(ActChgCrsSta +  90)
#define ActRchTxtEdiSylPra	(ActChgCrsSta +  91)
#define ActRchTxtEdiBib		(ActChgCrsSta +  92)
#define ActRchTxtEdiFAQ		(ActChgCrsSta +  93)
#define ActRchTxtEdiCrsLnk	(ActChgCrsSta +  94)
#define ActRcvPlaTxtCrsInf	(ActChgCrsSta +  95)
#define ActRcvPlaTxtTchGui	(ActChgCrsSta +  96)
#define ActRcvPlaTxtSylLec	(ActChgCrsSta +  97)
#define ActRcvPlaTxtSylPra	(ActChgCrsSta +  98)
#define ActRcvPlaTxtBib		(ActChgCrsSta +  99)
#define ActRcvPlaTxtFAQ		(ActChgCrsSta + 100)
#define ActRcvPlaTxtCrsLnk	(ActChgCrsSta + 101)
#define ActRcvRchTxtCrsInf	(ActChgCrsSta + 102)
#define ActRcvRchTxtTchGui	(ActChgCrsSta + 103)
#define ActRcvRchTxtSylLec	(ActChgCrsSta + 104)
#define ActRcvRchTxtSylPra	(ActChgCrsSta + 105)
#define ActRcvRchTxtBib		(ActChgCrsSta + 106)
#define ActRcvRchTxtFAQ		(ActChgCrsSta + 107)
#define ActRcvRchTxtCrsLnk	(ActChgCrsSta + 108)

#define ActPrnCrsTT		(ActChgCrsSta + 109)
#define ActEdiCrsTT		(ActChgCrsSta + 110)
#define ActChgCrsTT		(ActChgCrsSta + 111)
#define ActChgCrsTT1stDay	(ActChgCrsSta + 112)

/*****************************************************************************/
/***************************** Assessment tab ********************************/
/*****************************************************************************/
// Actions in menu
#define ActSeeAss		(ActChgCrsTT1stDay +   1)
#define ActSeeAsg		(ActChgCrsTT1stDay +   2)
#define ActReqTst		(ActChgCrsTT1stDay +   3)
#define ActSeeCal		(ActChgCrsTT1stDay +   4)
#define ActSeeAllExaAnn		(ActChgCrsTT1stDay +   5)
// Secondary actions
#define ActEdiAss		(ActChgCrsTT1stDay +   6)
#define ActChgFrcReaAss		(ActChgCrsTT1stDay +   7)
#define ActChgHavReaAss		(ActChgCrsTT1stDay +   8)
#define ActSelInfSrcAss		(ActChgCrsTT1stDay +   9)
#define ActRcvURLAss		(ActChgCrsTT1stDay +  10)
#define ActRcvPagAss		(ActChgCrsTT1stDay +  11)
#define ActEditorAss		(ActChgCrsTT1stDay +  12)
#define ActPlaTxtEdiAss		(ActChgCrsTT1stDay +  13)
#define ActRchTxtEdiAss		(ActChgCrsTT1stDay +  14)
#define ActRcvPlaTxtAss		(ActChgCrsTT1stDay +  15)
#define ActRcvRchTxtAss		(ActChgCrsTT1stDay +  16)

#define ActFrmNewAsg		(ActChgCrsTT1stDay +  17)
#define ActEdiOneAsg		(ActChgCrsTT1stDay +  18)
#define ActNewAsg		(ActChgCrsTT1stDay +  19)
#define ActChgAsg		(ActChgCrsTT1stDay +  20)
#define ActReqRemAsg		(ActChgCrsTT1stDay +  21)
#define ActRemAsg		(ActChgCrsTT1stDay +  22)
#define ActHidAsg		(ActChgCrsTT1stDay +  23)
#define ActShoAsg		(ActChgCrsTT1stDay +  24)

#define ActSeeTst		(ActChgCrsTT1stDay +  25)
#define ActAssTst		(ActChgCrsTT1stDay +  26)
#define ActEdiTstQst		(ActChgCrsTT1stDay +  27)
#define ActEdiOneTstQst		(ActChgCrsTT1stDay +  28)
#define ActReqImpTstQst		(ActChgCrsTT1stDay +  29)
#define ActImpTstQst		(ActChgCrsTT1stDay +  30)
#define ActLstTstQst		(ActChgCrsTT1stDay +  31)
#define ActRcvTstQst		(ActChgCrsTT1stDay +  32)
#define ActReqRemTstQst		(ActChgCrsTT1stDay +  33)
#define ActRemTstQst		(ActChgCrsTT1stDay +  34)
#define ActShfTstQst		(ActChgCrsTT1stDay +  35)
#define ActCfgTst		(ActChgCrsTT1stDay +  36)
#define ActEnableTag		(ActChgCrsTT1stDay +  37)
#define ActDisableTag		(ActChgCrsTT1stDay +  38)
#define ActRenTag		(ActChgCrsTT1stDay +  39)
#define ActRcvCfgTst		(ActChgCrsTT1stDay +  40)

#define ActReqSeeMyTstRes	(ActChgCrsTT1stDay +  41)
#define ActSeeMyTstRes		(ActChgCrsTT1stDay +  42)
#define ActSeeOneTstResMe	(ActChgCrsTT1stDay +  43)
#define ActReqSeeUsrTstRes	(ActChgCrsTT1stDay +  44)
#define ActSeeUsrTstRes		(ActChgCrsTT1stDay +  45)
#define ActSeeOneTstResOth	(ActChgCrsTT1stDay +  46)

#define ActPrnCal		(ActChgCrsTT1stDay +  47)
#define ActChgCal1stDay		(ActChgCrsTT1stDay +  48)

#define ActSeeOneExaAnn		(ActChgCrsTT1stDay +  49)
#define ActSeeDatExaAnn		(ActChgCrsTT1stDay +  50)
#define ActEdiExaAnn		(ActChgCrsTT1stDay +  51)
#define ActRcvExaAnn		(ActChgCrsTT1stDay +  52)
#define ActPrnExaAnn		(ActChgCrsTT1stDay +  53)
#define ActReqRemExaAnn		(ActChgCrsTT1stDay +  54)
#define ActRemExaAnn		(ActChgCrsTT1stDay +  55)
#define ActHidExaAnn		(ActChgCrsTT1stDay +  56)
#define ActShoExaAnn		(ActChgCrsTT1stDay +  57)

/*****************************************************************************/
/******************************** Files tab **********************************/
/*****************************************************************************/
// Actions in menu
#define ActSeeAdmDocIns		(ActShoExaAnn +   1)
#define ActAdmShaIns		(ActShoExaAnn +   2)
#define ActSeeAdmDocCtr		(ActShoExaAnn +   3)
#define ActAdmShaCtr		(ActShoExaAnn +   4)
#define ActSeeAdmDocDeg		(ActShoExaAnn +   5)
#define ActAdmShaDeg		(ActShoExaAnn +   6)
#define ActSeeAdmDocCrsGrp	(ActShoExaAnn +   7)
#define ActAdmTchCrsGrp		(ActShoExaAnn +   8)
#define ActAdmShaCrsGrp		(ActShoExaAnn +   9)
#define ActAdmAsgWrkUsr		(ActShoExaAnn +  10)
#define ActReqAsgWrkCrs		(ActShoExaAnn +  11)
#define ActSeeAdmMrk		(ActShoExaAnn +  12)
// Secondary actions
#define ActChgToSeeDocIns	(ActShoExaAnn +  13)
#define ActSeeDocIns		(ActShoExaAnn +  14)
#define ActExpSeeDocIns		(ActShoExaAnn +  15)
#define ActConSeeDocIns		(ActShoExaAnn +  16)
#define ActZIPSeeDocIns		(ActShoExaAnn +  17)
#define ActReqDatSeeDocIns	(ActShoExaAnn +  18)
#define ActDowSeeDocIns		(ActShoExaAnn +  19)
#define ActChgToAdmDocIns	(ActShoExaAnn +  20)
#define ActAdmDocIns		(ActShoExaAnn +  21)
#define ActReqRemFilDocIns	(ActShoExaAnn +  22)
#define ActRemFilDocIns		(ActShoExaAnn +  23)
#define ActRemFolDocIns		(ActShoExaAnn +  24)
#define ActCopDocIns		(ActShoExaAnn +  25)
#define ActPasDocIns		(ActShoExaAnn +  26)
#define ActRemTreDocIns		(ActShoExaAnn +  27)
#define ActFrmCreDocIns		(ActShoExaAnn +  28)
#define ActCreFolDocIns		(ActShoExaAnn +  29)
#define ActCreLnkDocIns		(ActShoExaAnn +  30)
#define ActRenFolDocIns		(ActShoExaAnn +  31)
#define ActRcvFilDocInsDZ	(ActShoExaAnn +  32)
#define ActRcvFilDocInsCla	(ActShoExaAnn +  33)
#define ActExpAdmDocIns		(ActShoExaAnn +  34)
#define ActConAdmDocIns		(ActShoExaAnn +  35)
#define ActZIPAdmDocIns		(ActShoExaAnn +  36)
#define ActShoDocIns		(ActShoExaAnn +  37)
#define ActHidDocIns		(ActShoExaAnn +  38)
#define ActReqDatAdmDocIns	(ActShoExaAnn +  39)
#define ActChgDatAdmDocIns	(ActShoExaAnn +  40)
#define ActDowAdmDocIns		(ActShoExaAnn +  41)

#define ActReqRemFilShaIns	(ActShoExaAnn +  42)
#define ActRemFilShaIns		(ActShoExaAnn +  43)
#define ActRemFolShaIns		(ActShoExaAnn +  44)
#define ActCopShaIns		(ActShoExaAnn +  45)
#define ActPasShaIns		(ActShoExaAnn +  46)
#define ActRemTreShaIns		(ActShoExaAnn +  47)
#define ActFrmCreShaIns		(ActShoExaAnn +  48)
#define ActCreFolShaIns		(ActShoExaAnn +  49)
#define ActCreLnkShaIns		(ActShoExaAnn +  50)
#define ActRenFolShaIns		(ActShoExaAnn +  51)
#define ActRcvFilShaInsDZ	(ActShoExaAnn +  52)
#define ActRcvFilShaInsCla	(ActShoExaAnn +  53)
#define ActExpShaIns		(ActShoExaAnn +  54)
#define ActConShaIns		(ActShoExaAnn +  55)
#define ActZIPShaIns		(ActShoExaAnn +  56)
#define ActReqDatShaIns		(ActShoExaAnn +  57)
#define ActChgDatShaIns		(ActShoExaAnn +  58)
#define ActDowShaIns		(ActShoExaAnn +  59)

#define ActChgToSeeDocCtr	(ActShoExaAnn +  60)
#define ActSeeDocCtr		(ActShoExaAnn +  61)
#define ActExpSeeDocCtr		(ActShoExaAnn +  62)
#define ActConSeeDocCtr		(ActShoExaAnn +  63)
#define ActZIPSeeDocCtr		(ActShoExaAnn +  64)
#define ActReqDatSeeDocCtr	(ActShoExaAnn +  65)
#define ActDowSeeDocCtr		(ActShoExaAnn +  66)
#define ActChgToAdmDocCtr	(ActShoExaAnn +  67)
#define ActAdmDocCtr		(ActShoExaAnn +  68)
#define ActReqRemFilDocCtr	(ActShoExaAnn +  69)
#define ActRemFilDocCtr		(ActShoExaAnn +  70)
#define ActRemFolDocCtr		(ActShoExaAnn +  71)
#define ActCopDocCtr		(ActShoExaAnn +  72)
#define ActPasDocCtr		(ActShoExaAnn +  73)
#define ActRemTreDocCtr		(ActShoExaAnn +  74)
#define ActFrmCreDocCtr		(ActShoExaAnn +  75)
#define ActCreFolDocCtr		(ActShoExaAnn +  76)
#define ActCreLnkDocCtr		(ActShoExaAnn +  77)
#define ActRenFolDocCtr		(ActShoExaAnn +  78)
#define ActRcvFilDocCtrDZ	(ActShoExaAnn +  79)
#define ActRcvFilDocCtrCla	(ActShoExaAnn +  80)
#define ActExpAdmDocCtr		(ActShoExaAnn +  81)
#define ActConAdmDocCtr		(ActShoExaAnn +  82)
#define ActZIPAdmDocCtr		(ActShoExaAnn +  83)
#define ActShoDocCtr		(ActShoExaAnn +  84)
#define ActHidDocCtr		(ActShoExaAnn +  85)
#define ActReqDatAdmDocCtr	(ActShoExaAnn +  86)
#define ActChgDatAdmDocCtr	(ActShoExaAnn +  87)
#define ActDowAdmDocCtr		(ActShoExaAnn +  88)

#define ActReqRemFilShaCtr	(ActShoExaAnn +  89)
#define ActRemFilShaCtr		(ActShoExaAnn +  90)
#define ActRemFolShaCtr		(ActShoExaAnn +  91)
#define ActCopShaCtr		(ActShoExaAnn +  92)
#define ActPasShaCtr		(ActShoExaAnn +  93)
#define ActRemTreShaCtr		(ActShoExaAnn +  94)
#define ActFrmCreShaCtr		(ActShoExaAnn +  95)
#define ActCreFolShaCtr		(ActShoExaAnn +  96)
#define ActCreLnkShaCtr		(ActShoExaAnn +  97)
#define ActRenFolShaCtr		(ActShoExaAnn +  98)
#define ActRcvFilShaCtrDZ	(ActShoExaAnn +  99)
#define ActRcvFilShaCtrCla	(ActShoExaAnn + 100)
#define ActExpShaCtr		(ActShoExaAnn + 101)
#define ActConShaCtr		(ActShoExaAnn + 102)
#define ActZIPShaCtr		(ActShoExaAnn + 103)
#define ActReqDatShaCtr		(ActShoExaAnn + 104)
#define ActChgDatShaCtr		(ActShoExaAnn + 105)
#define ActDowShaCtr		(ActShoExaAnn + 106)

#define ActChgToSeeDocDeg	(ActShoExaAnn + 107)
#define ActSeeDocDeg		(ActShoExaAnn + 108)
#define ActExpSeeDocDeg		(ActShoExaAnn + 109)
#define ActConSeeDocDeg		(ActShoExaAnn + 110)
#define ActZIPSeeDocDeg		(ActShoExaAnn + 111)
#define ActReqDatSeeDocDeg	(ActShoExaAnn + 112)
#define ActDowSeeDocDeg		(ActShoExaAnn + 113)
#define ActChgToAdmDocDeg	(ActShoExaAnn + 114)
#define ActAdmDocDeg		(ActShoExaAnn + 115)
#define ActReqRemFilDocDeg	(ActShoExaAnn + 116)
#define ActRemFilDocDeg		(ActShoExaAnn + 117)
#define ActRemFolDocDeg		(ActShoExaAnn + 118)
#define ActCopDocDeg		(ActShoExaAnn + 119)
#define ActPasDocDeg		(ActShoExaAnn + 120)
#define ActRemTreDocDeg		(ActShoExaAnn + 121)
#define ActFrmCreDocDeg		(ActShoExaAnn + 122)
#define ActCreFolDocDeg		(ActShoExaAnn + 123)
#define ActCreLnkDocDeg		(ActShoExaAnn + 124)
#define ActRenFolDocDeg		(ActShoExaAnn + 125)
#define ActRcvFilDocDegDZ	(ActShoExaAnn + 126)
#define ActRcvFilDocDegCla	(ActShoExaAnn + 127)
#define ActExpAdmDocDeg		(ActShoExaAnn + 128)
#define ActConAdmDocDeg		(ActShoExaAnn + 129)
#define ActZIPAdmDocDeg		(ActShoExaAnn + 130)
#define ActShoDocDeg		(ActShoExaAnn + 131)
#define ActHidDocDeg		(ActShoExaAnn + 132)
#define ActReqDatAdmDocDeg	(ActShoExaAnn + 133)
#define ActChgDatAdmDocDeg	(ActShoExaAnn + 134)
#define ActDowAdmDocDeg		(ActShoExaAnn + 135)

#define ActReqRemFilShaDeg	(ActShoExaAnn + 136)
#define ActRemFilShaDeg		(ActShoExaAnn + 137)
#define ActRemFolShaDeg		(ActShoExaAnn + 138)
#define ActCopShaDeg		(ActShoExaAnn + 139)
#define ActPasShaDeg		(ActShoExaAnn + 140)
#define ActRemTreShaDeg		(ActShoExaAnn + 141)
#define ActFrmCreShaDeg		(ActShoExaAnn + 142)
#define ActCreFolShaDeg		(ActShoExaAnn + 143)
#define ActCreLnkShaDeg		(ActShoExaAnn + 144)
#define ActRenFolShaDeg		(ActShoExaAnn + 145)
#define ActRcvFilShaDegDZ	(ActShoExaAnn + 146)
#define ActRcvFilShaDegCla	(ActShoExaAnn + 147)
#define ActExpShaDeg		(ActShoExaAnn + 148)
#define ActConShaDeg		(ActShoExaAnn + 149)
#define ActZIPShaDeg		(ActShoExaAnn + 150)
#define ActReqDatShaDeg		(ActShoExaAnn + 151)
#define ActChgDatShaDeg		(ActShoExaAnn + 152)
#define ActDowShaDeg		(ActShoExaAnn + 153)

#define ActChgToSeeDocCrs	(ActShoExaAnn + 154)
#define ActSeeDocCrs		(ActShoExaAnn + 155)
#define ActExpSeeDocCrs		(ActShoExaAnn + 156)
#define ActConSeeDocCrs		(ActShoExaAnn + 157)
#define ActZIPSeeDocCrs		(ActShoExaAnn + 158)
#define ActReqDatSeeDocCrs	(ActShoExaAnn + 159)
#define ActDowSeeDocCrs		(ActShoExaAnn + 160)

#define ActSeeDocGrp		(ActShoExaAnn + 161)
#define ActExpSeeDocGrp		(ActShoExaAnn + 162)
#define ActConSeeDocGrp		(ActShoExaAnn + 163)
#define ActZIPSeeDocGrp		(ActShoExaAnn + 164)
#define ActReqDatSeeDocGrp	(ActShoExaAnn + 165)
#define ActDowSeeDocGrp		(ActShoExaAnn + 166)

#define ActChgToAdmDocCrs	(ActShoExaAnn + 167)
#define ActAdmDocCrs		(ActShoExaAnn + 168)
#define ActReqRemFilDocCrs	(ActShoExaAnn + 169)
#define ActRemFilDocCrs		(ActShoExaAnn + 170)
#define ActRemFolDocCrs		(ActShoExaAnn + 171)
#define ActCopDocCrs		(ActShoExaAnn + 172)
#define ActPasDocCrs		(ActShoExaAnn + 173)
#define ActRemTreDocCrs		(ActShoExaAnn + 174)
#define ActFrmCreDocCrs		(ActShoExaAnn + 175)
#define ActCreFolDocCrs		(ActShoExaAnn + 176)
#define ActCreLnkDocCrs		(ActShoExaAnn + 177)
#define ActRenFolDocCrs		(ActShoExaAnn + 178)
#define ActRcvFilDocCrsDZ	(ActShoExaAnn + 179)
#define ActRcvFilDocCrsCla	(ActShoExaAnn + 180)
#define ActExpAdmDocCrs		(ActShoExaAnn + 181)
#define ActConAdmDocCrs		(ActShoExaAnn + 182)
#define ActZIPAdmDocCrs		(ActShoExaAnn + 183)
#define ActShoDocCrs		(ActShoExaAnn + 184)
#define ActHidDocCrs		(ActShoExaAnn + 185)
#define ActReqDatAdmDocCrs	(ActShoExaAnn + 186)
#define ActChgDatAdmDocCrs	(ActShoExaAnn + 187)
#define ActDowAdmDocCrs		(ActShoExaAnn + 188)

#define ActAdmDocGrp		(ActShoExaAnn + 189)
#define ActReqRemFilDocGrp	(ActShoExaAnn + 190)
#define ActRemFilDocGrp		(ActShoExaAnn + 191)
#define ActRemFolDocGrp		(ActShoExaAnn + 192)
#define ActCopDocGrp		(ActShoExaAnn + 193)
#define ActPasDocGrp		(ActShoExaAnn + 194)
#define ActRemTreDocGrp		(ActShoExaAnn + 195)
#define ActFrmCreDocGrp		(ActShoExaAnn + 196)
#define ActCreFolDocGrp		(ActShoExaAnn + 197)
#define ActCreLnkDocGrp		(ActShoExaAnn + 198)
#define ActRenFolDocGrp		(ActShoExaAnn + 199)
#define ActRcvFilDocGrpDZ	(ActShoExaAnn + 200)
#define ActRcvFilDocGrpCla	(ActShoExaAnn + 201)
#define ActExpAdmDocGrp		(ActShoExaAnn + 202)
#define ActConAdmDocGrp		(ActShoExaAnn + 203)
#define ActZIPAdmDocGrp		(ActShoExaAnn + 204)
#define ActShoDocGrp		(ActShoExaAnn + 205)
#define ActHidDocGrp		(ActShoExaAnn + 206)
#define ActReqDatAdmDocGrp	(ActShoExaAnn + 207)
#define ActChgDatAdmDocGrp	(ActShoExaAnn + 208)
#define ActDowAdmDocGrp		(ActShoExaAnn + 209)

#define ActChgToAdmTch		(ActShoExaAnn + 210)

#define ActAdmTchCrs		(ActShoExaAnn + 211)
#define ActReqRemFilTchCrs	(ActShoExaAnn + 212)
#define ActRemFilTchCrs		(ActShoExaAnn + 213)
#define ActRemFolTchCrs		(ActShoExaAnn + 214)
#define ActCopTchCrs		(ActShoExaAnn + 215)
#define ActPasTchCrs		(ActShoExaAnn + 216)
#define ActRemTreTchCrs		(ActShoExaAnn + 217)
#define ActFrmCreTchCrs		(ActShoExaAnn + 218)
#define ActCreFolTchCrs		(ActShoExaAnn + 219)
#define ActCreLnkTchCrs		(ActShoExaAnn + 220)
#define ActRenFolTchCrs		(ActShoExaAnn + 221)
#define ActRcvFilTchCrsDZ	(ActShoExaAnn + 222)
#define ActRcvFilTchCrsCla	(ActShoExaAnn + 223)
#define ActExpTchCrs		(ActShoExaAnn + 224)
#define ActConTchCrs		(ActShoExaAnn + 225)
#define ActZIPTchCrs		(ActShoExaAnn + 226)
#define ActReqDatTchCrs		(ActShoExaAnn + 227)
#define ActChgDatTchCrs		(ActShoExaAnn + 228)
#define ActDowTchCrs		(ActShoExaAnn + 229)

#define ActAdmTchGrp		(ActShoExaAnn + 230)
#define ActReqRemFilTchGrp	(ActShoExaAnn + 231)
#define ActRemFilTchGrp		(ActShoExaAnn + 232)
#define ActRemFolTchGrp		(ActShoExaAnn + 233)
#define ActCopTchGrp		(ActShoExaAnn + 234)
#define ActPasTchGrp		(ActShoExaAnn + 235)
#define ActRemTreTchGrp		(ActShoExaAnn + 236)
#define ActFrmCreTchGrp		(ActShoExaAnn + 237)
#define ActCreFolTchGrp		(ActShoExaAnn + 238)
#define ActCreLnkTchGrp		(ActShoExaAnn + 239)
#define ActRenFolTchGrp		(ActShoExaAnn + 240)
#define ActRcvFilTchGrpDZ	(ActShoExaAnn + 241)
#define ActRcvFilTchGrpCla	(ActShoExaAnn + 242)
#define ActExpTchGrp		(ActShoExaAnn + 243)
#define ActConTchGrp		(ActShoExaAnn + 244)
#define ActZIPTchGrp		(ActShoExaAnn + 245)
#define ActReqDatTchGrp		(ActShoExaAnn + 246)
#define ActChgDatTchGrp		(ActShoExaAnn + 247)
#define ActDowTchGrp		(ActShoExaAnn + 248)

#define ActChgToAdmSha		(ActShoExaAnn + 249)

#define ActAdmShaCrs		(ActShoExaAnn + 250)
#define ActReqRemFilShaCrs	(ActShoExaAnn + 251)
#define ActRemFilShaCrs		(ActShoExaAnn + 252)
#define ActRemFolShaCrs		(ActShoExaAnn + 253)
#define ActCopShaCrs		(ActShoExaAnn + 254)
#define ActPasShaCrs		(ActShoExaAnn + 255)
#define ActRemTreShaCrs		(ActShoExaAnn + 256)
#define ActFrmCreShaCrs		(ActShoExaAnn + 257)
#define ActCreFolShaCrs		(ActShoExaAnn + 258)
#define ActCreLnkShaCrs		(ActShoExaAnn + 259)
#define ActRenFolShaCrs		(ActShoExaAnn + 260)
#define ActRcvFilShaCrsDZ	(ActShoExaAnn + 261)
#define ActRcvFilShaCrsCla	(ActShoExaAnn + 262)
#define ActExpShaCrs		(ActShoExaAnn + 263)
#define ActConShaCrs		(ActShoExaAnn + 264)
#define ActZIPShaCrs		(ActShoExaAnn + 265)
#define ActReqDatShaCrs		(ActShoExaAnn + 266)
#define ActChgDatShaCrs		(ActShoExaAnn + 267)
#define ActDowShaCrs		(ActShoExaAnn + 268)

#define ActAdmShaGrp		(ActShoExaAnn + 269)
#define ActReqRemFilShaGrp	(ActShoExaAnn + 270)
#define ActRemFilShaGrp		(ActShoExaAnn + 271)
#define ActRemFolShaGrp		(ActShoExaAnn + 272)
#define ActCopShaGrp		(ActShoExaAnn + 273)
#define ActPasShaGrp		(ActShoExaAnn + 274)
#define ActRemTreShaGrp		(ActShoExaAnn + 275)
#define ActFrmCreShaGrp		(ActShoExaAnn + 276)
#define ActCreFolShaGrp		(ActShoExaAnn + 277)
#define ActCreLnkShaGrp		(ActShoExaAnn + 278)
#define ActRenFolShaGrp		(ActShoExaAnn + 279)
#define ActRcvFilShaGrpDZ	(ActShoExaAnn + 280)
#define ActRcvFilShaGrpCla	(ActShoExaAnn + 281)
#define ActExpShaGrp		(ActShoExaAnn + 282)
#define ActConShaGrp		(ActShoExaAnn + 283)
#define ActZIPShaGrp		(ActShoExaAnn + 284)
#define ActReqDatShaGrp		(ActShoExaAnn + 285)
#define ActChgDatShaGrp		(ActShoExaAnn + 286)
#define ActDowShaGrp		(ActShoExaAnn + 287)

#define ActAdmAsgWrkCrs		(ActShoExaAnn + 288)

#define ActReqRemFilAsgUsr	(ActShoExaAnn + 289)
#define ActRemFilAsgUsr		(ActShoExaAnn + 290)
#define ActRemFolAsgUsr		(ActShoExaAnn + 291)
#define ActCopAsgUsr		(ActShoExaAnn + 292)
#define ActPasAsgUsr		(ActShoExaAnn + 293)
#define ActRemTreAsgUsr		(ActShoExaAnn + 294)
#define ActFrmCreAsgUsr		(ActShoExaAnn + 295)
#define ActCreFolAsgUsr		(ActShoExaAnn + 296)
#define ActCreLnkAsgUsr		(ActShoExaAnn + 297)
#define ActRenFolAsgUsr		(ActShoExaAnn + 298)
#define ActRcvFilAsgUsrDZ	(ActShoExaAnn + 299)
#define ActRcvFilAsgUsrCla	(ActShoExaAnn + 300)
#define ActExpAsgUsr		(ActShoExaAnn + 301)
#define ActConAsgUsr		(ActShoExaAnn + 302)
#define ActZIPAsgUsr		(ActShoExaAnn + 303)
#define ActReqDatAsgUsr		(ActShoExaAnn + 304)
#define ActChgDatAsgUsr		(ActShoExaAnn + 305)
#define ActDowAsgUsr		(ActShoExaAnn + 306)

#define ActReqRemFilWrkUsr	(ActShoExaAnn + 307)
#define ActRemFilWrkUsr		(ActShoExaAnn + 308)
#define ActRemFolWrkUsr		(ActShoExaAnn + 309)
#define ActCopWrkUsr		(ActShoExaAnn + 310)
#define ActPasWrkUsr		(ActShoExaAnn + 311)
#define ActRemTreWrkUsr		(ActShoExaAnn + 312)
#define ActFrmCreWrkUsr		(ActShoExaAnn + 313)
#define ActCreFolWrkUsr		(ActShoExaAnn + 314)
#define ActCreLnkWrkUsr		(ActShoExaAnn + 315)
#define ActRenFolWrkUsr		(ActShoExaAnn + 316)
#define ActRcvFilWrkUsrDZ	(ActShoExaAnn + 317)
#define ActRcvFilWrkUsrCla	(ActShoExaAnn + 318)
#define ActExpWrkUsr		(ActShoExaAnn + 319)
#define ActConWrkUsr		(ActShoExaAnn + 320)
#define ActZIPWrkUsr		(ActShoExaAnn + 321)
#define ActReqDatWrkUsr		(ActShoExaAnn + 322)
#define ActChgDatWrkUsr		(ActShoExaAnn + 323)
#define ActDowWrkUsr		(ActShoExaAnn + 324)

#define ActReqRemFilAsgCrs	(ActShoExaAnn + 325)
#define ActRemFilAsgCrs		(ActShoExaAnn + 326)
#define ActRemFolAsgCrs		(ActShoExaAnn + 327)
#define ActCopAsgCrs		(ActShoExaAnn + 328)
#define ActPasAsgCrs		(ActShoExaAnn + 329)
#define ActRemTreAsgCrs		(ActShoExaAnn + 330)
#define ActFrmCreAsgCrs		(ActShoExaAnn + 331)
#define ActCreFolAsgCrs		(ActShoExaAnn + 332)
#define ActCreLnkAsgCrs		(ActShoExaAnn + 333)
#define ActRenFolAsgCrs		(ActShoExaAnn + 334)
#define ActRcvFilAsgCrsDZ	(ActShoExaAnn + 335)
#define ActRcvFilAsgCrsCla	(ActShoExaAnn + 336)
#define ActExpAsgCrs		(ActShoExaAnn + 337)
#define ActConAsgCrs		(ActShoExaAnn + 338)
#define ActZIPAsgCrs		(ActShoExaAnn + 339)
#define ActReqDatAsgCrs		(ActShoExaAnn + 340)
#define ActChgDatAsgCrs		(ActShoExaAnn + 341)
#define ActDowAsgCrs		(ActShoExaAnn + 342)

#define ActReqRemFilWrkCrs	(ActShoExaAnn + 343)
#define ActRemFilWrkCrs		(ActShoExaAnn + 344)
#define ActRemFolWrkCrs		(ActShoExaAnn + 345)
#define ActCopWrkCrs		(ActShoExaAnn + 346)
#define ActPasWrkCrs		(ActShoExaAnn + 347)
#define ActRemTreWrkCrs		(ActShoExaAnn + 348)
#define ActFrmCreWrkCrs		(ActShoExaAnn + 349)
#define ActCreFolWrkCrs		(ActShoExaAnn + 350)
#define ActCreLnkWrkCrs		(ActShoExaAnn + 351)
#define ActRenFolWrkCrs		(ActShoExaAnn + 352)
#define ActRcvFilWrkCrsDZ	(ActShoExaAnn + 353)
#define ActRcvFilWrkCrsCla	(ActShoExaAnn + 354)
#define ActExpWrkCrs		(ActShoExaAnn + 355)
#define ActConWrkCrs		(ActShoExaAnn + 356)
#define ActZIPWrkCrs		(ActShoExaAnn + 357)
#define ActReqDatWrkCrs		(ActShoExaAnn + 358)
#define ActChgDatWrkCrs		(ActShoExaAnn + 359)
#define ActDowWrkCrs		(ActShoExaAnn + 360)

#define ActChgToSeeMrk		(ActShoExaAnn + 361)

#define ActSeeMrkCrs		(ActShoExaAnn + 362)
#define ActExpSeeMrkCrs		(ActShoExaAnn + 363)
#define ActConSeeMrkCrs		(ActShoExaAnn + 364)
#define ActReqDatSeeMrkCrs	(ActShoExaAnn + 365)
#define ActSeeMyMrkCrs		(ActShoExaAnn + 366)

#define ActSeeMrkGrp		(ActShoExaAnn + 367)
#define ActExpSeeMrkGrp		(ActShoExaAnn + 368)
#define ActConSeeMrkGrp		(ActShoExaAnn + 369)
#define ActReqDatSeeMrkGrp	(ActShoExaAnn + 370)
#define ActSeeMyMrkGrp		(ActShoExaAnn + 371)

#define ActChgToAdmMrk		(ActShoExaAnn + 372)

#define ActAdmMrkCrs		(ActShoExaAnn + 373)
#define ActReqRemFilMrkCrs	(ActShoExaAnn + 374)
#define ActRemFilMrkCrs		(ActShoExaAnn + 375)
#define ActRemFolMrkCrs		(ActShoExaAnn + 376)
#define ActCopMrkCrs		(ActShoExaAnn + 377)
#define ActPasMrkCrs		(ActShoExaAnn + 378)
#define ActRemTreMrkCrs		(ActShoExaAnn + 379)
#define ActFrmCreMrkCrs		(ActShoExaAnn + 380)
#define ActCreFolMrkCrs		(ActShoExaAnn + 381)
#define ActRenFolMrkCrs		(ActShoExaAnn + 382)
#define ActRcvFilMrkCrsDZ	(ActShoExaAnn + 383)
#define ActRcvFilMrkCrsCla	(ActShoExaAnn + 384)
#define ActExpAdmMrkCrs		(ActShoExaAnn + 385)
#define ActConAdmMrkCrs		(ActShoExaAnn + 386)
#define ActZIPAdmMrkCrs		(ActShoExaAnn + 387)
#define ActShoMrkCrs		(ActShoExaAnn + 388)
#define ActHidMrkCrs		(ActShoExaAnn + 389)
#define ActReqDatAdmMrkCrs	(ActShoExaAnn + 390)
#define ActChgDatAdmMrkCrs	(ActShoExaAnn + 391)
#define ActDowAdmMrkCrs		(ActShoExaAnn + 392)
#define ActChgNumRowHeaCrs	(ActShoExaAnn + 393)
#define ActChgNumRowFooCrs	(ActShoExaAnn + 394)

#define ActAdmMrkGrp		(ActShoExaAnn + 395)
#define ActReqRemFilMrkGrp	(ActShoExaAnn + 396)
#define ActRemFilMrkGrp		(ActShoExaAnn + 397)
#define ActRemFolMrkGrp		(ActShoExaAnn + 398)
#define ActCopMrkGrp		(ActShoExaAnn + 399)
#define ActPasMrkGrp		(ActShoExaAnn + 400)
#define ActRemTreMrkGrp		(ActShoExaAnn + 401)
#define ActFrmCreMrkGrp		(ActShoExaAnn + 402)
#define ActCreFolMrkGrp		(ActShoExaAnn + 403)
#define ActRenFolMrkGrp		(ActShoExaAnn + 404)
#define ActRcvFilMrkGrpDZ	(ActShoExaAnn + 405)
#define ActRcvFilMrkGrpCla	(ActShoExaAnn + 406)
#define ActExpAdmMrkGrp		(ActShoExaAnn + 407)
#define ActConAdmMrkGrp		(ActShoExaAnn + 408)
#define ActZIPAdmMrkGrp		(ActShoExaAnn + 409)
#define ActShoMrkGrp		(ActShoExaAnn + 410)
#define ActHidMrkGrp		(ActShoExaAnn + 411)
#define ActReqDatAdmMrkGrp	(ActShoExaAnn + 412)
#define ActChgDatAdmMrkGrp	(ActShoExaAnn + 413)
#define ActDowAdmMrkGrp		(ActShoExaAnn + 414)
#define ActChgNumRowHeaGrp	(ActShoExaAnn + 415)
#define ActChgNumRowFooGrp	(ActShoExaAnn + 416)

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

#define ActReqPubPrf		(ActLstClk +  17)

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

#define ActSeePubPrf		(ActLstClk +  30)
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
#define ActRcvMsgUsr		(ActCht +  22)
#define ActReqDelAllSntMsg	(ActCht +  23)
#define ActReqDelAllRcvMsg	(ActCht +  24)
#define ActDelAllSntMsg		(ActCht +  25)
#define ActDelAllRcvMsg		(ActCht +  26)
#define ActDelSntMsg		(ActCht +  27)
#define ActDelRcvMsg		(ActCht +  28)
#define ActExpSntMsg		(ActCht +  29)
#define ActExpRcvMsg		(ActCht +  30)
#define ActConSntMsg		(ActCht +  31)
#define ActConRcvMsg		(ActCht +  32)
#define ActLstBanUsr		(ActCht +  33)
#define ActBanUsrMsg		(ActCht +  34)
#define ActUnbUsrMsg		(ActCht +  35)
#define ActUnbUsrLst		(ActCht +  36)

/*****************************************************************************/
/****************************** Statistics tab *******************************/
/*****************************************************************************/
// Actions in menu
#define ActSeeAllSvy		(ActUnbUsrLst +  1)
#define ActReqUseGbl		(ActUnbUsrLst +  2)
#define ActSeePhoDeg		(ActUnbUsrLst +  3)
#define ActReqStaCrs		(ActUnbUsrLst +  4)
#define ActReqAccGbl		(ActUnbUsrLst +  5)
#define ActReqMyUsgRep		(ActUnbUsrLst +  6)
#define ActMFUAct		(ActUnbUsrLst +  7)

// Secondary actions
#define ActSeeOneSvy		(ActUnbUsrLst +  8)
#define ActAnsSvy		(ActUnbUsrLst +  9)
#define ActFrmNewSvy		(ActUnbUsrLst + 10)
#define ActEdiOneSvy		(ActUnbUsrLst + 11)
#define ActNewSvy		(ActUnbUsrLst + 12)
#define ActChgSvy		(ActUnbUsrLst + 13)
#define ActReqRemSvy		(ActUnbUsrLst + 14)
#define ActRemSvy		(ActUnbUsrLst + 15)
#define ActReqRstSvy		(ActUnbUsrLst + 16)
#define ActRstSvy		(ActUnbUsrLst + 17)
#define ActHidSvy		(ActUnbUsrLst + 18)
#define ActShoSvy		(ActUnbUsrLst + 19)
#define ActEdiOneSvyQst		(ActUnbUsrLst + 20)
#define ActRcvSvyQst		(ActUnbUsrLst + 21)
#define ActReqRemSvyQst		(ActUnbUsrLst + 22)
#define ActRemSvyQst		(ActUnbUsrLst + 23)

#define ActSeeUseGbl		(ActUnbUsrLst + 24)
#define ActPrnPhoDeg		(ActUnbUsrLst + 25)
#define ActCalPhoDeg		(ActUnbUsrLst + 26)
#define ActSeeAccGbl		(ActUnbUsrLst + 27)
#define ActReqAccCrs		(ActUnbUsrLst + 28)
#define ActSeeAccCrs		(ActUnbUsrLst + 29)
#define ActSeeAllStaCrs		(ActUnbUsrLst + 30)

#define ActSeeMyUsgRep		(ActUnbUsrLst + 31)

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
#define ActSeeMyPubAgd		(ActSeeMyUsgRep + 28)
#define ActPrnAgdQR		(ActSeeMyUsgRep + 29)

#define ActChkUsrAcc		(ActSeeMyUsgRep + 30)
#define ActCreUsrAcc		(ActSeeMyUsgRep + 31)
#define ActRemID_Me		(ActSeeMyUsgRep + 32)
#define ActNewIDMe		(ActSeeMyUsgRep + 33)
#define ActRemOldNic		(ActSeeMyUsgRep + 34)
#define ActChgNic		(ActSeeMyUsgRep + 35)
#define ActRemMaiMe		(ActSeeMyUsgRep + 36)
#define ActNewMaiMe		(ActSeeMyUsgRep + 37)
#define ActCnfMai		(ActSeeMyUsgRep + 38)
#define ActFrmChgMyPwd		(ActSeeMyUsgRep + 39)
#define ActChgPwd		(ActSeeMyUsgRep + 40)
#define ActReqRemMyAcc		(ActSeeMyUsgRep + 41)
#define ActRemMyAcc		(ActSeeMyUsgRep + 42)

#define ActChgMyData		(ActSeeMyUsgRep + 43)

#define ActReqMyPho		(ActSeeMyUsgRep + 44)
#define ActDetMyPho		(ActSeeMyUsgRep + 45)
#define ActUpdMyPho		(ActSeeMyUsgRep + 46)
#define ActReqRemMyPho		(ActSeeMyUsgRep + 47)
#define ActRemMyPho		(ActSeeMyUsgRep + 48)

#define ActEdiPri		(ActSeeMyUsgRep + 49)
#define ActChgPriPho		(ActSeeMyUsgRep + 50)
#define ActChgPriPrf		(ActSeeMyUsgRep + 51)

#define ActReqEdiMyIns		(ActSeeMyUsgRep + 52)
#define ActChgCtyMyIns		(ActSeeMyUsgRep + 53)
#define ActChgMyIns		(ActSeeMyUsgRep + 54)
#define ActChgMyCtr		(ActSeeMyUsgRep + 55)
#define ActChgMyDpt		(ActSeeMyUsgRep + 56)
#define ActChgMyOff		(ActSeeMyUsgRep + 57)
#define ActChgMyOffPho		(ActSeeMyUsgRep + 58)

#define ActReqEdiMyNet		(ActSeeMyUsgRep + 59)
#define ActChgMyNet		(ActSeeMyUsgRep + 60)

#define ActChgThe		(ActSeeMyUsgRep + 61)
#define ActReqChgLan		(ActSeeMyUsgRep + 62)
#define ActChgLan		(ActSeeMyUsgRep + 63)
#define ActChg1stDay		(ActSeeMyUsgRep + 64)
#define ActChgCol		(ActSeeMyUsgRep + 65)
#define ActHidLftCol		(ActSeeMyUsgRep + 66)
#define ActHidRgtCol		(ActSeeMyUsgRep + 67)
#define ActShoLftCol		(ActSeeMyUsgRep + 68)
#define ActShoRgtCol		(ActSeeMyUsgRep + 69)
#define ActChgIco		(ActSeeMyUsgRep + 70)
#define ActChgMnu		(ActSeeMyUsgRep + 71)
#define ActChgNtfPrf		(ActSeeMyUsgRep + 72)

#define ActPrnUsrQR		(ActSeeMyUsgRep + 73)

#define ActPrnMyTT		(ActSeeMyUsgRep + 74)
#define ActEdiTut		(ActSeeMyUsgRep + 75)
#define ActChgTut		(ActSeeMyUsgRep + 76)
#define ActChgMyTT1stDay	(ActSeeMyUsgRep + 77)

#define ActReqRemFilBrf		(ActSeeMyUsgRep + 78)
#define ActRemFilBrf		(ActSeeMyUsgRep + 79)
#define ActRemFolBrf		(ActSeeMyUsgRep + 80)
#define ActCopBrf		(ActSeeMyUsgRep + 81)
#define ActPasBrf		(ActSeeMyUsgRep + 82)
#define ActRemTreBrf		(ActSeeMyUsgRep + 83)
#define ActFrmCreBrf		(ActSeeMyUsgRep + 84)
#define ActCreFolBrf		(ActSeeMyUsgRep + 85)
#define ActCreLnkBrf		(ActSeeMyUsgRep + 86)
#define ActRenFolBrf		(ActSeeMyUsgRep + 87)
#define ActRcvFilBrfDZ		(ActSeeMyUsgRep + 88)
#define ActRcvFilBrfCla		(ActSeeMyUsgRep + 89)
#define ActExpBrf		(ActSeeMyUsgRep + 90)
#define ActConBrf		(ActSeeMyUsgRep + 91)
#define ActZIPBrf		(ActSeeMyUsgRep + 92)
#define ActReqDatBrf		(ActSeeMyUsgRep + 93)
#define ActChgDatBrf		(ActSeeMyUsgRep + 94)
#define ActDowBrf		(ActSeeMyUsgRep + 95)

#define ActReqRemOldBrf		(ActSeeMyUsgRep + 96)
#define ActRemOldBrf		(ActSeeMyUsgRep + 97)

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
                               char ActTxt[Act_MAX_LENGTH_ACTION_TXT + 1]);

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

void Act_SetUniqueId (char UniqueId[Act_MAX_LENGTH_ID]);

void Act_AdjustActionWhenNoUsrLogged (void);
void Act_AdjustCurrentAction (void);

#endif
