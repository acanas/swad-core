// swad_timeline_note.c: social timeline notes

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2025 Antonio Cañas Vargas

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
#include <stdlib.h>		// For free

#include "swad_action_list.h"
#include "swad_alert.h"
#include "swad_box.h"
#include "swad_browser_database.h"
#include "swad_call_for_exam.h"
#include "swad_course.h"
#include "swad_error.h"
#include "swad_forum.h"
#include "swad_global.h"
#include "swad_hierarchy.h"
#include "swad_notice.h"
#include "swad_notification_database.h"
#include "swad_parameter_code.h"
#include "swad_photo.h"
#include "swad_profile.h"
#include "swad_timeline.h"
#include "swad_timeline_database.h"
#include "swad_timeline_form.h"
#include "swad_timeline_publication.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/**************************** Private constants ******************************/
/*****************************************************************************/

static TmlNot_Type_t TmlNot_NoteType[Brw_NUM_TYPES_FILE_BROWSER] =
  {
   [Brw_ADMI_DOC_INS] = TmlNot_INS_DOC_PUB_FILE,
   [Brw_ADMI_SHR_INS] = TmlNot_INS_SHA_PUB_FILE,
   [Brw_ADMI_DOC_CTR] = TmlNot_CTR_DOC_PUB_FILE,
   [Brw_ADMI_SHR_CTR] = TmlNot_CTR_SHA_PUB_FILE,
   [Brw_ADMI_DOC_DEG] = TmlNot_DEG_DOC_PUB_FILE,
   [Brw_ADMI_SHR_DEG] = TmlNot_DEG_SHA_PUB_FILE,
   [Brw_ADMI_DOC_CRS] = TmlNot_CRS_DOC_PUB_FILE,
   [Brw_ADMI_SHR_CRS] = TmlNot_CRS_SHA_PUB_FILE,
  };

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void TmlNot_WriteTopMessage (Tml_TopMessage_t TopMessage,long PublisherCod);
static void TmlNot_WriteNote (const struct Tml_Timeline *Timeline,
                              const struct TmlNot_Note *Not);
static void TmlNot_WriteAuthorTimeAndContent (const struct TmlNot_Note *Not,
                                              const struct Usr_Data *UsrDat);

static void TmlNot_WriteContent (const struct TmlNot_Note *Not);
static void TmlNot_GetAndWriteNoPost (const struct TmlNot_Note *Not);
static void TmlNot_GetLocationInHierarchy (const struct TmlNot_Note *Not,
					   struct Hie_Node Hie[Hie_NUM_LEVELS],
                                           struct For_Forum *Forum,
                                           char ForumName[For_MAX_BYTES_FORUM_NAME + 1]);
static void TmlNot_WriteLocationInHierarchy (const struct TmlNot_Note *Not,
					     struct Hie_Node Hie[Hie_NUM_LEVELS],
                                             const char ForumName[For_MAX_BYTES_FORUM_NAME + 1]);

static void TmlNot_PutFormGoToAction (const struct TmlNot_Note *Not,
                                      const struct For_Forums *Forums);

static void TmlNot_WriteButtonsAndComms (const struct Tml_Timeline *Timeline,
                                         const struct TmlNot_Note *Not,
                                         const struct Usr_Data *UsrDat);
static void TmlNot_WriteButtonToAddAComm (const struct Tml_Timeline *Timeline,
                                          const struct TmlNot_Note *Not,
                                          const char IdNewComm[Frm_MAX_BYTES_ID + 1]);
static void TmlNot_WriteFavShaRemAndComms (const struct Tml_Timeline *Timeline,
					   const struct TmlNot_Note *Not,
					   const struct Usr_Data *UsrDat);
static void TmlNot_WriteFavShaRem (const struct Tml_Timeline *Timeline,
                                   const struct TmlNot_Note *Not,
                                   const struct Usr_Data *UsrDat);

static void TmlNot_PutFormToRemoveNote (const struct Tml_Timeline *Timeline,
                                        long NotCod);

static void TmlNot_ReqRemNote (struct Tml_Timeline *Timeline);
static void TmlNot_PutParsRemoveNote (void *Timeline);
static void TmlNot_RemoveNote (void);
static void TmlNot_RemoveNoteMediaAndDBEntries (struct TmlNot_Note *Not);

static void TmlNot_GetNoteDataFromRow (MYSQL_RES *mysql_res,
                                       struct TmlNot_Note *Not);

static TmlNot_Type_t TmlNot_GetNoteTypeFromStr (const char *Str);

static void TmlNot_ResetNote (struct TmlNot_Note *Not);

/*****************************************************************************/
/****************** Show highlighted note above timeline *********************/
/*****************************************************************************/

void TmlNot_ShowHighlightedNote (struct Tml_Timeline *Timeline,
                                 struct TmlNot_Note *Not)
  {
   struct Usr_Data PublisherDat;
   Ntf_NotifyEvent_t NotifyEvent;
   static Tml_TopMessage_t TopMessages[Ntf_NUM_NOTIFY_EVENTS] =
     {
      [Ntf_EVENT_TML_COMMENT] = Tml_TOP_MESSAGE_COMMENTED,
      [Ntf_EVENT_TML_FAV    ] = Tml_TOP_MESSAGE_FAVED,
      [Ntf_EVENT_TML_SHARE  ] = Tml_TOP_MESSAGE_SHARED,
      [Ntf_EVENT_TML_MENTION] = Tml_TOP_MESSAGE_MENTIONED,
     };

   /***** Get other parameters *****/
   /* Get the publisher who did the action
      (publishing, commenting, faving, sharing, mentioning) */
   Usr_GetParOtherUsrCodEncrypted (&PublisherDat);

   /* Get what he/she did */
   NotifyEvent = Ntf_GetParNotifyEvent ();

   /***** Get data of the note *****/
   TmlNot_GetNoteDataByCod (Not);

   /***** Show the note highlighted *****/
   /* Begin box */
   Box_BoxBegin (NULL,NULL,NULL,NULL,Box_CLOSABLE);

      /* Begin container */
      HTM_DIV_Begin ("class=\"Tml_WIDTH Tml_NEW_PUB_%s\"",The_GetSuffix ());

         /* Check and write note with top message */
	 TmlNot_CheckAndWriteNoteWithTopMsg (Timeline,Not,
					     TopMessages[NotifyEvent],
					     PublisherDat.UsrCod);

      /* End container */
      HTM_DIV_End ();

   /* End box */
   Box_BoxEnd ();
   HTM_BR ();
  }

/*****************************************************************************/
/****************** Check and write note with top message ********************/
/*****************************************************************************/

void TmlNot_CheckAndWriteNoteWithTopMsg (const struct Tml_Timeline *Timeline,
	                                 const struct TmlNot_Note *Not,
                                         Tml_TopMessage_t TopMessage,
                                         long PublisherCod)	// Who did the action (publication, commenting, faving, sharing, mentioning)
  {
   /*
    ___________________________________________
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
   if (Not->NotCod <= 0 ||
       Not->UsrCod <= 0 ||
       Not->Type   == TmlNot_UNKNOWN)
     {
      Ale_ShowAlert (Ale_ERROR,"Error in note.");
      return;
     }

   /***** Write sharer/commenter if distinct to author *****/
   if (TopMessage != Tml_TOP_MESSAGE_NONE)
      TmlNot_WriteTopMessage (TopMessage,PublisherCod);

   /***** Write note *****/
   TmlNot_WriteNote (Timeline,Not);
  }

/*****************************************************************************/
/*************** Write sharer/commenter if distinct to author ****************/
/*****************************************************************************/

static void TmlNot_WriteTopMessage (Tml_TopMessage_t TopMessage,long PublisherCod)
  {
   extern const char *Txt_TIMELINE_NOTE_TOP_MESSAGES[Tml_NUM_TOP_MESSAGES];
   struct Usr_Data PublisherDat;

   /***** Initialize structure with user's data *****/
   Usr_UsrDataConstructor (&PublisherDat);

   /***** Get user's data *****/
   PublisherDat.UsrCod = PublisherCod;
   if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&PublisherDat,	// Really we only need EncryptedUsrCod and FullName
                                                Usr_DONT_GET_PREFS,
                                                Usr_DONT_GET_ROLE_IN_CRS) == Exi_EXISTS)
     {
      /***** Begin container *****/
      HTM_DIV_Begin ("class=\"Tml_TOP_CONT Tml_TOP_PUBLISHER Tml_WIDTH\"");

	 /***** Show publisher's name inside form to go to user's public profile *****/
         TmlNot_WriteAuthorName (&PublisherDat,"BT_LINK");

	 /***** Show action made *****/
         HTM_SP ();
         HTM_Txt (Txt_TIMELINE_NOTE_TOP_MESSAGES[TopMessage]); HTM_Colon ();

      /***** End container *****/
      HTM_DIV_End ();
     }

   /***** Free memory used for user's data *****/
   Usr_UsrDataDestructor (&PublisherDat);
  }

/*****************************************************************************/
/********************************* Show note *********************************/
/*****************************************************************************/

static void TmlNot_WriteNote (const struct Tml_Timeline *Timeline,
                              const struct TmlNot_Note *Not)
  {
   struct Usr_Data UsrDat;	// Author of the note
   __attribute__((unused)) Exi_Exist_t UsrExists;

   /***** Get author data *****/
   Usr_UsrDataConstructor (&UsrDat);
   UsrDat.UsrCod = Not->UsrCod;
   UsrExists = Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat,
							Usr_DONT_GET_PREFS,
							Usr_DONT_GET_ROLE_IN_CRS);

   /***** Left top: author's photo *****/
   TmlNot_ShowAuthorPhoto (&UsrDat);

   /***** Right top: author's name, time, and content *****/
   TmlNot_WriteAuthorTimeAndContent (Not,&UsrDat);
   HTM_BR ();

   /***** Bottom: buttons and comments *****/
   TmlNot_WriteButtonsAndComms (Timeline,Not,&UsrDat);

   /***** Free memory used for author's data *****/
   Usr_UsrDataDestructor (&UsrDat);
  }

/*****************************************************************************/
/*********************** Show photo of author of a note **********************/
/*****************************************************************************/

void TmlNot_ShowAuthorPhoto (struct Usr_Data *UsrDat)
  {
   static const char *ClassPhoto[PhoSha_NUM_SHAPES] =
     {
      [PhoSha_SHAPE_CIRCLE   ] = "PHOTOC45x60",
      [PhoSha_SHAPE_ELLIPSE  ] = "PHOTOE45x60",
      [PhoSha_SHAPE_OVAL     ] = "PHOTOO45x60",
      [PhoSha_SHAPE_RECTANGLE] = "PHOTOR45x60",
     };

   /***** Begin container *****/
   HTM_DIV_Begin ("class=\"Tml_LEFT_PHOTO\"");

      /***** Photo *****/
      Pho_ShowUsrPhotoIfAllowed (UsrDat,
                                 ClassPhoto[Gbl.Prefs.PhotoShape],Pho_ZOOM);

   /***** End container *****/
   HTM_DIV_End ();
  }

/*****************************************************************************/
/**** Write top right part of a note: author's name, time and note content ***/
/*****************************************************************************/

static void TmlNot_WriteAuthorTimeAndContent (const struct TmlNot_Note *Not,
                                              const struct Usr_Data *UsrDat)
  {
   char *Class;

   /***** Begin top container *****/
   HTM_DIV_Begin ("class=\"Tml_RIGHT_CONT Tml_RIGHT_WIDTH\"");

      /***** Write author's full name *****/
      if (asprintf (&Class,"Tml_RIGHT_AUTHOR Tml_RIGHT_AUTHOR_WIDTH BT_LINK DAT_STRONG_%s BOLD",
                    The_GetSuffix ()) < 0)
	 Err_NotEnoughMemoryExit ();
      TmlNot_WriteAuthorName (UsrDat,Class);
      free (Class);

      /***** Write date and time *****/
      Tml_WriteDateTime (Not->DateTimeUTC);

      /***** Write content of the note *****/
      TmlNot_WriteContent (Not);

   /***** End top container *****/
   HTM_DIV_End ();
  }

/*****************************************************************************/
/*************** Write name and nickname of author of a note *****************/
/*****************************************************************************/

void TmlNot_WriteAuthorName (const struct Usr_Data *UsrDat,
                             const char *Class)
  {
   extern const char *Txt_My_public_profile;
   extern const char *Txt_Another_user_s_profile;
   const char *Title[Usr_NUM_ME_OR_OTHER] =
     {
      [Usr_ME   ] = Txt_My_public_profile,
      [Usr_OTHER] = Txt_Another_user_s_profile,
     };

   /***** Show user's name inside form to go to user's public profile *****/
   /* Begin form */
   Frm_BeginForm (ActSeeOthPubPrf);
      Usr_PutParUsrCodEncrypted (UsrDat->EnUsrCod);

      /* Author's name */
      HTM_BUTTON_Submit_Begin (Title[Usr_ItsMe (UsrDat->UsrCod)],NULL,
			       "class=\"%s\"",Class);
	 HTM_Txt (UsrDat->FullName);
      HTM_BUTTON_End ();

   /* End form */
   Frm_EndForm ();
  }

/*****************************************************************************/
/*********************** Get and write a note content ************************/
/*****************************************************************************/

static void TmlNot_WriteContent (const struct TmlNot_Note *Not)
  {
   if (Not->Type == TmlNot_POST)	// It's a post
      TmlPst_GetAndWritePost (Not->Cod);
   else					// Not a post
      TmlNot_GetAndWriteNoPost (Not);
  }

/*****************************************************************************/
/***************** Get and write a note which is not a post ******************/
/*****************************************************************************/

static void TmlNot_GetAndWriteNoPost (const struct TmlNot_Note *Not)
  {
   struct Hie_Node Hie[Hie_NUM_LEVELS];
   struct For_Forums Forums;
   char ForumName[For_MAX_BYTES_FORUM_NAME + 1];
   char SummaryStr[Ntf_MAX_BYTES_SUMMARY + 1];

   /***** Reset forums *****/
   For_ResetForums (&Forums);

   /***** Get location in hierarchy *****/
   if (Not->Exists == Exi_EXISTS)
      TmlNot_GetLocationInHierarchy (Not,Hie,&Forums.Forum,ForumName);

   /***** Write note type *****/
   TmlNot_PutFormGoToAction (Not,&Forums);

   /***** Write location in hierarchy *****/
   if (Not->Exists == Exi_EXISTS)
      TmlNot_WriteLocationInHierarchy (Not,Hie,ForumName);

   /***** Get and write note summary *****/
   /* Get note summary */
   TmlNot_GetNoteSummary (Not,SummaryStr);

   /* Write note summary */
   HTM_DIV_Begin ("class=\"Tml_TXT Tml_TXT_%s\"",The_GetSuffix ());
      HTM_Txt (SummaryStr);
   HTM_DIV_End ();
  }

/*****************************************************************************/
/************************ Get location in hierarchy **************************/
/*****************************************************************************/

static void TmlNot_GetLocationInHierarchy (const struct TmlNot_Note *Not,
					   struct Hie_Node Hie[Hie_NUM_LEVELS],
                                           struct For_Forum *Forum,
                                           char ForumName[For_MAX_BYTES_FORUM_NAME + 1])
  {
   extern Err_SuccessOrError_t (*Hie_GetDataByCod[Hie_NUM_LEVELS]) (struct Hie_Node *Node);
   __attribute__((unused)) Err_SuccessOrError_t SuccessOrError;

   /***** Initialize location in hierarchy *****/
   Hie[Hie_CTY].HieCod =
   Hie[Hie_INS].HieCod =
   Hie[Hie_CTR].HieCod =
   Hie[Hie_DEG].HieCod =
   Hie[Hie_CRS].HieCod = -1L;

   /***** Get location in hierarchy *****/
   switch (Not->Type)
     {
      case TmlNot_INS_DOC_PUB_FILE:
      case TmlNot_INS_SHA_PUB_FILE:
	 /* Get institution data */
	 Hie[Hie_INS].HieCod = Not->HieCod;
	 SuccessOrError = Hie_GetDataByCod[Hie_INS] (&Hie[Hie_INS]);
	 break;
      case TmlNot_CTR_DOC_PUB_FILE:
      case TmlNot_CTR_SHA_PUB_FILE:
	 /* Get center data */
	 Hie[Hie_CTR].HieCod = Not->HieCod;
	 SuccessOrError = Hie_GetDataByCod[Hie_CTR] (&Hie[Hie_CTR]);
	 break;
      case TmlNot_DEG_DOC_PUB_FILE:
      case TmlNot_DEG_SHA_PUB_FILE:
	 /* Get degree data */
	 Hie[Hie_DEG].HieCod = Not->HieCod;
	 SuccessOrError = Hie_GetDataByCod[Hie_DEG] (&Hie[Hie_DEG]);
	 break;
      case TmlNot_CRS_DOC_PUB_FILE:
      case TmlNot_CRS_SHA_PUB_FILE:
      case TmlNot_CALL_FOR_EXAM:
      case TmlNot_NOTICE:
	 /* Get course data */
	 Hie[Hie_CRS].HieCod = Not->HieCod;
	 SuccessOrError = Hie_GetDataByCod[Hie_CRS] (&Hie[Hie_CRS]);
	 break;
      case TmlNot_FORUM_POST:
	 /* Get forum type of the post */
	 For_GetThreadForumTypeAndHieCodOfAPost (Not->Cod,Forum);

	 /* Set forum name in recipient's language */
	 For_SetForumName (Forum,ForumName,
			   Gbl.Prefs.Language,For_DONT_USE_HTML_ENTITIES);
	 break;
      default:
	 break;
     }
  }

/*****************************************************************************/
/*********************** Write location in hierarchy *************************/
/*****************************************************************************/

static void TmlNot_WriteLocationInHierarchy (const struct TmlNot_Note *Not,
					     struct Hie_Node Hie[Hie_NUM_LEVELS],
                                             const char ForumName[For_MAX_BYTES_FORUM_NAME + 1])
  {
   extern const char *Txt_HIERARCHY_SINGUL_Abc[Hie_NUM_LEVELS];
   extern const char *Txt_Forum;

   /***** Begin container *****/
   HTM_DIV_Begin ("class=\"Tml_LOC\"");

      /***** Write location *****/
      switch (Not->Type)
	{
	 case TmlNot_INS_DOC_PUB_FILE:
	 case TmlNot_INS_SHA_PUB_FILE:
	    /* Write location (institution) in hierarchy */
	    HTM_Txt (Txt_HIERARCHY_SINGUL_Abc[Hie_INS]); HTM_Colon ();
	    HTM_SP (); HTM_Txt (Hie[Hie_INS].ShrtName);
	    break;
	 case TmlNot_CTR_DOC_PUB_FILE:
	 case TmlNot_CTR_SHA_PUB_FILE:
	    /* Write location (center) in hierarchy */
	    HTM_Txt (Txt_HIERARCHY_SINGUL_Abc[Hie_CTR]); HTM_Colon ();
	    HTM_SP (); HTM_Txt (Hie[Hie_CTR].ShrtName);
	    break;
	 case TmlNot_DEG_DOC_PUB_FILE:
	 case TmlNot_DEG_SHA_PUB_FILE:
	    /* Write location (degree) in hierarchy */
	    HTM_Txt (Txt_HIERARCHY_SINGUL_Abc[Hie_DEG]); HTM_Colon ();
	    HTM_SP (); HTM_Txt (Hie[Hie_DEG].ShrtName);
	    break;
	 case TmlNot_CRS_DOC_PUB_FILE:
	 case TmlNot_CRS_SHA_PUB_FILE:
	 case TmlNot_CALL_FOR_EXAM:
	 case TmlNot_NOTICE:
	    /* Write location (course) in hierarchy */
	    HTM_Txt (Txt_HIERARCHY_SINGUL_Abc[Hie_CRS]); HTM_Colon ();
	    HTM_SP (); HTM_Txt (Hie[Hie_CRS].ShrtName);
	    break;
	 case TmlNot_FORUM_POST:
	    /* Write forum name */
	    HTM_Txt (Txt_Forum); HTM_Colon ();
	    HTM_SP (); HTM_Txt (ForumName);
	    break;
	 default:
	    break;
	}

   /***** End container *****/
   HTM_DIV_End ();
  }

