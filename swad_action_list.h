// swad_action_list.h: action list

#ifndef _SWAD_ACT_LST
#define _SWAD_ACT_LST
/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2024 Antonio Cañas Vargas

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
/************************* Not asociates with tabs ***************************/
/*****************************************************************************/

typedef enum
  {
   /********************************* No tab *********************************/

	ActAll,
	ActUnk,
	ActMnu,
	ActRefCon,
	ActWebSvc,

   /******************************* Start tab ********************************/

   // Log in ------------------------------------------------------------------

	ActFrmLogIn,
	ActLogIn,
	ActLogInNew,
	ActLogInLan,
	ActAnnSee,
	ActReqSndNewPwd,
	ActSndNewPwd,
	ActLogOut,

   // Search ------------------------------------------------------------------

	ActReqSch,
	ActSch,

   // Timeline ----------------------------------------------------------------

	ActSeeGblTL,
	ActRefNewPubGblTL,
	ActRefOldPubGblTL,
	ActRcvPstGblTL,
	ActRcvComGblTL,
	ActShoHidComGblTL,
	ActAllShaNotGblTL,
	ActAllFavNotGblTL,
	ActAllFavComGblTL,
	ActShaNotGblTL,
	ActUnsNotGblTL,
	ActFavNotGblTL,
	ActUnfNotGblTL,
	ActFavComGblTL,
	ActUnfComGblTL,
	ActReqRemPubGblTL,
	ActRemPubGblTL,
	ActReqRemComGblTL,
	ActRemComGblTL,

   // Profiles ----------------------------------------------------------------

	ActSeeSocPrf,
	ActReqOthPubPrf,
	ActRefOldPubUsrTL,
	ActRcvPstUsrTL,
	ActRcvComUsrTL,
	ActShoHidComUsrTL,
	ActAllShaNotUsrTL,
	ActAllFavNotUsrTL,
	ActAllFavComUsrTL,
	ActShaNotUsrTL,
	ActUnsNotUsrTL,
	ActFavNotUsrTL,
	ActUnfNotUsrTL,
	ActFavComUsrTL,
	ActUnfComUsrTL,
	ActReqRemPubUsrTL,
	ActRemPubUsrTL,
	ActReqRemComUsrTL,
	ActRemComUsrTL,
	ActSeeOthPubPrf,
	ActCalFig,
	ActFolUsr,
	ActUnfUsr,
	ActSeeFlg,
	ActSeeFlr,
	ActSeeUsrAgd,

   // Calendar ----------------------------------------------------------------

	ActSeeCal,
	ActPrnCal,
	ActChgCal1stDay,

   // Notifications -----------------------------------------------------------

	ActSeeNtf,
	ActSeeNewNtf,
	ActMrkNtfSee,
	ActSeeMai,
	ActEdiMai,
	ActNewMai,
	ActRemMai,
	ActRenMaiSho,
	ActRenMaiFul,

   /****************************** System tab ********************************/

   // System info -------------------------------------------------------------

	ActSeeSysInf,
	ActPrnSysInf,

   // Countries ---------------------------------------------------------------

	ActSeeCty,
	ActEdiCty,
	ActNewCty,
	ActRemCty,
	ActRenCty,
	ActChgCtyWWW,

   // Pending -----------------------------------------------------------------

	ActSeePen,

   // Links -------------------------------------------------------------------

	ActSeeLnk,
	ActEdiLnk,
	ActNewLnk,
	ActRemLnk,
	ActRenLnkSho,
	ActRenLnkFul,
	ActChgLnkWWW,
	ActSeeBan,
	ActEdiBan,
	ActNewBan,
	ActRemBan,
	ActUnhBan,
	ActHidBan,
	ActRenBanSho,
	ActRenBanFul,
	ActChgBanImg,
	ActChgBanWWW,
	ActClkBan,

   // Plugins -----------------------------------------------------------------

	ActSeePlg,
	ActEdiPlg,
	ActNewPlg,
	ActRemPlg,
	ActRenPlg,
	ActChgPlgDes,
	ActChgPlgLog,
	ActChgPlgAppKey,
	ActChgPlgURL,
	ActChgPlgIP,

   // Maintenance -------------------------------------------------------------

	ActMtn,
	ActSetUp,
	ActReqRemOldCrs,
	ActRemOldCrs,
	ActCvtSyl,

   /****************************** Country tab *******************************/

   // Country info ------------------------------------------------------------

	ActSeeCtyInf,
	ActPrnCtyInf,
	ActChgCtyMapAtt,

   // Institutions ------------------------------------------------------------

	ActSeeIns,
	ActEdiIns,
	ActReqIns,
	ActNewIns,
	ActRemIns,
	ActRenInsSho,
	ActRenInsFul,
	ActChgInsWWW,
	ActChgInsSta,

   /**************************** Institution tab *****************************/

   // Institution info --------------------------------------------------------

	ActSeeInsInf,
	ActPrnInsInf,
	ActChgInsCtyCfg,
	ActRenInsShoCfg,
	ActRenInsFulCfg,
	ActChgInsWWWCfg,
	ActReqInsLog,
	ActRecInsLog,
	ActRemInsLog,

   // Centers -----------------------------------------------------------------

	ActSeeCtr,
	ActEdiCtr,
	ActReqCtr,
	ActNewCtr,
	ActRemCtr,
	ActChgCtrPlc,
	ActRenCtrSho,
	ActRenCtrFul,
	ActChgCtrWWW,
	ActChgCtrSta,

   // Places ------------------------------------------------------------------

	ActSeePlc,
	ActEdiPlc,
	ActNewPlc,
	ActRemPlc,
	ActRenPlcSho,
	ActRenPlcFul,

   // Departments -------------------------------------------------------------

	ActSeeDpt,
	ActEdiDpt,
	ActNewDpt,
	ActRemDpt,
	ActChgDptIns,
	ActRenDptSho,
	ActRenDptFul,
	ActChgDptWWW,

   // Holidays ----------------------------------------------------------------

	ActSeeHld,
	ActEdiHld,
	ActNewHld,
	ActRemHld,
	ActChgHldPlc,
	ActChgHldTyp,
	ActChgHldStrDat,
	ActChgHldEndDat,
	ActRenHld,

   /******************************* Center tab *******************************/

   // Center info -------------------------------------------------------------

	ActSeeCtrInf,
	ActPrnCtrInf,
	ActChgCtrInsCfg,
	ActRenCtrShoCfg,
	ActRenCtrFulCfg,
	ActChgCtrPlcCfg,
	ActChgCtrLatCfg,
	ActChgCtrLgtCfg,
	ActChgCtrAltCfg,
	ActChgCtrWWWCfg,
	ActReqCtrLog,
	ActRecCtrLog,
	ActRemCtrLog,
	ActReqCtrPho,
	ActRecCtrPho,
	ActChgCtrPhoAtt,

   // Degrees -----------------------------------------------------------------

	ActSeeDeg,
	ActEdiDeg,
	ActReqDeg,
	ActNewDeg,
	ActRemDeg,
	ActRenDegSho,
	ActRenDegFul,
	ActChgDegTyp,
	ActChgDegWWW,
	ActChgDegSta,
	ActSeeDegTyp,
	ActEdiDegTyp,
	ActNewDegTyp,
	ActRemDegTyp,
	ActRenDegTyp,

   // Buildings ---------------------------------------------------------------

	ActSeeBld,
	ActEdiBld,
	ActNewBld,
	ActRemBld,
	ActRenBldSho,
	ActRenBldFul,
	ActRenBldLoc,

   // Rooms -------------------------------------------------------------------

	ActSeeRoo,
	ActEdiRoo,
	ActNewRoo,
	ActRemRoo,
	ActChgRooBld,
	ActChgRooFlo,
	ActChgRooTyp,
	ActRenRooSho,
	ActRenRooFul,
	ActChgRooMaxUsr,
	ActChgRooMAC,

   /******************************* Degree tab *******************************/

   // Degree info -------------------------------------------------------------

	ActSeeDegInf,
	ActPrnDegInf,
	ActChgDegCtrCfg,
	ActRenDegShoCfg,
	ActRenDegFulCfg,
	ActChgDegWWWCfg,
	ActReqDegLog,
	ActRecDegLog,
	ActRemDegLog,

   // Courses -----------------------------------------------------------------

	ActSeeCrs,
	ActEdiCrs,
	ActReqCrs,
	ActNewCrs,
	ActRemCrs,
	ActChgInsCrsCod,
	ActChgCrsYea,
	ActRenCrsSho,
	ActRenCrsFul,
	ActChgCrsSta,

   /****************************** Course tab ********************************/

   // Course info -------------------------------------------------------------

	ActSeeCrsInf,
	ActPrnCrsInf,
	ActEdiTreInf,		// #2038
	ActSeeTreNodInf,	// #2039
	ActFrmChgTreNodInf,	// #2040
	ActFrmNewTreNodInf,	// #2041
	ActChgTreNodInf,	// #2042
	ActNewTreNodInf,	// #2043
	ActReqRemTreNodInf,	// #2044
	ActRemTreNodInf,	// #2045
	ActHidTreNodInf,	// #2046
	ActUnhTreNodInf,	// #2047
	ActUp_TreNodInf,	// #2048
	ActDwnTreNodInf,	// #2049
	ActLftTreNodInf,	// #2050
	ActRgtTreNodInf,	// #2051
	ActExpSeeTreNodInf,	// #2052
	ActConSeeTreNodInf,	// #2053
	ActExpEdiTreNodInf,	// #2054
	ActConEdiTreNodInf,	// #2055
	ActChgCrsDegCfg,
	ActRenCrsShoCfg,
	ActRenCrsFulCfg,
	ActChgInsCrsCodCfg,
	ActChgCrsYeaCfg,
	ActCfgCrsInf,
	ActChgFrcReaCrsInf,
	ActChgHavReaCrsInf,
	ActSelInfSrcCrsInf,
	ActEdiPagCrsInf,
	ActRcvPagCrsInf,
	ActEdiURLCrsInf,
	ActRcvURLCrsInf,
	ActEditorCrsInf,
	ActEdiPlaTxtCrsInf,
	ActRcvPlaTxtCrsInf,
	ActEdiRchTxtCrsInf,
	ActRcvRchTxtCrsInf,
	ActReqLnkCrsInf,

   // Program -----------------------------------------------------------------

	ActSeePrg,
	ActEdiTrePrg,
	ActSeeTreNodPrg,
	ActFrmChgTreNodPrg,
	ActFrmNewTreNodPrg,
	ActChgTreNodPrg,
	ActNewTreNodPrg,
	ActReqRemTreNodPrg,
	ActRemTreNodPrg,
	ActHidTreNodPrg,
	ActUnhTreNodPrg,
	ActUp_TreNodPrg,
	ActDwnTreNodPrg,
	ActLftTreNodPrg,
	ActRgtTreNodPrg,
	ActExpSeeTreNodPrg,
	ActConSeeTreNodPrg,
	ActExpEdiTreNodPrg,
	ActConEdiTreNodPrg,
	ActSeeRscCliPrg,
	ActRemRscCliPrg,
	ActFrmSeePrgRsc,
	ActFrmEdiPrgRsc,
	ActNewPrgRsc,
	ActRenPrgRsc,
	ActReqRemPrgRsc,
	ActRemPrgRsc,
	ActHidPrgRsc,
	ActUnhPrgRsc,
	ActUp_PrgRsc,
	ActDwnPrgRsc,
	ActFrmChgLnkPrgRsc,
	ActChgLnkPrgRsc,

   // Teaching guide ----------------------------------------------------------

	ActSeeTchGui,
	ActCfgTchGui,
	ActEdiTreGui,		// #2056
	ActSeeTreNodGui,	// #2057
	ActFrmChgTreNodGui,	// #2058
	ActFrmNewTreNodGui,	// #2059
	ActChgTreNodGui,	// #2060
	ActNewTreNodGui,	// #2061
	ActReqRemTreNodGui,	// #2062
	ActRemTreNodGui,	// #2063
	ActHidTreNodGui,	// #2064
	ActUnhTreNodGui,	// #2065
	ActUp_TreNodGui,	// #2066
	ActDwnTreNodGui,	// #2067
	ActLftTreNodGui,	// #2068
	ActRgtTreNodGui,	// #2069
	ActExpSeeTreNodGui,	// #2070
	ActConSeeTreNodGui,	// #2071
	ActExpEdiTreNodGui,	// #2072
	ActConEdiTreNodGui,	// #2073
	ActChgFrcReaTchGui,
	ActChgHavReaTchGui,
	ActSelInfSrcTchGui,
	ActEdiPagTchGui,
	ActRcvPagTchGui,
	ActEdiURLTchGui,
	ActRcvURLTchGui,
	ActEditorTchGui,
	ActEdiPlaTxtTchGui,
	ActRcvPlaTxtTchGui,
	ActEdiRchTxtTchGui,
	ActRcvRchTxtTchGui,
	ActReqLnkTchGui,

   // Syllabus ----------------------------------------------------------------

	ActSeeSyl,
	ActCfgSyl,
	ActEdiTreSyl,
	ActSeeTreNodSyl,
	ActFrmChgTreNodSyl,
	ActFrmNewTreNodSyl,
	ActChgTreNodSyl,
	ActNewTreNodSyl,
	ActReqRemTreNodSyl,
	ActRemTreNodSyl,
	ActHidTreNodSyl,
	ActUnhTreNodSyl,
	ActUp_TreNodSyl,
	ActDwnTreNodSyl,
	ActLftTreNodSyl,
	ActRgtTreNodSyl,
	ActExpSeeTreNodSyl,
	ActConSeeTreNodSyl,
	ActExpEdiTreNodSyl,
	ActConEdiTreNodSyl,
	ActChgFrcReaSyl,
	ActChgHavReaSyl,
	ActSelInfSrcSyl,
	ActEdiPagSyl,
	ActRcvPagSyl,
	ActEdiURLSyl,
	ActRcvURLSyl,
	ActEdiPlaTxtSyl,
	ActRcvPlaTxtSyl,
	ActEdiRchTxtSyl,
	ActRcvRchTxtSyl,
	ActReqLnkSyl,

   // Bibliography ------------------------------------------------------------

	ActSeeBib,
	ActCfgBib,
	ActEdiTreBib,		// #2074
	ActSeeTreNodBib,	// #2075
	ActFrmChgTreNodBib,	// #2076
	ActFrmNewTreNodBib,	// #2077
	ActChgTreNodBib,	// #2078
	ActNewTreNodBib,	// #2079
	ActReqRemTreNodBib,	// #2080
	ActRemTreNodBib,	// #2081
	ActHidTreNodBib,	// #2082
	ActUnhTreNodBib,	// #2083
	ActUp_TreNodBib,	// #2084
	ActDwnTreNodBib,	// #2085
	ActLftTreNodBib,	// #2086
	ActRgtTreNodBib,	// #2087
	ActExpSeeTreNodBib,	// #2088
	ActConSeeTreNodBib,	// #2089
	ActExpEdiTreNodBib,	// #2090
	ActConEdiTreNodBib,	// #2091
	ActChgFrcReaBib,
	ActChgHavReaBib,
	ActSelInfSrcBib,
	ActEdiPagBib,
	ActRcvPagBib,
	ActEdiURLBib,
	ActRcvURLBib,
	ActEditorBib,
	ActEdiPlaTxtBib,
	ActRcvPlaTxtBib,
	ActEdiRchTxtBib,
	ActRcvRchTxtBib,
	ActReqLnkBib,

   // FAQ ---------------------------------------------------------------------

	ActSeeFAQ,
	ActCfgFAQ,
	ActEdiTreFAQ,		// #2092
	ActSeeTreNodFAQ,	// #2093
	ActFrmChgTreNodFAQ,	// #2094
	ActFrmNewTreNodFAQ,	// #2095
	ActChgTreNodFAQ,	// #2096
	ActNewTreNodFAQ,	// #2097
	ActReqRemTreNodFAQ,	// #2098
	ActRemTreNodFAQ,	// #2099
	ActHidTreNodFAQ,	// #2100
	ActUnhTreNodFAQ,	// #2101
	ActUp_TreNodFAQ,	// #2102
	ActDwnTreNodFAQ,	// #2103
	ActLftTreNodFAQ,	// #2104
	ActRgtTreNodFAQ,	// #2105
	ActExpSeeTreNodFAQ,	// #2106
	ActConSeeTreNodFAQ,	// #2107
	ActExpEdiTreNodFAQ,	// #2108
	ActConEdiTreNodFAQ,	// #2109
	ActChgFrcReaFAQ,
	ActChgHavReaFAQ,
	ActSelInfSrcFAQ,
	ActEdiPagFAQ,
	ActRcvPagFAQ,
	ActEdiURLFAQ,
	ActRcvURLFAQ,
	ActEditorFAQ,
	ActEdiPlaTxtFAQ,
	ActRcvPlaTxtFAQ,
	ActEdiRchTxtFAQ,
	ActRcvRchTxtFAQ,
	ActReqLnkFAQ,

   // Links -------------------------------------------------------------------

	ActSeeCrsLnk,
	ActCfgCrsLnk,
	ActEdiTreLnk,		// #2110
	ActSeeTreNodLnk,	// #2111
	ActFrmChgTreNodLnk,	// #2112
	ActFrmNewTreNodLnk,	// #2113
	ActChgTreNodLnk,	// #2114
	ActNewTreNodLnk,	// #2115
	ActReqRemTreNodLnk,	// #2116
	ActRemTreNodLnk,	// #2117
	ActHidTreNodLnk,	// #2118
	ActUnhTreNodLnk,	// #2119
	ActUp_TreNodLnk,	// #2120
	ActDwnTreNodLnk,	// #2121
	ActLftTreNodLnk,	// #2122
	ActRgtTreNodLnk,	// #2123
	ActExpSeeTreNodLnk,	// #2124
	ActConSeeTreNodLnk,	// #2125
	ActExpEdiTreNodLnk,	// #2126
	ActConEdiTreNodLnk,	// #2127
	ActChgFrcReaCrsLnk,
	ActChgHavReaCrsLnk,
	ActSelInfSrcCrsLnk,
	ActEdiPagCrsLnk,
	ActRcvPagCrsLnk,
	ActEdiURLCrsLnk,
	ActRcvURLCrsLnk,
	ActEditorCrsLnk,
	ActEdiPlaTxtCrsLnk,
	ActRcvPlaTxtCrsLnk,
	ActEdiRchTxtCrsLnk,
	ActRcvRchTxtCrsLnk,
	ActReqLnkCrsLnk,

   // Assessment --------------------------------------------------------------

	ActSeeAss,
	ActCfgAss,
	ActEdiTreAss,		// #2128
	ActSeeTreNodAss,	// #2129
	ActFrmChgTreNodAss,	// #2130
	ActFrmNewTreNodAss,	// #2131
	ActChgTreNodAss,	// #2132
	ActNewTreNodAss,	// #2133
	ActReqRemTreNodAss,	// #2134
	ActRemTreNodAss,	// #2135
	ActHidTreNodAss,	// #2136
	ActUnhTreNodAss,	// #2137
	ActUp_TreNodAss,	// #2138
	ActDwnTreNodAss,	// #2139
	ActLftTreNodAss,	// #2140
	ActRgtTreNodAss,	// #2141
	ActExpSeeTreNodAss,	// #2142
	ActConSeeTreNodAss,	// #2143
	ActExpEdiTreNodAss,	// #2144
	ActConEdiTreNodAss,	// #2145
	ActChgFrcReaAss,
	ActChgHavReaAss,
	ActSelInfSrcAss,
	ActEdiPagAss,
	ActRcvPagAss,
	ActEdiURLAss,
	ActRcvURLAss,
	ActEditorAss,
	ActEdiPlaTxtAss,
	ActRcvPlaTxtAss,
	ActEdiRchTxtAss,
	ActRcvRchTxtAss,
	ActReqLnkAss,

   // Timetable ---------------------------------------------------------------

	ActSeeCrsTT,
	ActPrnCrsTT,
	ActEdiCrsTT,
	ActChgCrsTT,
	ActChgCrsTT1stDay,
	ActReqLnkCrsTT,

   /*************************** Assessment tab *******************************/

   // Assignments -------------------------------------------------------------

	ActSeeAllAsg,
	ActFrmNewAsg,
	ActEdiOneAsg,
	ActSeeOneAsg,
	ActPrnOneAsg,
	ActNewAsg,
	ActChgAsg,
	ActReqRemAsg,
	ActRemAsg,
	ActHidAsg,
	ActUnhAsg,
	ActReqLnkAsg,

   // Projects ----------------------------------------------------------------

	ActSeeAllPrj,
	ActCfgPrj,
	ActChgNETCanCrePrj,
	ActChgRubPrj,
	ActReqUsrPrj,
	ActSeeTblAllPrj,
	ActReqLckAllPrj,
	ActReqUnlAllPrj,
	ActLckAllPrj,
	ActUnlAllPrj,
	ActFrmNewPrj,
	ActEdiOnePrj,
	ActSeeOnePrj,
	ActPrnOnePrj,
	ActNewPrj,
	ActChgPrj,
	ActReqRemPrj,
	ActRemPrj,
	ActHidPrj,
	ActUnhPrj,
	ActLckPrj,
	ActUnlPrj,
	ActChgPrjRev,
	ActReqAddStdPrj,
	ActReqAddTutPrj,
	ActReqAddEvlPrj,
	ActAddStdPrj,
	ActAddTutPrj,
	ActAddEvlPrj,
	ActReqRemStdPrj,
	ActReqRemTutPrj,
	ActReqRemEvlPrj,
	ActRemStdPrj,
	ActRemTutPrj,
	ActRemEvlPrj,
	ActChgPrjSco,
	ActReqLnkPrj,
	ActAdmDocPrj,
	ActReqRemFilDocPrj,
	ActRemFilDocPrj,
	ActRemFolDocPrj,
	ActCopDocPrj,
	ActPasDocPrj,
	ActRemTreDocPrj,
	ActFrmCreDocPrj,
	ActCreFolDocPrj,
	ActCreLnkDocPrj,
	ActRenFolDocPrj,
	ActRcvFilDocPrjDZ,
	ActRcvFilDocPrjCla,
	ActExpDocPrj,
	ActConDocPrj,
	ActZIPDocPrj,
	ActReqDatDocPrj,
	ActChgDatDocPrj,
	ActDowDocPrj,
	ActAdmAssPrj,
	ActReqRemFilAssPrj,
	ActRemFilAssPrj,
	ActRemFolAssPrj,
	ActCopAssPrj,
	ActPasAssPrj,
	ActRemTreAssPrj,
	ActFrmCreAssPrj,
	ActCreFolAssPrj,
	ActCreLnkAssPrj,
	ActRenFolAssPrj,
	ActRcvFilAssPrjDZ,
	ActRcvFilAssPrjCla,
	ActExpAssPrj,
	ActConAssPrj,
	ActZIPAssPrj,
	ActReqDatAssPrj,
	ActChgDatAssPrj,
	ActDowAssPrj,

   // Calls for exams ---------------------------------------------------------

	ActSeeAllCfe,
	ActSeeOneCfe,
	ActSeeDatCfe,
	ActEdiCfe,
	ActRcvCfe,
	ActPrnCfe,
	ActReqRemCfe,
	ActRemCfe,
	ActHidCfe,
	ActUnhCfe,
	ActReqLnkCfe,

   // Questions ---------------------------------------------------------------

	ActEdiTstQst,
	ActEdiOneTstQst,
	ActReqImpTstQst,
	ActImpTstQst,
	ActLstTstQst,
	ActNewTstQst,
	ActChgTstQst,
	ActReqRemSevTstQst,
	ActRemSevTstQst,
	ActReqRemOneTstQst,
	ActRemOneTstQst,
	ActChgShfTstQst,
	ActEdiTag,
	ActEnaTag,
	ActDisTag,
	ActRenTag,
	ActReqLnkTag,

   // Test --------------------------------------------------------------------

	ActReqTstAnyTag,
	ActReqTstOneTag,
	ActSeeTst,
	ActReqAssTst,
	ActAssTst,
	ActCfgTst,
	ActRcvCfgTst,
	ActReqSeeMyTstRes,
	ActSeeMyTstResCrs,
	ActSeeOneTstResMe,
	ActReqSeeUsrTstRes,
	ActSeeUsrTstResCrs,
	ActSeeOneTstResOth,
	ActReqLnkTst,

   // Exams -------------------------------------------------------------------

	ActSeeAllExa,
	ActSeeOneExa,
	ActFrmNewExa,
	ActEdiOneExa,
	ActNewExa,
	ActChgExa,
	ActReqRemExa,
	ActRemExa,
	ActHidExa,
	ActUnhExa,
	ActReqLnkExa,
	ActNewExaSet,
	ActReqRemExaSet,
	ActRemExaSet,
	ActUp_ExaSet,
	ActDwnExaSet,
	ActChgTitExaSet,
	ActChgNumQstExaSet,
	ActReqAddQstExaSet,
	ActLstTstQstForSet,
	ActAddQstToExa,
	ActReqRemSetQst,
	ActRemExaQst,
	ActValSetQst,
	ActInvSetQst,
	ActReqNewExaSes,
	ActReqChgExaSes,
	ActNewExaSes,
	ActChgExaSes,
	ActReqRemExaSes,
	ActRemExaSes,
	ActHidExaSes,
	ActUnhExaSes,
	ActSeeExaPrn,
	ActAnsExaPrn,
	ActEndExaPrn,
	ActSeeMyExaResCrs,
	ActSeeMyExaResExa,
	ActSeeMyExaResSes,
	ActSeeOneExaResMe,
	ActReqSeeUsrExaRes,
	ActSeeUsrExaResCrs,
	ActSeeUsrExaResExa,
	ActSeeUsrExaResSes,
	ActSeeOneExaResOth,
	ActChgVisExaRes,

   // Games -------------------------------------------------------------------

	ActSeeAllGam,
	ActSeeOneGam,
	ActReqNewMch,
	ActReqChgMch,
	ActNewMch,
	ActChgMch,
	ActReqRemMch,
	ActRemMch,
	ActResMch,
	ActBckMch,
	ActPlyPauMch,
	ActFwdMch,
	ActChgNumColMch,
	ActChgVisResMchQst,
	ActMchCntDwn,
	ActRefMchTch,
	ActJoiMch,
	ActSeeMchAnsQstStd,
	ActRemMchAnsQstStd,
	ActAnsMchQstStd,
	ActRefMchStd,
	ActSeeMyMchResCrs,
	ActSeeMyMchResGam,
	ActSeeMyMchResMch,
	ActSeeOneMchResMe,
	ActReqSeeUsrMchRes,
	ActSeeUsrMchResCrs,
	ActSeeUsrMchResGam,
	ActSeeUsrMchResMch,
	ActSeeOneMchResOth,
	ActChgVisResMchUsr,
	ActLstOneGam,
	ActFrmNewGam,
	ActEdiOneGam,
	ActNewGam,
	ActChgGam,
	ActReqRemGam,
	ActRemGam,
	ActHidGam,
	ActUnhGam,
	ActAddOneGamQst,
	ActGamLstTstQst,
	ActAddTstQstToGam,
	ActReqRemGamQst,
	ActRemGamQst,
	ActUp_GamQst,
	ActDwnGamQst,
	ActReqLnkGam,

   // Rubrics -----------------------------------------------------------------

	ActSeeAllRub,
	ActSeeOneRub,
	ActFrmNewRub,
	ActEdiOneRub,
	ActNewRub,
	ActChgRub,
	ActReqRemRub,
	ActRemRub,
	ActReqLnkRub,
	ActSeeRscCliRub,
	ActRemRscCliRub,
	ActNewRubCri,
	ActReqRemRubCri,
	ActRemRubCri,
	ActUp_RubCri,
	ActDwnRubCri,
	ActChgTitRubCri,
	ActChgLnkRubCri,
	ActChgMinRubCri,
	ActChgMaxRubCri,
	ActChgWeiRubCri,

   /****************************** Files tab *********************************/

   // Documents of institution ------------------------------------------------

	ActSeeAdmDocIns,
	ActChgToSeeDocIns,
	ActSeeDocIns,
	ActExpSeeDocIns,
	ActConSeeDocIns,
	ActZIPSeeDocIns,
	ActReqDatSeeDocIns,
	ActDowSeeDocIns,
	ActChgToAdmDocIns,
	ActAdmDocIns,
	ActReqRemFilDocIns,
	ActRemFilDocIns,
	ActRemFolDocIns,
	ActCopDocIns,
	ActPasDocIns,
	ActRemTreDocIns,
	ActFrmCreDocIns,
	ActCreFolDocIns,
	ActCreLnkDocIns,
	ActRenFolDocIns,
	ActRcvFilDocInsDZ,
	ActRcvFilDocInsCla,
	ActExpAdmDocIns,
	ActConAdmDocIns,
	ActZIPAdmDocIns,
	ActUnhDocIns,
	ActHidDocIns,
	ActReqDatAdmDocIns,
	ActChgDatAdmDocIns,
	ActDowAdmDocIns,

   // Shared files of institution ---------------------------------------------

	ActAdmShaIns,
	ActReqRemFilShaIns,
	ActRemFilShaIns,
	ActRemFolShaIns,
	ActCopShaIns,
	ActPasShaIns,
	ActRemTreShaIns,
	ActFrmCreShaIns,
	ActCreFolShaIns,
	ActCreLnkShaIns,
	ActRenFolShaIns,
	ActRcvFilShaInsDZ,
	ActRcvFilShaInsCla,
	ActExpShaIns,
	ActConShaIns,
	ActZIPShaIns,
	ActReqDatShaIns,
	ActChgDatShaIns,
	ActDowShaIns,

   // Documents of center -----------------------------------------------------

	ActSeeAdmDocCtr,
	ActChgToSeeDocCtr,
	ActSeeDocCtr,
	ActExpSeeDocCtr,
	ActConSeeDocCtr,
	ActZIPSeeDocCtr,
	ActReqDatSeeDocCtr,
	ActDowSeeDocCtr,
	ActChgToAdmDocCtr,
	ActAdmDocCtr,
	ActReqRemFilDocCtr,
	ActRemFilDocCtr,
	ActRemFolDocCtr,
	ActCopDocCtr,
	ActPasDocCtr,
	ActRemTreDocCtr,
	ActFrmCreDocCtr,
	ActCreFolDocCtr,
	ActCreLnkDocCtr,
	ActRenFolDocCtr,
	ActRcvFilDocCtrDZ,
	ActRcvFilDocCtrCla,
	ActExpAdmDocCtr,
	ActConAdmDocCtr,
	ActZIPAdmDocCtr,
	ActUnhDocCtr,
	ActHidDocCtr,
	ActReqDatAdmDocCtr,
	ActChgDatAdmDocCtr,
	ActDowAdmDocCtr,

   // Shared files of center --------------------------------------------------

	ActAdmShaCtr,
	ActReqRemFilShaCtr,
	ActRemFilShaCtr,
	ActRemFolShaCtr,
	ActCopShaCtr,
	ActPasShaCtr,
	ActRemTreShaCtr,
	ActFrmCreShaCtr,
	ActCreFolShaCtr,
	ActCreLnkShaCtr,
	ActRenFolShaCtr,
	ActRcvFilShaCtrDZ,
	ActRcvFilShaCtrCla,
	ActExpShaCtr,
	ActConShaCtr,
	ActZIPShaCtr,
	ActReqDatShaCtr,
	ActChgDatShaCtr,
	ActDowShaCtr,

   // Documents of degree -----------------------------------------------------

	ActSeeAdmDocDeg,
	ActChgToSeeDocDeg,
	ActSeeDocDeg,
	ActExpSeeDocDeg,
	ActConSeeDocDeg,
	ActZIPSeeDocDeg,
	ActReqDatSeeDocDeg,
	ActDowSeeDocDeg,
	ActChgToAdmDocDeg,
	ActAdmDocDeg,
	ActReqRemFilDocDeg,
	ActRemFilDocDeg,
	ActRemFolDocDeg,
	ActCopDocDeg,
	ActPasDocDeg,
	ActRemTreDocDeg,
	ActFrmCreDocDeg,
	ActCreFolDocDeg,
	ActCreLnkDocDeg,
	ActRenFolDocDeg,
	ActRcvFilDocDegDZ,
	ActRcvFilDocDegCla,
	ActExpAdmDocDeg,
	ActConAdmDocDeg,
	ActZIPAdmDocDeg,
	ActUnhDocDeg,
	ActHidDocDeg,
	ActReqDatAdmDocDeg,
	ActChgDatAdmDocDeg,
	ActDowAdmDocDeg,

   // Shared files of degree --------------------------------------------------

	ActAdmShaDeg,
	ActReqRemFilShaDeg,
	ActRemFilShaDeg,
	ActRemFolShaDeg,
	ActCopShaDeg,
	ActPasShaDeg,
	ActRemTreShaDeg,
	ActFrmCreShaDeg,
	ActCreFolShaDeg,
	ActCreLnkShaDeg,
	ActRenFolShaDeg,
	ActRcvFilShaDegDZ,
	ActRcvFilShaDegCla,
	ActExpShaDeg,
	ActConShaDeg,
	ActZIPShaDeg,
	ActReqDatShaDeg,
	ActChgDatShaDeg,
	ActDowShaDeg,

   // Documents of course -----------------------------------------------------

	ActSeeAdmDocCrsGrp,
	ActChgToSeeDocCrs,
	ActSeeDocCrs,
	ActExpSeeDocCrs,
	ActConSeeDocCrs,
	ActZIPSeeDocCrs,
	ActReqDatSeeDocCrs,
	ActReqLnkSeeDocCrs,
	ActDowSeeDocCrs,
	ActSeeDocGrp,
	ActExpSeeDocGrp,
	ActConSeeDocGrp,
	ActZIPSeeDocGrp,
	ActReqDatSeeDocGrp,
	ActDowSeeDocGrp,
	ActChgToAdmDocCrs,
	ActAdmDocCrs,
	ActReqRemFilDocCrs,
	ActRemFilDocCrs,
	ActRemFolDocCrs,
	ActCopDocCrs,
	ActPasDocCrs,
	ActRemTreDocCrs,
	ActFrmCreDocCrs,
	ActCreFolDocCrs,
	ActCreLnkDocCrs,
	ActRenFolDocCrs,
	ActRcvFilDocCrsDZ,
	ActRcvFilDocCrsCla,
	ActExpAdmDocCrs,
	ActConAdmDocCrs,
	ActZIPAdmDocCrs,
	ActUnhDocCrs,
	ActHidDocCrs,
	ActReqDatAdmDocCrs,
	ActChgDatAdmDocCrs,
	ActReqLnkAdmDocCrs,
	ActDowAdmDocCrs,
	ActAdmDocGrp,
	ActReqRemFilDocGrp,
	ActRemFilDocGrp,
	ActRemFolDocGrp,
	ActCopDocGrp,
	ActPasDocGrp,
	ActRemTreDocGrp,
	ActFrmCreDocGrp,
	ActCreFolDocGrp,
	ActCreLnkDocGrp,
	ActRenFolDocGrp,
	ActRcvFilDocGrpDZ,
	ActRcvFilDocGrpCla,
	ActExpAdmDocGrp,
	ActConAdmDocGrp,
	ActZIPAdmDocGrp,
	ActUnhDocGrp,
	ActHidDocGrp,
	ActReqDatAdmDocGrp,
	ActChgDatAdmDocGrp,
	ActDowAdmDocGrp,

   // Private documents for course teachers -----------------------------------

	ActAdmTchCrsGrp,
	ActChgToAdmTch,
	ActAdmTchCrs,
	ActReqRemFilTchCrs,
	ActRemFilTchCrs,
	ActRemFolTchCrs,
	ActCopTchCrs,
	ActPasTchCrs,
	ActRemTreTchCrs,
	ActFrmCreTchCrs,
	ActCreFolTchCrs,
	ActCreLnkTchCrs,
	ActRenFolTchCrs,
	ActRcvFilTchCrsDZ,
	ActRcvFilTchCrsCla,
	ActExpTchCrs,
	ActConTchCrs,
	ActZIPTchCrs,
	ActReqDatTchCrs,
	ActChgDatTchCrs,
	ActDowTchCrs,
	ActAdmTchGrp,
	ActReqRemFilTchGrp,
	ActRemFilTchGrp,
	ActRemFolTchGrp,
	ActCopTchGrp,
	ActPasTchGrp,
	ActRemTreTchGrp,
	ActFrmCreTchGrp,
	ActCreFolTchGrp,
	ActCreLnkTchGrp,
	ActRenFolTchGrp,
	ActRcvFilTchGrpDZ,
	ActRcvFilTchGrpCla,
	ActExpTchGrp,
	ActConTchGrp,
	ActZIPTchGrp,
	ActReqDatTchGrp,
	ActChgDatTchGrp,
	ActDowTchGrp,

   // Shared files of course --------------------------------------------------

	ActAdmShaCrsGrp,
	ActChgToAdmSha,
	ActAdmShaCrs,
	ActReqRemFilShaCrs,
	ActRemFilShaCrs,
	ActRemFolShaCrs,
	ActCopShaCrs,
	ActPasShaCrs,
	ActRemTreShaCrs,
	ActFrmCreShaCrs,
	ActCreFolShaCrs,
	ActCreLnkShaCrs,
	ActRenFolShaCrs,
	ActRcvFilShaCrsDZ,
	ActRcvFilShaCrsCla,
	ActExpShaCrs,
	ActConShaCrs,
	ActZIPShaCrs,
	ActReqDatShaCrs,
	ActChgDatShaCrs,
	ActDowShaCrs,
	ActAdmShaGrp,
	ActReqRemFilShaGrp,
	ActRemFilShaGrp,
	ActRemFolShaGrp,
	ActCopShaGrp,
	ActPasShaGrp,
	ActRemTreShaGrp,
	ActFrmCreShaGrp,
	ActCreFolShaGrp,
	ActCreLnkShaGrp,
	ActRenFolShaGrp,
	ActRcvFilShaGrpDZ,
	ActRcvFilShaGrpCla,
	ActExpShaGrp,
	ActConShaGrp,
	ActZIPShaGrp,
	ActReqDatShaGrp,
	ActChgDatShaGrp,
	ActDowShaGrp,

   // Assignments and works of user -------------------------------------------

	ActAdmAsgWrkUsr,
	ActReqRemFilAsgUsr,
	ActRemFilAsgUsr,
	ActRemFolAsgUsr,
	ActCopAsgUsr,
	ActPasAsgUsr,
	ActRemTreAsgUsr,
	ActFrmCreAsgUsr,
	ActCreFolAsgUsr,
	ActCreLnkAsgUsr,
	ActRenFolAsgUsr,
	ActRcvFilAsgUsrDZ,
	ActRcvFilAsgUsrCla,
	ActExpAsgUsr,
	ActConAsgUsr,
	ActZIPAsgUsr,
	ActReqDatAsgUsr,
	ActChgDatAsgUsr,
	ActDowAsgUsr,
	ActReqRemFilWrkUsr,
	ActRemFilWrkUsr,
	ActRemFolWrkUsr,
	ActCopWrkUsr,
	ActPasWrkUsr,
	ActRemTreWrkUsr,
	ActFrmCreWrkUsr,
	ActCreFolWrkUsr,
	ActCreLnkWrkUsr,
	ActRenFolWrkUsr,
	ActRcvFilWrkUsrDZ,
	ActRcvFilWrkUsrCla,
	ActExpWrkUsr,
	ActConWrkUsr,
	ActZIPWrkUsr,
	ActReqDatWrkUsr,
	ActChgDatWrkUsr,
	ActDowWrkUsr,

   // Assignments and works of course -----------------------------------------

	ActReqAsgWrkCrs,
	ActAdmAsgWrkCrs,
	ActReqRemFilAsgCrs,
	ActRemFilAsgCrs,
	ActRemFolAsgCrs,
	ActCopAsgCrs,
	ActPasAsgCrs,
	ActRemTreAsgCrs,
	ActFrmCreAsgCrs,
	ActCreFolAsgCrs,
	ActCreLnkAsgCrs,
	ActRenFolAsgCrs,
	ActRcvFilAsgCrsDZ,
	ActRcvFilAsgCrsCla,
	ActExpAsgCrs,
	ActConAsgCrs,
	ActZIPAsgCrs,
	ActReqDatAsgCrs,
	ActChgDatAsgCrs,
	ActDowAsgCrs,
	ActReqRemFilWrkCrs,
	ActRemFilWrkCrs,
	ActRemFolWrkCrs,
	ActCopWrkCrs,
	ActPasWrkCrs,
	ActRemTreWrkCrs,
	ActFrmCreWrkCrs,
	ActCreFolWrkCrs,
	ActCreLnkWrkCrs,
	ActRenFolWrkCrs,
	ActRcvFilWrkCrsDZ,
	ActRcvFilWrkCrsCla,
	ActExpWrkCrs,
	ActConWrkCrs,
	ActZIPWrkCrs,
	ActReqDatWrkCrs,
	ActChgDatWrkCrs,
	ActDowWrkCrs,

   // Marks -------------------------------------------------------------------

	ActSeeAdmMrk,
	ActChgToSeeMrk,
	ActSeeMrkCrs,
	ActExpSeeMrkCrs,
	ActConSeeMrkCrs,
	ActReqDatSeeMrkCrs,
	ActReqLnkSeeMrkCrs,
	ActSeeMyMrkCrs,
	ActSeeMrkGrp,
	ActExpSeeMrkGrp,
	ActConSeeMrkGrp,
	ActReqDatSeeMrkGrp,
	ActSeeMyMrkGrp,
	ActChgToAdmMrk,
	ActAdmMrkCrs,
	ActReqRemFilMrkCrs,
	ActRemFilMrkCrs,
	ActRemFolMrkCrs,
	ActCopMrkCrs,
	ActPasMrkCrs,
	ActRemTreMrkCrs,
	ActFrmCreMrkCrs,
	ActCreFolMrkCrs,
	ActRenFolMrkCrs,
	ActRcvFilMrkCrsDZ,
	ActRcvFilMrkCrsCla,
	ActExpAdmMrkCrs,
	ActConAdmMrkCrs,
	ActZIPAdmMrkCrs,
	ActUnhMrkCrs,
	ActHidMrkCrs,
	ActReqDatAdmMrkCrs,
	ActChgDatAdmMrkCrs,
	ActReqLnkAdmMrkCrs,
	ActDowAdmMrkCrs,
	ActChgNumRowHeaCrs,
	ActChgNumRowFooCrs,
	ActAdmMrkGrp,
	ActReqRemFilMrkGrp,
	ActRemFilMrkGrp,
	ActRemFolMrkGrp,
	ActCopMrkGrp,
	ActPasMrkGrp,
	ActRemTreMrkGrp,
	ActFrmCreMrkGrp,
	ActCreFolMrkGrp,
	ActRenFolMrkGrp,
	ActRcvFilMrkGrpDZ,
	ActRcvFilMrkGrpCla,
	ActExpAdmMrkGrp,
	ActConAdmMrkGrp,
	ActZIPAdmMrkGrp,
	ActUnhMrkGrp,
	ActHidMrkGrp,
	ActReqDatAdmMrkGrp,
	ActChgDatAdmMrkGrp,
	ActDowAdmMrkGrp,
	ActChgNumRowHeaGrp,
	ActChgNumRowFooGrp,

   // Briefcase ---------------------------------------------------------------

	ActAdmBrf,
	ActReqRemFilBrf,
	ActRemFilBrf,
	ActRemFolBrf,
	ActCopBrf,
	ActPasBrf,
	ActRemTreBrf,
	ActFrmCreBrf,
	ActCreFolBrf,
	ActCreLnkBrf,
	ActRenFolBrf,
	ActRcvFilBrfDZ,
	ActRcvFilBrfCla,
	ActExpBrf,
	ActConBrf,
	ActZIPBrf,
	ActReqDatBrf,
	ActChgDatBrf,
	ActDowBrf,
	ActReqRemOldBrf,
	ActRemOldBrf,

   /***************************** Users tab **********************************/

   // Groups ------------------------------------------------------------------

	ActReqSelAllGrp,
	ActReqSelOneGrpTyp,
	ActChgGrp,
	ActReqEdiGrp,
	ActNewGrpTyp,
	ActReqRemGrpTyp,
	ActRemGrpTyp,
	ActRenGrpTyp,
	ActChgMdtGrpTyp,
	ActChgMulGrpTyp,
	ActChgTimGrpTyp,
	ActNewGrp,
	ActReqRemGrp,
	ActRemGrp,
	ActOpeGrp,
	ActCloGrp,
	ActEnaFilZonGrp,
	ActDisFilZonGrp,
	ActChgGrpTyp,
	ActRenGrp,
	ActChgGrpRoo,
	ActChgMaxStdGrp,
	ActReqLnkAllGrp,
	ActReqLnkGrpTyp,

   // Students ----------------------------------------------------------------

	ActLstStd,
	ActLstStdAll,
	ActPrnStdPho,
	Act_DoAct_OnSevStd,
	ActAddClpSevStd,
	ActOwrClpSevStd,
	ActSeeCliStd,
	ActRemClpStd,
	ActSeeRecSevStd,
	ActPrnRecSevStd,
	ActReqMdfOneStd,
	ActReqMdfStd,
	ActReqStdPho,
	ActDetStdPho,
	ActChgStdPho,
	ActReqRemStdPho,
	ActRemStdPho,
	ActCreStd,
	ActUpdStd,
	ActCnfID_Std,
	ActFrmAccStd,
	ActRemNicStd,
	ActChgNicStd,
	ActRemID_Std,
	ActChgID_Std,
	ActChgPwdStd,
	ActRemMaiStd,
	ActChgMaiStd,
	ActRemStdCrs,
	ActReqRemAllStdCrs,
	ActRemAllStdCrs,
	ActReqAccEnrStd,
	ActAccEnrStd,
	ActRemMe_Std,
	ActReqFolSevStd,
	ActReqUnfSevStd,
	ActFolSevStd,
	ActUnfSevStd,
	ActSeeRecOneStd,
	ActReqEnrSevStd,
	ActRcvFrmEnrSevStd,
	ActRcvRecOthUsr,
	ActEdiRecFie,
	ActNewFie,
	ActReqRemFie,
	ActRemFie,
	ActRenFie,
	ActChgRowFie,
	ActChgVisFie,
	ActRcvRecCrs,

   // Teachers ----------------------------------------------------------------

	ActLstTch,
	ActLstTchAll,
	ActPrnTchPho,
	Act_DoAct_OnSevTch,
	ActAddClpSevTch,
	ActOwrClpSevTch,
	ActSeeCliTch,
	ActRemClpTch,
	ActSeeRecSevTch,
	ActPrnRecSevTch,
	ActReqMdfOneTch,
	ActReqMdfNET,
	ActReqMdfTch,
	ActReqTchPho,
	ActDetTchPho,
	ActChgTchPho,
	ActReqRemTchPho,
	ActRemTchPho,
	ActCreNET,
	ActCreTch,
	ActUpdNET,
	ActUpdTch,
	ActCnfID_Tch,
	ActFrmAccTch,
	ActRemNicTch,
	ActChgNicTch,
	ActRemID_Tch,
	ActChgID_Tch,
	ActChgPwdTch,
	ActRemMaiTch,
	ActChgMaiTch,
	ActRemNETCrs,
	ActRemTchCrs,
	ActReqAccEnrNET,
	ActReqAccEnrTch,
	ActAccEnrNET,
	ActAccEnrTch,
	ActRemMe_NET,
	ActRemMe_Tch,
	ActReqFolSevTch,
	ActReqUnfSevTch,
	ActFolSevTch,
	ActUnfSevTch,
	ActSeeRecOneTch,
	ActReqEnrSevNET,
	ActRcvFrmEnrSevNET,
	ActReqEnrSevTch,
	ActRcvFrmEnrSevTch,
	ActFrmLogInUsrAgd,
	ActLogInUsrAgd,
	ActLogInUsrAgdLan,
	ActReqLnkTch,

   // Others ------------------------------------------------------------------

	ActLstOth,
	ActNewAdmIns,
	ActRemAdmIns,
	ActNewAdmCtr,
	ActRemAdmCtr,
	ActNewAdmDeg,
	ActRemAdmDeg,
	ActLstGst,
	ActLstGstAll,
	ActPrnGstPho,
	Act_DoAct_OnSevGst,
	ActAddClpSevGst,
	ActOwrClpSevGst,
	ActSeeCliGst,
	ActRemClpGst,
	ActSeeRecSevGst,
	ActPrnRecSevGst,
	ActReqMdfOneOth,
	ActReqMdfOth,
	ActReqOthPho,
	ActDetOthPho,
	ActChgOthPho,
	ActReqRemOthPho,
	ActRemOthPho,
	ActCreOth,
	ActUpdOth,
	ActCnfID_Oth,
	ActFrmAccOth,
	ActRemNicOth,
	ActChgNicOth,
	ActRemID_Oth,
	ActChgID_Oth,
	ActChgPwdOth,
	ActRemMaiOth,
	ActChgMaiOth,
	ActRemUsrGbl,
	ActReqRemOldUsr,
	ActRemOldUsr,
	ActLstDupUsr,
	ActLstSimUsr,
	ActRemDupUsr,

   // Attendance --------------------------------------------------------------

	ActSeeAllAtt,
	ActReqLstUsrAtt,
	ActSeeLstMyAtt,
	ActPrnLstMyAtt,
	ActSeeLstUsrAtt,
	ActPrnLstUsrAtt,
	ActFrmNewAtt,
	ActEdiOneAtt,
	ActNewAtt,
	ActChgAtt,
	ActReqRemAtt,
	ActRemAtt,
	ActHidAtt,
	ActUnhAtt,
	ActSeeOneAtt,
	ActReqLnkAtt,
	ActRecAttStd,
	ActRecAttMe,

   // Sign up -----------------------------------------------------------------

	ActReqSignUp,
	ActSignUp,

   // Requests ----------------------------------------------------------------

	ActSeeSignUpReq,
	ActUpdSignUpReq,
	ActReqRejSignUp,
	ActRejSignUp,

   // Connected ---------------------------------------------------------------

	ActLstCon,

   /***************************** Messages tab *******************************/

   // Announcements -----------------------------------------------------------

	ActSeeAnn,
	ActWriAnn,
	ActNewAnn,
	ActHidAnn,
	ActUnhAnn,
	ActRemAnn,

   // Notices -----------------------------------------------------------------

	ActSeeAllNot,
	ActSeeOneNot,
	ActWriNot,
	ActNewNot,
	ActHidNot,
	ActUnhNot,
	ActReqRemNot,
	ActRemNot,

   // Forums ------------------------------------------------------------------

	ActSeeFor,
	ActSeeForCrsUsr,
	ActSeeForCrsTch,
	ActSeeForDegUsr,
	ActSeeForDegTch,
	ActSeeForCtrUsr,
	ActSeeForCtrTch,
	ActSeeForInsUsr,
	ActSeeForInsTch,
	ActSeeForGenUsr,
	ActSeeForGenTch,
	ActSeeForSWAUsr,
	ActSeeForSWATch,
	ActSeePstForCrsUsr,
	ActSeePstForCrsTch,
	ActSeePstForDegUsr,
	ActSeePstForDegTch,
	ActSeePstForCtrUsr,
	ActSeePstForCtrTch,
	ActSeePstForInsUsr,
	ActSeePstForInsTch,
	ActSeePstForGenUsr,
	ActSeePstForGenTch,
	ActSeePstForSWAUsr,
	ActSeePstForSWATch,
	ActRcvThrForCrsUsr,
	ActRcvThrForCrsTch,
	ActRcvThrForDegUsr,
	ActRcvThrForDegTch,
	ActRcvThrForCtrUsr,
	ActRcvThrForCtrTch,
	ActRcvThrForInsUsr,
	ActRcvThrForInsTch,
	ActRcvThrForGenUsr,
	ActRcvThrForGenTch,
	ActRcvThrForSWAUsr,
	ActRcvThrForSWATch,
	ActRcvRepForCrsUsr,
	ActRcvRepForCrsTch,
	ActRcvRepForDegUsr,
	ActRcvRepForDegTch,
	ActRcvRepForCtrUsr,
	ActRcvRepForCtrTch,
	ActRcvRepForInsUsr,
	ActRcvRepForInsTch,
	ActRcvRepForGenUsr,
	ActRcvRepForGenTch,
	ActRcvRepForSWAUsr,
	ActRcvRepForSWATch,
	ActReqDelThrCrsUsr,
	ActReqDelThrCrsTch,
	ActReqDelThrDegUsr,
	ActReqDelThrDegTch,
	ActReqDelThrCtrUsr,
	ActReqDelThrCtrTch,
	ActReqDelThrInsUsr,
	ActReqDelThrInsTch,
	ActReqDelThrGenUsr,
	ActReqDelThrGenTch,
	ActReqDelThrSWAUsr,
	ActReqDelThrSWATch,
	ActDelThrForCrsUsr,
	ActDelThrForCrsTch,
	ActDelThrForDegUsr,
	ActDelThrForDegTch,
	ActDelThrForCtrUsr,
	ActDelThrForCtrTch,
	ActDelThrForInsUsr,
	ActDelThrForInsTch,
	ActDelThrForGenUsr,
	ActDelThrForGenTch,
	ActDelThrForSWAUsr,
	ActDelThrForSWATch,
	ActCutThrForCrsUsr,
	ActCutThrForCrsTch,
	ActCutThrForDegUsr,
	ActCutThrForDegTch,
	ActCutThrForCtrUsr,
	ActCutThrForCtrTch,
	ActCutThrForInsUsr,
	ActCutThrForInsTch,
	ActCutThrForGenUsr,
	ActCutThrForGenTch,
	ActCutThrForSWAUsr,
	ActCutThrForSWATch,
	ActPasThrForCrsUsr,
	ActPasThrForCrsTch,
	ActPasThrForDegUsr,
	ActPasThrForDegTch,
	ActPasThrForCtrUsr,
	ActPasThrForCtrTch,
	ActPasThrForInsUsr,
	ActPasThrForInsTch,
	ActPasThrForGenUsr,
	ActPasThrForGenTch,
	ActPasThrForSWAUsr,
	ActPasThrForSWATch,
	ActDelPstForCrsUsr,
	ActDelPstForCrsTch,
	ActDelPstForDegUsr,
	ActDelPstForDegTch,
	ActDelPstForCtrUsr,
	ActDelPstForCtrTch,
	ActDelPstForInsUsr,
	ActDelPstForInsTch,
	ActDelPstForGenUsr,
	ActDelPstForGenTch,
	ActDelPstForSWAUsr,
	ActDelPstForSWATch,
	ActEnbPstForCrsUsr,
	ActEnbPstForCrsTch,
	ActEnbPstForDegUsr,
	ActEnbPstForDegTch,
	ActEnbPstForCtrUsr,
	ActEnbPstForCtrTch,
	ActEnbPstForInsUsr,
	ActEnbPstForInsTch,
	ActEnbPstForGenUsr,
	ActEnbPstForGenTch,
	ActEnbPstForSWAUsr,
	ActEnbPstForSWATch,
	ActDisPstForCrsUsr,
	ActDisPstForCrsTch,
	ActDisPstForDegUsr,
	ActDisPstForDegTch,
	ActDisPstForCtrUsr,
	ActDisPstForCtrTch,
	ActDisPstForInsUsr,
	ActDisPstForInsTch,
	ActDisPstForGenUsr,
	ActDisPstForGenTch,
	ActDisPstForSWAUsr,
	ActDisPstForSWATch,
	ActReqLnkForCrsUsr,

   // Chat --------------------------------------------------------------------

	ActSeeChtRms,
	ActCht,

   // Messages ----------------------------------------------------------------

	ActSeeRcvMsg,
	ActReqMsgUsr,
	ActSeeSntMsg,
	ActRcvMsgUsr,
	ActReqDelAllSntMsg,
	ActReqDelAllRcvMsg,
	ActDelAllSntMsg,
	ActDelAllRcvMsg,
	ActDelSntMsg,
	ActDelRcvMsg,
	ActExpSntMsg,
	ActExpRcvMsg,
	ActConSntMsg,
	ActConRcvMsg,
	ActLstBanUsr,
	ActBanUsrMsg,
	ActUnbUsrMsg,
	ActUnbUsrLst,

   // Mail --------------------------------------------------------------------

	ActReqMaiUsr,
	ActMaiUsr,

   /**************************** Analytics tab *******************************/

   // Figures -----------------------------------------------------------------

	ActReqUseGbl,
	ActSeeUseGbl,

   // Degrees -----------------------------------------------------------------

	ActSeePhoDeg,
	ActPrnPhoDeg,
	ActCalPhoDeg,

   // Indicators --------------------------------------------------------------

	ActReqStaCrs,
	ActSeeAllStaCrs,

   // Surveys -----------------------------------------------------------------

	ActSeeAllSvy,
	ActSeeOneSvy,
	ActAnsSvy,
	ActFrmNewSvy,
	ActEdiOneSvy,
	ActNewSvy,
	ActChgSvy,
	ActReqRemSvy,
	ActRemSvy,
	ActReqRstSvy,
	ActRstSvy,
	ActHidSvy,
	ActUnhSvy,
	ActReqLnkSvy,
	ActEdiOneSvyQst,
	ActNewSvyQst,
	ActChgSvyQst,
	ActReqRemSvyQst,
	ActRemSvyQst,

   // Visits ------------------------------------------------------------------

	ActReqAccGbl,
	ActSeeAccGbl,
	ActReqAccCrs,
	ActSeeAccCrs,
	ActLstClk,
	ActRefLstClk,

   // Report ------------------------------------------------------------------

	ActReqMyUsgRep,
	ActSeeMyUsgRep,

   // Frequent ----------------------------------------------------------------

	ActMFUAct,

   /****************************** Profile tab *******************************/

   // Session -----------------------------------------------------------------

	ActFrmRolSes,
	ActChgMyRol,

   // Courses -----------------------------------------------------------------

	ActMyCrs,

   // Timetable ---------------------------------------------------------------

	ActSeeMyTT,
	ActPrnMyTT,
	ActEdiTut,
	ActChgTut,
	ActChgMyTT1stDay,

   // Agenda ------------------------------------------------------------------

	ActSeeMyAgd,
	ActFrmNewEvtMyAgd,
	ActEdiOneEvtMyAgd,
	ActNewEvtMyAgd,
	ActChgEvtMyAgd,
	ActReqRemEvtMyAgd,
	ActRemEvtMyAgd,
	ActHidEvtMyAgd,
	ActUnhEvtMyAgd,
	ActPrvEvtMyAgd,
	ActPubEvtMyAgd,
	ActPrnAgdQR,

   // Account -----------------------------------------------------------------

	ActFrmMyAcc,
	ActChkUsrAcc,
	ActCreMyAcc,
	ActCreUsrAcc,
	ActRemMyID,
	ActChgMyID,
	ActRemMyNck,
	ActChgMyNck,
	ActRemMyMai,
	ActChgMyMai,
	ActCnfMai,
	ActChgMyPwd,
	ActReqRemMyAcc,
	ActRemMyAcc,
	ActPrnUsrQR,

   // Record ------------------------------------------------------------------

	ActReqEdiRecSha,
	ActChgMyData,
	ActReqMyPho,
	ActDetMyPho,
	ActUpdMyPho,
	ActReqRemMyPho,
	ActRemMyPho,
	ActChgCtyMyIns,
	ActChgMyIns,
	ActChgMyCtr,
	ActChgMyDpt,
	ActChgMyOff,
	ActChgMyOffPho,
	ActReqEdiMyNet,
	ActChgMyNet,

   // Settings ----------------------------------------------------------------

	ActReqEdiSet,
	ActChgThe,
	ActReqChgLan,
	ActChgLan,
	ActChg1stDay,
	ActChgDatFmt,
	ActChgCol,
	ActChgIco,
	ActChgMnu,
	ActChgUsrPho,
	ActChgPriPho,
	ActChgBasPriPrf,
	ActChgExtPriPrf,
	ActChgCooPrf,
	ActChgNtfPrf,

  } Act_Action_t;

//-----------------------------------------------------------------------------

#define ActLst_NUM_ACTIONS		((unsigned) ActChgNtfPrf + 1)

#define ActLst_MAX_ACTION_COD		2145

#define ActLst_DEFAULT_ACTION_AFTER_LOGIN ActSeeGblTL

#endif
