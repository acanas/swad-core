// swad_message.c: messages between users

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2025 Antonio Cañas Vargas

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
#include <linux/limits.h>	// For PATH_MAX
#include <stddef.h>		// For NULL
#include <stdio.h>		// For asprintf
#include <stdlib.h>		// For free
#include <string.h>		// For string functions
#include <time.h>		// For time

#include "swad_action.h"
#include "swad_action_list.h"
#include "swad_autolink.h"
#include "swad_box.h"
#include "swad_config.h"
#include "swad_course.h"
#include "swad_database.h"
#include "swad_error.h"
#include "swad_figure.h"
#include "swad_form.h"
#include "swad_forum.h"
#include "swad_global.h"
#include "swad_group.h"
#include "swad_hierarchy.h"
#include "swad_hierarchy_type.h"
#include "swad_HTML.h"
#include "swad_ID.h"
#include "swad_media.h"
#include "swad_message.h"
#include "swad_message_database.h"
#include "swad_nickname_database.h"
#include "swad_notification.h"
#include "swad_notification_database.h"
#include "swad_pagination.h"
#include "swad_parameter.h"
#include "swad_parameter_code.h"
#include "swad_photo.h"
#include "swad_profile.h"
#include "swad_profile_database.h"
#include "swad_session_database.h"
#include "swad_setting.h"
#include "swad_user.h"
#include "swad_user_clipboard.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/***************************** Private constants *****************************/
/*****************************************************************************/

// Forum images will be saved with:
// - maximum width of Msg_IMAGE_SAVED_MAX_HEIGHT
// - maximum height of Msg_IMAGE_SAVED_MAX_HEIGHT
// - maintaining the original aspect ratio (aspect ratio recommended: 3:2)
#define Msg_IMAGE_SAVED_MAX_WIDTH	768
#define Msg_IMAGE_SAVED_MAX_HEIGHT	768
#define Msg_IMAGE_SAVED_QUALITY		 90	// 1 to 100

static Pag_WhatPaginate_t Msg_WhatPaginate[Msg_NUM_TYPES_OF_MSGS] =
  {
   [Msg_WRITING ] = Pag_NONE,
   [Msg_RECEIVED] = Pag_MESSAGES_RECEIVED,
   [Msg_SENT    ] = Pag_MESSAGES_SENT,
  };

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Msg_ResetMessages (struct Msg_Messages *Messages);

static void Msg_PutFormMsgUsrs (struct Msg_Messages *Messages);

static void Msg_ShowSntOrRcvMessages (struct Msg_Messages *Messages);
static void Msg_PutLinkToViewBannedUsers(void);

static void Msg_SetNumMsgsStr (const struct Msg_Messages *Messages,
                               char **NumMsgsStr,unsigned NumUnreadMsgs);

static void Msg_PutIconsListMsgs (void *Messages);
static void Msg_PutParsOneMsg (void *Messages);

static void Msg_ShowFormSelectCourseSentOrRecMsgs (const struct Msg_Messages *Messages);
static void Msg_ShowFormToFilterMsgs (const struct Msg_Messages *Messages);

static void Msg_ShowFormToShowOnlyUnreadMessages (const struct Msg_Messages *Messages);
static bool Msg_GetParOnlyUnreadMsgs (void);
static void Msg_ShowASentOrReceivedMessage (struct Msg_Messages *Messages,
                                            long MsgNum,long MsgCod);
static void Msg_PutLinkToShowMorePotentialRecipients (struct Msg_Messages *Messages);
static void Msg_PutParsShowMorePotentialRecipients (void *Messages);
static void Msg_PutParsWriteMsg (void *Messages);
static void Msg_PutParsSubjectAndContent (void);
static void Msg_ShowOneUniqueRecipient (void);
static void Msg_WriteFormUsrsIDsOrNicksOtherRecipients (bool OtherRecipientsBefore);
static void Msg_WriteFormSubjectAndContentMsgToUsrs (struct Msg_Messages *Messages);

static void Msg_PutParOtherRecipients (void);

static void Msg_CreateRcvMsgForEachRecipient (struct Msg_Messages *Messages);

static void Msg_ShowNumMsgsDeleted (unsigned NumMsgs);

static void Msg_GetParMsgsCrsCod (struct Msg_Messages *Messages);
static void Msg_GetParFilterFromTo (struct Msg_Messages *Messages);
static void Msg_GetParFilterContent (struct Msg_Messages *Messages);

static long Msg_InsertNewMsg (const char *Subject,const char *Content,
                              struct Med_Media *Media);

static unsigned long Msg_RemoveSomeRecOrSntMsgsUsr (const struct Msg_Messages *Messages,
                                                    long UsrCod,
                                                    const char *FilterFromToSubquery);
static void Msg_RemoveRcvMsg (long MsgCod,long UsrCod);

static void Msg_GetMsgSntData (long MsgCod,long *CrsCod,long *UsrCod,
                               time_t *CreatTimeUTC,
                               char Subject[Cns_MAX_BYTES_SUBJECT + 1],
                               bool *Deleted);
static void Msg_GetMsgContent (long MsgCod,
                               char Content[Cns_MAX_BYTES_LONG_TEXT + 1],
                               struct Med_Media *Media);

static void Msg_WriteSentOrReceivedMsgSubject (struct Msg_Messages *Messages,
					       long MsgCod,const char *Subject,
                                               CloOpe_ClosedOrOpen_t ClosedOrOpen,
                                               bool Expanded);

static bool Msg_WriteCrsOrgMsg (long CrsCod);

static void Msg_WriteFormToReply (long MsgCod,long CrsCod,bool FromThisCrs,
                                  const struct Usr_Data *UsrDat);
static void Msg_WriteMsgFrom (struct Msg_Messages *Messages,
                              struct Usr_Data *UsrDat,bool Deleted);
static void Msg_WriteMsgTo (struct Msg_Messages *Messages,long MsgCod);

static void Msg_PutFormToBanSender (struct Msg_Messages *Messages,
                                    struct Usr_Data *UsrDat);
static void Msg_PutFormToUnbanSender (struct Msg_Messages *Messages,
                                      struct Usr_Data *UsrDat);
static void Msg_UnbanSender (void);

/*****************************************************************************/
/**************************** Reset messages context *************************/
/*****************************************************************************/

static void Msg_ResetMessages (struct Msg_Messages *Messages)
  {
   Messages->NumMsgs              = 0;
   Messages->Subject[0]           = '\0';
   Messages->Content              = NULL;
   Messages->FilterCrsCod         = -1L;
   Messages->FilterCrsShrtName[0] = '\0';
   Messages->FilterFromTo[0]      = '\0';
   Messages->FilterContent[0]     = '\0';
   Messages->ShowOnlyUnreadMsgs   = false;
   Messages->ExpandedMsgCod       = -1L;
   Messages->Reply.IsReply        = false;
   Messages->Reply.Replied        = false;
   Messages->Reply.OriginalMsgCod = -1L;
   Messages->Rcv.NumRecipients    = 0;
   Messages->Rcv.NumErrors        = 0;
   Messages->ShowOnlyOneRecipient = false;
   Messages->CurrentPage          = 0;
   Messages->MsgCod               = -1L;
  }

/*****************************************************************************/
/***************** Put a form to write a new message to users ****************/
/*****************************************************************************/

void Msg_FormMsgUsrs (void)
  {
   struct Msg_Messages Messages;

   /***** Reset messages context *****/
   Msg_ResetMessages (&Messages);

   /***** Allocate memory for message content *****/
   if ((Messages.Content = malloc (Cns_MAX_BYTES_LONG_TEXT + 1)) == NULL)
      Err_NotEnoughMemoryExit ();

   /***** Get possible hidden subject and content of the message *****/
   Par_GetParHTML ("HiddenSubject",Messages.Subject,Cns_MAX_BYTES_SUBJECT);
   Par_GetParAndChangeFormat ("HiddenContent",Messages.Content,Cns_MAX_BYTES_LONG_TEXT,
                              Str_TO_TEXT,Str_DONT_REMOVE_SPACES);

   /***** Show a form to compose a message to users *****/
   Msg_PutFormMsgUsrs (&Messages);

   /***** Free allocated memory for message content *****/
   free (Messages.Content);
  }

/*****************************************************************************/
/***************** Put a form to write a new message to users ****************/
/*****************************************************************************/

static void Msg_PutFormMsgUsrs (struct Msg_Messages *Messages)
  {
   extern const char *Hlp_COMMUNICATION_Messages_write;
   extern const char *Txt_Message;
   extern const char *Txt_MSG_To;
   unsigned NumUsrsInCrs = 0;	// Initialized to avoid warning
   bool ShowUsrsInCrs = false;
   bool GetUsrsInCrs;
   bool OtherRecipientsBefore = false;
   char *ClassInput;
   Pho_ShowPhotos_t ShowPhotos;
   __attribute__((unused)) Err_SuccessOrError_t SuccessOrError;

   Gbl.Usrs.LstUsrs[Rol_STD].NumUsrs =
   Gbl.Usrs.LstUsrs[Rol_NET].NumUsrs =
   Gbl.Usrs.LstUsrs[Rol_TCH].NumUsrs = 0;

   /***** Get parameter that indicates if the message is a reply to another message *****/
   if ((Messages->Reply.IsReply = Par_GetParBool ("IsReply")))
      /* Get original message code */
      Messages->Reply.OriginalMsgCod = ParCod_GetAndCheckPar (ParCod_Msg);

   /***** Get user's code of possible preselected recipient *****/
   if (Usr_GetParOtherUsrCodEncryptedAndGetUsrData ())	// There is a preselected recipient
      /* Get who to show as potential recipients:
         - only the selected recipient
         - any user (default) */
      Messages->ShowOnlyOneRecipient = Par_GetParBool ("ShowOnlyOneRecipient");
   else
      Messages->ShowOnlyOneRecipient = false;

   GetUsrsInCrs = !Messages->ShowOnlyOneRecipient &&				// Show list of potential recipients
	          (Gbl.Usrs.Me.IBelongToCurrent[Hie_CRS] == Usr_BELONG ||	// If there is a course selected and I belong to it...
	           Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM);			// ...or I am a system admin
   if (GetUsrsInCrs)
     {
      /***** Get and update type of list,
	     number of columns in class photo
	     and preference about view photos *****/
      Set_GetAndUpdatePrefsAboutUsrList (&ShowPhotos);

      /***** Get groups to show ******/
      Grp_GetParCodsSeveralGrpsToShowUsrs ();

      /***** Get and order lists of users from this course *****/
      Usr_GetListUsrs (Hie_CRS,Rol_STD);
      Usr_GetListUsrs (Hie_CRS,Rol_NET);
      Usr_GetListUsrs (Hie_CRS,Rol_TCH);
      NumUsrsInCrs = Gbl.Usrs.LstUsrs[Rol_STD].NumUsrs +	// Students
	             Gbl.Usrs.LstUsrs[Rol_NET].NumUsrs +	// Non-editing teachers
		     Gbl.Usrs.LstUsrs[Rol_TCH].NumUsrs;		// Teachers
     }

   /***** Begin box *****/
   Box_BoxBegin (Txt_Message,Msg_PutIconsListMsgs,Messages,
		 Hlp_COMMUNICATION_Messages_write,Box_NOT_CLOSABLE);

      if (Messages->ShowOnlyOneRecipient)
	 /***** Form to show several potential recipients *****/
	 Msg_PutLinkToShowMorePotentialRecipients (Messages);
      else
	{
	 /***** Get list of users belonging to the current course *****/
	 if (GetUsrsInCrs)
	   {
	    /***** Form to select groups *****/
	    Grp_ShowFormToSelectSeveralGroups (Msg_PutParsWriteMsg,Messages,
					       "CopyMessageToHiddenFields();");

	    /***** Begin section with user list *****/
	    HTM_SECTION_Begin (Usr_USER_LIST_SECTION_ID);

	       if (NumUsrsInCrs)
		 {
		  /***** Form to select type of list used for select several users *****/
		  Usr_ShowFormsToSelectUsrListType (Msg_PutParsWriteMsg,Messages,
						    "CopyMessageToHiddenFields();",
						    ShowPhotos);

		  /***** Put link to register students *****/
		  Enr_CheckStdsAndPutButtonToEnrolStdsInCurrentCrs ();

		  /***** Check if it's a big list *****/
		  ShowUsrsInCrs = Usr_GetIfShowBigList (NumUsrsInCrs,
							Msg_PutParsWriteMsg,Messages,
							"CopyMessageToHiddenFields();");

		  if (ShowUsrsInCrs)
		     /***** Get lists of selected users *****/
		     Usr_GetListsSelectedEncryptedUsrsCods (&Gbl.Usrs.Selected,
							    Usr_GET_LIST_ALL_USRS);
		 }

	    /***** End section with user list *****/
	    HTM_SECTION_End ();
	   }

	 /***** Get list of users' IDs or nicknames written explicitely *****/
	 SuccessOrError = Usr_GetListMsgRecipientsWrittenExplicitelyBySender (false);
	}

      /***** Begin form to select recipients and write the message *****/
      Frm_BeginFormId (ActRcvMsgUsr,Usr_FORM_TO_SELECT_USRS_ID);

	 if (Messages->Reply.IsReply)
	   {
	    Par_PutParChar ("IsReply",'Y');
	    ParCod_PutPar (ParCod_Msg,Messages->Reply.OriginalMsgCod);
	   }
	 if (Gbl.Usrs.Other.UsrDat.UsrCod > 0)
	   {
	    Usr_PutParOtherUsrCodEncrypted (Gbl.Usrs.Other.UsrDat.EnUsrCod);
	    if (Messages->ShowOnlyOneRecipient)
	       Par_PutParChar ("ShowOnlyOneRecipient",'Y');
	   }

	 /***** Begin table *****/
	 HTM_TABLE_BeginCenterPadding (2);

	    /***** "To:" section (recipients) *****/
	    HTM_TR_Begin (NULL);
	       /* Label */
	       Frm_LabelColumn ("Frm_C1 RT","",Txt_MSG_To);

	       /* Data */
	       HTM_TD_Begin ("class=\"Frm_C2 LT\"");
		  if (Messages->ShowOnlyOneRecipient)
		     /***** Show only one user as recipient *****/
		     Msg_ShowOneUniqueRecipient ();
		  else
		    {
		     if (ShowUsrsInCrs)
		       {
			/***** Show potential recipients *****/
			Usr_ListUsersToSelect (&Gbl.Usrs.Selected,ShowPhotos);
			OtherRecipientsBefore = true;
		       }
		     UsrClp_ListUsrsInMyClipboard (Frm_PUT_FORM,
						   false);		// Don't show if empty
		     if (!OtherRecipientsBefore)
		        OtherRecipientsBefore = (Gbl.Usrs.LstUsrs[Rol_UNK].NumUsrs != 0);
		     Msg_WriteFormUsrsIDsOrNicksOtherRecipients (OtherRecipientsBefore);	// Other users (nicknames)
		    }
	       HTM_TD_End ();
	    HTM_TR_End ();

	    /***** Subject and content sections *****/
	    Msg_WriteFormSubjectAndContentMsgToUsrs (Messages);

	 /***** End table *****/
	 HTM_TABLE_End ();

	 /***** Help for text editor and send button *****/
	 Lay_HelpPlainEditor ();

	 /***** Attached image (optional) *****/
	 if (asprintf (&ClassInput,"MSG_MED_INPUT INPUT_%s",The_GetSuffix ()) < 0)
	    Err_NotEnoughMemoryExit ();
	 Med_PutMediaUploader (-1,ClassInput);
	 free (ClassInput);

	 /***** Send button *****/
	 Btn_PutButton (Btn_SEND,NULL);

      /***** End form *****/
      Frm_EndForm ();

   /***** End box *****/
   Box_BoxEnd ();

   /***** Free memory used by the list of nicknames *****/
   Usr_FreeListOtherRecipients ();

   /***** Free memory used for by the lists of users *****/
   if (GetUsrsInCrs)
     {
      Usr_FreeUsrsList (Rol_TCH);
      Usr_FreeUsrsList (Rol_NET);
      Usr_FreeUsrsList (Rol_STD);
     }

   /***** Free memory used by list of selected users' codes *****/
   Usr_FreeListsSelectedEncryptedUsrsCods (&Gbl.Usrs.Selected);

   /***** Free memory for list of selected groups *****/
   Grp_FreeListCodSelectedGrps ();
  }

