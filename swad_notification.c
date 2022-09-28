// swad_notification.c: notifications about events, sent by email

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2022 Antonio Ca�as Vargas

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
/********************************* Headers ***********************************/
/*****************************************************************************/

#define _GNU_SOURCE 		// For asprintf
#include <stddef.h>		// For NULL
#include <stdio.h>		// For asprintf
#include <stdlib.h>		// For system
#include <string.h>
#include <sys/wait.h>		// For the macro WEXITSTATUS
#include <unistd.h>		// For unlink

#include "swad_action.h"
#include "swad_admin_database.h"
#include "swad_assignment_database.h"
#include "swad_box.h"
#include "swad_browser_database.h"
#include "swad_call_for_exam.h"
#include "swad_config.h"
#include "swad_config.h"
#include "swad_database.h"
#include "swad_enrolment.h"
#include "swad_enrolment_database.h"
#include "swad_error.h"
#include "swad_figure.h"
#include "swad_follow.h"
#include "swad_form.h"
#include "swad_forum.h"
#include "swad_forum_database.h"
#include "swad_global.h"
#include "swad_group_database.h"
#include "swad_hierarchy.h"
#include "swad_hierarchy_level.h"
#include "swad_HTML.h"
#include "swad_mark.h"
#include "swad_message.h"
#include "swad_message_database.h"
#include "swad_notice.h"
#include "swad_notification.h"
#include "swad_notification_database.h"
#include "swad_parameter.h"
#include "swad_setting.h"
#include "swad_setting_database.h"
#include "swad_survey.h"
#include "swad_survey_database.h"
#include "swad_timeline.h"
#include "swad_timeline_database.h"
#include "swad_timeline_notification.h"
#include "swad_timeline_publication.h"
#include "swad_user_database.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/****************************** Public constants *****************************/
/*****************************************************************************/

// strings are limited to Ntf_MAX_BYTES_NOTIFY_EVENT characters

const char *Ntf_WSNotifyEvents[Ntf_NUM_NOTIFY_EVENTS] =
  {
   [Ntf_EVENT_UNKNOWN          ] = "unknown",
   /* Start tab */
   [Ntf_EVENT_TML_COMMENT      ] = "timelineComment",
   [Ntf_EVENT_TML_FAV          ] = "timelineFav",
   [Ntf_EVENT_TML_SHARE        ] = "timelineShare",
   [Ntf_EVENT_TML_MENTION      ] = "timelineMention",
   [Ntf_EVENT_FOLLOWER         ] = "follower",
   /* System tab */
   /* Country tab */
   /* Institution tab */
   /* Center tab */
   /* Degree tab */
   /* Course tab */
   /* Assessment tab */
   [Ntf_EVENT_ASSIGNMENT       ] = "assignment",
   [Ntf_EVENT_SURVEY           ] = "survey",
   [Ntf_EVENT_CALL_FOR_EXAM    ] = "examAnnouncement",
   /* Files tab */
   [Ntf_EVENT_DOCUMENT_FILE    ] = "documentFile",
   [Ntf_EVENT_TEACHERS_FILE    ] = "teachersFile",
   [Ntf_EVENT_SHARED_FILE      ] = "sharedFile",
   [Ntf_EVENT_MARKS_FILE       ] = "marksFile",
   /* Users tab */
   [Ntf_EVENT_ENROLMENT_STD    ] = "enrollmentStudent",
   [Ntf_EVENT_ENROLMENT_NET    ] = "enrolmentNonEditingTeacher",
   [Ntf_EVENT_ENROLMENT_TCH    ] = "enrollmentTeacher",
   [Ntf_EVENT_ENROLMENT_REQUEST] = "enrollmentRequest",
   /* Messages tab */
   [Ntf_EVENT_NOTICE           ] = "notice",
   [Ntf_EVENT_FORUM_POST_COURSE] = "forumPostCourse",
   [Ntf_EVENT_FORUM_REPLY      ] = "forumReply",
   [Ntf_EVENT_MESSAGE          ] = "message",
   /* Analytics tab */
   /* Profile tab */
  };

static const Act_Action_t Ntf_DefaultActions[Ntf_NUM_NOTIFY_EVENTS] =
  {
   [Ntf_EVENT_UNKNOWN          ] = ActUnk,
   /* Start tab */
   [Ntf_EVENT_TML_COMMENT      ] = ActSeeGblTL,
   [Ntf_EVENT_TML_FAV          ] = ActSeeGblTL,
   [Ntf_EVENT_TML_SHARE        ] = ActSeeGblTL,
   [Ntf_EVENT_TML_MENTION      ] = ActSeeGblTL,
   [Ntf_EVENT_FOLLOWER         ] = ActSeeFlr,
   /* System tab */
   /* Country tab */
   /* Institution tab */
   /* Center tab */
   /* Degree tab */
   /* Course tab */
   /* Assessment tab */
   [Ntf_EVENT_ASSIGNMENT       ] = ActSeeAsg,
   [Ntf_EVENT_SURVEY           ] = ActSeeAllSvy,
   [Ntf_EVENT_CALL_FOR_EXAM    ] = ActSeeAllCfe,
   /* Files tab */
   [Ntf_EVENT_DOCUMENT_FILE    ] = ActSeeAdmDocCrsGrp,
   [Ntf_EVENT_TEACHERS_FILE    ] = ActAdmTchCrsGrp,
   [Ntf_EVENT_SHARED_FILE      ] = ActAdmShaCrsGrp,
   [Ntf_EVENT_MARKS_FILE       ] = ActSeeAdmMrk,
   /* Users tab */
   [Ntf_EVENT_ENROLMENT_STD    ] = ActReqAccEnrStd,
   [Ntf_EVENT_ENROLMENT_NET    ] = ActReqAccEnrNET,
   [Ntf_EVENT_ENROLMENT_TCH    ] = ActReqAccEnrTch,
   [Ntf_EVENT_ENROLMENT_REQUEST] = ActSeeSignUpReq,
   /* Messages tab */
   [Ntf_EVENT_NOTICE           ] = ActSeeOneNot,
   [Ntf_EVENT_FORUM_POST_COURSE] = ActSeeFor,
   [Ntf_EVENT_FORUM_REPLY      ] = ActSeeFor,
   [Ntf_EVENT_MESSAGE          ] = ActExpRcvMsg,
   /* Analytics tab */
   /* Profile tab */
  };

/*****************************************************************************/
/***************************** Private constants *****************************/
/*****************************************************************************/

// Notify me notification events
static const char *Ntf_ParamNotifMeAboutNotifyEvents[Ntf_NUM_NOTIFY_EVENTS] =
  {
   [Ntf_EVENT_UNKNOWN          ] = "NotifyNtfEventUnknown",
   /* Start tab */
   [Ntf_EVENT_TML_COMMENT      ] = "NotifyNtfEventTimelineComment",
   [Ntf_EVENT_TML_FAV          ] = "NotifyNtfEventTimelineFav",
   [Ntf_EVENT_TML_SHARE        ] = "NotifyNtfEventTimelineShare",
   [Ntf_EVENT_TML_MENTION      ] = "NotifyNtfEventTimelineMention",
   [Ntf_EVENT_FOLLOWER         ] = "NotifyNtfEventFollower",
   /* System tab */
   /* Country tab */
   /* Institution tab */
   /* Center tab */
   /* Degree tab */
   /* Course tab */
   /* Assessment tab */
   [Ntf_EVENT_ASSIGNMENT       ] = "NotifyNtfEventAssignment",
   [Ntf_EVENT_SURVEY           ] = "NotifyNtfEventSurvey",
   [Ntf_EVENT_CALL_FOR_EXAM    ] = "NotifyNtfEventExamAnnouncement",
   /* Files tab */
   [Ntf_EVENT_DOCUMENT_FILE    ] = "NotifyNtfEventDocumentFile",
   [Ntf_EVENT_TEACHERS_FILE    ] = "NotifyNtfEventTeachersFile",
   [Ntf_EVENT_SHARED_FILE      ] = "NotifyNtfEventSharedFile",
   [Ntf_EVENT_MARKS_FILE       ] = "NotifyNtfEventMarksFile",
   /* Messages tab */
   [Ntf_EVENT_NOTICE           ] = "NotifyNtfEventNotice",
   [Ntf_EVENT_FORUM_POST_COURSE] = "NotifyNtfEventForumPostCourse",
   [Ntf_EVENT_FORUM_REPLY      ] = "NotifyNtfEventForumReply",
   [Ntf_EVENT_MESSAGE          ] = "NotifyNtfEventMessage",
   /* Users tab */
   [Ntf_EVENT_ENROLMENT_STD    ] = "NotifyNtfEventEnrolmentStudent",
   [Ntf_EVENT_ENROLMENT_NET    ] = "NotifyNtfEventEnrolmentNonEditingTeacher",
   [Ntf_EVENT_ENROLMENT_TCH    ] = "NotifyNtfEventEnrolmentTeacher",
   [Ntf_EVENT_ENROLMENT_REQUEST] = "NotifyNtfEventEnrolmentRequest",
   /* Analytics tab */
   /* Profile tab */
  };

// Email me about notification events
static const char *Ntf_ParamEmailMeAboutNotifyEvents[Ntf_NUM_NOTIFY_EVENTS] =
  {
   [Ntf_EVENT_UNKNOWN          ] = "EmailNtfEventUnknown",
   /* Start tab */
   [Ntf_EVENT_TML_COMMENT      ] = "EmailNtfEventTimelineComment",
   [Ntf_EVENT_TML_FAV          ] = "EmailNtfEventTimelineFav",
   [Ntf_EVENT_TML_SHARE        ] = "EmailNtfEventTimelineShare",
   [Ntf_EVENT_TML_MENTION      ] = "EmailNtfEventTimelineMention",
   [Ntf_EVENT_FOLLOWER         ] = "EmailNtfEventSocialFollower",
   /* System tab */
   /* Country tab */
   /* Institution tab */
   /* Center tab */
   /* Degree tab */
   /* Course tab */
   /* Assessment tab */
   [Ntf_EVENT_ASSIGNMENT       ] = "EmailNtfEventAssignment",
   [Ntf_EVENT_SURVEY           ] = "EmailNtfEventSurvey",
   [Ntf_EVENT_CALL_FOR_EXAM    ] = "EmailNtfEventExamAnnouncement",
   /* Files tab */
   [Ntf_EVENT_DOCUMENT_FILE    ] = "EmailNtfEventDocumentFile",
   [Ntf_EVENT_TEACHERS_FILE    ] = "EmailNtfEventTeachersFile",
   [Ntf_EVENT_SHARED_FILE      ] = "EmailNtfEventSharedFile",
   [Ntf_EVENT_MARKS_FILE       ] = "EmailNtfEventMarksFile",
   /* Messages tab */
   [Ntf_EVENT_NOTICE           ] = "EmailNtfEventNotice",
   [Ntf_EVENT_FORUM_POST_COURSE] = "EmailNtfEventForumPostCourse",
   [Ntf_EVENT_FORUM_REPLY      ] = "EmailNtfEventForumReply",
   [Ntf_EVENT_MESSAGE          ] = "EmailNtfEventMessage",
   /* Users tab */
   [Ntf_EVENT_ENROLMENT_STD    ] = "EmailNtfEventEnrolmentStudent",
   [Ntf_EVENT_ENROLMENT_NET    ] = "EmailNtfEventEnrolmentNonEditingTeacher",
   [Ntf_EVENT_ENROLMENT_TCH    ] = "EmailNtfEventEnrolmentTeacher",
   [Ntf_EVENT_ENROLMENT_REQUEST] = "EmailNtfEventEnrolmentRequest",
   /* Analytics tab */
   /* Profile tab */
  };