/*****************************************************************************/
/************* Put form to go to an action depending on the note *************/
/*****************************************************************************/

static void TmlNot_PutFormGoToAction (const struct TmlNot_Note *Not,
                                      const struct For_Forums *Forums)
  {
   extern Act_Action_t For_ActionsSeeFor[For_NUM_TYPES_FORUM];
   extern const char *Txt_TIMELINE_NOTE[Tml_NOT_NUM_NOTE_TYPES];
   extern const char *Txt_not_available;
   char *Anchor = NULL;
   static Act_Action_t Tml_DefaultActions[Tml_NOT_NUM_NOTE_TYPES] =
     {
      [TmlNot_UNKNOWN          ] = ActUnk,
      /* Start tab */
      [TmlNot_POST             ] = ActUnk,	// action not used
      /* Institution tab */
      [TmlNot_INS_DOC_PUB_FILE ] = ActReqDatSeeDocIns,
      [TmlNot_INS_SHA_PUB_FILE ] = ActReqDatShaIns,
      /* Center tab */
      [TmlNot_CTR_DOC_PUB_FILE ] = ActReqDatSeeDocCtr,
      [TmlNot_CTR_SHA_PUB_FILE ] = ActReqDatShaCtr,
      /* Degree tab */
      [TmlNot_DEG_DOC_PUB_FILE ] = ActReqDatSeeDocDeg,
      [TmlNot_DEG_SHA_PUB_FILE ] = ActReqDatShaDeg,
      /* Course tab */
      [TmlNot_CRS_DOC_PUB_FILE ] = ActReqDatSeeDocCrs,
      [TmlNot_CRS_SHA_PUB_FILE ] = ActReqDatShaCrs,
      /* Assessment tab */
      [TmlNot_CALL_FOR_EXAM    ] = ActSeeOneCfe,
      /* Users tab */
      /* Messages tab */
      [TmlNot_NOTICE           ] = ActSeeOneNot,
      [TmlNot_FORUM_POST       ] = ActSeeFor,
      /* Analytics tab */
      /* Profile tab */
     };
   static const char *Tml_Icons[Tml_NOT_NUM_NOTE_TYPES] =
     {
      [TmlNot_UNKNOWN          ] = NULL,
      /* Start tab */
      [TmlNot_POST             ] = NULL,	// icon not used
      /* Institution tab */
      [TmlNot_INS_DOC_PUB_FILE ] = "file.svg",
      [TmlNot_INS_SHA_PUB_FILE ] = "file.svg",
      /* Center tab */
      [TmlNot_CTR_DOC_PUB_FILE ] = "file.svg",
      [TmlNot_CTR_SHA_PUB_FILE ] = "file.svg",
      /* Degree tab */
      [TmlNot_DEG_DOC_PUB_FILE ] = "file.svg",
      [TmlNot_DEG_SHA_PUB_FILE ] = "file.svg",
      /* Course tab */
      [TmlNot_CRS_DOC_PUB_FILE ] = "file.svg",
      [TmlNot_CRS_SHA_PUB_FILE ] = "file.svg",
      /* Assessment tab */
      [TmlNot_CALL_FOR_EXAM    ] = "bullhorn.svg",
      /* Users tab */
      /* Messages tab */
      [TmlNot_NOTICE           ] = "sticky-note.svg",
      [TmlNot_FORUM_POST       ] = "comments.svg",
      /* Analytics tab */
      /* Profile tab */
     };

   if (Not->Exists == Exi_EXISTS &&			// File/notice... pointed by this note is available...
       Frm_CheckIfInside () == Frm_OUTSIDE_FORM)	// ...and outside another form
     {
      /***** Begin container *****/
      HTM_DIV_Begin ("class=\"Tml_FORM\"");

	 /***** Begin form with parameters depending on the type of note *****/
	 switch (Not->Type)
	   {
	    case TmlNot_INS_DOC_PUB_FILE:
	    case TmlNot_INS_SHA_PUB_FILE:
	       Frm_BeginForm (Tml_DefaultActions[Not->Type]);
		  ParCod_PutPar (ParCod_Fil,Not->Cod);
		  if (Not->HieCod != Gbl.Hierarchy.Node[Hie_INS].HieCod)	// Not the current institution
		     ParCod_PutPar (ParCod_Ins,Not->HieCod);	// Go to another institution
	       break;
	    case TmlNot_CTR_DOC_PUB_FILE:
	    case TmlNot_CTR_SHA_PUB_FILE:
	       Frm_BeginForm (Tml_DefaultActions[Not->Type]);
		  ParCod_PutPar (ParCod_Fil,Not->Cod);
		  if (Not->HieCod != Gbl.Hierarchy.Node[Hie_CTR].HieCod)	// Not the current center
		     ParCod_PutPar (ParCod_Ctr,Not->HieCod);	// Go to another center
		  break;
	    case TmlNot_DEG_DOC_PUB_FILE:
	    case TmlNot_DEG_SHA_PUB_FILE:
	       Frm_BeginForm (Tml_DefaultActions[Not->Type]);
		  ParCod_PutPar (ParCod_Fil,Not->Cod);
		  if (Not->HieCod != Gbl.Hierarchy.Node[Hie_DEG].HieCod)	// Not the current degree
		     ParCod_PutPar (ParCod_Deg,Not->HieCod);	// Go to another degree
	       break;
	    case TmlNot_CRS_DOC_PUB_FILE:
	    case TmlNot_CRS_SHA_PUB_FILE:
	       Frm_BeginForm (Tml_DefaultActions[Not->Type]);
		  ParCod_PutPar (ParCod_Fil,Not->Cod);
		  if (Not->HieCod != Gbl.Hierarchy.Node[Hie_CRS].HieCod)	// Not the current course
		     ParCod_PutPar (ParCod_Crs,Not->HieCod);	// Go to another course
	       break;
	    case TmlNot_CALL_FOR_EXAM:
	       Frm_SetAnchorStr (Not->Cod,&Anchor);
	       Frm_BeginFormAnchor (Tml_DefaultActions[Not->Type],
				    Anchor);	// Locate on this specific exam
	       Frm_FreeAnchorStr (&Anchor);
		  ParCod_PutPar (ParCod_Exa,Not->Cod);
		  if (Not->HieCod != Gbl.Hierarchy.Node[Hie_CRS].HieCod)	// Not the current course
		     ParCod_PutPar (ParCod_Crs,Not->HieCod);	// Go to another course
	       break;
	    case TmlNot_POST:	// Not applicable
	       return;
	    case TmlNot_FORUM_POST:
	       Frm_BeginForm (For_ActionsSeeFor[Forums->Forum.Type]);
		  For_PutAllParsForum (1,	// Page of threads = first
				       1,	// Page of posts   = first
				       Forums->ForumSet,
				       Forums->ThreadsOrder,
				       Forums->Forum.HieCod,
				       Forums->Thread.Selected,
				       -1L);
		  if (Not->HieCod != Gbl.Hierarchy.Node[Hie_CRS].HieCod)	// Not the current course
		     ParCod_PutPar (ParCod_Crs,Not->HieCod);		// Go to another course
	       break;
	    case TmlNot_NOTICE:
	       Frm_SetAnchorStr (Not->Cod,&Anchor);
	       Frm_BeginFormAnchor (Tml_DefaultActions[Not->Type],Anchor);
	       Frm_FreeAnchorStr (&Anchor);
		  ParCod_PutPar (ParCod_Not,Not->Cod);
		  if (Not->HieCod != Gbl.Hierarchy.Node[Hie_CRS].HieCod)	// Not the current course
		     ParCod_PutPar (ParCod_Crs,Not->HieCod);		// Go to another course
	       break;
	    default:			// Not applicable
	       return;
	   }

	    /***** Icon and link to go to action *****/
	    /* Begin button */
	    HTM_BUTTON_Submit_Begin (Txt_TIMELINE_NOTE[Not->Type],NULL,
	                             "class=\"BT_LINK FORM_IN_%s ICO_HIGHLIGHT\"",
	                             The_GetSuffix ());

	       /* Icon and text */
	       Ico_PutIcon (Tml_Icons[Not->Type],Ico_BLACK,
	                    Txt_TIMELINE_NOTE[Not->Type],"CONTEXT_ICOx16");
	       HTM_NBSP ();
	       HTM_Txt (Txt_TIMELINE_NOTE[Not->Type]);

	    /* End button */
	    HTM_BUTTON_End ();

	 /***** End form *****/
	 Frm_EndForm ();

      /***** End container *****/
      HTM_DIV_End ();
     }
   else		// File/notice... pointed by this note is unavailable...
     {		// ...or inside another form
      /***** Do not put form *****/
      /* Begin container */
      HTM_DIV_Begin ("class=\"Tml_FORM_OFF\"");

         /* Text ("not available") */
	 HTM_Txt (Txt_TIMELINE_NOTE[Not->Type]);
	 if (Not->Exists == Exi_DOES_NOT_EXIST)
	   {
	    HTM_NBSP ();
	    HTM_ParTxtPar (Txt_not_available);
	   }

      /* End container */
      HTM_DIV_End ();
     }
  }