/*****************************************************************************/
/********** Put contextual link to show more potential recipients ************/
/*****************************************************************************/

static void Msg_PutLinkToShowMorePotentialRecipients (struct Msg_Messages *Messages)
  {
   extern const char *Txt_Show_more_recipients;

   /***** Contextual menu *****/
   Mnu_ContextMenuBegin ();
      Lay_PutContextualLinkIconText (ActReqMsgUsr,NULL,
				     Msg_PutParsShowMorePotentialRecipients,Messages,
				     "users.svg",Ico_BLACK,
				     Txt_Show_more_recipients,
				     "CopyMessageToHiddenFields();");
   Mnu_ContextMenuEnd ();
  }

/*****************************************************************************/
/************ Put parameters to show more potential recipients ***************/
/*****************************************************************************/

static void Msg_PutParsShowMorePotentialRecipients (void *Messages)
  {
   if (Messages)
     {
      if (((struct Msg_Messages *) Messages)->Reply.IsReply)
	{
	 Par_PutParChar ("IsReply",'Y');
	 ParCod_PutPar (ParCod_Msg,((struct Msg_Messages *) Messages)->Reply.OriginalMsgCod);
	}
      if (Gbl.Usrs.Other.UsrDat.UsrCod > 0)
	 Usr_PutParOtherUsrCodEncrypted (Gbl.Usrs.Other.UsrDat.EnUsrCod);

      /***** Hidden params to send subject and content *****/
      Msg_PutParsSubjectAndContent ();
     }
  }

/*****************************************************************************/
/***************** Put parameters when writing a message *********************/
/*****************************************************************************/

static void Msg_PutParsWriteMsg (void *Messages)
  {
   if (Messages)
     {
      Usr_PutParSelectedUsrsCods (&Gbl.Usrs.Selected);
      Msg_PutParOtherRecipients ();
      Msg_PutParsSubjectAndContent ();
      if (((struct Msg_Messages *) Messages)->Reply.IsReply)
	{
	 Par_PutParChar ("IsReply",'Y');
	 ParCod_PutPar (ParCod_Msg,((struct Msg_Messages *) Messages)->Reply.OriginalMsgCod);
	}
      if (Gbl.Usrs.Other.UsrDat.UsrCod > 0)
	{
	 Usr_PutParOtherUsrCodEncrypted (Gbl.Usrs.Other.UsrDat.EnUsrCod);
	 if (((struct Msg_Messages *) Messages)->ShowOnlyOneRecipient)
	    Par_PutParChar ("ShowOnlyOneRecipient",'Y');
	}
     }
  }

/*****************************************************************************/
/********** Put hidden parameters with message subject and content ***********/
/*****************************************************************************/

static void Msg_PutParsSubjectAndContent (void)
  {
   /***** Hidden params to send subject and content.
          When the user edit the subject or the content,
          they are copied here. *****/
   Par_PutParString (NULL,"HiddenSubject","");
   Par_PutParString (NULL,"HiddenContent","");
  }

/*****************************************************************************/
/************ Put parameters to show more potential recipients ***************/
/*****************************************************************************/

static void Msg_ShowOneUniqueRecipient (void)
  {
   static const char *ClassPhoto[PhoSha_NUM_SHAPES] =
     {
      [PhoSha_SHAPE_CIRCLE   ] = "PHOTOC21x28",
      [PhoSha_SHAPE_ELLIPSE  ] = "PHOTOE21x28",
      [PhoSha_SHAPE_OVAL     ] = "PHOTOO21x28",
      [PhoSha_SHAPE_RECTANGLE] = "PHOTOR21x28",
     };

   /***** Show user's photo *****/
   Pho_ShowUsrPhotoIfAllowed (&Gbl.Usrs.Other.UsrDat,
                              ClassPhoto[Gbl.Prefs.PhotoShape],Pho_ZOOM);

   /****** Write user's IDs ******/
   HTM_DIV_Begin ("class=\"MSG_TO_ONE_RCP %s_%s\"",
		  Gbl.Usrs.Other.UsrDat.Accepted ? "DAT_SMALL_NOBR_STRONG" :
			                           "DAT_SMALL_NOBR",
		  The_GetSuffix ());
      ID_WriteUsrIDs (&Gbl.Usrs.Other.UsrDat,NULL);
   HTM_DIV_End ();

   /***** Write user's name *****/
   HTM_DIV_Begin ("class=\"MSG_TO_ONE_RCP %s_%s\"",
		  Gbl.Usrs.Other.UsrDat.Accepted ? "DAT_SMALL_NOBR_STRONG" :
			                           "DAT_SMALL_NOBR",
		  The_GetSuffix ());
      HTM_Txt (Gbl.Usrs.Other.UsrDat.FullName);
   HTM_DIV_End ();

   /***** Hidden parameter with list of selected users,
          consisting of only user encrypted code *****/
   Usr_PutParSelectedOtherUsrCod ();
  }

/*****************************************************************************/
/************** Nicknames of recipients of a message to users ****************/
/*****************************************************************************/

static void Msg_WriteFormUsrsIDsOrNicksOtherRecipients (bool OtherRecipientsBefore)
  {
   extern const char *Txt_Other_recipients;
   extern const char *Txt_Recipients;
   extern const char *Txt_nicks_emails_or_IDs_separated_by_commas;

   /***** Title *****/
   HTM_LABEL_Begin ("for=\"OtherRecipients\""
		    " class=\"FORM_IN_%s\"",The_GetSuffix ());
      HTM_Txt (OtherRecipientsBefore ? Txt_Other_recipients :
				       Txt_Recipients);
      HTM_Colon (); HTM_LABEL_End ();

   /***** Textarea with users' @nicknames, emails or IDs *****/
   HTM_TEXTAREA_Begin (HTM_NO_ATTR,
		       "id=\"OtherRecipients\" name=\"OtherRecipients\""
		       " rows=\"2\" class=\"Frm_C2_INPUT INPUT_%s\""
		       " placeholder=\"%s\"",
		       The_GetSuffix (),
		       Txt_nicks_emails_or_IDs_separated_by_commas);
      HTM_Txt (Gbl.Usrs.ListOtherRecipients);
   HTM_TEXTAREA_End ();
  }

/*****************************************************************************/
/****** Write form fields with subject and content of a message to users *****/
/*****************************************************************************/

static void Msg_WriteFormSubjectAndContentMsgToUsrs (struct Msg_Messages *Messages)
  {
   extern const char *Txt_MSG_Subject;
   extern const char *Txt_MSG_Content;
   extern const char *Txt_Original_message;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   long MsgCod;
   bool SubjectAndContentComeFromForm = (Messages->Subject[0] ||
					 Messages->Content[0]);

   /***** Get possible code (of original message if it's a reply) *****/
   MsgCod = ParCod_GetPar (ParCod_Msg);

   /***** Message subject *****/
   HTM_TR_Begin (NULL);

      /* Label */
      Frm_LabelColumn ("Frm_C1 RT","MsgSubject",Txt_MSG_Subject);

      /* Data */
      HTM_TD_Begin ("class=\"Frm_C2 LT\"");
	 HTM_TEXTAREA_Begin (HTM_NO_ATTR,
			     "id=\"MsgSubject\" name=\"Subject\" rows=\"2\""
			     " class=\"Frm_C2_INPUT INPUT_%s\"",
			     The_GetSuffix ());

      /* If message is a reply ==> get original message */
      if (MsgCod > 0)	// It's a reply
	{
		  if (!SubjectAndContentComeFromForm)
		    {
		     /* Get subject and content of message from database */
		     if (Msg_DB_GetSubjectAndContent (&mysql_res,MsgCod) == Exi_DOES_NOT_EXIST)
			Err_WrongMessageExit ();

		     row = mysql_fetch_row (mysql_res);

		     /* Get subject (row[0]) and content (row[1]) */
		     Str_Copy (Messages->Subject,row[0],sizeof (Messages->Subject) - 1);
		     Str_Copy (Messages->Content,row[1],Cns_MAX_BYTES_LONG_TEXT);

		     /* Free structure that stores the query result */
		     DB_FreeMySQLResult (&mysql_res);
		    }

		  /* Write subject */
		  if (!SubjectAndContentComeFromForm)
		     HTM_Txt ("Re: ");
		  HTM_Txt (Messages->Subject);

	       HTM_TEXTAREA_End ();
	    HTM_TD_End ();
	 HTM_TR_End ();

	 /***** Message content *****/
	 HTM_TR_Begin (NULL);

	    /* Label */
	    Frm_LabelColumn ("Frm_C1 RT","MsgContent",Txt_MSG_Content);

	    /* Data */
	    HTM_TD_Begin ("class=\"Frm_C2 LT\"");
	       HTM_TEXTAREA_Begin (HTM_NO_ATTR,
				   "id=\"MsgContent\" name=\"Content\""
				   " rows=\"20\""
				   " class=\"Frm_C2_INPUT INPUT_%s\"",
				   The_GetSuffix ());

		  /* Begin textarea with a '\n', that will be not visible in textarea.
		     When Content is "\nLorem ipsum" (a white line before "Lorem ipsum"),
		     if we don't put the initial '\n' ==> the form will be sent starting
		     by "Lorem", without the white line */
		  HTM_LF ();

		  if (!SubjectAndContentComeFromForm)
		     HTM_TxtF ("\n\n----- %s -----\n",Txt_Original_message);

		  Msg_WriteMsgContent (Messages->Content,false,true);
      	}
      else	// It's not a reply
	{
		  /* End message subject */
		  HTM_Txt (Messages->Subject);

	       HTM_TEXTAREA_End ();
	    HTM_TD_End ();

	 HTM_TR_End ();

	 /***** Message content *****/
	 HTM_TR_Begin (NULL);

	    /* Label */
	    Frm_LabelColumn ("Frm_C1 RT","MsgContent",Txt_MSG_Content);

	    /* Data */
	    HTM_TD_Begin ("class=\"Frm_C2 LT\"");
	       HTM_TEXTAREA_Begin (HTM_NO_ATTR,
				   "id=\"MsgContent\" name=\"Content\""
				   " rows=\"20\""
				   " class=\"Frm_C2_INPUT INPUT_%s\"",
				   The_GetSuffix ());

		  /* Begin textarea with a '\n', that will be not visible in textarea.
		     When Content is "\nLorem ipsum" (a white line before "Lorem ipsum"),
		     if we don't put the initial '\n' ==> the form will be sent starting
		     by "Lorem", without the white line */
		  HTM_LF ();
		  HTM_Txt (Messages->Content);
	}

	 HTM_TEXTAREA_End ();
      HTM_TD_End ();
   HTM_TR_End ();
  }

/*****************************************************************************/
/********* Put hidden parameter for another recipient (one nickname) *********/
/*****************************************************************************/

static void Msg_PutParOtherRecipients (void)
  {
   if (Gbl.Usrs.ListOtherRecipients)
      if (Gbl.Usrs.ListOtherRecipients[0])
         Par_PutParString (NULL,"OtherRecipients",Gbl.Usrs.ListOtherRecipients);
  }

/*****************************************************************************/
/********************** Receive a new message from a user ********************/
/*****************************************************************************/

void Msg_RecMsgFromUsr (void)
  {
   extern const char *Txt_You_can_not_send_a_message_to_so_many_recipients_;
   extern const char *Txt_You_must_select_one_ore_more_recipients;
   extern const char *Txt_The_message_has_not_been_sent_to_any_recipient;
   extern const char *Txt_The_message_has_been_sent_to_1_recipient;
   extern const char *Txt_The_message_has_been_sent_to_X_recipients;
   extern const char *Txt_There_have_been_X_errors_in_sending_the_message;
   struct Msg_Messages Messages;
   Err_SuccessOrError_t SuccessOrError;

   /***** Reset messages context *****/
   Msg_ResetMessages (&Messages);

   /***** Allocate memory for message content *****/
   if ((Messages.Content = malloc (Cns_MAX_BYTES_LONG_TEXT + 1)) == NULL)
      Err_NotEnoughMemoryExit ();

   /***** Get data from form *****/
   /* Get subject and body */
   Par_GetParHTML ("Subject",Messages.Subject,Cns_MAX_BYTES_SUBJECT);
   Par_GetParAndChangeFormat ("Content",Messages.Content,Cns_MAX_BYTES_LONG_TEXT,
                              Str_DONT_CHANGE,Str_DONT_REMOVE_SPACES);

   /* Get parameter that indicates if the message is a reply to a previous message */
   if ((Messages.Reply.IsReply = Par_GetParBool ("IsReply")))
      /* Get original message code */
      Messages.Reply.OriginalMsgCod = ParCod_GetAndCheckPar (ParCod_Msg);

   /* Get user's code of possible preselected recipient */
   Usr_GetParOtherUsrCodEncryptedAndGetListIDs ();

   /* Get lists of selected users */
   Usr_GetListsSelectedEncryptedUsrsCods (&Gbl.Usrs.Selected,
					  Usr_GET_LIST_ALL_USRS);

   /* Get list of users' IDs or nicknames written explicitely */
   SuccessOrError = Usr_GetListMsgRecipientsWrittenExplicitelyBySender (true);

   /***** Check number of recipients *****/
   Messages.Rcv.NumRecipients = Usr_CountNumUsrsInListOfSelectedEncryptedUsrCods (&Gbl.Usrs.Selected);

   if (Messages.Rcv.NumRecipients)
     {
      if (Gbl.Usrs.Me.Role.Logged == Rol_STD &&
          Messages.Rcv.NumRecipients > Cfg_MAX_RECIPIENTS)
        {
         /* Write warning message */
         Ale_ShowAlert (Ale_WARNING,Txt_You_can_not_send_a_message_to_so_many_recipients_);
         SuccessOrError = Err_ERROR;
        }
     }
   else	// No recipients selected
     {
      /* Write warning message */
      Ale_ShowAlert (Ale_WARNING,Txt_You_must_select_one_ore_more_recipients);
      SuccessOrError = Err_ERROR;
     }

   /***** If error in list of recipients, show again the form used to write a message *****/
   switch (SuccessOrError)
     {
      case Err_SUCCESS:
	 /***** Loop over the list Gbl.Usrs.Selected.List[Rol_UNK],
		that holds the list of the recipients,
		creating a received message for each recipient *****/
	 Msg_CreateRcvMsgForEachRecipient (&Messages);

	 /***** Update received message setting Replied field to true *****/
	 if (Messages.Reply.Replied)
	    Msg_DB_SetRcvMsgAsReplied (Messages.Reply.OriginalMsgCod);

	 /***** Write final message *****/
	 if (Messages.Rcv.NumRecipients == 0)
	    Ale_ShowAlert (Ale_WARNING,Txt_The_message_has_not_been_sent_to_any_recipient);
	 else if (Messages.Rcv.NumRecipients == 1)
	    Ale_ShowAlert (Ale_SUCCESS,Txt_The_message_has_been_sent_to_1_recipient);
	 else
	    Ale_ShowAlert (Ale_SUCCESS,Txt_The_message_has_been_sent_to_X_recipients,
			   Messages.Rcv.NumRecipients);

	 /***** Show alert about errors on sending message *****/
	 if (Messages.Rcv.NumErrors > 1)
	    Ale_ShowAlert (Ale_ERROR,Txt_There_have_been_X_errors_in_sending_the_message,
			  Messages.Rcv.NumErrors);
	 break;
      case Err_ERROR:
      default:
	 /* Show the form again, with the subject and the message filled */
	 Str_ChangeFormat (Str_FROM_FORM,Str_TO_TEXT,
			   Messages.Content,Cns_MAX_BYTES_LONG_TEXT,Str_REMOVE_SPACES);
	 Msg_PutFormMsgUsrs (&Messages);
	 break;
     }

   /***** Free memory *****/
   /* Free memory used for list of users */
   Usr_FreeListOtherRecipients ();
   Usr_FreeListsSelectedEncryptedUsrsCods (&Gbl.Usrs.Selected);

   /* Free memory for message content */
   free (Messages.Content);
  }

/*****************************************************************************/
/************ Loop over the list Gbl.Usrs.Selected.List[Rol_UNK], ************/
/************ that holds the list of the recipients,              ************/
/************ creating a received message for each recipient      ************/
/*****************************************************************************/

static void Msg_CreateRcvMsgForEachRecipient (struct Msg_Messages *Messages)
  {
   extern const char *Txt_message_not_sent_to_X;
   extern const char *Txt_message_sent_to_X_notified_by_email;
   extern const char *Txt_message_sent_to_X_not_notified_by_email;
   extern const char *Txt_Error_getting_data_from_a_recipient;
   struct Med_Media Media;
   struct Usr_Data UsrDstData;
   const char *Ptr;
   bool MsgAlreadyInserted = false;
   long NewMsgCod = -1L;	// Initiliazed to avoid warning
   bool CreateNotif;
   bool NotifyByEmail;

   /***** Initialize and get media from form *****/
   Med_MediaConstructor (&Media);
   Media.Width   = Msg_IMAGE_SAVED_MAX_WIDTH;
   Media.Height  = Msg_IMAGE_SAVED_MAX_HEIGHT;
   Media.Quality = Msg_IMAGE_SAVED_QUALITY;
   Med_GetMediaFromForm (-1L,-1L,-1,&Media,NULL,NULL);
   Ale_ShowAlerts (NULL);

      /***** Initialize user's data *****/
      Usr_UsrDataConstructor (&UsrDstData);

         /***** Change content format *****/
	 Str_ChangeFormat (Str_FROM_FORM,Str_TO_RIGOROUS_HTML,
			   Messages->Content,Cns_MAX_BYTES_LONG_TEXT,Str_DONT_REMOVE_SPACES);

	 /***** Loop over list of recipients *****/
	 for (Ptr = Gbl.Usrs.Selected.List[Rol_UNK],
	      Messages->Rcv.NumRecipients = Messages->Rcv.NumErrors = 0;
	      *Ptr;
	     )
	   {
	    Par_GetNextStrUntilSeparParMult (&Ptr,UsrDstData.EnUsrCod,
					     Cry_BYTES_ENCRYPTED_STR_SHA256_BASE64);
	    Usr_GetUsrCodFromEncryptedUsrCod (&UsrDstData);
	    if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDstData,	// Get recipient's data from database
							 Usr_DONT_GET_PREFS,
							 Usr_DONT_GET_ROLE_IN_CRS))
	      {
	       /***** Check if recipient has banned me *****/
	       if (Msg_DB_CheckIfUsrIsBanned (Gbl.Usrs.Me.UsrDat.UsrCod,UsrDstData.UsrCod))
		  /***** Show an alert indicating that the message has not been sent successfully *****/
		  Ale_ShowAlert (Ale_WARNING,Txt_message_not_sent_to_X,UsrDstData.FullName);
	       else
		 {
		  /***** Create message *****/
		  if (!MsgAlreadyInserted)
		    {
		     // The message is inserted only once in the table of messages sent
		     NewMsgCod = Msg_InsertNewMsg (Messages->Subject,Messages->Content,&Media);
		     MsgAlreadyInserted = true;
		    }

		  /***** If this recipient is the original sender of a message been replied, set Replied to true *****/
		  Messages->Reply.Replied = (Messages->Reply.IsReply &&
					     UsrDstData.UsrCod == Gbl.Usrs.Other.UsrDat.UsrCod);

		  /***** This received message must be notified by email? *****/
		  CreateNotif = (UsrDstData.NtfEvents.CreateNotif & (1 << Ntf_EVENT_MESSAGE));
		  NotifyByEmail = CreateNotif &&
				  (UsrDstData.UsrCod != Gbl.Usrs.Me.UsrDat.UsrCod) &&
				  (UsrDstData.NtfEvents.SendEmail & (1 << Ntf_EVENT_MESSAGE));

		  /***** Create the received message for this recipient
			 and increment number of new messages received by this recipient *****/
		  Msg_DB_CreateRcvMsg (NewMsgCod,UsrDstData.UsrCod,NotifyByEmail);

		  /***** Create notification for this recipient.
			 If this recipient wants to receive notifications by -mail,
			 activate the sending of a notification *****/
		  if (CreateNotif)
		     Ntf_DB_StoreNotifyEventToUsr (Ntf_EVENT_MESSAGE,UsrDstData.UsrCod,NewMsgCod,
						   (Ntf_Status_t) (NotifyByEmail ? Ntf_STATUS_BIT_EMAIL :
										   0),
						   Gbl.Hierarchy.Node[Hie_INS].HieCod,
						   Gbl.Hierarchy.Node[Hie_CTR].HieCod,
						   Gbl.Hierarchy.Node[Hie_DEG].HieCod,
						   Gbl.Hierarchy.Node[Hie_CRS].HieCod);

		  /***** Show an alert indicating that the message has been sent successfully *****/
		  Ale_ShowAlert (Ale_SUCCESS,NotifyByEmail ? Txt_message_sent_to_X_notified_by_email :
							     Txt_message_sent_to_X_not_notified_by_email,
				 UsrDstData.FullName);

		  /***** Increment number of recipients *****/
		  Messages->Rcv.NumRecipients++;
		 }
	      }
	    else
	      {
	       Ale_ShowAlert (Ale_ERROR,Txt_Error_getting_data_from_a_recipient);
	       Messages->Rcv.NumErrors++;
	      }
	   }

      /***** Free memory used for user's data *****/
      Usr_UsrDataDestructor (&UsrDstData);

   /***** Free image *****/
   Med_MediaDestructor (&Media);
  }

/*****************************************************************************/
/***************** Request deletion of all received messages *****************/
/*****************************************************************************/

void Msg_ReqDelAllRecMsgs (void)
  {
   extern const char *Txt_Do_you_really_want_to_delete_the_unread_messages_received_from_USER_X_from_COURSE_Y_related_to_CONTENT_Z;
   extern const char *Txt_any_user;
   extern const char *Txt_Do_you_really_want_to_delete_all_messages_received_from_USER_X_from_COURSE_Y_related_to_CONTENT_Z;
   extern const char *Txt_Do_you_really_want_to_delete_the_unread_messages_received_from_USER_X_from_COURSE_Y;
   extern const char *Txt_Do_you_really_want_to_delete_all_messages_received_from_USER_X_from_COURSE_Y;
   struct Msg_Messages Messages;

   /***** Reset messages context *****/
   Msg_ResetMessages (&Messages);

   /***** Get parameters *****/
   Msg_GetParMsgsCrsCod (&Messages);
   Msg_GetParFilterFromTo (&Messages);
   Msg_GetParFilterContent (&Messages);
   Messages.ShowOnlyUnreadMsgs = Msg_GetParOnlyUnreadMsgs ();

   /***** Show question and button to remove messages received *****/
   /* Begin alert */
   if (Messages.FilterContent[0])
     {
      if (Messages.ShowOnlyUnreadMsgs)
         Ale_ShowAlertAndButtonBegin (Ale_QUESTION,Txt_Do_you_really_want_to_delete_the_unread_messages_received_from_USER_X_from_COURSE_Y_related_to_CONTENT_Z,
				      Messages.FilterFromTo[0] ? Messages.FilterFromTo :
								 Txt_any_user,
				      Messages.FilterCrsShrtName,Messages.FilterContent);
      else
         Ale_ShowAlertAndButtonBegin (Ale_QUESTION,Txt_Do_you_really_want_to_delete_all_messages_received_from_USER_X_from_COURSE_Y_related_to_CONTENT_Z,
				      Messages.FilterFromTo[0] ? Messages.FilterFromTo :
								 Txt_any_user,
				      Messages.FilterCrsShrtName,Messages.FilterContent);
     }
   else
     {
      if (Messages.ShowOnlyUnreadMsgs)
         Ale_ShowAlertAndButtonBegin (Ale_QUESTION,Txt_Do_you_really_want_to_delete_the_unread_messages_received_from_USER_X_from_COURSE_Y,
				      Messages.FilterFromTo[0] ? Messages.FilterFromTo :
								 Txt_any_user,
				      Messages.FilterCrsShrtName);
      else
         Ale_ShowAlertAndButtonBegin (Ale_QUESTION,Txt_Do_you_really_want_to_delete_all_messages_received_from_USER_X_from_COURSE_Y,
				      Messages.FilterFromTo[0] ? Messages.FilterFromTo :
								 Txt_any_user,
				      Messages.FilterCrsShrtName);
     }

   /* Show received messages again */
   Messages.TypeOfMessages = Msg_RECEIVED;
   Msg_ShowSntOrRcvMessages (&Messages);

   /* End alert */
   Ale_ShowAlertAndButtonEnd (ActDelAllRcvMsg,NULL,NULL,
                              Msg_PutParsMsgsFilters,&Messages,
                              Btn_REMOVE);
  }

/*****************************************************************************/
/******************* Request deletion of all sent messages *******************/
/*****************************************************************************/

void Msg_ReqDelAllSntMsgs (void)
  {
   extern const char *Txt_Do_you_really_want_to_delete_all_messages_sent_to_USER_X_from_COURSE_Y_related_to_CONTENT_Z;
   extern const char *Txt_any_user;
   extern const char *Txt_Do_you_really_want_to_delete_all_messages_sent_to_USER_X_from_COURSE_Y;
   struct Msg_Messages Messages;

   /***** Reset messages context *****/
   Msg_ResetMessages (&Messages);

   /***** Get parameters *****/
   Msg_GetParMsgsCrsCod (&Messages);
   Msg_GetParFilterFromTo (&Messages);
   Msg_GetParFilterContent (&Messages);

   /***** Show question and button to remove messages received *****/
   /* Begin alert */
   if (Messages.FilterContent[0])
      Ale_ShowAlertAndButtonBegin (Ale_QUESTION,Txt_Do_you_really_want_to_delete_all_messages_sent_to_USER_X_from_COURSE_Y_related_to_CONTENT_Z,
			           Messages.FilterFromTo[0] ? Messages.FilterFromTo :
							      Txt_any_user,
			           Messages.FilterCrsShrtName,Messages.FilterContent);
   else
      Ale_ShowAlertAndButtonBegin (Ale_QUESTION,Txt_Do_you_really_want_to_delete_all_messages_sent_to_USER_X_from_COURSE_Y,
			           Messages.FilterFromTo[0] ? Messages.FilterFromTo :
							      Txt_any_user,
			           Messages.FilterCrsShrtName);

   /* Show sent messages again */
   Messages.TypeOfMessages = Msg_SENT;
   Msg_ShowSntOrRcvMessages (&Messages);

   /* End alert */
   Ale_ShowAlertAndButtonEnd (ActDelAllSntMsg,NULL,NULL,
                              Msg_PutParsMsgsFilters,&Messages,
                              Btn_REMOVE);
  }

/*****************************************************************************/
/*********************** Delete all received messages ************************/
/*****************************************************************************/

void Msg_DelAllRecMsgs (void)
  {
   struct Msg_Messages Messages;
   char FilterFromToSubquery[Msg_DB_MAX_BYTES_MESSAGES_QUERY + 1];
   unsigned long NumMsgs;

   /***** Reset messages context *****/
   Msg_ResetMessages (&Messages);

   /***** Get parameters *****/
   Msg_GetParMsgsCrsCod (&Messages);
   Msg_GetParFilterFromTo (&Messages);
   Msg_GetParFilterContent (&Messages);
   Messages.ShowOnlyUnreadMsgs = Msg_GetParOnlyUnreadMsgs ();
   Msg_DB_MakeFilterFromToSubquery (&Messages,FilterFromToSubquery);

   /***** Delete messages *****/
   Messages.TypeOfMessages = Msg_RECEIVED;
   NumMsgs = Msg_RemoveSomeRecOrSntMsgsUsr (&Messages,
                                            Gbl.Usrs.Me.UsrDat.UsrCod,
                                            FilterFromToSubquery);
   Msg_ShowNumMsgsDeleted (NumMsgs);
   Msg_ShowRecMsgs ();
  }

