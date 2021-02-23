// swad_timeline_note.c: social timeline notes

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2021 Antonio Cañas Vargas

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General 3 License as
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

#define _GNU_SOURCE 		// For asprintf
#include <linux/limits.h>	// For PATH_MAX
#include <stdio.h>		// For asprintf

#include "swad_alert.h"
#include "swad_box.h"
#include "swad_course.h"
#include "swad_database.h"
#include "swad_exam_announcement.h"
#include "swad_forum.h"
#include "swad_global.h"
#include "swad_hierarchy.h"
#include "swad_notice.h"
#include "swad_photo.h"
#include "swad_profile.h"
#include "swad_timeline.h"
#include "swad_timeline_favourite.h"
#include "swad_timeline_form.h"
#include "swad_timeline_publication.h"
#include "swad_timeline_share.h"

/*****************************************************************************/
/****************************** Public constants *****************************/
/*****************************************************************************/

/*****************************************************************************/
/************************* Private constants and types ***********************/
/*****************************************************************************/

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/************************* Private global variables **************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void TL_Not_WriteTopMessage (TL_TopMessage_t TopMessage,long PublisherCod);
static void TL_Not_WriteNote (const struct TL_Timeline *Timeline,
                              const struct TL_Not_Note *Not);
static void TL_Not_ShowAuthorPhoto (struct UsrData *UsrDat);
static void TL_Not_WriteAuthorTimeAndContent (const struct TL_Not_Note *Not,
                                              const struct UsrData *UsrDat);

static void TL_Not_WriteContent (const struct TL_Not_Note *Not);
static void TL_Not_GetAndWriteNoPost (const struct TL_Not_Note *Not);
static void TL_Not_GetLocationInHierarchy (const struct TL_Not_Note *Not,
                                           struct Hie_Hierarchy *Hie,
                                           struct For_Forum *Forum,
                                           char ForumName[For_MAX_BYTES_FORUM_NAME + 1]);
static void TL_Not_WriteLocationInHierarchy (const struct TL_Not_Note *Not,
	                                     const struct Hie_Hierarchy *Hie,
                                             const char ForumName[For_MAX_BYTES_FORUM_NAME + 1]);

static void TL_Not_PutFormGoToAction (const struct TL_Not_Note *Not,
                                      const struct For_Forums *Forums);

static void TL_Not_WriteButtonsAndComments (const struct TL_Timeline *Timeline,
                                const struct TL_Not_Note *Not,
                                const struct UsrData *UsrDat);
static void TL_Not_WriteButtonToAddAComment (const struct TL_Not_Note *Not,
                                             const char IdNewComment[Frm_MAX_BYTES_ID + 1]);
static void TL_Not_WriteFavShaRemAndComments (const struct TL_Timeline *Timeline,
					      const struct TL_Not_Note *Not,
					      const struct UsrData *UsrDat);
static void TL_Not_WriteFavShaRem (const struct TL_Timeline *Timeline,
                                   const struct TL_Not_Note *Not,
                                   const struct UsrData *UsrDat);

static void TL_Not_PutFormToRemoveNote (const struct TL_Timeline *Timeline,
                                        long NotCod);

static void TL_Not_RequestRemovalNote (struct TL_Timeline *Timeline);
static void TL_Not_PutParamsRemoveNote (void *Timeline);
static void TL_Not_RemoveNote (void);
static void TL_Not_RemoveNoteMediaAndDBEntries (struct TL_Not_Note *Not);

static void TL_Not_GetDataOfNoteFromRow (MYSQL_ROW row,struct TL_Not_Note *Not);

static TL_Not_NoteType_t TL_Not_GetNoteTypeFromStr (const char *Str);

static void TL_Not_ResetNote (struct TL_Not_Note *Not);

/*****************************************************************************/
/****************** Show highlighted note above timeline *********************/
/*****************************************************************************/

void TL_Not_ShowHighlightedNote (struct TL_Timeline *Timeline,
                                 struct TL_Not_Note *Not)
  {
   struct UsrData PublisherDat;
   Ntf_NotifyEvent_t NotifyEvent;
   static const TL_TopMessage_t TopMessages[Ntf_NUM_NOTIFY_EVENTS] =
     {
      [Ntf_EVENT_UNKNOWN          ] = TL_TOP_MESSAGE_NONE,
      /* Start tab */
      [Ntf_EVENT_TIMELINE_COMMENT ] = TL_TOP_MESSAGE_COMMENTED,
      [Ntf_EVENT_TIMELINE_FAV     ] = TL_TOP_MESSAGE_FAVED,
      [Ntf_EVENT_TIMELINE_SHARE   ] = TL_TOP_MESSAGE_SHARED,
      [Ntf_EVENT_TIMELINE_MENTION ] = TL_TOP_MESSAGE_MENTIONED,
      [Ntf_EVENT_FOLLOWER         ] = TL_TOP_MESSAGE_NONE,
      /* System tab */
      /* Country tab */
      /* Institution tab */
      /* Centre tab */
      /* Degree tab */
      /* Course tab */
      /* Assessment tab */
      [Ntf_EVENT_ASSIGNMENT       ] = TL_TOP_MESSAGE_NONE,
      [Ntf_EVENT_SURVEY           ] = TL_TOP_MESSAGE_NONE,
      [Ntf_EVENT_EXAM_ANNOUNCEMENT] = TL_TOP_MESSAGE_NONE,
      /* Files tab */
      [Ntf_EVENT_DOCUMENT_FILE    ] = TL_TOP_MESSAGE_NONE,
      [Ntf_EVENT_TEACHERS_FILE    ] = TL_TOP_MESSAGE_NONE,
      [Ntf_EVENT_SHARED_FILE      ] = TL_TOP_MESSAGE_NONE,
      [Ntf_EVENT_MARKS_FILE       ] = TL_TOP_MESSAGE_NONE,
      /* Users tab */
      [Ntf_EVENT_ENROLMENT_STD    ] = TL_TOP_MESSAGE_NONE,
      [Ntf_EVENT_ENROLMENT_NET    ] = TL_TOP_MESSAGE_NONE,
      [Ntf_EVENT_ENROLMENT_TCH    ] = TL_TOP_MESSAGE_NONE,
      [Ntf_EVENT_ENROLMENT_REQUEST] = TL_TOP_MESSAGE_NONE,
      /* Messages tab */
      [Ntf_EVENT_NOTICE           ] = TL_TOP_MESSAGE_NONE,
      [Ntf_EVENT_FORUM_POST_COURSE] = TL_TOP_MESSAGE_NONE,
      [Ntf_EVENT_FORUM_REPLY      ] = TL_TOP_MESSAGE_NONE,
      [Ntf_EVENT_MESSAGE          ] = TL_TOP_MESSAGE_NONE,
      /* Analytics tab */
      /* Profile tab */
     };

   /***** Get other parameters *****/
   /* Get the publisher who did the action
      (publishing, commenting, faving, sharing, mentioning) */
   Usr_GetParamOtherUsrCodEncrypted (&PublisherDat);

   /* Get what he/she did */
   NotifyEvent = Ntf_GetParamNotifyEvent ();

   /***** Get data of the note *****/
   TL_Not_GetDataOfNoteByCod (Not);

   /***** Show the note highlighted *****/
   Box_BoxBegin (NULL,NULL,
		 NULL,NULL,
		 NULL,Box_CLOSABLE);
   HTM_DIV_Begin ("class=\"TL_WIDTH TL_NEW_PUB\"");
   TL_Not_CheckAndWriteNoteWithTopMsg (Timeline,Not,
		                       TopMessages[NotifyEvent],
		                       PublisherDat.UsrCod);
   HTM_DIV_End ();
   Box_BoxEnd ();
  }

/*****************************************************************************/
/**** Insert note in temporary tables used to not get notes already shown ****/
/*****************************************************************************/

void TL_Not_InsertNoteInJustRetrievedNotes (long NotCod)
  {
   /* Insert note in temporary table with just retrieved notes.
      This table will be used to not get notes already shown */
   DB_QueryINSERT ("can not store note code",
		   "INSERT IGNORE INTO tl_tmp_just_retrieved_notes"
		   " SET NotCod=%ld",
		   NotCod);
  }

void TL_Not_InsertNoteInVisibleTimeline (long NotCod)
  {
   /* Insert note in temporary table with visible timeline.
      This table will be used to not get notes already shown */
   DB_QueryINSERT ("can not store note code",
		   "INSERT IGNORE INTO tl_tmp_visible_timeline"
		   " SET NotCod=%ld",
		   NotCod);
  }

/*****************************************************************************/
/****************** Check and write note with top message ********************/
/*****************************************************************************/

void TL_Not_CheckAndWriteNoteWithTopMsg (const struct TL_Timeline *Timeline,
	                                 const struct TL_Not_Note *Not,
                                         TL_TopMessage_t TopMessage,
                                         long PublisherCod)	// Who did the action (publication, commenting, faving, sharing, mentioning)
  {
   /*__________________________________________
   |                                           | \
   | Top message:                              |   > top message
   |___________________________________________| /
   | _____  |                      |           | \              \
   ||     | | Author's name        | Date-time |  |              |
   ||Auth.| |______________________|___________|  |              |
   ||photo| |                                  |  |  author's    |
   ||_____| |                                  |   > name, time  |
   |        |               Note               |  |  and content |
   |        |             content              |  |              |
   |        |                                  |  |              |
   |        |__________________________________| /               |
   |        |             |             |      | \               |
   |        | Favs        | Shared      |Remove|  |               > note
   |        |_____________|_____________|______|  |              |
   |________|                                  |  |              |
   |        |             List                 |  |  buttons     |
   | Comment|              of                  |   > and         |
   | icon   |           comments               |  |  comments    |
   |________|__________________________________|  |              |
            |                                  |  |              |
            |    Form to write new comment     |  |              |
            |__________________________________| /              /
   */
   if (Not->NotCod   > 0 &&
       Not->UsrCod   > 0 &&
       Not->NoteType != TL_NOTE_UNKNOWN)
     {
      /***** Write sharer/commenter if distinct to author *****/
      if (TopMessage != TL_TOP_MESSAGE_NONE)
         TL_Not_WriteTopMessage (TopMessage,PublisherCod);

      /***** Write note *****/
      TL_Not_WriteNote (Timeline,Not);
     }
   else
      Ale_ShowAlert (Ale_ERROR,"Error in note.");
  }

/*****************************************************************************/
/*************** Write sharer/commenter if distinct to author ****************/
/*****************************************************************************/

static void TL_Not_WriteTopMessage (TL_TopMessage_t TopMessage,long PublisherCod)
  {
   extern const char *Txt_My_public_profile;
   extern const char *Txt_Another_user_s_profile;
   extern const char *Txt_TIMELINE_NOTE_TOP_MESSAGES[TL_NUM_TOP_MESSAGES];
   struct UsrData PublisherDat;

   /***** Initialize structure with user's data *****/
   Usr_UsrDataConstructor (&PublisherDat);

   /***** Get user's data *****/
   PublisherDat.UsrCod = PublisherCod;
   if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&PublisherDat,Usr_DONT_GET_PREFS))	// Really we only need EncryptedUsrCod and FullName
     {
      HTM_DIV_Begin ("class=\"TL_TOP_CONT TL_TOP_PUBLISHER TL_WIDTH\"");

      /***** Show user's name inside form to go to user's public profile *****/
      Frm_StartFormUnique (ActSeeOthPubPrf);
      Usr_PutParamUsrCodEncrypted (PublisherDat.EnUsrCod);
      HTM_BUTTON_SUBMIT_Begin (Usr_ItsMe (PublisherCod) ? Txt_My_public_profile :
				                          Txt_Another_user_s_profile,
			       "BT_LINK TL_TOP_PUBLISHER",NULL);
      HTM_Txt (PublisherDat.FullName);
      HTM_BUTTON_End ();
      Frm_EndForm ();

      /***** Show action made *****/
      HTM_TxtF (" %s:",Txt_TIMELINE_NOTE_TOP_MESSAGES[TopMessage]);

      HTM_DIV_End ();
     }

   /***** Free memory used for user's data *****/
   Usr_UsrDataDestructor (&PublisherDat);
  }

/*****************************************************************************/
/********************************* Show note *********************************/
/*****************************************************************************/

static void TL_Not_WriteNote (const struct TL_Timeline *Timeline,
                              const struct TL_Not_Note *Not)
  {
   struct UsrData UsrDat;	// Author of the note

   /***** Get author data *****/
   Usr_UsrDataConstructor (&UsrDat);
   UsrDat.UsrCod = Not->UsrCod;
   Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat,Usr_DONT_GET_PREFS);

   /***** Left top: author's photo *****/
   TL_Not_ShowAuthorPhoto (&UsrDat);

   /***** Right top: author's name, time, and content *****/
   TL_Not_WriteAuthorTimeAndContent (Not,&UsrDat);

   /***** Bottom: buttons and comments *****/
   TL_Not_WriteButtonsAndComments (Timeline,Not,&UsrDat);

   /***** Free memory used for author's data *****/
   Usr_UsrDataDestructor (&UsrDat);
  }

/*****************************************************************************/
/*********************** Show photo of author of a note **********************/
/*****************************************************************************/

static void TL_Not_ShowAuthorPhoto (struct UsrData *UsrDat)
  {
   bool ShowPhoto = false;
   char PhotoURL[PATH_MAX + 1];

   /***** Show author's photo *****/
   HTM_DIV_Begin ("class=\"TL_LEFT_PHOTO\"");
   ShowPhoto = Pho_ShowingUsrPhotoIsAllowed (UsrDat,PhotoURL);
   Pho_ShowUsrPhoto (UsrDat,ShowPhoto ? PhotoURL :
					NULL,
		     "PHOTO45x60",Pho_ZOOM,true);	// Use unique id
   HTM_DIV_End ();
  }

/*****************************************************************************/
/**** Write top right part of a note: author's name, time and note content ***/
/*****************************************************************************/

static void TL_Not_WriteAuthorTimeAndContent (const struct TL_Not_Note *Not,
                                              const struct UsrData *UsrDat)
  {
   /***** Begin top container *****/
   HTM_DIV_Begin ("class=\"TL_RIGHT_CONT TL_RIGHT_WIDTH\"");

   /***** Write author's full name *****/
   TL_Not_WriteAuthorName (UsrDat);

   /***** Write date and time *****/
   TL_WriteDateTime (Not->DateTimeUTC);

   /***** Write content of the note *****/
   TL_Not_WriteContent (Not);

   /***** End top container *****/
   HTM_DIV_End ();
  }

/*****************************************************************************/
/*************** Write name and nickname of author of a note *****************/
/*****************************************************************************/

void TL_Not_WriteAuthorName (const struct UsrData *UsrDat)
  {
   extern const char *Txt_My_public_profile;
   extern const char *Txt_Another_user_s_profile;

   /***** Show user's name inside form to go to user's public profile *****/
   Frm_StartFormUnique (ActSeeOthPubPrf);
   Usr_PutParamUsrCodEncrypted (UsrDat->EnUsrCod);
   HTM_BUTTON_SUBMIT_Begin (Usr_ItsMe (UsrDat->UsrCod) ? Txt_My_public_profile :
				                         Txt_Another_user_s_profile,
		            "BT_LINK TL_RIGHT_AUTHOR TL_RIGHT_AUTHOR_WIDTH DAT_N_BOLD",
			    NULL);
   HTM_Txt (UsrDat->FullName);
   HTM_BUTTON_End ();
   Frm_EndForm ();
  }

/*****************************************************************************/
/*********************** Get and write a note content ************************/
/*****************************************************************************/

static void TL_Not_WriteContent (const struct TL_Not_Note *Not)
  {
   if (Not->NoteType == TL_NOTE_POST)	// It's a post
      TL_Pst_GetAndWritePost (Not->Cod);
   else					// Not a post
      TL_Not_GetAndWriteNoPost (Not);
  }

/*****************************************************************************/
/***************** Get and write a note which is not a post ******************/
/*****************************************************************************/

static void TL_Not_GetAndWriteNoPost (const struct TL_Not_Note *Not)
  {
   struct Hie_Hierarchy Hie;
   struct For_Forums Forums;
   char ForumName[For_MAX_BYTES_FORUM_NAME + 1];
   char SummaryStr[Ntf_MAX_BYTES_SUMMARY + 1];

   /***** Reset forums *****/
   For_ResetForums (&Forums);

   /***** Get location in hierarchy *****/
   if (!Not->Unavailable)
      TL_Not_GetLocationInHierarchy (Not,&Hie,&Forums.Forum,ForumName);

   /***** Write note type *****/
   TL_Not_PutFormGoToAction (Not,&Forums);

   /***** Write location in hierarchy *****/
   if (!Not->Unavailable)
      TL_Not_WriteLocationInHierarchy (Not,&Hie,ForumName);

   /***** Get and write note summary *****/
   TL_Not_GetNoteSummary (Not,SummaryStr);
   HTM_DIV_Begin ("class=\"TL_TXT\"");
   HTM_Txt (SummaryStr);
   HTM_DIV_End ();
  }

/*****************************************************************************/
/************************ Get location in hierarchy **************************/
/*****************************************************************************/

static void TL_Not_GetLocationInHierarchy (const struct TL_Not_Note *Not,
                                           struct Hie_Hierarchy *Hie,
                                           struct For_Forum *Forum,
                                           char ForumName[For_MAX_BYTES_FORUM_NAME + 1])
  {
   /***** Initialize location in hierarchy *****/
   Hie->Cty.CtyCod =
   Hie->Ins.InsCod =
   Hie->Ctr.CtrCod =
   Hie->Deg.DegCod =
   Hie->Crs.CrsCod = -1L;

   /***** Get location in hierarchy *****/
   switch (Not->NoteType)
     {
      case TL_NOTE_INS_DOC_PUB_FILE:
      case TL_NOTE_INS_SHA_PUB_FILE:
	 /* Get institution data */
	 Hie->Ins.InsCod = Not->HieCod;
	 Ins_GetDataOfInstitutionByCod (&Hie->Ins);
	 break;
      case TL_NOTE_CTR_DOC_PUB_FILE:
      case TL_NOTE_CTR_SHA_PUB_FILE:
	 /* Get centre data */
	 Hie->Ctr.CtrCod = Not->HieCod;
	 Ctr_GetDataOfCentreByCod (&Hie->Ctr);
	 break;
      case TL_NOTE_DEG_DOC_PUB_FILE:
      case TL_NOTE_DEG_SHA_PUB_FILE:
	 /* Get degree data */
	 Hie->Deg.DegCod = Not->HieCod;
	 Deg_GetDataOfDegreeByCod (&Hie->Deg);
	 break;
      case TL_NOTE_CRS_DOC_PUB_FILE:
      case TL_NOTE_CRS_SHA_PUB_FILE:
      case TL_NOTE_EXAM_ANNOUNCEMENT:
      case TL_NOTE_NOTICE:
	 /* Get course data */
	 Hie->Crs.CrsCod = Not->HieCod;
	 Crs_GetDataOfCourseByCod (&Hie->Crs);
	 break;
      case TL_NOTE_FORUM_POST:
	 /* Get forum type of the post */
	 For_GetForumTypeAndLocationOfAPost (Not->Cod,Forum);
	 For_SetForumName (Forum,ForumName,Gbl.Prefs.Language,false);	// Set forum name in recipient's language
	 break;
      default:
	 break;
     }
  }

/*****************************************************************************/
/*********************** Write location in hierarchy *************************/
/*****************************************************************************/

static void TL_Not_WriteLocationInHierarchy (const struct TL_Not_Note *Not,
	                                     const struct Hie_Hierarchy *Hie,
                                             const char ForumName[For_MAX_BYTES_FORUM_NAME + 1])
  {
   extern const char *Txt_Forum;
   extern const char *Txt_Course;
   extern const char *Txt_Degree;
   extern const char *Txt_Centre;
   extern const char *Txt_Institution;

   switch (Not->NoteType)
     {
      case TL_NOTE_INS_DOC_PUB_FILE:
      case TL_NOTE_INS_SHA_PUB_FILE:
	 /* Write location (institution) in hierarchy */
	 HTM_DIV_Begin ("class=\"TL_LOC\"");
	 HTM_TxtF ("%s:&nbsp;%s",Txt_Institution,Hie->Ins.ShrtName);
	 HTM_DIV_End ();
	 break;
      case TL_NOTE_CTR_DOC_PUB_FILE:
      case TL_NOTE_CTR_SHA_PUB_FILE:
	 /* Write location (centre) in hierarchy */
	 HTM_DIV_Begin ("class=\"TL_LOC\"");
	 HTM_TxtF ("%s:&nbsp;%s",Txt_Centre,Hie->Ctr.ShrtName);
	 HTM_DIV_End ();
	 break;
      case TL_NOTE_DEG_DOC_PUB_FILE:
      case TL_NOTE_DEG_SHA_PUB_FILE:
	 /* Write location (degree) in hierarchy */
	 HTM_DIV_Begin ("class=\"TL_LOC\"");
	 HTM_TxtF ("%s:&nbsp;%s",Txt_Degree,Hie->Deg.ShrtName);
	 HTM_DIV_End ();
	 break;
      case TL_NOTE_CRS_DOC_PUB_FILE:
      case TL_NOTE_CRS_SHA_PUB_FILE:
      case TL_NOTE_EXAM_ANNOUNCEMENT:
      case TL_NOTE_NOTICE:
	 /* Write location (course) in hierarchy */
	 HTM_DIV_Begin ("class=\"TL_LOC\"");
	 HTM_TxtF ("%s:&nbsp;%s",Txt_Course,Hie->Crs.ShrtName);
	 HTM_DIV_End ();
	 break;
      case TL_NOTE_FORUM_POST:
	 /* Write forum name */
	 HTM_DIV_Begin ("class=\"TL_LOC\"");
	 HTM_TxtF ("%s:&nbsp;%s",Txt_Forum,ForumName);
	 HTM_DIV_End ();
	 break;
      default:
	 break;
     }
  }

/*****************************************************************************/
/************* Put form to go to an action depending on the note *************/
/*****************************************************************************/

static void TL_Not_PutFormGoToAction (const struct TL_Not_Note *Not,
                                      const struct For_Forums *Forums)
  {
   extern const Act_Action_t For_ActionsSeeFor[For_NUM_TYPES_FORUM];
   extern const char *The_ClassFormInBoxBold[The_NUM_THEMES];
   extern const char *Txt_TIMELINE_NOTE[TL_NOT_NUM_NOTE_TYPES];
   extern const char *Txt_not_available;
   char *Anchor = NULL;
   static const Act_Action_t TL_DefaultActions[TL_NOT_NUM_NOTE_TYPES] =
     {
      [TL_NOTE_UNKNOWN          ] = ActUnk,
      /* Start tab */
      [TL_NOTE_POST             ] = ActUnk,	// action not used
      /* Institution tab */
      [TL_NOTE_INS_DOC_PUB_FILE ] = ActReqDatSeeDocIns,
      [TL_NOTE_INS_SHA_PUB_FILE ] = ActReqDatShaIns,
      /* Centre tab */
      [TL_NOTE_CTR_DOC_PUB_FILE ] = ActReqDatSeeDocCtr,
      [TL_NOTE_CTR_SHA_PUB_FILE ] = ActReqDatShaCtr,
      /* Degree tab */
      [TL_NOTE_DEG_DOC_PUB_FILE ] = ActReqDatSeeDocDeg,
      [TL_NOTE_DEG_SHA_PUB_FILE ] = ActReqDatShaDeg,
      /* Course tab */
      [TL_NOTE_CRS_DOC_PUB_FILE ] = ActReqDatSeeDocCrs,
      [TL_NOTE_CRS_SHA_PUB_FILE ] = ActReqDatShaCrs,
      /* Assessment tab */
      [TL_NOTE_EXAM_ANNOUNCEMENT] = ActSeeOneExaAnn,
      /* Users tab */
      /* Messages tab */
      [TL_NOTE_NOTICE           ] = ActSeeOneNot,
      [TL_NOTE_FORUM_POST       ] = ActSeeFor,
      /* Analytics tab */
      /* Profile tab */
     };
   static const char *TL_Icons[TL_NOT_NUM_NOTE_TYPES] =
     {
      [TL_NOTE_UNKNOWN          ] = NULL,
      /* Start tab */
      [TL_NOTE_POST             ] = NULL,	// icon not used
      /* Institution tab */
      [TL_NOTE_INS_DOC_PUB_FILE ] = "file.svg",
      [TL_NOTE_INS_SHA_PUB_FILE ] = "file.svg",
      /* Centre tab */
      [TL_NOTE_CTR_DOC_PUB_FILE ] = "file.svg",
      [TL_NOTE_CTR_SHA_PUB_FILE ] = "file.svg",
      /* Degree tab */
      [TL_NOTE_DEG_DOC_PUB_FILE ] = "file.svg",
      [TL_NOTE_DEG_SHA_PUB_FILE ] = "file.svg",
      /* Course tab */
      [TL_NOTE_CRS_DOC_PUB_FILE ] = "file.svg",
      [TL_NOTE_CRS_SHA_PUB_FILE ] = "file.svg",
      /* Assessment tab */
      [TL_NOTE_EXAM_ANNOUNCEMENT] = "bullhorn.svg",
      /* Users tab */
      /* Messages tab */
      [TL_NOTE_NOTICE           ] = "sticky-note.svg",
      [TL_NOTE_FORUM_POST       ] = "comments.svg",
      /* Analytics tab */
      /* Profile tab */
     };

   if (Not->Unavailable ||	// File/notice... pointed by this note is unavailable
       Gbl.Form.Inside)		// Inside another form
     {
      /***** Do not put form *****/
      HTM_DIV_Begin ("class=\"TL_FORM_OFF\"");
      HTM_Txt (Txt_TIMELINE_NOTE[Not->NoteType]);
      if (Not->Unavailable)
         HTM_TxtF ("&nbsp;(%s)",Txt_not_available);
      HTM_DIV_End ();
     }
   else			// Not inside another form
     {
      HTM_DIV_Begin ("class=\"TL_FORM\"");

      /***** Begin form with parameters depending on the type of note *****/
      switch (Not->NoteType)
	{
	 case TL_NOTE_INS_DOC_PUB_FILE:
	 case TL_NOTE_INS_SHA_PUB_FILE:
	    Frm_StartFormUnique (TL_DefaultActions[Not->NoteType]);
	    Brw_PutHiddenParamFilCod (Not->Cod);
	    if (Not->HieCod != Gbl.Hierarchy.Ins.InsCod)	// Not the current institution
	       Ins_PutParamInsCod (Not->HieCod);		// Go to another institution
	    break;
	 case TL_NOTE_CTR_DOC_PUB_FILE:
	 case TL_NOTE_CTR_SHA_PUB_FILE:
	    Frm_StartFormUnique (TL_DefaultActions[Not->NoteType]);
	    Brw_PutHiddenParamFilCod (Not->Cod);
	    if (Not->HieCod != Gbl.Hierarchy.Ctr.CtrCod)	// Not the current centre
	       Ctr_PutParamCtrCod (Not->HieCod);		// Go to another centre
	    break;
	 case TL_NOTE_DEG_DOC_PUB_FILE:
	 case TL_NOTE_DEG_SHA_PUB_FILE:
	    Frm_StartFormUnique (TL_DefaultActions[Not->NoteType]);
	    Brw_PutHiddenParamFilCod (Not->Cod);
	    if (Not->HieCod != Gbl.Hierarchy.Deg.DegCod)	// Not the current degree
	       Deg_PutParamDegCod (Not->HieCod);		// Go to another degree
	    break;
	 case TL_NOTE_CRS_DOC_PUB_FILE:
	 case TL_NOTE_CRS_SHA_PUB_FILE:
	    Frm_StartFormUnique (TL_DefaultActions[Not->NoteType]);
	    Brw_PutHiddenParamFilCod (Not->Cod);
	    if (Not->HieCod != Gbl.Hierarchy.Crs.CrsCod)	// Not the current course
	       Crs_PutParamCrsCod (Not->HieCod);		// Go to another course
	    break;
	 case TL_NOTE_EXAM_ANNOUNCEMENT:
            Frm_SetAnchorStr (Not->Cod,&Anchor);
	    Frm_StartFormUniqueAnchor (TL_DefaultActions[Not->NoteType],
		                       Anchor);	// Locate on this specific exam
            Frm_FreeAnchorStr (Anchor);
	    ExaAnn_PutHiddenParamExaCod (Not->Cod);
	    if (Not->HieCod != Gbl.Hierarchy.Crs.CrsCod)	// Not the current course
	       Crs_PutParamCrsCod (Not->HieCod);		// Go to another course
	    break;
	 case TL_NOTE_POST:	// Not applicable
	    return;
	 case TL_NOTE_FORUM_POST:
	    Frm_StartFormUnique (For_ActionsSeeFor[Forums->Forum.Type]);
	    For_PutAllHiddenParamsForum (1,	// Page of threads = first
                                         1,	// Page of posts   = first
                                         Forums->ForumSet,
					 Forums->ThreadsOrder,
					 Forums->Forum.Location,
					 Forums->Thread.Selected,
					 -1L);
	    if (Not->HieCod != Gbl.Hierarchy.Crs.CrsCod)	// Not the current course
	       Crs_PutParamCrsCod (Not->HieCod);		// Go to another course
	    break;
	 case TL_NOTE_NOTICE:
            Frm_SetAnchorStr (Not->Cod,&Anchor);
	    Frm_StartFormUniqueAnchor (TL_DefaultActions[Not->NoteType],
				       Anchor);
            Frm_FreeAnchorStr (Anchor);
	    Not_PutHiddenParamNotCod (Not->Cod);
	    if (Not->HieCod != Gbl.Hierarchy.Crs.CrsCod)	// Not the current course
	       Crs_PutParamCrsCod (Not->HieCod);		// Go to another course
	    break;
	 default:			// Not applicable
	    return;
	}

      /***** Icon and link to go to action *****/
      HTM_BUTTON_SUBMIT_Begin (Txt_TIMELINE_NOTE[Not->NoteType],
			       Str_BuildStringStr ("BT_LINK %s ICO_HIGHLIGHT",
						   The_ClassFormInBoxBold[Gbl.Prefs.Theme]),
			       NULL);
      Ico_PutIcon (TL_Icons[Not->NoteType],Txt_TIMELINE_NOTE[Not->NoteType],"CONTEXT_ICO_x16");
      HTM_TxtF ("&nbsp;%s",Txt_TIMELINE_NOTE[Not->NoteType]);
      HTM_BUTTON_End ();
      Str_FreeString ();

      /***** End form *****/
      Frm_EndForm ();

      HTM_DIV_End ();
     }
  }

/*****************************************************************************/
/********************** Get note summary and content *************************/
/*****************************************************************************/

void TL_Not_GetNoteSummary (const struct TL_Not_Note *Not,
                            char SummaryStr[Ntf_MAX_BYTES_SUMMARY + 1])
  {
   SummaryStr[0] = '\0';

   switch (Not->NoteType)
     {
      case TL_NOTE_UNKNOWN:
          break;
      case TL_NOTE_INS_DOC_PUB_FILE:
      case TL_NOTE_INS_SHA_PUB_FILE:
      case TL_NOTE_CTR_DOC_PUB_FILE:
      case TL_NOTE_CTR_SHA_PUB_FILE:
      case TL_NOTE_DEG_DOC_PUB_FILE:
      case TL_NOTE_DEG_SHA_PUB_FILE:
      case TL_NOTE_CRS_DOC_PUB_FILE:
      case TL_NOTE_CRS_SHA_PUB_FILE:
	 Brw_GetSummaryAndContentOfFile (SummaryStr,NULL,Not->Cod,false);
         break;
      case TL_NOTE_EXAM_ANNOUNCEMENT:
         ExaAnn_GetSummaryAndContentExamAnn (SummaryStr,NULL,Not->Cod,false);
         break;
      case TL_NOTE_POST:
	 // Not applicable
         break;
      case TL_NOTE_FORUM_POST:
         For_GetSummaryAndContentForumPst (SummaryStr,NULL,Not->Cod,false);
         break;
      case TL_NOTE_NOTICE:
         Not_GetSummaryAndContentNotice (SummaryStr,NULL,Not->Cod,false);
         break;
     }
  }

/*****************************************************************************/
/************************ Write bottom part of a note ************************/
/*****************************************************************************/

static void TL_Not_WriteButtonsAndComments (const struct TL_Timeline *Timeline,
                                const struct TL_Not_Note *Not,
                                const struct UsrData *UsrDat)	// Author
  {
   char IdNewComment[Frm_MAX_BYTES_ID + 1];

   /***** Create unique id for new comment *****/
   Frm_SetUniqueId (IdNewComment);

   /***** Left: button to add a comment *****/
   TL_Not_WriteButtonToAddAComment (Not,IdNewComment);

   /***** Right: write favs, shared and remove buttons, and comments *****/
   TL_Not_WriteFavShaRemAndComments (Timeline,Not,UsrDat);

   /***** Put hidden form to write a new comment *****/
   TL_Com_PutHiddenFormToWriteNewComment (Timeline,Not->NotCod,IdNewComment);
  }

/*****************************************************************************/
/********************** Write button to add a comment ************************/
/*****************************************************************************/

static void TL_Not_WriteButtonToAddAComment (const struct TL_Not_Note *Not,
                                             const char IdNewComment[Frm_MAX_BYTES_ID + 1])
  {
   HTM_DIV_Begin ("class=\"TL_BOTTOM_LEFT\"");
   if (Not->Unavailable)	// Unavailable notes can not be commented
      TL_Com_PutIconCommentDisabled ();
   else
      TL_Com_PutIconToToggleComment (IdNewComment);
   HTM_DIV_End ();
  }

/*****************************************************************************/
/******* Write favs, shared and remove buttons, and comments of a note *******/
/*****************************************************************************/

static void TL_Not_WriteFavShaRemAndComments (const struct TL_Timeline *Timeline,
					      const struct TL_Not_Note *Not,
					      const struct UsrData *UsrDat)	// Author
  {
   /***** Begin container *****/
   HTM_DIV_Begin ("class=\"TL_BOTTOM_RIGHT TL_RIGHT_WIDTH\"");

   /***** Write favs, shared and remove buttons int the foot of a note *****/
   TL_Not_WriteFavShaRem (Timeline,Not,UsrDat);

   /***** Comments *****/
   TL_Com_WriteCommentsInNote (Timeline,Not);

   /***** End container *****/
   HTM_DIV_End ();
  }

/*****************************************************************************/
/******* Write favs, shared and remove buttons in the foot of a note *********/
/*****************************************************************************/

static void TL_Not_WriteFavShaRem (const struct TL_Timeline *Timeline,
                                   const struct TL_Not_Note *Not,
                                   const struct UsrData *UsrDat)	// Author
  {
   static unsigned NumDiv = 0;	// Used to create unique div id for fav and shared

   NumDiv++;

   /***** Begin foot container *****/
   HTM_DIV_Begin ("class=\"TL_FOOT TL_RIGHT_WIDTH\"");

   /***** Foot column 1: fav zone *****/
   HTM_DIV_Begin ("id=\"fav_not_%s_%u\" class=\"TL_FAV_NOT TL_FAV_NOT_WIDTH\"",
		  Gbl.UniqueNameEncrypted,NumDiv);
   TL_Fav_PutIconToFavUnfNote (Not,TL_Usr_SHOW_FEW_USRS);
   HTM_DIV_End ();

   /***** Foot column 2: share zone *****/
   HTM_DIV_Begin ("id=\"sha_not_%s_%u\" class=\"TL_SHA_NOT TL_SHA_NOT_WIDTH\"",
		  Gbl.UniqueNameEncrypted,NumDiv);
   TL_Sha_PutIconToShaUnsNote (Not,TL_Usr_SHOW_FEW_USRS);
   HTM_DIV_End ();

   /***** Foot column 3: icon to remove this note *****/
   HTM_DIV_Begin ("class=\"TL_REM\"");
   if (Usr_ItsMe (UsrDat->UsrCod))	// I am the author
      TL_Not_PutFormToRemoveNote (Timeline,Not->NotCod);
   HTM_DIV_End ();

   /***** End foot container *****/
   HTM_DIV_End ();
  }

/*****************************************************************************/
/**************************** Form to remove note ****************************/
/*****************************************************************************/

static void TL_Not_PutFormToRemoveNote (const struct TL_Timeline *Timeline,
                                        long NotCod)
  {
   extern const char *Txt_Remove;

   /***** Form to remove publication *****/
   TL_Frm_FormStart (Timeline,TL_Frm_REQ_REM_NOTE);
   TL_Not_PutHiddenParamNotCod (NotCod);
   Ico_PutIconLink ("trash.svg",Txt_Remove);
   Frm_EndForm ();
  }

/*****************************************************************************/
/***************** Store and publish a note into database ********************/
/*****************************************************************************/

void TL_Not_StoreAndPublishNote (TL_Not_NoteType_t NoteType,long Cod)
  {
   struct TL_Pub_Publication Pub;

   TL_Not_StoreAndPublishNoteInternal (NoteType,Cod,&Pub);
  }

void TL_Not_StoreAndPublishNoteInternal (TL_Not_NoteType_t NoteType,long Cod,struct TL_Pub_Publication *Pub)
  {
   long HieCod;	// Hierarchy code (institution/centre/degree/course)

   switch (NoteType)
     {
      case TL_NOTE_INS_DOC_PUB_FILE:
      case TL_NOTE_INS_SHA_PUB_FILE:
	 HieCod = Gbl.Hierarchy.Ins.InsCod;
	 break;
      case TL_NOTE_CTR_DOC_PUB_FILE:
      case TL_NOTE_CTR_SHA_PUB_FILE:
	 HieCod = Gbl.Hierarchy.Ctr.CtrCod;
	 break;
      case TL_NOTE_DEG_DOC_PUB_FILE:
      case TL_NOTE_DEG_SHA_PUB_FILE:
	 HieCod = Gbl.Hierarchy.Deg.DegCod;
	 break;
      case TL_NOTE_CRS_DOC_PUB_FILE:
      case TL_NOTE_CRS_SHA_PUB_FILE:
      case TL_NOTE_EXAM_ANNOUNCEMENT:
      case TL_NOTE_NOTICE:
	 HieCod = Gbl.Hierarchy.Crs.CrsCod;
	 break;
      default:
	 HieCod = -1L;
         break;
     }

   /***** Store note *****/
   Pub->NotCod =
   DB_QueryINSERTandReturnCode ("can not create new note",
				"INSERT INTO tl_notes"
				" (NoteType,Cod,UsrCod,HieCod,Unavailable,TimeNote)"
				" VALUES"
				" (%u,%ld,%ld,%ld,'N',NOW())",
				(unsigned) NoteType,
				Cod,Gbl.Usrs.Me.UsrDat.UsrCod,HieCod);

   /***** Publish note in timeline *****/
   Pub->PublisherCod = Gbl.Usrs.Me.UsrDat.UsrCod;
   Pub->PubType      = TL_Pub_ORIGINAL_NOTE;
   TL_Pub_PublishPubInTimeline (Pub);
  }

/*****************************************************************************/
/************************* Mark a note as unavailable ************************/
/*****************************************************************************/

void TL_Not_MarkNoteAsUnavailable (TL_Not_NoteType_t NoteType,long Cod)
  {
   /***** Mark the note as unavailable *****/
   DB_QueryUPDATE ("can not mark note as unavailable",
		   "UPDATE tl_notes SET Unavailable='Y'"
		   " WHERE NoteType=%u AND Cod=%ld",
		   (unsigned) NoteType,Cod);
  }

/*****************************************************************************/
/****************** Mark notes of one file as unavailable ********************/
/*****************************************************************************/

void TL_Not_MarkNoteOneFileAsUnavailable (const char *Path)
  {
   extern const Brw_FileBrowser_t Brw_FileBrowserForDB_files[Brw_NUM_TYPES_FILE_BROWSER];
   Brw_FileBrowser_t FileBrowser = Brw_FileBrowserForDB_files[Gbl.FileBrowser.Type];
   long FilCod;
   TL_Not_NoteType_t NoteType;

   switch (FileBrowser)
     {
      case Brw_ADMI_DOC_INS:
      case Brw_ADMI_SHR_INS:
      case Brw_ADMI_DOC_CTR:
      case Brw_ADMI_SHR_CTR:
      case Brw_ADMI_DOC_DEG:
      case Brw_ADMI_SHR_DEG:
      case Brw_ADMI_DOC_CRS:
      case Brw_ADMI_SHR_CRS:
         /***** Get file code *****/
	 FilCod = Brw_GetFilCodByPath (Path,true);	// Only if file is public
	 if (FilCod > 0)
	   {
	    /***** Mark possible note as unavailable *****/
	    switch (FileBrowser)
	      {
	       case Brw_ADMI_DOC_INS:
		  NoteType = TL_NOTE_INS_DOC_PUB_FILE;
		  break;
	       case Brw_ADMI_SHR_INS:
		  NoteType = TL_NOTE_INS_SHA_PUB_FILE;
		  break;
	       case Brw_ADMI_DOC_CTR:
		  NoteType = TL_NOTE_CTR_DOC_PUB_FILE;
		  break;
	       case Brw_ADMI_SHR_CTR:
		  NoteType = TL_NOTE_CTR_SHA_PUB_FILE;
		  break;
	       case Brw_ADMI_DOC_DEG:
		  NoteType = TL_NOTE_DEG_DOC_PUB_FILE;
		  break;
	       case Brw_ADMI_SHR_DEG:
		  NoteType = TL_NOTE_DEG_SHA_PUB_FILE;
		  break;
	       case Brw_ADMI_DOC_CRS:
		  NoteType = TL_NOTE_CRS_DOC_PUB_FILE;
		  break;
	       case Brw_ADMI_SHR_CRS:
		  NoteType = TL_NOTE_CRS_SHA_PUB_FILE;
		  break;
	       default:
		  return;
	      }
	    TL_Not_MarkNoteAsUnavailable (NoteType,FilCod);
	   }
         break;
      default:
	 break;
     }
  }

/*****************************************************************************/
/***** Mark possible notes involving children of a folder as unavailable *****/
/*****************************************************************************/

void TL_Not_MarkNotesChildrenOfFolderAsUnavailable (const char *Path)
  {
   extern const Brw_FileBrowser_t Brw_FileBrowserForDB_files[Brw_NUM_TYPES_FILE_BROWSER];
   Brw_FileBrowser_t FileBrowser = Brw_FileBrowserForDB_files[Gbl.FileBrowser.Type];
   long Cod = Brw_GetCodForFiles ();
   TL_Not_NoteType_t NoteType;

   switch (FileBrowser)
     {
      case Brw_ADMI_DOC_INS:
      case Brw_ADMI_SHR_INS:
      case Brw_ADMI_DOC_CTR:
      case Brw_ADMI_SHR_CTR:
      case Brw_ADMI_DOC_DEG:
      case Brw_ADMI_SHR_DEG:
      case Brw_ADMI_DOC_CRS:
      case Brw_ADMI_SHR_CRS:
	 /***** Mark possible note as unavailable *****/
	 switch (FileBrowser)
	   {
	    case Brw_ADMI_DOC_INS:
	       NoteType = TL_NOTE_INS_DOC_PUB_FILE;
	       break;
	    case Brw_ADMI_SHR_INS:
	       NoteType = TL_NOTE_INS_SHA_PUB_FILE;
	       break;
	    case Brw_ADMI_DOC_CTR:
	       NoteType = TL_NOTE_CTR_DOC_PUB_FILE;
	       break;
	    case Brw_ADMI_SHR_CTR:
	       NoteType = TL_NOTE_CTR_SHA_PUB_FILE;
	       break;
	    case Brw_ADMI_DOC_DEG:
	       NoteType = TL_NOTE_DEG_DOC_PUB_FILE;
	       break;
	    case Brw_ADMI_SHR_DEG:
	       NoteType = TL_NOTE_DEG_SHA_PUB_FILE;
	       break;
	    case Brw_ADMI_DOC_CRS:
	       NoteType = TL_NOTE_CRS_DOC_PUB_FILE;
	       break;
	    case Brw_ADMI_SHR_CRS:
	       NoteType = TL_NOTE_CRS_SHA_PUB_FILE;
	       break;
	    default:
	       return;
	   }
         DB_QueryUPDATE ("can not mark notes as unavailable",
			 "UPDATE tl_notes SET Unavailable='Y'"
		         " WHERE NoteType=%u AND Cod IN"
	                 " (SELECT FilCod FROM files"
			 " WHERE FileBrowser=%u AND Cod=%ld"
			 " AND Path LIKE '%s/%%' AND Public='Y')",	// Only public files
			 (unsigned) NoteType,
			 (unsigned) FileBrowser,Cod,
			 Path);
         break;
      default:
	 break;
     }
  }

/*****************************************************************************/
/****************** Put parameter with the code of a note ********************/
/*****************************************************************************/

void TL_Not_PutHiddenParamNotCod (long NotCod)
  {
   Par_PutHiddenParamLong (NULL,"NotCod",NotCod);
  }

/*****************************************************************************/
/****************** Get parameter with the code of a note ********************/
/*****************************************************************************/

long TL_Not_GetParamNotCod (void)
  {
   /***** Get note code *****/
   return Par_GetParToLong ("NotCod");
  }

/*****************************************************************************/
/*********************** Request the removal of a note ***********************/
/*****************************************************************************/

void TL_Not_RequestRemNoteUsr (void)
  {
   struct TL_Timeline Timeline;

   /***** Reset timeline context *****/
   TL_ResetTimeline (&Timeline);

   /***** Get user whom profile is displayed *****/
   Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ();

   /***** Show user's profile *****/
   Prf_ShowUserProfile (&Gbl.Usrs.Other.UsrDat);

   /***** Start section *****/
   HTM_SECTION_Begin (TL_TIMELINE_SECTION_ID);

   /***** Request the removal of note *****/
   TL_Not_RequestRemovalNote (&Timeline);

   /***** Write timeline again (user) *****/
   TL_ShowTimelineUsr (&Timeline);

   /***** End section *****/
   HTM_SECTION_End ();
  }

void TL_Not_RequestRemNoteGbl (void)
  {
   struct TL_Timeline Timeline;

   /***** Initialize timeline *****/
   TL_InitTimelineGbl (&Timeline);

   /***** Request the removal of note *****/
   TL_Not_RequestRemovalNote (&Timeline);

   /***** Write timeline again (global) *****/
   TL_ShowNoteAndTimelineGbl (&Timeline);
  }

static void TL_Not_RequestRemovalNote (struct TL_Timeline *Timeline)
  {
   extern const char *Txt_The_original_post_no_longer_exists;
   extern const char *Txt_Do_you_really_want_to_remove_the_following_post;
   struct TL_Not_Note Not;

   /***** Get data of note *****/
   Not.NotCod = TL_Not_GetParamNotCod ();
   TL_Not_GetDataOfNoteByCod (&Not);

   if (Not.NotCod > 0)
     {
      if (Usr_ItsMe (Not.UsrCod))	// I am the author of this note
	{
	 /***** Show question and button to remove note *****/
	 /* Begin alert */
	 TL_Frm_BeginAlertRemove (Txt_Do_you_really_want_to_remove_the_following_post);

	 /* Show note */
	 Box_BoxBegin (NULL,NULL,
		       NULL,NULL,
		       NULL,Box_CLOSABLE);
	 HTM_DIV_Begin ("class=\"TL_WIDTH\"");
	 TL_Not_CheckAndWriteNoteWithTopMsg (Timeline,&Not,
		                             TL_TOP_MESSAGE_NONE,
		                             -1L);
         HTM_DIV_End ();
	 Box_BoxEnd ();

	 /* End alert */
         Timeline->NotCod = Not.NotCod;	// Note to be removed
	 TL_Frm_EndAlertRemove (Timeline,TL_Frm_REM_NOTE,
	                        TL_Not_PutParamsRemoveNote);
	}
     }
   else
      Ale_ShowAlert (Ale_WARNING,Txt_The_original_post_no_longer_exists);
  }

/*****************************************************************************/
/********************* Put parameters to remove a note ***********************/
/*****************************************************************************/

static void TL_Not_PutParamsRemoveNote (void *Timeline)
  {
   if (Timeline)
     {
      if (Gbl.Usrs.Other.UsrDat.UsrCod > 0)	// User's timeline
	 Usr_PutParamOtherUsrCodEncrypted (Gbl.Usrs.Other.UsrDat.EnUsrCod);
      else					// Global timeline
	 Usr_PutHiddenParamWho (((struct TL_Timeline *) Timeline)->Who);
      TL_Not_PutHiddenParamNotCod (((struct TL_Timeline *) Timeline)->NotCod);
     }
  }

/*****************************************************************************/
/******************************* Remove a note *******************************/
/*****************************************************************************/

void TL_Not_RemoveNoteUsr (void)
  {
   struct TL_Timeline Timeline;

   /***** Reset timeline context *****/
   TL_ResetTimeline (&Timeline);

   /***** Get user whom profile is displayed *****/
   Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ();

   /***** Show user's profile *****/
   Prf_ShowUserProfile (&Gbl.Usrs.Other.UsrDat);

   /***** Start section *****/
   HTM_SECTION_Begin (TL_TIMELINE_SECTION_ID);

   /***** Remove a note *****/
   TL_Not_RemoveNote ();

   /***** Write updated timeline after removing (user) *****/
   TL_ShowTimelineUsr (&Timeline);

   /***** End section *****/
   HTM_SECTION_End ();
  }

void TL_Not_RemoveNoteGbl (void)
  {
   struct TL_Timeline Timeline;

   /***** Initialize timeline *****/
   TL_InitTimelineGbl (&Timeline);

   /***** Remove a note *****/
   TL_Not_RemoveNote ();

   /***** Write updated timeline after removing (global) *****/
   TL_ShowNoteAndTimelineGbl (&Timeline);
  }

static void TL_Not_RemoveNote (void)
  {
   extern const char *Txt_The_original_post_no_longer_exists;
   extern const char *Txt_TIMELINE_Post_removed;
   struct TL_Not_Note Not;

   /***** Get data of note *****/
   Not.NotCod = TL_Not_GetParamNotCod ();
   TL_Not_GetDataOfNoteByCod (&Not);

   if (Not.NotCod > 0)
     {
      if (Usr_ItsMe (Not.UsrCod))	// I am the author of this note
	{
	 /***** Delete note from database *****/
	 TL_Not_RemoveNoteMediaAndDBEntries (&Not);

	 /***** Reset note *****/
	 TL_Not_ResetNote (&Not);

	 /***** Message of success *****/
	 Ale_ShowAlert (Ale_SUCCESS,Txt_TIMELINE_Post_removed);
	}
     }
   else
      Ale_ShowAlert (Ale_WARNING,Txt_The_original_post_no_longer_exists);
  }

/*****************************************************************************/
/*********************** Remove a note from database *************************/
/*****************************************************************************/

static void TL_Not_RemoveNoteMediaAndDBEntries (struct TL_Not_Note *Not)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   long PubCod;
   unsigned long NumComments;
   unsigned long NumCom;
   long MedCod;

   /***** Remove comments associated to this note *****/
   /* Get comments of this note */
   NumComments = DB_QuerySELECT (&mysql_res,"can not get comments",
				 "SELECT PubCod"
				 " FROM tl_pubs"
				 " WHERE NotCod=%ld AND PubType=%u",
				 Not->NotCod,
				 (unsigned) TL_Pub_COMMENT_TO_NOTE);

   /* For each comment... */
   for (NumCom = 0;
	NumCom < NumComments;
	NumCom++)
     {
      /* Get code of comment **/
      row = mysql_fetch_row (mysql_res);
      PubCod = Str_ConvertStrCodToLongCod (row[0]);

      /* Remove media associated to comment
	 and delete comment from database */
      TL_Com_RemoveCommentMediaAndDBEntries (PubCod);
     }

   /* Free structure that stores the query result */
   DB_FreeMySQLResult (&mysql_res);

   /***** Remove media associated to post *****/
   if (Not->NoteType == TL_NOTE_POST)
     {
      /* Remove media associated to a post from database */
      if (DB_QuerySELECT (&mysql_res,"can not get media",
			  "SELECT MedCod"	// row[0]
			  " FROM tl_posts"
			  " WHERE PstCod=%ld",
			  Not->Cod) == 1)	// Result should have a unique row
        {
	 /* Get media code */
	 row = mysql_fetch_row (mysql_res);
	 MedCod = Str_ConvertStrCodToLongCod (row[0]);

	 /* Remove media */
	 Med_RemoveMedia (MedCod);
        }

      /* Free structure that stores the query result */
      DB_FreeMySQLResult (&mysql_res);
     }

   /***** Mark possible notifications on the publications
          of this note as removed *****/
   /* Mark notifications of the original note as removed */
   PubCod = TL_Not_GetPubCodOfOriginalNote (Not->NotCod);
   if (PubCod > 0)
     {
      Ntf_MarkNotifAsRemoved (Ntf_EVENT_TIMELINE_FAV    ,PubCod);
      Ntf_MarkNotifAsRemoved (Ntf_EVENT_TIMELINE_SHARE  ,PubCod);
      Ntf_MarkNotifAsRemoved (Ntf_EVENT_TIMELINE_MENTION,PubCod);
     }

   /***** Remove favs for this note *****/
   DB_QueryDELETE ("can not remove favs for note",
		   "DELETE FROM tl_notes_fav"
		   " WHERE NotCod=%ld",
		   Not->NotCod);

   /***** Remove all the publications of this note *****/
   DB_QueryDELETE ("can not remove a publication",
		   "DELETE FROM tl_pubs"
		   " WHERE NotCod=%ld",
		   Not->NotCod);

   /***** Remove note *****/
   DB_QueryDELETE ("can not remove a note",
		   "DELETE FROM tl_notes"
	           " WHERE NotCod=%ld"
	           " AND UsrCod=%ld",		// Extra check: I am the author
		   Not->NotCod,
		   Gbl.Usrs.Me.UsrDat.UsrCod);

   if (Not->NoteType == TL_NOTE_POST)
      /***** Remove post *****/
      DB_QueryDELETE ("can not remove a post",
		      "DELETE FROM tl_posts"
		      " WHERE PstCod=%ld",
		      Not->Cod);
  }

/*****************************************************************************/
/************************ Get data of note from row **************************/
/*****************************************************************************/

static void TL_Not_GetDataOfNoteFromRow (MYSQL_ROW row,struct TL_Not_Note *Not)
  {
   /*
   row[0]: NotCod
   row[1]: NoteType
   row[2]: Cod
   row[3]: UsrCod
   row[4]: HieCod
   row[5]: Unavailable
   row[5]: UNIX_TIMESTAMP(TimeNote)
   */
   /***** Get code (row[0]) *****/
   Not->NotCod      = Str_ConvertStrCodToLongCod (row[0]);

   /***** Get note type (row[1]) *****/
   Not->NoteType    = TL_Not_GetNoteTypeFromStr ((const char *) row[1]);

   /***** Get file/post... code (row[2]) *****/
   Not->Cod         = Str_ConvertStrCodToLongCod (row[2]);

   /***** Get (from) user code (row[3]) *****/
   Not->UsrCod      = Str_ConvertStrCodToLongCod (row[3]);

   /***** Get hierarchy code (row[4]) *****/
   Not->HieCod      = Str_ConvertStrCodToLongCod (row[4]);

   /***** File/post... unavailable (row[5]) *****/
   Not->Unavailable = (row[5][0] == 'Y');

   /***** Get time of the note (row[6]) *****/
   Not->DateTimeUTC = Dat_GetUNIXTimeFromStr (row[6]);

   /***** Get number of times this note has been shared *****/
   TL_Sha_UpdateNumTimesANoteHasBeenShared (Not);

   /***** Get number of times this note has been favourited *****/
   TL_Fav_GetNumTimesANoteHasBeenFav (Not);
  }

/*****************************************************************************/
/********* Get note type from string number coming from database *************/
/*****************************************************************************/

static TL_Not_NoteType_t TL_Not_GetNoteTypeFromStr (const char *Str)
  {
   unsigned UnsignedNum;

   if (sscanf (Str,"%u",&UnsignedNum) == 1)
      if (UnsignedNum < TL_NOT_NUM_NOTE_TYPES)
         return (TL_Not_NoteType_t) UnsignedNum;

   return TL_NOTE_UNKNOWN;
  }

/*****************************************************************************/
/*************************** Reset fields of note ****************************/
/*****************************************************************************/

static void TL_Not_ResetNote (struct TL_Not_Note *Not)
  {
   Not->NotCod      = -1L;
   Not->NoteType    = TL_NOTE_UNKNOWN;
   Not->UsrCod      = -1L;
   Not->HieCod      = -1L;
   Not->Cod         = -1L;
   Not->Unavailable = false;
   Not->DateTimeUTC = (time_t) 0;
   Not->NumShared   = 0;
  }

/*****************************************************************************/
/*************** Get code of publication of the original note ****************/
/*****************************************************************************/

long TL_Not_GetPubCodOfOriginalNote (long NotCod)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   long OriginalPubCod = -1L;

   /***** Get code of publication of the original note *****/
   if (DB_QuerySELECT (&mysql_res,"can not get code of publication",
		       "SELECT PubCod FROM tl_pubs"
		       " WHERE NotCod=%ld AND PubType=%u",
		       NotCod,(unsigned) TL_Pub_ORIGINAL_NOTE) == 1)   // Result should have a unique row
     {
      /* Get code of publication (row[0]) */
      row = mysql_fetch_row (mysql_res);
      OriginalPubCod = Str_ConvertStrCodToLongCod (row[0]);
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return OriginalPubCod;
  }

/*****************************************************************************/
/****** Add just retrieved notes to current timeline for this session ********/
/*****************************************************************************/

void TL_Not_AddNotesJustRetrievedToVisibleTimelineThisSession (void)
  {
   /* tl_timelines contains the distinct notes in timeline of each open session:
mysql> SELECT SessionId,COUNT(*) FROM tl_timelines GROUP BY SessionId;
+---------------------------------------------+----------+
| SessionId                                   | COUNT(*) |
+---------------------------------------------+----------+
| u-X-R3gKki7eKMXrNCP8bGhwOAZuVngRy7FNGZFMKzI |       52 | --> 52 distinct notes
| u1CoqL1YWl3_hR4wk4bI7vhnc-uRcCmIDyKYAgBB6kk |       10 |
| u8xqamzkorHfY4BvYRMXjNhzHvQyigZUZemO0YiMn48 |       10 |
| u_n2V_L3KrFjnd4SqZk0gxMFwZHRuWZ8_EIVTU9sdpI |       10 |
| V6pGe1kGGS_uO5i__waqXKnuDkPYaDZHNAYr-Zv-GJQ |        2 |
| vqDRz-iiM8v10Dl8ThwqIqmDRIklz8szJaqflwXZucs |       10 |
| w11juqKPx6lg-f_pL2ZBYqlagU1mEepSvvk9L3gDGac |       10 | --> 10 distinct notes
| wLg4e8KQljCcVuFWIkJjNeti89kAiwOZ3iyXdzm_eDk |       10 |
| wnU85YrwJHhZGWIZhd7LQfQTPrclIWHfMF3DcB-Rcgw |        4 |
| wRzRJFnHfzW61fZYnvMIaMRlkuWUeEyqXVQ6JeWA32k |       11 |
+---------------------------------------------+----------+
10 rows in set (0,01 sec)
   */
   DB_QueryINSERT ("can not insert notes in timeline",
		   "INSERT IGNORE INTO tl_timelines"
	           " (SessionId,NotCod)"
	           " SELECT '%s',NotCod FROM tl_tmp_just_retrieved_notes",
		   Gbl.Session.Id);
  }

/*****************************************************************************/
/******************** Get data of note using its code ************************/
/*****************************************************************************/

void TL_Not_GetDataOfNoteByCod (struct TL_Not_Note *Not)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;

   if (Not->NotCod > 0)
     {
      /***** Get data of note from database *****/
      if (DB_QuerySELECT (&mysql_res,"can not get data of note",
			  "SELECT NotCod,"			// row[0]
				 "NoteType,"			// row[1]
				 "Cod,"				// row[2]
				 "UsrCod,"			// row[3]
				 "HieCod,"			// row[4]
				 "Unavailable,"			// row[5]
				 "UNIX_TIMESTAMP(TimeNote)"	// row[6]
			  " FROM tl_notes"
			  " WHERE NotCod=%ld",
			  Not->NotCod))
	{
	 /***** Get data of note *****/
	 row = mysql_fetch_row (mysql_res);
	 TL_Not_GetDataOfNoteFromRow (row,Not);
	}
      else
	 /***** Reset fields of note *****/
	 TL_Not_ResetNote (Not);

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);
     }
   else
      /***** Reset fields of note *****/
      TL_Not_ResetNote (Not);
  }

/*****************************************************************************/
/******************* Clear unused old timelines in database ******************/
/*****************************************************************************/

void TL_Not_ClearOldTimelinesNotesFromDB (void)
  {
   /***** Remove timelines for expired sessions *****/
   DB_QueryDELETE ("can not remove old timelines",
		   "DELETE LOW_PRIORITY FROM tl_timelines"
                   " WHERE SessionId NOT IN (SELECT SessionId FROM sessions)");
  }

/*****************************************************************************/
/**************** Clear timeline for this session in database ****************/
/*****************************************************************************/

void TL_Not_ClearTimelineNotesThisSessionFromDB (void)
  {
   /***** Remove timeline for this session *****/
   DB_QueryDELETE ("can not remove timeline",
		   "DELETE FROM tl_timelines"
		   " WHERE SessionId='%s'",
		   Gbl.Session.Id);
  }

/*****************************************************************************/
/******** Create/drop temporary tables with notes already retrieved **********/
/*****************************************************************************/

void TL_Not_CreateTmpTablesWithNotesAlreadyRetrieved (const struct TL_Timeline *Timeline)
  {
   /***** Create temporary table with notes just retrieved *****/
   DB_Query ("can not create temporary table",
	     "CREATE TEMPORARY TABLE tl_tmp_just_retrieved_notes "
	     "(NotCod BIGINT NOT NULL,UNIQUE INDEX(NotCod))"
	     " ENGINE=MEMORY");

   if (Timeline->WhatToGet == TL_GET_ONLY_OLD_PUBS)
      /***** Create temporary table with all notes visible in timeline *****/
      DB_Query ("can not create temporary table",
		"CREATE TEMPORARY TABLE tl_tmp_visible_timeline "
		"(NotCod BIGINT NOT NULL,UNIQUE INDEX(NotCod))"
		" ENGINE=MEMORY"
		" SELECT NotCod FROM tl_timelines WHERE SessionId='%s'",
		Gbl.Session.Id);
  }

void TL_Not_DropTmpTableJustRetrievedNotes (void)
  {
   /***** Drop temporary table with notes just retrieved *****/
   DB_Query ("can not remove temporary table",
	     "DROP TEMPORARY TABLE IF EXISTS tl_tmp_just_retrieved_notes");
  }

void TL_Not_DropTmpTableVisibleTimeline (void)
  {
   /***** Drop temporary table with all notes visible in timeline *****/
   DB_Query ("can not remove temporary table",
             "DROP TEMPORARY TABLE IF EXISTS tl_tmp_visible_timeline");
  }
