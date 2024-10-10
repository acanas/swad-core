// swad_action_list.c: action list

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
#include "swad_group_resource.h"
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
#include "swad_tag_resource.h"
#include "swad_test_resource.h"
#include "swad_timeline.h"
#include "swad_timeline_comment.h"
#include "swad_timeline_favourite.h"
#include "swad_timeline_share.h"
#include "swad_timeline_who.h"
#include "swad_timetable.h"
#include "swad_user_clipboard.h"
#include "swad_zip.h"

/*****************************************************************************/
/************************** Public global variables **************************/
/*****************************************************************************/

const struct Act_Actions ActLst_Actions[ActLst_NUM_ACTIONS] =
  {
   //  ******************************************************************
   // Actions not in menu:
   [ActAll		] = { 645, 0,TabUnk,NULL			,NULL				,{{    0,    0},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3F8}},Act_NORM,Act_1ST},	// Used for statistics
   [ActUnk		] = { 194, 0,TabUnk,NULL			,NULL				,{{    0,    0},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3F8}},Act_NORM,Act_1ST},
   [ActMnu		] = {   2, 0,TabUnk,NULL			,NULL				,{{    0,    0},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3F8}},Act_NORM,Act_1ST},
   [ActRefCon		] = { 845, 0,TabUnk,NULL			,Lay_RefreshNotifsAndConnected	,{{    0,    0},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3F8}},Act_NORM,Act_REF},
   [ActWebSvc		] = { 892, 0,TabUnk,NULL			,API_WebService			,{{    0,    0},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3F8}},Act_NORM,Act_SVC},

   // TabStr ******************************************************************
   // Log in
   [ActFrmLogIn		] = {1521, 0,TabStr,NULL			,Usr_WriteLandingPage		,{{    0,    0},{0x001,0x001},{0x001,0x001},{0x001,0x001},{0x001,0x001},{0x001,0x001},{0x001,    0}},Act_NORM,Act_1ST},
   [ActLogIn		] = {   6, 0,TabStr,NULL			,Usr_WelcomeUsr			,{{    0,    0},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3F8}},Act_NORM,Act_1ST},
   [ActLogInNew		] = {1585, 0,TabStr,NULL			,Usr_WelcomeUsr			,{{    0,    0},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3F8}},Act_NORM,Act_1ST},
   [ActLogInLan		] = {1077, 0,TabStr,NULL			,Usr_WelcomeUsr			,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_1ST},
   [ActAnnSee		] = {1234, 0,TabStr,NULL			,Ann_MarkAnnouncementAsSeen	,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_1ST},
   [ActReqSndNewPwd	] = { 665, 0,TabStr,NULL			,Pwd_ShowFormSendNewPwd		,{{    0,    0},{0x001,0x001},{0x001,0x001},{0x001,0x001},{0x001,0x001},{0x001,0x001},{0x001,    0}},Act_NORM,Act_1ST},
   [ActSndNewPwd	] = { 633, 0,TabStr,NULL			,Pwd_ChkIdLoginAndSendNewPwd	,{{    0,    0},{0x001,0x001},{0x001,0x001},{0x001,0x001},{0x001,0x001},{0x001,0x001},{0x001,    0}},Act_NORM,Act_1ST},
   [ActLogOut		] = {  10, 0,TabStr,Ses_CloseSession		,Usr_Logout			,{{    0,    0},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3F8}},Act_NORM,Act_1ST},

   // Search
   [ActReqSch		] = { 627, 1,TabStr,NULL			,Sch_ReqSysSearch		,{{    0,    0},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3F8}},Act_NORM,Act_1ST},
   [ActSch		] = { 628, 1,TabStr,Sch_GetParsSearch		,Sch_SysSearch			,{{    0,    0},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3F8}},Act_NORM,Act_1ST},

   // Timeline
   [ActSeeGblTL		] = {1490, 2,TabStr,TmlWho_GetParWho		,Tml_ShowTimelineGbl		,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_1ST},
   [ActRefNewPubGblTL	] = {1509, 2,TabStr,TmlWho_GetParWho		,Tml_RefreshNewTimelineGbl	,{{    0,    0},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3F8}},Act_NORM,Act_REF},
   [ActRefOldPubGblTL	] = {1510, 2,TabStr,TmlWho_GetParWho		,Tml_RefreshOldTimelineGbl	,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_AJA},
   [ActRcvPstGblTL	] = {1492, 2,TabStr,TmlWho_GetParWho		,TmlPst_ReceivePostGbl		,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_DATA,Act_1ST},
   [ActRcvComGblTL	] = {1503, 2,TabStr,TmlWho_GetParWho		,TmlCom_ReceiveCommGbl		,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_DATA,Act_1ST},
   [ActShoHidComGblTL	] = {1806, 2,TabStr,NULL			,TmlCom_ShowHiddenCommsGbl	,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_AJA},
   [ActAllShaNotGblTL	] = {1766, 2,TabStr,NULL			,TmlSha_ShowAllSharersNoteGbl	,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_AJA},
   [ActAllFavNotGblTL	] = {1767, 2,TabStr,NULL			,TmlFav_ShowAllFaversNoteGbl	,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_AJA},
   [ActAllFavComGblTL	] = {1768, 2,TabStr,NULL			,TmlFav_ShowAllFaversComGbl	,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_AJA},
   [ActShaNotGblTL	] = {1495, 2,TabStr,NULL			,TmlSha_ShaNoteGbl		,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_AJA},
   [ActUnsNotGblTL	] = {1496, 2,TabStr,NULL			,TmlSha_UnsNoteGbl		,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_AJA},
   [ActFavNotGblTL	] = {1512, 2,TabStr,NULL			,TmlFav_FavNoteGbl		,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_AJA},
   [ActUnfNotGblTL	] = {1513, 2,TabStr,NULL			,TmlFav_UnfNoteGbl		,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_AJA},
   [ActFavComGblTL	] = {1516, 2,TabStr,NULL			,TmlFav_FavCommGbl		,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_AJA},
   [ActUnfComGblTL	] = {1517, 2,TabStr,NULL			,TmlFav_UnfCommGbl		,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_AJA},
   [ActReqRemPubGblTL	] = {1494, 2,TabStr,TmlWho_GetParWho		,TmlNot_ReqRemNoteGbl		,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_1ST},
   [ActRemPubGblTL	] = {1493, 2,TabStr,TmlWho_GetParWho		,TmlNot_RemoveNoteGbl		,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_1ST},
   [ActReqRemComGblTL	] = {1505, 2,TabStr,TmlWho_GetParWho		,TmlCom_ReqRemComGbl		,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_1ST},
   [ActRemComGblTL	] = {1507, 2,TabStr,TmlWho_GetParWho		,TmlCom_RemoveComGbl		,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_1ST},

   // Profiles
   [ActSeeSocPrf	] = {1520, 3,TabStr,NULL			,Prf_SeeSocialProfiles		,{{    0,    0},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3F8}},Act_NORM,Act_1ST},
   [ActReqOthPubPrf	] = {1401, 3,TabStr,NULL			,Prf_ReqUserProfile		,{{    0,    0},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3F8}},Act_NORM,Act_1ST},
   [ActRefOldPubUsrTL	] = {1511, 3,TabStr,NULL			,Tml_RefreshOldTimelineUsr	,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_AJA},
   [ActRcvPstUsrTL	] = {1498, 3,TabStr,NULL			,TmlPst_ReceivePostUsr		,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_DATA,Act_1ST},
   [ActRcvComUsrTL	] = {1504, 3,TabStr,NULL			,TmlCom_ReceiveCommUsr		,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_DATA,Act_1ST},
   [ActShoHidComUsrTL	] = {1807, 3,TabStr,NULL			,TmlCom_ShowHiddenCommsUsr	,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_AJA},
   [ActAllShaNotUsrTL	] = {1769, 3,TabStr,NULL			,TmlSha_ShowAllSharersNoteUsr	,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_AJA},
   [ActAllFavNotUsrTL	] = {1770, 3,TabStr,NULL			,TmlFav_ShowAllFaversNoteUsr	,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_AJA},
   [ActAllFavComUsrTL	] = {1771, 3,TabStr,NULL			,TmlFav_ShowAllFaversComUsr	,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_AJA},
   [ActShaNotUsrTL	] = {1499, 3,TabStr,NULL			,TmlSha_ShaNoteUsr		,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_AJA},
   [ActUnsNotUsrTL	] = {1500, 3,TabStr,NULL			,TmlSha_UnsNoteUsr		,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_AJA},
   [ActFavNotUsrTL	] = {1514, 3,TabStr,NULL			,TmlFav_FavNoteUsr		,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_AJA},
   [ActUnfNotUsrTL	] = {1515, 3,TabStr,NULL			,TmlFav_UnfNoteUsr		,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_AJA},
   [ActFavComUsrTL	] = {1518, 3,TabStr,NULL			,TmlFav_FavCommUsr		,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_AJA},
   [ActUnfComUsrTL	] = {1519, 3,TabStr,NULL			,TmlFav_UnfCommUsr		,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_AJA},
   [ActReqRemPubUsrTL	] = {1501, 3,TabStr,NULL			,TmlNot_ReqRemNoteUsr		,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_1ST},
   [ActRemPubUsrTL	] = {1502, 3,TabStr,NULL			,TmlNot_RemoveNoteUsr		,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_1ST},
   [ActReqRemComUsrTL	] = {1506, 3,TabStr,NULL			,TmlCom_ReqRemComUsr		,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_1ST},
   [ActRemComUsrTL	] = {1508, 3,TabStr,NULL			,TmlCom_RemoveComUsr		,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_1ST},
   [ActSeeOthPubPrf	] = {1402, 3,TabStr,NULL			,Prf_GetUsrDatAndShowUserProfile,{{    0,    0},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3F8}},Act_NORM,Act_1ST},
   [ActCalFig		] = {1405, 3,TabStr,NULL			,Prf_CalculateFigures		,{{    0,    0},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3F8}},Act_NORM,Act_1ST},
   [ActFolUsr		] = {1410, 3,TabStr,Fol_FollowUsr1		,Fol_FollowUsr2			,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_1ST},
   [ActUnfUsr		] = {1411, 3,TabStr,Fol_UnfollowUsr1		,Fol_UnfollowUsr2		,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_1ST},
   [ActSeeFlg		] = {1412, 3,TabStr,NULL			,Fol_ListFollowing		,{{    0,    0},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3F8}},Act_NORM,Act_1ST},
   [ActSeeFlr		] = {1413, 3,TabStr,NULL			,Fol_ListFollowers		,{{    0,    0},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3F8}},Act_NORM,Act_1ST},
   [ActSeeUsrAgd	] = {1611, 3,TabStr,NULL			,Agd_ShowUsrAgenda		,{{    0,    0},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3F8}},Act_NORM,Act_1ST},

   // Calendar
   [ActSeeCal		] = {1622, 4,TabStr,NULL			,Cal_ShowCalendar		,{{    0,    0},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3F8}},Act_NORM,Act_1ST},
   [ActPrnCal		] = {1623, 4,TabStr,NULL			,Cal_PrintCalendar		,{{    0,    0},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3F8}},Act_NORM,Act_NEW},
   [ActChgCal1stDay	] = {1624, 4,TabStr,Cal_Change1stDayOfWeek	,Cal_ShowCalendar		,{{    0,    0},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3F8}},Act_NORM,Act_1ST},

   // Notifications
   [ActSeeNtf         	] = { 990, 5,TabStr,NULL			,Ntf_ShowMyNotifications	,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_1ST},
   [ActSeeNewNtf	] = { 991, 5,TabStr,NULL			,Ntf_ShowMyNotifications	,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_1ST},
   [ActMrkNtfSee	] = {1146, 5,TabStr,NULL			,Ntf_MarkAllNotifAsSeen		,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_1ST},
   [ActSeeMai		] = { 855, 5,TabStr,NULL			,Mai_SeeMailDomains		,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_1ST},
   [ActEdiMai		] = { 856, 5,TabStr,NULL			,Mai_EditMailDomains		,{{    0,    0},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200}},Act_NORM,Act_1ST},
   [ActNewMai		] = { 857, 5,TabStr,Mai_ReceiveNewMailDom	,Mai_ContEditAfterChgMai	,{{    0,    0},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200}},Act_NORM,Act_1ST},
   [ActRemMai		] = { 860, 5,TabStr,Mai_RemoveMailDom		,Mai_ContEditAfterChgMai	,{{    0,    0},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200}},Act_NORM,Act_1ST},
   [ActRenMaiSho	] = { 858, 5,TabStr,Mai_RenameMailDomShrt	,Mai_ContEditAfterChgMai	,{{    0,    0},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200}},Act_NORM,Act_1ST},
   [ActRenMaiFul	] = { 859, 5,TabStr,Mai_RenameMailDomFull	,Mai_ContEditAfterChgMai	,{{    0,    0},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200}},Act_NORM,Act_1ST},

   // TabSys ******************************************************************
   // System info
   [ActSeeSysInf	] = {1818, 0,TabSys,NULL			,SysCfg_ShowConfiguration	,{{    0,    0},{0x3C7,0x3C7},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActPrnSysInf	] = {1819, 0,TabSys,NULL			,SysCfg_PrintConfiguration	,{{    0,    0},{0x3C7,0x3C7},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_NEW},

   // Countries
   [ActSeeCty		] = { 862, 1,TabSys,Cty_ListCountries1		,Cty_ListCountries2		,{{    0,    0},{0x3C7,0x3C7},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActEdiCty		] = { 863, 1,TabSys,NULL			,Cty_EditCountries		,{{    0,    0},{0x200,0x200},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActNewCty		] = { 864, 1,TabSys,Cty_ReceiveNewCountry	,Cty_ContEditAfterChgCty	,{{    0,    0},{0x200,0x200},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActRemCty		] = { 893, 1,TabSys,Cty_RemoveCountry		,Cty_ContEditAfterChgCty	,{{    0,    0},{0x200,0x200},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActRenCty		] = { 866, 1,TabSys,Cty_RenameCountry		,Cty_ContEditAfterChgCty	,{{    0,    0},{0x200,0x200},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActChgCtyWWW	] = {1157, 1,TabSys,Cty_ChangeCtyWWW		,Cty_ContEditAfterChgCty	,{{    0,    0},{0x200,0x200},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},

   // Pending
   [ActSeePen		] = {1060, 2,TabSys,NULL			,Hie_SeePending			,{{    0,    0},{0x3C0,0x3C0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},

   // Links
   [ActSeeLnk 		] = { 748, 3,TabSys,NULL			,Lnk_SeeLinks			,{{    0,    0},{0x3C7,0x3C7},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActEdiLnk		] = { 749, 3,TabSys,NULL			,Lnk_EditLinks			,{{    0,    0},{0x200,0x200},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActNewLnk		] = { 750, 3,TabSys,Lnk_ReceiveNewLink		,Lnk_ContEditAfterChgLnk	,{{    0,    0},{0x200,0x200},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActRemLnk		] = { 897, 3,TabSys,Lnk_RemoveLink		,Lnk_ContEditAfterChgLnk	,{{    0,    0},{0x200,0x200},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActRenLnkSho	] = { 753, 3,TabSys,Lnk_RenameLinkShort		,Lnk_ContEditAfterChgLnk	,{{    0,    0},{0x200,0x200},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActRenLnkFul	] = { 751, 3,TabSys,Lnk_RenameLinkFull		,Lnk_ContEditAfterChgLnk	,{{    0,    0},{0x200,0x200},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActChgLnkWWW	] = { 752, 3,TabSys,Lnk_ChangeLinkWWW		,Lnk_ContEditAfterChgLnk	,{{    0,    0},{0x200,0x200},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActSeeBan		] = {1137, 3,TabSys,NULL			,Ban_ShowAllBanners		,{{    0,    0},{0x3FF,0x3FF},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActEdiBan		] = {1138, 3,TabSys,NULL			,Ban_EditBanners		,{{    0,    0},{0x200,0x200},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActNewBan		] = {1139, 3,TabSys,Ban_ReceiveNewBanner	,Ban_ContEditAfterChgBan	,{{    0,    0},{0x200,0x200},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActRemBan		] = {1140, 3,TabSys,Ban_RemoveBanner		,Ban_ContEditAfterChgBan	,{{    0,    0},{0x200,0x200},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActUnhBan		] = {1212, 3,TabSys,Ban_UnhideBanner		,Ban_ContEditAfterChgBan	,{{    0,    0},{0x200,0x200},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActHidBan		] = {1213, 3,TabSys,Ban_HideBanner		,Ban_ContEditAfterChgBan	,{{    0,    0},{0x200,0x200},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActRenBanSho	] = {1141, 3,TabSys,Ban_RenameBannerShort	,Ban_ContEditAfterChgBan	,{{    0,    0},{0x200,0x200},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActRenBanFul	] = {1142, 3,TabSys,Ban_RenameBannerFull	,Ban_ContEditAfterChgBan	,{{    0,    0},{0x200,0x200},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActChgBanImg	] = {1144, 3,TabSys,Ban_ChangeBannerImg		,Ban_ContEditAfterChgBan	,{{    0,    0},{0x200,0x200},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActChgBanWWW	] = {1143, 3,TabSys,Ban_ChangeBannerWWW		,Ban_ContEditAfterChgBan	,{{    0,    0},{0x200,0x200},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActClkBan		] = {1145, 3,TabSys,Ban_ClickOnBanner		,NULL				,{{    0,    0},{0x3C7,0x3C7},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_DWN},

   // Plugins
   [ActSeePlg		] = { 777, 4,TabSys,NULL			,Plg_ListPlugins		,{{    0,    0},{0x200,0x200},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActEdiPlg		] = { 778, 4,TabSys,NULL			,Plg_EditPlugins		,{{    0,    0},{0x200,0x200},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActNewPlg		] = { 779, 4,TabSys,Plg_ReceiveNewPlg		,Plg_ContEditAfterChgPlg	,{{    0,    0},{0x200,0x200},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActRemPlg		] = { 889, 4,TabSys,Plg_RemovePlugin		,Plg_ContEditAfterChgPlg	,{{    0,    0},{0x200,0x200},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActRenPlg		] = { 782, 4,TabSys,Plg_RenamePlugin		,Plg_ContEditAfterChgPlg	,{{    0,    0},{0x200,0x200},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActChgPlgDes	] = { 888, 4,TabSys,Plg_ChangePlgDesc		,Plg_ContEditAfterChgPlg	,{{    0,    0},{0x200,0x200},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActChgPlgLog	] = { 781, 4,TabSys,Plg_ChangePlgLogo		,Plg_ContEditAfterChgPlg	,{{    0,    0},{0x200,0x200},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActChgPlgAppKey	] = { 986, 4,TabSys,Plg_ChangePlgAppKey		,Plg_ContEditAfterChgPlg	,{{    0,    0},{0x200,0x200},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActChgPlgURL	] = { 783, 4,TabSys,Plg_ChangePlgURL		,Plg_ContEditAfterChgPlg	,{{    0,    0},{0x200,0x200},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActChgPlgIP		] = { 780, 4,TabSys,Plg_ChangePlgIP		,Plg_ContEditAfterChgPlg	,{{    0,    0},{0x200,0x200},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},

   // Maintenance
   [ActMtn		] = {1820, 5,TabSys,NULL			,Mtn_Maintenance		,{{    0,    0},{0x200,0x200},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActSetUp		] = { 840, 5,TabSys,NULL			,Mtn_SetUp			,{{    0,    0},{0x200,0x200},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActReqRemOldCrs	] = {1109, 5,TabSys,NULL			,Mtn_RemoveOldCrss		,{{    0,    0},{0x200,0x200},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActRemOldCrs	] = {1110, 5,TabSys,NULL			,Crs_RemoveOldCrss		,{{    0,    0},{0x200,0x200},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},

   // TabCty ******************************************************************
   // Country info
   [ActSeeCtyInf	] = {1155, 0,TabCty,NULL			,CtyCfg_ShowConfiguration	,{{    0,    0},{    0,    0},{0x3C7,0x3C7},{    0,    0},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActPrnCtyInf	] = {1156, 0,TabCty,NULL			,CtyCfg_PrintConfiguration	,{{    0,    0},{    0,    0},{0x3C7,0x3C7},{    0,    0},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_NEW},
   [ActChgCtyMapAtt	] = {1158, 0,TabCty,NULL			,CtyCfg_ChangeCtyMapAttr	,{{    0,    0},{    0,    0},{0x200,0x200},{    0,    0},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},

   // Institutions
   [ActSeeIns		] = { 696, 1,TabCty,NULL			,Ins_ShowInssOfCurrentCty	,{{    0,    0},{    0,    0},{0x3C7,0x3C7},{    0,    0},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActEdiIns		] = { 697, 1,TabCty,NULL			,Ins_EditInstitutions		,{{    0,    0},{    0,    0},{0x3C6,0x3C6},{    0,    0},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActReqIns		] = {1210, 1,TabCty,Ins_ReceiveReqIns		,Ins_ContEditAfterChgIns	,{{    0,    0},{    0,    0},{0x3C6,0x3C6},{    0,    0},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActNewIns		] = { 698, 1,TabCty,Ins_ReceiveNewIns		,Ins_ContEditAfterChgIns	,{{    0,    0},{    0,    0},{0x200,0x200},{    0,    0},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActRemIns		] = { 759, 1,TabCty,Ins_RemoveInstitution	,Ins_ContEditAfterChgIns	,{{    0,    0},{    0,    0},{0x3C6,0x3C6},{    0,    0},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActRenInsSho	] = { 702, 1,TabCty,Ins_RenameInsShort		,Ins_ContEditAfterChgIns	,{{    0,    0},{    0,    0},{0x3C6,0x3C6},{    0,    0},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActRenInsFul	] = { 701, 1,TabCty,Ins_RenameInsFull		,Ins_ContEditAfterChgIns	,{{    0,    0},{    0,    0},{0x3C6,0x3C6},{    0,    0},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActChgInsWWW	] = { 700, 1,TabCty,Ins_ChangeInsWWW		,Ins_ContEditAfterChgIns	,{{    0,    0},{    0,    0},{0x3C6,0x3C6},{    0,    0},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActChgInsSta	] = {1211, 1,TabCty,Ins_ChangeInsStatus		,Ins_ContEditAfterChgIns	,{{    0,    0},{    0,    0},{0x3C6,0x3C6},{    0,    0},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},

   // TabIns ******************************************************************
   // Institution info
   [ActSeeInsInf	] = {1153, 0,TabIns,NULL			,InsCfg_ShowConfiguration	,{{    0,    0},{    0,    0},{    0,    0},{0x3C7,0x3C7},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActPrnInsInf	] = {1154, 0,TabIns,NULL			,InsCfg_PrintConfiguration	,{{    0,    0},{    0,    0},{    0,    0},{0x3C7,0x3C7},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_NEW},
   [ActChgInsCtyCfg	] = {1590, 0,TabIns,InsCfg_ChangeInsCty		,InsCfg_ContEditAfterChgIns	,{{    0,    0},{    0,    0},{    0,    0},{0x200,0x200},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActRenInsShoCfg	] = {1592, 0,TabIns,InsCfg_RenameInsShort	,InsCfg_ContEditAfterChgIns	,{{    0,    0},{    0,    0},{    0,    0},{0x200,0x200},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActRenInsFulCfg	] = {1591, 0,TabIns,InsCfg_RenameInsFull	,InsCfg_ContEditAfterChgIns	,{{    0,    0},{    0,    0},{    0,    0},{0x200,0x200},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActChgInsWWWCfg	] = {1593, 0,TabIns,NULL			,InsCfg_ChangeInsWWW		,{{    0,    0},{    0,    0},{    0,    0},{0x300,0x300},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActReqInsLog	] = {1245, 0,TabIns,NULL			,InsCfg_ReqLogo			,{{    0,    0},{    0,    0},{    0,    0},{0x300,0x300},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActRecInsLog	] = { 699, 0,TabIns,InsCfg_ReceiveLogo		,InsCfg_ShowConfiguration	,{{    0,    0},{    0,    0},{    0,    0},{0x300,0x300},{    0,    0},{    0,    0},{    0,    0}},Act_DATA,Act_1ST},
   [ActRemInsLog	] = {1341, 0,TabIns,InsCfg_RemoveLogo		,InsCfg_ShowConfiguration	,{{    0,    0},{    0,    0},{    0,    0},{0x300,0x300},{    0,    0},{    0,    0},{    0,    0}},Act_DATA,Act_1ST},

   // Centers
   [ActSeeCtr		] = { 676, 1,TabIns,NULL			,Ctr_ShowCtrsOfCurrentIns	,{{    0,    0},{    0,    0},{    0,    0},{0x3C7,0x3C7},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActEdiCtr		] = { 681, 1,TabIns,NULL			,Ctr_EditCenters		,{{    0,    0},{    0,    0},{    0,    0},{0x3C6,0x3C6},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActReqCtr		] = {1208, 1,TabIns,Ctr_ReceiveReqCtr		,Ctr_ContEditAfterChgCtr	,{{    0,    0},{    0,    0},{    0,    0},{0x3C6,0x3C6},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActNewCtr		] = { 685, 1,TabIns,Ctr_ReceiveNewCtr		,Ctr_ContEditAfterChgCtr	,{{    0,    0},{    0,    0},{    0,    0},{0x300,0x300},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActRemCtr		] = { 686, 1,TabIns,Ctr_RemoveCenter		,Ctr_ContEditAfterChgCtr	,{{    0,    0},{    0,    0},{    0,    0},{0x3C6,0x3C6},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActChgCtrPlc	] = { 706, 1,TabIns,Ctr_ChangeCtrPlc		,Ctr_ContEditAfterChgCtr	,{{    0,    0},{    0,    0},{    0,    0},{0x300,0x300},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActRenCtrSho	] = { 682, 1,TabIns,Ctr_RenameCenterShrt	,Ctr_ContEditAfterChgCtr	,{{    0,    0},{    0,    0},{    0,    0},{0x3C6,0x3C6},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActRenCtrFul	] = { 684, 1,TabIns,Ctr_RenameCenterFull	,Ctr_ContEditAfterChgCtr	,{{    0,    0},{    0,    0},{    0,    0},{0x3C6,0x3C6},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActChgCtrWWW	] = { 683, 1,TabIns,Ctr_ChangeCtrWWW		,Ctr_ContEditAfterChgCtr	,{{    0,    0},{    0,    0},{    0,    0},{0x3C6,0x3C6},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActChgCtrSta	] = {1209, 1,TabIns,Ctr_ChangeCtrStatus		,Ctr_ContEditAfterChgCtr	,{{    0,    0},{    0,    0},{    0,    0},{0x300,0x300},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},

   // Places
   [ActSeePlc		] = { 703, 2,TabIns,NULL			,Plc_SeeAllPlaces		,{{    0,    0},{    0,    0},{    0,    0},{0x3C7,0x3C7},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActEdiPlc		] = { 704, 2,TabIns,NULL			,Plc_EditPlaces			,{{    0,    0},{    0,    0},{    0,    0},{0x300,0x300},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActNewPlc		] = { 705, 2,TabIns,Plc_ReceiveNewPlace		,Plc_ContEditAfterChgPlc	,{{    0,    0},{    0,    0},{    0,    0},{0x300,0x300},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActRemPlc		] = { 776, 2,TabIns,Plc_RemovePlace		,Plc_ContEditAfterChgPlc	,{{    0,    0},{    0,    0},{    0,    0},{0x300,0x300},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActRenPlcSho	] = { 894, 2,TabIns,Plc_RenamePlaceShrt		,Plc_ContEditAfterChgPlc	,{{    0,    0},{    0,    0},{    0,    0},{0x300,0x300},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActRenPlcFul	] = { 895, 2,TabIns,Plc_RenamePlaceFull		,Plc_ContEditAfterChgPlc	,{{    0,    0},{    0,    0},{    0,    0},{0x300,0x300},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},

   // Departments
   [ActSeeDpt		] = { 675, 3,TabIns,NULL			,Dpt_SeeAllDepts		,{{    0,    0},{    0,    0},{    0,    0},{0x3C7,0x3C7},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActEdiDpt		] = { 677, 3,TabIns,NULL			,Dpt_EditDepartments		,{{    0,    0},{    0,    0},{    0,    0},{0x300,0x300},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActNewDpt		] = { 687, 3,TabIns,Dpt_ReceiveNewDpt		,Dpt_ContEditAfterChgDpt	,{{    0,    0},{    0,    0},{    0,    0},{0x300,0x300},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActRemDpt		] = { 690, 3,TabIns,Dpt_RemoveDepartment	,Dpt_ContEditAfterChgDpt	,{{    0,    0},{    0,    0},{    0,    0},{0x300,0x300},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActChgDptIns	] = { 721, 3,TabIns,Dpt_ChangeDepartIns		,Dpt_ContEditAfterChgDpt	,{{    0,    0},{    0,    0},{    0,    0},{0x300,0x300},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActRenDptSho	] = { 688, 3,TabIns,Dpt_RenameDepartShrt	,Dpt_ContEditAfterChgDpt	,{{    0,    0},{    0,    0},{    0,    0},{0x300,0x300},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActRenDptFul	] = { 689, 3,TabIns,Dpt_RenameDepartFull	,Dpt_ContEditAfterChgDpt	,{{    0,    0},{    0,    0},{    0,    0},{0x300,0x300},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActChgDptWWW	] = { 691, 3,TabIns,Dpt_ChangeDptWWW		,Dpt_ContEditAfterChgDpt	,{{    0,    0},{    0,    0},{    0,    0},{0x300,0x300},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},

   // Holidays
   [ActSeeHld		] = { 707, 4,TabIns,NULL			,Hld_SeeAllHolidays		,{{    0,    0},{    0,    0},{    0,    0},{0x3C7,0x3C7},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActEdiHld		] = { 713, 4,TabIns,NULL			,Hld_EditHolidays		,{{    0,    0},{    0,    0},{    0,    0},{0x300,0x300},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActNewHld		] = { 714, 4,TabIns,Hld_ReceiveNewHoliday	,Hld_ContEditAfterChgHld	,{{    0,    0},{    0,    0},{    0,    0},{0x300,0x300},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActRemHld		] = { 716, 4,TabIns,Hld_RemoveHoliday		,Hld_ContEditAfterChgHld	,{{    0,    0},{    0,    0},{    0,    0},{0x300,0x300},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActChgHldPlc	] = { 896, 4,TabIns,Hld_ChangeHolidayPlace	,Hld_ContEditAfterChgHld	,{{    0,    0},{    0,    0},{    0,    0},{0x300,0x300},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActChgHldTyp	] = { 715, 4,TabIns,Hld_ChangeHolidayType	,Hld_ContEditAfterChgHld	,{{    0,    0},{    0,    0},{    0,    0},{0x300,0x300},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActChgHldStrDat	] = { 717, 4,TabIns,Hld_ChangeStartDate		,Hld_ContEditAfterChgHld	,{{    0,    0},{    0,    0},{    0,    0},{0x300,0x300},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActChgHldEndDat	] = { 718, 4,TabIns,Hld_ChangeEndDate		,Hld_ContEditAfterChgHld	,{{    0,    0},{    0,    0},{    0,    0},{0x300,0x300},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActRenHld		] = { 766, 4,TabIns,Hld_RenameHoliday		,Hld_ContEditAfterChgHld	,{{    0,    0},{    0,    0},{    0,    0},{0x300,0x300},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},

   // TabCtr ******************************************************************
   // Center info
   [ActSeeCtrInf	] = {1151, 0,TabCtr,NULL			,CtrCfg_ShowConfiguration	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C7,0x3C7},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActPrnCtrInf	] = {1152, 0,TabCtr,NULL			,CtrCfg_PrintConfiguration	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C7,0x3C7},{    0,    0},{    0,    0}},Act_NORM,Act_NEW},
   [ActChgCtrInsCfg	] = {1589, 0,TabCtr,CtrCfg_ChangeCtrIns		,CtrCfg_ContEditAfterChgCtr	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x200},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActRenCtrShoCfg	] = {1595, 0,TabCtr,CtrCfg_RenameCenterShrt	,CtrCfg_ContEditAfterChgCtr	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x300,0x300},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActRenCtrFulCfg	] = {1594, 0,TabCtr,CtrCfg_RenameCenterFull	,CtrCfg_ContEditAfterChgCtr	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x300,0x300},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActChgCtrPlcCfg	] = {1648, 0,TabCtr,NULL			,CtrCfg_ChangeCtrPlc		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x380,0x380},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActChgCtrLatCfg	] = {1815, 0,TabCtr,NULL			,CtrCfg_ChangeCtrLatitude	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x380,0x380},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActChgCtrLgtCfg	] = {1816, 0,TabCtr,NULL			,CtrCfg_ChangeCtrLongitude	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x380,0x380},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActChgCtrAltCfg	] = {1817, 0,TabCtr,NULL			,CtrCfg_ChangeCtrAltitude	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x380,0x380},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActChgCtrWWWCfg	] = {1596, 0,TabCtr,NULL			,CtrCfg_ChangeCtrWWW		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x380,0x380},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActReqCtrLog	] = {1244, 0,TabCtr,NULL			,CtrCfg_ReqLogo			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x380,0x380},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActRecCtrLog	] = {1051, 0,TabCtr,CtrCfg_ReceiveLogo		,CtrCfg_ShowConfiguration	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x380,0x380},{    0,    0},{    0,    0}},Act_DATA,Act_1ST},
   [ActRemCtrLog	] = {1342, 0,TabCtr,CtrCfg_RemoveLogo		,CtrCfg_ShowConfiguration	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x380,0x380},{    0,    0},{    0,    0}},Act_DATA,Act_1ST},
   [ActReqCtrPho	] = {1160, 0,TabCtr,NULL			,CtrCfg_ReqPhoto		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x380,0x380},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActRecCtrPho	] = {1161, 0,TabCtr,NULL			,CtrCfg_ReceivePhoto		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x380,0x380},{    0,    0},{    0,    0}},Act_DATA,Act_1ST},
   [ActChgCtrPhoAtt	] = {1159, 0,TabCtr,NULL			,CtrCfg_ChangeCtrPhotoAttr	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x380,0x380},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},

   // Degrees
   [ActSeeDeg		] = {1011, 1,TabCtr,NULL			,Deg_ShowDegsOfCurrentCtr	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C7,0x3C7},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActEdiDeg		] = { 536, 1,TabCtr,NULL			,Deg_EditDegrees		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C6,0x3C6},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActReqDeg		] = {1206, 1,TabCtr,Deg_ReceiveReqDeg		,Deg_ContEditAfterChgDeg	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C6,0x3C6},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActNewDeg		] = { 540, 1,TabCtr,Deg_ReceiveNewDeg		,Deg_ContEditAfterChgDeg	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x380,0x380},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActRemDeg		] = { 542, 1,TabCtr,Deg_RemoveDeg		,Deg_ContEditAfterChgDeg	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C6,0x3C6},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActRenDegSho	] = { 546, 1,TabCtr,Deg_RenameDegShrt		,Deg_ContEditAfterChgDeg	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C6,0x3C6},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActRenDegFul	] = { 547, 1,TabCtr,Deg_RenameDegFull		,Deg_ContEditAfterChgDeg	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C6,0x3C6},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActChgDegTyp	] = { 544, 1,TabCtr,Deg_ChangeDegTyp		,Deg_ContEditAfterChgDeg	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C6,0x3C6},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActChgDegWWW	] = { 554, 1,TabCtr,Deg_ChangeDegWWW		,Deg_ContEditAfterChgDeg	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C6,0x3C6},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActChgDegSta	] = {1207, 1,TabCtr,Deg_ChangeDegStatus		,Deg_ContEditAfterChgDeg	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x380,0x380},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActSeeDegTyp	] = {1013, 1,TabCtr,NULL			,DegTyp_SeeDegTypesInDegTab	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C7,0x3C7},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActEdiDegTyp	] = { 573, 1,TabCtr,NULL			,DegTyp_GetAndEditDegTypes	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x200},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActNewDegTyp	] = { 537, 1,TabCtr,DegTyp_ReceiveNewDegTyp	,DegTyp_ContEditAfterChgDegTyp	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x200},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActRemDegTyp	] = { 545, 1,TabCtr,DegTyp_RemoveDegTyp		,DegTyp_ContEditAfterChgDegTyp	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x200},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActRenDegTyp	] = { 538, 1,TabCtr,DegTyp_RenameDegTyp		,DegTyp_ContEditAfterChgDegTyp	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x200},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},

   // Buildings
   [ActSeeBld		] = {1838, 2,TabCtr,NULL			,Bld_SeeBuildings		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C7,0x3C7},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActEdiBld		] = {1839, 2,TabCtr,NULL			,Bld_EditBuildings		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C6,0x3C6},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActNewBld		] = {1840, 2,TabCtr,Bld_ReceiveNewBuilding	,Bld_ContEditAfterChgBuilding	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C6,0x3C6},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActRemBld		] = {1841, 2,TabCtr,Bld_RemoveBuilding		,Bld_ContEditAfterChgBuilding	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C6,0x3C6},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActRenBldSho	] = {1842, 2,TabCtr,Bld_RenameBuildingShrt	,Bld_ContEditAfterChgBuilding	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C6,0x3C6},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActRenBldFul	] = {1843, 2,TabCtr,Bld_RenameBuildingFull	,Bld_ContEditAfterChgBuilding	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C6,0x3C6},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActRenBldLoc	] = {1844, 2,TabCtr,Bld_ChangeBuildingLoc	,Bld_ContEditAfterChgBuilding	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C6,0x3C6},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},

   // Rooms
   [ActSeeRoo		] = {1744, 3,TabCtr,NULL			,Roo_SeeRooms			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C7,0x3C7},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActEdiRoo		] = {1745, 3,TabCtr,NULL			,Roo_EditRooms			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C6,0x3C6},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActNewRoo		] = {1746, 3,TabCtr,Roo_ReceiveNewRoom		,Roo_ContEditAfterChgRoom	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C6,0x3C6},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActRemRoo		] = {1747, 3,TabCtr,Roo_RemoveRoom		,Roo_ContEditAfterChgRoom	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C6,0x3C6},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActChgRooBld	] = {1845, 3,TabCtr,Roo_ChangeBuilding		,Roo_ContEditAfterChgRoom	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C6,0x3C6},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActChgRooFlo	] = {1846, 3,TabCtr,Roo_ChangeFloor		,Roo_ContEditAfterChgRoom	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C6,0x3C6},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActChgRooTyp	] = {1847, 3,TabCtr,Roo_ChangeType		,Roo_ContEditAfterChgRoom	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C6,0x3C6},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActRenRooSho	] = {1748, 3,TabCtr,Roo_RenameRoomShrt		,Roo_ContEditAfterChgRoom	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C6,0x3C6},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActRenRooFul	] = {1749, 3,TabCtr,Roo_RenameRoomFull		,Roo_ContEditAfterChgRoom	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C6,0x3C6},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActChgRooMaxUsr	] = {1750, 3,TabCtr,Roo_ChangeCapacity		,Roo_ContEditAfterChgRoom	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C6,0x3C6},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActChgRooMAC	] = {1911, 3,TabCtr,Roo_ChangeMAC		,Roo_ContEditAfterChgRoom	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C6,0x3C6},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},

   // TabDeg ******************************************************************
   // Degree info
   [ActSeeDegInf	] = {1149, 0,TabDeg,NULL			,DegCfg_ShowConfiguration	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C7,0x3C7},{    0,    0}},Act_NORM,Act_1ST},
   [ActPrnDegInf	] = {1150, 0,TabDeg,NULL			,DegCfg_PrintConfiguration	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C7,0x3C7},{    0,    0}},Act_NORM,Act_NEW},
   [ActChgDegCtrCfg	] = {1588, 0,TabDeg,DegCfg_ChangeDegCtr		,DegCfg_ContEditAfterChgDeg	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x300,0x300},{    0,    0}},Act_NORM,Act_1ST},
   [ActRenDegShoCfg	] = {1598, 0,TabDeg,DegCfg_RenameDegreeShrt	,DegCfg_ContEditAfterChgDeg	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x380,0x380},{    0,    0}},Act_NORM,Act_1ST},
   [ActRenDegFulCfg	] = {1597, 0,TabDeg,DegCfg_RenameDegreeFull	,DegCfg_ContEditAfterChgDeg	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x380,0x380},{    0,    0}},Act_NORM,Act_1ST},
   [ActChgDegWWWCfg	] = {1599, 0,TabDeg,NULL			,DegCfg_ChangeDegWWW		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3C0},{    0,    0}},Act_NORM,Act_1ST},
   [ActReqDegLog	] = {1246, 0,TabDeg,NULL			,DegCfg_ReqLogo			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3C0},{    0,    0}},Act_NORM,Act_1ST},
   [ActRecDegLog	] = { 553, 0,TabDeg,DegCfg_ReceiveLogo		,DegCfg_ShowConfiguration	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3C0},{    0,    0}},Act_DATA,Act_1ST},
   [ActRemDegLog	] = {1343, 0,TabDeg,DegCfg_RemoveLogo		,DegCfg_ShowConfiguration	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3C0},{    0,    0}},Act_DATA,Act_1ST},

   // Courses
   [ActSeeCrs		] = {1009, 1,TabDeg,NULL			,Crs_ShowCrssOfCurrentDeg	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C7,0x3C7},{    0,    0}},Act_NORM,Act_1ST},
   [ActEdiCrs		] = { 555, 1,TabDeg,NULL			,Crs_EditCourses		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C6,0x3C6},{    0,    0}},Act_NORM,Act_1ST},
   [ActReqCrs		] = {1053, 1,TabDeg,Crs_ReceiveReqCrs		,Crs_ContEditAfterChgCrs	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C6,0x3C6},{    0,    0}},Act_NORM,Act_1ST},
   [ActNewCrs		] = { 556, 1,TabDeg,Crs_ReceiveNewCrs		,Crs_ContEditAfterChgCrs	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3C0},{    0,    0}},Act_NORM,Act_1ST},
   [ActRemCrs		] = { 560, 1,TabDeg,Crs_RemoveCourse		,Crs_ContEditAfterChgCrs	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C6,0x3C6},{    0,    0}},Act_NORM,Act_1ST},
   [ActChgInsCrsCod	] = {1025, 1,TabDeg,Crs_ChangeInsCrsCod		,Crs_ContEditAfterChgCrs	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C6,0x3C6},{    0,    0}},Act_NORM,Act_1ST},
   [ActChgCrsYea	] = { 561, 1,TabDeg,Crs_ChangeCrsYear		,Crs_ContEditAfterChgCrs	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C6,0x3C6},{    0,    0}},Act_NORM,Act_1ST},
   [ActRenCrsSho	] = { 563, 1,TabDeg,Crs_RenameCourseShrt	,Crs_ContEditAfterChgCrs	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C6,0x3C6},{    0,    0}},Act_NORM,Act_1ST},
   [ActRenCrsFul	] = { 564, 1,TabDeg,Crs_RenameCourseFull	,Crs_ContEditAfterChgCrs	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C6,0x3C6},{    0,    0}},Act_NORM,Act_1ST},
   [ActChgCrsSta	] = {1055, 1,TabDeg,Crs_ChangeCrsStatus		,Crs_ContEditAfterChgCrs	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3C0},{    0,    0}},Act_NORM,Act_1ST},

   // TabCrs ******************************************************************
   // Course info
   [ActSeeCrsInf	] = { 847, 0,TabCrs,NULL			,Crs_ShowIntroduction		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C7,0x3F8}},Act_NORM,Act_1ST},
   [ActPrnCrsInf	] = {1028, 0,TabCrs,NULL			,CrsCfg_PrintConfiguration	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C7,0x3F8}},Act_NORM,Act_NEW},
   [ActChgCrsDegCfg	] = {1587, 0,TabCrs,CrsCfg_ChangeCrsDeg		,CrsCfg_ContEditAfterChgCrs	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x380,0x380}},Act_NORM,Act_1ST},
   [ActRenCrsShoCfg	] = {1601, 0,TabCrs,CrsCfg_RenameCourseShrt	,CrsCfg_ContEditAfterChgCrs	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3C0}},Act_NORM,Act_1ST},
   [ActRenCrsFulCfg	] = {1600, 0,TabCrs,CrsCfg_RenameCourseFull	,CrsCfg_ContEditAfterChgCrs	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3C0}},Act_NORM,Act_1ST},
   [ActChgInsCrsCodCfg	] = {1024, 0,TabCrs,CrsCfg_ChangeInsCrsCod	,CrsCfg_ContEditAfterChgCrs	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActChgCrsYeaCfg	] = {1573, 0,TabCrs,CrsCfg_ChangeCrsYear	,CrsCfg_ContEditAfterChgCrs	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActEdiCrsInf	] = { 848, 0,TabCrs,NULL			,Inf_FormsToSelSendInfo		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActChgFrcReaCrsInf	] = { 877, 0,TabCrs,NULL			,Inf_ChangeForceReadInfo	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActChgHavReaCrsInf	] = { 878, 0,TabCrs,NULL			,Inf_ChangeIHaveReadInfo	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,0x008}},Act_NORM,Act_1ST},
   [ActSelInfSrcCrsInf	] = { 849, 0,TabCrs,NULL			,Inf_SetInfoSrc			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActRcvURLCrsInf	] = { 854, 0,TabCrs,NULL			,Inf_ReceiveURLInfo		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActRcvPagCrsInf	] = { 853, 0,TabCrs,NULL			,Inf_ReceivePagInfo		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_DATA,Act_1ST},
   [ActEditorCrsInf	] = { 852, 0,TabCrs,NULL			,Inf_EditorCourseInfo		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActPlaTxtEdiCrsInf	] = { 850, 0,TabCrs,NULL			,Inf_EditPlainTxtInfo		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActRchTxtEdiCrsInf	] = {1093, 0,TabCrs,NULL			,Inf_EditRichTxtInfo		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActRcvPlaTxtCrsInf	] = { 851, 0,TabCrs,NULL			,Inf_RecAndChangePlainTxtInfo	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActRcvRchTxtCrsInf	] = {1101, 0,TabCrs,NULL			,Inf_RecAndChangeRichTxtInfo	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},

   // Program
   [ActSeePrg		] = {1821, 1,TabCrs,NULL			,Prg_ShowCourseProgram		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActEdiPrg		] = {1926, 1,TabCrs,NULL			,Prg_EditCourseProgram		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActSeePrgItm	] = {1927, 1,TabCrs,NULL			,Prg_ViewItemAfterEdit		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActFrmChgPrgItm	] = {1823, 1,TabCrs,NULL			,Prg_ReqChangeItem		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActFrmNewPrgItm	] = {1822, 1,TabCrs,NULL			,Prg_ReqCreateItem		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActChgPrgItm	] = {1826, 1,TabCrs,NULL			,Prg_ReceiveChgItem		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActNewPrgItm	] = {1825, 1,TabCrs,NULL			,Prg_ReceiveNewItem		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActReqRemPrgItm	] = {1827, 1,TabCrs,NULL			,Prg_ReqRemItem			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActRemPrgItm	] = {1828, 1,TabCrs,NULL			,Prg_RemoveItem			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActHidPrgItm	] = {1829, 1,TabCrs,NULL			,Prg_HideItem			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActUnhPrgItm	] = {1830, 1,TabCrs,NULL			,Prg_UnhideItem			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActUp_PrgItm	] = {1831, 1,TabCrs,NULL			,Prg_MoveUpItem			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActDwnPrgItm	] = {1832, 1,TabCrs,NULL			,Prg_MoveDownItem		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActLftPrgItm	] = {1834, 1,TabCrs,NULL			,Prg_MoveLeftItem		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActRgtPrgItm	] = {1833, 1,TabCrs,NULL			,Prg_MoveRightItem		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActExpSeePrgItm	] = {1944, 1,TabCrs,NULL			,Prg_ExpandItem			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActConSeePrgItm	] = {1945, 1,TabCrs,NULL			,Prg_ContractItem		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActExpEdiPrgItm	] = {1946, 1,TabCrs,NULL			,Prg_ExpandItem			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActConEdiPrgItm	] = {1947, 1,TabCrs,NULL			,Prg_ContractItem		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActSeeRscCli_InPrg	] = {1970, 1,TabCrs,NULL			,PrgRsc_ViewResourceClipboard	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActRemRscCli_InPrg	] = {1971, 1,TabCrs,NULL			,PrgRsc_RemoveResourceClipboard	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActFrmSeePrgRsc	] = {1925, 1,TabCrs,NULL			,PrgRsc_ViewResourcesAfterEdit	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActFrmEdiPrgRsc	] = {1918, 1,TabCrs,NULL			,PrgRsc_EditResources		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActNewPrgRsc	] = {1929, 1,TabCrs,NULL			,PrgRsc_CreateResource		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActRenPrgRsc	] = {1928, 1,TabCrs,NULL			,PrgRsc_RenameResource		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActReqRemPrgRsc	] = {1919, 1,TabCrs,NULL			,PrgRsc_ReqRemResource		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActRemPrgRsc	] = {1920, 1,TabCrs,NULL			,PrgRsc_RemoveResource		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActHidPrgRsc	] = {1921, 1,TabCrs,NULL			,PrgRsc_HideResource		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActUnhPrgRsc	] = {1922, 1,TabCrs,NULL			,PrgRsc_UnhideResource		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActUp_PrgRsc	] = {1923, 1,TabCrs,NULL			,PrgRsc_MoveUpResource		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActDwnPrgRsc	] = {1924, 1,TabCrs,NULL			,PrgRsc_MoveDownResource	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActFrmChgLnkPrgRsc	] = {1932, 1,TabCrs,NULL			,PrgRsc_EditProgramWithClipboard,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActChgLnkPrgRsc	] = {1933, 1,TabCrs,NULL			,PrgRsc_ChangeLink		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},

   // Teaching guide
   [ActSeeTchGui	] = { 784, 2,TabCrs,NULL			,Inf_ShowInfo			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C7,0x3F8}},Act_NORM,Act_1ST},
   [ActEdiTchGui	] = { 785, 2,TabCrs,NULL			,Inf_FormsToSelSendInfo		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActChgFrcReaTchGui	] = { 870, 2,TabCrs,NULL			,Inf_ChangeForceReadInfo	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActChgHavReaTchGui	] = { 886, 2,TabCrs,NULL			,Inf_ChangeIHaveReadInfo	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,0x008}},Act_NORM,Act_1ST},
   [ActSelInfSrcTchGui	] = { 789, 2,TabCrs,NULL			,Inf_SetInfoSrc			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActRcvURLTchGui	] = { 791, 2,TabCrs,NULL			,Inf_ReceiveURLInfo		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActRcvPagTchGui	] = { 788, 2,TabCrs,NULL			,Inf_ReceivePagInfo		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_DATA,Act_1ST},
   [ActEditorTchGui	] = { 786, 2,TabCrs,NULL			,Inf_EditorTeachingGuide	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActPlaTxtEdiTchGui	] = { 787, 2,TabCrs,NULL			,Inf_EditPlainTxtInfo		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActRchTxtEdiTchGui	] = {1094, 2,TabCrs,NULL			,Inf_EditRichTxtInfo		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActRcvPlaTxtTchGui	] = { 790, 2,TabCrs,NULL			,Inf_RecAndChangePlainTxtInfo	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActRcvRchTxtTchGui	] = {1102, 2,TabCrs,NULL			,Inf_RecAndChangeRichTxtInfo	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},

   // Syllabus
   [ActSeeSyl		] = {1242, 3,TabCrs,NULL			,Inf_ShowInfo			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C7,0x3F8}},Act_NORM,Act_1ST},
   [ActEdiSyl		] = {  44, 3,TabCrs,NULL			,Inf_FormsToSelSendInfo		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActDelItmSyl	] = { 218, 3,TabCrs,NULL			,Syl_RemoveItemSyllabus		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActUp_IteSyl	] = { 221, 3,TabCrs,NULL			,Syl_UpItemSyllabus		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActDwnIteSyl	] = { 220, 3,TabCrs,NULL			,Syl_DownItemSyllabus		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActRgtIteSyl	] = { 223, 3,TabCrs,NULL			,Syl_RightItemSyllabus		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActLftIteSyl	] = { 222, 3,TabCrs,NULL			,Syl_LeftItemSyllabus		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActInsIteSyl	] = { 217, 3,TabCrs,NULL			,Syl_InsertItemSyllabus		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActModIteSyl	] = { 211, 3,TabCrs,NULL			,Syl_ModifyItemSyllabus		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActChgFrcReaSyl	] = { 871, 3,TabCrs,NULL			,Inf_ChangeForceReadInfo	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActChgHavReaSyl	] = { 880, 3,TabCrs,NULL			,Inf_ChangeIHaveReadInfo	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,0x008}},Act_NORM,Act_1ST},
   [ActSelInfSrcSyl	] = { 378, 3,TabCrs,NULL			,Inf_SetInfoSrc			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActRcvURLSyl	] = { 403, 3,TabCrs,NULL			,Inf_ReceiveURLInfo		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActRcvPagSyl	] = { 381, 3,TabCrs,NULL			,Inf_ReceivePagInfo		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_DATA,Act_1ST},
   [ActEditorSyl	] = { 372, 3,TabCrs,NULL			,Syl_EditSyllabus		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActPlaTxtEdiSyl	] = { 379, 3,TabCrs,NULL			,Inf_EditPlainTxtInfo		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActRchTxtEdiSyl	] = {1095, 3,TabCrs,NULL			,Inf_EditRichTxtInfo		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActRcvPlaTxtSyl	] = { 394, 3,TabCrs,NULL			,Inf_RecAndChangePlainTxtInfo	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActRcvRchTxtSyl	] = {1103, 3,TabCrs,NULL			,Inf_RecAndChangeRichTxtInfo	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},

   // Bibliography
   [ActSeeBib		] = {  32, 4,TabCrs,NULL			,Inf_ShowInfo			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C7,0x3F8}},Act_NORM,Act_1ST},
   [ActEdiBib		] = {  76, 4,TabCrs,NULL			,Inf_FormsToSelSendInfo		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActChgFrcReaBib	] = { 873, 4,TabCrs,NULL			,Inf_ChangeForceReadInfo	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActChgHavReaBib	] = { 884, 4,TabCrs,NULL			,Inf_ChangeIHaveReadInfo	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,0x008}},Act_NORM,Act_1ST},
   [ActSelInfSrcBib	] = { 370, 4,TabCrs,NULL			,Inf_SetInfoSrc			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActRcvURLBib	] = { 224, 4,TabCrs,NULL			,Inf_ReceiveURLInfo		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActRcvPagBib	] = { 185, 4,TabCrs,NULL			,Inf_ReceivePagInfo		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_DATA,Act_1ST},
   [ActEditorBib	] = { 376, 4,TabCrs,NULL			,Inf_EditorBibliography		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActPlaTxtEdiBib	] = { 377, 4,TabCrs,NULL			,Inf_EditPlainTxtInfo		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActRchTxtEdiBib	] = {1097, 4,TabCrs,NULL			,Inf_EditRichTxtInfo		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActRcvPlaTxtBib	] = { 398, 4,TabCrs,NULL			,Inf_RecAndChangePlainTxtInfo	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActRcvRchTxtBib	] = {1105, 4,TabCrs,NULL			,Inf_RecAndChangeRichTxtInfo	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},

   // FAQ
   [ActSeeFAQ		] = {  54, 5,TabCrs,NULL			,Inf_ShowInfo			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActEdiFAQ		] = { 109, 5,TabCrs,NULL			,Inf_FormsToSelSendInfo		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActChgFrcReaFAQ	] = { 874, 5,TabCrs,NULL			,Inf_ChangeForceReadInfo	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActChgHavReaFAQ	] = { 879, 5,TabCrs,NULL			,Inf_ChangeIHaveReadInfo	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,0x008}},Act_NORM,Act_1ST},
   [ActSelInfSrcFAQ	] = { 380, 5,TabCrs,NULL			,Inf_SetInfoSrc			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActRcvURLFAQ	] = { 234, 5,TabCrs,NULL			,Inf_ReceiveURLInfo		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActRcvPagFAQ	] = { 219, 5,TabCrs,NULL			,Inf_ReceivePagInfo		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_DATA,Act_1ST},
   [ActEditorFAQ	] = { 404, 5,TabCrs,NULL			,Inf_EditorFAQ			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActPlaTxtEdiFAQ	] = { 405, 5,TabCrs,NULL			,Inf_EditPlainTxtInfo		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActRchTxtEdiFAQ	] = {1098, 5,TabCrs,NULL			,Inf_EditRichTxtInfo		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActRcvPlaTxtFAQ	] = { 406, 5,TabCrs,NULL			,Inf_RecAndChangePlainTxtInfo	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActRcvRchTxtFAQ	] = {1106, 5,TabCrs,NULL			,Inf_RecAndChangeRichTxtInfo	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},

   // Links
   [ActSeeCrsLnk	] = {   9, 6,TabCrs,NULL			,Inf_ShowInfo			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActEdiCrsLnk	] = {  96, 6,TabCrs,NULL			,Inf_FormsToSelSendInfo		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActChgFrcReaCrsLnk	] = { 875, 6,TabCrs,NULL			,Inf_ChangeForceReadInfo	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActChgHavReaCrsLnk	] = { 885, 6,TabCrs,NULL			,Inf_ChangeIHaveReadInfo	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,0x008}},Act_NORM,Act_1ST},
   [ActSelInfSrcCrsLnk	] = { 385, 6,TabCrs,NULL			,Inf_SetInfoSrc			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActRcvURLCrsLnk	] = { 182, 6,TabCrs,NULL			,Inf_ReceiveURLInfo		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActRcvPagCrsLnk	] = { 164, 6,TabCrs,NULL			,Inf_ReceivePagInfo		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_DATA,Act_1ST},
   [ActEditorCrsLnk	] = { 388, 6,TabCrs,NULL			,Inf_EditorLinks		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActPlaTxtEdiCrsLnk	] = { 400, 6,TabCrs,NULL			,Inf_EditPlainTxtInfo		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActRchTxtEdiCrsLnk	] = {1099, 6,TabCrs,NULL			,Inf_EditRichTxtInfo		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActRcvPlaTxtCrsLnk	] = { 401, 6,TabCrs,NULL			,Inf_RecAndChangePlainTxtInfo	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActRcvRchTxtCrsLnk	] = {1107, 6,TabCrs,NULL			,Inf_RecAndChangeRichTxtInfo	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},

   // Assessment
   [ActSeeAss		] = {  15, 7,TabCrs,NULL			,Inf_ShowInfo			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C7,0x3F8}},Act_NORM,Act_1ST},
   [ActEdiAss		] = {  69, 7,TabCrs,NULL			,Inf_FormsToSelSendInfo		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActChgFrcReaAss	] = { 883, 7,TabCrs,NULL			,Inf_ChangeForceReadInfo	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActChgHavReaAss	] = { 898, 7,TabCrs,NULL			,Inf_ChangeIHaveReadInfo	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,0x008}},Act_NORM,Act_1ST},
   [ActSelInfSrcAss	] = { 384, 7,TabCrs,NULL			,Inf_SetInfoSrc			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActRcvURLAss	] = { 235, 7,TabCrs,NULL			,Inf_ReceiveURLInfo		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActRcvPagAss	] = { 184, 7,TabCrs,NULL			,Inf_ReceivePagInfo		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_DATA,Act_1ST},
   [ActEditorAss	] = { 386, 7,TabCrs,NULL			,Inf_EditorAssessment		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActPlaTxtEdiAss	] = { 387, 7,TabCrs,NULL			,Inf_EditPlainTxtInfo		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActRchTxtEdiAss	] = {1100, 7,TabCrs,NULL			,Inf_EditRichTxtInfo		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActRcvPlaTxtAss	] = { 397, 7,TabCrs,NULL			,Inf_RecAndChangePlainTxtInfo	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActRcvRchTxtAss	] = {1108, 7,TabCrs,NULL			,Inf_RecAndChangeRichTxtInfo	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},

   // Timetable
   [ActSeeCrsTT		] = {  25, 8,TabCrs,NULL			,Tmt_ShowClassTimeTable		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C7,0x3F8}},Act_NORM,Act_1ST},
   [ActPrnCrsTT		] = { 152, 8,TabCrs,NULL			,Tmt_ShowClassTimeTable		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C7,0x3F8}},Act_NORM,Act_NEW},
   [ActEdiCrsTT		] = {  45, 8,TabCrs,NULL			,Tmt_EditCrsTimeTable		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActChgCrsTT		] = {  53, 8,TabCrs,NULL			,Tmt_EditCrsTimeTable		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActChgCrsTT1stDay	] = {1486, 8,TabCrs,Cal_Change1stDayOfWeek	,Tmt_ShowClassTimeTable		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C7,0x3F8}},Act_NORM,Act_1ST},

   // TabAss ******************************************************************
   // Assignments
   [ActSeeAllAsg	] = { 801, 0,TabAss,NULL			,Asg_SeeAssignments		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActFrmNewAsg	] = { 812, 0,TabAss,NULL			,Asg_ReqCreatOrEditAsg		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActEdiOneAsg	] = { 814, 0,TabAss,NULL			,Asg_ReqCreatOrEditAsg		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActSeeOneAsg	] = {1942, 0,TabAss,NULL			,Asg_SeeOneAssignment		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x3F8}},Act_NORM,Act_1ST},
   [ActPrnOneAsg	] = {1637, 0,TabAss,NULL			,Asg_PrintOneAssignment		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x3F8}},Act_NORM,Act_NEW},
   [ActNewAsg		] = { 803, 0,TabAss,NULL			,Asg_ReceiveAssignment		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActChgAsg		] = { 815, 0,TabAss,NULL			,Asg_ReceiveAssignment		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActReqRemAsg	] = { 813, 0,TabAss,NULL			,Asg_ReqRemAssignment		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActRemAsg		] = { 806, 0,TabAss,NULL			,Asg_RemoveAssignment		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActHidAsg		] = { 964, 0,TabAss,NULL			,Asg_HideAssignment		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActUnhAsg		] = { 965, 0,TabAss,NULL			,Asg_UnhideAssignment		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActReqLnkAsg	] = {1943, 0,TabAss,NULL			,AsgRsc_GetLinkToAssignment	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},

   // Projects
   [ActSeeAllPrj	] = {1674, 1,TabAss,NULL			,Prj_SeeAllProjects		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActCfgPrj		] = {1803, 1,TabAss,NULL			,PrjCfg_ShowFormConfig		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActChgNETCanCrePrj	] = {1804, 1,TabAss,NULL			,PrjCfg_ChangeNETCanCreate	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActChgRubPrj	] = {1977, 1,TabAss,NULL			,PrjCfg_ChangeRubricsOfType	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActReqUsrPrj	] = {1805, 1,TabAss,NULL			,Prj_ListUsrsToSelect		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActSeeTblAllPrj	] = {1696, 1,TabAss,NULL			,Prj_ShowTableSelectedPrjs	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_NEW},
   [ActReqLckAllPrj	] = {1775, 1,TabAss,NULL			,Prj_ReqLockSelectedPrjsEdition	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActReqUnlAllPrj	] = {1776, 1,TabAss,NULL			,Prj_ReqUnloSelectedPrjsEdition	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActLckAllPrj	] = {1777, 1,TabAss,NULL			,Prj_LockSelectedPrjsEdition	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActUnlAllPrj	] = {1778, 1,TabAss,NULL			,Prj_UnloSelectedPrjsEdition	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActFrmNewPrj	] = {1675, 1,TabAss,NULL			,Prj_ReqCreatePrj		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x230}},Act_NORM,Act_1ST},
   [ActEdiOnePrj	] = {1676, 1,TabAss,NULL			,Prj_ReqEditPrj			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x230}},Act_NORM,Act_1ST},
   [ActSeeOnePrj	] = {1949, 1,TabAss,NULL			,Brw_ShowFileBrowserOrWorks	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActPrnOnePrj	] = {1677, 1,TabAss,NULL			,Prj_PrintOneProject		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x3F8}},Act_NORM,Act_NEW},
   [ActNewPrj		] = {1678, 1,TabAss,NULL			,Prj_ReceiveProject		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x230}},Act_NORM,Act_1ST},
   [ActChgPrj		] = {1679, 1,TabAss,NULL			,Prj_ReceiveProject		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x230}},Act_NORM,Act_1ST},
   [ActReqRemPrj	] = {1680, 1,TabAss,NULL			,Prj_ReqRemProject		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x230}},Act_NORM,Act_1ST},
   [ActRemPrj		] = {1681, 1,TabAss,NULL			,Prj_RemoveProject		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x230}},Act_NORM,Act_1ST},
   [ActHidPrj		] = {1682, 1,TabAss,NULL			,Prj_HideProject		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x230}},Act_NORM,Act_1ST},
   [ActUnhPrj		] = {1683, 1,TabAss,NULL			,Prj_UnhideProject		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x230}},Act_NORM,Act_1ST},
   [ActLckPrj		] = {1773, 1,TabAss,NULL			,Prj_LockProjectEdition		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_AJA},
   [ActUnlPrj		] = {1774, 1,TabAss,NULL			,Prj_UnloProjectEdition		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_AJA},
   [ActChgPrjRev	] = {1950, 1,TabAss,NULL			,Prj_ChangeReviewStatus		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActReqAddStdPrj	] = {1684, 1,TabAss,NULL			,Prj_ReqAddStds			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x230}},Act_NORM,Act_1ST},
   [ActReqAddTutPrj	] = {1685, 1,TabAss,NULL			,Prj_ReqAddTuts			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x230}},Act_NORM,Act_1ST},
   [ActReqAddEvlPrj	] = {1686, 1,TabAss,NULL			,Prj_ReqAddEvls			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x230}},Act_NORM,Act_1ST},
   [ActAddStdPrj	] = {1687, 1,TabAss,NULL			,Prj_GetSelectedUsrsAndAddStds	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x230}},Act_NORM,Act_1ST},
   [ActAddTutPrj	] = {1688, 1,TabAss,NULL			,Prj_GetSelectedUsrsAndAddTuts	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x230}},Act_NORM,Act_1ST},
   [ActAddEvlPrj	] = {1689, 1,TabAss,NULL			,Prj_GetSelectedUsrsAndAddEvls	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x230}},Act_NORM,Act_1ST},
   [ActReqRemStdPrj	] = {1690, 1,TabAss,NULL			,Prj_ReqRemStd			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x230}},Act_NORM,Act_1ST},
   [ActReqRemTutPrj	] = {1691, 1,TabAss,NULL			,Prj_ReqRemTut			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x230}},Act_NORM,Act_1ST},
   [ActReqRemEvlPrj	] = {1692, 1,TabAss,NULL			,Prj_ReqRemEvl			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x230}},Act_NORM,Act_1ST},
   [ActRemStdPrj	] = {1693, 1,TabAss,NULL			,Prj_RemStd			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x230}},Act_NORM,Act_1ST},
   [ActRemTutPrj	] = {1694, 1,TabAss,NULL			,Prj_RemTut			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x230}},Act_NORM,Act_1ST},
   [ActRemEvlPrj	] = {1695, 1,TabAss,NULL			,Prj_RemEvl			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x230}},Act_NORM,Act_1ST},
   [ActChgPrjSco	] = {1974, 1,TabAss,NULL			,Prj_ChangeCriterionScore	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActReqLnkPrj	] = {1948, 1,TabAss,NULL			,PrjRsc_GetLinkToProject	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActAdmDocPrj	] = {1697, 1,TabAss,NULL			,Brw_ShowFileBrowserOrWorks	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActReqRemFilDocPrj	] = {1698, 1,TabAss,NULL			,Brw_ReqRemFile			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActRemFilDocPrj	] = {1699, 1,TabAss,NULL			,Brw_RemFile			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActRemFolDocPrj	] = {1700, 1,TabAss,NULL			,Brw_RemFolder			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActCopDocPrj	] = {1701, 1,TabAss,NULL			,Brw_Copy			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActPasDocPrj	] = {1702, 1,TabAss,NULL			,Brw_Paste			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActRemTreDocPrj	] = {1703, 1,TabAss,NULL			,Brw_RemSubtree			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActFrmCreDocPrj	] = {1704, 1,TabAss,NULL			,Brw_ShowFormFileBrowser	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActCreFolDocPrj	] = {1705, 1,TabAss,NULL			,Brw_CreateFolder		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActCreLnkDocPrj	] = {1706, 1,TabAss,NULL			,Brw_CreateLink			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActRenFolDocPrj	] = {1707, 1,TabAss,NULL			,Brw_RenFolder			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActRcvFilDocPrjDZ	] = {1708, 1,TabAss,Brw_RcvFileDZ		,NULL				,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_DATA,Act_UPL},
   [ActRcvFilDocPrjCla	] = {1709, 1,TabAss,NULL			,Brw_RcvFileClassic		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_DATA,Act_1ST},
   [ActExpDocPrj	] = {1710, 1,TabAss,Brw_ExpandFileTree		,NULL				,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_204},
   [ActConDocPrj	] = {1711, 1,TabAss,Brw_ContractFileTree	,NULL				,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_204},
   [ActZIPDocPrj	] = {1712, 1,TabAss,NULL			,ZIP_CompressFileTree		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActReqDatDocPrj	] = {1713, 1,TabAss,NULL			,Brw_ShowFileMetadata		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActChgDatDocPrj	] = {1714, 1,TabAss,NULL			,Brw_ChgFileMetadata		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActDowDocPrj	] = {1715, 1,TabAss,Brw_DownloadFile		,NULL				,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_DWN},
   [ActAdmAssPrj	] = {1716, 1,TabAss,NULL			,Brw_ShowFileBrowserOrWorks	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActReqRemFilAssPrj	] = {1717, 1,TabAss,NULL			,Brw_ReqRemFile			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActRemFilAssPrj	] = {1718, 1,TabAss,NULL			,Brw_RemFile			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActRemFolAssPrj	] = {1719, 1,TabAss,NULL			,Brw_RemFolder			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActCopAssPrj	] = {1720, 1,TabAss,NULL			,Brw_Copy			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActPasAssPrj	] = {1721, 1,TabAss,NULL			,Brw_Paste			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActRemTreAssPrj	] = {1722, 1,TabAss,NULL			,Brw_RemSubtree			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActFrmCreAssPrj	] = {1723, 1,TabAss,NULL			,Brw_ShowFormFileBrowser	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActCreFolAssPrj	] = {1724, 1,TabAss,NULL			,Brw_CreateFolder		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActCreLnkAssPrj	] = {1725, 1,TabAss,NULL			,Brw_CreateLink			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActRenFolAssPrj	] = {1726, 1,TabAss,NULL			,Brw_RenFolder			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActRcvFilAssPrjDZ	] = {1727, 1,TabAss,Brw_RcvFileDZ		,NULL				,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_DATA,Act_UPL},
   [ActRcvFilAssPrjCla	] = {1728, 1,TabAss,NULL			,Brw_RcvFileClassic		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_DATA,Act_1ST},
   [ActExpAssPrj	] = {1729, 1,TabAss,Brw_ExpandFileTree		,NULL				,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_204},
   [ActConAssPrj	] = {1730, 1,TabAss,Brw_ContractFileTree	,NULL				,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_204},
   [ActZIPAssPrj	] = {1731, 1,TabAss,NULL			,ZIP_CompressFileTree		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActReqDatAssPrj	] = {1732, 1,TabAss,NULL			,Brw_ShowFileMetadata		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActChgDatAssPrj	] = {1733, 1,TabAss,NULL			,Brw_ChgFileMetadata		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActDowAssPrj	] = {1734, 1,TabAss,Brw_DownloadFile		,NULL				,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_DWN},

   // Calls for exams
   [ActSeeAllCfe	] = {  85, 2,TabAss,NULL			,Cfe_ListCallsForExamsSee	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C7,0x3F8}},Act_NORM,Act_1ST},
   [ActSeeOneCfe	] = {1572, 2,TabAss,NULL			,Cfe_ListCallsForExamsCod	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C7,0x3F8}},Act_NORM,Act_1ST},
   [ActSeeDatCfe	] = {1571, 2,TabAss,NULL			,Cfe_ListCallsForExamsDay	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C7,0x3F8}},Act_NORM,Act_1ST},
   [ActEdiCfe		] = {  91, 2,TabAss,NULL			,Cfe_PutFrmEditACallForExam	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActRcvCfe		] = { 110, 2,TabAss,Cfe_ReceiveCallForExam1	,Cfe_ReceiveCallForExam2	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActPrnCfe		] = { 179, 2,TabAss,NULL			,Cfe_PrintCallForExam		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C7,0x3F8}},Act_NORM,Act_NEW},
   [ActReqRemCfe	] = {1619, 2,TabAss,NULL			,Cfe_ReqRemCallForExam		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActRemCfe		] = { 187, 2,TabAss,Cfe_RemoveCallForExam1	,Cfe_RemoveCallForExam2		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActHidCfe		] = {1620, 2,TabAss,Cfe_HideCallForExam		,Cfe_ListCallsForExamsEdit	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActUnhCfe		] = {1621, 2,TabAss,Cfe_UnhideCallForExam	,Cfe_ListCallsForExamsEdit	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActReqLnkCfe	] = {1934, 2,TabAss,NULL			,CfeRsc_GetLinkToCallForExam	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},

   // Questions
   [ActEdiTstQst	] = { 104, 3,TabAss,Dat_SetDatesToPastNow	,Qst_ReqEditQsts		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActEdiOneTstQst	] = { 105, 3,TabAss,NULL			,Qst_ShowFormEditOneQst		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActReqImpTstQst	] = {1007, 3,TabAss,NULL			,QstImp_ShowFormImpQstsFromXML	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActImpTstQst	] = {1008, 3,TabAss,NULL			,QstImp_ImpQstsFromXML		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_DATA,Act_1ST},
   [ActLstTstQst	] = { 132, 3,TabAss,NULL			,Qst_ListQuestionsToEdit	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActNewTstQst	] = { 126, 3,TabAss,NULL			,Qst_ReceiveQst			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_DATA,Act_1ST},
   [ActChgTstQst	] = {1975, 3,TabAss,NULL			,Qst_ReceiveQst			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_DATA,Act_1ST},
   [ActReqRemSevTstQst	] = {1835, 3,TabAss,NULL			,Qst_ReqRemSelectedQsts		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActRemSevTstQst	] = {1836, 3,TabAss,NULL			,Qst_RemoveSelectedQsts		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActReqRemOneTstQst	] = {1523, 3,TabAss,NULL			,Qst_ReqRemOneQst		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActRemOneTstQst	] = { 133, 3,TabAss,NULL			,Qst_RemoveOneQst		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActChgShfTstQst	] = { 455, 3,TabAss,NULL			,Qst_ChangeShuffleQst		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActEdiTag		] = {1907, 3,TabAss,NULL			,Tag_ShowFormEditTags		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActEnaTag		] = { 453, 3,TabAss,NULL			,Tag_EnableTag			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActDisTag		] = { 452, 3,TabAss,NULL			,Tag_DisableTag			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActRenTag		] = { 143, 3,TabAss,NULL			,Tag_RenameTag			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActReqLnkTag	] = {1992, 4,TabAss,NULL			,TagRsc_GetLinkToTag		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},

   // Test
   [ActReqTstAnyTag	] = { 103, 4,TabAss,NULL			,Tst_ReqTest			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActReqTstOneTag	] = {1991, 4,TabAss,NULL			,Tst_ReqTest			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActSeeTst		] = {  29, 4,TabAss,NULL			,Tst_ShowNewTest		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActReqAssTst	] = {1837, 4,TabAss,NULL			,Tst_ReceiveTestDraft		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActAssTst		] = {  98, 4,TabAss,NULL			,Tst_AssessTest			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActCfgTst		] = { 451, 4,TabAss,NULL			,TstCfg_CheckAndShowFormConfig	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActRcvCfgTst	] = { 454, 4,TabAss,NULL			,TstCfg_ReceiveConfigTst	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActReqSeeMyTstRes	] = {1083, 4,TabAss,Dat_SetDatesToPastNow	,TstPrn_SelDatesToSeeMyPrints	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x208}},Act_NORM,Act_1ST},
   [ActSeeMyTstResCrs	] = {1084, 4,TabAss,NULL			,TstPrn_ShowMyPrints		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x208}},Act_NORM,Act_1ST},
   [ActSeeOneTstResMe	] = {1085, 4,TabAss,NULL			,TstPrn_ShowOnePrint		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x208}},Act_NORM,Act_1ST},
   [ActReqSeeUsrTstRes	] = {1080, 4,TabAss,Dat_SetDatesToPastNow	,TstPrn_SelUsrsToViewUsrsPrints	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x230}},Act_NORM,Act_1ST},
   [ActSeeUsrTstResCrs	] = {1081, 4,TabAss,NULL			,TstPrn_GetUsrsAndShowPrints	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x230}},Act_NORM,Act_1ST},
   [ActSeeOneTstResOth	] = {1082, 4,TabAss,NULL			,TstPrn_ShowOnePrint		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x230}},Act_NORM,Act_1ST},
   [ActReqLnkTst	] = {1990, 4,TabAss,NULL			,TstRsc_GetLinkToTest		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},

   // Exams
   [ActSeeAllExa	] = {1848, 5,TabAss,NULL			,Exa_SeeAllExams		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActSeeOneExa	] = {1849, 5,TabAss,NULL			,Exa_SeeOneExam			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActFrmNewExa	] = {1877, 5,TabAss,NULL			,Exa_ReqCreatOrEditExam		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActEdiOneExa	] = {1878, 5,TabAss,NULL			,Exa_ReqCreatOrEditExam		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActNewExa		] = {1879, 5,TabAss,NULL			,Exa_ReceiveExam		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActChgExa		] = {1880, 5,TabAss,NULL			,Exa_ReceiveExam		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActReqRemExa	] = {1881, 5,TabAss,NULL			,Exa_AskRemExam			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActRemExa		] = {1882, 5,TabAss,NULL			,Exa_RemoveExam			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActHidExa		] = {1883, 5,TabAss,NULL			,Exa_HideExam			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActUnhExa		] = {1884, 5,TabAss,NULL			,Exa_UnhideExam			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActReqLnkExa	] = {1936, 5,TabAss,NULL			,ExaRsc_GetLinkToExam		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActNewExaSet	] = {1898, 5,TabAss,NULL			,ExaSet_ReceiveSet		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActReqRemExaSet	] = {1893, 5,TabAss,NULL			,ExaSet_ReqRemSet		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActRemExaSet	] = {1894, 5,TabAss,NULL			,ExaSet_RemoveSet		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActUp_ExaSet	] = {1895, 5,TabAss,NULL			,ExaSet_MoveUpSet		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActDwnExaSet	] = {1896, 5,TabAss,NULL			,ExaSet_MoveDownSet		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActChgTitExaSet	] = {1897, 5,TabAss,NULL			,ExaSet_ChangeSetTitle		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActChgNumQstExaSet	] = {1899, 5,TabAss,NULL			,ExaSet_ChangeNumQstsToExam	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActReqAddQstExaSet	] = {1885, 5,TabAss,Dat_SetDatesToPastNow	,ExaSet_ReqSelectQstsToAddToSet	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActLstTstQstForSet	] = {1886, 5,TabAss,NULL			,ExaSet_ListQstsToAddToSet	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActAddQstToExa	] = {1887, 5,TabAss,NULL			,ExaSet_AddQstsToSet		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActReqRemSetQst	] = {1888, 5,TabAss,NULL			,ExaSet_ReqRemQstFromSet	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActRemExaQst	] = {1889, 5,TabAss,NULL			,ExaSet_RemoveQstFromSet	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActValSetQst	] = {1909, 5,TabAss,NULL			,ExaSet_ValidateQst		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActInvSetQst	] = {1910, 5,TabAss,NULL			,ExaSet_InvalidateQst		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActReqNewExaSes	] = {1852, 5,TabAss,NULL			,ExaSes_ReqCreatOrEditSes	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x230}},Act_NORM,Act_1ST},
   [ActReqChgExaSes	] = {1902, 5,TabAss,NULL			,ExaSes_ReqCreatOrEditSes	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActNewExaSes	] = {1853, 5,TabAss,NULL			,ExaSes_ReceiveSession		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x230}},Act_NORM,Act_1ST},
   [ActChgExaSes	] = {1903, 5,TabAss,NULL			,ExaSes_ReceiveSession		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActReqRemExaSes	] = {1850, 5,TabAss,NULL			,ExaSes_ReqRemSession		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x230}},Act_NORM,Act_1ST},
   [ActRemExaSes	] = {1851, 5,TabAss,NULL			,ExaSes_RemoveSession		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x230}},Act_NORM,Act_1ST},
   [ActHidExaSes	] = {1900, 5,TabAss,NULL			,ExaSes_HideSession		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActUnhExaSes	] = {1901, 5,TabAss,NULL			,ExaSes_UnhideSession		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActSeeExaPrn	] = {1904, 5,TabAss,NULL			,ExaPrn_ShowExamPrint		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActAnsExaPrn	] = {1906, 5,TabAss,NULL			,ExaPrn_ReceivePrintAnswer	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_AJA},
   [ActEndExaPrn	] = {1908, 5,TabAss,NULL			,ExaRes_ShowExaResultAfterFinish,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_NEW},
   [ActSeeMyExaResCrs	] = {1867, 5,TabAss,NULL			,ExaRes_ShowMyResultsInCrs	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x208}},Act_NORM,Act_1ST},
   [ActSeeMyExaResExa	] = {1868, 5,TabAss,NULL			,ExaRes_ShowMyResultsInExa	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x208}},Act_NORM,Act_1ST},
   [ActSeeMyExaResSes	] = {1869, 5,TabAss,NULL			,ExaRes_ShowMyResultsInSes	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x208}},Act_NORM,Act_1ST},
   [ActSeeOneExaResMe	] = {1870, 5,TabAss,NULL			,ExaRes_ShowOneExaResult	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x208}},Act_NORM,Act_1ST},
   [ActReqSeeUsrExaRes	] = {1871, 5,TabAss,NULL			,ExaRes_SelUsrsToViewResults	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x230}},Act_NORM,Act_1ST},
   [ActSeeUsrExaResCrs	] = {1872, 5,TabAss,NULL			,ExaRes_ShowAllResultsInCrs	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x230}},Act_NORM,Act_1ST},
   [ActSeeUsrExaResExa	] = {1873, 5,TabAss,NULL			,ExaRes_ShowAllResultsInExa	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x230}},Act_NORM,Act_1ST},
   [ActSeeUsrExaResSes	] = {1874, 5,TabAss,NULL			,ExaRes_ShowAllResultsInSes	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x230}},Act_NORM,Act_1ST},
   [ActSeeOneExaResOth	] = {1875, 5,TabAss,NULL			,ExaRes_ShowOneExaResult	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x230}},Act_NORM,Act_1ST},
   [ActChgVisExaRes	] = {1876, 5,TabAss,NULL			,ExaSes_ToggleVisResultsSesUsr	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x230}},Act_NORM,Act_1ST},

   // Games
   [ActSeeAllGam	] = {1649, 6,TabAss,NULL			,Gam_SeeAllGames		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActSeeOneGam	] = {1650, 6,TabAss,NULL			,Gam_SeeOneGame			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActReqNewMch	] = {1670, 6,TabAss,NULL			,Mch_ReqCreatOrEditMatch	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x230}},Act_NORM,Act_1ST},
   [ActReqChgMch	] = {1913, 6,TabAss,NULL			,Mch_ReqCreatOrEditMatch	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x230}},Act_NORM,Act_1ST},
   [ActNewMch		] = {1671, 6,TabAss,Mch_CreateNewMatch		,Mch_ResumeMatch		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x230}},Act_NORM,Act_NEW},
   [ActChgMch		] = {1914, 6,TabAss,NULL			,Mch_ChangeMatch		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x230}},Act_NORM,Act_1ST},
   [ActReqRemMch	] = {1783, 6,TabAss,NULL			,Mch_ReqRemMatch		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x230}},Act_NORM,Act_1ST},
   [ActRemMch		] = {1784, 6,TabAss,NULL			,Mch_RemoveMatch		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x230}},Act_NORM,Act_1ST},
   [ActResMch		] = {1785, 6,TabAss,Mch_GetMatchBeingPlayed	,Mch_ResumeMatch		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x230}},Act_NORM,Act_NEW},
   [ActBckMch		] = {1790, 6,TabAss,Mch_GetMatchBeingPlayed	,Mch_BackMatch			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x230}},Act_NORM,Act_2ND},
   [ActPlyPauMch	] = {1789, 6,TabAss,Mch_GetMatchBeingPlayed	,Mch_PlayPauseMatch		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x230}},Act_NORM,Act_2ND},
   [ActFwdMch		] = {1672, 6,TabAss,Mch_GetMatchBeingPlayed	,Mch_ForwardMatch		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x230}},Act_NORM,Act_2ND},
   [ActChgNumColMch	] = {1802, 6,TabAss,Mch_GetMatchBeingPlayed	,Mch_ChangeNumColsMch		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x230}},Act_NORM,Act_2ND},
   [ActChgVisResMchQst	] = {1794, 6,TabAss,Mch_GetMatchBeingPlayed	,Mch_ToggleVisResultsMchQst	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x230}},Act_NORM,Act_2ND},
   [ActMchCntDwn	] = {1814, 6,TabAss,Mch_GetMatchBeingPlayed	,Mch_StartCountdown		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x230}},Act_NORM,Act_AJA},
   [ActRefMchTch	] = {1788, 6,TabAss,Mch_GetMatchBeingPlayed	,Mch_RefreshMatchTch		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x230}},Act_NORM,Act_REF},
   [ActJoiMch		] = {1780, 6,TabAss,Mch_GetMatchBeingPlayed	,Mch_JoinMatchAsStd		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,0x008}},Act_NORM,Act_NEW},
   [ActSeeMchAnsQstStd	] = {1808, 6,TabAss,Mch_GetMatchBeingPlayed	,Mch_JoinMatchAsStd		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,0x008}},Act_NORM,Act_2ND},
   [ActRemMchAnsQstStd	] = {1809, 6,TabAss,Mch_GetMatchBeingPlayed	,Mch_RemMyQstAnsAndShowMchStatus,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,0x008}},Act_NORM,Act_2ND},
   [ActAnsMchQstStd	] = {1651, 6,TabAss,Mch_GetMatchBeingPlayed	,Mch_ReceiveQuestionAnswer	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,0x008}},Act_NORM,Act_2ND},
   [ActRefMchStd	] = {1782, 6,TabAss,Mch_GetMatchBeingPlayed	,Mch_RefreshMatchStd		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,0x008}},Act_NORM,Act_REF},
   [ActSeeMyMchResCrs	] = {1796, 6,TabAss,NULL			,MchRes_ShowMyMchResultsInCrs	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x208}},Act_NORM,Act_1ST},
   [ActSeeMyMchResGam	] = {1810, 6,TabAss,NULL			,MchRes_ShowMyMchResultsInGam	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x208}},Act_NORM,Act_1ST},
   [ActSeeMyMchResMch	] = {1812, 6,TabAss,NULL			,MchRes_ShowMyMchResultsInMch	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x208}},Act_NORM,Act_1ST},
   [ActSeeOneMchResMe	] = {1797, 6,TabAss,NULL			,MchRes_ShowOneMchResult	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x208}},Act_NORM,Act_1ST},
   [ActReqSeeUsrMchRes	] = {1798, 6,TabAss,NULL			,MchRes_SelUsrsToViewMchResults	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x230}},Act_NORM,Act_1ST},
   [ActSeeUsrMchResCrs	] = {1799, 6,TabAss,NULL			,MchRes_ShowAllMchResultsInCrs	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x230}},Act_NORM,Act_1ST},
   [ActSeeUsrMchResGam	] = {1811, 6,TabAss,NULL			,MchRes_ShowAllMchResultsInGam	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x230}},Act_NORM,Act_1ST},
   [ActSeeUsrMchResMch	] = {1813, 6,TabAss,NULL			,MchRes_ShowAllMchResultsInMch	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x230}},Act_NORM,Act_1ST},
   [ActSeeOneMchResOth	] = {1800, 6,TabAss,NULL			,MchRes_ShowOneMchResult	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x230}},Act_NORM,Act_1ST},
   [ActChgVisResMchUsr	] = {1801, 6,TabAss,NULL			,Mch_ToggleVisResultsMchUsr	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x230}},Act_NORM,Act_1ST},
   [ActLstOneGam	] = {1912, 6,TabAss,NULL			,Gam_ListGame			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActFrmNewGam	] = {1652, 6,TabAss,NULL			,Gam_ReqCreatOrEditGame		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActEdiOneGam	] = {1653, 6,TabAss,NULL			,Gam_ReqCreatOrEditGame		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActNewGam		] = {1654, 6,TabAss,NULL			,Gam_ReceiveGame		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActChgGam		] = {1655, 6,TabAss,NULL			,Gam_ReceiveGame		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActReqRemGam	] = {1656, 6,TabAss,NULL			,Gam_AskRemGame			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActRemGam		] = {1657, 6,TabAss,NULL			,Gam_RemoveGame			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActHidGam		] = {1660, 6,TabAss,NULL			,Gam_HideGame			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActUnhGam		] = {1661, 6,TabAss,NULL			,Gam_UnhideGame			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActAddOneGamQst	] = {1662, 6,TabAss,Dat_SetDatesToPastNow	,Gam_ReqSelectQstsToAddToGame	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActGamLstTstQst	] = {1666, 6,TabAss,NULL			,Gam_ListQstsToAddToGame	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActAddTstQstToGam	] = {1667, 6,TabAss,NULL			,Gam_AddQstsToGame		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActReqRemGamQst	] = {1664, 6,TabAss,NULL			,Gam_ReqRemQstFromGame		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActRemGamQst	] = {1665, 6,TabAss,NULL			,Gam_RemoveQstFromGame		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActUp_GamQst	] = {1668, 6,TabAss,NULL			,Gam_MoveUpQst			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActDwnGamQst	] = {1669, 6,TabAss,NULL			,Gam_MoveDownQst		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActReqLnkGam	] = {1935, 6,TabAss,NULL			,GamRsc_GetLinkToGame		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},

   // Rubrics
   [ActSeeAllRub	] = {1951, 7,TabAss,NULL			,Rub_SeeAllRubrics		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x200}},Act_NORM,Act_1ST},
   [ActSeeOneRub	] = {1952, 7,TabAss,NULL			,Rub_SeeOneRubric		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActFrmNewRub	] = {1953, 7,TabAss,NULL			,Rub_ReqCreatOrEditRubric	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActEdiOneRub	] = {1954, 7,TabAss,NULL			,Rub_ReqCreatOrEditRubric	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActNewRub		] = {1955, 7,TabAss,NULL			,Rub_ReceiveRubric		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActChgRub		] = {1956, 7,TabAss,NULL			,Rub_ReceiveRubric		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActReqRemRub	] = {1957, 7,TabAss,NULL			,Rub_AskRemRubric		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActRemRub		] = {1958, 7,TabAss,NULL			,Rub_RemoveRubric		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActReqLnkRub	] = {1969, 7,TabAss,NULL			,RubRsc_GetLinkToRubric		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActSeeRscCli_InRub	] = {1972, 7,TabAss,NULL			,RubRsc_ViewResourceClipboard	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActRemRscCli_InRub	] = {1973, 7,TabAss,NULL			,RubRsc_RemoveResourceClipboard	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActNewRubCri	] = {1959, 7,TabAss,NULL			,RubCri_ReceiveCriterion	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActReqRemRubCri	] = {1960, 7,TabAss,NULL			,RubCri_ReqRemCriterion		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActRemRubCri	] = {1961, 7,TabAss,NULL			,RubCri_RemoveCriterion		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActUp_RubCri	] = {1962, 7,TabAss,NULL			,RubCri_MoveUpCriterion		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActDwnRubCri	] = {1963, 7,TabAss,NULL			,RubCri_MoveDownCriterion	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActChgTitRubCri	] = {1964, 7,TabAss,NULL			,RubCri_ChangeTitle		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActChgLnkRubCri	] = {1968, 7,TabAss,NULL			,RubCri_ChangeLink		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActChgMinRubCri	] = {1965, 7,TabAss,NULL			,RubCri_ChangeMinValue		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActChgMaxRubCri	] = {1966, 7,TabAss,NULL			,RubCri_ChangeMaxValue		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActChgWeiRubCri	] = {1967, 7,TabAss,NULL			,RubCri_ChangeWeight		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},

   // TabFil ******************************************************************
   // Documents of institution
   [ActSeeAdmDocIns	] = {1249, 0,TabFil,NULL			,Brw_ShowFileBrowserOrWorks	,{{    0,    0},{    0,    0},{    0,    0},{0x3C7,0x3C7},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActChgToSeeDocIns	] = {1308, 0,TabFil,NULL			,Brw_ShowFileBrowserOrWorks	,{{    0,    0},{    0,    0},{    0,    0},{0x300,0x300},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActSeeDocIns	] = {1309, 0,TabFil,NULL			,Brw_ShowFileBrowserOrWorks	,{{    0,    0},{    0,    0},{    0,    0},{0x3C7,0x3C7},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActExpSeeDocIns	] = {1310, 0,TabFil,Brw_ExpandFileTree		,NULL				,{{    0,    0},{    0,    0},{    0,    0},{0x3C7,0x3C7},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_204},
   [ActConSeeDocIns	] = {1311, 0,TabFil,Brw_ContractFileTree	,NULL				,{{    0,    0},{    0,    0},{    0,    0},{0x3C7,0x3C7},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_204},
   [ActZIPSeeDocIns	] = {1312, 0,TabFil,NULL			,ZIP_CompressFileTree		,{{    0,    0},{    0,    0},{    0,    0},{0x3C7,0x3C7},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActReqDatSeeDocIns	] = {1313, 0,TabFil,NULL			,Brw_ShowFileMetadata		,{{    0,    0},{    0,    0},{    0,    0},{0x3C7,0x3C7},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActDowSeeDocIns	] = {1314, 0,TabFil,Brw_DownloadFile		,NULL				,{{    0,    0},{    0,    0},{    0,    0},{0x3C7,0x3C7},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_DWN},
   [ActChgToAdmDocIns	] = {1315, 0,TabFil,NULL			,Brw_ShowFileBrowserOrWorks	,{{    0,    0},{    0,    0},{    0,    0},{0x300,0x300},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActAdmDocIns	] = {1316, 0,TabFil,NULL			,Brw_ShowFileBrowserOrWorks	,{{    0,    0},{    0,    0},{    0,    0},{0x300,0x300},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActReqRemFilDocIns	] = {1317, 0,TabFil,NULL			,Brw_ReqRemFile			,{{    0,    0},{    0,    0},{    0,    0},{0x300,0x300},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActRemFilDocIns	] = {1318, 0,TabFil,NULL			,Brw_RemFile			,{{    0,    0},{    0,    0},{    0,    0},{0x300,0x300},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActRemFolDocIns	] = {1319, 0,TabFil,NULL			,Brw_RemFolder			,{{    0,    0},{    0,    0},{    0,    0},{0x300,0x300},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActCopDocIns	] = {1320, 0,TabFil,NULL			,Brw_Copy			,{{    0,    0},{    0,    0},{    0,    0},{0x300,0x300},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActPasDocIns	] = {1321, 0,TabFil,NULL			,Brw_Paste			,{{    0,    0},{    0,    0},{    0,    0},{0x300,0x300},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActRemTreDocIns	] = {1322, 0,TabFil,NULL			,Brw_RemSubtree			,{{    0,    0},{    0,    0},{    0,    0},{0x300,0x300},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActFrmCreDocIns	] = {1323, 0,TabFil,NULL			,Brw_ShowFormFileBrowser	,{{    0,    0},{    0,    0},{    0,    0},{0x300,0x300},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActCreFolDocIns	] = {1324, 0,TabFil,NULL			,Brw_CreateFolder		,{{    0,    0},{    0,    0},{    0,    0},{0x300,0x300},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActCreLnkDocIns	] = {1325, 0,TabFil,NULL			,Brw_CreateLink			,{{    0,    0},{    0,    0},{    0,    0},{0x300,0x300},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActRenFolDocIns	] = {1326, 0,TabFil,NULL			,Brw_RenFolder			,{{    0,    0},{    0,    0},{    0,    0},{0x300,0x300},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActRcvFilDocInsDZ	] = {1327, 0,TabFil,Brw_RcvFileDZ		,NULL				,{{    0,    0},{    0,    0},{    0,    0},{0x300,0x300},{    0,    0},{    0,    0},{    0,    0}},Act_DATA,Act_UPL},
   [ActRcvFilDocInsCla	] = {1328, 0,TabFil,NULL			,Brw_RcvFileClassic		,{{    0,    0},{    0,    0},{    0,    0},{0x300,0x300},{    0,    0},{    0,    0},{    0,    0}},Act_DATA,Act_1ST},
   [ActExpAdmDocIns	] = {1329, 0,TabFil,Brw_ExpandFileTree		,NULL				,{{    0,    0},{    0,    0},{    0,    0},{0x300,0x300},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_204},
   [ActConAdmDocIns	] = {1330, 0,TabFil,Brw_ContractFileTree	,NULL				,{{    0,    0},{    0,    0},{    0,    0},{0x300,0x300},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_204},
   [ActZIPAdmDocIns	] = {1331, 0,TabFil,NULL			,ZIP_CompressFileTree		,{{    0,    0},{    0,    0},{    0,    0},{0x300,0x300},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActUnhDocIns	] = {1332, 0,TabFil,NULL			,Brw_SetDocumentAsVisible	,{{    0,    0},{    0,    0},{    0,    0},{0x300,0x300},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActHidDocIns	] = {1333, 0,TabFil,NULL			,Brw_SetDocumentAsHidden	,{{    0,    0},{    0,    0},{    0,    0},{0x300,0x300},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActReqDatAdmDocIns	] = {1334, 0,TabFil,NULL			,Brw_ShowFileMetadata		,{{    0,    0},{    0,    0},{    0,    0},{0x300,0x300},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActChgDatAdmDocIns	] = {1335, 0,TabFil,NULL			,Brw_ChgFileMetadata		,{{    0,    0},{    0,    0},{    0,    0},{0x300,0x300},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActDowAdmDocIns   	] = {1336, 0,TabFil,Brw_DownloadFile		,NULL				,{{    0,    0},{    0,    0},{    0,    0},{0x300,0x300},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_DWN},

   // Shared files of institution
   [ActAdmShaIns	] = {1382, 1,TabFil,NULL			,Brw_ShowFileBrowserOrWorks	,{{    0,    0},{    0,    0},{    0,    0},{0x3C7,0x3C7},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActReqRemFilShaIns	] = {1383, 1,TabFil,NULL			,Brw_ReqRemFile			,{{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3C0},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActRemFilShaIns	] = {1384, 1,TabFil,NULL			,Brw_RemFile			,{{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3C0},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActRemFolShaIns	] = {1385, 1,TabFil,NULL			,Brw_RemFolder			,{{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3C0},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActCopShaIns	] = {1386, 1,TabFil,NULL			,Brw_Copy			,{{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3C0},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActPasShaIns	] = {1387, 1,TabFil,NULL			,Brw_Paste			,{{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3C0},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActRemTreShaIns	] = {1388, 1,TabFil,NULL			,Brw_RemSubtree			,{{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3C0},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActFrmCreShaIns	] = {1389, 1,TabFil,NULL			,Brw_ShowFormFileBrowser	,{{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3C0},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActCreFolShaIns	] = {1390, 1,TabFil,NULL			,Brw_CreateFolder		,{{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3C0},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActCreLnkShaIns	] = {1391, 1,TabFil,NULL			,Brw_CreateLink			,{{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3C0},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActRenFolShaIns	] = {1392, 1,TabFil,NULL			,Brw_RenFolder			,{{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3C0},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActRcvFilShaInsDZ	] = {1393, 1,TabFil,Brw_RcvFileDZ		,NULL				,{{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3C0},{    0,    0},{    0,    0},{    0,    0}},Act_DATA,Act_UPL},
   [ActRcvFilShaInsCla	] = {1394, 1,TabFil,NULL			,Brw_RcvFileClassic		,{{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3C0},{    0,    0},{    0,    0},{    0,    0}},Act_DATA,Act_1ST},
   [ActExpShaIns	] = {1395, 1,TabFil,Brw_ExpandFileTree		,NULL				,{{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3C0},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_204},
   [ActConShaIns	] = {1396, 1,TabFil,Brw_ContractFileTree	,NULL				,{{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3C0},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_204},
   [ActZIPShaIns	] = {1397, 1,TabFil,NULL			,ZIP_CompressFileTree		,{{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3C0},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActReqDatShaIns	] = {1398, 1,TabFil,NULL			,Brw_ShowFileMetadata		,{{    0,    0},{    0,    0},{    0,    0},{0x3C7,0x3C7},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActChgDatShaIns	] = {1399, 1,TabFil,NULL			,Brw_ChgFileMetadata		,{{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3C0},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActDowShaIns	] = {1400, 1,TabFil,Brw_DownloadFile		,NULL				,{{    0,    0},{    0,    0},{    0,    0},{0x3C7,0x3C7},{    0,    0},{    0,    0},{    0,    0}},Act_NORM,Act_DWN},

   // Documents of center
   [ActSeeAdmDocCtr	] = {1248, 2,TabFil,NULL			,Brw_ShowFileBrowserOrWorks	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C7,0x3C7},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActChgToSeeDocCtr	] = {1279, 2,TabFil,NULL			,Brw_ShowFileBrowserOrWorks	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x380,0x380},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActSeeDocCtr	] = {1280, 2,TabFil,NULL			,Brw_ShowFileBrowserOrWorks	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C7,0x3C7},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActExpSeeDocCtr	] = {1281, 2,TabFil,Brw_ExpandFileTree		,NULL				,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C7,0x3C7},{    0,    0},{    0,    0}},Act_NORM,Act_204},
   [ActConSeeDocCtr	] = {1282, 2,TabFil,Brw_ContractFileTree	,NULL				,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C7,0x3C7},{    0,    0},{    0,    0}},Act_NORM,Act_204},
   [ActZIPSeeDocCtr	] = {1283, 2,TabFil,NULL			,ZIP_CompressFileTree		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C7,0x3C7},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActReqDatSeeDocCtr	] = {1284, 2,TabFil,NULL			,Brw_ShowFileMetadata		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C7,0x3C7},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActDowSeeDocCtr   	] = {1285, 2,TabFil,Brw_DownloadFile		,NULL				,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C7,0x3C7},{    0,    0},{    0,    0}},Act_NORM,Act_DWN},
   [ActChgToAdmDocCtr	] = {1286, 2,TabFil,NULL			,Brw_ShowFileBrowserOrWorks	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x380,0x380},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActAdmDocCtr	] = {1287, 2,TabFil,NULL			,Brw_ShowFileBrowserOrWorks	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x380,0x380},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActReqRemFilDocCtr	] = {1288, 2,TabFil,NULL			,Brw_ReqRemFile			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x380,0x380},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActRemFilDocCtr	] = {1289, 2,TabFil,NULL			,Brw_RemFile			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x380,0x380},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActRemFolDocCtr	] = {1290, 2,TabFil,NULL			,Brw_RemFolder			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x380,0x380},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActCopDocCtr	] = {1291, 2,TabFil,NULL			,Brw_Copy			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x380,0x380},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActPasDocCtr	] = {1292, 2,TabFil,NULL			,Brw_Paste			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x380,0x380},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActRemTreDocCtr	] = {1293, 2,TabFil,NULL			,Brw_RemSubtree			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x380,0x380},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActFrmCreDocCtr	] = {1294, 2,TabFil,NULL			,Brw_ShowFormFileBrowser	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x380,0x380},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActCreFolDocCtr	] = {1295, 2,TabFil,NULL			,Brw_CreateFolder		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x380,0x380},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActCreLnkDocCtr	] = {1296, 2,TabFil,NULL			,Brw_CreateLink			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x380,0x380},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActRenFolDocCtr	] = {1297, 2,TabFil,NULL			,Brw_RenFolder			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x380,0x380},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActRcvFilDocCtrDZ	] = {1298, 2,TabFil,Brw_RcvFileDZ		,NULL				,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x380,0x380},{    0,    0},{    0,    0}},Act_DATA,Act_UPL},
   [ActRcvFilDocCtrCla	] = {1299, 2,TabFil,NULL			,Brw_RcvFileClassic		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x380,0x380},{    0,    0},{    0,    0}},Act_DATA,Act_1ST},
   [ActExpAdmDocCtr	] = {1300, 2,TabFil,Brw_ExpandFileTree		,NULL				,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x380,0x380},{    0,    0},{    0,    0}},Act_NORM,Act_204},
   [ActConAdmDocCtr	] = {1301, 2,TabFil,Brw_ContractFileTree	,NULL				,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x380,0x380},{    0,    0},{    0,    0}},Act_NORM,Act_204},
   [ActZIPAdmDocCtr	] = {1302, 2,TabFil,NULL			,ZIP_CompressFileTree		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x380,0x380},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActUnhDocCtr	] = {1303, 2,TabFil,NULL			,Brw_SetDocumentAsVisible	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x380,0x380},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActHidDocCtr	] = {1304, 2,TabFil,NULL			,Brw_SetDocumentAsHidden	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x380,0x380},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActReqDatAdmDocCtr	] = {1305, 2,TabFil,NULL			,Brw_ShowFileMetadata		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x380,0x380},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActChgDatAdmDocCtr	] = {1306, 2,TabFil,NULL			,Brw_ChgFileMetadata		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x380,0x380},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActDowAdmDocCtr   	] = {1307, 2,TabFil,Brw_DownloadFile		,NULL				,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x380,0x380},{    0,    0},{    0,    0}},Act_NORM,Act_DWN},

   // Shared files of center
   [ActAdmShaCtr	] = {1363, 3,TabFil,NULL			,Brw_ShowFileBrowserOrWorks	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C7,0x3C7},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActReqRemFilShaCtr	] = {1364, 3,TabFil,NULL			,Brw_ReqRemFile			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3C0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActRemFilShaCtr	] = {1365, 3,TabFil,NULL			,Brw_RemFile			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3C0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActRemFolShaCtr	] = {1366, 3,TabFil,NULL			,Brw_RemFolder			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3C0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActCopShaCtr	] = {1367, 3,TabFil,NULL			,Brw_Copy			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3C0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActPasShaCtr	] = {1368, 3,TabFil,NULL			,Brw_Paste			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3C0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActRemTreShaCtr	] = {1369, 3,TabFil,NULL			,Brw_RemSubtree			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3C0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActFrmCreShaCtr	] = {1370, 3,TabFil,NULL			,Brw_ShowFormFileBrowser	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3C0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActCreFolShaCtr	] = {1371, 3,TabFil,NULL			,Brw_CreateFolder		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3C0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActCreLnkShaCtr	] = {1372, 3,TabFil,NULL			,Brw_CreateLink			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3C0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActRenFolShaCtr	] = {1373, 3,TabFil,NULL			,Brw_RenFolder			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3C0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActRcvFilShaCtrDZ	] = {1374, 3,TabFil,Brw_RcvFileDZ		,NULL				,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3C0},{    0,    0},{    0,    0}},Act_DATA,Act_UPL},
   [ActRcvFilShaCtrCla	] = {1375, 3,TabFil,NULL			,Brw_RcvFileClassic		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3C0},{    0,    0},{    0,    0}},Act_DATA,Act_1ST},
   [ActExpShaCtr	] = {1376, 3,TabFil,Brw_ExpandFileTree		,NULL				,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3C0},{    0,    0},{    0,    0}},Act_NORM,Act_204},
   [ActConShaCtr	] = {1377, 3,TabFil,Brw_ContractFileTree	,NULL				,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3C0},{    0,    0},{    0,    0}},Act_NORM,Act_204},
   [ActZIPShaCtr	] = {1378, 3,TabFil,NULL			,ZIP_CompressFileTree		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3C0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActReqDatShaCtr	] = {1379, 3,TabFil,NULL			,Brw_ShowFileMetadata		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C7,0x3C7},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActChgDatShaCtr	] = {1380, 3,TabFil,NULL			,Brw_ChgFileMetadata		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3C0},{    0,    0},{    0,    0}},Act_NORM,Act_1ST},
   [ActDowShaCtr	] = {1381, 3,TabFil,Brw_DownloadFile		,NULL				,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C7,0x3C7},{    0,    0},{    0,    0}},Act_NORM,Act_DWN},

   // Documents of degree
   [ActSeeAdmDocDeg	] = {1247, 4,TabFil,NULL			,Brw_ShowFileBrowserOrWorks	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C7,0x3C7},{    0,    0}},Act_NORM,Act_1ST},
   [ActChgToSeeDocDeg	] = {1250, 4,TabFil,NULL			,Brw_ShowFileBrowserOrWorks	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3C0},{    0,    0}},Act_NORM,Act_1ST},
   [ActSeeDocDeg	] = {1251, 4,TabFil,NULL			,Brw_ShowFileBrowserOrWorks	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C7,0x3C7},{    0,    0}},Act_NORM,Act_1ST},
   [ActExpSeeDocDeg	] = {1252, 4,TabFil,Brw_ExpandFileTree		,NULL				,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C7,0x3C7},{    0,    0}},Act_NORM,Act_204},
   [ActConSeeDocDeg	] = {1253, 4,TabFil,Brw_ContractFileTree	,NULL				,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C7,0x3C7},{    0,    0}},Act_NORM,Act_204},
   [ActZIPSeeDocDeg	] = {1254, 4,TabFil,NULL			,ZIP_CompressFileTree		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C7,0x3C7},{    0,    0}},Act_NORM,Act_1ST},
   [ActReqDatSeeDocDeg	] = {1255, 4,TabFil,NULL			,Brw_ShowFileMetadata		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C7,0x3C7},{    0,    0}},Act_NORM,Act_1ST},
   [ActDowSeeDocDeg	] = {1256, 4,TabFil,Brw_DownloadFile		,NULL				,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C7,0x3C7},{    0,    0}},Act_NORM,Act_DWN},
   [ActChgToAdmDocDeg	] = {1257, 4,TabFil,NULL			,Brw_ShowFileBrowserOrWorks	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3C0},{    0,    0}},Act_NORM,Act_1ST},
   [ActAdmDocDeg	] = {1258, 4,TabFil,NULL			,Brw_ShowFileBrowserOrWorks	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3C0},{    0,    0}},Act_NORM,Act_1ST},
   [ActReqRemFilDocDeg	] = {1259, 4,TabFil,NULL			,Brw_ReqRemFile			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3C0},{    0,    0}},Act_NORM,Act_1ST},
   [ActRemFilDocDeg	] = {1260, 4,TabFil,NULL			,Brw_RemFile			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3C0},{    0,    0}},Act_NORM,Act_1ST},
   [ActRemFolDocDeg	] = {1261, 4,TabFil,NULL			,Brw_RemFolder			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3C0},{    0,    0}},Act_NORM,Act_1ST},
   [ActCopDocDeg	] = {1262, 4,TabFil,NULL			,Brw_Copy			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3C0},{    0,    0}},Act_NORM,Act_1ST},
   [ActPasDocDeg	] = {1263, 4,TabFil,NULL			,Brw_Paste			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3C0},{    0,    0}},Act_NORM,Act_1ST},
   [ActRemTreDocDeg	] = {1264, 4,TabFil,NULL			,Brw_RemSubtree			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3C0},{    0,    0}},Act_NORM,Act_1ST},
   [ActFrmCreDocDeg	] = {1265, 4,TabFil,NULL			,Brw_ShowFormFileBrowser	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3C0},{    0,    0}},Act_NORM,Act_1ST},
   [ActCreFolDocDeg	] = {1266, 4,TabFil,NULL			,Brw_CreateFolder		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3C0},{    0,    0}},Act_NORM,Act_1ST},
   [ActCreLnkDocDeg	] = {1267, 4,TabFil,NULL			,Brw_CreateLink			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3C0},{    0,    0}},Act_NORM,Act_1ST},
   [ActRenFolDocDeg	] = {1268, 4,TabFil,NULL			,Brw_RenFolder			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3C0},{    0,    0}},Act_NORM,Act_1ST},
   [ActRcvFilDocDegDZ	] = {1269, 4,TabFil,Brw_RcvFileDZ		,NULL				,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3C0},{    0,    0}},Act_DATA,Act_UPL},
   [ActRcvFilDocDegCla	] = {1270, 4,TabFil,NULL			,Brw_RcvFileClassic		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3C0},{    0,    0}},Act_DATA,Act_1ST},
   [ActExpAdmDocDeg	] = {1271, 4,TabFil,Brw_ExpandFileTree		,NULL				,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3C0},{    0,    0}},Act_NORM,Act_204},
   [ActConAdmDocDeg	] = {1272, 4,TabFil,Brw_ContractFileTree	,NULL				,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3C0},{    0,    0}},Act_NORM,Act_204},
   [ActZIPAdmDocDeg	] = {1273, 4,TabFil,NULL			,ZIP_CompressFileTree		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3C0},{    0,    0}},Act_NORM,Act_1ST},
   [ActUnhDocDeg	] = {1274, 4,TabFil,NULL			,Brw_SetDocumentAsVisible	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3C0},{    0,    0}},Act_NORM,Act_1ST},
   [ActHidDocDeg	] = {1275, 4,TabFil,NULL			,Brw_SetDocumentAsHidden	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3C0},{    0,    0}},Act_NORM,Act_1ST},
   [ActReqDatAdmDocDeg	] = {1276, 4,TabFil,NULL			,Brw_ShowFileMetadata		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3C0},{    0,    0}},Act_NORM,Act_1ST},
   [ActChgDatAdmDocDeg	] = {1277, 4,TabFil,NULL			,Brw_ChgFileMetadata		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3C0},{    0,    0}},Act_NORM,Act_1ST},
   [ActDowAdmDocDeg	] = {1278, 4,TabFil,Brw_DownloadFile		,NULL				,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3C0},{    0,    0}},Act_NORM,Act_DWN},

   // Shared files of degree
   [ActAdmShaDeg	] = {1344, 5,TabFil,NULL			,Brw_ShowFileBrowserOrWorks	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C7,0x3C7},{    0,    0}},Act_NORM,Act_1ST},
   [ActReqRemFilShaDeg	] = {1345, 5,TabFil,NULL			,Brw_ReqRemFile			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3C0},{    0,    0}},Act_NORM,Act_1ST},
   [ActRemFilShaDeg	] = {1346, 5,TabFil,NULL			,Brw_RemFile			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3C0},{    0,    0}},Act_NORM,Act_1ST},
   [ActRemFolShaDeg	] = {1347, 5,TabFil,NULL			,Brw_RemFolder			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3C0},{    0,    0}},Act_NORM,Act_1ST},
   [ActCopShaDeg	] = {1348, 5,TabFil,NULL			,Brw_Copy			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3C0},{    0,    0}},Act_NORM,Act_1ST},
   [ActPasShaDeg	] = {1349, 5,TabFil,NULL			,Brw_Paste			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3C0},{    0,    0}},Act_NORM,Act_1ST},
   [ActRemTreShaDeg	] = {1350, 5,TabFil,NULL			,Brw_RemSubtree			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3C0},{    0,    0}},Act_NORM,Act_1ST},
   [ActFrmCreShaDeg	] = {1351, 5,TabFil,NULL			,Brw_ShowFormFileBrowser	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3C0},{    0,    0}},Act_NORM,Act_1ST},
   [ActCreFolShaDeg	] = {1352, 5,TabFil,NULL			,Brw_CreateFolder		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3C0},{    0,    0}},Act_NORM,Act_1ST},
   [ActCreLnkShaDeg	] = {1353, 5,TabFil,NULL			,Brw_CreateLink			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3C0},{    0,    0}},Act_NORM,Act_1ST},
   [ActRenFolShaDeg	] = {1354, 5,TabFil,NULL			,Brw_RenFolder			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3C0},{    0,    0}},Act_NORM,Act_1ST},
   [ActRcvFilShaDegDZ	] = {1355, 5,TabFil,Brw_RcvFileDZ		,NULL				,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3C0},{    0,    0}},Act_DATA,Act_UPL},
   [ActRcvFilShaDegCla	] = {1356, 5,TabFil,NULL			,Brw_RcvFileClassic		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3C0},{    0,    0}},Act_DATA,Act_1ST},
   [ActExpShaDeg	] = {1357, 5,TabFil,Brw_ExpandFileTree		,NULL				,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3C0},{    0,    0}},Act_NORM,Act_204},
   [ActConShaDeg	] = {1358, 5,TabFil,Brw_ContractFileTree	,NULL				,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3C0},{    0,    0}},Act_NORM,Act_204},
   [ActZIPShaDeg	] = {1359, 5,TabFil,NULL			,ZIP_CompressFileTree		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3C0},{    0,    0}},Act_NORM,Act_1ST},
   [ActReqDatShaDeg	] = {1360, 5,TabFil,NULL			,Brw_ShowFileMetadata		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C7,0x3C7},{    0,    0}},Act_NORM,Act_1ST},
   [ActChgDatShaDeg	] = {1361, 5,TabFil,NULL			,Brw_ChgFileMetadata		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3C0},{    0,    0}},Act_NORM,Act_1ST},
   [ActDowShaDeg	] = {1362, 5,TabFil,Brw_DownloadFile		,NULL				,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C7,0x3C7},{    0,    0}},Act_NORM,Act_DWN},

   // Documents of course
   [ActSeeAdmDocCrsGrp	] = {   0, 6,TabFil,NULL			,Brw_ShowFileBrowserOrWorks	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C7,0x3F8}},Act_NORM,Act_1ST},
   [ActChgToSeeDocCrs	] = {1195, 6,TabFil,NULL			,Brw_ShowFileBrowserOrWorks	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C7,0x3F8}},Act_NORM,Act_1ST},
   [ActSeeDocCrs	] = {1078, 6,TabFil,NULL			,Brw_ShowFileBrowserOrWorks	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C7,0x3F8}},Act_NORM,Act_1ST},
   [ActExpSeeDocCrs	] = { 462, 6,TabFil,Brw_ExpandFileTree		,NULL				,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_204},
   [ActConSeeDocCrs	] = { 476, 6,TabFil,Brw_ContractFileTree	,NULL				,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_204},
   [ActZIPSeeDocCrs	] = {1124, 6,TabFil,NULL			,ZIP_CompressFileTree		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActReqDatSeeDocCrs	] = {1033, 6,TabFil,NULL			,Brw_ShowFileMetadata		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C7,0x3F8}},Act_NORM,Act_1ST},
   [ActReqLnkSeeDocCrs	] = {1930, 6,TabFil,NULL			,BrwRsc_GetLinkToDocFil		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActDowSeeDocCrs	] = {1111, 6,TabFil,Brw_DownloadFile		,NULL				,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C7,0x3F8}},Act_NORM,Act_DWN},
   [ActSeeDocGrp	] = {1200, 6,TabFil,NULL			,Brw_ShowFileBrowserOrWorks	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActExpSeeDocGrp	] = { 488, 6,TabFil,Brw_ExpandFileTree		,NULL				,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_204},
   [ActConSeeDocGrp	] = { 489, 6,TabFil,Brw_ContractFileTree	,NULL				,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_204},
   [ActZIPSeeDocGrp	] = {1125, 6,TabFil,NULL			,ZIP_CompressFileTree		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActReqDatSeeDocGrp	] = {1034, 6,TabFil,NULL			,Brw_ShowFileMetadata		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C7,0x3F8}},Act_NORM,Act_1ST},
   [ActDowSeeDocGrp	] = {1112, 6,TabFil,Brw_DownloadFile		,NULL				,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C7,0x3F8}},Act_NORM,Act_DWN},
   [ActChgToAdmDocCrs	] = {1196, 6,TabFil,NULL			,Brw_ShowFileBrowserOrWorks	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActAdmDocCrs	] = {  12, 6,TabFil,NULL			,Brw_ShowFileBrowserOrWorks	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActReqRemFilDocCrs	] = { 479, 6,TabFil,NULL			,Brw_ReqRemFile			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActRemFilDocCrs	] = { 480, 6,TabFil,NULL			,Brw_RemFile			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActRemFolDocCrs	] = { 497, 6,TabFil,NULL			,Brw_RemFolder			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActCopDocCrs	] = { 470, 6,TabFil,NULL			,Brw_Copy			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActPasDocCrs	] = { 478, 6,TabFil,NULL			,Brw_Paste			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActRemTreDocCrs	] = { 498, 6,TabFil,NULL			,Brw_RemSubtree			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActFrmCreDocCrs	] = { 481, 6,TabFil,NULL			,Brw_ShowFormFileBrowser	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActCreFolDocCrs	] = { 491, 6,TabFil,NULL			,Brw_CreateFolder		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActCreLnkDocCrs	] = {1225, 6,TabFil,NULL			,Brw_CreateLink			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActRenFolDocCrs	] = { 535, 6,TabFil,NULL			,Brw_RenFolder			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActRcvFilDocCrsDZ	] = {1214, 6,TabFil,Brw_RcvFileDZ		,NULL				,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_DATA,Act_UPL},
   [ActRcvFilDocCrsCla	] = { 482, 6,TabFil,NULL			,Brw_RcvFileClassic		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_DATA,Act_1ST},
   [ActExpAdmDocCrs	] = { 477, 6,TabFil,Brw_ExpandFileTree		,NULL				,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_204},
   [ActConAdmDocCrs	] = { 494, 6,TabFil,Brw_ContractFileTree	,NULL				,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_204},
   [ActZIPAdmDocCrs	] = {1126, 6,TabFil,NULL			,ZIP_CompressFileTree		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActUnhDocCrs	] = { 464, 6,TabFil,NULL			,Brw_SetDocumentAsVisible	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActHidDocCrs	] = { 465, 6,TabFil,NULL			,Brw_SetDocumentAsHidden	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActReqDatAdmDocCrs	] = {1029, 6,TabFil,NULL			,Brw_ShowFileMetadata		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActChgDatAdmDocCrs	] = { 996, 6,TabFil,NULL			,Brw_ChgFileMetadata		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActReqLnkAdmDocCrs	] = {1931, 6,TabFil,NULL			,BrwRsc_GetLinkToDocFil		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActDowAdmDocCrs	] = {1113, 6,TabFil,Brw_DownloadFile		,NULL				,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_DWN},
   [ActAdmDocGrp	] = {1201, 6,TabFil,NULL			,Brw_ShowFileBrowserOrWorks	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActReqRemFilDocGrp	] = { 473, 6,TabFil,NULL			,Brw_ReqRemFile			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActRemFilDocGrp	] = { 474, 6,TabFil,NULL			,Brw_RemFile			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActRemFolDocGrp	] = { 484, 6,TabFil,NULL			,Brw_RemFolder			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActCopDocGrp	] = { 472, 6,TabFil,NULL			,Brw_Copy			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActPasDocGrp	] = { 471, 6,TabFil,NULL			,Brw_Paste			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActRemTreDocGrp	] = { 485, 6,TabFil,NULL			,Brw_RemSubtree			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActFrmCreDocGrp	] = { 468, 6,TabFil,NULL			,Brw_ShowFormFileBrowser	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActCreFolDocGrp	] = { 469, 6,TabFil,NULL			,Brw_CreateFolder		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActCreLnkDocGrp	] = {1231, 6,TabFil,NULL			,Brw_CreateLink			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActRenFolDocGrp	] = { 490, 6,TabFil,NULL			,Brw_RenFolder			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActRcvFilDocGrpDZ	] = {1215, 6,TabFil,Brw_RcvFileDZ		,NULL				,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_DATA,Act_UPL},
   [ActRcvFilDocGrpCla	] = { 483, 6,TabFil,NULL			,Brw_RcvFileClassic		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_DATA,Act_1ST},
   [ActExpAdmDocGrp	] = { 486, 6,TabFil,Brw_ExpandFileTree		,NULL				,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_204},
   [ActConAdmDocGrp	] = { 487, 6,TabFil,Brw_ContractFileTree	,NULL				,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_204},
   [ActZIPAdmDocGrp	] = {1127, 6,TabFil,NULL			,ZIP_CompressFileTree		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActUnhDocGrp	] = { 493, 6,TabFil,NULL			,Brw_SetDocumentAsVisible	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActHidDocGrp	] = { 492, 6,TabFil,NULL			,Brw_SetDocumentAsHidden	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActReqDatAdmDocGrp	] = {1030, 6,TabFil,NULL			,Brw_ShowFileMetadata		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActChgDatAdmDocGrp	] = { 998, 6,TabFil,NULL			,Brw_ChgFileMetadata		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActDowAdmDocGrp	] = {1114, 6,TabFil,Brw_DownloadFile		,NULL				,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_DWN},

   // Private documents for course teachers
   [ActAdmTchCrsGrp	] = {1525, 7,TabFil,NULL			,Brw_ShowFileBrowserOrWorks	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3F0}},Act_NORM,Act_1ST},
   [ActChgToAdmTch	] = {1526, 7,TabFil,NULL			,Brw_ShowFileBrowserOrWorks	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3F0}},Act_NORM,Act_1ST},
   [ActAdmTchCrs	] = {1527, 7,TabFil,NULL			,Brw_ShowFileBrowserOrWorks	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3F0}},Act_NORM,Act_1ST},
   [ActReqRemFilTchCrs	] = {1528, 7,TabFil,NULL			,Brw_ReqRemFile			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3F0}},Act_NORM,Act_1ST},
   [ActRemFilTchCrs	] = {1529, 7,TabFil,NULL			,Brw_RemFile			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3F0}},Act_NORM,Act_1ST},
   [ActRemFolTchCrs	] = {1530, 7,TabFil,NULL			,Brw_RemFolder			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3F0}},Act_NORM,Act_1ST},
   [ActCopTchCrs	] = {1531, 7,TabFil,NULL			,Brw_Copy			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3F0}},Act_NORM,Act_1ST},
   [ActPasTchCrs	] = {1532, 7,TabFil,NULL			,Brw_Paste			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3F0}},Act_NORM,Act_1ST},
   [ActRemTreTchCrs	] = {1533, 7,TabFil,NULL			,Brw_RemSubtree			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3F0}},Act_NORM,Act_1ST},
   [ActFrmCreTchCrs	] = {1534, 7,TabFil,NULL			,Brw_ShowFormFileBrowser	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3F0}},Act_NORM,Act_1ST},
   [ActCreFolTchCrs	] = {1535, 7,TabFil,NULL			,Brw_CreateFolder		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3F0}},Act_NORM,Act_1ST},
   [ActCreLnkTchCrs	] = {1536, 7,TabFil,NULL			,Brw_CreateLink			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3F0}},Act_NORM,Act_1ST},
   [ActRenFolTchCrs	] = {1537, 7,TabFil,NULL			,Brw_RenFolder			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3F0}},Act_NORM,Act_1ST},
   [ActRcvFilTchCrsDZ	] = {1538, 7,TabFil,Brw_RcvFileDZ		,NULL				,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3F0}},Act_DATA,Act_UPL},
   [ActRcvFilTchCrsCla	] = {1539, 7,TabFil,NULL			,Brw_RcvFileClassic		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3F0}},Act_DATA,Act_1ST},
   [ActExpTchCrs	] = {1540, 7,TabFil,Brw_ExpandFileTree		,NULL				,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3F0}},Act_NORM,Act_204},
   [ActConTchCrs	] = {1541, 7,TabFil,Brw_ContractFileTree	,NULL				,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3F0}},Act_NORM,Act_204},
   [ActZIPTchCrs	] = {1542, 7,TabFil,NULL			,ZIP_CompressFileTree		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3F0}},Act_NORM,Act_1ST},
   [ActReqDatTchCrs	] = {1543, 7,TabFil,NULL			,Brw_ShowFileMetadata		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3F0}},Act_NORM,Act_1ST},
   [ActChgDatTchCrs	] = {1544, 7,TabFil,NULL			,Brw_ChgFileMetadata		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3F0}},Act_NORM,Act_1ST},
   [ActDowTchCrs	] = {1545, 7,TabFil,Brw_DownloadFile		,NULL				,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3F0}},Act_NORM,Act_DWN},
   [ActAdmTchGrp	] = {1546, 7,TabFil,NULL			,Brw_ShowFileBrowserOrWorks	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3F0}},Act_NORM,Act_1ST},
   [ActReqRemFilTchGrp	] = {1547, 7,TabFil,NULL			,Brw_ReqRemFile			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3F0}},Act_NORM,Act_1ST},
   [ActRemFilTchGrp	] = {1548, 7,TabFil,NULL			,Brw_RemFile			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3F0}},Act_NORM,Act_1ST},
   [ActRemFolTchGrp	] = {1549, 7,TabFil,NULL			,Brw_RemFolder			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3F0}},Act_NORM,Act_1ST},
   [ActCopTchGrp	] = {1550, 7,TabFil,NULL			,Brw_Copy			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3F0}},Act_NORM,Act_1ST},
   [ActPasTchGrp	] = {1551, 7,TabFil,NULL			,Brw_Paste			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3F0}},Act_NORM,Act_1ST},
   [ActRemTreTchGrp	] = {1552, 7,TabFil,NULL			,Brw_RemSubtree			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3F0}},Act_NORM,Act_1ST},
   [ActFrmCreTchGrp	] = {1553, 7,TabFil,NULL			,Brw_ShowFormFileBrowser	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3F0}},Act_NORM,Act_1ST},
   [ActCreFolTchGrp	] = {1554, 7,TabFil,NULL			,Brw_CreateFolder		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3F0}},Act_NORM,Act_1ST},
   [ActCreLnkTchGrp	] = {1555, 7,TabFil,NULL			,Brw_CreateLink			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3F0}},Act_NORM,Act_1ST},
   [ActRenFolTchGrp	] = {1556, 7,TabFil,NULL			,Brw_RenFolder			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3F0}},Act_NORM,Act_1ST},
   [ActRcvFilTchGrpDZ	] = {1557, 7,TabFil,Brw_RcvFileDZ		,NULL				,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3F0}},Act_DATA,Act_UPL},
   [ActRcvFilTchGrpCla	] = {1558, 7,TabFil,NULL			,Brw_RcvFileClassic		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3F0}},Act_DATA,Act_1ST},
   [ActExpTchGrp	] = {1559, 7,TabFil,Brw_ExpandFileTree		,NULL				,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3F0}},Act_NORM,Act_204},
   [ActConTchGrp	] = {1560, 7,TabFil,Brw_ContractFileTree	,NULL				,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3F0}},Act_NORM,Act_204},
   [ActZIPTchGrp	] = {1561, 7,TabFil,NULL			,ZIP_CompressFileTree		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3F0}},Act_NORM,Act_1ST},
   [ActReqDatTchGrp	] = {1562, 7,TabFil,NULL			,Brw_ShowFileMetadata		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3F0}},Act_NORM,Act_1ST},
   [ActChgDatTchGrp	] = {1563, 7,TabFil,NULL			,Brw_ChgFileMetadata		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3F0}},Act_NORM,Act_1ST},
   [ActDowTchGrp	] = {1564, 7,TabFil,Brw_DownloadFile		,NULL				,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3F0}},Act_NORM,Act_DWN},

   // Shared files of course
   [ActAdmShaCrsGrp	] = { 461, 8,TabFil,NULL			,Brw_ShowFileBrowserOrWorks	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C7,0x3F8}},Act_NORM,Act_1ST},
   [ActChgToAdmSha	] = {1197, 8,TabFil,NULL			,Brw_ShowFileBrowserOrWorks	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActAdmShaCrs	] = {1202, 8,TabFil,NULL			,Brw_ShowFileBrowserOrWorks	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActReqRemFilShaCrs	] = { 327, 8,TabFil,NULL			,Brw_ReqRemFile			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActRemFilShaCrs	] = { 328, 8,TabFil,NULL			,Brw_RemFile			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActRemFolShaCrs	] = { 325, 8,TabFil,NULL			,Brw_RemFolder			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActCopShaCrs	] = { 330, 8,TabFil,NULL			,Brw_Copy			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActPasShaCrs	] = { 331, 8,TabFil,NULL			,Brw_Paste			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActRemTreShaCrs	] = { 332, 8,TabFil,NULL			,Brw_RemSubtree			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActFrmCreShaCrs	] = { 323, 8,TabFil,NULL			,Brw_ShowFormFileBrowser	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActCreFolShaCrs	] = { 324, 8,TabFil,NULL			,Brw_CreateFolder		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActCreLnkShaCrs	] = {1226, 8,TabFil,NULL			,Brw_CreateLink			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActRenFolShaCrs	] = { 329, 8,TabFil,NULL			,Brw_RenFolder			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActRcvFilShaCrsDZ	] = {1216, 8,TabFil,Brw_RcvFileDZ		,NULL				,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_DATA,Act_UPL},
   [ActRcvFilShaCrsCla	] = { 326, 8,TabFil,NULL			,Brw_RcvFileClassic		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_DATA,Act_1ST},
   [ActExpShaCrs	] = { 421, 8,TabFil,Brw_ExpandFileTree		,NULL				,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_204},
   [ActConShaCrs	] = { 422, 8,TabFil,Brw_ContractFileTree	,NULL				,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_204},
   [ActZIPShaCrs	] = {1128, 8,TabFil,NULL			,ZIP_CompressFileTree		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActReqDatShaCrs	] = {1031, 8,TabFil,NULL			,Brw_ShowFileMetadata		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C7,0x3F8}},Act_NORM,Act_1ST},
   [ActChgDatShaCrs	] = {1000, 8,TabFil,NULL			,Brw_ChgFileMetadata		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActDowShaCrs	] = {1115, 8,TabFil,Brw_DownloadFile		,NULL				,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C7,0x3F8}},Act_NORM,Act_DWN},
   [ActAdmShaGrp	] = {1203, 8,TabFil,NULL			,Brw_ShowFileBrowserOrWorks	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActReqRemFilShaGrp	] = { 341, 8,TabFil,NULL			,Brw_ReqRemFile			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActRemFilShaGrp	] = { 342, 8,TabFil,NULL			,Brw_RemFile			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActRemFolShaGrp	] = { 338, 8,TabFil,NULL			,Brw_RemFolder			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActCopShaGrp	] = { 336, 8,TabFil,NULL			,Brw_Copy			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActPasShaGrp	] = { 337, 8,TabFil,NULL			,Brw_Paste			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActRemTreShaGrp	] = { 339, 8,TabFil,NULL			,Brw_RemSubtree			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActFrmCreShaGrp	] = { 333, 8,TabFil,NULL			,Brw_ShowFormFileBrowser	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActCreFolShaGrp	] = { 334, 8,TabFil,NULL			,Brw_CreateFolder		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActCreLnkShaGrp	] = {1227, 8,TabFil,NULL			,Brw_CreateLink			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActRenFolShaGrp	] = { 340, 8,TabFil,NULL			,Brw_RenFolder			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActRcvFilShaGrpDZ	] = {1217, 8,TabFil,Brw_RcvFileDZ		,NULL				,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_DATA,Act_UPL},
   [ActRcvFilShaGrpCla	] = { 335, 8,TabFil,NULL			,Brw_RcvFileClassic		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_DATA,Act_1ST},
   [ActExpShaGrp	] = { 427, 8,TabFil,Brw_ExpandFileTree		,NULL				,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_204},
   [ActConShaGrp	] = { 426, 8,TabFil,Brw_ContractFileTree	,NULL				,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_204},
   [ActZIPShaGrp	] = {1129, 8,TabFil,NULL			,ZIP_CompressFileTree		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActReqDatShaGrp	] = {1032, 8,TabFil,NULL			,Brw_ShowFileMetadata		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C7,0x3F8}},Act_NORM,Act_1ST},
   [ActChgDatShaGrp	] = {1002, 8,TabFil,NULL			,Brw_ChgFileMetadata		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActDowShaGrp	] = {1116, 8,TabFil,Brw_DownloadFile		,NULL				,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C7,0x3F8}},Act_NORM,Act_DWN},

   // Assignments and works of user
   [ActAdmAsgWrkUsr	] = { 792, 9,TabFil,NULL			,Brw_ShowFileBrowserOrWorks	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,0x008}},Act_NORM,Act_1ST},
   [ActReqRemFilAsgUsr	] = { 834, 9,TabFil,NULL			,Brw_ReqRemFile			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,0x008}},Act_NORM,Act_1ST},
   [ActRemFilAsgUsr	] = { 833, 9,TabFil,NULL			,Brw_RemFile			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,0x008}},Act_NORM,Act_1ST},
   [ActRemFolAsgUsr	] = { 827, 9,TabFil,NULL			,Brw_RemFolder			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,0x008}},Act_NORM,Act_1ST},
   [ActCopAsgUsr	] = { 829, 9,TabFil,NULL			,Brw_Copy			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,0x008}},Act_NORM,Act_1ST},
   [ActPasAsgUsr	] = { 830, 9,TabFil,NULL			,Brw_Paste			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,0x008}},Act_NORM,Act_1ST},
   [ActRemTreAsgUsr	] = { 828, 9,TabFil,NULL			,Brw_RemSubtree			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,0x008}},Act_NORM,Act_1ST},
   [ActFrmCreAsgUsr	] = { 825, 9,TabFil,NULL			,Brw_ShowFormFileBrowser	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,0x008}},Act_NORM,Act_1ST},
   [ActCreFolAsgUsr	] = { 826, 9,TabFil,NULL			,Brw_CreateFolder		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,0x008}},Act_NORM,Act_1ST},
   [ActCreLnkAsgUsr	] = {1232, 9,TabFil,NULL			,Brw_CreateLink			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,0x008}},Act_NORM,Act_1ST},
   [ActRenFolAsgUsr	] = { 839, 9,TabFil,NULL			,Brw_RenFolder			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,0x008}},Act_NORM,Act_1ST},
   [ActRcvFilAsgUsrDZ	] = {1218, 9,TabFil,Brw_RcvFileDZ		,NULL				,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,0x008}},Act_DATA,Act_UPL},
   [ActRcvFilAsgUsrCla	] = { 832, 9,TabFil,NULL			,Brw_RcvFileClassic		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,0x008}},Act_DATA,Act_1ST},
   [ActExpAsgUsr	] = { 824, 9,TabFil,Brw_ExpandFileTree		,NULL				,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,0x008}},Act_NORM,Act_204},
   [ActConAsgUsr	] = { 831, 9,TabFil,Brw_ContractFileTree	,NULL				,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,0x008}},Act_NORM,Act_204},
   [ActZIPAsgUsr	] = {1130, 9,TabFil,NULL			,ZIP_CompressFileTree		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,0x008}},Act_NORM,Act_1ST},
   [ActReqDatAsgUsr	] = {1039, 9,TabFil,NULL			,Brw_ShowFileMetadata		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,0x008}},Act_NORM,Act_1ST},
   [ActChgDatAsgUsr	] = {1040, 9,TabFil,NULL			,Brw_ChgFileMetadata		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,0x008}},Act_NORM,Act_1ST},
   [ActDowAsgUsr	] = {1117, 9,TabFil,Brw_DownloadFile		,NULL				,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,0x008}},Act_NORM,Act_DWN},
   [ActReqRemFilWrkUsr	] = { 288, 9,TabFil,NULL			,Brw_ReqRemFile			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,0x008}},Act_NORM,Act_1ST},
   [ActRemFilWrkUsr	] = { 169, 9,TabFil,NULL			,Brw_RemFile			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,0x008}},Act_NORM,Act_1ST},
   [ActRemFolWrkUsr	] = { 228, 9,TabFil,NULL			,Brw_RemFolder			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,0x008}},Act_NORM,Act_1ST},
   [ActCopWrkUsr	] = { 314, 9,TabFil,NULL			,Brw_Copy			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,0x008}},Act_NORM,Act_1ST},
   [ActPasWrkUsr	] = { 318, 9,TabFil,NULL			,Brw_Paste			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,0x008}},Act_NORM,Act_1ST},
   [ActRemTreWrkUsr	] = { 278, 9,TabFil,NULL			,Brw_RemSubtree			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,0x008}},Act_NORM,Act_1ST},
   [ActFrmCreWrkUsr	] = { 150, 9,TabFil,NULL			,Brw_ShowFormFileBrowser	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,0x008}},Act_NORM,Act_1ST},
   [ActCreFolWrkUsr	] = { 172, 9,TabFil,NULL			,Brw_CreateFolder		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,0x008}},Act_NORM,Act_1ST},
   [ActCreLnkWrkUsr	] = {1228, 9,TabFil,NULL			,Brw_CreateLink			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,0x008}},Act_NORM,Act_1ST},
   [ActRenFolWrkUsr	] = { 204, 9,TabFil,NULL			,Brw_RenFolder			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,0x008}},Act_NORM,Act_1ST},
   [ActRcvFilWrkUsrDZ	] = {1219, 9,TabFil,Brw_RcvFileDZ		,NULL				,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,0x008}},Act_DATA,Act_UPL},
   [ActRcvFilWrkUsrCla	] = { 148, 9,TabFil,NULL			,Brw_RcvFileClassic		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,0x008}},Act_DATA,Act_1ST},
   [ActExpWrkUsr	] = { 423, 9,TabFil,Brw_ExpandFileTree		,NULL				,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,0x008}},Act_NORM,Act_204},
   [ActConWrkUsr	] = { 425, 9,TabFil,Brw_ContractFileTree	,NULL				,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,0x008}},Act_NORM,Act_204},
   [ActZIPWrkUsr	] = {1131, 9,TabFil,NULL			,ZIP_CompressFileTree		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,0x008}},Act_NORM,Act_1ST},
   [ActReqDatWrkUsr	] = {1041, 9,TabFil,NULL			,Brw_ShowFileMetadata		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,0x008}},Act_NORM,Act_1ST},
   [ActChgDatWrkUsr	] = {1042, 9,TabFil,NULL			,Brw_ChgFileMetadata		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,0x008}},Act_NORM,Act_1ST},
   [ActDowWrkUsr	] = {1118, 9,TabFil,Brw_DownloadFile		,NULL				,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,0x008}},Act_NORM,Act_DWN},

   // Assignments and works of course
   [ActReqAsgWrkCrs	] = { 899,10,TabFil,NULL			,Brw_AskEditWorksCrs		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x230}},Act_NORM,Act_1ST},
   [ActAdmAsgWrkCrs	] = { 139,10,TabFil,NULL			,Brw_GetSelectedUsrsAndShowWorks,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActReqRemFilAsgCrs	] = { 837,10,TabFil,NULL			,Brw_ReqRemFile			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x230}},Act_NORM,Act_1ST},
   [ActRemFilAsgCrs	] = { 838,10,TabFil,NULL			,Brw_RemFile			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x230}},Act_NORM,Act_1ST},
   [ActRemFolAsgCrs	] = { 820,10,TabFil,NULL			,Brw_RemFolder			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x230}},Act_NORM,Act_1ST},
   [ActCopAsgCrs	] = { 836,10,TabFil,NULL			,Brw_Copy			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x230}},Act_NORM,Act_1ST},
   [ActPasAsgCrs	] = { 821,10,TabFil,NULL			,Brw_Paste			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x230}},Act_NORM,Act_1ST},
   [ActRemTreAsgCrs	] = { 822,10,TabFil,NULL			,Brw_RemSubtree			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x230}},Act_NORM,Act_1ST},
   [ActFrmCreAsgCrs	] = { 817,10,TabFil,NULL			,Brw_ShowFormFileBrowser	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x230}},Act_NORM,Act_1ST},
   [ActCreFolAsgCrs	] = { 818,10,TabFil,NULL			,Brw_CreateFolder		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x230}},Act_NORM,Act_1ST},
   [ActCreLnkAsgCrs	] = {1233,10,TabFil,NULL			,Brw_CreateLink			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x230}},Act_NORM,Act_1ST},
   [ActRenFolAsgCrs	] = { 823,10,TabFil,NULL			,Brw_RenFolder			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x230}},Act_NORM,Act_1ST},
   [ActRcvFilAsgCrsDZ	] = {1220,10,TabFil,Brw_RcvFileDZ		,NULL				,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x230}},Act_DATA,Act_UPL},
   [ActRcvFilAsgCrsCla	] = { 846,10,TabFil,NULL			,Brw_RcvFileClassic		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x230}},Act_DATA,Act_1ST},
   [ActExpAsgCrs	] = { 819,10,TabFil,Brw_ExpandFileTree		,NULL				,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x230}},Act_NORM,Act_204},
   [ActConAsgCrs	] = { 835,10,TabFil,Brw_ContractFileTree	,NULL				,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x230}},Act_NORM,Act_204},
   [ActZIPAsgCrs	] = {1132,10,TabFil,NULL			,ZIP_CompressFileTree		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x230}},Act_NORM,Act_1ST},
   [ActReqDatAsgCrs	] = {1043,10,TabFil,NULL			,Brw_ShowFileMetadata		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x230}},Act_NORM,Act_1ST},
   [ActChgDatAsgCrs	] = {1044,10,TabFil,NULL			,Brw_ChgFileMetadata		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x230}},Act_NORM,Act_1ST},
   [ActDowAsgCrs	] = {1119,10,TabFil,Brw_DownloadFile		,NULL				,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x230}},Act_NORM,Act_DWN},
   [ActReqRemFilWrkCrs	] = { 289,10,TabFil,NULL			,Brw_ReqRemFile			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x230}},Act_NORM,Act_1ST},
   [ActRemFilWrkCrs	] = { 209,10,TabFil,NULL			,Brw_RemFile			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x230}},Act_NORM,Act_1ST},
   [ActRemFolWrkCrs	] = { 210,10,TabFil,NULL			,Brw_RemFolder			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x230}},Act_NORM,Act_1ST},
   [ActCopWrkCrs	] = { 312,10,TabFil,NULL			,Brw_Copy			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x230}},Act_NORM,Act_1ST},
   [ActPasWrkCrs	] = { 319,10,TabFil,NULL			,Brw_Paste			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x230}},Act_NORM,Act_1ST},
   [ActRemTreWrkCrs	] = { 279,10,TabFil,NULL			,Brw_RemSubtree			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x230}},Act_NORM,Act_1ST},
   [ActFrmCreWrkCrs	] = { 205,10,TabFil,NULL			,Brw_ShowFormFileBrowser	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x230}},Act_NORM,Act_1ST},
   [ActCreFolWrkCrs	] = { 206,10,TabFil,NULL			,Brw_CreateFolder		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x230}},Act_NORM,Act_1ST},
   [ActCreLnkWrkCrs	] = {1229,10,TabFil,NULL			,Brw_CreateLink			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x230}},Act_NORM,Act_1ST},
   [ActRenFolWrkCrs	] = { 208,10,TabFil,NULL			,Brw_RenFolder			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x230}},Act_NORM,Act_1ST},
   [ActRcvFilWrkCrsDZ	] = {1221,10,TabFil,Brw_RcvFileDZ		,NULL				,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x230}},Act_DATA,Act_UPL},
   [ActRcvFilWrkCrsCla	] = { 207,10,TabFil,NULL			,Brw_RcvFileClassic		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x230}},Act_DATA,Act_1ST},
   [ActExpWrkCrs	] = { 416,10,TabFil,Brw_ExpandFileTree		,NULL				,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x230}},Act_NORM,Act_204},
   [ActConWrkCrs	] = { 424,10,TabFil,Brw_ContractFileTree	,NULL				,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x230}},Act_NORM,Act_204},
   [ActZIPWrkCrs	] = {1133,10,TabFil,NULL			,ZIP_CompressFileTree		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x230}},Act_NORM,Act_1ST},
   [ActReqDatWrkCrs	] = {1045,10,TabFil,NULL			,Brw_ShowFileMetadata		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x230}},Act_NORM,Act_1ST},
   [ActChgDatWrkCrs	] = {1046,10,TabFil,NULL			,Brw_ChgFileMetadata		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x230}},Act_NORM,Act_1ST},
   [ActDowWrkCrs	] = {1120,10,TabFil,Brw_DownloadFile		,NULL				,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x230}},Act_NORM,Act_DWN},

   // Marks
   [ActSeeAdmMrk	] = {  17,11,TabFil,NULL			,Brw_ShowFileBrowserOrWorks	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActChgToSeeMrk	] = {1198,11,TabFil,NULL			,Brw_ShowFileBrowserOrWorks	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActSeeMrkCrs	] = {1079,11,TabFil,NULL			,Brw_ShowFileBrowserOrWorks	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActExpSeeMrkCrs	] = { 528,11,TabFil,Brw_ExpandFileTree		,NULL				,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_204},
   [ActConSeeMrkCrs	] = { 527,11,TabFil,Brw_ContractFileTree	,NULL				,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_204},
   [ActReqDatSeeMrkCrs	] = {1086,11,TabFil,NULL			,Brw_ShowFileMetadata		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActReqLnkSeeMrkCrs	] = {1939,11,TabFil,NULL			,BrwRsc_GetLinkToMrkFil		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActSeeMyMrkCrs	] = { 523,11,TabFil,Mrk_ShowMyMarks		,NULL				,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_NEW},
   [ActSeeMrkGrp	] = {1204,11,TabFil,NULL			,Brw_ShowFileBrowserOrWorks	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActExpSeeMrkGrp	] = { 605,11,TabFil,Brw_ExpandFileTree		,NULL				,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_204},
   [ActConSeeMrkGrp	] = { 609,11,TabFil,Brw_ContractFileTree	,NULL				,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_204},
   [ActReqDatSeeMrkGrp	] = {1087,11,TabFil,NULL			,Brw_ShowFileMetadata		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActSeeMyMrkGrp	] = { 524,11,TabFil,Mrk_ShowMyMarks		,NULL				,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_NEW},
   [ActChgToAdmMrk	] = {1199,11,TabFil,NULL			,Brw_ShowFileBrowserOrWorks	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActAdmMrkCrs	] = { 284,11,TabFil,NULL			,Brw_ShowFileBrowserOrWorks	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActReqRemFilMrkCrs	] = { 595,11,TabFil,NULL			,Brw_ReqRemFile			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActRemFilMrkCrs	] = { 533,11,TabFil,NULL			,Brw_RemFile			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActRemFolMrkCrs	] = { 530,11,TabFil,NULL			,Brw_RemFolder			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActCopMrkCrs	] = { 501,11,TabFil,NULL			,Brw_Copy			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActPasMrkCrs	] = { 507,11,TabFil,NULL			,Brw_Paste			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActRemTreMrkCrs	] = { 534,11,TabFil,NULL			,Brw_RemSubtree			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActFrmCreMrkCrs	] = { 596,11,TabFil,NULL			,Brw_ShowFormFileBrowser	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActCreFolMrkCrs	] = { 506,11,TabFil,NULL			,Brw_CreateFolder		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActRenFolMrkCrs	] = { 574,11,TabFil,NULL			,Brw_RenFolder			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActRcvFilMrkCrsDZ	] = {1222,11,TabFil,Brw_RcvFileDZ		,NULL				,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_DATA,Act_UPL},
   [ActRcvFilMrkCrsCla	] = { 516,11,TabFil,NULL			,Brw_RcvFileClassic		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_DATA,Act_1ST},
   [ActExpAdmMrkCrs	] = { 607,11,TabFil,Brw_ExpandFileTree		,NULL				,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_204},
   [ActConAdmMrkCrs	] = { 621,11,TabFil,Brw_ContractFileTree	,NULL				,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_204},
   [ActZIPAdmMrkCrs	] = {1134,11,TabFil,NULL			,ZIP_CompressFileTree		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActUnhMrkCrs	] = {1191,11,TabFil,NULL			,Brw_SetDocumentAsVisible	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActHidMrkCrs	] = {1192,11,TabFil,NULL			,Brw_SetDocumentAsHidden	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActReqDatAdmMrkCrs	] = {1035,11,TabFil,NULL			,Brw_ShowFileMetadata		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActChgDatAdmMrkCrs	] = {1036,11,TabFil,NULL			,Brw_ChgFileMetadata		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActReqLnkAdmMrkCrs	] = {1940,11,TabFil,NULL			,BrwRsc_GetLinkToMrkFil		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActDowAdmMrkCrs	] = {1121,11,TabFil,Brw_DownloadFile		,NULL				,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C7,0x3F8}},Act_NORM,Act_DWN},
   [ActChgNumRowHeaCrs	] = { 503,11,TabFil,NULL			,Mrk_ChangeNumRowsHeader	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActChgNumRowFooCrs	] = { 504,11,TabFil,NULL			,Mrk_ChangeNumRowsFooter	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActAdmMrkGrp	] = {1205,11,TabFil,NULL			,Brw_ShowFileBrowserOrWorks	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActReqRemFilMrkGrp	] = { 600,11,TabFil,NULL			,Brw_ReqRemFile			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActRemFilMrkGrp	] = { 509,11,TabFil,NULL			,Brw_RemFile			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActRemFolMrkGrp	] = { 520,11,TabFil,NULL			,Brw_RemFolder			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActCopMrkGrp	] = { 519,11,TabFil,NULL			,Brw_Copy			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActPasMrkGrp	] = { 502,11,TabFil,NULL			,Brw_Paste			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActRemTreMrkGrp	] = { 521,11,TabFil,NULL			,Brw_RemSubtree			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActFrmCreMrkGrp	] = { 601,11,TabFil,NULL			,Brw_ShowFormFileBrowser	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActCreFolMrkGrp	] = { 513,11,TabFil,NULL			,Brw_CreateFolder		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActRenFolMrkGrp	] = { 529,11,TabFil,NULL			,Brw_RenFolder			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActRcvFilMrkGrpDZ	] = {1223,11,TabFil,Brw_RcvFileDZ		,NULL				,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_DATA,Act_UPL},
   [ActRcvFilMrkGrpCla	] = { 514,11,TabFil,NULL			,Brw_RcvFileClassic		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_DATA,Act_1ST},
   [ActExpAdmMrkGrp	] = { 631,11,TabFil,Brw_ExpandFileTree		,NULL				,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_204},
   [ActConAdmMrkGrp	] = { 900,11,TabFil,Brw_ContractFileTree	,NULL				,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_204},
   [ActZIPAdmMrkGrp	] = {1135,11,TabFil,NULL			,ZIP_CompressFileTree		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActUnhMrkGrp	] = {1193,11,TabFil,NULL			,Brw_SetDocumentAsVisible	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActHidMrkGrp	] = {1194,11,TabFil,NULL			,Brw_SetDocumentAsHidden	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActReqDatAdmMrkGrp	] = {1037,11,TabFil,NULL			,Brw_ShowFileMetadata		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActChgDatAdmMrkGrp	] = {1038,11,TabFil,NULL			,Brw_ChgFileMetadata		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActDowAdmMrkGrp	] = {1122,11,TabFil,Brw_DownloadFile		,NULL				,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C7,0x3F8}},Act_NORM,Act_DWN},
   [ActChgNumRowHeaGrp	] = { 510,11,TabFil,NULL			,Mrk_ChangeNumRowsHeader	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActChgNumRowFooGrp	] = { 511,11,TabFil,NULL			,Mrk_ChangeNumRowsFooter	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},

   // Briefcase
   [ActAdmBrf		] = {  23,12,TabFil,NULL			,Brw_ShowFileBrowserOrWorks	,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_1ST},
   [ActReqRemFilBrf	] = { 286,12,TabFil,NULL			,Brw_ReqRemFile			,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_1ST},
   [ActRemFilBrf	] = { 155,12,TabFil,NULL			,Brw_RemFile			,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_1ST},
   [ActRemFolBrf	] = { 196,12,TabFil,NULL			,Brw_RemFolder			,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_1ST},
   [ActCopBrf		] = { 311,12,TabFil,NULL			,Brw_Copy			,{{    0,    0},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3F8}},Act_NORM,Act_1ST},
   [ActPasBrf		] = { 315,12,TabFil,NULL			,Brw_Paste			,{{    0,    0},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3F8}},Act_NORM,Act_1ST},
   [ActRemTreBrf	] = { 276,12,TabFil,NULL			,Brw_RemSubtree			,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_1ST},
   [ActFrmCreBrf	] = { 597,12,TabFil,NULL			,Brw_ShowFormFileBrowser	,{{    0,    0},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3F8}},Act_NORM,Act_1ST},
   [ActCreFolBrf	] = { 170,12,TabFil,NULL			,Brw_CreateFolder		,{{    0,    0},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3F8}},Act_NORM,Act_1ST},
   [ActCreLnkBrf	] = {1230,12,TabFil,NULL			,Brw_CreateLink			,{{    0,    0},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3F8}},Act_NORM,Act_1ST},
   [ActRenFolBrf	] = { 197,12,TabFil,NULL			,Brw_RenFolder			,{{    0,    0},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3F8}},Act_NORM,Act_1ST},
   [ActRcvFilBrfDZ	] = {1224,12,TabFil,Brw_RcvFileDZ		,NULL				,{{    0,    0},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3F8}},Act_DATA,Act_UPL},
   [ActRcvFilBrfCla	] = { 153,12,TabFil,NULL			,Brw_RcvFileClassic		,{{    0,    0},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3F8}},Act_DATA,Act_1ST},
   [ActExpBrf		] = { 410,12,TabFil,Brw_ExpandFileTree		,NULL				,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_204},
   [ActConBrf		] = { 411,12,TabFil,Brw_ContractFileTree	,NULL				,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_204},
   [ActZIPBrf		] = {1136,12,TabFil,NULL			,ZIP_CompressFileTree		,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_1ST},
   [ActReqDatBrf	] = {1047,12,TabFil,NULL			,Brw_ShowFileMetadata		,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_1ST},
   [ActChgDatBrf	] = {1048,12,TabFil,NULL			,Brw_ChgFileMetadata		,{{    0,    0},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3F8}},Act_NORM,Act_1ST},
   [ActDowBrf		] = {1123,12,TabFil,Brw_DownloadFile		,NULL				,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_DWN},
   [ActReqRemOldBrf	] = {1488,12,TabFil,NULL			,Brw_AskRemoveOldFilesBriefcase	,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_1ST},
   [ActRemOldBrf	] = {1489,12,TabFil,NULL			,Brw_RemoveOldFilesBriefcase	,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_1ST},

   // TabUsr ******************************************************************
   // Groups
   [ActReqSelAllGrp	] = { 116, 0,TabUsr,NULL			,Grp_ShowLstGrpsToChgMyGrps	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActReqSelOneGrpTyp	] = {1995, 0,TabUsr,NULL			,Grp_ShowLstGrpsToChgMyGrps	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActChgGrp		] = { 118, 0,TabUsr,NULL			,Grp_ChangeMyGrpsAndShowChanges	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActReqEdiGrp	] = { 108, 0,TabUsr,NULL			,Grp_ReqEditGroups		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActNewGrpTyp	] = { 174, 0,TabUsr,NULL			,Grp_ReceiveNewGrpTyp		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActReqRemGrpTyp	] = { 236, 0,TabUsr,NULL			,Grp_ReqRemGroupType		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActRemGrpTyp	] = { 237, 0,TabUsr,NULL			,Grp_RemoveGroupType		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActRenGrpTyp	] = { 304, 0,TabUsr,NULL			,Grp_RenameGroupType		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActChgMdtGrpTyp	] = { 303, 0,TabUsr,NULL			,Grp_ChangeMandatGrpTyp		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActChgMulGrpTyp	] = { 302, 0,TabUsr,NULL			,Grp_ChangeMultiGrpTyp		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActChgTimGrpTyp	] = {1061, 0,TabUsr,NULL			,Grp_ChangeOpenTimeGrpTyp	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActNewGrp		] = { 122, 0,TabUsr,NULL			,Grp_ReceiveNewGrp		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActReqRemGrp	] = { 107, 0,TabUsr,NULL			,Grp_ReqRemGroup		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActRemGrp		] = { 175, 0,TabUsr,NULL			,Grp_RemoveGroup		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActOpeGrp		] = { 322, 0,TabUsr,NULL			,Grp_OpenGroup			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActCloGrp		] = { 321, 0,TabUsr,NULL			,Grp_CloseGroup			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActEnaFilZonGrp	] = { 495, 0,TabUsr,NULL			,Grp_EnableFileZonesGrp		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActDisFilZonGrp	] = { 496, 0,TabUsr,NULL			,Grp_DisableFileZonesGrp	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActChgGrpTyp	] = { 167, 0,TabUsr,NULL			,Grp_ChangeGroupType		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActRenGrp		] = { 121, 0,TabUsr,NULL			,Grp_RenameGroup		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActChgGrpRoo	] = {1752, 0,TabUsr,NULL			,Grp_ChangeGroupRoom		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActChgMaxStdGrp	] = { 106, 0,TabUsr,NULL			,Grp_ChangeMaxStdsGrp		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActReqLnkAllGrp	] = {1993, 0,TabUsr,NULL			,GrpRsc_GetLinkToGroups		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActReqLnkGrpTyp	] = {1994, 0,TabUsr,NULL			,GrpRsc_GetLinkToGrpTyp		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},

   // Students
   [ActLstStd		] = { 678, 1,TabUsr,NULL			,Usr_SeeStudents		,{{    0,    0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3F8}},Act_NORM,Act_1ST},
   [ActLstStdAll	] = {  42, 1,TabUsr,NULL			,Usr_ListAllDataStds		,{{    0,    0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3F8}},Act_NORM,Act_NEW},
   [ActPrnStdPho	] = { 120, 1,TabUsr,NULL			,Usr_SeeStdClassPhotoPrn	,{{    0,    0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3F8}},Act_NORM,Act_NEW},
   [Act_DoAct_OnSevStd	] = {1754, 1,TabUsr,Usr_DoActionOnUsrs1		,Usr_DoActionOnUsrs2		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActAddClpSevStd	] = {1988, 1,TabUsr,NULL			,UsrClp_AddStdsToClipboard	,{{    0,    0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3F8}},Act_NORM,Act_1ST},
   [ActOwrClpSevStd	] = {1979, 1,TabUsr,NULL			,UsrClp_OverwriteStdsClipboard	,{{    0,    0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3F8}},Act_NORM,Act_1ST},
   [ActSeeCliStd	] = {1985, 1,TabUsr,NULL			,UsrClp_ShowClipboardStds	,{{    0,    0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3F8}},Act_NORM,Act_1ST},
   [ActRemClpStd	] = {1982, 1,TabUsr,NULL			,UsrClp_RemoveClipboardStds	,{{    0,    0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3F8}},Act_NORM,Act_1ST},
   [ActSeeRecSevStd	] = {  89, 1,TabUsr,NULL			,Rec_ListRecordsStdsShow	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActPrnRecSevStd	] = { 111, 1,TabUsr,NULL			,Rec_ListRecordsStdsPrint	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_NEW},
   [ActReqMdfOneStd	] = {1415, 1,TabUsr,NULL			,Enr_ReqEnrRemStd		,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_1ST},
   [ActReqMdfStd	] = {1419, 1,TabUsr,NULL			,Enr_AskIfEnrRemAnotherStd	,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_1ST},
   [ActReqStdPho	] = {1433, 1,TabUsr,NULL			,Pho_ReqPhotoUsr		,{{    0,    0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3E0}},Act_NORM,Act_1ST},
   [ActDetStdPho	] = {1436, 1,TabUsr,NULL			,Pho_RecOtherUsrPhotoDetFaces	,{{    0,    0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3E0}},Act_DATA,Act_1ST},
   [ActChgStdPho	] = {1439, 1,TabUsr,Pho_ChangeUsrPhoto1		,Pho_ChangeUsrPhoto2		,{{    0,    0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3E0}},Act_NORM,Act_1ST},
   [ActReqRemStdPho	] = {1575, 1,TabUsr,NULL			,Pho_ReqRemUsrPhoto		,{{    0,    0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3E0}},Act_NORM,Act_1ST},
   [ActRemStdPho	] = {1442, 1,TabUsr,NULL			,Pho_RemoveUsrPhoto		,{{    0,    0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3E0}},Act_NORM,Act_1ST},
   [ActCreStd		] = {1445, 1,TabUsr,Enr_CreateNewUsr1		,Enr_CreateNewUsr2		,{{    0,    0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3E0}},Act_NORM,Act_1ST},
   [ActUpdStd		] = {1423, 1,TabUsr,Enr_ModifyUsr1		,Enr_ModifyUsr2			,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_1ST},
   [ActCnfID_Std	] = {1569, 1,TabUsr,NULL			,ID_ConfirmOtherUsrID		,{{    0,    0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3F0}},Act_NORM,Act_1ST},
   [ActFrmAccStd	] = {1736, 1,TabUsr,NULL			,Acc_ShowFormChgOtherUsrAccount	,{{    0,    0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0}},Act_NORM,Act_1ST},
   [ActRemNicStd	] = {1739, 1,TabUsr,NULL			,Nck_RemoveOtherUsrNick		,{{    0,    0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0}},Act_NORM,Act_1ST},
   [ActChgNicStd	] = {1742, 1,TabUsr,NULL			,Nck_ChangeOtherUsrNick		,{{    0,    0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0}},Act_NORM,Act_1ST},
   [ActRemID_Std	] = {1451, 1,TabUsr,NULL			,ID_RemoveOtherUsrID		,{{    0,    0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0}},Act_NORM,Act_1ST},
   [ActChgID_Std	] = {1454, 1,TabUsr,NULL			,ID_ChangeOtherUsrID		,{{    0,    0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0}},Act_NORM,Act_1ST},
   [ActChgPwdStd	] = {1468, 1,TabUsr,Pwd_UpdateOtherUsrPwd	,Acc_ShowFormChgOtherUsrAccount	,{{    0,    0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0}},Act_NORM,Act_1ST},
   [ActRemMaiStd	] = {1479, 1,TabUsr,NULL			,Mai_RemoveOtherUsrEmail	,{{    0,    0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0}},Act_NORM,Act_1ST},
   [ActChgMaiStd	] = {1482, 1,TabUsr,NULL			,Mai_ChangeOtherUsrEmail	,{{    0,    0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0}},Act_NORM,Act_1ST},
   [ActRemStdCrs	] = {1462, 1,TabUsr,Enr_RemUsrFromCrs1		,Enr_RemUsrFromCrs2		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3F8}},Act_NORM,Act_1ST},
   [ActReqRemAllStdCrs	] = {  88, 1,TabUsr,NULL			,Enr_AskRemAllStdsThisCrs	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActRemAllStdCrs	] = {  87, 1,TabUsr,NULL			,Enr_RemAllStdsThisCrs		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActReqAccEnrStd	] = {1456, 1,TabUsr,NULL			,Enr_ReqAcceptEnrolmentInCrs	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,0x3C8}},Act_NORM,Act_1ST},
   [ActAccEnrStd	] = {1458, 1,TabUsr,NULL			,Enr_AcceptEnrolMeInCrs	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,0x3C8}},Act_NORM,Act_1ST},
   [ActRemMe_Std	] = {1460, 1,TabUsr,NULL			,Enr_ReqRemMeFromCrs		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,0x3C8}},Act_NORM,Act_1ST},
   [ActReqFolSevStd	] = {1756, 1,TabUsr,NULL			,Fol_ReqFollowStds		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x3F8}},Act_NORM,Act_1ST},
   [ActReqUnfSevStd	] = {1758, 1,TabUsr,NULL			,Fol_ReqUnfollowStds		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x3F8}},Act_NORM,Act_1ST},
   [ActFolSevStd	] = {1760, 1,TabUsr,NULL			,Fol_FollowUsrs			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x3F8}},Act_NORM,Act_1ST},
   [ActUnfSevStd	] = {1762, 1,TabUsr,NULL			,Fol_UnfollowUsrs		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x3F8}},Act_NORM,Act_1ST},
   [ActSeeRecOneStd	] = {1174, 1,TabUsr,NULL			,Rec_GetUsrAndShowRecOneStdCrs	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActReqEnrSevStd	] = {1426, 1,TabUsr,NULL			,Enr_ReqAdminStds		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3E0}},Act_NORM,Act_1ST},
   [ActRcvFrmEnrSevStd	] = {1428, 1,TabUsr,NULL			,Enr_ReceiveAdminStds		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3E0}},Act_NORM,Act_1ST},
   [ActRcvRecOthUsr	] = { 300, 1,TabUsr,NULL			,Rec_UpdateAndShowOtherCrsRecord,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActEdiRecFie	] = { 292, 1,TabUsr,NULL			,Rec_ReqEditRecordFields	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActNewFie		] = { 293, 1,TabUsr,NULL			,Rec_ReceiveField		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActReqRemFie	] = { 294, 1,TabUsr,NULL			,Rec_ReqRemField		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActRemFie		] = { 295, 1,TabUsr,NULL			,Rec_RemoveField		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActRenFie		] = { 296, 1,TabUsr,NULL			,Rec_RenameField		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActChgRowFie	] = { 305, 1,TabUsr,NULL			,Rec_ChangeLinesField		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActChgVisFie	] = { 297, 1,TabUsr,NULL			,Rec_ChangeVisibilityField	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActRcvRecCrs	] = { 301, 1,TabUsr,NULL			,Rec_UpdateAndShowMyCrsRecord	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x208}},Act_NORM,Act_1ST},

   // Teachers
   [ActLstTch		] = { 679, 2,TabUsr,NULL			,Usr_SeeTeachers		,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_1ST},
   [ActLstTchAll	] = { 578, 2,TabUsr,NULL			,Usr_ListAllDataTchs 		,{{    0,    0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3F8}},Act_NORM,Act_NEW},
   [ActPrnTchPho	] = { 443, 2,TabUsr,NULL			,Usr_SeeTchClassPhotoPrn	,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_NEW},
   [Act_DoAct_OnSevTch	] = {1755, 2,TabUsr,Usr_DoActionOnUsrs1		,Usr_DoActionOnUsrs2		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3F8}},Act_NORM,Act_1ST},
   [ActAddClpSevTch	] = {1989, 2,TabUsr,NULL			,UsrClp_AddTchsToClipboard	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3F8}},Act_NORM,Act_1ST},
   [ActOwrClpSevTch	] = {1980, 2,TabUsr,NULL			,UsrClp_OverwriteTchsClipboard	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3F8}},Act_NORM,Act_1ST},
   [ActSeeCliTch	] = {1986, 2,TabUsr,NULL			,UsrClp_ShowClipboardTchs	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3F8}},Act_NORM,Act_1ST},
   [ActRemClpTch	] = {1983, 2,TabUsr,NULL			,UsrClp_RemoveClipboardTchs	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3F8}},Act_NORM,Act_1ST},
   [ActSeeRecSevTch	] = {  22, 2,TabUsr,NULL			,Rec_ListRecordsTchsShow	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3F8}},Act_NORM,Act_1ST},
   [ActPrnRecSevTch	] = { 127, 2,TabUsr,NULL			,Rec_ListRecordsTchsPrint	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3F8}},Act_NORM,Act_NEW},
   [ActReqMdfOneTch	] = {1416, 2,TabUsr,NULL			,Enr_ReqEnrRemTch		,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_1ST},
   [ActReqMdfNET	] = {1644, 2,TabUsr,NULL			,Enr_AskIfEnrRemAnotherTch	,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_1ST},
   [ActReqMdfTch	] = {1420, 2,TabUsr,NULL			,Enr_AskIfEnrRemAnotherTch	,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_1ST},
   [ActReqTchPho	] = {1434, 2,TabUsr,NULL			,Pho_ReqPhotoUsr		,{{    0,    0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3E0}},Act_NORM,Act_1ST},
   [ActDetTchPho	] = {1437, 2,TabUsr,NULL			,Pho_RecOtherUsrPhotoDetFaces	,{{    0,    0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3E0}},Act_DATA,Act_1ST},
   [ActChgTchPho	] = {1440, 2,TabUsr,Pho_ChangeUsrPhoto1		,Pho_ChangeUsrPhoto2		,{{    0,    0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3E0}},Act_NORM,Act_1ST},
   [ActReqRemTchPho	] = {1576, 2,TabUsr,NULL			,Pho_ReqRemUsrPhoto		,{{    0,    0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3E0}},Act_NORM,Act_1ST},
   [ActRemTchPho	] = {1443, 2,TabUsr,NULL			,Pho_RemoveUsrPhoto		,{{    0,    0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3E0}},Act_NORM,Act_1ST},
   [ActCreNET		] = {1645, 2,TabUsr,Enr_CreateNewUsr1		,Enr_CreateNewUsr2		,{{    0,    0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3E0}},Act_NORM,Act_1ST},
   [ActCreTch		] = {1446, 2,TabUsr,Enr_CreateNewUsr1		,Enr_CreateNewUsr2		,{{    0,    0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3E0}},Act_NORM,Act_1ST},
   [ActUpdNET		] = {1646, 2,TabUsr,Enr_ModifyUsr1		,Enr_ModifyUsr2			,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_1ST},
   [ActUpdTch		] = {1424, 2,TabUsr,Enr_ModifyUsr1		,Enr_ModifyUsr2			,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_1ST},
   [ActCnfID_Tch	] = {1570, 2,TabUsr,NULL			,ID_ConfirmOtherUsrID		,{{    0,    0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0}},Act_NORM,Act_1ST},
   [ActFrmAccTch	] = {1737, 2,TabUsr,NULL			,Acc_ShowFormChgOtherUsrAccount	,{{    0,    0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0}},Act_NORM,Act_1ST},
   [ActRemNicTch	] = {1740, 2,TabUsr,NULL			,Nck_RemoveOtherUsrNick		,{{    0,    0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0}},Act_NORM,Act_1ST},
   [ActChgNicTch	] = {1743, 2,TabUsr,NULL			,Nck_ChangeOtherUsrNick		,{{    0,    0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0}},Act_NORM,Act_1ST},
   [ActRemID_Tch	] = {1452, 2,TabUsr,NULL			,ID_RemoveOtherUsrID		,{{    0,    0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0}},Act_NORM,Act_1ST},
   [ActChgID_Tch	] = {1455, 2,TabUsr,NULL			,ID_ChangeOtherUsrID		,{{    0,    0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0}},Act_NORM,Act_1ST},
   [ActChgPwdTch	] = {1469, 2,TabUsr,Pwd_UpdateOtherUsrPwd	,Acc_ShowFormChgOtherUsrAccount	,{{    0,    0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0}},Act_NORM,Act_1ST},
   [ActRemMaiTch	] = {1480, 2,TabUsr,NULL			,Mai_RemoveOtherUsrEmail	,{{    0,    0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0}},Act_NORM,Act_1ST},
   [ActChgMaiTch	] = {1483, 2,TabUsr,NULL			,Mai_ChangeOtherUsrEmail	,{{    0,    0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0}},Act_NORM,Act_1ST},
   [ActRemNETCrs	] = {1647, 2,TabUsr,Enr_RemUsrFromCrs1		,Enr_RemUsrFromCrs2		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3F0}},Act_NORM,Act_1ST},
   [ActRemTchCrs	] = {1463, 2,TabUsr,Enr_RemUsrFromCrs1		,Enr_RemUsrFromCrs2		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3E0}},Act_NORM,Act_1ST},
   [ActReqAccEnrNET	] = {1639, 2,TabUsr,NULL			,Enr_ReqAcceptEnrolmentInCrs	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,0x3D0}},Act_NORM,Act_1ST},
   [ActReqAccEnrTch	] = {1457, 2,TabUsr,NULL			,Enr_ReqAcceptEnrolmentInCrs	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,0x3E0}},Act_NORM,Act_1ST},
   [ActAccEnrNET	] = {1640, 2,TabUsr,NULL			,Enr_AcceptEnrolMeInCrs	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,0x3D0}},Act_NORM,Act_1ST},
   [ActAccEnrTch	] = {1459, 2,TabUsr,NULL			,Enr_AcceptEnrolMeInCrs	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,0x3E0}},Act_NORM,Act_1ST},
   [ActRemMe_NET	] = {1641, 2,TabUsr,NULL			,Enr_ReqRemMeFromCrs		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,0x3D0}},Act_NORM,Act_1ST},
   [ActRemMe_Tch	] = {1461, 2,TabUsr,NULL			,Enr_ReqRemMeFromCrs		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,0x3E0}},Act_NORM,Act_1ST},
   [ActReqFolSevTch	] = {1757, 2,TabUsr,NULL			,Fol_ReqFollowTchs		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x3F8}},Act_NORM,Act_1ST},
   [ActReqUnfSevTch	] = {1759, 2,TabUsr,NULL			,Fol_ReqUnfollowTchs		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x3F8}},Act_NORM,Act_1ST},
   [ActFolSevTch	] = {1761, 2,TabUsr,NULL			,Fol_FollowUsrs			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x3F8}},Act_NORM,Act_1ST},
   [ActUnfSevTch	] = {1763, 2,TabUsr,NULL			,Fol_UnfollowUsrs		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x3F8}},Act_NORM,Act_1ST},
   [ActSeeRecOneTch	] = {1175, 2,TabUsr,NULL			,Rec_GetUsrAndShowRecOneTchCrs	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3F8}},Act_NORM,Act_1ST},
   [ActReqEnrSevNET	] = {1642, 2,TabUsr,NULL			,Enr_ReqAdminNonEditingTchs	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3C0}},Act_NORM,Act_1ST},
   [ActRcvFrmEnrSevNET	] = {1643, 2,TabUsr,NULL			,Enr_ReceiveAdminNonEditTchs	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3C0}},Act_NORM,Act_1ST},
   [ActReqEnrSevTch	] = {1427, 2,TabUsr,NULL			,Enr_ReqAdminTchs		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3C0}},Act_NORM,Act_1ST},
   [ActRcvFrmEnrSevTch	] = {1429, 2,TabUsr,NULL			,Enr_ReceiveAdminTchs		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C0,0x3C0}},Act_NORM,Act_1ST},
   [ActFrmLogInUsrAgd	] = {1614, 2,TabUsr,NULL			,Agd_PutFormLogInToShowUsrAgenda,{{    0,    0},{0x001,0x001},{0x001,0x001},{0x001,0x001},{0x001,0x001},{0x001,0x001},{0x001,    0}},Act_NORM,Act_1ST},
   [ActLogInUsrAgd	] = {1615, 2,TabUsr,NULL			,Agd_ShowOtherAgendaAfterLogIn	,{{    0,    0},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3F8}},Act_NORM,Act_1ST},
   [ActLogInUsrAgdLan	] = {1616, 2,TabUsr,NULL			,Agd_ShowOtherAgendaAfterLogIn	,{{    0,    0},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3F8}},Act_NORM,Act_1ST},

   // Others
   [ActLstOth		] = {1186, 3,TabUsr,NULL			,Usr_ListDataAdms		,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_1ST},
   [ActNewAdmIns	] = {1337, 3,TabUsr,NULL			,Adm_AddAdmToIns		,{{    0,    0},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200}},Act_NORM,Act_1ST},
   [ActRemAdmIns	] = {1338, 3,TabUsr,NULL			,Adm_RemAdmIns			,{{    0,    0},{0x300,0x300},{0x300,0x300},{0x300,0x300},{0x300,0x300},{0x300,0x300},{0x300,0x300}},Act_NORM,Act_1ST},
   [ActNewAdmCtr	] = {1339, 3,TabUsr,NULL			,Adm_AddAdmToCtr		,{{    0,    0},{0x300,0x300},{0x300,0x300},{0x300,0x300},{0x300,0x300},{0x300,0x300},{0x300,0x300}},Act_NORM,Act_1ST},
   [ActRemAdmCtr	] = {1340, 3,TabUsr,NULL			,Adm_RemAdmCtr			,{{    0,    0},{0x380,0x380},{0x380,0x380},{0x380,0x380},{0x380,0x380},{0x380,0x380},{0x380,0x380}},Act_NORM,Act_1ST},
   [ActNewAdmDeg	] = { 586, 3,TabUsr,NULL			,Adm_AddAdmToDeg		,{{    0,    0},{0x380,0x380},{0x380,0x380},{0x380,0x380},{0x380,0x380},{0x380,0x380},{0x380,0x380}},Act_NORM,Act_1ST},
   [ActRemAdmDeg	] = { 584, 3,TabUsr,NULL			,Adm_RemAdmDeg			,{{    0,    0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0}},Act_NORM,Act_1ST},
   [ActLstGst		] = { 587, 3,TabUsr,NULL			,Usr_SeeGuests			,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_1ST},
   [ActLstGstAll	] = {1189, 3,TabUsr,NULL			,Usr_ListAllDataGsts		,{{    0,    0},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200}},Act_NORM,Act_NEW},
   [ActPrnGstPho	] = {1190, 3,TabUsr,NULL			,Usr_SeeGstClassPhotoPrn	,{{    0,    0},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200}},Act_NORM,Act_NEW},
   [Act_DoAct_OnSevGst	] = {1753, 3,TabUsr,Usr_DoActionOnUsrs1		,Usr_DoActionOnUsrs2		,{{    0,    0},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200}},Act_NORM,Act_1ST},
   [ActAddClpSevGst	] = {1987, 3,TabUsr,NULL			,UsrClp_AddGstsToClipboard	,{{    0,    0},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200}},Act_NORM,Act_1ST},
   [ActOwrClpSevGst	] = {1978, 3,TabUsr,NULL			,UsrClp_OverwriteGstsClipboard	,{{    0,    0},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200}},Act_NORM,Act_1ST},
   [ActSeeCliGst	] = {1984, 3,TabUsr,NULL			,UsrClp_ShowClipboardGsts	,{{    0,    0},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200}},Act_NORM,Act_1ST},
   [ActRemClpGst	] = {1981, 3,TabUsr,NULL			,UsrClp_RemoveClipboardGsts	,{{    0,    0},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200}},Act_NORM,Act_1ST},
   [ActSeeRecSevGst	] = {1187, 3,TabUsr,NULL			,Rec_ListRecordsGstsShow	,{{    0,    0},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200}},Act_NORM,Act_1ST},
   [ActPrnRecSevGst	] = {1188, 3,TabUsr,NULL			,Rec_ListRecordsGstsPrint	,{{    0,    0},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200}},Act_NORM,Act_NEW},
   [ActReqMdfOneOth	] = {1414, 3,TabUsr,NULL			,Enr_ReqEnrRemOth		,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_1ST},
   [ActReqMdfOth	] = {1418, 3,TabUsr,NULL			,Enr_AskIfEnrRemAnotherOth	,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_1ST},
   [ActReqOthPho	] = {1432, 3,TabUsr,NULL			,Pho_ReqPhotoUsr		,{{    0,    0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3E0}},Act_NORM,Act_1ST},
   [ActDetOthPho	] = {1435, 3,TabUsr,NULL			,Pho_RecOtherUsrPhotoDetFaces	,{{    0,    0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3E0}},Act_DATA,Act_1ST},
   [ActChgOthPho	] = {1438, 3,TabUsr,Pho_ChangeUsrPhoto1		,Pho_ChangeUsrPhoto2		,{{    0,    0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3E0}},Act_NORM,Act_1ST},
   [ActReqRemOthPho	] = {1574, 3,TabUsr,NULL			,Pho_ReqRemUsrPhoto		,{{    0,    0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3E0}},Act_NORM,Act_1ST},
   [ActRemOthPho	] = {1441, 3,TabUsr,NULL			,Pho_RemoveUsrPhoto		,{{    0,    0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3E0}},Act_NORM,Act_1ST},
   [ActCreOth		] = {1444, 3,TabUsr,Enr_CreateNewUsr1		,Enr_CreateNewUsr2		,{{    0,    0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3E0}},Act_NORM,Act_1ST},
   [ActUpdOth		] = {1422, 3,TabUsr,Enr_ModifyUsr1		,Enr_ModifyUsr2			,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_1ST},
   [ActCnfID_Oth	] = {1568, 3,TabUsr,NULL			,ID_ConfirmOtherUsrID		,{{    0,    0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0}},Act_NORM,Act_1ST},
   [ActFrmAccOth	] = {1735, 3,TabUsr,NULL			,Acc_ShowFormChgOtherUsrAccount	,{{    0,    0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0}},Act_NORM,Act_1ST},
   [ActRemNicOth	] = {1738, 3,TabUsr,NULL			,Nck_RemoveOtherUsrNick		,{{    0,    0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0}},Act_NORM,Act_1ST},
   [ActChgNicOth	] = {1741, 3,TabUsr,NULL			,Nck_ChangeOtherUsrNick		,{{    0,    0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0}},Act_NORM,Act_1ST},
   [ActRemID_Oth	] = {1450, 3,TabUsr,NULL			,ID_RemoveOtherUsrID		,{{    0,    0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0}},Act_NORM,Act_1ST},
   [ActChgID_Oth	] = {1453, 3,TabUsr,NULL			,ID_ChangeOtherUsrID		,{{    0,    0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0}},Act_NORM,Act_1ST},
   [ActChgPwdOth	] = {1467, 3,TabUsr,Pwd_UpdateOtherUsrPwd	,Acc_ShowFormChgOtherUsrAccount	,{{    0,    0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0}},Act_NORM,Act_1ST},
   [ActRemMaiOth	] = {1478, 3,TabUsr,NULL			,Mai_RemoveOtherUsrEmail	,{{    0,    0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0}},Act_NORM,Act_1ST},
   [ActChgMaiOth	] = {1481, 3,TabUsr,NULL			,Mai_ChangeOtherUsrEmail	,{{    0,    0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0}},Act_NORM,Act_1ST},
   [ActRemUsrGbl	] = {  62, 3,TabUsr,NULL			,Acc_GetUsrCodAndRemUsrGbl	,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_1ST},
   [ActReqRemOldUsr	] = { 590, 3,TabUsr,NULL			,Enr_AskRemoveOldUsrs		,{{    0,    0},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200}},Act_NORM,Act_1ST},
   [ActRemOldUsr	] = { 773, 3,TabUsr,NULL			,Enr_RemoveOldUsrs		,{{    0,    0},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200}},Act_NORM,Act_1ST},
   [ActLstDupUsr	] = {1578, 3,TabUsr,NULL			,Dup_ListDuplicateUsrs		,{{    0,    0},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200}},Act_NORM,Act_1ST},
   [ActLstSimUsr	] = {1579, 3,TabUsr,NULL			,Dup_GetUsrCodAndListSimilarUsrs,{{    0,    0},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200}},Act_NORM,Act_1ST},
   [ActRemDupUsr	] = {1580, 3,TabUsr,NULL			,Dup_RemoveUsrFromListDupUsrs	,{{    0,    0},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200}},Act_NORM,Act_1ST},

   // Attendance
   [ActSeeAllAtt	] = { 861, 4,TabUsr,NULL			,Att_SeeEvents			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActReqLstUsrAtt	] = {1073, 4,TabUsr,NULL			,Att_ReqListUsrsAttendanceCrs	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x230}},Act_NORM,Act_1ST},
   [ActSeeLstMyAtt	] = {1473, 4,TabUsr,NULL			,Att_ListMyAttendanceCrs	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,0x008}},Act_NORM,Act_1ST},
   [ActPrnLstMyAtt	] = {1474, 4,TabUsr,NULL			,Att_PrintMyAttendanceCrs	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,0x008}},Act_NORM,Act_NEW},
   [ActSeeLstUsrAtt	] = {1074, 4,TabUsr,NULL			,Att_ListUsrsAttendanceCrs	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x230}},Act_NORM,Act_1ST},
   [ActPrnLstUsrAtt	] = {1075, 4,TabUsr,NULL			,Att_PrintUsrsAttendanceCrs	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x230}},Act_NORM,Act_NEW},
   [ActFrmNewAtt	] = {1063, 4,TabUsr,NULL			,Att_ReqCreatOrEditEvent	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActEdiOneAtt	] = {1064, 4,TabUsr,NULL			,Att_ReqCreatOrEditEvent	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActNewAtt		] = {1065, 4,TabUsr,NULL			,Att_ReceiveEvent		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActChgAtt		] = {1066, 4,TabUsr,NULL			,Att_ReceiveEvent		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActReqRemAtt	] = {1067, 4,TabUsr,NULL			,Att_AskRemEvent		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActRemAtt		] = {1068, 4,TabUsr,NULL			,Att_GetAndRemEvent		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActHidAtt		] = {1069, 4,TabUsr,NULL			,Att_HideEvent			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActUnhAtt		] = {1070, 4,TabUsr,NULL			,Att_UnhideEvent		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActSeeOneAtt	] = {1071, 4,TabUsr,NULL			,Att_SeeOneEvent		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x238}},Act_NORM,Act_1ST},
   [ActReqLnkAtt	] = {1938, 4,TabUsr,NULL			,AttRsc_GetLinkToEvent		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActRecAttStd	] = {1072, 4,TabUsr,NULL			,Att_RegisterStudentsInEvent	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActRecAttMe		] = {1076, 4,TabUsr,NULL			,Att_RegisterMeAsStdInEvent	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,0x008}},Act_NORM,Act_1ST},

   // Sign up
   [ActReqSignUp	] = {1054, 5,TabUsr,NULL			,Enr_ReqSignUpInCrs		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x006,    0}},Act_NORM,Act_1ST},
   [ActSignUp		] = {1056, 5,TabUsr,NULL			,Enr_SignUpInCrs		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x006,    0}},Act_NORM,Act_1ST},

   // Requests
   [ActSeeSignUpReq	] = {1057, 6,TabUsr,NULL			,Enr_ShowEnrolmentRequests	,{{    0,    0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3E0}},Act_NORM,Act_1ST},
   [ActUpdSignUpReq	] = {1522, 6,TabUsr,NULL			,Enr_UpdateEnrolmentRequests	,{{    0,    0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3E0}},Act_NORM,Act_1ST},
   [ActReqRejSignUp	] = {1058, 6,TabUsr,NULL			,Enr_AskIfRejectSignUp		,{{    0,    0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3E0}},Act_NORM,Act_1ST},
   [ActRejSignUp	] = {1059, 6,TabUsr,NULL			,Enr_RejectSignUp		,{{    0,    0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3E0}},Act_NORM,Act_1ST},

   // Connected
   [ActLstCon		] = { 995, 7,TabUsr,NULL			,Con_ShowConnectedUsrs		,{{    0,    0},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3F8}},Act_NORM,Act_1ST},

   // TabMsg ******************************************************************
   // Announcements
   [ActSeeAnn		] = {1235, 0,TabMsg,NULL			,Ann_ShowAllAnnouncements	,{{    0,    0},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3F8}},Act_NORM,Act_1ST},
   [ActWriAnn		] = {1237, 0,TabMsg,NULL			,Ann_ShowFormAnnouncement	,{{    0,    0},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200}},Act_NORM,Act_1ST},
   [ActNewAnn		] = {1238, 0,TabMsg,NULL			,Ann_ReceiveAnnouncement	,{{    0,    0},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200}},Act_NORM,Act_1ST},
   [ActHidAnn		] = {1470, 0,TabMsg,Ann_HideAnnouncement	,Ann_ShowAllAnnouncements	,{{    0,    0},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200}},Act_NORM,Act_1ST},
   [ActUnhAnn		] = {1471, 0,TabMsg,Ann_UnhideAnnouncement	,Ann_ShowAllAnnouncements	,{{    0,    0},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200}},Act_NORM,Act_1ST},
   [ActRemAnn		] = {1236, 0,TabMsg,NULL			,Ann_RemoveAnnouncement		,{{    0,    0},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200}},Act_NORM,Act_1ST},

   // Notices
   [ActSeeAllNot	] = { 762, 1,TabMsg,NULL			,Not_ListFullNotices		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C7,0x3F8}},Act_NORM,Act_1ST},
   [ActSeeOneNot	] = {1164, 1,TabMsg,Not_GetHighlightNotCod	,Not_ListFullNotices		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x3C7,0x3F8}},Act_NORM,Act_1ST},
   [ActWriNot		] = {  59, 1,TabMsg,NULL			,Not_ShowFormNotice		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActNewNot		] = {  60, 1,TabMsg,Not_ReceiveNotice		,Not_ListFullNotices		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActHidNot		] = { 763, 1,TabMsg,Not_HideNotice		,Not_ListFullNotices		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActUnhNot		] = { 764, 1,TabMsg,Not_UnhideNotice		,Not_ListFullNotices		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActReqRemNot	] = {1472, 1,TabMsg,NULL			,Not_ReqRemNotice		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActRemNot		] = {  73, 1,TabMsg,Not_RemoveNotice		,Not_ListNoticesAfterRemoval	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},

   // Forums
   [ActSeeFor		] = {  95, 2,TabMsg,NULL			,For_ShowForumTheads		,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_1ST},
   [ActSeeForCrsUsr	] = { 345, 2,TabMsg,NULL			,For_ShowForumTheads		,{{    0,    0},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3F8}},Act_NORM,Act_1ST},
   [ActSeeForCrsTch	] = { 431, 2,TabMsg,NULL			,For_ShowForumTheads		,{{    0,    0},{0x204,0x204},{0x204,0x204},{0x204,0x204},{0x204,0x204},{0x204,0x204},{0x204,0x230}},Act_NORM,Act_1ST},
   [ActSeeForDegUsr	] = { 241, 2,TabMsg,NULL			,For_ShowForumTheads		,{{    0,    0},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3F8}},Act_NORM,Act_1ST},
   [ActSeeForDegTch	] = { 243, 2,TabMsg,NULL			,For_ShowForumTheads		,{{    0,    0},{0x204,0x204},{0x204,0x204},{0x204,0x204},{0x204,0x204},{0x204,0x204},{0x3C4,0x3F0}},Act_NORM,Act_1ST},
   [ActSeeForCtrUsr	] = { 901, 2,TabMsg,NULL			,For_ShowForumTheads		,{{    0,    0},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3F8}},Act_NORM,Act_1ST},
   [ActSeeForCtrTch	] = { 430, 2,TabMsg,NULL			,For_ShowForumTheads		,{{    0,    0},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3F0}},Act_NORM,Act_1ST},
   [ActSeeForInsUsr	] = { 725, 2,TabMsg,NULL			,For_ShowForumTheads		,{{    0,    0},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3F8}},Act_NORM,Act_1ST},
   [ActSeeForInsTch	] = { 724, 2,TabMsg,NULL			,For_ShowForumTheads		,{{    0,    0},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3F0}},Act_NORM,Act_1ST},
   [ActSeeForGenUsr	] = { 726, 2,TabMsg,NULL			,For_ShowForumTheads		,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_1ST},
   [ActSeeForGenTch	] = { 723, 2,TabMsg,NULL			,For_ShowForumTheads		,{{    0,    0},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3F0}},Act_NORM,Act_1ST},
   [ActSeeForSWAUsr	] = { 242, 2,TabMsg,NULL			,For_ShowForumTheads		,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_1ST},
   [ActSeeForSWATch	] = { 245, 2,TabMsg,NULL			,For_ShowForumTheads		,{{    0,    0},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3F0}},Act_NORM,Act_1ST},
   [ActSeePstForCrsUsr	] = { 346, 2,TabMsg,NULL			,For_ShowThreadPosts		,{{    0,    0},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3F8}},Act_NORM,Act_1ST},
   [ActSeePstForCrsTch	] = { 347, 2,TabMsg,NULL			,For_ShowThreadPosts		,{{    0,    0},{0x204,0x204},{0x204,0x204},{0x204,0x204},{0x204,0x204},{0x204,0x204},{0x204,0x230}},Act_NORM,Act_1ST},
   [ActSeePstForDegUsr	] = { 255, 2,TabMsg,NULL			,For_ShowThreadPosts		,{{    0,    0},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3F8}},Act_NORM,Act_1ST},
   [ActSeePstForDegTch	] = { 291, 2,TabMsg,NULL			,For_ShowThreadPosts		,{{    0,    0},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3F0}},Act_NORM,Act_1ST},
   [ActSeePstForCtrUsr	] = { 348, 2,TabMsg,NULL			,For_ShowThreadPosts		,{{    0,    0},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3F8}},Act_NORM,Act_1ST},
   [ActSeePstForCtrTch	] = { 902, 2,TabMsg,NULL			,For_ShowThreadPosts		,{{    0,    0},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3F0}},Act_NORM,Act_1ST},
   [ActSeePstForInsUsr	] = { 730, 2,TabMsg,NULL			,For_ShowThreadPosts		,{{    0,    0},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3F8}},Act_NORM,Act_1ST},
   [ActSeePstForInsTch	] = { 746, 2,TabMsg,NULL			,For_ShowThreadPosts		,{{    0,    0},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3F0}},Act_NORM,Act_1ST},
   [ActSeePstForGenUsr	] = { 727, 2,TabMsg,NULL			,For_ShowThreadPosts		,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_1ST},
   [ActSeePstForGenTch	] = { 731, 2,TabMsg,NULL			,For_ShowThreadPosts		,{{    0,    0},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3F0}},Act_NORM,Act_1ST},
   [ActSeePstForSWAUsr	] = { 244, 2,TabMsg,NULL			,For_ShowThreadPosts		,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_1ST},
   [ActSeePstForSWATch	] = { 246, 2,TabMsg,NULL			,For_ShowThreadPosts		,{{    0,    0},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3F0}},Act_NORM,Act_1ST},
   [ActRcvThrForCrsUsr	] = { 350, 2,TabMsg,NULL			,For_ReceiveForumPost		,{{    0,    0},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3F8}},Act_DATA,Act_1ST},
   [ActRcvThrForCrsTch	] = { 754, 2,TabMsg,NULL			,For_ReceiveForumPost		,{{    0,    0},{0x204,0x204},{0x204,0x204},{0x204,0x204},{0x204,0x204},{0x204,0x204},{0x204,0x230}},Act_DATA,Act_1ST},
   [ActRcvThrForDegUsr	] = { 252, 2,TabMsg,NULL			,For_ReceiveForumPost		,{{    0,    0},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3F8}},Act_DATA,Act_1ST},
   [ActRcvThrForDegTch	] = { 247, 2,TabMsg,NULL			,For_ReceiveForumPost		,{{    0,    0},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3F0}},Act_DATA,Act_1ST},
   [ActRcvThrForCtrUsr	] = { 903, 2,TabMsg,NULL			,For_ReceiveForumPost		,{{    0,    0},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3F8}},Act_DATA,Act_1ST},
   [ActRcvThrForCtrTch	] = { 904, 2,TabMsg,NULL			,For_ReceiveForumPost		,{{    0,    0},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3F0}},Act_DATA,Act_1ST},
   [ActRcvThrForInsUsr	] = { 737, 2,TabMsg,NULL			,For_ReceiveForumPost		,{{    0,    0},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3F8}},Act_DATA,Act_1ST},
   [ActRcvThrForInsTch	] = { 769, 2,TabMsg,NULL			,For_ReceiveForumPost		,{{    0,    0},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3F0}},Act_DATA,Act_1ST},
   [ActRcvThrForGenUsr	] = { 736, 2,TabMsg,NULL			,For_ReceiveForumPost		,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_DATA,Act_1ST},
   [ActRcvThrForGenTch	] = { 765, 2,TabMsg,NULL			,For_ReceiveForumPost		,{{    0,    0},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3F0}},Act_DATA,Act_1ST},
   [ActRcvThrForSWAUsr	] = { 258, 2,TabMsg,NULL			,For_ReceiveForumPost		,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_DATA,Act_1ST},
   [ActRcvThrForSWATch	] = { 259, 2,TabMsg,NULL			,For_ReceiveForumPost		,{{    0,    0},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3F0}},Act_DATA,Act_1ST},
   [ActRcvRepForCrsUsr	] = { 599, 2,TabMsg,NULL			,For_ReceiveForumPost		,{{    0,    0},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3F8}},Act_DATA,Act_1ST},
   [ActRcvRepForCrsTch	] = { 755, 2,TabMsg,NULL			,For_ReceiveForumPost		,{{    0,    0},{0x204,0x204},{0x204,0x204},{0x204,0x204},{0x204,0x204},{0x204,0x204},{0x204,0x230}},Act_DATA,Act_1ST},
   [ActRcvRepForDegUsr	] = { 606, 2,TabMsg,NULL			,For_ReceiveForumPost		,{{    0,    0},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3F8}},Act_DATA,Act_1ST},
   [ActRcvRepForDegTch	] = { 617, 2,TabMsg,NULL			,For_ReceiveForumPost		,{{    0,    0},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3F0}},Act_DATA,Act_1ST},
   [ActRcvRepForCtrUsr	] = { 905, 2,TabMsg,NULL			,For_ReceiveForumPost		,{{    0,    0},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3F8}},Act_DATA,Act_1ST},
   [ActRcvRepForCtrTch	] = { 906, 2,TabMsg,NULL			,For_ReceiveForumPost		,{{    0,    0},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3F0}},Act_DATA,Act_1ST},
   [ActRcvRepForInsUsr	] = { 740, 2,TabMsg,NULL			,For_ReceiveForumPost		,{{    0,    0},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3F8}},Act_DATA,Act_1ST},
   [ActRcvRepForInsTch	] = { 770, 2,TabMsg,NULL			,For_ReceiveForumPost		,{{    0,    0},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3F0}},Act_DATA,Act_1ST},
   [ActRcvRepForGenUsr	] = { 747, 2,TabMsg,NULL			,For_ReceiveForumPost		,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_DATA,Act_1ST},
   [ActRcvRepForGenTch	] = { 816, 2,TabMsg,NULL			,For_ReceiveForumPost		,{{    0,    0},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3F0}},Act_DATA,Act_1ST},
   [ActRcvRepForSWAUsr	] = { 603, 2,TabMsg,NULL			,For_ReceiveForumPost		,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_DATA,Act_1ST},
   [ActRcvRepForSWATch	] = { 622, 2,TabMsg,NULL			,For_ReceiveForumPost		,{{    0,    0},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3F0}},Act_DATA,Act_1ST},
   [ActReqDelThrCrsUsr	] = { 867, 2,TabMsg,NULL			,For_ReqRemThread		,{{    0,    0},{0x204,0x204},{0x204,0x204},{0x204,0x204},{0x204,0x204},{0x204,0x204},{0x204,0x220}},Act_NORM,Act_1ST},
   [ActReqDelThrCrsTch	] = { 869, 2,TabMsg,NULL			,For_ReqRemThread		,{{    0,    0},{0x204,0x204},{0x204,0x204},{0x204,0x204},{0x204,0x204},{0x204,0x204},{0x204,0x220}},Act_NORM,Act_1ST},
   [ActReqDelThrDegUsr	] = { 907, 2,TabMsg,NULL			,For_ReqRemThread		,{{    0,    0},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C0}},Act_NORM,Act_1ST},
   [ActReqDelThrDegTch	] = { 908, 2,TabMsg,NULL			,For_ReqRemThread		,{{    0,    0},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C0}},Act_NORM,Act_1ST},
   [ActReqDelThrCtrUsr	] = { 909, 2,TabMsg,NULL			,For_ReqRemThread		,{{    0,    0},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C0}},Act_NORM,Act_1ST},
   [ActReqDelThrCtrTch	] = { 910, 2,TabMsg,NULL			,For_ReqRemThread		,{{    0,    0},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C0}},Act_NORM,Act_1ST},
   [ActReqDelThrInsUsr	] = { 911, 2,TabMsg,NULL			,For_ReqRemThread		,{{    0,    0},{0x204,0x204},{0x204,0x204},{0x204,0x204},{0x204,0x204},{0x204,0x204},{0x204,0x200}},Act_NORM,Act_1ST},
   [ActReqDelThrInsTch	] = { 912, 2,TabMsg,NULL			,For_ReqRemThread		,{{    0,    0},{0x204,0x204},{0x204,0x204},{0x204,0x204},{0x204,0x204},{0x204,0x204},{0x204,0x200}},Act_NORM,Act_1ST},
   [ActReqDelThrGenUsr	] = { 913, 2,TabMsg,NULL			,For_ReqRemThread		,{{    0,    0},{0x204,0x204},{0x204,0x204},{0x204,0x204},{0x204,0x204},{0x204,0x204},{0x204,0x200}},Act_NORM,Act_1ST},
   [ActReqDelThrGenTch	] = { 914, 2,TabMsg,NULL			,For_ReqRemThread		,{{    0,    0},{0x204,0x204},{0x204,0x204},{0x204,0x204},{0x204,0x204},{0x204,0x204},{0x204,0x200}},Act_NORM,Act_1ST},
   [ActReqDelThrSWAUsr	] = { 881, 2,TabMsg,NULL			,For_ReqRemThread		,{{    0,    0},{0x204,0x204},{0x204,0x204},{0x204,0x204},{0x204,0x204},{0x204,0x204},{0x204,0x200}},Act_NORM,Act_1ST},
   [ActReqDelThrSWATch	] = { 915, 2,TabMsg,NULL			,For_ReqRemThread		,{{    0,    0},{0x204,0x204},{0x204,0x204},{0x204,0x204},{0x204,0x204},{0x204,0x204},{0x204,0x200}},Act_NORM,Act_1ST},
   [ActDelThrForCrsUsr	] = { 868, 2,TabMsg,NULL			,For_RemoveThread		,{{    0,    0},{0x204,0x204},{0x204,0x204},{0x204,0x204},{0x204,0x204},{0x204,0x204},{0x204,0x220}},Act_NORM,Act_1ST},
   [ActDelThrForCrsTch	] = { 876, 2,TabMsg,NULL			,For_RemoveThread		,{{    0,    0},{0x204,0x204},{0x204,0x204},{0x204,0x204},{0x204,0x204},{0x204,0x204},{0x204,0x220}},Act_NORM,Act_1ST},
   [ActDelThrForDegUsr	] = { 916, 2,TabMsg,NULL			,For_RemoveThread		,{{    0,    0},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C0}},Act_NORM,Act_1ST},
   [ActDelThrForDegTch	] = { 917, 2,TabMsg,NULL			,For_RemoveThread		,{{    0,    0},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C0}},Act_NORM,Act_1ST},
   [ActDelThrForCtrUsr	] = { 918, 2,TabMsg,NULL			,For_RemoveThread		,{{    0,    0},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C0}},Act_NORM,Act_1ST},
   [ActDelThrForCtrTch	] = { 919, 2,TabMsg,NULL			,For_RemoveThread		,{{    0,    0},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C0}},Act_NORM,Act_1ST},
   [ActDelThrForInsUsr	] = { 920, 2,TabMsg,NULL			,For_RemoveThread		,{{    0,    0},{0x204,0x204},{0x204,0x204},{0x204,0x204},{0x204,0x204},{0x204,0x204},{0x204,0x200}},Act_NORM,Act_1ST},
   [ActDelThrForInsTch	] = { 921, 2,TabMsg,NULL			,For_RemoveThread		,{{    0,    0},{0x204,0x204},{0x204,0x204},{0x204,0x204},{0x204,0x204},{0x204,0x204},{0x204,0x200}},Act_NORM,Act_1ST},
   [ActDelThrForGenUsr	] = { 922, 2,TabMsg,NULL			,For_RemoveThread		,{{    0,    0},{0x204,0x204},{0x204,0x204},{0x204,0x204},{0x204,0x204},{0x204,0x204},{0x204,0x200}},Act_NORM,Act_1ST},
   [ActDelThrForGenTch	] = { 923, 2,TabMsg,NULL			,For_RemoveThread		,{{    0,    0},{0x204,0x204},{0x204,0x204},{0x204,0x204},{0x204,0x204},{0x204,0x204},{0x204,0x200}},Act_NORM,Act_1ST},
   [ActDelThrForSWAUsr	] = { 882, 2,TabMsg,NULL			,For_RemoveThread		,{{    0,    0},{0x204,0x204},{0x204,0x204},{0x204,0x204},{0x204,0x204},{0x204,0x204},{0x204,0x200}},Act_NORM,Act_1ST},
   [ActDelThrForSWATch	] = { 924, 2,TabMsg,NULL			,For_RemoveThread		,{{    0,    0},{0x204,0x204},{0x204,0x204},{0x204,0x204},{0x204,0x204},{0x204,0x204},{0x204,0x200}},Act_NORM,Act_1ST},
   [ActCutThrForCrsUsr	] = { 926, 2,TabMsg,NULL			,For_CutThread			,{{    0,    0},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200}},Act_NORM,Act_1ST},
   [ActCutThrForCrsTch	] = { 927, 2,TabMsg,NULL			,For_CutThread			,{{    0,    0},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200}},Act_NORM,Act_1ST},
   [ActCutThrForDegUsr	] = { 928, 2,TabMsg,NULL			,For_CutThread			,{{    0,    0},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200}},Act_NORM,Act_1ST},
   [ActCutThrForDegTch	] = { 929, 2,TabMsg,NULL			,For_CutThread			,{{    0,    0},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200}},Act_NORM,Act_1ST},
   [ActCutThrForCtrUsr	] = { 930, 2,TabMsg,NULL			,For_CutThread			,{{    0,    0},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200}},Act_NORM,Act_1ST},
   [ActCutThrForCtrTch	] = { 931, 2,TabMsg,NULL			,For_CutThread			,{{    0,    0},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200}},Act_NORM,Act_1ST},
   [ActCutThrForInsUsr	] = { 932, 2,TabMsg,NULL			,For_CutThread			,{{    0,    0},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200}},Act_NORM,Act_1ST},
   [ActCutThrForInsTch	] = { 933, 2,TabMsg,NULL			,For_CutThread			,{{    0,    0},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200}},Act_NORM,Act_1ST},
   [ActCutThrForGenUsr	] = { 934, 2,TabMsg,NULL			,For_CutThread			,{{    0,    0},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200}},Act_NORM,Act_1ST},
   [ActCutThrForGenTch	] = { 935, 2,TabMsg,NULL			,For_CutThread			,{{    0,    0},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200}},Act_NORM,Act_1ST},
   [ActCutThrForSWAUsr	] = { 890, 2,TabMsg,NULL			,For_CutThread			,{{    0,    0},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200}},Act_NORM,Act_1ST},
   [ActCutThrForSWATch	] = { 936, 2,TabMsg,NULL			,For_CutThread			,{{    0,    0},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200}},Act_NORM,Act_1ST},
   [ActPasThrForCrsUsr	] = { 891, 2,TabMsg,NULL			,For_PasteThread		,{{    0,    0},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200}},Act_NORM,Act_1ST},
   [ActPasThrForCrsTch	] = { 937, 2,TabMsg,NULL			,For_PasteThread		,{{    0,    0},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200}},Act_NORM,Act_1ST},
   [ActPasThrForDegUsr	] = { 938, 2,TabMsg,NULL			,For_PasteThread		,{{    0,    0},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200}},Act_NORM,Act_1ST},
   [ActPasThrForDegTch	] = { 939, 2,TabMsg,NULL			,For_PasteThread		,{{    0,    0},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200}},Act_NORM,Act_1ST},
   [ActPasThrForCtrUsr	] = { 940, 2,TabMsg,NULL			,For_PasteThread		,{{    0,    0},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200}},Act_NORM,Act_1ST},
   [ActPasThrForCtrTch	] = { 941, 2,TabMsg,NULL			,For_PasteThread		,{{    0,    0},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200}},Act_NORM,Act_1ST},
   [ActPasThrForInsUsr	] = { 942, 2,TabMsg,NULL			,For_PasteThread		,{{    0,    0},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200}},Act_NORM,Act_1ST},
   [ActPasThrForInsTch	] = { 943, 2,TabMsg,NULL			,For_PasteThread		,{{    0,    0},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200}},Act_NORM,Act_1ST},
   [ActPasThrForGenUsr	] = { 944, 2,TabMsg,NULL			,For_PasteThread		,{{    0,    0},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200}},Act_NORM,Act_1ST},
   [ActPasThrForGenTch	] = { 945, 2,TabMsg,NULL			,For_PasteThread		,{{    0,    0},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200}},Act_NORM,Act_1ST},
   [ActPasThrForSWAUsr	] = { 946, 2,TabMsg,NULL			,For_PasteThread		,{{    0,    0},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200}},Act_NORM,Act_1ST},
   [ActPasThrForSWATch	] = { 947, 2,TabMsg,NULL			,For_PasteThread		,{{    0,    0},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200}},Act_NORM,Act_1ST},
   [ActDelPstForCrsUsr	] = { 602, 2,TabMsg,NULL			,For_RemovePost			,{{    0,    0},{0x204,0x204},{0x204,0x204},{0x204,0x204},{0x204,0x204},{0x204,0x204},{0x204,0x238}},Act_NORM,Act_1ST},
   [ActDelPstForCrsTch	] = { 756, 2,TabMsg,NULL			,For_RemovePost			,{{    0,    0},{0x204,0x204},{0x204,0x204},{0x204,0x204},{0x204,0x204},{0x204,0x204},{0x204,0x230}},Act_NORM,Act_1ST},
   [ActDelPstForDegUsr	] = { 608, 2,TabMsg,NULL			,For_RemovePost			,{{    0,    0},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3F8}},Act_NORM,Act_1ST},
   [ActDelPstForDegTch	] = { 680, 2,TabMsg,NULL			,For_RemovePost			,{{    0,    0},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3F0}},Act_NORM,Act_1ST},
   [ActDelPstForCtrUsr	] = { 948, 2,TabMsg,NULL			,For_RemovePost			,{{    0,    0},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3F8}},Act_NORM,Act_1ST},
   [ActDelPstForCtrTch	] = { 949, 2,TabMsg,NULL			,For_RemovePost			,{{    0,    0},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3F0}},Act_NORM,Act_1ST},
   [ActDelPstForInsUsr	] = { 743, 2,TabMsg,NULL			,For_RemovePost			,{{    0,    0},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3F8}},Act_NORM,Act_1ST},
   [ActDelPstForInsTch	] = { 772, 2,TabMsg,NULL			,For_RemovePost			,{{    0,    0},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3F0}},Act_NORM,Act_1ST},
   [ActDelPstForGenUsr	] = { 735, 2,TabMsg,NULL			,For_RemovePost			,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_1ST},
   [ActDelPstForGenTch	] = { 950, 2,TabMsg,NULL			,For_RemovePost			,{{    0,    0},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3F0}},Act_NORM,Act_1ST},
   [ActDelPstForSWAUsr	] = { 613, 2,TabMsg,NULL			,For_RemovePost			,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_1ST},
   [ActDelPstForSWATch	] = { 623, 2,TabMsg,NULL			,For_RemovePost			,{{    0,    0},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3C4},{0x3C4,0x3F0}},Act_NORM,Act_1ST},
   [ActEnbPstForCrsUsr	] = { 624, 2,TabMsg,NULL			,For_EnablePost			,{{    0,    0},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActEnbPstForCrsTch	] = { 951, 2,TabMsg,NULL			,For_EnablePost			,{{    0,    0},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActEnbPstForDegUsr	] = { 616, 2,TabMsg,NULL			,For_EnablePost			,{{    0,    0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0}},Act_NORM,Act_1ST},
   [ActEnbPstForDegTch	] = { 619, 2,TabMsg,NULL			,For_EnablePost			,{{    0,    0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0}},Act_NORM,Act_1ST},
   [ActEnbPstForCtrUsr	] = { 952, 2,TabMsg,NULL			,For_EnablePost			,{{    0,    0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0}},Act_NORM,Act_1ST},
   [ActEnbPstForCtrTch	] = { 953, 2,TabMsg,NULL			,For_EnablePost			,{{    0,    0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0}},Act_NORM,Act_1ST},
   [ActEnbPstForInsUsr	] = { 954, 2,TabMsg,NULL			,For_EnablePost			,{{    0,    0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0}},Act_NORM,Act_1ST},
   [ActEnbPstForInsTch	] = { 955, 2,TabMsg,NULL			,For_EnablePost			,{{    0,    0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0}},Act_NORM,Act_1ST},
   [ActEnbPstForGenUsr	] = { 956, 2,TabMsg,NULL			,For_EnablePost			,{{    0,    0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0}},Act_NORM,Act_1ST},
   [ActEnbPstForGenTch	] = { 957, 2,TabMsg,NULL			,For_EnablePost			,{{    0,    0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0}},Act_NORM,Act_1ST},
   [ActEnbPstForSWAUsr	] = { 632, 2,TabMsg,NULL			,For_EnablePost			,{{    0,    0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0}},Act_NORM,Act_1ST},
   [ActEnbPstForSWATch	] = { 634, 2,TabMsg,NULL			,For_EnablePost			,{{    0,    0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0}},Act_NORM,Act_1ST},
   [ActDisPstForCrsUsr	] = { 610, 2,TabMsg,NULL			,For_DisablePost		,{{    0,    0},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActDisPstForCrsTch	] = { 958, 2,TabMsg,NULL			,For_DisablePost		,{{    0,    0},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActDisPstForDegUsr	] = { 615, 2,TabMsg,NULL			,For_DisablePost		,{{    0,    0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0}},Act_NORM,Act_1ST},
   [ActDisPstForDegTch	] = { 618, 2,TabMsg,NULL			,For_DisablePost		,{{    0,    0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0}},Act_NORM,Act_1ST},
   [ActDisPstForCtrUsr	] = { 959, 2,TabMsg,NULL			,For_DisablePost		,{{    0,    0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0}},Act_NORM,Act_1ST},
   [ActDisPstForCtrTch	] = { 960, 2,TabMsg,NULL			,For_DisablePost		,{{    0,    0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0}},Act_NORM,Act_1ST},
   [ActDisPstForInsUsr	] = { 961, 2,TabMsg,NULL			,For_DisablePost		,{{    0,    0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0}},Act_NORM,Act_1ST},
   [ActDisPstForInsTch	] = { 962, 2,TabMsg,NULL			,For_DisablePost		,{{    0,    0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0}},Act_NORM,Act_1ST},
   [ActDisPstForGenUsr	] = { 963, 2,TabMsg,NULL			,For_DisablePost		,{{    0,    0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0}},Act_NORM,Act_1ST},
   [ActDisPstForGenTch	] = { 925, 2,TabMsg,NULL			,For_DisablePost		,{{    0,    0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0}},Act_NORM,Act_1ST},
   [ActDisPstForSWAUsr	] = { 625, 2,TabMsg,NULL			,For_DisablePost		,{{    0,    0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0}},Act_NORM,Act_1ST},
   [ActDisPstForSWATch	] = { 635, 2,TabMsg,NULL			,For_DisablePost		,{{    0,    0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0}},Act_NORM,Act_1ST},
   [ActReqLnkForCrsUsr  ] = {1941, 2,TabMsg,NULL			,ForRsc_GetLinkToThread		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},

   // Chat
   [ActSeeChtRms	] = {  51, 3,TabMsg,NULL			,Cht_ShowChatRooms		,{{    0,    0},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200}},Act_NORM,Act_1ST},
   [ActCht		] = {  52, 3,TabMsg,Cht_OpenChatWindow		,NULL				,{{    0,    0},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200},{0x200,0x200}},Act_NORM,Act_NEW},

   // Messages
   [ActSeeRcvMsg	] = {   3, 4,TabMsg,NULL			,Msg_ShowRecMsgs		,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_1ST},
   [ActReqMsgUsr	] = {  26, 4,TabMsg,NULL			,Msg_FormMsgUsrs		,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_1ST},
   [ActSeeSntMsg	] = {  70, 4,TabMsg,NULL			,Msg_ShowSntMsgs		,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_1ST},
   [ActRcvMsgUsr	] = {  27, 4,TabMsg,NULL			,Msg_RecMsgFromUsr		,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_DATA,Act_1ST},
   [ActReqDelAllSntMsg	] = { 604, 4,TabMsg,NULL			,Msg_ReqDelAllSntMsgs		,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_1ST},
   [ActReqDelAllRcvMsg	] = { 593, 4,TabMsg,NULL			,Msg_ReqDelAllRecMsgs		,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_1ST},
   [ActDelAllSntMsg	] = { 434, 4,TabMsg,NULL			,Msg_DelAllSntMsgs		,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_1ST},
   [ActDelAllRcvMsg	] = { 436, 4,TabMsg,NULL			,Msg_DelAllRecMsgs		,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_1ST},
   [ActDelSntMsg	] = {  90, 4,TabMsg,NULL			,Msg_DelSntMsg			,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_1ST},
   [ActDelRcvMsg	] = {  64, 4,TabMsg,NULL			,Msg_DelRecMsg			,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_1ST},
   [ActExpSntMsg	] = { 664, 4,TabMsg,NULL			,Msg_ExpSntMsg			,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_1ST},
   [ActExpRcvMsg	] = { 663, 4,TabMsg,NULL			,Msg_ExpRecMsg			,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_1ST},
   [ActConSntMsg	] = {1020, 4,TabMsg,NULL			,Msg_ConSntMsg			,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_1ST},
   [ActConRcvMsg	] = {1019, 4,TabMsg,NULL			,Msg_ConRecMsg			,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_1ST},
   [ActLstBanUsr	] = {1017, 4,TabMsg,NULL			,Msg_ListBannedUsrs		,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_1ST},
   [ActBanUsrMsg	] = {1015, 4,TabMsg,NULL			,Msg_BanSenderWhenShowingMsgs	,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_1ST},
   [ActUnbUsrMsg	] = {1016, 4,TabMsg,NULL			,Msg_UnbanSenderWhenShowingMsgs	,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_1ST},
   [ActUnbUsrLst	] = {1018, 4,TabMsg,NULL			,Msg_UnbanSenderWhenListingUsrs	,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_1ST},

   // Mail
   [ActReqMaiUsr	] = {1772, 5,TabMsg,NULL			,Mai_ReqUsrsToListEmails	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x230}},Act_NORM,Act_1ST},
   [ActMaiUsr		] = { 100, 5,TabMsg,NULL			,Mai_GetSelUsrsAndListEmails	,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x230}},Act_NORM,Act_1ST},

   // TabAna ******************************************************************
   // Figures
   [ActReqUseGbl	] = { 761, 0,TabAna,NULL			,Fig_ReqShowFigures		,{{    0,    0},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3F8}},Act_NORM,Act_1ST},
   [ActSeeUseGbl	] = {  84, 0,TabAna,NULL			,Fig_ShowFigures		,{{    0,    0},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3F8}},Act_NORM,Act_1ST},

   // Degrees
   [ActSeePhoDeg	] = { 447, 1,TabAna,NULL			,Pho_ShowPhotoDegree		,{{    0,    0},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3F8}},Act_NORM,Act_1ST},
   [ActPrnPhoDeg	] = { 448, 1,TabAna,NULL			,Pho_PrintPhotoDegree  		,{{    0,    0},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3F8}},Act_NORM,Act_NEW},
   [ActCalPhoDeg	] = { 444, 1,TabAna,NULL			,Pho_CalcPhotoDegree		,{{    0,    0},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3F8}},Act_NORM,Act_1ST},

   // Indicators
   [ActReqStaCrs	] = { 767, 2,TabAna,NULL			,Ind_ReqIndicatorsCourses	,{{    0,    0},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3F8}},Act_NORM,Act_1ST},
   [ActSeeAllStaCrs	] = { 768, 2,TabAna,NULL			,Ind_ShowIndicatorsCourses	,{{    0,    0},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3F8}},Act_NORM,Act_NEW},

   // Surveys
   [ActSeeAllSvy	] = { 966, 3,TabAna,NULL			,Svy_SeeAllSurveys		,{{    0,    0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3F8}},Act_NORM,Act_1ST},
   [ActSeeOneSvy	] = { 982, 3,TabAna,NULL			,Svy_SeeOneSurvey		,{{    0,    0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3F8}},Act_NORM,Act_1ST},
   [ActAnsSvy		] = { 983, 3,TabAna,NULL			,Svy_ReceiveSurveyAnswers	,{{    0,    0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3F8}},Act_NORM,Act_1ST},
   [ActFrmNewSvy	] = { 973, 3,TabAna,NULL			,Svy_ReqCreatOrEditSvy		,{{    0,    0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3E0}},Act_NORM,Act_1ST},
   [ActEdiOneSvy	] = { 974, 3,TabAna,NULL			,Svy_ReqCreatOrEditSvy		,{{    0,    0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3E0}},Act_NORM,Act_1ST},
   [ActNewSvy		] = { 968, 3,TabAna,NULL			,Svy_ReceiveSurvey		,{{    0,    0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3E0}},Act_NORM,Act_1ST},
   [ActChgSvy		] = { 975, 3,TabAna,NULL			,Svy_ReceiveSurvey		,{{    0,    0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3E0}},Act_NORM,Act_1ST},
   [ActReqRemSvy	] = { 976, 3,TabAna,NULL			,Svy_AskRemSurvey		,{{    0,    0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3E0}},Act_NORM,Act_1ST},
   [ActRemSvy		] = { 969, 3,TabAna,NULL			,Svy_RemoveSurvey		,{{    0,    0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3E0}},Act_NORM,Act_1ST},
   [ActReqRstSvy	] = { 984, 3,TabAna,NULL			,Svy_AskResetSurvey		,{{    0,    0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3E0}},Act_NORM,Act_1ST},
   [ActRstSvy		] = { 985, 3,TabAna,NULL			,Svy_ResetSurvey		,{{    0,    0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3E0}},Act_NORM,Act_1ST},
   [ActHidSvy		] = { 977, 3,TabAna,NULL			,Svy_HideSurvey			,{{    0,    0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3E0}},Act_NORM,Act_1ST},
   [ActUnhSvy		] = { 978, 3,TabAna,NULL			,Svy_UnhideSurvey		,{{    0,    0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3E0}},Act_NORM,Act_1ST},
   [ActReqLnkSvy	] = {1937, 3,TabAna,NULL			,SvyRsc_GetLinkToSurvey		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x220}},Act_NORM,Act_1ST},
   [ActEdiOneSvyQst	] = { 979, 3,TabAna,NULL			,Svy_ReqEditQuestion		,{{    0,    0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3E0}},Act_NORM,Act_1ST},
   [ActNewSvyQst	] = { 980, 3,TabAna,NULL			,Svy_ReceiveQst			,{{    0,    0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3E0}},Act_NORM,Act_1ST},
   [ActChgSvyQst	] = {1976, 3,TabAna,NULL			,Svy_ReceiveQst			,{{    0,    0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3E0}},Act_NORM,Act_1ST},
   [ActReqRemSvyQst	] = {1524, 3,TabAna,NULL			,Svy_ReqRemQst			,{{    0,    0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3E0}},Act_NORM,Act_1ST},
   [ActRemSvyQst	] = { 981, 3,TabAna,NULL			,Svy_RemoveQst			,{{    0,    0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3C0},{0x3C0,0x3E0}},Act_NORM,Act_1ST},

   // Visits
   [ActReqAccGbl	] = { 591, 4,TabAna,Dat_SetDatesToRecWeeks	,Sta_AskShowGblHits		,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_1ST},
   [ActSeeAccGbl	] = {  79, 4,TabAna,NULL			,Sta_SeeGblAccesses		,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_1ST},
   [ActReqAccCrs	] = { 594, 4,TabAna,Dat_SetDatesToRecWeeks	,Sta_ReqCrsHits			,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x230}},Act_NORM,Act_1ST},
   [ActSeeAccCrs	] = { 119, 4,TabAna,NULL			,Sta_SeeCrsAccesses		,{{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{    0,    0},{0x200,0x230}},Act_NORM,Act_1ST},
   [ActLstClk		] = { 989, 4,TabAna,NULL			,Log_ShowLastClicks		,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_1ST},
   [ActRefLstClk	] = { 994, 4,TabAna,NULL			,Lay_RefreshLastClicks		,{{    0,    0},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3F8}},Act_NORM,Act_REF},

   // Report
   [ActReqMyUsgRep	] = {1586, 5,TabAna,NULL			,Rep_ReqMyUsageReport		,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_1ST},
   [ActSeeMyUsgRep	] = {1582, 5,TabAna,NULL			,Rep_ShowMyUsageReport		,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_1ST},

   // Frequent
   [ActMFUAct		] = { 993, 6,TabAna,NULL			,MFU_ShowMyMFUActions		,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_1ST},

   // TabPrf ******************************************************************
   // Session
   [ActFrmRolSes	] = { 843, 0,TabPrf,NULL			,Usr_WriteFormLogout		,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_1ST},
   [ActChgMyRol		] = { 589, 0,TabPrf,Rol_ChangeMyRole		,Usr_ShowFormsLogoutAndRole	,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_1ST},

   // Courses
   [ActMyCrs		] = { 987, 1,TabPrf,NULL			,Hie_SelOneNodeFromMyHierarchy	,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_1ST},

   // Timetable
   [ActSeeMyTT		] = { 408, 2,TabPrf,NULL			,Tmt_ShowClassTimeTable		,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_1ST},
   [ActPrnMyTT		] = { 409, 2,TabPrf,NULL			,Tmt_ShowClassTimeTable		,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_NEW},
   [ActEdiTut		] = {  65, 2,TabPrf,NULL			,Tmt_EditMyTutTimeTable		,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_1ST},
   [ActChgTut		] = {  48, 2,TabPrf,NULL			,Tmt_EditMyTutTimeTable		,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_1ST},
   [ActChgMyTT1stDay	] = {1487, 2,TabPrf,Cal_Change1stDayOfWeek	,Tmt_ShowClassTimeTable		,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C7,0x3F8}},Act_NORM,Act_1ST},

   // Agenda
   [ActSeeMyAgd		] = {1602, 3,TabPrf,NULL			,Agd_GetParsAndShowMyAgenda	,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_1ST},
   [ActFrmNewEvtMyAgd	] = {1603, 3,TabPrf,NULL			,Agd_ReqCreatOrEditEvent	,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_1ST},
   [ActEdiOneEvtMyAgd	] = {1604, 3,TabPrf,NULL			,Agd_ReqCreatOrEditEvent	,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_1ST},
   [ActNewEvtMyAgd	] = {1605, 3,TabPrf,NULL			,Agd_ReceiveEvent		,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_1ST},
   [ActChgEvtMyAgd	] = {1606, 3,TabPrf,NULL			,Agd_ReceiveEvent		,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_1ST},
   [ActReqRemEvtMyAgd	] = {1607, 3,TabPrf,NULL			,Agd_AskRemEvent		,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_1ST},
   [ActRemEvtMyAgd	] = {1608, 3,TabPrf,NULL			,Agd_RemoveEvent		,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_1ST},
   [ActHidEvtMyAgd	] = {1612, 3,TabPrf,NULL			,Agd_HideEvent			,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_1ST},
   [ActUnhEvtMyAgd	] = {1613, 3,TabPrf,NULL			,Agd_UnhideEvent		,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_1ST},
   [ActPrvEvtMyAgd	] = {1609, 3,TabPrf,NULL			,Agd_MakeEventPrivate		,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_1ST},
   [ActPubEvtMyAgd	] = {1610, 3,TabPrf,NULL			,Agd_MakeEventPublic		,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_1ST},
   [ActPrnAgdQR		] = {1618, 3,TabPrf,NULL			,Agd_PrintAgdQRCode		,{{    0,    0},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3F8}},Act_NORM,Act_NEW},

   // Account
   [ActFrmMyAcc		] = {  36, 4,TabPrf,NULL			,Acc_ShowFormMyAccount		,{{    0,    0},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3F8}},Act_NORM,Act_1ST},
   [ActChkUsrAcc	] = {1584, 4,TabPrf,NULL			,Acc_CheckIfEmptyAccountExists	,{{    0,    0},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3F8}},Act_NORM,Act_1ST},
   [ActCreMyAcc		] = {1787, 4,TabPrf,NULL			,Acc_ShowFormCreateMyAccount	,{{    0,    0},{0x001,0x001},{0x001,0x001},{0x001,0x001},{0x001,0x001},{0x001,0x001},{0x001,0x001}},Act_NORM,Act_1ST},
   [ActCreUsrAcc	] = {1163, 4,TabPrf,NULL			,Acc_AfterCreationNewAccount	,{{    0,    0},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3F8}},Act_NORM,Act_1ST},
   [ActRemMyID		] = {1147, 4,TabPrf,NULL			,ID_RemoveMyUsrID		,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_1ST},
   [ActChgMyID		] = {1148, 4,TabPrf,NULL			,ID_NewMyUsrID			,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_1ST},
   [ActRemMyNck		] = {1089, 4,TabPrf,NULL			,Nck_RemoveMyNick		,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_1ST},
   [ActChgMyNck		] = {  37, 4,TabPrf,NULL			,Nck_UpdateMyNick		,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_1ST},
   [ActRemMyMai		] = {1090, 4,TabPrf,NULL			,Mai_RemoveMyUsrEmail		,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_1ST},
   [ActChgMyMai		] = {1088, 4,TabPrf,NULL			,May_NewMyUsrEmail		,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_1ST},
   [ActCnfMai		] = {1091, 4,TabPrf,NULL			,Mai_ConfirmEmail		,{{    0,    0},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3F8}},Act_NORM,Act_1ST},
   [ActChgMyPwd		] = {  35, 4,TabPrf,Pwd_UpdateMyPwd		,Acc_ShowFormChgMyAccount	,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_1ST},
   [ActReqRemMyAcc	] = {1430, 4,TabPrf,NULL			,Acc_AskIfRemoveMyAccount	,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_1ST},
   [ActRemMyAcc		] = {1431, 4,TabPrf,NULL			,Acc_RemoveMyAccount		,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_1ST},
   [ActPrnUsrQR		] = {1022, 4,TabPrf,NULL			,QR_PrintUsrQRCode		,{{    0,    0},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3F8}},Act_NORM,Act_NEW},

   // Record
   [ActReqEdiRecSha	] = { 285, 5,TabPrf,NULL			,Rec_ShowMySharedRecordAndMore	,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_1ST},
   [ActChgMyData	] = { 298, 5,TabPrf,Rec_UpdateMyRecord		,Rec_ShowMySharedRecordUpd	,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_1ST},
   [ActReqMyPho		] = {  30, 5,TabPrf,NULL			,Pho_ReqMyPhoto			,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_1ST},
   [ActDetMyPho		] = { 693, 5,TabPrf,NULL			,Pho_RecMyPhotoDetFaces		,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_DATA,Act_1ST},
   [ActUpdMyPho		] = { 694, 5,TabPrf,Pho_UpdateMyPhoto1		,Pho_UpdateMyPhoto2		,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_1ST},
   [ActReqRemMyPho	] = {1577, 5,TabPrf,NULL			,Pho_ReqRemMyPhoto		,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_1ST},
   [ActRemMyPho		] = { 428, 5,TabPrf,Pho_RemoveMyPhoto1		,Pho_RemoveMyPhoto2		,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_1ST},
   [ActChgCtyMyIns	] = {1166, 5,TabPrf,NULL			,Rec_ChgCountryOfMyInstitution	,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_1ST},
   [ActChgMyIns		] = {1167, 5,TabPrf,NULL			,Rec_UpdateMyInstitution	,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_1ST},
   [ActChgMyCtr		] = {1168, 5,TabPrf,NULL			,Rec_UpdateMyCenter		,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_1ST},
   [ActChgMyDpt		] = {1169, 5,TabPrf,NULL			,Rec_UpdateMyDepartment		,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_1ST},
   [ActChgMyOff		] = {1170, 5,TabPrf,NULL			,Rec_UpdateMyOffice		,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_1ST},
   [ActChgMyOffPho	] = {1171, 5,TabPrf,NULL			,Rec_UpdateMyOfficePhone	,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_1ST},
   [ActReqEdiMyNet	] = {1172, 5,TabPrf,NULL			,Rec_ShowMySharedRecordAndMore	,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_1ST},
   [ActChgMyNet		] = {1173, 5,TabPrf,NULL			,Net_UpdateMyWebsAndSocialNets	,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_1ST},

   // Settings
   [ActReqEdiSet	] = { 673, 6,TabPrf,NULL			,Set_EditSettings		,{{    0,    0},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3F8}},Act_NORM,Act_1ST},
   [ActChgThe		] = { 841, 6,TabPrf,The_ChangeTheme		,Set_EditSettings		,{{    0,    0},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3F8}},Act_NORM,Act_1ST},
   [ActReqChgLan	] = { 992, 6,TabPrf,NULL			,Lan_AskChangeLanguage		,{{    0,    0},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3F8}},Act_NORM,Act_1ST},
   [ActChgLan		] = { 654, 6,TabPrf,Lan_ChangeLanguage		,Set_EditSettings		,{{    0,    0},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3F8}},Act_NORM,Act_1ST},
   [ActChg1stDay	] = {1484, 6,TabPrf,Cal_Change1stDayOfWeek	,Set_EditSettings		,{{    0,    0},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3F8}},Act_NORM,Act_1ST},
   [ActChgDatFmt	] = {1638, 6,TabPrf,Dat_ChangeDateFormat	,Set_EditSettings		,{{    0,    0},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3F8}},Act_NORM,Act_1ST},
   [ActChgCol		] = { 674, 6,TabPrf,Set_ChangeSideCols		,Set_EditSettings		,{{    0,    0},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3F8}},Act_NORM,Act_1ST},
   [ActChgIco		] = {1092, 6,TabPrf,Ico_ChangeIconSet		,Set_EditSettings		,{{    0,    0},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3F8}},Act_NORM,Act_1ST},
   [ActChgMnu		] = {1243, 6,TabPrf,Mnu_ChangeMenu		,Set_EditSettings		,{{    0,    0},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3F8}},Act_NORM,Act_1ST},
   [ActChgUsrPho	] = {1917, 6,TabPrf,PhoSha_ChangePhotoShape	,Set_EditSettings		,{{    0,    0},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3C7},{0x3C7,0x3F8}},Act_NORM,Act_1ST},
   [ActChgPriPho	] = { 774, 6,TabPrf,NULL			,Pho_ChangePhotoVisibility	,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_1ST},
   [ActChgBasPriPrf	] = {1404, 6,TabPrf,NULL			,Prf_ChangeBasicProfileVis	,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_1ST},
   [ActChgExtPriPrf	] = {1765, 6,TabPrf,NULL			,Prf_ChangeExtendedProfileVis	,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_1ST},
   [ActChgCooPrf	] = {1764, 6,TabPrf,NULL			,Coo_ChangeMyPrefsCookies	,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_1ST},
   [ActChgNtfPrf	] = { 775, 6,TabPrf,Ntf_ChangeNotifyEvents	,Set_EditSettings		,{{    0,    0},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3C6},{0x3C6,0x3F8}},Act_NORM,Act_1ST},
  };