/*****************************************************************************/
/************************* Delete all sent messages **************************/
/*****************************************************************************/

void Msg_DelAllSntMsgs (void)
  {
   struct Msg_Messages Messages;
   char FilterFromToSubquery[Msg_DB_MAX_BYTES_MESSAGES_QUERY + 1];
   unsigned long NumMsgs;

   /***** Reset messages context *****/
   Msg_ResetMessages (&Messages);

   /***** Get parameters *****/
   Msg_GetParMsgsCrsCod (&Messages);
   Msg_GetParFilterFromTo (&Messages);
   Msg_GetParFilterContent (&Messages);
   Msg_DB_MakeFilterFromToSubquery (&Messages,FilterFromToSubquery);

   /***** Delete messages *****/
   Messages.TypeOfMessages = Msg_SENT;
   NumMsgs = Msg_RemoveSomeRecOrSntMsgsUsr (&Messages,
                                            Gbl.Usrs.Me.UsrDat.UsrCod,
                                            FilterFromToSubquery);
   Msg_ShowNumMsgsDeleted (NumMsgs);
   Msg_ShowSntMsgs ();
  }

/*****************************************************************************/
/************* Write number of messages that have been deleted ***************/
/*****************************************************************************/

static void Msg_ShowNumMsgsDeleted (unsigned NumMsgs)
  {
   extern const char *Txt_One_message_has_been_deleted;
   extern const char *Txt_X_messages_have_been_deleted;

   if (NumMsgs == 1)
      Ale_ShowAlert (Ale_SUCCESS,Txt_One_message_has_been_deleted);
   else
      Ale_ShowAlert (Ale_SUCCESS,Txt_X_messages_have_been_deleted,
                     NumMsgs);
  }

/*****************************************************************************/
/**************** Get parameter with course origin of messages ***************/
/*****************************************************************************/

static void Msg_GetParMsgsCrsCod (struct Msg_Messages *Messages)
  {
   extern Err_SuccessOrError_t (*Hie_GetDataByCod[Hie_NUM_LEVELS]) (struct Hie_Node *Node);
   extern const char *Txt_any_course;
   struct Hie_Node Crs;
   __attribute__((unused)) Err_SuccessOrError_t SuccessOrError;

   if ((Messages->FilterCrsCod = ParCod_GetPar (ParCod_OthCrs)) > 0)	// If origin course specified
     {
      /* Get data of course */
      Crs.HieCod = Messages->FilterCrsCod;
      SuccessOrError = Hie_GetDataByCod[Hie_CRS] (&Crs);
     }

   Str_Copy (Messages->FilterCrsShrtName,Messages->FilterCrsCod > 0 ? Crs.ShrtName :
								      Txt_any_course,
	     sizeof (Messages->FilterCrsShrtName) - 1);
  }

/*****************************************************************************/
/******************* Get parameter with "from"/"to" filter ********************/
/*****************************************************************************/

static void Msg_GetParFilterFromTo (struct Msg_Messages *Messages)
  {
   /***** Get "from"/"to" filter *****/
   Par_GetParText ("FilterFromTo",Messages->FilterFromTo,
                     Usr_MAX_BYTES_FULL_NAME);
  }

/*****************************************************************************/
/********************* Get parameter with content filter *********************/
/*****************************************************************************/

static void Msg_GetParFilterContent (struct Msg_Messages *Messages)
  {
   /***** Get content filter *****/
   Par_GetParText ("FilterContent",Messages->FilterContent,
                     Msg_MAX_BYTES_FILTER_CONTENT);
  }

/*****************************************************************************/
/****************************** Delete a sent message ************************/
/*****************************************************************************/

void Msg_DelSntMsg (void)
  {
   extern const char *Txt_Message_deleted;
   long MsgCod;

   /***** Get the code of the message to delete *****/
   MsgCod = ParCod_GetAndCheckPar (ParCod_Msg);

   /***** Delete the message *****/
   Msg_DB_RemoveSntMsg (MsgCod);
   Ale_ShowAlert (Ale_SUCCESS,Txt_Message_deleted);

   /***** Show the remaining messages *****/
   Msg_ShowSntMsgs ();
  }

/*****************************************************************************/
/************************* Delete a received message *************************/
/*****************************************************************************/

void Msg_DelRecMsg (void)
  {
   extern const char *Txt_Message_deleted;
   long MsgCod;

   /***** Get the code of the message to delete *****/
   MsgCod = ParCod_GetAndCheckPar (ParCod_Msg);

   /***** Delete the message *****/
   Msg_RemoveRcvMsg (MsgCod,Gbl.Usrs.Me.UsrDat.UsrCod);
   Ale_ShowAlert (Ale_SUCCESS,Txt_Message_deleted);

   /***** Show the remaining messages *****/
   Msg_ShowRecMsgs ();
  }

/*****************************************************************************/
/**************************** Expand a sent message **************************/
/*****************************************************************************/

void Msg_ExpSntMsg (void)
  {
   struct Msg_Messages Messages;

   /***** Reset messages context *****/
   Msg_ResetMessages (&Messages);

   /***** Get the code of the message to expand *****/
   Messages.ExpandedMsgCod = ParCod_GetAndCheckPar (ParCod_Msg);

   /***** Expand the message *****/
   Msg_DB_ExpandSntMsg (Messages.ExpandedMsgCod);

   /***** Show again the messages *****/
   Msg_ShowSntMsgs ();
  }

/*****************************************************************************/
/************************** Expand a received message ************************/
/*****************************************************************************/

void Msg_ExpRecMsg (void)
  {
   struct Msg_Messages Messages;

   /***** Reset messages context *****/
   Msg_ResetMessages (&Messages);

   /***** Get the code of the message to expand *****/
   Messages.ExpandedMsgCod = ParCod_GetAndCheckPar (ParCod_Msg);

   /***** Expand the message *****/
   Msg_DB_ExpandRcvMsg (Messages.ExpandedMsgCod);

   /***** Mark possible notification as seen *****/
   Ntf_DB_MarkNotifAsSeenUsingCod (Ntf_EVENT_MESSAGE,Messages.ExpandedMsgCod);

   /***** Show again the messages *****/
   Msg_ShowRecMsgs ();
  }

/*****************************************************************************/
/*************************** Contract a sent message *************************/
/*****************************************************************************/

void Msg_ConSntMsg (void)
  {
   long MsgCod;

   /***** Get the code of the message to contract *****/
   MsgCod = ParCod_GetAndCheckPar (ParCod_Msg);

   /***** Contract the message *****/
   Msg_DB_ContractSntMsg (MsgCod);

   /***** Show again the messages *****/
   Msg_ShowSntMsgs ();
  }

/*****************************************************************************/
/************************* Contract a received message ***********************/
/*****************************************************************************/

void Msg_ConRecMsg (void)
  {
   long MsgCod;

   /***** Get the code of the message to contract *****/
   MsgCod = ParCod_GetAndCheckPar (ParCod_Msg);

   /***** Contract the message *****/
   Msg_DB_ContractRcvMsg (MsgCod);

   /***** Show again the messages *****/
   Msg_ShowRecMsgs ();
  }

/*****************************************************************************/
/********************** Insert a message in the database *********************/
/*****************************************************************************/
// Return the code of the new inserted message

static long Msg_InsertNewMsg (const char *Subject,const char *Content,
                              struct Med_Media *Media)
  {
   long MsgCod;

   /***** Store media in filesystem and database *****/
   Med_RemoveKeepOrStoreMedia (-1L,Media);

   /***** Insert message subject and content in the database *****/
   MsgCod = Msg_DB_CreateNewMsg (Subject,Content,Media->MedCod);

   /***** Insert message in sent messages *****/
   Msg_DB_CreateSntMsg (MsgCod,Gbl.Hierarchy.Node[Hie_CRS].HieCod);

   /***** Increment number of messages sent by me *****/
   Prf_DB_IncrementNumMsgSntUsr (Gbl.Usrs.Me.UsrDat.UsrCod);

   return MsgCod;
  }

/*****************************************************************************/
/************** Delete some received or sent messages of a user **************/
/*****************************************************************************/

static unsigned long Msg_RemoveSomeRecOrSntMsgsUsr (const struct Msg_Messages *Messages,
                                                    long UsrCod,
                                                    const char *FilterFromToSubquery)
  {
   MYSQL_RES *mysql_res;
   unsigned NumMsgs;
   unsigned NumMsg;
   long MsgCod;

   /***** Get some of the messages received or sent by this user from database *****/
   NumMsgs = Msg_DB_GetSntOrRcvMsgs (&mysql_res,
                                     Messages,UsrCod,FilterFromToSubquery);

   /***** Delete each message *****/
   for (NumMsg = 0;
	NumMsg < NumMsgs;
	NumMsg++)
     {
      if ((MsgCod = DB_GetNextCode (mysql_res)) <= 0)
         Err_WrongMessageExit ();

      switch (Messages->TypeOfMessages)
        {
         case Msg_RECEIVED:
            Msg_RemoveRcvMsg (MsgCod,UsrCod);
            break;
         case Msg_SENT:
            Msg_DB_RemoveSntMsg (MsgCod);
            break;
         default:
            break;
        }
     }

   /***** Free the MySQL result *****/
   DB_FreeMySQLResult (&mysql_res);

   return NumMsgs;
  }

/*****************************************************************************/
/************************* Remove a received message *************************/
/*****************************************************************************/

static void Msg_RemoveRcvMsg (long MsgCod,long UsrCod)
  {
   /***** Delete message from msg_rcv *****/
   Msg_DB_RemoveRcvMsg (MsgCod,UsrCod);

   /***** Mark possible notifications as removed *****/
   Ntf_DB_MarkNotifToOneUsrAsRemoved (Ntf_EVENT_MESSAGE,MsgCod,UsrCod);
  }

/*****************************************************************************/
/********************* Show messages sent to other users *********************/
/*****************************************************************************/

void Msg_ShowSntMsgs (void)
  {
   struct Msg_Messages Messages;

   /***** Reset messages context *****/
   Msg_ResetMessages (&Messages);

   /***** Show the sent messages *****/
   Messages.TypeOfMessages = Msg_SENT;
   Msg_ShowSntOrRcvMessages (&Messages);
  }

/*****************************************************************************/
/******************* Show messages received from other users *****************/
/*****************************************************************************/

void Msg_ShowRecMsgs (void)
  {
   struct Msg_Messages Messages;

   /***** Reset messages context *****/
   Msg_ResetMessages (&Messages);

   if (Msg_DB_GetNumUsrsBannedBy (Gbl.Usrs.Me.UsrDat.UsrCod))
     {
      /***** Contextual menu *****/
      Mnu_ContextMenuBegin ();
	 Msg_PutLinkToViewBannedUsers ();	// View banned users
      Mnu_ContextMenuEnd ();
     }

   /***** Show the received messages *****/
   Messages.TypeOfMessages = Msg_RECEIVED;
   Msg_ShowSntOrRcvMessages (&Messages);
  }

/*****************************************************************************/
/************************ Show sent or received messages *********************/
/*****************************************************************************/