// Icons for notification events
static const char *Ntf_Icons[Ntf_NUM_NOTIFY_EVENTS] =
  {
   [Ntf_EVENT_UNKNOWN          ] = "question.svg",
   /* Start tab */
   [Ntf_EVENT_TML_COMMENT      ] = "comment-dots.svg",
   [Ntf_EVENT_TML_FAV          ] = "star.svg",
   [Ntf_EVENT_TML_SHARE        ] = "share-alt.svg",
   [Ntf_EVENT_TML_MENTION      ] = "at.svg",
   [Ntf_EVENT_FOLLOWER         ] = "user-plus.svg",
   /* System tab */
   /* Country tab */
   /* Institution tab */
   /* Center tab */
   /* Degree tab */
   /* Course tab */
   /* Assessment tab */
   [Ntf_EVENT_ASSIGNMENT       ] = "edit.svg",
   [Ntf_EVENT_SURVEY           ] = "poll.svg",
   [Ntf_EVENT_CALL_FOR_EXAM    ] = "bullhorn.svg",
   /* Files tab */
   [Ntf_EVENT_DOCUMENT_FILE    ] = "file.svg",
   [Ntf_EVENT_TEACHERS_FILE    ] = "file.svg",
   [Ntf_EVENT_SHARED_FILE      ] = "file.svg",
   [Ntf_EVENT_MARKS_FILE       ] = "clipboard-list.svg",
   /* Messages tab */
   [Ntf_EVENT_NOTICE           ] = "sticky-note.svg",
   [Ntf_EVENT_FORUM_POST_COURSE] = "comments.svg",
   [Ntf_EVENT_FORUM_REPLY      ] = "comments.svg",
   [Ntf_EVENT_MESSAGE          ] = "envelope.svg",
   /* Users tab */
   [Ntf_EVENT_ENROLMENT_STD    ] = "user.svg",
   [Ntf_EVENT_ENROLMENT_NET    ] = "user-tie.svg",
   [Ntf_EVENT_ENROLMENT_TCH    ] = "user-tie.svg",
   [Ntf_EVENT_ENROLMENT_REQUEST] = "hand-point-up.svg",
   /* Analytics tab */
   /* Profile tab */
  };

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Ntf_PutIconsNotif (__attribute__((unused)) void *Args);

static void Ntf_WriteFormAllNotifications (bool AllNotifications);
static bool Ntf_GetAllNotificationsFromForm (void);

static Act_Action_t Ntf_StartFormGoToAction (Ntf_NotifyEvent_t NotifyEvent,
                                             long CrsCod,struct Usr_Data *UsrDat,long Cod,
                                             const struct For_Forums *Forums);
static void Ntf_PutHiddenParamNotifyEvent (Ntf_NotifyEvent_t NotifyEvent);

static void Ntf_SendPendingNotifByEMailToOneUsr (struct Usr_Data *ToUsrDat,unsigned *NumNotif,unsigned *NumMails);
static void Ntf_GetNumNotifSent (long DegCod,long CrsCod,
                                 Ntf_NotifyEvent_t NotifyEvent,
                                 unsigned *NumEvents,unsigned *NumMails);
static void Ntf_UpdateNumNotifSent (long DegCod,long CrsCod,
                                    Ntf_NotifyEvent_t NotifyEvent,
                                    unsigned NumEvents,unsigned NumMails);

static void Ntf_GetParamsNotifyEvents (void);

/*****************************************************************************/
/*************************** Show my notifications ***************************/
/*****************************************************************************/

