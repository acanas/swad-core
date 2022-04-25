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
#include <stdlib.h>		// For free

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

static const TmlNot_Type_t TmlNot_NoteType[Brw_NUM_TYPES_FILE_BROWSER] =
  {
   [Brw_ADMI_DOC_INS] = Tml_NOTE_INS_DOC_PUB_FILE,
   [Brw_ADMI_SHR_INS] = Tml_NOTE_INS_SHA_PUB_FILE,
   [Brw_ADMI_DOC_CTR] = Tml_NOTE_CTR_DOC_PUB_FILE,
   [Brw_ADMI_SHR_CTR] = Tml_NOTE_CTR_SHA_PUB_FILE,
   [Brw_ADMI_DOC_DEG] = Tml_NOTE_DEG_DOC_PUB_FILE,
   [Brw_ADMI_SHR_DEG] = Tml_NOTE_DEG_SHA_PUB_FILE,
   [Brw_ADMI_DOC_CRS] = Tml_NOTE_CRS_DOC_PUB_FILE,
   [Brw_ADMI_SHR_CRS] = Tml_NOTE_CRS_SHA_PUB_FILE,
  };

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void TmlNot_WriteTopMessage (Tml_TopMessage_t TopMessage,long PublisherCod);
static void TmlNot_WriteNote (const struct Tml_Timeline *Timeline,
                              const struct TmlNot_Note *Not);
static void TmlNot_WriteAuthorTimeAndContent (const struct TmlNot_Note *Not,
                                              const struct UsrData *UsrDat);

static void TmlNot_WriteContent (const struct TmlNot_Note *Not);
static void TmlNot_GetAndWriteNoPost (const struct TmlNot_Note *Not);
static void TmlNot_GetLocationInHierarchy (const struct TmlNot_Note *Not,
                                           struct Hie_Hierarchy *Hie,
                                           struct For_Forum *Forum,
                                           char ForumName[For_MAX_BYTES_FORUM_NAME + 1]);
static void TmlNot_WriteLocationInHierarchy (const struct TmlNot_Note *Not,
	                                     const struct Hie_Hierarchy *Hie,
                                             const char ForumName[For_MAX_BYTES_FORUM_NAME + 1]);

static void TmlNot_PutFormGoToAction (const struct TmlNot_Note *Not,
                                      const struct For_Forums *Forums);

static void TmlNot_WriteButtonsAndComms (const struct Tml_Timeline *Timeline,
                                         const struct TmlNot_Note *Not,
                                         const struct UsrData *UsrDat);
static void TmlNot_WriteButtonToAddAComm (const struct TmlNot_Note *Not,
                                          const char IdNewComm[Frm_MAX_BYTES_ID + 1]);
static void TmlNot_WriteFavShaRemAndComms (const struct Tml_Timeline *Timeline,
					   const struct TmlNot_Note *Not,
					   const struct UsrData *UsrDat);
static void TmlNot_WriteFavShaRem (const struct Tml_Timeline *Timeline,
                                   const struct TmlNot_Note *Not,
                                   const struct UsrData *UsrDat);

static void TmlNot_PutFormToRemoveNote (const struct Tml_Timeline *Timeline,
                                        long NotCod);

static void TmlNot_RequestRemovalNote (struct Tml_Timeline *Timeline);
static void TmlNot_PutParamsRemoveNote (void *Timeline);
static void TmlNot_RemoveNote (void);
static void TmlNot_RemoveNoteMediaAndDBEntries (struct TmlNot_Note *Not);

static void TmlNot_GetDataOfNoteFromRow (MYSQL_ROW row,struct TmlNot_Note *Not);

static TmlNot_Type_t TmlNot_GetNoteTypeFromStr (const char *Str);

static void TmlNot_ResetNote (struct TmlNot_Note *Not);

/*****************************************************************************/
/****************** Show highlighted note above timeline *********************/
/*****************************************************************************/