Act_Action_t ActLst_FromActCodToAction[1 + ActLst_MAX_ACTION_COD] =	// Do not reuse unique action codes!
  {
	ActSeeAdmDocCrsGrp,	//  #0
	ActUnk,			//  #1 (obsolete action)
	ActMnu,			//  #2
	ActSeeRcvMsg,		//  #3
	ActUnk,			//  #4 (obsolete action)
	ActUnk,			//  #5 (obsolete action)
	ActLogIn,		//  #6
	ActUnk,			//  #7 (obsolete action)
	ActUnk,			//  #8 (obsolete action)
	ActSeeCrsLnk,		//  #9
	ActLogOut,		// #10
	ActUnk,			// #11 (obsolete action)
	ActAdmDocCrs,		// #12
	ActUnk,			// #13 (obsolete action)
	ActUnk,			// #14 (obsolete action)
	ActSeeAss,		// #15
	ActUnk,			// #16 (obsolete action)
	ActSeeAdmMrk,		// #17
	ActUnk,			// #18 (obsolete action)
	ActUnk,			// #19 (obsolete action)
	ActUnk,			// #20 (obsolete action)
	ActUnk,			// #21 (obsolete action)
	ActSeeRecSevTch,	// #22
	ActAdmBrf,		// #23
	ActUnk,			// #24 (obsolete action)
	ActSeeCrsTT,		// #25
	ActReqMsgUsr,		// #26
	ActRcvMsgUsr,		// #27
	ActUnk,			// #28 (obsolete action)
	ActSeeTst,		// #29
	ActReqMyPho,		// #30
	ActUnk,			// #31 (obsolete action)
	ActSeeBib,		// #32
	ActUnk,			// #33 (obsolete action)
	ActUnk,			// #34 (obsolete action)
	ActChgMyPwd,		// #35
	ActFrmMyAcc,		// #36
	ActChgMyNck,		// #37
	ActUnk,			// #38 (obsolete action)
	ActUnk,			// #39 (obsolete action)
	ActUnk,			// #40 (obsolete action)
	ActUnk,			// #41 (obsolete action)
	ActLstStdAll,		// #42
	ActUnk,			// #43 (obsolete action)
	ActEdiSyl,		// #44
	ActEdiCrsTT,		// #45
	ActUnk,			// #46 (obsolete action)
	ActUnk,			// #47 (obsolete action)
	ActChgTut,		// #48
	ActUnk,			// #49 (obsolete action)
	ActUnk,			// #50 (obsolete action)
	ActSeeChtRms,		// #51
	ActCht,			// #52
	ActChgCrsTT,		// #53
	ActSeeFAQ,		// #54
	ActUnk,			// #55 (obsolete action)
	ActUnk,			// #56 (obsolete action)
	ActUnk,			// #57 (obsolete action)
	ActUnk,			// #58 (obsolete action)
	ActWriNot,		// #59
	ActNewNot,		// #60
	ActUnk,			// #61 (obsolete action)
	ActRemUsrGbl,		// #62
	ActUnk,			// #63 (obsolete action)
	ActDelRcvMsg,		// #64
	ActEdiTut,		// #65
	ActUnk,			// #66 (obsolete action)
	ActUnk,			// #67 (obsolete action)
	ActUnk,			// #68 (obsolete action)
	ActEdiAss,		// #69
	ActSeeSntMsg,		// #70
	ActUnk,			// #71 (obsolete action)
	ActUnk,			// #72 (obsolete action)
	ActRemNot,		// #73
	ActUnk,			// #74 (obsolete action)
	ActUnk,			// #75 (obsolete action)
	ActEdiBib,		// #76
	ActUnk,			// #77 (obsolete action)
	ActUnk,			// #78 (obsolete action)
	ActSeeAccGbl,		// #79
	ActUnk,			// #80 (obsolete action)
	ActUnk,			// #81 (obsolete action)
	ActUnk,			// #82 (obsolete action)
	ActUnk,			// #83 (obsolete action)
	ActSeeUseGbl,		// #84
	ActSeeAllCfe,		// #85
	ActUnk,			// #86 (obsolete action)
	ActRemAllStdCrs,	// #87
	ActReqRemAllStdCrs,	// #88
	ActSeeRecSevStd,	// #89
	ActDelSntMsg,		// #90
	ActEdiCfe,		// #91
	ActUnk,			// #92 (obsolete action)
	ActUnk,			// #93 (obsolete action)
	ActUnk,			// #94 (obsolete action)
	ActSeeFor,		// #95
	ActEdiCrsLnk,		// #96
	ActUnk,			// #97 (obsolete action)
	ActAssTst,		// #98
	ActUnk,			// #99 (obsolete action)
	ActMaiUsr,		// #100
	ActUnk,			// #101 (obsolete action)
	ActUnk,			// #102 (obsolete action)
	ActReqTstAnyTag,	// #103
	ActEdiTstQst,		// #104
	ActEdiOneTstQst,	// #105
	ActChgMaxStdGrp,	// #106
	ActReqRemGrp,		// #107
	ActReqEdiGrp,		// #108
	ActEdiFAQ,		// #109
	ActRcvCfe,		// #110
	ActPrnRecSevStd,	// #111
	ActUnk,			// #112 (obsolete action)
	ActUnk,			// #113 (obsolete action)
	ActUnk,			// #114 (obsolete action)
	ActUnk,			// #115 (obsolete action)
	ActReqSelAllGrp,	// #116
	ActUnk,			// #117 (obsolete action)
	ActChgGrp,		// #118
	ActSeeAccCrs,		// #119
	ActPrnStdPho,		// #120
	ActRenGrp,		// #121
	ActNewGrp,		// #122
	ActUnk,			// #123 (obsolete action)
	ActUnk,			// #124 (obsolete action)
	ActUnk,			// #125 (obsolete action)
	ActNewTstQst,		// #126
	ActPrnRecSevTch,	// #127
	ActUnk,			// #128 (obsolete action)
	ActUnk,			// #129 (obsolete action)
	ActUnk,			// #130 (obsolete action)
	ActUnk,			// #131 (obsolete action)
	ActLstTstQst,		// #132
	ActRemOneTstQst,	// #133
	ActUnk,			// #134 (obsolete action)
	ActUnk,			// #135 (obsolete action)
	ActUnk,			// #136 (obsolete action)
	ActUnk,			// #137 (obsolete action)
	ActUnk,			// #138 (obsolete action)
	ActAdmAsgWrkCrs,	// #139
	ActUnk,			// #140 (obsolete action)
	ActUnk,			// #141 (obsolete action)
	ActUnk,			// #142 (obsolete action)
	ActRenTag,		// #143
	ActUnk,			// #144 (obsolete action)
	ActUnk,			// #145 (obsolete action)
	ActUnk,			// #146 (obsolete action)
	ActUnk,			// #147 (obsolete action)
	ActRcvFilWrkUsrCla,	// #148
	ActUnk,			// #149 (obsolete action)
	ActFrmCreWrkUsr,	// #150
	ActUnk,			// #151 (obsolete action)
	ActPrnCrsTT,		// #152
	ActRcvFilBrfCla,	// #153
	ActUnk,			// #154 (obsolete action)
	ActRemFilBrf,		// #155
	ActUnk,			// #156 (obsolete action)
	ActUnk,			// #157 (obsolete action)
	ActUnk,			// #158 (obsolete action)
	ActUnk,			// #159 (obsolete action)
	ActUnk,			// #160 (obsolete action)
	ActUnk,			// #161 (obsolete action)
	ActUnk,			// #162 (obsolete action)
	ActUnk,			// #163 (obsolete action)
	ActRcvPagCrsLnk,	// #164
	ActUnk,			// #165 (obsolete action)
	ActUnk,			// #166 (obsolete action)
	ActChgGrpTyp,		// #167
	ActUnk,			// #168 (obsolete action)
	ActRemFilWrkUsr,	// #169
	ActCreFolBrf,		// #170
	ActUnk,			// #171 (obsolete action)
	ActCreFolWrkUsr,	// #172
	ActUnk,			// #173 (obsolete action)
	ActNewGrpTyp,		// #174
	ActRemGrp,		// #175
	ActUnk,			// #176 (obsolete action)
	ActUnk,			// #177 (obsolete action)
	ActUnk,			// #178 (obsolete action)
	ActPrnCfe,		// #179
	ActUnk,			// #180 (obsolete action)
	ActUnk,			// #181 (obsolete action)
	ActRcvURLCrsLnk,	// #182
	ActUnk,			// #183 (obsolete action)
	ActRcvPagAss,		// #184
	ActRcvPagBib,		// #185
	ActUnk,			// #186 (obsolete action)
	ActRemCfe,		// #187
	ActUnk,			// #188 (obsolete action)
	ActUnk,			// #189 (obsolete action)
	ActUnk,			// #190 (obsolete action)
	ActUnk,			// #191 (obsolete action)
	ActUnk,			// #192 (obsolete action)
	ActUnk,			// #193 (obsolete action)
	ActUnk,			// #194
	ActUnk,			// #195 (obsolete action)
	ActRemFolBrf,		// #196
	ActRenFolBrf,		// #197
	ActUnk,			// #198 (obsolete action)
	ActUnk,			// #199 (obsolete action)
	ActUnk,			// #200 (obsolete action)
	ActUnk,			// #201 (obsolete action)
	ActUnk,			// #202 (obsolete action)
	ActUnk,			// #203 (obsolete action)
	ActRenFolWrkUsr,	// #204
	ActFrmCreWrkCrs,	// #205
	ActCreFolWrkCrs,	// #206
	ActRcvFilWrkCrsCla,	// #207
	ActRenFolWrkCrs,	// #208
	ActRemFilWrkCrs,	// #209
	ActRemFolWrkCrs,	// #210
	ActModIteSyl,		// #211
	ActUnk,			// #212 (obsolete action)
	ActUnk,			// #213 (obsolete action)
	ActUnk,			// #214 (obsolete action)
	ActUnk,			// #215 (obsolete action)
	ActUnk,			// #216 (obsolete action)
	ActInsIteSyl,		// #217
	ActDelItmSyl,		// #218
	ActRcvPagFAQ,		// #219
	ActDwnIteSyl,		// #220
	ActUp_IteSyl,		// #221
	ActLftIteSyl,		// #222
	ActRgtIteSyl,		// #223
	ActRcvURLBib,		// #224
	ActUnk,			// #225 (obsolete action)
	ActUnk,			// #226 (obsolete action)
	ActUnk,			// #227 (obsolete action)
	ActRemFolWrkUsr,	// #228
	ActUnk,			// #229 (obsolete action)
	ActUnk,			// #230 (obsolete action)
	ActUnk,			// #231 (obsolete action)
	ActUnk,			// #232 (obsolete action)
	ActUnk,			// #233 (obsolete action)
	ActRcvURLFAQ,		// #234
	ActRcvURLAss,		// #235
	ActReqRemGrpTyp,	// #236
	ActRemGrpTyp,		// #237
	ActUnk,			// #238 (obsolete action)
	ActUnk,			// #239 (obsolete action)
	ActUnk,			// #240 (obsolete action)
	ActSeeForDegUsr,	// #241
	ActSeeForSWAUsr,	// #242
	ActSeeForDegTch,	// #243
	ActSeePstForSWAUsr,	// #244
	ActSeeForSWATch,	// #245
	ActSeePstForSWATch,	// #246
	ActRcvThrForDegTch,	// #247
	ActUnk,			// #248 (obsolete action)
	ActUnk,			// #249 (obsolete action)
	ActUnk,			// #250 (obsolete action)
	ActUnk,			// #251 (obsolete action)
	ActRcvThrForDegUsr,	// #252
	ActUnk,			// #253 (obsolete action)
	ActUnk,			// #254 (obsolete action)
	ActSeePstForDegUsr,	// #255
	ActUnk,			// #256 (obsolete action)
	ActUnk,			// #257 (obsolete action)
	ActRcvThrForSWAUsr,	// #258
	ActRcvThrForSWATch,	// #259
	ActUnk,			// #260 (obsolete action)
	ActUnk,			// #261 (obsolete action)
	ActUnk,			// #262 (obsolete action)
	ActUnk,			// #263 (obsolete action)
	ActUnk,			// #264 (obsolete action)
	ActUnk,			// #265 (obsolete action)
	ActUnk,			// #266 (obsolete action)
	ActUnk,			// #267 (obsolete action)
	ActUnk,			// #268 (obsolete action)
	ActUnk,			// #269 (obsolete action)
	ActUnk,			// #270 (obsolete action)
	ActUnk,			// #271 (obsolete action)
	ActUnk,			// #272 (obsolete action)
	ActUnk,			// #273 (obsolete action)
	ActUnk,			// #274 (obsolete action)
	ActUnk,			// #275 (obsolete action)
	ActRemTreBrf,		// #276
	ActUnk,			// #277 (obsolete action)
	ActRemTreWrkUsr,	// #278
	ActRemTreWrkCrs,	// #279
	ActUnk,			// #280 (obsolete action)
	ActUnk,			// #281 (obsolete action)
	ActUnk,			// #282 (obsolete action)
	ActUnk,			// #283 (obsolete action)
	ActAdmMrkCrs,		// #284
	ActReqEdiRecSha,	// #285
	ActReqRemFilBrf,	// #286
	ActUnk,			// #287 (obsolete action)
	ActReqRemFilWrkUsr,	// #288
	ActReqRemFilWrkCrs,	// #289
	ActUnk,			// #290 (obsolete action)
	ActSeePstForDegTch,	// #291
	ActEdiRecFie,		// #292
	ActNewFie,		// #293
	ActReqRemFie,		// #294
	ActRemFie,		// #295
	ActRenFie,		// #296
	ActChgVisFie,		// #297
	ActChgMyData,		// #298
	ActUnk,			// #299 (obsolete action)
	ActRcvRecOthUsr,	// #300
	ActRcvRecCrs,		// #301
	ActChgMulGrpTyp,	// #302
	ActChgMdtGrpTyp,	// #303
	ActRenGrpTyp,		// #304
	ActChgRowFie,		// #305
	ActUnk,			// #306 (obsolete action)
	ActUnk,			// #307 (obsolete action)
	ActUnk,			// #308 (obsolete action)
	ActUnk,			// #309 (obsolete action)
	ActUnk,			// #310 (obsolete action)
	ActCopBrf,		// #311
	ActCopWrkCrs,		// #312
	ActUnk,			// #313 (obsolete action)
	ActCopWrkUsr,		// #314
	ActPasBrf,		// #315
	ActUnk,			// #316 (obsolete action)
	ActUnk,			// #317 (obsolete action)
	ActPasWrkUsr,		// #318
	ActPasWrkCrs,		// #319
	ActUnk,			// #320 (obsolete action)
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
	ActUnk,			// #343 (obsolete action)
	ActUnk,			// #344 (obsolete action)
	ActSeeForCrsUsr,	// #345
	ActSeePstForCrsUsr,	// #346
	ActSeePstForCrsTch,	// #347
	ActSeePstForCtrUsr,	// #348
	ActUnk,			// #349 (obsolete action)
	ActRcvThrForCrsUsr,	// #350
	ActUnk,			// #351 (obsolete action)
	ActUnk,			// #352 (obsolete action)
	ActUnk,			// #353 (obsolete action)
	ActUnk,			// #354 (obsolete action)
	ActUnk,			// #355 (obsolete action)
	ActUnk,			// #356 (obsolete action)
	ActUnk,			// #357 (obsolete action)
	ActUnk,			// #358 (obsolete action)
	ActUnk,			// #359 (obsolete action)
	ActUnk,			// #360 (obsolete action)
	ActUnk,			// #361 (obsolete action)
	ActUnk,			// #362 (obsolete action)
	ActUnk,			// #363 (obsolete action)
	ActUnk,			// #364 (obsolete action)
	ActUnk,			// #365 (obsolete action)
	ActUnk,			// #366 (obsolete action)
	ActUnk,			// #367 (obsolete action)
	ActUnk,			// #368 (obsolete action)
	ActUnk,			// #369 (obsolete action)
	ActSelInfSrcBib,	// #370
	ActUnk,			// #371 (obsolete action)
	ActEditorSyl,		// #372
	ActUnk,			// #373 (obsolete action)
	ActUnk,			// #374 (obsolete action)
	ActUnk,			// #375 (obsolete action)
	ActEditorBib,		// #376
	ActPlaTxtEdiBib,	// #377
	ActSelInfSrcSyl,	// #378
	ActPlaTxtEdiSyl,	// #379
	ActSelInfSrcFAQ,	// #380
	ActRcvPagSyl,		// #381
	ActUnk,			// #382 (obsolete action)
	ActUnk,			// #383 (obsolete action)
	ActSelInfSrcAss,	// #384
	ActSelInfSrcCrsLnk,	// #385
	ActEditorAss,		// #386
	ActPlaTxtEdiAss,	// #387
	ActEditorCrsLnk,	// #388
	ActUnk,			// #389 (obsolete action)
	ActUnk,			// #390 (obsolete action)
	ActUnk,			// #391 (obsolete action)
	ActUnk,			// #392 (obsolete action)
	ActUnk,			// #393 (obsolete action)
	ActRcvPlaTxtSyl,	// #394
	ActUnk,			// #395 (obsolete action)
	ActUnk,			// #396 (obsolete action)
	ActRcvPlaTxtAss,	// #397
	ActRcvPlaTxtBib,	// #398
	ActUnk,			// #399 (obsolete action)
	ActPlaTxtEdiCrsLnk,	// #400
	ActRcvPlaTxtCrsLnk,	// #401
	ActUnk,			// #402 (obsolete action)
	ActRcvURLSyl,		// #403
	ActEditorFAQ,		// #404
	ActPlaTxtEdiFAQ,	// #405
	ActRcvPlaTxtFAQ,	// #406
	ActUnk,			// #407 (obsolete action)
	ActSeeMyTT,		// #408
	ActPrnMyTT,		// #409
	ActExpBrf,		// #410
	ActConBrf,		// #411
	ActUnk,			// #412 (obsolete action)
	ActUnk,			// #413 (obsolete action)
	ActUnk,			// #414 (obsolete action)
	ActUnk,			// #415 (obsolete action)
	ActExpWrkCrs,		// #416
	ActUnk,			// #417 (obsolete action)
	ActUnk,			// #418 (obsolete action)
	ActUnk,			// #419 (obsolete action)
	ActUnk,			// #420 (obsolete action)
	ActExpShaCrs,		// #421
	ActConShaCrs,		// #422
	ActExpWrkUsr,		// #423
	ActConWrkCrs,		// #424
	ActConWrkUsr,		// #425
	ActConShaGrp,		// #426
	ActExpShaGrp,		// #427
	ActRemMyPho,		// #428
	ActUnk,			// #429 (obsolete action)
	ActSeeForCtrTch,	// #430
	ActSeeForCrsTch,	// #431
	ActUnk,			// #432 (obsolete action)
	ActUnk,			// #433 (obsolete action)
	ActDelAllSntMsg,	// #434
	ActUnk,			// #435 (obsolete action)
	ActDelAllRcvMsg,	// #436
	ActUnk,			// #437 (obsolete action)
	ActUnk,			// #438 (obsolete action)
	ActUnk,			// #439 (obsolete action)
	ActUnk,			// #440 (obsolete action)
	ActUnk,			// #441 (obsolete action)
	ActUnk,			// #442 (obsolete action)
	ActPrnTchPho,		// #443
	ActCalPhoDeg,		// #444
	ActUnk,			// #445 (obsolete action)
	ActUnk,			// #446 (obsolete action)
	ActSeePhoDeg,		// #447
	ActPrnPhoDeg,		// #448
	ActUnk,			// #449 (obsolete action)
	ActUnk,			// #450 (obsolete action)
	ActCfgTst,		// #451
	ActDisTag,		// #452
	ActEnaTag,		// #453
	ActRcvCfgTst,		// #454
	ActChgShfTstQst,	// #455
	ActUnk,			// #456 (obsolete action)
	ActUnk,			// #457 (obsolete action)
	ActUnk,			// #458 (obsolete action)
	ActUnk,			// #459 (obsolete action)
	ActUnk,			// #460 (obsolete action)
	ActAdmShaCrsGrp,	// #461
	ActExpSeeDocCrs,	// #462
	ActUnk,			// #463 (obsolete action)
	ActUnhDocCrs,		// #464
	ActHidDocCrs,		// #465
	ActUnk,			// #466 (obsolete action)
	ActUnk,			// #467 (obsolete action)
	ActFrmCreDocGrp,	// #468
	ActCreFolDocGrp,	// #469
	ActCopDocCrs,		// #470
	ActPasDocGrp,		// #471
	ActCopDocGrp,		// #472
	ActReqRemFilDocGrp,	// #473
	ActRemFilDocGrp,	// #474
	ActUnk,			// #475 (obsolete action)
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
	ActUnk,			// #499 (obsolete action)
	ActUnk,			// #500 (obsolete action)
	ActCopMrkCrs,		// #501
	ActPasMrkGrp,		// #502
	ActChgNumRowHeaCrs,	// #503
	ActChgNumRowFooCrs,	// #504
	ActUnk,			// #505 (obsolete action)
	ActCreFolMrkCrs,	// #506
	ActPasMrkCrs,		// #507
	ActUnk,			// #508 (obsolete action)
	ActRemFilMrkGrp,	// #509
	ActChgNumRowHeaGrp,	// #510
	ActChgNumRowFooGrp,	// #511
	ActUnk,			// #512 (obsolete action)
	ActCreFolMrkGrp,	// #513
	ActRcvFilMrkGrpCla,	// #514
	ActUnk,			// #515 (obsolete action)
	ActRcvFilMrkCrsCla,	// #516
	ActUnk,			// #517 (obsolete action)
	ActUnk,			// #518 (obsolete action)
	ActCopMrkGrp,		// #519
	ActRemFolMrkGrp,	// #520
	ActRemTreMrkGrp,	// #521
	ActUnk,			// #522 (obsolete action)
	ActSeeMyMrkCrs,		// #523
	ActSeeMyMrkGrp,		// #524
	ActUnk,			// #525 (obsolete action)
	ActUnk,			// #526 (obsolete action)
	ActConSeeMrkCrs,	// #527
	ActExpSeeMrkCrs,	// #528
	ActRenFolMrkGrp,	// #529
	ActRemFolMrkCrs,	// #530
	ActUnk,			// #531 (obsolete action)
	ActUnk,			// #532 (obsolete action)
	ActRemFilMrkCrs,	// #533
	ActRemTreMrkCrs,	// #534
	ActRenFolDocCrs,	// #535
	ActEdiDeg,		// #536
	ActNewDegTyp,		// #537
	ActRenDegTyp,		// #538
	ActUnk,			// #539 (obsolete action)
	ActNewDeg,		// #540
	ActUnk,			// #541 (obsolete action)
	ActRemDeg,		// #542
	ActUnk,			// #543 (obsolete action)
	ActChgDegTyp,		// #544
	ActRemDegTyp,		// #545
	ActRenDegSho,		// #546
	ActRenDegFul,		// #547
	ActUnk,			// #548 (obsolete action)
	ActUnk,			// #549 (obsolete action)
	ActUnk,			// #550 (obsolete action)
	ActUnk,			// #551 (obsolete action)
	ActUnk,			// #552
	ActRecDegLog,		// #553
	ActChgDegWWW,		// #554
	ActEdiCrs,		// #555
	ActNewCrs,		// #556
	ActUnk,			// #557 (obsolete action)
	ActUnk,			// #558 (obsolete action)
	ActUnk,			// #559 (obsolete action)
	ActRemCrs,		// #560
	ActChgCrsYea,		// #561
	ActUnk,			// #562
	ActRenCrsSho,		// #563
	ActRenCrsFul,		// #564
	ActUnk,			// #565 (obsolete action)
	ActUnk,			// #566 (obsolete action)
	ActUnk,			// #567 (obsolete action)
	ActUnk,			// #568 (obsolete action)
	ActUnk,			// #569 (obsolete action)
	ActUnk,			// #570 (obsolete action)
	ActUnk,			// #571 (obsolete action)
	ActUnk,			// #572 (obsolete action)
	ActEdiDegTyp,		// #573
	ActRenFolMrkCrs,	// #574
	ActUnk,			// #575 (obsolete action)
	ActUnk,			// #576 (obsolete action)
	ActUnk,			// #577 (obsolete action)
	ActLstTchAll,		// #578
	ActUnk,			// #579 (obsolete action)
	ActUnk,			// #580 (obsolete action)
	ActUnk,			// #581 (obsolete action)
	ActUnk,			// #582 (obsolete action)
	ActUnk,			// #583 (obsolete action)
	ActRemAdmDeg,		// #584
	ActUnk,			// #585 (obsolete action)
	ActNewAdmDeg,		// #586
	ActLstGst,		// #587
	ActUnk,			// #588 (obsolete action)
	ActChgMyRol,		// #589
	ActReqRemOldUsr,	// #590
	ActReqAccGbl,		// #591
	ActUnk,			// #592 (obsolete action)
	ActReqDelAllRcvMsg,	// #593
	ActReqAccCrs,		// #594
	ActReqRemFilMrkCrs,	// #595
	ActFrmCreMrkCrs,	// #596
	ActFrmCreBrf,		// #597
	ActUnk,			// #598 (obsolete action)
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
	ActUnk,			// #611 (obsolete action)
	ActUnk,			// #612 (obsolete action)
	ActDelPstForSWAUsr,	// #613
	ActUnk,			// #614 (obsolete action)
	ActDisPstForDegUsr,	// #615
	ActEnbPstForDegUsr,	// #616
	ActRcvRepForDegTch,	// #617
	ActDisPstForDegTch,	// #618
	ActEnbPstForDegTch,	// #619
	ActUnk,			// #620 (obsolete action)
	ActConAdmMrkCrs,	// #621
	ActRcvRepForSWATch,	// #622
	ActDelPstForSWATch,	// #623
	ActEnbPstForCrsUsr,	// #624
	ActDisPstForSWAUsr,	// #625
	ActUnk,			// #626 (obsolete action)
	ActReqSch,		// #627
	ActSch,			// #628
	ActUnk,			// #629 (obsolete action)
	ActUnk,			// #630 (obsolete action)
	ActExpAdmMrkGrp,	// #631
	ActEnbPstForSWAUsr,	// #632
	ActSndNewPwd,		// #633
	ActEnbPstForSWATch,	// #634
	ActDisPstForSWATch,	// #635
	ActUnk,			// #636 (obsolete action)
	ActUnk,			// #637 (obsolete action)
	ActUnk,			// #638 (obsolete action)
	ActUnk,			// #639 (obsolete action)
	ActUnk,			// #640 (obsolete action)
	ActUnk,			// #641 (obsolete action)
	ActUnk,			// #642 (obsolete action)
	ActUnk,			// #643 (obsolete action)
	ActUnk,			// #644 (obsolete action)
	ActAll,			// #645
	ActUnk,			// #646 (obsolete action)
	ActUnk,			// #647 (obsolete action)
	ActUnk,			// #648 (obsolete action)
	ActUnk,			// #649 (obsolete action)
	ActUnk,			// #650 (obsolete action)
	ActUnk,			// #651 (obsolete action)
	ActUnk,			// #652 (obsolete action)
	ActUnk,			// #653 (obsolete action)
	ActChgLan,		// #654
	ActUnk,			// #655 (obsolete action)
	ActUnk,			// #656 (obsolete action)
	ActUnk,			// #657 (obsolete action)
	ActUnk,			// #658 (obsolete action)
	ActUnk,			// #659 (obsolete action)
	ActUnk,			// #660 (obsolete action)
	ActUnk,			// #661 (obsolete action)
	ActUnk,			// #662 (obsolete action)
	ActExpRcvMsg,		// #663
	ActExpSntMsg,		// #664
	ActReqSndNewPwd,	// #665
	ActUnk,			// #666 (obsolete action)
	ActUnk,			// #667 (obsolete action)
	ActUnk,			// #668 (obsolete action)
	ActUnk,			// #669 (obsolete action)
	ActUnk,			// #670 (obsolete action)
	ActUnk,			// #671 (obsolete action)
	ActUnk,			// #672 (obsolete action)
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
	ActUnk,			// #692 (obsolete action)
	ActDetMyPho,		// #693
	ActUpdMyPho,		// #694
	ActUnk,			// #695 (obsolete action)
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
	ActUnk,			// #708 (obsolete action)
	ActUnk,			// #709 (obsolete action)
	ActUnk,			// #710 (obsolete action)
	ActUnk,			// #711 (obsolete action)
	ActUnk,			// #712 (obsolete action)
	ActEdiHld,		// #713
	ActNewHld,		// #714
	ActChgHldTyp,		// #715
	ActRemHld,		// #716
	ActChgHldStrDat,	// #717
	ActChgHldEndDat,	// #718
	ActUnk,			// #719 (obsolete action)
	ActUnk,			// #720 (obsolete action)
	ActChgDptIns,		// #721
	ActUnk,			// #722 (obsolete action)
	ActSeeForGenTch,	// #723
	ActSeeForInsTch,	// #724
	ActSeeForInsUsr,	// #725
	ActSeeForGenUsr,	// #726
	ActSeePstForGenUsr,	// #727
	ActUnk,			// #728 (obsolete action)
	ActUnk,			// #729 (obsolete action)
	ActSeePstForInsUsr,	// #730
	ActSeePstForGenTch,	// #731
	ActUnk,			// #732 (obsolete action)
	ActUnk,			// #733 (obsolete action)
	ActUnk,			// #734 (obsolete action)
	ActDelPstForGenUsr,	// #735
	ActRcvThrForGenUsr,	// #736
	ActRcvThrForInsUsr,	// #737
	ActUnk,			// #738 (obsolete action)
	ActUnk,			// #739 (obsolete action)
	ActRcvRepForInsUsr,	// #740
	ActUnk,			// #741 (obsolete action)
	ActUnk,			// #742 (obsolete action)
	ActDelPstForInsUsr,	// #743
	ActUnk,			// #744 (obsolete action)
	ActUnk,			// #745 (obsolete action)
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
	ActUnk,			// #757 (obsolete action)
	ActUnk,			// #758 (obsolete action)
	ActRemIns,		// #759
	ActUnk,			// #760 (obsolete action)
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
	ActUnk,			// #771 (obsolete action)
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
	ActUnk,			// #793 (obsolete action)
	ActUnk,			// #794 (obsolete action)
	ActUnk,			// #795 (obsolete action)
	ActUnk,			// #796 (obsolete action)
	ActUnk,			// #797 (obsolete action)
	ActUnk,			// #798 (obsolete action)
	ActUnk,			// #799 (obsolete action)
	ActUnk,			// #800 (obsolete action)
	ActSeeAllAsg,		// #801
	ActUnk,			// #802 (obsolete action)
	ActNewAsg,		// #803
	ActUnk,			// #804 (obsolete action)
	ActUnk,			// #805 (obsolete action)
	ActRemAsg,		// #806
	ActUnk,			// #807 (obsolete action)
	ActUnk,			// #808 (obsolete action)
	ActUnk,			// #809 (obsolete action)
	ActUnk,			// #810 (obsolete action)
	ActUnk,			// #811 (obsolete action)
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
	ActUnk,			// #842 (obsolete action)
	ActFrmRolSes,		// #843
	ActUnk,			// #844 (obsolete action)
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
	ActUnk,			// #865 (obsolete action)
	ActRenCty,		// #866
	ActReqDelThrCrsUsr,	// #867
	ActDelThrForCrsUsr,	// #868
	ActReqDelThrCrsTch,	// #869
	ActChgFrcReaTchGui,	// #870
	ActChgFrcReaSyl,	// #871
	ActUnk,			// #872 (obsolete action)
	ActChgFrcReaBib,	// #873
	ActChgFrcReaFAQ,	// #874
	ActChgFrcReaCrsLnk,	// #875
	ActDelThrForCrsTch,	// #876
	ActChgFrcReaCrsInf,	// #877
	ActChgHavReaCrsInf,	// #878
	ActChgHavReaFAQ,	// #879
	ActChgHavReaSyl,	// #880
	ActReqDelThrSWAUsr,	// #881
	ActDelThrForSWAUsr,	// #882
	ActChgFrcReaAss,	// #883
	ActChgHavReaBib,	// #884
	ActChgHavReaCrsLnk,	// #885
	ActChgHavReaTchGui,	// #886
	ActUnk,			// #887 (obsolete action)
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
	ActUnk,			// #967 (obsolete action)
	ActNewSvy,		// #968
	ActRemSvy,		// #969
	ActUnk,			// #970 (obsolete action)
	ActUnk,			// #971 (obsolete action)
	ActUnk,			// #972 (obsolete action)
	ActFrmNewSvy,		// #973
	ActEdiOneSvy,		// #974
	ActChgSvy,		// #975
	ActReqRemSvy,		// #976
	ActHidSvy,		// #977
	ActUnhSvy,		// #978
	ActEdiOneSvyQst,	// #979
	ActNewSvyQst,		// #980
	ActRemSvyQst,		// #981
	ActSeeOneSvy,		// #982
	ActAnsSvy,		// #983
	ActReqRstSvy,		// #984
	ActRstSvy,		// #985
	ActChgPlgAppKey,	// #986
	ActMyCrs,		// #987
	ActUnk,			// #988 (obsolete action)
	ActLstClk,		// #989
	ActSeeNtf,		// #990
	ActSeeNewNtf,		// #991
	ActReqChgLan,		// #992
	ActMFUAct,		// #993
	ActRefLstClk,		// #994
	ActLstCon,		// #995
	ActChgDatAdmDocCrs,	// #996
	ActUnk,			// #997 (obsolete action)
	ActChgDatAdmDocGrp,	// #998
	ActUnk,			// #999 (obsolete action)
	ActChgDatShaCrs,	// #1000
	ActUnk,			// #1001 (obsolete action)
	ActChgDatShaGrp,	// #1002
	ActUnk,			// #1003 (obsolete action)
	ActUnk,			// #1004 (obsolete action)
	ActUnk,			// #1005 (obsolete action)
	ActUnk,			// #1006 (obsolete action)
	ActReqImpTstQst,	// #1007
	ActImpTstQst,		// #1008
	ActSeeCrs,		// #1009
	ActUnk,			// #1010 (obsolete action)
	ActSeeDeg,		// #1011
	ActUnk,			// #1012 (obsolete action)
	ActSeeDegTyp,		// #1013
	ActUnk,			// #1014 (obsolete action)
	ActBanUsrMsg,		// #1015
	ActUnbUsrMsg,		// #1016
	ActLstBanUsr,		// #1017
	ActUnbUsrLst,		// #1018
	ActConRcvMsg,		// #1019
	ActConSntMsg,		// #1020
	ActUnk,			// #1021 (obsolete action)
	ActPrnUsrQR,		// #1022
	ActUnk,			// #1023 (obsolete action)
	ActChgInsCrsCodCfg,	// #1024
	ActChgInsCrsCod,	// #1025
	ActUnk,			// #1026 (obsolete action)
	ActUnk,			// #1027 (obsolete action)
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
	ActUnk,			// #1049 (obsolete action)
	ActUnk,			// #1050 (obsolete action)
	ActRecCtrLog,		// #1051
	ActUnk,			// #1052 (obsolete action)
	ActReqCrs,		// #1053
	ActReqSignUp,		// #1054
	ActChgCrsSta,		// #1055
	ActSignUp,		// #1056
	ActSeeSignUpReq,	// #1057
	ActReqRejSignUp,	// #1058
	ActRejSignUp,		// #1059
	ActSeePen,		// #1060
	ActChgTimGrpTyp,	// #1061
	ActUnk,			// #1062 (obsolete action)
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
	ActRchTxtEdiSyl,	// #1095
	ActUnk,			// #1096 (obsolete action)
	ActRchTxtEdiBib,	// #1097
	ActRchTxtEdiFAQ,	// #1098
	ActRchTxtEdiCrsLnk,	// #1099
	ActRchTxtEdiAss,	// #1100
	ActRcvRchTxtCrsInf,	// #1101
	ActRcvRchTxtTchGui,	// #1102
	ActRcvRchTxtSyl,	// #1103
	ActUnk,			// #1104 (obsolete action)
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
	ActUnk,			// #1162 (obsolete action)
	ActCreUsrAcc,		// #1163
	ActSeeOneNot,		// #1164
	ActUnk,			// #1165 (obsolete action)
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
	ActUnk,			// #1176 (obsolete action)
	ActUnk,			// #1177 (obsolete action)
	ActUnk,			// #1178 (obsolete action)
	ActUnk,			// #1179 (obsolete action)
	ActUnk,			// #1180 (obsolete action)
	ActUnk,			// #1181 (obsolete action)
	ActUnk,			// #1182 (obsolete action)
	ActUnk,			// #1183 (obsolete action)
	ActUnk,			// #1184 (obsolete action)
	ActUnk,			// #1185 (obsolete action)
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
	ActNewAnn,		// #1238
	ActUnk,			// #1239 (obsolete action)
	ActUnk,			// #1240 (obsolete action)
	ActUnk,			// #1241 (obsolete action)
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
	ActUnk,			// #1403 (obsolete action)
	ActChgBasPriPrf,	// #1404
	ActCalFig,		// #1405
	ActUnk,			// #1406 (obsolete action)
	ActUnk,			// #1407 (obsolete action)
	ActUnk,			// #1408 (obsolete action)
	ActUnk,			// #1409 (obsolete action)
	ActFolUsr,		// #1410
	ActUnfUsr,		// #1411
	ActSeeFlg,		// #1412
	ActSeeFlr,		// #1413
	ActReqMdfOneOth,	// #1414
	ActReqMdfOneStd,	// #1415
	ActReqMdfOneTch,	// #1416
	ActUnk,			// #1417 (obsolete action)
	ActReqMdfOth,		// #1418
	ActReqMdfStd,		// #1419
	ActReqMdfTch,		// #1420
	ActUnk,			// #1421 (obsolete action)
	ActUpdOth,		// #1422
	ActUpdStd,		// #1423
	ActUpdTch,		// #1424
	ActUnk,			// #1425 (obsolete action)
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
	ActUnk,			// #1447 (obsolete action)
	ActUnk,			// #1448 (obsolete action)
	ActUnk,			// #1449 (obsolete action)
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
	ActUnk,			// #1464 (obsolete action)
	ActUnk,			// #1465 (obsolete action)
	ActUnk,			// #1466 (obsolete action)
	ActChgPwdOth,		// #1467
	ActChgPwdStd,		// #1468
	ActChgPwdTch,		// #1469
	ActHidAnn,		// #1470
	ActUnhAnn,		// #1471
	ActReqRemNot,		// #1472
	ActSeeLstMyAtt,		// #1473
	ActPrnLstMyAtt,		// #1474
	ActUnk,			// #1475 (obsolete action)
	ActUnk,			// #1476 (obsolete action)
	ActUnk,			// #1477 (obsolete action)
	ActRemMaiOth,		// #1478
	ActRemMaiStd,		// #1479
	ActRemMaiTch,		// #1480
	ActChgMaiOth,		// #1481
	ActChgMaiStd,		// #1482
	ActChgMaiTch,		// #1483
	ActChg1stDay,		// #1484
	ActUnk,			// #1485 (obsolete action)
	ActChgCrsTT1stDay,	// #1486
	ActChgMyTT1stDay,	// #1487
	ActReqRemOldBrf,	// #1488
	ActRemOldBrf,		// #1489
	ActSeeGblTL,		// #1490
	ActUnk,			// #1491 (obsolete action)
	ActRcvPstGblTL,		// #1492
	ActRemPubGblTL,		// #1493
	ActReqRemPubGblTL,	// #1494
	ActShaNotGblTL,		// #1495
	ActUnsNotGblTL,		// #1496
	ActUnk,			// #1497 (obsolete action)
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
	ActUnk,			// #1565 (obsolete action)
	ActUnk,			// #1566 (obsolete action)
	ActUnk,			// #1567 (obsolete action)
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
	ActUnk,			// #1581 (obsolete action)
	ActSeeMyUsgRep,		// #1582
	ActUnk,			// #1583 (obsolete action)
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
	ActUnk,			// #1617 (obsolete action)
	ActPrnAgdQR,		// #1618
	ActReqRemCfe,		// #1619
	ActHidCfe,		// #1620
	ActUnhCfe,		// #1621
	ActSeeCal,		// #1622
	ActPrnCal,		// #1623
	ActChgCal1stDay,	// #1624
	ActUnk,			// #1625 (obsolete action)
	ActUnk,			// #1626 (obsolete action)
	ActUnk,			// #1627 (obsolete action)
	ActUnk,			// #1628 (obsolete action)
	ActUnk,			// #1629 (obsolete action)
	ActUnk,			// #1630 (obsolete action)
	ActUnk,			// #1631 (obsolete action)
	ActUnk,			// #1632 (obsolete action)
	ActUnk,			// #1633 (obsolete action)
	ActUnk,			// #1634 (obsolete action)
	ActUnk,			// #1635 (obsolete action)
	ActUnk,			// #1636 (obsolete action)
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
	ActUnk,			// #1658 (obsolete action)
	ActUnk,			// #1659 (obsolete action)
	ActHidGam,		// #1660
	ActUnhGam,		// #1661
	ActAddOneGamQst,	// #1662
	ActUnk,			// #1663 (obsolete action)
	ActReqRemGamQst,	// #1664
	ActRemGamQst,		// #1665
	ActGamLstTstQst,	// #1666
	ActAddTstQstToGam,	// #1667
	ActUp_GamQst,		// #1668
	ActDwnGamQst,		// #1669
	ActReqNewMch,		// #1670
	ActNewMch,		// #1671
	ActFwdMch,		// #1672
	ActUnk,			// #1673 (obsolete action)
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
	ActRemNicOth,		// #1738
	ActRemNicStd,		// #1739
	ActRemNicTch,		// #1740
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
	ActUnk,			// #1751 (obsolete action)
	ActChgGrpRoo,		// #1752
	Act_DoAct_OnSevGst,	// #1753
	Act_DoAct_OnSevStd,	// #1754
	Act_DoAct_OnSevTch,	// #1755
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
	ActUnk,			// #1779 (obsolete action)
	ActJoiMch,		// #1780
	ActUnk,			// #1781 (obsolete action)
	ActRefMchStd,		// #1782
        ActReqRemMch,		// #1783
        ActRemMch,		// #1784
	ActResMch,		// #1785
	ActUnk,			// #1786 (obsolete action)
	ActCreMyAcc,		// #1787
	ActRefMchTch,		// #1788
	ActPlyPauMch,		// #1789
	ActBckMch,		// #1790
	ActUnk,			// #1791 (obsolete action)
	ActUnk,			// #1792 (obsolete action)
	ActUnk,			// #1793 (obsolete action)
	ActChgVisResMchQst,	// #1794
	ActUnk,			// #1795
	ActSeeMyMchResCrs,	// #1796
	ActSeeOneMchResMe,	// #1797
	ActReqSeeUsrMchRes,	// #1798
	ActSeeUsrMchResCrs,	// #1799
	ActSeeOneMchResOth,	// #1800
	ActChgVisResMchUsr,	// #1801
	ActChgNumColMch,	// #1802
	ActCfgPrj,		// #1803
	ActChgNETCanCrePrj,	// #1804
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
	ActUnk,			// #1824
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
	ActUnk,			// #1854 (obsolete action)
	ActUnk,			// #1855 (obsolete action)
	ActUnk,			// #1856 (obsolete action)
	ActUnk,			// #1857 (obsolete action)
	ActUnk,			// #1858 (obsolete action)
	ActUnk,			// #1859 (obsolete action)
	ActUnk,			// #1860 (obsolete action)
	ActUnk,			// #1861 (obsolete action)
	ActUnk,			// #1862 (obsolete action)
	ActUnk,			// #1863 (obsolete action)
	ActUnk,			// #1864 (obsolete action)
	ActUnk,			// #1865 (obsolete action)
	ActUnk,			// #1866 (obsolete action)
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
	ActUnk,			// #1890 (obsolete action)
	ActUnk,			// #1891 (obsolete action)
	ActUnk,			// #1892 (obsolete action)
	ActReqRemExaSet,	// #1893
	ActRemExaSet,		// #1894
	ActUp_ExaSet,		// #1895
	ActDwnExaSet,		// #1896
	ActChgTitExaSet,	// #1897
	ActNewExaSet,		// #1898
	ActChgNumQstExaSet,	// #1899
	ActHidExaSes,		// #1900
	ActUnhExaSes,		// #1901
	ActReqChgExaSes,	// #1902
	ActChgExaSes,		// #1903
	ActSeeExaPrn,		// #1904
	ActUnk,			// #1905 (obsolete action)
	ActAnsExaPrn,		// #1906
	ActEdiTag,		// #1907
	ActEndExaPrn,		// #1908
	ActValSetQst,		// #1909
	ActInvSetQst,		// #1910
	ActChgRooMAC,		// #1911
	ActLstOneGam,		// #1912
	ActReqChgMch,		// #1913
	ActChgMch,		// #1914
	ActUnk,			// #1915 (obsolete action)
	ActUnk,			// #1916 (obsolete action)
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
	ActChgTstQst,		// #1975
	ActChgSvyQst,		// #1976
	ActChgRubPrj,		// #1977
	ActOwrClpSevGst,	// #1978
	ActOwrClpSevStd,	// #1979
	ActOwrClpSevTch,	// #1980
	ActRemClpGst,		// #1981
	ActRemClpStd,		// #1982
	ActRemClpTch,		// #1983
	ActSeeCliGst,		// #1984
	ActSeeCliStd,		// #1985
	ActSeeCliTch,		// #1986
	ActAddClpSevGst,	// #1987
	ActAddClpSevStd,	// #1988
	ActAddClpSevTch,	// #1989
	ActReqLnkTst,		// #1990
	ActReqTstOneTag,	// #1991
	ActReqLnkTag,		// #1992
	ActReqLnkAllGrp,		// #1993
	ActReqLnkGrpTyp,	// #1994
	ActReqSelOneGrpTyp,	// #1995
  };