void Ntf_ShowMyNotifications (void)
  {
   extern const char *Hlp_START_Notifications;
   extern const char *Txt_Settings;
   extern const char *Txt_Domains;
   extern const char *Txt_Mark_all_NOTIFICATIONS_as_read;
   extern const char *Txt_Notifications;
   extern const char *Txt_Date;
   extern const char *Txt_Event;
   extern const char *Txt_Location;
   extern const char *Txt_MSG_From;
   extern const char *Txt_Email;
   extern const char *Txt_NOTIFY_EVENTS_SINGULAR[Ntf_NUM_NOTIFY_EVENTS];
   extern const char *Txt_Forum;
   extern const char *Txt_Course;
   extern const char *Txt_Degree;
   extern const char *Txt_Center;
   extern const char *Txt_Institution;
   extern const char *Txt_NOTIFICATION_STATUS[Ntf_NUM_STATUS_TXT];
   extern const char *Txt_You_have_no_notifications;
   extern const char *Txt_You_have_no_unread_notifications;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumNotif;
   unsigned NumNotifications;
   bool AllNotifications;
   Ntf_NotifyEvent_t NotifyEvent = (Ntf_NotifyEvent_t) 0;	// Initialized to avoid warning
   struct Usr_Data UsrDat;
   struct Hie_Hierarchy Hie;
   long Cod;
   struct For_Forums Forums;
   char ForumName[For_MAX_BYTES_FORUM_NAME + 1];
   time_t DateTimeUTC;	// Date-time of the event
   Ntf_Status_t Status;
   Ntf_StatusTxt_t StatusTxt;
   char SummaryStr[Ntf_MAX_BYTES_SUMMARY + 1];
   char *ContentStr;
   const char *ClassTxt;
   const char *ClassLink;
   const char *ClassAuthor;
   const char *ClassBg;
   bool PutLink;
   Act_Action_t Action = ActUnk;

   /***** Get my notifications from database *****/
   AllNotifications = Ntf_GetAllNotificationsFromForm ();
   NumNotifications = Ntf_DB_GetMyNotifications (&mysql_res,AllNotifications);

   /***** Contextual menu *****/
   Mnu_ContextMenuBegin ();
      Ntf_WriteFormAllNotifications (AllNotifications);	// Show all notifications
      if (NumNotifications)	// TODO: Show message only when I don't have notificacions at all
	 Lay_PutContextualLinkIconText (ActMrkNtfSee,NULL,
					NULL,NULL,
					"eye.svg",Ico_BLACK,
					Txt_Mark_all_NOTIFICATIONS_as_read,NULL);	// Mark notifications as read
      Lay_PutContextualLinkIconText (ActReqEdiSet,Ntf_NOTIFICATIONS_ID,
				     NULL,NULL,
				     "cog.svg",Ico_BLACK,
				     Txt_Settings,NULL);	// Change notification settings
      Lay_PutContextualLinkIconText (ActSeeMai,NULL,
				     NULL,NULL,
				     "envelope.svg",Ico_BLACK,
				     Txt_Domains,NULL);	// View allowed mail domains
   Mnu_ContextMenuEnd ();

   /***** Begin box *****/
   Box_BoxBegin (NULL,Txt_Notifications,
                 Ntf_PutIconsNotif,NULL,
                 Hlp_START_Notifications,Box_NOT_CLOSABLE);

      /***** List my notifications *****/
      if (NumNotifications)	// Notifications found
	{
	 /***** Initialize structure with user's data *****/
	 Usr_UsrDataConstructor (&UsrDat);

	 /***** Begin table *****/
	 HTM_TABLE_BeginWideMarginPadding (2);

	    /***** Heading *****/
	    HTM_TR_Begin (NULL);
	       HTM_TH_Span (Txt_Event   ,HTM_HEAD_LEFT  ,1,2,NULL);
	       HTM_TH      (Txt_MSG_From,HTM_HEAD_LEFT  );
	       HTM_TH      (Txt_Location,HTM_HEAD_LEFT  );
	       HTM_TH      (Txt_Date    ,HTM_HEAD_CENTER);
	       HTM_TH      (Txt_Email   ,HTM_HEAD_LEFT  );
	    HTM_TR_End ();

	    /***** List notifications one by one *****/
	    for (NumNotif = 0;
		 NumNotif < NumNotifications;
		 NumNotif++)
	      {
	       /***** Get next notification *****/
	       row = mysql_fetch_row (mysql_res);

	       /* Get event type (row[0]) */
	       NotifyEvent = Ntf_GetNotifyEventFromStr (row[0]);

	       /* Get (from) user code (row[1]) */
	       UsrDat.UsrCod = Str_ConvertStrCodToLongCod (row[1]);
	       Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat,	// Get user's data from database
							Usr_DONT_GET_PREFS,
							Usr_DONT_GET_ROLE_IN_CURRENT_CRS);

	       /* Get institution code (row[2]) */
	       Hie.Ins.InsCod = Str_ConvertStrCodToLongCod (row[2]);
	       Ins_GetDataOfInstitByCod (&Hie.Ins);

		/* Get center code (row[3]) */
	       Hie.Ctr.CtrCod = Str_ConvertStrCodToLongCod (row[3]);
	       Ctr_GetDataOfCenterByCod (&Hie.Ctr);

	       /* Get degree code (row[4]) */
	       Hie.Deg.DegCod = Str_ConvertStrCodToLongCod (row[4]);
	       Deg_GetDataOfDegreeByCod (&Hie.Deg);

	       /* Get course code (row[5]) */
	       Hie.Crs.CrsCod = Str_ConvertStrCodToLongCod (row[5]);
	       Crs_GetDataOfCourseByCod (&Hie.Crs);

	       /* Get message/post/... code (row[6]) */
	       Cod = Str_ConvertStrCodToLongCod (row[6]);

	       /* Get forum type of the post */
	       if (NotifyEvent == Ntf_EVENT_FORUM_POST_COURSE ||
		   NotifyEvent == Ntf_EVENT_FORUM_REPLY)
		 {
		  For_ResetForums (&Forums);
		  For_GetForumTypeAndLocationOfAPost (Cod,&Forums.Forum);
		  For_SetForumName (&Forums.Forum,
				    ForumName,Gbl.Prefs.Language,false);	// Set forum name in recipient's language
		 }

	       /* Get time of the event (row[7]) */
	       DateTimeUTC = Dat_GetUNIXTimeFromStr (row[7]);

	       /* Get status (row[8]) */
	       if (sscanf (row[8],"%u",&Status) != 1)
		  Err_WrongStatusExit ();
	       StatusTxt = Ntf_GetStatusTxtFromStatusBits (Status);

	       if (Status & Ntf_STATUS_BIT_REMOVED)	// The source of the notification was removed
		 {
		  ClassTxt    = "MSG_TIT_REM";
		  ClassLink   = "BT_LINK MSG_TIT_REM";
		  ClassAuthor = "MSG_AUT_LIGHT";
		  ClassBg     = "MSG_BG_REM";
		  PutLink = false;
		 }
	       else if (Status & Ntf_STATUS_BIT_READ)	// I have already seen the source of the notification
		 {
		  ClassTxt    = "MSG_TIT";
		  ClassLink   = "LT BT_LINK MSG_TIT";
		  ClassAuthor = "MSG_AUT";
		  ClassBg     = "MSG_BG";
		  PutLink = true;
		 }
	       else					// I have not seen the source of the notification
		 {
		  ClassTxt    = "MSG_TIT_NEW";
		  ClassLink   = "LT BT_LINK MSG_TIT_NEW";
		  ClassAuthor = "MSG_AUT_NEW";
		  ClassBg     = "MSG_BG_NEW";
		  PutLink = true;
		 }

	       /***** Write row for this notification *****/
	       /* Write event icon */
	       HTM_TR_Begin (NULL);

		  HTM_TD_Begin ("class=\"LT %s_%s\" style=\"width:25px;\"",
		                ClassBg,The_GetSuffix ());
		     if (PutLink)
		       {
			Action = Ntf_StartFormGoToAction (NotifyEvent,Hie.Crs.CrsCod,&UsrDat,Cod,&Forums);
			PutLink = Gbl.Form.Inside;
		       }

		     if (PutLink)
		       {
			Ico_PutIconLink (Ntf_Icons[NotifyEvent],Ico_BLACK,Action);
			Frm_EndForm ();
		       }
		     else
			Ico_PutIconOff (Ntf_Icons[NotifyEvent],Ico_BLACK,
					Txt_NOTIFY_EVENTS_SINGULAR[NotifyEvent]);
		  HTM_TD_End ();

		  /* Write event type */
		  HTM_TD_Begin ("class=\"LT %s_%s\"",ClassBg,The_GetSuffix ());
		     if (PutLink)
		       {
			Action = Ntf_StartFormGoToAction (NotifyEvent,Hie.Crs.CrsCod,&UsrDat,Cod,&Forums);
			PutLink = Gbl.Form.Inside;

			   HTM_BUTTON_Submit_Begin (Txt_NOTIFY_EVENTS_SINGULAR[NotifyEvent],
			                            "class=\"LT %s_%s\"",
			                            ClassLink,The_GetSuffix ());
			      HTM_Txt (Txt_NOTIFY_EVENTS_SINGULAR[NotifyEvent]);
			   HTM_BUTTON_End ();
			Frm_EndForm ();
		       }
		     else
		       {
			HTM_SPAN_Begin ("class=\"%s\"",ClassTxt);
			   HTM_Txt (Txt_NOTIFY_EVENTS_SINGULAR[NotifyEvent]);
			HTM_SPAN_End ();
		       }
		  HTM_TD_End ();

		  /* Write user (from) */
		  HTM_TD_Begin ("class=\"LT %s_%s %s_%s\"",
		                ClassAuthor,The_GetSuffix (),
		                ClassBg,The_GetSuffix ());
		     Msg_WriteMsgAuthor (&UsrDat,true);
		  HTM_TD_End ();

		  /* Write location */
		  HTM_TD_Begin ("class=\"LT %s_%s\"",
		                ClassBg,The_GetSuffix ());
		     if (NotifyEvent == Ntf_EVENT_FORUM_POST_COURSE ||
			 NotifyEvent == Ntf_EVENT_FORUM_REPLY)
		       {
			if (PutLink)
			  {
			   Action = Ntf_StartFormGoToAction (NotifyEvent,Hie.Crs.CrsCod,&UsrDat,Cod,&Forums);
			   PutLink = Gbl.Form.Inside;
		          }

			if (PutLink)
			   HTM_BUTTON_Submit_Begin (Txt_NOTIFY_EVENTS_SINGULAR[NotifyEvent],
			                            "class=\"LT %s_%s\"",
			                            ClassLink,The_GetSuffix ());
			else
			   HTM_SPAN_Begin ("class=\"%s_%s\"",
			                   ClassTxt,The_GetSuffix ());
			HTM_TxtF ("%s:&nbsp;%s",Txt_Forum,ForumName);
			if (PutLink)
			  {
			   HTM_BUTTON_End ();
			   Frm_EndForm ();
			  }
			else
			   HTM_SPAN_End ();
		       }
		     else
		       {
			if (PutLink)
			  {
			   Action = Ntf_StartFormGoToAction (NotifyEvent,Hie.Crs.CrsCod,&UsrDat,Cod,&Forums);
			   PutLink = Gbl.Form.Inside;
		          }

			if (PutLink)
			   HTM_BUTTON_Submit_Begin (Txt_NOTIFY_EVENTS_SINGULAR[NotifyEvent],
			                            "class=\"LT %s_%s\"",
			                            ClassLink,The_GetSuffix ());
			else
			   HTM_SPAN_Begin ("class=\"%s_%s\"",
			                   ClassTxt,The_GetSuffix ());

			if (Hie.Crs.CrsCod > 0)
			   HTM_TxtF ("%s:&nbsp;%s",Txt_Course,Hie.Crs.ShrtName);
			else if (Hie.Deg.DegCod > 0)
			   HTM_TxtF ("%s:&nbsp;%s",Txt_Degree,Hie.Deg.ShrtName);
			else if (Hie.Ctr.CtrCod > 0)
			   HTM_TxtF ("%s:&nbsp;%s",Txt_Center,Hie.Ctr.ShrtName);
			else if (Hie.Ins.InsCod > 0)
			   HTM_TxtF ("%s:&nbsp;%s",Txt_Institution,Hie.Ins.ShrtName);
			else
			   HTM_Hyphen ();

			if (PutLink)
			  {
			   HTM_BUTTON_End ();
			   Frm_EndForm ();
			  }
			else
			   HTM_SPAN_End ();
		       }
		  HTM_TD_End ();

		  /* Write date and time */
		  Msg_WriteMsgDate (DateTimeUTC,ClassTxt,ClassBg);

		  /* Write status (sent by email / pending to be sent by email) */
		  HTM_TD_Begin ("class=\"LT %s_%s %s_%s\"",
		                ClassTxt,The_GetSuffix (),
		                ClassBg,The_GetSuffix ());
		     HTM_Txt (Txt_NOTIFICATION_STATUS[StatusTxt]);
		  HTM_TD_End ();

	       HTM_TR_End ();

	       /***** Write content of the event *****/
	       if (PutLink)
		 {
		  ContentStr = NULL;

		  Ntf_GetNotifSummaryAndContent (SummaryStr,&ContentStr,NotifyEvent,
						 Cod,Hie.Crs.CrsCod,Gbl.Usrs.Me.UsrDat.UsrCod,
						 false);
		  HTM_TR_Begin (NULL);

		     HTM_TD_Begin ("colspan=\"2\"");
		     HTM_TD_End ();

		     HTM_TD_Begin ("colspan=\"4\" class=\"LT DAT_%s\""
			           " style=\"padding-bottom:12px;\"",
			           The_GetSuffix ());
			HTM_Txt (SummaryStr);
		     HTM_TD_End ();

		  HTM_TR_End ();
		 }
	      }

	 /***** End table *****/
	 HTM_TABLE_End ();

	 /***** Free memory used for user's data *****/
	 Usr_UsrDataDestructor (&UsrDat);
	}
      else
	 Ale_ShowAlert (Ale_INFO,AllNotifications ? Txt_You_have_no_notifications :
						    Txt_You_have_no_unread_notifications);

   /***** End box *****/
   Box_BoxEnd ();

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   /***** Reset to 0 the number of new notifications *****/
   Ntf_DB_UpdateMyLastAccessToNotifications ();
  }

/*****************************************************************************/
/****************** Put contextual icons in notifications ********************/
/*****************************************************************************/

static void Ntf_PutIconsNotif (__attribute__((unused)) void *Args)
  {
   /***** Put icon to show a figure *****/
   Fig_PutIconToShowFigure (Fig_NOTIFY_EVENTS);
  }

/*****************************************************************************/
/********** Write a form to select whether show all notifications ************/
/*****************************************************************************/

static void Ntf_WriteFormAllNotifications (bool AllNotifications)
  {
   extern const char *Txt_Show_all_notifications;
   extern const char *Txt_Show_all_NOTIFICATIONS;

   Lay_PutContextualCheckbox (ActSeeNtf,
                              NULL,
                              "All",
                              AllNotifications,false,
                              Txt_Show_all_notifications,
                              Txt_Show_all_NOTIFICATIONS);
  }

/*****************************************************************************/
/************* Get whether to show all notifications from form ***************/
/*****************************************************************************/

static bool Ntf_GetAllNotificationsFromForm (void)
  {
   return Par_GetParToBool ("All");
  }

/*****************************************************************************/
/*********** Put form to go to an action depending on the event **************/
/*****************************************************************************/