static void Msg_ShowSntOrRcvMessages (struct Msg_Messages *Messages)
  {
   extern const char *Hlp_COMMUNICATION_Messages_received;
   extern const char *Hlp_COMMUNICATION_Messages_sent;
   extern const char *Txt_Filters;
   extern const char *Txt_Update_messages;
   char FilterFromToSubquery[Msg_DB_MAX_BYTES_MESSAGES_QUERY + 1];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumMsg;
   char *NumMsgsStr;
   unsigned NumUnreadMsgs;
   struct Pag_Pagination Pagination;
   long MsgCod;
   static Act_Action_t ActionSee[Msg_NUM_TYPES_OF_MSGS] =
     {
      [Msg_WRITING ] = ActUnk,
      [Msg_RECEIVED] = ActSeeRcvMsg,
      [Msg_SENT    ] = ActSeeSntMsg,
     };
   static Pag_WhatPaginate_t WhatPaginate[Msg_NUM_TYPES_OF_MSGS] =
     {
      [Msg_WRITING ] = Pag_NONE,
      [Msg_RECEIVED] = Pag_MESSAGES_RECEIVED,
      [Msg_SENT    ] = Pag_MESSAGES_SENT,
     };
   const char *Help[Msg_NUM_TYPES_OF_MSGS] =
     {
      [Msg_WRITING ] = NULL,
      [Msg_RECEIVED] = Hlp_COMMUNICATION_Messages_received,
      [Msg_SENT    ] = Hlp_COMMUNICATION_Messages_sent,
     };

   /***** Get the page number *****/
   Messages->CurrentPage = Pag_GetParPagNum (WhatPaginate[Messages->TypeOfMessages]);

   /***** Get other parameters *****/
   Msg_GetParMsgsCrsCod (Messages);
   Msg_GetParFilterFromTo (Messages);
   Msg_GetParFilterContent (Messages);
   Msg_DB_MakeFilterFromToSubquery (Messages,FilterFromToSubquery);

   /***** Get number of unread messages *****/
   switch (Messages->TypeOfMessages)
     {
      case Msg_RECEIVED:
         Messages->ShowOnlyUnreadMsgs = Msg_GetParOnlyUnreadMsgs ();
         NumUnreadMsgs = Msg_DB_GetNumUnreadMsgs (Messages,
                                                  FilterFromToSubquery);
         break;
      case Msg_SENT:
      default:
         NumUnreadMsgs = 0;
         break;
     }

   /***** Get messages from database *****/
   Messages->NumMsgs = Msg_DB_GetSntOrRcvMsgs (&mysql_res,
                                               Messages,Gbl.Usrs.Me.UsrDat.UsrCod,
				               FilterFromToSubquery);

   /***** Begin box with messages *****/
   Msg_SetNumMsgsStr (Messages,&NumMsgsStr,NumUnreadMsgs);
   Box_BoxBegin (NumMsgsStr,Msg_PutIconsListMsgs,Messages,
                 Help[Messages->TypeOfMessages],Box_NOT_CLOSABLE);
   free (NumMsgsStr);

      /***** Filters for messages *****/
      HTM_FIELDSET_Begin (NULL);
	 HTM_LEGEND (Txt_Filters);

	 /* Form to see messages again */
	 Frm_BeginForm (ActionSee[Messages->TypeOfMessages]);

	    /* Filters */
	    HTM_TABLE_BeginCenterPadding (2);

	       Msg_ShowFormSelectCourseSentOrRecMsgs (Messages);
	       if (Messages->TypeOfMessages == Msg_RECEIVED)
		  Msg_ShowFormToShowOnlyUnreadMessages (Messages);

	       Msg_ShowFormToFilterMsgs (Messages);

	    HTM_TABLE_End ();

	    /* Put button to refresh */
	    Lay_WriteLinkToUpdate (Txt_Update_messages,"CopyMessageToHiddenFields();");

	 Frm_EndForm ();

      HTM_FIELDSET_End ();

      if (Messages->NumMsgs)		// If there are messages...
	{
	 if (Gbl.Action.Act == ActExpRcvMsg)	// Expanding a message, perhaps it is the result of following a link
						   // from a notification of received message, so show the page where the message is inside
	    /***** Get the page where the expanded message is inside *****/
	    for (NumMsg = 0;
		 NumMsg < Messages->NumMsgs;
		 NumMsg++)
	      {
	       row = mysql_fetch_row (mysql_res);
	       if (sscanf (row[0],"%ld",&MsgCod) != 1)
		  Err_WrongMessageExit ();

	       if (MsgCod == Messages->ExpandedMsgCod)	// Expanded message found
		 {
		  Messages->CurrentPage = NumMsg / Pag_ITEMS_PER_PAGE + 1;
		  break;
		 }
	      }

	 /***** Compute variables related to pagination *****/
	 Pagination.NumItems = Messages->NumMsgs;
	 Pagination.CurrentPage = Messages->CurrentPage;
	 Pag_CalculatePagination (&Pagination);
	 Messages->CurrentPage = Pagination.CurrentPage;

	 /***** Save my current page in order to show it next time I'll view my received/sent messages *****/
	 Ses_DB_SaveLastPageMsgIntoSession (WhatPaginate[Messages->TypeOfMessages],
					    Messages->CurrentPage);

	 /***** Write links to pages *****/
	 Pag_WriteLinksToPagesCentered (WhatPaginate[Messages->TypeOfMessages],&Pagination,
					Messages,-1L);

	 /***** Show received / sent messages in this page *****/
	 HTM_TABLE_BeginWidePadding (2);

	    mysql_data_seek (mysql_res,(my_ulonglong) (Pagination.FirstItemVisible - 1));
	    for (NumMsg  = Pagination.FirstItemVisible;
		 NumMsg <= Pagination.LastItemVisible;
		 NumMsg++)
	      {
	       row = mysql_fetch_row (mysql_res);

	       if (sscanf (row[0],"%ld",&MsgCod) != 1)
		  Err_WrongMessageExit ();
	       Msg_ShowASentOrReceivedMessage (Messages,
	                                       Messages->NumMsgs - NumMsg + 1,
	                                       MsgCod);
	      }

	 HTM_TABLE_End ();

	 /***** Write again links to pages *****/
	 Pag_WriteLinksToPagesCentered (WhatPaginate[Messages->TypeOfMessages],&Pagination,
					Messages,-1L);
	}

   /***** End box *****/
   Box_BoxEnd ();

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/****************** Put a link (form) to view banned users *******************/
/*****************************************************************************/

static void Msg_PutLinkToViewBannedUsers(void)
  {
   extern const char *Txt_Banned_users;

   Lay_PutContextualLinkIconText (ActLstBanUsr,NULL,
                                  NULL,NULL,
				  "user-lock.svg",Ico_RED,
				  Txt_Banned_users,NULL);
  }

/*****************************************************************************/
/***** Set string with number of messages and number of unread messages ******/
/*****************************************************************************/
// The string must be deallocated after calling this function

static void Msg_SetNumMsgsStr (const struct Msg_Messages *Messages,
                               char **NumMsgsStr,unsigned NumUnreadMsgs)
  {
   extern const char *Txt_message_received;
   extern const char *Txt_message_sent;
   extern const char *Txt_messages_received;
   extern const char *Txt_messages_sent;
   extern const char *Txt_unread_MESSAGE;
   extern const char *Txt_unread_MESSAGES;

   switch (Messages->TypeOfMessages)
     {
      case Msg_RECEIVED:
	 if (Messages->NumMsgs == 1)
	   {
	    if (NumUnreadMsgs)
	      {
	       if (asprintf (NumMsgsStr,"1 %s, 1 %s",
			     Txt_message_received,Txt_unread_MESSAGE) < 0)
                  Err_NotEnoughMemoryExit ();
	      }
	    else
	      {
	       if (asprintf (NumMsgsStr,"1 %s",Txt_message_received) < 0)
                  Err_NotEnoughMemoryExit ();
	      }
	   }
	 else
	   {
	    if (NumUnreadMsgs == 0)
	      {
	       if (asprintf (NumMsgsStr,"%u %s",
			     Messages->NumMsgs,Txt_messages_received) < 0)
                  Err_NotEnoughMemoryExit ();
	      }
	    else if (NumUnreadMsgs == 1)
	      {
	       if (asprintf (NumMsgsStr,"%u %s, 1 %s",
			     Messages->NumMsgs,Txt_messages_received,
			     Txt_unread_MESSAGE) < 0)
                  Err_NotEnoughMemoryExit ();
	      }
	    else
	      {
	       if (asprintf (NumMsgsStr,"%u %s, %u %s",
			     Messages->NumMsgs,Txt_messages_received,
			     NumUnreadMsgs,Txt_unread_MESSAGES) < 0)
                  Err_NotEnoughMemoryExit ();
	      }
	   }
	 break;
      case Msg_SENT:
	 if (Messages->NumMsgs == 1)
	   {
	    if (asprintf (NumMsgsStr,"1 %s",Txt_message_sent) < 0)
               Err_NotEnoughMemoryExit ();
	   }
	 else
	   {
	    if (asprintf (NumMsgsStr,"%u %s",
			  Messages->NumMsgs,Txt_messages_sent) < 0)
               Err_NotEnoughMemoryExit ();
	   }
	 break;
      default:
	 break;
     }
  }

/*****************************************************************************/
/***************** Put contextual icons in list of messages ******************/
/*****************************************************************************/

static void Msg_PutIconsListMsgs (void *Messages)
  {
   static Act_Action_t ActionReqDelAllMsg[Msg_NUM_TYPES_OF_MSGS] =
     {
      [Msg_WRITING ] = ActUnk,
      [Msg_RECEIVED] = ActReqDelAllRcvMsg,
      [Msg_SENT    ] = ActReqDelAllSntMsg,
     };

   if (Messages)
     {
      /***** Put icon to write a new message *****/
      switch (((struct Msg_Messages *) Messages)->TypeOfMessages)
        {
	 case Msg_RECEIVED:
	 case Msg_SENT:
	    Lay_PutContextualLinkOnlyIcon (ActReqMsgUsr,NULL,
					   Msg_PutParsMsgsFilters,Messages,
					   "marker.svg",Ico_BLACK);
	    break;
	 default:
	    break;
        }

      /***** Put icon to see received messages *****/
      switch (((struct Msg_Messages *) Messages)->TypeOfMessages)
        {
	 case Msg_WRITING:
	 case Msg_SENT:
	    Lay_PutContextualLinkOnlyIcon (ActSeeRcvMsg,NULL,
					   Msg_PutParsMsgsFilters,Messages,
					   "inbox.svg",Ico_BLACK);
	    break;
	 default:
	    break;
        }

      /***** Put icon to see sent messages *****/
      switch (((struct Msg_Messages *) Messages)->TypeOfMessages)
        {
	 case Msg_WRITING:
	 case Msg_RECEIVED:
	    Lay_PutContextualLinkOnlyIcon (ActSeeSntMsg,NULL,
					   Msg_PutParsMsgsFilters,Messages,
					   "share.svg",Ico_BLACK);
	    break;
	 default:
	    break;
        }

      /***** Put icon to remove messages *****/
      switch (((struct Msg_Messages *) Messages)->TypeOfMessages)
        {
	 case Msg_RECEIVED:
	 case Msg_SENT:
	    Ico_PutContextualIconToRemove (ActionReqDelAllMsg[((struct Msg_Messages *) Messages)->TypeOfMessages],NULL,
					   Msg_PutParsMsgsFilters,Messages);
	    break;
	 default:
	    break;
        }

      /***** Put icon to show a figure *****/
      Fig_PutIconToShowFigure (Fig_MESSAGES);
     }
  }

/*****************************************************************************/
/******* Put hidden parameters to expand, contract or delete a message *******/
/*****************************************************************************/

static void Msg_PutParsOneMsg (void *Messages)
  {
   if (Messages)
     {
      Pag_PutParPagNum (Msg_WhatPaginate[((struct Msg_Messages *) Messages)->TypeOfMessages],
			((struct Msg_Messages *) Messages)->CurrentPage);
      ParCod_PutPar (ParCod_Msg,((struct Msg_Messages *) Messages)->MsgCod);
      Msg_PutParsMsgsFilters (Messages);
     }
  }

/*****************************************************************************/
/****************** Put hidden parameters with filters ***********************/
/*****************************************************************************/

void Msg_PutParsMsgsFilters (void *Messages)
  {
   if (Messages)
     {
      if (((struct Msg_Messages *) Messages)->FilterCrsCod >= 0)
	 ParCod_PutPar (ParCod_OthCrs,((struct Msg_Messages *) Messages)->FilterCrsCod);

      if (((struct Msg_Messages *) Messages)->FilterFromTo[0])
	 Par_PutParString (NULL,"FilterFromTo",((struct Msg_Messages *) Messages)->FilterFromTo);

      if (((struct Msg_Messages *) Messages)->FilterContent[0])
	 Par_PutParString (NULL,"FilterContent",((struct Msg_Messages *) Messages)->FilterContent);

      if (((struct Msg_Messages *) Messages)->ShowOnlyUnreadMsgs)
	 Par_PutParChar ("OnlyUnreadMsgs",'Y');
     }
  }

/*****************************************************************************/
/********* Show form to select course for sent or received messages **********/
/*****************************************************************************/

static void Msg_ShowFormSelectCourseSentOrRecMsgs (const struct Msg_Messages *Messages)
  {
   extern const char *Txt_MSG_From_COURSE;
   extern const char *Txt_any_course;
   static unsigned (*GetDistinctCrssInMyRcvMsgs[Msg_NUM_TYPES_OF_MSGS]) (MYSQL_RES **mysql_res) =
     {
      [Msg_WRITING ] = NULL,
      [Msg_RECEIVED] = Msg_DB_GetDistinctCrssInMyRcvMsgs,
      [Msg_SENT    ] = Msg_DB_GetDistinctCrssInMySntMsgs,
     };
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumCrss = 0;	// Initialized to avoid warning
   unsigned NumCrs;
   long CrsCod;

   /***** Get distinct courses in my messages *****/
   if (GetDistinctCrssInMyRcvMsgs[Messages->TypeOfMessages])
      NumCrss = GetDistinctCrssInMyRcvMsgs[Messages->TypeOfMessages] (&mysql_res);

   /***** Course selection *****/
   HTM_TR_Begin (NULL);
      Frm_LabelColumn ("Frm_C1 RT","",Txt_MSG_From_COURSE);
      HTM_TD_Begin ("class=\"Frm_C2 LT\"");
	 HTM_LABEL_Begin ("class=\"FORM_IN_%s\"",The_GetSuffix ());
	    HTM_SELECT_Begin (HTM_NO_ATTR,NULL,
			      "name=\"FilterCrsCod\" class=\"Frm_C2 INPUT_%s\"",
			      The_GetSuffix ());

	       /* Write a first option to select any course */
	       HTM_OPTION (HTM_Type_STRING,"",
			   (Messages->FilterCrsCod < 0) ? HTM_SELECTED :
							  HTM_NO_ATTR,
			   "%s",Txt_any_course);

	       /* Write an option for each origin course */
	       for (NumCrs = 0;
		    NumCrs < NumCrss;
		    NumCrs++)
		 {
		  /* Get next course */
		  row = mysql_fetch_row (mysql_res);

		  if ((CrsCod = Str_ConvertStrCodToLongCod (row[0])) > 0)
		     HTM_OPTION (HTM_Type_LONG,&CrsCod,
				 (CrsCod == Messages->FilterCrsCod) ? HTM_SELECTED :
								      HTM_NO_ATTR,
				 "%s",row[1]);	// Course short name
		 }

	    HTM_SELECT_End ();
	 HTM_LABEL_End ();
      HTM_TD_End ();
   HTM_TR_End ();

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/***** Show form to filter "from" and "to" for received or sent messages *****/
/*****************************************************************************/

static void Msg_ShowFormToFilterMsgs (const struct Msg_Messages *Messages)
  {
   extern const char *Txt_MSG_From_USER;
   extern const char *Txt_MSG_To;
   extern const char *Txt_MSG_Content;
   static const char **TxtFromTo[Msg_NUM_TYPES_OF_MSGS] =
     {
      [Msg_WRITING ] = NULL,
      [Msg_RECEIVED] = &Txt_MSG_From_USER,
      [Msg_SENT    ] = &Txt_MSG_To
     };

   /***** Filter authors/recipients *****/
   HTM_TR_Begin (NULL);
      Frm_LabelColumn ("Frm_C1 RT","",*TxtFromTo[Messages->TypeOfMessages]);
      HTM_TD_Begin ("class=\"Frm_C2 LT\"");
	 HTM_LABEL_Begin ("class=\"FORM_IN_%s\"",The_GetSuffix ());
	    HTM_INPUT_SEARCH ("FilterFromTo",Usr_MAX_CHARS_FIRSTNAME_OR_SURNAME * 3,
			      Messages->FilterFromTo,
			      "size=\"20\" class=\"Frm_C2 INPUT_%s\"",
			      The_GetSuffix ());
	 HTM_LABEL_End ();
      HTM_TD_End ();
   HTM_TR_End ();

   /***** Filter message content *****/
   HTM_TR_Begin (NULL);
      Frm_LabelColumn ("Frm_C1 RT","",Txt_MSG_Content);
      HTM_TD_Begin ("class=\"LM\"");
	 HTM_LABEL_Begin ("class=\"FORM_IN_%s\"",The_GetSuffix ());
	    HTM_INPUT_SEARCH ("FilterContent",Msg_MAX_CHARS_FILTER_CONTENT,
			      Messages->FilterContent,
			      "size=\"20\" class=\"Frm_C2 INPUT_%s\"",
			      The_GetSuffix ());
	 HTM_LABEL_End ();
      HTM_TD_End ();
   HTM_TR_End ();
  }

/*****************************************************************************/
/**** Show form to select whether to show only unread (received) messages ****/
/*****************************************************************************/

static void Msg_ShowFormToShowOnlyUnreadMessages (const struct Msg_Messages *Messages)
  {
   extern const char *Txt_MSG_Unopened;
   extern const char *Txt_only_unopened_messages;

   /***** Put checkbox to select whether to show only unread (received) messages *****/
   HTM_TR_Begin (NULL);
      Frm_LabelColumn ("Frm_C1 RT","",Txt_MSG_Unopened);
      HTM_TD_Begin ("class=\"Frm_C2 LT\"");
	 HTM_LABEL_Begin ("class=\"FORM_IN_%s\"",The_GetSuffix ());
	    HTM_INPUT_CHECKBOX ("OnlyUnreadMsgs",
				Messages->ShowOnlyUnreadMsgs ? HTM_CHECKED :
							       HTM_NO_ATTR,
				"value=\"Y\"");
	    HTM_Txt (Txt_only_unopened_messages);
	 HTM_LABEL_End ();
      HTM_TD_End ();
   HTM_TR_End ();
  }

/*****************************************************************************/
/*********** Get parameter to show only unread (received) messages ***********/
/*****************************************************************************/

static bool Msg_GetParOnlyUnreadMsgs (void)
  {
   return Par_GetParBool ("OnlyUnreadMsgs");
  }

/*****************************************************************************/
/***************************** Get data of a message *************************/
/*****************************************************************************/

static void Msg_GetMsgSntData (long MsgCod,long *CrsCod,long *UsrCod,
                               time_t *CreatTimeUTC,
                               char Subject[Cns_MAX_BYTES_SUBJECT + 1],
                               bool *Deleted)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;

   /***** Get data of message from table msg_snt *****/
   /* Result should have a unique row */
   if (Msg_DB_GetMsgSntData (&mysql_res,MsgCod,Deleted) != 1)
      Err_WrongMessageExit ();

   /* Get number of rows */
   row = mysql_fetch_row (mysql_res);
   /*
   row[0]: CrsCod
   row[1]: UsrCod
   row[2]: UNIX_TIMESTAMP(CreatTime)
   */
   /* Get course (row[0]) and author (row[1]) */
   *CrsCod = Str_ConvertStrCodToLongCod (row[0]);
   *UsrCod = Str_ConvertStrCodToLongCod (row[1]);

   /* Get creation time (row[2]) */
   *CreatTimeUTC = Dat_GetUNIXTimeFromStr (row[2]);

   /* Free structure that stores the query result */
   DB_FreeMySQLResult (&mysql_res);

   /***** Get subject of message from database *****/
   Msg_DB_GetMsgSubject (MsgCod,Subject);
  }

/*****************************************************************************/
/*************** Get content and optional image of a message *****************/
/*****************************************************************************/

static void Msg_GetMsgContent (long MsgCod,
                               char Content[Cns_MAX_BYTES_LONG_TEXT + 1],
                               struct Med_Media *Media)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;

   /***** Get content of message from database *****/
   if (Msg_DB_GetMsgContent (&mysql_res,MsgCod) == Exi_DOES_NOT_EXIST)
      Err_WrongMessageExit ();

   /***** Get number of rows *****/
   row = mysql_fetch_row (mysql_res);
   /*
   row[0]: Content
   row[1]: MedCod
   */
   /****** Get content (row[0]) *****/
   Str_Copy (Content,row[0],Cns_MAX_BYTES_LONG_TEXT);

   /***** Get media (row[1]) *****/
   Media->MedCod = Str_ConvertStrCodToLongCod (row[1]);
   Med_GetMediaDataByCod (Media);

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/******** Show a sent or a received message (from a user to another) *********/
/*****************************************************************************/

static void Msg_ShowASentOrReceivedMessage (struct Msg_Messages *Messages,
                                            long MsgNum,long MsgCod)
  {
   extern const char *Txt_MSG_Replied;
   extern const char *Txt_MSG_Not_replied;
   extern const char *Txt_MSG_Unopened;
   extern const char *Txt_MSG_Sent;
   extern const char *Txt_MSG_From_USER;
   extern const char *Txt_MSG_To;
   extern const char *Txt_MSG_Content;
   static Act_Action_t ActionDelMsg[Msg_NUM_TYPES_OF_MSGS] =
     {
      [Msg_WRITING ] = ActUnk,
      [Msg_RECEIVED] = ActDelRcvMsg,
      [Msg_SENT    ] = ActDelSntMsg,
     };
   static const char *Class[Msg_NUM_TYPES_OF_MSGS][CloOpe_NUM_CLOSED_OPEN] =
     {
      [Msg_RECEIVED][CloOpe_CLOSED] = "MSG_BG_NEW",
      [Msg_RECEIVED][CloOpe_OPEN  ] = "MSG_BG",
      [Msg_SENT    ][CloOpe_CLOSED] = "MSG_BG",
      [Msg_SENT    ][CloOpe_OPEN  ] = "MSG_BG",
     };
   static struct
     {
      const char *Icon;
      const char **Title;
     } Icons[Msg_NUM_TYPES_OF_MSGS][CloOpe_NUM_CLOSED_OPEN][2] =
     {
      [Msg_RECEIVED][CloOpe_CLOSED][false] = {"envelope.svg"	      ,&Txt_MSG_Unopened   },
      [Msg_RECEIVED][CloOpe_CLOSED][true ] = {"envelope.svg"	      ,&Txt_MSG_Unopened   },
      [Msg_RECEIVED][CloOpe_OPEN  ][false] = {"envelope-open-text.svg",&Txt_MSG_Not_replied},
      [Msg_RECEIVED][CloOpe_OPEN  ][true ] = {"reply.svg"	      ,&Txt_MSG_Replied    },
      [Msg_SENT    ][CloOpe_CLOSED][false] = {"share.svg"	      ,&Txt_MSG_Sent       },
      [Msg_SENT    ][CloOpe_CLOSED][true ] = {"share.svg"	      ,&Txt_MSG_Sent       },
      [Msg_SENT    ][CloOpe_OPEN  ][false] = {"share.svg"	      ,&Txt_MSG_Sent       },
      [Msg_SENT    ][CloOpe_OPEN  ][true ] = {"share.svg"	      ,&Txt_MSG_Sent       },
     };
   static const char *ClassAuthor[CloOpe_NUM_CLOSED_OPEN] =
     {
      [CloOpe_CLOSED] = "MSG_AUT_NEW",
      [CloOpe_OPEN  ] = "MSG_AUT",
     };
   static const char *ClassDateTime[CloOpe_NUM_CLOSED_OPEN] =
     {
      [CloOpe_CLOSED] = "MSG_TIT_NEW",
      [CloOpe_OPEN  ] = "MSG_TIT",
     };
   static const char *ClassBg[CloOpe_NUM_CLOSED_OPEN] =
     {
      [CloOpe_CLOSED] = "MSG_BG_NEW",
      [CloOpe_OPEN  ] = "MSG_BG",
     };
   struct Usr_Data UsrDat;
   bool FromThisCrs = false;	// Initialized to avoid warning
   time_t CreatTimeUTC;		// Creation time of a message
   long CrsCod;
   char Subject[Cns_MAX_BYTES_SUBJECT + 1];
   char Content[Cns_MAX_BYTES_LONG_TEXT + 1];
   struct Med_Media Media;
   bool Deleted;
   CloOpe_ClosedOrOpen_t ClosedOrOpen = CloOpe_OPEN;
   bool Replied = false;	// Initialized to avoid warning
   bool Expanded = false;

   /***** Initialize structure with user's data *****/
   Usr_UsrDataConstructor (&UsrDat);

   /***** Get data of message *****/
   Msg_GetMsgSntData (MsgCod,&CrsCod,&UsrDat.UsrCod,&CreatTimeUTC,Subject,&Deleted);
   switch (Messages->TypeOfMessages)
     {
      case Msg_RECEIVED:
         Msg_DB_GetStatusOfRcvMsg (MsgCod,&ClosedOrOpen,&Replied,&Expanded);
         break;
      case Msg_SENT:
         Expanded = Msg_DB_GetStatusOfSntMsg (MsgCod);
         break;
      default:
	 Err_WrongMessageExit ();
	 break;
     }

   /***** Put an icon with message status *****/
   HTM_TR_Begin (NULL);

      /***** Icons *****/
      HTM_TD_Begin ("class=\"CONTEXT_COL %s_%s\"",
		    Class[Messages->TypeOfMessages][ClosedOrOpen],
		    The_GetSuffix ());

         /* Type of message icon (envelope, reply...) */
	 Ico_PutIcon (Icons[Messages->TypeOfMessages][ClosedOrOpen][Replied].Icon,
		      Ico_BLACK,
		      *Icons[Messages->TypeOfMessages][ClosedOrOpen][Replied].Title,
		      "ICO16x16");

	 HTM_BR ();

	 /* Form to delete message */
	 Messages->MsgCod = MsgCod;	// Message to be deleted
	 Ico_PutContextualIconToRemove (ActionDelMsg[Messages->TypeOfMessages],NULL,
					Msg_PutParsOneMsg,Messages);
      HTM_TD_End ();

      /***** Number *****/
      Msg_WriteMsgNumber (MsgNum,ClosedOrOpen == CloOpe_CLOSED);

      /***** Author *****/
      HTM_TD_Begin ("class=\"LT %s_%s %s_%s\"",
                    ClassAuthor[ClosedOrOpen],The_GetSuffix (),
                    ClassBg[ClosedOrOpen],The_GetSuffix ());
	 Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat,
						  Usr_DONT_GET_PREFS,
						  Usr_DONT_GET_ROLE_IN_CRS);
	 Usr_WriteAuthor (&UsrDat,For_ENABLED);
      HTM_TD_End ();

      /***** Subject *****/
      Msg_WriteSentOrReceivedMsgSubject (Messages,MsgCod,Subject,ClosedOrOpen,Expanded);

      /***** Date-time *****/
      Msg_WriteMsgDate (CreatTimeUTC,
                        ClassDateTime[ClosedOrOpen],
                        ClassBg[ClosedOrOpen]);

   HTM_TR_End ();

   if (Expanded)
     {
      HTM_TR_Begin (NULL);

	 HTM_TD_Begin ("rowspan=\"3\" colspan=\"2\" class=\"LT\"");
	    HTM_TABLE_BeginPadding (2);

	       /***** Write course origin of message *****/
	       HTM_TR_Begin (NULL);
		  HTM_TD_Begin ("class=\"LM\"");
		     FromThisCrs = Msg_WriteCrsOrgMsg (CrsCod);
		  HTM_TD_End ();
	       HTM_TR_End ();

	       /***** Form to reply message *****/
	       HTM_TR_Begin (NULL);
		  HTM_TD_Begin ("class=\"LM\"");
		     if (Messages->TypeOfMessages == Msg_RECEIVED &&
			 Gbl.Usrs.Me.Role.Logged >= Rol_USR)
			// Guests (users without courses) can read messages but not reply them
			Msg_WriteFormToReply (MsgCod,CrsCod,FromThisCrs,&UsrDat);
		  HTM_TD_End ();
	       HTM_TR_End ();

	    HTM_TABLE_End ();
	 HTM_TD_End ();

	 /***** Write "From:" *****/
	 HTM_TD_Begin ("class=\"RT MSG_TIT_%s\"",The_GetSuffix ());
	    HTM_Txt (Txt_MSG_From_USER); HTM_Colon (); HTM_NBSP ();
	 HTM_TD_End ();

	 HTM_TD_Begin ("colspan=\"2\" class=\"LT\"");
	    Msg_WriteMsgFrom (Messages,&UsrDat,Deleted);
	 HTM_TD_End ();

      HTM_TR_End ();

      /***** Write "To:" *****/
      HTM_TR_Begin (NULL);

	 HTM_TD_Begin ("class=\"RT MSG_TIT_%s\"",The_GetSuffix ());
	    HTM_Txt (Txt_MSG_To); HTM_Colon (); HTM_NBSP ();
	 HTM_TD_End ();

	 HTM_TD_Begin ("colspan=\"2\" class=\"LT\"");
	    Msg_WriteMsgTo (Messages,MsgCod);
	 HTM_TD_End ();

      HTM_TR_End ();

      HTM_TR_Begin (NULL);

	 /***** Initialize media *****/
	 Med_MediaConstructor (&Media);

	 /***** Get message content and optional media *****/
	 Msg_GetMsgContent (MsgCod,Content,&Media);

	 /***** Write "Content:" *****/
	 HTM_TD_Begin ("class=\"RT MSG_TIT_%s\"",The_GetSuffix ());
	    HTM_Txt (Txt_MSG_Content);  HTM_Colon (); HTM_NBSP ();
	 HTM_TD_End ();

	 /***** Show content and media *****/
	 HTM_TD_Begin ("colspan=\"2\" class=\"LT MSG_TXT_%s\"",The_GetSuffix ());
	    if (Content[0])
	       Msg_WriteMsgContent (Content,true,false);
	    Med_ShowMedia (&Media,"MSG_IMG_CONT","MSG_IMG");
	 HTM_TD_End ();

	 /***** Free media *****/
	 Med_MediaDestructor (&Media);

      HTM_TR_End ();
     }

   /***** Free memory used for user's data *****/
   Usr_UsrDataDestructor (&UsrDat);
  }

/*****************************************************************************/
/******************** Get subject and content of a message *******************/
/*****************************************************************************/

void Msg_GetNotifMessage (char SummaryStr[Ntf_MAX_BYTES_SUMMARY + 1],
                          char **ContentStr,long MsgCod,Ntf_GetContent_t GetContent)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   size_t Length;

   SummaryStr[0] = '\0';	// Return nothing on error

   /***** Get subject of message from database *****/
   if (Msg_DB_GetSubjectAndContent (&mysql_res,MsgCod) == Exi_EXISTS)
     {
      /***** Get subject and content of the message *****/
      row = mysql_fetch_row (mysql_res);

      /***** Copy subject *****/
      // TODO: Do only direct copy when Subject will be VARCHAR(255)
      if (strlen (row[0]) > Ntf_MAX_BYTES_SUMMARY)
	{
	 strncpy (SummaryStr,row[0],
		  Ntf_MAX_BYTES_SUMMARY);
	 SummaryStr[Ntf_MAX_BYTES_SUMMARY] = '\0';
	}
      else
	 Str_Copy (SummaryStr,row[0],Ntf_MAX_BYTES_SUMMARY);

      /***** Copy subject *****/
      if (GetContent == Ntf_GET_CONTENT)
	{
	 Length = strlen (row[1]);
	 if ((*ContentStr = malloc (Length + 1)) == NULL)
            Err_NotEnoughMemoryExit ();
	 Str_Copy (*ContentStr,row[1],Length);
	}
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/************************** Write number of message **************************/
/*****************************************************************************/

void Msg_WriteMsgNumber (unsigned long MsgNum,bool NewMsg)
  {
   HTM_TD_Begin ("class=\"CT %s_%s %s_%s\" style=\"width:45px;\"",
		 NewMsg ? "MSG_TIT_NEW" :
			  "MSG_TIT",The_GetSuffix (),
		 NewMsg ? "MSG_BG_NEW" :
			  "MSG_BG" ,The_GetSuffix ());
      HTM_UnsignedLong (MsgNum); HTM_Colon ();
   HTM_TD_End ();
  }

/*****************************************************************************/
/******************** Write subject of a received message ********************/
/*****************************************************************************/

static void Msg_WriteSentOrReceivedMsgSubject (struct Msg_Messages *Messages,
					       long MsgCod,const char *Subject,
                                               CloOpe_ClosedOrOpen_t ClosedOrOpen,
                                               bool Expanded)
  {
   extern const char *Txt_See_message;
   extern const char *Txt_Hide_message;
   extern const char *Txt_no_subject;
   static const char *ClassSubject[CloOpe_NUM_CLOSED_OPEN] =
     {
      [CloOpe_CLOSED] = "MSG_TIT_NEW",
      [CloOpe_OPEN  ] = "MSG_TIT",
     };
   static const char *ClassBg[CloOpe_NUM_CLOSED_OPEN] =
     {
      [CloOpe_CLOSED] = "MSG_BG_NEW",
      [CloOpe_OPEN  ] = "MSG_BG",
     };
   static Act_Action_t Action[Msg_NUM_TYPES_OF_MSGS][2] =
     {
      [Msg_WRITING ][false] = ActUnk,
      [Msg_WRITING ][true ] = ActUnk,
      [Msg_RECEIVED][false] = ActExpRcvMsg,
      [Msg_RECEIVED][true ] = ActConRcvMsg,
      [Msg_SENT    ][false] = ActExpSntMsg,
      [Msg_SENT    ][true ] = ActConSntMsg,
     };
   static const char **Title[2] =
     {
      [false] = &Txt_See_message,
      [true ] = &Txt_Hide_message,
     };

   /***** Begin cell *****/
   HTM_TD_Begin ("class=\"LT %s_%s %s_%s\"",
                 ClassSubject[ClosedOrOpen],The_GetSuffix (),
                 ClassBg[ClosedOrOpen],The_GetSuffix ());

      /***** Begin form to expand/contract the message *****/
      Frm_BeginForm (Action[Messages->TypeOfMessages][Expanded]);
	 Messages->MsgCod = MsgCod;	// Message to be contracted/expanded
	 Msg_PutParsOneMsg (Messages);

	 HTM_BUTTON_Submit_Begin (*Title[Expanded],NULL,"class=\"LT BT_LINK\"");

	    /***** Write subject *****/
	    if (Subject[0])
	       HTM_Txt (Subject);
	    else
	      {
	       HTM_OpenBracket ();
	          HTM_Txt (Txt_no_subject);
	       HTM_CloseBracket ();
	      }

	 /***** End form to expand the message *****/
	 HTM_BUTTON_End ();

      Frm_EndForm ();

   /***** End cell *****/
   HTM_TD_End ();
  }

/*****************************************************************************/
/********************* Write course origin of a message **********************/
/*****************************************************************************/
// Returns true if the origin course is the current course

static bool Msg_WriteCrsOrgMsg (long CrsCod)
  {
   extern Err_SuccessOrError_t (*Hie_GetDataByCod[Hie_NUM_LEVELS]) (struct Hie_Node *Node);
   extern const char *Txt_from_this_course;
   extern const char *Txt_no_course_of_origin;
   struct Hie_Node Crs;
   bool FromThisCrs = true;
   bool ThereIsOrgCrs = false;

   if (CrsCod > 0)
     {
      /* Get new course code from old course code */
      Crs.HieCod = CrsCod;

      /* Get data of current degree */
      if (Hie_GetDataByCod[Hie_CRS] (&Crs) == Err_SUCCESS)
        {
         ThereIsOrgCrs = true;
         if ((FromThisCrs = (CrsCod == Gbl.Hierarchy.Node[Hie_CRS].HieCod)))	// Message sent from current course
           {
            HTM_DIV_Begin ("class=\"MSG_AUT_%s\"",The_GetSuffix ());
	       HTM_ParTxtPar (Txt_from_this_course);
            HTM_DIV_End ();
           }
         else	// Message sent from another course
           {
            /* Write course, including link */
            Frm_BeginFormGoTo (ActSeeCrsInf);
	       ParCod_PutPar (ParCod_Crs,Crs.HieCod);
	       HTM_DIV_Begin ("class=\"MSG_AUT_%s\"",The_GetSuffix ());
		  HTM_OpenParenthesis ();
		     HTM_BUTTON_Submit_Begin (Str_BuildGoToTitle (Crs.FullName),NULL,
					      "class=\"BT_LINK\"");
		     Str_FreeGoToTitle ();
			HTM_Txt (Crs.ShrtName);
		     HTM_BUTTON_End ();
		  HTM_CloseParenthesis ();
	       HTM_DIV_End ();
            Frm_EndForm ();
           }
	}
     }
   if (!ThereIsOrgCrs)	// It's an old message without origin source specified, or is a message sent from none course
     {
      HTM_DIV_Begin ("class=\"MSG_AUT_%s\"",The_GetSuffix ());
	 HTM_ParTxtPar (Txt_no_course_of_origin);
      HTM_DIV_End ();
     }

   return FromThisCrs;
  }

/*****************************************************************************/
/************************* Write form to reply a message *********************/
/*****************************************************************************/

static void Msg_WriteFormToReply (long MsgCod,long CrsCod,bool FromThisCrs,
                                  const struct Usr_Data *UsrDat)
  {
   /***** Begin form and parameters *****/
   if (FromThisCrs)
      Frm_BeginForm (ActReqMsgUsr);
   else	// Not the current course ==> go to another course
     {
      Frm_BeginFormGoTo (ActReqMsgUsr);
	 ParCod_PutPar (ParCod_Crs,CrsCod);
     }
      Grp_PutParAllGroups ();
      Par_PutParChar ("IsReply",'Y');
      ParCod_PutPar (ParCod_Msg,MsgCod);
      Usr_PutParUsrCodEncrypted (UsrDat->EnUsrCod);
      Par_PutParChar ("ShowOnlyOneRecipient",'Y');

      /****** Link *****/
      Ico_PutIconLink ("reply.svg",Ico_BLACK,ActReqMsgUsr);

   /****** End form *****/
   Frm_EndForm ();
  }

/*****************************************************************************/
/************************** Write author of a message ************************/
/*****************************************************************************/

static void Msg_WriteMsgFrom (struct Msg_Messages *Messages,
                              struct Usr_Data *UsrDat,bool Deleted)
  {
   extern const char *Txt_MSG_Sent;
   extern const char *Txt_MSG_Sent_and_deleted;
   extern const char *Txt_ROLES_SINGUL_abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   static const char *ClassPhoto[PhoSha_NUM_SHAPES] =
     {
      [PhoSha_SHAPE_CIRCLE   ] = "PHOTOC21x28",
      [PhoSha_SHAPE_ELLIPSE  ] = "PHOTOE21x28",
      [PhoSha_SHAPE_OVAL     ] = "PHOTOO21x28",
      [PhoSha_SHAPE_RECTANGLE] = "PHOTOR21x28",
     };

   HTM_TABLE_Begin (NULL);
      HTM_TR_Begin (NULL);

	 /***** Put an icon to show if user has read the message *****/
	 HTM_TD_Begin ("class=\"LM\" style=\"width:20px;\"");
	    if (Deleted)
	       Ico_PutIcon ("share.svg",Ico_RED  ,Txt_MSG_Sent_and_deleted,"ICO16x16");
	    else
	       Ico_PutIcon ("share.svg",Ico_BLACK,Txt_MSG_Sent            ,"ICO16x16");
	 HTM_TD_End ();

	 /***** Put user's photo *****/
	 HTM_TD_Begin ("class=\"CM\" style=\"width:30px;\"");
	    Pho_ShowUsrPhotoIfAllowed (UsrDat,
	                               ClassPhoto[Gbl.Prefs.PhotoShape],Pho_ZOOM);
	 HTM_TD_End ();

	 /***** Write user's name *****/
	 HTM_TD_Begin ("class=\"LM MSG_AUT_%s\"",The_GetSuffix ());
	    if (UsrDat->UsrCod > 0)
	      {
	       HTM_Txt (UsrDat->FullName);
	       if (Act_GetSuperAction (Gbl.Action.Act) == ActSeeRcvMsg)
		 {
		  HTM_NBSP ();
		  if (Msg_DB_CheckIfUsrIsBanned (UsrDat->UsrCod,Gbl.Usrs.Me.UsrDat.UsrCod))
		     // Sender is banned
		     Msg_PutFormToUnbanSender (Messages,UsrDat);
		  else
		     // Sender is not banned
		     Msg_PutFormToBanSender (Messages,UsrDat);
		 }
	      }
	    else
	      {
	       HTM_OpenBracket ();
	          HTM_Txt (Txt_ROLES_SINGUL_abc[Rol_UNK][Usr_SEX_UNKNOWN]);	// User not found, likely an old user who has been removed
	       HTM_CloseBracket ();
	      }
	 HTM_TD_End ();

      HTM_TR_End ();
   HTM_TABLE_End ();
  }

/*****************************************************************************/
/******************** Write list of recipients of a message ******************/
/*****************************************************************************/

#define Msg_MAX_RECIPIENTS_TO_SHOW 10	// If number of recipients <= Msg_MAX_RECIPIENTS_TO_SHOW, show all recipients
#define Msg_DEF_RECIPIENTS_TO_SHOW  5	// If number of recipients  > Msg_MAX_RECIPIENTS_TO_SHOW, show only Msg_DEF_RECIPIENTS_TO_SHOW

static void Msg_WriteMsgTo (struct Msg_Messages *Messages,long MsgCod)
  {
   extern const char *Txt_MSG_Open_and_deleted;
   extern const char *Txt_MSG_Open;
   extern const char *Txt_MSG_Deleted_without_opening;
   extern const char *Txt_MSG_Unopened;
   extern const char *Txt_unknown_recipient;
   extern const char *Txt_unknown_recipients;
   extern const char *Txt_View_all_recipients;
   extern const char *Txt_and_X_other_recipients;
   extern const char *Txt_unknown_recipient;
   extern const char *Txt_unknown_recipients;
   static Act_Action_t ActionSee[Msg_NUM_TYPES_OF_MSGS] =
     {
      [Msg_WRITING ] = ActUnk,
      [Msg_RECEIVED] = ActSeeRcvMsg,
      [Msg_SENT    ] = ActSeeSntMsg,
     };
   static const char *ClassPhoto[PhoSha_NUM_SHAPES] =
     {
      [PhoSha_SHAPE_CIRCLE   ] = "PHOTOC21x28",
      [PhoSha_SHAPE_ELLIPSE  ] = "PHOTOE21x28",
      [PhoSha_SHAPE_OVAL     ] = "PHOTOO21x28",
      [PhoSha_SHAPE_RECTANGLE] = "PHOTOR21x28",
     };
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumRcp;
   struct
     {
      unsigned Total;
      unsigned Known;
      unsigned Unknown;
      unsigned ToShow;
     } NumRecipients;
   struct Usr_Data UsrDat;
   bool Deleted;
   bool OpenByDst;
   bool UsrValid;
   Pho_ShowPhotos_t ShowPhotos;
   const char *Title;
   char PhotoURL[WWW_MAX_BYTES_WWW + 1];

   /***** Get number of recipients of a message from database *****/
   NumRecipients.Total = Msg_DB_GetNumRecipients (MsgCod);

   /***** Get recipients of a message from database *****/
   NumRecipients.Known = Msg_DB_GetKnownRecipients (&mysql_res,MsgCod);

   /***** Check number of recipients *****/
   if (NumRecipients.Total)
     {
      /***** Begin table *****/
      HTM_TABLE_Begin (NULL);

	 /***** How many recipients will be shown? *****/
	 if (NumRecipients.Known <= Msg_MAX_RECIPIENTS_TO_SHOW)
	    NumRecipients.ToShow = NumRecipients.Known;
	 else	// A lot of recipients
	    /***** Get parameter that indicates if I want to see all recipients *****/
	    NumRecipients.ToShow = Par_GetParBool ("SeeAllRcpts") ? NumRecipients.Known :
								    Msg_DEF_RECIPIENTS_TO_SHOW;

	 /***** Initialize structure with user's data *****/
	 Usr_UsrDataConstructor (&UsrDat);

	 /***** Write known recipients *****/
	 for (NumRcp = 0;
	      NumRcp < NumRecipients.ToShow;
	      NumRcp++)
	   {
	    row = mysql_fetch_row (mysql_res);

	    /* Get user's code (row[0]) */
	    UsrDat.UsrCod = Str_ConvertStrCodToLongCod (row[0]);

	    /* Get if message has been deleted (row[1]) and read (row[2]) by recipient */
	    Deleted   = (row[1][0] == 'Y');
	    OpenByDst = (row[2][0] == 'Y');

	    /* Get user's data */
	    UsrValid = Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat,
								Usr_DONT_GET_PREFS,
								Usr_DONT_GET_ROLE_IN_CRS);

	    /* Put an icon to show if user has read the message */
	    Title = OpenByDst ? (Deleted ? Txt_MSG_Open_and_deleted :
					   Txt_MSG_Open) :
				(Deleted ? Txt_MSG_Deleted_without_opening :
					   Txt_MSG_Unopened);
	    HTM_TR_Begin (NULL);

	       HTM_TD_Begin ("class=\"LM\" style=\"width:20px;\"");
	          if (OpenByDst)
	             Ico_PutIcon ("envelope-open-text.svg",Deleted ? Ico_RED :
				                                     Ico_BLACK,
				  Title,"ICO16x16");
	          else
		     Ico_PutIcon ("envelope.svg"          ,Deleted ? Ico_RED :
			                                             Ico_BLACK,
				  Title,"ICO16x16");
	       HTM_TD_End ();

	       /* Put user's photo */
	       HTM_TD_Begin ("class=\"CT\" style=\"width:30px;\"");
		  ShowPhotos = (UsrValid ? Pho_ShowingUsrPhotoIsAllowed (&UsrDat,PhotoURL) :
					   Pho_PHOTOS_DONT_SHOW);
		  Pho_ShowUsrPhoto (&UsrDat,ShowPhotos == Pho_PHOTOS_SHOW ? PhotoURL :
									    NULL,
				    ClassPhoto[Gbl.Prefs.PhotoShape],Pho_ZOOM);
	       HTM_TD_End ();

	       /* Write user's name */
	       HTM_TD_Begin ("class=\"LM %s_%s\"",
	                     OpenByDst ? "MSG_AUT" :
				         "MSG_AUT_NEW",
			     The_GetSuffix ());
		  if (UsrValid)
		     HTM_Txt (UsrDat.FullName);
		  else
		    {
		     HTM_OpenBracket ();
		        HTM_Txt (Txt_unknown_recipient);	// User not found, likely a user who has been removed
		     HTM_CloseBracket ();
		    }
	       HTM_TD_End ();

	    HTM_TR_End ();
	   }

	 /***** If any recipients are unknown *****/
	 if ((NumRecipients.Unknown = NumRecipients.Total - NumRecipients.Known))
	   {
	    /***** Begin form to show all users *****/
	    HTM_TR_Begin (NULL);

	       HTM_TD_Begin ("colspan=\"3\" class=\"LM MSG_AUT_%s\"",
	                     The_GetSuffix ());
		  HTM_OpenBracket ();
		     HTM_UnsignedTxt (NumRecipients.Unknown,
				      Txt_unknown_recipient,
				      Txt_unknown_recipients);
		  HTM_CloseBracket ();
	       HTM_TD_End ();

	    HTM_TR_End ();
	   }

	 /***** If any known recipient is not listed *****/
	 if (NumRecipients.ToShow < NumRecipients.Known)
	   {
	    /***** Begin form to show all users *****/
	    HTM_TR_Begin (NULL);

	       HTM_TD_Begin ("colspan=\"3\" class=\"LM MSG_AUT_%s\"",
	                     The_GetSuffix ());
		  Frm_BeginForm (ActionSee[Messages->TypeOfMessages]);
		     Messages->MsgCod = MsgCod;	// Message to be expanded with all recipients visible
		     Msg_PutParsOneMsg (Messages);
		     Par_PutParChar ("SeeAllRcpts",'Y');
			HTM_BUTTON_Submit_Begin (Txt_View_all_recipients,NULL,
			                         "class=\"LM BT_LINK\"");
			   HTM_TxtF (Txt_and_X_other_recipients,
				     NumRecipients.Known -
				     NumRecipients.ToShow);
			HTM_BUTTON_End ();
		  Frm_EndForm ();
	       HTM_TD_End ();

	    HTM_TR_End ();
	   }

	 /***** Free memory used for user's data *****/
	 Usr_UsrDataDestructor (&UsrDat);

      /***** End table *****/
      HTM_TABLE_End ();
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/******************* Write the date of creation of a message *****************/
/*****************************************************************************/
// TimeUTC holds UTC date and time in UNIX format (seconds since 1970)

void Msg_WriteMsgDate (time_t TimeUTC,const char *ClassTxt,const char *ClassBg)
  {
   static unsigned UniqueId = 0;
   char *Id;

   UniqueId++;
   if (asprintf (&Id,"msg_date_%u",UniqueId) < 0)
      Err_NotEnoughMemoryExit ();

   /***** Begin cell *****/
   HTM_TD_Begin ("id=\"%s\" class=\"RT %s_%s %s_%s\" style=\"width:106px;\"",
                 Id,
                 ClassTxt,The_GetSuffix (),
                 ClassBg,The_GetSuffix ());

      /***** Write date and time *****/
      Dat_WriteLocalDateHMSFromUTC (Id,TimeUTC,
				    Gbl.Prefs.DateFormat,Dat_SEPARATOR_BREAK,
				    Dat_WRITE_TODAY |
				    Dat_WRITE_DATE_ON_SAME_DAY |
				    Dat_WRITE_HOUR |
				    Dat_WRITE_MINUTE);

   /***** End cell *****/
   HTM_TD_End ();

   free (Id);
  }

/*****************************************************************************/
/********************* Write the text (content) of a message *****************/
/*****************************************************************************/

void Msg_WriteMsgContent (char Content[Cns_MAX_BYTES_LONG_TEXT + 1],
                          bool InsertLinks,bool ChangeBRToRet)
  {
   /***** Insert links in URLs *****/
   if (InsertLinks)
      ALn_InsertLinks (Content,Cns_MAX_BYTES_LONG_TEXT,60);

   /***** Write message to file *****/
   if (ChangeBRToRet)
      Str_FilePrintStrChangingBRToRetAndNBSPToSpace (Fil_GetOutputFile (),Content);
   else
      HTM_Txt (Content);
  }

/*****************************************************************************/
/***************** Put a form to ban the sender of a message *****************/
/*****************************************************************************/

static void Msg_PutFormToBanSender (struct Msg_Messages *Messages,
                                    struct Usr_Data *UsrDat)
  {
   Frm_BeginForm (ActBanUsrMsg);
      Pag_PutParPagNum (Msg_WhatPaginate[Messages->TypeOfMessages],
			Messages->CurrentPage);
      Usr_PutParUsrCodEncrypted (UsrDat->EnUsrCod);
      Msg_PutParsMsgsFilters (Messages);
	 Ico_PutIconLink ("unlock.svg",Ico_GREEN,ActBanUsrMsg);
   Frm_EndForm ();
  }

/*****************************************************************************/
/**************** Put a form to unban the sender of a message ****************/
/*****************************************************************************/

static void Msg_PutFormToUnbanSender (struct Msg_Messages *Messages,
                                      struct Usr_Data *UsrDat)
  {
   Frm_BeginForm (ActUnbUsrMsg);
      Pag_PutParPagNum (Msg_WhatPaginate[Messages->TypeOfMessages],
			Messages->CurrentPage);
      Usr_PutParUsrCodEncrypted (UsrDat->EnUsrCod);
      Msg_PutParsMsgsFilters (Messages);
	 Ico_PutIconLink ("lock.svg",Ico_RED,ActUnbUsrMsg);
   Frm_EndForm ();
  }

/*****************************************************************************/
/********* Ban a sender of a message when showing received messages **********/
/*****************************************************************************/

void Msg_BanSenderWhenShowingMsgs (void)
  {
   extern const char *Txt_From_this_time_you_will_not_receive_messages_from_X;

   /***** Get user's code from form *****/
   Usr_GetParOtherUsrCodEncryptedAndGetListIDs ();

   /***** Get password, user type and user's data from database *****/
   if (!Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&Gbl.Usrs.Other.UsrDat,
                                                 Usr_DONT_GET_PREFS,
                                                 Usr_DONT_GET_ROLE_IN_CRS))
      Err_WrongUserExit ();

   /***** Insert pair (sender's code - my code) in table of banned senders if not inserted *****/
   Msg_DB_CreateUsrsPairIntoBanned (Gbl.Usrs.Other.UsrDat.UsrCod,	// From
                                    Gbl.Usrs.Me.UsrDat.UsrCod);		// To

   /***** Show alert with the change made *****/
   Ale_ShowAlert (Ale_SUCCESS,Txt_From_this_time_you_will_not_receive_messages_from_X,
                  Gbl.Usrs.Other.UsrDat.FullName);

   /**** Show received messages again */
   Msg_ShowRecMsgs ();
  }

/*****************************************************************************/
/******** Unban a sender of a message when showing received messages *********/
/*****************************************************************************/

void Msg_UnbanSenderWhenShowingMsgs (void)
  {
   /**** Unban sender *****/
   Msg_UnbanSender ();

   /**** Show received messages again */
   Msg_ShowRecMsgs ();
  }

/*****************************************************************************/
/********** Unban a sender of a message when listing banned users ************/
/*****************************************************************************/

void Msg_UnbanSenderWhenListingUsrs (void)
  {
   /**** Unban sender *****/
   Msg_UnbanSender ();

   /**** List banned users again */
   Msg_ListBannedUsrs ();
  }

/*****************************************************************************/
/************************ Unban a sender of a message ************************/
/*****************************************************************************/

static void Msg_UnbanSender (void)
  {
   extern const char *Txt_From_this_time_you_can_receive_messages_from_X;

   /***** Get user's code from form *****/
   Usr_GetParOtherUsrCodEncryptedAndGetListIDs ();

   /***** Get password, user type and user's data from database *****/
   if (!Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&Gbl.Usrs.Other.UsrDat,
                                                 Usr_DONT_GET_PREFS,
                                                 Usr_DONT_GET_ROLE_IN_CRS))
      Err_WrongUserExit ();

   /***** Remove pair (sender's code - my code) from table of banned senders *****/
   Msg_DB_RemoveUsrsPairFromBanned (Gbl.Usrs.Other.UsrDat.UsrCod,	// From
                                    Gbl.Usrs.Me.UsrDat.UsrCod);		// To

   /***** Show alert with the change made *****/
   Ale_ShowAlert (Ale_SUCCESS,Txt_From_this_time_you_can_receive_messages_from_X,
                  Gbl.Usrs.Other.UsrDat.FullName);
  }

/*****************************************************************************/
/*********** List banned users, who can not sent messages to me **************/
/*****************************************************************************/

void Msg_ListBannedUsrs (void)
  {
   extern const char *Txt_You_have_not_banned_any_sender;
   extern const char *Txt_Banned_users;
   static const char *ClassPhoto[PhoSha_NUM_SHAPES] =
     {
      [PhoSha_SHAPE_CIRCLE   ] = "PHOTOC21x28",
      [PhoSha_SHAPE_ELLIPSE  ] = "PHOTOE21x28",
      [PhoSha_SHAPE_OVAL     ] = "PHOTOO21x28",
      [PhoSha_SHAPE_RECTANGLE] = "PHOTOR21x28",
     };
   MYSQL_RES *mysql_res;
   unsigned NumUsr;
   unsigned NumUsrs;
   struct Usr_Data UsrDat;

   /***** Get users banned by me *****/
   if ((NumUsrs = Msg_DB_GetUsrsBannedBy (&mysql_res,Gbl.Usrs.Me.UsrDat.UsrCod)))
     {
      /***** Initialize structure with user's data *****/
      Usr_UsrDataConstructor (&UsrDat);

      /***** Begin box and table *****/
      Box_BoxTableBegin (Txt_Banned_users,NULL,NULL,NULL,Box_NOT_CLOSABLE,2);

	 /***** List users *****/
	 for (NumUsr  = 1;
	      NumUsr <= NumUsrs;
	      NumUsr++)
	   {
	    /* Get user's code */
	    UsrDat.UsrCod = DB_GetNextCode (mysql_res);

	    /* Get user's data from database */
	    if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat,
							 Usr_DONT_GET_PREFS,
							 Usr_DONT_GET_ROLE_IN_CRS))
	      {
	       HTM_TR_Begin (NULL);

		  /* Put form to unban user */
		  HTM_TD_Begin ("class=\"BM\"");
		     Frm_BeginForm (ActUnbUsrLst);
			Usr_PutParUsrCodEncrypted (UsrDat.EnUsrCod);
			Ico_PutIconLink ("lock.svg",Ico_RED,ActUnbUsrLst);
		     Frm_EndForm ();
		  HTM_TD_End ();

		  /* Show photo */
		  HTM_TD_Begin ("class=\"LM\" style=\"width:30px;\"");
		     Pho_ShowUsrPhotoIfAllowed (&UsrDat,
		                                ClassPhoto[Gbl.Prefs.PhotoShape],Pho_ZOOM);
		  HTM_TD_End ();

		  /* Write user's full name */
		  HTM_TD_Txt_Left (UsrDat.FullName);

	       HTM_TR_End ();
	      }
	   }

      /***** End table and box *****/
      Box_BoxTableEnd ();

      /***** Free memory used for user's data *****/
      Usr_UsrDataDestructor (&UsrDat);
     }
   else   // If not result ==> sent message is deleted
      Ale_ShowAlert (Ale_INFO,Txt_You_have_not_banned_any_sender);

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/************************ Show figures about messages ************************/
/*****************************************************************************/