void TmlNot_ShowHighlightedNote (struct Tml_Timeline *Timeline,
                                  struct TmlNot_Note *Not)
  {
   struct UsrData PublisherDat;
   Ntf_NotifyEvent_t NotifyEvent;
   static const Tml_TopMessage_t TopMessages[Ntf_NUM_NOTIFY_EVENTS] =
     {
      [Ntf_EVENT_TML_COMMENT] = Tml_TOP_MESSAGE_COMMENTED,
      [Ntf_EVENT_TML_FAV    ] = Tml_TOP_MESSAGE_FAVED,
      [Ntf_EVENT_TML_SHARE  ] = Tml_TOP_MESSAGE_SHARED,
      [Ntf_EVENT_TML_MENTION] = Tml_TOP_MESSAGE_MENTIONED,
     };

   /***** Get other parameters *****/
   /* Get the publisher who did the action
      (publishing, commenting, faving, sharing, mentioning) */
   Usr_GetParamOtherUsrCodEncrypted (&PublisherDat);

   /* Get what he/she did */
   NotifyEvent = Ntf_GetParamNotifyEvent ();

   /***** Get data of the note *****/
   TmlNot_GetDataOfNoteByCod (Not);

   /***** Show the note highlighted *****/
   /* Begin box */
   Box_BoxBegin (NULL,NULL,
		 NULL,NULL,
		 NULL,Box_CLOSABLE);

      /* Begin container */
      HTM_DIV_Begin ("class=\"Tml_WIDTH Tml_NEW_PUB\"");

         /* Check and write note with top message */
	 TmlNot_CheckAndWriteNoteWithTopMsg (Timeline,Not,
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
       Not->Type   == Tml_NOTE_UNKNOWN)
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
   struct UsrData PublisherDat;

   /***** Initialize structure with user's data *****/
   Usr_UsrDataConstructor (&PublisherDat);

   /***** Get user's data *****/
   PublisherDat.UsrCod = PublisherCod;
   if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&PublisherDat,	// Really we only need EncryptedUsrCod and FullName
                                                Usr_DONT_GET_PREFS,
                                                Usr_DONT_GET_ROLE_IN_CURRENT_CRS))
     {
      /***** Begin container *****/
      HTM_DIV_Begin ("class=\"Tml_TOP_CONT Tml_TOP_PUBLISHER Tml_WIDTH\"");

	 /***** Show publisher's name inside form to go to user's public profile *****/
         TmlNot_WriteAuthorName (&PublisherDat,"BT_LINK");

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

static void TmlNot_WriteNote (const struct Tml_Timeline *Timeline,
                              const struct TmlNot_Note *Not)
  {
   struct UsrData UsrDat;	// Author of the note

   /***** Get author data *****/
   Usr_UsrDataConstructor (&UsrDat);
   UsrDat.UsrCod = Not->UsrCod;
   Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat,
                                            Usr_DONT_GET_PREFS,
                                            Usr_DONT_GET_ROLE_IN_CURRENT_CRS);

   /***** Left top: author's photo *****/
   TmlNot_ShowAuthorPhoto (&UsrDat,true);	// Use unique id

   /***** Right top: author's name, time, and content *****/
   TmlNot_WriteAuthorTimeAndContent (Not,&UsrDat);

   /***** Bottom: buttons and comments *****/
   TmlNot_WriteButtonsAndComms (Timeline,Not,&UsrDat);

   /***** Free memory used for author's data *****/
   Usr_UsrDataDestructor (&UsrDat);
  }

/*****************************************************************************/
/*********************** Show photo of author of a note **********************/
/*****************************************************************************/

void TmlNot_ShowAuthorPhoto (struct UsrData *UsrDat,bool FormUnique)
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
                                 ClassPhoto[Gbl.Prefs.PhotoShape],Pho_ZOOM,
                                 FormUnique);

   /***** End container *****/
   HTM_DIV_End ();
  }

/*****************************************************************************/
/**** Write top right part of a note: author's name, time and note content ***/
/*****************************************************************************/

static void TmlNot_WriteAuthorTimeAndContent (const struct TmlNot_Note *Not,
                                              const struct UsrData *UsrDat)
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

void TmlNot_WriteAuthorName (const struct UsrData *UsrDat,
                             const char *Class)
  {
   extern const char *Txt_My_public_profile;
   extern const char *Txt_Another_user_s_profile;

   /***** Show user's name inside form to go to user's public profile *****/
   /* Begin form */
   Frm_BeginFormUnique (ActSeeOthPubPrf);
      Usr_PutParamUsrCodEncrypted (UsrDat->EnUsrCod);

      /* Author's name */
      HTM_BUTTON_Submit_Begin (Usr_ItsMe (UsrDat->UsrCod) ? Txt_My_public_profile :
							    Txt_Another_user_s_profile,
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
   if (Not->Type == Tml_NOTE_POST)	// It's a post
      TmlPst_GetAndWritePost (Not->Cod);
   else					// Not a post
      TmlNot_GetAndWriteNoPost (Not);
  }

/*****************************************************************************/
/***************** Get and write a note which is not a post ******************/
/*****************************************************************************/

static void TmlNot_GetAndWriteNoPost (const struct TmlNot_Note *Not)
  {
   struct Hie_Hierarchy Hie;
   struct For_Forums Forums;
   char ForumName[For_MAX_BYTES_FORUM_NAME + 1];
   char SummaryStr[Ntf_MAX_BYTES_SUMMARY + 1];

   /***** Reset forums *****/
   For_ResetForums (&Forums);

   /***** Get location in hierarchy *****/
   if (!Not->Unavailable)
      TmlNot_GetLocationInHierarchy (Not,&Hie,&Forums.Forum,ForumName);

   /***** Write note type *****/
   TmlNot_PutFormGoToAction (Not,&Forums);

   /***** Write location in hierarchy *****/
   if (!Not->Unavailable)
      TmlNot_WriteLocationInHierarchy (Not,&Hie,ForumName);

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
   switch (Not->Type)
     {
      case Tml_NOTE_INS_DOC_PUB_FILE:
      case Tml_NOTE_INS_SHA_PUB_FILE:
	 /* Get institution data */
	 Hie->Ins.InsCod = Not->HieCod;
	 Ins_GetDataOfInstitByCod (&Hie->Ins);
	 break;
      case Tml_NOTE_CTR_DOC_PUB_FILE:
      case Tml_NOTE_CTR_SHA_PUB_FILE:
	 /* Get center data */
	 Hie->Ctr.CtrCod = Not->HieCod;
	 Ctr_GetDataOfCenterByCod (&Hie->Ctr);
	 break;
      case Tml_NOTE_DEG_DOC_PUB_FILE:
      case Tml_NOTE_DEG_SHA_PUB_FILE:
	 /* Get degree data */
	 Hie->Deg.DegCod = Not->HieCod;
	 Deg_GetDataOfDegreeByCod (&Hie->Deg);
	 break;
      case Tml_NOTE_CRS_DOC_PUB_FILE:
      case Tml_NOTE_CRS_SHA_PUB_FILE:
      case Tml_NOTE_CALL_FOR_EXAM:
      case Tml_NOTE_NOTICE:
	 /* Get course data */
	 Hie->Crs.CrsCod = Not->HieCod;
	 Crs_GetDataOfCourseByCod (&Hie->Crs);
	 break;
      case Tml_NOTE_FORUM_POST:
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

static void TmlNot_WriteLocationInHierarchy (const struct TmlNot_Note *Not,
	                                     const struct Hie_Hierarchy *Hie,
                                             const char ForumName[For_MAX_BYTES_FORUM_NAME + 1])
  {
   extern const char *Txt_Institution;
   extern const char *Txt_Center;
   extern const char *Txt_Degree;
   extern const char *Txt_Course;
   extern const char *Txt_Forum;

   /***** Begin container *****/
   HTM_DIV_Begin ("class=\"Tml_LOC\"");

      /***** Write location *****/
      switch (Not->Type)
	{
	 case Tml_NOTE_INS_DOC_PUB_FILE:
	 case Tml_NOTE_INS_SHA_PUB_FILE:
	    /* Write location (institution) in hierarchy */
	    HTM_TxtF ("%s:&nbsp;%s",
	              Txt_Institution,
	              Hie->Ins.ShrtName);
	    break;
	 case Tml_NOTE_CTR_DOC_PUB_FILE:
	 case Tml_NOTE_CTR_SHA_PUB_FILE:
	    /* Write location (center) in hierarchy */
	    HTM_TxtF ("%s:&nbsp;%s",
	              Txt_Center,
	              Hie->Ctr.ShrtName);
	    break;
	 case Tml_NOTE_DEG_DOC_PUB_FILE:
	 case Tml_NOTE_DEG_SHA_PUB_FILE:
	    /* Write location (degree) in hierarchy */
	    HTM_TxtF ("%s:&nbsp;%s",
	              Txt_Degree,
	              Hie->Deg.ShrtName);
	    break;
	 case Tml_NOTE_CRS_DOC_PUB_FILE:
	 case Tml_NOTE_CRS_SHA_PUB_FILE:
	 case Tml_NOTE_CALL_FOR_EXAM:
	 case Tml_NOTE_NOTICE:
	    /* Write location (course) in hierarchy */
	    HTM_TxtF ("%s:&nbsp;%s",
	              Txt_Course,
	              Hie->Crs.ShrtName);
	    break;
	 case Tml_NOTE_FORUM_POST:
	    /* Write forum name */
	    HTM_TxtF ("%s:&nbsp;%s",
	              Txt_Forum,
	              ForumName);
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
   extern const Act_Action_t For_ActionsSeeFor[For_NUM_TYPES_FORUM];
   extern const char *Txt_TIMELINE_NOTE[Tml_NOT_NUM_NOTE_TYPES];
   extern const char *Txt_not_available;
   char *Anchor = NULL;
   static const Act_Action_t Tml_DefaultActions[Tml_NOT_NUM_NOTE_TYPES] =
     {
      [Tml_NOTE_UNKNOWN          ] = ActUnk,
      /* Start tab */
      [Tml_NOTE_POST             ] = ActUnk,	// action not used
      /* Institution tab */
      [Tml_NOTE_INS_DOC_PUB_FILE ] = ActReqDatSeeDocIns,
      [Tml_NOTE_INS_SHA_PUB_FILE ] = ActReqDatShaIns,
      /* Center tab */
      [Tml_NOTE_CTR_DOC_PUB_FILE ] = ActReqDatSeeDocCtr,
      [Tml_NOTE_CTR_SHA_PUB_FILE ] = ActReqDatShaCtr,
      /* Degree tab */
      [Tml_NOTE_DEG_DOC_PUB_FILE ] = ActReqDatSeeDocDeg,
      [Tml_NOTE_DEG_SHA_PUB_FILE ] = ActReqDatShaDeg,
      /* Course tab */
      [Tml_NOTE_CRS_DOC_PUB_FILE ] = ActReqDatSeeDocCrs,
      [Tml_NOTE_CRS_SHA_PUB_FILE ] = ActReqDatShaCrs,
      /* Assessment tab */
      [Tml_NOTE_CALL_FOR_EXAM    ] = ActSeeOneCfe,
      /* Users tab */
      /* Messages tab */
      [Tml_NOTE_NOTICE           ] = ActSeeOneNot,
      [Tml_NOTE_FORUM_POST       ] = ActSeeFor,
      /* Analytics tab */
      /* Profile tab */
     };
   static const char *Tml_Icons[Tml_NOT_NUM_NOTE_TYPES] =
     {
      [Tml_NOTE_UNKNOWN          ] = NULL,
      /* Start tab */
      [Tml_NOTE_POST             ] = NULL,	// icon not used
      /* Institution tab */
      [Tml_NOTE_INS_DOC_PUB_FILE ] = "file.svg",
      [Tml_NOTE_INS_SHA_PUB_FILE ] = "file.svg",
      /* Center tab */
      [Tml_NOTE_CTR_DOC_PUB_FILE ] = "file.svg",
      [Tml_NOTE_CTR_SHA_PUB_FILE ] = "file.svg",
      /* Degree tab */
      [Tml_NOTE_DEG_DOC_PUB_FILE ] = "file.svg",
      [Tml_NOTE_DEG_SHA_PUB_FILE ] = "file.svg",
      /* Course tab */
      [Tml_NOTE_CRS_DOC_PUB_FILE ] = "file.svg",
      [Tml_NOTE_CRS_SHA_PUB_FILE ] = "file.svg",
      /* Assessment tab */
      [Tml_NOTE_CALL_FOR_EXAM    ] = "bullhorn.svg",
      /* Users tab */
      /* Messages tab */
      [Tml_NOTE_NOTICE           ] = "sticky-note.svg",
      [Tml_NOTE_FORUM_POST       ] = "comments.svg",
      /* Analytics tab */
      /* Profile tab */
     };

   if (Not->Unavailable ||	// File/notice... pointed by this note is unavailable
       Gbl.Form.Inside)		// Inside another form
     {
      /***** Do not put form *****/
      /* Begin container */
      HTM_DIV_Begin ("class=\"Tml_FORM_OFF\"");

         /* Text ("not available") */
	 HTM_Txt (Txt_TIMELINE_NOTE[Not->Type]);
	 if (Not->Unavailable)
	    HTM_TxtF ("&nbsp;(%s)",Txt_not_available);

      /* End container */
      HTM_DIV_End ();
     }
   else			// Not inside another form
     {
      /***** Begin container *****/
      HTM_DIV_Begin ("class=\"Tml_FORM\"");

	 /***** Begin form with parameters depending on the type of note *****/
	 switch (Not->Type)
	   {
	    case Tml_NOTE_INS_DOC_PUB_FILE:
	    case Tml_NOTE_INS_SHA_PUB_FILE:
	       Frm_BeginFormUnique (Tml_DefaultActions[Not->Type]);
		  Brw_PutHiddenParamFilCod (Not->Cod);
		  if (Not->HieCod != Gbl.Hierarchy.Ins.InsCod)	// Not the current institution
		     Ins_PutParamInsCod (Not->HieCod);		// Go to another institution
	       break;
	    case Tml_NOTE_CTR_DOC_PUB_FILE:
	    case Tml_NOTE_CTR_SHA_PUB_FILE:
	       Frm_BeginFormUnique (Tml_DefaultActions[Not->Type]);
		  Brw_PutHiddenParamFilCod (Not->Cod);
		  if (Not->HieCod != Gbl.Hierarchy.Ctr.CtrCod)	// Not the current center
		     Ctr_PutParamCtrCod (Not->HieCod);		// Go to another center
		  break;
	    case Tml_NOTE_DEG_DOC_PUB_FILE:
	    case Tml_NOTE_DEG_SHA_PUB_FILE:
	       Frm_BeginFormUnique (Tml_DefaultActions[Not->Type]);
		  Brw_PutHiddenParamFilCod (Not->Cod);
		  if (Not->HieCod != Gbl.Hierarchy.Deg.DegCod)	// Not the current degree
		     Deg_PutParamDegCod (Not->HieCod);		// Go to another degree
	       break;
	    case Tml_NOTE_CRS_DOC_PUB_FILE:
	    case Tml_NOTE_CRS_SHA_PUB_FILE:
	       Frm_BeginFormUnique (Tml_DefaultActions[Not->Type]);
		  Brw_PutHiddenParamFilCod (Not->Cod);
		  if (Not->HieCod != Gbl.Hierarchy.Crs.CrsCod)	// Not the current course
		     Crs_PutParamCrsCod (Not->HieCod);		// Go to another course
	       break;
	    case Tml_NOTE_CALL_FOR_EXAM:
	       Frm_SetAnchorStr (Not->Cod,&Anchor);
	       Frm_BeginFormUniqueAnchor (Tml_DefaultActions[Not->Type],
					  Anchor);	// Locate on this specific exam
	       Frm_FreeAnchorStr (Anchor);
		  Cfe_PutHiddenParamExaCod (Not->Cod);
		  if (Not->HieCod != Gbl.Hierarchy.Crs.CrsCod)	// Not the current course
		     Crs_PutParamCrsCod (Not->HieCod);		// Go to another course
	       break;
	    case Tml_NOTE_POST:	// Not applicable
	       return;
	    case Tml_NOTE_FORUM_POST:
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
	    case Tml_NOTE_NOTICE:
	       Frm_SetAnchorStr (Not->Cod,&Anchor);
	       Frm_BeginFormUniqueAnchor (Tml_DefaultActions[Not->Type],
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
	    HTM_BUTTON_Submit_Begin (Txt_TIMELINE_NOTE[Not->Type],
	                             "class=\"BT_LINK FORM_IN_WHITE %s ICO_HIGHLIGHT\"",
	                             The_GetSuffix ());

	       /* Icon and text */
	       Ico_PutIcon (Tml_Icons[Not->Type],Ico_BLACK,
	                    Txt_TIMELINE_NOTE[Not->Type],"CONTEXT_ICOx16");
	       HTM_TxtF ("&nbsp;%s",Txt_TIMELINE_NOTE[Not->Type]);

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

void TmlNot_GetNoteSummary (const struct TmlNot_Note *Not,
                            char SummaryStr[Ntf_MAX_BYTES_SUMMARY + 1])
  {
   SummaryStr[0] = '\0';

   switch (Not->Type)
     {
      case Tml_NOTE_UNKNOWN:
          break;
      case Tml_NOTE_INS_DOC_PUB_FILE:
      case Tml_NOTE_INS_SHA_PUB_FILE:
      case Tml_NOTE_CTR_DOC_PUB_FILE:
      case Tml_NOTE_CTR_SHA_PUB_FILE:
      case Tml_NOTE_DEG_DOC_PUB_FILE:
      case Tml_NOTE_DEG_SHA_PUB_FILE:
      case Tml_NOTE_CRS_DOC_PUB_FILE:
      case Tml_NOTE_CRS_SHA_PUB_FILE:
	 Brw_GetSummaryAndContentOfFile (SummaryStr,NULL,Not->Cod,false);
         break;
      case Tml_NOTE_CALL_FOR_EXAM:
         Cfe_GetSummaryAndContentCallForExam (SummaryStr,NULL,Not->Cod,false);
         break;
      case Tml_NOTE_POST:
	 // Not applicable
         break;
      case Tml_NOTE_FORUM_POST:
         For_GetSummaryAndContentForumPst (SummaryStr,NULL,Not->Cod,false);
         break;
      case Tml_NOTE_NOTICE:
         Not_GetSummaryAndContentNotice (SummaryStr,NULL,Not->Cod,false);
         break;
     }
  }

/*****************************************************************************/
/************************ Write bottom part of a note ************************/
/*****************************************************************************/

static void TmlNot_WriteButtonsAndComms (const struct Tml_Timeline *Timeline,
                                         const struct TmlNot_Note *Not,
                                         const struct UsrData *UsrDat)	// Author
  {
   char IdNewComm[Frm_MAX_BYTES_ID + 1];

   /***** Create unique id for new comment *****/
   Frm_SetUniqueId (IdNewComm);

   /***** Left: button to add a comment *****/
   TmlNot_WriteButtonToAddAComm (Not,IdNewComm);

   /***** Right: write favs, shared and remove buttons, and comments *****/
   TmlNot_WriteFavShaRemAndComms (Timeline,Not,UsrDat);

   /***** Put hidden form to write a new comment *****/
   TmlCom_PutPhotoAndFormToWriteNewComm (Timeline,Not->NotCod,IdNewComm);
  }

/*****************************************************************************/
/********************** Write button to add a comment ************************/
/*****************************************************************************/

static void TmlNot_WriteButtonToAddAComm (const struct TmlNot_Note *Not,
                                          const char IdNewComm[Frm_MAX_BYTES_ID + 1])
  {
   /***** Begin container *****/
   HTM_DIV_Begin ("class=\"Tml_BOTTOM_LEFT\"");

      /***** Button to add a comment *****/
      if (Not->Unavailable)	// Unavailable notes can not be commented
	 TmlCom_PutIconCommDisabled ();
      else
	 TmlCom_PutIconToToggleComm (IdNewComm);

   /***** End container *****/
   HTM_DIV_End ();
  }

/*****************************************************************************/
/******* Write favs, shared and remove buttons, and comments of a note *******/
/*****************************************************************************/

static void TmlNot_WriteFavShaRemAndComms (const struct Tml_Timeline *Timeline,
					   const struct TmlNot_Note *Not,
					   const struct UsrData *UsrDat)	// Author
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
                                   const struct UsrData *UsrDat)	// Author
  {
   static unsigned NumDiv = 0;	// Used to create unique div id for fav and shared

   NumDiv++;

   /***** Begin foot container *****/
   HTM_DIV_Begin ("class=\"Tml_FOOT Tml_RIGHT_WIDTH\"");

      /***** Foot column 1: fav zone *****/
      HTM_DIV_Begin ("id=\"fav_not_%s_%u\""
	             " class=\"Tml_FAV_NOT Tml_FAV_NOT_WIDTH\"",
		     Gbl.UniqueNameEncrypted,NumDiv);
	 TmlUsr_PutIconFavSha (TmlUsr_FAV_UNF_NOTE,
	                        Not->NotCod,Not->UsrCod,Not->NumFavs,
			        TmlUsr_SHOW_FEW_USRS);
      HTM_DIV_End ();

      /***** Foot column 2: share zone *****/
      HTM_DIV_Begin ("id=\"sha_not_%s_%u\""
	             " class=\"Tml_SHA_NOT Tml_SHA_NOT_WIDTH\"",
		     Gbl.UniqueNameEncrypted,NumDiv);
	 TmlUsr_PutIconFavSha (TmlUsr_SHA_UNS_NOTE,
	                        Not->NotCod,Not->UsrCod,Not->NumShared,
	                        TmlUsr_SHOW_FEW_USRS);
      HTM_DIV_End ();

      /***** Foot column 3: icon to remove this note *****/
      HTM_DIV_Begin ("class=\"Tml_REM\"");
	 if (Usr_ItsMe (UsrDat->UsrCod))	// I am the author
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
   extern const char *Txt_Remove;

   /***** Form to remove publication *****/
   /* Begin form */
   TmlFrm_BeginForm (Timeline,TmlFrm_REQ_REM_NOTE);
      TmlNot_PutHiddenParamNotCod (NotCod);

      /* Icon to remove */
      Ico_PutIconLink ("trash.svg",Ico_RED,Txt_Remove);

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
   long HieCod;	// Hierarchy code (institution/center/degree/course)

   switch (NoteType)
     {
      case Tml_NOTE_INS_DOC_PUB_FILE:
      case Tml_NOTE_INS_SHA_PUB_FILE:
	 HieCod = Gbl.Hierarchy.Ins.InsCod;
	 break;
      case Tml_NOTE_CTR_DOC_PUB_FILE:
      case Tml_NOTE_CTR_SHA_PUB_FILE:
	 HieCod = Gbl.Hierarchy.Ctr.CtrCod;
	 break;
      case Tml_NOTE_DEG_DOC_PUB_FILE:
      case Tml_NOTE_DEG_SHA_PUB_FILE:
	 HieCod = Gbl.Hierarchy.Deg.DegCod;
	 break;
      case Tml_NOTE_CRS_DOC_PUB_FILE:
      case Tml_NOTE_CRS_SHA_PUB_FILE:
      case Tml_NOTE_CALL_FOR_EXAM:
      case Tml_NOTE_NOTICE:
	 HieCod = Gbl.Hierarchy.Crs.CrsCod;
	 break;
      default:
	 HieCod = -1L;
         break;
     }

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
                                            true)) > 0)	// Only public files
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
						     Brw_GetCodForFileBrowser (),
						     Path);
  }

/*****************************************************************************/
/****************** Put parameter with the code of a note ********************/
/*****************************************************************************/

void TmlNot_PutHiddenParamNotCod (long NotCod)
  {
   Par_PutHiddenParamLong (NULL,"NotCod",NotCod);
  }

/*****************************************************************************/
/****************** Get parameter with the code of a note ********************/
/*****************************************************************************/

long TmlNot_GetParamNotCod (void)
  {
   /***** Get note code *****/
   return Par_GetParToLong ("NotCod");
  }

/*****************************************************************************/
/*********************** Request the removal of a note ***********************/
/*****************************************************************************/

void TmlNot_RequestRemNoteUsr (void)
  {
   struct Tml_Timeline Timeline;

   /***** Reset timeline context *****/
   Tml_ResetTimeline (&Timeline);

   /***** Get user whom profile is displayed *****/
   Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ();

   /***** Show user's profile *****/
   Prf_ShowUserProfile (&Gbl.Usrs.Other.UsrDat);

   /***** Begin section *****/
   HTM_SECTION_Begin (Tml_TIMELINE_SECTION_ID);

      /***** Request the removal of note *****/
      TmlNot_RequestRemovalNote (&Timeline);

      /***** Write timeline again (user) *****/
      Tml_ShowTimelineUsr (&Timeline);

   /***** End section *****/
   HTM_SECTION_End ();
  }

void TmlNot_RequestRemNoteGbl (void)
  {
   struct Tml_Timeline Timeline;

   /***** Initialize timeline *****/
   Tml_InitTimelineGbl (&Timeline);

   /***** Request the removal of note *****/
   TmlNot_RequestRemovalNote (&Timeline);

   /***** Write timeline again (global) *****/
   Tml_ShowNoteAndTimelineGbl (&Timeline);
  }

static void TmlNot_RequestRemovalNote (struct Tml_Timeline *Timeline)
  {
   extern const char *Txt_Do_you_really_want_to_remove_the_following_post;
   struct TmlNot_Note Not;

   /***** Get data of note *****/
   Not.NotCod = TmlNot_GetParamNotCod ();
   TmlNot_GetDataOfNoteByCod (&Not);

   /***** Do some checks *****/
   if (!TmlUsr_CheckIfICanRemove (Not.NotCod,Not.UsrCod))
      return;

   /***** Show question and button to remove note *****/
   /* Begin alert */
   TmlFrm_BeginAlertRemove (Txt_Do_you_really_want_to_remove_the_following_post);

   /* Show note */
   Box_BoxBegin (NULL,NULL,
		 NULL,NULL,
		 NULL,Box_CLOSABLE);
      HTM_DIV_Begin ("class=\"Tml_WIDTH\"");
	 TmlNot_CheckAndWriteNoteWithTopMsg (Timeline,&Not,
					      Tml_TOP_MESSAGE_NONE,
					      -1L);
      HTM_DIV_End ();
   Box_BoxEnd ();

   /* End alert */
   Timeline->NotCod = Not.NotCod;	// Note to be removed
   TmlFrm_EndAlertRemove (Timeline,TmlFrm_REM_NOTE,
			   TmlNot_PutParamsRemoveNote);
  }

/*****************************************************************************/
/********************* Put parameters to remove a note ***********************/
/*****************************************************************************/

static void TmlNot_PutParamsRemoveNote (void *Timeline)
  {
   if (Timeline)
     {
      if (Gbl.Usrs.Other.UsrDat.UsrCod > 0)	// User's timeline
	 Usr_PutParamOtherUsrCodEncrypted (Gbl.Usrs.Other.UsrDat.EnUsrCod);
      else					// Global timeline
	 Usr_PutHiddenParamWho (((struct Tml_Timeline *) Timeline)->Who);
      TmlNot_PutHiddenParamNotCod (((struct Tml_Timeline *) Timeline)->NotCod);
     }
  }

/*****************************************************************************/
/******************************* Remove a note *******************************/
/*****************************************************************************/

void TmlNot_RemoveNoteUsr (void)
  {
   struct Tml_Timeline Timeline;

   /***** Reset timeline context *****/
   Tml_ResetTimeline (&Timeline);

   /***** Get user whom profile is displayed *****/
   Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ();

   /***** Show user's profile *****/
   Prf_ShowUserProfile (&Gbl.Usrs.Other.UsrDat);

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
   Not.NotCod = TmlNot_GetParamNotCod ();
   TmlNot_GetDataOfNoteByCod (&Not);

   /***** Trivial check 1: note code should be > 0 *****/
   if (Not.NotCod <= 0)
     {
      Ale_ShowAlert (Ale_WARNING,Txt_The_post_no_longer_exists);
      return;
     }

   /***** Trivial check 2: Am I the author of this note *****/
   if (!Usr_ItsMe (Not.UsrCod))
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
   if (Not->Type == Tml_NOTE_POST)
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

   if (Not->Type == Tml_NOTE_POST)
      /***** Remove post *****/
      Tml_DB_RemovePost (Not->Cod);
  }

/*****************************************************************************/
/************************ Get data of note from row **************************/
/*****************************************************************************/

static void TmlNot_GetDataOfNoteFromRow (MYSQL_ROW row,struct TmlNot_Note *Not)
  {
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
   Not->Unavailable = (row[5][0] == 'Y');

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

   return Tml_NOTE_UNKNOWN;
  }

/*****************************************************************************/
/*************************** Reset fields of note ****************************/
/*****************************************************************************/

static void TmlNot_ResetNote (struct TmlNot_Note *Not)
  {
   Not->NotCod      = -1L;
   Not->Type        = Tml_NOTE_UNKNOWN;
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

void TmlNot_GetDataOfNoteByCod (struct TmlNot_Note *Not)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;

   /***** Trivial check: note code should be > 0 *****/
   if (Not->NotCod <= 0)
     {
      /* Reset fields of note */
      TmlNot_ResetNote (Not);
      return;
     }

   /***** Get data of note from database *****/
   if (Tml_DB_GetDataOfNoteByCod (Not->NotCod,&mysql_res))
     {
      /* Get data of note */
      row = mysql_fetch_row (mysql_res);
      TmlNot_GetDataOfNoteFromRow (row,Not);
     }
   else
      /* Reset fields of note */
      TmlNot_ResetNote (Not);

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }
