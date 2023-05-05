// swad_action_list.c: action list

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.
    This file is part of SWAD core.
    Copyright (C) 1999-2023 Antonio Cañas Vargas
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
/*********************************** Headers *********************************/
/*****************************************************************************/

#include "swad_account.h"
#include "swad_action.h"
#include "swad_action_list.h"
#include "swad_admin.h"
#include "swad_agenda.h"
#include "swad_announcement.h"
#include "swad_assignment_resource.h"
#include "swad_attendance.h"
#include "swad_attendance_resource.h"
#include "swad_banner.h"
#include "swad_browser_resource.h"
#include "swad_building.h"
#include "swad_calendar.h"
#include "swad_call_for_exam.h"
#include "swad_call_for_exam_resource.h"
#include "swad_center_config.h"
#include "swad_config.h"
#include "swad_cookie.h"
#include "swad_country.h"
#include "swad_country_config.h"
#include "swad_course.h"
#include "swad_course_config.h"
#include "swad_chat.h"
#include "swad_database.h"
#include "swad_date.h"
#include "swad_degree_config.h"
#include "swad_degree_type.h"
#include "swad_department.h"
#include "swad_duplicate.h"
#include "swad_exam.h"
#include "swad_exam_print.h"
#include "swad_exam_resource.h"
#include "swad_exam_result.h"
#include "swad_exam_session.h"
#include "swad_exam_set.h"
#include "swad_enrolment.h"
#include "swad_figure.h"
#include "swad_follow.h"
#include "swad_forum.h"
#include "swad_forum_resource.h"
#include "swad_game.h"
#include "swad_game_resource.h"
#include "swad_global.h"
#include "swad_group_database.h"
#include "swad_hierarchy.h"
#include "swad_holiday.h"
#include "swad_ID.h"
#include "swad_indicator.h"
#include "swad_institution_config.h"
#include "swad_language.h"
#include "swad_link.h"
#include "swad_log.h"
#include "swad_mail.h"
#include "swad_maintenance.h"
#include "swad_mark.h"
#include "swad_match.h"
#include "swad_match_result.h"
#include "swad_message.h"
#include "swad_MFU.h"
#include "swad_network.h"
#include "swad_nickname.h"
#include "swad_notice.h"
#include "swad_notification.h"
#include "swad_parameter.h"
#include "swad_password.h"
#include "swad_photo.h"
#include "swad_place.h"
#include "swad_plugin.h"
#include "swad_privacy.h"
#include "swad_profile.h"
#include "swad_program.h"
#include "swad_program_resource.h"
#include "swad_project.h"
#include "swad_project_config.h"
#include "swad_project_resource.h"
#include "swad_question_import.h"
#include "swad_QR.h"
#include "swad_report.h"
#include "swad_role.h"
#include "swad_rubric.h"
#include "swad_rubric_criteria.h"
#include "swad_rubric_resource.h"
#include "swad_search.h"
#include "swad_session.h"
#include "swad_setting.h"
#include "swad_survey.h"
#include "swad_survey_resource.h"
#include "swad_system_config.h"
#include "swad_tab.h"
#include "swad_tag.h"
#include "swad_timeline.h"
#include "swad_timeline_comment.h"
#include "swad_timeline_favourite.h"
#include "swad_timeline_share.h"
#include "swad_timeline_who.h"
#include "swad_timetable.h"
#include "swad_zip.h"

/*****************************************************************************/
/************************** Public global variables **************************/
/*****************************************************************************/

const struct Act_Actions ActLst_Actions[ActLst_NUM_ACTIONS] =
  {
   // TabUnk ******************************************************************
   // Actions not in menu:
   [ActAll		] = { 645,-1,TabUnk,ActAll		,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,NULL				,NULL},	// Used for statistics
   [ActUnk		] = { 194,-1,TabUnk,ActUnk		,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,NULL				,NULL},
   [ActMnu		] = {   2,-1,TabUnk,ActMnu		,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,NULL				,NULL},
   [ActRefCon		] = { 845,-1,TabUnk,ActRefCon		,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_AJAX_RFRESH,NULL				,Lay_RefreshNotifsAndConnected	,NULL},
   [ActWebSvc		] = { 892,-1,TabUnk,ActWebSvc		,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_WEB_SERVICE,NULL				,API_WebService			,NULL},

   // TabStr ******************************************************************
   [ActFrmLogIn		] = {1521, 0,TabStr,ActFrmLogIn		,    0,0x001,0x001,0x001,0x001,0x001,0x001,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Usr_WriteLandingPage		,"power-off"		},
   [ActReqSch		] = { 627, 1,TabStr,ActReqSch		,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Sch_ReqSysSearch		,"search"		},
   [ActSeeGblTL		] = {1490, 2,TabStr,ActSeeGblTL		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,TmlWho_GetParWho		,Tml_ShowTimelineGbl		,"comment-dots"		},
   [ActSeeSocPrf	] = {1520, 3,TabStr,ActSeeSocPrf	,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Prf_SeeSocialProfiles		,"user-circle"		},
   [ActSeeCal		] = {1622, 4,TabStr,ActSeeCal		,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Cal_ShowCalendar		,"calendar"		},
   [ActSeeNtf         	] = { 990, 5,TabStr,ActSeeNtf		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Ntf_ShowMyNotifications	,"bell"			},

   [ActLogIn		] = {   6,-1,TabUnk,ActFrmLogIn		,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Usr_WelcomeUsr			,NULL},
   [ActLogInNew		] = {1585,-1,TabUnk,ActFrmLogIn		,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Usr_WelcomeUsr			,NULL},
   [ActLogInLan		] = {1077,-1,TabUnk,ActFrmLogIn		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Usr_WelcomeUsr			,NULL},
   [ActAnnSee		] = {1234,-1,TabUnk,ActFrmLogIn		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Ann_MarkAnnouncementAsSeen	,NULL},
   [ActReqSndNewPwd	] = { 665,-1,TabUnk,ActFrmLogIn		,    0,0x001,0x001,0x001,0x001,0x001,0x001,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Pwd_ShowFormSendNewPwd		,NULL},
   [ActSndNewPwd	] = { 633,-1,TabUnk,ActFrmLogIn		,    0,0x001,0x001,0x001,0x001,0x001,0x001,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Pwd_ChkIdLoginAndSendNewPwd	,NULL},
   [ActLogOut		] = {  10,-1,TabUnk,ActFrmLogIn		,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_BRW_1ST_TAB,Ses_CloseSession		,Usr_Logout			,NULL},

   [ActSch		] = { 628,-1,TabUnk,ActReqSch		,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_BRW_1ST_TAB,Sch_GetParsSearch		,Sch_SysSearch			,NULL},

   [ActRefNewPubGblTL	] = {1509,-1,TabUnk,ActSeeGblTL		,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_AJAX_RFRESH,TmlWho_GetParWho		,Tml_RefreshNewTimelineGbl	,NULL},
   [ActRefOldPubGblTL	] = {1510,-1,TabUnk,ActSeeGblTL		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_AJAX_NORMAL,TmlWho_GetParWho		,Tml_RefreshOldTimelineGbl	,NULL},
   [ActRcvPstGblTL	] = {1492,-1,TabUnk,ActSeeGblTL		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_DATA,Act_BRW_1ST_TAB,TmlWho_GetParWho		,TmlPst_ReceivePostGbl		,NULL},
   [ActRcvComGblTL	] = {1503,-1,TabUnk,ActSeeGblTL		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_DATA,Act_BRW_1ST_TAB,TmlWho_GetParWho		,TmlCom_ReceiveCommGbl		,NULL},
   [ActShoHidComGblTL	] = {1806,-1,TabUnk,ActSeeGblTL		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_AJAX_NORMAL,NULL				,TmlCom_ShowHiddenCommsGbl	,NULL},
   [ActAllShaNotGblTL	] = {1766,-1,TabUnk,ActSeeGblTL		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_AJAX_NORMAL,NULL				,TmlSha_ShowAllSharersNoteGbl	,NULL},
   [ActAllFavNotGblTL	] = {1767,-1,TabUnk,ActSeeGblTL		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_AJAX_NORMAL,NULL				,TmlFav_ShowAllFaversNoteGbl	,NULL},
   [ActAllFavComGblTL	] = {1768,-1,TabUnk,ActSeeGblTL		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_AJAX_NORMAL,NULL				,TmlFav_ShowAllFaversComGbl	,NULL},
   [ActShaNotGblTL	] = {1495,-1,TabUnk,ActSeeGblTL		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_AJAX_NORMAL,NULL				,TmlSha_ShaNoteGbl		,NULL},
   [ActUnsNotGblTL	] = {1496,-1,TabUnk,ActSeeGblTL		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_AJAX_NORMAL,NULL				,TmlSha_UnsNoteGbl		,NULL},
   [ActFavNotGblTL	] = {1512,-1,TabUnk,ActSeeGblTL		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_AJAX_NORMAL,NULL				,TmlFav_FavNoteGbl		,NULL},
   [ActUnfNotGblTL	] = {1513,-1,TabUnk,ActSeeGblTL		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_AJAX_NORMAL,NULL				,TmlFav_UnfNoteGbl		,NULL},
   [ActFavComGblTL	] = {1516,-1,TabUnk,ActSeeGblTL		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_AJAX_NORMAL,NULL				,TmlFav_FavCommGbl		,NULL},
   [ActUnfComGblTL	] = {1517,-1,TabUnk,ActSeeGblTL		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_AJAX_NORMAL,NULL				,TmlFav_UnfCommGbl		,NULL},
   [ActReqRemPubGblTL	] = {1494,-1,TabUnk,ActSeeGblTL		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,TmlWho_GetParWho		,TmlNot_ReqRemNoteGbl		,NULL},
   [ActRemPubGblTL	] = {1493,-1,TabUnk,ActSeeGblTL		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,TmlWho_GetParWho		,TmlNot_RemoveNoteGbl		,NULL},
   [ActReqRemComGblTL	] = {1505,-1,TabUnk,ActSeeGblTL		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,TmlWho_GetParWho		,TmlCom_ReqRemComGbl		,NULL},
   [ActRemComGblTL	] = {1507,-1,TabUnk,ActSeeGblTL		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,TmlWho_GetParWho		,TmlCom_RemoveComGbl		,NULL},

   [ActReqOthPubPrf	] = {1401,-1,TabUnk,ActSeeSocPrf	,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Prf_ReqUserProfile		,NULL},

   [ActRefOldPubUsrTL	] = {1511,-1,TabUnk,ActSeeSocPrf	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_AJAX_NORMAL,NULL				,Tml_RefreshOldTimelineUsr	,NULL},
   [ActRcvPstUsrTL	] = {1498,-1,TabUnk,ActSeeSocPrf	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_DATA,Act_BRW_1ST_TAB,NULL				,TmlPst_ReceivePostUsr		,NULL},
   [ActRcvComUsrTL	] = {1504,-1,TabUnk,ActSeeSocPrf	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_DATA,Act_BRW_1ST_TAB,NULL				,TmlCom_ReceiveCommUsr		,NULL},
   [ActShoHidComUsrTL	] = {1807,-1,TabUnk,ActSeeSocPrf	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_AJAX_NORMAL,NULL				,TmlCom_ShowHiddenCommsUsr	,NULL},
   [ActAllShaNotUsrTL	] = {1769,-1,TabUnk,ActSeeSocPrf	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_AJAX_NORMAL,NULL				,TmlSha_ShowAllSharersNoteUsr	,NULL},
   [ActAllFavNotUsrTL	] = {1770,-1,TabUnk,ActSeeSocPrf	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_AJAX_NORMAL,NULL				,TmlFav_ShowAllFaversNoteUsr	,NULL},
   [ActAllFavComUsrTL	] = {1771,-1,TabUnk,ActSeeSocPrf	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_AJAX_NORMAL,NULL				,TmlFav_ShowAllFaversComUsr	,NULL},
   [ActShaNotUsrTL	] = {1499,-1,TabUnk,ActSeeSocPrf	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_AJAX_NORMAL,NULL				,TmlSha_ShaNoteUsr		,NULL},
   [ActUnsNotUsrTL	] = {1500,-1,TabUnk,ActSeeSocPrf	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_AJAX_NORMAL,NULL				,TmlSha_UnsNoteUsr		,NULL},
   [ActFavNotUsrTL	] = {1514,-1,TabUnk,ActSeeSocPrf	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_AJAX_NORMAL,NULL				,TmlFav_FavNoteUsr		,NULL},
   [ActUnfNotUsrTL	] = {1515,-1,TabUnk,ActSeeSocPrf	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_AJAX_NORMAL,NULL				,TmlFav_UnfNoteUsr		,NULL},
   [ActFavComUsrTL	] = {1518,-1,TabUnk,ActSeeSocPrf	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_AJAX_NORMAL,NULL				,TmlFav_FavCommUsr		,NULL},
   [ActUnfComUsrTL	] = {1519,-1,TabUnk,ActSeeSocPrf	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_AJAX_NORMAL,NULL				,TmlFav_UnfCommUsr		,NULL},
   [ActReqRemPubUsrTL	] = {1501,-1,TabUnk,ActSeeSocPrf	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,TmlNot_ReqRemNoteUsr		,NULL},
   [ActRemPubUsrTL	] = {1502,-1,TabUnk,ActSeeSocPrf	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,TmlNot_RemoveNoteUsr		,NULL},
   [ActReqRemComUsrTL	] = {1506,-1,TabUnk,ActSeeSocPrf	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,TmlCom_ReqRemComUsr		,NULL},
   [ActRemComUsrTL	] = {1508,-1,TabUnk,ActSeeSocPrf	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,TmlCom_RemoveComUsr		,NULL},

   [ActSeeOthPubPrf	] = {1402,-1,TabUnk,ActSeeSocPrf	,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Prf_GetUsrDatAndShowUserProfile,NULL},
   [ActCalFig		] = {1405,-1,TabUnk,ActSeeSocPrf	,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Prf_CalculateFigures		,NULL},

   [ActFolUsr		] = {1410,-1,TabUnk,ActSeeSocPrf	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,Fol_FollowUsr1			,Fol_FollowUsr2			,NULL},
   [ActUnfUsr		] = {1411,-1,TabUnk,ActSeeSocPrf	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,Fol_UnfollowUsr1		,Fol_UnfollowUsr2		,NULL},
   [ActSeeFlg		] = {1412,-1,TabUnk,ActSeeSocPrf	,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Fol_ListFollowing		,NULL},
   [ActSeeFlr		] = {1413,-1,TabUnk,ActSeeSocPrf	,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Fol_ListFollowers		,NULL},

   [ActPrnCal		] = {1623,-1,TabUnk,ActSeeCal		,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_BRW_NEW_TAB,NULL				,Cal_PrintCalendar		,NULL},
   [ActChgCal1stDay	] = {1624,-1,TabUnk,ActSeeCal		,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_BRW_1ST_TAB,Cal_ChangeFirstDayOfWeek	,Cal_ShowCalendar		,NULL},

   [ActSeeNewNtf	] = { 991,-1,TabUnk,ActSeeNtf		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Ntf_ShowMyNotifications	,NULL},
   [ActMrkNtfSee	] = {1146,-1,TabUnk,ActSeeNtf		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Ntf_MarkAllNotifAsSeen		,NULL},

   [ActSeeMai		] = { 855,-1,TabUnk,ActSeeNtf		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Mai_SeeMailDomains		,NULL},
   [ActEdiMai		] = { 856,-1,TabUnk,ActSeeNtf		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Mai_EditMailDomains		,NULL},
   [ActNewMai		] = { 857,-1,TabUnk,ActSeeNtf		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,Mai_ReceiveFormNewMailDomain	,Mai_ContEditAfterChgMai	,NULL},
   [ActRemMai		] = { 860,-1,TabUnk,ActSeeNtf		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,Mai_RemoveMailDomain		,Mai_ContEditAfterChgMai	,NULL},
   [ActRenMaiSho	] = { 858,-1,TabUnk,ActSeeNtf		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,Mai_RenameMailDomainShort	,Mai_ContEditAfterChgMai	,NULL},
   [ActRenMaiFul	] = { 859,-1,TabUnk,ActSeeNtf		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,Mai_RenameMailDomainFull	,Mai_ContEditAfterChgMai	,NULL},

   // TabSys ******************************************************************
   // Actions in menu:
   [ActSeeSysInf	] = {1818, 0,TabSys,ActSeeSysInf	,    0,    0,    0,    0,    0,    0,0x3C7,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,SysCfg_ShowConfiguration	,"info"			},
   [ActSeeCty		] = { 862, 1,TabSys,ActSeeCty		,    0,    0,    0,    0,    0,    0,0x3C7,Act_CONT_NORM,Act_BRW_1ST_TAB,Cty_ListCountries1		,Cty_ListCountries2		,"globe-americas"	},
   [ActSeePen		] = {1060, 2,TabSys,ActSeePen		,    0,    0,    0,    0,    0,    0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Hie_SeePending			,"sitemap"		},
   [ActSeeLnk		] = { 748, 3,TabSys,ActSeeLnk		,    0,    0,    0,    0,    0,    0,0x3C7,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Lnk_SeeLinks			,"up-right-from-square"	},
   [ActSeePlg		] = { 777, 4,TabSys,ActSeePlg		,    0,    0,    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Plg_ListPlugins		,"puzzle-piece"		},
   [ActMtn		] = {1820, 5,TabSys,ActMtn		,    0,    0,    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Mtn_Maintenance		,"tools"		},

   // Actions not in menu:
   [ActPrnSysInf	] = {1819,-1,TabUnk,ActSeeSysInf	,    0,    0,    0,    0,    0,    0,0x3C7,Act_CONT_NORM,Act_BRW_NEW_TAB,NULL				,SysCfg_PrintConfiguration	,NULL},

   [ActEdiCty		] = { 863,-1,TabUnk,ActSeeCty		,    0,    0,    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Cty_EditCountries		,NULL},
   [ActNewCty		] = { 864,-1,TabUnk,ActSeeCty		,    0,    0,    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,Cty_ReceiveFormNewCountry	,Cty_ContEditAfterChgCty	,NULL},
   [ActRemCty		] = { 893,-1,TabUnk,ActSeeCty		,    0,    0,    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,Cty_RemoveCountry		,Cty_ContEditAfterChgCty	,NULL},
   [ActRenCty		] = { 866,-1,TabUnk,ActSeeCty		,    0,    0,    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,Cty_RenameCountry		,Cty_ContEditAfterChgCty	,NULL},
   [ActChgCtyWWW	] = {1157,-1,TabUnk,ActSeeCty		,    0,    0,    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,Cty_ChangeCtyWWW		,Cty_ContEditAfterChgCty	,NULL},

   [ActSeeBan		] = {1137,-1,TabUnk,ActSeeLnk		,    0,    0,    0,    0,    0,    0,0x3FF,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Ban_ShowAllBanners		,NULL},
   [ActEdiBan		] = {1138,-1,TabUnk,ActSeeLnk		,    0,    0,    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Ban_EditBanners		,NULL},
   [ActNewBan		] = {1139,-1,TabUnk,ActSeeLnk		,    0,    0,    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,Ban_ReceiveFormNewBanner	,Ban_ContEditAfterChgBan	,NULL},
   [ActRemBan		] = {1140,-1,TabUnk,ActSeeLnk		,    0,    0,    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,Ban_RemoveBanner		,Ban_ContEditAfterChgBan	,NULL},
   [ActUnhBan		] = {1212,-1,TabUnk,ActSeeLnk		,    0,    0,    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,Ban_ShowBanner			,Ban_ContEditAfterChgBan	,NULL},
   [ActHidBan		] = {1213,-1,TabUnk,ActSeeLnk		,    0,    0,    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,Ban_HideBanner			,Ban_ContEditAfterChgBan	,NULL},
   [ActRenBanSho	] = {1141,-1,TabUnk,ActSeeLnk		,    0,    0,    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,Ban_RenameBannerShort		,Ban_ContEditAfterChgBan	,NULL},
   [ActRenBanFul	] = {1142,-1,TabUnk,ActSeeLnk		,    0,    0,    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,Ban_RenameBannerFull		,Ban_ContEditAfterChgBan	,NULL},
   [ActChgBanImg	] = {1144,-1,TabUnk,ActSeeLnk		,    0,    0,    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,Ban_ChangeBannerImg		,Ban_ContEditAfterChgBan	,NULL},
   [ActChgBanWWW	] = {1143,-1,TabUnk,ActSeeLnk		,    0,    0,    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,Ban_ChangeBannerWWW		,Ban_ContEditAfterChgBan	,NULL},
   [ActClkBan		] = {1145,-1,TabUnk,ActSeeLnk		,    0,    0,    0,    0,    0,    0,0x3C7,Act_CONT_NORM,Act_DOWNLD_FILE,Ban_ClickOnBanner		,NULL				,NULL},

   [ActEdiLnk		] = { 749,-1,TabUnk,ActSeeLnk		,    0,    0,    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Lnk_EditLinks			,NULL},
   [ActNewLnk		] = { 750,-1,TabUnk,ActSeeLnk		,    0,    0,    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,Lnk_ReceiveFormNewLink		,Lnk_ContEditAfterChgLnk	,NULL},
   [ActRemLnk		] = { 897,-1,TabUnk,ActSeeLnk		,    0,    0,    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,Lnk_RemoveLink			,Lnk_ContEditAfterChgLnk	,NULL},
   [ActRenLnkSho	] = { 753,-1,TabUnk,ActSeeLnk		,    0,    0,    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,Lnk_RenameLinkShort		,Lnk_ContEditAfterChgLnk	,NULL},
   [ActRenLnkFul	] = { 751,-1,TabUnk,ActSeeLnk		,    0,    0,    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,Lnk_RenameLinkFull		,Lnk_ContEditAfterChgLnk	,NULL},
   [ActChgLnkWWW	] = { 752,-1,TabUnk,ActSeeLnk		,    0,    0,    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,Lnk_ChangeLinkWWW		,Lnk_ContEditAfterChgLnk	,NULL},

   [ActEdiPlg		] = { 778,-1,TabUnk,ActSeePlg		,    0,    0,    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Plg_EditPlugins		,NULL},
   [ActNewPlg		] = { 779,-1,TabUnk,ActSeePlg		,    0,    0,    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,Plg_ReceiveFormNewPlg		,Plg_ContEditAfterChgPlg	,NULL},
   [ActRemPlg		] = { 889,-1,TabUnk,ActSeePlg		,    0,    0,    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,Plg_RemovePlugin		,Plg_ContEditAfterChgPlg	,NULL},
   [ActRenPlg		] = { 782,-1,TabUnk,ActSeePlg		,    0,    0,    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,Plg_RenamePlugin		,Plg_ContEditAfterChgPlg	,NULL},
   [ActChgPlgDes	] = { 888,-1,TabUnk,ActSeePlg		,    0,    0,    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,Plg_ChangePlgDescription	,Plg_ContEditAfterChgPlg	,NULL},
   [ActChgPlgLog	] = { 781,-1,TabUnk,ActSeePlg		,    0,    0,    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,Plg_ChangePlgLogo		,Plg_ContEditAfterChgPlg	,NULL},
   [ActChgPlgAppKey	] = { 986,-1,TabUnk,ActSeePlg		,    0,    0,    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,Plg_ChangePlgAppKey		,Plg_ContEditAfterChgPlg	,NULL},
   [ActChgPlgURL	] = { 783,-1,TabUnk,ActSeePlg		,    0,    0,    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,Plg_ChangePlgURL		,Plg_ContEditAfterChgPlg	,NULL},
   [ActChgPlgIP		] = { 780,-1,TabUnk,ActSeePlg		,    0,    0,    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,Plg_ChangePlgIP		,Plg_ContEditAfterChgPlg	,NULL},

   [ActSetUp		] = { 840,-1,TabUnk,ActMtn		,    0,    0,    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Mtn_SetUp			,NULL},
   [ActReqRemOldCrs	] = {1109,-1,TabUnk,ActMtn		,    0,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Mtn_RemoveOldCrss		,NULL},
   [ActRemOldCrs	] = {1110,-1,TabUnk,ActMtn		,    0,    0,    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Crs_RemoveOldCrss		,NULL},

   // TabCty ******************************************************************
   // Actions in menu:
   [ActSeeCtyInf	] = {1155, 0,TabCty,ActSeeCtyInf	,    0,    0,    0,    0,    0,0x3C7,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,CtyCfg_ShowConfiguration	,"info"			},
   [ActSeeIns		] = { 696, 1,TabCty,ActSeeIns		,    0,    0,    0,    0,    0,0x3C7,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Ins_ShowInssOfCurrentCty	,"university"		},

   // Actions not in menu:
   [ActPrnCtyInf	] = {1156,-1,TabUnk,ActSeeCtyInf	,    0,    0,    0,    0,    0,0x3C7,    0,Act_CONT_NORM,Act_BRW_NEW_TAB,NULL				,CtyCfg_PrintConfiguration	,NULL},
   [ActChgCtyMapAtt	] = {1158,-1,TabUnk,ActSeeCtyInf	,    0,    0,    0,    0,    0,0x200,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,CtyCfg_ChangeCtyMapAttr	,NULL},

   [ActEdiIns		] = { 697,-1,TabUnk,ActSeeIns		,    0,    0,    0,    0,    0,0x3C6,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Ins_EditInstitutions		,NULL},
   [ActReqIns		] = {1210,-1,TabUnk,ActSeeIns		,    0,    0,    0,    0,    0,0x3C6,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Ins_ReceiveFormReqIns		,Ins_ContEditAfterChgIns	,NULL},
   [ActNewIns		] = { 698,-1,TabUnk,ActSeeIns		,    0,    0,    0,    0,    0,0x200,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Ins_ReceiveFormNewIns		,Ins_ContEditAfterChgIns	,NULL},
   [ActRemIns		] = { 759,-1,TabUnk,ActSeeIns		,    0,    0,    0,    0,    0,0x3C6,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Ins_RemoveInstitution		,Ins_ContEditAfterChgIns	,NULL},
   [ActRenInsSho	] = { 702,-1,TabUnk,ActSeeIns		,    0,    0,    0,    0,    0,0x3C6,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Ins_RenameInsShort		,Ins_ContEditAfterChgIns	,NULL},
   [ActRenInsFul	] = { 701,-1,TabUnk,ActSeeIns		,    0,    0,    0,    0,    0,0x3C6,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Ins_RenameInsFull		,Ins_ContEditAfterChgIns	,NULL},
   [ActChgInsWWW	] = { 700,-1,TabUnk,ActSeeIns		,    0,    0,    0,    0,    0,0x3C6,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Ins_ChangeInsWWW		,Ins_ContEditAfterChgIns	,NULL},
   [ActChgInsSta	] = {1211,-1,TabUnk,ActSeeIns		,    0,    0,    0,    0,    0,0x3C6,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Ins_ChangeInsStatus		,Ins_ContEditAfterChgIns	,NULL},

   // TabIns ******************************************************************
   // Actions in menu:
   [ActSeeInsInf	] = {1153, 0,TabIns,ActSeeInsInf	,    0,    0,    0,    0,0x3C7,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,InsCfg_ShowConfiguration	,"info"			},
   [ActSeeCtr		] = { 676, 1,TabIns,ActSeeCtr		,    0,    0,    0,    0,0x3C7,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Ctr_ShowCtrsOfCurrentIns	,"building"		},
   [ActSeePlc		] = { 703, 2,TabIns,ActSeePlc		,    0,    0,    0,    0,0x3C7,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Plc_SeeAllPlaces		,"map-marker-alt"	},
   [ActSeeDpt		] = { 675, 3,TabIns,ActSeeDpt		,    0,    0,    0,    0,0x3C7,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Dpt_SeeAllDepts		,"users"		},
   [ActSeeHld		] = { 707, 4,TabIns,ActSeeHld		,    0,    0,    0,    0,0x3C7,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Hld_SeeAllHolidays		,"calendar-day"		},

   // Actions not in menu:
   [ActPrnInsInf	] = {1154,-1,TabUnk,ActSeeInsInf	,    0,    0,    0,    0,0x3C7,    0,    0,Act_CONT_NORM,Act_BRW_NEW_TAB,NULL				,InsCfg_PrintConfiguration	,NULL},
   [ActChgInsCtyCfg	] = {1590,-1,TabUnk,ActSeeInsInf	,    0,    0,    0,    0,0x200,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,InsCfg_ChangeInsCty		,InsCfg_ContEditAfterChgIns	,NULL},
   [ActRenInsShoCfg	] = {1592,-1,TabUnk,ActSeeInsInf	,    0,    0,    0,    0,0x200,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,InsCfg_RenameInsShort		,InsCfg_ContEditAfterChgIns	,NULL},
   [ActRenInsFulCfg	] = {1591,-1,TabUnk,ActSeeInsInf	,    0,    0,    0,    0,0x200,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,InsCfg_RenameInsFull		,InsCfg_ContEditAfterChgIns	,NULL},
   [ActChgInsWWWCfg	] = {1593,-1,TabUnk,ActSeeInsInf	,    0,    0,    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,InsCfg_ChangeInsWWW		,NULL},
   [ActReqInsLog	] = {1245,-1,TabUnk,ActSeeInsInf	,    0,    0,    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,InsCfg_ReqLogo			,NULL},
   [ActRecInsLog	] = { 699,-1,TabUnk,ActSeeInsInf	,    0,    0,    0,    0,0x300,    0,    0,Act_CONT_DATA,Act_BRW_1ST_TAB,InsCfg_ReceiveLogo		,InsCfg_ShowConfiguration	,NULL},
   [ActRemInsLog	] = {1341,-1,TabUnk,ActSeeInsInf	,    0,    0,    0,    0,0x300,    0,    0,Act_CONT_DATA,Act_BRW_1ST_TAB,InsCfg_RemoveLogo		,InsCfg_ShowConfiguration	,NULL},

   [ActEdiCtr		] = { 681,-1,TabUnk,ActSeeCtr		,    0,    0,    0,    0,0x3C6,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Ctr_EditCenters		,NULL},
   [ActReqCtr		] = {1208,-1,TabUnk,ActSeeCtr		,    0,    0,    0,    0,0x3C6,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Ctr_ReceiveFormReqCtr		,Ctr_ContEditAfterChgCtr	,NULL},
   [ActNewCtr		] = { 685,-1,TabUnk,ActSeeCtr		,    0,    0,    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Ctr_ReceiveFormNewCtr		,Ctr_ContEditAfterChgCtr	,NULL},
   [ActRemCtr		] = { 686,-1,TabUnk,ActSeeCtr		,    0,    0,    0,    0,0x3C6,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Ctr_RemoveCenter		,Ctr_ContEditAfterChgCtr	,NULL},
   [ActChgCtrPlc	] = { 706,-1,TabUnk,ActSeeCtr		,    0,    0,    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Ctr_ChangeCtrPlc		,Ctr_ContEditAfterChgCtr	,NULL},
   [ActRenCtrSho	] = { 682,-1,TabUnk,ActSeeCtr		,    0,    0,    0,    0,0x3C6,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Ctr_RenameCenterShort		,Ctr_ContEditAfterChgCtr	,NULL},
   [ActRenCtrFul	] = { 684,-1,TabUnk,ActSeeCtr		,    0,    0,    0,    0,0x3C6,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Ctr_RenameCenterFull		,Ctr_ContEditAfterChgCtr	,NULL},
   [ActChgCtrWWW	] = { 683,-1,TabUnk,ActSeeCtr		,    0,    0,    0,    0,0x3C6,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Ctr_ChangeCtrWWW		,Ctr_ContEditAfterChgCtr	,NULL},
   [ActChgCtrSta	] = {1209,-1,TabUnk,ActSeeCtr		,    0,    0,    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Ctr_ChangeCtrStatus		,Ctr_ContEditAfterChgCtr	,NULL},

   [ActEdiPlc		] = { 704,-1,TabUnk,ActSeePlc		,    0,    0,    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Plc_EditPlaces			,NULL},
   [ActNewPlc		] = { 705,-1,TabUnk,ActSeePlc		,    0,    0,    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Plc_ReceiveFormNewPlace	,Plc_ContEditAfterChgPlc	,NULL},
   [ActRemPlc		] = { 776,-1,TabUnk,ActSeePlc		,    0,    0,    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Plc_RemovePlace		,Plc_ContEditAfterChgPlc	,NULL},
   [ActRenPlcSho	] = { 894,-1,TabUnk,ActSeePlc		,    0,    0,    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Plc_RenamePlaceShort		,Plc_ContEditAfterChgPlc	,NULL},
   [ActRenPlcFul	] = { 895,-1,TabUnk,ActSeePlc		,    0,    0,    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Plc_RenamePlaceFull		,Plc_ContEditAfterChgPlc	,NULL},

   [ActEdiDpt		] = { 677,-1,TabUnk,ActSeeDpt		,    0,    0,    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Dpt_EditDepartments		,NULL},
   [ActNewDpt		] = { 687,-1,TabUnk,ActSeeDpt		,    0,    0,    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Dpt_ReceiveFormNewDpt		,Dpt_ContEditAfterChgDpt	,NULL},
   [ActRemDpt		] = { 690,-1,TabUnk,ActSeeDpt		,    0,    0,    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Dpt_RemoveDepartment		,Dpt_ContEditAfterChgDpt	,NULL},
   [ActChgDptIns	] = { 721,-1,TabUnk,ActSeeDpt		,    0,    0,    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Dpt_ChangeDepartIns		,Dpt_ContEditAfterChgDpt	,NULL},
   [ActRenDptSho	] = { 688,-1,TabUnk,ActSeeDpt		,    0,    0,    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Dpt_RenameDepartShort		,Dpt_ContEditAfterChgDpt	,NULL},
   [ActRenDptFul	] = { 689,-1,TabUnk,ActSeeDpt		,    0,    0,    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Dpt_RenameDepartFull		,Dpt_ContEditAfterChgDpt	,NULL},
   [ActChgDptWWW	] = { 691,-1,TabUnk,ActSeeDpt		,    0,    0,    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Dpt_ChangeDptWWW		,Dpt_ContEditAfterChgDpt	,NULL},

   [ActEdiHld		] = { 713,-1,TabUnk,ActSeeHld		,    0,    0,    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Hld_EditHolidays		,NULL},
   [ActNewHld		] = { 714,-1,TabUnk,ActSeeHld		,    0,    0,    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Hld_ReceiveFormNewHoliday	,Hld_ContEditAfterChgHld	,NULL},
   [ActRemHld		] = { 716,-1,TabUnk,ActSeeHld		,    0,    0,    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Hld_RemoveHoliday		,Hld_ContEditAfterChgHld	,NULL},
   [ActChgHldPlc	] = { 896,-1,TabUnk,ActSeeHld		,    0,    0,    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Hld_ChangeHolidayPlace		,Hld_ContEditAfterChgHld	,NULL},
   [ActChgHldTyp	] = { 715,-1,TabUnk,ActSeeHld		,    0,    0,    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Hld_ChangeHolidayType		,Hld_ContEditAfterChgHld	,NULL},
   [ActChgHldStrDat	] = { 717,-1,TabUnk,ActSeeHld		,    0,    0,    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Hld_ChangeStartDate		,Hld_ContEditAfterChgHld	,NULL},
   [ActChgHldEndDat	] = { 718,-1,TabUnk,ActSeeHld		,    0,    0,    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Hld_ChangeEndDate		,Hld_ContEditAfterChgHld	,NULL},
   [ActRenHld		] = { 766,-1,TabUnk,ActSeeHld		,    0,    0,    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Hld_RenameHoliday		,Hld_ContEditAfterChgHld	,NULL},

   // TabCtr ******************************************************************
   // Actions in menu:
   [ActSeeCtrInf	] = {1151, 0,TabCtr,ActSeeCtrInf	,    0,    0,    0,0x3C7,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,CtrCfg_ShowConfiguration	,"info"			},
   [ActSeeDeg		] = {1011, 1,TabCtr,ActSeeDeg		,    0,    0,    0,0x3C7,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Deg_ShowDegsOfCurrentCtr	,"graduation-cap"	},
   [ActSeeBld		] = {1838, 2,TabCtr,ActSeeBld		,    0,    0,    0,0x3C7,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Bld_SeeBuildings		,"building"		},
   [ActSeeRoo		] = {1744, 2,TabCtr,ActSeeRoo		,    0,    0,    0,0x3C7,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Roo_SeeRooms			,"chalkboard-teacher"	},

   // Actions not in menu:
   [ActPrnCtrInf	] = {1152,-1,TabUnk,ActSeeCtrInf	,    0,    0,    0,0x3C7,    0,    0,    0,Act_CONT_NORM,Act_BRW_NEW_TAB,NULL				,CtrCfg_PrintConfiguration	,NULL},
   [ActChgCtrInsCfg	] = {1589,-1,TabUnk,ActSeeCtrInf	,    0,    0,    0,0x200,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,CtrCfg_ChangeCtrIns		,CtrCfg_ContEditAfterChgCtr	,NULL},
   [ActRenCtrShoCfg	] = {1595,-1,TabUnk,ActSeeCtrInf	,    0,    0,    0,0x300,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,CtrCfg_RenameCenterShort	,CtrCfg_ContEditAfterChgCtr	,NULL},
   [ActRenCtrFulCfg	] = {1594,-1,TabUnk,ActSeeCtrInf	,    0,    0,    0,0x300,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,CtrCfg_RenameCenterFull	,CtrCfg_ContEditAfterChgCtr	,NULL},
   [ActChgCtrPlcCfg	] = {1648,-1,TabUnk,ActSeeCtrInf	,    0,    0,    0,0x380,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,CtrCfg_ChangeCtrPlc		,NULL},
   [ActChgCtrLatCfg	] = {1815,-1,TabUnk,ActSeeCtrInf	,    0,    0,    0,0x380,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,CtrCfg_ChangeCtrLatitude	,NULL},
   [ActChgCtrLgtCfg	] = {1816,-1,TabUnk,ActSeeCtrInf	,    0,    0,    0,0x380,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,CtrCfg_ChangeCtrLongitude	,NULL},
   [ActChgCtrAltCfg	] = {1817,-1,TabUnk,ActSeeCtrInf	,    0,    0,    0,0x380,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,CtrCfg_ChangeCtrAltitude	,NULL},
   [ActChgCtrWWWCfg	] = {1596,-1,TabUnk,ActSeeCtrInf	,    0,    0,    0,0x380,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,CtrCfg_ChangeCtrWWW		,NULL},
   [ActReqCtrLog	] = {1244,-1,TabUnk,ActSeeCtrInf	,    0,    0,    0,0x380,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,CtrCfg_ReqLogo			,NULL},
   [ActRecCtrLog	] = {1051,-1,TabUnk,ActSeeCtrInf	,    0,    0,    0,0x380,    0,    0,    0,Act_CONT_DATA,Act_BRW_1ST_TAB,CtrCfg_ReceiveLogo		,CtrCfg_ShowConfiguration	,NULL},
   [ActRemCtrLog	] = {1342,-1,TabUnk,ActSeeCtrInf	,    0,    0,    0,0x380,    0,    0,    0,Act_CONT_DATA,Act_BRW_1ST_TAB,CtrCfg_RemoveLogo		,CtrCfg_ShowConfiguration	,NULL},
   [ActReqCtrPho	] = {1160,-1,TabUnk,ActSeeCtrInf	,    0,    0,    0,0x380,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,CtrCfg_ReqPhoto		,NULL},
   [ActRecCtrPho	] = {1161,-1,TabUnk,ActSeeCtrInf	,    0,    0,    0,0x380,    0,    0,    0,Act_CONT_DATA,Act_BRW_1ST_TAB,NULL				,CtrCfg_ReceivePhoto		,NULL},
   [ActChgCtrPhoAtt	] = {1159,-1,TabUnk,ActSeeCtrInf	,    0,    0,    0,0x380,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,CtrCfg_ChangeCtrPhotoAttr	,NULL},

   [ActSeeDegTyp	] = {1013,-1,TabUnk,ActSeeDeg		,    0,    0,    0,0x3C7,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,DegTyp_SeeDegreeTypesInDegTab	,NULL},
   [ActEdiDegTyp	] = { 573,-1,TabUnk,ActSeeDeg		,    0,    0,    0,0x200,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,DegTyp_GetAndEditDegreeTypes	,NULL},
   [ActNewDegTyp	] = { 537,-1,TabUnk,ActSeeDeg		,    0,    0,    0,0x200,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,DegTyp_ReceiveFormNewDegreeType,DegTyp_ContEditAfterChgDegTyp	,NULL},
   [ActRemDegTyp	] = { 545,-1,TabUnk,ActSeeDeg		,    0,    0,    0,0x200,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,DegTyp_RemoveDegreeType	,DegTyp_ContEditAfterChgDegTyp	,NULL},
   [ActRenDegTyp	] = { 538,-1,TabUnk,ActSeeDeg		,    0,    0,    0,0x200,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,DegTyp_RenameDegreeType	,DegTyp_ContEditAfterChgDegTyp	,NULL},

   [ActEdiDeg		] = { 536,-1,TabUnk,ActSeeDeg		,    0,    0,    0,0x3C6,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Deg_EditDegrees		,NULL},
   [ActReqDeg		] = {1206,-1,TabUnk,ActSeeDeg		,    0,    0,    0,0x3C6,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Deg_ReceiveFormReqDeg		,Deg_ContEditAfterChgDeg	,NULL},
   [ActNewDeg		] = { 540,-1,TabUnk,ActSeeDeg		,    0,    0,    0,0x380,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Deg_ReceiveFormNewDeg		,Deg_ContEditAfterChgDeg	,NULL},
   [ActRemDeg		] = { 542,-1,TabUnk,ActSeeDeg		,    0,    0,    0,0x3C6,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Deg_RemoveDegree		,Deg_ContEditAfterChgDeg	,NULL},
   [ActRenDegSho	] = { 546,-1,TabUnk,ActSeeDeg		,    0,    0,    0,0x3C6,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Deg_RenameDegreeShort		,Deg_ContEditAfterChgDeg	,NULL},
   [ActRenDegFul	] = { 547,-1,TabUnk,ActSeeDeg		,    0,    0,    0,0x3C6,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Deg_RenameDegreeFull		,Deg_ContEditAfterChgDeg	,NULL},
   [ActChgDegTyp	] = { 544,-1,TabUnk,ActSeeDeg		,    0,    0,    0,0x3C6,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Deg_ChangeDegreeType		,Deg_ContEditAfterChgDeg	,NULL},
   [ActChgDegWWW	] = { 554,-1,TabUnk,ActSeeDeg		,    0,    0,    0,0x3C6,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Deg_ChangeDegWWW		,Deg_ContEditAfterChgDeg	,NULL},
   [ActChgDegSta	] = {1207,-1,TabUnk,ActSeeDeg		,    0,    0,    0,0x380,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Deg_ChangeDegStatus		,Deg_ContEditAfterChgDeg	,NULL},

   [ActEdiBld		] = {1839,-1,TabUnk,ActSeeBld		,    0,    0,    0,0x3C6,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Bld_EditBuildings		,NULL},
   [ActNewBld		] = {1840,-1,TabUnk,ActSeeBld		,    0,    0,    0,0x3C6,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Bld_ReceiveFormNewBuilding	,Bld_ContEditAfterChgBuilding	,NULL},
   [ActRemBld		] = {1841,-1,TabUnk,ActSeeBld		,    0,    0,    0,0x3C6,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Bld_RemoveBuilding		,Bld_ContEditAfterChgBuilding	,NULL},
   [ActRenBldSho	] = {1842,-1,TabUnk,ActSeeBld		,    0,    0,    0,0x3C6,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Bld_RenameBuildingShort	,Bld_ContEditAfterChgBuilding	,NULL},
   [ActRenBldFul	] = {1843,-1,TabUnk,ActSeeBld		,    0,    0,    0,0x3C6,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Bld_RenameBuildingFull		,Bld_ContEditAfterChgBuilding	,NULL},
   [ActRenBldLoc	] = {1844,-1,TabUnk,ActSeeBld		,    0,    0,    0,0x3C6,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Bld_ChangeBuildingLocation	,Bld_ContEditAfterChgBuilding	,NULL},

   [ActEdiRoo		] = {1745,-1,TabUnk,ActSeeRoo		,    0,    0,    0,0x3C6,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Roo_EditRooms			,NULL},
   [ActNewRoo		] = {1746,-1,TabUnk,ActSeeRoo		,    0,    0,    0,0x3C6,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Roo_ReceiveFormNewRoom		,Roo_ContEditAfterChgRoom	,NULL},
   [ActRemRoo		] = {1747,-1,TabUnk,ActSeeRoo		,    0,    0,    0,0x3C6,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Roo_RemoveRoom			,Roo_ContEditAfterChgRoom	,NULL},
   [ActChgRooBld	] = {1845,-1,TabUnk,ActSeeRoo		,    0,    0,    0,0x3C6,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Roo_ChangeBuilding		,Roo_ContEditAfterChgRoom	,NULL},
   [ActChgRooFlo	] = {1846,-1,TabUnk,ActSeeRoo		,    0,    0,    0,0x3C6,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Roo_ChangeFloor		,Roo_ContEditAfterChgRoom	,NULL},
   [ActChgRooTyp	] = {1847,-1,TabUnk,ActSeeRoo		,    0,    0,    0,0x3C6,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Roo_ChangeType			,Roo_ContEditAfterChgRoom	,NULL},
   [ActRenRooSho	] = {1748,-1,TabUnk,ActSeeRoo		,    0,    0,    0,0x3C6,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Roo_RenameRoomShort		,Roo_ContEditAfterChgRoom	,NULL},
   [ActRenRooFul	] = {1749,-1,TabUnk,ActSeeRoo		,    0,    0,    0,0x3C6,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Roo_RenameRoomFull		,Roo_ContEditAfterChgRoom	,NULL},
   [ActChgRooMaxUsr	] = {1750,-1,TabUnk,ActSeeRoo		,    0,    0,    0,0x3C6,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Roo_ChangeCapacity		,Roo_ContEditAfterChgRoom	,NULL},
   [ActChgRooMAC	] = {1911,-1,TabUnk,ActSeeRoo		,    0,    0,    0,0x3C6,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Roo_ChangeMAC			,Roo_ContEditAfterChgRoom	,NULL},

   // TabDeg ******************************************************************
   // Actions in menu:
   [ActSeeDegInf	] = {1149, 0,TabDeg,ActSeeDegInf	,    0,    0,0x3C7,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,DegCfg_ShowConfiguration	,"info"			},
   [ActSeeCrs		] = {1009, 1,TabDeg,ActSeeCrs		,    0,    0,0x3C7,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Crs_ShowCrssOfCurrentDeg	,"chalkboard-teacher"	},

   // Actions not in menu:
   [ActPrnDegInf	] = {1150,-1,TabUnk,ActSeeDegInf	,    0,    0,0x3C7,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_NEW_TAB,NULL				,DegCfg_PrintConfiguration	,NULL},
   [ActChgDegCtrCfg	] = {1588,-1,TabUnk,ActSeeDegInf	,    0,    0,0x300,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,DegCfg_ChangeDegCtr		,DegCfg_ContEditAfterChgDeg	,NULL},
   [ActRenDegShoCfg	] = {1598,-1,TabUnk,ActSeeDegInf	,    0,    0,0x380,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,DegCfg_RenameDegreeShort	,DegCfg_ContEditAfterChgDeg	,NULL},
   [ActRenDegFulCfg	] = {1597,-1,TabUnk,ActSeeDegInf	,    0,    0,0x380,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,DegCfg_RenameDegreeFull	,DegCfg_ContEditAfterChgDeg	,NULL},
   [ActChgDegWWWCfg	] = {1599,-1,TabUnk,ActSeeDegInf	,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,DegCfg_ChangeDegWWW		,NULL},
   [ActReqDegLog	] = {1246,-1,TabUnk,ActSeeDegInf	,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,DegCfg_ReqLogo			,NULL},
   [ActRecDegLog	] = { 553,-1,TabUnk,ActSeeDegInf	,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_DATA,Act_BRW_1ST_TAB,DegCfg_ReceiveLogo		,DegCfg_ShowConfiguration	,NULL},
   [ActRemDegLog	] = {1343,-1,TabUnk,ActSeeDegInf	,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_DATA,Act_BRW_1ST_TAB,DegCfg_RemoveLogo		,DegCfg_ShowConfiguration	,NULL},

   [ActEdiCrs		] = { 555,-1,TabUnk,ActSeeCrs		,    0,    0,0x3C6,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Crs_EditCourses		,NULL},
   [ActReqCrs		] = {1053,-1,TabUnk,ActSeeCrs		,    0,    0,0x3C6,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Crs_ReceiveFormReqCrs		,Crs_ContEditAfterChgCrs	,NULL},
   [ActNewCrs		] = { 556,-1,TabUnk,ActSeeCrs		,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Crs_ReceiveFormNewCrs		,Crs_ContEditAfterChgCrs	,NULL},
   [ActRemCrs		] = { 560,-1,TabUnk,ActSeeCrs		,    0,    0,0x3C6,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Crs_RemoveCourse		,Crs_ContEditAfterChgCrs	,NULL},
   [ActChgInsCrsCod	] = {1025,-1,TabUnk,ActSeeCrs		,    0,    0,0x3C6,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Crs_ChangeInsCrsCod		,Crs_ContEditAfterChgCrs	,NULL},
   [ActChgCrsYea	] = { 561,-1,TabUnk,ActSeeCrs		,    0,    0,0x3C6,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Crs_ChangeCrsYear		,Crs_ContEditAfterChgCrs	,NULL},
   [ActRenCrsSho	] = { 563,-1,TabUnk,ActSeeCrs		,    0,    0,0x3C6,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Crs_RenameCourseShort		,Crs_ContEditAfterChgCrs	,NULL},
   [ActRenCrsFul	] = { 564,-1,TabUnk,ActSeeCrs		,    0,    0,0x3C6,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Crs_RenameCourseFull		,Crs_ContEditAfterChgCrs	,NULL},
   [ActChgCrsSta	] = {1055,-1,TabUnk,ActSeeCrs		,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Crs_ChangeCrsStatus		,Crs_ContEditAfterChgCrs	,NULL},

   // TabCrs ******************************************************************
   // Actions in menu:
   [ActSeeCrsInf	] = { 847, 0,TabCrs,ActSeeCrsInf	,0x3F8,0x3C7,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Crs_ShowIntroduction		,"info"			},
   [ActSeePrg		] = {1821, 1,TabCrs,ActSeePrg		,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Prg_ShowCourseProgram		,"clipboard-list"	},
   [ActSeeTchGui	] = { 784, 2,TabCrs,ActSeeTchGui	,0x3F8,0x3C7,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_ShowInfo			,"book-open"		},
   [ActSeeSyl		] = {1242, 3,TabCrs,ActSeeSyl		,0x3F8,0x3C7,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_ShowInfo			,"list-ol"		},
   [ActSeeBib		] = {  32, 4,TabCrs,ActSeeBib		,0x3F8,0x3C7,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_ShowInfo			,"book"			},
   [ActSeeFAQ		] = {  54, 5,TabCrs,ActSeeFAQ		,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_ShowInfo			,"question"		},
   [ActSeeCrsLnk	] = {   9, 6,TabCrs,ActSeeCrsLnk	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_ShowInfo			,"up-right-from-square"	},
   [ActSeeAss		] = {  15, 7,TabCrs,ActSeeAss		,0x3F8,0x3C7,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_ShowInfo			,"check"		},
   [ActSeeCrsTT		] = {  25, 8,TabCrs,ActSeeCrsTT		,0x3F8,0x3C7,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Tmt_ShowClassTimeTable		,"clock"		},

   // Actions not in menu:
   [ActPrnCrsInf	] = {1028,-1,TabUnk,ActSeeCrsInf	,0x3F8,0x3C7,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_NEW_TAB,NULL				,CrsCfg_PrintConfiguration	,NULL},
   [ActChgCrsDegCfg	] = {1587,-1,TabUnk,ActSeeCrsInf	,0x380,0x380,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,CrsCfg_ChangeCrsDeg		,CrsCfg_ContEditAfterChgCrs	,NULL},
   [ActRenCrsShoCfg	] = {1601,-1,TabUnk,ActSeeCrsInf	,0x3C0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,CrsCfg_RenameCourseShort	,CrsCfg_ContEditAfterChgCrs	,NULL},
   [ActRenCrsFulCfg	] = {1600,-1,TabUnk,ActSeeCrsInf	,0x3C0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,CrsCfg_RenameCourseFull	,CrsCfg_ContEditAfterChgCrs	,NULL},
   [ActChgInsCrsCodCfg	] = {1024,-1,TabUnk,ActSeeCrsInf	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,CrsCfg_ChangeInsCrsCod		,CrsCfg_ContEditAfterChgCrs	,NULL},
   [ActChgCrsYeaCfg	] = {1573,-1,TabUnk,ActSeeCrsInf	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,CrsCfg_ChangeCrsYear		,CrsCfg_ContEditAfterChgCrs	,NULL},
   [ActEdiCrsInf	] = { 848,-1,TabUnk,ActSeeCrsInf	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_FormsToSelSendInfo		,NULL},

   [ActEdiPrg		] = {1926, 1,TabCrs,ActSeePrg		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Prg_EditCourseProgram		,NULL},
   [ActSeePrgItm	] = {1927,-1,TabUnk,ActSeePrg		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Prg_ViewItemAfterEdit		,NULL},
   [ActFrmChgPrgItm	] = {1823,-1,TabUnk,ActSeePrg		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Prg_ReqChangeItem		,NULL},
   [ActFrmNewPrgItm	] = {1822,-1,TabUnk,ActSeePrg		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Prg_ReqCreateItem		,NULL},
   [ActChgPrgItm	] = {1826,-1,TabUnk,ActSeePrg		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Prg_ReceiveFormChgItem		,NULL},
   [ActNewPrgItm	] = {1825,-1,TabUnk,ActSeePrg		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Prg_ReceiveFormNewItem		,NULL},
   [ActReqRemPrgItm	] = {1827,-1,TabUnk,ActSeePrg		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Prg_ReqRemItem			,NULL},
   [ActRemPrgItm	] = {1828,-1,TabUnk,ActSeePrg		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Prg_RemoveItem			,NULL},
   [ActHidPrgItm	] = {1829,-1,TabUnk,ActSeePrg		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Prg_HideItem			,NULL},
   [ActUnhPrgItm	] = {1830,-1,TabUnk,ActSeePrg		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Prg_UnhideItem			,NULL},
   [ActUp_PrgItm	] = {1831,-1,TabUnk,ActSeePrg		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Prg_MoveUpItem			,NULL},
   [ActDwnPrgItm	] = {1832,-1,TabUnk,ActSeePrg		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Prg_MoveDownItem		,NULL},
   [ActLftPrgItm	] = {1834,-1,TabUnk,ActSeePrg		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Prg_MoveLeftItem		,NULL},
   [ActRgtPrgItm	] = {1833,-1,TabUnk,ActSeePrg		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Prg_MoveRightItem		,NULL},
   [ActExpSeePrgItm	] = {1944,-1,TabUnk,ActSeePrg		,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Prg_ExpandItem			,NULL},
   [ActExpEdiPrgItm	] = {1946,-1,TabUnk,ActSeePrg		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Prg_ExpandItem			,NULL},
   [ActConSeePrgItm	] = {1945,-1,TabUnk,ActSeePrg		,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Prg_ContractItem		,NULL},
   [ActConEdiPrgItm	] = {1947,-1,TabUnk,ActSeePrg		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Prg_ContractItem		,NULL},

   [ActSeeRscCli_InPrg	] = {1970,-1,TabUnk,ActSeePrg		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,PrgRsc_ViewResourceClipboard	,NULL},
   [ActRemRscCli_InPrg	] = {1971,-1,TabUnk,ActSeePrg		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,PrgRsc_RemoveResourceClipboard	,NULL},
   [ActFrmSeePrgRsc	] = {1925,-1,TabUnk,ActSeePrg		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,PrgRsc_ViewResourcesAfterEdit	,NULL},
   [ActFrmEdiPrgRsc	] = {1918,-1,TabUnk,ActSeePrg		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,PrgRsc_EditResources		,NULL},
   [ActNewPrgRsc	] = {1929,-1,TabUnk,ActSeePrg		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,PrgRsc_CreateResource		,NULL},
   [ActRenPrgRsc	] = {1928,-1,TabUnk,ActSeePrg		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,PrgRsc_RenameResource		,NULL},
   [ActReqRemPrgRsc	] = {1919,-1,TabUnk,ActSeePrg		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,PrgRsc_ReqRemResource		,NULL},
   [ActRemPrgRsc	] = {1920,-1,TabUnk,ActSeePrg		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,PrgRsc_RemoveResource		,NULL},
   [ActHidPrgRsc	] = {1921,-1,TabUnk,ActSeePrg		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,PrgRsc_HideResource		,NULL},
   [ActUnhPrgRsc	] = {1922,-1,TabUnk,ActSeePrg		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,PrgRsc_UnhideResource		,NULL},
   [ActUp_PrgRsc	] = {1923,-1,TabUnk,ActSeePrg		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,PrgRsc_MoveUpResource		,NULL},
   [ActDwnPrgRsc	] = {1924,-1,TabUnk,ActSeePrg		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,PrgRsc_MoveDownResource	,NULL},
   [ActFrmChgLnkPrgRsc	] = {1932,-1,TabUnk,ActSeePrg		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,PrgRsc_EditProgramWithClipboard,NULL},
   [ActChgLnkPrgRsc	] = {1933,-1,TabUnk,ActSeePrg		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,PrgRsc_ChangeLink		,NULL},

   [ActEdiTchGui	] = { 785,-1,TabUnk,ActSeeTchGui	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_FormsToSelSendInfo		,NULL},

   [ActSeeSylLec	] = {  28,-1,TabUnk,ActSeeSyl		,0x3F8,0x3C7,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_ShowInfo			,NULL},
   [ActSeeSylPra	] = {  20,-1,TabUnk,ActSeeSyl		,0x3F8,0x3C7,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_ShowInfo			,NULL},
   [ActEdiSylLec	] = {  44,-1,TabUnk,ActSeeSyl		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_FormsToSelSendInfo		,NULL},
   [ActEdiSylPra	] = {  74,-1,TabUnk,ActSeeSyl		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_FormsToSelSendInfo		,NULL},
   [ActDelItmSylLec	] = { 218,-1,TabUnk,ActSeeSyl		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Syl_RemoveItemSyllabus		,NULL},
   [ActDelItmSylPra	] = { 183,-1,TabUnk,ActSeeSyl		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Syl_RemoveItemSyllabus		,NULL},
   [ActUp_IteSylLec	] = { 221,-1,TabUnk,ActSeeSyl		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Syl_UpItemSyllabus		,NULL},
   [ActUp_IteSylPra	] = { 213,-1,TabUnk,ActSeeSyl		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Syl_UpItemSyllabus		,NULL},
   [ActDwnIteSylLec	] = { 220,-1,TabUnk,ActSeeSyl		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Syl_DownItemSyllabus		,NULL},
   [ActDwnIteSylPra	] = { 212,-1,TabUnk,ActSeeSyl		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Syl_DownItemSyllabus		,NULL},
   [ActRgtIteSylLec	] = { 223,-1,TabUnk,ActSeeSyl		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Syl_RightItemSyllabus		,NULL},
   [ActRgtIteSylPra	] = { 215,-1,TabUnk,ActSeeSyl		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Syl_RightItemSyllabus		,NULL},
   [ActLftIteSylLec	] = { 222,-1,TabUnk,ActSeeSyl		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Syl_LeftItemSyllabus		,NULL},
   [ActLftIteSylPra	] = { 214,-1,TabUnk,ActSeeSyl		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Syl_LeftItemSyllabus		,NULL},
   [ActInsIteSylLec	] = { 217,-1,TabUnk,ActSeeSyl		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Syl_InsertItemSyllabus		,NULL},
   [ActInsIteSylPra	] = { 181,-1,TabUnk,ActSeeSyl		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Syl_InsertItemSyllabus		,NULL},
   [ActModIteSylLec	] = { 211,-1,TabUnk,ActSeeSyl		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Syl_ModifyItemSyllabus		,NULL},
   [ActModIteSylPra	] = { 216,-1,TabUnk,ActSeeSyl		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Syl_ModifyItemSyllabus		,NULL},

   [ActEdiBib		] = {  76,-1,TabUnk,ActSeeBib		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_FormsToSelSendInfo		,NULL},
   [ActEdiFAQ		] = { 109,-1,TabUnk,ActSeeFAQ		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_FormsToSelSendInfo		,NULL},
   [ActEdiCrsLnk	] = {  96,-1,TabUnk,ActSeeCrsLnk	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_FormsToSelSendInfo		,NULL},
   [ActEdiAss		] = {  69,-1,TabUnk,ActSeeAss		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_FormsToSelSendInfo		,NULL},

   [ActChgFrcReaCrsInf	] = { 877,-1,TabUnk,ActSeeCrsInf	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_ChangeForceReadInfo	,NULL},
   [ActChgFrcReaTchGui	] = { 870,-1,TabUnk,ActSeeTchGui	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_ChangeForceReadInfo	,NULL},
   [ActChgFrcReaSylLec	] = { 871,-1,TabUnk,ActSeeSyl		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_ChangeForceReadInfo	,NULL},
   [ActChgFrcReaSylPra	] = { 872,-1,TabUnk,ActSeeSyl		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_ChangeForceReadInfo	,NULL},
   [ActChgFrcReaBib	] = { 873,-1,TabUnk,ActSeeBib		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_ChangeForceReadInfo	,NULL},
   [ActChgFrcReaFAQ	] = { 874,-1,TabUnk,ActSeeFAQ		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_ChangeForceReadInfo	,NULL},
   [ActChgFrcReaCrsLnk	] = { 875,-1,TabUnk,ActSeeCrsLnk	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_ChangeForceReadInfo	,NULL},
   [ActChgFrcReaAss	] = { 883,-1,TabUnk,ActSeeAss		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_ChangeForceReadInfo	,NULL},

   [ActChgHavReaCrsInf	] = { 878,-1,TabUnk,ActSeeCrsInf	,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_ChangeIHaveReadInfo	,NULL},
   [ActChgHavReaTchGui	] = { 886,-1,TabUnk,ActSeeTchGui	,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_ChangeIHaveReadInfo	,NULL},
   [ActChgHavReaSylLec	] = { 880,-1,TabUnk,ActSeeSyl		,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_ChangeIHaveReadInfo	,NULL},
   [ActChgHavReaSylPra	] = { 887,-1,TabUnk,ActSeeSyl		,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_ChangeIHaveReadInfo	,NULL},
   [ActChgHavReaBib	] = { 884,-1,TabUnk,ActSeeBib		,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_ChangeIHaveReadInfo	,NULL},
   [ActChgHavReaFAQ	] = { 879,-1,TabUnk,ActSeeFAQ		,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_ChangeIHaveReadInfo	,NULL},
   [ActChgHavReaCrsLnk	] = { 885,-1,TabUnk,ActSeeCrsLnk	,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_ChangeIHaveReadInfo	,NULL},
   [ActChgHavReaAss	] = { 898,-1,TabUnk,ActSeeAss		,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_ChangeIHaveReadInfo	,NULL},

   [ActSelInfSrcCrsInf	] = { 849,-1,TabUnk,ActSeeCrsInf	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_SetInfoSrc			,NULL},
   [ActSelInfSrcTchGui	] = { 789,-1,TabUnk,ActSeeTchGui	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_SetInfoSrc			,NULL},
   [ActSelInfSrcSylLec	] = { 378,-1,TabUnk,ActSeeSyl		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_SetInfoSrc			,NULL},
   [ActSelInfSrcSylPra	] = { 382,-1,TabUnk,ActSeeSyl		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_SetInfoSrc			,NULL},
   [ActSelInfSrcBib	] = { 370,-1,TabUnk,ActSeeBib		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_SetInfoSrc			,NULL},
   [ActSelInfSrcFAQ	] = { 380,-1,TabUnk,ActSeeFAQ		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_SetInfoSrc			,NULL},
   [ActSelInfSrcCrsLnk	] = { 385,-1,TabUnk,ActSeeCrsLnk	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_SetInfoSrc			,NULL},
   [ActSelInfSrcAss	] = { 384,-1,TabUnk,ActSeeAss		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_SetInfoSrc			,NULL},

   [ActRcvURLCrsInf	] = { 854,-1,TabUnk,ActSeeCrsInf	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_ReceiveURLInfo		,NULL},
   [ActRcvURLTchGui	] = { 791,-1,TabUnk,ActSeeTchGui	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_ReceiveURLInfo		,NULL},
   [ActRcvURLSylLec	] = { 403,-1,TabUnk,ActSeeSyl		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_ReceiveURLInfo		,NULL},
   [ActRcvURLSylPra	] = { 402,-1,TabUnk,ActSeeSyl		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_ReceiveURLInfo		,NULL},
   [ActRcvURLBib	] = { 224,-1,TabUnk,ActSeeBib		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_ReceiveURLInfo		,NULL},
   [ActRcvURLFAQ	] = { 234,-1,TabUnk,ActSeeFAQ		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_ReceiveURLInfo		,NULL},
   [ActRcvURLCrsLnk	] = { 182,-1,TabUnk,ActSeeCrsLnk	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_ReceiveURLInfo		,NULL},
   [ActRcvURLAss	] = { 235,-1,TabUnk,ActSeeAss		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_ReceiveURLInfo		,NULL},

   [ActRcvPagCrsInf	] = { 853,-1,TabUnk,ActSeeCrsInf	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_DATA,Act_BRW_1ST_TAB,NULL				,Inf_ReceivePagInfo		,NULL},
   [ActRcvPagTchGui	] = { 788,-1,TabUnk,ActSeeTchGui	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_DATA,Act_BRW_1ST_TAB,NULL				,Inf_ReceivePagInfo		,NULL},
   [ActRcvPagSylLec	] = { 381,-1,TabUnk,ActSeeSyl		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_DATA,Act_BRW_1ST_TAB,NULL				,Inf_ReceivePagInfo		,NULL},
   [ActRcvPagSylPra	] = { 383,-1,TabUnk,ActSeeSyl		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_DATA,Act_BRW_1ST_TAB,NULL				,Inf_ReceivePagInfo		,NULL},
   [ActRcvPagBib	] = { 185,-1,TabUnk,ActSeeBib		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_DATA,Act_BRW_1ST_TAB,NULL				,Inf_ReceivePagInfo		,NULL},
   [ActRcvPagFAQ	] = { 219,-1,TabUnk,ActSeeFAQ		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_DATA,Act_BRW_1ST_TAB,NULL				,Inf_ReceivePagInfo		,NULL},
   [ActRcvPagCrsLnk	] = { 164,-1,TabUnk,ActSeeCrsLnk	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_DATA,Act_BRW_1ST_TAB,NULL				,Inf_ReceivePagInfo		,NULL},
   [ActRcvPagAss	] = { 184,-1,TabUnk,ActSeeAss		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_DATA,Act_BRW_1ST_TAB,NULL				,Inf_ReceivePagInfo		,NULL},

   [ActEditorCrsInf	] = { 852,-1,TabUnk,ActSeeCrsInf	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_EditorCourseInfo		,NULL},
   [ActEditorTchGui	] = { 786,-1,TabUnk,ActSeeTchGui	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_EditorTeachingGuide	,NULL},
   [ActEditorSylLec	] = { 372,-1,TabUnk,ActSeeSyl		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Syl_EditSyllabus		,NULL},
   [ActEditorSylPra	] = { 371,-1,TabUnk,ActSeeSyl		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Syl_EditSyllabus		,NULL},
   [ActEditorBib	] = { 376,-1,TabUnk,ActSeeBib		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_EditorBibliography		,NULL},
   [ActEditorFAQ	] = { 404,-1,TabUnk,ActSeeFAQ		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_EditorFAQ   		,NULL},
   [ActEditorCrsLnk	] = { 388,-1,TabUnk,ActSeeCrsLnk	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_EditorLinks		,NULL},
   [ActEditorAss	] = { 386,-1,TabUnk,ActSeeAss		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_EditorAssessment		,NULL},

   [ActPlaTxtEdiCrsInf	] = { 850,-1,TabUnk,ActSeeCrsInf	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_EditPlainTxtInfo		,NULL},
   [ActPlaTxtEdiTchGui	] = { 787,-1,TabUnk,ActSeeTchGui	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_EditPlainTxtInfo		,NULL},
   [ActPlaTxtEdiSylLec	] = { 379,-1,TabUnk,ActSeeSyl		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_EditPlainTxtInfo		,NULL},
   [ActPlaTxtEdiSylPra	] = { 389,-1,TabUnk,ActSeeSyl		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_EditPlainTxtInfo		,NULL},
   [ActPlaTxtEdiBib	] = { 377,-1,TabUnk,ActSeeBib		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_EditPlainTxtInfo		,NULL},
   [ActPlaTxtEdiFAQ	] = { 405,-1,TabUnk,ActSeeFAQ		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_EditPlainTxtInfo   	,NULL},
   [ActPlaTxtEdiCrsLnk	] = { 400,-1,TabUnk,ActSeeCrsLnk	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_EditPlainTxtInfo		,NULL},
   [ActPlaTxtEdiAss	] = { 387,-1,TabUnk,ActSeeAss		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_EditPlainTxtInfo		,NULL},

   [ActRchTxtEdiCrsInf	] = {1093,-1,TabUnk,ActSeeCrsInf	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_EditRichTxtInfo		,NULL},
   [ActRchTxtEdiTchGui	] = {1094,-1,TabUnk,ActSeeTchGui	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_EditRichTxtInfo		,NULL},
   [ActRchTxtEdiSylLec	] = {1095,-1,TabUnk,ActSeeSyl		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_EditRichTxtInfo		,NULL},
   [ActRchTxtEdiSylPra	] = {1096,-1,TabUnk,ActSeeSyl		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_EditRichTxtInfo		,NULL},
   [ActRchTxtEdiBib	] = {1097,-1,TabUnk,ActSeeBib		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_EditRichTxtInfo		,NULL},
   [ActRchTxtEdiFAQ	] = {1098,-1,TabUnk,ActSeeFAQ		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_EditRichTxtInfo   		,NULL},
   [ActRchTxtEdiCrsLnk	] = {1099,-1,TabUnk,ActSeeCrsLnk	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_EditRichTxtInfo		,NULL},
   [ActRchTxtEdiAss	] = {1100,-1,TabUnk,ActSeeAss		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_EditRichTxtInfo		,NULL},

   [ActRcvPlaTxtCrsInf	] = { 851,-1,TabUnk,ActSeeCrsInf	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_RecAndChangePlainTxtInfo	,NULL},
   [ActRcvPlaTxtTchGui	] = { 790,-1,TabUnk,ActSeeTchGui	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_RecAndChangePlainTxtInfo	,NULL},
   [ActRcvPlaTxtSylLec	] = { 394,-1,TabUnk,ActSeeSyl		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_RecAndChangePlainTxtInfo	,NULL},
   [ActRcvPlaTxtSylPra	] = { 396,-1,TabUnk,ActSeeSyl		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_RecAndChangePlainTxtInfo	,NULL},
   [ActRcvPlaTxtBib	] = { 398,-1,TabUnk,ActSeeBib		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_RecAndChangePlainTxtInfo	,NULL},
   [ActRcvPlaTxtFAQ	] = { 406,-1,TabUnk,ActSeeFAQ		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_RecAndChangePlainTxtInfo	,NULL},
   [ActRcvPlaTxtCrsLnk	] = { 401,-1,TabUnk,ActSeeCrsLnk	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_RecAndChangePlainTxtInfo	,NULL},
   [ActRcvPlaTxtAss	] = { 397,-1,TabUnk,ActSeeAss		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_RecAndChangePlainTxtInfo	,NULL},

   [ActRcvRchTxtCrsInf	] = {1101,-1,TabUnk,ActSeeCrsInf	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_RecAndChangeRichTxtInfo	,NULL},
   [ActRcvRchTxtTchGui	] = {1102,-1,TabUnk,ActSeeTchGui	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_RecAndChangeRichTxtInfo	,NULL},
   [ActRcvRchTxtSylLec	] = {1103,-1,TabUnk,ActSeeSyl		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_RecAndChangeRichTxtInfo	,NULL},
   [ActRcvRchTxtSylPra	] = {1104,-1,TabUnk,ActSeeSyl		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_RecAndChangeRichTxtInfo	,NULL},
   [ActRcvRchTxtBib	] = {1105,-1,TabUnk,ActSeeBib		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_RecAndChangeRichTxtInfo	,NULL},
   [ActRcvRchTxtFAQ	] = {1106,-1,TabUnk,ActSeeFAQ		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_RecAndChangeRichTxtInfo	,NULL},
   [ActRcvRchTxtCrsLnk	] = {1107,-1,TabUnk,ActSeeCrsLnk	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_RecAndChangeRichTxtInfo	,NULL},
   [ActRcvRchTxtAss	] = {1108,-1,TabUnk,ActSeeAss		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Inf_RecAndChangeRichTxtInfo	,NULL},

   [ActPrnCrsTT		] = { 152,-1,TabUnk,ActSeeCrsTT		,0x3F8,0x3C7,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_NEW_TAB,NULL				,Tmt_ShowClassTimeTable		,NULL},
   [ActEdiCrsTT		] = {  45,-1,TabUnk,ActSeeCrsTT		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Tmt_EditCrsTimeTable		,NULL},
   [ActChgCrsTT		] = {  53,-1,TabUnk,ActSeeCrsTT		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Tmt_EditCrsTimeTable		,NULL},
   [ActChgCrsTT1stDay	] = {1486,-1,TabUnk,ActSeeCrsTT		,0x3F8,0x3C7,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Cal_ChangeFirstDayOfWeek	,Tmt_ShowClassTimeTable		,NULL},

   // TabAss ******************************************************************
   // Actions in menu:
   [ActSeeAllAsg	] = { 801, 0,TabAss,ActSeeAllAsg	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Asg_SeeAssignments		,"edit"			},
   [ActSeeAllPrj	] = {1674, 1,TabAss,ActSeeAllPrj	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Prj_SeeAllProjects		,"file-invoice"		},
   [ActSeeAllCfe	] = {  85, 2,TabAss,ActSeeAllCfe	,0x3F8,0x3C7,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Cfe_ListCallsForExamsSee	,"bullhorn"		},
   [ActEdiTstQst	] = { 104, 3,TabAss,ActEdiTstQst	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Dat_SetIniEndDatesToPastAndNow	,Qst_ReqEditQsts		,"clipboard-question"	},
   [ActReqTst		] = { 103, 4,TabAss,ActReqTst		,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Tst_ReqTest			,"check"		},
   [ActSeeAllExa	] = {1848, 5,TabAss,ActSeeAllExa	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Exa_SeeAllExams		,"file-signature"	},
   [ActSeeAllGam	] = {1649, 6,TabAss,ActSeeAllGam	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Gam_SeeAllGames		,"gamepad"		},
   [ActSeeAllRub	] = {1951, 7,TabAss,ActSeeAllRub	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Rub_SeeAllRubrics		,"tasks"		},

   // Actions not in menu:
   [ActFrmNewAsg	] = { 812,-1,TabUnk,ActSeeAllAsg	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Asg_ReqCreatOrEditAsg		,NULL},
   [ActEdiOneAsg	] = { 814,-1,TabUnk,ActSeeAllAsg	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Asg_ReqCreatOrEditAsg		,NULL},
   [ActSeeOneAsg	] = {1942,-1,TabUnk,ActSeeAllAsg	,0x3F8,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Asg_SeeOneAssignment		,NULL},
   [ActPrnOneAsg	] = {1637,-1,TabUnk,ActSeeAllAsg	,0x3F8,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_NEW_TAB,NULL				,Asg_PrintOneAssignment		,NULL},
   [ActNewAsg		] = { 803,-1,TabUnk,ActSeeAllAsg	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Asg_ReceiveFormAssignment	,NULL},
   [ActChgAsg		] = { 815,-1,TabUnk,ActSeeAllAsg	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Asg_ReceiveFormAssignment	,NULL},
   [ActReqRemAsg	] = { 813,-1,TabUnk,ActSeeAllAsg	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Asg_ReqRemAssignment		,NULL},
   [ActRemAsg		] = { 806,-1,TabUnk,ActSeeAllAsg	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Asg_RemoveAssignment		,NULL},
   [ActHidAsg		] = { 964,-1,TabUnk,ActSeeAllAsg	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Asg_HideAssignment		,NULL},
   [ActUnhAsg		] = { 965,-1,TabUnk,ActSeeAllAsg	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Asg_UnhideAssignment		,NULL},
   [ActReqLnkAsg	] = {1943,-1,TabUnk,ActSeeAllAsg	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,AsgRsc_GetLinkToAssignment	,NULL},

   [ActCfgPrj		] = {1803,-1,TabUnk,ActSeeAllPrj	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,PrjCfg_ShowFormConfig		,NULL},
   [ActRcvCfgPrj	] = {1804,-1,TabUnk,ActSeeAllPrj	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,PrjCfg_ReceiveConfig		,NULL},
   [ActReqUsrPrj	] = {1805,-1,TabUnk,ActSeeAllPrj	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Prj_ListUsrsToSelect		,NULL},
   [ActSeeTblAllPrj	] = {1696,-1,TabUnk,ActSeeAllPrj	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_NEW_TAB,NULL				,Prj_ShowTableSelectedPrjs	,NULL},
   [ActReqLckAllPrj	] = {1775,-1,TabUnk,ActSeeAllPrj	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Prj_ReqLockSelectedPrjsEdition	,NULL},
   [ActReqUnlAllPrj	] = {1776,-1,TabUnk,ActSeeAllPrj	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Prj_ReqUnloSelectedPrjsEdition	,NULL},
   [ActLckAllPrj	] = {1777,-1,TabUnk,ActSeeAllPrj	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Prj_LockSelectedPrjsEdition	,NULL},
   [ActUnlAllPrj	] = {1778,-1,TabUnk,ActSeeAllPrj	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Prj_UnloSelectedPrjsEdition	,NULL},
   [ActFrmNewPrj	] = {1675,-1,TabUnk,ActSeeAllPrj	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Prj_ReqCreatePrj		,NULL},
   [ActEdiOnePrj	] = {1676,-1,TabUnk,ActSeeAllPrj	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Prj_ReqEditPrj			,NULL},
   [ActSeeOnePrj	] = {1949,-1,TabUnk,ActSeeAllPrj	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileBrowserOrWorks	,NULL},
   [ActPrnOnePrj	] = {1677,-1,TabUnk,ActSeeAllPrj	,0x3F8,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_NEW_TAB,NULL				,Prj_PrintOneProject		,NULL},
   [ActNewPrj		] = {1678,-1,TabUnk,ActSeeAllPrj	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Prj_ReceiveFormProject		,NULL},
   [ActChgPrj		] = {1679,-1,TabUnk,ActSeeAllPrj	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Prj_ReceiveFormProject		,NULL},
   [ActReqRemPrj	] = {1680,-1,TabUnk,ActSeeAllPrj	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Prj_ReqRemProject		,NULL},
   [ActRemPrj		] = {1681,-1,TabUnk,ActSeeAllPrj	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Prj_RemoveProject		,NULL},
   [ActHidPrj		] = {1682,-1,TabUnk,ActSeeAllPrj	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Prj_HideProject		,NULL},
   [ActUnhPrj		] = {1683,-1,TabUnk,ActSeeAllPrj	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Prj_UnhideProject		,NULL},
   [ActLckPrj		] = {1773,-1,TabUnk,ActSeeAllPrj	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_AJAX_NORMAL,NULL				,Prj_LockProjectEdition		,NULL},
   [ActUnlPrj		] = {1774,-1,TabUnk,ActSeeAllPrj	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_AJAX_NORMAL,NULL				,Prj_UnloProjectEdition		,NULL},
   [ActChgPrjRev	] = {1950,-1,TabUnk,ActSeeAllPrj	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Prj_ChangeReviewStatus		,NULL},
   [ActReqAddStdPrj	] = {1684,-1,TabUnk,ActSeeAllPrj	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Prj_ReqAddStds			,NULL},
   [ActReqAddTutPrj	] = {1685,-1,TabUnk,ActSeeAllPrj	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Prj_ReqAddTuts			,NULL},
   [ActReqAddEvlPrj	] = {1686,-1,TabUnk,ActSeeAllPrj	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Prj_ReqAddEvls			,NULL},
   [ActAddStdPrj	] = {1687,-1,TabUnk,ActSeeAllPrj	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Prj_GetSelectedUsrsAndAddStds	,NULL},
   [ActAddTutPrj	] = {1688,-1,TabUnk,ActSeeAllPrj	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Prj_GetSelectedUsrsAndAddTuts	,NULL},
   [ActAddEvlPrj	] = {1689,-1,TabUnk,ActSeeAllPrj	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Prj_GetSelectedUsrsAndAddEvls	,NULL},
   [ActReqRemStdPrj	] = {1690,-1,TabUnk,ActSeeAllPrj	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Prj_ReqRemStd			,NULL},
   [ActReqRemTutPrj	] = {1691,-1,TabUnk,ActSeeAllPrj	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Prj_ReqRemTut			,NULL},
   [ActReqRemEvlPrj	] = {1692,-1,TabUnk,ActSeeAllPrj	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Prj_ReqRemEvl			,NULL},
   [ActRemStdPrj	] = {1693,-1,TabUnk,ActSeeAllPrj	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Prj_RemStd			,NULL},
   [ActRemTutPrj	] = {1694,-1,TabUnk,ActSeeAllPrj	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Prj_RemTut			,NULL},
   [ActRemEvlPrj	] = {1695,-1,TabUnk,ActSeeAllPrj	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Prj_RemEvl			,NULL},
   [ActChgPrjSco	] = {1974,-1,TabUnk,ActSeeAllPrj	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Prj_ChangeCriterionScore	,NULL},
   [ActReqLnkPrj	] = {1948,-1,TabUnk,ActSeeAllPrj	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,PrjRsc_GetLinkToProject	,NULL},

   [ActAdmDocPrj	] = {1697,-1,TabUnk,ActSeeAllPrj	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileBrowserOrWorks	,NULL},
   [ActReqRemFilDocPrj	] = {1698,-1,TabUnk,ActSeeAllPrj	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_AskRemFileFromTree		,NULL},
   [ActRemFilDocPrj	] = {1699,-1,TabUnk,ActSeeAllPrj	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RemFileFromTree		,NULL},
   [ActRemFolDocPrj	] = {1700,-1,TabUnk,ActSeeAllPrj	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RemFolderFromTree		,NULL},
   [ActCopDocPrj	] = {1701,-1,TabUnk,ActSeeAllPrj	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_CopyFromFileBrowser	,NULL},
   [ActPasDocPrj	] = {1702,-1,TabUnk,ActSeeAllPrj	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_PasteIntoFileBrowser	,NULL},
   [ActRemTreDocPrj	] = {1703,-1,TabUnk,ActSeeAllPrj	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RemSubtreeInFileBrowser	,NULL},
   [ActFrmCreDocPrj	] = {1704,-1,TabUnk,ActSeeAllPrj	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFormFileBrowser	,NULL},
   [ActCreFolDocPrj	] = {1705,-1,TabUnk,ActSeeAllPrj	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RecFolderFileBrowser	,NULL},
   [ActCreLnkDocPrj	] = {1706,-1,TabUnk,ActSeeAllPrj	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RecLinkFileBrowser		,NULL},
   [ActRenFolDocPrj	] = {1707,-1,TabUnk,ActSeeAllPrj	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RenFolderFileBrowser	,NULL},
   [ActRcvFilDocPrjDZ	] = {1708,-1,TabUnk,ActSeeAllPrj	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_DATA,Act_UPLOAD_FILE,Brw_RcvFileInFileBrwDropzone	,NULL				,NULL},
   [ActRcvFilDocPrjCla	] = {1709,-1,TabUnk,ActSeeAllPrj	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_DATA,Act_BRW_1ST_TAB,NULL				,Brw_RcvFileInFileBrwClassic	,NULL},
   [ActExpDocPrj	] = {1710,-1,TabUnk,ActSeeAllPrj	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_204_NO_CONT,Brw_ExpandFileTree		,NULL				,NULL},
   [ActConDocPrj	] = {1711,-1,TabUnk,ActSeeAllPrj	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_204_NO_CONT,Brw_ContractFileTree		,NULL				,NULL},
   [ActZIPDocPrj	] = {1712,-1,TabUnk,ActSeeAllPrj	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,ZIP_CompressFileTree		,NULL},
   [ActReqDatDocPrj	] = {1713,-1,TabUnk,ActSeeAllPrj	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileMetadata		,NULL},
   [ActChgDatDocPrj	] = {1714,-1,TabUnk,ActSeeAllPrj	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ChgFileMetadata		,NULL},
   [ActDowDocPrj	] = {1715,-1,TabUnk,ActSeeAllPrj	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_DOWNLD_FILE,Brw_DownloadFile		,NULL				,NULL},

   [ActAdmAssPrj	] = {1716,-1,TabUnk,ActSeeAllPrj	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileBrowserOrWorks	,NULL},
   [ActReqRemFilAssPrj	] = {1717,-1,TabUnk,ActSeeAllPrj	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_AskRemFileFromTree		,NULL},
   [ActRemFilAssPrj	] = {1718,-1,TabUnk,ActSeeAllPrj	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RemFileFromTree		,NULL},
   [ActRemFolAssPrj	] = {1719,-1,TabUnk,ActSeeAllPrj	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RemFolderFromTree		,NULL},
   [ActCopAssPrj	] = {1720,-1,TabUnk,ActSeeAllPrj	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_CopyFromFileBrowser	,NULL},
   [ActPasAssPrj	] = {1721,-1,TabUnk,ActSeeAllPrj	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_PasteIntoFileBrowser	,NULL},
   [ActRemTreAssPrj	] = {1722,-1,TabUnk,ActSeeAllPrj	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RemSubtreeInFileBrowser	,NULL},
   [ActFrmCreAssPrj	] = {1723,-1,TabUnk,ActSeeAllPrj	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFormFileBrowser	,NULL},
   [ActCreFolAssPrj	] = {1724,-1,TabUnk,ActSeeAllPrj	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RecFolderFileBrowser	,NULL},
   [ActCreLnkAssPrj	] = {1725,-1,TabUnk,ActSeeAllPrj	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RecLinkFileBrowser		,NULL},
   [ActRenFolAssPrj	] = {1726,-1,TabUnk,ActSeeAllPrj	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RenFolderFileBrowser	,NULL},
   [ActRcvFilAssPrjDZ	] = {1727,-1,TabUnk,ActSeeAllPrj	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_DATA,Act_UPLOAD_FILE,Brw_RcvFileInFileBrwDropzone	,NULL				,NULL},
   [ActRcvFilAssPrjCla	] = {1728,-1,TabUnk,ActSeeAllPrj	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_DATA,Act_BRW_1ST_TAB,NULL				,Brw_RcvFileInFileBrwClassic	,NULL},
   [ActExpAssPrj	] = {1729,-1,TabUnk,ActSeeAllPrj	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_204_NO_CONT,Brw_ExpandFileTree		,NULL				,NULL},
   [ActConAssPrj	] = {1730,-1,TabUnk,ActSeeAllPrj	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_204_NO_CONT,Brw_ContractFileTree		,NULL				,NULL},
   [ActZIPAssPrj	] = {1731,-1,TabUnk,ActSeeAllPrj	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,ZIP_CompressFileTree		,NULL},
   [ActReqDatAssPrj	] = {1732,-1,TabUnk,ActSeeAllPrj	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileMetadata		,NULL},
   [ActChgDatAssPrj	] = {1733,-1,TabUnk,ActSeeAllPrj	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ChgFileMetadata		,NULL},
   [ActDowAssPrj	] = {1734,-1,TabUnk,ActSeeAllPrj	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_DOWNLD_FILE,Brw_DownloadFile		,NULL				,NULL},

   [ActSeeOneCfe	] = {1572,-1,TabUnk,ActSeeAllCfe	,0x3F8,0x3C7,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Cfe_ListCallsForExamsCod	,NULL},
   [ActSeeDatCfe	] = {1571,-1,TabUnk,ActSeeAllCfe	,0x3F8,0x3C7,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Cfe_ListCallsForExamsDay	,NULL},
   [ActEdiCfe		] = {  91,-1,TabUnk,ActSeeAllCfe	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Cfe_PutFrmEditACallForExam	,NULL},
   [ActRcvCfe		] = { 110,-1,TabUnk,ActSeeAllCfe	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Cfe_ReceiveCallForExam1	,Cfe_ReceiveCallForExam2	,NULL},
   [ActPrnCfe		] = { 179,-1,TabUnk,ActSeeAllCfe	,0x3F8,0x3C7,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_NEW_TAB,NULL				,Cfe_PrintCallForExam		,NULL},
   [ActReqRemCfe	] = {1619,-1,TabUnk,ActSeeAllCfe	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Cfe_ReqRemCallForExam		,NULL},
   [ActRemCfe		] = { 187,-1,TabUnk,ActSeeAllCfe	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Cfe_RemoveCallForExam1		,Cfe_RemoveCallForExam2		,NULL},
   [ActHidCfe		] = {1620,-1,TabUnk,ActSeeAllCfe	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Cfe_HideCallForExam		,Cfe_ListCallsForExamsEdit	,NULL},
   [ActUnhCfe		] = {1621,-1,TabUnk,ActSeeAllCfe	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Cfe_UnhideCallForExam		,Cfe_ListCallsForExamsEdit	,NULL},
   [ActReqLnkCfe	] = {1934,-1,TabUnk,ActSeeAllCfe	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,CfeRsc_GetLinkToCallForExam	,NULL},

   [ActEdiOneTstQst	] = { 105,-1,TabUnk,ActEdiTstQst	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Qst_ShowFormEditOneQst		,NULL},
   [ActReqImpTstQst	] = {1007,-1,TabUnk,ActEdiTstQst	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,QstImp_ShowFormImpQstsFromXML	,NULL},
   [ActImpTstQst	] = {1008,-1,TabUnk,ActEdiTstQst	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_DATA,Act_BRW_1ST_TAB,NULL				,QstImp_ImpQstsFromXML		,NULL},
   [ActLstTstQst	] = { 132,-1,TabUnk,ActEdiTstQst	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Qst_ListQuestionsToEdit	,NULL},
   [ActRcvTstQst	] = { 126,-1,TabUnk,ActEdiTstQst	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_DATA,Act_BRW_1ST_TAB,NULL				,Qst_ReceiveQst			,NULL},
   [ActReqRemSevTstQst	] = {1835,-1,TabUnk,ActEdiTstQst	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Qst_ReqRemSelectedQsts		,NULL},
   [ActRemSevTstQst	] = {1836,-1,TabUnk,ActEdiTstQst	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Qst_RemoveSelectedQsts		,NULL},
   [ActReqRemOneTstQst	] = {1523,-1,TabUnk,ActEdiTstQst	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Qst_ReqRemOneQst		,NULL},
   [ActRemOneTstQst	] = { 133,-1,TabUnk,ActEdiTstQst	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Qst_RemoveOneQst		,NULL},
   [ActChgShfTstQst	] = { 455,-1,TabUnk,ActEdiTstQst	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Qst_ChangeShuffleQst		,NULL},

   [ActEdiTag		] = {1907,-1,TabUnk,ActEdiTstQst	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Tag_ShowFormEditTags		,NULL},
   [ActEnaTag		] = { 453,-1,TabUnk,ActEdiTstQst	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Tag_EnableTag			,NULL},
   [ActDisTag		] = { 452,-1,TabUnk,ActEdiTstQst	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Tag_DisableTag			,NULL},
   [ActRenTag		] = { 143,-1,TabUnk,ActEdiTstQst	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Tag_RenameTag			,NULL},

   [ActSeeTst		] = {  29,-1,TabUnk,ActReqTst		,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Tst_ShowNewTest		,NULL},
   [ActReqAssTst	] = {1837,-1,TabUnk,ActReqTst		,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Tst_ReceiveTestDraft		,NULL},
   [ActAssTst		] = {  98,-1,TabUnk,ActReqTst		,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Tst_AssessTest			,NULL},

   [ActCfgTst		] = { 451,-1,TabUnk,ActReqTst		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,TstCfg_CheckAndShowFormConfig	,NULL},
   [ActRcvCfgTst	] = { 454,-1,TabUnk,ActReqTst		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,TstCfg_ReceiveConfigTst	,NULL},

   [ActReqSeeMyTstRes	] = {1083,-1,TabUnk,ActReqTst		,0x208,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Dat_SetIniEndDatesToPastAndNow	,TstPrn_SelDatesToSeeMyPrints	,NULL},
   [ActSeeMyTstResCrs	] = {1084,-1,TabUnk,ActReqTst		,0x208,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,TstPrn_ShowMyPrints		,NULL},
   [ActSeeOneTstResMe	] = {1085,-1,TabUnk,ActReqTst		,0x208,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,TstPrn_ShowOnePrint		,NULL},
   [ActReqSeeUsrTstRes	] = {1080,-1,TabUnk,ActReqTst		,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Dat_SetIniEndDatesToPastAndNow	,TstPrn_SelUsrsToViewUsrsPrints	,NULL},
   [ActSeeUsrTstResCrs	] = {1081,-1,TabUnk,ActReqTst		,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,TstPrn_GetUsrsAndShowPrints	,NULL},
   [ActSeeOneTstResOth	] = {1082,-1,TabUnk,ActReqTst		,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,TstPrn_ShowOnePrint		,NULL},

   [ActSeeOneExa	] = {1849,-1,TabUnk,ActSeeAllExa	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Exa_SeeOneExam			,NULL},

   [ActFrmNewExa	] = {1877,-1,TabUnk,ActSeeAllExa	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Exa_ReqCreatOrEditExam		,NULL},
   [ActEdiOneExa	] = {1878,-1,TabUnk,ActSeeAllExa	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Exa_ReqCreatOrEditExam		,NULL},
   [ActNewExa		] = {1879,-1,TabUnk,ActSeeAllExa	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Exa_ReceiveFormExam		,NULL},
   [ActChgExa		] = {1880,-1,TabUnk,ActSeeAllExa	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Exa_ReceiveFormExam		,NULL},
   [ActReqRemExa	] = {1881,-1,TabUnk,ActSeeAllExa	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Exa_AskRemExam			,NULL},
   [ActRemExa		] = {1882,-1,TabUnk,ActSeeAllExa	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Exa_RemoveExam			,NULL},
   [ActHidExa		] = {1883,-1,TabUnk,ActSeeAllExa	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Exa_HideExam			,NULL},
   [ActUnhExa		] = {1884,-1,TabUnk,ActSeeAllExa	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Exa_UnhideExam			,NULL},
   [ActReqLnkExa	] = {1936,-1,TabUnk,ActSeeAllExa	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,ExaRsc_GetLinkToExam		,NULL},

   [ActNewExaSet	] = {1898,-1,TabUnk,ActSeeAllExa	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,ExaSet_ReceiveFormSet		,NULL},
   [ActReqRemExaSet	] = {1893,-1,TabUnk,ActSeeAllExa	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,ExaSet_ReqRemSet		,NULL},
   [ActRemExaSet	] = {1894,-1,TabUnk,ActSeeAllExa	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,ExaSet_RemoveSet		,NULL},
   [ActUp_ExaSet	] = {1895,-1,TabUnk,ActSeeAllExa	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,ExaSet_MoveUpSet		,NULL},
   [ActDwnExaSet	] = {1896,-1,TabUnk,ActSeeAllExa	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,ExaSet_MoveDownSet		,NULL},
   [ActChgTitExaSet	] = {1897,-1,TabUnk,ActSeeAllExa	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,ExaSet_ChangeSetTitle		,NULL},
   [ActChgNumQstExaSet	] = {1899,-1,TabUnk,ActSeeAllExa	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,ExaSet_ChangeNumQstsToExam	,NULL},

   [ActReqAddQstExaSet	] = {1885,-1,TabUnk,ActSeeAllExa	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Dat_SetIniEndDatesToPastAndNow	,ExaSet_ReqSelectQstsToAddToSet	,NULL},
   [ActLstTstQstForSet	] = {1886,-1,TabUnk,ActSeeAllExa	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,ExaSet_ListQstsToAddToSet	,NULL},
   [ActAddQstToExa	] = {1887,-1,TabUnk,ActSeeAllExa	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,ExaSet_AddQstsToSet		,NULL},
   [ActReqRemSetQst	] = {1888,-1,TabUnk,ActSeeAllExa	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,ExaSet_ReqRemQstFromSet	,NULL},
   [ActRemExaQst	] = {1889,-1,TabUnk,ActSeeAllExa	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,ExaSet_RemoveQstFromSet	,NULL},
   [ActValSetQst	] = {1909,-1,TabUnk,ActSeeAllExa	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,ExaSet_ValidateQst		,NULL},
   [ActInvSetQst	] = {1910,-1,TabUnk,ActSeeAllExa	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,ExaSet_InvalidateQst		,NULL},

   [ActReqNewExaSes	] = {1852,-1,TabUnk,ActSeeAllExa	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,ExaSes_ReqCreatOrEditSes	,NULL},
   [ActEdiOneExaSes	] = {1902,-1,TabUnk,ActSeeAllExa	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,ExaSes_ReqCreatOrEditSes	,NULL},
   [ActNewExaSes	] = {1853,-1,TabUnk,ActSeeAllExa	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,ExaSes_ReceiveFormSession	,NULL},
   [ActChgExaSes	] = {1903,-1,TabUnk,ActSeeAllExa	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,ExaSes_ReceiveFormSession	,NULL},
   [ActReqRemExaSes	] = {1850,-1,TabUnk,ActSeeAllExa	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,ExaSes_ReqRemSession		,NULL},
   [ActRemExaSes	] = {1851,-1,TabUnk,ActSeeAllExa	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,ExaSes_RemoveSession		,NULL},
   [ActHidExaSes	] = {1900,-1,TabUnk,ActSeeAllExa	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,ExaSes_HideSession		,NULL},
   [ActUnhExaSes	] = {1901,-1,TabUnk,ActSeeAllExa	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,ExaSes_UnhideSession		,NULL},

   [ActSeeExaPrn	] = {1904,-1,TabUnk,ActSeeAllExa	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,ExaPrn_ShowExamPrint		,NULL},
   [ActAnsExaPrn	] = {1906,-1,TabUnk,ActSeeAllExa	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_AJAX_NORMAL,NULL				,ExaPrn_ReceivePrintAnswer	,NULL},
   [ActEndExaPrn	] = {1908,-1,TabUnk,ActSeeAllExa	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_NEW_TAB,NULL				,ExaRes_ShowExaResultAfterFinish,NULL},

   [ActSeeMyExaResCrs	] = {1867,-1,TabUnk,ActSeeAllExa	,0x208,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,ExaRes_ShowMyResultsInCrs	,NULL},
   [ActSeeMyExaResExa	] = {1868,-1,TabUnk,ActSeeAllExa	,0x208,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,ExaRes_ShowMyResultsInExa	,NULL},
   [ActSeeMyExaResSes	] = {1869,-1,TabUnk,ActSeeAllExa	,0x208,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,ExaRes_ShowMyResultsInSes	,NULL},
   [ActSeeOneExaResMe	] = {1870,-1,TabUnk,ActSeeAllExa	,0x208,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,ExaRes_ShowOneExaResult	,NULL},

   [ActReqSeeUsrExaRes	] = {1871,-1,TabUnk,ActSeeAllExa	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,ExaRes_SelUsrsToViewResults	,NULL},
   [ActSeeUsrExaResCrs	] = {1872,-1,TabUnk,ActSeeAllExa	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,ExaRes_ShowAllResultsInCrs	,NULL},
   [ActSeeUsrExaResExa	] = {1873,-1,TabUnk,ActSeeAllExa	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,ExaRes_ShowAllResultsInExa	,NULL},
   [ActSeeUsrExaResSes	] = {1874,-1,TabUnk,ActSeeAllExa	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,ExaRes_ShowAllResultsInSes	,NULL},
   [ActSeeOneExaResOth	] = {1875,-1,TabUnk,ActSeeAllExa	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,ExaRes_ShowOneExaResult	,NULL},

   [ActChgVisExaRes	] = {1876,-1,TabUnk,ActSeeAllExa	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,ExaSes_ToggleVisResultsSesUsr	,NULL},

   [ActSeeOneGam	] = {1650,-1,TabUnk,ActSeeAllGam	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Gam_SeeOneGame			,NULL},

   [ActReqRemMch	] = {1783,-1,TabUnk,ActSeeAllGam	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Mch_ReqRemMatch		,NULL},
   [ActRemMch		] = {1784,-1,TabUnk,ActSeeAllGam	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Mch_RemoveMatch		,NULL},
   [ActEdiMch		] = {1913,-1,TabUnk,ActSeeAllGam	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Mch_EditMatch			,NULL},
   [ActChgMch		] = {1914,-1,TabUnk,ActSeeAllGam	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Mch_ChangeMatch		,NULL},
   [ActReqNewMch	] = {1670,-1,TabUnk,ActSeeAllGam	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Gam_ReqNewMatch		,NULL},
   [ActNewMch		] = {1671,-1,TabUnk,ActSeeAllGam	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_NEW_TAB,Mch_CreateNewMatch		,Mch_ResumeMatch		,NULL},
   [ActResMch		] = {1785,-1,TabUnk,ActSeeAllGam	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_NEW_TAB,Mch_GetMatchBeingPlayed	,Mch_ResumeMatch		,NULL},
   [ActBckMch		] = {1790,-1,TabUnk,ActSeeAllGam	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_2ND_TAB,Mch_GetMatchBeingPlayed	,Mch_BackMatch			,NULL},
   [ActPlyPauMch	] = {1789,-1,TabUnk,ActSeeAllGam	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_2ND_TAB,Mch_GetMatchBeingPlayed	,Mch_PlayPauseMatch		,NULL},
   [ActFwdMch		] = {1672,-1,TabUnk,ActSeeAllGam	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_2ND_TAB,Mch_GetMatchBeingPlayed	,Mch_ForwardMatch		,NULL},
   [ActChgNumColMch	] = {1802,-1,TabUnk,ActSeeAllGam	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_2ND_TAB,Mch_GetMatchBeingPlayed	,Mch_ChangeNumColsMch		,NULL},
   [ActChgVisResMchQst	] = {1794,-1,TabUnk,ActSeeAllGam	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_2ND_TAB,Mch_GetMatchBeingPlayed	,Mch_ToggleVisResultsMchQst	,NULL},
   [ActMchCntDwn	] = {1814,-1,TabUnk,ActSeeAllGam	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_AJAX_NORMAL,Mch_GetMatchBeingPlayed	,Mch_StartCountdown		,NULL},
   [ActRefMchTch	] = {1788,-1,TabUnk,ActSeeAllGam	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_AJAX_RFRESH,Mch_GetMatchBeingPlayed	,Mch_RefreshMatchTch		,NULL},

   [ActJoiMch		] = {1780,-1,TabUnk,ActSeeAllGam	,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_NEW_TAB,Mch_GetMatchBeingPlayed	,Mch_JoinMatchAsStd		,NULL},
   [ActSeeMchAnsQstStd	] = {1808,-1,TabUnk,ActSeeAllGam	,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_2ND_TAB,Mch_GetMatchBeingPlayed	,Mch_JoinMatchAsStd		,NULL},
   [ActRemMchAnsQstStd	] = {1809,-1,TabUnk,ActSeeAllGam	,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_2ND_TAB,Mch_GetMatchBeingPlayed	,Mch_RemMyQstAnsAndShowMchStatus,NULL},
   [ActAnsMchQstStd	] = {1651,-1,TabUnk,ActSeeAllGam	,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_2ND_TAB,Mch_GetMatchBeingPlayed	,Mch_ReceiveQuestionAnswer	,NULL},
   [ActRefMchStd	] = {1782,-1,TabUnk,ActSeeAllGam	,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_AJAX_RFRESH,Mch_GetMatchBeingPlayed	,Mch_RefreshMatchStd		,NULL},

   [ActSeeMyMchResCrs	] = {1796,-1,TabUnk,ActSeeAllGam	,0x208,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,MchRes_ShowMyMchResultsInCrs	,NULL},
   [ActSeeMyMchResGam	] = {1810,-1,TabUnk,ActSeeAllGam	,0x208,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,MchRes_ShowMyMchResultsInGam	,NULL},
   [ActSeeMyMchResMch	] = {1812,-1,TabUnk,ActSeeAllGam	,0x208,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,MchRes_ShowMyMchResultsInMch	,NULL},
   [ActSeeOneMchResMe	] = {1797,-1,TabUnk,ActSeeAllGam	,0x208,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,MchRes_ShowOneMchResult	,NULL},

   [ActReqSeeUsrMchRes	] = {1798,-1,TabUnk,ActSeeAllGam	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,MchRes_SelUsrsToViewMchResults	,NULL},
   [ActSeeUsrMchResCrs	] = {1799,-1,TabUnk,ActSeeAllGam	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,MchRes_ShowAllMchResultsInCrs	,NULL},
   [ActSeeUsrMchResGam	] = {1811,-1,TabUnk,ActSeeAllGam	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,MchRes_ShowAllMchResultsInGam	,NULL},
   [ActSeeUsrMchResMch	] = {1813,-1,TabUnk,ActSeeAllGam	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,MchRes_ShowAllMchResultsInMch	,NULL},
   [ActSeeOneMchResOth	] = {1800,-1,TabUnk,ActSeeAllGam	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,MchRes_ShowOneMchResult	,NULL},

   [ActChgVisResMchUsr	] = {1801,-1,TabUnk,ActSeeAllGam	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Mch_ToggleVisResultsMchUsr	,NULL},

   [ActLstOneGam	] = {1912,-1,TabUnk,ActSeeAllGam	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Gam_ListGame			,NULL},

   [ActFrmNewGam	] = {1652,-1,TabUnk,ActSeeAllGam	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Gam_ReqCreatOrEditGame		,NULL},
   [ActEdiOneGam	] = {1653,-1,TabUnk,ActSeeAllGam	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Gam_ReqCreatOrEditGame		,NULL},
   [ActNewGam		] = {1654,-1,TabUnk,ActSeeAllGam	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Gam_ReceiveFormGame		,NULL},
   [ActChgGam		] = {1655,-1,TabUnk,ActSeeAllGam	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Gam_ReceiveFormGame		,NULL},
   [ActReqRemGam	] = {1656,-1,TabUnk,ActSeeAllGam	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Gam_AskRemGame			,NULL},
   [ActRemGam		] = {1657,-1,TabUnk,ActSeeAllGam	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Gam_RemoveGame			,NULL},
   [ActHidGam		] = {1660,-1,TabUnk,ActSeeAllGam	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Gam_HideGame			,NULL},
   [ActUnhGam		] = {1661,-1,TabUnk,ActSeeAllGam	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Gam_UnhideGame			,NULL},
   [ActAddOneGamQst	] = {1662,-1,TabUnk,ActSeeAllGam	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Dat_SetIniEndDatesToPastAndNow	,Gam_ReqSelectQstsToAddToGame	,NULL},
   [ActGamLstTstQst	] = {1666,-1,TabUnk,ActSeeAllGam	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Gam_ListQstsToAddToGame	,NULL},
   [ActAddTstQstToGam	] = {1667,-1,TabUnk,ActSeeAllGam	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Gam_AddQstsToGame		,NULL},
   [ActReqRemGamQst	] = {1664,-1,TabUnk,ActSeeAllGam	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Gam_ReqRemQstFromGame		,NULL},
   [ActRemGamQst	] = {1665,-1,TabUnk,ActSeeAllGam	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Gam_RemoveQstFromGame		,NULL},
   [ActUp_GamQst	] = {1668,-1,TabUnk,ActSeeAllGam	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Gam_MoveUpQst			,NULL},
   [ActDwnGamQst	] = {1669,-1,TabUnk,ActSeeAllGam	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Gam_MoveDownQst		,NULL},
   [ActReqLnkGam	] = {1935,-1,TabUnk,ActSeeAllGam	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,GamRsc_GetLinkToGame		,NULL},

   [ActSeeOneRub	] = {1952,-1,TabUnk,ActSeeAllRub	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Rub_SeeOneRubric		,NULL},
   [ActFrmNewRub	] = {1953,-1,TabUnk,ActSeeAllRub	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Rub_ReqCreatOrEditRubric	,NULL},
   [ActEdiOneRub	] = {1954,-1,TabUnk,ActSeeAllRub	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Rub_ReqCreatOrEditRubric	,NULL},
   [ActNewRub		] = {1955,-1,TabUnk,ActSeeAllRub	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Rub_ReceiveFormRubric		,NULL},
   [ActChgRub		] = {1956,-1,TabUnk,ActSeeAllRub	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Rub_ReceiveFormRubric		,NULL},
   [ActReqRemRub	] = {1957,-1,TabUnk,ActSeeAllRub	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Rub_AskRemRubric		,NULL},
   [ActRemRub		] = {1958,-1,TabUnk,ActSeeAllRub	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Rub_RemoveRubric		,NULL},
   [ActReqLnkRub	] = {1969,-1,TabUnk,ActSeeAllRub	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,RubRsc_GetLinkToRubric		,NULL},
   [ActSeeRscCli_InRub	] = {1972,-1,TabUnk,ActSeeAllRub	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,RubRsc_ViewResourceClipboard	,NULL},
   [ActRemRscCli_InRub	] = {1973,-1,TabUnk,ActSeeAllRub	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,RubRsc_RemoveResourceClipboard	,NULL},

   [ActNewRubCri	] = {1959,-1,TabUnk,ActSeeAllRub	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,RubCri_ReceiveFormCriterion	,NULL},
   [ActReqRemRubCri	] = {1960,-1,TabUnk,ActSeeAllRub	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,RubCri_ReqRemCriterion		,NULL},
   [ActRemRubCri	] = {1961,-1,TabUnk,ActSeeAllRub	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,RubCri_RemoveCriterion		,NULL},
   [ActUp_RubCri	] = {1962,-1,TabUnk,ActSeeAllRub	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,RubCri_MoveUpCriterion		,NULL},
   [ActDwnRubCri	] = {1963,-1,TabUnk,ActSeeAllRub	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,RubCri_MoveDownCriterion	,NULL},
   [ActChgTitRubCri	] = {1964,-1,TabUnk,ActSeeAllRub	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,RubCri_ChangeTitle		,NULL},
   [ActChgLnkRubCri	] = {1968,-1,TabUnk,ActSeeAllRub	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,RubCri_ChangeLink		,NULL},
   [ActChgMinRubCri	] = {1965,-1,TabUnk,ActSeeAllRub	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,RubCri_ChangeMinValue		,NULL},
   [ActChgMaxRubCri	] = {1966,-1,TabUnk,ActSeeAllRub	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,RubCri_ChangeMaxValue		,NULL},
   [ActChgWeiRubCri	] = {1967,-1,TabUnk,ActSeeAllRub	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,RubCri_ChangeWeight		,NULL},

   // TabFil ******************************************************************
   // Actions in menu:
   [ActSeeAdmDocIns	] = {1249, 0,TabFil,ActSeeAdmDocIns	,    0,    0,    0,    0,0x3C7,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileBrowserOrWorks	,"folder-open"		},
   [ActAdmShaIns	] = {1382, 1,TabFil,ActAdmShaIns	,    0,    0,    0,    0,0x3C7,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileBrowserOrWorks	,"folder-open"		},
   [ActSeeAdmDocCtr	] = {1248, 2,TabFil,ActSeeAdmDocCtr	,    0,    0,    0,0x3C7,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileBrowserOrWorks	,"folder-open"		},
   [ActAdmShaCtr	] = {1363, 3,TabFil,ActAdmShaCtr	,    0,    0,    0,0x3C7,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileBrowserOrWorks	,"folder-open"		},
   [ActSeeAdmDocDeg	] = {1247, 4,TabFil,ActSeeAdmDocDeg	,    0,    0,0x3C7,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileBrowserOrWorks	,"folder-open"		},
   [ActAdmShaDeg	] = {1344, 5,TabFil,ActAdmShaDeg	,    0,    0,0x3C7,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileBrowserOrWorks	,"folder-open"		},
   [ActSeeAdmDocCrsGrp	] = {   0, 6,TabFil,ActSeeAdmDocCrsGrp	,0x3F8,0x3C7,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileBrowserOrWorks	,"folder-open"		},
   [ActAdmTchCrsGrp	] = {1525, 7,TabFil,ActAdmTchCrsGrp	,0x3F0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileBrowserOrWorks	,"folder-open"		},
   [ActAdmShaCrsGrp	] = { 461, 8,TabFil,ActAdmShaCrsGrp	,0x3F8,0x3C7,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileBrowserOrWorks	,"folder-open"		},
   [ActAdmAsgWrkUsr	] = { 792, 9,TabFil,ActAdmAsgWrkUsr	,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileBrowserOrWorks	,"folder-open"		},
   [ActReqAsgWrkCrs	] = { 899,10,TabFil,ActReqAsgWrkCrs	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_AskEditWorksCrs		,"folder-open"		},
   [ActSeeAdmMrk	] = {  17,11,TabFil,ActSeeAdmMrk	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileBrowserOrWorks	,"list-alt"		},
   [ActAdmBrf		] = {  23,12,TabFil,ActAdmBrf		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileBrowserOrWorks	,"briefcase"		},

   // Actions not in menu:
   [ActChgToSeeDocIns	] = {1308,-1,TabUnk,ActSeeAdmDocIns	,    0,    0,    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileBrowserOrWorks	,NULL},

   [ActSeeDocIns	] = {1309,-1,TabUnk,ActSeeAdmDocIns	,    0,    0,    0,    0,0x3C7,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileBrowserOrWorks	,NULL},
   [ActExpSeeDocIns	] = {1310,-1,TabUnk,ActSeeAdmDocIns	,    0,    0,    0,    0,0x3C7,    0,    0,Act_CONT_NORM,Act_204_NO_CONT,Brw_ExpandFileTree		,NULL				,NULL},
   [ActConSeeDocIns	] = {1311,-1,TabUnk,ActSeeAdmDocIns	,    0,    0,    0,    0,0x3C7,    0,    0,Act_CONT_NORM,Act_204_NO_CONT,Brw_ContractFileTree		,NULL				,NULL},
   [ActZIPSeeDocIns	] = {1312,-1,TabUnk,ActSeeAdmDocIns	,    0,    0,    0,    0,0x3C7,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,ZIP_CompressFileTree		,NULL},
   [ActReqDatSeeDocIns	] = {1313,-1,TabUnk,ActSeeAdmDocIns	,    0,    0,    0,    0,0x3C7,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileMetadata		,NULL},
   [ActDowSeeDocIns	] = {1314,-1,TabUnk,ActSeeAdmDocIns	,    0,    0,    0,    0,0x3C7,    0,    0,Act_CONT_NORM,Act_DOWNLD_FILE,Brw_DownloadFile		,NULL				,NULL},

   [ActChgToAdmDocIns	] = {1315,-1,TabUnk,ActSeeAdmDocIns	,    0,    0,    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileBrowserOrWorks	,NULL},

   [ActAdmDocIns	] = {1316,-1,TabUnk,ActSeeAdmDocIns	,    0,    0,    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileBrowserOrWorks	,NULL},
   [ActReqRemFilDocIns	] = {1317,-1,TabUnk,ActSeeAdmDocIns	,    0,    0,    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_AskRemFileFromTree		,NULL},
   [ActRemFilDocIns	] = {1318,-1,TabUnk,ActSeeAdmDocIns	,    0,    0,    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RemFileFromTree		,NULL},
   [ActRemFolDocIns	] = {1319,-1,TabUnk,ActSeeAdmDocIns	,    0,    0,    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RemFolderFromTree		,NULL},
   [ActCopDocIns	] = {1320,-1,TabUnk,ActSeeAdmDocIns	,    0,    0,    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_CopyFromFileBrowser	,NULL},
   [ActPasDocIns	] = {1321,-1,TabUnk,ActSeeAdmDocIns	,    0,    0,    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_PasteIntoFileBrowser	,NULL},
   [ActRemTreDocIns	] = {1322,-1,TabUnk,ActSeeAdmDocIns	,    0,    0,    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RemSubtreeInFileBrowser	,NULL},
   [ActFrmCreDocIns	] = {1323,-1,TabUnk,ActSeeAdmDocIns	,    0,    0,    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFormFileBrowser	,NULL},
   [ActCreFolDocIns	] = {1324,-1,TabUnk,ActSeeAdmDocIns	,    0,    0,    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RecFolderFileBrowser	,NULL},
   [ActCreLnkDocIns	] = {1325,-1,TabUnk,ActSeeAdmDocIns	,    0,    0,    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RecLinkFileBrowser		,NULL},
   [ActRenFolDocIns	] = {1326,-1,TabUnk,ActSeeAdmDocIns	,    0,    0,    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RenFolderFileBrowser	,NULL},
   [ActRcvFilDocInsDZ	] = {1327,-1,TabUnk,ActSeeAdmDocIns	,    0,    0,    0,    0,0x300,    0,    0,Act_CONT_DATA,Act_UPLOAD_FILE,Brw_RcvFileInFileBrwDropzone	,NULL				,NULL},
   [ActRcvFilDocInsCla	] = {1328,-1,TabUnk,ActSeeAdmDocIns	,    0,    0,    0,    0,0x300,    0,    0,Act_CONT_DATA,Act_BRW_1ST_TAB,NULL				,Brw_RcvFileInFileBrwClassic	,NULL},
   [ActExpAdmDocIns	] = {1329,-1,TabUnk,ActSeeAdmDocIns	,    0,    0,    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_204_NO_CONT,Brw_ExpandFileTree		,NULL				,NULL},
   [ActConAdmDocIns	] = {1330,-1,TabUnk,ActSeeAdmDocIns	,    0,    0,    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_204_NO_CONT,Brw_ContractFileTree		,NULL				,NULL},
   [ActZIPAdmDocIns	] = {1331,-1,TabUnk,ActSeeAdmDocIns	,    0,    0,    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,ZIP_CompressFileTree		,NULL},
   [ActUnhDocIns	] = {1332,-1,TabUnk,ActSeeAdmDocIns	,    0,    0,    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_SetDocumentAsVisible	,NULL},
   [ActHidDocIns	] = {1333,-1,TabUnk,ActSeeAdmDocIns	,    0,    0,    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_SetDocumentAsHidden	,NULL},
   [ActReqDatAdmDocIns	] = {1334,-1,TabUnk,ActSeeAdmDocIns	,    0,    0,    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileMetadata		,NULL},
   [ActChgDatAdmDocIns	] = {1335,-1,TabUnk,ActSeeAdmDocIns	,    0,    0,    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ChgFileMetadata		,NULL},
   [ActDowAdmDocIns   	] = {1336,-1,TabUnk,ActSeeAdmDocIns	,    0,    0,    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_DOWNLD_FILE,Brw_DownloadFile		,NULL				,NULL},

   [ActReqRemFilShaIns	] = {1383,-1,TabUnk,ActAdmShaIns	,    0,    0,    0,    0,0x3C0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_AskRemFileFromTree		,NULL},
   [ActRemFilShaIns	] = {1384,-1,TabUnk,ActAdmShaIns	,    0,    0,    0,    0,0x3C0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RemFileFromTree		,NULL},
   [ActRemFolShaIns	] = {1385,-1,TabUnk,ActAdmShaIns	,    0,    0,    0,    0,0x3C0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RemFolderFromTree		,NULL},
   [ActCopShaIns	] = {1386,-1,TabUnk,ActAdmShaIns	,    0,    0,    0,    0,0x3C0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_CopyFromFileBrowser	,NULL},
   [ActPasShaIns	] = {1387,-1,TabUnk,ActAdmShaIns	,    0,    0,    0,    0,0x3C0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_PasteIntoFileBrowser	,NULL},
   [ActRemTreShaIns	] = {1388,-1,TabUnk,ActAdmShaIns	,    0,    0,    0,    0,0x3C0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RemSubtreeInFileBrowser	,NULL},
   [ActFrmCreShaIns	] = {1389,-1,TabUnk,ActAdmShaIns	,    0,    0,    0,    0,0x3C0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFormFileBrowser	,NULL},
   [ActCreFolShaIns	] = {1390,-1,TabUnk,ActAdmShaIns	,    0,    0,    0,    0,0x3C0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RecFolderFileBrowser	,NULL},
   [ActCreLnkShaIns	] = {1391,-1,TabUnk,ActAdmShaIns	,    0,    0,    0,    0,0x3C0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RecLinkFileBrowser		,NULL},
   [ActRenFolShaIns	] = {1392,-1,TabUnk,ActAdmShaIns	,    0,    0,    0,    0,0x3C0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RenFolderFileBrowser	,NULL},
   [ActRcvFilShaInsDZ	] = {1393,-1,TabUnk,ActAdmShaIns	,    0,    0,    0,    0,0x3C0,    0,    0,Act_CONT_DATA,Act_UPLOAD_FILE,Brw_RcvFileInFileBrwDropzone	,NULL				,NULL},
   [ActRcvFilShaInsCla	] = {1394,-1,TabUnk,ActAdmShaIns	,    0,    0,    0,    0,0x3C0,    0,    0,Act_CONT_DATA,Act_BRW_1ST_TAB,NULL				,Brw_RcvFileInFileBrwClassic	,NULL},
   [ActExpShaIns	] = {1395,-1,TabUnk,ActAdmShaIns	,    0,    0,    0,    0,0x3C0,    0,    0,Act_CONT_NORM,Act_204_NO_CONT,Brw_ExpandFileTree		,NULL				,NULL},
   [ActConShaIns	] = {1396,-1,TabUnk,ActAdmShaIns	,    0,    0,    0,    0,0x3C0,    0,    0,Act_CONT_NORM,Act_204_NO_CONT,Brw_ContractFileTree		,NULL				,NULL},
   [ActZIPShaIns	] = {1397,-1,TabUnk,ActAdmShaIns	,    0,    0,    0,    0,0x3C0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,ZIP_CompressFileTree		,NULL},
   [ActReqDatShaIns	] = {1398,-1,TabUnk,ActAdmShaIns	,    0,    0,    0,    0,0x3C7,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileMetadata		,NULL},
   [ActChgDatShaIns	] = {1399,-1,TabUnk,ActAdmShaIns	,    0,    0,    0,    0,0x3C0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ChgFileMetadata		,NULL},
   [ActDowShaIns	] = {1400,-1,TabUnk,ActAdmShaIns	,    0,    0,    0,    0,0x3C7,    0,    0,Act_CONT_NORM,Act_DOWNLD_FILE,Brw_DownloadFile		,NULL				,NULL},

   [ActChgToSeeDocCtr	] = {1279,-1,TabUnk,ActSeeAdmDocCtr	,    0,    0,    0,0x380,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileBrowserOrWorks	,NULL},

   [ActSeeDocCtr	] = {1280,-1,TabUnk,ActSeeAdmDocCtr	,    0,    0,    0,0x3C7,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileBrowserOrWorks	,NULL},
   [ActExpSeeDocCtr	] = {1281,-1,TabUnk,ActSeeAdmDocCtr	,    0,    0,    0,0x3C7,    0,    0,    0,Act_CONT_NORM,Act_204_NO_CONT,Brw_ExpandFileTree		,NULL				,NULL},
   [ActConSeeDocCtr	] = {1282,-1,TabUnk,ActSeeAdmDocCtr	,    0,    0,    0,0x3C7,    0,    0,    0,Act_CONT_NORM,Act_204_NO_CONT,Brw_ContractFileTree		,NULL				,NULL},
   [ActZIPSeeDocCtr	] = {1283,-1,TabUnk,ActSeeAdmDocCtr	,    0,    0,    0,0x3C7,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,ZIP_CompressFileTree		,NULL},
   [ActReqDatSeeDocCtr	] = {1284,-1,TabUnk,ActSeeAdmDocCtr	,    0,    0,    0,0x3C7,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileMetadata		,NULL},
   [ActDowSeeDocCtr   	] = {1285,-1,TabUnk,ActSeeAdmDocCtr	,    0,    0,    0,0x3C7,    0,    0,    0,Act_CONT_NORM,Act_DOWNLD_FILE,Brw_DownloadFile		,NULL				,NULL},

   [ActChgToAdmDocCtr	] = {1286,-1,TabUnk,ActSeeAdmDocCtr	,    0,    0,    0,0x380,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileBrowserOrWorks	,NULL},

   [ActAdmDocCtr	] = {1287,-1,TabUnk,ActSeeAdmDocCtr	,    0,    0,    0,0x380,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileBrowserOrWorks	,NULL},
   [ActReqRemFilDocCtr	] = {1288,-1,TabUnk,ActSeeAdmDocCtr	,    0,    0,    0,0x380,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_AskRemFileFromTree		,NULL},
   [ActRemFilDocCtr	] = {1289,-1,TabUnk,ActSeeAdmDocCtr	,    0,    0,    0,0x380,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RemFileFromTree		,NULL},
   [ActRemFolDocCtr	] = {1290,-1,TabUnk,ActSeeAdmDocCtr	,    0,    0,    0,0x380,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RemFolderFromTree		,NULL},
   [ActCopDocCtr	] = {1291,-1,TabUnk,ActSeeAdmDocCtr	,    0,    0,    0,0x380,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_CopyFromFileBrowser	,NULL},
   [ActPasDocCtr	] = {1292,-1,TabUnk,ActSeeAdmDocCtr	,    0,    0,    0,0x380,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_PasteIntoFileBrowser	,NULL},
   [ActRemTreDocCtr	] = {1293,-1,TabUnk,ActSeeAdmDocCtr	,    0,    0,    0,0x380,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RemSubtreeInFileBrowser	,NULL},
   [ActFrmCreDocCtr	] = {1294,-1,TabUnk,ActSeeAdmDocCtr	,    0,    0,    0,0x380,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFormFileBrowser	,NULL},
   [ActCreFolDocCtr	] = {1295,-1,TabUnk,ActSeeAdmDocCtr	,    0,    0,    0,0x380,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RecFolderFileBrowser	,NULL},
   [ActCreLnkDocCtr	] = {1296,-1,TabUnk,ActSeeAdmDocCtr	,    0,    0,    0,0x380,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RecLinkFileBrowser		,NULL},
   [ActRenFolDocCtr	] = {1297,-1,TabUnk,ActSeeAdmDocCtr	,    0,    0,    0,0x380,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RenFolderFileBrowser	,NULL},
   [ActRcvFilDocCtrDZ	] = {1298,-1,TabUnk,ActSeeAdmDocCtr	,    0,    0,    0,0x380,    0,    0,    0,Act_CONT_DATA,Act_UPLOAD_FILE,Brw_RcvFileInFileBrwDropzone	,NULL				,NULL},
   [ActRcvFilDocCtrCla	] = {1299,-1,TabUnk,ActSeeAdmDocCtr	,    0,    0,    0,0x380,    0,    0,    0,Act_CONT_DATA,Act_BRW_1ST_TAB,NULL				,Brw_RcvFileInFileBrwClassic	,NULL},
   [ActExpAdmDocCtr	] = {1300,-1,TabUnk,ActSeeAdmDocCtr	,    0,    0,    0,0x380,    0,    0,    0,Act_CONT_NORM,Act_204_NO_CONT,Brw_ExpandFileTree		,NULL				,NULL},
   [ActConAdmDocCtr	] = {1301,-1,TabUnk,ActSeeAdmDocCtr	,    0,    0,    0,0x380,    0,    0,    0,Act_CONT_NORM,Act_204_NO_CONT,Brw_ContractFileTree		,NULL				,NULL},
   [ActZIPAdmDocCtr	] = {1302,-1,TabUnk,ActSeeAdmDocCtr	,    0,    0,    0,0x380,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,ZIP_CompressFileTree		,NULL},
   [ActUnhDocCtr	] = {1303,-1,TabUnk,ActSeeAdmDocCtr	,    0,    0,    0,0x380,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_SetDocumentAsVisible	,NULL},
   [ActHidDocCtr	] = {1304,-1,TabUnk,ActSeeAdmDocCtr	,    0,    0,    0,0x380,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_SetDocumentAsHidden	,NULL},
   [ActReqDatAdmDocCtr	] = {1305,-1,TabUnk,ActSeeAdmDocCtr	,    0,    0,    0,0x380,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileMetadata		,NULL},
   [ActChgDatAdmDocCtr	] = {1306,-1,TabUnk,ActSeeAdmDocCtr	,    0,    0,    0,0x380,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ChgFileMetadata		,NULL},
   [ActDowAdmDocCtr   	] = {1307,-1,TabUnk,ActSeeAdmDocCtr	,    0,    0,    0,0x380,    0,    0,    0,Act_CONT_NORM,Act_DOWNLD_FILE,Brw_DownloadFile		,NULL				,NULL},

   [ActReqRemFilShaCtr	] = {1364,-1,TabUnk,ActAdmShaCtr	,    0,    0,    0,0x3C0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_AskRemFileFromTree		,NULL},
   [ActRemFilShaCtr	] = {1365,-1,TabUnk,ActAdmShaCtr	,    0,    0,    0,0x3C0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RemFileFromTree		,NULL},
   [ActRemFolShaCtr	] = {1366,-1,TabUnk,ActAdmShaCtr	,    0,    0,    0,0x3C0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RemFolderFromTree		,NULL},
   [ActCopShaCtr	] = {1367,-1,TabUnk,ActAdmShaCtr	,    0,    0,    0,0x3C0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_CopyFromFileBrowser	,NULL},
   [ActPasShaCtr	] = {1368,-1,TabUnk,ActAdmShaCtr	,    0,    0,    0,0x3C0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_PasteIntoFileBrowser	,NULL},
   [ActRemTreShaCtr	] = {1369,-1,TabUnk,ActAdmShaCtr	,    0,    0,    0,0x3C0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RemSubtreeInFileBrowser	,NULL},
   [ActFrmCreShaCtr	] = {1370,-1,TabUnk,ActAdmShaCtr	,    0,    0,    0,0x3C0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFormFileBrowser	,NULL},
   [ActCreFolShaCtr	] = {1371,-1,TabUnk,ActAdmShaCtr	,    0,    0,    0,0x3C0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RecFolderFileBrowser	,NULL},
   [ActCreLnkShaCtr	] = {1372,-1,TabUnk,ActAdmShaCtr	,    0,    0,    0,0x3C0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RecLinkFileBrowser		,NULL},
   [ActRenFolShaCtr	] = {1373,-1,TabUnk,ActAdmShaCtr	,    0,    0,    0,0x3C0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RenFolderFileBrowser	,NULL},
   [ActRcvFilShaCtrDZ	] = {1374,-1,TabUnk,ActAdmShaCtr	,    0,    0,    0,0x3C0,    0,    0,    0,Act_CONT_DATA,Act_UPLOAD_FILE,Brw_RcvFileInFileBrwDropzone	,NULL				,NULL},
   [ActRcvFilShaCtrCla	] = {1375,-1,TabUnk,ActAdmShaCtr	,    0,    0,    0,0x3C0,    0,    0,    0,Act_CONT_DATA,Act_BRW_1ST_TAB,NULL				,Brw_RcvFileInFileBrwClassic	,NULL},
   [ActExpShaCtr	] = {1376,-1,TabUnk,ActAdmShaCtr	,    0,    0,    0,0x3C0,    0,    0,    0,Act_CONT_NORM,Act_204_NO_CONT,Brw_ExpandFileTree		,NULL				,NULL},
   [ActConShaCtr	] = {1377,-1,TabUnk,ActAdmShaCtr	,    0,    0,    0,0x3C0,    0,    0,    0,Act_CONT_NORM,Act_204_NO_CONT,Brw_ContractFileTree		,NULL				,NULL},
   [ActZIPShaCtr	] = {1378,-1,TabUnk,ActAdmShaCtr	,    0,    0,    0,0x3C0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,ZIP_CompressFileTree		,NULL},
   [ActReqDatShaCtr	] = {1379,-1,TabUnk,ActAdmShaCtr	,    0,    0,    0,0x3C7,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileMetadata		,NULL},
   [ActChgDatShaCtr	] = {1380,-1,TabUnk,ActAdmShaCtr	,    0,    0,    0,0x3C0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ChgFileMetadata		,NULL},
   [ActDowShaCtr	] = {1381,-1,TabUnk,ActAdmShaCtr	,    0,    0,    0,0x3C7,    0,    0,    0,Act_CONT_NORM,Act_DOWNLD_FILE,Brw_DownloadFile		,NULL				,NULL},

   [ActChgToSeeDocDeg	] = {1250,-1,TabUnk,ActSeeAdmDocDeg	,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileBrowserOrWorks	,NULL},

   [ActSeeDocDeg	] = {1251,-1,TabUnk,ActSeeAdmDocDeg	,    0,    0,0x3C7,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileBrowserOrWorks	,NULL},
   [ActExpSeeDocDeg	] = {1252,-1,TabUnk,ActSeeAdmDocDeg	,    0,    0,0x3C7,    0,    0,    0,    0,Act_CONT_NORM,Act_204_NO_CONT,Brw_ExpandFileTree		,NULL				,NULL},
   [ActConSeeDocDeg	] = {1253,-1,TabUnk,ActSeeAdmDocDeg	,    0,    0,0x3C7,    0,    0,    0,    0,Act_CONT_NORM,Act_204_NO_CONT,Brw_ContractFileTree		,NULL				,NULL},
   [ActZIPSeeDocDeg	] = {1254,-1,TabUnk,ActSeeAdmDocDeg	,    0,    0,0x3C7,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,ZIP_CompressFileTree		,NULL},
   [ActReqDatSeeDocDeg	] = {1255,-1,TabUnk,ActSeeAdmDocDeg	,    0,    0,0x3C7,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileMetadata		,NULL},
   [ActDowSeeDocDeg	] = {1256,-1,TabUnk,ActSeeAdmDocDeg	,    0,    0,0x3C7,    0,    0,    0,    0,Act_CONT_NORM,Act_DOWNLD_FILE,Brw_DownloadFile		,NULL				,NULL},

   [ActChgToAdmDocDeg	] = {1257,-1,TabUnk,ActSeeAdmDocDeg	,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileBrowserOrWorks	,NULL},

   [ActAdmDocDeg	] = {1258,-1,TabUnk,ActSeeAdmDocDeg	,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileBrowserOrWorks	,NULL},
   [ActReqRemFilDocDeg	] = {1259,-1,TabUnk,ActSeeAdmDocDeg	,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_AskRemFileFromTree		,NULL},
   [ActRemFilDocDeg	] = {1260,-1,TabUnk,ActSeeAdmDocDeg	,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RemFileFromTree		,NULL},
   [ActRemFolDocDeg	] = {1261,-1,TabUnk,ActSeeAdmDocDeg	,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RemFolderFromTree		,NULL},
   [ActCopDocDeg	] = {1262,-1,TabUnk,ActSeeAdmDocDeg	,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_CopyFromFileBrowser	,NULL},
   [ActPasDocDeg	] = {1263,-1,TabUnk,ActSeeAdmDocDeg	,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_PasteIntoFileBrowser	,NULL},
   [ActRemTreDocDeg	] = {1264,-1,TabUnk,ActSeeAdmDocDeg	,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RemSubtreeInFileBrowser	,NULL},
   [ActFrmCreDocDeg	] = {1265,-1,TabUnk,ActSeeAdmDocDeg	,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFormFileBrowser	,NULL},
   [ActCreFolDocDeg	] = {1266,-1,TabUnk,ActSeeAdmDocDeg	,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RecFolderFileBrowser	,NULL},
   [ActCreLnkDocDeg	] = {1267,-1,TabUnk,ActSeeAdmDocDeg	,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RecLinkFileBrowser		,NULL},
   [ActRenFolDocDeg	] = {1268,-1,TabUnk,ActSeeAdmDocDeg	,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RenFolderFileBrowser	,NULL},
   [ActRcvFilDocDegDZ	] = {1269,-1,TabUnk,ActSeeAdmDocDeg	,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_DATA,Act_UPLOAD_FILE,Brw_RcvFileInFileBrwDropzone	,NULL				,NULL},
   [ActRcvFilDocDegCla	] = {1270,-1,TabUnk,ActSeeAdmDocDeg	,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_DATA,Act_BRW_1ST_TAB,NULL				,Brw_RcvFileInFileBrwClassic	,NULL},
   [ActExpAdmDocDeg	] = {1271,-1,TabUnk,ActSeeAdmDocDeg	,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_204_NO_CONT,Brw_ExpandFileTree		,NULL				,NULL},
   [ActConAdmDocDeg	] = {1272,-1,TabUnk,ActSeeAdmDocDeg	,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_204_NO_CONT,Brw_ContractFileTree		,NULL				,NULL},
   [ActZIPAdmDocDeg	] = {1273,-1,TabUnk,ActSeeAdmDocDeg	,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,ZIP_CompressFileTree		,NULL},
   [ActUnhDocDeg	] = {1274,-1,TabUnk,ActSeeAdmDocDeg	,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_SetDocumentAsVisible	,NULL},
   [ActHidDocDeg	] = {1275,-1,TabUnk,ActSeeAdmDocDeg	,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_SetDocumentAsHidden	,NULL},
   [ActReqDatAdmDocDeg	] = {1276,-1,TabUnk,ActSeeAdmDocDeg	,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileMetadata		,NULL},
   [ActChgDatAdmDocDeg	] = {1277,-1,TabUnk,ActSeeAdmDocDeg	,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ChgFileMetadata		,NULL},
   [ActDowAdmDocDeg	] = {1278,-1,TabUnk,ActSeeAdmDocDeg	,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_DOWNLD_FILE,Brw_DownloadFile		,NULL				,NULL},

   [ActReqRemFilShaDeg	] = {1345,-1,TabUnk,ActAdmShaDeg	,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_AskRemFileFromTree		,NULL},
   [ActRemFilShaDeg	] = {1346,-1,TabUnk,ActAdmShaDeg	,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RemFileFromTree		,NULL},
   [ActRemFolShaDeg	] = {1347,-1,TabUnk,ActAdmShaDeg	,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RemFolderFromTree		,NULL},
   [ActCopShaDeg	] = {1348,-1,TabUnk,ActAdmShaDeg	,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_CopyFromFileBrowser	,NULL},
   [ActPasShaDeg	] = {1349,-1,TabUnk,ActAdmShaDeg	,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_PasteIntoFileBrowser	,NULL},
   [ActRemTreShaDeg	] = {1350,-1,TabUnk,ActAdmShaDeg	,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RemSubtreeInFileBrowser	,NULL},
   [ActFrmCreShaDeg	] = {1351,-1,TabUnk,ActAdmShaDeg	,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFormFileBrowser	,NULL},
   [ActCreFolShaDeg	] = {1352,-1,TabUnk,ActAdmShaDeg	,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RecFolderFileBrowser	,NULL},
   [ActCreLnkShaDeg	] = {1353,-1,TabUnk,ActAdmShaDeg	,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RecLinkFileBrowser		,NULL},
   [ActRenFolShaDeg	] = {1354,-1,TabUnk,ActAdmShaDeg	,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RenFolderFileBrowser	,NULL},
   [ActRcvFilShaDegDZ	] = {1355,-1,TabUnk,ActAdmShaDeg	,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_DATA,Act_UPLOAD_FILE,Brw_RcvFileInFileBrwDropzone	,NULL				,NULL},
   [ActRcvFilShaDegCla	] = {1356,-1,TabUnk,ActAdmShaDeg	,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_DATA,Act_BRW_1ST_TAB,NULL				,Brw_RcvFileInFileBrwClassic	,NULL},
   [ActExpShaDeg	] = {1357,-1,TabUnk,ActAdmShaDeg	,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_204_NO_CONT,Brw_ExpandFileTree		,NULL				,NULL},
   [ActConShaDeg	] = {1358,-1,TabUnk,ActAdmShaDeg	,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_204_NO_CONT,Brw_ContractFileTree		,NULL				,NULL},
   [ActZIPShaDeg	] = {1359,-1,TabUnk,ActAdmShaDeg	,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,ZIP_CompressFileTree		,NULL},
   [ActReqDatShaDeg	] = {1360,-1,TabUnk,ActAdmShaDeg	,    0,    0,0x3C7,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileMetadata		,NULL},
   [ActChgDatShaDeg	] = {1361,-1,TabUnk,ActAdmShaDeg	,    0,    0,0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ChgFileMetadata		,NULL},
   [ActDowShaDeg	] = {1362,-1,TabUnk,ActAdmShaDeg	,    0,    0,0x3C7,    0,    0,    0,    0,Act_CONT_NORM,Act_DOWNLD_FILE,Brw_DownloadFile		,NULL				,NULL},

   [ActChgToSeeDocCrs	] = {1195,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x3F8,0x3C7,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileBrowserOrWorks	,NULL},

   [ActSeeDocCrs	] = {1078,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x3F8,0x3C7,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileBrowserOrWorks	,NULL},
   [ActExpSeeDocCrs	] = { 462,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_204_NO_CONT,Brw_ExpandFileTree		,NULL				,NULL},
   [ActConSeeDocCrs	] = { 476,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_204_NO_CONT,Brw_ContractFileTree		,NULL				,NULL},
   [ActZIPSeeDocCrs	] = {1124,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,ZIP_CompressFileTree		,NULL},
   [ActReqDatSeeDocCrs	] = {1033,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x3F8,0x3C7,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileMetadata		,NULL},
   [ActReqLnkSeeDocCrs	] = {1930,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,BrwRsc_GetLinkToDocFil		,NULL},
   [ActDowSeeDocCrs	] = {1111,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x3F8,0x3C7,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_DOWNLD_FILE,Brw_DownloadFile		,NULL				,NULL},

   [ActSeeDocGrp	] = {1200,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileBrowserOrWorks	,NULL},
   [ActExpSeeDocGrp	] = { 488,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_204_NO_CONT,Brw_ExpandFileTree		,NULL				,NULL},
   [ActConSeeDocGrp	] = { 489,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_204_NO_CONT,Brw_ContractFileTree		,NULL				,NULL},
   [ActZIPSeeDocGrp	] = {1125,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,ZIP_CompressFileTree		,NULL},
   [ActReqDatSeeDocGrp	] = {1034,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x3F8,0x3C7,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileMetadata		,NULL},
   [ActDowSeeDocGrp	] = {1112,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x3F8,0x3C7,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_DOWNLD_FILE,Brw_DownloadFile		,NULL				,NULL},

   [ActChgToAdmDocCrs	] = {1196,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileBrowserOrWorks	,NULL},

   [ActAdmDocCrs	] = {  12,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileBrowserOrWorks	,NULL},
   [ActReqRemFilDocCrs	] = { 479,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_AskRemFileFromTree		,NULL},
   [ActRemFilDocCrs	] = { 480,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RemFileFromTree		,NULL},
   [ActRemFolDocCrs	] = { 497,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RemFolderFromTree		,NULL},
   [ActCopDocCrs	] = { 470,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_CopyFromFileBrowser	,NULL},
   [ActPasDocCrs	] = { 478,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_PasteIntoFileBrowser	,NULL},
   [ActRemTreDocCrs	] = { 498,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RemSubtreeInFileBrowser	,NULL},
   [ActFrmCreDocCrs	] = { 481,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFormFileBrowser	,NULL},
   [ActCreFolDocCrs	] = { 491,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RecFolderFileBrowser	,NULL},
   [ActCreLnkDocCrs	] = {1225,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RecLinkFileBrowser		,NULL},
   [ActRenFolDocCrs	] = { 535,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RenFolderFileBrowser	,NULL},
   [ActRcvFilDocCrsDZ	] = {1214,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_DATA,Act_UPLOAD_FILE,Brw_RcvFileInFileBrwDropzone	,NULL				,NULL},
   [ActRcvFilDocCrsCla	] = { 482,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_DATA,Act_BRW_1ST_TAB,NULL				,Brw_RcvFileInFileBrwClassic	,NULL},
   [ActExpAdmDocCrs	] = { 477,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_204_NO_CONT,Brw_ExpandFileTree		,NULL				,NULL},
   [ActConAdmDocCrs	] = { 494,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_204_NO_CONT,Brw_ContractFileTree		,NULL				,NULL},
   [ActZIPAdmDocCrs	] = {1126,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,ZIP_CompressFileTree		,NULL},
   [ActUnhDocCrs	] = { 464,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_SetDocumentAsVisible	,NULL},
   [ActHidDocCrs	] = { 465,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_SetDocumentAsHidden	,NULL},
   [ActReqDatAdmDocCrs	] = {1029,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileMetadata		,NULL},
   [ActChgDatAdmDocCrs	] = { 996,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ChgFileMetadata		,NULL},
   [ActReqLnkAdmDocCrs	] = {1931,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,BrwRsc_GetLinkToDocFil		,NULL},
   [ActDowAdmDocCrs	] = {1113,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_DOWNLD_FILE,Brw_DownloadFile		,NULL				,NULL},

   [ActAdmDocGrp	] = {1201,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileBrowserOrWorks	,NULL},
   [ActReqRemFilDocGrp	] = { 473,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_AskRemFileFromTree		,NULL},
   [ActRemFilDocGrp	] = { 474,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RemFileFromTree		,NULL},
   [ActRemFolDocGrp	] = { 484,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RemFolderFromTree		,NULL},
   [ActCopDocGrp	] = { 472,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_CopyFromFileBrowser	,NULL},
   [ActPasDocGrp	] = { 471,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_PasteIntoFileBrowser	,NULL},
   [ActRemTreDocGrp	] = { 485,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RemSubtreeInFileBrowser	,NULL},
   [ActFrmCreDocGrp	] = { 468,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFormFileBrowser	,NULL},
   [ActCreFolDocGrp	] = { 469,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RecFolderFileBrowser	,NULL},
   [ActCreLnkDocGrp	] = {1231,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RecLinkFileBrowser		,NULL},
   [ActRenFolDocGrp	] = { 490,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RenFolderFileBrowser	,NULL},
   [ActRcvFilDocGrpDZ	] = {1215,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_DATA,Act_UPLOAD_FILE,Brw_RcvFileInFileBrwDropzone	,NULL				,NULL},
   [ActRcvFilDocGrpCla	] = { 483,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_DATA,Act_BRW_1ST_TAB,NULL				,Brw_RcvFileInFileBrwClassic	,NULL},
   [ActExpAdmDocGrp	] = { 486,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_204_NO_CONT,Brw_ExpandFileTree		,NULL				,NULL},
   [ActConAdmDocGrp	] = { 487,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_204_NO_CONT,Brw_ContractFileTree		,NULL				,NULL},
   [ActZIPAdmDocGrp	] = {1127,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,ZIP_CompressFileTree		,NULL},
   [ActUnhDocGrp	] = { 493,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_SetDocumentAsVisible	,NULL},
   [ActHidDocGrp	] = { 492,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_SetDocumentAsHidden	,NULL},
   [ActReqDatAdmDocGrp	] = {1030,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileMetadata		,NULL},
   [ActChgDatAdmDocGrp	] = { 998,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ChgFileMetadata		,NULL},
   [ActDowAdmDocGrp	] = {1114,-1,TabUnk,ActSeeAdmDocCrsGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_DOWNLD_FILE,Brw_DownloadFile		,NULL				,NULL},

   [ActChgToAdmTch	] = {1526,-1,TabUnk,ActAdmTchCrsGrp	,0x3F0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileBrowserOrWorks	,NULL},

   [ActAdmTchCrs	] = {1527,-1,TabUnk,ActAdmTchCrsGrp	,0x3F0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileBrowserOrWorks	,NULL},
   [ActReqRemFilTchCrs	] = {1528,-1,TabUnk,ActAdmTchCrsGrp	,0x3F0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_AskRemFileFromTree		,NULL},
   [ActRemFilTchCrs	] = {1529,-1,TabUnk,ActAdmTchCrsGrp	,0x3F0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RemFileFromTree		,NULL},
   [ActRemFolTchCrs	] = {1530,-1,TabUnk,ActAdmTchCrsGrp	,0x3F0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RemFolderFromTree		,NULL},
   [ActCopTchCrs	] = {1531,-1,TabUnk,ActAdmTchCrsGrp	,0x3F0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_CopyFromFileBrowser	,NULL},
   [ActPasTchCrs	] = {1532,-1,TabUnk,ActAdmTchCrsGrp	,0x3F0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_PasteIntoFileBrowser	,NULL},
   [ActRemTreTchCrs	] = {1533,-1,TabUnk,ActAdmTchCrsGrp	,0x3F0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RemSubtreeInFileBrowser	,NULL},
   [ActFrmCreTchCrs	] = {1534,-1,TabUnk,ActAdmTchCrsGrp	,0x3F0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFormFileBrowser	,NULL},
   [ActCreFolTchCrs	] = {1535,-1,TabUnk,ActAdmTchCrsGrp	,0x3F0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RecFolderFileBrowser	,NULL},
   [ActCreLnkTchCrs	] = {1536,-1,TabUnk,ActAdmTchCrsGrp	,0x3F0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RecLinkFileBrowser		,NULL},
   [ActRenFolTchCrs	] = {1537,-1,TabUnk,ActAdmTchCrsGrp	,0x3F0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RenFolderFileBrowser	,NULL},
   [ActRcvFilTchCrsDZ	] = {1538,-1,TabUnk,ActAdmTchCrsGrp	,0x3F0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_DATA,Act_UPLOAD_FILE,Brw_RcvFileInFileBrwDropzone	,NULL				,NULL},
   [ActRcvFilTchCrsCla	] = {1539,-1,TabUnk,ActAdmTchCrsGrp	,0x3F0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_DATA,Act_BRW_1ST_TAB,NULL				,Brw_RcvFileInFileBrwClassic	,NULL},
   [ActExpTchCrs	] = {1540,-1,TabUnk,ActAdmTchCrsGrp	,0x3F0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_204_NO_CONT,Brw_ExpandFileTree		,NULL				,NULL},
   [ActConTchCrs	] = {1541,-1,TabUnk,ActAdmTchCrsGrp	,0x3F0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_204_NO_CONT,Brw_ContractFileTree		,NULL				,NULL},
   [ActZIPTchCrs	] = {1542,-1,TabUnk,ActAdmTchCrsGrp	,0x3F0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,ZIP_CompressFileTree		,NULL},
   [ActReqDatTchCrs	] = {1543,-1,TabUnk,ActAdmTchCrsGrp	,0x3F0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileMetadata		,NULL},
   [ActChgDatTchCrs	] = {1544,-1,TabUnk,ActAdmTchCrsGrp	,0x3F0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ChgFileMetadata		,NULL},
   [ActDowTchCrs	] = {1545,-1,TabUnk,ActAdmTchCrsGrp	,0x3F0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_DOWNLD_FILE,Brw_DownloadFile		,NULL				,NULL},

   [ActAdmTchGrp	] = {1546,-1,TabUnk,ActAdmTchCrsGrp	,0x3F0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileBrowserOrWorks	,NULL},
   [ActReqRemFilTchGrp	] = {1547,-1,TabUnk,ActAdmTchCrsGrp	,0x3F0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_AskRemFileFromTree		,NULL},
   [ActRemFilTchGrp	] = {1548,-1,TabUnk,ActAdmTchCrsGrp	,0x3F0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RemFileFromTree		,NULL},
   [ActRemFolTchGrp	] = {1549,-1,TabUnk,ActAdmTchCrsGrp	,0x3F0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RemFolderFromTree		,NULL},
   [ActCopTchGrp	] = {1550,-1,TabUnk,ActAdmTchCrsGrp	,0x3F0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_CopyFromFileBrowser	,NULL},
   [ActPasTchGrp	] = {1551,-1,TabUnk,ActAdmTchCrsGrp	,0x3F0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_PasteIntoFileBrowser	,NULL},
   [ActRemTreTchGrp	] = {1552,-1,TabUnk,ActAdmTchCrsGrp	,0x3F0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RemSubtreeInFileBrowser	,NULL},
   [ActFrmCreTchGrp	] = {1553,-1,TabUnk,ActAdmTchCrsGrp	,0x3F0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFormFileBrowser	,NULL},
   [ActCreFolTchGrp	] = {1554,-1,TabUnk,ActAdmTchCrsGrp	,0x3F0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RecFolderFileBrowser	,NULL},
   [ActCreLnkTchGrp	] = {1555,-1,TabUnk,ActAdmTchCrsGrp	,0x3F0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RecLinkFileBrowser		,NULL},
   [ActRenFolTchGrp	] = {1556,-1,TabUnk,ActAdmTchCrsGrp	,0x3F0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RenFolderFileBrowser	,NULL},
   [ActRcvFilTchGrpDZ	] = {1557,-1,TabUnk,ActAdmTchCrsGrp	,0x3F0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_DATA,Act_UPLOAD_FILE,Brw_RcvFileInFileBrwDropzone	,NULL				,NULL},
   [ActRcvFilTchGrpCla	] = {1558,-1,TabUnk,ActAdmTchCrsGrp	,0x3F0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_DATA,Act_BRW_1ST_TAB,NULL				,Brw_RcvFileInFileBrwClassic	,NULL},
   [ActExpTchGrp	] = {1559,-1,TabUnk,ActAdmTchCrsGrp	,0x3F0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_204_NO_CONT,Brw_ExpandFileTree		,NULL				,NULL},
   [ActConTchGrp	] = {1560,-1,TabUnk,ActAdmTchCrsGrp	,0x3F0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_204_NO_CONT,Brw_ContractFileTree		,NULL				,NULL},
   [ActZIPTchGrp	] = {1561,-1,TabUnk,ActAdmTchCrsGrp	,0x3F0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,ZIP_CompressFileTree		,NULL},
   [ActReqDatTchGrp	] = {1562,-1,TabUnk,ActAdmTchCrsGrp	,0x3F0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileMetadata		,NULL},
   [ActChgDatTchGrp	] = {1563,-1,TabUnk,ActAdmTchCrsGrp	,0x3F0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ChgFileMetadata		,NULL},
   [ActDowTchGrp	] = {1564,-1,TabUnk,ActAdmTchCrsGrp	,0x3F0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_DOWNLD_FILE,Brw_DownloadFile		,NULL				,NULL},

   [ActChgToAdmSha	] = {1197,-1,TabUnk,ActAdmShaCrsGrp	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileBrowserOrWorks	,NULL},

   [ActAdmShaCrs	] = {1202,-1,TabUnk,ActAdmShaCrsGrp	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileBrowserOrWorks	,NULL},
   [ActReqRemFilShaCrs	] = { 327,-1,TabUnk,ActAdmShaCrsGrp	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_AskRemFileFromTree		,NULL},
   [ActRemFilShaCrs	] = { 328,-1,TabUnk,ActAdmShaCrsGrp	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RemFileFromTree		,NULL},
   [ActRemFolShaCrs	] = { 325,-1,TabUnk,ActAdmShaCrsGrp	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RemFolderFromTree		,NULL},
   [ActCopShaCrs	] = { 330,-1,TabUnk,ActAdmShaCrsGrp	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_CopyFromFileBrowser	,NULL},
   [ActPasShaCrs	] = { 331,-1,TabUnk,ActAdmShaCrsGrp	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_PasteIntoFileBrowser	,NULL},
   [ActRemTreShaCrs	] = { 332,-1,TabUnk,ActAdmShaCrsGrp	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RemSubtreeInFileBrowser	,NULL},
   [ActFrmCreShaCrs	] = { 323,-1,TabUnk,ActAdmShaCrsGrp	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFormFileBrowser	,NULL},
   [ActCreFolShaCrs	] = { 324,-1,TabUnk,ActAdmShaCrsGrp	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RecFolderFileBrowser	,NULL},
   [ActCreLnkShaCrs	] = {1226,-1,TabUnk,ActAdmShaCrsGrp	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RecLinkFileBrowser		,NULL},
   [ActRenFolShaCrs	] = { 329,-1,TabUnk,ActAdmShaCrsGrp	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RenFolderFileBrowser	,NULL},
   [ActRcvFilShaCrsDZ	] = {1216,-1,TabUnk,ActAdmShaCrsGrp	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_DATA,Act_UPLOAD_FILE,Brw_RcvFileInFileBrwDropzone	,NULL				,NULL},
   [ActRcvFilShaCrsCla	] = { 326,-1,TabUnk,ActAdmShaCrsGrp	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_DATA,Act_BRW_1ST_TAB,NULL				,Brw_RcvFileInFileBrwClassic	,NULL},
   [ActExpShaCrs	] = { 421,-1,TabUnk,ActAdmShaCrsGrp	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_204_NO_CONT,Brw_ExpandFileTree		,NULL				,NULL},
   [ActConShaCrs	] = { 422,-1,TabUnk,ActAdmShaCrsGrp	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_204_NO_CONT,Brw_ContractFileTree		,NULL				,NULL},
   [ActZIPShaCrs	] = {1128,-1,TabUnk,ActAdmShaCrsGrp	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,ZIP_CompressFileTree		,NULL},
   [ActReqDatShaCrs	] = {1031,-1,TabUnk,ActAdmShaCrsGrp	,0x3F8,0x3C7,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileMetadata		,NULL},
   [ActChgDatShaCrs	] = {1000,-1,TabUnk,ActAdmShaCrsGrp	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ChgFileMetadata		,NULL},
   [ActDowShaCrs	] = {1115,-1,TabUnk,ActAdmShaCrsGrp	,0x3F8,0x3C7,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_DOWNLD_FILE,Brw_DownloadFile		,NULL				,NULL},

   [ActAdmShaGrp	] = {1203,-1,TabUnk,ActAdmShaCrsGrp	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileBrowserOrWorks	,NULL},
   [ActReqRemFilShaGrp	] = { 341,-1,TabUnk,ActAdmShaCrsGrp	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_AskRemFileFromTree		,NULL},
   [ActRemFilShaGrp	] = { 342,-1,TabUnk,ActAdmShaCrsGrp	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RemFileFromTree		,NULL},
   [ActRemFolShaGrp	] = { 338,-1,TabUnk,ActAdmShaCrsGrp	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RemFolderFromTree		,NULL},
   [ActCopShaGrp	] = { 336,-1,TabUnk,ActAdmShaCrsGrp	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_CopyFromFileBrowser	,NULL},
   [ActPasShaGrp	] = { 337,-1,TabUnk,ActAdmShaCrsGrp	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_PasteIntoFileBrowser	,NULL},
   [ActRemTreShaGrp	] = { 339,-1,TabUnk,ActAdmShaCrsGrp	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RemSubtreeInFileBrowser	,NULL},
   [ActFrmCreShaGrp	] = { 333,-1,TabUnk,ActAdmShaCrsGrp	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFormFileBrowser	,NULL},
   [ActCreFolShaGrp	] = { 334,-1,TabUnk,ActAdmShaCrsGrp	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RecFolderFileBrowser	,NULL},
   [ActCreLnkShaGrp	] = {1227,-1,TabUnk,ActAdmShaCrsGrp	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RecLinkFileBrowser		,NULL},
   [ActRenFolShaGrp	] = { 340,-1,TabUnk,ActAdmShaCrsGrp	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RenFolderFileBrowser	,NULL},
   [ActRcvFilShaGrpDZ	] = {1217,-1,TabUnk,ActAdmShaCrsGrp	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_DATA,Act_UPLOAD_FILE,Brw_RcvFileInFileBrwDropzone	,NULL				,NULL},
   [ActRcvFilShaGrpCla	] = { 335,-1,TabUnk,ActAdmShaCrsGrp	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_DATA,Act_BRW_1ST_TAB,NULL				,Brw_RcvFileInFileBrwClassic	,NULL},
   [ActExpShaGrp	] = { 427,-1,TabUnk,ActAdmShaCrsGrp	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_204_NO_CONT,Brw_ExpandFileTree		,NULL				,NULL},
   [ActConShaGrp	] = { 426,-1,TabUnk,ActAdmShaCrsGrp	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_204_NO_CONT,Brw_ContractFileTree		,NULL				,NULL},
   [ActZIPShaGrp	] = {1129,-1,TabUnk,ActAdmShaCrsGrp	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,ZIP_CompressFileTree		,NULL},
   [ActReqDatShaGrp	] = {1032,-1,TabUnk,ActAdmShaCrsGrp	,0x3F8,0x3C7,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileMetadata		,NULL},
   [ActChgDatShaGrp	] = {1002,-1,TabUnk,ActAdmShaCrsGrp	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ChgFileMetadata		,NULL},
   [ActDowShaGrp	] = {1116,-1,TabUnk,ActAdmShaCrsGrp	,0x3F8,0x3C7,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_DOWNLD_FILE,Brw_DownloadFile		,NULL				,NULL},

   [ActAdmAsgWrkCrs	] = { 139,-1,TabUnk,ActReqAsgWrkCrs	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_GetSelectedUsrsAndShowWorks,NULL},

   [ActReqRemFilAsgUsr	] = { 834,-1,TabUnk,ActAdmAsgWrkUsr	,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_AskRemFileFromTree		,NULL},
   [ActRemFilAsgUsr	] = { 833,-1,TabUnk,ActAdmAsgWrkUsr	,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RemFileFromTree		,NULL},
   [ActRemFolAsgUsr	] = { 827,-1,TabUnk,ActAdmAsgWrkUsr	,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RemFolderFromTree		,NULL},
   [ActCopAsgUsr	] = { 829,-1,TabUnk,ActAdmAsgWrkUsr	,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_CopyFromFileBrowser	,NULL},
   [ActPasAsgUsr	] = { 830,-1,TabUnk,ActAdmAsgWrkUsr	,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_PasteIntoFileBrowser	,NULL},
   [ActRemTreAsgUsr	] = { 828,-1,TabUnk,ActAdmAsgWrkUsr	,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RemSubtreeInFileBrowser	,NULL},
   [ActFrmCreAsgUsr	] = { 825,-1,TabUnk,ActAdmAsgWrkUsr	,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFormFileBrowser	,NULL},
   [ActCreFolAsgUsr	] = { 826,-1,TabUnk,ActAdmAsgWrkUsr	,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RecFolderFileBrowser	,NULL},
   [ActCreLnkAsgUsr	] = {1232,-1,TabUnk,ActAdmAsgWrkUsr	,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RecLinkFileBrowser		,NULL},
   [ActRenFolAsgUsr	] = { 839,-1,TabUnk,ActAdmAsgWrkUsr	,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RenFolderFileBrowser	,NULL},
   [ActRcvFilAsgUsrDZ	] = {1218,-1,TabUnk,ActAdmAsgWrkUsr	,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_DATA,Act_UPLOAD_FILE,Brw_RcvFileInFileBrwDropzone	,NULL				,NULL},
   [ActRcvFilAsgUsrCla	] = { 832,-1,TabUnk,ActAdmAsgWrkUsr	,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_DATA,Act_BRW_1ST_TAB,NULL				,Brw_RcvFileInFileBrwClassic	,NULL},
   [ActExpAsgUsr	] = { 824,-1,TabUnk,ActAdmAsgWrkUsr	,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_204_NO_CONT,Brw_ExpandFileTree		,NULL				,NULL},
   [ActConAsgUsr	] = { 831,-1,TabUnk,ActAdmAsgWrkUsr	,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_204_NO_CONT,Brw_ContractFileTree		,NULL				,NULL},
   [ActZIPAsgUsr	] = {1130,-1,TabUnk,ActAdmAsgWrkUsr	,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,ZIP_CompressFileTree		,NULL},
   [ActReqDatAsgUsr	] = {1039,-1,TabUnk,ActAdmAsgWrkUsr	,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileMetadata		,NULL},
   [ActChgDatAsgUsr	] = {1040,-1,TabUnk,ActAdmAsgWrkUsr	,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ChgFileMetadata		,NULL},
   [ActDowAsgUsr	] = {1117,-1,TabUnk,ActAdmAsgWrkUsr	,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_DOWNLD_FILE,Brw_DownloadFile		,NULL				,NULL},

   [ActReqRemFilWrkUsr	] = { 288,-1,TabUnk,ActAdmAsgWrkUsr	,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_AskRemFileFromTree		,NULL},
   [ActRemFilWrkUsr	] = { 169,-1,TabUnk,ActAdmAsgWrkUsr	,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RemFileFromTree		,NULL},
   [ActRemFolWrkUsr	] = { 228,-1,TabUnk,ActAdmAsgWrkUsr	,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RemFolderFromTree		,NULL},
   [ActCopWrkUsr	] = { 314,-1,TabUnk,ActAdmAsgWrkUsr	,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_CopyFromFileBrowser	,NULL},
   [ActPasWrkUsr	] = { 318,-1,TabUnk,ActAdmAsgWrkUsr	,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_PasteIntoFileBrowser	,NULL},
   [ActRemTreWrkUsr	] = { 278,-1,TabUnk,ActAdmAsgWrkUsr	,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RemSubtreeInFileBrowser	,NULL},
   [ActFrmCreWrkUsr	] = { 150,-1,TabUnk,ActAdmAsgWrkUsr	,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFormFileBrowser	,NULL},
   [ActCreFolWrkUsr	] = { 172,-1,TabUnk,ActAdmAsgWrkUsr	,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RecFolderFileBrowser	,NULL},
   [ActCreLnkWrkUsr	] = {1228,-1,TabUnk,ActAdmAsgWrkUsr	,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RecLinkFileBrowser		,NULL},
   [ActRenFolWrkUsr	] = { 204,-1,TabUnk,ActAdmAsgWrkUsr	,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RenFolderFileBrowser	,NULL},
   [ActRcvFilWrkUsrDZ	] = {1219,-1,TabUnk,ActAdmAsgWrkUsr	,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_DATA,Act_UPLOAD_FILE,Brw_RcvFileInFileBrwDropzone	,NULL				,NULL},
   [ActRcvFilWrkUsrCla	] = { 148,-1,TabUnk,ActAdmAsgWrkUsr	,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_DATA,Act_BRW_1ST_TAB,NULL				,Brw_RcvFileInFileBrwClassic	,NULL},
   [ActExpWrkUsr	] = { 423,-1,TabUnk,ActAdmAsgWrkUsr	,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_204_NO_CONT,Brw_ExpandFileTree		,NULL				,NULL},
   [ActConWrkUsr	] = { 425,-1,TabUnk,ActAdmAsgWrkUsr	,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_204_NO_CONT,Brw_ContractFileTree		,NULL				,NULL},
   [ActZIPWrkUsr	] = {1131,-1,TabUnk,ActAdmAsgWrkUsr	,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,ZIP_CompressFileTree		,NULL},
   [ActReqDatWrkUsr	] = {1041,-1,TabUnk,ActAdmAsgWrkUsr	,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileMetadata		,NULL},
   [ActChgDatWrkUsr	] = {1042,-1,TabUnk,ActAdmAsgWrkUsr	,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ChgFileMetadata		,NULL},
   [ActDowWrkUsr	] = {1118,-1,TabUnk,ActAdmAsgWrkUsr	,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_DOWNLD_FILE,Brw_DownloadFile		,NULL				,NULL},

   [ActReqRemFilAsgCrs	] = { 837,-1,TabUnk,ActReqAsgWrkCrs	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_AskRemFileFromTree		,NULL},
   [ActRemFilAsgCrs	] = { 838,-1,TabUnk,ActReqAsgWrkCrs	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RemFileFromTree		,NULL},
   [ActRemFolAsgCrs	] = { 820,-1,TabUnk,ActReqAsgWrkCrs	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RemFolderFromTree		,NULL},
   [ActCopAsgCrs	] = { 836,-1,TabUnk,ActReqAsgWrkCrs	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_CopyFromFileBrowser	,NULL},
   [ActPasAsgCrs	] = { 821,-1,TabUnk,ActReqAsgWrkCrs	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_PasteIntoFileBrowser	,NULL},
   [ActRemTreAsgCrs	] = { 822,-1,TabUnk,ActReqAsgWrkCrs	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RemSubtreeInFileBrowser	,NULL},
   [ActFrmCreAsgCrs	] = { 817,-1,TabUnk,ActReqAsgWrkCrs	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFormFileBrowser	,NULL},
   [ActCreFolAsgCrs	] = { 818,-1,TabUnk,ActReqAsgWrkCrs	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RecFolderFileBrowser	,NULL},
   [ActCreLnkAsgCrs	] = {1233,-1,TabUnk,ActReqAsgWrkCrs	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RecLinkFileBrowser		,NULL},
   [ActRenFolAsgCrs	] = { 823,-1,TabUnk,ActReqAsgWrkCrs	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RenFolderFileBrowser	,NULL},
   [ActRcvFilAsgCrsDZ	] = {1220,-1,TabUnk,ActReqAsgWrkCrs	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_DATA,Act_UPLOAD_FILE,Brw_RcvFileInFileBrwDropzone	,NULL				,NULL},
   [ActRcvFilAsgCrsCla	] = { 846,-1,TabUnk,ActReqAsgWrkCrs	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_DATA,Act_BRW_1ST_TAB,NULL				,Brw_RcvFileInFileBrwClassic	,NULL},
   [ActExpAsgCrs	] = { 819,-1,TabUnk,ActReqAsgWrkCrs	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_204_NO_CONT,Brw_ExpandFileTree		,NULL				,NULL},
   [ActConAsgCrs	] = { 835,-1,TabUnk,ActReqAsgWrkCrs	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_204_NO_CONT,Brw_ContractFileTree		,NULL				,NULL},
   [ActZIPAsgCrs	] = {1132,-1,TabUnk,ActReqAsgWrkCrs	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,ZIP_CompressFileTree		,NULL},
   [ActReqDatAsgCrs	] = {1043,-1,TabUnk,ActReqAsgWrkCrs	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileMetadata		,NULL},
   [ActChgDatAsgCrs	] = {1044,-1,TabUnk,ActReqAsgWrkCrs	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ChgFileMetadata		,NULL},
   [ActDowAsgCrs	] = {1119,-1,TabUnk,ActReqAsgWrkCrs	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_DOWNLD_FILE,Brw_DownloadFile		,NULL				,NULL},

   [ActReqRemFilWrkCrs	] = { 289,-1,TabUnk,ActReqAsgWrkCrs	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_AskRemFileFromTree		,NULL},
   [ActRemFilWrkCrs	] = { 209,-1,TabUnk,ActReqAsgWrkCrs	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RemFileFromTree		,NULL},
   [ActRemFolWrkCrs	] = { 210,-1,TabUnk,ActReqAsgWrkCrs	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RemFolderFromTree		,NULL},
   [ActCopWrkCrs	] = { 312,-1,TabUnk,ActReqAsgWrkCrs	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_CopyFromFileBrowser	,NULL},
   [ActPasWrkCrs	] = { 319,-1,TabUnk,ActReqAsgWrkCrs	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_PasteIntoFileBrowser	,NULL},
   [ActRemTreWrkCrs	] = { 279,-1,TabUnk,ActReqAsgWrkCrs	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RemSubtreeInFileBrowser	,NULL},
   [ActFrmCreWrkCrs	] = { 205,-1,TabUnk,ActReqAsgWrkCrs	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFormFileBrowser	,NULL},
   [ActCreFolWrkCrs	] = { 206,-1,TabUnk,ActReqAsgWrkCrs	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RecFolderFileBrowser	,NULL},
   [ActCreLnkWrkCrs	] = {1229,-1,TabUnk,ActReqAsgWrkCrs	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RecLinkFileBrowser		,NULL},
   [ActRenFolWrkCrs	] = { 208,-1,TabUnk,ActReqAsgWrkCrs	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RenFolderFileBrowser	,NULL},
   [ActRcvFilWrkCrsDZ	] = {1221,-1,TabUnk,ActReqAsgWrkCrs	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_DATA,Act_UPLOAD_FILE,Brw_RcvFileInFileBrwDropzone	,NULL				,NULL},
   [ActRcvFilWrkCrsCla	] = { 207,-1,TabUnk,ActReqAsgWrkCrs	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_DATA,Act_BRW_1ST_TAB,NULL				,Brw_RcvFileInFileBrwClassic	,NULL},
   [ActExpWrkCrs	] = { 416,-1,TabUnk,ActReqAsgWrkCrs	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_204_NO_CONT,Brw_ExpandFileTree		,NULL				,NULL},
   [ActConWrkCrs	] = { 424,-1,TabUnk,ActReqAsgWrkCrs	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_204_NO_CONT,Brw_ContractFileTree		,NULL				,NULL},
   [ActZIPWrkCrs	] = {1133,-1,TabUnk,ActReqAsgWrkCrs	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,ZIP_CompressFileTree		,NULL},
   [ActReqDatWrkCrs	] = {1045,-1,TabUnk,ActReqAsgWrkCrs	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileMetadata		,NULL},
   [ActChgDatWrkCrs	] = {1046,-1,TabUnk,ActReqAsgWrkCrs	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ChgFileMetadata		,NULL},
   [ActDowWrkCrs	] = {1120,-1,TabUnk,ActReqAsgWrkCrs	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_DOWNLD_FILE,Brw_DownloadFile		,NULL				,NULL},

   [ActChgToSeeMrk	] = {1198,-1,TabUnk,ActSeeAdmMrk	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileBrowserOrWorks	,NULL},

   [ActSeeMrkCrs	] = {1079,-1,TabUnk,ActSeeAdmMrk	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileBrowserOrWorks	,NULL},
   [ActExpSeeMrkCrs	] = { 528,-1,TabUnk,ActSeeAdmMrk	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_204_NO_CONT,Brw_ExpandFileTree		,NULL				,NULL},
   [ActConSeeMrkCrs	] = { 527,-1,TabUnk,ActSeeAdmMrk	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_204_NO_CONT,Brw_ContractFileTree		,NULL				,NULL},
   [ActReqDatSeeMrkCrs	] = {1086,-1,TabUnk,ActSeeAdmMrk	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileMetadata		,NULL},
   [ActReqLnkSeeMrkCrs	] = {1939,-1,TabUnk,ActSeeAdmMrk	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,BrwRsc_GetLinkToMrkFil		,NULL},
   [ActSeeMyMrkCrs	] = { 523,-1,TabUnk,ActSeeAdmMrk	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_NEW_TAB,Mrk_ShowMyMarks		,NULL				,NULL},

   [ActSeeMrkGrp	] = {1204,-1,TabUnk,ActSeeAdmMrk	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileBrowserOrWorks	,NULL},
   [ActExpSeeMrkGrp	] = { 605,-1,TabUnk,ActSeeAdmMrk	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_204_NO_CONT,Brw_ExpandFileTree		,NULL				,NULL},
   [ActConSeeMrkGrp	] = { 609,-1,TabUnk,ActSeeAdmMrk	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_204_NO_CONT,Brw_ContractFileTree		,NULL				,NULL},
   [ActReqDatSeeMrkGrp	] = {1087,-1,TabUnk,ActSeeAdmMrk	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileMetadata		,NULL},
   [ActSeeMyMrkGrp	] = { 524,-1,TabUnk,ActSeeAdmMrk	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_NEW_TAB,Mrk_ShowMyMarks		,NULL				,NULL},

   [ActChgToAdmMrk	] = {1199,-1,TabUnk,ActSeeAdmMrk	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileBrowserOrWorks	,NULL},

   [ActAdmMrkCrs	] = { 284,-1,TabUnk,ActSeeAdmMrk	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileBrowserOrWorks	,NULL},
   [ActReqRemFilMrkCrs	] = { 595,-1,TabUnk,ActSeeAdmMrk	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_AskRemFileFromTree		,NULL},
   [ActRemFilMrkCrs	] = { 533,-1,TabUnk,ActSeeAdmMrk	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RemFileFromTree		,NULL},
   [ActRemFolMrkCrs	] = { 530,-1,TabUnk,ActSeeAdmMrk	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RemFolderFromTree		,NULL},
   [ActCopMrkCrs	] = { 501,-1,TabUnk,ActSeeAdmMrk	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_CopyFromFileBrowser	,NULL},
   [ActPasMrkCrs	] = { 507,-1,TabUnk,ActSeeAdmMrk	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_PasteIntoFileBrowser	,NULL},
   [ActRemTreMrkCrs	] = { 534,-1,TabUnk,ActSeeAdmMrk	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RemSubtreeInFileBrowser	,NULL},
   [ActFrmCreMrkCrs	] = { 596,-1,TabUnk,ActSeeAdmMrk	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFormFileBrowser	,NULL},
   [ActCreFolMrkCrs	] = { 506,-1,TabUnk,ActSeeAdmMrk	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RecFolderFileBrowser	,NULL},
   [ActRenFolMrkCrs	] = { 574,-1,TabUnk,ActSeeAdmMrk	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RenFolderFileBrowser	,NULL},
   [ActRcvFilMrkCrsDZ	] = {1222,-1,TabUnk,ActSeeAdmMrk	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_DATA,Act_UPLOAD_FILE,Brw_RcvFileInFileBrwDropzone	,NULL				,NULL},
   [ActRcvFilMrkCrsCla	] = { 516,-1,TabUnk,ActSeeAdmMrk	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_DATA,Act_BRW_1ST_TAB,NULL				,Brw_RcvFileInFileBrwClassic	,NULL},
   [ActExpAdmMrkCrs	] = { 607,-1,TabUnk,ActSeeAdmMrk	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_204_NO_CONT,Brw_ExpandFileTree		,NULL				,NULL},
   [ActConAdmMrkCrs	] = { 621,-1,TabUnk,ActSeeAdmMrk	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_204_NO_CONT,Brw_ContractFileTree		,NULL				,NULL},
   [ActZIPAdmMrkCrs	] = {1134,-1,TabUnk,ActSeeAdmMrk	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,ZIP_CompressFileTree		,NULL},
   [ActUnhMrkCrs	] = {1191,-1,TabUnk,ActSeeAdmMrk	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_SetDocumentAsVisible	,NULL},
   [ActHidMrkCrs	] = {1192,-1,TabUnk,ActSeeAdmMrk	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_SetDocumentAsHidden	,NULL},
   [ActReqDatAdmMrkCrs	] = {1035,-1,TabUnk,ActSeeAdmMrk	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileMetadata		,NULL},
   [ActChgDatAdmMrkCrs	] = {1036,-1,TabUnk,ActSeeAdmMrk	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ChgFileMetadata		,NULL},
   [ActReqLnkAdmMrkCrs	] = {1940,-1,TabUnk,ActSeeAdmMrk	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,BrwRsc_GetLinkToMrkFil		,NULL},
   [ActDowAdmMrkCrs	] = {1121,-1,TabUnk,ActSeeAdmMrk	,0x3F8,0x3C7,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_DOWNLD_FILE,Brw_DownloadFile		,NULL				,NULL},
   [ActChgNumRowHeaCrs	] = { 503,-1,TabUnk,ActSeeAdmMrk	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Mrk_ChangeNumRowsHeader	,NULL},
   [ActChgNumRowFooCrs	] = { 504,-1,TabUnk,ActSeeAdmMrk	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Mrk_ChangeNumRowsFooter	,NULL},

   [ActAdmMrkGrp	] = {1205,-1,TabUnk,ActSeeAdmMrk	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileBrowserOrWorks	,NULL},
   [ActReqRemFilMrkGrp	] = { 600,-1,TabUnk,ActSeeAdmMrk	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_AskRemFileFromTree		,NULL},
   [ActRemFilMrkGrp	] = { 509,-1,TabUnk,ActSeeAdmMrk	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RemFileFromTree		,NULL},
   [ActRemFolMrkGrp	] = { 520,-1,TabUnk,ActSeeAdmMrk	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RemFolderFromTree		,NULL},
   [ActCopMrkGrp	] = { 519,-1,TabUnk,ActSeeAdmMrk	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_CopyFromFileBrowser	,NULL},
   [ActPasMrkGrp	] = { 502,-1,TabUnk,ActSeeAdmMrk	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_PasteIntoFileBrowser	,NULL},
   [ActRemTreMrkGrp	] = { 521,-1,TabUnk,ActSeeAdmMrk	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RemSubtreeInFileBrowser	,NULL},
   [ActFrmCreMrkGrp	] = { 601,-1,TabUnk,ActSeeAdmMrk	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFormFileBrowser	,NULL},
   [ActCreFolMrkGrp	] = { 513,-1,TabUnk,ActSeeAdmMrk	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RecFolderFileBrowser	,NULL},
   [ActRenFolMrkGrp	] = { 529,-1,TabUnk,ActSeeAdmMrk	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RenFolderFileBrowser	,NULL},
   [ActRcvFilMrkGrpDZ	] = {1223,-1,TabUnk,ActSeeAdmMrk	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_DATA,Act_UPLOAD_FILE,Brw_RcvFileInFileBrwDropzone	,NULL				,NULL},
   [ActRcvFilMrkGrpCla	] = { 514,-1,TabUnk,ActSeeAdmMrk	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_DATA,Act_BRW_1ST_TAB,NULL				,Brw_RcvFileInFileBrwClassic	,NULL},
   [ActExpAdmMrkGrp	] = { 631,-1,TabUnk,ActSeeAdmMrk	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_204_NO_CONT,Brw_ExpandFileTree		,NULL				,NULL},
   [ActConAdmMrkGrp	] = { 900,-1,TabUnk,ActSeeAdmMrk	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_204_NO_CONT,Brw_ContractFileTree		,NULL				,NULL},
   [ActZIPAdmMrkGrp	] = {1135,-1,TabUnk,ActSeeAdmMrk	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,ZIP_CompressFileTree		,NULL},
   [ActUnhMrkGrp	] = {1193,-1,TabUnk,ActSeeAdmMrk	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_SetDocumentAsVisible	,NULL},
   [ActHidMrkGrp	] = {1194,-1,TabUnk,ActSeeAdmMrk	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_SetDocumentAsHidden	,NULL},
   [ActReqDatAdmMrkGrp	] = {1037,-1,TabUnk,ActSeeAdmMrk	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileMetadata		,NULL},
   [ActChgDatAdmMrkGrp	] = {1038,-1,TabUnk,ActSeeAdmMrk	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ChgFileMetadata		,NULL},
   [ActDowAdmMrkGrp	] = {1122,-1,TabUnk,ActSeeAdmMrk	,0x3F8,0x3C7,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_DOWNLD_FILE,Brw_DownloadFile		,NULL				,NULL},
   [ActChgNumRowHeaGrp	] = { 510,-1,TabUnk,ActSeeAdmMrk	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Mrk_ChangeNumRowsHeader	,NULL},
   [ActChgNumRowFooGrp	] = { 511,-1,TabUnk,ActSeeAdmMrk	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Mrk_ChangeNumRowsFooter	,NULL},

   [ActReqRemFilBrf	] = { 286,-1,TabUnk,ActAdmBrf		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_AskRemFileFromTree		,NULL},
   [ActRemFilBrf	] = { 155,-1,TabUnk,ActAdmBrf		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RemFileFromTree		,NULL},
   [ActRemFolBrf	] = { 196,-1,TabUnk,ActAdmBrf		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RemFolderFromTree		,NULL},
   [ActCopBrf		] = { 311,-1,TabUnk,ActAdmBrf		,0x3F8,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_CopyFromFileBrowser	,NULL},
   [ActPasBrf		] = { 315,-1,TabUnk,ActAdmBrf		,0x3F8,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_PasteIntoFileBrowser	,NULL},
   [ActRemTreBrf	] = { 276,-1,TabUnk,ActAdmBrf		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RemSubtreeInFileBrowser	,NULL},
   [ActFrmCreBrf	] = { 597,-1,TabUnk,ActAdmBrf		,0x3F8,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFormFileBrowser	,NULL},
   [ActCreFolBrf	] = { 170,-1,TabUnk,ActAdmBrf		,0x3F8,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RecFolderFileBrowser	,NULL},
   [ActCreLnkBrf	] = {1230,-1,TabUnk,ActAdmBrf		,0x3F8,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RecLinkFileBrowser		,NULL},
   [ActRenFolBrf	] = { 197,-1,TabUnk,ActAdmBrf		,0x3F8,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RenFolderFileBrowser	,NULL},
   [ActRcvFilBrfDZ	] = {1224,-1,TabUnk,ActAdmBrf		,0x3F8,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_DATA,Act_UPLOAD_FILE,Brw_RcvFileInFileBrwDropzone	,NULL				,NULL},
   [ActRcvFilBrfCla	] = { 153,-1,TabUnk,ActAdmBrf		,0x3F8,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_DATA,Act_BRW_1ST_TAB,NULL				,Brw_RcvFileInFileBrwClassic	,NULL},
   [ActExpBrf		] = { 410,-1,TabUnk,ActAdmBrf		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_204_NO_CONT,Brw_ExpandFileTree		,NULL				,NULL},
   [ActConBrf		] = { 411,-1,TabUnk,ActAdmBrf		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_204_NO_CONT,Brw_ContractFileTree		,NULL				,NULL},
   [ActZIPBrf		] = {1136,-1,TabUnk,ActAdmBrf		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,ZIP_CompressFileTree		,NULL},
   [ActReqDatBrf	] = {1047,-1,TabUnk,ActAdmBrf		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ShowFileMetadata		,NULL},
   [ActChgDatBrf	] = {1048,-1,TabUnk,ActAdmBrf		,0x3F8,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_ChgFileMetadata		,NULL},
   [ActDowBrf		] = {1123,-1,TabUnk,ActAdmBrf		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_DOWNLD_FILE,Brw_DownloadFile		,NULL				,NULL},
   [ActReqRemOldBrf	] = {1488,-1,TabUnk,ActAdmBrf		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_AskRemoveOldFilesBriefcase	,NULL},
   [ActRemOldBrf	] = {1489,-1,TabUnk,ActAdmBrf		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Brw_RemoveOldFilesBriefcase	,NULL},

   // TabUsr ******************************************************************
   // Actions in menu:
   [ActReqSelGrp	] = { 116, 0,TabUsr,ActReqSelGrp	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Grp_ReqRegisterInGrps		,"sitemap"		},
   [ActLstStd		] = { 678, 1,TabUsr,ActLstStd		,0x3F8,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Usr_SeeStudents		,"users"		},
   [ActLstTch		] = { 679, 2,TabUsr,ActLstTch		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Usr_SeeTeachers		,"user-tie"		},
   [ActLstOth		] = {1186, 3,TabUsr,ActLstOth		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Usr_ListDataAdms		,"user-friends"		},
   [ActSeeAllAtt	] = { 861, 4,TabUsr,ActSeeAllAtt	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Att_SeeEvents			,"calendar-check"	},
   [ActReqSignUp	] = {1054, 5,TabUsr,ActReqSignUp	,    0,0x006,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Enr_ReqSignUpInCrs		,"hand-point-up"	},
   [ActSeeSignUpReq	] = {1057, 6,TabUsr,ActSeeSignUpReq	,0x3E0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Enr_ShowEnrolmentRequests	,"hand-point-up"	},
   [ActLstCon		] = { 995, 7,TabUsr,ActLstCon		,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Con_ShowConnectedUsrs		,"user-clock"		},

   // Actions not in menu:
   [ActChgGrp		] = { 118,-1,TabUnk,ActReqSelGrp	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Grp_ChangeMyGrpsAndShowChanges	,NULL},

   [ActReqEdiGrp	] = { 108,-1,TabUnk,ActReqSelGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Grp_ReqEditGroups		,NULL},

   [ActNewGrpTyp	] = { 174,-1,TabUnk,ActReqSelGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Grp_ReceiveFormNewGrpTyp	,NULL},
   [ActReqRemGrpTyp	] = { 236,-1,TabUnk,ActReqSelGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Grp_ReqRemGroupType		,NULL},
   [ActRemGrpTyp	] = { 237,-1,TabUnk,ActReqSelGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Grp_RemoveGroupType		,NULL},
   [ActRenGrpTyp	] = { 304,-1,TabUnk,ActReqSelGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Grp_RenameGroupType		,NULL},
   [ActChgMdtGrpTyp	] = { 303,-1,TabUnk,ActReqSelGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Grp_ChangeMandatGrpTyp		,NULL},
   [ActChgMulGrpTyp	] = { 302,-1,TabUnk,ActReqSelGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Grp_ChangeMultiGrpTyp		,NULL},
   [ActChgTimGrpTyp	] = {1061,-1,TabUnk,ActReqSelGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Grp_ChangeOpenTimeGrpTyp	,NULL},

   [ActNewGrp		] = { 122,-1,TabUnk,ActReqSelGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Grp_ReceiveFormNewGrp		,NULL},
   [ActReqRemGrp	] = { 107,-1,TabUnk,ActReqSelGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Grp_ReqRemGroup		,NULL},
   [ActRemGrp		] = { 175,-1,TabUnk,ActReqSelGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Grp_RemoveGroup		,NULL},
   [ActOpeGrp		] = { 322,-1,TabUnk,ActReqSelGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Grp_OpenGroup			,NULL},
   [ActCloGrp		] = { 321,-1,TabUnk,ActReqSelGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Grp_CloseGroup			,NULL},
   [ActEnaFilZonGrp	] = { 495,-1,TabUnk,ActReqSelGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Grp_EnableFileZonesGrp		,NULL},
   [ActDisFilZonGrp	] = { 496,-1,TabUnk,ActReqSelGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Grp_DisableFileZonesGrp	,NULL},
   [ActChgGrpTyp	] = { 167,-1,TabUnk,ActReqSelGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Grp_ChangeGroupType		,NULL},
   [ActRenGrp		] = { 121,-1,TabUnk,ActReqSelGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Grp_RenameGroup		,NULL},
   [ActChgGrpRoo	] = {1752,-1,TabUnk,ActReqSelGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Grp_ChangeGroupRoom		,NULL},
   [ActChgMaxStdGrp	] = { 106,-1,TabUnk,ActReqSelGrp	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Grp_ChangeMaxStdsGrp		,NULL},

   [ActLstGst		] = { 587,-1,TabUnk,ActLstOth		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Usr_SeeGuests			,NULL},

   [ActPrnGstPho	] = {1190,-1,TabUnk,ActLstOth		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_BRW_NEW_TAB,NULL				,Usr_SeeGstClassPhotoPrn	,NULL},
   [ActPrnStdPho	] = { 120,-1,TabUnk,ActLstStd		,0x3F8,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_NEW_TAB,NULL				,Usr_SeeStdClassPhotoPrn	,NULL},
   [ActPrnTchPho	] = { 443,-1,TabUnk,ActLstTch		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_NEW_TAB,NULL				,Usr_SeeTchClassPhotoPrn	,NULL},
   [ActLstGstAll	] = {1189,-1,TabUnk,ActLstOth		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_BRW_NEW_TAB,NULL				,Usr_ListAllDataGsts		,NULL},
   [ActLstStdAll	] = {  42,-1,TabUnk,ActLstStd		,0x3F8,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_NEW_TAB,NULL				,Usr_ListAllDataStds		,NULL},
   [ActLstTchAll	] = { 578,-1,TabUnk,ActLstTch		,0x3F8,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_NEW_TAB,NULL				,Usr_ListAllDataTchs 		,NULL},

   [ActSeeRecOneStd	] = {1174,-1,TabUnk,ActLstStd		,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Rec_GetUsrAndShowRecOneStdCrs	,NULL},
   [ActSeeRecOneTch	] = {1175,-1,TabUnk,ActLstTch		,0x3F8,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Rec_GetUsrAndShowRecOneTchCrs	,NULL},

   [ActDoActOnSevGst	] = {1753,-1,TabUnk,ActLstOth		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,Usr_DoActionOnSeveralUsrs1	,Usr_DoActionOnSeveralUsrs2	,NULL},
   [ActDoActOnSevStd	] = {1754,-1,TabUnk,ActLstStd		,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Usr_DoActionOnSeveralUsrs1	,Usr_DoActionOnSeveralUsrs2	,NULL},
   [ActDoActOnSevTch	] = {1755,-1,TabUnk,ActLstTch		,0x3F8,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Usr_DoActionOnSeveralUsrs1	,Usr_DoActionOnSeveralUsrs2	,NULL},

   [ActSeeRecSevGst	] = {1187,-1,TabUnk,ActLstOth		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Rec_ListRecordsGstsShow	,NULL},
   [ActSeeRecSevStd	] = {  89,-1,TabUnk,ActLstStd		,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Rec_ListRecordsStdsShow	,NULL},
   [ActSeeRecSevTch	] = {  22,-1,TabUnk,ActLstTch		,0x3F8,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Rec_ListRecordsTchsShow	,NULL},
   [ActPrnRecSevGst	] = {1188,-1,TabUnk,ActLstOth		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_BRW_NEW_TAB,NULL				,Rec_ListRecordsGstsPrint	,NULL},
   [ActPrnRecSevStd	] = { 111,-1,TabUnk,ActLstStd		,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_NEW_TAB,NULL				,Rec_ListRecordsStdsPrint	,NULL},
   [ActPrnRecSevTch	] = { 127,-1,TabUnk,ActLstTch		,0x3F8,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_NEW_TAB,NULL				,Rec_ListRecordsTchsPrint	,NULL},

   [ActRcvRecOthUsr	] = { 300,-1,TabUnk,ActLstStd		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Rec_UpdateAndShowOtherCrsRecord,NULL},
   [ActEdiRecFie	] = { 292,-1,TabUnk,ActLstStd		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Rec_ReqEditRecordFields	,NULL},
   [ActNewFie		] = { 293,-1,TabUnk,ActLstStd		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Rec_ReceiveFormField		,NULL},
   [ActReqRemFie	] = { 294,-1,TabUnk,ActLstStd		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Rec_ReqRemField		,NULL},
   [ActRemFie		] = { 295,-1,TabUnk,ActLstStd		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Rec_RemoveField		,NULL},
   [ActRenFie		] = { 296,-1,TabUnk,ActLstStd		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Rec_RenameField		,NULL},
   [ActChgRowFie	] = { 305,-1,TabUnk,ActLstStd		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Rec_ChangeLinesField		,NULL},
   [ActChgVisFie	] = { 297,-1,TabUnk,ActLstStd		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Rec_ChangeVisibilityField	,NULL},
   [ActRcvRecCrs	] = { 301,-1,TabUnk,ActLstStd		,0x208,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Rec_UpdateAndShowMyCrsRecord	,NULL},

   [ActFrmLogInUsrAgd	] = {1614,-1,TabUnk,ActLstTch		,    0,0x001,0x001,0x001,0x001,0x001,0x001,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Agd_PutFormLogInToShowUsrAgenda,NULL},
   [ActLogInUsrAgd	] = {1615,-1,TabUnk,ActLstTch		,0x3F8,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Agd_ShowOtherAgendaAfterLogIn	,NULL},
   [ActLogInUsrAgdLan	] = {1616,-1,TabUnk,ActLstTch		,0x3F8,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Agd_ShowOtherAgendaAfterLogIn	,NULL},
   [ActSeeUsrAgd	] = {1611,-1,TabUnk,ActLstTch		,0x3F8,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Agd_ShowUsrAgenda		,NULL},

   [ActReqEnrSevStd	] = {1426,-1,TabUnk,ActLstStd		,0x3E0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Enr_ReqAdminStds		,NULL},
   [ActReqEnrSevNET	] = {1642,-1,TabUnk,ActLstTch		,0x3C0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Enr_ReqAdminNonEditingTchs	,NULL},
   [ActReqEnrSevTch	] = {1427,-1,TabUnk,ActLstTch		,0x3C0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Enr_ReqAdminTchs		,NULL},

   [ActReqLstUsrAtt	] = {1073,-1,TabUnk,ActSeeAllAtt	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Att_ReqListUsrsAttendanceCrs	,NULL},
   [ActSeeLstMyAtt	] = {1473,-1,TabUnk,ActSeeAllAtt	,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Att_ListMyAttendanceCrs	,NULL},
   [ActPrnLstMyAtt	] = {1474,-1,TabUnk,ActSeeAllAtt	,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_NEW_TAB,NULL				,Att_PrintMyAttendanceCrs	,NULL},
   [ActSeeLstUsrAtt	] = {1074,-1,TabUnk,ActSeeAllAtt	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Att_ListUsrsAttendanceCrs	,NULL},
   [ActPrnLstUsrAtt	] = {1075,-1,TabUnk,ActSeeAllAtt	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_NEW_TAB,NULL				,Att_PrintUsrsAttendanceCrs	,NULL},
   [ActFrmNewAtt	] = {1063,-1,TabUnk,ActSeeAllAtt	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Att_ReqCreatOrEditEvent	,NULL},
   [ActEdiOneAtt	] = {1064,-1,TabUnk,ActSeeAllAtt	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Att_ReqCreatOrEditEvent	,NULL},
   [ActNewAtt		] = {1065,-1,TabUnk,ActSeeAllAtt	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Att_ReceiveFormEvent		,NULL},
   [ActChgAtt		] = {1066,-1,TabUnk,ActSeeAllAtt	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Att_ReceiveFormEvent		,NULL},
   [ActReqRemAtt	] = {1067,-1,TabUnk,ActSeeAllAtt	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Att_AskRemEvent		,NULL},
   [ActRemAtt		] = {1068,-1,TabUnk,ActSeeAllAtt	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Att_GetAndRemEvent		,NULL},
   [ActHidAtt		] = {1069,-1,TabUnk,ActSeeAllAtt	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Att_HideEvent			,NULL},
   [ActUnhAtt		] = {1070,-1,TabUnk,ActSeeAllAtt	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Att_UnhideEvent		,NULL},
   [ActSeeOneAtt	] = {1071,-1,TabUnk,ActSeeAllAtt	,0x238,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Att_SeeOneEvent		,NULL},
   [ActReqLnkAtt	] = {1938,-1,TabUnk,ActSeeAllAtt	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,AttRsc_GetLinkToEvent		,NULL},
   [ActRecAttStd	] = {1072,-1,TabUnk,ActSeeAllAtt	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Att_RegisterStudentsInEvent	,NULL},
   [ActRecAttMe		] = {1076,-1,TabUnk,ActSeeAllAtt	,0x008,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Att_RegisterMeAsStdInEvent	,NULL},

   [ActSignUp		] = {1056,-1,TabUnk,ActReqSignUp	,    0,0x006,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Enr_SignUpInCrs		,NULL},
   [ActUpdSignUpReq	] = {1522,-1,TabUnk,ActSeeSignUpReq	,0x3E0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Enr_UpdateEnrolmentRequests	,NULL},
   [ActReqRejSignUp	] = {1058,-1,TabUnk,ActSeeSignUpReq	,0x3E0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Enr_AskIfRejectSignUp		,NULL},
   [ActRejSignUp	] = {1059,-1,TabUnk,ActSeeSignUpReq	,0x3E0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Enr_RejectSignUp		,NULL},

   [ActReqMdfOneOth	] = {1414,-1,TabUnk,ActLstOth		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Enr_ReqRegRemOth		,NULL},
   [ActReqMdfOneStd	] = {1415,-1,TabUnk,ActLstStd		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Enr_ReqRegRemStd		,NULL},
   [ActReqMdfOneTch	] = {1416,-1,TabUnk,ActLstTch		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Enr_ReqRegRemTch		,NULL},

   [ActReqMdfOth	] = {1418,-1,TabUnk,ActLstOth		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Enr_AskIfRegRemAnotherOth	,NULL},
   [ActReqMdfStd	] = {1419,-1,TabUnk,ActLstStd		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Enr_AskIfRegRemAnotherStd	,NULL},
   [ActReqMdfNET	] = {1644,-1,TabUnk,ActLstTch		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Enr_AskIfRegRemAnotherTch	,NULL},
   [ActReqMdfTch	] = {1420,-1,TabUnk,ActLstTch		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Enr_AskIfRegRemAnotherTch	,NULL},

   [ActReqOthPho	] = {1432,-1,TabUnk,ActLstOth		,0x3E0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Pho_SendPhotoUsr		,NULL},
   [ActReqStdPho	] = {1433,-1,TabUnk,ActLstStd		,0x3E0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Pho_SendPhotoUsr		,NULL},
   [ActReqTchPho	] = {1434,-1,TabUnk,ActLstTch		,0x3E0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Pho_SendPhotoUsr		,NULL},

   [ActDetOthPho	] = {1435,-1,TabUnk,ActLstOth		,0x3E0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_DATA,Act_BRW_1ST_TAB,NULL				,Pho_RecOtherUsrPhotoDetFaces	,NULL},
   [ActDetStdPho	] = {1436,-1,TabUnk,ActLstStd		,0x3E0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_DATA,Act_BRW_1ST_TAB,NULL				,Pho_RecOtherUsrPhotoDetFaces	,NULL},
   [ActDetTchPho	] = {1437,-1,TabUnk,ActLstTch		,0x3E0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_DATA,Act_BRW_1ST_TAB,NULL				,Pho_RecOtherUsrPhotoDetFaces	,NULL},

   [ActChgOthPho	] = {1438,-1,TabUnk,ActLstOth		,0x3E0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,Pho_ChangeUsrPhoto1		,Pho_ChangeUsrPhoto2		,NULL},
   [ActChgStdPho	] = {1439,-1,TabUnk,ActLstStd		,0x3E0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,Pho_ChangeUsrPhoto1		,Pho_ChangeUsrPhoto2		,NULL},
   [ActChgTchPho	] = {1440,-1,TabUnk,ActLstTch		,0x3E0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,Pho_ChangeUsrPhoto1		,Pho_ChangeUsrPhoto2		,NULL},

   [ActReqRemOthPho	] = {1574,-1,TabUnk,ActLstOth		,0x3E0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Pho_ReqRemUsrPhoto		,NULL},
   [ActReqRemStdPho	] = {1575,-1,TabUnk,ActLstStd		,0x3E0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Pho_ReqRemUsrPhoto		,NULL},
   [ActReqRemTchPho	] = {1576,-1,TabUnk,ActLstTch		,0x3E0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Pho_ReqRemUsrPhoto		,NULL},
   [ActRemOthPho	] = {1441,-1,TabUnk,ActLstOth		,0x3E0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Pho_RemoveUsrPhoto		,NULL},
   [ActRemStdPho	] = {1442,-1,TabUnk,ActLstStd		,0x3E0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Pho_RemoveUsrPhoto		,NULL},
   [ActRemTchPho	] = {1443,-1,TabUnk,ActLstTch		,0x3E0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Pho_RemoveUsrPhoto		,NULL},

   [ActCreOth		] = {1444,-1,TabUnk,ActLstOth		,0x3E0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,Enr_CreateNewUsr1		,Enr_CreateNewUsr2		,NULL},
   [ActCreStd		] = {1445,-1,TabUnk,ActLstStd		,0x3E0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,Enr_CreateNewUsr1		,Enr_CreateNewUsr2		,NULL},
   [ActCreNET		] = {1645,-1,TabUnk,ActLstTch		,0x3E0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,Enr_CreateNewUsr1		,Enr_CreateNewUsr2		,NULL},
   [ActCreTch		] = {1446,-1,TabUnk,ActLstTch		,0x3E0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,Enr_CreateNewUsr1		,Enr_CreateNewUsr2		,NULL},

   [ActUpdOth		] = {1422,-1,TabUnk,ActLstOth		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,Enr_ModifyUsr1			,Enr_ModifyUsr2			,NULL},
   [ActUpdStd		] = {1423,-1,TabUnk,ActLstStd		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,Enr_ModifyUsr1			,Enr_ModifyUsr2			,NULL},
   [ActUpdNET		] = {1646,-1,TabUnk,ActLstTch		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,Enr_ModifyUsr1			,Enr_ModifyUsr2			,NULL},
   [ActUpdTch		] = {1424,-1,TabUnk,ActLstTch		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,Enr_ModifyUsr1			,Enr_ModifyUsr2			,NULL},

   [ActReqAccEnrStd	] = {1456,-1,TabUnk,ActLstStd		,0x3C8,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Enr_ReqAcceptRegisterInCrs	,NULL},
   [ActReqAccEnrNET	] = {1639,-1,TabUnk,ActLstTch		,0x3D0,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Enr_ReqAcceptRegisterInCrs	,NULL},
   [ActReqAccEnrTch	] = {1457,-1,TabUnk,ActLstTch		,0x3E0,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Enr_ReqAcceptRegisterInCrs	,NULL},
   [ActAccEnrStd	] = {1458,-1,TabUnk,ActLstStd		,0x3C8,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Enr_AcceptRegisterMeInCrs	,NULL},
   [ActAccEnrNET	] = {1640,-1,TabUnk,ActLstTch		,0x3D0,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Enr_AcceptRegisterMeInCrs	,NULL},
   [ActAccEnrTch	] = {1459,-1,TabUnk,ActLstTch		,0x3E0,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Enr_AcceptRegisterMeInCrs	,NULL},
   [ActRemMe_Std	] = {1460,-1,TabUnk,ActLstStd		,0x3C8,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Enr_ReqRemMeFromCrs		,NULL},
   [ActRemMe_NET	] = {1641,-1,TabUnk,ActLstTch		,0x3D0,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Enr_ReqRemMeFromCrs		,NULL},
   [ActRemMe_Tch	] = {1461,-1,TabUnk,ActLstTch		,0x3E0,    0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Enr_ReqRemMeFromCrs		,NULL},

   [ActNewAdmIns	] = {1337,-1,TabUnk,ActLstOth		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Adm_AddAdmToIns		,NULL},
   [ActRemAdmIns	] = {1338,-1,TabUnk,ActLstOth		,0x300,0x300,0x300,0x300,0x300,0x300,0x300,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Adm_RemAdmIns			,NULL},

   [ActNewAdmCtr	] = {1339,-1,TabUnk,ActLstOth		,0x300,0x300,0x300,0x300,0x300,0x300,0x300,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Adm_AddAdmToCtr		,NULL},
   [ActRemAdmCtr	] = {1340,-1,TabUnk,ActLstOth		,0x380,0x380,0x380,0x380,0x380,0x380,0x380,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Adm_RemAdmCtr			,NULL},

   [ActNewAdmDeg	] = { 586,-1,TabUnk,ActLstOth		,0x380,0x380,0x380,0x380,0x380,0x380,0x380,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Adm_AddAdmToDeg		,NULL},
   [ActRemAdmDeg	] = { 584,-1,TabUnk,ActLstOth		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Adm_RemAdmDeg			,NULL},

   [ActRcvFrmEnrSevStd	] = {1428,-1,TabUnk,ActLstStd		,0x3E0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Enr_ReceiveFormAdminStds	,NULL},
   [ActRcvFrmEnrSevNET	] = {1643,-1,TabUnk,ActLstTch		,0x3C0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Enr_ReceiveFormAdminNonEditTchs,NULL},
   [ActRcvFrmEnrSevTch	] = {1429,-1,TabUnk,ActLstTch		,0x3C0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Enr_ReceiveFormAdminTchs	,NULL},

   [ActCnfID_Oth	] = {1568,-1,TabUnk,ActLstOth		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,ID_ConfirmOtherUsrID		,NULL},
   [ActCnfID_Std	] = {1569,-1,TabUnk,ActLstStd		,0x3F0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,ID_ConfirmOtherUsrID		,NULL},
   [ActCnfID_Tch	] = {1570,-1,TabUnk,ActLstTch		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,ID_ConfirmOtherUsrID		,NULL},

   [ActFrmAccOth	] = {1735,-1,TabUnk,ActLstOth		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Acc_ShowFormChgOtherUsrAccount	,NULL},
   [ActFrmAccStd	] = {1736,-1,TabUnk,ActLstStd		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Acc_ShowFormChgOtherUsrAccount	,NULL},
   [ActFrmAccTch	] = {1737,-1,TabUnk,ActLstTch		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Acc_ShowFormChgOtherUsrAccount	,NULL},

   [ActRemNicOth	] = {1738,-1,TabUnk,ActLstOth		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Nck_RemoveOtherUsrNick		,NULL},
   [ActRemNicStd	] = {1739,-1,TabUnk,ActLstStd		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Nck_RemoveOtherUsrNick		,NULL},
   [ActRemNicTch	] = {1740,-1,TabUnk,ActLstTch		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Nck_RemoveOtherUsrNick		,NULL},
   [ActChgNicOth	] = {1741,-1,TabUnk,ActLstOth		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Nck_ChangeOtherUsrNick		,NULL},
   [ActChgNicStd	] = {1742,-1,TabUnk,ActLstStd		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Nck_ChangeOtherUsrNick		,NULL},
   [ActChgNicTch	] = {1743,-1,TabUnk,ActLstTch		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Nck_ChangeOtherUsrNick		,NULL},

   [ActRemID_Oth	] = {1450,-1,TabUnk,ActLstOth		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,ID_RemoveOtherUsrID		,NULL},
   [ActRemID_Std	] = {1451,-1,TabUnk,ActLstStd		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,ID_RemoveOtherUsrID		,NULL},
   [ActRemID_Tch	] = {1452,-1,TabUnk,ActLstTch		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,ID_RemoveOtherUsrID		,NULL},
   [ActChgID_Oth	] = {1453,-1,TabUnk,ActLstOth		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,ID_ChangeOtherUsrID		,NULL},
   [ActChgID_Std	] = {1454,-1,TabUnk,ActLstStd		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,ID_ChangeOtherUsrID		,NULL},
   [ActChgID_Tch	] = {1455,-1,TabUnk,ActLstTch		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,ID_ChangeOtherUsrID		,NULL},

   [ActChgPwdOth	] = {1467,-1,TabUnk,ActLstOth		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,Pwd_UpdateOtherUsrPwd		,Acc_ShowFormChgOtherUsrAccount	,NULL},
   [ActChgPwdStd	] = {1468,-1,TabUnk,ActLstStd		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,Pwd_UpdateOtherUsrPwd		,Acc_ShowFormChgOtherUsrAccount	,NULL},
   [ActChgPwdTch	] = {1469,-1,TabUnk,ActLstTch		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,Pwd_UpdateOtherUsrPwd		,Acc_ShowFormChgOtherUsrAccount	,NULL},

   [ActRemMaiOth	] = {1478,-1,TabUnk,ActLstOth		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Mai_RemoveOtherUsrEmail	,NULL},
   [ActRemMaiStd	] = {1479,-1,TabUnk,ActLstStd		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Mai_RemoveOtherUsrEmail	,NULL},
   [ActRemMaiTch	] = {1480,-1,TabUnk,ActLstTch		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Mai_RemoveOtherUsrEmail	,NULL},
   [ActChgMaiOth	] = {1481,-1,TabUnk,ActLstOth		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Mai_ChangeOtherUsrEmail	,NULL},
   [ActChgMaiStd	] = {1482,-1,TabUnk,ActLstStd		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Mai_ChangeOtherUsrEmail	,NULL},
   [ActChgMaiTch	] = {1483,-1,TabUnk,ActLstTch		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Mai_ChangeOtherUsrEmail	,NULL},

   [ActRemStdCrs	] = {1462,-1,TabUnk,ActLstStd		,0x3F8,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Enr_RemUsrFromCrs1		,Enr_RemUsrFromCrs2		,NULL},
   [ActRemNETCrs	] = {1647,-1,TabUnk,ActLstTch		,0x3F0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Enr_RemUsrFromCrs1		,Enr_RemUsrFromCrs2		,NULL},
   [ActRemTchCrs	] = {1463,-1,TabUnk,ActLstTch		,0x3E0,0x3C0,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Enr_RemUsrFromCrs1		,Enr_RemUsrFromCrs2		,NULL},

   [ActRemUsrGbl	] = {  62,-1,TabUnk,ActLstOth		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Acc_GetUsrCodAndRemUsrGbl	,NULL},

   [ActReqRemAllStdCrs	] = {  88,-1,TabUnk,ActLstStd		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Enr_AskRemAllStdsThisCrs	,NULL},
   [ActRemAllStdCrs	] = {  87,-1,TabUnk,ActLstStd		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Enr_RemAllStdsThisCrs		,NULL},

   [ActReqRemOldUsr	] = { 590,-1,TabUnk,ActLstOth		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Enr_AskRemoveOldUsrs		,NULL},
   [ActRemOldUsr	] = { 773,-1,TabUnk,ActLstOth		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Enr_RemoveOldUsrs		,NULL},

   [ActLstDupUsr	] = {1578,-1,TabUnk,ActLstOth		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Dup_ListDuplicateUsrs		,NULL},
   [ActLstSimUsr	] = {1579,-1,TabUnk,ActLstOth		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Dup_GetUsrCodAndListSimilarUsrs,NULL},
   [ActRemDupUsr	] = {1580,-1,TabUnk,ActLstOth		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Dup_RemoveUsrFromListDupUsrs	,NULL},

   [ActReqFolSevStd	] = {1756,-1,TabUnk,ActLstStd		,0x3F8,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Fol_ReqFollowStds		,NULL},
   [ActReqFolSevTch	] = {1757,-1,TabUnk,ActLstTch		,0x3F8,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Fol_ReqFollowTchs		,NULL},
   [ActReqUnfSevStd	] = {1758,-1,TabUnk,ActLstStd		,0x3F8,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Fol_ReqUnfollowStds		,NULL},
   [ActReqUnfSevTch	] = {1759,-1,TabUnk,ActLstTch		,0x3F8,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Fol_ReqUnfollowTchs		,NULL},
   [ActFolSevStd	] = {1760,-1,TabUnk,ActLstStd		,0x3F8,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Fol_FollowUsrs			,NULL},
   [ActFolSevTch	] = {1761,-1,TabUnk,ActLstTch		,0x3F8,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Fol_FollowUsrs			,NULL},
   [ActUnfSevStd	] = {1762,-1,TabUnk,ActLstStd		,0x3F8,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Fol_UnfollowUsrs		,NULL},
   [ActUnfSevTch	] = {1763,-1,TabUnk,ActLstTch		,0x3F8,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Fol_UnfollowUsrs		,NULL},

   // TabMsg ******************************************************************
   // Actions in menu:
   [ActSeeAnn		] = {1235, 0,TabMsg,ActSeeAnn		,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Ann_ShowAllAnnouncements	,"bullhorn"		},
   [ActSeeAllNot	] = { 762, 1,TabMsg,ActSeeAllNot	,0x3F8,0x3C7,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Not_ListFullNotices		,"sticky-note"		},
   [ActSeeFor		] = {  95, 2,TabMsg,ActSeeFor		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_ShowForumTheads		,"comments"		},
// [ActSeeChtRms	] = {  51, 3,TabMsg,ActSeeChtRms	,0x3F8,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Cht_ShowChatRooms		,"comments"		},
   [ActSeeChtRms	] = {  51, 3,TabMsg,ActSeeChtRms	,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Cht_ShowChatRooms		,"comments"		},
   [ActSeeRcvMsg	] = {   3, 4,TabMsg,ActSeeRcvMsg	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Msg_ShowRecMsgs		,"envelope"		},
   [ActReqMaiUsr	] = {1772, 5,TabMsg,ActReqMaiUsr	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Mai_ReqUsrsToListEmails	,"at"			},

   // Actions not in menu:
   [ActWriAnn		] = {1237,-1,TabUnk,ActSeeAnn		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Ann_ShowFormAnnouncement	,NULL},
   [ActRcvAnn		] = {1238,-1,TabUnk,ActSeeAnn		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Ann_ReceiveAnnouncement	,NULL},
   [ActHidAnn		] = {1470,-1,TabUnk,ActSeeAnn		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,Ann_HideActiveAnnouncement	,Ann_ShowAllAnnouncements	,NULL},
   [ActUnhAnn		] = {1471,-1,TabUnk,ActSeeAnn		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,Ann_RevealHiddenAnnouncement	,Ann_ShowAllAnnouncements	,NULL},
   [ActRemAnn		] = {1236,-1,TabUnk,ActSeeAnn		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Ann_RemoveAnnouncement		,NULL},

   [ActSeeOneNot	] = {1164,-1,TabUnk,ActSeeAllNot	,0x3F8,0x3C7,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Not_GetHighLightedNotCod	,Not_ListFullNotices		,NULL},
   [ActWriNot		] = {  59,-1,TabUnk,ActSeeAllNot	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Not_ShowFormNotice		,NULL},
   [ActRcvNot		] = {  60,-1,TabUnk,ActSeeAllNot	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Not_ReceiveNotice		,Not_ListFullNotices		,NULL},
   [ActHidNot		] = { 763,-1,TabUnk,ActSeeAllNot	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Not_HideActiveNotice		,Not_ListFullNotices		,NULL},
   [ActUnhNot		] = { 764,-1,TabUnk,ActSeeAllNot	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Not_RevealHiddenNotice		,Not_ListFullNotices		,NULL},
   [ActReqRemNot	] = {1472,-1,TabUnk,ActSeeAllNot	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Not_ReqRemNotice		,NULL},
   [ActRemNot		] = {  73,-1,TabUnk,ActSeeAllNot	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Not_RemoveNotice		,Not_ListNoticesAfterRemoval	,NULL},

   [ActSeeForCrsUsr	] = { 345,-1,TabUnk,ActSeeFor		,0x3F8,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_ShowForumTheads		,NULL},
   [ActSeeForCrsTch	] = { 431,-1,TabUnk,ActSeeFor		,0x230,0x204,0x204,0x204,0x204,0x204,0x204,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_ShowForumTheads		,NULL},
   [ActSeeForDegUsr	] = { 241,-1,TabUnk,ActSeeFor		,0x3F8,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_ShowForumTheads		,NULL},
   [ActSeeForDegTch	] = { 243,-1,TabUnk,ActSeeFor		,0x3F0,0x3C4,0x204,0x204,0x204,0x204,0x204,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_ShowForumTheads		,NULL},
   [ActSeeForCtrUsr	] = { 901,-1,TabUnk,ActSeeFor		,0x3F8,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_ShowForumTheads		,NULL},
   [ActSeeForCtrTch	] = { 430,-1,TabUnk,ActSeeFor		,0x3F0,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_ShowForumTheads		,NULL},
   [ActSeeForInsUsr	] = { 725,-1,TabUnk,ActSeeFor		,0x3F8,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_ShowForumTheads		,NULL},
   [ActSeeForInsTch	] = { 724,-1,TabUnk,ActSeeFor		,0x3F0,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_ShowForumTheads		,NULL},
   [ActSeeForGenUsr	] = { 726,-1,TabUnk,ActSeeFor		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_ShowForumTheads		,NULL},
   [ActSeeForGenTch	] = { 723,-1,TabUnk,ActSeeFor		,0x3F0,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_ShowForumTheads		,NULL},
   [ActSeeForSWAUsr	] = { 242,-1,TabUnk,ActSeeFor		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_ShowForumTheads		,NULL},
   [ActSeeForSWATch	] = { 245,-1,TabUnk,ActSeeFor		,0x3F0,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_ShowForumTheads		,NULL},

   [ActSeePstForCrsUsr	] = { 346,-1,TabUnk,ActSeeFor		,0x3F8,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_ShowThreadPosts		,NULL},
   [ActSeePstForCrsTch	] = { 347,-1,TabUnk,ActSeeFor		,0x230,0x204,0x204,0x204,0x204,0x204,0x204,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_ShowThreadPosts		,NULL},
   [ActSeePstForDegUsr	] = { 255,-1,TabUnk,ActSeeFor		,0x3F8,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_ShowThreadPosts		,NULL},
   [ActSeePstForDegTch	] = { 291,-1,TabUnk,ActSeeFor		,0x3F0,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_ShowThreadPosts		,NULL},
   [ActSeePstForCtrUsr	] = { 348,-1,TabUnk,ActSeeFor		,0x3F8,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_ShowThreadPosts		,NULL},
   [ActSeePstForCtrTch	] = { 902,-1,TabUnk,ActSeeFor		,0x3F0,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_ShowThreadPosts		,NULL},
   [ActSeePstForInsUsr	] = { 730,-1,TabUnk,ActSeeFor		,0x3F8,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_ShowThreadPosts		,NULL},
   [ActSeePstForInsTch	] = { 746,-1,TabUnk,ActSeeFor		,0x3F0,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_ShowThreadPosts		,NULL},
   [ActSeePstForGenUsr	] = { 727,-1,TabUnk,ActSeeFor		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_ShowThreadPosts		,NULL},
   [ActSeePstForGenTch	] = { 731,-1,TabUnk,ActSeeFor		,0x3F0,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_ShowThreadPosts		,NULL},
   [ActSeePstForSWAUsr	] = { 244,-1,TabUnk,ActSeeFor		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_ShowThreadPosts		,NULL},
   [ActSeePstForSWATch	] = { 246,-1,TabUnk,ActSeeFor		,0x3F0,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_ShowThreadPosts		,NULL},

   [ActRcvThrForCrsUsr	] = { 350,-1,TabUnk,ActSeeFor		,0x3F8,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_DATA,Act_BRW_1ST_TAB,NULL				,For_ReceiveForumPost		,NULL},
   [ActRcvThrForCrsTch	] = { 754,-1,TabUnk,ActSeeFor		,0x230,0x204,0x204,0x204,0x204,0x204,0x204,Act_CONT_DATA,Act_BRW_1ST_TAB,NULL				,For_ReceiveForumPost		,NULL},
   [ActRcvThrForDegUsr	] = { 252,-1,TabUnk,ActSeeFor		,0x3F8,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_DATA,Act_BRW_1ST_TAB,NULL				,For_ReceiveForumPost		,NULL},
   [ActRcvThrForDegTch	] = { 247,-1,TabUnk,ActSeeFor		,0x3F0,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_DATA,Act_BRW_1ST_TAB,NULL				,For_ReceiveForumPost		,NULL},
   [ActRcvThrForCtrUsr	] = { 903,-1,TabUnk,ActSeeFor		,0x3F8,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_DATA,Act_BRW_1ST_TAB,NULL				,For_ReceiveForumPost		,NULL},
   [ActRcvThrForCtrTch	] = { 904,-1,TabUnk,ActSeeFor		,0x3F0,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_DATA,Act_BRW_1ST_TAB,NULL				,For_ReceiveForumPost		,NULL},
   [ActRcvThrForInsUsr	] = { 737,-1,TabUnk,ActSeeFor		,0x3F8,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_DATA,Act_BRW_1ST_TAB,NULL				,For_ReceiveForumPost		,NULL},
   [ActRcvThrForInsTch	] = { 769,-1,TabUnk,ActSeeFor		,0x3F0,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_DATA,Act_BRW_1ST_TAB,NULL				,For_ReceiveForumPost		,NULL},
   [ActRcvThrForGenUsr	] = { 736,-1,TabUnk,ActSeeFor		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_DATA,Act_BRW_1ST_TAB,NULL				,For_ReceiveForumPost		,NULL},
   [ActRcvThrForGenTch	] = { 765,-1,TabUnk,ActSeeFor		,0x3F0,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_DATA,Act_BRW_1ST_TAB,NULL				,For_ReceiveForumPost		,NULL},
   [ActRcvThrForSWAUsr	] = { 258,-1,TabUnk,ActSeeFor		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_DATA,Act_BRW_1ST_TAB,NULL				,For_ReceiveForumPost		,NULL},
   [ActRcvThrForSWATch	] = { 259,-1,TabUnk,ActSeeFor		,0x3F0,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_DATA,Act_BRW_1ST_TAB,NULL				,For_ReceiveForumPost		,NULL},

   [ActRcvRepForCrsUsr	] = { 599,-1,TabUnk,ActSeeFor		,0x3F8,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_DATA,Act_BRW_1ST_TAB,NULL				,For_ReceiveForumPost		,NULL},
   [ActRcvRepForCrsTch	] = { 755,-1,TabUnk,ActSeeFor		,0x230,0x204,0x204,0x204,0x204,0x204,0x204,Act_CONT_DATA,Act_BRW_1ST_TAB,NULL				,For_ReceiveForumPost		,NULL},
   [ActRcvRepForDegUsr	] = { 606,-1,TabUnk,ActSeeFor		,0x3F8,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_DATA,Act_BRW_1ST_TAB,NULL				,For_ReceiveForumPost		,NULL},
   [ActRcvRepForDegTch	] = { 617,-1,TabUnk,ActSeeFor		,0x3F0,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_DATA,Act_BRW_1ST_TAB,NULL				,For_ReceiveForumPost		,NULL},
   [ActRcvRepForCtrUsr	] = { 905,-1,TabUnk,ActSeeFor		,0x3F8,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_DATA,Act_BRW_1ST_TAB,NULL				,For_ReceiveForumPost		,NULL},
   [ActRcvRepForCtrTch	] = { 906,-1,TabUnk,ActSeeFor		,0x3F0,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_DATA,Act_BRW_1ST_TAB,NULL				,For_ReceiveForumPost		,NULL},
   [ActRcvRepForInsUsr	] = { 740,-1,TabUnk,ActSeeFor		,0x3F8,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_DATA,Act_BRW_1ST_TAB,NULL				,For_ReceiveForumPost		,NULL},
   [ActRcvRepForInsTch	] = { 770,-1,TabUnk,ActSeeFor		,0x3F0,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_DATA,Act_BRW_1ST_TAB,NULL				,For_ReceiveForumPost		,NULL},
   [ActRcvRepForGenUsr	] = { 747,-1,TabUnk,ActSeeFor		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_DATA,Act_BRW_1ST_TAB,NULL				,For_ReceiveForumPost		,NULL},
   [ActRcvRepForGenTch	] = { 816,-1,TabUnk,ActSeeFor		,0x3F0,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_DATA,Act_BRW_1ST_TAB,NULL				,For_ReceiveForumPost		,NULL},
   [ActRcvRepForSWAUsr	] = { 603,-1,TabUnk,ActSeeFor		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_DATA,Act_BRW_1ST_TAB,NULL				,For_ReceiveForumPost		,NULL},
   [ActRcvRepForSWATch	] = { 622,-1,TabUnk,ActSeeFor		,0x3F0,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_DATA,Act_BRW_1ST_TAB,NULL				,For_ReceiveForumPost		,NULL},

   [ActReqDelThrCrsUsr	] = { 867,-1,TabUnk,ActSeeFor		,0x220,0x204,0x204,0x204,0x204,0x204,0x204,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_ReqRemThread		,NULL},
   [ActReqDelThrCrsTch	] = { 869,-1,TabUnk,ActSeeFor		,0x220,0x204,0x204,0x204,0x204,0x204,0x204,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_ReqRemThread		,NULL},
   [ActReqDelThrDegUsr	] = { 907,-1,TabUnk,ActSeeFor		,0x3C0,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_ReqRemThread		,NULL},
   [ActReqDelThrDegTch	] = { 908,-1,TabUnk,ActSeeFor		,0x3C0,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_ReqRemThread		,NULL},
   [ActReqDelThrCtrUsr	] = { 909,-1,TabUnk,ActSeeFor		,0x3C0,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_ReqRemThread		,NULL},
   [ActReqDelThrCtrTch	] = { 910,-1,TabUnk,ActSeeFor		,0x3C0,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_ReqRemThread		,NULL},
   [ActReqDelThrInsUsr	] = { 911,-1,TabUnk,ActSeeFor		,0x200,0x204,0x204,0x204,0x204,0x204,0x204,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_ReqRemThread		,NULL},
   [ActReqDelThrInsTch	] = { 912,-1,TabUnk,ActSeeFor		,0x200,0x204,0x204,0x204,0x204,0x204,0x204,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_ReqRemThread		,NULL},
   [ActReqDelThrGenUsr	] = { 913,-1,TabUnk,ActSeeFor		,0x200,0x204,0x204,0x204,0x204,0x204,0x204,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_ReqRemThread		,NULL},
   [ActReqDelThrGenTch	] = { 914,-1,TabUnk,ActSeeFor		,0x200,0x204,0x204,0x204,0x204,0x204,0x204,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_ReqRemThread		,NULL},
   [ActReqDelThrSWAUsr	] = { 881,-1,TabUnk,ActSeeFor		,0x200,0x204,0x204,0x204,0x204,0x204,0x204,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_ReqRemThread		,NULL},
   [ActReqDelThrSWATch	] = { 915,-1,TabUnk,ActSeeFor		,0x200,0x204,0x204,0x204,0x204,0x204,0x204,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_ReqRemThread		,NULL},

   [ActDelThrForCrsUsr	] = { 868,-1,TabUnk,ActSeeFor		,0x220,0x204,0x204,0x204,0x204,0x204,0x204,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_RemoveThread		,NULL},
   [ActDelThrForCrsTch	] = { 876,-1,TabUnk,ActSeeFor		,0x220,0x204,0x204,0x204,0x204,0x204,0x204,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_RemoveThread		,NULL},
   [ActDelThrForDegUsr	] = { 916,-1,TabUnk,ActSeeFor		,0x3C0,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_RemoveThread		,NULL},
   [ActDelThrForDegTch	] = { 917,-1,TabUnk,ActSeeFor		,0x3C0,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_RemoveThread		,NULL},
   [ActDelThrForCtrUsr	] = { 918,-1,TabUnk,ActSeeFor		,0x3C0,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_RemoveThread		,NULL},
   [ActDelThrForCtrTch	] = { 919,-1,TabUnk,ActSeeFor		,0x3C0,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_RemoveThread		,NULL},
   [ActDelThrForInsUsr	] = { 920,-1,TabUnk,ActSeeFor		,0x200,0x204,0x204,0x204,0x204,0x204,0x204,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_RemoveThread		,NULL},
   [ActDelThrForInsTch	] = { 921,-1,TabUnk,ActSeeFor		,0x200,0x204,0x204,0x204,0x204,0x204,0x204,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_RemoveThread		,NULL},
   [ActDelThrForGenUsr	] = { 922,-1,TabUnk,ActSeeFor		,0x200,0x204,0x204,0x204,0x204,0x204,0x204,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_RemoveThread		,NULL},
   [ActDelThrForGenTch	] = { 923,-1,TabUnk,ActSeeFor		,0x200,0x204,0x204,0x204,0x204,0x204,0x204,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_RemoveThread		,NULL},
   [ActDelThrForSWAUsr	] = { 882,-1,TabUnk,ActSeeFor		,0x200,0x204,0x204,0x204,0x204,0x204,0x204,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_RemoveThread		,NULL},
   [ActDelThrForSWATch	] = { 924,-1,TabUnk,ActSeeFor		,0x200,0x204,0x204,0x204,0x204,0x204,0x204,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_RemoveThread		,NULL},

   [ActCutThrForCrsUsr	] = { 926,-1,TabUnk,ActSeeFor		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_CutThread			,NULL},
   [ActCutThrForCrsTch	] = { 927,-1,TabUnk,ActSeeFor		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_CutThread			,NULL},
   [ActCutThrForDegUsr	] = { 928,-1,TabUnk,ActSeeFor		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_CutThread			,NULL},
   [ActCutThrForDegTch	] = { 929,-1,TabUnk,ActSeeFor		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_CutThread			,NULL},
   [ActCutThrForCtrUsr	] = { 930,-1,TabUnk,ActSeeFor		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_CutThread			,NULL},
   [ActCutThrForCtrTch	] = { 931,-1,TabUnk,ActSeeFor		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_CutThread			,NULL},
   [ActCutThrForInsUsr	] = { 932,-1,TabUnk,ActSeeFor		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_CutThread			,NULL},
   [ActCutThrForInsTch	] = { 933,-1,TabUnk,ActSeeFor		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_CutThread			,NULL},
   [ActCutThrForGenUsr	] = { 934,-1,TabUnk,ActSeeFor		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_CutThread			,NULL},
   [ActCutThrForGenTch	] = { 935,-1,TabUnk,ActSeeFor		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_CutThread			,NULL},
   [ActCutThrForSWAUsr	] = { 890,-1,TabUnk,ActSeeFor		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_CutThread			,NULL},
   [ActCutThrForSWATch	] = { 936,-1,TabUnk,ActSeeFor		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_CutThread			,NULL},

   [ActPasThrForCrsUsr	] = { 891,-1,TabUnk,ActSeeFor		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_PasteThread		,NULL},
   [ActPasThrForCrsTch	] = { 937,-1,TabUnk,ActSeeFor		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_PasteThread		,NULL},
   [ActPasThrForDegUsr	] = { 938,-1,TabUnk,ActSeeFor		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_PasteThread		,NULL},
   [ActPasThrForDegTch	] = { 939,-1,TabUnk,ActSeeFor		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_PasteThread		,NULL},
   [ActPasThrForCtrUsr	] = { 940,-1,TabUnk,ActSeeFor		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_PasteThread		,NULL},
   [ActPasThrForCtrTch	] = { 941,-1,TabUnk,ActSeeFor		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_PasteThread		,NULL},
   [ActPasThrForInsUsr	] = { 942,-1,TabUnk,ActSeeFor		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_PasteThread		,NULL},
   [ActPasThrForInsTch	] = { 943,-1,TabUnk,ActSeeFor		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_PasteThread		,NULL},
   [ActPasThrForGenUsr	] = { 944,-1,TabUnk,ActSeeFor		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_PasteThread		,NULL},
   [ActPasThrForGenTch	] = { 945,-1,TabUnk,ActSeeFor		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_PasteThread		,NULL},
   [ActPasThrForSWAUsr	] = { 946,-1,TabUnk,ActSeeFor		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_PasteThread		,NULL},
   [ActPasThrForSWATch	] = { 947,-1,TabUnk,ActSeeFor		,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_PasteThread		,NULL},

   [ActDelPstForCrsUsr	] = { 602,-1,TabUnk,ActSeeFor		,0x238,0x204,0x204,0x204,0x204,0x204,0x204,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_RemovePost			,NULL},
   [ActDelPstForCrsTch	] = { 756,-1,TabUnk,ActSeeFor		,0x230,0x204,0x204,0x204,0x204,0x204,0x204,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_RemovePost			,NULL},
   [ActDelPstForDegUsr	] = { 608,-1,TabUnk,ActSeeFor		,0x3F8,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_RemovePost			,NULL},
   [ActDelPstForDegTch	] = { 680,-1,TabUnk,ActSeeFor		,0x3F0,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_RemovePost			,NULL},
   [ActDelPstForCtrUsr	] = { 948,-1,TabUnk,ActSeeFor		,0x3F8,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_RemovePost			,NULL},
   [ActDelPstForCtrTch	] = { 949,-1,TabUnk,ActSeeFor		,0x3F0,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_RemovePost			,NULL},
   [ActDelPstForInsUsr	] = { 743,-1,TabUnk,ActSeeFor		,0x3F8,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_RemovePost			,NULL},
   [ActDelPstForInsTch	] = { 772,-1,TabUnk,ActSeeFor		,0x3F0,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_RemovePost			,NULL},
   [ActDelPstForGenUsr	] = { 735,-1,TabUnk,ActSeeFor		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_RemovePost			,NULL},
   [ActDelPstForGenTch	] = { 950,-1,TabUnk,ActSeeFor		,0x3F0,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_RemovePost			,NULL},
   [ActDelPstForSWAUsr	] = { 613,-1,TabUnk,ActSeeFor		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_RemovePost			,NULL},
   [ActDelPstForSWATch	] = { 623,-1,TabUnk,ActSeeFor		,0x3F0,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_RemovePost			,NULL},

   [ActEnbPstForCrsUsr	] = { 624,-1,TabUnk,ActSeeFor		,0x220,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_EnablePost			,NULL},
   [ActEnbPstForCrsTch	] = { 951,-1,TabUnk,ActSeeFor		,0x220,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_EnablePost			,NULL},
   [ActEnbPstForDegUsr	] = { 616,-1,TabUnk,ActSeeFor		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_EnablePost			,NULL},
   [ActEnbPstForDegTch	] = { 619,-1,TabUnk,ActSeeFor		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_EnablePost			,NULL},
   [ActEnbPstForCtrUsr	] = { 952,-1,TabUnk,ActSeeFor		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_EnablePost			,NULL},
   [ActEnbPstForCtrTch	] = { 953,-1,TabUnk,ActSeeFor		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_EnablePost			,NULL},
   [ActEnbPstForInsUsr	] = { 954,-1,TabUnk,ActSeeFor		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_EnablePost			,NULL},
   [ActEnbPstForInsTch	] = { 955,-1,TabUnk,ActSeeFor		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_EnablePost			,NULL},
   [ActEnbPstForGenUsr	] = { 956,-1,TabUnk,ActSeeFor		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_EnablePost			,NULL},
   [ActEnbPstForGenTch	] = { 957,-1,TabUnk,ActSeeFor		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_EnablePost			,NULL},
   [ActEnbPstForSWAUsr	] = { 632,-1,TabUnk,ActSeeFor		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_EnablePost			,NULL},
   [ActEnbPstForSWATch	] = { 634,-1,TabUnk,ActSeeFor		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_EnablePost			,NULL},

   [ActDisPstForCrsUsr	] = { 610,-1,TabUnk,ActSeeFor		,0x220,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_DisablePost		,NULL},
   [ActDisPstForCrsTch	] = { 958,-1,TabUnk,ActSeeFor		,0x220,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_DisablePost		,NULL},
   [ActDisPstForDegUsr	] = { 615,-1,TabUnk,ActSeeFor		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_DisablePost		,NULL},
   [ActDisPstForDegTch	] = { 618,-1,TabUnk,ActSeeFor		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_DisablePost		,NULL},
   [ActDisPstForCtrUsr	] = { 959,-1,TabUnk,ActSeeFor		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_DisablePost		,NULL},
   [ActDisPstForCtrTch	] = { 960,-1,TabUnk,ActSeeFor		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_DisablePost		,NULL},
   [ActDisPstForInsUsr	] = { 961,-1,TabUnk,ActSeeFor		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_DisablePost		,NULL},
   [ActDisPstForInsTch	] = { 962,-1,TabUnk,ActSeeFor		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_DisablePost		,NULL},
   [ActDisPstForGenUsr	] = { 963,-1,TabUnk,ActSeeFor		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_DisablePost		,NULL},
   [ActDisPstForGenTch	] = { 925,-1,TabUnk,ActSeeFor		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_DisablePost		,NULL},
   [ActDisPstForSWAUsr	] = { 625,-1,TabUnk,ActSeeFor		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_DisablePost		,NULL},
   [ActDisPstForSWATch	] = { 635,-1,TabUnk,ActSeeFor		,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,For_DisablePost		,NULL},

   [ActReqLnkForCrsUsr  ] = {1941,-1,TabUnk,ActSeeFor		,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,ForRsc_GetLinkToThread		,NULL},

// [ActCht		] = {  52,-1,TabUnk,ActSeeChtRms	,0x3F8,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_BRW_NEW_TAB,Cht_OpenChatWindow		,NULL				,NULL},
   [ActCht		] = {  52,-1,TabUnk,ActSeeChtRms	,0x200,0x200,0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_BRW_NEW_TAB,Cht_OpenChatWindow		,NULL				,NULL},

   [ActReqMsgUsr	] = {  26,-1,TabUnk,ActSeeRcvMsg	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Msg_FormMsgUsrs		,NULL},
   [ActSeeSntMsg	] = {  70,-1,TabUnk,ActSeeRcvMsg	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Msg_ShowSntMsgs		,NULL},

   [ActRcvMsgUsr	] = {  27,-1,TabUnk,ActSeeRcvMsg	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_DATA,Act_BRW_1ST_TAB,NULL				,Msg_RecMsgFromUsr		,NULL},
   [ActReqDelAllSntMsg	] = { 604,-1,TabUnk,ActSeeRcvMsg	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Msg_ReqDelAllSntMsgs		,NULL},
   [ActReqDelAllRcvMsg	] = { 593,-1,TabUnk,ActSeeRcvMsg	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Msg_ReqDelAllRecMsgs		,NULL},
   [ActDelAllSntMsg	] = { 434,-1,TabUnk,ActSeeRcvMsg	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Msg_DelAllSntMsgs		,NULL},
   [ActDelAllRcvMsg	] = { 436,-1,TabUnk,ActSeeRcvMsg	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Msg_DelAllRecMsgs		,NULL},
   [ActDelSntMsg	] = {  90,-1,TabUnk,ActSeeRcvMsg	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Msg_DelSntMsg			,NULL},
   [ActDelRcvMsg	] = {  64,-1,TabUnk,ActSeeRcvMsg	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Msg_DelRecMsg			,NULL},
   [ActExpSntMsg	] = { 664,-1,TabUnk,ActSeeRcvMsg	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Msg_ExpSntMsg			,NULL},
   [ActExpRcvMsg	] = { 663,-1,TabUnk,ActSeeRcvMsg	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Msg_ExpRecMsg			,NULL},
   [ActConSntMsg	] = {1020,-1,TabUnk,ActSeeRcvMsg	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Msg_ConSntMsg			,NULL},
   [ActConRcvMsg	] = {1019,-1,TabUnk,ActSeeRcvMsg	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Msg_ConRecMsg			,NULL},

   [ActLstBanUsr	] = {1017,-1,TabUnk,ActSeeRcvMsg	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Msg_ListBannedUsrs		,NULL},
   [ActBanUsrMsg	] = {1015,-1,TabUnk,ActSeeRcvMsg	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Msg_BanSenderWhenShowingMsgs	,NULL},
   [ActUnbUsrMsg	] = {1016,-1,TabUnk,ActSeeRcvMsg	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Msg_UnbanSenderWhenShowingMsgs	,NULL},
   [ActUnbUsrLst	] = {1018,-1,TabUnk,ActSeeRcvMsg	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Msg_UnbanSenderWhenListingUsrs	,NULL},

   [ActMaiUsr		] = { 100,-1,TabUnk,ActReqMaiUsr	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Mai_GetSelUsrsAndListEmails	,NULL},

   // TabAna ******************************************************************
   // Actions in menu:
   [ActReqUseGbl	] = { 761, 0,TabAna,ActReqUseGbl	,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Fig_ReqShowFigures		,"chart-pie"		},
   [ActSeePhoDeg	] = { 447, 1,TabAna,ActSeePhoDeg	,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Pho_ShowPhotoDegree		,"graduation-cap"	},
   [ActReqStaCrs	] = { 767, 2,TabAna,ActReqStaCrs	,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Ind_ReqIndicatorsCourses	,"tasks"		},
   [ActSeeAllSvy	] = { 966, 3,TabAna,ActSeeAllSvy	,0x3F8,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Svy_SeeAllSurveys		,"poll"			},
   [ActReqAccGbl	] = { 591, 4,TabAna,ActReqAccGbl	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,Dat_SetIniEndDatesToRecentWeeks,Sta_AskShowGblHits		,"chart-line"		},
   [ActReqMyUsgRep	] = {1586, 5,TabAna,ActReqMyUsgRep	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Rep_ReqMyUsageReport		,"file-alt"		},
   [ActMFUAct		] = { 993, 6,TabAna,ActMFUAct		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,MFU_ShowMyMFUActions		,"heart"		},

   // Actions not in menu:
   [ActSeeUseGbl	] = {  84,-1,TabUnk,ActReqUseGbl	,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Fig_ShowFigures		,NULL},
   [ActPrnPhoDeg	] = { 448,-1,TabUnk,ActSeePhoDeg	,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_BRW_NEW_TAB,NULL				,Pho_PrintPhotoDegree  		,NULL},
   [ActCalPhoDeg	] = { 444,-1,TabUnk,ActSeePhoDeg	,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Pho_CalcPhotoDegree		,NULL},

   [ActSeeAllStaCrs	] = { 768,-1,TabUnk,ActReqAccGbl	,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_BRW_NEW_TAB,NULL				,Ind_ShowIndicatorsCourses	,NULL},

   [ActSeeOneSvy	] = { 982,-1,TabUnk,ActSeeAllSvy	,0x3F8,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Svy_SeeOneSurvey		,NULL},
   [ActAnsSvy		] = { 983,-1,TabUnk,ActSeeAllSvy	,0x3F8,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Svy_ReceiveSurveyAnswers	,NULL},
   [ActFrmNewSvy	] = { 973,-1,TabUnk,ActSeeAllSvy	,0x3E0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Svy_ReqCreatOrEditSvy		,NULL},
   [ActEdiOneSvy	] = { 974,-1,TabUnk,ActSeeAllSvy	,0x3E0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Svy_ReqCreatOrEditSvy		,NULL},
   [ActNewSvy		] = { 968,-1,TabUnk,ActSeeAllSvy	,0x3E0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Svy_ReceiveFormSurvey		,NULL},
   [ActChgSvy		] = { 975,-1,TabUnk,ActSeeAllSvy	,0x3E0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Svy_ReceiveFormSurvey		,NULL},
   [ActReqRemSvy	] = { 976,-1,TabUnk,ActSeeAllSvy	,0x3E0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Svy_AskRemSurvey		,NULL},
   [ActRemSvy		] = { 969,-1,TabUnk,ActSeeAllSvy	,0x3E0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Svy_RemoveSurvey		,NULL},
   [ActReqRstSvy	] = { 984,-1,TabUnk,ActSeeAllSvy	,0x3E0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Svy_AskResetSurvey		,NULL},
   [ActRstSvy		] = { 985,-1,TabUnk,ActSeeAllSvy	,0x3E0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Svy_ResetSurvey		,NULL},
   [ActHidSvy		] = { 977,-1,TabUnk,ActSeeAllSvy	,0x3E0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Svy_HideSurvey			,NULL},
   [ActUnhSvy		] = { 978,-1,TabUnk,ActSeeAllSvy	,0x3E0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Svy_UnhideSurvey		,NULL},
   [ActReqLnkSvy	] = {1937,-1,TabUnk,ActSeeAllSvy	,0x220,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,SvyRsc_GetLinkToSurvey		,NULL},
   [ActEdiOneSvyQst	] = { 979,-1,TabUnk,ActSeeAllSvy	,0x3E0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Svy_ReqEditQuestion		,NULL},
   [ActRcvSvyQst	] = { 980,-1,TabUnk,ActSeeAllSvy	,0x3E0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Svy_ReceiveQst			,NULL},
   [ActReqRemSvyQst	] = {1524,-1,TabUnk,ActSeeAllSvy	,0x3E0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Svy_ReqRemQst			,NULL},
   [ActRemSvyQst	] = { 981,-1,TabUnk,ActSeeAllSvy	,0x3E0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Svy_RemoveQst			,NULL},

   [ActSeeAccGbl	] = {  79,-1,TabUnk,ActReqAccGbl	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Sta_SeeGblAccesses		,NULL},
   [ActReqAccCrs	] = { 594,-1,TabUnk,ActReqAccGbl	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,Dat_SetIniEndDatesToRecentWeeks,Sta_AskShowCrsHits		,NULL},
   [ActSeeAccCrs	] = { 119,-1,TabUnk,ActReqAccGbl	,0x230,0x200,    0,    0,    0,    0,    0,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Sta_SeeCrsAccesses		,NULL},

   [ActLstClk		] = { 989,-1,TabUnk,ActReqAccGbl	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Log_ShowLastClicks		,NULL},
   [ActRefLstClk	] = { 994,-1,TabUnk,ActRefLstClk	,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_AJAX_RFRESH,NULL				,Lay_RefreshLastClicks		,NULL},

   [ActSeeMyUsgRep	] = {1582,-1,TabUnk,ActReqMyUsgRep	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Rep_ShowMyUsageReport		,NULL},

   // TabPrf ******************************************************************
   // Actions in menu:
   [ActFrmRolSes	] = { 843, 0,TabPrf,ActFrmRolSes	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Usr_WriteFormLogout		,"power-off"		},
   [ActMyCrs		] = { 987, 1,TabPrf,ActMyCrs		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Crs_ReqSelectOneOfMyCourses	,"sitemap"		},
   [ActSeeMyTT		] = { 408, 2,TabPrf,ActSeeMyTT		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Tmt_ShowClassTimeTable		,"clock"		},
   [ActSeeMyAgd		] = {1602, 3,TabPrf,ActSeeMyAgd		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Agd_GetParsAndShowMyAgenda	,"calendar"		},
   [ActFrmMyAcc		] = {  36, 4,TabPrf,ActFrmMyAcc		,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Acc_ShowFormMyAccount		,"at"			},
   [ActReqEdiRecSha	] = { 285, 5,TabPrf,ActReqEdiRecSha	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Rec_ShowMySharedRecordAndMore	,"address-card"		},
   [ActReqEdiSet	] = { 673, 6,TabPrf,ActReqEdiSet	,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Set_EditSettings		,"cog"			},

   // Actions not in menu:
   [ActChgMyRol		] = { 589,-1,TabUnk,ActFrmRolSes	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,Rol_ChangeMyRole		,Usr_ShowFormsLogoutAndRole	,NULL},

   [ActFrmNewEvtMyAgd	] = {1603,-1,TabUnk,ActSeeMyAgd		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Agd_ReqCreatOrEditEvent	,NULL},
   [ActEdiOneEvtMyAgd	] = {1604,-1,TabUnk,ActSeeMyAgd		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Agd_ReqCreatOrEditEvent	,NULL},
   [ActNewEvtMyAgd	] = {1605,-1,TabUnk,ActSeeMyAgd		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Agd_ReceiveFormEvent		,NULL},
   [ActChgEvtMyAgd	] = {1606,-1,TabUnk,ActSeeMyAgd		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Agd_ReceiveFormEvent		,NULL},
   [ActReqRemEvtMyAgd	] = {1607,-1,TabUnk,ActSeeMyAgd		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Agd_AskRemEvent		,NULL},
   [ActRemEvtMyAgd	] = {1608,-1,TabUnk,ActSeeMyAgd		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Agd_RemoveEvent		,NULL},
   [ActHidEvtMyAgd	] = {1612,-1,TabUnk,ActSeeMyAgd		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Agd_HideEvent			,NULL},
   [ActUnhEvtMyAgd	] = {1613,-1,TabUnk,ActSeeMyAgd		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Agd_UnhideEvent		,NULL},
   [ActPrvEvtMyAgd	] = {1609,-1,TabUnk,ActSeeMyAgd		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Agd_MakeEventPrivate		,NULL},
   [ActPubEvtMyAgd	] = {1610,-1,TabUnk,ActSeeMyAgd		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Agd_MakeEventPublic		,NULL},
   [ActPrnAgdQR		] = {1618,-1,TabUnk,ActSeeMyAgd		,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_BRW_NEW_TAB,NULL				,Agd_PrintAgdQRCode		,NULL},

   [ActChkUsrAcc	] = {1584,-1,TabUnk,ActFrmMyAcc		,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Acc_CheckIfEmptyAccountExists	,NULL},
   [ActCreMyAcc		] = {1787,-1,TabUnk,ActFrmMyAcc		,0x001,0x001,0x001,0x001,0x001,0x001,0x001,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Acc_ShowFormCreateMyAccount	,NULL},
   [ActCreUsrAcc	] = {1163,-1,TabUnk,ActFrmMyAcc		,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Acc_AfterCreationNewAccount	,NULL},

   [ActRemMyID		] = {1147,-1,TabUnk,ActFrmMyAcc		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,ID_RemoveMyUsrID		,NULL},
   [ActChgMyID		] = {1148,-1,TabUnk,ActFrmMyAcc		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,ID_NewMyUsrID			,NULL},

   [ActRemMyNck		] = {1089,-1,TabUnk,ActFrmMyAcc		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Nck_RemoveMyNick		,NULL},
   [ActChgMyNck		] = {  37,-1,TabUnk,ActFrmMyAcc		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Nck_UpdateMyNick		,NULL},

   [ActRemMyMai		] = {1090,-1,TabUnk,ActFrmMyAcc		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Mai_RemoveMyUsrEmail		,NULL},
   [ActChgMyMai		] = {1088,-1,TabUnk,ActFrmMyAcc		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,May_NewMyUsrEmail		,NULL},
   [ActCnfMai		] = {1091,-1,TabUnk,ActFrmMyAcc		,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Mai_ConfirmEmail		,NULL},

   [ActChgMyPwd		] = {  35,-1,TabUnk,ActFrmMyAcc		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,Pwd_UpdateMyPwd		,Acc_ShowFormChgMyAccount	,NULL},

   [ActReqRemMyAcc	] = {1430,-1,TabUnk,ActFrmMyAcc		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Acc_AskIfRemoveMyAccount	,NULL},
   [ActRemMyAcc		] = {1431,-1,TabUnk,ActFrmMyAcc		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Acc_RemoveMyAccount		,NULL},

   [ActChgMyData	] = { 298,-1,TabUnk,ActReqEdiRecSha	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,Rec_UpdateMyRecord		,Rec_ShowMySharedRecordUpd	,NULL},

   [ActReqMyPho		] = {  30,-1,TabUnk,ActReqEdiRecSha	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Pho_ReqMyPhoto			,NULL},
   [ActDetMyPho		] = { 693,-1,TabUnk,ActReqEdiRecSha	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_DATA,Act_BRW_1ST_TAB,NULL				,Pho_RecMyPhotoDetFaces		,NULL},
   [ActUpdMyPho		] = { 694,-1,TabUnk,ActReqEdiRecSha	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,Pho_UpdateMyPhoto1		,Pho_UpdateMyPhoto2		,NULL},
   [ActReqRemMyPho	] = {1577,-1,TabUnk,ActReqEdiRecSha	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Pho_ReqRemMyPhoto		,NULL},
   [ActRemMyPho		] = { 428,-1,TabUnk,ActReqEdiRecSha	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,Pho_RemoveMyPhoto1		,Pho_RemoveMyPhoto2		,NULL},

   [ActChgCtyMyIns	] = {1166,-1,TabUnk,ActReqEdiRecSha	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Rec_ChgCountryOfMyInstitution	,NULL},
   [ActChgMyIns		] = {1167,-1,TabUnk,ActReqEdiRecSha	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Rec_UpdateMyInstitution	,NULL},
   [ActChgMyCtr		] = {1168,-1,TabUnk,ActReqEdiRecSha	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Rec_UpdateMyCenter		,NULL},
   [ActChgMyDpt		] = {1169,-1,TabUnk,ActReqEdiRecSha	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Rec_UpdateMyDepartment		,NULL},
   [ActChgMyOff		] = {1170,-1,TabUnk,ActReqEdiRecSha	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Rec_UpdateMyOffice		,NULL},
   [ActChgMyOffPho	] = {1171,-1,TabUnk,ActReqEdiRecSha	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Rec_UpdateMyOfficePhone	,NULL},

   [ActReqEdiMyNet	] = {1172,-1,TabUnk,ActReqEdiRecSha	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Rec_ShowMySharedRecordAndMore	,NULL},
   [ActChgMyNet		] = {1173,-1,TabUnk,ActReqEdiRecSha	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Net_UpdateMyWebsAndSocialNets	,NULL},

   [ActChgThe		] = { 841,-1,TabUnk,ActReqEdiSet	,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_BRW_1ST_TAB,The_ChangeTheme		,Set_EditSettings		,NULL},
   [ActReqChgLan	] = { 992,-1,TabUnk,ActReqEdiSet	,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Lan_AskChangeLanguage		,NULL},
   [ActChgLan		] = { 654,-1,TabUnk,ActReqEdiSet	,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_BRW_1ST_TAB,Lan_ChangeLanguage		,Set_EditSettings		,NULL},
   [ActChg1stDay	] = {1484,-1,TabUnk,ActReqEdiSet	,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_BRW_1ST_TAB,Cal_ChangeFirstDayOfWeek	,Set_EditSettings		,NULL},
   [ActChgDatFmt	] = {1638,-1,TabUnk,ActReqEdiSet	,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_BRW_1ST_TAB,Dat_ChangeDateFormat		,Set_EditSettings		,NULL},
   [ActChgCol		] = { 674,-1,TabUnk,ActReqEdiSet	,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_BRW_1ST_TAB,Set_ChangeSideCols		,Set_EditSettings		,NULL},
   [ActChgIco		] = {1092,-1,TabUnk,ActReqEdiSet	,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_BRW_1ST_TAB,Ico_ChangeIconSet		,Set_EditSettings		,NULL},
   [ActChgMnu		] = {1243,-1,TabUnk,ActReqEdiSet	,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_BRW_1ST_TAB,Mnu_ChangeMenu			,Set_EditSettings		,NULL},
   [ActChgUsrPho	] = {1917,-1,TabUnk,ActReqEdiSet	,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_BRW_1ST_TAB,PhoSha_ChangePhotoShape	,Set_EditSettings		,NULL},
   [ActChgPriPho	] = { 774,-1,TabUnk,ActReqEdiSet	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Pho_ChangePhotoVisibility	,NULL},
   [ActChgBasPriPrf	] = {1404,-1,TabUnk,ActReqEdiSet	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Prf_ChangeBasicProfileVis	,NULL},
   [ActChgExtPriPrf	] = {1765,-1,TabUnk,ActReqEdiSet	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Prf_ChangeExtendedProfileVis	,NULL},
   [ActChgCooPrf	] = {1764,-1,TabUnk,ActReqEdiSet	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Coo_ChangeMyPrefsCookies	,NULL},
   [ActChgNtfPrf	] = { 775,-1,TabUnk,ActReqEdiSet	,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,Ntf_ChangeNotifyEvents		,Set_EditSettings		,NULL},

   [ActPrnUsrQR		] = {1022,-1,TabUnk,ActFrmMyAcc		,0x3F8,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_BRW_NEW_TAB,NULL				,QR_PrintUsrQRCode		,NULL},

   [ActPrnMyTT		] = { 409,-1,TabUnk,ActSeeMyTT		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_NEW_TAB,NULL				,Tmt_ShowClassTimeTable		,NULL},
   [ActEdiTut		] = {  65,-1,TabUnk,ActSeeMyTT		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Tmt_EditMyTutTimeTable		,NULL},
   [ActChgTut		] = {  48,-1,TabUnk,ActSeeMyTT		,0x3F8,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,NULL				,Tmt_EditMyTutTimeTable		,NULL},
   [ActChgMyTT1stDay	] = {1487,-1,TabUnk,ActSeeMyTT		,0x3F8,0x3C7,0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_BRW_1ST_TAB,Cal_ChangeFirstDayOfWeek	,Tmt_ShowClassTimeTable		,NULL},
  };

Act_Action_t ActLst_FromActCodToAction[1 + ActLst_MAX_ACTION_COD] =	// Do not reuse unique action codes!
  {
	ActSeeAdmDocCrsGrp,	//  #0
	-1,			//  #1 (obsolete action)
	ActMnu,			//  #2
	ActSeeRcvMsg,		//  #3
	-1,			//  #4 (obsolete action)
	-1,			//  #5 (obsolete action)
	ActLogIn,		//  #6
	-1,			//  #7 (obsolete action)
	-1,			//  #8 (obsolete action)
	ActSeeCrsLnk,		//  #9
	ActLogOut,		// #10
	-1,			// #11 (obsolete action)
	ActAdmDocCrs,		// #12
	-1,			// #13 (obsolete action)
	-1,			// #14 (obsolete action)
	ActSeeAss,		// #15
	-1,			// #16 (obsolete action)
	ActSeeAdmMrk,		// #17
	-1,			// #18 (obsolete action)
	-1,			// #19 (obsolete action)
	ActSeeSylPra,		// #20
	-1,			// #21 (obsolete action)
	ActSeeRecSevTch,	// #22
	ActAdmBrf,		// #23
	-1,			// #24 (obsolete action)
	ActSeeCrsTT,		// #25
	ActReqMsgUsr,		// #26
	ActRcvMsgUsr,		// #27
	ActSeeSylLec,		// #28
	ActSeeTst,		// #29
	ActReqMyPho,		// #30
	-1,			// #31 (obsolete action)
	ActSeeBib,		// #32
	-1,			// #33 (obsolete action)
	-1,			// #34 (obsolete action)
	ActChgMyPwd,		// #35
	ActFrmMyAcc,		// #36
	ActChgMyNck,		// #37
	-1,			// #38 (obsolete action)
	-1,			// #39 (obsolete action)
	-1,			// #40 (obsolete action)
	-1,			// #41 (obsolete action)
	ActLstStdAll,		// #42
	-1,			// #43 (obsolete action)
	ActEdiSylLec,		// #44
	ActEdiCrsTT,		// #45
	-1,			// #46 (obsolete action)
	-1,			// #47 (obsolete action)
	ActChgTut,		// #48
	-1,			// #49 (obsolete action)
	-1,			// #50 (obsolete action)
	ActSeeChtRms,		// #51
	ActCht,			// #52
	ActChgCrsTT,		// #53
	ActSeeFAQ,		// #54
	-1,			// #55 (obsolete action)
	-1,			// #56 (obsolete action)
	-1,			// #57 (obsolete action)
	-1,			// #58 (obsolete action)
	ActWriNot,		// #59
	ActRcvNot,		// #60
	-1,			// #61 (obsolete action)
	ActRemUsrGbl,		// #62
	-1,			// #63 (obsolete action)
	ActDelRcvMsg,		// #64
	ActEdiTut,		// #65
	-1,			// #66 (obsolete action)
	-1,			// #67 (obsolete action)
	-1,			// #68 (obsolete action)
	ActEdiAss,		// #69
	ActSeeSntMsg,		// #70
	-1,			// #71 (obsolete action)
	-1,			// #72 (obsolete action)
	ActRemNot,		// #73
	ActEdiSylPra,		// #74
	-1,			// #75 (obsolete action)
	ActEdiBib,		// #76
	-1,			// #77 (obsolete action)
	-1,			// #78 (obsolete action)
	ActSeeAccGbl,		// #79
	-1,			// #80 (obsolete action)
	-1,			// #81 (obsolete action)
	-1,			// #82 (obsolete action)
	-1,			// #83 (obsolete action)
	ActSeeUseGbl,		// #84
	ActSeeAllCfe,		// #85
	-1,			// #86 (obsolete action)
	ActRemAllStdCrs,	// #87
	ActReqRemAllStdCrs,	// #88
	ActSeeRecSevStd,	// #89
	ActDelSntMsg,		// #90
	ActEdiCfe,		// #91
	-1,			// #92 (obsolete action)
	-1,			// #93 (obsolete action)
	-1,			// #94 (obsolete action)
	ActSeeFor,		// #95
	ActEdiCrsLnk,		// #96
	-1,			// #97 (obsolete action)
	ActAssTst,		// #98
	-1,			// #99 (obsolete action)
	ActMaiUsr,		// #100
	-1,			// #101 (obsolete action)
	-1,			// #102 (obsolete action)
	ActReqTst,		// #103
	ActEdiTstQst,		// #104
	ActEdiOneTstQst,	// #105
	ActChgMaxStdGrp,	// #106
	ActReqRemGrp,		// #107
	ActReqEdiGrp,		// #108
	ActEdiFAQ,		// #109
	ActRcvCfe,		// #110
	ActPrnRecSevStd,	// #111
	-1,			// #112 (obsolete action)
	-1,			// #113 (obsolete action)
	-1,			// #114 (obsolete action)
	-1,			// #115 (obsolete action)
	ActReqSelGrp,		// #116
	-1,			// #117 (obsolete action)
	ActChgGrp,		// #118
	ActSeeAccCrs,		// #119
	ActPrnStdPho,		// #120
	ActRenGrp,		// #121
	ActNewGrp,		// #122
	-1,			// #123 (obsolete action)
	-1,			// #124 (obsolete action)
	-1,			// #125 (obsolete action)
	ActRcvTstQst,		// #126
	ActPrnRecSevTch,	// #127
	-1,			// #128 (obsolete action)
	-1,			// #129 (obsolete action)
	-1,			// #130 (obsolete action)
	-1,			// #131 (obsolete action)
	ActLstTstQst,		// #132
	ActRemOneTstQst,	// #133
	-1,			// #134 (obsolete action)
	-1,			// #135 (obsolete action)
	-1,			// #136 (obsolete action)
	-1,			// #137 (obsolete action)
	-1,			// #138 (obsolete action)
	ActAdmAsgWrkCrs,	// #139
	-1,			// #140 (obsolete action)
	-1,			// #141 (obsolete action)
	-1,			// #142 (obsolete action)
	ActRenTag,		// #143
	-1,			// #144 (obsolete action)
	-1,			// #145 (obsolete action)
	-1,			// #146 (obsolete action)
	-1,			// #147 (obsolete action)
	ActRcvFilWrkUsrCla,	// #148
	-1,			// #149 (obsolete action)
	ActFrmCreWrkUsr,	// #150
	-1,			// #151 (obsolete action)
	ActPrnCrsTT,		// #152
	ActRcvFilBrfCla,	// #153
	-1,			// #154 (obsolete action)
	ActRemFilBrf,		// #155
	-1,			// #156 (obsolete action)
	-1,			// #157 (obsolete action)
	-1,			// #158 (obsolete action)
	-1,			// #159 (obsolete action)
	-1,			// #160 (obsolete action)
	-1,			// #161 (obsolete action)
	-1,			// #162 (obsolete action)
	-1,			// #163 (obsolete action)
	ActRcvPagCrsLnk,	// #164
	-1,			// #165 (obsolete action)
	-1,			// #166 (obsolete action)
	ActChgGrpTyp,		// #167
	-1,			// #168 (obsolete action)
	ActRemFilWrkUsr,	// #169
	ActCreFolBrf,		// #170
	-1,			// #171 (obsolete action)
	ActCreFolWrkUsr,	// #172
	-1,			// #173 (obsolete action)
	ActNewGrpTyp,		// #174
	ActRemGrp,		// #175
	-1,			// #176 (obsolete action)
	-1,			// #177 (obsolete action)
	-1,			// #178 (obsolete action)
	ActPrnCfe,		// #179
	-1,			// #180 (obsolete action)
	ActInsIteSylPra,	// #181
	ActRcvURLCrsLnk,	// #182
	ActDelItmSylPra,	// #183
	ActRcvPagAss,		// #184
	ActRcvPagBib,		// #185
	-1,			// #186 (obsolete action)
	ActRemCfe,		// #187
	-1,			// #188 (obsolete action)
	-1,			// #189 (obsolete action)
	-1,			// #190 (obsolete action)
	-1,			// #191 (obsolete action)
	-1,			// #192 (obsolete action)
	-1,			// #193 (obsolete action)
	ActUnk,			// #194
	-1,			// #195 (obsolete action)
	ActRemFolBrf,		// #196
	ActRenFolBrf,		// #197
	-1,			// #198 (obsolete action)
	-1,			// #199 (obsolete action)
	-1,			// #200 (obsolete action)
	-1,			// #201 (obsolete action)
	-1,			// #202 (obsolete action)
	-1,			// #203 (obsolete action)
	ActRenFolWrkUsr,	// #204
	ActFrmCreWrkCrs,	// #205
	ActCreFolWrkCrs,	// #206
	ActRcvFilWrkCrsCla,	// #207
	ActRenFolWrkCrs,	// #208
	ActRemFilWrkCrs,	// #209
	ActRemFolWrkCrs,	// #210
	ActModIteSylLec,	// #211
	ActDwnIteSylPra,	// #212
	ActUp_IteSylPra,	// #213
	ActLftIteSylPra,	// #214
	ActRgtIteSylPra,	// #215
	ActModIteSylPra,	// #216
	ActInsIteSylLec,	// #217
	ActDelItmSylLec,	// #218
	ActRcvPagFAQ,		// #219
	ActDwnIteSylLec,	// #220
	ActUp_IteSylLec,	// #221
	ActLftIteSylLec,	// #222
	ActRgtIteSylLec,	// #223
	ActRcvURLBib,		// #224
	-1,			// #225 (obsolete action)
	-1,			// #226 (obsolete action)
	-1,			// #227 (obsolete action)
	ActRemFolWrkUsr,	// #228
	-1,			// #229 (obsolete action)
	-1,			// #230 (obsolete action)
	-1,			// #231 (obsolete action)
	-1,			// #232 (obsolete action)
	-1,			// #233 (obsolete action)
	ActRcvURLFAQ,		// #234
	ActRcvURLAss,		// #235
	ActReqRemGrpTyp,	// #236
	ActRemGrpTyp,		// #237
	-1,			// #238 (obsolete action)
	-1,			// #239 (obsolete action)
	-1,			// #240 (obsolete action)
	ActSeeForDegUsr,	// #241
	ActSeeForSWAUsr,	// #242
	ActSeeForDegTch,	// #243
	ActSeePstForSWAUsr,	// #244
	ActSeeForSWATch,	// #245
	ActSeePstForSWATch,	// #246
	ActRcvThrForDegTch,	// #247
	-1,			// #248 (obsolete action)
	-1,			// #249 (obsolete action)
	-1,			// #250 (obsolete action)
	-1,			// #251 (obsolete action)
	ActRcvThrForDegUsr,	// #252
	-1,			// #253 (obsolete action)
	-1,			// #254 (obsolete action)
	ActSeePstForDegUsr,	// #255
	-1,			// #256 (obsolete action)
	-1,			// #257 (obsolete action)
	ActRcvThrForSWAUsr,	// #258
	ActRcvThrForSWATch,	// #259
	-1,			// #260 (obsolete action)
	-1,			// #261 (obsolete action)
	-1,			// #262 (obsolete action)
	-1,			// #263 (obsolete action)
	-1,			// #264 (obsolete action)
	-1,			// #265 (obsolete action)
	-1,			// #266 (obsolete action)
	-1,			// #267 (obsolete action)
	-1,			// #268 (obsolete action)
	-1,			// #269 (obsolete action)
	-1,			// #270 (obsolete action)
	-1,			// #271 (obsolete action)
	-1,			// #272 (obsolete action)
	-1,			// #273 (obsolete action)
	-1,			// #274 (obsolete action)
	-1,			// #275 (obsolete action)
	ActRemTreBrf,		// #276
	-1,			// #277 (obsolete action)
	ActRemTreWrkUsr,	// #278
	ActRemTreWrkCrs,	// #279
	-1,			// #280 (obsolete action)
	-1,			// #281 (obsolete action)
	-1,			// #282 (obsolete action)
	-1,			// #283 (obsolete action)
	ActAdmMrkCrs,		// #284
	ActReqEdiRecSha,	// #285
	ActReqRemFilBrf,	// #286
	-1,			// #287 (obsolete action)
	ActReqRemFilWrkUsr,	// #288
	ActReqRemFilWrkCrs,	// #289
	-1,			// #290 (obsolete action)
	ActSeePstForDegTch,	// #291
	ActEdiRecFie,		// #292
	ActNewFie,		// #293
	ActReqRemFie,		// #294
	ActRemFie,		// #295
	ActRenFie,		// #296
	ActChgVisFie,		// #297
	ActChgMyData,		// #298
	-1,			// #299 (obsolete action)
	ActRcvRecOthUsr,	// #300
	ActRcvRecCrs,		// #301
	ActChgMulGrpTyp,	// #302
	ActChgMdtGrpTyp,	// #303
	ActRenGrpTyp,		// #304
	ActChgRowFie,		// #305
	-1,			// #306 (obsolete action)
	-1,			// #307 (obsolete action)
	-1,			// #308 (obsolete action)
	-1,			// #309 (obsolete action)
	-1,			// #310 (obsolete action)
	ActCopBrf,		// #311
	ActCopWrkCrs,		// #312
	-1,			// #313 (obsolete action)
	ActCopWrkUsr,		// #314
	ActPasBrf,		// #315
	-1,			// #316 (obsolete action)
	-1,			// #317 (obsolete action)
	ActPasWrkUsr,		// #318
	ActPasWrkCrs,		// #319
	-1,			// #320 (obsolete action)
	ActCloGrp,		// #321
	ActOpeGrp,		// #322
	ActFrmCreShaCrs,	// #323
	ActCreFolShaCrs,	// #324
	ActRemFolShaCrs,	// #325
	ActRcvFilShaCrsCla,	// #326
	ActReqRemFilShaCrs,	// #327
	ActRemFilShaCrs,	// #328
	ActRenFolShaCrs,	// #329
	ActCopShaCrs,		// #330
	ActPasShaCrs,		// #331
	ActRemTreShaCrs,	// #332
	ActFrmCreShaGrp,	// #333
	ActCreFolShaGrp,	// #334
	ActRcvFilShaGrpCla,	// #335
	ActCopShaGrp,		// #336
	ActPasShaGrp,		// #337
	ActRemFolShaGrp,	// #338
	ActRemTreShaGrp,	// #339
	ActRenFolShaGrp,	// #340
	ActReqRemFilShaGrp,	// #341
	ActRemFilShaGrp,	// #342
	-1,			// #343 (obsolete action)
	-1,			// #344 (obsolete action)
	ActSeeForCrsUsr,	// #345
	ActSeePstForCrsUsr,	// #346
	ActSeePstForCrsTch,	// #347
	ActSeePstForCtrUsr,	// #348
	-1,			// #349 (obsolete action)
	ActRcvThrForCrsUsr,	// #350
	-1,			// #351 (obsolete action)
	-1,			// #352 (obsolete action)
	-1,			// #353 (obsolete action)
	-1,			// #354 (obsolete action)
	-1,			// #355 (obsolete action)
	-1,			// #356 (obsolete action)
	-1,			// #357 (obsolete action)
	-1,			// #358 (obsolete action)
	-1,			// #359 (obsolete action)
	-1,			// #360 (obsolete action)
	-1,			// #361 (obsolete action)
	-1,			// #362 (obsolete action)
	-1,			// #363 (obsolete action)
	-1,			// #364 (obsolete action)
	-1,			// #365 (obsolete action)
	-1,			// #366 (obsolete action)
	-1,			// #367 (obsolete action)
	-1,			// #368 (obsolete action)
	-1,			// #369 (obsolete action)
	ActSelInfSrcBib,	// #370
	ActEditorSylPra,	// #371
	ActEditorSylLec,	// #372
	-1,			// #373 (obsolete action)
	-1,			// #374 (obsolete action)
	-1,			// #375 (obsolete action)
	ActEditorBib,		// #376
	ActPlaTxtEdiBib,	// #377
	ActSelInfSrcSylLec,	// #378
	ActPlaTxtEdiSylLec,	// #379
	ActSelInfSrcFAQ,	// #380
	ActRcvPagSylLec,	// #381
	ActSelInfSrcSylPra,	// #382
	ActRcvPagSylPra,	// #383
	ActSelInfSrcAss,	// #384
	ActSelInfSrcCrsLnk,	// #385
	ActEditorAss,		// #386
	ActPlaTxtEdiAss,	// #387
	ActEditorCrsLnk,	// #388
	ActPlaTxtEdiSylPra,	// #389
	-1,			// #390 (obsolete action)
	-1,			// #391 (obsolete action)
	-1,			// #392 (obsolete action)
	-1,			// #393 (obsolete action)
	ActRcvPlaTxtSylLec,	// #394
	-1,			// #395 (obsolete action)
	ActRcvPlaTxtSylPra,	// #396
	ActRcvPlaTxtAss,	// #397
	ActRcvPlaTxtBib,	// #398
	-1,			// #399 (obsolete action)
	ActPlaTxtEdiCrsLnk,	// #400
	ActRcvPlaTxtCrsLnk,	// #401
	ActRcvURLSylPra,	// #402
	ActRcvURLSylLec,	// #403
	ActEditorFAQ,		// #404
	ActPlaTxtEdiFAQ,	// #405
	ActRcvPlaTxtFAQ,	// #406
	-1,			// #407 (obsolete action)
	ActSeeMyTT,		// #408
	ActPrnMyTT,		// #409
	ActExpBrf,		// #410
	ActConBrf,		// #411
	-1,			// #412 (obsolete action)
	-1,			// #413 (obsolete action)
	-1,			// #414 (obsolete action)
	-1,			// #415 (obsolete action)
	ActExpWrkCrs,		// #416
	-1,			// #417 (obsolete action)
	-1,			// #418 (obsolete action)
	-1,			// #419 (obsolete action)
	-1,			// #420 (obsolete action)
	ActExpShaCrs,		// #421
	ActConShaCrs,		// #422
	ActExpWrkUsr,		// #423
	ActConWrkCrs,		// #424
	ActConWrkUsr,		// #425
	ActConShaGrp,		// #426
	ActExpShaGrp,		// #427
	ActRemMyPho,		// #428
	-1,			// #429 (obsolete action)
	ActSeeForCtrTch,	// #430
	ActSeeForCrsTch,	// #431
	-1,			// #432 (obsolete action)
	-1,			// #433 (obsolete action)
	ActDelAllSntMsg,	// #434
	-1,			// #435 (obsolete action)
	ActDelAllRcvMsg,	// #436
	-1,			// #437 (obsolete action)
	-1,			// #438 (obsolete action)
	-1,			// #439 (obsolete action)
	-1,			// #440 (obsolete action)
	-1,			// #441 (obsolete action)
	-1,			// #442 (obsolete action)
	ActPrnTchPho,		// #443
	ActCalPhoDeg,		// #444
	-1,			// #445 (obsolete action)
	-1,			// #446 (obsolete action)
	ActSeePhoDeg,		// #447
	ActPrnPhoDeg,		// #448
	-1,			// #449 (obsolete action)
	-1,			// #450 (obsolete action)
	ActCfgTst,		// #451
	ActDisTag,		// #452
	ActEnaTag,		// #453
	ActRcvCfgTst,		// #454
	ActChgShfTstQst,	// #455
	-1,			// #456 (obsolete action)
	-1,			// #457 (obsolete action)
	-1,			// #458 (obsolete action)
	-1,			// #459 (obsolete action)
	-1,			// #460 (obsolete action)
	ActAdmShaCrsGrp,	// #461
	ActExpSeeDocCrs,	// #462
	-1,			// #463 (obsolete action)
	ActUnhDocCrs,		// #464
	ActHidDocCrs,		// #465
	-1,			// #466 (obsolete action)
	-1,			// #467 (obsolete action)
	ActFrmCreDocGrp,	// #468
	ActCreFolDocGrp,	// #469
	ActCopDocCrs,		// #470
	ActPasDocGrp,		// #471
	ActCopDocGrp,		// #472
	ActReqRemFilDocGrp,	// #473
	ActRemFilDocGrp,	// #474
	-1,			// #475 (obsolete action)
	ActConSeeDocCrs,	// #476
	ActExpAdmDocCrs,	// #477
	ActPasDocCrs,		// #478
	ActReqRemFilDocCrs,	// #479
	ActRemFilDocCrs,	// #480
	ActFrmCreDocCrs,	// #481
	ActRcvFilDocCrsCla,	// #482
	ActRcvFilDocGrpCla,	// #483
	ActRemFolDocGrp,	// #484
	ActRemTreDocGrp,	// #485
	ActExpAdmDocGrp,	// #486
	ActConAdmDocGrp,	// #487
	ActExpSeeDocGrp,	// #488
	ActConSeeDocGrp,	// #489
	ActRenFolDocGrp,	// #490
	ActCreFolDocCrs,	// #491
	ActHidDocGrp,		// #492
	ActUnhDocGrp,		// #493
	ActConAdmDocCrs,	// #494
	ActEnaFilZonGrp,	// #495
	ActDisFilZonGrp,	// #496
	ActRemFolDocCrs,	// #497
	ActRemTreDocCrs,	// #498
	-1,			// #499 (obsolete action)
	-1,			// #500 (obsolete action)
	ActCopMrkCrs,		// #501
	ActPasMrkGrp,		// #502
	ActChgNumRowHeaCrs,	// #503
	ActChgNumRowFooCrs,	// #504
	-1,			// #505 (obsolete action)
	ActCreFolMrkCrs,	// #506
	ActPasMrkCrs,		// #507
	-1,			// #508 (obsolete action)
	ActRemFilMrkGrp,	// #509
	ActChgNumRowHeaGrp,	// #510
	ActChgNumRowFooGrp,	// #511
	-1,			// #512 (obsolete action)
	ActCreFolMrkGrp,	// #513
	ActRcvFilMrkGrpCla,	// #514
	-1,			// #515 (obsolete action)
	ActRcvFilMrkCrsCla,	// #516
	-1,			// #517 (obsolete action)
	-1,			// #518 (obsolete action)
	ActCopMrkGrp,		// #519
	ActRemFolMrkGrp,	// #520
	ActRemTreMrkGrp,	// #521
	-1,			// #522 (obsolete action)
	ActSeeMyMrkCrs,		// #523
	ActSeeMyMrkGrp,		// #524
	-1,			// #525 (obsolete action)
	-1,			// #526 (obsolete action)
	ActConSeeMrkCrs,	// #527
	ActExpSeeMrkCrs,	// #528
	ActRenFolMrkGrp,	// #529
	ActRemFolMrkCrs,	// #530
	-1,			// #531 (obsolete action)
	-1,			// #532 (obsolete action)
	ActRemFilMrkCrs,	// #533
	ActRemTreMrkCrs,	// #534
	ActRenFolDocCrs,	// #535
	ActEdiDeg,		// #536
	ActNewDegTyp,		// #537
	ActRenDegTyp,		// #538
	-1,			// #539 (obsolete action)
	ActNewDeg,		// #540
	-1,			// #541 (obsolete action)
	ActRemDeg,		// #542
	-1,			// #543 (obsolete action)
	ActChgDegTyp,		// #544
	ActRemDegTyp,		// #545
	ActRenDegSho,		// #546
	ActRenDegFul,		// #547
	-1,			// #548 (obsolete action)
	-1,			// #549 (obsolete action)
	-1,			// #550 (obsolete action)
	-1,			// #551 (obsolete action)
	-1,			// #552
	ActRecDegLog,		// #553
	ActChgDegWWW,		// #554
	ActEdiCrs,		// #555
	ActNewCrs,		// #556
	-1,			// #557 (obsolete action)
	-1,			// #558 (obsolete action)
	-1,			// #559 (obsolete action)
	ActRemCrs,		// #560
	ActChgCrsYea,		// #561
	-1,			// #562
	ActRenCrsSho,		// #563
	ActRenCrsFul,		// #564
	-1,			// #565 (obsolete action)
	-1,			// #566 (obsolete action)
	-1,			// #567 (obsolete action)
	-1,			// #568 (obsolete action)
	-1,			// #569 (obsolete action)
	-1,			// #570 (obsolete action)
	-1,			// #571 (obsolete action)
	-1,			// #572 (obsolete action)
	ActEdiDegTyp,		// #573
	ActRenFolMrkCrs,	// #574
	-1,			// #575 (obsolete action)
	-1,			// #576 (obsolete action)
	-1,			// #577 (obsolete action)
	ActLstTchAll,		// #578
	-1,			// #579 (obsolete action)
	-1,			// #580 (obsolete action)
	-1,			// #581 (obsolete action)
	-1,			// #582 (obsolete action)
	-1,			// #583 (obsolete action)
	ActRemAdmDeg,		// #584
	-1,			// #585 (obsolete action)
	ActNewAdmDeg,		// #586
	ActLstGst,		// #587
	-1,			// #588 (obsolete action)
	ActChgMyRol,		// #589
	ActReqRemOldUsr,	// #590
	ActReqAccGbl,		// #591
	-1,			// #592 (obsolete action)
	ActReqDelAllRcvMsg,	// #593
	ActReqAccCrs,		// #594
	ActReqRemFilMrkCrs,	// #595
	ActFrmCreMrkCrs,	// #596
	ActFrmCreBrf,		// #597
	-1,			// #598 (obsolete action)
	ActRcvRepForCrsUsr,	// #599
	ActReqRemFilMrkGrp,	// #600
	ActFrmCreMrkGrp,	// #601
	ActDelPstForCrsUsr,	// #602
	ActRcvRepForSWAUsr,	// #603
	ActReqDelAllSntMsg,	// #604
	ActExpSeeMrkGrp,	// #605
	ActRcvRepForDegUsr,	// #606
	ActExpAdmMrkCrs,	// #607
	ActDelPstForDegUsr,	// #608
	ActConSeeMrkGrp,	// #609
	ActDisPstForCrsUsr,	// #610
	-1,			// #611 (obsolete action)
	-1,			// #612 (obsolete action)
	ActDelPstForSWAUsr,	// #613
	-1,			// #614 (obsolete action)
	ActDisPstForDegUsr,	// #615
	ActEnbPstForDegUsr,	// #616
	ActRcvRepForDegTch,	// #617
	ActDisPstForDegTch,	// #618
	ActEnbPstForDegTch,	// #619
	-1,			// #620 (obsolete action)
	ActConAdmMrkCrs,	// #621
	ActRcvRepForSWATch,	// #622
	ActDelPstForSWATch,	// #623
	ActEnbPstForCrsUsr,	// #624
	ActDisPstForSWAUsr,	// #625
	-1,			// #626 (obsolete action)
	ActReqSch,		// #627
	ActSch,			// #628
	-1,			// #629 (obsolete action)
	-1,			// #630 (obsolete action)
	ActExpAdmMrkGrp,	// #631
	ActEnbPstForSWAUsr,	// #632
	ActSndNewPwd,		// #633
	ActEnbPstForSWATch,	// #634
	ActDisPstForSWATch,	// #635
	-1,			// #636 (obsolete action)
	-1,			// #637 (obsolete action)
	-1,			// #638 (obsolete action)
	-1,			// #639 (obsolete action)
	-1,			// #640 (obsolete action)
	-1,			// #641 (obsolete action)
	-1,			// #642 (obsolete action)
	-1,			// #643 (obsolete action)
	-1,			// #644 (obsolete action)
	ActAll,			// #645
	-1,			// #646 (obsolete action)
	-1,			// #647 (obsolete action)
	-1,			// #648 (obsolete action)
	-1,			// #649 (obsolete action)
	-1,			// #650 (obsolete action)
	-1,			// #651 (obsolete action)
	-1,			// #652 (obsolete action)
	-1,			// #653 (obsolete action)
	ActChgLan,		// #654
	-1,			// #655 (obsolete action)
	-1,			// #656 (obsolete action)
	-1,			// #657 (obsolete action)
	-1,			// #658 (obsolete action)
	-1,			// #659 (obsolete action)
	-1,			// #660 (obsolete action)
	-1,			// #661 (obsolete action)
	-1,			// #662 (obsolete action)
	ActExpRcvMsg,		// #663
	ActExpSntMsg,		// #664
	ActReqSndNewPwd,	// #665
	-1,			// #666 (obsolete action)
	-1,			// #667 (obsolete action)
	-1,			// #668 (obsolete action)
	-1,			// #669 (obsolete action)
	-1,			// #670 (obsolete action)
	-1,			// #671 (obsolete action)
	-1,			// #672 (obsolete action)
	ActReqEdiSet,		// #673
	ActChgCol,		// #674
	ActSeeDpt,		// #675
	ActSeeCtr,		// #676
	ActEdiDpt,		// #677
	ActLstStd,		// #678
	ActLstTch,		// #679
	ActDelPstForDegTch,	// #680
	ActEdiCtr,		// #681
	ActRenCtrSho,		// #682
	ActChgCtrWWW,		// #683
	ActRenCtrFul,		// #684
	ActNewCtr,		// #685
	ActRemCtr,		// #686
	ActNewDpt,		// #687
	ActRenDptSho,		// #688
	ActRenDptFul,		// #689
	ActRemDpt,		// #690
	ActChgDptWWW,		// #691
	-1,			// #692 (obsolete action)
	ActDetMyPho,		// #693
	ActUpdMyPho,		// #694
	-1,			// #695 (obsolete action)
	ActSeeIns,		// #696
	ActEdiIns,		// #697
	ActNewIns,		// #698
	ActRecInsLog,		// #699
	ActChgInsWWW,		// #700
	ActRenInsFul,		// #701
	ActRenInsSho,		// #702
	ActSeePlc,		// #703
	ActEdiPlc,		// #704
	ActNewPlc,		// #705
	ActChgCtrPlc,		// #706
	ActSeeHld,		// #707
	-1,			// #708 (obsolete action)
	-1,			// #709 (obsolete action)
	-1,			// #710 (obsolete action)
	-1,			// #711 (obsolete action)
	-1,			// #712 (obsolete action)
	ActEdiHld,		// #713
	ActNewHld,		// #714
	ActChgHldTyp,		// #715
	ActRemHld,		// #716
	ActChgHldStrDat,	// #717
	ActChgHldEndDat,	// #718
	-1,			// #719 (obsolete action)
	-1,			// #720 (obsolete action)
	ActChgDptIns,		// #721
	-1,			// #722 (obsolete action)
	ActSeeForGenTch,	// #723
	ActSeeForInsTch,	// #724
	ActSeeForInsUsr,	// #725
	ActSeeForGenUsr,	// #726
	ActSeePstForGenUsr,	// #727
	-1,			// #728 (obsolete action)
	-1,			// #729 (obsolete action)
	ActSeePstForInsUsr,	// #730
	ActSeePstForGenTch,	// #731
	-1,			// #732 (obsolete action)
	-1,			// #733 (obsolete action)
	-1,			// #734 (obsolete action)
	ActDelPstForGenUsr,	// #735
	ActRcvThrForGenUsr,	// #736
	ActRcvThrForInsUsr,	// #737
	-1,			// #738 (obsolete action)
	-1,			// #739 (obsolete action)
	ActRcvRepForInsUsr,	// #740
	-1,			// #741 (obsolete action)
	-1,			// #742 (obsolete action)
	ActDelPstForInsUsr,	// #743
	-1,			// #744 (obsolete action)
	-1,			// #745 (obsolete action)
	ActSeePstForInsTch,	// #746
	ActRcvRepForGenUsr,	// #747
	ActSeeLnk,		// #748
	ActEdiLnk,		// #749
	ActNewLnk,		// #750
	ActRenLnkFul,		// #751
	ActChgLnkWWW,		// #752
	ActRenLnkSho,		// #753
	ActRcvThrForCrsTch,	// #754
	ActRcvRepForCrsTch,	// #755
	ActDelPstForCrsTch,	// #756
	-1,			// #757 (obsolete action)
	-1,			// #758 (obsolete action)
	ActRemIns,		// #759
	-1,			// #760 (obsolete action)
	ActReqUseGbl,		// #761
	ActSeeAllNot,		// #762
	ActHidNot,		// #763
	ActUnhNot,		// #764
	ActRcvThrForGenTch,	// #765
	ActRenHld,		// #766
	ActReqStaCrs,		// #767
	ActSeeAllStaCrs,	// #768
	ActRcvThrForInsTch,	// #769
	ActRcvRepForInsTch,	// #770
	-1,			// #771 (obsolete action)
	ActDelPstForInsTch,	// #772
	ActRemOldUsr,		// #773
	ActChgPriPho,		// #774
	ActChgNtfPrf,		// #775
	ActRemPlc,		// #776
	ActSeePlg,		// #777
	ActEdiPlg,		// #778
	ActNewPlg,		// #779
	ActChgPlgIP,		// #780
	ActChgPlgLog,		// #781
	ActRenPlg,		// #782
	ActChgPlgURL,		// #783
	ActSeeTchGui,		// #784
	ActEdiTchGui,		// #785
	ActEditorTchGui,	// #786
	ActPlaTxtEdiTchGui,	// #787
	ActRcvPagTchGui,	// #788
	ActSelInfSrcTchGui,	// #789
	ActRcvPlaTxtTchGui,	// #790
	ActRcvURLTchGui,	// #791
	ActAdmAsgWrkUsr,	// #792
	-1,			// #793 (obsolete action)
	-1,			// #794 (obsolete action)
	-1,			// #795 (obsolete action)
	-1,			// #796 (obsolete action)
	-1,			// #797 (obsolete action)
	-1,			// #798 (obsolete action)
	-1,			// #799 (obsolete action)
	-1,			// #800 (obsolete action)
	ActSeeAllAsg,		// #801
	-1,			// #802 (obsolete action)
	ActNewAsg,		// #803
	-1,			// #804 (obsolete action)
	-1,			// #805 (obsolete action)
	ActRemAsg,		// #806
	-1,			// #807 (obsolete action)
	-1,			// #808 (obsolete action)
	-1,			// #809 (obsolete action)
	-1,			// #810 (obsolete action)
	-1,			// #811 (obsolete action)
	ActFrmNewAsg,		// #812
	ActReqRemAsg,		// #813
	ActEdiOneAsg,		// #814
	ActChgAsg,		// #815
	ActRcvRepForGenTch,	// #816
	ActFrmCreAsgCrs,	// #817
	ActCreFolAsgCrs,	// #818
	ActExpAsgCrs,		// #819
	ActRemFolAsgCrs,	// #820
	ActPasAsgCrs,		// #821
	ActRemTreAsgCrs,	// #822
	ActRenFolAsgCrs,	// #823
	ActExpAsgUsr,		// #824
	ActFrmCreAsgUsr,	// #825
	ActCreFolAsgUsr,	// #826
	ActRemFolAsgUsr,	// #827
	ActRemTreAsgUsr,	// #828
	ActCopAsgUsr,		// #829
	ActPasAsgUsr,		// #830
	ActConAsgUsr,		// #831
	ActRcvFilAsgUsrCla,	// #832
	ActRemFilAsgUsr,	// #833
	ActReqRemFilAsgUsr,	// #834
	ActConAsgCrs,		// #835
	ActCopAsgCrs,		// #836
	ActReqRemFilAsgCrs,	// #837
	ActRemFilAsgCrs,	// #838
	ActRenFolAsgUsr,	// #839
	ActSetUp,		// #840
	ActChgThe,		// #841
	-1,			// #842 (obsolete action)
	ActFrmRolSes,		// #843
	-1,			// #844 (obsolete action)
	ActRefCon,		// #845
	ActRcvFilAsgCrsCla,	// #846
	ActSeeCrsInf,		// #847
	ActEdiCrsInf,		// #848
	ActSelInfSrcCrsInf,	// #849
	ActPlaTxtEdiCrsInf,	// #850
	ActRcvPlaTxtCrsInf,	// #851
	ActEditorCrsInf,	// #852
	ActRcvPagCrsInf,	// #853
	ActRcvURLCrsInf,	// #854
	ActSeeMai,		// #855
	ActEdiMai,		// #856
	ActNewMai,		// #857
	ActRenMaiSho,		// #858
	ActRenMaiFul,		// #859
	ActRemMai,		// #860
	ActSeeAllAtt,		// #861
	ActSeeCty,		// #862
	ActEdiCty,		// #863
	ActNewCty,		// #864
	-1,			// #865 (obsolete action)
	ActRenCty,		// #866
	ActReqDelThrCrsUsr,	// #867
	ActDelThrForCrsUsr,	// #868
	ActReqDelThrCrsTch,	// #869
	ActChgFrcReaTchGui,	// #870
	ActChgFrcReaSylLec,	// #871
	ActChgFrcReaSylPra,	// #872
	ActChgFrcReaBib,	// #873
	ActChgFrcReaFAQ,	// #874
	ActChgFrcReaCrsLnk,	// #875
	ActDelThrForCrsTch,	// #876
	ActChgFrcReaCrsInf,	// #877
	ActChgHavReaCrsInf,	// #878
	ActChgHavReaFAQ,	// #879
	ActChgHavReaSylLec,	// #880
	ActReqDelThrSWAUsr,	// #881
	ActDelThrForSWAUsr,	// #882
	ActChgFrcReaAss,	// #883
	ActChgHavReaBib,	// #884
	ActChgHavReaCrsLnk,	// #885
	ActChgHavReaTchGui,	// #886
	ActChgHavReaSylPra,	// #887
	ActChgPlgDes,		// #888
	ActRemPlg,		// #889
	ActCutThrForSWAUsr,	// #890
	ActPasThrForCrsUsr,	// #891
	ActWebSvc,		// #892
	ActRemCty,		// #893
	ActRenPlcSho,		// #894
	ActRenPlcFul,		// #895
	ActChgHldPlc,		// #896
	ActRemLnk,		// #897
	ActChgHavReaAss,	// #898
	ActReqAsgWrkCrs,	// #899
	ActConAdmMrkGrp,	// #900
	ActSeeForCtrUsr,	// #901
	ActSeePstForCtrTch,	// #902
	ActRcvThrForCtrUsr,	// #903
	ActRcvThrForCtrTch,	// #904
	ActRcvRepForCtrUsr,	// #905
	ActRcvRepForCtrTch,	// #906
	ActReqDelThrDegUsr,	// #907
	ActReqDelThrDegTch,	// #908
	ActReqDelThrCtrUsr,	// #909
	ActReqDelThrCtrTch,	// #910
	ActReqDelThrInsUsr,	// #911
	ActReqDelThrInsTch,	// #912
	ActReqDelThrGenUsr,	// #913
	ActReqDelThrGenTch,	// #914
	ActReqDelThrSWATch,	// #915
	ActDelThrForDegUsr,	// #916
	ActDelThrForDegTch,	// #917
	ActDelThrForCtrUsr,	// #918
	ActDelThrForCtrTch,	// #919
	ActDelThrForInsUsr,	// #920
	ActDelThrForInsTch,	// #921
	ActDelThrForGenUsr,	// #922
	ActDelThrForGenTch,	// #923
	ActDelThrForSWATch,	// #924
	ActDisPstForGenTch,	// #925
	ActCutThrForCrsUsr,	// #926
	ActCutThrForCrsTch,	// #927
	ActCutThrForDegUsr,	// #928
	ActCutThrForDegTch,	// #929
	ActCutThrForCtrUsr,	// #930
	ActCutThrForCtrTch,	// #931
	ActCutThrForInsUsr,	// #932
	ActCutThrForInsTch,	// #933
	ActCutThrForGenUsr,	// #934
	ActCutThrForGenTch,	// #935
	ActCutThrForSWATch,	// #936
	ActPasThrForCrsTch,	// #937
	ActPasThrForDegUsr,	// #938
	ActPasThrForDegTch,	// #939
	ActPasThrForCtrUsr,	// #940
	ActPasThrForCtrTch,	// #941
	ActPasThrForInsUsr,	// #942
	ActPasThrForInsTch,	// #943
	ActPasThrForGenUsr,	// #944
	ActPasThrForGenTch,	// #945
	ActPasThrForSWAUsr,	// #946
	ActPasThrForSWATch,	// #947
	ActDelPstForCtrUsr,	// #948
	ActDelPstForCtrTch,	// #949
	ActDelPstForGenTch,	// #950
	ActEnbPstForCrsTch,	// #951
	ActEnbPstForCtrUsr,	// #952
	ActEnbPstForCtrTch,	// #953
	ActEnbPstForInsUsr,	// #954
	ActEnbPstForInsTch,	// #955
	ActEnbPstForGenUsr,	// #956
	ActEnbPstForGenTch,	// #957
	ActDisPstForCrsTch,	// #958
	ActDisPstForCtrUsr,	// #959
	ActDisPstForCtrTch,	// #960
	ActDisPstForInsUsr,	// #961
	ActDisPstForInsTch,	// #962
	ActDisPstForGenUsr,	// #963
	ActHidAsg,		// #964
	ActUnhAsg,		// #965
	ActSeeAllSvy,		// #966
	-1,			// #967 (obsolete action)
	ActNewSvy,		// #968
	ActRemSvy,		// #969
	-1,			// #970 (obsolete action)
	-1,			// #971 (obsolete action)
	-1,			// #972 (obsolete action)
	ActFrmNewSvy,		// #973
	ActEdiOneSvy,		// #974
	ActChgSvy,		// #975
	ActReqRemSvy,		// #976
	ActHidSvy,		// #977
	ActUnhSvy,		// #978
	ActEdiOneSvyQst,	// #979
	ActRcvSvyQst,		// #980
	ActRemSvyQst,		// #981
	ActSeeOneSvy,		// #982
	ActAnsSvy,		// #983
	ActReqRstSvy,		// #984
	ActRstSvy,		// #985
	ActChgPlgAppKey,	// #986
	ActMyCrs,		// #987
	-1,			// #988 (obsolete action)
	ActLstClk,		// #989
	ActSeeNtf,		// #990
	ActSeeNewNtf,		// #991
	ActReqChgLan,		// #992
	ActMFUAct,		// #993
	ActRefLstClk,		// #994
	ActLstCon,		// #995
	ActChgDatAdmDocCrs,	// #996
	-1,			// #997 (obsolete action)
	ActChgDatAdmDocGrp,	// #998
	-1,			// #999 (obsolete action)
	ActChgDatShaCrs,	// #1000
	-1,			// #1001 (obsolete action)
	ActChgDatShaGrp,	// #1002
	-1,			// #1003 (obsolete action)
	-1,			// #1004 (obsolete action)
	-1,			// #1005 (obsolete action)
	-1,			// #1006 (obsolete action)
	ActReqImpTstQst,	// #1007
	ActImpTstQst,		// #1008
	ActSeeCrs,		// #1009
	-1,			// #1010 (obsolete action)
	ActSeeDeg,		// #1011
	-1,			// #1012 (obsolete action)
	ActSeeDegTyp,		// #1013
	-1,			// #1014 (obsolete action)
	ActBanUsrMsg,		// #1015
	ActUnbUsrMsg,		// #1016
	ActLstBanUsr,		// #1017
	ActUnbUsrLst,		// #1018
	ActConRcvMsg,		// #1019
	ActConSntMsg,		// #1020
	-1,			// #1021 (obsolete action)
	ActPrnUsrQR,		// #1022
	-1,			// #1023 (obsolete action)
	ActChgInsCrsCodCfg,	// #1024
	ActChgInsCrsCod,	// #1025
	-1,			// #1026 (obsolete action)
	-1,			// #1027 (obsolete action)
	ActPrnCrsInf,		// #1028
	ActReqDatAdmDocCrs,	// #1029
	ActReqDatAdmDocGrp,	// #1030
	ActReqDatShaCrs,	// #1031
	ActReqDatShaGrp,	// #1032
	ActReqDatSeeDocCrs,	// #1033
	ActReqDatSeeDocGrp,	// #1034
        ActReqDatAdmMrkCrs,	// #1035
        ActChgDatAdmMrkCrs,	// #1036
        ActReqDatAdmMrkGrp,	// #1037
        ActChgDatAdmMrkGrp,	// #1038
        ActReqDatAsgUsr,	// #1039
        ActChgDatAsgUsr,	// #1040
        ActReqDatWrkUsr,	// #1041
        ActChgDatWrkUsr,	// #1042
        ActReqDatAsgCrs,	// #1043
        ActChgDatAsgCrs,	// #1044
        ActReqDatWrkCrs,	// #1045
        ActChgDatWrkCrs,	// #1046
        ActReqDatBrf,		// #1047
        ActChgDatBrf,		// #1048
	-1,			// #1049 (obsolete action)
	-1,			// #1050 (obsolete action)
	ActRecCtrLog,		// #1051
	-1,			// #1052 (obsolete action)
	ActReqCrs,		// #1053
	ActReqSignUp,		// #1054
	ActChgCrsSta,		// #1055
	ActSignUp,		// #1056
	ActSeeSignUpReq,	// #1057
	ActReqRejSignUp,	// #1058
	ActRejSignUp,		// #1059
	ActSeePen,		// #1060
	ActChgTimGrpTyp,	// #1061
	-1,			// #1062 (obsolete action)
	ActFrmNewAtt,		// #1063
	ActEdiOneAtt,		// #1064
	ActNewAtt,		// #1065
	ActChgAtt,		// #1066
	ActReqRemAtt,		// #1067
	ActRemAtt,		// #1068
	ActHidAtt,		// #1069
	ActUnhAtt,		// #1070
	ActSeeOneAtt,		// #1071
	ActRecAttStd,		// #1072
	ActReqLstUsrAtt,	// #1073
	ActSeeLstUsrAtt,	// #1074
	ActPrnLstUsrAtt,	// #1075
	ActRecAttMe,		// #1076
	ActLogInLan,		// #1077
	ActSeeDocCrs,		// #1078
	ActSeeMrkCrs,		// #1079
	ActReqSeeUsrTstRes,	// #1080
	ActSeeUsrTstResCrs,	// #1081
	ActSeeOneTstResOth,	// #1082
	ActReqSeeMyTstRes,	// #1083
	ActSeeMyTstResCrs,	// #1084
	ActSeeOneTstResMe,	// #1085
	ActReqDatSeeMrkCrs,	// #1086
	ActReqDatSeeMrkGrp,	// #1087
	ActChgMyMai,		// #1088
	ActRemMyNck,		// #1089
	ActRemMyMai,		// #1090
	ActCnfMai,		// #1091
	ActChgIco,		// #1092
	ActRchTxtEdiCrsInf,	// #1093
	ActRchTxtEdiTchGui,	// #1094
	ActRchTxtEdiSylLec,	// #1095
	ActRchTxtEdiSylPra,	// #1096
	ActRchTxtEdiBib,	// #1097
	ActRchTxtEdiFAQ,	// #1098
	ActRchTxtEdiCrsLnk,	// #1099
	ActRchTxtEdiAss,	// #1100
	ActRcvRchTxtCrsInf,	// #1101
	ActRcvRchTxtTchGui,	// #1102
	ActRcvRchTxtSylLec,	// #1103
	ActRcvRchTxtSylPra,	// #1104
	ActRcvRchTxtBib,	// #1105
	ActRcvRchTxtFAQ,	// #1106
	ActRcvRchTxtCrsLnk,	// #1107
	ActRcvRchTxtAss,	// #1108
	ActReqRemOldCrs,	// #1109
	ActRemOldCrs,		// #1110
	ActDowSeeDocCrs,	// #1111
	ActDowSeeDocGrp,	// #1112
	ActDowAdmDocCrs,	// #1113
	ActDowAdmDocGrp,	// #1114
	ActDowShaCrs,		// #1115
	ActDowShaGrp,		// #1116
        ActDowAsgUsr,		// #1117
        ActDowWrkUsr,		// #1118
        ActDowAsgCrs,		// #1119
        ActDowWrkCrs,		// #1120
        ActDowAdmMrkCrs,	// #1121
        ActDowAdmMrkGrp,	// #1122
        ActDowBrf,		// #1123
       	ActZIPSeeDocCrs,	// #1124
       	ActZIPSeeDocGrp,	// #1125
       	ActZIPAdmDocCrs,	// #1126
       	ActZIPAdmDocGrp,	// #1127
	ActZIPShaCrs,		// #1128
	ActZIPShaGrp,		// #1129
	ActZIPAsgUsr,		// #1130
	ActZIPWrkUsr,		// #1131
	ActZIPAsgCrs,		// #1132
	ActZIPWrkCrs,		// #1133
	ActZIPAdmMrkCrs,	// #1134
	ActZIPAdmMrkGrp,	// #1135
	ActZIPBrf,		// #1136
	ActSeeBan,		// #1137
	ActEdiBan,		// #1138
	ActNewBan,		// #1139
	ActRemBan,		// #1140
	ActRenBanSho,		// #1141
	ActRenBanFul,		// #1142
	ActChgBanWWW,		// #1143
	ActChgBanImg,		// #1144
	ActClkBan,		// #1145
	ActMrkNtfSee,		// #1146
	ActRemMyID,		// #1147
	ActChgMyID,		// #1148
	ActSeeDegInf,		// #1149
	ActPrnDegInf,		// #1150
	ActSeeCtrInf,		// #1151
	ActPrnCtrInf,		// #1152
	ActSeeInsInf,		// #1153
	ActPrnInsInf,		// #1154
	ActSeeCtyInf,		// #1155
	ActPrnCtyInf,		// #1156
	ActChgCtyWWW,		// #1157
	ActChgCtyMapAtt,	// #1158
	ActChgCtrPhoAtt,	// #1159
	ActReqCtrPho,		// #1160
	ActRecCtrPho,		// #1161
	-1,			// #1162 (obsolete action)
	ActCreUsrAcc,		// #1163
	ActSeeOneNot,		// #1164
	-1,			// #1165 (obsolete action)
	ActChgCtyMyIns,		// #1166
	ActChgMyIns,		// #1167
	ActChgMyCtr,		// #1168
	ActChgMyDpt,		// #1169
	ActChgMyOff,		// #1170
	ActChgMyOffPho,		// #1171
	ActReqEdiMyNet,		// #1172
	ActChgMyNet,		// #1173
	ActSeeRecOneStd,	// #1174
	ActSeeRecOneTch,	// #1175
	-1,			// #1176 (obsolete action)
	-1,			// #1177 (obsolete action)
	-1,			// #1178 (obsolete action)
	-1,			// #1179 (obsolete action)
	-1,			// #1180 (obsolete action)
	-1,			// #1181 (obsolete action)
	-1,			// #1182 (obsolete action)
	-1,			// #1183 (obsolete action)
	-1,			// #1184 (obsolete action)
	-1,			// #1185 (obsolete action)
	ActLstOth,		// #1186
	ActSeeRecSevGst,	// #1187
	ActPrnRecSevGst,	// #1188
	ActLstGstAll,		// #1189
	ActPrnGstPho,		// #1190
	ActUnhMrkCrs,		// #1191
	ActHidMrkCrs,		// #1192
	ActUnhMrkGrp,		// #1193
	ActHidMrkGrp,		// #1194
	ActChgToSeeDocCrs,	// #1195
	ActChgToAdmDocCrs,	// #1196
	ActChgToAdmSha,		// #1197
	ActChgToSeeMrk,		// #1198
	ActChgToAdmMrk,		// #1199
	ActSeeDocGrp,		// #1200
	ActAdmDocGrp,		// #1201
	ActAdmShaCrs,		// #1202
	ActAdmShaGrp,		// #1203
	ActSeeMrkGrp,		// #1204
	ActAdmMrkGrp,		// #1205
	ActReqDeg,		// #1206
	ActChgDegSta,		// #1207
	ActReqCtr,		// #1208
	ActChgCtrSta,		// #1209
	ActReqIns,		// #1210
	ActChgInsSta,		// #1211
	ActUnhBan,		// #1212
	ActHidBan,		// #1213
	ActRcvFilDocCrsDZ,	// #1214
	ActRcvFilDocGrpDZ,	// #1215
	ActRcvFilShaCrsDZ,	// #1216
	ActRcvFilShaGrpDZ,	// #1217
	ActRcvFilAsgUsrDZ,	// #1218
	ActRcvFilWrkUsrDZ,	// #1219
	ActRcvFilAsgCrsDZ,	// #1220
	ActRcvFilWrkCrsDZ,	// #1221
	ActRcvFilMrkCrsDZ,	// #1222
	ActRcvFilMrkGrpDZ,	// #1223
	ActRcvFilBrfDZ,		// #1224
	ActCreLnkDocCrs,	// #1225
	ActCreLnkShaCrs,	// #1226
	ActCreLnkShaGrp,	// #1227
	ActCreLnkWrkUsr,	// #1228
	ActCreLnkWrkCrs,	// #1229
	ActCreLnkBrf,		// #1230
	ActCreLnkDocGrp,	// #1231
	ActCreLnkAsgUsr,	// #1232
	ActCreLnkAsgCrs,	// #1233
	ActAnnSee,		// #1234
	ActSeeAnn,		// #1235
	ActRemAnn,		// #1236
	ActWriAnn,		// #1237
	ActRcvAnn,		// #1238
	-1,			// #1239 (obsolete action)
	-1,			// #1240 (obsolete action)
	-1,			// #1241 (obsolete action)
	ActSeeSyl,		// #1242
	ActChgMnu,		// #1243
	ActReqCtrLog,		// #1244
	ActReqInsLog,		// #1245
	ActReqDegLog,		// #1246
	ActSeeAdmDocDeg,	// #1247
	ActSeeAdmDocCtr,	// #1248
	ActSeeAdmDocIns,	// #1249
	ActChgToSeeDocDeg,	// #1250
	ActSeeDocDeg,		// #1251
	ActExpSeeDocDeg,	// #1252
	ActConSeeDocDeg,	// #1253
	ActZIPSeeDocDeg,	// #1254
	ActReqDatSeeDocDeg,	// #1255
	ActDowSeeDocDeg,	// #1256
	ActChgToAdmDocDeg,	// #1257
	ActAdmDocDeg,		// #1258
	ActReqRemFilDocDeg,	// #1259
	ActRemFilDocDeg,	// #1260
	ActRemFolDocDeg,	// #1261
	ActCopDocDeg,		// #1262
	ActPasDocDeg,		// #1263
	ActRemTreDocDeg,	// #1264
	ActFrmCreDocDeg,	// #1265
	ActCreFolDocDeg,	// #1266
	ActCreLnkDocDeg,	// #1267
	ActRenFolDocDeg,	// #1268
	ActRcvFilDocDegDZ,	// #1269
	ActRcvFilDocDegCla,	// #1270
	ActExpAdmDocDeg,	// #1271
	ActConAdmDocDeg,	// #1272
	ActZIPAdmDocDeg,	// #1273
	ActUnhDocDeg,		// #1274
	ActHidDocDeg,		// #1275
	ActReqDatAdmDocDeg,	// #1276
	ActChgDatAdmDocDeg,	// #1277
	ActDowAdmDocDeg,	// #1278
	ActChgToSeeDocCtr,	// #1279
	ActSeeDocCtr,		// #1280
	ActExpSeeDocCtr,	// #1281
	ActConSeeDocCtr,	// #1282
	ActZIPSeeDocCtr,	// #1283
	ActReqDatSeeDocCtr,	// #1284
	ActDowSeeDocCtr,	// #1285
	ActChgToAdmDocCtr,	// #1286
	ActAdmDocCtr,		// #1287
	ActReqRemFilDocCtr,	// #1288
	ActRemFilDocCtr,	// #1289
	ActRemFolDocCtr,	// #1290
	ActCopDocCtr,		// #1291
	ActPasDocCtr,		// #1292
	ActRemTreDocCtr,	// #1293
	ActFrmCreDocCtr,	// #1294
	ActCreFolDocCtr,	// #1295
	ActCreLnkDocCtr,	// #1296
	ActRenFolDocCtr,	// #1297
	ActRcvFilDocCtrDZ,	// #1298
	ActRcvFilDocCtrCla,	// #1299
	ActExpAdmDocCtr,	// #1300
	ActConAdmDocCtr,	// #1301
	ActZIPAdmDocCtr,	// #1302
	ActUnhDocCtr,		// #1303
	ActHidDocCtr,		// #1304
	ActReqDatAdmDocCtr,	// #1305
	ActChgDatAdmDocCtr,	// #1306
	ActDowAdmDocCtr,	// #1307
	ActChgToSeeDocIns,	// #1308
	ActSeeDocIns,		// #1309
	ActExpSeeDocIns,	// #1310
	ActConSeeDocIns,	// #1311
	ActZIPSeeDocIns,	// #1312
	ActReqDatSeeDocIns,	// #1313
	ActDowSeeDocIns,	// #1314
	ActChgToAdmDocIns,	// #1315
	ActAdmDocIns,		// #1316
	ActReqRemFilDocIns,	// #1317
	ActRemFilDocIns,	// #1318
	ActRemFolDocIns,	// #1319
	ActCopDocIns,		// #1320
	ActPasDocIns,		// #1321
	ActRemTreDocIns,	// #1322
	ActFrmCreDocIns,	// #1323
	ActCreFolDocIns,	// #1324
	ActCreLnkDocIns,	// #1325
	ActRenFolDocIns,	// #1326
	ActRcvFilDocInsDZ,	// #1327
	ActRcvFilDocInsCla,	// #1328
	ActExpAdmDocIns,	// #1329
	ActConAdmDocIns,	// #1330
	ActZIPAdmDocIns,	// #1331
	ActUnhDocIns,		// #1332
	ActHidDocIns,		// #1333
	ActReqDatAdmDocIns,	// #1334
	ActChgDatAdmDocIns,	// #1335
	ActDowAdmDocIns,	// #1336
	ActNewAdmIns,		// #1337
	ActRemAdmIns,		// #1338
	ActNewAdmCtr,		// #1339
	ActRemAdmCtr,		// #1340
	ActRemInsLog,		// #1341
	ActRemCtrLog,		// #1342
	ActRemDegLog,		// #1343
	ActAdmShaDeg,		// #1344
	ActReqRemFilShaDeg,	// #1345
	ActRemFilShaDeg,	// #1346
	ActRemFolShaDeg,	// #1347
	ActCopShaDeg,		// #1348
	ActPasShaDeg,		// #1349
	ActRemTreShaDeg,	// #1350
	ActFrmCreShaDeg,	// #1351
	ActCreFolShaDeg,	// #1352
	ActCreLnkShaDeg,	// #1353
	ActRenFolShaDeg,	// #1354
	ActRcvFilShaDegDZ,	// #1355
	ActRcvFilShaDegCla,	// #1356
	ActExpShaDeg,		// #1357
	ActConShaDeg,		// #1358
	ActZIPShaDeg,		// #1359
	ActReqDatShaDeg,	// #1360
	ActChgDatShaDeg,	// #1361
	ActDowShaDeg,		// #1362
	ActAdmShaCtr,		// #1363
	ActReqRemFilShaCtr,	// #1364
	ActRemFilShaCtr,	// #1365
	ActRemFolShaCtr,	// #1366
	ActCopShaCtr,		// #1367
	ActPasShaCtr,		// #1368
	ActRemTreShaCtr,	// #1369
	ActFrmCreShaCtr,	// #1370
	ActCreFolShaCtr,	// #1371
	ActCreLnkShaCtr,	// #1372
	ActRenFolShaCtr,	// #1373
	ActRcvFilShaCtrDZ,	// #1374
	ActRcvFilShaCtrCla,	// #1375
	ActExpShaCtr,		// #1376
	ActConShaCtr,		// #1377
	ActZIPShaCtr,		// #1378
	ActReqDatShaCtr,	// #1379
	ActChgDatShaCtr,	// #1380
	ActDowShaCtr,		// #1381
	ActAdmShaIns,		// #1382
	ActReqRemFilShaIns,	// #1383
	ActRemFilShaIns,	// #1384
	ActRemFolShaIns,	// #1385
	ActCopShaIns,		// #1386
	ActPasShaIns,		// #1387
	ActRemTreShaIns,	// #1388
	ActFrmCreShaIns,	// #1389
	ActCreFolShaIns,	// #1390
	ActCreLnkShaIns,	// #1391
	ActRenFolShaIns,	// #1392
	ActRcvFilShaInsDZ,	// #1393
	ActRcvFilShaInsCla,	// #1394
	ActExpShaIns,		// #1395
	ActConShaIns,		// #1396
	ActZIPShaIns,		// #1397
	ActReqDatShaIns,	// #1398
	ActChgDatShaIns,	// #1399
	ActDowShaIns,		// #1400
	ActReqOthPubPrf,	// #1401
	ActSeeOthPubPrf,	// #1402
	-1,			// #1403 (obsolete action)
	ActChgBasPriPrf,	// #1404
	ActCalFig,		// #1405
	-1,			// #1406 (obsolete action)
	-1,			// #1407 (obsolete action)
	-1,			// #1408 (obsolete action)
	-1,			// #1409 (obsolete action)
	ActFolUsr,		// #1410
	ActUnfUsr,		// #1411
	ActSeeFlg,		// #1412
	ActSeeFlr,		// #1413
	ActReqMdfOneOth,	// #1414
	ActReqMdfOneStd,	// #1415
	ActReqMdfOneTch,	// #1416
	-1,			// #1417 (obsolete action)
	ActReqMdfOth,		// #1418
	ActReqMdfStd,		// #1419
	ActReqMdfTch,		// #1420
	-1,			// #1421 (obsolete action)
	ActUpdOth,		// #1422
	ActUpdStd,		// #1423
	ActUpdTch,		// #1424
	-1,			// #1425 (obsolete action)
	ActReqEnrSevStd,	// #1426
	ActReqEnrSevTch,	// #1427
	ActRcvFrmEnrSevStd,	// #1428
	ActRcvFrmEnrSevTch,	// #1429
	ActReqRemMyAcc,		// #1430
	ActRemMyAcc,		// #1431
	ActReqOthPho,		// #1432
	ActReqStdPho,		// #1433
	ActReqTchPho,		// #1434
	ActDetOthPho,		// #1435
	ActDetStdPho,		// #1436
	ActDetTchPho,		// #1437
	ActChgOthPho,		// #1438
	ActChgStdPho,		// #1439
	ActChgTchPho,		// #1440
	ActRemOthPho,		// #1441
	ActRemStdPho,		// #1442
	ActRemTchPho,		// #1443
	ActCreOth,		// #1444
	ActCreStd,		// #1445
	ActCreTch,		// #1446
	-1,			// #1447 (obsolete action)
	-1,			// #1448 (obsolete action)
	-1,			// #1449 (obsolete action)
	ActRemID_Oth,		// #1450
	ActRemID_Std,		// #1451
	ActRemID_Tch,		// #1452
	ActChgID_Oth,		// #1453
	ActChgID_Std,		// #1454
	ActChgID_Tch,		// #1455
	ActReqAccEnrStd,	// #1456
	ActReqAccEnrTch,	// #1457
	ActAccEnrStd,		// #1458
	ActAccEnrTch,		// #1459
	ActRemMe_Std,		// #1460
	ActRemMe_Tch,		// #1461
	ActRemStdCrs,		// #1462
	ActRemTchCrs,		// #1463
	-1,			// #1464 (obsolete action)
	-1,			// #1465 (obsolete action)
	-1,			// #1466 (obsolete action)
	ActChgPwdOth,		// #1467
	ActChgPwdStd,		// #1468
	ActChgPwdTch,		// #1469
	ActHidAnn,		// #1470
	ActUnhAnn,		// #1471
	ActReqRemNot,		// #1472
	ActSeeLstMyAtt,		// #1473
	ActPrnLstMyAtt,		// #1474
	-1,			// #1475 (obsolete action)
	-1,			// #1476 (obsolete action)
	-1,			// #1477 (obsolete action)
	ActRemMaiOth,		// #1478
	ActRemMaiStd,		// #1479
	ActRemMaiTch,		// #1480
	ActChgMaiOth,		// #1481
	ActChgMaiStd,		// #1482
	ActChgMaiTch,		// #1483
	ActChg1stDay,		// #1484
	-1,			// #1485 (obsolete action)
	ActChgCrsTT1stDay,	// #1486
	ActChgMyTT1stDay,	// #1487
	ActReqRemOldBrf,	// #1488
	ActRemOldBrf,		// #1489
	ActSeeGblTL,		// #1490
	-1,			// #1491 (obsolete action)
	ActRcvPstGblTL,		// #1492
	ActRemPubGblTL,		// #1493
	ActReqRemPubGblTL,	// #1494
	ActShaNotGblTL,		// #1495
	ActUnsNotGblTL,		// #1496
	-1,			// #1497 (obsolete action)
	ActRcvPstUsrTL,		// #1498
	ActShaNotUsrTL,		// #1499
	ActUnsNotUsrTL,		// #1500
	ActReqRemPubUsrTL,	// #1501
	ActRemPubUsrTL,		// #1502
	ActRcvComGblTL,		// #1503
	ActRcvComUsrTL,		// #1504
	ActReqRemComGblTL,	// #1505
	ActReqRemComUsrTL,	// #1506
	ActRemComGblTL,		// #1507
	ActRemComUsrTL,		// #1508
	ActRefNewPubGblTL,	// #1509
	ActRefOldPubGblTL,	// #1510
	ActRefOldPubUsrTL,	// #1511
	ActFavNotGblTL,		// #1512
	ActUnfNotGblTL,		// #1513
	ActFavNotUsrTL,		// #1514
	ActUnfNotUsrTL,		// #1515
	ActFavComGblTL,		// #1516
	ActUnfComGblTL,		// #1517
	ActFavComUsrTL,		// #1518
	ActUnfComUsrTL,		// #1519
	ActSeeSocPrf,		// #1520
	ActFrmLogIn,		// #1521
	ActUpdSignUpReq,	// #1522
	ActReqRemOneTstQst,	// #1523
	ActReqRemSvyQst,	// #1524
	ActAdmTchCrsGrp,	// #1525
	ActChgToAdmTch,		// #1526
	ActAdmTchCrs,		// #1527
	ActReqRemFilTchCrs,	// #1528
	ActRemFilTchCrs,	// #1529
	ActRemFolTchCrs,	// #1530
	ActCopTchCrs,		// #1531
	ActPasTchCrs,		// #1532
	ActRemTreTchCrs,	// #1533
	ActFrmCreTchCrs,	// #1534
	ActCreFolTchCrs,	// #1535
	ActCreLnkTchCrs,	// #1536
	ActRenFolTchCrs,	// #1537
	ActRcvFilTchCrsDZ,	// #1538
	ActRcvFilTchCrsCla,	// #1539
	ActExpTchCrs,		// #1540
	ActConTchCrs,		// #1541
	ActZIPTchCrs,		// #1542
	ActReqDatTchCrs,	// #1543
	ActChgDatTchCrs,	// #1544
	ActDowTchCrs,		// #1545
	ActAdmTchGrp,		// #1546
	ActReqRemFilTchGrp,	// #1547
	ActRemFilTchGrp,	// #1548
	ActRemFolTchGrp,	// #1549
	ActCopTchGrp,		// #1550
	ActPasTchGrp,		// #1551
	ActRemTreTchGrp,	// #1552
	ActFrmCreTchGrp,	// #1553
	ActCreFolTchGrp,	// #1554
	ActCreLnkTchGrp,	// #1555
	ActRenFolTchGrp,	// #1556
	ActRcvFilTchGrpDZ,	// #1557
	ActRcvFilTchGrpCla,	// #1558
	ActExpTchGrp,		// #1559
	ActConTchGrp,		// #1560
	ActZIPTchGrp,		// #1561
	ActReqDatTchGrp,	// #1562
	ActChgDatTchGrp,	// #1563
	ActDowTchGrp,		// #1564
	-1,			// #1565 (obsolete action)
	-1,			// #1566 (obsolete action)
	-1,			// #1567 (obsolete action)
	ActCnfID_Oth,		// #1568
	ActCnfID_Std,		// #1569
	ActCnfID_Tch,		// #1570
	ActSeeDatCfe,		// #1571
	ActSeeOneCfe,		// #1572
	ActChgCrsYeaCfg,	// #1573
	ActReqRemOthPho,	// #1574
	ActReqRemStdPho,	// #1575
	ActReqRemTchPho,	// #1576
	ActReqRemMyPho,		// #1577
	ActLstDupUsr,		// #1578
	ActLstSimUsr,		// #1579
	ActRemDupUsr,		// #1580
	-1,			// #1581 (obsolete action)
	ActSeeMyUsgRep,		// #1582
	-1,			// #1583 (obsolete action)
	ActChkUsrAcc,		// #1584
	ActLogInNew,		// #1585
	ActReqMyUsgRep,		// #1586
	ActChgCrsDegCfg,	// #1587
	ActChgDegCtrCfg,	// #1588
	ActChgCtrInsCfg,	// #1589
	ActChgInsCtyCfg,	// #1590
	ActRenInsFulCfg,	// #1591
	ActRenInsShoCfg,	// #1592
	ActChgInsWWWCfg,	// #1593
	ActRenCtrFulCfg,	// #1594
	ActRenCtrShoCfg,	// #1595
	ActChgCtrWWWCfg,	// #1596
	ActRenDegFulCfg,	// #1597
	ActRenDegShoCfg,	// #1598
	ActChgDegWWWCfg,	// #1599
	ActRenCrsFulCfg,	// #1600
	ActRenCrsShoCfg,	// #1601
	ActSeeMyAgd,		// #1602
	ActFrmNewEvtMyAgd,	// #1603
	ActEdiOneEvtMyAgd,	// #1604
	ActNewEvtMyAgd,		// #1605
	ActChgEvtMyAgd,		// #1606
	ActReqRemEvtMyAgd,	// #1607
	ActRemEvtMyAgd,		// #1608
	ActPrvEvtMyAgd,		// #1609
	ActPubEvtMyAgd,		// #1610
	ActSeeUsrAgd,		// #1611
	ActHidEvtMyAgd,		// #1612
	ActUnhEvtMyAgd,		// #1613
	ActFrmLogInUsrAgd,	// #1614
	ActLogInUsrAgd,		// #1615
	ActLogInUsrAgdLan,	// #1616
	-1,			// #1617 (obsolete action)
	ActPrnAgdQR,		// #1618
	ActReqRemCfe,		// #1619
	ActHidCfe,		// #1620
	ActUnhCfe,		// #1621
	ActSeeCal,		// #1622
	ActPrnCal,		// #1623
	ActChgCal1stDay,	// #1624
	-1,			// #1625 (obsolete action)
	-1,			// #1626 (obsolete action)
	-1,			// #1627 (obsolete action)
	-1,			// #1628 (obsolete action)
	-1,			// #1629 (obsolete action)
	-1,			// #1630 (obsolete action)
	-1,			// #1631 (obsolete action)
	-1,			// #1632 (obsolete action)
	-1,			// #1633 (obsolete action)
	-1,			// #1634 (obsolete action)
	-1,			// #1635 (obsolete action)
	-1,			// #1636 (obsolete action)
	ActPrnOneAsg,		// #1637
	ActChgDatFmt,		// #1638
	ActReqAccEnrNET,	// #1639
	ActAccEnrNET,		// #1640
	ActRemMe_NET,		// #1641
	ActReqEnrSevNET,	// #1642
	ActRcvFrmEnrSevNET,	// #1643
	ActReqMdfNET,		// #1644
	ActCreNET,		// #1645
	ActUpdNET,		// #1646
	ActRemNETCrs,		// #1647
	ActChgCtrPlcCfg,	// #1648
	ActSeeAllGam,		// #1649
	ActSeeOneGam,		// #1650
	ActAnsMchQstStd,	// #1651
	ActFrmNewGam,		// #1652
	ActEdiOneGam,		// #1653
	ActNewGam,		// #1654
	ActChgGam,		// #1655
	ActReqRemGam,		// #1656
	ActRemGam,		// #1657
	-1,			// #1658 (obsolete action)
	-1,			// #1659 (obsolete action)
	ActHidGam,		// #1660
	ActUnhGam,		// #1661
	ActAddOneGamQst,	// #1662
	-1,			// #1663 (obsolete action)
	ActReqRemGamQst,	// #1664
	ActRemGamQst,		// #1665
	ActGamLstTstQst,	// #1666
	ActAddTstQstToGam,	// #1667
	ActUp_GamQst,		// #1668
	ActDwnGamQst,		// #1669
	ActReqNewMch,		// #1670
	ActNewMch,		// #1671
	ActFwdMch,		// #1672
	-1,			// #1673 (obsolete action)
	ActSeeAllPrj,		// #1674
	ActFrmNewPrj,		// #1675
	ActEdiOnePrj,		// #1676
	ActPrnOnePrj,		// #1677
	ActNewPrj,		// #1678
	ActChgPrj,		// #1679
	ActReqRemPrj,		// #1680
	ActRemPrj,		// #1681
	ActHidPrj,		// #1682
	ActUnhPrj,		// #1683
	ActReqAddStdPrj,	// #1684
	ActReqAddTutPrj,	// #1685
	ActReqAddEvlPrj,	// #1686
	ActAddStdPrj,		// #1687
	ActAddTutPrj,		// #1688
	ActAddEvlPrj,		// #1689
	ActReqRemStdPrj,	// #1690
	ActReqRemTutPrj,	// #1691
	ActReqRemEvlPrj,	// #1692
	ActRemStdPrj,		// #1693
	ActRemTutPrj,		// #1694
	ActRemEvlPrj,		// #1695
	ActSeeTblAllPrj,	// #1696
	ActAdmDocPrj,		// #1697
	ActReqRemFilDocPrj,	// #1698
	ActRemFilDocPrj,	// #1699
	ActRemFolDocPrj,	// #1700
	ActCopDocPrj,		// #1701
	ActPasDocPrj,		// #1702
	ActRemTreDocPrj,	// #1703
	ActFrmCreDocPrj,	// #1704
	ActCreFolDocPrj,	// #1705
	ActCreLnkDocPrj,	// #1706
	ActRenFolDocPrj,	// #1707
	ActRcvFilDocPrjDZ,	// #1708
	ActRcvFilDocPrjCla,	// #1709
	ActExpDocPrj,		// #1710
	ActConDocPrj,		// #1711
	ActZIPDocPrj,		// #1712
	ActReqDatDocPrj,	// #1713
	ActChgDatDocPrj,	// #1714
	ActDowDocPrj,		// #1715
	ActAdmAssPrj,		// #1716
	ActReqRemFilAssPrj,	// #1717
	ActRemFilAssPrj,	// #1718
	ActRemFolAssPrj,	// #1719
	ActCopAssPrj,		// #1720
	ActPasAssPrj,		// #1721
	ActRemTreAssPrj,	// #1722
	ActFrmCreAssPrj,	// #1723
	ActCreFolAssPrj,	// #1724
	ActCreLnkAssPrj,	// #1725
	ActRenFolAssPrj,	// #1726
	ActRcvFilAssPrjDZ,	// #1727
	ActRcvFilAssPrjCla,	// #1728
	ActExpAssPrj,		// #1729
	ActConAssPrj,		// #1730
	ActZIPAssPrj,		// #1731
	ActReqDatAssPrj,	// #1732
	ActChgDatAssPrj,	// #1733
	ActDowAssPrj,		// #1734
	ActFrmAccOth,		// #1735
	ActFrmAccStd,		// #1736
	ActFrmAccTch,		// #1737
	ActRemNicOth,	// #1738
	ActRemNicStd,	// #1739
	ActRemNicTch,	// #1740
	ActChgNicOth,		// #1741
	ActChgNicStd,		// #1742
	ActChgNicTch,		// #1743
	ActSeeRoo,		// #1744
	ActEdiRoo,		// #1745
	ActNewRoo,		// #1746
	ActRemRoo,		// #1747
	ActRenRooSho,		// #1748
	ActRenRooFul,		// #1749
	ActChgRooMaxUsr,	// #1750
	-1,			// #1751 (obsolete action)
	ActChgGrpRoo,		// #1752
	ActDoActOnSevGst,	// #1753
	ActDoActOnSevStd,	// #1754
	ActDoActOnSevTch,	// #1755
	ActReqFolSevStd,	// #1756
	ActReqFolSevTch,	// #1757
	ActReqUnfSevStd,	// #1758
	ActReqUnfSevTch,	// #1759
	ActFolSevStd,		// #1760
	ActFolSevTch,		// #1761
	ActUnfSevStd,		// #1762
	ActUnfSevTch,		// #1763
	ActChgCooPrf,		// #1764
	ActChgExtPriPrf,	// #1765
        ActAllShaNotGblTL,	// #1766
        ActAllFavNotGblTL,	// #1767
        ActAllFavComGblTL,	// #1768
        ActAllShaNotUsrTL,	// #1769
        ActAllFavNotUsrTL,	// #1770
        ActAllFavComUsrTL,	// #1771
	ActReqMaiUsr,		// #1772
	ActLckPrj,		// #1773
	ActUnlPrj,		// #1774
	ActReqLckAllPrj,	// #1775
	ActReqUnlAllPrj,	// #1776
	ActLckAllPrj,		// #1777
	ActUnlAllPrj,		// #1778
	-1,			// #1779 (obsolete action)
	ActJoiMch,		// #1780
	-1,			// #1781 (obsolete action)
	ActRefMchStd,		// #1782
        ActReqRemMch,		// #1783
        ActRemMch,		// #1784
	ActResMch,		// #1785
	-1,			// #1786 (obsolete action)
	ActCreMyAcc,		// #1787
	ActRefMchTch,		// #1788
	ActPlyPauMch,		// #1789
	ActBckMch,		// #1790
	-1,			// #1791 (obsolete action)
	-1,			// #1792 (obsolete action)
	-1,			// #1793 (obsolete action)
	ActChgVisResMchQst,	// #1794
	-1,			// #1795
	ActSeeMyMchResCrs,	// #1796
	ActSeeOneMchResMe,	// #1797
	ActReqSeeUsrMchRes,	// #1798
	ActSeeUsrMchResCrs,	// #1799
	ActSeeOneMchResOth,	// #1800
	ActChgVisResMchUsr,	// #1801
	ActChgNumColMch,	// #1802
	ActCfgPrj,		// #1803
	ActRcvCfgPrj,		// #1804
	ActReqUsrPrj,		// #1805
	ActShoHidComGblTL,	// #1806
	ActShoHidComUsrTL,	// #1807
        ActSeeMchAnsQstStd,	// #1808
        ActRemMchAnsQstStd,	// #1809
        ActSeeMyMchResGam,	// #1810
        ActSeeUsrMchResGam,	// #1811
        ActSeeMyMchResMch,	// #1812
        ActSeeUsrMchResMch,	// #1813
	ActMchCntDwn,		// #1814
	ActChgCtrLatCfg,	// #1815
	ActChgCtrLgtCfg,	// #1816
	ActChgCtrAltCfg,	// #1817
	ActSeeSysInf,		// #1818
	ActPrnSysInf,		// #1819
	ActMtn,			// #1820
	ActSeePrg,		// #1821
	ActFrmNewPrgItm,	// #1822
	ActFrmChgPrgItm,	// #1823
	-1,			// #1824
	ActNewPrgItm,		// #1825
	ActChgPrgItm,		// #1826
	ActReqRemPrgItm,	// #1827
	ActRemPrgItm,		// #1828
	ActHidPrgItm,		// #1829
	ActUnhPrgItm,		// #1830
	ActUp_PrgItm,		// #1832
	ActDwnPrgItm,		// #1833
	ActRgtPrgItm,		// #1833
	ActLftPrgItm,		// #1834
	ActReqRemSevTstQst,	// #1835
	ActRemSevTstQst,	// #1836
	ActReqAssTst,		// #1837
	ActSeeBld,		// #1838
	ActEdiBld,		// #1839
	ActNewBld,		// #1840
	ActRemBld,		// #1841
	ActRenBldSho,		// #1842
	ActRenBldFul,		// #1843
	ActRenBldLoc,		// #1844
	ActChgRooBld,		// #1845
	ActChgRooFlo,		// #1846
	ActChgRooTyp,		// #1847
	ActSeeAllExa,		// #1848
	ActSeeOneExa,		// #1849
	ActReqRemExaSes,	// #1850
	ActRemExaSes,		// #1851
	ActReqNewExaSes,	// #1852
	ActNewExaSes,		// #1853
	-1,			// #1854 (obsolete action)
	-1,			// #1855 (obsolete action)
	-1,			// #1856 (obsolete action)
	-1,			// #1857 (obsolete action)
	-1,			// #1858 (obsolete action)
	-1,			// #1859 (obsolete action)
	-1,			// #1860 (obsolete action)
	-1,			// #1861 (obsolete action)
	-1,			// #1862 (obsolete action)
	-1,			// #1863 (obsolete action)
	-1,			// #1864 (obsolete action)
	-1,			// #1865 (obsolete action)
	-1,			// #1866 (obsolete action)
	ActSeeMyExaResCrs,	// #1867
	ActSeeMyExaResExa,	// #1868
	ActSeeMyExaResSes,	// #1869
	ActSeeOneExaResMe,	// #1870
	ActReqSeeUsrExaRes,	// #1871
	ActSeeUsrExaResCrs,	// #1872
	ActSeeUsrExaResExa,	// #1873
	ActSeeUsrExaResSes,	// #1874
	ActSeeOneExaResOth,	// #1875
	ActChgVisExaRes,	// #1876
	ActFrmNewExa,		// #1877
	ActEdiOneExa,		// #1878
	ActNewExa,		// #1879
	ActChgExa,		// #1880
	ActReqRemExa,		// #1881
	ActRemExa,		// #1882
	ActHidExa,		// #1883
	ActUnhExa,		// #1884
	ActReqAddQstExaSet,	// #1885
	ActLstTstQstForSet,	// #1886
	ActAddQstToExa,		// #1887
	ActReqRemSetQst,	// #1888
	ActRemExaQst,		// #1889
	-1,			// #1890 (obsolete action)
	-1,			// #1891 (obsolete action)
	-1,			// #1892 (obsolete action)
	ActReqRemExaSet,	// #1893
	ActRemExaSet,		// #1894
	ActUp_ExaSet,		// #1895
	ActDwnExaSet,		// #1896
	ActChgTitExaSet,	// #1897
	ActNewExaSet,		// #1898
	ActChgNumQstExaSet,	// #1899
	ActHidExaSes,		// #1900
	ActUnhExaSes,		// #1901
	ActEdiOneExaSes,	// #1902
	ActChgExaSes,		// #1903
	ActSeeExaPrn,		// #1904
	-1,			// #1905 (obsolete action)
	ActAnsExaPrn,		// #1906
	ActEdiTag,		// #1907
	ActEndExaPrn,		// #1908
	ActValSetQst,		// #1909
	ActInvSetQst,		// #1910
	ActChgRooMAC,		// #1911
	ActLstOneGam,		// #1912
	ActEdiMch,		// #1913
	ActChgMch,		// #1914
	-1,			// #1915 (obsolete action)
	-1,			// #1916 (obsolete action)
	ActChgUsrPho,		// #1917
	ActFrmEdiPrgRsc,	// #1918
	ActReqRemPrgRsc,	// #1919
	ActRemPrgRsc,		// #1920
	ActHidPrgRsc,		// #1921
	ActUnhPrgRsc,		// #1922
	ActUp_PrgRsc,		// #1923
	ActDwnPrgRsc,		// #1924
	ActFrmSeePrgRsc,	// #1925
	ActEdiPrg,		// #1926
	ActSeePrgItm,		// #1927
	ActRenPrgRsc,		// #1928
	ActNewPrgRsc,		// #1929
	ActReqLnkSeeDocCrs,	// #1930
	ActReqLnkAdmDocCrs,	// #1931
	ActFrmChgLnkPrgRsc,	// #1932
	ActChgLnkPrgRsc,	// #1933
	ActReqLnkCfe,		// #1934
	ActReqLnkGam,		// #1935
	ActReqLnkExa,		// #1936
	ActReqLnkSvy,		// #1937
	ActReqLnkAtt,		// #1938
	ActReqLnkSeeMrkCrs,	// #1939
	ActReqLnkAdmMrkCrs,	// #1940
	ActReqLnkForCrsUsr,	// #1941
	ActSeeOneAsg,		// #1942
	ActReqLnkAsg,		// #1943
	ActExpSeePrgItm,	// #1944
	ActConSeePrgItm,	// #1945
	ActExpEdiPrgItm,	// #1946
	ActConEdiPrgItm,	// #1947
	ActReqLnkPrj,		// #1948
	ActSeeOnePrj,		// #1949
	ActChgPrjRev,		// #1950
	ActSeeAllRub,		// #1951
	ActSeeOneRub,		// #1952
	ActFrmNewRub,		// #1953
	ActEdiOneRub,		// #1954
	ActNewRub,		// #1955
	ActChgRub,		// #1956
	ActReqRemRub,		// #1957
	ActRemRub,		// #1958
	ActNewRubCri,		// #1959
	ActReqRemRubCri,	// #1960
	ActRemRubCri,		// #1961
	ActUp_RubCri,		// #1962
	ActDwnRubCri,		// #1963
	ActChgTitRubCri,	// #1964
	ActChgMinRubCri,	// #1965
	ActChgMaxRubCri,	// #1966
	ActChgWeiRubCri,	// #1967
	ActChgLnkRubCri,	// #1968
	ActReqLnkRub,		// #1969
	ActSeeRscCli_InPrg,	// #1970
	ActRemRscCli_InPrg,	// #1971
	ActSeeRscCli_InRub,	// #1972
	ActRemRscCli_InRub,	// #1973
	ActChgPrjSco,		// #1974
  };