void Msg_GetAndShowMsgsStats (Hie_Level_t HieLvl)
  {
   extern const char *Hlp_ANALYTICS_Figures_messages;
   extern const char *Txt_FIGURE_TYPES[Fig_NUM_FIGURES];
   extern const char *Txt_Messages;
   extern const char *Txt_MSGS_Not_deleted;
   extern const char *Txt_MSGS_Deleted;
   extern const char *Txt_Total;
   extern const char *Txt_Number_of_BR_notifications;
   extern const char *Txt_MSGS_Sent;
   extern const char *Txt_MSGS_Received;
   unsigned NumMsgsSentNotDeleted,NumMsgsSentDeleted;
   unsigned NumMsgsReceivedNotDeleted,NumMsgsReceivedAndDeleted;
   unsigned NumMsgsReceivedAndNotified;

   /***** Get the number of unique messages sent from this location *****/
   NumMsgsSentNotDeleted      = Msg_DB_GetNumSntMsgs (HieLvl,Msg_STATUS_ALL     );
   NumMsgsSentDeleted         = Msg_DB_GetNumSntMsgs (HieLvl,Msg_STATUS_DELETED );

   NumMsgsReceivedNotDeleted  = Msg_DB_GetNumRcvMsgs (HieLvl,Msg_STATUS_ALL     );
   NumMsgsReceivedAndDeleted  = Msg_DB_GetNumRcvMsgs (HieLvl,Msg_STATUS_DELETED );
   NumMsgsReceivedAndNotified = Msg_DB_GetNumRcvMsgs (HieLvl,Msg_STATUS_NOTIFIED);

   /***** Begin box and table *****/
   Box_BoxTableBegin (Txt_FIGURE_TYPES[Fig_MESSAGES],NULL,NULL,
                      Hlp_ANALYTICS_Figures_messages,Box_NOT_CLOSABLE,2);

      /***** Write table heading *****/
      HTM_TR_Begin (NULL);
	 HTM_TH (Txt_Messages                  ,HTM_HEAD_LEFT);
	 HTM_TH (Txt_MSGS_Not_deleted          ,HTM_HEAD_RIGHT);
	 HTM_TH (Txt_MSGS_Deleted              ,HTM_HEAD_RIGHT);
	 HTM_TH (Txt_Total                     ,HTM_HEAD_RIGHT);
	 HTM_TH (Txt_Number_of_BR_notifications,HTM_HEAD_RIGHT);
      HTM_TR_End ();

      /***** Write number of messages *****/
      HTM_TR_Begin (NULL);
	 HTM_TD_Txt_Left (Txt_MSGS_Sent);
	 HTM_TD_Unsigned (NumMsgsSentNotDeleted);
	 HTM_TD_Unsigned (NumMsgsSentDeleted);
	 HTM_TD_Unsigned (NumMsgsSentNotDeleted + NumMsgsSentDeleted);
	 HTM_TD_Txt_Right ("-");
      HTM_TR_End ();

      HTM_TR_Begin (NULL);
	 HTM_TD_Txt_Left (Txt_MSGS_Received);
	 HTM_TD_Unsigned (NumMsgsReceivedNotDeleted);
	 HTM_TD_Unsigned (NumMsgsReceivedAndDeleted);
	 HTM_TD_Unsigned (NumMsgsReceivedNotDeleted + NumMsgsReceivedAndDeleted);
	 HTM_TD_Unsigned (NumMsgsReceivedAndNotified);
      HTM_TR_End ();

   /***** End table and box *****/
   Box_BoxTableEnd ();
  }