/*****************************************************************************/
/********************** Get note summary and content *************************/
/*****************************************************************************/

void TmlNot_GetNoteSummary (const struct TmlNot_Note *Not,
                            char SummaryStr[Ntf_MAX_BYTES_SUMMARY + 1])
  {
   SummaryStr[0] = '\0';

   switch (Not->Type)
     {
      case TmlNot_UNKNOWN:
          break;
      case TmlNot_INS_DOC_PUB_FILE:
      case TmlNot_INS_SHA_PUB_FILE:
      case TmlNot_CTR_DOC_PUB_FILE:
      case TmlNot_CTR_SHA_PUB_FILE:
      case TmlNot_DEG_DOC_PUB_FILE:
      case TmlNot_DEG_SHA_PUB_FILE:
      case TmlNot_CRS_DOC_PUB_FILE:
      case TmlNot_CRS_SHA_PUB_FILE:
	 Brw_GetSummaryAndContentOfFile (SummaryStr,NULL,Not->Cod,
					 Ntf_DONT_GET_CONTENT);
         break;
      case TmlNot_CALL_FOR_EXAM:
         Cfe_GetSummaryAndContentCallForExam (SummaryStr,NULL,Not->Cod,
					      Ntf_DONT_GET_CONTENT);
         break;
      case TmlNot_POST:
	 // Not applicable
         break;
      case TmlNot_FORUM_POST:
         For_GetSummaryAndContentForumPst (SummaryStr,NULL,Not->Cod,
					   Ntf_DONT_GET_CONTENT);
         break;
      case TmlNot_NOTICE:
         Not_GetSummaryAndContentNotice (SummaryStr,NULL,Not->Cod,
					 Ntf_DONT_GET_CONTENT);
         break;
     }
  }

/*****************************************************************************/
/************************ Write bottom part of a note ************************/
/*****************************************************************************/

static void TmlNot_WriteButtonsAndComms (const struct Tml_Timeline *Timeline,
                                         const struct TmlNot_Note *Not,
                                         const struct Usr_Data *UsrDat)	// Author
  {
   char IdNewComm[Frm_MAX_BYTES_ID + 1];

   /***** Create unique id for new comment *****/
   Frm_SetUniqueId (IdNewComm);

   /***** Left: button to add a comment *****/
   TmlNot_WriteButtonToAddAComm (Timeline,Not,IdNewComm);

   /***** Right: write favs, shared and remove buttons, and comments *****/
   TmlNot_WriteFavShaRemAndComms (Timeline,Not,UsrDat);

   /***** Put hidden form to write a new comment *****/
   if (Gbl.Usrs.Me.Hierarchy[Hie_CRS].Num)	// I can post only if I am enroled in any course
      TmlCom_PutPhotoAndFormToWriteNewComm (Timeline,Not->NotCod,IdNewComm);
  }

/*****************************************************************************/
/********************** Write button to add a comment ************************/
/*****************************************************************************/

static void TmlNot_WriteButtonToAddAComm (const struct Tml_Timeline *Timeline,
                                          const struct TmlNot_Note *Not,
                                          const char IdNewComm[Frm_MAX_BYTES_ID + 1])
  {
   /***** Begin container *****/
   HTM_DIV_Begin ("class=\"Tml_BOTTOM_LEFT\"");

      /***** Button to add a comment *****/
      if (Not->Exists == Exi_EXISTS &&	// Only available notes can be commented
	  Gbl.Usrs.Me.Hierarchy[Hie_CRS].Num)	// I can post only if I am enroled in any course
	 TmlCom_PutIconToToggleComm (Timeline,IdNewComm);
      else
         // I can not comment
	 TmlCom_PutIconCommDisabled ();

   /***** End container *****/
   HTM_DIV_End ();
  }

/*****************************************************************************/
/******* Write favs, shared and remove buttons, and comments of a note *******/
/*****************************************************************************/

static void TmlNot_WriteFavShaRemAndComms (const struct Tml_Timeline *Timeline,
					   const struct TmlNot_Note *Not,
					   const struct Usr_Data *UsrDat)	// Author
  {
   /***** Begin container *****/
   HTM_DIV_Begin ("class=\"Tml_BOTTOM_RIGHT Tml_RIGHT_WIDTH\"");

      /***** Write favs, shared and remove buttons int the foot of a note *****/
      TmlNot_WriteFavShaRem (Timeline,Not,UsrDat);

      /***** Comments *****/
      TmlCom_WriteCommsInNote (Timeline,Not);

   /***** End container *****/
   HTM_DIV_End ();
  }

/*****************************************************************************/
/******* Write favs, shared and remove buttons in the foot of a note *********/
/*****************************************************************************/

static void TmlNot_WriteFavShaRem (const struct Tml_Timeline *Timeline,
                                   const struct TmlNot_Note *Not,
                                   const struct Usr_Data *UsrDat)	// Author
  {
   static unsigned NumDiv = 0;	// Used to create unique div id for fav and shared
   const char *UniqueNameEncrypted = Cry_GetUniqueNameEncrypted ();

   NumDiv++;

   /***** Begin foot container *****/
   HTM_DIV_Begin ("class=\"Tml_FOOT Tml_RIGHT_WIDTH\"");

      /***** Foot column 1: fav zone *****/
      HTM_DIV_Begin ("id=\"fav_not_%s_%u\""
	             " class=\"Tml_FAV_NOT Tml_FAV_NOT_WIDTH\"",
		     UniqueNameEncrypted,NumDiv);
	 TmlUsr_PutIconFavSha (TmlUsr_FAV_UNF_NOTE,
	                       Not->NotCod,Not->UsrCod,Not->NumFavs,
			       TmlUsr_SHOW_FEW_USRS);
      HTM_DIV_End ();

      /***** Foot column 2: share zone *****/
      HTM_DIV_Begin ("id=\"sha_not_%s_%u\""
	             " class=\"Tml_SHA_NOT Tml_SHA_NOT_WIDTH\"",
		     UniqueNameEncrypted,NumDiv);
	 TmlUsr_PutIconFavSha (TmlUsr_SHA_UNS_NOTE,
	                       Not->NotCod,Not->UsrCod,Not->NumShared,
	                       TmlUsr_SHOW_FEW_USRS);
      HTM_DIV_End ();

      /***** Foot column 3: icon to remove this note *****/
      HTM_DIV_Begin ("class=\"Tml_REM\"");
	 if (Usr_ItsMe (UsrDat->UsrCod) == Usr_ME)	// I am the author
	    TmlNot_PutFormToRemoveNote (Timeline,Not->NotCod);
      HTM_DIV_End ();

   /***** End foot container *****/
   HTM_DIV_End ();
  }

/*****************************************************************************/
/**************************** Form to remove note ****************************/
/*****************************************************************************/

static void TmlNot_PutFormToRemoveNote (const struct Tml_Timeline *Timeline,
                                        long NotCod)
  {
   extern Act_Action_t TmlFrm_ActionUsr[TmlFrm_NUM_ACTIONS];
   extern Act_Action_t TmlFrm_ActionGbl[TmlFrm_NUM_ACTIONS];

   /***** Form to remove publication *****/
   /* Begin form */
   TmlFrm_BeginForm (Timeline,TmlFrm_REQ_REM_NOTE);
      ParCod_PutPar (ParCod_Not,NotCod);

      /* Icon to remove */
      Ico_PutIconLink ("trash.svg",Ico_RED,
                       Gbl.Usrs.Other.UsrDat.UsrCod > 0 ? TmlFrm_ActionUsr[TmlFrm_REQ_REM_NOTE] :
                		                          TmlFrm_ActionGbl[TmlFrm_REQ_REM_NOTE]);

   /* End form */
   TmlFrm_EndForm ();
  }

/*****************************************************************************/
/***************** Store and publish a note into database ********************/
/*****************************************************************************/

void TmlNot_StoreAndPublishNote (TmlNot_Type_t NoteType,long Cod)
  {
   struct TmlPub_Publication Pub;

   TmlNot_StoreAndPublishNoteInternal (NoteType,Cod,&Pub);
  }

void TmlNot_StoreAndPublishNoteInternal (TmlNot_Type_t NoteType,long Cod,
                                         struct TmlPub_Publication *Pub)
  {
   static Hie_Level_t HieLvl[Tml_NOT_NUM_NOTE_TYPES] =
     {
      [TmlNot_INS_DOC_PUB_FILE] = Hie_INS,
      [TmlNot_INS_SHA_PUB_FILE] = Hie_INS,

      [TmlNot_CTR_DOC_PUB_FILE] = Hie_CTR,
      [TmlNot_CTR_SHA_PUB_FILE] = Hie_CTR,

      [TmlNot_DEG_DOC_PUB_FILE] = Hie_DEG,
      [TmlNot_DEG_SHA_PUB_FILE] = Hie_DEG,

      [TmlNot_CRS_DOC_PUB_FILE] = Hie_CRS,
      [TmlNot_CRS_SHA_PUB_FILE] = Hie_CRS,
      [TmlNot_CALL_FOR_EXAM   ] = Hie_CRS,
      [TmlNot_NOTICE          ] = Hie_CRS,
     };
   long HieCod;	// Hierarchy code (institution/center/degree/course)

   if (HieLvl[NoteType])
      HieCod = Gbl.Hierarchy.Node[HieLvl[NoteType]].HieCod;
   else
      HieCod = -1L;

   /***** Publish note in timeline *****/
   Pub->PublisherCod = Gbl.Usrs.Me.UsrDat.UsrCod;
   Pub->NotCod       = Tml_DB_CreateNewNote (NoteType,Cod,Pub->PublisherCod,HieCod);
   Pub->Type         = TmlPub_ORIGINAL_NOTE;
   TmlPub_PublishPubInTimeline (Pub);
  }

/*****************************************************************************/
/****************** Mark notes of one file as unavailable ********************/
/*****************************************************************************/

void TmlNot_MarkNoteOneFileAsUnavailable (const char *Path)
  {
   extern const Brw_FileBrowser_t Brw_DB_FileBrowserForDB_files[Brw_NUM_TYPES_FILE_BROWSER];
   Brw_FileBrowser_t FileBrowser = Brw_DB_FileBrowserForDB_files[Gbl.FileBrowser.Type];
   long FilCod;

   if (TmlNot_NoteType[FileBrowser])
      /***** Get file code *****/
      if ((FilCod = Brw_DB_GetFilCodByPath (Path,
                                            Brw_ONLY_PUBLIC_FILES)) > 0)	// Only public files
	 /***** Mark possible note as unavailable *****/
	 Tml_DB_MarkNoteAsUnavailable (TmlNot_NoteType[FileBrowser],FilCod);
  }

/*****************************************************************************/
/***** Mark possible notes involving children of a folder as unavailable *****/
/*****************************************************************************/

void TmlNot_MarkNotesChildrenOfFolderAsUnavailable (const char *Path)
  {
   extern const Brw_FileBrowser_t Brw_DB_FileBrowserForDB_files[Brw_NUM_TYPES_FILE_BROWSER];
   Brw_FileBrowser_t FileBrowser = Brw_DB_FileBrowserForDB_files[Gbl.FileBrowser.Type];

   if (TmlNot_NoteType[FileBrowser])
      Tml_DB_MarkNotesChildrenOfFolderAsUnavailable (TmlNot_NoteType[FileBrowser],FileBrowser,
						     Brw_GetCodForFileBrowser (Gbl.FileBrowser.Type),
						     Path);
  }

/*****************************************************************************/
/*********************** Request the removal of a note ***********************/
/*****************************************************************************/

void TmlNot_ReqRemNoteUsr (void)
  {
   struct Tml_Timeline Timeline;
   __attribute__((unused)) Exi_Exist_t UsrExists;
   __attribute__((unused)) Err_SuccessOrError_t ProfileShown;

   /***** Reset timeline context *****/
   Tml_ResetTimeline (&Timeline);

   /***** Get user whom profile is displayed *****/
   UsrExists = Usr_GetParOtherUsrCodEncryptedAndGetUsrData ();

   /***** Show user's profile *****/
   ProfileShown = Prf_ShowUsrProfile (&Gbl.Usrs.Other.UsrDat);

   /***** Begin section *****/
   HTM_SECTION_Begin (Tml_TIMELINE_SECTION_ID);

      /***** Request the removal of note *****/
      TmlNot_ReqRemNote (&Timeline);

      /***** Write timeline again (user) *****/
      Tml_ShowTimelineUsr (&Timeline);

   /***** End section *****/
   HTM_SECTION_End ();
  }

void TmlNot_ReqRemNoteGbl (void)
  {
   struct Tml_Timeline Timeline;

   /***** Initialize timeline *****/
   Tml_InitTimelineGbl (&Timeline);

   /***** Request the removal of note *****/
   TmlNot_ReqRemNote (&Timeline);

   /***** Write timeline again (global) *****/
   Tml_ShowNoteAndTimelineGbl (&Timeline);
  }

static void TmlNot_ReqRemNote (struct Tml_Timeline *Timeline)
  {
   extern const char *Txt_Do_you_really_want_to_remove_the_following_post;
   struct TmlNot_Note Not;

   /***** Get data of note *****/
   Not.NotCod = ParCod_GetAndCheckPar (ParCod_Not);
   TmlNot_GetNoteDataByCod (&Not);

   /***** Do some checks *****/
   if (TmlUsr_CheckIfICanRemove (Not.NotCod,Not.UsrCod) == Usr_CAN_NOT)
      return;

   /***** Show question and button to remove note *****/
   /* Begin alert */
   TmlFrm_BeginAlertRemove (Txt_Do_you_really_want_to_remove_the_following_post);

   /* Show note */
   Box_BoxBegin (NULL,NULL,NULL,NULL,Box_NOT_CLOSABLE);
      HTM_UL_Begin ("class=\"Tml_LIST\"");
	 HTM_LI_Begin ("class=\"Tml_WIDTH\"");
	    TmlNot_CheckAndWriteNoteWithTopMsg (Timeline,&Not,
						Tml_TOP_MESSAGE_NONE,
						-1L);
	 HTM_LI_End ();
      HTM_UL_End ();
   Box_BoxEnd ();
   HTM_BR ();

   /* End alert */
   Timeline->NotCod = Not.NotCod;	// Note to be removed
   TmlFrm_EndAlertRemove (Timeline,TmlFrm_REM_NOTE,TmlNot_PutParsRemoveNote);
  }

/*****************************************************************************/
/********************* Put parameters to remove a note ***********************/
/*****************************************************************************/

static void TmlNot_PutParsRemoveNote (void *Timeline)
  {
   if (Timeline)
     {
      if (Gbl.Usrs.Other.UsrDat.UsrCod > 0)	// User's timeline
	 Usr_PutParOtherUsrCodEncrypted (Gbl.Usrs.Other.UsrDat.EnUsrCod);
      else					// Global timeline
	 Usr_PutParWho (((struct Tml_Timeline *) Timeline)->Who);
      ParCod_PutPar (ParCod_Not,((struct Tml_Timeline *) Timeline)->NotCod);
     }
  }

/*****************************************************************************/
/******************************* Remove a note *******************************/
/*****************************************************************************/

void TmlNot_RemoveNoteUsr (void)
  {
   struct Tml_Timeline Timeline;
   __attribute__((unused)) Exi_Exist_t UsrExists;
   __attribute__((unused)) Err_SuccessOrError_t ProfileShown;

   /***** Reset timeline context *****/
   Tml_ResetTimeline (&Timeline);

   /***** Get user whom profile is displayed *****/
   UsrExists = Usr_GetParOtherUsrCodEncryptedAndGetUsrData ();

   /***** Show user's profile *****/
   ProfileShown = Prf_ShowUsrProfile (&Gbl.Usrs.Other.UsrDat);

   /***** Begin section *****/
   HTM_SECTION_Begin (Tml_TIMELINE_SECTION_ID);

      /***** Remove a note *****/
      TmlNot_RemoveNote ();

      /***** Write updated timeline after removing (user) *****/
      Tml_ShowTimelineUsr (&Timeline);

   /***** End section *****/
   HTM_SECTION_End ();
  }

void TmlNot_RemoveNoteGbl (void)
  {
   struct Tml_Timeline Timeline;

   /***** Initialize timeline *****/
   Tml_InitTimelineGbl (&Timeline);

   /***** Remove a note *****/
   TmlNot_RemoveNote ();

   /***** Write updated timeline after removing (global) *****/
   Tml_ShowNoteAndTimelineGbl (&Timeline);
  }

static void TmlNot_RemoveNote (void)
  {
   extern const char *Txt_The_post_no_longer_exists;
   extern const char *Txt_TIMELINE_Post_removed;
   struct TmlNot_Note Not;

   /***** Get data of note *****/
   Not.NotCod = ParCod_GetAndCheckPar (ParCod_Not);
   TmlNot_GetNoteDataByCod (&Not);

   /***** Trivial check 1: note code should be > 0 *****/
   if (Not.NotCod <= 0)
     {
      Ale_ShowAlert (Ale_WARNING,Txt_The_post_no_longer_exists);
      return;
     }

   /***** Trivial check 2: Am I the author of this note? *****/
   if (Usr_ItsMe (Not.UsrCod) == Usr_OTHER)
     {
      Err_NoPermission ();
      return;
     }

   /***** Delete note from database *****/
   TmlNot_RemoveNoteMediaAndDBEntries (&Not);

   /***** Reset note *****/
   TmlNot_ResetNote (&Not);

   /***** Message of success *****/
   Ale_ShowAlert (Ale_SUCCESS,Txt_TIMELINE_Post_removed);
  }

/*****************************************************************************/
/*********************** Remove a note from database *************************/
/*****************************************************************************/

static void TmlNot_RemoveNoteMediaAndDBEntries (struct TmlNot_Note *Not)
  {
   MYSQL_RES *mysql_res;
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
      PubCod = DB_GetNextCode (mysql_res);

      /* Remove media associated to comment
	 and delete comment from database */
      TmlCom_RemoveCommMediaAndDBEntries (PubCod);
     }

   /* Free structure that stores the query result */
   DB_FreeMySQLResult (&mysql_res);

   /***** Remove media associated to post *****/
   if (Not->Type == TmlNot_POST)
      if ((MedCod = Tml_DB_GetMedCodFromPost (Not->Cod)) > 0)
	 Med_RemoveMedia (MedCod);

   /***** Mark possible notifications on the publications
          of this note as removed *****/
   if ((PubCod = Tml_DB_GetPubCodOfOriginalNote (Not->NotCod)) > 0)
     {
      Ntf_DB_MarkNotifAsRemoved (Ntf_EVENT_TML_FAV    ,PubCod);
      Ntf_DB_MarkNotifAsRemoved (Ntf_EVENT_TML_SHARE  ,PubCod);
      Ntf_DB_MarkNotifAsRemoved (Ntf_EVENT_TML_MENTION,PubCod);
     }

   /***** Remove favs for this note *****/
   Tml_DB_RemoveNoteFavs (Not->NotCod);

   /***** Remove all publications of this note *****/
   Tml_DB_RemoveNotePubs (Not->NotCod);

   /***** Remove note *****/
   Tml_DB_RemoveNote (Not->NotCod);

   if (Not->Type == TmlNot_POST)
      /***** Remove post *****/
      Tml_DB_RemovePost (Not->Cod);
  }

/*****************************************************************************/
/************************ Get data of note from row **************************/
/*****************************************************************************/

static void TmlNot_GetNoteDataFromRow (MYSQL_RES *mysql_res,
                                       struct TmlNot_Note *Not)
  {
   MYSQL_ROW row;

   /***** Get next row from result *****/
   row = mysql_fetch_row (mysql_res);
   /*
   row[0]: NotCod
   row[1]: NoteType
   row[2]: Cod
   row[3]: UsrCod
   row[4]: HieCod
   row[5]: Unavailable
   row[6]: UNIX_TIMESTAMP(TimeNote)
   */
   /***** Get code (row[0]) *****/
   Not->NotCod      = Str_ConvertStrCodToLongCod (row[0]);

   /***** Get note type (row[1]) *****/
   Not->Type        = TmlNot_GetNoteTypeFromStr (row[1]);

   /***** Get file/post... code (row[2]),
              (from) user code (row[3])
          and hierarchy code (row[4]) *****/
   Not->Cod         = Str_ConvertStrCodToLongCod (row[2]);
   Not->UsrCod      = Str_ConvertStrCodToLongCod (row[3]);
   Not->HieCod      = Str_ConvertStrCodToLongCod (row[4]);

   /***** File/post... unavailable (row[5]) *****/
   Not->Exists = row[5][0] == 'Y' ? Exi_DOES_NOT_EXIST :
				    Exi_EXISTS;

   /***** Get time of the note (row[6]) *****/
   Not->DateTimeUTC = Dat_GetUNIXTimeFromStr (row[6]);

   /***** Get number of times this note has been shared and favourited *****/
   Not->NumShared   = Tml_DB_GetNumSharers (Not->NotCod,Not->UsrCod);
   Not->NumFavs     = Tml_DB_GetNumFavers (TmlUsr_FAV_UNF_NOTE,
                                           Not->NotCod,Not->UsrCod);
  }

/*****************************************************************************/
/********* Get note type from string number coming from database *************/
/*****************************************************************************/

static TmlNot_Type_t TmlNot_GetNoteTypeFromStr (const char *Str)
  {
   unsigned UnsignedNum;

   if (sscanf (Str,"%u",&UnsignedNum) == 1)
      if (UnsignedNum < Tml_NOT_NUM_NOTE_TYPES)
         return (TmlNot_Type_t) UnsignedNum;

   return TmlNot_UNKNOWN;
  }

/*****************************************************************************/
/*************************** Reset fields of note ****************************/
/*****************************************************************************/

static void TmlNot_ResetNote (struct TmlNot_Note *Not)
  {
   Not->NotCod      = -1L;
   Not->Type        = TmlNot_UNKNOWN;
   Not->UsrCod      = -1L;
   Not->HieCod      = -1L;
   Not->Cod         = -1L;
   Not->Exists      = Exi_EXISTS;
   Not->DateTimeUTC = (time_t) 0;
   Not->NumShared   = 0;
  }

/*****************************************************************************/
/******************** Get data of note using its code ************************/
/*****************************************************************************/

void TmlNot_GetNoteDataByCod (struct TmlNot_Note *Not)
  {
   MYSQL_RES *mysql_res;

   /***** Trivial check: note code should be > 0 *****/
   if (Not->NotCod <= 0)
     {
      /* Reset fields of note */
      TmlNot_ResetNote (Not);
      return;
     }

   /***** Get data of note from database *****/
   switch (Tml_DB_GetNoteDataByCod (Not->NotCod,&mysql_res))
     {
      case Exi_EXISTS:
	 TmlNot_GetNoteDataFromRow (mysql_res,Not);
	 break;
      case Exi_DOES_NOT_EXIST:
      default:
	 /* Reset fields of note */
	 TmlNot_ResetNote (Not);
	 break;
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }
