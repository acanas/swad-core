// swad_notification.c: notifications about events, sent by email

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2021 Antonio Cañas Vargas

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

#include <stddef.h>		// For NULL
#include <stdlib.h>		// For system
#include <string.h>
#include <sys/wait.h>		// For the macro WEXITSTATUS
#include <unistd.h>		// For unlink

#include "swad_action.h"
#include "swad_box.h"
#include "swad_config.h"
#include "swad_config.h"
#include "swad_database.h"
#include "swad_enrolment.h"
#include "swad_exam_announcement.h"
#include "swad_figure.h"
#include "swad_follow.h"
#include "swad_form.h"
#include "swad_forum.h"
#include "swad_global.h"
#include "swad_hierarchy.h"
#include "swad_HTML.h"
#include "swad_mark.h"
#include "swad_message.h"
#include "swad_notice.h"
#include "swad_notification.h"
#include "swad_parameter.h"
#include "swad_survey.h"
#include "swad_timeline.h"
#include "swad_timeline_notification.h"
#include "swad_timeline_publication.h"

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
   [Ntf_EVENT_TIMELINE_COMMENT ] = "timelineComment",
   [Ntf_EVENT_TIMELINE_FAV     ] = "timelineFav",
   [Ntf_EVENT_TIMELINE_SHARE   ] = "timelineShare",
   [Ntf_EVENT_TIMELINE_MENTION ] = "timelineMention",
   [Ntf_EVENT_FOLLOWER         ] = "follower",
   /* System tab */
   /* Country tab */
   /* Institution tab */
   /* Centre tab */
   /* Degree tab */
   /* Course tab */
   /* Assessment tab */
   [Ntf_EVENT_ASSIGNMENT       ] = "assignment",
   [Ntf_EVENT_SURVEY           ] = "survey",
   [Ntf_EVENT_EXAM_ANNOUNCEMENT] = "examAnnouncement",
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
   [Ntf_EVENT_TIMELINE_COMMENT ] = ActSeeGblTL,
   [Ntf_EVENT_TIMELINE_FAV     ] = ActSeeGblTL,
   [Ntf_EVENT_TIMELINE_SHARE   ] = ActSeeGblTL,
   [Ntf_EVENT_TIMELINE_MENTION ] = ActSeeGblTL,
   [Ntf_EVENT_FOLLOWER         ] = ActSeeFlr,
   /* System tab */
   /* Country tab */
   /* Institution tab */
   /* Centre tab */
   /* Degree tab */
   /* Course tab */
   /* Assessment tab */
   [Ntf_EVENT_ASSIGNMENT       ] = ActSeeAsg,
   [Ntf_EVENT_SURVEY           ] = ActSeeAllSvy,
   [Ntf_EVENT_EXAM_ANNOUNCEMENT] = ActSeeAllExaAnn,
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
   [Ntf_EVENT_TIMELINE_COMMENT ] = "NotifyNtfEventTimelineComment",
   [Ntf_EVENT_TIMELINE_FAV     ] = "NotifyNtfEventTimelineFav",
   [Ntf_EVENT_TIMELINE_SHARE   ] = "NotifyNtfEventTimelineShare",
   [Ntf_EVENT_TIMELINE_MENTION ] = "NotifyNtfEventTimelineMention",
   [Ntf_EVENT_FOLLOWER         ] = "NotifyNtfEventFollower",
   /* System tab */
   /* Country tab */
   /* Institution tab */
   /* Centre tab */
   /* Degree tab */
   /* Course tab */
   /* Assessment tab */
   [Ntf_EVENT_ASSIGNMENT       ] = "NotifyNtfEventAssignment",
   [Ntf_EVENT_SURVEY           ] = "NotifyNtfEventSurvey",
   [Ntf_EVENT_EXAM_ANNOUNCEMENT] = "NotifyNtfEventExamAnnouncement",
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
   [Ntf_EVENT_TIMELINE_COMMENT ] = "EmailNtfEventTimelineComment",
   [Ntf_EVENT_TIMELINE_FAV     ] = "EmailNtfEventTimelineFav",
   [Ntf_EVENT_TIMELINE_SHARE   ] = "EmailNtfEventTimelineShare",
   [Ntf_EVENT_TIMELINE_MENTION ] = "EmailNtfEventTimelineMention",
   [Ntf_EVENT_FOLLOWER         ] = "EmailNtfEventSocialFollower",
   /* System tab */
   /* Country tab */
   /* Institution tab */
   /* Centre tab */
   /* Degree tab */
   /* Course tab */
   /* Assessment tab */
   [Ntf_EVENT_ASSIGNMENT       ] = "EmailNtfEventAssignment",
   [Ntf_EVENT_SURVEY           ] = "EmailNtfEventSurvey",
   [Ntf_EVENT_EXAM_ANNOUNCEMENT] = "EmailNtfEventExamAnnouncement",
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
   [Ntf_EVENT_TIMELINE_COMMENT ] = "comment-dots.svg",
   [Ntf_EVENT_TIMELINE_FAV     ] = "star.svg",
   [Ntf_EVENT_TIMELINE_SHARE   ] = "share-alt.svg",
   [Ntf_EVENT_TIMELINE_MENTION ] = "at.svg",
   [Ntf_EVENT_FOLLOWER         ] = "user-plus.svg",
   /* System tab */
   /* Country tab */
   /* Institution tab */
   /* Centre tab */
   /* Degree tab */
   /* Course tab */
   /* Assessment tab */
   [Ntf_EVENT_ASSIGNMENT       ] = "edit.svg",
   [Ntf_EVENT_SURVEY           ] = "poll.svg",
   [Ntf_EVENT_EXAM_ANNOUNCEMENT] = "bullhorn.svg",
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

static bool Ntf_StartFormGoToAction (Ntf_NotifyEvent_t NotifyEvent,
                                     long CrsCod,struct UsrData *UsrDat,long Cod,
                                     const struct For_Forums *Forums);
static void Ntf_PutHiddenParamNotifyEvent (Ntf_NotifyEvent_t NotifyEvent);

static void Ntf_UpdateMyLastAccessToNotifications (void);
static void Ntf_SendPendingNotifByEMailToOneUsr (struct UsrData *ToUsrDat,unsigned *NumNotif,unsigned *NumMails);
static void Ntf_GetNumNotifSent (long DegCod,long CrsCod,
                                 Ntf_NotifyEvent_t NotifyEvent,
                                 unsigned *NumEvents,unsigned *NumMails);
static void Ntf_UpdateNumNotifSent (long DegCod,long CrsCod,
                                    Ntf_NotifyEvent_t NotifyEvent,
                                    unsigned NumEvents,unsigned NumMails);

static void Ntf_GetParamsNotifyEvents (void);
static unsigned Ntf_GetNumberOfAllMyUnseenNtfs (void);
static unsigned Ntf_GetNumberOfMyNewUnseenNtfs (void);

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
   extern const char *Txt_Centre;
   extern const char *Txt_Institution;
   extern const char *Txt_NOTIFICATION_STATUS[Ntf_NUM_STATUS_TXT];
   extern const char *Txt_You_have_no_notifications;
   extern const char *Txt_You_have_no_unread_notifications;
   char SubQuery[128];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumNotif;
   unsigned long NumNotifications;
   bool AllNotifications;
   Ntf_NotifyEvent_t NotifyEvent = (Ntf_NotifyEvent_t) 0;	// Initialized to avoid warning
   struct UsrData UsrDat;
   struct Hie_Hierarchy Hie;
   long Cod;
   struct For_Forums Forums;
   char ForumName[For_MAX_BYTES_FORUM_NAME + 1];
   time_t DateTimeUTC;	// Date-time of the event
   Ntf_Status_t Status;
   Ntf_StatusTxt_t StatusTxt;
   char SummaryStr[Ntf_MAX_BYTES_SUMMARY + 1];
   char *ContentStr;
   const char *ClassBackground;
   const char *ClassText;
   const char *ClassLink;
   const char *ClassAuthorBg;
   bool PutLink;

   /***** Get my notifications from database *****/
   AllNotifications = Ntf_GetAllNotificationsFromForm ();
   if (AllNotifications)
      SubQuery[0] = '\0';
   else
      sprintf (SubQuery," AND (Status&%u)=0",
               Ntf_STATUS_BIT_READ |
               Ntf_STATUS_BIT_REMOVED);
   NumNotifications = DB_QuerySELECT (&mysql_res,"can not get your notifications",
				      "SELECT NotifyEvent,FromUsrCod,InsCod,CtrCod,DegCod,CrsCod,"
				      "Cod,UNIX_TIMESTAMP(TimeNotif),Status"
				      " FROM notif"
				      " WHERE ToUsrCod=%ld%s"
				      " ORDER BY TimeNotif DESC",
				      Gbl.Usrs.Me.UsrDat.UsrCod,SubQuery);

   /***** Contextual menu *****/
   Mnu_ContextMenuBegin ();
   Ntf_WriteFormAllNotifications (AllNotifications);	// Show all notifications
   if (NumNotifications)	// TODO: Show message only when I don't have notificacions at all
      Lay_PutContextualLinkIconText (ActMrkNtfSee,NULL,
                                     NULL,NULL,
				     "eye.svg",
				     Txt_Mark_all_NOTIFICATIONS_as_read);	// Mark notifications as read
   Lay_PutContextualLinkIconText (ActReqEdiSet,Ntf_NOTIFICATIONS_ID,
                                  NULL,NULL,
				  "cog.svg",
				  Txt_Settings);	// Change notification settings
   Lay_PutContextualLinkIconText (ActSeeMai,NULL,
                                  NULL,NULL,
				  "envelope.svg",
				  Txt_Domains);		// View allowed mail domains
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
      HTM_TR_Begin (NULL);

      HTM_TH (1,2,"LM",Txt_Event);
      HTM_TH (1,1,"LM",Txt_MSG_From);
      HTM_TH (1,1,"LM",Txt_Location);
      HTM_TH (1,1,"CM",Txt_Date);
      HTM_TH (1,1,"LM",Txt_Email);

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
         Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat,Usr_DONT_GET_PREFS);		// Get user's data from the database

         /* Get institution code (row[2]) */
         Hie.Ins.InsCod = Str_ConvertStrCodToLongCod (row[2]);
         Ins_GetDataOfInstitutionByCod (&Hie.Ins);

          /* Get centre code (row[3]) */
         Hie.Ctr.CtrCod = Str_ConvertStrCodToLongCod (row[3]);
         Ctr_GetDataOfCentreByCod (&Hie.Ctr);

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
            Lay_ShowErrorAndExit ("Wrong notification status.");
         StatusTxt = Ntf_GetStatusTxtFromStatusBits (Status);

         if (Status & Ntf_STATUS_BIT_REMOVED)	// The source of the notification was removed
           {
            ClassBackground   = "MSG_TIT_BG_REM";
            ClassText         = "MSG_TIT_REM";
            ClassLink         = "BT_LINK MSG_TIT_REM";
            ClassAuthorBg     = "MSG_AUT_BG_REM";
            PutLink = false;
           }
         else if (Status & Ntf_STATUS_BIT_READ)	// I have already seen the source of the notification
           {
            ClassBackground   = "MSG_TIT_BG";
            ClassText         = "MSG_TIT";
            ClassLink         = "BT_LINK LT MSG_TIT";
            ClassAuthorBg     = "MSG_AUT_BG";
            PutLink = true;
           }
         else					// I have not seen the source of the notification
           {
            ClassBackground   = "MSG_TIT_BG_NEW";
            ClassText         = "MSG_TIT_NEW";
            ClassLink         = "BT_LINK LT MSG_TIT_NEW";
            ClassAuthorBg     = "MSG_AUT_BG_NEW";
            PutLink = true;
           }

         /***** Write row for this notification *****/
	 /* Write event icon */
         HTM_TR_Begin (NULL);

         HTM_TD_Begin ("class=\"%s LT\" style=\"width:25px;\"",ClassBackground);
         if (PutLink)
            PutLink = Ntf_StartFormGoToAction (NotifyEvent,Hie.Crs.CrsCod,&UsrDat,Cod,&Forums);

         if (PutLink)
           {
            Ico_PutIconLink (Ntf_Icons[NotifyEvent],
        	             Txt_NOTIFY_EVENTS_SINGULAR[NotifyEvent]);
	    Frm_EndForm ();
           }
         else
            Ico_PutIconOff (Ntf_Icons[NotifyEvent],
        	            Txt_NOTIFY_EVENTS_SINGULAR[NotifyEvent]);
         HTM_TD_End ();

         /* Write event type */
         HTM_TD_Begin ("class=\"%s LT\"",ClassBackground);
         if (PutLink)
           {
            PutLink = Ntf_StartFormGoToAction (NotifyEvent,Hie.Crs.CrsCod,&UsrDat,Cod,&Forums);
            HTM_BUTTON_SUBMIT_Begin (Txt_NOTIFY_EVENTS_SINGULAR[NotifyEvent],ClassLink,NULL);
            HTM_Txt (Txt_NOTIFY_EVENTS_SINGULAR[NotifyEvent]);
            HTM_BUTTON_End ();
            Frm_EndForm ();
           }
         else
           {
            HTM_SPAN_Begin ("class=\"%s\"",ClassText);
            HTM_Txt (Txt_NOTIFY_EVENTS_SINGULAR[NotifyEvent]);
            HTM_SPAN_End ();
           }
         HTM_TD_End ();

         /* Write user (from) */
	 HTM_TD_Begin ("class=\"%s LT\"",ClassAuthorBg);
	 Msg_WriteMsgAuthor (&UsrDat,true,NULL);
	 HTM_TD_End ();

         /* Write location */
         HTM_TD_Begin ("class=\"%s LT\"",ClassBackground);
         if (NotifyEvent == Ntf_EVENT_FORUM_POST_COURSE ||
             NotifyEvent == Ntf_EVENT_FORUM_REPLY)
           {
            if (PutLink)
               PutLink = Ntf_StartFormGoToAction (NotifyEvent,Hie.Crs.CrsCod,&UsrDat,Cod,&Forums);

            if (PutLink)
               HTM_BUTTON_SUBMIT_Begin (Txt_NOTIFY_EVENTS_SINGULAR[NotifyEvent],ClassLink,NULL);
            else
               HTM_SPAN_Begin ("class=\"%s\"",ClassText);
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
               PutLink = Ntf_StartFormGoToAction (NotifyEvent,Hie.Crs.CrsCod,&UsrDat,Cod,&Forums);

            if (PutLink)
               HTM_BUTTON_SUBMIT_Begin (Txt_NOTIFY_EVENTS_SINGULAR[NotifyEvent],ClassLink,NULL);
            else
               HTM_SPAN_Begin ("class=\"%s\"",ClassText);

            if (Hie.Crs.CrsCod > 0)
               HTM_TxtF ("%s:&nbsp;%s",Txt_Course,Hie.Crs.ShrtName);
            else if (Hie.Deg.DegCod > 0)
               HTM_TxtF ("%s:&nbsp;%s",Txt_Degree,Hie.Deg.ShrtName);
            else if (Hie.Ctr.CtrCod > 0)
               HTM_TxtF ("%s:&nbsp;%s",Txt_Centre,Hie.Ctr.ShrtName);
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
         Msg_WriteMsgDate (DateTimeUTC,ClassBackground);

         /* Write status (sent by email / pending to be sent by email) */
         HTM_TD_Begin ("class=\"%s LT\"",ClassBackground);
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

            HTM_TD_Begin ("colspan=\"4\" class=\"DAT LT\" style=\"padding-bottom:12px;\"");
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
   Ntf_UpdateMyLastAccessToNotifications ();
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
// Return the value of Gbl.Form.Inside (true if form is started)

static bool Ntf_StartFormGoToAction (Ntf_NotifyEvent_t NotifyEvent,
                                     long CrsCod,struct UsrData *UsrDat,long Cod,
                                     const struct For_Forums *Forums)
  {
   extern const Act_Action_t For_ActionsSeeFor[For_NUM_TYPES_FORUM];
   struct FileMetadata FileMetadata;
   long InsCod = -1L;
   long CtrCod = -1L;
   long DegCod = -1L;
   long GrpCod = -1L;
   Act_Action_t Action = ActUnk;				// Initialized to avoid warning

   /***** Parameters depending on the type of event *****/
   switch (NotifyEvent)
     {
      case Ntf_EVENT_DOCUMENT_FILE:
      case Ntf_EVENT_TEACHERS_FILE:
      case Ntf_EVENT_SHARED_FILE:
      case Ntf_EVENT_MARKS_FILE:
         Action = ActUnk;
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
      case Ntf_EVENT_TIMELINE_COMMENT:
      case Ntf_EVENT_TIMELINE_FAV:
      case Ntf_EVENT_TIMELINE_SHARE:
      case Ntf_EVENT_TIMELINE_MENTION:
	 // Cod is the code of the social publishing
         Frm_BeginForm (ActSeeGblTL);
	 TL_Pub_PutHiddenParamPubCod (Cod);
         Usr_PutParamUsrCodEncrypted (UsrDat->EnUsrCod);
         Ntf_PutHiddenParamNotifyEvent (NotifyEvent);
	 break;
      case Ntf_EVENT_FOLLOWER:
         if (UsrDat->EnUsrCod[0])	// User's code found ==>
					// go to user's public profile
           {
            Frm_BeginForm (ActSeeOthPubPrf);
            /* Put param to go to follower's profile */
            Usr_PutParamUsrCodEncrypted (UsrDat->EnUsrCod);
           }
         else	// No user's code found ==> go to see my followers
            Frm_BeginForm (ActSeeFlr);
	 break;
      case Ntf_EVENT_FORUM_POST_COURSE:
      case Ntf_EVENT_FORUM_REPLY:
	 Frm_BeginForm (For_ActionsSeeFor[Forums->Forum.Type]);
	 For_PutAllHiddenParamsForum (1,	// Page of threads = first
                                      1,	// Page of posts   = first
                                      Forums->ForumSet,
				      Forums->ThreadsOrder,
				      Forums->Forum.Location,
				      Forums->Thread.Selected,
				      -1L);
	 break;
      case Ntf_EVENT_NOTICE:
         Frm_BeginForm (ActSeeOneNot);
	 Not_PutHiddenParamNotCod (Cod);
	 break;
      case Ntf_EVENT_MESSAGE:
         Frm_BeginForm (ActExpRcvMsg);
	 Msg_PutHiddenParamMsgCod (Cod);
	 break;
      default:
         Frm_BeginForm (Ntf_DefaultActions[NotifyEvent]);
	 break;
     }

   /***** Parameter to go to another course/degree/centre/institution *****/
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
      else if (CtrCod > 0)				// Centre specified
	{
	 if (CtrCod != Gbl.Hierarchy.Ctr.CtrCod)	// Not the current centre
	    Ctr_PutParamCtrCod (CtrCod);		// Go to another centre
	}
      else if (InsCod > 0)				// Institution specified
	{
	 if (InsCod != Gbl.Hierarchy.Ins.InsCod)	// Not the current institution
	    Ins_PutParamInsCod (InsCod);		// Go to another institution
	}
     }

   return Gbl.Form.Inside;
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
      case Ntf_EVENT_EXAM_ANNOUNCEMENT:
         ExaAnn_GetSummaryAndContentExamAnn (SummaryStr,ContentStr,Cod,GetContent);
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
      case Ntf_EVENT_TIMELINE_COMMENT:
      case Ntf_EVENT_TIMELINE_FAV:
      case Ntf_EVENT_TIMELINE_SHARE:
      case Ntf_EVENT_TIMELINE_MENTION:
	 // Cod is the code of the social publishing
	 TL_Ntf_GetNotifPublication (SummaryStr,ContentStr,Cod,GetContent);
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
            Msg_SetReceivedMsgAsOpen (Cod,UsrCod);
         break;
      case Ntf_EVENT_SURVEY:
         Svy_GetNotifSurvey (SummaryStr,ContentStr,Cod,GetContent);
         break;
     }
  }

/*****************************************************************************/
/********************** Set possible notification as seen ********************/
/*****************************************************************************/

void Ntf_MarkNotifAsSeen (Ntf_NotifyEvent_t NotifyEvent,long Cod,long CrsCod,long ToUsrCod)
  {
   /***** Set notification as seen by me *****/
   if (ToUsrCod > 0)	// If the user code is specified
     {
      if (Cod > 0)		// Set only one notification
				// for the user as seen
         DB_QueryUPDATE ("can not set notification(s) as seen",
			 "UPDATE notif SET Status=(Status | %u)"
			 " WHERE ToUsrCod=%ld AND NotifyEvent=%u AND Cod=%ld",
                         (unsigned) Ntf_STATUS_BIT_READ,
                         ToUsrCod,(unsigned) NotifyEvent,Cod);
      else if (CrsCod > 0)	// Set all notifications of this type
				// in the current course for the user as seen
         DB_QueryUPDATE ("can not set notification(s) as seen",
			 "UPDATE notif SET Status=(Status | %u)"
			 " WHERE ToUsrCod=%ld AND NotifyEvent=%u AND CrsCod=%ld",
                         (unsigned) Ntf_STATUS_BIT_READ,
                         ToUsrCod,(unsigned) NotifyEvent,Gbl.Hierarchy.Crs.CrsCod);
      else			// Set all notifications of this type
				// for the user as seen
         DB_QueryUPDATE ("can not set notification(s) as seen",
			 "UPDATE notif SET Status=(Status | %u)"
			 " WHERE ToUsrCod=%ld AND NotifyEvent=%u",
                         (unsigned) Ntf_STATUS_BIT_READ,
                         ToUsrCod,(unsigned) NotifyEvent);
     }
  }

/*****************************************************************************/
/******************* Set possible notifications as removed *******************/
/*****************************************************************************/

void Ntf_MarkNotifAsRemoved (Ntf_NotifyEvent_t NotifyEvent,long Cod)
  {
   /***** Set notification as removed *****/
   DB_QueryUPDATE ("can not set notification(s) as removed",
		   "UPDATE notif SET Status=(Status | %u)"
		   " WHERE NotifyEvent=%u AND Cod=%ld",
	           (unsigned) Ntf_STATUS_BIT_REMOVED,
	           (unsigned) NotifyEvent,Cod);
  }

/*****************************************************************************/
/******************** Set possible notification as removed *******************/
/*****************************************************************************/

void Ntf_MarkNotifToOneUsrAsRemoved (Ntf_NotifyEvent_t NotifyEvent,long Cod,long ToUsrCod)
  {
   /***** Set notification as removed *****/
   if (Cod > 0)	// Set only one notification as removed
      DB_QueryUPDATE ("can not set notification(s) as removed",
		      "UPDATE notif SET Status=(Status | %u)"
		      " WHERE ToUsrCod=%ld AND NotifyEvent=%u AND Cod=%ld",
	              (unsigned) Ntf_STATUS_BIT_REMOVED,
	              ToUsrCod,(unsigned) NotifyEvent,
		      Cod);
   else		// Set all notifications of this type,
		// in the current course for the user, as removed
      DB_QueryUPDATE ("can not set notification(s) as removed",
		      "UPDATE notif SET Status=(Status | %u)"
		      " WHERE ToUsrCod=%ld AND NotifyEvent=%u AND CrsCod=%ld",
	              (unsigned) Ntf_STATUS_BIT_REMOVED,
	              ToUsrCod,(unsigned) NotifyEvent,
		      Gbl.Hierarchy.Crs.CrsCod);
  }

/*****************************************************************************/
/*********** Set possible notifications from a course as removed *************/
/*****************************************************************************/
// This function should be called when a course is removed
// because notifications from this course will not be available after course removing.
// However, notifications about new messages should not be removed
// because the messages will remain available

void Ntf_MarkNotifInCrsAsRemoved (long ToUsrCod,long CrsCod)
  {
   /***** Set all notifications from the course as removed,
          except notifications about new messages *****/
   if (ToUsrCod > 0)	// If the user code is specified
      DB_QueryUPDATE ("can not set notification(s) as removed",
		      "UPDATE notif SET Status=(Status | %u)"
		      " WHERE ToUsrCod=%ld"
		      " AND CrsCod=%ld"
		      " AND NotifyEvent<>%u",	// messages will remain available
	              (unsigned) Ntf_STATUS_BIT_REMOVED,
	              ToUsrCod,
	              CrsCod,(unsigned) Ntf_EVENT_MESSAGE);
   else			// User code not specified ==> any user
      DB_QueryUPDATE ("can not set notification(s) as removed",
		      "UPDATE notif SET Status=(Status | %u)"
		      " WHERE CrsCod=%ld"
		      " AND NotifyEvent<>%u",	// messages will remain available
	              (unsigned) Ntf_STATUS_BIT_REMOVED,
	              CrsCod,(unsigned) Ntf_EVENT_MESSAGE);
  }

/*****************************************************************************/
/*********** Mark possible notifications of one file as removed **************/
/*****************************************************************************/

void Ntf_MarkNotifOneFileAsRemoved (const char *Path)
  {
   extern const Brw_FileBrowser_t Brw_FileBrowserForDB_files[Brw_NUM_TYPES_FILE_BROWSER];
   Brw_FileBrowser_t FileBrowser = Brw_FileBrowserForDB_files[Gbl.FileBrowser.Type];
   long FilCod;
   Ntf_NotifyEvent_t NotifyEvent;

   switch (FileBrowser)
     {
      case Brw_ADMI_DOC_CRS:
      case Brw_ADMI_DOC_GRP:
      case Brw_ADMI_TCH_CRS:
      case Brw_ADMI_TCH_GRP:
      case Brw_ADMI_SHR_CRS:
      case Brw_ADMI_SHR_GRP:
      case Brw_ADMI_MRK_CRS:
      case Brw_ADMI_MRK_GRP:
         /***** Get file code *****/
	 FilCod = Brw_GetFilCodByPath (Path,false);	// Any file, public or not
	 if (FilCod > 0)
	   {
	    /***** Set notification as removed *****/
	    switch (FileBrowser)
	      {
	       case Brw_ADMI_DOC_CRS:
	       case Brw_ADMI_DOC_GRP:
		  NotifyEvent = Ntf_EVENT_DOCUMENT_FILE;
		  break;
	       case Brw_ADMI_TCH_CRS:
	       case Brw_ADMI_TCH_GRP:
		  NotifyEvent = Ntf_EVENT_TEACHERS_FILE;
		  break;
	       case Brw_ADMI_SHR_CRS:
	       case Brw_ADMI_SHR_GRP:
		  NotifyEvent = Ntf_EVENT_SHARED_FILE;
		  break;
	       case Brw_ADMI_MRK_CRS:
	       case Brw_ADMI_MRK_GRP:
		  NotifyEvent = Ntf_EVENT_MARKS_FILE;
		  break;
	       default:
		  return;
	      }
            Ntf_MarkNotifAsRemoved (NotifyEvent,FilCod);
	   }
         break;
      default:
	 break;
     }
  }

/*****************************************************************************/
/*** Mark possible notifications involving children of a folder as removed ***/
/*****************************************************************************/

void Ntf_MarkNotifChildrenOfFolderAsRemoved (const char *Path)
  {
   extern const Brw_FileBrowser_t Brw_FileBrowserForDB_files[Brw_NUM_TYPES_FILE_BROWSER];
   Brw_FileBrowser_t FileBrowser = Brw_FileBrowserForDB_files[Gbl.FileBrowser.Type];
   long Cod = Brw_GetCodForFiles ();
   Ntf_NotifyEvent_t NotifyEvent;

   switch (FileBrowser)
     {
      case Brw_ADMI_DOC_CRS:
      case Brw_ADMI_DOC_GRP:
      case Brw_ADMI_TCH_CRS:
      case Brw_ADMI_TCH_GRP:
      case Brw_ADMI_SHR_CRS:
      case Brw_ADMI_SHR_GRP:
      case Brw_ADMI_MRK_CRS:
      case Brw_ADMI_MRK_GRP:
         /***** Set notification as removed *****/
	 switch (FileBrowser)
	   {
	    case Brw_ADMI_DOC_CRS:
	    case Brw_ADMI_DOC_GRP:
	       NotifyEvent = Ntf_EVENT_DOCUMENT_FILE;
	       break;
	    case Brw_ADMI_TCH_CRS:
	    case Brw_ADMI_TCH_GRP:
	       NotifyEvent = Ntf_EVENT_TEACHERS_FILE;
	       break;
	    case Brw_ADMI_SHR_CRS:
	    case Brw_ADMI_SHR_GRP:
	       NotifyEvent = Ntf_EVENT_SHARED_FILE;
	       break;
	    case Brw_ADMI_MRK_CRS:
	    case Brw_ADMI_MRK_GRP:
	       NotifyEvent = Ntf_EVENT_MARKS_FILE;
	       break;
	    default:
	       return;
	   }
	 DB_QueryUPDATE ("can not set notification(s) as removed",
			 "UPDATE notif SET Status=(Status | %u)"
			 " WHERE NotifyEvent=%u AND Cod IN"
			 " (SELECT FilCod FROM files"
			 " WHERE FileBrowser=%u AND Cod=%ld"
			 " AND Path LIKE '%s/%%')",
		         (unsigned) Ntf_STATUS_BIT_REMOVED,
		         (unsigned) NotifyEvent,
		         (unsigned) FileBrowser,Cod,
		         Path);
         break;
      default:
	 break;
     }
  }

/*****************************************************************************/
/******* Set all possible notifications of files in a group as removed *******/
/*****************************************************************************/

void Ntf_MarkNotifFilesInGroupAsRemoved (long GrpCod)
  {
   /***** Set notifications as removed *****/
   DB_QueryUPDATE ("can not set notification(s) as removed",
		   "UPDATE notif SET Status=(Status | %u)"
		   " WHERE NotifyEvent IN (%u,%u,%u,%u) AND Cod IN"
		   " (SELECT FilCod FROM files"
		   " WHERE FileBrowser IN (%u,%u,%u,%u) AND Cod=%ld)",
	           (unsigned) Ntf_STATUS_BIT_REMOVED,
	           (unsigned) Ntf_EVENT_DOCUMENT_FILE,
	           (unsigned) Ntf_EVENT_TEACHERS_FILE,
	           (unsigned) Ntf_EVENT_SHARED_FILE,
	           (unsigned) Ntf_EVENT_MARKS_FILE,
	           (unsigned) Brw_ADMI_DOC_GRP,
	           (unsigned) Brw_ADMI_TCH_GRP,
	           (unsigned) Brw_ADMI_SHR_GRP,
	           (unsigned) Brw_ADMI_MRK_GRP,
	           GrpCod);
  }

/*****************************************************************************/
/********** Get a list with user's codes of all users to be notified *********/
/********** about an event, and notify them                          *********/
/*****************************************************************************/
// Return the number of users notified by email

unsigned Ntf_StoreNotifyEventsToAllUsrs (Ntf_NotifyEvent_t NotifyEvent,long Cod)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRow;
   unsigned long NumRows = 0;	// Initialized to avoid warning
   struct UsrData UsrDat;
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
         switch (Gbl.FileBrowser.Type)
           {
            case Brw_ADMI_DOC_CRS:
            case Brw_ADMI_SHR_CRS:
            case Brw_ADMI_MRK_CRS:	// Notify all users in course except me
               NumRows = DB_QuerySELECT (&mysql_res,"can not get users"
					      " to be notified",
					 "SELECT UsrCod FROM crs_usr"
					 " WHERE CrsCod=%ld"
					 " AND UsrCod<>%ld",
					 Gbl.Hierarchy.Crs.CrsCod,
					 Gbl.Usrs.Me.UsrDat.UsrCod);
               break;
            case Brw_ADMI_TCH_CRS:	// Notify all teachers in course except me
               NumRows = DB_QuerySELECT (&mysql_res,"can not get users"
					      " to be notified",
					 "SELECT UsrCod FROM crs_usr"
					 " WHERE CrsCod=%ld"
					 " AND UsrCod<>%ld"
					 " AND Role=%u",	// Notify teachers only
					 Gbl.Hierarchy.Crs.CrsCod,
					 Gbl.Usrs.Me.UsrDat.UsrCod,
					 (unsigned) Rol_TCH);
               break;
            case Brw_ADMI_DOC_GRP:
            case Brw_ADMI_SHR_GRP:
            case Brw_ADMI_MRK_GRP:	// Notify all users in group except me
               NumRows = DB_QuerySELECT (&mysql_res,"can not get users"
					      " to be notified",
					 "SELECT UsrCod FROM crs_grp_usr"
					 " WHERE crs_grp_usr.GrpCod=%ld"
					 " AND crs_grp_usr.UsrCod<>%ld",
					 Gbl.Crs.Grps.GrpCod,
					 Gbl.Usrs.Me.UsrDat.UsrCod);
               break;
            case Brw_ADMI_TCH_GRP:	// Notify all teachers in group except me
               NumRows = DB_QuerySELECT (&mysql_res,"can not get users"
					      " to be notified",
				         "SELECT crs_grp_usr.UsrCod"
					 " FROM crs_grp_usr,crs_grp,crs_grp_types,crs_usr"
					 " WHERE crs_grp_usr.GrpCod=%ld"
					 " AND crs_grp_usr.UsrCod<>%ld"
					 " AND crs_grp_usr.GrpCod=crs_grp.GrpCod"
					 " AND crs_grp.GrpTypCod=crs_grp_types.GrpTypCod"
					 " AND crs_grp_types.CrsCod=crs_usr.CrsCod"
					 " AND crs_usr.Role=%u",	// Notify teachers only
					 Gbl.Crs.Grps.GrpCod,
					 Gbl.Usrs.Me.UsrDat.UsrCod,
					 (unsigned) Rol_TCH);
               break;
            default:	// This function should not be called in other cases
               return 0;
           }
         break;
      case Ntf_EVENT_ASSIGNMENT:
         // 1. If the assignment is available for the whole course ==> get all users enroled in the course except me
         // 2. If the assignment is available only for some groups ==> get all users who belong to any of the groups except me
         // Cases 1 and 2 are mutually exclusive, so the union returns the case 1 or 2
         NumRows = DB_QuerySELECT (&mysql_res,"can not get users"
					      " to be notified",
				   "(SELECT crs_usr.UsrCod"
				   " FROM assignments,crs_usr"
				   " WHERE assignments.AsgCod=%ld"
				   " AND assignments.AsgCod NOT IN"
				   " (SELECT AsgCod FROM asg_grp WHERE AsgCod=%ld)"
				   " AND assignments.CrsCod=crs_usr.CrsCod"
				   " AND crs_usr.UsrCod<>%ld)"
				   " UNION "
				   "(SELECT DISTINCT crs_grp_usr.UsrCod"
				   " FROM asg_grp,crs_grp_usr"
				   " WHERE asg_grp.AsgCod=%ld"
				   " AND asg_grp.GrpCod=crs_grp_usr.GrpCod"
				   " AND crs_grp_usr.UsrCod<>%ld)",
				   Cod,Cod,Gbl.Usrs.Me.UsrDat.UsrCod,
				   Cod,Gbl.Usrs.Me.UsrDat.UsrCod);
         break;
      case Ntf_EVENT_EXAM_ANNOUNCEMENT:
      case Ntf_EVENT_NOTICE:
         NumRows = DB_QuerySELECT (&mysql_res,"can not get users"
					      " to be notified",
				   "SELECT UsrCod FROM crs_usr"
				   " WHERE CrsCod=%ld AND UsrCod<>%ld",
				   Gbl.Hierarchy.Crs.CrsCod,
				   Gbl.Usrs.Me.UsrDat.UsrCod);
         break;
      case Ntf_EVENT_ENROLMENT_STD:	// This function should not be called in this case
      case Ntf_EVENT_ENROLMENT_NET:	// This function should not be called in this case
      case Ntf_EVENT_ENROLMENT_TCH:	// This function should not be called in this case
         return 0;
      case Ntf_EVENT_ENROLMENT_REQUEST:
	 if (Usr_GetNumUsrsInCrss (Hie_Lvl_CRS,Gbl.Hierarchy.Crs.CrsCod,
				   1 << Rol_TCH))
	    // If this course has teachers ==> send notification to teachers
	    NumRows = DB_QuerySELECT (&mysql_res,"can not get users"
					         " to be notified",
				      "SELECT UsrCod FROM crs_usr"
				      " WHERE CrsCod=%ld"
				      " AND UsrCod<>%ld"
				      " AND Role=%u",	// Notify teachers only
				      Gbl.Hierarchy.Crs.CrsCod,
				      Gbl.Usrs.Me.UsrDat.UsrCod,
				      (unsigned) Rol_TCH);
	 else	// Course without teachers
	    // If this course has no teachers
	    // and I want to be a teacher (checked before calling this function
	    // to not send requests to be a student to admins)
	    // ==> send notification to administrators or superusers
	    NumRows = DB_QuerySELECT (&mysql_res,"can not get users"
					         " to be notified",
				      "SELECT UsrCod FROM admin"
				      " WHERE (Scope='%s'"
				      " OR (Scope='%s' AND Cod=%ld)"
				      " OR (Scope='%s' AND Cod=%ld)"
				      " OR (Scope='%s' AND Cod=%ld))"
				      " AND UsrCod<>%ld",
				      Sco_GetDBStrFromScope (Hie_Lvl_SYS),
				      Sco_GetDBStrFromScope (Hie_Lvl_INS),Gbl.Hierarchy.Ins.InsCod,
				      Sco_GetDBStrFromScope (Hie_Lvl_CTR),Gbl.Hierarchy.Ctr.CtrCod,
				      Sco_GetDBStrFromScope (Hie_Lvl_DEG),Gbl.Hierarchy.Deg.DegCod,
				      Gbl.Usrs.Me.UsrDat.UsrCod);
         break;
      case Ntf_EVENT_TIMELINE_COMMENT:	// New comment to one of my social notes or comments
         // Cod is the code of the social publishing
	 NumRows = DB_QuerySELECT (&mysql_res,"can not get users"
					      " to be notified",
				   "SELECT DISTINCT(PublisherCod) FROM tl_pubs"
				   " WHERE NotCod="
				   "(SELECT NotCod FROM tl_pubs"
				   " WHERE PubCod=%ld)"
				   " AND PublisherCod<>%ld",
				   Cod,Gbl.Usrs.Me.UsrDat.UsrCod);
         break;
      case Ntf_EVENT_TIMELINE_FAV:		// New favourite to one of my social notes or comments
      case Ntf_EVENT_TIMELINE_SHARE:		// New sharing of one of my social notes
      case Ntf_EVENT_TIMELINE_MENTION:
      case Ntf_EVENT_FOLLOWER:
	 // This function should not be called in these cases
         return 0;
      case Ntf_EVENT_FORUM_POST_COURSE:
	 // Check if forum is for users or for all users in the course
	 For_GetForumTypeAndLocationOfAPost (Cod,&ForumSelected);

	 switch (ForumSelected.Type)
	   {
	    case For_FORUM_COURSE_USRS:
	       NumRows = DB_QuerySELECT (&mysql_res,"can not get users"
					            " to be notified",
					 "SELECT UsrCod FROM crs_usr"
					 " WHERE CrsCod=%ld AND UsrCod<>%ld",
					 Gbl.Hierarchy.Crs.CrsCod,
					 Gbl.Usrs.Me.UsrDat.UsrCod);
	       break;
	    case For_FORUM_COURSE_TCHS:
	       NumRows = DB_QuerySELECT (&mysql_res,"can not get users"
					            " to be notified",
					 "SELECT UsrCod FROM crs_usr"
					 " WHERE CrsCod=%ld AND Role=%u AND UsrCod<>%ld",
					 Gbl.Hierarchy.Crs.CrsCod,
					 (unsigned) Rol_TCH,
					 Gbl.Usrs.Me.UsrDat.UsrCod);
	       break;
	    default:
	       return 0;
	   }
         break;
      case Ntf_EVENT_FORUM_REPLY:
         NumRows = DB_QuerySELECT (&mysql_res,"can not get users"
					      " to be notified",
				   "SELECT DISTINCT(UsrCod) FROM forum_post"
				   " WHERE ThrCod = (SELECT ThrCod FROM forum_post"
				   " WHERE PstCod=%ld)"
				   " AND UsrCod<>%ld",
				   Cod,Gbl.Usrs.Me.UsrDat.UsrCod);
         break;
      case Ntf_EVENT_MESSAGE:		// This function should not be called in this case
	 return 0;
      case Ntf_EVENT_SURVEY:	// Only surveys for a course are notified, not surveys for a degree or global
         // 1. If the survey is available for the whole course ==> get users enroled in the course whose role is available in survey, except me
         // 2. If the survey is available only for some groups ==> get users who belong to any of the groups and whose role is available in survey, except me
         // Cases 1 and 2 are mutually exclusive, so the union returns the case 1 or 2
         NumRows = DB_QuerySELECT (&mysql_res,"can not get users"
					      " to be notified",
				   "(SELECT crs_usr.UsrCod"
				   " FROM surveys,crs_usr"
				   " WHERE surveys.SvyCod=%ld"
				   " AND surveys.SvyCod NOT IN"
				   " (SELECT SvyCod FROM svy_grp WHERE SvyCod=%ld)"
				   " AND surveys.Scope='%s' AND surveys.Cod=crs_usr.CrsCod"
				   " AND crs_usr.UsrCod<>%ld"
				   " AND (surveys.Roles&(1<<crs_usr.Role))<>0)"
				   " UNION "
				   "(SELECT DISTINCT crs_grp_usr.UsrCod"
				   " FROM svy_grp,crs_grp_usr,surveys,crs_usr"
				   " WHERE svy_grp.SvyCod=%ld"
				   " AND svy_grp.GrpCod=crs_grp_usr.GrpCod"
				   " AND crs_grp_usr.UsrCod=crs_usr.UsrCod"
				   " AND crs_grp_usr.UsrCod<>%ld"
				   " AND svy_grp.SvyCod=surveys.SvyCod"
				   " AND surveys.Scope='%s' AND surveys.Cod=crs_usr.CrsCod"
				   " AND (surveys.Roles&(1<<crs_usr.Role))<>0)",
				   Cod,
				   Cod,
				   Sco_GetDBStrFromScope (Hie_Lvl_CRS),
				   Gbl.Usrs.Me.UsrDat.UsrCod,
				   Cod,
				   Gbl.Usrs.Me.UsrDat.UsrCod,
				   Sco_GetDBStrFromScope (Hie_Lvl_CRS));
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
	 case For_FORUM_CENTRE_USRS:
	 case For_FORUM_CENTRE_TCHS:
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

   if (NumRows) // Users found
     {
      /***** Initialize structure with user's data *****/
      Usr_UsrDataConstructor (&UsrDat);

      /***** Notify the users one by one *****/
      for (NumRow = 0;
	   NumRow < NumRows;
	   NumRow++)
	{
         /* Get next user */
         row = mysql_fetch_row (mysql_res);

         /* Get user code */
         UsrDat.UsrCod = Str_ConvertStrCodToLongCod (row[0]);

	 if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat,Usr_DONT_GET_PREFS))		// Get user's data from the database
            if ((UsrDat.NtfEvents.CreateNotif & NotifyEventMask))	// Create notification
              {
	       if ((UsrDat.NtfEvents.SendEmail & NotifyEventMask))	// Send notification by email
		 {
		  Ntf_StoreNotifyEventToOneUser (NotifyEvent,&UsrDat,Cod,
						 (Ntf_Status_t) Ntf_STATUS_BIT_EMAIL,
						 InsCod,CtrCod,DegCod,CrsCod);
		  NumUsrsToBeNotifiedByEMail++;
		 }
	       else							// Don't send notification by email
		  Ntf_StoreNotifyEventToOneUser (NotifyEvent,&UsrDat,Cod,(Ntf_Status_t) 0,
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
/************** Store a notify event to one user into database ***************/
/*****************************************************************************/

void Ntf_StoreNotifyEventToOneUser (Ntf_NotifyEvent_t NotifyEvent,
                                    struct UsrData *UsrDat,
                                    long Cod,Ntf_Status_t Status,
                                    long InsCod,long CtrCod,long DegCod,long CrsCod)
  {
   /***** Store notify event *****/
   DB_QueryINSERT ("can not create new notification event",
		   "INSERT INTO notif"
		   " (NotifyEvent,ToUsrCod,FromUsrCod,"
		   "InsCod,CtrCod,DegCod,CrsCod,Cod,TimeNotif,Status)"
		   " VALUES"
		   " (%u,%ld,%ld,"
		   "%ld,%ld,%ld,%ld,%ld,NOW(),%u)",
	           (unsigned) NotifyEvent,
		   UsrDat->UsrCod,Gbl.Usrs.Me.UsrDat.UsrCod,
	           InsCod,CtrCod,DegCod,CrsCod,Cod,(unsigned) Status);
  }

/*****************************************************************************/
/*************** Reset my number of new notifications to 0 *******************/
/*****************************************************************************/

static void Ntf_UpdateMyLastAccessToNotifications (void)
  {
   /***** Reset to 0 my number of new notifications *****/
   DB_QueryUPDATE ("can not update last access to notifications",
		   "UPDATE usr_last SET LastAccNotif=NOW()"
		   " WHERE UsrCod=%ld",
                   Gbl.Usrs.Me.UsrDat.UsrCod);
  }

/*****************************************************************************/
/***************** Send all pending notifications by email *******************/
/*****************************************************************************/

void Ntf_SendPendingNotifByEMailToAllUsrs (void)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;
   unsigned long NumRow;
   struct UsrData UsrDat;
   unsigned NumNotif;
   unsigned NumTotalNotif = 0;
   unsigned NumMails;
   unsigned NumTotalMails = 0;

   /***** Get users who must be notified from database ******/
   //  (Status & Ntf_STATUS_BIT_EMAIL) &&
   // !(Status & Ntf_STATUS_BIT_SENT) &&
   // !(Status & (Ntf_STATUS_BIT_READ | Ntf_STATUS_BIT_REMOVED))
   if ((NumRows = DB_QuerySELECT (&mysql_res,"can not get users"
					     " who must be notified",
				  "SELECT DISTINCT ToUsrCod FROM notif"
				  " WHERE TimeNotif<FROM_UNIXTIME(UNIX_TIMESTAMP()-%lu)"
				  " AND (Status & %u)<>0"
				  " AND (Status & %u)=0"
				  " AND (Status & %u)=0",
				  Cfg_TIME_TO_SEND_PENDING_NOTIF,
				  (unsigned) Ntf_STATUS_BIT_EMAIL,
				  (unsigned) Ntf_STATUS_BIT_SENT,
				  (unsigned) (Ntf_STATUS_BIT_READ | Ntf_STATUS_BIT_REMOVED)))) // Events found
     {
      /***** Initialize structure with user's data *****/
      Usr_UsrDataConstructor (&UsrDat);

      /***** Notify the users one by one *****/
      for (NumRow = 0;
	   NumRow < NumRows;
	   NumRow++)
	{
         /* Get next user */
         row = mysql_fetch_row (mysql_res);

         /* Get user code */
         UsrDat.UsrCod = Str_ConvertStrCodToLongCod (row[0]);

         /* Get user's data */
	 if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat,Usr_DONT_GET_PREFS))		// Get user's data from the database
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
   DB_QueryDELETE ("can not remove old notifications",
		   "DELETE LOW_PRIORITY FROM notif"
		   " WHERE TimeNotif<FROM_UNIXTIME(UNIX_TIMESTAMP()-%lu)",
                   Cfg_TIME_TO_DELETE_OLD_NOTIF);
  }

/*****************************************************************************/
/************ Send pending notifications of one user by email ****************/
/*****************************************************************************/

static void Ntf_SendPendingNotifByEMailToOneUsr (struct UsrData *ToUsrDat,unsigned *NumNotif,unsigned *NumMails)
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
   unsigned long NumRow;
   unsigned long NumRows;
   Lan_Language_t ToUsrLanguage;
   struct UsrData FromUsrDat;
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
   *NumNotif = *NumMails = 0;

   if (Mai_CheckIfUsrCanReceiveEmailNotif (ToUsrDat))
     {
      /***** Get pending notifications of this user from database ******/
      NumRows = DB_QuerySELECT (&mysql_res,"can not get pending notifications"
					   " of a user",
				"SELECT NotifyEvent,"
				       "FromUsrCod,"
				       "InsCod,"
				       "CtrCod,"
				       "DegCod,"
				       "CrsCod,"
				       "Cod"
				" FROM notif WHERE ToUsrCod=%ld"
				" AND (Status & %u)<>0"
				" AND (Status & %u)=0"
				" AND (Status & %u)=0"
				" ORDER BY TimeNotif,NotifyEvent",
				ToUsrDat->UsrCod,
				(unsigned) Ntf_STATUS_BIT_EMAIL,
				(unsigned) Ntf_STATUS_BIT_SENT,
				(unsigned) (Ntf_STATUS_BIT_READ | Ntf_STATUS_BIT_REMOVED));

      if (NumRows) // Events found
	{
	 /***** If user has no language, set it to current language *****/
	 ToUsrLanguage = ToUsrDat->Prefs.Language;
	 if (ToUsrLanguage == Lan_LANGUAGE_UNKNOWN)
	    ToUsrLanguage = Gbl.Prefs.Language;

	 /***** Create temporary file for mail content *****/
	 Mai_CreateFileNameMail (FileNameMail,&FileMail);

	 /***** Welcome note *****/
	 Mai_WriteWelcomeNoteEMail (FileMail,ToUsrDat);
	 if (NumRows == 1)
	    fprintf (FileMail,Txt_NOTIFY_EVENTS_There_is_a_new_event_NO_HTML[ToUsrLanguage],
		     Cfg_PLATFORM_SHORT_NAME);
	 else
	    fprintf (FileMail,Txt_NOTIFY_EVENTS_There_are_X_new_events_NO_HTML[ToUsrLanguage],
		     (unsigned) NumRows,Cfg_PLATFORM_SHORT_NAME);
	 fprintf (FileMail,": \n");

	 /***** Initialize structure with origin user's data *****/
	 Usr_UsrDataConstructor (&FromUsrDat);

	 /***** Inform about the events one by one *****/
	 for (NumRow = 0;
	      NumRow < NumRows;
	      NumRow++)
	   {
	    /* Get next event */
	    row = mysql_fetch_row (mysql_res);

	    /* Get event type (row[0]) */
	    NotifyEvent = Ntf_GetNotifyEventFromStr (row[0]);

	    /* Get origin user code (row[1]) */
	    FromUsrDat.UsrCod = Str_ConvertStrCodToLongCod (row[1]);
	    Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&FromUsrDat,Usr_DONT_GET_PREFS);		// Get origin user's data from the database

	    /* Get institution code (row[2]) */
	    Hie.Ins.InsCod = Str_ConvertStrCodToLongCod (row[2]);
	    Ins_GetDataOfInstitutionByCod (&Hie.Ins);

	    /* Get centre code (row[3]) */
	    Hie.Ctr.CtrCod = Str_ConvertStrCodToLongCod (row[3]);
	    Ctr_GetDataOfCentreByCod (&Hie.Ctr);

	    /* Get degree code (row[4]) */
	    Hie.Deg.DegCod = Str_ConvertStrCodToLongCod (row[4]);
	    Deg_GetDataOfDegreeByCod (&Hie.Deg);

	    /* Get course code (row[5]) */
	    Hie.Crs.CrsCod = Str_ConvertStrCodToLongCod (row[5]);
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
	       case Ntf_EVENT_TIMELINE_COMMENT:
	       case Ntf_EVENT_TIMELINE_FAV:
	       case Ntf_EVENT_TIMELINE_SHARE:
	       case Ntf_EVENT_TIMELINE_MENTION:
	       case Ntf_EVENT_FOLLOWER:
		  break;
	       case Ntf_EVENT_DOCUMENT_FILE:
	       case Ntf_EVENT_TEACHERS_FILE:
	       case Ntf_EVENT_SHARED_FILE:
	       case Ntf_EVENT_ASSIGNMENT:
	       case Ntf_EVENT_EXAM_ANNOUNCEMENT:
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
	    Lay_ShowErrorAndExit ("Error when running script to send email.");

	 /***** Remove temporary file *****/
	 unlink (FileNameMail);

	 /***** Update number of notifications, number of mails and statistics *****/
	 ReturnCode = WEXITSTATUS(ReturnCode);
	 if (ReturnCode == 0)	// Message sent successfully
	   {
	    *NumNotif = (unsigned) NumRows;
	    *NumMails = 1;

	    /* Update statistics about notifications */
	    Ntf_UpdateNumNotifSent (Hie.Deg.DegCod,Hie.Crs.CrsCod,NotifyEvent,*NumNotif,*NumMails);
	   }

	 /***** Mark all the pending notifications of this user as 'sent' *****/
	 DB_QueryUPDATE ("can not set pending notifications of a user as sent",
			 "UPDATE notif SET Status=(Status | %u)"
			 " WHERE ToUsrCod=%ld"
			 " AND (Status & %u)<>0 AND (Status & %u)=0  AND (Status & %u)=0",
		         (unsigned) Ntf_STATUS_BIT_SENT,ToUsrDat->UsrCod,
		         (unsigned) Ntf_STATUS_BIT_EMAIL,
		         (unsigned) Ntf_STATUS_BIT_SENT,
		         (unsigned) (Ntf_STATUS_BIT_READ | Ntf_STATUS_BIT_REMOVED));
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
   unsigned long NumRows;

   /***** Get number of notifications sent by email from database *****/
   NumRows = DB_QuerySELECT (&mysql_res,"can not get number of notifications"
				        " sent by email",
			     "SELECT NumEvents,NumMails FROM sta_notif"
			     " WHERE DegCod=%ld AND CrsCod=%ld"
			     " AND NotifyEvent=%u",
			     DegCod,CrsCod,(unsigned) NotifyEvent);

   /***** Get number of rows *****/
   if (NumRows)
     {
      row = mysql_fetch_row (mysql_res);
      if (sscanf (row[0],"%u",NumEvents) != 1)
         Lay_ShowErrorAndExit ("Error when getting number of notifications sent by email.");
      if (sscanf (row[1],"%u",NumMails) != 1)
         Lay_ShowErrorAndExit ("Error when getting number of notifications sent by email.");
     }
   else
      *NumEvents = *NumMails = 0;

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/******************** Update number of notify emails sent ********************/
/*****************************************************************************/

static void Ntf_UpdateNumNotifSent (long DegCod,long CrsCod,
                                    Ntf_NotifyEvent_t NotifyEvent,
                                    unsigned NumEvents,unsigned NumMails)
  {
   unsigned CurrentNumEvents;
   unsigned CurrentNumMails;

   /***** Get number of events notified and number of mails sent *****/
   Ntf_GetNumNotifSent (DegCod,CrsCod,NotifyEvent,&CurrentNumEvents,&CurrentNumMails);

   /***** Update number of users notified *****/
   DB_QueryREPLACE ("can not update the number of sent notifications",
		    "REPLACE INTO sta_notif"
		    " (DegCod,CrsCod,NotifyEvent,NumEvents,NumMails)"
		    " VALUES"
		    " (%ld,%ld,%u,%u,%u)",
	            DegCod,CrsCod,(unsigned) NotifyEvent,
	            CurrentNumEvents + NumEvents,
	            CurrentNumMails + NumMails);
  }

/*****************************************************************************/
/******************** Mark all my notifications as seen **********************/
/*****************************************************************************/

void Ntf_MarkAllNotifAsSeen (void)
  {
   /***** Set all my notifications as seen *****/
   DB_QueryUPDATE ("can not set notification(s) as seen",
		   "UPDATE notif SET Status=(Status | %u)"
		   " WHERE ToUsrCod=%ld",
	           (unsigned) Ntf_STATUS_BIT_READ,
	           Gbl.Usrs.Me.UsrDat.UsrCod);

   /***** Show my notifications again *****/
   Ntf_ShowMyNotifications ();
  }

/*****************************************************************************/
/*** Activate the sending of email to notify me that I have new messages *****/
/*****************************************************************************/

void Ntf_PutFormChangeNotifSentByEMail (void)
  {
   extern const char *Hlp_PROFILE_Settings_notifications;
   extern const char *The_ClassFormInBox[The_NUM_THEMES];
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

   HTM_TH (1,1,"CM",Txt_Create_BR_notification);
   HTM_TH (1,1,"CM",Txt_Notify_me_BR_by_email);

   HTM_TR_End ();

   /***** Checkbox to activate internal notifications and email notifications
          about events *****/
   for (NotifyEvent  = (Ntf_NotifyEvent_t) 1;
	NotifyEvent <= (Ntf_NotifyEvent_t) (Ntf_NUM_NOTIFY_EVENTS - 1);
	NotifyEvent++)	// O is reserved for Ntf_EVENT_UNKNOWN
     {
      HTM_TR_Begin (NULL);

      HTM_TD_Begin ("class=\"%s RM\"",The_ClassFormInBox[Gbl.Prefs.Theme]);
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
   DB_QueryUPDATE ("can not update user's settings",
		   "UPDATE usr_data"
		   " SET NotifNtfEvents=%u,EmailNtfEvents=%u"
		   " WHERE UsrCod=%ld",
	           Gbl.Usrs.Me.UsrDat.NtfEvents.CreateNotif,
	           Gbl.Usrs.Me.UsrDat.NtfEvents.SendEmail,
	           Gbl.Usrs.Me.UsrDat.UsrCod);

   /***** Show message *****/
   Ale_ShowAlert (Ale_SUCCESS,Txt_Your_settings_about_notifications_have_changed);
  }

/*****************************************************************************/
/************************ Write number of notifications **********************/
/*****************************************************************************/

void Ntf_WriteNumberOfNewNtfs (void)
  {
   extern const char *The_ClassNotif[The_NUM_THEMES];
   extern const char *Txt_See_notifications;
   extern const char *Txt_notification;
   extern const char *Txt_notifications;
   extern const char *Txt_Notifications;
   extern const char *Txt_NOTIF_new_SINGULAR;
   extern const char *Txt_NOTIF_new_PLURAL;
   unsigned NumUnseenNtfs;
   unsigned NumNewNtfs = 0;

   /***** Get my number of unseen notifications *****/
   if ((NumUnseenNtfs = Ntf_GetNumberOfAllMyUnseenNtfs ()))
      NumNewNtfs = Ntf_GetNumberOfMyNewUnseenNtfs ();

   /***** Begin form *****/
   Frm_BeginFormId (ActSeeNewNtf,"form_ntf");
   HTM_BUTTON_SUBMIT_Begin (Txt_See_notifications,The_ClassNotif[Gbl.Prefs.Theme],NULL);

   /***** Number of unseen notifications *****/
   HTM_SPAN_Begin ("id=\"notif_all\"");
   HTM_TxtF ("%u&nbsp;%s",NumUnseenNtfs,NumUnseenNtfs == 1 ? Txt_notification :
				                             Txt_notifications);
   HTM_SPAN_End ();

   /***** Icon and number of new notifications *****/
   if (NumNewNtfs)
     {
      HTM_BR ();
      HTM_IMG (Gbl.Prefs.URLTheme,"bell.svg",Txt_Notifications,
	       "class=\"ICO16x16\"");
      HTM_TxtF ("&nbsp;%u",NumNewNtfs);
      HTM_SPAN_Begin ("id=\"notif_new\"");
      HTM_TxtF ("&nbsp;%s",NumNewNtfs == 1 ? Txt_NOTIF_new_SINGULAR :
				             Txt_NOTIF_new_PLURAL);
      HTM_SPAN_End ();
     }

   /***** End form *****/
   HTM_BUTTON_End ();
   Frm_EndForm ();
  }

/*****************************************************************************/
/************* Get the number of (all) my unseen notifications ***************/
/*****************************************************************************/

static unsigned Ntf_GetNumberOfAllMyUnseenNtfs (void)
  {
   /***** Get number of places with a name from database *****/
   return DB_QueryCOUNT ("can not get number of unseen notifications",
			 "SELECT COUNT(*) FROM notif"
			 " WHERE ToUsrCod=%ld AND (Status & %u)=0",
			 Gbl.Usrs.Me.UsrDat.UsrCod,
			 (unsigned) (Ntf_STATUS_BIT_READ | Ntf_STATUS_BIT_REMOVED));
  }

/*****************************************************************************/
/************** Get the number of my new unseen notifications ****************/
/*****************************************************************************/

static unsigned Ntf_GetNumberOfMyNewUnseenNtfs (void)
  {
   /***** Get number of places with a name from database *****/
   return DB_QueryCOUNT ("can not get number of unseen notifications",
			 "SELECT COUNT(*) FROM notif"
			 " WHERE ToUsrCod=%ld AND (Status & %u)=0"
			 " AND TimeNotif>FROM_UNIXTIME(%ld)",
			 Gbl.Usrs.Me.UsrDat.UsrCod,
			 (unsigned) (Ntf_STATUS_BIT_READ | Ntf_STATUS_BIT_REMOVED),
			 Gbl.Usrs.Me.UsrLast.LastAccNotif);
  }

/*****************************************************************************/
/**************** Remove all notifications made to a user ********************/
/*****************************************************************************/

void Ntf_RemoveUsrNtfs (long ToUsrCod)
  {
   /***** Delete notifications of a user ******/
   DB_QueryDELETE ("can not remove notifications of a user",
		   "DELETE LOW_PRIORITY FROM notif WHERE ToUsrCod=%ld",
                   ToUsrCod);
  }
