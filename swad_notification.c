// swad_notification.c: notifications about events, sent by email

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
#include "swad_action_list.h"
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
#include "swad_hierarchy_type.h"
#include "swad_HTML.h"
#include "swad_mark.h"
#include "swad_message.h"
#include "swad_message_database.h"
#include "swad_notice.h"
#include "swad_notification.h"
#include "swad_notification_database.h"
#include "swad_parameter_code.h"
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
   [Ntf_EVENT_SURVEY           ] = "survey",
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
   [Ntf_EVENT_ASSIGNMENT       ] = ActSeeAllAsg,
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
   [Ntf_EVENT_SURVEY           ] = ActSeeAllSvy,
   /* Profile tab */
  };

/*****************************************************************************/
/***************************** Private constants *****************************/
/*****************************************************************************/

// Notify me notification events
static const char *Ntf_ParNotifMeAboutNotifyEvents[Ntf_NUM_NOTIFY_EVENTS] =
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
   [Ntf_EVENT_SURVEY           ] = "NotifyNtfEventSurvey",
   /* Profile tab */
  };

// Email me about notification events
static const char *Ntf_ParEmailMeAboutNotifyEvents[Ntf_NUM_NOTIFY_EVENTS] =
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
   [Ntf_EVENT_SURVEY           ] = "EmailNtfEventSurvey",
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
   [Ntf_EVENT_SURVEY           ] = "poll.svg",
   /* Profile tab */
  };

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Ntf_PutContextualLinks (bool AllNotifications,
				    unsigned NumNotifications);
static void Ntf_PutIconsNotif (__attribute__((unused)) void *Args);
static void Ntf_WriteHeading (void);
static void Ntf_GetNotif (MYSQL_RES *mysql_res,
			  Ntf_NotifyEvent_t *NotifyEvent,
			  struct Usr_Data *UsrDat,
			  struct Hie_Node Hie[Hie_NUM_LEVELS],
			  long *Cod,time_t *DateTimeUTC,
			  Ntf_Status_t *Status);
static void Ntf_WriteNotif (Ntf_NotifyEvent_t NotifyEvent,
			    struct Usr_Data *UsrDat,
			    const struct Hie_Node Hie[Hie_NUM_LEVELS],
			    long Cod,time_t DateTimeUTC,
			    Ntf_Status_t Status);

static bool Ntf_GetAllNotificationsFromForm (void);

static Act_Action_t Ntf_StartFormGoToAction (Ntf_NotifyEvent_t NotifyEvent,
                                             long CrsCod,const struct Usr_Data *UsrDat,long Cod,
                                             const struct For_Forums *Forums);
static void Ntf_PutParNotifyEvent (Ntf_NotifyEvent_t NotifyEvent);

static void Ntf_SendPendingNotifByEMailToOneUsr (const struct Usr_Data *ToUsrDat,
                                                 unsigned *NumNotif,
                                                 unsigned *NumMails);
static void Ntf_GetNumNotifSent (long DegCod,long CrsCod,
                                 Ntf_NotifyEvent_t NotifyEvent,
                                 unsigned *NumEvents,unsigned *NumMails);
static void Ntf_UpdateNumNotifSent (long DegCod,long CrsCod,
                                    Ntf_NotifyEvent_t NotifyEvent,
                                    unsigned NumEvents,unsigned NumMails);

static void Ntf_GetParsNotifyEvents (void);

/*****************************************************************************/
/*************************** Show my notifications ***************************/
/*****************************************************************************/

void Ntf_ShowMyNotifications (void)
  {
   extern const char *Hlp_START_Notifications;
   extern const char *Txt_Notifications;
   extern const char *Txt_You_have_no_notifications;
   extern const char *Txt_You_have_no_unread_notifications;
   MYSQL_RES *mysql_res;
   unsigned NumNotif;
   unsigned NumNotifications;
   bool AllNotifications;
   Ntf_NotifyEvent_t NotifyEvent = (Ntf_NotifyEvent_t) 0;	// Initialized to avoid warning
   struct Usr_Data UsrDat;
   struct Hie_Node Hie[Hie_NUM_LEVELS];
   long Cod;
   time_t DateTimeUTC;	// Date-time of the event
   Ntf_Status_t Status;

   /***** Get my notifications from database *****/
   AllNotifications = Ntf_GetAllNotificationsFromForm ();
   NumNotifications = Ntf_DB_GetMyNotifications (&mysql_res,AllNotifications);

   /***** Contextual menu *****/
   Ntf_PutContextualLinks (AllNotifications,NumNotifications);

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
	    Ntf_WriteHeading ();

	    /***** List notifications one by one *****/
	    for (NumNotif = 0;
		 NumNotif < NumNotifications;
		 NumNotif++)
	      {
	       /* Get notification */
	       Ntf_GetNotif (mysql_res,
			     &NotifyEvent,&UsrDat,Hie,&Cod,&DateTimeUTC,&Status);

	       /* Write row for this notification */
	       Ntf_WriteNotif (NotifyEvent,&UsrDat,Hie,Cod,DateTimeUTC,Status);
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
/*************************** Put contextual links ****************************/
/*****************************************************************************/

static void Ntf_PutContextualLinks (bool AllNotifications,
				    unsigned NumNotifications)
  {
   extern const char *Txt_Show_all_notifications;
   extern const char *Txt_Show_all_NOTIFICATIONS;
   extern const char *Txt_Mark_all_NOTIFICATIONS_as_read;
   extern const char *Txt_Domains;

   Mnu_ContextMenuBegin ();

      /***** Show all notifications *****/
      Lay_PutContextualCheckbox (ActSeeNtf,NULL,
				 "All",
				 AllNotifications,false,
				 Txt_Show_all_notifications,
				 Txt_Show_all_NOTIFICATIONS);

      /***** Mark notifications as read *****/
      if (NumNotifications)	// TODO: Show message only when I don't have notificacions at all
	 Lay_PutContextualLinkIconText (ActMrkNtfSee,NULL,
					NULL,NULL,
					"eye.svg",Ico_BLACK,
					Txt_Mark_all_NOTIFICATIONS_as_read,NULL);

      /***** View allowed mail domains *****/
      Lay_PutContextualLinkIconText (ActSeeMai,NULL,
				     NULL,NULL,
				     "envelope.svg",Ico_BLACK,
				     Txt_Domains,NULL);

   Mnu_ContextMenuEnd ();
  }

/*****************************************************************************/
/****************** Put contextual icons in notifications ********************/
/*****************************************************************************/

static void Ntf_PutIconsNotif (__attribute__((unused)) void *Args)
  {
   /***** Edit notification settings *****/
   Ico_PutContextualIconToConfigure (ActReqEdiSet,Ntf_NOTIFICATIONS_ID,
				     NULL,NULL);

   /***** Put icon to show a figure *****/
   Fig_PutIconToShowFigure (Fig_NOTIFY_EVENTS);
  }

/*****************************************************************************/
/**************************** Write table heading ****************************/
/*****************************************************************************/

static void Ntf_WriteHeading (void)
  {
   extern const char *Txt_Event;
   extern const char *Txt_MSG_From;
   extern const char *Txt_Location;
   extern const char *Txt_Date;
   extern const char *Txt_Email;

   HTM_TR_Begin (NULL);
      HTM_TH_Span (Txt_Event   ,HTM_HEAD_LEFT ,1,2,NULL);
      HTM_TH      (Txt_MSG_From,HTM_HEAD_LEFT );
      HTM_TH      (Txt_Location,HTM_HEAD_LEFT );
      HTM_TH      (Txt_Date    ,HTM_HEAD_RIGHT);
      HTM_TH      (Txt_Email   ,HTM_HEAD_LEFT );
   HTM_TR_End ();
  }

/*****************************************************************************/
/****************** Get a notification from database result ******************/
/*****************************************************************************/

static void Ntf_GetNotif (MYSQL_RES *mysql_res,
			  Ntf_NotifyEvent_t *NotifyEvent,
			  struct Usr_Data *UsrDat,
			  struct Hie_Node Hie[Hie_NUM_LEVELS],
			  long *Cod,time_t *DateTimeUTC,
			  Ntf_Status_t *Status)
  {
   extern bool (*Hie_GetDataByCod[Hie_NUM_LEVELS]) (struct Hie_Node *Node);
   MYSQL_ROW row;
   Hie_Level_t Level;
   unsigned Col;

   /***** Get next notification *****/
   row = mysql_fetch_row (mysql_res);

   /***** Get event type (row[0]) *****/
   *NotifyEvent = Ntf_GetNotifyEventFromStr (row[0]);

   /***** Get (from) user code (row[1]) *****/
   UsrDat->UsrCod = Str_ConvertStrCodToLongCod (row[1]);
   Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (UsrDat,	// Get user's data from database
					    Usr_DONT_GET_PREFS,
					    Usr_DONT_GET_ROLE_IN_CRS);

   /***** Get institution code, center code, degree code and course code
          (row[2], row[3], row[4] and row[5]) *****/
   for (Level  = Hie_INS, Col = 2;
	Level <= Hie_CRS;
	Level++, Col++)
     {
      Hie[Level].HieCod = Str_ConvertStrCodToLongCod (row[Col]);
      Hie_GetDataByCod[Level] (&Hie[Level]);
     }

   /***** Get message/post/... code (row[6]) *****/
   *Cod = Str_ConvertStrCodToLongCod (row[6]);

   /***** Get time of the event (row[7]) *****/
   *DateTimeUTC = Dat_GetUNIXTimeFromStr (row[7]);

   /***** Get status (row[8]) *****/
   if (sscanf (row[8],"%u",Status) != 1)
      Err_WrongStatusExit ();
  }

/*****************************************************************************/
/******************* Write a table row for a notification ********************/
/*****************************************************************************/

static void Ntf_WriteNotif (Ntf_NotifyEvent_t NotifyEvent,
			    struct Usr_Data *UsrDat,
			    const struct Hie_Node Hie[Hie_NUM_LEVELS],
			    long Cod,time_t DateTimeUTC,
			    Ntf_Status_t Status)
  {
   extern const char *Txt_NOTIFY_EVENTS_SINGULAR[Ntf_NUM_NOTIFY_EVENTS];
   extern const char *Txt_Forum;
   extern const char *Txt_NOTIFICATION_STATUS[Ntf_NUM_STATUS_TXT];
   extern const char *Txt_HIERARCHY_SINGUL_Abc[Hie_NUM_LEVELS];
   extern const char *Txt_You_have_no_notifications;
   extern const char *Txt_You_have_no_unread_notifications;
   char SummaryStr[Ntf_MAX_BYTES_SUMMARY + 1];
   char *ContentStr;
   struct
     {
      const char *Txt;
      const char *Link;
      const char *Author;
      const char *Bg;
     } Class;
   Frm_PutForm_t PutForm;
   Act_Action_t Action = ActUnk;
   Hie_Level_t Level;
   Ntf_StatusTxt_t StatusTxt;
   struct For_Forums Forums;
   char ForumName[For_MAX_BYTES_FORUM_NAME + 1];

   if (Status & Ntf_STATUS_BIT_REMOVED)	// The source of the notification was removed
     {
      Class.Txt    = "MSG_TIT_REM";
      Class.Link   = "BT_LINK MSG_TIT_REM";
      Class.Author = "MSG_AUT_LIGHT";
      Class.Bg     = "MSG_BG_REM";
      PutForm = Frm_DONT_PUT_FORM;
     }
   else
     {
      if (Status & Ntf_STATUS_BIT_READ)	// I have already seen the source of the notification
	{
	 Class.Txt    = "MSG_TIT";
	 Class.Link   = "LT BT_LINK MSG_TIT";
	 Class.Author = "MSG_AUT";
	 Class.Bg     = "MSG_BG";
	}
      else					// I have not seen the source of the notification
	{
	 Class.Txt    = "MSG_TIT_NEW";
	 Class.Link   = "LT BT_LINK MSG_TIT_NEW";
	 Class.Author = "MSG_AUT_NEW";
	 Class.Bg     = "MSG_BG_NEW";
	}
      PutForm = Frm_PUT_FORM;
     }

   /***** Write row for this notification *****/
   /* Write event icon */
   HTM_TR_Begin (NULL);

      HTM_TD_Begin ("class=\"LT %s_%s\" style=\"width:25px;\"",
		    Class.Bg,The_GetSuffix ());
	 if (PutForm == Frm_PUT_FORM)
	   {
	    Action = Ntf_StartFormGoToAction (NotifyEvent,Hie[Hie_CRS].HieCod,UsrDat,Cod,&Forums);
	    PutForm = Frm_CheckIfInside () ? Frm_PUT_FORM :
					     Frm_DONT_PUT_FORM;
	   }
	 switch (PutForm)
	   {
	    case Frm_DONT_PUT_FORM:
	       Ico_PutIconOff (Ntf_Icons[NotifyEvent],Ico_BLACK,
			       Txt_NOTIFY_EVENTS_SINGULAR[NotifyEvent]);
	       break;
	    case Frm_PUT_FORM:
	       Ico_PutIconLink (Ntf_Icons[NotifyEvent],Ico_BLACK,Action);
	       Frm_EndForm ();
	       break;
	   }
      HTM_TD_End ();

      /* Write event type */
      HTM_TD_Begin ("class=\"LT %s_%s\"",Class.Bg,The_GetSuffix ());
	 switch (PutForm)
	   {
	    case Frm_DONT_PUT_FORM:
	       HTM_SPAN_Begin ("class=\"Ntf_TYPE %s\"",Class.Txt);
		  HTM_Txt (Txt_NOTIFY_EVENTS_SINGULAR[NotifyEvent]);
	       HTM_SPAN_End ();
	       break;
	    case Frm_PUT_FORM:
	       Action = Ntf_StartFormGoToAction (NotifyEvent,Hie[Hie_CRS].HieCod,UsrDat,Cod,&Forums);
	       PutForm = Frm_CheckIfInside () ? Frm_PUT_FORM :
						Frm_DONT_PUT_FORM;
		  HTM_BUTTON_Submit_Begin (Txt_NOTIFY_EVENTS_SINGULAR[NotifyEvent],
					   "class=\"Ntf_TYPE LT %s_%s\"",
					   Class.Link,The_GetSuffix ());
		     HTM_Txt (Txt_NOTIFY_EVENTS_SINGULAR[NotifyEvent]);
		  HTM_BUTTON_End ();
	       Frm_EndForm ();
	       break;
	   }
      HTM_TD_End ();

      /* Write user (from) */
      HTM_TD_Begin ("class=\"LT %s_%s %s_%s\"",
		    Class.Author,The_GetSuffix (),
		    Class.Bg,The_GetSuffix ());
	 Usr_WriteAuthor (UsrDat,Cns_ENABLED);
      HTM_TD_End ();

      /* Write location */
      HTM_TD_Begin ("class=\"LT %s_%s\"",Class.Bg,The_GetSuffix ());
	 if (NotifyEvent == Ntf_EVENT_FORUM_POST_COURSE ||
	     NotifyEvent == Ntf_EVENT_FORUM_REPLY)
	   {
	    if (PutForm == Frm_PUT_FORM)
	      {
	       Action = Ntf_StartFormGoToAction (NotifyEvent,Hie[Hie_CRS].HieCod,UsrDat,Cod,&Forums);
	       PutForm = Frm_CheckIfInside () ? Frm_PUT_FORM :
						Frm_DONT_PUT_FORM;
	      }

	    switch (PutForm)
	      {
	       case Frm_DONT_PUT_FORM:
		  HTM_DIV_Begin ("class=\"Ntf_LOCATION %s_%s\"",
				  Class.Txt,The_GetSuffix ());
		  break;
	       case Frm_PUT_FORM:
		  HTM_BUTTON_Submit_Begin (Txt_NOTIFY_EVENTS_SINGULAR[NotifyEvent],
					   "class=\"Ntf_LOCATION %s_%s\"",
					   Class.Link,The_GetSuffix ());
		  break;
	      }
	    /* Get forum type of the post */
	    For_ResetForums (&Forums);
	    For_GetThreadForumTypeAndHieCodOfAPost (Cod,&Forums.Forum);
	    For_SetForumName (&Forums.Forum,ForumName,Gbl.Prefs.Language,false);	// Set forum name in recipient's language
	    HTM_TxtF ("%s: %s",Txt_Forum,ForumName);
	    switch (PutForm)
	      {
	       case Frm_DONT_PUT_FORM:
		  HTM_DIV_End ();
		  break;
	       case Frm_PUT_FORM:
		  HTM_BUTTON_End ();
		  Frm_EndForm ();
		  break;
	      }
	   }
	 else
	   {
	    if (PutForm == Frm_PUT_FORM)
	      {
	       Action = Ntf_StartFormGoToAction (NotifyEvent,Hie[Hie_CRS].HieCod,UsrDat,Cod,&Forums);
	       PutForm = Frm_CheckIfInside () ? Frm_PUT_FORM :
						Frm_DONT_PUT_FORM;
	      }

	    switch (PutForm)
	      {
	       case Frm_DONT_PUT_FORM:
		  HTM_DIV_Begin ("class=\"Ntf_LOCATION %s_%s\"",
				 Class.Txt,The_GetSuffix ());
		  break;
	       case Frm_PUT_FORM:
		  HTM_BUTTON_Submit_Begin (Txt_NOTIFY_EVENTS_SINGULAR[NotifyEvent],
					   "class=\"Ntf_LOCATION %s_%s\"",
					   Class.Link,The_GetSuffix ());
		  break;
	      }

	    for (Level  = Hie_CRS;
		 Level >= Hie_INS;
		 Level--)
	       if (Hie[Level].HieCod > 0)
		 {
		  HTM_TxtF ("%s: %s",
			    Txt_HIERARCHY_SINGUL_Abc[Level],
			    Hie[Level].ShrtName);
		  break;
		 }
	    if (Level < Hie_INS)
	       HTM_Hyphen ();

	    switch (PutForm)
	      {
	       case Frm_DONT_PUT_FORM:
		  HTM_DIV_End ();
		  break;
	       case Frm_PUT_FORM:
		  HTM_BUTTON_End ();
		  Frm_EndForm ();
		  break;
	      }
	   }

      HTM_TD_End ();

      /* Write date and time */
      Msg_WriteMsgDate (DateTimeUTC,Class.Txt,Class.Bg);

      /* Write status (sent by email / pending to be sent by email) */
      HTM_TD_Begin ("class=\"LT %s_%s %s_%s\"",
		    Class.Txt,The_GetSuffix (),
		    Class.Bg,The_GetSuffix ());
	 StatusTxt = Ntf_GetStatusTxtFromStatusBits (Status);
	 HTM_Txt (Txt_NOTIFICATION_STATUS[StatusTxt]);
      HTM_TD_End ();

   HTM_TR_End ();

   /***** Write content of the event *****/
   if (PutForm == Frm_PUT_FORM)
     {
      ContentStr = NULL;

      Ntf_GetNotifSummaryAndContent (SummaryStr,&ContentStr,NotifyEvent,
				     Cod,Hie[Hie_CRS].HieCod,
				     Gbl.Usrs.Me.UsrDat.UsrCod,
				     Ntf_DONT_GET_CONTENT);
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

/*****************************************************************************/
/************* Get whether to show all notifications from form ***************/
/*****************************************************************************/

static bool Ntf_GetAllNotificationsFromForm (void)
  {
   return Par_GetParBool ("All");
  }

/*****************************************************************************/
/*********** Put form to go to an action depending on the event **************/
/*****************************************************************************/

static Act_Action_t Ntf_StartFormGoToAction (Ntf_NotifyEvent_t NotifyEvent,
                                             long CrsCod,const struct Usr_Data *UsrDat,long Cod,
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
            ParCod_PutPar (ParCod_Exa,Cod);

	 /* Free anchor string */
	 Frm_FreeAnchorStr (&Anchor);
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
	       ParCod_PutPar (ParCod_Grp,GrpCod);
	       ParCod_PutPar (ParCod_Fil,FileMetadata.FilCod);
	   }
	 break;
      case Ntf_EVENT_TML_COMMENT:
      case Ntf_EVENT_TML_FAV:
      case Ntf_EVENT_TML_SHARE:
      case Ntf_EVENT_TML_MENTION:
	 // Cod is the code of the social publishing
	 Action = ActSeeGblTL;
         Frm_BeginForm (Action);
	    ParCod_PutPar (ParCod_Pub,Cod);
	    Usr_PutParUsrCodEncrypted (UsrDat->EnUsrCod);
	    Ntf_PutParNotifyEvent (NotifyEvent);
	 break;
      case Ntf_EVENT_FOLLOWER:
         if (UsrDat->EnUsrCod[0])	// User's code found ==>
					// go to user's public profile
           {
            Action = ActSeeOthPubPrf;
            Frm_BeginForm (Action);
	       /* Put param to go to follower's profile */
	       Usr_PutParUsrCodEncrypted (UsrDat->EnUsrCod);
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
	    For_PutAllParsForum (1,	// Page of threads = first
				 1,	// Page of posts   = first
				 Forums->ForumSet,
				 Forums->ThreadsOrder,
				 Forums->Forum.HieCod,
				 Forums->Thread.Selected,
				 -1L);
	 break;
      case Ntf_EVENT_NOTICE:
	 Action = ActSeeOneNot;
         Frm_BeginForm (Action);
	    ParCod_PutPar (ParCod_Not,Cod);
	 break;
      case Ntf_EVENT_MESSAGE:
	 Action = ActExpRcvMsg;
         Frm_BeginForm (Action);
	    ParCod_PutPar (ParCod_Msg,Cod);
	 break;
      default:
	 Action = Ntf_DefaultActions[NotifyEvent];
         Frm_BeginForm (Action);
	 break;
     }

   /***** Parameter to go to another course/degree/center/institution *****/
   if (Frm_CheckIfInside ())
     {
      if (CrsCod > 0)					// Course specified
	{
	 if (CrsCod != Gbl.Hierarchy.Node[Hie_CRS].HieCod)	// Not the current course
	    ParCod_PutPar (ParCod_Crs,CrsCod);			// Go to another course
	}
      else if (DegCod > 0)				// Degree specified
	{
	 if (DegCod != Gbl.Hierarchy.Node[Hie_DEG].HieCod)	// Not the current degree
	    ParCod_PutPar (ParCod_Deg,DegCod);			// Go to another degree
	}
      else if (CtrCod > 0)				// Center specified
	{
	 if (CtrCod != Gbl.Hierarchy.Node[Hie_CTR].HieCod)	// Not the current center
	    ParCod_PutPar (ParCod_Ctr,CtrCod);			// Go to another center
	}
      else if (InsCod > 0)				// Institution specified
	{
	 if (InsCod != Gbl.Hierarchy.Node[Hie_INS].HieCod)	// Not the current institution
	    ParCod_PutPar (ParCod_Ins,InsCod);			// Go to another institution
	}
     }

   return Action;
  }

/*****************************************************************************/
/******************* Get parameter with notify event type ********************/
/*****************************************************************************/

static void Ntf_PutParNotifyEvent (Ntf_NotifyEvent_t NotifyEvent)
  {
   Par_PutParUnsigned (NULL,"NotifyEvent",(unsigned) NotifyEvent);
  }

/*****************************************************************************/
/******************* Get parameter with notify event type ********************/
/*****************************************************************************/

Ntf_NotifyEvent_t Ntf_GetParNotifyEvent (void)
  {
   return (Ntf_NotifyEvent_t)
	  Par_GetParUnsignedLong ("NotifyEvent",
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
                                    Ntf_GetContent_t GetContent)
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
	 if (Enr_GetNumUsrsInCrss (Hie_CRS,Gbl.Hierarchy.Node[Hie_CRS].HieCod,
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
	 For_GetThreadForumTypeAndHieCodOfAPost (Cod,&ForumSelected);
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
            InsCod = ForumSelected.HieCod;
            break;
	 case For_FORUM_CENTER_USRS:
	 case For_FORUM_CENTER_TCHS:
            CtrCod = ForumSelected.HieCod;
            break;
	 case For_FORUM_DEGREE_USRS:
	 case For_FORUM_DEGREE_TCHS:
            DegCod = ForumSelected.HieCod;
            break;
	 case For_FORUM_COURSE_USRS:
	 case For_FORUM_COURSE_TCHS:
            CrsCod = ForumSelected.HieCod;
            break;
	 default:
	    break;
        }
     }
   else
     {
      InsCod = Gbl.Hierarchy.Node[Hie_INS].HieCod;
      CtrCod = Gbl.Hierarchy.Node[Hie_CTR].HieCod;
      DegCod = Gbl.Hierarchy.Node[Hie_DEG].HieCod;
      CrsCod = Gbl.Hierarchy.Node[Hie_CRS].HieCod;
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
	                                              Usr_DONT_GET_ROLE_IN_CRS))
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
   struct Usr_Data ToUsrDat;
   unsigned NumNotif;
   unsigned NumTotalNotif = 0;
   unsigned NumMails;
   unsigned NumTotalMails = 0;

   /***** Get users who must be notified from database ******/
   if ((NumUsrs = Ntf_DB_GetUsrsWhoMustBeNotified (&mysql_res)))
     {
      /***** Initialize structure with user's data *****/
      Usr_UsrDataConstructor (&ToUsrDat);

      /***** Notify the users one by one *****/
      for (NumUsr = 0;
	   NumUsr < NumUsrs;
	   NumUsr++)
	{
         /* Get next user */
         ToUsrDat.UsrCod = DB_GetNextCode (mysql_res);

         /* Get user's data */
	 if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&ToUsrDat,	// Get user's data from database
	                                              Usr_GET_PREFS,	// User's language necessary to write email
	                                              Usr_DONT_GET_ROLE_IN_CRS))
           {
            /* Send one email to this user */
            Ntf_SendPendingNotifByEMailToOneUsr (&ToUsrDat,&NumNotif,&NumMails);
            NumTotalNotif += NumNotif;
            NumTotalMails += NumMails;
           }
        }

      /***** Free memory used for user's data *****/
      Usr_UsrDataDestructor (&ToUsrDat);
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   /***** Delete old notifications ******/
   Ntf_DB_RemoveOldNtfs ();
  }

/*****************************************************************************/
/************ Send pending notifications of one user by email ****************/
/*****************************************************************************/

static void Ntf_SendPendingNotifByEMailToOneUsr (const struct Usr_Data *ToUsrDat,
                                                 unsigned *NumNotif,
                                                 unsigned *NumMails)
  {
   extern bool (*Hie_GetDataByCod[Hie_NUM_LEVELS]) (struct Hie_Node *Node);
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
   struct Hie_Node Hie[Hie_NUM_LEVELS];
   Hie_Level_t Level;
   unsigned Col;
   long Cod;
   struct For_Forum ForumSelected;
   char ForumName[For_MAX_BYTES_FORUM_NAME + 1];
   char FileNameMail[PATH_MAX + 1];
   FILE *FileMail;
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
	    ToUsrLanguage = Cfg_DEFAULT_LANGUAGE;

	 /***** Create temporary file for mail content *****/
	 Mai_CreateFileNameMail (FileNameMail,&FileMail);

	 /***** Welcome note *****/
	 Mai_WriteWelcomeNoteEMail (FileMail,ToUsrDat,ToUsrLanguage);
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
	    /*
	    row[0]: NotifyEvent
	    row[1]: FromUsrCod
	    row[2]: InsCod
	    row[3]: CtrCod
	    row[4]: DegCod
	    row[5]: CrsCod
	    row[6]: Cod
	    */
	    /* Get event type (row[0]) */
	    NotifyEvent = Ntf_GetNotifyEventFromStr (row[0]);

	    /* Get origin user code (row[1]) */
	    FromUsrDat.UsrCod = Str_ConvertStrCodToLongCod (row[1]);
	    Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&FromUsrDat,	// Get origin user's data from database
	                                             Usr_DONT_GET_PREFS,
	                                             Usr_DONT_GET_ROLE_IN_CRS);

	    /* Get data of institution, center, degree and course
	       (row[2], row[3], row[4], row[5]) */
	    for (Level  = Hie_INS, Col = 2;
		 Level <= Hie_CRS;
		 Level++, Col++)
	       {
	        Hie[Level].HieCod = Str_ConvertStrCodToLongCod (row[Col]);
		Hie_GetDataByCod[Level] (&Hie[Level]);
	       }

	    /* Get message/post/... code (row[6]) */
	    Cod = Str_ConvertStrCodToLongCod (row[6]);

	    /* Get forum type */
	    if (NotifyEvent == Ntf_EVENT_FORUM_POST_COURSE ||
		NotifyEvent == Ntf_EVENT_FORUM_REPLY)
	       For_GetThreadForumTypeAndHieCodOfAPost (Cod,&ForumSelected);

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
		  if (Hie[Hie_CRS].HieCod > 0)
		     fprintf (FileMail,"%s: %s\n",
			      Txt_Course_NO_HTML[ToUsrLanguage],
			      Hie[Hie_CRS].FullName);
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
         ReturnCode = Mai_SendMailMsg (FileNameMail,
                                       Txt_Notifications_NO_HTML[ToUsrLanguage],
                                       ToUsrDat->Email);

	 /***** Remove temporary file *****/
	 unlink (FileNameMail);

	 /***** Update number of notifications, number of mails and statistics *****/
	 if (ReturnCode == 0)	// Message sent successfully
	   {
	    *NumNotif = NumNtfs;
	    *NumMails = 1;

	    /* Update statistics about notifications */
	    Ntf_UpdateNumNotifSent (Hie[Hie_DEG].HieCod,Hie[Hie_CRS].HieCod,NotifyEvent,
	                            *NumNotif,*NumMails);
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
			HTM_INPUT_CHECKBOX (Ntf_ParNotifMeAboutNotifyEvents[NotifyEvent],HTM_DONT_SUBMIT_ON_CHANGE,
					    "value=\"Y\"%s",
					    (Gbl.Usrs.Me.UsrDat.NtfEvents.CreateNotif &
					     (1 << NotifyEvent)) ? " checked=\"checked\"" :
								   "");
		     HTM_TD_End ();

		     HTM_TD_Begin ("class=\"CM\"");
			HTM_INPUT_CHECKBOX (Ntf_ParEmailMeAboutNotifyEvents[NotifyEvent],HTM_DONT_SUBMIT_ON_CHANGE,
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

static void Ntf_GetParsNotifyEvents (void)
  {
   Ntf_NotifyEvent_t NotifyEvent;
   bool CreateNotifForThisEvent;

   Gbl.Usrs.Me.UsrDat.NtfEvents.CreateNotif = 0;
   Gbl.Usrs.Me.UsrDat.NtfEvents.SendEmail = 0;
   for (NotifyEvent  = (Ntf_NotifyEvent_t) 1;
	NotifyEvent <= (Ntf_NotifyEvent_t) (Ntf_NUM_NOTIFY_EVENTS - 1);
	NotifyEvent++)	// 0 is reserved for Ntf_EVENT_UNKNOWN
     {
      if ((CreateNotifForThisEvent = Par_GetParBool (Ntf_ParNotifMeAboutNotifyEvents[NotifyEvent])))
         Gbl.Usrs.Me.UsrDat.NtfEvents.CreateNotif |= (1 << NotifyEvent);

      if (CreateNotifForThisEvent)
	{
         Par_GetParBool (Ntf_ParEmailMeAboutNotifyEvents[NotifyEvent]);
         if (Par_GetParBool (Ntf_ParEmailMeAboutNotifyEvents[NotifyEvent]))
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
   Ntf_GetParsNotifyEvents ();

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
   Frm_BeginForm (ActSeeNewNtf);

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
	    HTM_TD_Txt_Left (Txt_NOTIFY_EVENTS_PLURAL[NotifyEvent]);
	    HTM_TD_Unsigned (NumUsrs[NotifyEvent]);
	    HTM_TD_Percentage (NumUsrs[NotifyEvent],NumUsrsTotal );
	    HTM_TD_Unsigned (NumEvents[NotifyEvent]);
	    HTM_TD_Unsigned (NumMails[NotifyEvent]);
	 HTM_TR_End ();
	}

      /***** Write total number of users who want to be notified by email on some event *****/
      HTM_TR_Begin (NULL);
	 HTM_TD_LINE_TOP_Txt (Txt_Total);
	 HTM_TD_LINE_TOP_Unsigned (NumUsrsTotalWhoWantToBeNotifiedByEMailAboutSomeEvent);
	 HTM_TD_LINE_TOP_Percentage (NumUsrsTotalWhoWantToBeNotifiedByEMailAboutSomeEvent,NumUsrsTotal);
	 HTM_TD_LINE_TOP_Unsigned (NumEventsTotal);
	 HTM_TD_LINE_TOP_Unsigned (NumMailsTotal);
      HTM_TR_End ();

   /***** End table and box *****/
   Box_BoxTableEnd ();
  }
