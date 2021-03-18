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
#include "swad_call_for_exam.h"
#include "swad_course.h"
#include "swad_forum.h"
#include "swad_global.h"
#include "swad_hierarchy.h"
#include "swad_notice.h"
#include "swad_photo.h"
#include "swad_profile.h"
#include "swad_timeline.h"
#include "swad_timeline_database.h"
#include "swad_timeline_form.h"
#include "swad_timeline_publication.h"

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

static void Tml_Not_WriteTopMessage (Tml_TopMessage_t TopMessage,long PublisherCod);
static void Tml_Not_WriteNote (const struct Tml_Timeline *Timeline,
                               const struct Tml_Not_Note *Not);
static void Tml_Not_WriteAuthorTimeAndContent (const struct Tml_Not_Note *Not,
                                               const struct UsrData *UsrDat);

static void Tml_Not_WriteContent (const struct Tml_Not_Note *Not);
static void Tml_Not_GetAndWriteNoPost (const struct Tml_Not_Note *Not);
static void Tml_Not_GetLocationInHierarchy (const struct Tml_Not_Note *Not,
                                            struct Hie_Hierarchy *Hie,
                                            struct For_Forum *Forum,
                                            char ForumName[For_MAX_BYTES_FORUM_NAME + 1]);
static void Tml_Not_WriteLocationInHierarchy (const struct Tml_Not_Note *Not,
	                                      const struct Hie_Hierarchy *Hie,
                                              const char ForumName[For_MAX_BYTES_FORUM_NAME + 1]);

static void Tml_Not_PutFormGoToAction (const struct Tml_Not_Note *Not,
                                       const struct For_Forums *Forums);

static void Tml_Not_WriteButtonsAndComms (const struct Tml_Timeline *Timeline,
                                          const struct Tml_Not_Note *Not,
                                          const struct UsrData *UsrDat);
static void Tml_Not_WriteButtonToAddAComm (const struct Tml_Not_Note *Not,
                                           const char IdNewComm[Frm_MAX_BYTES_ID + 1]);
static void Tml_Not_WriteFavShaRemAndComms (const struct Tml_Timeline *Timeline,
					    const struct Tml_Not_Note *Not,
					    const struct UsrData *UsrDat);
static void Tml_Not_WriteFavShaRem (const struct Tml_Timeline *Timeline,
                                    const struct Tml_Not_Note *Not,
                                    const struct UsrData *UsrDat);

static void Tml_Not_PutFormToRemoveNote (const struct Tml_Timeline *Timeline,
                                         long NotCod);

static void Tml_Not_RequestRemovalNote (struct Tml_Timeline *Timeline);
static void Tml_Not_PutParamsRemoveNote (void *Timeline);
static void Tml_Not_RemoveNote (void);
static void Tml_Not_RemoveNoteMediaAndDBEntries (struct Tml_Not_Note *Not);

static void Tml_Not_GetDataOfNoteFromRow (MYSQL_ROW row,struct Tml_Not_Note *Not);

static Tml_Not_NoteType_t Tml_Not_GetNoteTypeFromStr (const char *Str);

static void Tml_Not_ResetNote (struct Tml_Not_Note *Not);

/*****************************************************************************/
/****************** Show highlighted note above timeline *********************/
/*****************************************************************************/

void Tml_Not_ShowHighlightedNote (struct Tml_Timeline *Timeline,
                                  struct Tml_Not_Note *Not)
  {
   struct UsrData PublisherDat;
   Ntf_NotifyEvent_t NotifyEvent;
   static const Tml_TopMessage_t TopMessages[Ntf_NUM_NOTIFY_EVENTS] =
     {
      [Ntf_EVENT_UNKNOWN          ] = Tml_TOP_MESSAGE_NONE,
      /* Start tab */
      [Ntf_EVENT_TL_COMMENT       ] = Tml_TOP_MESSAGE_COMMENTED,
      [Ntf_EVENT_TL_FAV           ] = Tml_TOP_MESSAGE_FAVED,
      [Ntf_EVENT_TL_SHARE         ] = Tml_TOP_MESSAGE_SHARED,
      [Ntf_EVENT_TL_MENTION       ] = Tml_TOP_MESSAGE_MENTIONED,
      [Ntf_EVENT_FOLLOWER         ] = Tml_TOP_MESSAGE_NONE,
      /* System tab */
      /* Country tab */
      /* Institution tab */
      /* Center tab */
      /* Degree tab */
      /* Course tab */
      /* Assessment tab */
      [Ntf_EVENT_ASSIGNMENT       ] = Tml_TOP_MESSAGE_NONE,
      [Ntf_EVENT_SURVEY           ] = Tml_TOP_MESSAGE_NONE,
      [Ntf_EVENT_CALL_FOR_EXAM] = Tml_TOP_MESSAGE_NONE,
      /* Files tab */
      [Ntf_EVENT_DOCUMENT_FILE    ] = Tml_TOP_MESSAGE_NONE,
      [Ntf_EVENT_TEACHERS_FILE    ] = Tml_TOP_MESSAGE_NONE,
      [Ntf_EVENT_SHARED_FILE      ] = Tml_TOP_MESSAGE_NONE,
      [Ntf_EVENT_MARKS_FILE       ] = Tml_TOP_MESSAGE_NONE,
      /* Users tab */
      [Ntf_EVENT_ENROLMENT_STD    ] = Tml_TOP_MESSAGE_NONE,
      [Ntf_EVENT_ENROLMENT_NET    ] = Tml_TOP_MESSAGE_NONE,
      [Ntf_EVENT_ENROLMENT_TCH    ] = Tml_TOP_MESSAGE_NONE,
      [Ntf_EVENT_ENROLMENT_REQUEST] = Tml_TOP_MESSAGE_NONE,
      /* Messages tab */
      [Ntf_EVENT_NOTICE           ] = Tml_TOP_MESSAGE_NONE,
      [Ntf_EVENT_FORUM_POST_COURSE] = Tml_TOP_MESSAGE_NONE,
      [Ntf_EVENT_FORUM_REPLY      ] = Tml_TOP_MESSAGE_NONE,
      [Ntf_EVENT_MESSAGE          ] = Tml_TOP_MESSAGE_NONE,
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
   Tml_Not_GetDataOfNoteByCod (Not);

   /***** Show the note highlighted *****/
   /* Begin box */
   Box_BoxBegin (NULL,NULL,
		 NULL,NULL,
		 NULL,Box_CLOSABLE);

      /* Begin container */
      HTM_DIV_Begin ("class=\"TL_WIDTH TL_NEW_PUB\"");

         /* Check and write note with top message */
	 Tml_Not_CheckAndWriteNoteWithTopMsg (Timeline,Not,
					     TopMessages[NotifyEvent],
					     PublisherDat.UsrCod);

      /* End container */
      HTM_DIV_End ();

   /* End box */
   Box_BoxEnd ();
  }

/*****************************************************************************/
/****************** Check and write note with top message ********************/
/*****************************************************************************/

void Tml_Not_CheckAndWriteNoteWithTopMsg (const struct Tml_Timeline *Timeline,
	                                  const struct Tml_Not_Note *Not,
                                          Tml_TopMessage_t TopMessage,
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
   /***** Trivial check: codes *****/
   if (Not->NotCod   <= 0 ||
       Not->UsrCod   <= 0 ||
       Not->NoteType == TL_NOTE_UNKNOWN)
     {
      Ale_ShowAlert (Ale_ERROR,"Error in note.");
      return;
     }

   /***** Write sharer/commenter if distinct to author *****/
   if (TopMessage != Tml_TOP_MESSAGE_NONE)
      Tml_Not_WriteTopMessage (TopMessage,PublisherCod);

   /***** Write note *****/
   Tml_Not_WriteNote (Timeline,Not);
  }

/*****************************************************************************/
/*************** Write sharer/commenter if distinct to author ****************/
/*****************************************************************************/

static void Tml_Not_WriteTopMessage (Tml_TopMessage_t TopMessage,long PublisherCod)
  {
   extern const char *Txt_My_public_profile;
   extern const char *Txt_Another_user_s_profile;
   extern const char *Txt_TIMELINE_NOTE_TOP_MESSAGES[Tml_NUM_TOP_MESSAGES];
   struct UsrData PublisherDat;

   /***** Initialize structure with user's data *****/
   Usr_UsrDataConstructor (&PublisherDat);

   /***** Get user's data *****/
   PublisherDat.UsrCod = PublisherCod;
   if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&PublisherDat,Usr_DONT_GET_PREFS))	// Really we only need EncryptedUsrCod and FullName
     {
      /***** Begin container *****/
      HTM_DIV_Begin ("class=\"Tml_TOP_CONT Tml_TOP_PUBLISHER TL_WIDTH\"");

	 /***** Show publisher's name inside form to go to user's public profile *****/
         /* Begin form */
	 Frm_BeginFormUnique (ActSeeOthPubPrf);
	 Usr_PutParamUsrCodEncrypted (PublisherDat.EnUsrCod);

	    /* Publisher's name */
	    HTM_BUTTON_SUBMIT_Begin (Usr_ItsMe (PublisherCod) ? Txt_My_public_profile :
								Txt_Another_user_s_profile,
				     "BT_LINK Tml_TOP_PUBLISHER",NULL);
	       HTM_Txt (PublisherDat.FullName);
	    HTM_BUTTON_End ();

	 /* End form */
	 Frm_EndForm ();

	 /***** Show action made *****/
	 HTM_TxtF (" %s:",Txt_TIMELINE_NOTE_TOP_MESSAGES[TopMessage]);

      /***** End container *****/
      HTM_DIV_End ();
     }

   /***** Free memory used for user's data *****/
   Usr_UsrDataDestructor (&PublisherDat);
  }

/*****************************************************************************/
/********************************* Show note *********************************/
/*****************************************************************************/

static void Tml_Not_WriteNote (const struct Tml_Timeline *Timeline,
                               const struct Tml_Not_Note *Not)
  {
   struct UsrData UsrDat;	// Author of the note

   /***** Get author data *****/
   Usr_UsrDataConstructor (&UsrDat);
   UsrDat.UsrCod = Not->UsrCod;
   Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat,Usr_DONT_GET_PREFS);

   /***** Left top: author's photo *****/
   Tml_Not_ShowAuthorPhoto (&UsrDat,true);	// Use unique id

   /***** Right top: author's name, time, and content *****/
   Tml_Not_WriteAuthorTimeAndContent (Not,&UsrDat);

   /***** Bottom: buttons and comments *****/
   Tml_Not_WriteButtonsAndComms (Timeline,Not,&UsrDat);

   /***** Free memory used for author's data *****/
   Usr_UsrDataDestructor (&UsrDat);
  }

/*****************************************************************************/
/*********************** Show photo of author of a note **********************/
/*****************************************************************************/

void Tml_Not_ShowAuthorPhoto (struct UsrData *UsrDat,bool FormUnique)
  {
   /***** Show author's photo *****/
   /* Begin container */
   HTM_DIV_Begin ("class=\"TL_LEFT_PHOTO\"");

      /* Photo */
      Pho_ShowUsrPhotoIfAllowed (UsrDat,"PHOTO45x60",Pho_ZOOM,FormUnique);

   /* End container */
   HTM_DIV_End ();
  }

/*****************************************************************************/
/**** Write top right part of a note: author's name, time and note content ***/
/*****************************************************************************/

static void Tml_Not_WriteAuthorTimeAndContent (const struct Tml_Not_Note *Not,
                                               const struct UsrData *UsrDat)
  {
   /***** Begin top container *****/
   HTM_DIV_Begin ("class=\"TL_RIGHT_CONT TL_RIGHT_WIDTH\"");

      /***** Write author's full name *****/
      Tml_Not_WriteAuthorName (UsrDat);

      /***** Write date and time *****/
      Tml_WriteDateTime (Not->DateTimeUTC);

      /***** Write content of the note *****/
      Tml_Not_WriteContent (Not);

   /***** End top container *****/
   HTM_DIV_End ();
  }

/*****************************************************************************/
/*************** Write name and nickname of author of a note *****************/
/*****************************************************************************/

void Tml_Not_WriteAuthorName (const struct UsrData *UsrDat)
  {
   extern const char *Txt_My_public_profile;
   extern const char *Txt_Another_user_s_profile;

   /***** Show user's name inside form to go to user's public profile *****/
   /* Begin form */
   Frm_BeginFormUnique (ActSeeOthPubPrf);
   Usr_PutParamUsrCodEncrypted (UsrDat->EnUsrCod);

      /* Author's name */
      HTM_BUTTON_SUBMIT_Begin (Usr_ItsMe (UsrDat->UsrCod) ? Txt_My_public_profile :
							    Txt_Another_user_s_profile,
			       "BT_LINK TL_RIGHT_AUTHOR TL_RIGHT_AUTHOR_WIDTH DAT_N_BOLD",
			       NULL);
	 HTM_Txt (UsrDat->FullName);
      HTM_BUTTON_End ();

   /* End form */
   Frm_EndForm ();
  }

/*****************************************************************************/
/*********************** Get and write a note content ************************/
/*****************************************************************************/

static void Tml_Not_WriteContent (const struct Tml_Not_Note *Not)
  {
   if (Not->NoteType == TL_NOTE_POST)	// It's a post
      Tml_Pst_GetAndWritePost (Not->Cod);
   else					// Not a post
      Tml_Not_GetAndWriteNoPost (Not);
  }

/*****************************************************************************/
/***************** Get and write a note which is not a post ******************/
/*****************************************************************************/

static void Tml_Not_GetAndWriteNoPost (const struct Tml_Not_Note *Not)
  {
   struct Hie_Hierarchy Hie;
   struct For_Forums Forums;
   char ForumName[For_MAX_BYTES_FORUM_NAME + 1];
   char SummaryStr[Ntf_MAX_BYTES_SUMMARY + 1];

   /***** Reset forums *****/
   For_ResetForums (&Forums);

   /***** Get location in hierarchy *****/
   if (!Not->Unavailable)
      Tml_Not_GetLocationInHierarchy (Not,&Hie,&Forums.Forum,ForumName);

   /***** Write note type *****/
   Tml_Not_PutFormGoToAction (Not,&Forums);

   /***** Write location in hierarchy *****/
   if (!Not->Unavailable)
      Tml_Not_WriteLocationInHierarchy (Not,&Hie,ForumName);

   /***** Get and write note summary *****/
   /* Get note summary */
   Tml_Not_GetNoteSummary (Not,SummaryStr);

   /* Write note summary */
   HTM_DIV_Begin ("class=\"TL_TXT\"");
      HTM_Txt (SummaryStr);
   HTM_DIV_End ();
  }

/*****************************************************************************/
/************************ Get location in hierarchy **************************/
/*****************************************************************************/

static void Tml_Not_GetLocationInHierarchy (const struct Tml_Not_Note *Not,
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
	 /* Get center data */
	 Hie->Ctr.CtrCod = Not->HieCod;
	 Ctr_GetDataOfCenterByCod (&Hie->Ctr);
	 break;
      case TL_NOTE_DEG_DOC_PUB_FILE:
      case TL_NOTE_DEG_SHA_PUB_FILE:
	 /* Get degree data */
	 Hie->Deg.DegCod = Not->HieCod;
	 Deg_GetDataOfDegreeByCod (&Hie->Deg);
	 break;
      case TL_NOTE_CRS_DOC_PUB_FILE:
      case TL_NOTE_CRS_SHA_PUB_FILE:
      case TL_NOTE_CALL_FOR_EXAM:
      case TL_NOTE_NOTICE:
	 /* Get course data */
	 Hie->Crs.CrsCod = Not->HieCod;
	 Crs_GetDataOfCourseByCod (&Hie->Crs);
	 break;
      case TL_NOTE_FORUM_POST:
	 /* Get forum type of the post */
	 For_GetForumTypeAndLocationOfAPost (Not->Cod,Forum);

	 /* Set forum name in recipient's language */
	 For_SetForumName (Forum,ForumName,Gbl.Prefs.Language,false);
	 break;
      default:
	 break;
     }
  }

/*****************************************************************************/
/*********************** Write location in hierarchy *************************/
/*****************************************************************************/

static void Tml_Not_WriteLocationInHierarchy (const struct Tml_Not_Note *Not,
	                                      const struct Hie_Hierarchy *Hie,
                                              const char ForumName[For_MAX_BYTES_FORUM_NAME + 1])
  {
   extern const char *Txt_Forum;
   extern const char *Txt_Course;
   extern const char *Txt_Degree;
   extern const char *Txt_Center;
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
	 /* Write location (center) in hierarchy */
	 HTM_DIV_Begin ("class=\"TL_LOC\"");
	    HTM_TxtF ("%s:&nbsp;%s",Txt_Center,Hie->Ctr.ShrtName);
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
      case TL_NOTE_CALL_FOR_EXAM:
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

static void Tml_Not_PutFormGoToAction (const struct Tml_Not_Note *Not,
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
      /* Center tab */
      [TL_NOTE_CTR_DOC_PUB_FILE ] = ActReqDatSeeDocCtr,
      [TL_NOTE_CTR_SHA_PUB_FILE ] = ActReqDatShaCtr,
      /* Degree tab */
      [TL_NOTE_DEG_DOC_PUB_FILE ] = ActReqDatSeeDocDeg,
      [TL_NOTE_DEG_SHA_PUB_FILE ] = ActReqDatShaDeg,
      /* Course tab */
      [TL_NOTE_CRS_DOC_PUB_FILE ] = ActReqDatSeeDocCrs,
      [TL_NOTE_CRS_SHA_PUB_FILE ] = ActReqDatShaCrs,
      /* Assessment tab */
      [TL_NOTE_CALL_FOR_EXAM] = ActSeeOneCfe,
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
      /* Center tab */
      [TL_NOTE_CTR_DOC_PUB_FILE ] = "file.svg",
      [TL_NOTE_CTR_SHA_PUB_FILE ] = "file.svg",
      /* Degree tab */
      [TL_NOTE_DEG_DOC_PUB_FILE ] = "file.svg",
      [TL_NOTE_DEG_SHA_PUB_FILE ] = "file.svg",
      /* Course tab */
      [TL_NOTE_CRS_DOC_PUB_FILE ] = "file.svg",
      [TL_NOTE_CRS_SHA_PUB_FILE ] = "file.svg",
      /* Assessment tab */
      [TL_NOTE_CALL_FOR_EXAM] = "bullhorn.svg",
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
      /* Begin container */
      HTM_DIV_Begin ("class=\"TL_FORM_OFF\"");

         /* Text ("not available") */
	 HTM_Txt (Txt_TIMELINE_NOTE[Not->NoteType]);
	 if (Not->Unavailable)
	    HTM_TxtF ("&nbsp;(%s)",Txt_not_available);

      /* End container */
      HTM_DIV_End ();
     }
   else			// Not inside another form
     {
      /***** Begin container *****/
      HTM_DIV_Begin ("class=\"TL_FORM\"");

	 /***** Begin form with parameters depending on the type of note *****/
	 switch (Not->NoteType)
	   {
	    case TL_NOTE_INS_DOC_PUB_FILE:
	    case TL_NOTE_INS_SHA_PUB_FILE:
	       Frm_BeginFormUnique (TL_DefaultActions[Not->NoteType]);
	       Brw_PutHiddenParamFilCod (Not->Cod);
	       if (Not->HieCod != Gbl.Hierarchy.Ins.InsCod)	// Not the current institution
		  Ins_PutParamInsCod (Not->HieCod);		// Go to another institution
	       break;
	    case TL_NOTE_CTR_DOC_PUB_FILE:
	    case TL_NOTE_CTR_SHA_PUB_FILE:
	       Frm_BeginFormUnique (TL_DefaultActions[Not->NoteType]);
	       Brw_PutHiddenParamFilCod (Not->Cod);
	       if (Not->HieCod != Gbl.Hierarchy.Ctr.CtrCod)	// Not the current center
		  Ctr_PutParamCtrCod (Not->HieCod);		// Go to another center
	       break;
	    case TL_NOTE_DEG_DOC_PUB_FILE:
	    case TL_NOTE_DEG_SHA_PUB_FILE:
	       Frm_BeginFormUnique (TL_DefaultActions[Not->NoteType]);
	       Brw_PutHiddenParamFilCod (Not->Cod);
	       if (Not->HieCod != Gbl.Hierarchy.Deg.DegCod)	// Not the current degree
		  Deg_PutParamDegCod (Not->HieCod);		// Go to another degree
	       break;
	    case TL_NOTE_CRS_DOC_PUB_FILE:
	    case TL_NOTE_CRS_SHA_PUB_FILE:
	       Frm_BeginFormUnique (TL_DefaultActions[Not->NoteType]);
	       Brw_PutHiddenParamFilCod (Not->Cod);
	       if (Not->HieCod != Gbl.Hierarchy.Crs.CrsCod)	// Not the current course
		  Crs_PutParamCrsCod (Not->HieCod);		// Go to another course
	       break;
	    case TL_NOTE_CALL_FOR_EXAM:
	       Frm_SetAnchorStr (Not->Cod,&Anchor);
	       Frm_BeginFormUniqueAnchor (TL_DefaultActions[Not->NoteType],
					  Anchor);	// Locate on this specific exam
	       Frm_FreeAnchorStr (Anchor);
	       Cfe_PutHiddenParamExaCod (Not->Cod);
	       if (Not->HieCod != Gbl.Hierarchy.Crs.CrsCod)	// Not the current course
		  Crs_PutParamCrsCod (Not->HieCod);		// Go to another course
	       break;
	    case TL_NOTE_POST:	// Not applicable
	       return;
	    case TL_NOTE_FORUM_POST:
	       Frm_BeginFormUnique (For_ActionsSeeFor[Forums->Forum.Type]);
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
	       Frm_BeginFormUniqueAnchor (TL_DefaultActions[Not->NoteType],
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
	    /* Begin button */
	    HTM_BUTTON_SUBMIT_Begin (Txt_TIMELINE_NOTE[Not->NoteType],
				     Str_BuildStringStr ("BT_LINK %s ICO_HIGHLIGHT",
							 The_ClassFormInBoxBold[Gbl.Prefs.Theme]),
				     NULL);
	    Str_FreeString ();

	       /* Icon and text */
	       Ico_PutIcon (TL_Icons[Not->NoteType],
	                    Txt_TIMELINE_NOTE[Not->NoteType],"CONTEXT_ICO_x16");
	       HTM_TxtF ("&nbsp;%s",Txt_TIMELINE_NOTE[Not->NoteType]);

	    /* End button */
	    HTM_BUTTON_End ();

	 /***** End form *****/
	 Frm_EndForm ();

      /***** End container *****/
      HTM_DIV_End ();
     }
  }

/*****************************************************************************/
/********************** Get note summary and content *************************/
/*****************************************************************************/

void Tml_Not_GetNoteSummary (const struct Tml_Not_Note *Not,
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
      case TL_NOTE_CALL_FOR_EXAM:
         Cfe_GetSummaryAndContentCallForExam (SummaryStr,NULL,Not->Cod,false);
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

static void Tml_Not_WriteButtonsAndComms (const struct Tml_Timeline *Timeline,
                                          const struct Tml_Not_Note *Not,
                                          const struct UsrData *UsrDat)	// Author
  {
   char IdNewComm[Frm_MAX_BYTES_ID + 1];

   /***** Create unique id for new comment *****/
   Frm_SetUniqueId (IdNewComm);

   /***** Left: button to add a comment *****/
   Tml_Not_WriteButtonToAddAComm (Not,IdNewComm);

   /***** Right: write favs, shared and remove buttons, and comments *****/
   Tml_Not_WriteFavShaRemAndComms (Timeline,Not,UsrDat);

   /***** Put hidden form to write a new comment *****/
   Tml_Com_PutPhotoAndFormToWriteNewComm (Timeline,Not->NotCod,IdNewComm);
  }

/*****************************************************************************/
/********************** Write button to add a comment ************************/
/*****************************************************************************/

static void Tml_Not_WriteButtonToAddAComm (const struct Tml_Not_Note *Not,
                                           const char IdNewComm[Frm_MAX_BYTES_ID + 1])
  {
   /***** Begin container *****/
   HTM_DIV_Begin ("class=\"TL_BOTTOM_LEFT\"");

      /***** Button to add a comment *****/
      if (Not->Unavailable)	// Unavailable notes can not be commented
	 Tml_Com_PutIconCommDisabled ();
      else
	 Tml_Com_PutIconToToggleComm (IdNewComm);

   /***** End container *****/
   HTM_DIV_End ();
  }

/*****************************************************************************/
/******* Write favs, shared and remove buttons, and comments of a note *******/
/*****************************************************************************/

static void Tml_Not_WriteFavShaRemAndComms (const struct Tml_Timeline *Timeline,
					    const struct Tml_Not_Note *Not,
					    const struct UsrData *UsrDat)	// Author
  {
   /***** Begin container *****/
   HTM_DIV_Begin ("class=\"TL_BOTTOM_RIGHT TL_RIGHT_WIDTH\"");

      /***** Write favs, shared and remove buttons int the foot of a note *****/
      Tml_Not_WriteFavShaRem (Timeline,Not,UsrDat);

      /***** Comments *****/
      Tml_Com_WriteCommsInNote (Timeline,Not);

   /***** End container *****/
   HTM_DIV_End ();
  }

/*****************************************************************************/
/******* Write favs, shared and remove buttons in the foot of a note *********/
/*****************************************************************************/

static void Tml_Not_WriteFavShaRem (const struct Tml_Timeline *Timeline,
                                    const struct Tml_Not_Note *Not,
                                    const struct UsrData *UsrDat)	// Author
  {
   static unsigned NumDiv = 0;	// Used to create unique div id for fav and shared

   NumDiv++;

   /***** Begin foot container *****/
   HTM_DIV_Begin ("class=\"TL_FOOT TL_RIGHT_WIDTH\"");

      /***** Foot column 1: fav zone *****/
      HTM_DIV_Begin ("id=\"fav_not_%s_%u\""
	             " class=\"TL_FAV_NOT TL_FAV_NOT_WIDTH\"",
		     Gbl.UniqueNameEncrypted,NumDiv);
	 Tml_Usr_PutIconFavSha (Tml_Usr_FAV_UNF_NOTE,
	                        Not->NotCod,Not->UsrCod,Not->NumFavs,
			        Tml_Usr_SHOW_FEW_USRS);
      HTM_DIV_End ();

      /***** Foot column 2: share zone *****/
      HTM_DIV_Begin ("id=\"sha_not_%s_%u\""
	             " class=\"TL_SHA_NOT TL_SHA_NOT_WIDTH\"",
		     Gbl.UniqueNameEncrypted,NumDiv);
	 Tml_Usr_PutIconFavSha (Tml_Usr_SHA_UNS_NOTE,
	                       Not->NotCod,Not->UsrCod,Not->NumShared,
	                       Tml_Usr_SHOW_FEW_USRS);
      HTM_DIV_End ();

      /***** Foot column 3: icon to remove this note *****/
      HTM_DIV_Begin ("class=\"TL_REM\"");
	 if (Usr_ItsMe (UsrDat->UsrCod))	// I am the author
	    Tml_Not_PutFormToRemoveNote (Timeline,Not->NotCod);
      HTM_DIV_End ();

   /***** End foot container *****/
   HTM_DIV_End ();
  }

/*****************************************************************************/
/**************************** Form to remove note ****************************/
/*****************************************************************************/

static void Tml_Not_PutFormToRemoveNote (const struct Tml_Timeline *Timeline,
                                        long NotCod)
  {
   extern const char *Txt_Remove;

   /***** Form to remove publication *****/
   /* Begin form */
   Tml_Frm_BeginForm (Timeline,Tml_Frm_REQ_REM_NOTE);
   Tml_Not_PutHiddenParamNotCod (NotCod);

      /* Icon to remove */
      Ico_PutIconLink ("trash.svg",Txt_Remove);

   /* End form */
   Tml_Frm_EndForm ();
  }

/*****************************************************************************/
/***************** Store and publish a note into database ********************/
/*****************************************************************************/

void Tml_Not_StoreAndPublishNote (Tml_Not_NoteType_t NoteType,long Cod)
  {
   struct Tml_Pub_Publication Pub;

   Tml_Not_StoreAndPublishNoteInternal (NoteType,Cod,&Pub);
  }

void Tml_Not_StoreAndPublishNoteInternal (Tml_Not_NoteType_t NoteType,long Cod,
                                          struct Tml_Pub_Publication *Pub)
  {
   long HieCod;	// Hierarchy code (institution/center/degree/course)

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
      case TL_NOTE_CALL_FOR_EXAM:
      case TL_NOTE_NOTICE:
	 HieCod = Gbl.Hierarchy.Crs.CrsCod;
	 break;
      default:
	 HieCod = -1L;
         break;
     }

   /***** Publish note in timeline *****/
   Pub->PublisherCod = Gbl.Usrs.Me.UsrDat.UsrCod;
   Pub->NotCod       = Tml_DB_CreateNewNote (NoteType,Cod,Pub->PublisherCod,HieCod);
   Pub->PubType      = Tml_Pub_ORIGINAL_NOTE;
   Tml_Pub_PublishPubInTimeline (Pub);
  }

/*****************************************************************************/
/****************** Mark notes of one file as unavailable ********************/
/*****************************************************************************/

void Tml_Not_MarkNoteOneFileAsUnavailable (const char *Path)
  {
   extern const Brw_FileBrowser_t Brw_FileBrowserForDB_files[Brw_NUM_TYPES_FILE_BROWSER];
   Brw_FileBrowser_t FileBrowser = Brw_FileBrowserForDB_files[Gbl.FileBrowser.Type];
   long FilCod;
   Tml_Not_NoteType_t NoteType;

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
	    Tml_DB_MarkNoteAsUnavailable (NoteType,FilCod);
	   }
         break;
      default:
	 break;
     }
  }

/*****************************************************************************/
/***** Mark possible notes involving children of a folder as unavailable *****/
/*****************************************************************************/

void Tml_Not_MarkNotesChildrenOfFolderAsUnavailable (const char *Path)
  {
   extern const Brw_FileBrowser_t Brw_FileBrowserForDB_files[Brw_NUM_TYPES_FILE_BROWSER];
   Brw_FileBrowser_t FileBrowser = Brw_FileBrowserForDB_files[Gbl.FileBrowser.Type];
   Tml_Not_NoteType_t NoteType;

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
         Tml_DB_MarkNotesChildrenOfFolderAsUnavailable (NoteType,
                                                       FileBrowser,Brw_GetCodForFiles (),
                                                       Path);
         break;
      default:
	 break;
     }
  }

/*****************************************************************************/
/****************** Put parameter with the code of a note ********************/
/*****************************************************************************/

void Tml_Not_PutHiddenParamNotCod (long NotCod)
  {
   Par_PutHiddenParamLong (NULL,"NotCod",NotCod);
  }

/*****************************************************************************/
/****************** Get parameter with the code of a note ********************/
/*****************************************************************************/

long Tml_Not_GetParamNotCod (void)
  {
   /***** Get note code *****/
   return Par_GetParToLong ("NotCod");
  }

/*****************************************************************************/
/*********************** Request the removal of a note ***********************/
/*****************************************************************************/

void Tml_Not_RequestRemNoteUsr (void)
  {
   struct Tml_Timeline Timeline;

   /***** Reset timeline context *****/
   Tml_ResetTimeline (&Timeline);

   /***** Get user whom profile is displayed *****/
   Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ();

   /***** Show user's profile *****/
   Prf_ShowUserProfile (&Gbl.Usrs.Other.UsrDat);

   /***** Begin section *****/
   HTM_SECTION_Begin (TL_TIMELINE_SECTION_ID);

      /***** Request the removal of note *****/
      Tml_Not_RequestRemovalNote (&Timeline);

      /***** Write timeline again (user) *****/
      Tml_ShowTimelineUsr (&Timeline);

   /***** End section *****/
   HTM_SECTION_End ();
  }

void Tml_Not_RequestRemNoteGbl (void)
  {
   struct Tml_Timeline Timeline;

   /***** Initialize timeline *****/
   Tml_InitTimelineGbl (&Timeline);

   /***** Request the removal of note *****/
   Tml_Not_RequestRemovalNote (&Timeline);

   /***** Write timeline again (global) *****/
   Tml_ShowNoteAndTimelineGbl (&Timeline);
  }

static void Tml_Not_RequestRemovalNote (struct Tml_Timeline *Timeline)
  {
   extern const char *Txt_Do_you_really_want_to_remove_the_following_post;
   struct Tml_Not_Note Not;

   /***** Get data of note *****/
   Not.NotCod = Tml_Not_GetParamNotCod ();
   Tml_Not_GetDataOfNoteByCod (&Not);

   /***** Do some checks *****/
   if (!Tml_Usr_CheckIfICanRemove (Not.NotCod,Not.UsrCod))
      return;

   /***** Show question and button to remove note *****/
   /* Begin alert */
   Tml_Frm_BeginAlertRemove (Txt_Do_you_really_want_to_remove_the_following_post);

   /* Show note */
   Box_BoxBegin (NULL,NULL,
		 NULL,NULL,
		 NULL,Box_CLOSABLE);
      HTM_DIV_Begin ("class=\"TL_WIDTH\"");
	 Tml_Not_CheckAndWriteNoteWithTopMsg (Timeline,&Not,
					     Tml_TOP_MESSAGE_NONE,
					     -1L);
      HTM_DIV_End ();
   Box_BoxEnd ();

   /* End alert */
   Timeline->NotCod = Not.NotCod;	// Note to be removed
   Tml_Frm_EndAlertRemove (Timeline,Tml_Frm_REM_NOTE,
			  Tml_Not_PutParamsRemoveNote);
  }

/*****************************************************************************/
/********************* Put parameters to remove a note ***********************/
/*****************************************************************************/

static void Tml_Not_PutParamsRemoveNote (void *Timeline)
  {
   if (Timeline)
     {
      if (Gbl.Usrs.Other.UsrDat.UsrCod > 0)	// User's timeline
	 Usr_PutParamOtherUsrCodEncrypted (Gbl.Usrs.Other.UsrDat.EnUsrCod);
      else					// Global timeline
	 Usr_PutHiddenParamWho (((struct Tml_Timeline *) Timeline)->Who);
      Tml_Not_PutHiddenParamNotCod (((struct Tml_Timeline *) Timeline)->NotCod);
     }
  }

/*****************************************************************************/
/******************************* Remove a note *******************************/
/*****************************************************************************/

void Tml_Not_RemoveNoteUsr (void)
  {
   struct Tml_Timeline Timeline;

   /***** Reset timeline context *****/
   Tml_ResetTimeline (&Timeline);

   /***** Get user whom profile is displayed *****/
   Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ();

   /***** Show user's profile *****/
   Prf_ShowUserProfile (&Gbl.Usrs.Other.UsrDat);

   /***** Begin section *****/
   HTM_SECTION_Begin (TL_TIMELINE_SECTION_ID);

      /***** Remove a note *****/
      Tml_Not_RemoveNote ();

      /***** Write updated timeline after removing (user) *****/
      Tml_ShowTimelineUsr (&Timeline);

   /***** End section *****/
   HTM_SECTION_End ();
  }

void Tml_Not_RemoveNoteGbl (void)
  {
   struct Tml_Timeline Timeline;

   /***** Initialize timeline *****/
   Tml_InitTimelineGbl (&Timeline);

   /***** Remove a note *****/
   Tml_Not_RemoveNote ();

   /***** Write updated timeline after removing (global) *****/
   Tml_ShowNoteAndTimelineGbl (&Timeline);
  }

static void Tml_Not_RemoveNote (void)
  {
   extern const char *Txt_The_post_no_longer_exists;
   extern const char *Txt_TIMELINE_Post_removed;
   struct Tml_Not_Note Not;

   /***** Get data of note *****/
   Not.NotCod = Tml_Not_GetParamNotCod ();
   Tml_Not_GetDataOfNoteByCod (&Not);

   /***** Trivial check 1: note code should be > 0 *****/
   if (Not.NotCod <= 0)
     {
      Ale_ShowAlert (Ale_WARNING,Txt_The_post_no_longer_exists);
      return;
     }

   /***** Trivial check 2: Am I the author of this note *****/
   if (!Usr_ItsMe (Not.UsrCod))
     {
      Ale_ShowAlert (Ale_ERROR,"You are not the author.");
      return;
     }

   /***** Delete note from database *****/
   Tml_Not_RemoveNoteMediaAndDBEntries (&Not);

   /***** Reset note *****/
   Tml_Not_ResetNote (&Not);

   /***** Message of success *****/
   Ale_ShowAlert (Ale_SUCCESS,Txt_TIMELINE_Post_removed);
  }

/*****************************************************************************/
/*********************** Remove a note from database *************************/
/*****************************************************************************/

static void Tml_Not_RemoveNoteMediaAndDBEntries (struct Tml_Not_Note *Not)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   long PubCod;
   unsigned long NumComms;
   unsigned long NumComm;
   long MedCod;

   /***** Remove comments associated to this note *****/
   /* Get comments of this note */
   NumComms = Tml_DB_GetComms (Not->NotCod,&mysql_res);

   /* For each comment... */
   for (NumComm = 0;
	NumComm < NumComms;
	NumComm++)
     {
      /* Get code of comment **/
      row = mysql_fetch_row (mysql_res);
      PubCod = Str_ConvertStrCodToLongCod (row[0]);

      /* Remove media associated to comment
	 and delete comment from database */
      Tml_Com_RemoveCommMediaAndDBEntries (PubCod);
     }

   /* Free structure that stores the query result */
   DB_FreeMySQLResult (&mysql_res);

   /***** Remove media associated to post *****/
   if (Not->NoteType == TL_NOTE_POST)
      if ((MedCod = Tml_DB_GetMedCodFromPost (Not->Cod)) > 0)
	 Med_RemoveMedia (MedCod);

   /***** Mark possible notifications on the publications
          of this note as removed *****/
   PubCod = Tml_DB_GetPubCodOfOriginalNote (Not->NotCod);
   if (PubCod > 0)
     {
      Ntf_MarkNotifAsRemoved (Ntf_EVENT_TL_FAV    ,PubCod);
      Ntf_MarkNotifAsRemoved (Ntf_EVENT_TL_SHARE  ,PubCod);
      Ntf_MarkNotifAsRemoved (Ntf_EVENT_TL_MENTION,PubCod);
     }

   /***** Remove favs for this note *****/
   Tml_DB_RemoveNoteFavs (Not->NotCod);

   /***** Remove all publications of this note *****/
   Tml_DB_RemoveNotePubs (Not->NotCod);

   /***** Remove note *****/
   Tml_DB_RemoveNote (Not->NotCod);

   if (Not->NoteType == TL_NOTE_POST)
      /***** Remove post *****/
      Tml_DB_RemovePost (Not->Cod);
  }

/*****************************************************************************/
/************************ Get data of note from row **************************/
/*****************************************************************************/

static void Tml_Not_GetDataOfNoteFromRow (MYSQL_ROW row,struct Tml_Not_Note *Not)
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
   Not->NoteType    = Tml_Not_GetNoteTypeFromStr (row[1]);

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
   Not->NumShared = Tml_DB_GetNumSharers (Not->NotCod,Not->UsrCod);

   /***** Get number of times this note has been favourited *****/
   Not->NumFavs = Tml_DB_GetNumFavers (Tml_Usr_FAV_UNF_NOTE,
                                      Not->NotCod,Not->UsrCod);
  }

/*****************************************************************************/
/********* Get note type from string number coming from database *************/
/*****************************************************************************/

static Tml_Not_NoteType_t Tml_Not_GetNoteTypeFromStr (const char *Str)
  {
   unsigned UnsignedNum;

   if (sscanf (Str,"%u",&UnsignedNum) == 1)
      if (UnsignedNum < TL_NOT_NUM_NOTE_TYPES)
         return (Tml_Not_NoteType_t) UnsignedNum;

   return TL_NOTE_UNKNOWN;
  }

/*****************************************************************************/
/*************************** Reset fields of note ****************************/
/*****************************************************************************/

static void Tml_Not_ResetNote (struct Tml_Not_Note *Not)
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
/******************** Get data of note using its code ************************/
/*****************************************************************************/

void Tml_Not_GetDataOfNoteByCod (struct Tml_Not_Note *Not)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;

   /***** Trivial check: note code should be > 0 *****/
   if (Not->NotCod <= 0)
     {
      /***** Reset fields of note *****/
      Tml_Not_ResetNote (Not);
      return;
     }

   /***** Get data of note from database *****/
   if (Tml_DB_GetDataOfNoteByCod (Not->NotCod,&mysql_res))
     {
      /***** Get data of note *****/
      row = mysql_fetch_row (mysql_res);
      Tml_Not_GetDataOfNoteFromRow (row,Not);
     }
   else
      /***** Reset fields of note *****/
      Tml_Not_ResetNote (Not);

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }
