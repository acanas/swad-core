// swad_message.c: messages between users

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2019 Antonio Cañas Vargas

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
#include <linux/stddef.h>	// For NULL
#include <stdio.h>		// For asprintf
#include <stdlib.h>		// For free
#include <string.h>		// For string functions
#include <time.h>		// For time

#include "swad_action.h"
#include "swad_box.h"
#include "swad_config.h"
#include "swad_course.h"
#include "swad_database.h"
#include "swad_form.h"
#include "swad_forum.h"
#include "swad_global.h"
#include "swad_group.h"
#include "swad_HTML.h"
#include "swad_ID.h"
#include "swad_message.h"
#include "swad_notification.h"
#include "swad_parameter.h"
#include "swad_photo.h"
#include "swad_profile.h"
#include "swad_user.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/***************************** Private constants *****************************/
/*****************************************************************************/

#define Msg_MAX_BYTES_MESSAGES_QUERY (4 * 1024 - 1)

// Forum images will be saved with:
// - maximum width of Msg_IMAGE_SAVED_MAX_HEIGHT
// - maximum height of Msg_IMAGE_SAVED_MAX_HEIGHT
// - maintaining the original aspect ratio (aspect ratio recommended: 3:2)
#define Msg_IMAGE_SAVED_MAX_WIDTH	768
#define Msg_IMAGE_SAVED_MAX_HEIGHT	512
#define Msg_IMAGE_SAVED_QUALITY		 75	// 1 to 100

static const Pag_WhatPaginate_t Msg_WhatPaginate[Msg_NUM_TYPES_OF_MSGS] =
  {
   Pag_MESSAGES_RECEIVED,	// Msg_MESSAGES_RECEIVED
   Pag_MESSAGES_SENT		// Msg_MESSAGES_SENT
  };

/*****************************************************************************/
/******************************** Private types ******************************/
/*****************************************************************************/

/*****************************************************************************/
/**************************** Internal prototypes ****************************/
/*****************************************************************************/

static void Msg_PutFormMsgUsrs (char Content[Cns_MAX_BYTES_LONG_TEXT + 1]);

static void Msg_ShowSentOrReceivedMessages (void);
static unsigned long Msg_GetNumUsrsBannedByMe (void);
static void Msg_PutLinkToViewBannedUsers(void);
static unsigned long Msg_GetSentOrReceivedMsgs (long UsrCod,
						long FilterCrsCod,
						const char *FilterFromToSubquery,
						MYSQL_RES **mysql_res);

static char *Msg_WriteNumMsgs (unsigned NumUnreadMsgs);

static void Msg_PutIconsListMsgs (void);
static void Msg_PutHiddenParamsOneMsg (void);

static void Msg_ShowFormToShowOnlyUnreadMessages (void);
static void Msg_GetParamOnlyUnreadMsgs (void);
static void Msg_ShowASentOrReceivedMessage (long MsgNum,long MsgCod);
static void Msg_GetStatusOfSentMsg (long MsgCod,bool *Expanded);
static void Msg_GetStatusOfReceivedMsg (long MsgCod,bool *Open,bool *Replied,bool *Expanded);
static long Msg_GetParamMsgCod (void);
static void Msg_PutLinkToShowMorePotentialRecipients (void);
static void Msg_PutParamsShowMorePotentialRecipients (void);
static void Msg_PutParamsWriteMsg (void);
static void Msg_ShowOneUniqueRecipient (void);
static void Msg_WriteFormUsrsIDsOrNicksOtherRecipients (void);
static void Msg_WriteFormSubjectAndContentMsgToUsrs (char Content[Cns_MAX_BYTES_LONG_TEXT + 1]);
static void Msg_ShowNumMsgsDeleted (unsigned NumMsgs);

static void Msg_MakeFilterFromToSubquery (char FilterFromToSubquery[Msg_MAX_BYTES_MESSAGES_QUERY + 1]);

static void Msg_ExpandSentMsg (long MsgCod);
static void Msg_ExpandReceivedMsg (long MsgCod);
static void Msg_ContractSentMsg (long MsgCod);
static void Msg_ContractReceivedMsg (long MsgCod);

static long Msg_InsertNewMsg (const char *Subject,const char *Content,
                              struct Media *Media);

static unsigned long Msg_DelSomeRecOrSntMsgsUsr (Msg_TypeOfMessages_t TypeOfMessages,long UsrCod,
                                                 long FilterCrsCod,const char *FilterFromToSubquery);
static void Msg_InsertReceivedMsgIntoDB (long MsgCod,long UsrCod,bool NotifyByEmail);
static void Msg_SetReceivedMsgAsReplied (long MsgCod);
static void Msg_MoveReceivedMsgToDeleted (long MsgCod,long UsrCod);
static void Msg_MoveSentMsgToDeleted (long MsgCod);
static void Msg_MoveMsgContentToDeleted (long MsgCod);
static bool Msg_CheckIfSentMsgIsDeleted (long MsgCod);
static bool Msg_CheckIfReceivedMsgIsDeletedForAllItsRecipients (long MsgCod);
static unsigned Msg_GetNumUnreadMsgs (long FilterCrsCod,const char *FilterFromToSubquery);

static void Msg_GetMsgSntData (long MsgCod,long *CrsCod,long *UsrCod,
                               time_t *CreatTimeUTC,
                               char Subject[Cns_MAX_BYTES_SUBJECT + 1],
                               bool *Deleted);
static void Msg_GetMsgContent (long MsgCod,char Content[Cns_MAX_BYTES_LONG_TEXT + 1],
                               struct Media *Media);

static void Msg_WriteSentOrReceivedMsgSubject (long MsgCod,const char *Subject,bool Open,bool Expanded);
static void Msg_WriteFormToReply (long MsgCod,long CrsCod,
                                  bool ThisCrs,bool Replied,
                                  const struct UsrData *UsrDat);
static void Msg_WriteMsgFrom (struct UsrData *UsrDat,bool Deleted);
static void Msg_WriteMsgTo (long MsgCod);

static void Msg_PutFormToBanSender (struct UsrData *UsrDat);
static void Msg_PutFormToUnbanSender (struct UsrData *UsrDat);
static void Msg_UnbanSender (void);

/*****************************************************************************/
/***************** Put a form to write a new message to users ****************/
/*****************************************************************************/

void Msg_FormMsgUsrs (void)
  {
   char Content[Cns_MAX_BYTES_LONG_TEXT + 1];

   /***** Get possible hidden subject and content of the message *****/
   Par_GetParToHTML ("HiddenSubject",Gbl.Msg.Subject,Cns_MAX_BYTES_SUBJECT);
   Par_GetParAndChangeFormat ("HiddenContent",Content,Cns_MAX_BYTES_LONG_TEXT,
                              Str_TO_TEXT,false);

   /***** Show a form to compose a message to users *****/
   Msg_PutFormMsgUsrs (Content);
  }

/*****************************************************************************/
/***************** Put a form to write a new message to users ****************/
/*****************************************************************************/

static void Msg_PutFormMsgUsrs (char Content[Cns_MAX_BYTES_LONG_TEXT + 1])
  {
   extern const char *Hlp_MESSAGES_Write;
   extern const char *The_ClassFormInBox[The_NUM_THEMES];
   extern const char *Txt_Reply_message;
   extern const char *Txt_New_message;
   extern const char *Txt_MSG_To;
   extern const char *Txt_Send_message;
   unsigned NumUsrsInCrs = 0;	// Initialized to avoid warning
   bool ShowUsrsInCrs = false;
   bool GetUsrsInCrs;

   Gbl.Usrs.LstUsrs[Rol_STD].NumUsrs =
   Gbl.Usrs.LstUsrs[Rol_NET].NumUsrs =
   Gbl.Usrs.LstUsrs[Rol_TCH].NumUsrs = 0;

   /***** Get parameter that indicates if the message is a reply to another message *****/
   if ((Gbl.Msg.Reply.IsReply = Par_GetParToBool ("IsReply")))
      /* Get original message code */
      if ((Gbl.Msg.Reply.OriginalMsgCod = Msg_GetParamMsgCod ()) <= 0)
         Lay_ShowErrorAndExit ("Wrong code of message.");

   /***** Get user's code of possible preselected recipient *****/
   if (Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ())	// There is a preselected recipient
      /* Get who to show as potential recipients:
         - only the selected recipient
         - any user (default) */
      Gbl.Msg.ShowOnlyOneRecipient = Par_GetParToBool ("ShowOnlyOneRecipient");
   else
      Gbl.Msg.ShowOnlyOneRecipient = false;

   GetUsrsInCrs = !Gbl.Msg.ShowOnlyOneRecipient &&	// Show list of potential recipients
	          (Gbl.Usrs.Me.IBelongToCurrentCrs ||	// If there is a course selected and I belong to it
	           Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM);
   if (GetUsrsInCrs)
     {
      /***** Get and update type of list,
	     number of columns in class photo
	     and preference about view photos *****/
      Usr_GetAndUpdatePrefsAboutUsrList ();

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
   Box_BoxBegin (NULL,Gbl.Msg.Reply.IsReply ? Txt_Reply_message :
					      Txt_New_message,NULL,
		 Hlp_MESSAGES_Write,Box_NOT_CLOSABLE);

   if (Gbl.Msg.ShowOnlyOneRecipient)
      /***** Form to show several potential recipients *****/
      Msg_PutLinkToShowMorePotentialRecipients ();
   else
     {
      /***** Get list of users belonging to the current course *****/
      if (GetUsrsInCrs)
	{
	 /***** Form to select groups *****/
	 Grp_ShowFormToSelectSeveralGroups (Msg_PutParamsWriteMsg,
	                                    Grp_MY_GROUPS);

	 /***** Start section with user list *****/
         HTM_SECTION_Begin (Usr_USER_LIST_SECTION_ID);

	 if (NumUsrsInCrs)
	   {
	    /***** Form to select type of list used for select several users *****/
	    Usr_ShowFormsToSelectUsrListType (Msg_PutParamsWriteMsg);

	    /***** Put link to register students *****/
	    Enr_CheckStdsAndPutButtonToRegisterStdsInCurrentCrs ();

	    /***** Check if it's a big list *****/
	    ShowUsrsInCrs = Usr_GetIfShowBigList (NumUsrsInCrs,Msg_PutParamsWriteMsg,
		                                  "CopyMessageToHiddenFields();");

	    if (ShowUsrsInCrs)
	       /***** Get lists of selected users *****/
	       Usr_GetListsSelectedEncryptedUsrsCods (&Gbl.Usrs.Selected);
	   }

	 /***** End section with user list *****/
         HTM_SECTION_End ();
        }

      /***** Get list of users' IDs or nicknames written explicitely *****/
      Usr_GetListMsgRecipientsWrittenExplicitelyBySender (false);
     }

   /***** Begin form to select recipients and write the message *****/
   Frm_StartForm (ActRcvMsgUsr);
   if (Gbl.Msg.Reply.IsReply)
     {
      Par_PutHiddenParamChar ("IsReply",'Y');
      Msg_PutHiddenParamMsgCod (Gbl.Msg.Reply.OriginalMsgCod);
     }
   if (Gbl.Usrs.Other.UsrDat.UsrCod > 0)
     {
      Usr_PutParamOtherUsrCodEncrypted ();
      if (Gbl.Msg.ShowOnlyOneRecipient)
	 Par_PutHiddenParamChar ("ShowOnlyOneRecipient",'Y');
     }

   /***** Begin table *****/
   HTM_TABLE_BeginCenterPadding (2);

   /***** "To:" section (recipients) *****/
   HTM_TR_Begin (NULL);

   HTM_TD_Begin ("class=\"%s RT\"",The_ClassFormInBox[Gbl.Prefs.Theme]);
   HTM_TxtF ("%s:",Txt_MSG_To);
   HTM_TD_End ();

   HTM_TD_Begin ("class=\"LT\"");
   if (Gbl.Msg.ShowOnlyOneRecipient)
      /***** Show only one user as recipient *****/
      Msg_ShowOneUniqueRecipient ();
   else
     {
      /***** Show potential recipients *****/
      HTM_TABLE_BeginWide ();
      if (ShowUsrsInCrs)
	{
	 Usr_ListUsersToSelect (Rol_TCH,&Gbl.Usrs.Selected);	// All teachers in course
	 Usr_ListUsersToSelect (Rol_NET,&Gbl.Usrs.Selected);	// All non-editing teachers in course
	 Usr_ListUsersToSelect (Rol_STD,&Gbl.Usrs.Selected);	// All students in selected groups
	}
      Msg_WriteFormUsrsIDsOrNicksOtherRecipients ();	// Other users (nicknames)
      HTM_TABLE_End ();
     }
   HTM_TD_End ();

   HTM_TR_End ();

   /***** Subject and content sections *****/
   Msg_WriteFormSubjectAndContentMsgToUsrs (Content);

   /***** End table *****/
   HTM_TABLE_End ();

   /***** Help for text editor and send button *****/
   Lay_HelpPlainEditor ();

   /***** Attached image (optional) *****/
   Med_PutMediaUploader (-1,"MSG_MED_INPUT");

   /***** Send button *****/
   Btn_PutCreateButton (Txt_Send_message);

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

static void Msg_PutLinkToShowMorePotentialRecipients (void)
  {
   extern const char *Txt_Show_more_recipients;

   /***** Contextual menu *****/
   Mnu_ContextMenuBegin ();
   Lay_PutContextualLinkIconTextOnSubmit (ActReqMsgUsr,NULL,
					  Msg_PutParamsShowMorePotentialRecipients,
					  "users.svg",
					  Txt_Show_more_recipients,
					  "CopyMessageToHiddenFields();");	// Shor more potential recipients
   Mnu_ContextMenuEnd ();
  }

/*****************************************************************************/
/************ Put parameters to show more potential recipients ***************/
/*****************************************************************************/

static void Msg_PutParamsShowMorePotentialRecipients (void)
  {
   if (Gbl.Msg.Reply.IsReply)
     {
      Par_PutHiddenParamChar ("IsReply",'Y');
      Msg_PutHiddenParamMsgCod (Gbl.Msg.Reply.OriginalMsgCod);
     }
   if (Gbl.Usrs.Other.UsrDat.UsrCod > 0)
      Usr_PutParamOtherUsrCodEncrypted ();

   /***** Hidden params to send subject and content *****/
   Msg_PutHiddenParamsSubjectAndContent ();
  }

/*****************************************************************************/
/***************** Put parameters when writing a message *********************/
/*****************************************************************************/

static void Msg_PutParamsWriteMsg (void)
  {
   Usr_PutHiddenParSelectedUsrsCods (&Gbl.Usrs.Selected);
   Msg_PutHiddenParamOtherRecipients ();
   Msg_PutHiddenParamsSubjectAndContent ();
   if (Gbl.Msg.Reply.IsReply)
     {
      Par_PutHiddenParamChar ("IsReply",'Y');
      Msg_PutHiddenParamMsgCod (Gbl.Msg.Reply.OriginalMsgCod);
     }
   if (Gbl.Usrs.Other.UsrDat.UsrCod > 0)
     {
      Usr_PutParamOtherUsrCodEncrypted ();
      if (Gbl.Msg.ShowOnlyOneRecipient)
	 Par_PutHiddenParamChar ("ShowOnlyOneRecipient",'Y');
     }
  }

/*****************************************************************************/
/********** Put hidden parameters with message subject and content ***********/
/*****************************************************************************/

void Msg_PutHiddenParamsSubjectAndContent (void)
  {
   /***** Hidden params to send subject and content.
          When the user edit the subject or the content,
          they are copied here. *****/
   Par_PutHiddenParamString (NULL,"HiddenSubject","");
   Par_PutHiddenParamString (NULL,"HiddenContent","");
  }

/*****************************************************************************/
/************ Put parameters to show more potential recipients ***************/
/*****************************************************************************/

static void Msg_ShowOneUniqueRecipient (void)
  {
   char PhotoURL[PATH_MAX + 1];
   bool ShowPhoto;

   /***** Show user's photo *****/
   ShowPhoto = Pho_ShowingUsrPhotoIsAllowed (&Gbl.Usrs.Other.UsrDat,PhotoURL);
   Pho_ShowUsrPhoto (&Gbl.Usrs.Other.UsrDat,ShowPhoto ? PhotoURL :
					                NULL,
		     "PHOTO21x28",Pho_ZOOM,false);

   /****** Write user's IDs ******/
   HTM_DIV_Begin ("class=\"MSG_TO_ONE_RCP %s\"",
		  Gbl.Usrs.Other.UsrDat.Accepted ? "DAT_SMALL_NOBR_N" :
						   "DAT_SMALL_NOBR");
   ID_WriteUsrIDs (&Gbl.Usrs.Other.UsrDat,NULL);
   HTM_DIV_End ();

   /***** Write user's name *****/
   HTM_DIV_Begin ("class=\"MSG_TO_ONE_RCP %s\"",
		  Gbl.Usrs.Other.UsrDat.Accepted ? "DAT_SMALL_NOBR_N" :
						   "DAT_SMALL_NOBR");
   HTM_Txt (Gbl.Usrs.Other.UsrDat.FullName);
   HTM_DIV_End ();

   /***** Hidden parameter with user's nickname *****/
   Msg_PutHiddenParamAnotherRecipient (&Gbl.Usrs.Other.UsrDat);
  }

/*****************************************************************************/
/************** Nicknames of recipients of a message to users ****************/
/*****************************************************************************/

static void Msg_WriteFormUsrsIDsOrNicksOtherRecipients (void)
  {
   extern const char *Txt_Other_recipients;
   extern const char *Txt_Recipients;
   extern const char *Txt_nicks_emails_or_IDs_separated_by_commas;
   char Nickname[Nck_MAX_BYTES_NICKNAME_WITHOUT_ARROBA + 1];
   unsigned ColSpan;
   bool StdsAndTchsWritten = Gbl.Hierarchy.Level == Hie_CRS &&		// Course selected
                             (Gbl.Usrs.Me.IBelongToCurrentCrs ||	// I belong to it
                              Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM);

   /***** How many columns? *****/
   if (StdsAndTchsWritten)
      ColSpan = Usr_GetColumnsForSelectUsrs ();
   else
      ColSpan = 1;

   /***** Title *****/
   HTM_TR_Begin (NULL);

   HTM_TH_Begin (1,ColSpan,"LM LIGHT_BLUE");
   HTM_LABEL_Begin ("for=\"OtherRecipients\"");
   HTM_TxtF ("%s:",StdsAndTchsWritten ? Txt_Other_recipients :
				        Txt_Recipients);
   HTM_LABEL_End ();
   HTM_TH_End ();

   HTM_TR_End ();

   /***** Textarea with users' @nicknames, emails or IDs *****/
   HTM_TR_Begin (NULL);

   HTM_TD_Begin ("colspan=\"%u\" class=\"LM\"",ColSpan);
   HTM_TEXTAREA_Begin ("id=\"OtherRecipients\" name=\"OtherRecipients\""
	               " class=\"MSG_RECIPIENTS\" rows=\"2\" placeholder=\"%s\"",
                       Txt_nicks_emails_or_IDs_separated_by_commas);
   if (Gbl.Usrs.ListOtherRecipients[0])
      HTM_Txt (Gbl.Usrs.ListOtherRecipients);
   else if (Gbl.Usrs.Other.UsrDat.UsrCod > 0)	// If there is a recipient
						// and there's no list of explicit recipients,
						// write @nickname of original sender
      if (Nck_GetNicknameFromUsrCod (Gbl.Usrs.Other.UsrDat.UsrCod,Nickname))
         HTM_TxtF ("@%s",Nickname);
   HTM_TEXTAREA_End ();
   HTM_TD_End ();

   HTM_TR_End ();
  }

/*****************************************************************************/
/****** Write form fields with subject and content of a message to users *****/
/*****************************************************************************/

static void Msg_WriteFormSubjectAndContentMsgToUsrs (char Content[Cns_MAX_BYTES_LONG_TEXT + 1])
  {
   extern const char *The_ClassFormInBox[The_NUM_THEMES];
   extern const char *Txt_MSG_Subject;
   extern const char *Txt_MSG_Content;
   extern const char *Txt_Original_message;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;
   long MsgCod;
   bool SubjectAndContentComeFromForm = (Gbl.Msg.Subject[0] || Content[0]);

   /***** Get possible code (of original message if it's a reply) *****/
   MsgCod = Msg_GetParamMsgCod ();

   /***** Message subject *****/
   HTM_TR_Begin (NULL);

   HTM_TD_Begin ("class=\"RT\"");
   HTM_LABEL_Begin ("for=\"MsgSubject\" class=\"%s\"",
                    The_ClassFormInBox[Gbl.Prefs.Theme]);
   HTM_TxtF ("%s:",Txt_MSG_Subject);
   HTM_LABEL_End ();
   HTM_TD_End ();

   HTM_TD_Begin ("class=\"LM\"");
   HTM_TEXTAREA_Begin ("id=\"MsgSubject\" name=\"Subject\""
	               " class=\"MSG_SUBJECT\" rows=\"2\"");

   /* If message is a reply ==> get original message */
   if (MsgCod > 0)	// It's a reply
     {
      if (!SubjectAndContentComeFromForm)
	{
	 /* Get subject and content of message from database */
	 NumRows = DB_QuerySELECT (&mysql_res,"can not get message content",
				   "SELECT Subject,Content FROM msg_content"
				   " WHERE MsgCod=%ld",MsgCod);

	 /* Result should have a unique row */
	 if (NumRows != 1)
	    Lay_ShowErrorAndExit ("Error when getting message.");

	 row = mysql_fetch_row (mysql_res);

	 /* Get subject */
	 Str_Copy (Gbl.Msg.Subject,row[0],
	           Cns_MAX_BYTES_SUBJECT);

	 /* Get content */
	 Str_Copy (Content,row[1],
	           Cns_MAX_BYTES_LONG_TEXT);

	 /* Free structure that stores the query result */
	 DB_FreeMySQLResult (&mysql_res);
	}

      /* Write subject */
      if (!SubjectAndContentComeFromForm)
	 HTM_Txt ("Re: ");
      HTM_Txt (Gbl.Msg.Subject);

      HTM_TEXTAREA_End ();
      HTM_TD_End ();

      HTM_TR_End ();

      /***** Message content *****/
      HTM_TR_Begin (NULL);

      HTM_TD_Begin ("class=\"RT\"");
      HTM_LABEL_Begin ("for=\"MsgContent\" class=\"%s\"",
	               The_ClassFormInBox[Gbl.Prefs.Theme]);
      HTM_TxtF ("%s:",Txt_MSG_Content);
      HTM_LABEL_End ();
      HTM_TD_End ();

      HTM_TD_Begin ("class=\"LM\"");
      HTM_TEXTAREA_Begin ("id=\"MsgContent\" name=\"Content\""
                          " class=\"MSG_CONTENT\" rows=\"20\"");

      /* Start textarea with a '\n', that will be not visible in textarea.
         When Content is "\nLorem ipsum" (a white line before "Lorem ipsum"),
         if we don't put the initial '\n' ==> the form will be sent starting
         by "Lorem", without the white line */
      HTM_Txt ("\n");

      if (!SubjectAndContentComeFromForm)
	 HTM_TxtF ("\n\n----- %s -----\n",Txt_Original_message);

      Msg_WriteMsgContent (Content,Cns_MAX_BYTES_LONG_TEXT,false,true);
     }
   else	// It's not a reply
     {
      /* End message subject */
      HTM_Txt (Gbl.Msg.Subject);

      HTM_TEXTAREA_End ();
      HTM_TD_End ();

      HTM_TR_End ();

      /***** Message content *****/
      HTM_TR_Begin (NULL);

      HTM_TD_Begin ("class=\"RT\"");
      HTM_LABEL_Begin ("for=\"MsgContent\" class=\"%s\"",
                       The_ClassFormInBox[Gbl.Prefs.Theme]);
      HTM_TxtF ("%s:",Txt_MSG_Content);
      HTM_LABEL_End ();
      HTM_TD_End ();

      HTM_TD_Begin ("class=\"LM\"");
      HTM_TEXTAREA_Begin ("id=\"MsgContent\" name=\"Content\""
                          " class=\"MSG_CONTENT\" rows=\"20\"");

      /* Start textarea with a '\n', that will be not visible in textarea.
         When Content is "\nLorem ipsum" (a white line before "Lorem ipsum"),
         if we don't put the initial '\n' ==> the form will be sent starting
         by "Lorem", without the white line */
      HTM_TxtF ("\n%s",Content);
     }

   HTM_TEXTAREA_End ();
   HTM_TD_End ();

   HTM_TR_End ();
  }

/*****************************************************************************/
/********* Put hidden parameter for another recipient (one nickname) *********/
/*****************************************************************************/

void Msg_PutHiddenParamAnotherRecipient (const struct UsrData *UsrDat)
  {
   char NicknameWithArroba[Nck_MAX_BYTES_NICKNAME_FROM_FORM + 1];

   snprintf (NicknameWithArroba,sizeof (NicknameWithArroba),
	     "@%s",
	     UsrDat->Nickname);
   Par_PutHiddenParamString (NULL,"OtherRecipients",NicknameWithArroba);
  }

/*****************************************************************************/
/********* Put hidden parameter for another recipient (one nickname) *********/
/*****************************************************************************/

void Msg_PutHiddenParamOtherRecipients (void)
  {
   if (Gbl.Usrs.ListOtherRecipients)
      if (Gbl.Usrs.ListOtherRecipients[0])
         Par_PutHiddenParamString (NULL,"OtherRecipients",Gbl.Usrs.ListOtherRecipients);
  }

/*****************************************************************************/
/********************** Receive a new message from a user ********************/
/*****************************************************************************/

void Msg_RecMsgFromUsr (void)
  {
   extern const char *Txt_You_can_not_send_a_message_to_so_many_recipients_;
   extern const char *Txt_You_must_select_one_ore_more_recipients;
   extern const char *Txt_message_not_sent_to_X;
   extern const char *Txt_message_sent_to_X_notified_by_email;
   extern const char *Txt_message_sent_to_X_not_notified_by_email;
   extern const char *Txt_Error_getting_data_from_a_recipient;
   extern const char *Txt_The_message_has_not_been_sent_to_any_recipient;
   extern const char *Txt_The_message_has_been_sent_to_1_recipient;
   extern const char *Txt_The_message_has_been_sent_to_X_recipients;
   extern const char *Txt_There_have_been_X_errors_in_sending_the_message;
   bool IsReply;
   bool RecipientHasBannedMe;
   bool Replied = false;
   long OriginalMsgCod = -1L;	// Initialized to avoid warning
   const char *Ptr;
   unsigned NumRecipients;
   unsigned NumRecipientsToBeNotifiedByEMail = 0;
   struct UsrData UsrDstData;
   int NumErrors = 0;
   long NewMsgCod = -1L;	// Initiliazed to avoid warning
   bool MsgAlreadyInserted = false;
   bool CreateNotif;
   bool NotifyByEmail;
   char Content[Cns_MAX_BYTES_LONG_TEXT + 1];
   struct Media Media;
   bool Error = false;

   /***** Get data from form *****/
   /* Get subject */
   Par_GetParToHTML ("Subject",Gbl.Msg.Subject,Cns_MAX_BYTES_SUBJECT);

   /* Get body */
   Par_GetParAndChangeFormat ("Content",Content,Cns_MAX_BYTES_LONG_TEXT,
                              Str_DONT_CHANGE,false);

   /* Get parameter that indicates if the message is a reply to a previous message */
   if ((IsReply = Par_GetParToBool ("IsReply")))
      /* Get original message code */
      if ((OriginalMsgCod = Msg_GetParamMsgCod ()) <= 0)
         Lay_ShowErrorAndExit ("Wrong code of message.");

   /* Get user's code of possible preselected recipient */
   Usr_GetParamOtherUsrCodEncryptedAndGetListIDs ();

   /* Get lists of selected users */
   Usr_GetListsSelectedEncryptedUsrsCods (&Gbl.Usrs.Selected);

   /* Get list of users' IDs or nicknames written explicitely */
   Error = Usr_GetListMsgRecipientsWrittenExplicitelyBySender (true);

   /***** Check number of recipients *****/
   if ((NumRecipients = Usr_CountNumUsrsInListOfSelectedEncryptedUsrCods (&Gbl.Usrs.Selected)))
     {
      if (Gbl.Usrs.Me.Role.Logged == Rol_STD &&
          NumRecipients > Cfg_MAX_RECIPIENTS)
        {
         /* Write warning message */
         Ale_ShowAlert (Ale_WARNING,Txt_You_can_not_send_a_message_to_so_many_recipients_);
         Error = true;
        }
     }
   else	// No recipients selected
     {
      /* Write warning message */
      Ale_ShowAlert (Ale_WARNING,Txt_You_must_select_one_ore_more_recipients);
      Error = true;
     }

   /***** If error in list of recipients, show again the form used to write a message *****/
   if (Error)
     {
      /* Show the form again, with the subject and the message filled */
      Str_ChangeFormat (Str_FROM_FORM,Str_TO_TEXT,
                        Content,Cns_MAX_BYTES_LONG_TEXT,true);
      Msg_PutFormMsgUsrs (Content);
      return;
     }

   /***** Initialize structure with user's data *****/
   Usr_UsrDataConstructor (&UsrDstData);

   /***** Initialize image *****/
   Med_MediaConstructor (&Media);

   /***** Get attached image (action, file and title) *****/
   Media.Width   = Msg_IMAGE_SAVED_MAX_WIDTH;
   Media.Height  = Msg_IMAGE_SAVED_MAX_HEIGHT;
   Media.Quality = Msg_IMAGE_SAVED_QUALITY;
   Med_GetMediaFromForm (-1,&Media,NULL,NULL);
   Ale_ShowAlerts (NULL);

   /***** Loop over the list Gbl.Usrs.Selected.List[Rol_UNK], that holds the list of the
	  recipients, creating a received message for each recipient *****/
   Str_ChangeFormat (Str_FROM_FORM,Str_TO_RIGOROUS_HTML,
                     Content,Cns_MAX_BYTES_LONG_TEXT,false);
   Ptr = Gbl.Usrs.Selected.List[Rol_UNK];
   NumRecipients = 0;
   while (*Ptr)
     {
      Par_GetNextStrUntilSeparParamMult (&Ptr,UsrDstData.EncryptedUsrCod,
                                         Cry_BYTES_ENCRYPTED_STR_SHA256_BASE64);
      Usr_GetUsrCodFromEncryptedUsrCod (&UsrDstData);
      if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDstData,Usr_DONT_GET_PREFS))		// Get recipient's data from the database
        {
         /***** Check if recipient has banned me *****/
         RecipientHasBannedMe = Msg_CheckIfUsrIsBanned (Gbl.Usrs.Me.UsrDat.UsrCod,UsrDstData.UsrCod);

         if (RecipientHasBannedMe)
            /***** Show an alert indicating that the message has not been sent successfully *****/
            Ale_ShowAlert (Ale_WARNING,Txt_message_not_sent_to_X,
		           UsrDstData.FullName);
         else
           {
            /***** Create message *****/
            if (!MsgAlreadyInserted)
              {
               // The message is inserted only once in the table of messages sent
               NewMsgCod = Msg_InsertNewMsg (Gbl.Msg.Subject,Content,&Media);
               MsgAlreadyInserted = true;
              }

            /***** If this recipient is the original sender of a message been replied, set Replied to true *****/
            Replied = (IsReply &&
        	       UsrDstData.UsrCod == Gbl.Usrs.Other.UsrDat.UsrCod);

            /***** This received message must be notified by email? *****/
            CreateNotif = (UsrDstData.NtfEvents.CreateNotif & (1 << Ntf_EVENT_MESSAGE));
            NotifyByEmail = CreateNotif &&
        	            (UsrDstData.UsrCod != Gbl.Usrs.Me.UsrDat.UsrCod) &&
                            (UsrDstData.NtfEvents.SendEmail & (1 << Ntf_EVENT_MESSAGE));

            /***** Create the received message for this recipient
                   and increment number of new messages received by this recipient *****/
            Msg_InsertReceivedMsgIntoDB (NewMsgCod,UsrDstData.UsrCod,NotifyByEmail);

            /***** Create notification for this recipient.
                   If this recipient wants to receive notifications by -mail,
                   activate the sending of a notification *****/
            if (CreateNotif)
               Ntf_StoreNotifyEventToOneUser (Ntf_EVENT_MESSAGE,&UsrDstData,NewMsgCod,
                                              (Ntf_Status_t) (NotifyByEmail ? Ntf_STATUS_BIT_EMAIL :
                                        	                              0));

            /***** Show an alert indicating that the message has been sent successfully *****/
            Ale_ShowAlert (Ale_SUCCESS,NotifyByEmail ? Txt_message_sent_to_X_notified_by_email :
                                                       Txt_message_sent_to_X_not_notified_by_email,
                           UsrDstData.FullName);

            /***** Increment number of recipients *****/
            if (NotifyByEmail)
               NumRecipientsToBeNotifiedByEMail++;
            NumRecipients++;
           }
        }
      else
        {
         Ale_ShowAlert (Ale_ERROR,Txt_Error_getting_data_from_a_recipient);
         NumErrors++;
        }
     }

   /***** Free image *****/
   Med_MediaDestructor (&Media);

   /***** Free memory used for user's data *****/
   Usr_UsrDataDestructor (&UsrDstData);

   /***** Free memory *****/
   /* Free memory used for list of users */
   Usr_FreeListOtherRecipients ();
   Usr_FreeListsSelectedEncryptedUsrsCods (&Gbl.Usrs.Selected);

   /***** Update received message setting Replied field to true *****/
   if (Replied)
      Msg_SetReceivedMsgAsReplied (OriginalMsgCod);

   /***** Write final message *****/
   if (NumRecipients)
     {
      if (NumRecipients == 1)
         Ale_ShowAlert (Ale_SUCCESS,Txt_The_message_has_been_sent_to_1_recipient);
      else
         Ale_ShowAlert (Ale_SUCCESS,Txt_The_message_has_been_sent_to_X_recipients,
                        (unsigned) NumRecipients);
     }
   else
      Ale_ShowAlert (Ale_WARNING,Txt_The_message_has_not_been_sent_to_any_recipient);

   /***** Show alert about errors on sending message *****/
   if (NumErrors > 1)
      Ale_ShowAlert (Ale_ERROR,Txt_There_have_been_X_errors_in_sending_the_message,
                    (unsigned) NumErrors);
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
   extern const char *Txt_Delete_messages_received;

   /***** Get parameters *****/
   Msg_GetParamMsgsCrsCod ();
   Msg_GetParamFilterFromTo ();
   Msg_GetParamFilterContent ();
   Msg_GetParamOnlyUnreadMsgs ();

   /***** Show question and button to remove messages received *****/
   /* Start alert */
   if (Gbl.Msg.FilterContent[0])
     {
      if (Gbl.Msg.ShowOnlyUnreadMsgs)
         Ale_ShowAlertAndButton1 (Ale_QUESTION,Txt_Do_you_really_want_to_delete_the_unread_messages_received_from_USER_X_from_COURSE_Y_related_to_CONTENT_Z,
				  Gbl.Msg.FilterFromTo[0] ? Gbl.Msg.FilterFromTo :
							    Txt_any_user,
				  Gbl.Msg.FilterCrsShrtName,Gbl.Msg.FilterContent);
      else
         Ale_ShowAlertAndButton1 (Ale_QUESTION,Txt_Do_you_really_want_to_delete_all_messages_received_from_USER_X_from_COURSE_Y_related_to_CONTENT_Z,
				  Gbl.Msg.FilterFromTo[0] ? Gbl.Msg.FilterFromTo :
							    Txt_any_user,
				  Gbl.Msg.FilterCrsShrtName,Gbl.Msg.FilterContent);
     }
   else
     {
      if (Gbl.Msg.ShowOnlyUnreadMsgs)
         Ale_ShowAlertAndButton1 (Ale_QUESTION,Txt_Do_you_really_want_to_delete_the_unread_messages_received_from_USER_X_from_COURSE_Y,
				  Gbl.Msg.FilterFromTo[0] ? Gbl.Msg.FilterFromTo :
							    Txt_any_user,
				  Gbl.Msg.FilterCrsShrtName);
      else
         Ale_ShowAlertAndButton1 (Ale_QUESTION,Txt_Do_you_really_want_to_delete_all_messages_received_from_USER_X_from_COURSE_Y,
				  Gbl.Msg.FilterFromTo[0] ? Gbl.Msg.FilterFromTo :
							    Txt_any_user,
				  Gbl.Msg.FilterCrsShrtName);
     }

   /* Show received messages again */
   Gbl.Msg.TypeOfMessages = Msg_MESSAGES_RECEIVED;
   Msg_ShowSentOrReceivedMessages ();

   /* End alert */
   Ale_ShowAlertAndButton2 (ActDelAllRcvMsg,NULL,NULL,
                            Msg_PutHiddenParamsMsgsFilters,
                            Btn_REMOVE_BUTTON,Txt_Delete_messages_received);
  }

/*****************************************************************************/
/******************* Request deletion of all sent messages *******************/
/*****************************************************************************/

void Msg_ReqDelAllSntMsgs (void)
  {
   extern const char *Txt_Do_you_really_want_to_delete_all_messages_sent_to_USER_X_from_COURSE_Y_related_to_CONTENT_Z;
   extern const char *Txt_any_user;
   extern const char *Txt_Do_you_really_want_to_delete_all_messages_sent_to_USER_X_from_COURSE_Y;
   extern const char *Txt_Delete_messages_sent;

   /***** Get parameters *****/
   Msg_GetParamMsgsCrsCod ();
   Msg_GetParamFilterFromTo ();
   Msg_GetParamFilterContent ();

   /***** Show question and button to remove messages received *****/
   /* Start alert */
   if (Gbl.Msg.FilterContent[0])
      Ale_ShowAlertAndButton1 (Ale_QUESTION,Txt_Do_you_really_want_to_delete_all_messages_sent_to_USER_X_from_COURSE_Y_related_to_CONTENT_Z,
			       Gbl.Msg.FilterFromTo[0] ? Gbl.Msg.FilterFromTo :
							 Txt_any_user,
			       Gbl.Msg.FilterCrsShrtName,Gbl.Msg.FilterContent);
   else
      Ale_ShowAlertAndButton1 (Ale_QUESTION,Txt_Do_you_really_want_to_delete_all_messages_sent_to_USER_X_from_COURSE_Y,
			       Gbl.Msg.FilterFromTo[0] ? Gbl.Msg.FilterFromTo :
							 Txt_any_user,
			       Gbl.Msg.FilterCrsShrtName);

   /* Show sent messages again */
   Gbl.Msg.TypeOfMessages = Msg_MESSAGES_SENT;
   Msg_ShowSentOrReceivedMessages ();

   /* End alert */
   Ale_ShowAlertAndButton2 (ActDelAllSntMsg,NULL,NULL,
                            Msg_PutHiddenParamsMsgsFilters,
                            Btn_REMOVE_BUTTON,Txt_Delete_messages_sent);
  }

/*****************************************************************************/
/*********************** Delete all received messages ************************/
/*****************************************************************************/

void Msg_DelAllRecMsgs (void)
  {
   char FilterFromToSubquery[Msg_MAX_BYTES_MESSAGES_QUERY + 1];
   unsigned long NumMsgs;

   /***** Get parameters *****/
   Msg_GetParamMsgsCrsCod ();
   Msg_GetParamFilterFromTo ();
   Msg_GetParamFilterContent ();
   Msg_GetParamOnlyUnreadMsgs ();
   Msg_MakeFilterFromToSubquery (FilterFromToSubquery);

   /***** Delete messages *****/
   NumMsgs = Msg_DelSomeRecOrSntMsgsUsr (Msg_MESSAGES_RECEIVED,Gbl.Usrs.Me.UsrDat.UsrCod,Gbl.Msg.FilterCrsCod,FilterFromToSubquery);
   Msg_ShowNumMsgsDeleted (NumMsgs);
   Msg_ShowRecMsgs ();
  }

/*****************************************************************************/
/************************* Delete all sent messages **************************/
/*****************************************************************************/

void Msg_DelAllSntMsgs (void)
  {
   char FilterFromToSubquery[Msg_MAX_BYTES_MESSAGES_QUERY + 1];
   unsigned long NumMsgs;

   /***** Get parameters *****/
   Msg_GetParamMsgsCrsCod ();
   Msg_GetParamFilterFromTo ();
   Msg_GetParamFilterContent ();
   Msg_MakeFilterFromToSubquery (FilterFromToSubquery);

   /***** Delete messages *****/
   NumMsgs = Msg_DelSomeRecOrSntMsgsUsr (Msg_MESSAGES_SENT,Gbl.Usrs.Me.UsrDat.UsrCod,Gbl.Msg.FilterCrsCod,FilterFromToSubquery);
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

void Msg_GetParamMsgsCrsCod (void)
  {
   extern const char *Txt_any_course;
   struct Course Crs;

   if ((Gbl.Msg.FilterCrsCod = Par_GetParToLong ("FilterCrsCod")) > 0)	// If origin course specified
     {
      /* Get data of course */
      Crs.CrsCod = Gbl.Msg.FilterCrsCod;
      Crs_GetDataOfCourseByCod (&Crs);

      Str_Copy (Gbl.Msg.FilterCrsShrtName,Crs.ShrtName,
                Hie_MAX_BYTES_SHRT_NAME);
     }
   else
      Str_Copy (Gbl.Msg.FilterCrsShrtName,Txt_any_course,
                Hie_MAX_BYTES_SHRT_NAME);
  }

/*****************************************************************************/
/******************* Get parameter with "from"/"to" filter ********************/
/*****************************************************************************/

void Msg_GetParamFilterFromTo (void)
  {
   /***** Get "from"/"to" filter *****/
   Par_GetParToText ("FilterFromTo",Gbl.Msg.FilterFromTo,
                     Usr_MAX_BYTES_FULL_NAME);
  }

/*****************************************************************************/
/********************* Get parameter with content filter *********************/
/*****************************************************************************/

void Msg_GetParamFilterContent (void)
  {
   /***** Get content filter *****/
   Par_GetParToText ("FilterContent",Gbl.Msg.FilterContent,
                     Msg_MAX_BYTES_FILTER_CONTENT);
  }

/*****************************************************************************/
/************************* Make "from"/"to" subquery *************************/
/*****************************************************************************/

static void Msg_MakeFilterFromToSubquery (char FilterFromToSubquery[Msg_MAX_BYTES_MESSAGES_QUERY + 1])
  {
   const char *Ptr;
   char SearchWord[Usr_MAX_BYTES_FIRSTNAME_OR_SURNAME + 1];

   /***** Split "from"/"to" string into words *****/
   if (Gbl.Msg.FilterFromTo[0])
     {
      Ptr = Gbl.Msg.FilterFromTo;
      Str_Copy (FilterFromToSubquery,
                " AND CONCAT(usr_data.FirstName,' ',usr_data.Surname1,' ',usr_data.Surname2) LIKE '",
                Msg_MAX_BYTES_MESSAGES_QUERY);
      while (*Ptr)
        {
         Str_GetNextStringUntilSpace (&Ptr,SearchWord,Usr_MAX_BYTES_FIRSTNAME_OR_SURNAME);
         if (strlen (FilterFromToSubquery) + strlen (SearchWord) + 512 >
             Msg_MAX_BYTES_MESSAGES_QUERY)	// Prevent string overflow
            break;
         Str_Concat (FilterFromToSubquery,"%",
                     Msg_MAX_BYTES_MESSAGES_QUERY);
         Str_Concat (FilterFromToSubquery,SearchWord,
                     Msg_MAX_BYTES_MESSAGES_QUERY);
        }
      Str_Concat (FilterFromToSubquery,"%'",
                  Msg_MAX_BYTES_MESSAGES_QUERY);
     }
   else
      FilterFromToSubquery[0] = '\0';
  }

/*****************************************************************************/
/****************************** Delete a sent message ************************/
/*****************************************************************************/

void Msg_DelSntMsg (void)
  {
   extern const char *Txt_Message_deleted;
   long MsgCod;

   /***** Get the code of the message to delete *****/
   if ((MsgCod = Msg_GetParamMsgCod ()) <= 0)
      Lay_ShowErrorAndExit ("Wrong code of message.");

   /***** Delete the message *****/
   /* Delete the sent message */
   Msg_MoveSentMsgToDeleted (MsgCod);
   Ale_ShowAlert (Ale_SUCCESS,Txt_Message_deleted);

   /* Show the remaining messages */
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
   if ((MsgCod = Msg_GetParamMsgCod ()) <= 0)
      Lay_ShowErrorAndExit ("Wrong code of message.");

   /***** Delete the message *****/
   /* Delete the received message */
   Msg_MoveReceivedMsgToDeleted (MsgCod,Gbl.Usrs.Me.UsrDat.UsrCod);
   Ale_ShowAlert (Ale_SUCCESS,Txt_Message_deleted);

   /* Show the remaining messages */
   Msg_ShowRecMsgs ();
  }

/*****************************************************************************/
/**************************** Expand a sent message **************************/
/*****************************************************************************/

void Msg_ExpSntMsg (void)
  {
   /***** Get the code of the message to expand *****/
   if ((Gbl.Msg.ExpandedMsgCod = Msg_GetParamMsgCod ()) <= 0)
      Lay_ShowErrorAndExit ("Wrong code of message.");

   /***** Expand the message *****/
   Msg_ExpandSentMsg (Gbl.Msg.ExpandedMsgCod);

   /***** Show again the messages *****/
   Msg_ShowSntMsgs ();
  }

/*****************************************************************************/
/************************** Expand a received message ************************/
/*****************************************************************************/

void Msg_ExpRecMsg (void)
  {
   /***** Get the code of the message to expand *****/
   if ((Gbl.Msg.ExpandedMsgCod = Msg_GetParamMsgCod ()) <= 0)
      Lay_ShowErrorAndExit ("Wrong code of message.");

   /***** Expand the message *****/
   Msg_ExpandReceivedMsg (Gbl.Msg.ExpandedMsgCod);

   /***** Mark possible notification as seen *****/
   Ntf_MarkNotifAsSeen (Ntf_EVENT_MESSAGE,
	               Gbl.Msg.ExpandedMsgCod,-1L,
	               Gbl.Usrs.Me.UsrDat.UsrCod);

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
   if ((MsgCod = Msg_GetParamMsgCod ()) <= 0)
      Lay_ShowErrorAndExit ("Wrong code of message.");

   /***** Contract the message *****/
   Msg_ContractSentMsg (MsgCod);

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
   if ((MsgCod = Msg_GetParamMsgCod ()) <= 0)
      Lay_ShowErrorAndExit ("Wrong code of message.");

   /***** Contract the message *****/
   Msg_ContractReceivedMsg (MsgCod);

   /***** Show again the messages *****/
   Msg_ShowRecMsgs ();
  }

/*****************************************************************************/
/**************************** Expand a sent message **************************/
/*****************************************************************************/

static void Msg_ExpandSentMsg (long MsgCod)
  {
   /***** Expand message in sent message table *****/
   DB_QueryUPDATE ("can not expand a sent message",
		   "UPDATE msg_snt SET Expanded='Y'"
		   " WHERE MsgCod=%ld AND UsrCod=%ld",
                   MsgCod,Gbl.Usrs.Me.UsrDat.UsrCod);

   /***** Contract all my other messages in sent message table *****/
   DB_QueryUPDATE ("can not contract a sent message",
		   "UPDATE msg_snt SET Expanded='N'"
		   " WHERE UsrCod=%ld AND MsgCod<>%ld",
                   Gbl.Usrs.Me.UsrDat.UsrCod,MsgCod);
  }

/*****************************************************************************/
/************************* Expand a received message *************************/
/*****************************************************************************/

static void Msg_ExpandReceivedMsg (long MsgCod)
  {
   /***** Expand message in received message table and mark it as read by me *****/
   DB_QueryUPDATE ("can not expand a received message",
		   "UPDATE msg_rcv SET Open='Y',Expanded='Y'"
		   " WHERE MsgCod=%ld AND UsrCod=%ld",
                   MsgCod,Gbl.Usrs.Me.UsrDat.UsrCod);

   /***** Contract all my other messages in received message table *****/
   DB_QueryUPDATE ("can not contract a received message",
		   "UPDATE msg_rcv SET Expanded='N'"
		   " WHERE UsrCod=%ld AND MsgCod<>%ld",
                   Gbl.Usrs.Me.UsrDat.UsrCod,MsgCod);
  }

/*****************************************************************************/
/************************** Contract a sent message **************************/
/*****************************************************************************/

static void Msg_ContractSentMsg (long MsgCod)
  {
   /***** Contract message in sent message table *****/
   DB_QueryUPDATE ("can not contract a sent message",
		   "UPDATE msg_snt SET Expanded='N'"
		   " WHERE MsgCod=%ld AND UsrCod=%ld",
                   MsgCod,Gbl.Usrs.Me.UsrDat.UsrCod);
  }

/*****************************************************************************/
/************************ Contract a received message ************************/
/*****************************************************************************/

static void Msg_ContractReceivedMsg (long MsgCod)
  {
   /***** Contract message in received message table *****/
   DB_QueryUPDATE ("can not contract a received message",
		   "UPDATE msg_rcv SET Expanded='N'"
		   " WHERE MsgCod=%ld AND UsrCod=%ld",
                   MsgCod,Gbl.Usrs.Me.UsrDat.UsrCod);
  }

/*****************************************************************************/
/********************** Mark a received message as open **********************/
/*****************************************************************************/

void Msg_SetReceivedMsgAsOpen (long MsgCod,long UsrCod)
  {
   /***** Mark message as read by user *****/
   DB_QueryUPDATE ("can not mark a received message as open",
		   "UPDATE msg_rcv SET Open='Y'"
		   " WHERE MsgCod=%ld AND UsrCod=%ld",
                   MsgCod,UsrCod);
  }

/*****************************************************************************/
/********************** Insert a message in the database *********************/
/*****************************************************************************/
// Return the code of the new inserted message

static long Msg_InsertNewMsg (const char *Subject,const char *Content,
                              struct Media *Media)
  {
   long MsgCod;

   /***** Store media in filesystem and database *****/
   Med_RemoveKeepOrStoreMedia (-1L,Media);

   /***** Insert message subject and content in the database *****/
   MsgCod =
   DB_QueryINSERTandReturnCode ("can not create message",
				"INSERT INTO msg_content"
				" (Subject,Content,MedCod)"
				" VALUES"
				" ('%s','%s',%ld)",
				Subject,Content,Media->MedCod);

   /***** Insert message in sent messages *****/
   DB_QueryINSERT ("can not create message",
		   "INSERT INTO msg_snt"
		   " (MsgCod,CrsCod,UsrCod,Expanded,CreatTime)"
		   " VALUES"
		   " (%ld,%ld,%ld,'N',NOW())",
	           MsgCod,
	           Gbl.Hierarchy.Crs.CrsCod,
	           Gbl.Usrs.Me.UsrDat.UsrCod);

   /***** Increment number of messages sent by me *****/
   Prf_IncrementNumMsgSntUsr (Gbl.Usrs.Me.UsrDat.UsrCod);

   return MsgCod;
  }

/*****************************************************************************/
/************** Delete some received or sent messages of a user **************/
/*****************************************************************************/

static unsigned long Msg_DelSomeRecOrSntMsgsUsr (Msg_TypeOfMessages_t TypeOfMessages,long UsrCod,
                                                 long FilterCrsCod,const char *FilterFromToSubquery)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long MsgNum;
   unsigned long NumMsgs;
   long MsgCod;

   /***** Get some of the messages received or sent by this user from database *****/
   NumMsgs = Msg_GetSentOrReceivedMsgs (UsrCod,
				        FilterCrsCod,
					FilterFromToSubquery,
				        &mysql_res);

   /***** Delete each message *****/
   for (MsgNum = 0;
	MsgNum < NumMsgs;
	MsgNum++)
     {
      row = mysql_fetch_row (mysql_res);
      if (sscanf (row[0],"%ld",&MsgCod) != 1)
         Lay_ShowErrorAndExit ("Wrong code of message.");
      switch (TypeOfMessages)
        {
         case Msg_MESSAGES_RECEIVED:
            Msg_MoveReceivedMsgToDeleted (MsgCod,UsrCod);
            break;
         case Msg_MESSAGES_SENT:
            Msg_MoveSentMsgToDeleted (MsgCod);
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
/*************** Delete all received or sent messages of a user **************/
/*****************************************************************************/

void Msg_DelAllRecAndSntMsgsUsr (long UsrCod)
  {
   /***** Move messages from msg_rcv to msg_rcv_deleted *****/
   /* Insert messages into msg_rcv_deleted */
   DB_QueryINSERT ("can not remove received messages",
		   "INSERT IGNORE INTO msg_rcv_deleted"
		   " (MsgCod,UsrCod,Notified,Open,Replied)"
		   " SELECT MsgCod,UsrCod,Notified,Open,Replied FROM msg_rcv"
		   " WHERE UsrCod=%ld",
                   UsrCod);

   /* Delete messages from msg_rcv *****/
   DB_QueryDELETE ("can not remove received messages",
		   "DELETE FROM msg_rcv WHERE UsrCod=%ld",
		   UsrCod);

   /***** Move message from msg_snt to msg_snt_deleted *****/
   /* Insert message into msg_snt_deleted */
   DB_QueryINSERT ("can not remove sent messages",
		   "INSERT IGNORE INTO msg_snt_deleted"
		   " (MsgCod,CrsCod,UsrCod,CreatTime)"
		   " SELECT MsgCod,CrsCod,UsrCod,CreatTime"
		   " FROM msg_snt WHERE UsrCod=%ld",
                   UsrCod);

   /* Delete message from msg_snt *****/
   DB_QueryDELETE ("can not remove sent messages",
		   "DELETE FROM msg_snt WHERE UsrCod=%ld",
		   UsrCod);
  }

/*****************************************************************************/
/**** Insert a message y su destinatario in the table of messages received ***/
/*****************************************************************************/

static void Msg_InsertReceivedMsgIntoDB (long MsgCod,long UsrCod,bool NotifyByEmail)
  {
   /***** Insert message received in the database *****/
   DB_QueryINSERT ("can not create received message",
		   "INSERT INTO msg_rcv"
		   " (MsgCod,UsrCod,Notified,Open,Replied,Expanded)"
		   " VALUES"
		   " (%ld,%ld,'%c','N','N','N')",
	           MsgCod,UsrCod,
	           NotifyByEmail ? 'Y' :
			           'N');
  }

/*****************************************************************************/
/******** Update received message by setting Replied field to true ***********/
/*****************************************************************************/

static void Msg_SetReceivedMsgAsReplied (long MsgCod)
  {
   /***** Update received message by setting Replied field to true *****/
   DB_QueryUPDATE ("can not update a received message",
		   "UPDATE msg_rcv SET Replied='Y'"
		   " WHERE MsgCod=%ld AND UsrCod=%ld",
                   MsgCod,Gbl.Usrs.Me.UsrDat.UsrCod);
  }

/*****************************************************************************/
/************ Delete a message from the received message table ***************/
/*****************************************************************************/

static void Msg_MoveReceivedMsgToDeleted (long MsgCod,long UsrCod)
  {
   /***** Move message from msg_rcv to msg_rcv_deleted *****/
   /* Insert message into msg_rcv_deleted */
   DB_QueryINSERT ("can not remove a received message",
		   "INSERT IGNORE INTO msg_rcv_deleted"
		   " (MsgCod,UsrCod,Notified,Open,Replied)"
		   " SELECT MsgCod,UsrCod,Notified,Open,Replied"
		   " FROM msg_rcv WHERE MsgCod=%ld AND UsrCod=%ld",
                   MsgCod,UsrCod);

   /* Delete message from msg_rcv *****/
   DB_QueryDELETE ("can not remove a received message",
		   "DELETE FROM msg_rcv WHERE MsgCod=%ld AND UsrCod=%ld",
                   MsgCod,UsrCod);

   /***** If message content is not longer necessary, move it to msg_content_deleted *****/
   if (Msg_CheckIfSentMsgIsDeleted (MsgCod))
      if (Msg_CheckIfReceivedMsgIsDeletedForAllItsRecipients (MsgCod))
         Msg_MoveMsgContentToDeleted (MsgCod);

   /***** Mark possible notifications as removed *****/
   Ntf_MarkNotifToOneUsrAsRemoved (Ntf_EVENT_MESSAGE,MsgCod,UsrCod);
  }

/*****************************************************************************/
/************** Delete a message from the sent message table *****************/
/*****************************************************************************/

static void Msg_MoveSentMsgToDeleted (long MsgCod)
  {
   /***** Move message from msg_snt to msg_snt_deleted *****/
   /* Insert message into msg_snt_deleted */
   DB_QueryINSERT ("can not remove a sent message",
		   "INSERT IGNORE INTO msg_snt_deleted"
		   " (MsgCod,CrsCod,UsrCod,CreatTime)"
		   " SELECT MsgCod,CrsCod,UsrCod,CreatTime"
		   " FROM msg_snt WHERE MsgCod=%ld",
                   MsgCod);

   /* Delete message from msg_snt *****/
   DB_QueryDELETE ("can not remove a sent message",
		   "DELETE FROM msg_snt WHERE MsgCod=%ld",
		   MsgCod);

   /***** If message content is not longer necessary, move it to msg_content_deleted *****/
   if (Msg_CheckIfReceivedMsgIsDeletedForAllItsRecipients (MsgCod))
      Msg_MoveMsgContentToDeleted (MsgCod);
  }

/*****************************************************************************/
/*************** Delete the subject and content of a message *****************/
/*****************************************************************************/

static void Msg_MoveMsgContentToDeleted (long MsgCod)
  {
   /***** Move message from msg_content to msg_content_deleted *****/
   /* Insert message content into msg_content_deleted */
   DB_QueryINSERT ("can not remove the content of a message",
		   "INSERT IGNORE INTO msg_content_deleted"
		   " (MsgCod,Subject,Content,MedCod)"
		   " SELECT MsgCod,Subject,Content,MedCod"
		   " FROM msg_content WHERE MsgCod=%ld",
                   MsgCod);

   /* TODO: Messages in msg_content_deleted older than a certain time
      should be deleted to ensure the protection of personal data */

   /* Delete message from msg_content *****/
   DB_QueryUPDATE ("can not remove the content of a message",
		   "DELETE FROM msg_content WHERE MsgCod=%ld",
		   MsgCod);
  }

/*****************************************************************************/
/***** Delete the subject and content of all completely deleted messages *****/
/*****************************************************************************/

void Msg_MoveUnusedMsgsContentToDeleted (void)
  {
   /***** Move messages from msg_content to msg_content_deleted *****/
   /* Insert message content into msg_content_deleted */
   DB_QueryINSERT ("can not remove the content of some messages",
		   "INSERT IGNORE INTO msg_content_deleted"
		   " (MsgCod,Subject,Content)"
		   " SELECT MsgCod,Subject,Content FROM msg_content"
		   " WHERE MsgCod NOT IN (SELECT MsgCod FROM msg_snt)"
		   " AND MsgCod NOT IN (SELECT DISTINCT MsgCod FROM msg_rcv)");

   /* Messages in msg_content_deleted older than a certain time
      should be deleted to ensure the protection of personal data */

   /* Delete message from msg_content *****/
   DB_QueryUPDATE ("can not remove the content of some messages",
		   "DELETE FROM msg_content"
		   " WHERE MsgCod NOT IN (SELECT MsgCod FROM msg_snt)"
		   " AND MsgCod NOT IN (SELECT DISTINCT MsgCod FROM msg_rcv)");
  }

/*****************************************************************************/
/******************** Check if a sent message is deleted *********************/
/*****************************************************************************/

static bool Msg_CheckIfSentMsgIsDeleted (long MsgCod)
  {
   /***** Get if the message code is in table of sent messages not deleted *****/
   return (DB_QueryCOUNT ("can not check if a sent message is deleted",
			  "SELECT COUNT(*) FROM msg_snt"
			  " WHERE MsgCod=%ld",
			  MsgCod) == 0);	// The message has been deleted
						// by its author when it is not present
						// in table of sent messages undeleted
  }

/*****************************************************************************/
/***** Check if a received message has been deleted by all its recipients ****/
/*****************************************************************************/

static bool Msg_CheckIfReceivedMsgIsDeletedForAllItsRecipients (long MsgCod)
  {
   /***** Get if the message code is in table of received messages not deleted *****/
   return (DB_QueryCOUNT ("can not check if a received message"
			  " is deleted by all recipients",
			  "SELECT COUNT(*) FROM msg_rcv"
			  " WHERE MsgCod=%ld",
			  MsgCod) == 0);	// The message has been deleted
						// by all its recipients when it is not present
						// in table of received messages undeleted
  }

/*****************************************************************************/
/******** Get number of received messages that haven't been read by me *******/
/*****************************************************************************/

static unsigned Msg_GetNumUnreadMsgs (long FilterCrsCod,const char *FilterFromToSubquery)
  {
   char SubQuery[Msg_MAX_BYTES_MESSAGES_QUERY + 1];
   unsigned NumMsgs;

   /***** Get number of unread messages from database *****/
   if (FilterCrsCod >= 0)	// If origin course selected
     {
      if (FilterFromToSubquery[0])
         sprintf (SubQuery,"SELECT msg_rcv.MsgCod FROM msg_rcv,msg_snt,usr_data"
                           " WHERE msg_rcv.UsrCod=%ld AND msg_rcv.Open='N'"
                           " AND msg_rcv.MsgCod=msg_snt.MsgCod"
                           " AND msg_snt.CrsCod=%ld"
                           " AND msg_snt.UsrCod=usr_data.UsrCod%s",
                  Gbl.Usrs.Me.UsrDat.UsrCod,
                  FilterCrsCod,
                  FilterFromToSubquery);
      else
         sprintf (SubQuery,"SELECT msg_rcv.MsgCod FROM msg_rcv,msg_snt"
                           " WHERE msg_rcv.UsrCod=%ld AND msg_rcv.Open='N'"
                           " AND msg_rcv.MsgCod=msg_snt.MsgCod"
                           " AND msg_snt.CrsCod=%ld",
                  Gbl.Usrs.Me.UsrDat.UsrCod,
                  FilterCrsCod);
     }
   else	// If no origin course selected
     {
      if (FilterFromToSubquery[0])
         sprintf (SubQuery,"SELECT msg_rcv.MsgCod FROM msg_rcv,msg_snt,usr_data"
                           " WHERE msg_rcv.UsrCod=%ld AND msg_rcv.Open='N'"
                           " AND msg_rcv.MsgCod=msg_snt.MsgCod"
                           " AND msg_snt.UsrCod=usr_data.UsrCod%s",
                  Gbl.Usrs.Me.UsrDat.UsrCod,
                  FilterFromToSubquery);
      else
         sprintf (SubQuery,"SELECT MsgCod FROM msg_rcv"
                           " WHERE UsrCod=%ld AND Open='N'",
                  Gbl.Usrs.Me.UsrDat.UsrCod);
     }

   if (Gbl.Msg.FilterContent[0])
      NumMsgs =
      (unsigned) DB_QueryCOUNT ("can not get number of unread messages",
				"SELECT COUNT(*) FROM msg_content"
				" WHERE MsgCod IN (%s)"
				" AND MATCH (Subject,Content) AGAINST ('%s')",
				SubQuery,
				Gbl.Msg.FilterContent);
   else
      NumMsgs =
      (unsigned) DB_QueryCOUNT ("can not get number of unread messages",
				"SELECT COUNT(*) FROM (%s) AS T",
				SubQuery);

   return NumMsgs;
  }

/*****************************************************************************/
/********************* Show messages sent to other users *********************/
/*****************************************************************************/

void Msg_ShowSntMsgs (void)
  {
   /***** Show the sent messages *****/
   Gbl.Msg.TypeOfMessages = Msg_MESSAGES_SENT;
   Msg_ShowSentOrReceivedMessages ();
  }

/*****************************************************************************/
/******************* Show messages received from other users *****************/
/*****************************************************************************/

void Msg_ShowRecMsgs (void)
  {
   if (Msg_GetNumUsrsBannedByMe ())
     {
      /***** Contextual menu *****/
      Mnu_ContextMenuBegin ();
      Msg_PutLinkToViewBannedUsers ();	// View banned users
      Mnu_ContextMenuEnd ();
     }

   /***** Show the received messages *****/
   Gbl.Msg.TypeOfMessages = Msg_MESSAGES_RECEIVED;
   Msg_ShowSentOrReceivedMessages ();
  }

/*****************************************************************************/
/************************ Show sent or received messages *********************/
/*****************************************************************************/

static void Msg_ShowSentOrReceivedMessages (void)
  {
   extern const char *Hlp_MESSAGES_Received;
   extern const char *Hlp_MESSAGES_Received_filter;
   extern const char *Hlp_MESSAGES_Sent;
   extern const char *Hlp_MESSAGES_Sent_filter;
   extern const char *The_ClassFormLinkInBoxBold[The_NUM_THEMES];
   extern const char *Txt_Filter;
   extern const char *Txt_Update_messages;
   char FilterFromToSubquery[Msg_MAX_BYTES_MESSAGES_QUERY + 1];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRow;
   unsigned long NumRows;
   unsigned long NumMsg = 0;		// Initialized to avoid warning
   unsigned NumUnreadMsgs = 0;		// Initialized to avoid warning
   struct Pagination Pagination;
   long MsgCod;
   static const Act_Action_t ActionSee[Msg_NUM_TYPES_OF_MSGS] =
     {
      ActSeeRcvMsg,
      ActSeeSntMsg,
     };
   static const Pag_WhatPaginate_t WhatPaginate[Msg_NUM_TYPES_OF_MSGS] =
     {
      Pag_MESSAGES_RECEIVED,
      Pag_MESSAGES_SENT,
     };
   const char *Help[Msg_NUM_TYPES_OF_MSGS] =
     {
      Hlp_MESSAGES_Received,
      Hlp_MESSAGES_Sent,
     };
   const char *HelpFilter[Msg_NUM_TYPES_OF_MSGS] =
     {
      Hlp_MESSAGES_Received_filter,
      Hlp_MESSAGES_Sent_filter,
     };

   /***** Get the page number *****/
   Gbl.Msg.CurrentPage = Pag_GetParamPagNum (WhatPaginate[Gbl.Msg.TypeOfMessages]);

   /***** Get other parameters *****/
   Msg_GetParamMsgsCrsCod ();
   Msg_GetParamFilterFromTo ();
   Msg_GetParamFilterContent ();
   Msg_MakeFilterFromToSubquery (FilterFromToSubquery);
   Msg_GetDistinctCoursesInMyMessages ();

   /***** Get number of unread messages *****/
   switch (Gbl.Msg.TypeOfMessages)
     {
      case Msg_MESSAGES_RECEIVED:
         Msg_GetParamOnlyUnreadMsgs ();
         NumUnreadMsgs = Msg_GetNumUnreadMsgs (Gbl.Msg.FilterCrsCod,FilterFromToSubquery);
         break;
      case Msg_MESSAGES_SENT:
         NumUnreadMsgs = 0;
         break;
     }

   /***** Get messages from database *****/
   NumRows = Msg_GetSentOrReceivedMsgs (Gbl.Usrs.Me.UsrDat.UsrCod,
					Gbl.Msg.FilterCrsCod,
					FilterFromToSubquery,
				        &mysql_res);
   Gbl.Msg.NumMsgs = (unsigned) NumRows;

   /***** Begin box with messages *****/
   Box_BoxBegin ("97%",Msg_WriteNumMsgs (NumUnreadMsgs),Msg_PutIconsListMsgs,
                 Help[Gbl.Msg.TypeOfMessages],Box_NOT_CLOSABLE);

   /***** Filter messages *****/
   /* Begin box with filter */
   Box_BoxBegin (NULL,Txt_Filter,NULL,
                 HelpFilter[Gbl.Msg.TypeOfMessages],Box_CLOSABLE);

   /* Form to see messages again */
   Frm_StartForm (ActionSee[Gbl.Msg.TypeOfMessages]);

   HTM_DIV_Begin ("class=\"CM\"");
   Msg_ShowFormSelectCourseSentOrRecMsgs ();
   if (Gbl.Msg.TypeOfMessages == Msg_MESSAGES_RECEIVED)
      Msg_ShowFormToShowOnlyUnreadMessages ();
   HTM_DIV_End ();
   Msg_ShowFormToFilterMsgs ();

   /***** Contextual menu *****/
   Mnu_ContextMenuBegin ();
   HTM_BUTTON_Animated_Begin (Txt_Update_messages,
	                      The_ClassFormLinkInBoxBold[Gbl.Prefs.Theme],
			      NULL);
   Ico_PutCalculateIconWithText (Txt_Update_messages);	// Animated icon to update messages
   HTM_BUTTON_End ();
   Mnu_ContextMenuEnd ();

   Frm_EndForm ();

   /* End box */
   Box_BoxEnd ();


   if (Gbl.Msg.NumMsgs)		// If there are messages...
     {
      if (Gbl.Action.Act == ActExpRcvMsg)	// Expanding a message, perhaps it is the result of following a link
						// from a notification of received message, so show the page where the message is inside
        {
         /***** Get the page where the expanded message is inside *****/
         for (NumRow = 0;
              NumRow < NumRows;
              NumRow++)
           {
            row = mysql_fetch_row (mysql_res);
            if (sscanf (row[0],"%ld",&MsgCod) != 1)
               Lay_ShowErrorAndExit ("Wrong code of message when searching for a page.");

            if (MsgCod == Gbl.Msg.ExpandedMsgCod)	// Expanded message found
              {
               Gbl.Msg.CurrentPage = (unsigned) (NumRow / Pag_ITEMS_PER_PAGE) + 1;
               break;
              }
           }
        }

      /***** Compute variables related to pagination *****/
      Pagination.NumItems = Gbl.Msg.NumMsgs;
      Pagination.CurrentPage = (int) Gbl.Msg.CurrentPage;
      Pag_CalculatePagination (&Pagination);
      Gbl.Msg.CurrentPage = (unsigned) Pagination.CurrentPage;

      /***** Save my current page in order to show it next time I'll view my received/sent messages *****/
      Pag_SaveLastPageMsgIntoSession (WhatPaginate[Gbl.Msg.TypeOfMessages],
	                              Gbl.Msg.CurrentPage);

      /***** Write links to pages *****/
      if (Pagination.MoreThanOnePage)
         Pag_WriteLinksToPagesCentered (WhatPaginate[Gbl.Msg.TypeOfMessages],
                                        0,
                                        &Pagination);

      /***** Show received / sent messages in this page *****/
      HTM_TABLE_BeginWidePadding (2);

      mysql_data_seek (mysql_res,(my_ulonglong) (Pagination.FirstItemVisible - 1));
      for (NumRow = Pagination.FirstItemVisible;
           NumRow <= Pagination.LastItemVisible;
           NumRow++)
        {
         row = mysql_fetch_row (mysql_res);

         if (sscanf (row[0],"%ld",&MsgCod) != 1)
            Lay_ShowErrorAndExit ("Wrong code of message when listing the messages in a page.");
         NumMsg = NumRows - NumRow + 1;
         Msg_ShowASentOrReceivedMessage (NumMsg,MsgCod);
        }

      HTM_TABLE_End ();

      /***** Write again links to pages *****/
      if (Pagination.MoreThanOnePage)
         Pag_WriteLinksToPagesCentered (WhatPaginate[Gbl.Msg.TypeOfMessages],
                                        0,
                                        &Pagination);
     }

   /***** End box *****/
   Box_BoxEnd ();

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/********************* Get number of user I have banned **********************/
/*****************************************************************************/

static unsigned long Msg_GetNumUsrsBannedByMe (void)
  {
   /***** Get number of users I have banned *****/
   return DB_QueryCOUNT ("can not get number of users you have banned",
			 "SELECT COUNT(*) FROM msg_banned"
			 " WHERE ToUsrCod=%ld",
			 Gbl.Usrs.Me.UsrDat.UsrCod);
  }

/*****************************************************************************/
/****************** Put a link (form) to view banned users *******************/
/*****************************************************************************/

static void Msg_PutLinkToViewBannedUsers(void)
  {
   extern const char *Txt_Banned_users;

   Lay_PutContextualLinkIconText (ActLstBanUsr,NULL,NULL,
				  "lock.svg",
				  Txt_Banned_users);
  }

/*****************************************************************************/
/********* Generate a query to select messages received or sent **************/
/*****************************************************************************/

static unsigned long Msg_GetSentOrReceivedMsgs (long UsrCod,
						long FilterCrsCod,
						const char *FilterFromToSubquery,
						MYSQL_RES **mysql_res)
  {
   char *SubQuery;
   const char *StrUnreadMsg;
   unsigned long NumMsgs;

   if (FilterCrsCod > 0)	// If origin course selected
      switch (Gbl.Msg.TypeOfMessages)
        {
         case Msg_MESSAGES_RECEIVED:
            StrUnreadMsg = (Gbl.Msg.ShowOnlyUnreadMsgs ? " AND msg_rcv.Open='N'" :
        	                                         "");
            if (FilterFromToSubquery[0])
              {
               if (asprintf (&SubQuery,"(SELECT msg_rcv.MsgCod"
				       " FROM msg_rcv,msg_snt,usr_data"
				       " WHERE msg_rcv.UsrCod=%ld%s"
				       " AND msg_rcv.MsgCod=msg_snt.MsgCod"
				       " AND msg_snt.CrsCod=%ld"
				       " AND msg_snt.UsrCod=usr_data.UsrCod%s)"
				       " UNION "
				       "(SELECT msg_rcv.MsgCod"
				       " FROM msg_rcv,msg_snt_deleted,usr_data"
				       " WHERE msg_rcv.UsrCod=%ld%s"
				       " AND msg_rcv.MsgCod=msg_snt_deleted.MsgCod"
				       " AND msg_snt_deleted.CrsCod=%ld"
				       " AND msg_snt_deleted.UsrCod=usr_data.UsrCod%s)",
			     UsrCod,StrUnreadMsg,FilterCrsCod,FilterFromToSubquery,
			     UsrCod,StrUnreadMsg,FilterCrsCod,FilterFromToSubquery) < 0)
                  Lay_NotEnoughMemoryExit ();
              }
            else
              {
               if (asprintf (&SubQuery,"(SELECT msg_rcv.MsgCod"
				       " FROM msg_rcv,msg_snt"
				       " WHERE msg_rcv.UsrCod=%ld%s"
				       " AND msg_rcv.MsgCod=msg_snt.MsgCod"
				       " AND msg_snt.CrsCod=%ld)"
				       " UNION "
				       "(SELECT msg_rcv.MsgCod"
				       " FROM msg_rcv,msg_snt_deleted"
				       " WHERE msg_rcv.UsrCod=%ld%s"
				       " AND msg_rcv.MsgCod=msg_snt_deleted.MsgCod"
				       " AND msg_snt_deleted.CrsCod=%ld)",
			     UsrCod,StrUnreadMsg,FilterCrsCod,
			     UsrCod,StrUnreadMsg,FilterCrsCod) < 0)
                  Lay_NotEnoughMemoryExit ();
              }
            break;
         case Msg_MESSAGES_SENT:
            if (FilterFromToSubquery[0])
              {
               if (asprintf (&SubQuery,"(SELECT DISTINCT msg_snt.MsgCod"
				       " FROM msg_snt,msg_rcv,usr_data"
				       " WHERE msg_snt.UsrCod=%ld"
				       " AND msg_snt.CrsCod=%ld"
				       " AND msg_snt.MsgCod=msg_rcv.MsgCod"
				       " AND msg_rcv.UsrCod=usr_data.UsrCod%s)"
				       " UNION "
				       "(SELECT DISTINCT msg_snt.MsgCod"
				       " FROM msg_snt,msg_rcv_deleted,usr_data"
				       " WHERE msg_snt.UsrCod=%ld"
				       " AND msg_snt.CrsCod=%ld"
				       " AND msg_snt.MsgCod=msg_rcv_deleted.MsgCod"
				       " AND msg_rcv_deleted.UsrCod=usr_data.UsrCod%s)",
			     UsrCod,FilterCrsCod,FilterFromToSubquery,
			     UsrCod,FilterCrsCod,FilterFromToSubquery) < 0)
                  Lay_NotEnoughMemoryExit ();
              }
            else
              {
               if (asprintf (&SubQuery,"SELECT MsgCod"
				       " FROM msg_snt"
				       " WHERE UsrCod=%ld AND CrsCod=%ld",
			     UsrCod,FilterCrsCod) < 0)
                  Lay_NotEnoughMemoryExit ();
              }
            break;
         default: // Not aplicable here
            break;
        }
   else	// If no origin course selected
      switch (Gbl.Msg.TypeOfMessages)
        {
         case Msg_MESSAGES_RECEIVED:
            if (FilterFromToSubquery[0])
              {
               StrUnreadMsg = (Gbl.Msg.ShowOnlyUnreadMsgs ? " AND msg_rcv.Open='N'" :
        	                                            "");
               if (asprintf (&SubQuery,"(SELECT msg_rcv.MsgCod"
				       " FROM msg_rcv,msg_snt,usr_data"
				       " WHERE msg_rcv.UsrCod=%ld%s"
				       " AND msg_rcv.MsgCod=msg_snt.MsgCod"
				       " AND msg_snt.UsrCod=usr_data.UsrCod%s)"
				       " UNION "
				       "(SELECT msg_rcv.MsgCod"
				       " FROM msg_rcv,msg_snt_deleted,usr_data"
				       " WHERE msg_rcv.UsrCod=%ld%s"
				       " AND msg_rcv.MsgCod=msg_snt_deleted.MsgCod"
				       " AND msg_snt_deleted.UsrCod=usr_data.UsrCod%s)",
			     UsrCod,StrUnreadMsg,FilterFromToSubquery,
			     UsrCod,StrUnreadMsg,FilterFromToSubquery) < 0)
                  Lay_NotEnoughMemoryExit ();
              }
            else
              {
               StrUnreadMsg = (Gbl.Msg.ShowOnlyUnreadMsgs ? " AND Open='N'" :
        	                                            "");
               if (asprintf (&SubQuery,"SELECT MsgCod"
				       " FROM msg_rcv"
				       " WHERE UsrCod=%ld%s",
			     UsrCod,StrUnreadMsg) < 0)
                  Lay_NotEnoughMemoryExit ();
              }
            break;
         case Msg_MESSAGES_SENT:
            if (FilterFromToSubquery[0])
              {
               if (asprintf (&SubQuery,"(SELECT msg_snt.MsgCod"
				       " FROM msg_snt,msg_rcv,usr_data"
				       " WHERE msg_snt.UsrCod=%ld"
				       " AND msg_snt.MsgCod=msg_rcv.MsgCod"
				       " AND msg_rcv.UsrCod=usr_data.UsrCod%s)"
				       " UNION "
				       "(SELECT msg_snt.MsgCod"
				       " FROM msg_snt,msg_rcv_deleted,usr_data"
				       " WHERE msg_snt.UsrCod=%ld"
				       " AND msg_snt.MsgCod=msg_rcv_deleted.MsgCod"
				       " AND msg_rcv_deleted.UsrCod=usr_data.UsrCod%s)",
			     UsrCod,FilterFromToSubquery,
			     UsrCod,FilterFromToSubquery) < 0)
                  Lay_NotEnoughMemoryExit ();
              }
            else
              {
               if (asprintf (&SubQuery,"SELECT MsgCod"
				       " FROM msg_snt"
				       " WHERE UsrCod=%ld",
                             UsrCod) < 0)
                  Lay_NotEnoughMemoryExit ();
              }
            break;
         default: // Not aplicable here
            break;
        }

   if (Gbl.Msg.FilterContent[0])
      /* Match against the content written in filter form */
      NumMsgs = DB_QuerySELECT (mysql_res,"can not get messages",
				"SELECT MsgCod"
				" FROM msg_content"
				" WHERE MsgCod IN (SELECT MsgCod FROM (%s) AS M)"
				" AND MATCH (Subject,Content) AGAINST ('%s')"
				" ORDER BY MsgCod DESC",	// End the query ordering the result from most recent message to oldest
				SubQuery,Gbl.Msg.FilterContent);
   else
      NumMsgs = DB_QuerySELECT (mysql_res,"can not get messages",
				"%s"
				" ORDER BY MsgCod DESC",	// End the query ordering the result from most recent message to oldest
				SubQuery);

   /***** Free memory used for subquery *****/
   free (SubQuery);

   return NumMsgs;
  }

/*****************************************************************************/
/**** Get the number of unique messages sent by any teacher from a course ****/
/*****************************************************************************/

unsigned Msg_GetNumMsgsSentByTchsCrs (long CrsCod)
  {
   /***** Get the number of unique messages sent by any teacher from this course *****/
   return
   (unsigned) DB_QueryCOUNT ("can not get the number of messages"
			     " sent by teachers",
			     "SELECT COUNT(*) FROM msg_snt,crs_usr"
			     " WHERE msg_snt.CrsCod=%ld"
			     " AND crs_usr.CrsCod=%ld AND crs_usr.Role=%u"
			     " AND msg_snt.UsrCod=crs_usr.UsrCod",
			     CrsCod,CrsCod,(unsigned) Rol_TCH);
  }

/*****************************************************************************/
/************** Get the number of unique messages sent by a user *************/
/*****************************************************************************/

unsigned long Msg_GetNumMsgsSentByUsr (long UsrCod)
  {
   /***** Get the number of unique messages sent by any teacher from this course *****/
   return DB_QueryCOUNT ("can not get the number of messages sent by a user",
			 "SELECT"
			 " (SELECT COUNT(*) FROM msg_snt"
			 " WHERE UsrCod=%ld)"
			 " +"
			 " (SELECT COUNT(*) FROM msg_snt_deleted"
			 " WHERE UsrCod=%ld)",
			 UsrCod,
			 UsrCod);
  }

/*****************************************************************************/
/******** Get the number of unique messages sent from this location **********/
/******** (all the platform, current degree or current course)      **********/
/*****************************************************************************/

unsigned Msg_GetNumMsgsSent (Hie_Level_t Scope,Msg_Status_t MsgStatus)
  {
   const char *Table = "msg_snt";
   unsigned NumMsgs = 0;	// Initialized to avoid warning

   /***** Get the number of messages sent from this location
          (all the platform, current degree or current course) from database *****/
   switch (MsgStatus)
     {
      case Msg_STATUS_ALL:
      case Msg_STATUS_NOTIFIED:
         Table = "msg_snt";
         break;
      case Msg_STATUS_DELETED:
         Table = "msg_snt_deleted";
         break;
     }
   switch (Scope)
     {
      case Hie_SYS:
	 NumMsgs = (unsigned) DB_GetNumRowsTable (Table);
         break;
      case Hie_CTY:
         NumMsgs =
         (unsigned) DB_QueryCOUNT ("can not get number of sent messages",
				   "SELECT COUNT(*)"
				   " FROM institutions,centres,degrees,courses,%s"
				   " WHERE institutions.CtyCod=%ld"
				   " AND institutions.InsCod=centres.InsCod"
				   " AND centres.CtrCod=degrees.CtrCod"
				   " AND degrees.DegCod=courses.DegCod"
				   " AND courses.CrsCod=%s.CrsCod",
				   Table,
				   Gbl.Hierarchy.Cty.CtyCod,
				   Table);
         break;
      case Hie_INS:
         NumMsgs =
         (unsigned) DB_QueryCOUNT ("can not get number of sent messages",
				   "SELECT COUNT(*)"
				   " FROM centres,degrees,courses,%s"
				   " WHERE centres.InsCod=%ld"
				   " AND centres.CtrCod=degrees.CtrCod"
				   " AND degrees.DegCod=courses.DegCod"
				   " AND courses.CrsCod=%s.CrsCod",
				   Table,
				   Gbl.Hierarchy.Ins.InsCod,
				   Table);
         break;
      case Hie_CTR:
         NumMsgs =
         (unsigned) DB_QueryCOUNT ("can not get number of sent messages",
				   "SELECT COUNT(*)"
				   " FROM degrees,courses,%s"
				   " WHERE degrees.CtrCod=%ld"
				   " AND degrees.DegCod=courses.DegCod"
				   " AND courses.CrsCod=%s.CrsCod",
				   Table,
				   Gbl.Hierarchy.Ctr.CtrCod,
				   Table);
         break;
      case Hie_DEG:
         NumMsgs =
         (unsigned) DB_QueryCOUNT ("can not get number of sent messages",
				   "SELECT COUNT(*)"
				   " FROM courses,%s"
				   " WHERE courses.DegCod=%ld"
				   " AND courses.CrsCod=%s.CrsCod",
				   Table,
				   Gbl.Hierarchy.Deg.DegCod,
				   Table);
         break;
      case Hie_CRS:
         NumMsgs =
         (unsigned) DB_QueryCOUNT ("can not get number of sent messages",
				   "SELECT COUNT(*)"
				   " FROM %s"
				   " WHERE CrsCod=%ld",
				   Table,
				   Gbl.Hierarchy.Crs.CrsCod);
         break;
      default:
	 Lay_WrongScopeExit ();
	 break;
     }

   return NumMsgs;
  }

/*****************************************************************************/
/****** Get the number of unique messages received from this location ********/
/****** (all the platform, current degree or current course)          ********/
/*****************************************************************************/

unsigned Msg_GetNumMsgsReceived (Hie_Level_t Scope,Msg_Status_t MsgStatus)
  {
   char *Table;
   unsigned NumMsgs = 0;	// Initialized to avoid warning

   /***** Get the number of unique messages sent from this location
          (all the platform, current degree or current course) from database *****/
   switch (MsgStatus)
     {
      case Msg_STATUS_ALL:
      case Msg_STATUS_DELETED:
         Table = (MsgStatus == Msg_STATUS_ALL) ? "msg_rcv" :
                                                 "msg_rcv_deleted";
         switch (Scope)
           {
            case Hie_SYS:
               NumMsgs = (unsigned) DB_GetNumRowsTable (Table);
               break;
            case Hie_CTY:
               NumMsgs =
               (unsigned) DB_QueryCOUNT ("can not get number"
        				 " of received messages",
					 "SELECT COUNT(*)"
					 " FROM institutions,centres,degrees,courses,%s,msg_snt"
					 " WHERE institutions.CtyCod=%ld"
					 " AND institutions.InsCod=centres.InsCod"
					 " AND centres.CtrCod=degrees.CtrCod"
					 " AND degrees.DegCod=courses.DegCod"
					 " AND courses.CrsCod=msg_snt.CrsCod"
					 " AND msg_snt.MsgCod=%s.MsgCod",
					 Table,
					 Gbl.Hierarchy.Cty.CtyCod,
					 Table);
               break;
            case Hie_INS:
               NumMsgs =
               (unsigned) DB_QueryCOUNT ("can not get number"
        				 " of received messages",
					 "SELECT COUNT(*)"
					 " FROM centres,degrees,courses,%s,msg_snt"
					 " WHERE centres.InsCod=%ld"
					 " AND centres.CtrCod=degrees.CtrCod"
					 " AND degrees.DegCod=courses.DegCod"
					 " AND courses.CrsCod=msg_snt.CrsCod"
					 " AND msg_snt.MsgCod=%s.MsgCod",
					 Table,
					 Gbl.Hierarchy.Ins.InsCod,
					 Table);
               break;
            case Hie_CTR:
               NumMsgs =
               (unsigned) DB_QueryCOUNT ("can not get number"
        				 " of received messages",
					 "SELECT COUNT(*)"
					 " FROM degrees,courses,%s,msg_snt"
					 " WHERE degrees.CtrCod=%ld"
					 " AND degrees.DegCod=courses.DegCod"
					 " AND courses.CrsCod=msg_snt.CrsCod"
					 " AND msg_snt.MsgCod=%s.MsgCod",
					 Table,
					 Gbl.Hierarchy.Ctr.CtrCod,
					 Table);
               break;
            case Hie_DEG:
               NumMsgs =
               (unsigned) DB_QueryCOUNT ("can not get number"
        				 " of received messages",
					 "SELECT COUNT(*)"
					 " FROM courses,%s,msg_snt"
					 " WHERE courses.DegCod=%ld"
					 " AND courses.CrsCod=msg_snt.CrsCod"
					 " AND msg_snt.MsgCod=%s.MsgCod",
					 Table,
					 Gbl.Hierarchy.Deg.DegCod,
					 Table);
               break;
            case Hie_CRS:
               NumMsgs =
               (unsigned) DB_QueryCOUNT ("can not get number"
        				 " of received messages",
					 "SELECT COUNT(*)"
					 " FROM msg_snt,%s"
					 " WHERE msg_snt.CrsCod=%ld"
					 " AND msg_snt.MsgCod=%s.MsgCod",
					 Table,
					 Gbl.Hierarchy.Crs.CrsCod,
					 Table);
               break;
	    default:
	       Lay_WrongScopeExit ();
	       break;
           }
         break;
      case Msg_STATUS_NOTIFIED:
         switch (Scope)
           {
            case Hie_SYS:
               NumMsgs =
               (unsigned) DB_QueryCOUNT ("can not get number"
        				 " of received messages",
					 "SELECT "
					 "(SELECT COUNT(*)"
					 " FROM msg_rcv"
					 " WHERE Notified='Y')"
					 " + "
					 "(SELECT COUNT(*)"
					 " FROM msg_rcv_deleted"
					 " WHERE Notified='Y')");
               break;
            case Hie_CTY:
               NumMsgs =
               (unsigned) DB_QueryCOUNT ("can not get number"
        				 " of received messages",
					 "SELECT "
					 "(SELECT COUNT(*)"
					 " FROM institutions,centres,degrees,courses,msg_snt,msg_rcv"
					 " WHERE institutions.CtyCod=%ld"
					 " AND institutions.InsCod=centres.InsCod"
					 " AND centres.CtrCod=degrees.CtrCod"
					 " AND degrees.DegCod=courses.DegCod"
					 " AND courses.CrsCod=msg_snt.CrsCod"
					 " AND msg_snt.MsgCod=msg_rcv.MsgCod"
					 " AND msg_rcv.Notified='Y')"
					 " + "
					 "(SELECT COUNT(*)"
					 " FROM institutions,centres,degrees,courses,msg_snt,msg_rcv_deleted"
					 " WHERE institutions.CtyCod=%ld"
					 " AND institutions.InsCod=centres.InsCod"
					 " AND centres.CtrCod=degrees.CtrCod"
					 " AND degrees.DegCod=courses.DegCod"
					 " AND courses.CrsCod=msg_snt.CrsCod"
					 " AND msg_snt.MsgCod=msg_rcv_deleted.MsgCod"
					 " AND msg_rcv_deleted.Notified='Y')",
					 Gbl.Hierarchy.Cty.CtyCod,
					 Gbl.Hierarchy.Cty.CtyCod);
               break;
            case Hie_INS:
               NumMsgs =
               (unsigned) DB_QueryCOUNT ("can not get number"
        				 " of received messages",
					 "SELECT "
					 "(SELECT COUNT(*)"
					 " FROM centres,degrees,courses,msg_snt,msg_rcv"
					 " WHERE centres.InsCod=%ld"
					 " AND centres.CtrCod=degrees.CtrCod"
					 " AND degrees.DegCod=courses.DegCod"
					 " AND courses.CrsCod=msg_snt.CrsCod"
					 " AND msg_snt.MsgCod=msg_rcv.MsgCod"
					 " AND msg_rcv.Notified='Y')"
					 " + "
					 "(SELECT COUNT(*)"
					 " FROM centres,degrees,courses,msg_snt,msg_rcv_deleted"
					 " WHERE centres.InsCod=%ld"
					 " AND centres.CtrCod=degrees.CtrCod"
					 " AND degrees.DegCod=courses.DegCod"
					 " AND courses.CrsCod=msg_snt.CrsCod"
					 " AND msg_snt.MsgCod=msg_rcv_deleted.MsgCod"
					 " AND msg_rcv_deleted.Notified='Y')",
					 Gbl.Hierarchy.Ins.InsCod,
					 Gbl.Hierarchy.Ins.InsCod);
               break;
            case Hie_CTR:
               NumMsgs =
               (unsigned) DB_QueryCOUNT ("can not get number"
        				 " of received messages",
					 "SELECT "
					 "(SELECT COUNT(*)"
					 " FROM degrees,courses,msg_snt,msg_rcv"
					 " WHERE degrees.CtrCod=%ld"
					 " AND degrees.DegCod=courses.DegCod"
					 " AND courses.CrsCod=msg_snt.CrsCod"
					 " AND msg_snt.MsgCod=msg_rcv.MsgCod"
					 " AND msg_rcv.Notified='Y')"
					 " + "
					 "(SELECT COUNT(*)"
					 " FROM degrees,courses,msg_snt,msg_rcv_deleted"
					 " WHERE degrees.CtrCod=%ld"
					 " AND degrees.DegCod=courses.DegCod"
					 " AND courses.CrsCod=msg_snt.CrsCod"
					 " AND msg_snt.MsgCod=msg_rcv_deleted.MsgCod"
					 " AND msg_rcv_deleted.Notified='Y')",
					 Gbl.Hierarchy.Ctr.CtrCod,
					 Gbl.Hierarchy.Ctr.CtrCod);
               break;
            case Hie_DEG:
               NumMsgs =
               (unsigned) DB_QueryCOUNT ("can not get number"
        				 " of received messages",
					 "SELECT "
					 "(SELECT COUNT(*)"
					 " FROM courses,msg_snt,msg_rcv"
					 " WHERE courses.DegCod=%ld"
					 " AND courses.CrsCod=msg_snt.CrsCod"
					 " AND msg_snt.MsgCod=msg_rcv.MsgCod"
					 " AND msg_rcv.Notified='Y')"
					 " + "
					 "(SELECT COUNT(*)"
					 " FROM courses,msg_snt,msg_rcv_deleted"
					 " WHERE courses.DegCod=%ld"
					 " AND courses.CrsCod=msg_snt.CrsCod"
					 " AND msg_snt.MsgCod=msg_rcv_deleted.MsgCod"
					 " AND msg_rcv_deleted.Notified='Y')",
					 Gbl.Hierarchy.Deg.DegCod,
					 Gbl.Hierarchy.Deg.DegCod);
               break;
            case Hie_CRS:
               NumMsgs =
               (unsigned) DB_QueryCOUNT ("can not get number"
        				 " of received messages",
					 "SELECT "
					 "(SELECT COUNT(*)"
					 " FROM msg_snt,msg_rcv"
					 " WHERE msg_snt.CrsCod=%ld"
					 " AND msg_snt.MsgCod=msg_rcv.MsgCod"
					 " AND msg_rcv.Notified='Y')"
					 " + "
					 "(SELECT COUNT(*)"
					 " FROM msg_snt,msg_rcv_deleted"
					 " WHERE msg_snt.CrsCod=%ld"
					 " AND msg_snt.MsgCod=msg_rcv_deleted.MsgCod"
					 " AND msg_rcv_deleted.Notified='Y')",
					 Gbl.Hierarchy.Crs.CrsCod,
					 Gbl.Hierarchy.Crs.CrsCod);
               break;
	    default:
	       Lay_WrongScopeExit ();
	       break;
           }
         break;
     }

   return NumMsgs;
  }

/*****************************************************************************/
/********* Write number of messages and number of unread messages ************/
/*****************************************************************************/
// Fill Gbl.Title

static char *Msg_WriteNumMsgs (unsigned NumUnreadMsgs)
  {
   extern const char *Txt_message_received;
   extern const char *Txt_message_sent;
   extern const char *Txt_messages_received;
   extern const char *Txt_messages_sent;
   extern const char *Txt_unread_MESSAGE;
   extern const char *Txt_unread_MESSAGES;

   switch (Gbl.Msg.TypeOfMessages)
     {
      case Msg_MESSAGES_RECEIVED:
	 if (Gbl.Msg.NumMsgs == 1)
	   {
	    if (NumUnreadMsgs)
	       snprintf (Gbl.Title,sizeof (Gbl.Title),
		         "1 %s, 1 %s",
			 Txt_message_received,Txt_unread_MESSAGE);
	    else
	       snprintf (Gbl.Title,sizeof (Gbl.Title),
		         "1 %s",
			 Txt_message_received);
	   }
	 else
	   {
	    if (NumUnreadMsgs == 0)
	       snprintf (Gbl.Title,sizeof (Gbl.Title),
		         "%u %s",
			 Gbl.Msg.NumMsgs,Txt_messages_received);
	    else if (NumUnreadMsgs == 1)
	       snprintf (Gbl.Title,sizeof (Gbl.Title),
		         "%u %s, 1 %s",
			 Gbl.Msg.NumMsgs,Txt_messages_received,
			 Txt_unread_MESSAGE);
	    else
	       snprintf (Gbl.Title,sizeof (Gbl.Title),
		         "%u %s, %u %s",
			 Gbl.Msg.NumMsgs,Txt_messages_received,
			 NumUnreadMsgs,Txt_unread_MESSAGES);
	   }
	 break;
      case Msg_MESSAGES_SENT:
	 if (Gbl.Msg.NumMsgs == 1)
	    snprintf (Gbl.Title,sizeof (Gbl.Title),
		      "1 %s",
		      Txt_message_sent);
	 else
	    snprintf (Gbl.Title,sizeof (Gbl.Title),
		      "%u %s",
		      Gbl.Msg.NumMsgs,Txt_messages_sent);
	 break;
     }

   return Gbl.Title;
  }

/*****************************************************************************/
/***************** Put contextual icons in list of messages ******************/
/*****************************************************************************/

static void Msg_PutIconsListMsgs (void)
  {
   static const Act_Action_t ActionReqDelAllMsg[Msg_NUM_TYPES_OF_MSGS] =
     {
      ActReqDelAllRcvMsg,
      ActReqDelAllSntMsg,
     };

   /***** Put icon to remove messages *****/
   Ico_PutContextualIconToRemove (ActionReqDelAllMsg[Gbl.Msg.TypeOfMessages],
                                  Msg_PutHiddenParamsMsgsFilters);

   /***** Put icon to show a figure *****/
   Gbl.Figures.FigureType = Fig_MESSAGES;
   Fig_PutIconToShowFigure ();
  }

/*****************************************************************************/
/******* Put hidden parameters to expand, contract or delete a message *******/
/*****************************************************************************/

static void Msg_PutHiddenParamsOneMsg (void)
  {
   Pag_PutHiddenParamPagNum (Msg_WhatPaginate[Gbl.Msg.TypeOfMessages],
			     Gbl.Msg.CurrentPage);
   Msg_PutHiddenParamMsgCod (Gbl.Msg.MsgCod);
   Msg_PutHiddenParamsMsgsFilters ();
  }

/*****************************************************************************/
/****************** Put hidden parameters with filters ***********************/
/*****************************************************************************/

void Msg_PutHiddenParamsMsgsFilters (void)
  {
   if (Gbl.Msg.FilterCrsCod >= 0)
      Par_PutHiddenParamLong (NULL,"FilterCrsCod",Gbl.Msg.FilterCrsCod);
   if (Gbl.Msg.FilterFromTo[0])
      Par_PutHiddenParamString (NULL,"FilterFromTo",Gbl.Msg.FilterFromTo);
   if (Gbl.Msg.FilterContent[0])
      Par_PutHiddenParamString (NULL,"FilterContent",Gbl.Msg.FilterContent);
   if (Gbl.Msg.ShowOnlyUnreadMsgs)
      Par_PutHiddenParamChar ("OnlyUnreadMsgs",'Y');
  }

/*****************************************************************************/
/********************* Get dictinct courses in my messages *******************/
/*****************************************************************************/

void Msg_GetDistinctCoursesInMyMessages (void)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRow;
   unsigned long NumRows = 0;	// Initialized to avoid warning
   struct Course Crs;

   /***** Get distinct courses in my messages from database *****/
   switch (Gbl.Msg.TypeOfMessages)
     {
      case Msg_MESSAGES_RECEIVED:
         NumRows = DB_QuerySELECT (&mysql_res,"can not get distinct courses"
					      " in your messages",""
				   "SELECT DISTINCT courses.CrsCod,courses.ShortName"
				   " FROM msg_rcv,msg_snt,courses"
				   " WHERE msg_rcv.UsrCod=%ld"
				   " AND msg_rcv.MsgCod=msg_snt.MsgCod"
				   " AND msg_snt.CrsCod=courses.CrsCod"
				   " ORDER BY courses.ShortName",
				   Gbl.Usrs.Me.UsrDat.UsrCod);
         break;
      case Msg_MESSAGES_SENT:
         NumRows = DB_QuerySELECT (&mysql_res,"can not get distinct courses"
					      " in your messages",
				   "SELECT DISTINCT courses.CrsCod,courses.ShortName"
				   " FROM msg_snt,courses"
				   " WHERE msg_snt.UsrCod=%ld"
				   " AND msg_snt.CrsCod=courses.CrsCod"
				   " ORDER BY courses.ShortName",
				   Gbl.Usrs.Me.UsrDat.UsrCod);
         break;
      default: // Not aplicable here
         break;
     }

   /***** Get distinct courses in messages from database *****/
   Gbl.Msg.NumCourses = 0;
   for (NumRow = 0;
	NumRow < NumRows;
	NumRow++)
     {
      /* Get next course */
      row = mysql_fetch_row (mysql_res);
      Crs.CrsCod = Str_ConvertStrCodToLongCod (row[0]);
      if (Crs.CrsCod >= 0 && Gbl.Msg.NumCourses < Crs_MAX_COURSES_PER_USR)
         if (Crs_GetDataOfCourseByCod (&Crs))
           {
            Gbl.Msg.Courses[Gbl.Msg.NumCourses].CrsCod = Crs.CrsCod;
            Str_Copy (Gbl.Msg.Courses[Gbl.Msg.NumCourses].ShrtName,Crs.ShrtName,
                      Hie_MAX_BYTES_SHRT_NAME);
            Gbl.Msg.NumCourses++;
           }
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/********* Show form to select course for sent or received messages **********/
/*****************************************************************************/

void Msg_ShowFormSelectCourseSentOrRecMsgs (void)
  {
   extern const char *The_ClassFormInBox[The_NUM_THEMES];
   extern const char *Txt_Messages_received_from_A_COURSE;
   extern const char *Txt_Messages_sent_from_A_COURSE;
   extern const char *Txt_any_course;
   unsigned NumOriginCrs;
   const char *TxtSelector[Msg_NUM_TYPES_OF_MSGS] =
     {
      Txt_Messages_received_from_A_COURSE,
      Txt_Messages_sent_from_A_COURSE
     };

   /***** Course selection *****/
   HTM_LABEL_Begin ("class=\"%s\"",The_ClassFormInBox[Gbl.Prefs.Theme]);
   HTM_TxtF ("%s&nbsp;",TxtSelector[Gbl.Msg.TypeOfMessages]);
   HTM_SELECT_Begin (false,
		     "name=\"FilterCrsCod\"");
   HTM_OPTION (HTM_Type_STRING,"",
	       Gbl.Msg.FilterCrsCod < 0,false,
	       "%s",Txt_any_course);

   /***** Write an option for each origin course *****/
   for (NumOriginCrs = 0;
	NumOriginCrs < Gbl.Msg.NumCourses;
	NumOriginCrs++)
      HTM_OPTION (HTM_Type_LONG,&Gbl.Msg.Courses[NumOriginCrs].CrsCod,
		  Gbl.Msg.Courses[NumOriginCrs].CrsCod == Gbl.Msg.FilterCrsCod,false,
		  "%s",Gbl.Msg.Courses[NumOriginCrs].ShrtName);
   HTM_SELECT_End ();
   HTM_LABEL_End ();
  }

/*****************************************************************************/
/***** Show form to filter "from" and "to" for received or sent messages *****/
/*****************************************************************************/

void Msg_ShowFormToFilterMsgs (void)
  {
   extern const char *The_ClassFormInBox[The_NUM_THEMES];
   extern const char *Txt_MSG_From;
   extern const char *Txt_MSG_To;
   extern const char *Txt_MSG_Content;
   const char *TxtFromTo[Msg_NUM_TYPES_OF_MSGS] =
     {
      Txt_MSG_From,
      Txt_MSG_To
     };

   /***** Begin table *****/
   HTM_TABLE_BeginCenterPadding (2);

   HTM_TR_Begin (NULL);

   /***** Filter authors/recipients *****/
   HTM_TD_Begin ("class=\"LM\"");
   HTM_LABEL_Begin ("class=\"%s\"",The_ClassFormInBox[Gbl.Prefs.Theme]);
   HTM_TxtF ("%s:&nbsp;",TxtFromTo[Gbl.Msg.TypeOfMessages]);
   HTM_INPUT_SEARCH ("FilterFromTo",Usr_MAX_CHARS_FIRSTNAME_OR_SURNAME * 3,
		     Gbl.Msg.FilterFromTo,
	             "size=\"20\"");
   HTM_LABEL_End ();
   HTM_TD_End ();

   /***** Filter message content *****/
   HTM_TD_Begin ("class=\"LM\"");
   HTM_LABEL_Begin ("class=\"%s\"",The_ClassFormInBox[Gbl.Prefs.Theme]);
   HTM_TxtF ("%s:&nbsp;",Txt_MSG_Content);
   HTM_INPUT_SEARCH ("FilterContent",Msg_MAX_CHARS_FILTER_CONTENT,
		     Gbl.Msg.FilterContent,
	             "size=\"20\"");
   HTM_LABEL_End ();
   HTM_TD_End ();

   HTM_TR_End ();

   /***** End table *****/
   HTM_TABLE_End ();
  }

/*****************************************************************************/
/**** Show form to select whether to show only unread (received) messages ****/
/*****************************************************************************/

static void Msg_ShowFormToShowOnlyUnreadMessages (void)
  {
   extern const char *The_ClassFormInBox[The_NUM_THEMES];
   extern const char *Txt_only_unread_messages;

   /***** Put checkbox to select whether to show only unread (received) messages *****/
   HTM_LABEL_Begin ("class=\"%s\"",The_ClassFormInBox[Gbl.Prefs.Theme]);
   HTM_INPUT_CHECKBOX ("OnlyUnreadMsgs",false,
		       "value=\"Y\"%s",
		       Gbl.Msg.ShowOnlyUnreadMsgs ? " checked=\"checked\"" : "");
   HTM_Txt (Txt_only_unread_messages);
   HTM_LABEL_End ();
  }

/*****************************************************************************/
/*********** Get parameter to show only unread (received) messages ***********/
/*****************************************************************************/

static void Msg_GetParamOnlyUnreadMsgs (void)
  {
   /***** Get parameter to show only unread (received) messages *****/
   Gbl.Msg.ShowOnlyUnreadMsgs = Par_GetParToBool ("OnlyUnreadMsgs");
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
   unsigned long NumRows;

   /***** Get data of message from table msg_snt *****/
   *Deleted = false;
   NumRows = DB_QuerySELECT (&mysql_res,"can not get data of a message",
			     "SELECT CrsCod,UsrCod,UNIX_TIMESTAMP(CreatTime)"
			     " FROM msg_snt WHERE MsgCod=%ld",
			     MsgCod);

   if (NumRows == 0)   // If not result ==> sent message is deleted
     {
      /***** Get data of message from table msg_snt_deleted *****/
      NumRows = DB_QuerySELECT (&mysql_res,"can not get data of a message",
				"SELECT CrsCod,UsrCod,UNIX_TIMESTAMP(CreatTime)"
				" FROM msg_snt_deleted WHERE MsgCod=%ld",
				MsgCod);

      *Deleted = true;
     }

   /* Result should have a unique row */
   if (NumRows != 1)
      Lay_ShowErrorAndExit ("Error when getting data of a message.");

   /* Get number of rows */
   row = mysql_fetch_row (mysql_res);

   /* Get location (row[0]) */
   *CrsCod = Str_ConvertStrCodToLongCod (row[0]);

   /* Get author code (row[1]) */
   *UsrCod = Str_ConvertStrCodToLongCod (row[1]);

   /* Get creation time (row[2]) */
   *CreatTimeUTC = Dat_GetUNIXTimeFromStr (row[2]);

   /* Free structure that stores the query result */
   DB_FreeMySQLResult (&mysql_res);

   /***** Get subject of message from database *****/
   Msg_GetMsgSubject (MsgCod,Subject);
  }

/*****************************************************************************/
/************************ Get the subject of a message ***********************/
/*****************************************************************************/

void Msg_GetMsgSubject (long MsgCod,char Subject[Cns_MAX_BYTES_SUBJECT + 1])
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;

   /***** Get subject of message from database *****/
   if (DB_QuerySELECT (&mysql_res,"can not get the subject of a message",
	               "SELECT Subject FROM msg_content"
	               " WHERE MsgCod=%ld",
		       MsgCod) == 1)	// Result should have a unique row
     {
      /***** Get subject *****/
      row = mysql_fetch_row (mysql_res);
      Str_Copy (Subject,row[0],
                Cns_MAX_BYTES_SUBJECT);
     }
   else
      Subject[0] = '\0';

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/*************** Get content and optional image of a message *****************/
/*****************************************************************************/

static void Msg_GetMsgContent (long MsgCod,char Content[Cns_MAX_BYTES_LONG_TEXT + 1],
                               struct Media *Media)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;

   /***** Get content of message from database *****/
   NumRows = DB_QuerySELECT (&mysql_res,"can not get the content of a message",
			     "SELECT Content,"		// row[0]
				    "MedCod"		// row[1]
			     " FROM msg_content WHERE MsgCod=%ld",
			     MsgCod);

   /***** Result should have a unique row *****/
   if (NumRows != 1)
      Lay_ShowErrorAndExit ("Error when getting content of a message.");

   /***** Get number of rows *****/
   row = mysql_fetch_row (mysql_res);

   /****** Get content (row[0]) *****/
   Str_Copy (Content,row[0],
             Cns_MAX_BYTES_LONG_TEXT);

   /***** Get media (row[1]) *****/
   Media->MedCod = Str_ConvertStrCodToLongCod (row[1]);
   Med_GetMediaDataByCod (Media);

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/********************** Get if a sent message is expanded ********************/
/*****************************************************************************/

static void Msg_GetStatusOfSentMsg (long MsgCod,bool *Expanded)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;

   /***** Get if sent message has been replied/expanded from database *****/
   NumRows = DB_QuerySELECT (&mysql_res,"can not get if a sent message"
				        " has been replied/expanded",
			     "SELECT Expanded FROM msg_snt"
			     " WHERE MsgCod=%ld AND UsrCod=%ld",
			     MsgCod,Gbl.Usrs.Me.UsrDat.UsrCod);

   /***** Result should have a unique row *****/
   if (NumRows != 1)
      Lay_ShowErrorAndExit ("Error when getting if a sent message has been replied/expanded.");

   /***** Get number of rows *****/
   row = mysql_fetch_row (mysql_res);

   /***** Get if message is expanded *****/
   *Expanded = (row[0][0] == 'Y');

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/***************** Get if a received message has been replied ****************/
/*****************************************************************************/

static void Msg_GetStatusOfReceivedMsg (long MsgCod,bool *Open,bool *Replied,bool *Expanded)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;

   /***** Get if received message has been replied/expanded from database *****/
   NumRows = DB_QuerySELECT (&mysql_res,"can not get if a received message"
				        " has been replied/expanded",
			     "SELECT Open,Replied,Expanded FROM msg_rcv"
			     " WHERE MsgCod=%ld AND UsrCod=%ld",
			     MsgCod,Gbl.Usrs.Me.UsrDat.UsrCod);

   /***** Result should have a unique row *****/
   if (NumRows != 1)
      Lay_ShowErrorAndExit ("Error when getting if a received message has been replied/expanded.");

   /***** Get number of rows *****/
   row = mysql_fetch_row (mysql_res);

   /***** Get if message has been read by me *****/
   *Open     = (row[0][0] == 'Y');

   /***** Get if message has been replied *****/
   *Replied  = (row[1][0] == 'Y');

   /***** Get if message is expanded *****/
   *Expanded = (row[2][0] == 'Y');

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/******** Show a sent or a received message (from a user to another) *********/
/*****************************************************************************/

static void Msg_ShowASentOrReceivedMessage (long MsgNum,long MsgCod)
  {
   extern const char *Txt_MSG_Replied;
   extern const char *Txt_MSG_Not_replied;
   extern const char *Txt_MSG_Unopened;
   extern const char *Txt_MSG_Sent;
   extern const char *Txt_MSG_From;
   extern const char *Txt_MSG_To;
   extern const char *Txt_MSG_Content;
   static const Act_Action_t ActionDelMsg[Msg_NUM_TYPES_OF_MSGS] =
     {
      ActDelRcvMsg,
      ActDelSntMsg,
     };
   struct UsrData UsrDat;
   const char *Title = NULL;	// Initialized to avoid warning
   bool FromThisCrs = false;	// Initialized to avoid warning
   time_t CreatTimeUTC;		// Creation time of a message
   long CrsCod;
   char Subject[Cns_MAX_BYTES_SUBJECT + 1];
   char Content[Cns_MAX_BYTES_LONG_TEXT + 1];
   struct Media Media;
   bool Deleted;
   bool Open = true;
   bool Replied = false;	// Initialized to avoid warning
   bool Expanded = false;

   /***** Initialize structure with user's data *****/
   Usr_UsrDataConstructor (&UsrDat);

   /***** Get data of message *****/
   Msg_GetMsgSntData (MsgCod,&CrsCod,&UsrDat.UsrCod,&CreatTimeUTC,Subject,&Deleted);
   switch (Gbl.Msg.TypeOfMessages)
     {
      case Msg_MESSAGES_RECEIVED:
         Msg_GetStatusOfReceivedMsg (MsgCod,&Open,&Replied,&Expanded);
         break;
      case Msg_MESSAGES_SENT:
         Msg_GetStatusOfSentMsg (MsgCod,&Expanded);
         break;
     }

   /***** Put an icon with message status *****/
   switch (Gbl.Msg.TypeOfMessages)
     {
      case Msg_MESSAGES_RECEIVED:
         Title = (Open ? (Replied ? Txt_MSG_Replied :
                                    Txt_MSG_Not_replied) :
                         Txt_MSG_Unopened);
	 break;
      case Msg_MESSAGES_SENT:
	 Title = Txt_MSG_Sent;
	 break;
     }

   HTM_TR_Begin (NULL);

   HTM_TD_Begin ("class=\"CONTEXT_COL %s\"",
		 Gbl.Msg.TypeOfMessages == Msg_MESSAGES_RECEIVED ? (Open ? "BG_MSG_BLUE" :
									   "BG_MSG_GREEN") :
								   "BG_MSG_BLUE");
   Ico_PutIcon (Gbl.Msg.TypeOfMessages == Msg_MESSAGES_RECEIVED ? (Open ? (Replied ? "reply.svg" :
        	                                                                     "envelope-open-text.svg") :
                                                                           "envelope.svg") :
                                                                   "share.svg",
		Title,"ICO16x16");

   /***** Form to delete message *****/
   HTM_BR ();
   Gbl.Msg.MsgCod = MsgCod;	// Message to be deleted
   Ico_PutContextualIconToRemove (ActionDelMsg[Gbl.Msg.TypeOfMessages],
                                  Msg_PutHiddenParamsOneMsg);
   HTM_TD_End ();

   /***** Write message number *****/
   Msg_WriteMsgNumber (MsgNum,!Open);

   /***** Write message author *****/
   Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat,Usr_DONT_GET_PREFS);

   HTM_TD_Begin ("class=\"%s LT\"",Open ? "MSG_AUT_BG" :
			                        "MSG_AUT_BG_NEW");
   Msg_WriteMsgAuthor (&UsrDat,true,NULL);
   HTM_TD_End ();

   /***** Write subject *****/
   Msg_WriteSentOrReceivedMsgSubject (MsgCod,Subject,Open,Expanded);

   /***** Write date-time *****/
   Msg_WriteMsgDate (CreatTimeUTC,Open ? "MSG_TIT_BG" :
	                                 "MSG_TIT_BG_NEW");

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
      if (Gbl.Msg.TypeOfMessages == Msg_MESSAGES_RECEIVED &&
	  Gbl.Usrs.Me.Role.Logged >= Rol_USR)
	 // Guests (users without courses) can read messages but not reply them
         Msg_WriteFormToReply (MsgCod,CrsCod,FromThisCrs,Replied,&UsrDat);
      HTM_TD_End ();
      HTM_TR_End ();

      HTM_TABLE_End ();
      HTM_TD_End ();

      /***** Write "From:" *****/
      HTM_TD_Begin ("class=\"RT MSG_TIT\"");
      HTM_TxtF ("%s:&nbsp;",Txt_MSG_From);
      HTM_TD_End ();

      HTM_TD_Begin ("colspan=\"2\" class=\"LT\"");
      Msg_WriteMsgFrom (&UsrDat,Deleted);
      HTM_TD_End ();

      HTM_TR_End ();

      /***** Write "To:" *****/
      HTM_TR_Begin (NULL);

      HTM_TD_Begin ("class=\"RT MSG_TIT\"");
      HTM_TxtF ("%s:&nbsp;",Txt_MSG_To);
      HTM_TD_End ();

      HTM_TD_Begin ("colspan=\"2\" class=\"LT\"");
      Msg_WriteMsgTo (MsgCod);
      HTM_TD_End ();

      HTM_TR_End ();

      HTM_TR_Begin (NULL);

      /***** Write "Content:" *****/
      HTM_TD_Begin ("class=\"RT MSG_TIT\"");
      HTM_TxtF ("%s:&nbsp;",Txt_MSG_Content);
      HTM_TD_End ();

      /***** Initialize image *****/
      Med_MediaConstructor (&Media);

      /***** Get message content and optional image *****/
      Msg_GetMsgContent (MsgCod,Content,&Media);

      /***** Show content and image *****/
      HTM_TD_Begin ("colspan=\"2\" class=\"MSG_TXT LT\"");
      if (Content[0])
         Msg_WriteMsgContent (Content,Cns_MAX_BYTES_LONG_TEXT,true,false);
      Med_ShowMedia (&Media,"MSG_IMG_CONTAINER","MSG_IMG");
      HTM_TD_End ();

      HTM_TR_End ();

      /***** Free image *****/
      Med_MediaDestructor (&Media);
     }

   /***** Free memory used for user's data *****/
   Usr_UsrDataDestructor (&UsrDat);
  }

/*****************************************************************************/
/******************** Get subject and content of a message *******************/
/*****************************************************************************/

void Msg_GetNotifMessage (char SummaryStr[Ntf_MAX_BYTES_SUMMARY + 1],
                          char **ContentStr,long MsgCod,bool GetContent)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   size_t Length;

   SummaryStr[0] = '\0';	// Return nothing on error
   // This function may be called inside a web service, so don't report error

   /***** Get subject of message from database *****/
   if (DB_QuerySELECT (&mysql_res,"can not get subject and content"
				  " of a message",
		       "SELECT Subject,Content FROM msg_content"
		       " WHERE MsgCod=%ld",
		       MsgCod) == 1)	// Result should have a unique row
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
	 Str_Copy (SummaryStr,row[0],
		   Ntf_MAX_BYTES_SUMMARY);

      /***** Copy subject *****/
      if (GetContent)
	{
	 Length = strlen (row[1]);
	 if ((*ContentStr = (char *) malloc (Length + 1)) == NULL)
	    Lay_ShowErrorAndExit ("Error allocating memory for notification content.");
	 Str_Copy (*ContentStr,row[1],
		   Length);
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
   HTM_TD_Begin ("class=\"%s CT\" style=\"width:45px;\"",
		 NewMsg ? "MSG_TIT_BG_NEW" :
			  "MSG_TIT_BG");
   HTM_TxtF ("%lu:",MsgNum);
   HTM_TD_End ();
  }

/*****************************************************************************/
/******************** Write subject of a received message ********************/
/*****************************************************************************/

static void Msg_WriteSentOrReceivedMsgSubject (long MsgCod,const char *Subject,bool Open,bool Expanded)
  {
   extern const char *Txt_Hide_message;
   extern const char *Txt_See_message;
   extern const char *Txt_no_subject;

   /***** Start cell *****/
   HTM_TD_Begin ("class=\"%s LT\"",Open ? "MSG_TIT_BG" :
        	                                "MSG_TIT_BG_NEW");

   /***** Begin form to expand/contract the message *****/
   Frm_StartForm (Gbl.Msg.TypeOfMessages == Msg_MESSAGES_RECEIVED ? (Expanded ? ActConRcvMsg :
	                                                                        ActExpRcvMsg) :
                                                                    (Expanded ? ActConSntMsg :
                                                        	                ActExpSntMsg));
   Gbl.Msg.MsgCod = MsgCod;	// Message to be contracted/expanded
   Msg_PutHiddenParamsOneMsg ();
   HTM_BUTTON_SUBMIT_Begin (Expanded ? Txt_Hide_message :
				       Txt_See_message,
			    Open ? "BT_LINK LT MSG_TIT" :
				   "BT_LINK LT MSG_TIT_NEW",
			    NULL);

   /***** Write subject *****/
   if (Subject[0])
      HTM_Txt (Subject);
   else
      HTM_TxtF ("[%s]",Txt_no_subject);

   /***** End form to expand the message *****/
   HTM_BUTTON_End ();
   Frm_EndForm ();

   /***** End cell *****/
   HTM_TD_End ();
  }

/*****************************************************************************/
/************************ Write the author of a message **********************/
/*****************************************************************************/
// Input: UsrDat must hold user's data

void Msg_WriteMsgAuthor (struct UsrData *UsrDat,bool Enabled,const char *BgColor)
  {
   extern const char *Txt_Unknown_or_without_photo;
   bool ShowPhoto = false;
   char PhotoURL[PATH_MAX + 1];
   bool WriteAuthor;

   /***** Write author name or don't write it? *****/
   WriteAuthor = false;
   if (Enabled)
      if (UsrDat->UsrCod > 0)
         WriteAuthor = true;

   /***** Begin table and row *****/
   HTM_TABLE_BeginPadding (2);
   HTM_TR_Begin (NULL);

   /***** Start first column with author's photo
          (if author has a web page, put a link to it) *****/
   if (BgColor)
      HTM_TD_Begin ("class=\"CT %s\" style=\"width:30px;\"",BgColor);
   else
      HTM_TD_Begin ("class=\"CT\" style=\"width:30px;\"");

   if (WriteAuthor)
     {
      ShowPhoto = Pho_ShowingUsrPhotoIsAllowed (UsrDat,PhotoURL);
      Pho_ShowUsrPhoto (UsrDat,ShowPhoto ? PhotoURL :
                                           NULL,
                        "PHOTO30x40",Pho_ZOOM,false);
     }
   else
      Ico_PutIcon ("usr_bl.jpg",Txt_Unknown_or_without_photo,"PHOTO30x40");
   HTM_TD_End ();

   /***** Second column with user name (if author has a web page, put a link to it) *****/
   if (WriteAuthor)
     {
      if (BgColor)
	 HTM_TD_Begin ("class=\"LT %s\"",BgColor);
      else
	 HTM_TD_Begin ("class=\"LT\"");

      HTM_DIV_Begin ("class=\"AUTHOR_2_LINES\"");	// Limited width
      Usr_WriteFirstNameBRSurnames (UsrDat);
      HTM_DIV_End ();
     }
   else
     {
      if (BgColor)
         HTM_TD_Begin ("class=\"LM %s\"",BgColor);
      else
	 HTM_TD_Begin ("class=\"LM\"");
     }

   /***** End second column *****/
   HTM_TD_End ();

   /***** End row and table *****/
   HTM_TR_End ();
   HTM_TABLE_End ();
  }

/*****************************************************************************/
/********************* Write course origin of a message **********************/
/*****************************************************************************/
// Returns true if the origin course is the current course

bool Msg_WriteCrsOrgMsg (long CrsCod)
  {
   extern const char *Txt_from_this_course;
   extern const char *Txt_Go_to_X;
   extern const char *Txt_no_course_of_origin;
   struct Course Crs;
   bool FromThisCrs = true;
   bool ThereIsOrgCrs = false;

   if (CrsCod > 0)
     {
      /* Get new course code from old course code */
      Crs.CrsCod = CrsCod;

      /* Get data of current degree */
      if (Crs_GetDataOfCourseByCod (&Crs))
        {
         ThereIsOrgCrs = true;
         if ((FromThisCrs = (CrsCod == Gbl.Hierarchy.Crs.CrsCod)))	// Message sent from current course
           {
            HTM_DIV_Begin ("class=\"AUTHOR_TXT\"");
            HTM_TxtF ("(%s)",Txt_from_this_course);
            HTM_DIV_End ();
           }
         else	// Message sent from another course
           {
            /* Write course, including link */
            Frm_StartFormGoTo (ActSeeCrsInf);
            Crs_PutParamCrsCod (Crs.CrsCod);
            HTM_DIV_Begin ("class=\"AUTHOR_TXT\"");
            HTM_Txt ("(");
            snprintf (Gbl.Title,sizeof (Gbl.Title),
        	      Txt_Go_to_X,
		      Crs.FullName);
            HTM_BUTTON_SUBMIT_Begin (Gbl.Title,"BT_LINK AUTHOR_TXT",NULL);
            HTM_Txt (Crs.ShrtName);
            HTM_BUTTON_End ();
            HTM_Txt (")");
            HTM_DIV_End ();
            Frm_EndForm ();
           }
	}
     }
   if (!ThereIsOrgCrs)	// It's an old message without origin source specified, or is a message sent from none course
     {
      HTM_DIV_Begin ("class=\"AUTHOR_TXT\"");
      HTM_TxtF ("(%s)",Txt_no_course_of_origin);
      HTM_DIV_End ();
     }

   return FromThisCrs;
  }

/*****************************************************************************/
/************************* Write form to reply a message *********************/
/*****************************************************************************/

static void Msg_WriteFormToReply (long MsgCod,long CrsCod,
                                  bool FromThisCrs,bool Replied,
                                  const struct UsrData *UsrDat)
  {
   extern const char *Txt_Reply;
   extern const char *Txt_Reply_again;
   extern const char *Txt_Go_to_course_and_reply;
   extern const char *Txt_Go_to_course_and_reply_again;

   /***** Form start and parameters *****/
   if (FromThisCrs)
      Frm_StartForm (ActReqMsgUsr);
   else	// Not the current course ==> go to another course
     {
      Frm_StartFormGoTo (ActReqMsgUsr);
      Crs_PutParamCrsCod (CrsCod);
     }
   Grp_PutParamAllGroups ();
   Par_PutHiddenParamChar ("IsReply",'Y');
   Msg_PutHiddenParamMsgCod (MsgCod);
   Usr_PutParamUsrCodEncrypted (UsrDat->EncryptedUsrCod);
   Par_PutHiddenParamChar ("ShowOnlyOneRecipient",'Y');

   /****** Link and form end *****/
   Ico_PutIconLink ("reply.svg",
                    FromThisCrs ? (Replied ? Txt_Reply_again :
				             Txt_Reply) :
				  (Replied ? Txt_Go_to_course_and_reply_again :
				             Txt_Go_to_course_and_reply));
   Frm_EndForm ();
  }

/*****************************************************************************/
/************************** Write author of a message ************************/
/*****************************************************************************/

static void Msg_WriteMsgFrom (struct UsrData *UsrDat,bool Deleted)
  {
   extern const char *Txt_MSG_Sent;
   extern const char *Txt_MSG_Sent_and_deleted;
   extern const char *Txt_ROLES_SINGUL_abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   bool ShowPhoto;
   char PhotoURL[PATH_MAX + 1];

   HTM_TABLE_Begin (NULL);
   HTM_TR_Begin (NULL);

   /***** Put an icon to show if user has read the message *****/
   HTM_TD_Begin ("class=\"LM\" style=\"width:20px;\"");
   Ico_PutIcon (Deleted ? "share-red.svg" :
        	          "share.svg",
	        Deleted ? Txt_MSG_Sent_and_deleted :
                          Txt_MSG_Sent,
		"ICO16x16");
   HTM_TD_End ();

   /***** Put user's photo *****/
   HTM_TD_Begin ("class=\"CM\" style=\"width:30px;\"");
   ShowPhoto = (Pho_ShowingUsrPhotoIsAllowed (UsrDat,PhotoURL));
   Pho_ShowUsrPhoto (UsrDat,ShowPhoto ? PhotoURL :
                	                NULL,
                     "PHOTO21x28",Pho_ZOOM,false);
   HTM_TD_End ();

   /***** Write user's name *****/
   HTM_TD_Begin ("class=\"AUTHOR_TXT LM\"");
   if (UsrDat->UsrCod > 0)
     {
      HTM_Txt (UsrDat->FullName);
      if (Act_GetSuperAction (Gbl.Action.Act) == ActSeeRcvMsg)
	{
         HTM_NBSP ();
         if (Msg_CheckIfUsrIsBanned (UsrDat->UsrCod,Gbl.Usrs.Me.UsrDat.UsrCod))
            // Sender is banned
            Msg_PutFormToUnbanSender (UsrDat);
         else
            // Sender is not banned
            Msg_PutFormToBanSender (UsrDat);
	}
     }
   else
      HTM_TxtF ("[%s]",Txt_ROLES_SINGUL_abc[Rol_UNK][Usr_SEX_UNKNOWN]);	// User not found, likely an old user who has been removed
   HTM_TD_End ();

   HTM_TR_End ();
   HTM_TABLE_End ();
  }

/*****************************************************************************/
/******************** Write list of recipients of a message ******************/
/*****************************************************************************/

#define Msg_MAX_RECIPIENTS_TO_SHOW 10	// If number of recipients <= Msg_MAX_RECIPIENTS_TO_SHOW, show all recipients
#define Msg_DEF_RECIPIENTS_TO_SHOW  5	// If number of recipients  > Msg_MAX_RECIPIENTS_TO_SHOW, show only Msg_DEF_RECIPIENTS_TO_SHOW

static void Msg_WriteMsgTo (long MsgCod)
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
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumRcp;
   unsigned NumRecipientsTotal;
   unsigned NumRecipientsKnown;
   unsigned NumRecipientsUnknown;
   unsigned NumRecipientsToShow;
   struct UsrData UsrDat;
   bool Deleted;
   bool OpenByDst;
   bool UsrValid;
   bool ShowPhoto;
   const char *Title;
   char PhotoURL[PATH_MAX + 1];
   static const Act_Action_t ActionSee[Msg_NUM_TYPES_OF_MSGS] =
     {
      ActSeeRcvMsg,
      ActSeeSntMsg,
     };

   /***** Get number of recipients of a message from database *****/
   NumRecipientsTotal =
   (unsigned) DB_QueryCOUNT ("can not get number of recipients",
			     "SELECT "
			     "(SELECT COUNT(*) FROM msg_rcv"
			     " WHERE MsgCod=%ld)"
			     " + "
			     "(SELECT COUNT(*) FROM msg_rcv_deleted"
			     " WHERE MsgCod=%ld)",
			     MsgCod,MsgCod);

   /***** Get recipients of a message from database *****/
   NumRecipientsKnown =
   (unsigned) DB_QuerySELECT (&mysql_res,"can not get recipients of a message",
			      "(SELECT msg_rcv.UsrCod,"
				      "'N',"
				      "msg_rcv.Open,"
				      "usr_data.Surname1 AS S1,"
				      "usr_data.Surname2 AS S2,"
				      "usr_data.FirstName AS FN"
			      " FROM msg_rcv,usr_data"
			      " WHERE msg_rcv.MsgCod=%ld"
			      " AND msg_rcv.UsrCod=usr_data.UsrCod)"
			      " UNION "
			      "(SELECT msg_rcv_deleted.UsrCod,"
				      "'Y',"
				      "msg_rcv_deleted.Open,"
				      "usr_data.Surname1 AS S1,"
				      "usr_data.Surname2 AS S2,"
				      "usr_data.FirstName AS FN"
			      " FROM msg_rcv_deleted,usr_data"
			      " WHERE msg_rcv_deleted.MsgCod=%ld"
			      " AND msg_rcv_deleted.UsrCod=usr_data.UsrCod)"
			      " ORDER BY S1,S2,FN",
			      MsgCod,MsgCod);

   /***** Check number of recipients *****/
   if (NumRecipientsTotal)
     {
      /***** Begin table *****/
      HTM_TABLE_Begin (NULL);

      /***** How many recipients will be shown? *****/
      if (NumRecipientsKnown <= Msg_MAX_RECIPIENTS_TO_SHOW)
         NumRecipientsToShow = NumRecipientsKnown;
      else	// A lot of recipients
         /***** Get parameter that indicates if I want to see all recipients *****/
         NumRecipientsToShow = Par_GetParToBool ("SeeAllRcpts") ? NumRecipientsKnown :
                                                                  Msg_DEF_RECIPIENTS_TO_SHOW;

      /***** Initialize structure with user's data *****/
      Usr_UsrDataConstructor (&UsrDat);

      /***** Write known recipients *****/
      for (NumRcp = 0;
	   NumRcp < NumRecipientsToShow;
	   NumRcp++)
        {
         /* Get user's code */
         row = mysql_fetch_row (mysql_res);
         UsrDat.UsrCod = Str_ConvertStrCodToLongCod (row[0]);

         /* Get if message has been deleted by recipient */
         Deleted   = (row[1][0] == 'Y');

         /* Get if message has been read by recipient */
         OpenByDst = (row[2][0] == 'Y');

         /* Get user's data */
	 UsrValid = Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat,Usr_DONT_GET_PREFS);

         /* Put an icon to show if user has read the message */
	 Title = OpenByDst ? (Deleted ? Txt_MSG_Open_and_deleted :
                                        Txt_MSG_Open) :
                             (Deleted ? Txt_MSG_Deleted_without_opening :
                                        Txt_MSG_Unopened);
         HTM_TR_Begin (NULL);

         HTM_TD_Begin ("class=\"LM\" style=\"width:20px;\"");
         Ico_PutIcon (OpenByDst ? (Deleted ? "envelope-open-text-red.svg"   :
                	                     "envelope-open-text.svg") :
                                  (Deleted ? "envelope-red.svg" :
                        	             "envelope.svg"),
                      Title,"ICO16x16");
         HTM_TD_End ();

         /* Put user's photo */
         HTM_TD_Begin ("class=\"CT\" style=\"width:30px;\"");
         ShowPhoto = (UsrValid ? Pho_ShowingUsrPhotoIsAllowed (&UsrDat,PhotoURL) :
                                 false);
         Pho_ShowUsrPhoto (&UsrDat,ShowPhoto ? PhotoURL :
                        	               NULL,
                           "PHOTO21x28",Pho_ZOOM,false);
         HTM_TD_End ();

         /* Write user's name */
         HTM_TD_Begin ("class=\"%s LM\"",OpenByDst ? "AUTHOR_TXT" :
                	                                      "AUTHOR_TXT_NEW");
         if (UsrValid)
            HTM_Txt (UsrDat.FullName);
         else
            HTM_TxtF ("[%s]",Txt_unknown_recipient);	// User not found, likely a user who has been removed
         HTM_TD_End ();

         HTM_TR_End ();
        }

      /***** If any recipients are unknown *****/
      if ((NumRecipientsUnknown = NumRecipientsTotal - NumRecipientsKnown))
	{
         /***** Begin form to show all the users *****/
         HTM_TR_Begin (NULL);

	 HTM_TD_Begin ("colspan=\"3\" class=\"AUTHOR_TXT LM\"");
	 HTM_TxtF ("[%u %s]",
                   NumRecipientsUnknown,
                   (NumRecipientsUnknown == 1) ? Txt_unknown_recipient :
                                                 Txt_unknown_recipients);
	 HTM_TD_End ();

	 HTM_TR_End ();
	}

      /***** If any known recipient is not listed *****/
      if (NumRecipientsToShow < NumRecipientsKnown)
        {
         /***** Begin form to show all the users *****/
         HTM_TR_Begin (NULL);

         HTM_TD_Begin ("colspan=\"3\" class=\"AUTHOR_TXT LM\"");
         Frm_StartForm (ActionSee[Gbl.Msg.TypeOfMessages]);
         Gbl.Msg.MsgCod = MsgCod;	// Message to be expanded with all recipients visible
         Msg_PutHiddenParamsOneMsg ();
         Par_PutHiddenParamChar ("SeeAllRcpts",'Y');
         HTM_BUTTON_SUBMIT_Begin (Txt_View_all_recipients,"BT_LINK AUTHOR_TXT",NULL);
         HTM_TxtF (Txt_and_X_other_recipients,
                   NumRecipientsKnown - NumRecipientsToShow);
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

void Msg_WriteMsgDate (time_t TimeUTC,const char *ClassBackground)
  {
   static unsigned UniqueId = 0;
   char *Id;

   UniqueId++;
   if (asprintf (&Id,"msg_date_%u",UniqueId) < 0)
      Lay_NotEnoughMemoryExit ();

   /***** Start cell *****/
   HTM_TD_Begin ("id=\"%s\" class=\"%s RT\" style=\"width:106px;\"",
                 Id,ClassBackground);

   /***** Write date and time *****/
   Dat_WriteLocalDateHMSFromUTC (Id,TimeUTC,
				 Gbl.Prefs.DateFormat,Dat_SEPARATOR_COMMA,
				 true,true,false,0x6);

   /***** End cell *****/
   HTM_TD_End ();

   free (Id);
  }

/*****************************************************************************/
/********************* Write the text (content) of a message *****************/
/*****************************************************************************/

void Msg_WriteMsgContent (char *Content,unsigned long MaxLength,bool InsertLinks,bool ChangeBRToRet)
  {
   /***** Insert links in URLs *****/
   if (InsertLinks)
      Str_InsertLinks (Content,MaxLength,60);

   /***** Write message to file *****/
   if (ChangeBRToRet)
      Str_FilePrintStrChangingBRToRetAndNBSPToSpace (Gbl.F.Out,Content);
   else
      HTM_Txt (Content);
  }

/*****************************************************************************/
/*************** Get parameter with the code of a message ********************/
/*****************************************************************************/

void Msg_PutHiddenParamMsgCod (long MsgCod)
  {
   Par_PutHiddenParamLong (NULL,"MsgCod",MsgCod);
  }

/*****************************************************************************/
/*************** Get parameter with the code of a message ********************/
/*****************************************************************************/

static long Msg_GetParamMsgCod (void)
  {
   /***** Get code of message *****/
   return Par_GetParToLong ("MsgCod");
  }

/*****************************************************************************/
/***************** Put a form to ban the sender of a message *****************/
/*****************************************************************************/

static void Msg_PutFormToBanSender (struct UsrData *UsrDat)
  {
   extern const char *Txt_Sender_permitted_click_to_ban_him;

   Frm_StartForm (ActBanUsrMsg);
   Pag_PutHiddenParamPagNum (Msg_WhatPaginate[Gbl.Msg.TypeOfMessages],
	                     Gbl.Msg.CurrentPage);
   Usr_PutParamUsrCodEncrypted (UsrDat->EncryptedUsrCod);
   Msg_PutHiddenParamsMsgsFilters ();
   Ico_PutIconLink ("unlock.svg",Txt_Sender_permitted_click_to_ban_him);
   Frm_EndForm ();
  }

/*****************************************************************************/
/**************** Put a form to unban the sender of a message ****************/
/*****************************************************************************/

static void Msg_PutFormToUnbanSender (struct UsrData *UsrDat)
  {
   extern const char *Txt_Sender_banned_click_to_unban_him;

   Frm_StartForm (ActUnbUsrMsg);
   Pag_PutHiddenParamPagNum (Msg_WhatPaginate[Gbl.Msg.TypeOfMessages],
	                     Gbl.Msg.CurrentPage);
   Usr_PutParamUsrCodEncrypted (UsrDat->EncryptedUsrCod);
   Msg_PutHiddenParamsMsgsFilters ();
   Ico_PutIconLink ("lock.svg",Txt_Sender_banned_click_to_unban_him);
   Frm_EndForm ();
  }

/*****************************************************************************/
/********* Ban a sender of a message when showing received messages **********/
/*****************************************************************************/

void Msg_BanSenderWhenShowingMsgs (void)
  {
   extern const char *Txt_From_this_time_you_will_not_receive_messages_from_X;

   /***** Get user's code from form *****/
   Usr_GetParamOtherUsrCodEncryptedAndGetListIDs ();

   /***** Get password, user type and user's data from database *****/
   if (!Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&Gbl.Usrs.Other.UsrDat,Usr_DONT_GET_PREFS))
      Lay_ShowErrorAndExit ("Sender does not exist.");

   /***** Insert pair (sender's code - my code) in table of banned senders if not inserted *****/
   DB_QueryREPLACE ("can not ban sender",
		    "REPLACE INTO msg_banned"
		    " (FromUsrCod,ToUsrCod)"
		    " VALUES"
		    " (%ld,%ld)",
                    Gbl.Usrs.Other.UsrDat.UsrCod,Gbl.Usrs.Me.UsrDat.UsrCod);

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
   Usr_GetParamOtherUsrCodEncryptedAndGetListIDs ();

   /***** Get password, user type and user's data from database *****/
   if (!Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&Gbl.Usrs.Other.UsrDat,Usr_DONT_GET_PREFS))
      Lay_ShowErrorAndExit ("Sender does not exist.");

   /***** Remove pair (sender's code - my code) from table of banned senders *****/
   DB_QueryDELETE ("can not ban sender",
		   "DELETE FROM msg_banned"
		   " WHERE FromUsrCod=%ld AND ToUsrCod=%ld",
                   Gbl.Usrs.Other.UsrDat.UsrCod,Gbl.Usrs.Me.UsrDat.UsrCod);

   /***** Show alert with the change made *****/
   Ale_ShowAlert (Ale_SUCCESS,Txt_From_this_time_you_can_receive_messages_from_X,
                  Gbl.Usrs.Other.UsrDat.FullName);
  }

/*****************************************************************************/
/**************** Chech if a user is banned by another user ******************/
/*****************************************************************************/

bool Msg_CheckIfUsrIsBanned (long FromUsrCod,long ToUsrCod)
  {
   /***** Get if FromUsrCod is banned by ToUsrCod *****/
   return (DB_QueryCOUNT ("can not check if a user is banned",
			  "SELECT COUNT(*) FROM msg_banned"
			  " WHERE FromUsrCod=%ld AND ToUsrCod=%ld",
			  FromUsrCod,ToUsrCod) != 0);
  }

/*****************************************************************************/
/************************ Remove user from banned table **********************/
/*****************************************************************************/

void Msg_RemoveUsrFromBanned (long UsrCod)
  {
   /***** Remove pair (sender's code - my code)
          from table of banned senders *****/
   DB_QueryDELETE ("can not remove user from table of banned users",
		   "DELETE FROM msg_banned"
		   " WHERE FromUsrCod=%ld OR ToUsrCod=%ld",
                   UsrCod,UsrCod);
  }

/*****************************************************************************/
/*********** List banned users, who can not sent messages to me **************/
/*****************************************************************************/

void Msg_ListBannedUsrs (void)
  {
   extern const char *Txt_You_have_not_banned_any_sender;
   extern const char *Txt_Banned_users;
   extern const char *Txt_Sender_banned_click_to_unban_him;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumUsr,NumUsrs;
   struct UsrData UsrDat;
   bool ShowPhoto;
   char PhotoURL[PATH_MAX + 1];

   /***** Get my banned users *****/
   NumUsrs = (unsigned) DB_QuerySELECT (&mysql_res,"can not get banned users",
				        "SELECT msg_banned.FromUsrCod"
				        " FROM msg_banned,usr_data"
				        " WHERE msg_banned.ToUsrCod=%ld"
				        " AND msg_banned.FromUsrCod=usr_data.UsrCod"
				        " ORDER BY usr_data.Surname1,"
				                  "usr_data.Surname2,"
				                  "usr_data.FirstName",
				        Gbl.Usrs.Me.UsrDat.UsrCod);

   if (NumUsrs == 0)   // If not result ==> sent message is deleted
      Ale_ShowAlert (Ale_INFO,Txt_You_have_not_banned_any_sender);
   else
     {
      /***** Initialize structure with user's data *****/
      Usr_UsrDataConstructor (&UsrDat);

      /***** Begin box and table *****/
      Box_StartBoxTable (NULL,Txt_Banned_users,NULL,
                         NULL,Box_NOT_CLOSABLE,2);

      /***** List users *****/
      for (NumUsr = 1;
	   NumUsr <= NumUsrs;
	   NumUsr++)
        {
         row = mysql_fetch_row (mysql_res);

         /* Get user's code (row[0]) */
         UsrDat.UsrCod = Str_ConvertStrCodToLongCod (row[0]);

         /* Get user's data from database */
         if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat,Usr_DONT_GET_PREFS))
           {
            HTM_TR_Begin (NULL);

            /* Put form to unban user */
            HTM_TD_Begin ("class=\"BM\"");
            Frm_StartForm (ActUnbUsrLst);
            Usr_PutParamUsrCodEncrypted (UsrDat.EncryptedUsrCod);
            Ico_PutIconLink ("lock.svg",Txt_Sender_banned_click_to_unban_him);
            Frm_EndForm ();
            HTM_TD_End ();

            /* Show photo */
            HTM_TD_Begin ("class=\"LM\" style=\"width:30px;\"");
            ShowPhoto = Pho_ShowingUsrPhotoIsAllowed (&UsrDat,PhotoURL);
            Pho_ShowUsrPhoto (&UsrDat,ShowPhoto ? PhotoURL :
                        	                  NULL,
                              "PHOTO21x28",Pho_ZOOM,false);
            HTM_TD_End ();

            /* Write user's full name */
            HTM_TD_Begin ("class=\"DAT LM\"");
            HTM_Txt (UsrDat.FullName);
            HTM_TD_End ();

            HTM_TR_End ();
           }
        }

      /***** End table and box *****/
      Box_EndBoxTable ();

      /***** Free memory used for user's data *****/
      Usr_UsrDataDestructor (&UsrDat);
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }
