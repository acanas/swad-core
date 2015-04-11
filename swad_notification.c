// swad_notification.c: notifications about events, sent by e-mail

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2015 Antonio Cañas Vargas

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

#include <linux/stddef.h>	// For NULL
#include <stdlib.h>		// For system
#include <string.h>
#include <sys/wait.h>		// For the macro WEXITSTATUS
#include <unistd.h>		// For unlink

#include "swad_action.h"
#include "swad_config.h"
#include "swad_config.h"
#include "swad_database.h"
#include "swad_enrollment.h"
#include "swad_exam.h"
#include "swad_follow.h"
#include "swad_global.h"
#include "swad_mark.h"
#include "swad_notice.h"
#include "swad_notification.h"
#include "swad_parameter.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/****************************** Public constants *****************************/
/*****************************************************************************/

const char *Ntf_WSNotifyEvents[Ntf_NUM_NOTIFY_EVENTS] =
  {
   "unknown",

   /* Course tab */
   "documentFile",
   "sharedFile",

   /* Assessment tab */
   "assignment",
   "examAnnouncement",
   "marksFile",

   /* Enrollment tab */
   "enrollment",
   "enrollmentRequest",

   /* Messages tab */
   "notice",
   "forumPostCourse",
   "forumReply",
   "message",

   /* Statistics tab */
   "survey",

   /* Profile tab */
   "follower",
  };

static const Act_Action_t Ntf_DefaultActions[Ntf_NUM_NOTIFY_EVENTS] =
  {
   ActUnk,		// Ntf_EVENT_UNKNOWN

   /* Course tab */
   ActSeeAdmDocCrs,	// Ntf_EVENT_DOCUMENT_FILE
   ActAdmCom,		// Ntf_EVENT_SHARED_FILE

   /* Assessment tab */
   ActSeeAsg,		// Ntf_EVENT_ASSIGNMENT
   ActSeeExaAnn,	// Ntf_EVENT_EXAM_ANNOUNCEMENT
   ActSeeAdmMrk,	// Ntf_EVENT_MARKS_FILE

   /* Users tab */
   ActReqAccEnrCrs,	// Ntf_EVENT_ENROLLMENT
   ActSeeSignUpReq,	// Ntf_EVENT_ENROLLMENT_REQUEST

   /* Messages tab */
   ActSeeNot,		// Ntf_EVENT_NOTICE
   ActSeeFor,		// Ntf_EVENT_FORUM_POST_COURSE
   ActSeeFor,		// Ntf_EVENT_FORUM_REPLY
   ActExpRcvMsg,	// Ntf_EVENT_MESSAGE

   /* Statistics tab */
   ActSeeAllSvy,	// Ntf_EVENT_SURVEY

   /* Profile tab */
   ActSeeFlr,		// Ntf_EVENT_FOLLOWER
  };

/*****************************************************************************/
/***************************** Private constants *****************************/
/*****************************************************************************/

// Notify me notification events
static const char *Ntf_ParamNotifMeAboutNotifyEvents[Ntf_NUM_NOTIFY_EVENTS] =
  {
   "NotifyNtfEventUnknown",

   /* Course tab */
   "NotifyNtfEventDocumentFile",
   "NotifyNtfEventSharedFile",

   /* Assessment tab */
   "NotifyNtfEventAssignment",
   "NotifyNtfEventExamAnnouncement",
   "NotifyNtfEventMarksFile",

   /* Enrollment tab */
   "NotifyNtfEventEnrollment",
   "NotifyNtfEventEnrollmentRequest",

   /* Messages tab */
   "NotifyNtfEventNotice",
   "NotifyNtfEventForumPostCourse",
   "NotifyNtfEventForumReply",
   "NotifyNtfEventMessage",

   /* Statistics tab */
   "NotifyNtfEventSurvey",

   /* Profile tab */
   "NotifyNtfEventFollower",
  };

// Email me about notification events
static const char *Ntf_ParamEmailMeAboutNotifyEvents[Ntf_NUM_NOTIFY_EVENTS] =
  {
   "EmailNtfEventUnknown",

   /* Course tab */
   "EmailNtfEventDocumentFile",
   "EmailNtfEventSharedFile",

   /* Assessment tab */
   "EmailNtfEventAssignment",
   "EmailNtfEventExamAnnouncement",
   "EmailNtfEventMarksFile",

   /* Enrollment tab */
   "EmailNtfEventEnrollment",
   "EmailNtfEventEnrollmentRequest",

   /* Messages tab */
   "EmailNtfEventNotice",
   "EmailNtfEventForumPostCourse",
   "EmailNtfEventForumReply",
   "EmailNtfEventMessage",

   /* Statistics tab */
   "EmailNtfEventSurvey",

   /* Profile tab */
   "EmailNtfEventFollower",
  };

// Icons for notification events
static const char *Ntf_Icons[Ntf_NUM_NOTIFY_EVENTS] =
  {
   "faq",			// Ntf_EVENT_UNKNOWN

   /* Course tab */
   "folder",			// Ntf_EVENT_DOCUMENT_FILE	// TODO: Change icon to "file"
   "folderusers",		// Ntf_EVENT_SHARED_FILE	// TODO: Change icon to "file"

   /* Assessment tab */
   "desk",			// Ntf_EVENT_ASSIGNMENT
   "announce",			// Ntf_EVENT_EXAM_ANNOUNCEMENT
   "grades",			// Ntf_EVENT_MARKS_FILE

   /* Enrollment tab */
   "adduser",			// Ntf_EVENT_ENROLLMENT
   "enrollmentrequest",		// Ntf_EVENT_ENROLLMENT_REQUEST

   /* Messages tab */
   "note",			// Ntf_EVENT_NOTICE
   "forum",			// Ntf_EVENT_FORUM_POST_COURSE
   "forum",			// Ntf_EVENT_FORUM_REPLY
   "recmsg",			// Ntf_EVENT_MESSAGE

   /* Statistics tab */
   "survey",			// Ntf_EVENT_SURVEY

   /* Profile tab */
   "follow",			// Ntf_EVENT_FOLLOWER
  };

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Ntf_WriteFormAllNotifications (bool AllNotifications);
static bool Ntf_GetAllNotificationsFromForm (void);
static void Ntf_StartFormGoToAction (Ntf_NotifyEvent_t NotifyEvent,
                                     long CrsCod,long Cod);
static void Ntf_UpdateMyLastAccessToNotifications (void);
static void Ntf_SendPendingNotifByEMailToOneUsr (struct UsrData *ToUsrDat,unsigned *NumNotif,unsigned *NumMails);
static void Ntf_GetNumNotifSent (long DegCod,long CrsCod,
                                 Ntf_NotifyEvent_t NotifyEvent,
                                 unsigned *NumEvents,unsigned *NumMails);
static void Ntf_UpdateNumNotifSent (long DegCod,long CrsCod,
                                    Ntf_NotifyEvent_t NotifyEvent,
                                    unsigned NumEvents,unsigned NumMails);
static void Ntf_PutLinkToChangePrefs (void);
static void Ntf_PutLinkToMarkAllNotifAsSeen (void);
static void Ntf_GetParamsNotifyEvents (void);
static unsigned Ntf_GetNumberOfAllMyUnseenNtfs (void);
static unsigned Ntf_GetNumberOfMyNewUnseenNtfs (void);

/*****************************************************************************/
/*************************** Show my notifications ***************************/
/*****************************************************************************/