static Act_Action_t Ntf_StartFormGoToAction (Ntf_NotifyEvent_t NotifyEvent,
                                             long CrsCod,struct Usr_Data *UsrDat,long Cod,
                                             const struct For_Forums *Forums)
  {
   extern const Act_Action_t For_ActionsSeeFor[For_NUM_TYPES_FORUM];
   struct Brw_FileMetadata FileMetadata;
   long InsCod = -1L;
   long CtrCod = -1L;
   long DegCod = -1L;
   long GrpCod = -1L;
   Act_Action_t Action = ActUnk;
   char *Anchor = NULL;

   /***** Parameters depending on the type of event *****/
   switch (NotifyEvent)
     {
      case Ntf_EVENT_CALL_FOR_EXAM:
	 /* Build anchor string */
	 Frm_SetAnchorStr (Cod,&Anchor);

	 /* Begin form */
	 Action = ActSeeOneCfe;
         Frm_BeginFormAnchor (ActSeeOneCfe,Anchor);
            Cfe_PutHiddenParamExaCod (Cod);

	 /* Free anchor string */
	 Frm_FreeAnchorStr (Anchor);
	 break;
      case Ntf_EVENT_DOCUMENT_FILE:
      case Ntf_EVENT_TEACHERS_FILE:
      case Ntf_EVENT_SHARED_FILE:
      case Ntf_EVENT_MARKS_FILE:
	 FileMetadata.FilCod = Cod;
         if (FileMetadata.FilCod > 0)
            Brw_GetFileMetadataByCod (&FileMetadata);
	 if (FileMetadata.FilCod > 0)
	   {
	    Brw_GetCrsGrpFromFileMetadata (FileMetadata.FileBrowser,FileMetadata.Cod,
					   &InsCod,&CtrCod,&DegCod,&CrsCod,&GrpCod);
	    switch (NotifyEvent)
	      {
	       case Ntf_EVENT_DOCUMENT_FILE:
		  Action = (GrpCod > 0 ? ActReqDatSeeDocGrp :
			   (CrsCod > 0 ? ActReqDatSeeDocCrs :
			   (DegCod > 0 ? ActReqDatSeeDocDeg :
			   (CtrCod > 0 ? ActReqDatSeeDocCtr :
					 ActReqDatSeeDocIns))));
		  break;
	       case Ntf_EVENT_TEACHERS_FILE:
		  Action = (GrpCod > 0 ? ActReqDatTchGrp :
					 ActReqDatTchCrs);
		  break;
	       case Ntf_EVENT_SHARED_FILE:
		  Action = (GrpCod > 0 ? ActReqDatShaGrp :
			   (CrsCod > 0 ? ActReqDatShaCrs :
			   (DegCod > 0 ? ActReqDatShaDeg :
			   (CtrCod > 0 ? ActReqDatShaCtr :
					 ActReqDatShaIns))));
		  break;
	       case Ntf_EVENT_MARKS_FILE:
		  Action = (GrpCod > 0 ? ActReqDatSeeMrkGrp :
					 ActReqDatSeeMrkCrs);
		  break;
	       default:	// Not aplicable here
		  break;
	      }
	    Frm_BeginForm (Action);
	       if (GrpCod > 0)
		  Grp_PutParamGrpCod (&GrpCod);
	       Brw_PutHiddenParamFilCod (FileMetadata.FilCod);
	   }
	 break;
      case Ntf_EVENT_TML_COMMENT:
      case Ntf_EVENT_TML_FAV:
      case Ntf_EVENT_TML_SHARE:
      case Ntf_EVENT_TML_MENTION:
	 // Cod is the code of the social publishing
	 Action = ActSeeGblTL;
         Frm_BeginForm (Action);
	    TmlPub_PutHiddenParamPubCod (Cod);
	    Usr_PutParamUsrCodEncrypted (UsrDat->EnUsrCod);
	    Ntf_PutHiddenParamNotifyEvent (NotifyEvent);
	 break;
      case Ntf_EVENT_FOLLOWER:
         if (UsrDat->EnUsrCod[0])	// User's code found ==>
					// go to user's public profile
           {
            Action = ActSeeOthPubPrf;
            Frm_BeginForm (Action);
	       /* Put param to go to follower's profile */
	       Usr_PutParamUsrCodEncrypted (UsrDat->EnUsrCod);
           }
         else	// No user's code found ==> go to see my followers
           {
            Action = ActSeeFlr;
            Frm_BeginForm (Action);
           }
	 break;
      case Ntf_EVENT_FORUM_POST_COURSE:
      case Ntf_EVENT_FORUM_REPLY:
	 Action = For_ActionsSeeFor[Forums->Forum.Type];
	 Frm_BeginForm (Action);
	    For_PutAllHiddenParamsForum (1,	// Page of threads = first
					 1,	// Page of posts   = first
					 Forums->ForumSet,
					 Forums->ThreadsOrder,
					 Forums->Forum.Location,
					 Forums->Thread.Selected,
					 -1L);
	 break;
      case Ntf_EVENT_NOTICE:
	 Action = ActSeeOneNot;
         Frm_BeginForm (Action);
	    Not_PutHiddenParamNotCod (Cod);
	 break;
      case Ntf_EVENT_MESSAGE:
	 Action = ActExpRcvMsg;
         Frm_BeginForm (Action);
	    Msg_PutHiddenParamMsgCod (Cod);
	 break;
      default:
	 Action = Ntf_DefaultActions[NotifyEvent];
         Frm_BeginForm (Action);
	 break;
     }

   /***** Parameter to go to another course/degree/center/institution *****/
   if (Gbl.Form.Inside)
     {
      if (CrsCod > 0)					// Course specified
	{
	 if (CrsCod != Gbl.Hierarchy.Crs.CrsCod)	// Not the current course
	    Crs_PutParamCrsCod (CrsCod);		// Go to another course
	}
      else if (DegCod > 0)				// Degree specified
	{
	 if (DegCod != Gbl.Hierarchy.Deg.DegCod)	// Not the current degree
	    Deg_PutParamDegCod (DegCod);		// Go to another degree
	}
      else if (CtrCod > 0)				// Center specified
	{
	 if (CtrCod != Gbl.Hierarchy.Ctr.CtrCod)	// Not the current center
	    Ctr_PutParamCtrCod (CtrCod);		// Go to another center
	}
      else if (InsCod > 0)				// Institution specified
	{
	 if (InsCod != Gbl.Hierarchy.Ins.InsCod)	// Not the current institution
	    Ins_PutParamInsCod (InsCod);		// Go to another institution
	}
     }

   return Action;
  }

/*****************************************************************************/
/******************* Get parameter with notify event type ********************/
/*****************************************************************************/

static void Ntf_PutHiddenParamNotifyEvent (Ntf_NotifyEvent_t NotifyEvent)
  {
   Par_PutHiddenParamUnsigned (NULL,"NotifyEvent",(unsigned) NotifyEvent);
  }

/*****************************************************************************/
/******************* Get parameter with notify event type ********************/
/*****************************************************************************/

Ntf_NotifyEvent_t Ntf_GetParamNotifyEvent (void)
  {
   return (Ntf_NotifyEvent_t)
	  Par_GetParToUnsignedLong ("NotifyEvent",
                                    0,
                                    Ntf_NUM_NOTIFY_EVENTS - 1,
                                    (unsigned long) Ntf_EVENT_UNKNOWN);
  }

/*****************************************************************************/
/****************** Set StatusTxt depending on status bits *******************/
/*****************************************************************************/
//   Ntf_STATUS_NO_EMAIL        = 0,	// --0 !(Status & Ntf_STATUS_BIT_EMAIL)
//   Ntf_STATUS_EMAIL_PENDING   = 1,	// 001  (Status & Ntf_STATUS_BIT_EMAIL) && !(Status & Ntf_STATUS_BIT_SENT) && !(Status & Ntf_STATUS_BIT_READ)
//   Ntf_STATUS_EMAIL_CANCELLED = 2,	// 101  (Status & Ntf_STATUS_BIT_EMAIL) && !(Status & Ntf_STATUS_BIT_SENT) &&  (Status & Ntf_STATUS_BIT_READ)
//   Ntf_STATUS_EMAIL_SENT      = 3,	// -11  (Status & Ntf_STATUS_BIT_EMAIL) &&  (Status & Ntf_STATUS_BIT_SENT)

Ntf_StatusTxt_t Ntf_GetStatusTxtFromStatusBits (Ntf_Status_t Status)
  {
   if (!(Status & Ntf_STATUS_BIT_EMAIL))
      return Ntf_STATUS_NO_EMAIL;
   if ( (Status & Ntf_STATUS_BIT_SENT))
      return Ntf_STATUS_EMAIL_SENT;
   if ( (Status & (Ntf_STATUS_BIT_READ | Ntf_STATUS_BIT_REMOVED)))
      return Ntf_STATUS_EMAIL_CANCELLED;
   return Ntf_STATUS_EMAIL_PENDING;
  }

/*****************************************************************************/
/******************* Get notification summary and content ********************/
/*****************************************************************************/

void Ntf_GetNotifSummaryAndContent (char SummaryStr[Ntf_MAX_BYTES_SUMMARY + 1],
                                    char **ContentStr,
                                    Ntf_NotifyEvent_t NotifyEvent,
                                    long Cod,long CrsCod,long UsrCod,
                                    bool GetContent)
  {
   SummaryStr[0] = '\0';

   switch (NotifyEvent)
     {
      case Ntf_EVENT_UNKNOWN:
         break;
      case Ntf_EVENT_DOCUMENT_FILE:
      case Ntf_EVENT_TEACHERS_FILE:
      case Ntf_EVENT_SHARED_FILE:
	 Brw_GetSummaryAndContentOfFile (SummaryStr,ContentStr,Cod,GetContent);
         break;
      case Ntf_EVENT_ASSIGNMENT:
         Asg_GetNotifAssignment (SummaryStr,ContentStr,Cod,GetContent);
         break;
      case Ntf_EVENT_CALL_FOR_EXAM:
         Cfe_GetSummaryAndContentCallForExam (SummaryStr,ContentStr,Cod,GetContent);
         break;
      case Ntf_EVENT_MARKS_FILE:
         Mrk_GetNotifMyMarks (SummaryStr,ContentStr,Cod,UsrCod,GetContent);
         break;
      case Ntf_EVENT_ENROLMENT_STD:
      case Ntf_EVENT_ENROLMENT_NET:
      case Ntf_EVENT_ENROLMENT_TCH:
	 Enr_GetNotifEnrolment (SummaryStr,CrsCod,UsrCod);
         break;
      case Ntf_EVENT_ENROLMENT_REQUEST:
	 Enr_GetNotifEnrolmentRequest (SummaryStr,ContentStr,Cod,GetContent);
         break;
      case Ntf_EVENT_TML_COMMENT:
      case Ntf_EVENT_TML_FAV:
      case Ntf_EVENT_TML_SHARE:
      case Ntf_EVENT_TML_MENTION:
	 // Cod is the code of the social publishing
	 TmlNtf_GetNotifPublication (SummaryStr,ContentStr,Cod,GetContent);
         break;
      case Ntf_EVENT_FOLLOWER:
         Fol_GetNotifFollower (SummaryStr,ContentStr);
         break;
      case Ntf_EVENT_FORUM_POST_COURSE:
      case Ntf_EVENT_FORUM_REPLY:
         For_GetSummaryAndContentForumPst (SummaryStr,ContentStr,Cod,GetContent);
         break;
      case Ntf_EVENT_NOTICE:
         Not_GetSummaryAndContentNotice (SummaryStr,ContentStr,Cod,GetContent);
         break;
      case Ntf_EVENT_MESSAGE:
         Msg_GetNotifMessage (SummaryStr,ContentStr,Cod,GetContent);
         if (Gbl.WebService.IsWebService)
            /* Set the message as open by me, because I can read it in an extern application */
            Msg_DB_SetRcvMsgAsOpen (Cod,UsrCod);
         break;
      case Ntf_EVENT_SURVEY:
         Svy_GetNotifSurvey (SummaryStr,ContentStr,Cod,GetContent);
         break;
     }
  }

/*****************************************************************************/
/*********** Mark possible notifications of one file as removed **************/
/*****************************************************************************/

void Ntf_MarkNotifOneFileAsRemoved (const char *Path)
  {
   extern const Brw_FileBrowser_t Brw_DB_FileBrowserForDB_files[Brw_NUM_TYPES_FILE_BROWSER];
   static const Ntf_NotifyEvent_t NotifyEvent[Brw_NUM_TYPES_FILE_BROWSER] =
     {
      [Brw_ADMI_DOC_CRS] = Ntf_EVENT_DOCUMENT_FILE,
      [Brw_ADMI_DOC_GRP] = Ntf_EVENT_DOCUMENT_FILE,
      [Brw_ADMI_TCH_CRS] = Ntf_EVENT_TEACHERS_FILE,
      [Brw_ADMI_TCH_GRP] = Ntf_EVENT_TEACHERS_FILE,
      [Brw_ADMI_SHR_CRS] = Ntf_EVENT_SHARED_FILE,
      [Brw_ADMI_SHR_GRP] = Ntf_EVENT_SHARED_FILE,
      [Brw_ADMI_MRK_CRS] = Ntf_EVENT_MARKS_FILE,
      [Brw_ADMI_MRK_GRP] = Ntf_EVENT_MARKS_FILE,
     };
   Brw_FileBrowser_t FileBrowser = Brw_DB_FileBrowserForDB_files[Gbl.FileBrowser.Type];
   long FilCod;

   if (NotifyEvent[FileBrowser])
     {
      /***** Get file code *****/
      FilCod = Brw_DB_GetFilCodByPath (Path,false);	// Any file, public or not
      if (FilCod > 0)
	 /***** Set notification as removed *****/
	 Ntf_DB_MarkNotifAsRemoved (NotifyEvent[FileBrowser],FilCod);
     }
  }

/*****************************************************************************/
/*** Mark possible notifications involving children of a folder as removed ***/
/*****************************************************************************/

void Ntf_MarkNotifChildrenOfFolderAsRemoved (const char *Path)
  {
   extern const Brw_FileBrowser_t Brw_DB_FileBrowserForDB_files[Brw_NUM_TYPES_FILE_BROWSER];
   static const Ntf_NotifyEvent_t NotifyEvent[Brw_NUM_TYPES_FILE_BROWSER] =
     {
      [Brw_ADMI_DOC_CRS] = Ntf_EVENT_DOCUMENT_FILE,
      [Brw_ADMI_DOC_GRP] = Ntf_EVENT_DOCUMENT_FILE,
      [Brw_ADMI_TCH_CRS] = Ntf_EVENT_TEACHERS_FILE,
      [Brw_ADMI_TCH_GRP] = Ntf_EVENT_TEACHERS_FILE,
      [Brw_ADMI_SHR_CRS] = Ntf_EVENT_SHARED_FILE,
      [Brw_ADMI_SHR_GRP] = Ntf_EVENT_SHARED_FILE,
      [Brw_ADMI_MRK_CRS] = Ntf_EVENT_MARKS_FILE,
      [Brw_ADMI_MRK_GRP] = Ntf_EVENT_MARKS_FILE,
     };
   Brw_FileBrowser_t FileBrowser = Brw_DB_FileBrowserForDB_files[Gbl.FileBrowser.Type];

   if (NotifyEvent[FileBrowser])
      /***** Set notification as removed *****/
      Ntf_DB_MarkNotifChildrenOfFolderAsRemoved (NotifyEvent[FileBrowser],FileBrowser,
						 Brw_GetCodForFileBrowser (),
						 Path);
  }

/*****************************************************************************/
/********** Get a list with user's codes of all users to be notified *********/
/********** about an event, and notify them                          *********/
/*****************************************************************************/
// Return the number of users notified by email

unsigned Ntf_StoreNotifyEventsToAllUsrs (Ntf_NotifyEvent_t NotifyEvent,long Cod)
  {
   static unsigned (*GetUsrsBrowser[Brw_NUM_TYPES_FILE_BROWSER]) (MYSQL_RES **mysql_res) =
     {
      // Notify all users in course except me
      [Brw_ADMI_DOC_CRS] = Enr_DB_GetUsrsFromCurrentCrsExceptMe,
      [Brw_ADMI_SHR_CRS] = Enr_DB_GetUsrsFromCurrentCrsExceptMe,
      [Brw_ADMI_MRK_CRS] = Enr_DB_GetUsrsFromCurrentCrsExceptMe,

      // Notify all teachers in course except me
      [Brw_ADMI_TCH_CRS] = Enr_DB_GetTchsFromCurrentCrsExceptMe,

      // Notify all users in group except me
      [Brw_ADMI_DOC_GRP] = Grp_DB_GetUsrsFromCurrentGrpExceptMe,
      [Brw_ADMI_SHR_GRP] = Grp_DB_GetUsrsFromCurrentGrpExceptMe,
      [Brw_ADMI_MRK_GRP] = Grp_DB_GetUsrsFromCurrentGrpExceptMe,

      // Notify all teachers in group except me
      [Brw_ADMI_TCH_GRP] = Grp_DB_GetTchsFromCurrentGrpExceptMe,
     };
   static unsigned (*GetUsrsForum[For_NUM_TYPES_FORUM]) (MYSQL_RES **mysql_res) =
     {
      [For_FORUM_COURSE_USRS] = Enr_DB_GetUsrsFromCurrentCrsExceptMe,
      [For_FORUM_COURSE_TCHS] = Enr_DB_GetTchsFromCurrentCrsExceptMe,
     };
   MYSQL_RES *mysql_res;
   unsigned NumUsrs = 0;	// Initialized to avoid warning
   unsigned NumUsr;
   struct Usr_Data UsrDat;
   struct For_Forum ForumSelected;
   long InsCod;
   long CtrCod;
   long DegCod;
   long CrsCod;
   unsigned NumUsrsToBeNotifiedByEMail = 0;
   unsigned NotifyEventMask = (1 << NotifyEvent);

   /***** Get users who want to be notified from database ******/
   switch (NotifyEvent)
     {
      case Ntf_EVENT_UNKNOWN:	// This function should not be called in this case
         return 0;
      case Ntf_EVENT_DOCUMENT_FILE:
      case Ntf_EVENT_TEACHERS_FILE:
      case Ntf_EVENT_SHARED_FILE:
      case Ntf_EVENT_MARKS_FILE:
	 if (GetUsrsBrowser[Gbl.FileBrowser.Type])
            NumUsrs = GetUsrsBrowser[Gbl.FileBrowser.Type] (&mysql_res);
	 else
	    return 0;
         break;
      case Ntf_EVENT_ASSIGNMENT:
         NumUsrs = Asg_DB_GetUsrsFromAssignmentExceptMe (&mysql_res,Cod);
         break;
      case Ntf_EVENT_CALL_FOR_EXAM:
      case Ntf_EVENT_NOTICE:
         NumUsrs = Enr_DB_GetUsrsFromCurrentCrsExceptMe (&mysql_res);
         break;
      case Ntf_EVENT_ENROLMENT_STD:	// This function should not be called in this case
      case Ntf_EVENT_ENROLMENT_NET:	// This function should not be called in this case
      case Ntf_EVENT_ENROLMENT_TCH:	// This function should not be called in this case
         return 0;
      case Ntf_EVENT_ENROLMENT_REQUEST:
	 if (Enr_GetNumUsrsInCrss (HieLvl_CRS,Gbl.Hierarchy.Crs.CrsCod,
				   1 << Rol_TCH))
	    // If this course has teachers ==> send notification to teachers
            NumUsrs = Enr_DB_GetTchsFromCurrentCrsExceptMe (&mysql_res);
	 else	// Course without teachers
	    // If this course has no teachers
	    // and I want to be a teacher (checked before calling this function
	    // to not send requests to be a student to admins)
	    // ==> send notification to administrators or superusers
	    NumUsrs = Adm_DB_GetAdmsCurrentScopeExceptMe (&mysql_res);
         break;
      case Ntf_EVENT_TML_COMMENT:	// New comment to one of my social notes or comments
         // Cod is the code of the social publishing
	 NumUsrs = Tml_DB_GetPublishersInNoteExceptMe (&mysql_res,Cod);
         break;
      case Ntf_EVENT_TML_FAV:		// New favourite to one of my social notes or comments
      case Ntf_EVENT_TML_SHARE:		// New sharing of one of my social notes
      case Ntf_EVENT_TML_MENTION:
      case Ntf_EVENT_FOLLOWER:
	 // This function should not be called in these cases
         return 0;
      case Ntf_EVENT_FORUM_POST_COURSE:
	 // Check if forum is for users or for all users in the course
	 For_GetForumTypeAndLocationOfAPost (Cod,&ForumSelected);

	 if (GetUsrsForum[ForumSelected.Type])
            NumUsrs = GetUsrsForum[ForumSelected.Type] (&mysql_res);
	 else
	    return 0;
         break;
      case Ntf_EVENT_FORUM_REPLY:
         // Cod is the code of the post
         NumUsrs = For_DB_GetPublishersInThreadExceptMe (&mysql_res,Cod);
         break;
      case Ntf_EVENT_MESSAGE:		// This function should not be called in this case
	 return 0;
      case Ntf_EVENT_SURVEY:	// Only surveys for a course are notified, not surveys for a degree or global
         NumUsrs = Svy_DB_GetUsrsFromSurveyExceptMe (&mysql_res,Cod);
         break;
     }

   if (NotifyEvent == Ntf_EVENT_FORUM_POST_COURSE ||
       NotifyEvent == Ntf_EVENT_FORUM_REPLY)
     {
      InsCod = CtrCod = DegCod = CrsCod = -1L;
      switch (ForumSelected.Type)
        {
	 case For_FORUM_INSTIT_USRS:
	 case For_FORUM_INSTIT_TCHS:
            InsCod = ForumSelected.Location;
            break;
	 case For_FORUM_CENTER_USRS:
	 case For_FORUM_CENTER_TCHS:
            CtrCod = ForumSelected.Location;
            break;
	 case For_FORUM_DEGREE_USRS:
	 case For_FORUM_DEGREE_TCHS:
            DegCod = ForumSelected.Location;
            break;
	 case For_FORUM_COURSE_USRS:
	 case For_FORUM_COURSE_TCHS:
            CrsCod = ForumSelected.Location;
            break;
	 default:
	    break;
        }
     }
   else
     {
      InsCod = Gbl.Hierarchy.Ins.InsCod;
      CtrCod = Gbl.Hierarchy.Ctr.CtrCod;
      DegCod = Gbl.Hierarchy.Deg.DegCod;
      CrsCod = Gbl.Hierarchy.Crs.CrsCod;
     }

   if (NumUsrs) // Users found
     {
      /***** Initialize structure with user's data *****/
      Usr_UsrDataConstructor (&UsrDat);

      /***** Notify the users one by one *****/
      for (NumUsr = 0;
	   NumUsr < NumUsrs;
	   NumUsr++)
	{
         /* Get next user */
         UsrDat.UsrCod = DB_GetNextCode (mysql_res);

	 if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat,	// Get user's data from database
	                                              Usr_DONT_GET_PREFS,
	                                              Usr_DONT_GET_ROLE_IN_CURRENT_CRS))
            if ((UsrDat.NtfEvents.CreateNotif & NotifyEventMask))	// Create notification
              {
	       if ((UsrDat.NtfEvents.SendEmail & NotifyEventMask))	// Send notification by email
		 {
		  Ntf_DB_StoreNotifyEventToUsr (NotifyEvent,UsrDat.UsrCod,Cod,
						(Ntf_Status_t) Ntf_STATUS_BIT_EMAIL,
						InsCod,CtrCod,DegCod,CrsCod);
		  NumUsrsToBeNotifiedByEMail++;
		 }
	       else							// Don't send notification by email
		  Ntf_DB_StoreNotifyEventToUsr (NotifyEvent,UsrDat.UsrCod,Cod,
		                                (Ntf_Status_t) 0,
						InsCod,CtrCod,DegCod,CrsCod);
              }
        }

      /***** Free memory used for user's data *****/
      Usr_UsrDataDestructor (&UsrDat);
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return NumUsrsToBeNotifiedByEMail;
  }

