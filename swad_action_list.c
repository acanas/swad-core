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
   [ActAll		] = { 645,-1,TabUnk,ActAll		,{0x3C7,0x3F8},0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_1ST_TAB,NULL			,NULL				},	// Used for statistics
   [ActUnk		] = { 194,-1,TabUnk,ActUnk		,{0x3C7,0x3F8},0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_1ST_TAB,NULL			,NULL				},
   [ActMnu		] = {   2,-1,TabUnk,ActMnu		,{0x3C7,0x3F8},0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_1ST_TAB,NULL			,NULL				},
   [ActRefCon		] = { 845,-1,TabUnk,ActRefCon		,{0x3C7,0x3F8},0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_AJAX_RF,NULL			,Lay_RefreshNotifsAndConnected	},
   [ActWebSvc		] = { 892,-1,TabUnk,ActWebSvc		,{0x3C7,0x3F8},0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_WEB_SVC,NULL			,API_WebService			},

   // TabStr ******************************************************************
   // Log in
   [ActFrmLogIn		] = {1521, 0,TabStr,ActFrmLogIn		,{0x001,    0},0x001,0x001,0x001,0x001,0x001,Act_CONT_NORM,Act_1ST_TAB,NULL			,Usr_WriteLandingPage		},
   [ActLogIn		] = {   6,-1,TabUnk,ActFrmLogIn		,{0x3C7,0x3F8},0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_1ST_TAB,NULL			,Usr_WelcomeUsr			},
   [ActLogInNew		] = {1585,-1,TabUnk,ActFrmLogIn		,{0x3C7,0x3F8},0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_1ST_TAB,NULL			,Usr_WelcomeUsr			},
   [ActLogInLan		] = {1077,-1,TabUnk,ActFrmLogIn		,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_1ST_TAB,NULL			,Usr_WelcomeUsr			},
   [ActAnnSee		] = {1234,-1,TabUnk,ActFrmLogIn		,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_1ST_TAB,NULL			,Ann_MarkAnnouncementAsSeen	},
   [ActReqSndNewPwd	] = { 665,-1,TabUnk,ActFrmLogIn		,{0x001,    0},0x001,0x001,0x001,0x001,0x001,Act_CONT_NORM,Act_1ST_TAB,NULL			,Pwd_ShowFormSendNewPwd		},
   [ActSndNewPwd	] = { 633,-1,TabUnk,ActFrmLogIn		,{0x001,    0},0x001,0x001,0x001,0x001,0x001,Act_CONT_NORM,Act_1ST_TAB,NULL			,Pwd_ChkIdLoginAndSendNewPwd	},
   [ActLogOut		] = {  10,-1,TabUnk,ActFrmLogIn		,{0x3C7,0x3F8},0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_1ST_TAB,Ses_CloseSession		,Usr_Logout			},

   // Search
   [ActReqSch		] = { 627, 1,TabStr,ActReqSch		,{0x3C7,0x3F8},0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_1ST_TAB,NULL			,Sch_ReqSysSearch		},
   [ActSch		] = { 628,-1,TabUnk,ActReqSch		,{0x3C7,0x3F8},0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_1ST_TAB,Sch_GetParsSearch	,Sch_SysSearch			},

   // Timeline
   [ActSeeGblTL		] = {1490, 2,TabStr,ActSeeGblTL		,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_1ST_TAB,TmlWho_GetParWho		,Tml_ShowTimelineGbl		},
   [ActRefNewPubGblTL	] = {1509,-1,TabUnk,ActSeeGblTL		,{0x3C7,0x3F8},0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_AJAX_RF,TmlWho_GetParWho		,Tml_RefreshNewTimelineGbl	},
   [ActRefOldPubGblTL	] = {1510,-1,TabUnk,ActSeeGblTL		,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_AJAX_NR,TmlWho_GetParWho		,Tml_RefreshOldTimelineGbl	},
   [ActRcvPstGblTL	] = {1492,-1,TabUnk,ActSeeGblTL		,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_DATA,Act_1ST_TAB,TmlWho_GetParWho		,TmlPst_ReceivePostGbl		},
   [ActRcvComGblTL	] = {1503,-1,TabUnk,ActSeeGblTL		,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_DATA,Act_1ST_TAB,TmlWho_GetParWho		,TmlCom_ReceiveCommGbl		},
   [ActShoHidComGblTL	] = {1806,-1,TabUnk,ActSeeGblTL		,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_AJAX_NR,NULL			,TmlCom_ShowHiddenCommsGbl	},
   [ActAllShaNotGblTL	] = {1766,-1,TabUnk,ActSeeGblTL		,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_AJAX_NR,NULL			,TmlSha_ShowAllSharersNoteGbl	},
   [ActAllFavNotGblTL	] = {1767,-1,TabUnk,ActSeeGblTL		,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_AJAX_NR,NULL			,TmlFav_ShowAllFaversNoteGbl	},
   [ActAllFavComGblTL	] = {1768,-1,TabUnk,ActSeeGblTL		,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_AJAX_NR,NULL			,TmlFav_ShowAllFaversComGbl	},
   [ActShaNotGblTL	] = {1495,-1,TabUnk,ActSeeGblTL		,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_AJAX_NR,NULL			,TmlSha_ShaNoteGbl		},
   [ActUnsNotGblTL	] = {1496,-1,TabUnk,ActSeeGblTL		,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_AJAX_NR,NULL			,TmlSha_UnsNoteGbl		},
   [ActFavNotGblTL	] = {1512,-1,TabUnk,ActSeeGblTL		,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_AJAX_NR,NULL			,TmlFav_FavNoteGbl		},
   [ActUnfNotGblTL	] = {1513,-1,TabUnk,ActSeeGblTL		,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_AJAX_NR,NULL			,TmlFav_UnfNoteGbl		},
   [ActFavComGblTL	] = {1516,-1,TabUnk,ActSeeGblTL		,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_AJAX_NR,NULL			,TmlFav_FavCommGbl		},
   [ActUnfComGblTL	] = {1517,-1,TabUnk,ActSeeGblTL		,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_AJAX_NR,NULL			,TmlFav_UnfCommGbl		},
   [ActReqRemPubGblTL	] = {1494,-1,TabUnk,ActSeeGblTL		,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_1ST_TAB,TmlWho_GetParWho		,TmlNot_ReqRemNoteGbl		},
   [ActRemPubGblTL	] = {1493,-1,TabUnk,ActSeeGblTL		,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_1ST_TAB,TmlWho_GetParWho		,TmlNot_RemoveNoteGbl		},
   [ActReqRemComGblTL	] = {1505,-1,TabUnk,ActSeeGblTL		,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_1ST_TAB,TmlWho_GetParWho		,TmlCom_ReqRemComGbl		},
   [ActRemComGblTL	] = {1507,-1,TabUnk,ActSeeGblTL		,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_1ST_TAB,TmlWho_GetParWho		,TmlCom_RemoveComGbl		},

   // Profiles
   [ActSeeSocPrf	] = {1520, 3,TabStr,ActSeeSocPrf	,{0x3C7,0x3F8},0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_1ST_TAB,NULL			,Prf_SeeSocialProfiles		},
   [ActReqOthPubPrf	] = {1401,-1,TabUnk,ActSeeSocPrf	,{0x3C7,0x3F8},0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_1ST_TAB,NULL			,Prf_ReqUserProfile		},
   [ActRefOldPubUsrTL	] = {1511,-1,TabUnk,ActSeeSocPrf	,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_AJAX_NR,NULL			,Tml_RefreshOldTimelineUsr	},
   [ActRcvPstUsrTL	] = {1498,-1,TabUnk,ActSeeSocPrf	,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_DATA,Act_1ST_TAB,NULL			,TmlPst_ReceivePostUsr		},
   [ActRcvComUsrTL	] = {1504,-1,TabUnk,ActSeeSocPrf	,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_DATA,Act_1ST_TAB,NULL			,TmlCom_ReceiveCommUsr		},
   [ActShoHidComUsrTL	] = {1807,-1,TabUnk,ActSeeSocPrf	,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_AJAX_NR,NULL			,TmlCom_ShowHiddenCommsUsr	},
   [ActAllShaNotUsrTL	] = {1769,-1,TabUnk,ActSeeSocPrf	,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_AJAX_NR,NULL			,TmlSha_ShowAllSharersNoteUsr	},
   [ActAllFavNotUsrTL	] = {1770,-1,TabUnk,ActSeeSocPrf	,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_AJAX_NR,NULL			,TmlFav_ShowAllFaversNoteUsr	},
   [ActAllFavComUsrTL	] = {1771,-1,TabUnk,ActSeeSocPrf	,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_AJAX_NR,NULL			,TmlFav_ShowAllFaversComUsr	},
   [ActShaNotUsrTL	] = {1499,-1,TabUnk,ActSeeSocPrf	,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_AJAX_NR,NULL			,TmlSha_ShaNoteUsr		},
   [ActUnsNotUsrTL	] = {1500,-1,TabUnk,ActSeeSocPrf	,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_AJAX_NR,NULL			,TmlSha_UnsNoteUsr		},
   [ActFavNotUsrTL	] = {1514,-1,TabUnk,ActSeeSocPrf	,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_AJAX_NR,NULL			,TmlFav_FavNoteUsr		},
   [ActUnfNotUsrTL	] = {1515,-1,TabUnk,ActSeeSocPrf	,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_AJAX_NR,NULL			,TmlFav_UnfNoteUsr		},
   [ActFavComUsrTL	] = {1518,-1,TabUnk,ActSeeSocPrf	,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_AJAX_NR,NULL			,TmlFav_FavCommUsr		},
   [ActUnfComUsrTL	] = {1519,-1,TabUnk,ActSeeSocPrf	,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_AJAX_NR,NULL			,TmlFav_UnfCommUsr		},
   [ActReqRemPubUsrTL	] = {1501,-1,TabUnk,ActSeeSocPrf	,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_1ST_TAB,NULL			,TmlNot_ReqRemNoteUsr		},
   [ActRemPubUsrTL	] = {1502,-1,TabUnk,ActSeeSocPrf	,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_1ST_TAB,NULL			,TmlNot_RemoveNoteUsr		},
   [ActReqRemComUsrTL	] = {1506,-1,TabUnk,ActSeeSocPrf	,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_1ST_TAB,NULL			,TmlCom_ReqRemComUsr		},
   [ActRemComUsrTL	] = {1508,-1,TabUnk,ActSeeSocPrf	,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_1ST_TAB,NULL			,TmlCom_RemoveComUsr		},
   [ActSeeOthPubPrf	] = {1402,-1,TabUnk,ActSeeSocPrf	,{0x3C7,0x3F8},0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_1ST_TAB,NULL			,Prf_GetUsrDatAndShowUserProfile},
   [ActCalFig		] = {1405,-1,TabUnk,ActSeeSocPrf	,{0x3C7,0x3F8},0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_1ST_TAB,NULL			,Prf_CalculateFigures		},
   [ActFolUsr		] = {1410,-1,TabUnk,ActSeeSocPrf	,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_1ST_TAB,Fol_FollowUsr1		,Fol_FollowUsr2			},
   [ActUnfUsr		] = {1411,-1,TabUnk,ActSeeSocPrf	,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_1ST_TAB,Fol_UnfollowUsr1		,Fol_UnfollowUsr2		},
   [ActSeeFlg		] = {1412,-1,TabUnk,ActSeeSocPrf	,{0x3C7,0x3F8},0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_1ST_TAB,NULL			,Fol_ListFollowing		},
   [ActSeeFlr		] = {1413,-1,TabUnk,ActSeeSocPrf	,{0x3C7,0x3F8},0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_1ST_TAB,NULL			,Fol_ListFollowers		},
   [ActSeeUsrAgd	] = {1611,-1,TabUnk,ActSeeSocPrf	,{0x3C4,0x3F8},0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_1ST_TAB,NULL			,Agd_ShowUsrAgenda		},

   // Calendar
   [ActSeeCal		] = {1622, 4,TabStr,ActSeeCal		,{0x3C7,0x3F8},0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_1ST_TAB,NULL			,Cal_ShowCalendar		},
   [ActPrnCal		] = {1623,-1,TabUnk,ActSeeCal		,{0x3C7,0x3F8},0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_NEW_TAB,NULL			,Cal_PrintCalendar		},
   [ActChgCal1stDay	] = {1624,-1,TabUnk,ActSeeCal		,{0x3C7,0x3F8},0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_1ST_TAB,Cal_ChangeFirstDayOfWeek	,Cal_ShowCalendar		},

   // Notifications
   [ActSeeNtf         	] = { 990, 5,TabStr,ActSeeNtf		,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_1ST_TAB,NULL			,Ntf_ShowMyNotifications	},
   [ActSeeNewNtf	] = { 991,-1,TabUnk,ActSeeNtf		,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_1ST_TAB,NULL			,Ntf_ShowMyNotifications	},
   [ActMrkNtfSee	] = {1146,-1,TabUnk,ActSeeNtf		,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_1ST_TAB,NULL			,Ntf_MarkAllNotifAsSeen		},
   [ActSeeMai		] = { 855,-1,TabUnk,ActSeeNtf		,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_1ST_TAB,NULL			,Mai_SeeMailDomains		},
   [ActEdiMai		] = { 856,-1,TabUnk,ActSeeNtf		,{0x200,0x200},0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_1ST_TAB,NULL			,Mai_EditMailDomains		},
   [ActNewMai		] = { 857,-1,TabUnk,ActSeeNtf		,{0x200,0x200},0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_1ST_TAB,Mai_ReceiveNewMailDomain	,Mai_ContEditAfterChgMai	},
   [ActRemMai		] = { 860,-1,TabUnk,ActSeeNtf		,{0x200,0x200},0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_1ST_TAB,Mai_RemoveMailDomain	,Mai_ContEditAfterChgMai	},
   [ActRenMaiSho	] = { 858,-1,TabUnk,ActSeeNtf		,{0x200,0x200},0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_1ST_TAB,Mai_RenameMailDomainShort,Mai_ContEditAfterChgMai	},
   [ActRenMaiFul	] = { 859,-1,TabUnk,ActSeeNtf		,{0x200,0x200},0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_1ST_TAB,Mai_RenameMailDomainFull	,Mai_ContEditAfterChgMai	},

   // TabSys ******************************************************************
   // System info
   [ActSeeSysInf	] = {1818, 0,TabSys,ActSeeSysInf	,{    0,    0},    0,    0,    0,    0,0x3C7,Act_CONT_NORM,Act_1ST_TAB,NULL			,SysCfg_ShowConfiguration	},
   [ActPrnSysInf	] = {1819,-1,TabUnk,ActSeeSysInf	,{    0,    0},    0,    0,    0,    0,0x3C7,Act_CONT_NORM,Act_NEW_TAB,NULL			,SysCfg_PrintConfiguration	},

   // Countries
   [ActSeeCty		] = { 862, 1,TabSys,ActSeeCty		,{    0,    0},    0,    0,    0,    0,0x3C7,Act_CONT_NORM,Act_1ST_TAB,Cty_ListCountries1	,Cty_ListCountries2		},
   [ActEdiCty		] = { 863,-1,TabUnk,ActSeeCty		,{    0,    0},    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_1ST_TAB,NULL			,Cty_EditCountries		},
   [ActNewCty		] = { 864,-1,TabUnk,ActSeeCty		,{    0,    0},    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_1ST_TAB,Cty_ReceiveNewCountry	,Cty_ContEditAfterChgCty	},
   [ActRemCty		] = { 893,-1,TabUnk,ActSeeCty		,{    0,    0},    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_1ST_TAB,Cty_RemoveCountry	,Cty_ContEditAfterChgCty	},
   [ActRenCty		] = { 866,-1,TabUnk,ActSeeCty		,{    0,    0},    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_1ST_TAB,Cty_RenameCountry	,Cty_ContEditAfterChgCty	},
   [ActChgCtyWWW	] = {1157,-1,TabUnk,ActSeeCty		,{    0,    0},    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_1ST_TAB,Cty_ChangeCtyWWW		,Cty_ContEditAfterChgCty	},

   // Pending
   [ActSeePen		] = {1060, 2,TabSys,ActSeePen		,{    0,    0},    0,    0,    0,    0,0x3C0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Hie_SeePending			},

   // Links
   [ActSeeLnk 		] = { 748, 3,TabSys,ActSeeLnk		,{    0,    0},    0,    0,    0,    0,0x3C7,Act_CONT_NORM,Act_1ST_TAB,NULL			,Lnk_SeeLinks			},
   [ActEdiLnk		] = { 749,-1,TabUnk,ActSeeLnk		,{    0,    0},    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_1ST_TAB,NULL			,Lnk_EditLinks			},
   [ActNewLnk		] = { 750,-1,TabUnk,ActSeeLnk		,{    0,    0},    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_1ST_TAB,Lnk_ReceiveNewLink	,Lnk_ContEditAfterChgLnk	},
   [ActRemLnk		] = { 897,-1,TabUnk,ActSeeLnk		,{    0,    0},    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_1ST_TAB,Lnk_RemoveLink		,Lnk_ContEditAfterChgLnk	},
   [ActRenLnkSho	] = { 753,-1,TabUnk,ActSeeLnk		,{    0,    0},    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_1ST_TAB,Lnk_RenameLinkShort	,Lnk_ContEditAfterChgLnk	},
   [ActRenLnkFul	] = { 751,-1,TabUnk,ActSeeLnk		,{    0,    0},    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_1ST_TAB,Lnk_RenameLinkFull	,Lnk_ContEditAfterChgLnk	},
   [ActChgLnkWWW	] = { 752,-1,TabUnk,ActSeeLnk		,{    0,    0},    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_1ST_TAB,Lnk_ChangeLinkWWW	,Lnk_ContEditAfterChgLnk	},
   [ActSeeBan		] = {1137,-1,TabUnk,ActSeeLnk		,{    0,    0},    0,    0,    0,    0,0x3FF,Act_CONT_NORM,Act_1ST_TAB,NULL			,Ban_ShowAllBanners		},
   [ActEdiBan		] = {1138,-1,TabUnk,ActSeeLnk		,{    0,    0},    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_1ST_TAB,NULL			,Ban_EditBanners		},
   [ActNewBan		] = {1139,-1,TabUnk,ActSeeLnk		,{    0,    0},    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_1ST_TAB,Ban_ReceiveNewBanner	,Ban_ContEditAfterChgBan	},
   [ActRemBan		] = {1140,-1,TabUnk,ActSeeLnk		,{    0,    0},    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_1ST_TAB,Ban_RemoveBanner		,Ban_ContEditAfterChgBan	},
   [ActUnhBan		] = {1212,-1,TabUnk,ActSeeLnk		,{    0,    0},    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_1ST_TAB,Ban_UnhideBanner		,Ban_ContEditAfterChgBan	},
   [ActHidBan		] = {1213,-1,TabUnk,ActSeeLnk		,{    0,    0},    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_1ST_TAB,Ban_HideBanner		,Ban_ContEditAfterChgBan	},
   [ActRenBanSho	] = {1141,-1,TabUnk,ActSeeLnk		,{    0,    0},    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_1ST_TAB,Ban_RenameBannerShort	,Ban_ContEditAfterChgBan	},
   [ActRenBanFul	] = {1142,-1,TabUnk,ActSeeLnk		,{    0,    0},    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_1ST_TAB,Ban_RenameBannerFull	,Ban_ContEditAfterChgBan	},
   [ActChgBanImg	] = {1144,-1,TabUnk,ActSeeLnk		,{    0,    0},    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_1ST_TAB,Ban_ChangeBannerImg	,Ban_ContEditAfterChgBan	},
   [ActChgBanWWW	] = {1143,-1,TabUnk,ActSeeLnk		,{    0,    0},    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_1ST_TAB,Ban_ChangeBannerWWW	,Ban_ContEditAfterChgBan	},
   [ActClkBan		] = {1145,-1,TabUnk,ActSeeLnk		,{    0,    0},    0,    0,    0,    0,0x3C7,Act_CONT_NORM,Act_DWN_FIL,Ban_ClickOnBanner	,NULL				},

   // Plugins
   [ActSeePlg		] = { 777, 4,TabSys,ActSeePlg		,{    0,    0},    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_1ST_TAB,NULL			,Plg_ListPlugins		},
   [ActEdiPlg		] = { 778,-1,TabUnk,ActSeePlg		,{    0,    0},    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_1ST_TAB,NULL			,Plg_EditPlugins		},
   [ActNewPlg		] = { 779,-1,TabUnk,ActSeePlg		,{    0,    0},    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_1ST_TAB,Plg_ReceiveNewPlg	,Plg_ContEditAfterChgPlg	},
   [ActRemPlg		] = { 889,-1,TabUnk,ActSeePlg		,{    0,    0},    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_1ST_TAB,Plg_RemovePlugin		,Plg_ContEditAfterChgPlg	},
   [ActRenPlg		] = { 782,-1,TabUnk,ActSeePlg		,{    0,    0},    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_1ST_TAB,Plg_RenamePlugin		,Plg_ContEditAfterChgPlg	},
   [ActChgPlgDes	] = { 888,-1,TabUnk,ActSeePlg		,{    0,    0},    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_1ST_TAB,Plg_ChangePlgDescription	,Plg_ContEditAfterChgPlg	},
   [ActChgPlgLog	] = { 781,-1,TabUnk,ActSeePlg		,{    0,    0},    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_1ST_TAB,Plg_ChangePlgLogo	,Plg_ContEditAfterChgPlg	},
   [ActChgPlgAppKey	] = { 986,-1,TabUnk,ActSeePlg		,{    0,    0},    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_1ST_TAB,Plg_ChangePlgAppKey	,Plg_ContEditAfterChgPlg	},
   [ActChgPlgURL	] = { 783,-1,TabUnk,ActSeePlg		,{    0,    0},    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_1ST_TAB,Plg_ChangePlgURL		,Plg_ContEditAfterChgPlg	},
   [ActChgPlgIP		] = { 780,-1,TabUnk,ActSeePlg		,{    0,    0},    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_1ST_TAB,Plg_ChangePlgIP		,Plg_ContEditAfterChgPlg	},

   // Maintenance
   [ActMtn		] = {1820, 5,TabSys,ActMtn		,{    0,    0},    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_1ST_TAB,NULL			,Mtn_Maintenance		},
   [ActSetUp		] = { 840,-1,TabUnk,ActMtn		,{    0,    0},    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_1ST_TAB,NULL			,Mtn_SetUp			},
   [ActReqRemOldCrs	] = {1109,-1,TabUnk,ActMtn		,{    0,    0},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Mtn_RemoveOldCrss		},
   [ActRemOldCrs	] = {1110,-1,TabUnk,ActMtn		,{    0,    0},    0,    0,    0,    0,0x200,Act_CONT_NORM,Act_1ST_TAB,NULL			,Crs_RemoveOldCrss		},

   // TabCty ******************************************************************
   // Country info
   [ActSeeCtyInf	] = {1155, 0,TabCty,ActSeeCtyInf	,{    0,    0},    0,    0,    0,0x3C7,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,CtyCfg_ShowConfiguration	},
   [ActPrnCtyInf	] = {1156,-1,TabUnk,ActSeeCtyInf	,{    0,    0},    0,    0,    0,0x3C7,    0,Act_CONT_NORM,Act_NEW_TAB,NULL			,CtyCfg_PrintConfiguration	},
   [ActChgCtyMapAtt	] = {1158,-1,TabUnk,ActSeeCtyInf	,{    0,    0},    0,    0,    0,0x200,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,CtyCfg_ChangeCtyMapAttr	},

   // Institutions
   [ActSeeIns		] = { 696, 1,TabCty,ActSeeIns		,{    0,    0},    0,    0,    0,0x3C7,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Ins_ShowInssOfCurrentCty	},
   [ActEdiIns		] = { 697,-1,TabUnk,ActSeeIns		,{    0,    0},    0,    0,    0,0x3C6,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Ins_EditInstitutions		},
   [ActReqIns		] = {1210,-1,TabUnk,ActSeeIns		,{    0,    0},    0,    0,    0,0x3C6,    0,Act_CONT_NORM,Act_1ST_TAB,Ins_ReceiveReqIns	,Ins_ContEditAfterChgIns	},
   [ActNewIns		] = { 698,-1,TabUnk,ActSeeIns		,{    0,    0},    0,    0,    0,0x200,    0,Act_CONT_NORM,Act_1ST_TAB,Ins_ReceiveNewIns	,Ins_ContEditAfterChgIns	},
   [ActRemIns		] = { 759,-1,TabUnk,ActSeeIns		,{    0,    0},    0,    0,    0,0x3C6,    0,Act_CONT_NORM,Act_1ST_TAB,Ins_RemoveInstitution	,Ins_ContEditAfterChgIns	},
   [ActRenInsSho	] = { 702,-1,TabUnk,ActSeeIns		,{    0,    0},    0,    0,    0,0x3C6,    0,Act_CONT_NORM,Act_1ST_TAB,Ins_RenameInsShort	,Ins_ContEditAfterChgIns	},
   [ActRenInsFul	] = { 701,-1,TabUnk,ActSeeIns		,{    0,    0},    0,    0,    0,0x3C6,    0,Act_CONT_NORM,Act_1ST_TAB,Ins_RenameInsFull	,Ins_ContEditAfterChgIns	},
   [ActChgInsWWW	] = { 700,-1,TabUnk,ActSeeIns		,{    0,    0},    0,    0,    0,0x3C6,    0,Act_CONT_NORM,Act_1ST_TAB,Ins_ChangeInsWWW		,Ins_ContEditAfterChgIns	},
   [ActChgInsSta	] = {1211,-1,TabUnk,ActSeeIns		,{    0,    0},    0,    0,    0,0x3C6,    0,Act_CONT_NORM,Act_1ST_TAB,Ins_ChangeInsStatus	,Ins_ContEditAfterChgIns	},

   // TabIns ******************************************************************
   // Institution info
   [ActSeeInsInf	] = {1153, 0,TabIns,ActSeeInsInf	,{    0,    0},    0,    0,0x3C7,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,InsCfg_ShowConfiguration	},
   [ActPrnInsInf	] = {1154,-1,TabUnk,ActSeeInsInf	,{    0,    0},    0,    0,0x3C7,    0,    0,Act_CONT_NORM,Act_NEW_TAB,NULL			,InsCfg_PrintConfiguration	},
   [ActChgInsCtyCfg	] = {1590,-1,TabUnk,ActSeeInsInf	,{    0,    0},    0,    0,0x200,    0,    0,Act_CONT_NORM,Act_1ST_TAB,InsCfg_ChangeInsCty	,InsCfg_ContEditAfterChgIns	},
   [ActRenInsShoCfg	] = {1592,-1,TabUnk,ActSeeInsInf	,{    0,    0},    0,    0,0x200,    0,    0,Act_CONT_NORM,Act_1ST_TAB,InsCfg_RenameInsShort	,InsCfg_ContEditAfterChgIns	},
   [ActRenInsFulCfg	] = {1591,-1,TabUnk,ActSeeInsInf	,{    0,    0},    0,    0,0x200,    0,    0,Act_CONT_NORM,Act_1ST_TAB,InsCfg_RenameInsFull	,InsCfg_ContEditAfterChgIns	},
   [ActChgInsWWWCfg	] = {1593,-1,TabUnk,ActSeeInsInf	,{    0,    0},    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,InsCfg_ChangeInsWWW		},
   [ActReqInsLog	] = {1245,-1,TabUnk,ActSeeInsInf	,{    0,    0},    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,InsCfg_ReqLogo			},
   [ActRecInsLog	] = { 699,-1,TabUnk,ActSeeInsInf	,{    0,    0},    0,    0,0x300,    0,    0,Act_CONT_DATA,Act_1ST_TAB,InsCfg_ReceiveLogo	,InsCfg_ShowConfiguration	},
   [ActRemInsLog	] = {1341,-1,TabUnk,ActSeeInsInf	,{    0,    0},    0,    0,0x300,    0,    0,Act_CONT_DATA,Act_1ST_TAB,InsCfg_RemoveLogo	,InsCfg_ShowConfiguration	},

   // Centers
   [ActSeeCtr		] = { 676, 1,TabIns,ActSeeCtr		,{    0,    0},    0,    0,0x3C7,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Ctr_ShowCtrsOfCurrentIns	},
   [ActEdiCtr		] = { 681,-1,TabUnk,ActSeeCtr		,{    0,    0},    0,    0,0x3C6,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Ctr_EditCenters		},
   [ActReqCtr		] = {1208,-1,TabUnk,ActSeeCtr		,{    0,    0},    0,    0,0x3C6,    0,    0,Act_CONT_NORM,Act_1ST_TAB,Ctr_ReceiveReqCtr	,Ctr_ContEditAfterChgCtr	},
   [ActNewCtr		] = { 685,-1,TabUnk,ActSeeCtr		,{    0,    0},    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_1ST_TAB,Ctr_ReceiveNewCtr	,Ctr_ContEditAfterChgCtr	},
   [ActRemCtr		] = { 686,-1,TabUnk,ActSeeCtr		,{    0,    0},    0,    0,0x3C6,    0,    0,Act_CONT_NORM,Act_1ST_TAB,Ctr_RemoveCenter		,Ctr_ContEditAfterChgCtr	},
   [ActChgCtrPlc	] = { 706,-1,TabUnk,ActSeeCtr		,{    0,    0},    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_1ST_TAB,Ctr_ChangeCtrPlc		,Ctr_ContEditAfterChgCtr	},
   [ActRenCtrSho	] = { 682,-1,TabUnk,ActSeeCtr		,{    0,    0},    0,    0,0x3C6,    0,    0,Act_CONT_NORM,Act_1ST_TAB,Ctr_RenameCenterShort	,Ctr_ContEditAfterChgCtr	},
   [ActRenCtrFul	] = { 684,-1,TabUnk,ActSeeCtr		,{    0,    0},    0,    0,0x3C6,    0,    0,Act_CONT_NORM,Act_1ST_TAB,Ctr_RenameCenterFull	,Ctr_ContEditAfterChgCtr	},
   [ActChgCtrWWW	] = { 683,-1,TabUnk,ActSeeCtr		,{    0,    0},    0,    0,0x3C6,    0,    0,Act_CONT_NORM,Act_1ST_TAB,Ctr_ChangeCtrWWW		,Ctr_ContEditAfterChgCtr	},
   [ActChgCtrSta	] = {1209,-1,TabUnk,ActSeeCtr		,{    0,    0},    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_1ST_TAB,Ctr_ChangeCtrStatus	,Ctr_ContEditAfterChgCtr	},

   // Places
   [ActSeePlc		] = { 703, 2,TabIns,ActSeePlc		,{    0,    0},    0,    0,0x3C7,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Plc_SeeAllPlaces		},
   [ActEdiPlc		] = { 704,-1,TabUnk,ActSeePlc		,{    0,    0},    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Plc_EditPlaces			},
   [ActNewPlc		] = { 705,-1,TabUnk,ActSeePlc		,{    0,    0},    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_1ST_TAB,Plc_ReceiveNewPlace	,Plc_ContEditAfterChgPlc	},
   [ActRemPlc		] = { 776,-1,TabUnk,ActSeePlc		,{    0,    0},    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_1ST_TAB,Plc_RemovePlace		,Plc_ContEditAfterChgPlc	},
   [ActRenPlcSho	] = { 894,-1,TabUnk,ActSeePlc		,{    0,    0},    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_1ST_TAB,Plc_RenamePlaceShort	,Plc_ContEditAfterChgPlc	},
   [ActRenPlcFul	] = { 895,-1,TabUnk,ActSeePlc		,{    0,    0},    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_1ST_TAB,Plc_RenamePlaceFull	,Plc_ContEditAfterChgPlc	},

   // Departments
   [ActSeeDpt		] = { 675, 3,TabIns,ActSeeDpt		,{    0,    0},    0,    0,0x3C7,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Dpt_SeeAllDepts		},
   [ActEdiDpt		] = { 677,-1,TabUnk,ActSeeDpt		,{    0,    0},    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Dpt_EditDepartments		},
   [ActNewDpt		] = { 687,-1,TabUnk,ActSeeDpt		,{    0,    0},    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_1ST_TAB,Dpt_ReceiveNewDpt	,Dpt_ContEditAfterChgDpt	},
   [ActRemDpt		] = { 690,-1,TabUnk,ActSeeDpt		,{    0,    0},    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_1ST_TAB,Dpt_RemoveDepartment	,Dpt_ContEditAfterChgDpt	},
   [ActChgDptIns	] = { 721,-1,TabUnk,ActSeeDpt		,{    0,    0},    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_1ST_TAB,Dpt_ChangeDepartIns	,Dpt_ContEditAfterChgDpt	},
   [ActRenDptSho	] = { 688,-1,TabUnk,ActSeeDpt		,{    0,    0},    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_1ST_TAB,Dpt_RenameDepartShort	,Dpt_ContEditAfterChgDpt	},
   [ActRenDptFul	] = { 689,-1,TabUnk,ActSeeDpt		,{    0,    0},    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_1ST_TAB,Dpt_RenameDepartFull	,Dpt_ContEditAfterChgDpt	},
   [ActChgDptWWW	] = { 691,-1,TabUnk,ActSeeDpt		,{    0,    0},    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_1ST_TAB,Dpt_ChangeDptWWW		,Dpt_ContEditAfterChgDpt	},

   // Holidays
   [ActSeeHld		] = { 707, 4,TabIns,ActSeeHld		,{    0,    0},    0,    0,0x3C7,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Hld_SeeAllHolidays		},
   [ActEdiHld		] = { 713,-1,TabUnk,ActSeeHld		,{    0,    0},    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Hld_EditHolidays		},
   [ActNewHld		] = { 714,-1,TabUnk,ActSeeHld		,{    0,    0},    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_1ST_TAB,Hld_ReceiveNewHoliday	,Hld_ContEditAfterChgHld	},
   [ActRemHld		] = { 716,-1,TabUnk,ActSeeHld		,{    0,    0},    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_1ST_TAB,Hld_RemoveHoliday	,Hld_ContEditAfterChgHld	},
   [ActChgHldPlc	] = { 896,-1,TabUnk,ActSeeHld		,{    0,    0},    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_1ST_TAB,Hld_ChangeHolidayPlace	,Hld_ContEditAfterChgHld	},
   [ActChgHldTyp	] = { 715,-1,TabUnk,ActSeeHld		,{    0,    0},    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_1ST_TAB,Hld_ChangeHolidayType	,Hld_ContEditAfterChgHld	},
   [ActChgHldStrDat	] = { 717,-1,TabUnk,ActSeeHld		,{    0,    0},    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_1ST_TAB,Hld_ChangeStartDate	,Hld_ContEditAfterChgHld	},
   [ActChgHldEndDat	] = { 718,-1,TabUnk,ActSeeHld		,{    0,    0},    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_1ST_TAB,Hld_ChangeEndDate	,Hld_ContEditAfterChgHld	},
   [ActRenHld		] = { 766,-1,TabUnk,ActSeeHld		,{    0,    0},    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_1ST_TAB,Hld_RenameHoliday	,Hld_ContEditAfterChgHld	},

   // TabCtr ******************************************************************
   // Center info
   [ActSeeCtrInf	] = {1151, 0,TabCtr,ActSeeCtrInf	,{    0,    0},    0,0x3C7,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,CtrCfg_ShowConfiguration	},
   [ActPrnCtrInf	] = {1152,-1,TabUnk,ActSeeCtrInf	,{    0,    0},    0,0x3C7,    0,    0,    0,Act_CONT_NORM,Act_NEW_TAB,NULL			,CtrCfg_PrintConfiguration	},
   [ActChgCtrInsCfg	] = {1589,-1,TabUnk,ActSeeCtrInf	,{    0,    0},    0,0x200,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,CtrCfg_ChangeCtrIns	,CtrCfg_ContEditAfterChgCtr	},
   [ActRenCtrShoCfg	] = {1595,-1,TabUnk,ActSeeCtrInf	,{    0,    0},    0,0x300,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,CtrCfg_RenameCenterShort	,CtrCfg_ContEditAfterChgCtr	},
   [ActRenCtrFulCfg	] = {1594,-1,TabUnk,ActSeeCtrInf	,{    0,    0},    0,0x300,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,CtrCfg_RenameCenterFull	,CtrCfg_ContEditAfterChgCtr	},
   [ActChgCtrPlcCfg	] = {1648,-1,TabUnk,ActSeeCtrInf	,{    0,    0},    0,0x380,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,CtrCfg_ChangeCtrPlc		},
   [ActChgCtrLatCfg	] = {1815,-1,TabUnk,ActSeeCtrInf	,{    0,    0},    0,0x380,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,CtrCfg_ChangeCtrLatitude	},
   [ActChgCtrLgtCfg	] = {1816,-1,TabUnk,ActSeeCtrInf	,{    0,    0},    0,0x380,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,CtrCfg_ChangeCtrLongitude	},
   [ActChgCtrAltCfg	] = {1817,-1,TabUnk,ActSeeCtrInf	,{    0,    0},    0,0x380,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,CtrCfg_ChangeCtrAltitude	},
   [ActChgCtrWWWCfg	] = {1596,-1,TabUnk,ActSeeCtrInf	,{    0,    0},    0,0x380,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,CtrCfg_ChangeCtrWWW		},
   [ActReqCtrLog	] = {1244,-1,TabUnk,ActSeeCtrInf	,{    0,    0},    0,0x380,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,CtrCfg_ReqLogo			},
   [ActRecCtrLog	] = {1051,-1,TabUnk,ActSeeCtrInf	,{    0,    0},    0,0x380,    0,    0,    0,Act_CONT_DATA,Act_1ST_TAB,CtrCfg_ReceiveLogo	,CtrCfg_ShowConfiguration	},
   [ActRemCtrLog	] = {1342,-1,TabUnk,ActSeeCtrInf	,{    0,    0},    0,0x380,    0,    0,    0,Act_CONT_DATA,Act_1ST_TAB,CtrCfg_RemoveLogo	,CtrCfg_ShowConfiguration	},
   [ActReqCtrPho	] = {1160,-1,TabUnk,ActSeeCtrInf	,{    0,    0},    0,0x380,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,CtrCfg_ReqPhoto		},
   [ActRecCtrPho	] = {1161,-1,TabUnk,ActSeeCtrInf	,{    0,    0},    0,0x380,    0,    0,    0,Act_CONT_DATA,Act_1ST_TAB,NULL			,CtrCfg_ReceivePhoto		},
   [ActChgCtrPhoAtt	] = {1159,-1,TabUnk,ActSeeCtrInf	,{    0,    0},    0,0x380,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,CtrCfg_ChangeCtrPhotoAttr	},

   // Degrees
   [ActSeeDeg		] = {1011, 1,TabCtr,ActSeeDeg		,{    0,    0},    0,0x3C7,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Deg_ShowDegsOfCurrentCtr	},
   [ActEdiDeg		] = { 536,-1,TabUnk,ActSeeDeg		,{    0,    0},    0,0x3C6,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Deg_EditDegrees		},
   [ActReqDeg		] = {1206,-1,TabUnk,ActSeeDeg		,{    0,    0},    0,0x3C6,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,Deg_ReceiveReqDeg	,Deg_ContEditAfterChgDeg	},
   [ActNewDeg		] = { 540,-1,TabUnk,ActSeeDeg		,{    0,    0},    0,0x380,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,Deg_ReceiveNewDeg	,Deg_ContEditAfterChgDeg	},
   [ActRemDeg		] = { 542,-1,TabUnk,ActSeeDeg		,{    0,    0},    0,0x3C6,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,Deg_RemoveDegree		,Deg_ContEditAfterChgDeg	},
   [ActRenDegSho	] = { 546,-1,TabUnk,ActSeeDeg		,{    0,    0},    0,0x3C6,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,Deg_RenameDegreeShort	,Deg_ContEditAfterChgDeg	},
   [ActRenDegFul	] = { 547,-1,TabUnk,ActSeeDeg		,{    0,    0},    0,0x3C6,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,Deg_RenameDegreeFull	,Deg_ContEditAfterChgDeg	},
   [ActChgDegTyp	] = { 544,-1,TabUnk,ActSeeDeg		,{    0,    0},    0,0x3C6,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,Deg_ChangeDegreeType	,Deg_ContEditAfterChgDeg	},
   [ActChgDegWWW	] = { 554,-1,TabUnk,ActSeeDeg		,{    0,    0},    0,0x3C6,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,Deg_ChangeDegWWW		,Deg_ContEditAfterChgDeg	},
   [ActChgDegSta	] = {1207,-1,TabUnk,ActSeeDeg		,{    0,    0},    0,0x380,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,Deg_ChangeDegStatus	,Deg_ContEditAfterChgDeg	},
   [ActSeeDegTyp	] = {1013,-1,TabUnk,ActSeeDeg		,{    0,    0},    0,0x3C7,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,DegTyp_SeeDegreeTypesInDegTab	},
   [ActEdiDegTyp	] = { 573,-1,TabUnk,ActSeeDeg		,{    0,    0},    0,0x200,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,DegTyp_GetAndEditDegreeTypes	},
   [ActNewDegTyp	] = { 537,-1,TabUnk,ActSeeDeg		,{    0,    0},    0,0x200,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,DegTyp_ReceiveNewDegreeType,DegTyp_ContEditAfterChgDegTyp	},
   [ActRemDegTyp	] = { 545,-1,TabUnk,ActSeeDeg		,{    0,    0},    0,0x200,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,DegTyp_RemoveDegreeType	,DegTyp_ContEditAfterChgDegTyp	},
   [ActRenDegTyp	] = { 538,-1,TabUnk,ActSeeDeg		,{    0,    0},    0,0x200,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,DegTyp_RenameDegreeType	,DegTyp_ContEditAfterChgDegTyp	},

   // Buildings
   [ActSeeBld		] = {1838, 2,TabCtr,ActSeeBld		,{    0,    0},    0,0x3C7,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Bld_SeeBuildings		},
   [ActEdiBld		] = {1839,-1,TabUnk,ActSeeBld		,{    0,    0},    0,0x3C6,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Bld_EditBuildings		},
   [ActNewBld		] = {1840,-1,TabUnk,ActSeeBld		,{    0,    0},    0,0x3C6,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,Bld_ReceiveNewBuilding	,Bld_ContEditAfterChgBuilding	},
   [ActRemBld		] = {1841,-1,TabUnk,ActSeeBld		,{    0,    0},    0,0x3C6,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,Bld_RemoveBuilding	,Bld_ContEditAfterChgBuilding	},
   [ActRenBldSho	] = {1842,-1,TabUnk,ActSeeBld		,{    0,    0},    0,0x3C6,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,Bld_RenameBuildingShort	,Bld_ContEditAfterChgBuilding	},
   [ActRenBldFul	] = {1843,-1,TabUnk,ActSeeBld		,{    0,    0},    0,0x3C6,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,Bld_RenameBuildingFull	,Bld_ContEditAfterChgBuilding	},
   [ActRenBldLoc	] = {1844,-1,TabUnk,ActSeeBld		,{    0,    0},    0,0x3C6,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,Bld_ChangeBuildingLocation,Bld_ContEditAfterChgBuilding	},

   // Rooms
   [ActSeeRoo		] = {1744, 2,TabCtr,ActSeeRoo		,{    0,    0},    0,0x3C7,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Roo_SeeRooms			},
   [ActEdiRoo		] = {1745,-1,TabUnk,ActSeeRoo		,{    0,    0},    0,0x3C6,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Roo_EditRooms			},
   [ActNewRoo		] = {1746,-1,TabUnk,ActSeeRoo		,{    0,    0},    0,0x3C6,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,Roo_ReceiveNewRoom	,Roo_ContEditAfterChgRoom	},
   [ActRemRoo		] = {1747,-1,TabUnk,ActSeeRoo		,{    0,    0},    0,0x3C6,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,Roo_RemoveRoom		,Roo_ContEditAfterChgRoom	},
   [ActChgRooBld	] = {1845,-1,TabUnk,ActSeeRoo		,{    0,    0},    0,0x3C6,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,Roo_ChangeBuilding	,Roo_ContEditAfterChgRoom	},
   [ActChgRooFlo	] = {1846,-1,TabUnk,ActSeeRoo		,{    0,    0},    0,0x3C6,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,Roo_ChangeFloor		,Roo_ContEditAfterChgRoom	},
   [ActChgRooTyp	] = {1847,-1,TabUnk,ActSeeRoo		,{    0,    0},    0,0x3C6,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,Roo_ChangeType		,Roo_ContEditAfterChgRoom	},
   [ActRenRooSho	] = {1748,-1,TabUnk,ActSeeRoo		,{    0,    0},    0,0x3C6,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,Roo_RenameRoomShort	,Roo_ContEditAfterChgRoom	},
   [ActRenRooFul	] = {1749,-1,TabUnk,ActSeeRoo		,{    0,    0},    0,0x3C6,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,Roo_RenameRoomFull	,Roo_ContEditAfterChgRoom	},
   [ActChgRooMaxUsr	] = {1750,-1,TabUnk,ActSeeRoo		,{    0,    0},    0,0x3C6,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,Roo_ChangeCapacity	,Roo_ContEditAfterChgRoom	},
   [ActChgRooMAC	] = {1911,-1,TabUnk,ActSeeRoo		,{    0,    0},    0,0x3C6,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,Roo_ChangeMAC		,Roo_ContEditAfterChgRoom	},

   // TabDeg ******************************************************************
   // Degree info
   [ActSeeDegInf	] = {1149, 0,TabDeg,ActSeeDegInf	,{    0,    0},0x3C7,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,DegCfg_ShowConfiguration	},
   [ActPrnDegInf	] = {1150,-1,TabUnk,ActSeeDegInf	,{    0,    0},0x3C7,    0,    0,    0,    0,Act_CONT_NORM,Act_NEW_TAB,NULL			,DegCfg_PrintConfiguration	},
   [ActChgDegCtrCfg	] = {1588,-1,TabUnk,ActSeeDegInf	,{    0,    0},0x300,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,DegCfg_ChangeDegCtr	,DegCfg_ContEditAfterChgDeg	},
   [ActRenDegShoCfg	] = {1598,-1,TabUnk,ActSeeDegInf	,{    0,    0},0x380,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,DegCfg_RenameDegreeShort	,DegCfg_ContEditAfterChgDeg	},
   [ActRenDegFulCfg	] = {1597,-1,TabUnk,ActSeeDegInf	,{    0,    0},0x380,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,DegCfg_RenameDegreeFull	,DegCfg_ContEditAfterChgDeg	},
   [ActChgDegWWWCfg	] = {1599,-1,TabUnk,ActSeeDegInf	,{    0,    0},0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,DegCfg_ChangeDegWWW		},
   [ActReqDegLog	] = {1246,-1,TabUnk,ActSeeDegInf	,{    0,    0},0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,DegCfg_ReqLogo			},
   [ActRecDegLog	] = { 553,-1,TabUnk,ActSeeDegInf	,{    0,    0},0x3C0,    0,    0,    0,    0,Act_CONT_DATA,Act_1ST_TAB,DegCfg_ReceiveLogo	,DegCfg_ShowConfiguration	},
   [ActRemDegLog	] = {1343,-1,TabUnk,ActSeeDegInf	,{    0,    0},0x3C0,    0,    0,    0,    0,Act_CONT_DATA,Act_1ST_TAB,DegCfg_RemoveLogo	,DegCfg_ShowConfiguration	},

   // Courses
   [ActSeeCrs		] = {1009, 1,TabDeg,ActSeeCrs		,{    0,    0},0x3C7,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Crs_ShowCrssOfCurrentDeg	},
   [ActEdiCrs		] = { 555,-1,TabUnk,ActSeeCrs		,{    0,    0},0x3C6,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Crs_EditCourses		},
   [ActReqCrs		] = {1053,-1,TabUnk,ActSeeCrs		,{    0,    0},0x3C6,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,Crs_ReceiveReqCrs	,Crs_ContEditAfterChgCrs	},
   [ActNewCrs		] = { 556,-1,TabUnk,ActSeeCrs		,{    0,    0},0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,Crs_ReceiveNewCrs	,Crs_ContEditAfterChgCrs	},
   [ActRemCrs		] = { 560,-1,TabUnk,ActSeeCrs		,{    0,    0},0x3C6,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,Crs_RemoveCourse		,Crs_ContEditAfterChgCrs	},
   [ActChgInsCrsCod	] = {1025,-1,TabUnk,ActSeeCrs		,{    0,    0},0x3C6,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,Crs_ChangeInsCrsCod	,Crs_ContEditAfterChgCrs	},
   [ActChgCrsYea	] = { 561,-1,TabUnk,ActSeeCrs		,{    0,    0},0x3C6,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,Crs_ChangeCrsYear	,Crs_ContEditAfterChgCrs	},
   [ActRenCrsSho	] = { 563,-1,TabUnk,ActSeeCrs		,{    0,    0},0x3C6,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,Crs_RenameCourseShort	,Crs_ContEditAfterChgCrs	},
   [ActRenCrsFul	] = { 564,-1,TabUnk,ActSeeCrs		,{    0,    0},0x3C6,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,Crs_RenameCourseFull	,Crs_ContEditAfterChgCrs	},
   [ActChgCrsSta	] = {1055,-1,TabUnk,ActSeeCrs		,{    0,    0},0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,Crs_ChangeCrsStatus	,Crs_ContEditAfterChgCrs	},

   // TabCrs ******************************************************************
   // Course info
   [ActSeeCrsInf	] = { 847, 0,TabCrs,ActSeeCrsInf	,{0x3C7,0x3F8},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Crs_ShowIntroduction		},
   [ActPrnCrsInf	] = {1028,-1,TabUnk,ActSeeCrsInf	,{0x3C7,0x3F8},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_NEW_TAB,NULL			,CrsCfg_PrintConfiguration	},
   [ActChgCrsDegCfg	] = {1587,-1,TabUnk,ActSeeCrsInf	,{0x380,0x380},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,CrsCfg_ChangeCrsDeg	,CrsCfg_ContEditAfterChgCrs	},
   [ActRenCrsShoCfg	] = {1601,-1,TabUnk,ActSeeCrsInf	,{0x3C0,0x3C0},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,CrsCfg_RenameCourseShort	,CrsCfg_ContEditAfterChgCrs	},
   [ActRenCrsFulCfg	] = {1600,-1,TabUnk,ActSeeCrsInf	,{0x3C0,0x3C0},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,CrsCfg_RenameCourseFull	,CrsCfg_ContEditAfterChgCrs	},
   [ActChgInsCrsCodCfg	] = {1024,-1,TabUnk,ActSeeCrsInf	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,CrsCfg_ChangeInsCrsCod	,CrsCfg_ContEditAfterChgCrs	},
   [ActChgCrsYeaCfg	] = {1573,-1,TabUnk,ActSeeCrsInf	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,CrsCfg_ChangeCrsYear	,CrsCfg_ContEditAfterChgCrs	},
   [ActEdiCrsInf	] = { 848,-1,TabUnk,ActSeeCrsInf	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Inf_FormsToSelSendInfo		},
   [ActChgFrcReaCrsInf	] = { 877,-1,TabUnk,ActSeeCrsInf	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Inf_ChangeForceReadInfo	},
   [ActChgHavReaCrsInf	] = { 878,-1,TabUnk,ActSeeCrsInf	,{    0,0x008},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Inf_ChangeIHaveReadInfo	},
   [ActSelInfSrcCrsInf	] = { 849,-1,TabUnk,ActSeeCrsInf	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Inf_SetInfoSrc			},
   [ActRcvURLCrsInf	] = { 854,-1,TabUnk,ActSeeCrsInf	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Inf_ReceiveURLInfo		},
   [ActRcvPagCrsInf	] = { 853,-1,TabUnk,ActSeeCrsInf	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_DATA,Act_1ST_TAB,NULL			,Inf_ReceivePagInfo		},
   [ActEditorCrsInf	] = { 852,-1,TabUnk,ActSeeCrsInf	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Inf_EditorCourseInfo		},
   [ActPlaTxtEdiCrsInf	] = { 850,-1,TabUnk,ActSeeCrsInf	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Inf_EditPlainTxtInfo		},
   [ActRchTxtEdiCrsInf	] = {1093,-1,TabUnk,ActSeeCrsInf	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Inf_EditRichTxtInfo		},
   [ActRcvPlaTxtCrsInf	] = { 851,-1,TabUnk,ActSeeCrsInf	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Inf_RecAndChangePlainTxtInfo	},
   [ActRcvRchTxtCrsInf	] = {1101,-1,TabUnk,ActSeeCrsInf	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Inf_RecAndChangeRichTxtInfo	},

   // Program
   [ActSeePrg		] = {1821, 1,TabCrs,ActSeePrg		,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Prg_ShowCourseProgram		},
   [ActEdiPrg		] = {1926, 1,TabUnk,ActSeePrg		,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Prg_EditCourseProgram		},
   [ActSeePrgItm	] = {1927,-1,TabUnk,ActSeePrg		,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Prg_ViewItemAfterEdit		},
   [ActFrmChgPrgItm	] = {1823,-1,TabUnk,ActSeePrg		,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Prg_ReqChangeItem		},
   [ActFrmNewPrgItm	] = {1822,-1,TabUnk,ActSeePrg		,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Prg_ReqCreateItem		},
   [ActChgPrgItm	] = {1826,-1,TabUnk,ActSeePrg		,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Prg_ReceiveChgItem		},
   [ActNewPrgItm	] = {1825,-1,TabUnk,ActSeePrg		,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Prg_ReceiveNewItem		},
   [ActReqRemPrgItm	] = {1827,-1,TabUnk,ActSeePrg		,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Prg_ReqRemItem			},
   [ActRemPrgItm	] = {1828,-1,TabUnk,ActSeePrg		,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Prg_RemoveItem			},
   [ActHidPrgItm	] = {1829,-1,TabUnk,ActSeePrg		,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Prg_HideItem			},
   [ActUnhPrgItm	] = {1830,-1,TabUnk,ActSeePrg		,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Prg_UnhideItem			},
   [ActUp_PrgItm	] = {1831,-1,TabUnk,ActSeePrg		,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Prg_MoveUpItem			},
   [ActDwnPrgItm	] = {1832,-1,TabUnk,ActSeePrg		,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Prg_MoveDownItem		},
   [ActLftPrgItm	] = {1834,-1,TabUnk,ActSeePrg		,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Prg_MoveLeftItem		},
   [ActRgtPrgItm	] = {1833,-1,TabUnk,ActSeePrg		,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Prg_MoveRightItem		},
   [ActExpSeePrgItm	] = {1944,-1,TabUnk,ActSeePrg		,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Prg_ExpandItem			},
   [ActConSeePrgItm	] = {1945,-1,TabUnk,ActSeePrg		,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Prg_ContractItem		},
   [ActExpEdiPrgItm	] = {1946,-1,TabUnk,ActSeePrg		,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Prg_ExpandItem			},
   [ActConEdiPrgItm	] = {1947,-1,TabUnk,ActSeePrg		,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Prg_ContractItem		},
   [ActSeeRscCli_InPrg	] = {1970,-1,TabUnk,ActSeePrg		,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,PrgRsc_ViewResourceClipboard	},
   [ActRemRscCli_InPrg	] = {1971,-1,TabUnk,ActSeePrg		,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,PrgRsc_RemoveResourceClipboard	},
   [ActFrmSeePrgRsc	] = {1925,-1,TabUnk,ActSeePrg		,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,PrgRsc_ViewResourcesAfterEdit	},
   [ActFrmEdiPrgRsc	] = {1918,-1,TabUnk,ActSeePrg		,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,PrgRsc_EditResources		},
   [ActNewPrgRsc	] = {1929,-1,TabUnk,ActSeePrg		,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,PrgRsc_CreateResource		},
   [ActRenPrgRsc	] = {1928,-1,TabUnk,ActSeePrg		,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,PrgRsc_RenameResource		},
   [ActReqRemPrgRsc	] = {1919,-1,TabUnk,ActSeePrg		,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,PrgRsc_ReqRemResource		},
   [ActRemPrgRsc	] = {1920,-1,TabUnk,ActSeePrg		,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,PrgRsc_RemoveResource		},
   [ActHidPrgRsc	] = {1921,-1,TabUnk,ActSeePrg		,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,PrgRsc_HideResource		},
   [ActUnhPrgRsc	] = {1922,-1,TabUnk,ActSeePrg		,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,PrgRsc_UnhideResource		},
   [ActUp_PrgRsc	] = {1923,-1,TabUnk,ActSeePrg		,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,PrgRsc_MoveUpResource		},
   [ActDwnPrgRsc	] = {1924,-1,TabUnk,ActSeePrg		,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,PrgRsc_MoveDownResource	},
   [ActFrmChgLnkPrgRsc	] = {1932,-1,TabUnk,ActSeePrg		,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,PrgRsc_EditProgramWithClipboard},
   [ActChgLnkPrgRsc	] = {1933,-1,TabUnk,ActSeePrg		,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,PrgRsc_ChangeLink		},

   // Teaching guide
   [ActSeeTchGui	] = { 784, 2,TabCrs,ActSeeTchGui	,{0x3C7,0x3F8},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Inf_ShowInfo			},
   [ActEdiTchGui	] = { 785,-1,TabUnk,ActSeeTchGui	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Inf_FormsToSelSendInfo		},
   [ActChgFrcReaTchGui	] = { 870,-1,TabUnk,ActSeeTchGui	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Inf_ChangeForceReadInfo	},
   [ActChgHavReaTchGui	] = { 886,-1,TabUnk,ActSeeTchGui	,{    0,0x008},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Inf_ChangeIHaveReadInfo	},
   [ActSelInfSrcTchGui	] = { 789,-1,TabUnk,ActSeeTchGui	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Inf_SetInfoSrc			},
   [ActRcvURLTchGui	] = { 791,-1,TabUnk,ActSeeTchGui	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Inf_ReceiveURLInfo		},
   [ActRcvPagTchGui	] = { 788,-1,TabUnk,ActSeeTchGui	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_DATA,Act_1ST_TAB,NULL			,Inf_ReceivePagInfo		},
   [ActEditorTchGui	] = { 786,-1,TabUnk,ActSeeTchGui	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Inf_EditorTeachingGuide	},
   [ActPlaTxtEdiTchGui	] = { 787,-1,TabUnk,ActSeeTchGui	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Inf_EditPlainTxtInfo		},
   [ActRchTxtEdiTchGui	] = {1094,-1,TabUnk,ActSeeTchGui	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Inf_EditRichTxtInfo		},
   [ActRcvPlaTxtTchGui	] = { 790,-1,TabUnk,ActSeeTchGui	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Inf_RecAndChangePlainTxtInfo	},
   [ActRcvRchTxtTchGui	] = {1102,-1,TabUnk,ActSeeTchGui	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Inf_RecAndChangeRichTxtInfo	},

   // Syllabus
   [ActSeeSyl		] = {1242, 3,TabCrs,ActSeeSyl		,{0x3C7,0x3F8},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Inf_ShowInfo			},
   [ActEdiSyl		] = {  44,-1,TabUnk,ActSeeSyl		,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Inf_FormsToSelSendInfo		},
   [ActDelItmSyl	] = { 218,-1,TabUnk,ActSeeSyl		,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Syl_RemoveItemSyllabus		},
   [ActUp_IteSyl	] = { 221,-1,TabUnk,ActSeeSyl		,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Syl_UpItemSyllabus		},
   [ActDwnIteSyl	] = { 220,-1,TabUnk,ActSeeSyl		,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Syl_DownItemSyllabus		},
   [ActRgtIteSyl	] = { 223,-1,TabUnk,ActSeeSyl		,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Syl_RightItemSyllabus		},
   [ActLftIteSyl	] = { 222,-1,TabUnk,ActSeeSyl		,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Syl_LeftItemSyllabus		},
   [ActInsIteSyl	] = { 217,-1,TabUnk,ActSeeSyl		,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Syl_InsertItemSyllabus		},
   [ActModIteSyl	] = { 211,-1,TabUnk,ActSeeSyl		,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Syl_ModifyItemSyllabus		},
   [ActChgFrcReaSyl	] = { 871,-1,TabUnk,ActSeeSyl		,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Inf_ChangeForceReadInfo	},
   [ActChgHavReaSyl	] = { 880,-1,TabUnk,ActSeeSyl		,{    0,0x008},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Inf_ChangeIHaveReadInfo	},
   [ActSelInfSrcSyl	] = { 378,-1,TabUnk,ActSeeSyl		,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Inf_SetInfoSrc			},
   [ActRcvURLSyl	] = { 403,-1,TabUnk,ActSeeSyl		,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Inf_ReceiveURLInfo		},
   [ActRcvPagSyl	] = { 381,-1,TabUnk,ActSeeSyl		,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_DATA,Act_1ST_TAB,NULL			,Inf_ReceivePagInfo		},
   [ActEditorSyl	] = { 372,-1,TabUnk,ActSeeSyl		,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Syl_EditSyllabus		},
   [ActPlaTxtEdiSyl	] = { 379,-1,TabUnk,ActSeeSyl		,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Inf_EditPlainTxtInfo		},
   [ActRchTxtEdiSyl	] = {1095,-1,TabUnk,ActSeeSyl		,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Inf_EditRichTxtInfo		},
   [ActRcvPlaTxtSyl	] = { 394,-1,TabUnk,ActSeeSyl		,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Inf_RecAndChangePlainTxtInfo	},
   [ActRcvRchTxtSyl	] = {1103,-1,TabUnk,ActSeeSyl		,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Inf_RecAndChangeRichTxtInfo	},

   // Bibliography
   [ActSeeBib		] = {  32, 4,TabCrs,ActSeeBib		,{0x3C7,0x3F8},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Inf_ShowInfo			},
   [ActEdiBib		] = {  76,-1,TabUnk,ActSeeBib		,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Inf_FormsToSelSendInfo		},
   [ActChgFrcReaBib	] = { 873,-1,TabUnk,ActSeeBib		,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Inf_ChangeForceReadInfo	},
   [ActChgHavReaBib	] = { 884,-1,TabUnk,ActSeeBib		,{    0,0x008},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Inf_ChangeIHaveReadInfo	},
   [ActSelInfSrcBib	] = { 370,-1,TabUnk,ActSeeBib		,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Inf_SetInfoSrc			},
   [ActRcvURLBib	] = { 224,-1,TabUnk,ActSeeBib		,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Inf_ReceiveURLInfo		},
   [ActRcvPagBib	] = { 185,-1,TabUnk,ActSeeBib		,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_DATA,Act_1ST_TAB,NULL			,Inf_ReceivePagInfo		},
   [ActEditorBib	] = { 376,-1,TabUnk,ActSeeBib		,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Inf_EditorBibliography		},
   [ActPlaTxtEdiBib	] = { 377,-1,TabUnk,ActSeeBib		,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Inf_EditPlainTxtInfo		},
   [ActRchTxtEdiBib	] = {1097,-1,TabUnk,ActSeeBib		,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Inf_EditRichTxtInfo		},
   [ActRcvPlaTxtBib	] = { 398,-1,TabUnk,ActSeeBib		,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Inf_RecAndChangePlainTxtInfo	},
   [ActRcvRchTxtBib	] = {1105,-1,TabUnk,ActSeeBib		,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Inf_RecAndChangeRichTxtInfo	},

   // FAQ
   [ActSeeFAQ		] = {  54, 5,TabCrs,ActSeeFAQ		,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Inf_ShowInfo			},
   [ActEdiFAQ		] = { 109,-1,TabUnk,ActSeeFAQ		,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Inf_FormsToSelSendInfo		},
   [ActChgFrcReaFAQ	] = { 874,-1,TabUnk,ActSeeFAQ		,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Inf_ChangeForceReadInfo	},
   [ActChgHavReaFAQ	] = { 879,-1,TabUnk,ActSeeFAQ		,{    0,0x008},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Inf_ChangeIHaveReadInfo	},
   [ActSelInfSrcFAQ	] = { 380,-1,TabUnk,ActSeeFAQ		,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Inf_SetInfoSrc			},
   [ActRcvURLFAQ	] = { 234,-1,TabUnk,ActSeeFAQ		,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Inf_ReceiveURLInfo		},
   [ActRcvPagFAQ	] = { 219,-1,TabUnk,ActSeeFAQ		,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_DATA,Act_1ST_TAB,NULL			,Inf_ReceivePagInfo		},
   [ActEditorFAQ	] = { 404,-1,TabUnk,ActSeeFAQ		,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Inf_EditorFAQ   		},
   [ActPlaTxtEdiFAQ	] = { 405,-1,TabUnk,ActSeeFAQ		,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Inf_EditPlainTxtInfo   	},
   [ActRchTxtEdiFAQ	] = {1098,-1,TabUnk,ActSeeFAQ		,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Inf_EditRichTxtInfo   		},
   [ActRcvPlaTxtFAQ	] = { 406,-1,TabUnk,ActSeeFAQ		,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Inf_RecAndChangePlainTxtInfo	},
   [ActRcvRchTxtFAQ	] = {1106,-1,TabUnk,ActSeeFAQ		,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Inf_RecAndChangeRichTxtInfo	},

   // Links
   [ActSeeCrsLnk	] = {   9, 6,TabCrs,ActSeeCrsLnk	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Inf_ShowInfo			},
   [ActEdiCrsLnk	] = {  96,-1,TabUnk,ActSeeCrsLnk	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Inf_FormsToSelSendInfo		},
   [ActChgFrcReaCrsLnk	] = { 875,-1,TabUnk,ActSeeCrsLnk	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Inf_ChangeForceReadInfo	},
   [ActChgHavReaCrsLnk	] = { 885,-1,TabUnk,ActSeeCrsLnk	,{    0,0x008},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Inf_ChangeIHaveReadInfo	},
   [ActSelInfSrcCrsLnk	] = { 385,-1,TabUnk,ActSeeCrsLnk	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Inf_SetInfoSrc			},
   [ActRcvURLCrsLnk	] = { 182,-1,TabUnk,ActSeeCrsLnk	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Inf_ReceiveURLInfo		},
   [ActRcvPagCrsLnk	] = { 164,-1,TabUnk,ActSeeCrsLnk	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_DATA,Act_1ST_TAB,NULL			,Inf_ReceivePagInfo		},
   [ActEditorCrsLnk	] = { 388,-1,TabUnk,ActSeeCrsLnk	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Inf_EditorLinks		},
   [ActPlaTxtEdiCrsLnk	] = { 400,-1,TabUnk,ActSeeCrsLnk	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Inf_EditPlainTxtInfo		},
   [ActRchTxtEdiCrsLnk	] = {1099,-1,TabUnk,ActSeeCrsLnk	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Inf_EditRichTxtInfo		},
   [ActRcvPlaTxtCrsLnk	] = { 401,-1,TabUnk,ActSeeCrsLnk	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Inf_RecAndChangePlainTxtInfo	},
   [ActRcvRchTxtCrsLnk	] = {1107,-1,TabUnk,ActSeeCrsLnk	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Inf_RecAndChangeRichTxtInfo	},

   // Assessment
   [ActSeeAss		] = {  15, 7,TabCrs,ActSeeAss		,{0x3C7,0x3F8},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Inf_ShowInfo			},
   [ActEdiAss		] = {  69,-1,TabUnk,ActSeeAss		,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Inf_FormsToSelSendInfo		},
   [ActChgFrcReaAss	] = { 883,-1,TabUnk,ActSeeAss		,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Inf_ChangeForceReadInfo	},
   [ActChgHavReaAss	] = { 898,-1,TabUnk,ActSeeAss		,{    0,0x008},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Inf_ChangeIHaveReadInfo	},
   [ActSelInfSrcAss	] = { 384,-1,TabUnk,ActSeeAss		,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Inf_SetInfoSrc			},
   [ActRcvURLAss	] = { 235,-1,TabUnk,ActSeeAss		,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Inf_ReceiveURLInfo		},
   [ActRcvPagAss	] = { 184,-1,TabUnk,ActSeeAss		,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_DATA,Act_1ST_TAB,NULL			,Inf_ReceivePagInfo		},
   [ActEditorAss	] = { 386,-1,TabUnk,ActSeeAss		,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Inf_EditorAssessment		},
   [ActPlaTxtEdiAss	] = { 387,-1,TabUnk,ActSeeAss		,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Inf_EditPlainTxtInfo		},
   [ActRchTxtEdiAss	] = {1100,-1,TabUnk,ActSeeAss		,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Inf_EditRichTxtInfo		},
   [ActRcvPlaTxtAss	] = { 397,-1,TabUnk,ActSeeAss		,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Inf_RecAndChangePlainTxtInfo	},
   [ActRcvRchTxtAss	] = {1108,-1,TabUnk,ActSeeAss		,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Inf_RecAndChangeRichTxtInfo	},

   // Timetable
   [ActSeeCrsTT		] = {  25, 8,TabCrs,ActSeeCrsTT		,{0x3C7,0x3F8},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Tmt_ShowClassTimeTable		},
   [ActPrnCrsTT		] = { 152,-1,TabUnk,ActSeeCrsTT		,{0x3C7,0x3F8},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_NEW_TAB,NULL			,Tmt_ShowClassTimeTable		},
   [ActEdiCrsTT		] = {  45,-1,TabUnk,ActSeeCrsTT		,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Tmt_EditCrsTimeTable		},
   [ActChgCrsTT		] = {  53,-1,TabUnk,ActSeeCrsTT		,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Tmt_EditCrsTimeTable		},
   [ActChgCrsTT1stDay	] = {1486,-1,TabUnk,ActSeeCrsTT		,{0x3C7,0x3F8},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,Cal_ChangeFirstDayOfWeek	,Tmt_ShowClassTimeTable		},

   // TabAss ******************************************************************
   // Assignments
   [ActSeeAllAsg	] = { 801, 0,TabAss,ActSeeAllAsg	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Asg_SeeAssignments		},
   [ActFrmNewAsg	] = { 812,-1,TabUnk,ActSeeAllAsg	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Asg_ReqCreatOrEditAsg		},
   [ActEdiOneAsg	] = { 814,-1,TabUnk,ActSeeAllAsg	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Asg_ReqCreatOrEditAsg		},
   [ActSeeOneAsg	] = {1942,-1,TabUnk,ActSeeAllAsg	,{0x200,0x3F8},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Asg_SeeOneAssignment		},
   [ActPrnOneAsg	] = {1637,-1,TabUnk,ActSeeAllAsg	,{0x200,0x3F8},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_NEW_TAB,NULL			,Asg_PrintOneAssignment		},
   [ActNewAsg		] = { 803,-1,TabUnk,ActSeeAllAsg	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Asg_ReceiveAssignment		},
   [ActChgAsg		] = { 815,-1,TabUnk,ActSeeAllAsg	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Asg_ReceiveAssignment		},
   [ActReqRemAsg	] = { 813,-1,TabUnk,ActSeeAllAsg	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Asg_ReqRemAssignment		},
   [ActRemAsg		] = { 806,-1,TabUnk,ActSeeAllAsg	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Asg_RemoveAssignment		},
   [ActHidAsg		] = { 964,-1,TabUnk,ActSeeAllAsg	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Asg_HideAssignment		},
   [ActUnhAsg		] = { 965,-1,TabUnk,ActSeeAllAsg	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Asg_UnhideAssignment		},
   [ActReqLnkAsg	] = {1943,-1,TabUnk,ActSeeAllAsg	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,AsgRsc_GetLinkToAssignment	},

   // Projects
   [ActSeeAllPrj	] = {1674, 1,TabAss,ActSeeAllPrj	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Prj_SeeAllProjects		},
   [ActCfgPrj		] = {1803,-1,TabUnk,ActSeeAllPrj	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,PrjCfg_ShowFormConfig		},
   [ActChgNETCanCrePrj	] = {1804,-1,TabUnk,ActSeeAllPrj	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,PrjCfg_ChangeNETCanCreate	},
   [ActChgRubPrj	] = {1977,-1,TabUnk,ActSeeAllPrj	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,PrjCfg_ChangeRubricsOfType	},
   [ActReqUsrPrj	] = {1805,-1,TabUnk,ActSeeAllPrj	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Prj_ListUsrsToSelect		},
   [ActSeeTblAllPrj	] = {1696,-1,TabUnk,ActSeeAllPrj	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_NEW_TAB,NULL			,Prj_ShowTableSelectedPrjs	},
   [ActReqLckAllPrj	] = {1775,-1,TabUnk,ActSeeAllPrj	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Prj_ReqLockSelectedPrjsEdition	},
   [ActReqUnlAllPrj	] = {1776,-1,TabUnk,ActSeeAllPrj	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Prj_ReqUnloSelectedPrjsEdition	},
   [ActLckAllPrj	] = {1777,-1,TabUnk,ActSeeAllPrj	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Prj_LockSelectedPrjsEdition	},
   [ActUnlAllPrj	] = {1778,-1,TabUnk,ActSeeAllPrj	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Prj_UnloSelectedPrjsEdition	},
   [ActFrmNewPrj	] = {1675,-1,TabUnk,ActSeeAllPrj	,{0x200,0x230},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Prj_ReqCreatePrj		},
   [ActEdiOnePrj	] = {1676,-1,TabUnk,ActSeeAllPrj	,{0x200,0x230},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Prj_ReqEditPrj			},
   [ActSeeOnePrj	] = {1949,-1,TabUnk,ActSeeAllPrj	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_ShowFileBrowserOrWorks	},
   [ActPrnOnePrj	] = {1677,-1,TabUnk,ActSeeAllPrj	,{0x200,0x3F8},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_NEW_TAB,NULL			,Prj_PrintOneProject		},
   [ActNewPrj		] = {1678,-1,TabUnk,ActSeeAllPrj	,{0x200,0x230},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Prj_ReceiveProject		},
   [ActChgPrj		] = {1679,-1,TabUnk,ActSeeAllPrj	,{0x200,0x230},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Prj_ReceiveProject		},
   [ActReqRemPrj	] = {1680,-1,TabUnk,ActSeeAllPrj	,{0x200,0x230},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Prj_ReqRemProject		},
   [ActRemPrj		] = {1681,-1,TabUnk,ActSeeAllPrj	,{0x200,0x230},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Prj_RemoveProject		},
   [ActHidPrj		] = {1682,-1,TabUnk,ActSeeAllPrj	,{0x200,0x230},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Prj_HideProject		},
   [ActUnhPrj		] = {1683,-1,TabUnk,ActSeeAllPrj	,{0x200,0x230},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Prj_UnhideProject		},
   [ActLckPrj		] = {1773,-1,TabUnk,ActSeeAllPrj	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_AJAX_NR,NULL			,Prj_LockProjectEdition		},
   [ActUnlPrj		] = {1774,-1,TabUnk,ActSeeAllPrj	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_AJAX_NR,NULL			,Prj_UnloProjectEdition		},
   [ActChgPrjRev	] = {1950,-1,TabUnk,ActSeeAllPrj	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Prj_ChangeReviewStatus		},
   [ActReqAddStdPrj	] = {1684,-1,TabUnk,ActSeeAllPrj	,{0x200,0x230},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Prj_ReqAddStds			},
   [ActReqAddTutPrj	] = {1685,-1,TabUnk,ActSeeAllPrj	,{0x200,0x230},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Prj_ReqAddTuts			},
   [ActReqAddEvlPrj	] = {1686,-1,TabUnk,ActSeeAllPrj	,{0x200,0x230},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Prj_ReqAddEvls			},
   [ActAddStdPrj	] = {1687,-1,TabUnk,ActSeeAllPrj	,{0x200,0x230},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Prj_GetSelectedUsrsAndAddStds	},
   [ActAddTutPrj	] = {1688,-1,TabUnk,ActSeeAllPrj	,{0x200,0x230},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Prj_GetSelectedUsrsAndAddTuts	},
   [ActAddEvlPrj	] = {1689,-1,TabUnk,ActSeeAllPrj	,{0x200,0x230},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Prj_GetSelectedUsrsAndAddEvls	},
   [ActReqRemStdPrj	] = {1690,-1,TabUnk,ActSeeAllPrj	,{0x200,0x230},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Prj_ReqRemStd			},
   [ActReqRemTutPrj	] = {1691,-1,TabUnk,ActSeeAllPrj	,{0x200,0x230},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Prj_ReqRemTut			},
   [ActReqRemEvlPrj	] = {1692,-1,TabUnk,ActSeeAllPrj	,{0x200,0x230},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Prj_ReqRemEvl			},
   [ActRemStdPrj	] = {1693,-1,TabUnk,ActSeeAllPrj	,{0x200,0x230},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Prj_RemStd			},
   [ActRemTutPrj	] = {1694,-1,TabUnk,ActSeeAllPrj	,{0x200,0x230},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Prj_RemTut			},
   [ActRemEvlPrj	] = {1695,-1,TabUnk,ActSeeAllPrj	,{0x200,0x230},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Prj_RemEvl			},
   [ActChgPrjSco	] = {1974,-1,TabUnk,ActSeeAllPrj	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Prj_ChangeCriterionScore	},
   [ActReqLnkPrj	] = {1948,-1,TabUnk,ActSeeAllPrj	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,PrjRsc_GetLinkToProject	},
   [ActAdmDocPrj	] = {1697,-1,TabUnk,ActSeeAllPrj	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_ShowFileBrowserOrWorks	},
   [ActReqRemFilDocPrj	] = {1698,-1,TabUnk,ActSeeAllPrj	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_AskRemFileFromTree		},
   [ActRemFilDocPrj	] = {1699,-1,TabUnk,ActSeeAllPrj	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_RemFileFromTree		},
   [ActRemFolDocPrj	] = {1700,-1,TabUnk,ActSeeAllPrj	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_RemFolderFromTree		},
   [ActCopDocPrj	] = {1701,-1,TabUnk,ActSeeAllPrj	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_CopyFromFileBrowser	},
   [ActPasDocPrj	] = {1702,-1,TabUnk,ActSeeAllPrj	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_PasteIntoFileBrowser	},
   [ActRemTreDocPrj	] = {1703,-1,TabUnk,ActSeeAllPrj	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_RemSubtreeInFileBrowser	},
   [ActFrmCreDocPrj	] = {1704,-1,TabUnk,ActSeeAllPrj	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_ShowFormFileBrowser	},
   [ActCreFolDocPrj	] = {1705,-1,TabUnk,ActSeeAllPrj	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_RecFolderFileBrowser	},
   [ActCreLnkDocPrj	] = {1706,-1,TabUnk,ActSeeAllPrj	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_RecLinkFileBrowser		},
   [ActRenFolDocPrj	] = {1707,-1,TabUnk,ActSeeAllPrj	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_RenFolderFileBrowser	},
   [ActRcvFilDocPrjDZ	] = {1708,-1,TabUnk,ActSeeAllPrj	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_DATA,Act_UPL_FIL,Brw_RcvFileInFileBrwDZ	,NULL				},
   [ActRcvFilDocPrjCla	] = {1709,-1,TabUnk,ActSeeAllPrj	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_DATA,Act_1ST_TAB,NULL			,Brw_RcvFileInFileBrwClassic	},
   [ActExpDocPrj	] = {1710,-1,TabUnk,ActSeeAllPrj	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_204_NOC,Brw_ExpandFileTree	,NULL				},
   [ActConDocPrj	] = {1711,-1,TabUnk,ActSeeAllPrj	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_204_NOC,Brw_ContractFileTree	,NULL				},
   [ActZIPDocPrj	] = {1712,-1,TabUnk,ActSeeAllPrj	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,ZIP_CompressFileTree		},
   [ActReqDatDocPrj	] = {1713,-1,TabUnk,ActSeeAllPrj	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_ShowFileMetadata		},
   [ActChgDatDocPrj	] = {1714,-1,TabUnk,ActSeeAllPrj	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_ChgFileMetadata		},
   [ActDowDocPrj	] = {1715,-1,TabUnk,ActSeeAllPrj	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_DWN_FIL,Brw_DownloadFile		,NULL				},
   [ActAdmAssPrj	] = {1716,-1,TabUnk,ActSeeAllPrj	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_ShowFileBrowserOrWorks	},
   [ActReqRemFilAssPrj	] = {1717,-1,TabUnk,ActSeeAllPrj	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_AskRemFileFromTree		},
   [ActRemFilAssPrj	] = {1718,-1,TabUnk,ActSeeAllPrj	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_RemFileFromTree		},
   [ActRemFolAssPrj	] = {1719,-1,TabUnk,ActSeeAllPrj	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_RemFolderFromTree		},
   [ActCopAssPrj	] = {1720,-1,TabUnk,ActSeeAllPrj	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_CopyFromFileBrowser	},
   [ActPasAssPrj	] = {1721,-1,TabUnk,ActSeeAllPrj	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_PasteIntoFileBrowser	},
   [ActRemTreAssPrj	] = {1722,-1,TabUnk,ActSeeAllPrj	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_RemSubtreeInFileBrowser	},
   [ActFrmCreAssPrj	] = {1723,-1,TabUnk,ActSeeAllPrj	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_ShowFormFileBrowser	},
   [ActCreFolAssPrj	] = {1724,-1,TabUnk,ActSeeAllPrj	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_RecFolderFileBrowser	},
   [ActCreLnkAssPrj	] = {1725,-1,TabUnk,ActSeeAllPrj	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_RecLinkFileBrowser		},
   [ActRenFolAssPrj	] = {1726,-1,TabUnk,ActSeeAllPrj	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_RenFolderFileBrowser	},
   [ActRcvFilAssPrjDZ	] = {1727,-1,TabUnk,ActSeeAllPrj	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_DATA,Act_UPL_FIL,Brw_RcvFileInFileBrwDZ	,NULL				},
   [ActRcvFilAssPrjCla	] = {1728,-1,TabUnk,ActSeeAllPrj	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_DATA,Act_1ST_TAB,NULL			,Brw_RcvFileInFileBrwClassic	},
   [ActExpAssPrj	] = {1729,-1,TabUnk,ActSeeAllPrj	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_204_NOC,Brw_ExpandFileTree	,NULL				},
   [ActConAssPrj	] = {1730,-1,TabUnk,ActSeeAllPrj	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_204_NOC,Brw_ContractFileTree	,NULL				},
   [ActZIPAssPrj	] = {1731,-1,TabUnk,ActSeeAllPrj	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,ZIP_CompressFileTree		},
   [ActReqDatAssPrj	] = {1732,-1,TabUnk,ActSeeAllPrj	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_ShowFileMetadata		},
   [ActChgDatAssPrj	] = {1733,-1,TabUnk,ActSeeAllPrj	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_ChgFileMetadata		},
   [ActDowAssPrj	] = {1734,-1,TabUnk,ActSeeAllPrj	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_DWN_FIL,Brw_DownloadFile		,NULL				},

   // Calls for exams
   [ActSeeAllCfe	] = {  85, 2,TabAss,ActSeeAllCfe	,{0x3C7,0x3F8},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Cfe_ListCallsForExamsSee	},
   [ActSeeOneCfe	] = {1572,-1,TabUnk,ActSeeAllCfe	,{0x3C7,0x3F8},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Cfe_ListCallsForExamsCod	},
   [ActSeeDatCfe	] = {1571,-1,TabUnk,ActSeeAllCfe	,{0x3C7,0x3F8},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Cfe_ListCallsForExamsDay	},
   [ActEdiCfe		] = {  91,-1,TabUnk,ActSeeAllCfe	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Cfe_PutFrmEditACallForExam	},
   [ActRcvCfe		] = { 110,-1,TabUnk,ActSeeAllCfe	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,Cfe_ReceiveCallForExam1	,Cfe_ReceiveCallForExam2	},
   [ActPrnCfe		] = { 179,-1,TabUnk,ActSeeAllCfe	,{0x3C7,0x3F8},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_NEW_TAB,NULL			,Cfe_PrintCallForExam		},
   [ActReqRemCfe	] = {1619,-1,TabUnk,ActSeeAllCfe	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Cfe_ReqRemCallForExam		},
   [ActRemCfe		] = { 187,-1,TabUnk,ActSeeAllCfe	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,Cfe_RemoveCallForExam1	,Cfe_RemoveCallForExam2		},
   [ActHidCfe		] = {1620,-1,TabUnk,ActSeeAllCfe	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,Cfe_HideCallForExam	,Cfe_ListCallsForExamsEdit	},
   [ActUnhCfe		] = {1621,-1,TabUnk,ActSeeAllCfe	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,Cfe_UnhideCallForExam	,Cfe_ListCallsForExamsEdit	},
   [ActReqLnkCfe	] = {1934,-1,TabUnk,ActSeeAllCfe	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,CfeRsc_GetLinkToCallForExam	},

   // Questions
   [ActEdiTstQst	] = { 104, 3,TabAss,ActEdiTstQst	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,Dat_SetDatesToPastAndNow	,Qst_ReqEditQsts		},
   [ActEdiOneTstQst	] = { 105,-1,TabUnk,ActEdiTstQst	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Qst_ShowFormEditOneQst		},
   [ActReqImpTstQst	] = {1007,-1,TabUnk,ActEdiTstQst	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,QstImp_ShowFormImpQstsFromXML	},
   [ActImpTstQst	] = {1008,-1,TabUnk,ActEdiTstQst	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_DATA,Act_1ST_TAB,NULL			,QstImp_ImpQstsFromXML		},
   [ActLstTstQst	] = { 132,-1,TabUnk,ActEdiTstQst	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Qst_ListQuestionsToEdit	},
   [ActNewTstQst	] = { 126,-1,TabUnk,ActEdiTstQst	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_DATA,Act_1ST_TAB,NULL			,Qst_ReceiveQst			},
   [ActChgTstQst	] = {1975,-1,TabUnk,ActEdiTstQst	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_DATA,Act_1ST_TAB,NULL			,Qst_ReceiveQst			},
   [ActReqRemSevTstQst	] = {1835,-1,TabUnk,ActEdiTstQst	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Qst_ReqRemSelectedQsts		},
   [ActRemSevTstQst	] = {1836,-1,TabUnk,ActEdiTstQst	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Qst_RemoveSelectedQsts		},
   [ActReqRemOneTstQst	] = {1523,-1,TabUnk,ActEdiTstQst	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Qst_ReqRemOneQst		},
   [ActRemOneTstQst	] = { 133,-1,TabUnk,ActEdiTstQst	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Qst_RemoveOneQst		},
   [ActChgShfTstQst	] = { 455,-1,TabUnk,ActEdiTstQst	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Qst_ChangeShuffleQst		},
   [ActEdiTag		] = {1907,-1,TabUnk,ActEdiTstQst	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Tag_ShowFormEditTags		},
   [ActEnaTag		] = { 453,-1,TabUnk,ActEdiTstQst	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Tag_EnableTag			},
   [ActDisTag		] = { 452,-1,TabUnk,ActEdiTstQst	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Tag_DisableTag			},
   [ActRenTag		] = { 143,-1,TabUnk,ActEdiTstQst	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Tag_RenameTag			},

   // Test
   [ActReqTst		] = { 103, 4,TabAss,ActReqTst		,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Tst_ReqTest			},
   [ActSeeTst		] = {  29,-1,TabUnk,ActReqTst		,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Tst_ShowNewTest		},
   [ActReqAssTst	] = {1837,-1,TabUnk,ActReqTst		,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Tst_ReceiveTestDraft		},
   [ActAssTst		] = {  98,-1,TabUnk,ActReqTst		,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Tst_AssessTest			},
   [ActCfgTst		] = { 451,-1,TabUnk,ActReqTst		,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,TstCfg_CheckAndShowFormConfig	},
   [ActRcvCfgTst	] = { 454,-1,TabUnk,ActReqTst		,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,TstCfg_ReceiveConfigTst	},
   [ActReqSeeMyTstRes	] = {1083,-1,TabUnk,ActReqTst		,{0x200,0x208},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,Dat_SetDatesToPastAndNow	,TstPrn_SelDatesToSeeMyPrints	},
   [ActSeeMyTstResCrs	] = {1084,-1,TabUnk,ActReqTst		,{0x200,0x208},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,TstPrn_ShowMyPrints		},
   [ActSeeOneTstResMe	] = {1085,-1,TabUnk,ActReqTst		,{0x200,0x208},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,TstPrn_ShowOnePrint		},
   [ActReqSeeUsrTstRes	] = {1080,-1,TabUnk,ActReqTst		,{0x200,0x230},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,Dat_SetDatesToPastAndNow	,TstPrn_SelUsrsToViewUsrsPrints},
   [ActSeeUsrTstResCrs	] = {1081,-1,TabUnk,ActReqTst		,{0x200,0x230},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,TstPrn_GetUsrsAndShowPrints	},
   [ActSeeOneTstResOth	] = {1082,-1,TabUnk,ActReqTst		,{0x200,0x230},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,TstPrn_ShowOnePrint		},

   // Exams
   [ActSeeAllExa	] = {1848, 5,TabAss,ActSeeAllExa	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Exa_SeeAllExams		},
   [ActSeeOneExa	] = {1849,-1,TabUnk,ActSeeAllExa	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Exa_SeeOneExam			},
   [ActFrmNewExa	] = {1877,-1,TabUnk,ActSeeAllExa	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Exa_ReqCreatOrEditExam		},
   [ActEdiOneExa	] = {1878,-1,TabUnk,ActSeeAllExa	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Exa_ReqCreatOrEditExam		},
   [ActNewExa		] = {1879,-1,TabUnk,ActSeeAllExa	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Exa_ReceiveExam		},
   [ActChgExa		] = {1880,-1,TabUnk,ActSeeAllExa	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Exa_ReceiveExam		},
   [ActReqRemExa	] = {1881,-1,TabUnk,ActSeeAllExa	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Exa_AskRemExam			},
   [ActRemExa		] = {1882,-1,TabUnk,ActSeeAllExa	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Exa_RemoveExam			},
   [ActHidExa		] = {1883,-1,TabUnk,ActSeeAllExa	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Exa_HideExam			},
   [ActUnhExa		] = {1884,-1,TabUnk,ActSeeAllExa	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Exa_UnhideExam			},
   [ActReqLnkExa	] = {1936,-1,TabUnk,ActSeeAllExa	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,ExaRsc_GetLinkToExam		},
   [ActNewExaSet	] = {1898,-1,TabUnk,ActSeeAllExa	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,ExaSet_ReceiveSet		},
   [ActReqRemExaSet	] = {1893,-1,TabUnk,ActSeeAllExa	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,ExaSet_ReqRemSet		},
   [ActRemExaSet	] = {1894,-1,TabUnk,ActSeeAllExa	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,ExaSet_RemoveSet		},
   [ActUp_ExaSet	] = {1895,-1,TabUnk,ActSeeAllExa	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,ExaSet_MoveUpSet		},
   [ActDwnExaSet	] = {1896,-1,TabUnk,ActSeeAllExa	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,ExaSet_MoveDownSet		},
   [ActChgTitExaSet	] = {1897,-1,TabUnk,ActSeeAllExa	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,ExaSet_ChangeSetTitle		},
   [ActChgNumQstExaSet	] = {1899,-1,TabUnk,ActSeeAllExa	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,ExaSet_ChangeNumQstsToExam	},
   [ActReqAddQstExaSet	] = {1885,-1,TabUnk,ActSeeAllExa	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,Dat_SetDatesToPastAndNow	,ExaSet_ReqSelectQstsToAddToSet	},
   [ActLstTstQstForSet	] = {1886,-1,TabUnk,ActSeeAllExa	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,ExaSet_ListQstsToAddToSet	},
   [ActAddQstToExa	] = {1887,-1,TabUnk,ActSeeAllExa	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,ExaSet_AddQstsToSet		},
   [ActReqRemSetQst	] = {1888,-1,TabUnk,ActSeeAllExa	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,ExaSet_ReqRemQstFromSet	},
   [ActRemExaQst	] = {1889,-1,TabUnk,ActSeeAllExa	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,ExaSet_RemoveQstFromSet	},
   [ActValSetQst	] = {1909,-1,TabUnk,ActSeeAllExa	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,ExaSet_ValidateQst		},
   [ActInvSetQst	] = {1910,-1,TabUnk,ActSeeAllExa	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,ExaSet_InvalidateQst		},
   [ActReqNewExaSes	] = {1852,-1,TabUnk,ActSeeAllExa	,{0x200,0x230},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,ExaSes_ReqCreatOrEditSes	},
   [ActReqChgExaSes	] = {1902,-1,TabUnk,ActSeeAllExa	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,ExaSes_ReqCreatOrEditSes	},
   [ActNewExaSes	] = {1853,-1,TabUnk,ActSeeAllExa	,{0x200,0x230},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,ExaSes_ReceiveSession		},
   [ActChgExaSes	] = {1903,-1,TabUnk,ActSeeAllExa	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,ExaSes_ReceiveSession		},
   [ActReqRemExaSes	] = {1850,-1,TabUnk,ActSeeAllExa	,{0x200,0x230},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,ExaSes_ReqRemSession		},
   [ActRemExaSes	] = {1851,-1,TabUnk,ActSeeAllExa	,{0x200,0x230},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,ExaSes_RemoveSession		},
   [ActHidExaSes	] = {1900,-1,TabUnk,ActSeeAllExa	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,ExaSes_HideSession		},
   [ActUnhExaSes	] = {1901,-1,TabUnk,ActSeeAllExa	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,ExaSes_UnhideSession		},
   [ActSeeExaPrn	] = {1904,-1,TabUnk,ActSeeAllExa	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,ExaPrn_ShowExamPrint		},
   [ActAnsExaPrn	] = {1906,-1,TabUnk,ActSeeAllExa	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_AJAX_NR,NULL			,ExaPrn_ReceivePrintAnswer	},
   [ActEndExaPrn	] = {1908,-1,TabUnk,ActSeeAllExa	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_NEW_TAB,NULL			,ExaRes_ShowExaResultAfterFinish},
   [ActSeeMyExaResCrs	] = {1867,-1,TabUnk,ActSeeAllExa	,{0x200,0x208},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,ExaRes_ShowMyResultsInCrs	},
   [ActSeeMyExaResExa	] = {1868,-1,TabUnk,ActSeeAllExa	,{0x200,0x208},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,ExaRes_ShowMyResultsInExa	},
   [ActSeeMyExaResSes	] = {1869,-1,TabUnk,ActSeeAllExa	,{0x200,0x208},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,ExaRes_ShowMyResultsInSes	},
   [ActSeeOneExaResMe	] = {1870,-1,TabUnk,ActSeeAllExa	,{0x200,0x208},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,ExaRes_ShowOneExaResult	},
   [ActReqSeeUsrExaRes	] = {1871,-1,TabUnk,ActSeeAllExa	,{0x200,0x230},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,ExaRes_SelUsrsToViewResults	},
   [ActSeeUsrExaResCrs	] = {1872,-1,TabUnk,ActSeeAllExa	,{0x200,0x230},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,ExaRes_ShowAllResultsInCrs	},
   [ActSeeUsrExaResExa	] = {1873,-1,TabUnk,ActSeeAllExa	,{0x200,0x230},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,ExaRes_ShowAllResultsInExa	},
   [ActSeeUsrExaResSes	] = {1874,-1,TabUnk,ActSeeAllExa	,{0x200,0x230},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,ExaRes_ShowAllResultsInSes	},
   [ActSeeOneExaResOth	] = {1875,-1,TabUnk,ActSeeAllExa	,{0x200,0x230},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,ExaRes_ShowOneExaResult	},
   [ActChgVisExaRes	] = {1876,-1,TabUnk,ActSeeAllExa	,{0x200,0x230},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,ExaSes_ToggleVisResultsSesUsr	},

   // Games
   [ActSeeAllGam	] = {1649, 6,TabAss,ActSeeAllGam	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Gam_SeeAllGames		},
   [ActSeeOneGam	] = {1650,-1,TabUnk,ActSeeAllGam	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Gam_SeeOneGame			},
   [ActReqNewMch	] = {1670,-1,TabUnk,ActSeeAllGam	,{0x200,0x230},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Mch_ReqCreatOrEditMatch	},
   [ActReqChgMch	] = {1913,-1,TabUnk,ActSeeAllGam	,{0x200,0x230},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Mch_ReqCreatOrEditMatch	},
   [ActNewMch		] = {1671,-1,TabUnk,ActSeeAllGam	,{0x200,0x230},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_NEW_TAB,Mch_CreateNewMatch	,Mch_ResumeMatch		},
   [ActChgMch		] = {1914,-1,TabUnk,ActSeeAllGam	,{0x200,0x230},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Mch_ChangeMatch		},
   [ActReqRemMch	] = {1783,-1,TabUnk,ActSeeAllGam	,{0x200,0x230},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Mch_ReqRemMatch		},
   [ActRemMch		] = {1784,-1,TabUnk,ActSeeAllGam	,{0x200,0x230},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Mch_RemoveMatch		},
   [ActResMch		] = {1785,-1,TabUnk,ActSeeAllGam	,{0x200,0x230},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_NEW_TAB,Mch_GetMatchBeingPlayed	,Mch_ResumeMatch		},
   [ActBckMch		] = {1790,-1,TabUnk,ActSeeAllGam	,{0x200,0x230},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_2ND_TAB,Mch_GetMatchBeingPlayed	,Mch_BackMatch			},
   [ActPlyPauMch	] = {1789,-1,TabUnk,ActSeeAllGam	,{0x200,0x230},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_2ND_TAB,Mch_GetMatchBeingPlayed	,Mch_PlayPauseMatch		},
   [ActFwdMch		] = {1672,-1,TabUnk,ActSeeAllGam	,{0x200,0x230},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_2ND_TAB,Mch_GetMatchBeingPlayed	,Mch_ForwardMatch		},
   [ActChgNumColMch	] = {1802,-1,TabUnk,ActSeeAllGam	,{0x200,0x230},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_2ND_TAB,Mch_GetMatchBeingPlayed	,Mch_ChangeNumColsMch		},
   [ActChgVisResMchQst	] = {1794,-1,TabUnk,ActSeeAllGam	,{0x200,0x230},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_2ND_TAB,Mch_GetMatchBeingPlayed	,Mch_ToggleVisResultsMchQst	},
   [ActMchCntDwn	] = {1814,-1,TabUnk,ActSeeAllGam	,{0x200,0x230},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_AJAX_NR,Mch_GetMatchBeingPlayed	,Mch_StartCountdown		},
   [ActRefMchTch	] = {1788,-1,TabUnk,ActSeeAllGam	,{0x200,0x230},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_AJAX_RF,Mch_GetMatchBeingPlayed	,Mch_RefreshMatchTch		},
   [ActJoiMch		] = {1780,-1,TabUnk,ActSeeAllGam	,{    0,0x008},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_NEW_TAB,Mch_GetMatchBeingPlayed	,Mch_JoinMatchAsStd		},
   [ActSeeMchAnsQstStd	] = {1808,-1,TabUnk,ActSeeAllGam	,{    0,0x008},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_2ND_TAB,Mch_GetMatchBeingPlayed	,Mch_JoinMatchAsStd		},
   [ActRemMchAnsQstStd	] = {1809,-1,TabUnk,ActSeeAllGam	,{    0,0x008},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_2ND_TAB,Mch_GetMatchBeingPlayed	,Mch_RemMyQstAnsAndShowMchStatus},
   [ActAnsMchQstStd	] = {1651,-1,TabUnk,ActSeeAllGam	,{    0,0x008},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_2ND_TAB,Mch_GetMatchBeingPlayed	,Mch_ReceiveQuestionAnswer	},
   [ActRefMchStd	] = {1782,-1,TabUnk,ActSeeAllGam	,{    0,0x008},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_AJAX_RF,Mch_GetMatchBeingPlayed	,Mch_RefreshMatchStd		},
   [ActSeeMyMchResCrs	] = {1796,-1,TabUnk,ActSeeAllGam	,{0x200,0x208},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,MchRes_ShowMyMchResultsInCrs	},
   [ActSeeMyMchResGam	] = {1810,-1,TabUnk,ActSeeAllGam	,{0x200,0x208},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,MchRes_ShowMyMchResultsInGam	},
   [ActSeeMyMchResMch	] = {1812,-1,TabUnk,ActSeeAllGam	,{0x200,0x208},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,MchRes_ShowMyMchResultsInMch	},
   [ActSeeOneMchResMe	] = {1797,-1,TabUnk,ActSeeAllGam	,{0x200,0x208},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,MchRes_ShowOneMchResult	},
   [ActReqSeeUsrMchRes	] = {1798,-1,TabUnk,ActSeeAllGam	,{0x200,0x230},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,MchRes_SelUsrsToViewMchResults	},
   [ActSeeUsrMchResCrs	] = {1799,-1,TabUnk,ActSeeAllGam	,{0x200,0x230},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,MchRes_ShowAllMchResultsInCrs	},
   [ActSeeUsrMchResGam	] = {1811,-1,TabUnk,ActSeeAllGam	,{0x200,0x230},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,MchRes_ShowAllMchResultsInGam	},
   [ActSeeUsrMchResMch	] = {1813,-1,TabUnk,ActSeeAllGam	,{0x200,0x230},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,MchRes_ShowAllMchResultsInMch	},
   [ActSeeOneMchResOth	] = {1800,-1,TabUnk,ActSeeAllGam	,{0x200,0x230},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,MchRes_ShowOneMchResult	},
   [ActChgVisResMchUsr	] = {1801,-1,TabUnk,ActSeeAllGam	,{0x200,0x230},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Mch_ToggleVisResultsMchUsr	},
   [ActLstOneGam	] = {1912,-1,TabUnk,ActSeeAllGam	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Gam_ListGame			},
   [ActFrmNewGam	] = {1652,-1,TabUnk,ActSeeAllGam	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Gam_ReqCreatOrEditGame		},
   [ActEdiOneGam	] = {1653,-1,TabUnk,ActSeeAllGam	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Gam_ReqCreatOrEditGame		},
   [ActNewGam		] = {1654,-1,TabUnk,ActSeeAllGam	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Gam_ReceiveGame		},
   [ActChgGam		] = {1655,-1,TabUnk,ActSeeAllGam	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Gam_ReceiveGame		},
   [ActReqRemGam	] = {1656,-1,TabUnk,ActSeeAllGam	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Gam_AskRemGame			},
   [ActRemGam		] = {1657,-1,TabUnk,ActSeeAllGam	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Gam_RemoveGame			},
   [ActHidGam		] = {1660,-1,TabUnk,ActSeeAllGam	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Gam_HideGame			},
   [ActUnhGam		] = {1661,-1,TabUnk,ActSeeAllGam	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Gam_UnhideGame			},
   [ActAddOneGamQst	] = {1662,-1,TabUnk,ActSeeAllGam	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,Dat_SetDatesToPastAndNow	,Gam_ReqSelectQstsToAddToGame	},
   [ActGamLstTstQst	] = {1666,-1,TabUnk,ActSeeAllGam	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Gam_ListQstsToAddToGame	},
   [ActAddTstQstToGam	] = {1667,-1,TabUnk,ActSeeAllGam	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Gam_AddQstsToGame		},
   [ActReqRemGamQst	] = {1664,-1,TabUnk,ActSeeAllGam	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Gam_ReqRemQstFromGame		},
   [ActRemGamQst	] = {1665,-1,TabUnk,ActSeeAllGam	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Gam_RemoveQstFromGame		},
   [ActUp_GamQst	] = {1668,-1,TabUnk,ActSeeAllGam	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Gam_MoveUpQst			},
   [ActDwnGamQst	] = {1669,-1,TabUnk,ActSeeAllGam	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Gam_MoveDownQst		},
   [ActReqLnkGam	] = {1935,-1,TabUnk,ActSeeAllGam	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,GamRsc_GetLinkToGame		},

   // Rubrics
   [ActSeeAllRub	] = {1951, 7,TabAss,ActSeeAllRub	,{0x200,0x200},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Rub_SeeAllRubrics		},
   [ActSeeOneRub	] = {1952,-1,TabUnk,ActSeeAllRub	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Rub_SeeOneRubric		},
   [ActFrmNewRub	] = {1953,-1,TabUnk,ActSeeAllRub	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Rub_ReqCreatOrEditRubric	},
   [ActEdiOneRub	] = {1954,-1,TabUnk,ActSeeAllRub	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Rub_ReqCreatOrEditRubric	},
   [ActNewRub		] = {1955,-1,TabUnk,ActSeeAllRub	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Rub_ReceiveRubric		},
   [ActChgRub		] = {1956,-1,TabUnk,ActSeeAllRub	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Rub_ReceiveRubric		},
   [ActReqRemRub	] = {1957,-1,TabUnk,ActSeeAllRub	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Rub_AskRemRubric		},
   [ActRemRub		] = {1958,-1,TabUnk,ActSeeAllRub	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Rub_RemoveRubric		},
   [ActReqLnkRub	] = {1969,-1,TabUnk,ActSeeAllRub	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,RubRsc_GetLinkToRubric		},
   [ActSeeRscCli_InRub	] = {1972,-1,TabUnk,ActSeeAllRub	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,RubRsc_ViewResourceClipboard	},
   [ActRemRscCli_InRub	] = {1973,-1,TabUnk,ActSeeAllRub	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,RubRsc_RemoveResourceClipboard	},
   [ActNewRubCri	] = {1959,-1,TabUnk,ActSeeAllRub	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,RubCri_ReceiveCriterion	},
   [ActReqRemRubCri	] = {1960,-1,TabUnk,ActSeeAllRub	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,RubCri_ReqRemCriterion		},
   [ActRemRubCri	] = {1961,-1,TabUnk,ActSeeAllRub	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,RubCri_RemoveCriterion		},
   [ActUp_RubCri	] = {1962,-1,TabUnk,ActSeeAllRub	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,RubCri_MoveUpCriterion		},
   [ActDwnRubCri	] = {1963,-1,TabUnk,ActSeeAllRub	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,RubCri_MoveDownCriterion	},
   [ActChgTitRubCri	] = {1964,-1,TabUnk,ActSeeAllRub	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,RubCri_ChangeTitle		},
   [ActChgLnkRubCri	] = {1968,-1,TabUnk,ActSeeAllRub	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,RubCri_ChangeLink		},
   [ActChgMinRubCri	] = {1965,-1,TabUnk,ActSeeAllRub	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,RubCri_ChangeMinValue		},
   [ActChgMaxRubCri	] = {1966,-1,TabUnk,ActSeeAllRub	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,RubCri_ChangeMaxValue		},
   [ActChgWeiRubCri	] = {1967,-1,TabUnk,ActSeeAllRub	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,RubCri_ChangeWeight		},

   // TabFil ******************************************************************
   // Documents of institution
   [ActSeeAdmDocIns	] = {1249, 0,TabFil,ActSeeAdmDocIns	,{    0,    0},    0,    0,0x3C7,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_ShowFileBrowserOrWorks	},
   [ActChgToSeeDocIns	] = {1308,-1,TabUnk,ActSeeAdmDocIns	,{    0,    0},    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_ShowFileBrowserOrWorks	},
   [ActSeeDocIns	] = {1309,-1,TabUnk,ActSeeAdmDocIns	,{    0,    0},    0,    0,0x3C7,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_ShowFileBrowserOrWorks	},
   [ActExpSeeDocIns	] = {1310,-1,TabUnk,ActSeeAdmDocIns	,{    0,    0},    0,    0,0x3C7,    0,    0,Act_CONT_NORM,Act_204_NOC,Brw_ExpandFileTree	,NULL				},
   [ActConSeeDocIns	] = {1311,-1,TabUnk,ActSeeAdmDocIns	,{    0,    0},    0,    0,0x3C7,    0,    0,Act_CONT_NORM,Act_204_NOC,Brw_ContractFileTree	,NULL				},
   [ActZIPSeeDocIns	] = {1312,-1,TabUnk,ActSeeAdmDocIns	,{    0,    0},    0,    0,0x3C7,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,ZIP_CompressFileTree		},
   [ActReqDatSeeDocIns	] = {1313,-1,TabUnk,ActSeeAdmDocIns	,{    0,    0},    0,    0,0x3C7,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_ShowFileMetadata		},
   [ActDowSeeDocIns	] = {1314,-1,TabUnk,ActSeeAdmDocIns	,{    0,    0},    0,    0,0x3C7,    0,    0,Act_CONT_NORM,Act_DWN_FIL,Brw_DownloadFile		,NULL				},
   [ActChgToAdmDocIns	] = {1315,-1,TabUnk,ActSeeAdmDocIns	,{    0,    0},    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_ShowFileBrowserOrWorks	},
   [ActAdmDocIns	] = {1316,-1,TabUnk,ActSeeAdmDocIns	,{    0,    0},    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_ShowFileBrowserOrWorks	},
   [ActReqRemFilDocIns	] = {1317,-1,TabUnk,ActSeeAdmDocIns	,{    0,    0},    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_AskRemFileFromTree		},
   [ActRemFilDocIns	] = {1318,-1,TabUnk,ActSeeAdmDocIns	,{    0,    0},    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_RemFileFromTree		},
   [ActRemFolDocIns	] = {1319,-1,TabUnk,ActSeeAdmDocIns	,{    0,    0},    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_RemFolderFromTree		},
   [ActCopDocIns	] = {1320,-1,TabUnk,ActSeeAdmDocIns	,{    0,    0},    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_CopyFromFileBrowser	},
   [ActPasDocIns	] = {1321,-1,TabUnk,ActSeeAdmDocIns	,{    0,    0},    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_PasteIntoFileBrowser	},
   [ActRemTreDocIns	] = {1322,-1,TabUnk,ActSeeAdmDocIns	,{    0,    0},    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_RemSubtreeInFileBrowser	},
   [ActFrmCreDocIns	] = {1323,-1,TabUnk,ActSeeAdmDocIns	,{    0,    0},    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_ShowFormFileBrowser	},
   [ActCreFolDocIns	] = {1324,-1,TabUnk,ActSeeAdmDocIns	,{    0,    0},    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_RecFolderFileBrowser	},
   [ActCreLnkDocIns	] = {1325,-1,TabUnk,ActSeeAdmDocIns	,{    0,    0},    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_RecLinkFileBrowser		},
   [ActRenFolDocIns	] = {1326,-1,TabUnk,ActSeeAdmDocIns	,{    0,    0},    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_RenFolderFileBrowser	},
   [ActRcvFilDocInsDZ	] = {1327,-1,TabUnk,ActSeeAdmDocIns	,{    0,    0},    0,    0,0x300,    0,    0,Act_CONT_DATA,Act_UPL_FIL,Brw_RcvFileInFileBrwDZ	,NULL				},
   [ActRcvFilDocInsCla	] = {1328,-1,TabUnk,ActSeeAdmDocIns	,{    0,    0},    0,    0,0x300,    0,    0,Act_CONT_DATA,Act_1ST_TAB,NULL			,Brw_RcvFileInFileBrwClassic	},
   [ActExpAdmDocIns	] = {1329,-1,TabUnk,ActSeeAdmDocIns	,{    0,    0},    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_204_NOC,Brw_ExpandFileTree	,NULL				},
   [ActConAdmDocIns	] = {1330,-1,TabUnk,ActSeeAdmDocIns	,{    0,    0},    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_204_NOC,Brw_ContractFileTree	,NULL				},
   [ActZIPAdmDocIns	] = {1331,-1,TabUnk,ActSeeAdmDocIns	,{    0,    0},    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,ZIP_CompressFileTree		},
   [ActUnhDocIns	] = {1332,-1,TabUnk,ActSeeAdmDocIns	,{    0,    0},    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_SetDocumentAsVisible	},
   [ActHidDocIns	] = {1333,-1,TabUnk,ActSeeAdmDocIns	,{    0,    0},    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_SetDocumentAsHidden	},
   [ActReqDatAdmDocIns	] = {1334,-1,TabUnk,ActSeeAdmDocIns	,{    0,    0},    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_ShowFileMetadata		},
   [ActChgDatAdmDocIns	] = {1335,-1,TabUnk,ActSeeAdmDocIns	,{    0,    0},    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_ChgFileMetadata		},
   [ActDowAdmDocIns   	] = {1336,-1,TabUnk,ActSeeAdmDocIns	,{    0,    0},    0,    0,0x300,    0,    0,Act_CONT_NORM,Act_DWN_FIL,Brw_DownloadFile		,NULL				},

   // Shared files of institution
   [ActAdmShaIns	] = {1382, 1,TabFil,ActAdmShaIns	,{    0,    0},    0,    0,0x3C7,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_ShowFileBrowserOrWorks	},
   [ActReqRemFilShaIns	] = {1383,-1,TabUnk,ActAdmShaIns	,{    0,    0},    0,    0,0x3C0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_AskRemFileFromTree		},
   [ActRemFilShaIns	] = {1384,-1,TabUnk,ActAdmShaIns	,{    0,    0},    0,    0,0x3C0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_RemFileFromTree		},
   [ActRemFolShaIns	] = {1385,-1,TabUnk,ActAdmShaIns	,{    0,    0},    0,    0,0x3C0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_RemFolderFromTree		},
   [ActCopShaIns	] = {1386,-1,TabUnk,ActAdmShaIns	,{    0,    0},    0,    0,0x3C0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_CopyFromFileBrowser	},
   [ActPasShaIns	] = {1387,-1,TabUnk,ActAdmShaIns	,{    0,    0},    0,    0,0x3C0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_PasteIntoFileBrowser	},
   [ActRemTreShaIns	] = {1388,-1,TabUnk,ActAdmShaIns	,{    0,    0},    0,    0,0x3C0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_RemSubtreeInFileBrowser	},
   [ActFrmCreShaIns	] = {1389,-1,TabUnk,ActAdmShaIns	,{    0,    0},    0,    0,0x3C0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_ShowFormFileBrowser	},
   [ActCreFolShaIns	] = {1390,-1,TabUnk,ActAdmShaIns	,{    0,    0},    0,    0,0x3C0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_RecFolderFileBrowser	},
   [ActCreLnkShaIns	] = {1391,-1,TabUnk,ActAdmShaIns	,{    0,    0},    0,    0,0x3C0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_RecLinkFileBrowser		},
   [ActRenFolShaIns	] = {1392,-1,TabUnk,ActAdmShaIns	,{    0,    0},    0,    0,0x3C0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_RenFolderFileBrowser	},
   [ActRcvFilShaInsDZ	] = {1393,-1,TabUnk,ActAdmShaIns	,{    0,    0},    0,    0,0x3C0,    0,    0,Act_CONT_DATA,Act_UPL_FIL,Brw_RcvFileInFileBrwDZ	,NULL				},
   [ActRcvFilShaInsCla	] = {1394,-1,TabUnk,ActAdmShaIns	,{    0,    0},    0,    0,0x3C0,    0,    0,Act_CONT_DATA,Act_1ST_TAB,NULL			,Brw_RcvFileInFileBrwClassic	},
   [ActExpShaIns	] = {1395,-1,TabUnk,ActAdmShaIns	,{    0,    0},    0,    0,0x3C0,    0,    0,Act_CONT_NORM,Act_204_NOC,Brw_ExpandFileTree	,NULL				},
   [ActConShaIns	] = {1396,-1,TabUnk,ActAdmShaIns	,{    0,    0},    0,    0,0x3C0,    0,    0,Act_CONT_NORM,Act_204_NOC,Brw_ContractFileTree	,NULL				},
   [ActZIPShaIns	] = {1397,-1,TabUnk,ActAdmShaIns	,{    0,    0},    0,    0,0x3C0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,ZIP_CompressFileTree		},
   [ActReqDatShaIns	] = {1398,-1,TabUnk,ActAdmShaIns	,{    0,    0},    0,    0,0x3C7,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_ShowFileMetadata		},
   [ActChgDatShaIns	] = {1399,-1,TabUnk,ActAdmShaIns	,{    0,    0},    0,    0,0x3C0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_ChgFileMetadata		},
   [ActDowShaIns	] = {1400,-1,TabUnk,ActAdmShaIns	,{    0,    0},    0,    0,0x3C7,    0,    0,Act_CONT_NORM,Act_DWN_FIL,Brw_DownloadFile		,NULL				},

   // Documents of center
   [ActSeeAdmDocCtr	] = {1248, 2,TabFil,ActSeeAdmDocCtr	,{    0,    0},    0,0x3C7,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_ShowFileBrowserOrWorks	},
   [ActChgToSeeDocCtr	] = {1279,-1,TabUnk,ActSeeAdmDocCtr	,{    0,    0},    0,0x380,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_ShowFileBrowserOrWorks	},
   [ActSeeDocCtr	] = {1280,-1,TabUnk,ActSeeAdmDocCtr	,{    0,    0},    0,0x3C7,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_ShowFileBrowserOrWorks	},
   [ActExpSeeDocCtr	] = {1281,-1,TabUnk,ActSeeAdmDocCtr	,{    0,    0},    0,0x3C7,    0,    0,    0,Act_CONT_NORM,Act_204_NOC,Brw_ExpandFileTree	,NULL				},
   [ActConSeeDocCtr	] = {1282,-1,TabUnk,ActSeeAdmDocCtr	,{    0,    0},    0,0x3C7,    0,    0,    0,Act_CONT_NORM,Act_204_NOC,Brw_ContractFileTree	,NULL				},
   [ActZIPSeeDocCtr	] = {1283,-1,TabUnk,ActSeeAdmDocCtr	,{    0,    0},    0,0x3C7,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,ZIP_CompressFileTree		},
   [ActReqDatSeeDocCtr	] = {1284,-1,TabUnk,ActSeeAdmDocCtr	,{    0,    0},    0,0x3C7,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_ShowFileMetadata		},
   [ActDowSeeDocCtr   	] = {1285,-1,TabUnk,ActSeeAdmDocCtr	,{    0,    0},    0,0x3C7,    0,    0,    0,Act_CONT_NORM,Act_DWN_FIL,Brw_DownloadFile		,NULL				},
   [ActChgToAdmDocCtr	] = {1286,-1,TabUnk,ActSeeAdmDocCtr	,{    0,    0},    0,0x380,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_ShowFileBrowserOrWorks	},
   [ActAdmDocCtr	] = {1287,-1,TabUnk,ActSeeAdmDocCtr	,{    0,    0},    0,0x380,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_ShowFileBrowserOrWorks	},
   [ActReqRemFilDocCtr	] = {1288,-1,TabUnk,ActSeeAdmDocCtr	,{    0,    0},    0,0x380,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_AskRemFileFromTree		},
   [ActRemFilDocCtr	] = {1289,-1,TabUnk,ActSeeAdmDocCtr	,{    0,    0},    0,0x380,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_RemFileFromTree		},
   [ActRemFolDocCtr	] = {1290,-1,TabUnk,ActSeeAdmDocCtr	,{    0,    0},    0,0x380,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_RemFolderFromTree		},
   [ActCopDocCtr	] = {1291,-1,TabUnk,ActSeeAdmDocCtr	,{    0,    0},    0,0x380,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_CopyFromFileBrowser	},
   [ActPasDocCtr	] = {1292,-1,TabUnk,ActSeeAdmDocCtr	,{    0,    0},    0,0x380,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_PasteIntoFileBrowser	},
   [ActRemTreDocCtr	] = {1293,-1,TabUnk,ActSeeAdmDocCtr	,{    0,    0},    0,0x380,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_RemSubtreeInFileBrowser	},
   [ActFrmCreDocCtr	] = {1294,-1,TabUnk,ActSeeAdmDocCtr	,{    0,    0},    0,0x380,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_ShowFormFileBrowser	},
   [ActCreFolDocCtr	] = {1295,-1,TabUnk,ActSeeAdmDocCtr	,{    0,    0},    0,0x380,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_RecFolderFileBrowser	},
   [ActCreLnkDocCtr	] = {1296,-1,TabUnk,ActSeeAdmDocCtr	,{    0,    0},    0,0x380,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_RecLinkFileBrowser		},
   [ActRenFolDocCtr	] = {1297,-1,TabUnk,ActSeeAdmDocCtr	,{    0,    0},    0,0x380,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_RenFolderFileBrowser	},
   [ActRcvFilDocCtrDZ	] = {1298,-1,TabUnk,ActSeeAdmDocCtr	,{    0,    0},    0,0x380,    0,    0,    0,Act_CONT_DATA,Act_UPL_FIL,Brw_RcvFileInFileBrwDZ	,NULL				},
   [ActRcvFilDocCtrCla	] = {1299,-1,TabUnk,ActSeeAdmDocCtr	,{    0,    0},    0,0x380,    0,    0,    0,Act_CONT_DATA,Act_1ST_TAB,NULL			,Brw_RcvFileInFileBrwClassic	},
   [ActExpAdmDocCtr	] = {1300,-1,TabUnk,ActSeeAdmDocCtr	,{    0,    0},    0,0x380,    0,    0,    0,Act_CONT_NORM,Act_204_NOC,Brw_ExpandFileTree	,NULL				},
   [ActConAdmDocCtr	] = {1301,-1,TabUnk,ActSeeAdmDocCtr	,{    0,    0},    0,0x380,    0,    0,    0,Act_CONT_NORM,Act_204_NOC,Brw_ContractFileTree	,NULL				},
   [ActZIPAdmDocCtr	] = {1302,-1,TabUnk,ActSeeAdmDocCtr	,{    0,    0},    0,0x380,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,ZIP_CompressFileTree		},
   [ActUnhDocCtr	] = {1303,-1,TabUnk,ActSeeAdmDocCtr	,{    0,    0},    0,0x380,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_SetDocumentAsVisible	},
   [ActHidDocCtr	] = {1304,-1,TabUnk,ActSeeAdmDocCtr	,{    0,    0},    0,0x380,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_SetDocumentAsHidden	},
   [ActReqDatAdmDocCtr	] = {1305,-1,TabUnk,ActSeeAdmDocCtr	,{    0,    0},    0,0x380,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_ShowFileMetadata		},
   [ActChgDatAdmDocCtr	] = {1306,-1,TabUnk,ActSeeAdmDocCtr	,{    0,    0},    0,0x380,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_ChgFileMetadata		},
   [ActDowAdmDocCtr   	] = {1307,-1,TabUnk,ActSeeAdmDocCtr	,{    0,    0},    0,0x380,    0,    0,    0,Act_CONT_NORM,Act_DWN_FIL,Brw_DownloadFile		,NULL				},

   // Shared files of center
   [ActAdmShaCtr	] = {1363, 3,TabFil,ActAdmShaCtr	,{    0,    0},    0,0x3C7,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_ShowFileBrowserOrWorks	},
   [ActReqRemFilShaCtr	] = {1364,-1,TabUnk,ActAdmShaCtr	,{    0,    0},    0,0x3C0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_AskRemFileFromTree		},
   [ActRemFilShaCtr	] = {1365,-1,TabUnk,ActAdmShaCtr	,{    0,    0},    0,0x3C0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_RemFileFromTree		},
   [ActRemFolShaCtr	] = {1366,-1,TabUnk,ActAdmShaCtr	,{    0,    0},    0,0x3C0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_RemFolderFromTree		},
   [ActCopShaCtr	] = {1367,-1,TabUnk,ActAdmShaCtr	,{    0,    0},    0,0x3C0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_CopyFromFileBrowser	},
   [ActPasShaCtr	] = {1368,-1,TabUnk,ActAdmShaCtr	,{    0,    0},    0,0x3C0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_PasteIntoFileBrowser	},
   [ActRemTreShaCtr	] = {1369,-1,TabUnk,ActAdmShaCtr	,{    0,    0},    0,0x3C0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_RemSubtreeInFileBrowser	},
   [ActFrmCreShaCtr	] = {1370,-1,TabUnk,ActAdmShaCtr	,{    0,    0},    0,0x3C0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_ShowFormFileBrowser	},
   [ActCreFolShaCtr	] = {1371,-1,TabUnk,ActAdmShaCtr	,{    0,    0},    0,0x3C0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_RecFolderFileBrowser	},
   [ActCreLnkShaCtr	] = {1372,-1,TabUnk,ActAdmShaCtr	,{    0,    0},    0,0x3C0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_RecLinkFileBrowser		},
   [ActRenFolShaCtr	] = {1373,-1,TabUnk,ActAdmShaCtr	,{    0,    0},    0,0x3C0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_RenFolderFileBrowser	},
   [ActRcvFilShaCtrDZ	] = {1374,-1,TabUnk,ActAdmShaCtr	,{    0,    0},    0,0x3C0,    0,    0,    0,Act_CONT_DATA,Act_UPL_FIL,Brw_RcvFileInFileBrwDZ	,NULL				},
   [ActRcvFilShaCtrCla	] = {1375,-1,TabUnk,ActAdmShaCtr	,{    0,    0},    0,0x3C0,    0,    0,    0,Act_CONT_DATA,Act_1ST_TAB,NULL			,Brw_RcvFileInFileBrwClassic	},
   [ActExpShaCtr	] = {1376,-1,TabUnk,ActAdmShaCtr	,{    0,    0},    0,0x3C0,    0,    0,    0,Act_CONT_NORM,Act_204_NOC,Brw_ExpandFileTree	,NULL				},
   [ActConShaCtr	] = {1377,-1,TabUnk,ActAdmShaCtr	,{    0,    0},    0,0x3C0,    0,    0,    0,Act_CONT_NORM,Act_204_NOC,Brw_ContractFileTree	,NULL				},
   [ActZIPShaCtr	] = {1378,-1,TabUnk,ActAdmShaCtr	,{    0,    0},    0,0x3C0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,ZIP_CompressFileTree		},
   [ActReqDatShaCtr	] = {1379,-1,TabUnk,ActAdmShaCtr	,{    0,    0},    0,0x3C7,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_ShowFileMetadata		},
   [ActChgDatShaCtr	] = {1380,-1,TabUnk,ActAdmShaCtr	,{    0,    0},    0,0x3C0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_ChgFileMetadata		},
   [ActDowShaCtr	] = {1381,-1,TabUnk,ActAdmShaCtr	,{    0,    0},    0,0x3C7,    0,    0,    0,Act_CONT_NORM,Act_DWN_FIL,Brw_DownloadFile		,NULL				},

   // Documents of degree
   [ActSeeAdmDocDeg	] = {1247, 4,TabFil,ActSeeAdmDocDeg	,{    0,    0},0x3C7,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_ShowFileBrowserOrWorks	},
   [ActChgToSeeDocDeg	] = {1250,-1,TabUnk,ActSeeAdmDocDeg	,{    0,    0},0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_ShowFileBrowserOrWorks	},
   [ActSeeDocDeg	] = {1251,-1,TabUnk,ActSeeAdmDocDeg	,{    0,    0},0x3C7,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_ShowFileBrowserOrWorks	},
   [ActExpSeeDocDeg	] = {1252,-1,TabUnk,ActSeeAdmDocDeg	,{    0,    0},0x3C7,    0,    0,    0,    0,Act_CONT_NORM,Act_204_NOC,Brw_ExpandFileTree	,NULL				},
   [ActConSeeDocDeg	] = {1253,-1,TabUnk,ActSeeAdmDocDeg	,{    0,    0},0x3C7,    0,    0,    0,    0,Act_CONT_NORM,Act_204_NOC,Brw_ContractFileTree	,NULL				},
   [ActZIPSeeDocDeg	] = {1254,-1,TabUnk,ActSeeAdmDocDeg	,{    0,    0},0x3C7,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,ZIP_CompressFileTree		},
   [ActReqDatSeeDocDeg	] = {1255,-1,TabUnk,ActSeeAdmDocDeg	,{    0,    0},0x3C7,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_ShowFileMetadata		},
   [ActDowSeeDocDeg	] = {1256,-1,TabUnk,ActSeeAdmDocDeg	,{    0,    0},0x3C7,    0,    0,    0,    0,Act_CONT_NORM,Act_DWN_FIL,Brw_DownloadFile		,NULL				},
   [ActChgToAdmDocDeg	] = {1257,-1,TabUnk,ActSeeAdmDocDeg	,{    0,    0},0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_ShowFileBrowserOrWorks	},
   [ActAdmDocDeg	] = {1258,-1,TabUnk,ActSeeAdmDocDeg	,{    0,    0},0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_ShowFileBrowserOrWorks	},
   [ActReqRemFilDocDeg	] = {1259,-1,TabUnk,ActSeeAdmDocDeg	,{    0,    0},0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_AskRemFileFromTree		},
   [ActRemFilDocDeg	] = {1260,-1,TabUnk,ActSeeAdmDocDeg	,{    0,    0},0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_RemFileFromTree		},
   [ActRemFolDocDeg	] = {1261,-1,TabUnk,ActSeeAdmDocDeg	,{    0,    0},0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_RemFolderFromTree		},
   [ActCopDocDeg	] = {1262,-1,TabUnk,ActSeeAdmDocDeg	,{    0,    0},0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_CopyFromFileBrowser	},
   [ActPasDocDeg	] = {1263,-1,TabUnk,ActSeeAdmDocDeg	,{    0,    0},0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_PasteIntoFileBrowser	},
   [ActRemTreDocDeg	] = {1264,-1,TabUnk,ActSeeAdmDocDeg	,{    0,    0},0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_RemSubtreeInFileBrowser	},
   [ActFrmCreDocDeg	] = {1265,-1,TabUnk,ActSeeAdmDocDeg	,{    0,    0},0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_ShowFormFileBrowser	},
   [ActCreFolDocDeg	] = {1266,-1,TabUnk,ActSeeAdmDocDeg	,{    0,    0},0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_RecFolderFileBrowser	},
   [ActCreLnkDocDeg	] = {1267,-1,TabUnk,ActSeeAdmDocDeg	,{    0,    0},0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_RecLinkFileBrowser		},
   [ActRenFolDocDeg	] = {1268,-1,TabUnk,ActSeeAdmDocDeg	,{    0,    0},0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_RenFolderFileBrowser	},
   [ActRcvFilDocDegDZ	] = {1269,-1,TabUnk,ActSeeAdmDocDeg	,{    0,    0},0x3C0,    0,    0,    0,    0,Act_CONT_DATA,Act_UPL_FIL,Brw_RcvFileInFileBrwDZ	,NULL				},
   [ActRcvFilDocDegCla	] = {1270,-1,TabUnk,ActSeeAdmDocDeg	,{    0,    0},0x3C0,    0,    0,    0,    0,Act_CONT_DATA,Act_1ST_TAB,NULL			,Brw_RcvFileInFileBrwClassic	},
   [ActExpAdmDocDeg	] = {1271,-1,TabUnk,ActSeeAdmDocDeg	,{    0,    0},0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_204_NOC,Brw_ExpandFileTree	,NULL				},
   [ActConAdmDocDeg	] = {1272,-1,TabUnk,ActSeeAdmDocDeg	,{    0,    0},0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_204_NOC,Brw_ContractFileTree	,NULL				},
   [ActZIPAdmDocDeg	] = {1273,-1,TabUnk,ActSeeAdmDocDeg	,{    0,    0},0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,ZIP_CompressFileTree		},
   [ActUnhDocDeg	] = {1274,-1,TabUnk,ActSeeAdmDocDeg	,{    0,    0},0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_SetDocumentAsVisible	},
   [ActHidDocDeg	] = {1275,-1,TabUnk,ActSeeAdmDocDeg	,{    0,    0},0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_SetDocumentAsHidden	},
   [ActReqDatAdmDocDeg	] = {1276,-1,TabUnk,ActSeeAdmDocDeg	,{    0,    0},0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_ShowFileMetadata		},
   [ActChgDatAdmDocDeg	] = {1277,-1,TabUnk,ActSeeAdmDocDeg	,{    0,    0},0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_ChgFileMetadata		},
   [ActDowAdmDocDeg	] = {1278,-1,TabUnk,ActSeeAdmDocDeg	,{    0,    0},0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_DWN_FIL,Brw_DownloadFile		,NULL				},

   // Shared files of degree
   [ActAdmShaDeg	] = {1344, 5,TabFil,ActAdmShaDeg	,{    0,    0},0x3C7,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_ShowFileBrowserOrWorks	},
   [ActReqRemFilShaDeg	] = {1345,-1,TabUnk,ActAdmShaDeg	,{    0,    0},0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_AskRemFileFromTree		},
   [ActRemFilShaDeg	] = {1346,-1,TabUnk,ActAdmShaDeg	,{    0,    0},0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_RemFileFromTree		},
   [ActRemFolShaDeg	] = {1347,-1,TabUnk,ActAdmShaDeg	,{    0,    0},0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_RemFolderFromTree		},
   [ActCopShaDeg	] = {1348,-1,TabUnk,ActAdmShaDeg	,{    0,    0},0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_CopyFromFileBrowser	},
   [ActPasShaDeg	] = {1349,-1,TabUnk,ActAdmShaDeg	,{    0,    0},0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_PasteIntoFileBrowser	},
   [ActRemTreShaDeg	] = {1350,-1,TabUnk,ActAdmShaDeg	,{    0,    0},0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_RemSubtreeInFileBrowser	},
   [ActFrmCreShaDeg	] = {1351,-1,TabUnk,ActAdmShaDeg	,{    0,    0},0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_ShowFormFileBrowser	},
   [ActCreFolShaDeg	] = {1352,-1,TabUnk,ActAdmShaDeg	,{    0,    0},0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_RecFolderFileBrowser	},
   [ActCreLnkShaDeg	] = {1353,-1,TabUnk,ActAdmShaDeg	,{    0,    0},0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_RecLinkFileBrowser		},
   [ActRenFolShaDeg	] = {1354,-1,TabUnk,ActAdmShaDeg	,{    0,    0},0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_RenFolderFileBrowser	},
   [ActRcvFilShaDegDZ	] = {1355,-1,TabUnk,ActAdmShaDeg	,{    0,    0},0x3C0,    0,    0,    0,    0,Act_CONT_DATA,Act_UPL_FIL,Brw_RcvFileInFileBrwDZ	,NULL				},
   [ActRcvFilShaDegCla	] = {1356,-1,TabUnk,ActAdmShaDeg	,{    0,    0},0x3C0,    0,    0,    0,    0,Act_CONT_DATA,Act_1ST_TAB,NULL			,Brw_RcvFileInFileBrwClassic	},
   [ActExpShaDeg	] = {1357,-1,TabUnk,ActAdmShaDeg	,{    0,    0},0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_204_NOC,Brw_ExpandFileTree	,NULL				},
   [ActConShaDeg	] = {1358,-1,TabUnk,ActAdmShaDeg	,{    0,    0},0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_204_NOC,Brw_ContractFileTree	,NULL				},
   [ActZIPShaDeg	] = {1359,-1,TabUnk,ActAdmShaDeg	,{    0,    0},0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,ZIP_CompressFileTree		},
   [ActReqDatShaDeg	] = {1360,-1,TabUnk,ActAdmShaDeg	,{    0,    0},0x3C7,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_ShowFileMetadata		},
   [ActChgDatShaDeg	] = {1361,-1,TabUnk,ActAdmShaDeg	,{    0,    0},0x3C0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_ChgFileMetadata		},
   [ActDowShaDeg	] = {1362,-1,TabUnk,ActAdmShaDeg	,{    0,    0},0x3C7,    0,    0,    0,    0,Act_CONT_NORM,Act_DWN_FIL,Brw_DownloadFile		,NULL				},

   // Documents of course
   [ActSeeAdmDocCrsGrp	] = {   0, 6,TabFil,ActSeeAdmDocCrsGrp	,{0x3C7,0x3F8},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_ShowFileBrowserOrWorks	},
   [ActChgToSeeDocCrs	] = {1195,-1,TabUnk,ActSeeAdmDocCrsGrp	,{0x3C7,0x3F8},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_ShowFileBrowserOrWorks	},
   [ActSeeDocCrs	] = {1078,-1,TabUnk,ActSeeAdmDocCrsGrp	,{0x3C7,0x3F8},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_ShowFileBrowserOrWorks	},
   [ActExpSeeDocCrs	] = { 462,-1,TabUnk,ActSeeAdmDocCrsGrp	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_204_NOC,Brw_ExpandFileTree	,NULL				},
   [ActConSeeDocCrs	] = { 476,-1,TabUnk,ActSeeAdmDocCrsGrp	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_204_NOC,Brw_ContractFileTree	,NULL				},
   [ActZIPSeeDocCrs	] = {1124,-1,TabUnk,ActSeeAdmDocCrsGrp	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,ZIP_CompressFileTree		},
   [ActReqDatSeeDocCrs	] = {1033,-1,TabUnk,ActSeeAdmDocCrsGrp	,{0x3C7,0x3F8},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_ShowFileMetadata		},
   [ActReqLnkSeeDocCrs	] = {1930,-1,TabUnk,ActSeeAdmDocCrsGrp	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,BrwRsc_GetLinkToDocFil		},
   [ActDowSeeDocCrs	] = {1111,-1,TabUnk,ActSeeAdmDocCrsGrp	,{0x3C7,0x3F8},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_DWN_FIL,Brw_DownloadFile		,NULL				},
   [ActSeeDocGrp	] = {1200,-1,TabUnk,ActSeeAdmDocCrsGrp	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_ShowFileBrowserOrWorks	},
   [ActExpSeeDocGrp	] = { 488,-1,TabUnk,ActSeeAdmDocCrsGrp	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_204_NOC,Brw_ExpandFileTree	,NULL				},
   [ActConSeeDocGrp	] = { 489,-1,TabUnk,ActSeeAdmDocCrsGrp	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_204_NOC,Brw_ContractFileTree	,NULL				},
   [ActZIPSeeDocGrp	] = {1125,-1,TabUnk,ActSeeAdmDocCrsGrp	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,ZIP_CompressFileTree		},
   [ActReqDatSeeDocGrp	] = {1034,-1,TabUnk,ActSeeAdmDocCrsGrp	,{0x3C7,0x3F8},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_ShowFileMetadata		},
   [ActDowSeeDocGrp	] = {1112,-1,TabUnk,ActSeeAdmDocCrsGrp	,{0x3C7,0x3F8},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_DWN_FIL,Brw_DownloadFile		,NULL				},
   [ActChgToAdmDocCrs	] = {1196,-1,TabUnk,ActSeeAdmDocCrsGrp	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_ShowFileBrowserOrWorks	},
   [ActAdmDocCrs	] = {  12,-1,TabUnk,ActSeeAdmDocCrsGrp	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_ShowFileBrowserOrWorks	},
   [ActReqRemFilDocCrs	] = { 479,-1,TabUnk,ActSeeAdmDocCrsGrp	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_AskRemFileFromTree		},
   [ActRemFilDocCrs	] = { 480,-1,TabUnk,ActSeeAdmDocCrsGrp	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_RemFileFromTree		},
   [ActRemFolDocCrs	] = { 497,-1,TabUnk,ActSeeAdmDocCrsGrp	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_RemFolderFromTree		},
   [ActCopDocCrs	] = { 470,-1,TabUnk,ActSeeAdmDocCrsGrp	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_CopyFromFileBrowser	},
   [ActPasDocCrs	] = { 478,-1,TabUnk,ActSeeAdmDocCrsGrp	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_PasteIntoFileBrowser	},
   [ActRemTreDocCrs	] = { 498,-1,TabUnk,ActSeeAdmDocCrsGrp	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_RemSubtreeInFileBrowser	},
   [ActFrmCreDocCrs	] = { 481,-1,TabUnk,ActSeeAdmDocCrsGrp	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_ShowFormFileBrowser	},
   [ActCreFolDocCrs	] = { 491,-1,TabUnk,ActSeeAdmDocCrsGrp	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_RecFolderFileBrowser	},
   [ActCreLnkDocCrs	] = {1225,-1,TabUnk,ActSeeAdmDocCrsGrp	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_RecLinkFileBrowser		},
   [ActRenFolDocCrs	] = { 535,-1,TabUnk,ActSeeAdmDocCrsGrp	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_RenFolderFileBrowser	},
   [ActRcvFilDocCrsDZ	] = {1214,-1,TabUnk,ActSeeAdmDocCrsGrp	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_DATA,Act_UPL_FIL,Brw_RcvFileInFileBrwDZ	,NULL				},
   [ActRcvFilDocCrsCla	] = { 482,-1,TabUnk,ActSeeAdmDocCrsGrp	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_DATA,Act_1ST_TAB,NULL			,Brw_RcvFileInFileBrwClassic	},
   [ActExpAdmDocCrs	] = { 477,-1,TabUnk,ActSeeAdmDocCrsGrp	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_204_NOC,Brw_ExpandFileTree	,NULL				},
   [ActConAdmDocCrs	] = { 494,-1,TabUnk,ActSeeAdmDocCrsGrp	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_204_NOC,Brw_ContractFileTree	,NULL				},
   [ActZIPAdmDocCrs	] = {1126,-1,TabUnk,ActSeeAdmDocCrsGrp	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,ZIP_CompressFileTree		},
   [ActUnhDocCrs	] = { 464,-1,TabUnk,ActSeeAdmDocCrsGrp	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_SetDocumentAsVisible	},
   [ActHidDocCrs	] = { 465,-1,TabUnk,ActSeeAdmDocCrsGrp	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_SetDocumentAsHidden	},
   [ActReqDatAdmDocCrs	] = {1029,-1,TabUnk,ActSeeAdmDocCrsGrp	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_ShowFileMetadata		},
   [ActChgDatAdmDocCrs	] = { 996,-1,TabUnk,ActSeeAdmDocCrsGrp	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_ChgFileMetadata		},
   [ActReqLnkAdmDocCrs	] = {1931,-1,TabUnk,ActSeeAdmDocCrsGrp	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,BrwRsc_GetLinkToDocFil		},
   [ActDowAdmDocCrs	] = {1113,-1,TabUnk,ActSeeAdmDocCrsGrp	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_DWN_FIL,Brw_DownloadFile		,NULL				},
   [ActAdmDocGrp	] = {1201,-1,TabUnk,ActSeeAdmDocCrsGrp	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_ShowFileBrowserOrWorks	},
   [ActReqRemFilDocGrp	] = { 473,-1,TabUnk,ActSeeAdmDocCrsGrp	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_AskRemFileFromTree		},
   [ActRemFilDocGrp	] = { 474,-1,TabUnk,ActSeeAdmDocCrsGrp	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_RemFileFromTree		},
   [ActRemFolDocGrp	] = { 484,-1,TabUnk,ActSeeAdmDocCrsGrp	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_RemFolderFromTree		},
   [ActCopDocGrp	] = { 472,-1,TabUnk,ActSeeAdmDocCrsGrp	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_CopyFromFileBrowser	},
   [ActPasDocGrp	] = { 471,-1,TabUnk,ActSeeAdmDocCrsGrp	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_PasteIntoFileBrowser	},
   [ActRemTreDocGrp	] = { 485,-1,TabUnk,ActSeeAdmDocCrsGrp	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_RemSubtreeInFileBrowser	},
   [ActFrmCreDocGrp	] = { 468,-1,TabUnk,ActSeeAdmDocCrsGrp	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_ShowFormFileBrowser	},
   [ActCreFolDocGrp	] = { 469,-1,TabUnk,ActSeeAdmDocCrsGrp	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_RecFolderFileBrowser	},
   [ActCreLnkDocGrp	] = {1231,-1,TabUnk,ActSeeAdmDocCrsGrp	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_RecLinkFileBrowser		},
   [ActRenFolDocGrp	] = { 490,-1,TabUnk,ActSeeAdmDocCrsGrp	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_RenFolderFileBrowser	},
   [ActRcvFilDocGrpDZ	] = {1215,-1,TabUnk,ActSeeAdmDocCrsGrp	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_DATA,Act_UPL_FIL,Brw_RcvFileInFileBrwDZ	,NULL				},
   [ActRcvFilDocGrpCla	] = { 483,-1,TabUnk,ActSeeAdmDocCrsGrp	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_DATA,Act_1ST_TAB,NULL			,Brw_RcvFileInFileBrwClassic	},
   [ActExpAdmDocGrp	] = { 486,-1,TabUnk,ActSeeAdmDocCrsGrp	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_204_NOC,Brw_ExpandFileTree	,NULL				},
   [ActConAdmDocGrp	] = { 487,-1,TabUnk,ActSeeAdmDocCrsGrp	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_204_NOC,Brw_ContractFileTree	,NULL				},
   [ActZIPAdmDocGrp	] = {1127,-1,TabUnk,ActSeeAdmDocCrsGrp	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,ZIP_CompressFileTree		},
   [ActUnhDocGrp	] = { 493,-1,TabUnk,ActSeeAdmDocCrsGrp	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_SetDocumentAsVisible	},
   [ActHidDocGrp	] = { 492,-1,TabUnk,ActSeeAdmDocCrsGrp	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_SetDocumentAsHidden	},
   [ActReqDatAdmDocGrp	] = {1030,-1,TabUnk,ActSeeAdmDocCrsGrp	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_ShowFileMetadata		},
   [ActChgDatAdmDocGrp	] = { 998,-1,TabUnk,ActSeeAdmDocCrsGrp	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_ChgFileMetadata		},
   [ActDowAdmDocGrp	] = {1114,-1,TabUnk,ActSeeAdmDocCrsGrp	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_DWN_FIL,Brw_DownloadFile		,NULL				},

   // Private documents for course teachers
   [ActAdmTchCrsGrp	] = {1525, 7,TabFil,ActAdmTchCrsGrp	,{0x3C0,0x3F0},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_ShowFileBrowserOrWorks	},
   [ActChgToAdmTch	] = {1526,-1,TabUnk,ActAdmTchCrsGrp	,{0x3C0,0x3F0},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_ShowFileBrowserOrWorks	},
   [ActAdmTchCrs	] = {1527,-1,TabUnk,ActAdmTchCrsGrp	,{0x3C0,0x3F0},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_ShowFileBrowserOrWorks	},
   [ActReqRemFilTchCrs	] = {1528,-1,TabUnk,ActAdmTchCrsGrp	,{0x3C0,0x3F0},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_AskRemFileFromTree		},
   [ActRemFilTchCrs	] = {1529,-1,TabUnk,ActAdmTchCrsGrp	,{0x3C0,0x3F0},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_RemFileFromTree		},
   [ActRemFolTchCrs	] = {1530,-1,TabUnk,ActAdmTchCrsGrp	,{0x3C0,0x3F0},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_RemFolderFromTree		},
   [ActCopTchCrs	] = {1531,-1,TabUnk,ActAdmTchCrsGrp	,{0x3C0,0x3F0},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_CopyFromFileBrowser	},
   [ActPasTchCrs	] = {1532,-1,TabUnk,ActAdmTchCrsGrp	,{0x3C0,0x3F0},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_PasteIntoFileBrowser	},
   [ActRemTreTchCrs	] = {1533,-1,TabUnk,ActAdmTchCrsGrp	,{0x3C0,0x3F0},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_RemSubtreeInFileBrowser	},
   [ActFrmCreTchCrs	] = {1534,-1,TabUnk,ActAdmTchCrsGrp	,{0x3C0,0x3F0},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_ShowFormFileBrowser	},
   [ActCreFolTchCrs	] = {1535,-1,TabUnk,ActAdmTchCrsGrp	,{0x3C0,0x3F0},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_RecFolderFileBrowser	},
   [ActCreLnkTchCrs	] = {1536,-1,TabUnk,ActAdmTchCrsGrp	,{0x3C0,0x3F0},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_RecLinkFileBrowser		},
   [ActRenFolTchCrs	] = {1537,-1,TabUnk,ActAdmTchCrsGrp	,{0x3C0,0x3F0},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_RenFolderFileBrowser	},
   [ActRcvFilTchCrsDZ	] = {1538,-1,TabUnk,ActAdmTchCrsGrp	,{0x3C0,0x3F0},    0,    0,    0,    0,    0,Act_CONT_DATA,Act_UPL_FIL,Brw_RcvFileInFileBrwDZ	,NULL				},
   [ActRcvFilTchCrsCla	] = {1539,-1,TabUnk,ActAdmTchCrsGrp	,{0x3C0,0x3F0},    0,    0,    0,    0,    0,Act_CONT_DATA,Act_1ST_TAB,NULL			,Brw_RcvFileInFileBrwClassic	},
   [ActExpTchCrs	] = {1540,-1,TabUnk,ActAdmTchCrsGrp	,{0x3C0,0x3F0},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_204_NOC,Brw_ExpandFileTree	,NULL				},
   [ActConTchCrs	] = {1541,-1,TabUnk,ActAdmTchCrsGrp	,{0x3C0,0x3F0},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_204_NOC,Brw_ContractFileTree	,NULL				},
   [ActZIPTchCrs	] = {1542,-1,TabUnk,ActAdmTchCrsGrp	,{0x3C0,0x3F0},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,ZIP_CompressFileTree		},
   [ActReqDatTchCrs	] = {1543,-1,TabUnk,ActAdmTchCrsGrp	,{0x3C0,0x3F0},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_ShowFileMetadata		},
   [ActChgDatTchCrs	] = {1544,-1,TabUnk,ActAdmTchCrsGrp	,{0x3C0,0x3F0},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_ChgFileMetadata		},
   [ActDowTchCrs	] = {1545,-1,TabUnk,ActAdmTchCrsGrp	,{0x3C0,0x3F0},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_DWN_FIL,Brw_DownloadFile		,NULL				},
   [ActAdmTchGrp	] = {1546,-1,TabUnk,ActAdmTchCrsGrp	,{0x3C0,0x3F0},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_ShowFileBrowserOrWorks	},
   [ActReqRemFilTchGrp	] = {1547,-1,TabUnk,ActAdmTchCrsGrp	,{0x3C0,0x3F0},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_AskRemFileFromTree		},
   [ActRemFilTchGrp	] = {1548,-1,TabUnk,ActAdmTchCrsGrp	,{0x3C0,0x3F0},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_RemFileFromTree		},
   [ActRemFolTchGrp	] = {1549,-1,TabUnk,ActAdmTchCrsGrp	,{0x3C0,0x3F0},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_RemFolderFromTree		},
   [ActCopTchGrp	] = {1550,-1,TabUnk,ActAdmTchCrsGrp	,{0x3C0,0x3F0},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_CopyFromFileBrowser	},
   [ActPasTchGrp	] = {1551,-1,TabUnk,ActAdmTchCrsGrp	,{0x3C0,0x3F0},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_PasteIntoFileBrowser	},
   [ActRemTreTchGrp	] = {1552,-1,TabUnk,ActAdmTchCrsGrp	,{0x3C0,0x3F0},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_RemSubtreeInFileBrowser	},
   [ActFrmCreTchGrp	] = {1553,-1,TabUnk,ActAdmTchCrsGrp	,{0x3C0,0x3F0},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_ShowFormFileBrowser	},
   [ActCreFolTchGrp	] = {1554,-1,TabUnk,ActAdmTchCrsGrp	,{0x3C0,0x3F0},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_RecFolderFileBrowser	},
   [ActCreLnkTchGrp	] = {1555,-1,TabUnk,ActAdmTchCrsGrp	,{0x3C0,0x3F0},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_RecLinkFileBrowser		},
   [ActRenFolTchGrp	] = {1556,-1,TabUnk,ActAdmTchCrsGrp	,{0x3C0,0x3F0},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_RenFolderFileBrowser	},
   [ActRcvFilTchGrpDZ	] = {1557,-1,TabUnk,ActAdmTchCrsGrp	,{0x3C0,0x3F0},    0,    0,    0,    0,    0,Act_CONT_DATA,Act_UPL_FIL,Brw_RcvFileInFileBrwDZ	,NULL				},
   [ActRcvFilTchGrpCla	] = {1558,-1,TabUnk,ActAdmTchCrsGrp	,{0x3C0,0x3F0},    0,    0,    0,    0,    0,Act_CONT_DATA,Act_1ST_TAB,NULL			,Brw_RcvFileInFileBrwClassic	},
   [ActExpTchGrp	] = {1559,-1,TabUnk,ActAdmTchCrsGrp	,{0x3C0,0x3F0},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_204_NOC,Brw_ExpandFileTree	,NULL				},
   [ActConTchGrp	] = {1560,-1,TabUnk,ActAdmTchCrsGrp	,{0x3C0,0x3F0},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_204_NOC,Brw_ContractFileTree	,NULL				},
   [ActZIPTchGrp	] = {1561,-1,TabUnk,ActAdmTchCrsGrp	,{0x3C0,0x3F0},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,ZIP_CompressFileTree		},
   [ActReqDatTchGrp	] = {1562,-1,TabUnk,ActAdmTchCrsGrp	,{0x3C0,0x3F0},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_ShowFileMetadata		},
   [ActChgDatTchGrp	] = {1563,-1,TabUnk,ActAdmTchCrsGrp	,{0x3C0,0x3F0},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_ChgFileMetadata		},
   [ActDowTchGrp	] = {1564,-1,TabUnk,ActAdmTchCrsGrp	,{0x3C0,0x3F0},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_DWN_FIL,Brw_DownloadFile		,NULL				},

   // Shared files of course
   [ActAdmShaCrsGrp	] = { 461, 8,TabFil,ActAdmShaCrsGrp	,{0x3C7,0x3F8},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_ShowFileBrowserOrWorks	},
   [ActChgToAdmSha	] = {1197,-1,TabUnk,ActAdmShaCrsGrp	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_ShowFileBrowserOrWorks	},
   [ActAdmShaCrs	] = {1202,-1,TabUnk,ActAdmShaCrsGrp	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_ShowFileBrowserOrWorks	},
   [ActReqRemFilShaCrs	] = { 327,-1,TabUnk,ActAdmShaCrsGrp	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_AskRemFileFromTree		},
   [ActRemFilShaCrs	] = { 328,-1,TabUnk,ActAdmShaCrsGrp	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_RemFileFromTree		},
   [ActRemFolShaCrs	] = { 325,-1,TabUnk,ActAdmShaCrsGrp	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_RemFolderFromTree		},
   [ActCopShaCrs	] = { 330,-1,TabUnk,ActAdmShaCrsGrp	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_CopyFromFileBrowser	},
   [ActPasShaCrs	] = { 331,-1,TabUnk,ActAdmShaCrsGrp	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_PasteIntoFileBrowser	},
   [ActRemTreShaCrs	] = { 332,-1,TabUnk,ActAdmShaCrsGrp	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_RemSubtreeInFileBrowser	},
   [ActFrmCreShaCrs	] = { 323,-1,TabUnk,ActAdmShaCrsGrp	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_ShowFormFileBrowser	},
   [ActCreFolShaCrs	] = { 324,-1,TabUnk,ActAdmShaCrsGrp	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_RecFolderFileBrowser	},
   [ActCreLnkShaCrs	] = {1226,-1,TabUnk,ActAdmShaCrsGrp	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_RecLinkFileBrowser		},
   [ActRenFolShaCrs	] = { 329,-1,TabUnk,ActAdmShaCrsGrp	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_RenFolderFileBrowser	},
   [ActRcvFilShaCrsDZ	] = {1216,-1,TabUnk,ActAdmShaCrsGrp	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_DATA,Act_UPL_FIL,Brw_RcvFileInFileBrwDZ	,NULL				},
   [ActRcvFilShaCrsCla	] = { 326,-1,TabUnk,ActAdmShaCrsGrp	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_DATA,Act_1ST_TAB,NULL			,Brw_RcvFileInFileBrwClassic	},
   [ActExpShaCrs	] = { 421,-1,TabUnk,ActAdmShaCrsGrp	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_204_NOC,Brw_ExpandFileTree	,NULL				},
   [ActConShaCrs	] = { 422,-1,TabUnk,ActAdmShaCrsGrp	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_204_NOC,Brw_ContractFileTree	,NULL				},
   [ActZIPShaCrs	] = {1128,-1,TabUnk,ActAdmShaCrsGrp	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,ZIP_CompressFileTree		},
   [ActReqDatShaCrs	] = {1031,-1,TabUnk,ActAdmShaCrsGrp	,{0x3C7,0x3F8},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_ShowFileMetadata		},
   [ActChgDatShaCrs	] = {1000,-1,TabUnk,ActAdmShaCrsGrp	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_ChgFileMetadata		},
   [ActDowShaCrs	] = {1115,-1,TabUnk,ActAdmShaCrsGrp	,{0x3C7,0x3F8},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_DWN_FIL,Brw_DownloadFile		,NULL				},
   [ActAdmShaGrp	] = {1203,-1,TabUnk,ActAdmShaCrsGrp	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_ShowFileBrowserOrWorks	},
   [ActReqRemFilShaGrp	] = { 341,-1,TabUnk,ActAdmShaCrsGrp	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_AskRemFileFromTree		},
   [ActRemFilShaGrp	] = { 342,-1,TabUnk,ActAdmShaCrsGrp	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_RemFileFromTree		},
   [ActRemFolShaGrp	] = { 338,-1,TabUnk,ActAdmShaCrsGrp	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_RemFolderFromTree		},
   [ActCopShaGrp	] = { 336,-1,TabUnk,ActAdmShaCrsGrp	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_CopyFromFileBrowser	},
   [ActPasShaGrp	] = { 337,-1,TabUnk,ActAdmShaCrsGrp	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_PasteIntoFileBrowser	},
   [ActRemTreShaGrp	] = { 339,-1,TabUnk,ActAdmShaCrsGrp	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_RemSubtreeInFileBrowser	},
   [ActFrmCreShaGrp	] = { 333,-1,TabUnk,ActAdmShaCrsGrp	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_ShowFormFileBrowser	},
   [ActCreFolShaGrp	] = { 334,-1,TabUnk,ActAdmShaCrsGrp	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_RecFolderFileBrowser	},
   [ActCreLnkShaGrp	] = {1227,-1,TabUnk,ActAdmShaCrsGrp	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_RecLinkFileBrowser		},
   [ActRenFolShaGrp	] = { 340,-1,TabUnk,ActAdmShaCrsGrp	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_RenFolderFileBrowser	},
   [ActRcvFilShaGrpDZ	] = {1217,-1,TabUnk,ActAdmShaCrsGrp	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_DATA,Act_UPL_FIL,Brw_RcvFileInFileBrwDZ	,NULL				},
   [ActRcvFilShaGrpCla	] = { 335,-1,TabUnk,ActAdmShaCrsGrp	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_DATA,Act_1ST_TAB,NULL			,Brw_RcvFileInFileBrwClassic	},
   [ActExpShaGrp	] = { 427,-1,TabUnk,ActAdmShaCrsGrp	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_204_NOC,Brw_ExpandFileTree	,NULL				},
   [ActConShaGrp	] = { 426,-1,TabUnk,ActAdmShaCrsGrp	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_204_NOC,Brw_ContractFileTree	,NULL				},
   [ActZIPShaGrp	] = {1129,-1,TabUnk,ActAdmShaCrsGrp	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,ZIP_CompressFileTree		},
   [ActReqDatShaGrp	] = {1032,-1,TabUnk,ActAdmShaCrsGrp	,{0x3C7,0x3F8},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_ShowFileMetadata		},
   [ActChgDatShaGrp	] = {1002,-1,TabUnk,ActAdmShaCrsGrp	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_ChgFileMetadata		},
   [ActDowShaGrp	] = {1116,-1,TabUnk,ActAdmShaCrsGrp	,{0x3C7,0x3F8},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_DWN_FIL,Brw_DownloadFile		,NULL				},

   // Assignments and works of user
   [ActAdmAsgWrkUsr	] = { 792, 9,TabFil,ActAdmAsgWrkUsr	,{    0,0x008},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_ShowFileBrowserOrWorks	},
   [ActReqRemFilAsgUsr	] = { 834,-1,TabUnk,ActAdmAsgWrkUsr	,{    0,0x008},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_AskRemFileFromTree		},
   [ActRemFilAsgUsr	] = { 833,-1,TabUnk,ActAdmAsgWrkUsr	,{    0,0x008},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_RemFileFromTree		},
   [ActRemFolAsgUsr	] = { 827,-1,TabUnk,ActAdmAsgWrkUsr	,{    0,0x008},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_RemFolderFromTree		},
   [ActCopAsgUsr	] = { 829,-1,TabUnk,ActAdmAsgWrkUsr	,{    0,0x008},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_CopyFromFileBrowser	},
   [ActPasAsgUsr	] = { 830,-1,TabUnk,ActAdmAsgWrkUsr	,{    0,0x008},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_PasteIntoFileBrowser	},
   [ActRemTreAsgUsr	] = { 828,-1,TabUnk,ActAdmAsgWrkUsr	,{    0,0x008},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_RemSubtreeInFileBrowser	},
   [ActFrmCreAsgUsr	] = { 825,-1,TabUnk,ActAdmAsgWrkUsr	,{    0,0x008},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_ShowFormFileBrowser	},
   [ActCreFolAsgUsr	] = { 826,-1,TabUnk,ActAdmAsgWrkUsr	,{    0,0x008},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_RecFolderFileBrowser	},
   [ActCreLnkAsgUsr	] = {1232,-1,TabUnk,ActAdmAsgWrkUsr	,{    0,0x008},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_RecLinkFileBrowser		},
   [ActRenFolAsgUsr	] = { 839,-1,TabUnk,ActAdmAsgWrkUsr	,{    0,0x008},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_RenFolderFileBrowser	},
   [ActRcvFilAsgUsrDZ	] = {1218,-1,TabUnk,ActAdmAsgWrkUsr	,{    0,0x008},    0,    0,    0,    0,    0,Act_CONT_DATA,Act_UPL_FIL,Brw_RcvFileInFileBrwDZ	,NULL				},
   [ActRcvFilAsgUsrCla	] = { 832,-1,TabUnk,ActAdmAsgWrkUsr	,{    0,0x008},    0,    0,    0,    0,    0,Act_CONT_DATA,Act_1ST_TAB,NULL			,Brw_RcvFileInFileBrwClassic	},
   [ActExpAsgUsr	] = { 824,-1,TabUnk,ActAdmAsgWrkUsr	,{    0,0x008},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_204_NOC,Brw_ExpandFileTree	,NULL				},
   [ActConAsgUsr	] = { 831,-1,TabUnk,ActAdmAsgWrkUsr	,{    0,0x008},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_204_NOC,Brw_ContractFileTree	,NULL				},
   [ActZIPAsgUsr	] = {1130,-1,TabUnk,ActAdmAsgWrkUsr	,{    0,0x008},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,ZIP_CompressFileTree		},
   [ActReqDatAsgUsr	] = {1039,-1,TabUnk,ActAdmAsgWrkUsr	,{    0,0x008},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_ShowFileMetadata		},
   [ActChgDatAsgUsr	] = {1040,-1,TabUnk,ActAdmAsgWrkUsr	,{    0,0x008},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_ChgFileMetadata		},
   [ActDowAsgUsr	] = {1117,-1,TabUnk,ActAdmAsgWrkUsr	,{    0,0x008},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_DWN_FIL,Brw_DownloadFile		,NULL				},
   [ActReqRemFilWrkUsr	] = { 288,-1,TabUnk,ActAdmAsgWrkUsr	,{    0,0x008},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_AskRemFileFromTree		},
   [ActRemFilWrkUsr	] = { 169,-1,TabUnk,ActAdmAsgWrkUsr	,{    0,0x008},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_RemFileFromTree		},
   [ActRemFolWrkUsr	] = { 228,-1,TabUnk,ActAdmAsgWrkUsr	,{    0,0x008},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_RemFolderFromTree		},
   [ActCopWrkUsr	] = { 314,-1,TabUnk,ActAdmAsgWrkUsr	,{    0,0x008},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_CopyFromFileBrowser	},
   [ActPasWrkUsr	] = { 318,-1,TabUnk,ActAdmAsgWrkUsr	,{    0,0x008},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_PasteIntoFileBrowser	},
   [ActRemTreWrkUsr	] = { 278,-1,TabUnk,ActAdmAsgWrkUsr	,{    0,0x008},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_RemSubtreeInFileBrowser	},
   [ActFrmCreWrkUsr	] = { 150,-1,TabUnk,ActAdmAsgWrkUsr	,{    0,0x008},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_ShowFormFileBrowser	},
   [ActCreFolWrkUsr	] = { 172,-1,TabUnk,ActAdmAsgWrkUsr	,{    0,0x008},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_RecFolderFileBrowser	},
   [ActCreLnkWrkUsr	] = {1228,-1,TabUnk,ActAdmAsgWrkUsr	,{    0,0x008},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_RecLinkFileBrowser		},
   [ActRenFolWrkUsr	] = { 204,-1,TabUnk,ActAdmAsgWrkUsr	,{    0,0x008},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_RenFolderFileBrowser	},
   [ActRcvFilWrkUsrDZ	] = {1219,-1,TabUnk,ActAdmAsgWrkUsr	,{    0,0x008},    0,    0,    0,    0,    0,Act_CONT_DATA,Act_UPL_FIL,Brw_RcvFileInFileBrwDZ	,NULL				},
   [ActRcvFilWrkUsrCla	] = { 148,-1,TabUnk,ActAdmAsgWrkUsr	,{    0,0x008},    0,    0,    0,    0,    0,Act_CONT_DATA,Act_1ST_TAB,NULL			,Brw_RcvFileInFileBrwClassic	},
   [ActExpWrkUsr	] = { 423,-1,TabUnk,ActAdmAsgWrkUsr	,{    0,0x008},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_204_NOC,Brw_ExpandFileTree	,NULL				},
   [ActConWrkUsr	] = { 425,-1,TabUnk,ActAdmAsgWrkUsr	,{    0,0x008},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_204_NOC,Brw_ContractFileTree	,NULL				},
   [ActZIPWrkUsr	] = {1131,-1,TabUnk,ActAdmAsgWrkUsr	,{    0,0x008},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,ZIP_CompressFileTree		},
   [ActReqDatWrkUsr	] = {1041,-1,TabUnk,ActAdmAsgWrkUsr	,{    0,0x008},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_ShowFileMetadata		},
   [ActChgDatWrkUsr	] = {1042,-1,TabUnk,ActAdmAsgWrkUsr	,{    0,0x008},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_ChgFileMetadata		},
   [ActDowWrkUsr	] = {1118,-1,TabUnk,ActAdmAsgWrkUsr	,{    0,0x008},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_DWN_FIL,Brw_DownloadFile		,NULL				},

   // Assignments and works of course
   [ActReqAsgWrkCrs	] = { 899,10,TabFil,ActReqAsgWrkCrs	,{0x200,0x230},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_AskEditWorksCrs		},
   [ActAdmAsgWrkCrs	] = { 139,-1,TabUnk,ActReqAsgWrkCrs	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_GetSelectedUsrsAndShowWorks},
   [ActReqRemFilAsgCrs	] = { 837,-1,TabUnk,ActReqAsgWrkCrs	,{0x200,0x230},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_AskRemFileFromTree		},
   [ActRemFilAsgCrs	] = { 838,-1,TabUnk,ActReqAsgWrkCrs	,{0x200,0x230},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_RemFileFromTree		},
   [ActRemFolAsgCrs	] = { 820,-1,TabUnk,ActReqAsgWrkCrs	,{0x200,0x230},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_RemFolderFromTree		},
   [ActCopAsgCrs	] = { 836,-1,TabUnk,ActReqAsgWrkCrs	,{0x200,0x230},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_CopyFromFileBrowser	},
   [ActPasAsgCrs	] = { 821,-1,TabUnk,ActReqAsgWrkCrs	,{0x200,0x230},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_PasteIntoFileBrowser	},
   [ActRemTreAsgCrs	] = { 822,-1,TabUnk,ActReqAsgWrkCrs	,{0x200,0x230},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_RemSubtreeInFileBrowser	},
   [ActFrmCreAsgCrs	] = { 817,-1,TabUnk,ActReqAsgWrkCrs	,{0x200,0x230},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_ShowFormFileBrowser	},
   [ActCreFolAsgCrs	] = { 818,-1,TabUnk,ActReqAsgWrkCrs	,{0x200,0x230},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_RecFolderFileBrowser	},
   [ActCreLnkAsgCrs	] = {1233,-1,TabUnk,ActReqAsgWrkCrs	,{0x200,0x230},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_RecLinkFileBrowser		},
   [ActRenFolAsgCrs	] = { 823,-1,TabUnk,ActReqAsgWrkCrs	,{0x200,0x230},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_RenFolderFileBrowser	},
   [ActRcvFilAsgCrsDZ	] = {1220,-1,TabUnk,ActReqAsgWrkCrs	,{0x200,0x230},    0,    0,    0,    0,    0,Act_CONT_DATA,Act_UPL_FIL,Brw_RcvFileInFileBrwDZ	,NULL				},
   [ActRcvFilAsgCrsCla	] = { 846,-1,TabUnk,ActReqAsgWrkCrs	,{0x200,0x230},    0,    0,    0,    0,    0,Act_CONT_DATA,Act_1ST_TAB,NULL			,Brw_RcvFileInFileBrwClassic	},
   [ActExpAsgCrs	] = { 819,-1,TabUnk,ActReqAsgWrkCrs	,{0x200,0x230},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_204_NOC,Brw_ExpandFileTree	,NULL				},
   [ActConAsgCrs	] = { 835,-1,TabUnk,ActReqAsgWrkCrs	,{0x200,0x230},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_204_NOC,Brw_ContractFileTree	,NULL				},
   [ActZIPAsgCrs	] = {1132,-1,TabUnk,ActReqAsgWrkCrs	,{0x200,0x230},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,ZIP_CompressFileTree		},
   [ActReqDatAsgCrs	] = {1043,-1,TabUnk,ActReqAsgWrkCrs	,{0x200,0x230},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_ShowFileMetadata		},
   [ActChgDatAsgCrs	] = {1044,-1,TabUnk,ActReqAsgWrkCrs	,{0x200,0x230},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_ChgFileMetadata		},
   [ActDowAsgCrs	] = {1119,-1,TabUnk,ActReqAsgWrkCrs	,{0x200,0x230},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_DWN_FIL,Brw_DownloadFile		,NULL				},
   [ActReqRemFilWrkCrs	] = { 289,-1,TabUnk,ActReqAsgWrkCrs	,{0x200,0x230},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_AskRemFileFromTree		},
   [ActRemFilWrkCrs	] = { 209,-1,TabUnk,ActReqAsgWrkCrs	,{0x200,0x230},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_RemFileFromTree		},
   [ActRemFolWrkCrs	] = { 210,-1,TabUnk,ActReqAsgWrkCrs	,{0x200,0x230},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_RemFolderFromTree		},
   [ActCopWrkCrs	] = { 312,-1,TabUnk,ActReqAsgWrkCrs	,{0x200,0x230},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_CopyFromFileBrowser	},
   [ActPasWrkCrs	] = { 319,-1,TabUnk,ActReqAsgWrkCrs	,{0x200,0x230},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_PasteIntoFileBrowser	},
   [ActRemTreWrkCrs	] = { 279,-1,TabUnk,ActReqAsgWrkCrs	,{0x200,0x230},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_RemSubtreeInFileBrowser	},
   [ActFrmCreWrkCrs	] = { 205,-1,TabUnk,ActReqAsgWrkCrs	,{0x200,0x230},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_ShowFormFileBrowser	},
   [ActCreFolWrkCrs	] = { 206,-1,TabUnk,ActReqAsgWrkCrs	,{0x200,0x230},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_RecFolderFileBrowser	},
   [ActCreLnkWrkCrs	] = {1229,-1,TabUnk,ActReqAsgWrkCrs	,{0x200,0x230},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_RecLinkFileBrowser		},
   [ActRenFolWrkCrs	] = { 208,-1,TabUnk,ActReqAsgWrkCrs	,{0x200,0x230},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_RenFolderFileBrowser	},
   [ActRcvFilWrkCrsDZ	] = {1221,-1,TabUnk,ActReqAsgWrkCrs	,{0x200,0x230},    0,    0,    0,    0,    0,Act_CONT_DATA,Act_UPL_FIL,Brw_RcvFileInFileBrwDZ	,NULL				},
   [ActRcvFilWrkCrsCla	] = { 207,-1,TabUnk,ActReqAsgWrkCrs	,{0x200,0x230},    0,    0,    0,    0,    0,Act_CONT_DATA,Act_1ST_TAB,NULL			,Brw_RcvFileInFileBrwClassic	},
   [ActExpWrkCrs	] = { 416,-1,TabUnk,ActReqAsgWrkCrs	,{0x200,0x230},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_204_NOC,Brw_ExpandFileTree	,NULL				},
   [ActConWrkCrs	] = { 424,-1,TabUnk,ActReqAsgWrkCrs	,{0x200,0x230},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_204_NOC,Brw_ContractFileTree	,NULL				},
   [ActZIPWrkCrs	] = {1133,-1,TabUnk,ActReqAsgWrkCrs	,{0x200,0x230},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,ZIP_CompressFileTree		},
   [ActReqDatWrkCrs	] = {1045,-1,TabUnk,ActReqAsgWrkCrs	,{0x200,0x230},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_ShowFileMetadata		},
   [ActChgDatWrkCrs	] = {1046,-1,TabUnk,ActReqAsgWrkCrs	,{0x200,0x230},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_ChgFileMetadata		},
   [ActDowWrkCrs	] = {1120,-1,TabUnk,ActReqAsgWrkCrs	,{0x200,0x230},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_DWN_FIL,Brw_DownloadFile		,NULL				},

   // Marks
   [ActSeeAdmMrk	] = {  17,11,TabFil,ActSeeAdmMrk	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_ShowFileBrowserOrWorks	},
   [ActChgToSeeMrk	] = {1198,-1,TabUnk,ActSeeAdmMrk	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_ShowFileBrowserOrWorks	},
   [ActSeeMrkCrs	] = {1079,-1,TabUnk,ActSeeAdmMrk	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_ShowFileBrowserOrWorks	},
   [ActExpSeeMrkCrs	] = { 528,-1,TabUnk,ActSeeAdmMrk	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_204_NOC,Brw_ExpandFileTree	,NULL				},
   [ActConSeeMrkCrs	] = { 527,-1,TabUnk,ActSeeAdmMrk	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_204_NOC,Brw_ContractFileTree	,NULL				},
   [ActReqDatSeeMrkCrs	] = {1086,-1,TabUnk,ActSeeAdmMrk	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_ShowFileMetadata		},
   [ActReqLnkSeeMrkCrs	] = {1939,-1,TabUnk,ActSeeAdmMrk	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,BrwRsc_GetLinkToMrkFil		},
   [ActSeeMyMrkCrs	] = { 523,-1,TabUnk,ActSeeAdmMrk	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_NEW_TAB,Mrk_ShowMyMarks		,NULL				},
   [ActSeeMrkGrp	] = {1204,-1,TabUnk,ActSeeAdmMrk	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_ShowFileBrowserOrWorks	},
   [ActExpSeeMrkGrp	] = { 605,-1,TabUnk,ActSeeAdmMrk	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_204_NOC,Brw_ExpandFileTree	,NULL				},
   [ActConSeeMrkGrp	] = { 609,-1,TabUnk,ActSeeAdmMrk	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_204_NOC,Brw_ContractFileTree	,NULL				},
   [ActReqDatSeeMrkGrp	] = {1087,-1,TabUnk,ActSeeAdmMrk	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_ShowFileMetadata		},
   [ActSeeMyMrkGrp	] = { 524,-1,TabUnk,ActSeeAdmMrk	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_NEW_TAB,Mrk_ShowMyMarks		,NULL				},
   [ActChgToAdmMrk	] = {1199,-1,TabUnk,ActSeeAdmMrk	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_ShowFileBrowserOrWorks	},
   [ActAdmMrkCrs	] = { 284,-1,TabUnk,ActSeeAdmMrk	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_ShowFileBrowserOrWorks	},
   [ActReqRemFilMrkCrs	] = { 595,-1,TabUnk,ActSeeAdmMrk	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_AskRemFileFromTree		},
   [ActRemFilMrkCrs	] = { 533,-1,TabUnk,ActSeeAdmMrk	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_RemFileFromTree		},
   [ActRemFolMrkCrs	] = { 530,-1,TabUnk,ActSeeAdmMrk	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_RemFolderFromTree		},
   [ActCopMrkCrs	] = { 501,-1,TabUnk,ActSeeAdmMrk	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_CopyFromFileBrowser	},
   [ActPasMrkCrs	] = { 507,-1,TabUnk,ActSeeAdmMrk	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_PasteIntoFileBrowser	},
   [ActRemTreMrkCrs	] = { 534,-1,TabUnk,ActSeeAdmMrk	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_RemSubtreeInFileBrowser	},
   [ActFrmCreMrkCrs	] = { 596,-1,TabUnk,ActSeeAdmMrk	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_ShowFormFileBrowser	},
   [ActCreFolMrkCrs	] = { 506,-1,TabUnk,ActSeeAdmMrk	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_RecFolderFileBrowser	},
   [ActRenFolMrkCrs	] = { 574,-1,TabUnk,ActSeeAdmMrk	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_RenFolderFileBrowser	},
   [ActRcvFilMrkCrsDZ	] = {1222,-1,TabUnk,ActSeeAdmMrk	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_DATA,Act_UPL_FIL,Brw_RcvFileInFileBrwDZ	,NULL				},
   [ActRcvFilMrkCrsCla	] = { 516,-1,TabUnk,ActSeeAdmMrk	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_DATA,Act_1ST_TAB,NULL			,Brw_RcvFileInFileBrwClassic	},
   [ActExpAdmMrkCrs	] = { 607,-1,TabUnk,ActSeeAdmMrk	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_204_NOC,Brw_ExpandFileTree	,NULL				},
   [ActConAdmMrkCrs	] = { 621,-1,TabUnk,ActSeeAdmMrk	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_204_NOC,Brw_ContractFileTree	,NULL				},
   [ActZIPAdmMrkCrs	] = {1134,-1,TabUnk,ActSeeAdmMrk	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,ZIP_CompressFileTree		},
   [ActUnhMrkCrs	] = {1191,-1,TabUnk,ActSeeAdmMrk	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_SetDocumentAsVisible	},
   [ActHidMrkCrs	] = {1192,-1,TabUnk,ActSeeAdmMrk	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_SetDocumentAsHidden	},
   [ActReqDatAdmMrkCrs	] = {1035,-1,TabUnk,ActSeeAdmMrk	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_ShowFileMetadata		},
   [ActChgDatAdmMrkCrs	] = {1036,-1,TabUnk,ActSeeAdmMrk	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_ChgFileMetadata		},
   [ActReqLnkAdmMrkCrs	] = {1940,-1,TabUnk,ActSeeAdmMrk	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,BrwRsc_GetLinkToMrkFil		},
   [ActDowAdmMrkCrs	] = {1121,-1,TabUnk,ActSeeAdmMrk	,{0x3C7,0x3F8},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_DWN_FIL,Brw_DownloadFile		,NULL				},
   [ActChgNumRowHeaCrs	] = { 503,-1,TabUnk,ActSeeAdmMrk	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Mrk_ChangeNumRowsHeader	},
   [ActChgNumRowFooCrs	] = { 504,-1,TabUnk,ActSeeAdmMrk	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Mrk_ChangeNumRowsFooter	},
   [ActAdmMrkGrp	] = {1205,-1,TabUnk,ActSeeAdmMrk	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_ShowFileBrowserOrWorks	},
   [ActReqRemFilMrkGrp	] = { 600,-1,TabUnk,ActSeeAdmMrk	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_AskRemFileFromTree		},
   [ActRemFilMrkGrp	] = { 509,-1,TabUnk,ActSeeAdmMrk	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_RemFileFromTree		},
   [ActRemFolMrkGrp	] = { 520,-1,TabUnk,ActSeeAdmMrk	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_RemFolderFromTree		},
   [ActCopMrkGrp	] = { 519,-1,TabUnk,ActSeeAdmMrk	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_CopyFromFileBrowser	},
   [ActPasMrkGrp	] = { 502,-1,TabUnk,ActSeeAdmMrk	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_PasteIntoFileBrowser	},
   [ActRemTreMrkGrp	] = { 521,-1,TabUnk,ActSeeAdmMrk	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_RemSubtreeInFileBrowser	},
   [ActFrmCreMrkGrp	] = { 601,-1,TabUnk,ActSeeAdmMrk	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_ShowFormFileBrowser	},
   [ActCreFolMrkGrp	] = { 513,-1,TabUnk,ActSeeAdmMrk	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_RecFolderFileBrowser	},
   [ActRenFolMrkGrp	] = { 529,-1,TabUnk,ActSeeAdmMrk	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_RenFolderFileBrowser	},
   [ActRcvFilMrkGrpDZ	] = {1223,-1,TabUnk,ActSeeAdmMrk	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_DATA,Act_UPL_FIL,Brw_RcvFileInFileBrwDZ	,NULL				},
   [ActRcvFilMrkGrpCla	] = { 514,-1,TabUnk,ActSeeAdmMrk	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_DATA,Act_1ST_TAB,NULL			,Brw_RcvFileInFileBrwClassic	},
   [ActExpAdmMrkGrp	] = { 631,-1,TabUnk,ActSeeAdmMrk	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_204_NOC,Brw_ExpandFileTree	,NULL				},
   [ActConAdmMrkGrp	] = { 900,-1,TabUnk,ActSeeAdmMrk	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_204_NOC,Brw_ContractFileTree	,NULL				},
   [ActZIPAdmMrkGrp	] = {1135,-1,TabUnk,ActSeeAdmMrk	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,ZIP_CompressFileTree		},
   [ActUnhMrkGrp	] = {1193,-1,TabUnk,ActSeeAdmMrk	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_SetDocumentAsVisible	},
   [ActHidMrkGrp	] = {1194,-1,TabUnk,ActSeeAdmMrk	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_SetDocumentAsHidden	},
   [ActReqDatAdmMrkGrp	] = {1037,-1,TabUnk,ActSeeAdmMrk	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_ShowFileMetadata		},
   [ActChgDatAdmMrkGrp	] = {1038,-1,TabUnk,ActSeeAdmMrk	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_ChgFileMetadata		},
   [ActDowAdmMrkGrp	] = {1122,-1,TabUnk,ActSeeAdmMrk	,{0x3C7,0x3F8},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_DWN_FIL,Brw_DownloadFile		,NULL				},
   [ActChgNumRowHeaGrp	] = { 510,-1,TabUnk,ActSeeAdmMrk	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Mrk_ChangeNumRowsHeader	},
   [ActChgNumRowFooGrp	] = { 511,-1,TabUnk,ActSeeAdmMrk	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Mrk_ChangeNumRowsFooter	},

   // Briefcase
   [ActAdmBrf		] = {  23,12,TabFil,ActAdmBrf		,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_ShowFileBrowserOrWorks	},
   [ActReqRemFilBrf	] = { 286,-1,TabUnk,ActAdmBrf		,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_AskRemFileFromTree		},
   [ActRemFilBrf	] = { 155,-1,TabUnk,ActAdmBrf		,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_RemFileFromTree		},
   [ActRemFolBrf	] = { 196,-1,TabUnk,ActAdmBrf		,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_RemFolderFromTree		},
   [ActCopBrf		] = { 311,-1,TabUnk,ActAdmBrf		,{0x3C4,0x3F8},0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_CopyFromFileBrowser	},
   [ActPasBrf		] = { 315,-1,TabUnk,ActAdmBrf		,{0x3C4,0x3F8},0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_PasteIntoFileBrowser	},
   [ActRemTreBrf	] = { 276,-1,TabUnk,ActAdmBrf		,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_RemSubtreeInFileBrowser	},
   [ActFrmCreBrf	] = { 597,-1,TabUnk,ActAdmBrf		,{0x3C4,0x3F8},0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_ShowFormFileBrowser	},
   [ActCreFolBrf	] = { 170,-1,TabUnk,ActAdmBrf		,{0x3C4,0x3F8},0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_RecFolderFileBrowser	},
   [ActCreLnkBrf	] = {1230,-1,TabUnk,ActAdmBrf		,{0x3C4,0x3F8},0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_RecLinkFileBrowser		},
   [ActRenFolBrf	] = { 197,-1,TabUnk,ActAdmBrf		,{0x3C4,0x3F8},0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_RenFolderFileBrowser	},
   [ActRcvFilBrfDZ	] = {1224,-1,TabUnk,ActAdmBrf		,{0x3C4,0x3F8},0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_DATA,Act_UPL_FIL,Brw_RcvFileInFileBrwDZ	,NULL				},
   [ActRcvFilBrfCla	] = { 153,-1,TabUnk,ActAdmBrf		,{0x3C4,0x3F8},0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_DATA,Act_1ST_TAB,NULL			,Brw_RcvFileInFileBrwClassic	},
   [ActExpBrf		] = { 410,-1,TabUnk,ActAdmBrf		,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_204_NOC,Brw_ExpandFileTree	,NULL				},
   [ActConBrf		] = { 411,-1,TabUnk,ActAdmBrf		,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_204_NOC,Brw_ContractFileTree	,NULL				},
   [ActZIPBrf		] = {1136,-1,TabUnk,ActAdmBrf		,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_1ST_TAB,NULL			,ZIP_CompressFileTree		},
   [ActReqDatBrf	] = {1047,-1,TabUnk,ActAdmBrf		,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_ShowFileMetadata		},
   [ActChgDatBrf	] = {1048,-1,TabUnk,ActAdmBrf		,{0x3C4,0x3F8},0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_ChgFileMetadata		},
   [ActDowBrf		] = {1123,-1,TabUnk,ActAdmBrf		,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_DWN_FIL,Brw_DownloadFile		,NULL				},
   [ActReqRemOldBrf	] = {1488,-1,TabUnk,ActAdmBrf		,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_AskRemoveOldFilesBriefcase	},
   [ActRemOldBrf	] = {1489,-1,TabUnk,ActAdmBrf		,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_1ST_TAB,NULL			,Brw_RemoveOldFilesBriefcase	},

   // TabUsr ******************************************************************
   // Groups
   [ActReqSelGrp	] = { 116, 0,TabUsr,ActReqSelGrp	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Grp_ReqRegisterInGrps		},
   [ActChgGrp		] = { 118,-1,TabUnk,ActReqSelGrp	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Grp_ChangeMyGrpsAndShowChanges	},
   [ActReqEdiGrp	] = { 108,-1,TabUnk,ActReqSelGrp	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Grp_ReqEditGroups		},
   [ActNewGrpTyp	] = { 174,-1,TabUnk,ActReqSelGrp	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Grp_ReceiveNewGrpTyp		},
   [ActReqRemGrpTyp	] = { 236,-1,TabUnk,ActReqSelGrp	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Grp_ReqRemGroupType		},
   [ActRemGrpTyp	] = { 237,-1,TabUnk,ActReqSelGrp	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Grp_RemoveGroupType		},
   [ActRenGrpTyp	] = { 304,-1,TabUnk,ActReqSelGrp	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Grp_RenameGroupType		},
   [ActChgMdtGrpTyp	] = { 303,-1,TabUnk,ActReqSelGrp	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Grp_ChangeMandatGrpTyp		},
   [ActChgMulGrpTyp	] = { 302,-1,TabUnk,ActReqSelGrp	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Grp_ChangeMultiGrpTyp		},
   [ActChgTimGrpTyp	] = {1061,-1,TabUnk,ActReqSelGrp	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Grp_ChangeOpenTimeGrpTyp	},
   [ActNewGrp		] = { 122,-1,TabUnk,ActReqSelGrp	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Grp_ReceiveNewGrp		},
   [ActReqRemGrp	] = { 107,-1,TabUnk,ActReqSelGrp	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Grp_ReqRemGroup		},
   [ActRemGrp		] = { 175,-1,TabUnk,ActReqSelGrp	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Grp_RemoveGroup		},
   [ActOpeGrp		] = { 322,-1,TabUnk,ActReqSelGrp	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Grp_OpenGroup			},
   [ActCloGrp		] = { 321,-1,TabUnk,ActReqSelGrp	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Grp_CloseGroup			},
   [ActEnaFilZonGrp	] = { 495,-1,TabUnk,ActReqSelGrp	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Grp_EnableFileZonesGrp		},
   [ActDisFilZonGrp	] = { 496,-1,TabUnk,ActReqSelGrp	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Grp_DisableFileZonesGrp	},
   [ActChgGrpTyp	] = { 167,-1,TabUnk,ActReqSelGrp	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Grp_ChangeGroupType		},
   [ActRenGrp		] = { 121,-1,TabUnk,ActReqSelGrp	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Grp_RenameGroup		},
   [ActChgGrpRoo	] = {1752,-1,TabUnk,ActReqSelGrp	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Grp_ChangeGroupRoom		},
   [ActChgMaxStdGrp	] = { 106,-1,TabUnk,ActReqSelGrp	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Grp_ChangeMaxStdsGrp		},

   // Students
   [ActLstStd		] = { 678, 1,TabUsr,ActLstStd		,{0x3C0,0x3F8},0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Usr_SeeStudents		},
   [ActLstStdAll	] = {  42,-1,TabUnk,ActLstStd		,{0x3C0,0x3F8},0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_NEW_TAB,NULL			,Usr_ListAllDataStds		},
   [ActPrnStdPho	] = { 120,-1,TabUnk,ActLstStd		,{0x3C0,0x3F8},0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_NEW_TAB,NULL			,Usr_SeeStdClassPhotoPrn	},
   [Act_DoAct_OnSevStd	] = {1754,-1,TabUnk,ActLstStd		,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,Usr_DoActionOnSeveralUsrs1,Usr_DoActionOnSeveralUsrs2	},
   [ActSeeRecSevStd	] = {  89,-1,TabUnk,ActLstStd		,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Rec_ListRecordsStdsShow	},
   [ActPrnRecSevStd	] = { 111,-1,TabUnk,ActLstStd		,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_NEW_TAB,NULL			,Rec_ListRecordsStdsPrint	},
   [ActReqMdfOneStd	] = {1415,-1,TabUnk,ActLstStd		,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_1ST_TAB,NULL			,Enr_ReqRegRemStd		},
   [ActReqMdfStd	] = {1419,-1,TabUnk,ActLstStd		,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_1ST_TAB,NULL			,Enr_AskIfRegRemAnotherStd	},
   [ActReqStdPho	] = {1433,-1,TabUnk,ActLstStd		,{0x3C0,0x3E0},0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Pho_SendPhotoUsr		},
   [ActDetStdPho	] = {1436,-1,TabUnk,ActLstStd		,{0x3C0,0x3E0},0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_DATA,Act_1ST_TAB,NULL			,Pho_RecOtherUsrPhotoDetFaces	},
   [ActChgStdPho	] = {1439,-1,TabUnk,ActLstStd		,{0x3C0,0x3E0},0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_1ST_TAB,Pho_ChangeUsrPhoto1	,Pho_ChangeUsrPhoto2		},
   [ActReqRemStdPho	] = {1575,-1,TabUnk,ActLstStd		,{0x3C0,0x3E0},0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Pho_ReqRemUsrPhoto		},
   [ActRemStdPho	] = {1442,-1,TabUnk,ActLstStd		,{0x3C0,0x3E0},0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Pho_RemoveUsrPhoto		},
   [ActCreStd		] = {1445,-1,TabUnk,ActLstStd		,{0x3C0,0x3E0},0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_1ST_TAB,Enr_CreateNewUsr1	,Enr_CreateNewUsr2		},
   [ActUpdStd		] = {1423,-1,TabUnk,ActLstStd		,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_1ST_TAB,Enr_ModifyUsr1		,Enr_ModifyUsr2			},
   [ActCnfID_Std	] = {1569,-1,TabUnk,ActLstStd		,{0x3C0,0x3F0},0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_1ST_TAB,NULL			,ID_ConfirmOtherUsrID		},
   [ActFrmAccStd	] = {1736,-1,TabUnk,ActLstStd		,{0x3C0,0x3C0},0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Acc_ShowFormChgOtherUsrAccount	},
   [ActRemNicStd	] = {1739,-1,TabUnk,ActLstStd		,{0x3C0,0x3C0},0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Nck_RemoveOtherUsrNick		},
   [ActChgNicStd	] = {1742,-1,TabUnk,ActLstStd		,{0x3C0,0x3C0},0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Nck_ChangeOtherUsrNick		},
   [ActRemID_Std	] = {1451,-1,TabUnk,ActLstStd		,{0x3C0,0x3C0},0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_1ST_TAB,NULL			,ID_RemoveOtherUsrID		},
   [ActChgID_Std	] = {1454,-1,TabUnk,ActLstStd		,{0x3C0,0x3C0},0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_1ST_TAB,NULL			,ID_ChangeOtherUsrID		},
   [ActChgPwdStd	] = {1468,-1,TabUnk,ActLstStd		,{0x3C0,0x3C0},0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_1ST_TAB,Pwd_UpdateOtherUsrPwd	,Acc_ShowFormChgOtherUsrAccount	},
   [ActRemMaiStd	] = {1479,-1,TabUnk,ActLstStd		,{0x3C0,0x3C0},0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Mai_RemoveOtherUsrEmail	},
   [ActChgMaiStd	] = {1482,-1,TabUnk,ActLstStd		,{0x3C0,0x3C0},0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Mai_ChangeOtherUsrEmail	},
   [ActRemStdCrs	] = {1462,-1,TabUnk,ActLstStd		,{0x3C0,0x3F8},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,Enr_RemUsrFromCrs1	,Enr_RemUsrFromCrs2		},
   [ActReqRemAllStdCrs	] = {  88,-1,TabUnk,ActLstStd		,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Enr_AskRemAllStdsThisCrs	},
   [ActRemAllStdCrs	] = {  87,-1,TabUnk,ActLstStd		,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Enr_RemAllStdsThisCrs		},
   [ActReqAccEnrStd	] = {1456,-1,TabUnk,ActLstStd		,{    0,0x3C8},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Enr_ReqAcceptRegisterInCrs	},
   [ActAccEnrStd	] = {1458,-1,TabUnk,ActLstStd		,{    0,0x3C8},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Enr_AcceptRegisterMeInCrs	},
   [ActRemMe_Std	] = {1460,-1,TabUnk,ActLstStd		,{    0,0x3C8},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Enr_ReqRemMeFromCrs		},
   [ActReqFolSevStd	] = {1756,-1,TabUnk,ActLstStd		,{0x200,0x3F8},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Fol_ReqFollowStds		},
   [ActReqUnfSevStd	] = {1758,-1,TabUnk,ActLstStd		,{0x200,0x3F8},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Fol_ReqUnfollowStds		},
   [ActFolSevStd	] = {1760,-1,TabUnk,ActLstStd		,{0x200,0x3F8},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Fol_FollowUsrs			},
   [ActUnfSevStd	] = {1762,-1,TabUnk,ActLstStd		,{0x200,0x3F8},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Fol_UnfollowUsrs		},
   [ActSeeRecOneStd	] = {1174,-1,TabUnk,ActLstStd		,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Rec_GetUsrAndShowRecOneStdCrs	},
   [ActReqEnrSevStd	] = {1426,-1,TabUnk,ActLstStd		,{0x3C0,0x3E0},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Enr_ReqAdminStds		},
   [ActRcvFrmEnrSevStd	] = {1428,-1,TabUnk,ActLstStd		,{0x3C0,0x3E0},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Enr_ReceiveAdminStds		},
   [ActRcvRecOthUsr	] = { 300,-1,TabUnk,ActLstStd		,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Rec_UpdateAndShowOtherCrsRecord},
   [ActEdiRecFie	] = { 292,-1,TabUnk,ActLstStd		,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Rec_ReqEditRecordFields	},
   [ActNewFie		] = { 293,-1,TabUnk,ActLstStd		,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Rec_ReceiveField		},
   [ActReqRemFie	] = { 294,-1,TabUnk,ActLstStd		,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Rec_ReqRemField		},
   [ActRemFie		] = { 295,-1,TabUnk,ActLstStd		,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Rec_RemoveField		},
   [ActRenFie		] = { 296,-1,TabUnk,ActLstStd		,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Rec_RenameField		},
   [ActChgRowFie	] = { 305,-1,TabUnk,ActLstStd		,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Rec_ChangeLinesField		},
   [ActChgVisFie	] = { 297,-1,TabUnk,ActLstStd		,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Rec_ChangeVisibilityField	},
   [ActRcvRecCrs	] = { 301,-1,TabUnk,ActLstStd		,{0x200,0x208},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Rec_UpdateAndShowMyCrsRecord	},

   // Teachers
   [ActLstTch		] = { 679, 2,TabUsr,ActLstTch		,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_1ST_TAB,NULL			,Usr_SeeTeachers		},
   [ActLstTchAll	] = { 578,-1,TabUnk,ActLstTch		,{0x3C0,0x3F8},0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_NEW_TAB,NULL			,Usr_ListAllDataTchs 		},
   [ActPrnTchPho	] = { 443,-1,TabUnk,ActLstTch		,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_NEW_TAB,NULL			,Usr_SeeTchClassPhotoPrn	},
   [Act_DoAct_OnSevTch	] = {1755,-1,TabUnk,ActLstTch		,{0x3C0,0x3F8},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,Usr_DoActionOnSeveralUsrs1,Usr_DoActionOnSeveralUsrs2	},
   [ActSeeRecSevTch	] = {  22,-1,TabUnk,ActLstTch		,{0x3C0,0x3F8},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Rec_ListRecordsTchsShow	},
   [ActPrnRecSevTch	] = { 127,-1,TabUnk,ActLstTch		,{0x3C0,0x3F8},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_NEW_TAB,NULL			,Rec_ListRecordsTchsPrint	},
   [ActReqMdfOneTch	] = {1416,-1,TabUnk,ActLstTch		,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_1ST_TAB,NULL			,Enr_ReqRegRemTch		},
   [ActReqMdfNET	] = {1644,-1,TabUnk,ActLstTch		,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_1ST_TAB,NULL			,Enr_AskIfRegRemAnotherTch	},
   [ActReqMdfTch	] = {1420,-1,TabUnk,ActLstTch		,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_1ST_TAB,NULL			,Enr_AskIfRegRemAnotherTch	},
   [ActReqTchPho	] = {1434,-1,TabUnk,ActLstTch		,{0x3C0,0x3E0},0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Pho_SendPhotoUsr		},
   [ActDetTchPho	] = {1437,-1,TabUnk,ActLstTch		,{0x3C0,0x3E0},0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_DATA,Act_1ST_TAB,NULL			,Pho_RecOtherUsrPhotoDetFaces	},
   [ActChgTchPho	] = {1440,-1,TabUnk,ActLstTch		,{0x3C0,0x3E0},0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_1ST_TAB,Pho_ChangeUsrPhoto1	,Pho_ChangeUsrPhoto2		},
   [ActReqRemTchPho	] = {1576,-1,TabUnk,ActLstTch		,{0x3C0,0x3E0},0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Pho_ReqRemUsrPhoto		},
   [ActRemTchPho	] = {1443,-1,TabUnk,ActLstTch		,{0x3C0,0x3E0},0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Pho_RemoveUsrPhoto		},
   [ActCreNET		] = {1645,-1,TabUnk,ActLstTch		,{0x3C0,0x3E0},0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_1ST_TAB,Enr_CreateNewUsr1	,Enr_CreateNewUsr2		},
   [ActCreTch		] = {1446,-1,TabUnk,ActLstTch		,{0x3C0,0x3E0},0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_1ST_TAB,Enr_CreateNewUsr1	,Enr_CreateNewUsr2		},
   [ActUpdNET		] = {1646,-1,TabUnk,ActLstTch		,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_1ST_TAB,Enr_ModifyUsr1		,Enr_ModifyUsr2			},
   [ActUpdTch		] = {1424,-1,TabUnk,ActLstTch		,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_1ST_TAB,Enr_ModifyUsr1		,Enr_ModifyUsr2			},
   [ActCnfID_Tch	] = {1570,-1,TabUnk,ActLstTch		,{0x3C0,0x3C0},0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_1ST_TAB,NULL			,ID_ConfirmOtherUsrID		},
   [ActFrmAccTch	] = {1737,-1,TabUnk,ActLstTch		,{0x3C0,0x3C0},0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Acc_ShowFormChgOtherUsrAccount	},
   [ActRemNicTch	] = {1740,-1,TabUnk,ActLstTch		,{0x3C0,0x3C0},0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Nck_RemoveOtherUsrNick		},
   [ActChgNicTch	] = {1743,-1,TabUnk,ActLstTch		,{0x3C0,0x3C0},0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Nck_ChangeOtherUsrNick		},
   [ActRemID_Tch	] = {1452,-1,TabUnk,ActLstTch		,{0x3C0,0x3C0},0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_1ST_TAB,NULL			,ID_RemoveOtherUsrID		},
   [ActChgID_Tch	] = {1455,-1,TabUnk,ActLstTch		,{0x3C0,0x3C0},0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_1ST_TAB,NULL			,ID_ChangeOtherUsrID		},
   [ActChgPwdTch	] = {1469,-1,TabUnk,ActLstTch		,{0x3C0,0x3C0},0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_1ST_TAB,Pwd_UpdateOtherUsrPwd	,Acc_ShowFormChgOtherUsrAccount	},
   [ActRemMaiTch	] = {1480,-1,TabUnk,ActLstTch		,{0x3C0,0x3C0},0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Mai_RemoveOtherUsrEmail	},
   [ActChgMaiTch	] = {1483,-1,TabUnk,ActLstTch		,{0x3C0,0x3C0},0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Mai_ChangeOtherUsrEmail	},
   [ActRemNETCrs	] = {1647,-1,TabUnk,ActLstTch		,{0x3C0,0x3F0},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,Enr_RemUsrFromCrs1	,Enr_RemUsrFromCrs2		},
   [ActRemTchCrs	] = {1463,-1,TabUnk,ActLstTch		,{0x3C0,0x3E0},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,Enr_RemUsrFromCrs1	,Enr_RemUsrFromCrs2		},
   [ActReqAccEnrNET	] = {1639,-1,TabUnk,ActLstTch		,{    0,0x3D0},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Enr_ReqAcceptRegisterInCrs	},
   [ActReqAccEnrTch	] = {1457,-1,TabUnk,ActLstTch		,{    0,0x3E0},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Enr_ReqAcceptRegisterInCrs	},
   [ActAccEnrNET	] = {1640,-1,TabUnk,ActLstTch		,{    0,0x3D0},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Enr_AcceptRegisterMeInCrs	},
   [ActAccEnrTch	] = {1459,-1,TabUnk,ActLstTch		,{    0,0x3E0},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Enr_AcceptRegisterMeInCrs	},
   [ActRemMe_NET	] = {1641,-1,TabUnk,ActLstTch		,{    0,0x3D0},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Enr_ReqRemMeFromCrs		},
   [ActRemMe_Tch	] = {1461,-1,TabUnk,ActLstTch		,{    0,0x3E0},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Enr_ReqRemMeFromCrs		},
   [ActReqFolSevTch	] = {1757,-1,TabUnk,ActLstTch		,{0x200,0x3F8},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Fol_ReqFollowTchs		},
   [ActReqUnfSevTch	] = {1759,-1,TabUnk,ActLstTch		,{0x200,0x3F8},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Fol_ReqUnfollowTchs		},
   [ActFolSevTch	] = {1761,-1,TabUnk,ActLstTch		,{0x200,0x3F8},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Fol_FollowUsrs			},
   [ActUnfSevTch	] = {1763,-1,TabUnk,ActLstTch		,{0x200,0x3F8},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Fol_UnfollowUsrs		},
   [ActSeeRecOneTch	] = {1175,-1,TabUnk,ActLstTch		,{0x3C0,0x3F8},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Rec_GetUsrAndShowRecOneTchCrs	},
   [ActReqEnrSevNET	] = {1642,-1,TabUnk,ActLstTch		,{0x3C0,0x3C0},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Enr_ReqAdminNonEditingTchs	},
   [ActRcvFrmEnrSevNET	] = {1643,-1,TabUnk,ActLstTch		,{0x3C0,0x3C0},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Enr_ReceiveAdminNonEditTchs	},
   [ActReqEnrSevTch	] = {1427,-1,TabUnk,ActLstTch		,{0x3C0,0x3C0},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Enr_ReqAdminTchs		},
   [ActRcvFrmEnrSevTch	] = {1429,-1,TabUnk,ActLstTch		,{0x3C0,0x3C0},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Enr_ReceiveAdminTchs		},
   [ActFrmLogInUsrAgd	] = {1614,-1,TabUnk,ActLstTch		,{0x001,    0},0x001,0x001,0x001,0x001,0x001,Act_CONT_NORM,Act_1ST_TAB,NULL			,Agd_PutFormLogInToShowUsrAgenda},
   [ActLogInUsrAgd	] = {1615,-1,TabUnk,ActLstTch		,{0x3C4,0x3F8},0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_1ST_TAB,NULL			,Agd_ShowOtherAgendaAfterLogIn	},
   [ActLogInUsrAgdLan	] = {1616,-1,TabUnk,ActLstTch		,{0x3C4,0x3F8},0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_1ST_TAB,NULL			,Agd_ShowOtherAgendaAfterLogIn	},

   // Others
   [ActLstOth		] = {1186, 3,TabUsr,ActLstOth		,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_1ST_TAB,NULL			,Usr_ListDataAdms		},
   [ActNewAdmIns	] = {1337,-1,TabUnk,ActLstOth		,{0x200,0x200},0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_1ST_TAB,NULL			,Adm_AddAdmToIns		},
   [ActRemAdmIns	] = {1338,-1,TabUnk,ActLstOth		,{0x300,0x300},0x300,0x300,0x300,0x300,0x300,Act_CONT_NORM,Act_1ST_TAB,NULL			,Adm_RemAdmIns			},
   [ActNewAdmCtr	] = {1339,-1,TabUnk,ActLstOth		,{0x300,0x300},0x300,0x300,0x300,0x300,0x300,Act_CONT_NORM,Act_1ST_TAB,NULL			,Adm_AddAdmToCtr		},
   [ActRemAdmCtr	] = {1340,-1,TabUnk,ActLstOth		,{0x380,0x380},0x380,0x380,0x380,0x380,0x380,Act_CONT_NORM,Act_1ST_TAB,NULL			,Adm_RemAdmCtr			},
   [ActNewAdmDeg	] = { 586,-1,TabUnk,ActLstOth		,{0x380,0x380},0x380,0x380,0x380,0x380,0x380,Act_CONT_NORM,Act_1ST_TAB,NULL			,Adm_AddAdmToDeg		},
   [ActRemAdmDeg	] = { 584,-1,TabUnk,ActLstOth		,{0x3C0,0x3C0},0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Adm_RemAdmDeg			},
   [ActLstGst		] = { 587,-1,TabUnk,ActLstOth		,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_1ST_TAB,NULL			,Usr_SeeGuests			},
   [ActLstGstAll	] = {1189,-1,TabUnk,ActLstOth		,{0x200,0x200},0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_NEW_TAB,NULL			,Usr_ListAllDataGsts		},
   [ActPrnGstPho	] = {1190,-1,TabUnk,ActLstOth		,{0x200,0x200},0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_NEW_TAB,NULL			,Usr_SeeGstClassPhotoPrn	},
   [Act_DoAct_OnSevGst	] = {1753,-1,TabUnk,ActLstOth		,{0x200,0x200},0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_1ST_TAB,Usr_DoActionOnSeveralUsrs1,Usr_DoActionOnSeveralUsrs2	},
   [ActSeeRecSevGst	] = {1187,-1,TabUnk,ActLstOth		,{0x200,0x200},0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_1ST_TAB,NULL			,Rec_ListRecordsGstsShow	},
   [ActPrnRecSevGst	] = {1188,-1,TabUnk,ActLstOth		,{0x200,0x200},0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_NEW_TAB,NULL			,Rec_ListRecordsGstsPrint	},
   [ActReqMdfOneOth	] = {1414,-1,TabUnk,ActLstOth		,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_1ST_TAB,NULL			,Enr_ReqRegRemOth		},
   [ActReqMdfOth	] = {1418,-1,TabUnk,ActLstOth		,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_1ST_TAB,NULL			,Enr_AskIfRegRemAnotherOth	},
   [ActReqOthPho	] = {1432,-1,TabUnk,ActLstOth		,{0x3C0,0x3E0},0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Pho_SendPhotoUsr		},
   [ActDetOthPho	] = {1435,-1,TabUnk,ActLstOth		,{0x3C0,0x3E0},0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_DATA,Act_1ST_TAB,NULL			,Pho_RecOtherUsrPhotoDetFaces	},
   [ActChgOthPho	] = {1438,-1,TabUnk,ActLstOth		,{0x3C0,0x3E0},0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_1ST_TAB,Pho_ChangeUsrPhoto1	,Pho_ChangeUsrPhoto2		},
   [ActReqRemOthPho	] = {1574,-1,TabUnk,ActLstOth		,{0x3C0,0x3E0},0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Pho_ReqRemUsrPhoto		},
   [ActRemOthPho	] = {1441,-1,TabUnk,ActLstOth		,{0x3C0,0x3E0},0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Pho_RemoveUsrPhoto		},
   [ActCreOth		] = {1444,-1,TabUnk,ActLstOth		,{0x3C0,0x3E0},0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_1ST_TAB,Enr_CreateNewUsr1	,Enr_CreateNewUsr2		},
   [ActUpdOth		] = {1422,-1,TabUnk,ActLstOth		,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_1ST_TAB,Enr_ModifyUsr1		,Enr_ModifyUsr2			},
   [ActCnfID_Oth	] = {1568,-1,TabUnk,ActLstOth		,{0x3C0,0x3C0},0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_1ST_TAB,NULL			,ID_ConfirmOtherUsrID		},
   [ActFrmAccOth	] = {1735,-1,TabUnk,ActLstOth		,{0x3C0,0x3C0},0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Acc_ShowFormChgOtherUsrAccount	},
   [ActRemNicOth	] = {1738,-1,TabUnk,ActLstOth		,{0x3C0,0x3C0},0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Nck_RemoveOtherUsrNick		},
   [ActChgNicOth	] = {1741,-1,TabUnk,ActLstOth		,{0x3C0,0x3C0},0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Nck_ChangeOtherUsrNick		},
   [ActRemID_Oth	] = {1450,-1,TabUnk,ActLstOth		,{0x3C0,0x3C0},0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_1ST_TAB,NULL			,ID_RemoveOtherUsrID		},
   [ActChgID_Oth	] = {1453,-1,TabUnk,ActLstOth		,{0x3C0,0x3C0},0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_1ST_TAB,NULL			,ID_ChangeOtherUsrID		},
   [ActChgPwdOth	] = {1467,-1,TabUnk,ActLstOth		,{0x3C0,0x3C0},0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_1ST_TAB,Pwd_UpdateOtherUsrPwd	,Acc_ShowFormChgOtherUsrAccount	},
   [ActRemMaiOth	] = {1478,-1,TabUnk,ActLstOth		,{0x3C0,0x3C0},0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Mai_RemoveOtherUsrEmail	},
   [ActChgMaiOth	] = {1481,-1,TabUnk,ActLstOth		,{0x3C0,0x3C0},0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Mai_ChangeOtherUsrEmail	},
   [ActRemUsrGbl	] = {  62,-1,TabUnk,ActLstOth		,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_1ST_TAB,NULL			,Acc_GetUsrCodAndRemUsrGbl	},
   [ActReqRemOldUsr	] = { 590,-1,TabUnk,ActLstOth		,{0x200,0x200},0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_1ST_TAB,NULL			,Enr_AskRemoveOldUsrs		},
   [ActRemOldUsr	] = { 773,-1,TabUnk,ActLstOth		,{0x200,0x200},0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_1ST_TAB,NULL			,Enr_RemoveOldUsrs		},
   [ActLstDupUsr	] = {1578,-1,TabUnk,ActLstOth		,{0x200,0x200},0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_1ST_TAB,NULL			,Dup_ListDuplicateUsrs		},
   [ActLstSimUsr	] = {1579,-1,TabUnk,ActLstOth		,{0x200,0x200},0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_1ST_TAB,NULL			,Dup_GetUsrCodAndListSimilarUsrs},
   [ActRemDupUsr	] = {1580,-1,TabUnk,ActLstOth		,{0x200,0x200},0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_1ST_TAB,NULL			,Dup_RemoveUsrFromListDupUsrs	},

   // Attendance
   [ActSeeAllAtt	] = { 861, 4,TabUsr,ActSeeAllAtt	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Att_SeeEvents			},
   [ActReqLstUsrAtt	] = {1073,-1,TabUnk,ActSeeAllAtt	,{0x200,0x230},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Att_ReqListUsrsAttendanceCrs	},
   [ActSeeLstMyAtt	] = {1473,-1,TabUnk,ActSeeAllAtt	,{    0,0x008},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Att_ListMyAttendanceCrs	},
   [ActPrnLstMyAtt	] = {1474,-1,TabUnk,ActSeeAllAtt	,{    0,0x008},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_NEW_TAB,NULL			,Att_PrintMyAttendanceCrs	},
   [ActSeeLstUsrAtt	] = {1074,-1,TabUnk,ActSeeAllAtt	,{0x200,0x230},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Att_ListUsrsAttendanceCrs	},
   [ActPrnLstUsrAtt	] = {1075,-1,TabUnk,ActSeeAllAtt	,{0x200,0x230},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_NEW_TAB,NULL			,Att_PrintUsrsAttendanceCrs	},
   [ActFrmNewAtt	] = {1063,-1,TabUnk,ActSeeAllAtt	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Att_ReqCreatOrEditEvent	},
   [ActEdiOneAtt	] = {1064,-1,TabUnk,ActSeeAllAtt	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Att_ReqCreatOrEditEvent	},
   [ActNewAtt		] = {1065,-1,TabUnk,ActSeeAllAtt	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Att_ReceiveEvent		},
   [ActChgAtt		] = {1066,-1,TabUnk,ActSeeAllAtt	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Att_ReceiveEvent		},
   [ActReqRemAtt	] = {1067,-1,TabUnk,ActSeeAllAtt	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Att_AskRemEvent		},
   [ActRemAtt		] = {1068,-1,TabUnk,ActSeeAllAtt	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Att_GetAndRemEvent		},
   [ActHidAtt		] = {1069,-1,TabUnk,ActSeeAllAtt	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Att_HideEvent			},
   [ActUnhAtt		] = {1070,-1,TabUnk,ActSeeAllAtt	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Att_UnhideEvent		},
   [ActSeeOneAtt	] = {1071,-1,TabUnk,ActSeeAllAtt	,{0x200,0x238},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Att_SeeOneEvent		},
   [ActReqLnkAtt	] = {1938,-1,TabUnk,ActSeeAllAtt	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,AttRsc_GetLinkToEvent		},
   [ActRecAttStd	] = {1072,-1,TabUnk,ActSeeAllAtt	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Att_RegisterStudentsInEvent	},
   [ActRecAttMe		] = {1076,-1,TabUnk,ActSeeAllAtt	,{    0,0x008},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Att_RegisterMeAsStdInEvent	},

   // Sign up
   [ActReqSignUp	] = {1054, 5,TabUsr,ActReqSignUp	,{0x006,    0},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Enr_ReqSignUpInCrs		},
   [ActSignUp		] = {1056,-1,TabUnk,ActReqSignUp	,{0x006,    0},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Enr_SignUpInCrs		},

   // Requests
   [ActSeeSignUpReq	] = {1057, 6,TabUsr,ActSeeSignUpReq	,{0x3C0,0x3E0},0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Enr_ShowEnrolmentRequests	},
   [ActUpdSignUpReq	] = {1522,-1,TabUnk,ActSeeSignUpReq	,{0x3C0,0x3E0},0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Enr_UpdateEnrolmentRequests	},
   [ActReqRejSignUp	] = {1058,-1,TabUnk,ActSeeSignUpReq	,{0x3C0,0x3E0},0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Enr_AskIfRejectSignUp		},
   [ActRejSignUp	] = {1059,-1,TabUnk,ActSeeSignUpReq	,{0x3C0,0x3E0},0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Enr_RejectSignUp		},

   // Connected
   [ActLstCon		] = { 995, 7,TabUsr,ActLstCon		,{0x3C7,0x3F8},0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_1ST_TAB,NULL			,Con_ShowConnectedUsrs		},

   // TabMsg ******************************************************************
   // Announcements
   [ActSeeAnn		] = {1235, 0,TabMsg,ActSeeAnn		,{0x3C7,0x3F8},0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_1ST_TAB,NULL			,Ann_ShowAllAnnouncements	},
   [ActWriAnn		] = {1237,-1,TabUnk,ActSeeAnn		,{0x200,0x200},0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_1ST_TAB,NULL			,Ann_ShowFormAnnouncement	},
   [ActNewAnn		] = {1238,-1,TabUnk,ActSeeAnn		,{0x200,0x200},0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_1ST_TAB,NULL			,Ann_ReceiveAnnouncement	},
   [ActHidAnn		] = {1470,-1,TabUnk,ActSeeAnn		,{0x200,0x200},0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_1ST_TAB,Ann_HideAnnouncement	,Ann_ShowAllAnnouncements	},
   [ActUnhAnn		] = {1471,-1,TabUnk,ActSeeAnn		,{0x200,0x200},0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_1ST_TAB,Ann_UnhideAnnouncement	,Ann_ShowAllAnnouncements	},
   [ActRemAnn		] = {1236,-1,TabUnk,ActSeeAnn		,{0x200,0x200},0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_1ST_TAB,NULL			,Ann_RemoveAnnouncement		},

   // Notices
   [ActSeeAllNot	] = { 762, 1,TabMsg,ActSeeAllNot	,{0x3C7,0x3F8},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Not_ListFullNotices		},
   [ActSeeOneNot	] = {1164,-1,TabUnk,ActSeeAllNot	,{0x3C7,0x3F8},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,Not_GetHighLightedNotCod	,Not_ListFullNotices		},
   [ActWriNot		] = {  59,-1,TabUnk,ActSeeAllNot	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Not_ShowFormNotice		},
   [ActNewNot		] = {  60,-1,TabUnk,ActSeeAllNot	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,Not_ReceiveNotice	,Not_ListFullNotices		},
   [ActHidNot		] = { 763,-1,TabUnk,ActSeeAllNot	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,Not_HideNotice		,Not_ListFullNotices		},
   [ActUnhNot		] = { 764,-1,TabUnk,ActSeeAllNot	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,Not_UnhideNotice		,Not_ListFullNotices		},
   [ActReqRemNot	] = {1472,-1,TabUnk,ActSeeAllNot	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Not_ReqRemNotice		},
   [ActRemNot		] = {  73,-1,TabUnk,ActSeeAllNot	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,Not_RemoveNotice		,Not_ListNoticesAfterRemoval	},

   // Forums
   [ActSeeFor		] = {  95, 2,TabMsg,ActSeeFor		,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_1ST_TAB,NULL			,For_ShowForumTheads		},
   [ActSeeForCrsUsr	] = { 345,-1,TabUnk,ActSeeFor		,{0x3C4,0x3F8},0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_1ST_TAB,NULL			,For_ShowForumTheads		},
   [ActSeeForCrsTch	] = { 431,-1,TabUnk,ActSeeFor		,{0x204,0x230},0x204,0x204,0x204,0x204,0x204,Act_CONT_NORM,Act_1ST_TAB,NULL			,For_ShowForumTheads		},
   [ActSeeForDegUsr	] = { 241,-1,TabUnk,ActSeeFor		,{0x3C4,0x3F8},0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_1ST_TAB,NULL			,For_ShowForumTheads		},
   [ActSeeForDegTch	] = { 243,-1,TabUnk,ActSeeFor		,{0x3C4,0x3F0},0x204,0x204,0x204,0x204,0x204,Act_CONT_NORM,Act_1ST_TAB,NULL			,For_ShowForumTheads		},
   [ActSeeForCtrUsr	] = { 901,-1,TabUnk,ActSeeFor		,{0x3C4,0x3F8},0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_1ST_TAB,NULL			,For_ShowForumTheads		},
   [ActSeeForCtrTch	] = { 430,-1,TabUnk,ActSeeFor		,{0x3C4,0x3F0},0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_1ST_TAB,NULL			,For_ShowForumTheads		},
   [ActSeeForInsUsr	] = { 725,-1,TabUnk,ActSeeFor		,{0x3C4,0x3F8},0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_1ST_TAB,NULL			,For_ShowForumTheads		},
   [ActSeeForInsTch	] = { 724,-1,TabUnk,ActSeeFor		,{0x3C4,0x3F0},0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_1ST_TAB,NULL			,For_ShowForumTheads		},
   [ActSeeForGenUsr	] = { 726,-1,TabUnk,ActSeeFor		,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_1ST_TAB,NULL			,For_ShowForumTheads		},
   [ActSeeForGenTch	] = { 723,-1,TabUnk,ActSeeFor		,{0x3C4,0x3F0},0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_1ST_TAB,NULL			,For_ShowForumTheads		},
   [ActSeeForSWAUsr	] = { 242,-1,TabUnk,ActSeeFor		,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_1ST_TAB,NULL			,For_ShowForumTheads		},
   [ActSeeForSWATch	] = { 245,-1,TabUnk,ActSeeFor		,{0x3C4,0x3F0},0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_1ST_TAB,NULL			,For_ShowForumTheads		},
   [ActSeePstForCrsUsr	] = { 346,-1,TabUnk,ActSeeFor		,{0x3C4,0x3F8},0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_1ST_TAB,NULL			,For_ShowThreadPosts		},
   [ActSeePstForCrsTch	] = { 347,-1,TabUnk,ActSeeFor		,{0x204,0x230},0x204,0x204,0x204,0x204,0x204,Act_CONT_NORM,Act_1ST_TAB,NULL			,For_ShowThreadPosts		},
   [ActSeePstForDegUsr	] = { 255,-1,TabUnk,ActSeeFor		,{0x3C4,0x3F8},0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_1ST_TAB,NULL			,For_ShowThreadPosts		},
   [ActSeePstForDegTch	] = { 291,-1,TabUnk,ActSeeFor		,{0x3C4,0x3F0},0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_1ST_TAB,NULL			,For_ShowThreadPosts		},
   [ActSeePstForCtrUsr	] = { 348,-1,TabUnk,ActSeeFor		,{0x3C4,0x3F8},0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_1ST_TAB,NULL			,For_ShowThreadPosts		},
   [ActSeePstForCtrTch	] = { 902,-1,TabUnk,ActSeeFor		,{0x3C4,0x3F0},0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_1ST_TAB,NULL			,For_ShowThreadPosts		},
   [ActSeePstForInsUsr	] = { 730,-1,TabUnk,ActSeeFor		,{0x3C4,0x3F8},0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_1ST_TAB,NULL			,For_ShowThreadPosts		},
   [ActSeePstForInsTch	] = { 746,-1,TabUnk,ActSeeFor		,{0x3C4,0x3F0},0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_1ST_TAB,NULL			,For_ShowThreadPosts		},
   [ActSeePstForGenUsr	] = { 727,-1,TabUnk,ActSeeFor		,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_1ST_TAB,NULL			,For_ShowThreadPosts		},
   [ActSeePstForGenTch	] = { 731,-1,TabUnk,ActSeeFor		,{0x3C4,0x3F0},0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_1ST_TAB,NULL			,For_ShowThreadPosts		},
   [ActSeePstForSWAUsr	] = { 244,-1,TabUnk,ActSeeFor		,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_1ST_TAB,NULL			,For_ShowThreadPosts		},
   [ActSeePstForSWATch	] = { 246,-1,TabUnk,ActSeeFor		,{0x3C4,0x3F0},0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_1ST_TAB,NULL			,For_ShowThreadPosts		},
   [ActRcvThrForCrsUsr	] = { 350,-1,TabUnk,ActSeeFor		,{0x3C4,0x3F8},0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_DATA,Act_1ST_TAB,NULL			,For_ReceiveForumPost		},
   [ActRcvThrForCrsTch	] = { 754,-1,TabUnk,ActSeeFor		,{0x204,0x230},0x204,0x204,0x204,0x204,0x204,Act_CONT_DATA,Act_1ST_TAB,NULL			,For_ReceiveForumPost		},
   [ActRcvThrForDegUsr	] = { 252,-1,TabUnk,ActSeeFor		,{0x3C4,0x3F8},0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_DATA,Act_1ST_TAB,NULL			,For_ReceiveForumPost		},
   [ActRcvThrForDegTch	] = { 247,-1,TabUnk,ActSeeFor		,{0x3C4,0x3F0},0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_DATA,Act_1ST_TAB,NULL			,For_ReceiveForumPost		},
   [ActRcvThrForCtrUsr	] = { 903,-1,TabUnk,ActSeeFor		,{0x3C4,0x3F8},0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_DATA,Act_1ST_TAB,NULL			,For_ReceiveForumPost		},
   [ActRcvThrForCtrTch	] = { 904,-1,TabUnk,ActSeeFor		,{0x3C4,0x3F0},0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_DATA,Act_1ST_TAB,NULL			,For_ReceiveForumPost		},
   [ActRcvThrForInsUsr	] = { 737,-1,TabUnk,ActSeeFor		,{0x3C4,0x3F8},0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_DATA,Act_1ST_TAB,NULL			,For_ReceiveForumPost		},
   [ActRcvThrForInsTch	] = { 769,-1,TabUnk,ActSeeFor		,{0x3C4,0x3F0},0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_DATA,Act_1ST_TAB,NULL			,For_ReceiveForumPost		},
   [ActRcvThrForGenUsr	] = { 736,-1,TabUnk,ActSeeFor		,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_DATA,Act_1ST_TAB,NULL			,For_ReceiveForumPost		},
   [ActRcvThrForGenTch	] = { 765,-1,TabUnk,ActSeeFor		,{0x3C4,0x3F0},0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_DATA,Act_1ST_TAB,NULL			,For_ReceiveForumPost		},
   [ActRcvThrForSWAUsr	] = { 258,-1,TabUnk,ActSeeFor		,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_DATA,Act_1ST_TAB,NULL			,For_ReceiveForumPost		},
   [ActRcvThrForSWATch	] = { 259,-1,TabUnk,ActSeeFor		,{0x3C4,0x3F0},0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_DATA,Act_1ST_TAB,NULL			,For_ReceiveForumPost		},
   [ActRcvRepForCrsUsr	] = { 599,-1,TabUnk,ActSeeFor		,{0x3C4,0x3F8},0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_DATA,Act_1ST_TAB,NULL			,For_ReceiveForumPost		},
   [ActRcvRepForCrsTch	] = { 755,-1,TabUnk,ActSeeFor		,{0x204,0x230},0x204,0x204,0x204,0x204,0x204,Act_CONT_DATA,Act_1ST_TAB,NULL			,For_ReceiveForumPost		},
   [ActRcvRepForDegUsr	] = { 606,-1,TabUnk,ActSeeFor		,{0x3C4,0x3F8},0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_DATA,Act_1ST_TAB,NULL			,For_ReceiveForumPost		},
   [ActRcvRepForDegTch	] = { 617,-1,TabUnk,ActSeeFor		,{0x3C4,0x3F0},0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_DATA,Act_1ST_TAB,NULL			,For_ReceiveForumPost		},
   [ActRcvRepForCtrUsr	] = { 905,-1,TabUnk,ActSeeFor		,{0x3C4,0x3F8},0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_DATA,Act_1ST_TAB,NULL			,For_ReceiveForumPost		},
   [ActRcvRepForCtrTch	] = { 906,-1,TabUnk,ActSeeFor		,{0x3C4,0x3F0},0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_DATA,Act_1ST_TAB,NULL			,For_ReceiveForumPost		},
   [ActRcvRepForInsUsr	] = { 740,-1,TabUnk,ActSeeFor		,{0x3C4,0x3F8},0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_DATA,Act_1ST_TAB,NULL			,For_ReceiveForumPost		},
   [ActRcvRepForInsTch	] = { 770,-1,TabUnk,ActSeeFor		,{0x3C4,0x3F0},0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_DATA,Act_1ST_TAB,NULL			,For_ReceiveForumPost		},
   [ActRcvRepForGenUsr	] = { 747,-1,TabUnk,ActSeeFor		,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_DATA,Act_1ST_TAB,NULL			,For_ReceiveForumPost		},
   [ActRcvRepForGenTch	] = { 816,-1,TabUnk,ActSeeFor		,{0x3C4,0x3F0},0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_DATA,Act_1ST_TAB,NULL			,For_ReceiveForumPost		},
   [ActRcvRepForSWAUsr	] = { 603,-1,TabUnk,ActSeeFor		,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_DATA,Act_1ST_TAB,NULL			,For_ReceiveForumPost		},
   [ActRcvRepForSWATch	] = { 622,-1,TabUnk,ActSeeFor		,{0x3C4,0x3F0},0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_DATA,Act_1ST_TAB,NULL			,For_ReceiveForumPost		},
   [ActReqDelThrCrsUsr	] = { 867,-1,TabUnk,ActSeeFor		,{0x204,0x220},0x204,0x204,0x204,0x204,0x204,Act_CONT_NORM,Act_1ST_TAB,NULL			,For_ReqRemThread		},
   [ActReqDelThrCrsTch	] = { 869,-1,TabUnk,ActSeeFor		,{0x204,0x220},0x204,0x204,0x204,0x204,0x204,Act_CONT_NORM,Act_1ST_TAB,NULL			,For_ReqRemThread		},
   [ActReqDelThrDegUsr	] = { 907,-1,TabUnk,ActSeeFor		,{0x3C4,0x3C0},0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_1ST_TAB,NULL			,For_ReqRemThread		},
   [ActReqDelThrDegTch	] = { 908,-1,TabUnk,ActSeeFor		,{0x3C4,0x3C0},0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_1ST_TAB,NULL			,For_ReqRemThread		},
   [ActReqDelThrCtrUsr	] = { 909,-1,TabUnk,ActSeeFor		,{0x3C4,0x3C0},0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_1ST_TAB,NULL			,For_ReqRemThread		},
   [ActReqDelThrCtrTch	] = { 910,-1,TabUnk,ActSeeFor		,{0x3C4,0x3C0},0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_1ST_TAB,NULL			,For_ReqRemThread		},
   [ActReqDelThrInsUsr	] = { 911,-1,TabUnk,ActSeeFor		,{0x204,0x200},0x204,0x204,0x204,0x204,0x204,Act_CONT_NORM,Act_1ST_TAB,NULL			,For_ReqRemThread		},
   [ActReqDelThrInsTch	] = { 912,-1,TabUnk,ActSeeFor		,{0x204,0x200},0x204,0x204,0x204,0x204,0x204,Act_CONT_NORM,Act_1ST_TAB,NULL			,For_ReqRemThread		},
   [ActReqDelThrGenUsr	] = { 913,-1,TabUnk,ActSeeFor		,{0x204,0x200},0x204,0x204,0x204,0x204,0x204,Act_CONT_NORM,Act_1ST_TAB,NULL			,For_ReqRemThread		},
   [ActReqDelThrGenTch	] = { 914,-1,TabUnk,ActSeeFor		,{0x204,0x200},0x204,0x204,0x204,0x204,0x204,Act_CONT_NORM,Act_1ST_TAB,NULL			,For_ReqRemThread		},
   [ActReqDelThrSWAUsr	] = { 881,-1,TabUnk,ActSeeFor		,{0x204,0x200},0x204,0x204,0x204,0x204,0x204,Act_CONT_NORM,Act_1ST_TAB,NULL			,For_ReqRemThread		},
   [ActReqDelThrSWATch	] = { 915,-1,TabUnk,ActSeeFor		,{0x204,0x200},0x204,0x204,0x204,0x204,0x204,Act_CONT_NORM,Act_1ST_TAB,NULL			,For_ReqRemThread		},
   [ActDelThrForCrsUsr	] = { 868,-1,TabUnk,ActSeeFor		,{0x204,0x220},0x204,0x204,0x204,0x204,0x204,Act_CONT_NORM,Act_1ST_TAB,NULL			,For_RemoveThread		},
   [ActDelThrForCrsTch	] = { 876,-1,TabUnk,ActSeeFor		,{0x204,0x220},0x204,0x204,0x204,0x204,0x204,Act_CONT_NORM,Act_1ST_TAB,NULL			,For_RemoveThread		},
   [ActDelThrForDegUsr	] = { 916,-1,TabUnk,ActSeeFor		,{0x3C4,0x3C0},0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_1ST_TAB,NULL			,For_RemoveThread		},
   [ActDelThrForDegTch	] = { 917,-1,TabUnk,ActSeeFor		,{0x3C4,0x3C0},0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_1ST_TAB,NULL			,For_RemoveThread		},
   [ActDelThrForCtrUsr	] = { 918,-1,TabUnk,ActSeeFor		,{0x3C4,0x3C0},0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_1ST_TAB,NULL			,For_RemoveThread		},
   [ActDelThrForCtrTch	] = { 919,-1,TabUnk,ActSeeFor		,{0x3C4,0x3C0},0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_1ST_TAB,NULL			,For_RemoveThread		},
   [ActDelThrForInsUsr	] = { 920,-1,TabUnk,ActSeeFor		,{0x204,0x200},0x204,0x204,0x204,0x204,0x204,Act_CONT_NORM,Act_1ST_TAB,NULL			,For_RemoveThread		},
   [ActDelThrForInsTch	] = { 921,-1,TabUnk,ActSeeFor		,{0x204,0x200},0x204,0x204,0x204,0x204,0x204,Act_CONT_NORM,Act_1ST_TAB,NULL			,For_RemoveThread		},
   [ActDelThrForGenUsr	] = { 922,-1,TabUnk,ActSeeFor		,{0x204,0x200},0x204,0x204,0x204,0x204,0x204,Act_CONT_NORM,Act_1ST_TAB,NULL			,For_RemoveThread		},
   [ActDelThrForGenTch	] = { 923,-1,TabUnk,ActSeeFor		,{0x204,0x200},0x204,0x204,0x204,0x204,0x204,Act_CONT_NORM,Act_1ST_TAB,NULL			,For_RemoveThread		},
   [ActDelThrForSWAUsr	] = { 882,-1,TabUnk,ActSeeFor		,{0x204,0x200},0x204,0x204,0x204,0x204,0x204,Act_CONT_NORM,Act_1ST_TAB,NULL			,For_RemoveThread		},
   [ActDelThrForSWATch	] = { 924,-1,TabUnk,ActSeeFor		,{0x204,0x200},0x204,0x204,0x204,0x204,0x204,Act_CONT_NORM,Act_1ST_TAB,NULL			,For_RemoveThread		},
   [ActCutThrForCrsUsr	] = { 926,-1,TabUnk,ActSeeFor		,{0x200,0x200},0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_1ST_TAB,NULL			,For_CutThread			},
   [ActCutThrForCrsTch	] = { 927,-1,TabUnk,ActSeeFor		,{0x200,0x200},0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_1ST_TAB,NULL			,For_CutThread			},
   [ActCutThrForDegUsr	] = { 928,-1,TabUnk,ActSeeFor		,{0x200,0x200},0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_1ST_TAB,NULL			,For_CutThread			},
   [ActCutThrForDegTch	] = { 929,-1,TabUnk,ActSeeFor		,{0x200,0x200},0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_1ST_TAB,NULL			,For_CutThread			},
   [ActCutThrForCtrUsr	] = { 930,-1,TabUnk,ActSeeFor		,{0x200,0x200},0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_1ST_TAB,NULL			,For_CutThread			},
   [ActCutThrForCtrTch	] = { 931,-1,TabUnk,ActSeeFor		,{0x200,0x200},0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_1ST_TAB,NULL			,For_CutThread			},
   [ActCutThrForInsUsr	] = { 932,-1,TabUnk,ActSeeFor		,{0x200,0x200},0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_1ST_TAB,NULL			,For_CutThread			},
   [ActCutThrForInsTch	] = { 933,-1,TabUnk,ActSeeFor		,{0x200,0x200},0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_1ST_TAB,NULL			,For_CutThread			},
   [ActCutThrForGenUsr	] = { 934,-1,TabUnk,ActSeeFor		,{0x200,0x200},0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_1ST_TAB,NULL			,For_CutThread			},
   [ActCutThrForGenTch	] = { 935,-1,TabUnk,ActSeeFor		,{0x200,0x200},0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_1ST_TAB,NULL			,For_CutThread			},
   [ActCutThrForSWAUsr	] = { 890,-1,TabUnk,ActSeeFor		,{0x200,0x200},0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_1ST_TAB,NULL			,For_CutThread			},
   [ActCutThrForSWATch	] = { 936,-1,TabUnk,ActSeeFor		,{0x200,0x200},0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_1ST_TAB,NULL			,For_CutThread			},
   [ActPasThrForCrsUsr	] = { 891,-1,TabUnk,ActSeeFor		,{0x200,0x200},0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_1ST_TAB,NULL			,For_PasteThread		},
   [ActPasThrForCrsTch	] = { 937,-1,TabUnk,ActSeeFor		,{0x200,0x200},0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_1ST_TAB,NULL			,For_PasteThread		},
   [ActPasThrForDegUsr	] = { 938,-1,TabUnk,ActSeeFor		,{0x200,0x200},0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_1ST_TAB,NULL			,For_PasteThread		},
   [ActPasThrForDegTch	] = { 939,-1,TabUnk,ActSeeFor		,{0x200,0x200},0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_1ST_TAB,NULL			,For_PasteThread		},
   [ActPasThrForCtrUsr	] = { 940,-1,TabUnk,ActSeeFor		,{0x200,0x200},0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_1ST_TAB,NULL			,For_PasteThread		},
   [ActPasThrForCtrTch	] = { 941,-1,TabUnk,ActSeeFor		,{0x200,0x200},0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_1ST_TAB,NULL			,For_PasteThread		},
   [ActPasThrForInsUsr	] = { 942,-1,TabUnk,ActSeeFor		,{0x200,0x200},0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_1ST_TAB,NULL			,For_PasteThread		},
   [ActPasThrForInsTch	] = { 943,-1,TabUnk,ActSeeFor		,{0x200,0x200},0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_1ST_TAB,NULL			,For_PasteThread		},
   [ActPasThrForGenUsr	] = { 944,-1,TabUnk,ActSeeFor		,{0x200,0x200},0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_1ST_TAB,NULL			,For_PasteThread		},
   [ActPasThrForGenTch	] = { 945,-1,TabUnk,ActSeeFor		,{0x200,0x200},0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_1ST_TAB,NULL			,For_PasteThread		},
   [ActPasThrForSWAUsr	] = { 946,-1,TabUnk,ActSeeFor		,{0x200,0x200},0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_1ST_TAB,NULL			,For_PasteThread		},
   [ActPasThrForSWATch	] = { 947,-1,TabUnk,ActSeeFor		,{0x200,0x200},0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_1ST_TAB,NULL			,For_PasteThread		},
   [ActDelPstForCrsUsr	] = { 602,-1,TabUnk,ActSeeFor		,{0x204,0x238},0x204,0x204,0x204,0x204,0x204,Act_CONT_NORM,Act_1ST_TAB,NULL			,For_RemovePost			},
   [ActDelPstForCrsTch	] = { 756,-1,TabUnk,ActSeeFor		,{0x204,0x230},0x204,0x204,0x204,0x204,0x204,Act_CONT_NORM,Act_1ST_TAB,NULL			,For_RemovePost			},
   [ActDelPstForDegUsr	] = { 608,-1,TabUnk,ActSeeFor		,{0x3C4,0x3F8},0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_1ST_TAB,NULL			,For_RemovePost			},
   [ActDelPstForDegTch	] = { 680,-1,TabUnk,ActSeeFor		,{0x3C4,0x3F0},0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_1ST_TAB,NULL			,For_RemovePost			},
   [ActDelPstForCtrUsr	] = { 948,-1,TabUnk,ActSeeFor		,{0x3C4,0x3F8},0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_1ST_TAB,NULL			,For_RemovePost			},
   [ActDelPstForCtrTch	] = { 949,-1,TabUnk,ActSeeFor		,{0x3C4,0x3F0},0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_1ST_TAB,NULL			,For_RemovePost			},
   [ActDelPstForInsUsr	] = { 743,-1,TabUnk,ActSeeFor		,{0x3C4,0x3F8},0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_1ST_TAB,NULL			,For_RemovePost			},
   [ActDelPstForInsTch	] = { 772,-1,TabUnk,ActSeeFor		,{0x3C4,0x3F0},0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_1ST_TAB,NULL			,For_RemovePost			},
   [ActDelPstForGenUsr	] = { 735,-1,TabUnk,ActSeeFor		,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_1ST_TAB,NULL			,For_RemovePost			},
   [ActDelPstForGenTch	] = { 950,-1,TabUnk,ActSeeFor		,{0x3C4,0x3F0},0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_1ST_TAB,NULL			,For_RemovePost			},
   [ActDelPstForSWAUsr	] = { 613,-1,TabUnk,ActSeeFor		,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_1ST_TAB,NULL			,For_RemovePost			},
   [ActDelPstForSWATch	] = { 623,-1,TabUnk,ActSeeFor		,{0x3C4,0x3F0},0x3C4,0x3C4,0x3C4,0x3C4,0x3C4,Act_CONT_NORM,Act_1ST_TAB,NULL			,For_RemovePost			},
   [ActEnbPstForCrsUsr	] = { 624,-1,TabUnk,ActSeeFor		,{0x200,0x220},0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_1ST_TAB,NULL			,For_EnablePost			},
   [ActEnbPstForCrsTch	] = { 951,-1,TabUnk,ActSeeFor		,{0x200,0x220},0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_1ST_TAB,NULL			,For_EnablePost			},
   [ActEnbPstForDegUsr	] = { 616,-1,TabUnk,ActSeeFor		,{0x3C0,0x3C0},0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_1ST_TAB,NULL			,For_EnablePost			},
   [ActEnbPstForDegTch	] = { 619,-1,TabUnk,ActSeeFor		,{0x3C0,0x3C0},0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_1ST_TAB,NULL			,For_EnablePost			},
   [ActEnbPstForCtrUsr	] = { 952,-1,TabUnk,ActSeeFor		,{0x3C0,0x3C0},0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_1ST_TAB,NULL			,For_EnablePost			},
   [ActEnbPstForCtrTch	] = { 953,-1,TabUnk,ActSeeFor		,{0x3C0,0x3C0},0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_1ST_TAB,NULL			,For_EnablePost			},
   [ActEnbPstForInsUsr	] = { 954,-1,TabUnk,ActSeeFor		,{0x3C0,0x3C0},0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_1ST_TAB,NULL			,For_EnablePost			},
   [ActEnbPstForInsTch	] = { 955,-1,TabUnk,ActSeeFor		,{0x3C0,0x3C0},0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_1ST_TAB,NULL			,For_EnablePost			},
   [ActEnbPstForGenUsr	] = { 956,-1,TabUnk,ActSeeFor		,{0x3C0,0x3C0},0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_1ST_TAB,NULL			,For_EnablePost			},
   [ActEnbPstForGenTch	] = { 957,-1,TabUnk,ActSeeFor		,{0x3C0,0x3C0},0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_1ST_TAB,NULL			,For_EnablePost			},
   [ActEnbPstForSWAUsr	] = { 632,-1,TabUnk,ActSeeFor		,{0x3C0,0x3C0},0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_1ST_TAB,NULL			,For_EnablePost			},
   [ActEnbPstForSWATch	] = { 634,-1,TabUnk,ActSeeFor		,{0x3C0,0x3C0},0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_1ST_TAB,NULL			,For_EnablePost			},
   [ActDisPstForCrsUsr	] = { 610,-1,TabUnk,ActSeeFor		,{0x200,0x220},0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_1ST_TAB,NULL			,For_DisablePost		},
   [ActDisPstForCrsTch	] = { 958,-1,TabUnk,ActSeeFor		,{0x200,0x220},0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_1ST_TAB,NULL			,For_DisablePost		},
   [ActDisPstForDegUsr	] = { 615,-1,TabUnk,ActSeeFor		,{0x3C0,0x3C0},0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_1ST_TAB,NULL			,For_DisablePost		},
   [ActDisPstForDegTch	] = { 618,-1,TabUnk,ActSeeFor		,{0x3C0,0x3C0},0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_1ST_TAB,NULL			,For_DisablePost		},
   [ActDisPstForCtrUsr	] = { 959,-1,TabUnk,ActSeeFor		,{0x3C0,0x3C0},0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_1ST_TAB,NULL			,For_DisablePost		},
   [ActDisPstForCtrTch	] = { 960,-1,TabUnk,ActSeeFor		,{0x3C0,0x3C0},0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_1ST_TAB,NULL			,For_DisablePost		},
   [ActDisPstForInsUsr	] = { 961,-1,TabUnk,ActSeeFor		,{0x3C0,0x3C0},0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_1ST_TAB,NULL			,For_DisablePost		},
   [ActDisPstForInsTch	] = { 962,-1,TabUnk,ActSeeFor		,{0x3C0,0x3C0},0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_1ST_TAB,NULL			,For_DisablePost		},
   [ActDisPstForGenUsr	] = { 963,-1,TabUnk,ActSeeFor		,{0x3C0,0x3C0},0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_1ST_TAB,NULL			,For_DisablePost		},
   [ActDisPstForGenTch	] = { 925,-1,TabUnk,ActSeeFor		,{0x3C0,0x3C0},0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_1ST_TAB,NULL			,For_DisablePost		},
   [ActDisPstForSWAUsr	] = { 625,-1,TabUnk,ActSeeFor		,{0x3C0,0x3C0},0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_1ST_TAB,NULL			,For_DisablePost		},
   [ActDisPstForSWATch	] = { 635,-1,TabUnk,ActSeeFor		,{0x3C0,0x3C0},0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_1ST_TAB,NULL			,For_DisablePost		},
   [ActReqLnkForCrsUsr  ] = {1941,-1,TabUnk,ActSeeFor		,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,ForRsc_GetLinkToThread		},

   // Chat
   [ActSeeChtRms	] = {  51, 3,TabMsg,ActSeeChtRms	,{0x200,0x200},0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_1ST_TAB,NULL			,Cht_ShowChatRooms		},
   [ActCht		] = {  52,-1,TabUnk,ActSeeChtRms	,{0x200,0x200},0x200,0x200,0x200,0x200,0x200,Act_CONT_NORM,Act_NEW_TAB,Cht_OpenChatWindow	,NULL				},

   // Messages
   [ActSeeRcvMsg	] = {   3, 4,TabMsg,ActSeeRcvMsg	,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_1ST_TAB,NULL			,Msg_ShowRecMsgs		},
   [ActReqMsgUsr	] = {  26,-1,TabUnk,ActSeeRcvMsg	,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_1ST_TAB,NULL			,Msg_FormMsgUsrs		},
   [ActSeeSntMsg	] = {  70,-1,TabUnk,ActSeeRcvMsg	,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_1ST_TAB,NULL			,Msg_ShowSntMsgs		},
   [ActRcvMsgUsr	] = {  27,-1,TabUnk,ActSeeRcvMsg	,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_DATA,Act_1ST_TAB,NULL			,Msg_RecMsgFromUsr		},
   [ActReqDelAllSntMsg	] = { 604,-1,TabUnk,ActSeeRcvMsg	,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_1ST_TAB,NULL			,Msg_ReqDelAllSntMsgs		},
   [ActReqDelAllRcvMsg	] = { 593,-1,TabUnk,ActSeeRcvMsg	,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_1ST_TAB,NULL			,Msg_ReqDelAllRecMsgs		},
   [ActDelAllSntMsg	] = { 434,-1,TabUnk,ActSeeRcvMsg	,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_1ST_TAB,NULL			,Msg_DelAllSntMsgs		},
   [ActDelAllRcvMsg	] = { 436,-1,TabUnk,ActSeeRcvMsg	,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_1ST_TAB,NULL			,Msg_DelAllRecMsgs		},
   [ActDelSntMsg	] = {  90,-1,TabUnk,ActSeeRcvMsg	,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_1ST_TAB,NULL			,Msg_DelSntMsg			},
   [ActDelRcvMsg	] = {  64,-1,TabUnk,ActSeeRcvMsg	,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_1ST_TAB,NULL			,Msg_DelRecMsg			},
   [ActExpSntMsg	] = { 664,-1,TabUnk,ActSeeRcvMsg	,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_1ST_TAB,NULL			,Msg_ExpSntMsg			},
   [ActExpRcvMsg	] = { 663,-1,TabUnk,ActSeeRcvMsg	,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_1ST_TAB,NULL			,Msg_ExpRecMsg			},
   [ActConSntMsg	] = {1020,-1,TabUnk,ActSeeRcvMsg	,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_1ST_TAB,NULL			,Msg_ConSntMsg			},
   [ActConRcvMsg	] = {1019,-1,TabUnk,ActSeeRcvMsg	,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_1ST_TAB,NULL			,Msg_ConRecMsg			},
   [ActLstBanUsr	] = {1017,-1,TabUnk,ActSeeRcvMsg	,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_1ST_TAB,NULL			,Msg_ListBannedUsrs		},
   [ActBanUsrMsg	] = {1015,-1,TabUnk,ActSeeRcvMsg	,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_1ST_TAB,NULL			,Msg_BanSenderWhenShowingMsgs	},
   [ActUnbUsrMsg	] = {1016,-1,TabUnk,ActSeeRcvMsg	,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_1ST_TAB,NULL			,Msg_UnbanSenderWhenShowingMsgs	},
   [ActUnbUsrLst	] = {1018,-1,TabUnk,ActSeeRcvMsg	,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_1ST_TAB,NULL			,Msg_UnbanSenderWhenListingUsrs	},

   // Mail
   [ActReqMaiUsr	] = {1772, 5,TabMsg,ActReqMaiUsr	,{0x200,0x230},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Mai_ReqUsrsToListEmails	},
   [ActMaiUsr		] = { 100,-1,TabUnk,ActReqMaiUsr	,{0x200,0x230},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Mai_GetSelUsrsAndListEmails	},

   // TabAna ******************************************************************
   // Figures
   [ActReqUseGbl	] = { 761, 0,TabAna,ActReqUseGbl	,{0x3C7,0x3F8},0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_1ST_TAB,NULL			,Fig_ReqShowFigures		},
   [ActSeeUseGbl	] = {  84,-1,TabUnk,ActReqUseGbl	,{0x3C7,0x3F8},0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_1ST_TAB,NULL			,Fig_ShowFigures		},

   // Degrees
   [ActSeePhoDeg	] = { 447, 1,TabAna,ActSeePhoDeg	,{0x3C7,0x3F8},0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_1ST_TAB,NULL			,Pho_ShowPhotoDegree		},
   [ActPrnPhoDeg	] = { 448,-1,TabUnk,ActSeePhoDeg	,{0x3C7,0x3F8},0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_NEW_TAB,NULL			,Pho_PrintPhotoDegree  		},
   [ActCalPhoDeg	] = { 444,-1,TabUnk,ActSeePhoDeg	,{0x3C7,0x3F8},0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_1ST_TAB,NULL			,Pho_CalcPhotoDegree		},

   // Indicators
   [ActReqStaCrs	] = { 767, 2,TabAna,ActReqStaCrs	,{0x3C7,0x3F8},0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_1ST_TAB,NULL			,Ind_ReqIndicatorsCourses	},
   [ActSeeAllStaCrs	] = { 768,-1,TabUnk,ActReqAccGbl	,{0x3C7,0x3F8},0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_NEW_TAB,NULL			,Ind_ShowIndicatorsCourses	},

   // Surveys
   [ActSeeAllSvy	] = { 966, 3,TabAna,ActSeeAllSvy	,{0x3C0,0x3F8},0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Svy_SeeAllSurveys		},
   [ActSeeOneSvy	] = { 982,-1,TabUnk,ActSeeAllSvy	,{0x3C0,0x3F8},0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Svy_SeeOneSurvey		},
   [ActAnsSvy		] = { 983,-1,TabUnk,ActSeeAllSvy	,{0x3C0,0x3F8},0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Svy_ReceiveSurveyAnswers	},
   [ActFrmNewSvy	] = { 973,-1,TabUnk,ActSeeAllSvy	,{0x3C0,0x3E0},0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Svy_ReqCreatOrEditSvy		},
   [ActEdiOneSvy	] = { 974,-1,TabUnk,ActSeeAllSvy	,{0x3C0,0x3E0},0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Svy_ReqCreatOrEditSvy		},
   [ActNewSvy		] = { 968,-1,TabUnk,ActSeeAllSvy	,{0x3C0,0x3E0},0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Svy_ReceiveSurvey		},
   [ActChgSvy		] = { 975,-1,TabUnk,ActSeeAllSvy	,{0x3C0,0x3E0},0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Svy_ReceiveSurvey		},
   [ActReqRemSvy	] = { 976,-1,TabUnk,ActSeeAllSvy	,{0x3C0,0x3E0},0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Svy_AskRemSurvey		},
   [ActRemSvy		] = { 969,-1,TabUnk,ActSeeAllSvy	,{0x3C0,0x3E0},0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Svy_RemoveSurvey		},
   [ActReqRstSvy	] = { 984,-1,TabUnk,ActSeeAllSvy	,{0x3C0,0x3E0},0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Svy_AskResetSurvey		},
   [ActRstSvy		] = { 985,-1,TabUnk,ActSeeAllSvy	,{0x3C0,0x3E0},0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Svy_ResetSurvey		},
   [ActHidSvy		] = { 977,-1,TabUnk,ActSeeAllSvy	,{0x3C0,0x3E0},0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Svy_HideSurvey			},
   [ActUnhSvy		] = { 978,-1,TabUnk,ActSeeAllSvy	,{0x3C0,0x3E0},0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Svy_UnhideSurvey		},
   [ActReqLnkSvy	] = {1937,-1,TabUnk,ActSeeAllSvy	,{0x200,0x220},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,SvyRsc_GetLinkToSurvey		},
   [ActEdiOneSvyQst	] = { 979,-1,TabUnk,ActSeeAllSvy	,{0x3C0,0x3E0},0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Svy_ReqEditQuestion		},
   [ActNewSvyQst	] = { 980,-1,TabUnk,ActSeeAllSvy	,{0x3C0,0x3E0},0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Svy_ReceiveQst			},
   [ActChgSvyQst	] = {1976,-1,TabUnk,ActSeeAllSvy	,{0x3C0,0x3E0},0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Svy_ReceiveQst			},
   [ActReqRemSvyQst	] = {1524,-1,TabUnk,ActSeeAllSvy	,{0x3C0,0x3E0},0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Svy_ReqRemQst			},
   [ActRemSvyQst	] = { 981,-1,TabUnk,ActSeeAllSvy	,{0x3C0,0x3E0},0x3C0,0x3C0,0x3C0,0x3C0,0x3C0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Svy_RemoveQst			},

   // Visits
   [ActReqAccGbl	] = { 591, 4,TabAna,ActReqAccGbl	,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_1ST_TAB,Dat_SetDatesToRecentWeeks,Sta_AskShowGblHits		},
   [ActSeeAccGbl	] = {  79,-1,TabUnk,ActReqAccGbl	,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_1ST_TAB,NULL			,Sta_SeeGblAccesses		},
   [ActReqAccCrs	] = { 594,-1,TabUnk,ActReqAccGbl	,{0x200,0x230},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,Dat_SetDatesToRecentWeeks,Sta_ReqCrsHits			},
   [ActSeeAccCrs	] = { 119,-1,TabUnk,ActReqAccGbl	,{0x200,0x230},    0,    0,    0,    0,    0,Act_CONT_NORM,Act_1ST_TAB,NULL			,Sta_SeeCrsAccesses		},
   [ActLstClk		] = { 989,-1,TabUnk,ActReqAccGbl	,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_1ST_TAB,NULL			,Log_ShowLastClicks		},
   [ActRefLstClk	] = { 994,-1,TabUnk,ActReqAccGbl	,{0x3C7,0x3F8},0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_AJAX_RF,NULL			,Lay_RefreshLastClicks		},

   // Report
   [ActReqMyUsgRep	] = {1586, 5,TabAna,ActReqMyUsgRep	,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_1ST_TAB,NULL			,Rep_ReqMyUsageReport		},
   [ActSeeMyUsgRep	] = {1582,-1,TabUnk,ActReqMyUsgRep	,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_1ST_TAB,NULL			,Rep_ShowMyUsageReport		},

   // Frequent
   [ActMFUAct		] = { 993, 6,TabAna,ActMFUAct		,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_1ST_TAB,NULL			,MFU_ShowMyMFUActions		},

   // TabPrf ******************************************************************
   // Session
   [ActFrmRolSes	] = { 843, 0,TabPrf,ActFrmRolSes	,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_1ST_TAB,NULL			,Usr_WriteFormLogout		},
   [ActChgMyRol		] = { 589,-1,TabUnk,ActFrmRolSes	,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_1ST_TAB,Rol_ChangeMyRole		,Usr_ShowFormsLogoutAndRole	},

   // Courses
   [ActMyCrs		] = { 987, 1,TabPrf,ActMyCrs		,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_1ST_TAB,NULL			,Hie_SelOneNodeFromMyHierarchy	},

   // Timetable
   [ActSeeMyTT		] = { 408, 2,TabPrf,ActSeeMyTT		,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_1ST_TAB,NULL			,Tmt_ShowClassTimeTable		},
   [ActPrnMyTT		] = { 409,-1,TabUnk,ActSeeMyTT		,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_NEW_TAB,NULL			,Tmt_ShowClassTimeTable		},
   [ActEdiTut		] = {  65,-1,TabUnk,ActSeeMyTT		,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_1ST_TAB,NULL			,Tmt_EditMyTutTimeTable		},
   [ActChgTut		] = {  48,-1,TabUnk,ActSeeMyTT		,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_1ST_TAB,NULL			,Tmt_EditMyTutTimeTable		},
   [ActChgMyTT1stDay	] = {1487,-1,TabUnk,ActSeeMyTT		,{0x3C7,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_1ST_TAB,Cal_ChangeFirstDayOfWeek	,Tmt_ShowClassTimeTable		},

   // Agenda
   [ActSeeMyAgd		] = {1602, 3,TabPrf,ActSeeMyAgd		,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_1ST_TAB,NULL			,Agd_GetParsAndShowMyAgenda	},
   [ActFrmNewEvtMyAgd	] = {1603,-1,TabUnk,ActSeeMyAgd		,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_1ST_TAB,NULL			,Agd_ReqCreatOrEditEvent	},
   [ActEdiOneEvtMyAgd	] = {1604,-1,TabUnk,ActSeeMyAgd		,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_1ST_TAB,NULL			,Agd_ReqCreatOrEditEvent	},
   [ActNewEvtMyAgd	] = {1605,-1,TabUnk,ActSeeMyAgd		,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_1ST_TAB,NULL			,Agd_ReceiveEvent		},
   [ActChgEvtMyAgd	] = {1606,-1,TabUnk,ActSeeMyAgd		,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_1ST_TAB,NULL			,Agd_ReceiveEvent		},
   [ActReqRemEvtMyAgd	] = {1607,-1,TabUnk,ActSeeMyAgd		,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_1ST_TAB,NULL			,Agd_AskRemEvent		},
   [ActRemEvtMyAgd	] = {1608,-1,TabUnk,ActSeeMyAgd		,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_1ST_TAB,NULL			,Agd_RemoveEvent		},
   [ActHidEvtMyAgd	] = {1612,-1,TabUnk,ActSeeMyAgd		,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_1ST_TAB,NULL			,Agd_HideEvent			},
   [ActUnhEvtMyAgd	] = {1613,-1,TabUnk,ActSeeMyAgd		,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_1ST_TAB,NULL			,Agd_UnhideEvent		},
   [ActPrvEvtMyAgd	] = {1609,-1,TabUnk,ActSeeMyAgd		,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_1ST_TAB,NULL			,Agd_MakeEventPrivate		},
   [ActPubEvtMyAgd	] = {1610,-1,TabUnk,ActSeeMyAgd		,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_1ST_TAB,NULL			,Agd_MakeEventPublic		},
   [ActPrnAgdQR		] = {1618,-1,TabUnk,ActSeeMyAgd		,{0x3C7,0x3F8},0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_NEW_TAB,NULL			,Agd_PrintAgdQRCode		},

   // Account
   [ActFrmMyAcc		] = {  36, 4,TabPrf,ActFrmMyAcc		,{0x3C7,0x3F8},0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_1ST_TAB,NULL			,Acc_ShowFormMyAccount		},
   [ActChkUsrAcc	] = {1584,-1,TabUnk,ActFrmMyAcc		,{0x3C7,0x3F8},0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_1ST_TAB,NULL			,Acc_CheckIfEmptyAccountExists	},
   [ActCreMyAcc		] = {1787,-1,TabUnk,ActFrmMyAcc		,{0x001,0x001},0x001,0x001,0x001,0x001,0x001,Act_CONT_NORM,Act_1ST_TAB,NULL			,Acc_ShowFormCreateMyAccount	},
   [ActCreUsrAcc	] = {1163,-1,TabUnk,ActFrmMyAcc		,{0x3C7,0x3F8},0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_1ST_TAB,NULL			,Acc_AfterCreationNewAccount	},
   [ActRemMyID		] = {1147,-1,TabUnk,ActFrmMyAcc		,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_1ST_TAB,NULL			,ID_RemoveMyUsrID		},
   [ActChgMyID		] = {1148,-1,TabUnk,ActFrmMyAcc		,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_1ST_TAB,NULL			,ID_NewMyUsrID			},
   [ActRemMyNck		] = {1089,-1,TabUnk,ActFrmMyAcc		,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_1ST_TAB,NULL			,Nck_RemoveMyNick		},
   [ActChgMyNck		] = {  37,-1,TabUnk,ActFrmMyAcc		,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_1ST_TAB,NULL			,Nck_UpdateMyNick		},
   [ActRemMyMai		] = {1090,-1,TabUnk,ActFrmMyAcc		,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_1ST_TAB,NULL			,Mai_RemoveMyUsrEmail		},
   [ActChgMyMai		] = {1088,-1,TabUnk,ActFrmMyAcc		,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_1ST_TAB,NULL			,May_NewMyUsrEmail		},
   [ActCnfMai		] = {1091,-1,TabUnk,ActFrmMyAcc		,{0x3C7,0x3F8},0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_1ST_TAB,NULL			,Mai_ConfirmEmail		},
   [ActChgMyPwd		] = {  35,-1,TabUnk,ActFrmMyAcc		,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_1ST_TAB,Pwd_UpdateMyPwd		,Acc_ShowFormChgMyAccount	},
   [ActReqRemMyAcc	] = {1430,-1,TabUnk,ActFrmMyAcc		,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_1ST_TAB,NULL			,Acc_AskIfRemoveMyAccount	},
   [ActRemMyAcc		] = {1431,-1,TabUnk,ActFrmMyAcc		,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_1ST_TAB,NULL			,Acc_RemoveMyAccount		},
   [ActPrnUsrQR		] = {1022,-1,TabUnk,ActFrmMyAcc		,{0x3C7,0x3F8},0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_NEW_TAB,NULL			,QR_PrintUsrQRCode		},

   // Record
   [ActReqEdiRecSha	] = { 285, 5,TabPrf,ActReqEdiRecSha	,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_1ST_TAB,NULL			,Rec_ShowMySharedRecordAndMore	},
   [ActChgMyData	] = { 298,-1,TabUnk,ActReqEdiRecSha	,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_1ST_TAB,Rec_UpdateMyRecord	,Rec_ShowMySharedRecordUpd	},
   [ActReqMyPho		] = {  30,-1,TabUnk,ActReqEdiRecSha	,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_1ST_TAB,NULL			,Pho_ReqMyPhoto			},
   [ActDetMyPho		] = { 693,-1,TabUnk,ActReqEdiRecSha	,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_DATA,Act_1ST_TAB,NULL			,Pho_RecMyPhotoDetFaces		},
   [ActUpdMyPho		] = { 694,-1,TabUnk,ActReqEdiRecSha	,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_1ST_TAB,Pho_UpdateMyPhoto1	,Pho_UpdateMyPhoto2		},
   [ActReqRemMyPho	] = {1577,-1,TabUnk,ActReqEdiRecSha	,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_1ST_TAB,NULL			,Pho_ReqRemMyPhoto		},
   [ActRemMyPho		] = { 428,-1,TabUnk,ActReqEdiRecSha	,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_1ST_TAB,Pho_RemoveMyPhoto1	,Pho_RemoveMyPhoto2		},
   [ActChgCtyMyIns	] = {1166,-1,TabUnk,ActReqEdiRecSha	,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_1ST_TAB,NULL			,Rec_ChgCountryOfMyInstitution	},
   [ActChgMyIns		] = {1167,-1,TabUnk,ActReqEdiRecSha	,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_1ST_TAB,NULL			,Rec_UpdateMyInstitution	},
   [ActChgMyCtr		] = {1168,-1,TabUnk,ActReqEdiRecSha	,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_1ST_TAB,NULL			,Rec_UpdateMyCenter		},
   [ActChgMyDpt		] = {1169,-1,TabUnk,ActReqEdiRecSha	,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_1ST_TAB,NULL			,Rec_UpdateMyDepartment		},
   [ActChgMyOff		] = {1170,-1,TabUnk,ActReqEdiRecSha	,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_1ST_TAB,NULL			,Rec_UpdateMyOffice		},
   [ActChgMyOffPho	] = {1171,-1,TabUnk,ActReqEdiRecSha	,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_1ST_TAB,NULL			,Rec_UpdateMyOfficePhone	},
   [ActReqEdiMyNet	] = {1172,-1,TabUnk,ActReqEdiRecSha	,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_1ST_TAB,NULL			,Rec_ShowMySharedRecordAndMore	},
   [ActChgMyNet		] = {1173,-1,TabUnk,ActReqEdiRecSha	,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_1ST_TAB,NULL			,Net_UpdateMyWebsAndSocialNets	},

   // Settings
   [ActReqEdiSet	] = { 673, 6,TabPrf,ActReqEdiSet	,{0x3C7,0x3F8},0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_1ST_TAB,NULL			,Set_EditSettings		},
   [ActChgThe		] = { 841,-1,TabUnk,ActReqEdiSet	,{0x3C7,0x3F8},0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_1ST_TAB,The_ChangeTheme		,Set_EditSettings		},
   [ActReqChgLan	] = { 992,-1,TabUnk,ActReqEdiSet	,{0x3C7,0x3F8},0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_1ST_TAB,NULL			,Lan_AskChangeLanguage		},
   [ActChgLan		] = { 654,-1,TabUnk,ActReqEdiSet	,{0x3C7,0x3F8},0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_1ST_TAB,Lan_ChangeLanguage	,Set_EditSettings		},
   [ActChg1stDay	] = {1484,-1,TabUnk,ActReqEdiSet	,{0x3C7,0x3F8},0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_1ST_TAB,Cal_ChangeFirstDayOfWeek	,Set_EditSettings		},
   [ActChgDatFmt	] = {1638,-1,TabUnk,ActReqEdiSet	,{0x3C7,0x3F8},0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_1ST_TAB,Dat_ChangeDateFormat	,Set_EditSettings		},
   [ActChgCol		] = { 674,-1,TabUnk,ActReqEdiSet	,{0x3C7,0x3F8},0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_1ST_TAB,Set_ChangeSideCols	,Set_EditSettings		},
   [ActChgIco		] = {1092,-1,TabUnk,ActReqEdiSet	,{0x3C7,0x3F8},0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_1ST_TAB,Ico_ChangeIconSet	,Set_EditSettings		},
   [ActChgMnu		] = {1243,-1,TabUnk,ActReqEdiSet	,{0x3C7,0x3F8},0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_1ST_TAB,Mnu_ChangeMenu		,Set_EditSettings		},
   [ActChgUsrPho	] = {1917,-1,TabUnk,ActReqEdiSet	,{0x3C7,0x3F8},0x3C7,0x3C7,0x3C7,0x3C7,0x3C7,Act_CONT_NORM,Act_1ST_TAB,PhoSha_ChangePhotoShape	,Set_EditSettings		},
   [ActChgPriPho	] = { 774,-1,TabUnk,ActReqEdiSet	,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_1ST_TAB,NULL			,Pho_ChangePhotoVisibility	},
   [ActChgBasPriPrf	] = {1404,-1,TabUnk,ActReqEdiSet	,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_1ST_TAB,NULL			,Prf_ChangeBasicProfileVis	},
   [ActChgExtPriPrf	] = {1765,-1,TabUnk,ActReqEdiSet	,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_1ST_TAB,NULL			,Prf_ChangeExtendedProfileVis	},
   [ActChgCooPrf	] = {1764,-1,TabUnk,ActReqEdiSet	,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_1ST_TAB,NULL			,Coo_ChangeMyPrefsCookies	},
   [ActChgNtfPrf	] = { 775,-1,TabUnk,ActReqEdiSet	,{0x3C6,0x3F8},0x3C6,0x3C6,0x3C6,0x3C6,0x3C6,Act_CONT_NORM,Act_1ST_TAB,Ntf_ChangeNotifyEvents	,Set_EditSettings		},
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
	ActReqTst,		// #103
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
	ActReqSelGrp,		// #116
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
  };