void Ntf_ShowMyNotifications (void)
  {
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
   extern const char *Txt_No_notifications_for_you;
   char Query[512];
   char SubQuery[128];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumNotif;
   unsigned long NumNotifications;
   bool AllNotifications;
   Ntf_NotifyEvent_t NotifyEvent = (Ntf_NotifyEvent_t) 0;	// Initialized to avoid warning
   struct UsrData UsrDat;
   struct Institution Ins;
   struct Centre Ctr;
   struct Degree Deg;
   struct Course Crs;
   long Cod;
   char ForumName[512];
   char DateTime[4+2+2+2+2+2+1];	// Time of the event in YYYYMMDDHHMMSS format
   Ntf_Status_t Status;
   Ntf_StatusTxt_t StatusTxt;
   char *SummaryStr;
   char *ContentStr;
   const char *ClassBackground;
   const char *ClassAnchor;
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
   sprintf (Query,"SELECT NotifyEvent,FromUsrCod,InsCod,CtrCod,DegCod,CrsCod,"
	          "Cod,DATE_FORMAT(TimeNotif,'%%Y%%m%%d%%H%%i%%S'),Status"
                  " FROM notif"
                  " WHERE ToUsrCod='%ld'%s"
                  " ORDER BY TimeNotif DESC",
            Gbl.Usrs.Me.UsrDat.UsrCod,SubQuery);
   NumNotifications = DB_QuerySELECT (Query,&mysql_res,"can not get your notifications");

   /***** Buttons to change preferences and to mark all notifications as seen *****/
   fprintf (Gbl.F.Out,"<div class=\"CONTEXT_MENU\">");
   Ntf_PutLinkToChangePrefs ();			// Put form to change notification preferences
   if (NumNotifications)
      Ntf_PutLinkToMarkAllNotifAsSeen ();	// Put form to change notification preferences
   fprintf (Gbl.F.Out,"</div>");

   /***** Write form to show all notifications *****/
   Ntf_WriteFormAllNotifications (AllNotifications);

   /***** List my notifications *****/
   if (NumNotifications)	// Notifications found
     {
      /***** Allocate memory for the summary of the notification *****/
      if ((SummaryStr = malloc (Cns_MAX_BYTES_TEXT+1)) == NULL)
         Lay_ShowErrorAndExit ("Not enough memory to store the summary of the notification.");

      /***** Initialize structure with user's data *****/
      Usr_UsrDataConstructor (&UsrDat);

      /***** Table start *****/
      Lay_StartRoundFrameTable10 (NULL,2,Txt_Notifications);
      fprintf (Gbl.F.Out,"<tr>"
                         "<th colspan=\"2\" class=\"TIT_TBL\""
                         " style=\"text-align:left;\">"
                         "%s"
                         "</th>"
                         "<th colspan=\"2\" class=\"TIT_TBL\""
                         " style=\"text-align:left;\">"
                         "%s"
                         "</th>"
                         "<th class=\"TIT_TBL\" style=\"text-align:left;\">"
                         "%s"
                         "</th>"
                         "<th class=\"TIT_TBL\" style=\"text-align:center;\">"
                         "%s"
                         "</th>"
                         "<th class=\"TIT_TBL\" style=\"text-align:left;\">"
                         "%s"
                         "</th>"
                         "</tr>",
               Txt_Event,
               Txt_MSG_From,
               Txt_Location,
               Txt_Date,
               Txt_Email);	// Date and time, in YYYY-MM-DD HH:MM:SS format

      /***** List notifications one by one *****/
      for (NumNotif = 0;
	   NumNotif < NumNotifications;
	   NumNotif++)
	{
         /***** Get next notification *****/
         row = mysql_fetch_row (mysql_res);

         /* Get event type (row[0]) */
         NotifyEvent = Ntf_GetNotifyEventFromDB ((const char *) row[0]);

         /* Get (from) user code (row[1]) */
         UsrDat.UsrCod = Str_ConvertStrCodToLongCod (row[1]);
         Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat);		// Get user's data from the database

         /* Get institution code (row[2]) */
         Ins.InsCod = Str_ConvertStrCodToLongCod (row[2]);
         Ins_GetDataOfInstitutionByCod (&Ins,Ins_GET_MINIMAL_DATA);

          /* Get centre code (row[3]) */
         Ctr.CtrCod = Str_ConvertStrCodToLongCod (row[3]);
         Ctr_GetDataOfCentreByCod (&Ctr);

         /* Get degree code (row[4]) */
         Deg.DegCod = Str_ConvertStrCodToLongCod (row[4]);
         Deg_GetDataOfDegreeByCod (&Deg);

         /* Get course code (row[5]) */
         Crs.CrsCod = Str_ConvertStrCodToLongCod (row[5]);
         Crs_GetDataOfCourseByCod (&Crs);

         /* Get message/post/... code (row[6]) */
         Cod = Str_ConvertStrCodToLongCod (row[6]);

         /* Get forum type of the post */
         if (NotifyEvent == Ntf_EVENT_FORUM_POST_COURSE ||
             NotifyEvent == Ntf_EVENT_FORUM_REPLY)
           {
            Gbl.Forum.ForumType = For_GetForumTypeOfAPost (Cod);
            For_SetForumName (Gbl.Forum.ForumType,
        	              &Ins,
        	              &Ctr,
        	              &Deg,
        	              &Crs,
        	              ForumName,Gbl.Prefs.Language,false);	// Set forum name in recipient's language
            Gbl.Forum.Ins.InsCod = Ins.InsCod;
            Gbl.Forum.Ctr.CtrCod = Ctr.CtrCod;
            Gbl.Forum.Deg.DegCod = Deg.DegCod;
            Gbl.Forum.Crs.CrsCod = Crs.CrsCod;
           }

         /* Get time of the event (row[7]) */
         strncpy (DateTime,row[7],4+2+2+2+2+2);
         DateTime[4+2+2+2+2+2] = '\0';

         /* Get status (row[8]) */
         if (sscanf (row[8],"%u",&Status) != 1)
            Lay_ShowErrorAndExit ("Wrong notification status.");
         StatusTxt = Ntf_GetStatusTxtFromStatusBits (Status);

         if (Status & Ntf_STATUS_BIT_REMOVED)	// The source of the notification was removed
           {
            ClassBackground   = "MSG_TIT_BG_REM";
            ClassAnchor       = "MSG_TIT_REM";
            ClassAuthorBg     = "MSG_AUT_BG_REM";
            PutLink = false;
           }
         else if (Status & Ntf_STATUS_BIT_READ)	// I have already seen the source of the notification
           {
            ClassBackground   = "MSG_TIT_BG";
            ClassAnchor       = "MSG_TIT";
            ClassAuthorBg     = "MSG_AUT_BG";
            PutLink = true;
           }
         else					// I have not seen the source of the notification
           {
            ClassBackground   = "MSG_TIT_BG_NEW";
            ClassAnchor       = "MSG_TIT_NEW";
            ClassAuthorBg     = "MSG_AUT_BG_NEW";
            PutLink = true;
           }

         /***** Write row for this notification *****/
	 /* Write event icon */
         fprintf (Gbl.F.Out,"<tr>"
                            "<td class=\"%s\" style=\"width:20px;"
                            " text-align:left; vertical-align:top;\">",
                  ClassBackground);
         if (PutLink)
           {
            Ntf_StartFormGoToAction (NotifyEvent,Crs.CrsCod,Cod);
            fprintf (Gbl.F.Out,"<input type=\"image\" src=\"%s/%s16x16.gif\""
        	               " alt=\"%s\" title=\"%s\" class=\"ICON16x16\" />",
                     Gbl.Prefs.IconsURL,
                     Ntf_Icons[NotifyEvent],
                     Txt_NOTIFY_EVENTS_SINGULAR[NotifyEvent],
                     Txt_NOTIFY_EVENTS_SINGULAR[NotifyEvent]);
            Act_FormEnd ();
           }
         else
            fprintf (Gbl.F.Out,"<img src=\"%s/%s16x16.gif\" alt=\"%s\""
        	               " class=\"ICON16x16\" />",
                     Gbl.Prefs.IconsURL,
                     Ntf_Icons[NotifyEvent],
                     Txt_NOTIFY_EVENTS_SINGULAR[NotifyEvent]);
         fprintf (Gbl.F.Out,"</td>");

         /* Write event type */
         fprintf (Gbl.F.Out,"<td class=\"%s\" style=\"text-align:left;"
                            " vertical-align:top;\">",
                  ClassBackground);
         if (PutLink)
           {
            Ntf_StartFormGoToAction (NotifyEvent,Crs.CrsCod,Cod);
            Act_LinkFormSubmit (Txt_NOTIFY_EVENTS_SINGULAR[NotifyEvent],ClassAnchor);
            fprintf (Gbl.F.Out,"%s</a>",
                     Txt_NOTIFY_EVENTS_SINGULAR[NotifyEvent]);
            Act_FormEnd ();
           }
         else
            fprintf (Gbl.F.Out,"<span class=\"%s\">%s</span>",
                     ClassAnchor,Txt_NOTIFY_EVENTS_SINGULAR[NotifyEvent]);
         fprintf (Gbl.F.Out,"</td>");

         /* Write user (from) */
         Msg_WriteMsgAuthor (&UsrDat,80,11,ClassAuthorBg,true,NULL);

         /* Write location */
         fprintf (Gbl.F.Out,"<td class=\"%s\" style=\"text-align:left;"
                            " vertical-align:top;\">",
                  ClassBackground);
         if (NotifyEvent == Ntf_EVENT_FORUM_POST_COURSE ||
             NotifyEvent == Ntf_EVENT_FORUM_REPLY)
           {
            if (PutLink)
              {
               Ntf_StartFormGoToAction (NotifyEvent,Crs.CrsCod,Cod);
               Act_LinkFormSubmit (Txt_NOTIFY_EVENTS_SINGULAR[NotifyEvent],ClassAnchor);
              }
            else
               fprintf (Gbl.F.Out,"<span class=\"%s\">",ClassAnchor);
            fprintf (Gbl.F.Out,"%s: %s",Txt_Forum,ForumName);
            if (PutLink)
              {
               fprintf (Gbl.F.Out,"</a>");
               Act_FormEnd ();
              }
            else
               fprintf (Gbl.F.Out,"</span>");
           }
         else if (Crs.CrsCod > 0)
           {
            if (PutLink)
              {
               Ntf_StartFormGoToAction (NotifyEvent,Crs.CrsCod,Cod);
               Act_LinkFormSubmit (Txt_NOTIFY_EVENTS_SINGULAR[NotifyEvent],ClassAnchor);
              }
            else
               fprintf (Gbl.F.Out,"<span class=\"%s\">",ClassAnchor);
            fprintf (Gbl.F.Out,"%s: %s",Txt_Course,Crs.ShortName);
            if (PutLink)
              {
               fprintf (Gbl.F.Out,"</a>");
               Act_FormEnd ();
              }
            else
               fprintf (Gbl.F.Out,"</span>");
           }
         else if (Deg.DegCod > 0)
           {
            fprintf (Gbl.F.Out,"<a href=\"%s\" class=\"%s\" target=\"_blank\">%s: %s</a>",
                     Deg.WWW,ClassAnchor,
                     Txt_Degree,Deg.ShortName);
           }
         else if (Ctr.CtrCod > 0)
           {
            fprintf (Gbl.F.Out,"<a href=\"%s\" class=\"%s\" target=\"_blank\">%s: %s</a>",
                     Ctr.WWW,ClassAnchor,
                     Txt_Centre,Ctr.ShortName);
           }
         else if (Ins.InsCod > 0)
            fprintf (Gbl.F.Out,"<a href=\"%s\" class=\"%s\" target=\"_blank\">%s: %s</a>",
                     Ins.WWW,ClassAnchor,
                     Txt_Institution,Ins.ShortName);
         else
            fprintf (Gbl.F.Out,"-");
         fprintf (Gbl.F.Out,"</td>");

         /* Write date and time (DateTime holds date and time in YYYYMMDDHHMMSS format) */
         Msg_WriteMsgDate (DateTime,ClassBackground);

         /* Write status (sent by email / pending to be sent by email) */
         fprintf (Gbl.F.Out,"<td class=\"%s\" style=\"text-align:left;"
                            " vertical-align:top;\">"
                            "%s"
                            "</td>"  \
                            "</tr>",
                  ClassBackground,Txt_NOTIFICATION_STATUS[StatusTxt]);

         /***** Write content of the event *****/
         if (PutLink)
           {
            ContentStr = NULL;
            Ntf_GetNotifSummaryAndContent (SummaryStr,&ContentStr,NotifyEvent,
                                           Cod,Crs.CrsCod,Gbl.Usrs.Me.UsrDat.UsrCod,Cfg_MAX_CHARS_NOTIF_SUMMARY_SWAD,false);
            fprintf (Gbl.F.Out,"<tr>"
                               "<td colspan=\"2\"></td>"
                               "<td colspan=\"5\" class=\"DAT\""
                               " style=\"text-align:left; vertical-align:top;"
                               " padding-bottom:10px;\">"
                               "%s"
                               "</td>"
                               "</tr>",
                     SummaryStr);
            if (ContentStr != NULL)
              {
               free ((void *) ContentStr);
               ContentStr = NULL;
              }
           }
        }

      /***** Table end *****/
      Lay_EndRoundFrameTable10 (Lay_NO_BUTTON,NULL);

      /***** Free memory used for user's data *****/
      Usr_UsrDataDestructor (&UsrDat);

      /***** Free summary *****/
      free ((void *) SummaryStr);
     }
   else
      Lay_ShowAlert (Lay_INFO,Txt_No_notifications_for_you);

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   /***** Reset to 0 the number of new notifications *****/
   Ntf_UpdateMyLastAccessToNotifications ();
  }

/*****************************************************************************/
/********** Write a form to select whether show all notifications ************/
/*****************************************************************************/

static void Ntf_WriteFormAllNotifications (bool AllNotifications)
  {
   extern const char *The_ClassFormul[The_NUM_THEMES];
   extern const char *Txt_Show_all_notifications;

   /***** Start form *****/
   fprintf (Gbl.F.Out,"<div class=\"%s\" style=\"text-align:center;"
	              " vertical-align:middle;\">",
	    The_ClassFormul[Gbl.Prefs.Theme]);
   Act_FormStart (ActSeeNtf);

   /***** End form *****/
   fprintf (Gbl.F.Out,"<input type=\"checkbox\" name=\"All\" value=\"Y\"");
   if (AllNotifications)
      fprintf (Gbl.F.Out," checked=\"checked\"");
   fprintf (Gbl.F.Out," onclick=\"javascript:document.getElementById('%s').submit();\" />"
                      " %s",
            Gbl.FormId,
            Txt_Show_all_notifications);
   Act_FormEnd ();
   fprintf (Gbl.F.Out,"</div>");
  }

/*****************************************************************************/
/************* Get whether to show all notifications from form ***************/
/*****************************************************************************/

static bool Ntf_GetAllNotificationsFromForm (void)
  {
   char YN[1+1];

   Par_GetParToText ("All",YN,1);
   return (Str_ConvertToUpperLetter (YN[0]) == 'Y');
  }

/*****************************************************************************/
/********* Pur parameters to go to an action depending on the event **********/
/*****************************************************************************/

static void Ntf_StartFormGoToAction (Ntf_NotifyEvent_t NotifyEvent,
                                     long CrsCod,long Cod)
  {
   extern const Act_Action_t For_ActionsSeeFor[For_NUM_TYPES_FORUM];
   struct FileMetadata FileMetadata;
   long GrpCod = -1L;
   char PathUntilFileName[PATH_MAX+1];
   char FileName[NAME_MAX+1];
   Act_Action_t Action = ActUnk;				// Initialized to avoid warning

   /***** Parameters depending on the type of event *****/
   switch (NotifyEvent)
     {
      case Ntf_EVENT_DOCUMENT_FILE:
      case Ntf_EVENT_SHARED_FILE:
      case Ntf_EVENT_MARKS_FILE:
	 if (Cod > 0)	// File code
	   {
	    FileMetadata.FilCod = Cod;
            Brw_GetFileMetadataByCod (&FileMetadata);
            Brw_GetCrsGrpFromFileMetadata (FileMetadata.FileBrowser,FileMetadata.Cod,&CrsCod,&GrpCod);
	    Str_SplitFullPathIntoPathAndFileName (FileMetadata.Path,
						  PathUntilFileName,
						  FileName);
	   }
	 switch (NotifyEvent)
	   {
	    case Ntf_EVENT_DOCUMENT_FILE:
	       Action = (Cod > 0) ? ((GrpCod > 0) ? ActReqDatSeeDocGrp :
		                                    ActReqDatSeeDocCrs) :
		                    ((GrpCod > 0) ? ActSeeDocGrp :
		                	            ActSeeDocCrs);
	       break;
	    case Ntf_EVENT_SHARED_FILE:
	       Action = (Cod > 0) ? ((GrpCod > 0) ? ActReqDatComGrp :
		                                    ActReqDatComCrs) :
		                    ((GrpCod > 0) ? ActAdmComGrp :
		                	            ActAdmComCrs);
	       break;
	    case Ntf_EVENT_MARKS_FILE:
	       Action = (Cod > 0) ? ((GrpCod > 0) ? ActReqDatSeeMrkGrp :
		                                    ActReqDatSeeMrkCrs) :
		                    ((GrpCod > 0) ? ActSeeMrkGrp :
		                	            ActSeeMrkCrs);
	       break;
	    default:	// Not aplicable here
	       break;
	   }
         Act_FormStart (Action);
	 Grp_PutParamGrpCod (GrpCod > 0 ? GrpCod :
	                                  -1L);
	 if (Cod > 0)	// File code
	    Brw_PutParamsPathAndFile (Brw_IS_UNKNOWN,PathUntilFileName,FileName);	// TODO: Brw_IS_UNKNOWN should be changed to Brw_IS_FILE or Brw_IS_LINK
	 break;
      case Ntf_EVENT_FORUM_POST_COURSE:
      case Ntf_EVENT_FORUM_REPLY:
	 Act_FormStart (For_ActionsSeeFor[Gbl.Forum.ForumType]);
	 For_PutAllHiddenParamsForum ();
	 break;
      case Ntf_EVENT_MESSAGE:
         Act_FormStart (Ntf_DefaultActions[NotifyEvent]);
	 Msg_PutHiddenParamMsgCod (Cod);
	 break;
      default:
         Act_FormStart (Ntf_DefaultActions[NotifyEvent]);
	 break;
     }

   /***** Parameter to go to another course *****/
   if (CrsCod > 0 &&				// Course specified
       CrsCod != Gbl.CurrentCrs.Crs.CrsCod)	// Not the current course
      Crs_PutParamCrsCod (CrsCod);		// Go to another course
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
/********************* Mark possible notification as seen ********************/
/*****************************************************************************/

void Ntf_GetNotifSummaryAndContent (char *SummaryStr,char **ContentStr,Ntf_NotifyEvent_t NotifyEvent,
                                    long Cod,long CrsCod,long UsrCod,unsigned MaxChars,bool GetContent)
  {
   SummaryStr[0] = '\0';

   switch (NotifyEvent)
     {
      case Ntf_EVENT_UNKNOWN:
         break;
      case Ntf_EVENT_DOCUMENT_FILE:
      case Ntf_EVENT_SHARED_FILE:
	 Brw_GetNotifDocOrSharedFile (SummaryStr,ContentStr,Cod,MaxChars,GetContent);
         break;
      case Ntf_EVENT_ASSIGNMENT:
         Asg_GetNotifAssignment (SummaryStr,ContentStr,Cod,MaxChars,GetContent);
         break;
      case Ntf_EVENT_EXAM_ANNOUNCEMENT:
         Exa_GetNotifExamAnnouncement (SummaryStr,ContentStr,Cod,MaxChars,GetContent);
         break;
      case Ntf_EVENT_MARKS_FILE:
         Mrk_GetNotifMyMarks (SummaryStr,ContentStr,Cod,UsrCod,MaxChars,GetContent);
         break;
      case Ntf_EVENT_ENROLLMENT:
	 Enr_GetNotifEnrollment (SummaryStr,CrsCod,UsrCod,MaxChars);
         break;
      case Ntf_EVENT_ENROLLMENT_REQUEST:
	 Enr_GetNotifEnrollmentRequest (SummaryStr,ContentStr,Cod,MaxChars,GetContent);
         break;
      case Ntf_EVENT_NOTICE:
         Not_GetNotifNotice (SummaryStr,ContentStr,Cod,MaxChars,GetContent);
         break;
      case Ntf_EVENT_FORUM_POST_COURSE:
      case Ntf_EVENT_FORUM_REPLY:
         For_GetNotifForumPst (SummaryStr,ContentStr,Cod,MaxChars,GetContent);
         break;
      case Ntf_EVENT_MESSAGE:
         Msg_GetNotifMessage (SummaryStr,ContentStr,Cod,MaxChars,GetContent);
         if (Gbl.WebService.IsWebService)
            /* Set the message as open by me, because I can read it in an extern application */
            Msg_SetReceivedMsgAsOpen (Cod,UsrCod);
         break;
      case Ntf_EVENT_SURVEY:
         Svy_GetNotifSurvey (SummaryStr,ContentStr,Cod,MaxChars,GetContent);
         break;
      case Ntf_EVENT_FOLLOWER:
         Fol_GetNotifFollower (SummaryStr,ContentStr);
         break;
     }

   //if (Gbl.WebService.IsWebService)
      /* Set the notification as seen, because I can see it in an extern application */
   //   Ntf_SetNotifAsSeen (NotifyEvent,Cod,UsrCod);
  }

/*****************************************************************************/
/********************** Set possible notification as seen ********************/
/*****************************************************************************/

void Ntf_SetNotifAsSeen (Ntf_NotifyEvent_t NotifyEvent,long Cod,long ToUsrCod)
  {
   char Query[512];

   /***** Set notification as seen by me *****/
   if (ToUsrCod > 0)	// If the user code is specified
     {
      if (Cod > 0)	// Set only one notification as seen
         sprintf (Query,"UPDATE notif SET Status=(Status | %u)"
                        " WHERE ToUsrCod='%ld' AND NotifyEvent='%u' AND Cod='%ld'",
                  (unsigned) Ntf_STATUS_BIT_READ,
                  ToUsrCod,(unsigned) NotifyEvent,Cod);
      else		// Set all notifications of this type, in the current course for the user, as seen
         sprintf (Query,"UPDATE notif SET Status=(Status | %u)"
                        " WHERE ToUsrCod='%ld' AND NotifyEvent='%u' AND CrsCod='%ld'",
                  (unsigned) Ntf_STATUS_BIT_READ,
                  ToUsrCod,(unsigned) NotifyEvent,Gbl.CurrentCrs.Crs.CrsCod);
      DB_QueryUPDATE (Query,"can not set notification(s) as seen");
     }
  }

/*****************************************************************************/
/******************* Set possible notifications as removed *******************/
/*****************************************************************************/

void Ntf_SetNotifAsRemoved (Ntf_NotifyEvent_t NotifyEvent,long Cod)
  {
   char Query[512];

   /***** Set notification as removed *****/
   sprintf (Query,"UPDATE notif SET Status=(Status | %u)"
                  " WHERE NotifyEvent='%u' AND Cod='%ld'",
            (unsigned) Ntf_STATUS_BIT_REMOVED,
            (unsigned) NotifyEvent,Cod);
   DB_QueryUPDATE (Query,"can not set notification(s) as removed");
  }

/*****************************************************************************/
/******************** Set possible notification as removed *******************/
/*****************************************************************************/

void Ntf_SetNotifToOneUsrAsRemoved (Ntf_NotifyEvent_t NotifyEvent,long Cod,long ToUsrCod)
  {
   char Query[512];

   /***** Set notification as removed *****/
   if (Cod > 0)	// Set only one notification as removed
      sprintf (Query,"UPDATE notif SET Status=(Status | %u)"
		     " WHERE ToUsrCod='%ld' AND NotifyEvent='%u' AND Cod='%ld'",
	       (unsigned) Ntf_STATUS_BIT_REMOVED,
	       ToUsrCod,(unsigned) NotifyEvent,Cod);
   else		// Set all notifications of this type, in the current course for the user, as removed
      sprintf (Query,"UPDATE notif SET Status=(Status | %u)"
		     " WHERE ToUsrCod='%ld' AND NotifyEvent='%u' AND CrsCod='%ld'",
	       (unsigned) Ntf_STATUS_BIT_REMOVED,
	       ToUsrCod,(unsigned) NotifyEvent,Gbl.CurrentCrs.Crs.CrsCod);
   DB_QueryUPDATE (Query,"can not set notification(s) as removed");
  }

/*****************************************************************************/
/*********** Set possible notifications from a course as removed *************/
/*****************************************************************************/
// This function should be called when a course is removed
// because notifications from this course will not be available after course removing.
// However, notifications about new messages should not be removed
// because the messages will remain available

void Ntf_SetNotifInCrsAsRemoved (long CrsCod,long ToUsrCod)
  {
   char Query[512];

   /***** Set all notifications from the course as removed,
          except notifications about new messages *****/
   if (ToUsrCod > 0)	// If the user code is specified
      sprintf (Query,"UPDATE notif SET Status=(Status | %u)"
		     " WHERE ToUsrCod='%ld' AND CrsCod='%ld' AND NotifyEvent<>'%u'",
	       (unsigned) Ntf_STATUS_BIT_REMOVED,
	       ToUsrCod,CrsCod,(unsigned) Ntf_EVENT_MESSAGE);
   else
      sprintf (Query,"UPDATE notif SET Status=(Status | %u)"
		     " WHERE CrsCod='%ld' AND NotifyEvent<>'%u'",
	       (unsigned) Ntf_STATUS_BIT_REMOVED,
	       CrsCod,(unsigned) Ntf_EVENT_MESSAGE);
   DB_QueryUPDATE (Query,"can not set notification(s) as removed");
  }

/*****************************************************************************/
/************ Set possible notifications of one file as removed **************/
/*****************************************************************************/

void Ntf_SetNotifOneFileAsRemoved (Brw_FileBrowser_t FileBrowser,
                                   long Cod,const char *Path)
  {
   char Query[512];
   char SubQuery[256];
   Ntf_NotifyEvent_t NotifyEvent;

   /***** Set notify event depending on browser zone *****/
   switch (FileBrowser)
     {
      case Brw_ADMI_DOCUM_CRS:
      case Brw_ADMI_DOCUM_GRP:
	 NotifyEvent = Ntf_EVENT_DOCUMENT_FILE;
	 break;
      case Brw_ADMI_SHARE_CRS:
      case Brw_ADMI_SHARE_GRP:
	 NotifyEvent = Ntf_EVENT_SHARED_FILE;
	 break;
      case Brw_ADMI_MARKS_CRS:
      case Brw_ADMI_MARKS_GRP:
	 NotifyEvent = Ntf_EVENT_MARKS_FILE;
	 break;
      default:
	 return;
     }

   /***** Set notification as removed *****/
   switch (FileBrowser)
     {
      case Brw_ADMI_DOCUM_CRS:
      case Brw_ADMI_DOCUM_GRP:
      case Brw_ADMI_SHARE_CRS:
      case Brw_ADMI_SHARE_GRP:
      case Brw_ADMI_MARKS_CRS:
      case Brw_ADMI_MARKS_GRP:
	 sprintf (SubQuery,"SELECT FilCod FROM files"
			   " WHERE FileBrowser='%u' AND Cod='%ld' AND Path='%s'",
		  (unsigned) FileBrowser,Cod,Path);
         break;
      default:
	 break;
     }
   sprintf (Query,"UPDATE notif SET Status=(Status | %u)"
		  " WHERE NotifyEvent='%u' AND Cod IN (%s)",
	    (unsigned) Ntf_STATUS_BIT_REMOVED,
	    (unsigned) NotifyEvent,SubQuery);
   DB_QueryUPDATE (Query,"can not set notification(s) as removed");
  }

/*****************************************************************************/
/************** Set possible notifications of marks as removed ***************/
/*****************************************************************************/

void Ntf_SetNotifChildrenOfFolderAsRemoved (Brw_FileBrowser_t FileBrowser,
                                            long Cod,const char *Path)
  {
   char Query[512];
   char SubQuery[256];
   Ntf_NotifyEvent_t NotifyEvent;

   /***** Set notify event depending on browser zone *****/
   switch (FileBrowser)
     {
      case Brw_ADMI_DOCUM_CRS:
      case Brw_ADMI_DOCUM_GRP:
	 NotifyEvent = Ntf_EVENT_DOCUMENT_FILE;
	 break;
      case Brw_ADMI_SHARE_CRS:
      case Brw_ADMI_SHARE_GRP:
	 NotifyEvent = Ntf_EVENT_SHARED_FILE;
	 break;
      case Brw_ADMI_MARKS_CRS:
      case Brw_ADMI_MARKS_GRP:
	 NotifyEvent = Ntf_EVENT_MARKS_FILE;
	 break;
      default:
	 return;
     }

   /***** Set notification as removed *****/
   switch (FileBrowser)
     {
      case Brw_ADMI_DOCUM_CRS:
      case Brw_ADMI_DOCUM_GRP:
      case Brw_ADMI_SHARE_CRS:
      case Brw_ADMI_SHARE_GRP:
      case Brw_ADMI_MARKS_CRS:
      case Brw_ADMI_MARKS_GRP:
	 sprintf (SubQuery,"SELECT FilCod FROM files"
			   " WHERE FileBrowser='%u' AND Cod='%ld' AND Path LIKE '%s/%%'",
		  (unsigned) FileBrowser,Cod,Path);
         break;
      default:
	 break;
     }
   sprintf (Query,"UPDATE notif SET Status=(Status | %u)"
		  " WHERE NotifyEvent='%u' AND Cod IN (%s)",
	    (unsigned) Ntf_STATUS_BIT_REMOVED,
	    (unsigned) NotifyEvent,SubQuery);
   DB_QueryUPDATE (Query,"can not set notification(s) as removed");
  }

/*****************************************************************************/
/******* Set all possible notifications of files in a group as removed *******/
/*****************************************************************************/

void Ntf_SetNotifFilesInGroupAsRemoved (long GrpCod)
  {
   char Query[512];

   /***** Set notifications as removed *****/
   sprintf (Query,"UPDATE notif SET Status=(Status | %u)"
                  " WHERE NotifyEvent IN ('%u','%u','%u') AND Cod IN"
                  " (SELECT FilCod FROM files"
                  " WHERE FileBrowser IN ('%u','%u','%u') AND Cod='%ld')",
            (unsigned) Ntf_STATUS_BIT_REMOVED,
            (unsigned) Ntf_EVENT_DOCUMENT_FILE,
            (unsigned) Ntf_EVENT_SHARED_FILE,
            (unsigned) Ntf_EVENT_MARKS_FILE,
            (unsigned) Brw_ADMI_DOCUM_GRP,
            (unsigned) Brw_ADMI_SHARE_GRP,
            (unsigned) Brw_ADMI_MARKS_GRP,
            GrpCod);
   DB_QueryUPDATE (Query,"can not set notification(s) as removed");
  }

/*****************************************************************************/
/***** Get a list with user's codes of all users in current course/group  ****/
/***** who wants to be notified by e-mail about an event, and notify them ****/
/*****************************************************************************/
// Return the number of users notified by e-mail

unsigned Ntf_StoreNotifyEventsToAllUsrs (Ntf_NotifyEvent_t NotifyEvent,long Cod)
  {
   char Query[1024];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRow;
   unsigned long NumRows;
   struct UsrData UsrDat;
   unsigned NumUsrsToBeNotifiedByEMail = 0;
   unsigned NotifyEventMask = (1 << NotifyEvent);

   /***** Get users who want to be notified from database ******/
   switch (NotifyEvent)
     {
      case Ntf_EVENT_UNKNOWN:	// This function should not be called in this case
         return 0;
      case Ntf_EVENT_DOCUMENT_FILE:
      case Ntf_EVENT_SHARED_FILE:
      case Ntf_EVENT_MARKS_FILE:
         switch (Gbl.FileBrowser.Type)
           {
            case Brw_ADMI_DOCUM_CRS:
            case Brw_ADMI_SHARE_CRS:
            case Brw_ADMI_MARKS_CRS:
               sprintf (Query,"SELECT UsrCod FROM crs_usr"
                              " WHERE CrsCod='%ld'"
                              " AND UsrCod<>'%ld'",
                        Gbl.CurrentCrs.Crs.CrsCod,
                        Gbl.Usrs.Me.UsrDat.UsrCod);
               break;
            case Brw_ADMI_DOCUM_GRP:
            case Brw_ADMI_SHARE_GRP:
            case Brw_ADMI_MARKS_GRP:
               sprintf (Query,"SELECT UsrCod FROM crs_grp_usr"
                              " WHERE crs_grp_usr.GrpCod='%ld'"
                              " AND crs_grp_usr.UsrCod<>'%ld'",
                        Gbl.CurrentCrs.Grps.GrpCod,
                        Gbl.Usrs.Me.UsrDat.UsrCod);
               break;
            default:	// This function should not be called in other cases
               return 0;
           }
         break;
      case Ntf_EVENT_ASSIGNMENT:
         // 1. If the assignment is available for the whole course ==> get all users enrolled in the course except me
         // 2. If the assignment is available only for some groups ==> get all users who belong to any of the groups except me
         // Cases 1 and 2 are mutually exclusive, so the union returns the case 1 or 2
         sprintf (Query,"(SELECT crs_usr.UsrCod"
                        " FROM assignments,crs_usr"
                        " WHERE assignments.AsgCod='%ld'"
                        " AND assignments.AsgCod NOT IN"
                        " (SELECT AsgCod FROM asg_grp WHERE AsgCod='%ld')"
                        " AND assignments.CrsCod=crs_usr.CrsCod"
                        " AND crs_usr.UsrCod<>'%ld')"
                        " UNION "
                        "(SELECT DISTINCT crs_grp_usr.UsrCod"
                        " FROM asg_grp,crs_grp_usr"
                        " WHERE asg_grp.AsgCod='%ld'"
                        " AND asg_grp.GrpCod=crs_grp_usr.GrpCod"
                        " AND crs_grp_usr.UsrCod<>'%ld')",
                  Cod,Cod,Gbl.Usrs.Me.UsrDat.UsrCod,
                  Cod,Gbl.Usrs.Me.UsrDat.UsrCod);
         break;
      case Ntf_EVENT_EXAM_ANNOUNCEMENT:
      case Ntf_EVENT_NOTICE:
         sprintf (Query,"SELECT UsrCod FROM crs_usr"
                        " WHERE CrsCod='%ld' AND UsrCod<>'%ld'",
                  Gbl.CurrentCrs.Crs.CrsCod,
                  Gbl.Usrs.Me.UsrDat.UsrCod);
         break;
      case Ntf_EVENT_ENROLLMENT:	// This function should not be called in this case
         return 0;
      case Ntf_EVENT_ENROLLMENT_REQUEST:
	 if (Gbl.CurrentCrs.Crs.NumTchs)
	    // If this course has teachers ==> send notification to teachers
	    sprintf (Query,"SELECT UsrCod FROM crs_usr"
			   " WHERE CrsCod='%ld' AND UsrCod<>'%ld'"
			   " AND Role='%u'",	// Notify teachers only
		     Gbl.CurrentCrs.Crs.CrsCod,
		     Gbl.Usrs.Me.UsrDat.UsrCod,
		     (unsigned) Rol_TEACHER);
	 else	// Course without teachers
	    // If this course has no teachers
	    // and I want to be a teacher (checked before calling this function
	    // to not send requests to be a student to admins)
	    // ==> send notification to administrators or superusers
	    sprintf (Query,"SELECT UsrCod FROM admin"
	 		   " WHERE (Scope='Sys'"
	 		   " OR (Scope='Ins' AND Cod='%ld')"
	 		   " OR (Scope='Ctr' AND Cod='%ld')"
	 		   " OR (Scope='Deg' AND Cod='%ld'))"
	 		   " AND UsrCod<>'%ld'",
	 	     Gbl.CurrentIns.Ins.InsCod,
	 	     Gbl.CurrentCtr.Ctr.CtrCod,
	 	     Gbl.CurrentDeg.Deg.DegCod,
	 	     Gbl.Usrs.Me.UsrDat.UsrCod);
         break;
      case Ntf_EVENT_FORUM_POST_COURSE:
	 // Check if forum is for users or for all users in the course
	 switch (For_GetForumTypeOfAPost (Cod))
	   {
	    case For_FORUM_COURSE_USRS:
	       sprintf (Query,"SELECT UsrCod FROM crs_usr"
			      " WHERE CrsCod='%ld' AND UsrCod<>'%ld'",
			Gbl.CurrentCrs.Crs.CrsCod,
			Gbl.Usrs.Me.UsrDat.UsrCod);
	       break;
	    case For_FORUM_COURSE_TCHS:
	       sprintf (Query,"SELECT UsrCod FROM crs_usr"
			      " WHERE CrsCod='%ld' AND Role='%u' AND UsrCod<>'%ld'",
			Gbl.CurrentCrs.Crs.CrsCod,
			(unsigned) Rol_TEACHER,
			Gbl.Usrs.Me.UsrDat.UsrCod);
	       break;
	    default:
	       return 0;
	   }
         break;
      case Ntf_EVENT_FORUM_REPLY:
         sprintf (Query,"SELECT DISTINCT(UsrCod) FROM forum_post"
                        " WHERE ThrCod = (SELECT ThrCod FROM forum_post"
                        " WHERE PstCod='%ld')"
                        " AND UsrCod<>'%ld'",
                  Cod,Gbl.Usrs.Me.UsrDat.UsrCod);
         break;
      case Ntf_EVENT_MESSAGE:		// This function should not be called in this case
	 return 0;
      case Ntf_EVENT_SURVEY:	// Only surveys for a course are notified, not surveys for a degree or global
         // 1. If the survey is available for the whole course ==> get users enrolled in the course whose role is available in survey, except me
         // 2. If the survey is available only for some groups ==> get users who belong to any of the groups and whose role is available in survey, except me
         // Cases 1 and 2 are mutually exclusive, so the union returns the case 1 or 2
         sprintf (Query,"(SELECT crs_usr.UsrCod"
                        " FROM surveys,crs_usr"
                        " WHERE surveys.SvyCod='%ld'"
                        " AND surveys.SvyCod NOT IN"
                        " (SELECT SvyCod FROM svy_grp WHERE SvyCod='%ld')"
                        " AND surveys.CrsCod=crs_usr.CrsCod"
                        " AND crs_usr.UsrCod<>'%ld'"
                        " AND (surveys.Roles&(1<<crs_usr.Role))<>0)"
                        " UNION "
                        "(SELECT DISTINCT crs_grp_usr.UsrCod"
                        " FROM svy_grp,crs_grp_usr,surveys,crs_usr"
                        " WHERE svy_grp.SvyCod='%ld'"
                        " AND svy_grp.GrpCod=crs_grp_usr.GrpCod"
                        " AND crs_grp_usr.UsrCod=crs_usr.UsrCod"
                        " AND crs_grp_usr.UsrCod<>'%ld'"
                        " AND svy_grp.SvyCod=surveys.SvyCod"
                        " AND surveys.CrsCod=crs_usr.CrsCod"
                        " AND (surveys.Roles&(1<<crs_usr.Role))<>0)",
                  Cod,Cod,Gbl.Usrs.Me.UsrDat.UsrCod,
                  Cod,Gbl.Usrs.Me.UsrDat.UsrCod);
         break;
      case Ntf_EVENT_FOLLOWER:	// This function should not be called in this case
         return 0;
     }

   if ((NumRows = DB_QuerySELECT (Query,&mysql_res,"can not get users to be notified"))) // Users found
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

	 if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat))		// Get user's data from the database
            if ((UsrDat.Prefs.NotifNtfEvents & NotifyEventMask))	// Create notification
              {
	       if ((UsrDat.Prefs.EmailNtfEvents & NotifyEventMask))	// Send notification by email
		 {
		  Ntf_StoreNotifyEventToOneUser (NotifyEvent,&UsrDat,Cod,
						 (Ntf_Status_t) Ntf_STATUS_BIT_EMAIL);
		  NumUsrsToBeNotifiedByEMail++;
		 }
	       else							// Don't send notification by email
		  Ntf_StoreNotifyEventToOneUser (NotifyEvent,&UsrDat,Cod,(Ntf_Status_t) 0);
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
                                    long Cod,Ntf_Status_t Status)
  {
   char Query[512];
   long InsCod;
   long CtrCod;
   long DegCod;
   long CrsCod;
   // long GrpCod;

   if (NotifyEvent == Ntf_EVENT_FORUM_POST_COURSE ||
       NotifyEvent == Ntf_EVENT_FORUM_REPLY)
     {
      InsCod = Gbl.Forum.Ins.InsCod;
      CtrCod = Gbl.Forum.Ctr.CtrCod;
      DegCod = Gbl.Forum.Deg.DegCod;
      CrsCod = Gbl.Forum.Crs.CrsCod;
      // GrpCod = -1L;	// There are no forums for a group
     }
   else
     {
      InsCod = Gbl.CurrentIns.Ins.InsCod;
      CtrCod = Gbl.CurrentCtr.Ctr.CtrCod;
      DegCod = Gbl.CurrentDeg.Deg.DegCod;
      CrsCod = Gbl.CurrentCrs.Crs.CrsCod;
      // GrpCod = Gbl.CurrentCrs.Grps.GrpCod;
     }

   /***** Store notify event *****/
   sprintf (Query,"INSERT INTO notif (NotifyEvent,ToUsrCod,FromUsrCod,"
	          "InsCod,CtrCod,DegCod,CrsCod,"
	          "Cod,TimeNotif,Status)"
                  " VALUES ('%u','%ld','%ld',"
                  "'%ld','%ld','%ld','%ld',"
                  "'%ld',NOW(),'%u')",
            (unsigned) NotifyEvent,UsrDat->UsrCod,Gbl.Usrs.Me.UsrDat.UsrCod,
            InsCod,CtrCod,DegCod,CrsCod,
            Cod,(unsigned) Status);
   DB_QueryINSERT (Query,"can not create new notification event");
  }

/*****************************************************************************/
/********* Reset my number of new received notifications to 0 ****************/
/*****************************************************************************/

static void Ntf_UpdateMyLastAccessToNotifications (void)
  {
   char Query[512];

   /***** Reset to 0 my number of new received messages *****/
   sprintf (Query,"UPDATE usr_last SET LastAccNotif=NOW()"
	          " WHERE UsrCod='%ld'",
            Gbl.Usrs.Me.UsrDat.UsrCod);
   DB_QueryUPDATE (Query,"can not update last access to notifications");
  }

/*****************************************************************************/
/***************** Send all pending notifications by e-mail ******************/
/*****************************************************************************/

void Ntf_SendPendingNotifByEMailToAllUsrs (void)
  {
   char Query[512];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRow,NumRows;
   struct UsrData UsrDat;
   unsigned NumNotif;
   unsigned NumTotalNotif = 0;
   unsigned NumMails;
   unsigned NumTotalMails = 0;

   /***** Get users who must be notified from database ******/
   // (Status & Ntf_STATUS_BIT_EMAIL) && !(Status & Ntf_STATUS_BIT_SENT) && !(Status & (Ntf_STATUS_BIT_READ | Ntf_STATUS_BIT_REMOVED))
   sprintf (Query,"SELECT DISTINCT ToUsrCod FROM notif"
                  " WHERE UNIX_TIMESTAMP(TimeNotif) < UNIX_TIMESTAMP()-%lu"
                  " AND (Status & %u)<>0 AND (Status & %u)=0 AND (Status & %u)=0",
            Cfg_TIME_TO_SEND_PENDING_NOTIF,
            (unsigned) Ntf_STATUS_BIT_EMAIL,
            (unsigned) Ntf_STATUS_BIT_SENT,
            (unsigned) (Ntf_STATUS_BIT_READ | Ntf_STATUS_BIT_REMOVED));
   if ((NumRows = DB_QuerySELECT (Query,&mysql_res,"can not get users who must be notified"))) // Events found
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
	 if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat))		// Get user's data from the database
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
   sprintf (Query,"DELETE LOW_PRIORITY FROM notif"
	          " WHERE UNIX_TIMESTAMP(TimeNotif) < UNIX_TIMESTAMP()-%lu",
            Cfg_TIME_TO_DELETE_OLD_NOTIF);
   DB_QueryDELETE (Query,"can not remove old notifications");
  }

/*****************************************************************************/
/************ Send pending notifications of one user by e-mail ***************/
/*****************************************************************************/

static void Ntf_SendPendingNotifByEMailToOneUsr (struct UsrData *ToUsrDat,unsigned *NumNotif,unsigned *NumMails)
  {
   extern const char *Txt_NOTIFY_EVENTS_There_is_a_new_event_NO_HTML[Txt_NUM_LANGUAGES];
   extern const char *Txt_NOTIFY_EVENTS_There_are_X_new_events_NO_HTML[Txt_NUM_LANGUAGES];
   extern const char *Txt_NOTIFY_EVENTS_SINGULAR_NO_HTML[Ntf_NUM_NOTIFY_EVENTS][Txt_NUM_LANGUAGES];
   extern const char *Txt_Course_NO_HTML[Txt_NUM_LANGUAGES];
   extern const char *Txt_Forum_NO_HTML[Txt_NUM_LANGUAGES];
   extern const char *Txt_MSG_From_NO_HTML[Txt_NUM_LANGUAGES];
   extern const char *Txt_Go_to_NO_HTML[Txt_NUM_LANGUAGES];
   extern const char *Txt_TAB_Messages_NO_HTML[Txt_NUM_LANGUAGES];
   extern const char *Txt_Notifications_NO_HTML[Txt_NUM_LANGUAGES];
   extern const char *Txt_If_you_no_longer_wish_to_receive_email_notifications_NO_HTML[Txt_NUM_LANGUAGES];
   char MailDomain[Cns_MAX_BYTES_STRING+1];
   char Query[512];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRow,NumRows;
   struct UsrData FromUsrDat;
   Ntf_NotifyEvent_t NotifyEvent = (Ntf_NotifyEvent_t) 0;	// Initialized to avoid warning
   struct Institution Ins;
   struct Centre Ctr;
   struct Degree Deg;
   struct Course Crs;
   long Cod;
   For_ForumType_t ForumType = (For_ForumType_t) 0;	// Initialized to avoid warning
   char ForumName[512];
   char Command[2048]; // Command to execute for sending an e-mail
   int ReturnCode;

   /***** Return 0 notifications and 0 mails when error *****/
   *NumNotif = *NumMails = 0;

   Str_GetMailBox (ToUsrDat->Email,MailDomain,Cns_MAX_BYTES_STRING);
   if (Mai_CheckIfMailDomainIsAllowedForNotifications (MailDomain))
     {
      /***** Get pending notifications of this user from database ******/
      sprintf (Query,"SELECT NotifyEvent,FromUsrCod,InsCod,CtrCod,DegCod,CrsCod,"
	             "Cod,DATE_FORMAT(TimeNotif,'%%Y-%%m-%%d %%H:%%i:%%S')"
		     " FROM notif WHERE ToUsrCod='%ld'"
		     " AND (Status & %u)<>0 AND (Status & %u)=0 AND (Status & %u)=0"
		     " ORDER BY TimeNotif,NotifyEvent",
	       ToUsrDat->UsrCod,
	       (unsigned) Ntf_STATUS_BIT_EMAIL,(unsigned) Ntf_STATUS_BIT_SENT,(unsigned) (Ntf_STATUS_BIT_READ | Ntf_STATUS_BIT_REMOVED));
      NumRows = DB_QuerySELECT (Query,&mysql_res,"can not get pending notifications of a user");

      if (NumRows) // Events found
	{
	 /***** Create temporary file for mail content *****/
	 Mai_CreateFileNameMail ();

	 /***** Welcome note *****/
	 Mai_WriteWelcomeNoteEMail (ToUsrDat);
	 if (NumRows == 1)
	    fprintf (Gbl.Msg.FileMail,Txt_NOTIFY_EVENTS_There_is_a_new_event_NO_HTML[ToUsrDat->Prefs.Language],
		     Cfg_PLATFORM_SHORT_NAME);
	 else
	    fprintf (Gbl.Msg.FileMail,Txt_NOTIFY_EVENTS_There_are_X_new_events_NO_HTML[ToUsrDat->Prefs.Language],
		     (unsigned) NumRows,Cfg_PLATFORM_SHORT_NAME);
	 fprintf (Gbl.Msg.FileMail,": \n");

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
	    NotifyEvent = Ntf_GetNotifyEventFromDB ((const char *) row[0]);

	    /* Get origin user code (row[1]) */
	    FromUsrDat.UsrCod = Str_ConvertStrCodToLongCod (row[1]);
	    Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&FromUsrDat);		// Get origin user's data from the database

	    /* Get institution code (row[2]) */
	    Ins.InsCod = Str_ConvertStrCodToLongCod (row[2]);
	    Ins_GetDataOfInstitutionByCod (&Ins,Ins_GET_MINIMAL_DATA);

	    /* Get centre code (row[3]) */
	    Ctr.CtrCod = Str_ConvertStrCodToLongCod (row[3]);
	    Ctr_GetDataOfCentreByCod (&Ctr);

	    /* Get degree code (row[4]) */
	    Deg.DegCod = Str_ConvertStrCodToLongCod (row[4]);
	    Deg_GetDataOfDegreeByCod (&Deg);

	    /* Get course code (row[5]) */
	    Crs.CrsCod = Str_ConvertStrCodToLongCod (row[5]);
	    Crs_GetDataOfCourseByCod (&Crs);

	    /* Get message/post/... code (row[6]) */
	    Cod = Str_ConvertStrCodToLongCod (row[6]);

	    /* Get forum type */
	    if (NotifyEvent == Ntf_EVENT_FORUM_POST_COURSE ||
	        NotifyEvent == Ntf_EVENT_FORUM_REPLY)
	       ForumType = For_GetForumTypeOfAPost (Cod);

	    /* Write date and time when the event happened (row[7], in YYYY-MM-DD HH:MM:SS format) */
	    fprintf (Gbl.Msg.FileMail,"\n%s: ",row[7]);

	    /* Information about the type of this event */
	    fprintf (Gbl.Msg.FileMail,Txt_NOTIFY_EVENTS_SINGULAR_NO_HTML[NotifyEvent][ToUsrDat->Prefs.Language],
		     Cfg_PLATFORM_SHORT_NAME);
	    fprintf (Gbl.Msg.FileMail,"\n");

	    /* Course/forum: */
	    switch (NotifyEvent)
	      {
	       case Ntf_EVENT_UNKNOWN:
	       case Ntf_EVENT_FOLLOWER:
		  break;
	       case Ntf_EVENT_DOCUMENT_FILE:
	       case Ntf_EVENT_SHARED_FILE:
	       case Ntf_EVENT_ASSIGNMENT:
	       case Ntf_EVENT_EXAM_ANNOUNCEMENT:
	       case Ntf_EVENT_MARKS_FILE:
	       case Ntf_EVENT_ENROLLMENT:
	       case Ntf_EVENT_ENROLLMENT_REQUEST:
	       case Ntf_EVENT_NOTICE:
	       case Ntf_EVENT_MESSAGE:
	       case Ntf_EVENT_SURVEY:
		  if (Crs.CrsCod > 0)
		     fprintf (Gbl.Msg.FileMail,"%s: %s\n",
			      Txt_Course_NO_HTML[ToUsrDat->Prefs.Language],
			      Crs.FullName);
		  break;
	       case Ntf_EVENT_FORUM_POST_COURSE:
	       case Ntf_EVENT_FORUM_REPLY:
		  For_SetForumName (ForumType,
				    &Ins,
				    &Ctr,
				    &Deg,
				    &Crs,
				    ForumName,ToUsrDat->Prefs.Language,false);	// Set forum name in recipient's language
		  fprintf (Gbl.Msg.FileMail,"%s: %s\n",
			   Txt_Forum_NO_HTML[ToUsrDat->Prefs.Language],
			   ForumName);
		  break;
	      }
	    /* From: */
	    fprintf (Gbl.Msg.FileMail,"%s: %s\n",
		     Txt_MSG_From_NO_HTML[ToUsrDat->Prefs.Language],
		     FromUsrDat.FullName);
	   }

	 /***** Free memory used for origin user's data *****/
	 Usr_UsrDataDestructor (&FromUsrDat);

	 /* Go to: */
	 fprintf (Gbl.Msg.FileMail,"%s: %s/ > %s > %s\n",
		  Txt_Go_to_NO_HTML[ToUsrDat->Prefs.Language],
		  Cfg_HTTPS_URL_SWAD_CGI,
		  Txt_TAB_Messages_NO_HTML[ToUsrDat->Prefs.Language],
		  Txt_Notifications_NO_HTML[ToUsrDat->Prefs.Language]);

	 /* Disclaimer */
	 fprintf (Gbl.Msg.FileMail,"\n%s\n",
		  Txt_If_you_no_longer_wish_to_receive_email_notifications_NO_HTML[ToUsrDat->Prefs.Language]);

	 /* Footer note */
	 Mai_WriteFootNoteEMail (ToUsrDat->Prefs.Language);

	 fclose (Gbl.Msg.FileMail);

	 /***** Call the command to send an e-mail *****/
	 sprintf (Command,"%s \"%s\" \"%s\" \"%s\" \"%s\" \"%s\" \"[%s] %s\" \"%s\"",
		  Cfg_COMMAND_SEND_AUTOMATIC_E_MAIL,
		  Cfg_AUTOMATIC_EMAIL_SMTP_SERVER,
		  Cfg_AUTOMATIC_EMAIL_SMTP_PORT,
		  Cfg_AUTOMATIC_EMAIL_FROM,
                  Gbl.Config.SMTPPassword,
		  ToUsrDat->Email,
		  Cfg_PLATFORM_SHORT_NAME,Txt_Notifications_NO_HTML[ToUsrDat->Prefs.Language],
		  Gbl.Msg.FileNameMail);
	 ReturnCode = system (Command);
	 if (ReturnCode == -1)
	    Lay_ShowErrorAndExit ("Error when running script to send e-mail.");

	 /***** Remove temporary file *****/
	 unlink (Gbl.Msg.FileNameMail);

	 /***** Update number of notifications, number of mails and statistics *****/
	 ReturnCode = WEXITSTATUS(ReturnCode);
	 if (ReturnCode == 0)	// Message sent successfully
	   {
	    *NumNotif = (unsigned) NumRows;
	    *NumMails = 1;

	    /* Update statistics about notifications */
	    Ntf_UpdateNumNotifSent (Deg.DegCod,Crs.CrsCod,NotifyEvent,*NumNotif,*NumMails);
	   }

	 /***** Mark all the pending notifications of this user as 'sent' *****/
	 sprintf (Query,"UPDATE notif SET Status=(Status | %u)"
	                " WHERE ToUsrCod='%ld'"
			" AND (Status & %u)<>0 AND (Status & %u)=0  AND (Status & %u)=0",
		  (unsigned) Ntf_STATUS_BIT_SENT,ToUsrDat->UsrCod,
		  (unsigned) Ntf_STATUS_BIT_EMAIL,
		  (unsigned) Ntf_STATUS_BIT_SENT,
		  (unsigned) (Ntf_STATUS_BIT_READ | Ntf_STATUS_BIT_REMOVED));
	 DB_QueryUPDATE (Query,"can not set pending notifications of a user as sent");
	}

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);
     }
  }

/*****************************************************************************/
/****** Get notify event type from string number coming from database ********/
/*****************************************************************************/

Ntf_NotifyEvent_t Ntf_GetNotifyEventFromDB (const char *Str)
  {
   unsigned UnsignedNum;

   if (sscanf (Str,"%u",&UnsignedNum) == 1)
      if (UnsignedNum < Ntf_NUM_NOTIFY_EVENTS)
         return (Ntf_NotifyEvent_t) UnsignedNum;

   return Ntf_EVENT_UNKNOWN;
  }

/*****************************************************************************/
/******** Show an alert with the number of users notified by e-mail **********/
/*****************************************************************************/

void Ntf_ShowAlertNumUsrsToBeNotifiedByEMail (unsigned NumUsrsToBeNotifiedByEMail)
  {
   extern const char *Txt_No_of_users_who_will_be_notified_by_e_mail;

   sprintf (Gbl.Message,"%s: %u",
            Txt_No_of_users_who_will_be_notified_by_e_mail,NumUsrsToBeNotifiedByEMail);
   Lay_ShowAlert (Lay_INFO,Gbl.Message);
  }

/*****************************************************************************/
/************* Get number of events notified and e-mails sent ****************/
/*****************************************************************************/

static void Ntf_GetNumNotifSent (long DegCod,long CrsCod,
                                 Ntf_NotifyEvent_t NotifyEvent,
                                 unsigned *NumEvents,unsigned *NumMails)
  {
   char Query[512];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;

   /***** Get number of notifications sent by e-mail from database *****/
   sprintf (Query,"SELECT NumEvents,NumMails FROM sta_notif"
                  " WHERE DegCod='%ld' AND CrsCod='%ld' AND NotifyEvent='%u'",
            DegCod,CrsCod,(unsigned) NotifyEvent);
   NumRows = DB_QuerySELECT (Query,&mysql_res,"can not get number of notifications sent by e-mail");

   /***** Get number of rows *****/
   if (NumRows)
     {
      row = mysql_fetch_row (mysql_res);
      if (sscanf (row[0],"%u",NumEvents) != 1)
         Lay_ShowErrorAndExit ("Error when getting number of notifications sent by e-mail.");
      if (sscanf (row[1],"%u",NumMails) != 1)
         Lay_ShowErrorAndExit ("Error when getting number of notifications sent by e-mail.");
     }
   else
      *NumEvents = *NumMails = 0;

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/******************** Update number of notify e-mails sent *******************/
/*****************************************************************************/

static void Ntf_UpdateNumNotifSent (long DegCod,long CrsCod,
                                    Ntf_NotifyEvent_t NotifyEvent,
                                    unsigned NumEvents,unsigned NumMails)
  {
   char Query[512];
   unsigned CurrentNumEvents;
   unsigned CurrentNumMails;

   /***** Get number of events notified and number of mails sent *****/
   Ntf_GetNumNotifSent (DegCod,CrsCod,NotifyEvent,&CurrentNumEvents,&CurrentNumMails);

   /***** Update number of users notified *****/
   sprintf (Query,"REPLACE INTO sta_notif (DegCod,CrsCod,NotifyEvent,NumEvents,NumMails)"
	          " VALUES ('%ld','%ld','%u','%u','%u')",
            DegCod,CrsCod,(unsigned) NotifyEvent,
            CurrentNumEvents + NumEvents,
            CurrentNumMails + NumMails);
   DB_QueryREPLACE (Query,"can not update the number of sent notifications");
  }

/*****************************************************************************/
/********* Put a link (form) to edit preferences about notifications *********/
/*****************************************************************************/

static void Ntf_PutLinkToChangePrefs (void)
  {
   extern const char *Txt_Change_preferences;

   Act_PutContextualLink (ActEdiPrf,NULL,"heart",Txt_Change_preferences);
  }

/*****************************************************************************/
/*********** Put a link (form) to mark all my notifications as seen **********/
/*****************************************************************************/

static void Ntf_PutLinkToMarkAllNotifAsSeen (void)
  {
   extern const char *The_ClassFormulB[The_NUM_THEMES];
   extern const char *Txt_Mark_all_notifications_as_read;

   Act_FormStart (ActMrkNtfSee);
   Act_LinkFormSubmit (Txt_Mark_all_notifications_as_read,The_ClassFormulB[Gbl.Prefs.Theme]);
   Lay_PutSendIcon ("visible_on",Txt_Mark_all_notifications_as_read,Txt_Mark_all_notifications_as_read);
   Act_FormEnd ();
  }

/*****************************************************************************/
/******************** Mark all my notifications as seen **********************/
/*****************************************************************************/

void Ntf_MarkAllNotifAsSeen (void)
  {
   char Query[256];

   /***** Set all my notifications as seen *****/
   sprintf (Query,"UPDATE notif SET Status=(Status | %u)"
		  " WHERE ToUsrCod='%ld'",
	    (unsigned) Ntf_STATUS_BIT_READ,
	    Gbl.Usrs.Me.UsrDat.UsrCod);
   DB_QueryUPDATE (Query,"can not set notification(s) as seen");

   /***** Show my notifications again *****/
   Ntf_ShowMyNotifications ();
  }

/*****************************************************************************/
/*** Activate the sending of e-mail to notify me that I have new messages ****/
/*****************************************************************************/

void Ntf_PutFormChangeNotifSentByEMail (void)
  {
   extern const char *The_ClassFormul[The_NUM_THEMES];
   extern const char *Txt_Save_changes;
   extern const char *Txt_Notifications;
   extern const char *Txt_Create_BR_notification;
   extern const char *Txt_Notify_me_BR_by_e_mail;
   extern const char *Txt_NOTIFY_EVENTS_PLURAL[Ntf_NUM_NOTIFY_EVENTS];
   Ntf_NotifyEvent_t NotifyEvent;

   /***** Start form *****/
   Act_FormStart (ActChgNtfPrf);

   /***** Start frame *****/
   Lay_StartRoundFrameTable10 (NULL,2,Txt_Notifications);
   fprintf (Gbl.F.Out,"<tr>"
		      "<th></th>"
		      "<th class=\"TIT_TBL\" style=\"text-align:center;\">"
		      "%s"
		      "</th>"
                      "<th class=\"TIT_TBL\" style=\"text-align:center;\">"
                      "%s"
                      "</th>"
	              "</tr>",
	    Txt_Create_BR_notification,
	    Txt_Notify_me_BR_by_e_mail);

   /***** Checkbox to activate the sending of e-mail to notify me about events *****/
   for (NotifyEvent = (Ntf_NotifyEvent_t) 1;
	NotifyEvent < Ntf_NUM_NOTIFY_EVENTS;
	NotifyEvent++)	// O is reserved for Ntf_EVENT_UNKNOWN
     {
      fprintf (Gbl.F.Out,"<tr>"
	                 "<td class=\"%s\" style=\"text-align:right;\">"
	                 "%s:"
	                 "</td>"
	                 "<td style=\"text-align:center;\">"
                         "<input type=\"checkbox\" name=\"%s\" value=\"Y\"",
               The_ClassFormul[Gbl.Prefs.Theme],
               Txt_NOTIFY_EVENTS_PLURAL[NotifyEvent],
               Ntf_ParamNotifMeAboutNotifyEvents[NotifyEvent]);
      if ((Gbl.Usrs.Me.UsrDat.Prefs.NotifNtfEvents & (1 << NotifyEvent)))
          fprintf (Gbl.F.Out," checked=\"checked\"");
      fprintf (Gbl.F.Out," />"
	                 "</td>"
	                 "<td style=\"text-align:center;\">"
                         "<input type=\"checkbox\" name=\"%s\" value=\"Y\"",
               Ntf_ParamEmailMeAboutNotifyEvents[NotifyEvent]);
      if ((Gbl.Usrs.Me.UsrDat.Prefs.EmailNtfEvents & (1 << NotifyEvent)))
          fprintf (Gbl.F.Out," checked=\"checked\"");
      fprintf (Gbl.F.Out," />"
	                 "</td>"
	                 "</tr>");
     }

   /***** Button to save changes and end frame *****/
   Lay_EndRoundFrameTable10 (Lay_CONFIRM_BUTTON,Txt_Save_changes);

   /***** End form *****/
   Act_FormEnd ();
  }

/*****************************************************************************/
/** Get parameter with the sending of e-mail to notify me that I have msgs. **/
/*****************************************************************************/

static void Ntf_GetParamsNotifyEvents (void)
  {
   Ntf_NotifyEvent_t NotifyEvent;
   char YN[1+1];
   bool CreateNotifForThisEvent;

   Gbl.Usrs.Me.UsrDat.Prefs.NotifNtfEvents = 0;
   Gbl.Usrs.Me.UsrDat.Prefs.EmailNtfEvents = 0;
   for (NotifyEvent = (Ntf_NotifyEvent_t) 1;
	NotifyEvent < Ntf_NUM_NOTIFY_EVENTS;
	NotifyEvent++)	// 0 is reserved for Ntf_EVENT_UNKNOWN
     {
      Par_GetParToText (Ntf_ParamNotifMeAboutNotifyEvents[NotifyEvent],YN,1);
      if ((CreateNotifForThisEvent = (Str_ConvertToUpperLetter (YN[0]) == 'Y')))
         Gbl.Usrs.Me.UsrDat.Prefs.NotifNtfEvents |= (1 << NotifyEvent);

      if (CreateNotifForThisEvent)
	{
         Par_GetParToText (Ntf_ParamEmailMeAboutNotifyEvents[NotifyEvent],YN,1);
         if (Str_ConvertToUpperLetter (YN[0]) == 'Y')
            Gbl.Usrs.Me.UsrDat.Prefs.EmailNtfEvents |= (1 << NotifyEvent);
	}
     }
  }

/*****************************************************************************/
/**** Change my preference about sending me notify e-mails about events ******/
/*****************************************************************************/

void Ntf_ChangeNotifyEvents (void)
  {
   extern const char *Txt_Your_preferences_about_notifications_have_changed;
   char Query[512];

   /***** Get param with whether notify me about events *****/
   Ntf_GetParamsNotifyEvents ();

   /***** Store preferences about notify events *****/
   sprintf (Query,"UPDATE usr_data"
	          " SET NotifNtfEvents='%u',EmailNtfEvents='%u'"
	          " WHERE UsrCod='%ld'",
            Gbl.Usrs.Me.UsrDat.Prefs.NotifNtfEvents,
            Gbl.Usrs.Me.UsrDat.Prefs.EmailNtfEvents,
            Gbl.Usrs.Me.UsrDat.UsrCod);
   DB_QueryUPDATE (Query,"can not update user's preferences");

   /***** Show message *****/
   Lay_ShowAlert (Lay_SUCCESS,Txt_Your_preferences_about_notifications_have_changed);
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
   extern const char *Txt_NOTIF_new_SINGULAR;
   extern const char *Txt_NOTIF_new_PLURAL;
   unsigned NumUnseenNtfs;
   unsigned NumNewNtfs = 0;

   /***** Get my number of unseen notifications *****/
   if ((NumUnseenNtfs = Ntf_GetNumberOfAllMyUnseenNtfs ()) != 0)
      NumNewNtfs = Ntf_GetNumberOfMyNewUnseenNtfs ();

   /***** Start form *****/
   Act_FormStartId (ActSeeNewNtf,"form_ntf");
   Act_LinkFormSubmitId (Txt_See_notifications,
                         The_ClassNotif[Gbl.Prefs.Theme],"form_ntf");

   /***** Number of unseen notifications *****/
   if (NumUnseenNtfs == 1)
      fprintf (Gbl.F.Out,"1 %s",
               Txt_notification);
   else
      fprintf (Gbl.F.Out,"%u %s",
               NumUnseenNtfs,
               Txt_notifications);

   if (NumNewNtfs)
     {
      /***** Icon *****/
      fprintf (Gbl.F.Out,"<br />"
	                 "<img src=\"%s/bell16x16.png\""
	                 " style=\"width:16px; height:16px;"
	                 " vertical-align:middle;\" /> ",
               Gbl.Prefs.PathTheme);

      /***** Number of new notifications *****/
      if (NumNewNtfs == 1)
         fprintf (Gbl.F.Out,"1 %s",
               Txt_NOTIF_new_SINGULAR);
      else
         fprintf (Gbl.F.Out,"%u %s",
                  NumNewNtfs,
                  Txt_NOTIF_new_PLURAL);
     }

   /***** End form *****/
   fprintf (Gbl.F.Out,"</a>");
   Act_FormEnd ();
  }

/*****************************************************************************/
/************* Get the number of (all) my unseen notifications ***************/
/*****************************************************************************/

static unsigned Ntf_GetNumberOfAllMyUnseenNtfs (void)
  {
   char Query[512];

   /***** Get number of places with a name from database *****/
   sprintf (Query,"SELECT COUNT(*) FROM notif"
                  " WHERE ToUsrCod='%ld' AND (Status & %u)=0",
            Gbl.Usrs.Me.UsrDat.UsrCod,(unsigned) (Ntf_STATUS_BIT_READ | Ntf_STATUS_BIT_REMOVED));
   return DB_QueryCOUNT (Query,"can not get number of unseen notifications");
  }

/*****************************************************************************/
/************** Get the number of my new unseen notifications ****************/
/*****************************************************************************/

static unsigned Ntf_GetNumberOfMyNewUnseenNtfs (void)
  {
   char Query[256];

   /***** Get number of places with a name from database *****/
   sprintf (Query,"SELECT COUNT(*) FROM notif"
                  " WHERE ToUsrCod='%ld' AND (Status & %u)=0"
                  " AND UNIX_TIMESTAMP(TimeNotif)>'%ld'",
            Gbl.Usrs.Me.UsrDat.UsrCod,
            (unsigned) (Ntf_STATUS_BIT_READ | Ntf_STATUS_BIT_REMOVED),
            Gbl.Usrs.Me.UsrLast.LastAccNotif);
   return DB_QueryCOUNT (Query,"can not get number of unseen notifications");
  }

/*****************************************************************************/
/**************** Remove all notifications made to a user ********************/
/*****************************************************************************/

void Ntf_RemoveUsrNtfs (long ToUsrCod)
  {
   char Query[256];

   /***** Delete notifications of a user ******/
   sprintf (Query,"DELETE LOW_PRIORITY FROM notif"
	          " WHERE ToUsrCod='%ld'",
            ToUsrCod);
   DB_QueryDELETE (Query,"can not remove notifications of a user");
  }