/*****************************************************************************/
/***************** Send all pending notifications by email *******************/
/*****************************************************************************/

void Ntf_SendPendingNotifByEMailToAllUsrs (void)
  {
   MYSQL_RES *mysql_res;
   unsigned NumUsrs;
   unsigned NumUsr;
   struct Usr_Data UsrDat;
   unsigned NumNotif;
   unsigned NumTotalNotif = 0;
   unsigned NumMails;
   unsigned NumTotalMails = 0;

   /***** Get users who must be notified from database ******/
   if ((NumUsrs = Ntf_DB_GetUsrsWhoMustBeNotified (&mysql_res)))
     {
      /***** Initialize structure with user's data *****/
      Usr_UsrDataConstructor (&UsrDat);

      /***** Notify the users one by one *****/
      for (NumUsr = 0;
	   NumUsr < NumUsrs;
	   NumUsr++)
	{
         /* Get next user */
         UsrDat.UsrCod = DB_GetNextCode (mysql_res);

         /* Get user's data */
	 if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat,	// Get user's data from database
	                                              Usr_DONT_GET_PREFS,
	                                              Usr_DONT_GET_ROLE_IN_CURRENT_CRS))
           {
            /* Send one email to this user */
            Ntf_SendPendingNotifByEMailToOneUsr (&UsrDat,&NumNotif,&NumMails);
            NumTotalNotif += NumNotif;
            NumTotalMails += NumMails;
           }
        }

      /***** Free memory used for user's data *****/
      Usr_UsrDataDestructor (&UsrDat);
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   /***** Delete old notifications ******/
   Ntf_DB_RemoveOldNtfs ();
  }

/*****************************************************************************/
/************ Send pending notifications of one user by email ****************/
/*****************************************************************************/

static void Ntf_SendPendingNotifByEMailToOneUsr (struct Usr_Data *ToUsrDat,unsigned *NumNotif,unsigned *NumMails)
  {
   extern const char *Txt_NOTIFY_EVENTS_There_is_a_new_event_NO_HTML[1 + Lan_NUM_LANGUAGES];
   extern const char *Txt_NOTIFY_EVENTS_There_are_X_new_events_NO_HTML[1 + Lan_NUM_LANGUAGES];
   extern const char *Txt_NOTIFY_EVENTS_SINGULAR_NO_HTML[Ntf_NUM_NOTIFY_EVENTS][1 + Lan_NUM_LANGUAGES];
   extern const char *Txt_Course_NO_HTML[1 + Lan_NUM_LANGUAGES];
   extern const char *Txt_Forum_NO_HTML[1 + Lan_NUM_LANGUAGES];
   extern const char *Txt_MSG_From_NO_HTML[1 + Lan_NUM_LANGUAGES];
   extern const char *Txt_Go_to_NO_HTML[1 + Lan_NUM_LANGUAGES];
   extern const char *Txt_TAB_Messages_NO_HTML[1 + Lan_NUM_LANGUAGES];
   extern const char *Txt_Notifications_NO_HTML[1 + Lan_NUM_LANGUAGES];
   extern const char *Txt_If_you_no_longer_wish_to_receive_email_notifications_NO_HTML[1 + Lan_NUM_LANGUAGES];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumNtfs;
   unsigned NumNtf;
   Lan_Language_t ToUsrLanguage;
   struct Usr_Data FromUsrDat;
   Ntf_NotifyEvent_t NotifyEvent = (Ntf_NotifyEvent_t) 0;	// Initialized to avoid warning
   struct Hie_Hierarchy Hie;
   long Cod;
   struct For_Forum ForumSelected;
   char ForumName[For_MAX_BYTES_FORUM_NAME + 1];
   char FileNameMail[PATH_MAX + 1];
   FILE *FileMail;
   char Command[2048 +
		Cfg_MAX_BYTES_SMTP_PASSWORD +
		Cns_MAX_BYTES_EMAIL_ADDRESS +
		PATH_MAX]; // Command to execute for sending an email
   int ReturnCode;

   /***** Return 0 notifications and 0 mails when error *****/
   *NumNotif =
   *NumMails = 0;

   if (Mai_CheckIfUsrCanReceiveEmailNotif (ToUsrDat))
     {
      /***** Get pending notifications of this user from database ******/
      if ((NumNtfs = Ntf_DB_GetPendingNtfsToUsr (&mysql_res,ToUsrDat->UsrCod))) // Notifications found
	{
	 /***** If user has no language, set it to current language *****/
	 ToUsrLanguage = ToUsrDat->Prefs.Language;
	 if (ToUsrLanguage == Lan_LANGUAGE_UNKNOWN)
	    ToUsrLanguage = Gbl.Prefs.Language;

	 /***** Create temporary file for mail content *****/
	 Mai_CreateFileNameMail (FileNameMail,&FileMail);

	 /***** Welcome note *****/
	 Mai_WriteWelcomeNoteEMail (FileMail,ToUsrDat);
	 if (NumNtfs == 1)
	    fprintf (FileMail,Txt_NOTIFY_EVENTS_There_is_a_new_event_NO_HTML[ToUsrLanguage],
		     Cfg_PLATFORM_SHORT_NAME);
	 else
	    fprintf (FileMail,Txt_NOTIFY_EVENTS_There_are_X_new_events_NO_HTML[ToUsrLanguage],
		     (unsigned) NumNtfs,Cfg_PLATFORM_SHORT_NAME);
	 fprintf (FileMail,": \n");

	 /***** Initialize structure with origin user's data *****/
	 Usr_UsrDataConstructor (&FromUsrDat);

	 /***** Inform about the events one by one *****/
	 for (NumNtf = 0;
	      NumNtf < NumNtfs;
	      NumNtf++)
	   {
	    /* Get next event */
	    row = mysql_fetch_row (mysql_res);

	    /* Get event type (row[0]) */
	    NotifyEvent = Ntf_GetNotifyEventFromStr (row[0]);

	    /* Get origin user code (row[1]) */
	    FromUsrDat.UsrCod = Str_ConvertStrCodToLongCod (row[1]);
	    Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&FromUsrDat,	// Get origin user's data from database
	                                             Usr_DONT_GET_PREFS,
	                                             Usr_DONT_GET_ROLE_IN_CURRENT_CRS);

	    /* Get institution code (row[2]),
	           center code (row[3]),
	           degree code (row[4]),
	           course code (row[5]) */
	    Hie.Ins.InsCod = Str_ConvertStrCodToLongCod (row[2]);
	    Hie.Ctr.CtrCod = Str_ConvertStrCodToLongCod (row[3]);
	    Hie.Deg.DegCod = Str_ConvertStrCodToLongCod (row[4]);
	    Hie.Crs.CrsCod = Str_ConvertStrCodToLongCod (row[5]);

	    /* Get data of institution, center, degree and course */
	    Ins_GetDataOfInstitByCod (&Hie.Ins);
	    Ctr_GetDataOfCenterByCod (&Hie.Ctr);
	    Deg_GetDataOfDegreeByCod (&Hie.Deg);
	    Crs_GetDataOfCourseByCod (&Hie.Crs);

	    /* Get message/post/... code (row[6]) */
	    Cod = Str_ConvertStrCodToLongCod (row[6]);

	    /* Get forum type */
	    if (NotifyEvent == Ntf_EVENT_FORUM_POST_COURSE ||
		NotifyEvent == Ntf_EVENT_FORUM_REPLY)
	       For_GetForumTypeAndLocationOfAPost (Cod,&ForumSelected);

	    /* Information about the type of this event */
	    fprintf (FileMail,Txt_NOTIFY_EVENTS_SINGULAR_NO_HTML[NotifyEvent][ToUsrLanguage],
		     Cfg_PLATFORM_SHORT_NAME);
	    fprintf (FileMail,"\n");

	    /* Course/forum: */
	    switch (NotifyEvent)
	      {
	       case Ntf_EVENT_UNKNOWN:
	       case Ntf_EVENT_TML_COMMENT:
	       case Ntf_EVENT_TML_FAV:
	       case Ntf_EVENT_TML_SHARE:
	       case Ntf_EVENT_TML_MENTION:
	       case Ntf_EVENT_FOLLOWER:
		  break;
	       case Ntf_EVENT_DOCUMENT_FILE:
	       case Ntf_EVENT_TEACHERS_FILE:
	       case Ntf_EVENT_SHARED_FILE:
	       case Ntf_EVENT_ASSIGNMENT:
	       case Ntf_EVENT_CALL_FOR_EXAM:
	       case Ntf_EVENT_MARKS_FILE:
	       case Ntf_EVENT_ENROLMENT_STD:
	       case Ntf_EVENT_ENROLMENT_NET:
	       case Ntf_EVENT_ENROLMENT_TCH:
	       case Ntf_EVENT_ENROLMENT_REQUEST:
	       case Ntf_EVENT_NOTICE:
	       case Ntf_EVENT_MESSAGE:
	       case Ntf_EVENT_SURVEY:
		  if (Hie.Crs.CrsCod > 0)
		     fprintf (FileMail,"%s: %s\n",
			      Txt_Course_NO_HTML[ToUsrLanguage],
			      Hie.Crs.FullName);
		  break;
	       case Ntf_EVENT_FORUM_POST_COURSE:
	       case Ntf_EVENT_FORUM_REPLY:
		  For_SetForumName (&ForumSelected,
				    ForumName,ToUsrLanguage,false);	// Set forum name in recipient's language
		  fprintf (FileMail,"%s: %s\n",
			   Txt_Forum_NO_HTML[ToUsrLanguage],
			   ForumName);
		  break;
	      }
	    /* From: */
	    fprintf (FileMail,"%s: %s\n",
		     Txt_MSG_From_NO_HTML[ToUsrLanguage],
		     FromUsrDat.FullName);
	   }

	 /***** Free memory used for origin user's data *****/
	 Usr_UsrDataDestructor (&FromUsrDat);

	 /* Go to: */
	 fprintf (FileMail,"%s: %s/ > %s > %s\n",
		  Txt_Go_to_NO_HTML[ToUsrLanguage],
		  Cfg_URL_SWAD_CGI,
		  Txt_TAB_Messages_NO_HTML[ToUsrLanguage],
		  Txt_Notifications_NO_HTML[ToUsrLanguage]);

	 /* Disclaimer */
	 fprintf (FileMail,"\n%s\n",
		  Txt_If_you_no_longer_wish_to_receive_email_notifications_NO_HTML[ToUsrLanguage]);

	 /* Footer note */
	 Mai_WriteFootNoteEMail (FileMail,ToUsrLanguage);

	 fclose (FileMail);

	 /***** Call the command to send an email *****/
	 snprintf (Command,sizeof (Command),
	           "%s \"%s\" \"%s\" \"%s\" \"%s\" \"%s\" \"[%s] %s\" \"%s\"",
		   Cfg_COMMAND_SEND_AUTOMATIC_EMAIL,
		   Cfg_AUTOMATIC_EMAIL_SMTP_SERVER,
		   Cfg_AUTOMATIC_EMAIL_SMTP_PORT,
		   Cfg_AUTOMATIC_EMAIL_FROM,
		   Gbl.Config.SMTPPassword,
		   ToUsrDat->Email,
		   Cfg_PLATFORM_SHORT_NAME,
		   Txt_Notifications_NO_HTML[ToUsrLanguage],
		   FileNameMail);
	 ReturnCode = system (Command);
	 if (ReturnCode == -1)
	    Err_ShowErrorAndExit ("Error when running script to send email.");

	 /***** Remove temporary file *****/
	 unlink (FileNameMail);

	 /***** Update number of notifications, number of mails and statistics *****/
	 ReturnCode = WEXITSTATUS(ReturnCode);
	 if (ReturnCode == 0)	// Message sent successfully
	   {
	    *NumNotif = NumNtfs;
	    *NumMails = 1;

	    /* Update statistics about notifications */
	    Ntf_UpdateNumNotifSent (Hie.Deg.DegCod,Hie.Crs.CrsCod,NotifyEvent,*NumNotif,*NumMails);
	   }

	 /***** Mark all pending notifications of this user as 'sent' *****/
	 Ntf_DB_MarkPendingNtfsAsSent (ToUsrDat->UsrCod);
	}

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);
     }
  }

/*****************************************************************************/
/****** Get notify event type from string number coming from database ********/
/*****************************************************************************/

Ntf_NotifyEvent_t Ntf_GetNotifyEventFromStr (const char *Str)
  {
   unsigned UnsignedNum;

   if (sscanf (Str,"%u",&UnsignedNum) == 1)
      if (UnsignedNum < Ntf_NUM_NOTIFY_EVENTS)
         return (Ntf_NotifyEvent_t) UnsignedNum;

   return Ntf_EVENT_UNKNOWN;
  }

/*****************************************************************************/
/************* Get number of events notified and emails sent *****************/
/*****************************************************************************/

static void Ntf_GetNumNotifSent (long DegCod,long CrsCod,
                                 Ntf_NotifyEvent_t NotifyEvent,
                                 unsigned *NumEvents,unsigned *NumMails)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;

   /***** Get number of notifications sent by email from database *****/
   if (Ntf_DB_GetNumNotifSent (&mysql_res,DegCod,CrsCod,NotifyEvent))
     {
      row = mysql_fetch_row (mysql_res);
      if (sscanf (row[0],"%u",NumEvents) != 1)
         Err_ShowErrorAndExit ("Error when getting number of notifications sent by email.");
      if (sscanf (row[1],"%u",NumMails) != 1)
         Err_ShowErrorAndExit ("Error when getting number of notifications sent by email.");
     }
   else
      *NumEvents = *NumMails = 0;

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/********** Update number of notifications and notify emails sent ************/
/*****************************************************************************/

static void Ntf_UpdateNumNotifSent (long DegCod,long CrsCod,
                                    Ntf_NotifyEvent_t NotifyEvent,
                                    unsigned NumEvents,unsigned NumMails)
  {
   unsigned CurrentNumEvents;
   unsigned CurrentNumMails;

   /***** Get number of events notified and number of mails sent *****/
   Ntf_GetNumNotifSent (DegCod,CrsCod,NotifyEvent,&CurrentNumEvents,&CurrentNumMails);

   /***** Update number of notifications and notify emails sent *****/
   Ntf_DB_UpdateNumNotifSent (DegCod,CrsCod,NotifyEvent,
                              CurrentNumEvents + NumEvents,
                              CurrentNumMails + NumMails);
  }

/*****************************************************************************/
/******************** Mark all my notifications as seen **********************/
/*****************************************************************************/

void Ntf_MarkAllNotifAsSeen (void)
  {
   /***** Set all my notifications as seen *****/
   Ntf_DB_MarkAllMyNotifAsSeen ();

   /***** Show my notifications again *****/
   Ntf_ShowMyNotifications ();
  }

/*****************************************************************************/
/*** Activate the sending of email to notify me that I have new messages *****/
/*****************************************************************************/

void Ntf_PutFormChangeNotifSentByEMail (void)
  {
   extern const char *Hlp_PROFILE_Settings_notifications;
   extern const char *Txt_Save_changes;
   extern const char *Txt_Notifications;
   extern const char *Txt_Create_BR_notification;
   extern const char *Txt_Notify_me_BR_by_email;
   extern const char *Txt_NOTIFY_EVENTS_PLURAL[Ntf_NUM_NOTIFY_EVENTS];
   Ntf_NotifyEvent_t NotifyEvent;

   /***** Begin section with settings on privacy *****/
   HTM_SECTION_Begin (Ntf_NOTIFICATIONS_ID);

      /***** Begin box *****/
      Box_BoxBegin (NULL,Txt_Notifications,
		    Ntf_PutIconsNotif,NULL,
		    Hlp_PROFILE_Settings_notifications,Box_NOT_CLOSABLE);

	 /***** Begin form *****/
	 Frm_BeginForm (ActChgNtfPrf);

	    /***** Warning if I can not receive email notifications *****/
	    if (!Mai_CheckIfUsrCanReceiveEmailNotif (&Gbl.Usrs.Me.UsrDat))
	       Mai_WriteWarningEmailNotifications ();

	    /***** List of notifications *****/
	    HTM_TABLE_BeginCenterPadding (2);

	       HTM_TR_Begin (NULL);
		  HTM_TH_Empty (1);
		  HTM_TH (Txt_Create_BR_notification,HTM_HEAD_CENTER);
		  HTM_TH (Txt_Notify_me_BR_by_email ,HTM_HEAD_CENTER);
	       HTM_TR_End ();

	       /***** Checkbox to activate internal notifications and email notifications
		      about events *****/
	       for (NotifyEvent  = (Ntf_NotifyEvent_t) 1;
		    NotifyEvent <= (Ntf_NotifyEvent_t) (Ntf_NUM_NOTIFY_EVENTS - 1);
		    NotifyEvent++)	// O is reserved for Ntf_EVENT_UNKNOWN
		 {
		  HTM_TR_Begin (NULL);

		     HTM_TD_Begin ("class=\"RM FORM_IN_%s\"",
		                   The_GetSuffix ());
			HTM_TxtColon (Txt_NOTIFY_EVENTS_PLURAL[NotifyEvent]);
		     HTM_TD_End ();

		     HTM_TD_Begin ("class=\"CM\"");
			HTM_INPUT_CHECKBOX (Ntf_ParamNotifMeAboutNotifyEvents[NotifyEvent],HTM_DONT_SUBMIT_ON_CHANGE,
					    "value=\"Y\"%s",
					    (Gbl.Usrs.Me.UsrDat.NtfEvents.CreateNotif &
					     (1 << NotifyEvent)) ? " checked=\"checked\"" :
								   "");
		     HTM_TD_End ();

		     HTM_TD_Begin ("class=\"CM\"");
			HTM_INPUT_CHECKBOX (Ntf_ParamEmailMeAboutNotifyEvents[NotifyEvent],HTM_DONT_SUBMIT_ON_CHANGE,
					    "value=\"Y\"%s",
					    (Gbl.Usrs.Me.UsrDat.NtfEvents.SendEmail &
					     (1 << NotifyEvent)) ? " checked=\"checked\"" :
								   "");
		     HTM_TD_End ();

		  HTM_TR_End ();
		 }

	    HTM_TABLE_End ();

	    /***** Button to save changes *****/
	    Btn_PutConfirmButton (Txt_Save_changes);

	 /***** End form *****/
	 Frm_EndForm ();

      /***** End box *****/
      Box_BoxEnd ();

   /***** End section with settings about notifications *****/
   HTM_SECTION_End ();
  }

/*****************************************************************************/
/** Get parameter with the sending of email to notify me that I have msgs. ***/
/*****************************************************************************/

static void Ntf_GetParamsNotifyEvents (void)
  {
   Ntf_NotifyEvent_t NotifyEvent;
   bool CreateNotifForThisEvent;

   Gbl.Usrs.Me.UsrDat.NtfEvents.CreateNotif = 0;
   Gbl.Usrs.Me.UsrDat.NtfEvents.SendEmail = 0;
   for (NotifyEvent  = (Ntf_NotifyEvent_t) 1;
	NotifyEvent <= (Ntf_NotifyEvent_t) (Ntf_NUM_NOTIFY_EVENTS - 1);
	NotifyEvent++)	// 0 is reserved for Ntf_EVENT_UNKNOWN
     {
      if ((CreateNotifForThisEvent = Par_GetParToBool (Ntf_ParamNotifMeAboutNotifyEvents[NotifyEvent])))
         Gbl.Usrs.Me.UsrDat.NtfEvents.CreateNotif |= (1 << NotifyEvent);

      if (CreateNotifForThisEvent)
	{
         Par_GetParToBool (Ntf_ParamEmailMeAboutNotifyEvents[NotifyEvent]);
         if (Par_GetParToBool (Ntf_ParamEmailMeAboutNotifyEvents[NotifyEvent]))
            Gbl.Usrs.Me.UsrDat.NtfEvents.SendEmail |= (1 << NotifyEvent);
	}
     }
  }

/*****************************************************************************/
/******* Change my setting about sending me notify emails about events *******/
/*****************************************************************************/

void Ntf_ChangeNotifyEvents (void)
  {
   extern const char *Txt_Your_settings_about_notifications_have_changed;

   /***** Get param with whether notify me about events *****/
   Ntf_GetParamsNotifyEvents ();

   /***** Store settings about notify events *****/
   Set_DB_UpdateMySettingsAboutNotifyEvents ();

   /***** Show message *****/
   Ale_ShowAlert (Ale_SUCCESS,Txt_Your_settings_about_notifications_have_changed);
  }

/*****************************************************************************/
/************************ Write number of notifications **********************/
/*****************************************************************************/

void Ntf_WriteNumberOfNewNtfs (void)
  {
   extern const char *Txt_See_notifications;
   extern const char *Txt_notification;
   extern const char *Txt_notifications;
   extern const char *Txt_Notifications;
   extern const char *Txt_NOTIF_new_SINGULAR;
   extern const char *Txt_NOTIF_new_PLURAL;
   unsigned NumUnseenNtfs;
   unsigned NumNewNtfs = 0;

   /***** Get my number of unseen notifications *****/
   if ((NumUnseenNtfs = Ntf_DB_GetNumAllMyUnseenNtfs ()))
      NumNewNtfs = Ntf_DB_GetNumMyNewUnseenNtfs ();

   /***** Begin form *****/
   Frm_BeginFormId (ActSeeNewNtf,"form_ntf");

      /***** Begin link *****/
      HTM_BUTTON_Submit_Begin (Txt_See_notifications,"class=\"BT_LINK\"");

	 /***** Number of unseen notifications *****/
	 HTM_SPAN_Begin ("id=\"notif_all\"");
	    HTM_TxtF ("%u&nbsp;%s",NumUnseenNtfs,
		      NumUnseenNtfs == 1 ? Txt_notification :
					   Txt_notifications);
	 HTM_SPAN_End ();

	 /***** Icon and number of new notifications *****/
	 if (NumNewNtfs)
	   {
	    HTM_BR ();
	    HTM_IMG (Cfg_URL_ICON_PUBLIC,"bell.svg",Txt_Notifications,
		     "class=\"ICO16x16 NOTIF_ICO_%s\"",The_GetSuffix ());
	    HTM_TxtF ("&nbsp;%u",NumNewNtfs);
	    HTM_SPAN_Begin ("id=\"notif_new\"");
	       HTM_TxtF ("&nbsp;%s",NumNewNtfs == 1 ? Txt_NOTIF_new_SINGULAR :
						      Txt_NOTIF_new_PLURAL);
	    HTM_SPAN_End ();
	   }

      /***** End link *****/
      HTM_BUTTON_End ();

   /***** End form *****/
   Frm_EndForm ();
  }

/*****************************************************************************/
/****** Get and show number of users who want to be notified by email ********/
/*****************************************************************************/

void Ntf_GetAndShowNumUsrsPerNotifyEvent (void)
  {
   extern const char *Hlp_ANALYTICS_Figures_notifications;
   extern const char *Txt_FIGURE_TYPES[Fig_NUM_FIGURES];
   extern const char *Txt_Event;
   extern const char *Txt_NOTIFY_EVENTS_PLURAL[Ntf_NUM_NOTIFY_EVENTS];
   extern const char *Txt_Number_of_users;
   extern const char *Txt_PERCENT_of_users;
   extern const char *Txt_Number_of_events;
   extern const char *Txt_Number_of_emails;
   extern const char *Txt_Total;
   Ntf_NotifyEvent_t NotifyEvent;
   char *SubQuery;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumUsrsTotal;
   unsigned NumUsrsTotalWhoWantToBeNotifiedByEMailAboutSomeEvent;
   unsigned NumUsrs[Ntf_NUM_NOTIFY_EVENTS];
   unsigned NumEventsTotal = 0;
   unsigned NumEvents[Ntf_NUM_NOTIFY_EVENTS];
   unsigned NumMailsTotal = 0;
   unsigned NumMails[Ntf_NUM_NOTIFY_EVENTS];

   /***** Begin box and table *****/
   Box_BoxTableBegin (NULL,Txt_FIGURE_TYPES[Fig_NOTIFY_EVENTS],
                      NULL,NULL,
                      Hlp_ANALYTICS_Figures_notifications,Box_NOT_CLOSABLE,2);

      /***** Heading row *****/
      HTM_TR_Begin (NULL);
	 HTM_TH (Txt_Event           ,HTM_HEAD_LEFT);
	 HTM_TH (Txt_Number_of_users ,HTM_HEAD_RIGHT);
	 HTM_TH (Txt_PERCENT_of_users,HTM_HEAD_RIGHT);
	 HTM_TH (Txt_Number_of_events,HTM_HEAD_RIGHT);
	 HTM_TH (Txt_Number_of_emails,HTM_HEAD_RIGHT);
      HTM_TR_End ();

      /***** Get total number of users *****/
      NumUsrsTotal = Usr_GetTotalNumberOfUsers ();

      /***** Get total number of users who want to be
	     notified by email on some event, from database *****/
      NumUsrsTotalWhoWantToBeNotifiedByEMailAboutSomeEvent =
      Usr_DB_GetNumUsrsWhoChoseAnOption ("usr_data.EmailNtfEvents<>0");

      /***** For each notify event... *****/
      for (NotifyEvent  = (Ntf_NotifyEvent_t) 1;
	   NotifyEvent <= (Ntf_NotifyEvent_t) (Ntf_NUM_NOTIFY_EVENTS - 1);
	   NotifyEvent++) // 0 is reserved for Ntf_EVENT_UNKNOWN
	{
	 /* Get the number of users who want to be notified by email on this event, from database */
	 if (asprintf (&SubQuery,"((usr_data.EmailNtfEvents & %u)<>0)",
		       (1 << NotifyEvent)) < 0)
	    Err_NotEnoughMemoryExit ();
	 NumUsrs[NotifyEvent] = Usr_DB_GetNumUsrsWhoChoseAnOption (SubQuery);
	 free (SubQuery);

	 /* Get number of notifications by email from database */
	 if (Ntf_DB_GetNumNotifs (&mysql_res,NotifyEvent))
	   {
	    row = mysql_fetch_row (mysql_res);

	    /* Get number of events notified */
	    if (row[0])
	      {
	       if (sscanf (row[0],"%u",&NumEvents[NotifyEvent]) != 1)
		  Err_ShowErrorAndExit ("Error when getting the number of notifications by email.");
	      }
	    else
	       NumEvents[NotifyEvent] = 0;

	    /* Get number of mails sent */
	    if (row[1])
	      {
	       if (sscanf (row[1],"%u",&NumMails[NotifyEvent]) != 1)
		  Err_ShowErrorAndExit ("Error when getting the number of emails to notify events3.");
	      }
	    else
	       NumMails[NotifyEvent] = 0;
	   }
	 else
	   {
	    NumEvents[NotifyEvent] = 0;
	    NumMails[NotifyEvent]  = 0;
	   }

	 /* Free structure that stores the query result */
	 DB_FreeMySQLResult (&mysql_res);

	 /* Update total number of events and mails */
	 NumEventsTotal += NumEvents[NotifyEvent];
	 NumMailsTotal  += NumMails [NotifyEvent];
	}

      /***** Write number of users who want to be notified by email on each event *****/
      for (NotifyEvent  = (Ntf_NotifyEvent_t) 1;
	   NotifyEvent <= (Ntf_NotifyEvent_t) (Ntf_NUM_NOTIFY_EVENTS - 1);
	   NotifyEvent++) // 0 is reserved for Ntf_EVENT_UNKNOWN
	{
	 HTM_TR_Begin (NULL);

	    HTM_TD_Begin ("class=\"LM DAT_%s\"",
	                  The_GetSuffix ());
	       HTM_Txt (Txt_NOTIFY_EVENTS_PLURAL[NotifyEvent]);
	    HTM_TD_End ();

	    HTM_TD_Begin ("class=\"RM DAT_%s\"",
	                  The_GetSuffix ());
	       HTM_Unsigned (NumUsrs[NotifyEvent]);
	    HTM_TD_End ();

	    HTM_TD_Begin ("class=\"RM DAT_%s\"",
	                  The_GetSuffix ());
	       HTM_Percentage (NumUsrsTotal ? (double) NumUsrs[NotifyEvent] * 100.0 /
					      (double) NumUsrsTotal :
					      0.0);
	    HTM_TD_End ();

	    HTM_TD_Begin ("class=\"RM DAT_%s\"",
	                  The_GetSuffix ());
	       HTM_Unsigned (NumEvents[NotifyEvent]);
	    HTM_TD_End ();

	    HTM_TD_Begin ("class=\"RM DAT_%s\"",
	                  The_GetSuffix ());
	       HTM_Unsigned (NumMails[NotifyEvent]);
	    HTM_TD_End ();

	 HTM_TR_End ();
	}

      /***** Write total number of users who want to be notified by email on some event *****/
      HTM_TR_Begin (NULL);

	 HTM_TD_Begin ("class=\"LM DAT_STRONG_%s LINE_TOP\"",
	               The_GetSuffix ());
	    HTM_Txt (Txt_Total);
	 HTM_TD_End ();

	 HTM_TD_Begin ("class=\"RM DAT_STRONG_%s LINE_TOP\"",
	               The_GetSuffix ());
	    HTM_Unsigned (NumUsrsTotalWhoWantToBeNotifiedByEMailAboutSomeEvent);
	 HTM_TD_End ();

	 HTM_TD_Begin ("class=\"RM DAT_STRONG_%s LINE_TOP\"",
	               The_GetSuffix ());
	    HTM_Percentage (NumUsrsTotal ? (double) NumUsrsTotalWhoWantToBeNotifiedByEMailAboutSomeEvent * 100.0 /
					   (double) NumUsrsTotal :
					   0.0);
	 HTM_TD_End ();

	 HTM_TD_Begin ("class=\"RM DAT_STRONG_%s LINE_TOP\"",
	               The_GetSuffix ());
	    HTM_Unsigned (NumEventsTotal);
	 HTM_TD_End ();

	 HTM_TD_Begin ("class=\"RM DAT_STRONG_%s LINE_TOP\"",
	               The_GetSuffix ());
	    HTM_Unsigned (NumMailsTotal);
	 HTM_TD_End ();

      HTM_TR_End ();

   /***** End table and box *****/
   Box_BoxTableEnd ();
  }
