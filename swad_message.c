// swad_message.c: messages between users

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2016 Antonio Cañas Vargas

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

#include <linux/limits.h>	// For PATH_MAX
#include <linux/stddef.h>	// For NULL
#include <stdlib.h>		// For free
#include <string.h>		// For string functions
#include <time.h>		// For time

#include "swad_action.h"
#include "swad_config.h"
#include "swad_course.h"
#include "swad_database.h"
#include "swad_forum.h"
#include "swad_global.h"
#include "swad_group.h"
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

#define Msg_MAX_LENGTH_MESSAGES_QUERY 4096

// Forum images will be saved with:
// - maximum width of Msg_IMAGE_SAVED_MAX_HEIGHT
// - maximum height of Msg_IMAGE_SAVED_MAX_HEIGHT
// - maintaining the original aspect ratio (aspect ratio recommended: 3:2)
#define Msg_IMAGE_SAVED_MAX_WIDTH	768
#define Msg_IMAGE_SAVED_MAX_HEIGHT	512
#define Msg_IMAGE_SAVED_QUALITY		 75	// 1 to 100

/*****************************************************************************/
/******************************** Private types ******************************/
/*****************************************************************************/

/*****************************************************************************/
/**************************** Internal prototypes ****************************/
/*****************************************************************************/

static void Msg_PutFormMsgUsrs (char *Content);

static void Msg_ShowSentOrReceivedMessages (void);
static unsigned long Msg_GetNumUsrsBannedByMe (void);
static void Msg_PutLinkToViewBannedUsers(void);
static void Msg_ConstructQueryToSelectSentOrReceivedMsgs (char *Query,long UsrCod,
                                                          long FilterCrsCod,const char *FilterFromToSubquery);

static char *Msg_WriteNumMsgs (unsigned NumUnreadMsgs);

static void Msg_PutIconsListMsgs (void);
static void Msg_PutIconToRemoveOneRcvMsg (void);
static void Msg_PutIconToRemoveSevRcvMsgs (void);
static void Msg_PutIconToRemoveOneSntMsg (void);
static void Msg_PutIconToRemoveSevSntMsgs (void);

static void Msg_ShowFormToShowOnlyUnreadMessages (void);
static void Msg_GetParamOnlyUnreadMsgs (void);
static void Msg_ShowASentOrReceivedMessage (long MsgNum,long MsgCod);
static void Msg_GetStatusOfSentMsg (long MsgCod,bool *Expanded);
static void Msg_GetStatusOfReceivedMsg (long MsgCod,bool *Open,bool *Replied,bool *Expanded);
static long Msg_GetParamMsgCod (void);
static void Msg_PutLinkToShowMorePotentialRecipients (void);
static void Msg_PutParamsShowMorePotentialRecipients (void);
static void Msg_ShowOneUniqueRecipient (void);
static void Msg_WriteFormUsrsIDsOrNicksOtherRecipients (void);
static void Msg_WriteFormSubjectAndContentMsgToUsrs (char *Content);
static void Msg_ShowNumMsgsDeleted (unsigned NumMsgs);

static void Msg_MakeFilterFromToSubquery (char *FilterFromToSubquery);

static void Msg_ExpandSentMsg (long MsgCod);
static void Msg_ExpandReceivedMsg (long MsgCod);
static void Msg_ContractSentMsg (long MsgCod);
static void Msg_ContractReceivedMsg (long MsgCod);

static long Msg_InsertNewMsg (const char *Subject,const char *Content,
                              struct Image *Image);

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
                               time_t *CreatTimeUTC,char *Subject,bool *Deleted);
static void Msg_GetMsgContent (long MsgCod,char *Content,struct Image *Image);

static void Msg_WriteSentOrReceivedMsgSubject (long MsgCod,const char *Subject,bool Open,bool Expanded);
static void Msg_WriteFormToReply (long MsgCod,long CrsCod,
                                  bool ThisCrs,bool Replied,
                                  const struct UsrData *UsrDat);
static void Msg_WriteMsgFrom (struct UsrData *UsrDat,bool Deleted);
static void Msg_WriteMsgTo (long MsgCod);

static void Msg_PutFormToDeleteMessage (long MsgCod);

static void Msg_PutFormToBanSender (struct UsrData *UsrDat);
static void Msg_PutFormToUnbanSender (struct UsrData *UsrDat);
static void Msg_UnbanSender (void);
static bool Msg_CheckIfUsrIsBanned (long FromUsrCod,long ToUsrCod);

/*****************************************************************************/
/***************** Put a form to write a new message to users ****************/
/*****************************************************************************/

void Msg_FormMsgUsrs (void)
  {
   char Content[Cns_MAX_BYTES_LONG_TEXT+1];

   /***** Get possible hidden subject and content of the message *****/
   Par_GetParToHTML ("HiddenSubject",Gbl.Msg.Subject,Cns_MAX_BYTES_SUBJECT);
   Par_GetParAndChangeFormat ("HiddenContent",Content,Cns_MAX_BYTES_LONG_TEXT,
                              Str_TO_TEXT,false);

   Msg_PutFormMsgUsrs (Content);
  }

/*****************************************************************************/
/***************** Put a form to write a new message to users ****************/
/*****************************************************************************/

static void Msg_PutFormMsgUsrs (char *Content)
  {
   extern const char *Hlp_MESSAGES_Write;
   extern const char *The_ClassForm[The_NUM_THEMES];
   extern const char *Txt_Reply_message;
   extern const char *Txt_New_message;
   extern const char *Txt_MSG_To;
   extern const char *Txt_Send_message;
   char YN[1+1];
   unsigned NumUsrsInCrs;
   bool ShowUsrsInCrs = false;
   bool GetUsrsInCrs;

   Gbl.Usrs.LstUsrs[Rol_TEACHER].NumUsrs =
   Gbl.Usrs.LstUsrs[Rol_STUDENT].NumUsrs = 0;

   /***** Get parameter that indicates if the message is a reply to another message *****/
   Par_GetParToText ("IsReply",YN,1);
   if ((Gbl.Msg.Reply.IsReply = (Str_ConvertToUpperLetter (YN[0]) == 'Y')))
      /* Get original message code */
      if ((Gbl.Msg.Reply.OriginalMsgCod = Msg_GetParamMsgCod ()) <= 0)
         Lay_ShowErrorAndExit ("Wrong code of message.");

   /***** Get user's code of possible preselected recipient *****/
   if (Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ())	// There is a preselected recipient
     {
      /* Get who to show as potential recipients:
         - only the selected recipient
         - any user (default) */
      Par_GetParToText ("ShowOnlyOneRecipient",YN,1);
      Gbl.Msg.ShowOnlyOneRecipient = (Str_ConvertToUpperLetter (YN[0]) == 'Y');
     }
   else
      Gbl.Msg.ShowOnlyOneRecipient = false;

   GetUsrsInCrs = !Gbl.Msg.ShowOnlyOneRecipient &&	// Show list of potential recipients
	          (Gbl.Usrs.Me.IBelongToCurrentCrs ||	// If there is a course selected and I belong to it
	           Gbl.Usrs.Me.LoggedRole == Rol_SYS_ADM);

   if (GetUsrsInCrs)
     {
      /***** Get and update type of list,
	     number of columns in class photo
	     and preference about view photos *****/
      Usr_GetAndUpdatePrefsAboutUsrList ();

      /***** Get groups to show ******/
      Grp_GetParCodsSeveralGrpsToShowUsrs ();

      /***** Get and order lists of users from this course *****/
      Usr_GetListUsrs (Rol_TEACHER,Sco_SCOPE_CRS);
      Usr_GetListUsrs (Rol_STUDENT,Sco_SCOPE_CRS);
      NumUsrsInCrs = Gbl.Usrs.LstUsrs[Rol_TEACHER].NumUsrs +
		     Gbl.Usrs.LstUsrs[Rol_STUDENT].NumUsrs;
     }

   /***** Start frame *****/
   Lay_StartRoundFrame (NULL,
			Gbl.Msg.Reply.IsReply ? Txt_Reply_message :
						Txt_New_message,
			NULL,Hlp_MESSAGES_Write);

   if (Gbl.Msg.ShowOnlyOneRecipient)
      /***** Form to show several potential recipients *****/
      Msg_PutLinkToShowMorePotentialRecipients ();
   else
     {
      /***** Get list of users belonging to the current course *****/
      if (GetUsrsInCrs)
	{
	 /***** Form to select groups *****/
	 Grp_ShowFormToSelectSeveralGroups (ActReqMsgUsr);

	 if (NumUsrsInCrs)
	   {
	    /***** Form to select type of list used for select several users *****/
	    Usr_ShowFormsToSelectUsrListType (ActReqMsgUsr);

	    /***** Check if it's a big list *****/
	    ShowUsrsInCrs = Usr_GetIfShowBigList (NumUsrsInCrs,"CopyMessageToHiddenFields()");

	    if (ShowUsrsInCrs)
	       /***** Get lists of selected users *****/
	       Usr_GetListsSelectedUsrsCods ();
	   }
        }

      /***** Get list of users' IDs or nicknames written explicitely *****/
      Usr_GetListMsgRecipientsWrittenExplicitelyBySender (false);
     }

   /***** Start form to select recipients and write the message *****/
   Act_FormStart (ActRcvMsgUsr);
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

   /***** Start table *****/
   fprintf (Gbl.F.Out,"<table class=\"CELLS_PAD_2\">");

   /***** "To:" section (recipients) *****/
   fprintf (Gbl.F.Out,"<tr>"
		      "<td class=\"%s RIGHT_TOP\">"
		      "%s:"
		      "</td>"
		      "<td class=\"LEFT_TOP\">",
	    The_ClassForm[Gbl.Prefs.Theme],Txt_MSG_To);
   if (Gbl.Msg.ShowOnlyOneRecipient)
      /***** Show only one user as recipient *****/
      Msg_ShowOneUniqueRecipient ();
   else
     {
      /***** Show potential recipients *****/
      fprintf (Gbl.F.Out,"<table style=\"width:100%%;\">");
      if (ShowUsrsInCrs)
	{
	 Usr_ListUsersToSelect (Rol_TEACHER);	// All teachers in course
	 Usr_ListUsersToSelect (Rol_STUDENT);	// All students in selected groups
	}
      Msg_WriteFormUsrsIDsOrNicksOtherRecipients ();	// Other users (nicknames)
      fprintf (Gbl.F.Out,"</table>");
     }

   fprintf (Gbl.F.Out,"</td>"
		      "</tr>");

   /***** Subject and content sections *****/
   Msg_WriteFormSubjectAndContentMsgToUsrs (Content);

   /***** End table *****/
   fprintf (Gbl.F.Out,"</table>");

   /***** Help for text editor and send button *****/
   Lay_HelpPlainEditor ();

   /***** Attached image (optional) *****/
   Img_PutImageUploader (-1,"MSG_IMG_TIT_URL");

   /***** Send button *****/
   Lay_PutCreateButton (Txt_Send_message);

   /***** End form *****/
   Act_FormEnd ();

   /***** End frame *****/
   Lay_EndRoundFrame ();

   /***** Free memory used by the list of nicknames *****/
   Usr_FreeListOtherRecipients ();

   /***** Free memory used for by the lists of users *****/
   if (GetUsrsInCrs)
     {
      Usr_FreeUsrsList (Rol_TEACHER);
      Usr_FreeUsrsList (Rol_STUDENT);
     }

   /***** Free memory used by list of selected users' codes *****/
   Usr_FreeListsSelectedUsrsCods ();

   /***** Free memory for list of selected groups *****/
   Grp_FreeListCodSelectedGrps ();
  }

/*****************************************************************************/
/********** Put contextual link to show more potential recipients ************/
/*****************************************************************************/

static void Msg_PutLinkToShowMorePotentialRecipients (void)
  {
   extern const char *Txt_Show_more_recipients;

   fprintf (Gbl.F.Out,"<div class=\"CONTEXT_MENU\">");
   Lay_PutContextualLink (ActReqMsgUsr,Msg_PutParamsShowMorePotentialRecipients,
			  "usrs64x64.gif",
			  Txt_Show_more_recipients,Txt_Show_more_recipients,
                          "CopyMessageToHiddenFields()");
   fprintf (Gbl.F.Out,"</div>");
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
/********** Put hidden parameters with message subject and content ***********/
/*****************************************************************************/

void Msg_PutHiddenParamsSubjectAndContent (void)
  {
   /***** Hidden params to send subject and content.
          When the user edit the subject or the content,
          they are copied here. *****/
   fprintf (Gbl.F.Out,"<input type=\"hidden\" name=\"HiddenSubject\" value=\"\" />"
                      "<input type=\"hidden\" name=\"HiddenContent\" value=\"\" />");
  }

/*****************************************************************************/
/************ Put parameters to show more potential recipients ***************/
/*****************************************************************************/

static void Msg_ShowOneUniqueRecipient (void)
  {
   char PhotoURL[PATH_MAX+1];
   bool ShowPhoto;

   /***** Show user's photo *****/
   ShowPhoto = Pho_ShowUsrPhotoIsAllowed (&Gbl.Usrs.Other.UsrDat,PhotoURL);
   Pho_ShowUsrPhoto (&Gbl.Usrs.Other.UsrDat,ShowPhoto ? PhotoURL :
					                NULL,
		     "PHOTO21x28",Pho_ZOOM,false);

   /****** Write user's IDs ******/
   fprintf (Gbl.F.Out,"<div class=\"MSG_TO_ONE_RCP %s\">",
            Gbl.Usrs.Other.UsrDat.Accepted ? "DAT_SMALL_NOBR_N" :
        	                             "DAT_SMALL_NOBR");
   ID_WriteUsrIDs (&Gbl.Usrs.Other.UsrDat);
   fprintf (Gbl.F.Out,"</div>");

   /***** Write user's name *****/
   fprintf (Gbl.F.Out,"<div class=\"MSG_TO_ONE_RCP %s\">%s</div>",
            Gbl.Usrs.Other.UsrDat.Accepted ? "DAT_SMALL_NOBR_N" :
        	                             "DAT_SMALL_NOBR",
            Gbl.Usrs.Other.UsrDat.FullName);

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
   char Nickname[Nck_MAX_LENGTH_NICKNAME_WITHOUT_ARROBA+1];	// old version because is a nickname retrieved from database. TODO: change in 2013
   bool PutColspan;
   unsigned Colspan;

   PutColspan = Gbl.CurrentCrs.Crs.CrsCod > 0 &&	// If there is a course selected
                (Gbl.Usrs.Me.IBelongToCurrentCrs ||	// I belong to it
                 Gbl.Usrs.Me.LoggedRole == Rol_SYS_ADM);
   if (PutColspan)
      Colspan = Usr_GetColumnsForSelectUsrs ();

   /***** Textarea with users' @nicknames, emails or IDs *****/
   fprintf (Gbl.F.Out,"<tr>"
	              "<th class=\"LEFT_MIDDLE LIGHT_BLUE\"");
   if (PutColspan)
      fprintf (Gbl.F.Out," colspan=\"%u\">%s:",
	       Colspan,Txt_Other_recipients);
   else
      fprintf (Gbl.F.Out," >%s:",
	       Txt_Recipients);
   fprintf (Gbl.F.Out,"</th>"
	              "</tr>"
                      "<tr>"
                      "<td");
   if (PutColspan)
      fprintf (Gbl.F.Out," colspan=\"%u\"",Colspan);
   fprintf (Gbl.F.Out," class=\"LEFT_MIDDLE\">"
	              "<textarea name=\"OtherRecipients\" cols=\"72\" rows=\"2\""
	              " placeholder=\"%s&hellip;\">",
            Txt_nicks_emails_or_IDs_separated_by_commas);
   if (Gbl.Usrs.ListOtherRecipients[0])
      fprintf (Gbl.F.Out,"%s",Gbl.Usrs.ListOtherRecipients);
//   else if (Gbl.Msg.Reply.IsReply)	// If this is a reply message
   else if (Gbl.Usrs.Other.UsrDat.UsrCod > 0)	// If there is a recipient
						// and there's no list of explicit recipients,
						// write @nickname of original sender
      if (Nck_GetNicknameFromUsrCod (Gbl.Usrs.Other.UsrDat.UsrCod,Nickname))
         fprintf (Gbl.F.Out,"@%s",Nickname);
   fprintf (Gbl.F.Out,"</textarea>"
	              "</td>"
	              "</tr>");
  }

/*****************************************************************************/
/****** Write form fields with subject and content of a message to users *****/
/*****************************************************************************/

static void Msg_WriteFormSubjectAndContentMsgToUsrs (char *Content)
  {
   extern const char *The_ClassForm[The_NUM_THEMES];
   extern const char *Txt_MSG_Subject;
   extern const char *Txt_MSG_Message;
   extern const char *Txt_Original_message;
   char Query[512];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;
   long MsgCod;
   bool SubjectAndContentComeFromForm = (Gbl.Msg.Subject[0] || Content[0]);

   /***** Get possible code (of original message if it's a reply) *****/
   MsgCod = Msg_GetParamMsgCod ();

   /***** Subject of new message *****/
   fprintf (Gbl.F.Out,"<tr>"
	              "<td class=\"%s RIGHT_TOP\">"
	              "%s: "
	              "</td>"
                      "<td class=\"LEFT_MIDDLE\">"
                      "<textarea id=\"MsgSubject\" name=\"Subject\""
                      " cols=\"72\" rows=\"2\">",
            The_ClassForm[Gbl.Prefs.Theme],
            Txt_MSG_Subject);

   /***** If message is a reply ==> get original message *****/
   if (MsgCod > 0)	// It's a reply
     {
      if (!SubjectAndContentComeFromForm)
	{
	 /***** Get subject and content of message from database *****/
	 sprintf (Query,"SELECT Subject,Content FROM msg_content"
			" WHERE MsgCod='%ld'",MsgCod);
	 NumRows = DB_QuerySELECT (Query,&mysql_res,"can not get message content");

	 /***** Result should have a unique row *****/
	 if (NumRows != 1)
	    Lay_ShowErrorAndExit ("Error when getting message.");

	 row = mysql_fetch_row (mysql_res);

	 /* Get subject */
	 strncpy (Gbl.Msg.Subject,row[0],Cns_MAX_BYTES_SUBJECT);
	 Gbl.Msg.Subject[Cns_MAX_BYTES_SUBJECT] = '\0';

	 /* Get content */
	 strncpy (Content,row[1],Cns_MAX_BYTES_LONG_TEXT);
	 Content[Cns_MAX_BYTES_LONG_TEXT] = '\0';

	 /***** Free structure that stores the query result *****/
	 DB_FreeMySQLResult (&mysql_res);
	}

      /***** Write subject *****/
      if (!SubjectAndContentComeFromForm)
	 fprintf (Gbl.F.Out,"Re: ");
      fprintf (Gbl.F.Out,"%s"
                         "</textarea>"
	                 "</td>"
	                 "</tr>",
	       Gbl.Msg.Subject);

      /***** Write content *****/
      fprintf (Gbl.F.Out,"<tr>"
	                 "<td class=\"%s RIGHT_TOP\">"
	                 "%s: "
	                 "</td>"
                         "<td class=\"LEFT_MIDDLE\">"
                         "<textarea id=\"MsgContent\" name=\"Content\""
                         " cols=\"72\" rows=\"20\">",
               The_ClassForm[Gbl.Prefs.Theme],Txt_MSG_Message);

      /* Start textarea with a '\n', that will be not visible in textarea.
         When Content is "\nLorem ipsum" (a white line before "Lorem ipsum"),
         if we don't put the initial '\n' ==> the form will be sent starting
         by "Lorem", without the white line */
      fprintf (Gbl.F.Out,"\n");

      if (!SubjectAndContentComeFromForm)
	 fprintf (Gbl.F.Out,"\n\n----- %s -----\n",
		  Txt_Original_message);

      Msg_WriteMsgContent (Content,Cns_MAX_BYTES_LONG_TEXT,false,true);
     }
   else	// It's not a reply
     {
      /***** Subject of new message *****/
      fprintf (Gbl.F.Out,"%s</textarea>"
	                 "</td>"
	                 "</tr>",
	       Gbl.Msg.Subject);

      /***** Content of new message *****/
      fprintf (Gbl.F.Out,"<tr>"
	                 "<td class=\"%s RIGHT_TOP\">"
	                 "%s: "
	                 "</td>"
                         "<td class=\"LEFT_MIDDLE\">"
                         "<textarea id=\"MsgContent\" name=\"Content\""
                         " cols=\"72\" rows=\"20\">",
               The_ClassForm[Gbl.Prefs.Theme],
               Txt_MSG_Message);

      /* Start textarea with a '\n', that will be not visible in textarea.
         When Content is "\nLorem ipsum" (a white line before "Lorem ipsum"),
         if we don't put the initial '\n' ==> the form will be sent starting
         by "Lorem", without the white line */
      fprintf (Gbl.F.Out,"\n%s",Content);
     }
   fprintf (Gbl.F.Out,"</textarea>"
	              "</td>"
	              "</tr>");
  }

/*****************************************************************************/
/********* Put hidden parameter for another recipient (one nickname) *********/
/*****************************************************************************/

void Msg_PutHiddenParamAnotherRecipient (const struct UsrData *UsrDat)
  {
   char NicknameWithArroba[Nck_MAX_BYTES_NICKNAME_WITH_ARROBA+1];

   sprintf (NicknameWithArroba,"@%s",UsrDat->Nickname);
   Par_PutHiddenParamString ("OtherRecipients",NicknameWithArroba);
  }

/*****************************************************************************/
/********* Put hidden parameter for another recipient (one nickname) *********/
/*****************************************************************************/

void Msg_PutHiddenParamOtherRecipients (void)
  {
   if (Gbl.Usrs.ListOtherRecipients)
      if (Gbl.Usrs.ListOtherRecipients[0])
         Par_PutHiddenParamString ("OtherRecipients",Gbl.Usrs.ListOtherRecipients);
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
   extern const char *Txt_Do_not_reload_this_page_because_the_message_will_be_sent_again_;
   extern const char *Txt_The_message_has_not_been_sent_to_any_recipient;
   extern const char *Txt_The_message_has_been_sent_to_1_recipient;
   extern const char *Txt_The_message_has_been_sent_to_X_recipients;
   extern const char *Txt_There_have_been_X_errors_in_sending_the_message;
   char YN[1+1];
   bool IsReply;
   bool RecipientHasBannedMe;
   bool Replied = false;
   long OriginalMsgCod = -1L;	// Initialized to avoid warning
   const char *Ptr;
   unsigned NumRecipients;
   unsigned NumRecipientsToBeNotifiedByEMail = 0;
   struct UsrData UsrDstData;
   int NumErrors = 0;
   char *ListUsrsDst;
   long NewMsgCod = -1L;	// Initiliazed to avoid warning
   bool MsgAlreadyInserted = false;
   bool CreateNotif;
   bool NotifyByEmail;
   char Content[Cns_MAX_BYTES_LONG_TEXT+1];
   struct Image Image;
   bool Error = false;

   /***** Get data from form *****/
   /* Get subject */
   Par_GetParToHTML ("Subject",Gbl.Msg.Subject,Cns_MAX_BYTES_SUBJECT);

   /* Get body */
   Par_GetParAndChangeFormat ("Content",Content,Cns_MAX_BYTES_LONG_TEXT,
                              Str_DONT_CHANGE,false);

   /* Get parameter that indicates if the message is a reply to a previous message */
   Par_GetParToText ("IsReply",YN,1);
   if ((IsReply = (Str_ConvertToUpperLetter (YN[0]) == 'Y')))
      /* Get original message code */
      if ((OriginalMsgCod = Msg_GetParamMsgCod ()) <= 0)
         Lay_ShowErrorAndExit ("Wrong code of message.");

   /* Get user's code of possible preselected recipient */
   Usr_GetParamOtherUsrCodEncryptedAndGetListIDs ();

   /* Get lists of selected users */
   Usr_GetListsSelectedUsrsCods ();

   /* Get list of users' IDs or nicknames written explicitely */
   Error = Usr_GetListMsgRecipientsWrittenExplicitelyBySender (true);

   /***** Check number of recipients *****/
   if ((NumRecipients = Usr_CountNumUsrsInListOfSelectedUsrs ()))
     {
      if (Gbl.Usrs.Me.LoggedRole == Rol_STUDENT &&
          NumRecipients > Cfg_MAX_RECIPIENTS)
        {
         /* Write warning message */
         Lay_ShowAlert (Lay_WARNING,Txt_You_can_not_send_a_message_to_so_many_recipients_);
         Error = true;
        }
     }
   else	// No recipients selected
     {
      /* Write warning message */
      Lay_ShowAlert (Lay_WARNING,Txt_You_must_select_one_ore_more_recipients);
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

   /***** Allocate space to store a list of recipients with the following format:
	  "FirstName Surname1 Surname2; FirstName Surname1 Surname2; FirstName Surname1 Surname2" *****/
   if ((ListUsrsDst = (char *) malloc (((Usr_MAX_BYTES_NAME+1)*3+1)*NumRecipients)) == NULL)
      Lay_ShowErrorAndExit ("Not enough memory to store email addresses of recipients.");
   ListUsrsDst[0] = '\0';

   /***** Initialize structure with user's data *****/
   Usr_UsrDataConstructor (&UsrDstData);

   /***** Initialize image *****/
   Img_ImageConstructor (&Image);

   /***** Get attached image (action, file and title) *****/
   Image.Width   = Msg_IMAGE_SAVED_MAX_WIDTH;
   Image.Height  = Msg_IMAGE_SAVED_MAX_HEIGHT;
   Image.Quality = Msg_IMAGE_SAVED_QUALITY;
   Img_GetImageFromForm (-1,&Image,NULL);

   /***** Loop over the list Gbl.Usrs.Select.All, that holds the list of the
	  recipients, creating a received message for each recipient *****/
   Str_ChangeFormat (Str_FROM_FORM,Str_TO_RIGOROUS_HTML,
                     Content,Cns_MAX_BYTES_LONG_TEXT,false);
   Ptr = Gbl.Usrs.Select.All;
   NumRecipients = 0;

   while (*Ptr)
     {
      Par_GetNextStrUntilSeparParamMult (&Ptr,UsrDstData.EncryptedUsrCod,Cry_LENGTH_ENCRYPTED_STR_SHA256_BASE64);
      Usr_GetUsrCodFromEncryptedUsrCod (&UsrDstData);
      if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDstData))		// Get recipient's data from the database
        {
         /***** Check if recipient has banned me *****/
         RecipientHasBannedMe = Msg_CheckIfUsrIsBanned (Gbl.Usrs.Me.UsrDat.UsrCod,UsrDstData.UsrCod);

         if (RecipientHasBannedMe)
           {
            /***** Show an alert indicating that the message has not been sent successfully *****/
            sprintf (Gbl.Message,Txt_message_not_sent_to_X,UsrDstData.FullName);
            Lay_ShowAlert (Lay_WARNING,Gbl.Message);
           }
         else
           {
            /***** Create message *****/
            if (!MsgAlreadyInserted)
              {
               // The message is inserted only once in the table of messages sent
               NewMsgCod = Msg_InsertNewMsg (Gbl.Msg.Subject,Content,&Image);
               MsgAlreadyInserted = true;
              }

            /***** If this recipient is the original sender of a message been replied, set Replied to true *****/
            Replied = (IsReply &&
        	       UsrDstData.UsrCod == Gbl.Usrs.Other.UsrDat.UsrCod);

            /***** This received message must be notified by email? *****/
            CreateNotif = (UsrDstData.Prefs.NotifNtfEvents & (1 << Ntf_EVENT_MESSAGE));
            NotifyByEmail = CreateNotif &&
        	            (UsrDstData.UsrCod != Gbl.Usrs.Me.UsrDat.UsrCod) &&
                            (UsrDstData.Prefs.EmailNtfEvents & (1 << Ntf_EVENT_MESSAGE));

            /***** Create the received message for this recipient
                   and ncrement number of new messages received by this recipient *****/
            Msg_InsertReceivedMsgIntoDB (NewMsgCod,UsrDstData.UsrCod,NotifyByEmail);

            /***** Create notification for this recipient.
                   If this recipient wants to receive notifications by -mail,
                   activate the sending of a notification *****/
            if (CreateNotif)
               Ntf_StoreNotifyEventToOneUser (Ntf_EVENT_MESSAGE,&UsrDstData,NewMsgCod,
                                              (Ntf_Status_t) (NotifyByEmail ? Ntf_STATUS_BIT_EMAIL :
                                        	                              0));

            /***** Show an alert indicating that the message has been sent successfully *****/
            sprintf (Gbl.Message,NotifyByEmail ? Txt_message_sent_to_X_notified_by_email :
                                                 Txt_message_sent_to_X_not_notified_by_email,
                     UsrDstData.FullName);
            Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);

            /***** Increment number of recipients *****/
            if (NotifyByEmail)
               NumRecipientsToBeNotifiedByEMail++;
            NumRecipients++;
           }
        }
      else
        {
         Lay_ShowAlert (Lay_ERROR,Txt_Error_getting_data_from_a_recipient);
         NumErrors++;
        }
     }

   /***** Free image *****/
   Img_ImageDestructor (&Image);

   /***** Free memory used for user's data *****/
   Usr_UsrDataDestructor (&UsrDstData);

   /***** Free memory *****/
   /* Free memory used for list of recipients' names */
   free (ListUsrsDst);

   /* Free memory used for list of users */
   Usr_FreeListOtherRecipients ();
   Usr_FreeListsSelectedUsrsCods ();

   /***** Update received message setting Replied field to true *****/
   if (Replied)
      Msg_SetReceivedMsgAsReplied (OriginalMsgCod);

   /***** Write final message *****/
   if (NumRecipients)
     {
      if (NumRecipients == 1)
         Lay_ShowAlert (Lay_SUCCESS,Txt_The_message_has_been_sent_to_1_recipient);
      else
        {
         sprintf (Gbl.Message,Txt_The_message_has_been_sent_to_X_recipients,
                  (unsigned) NumRecipients);
         Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);
        }

      /***** Show message about number of users to be notified *****/
      Ntf_ShowAlertNumUsrsToBeNotifiedByEMail (NumRecipientsToBeNotifiedByEMail);
      Lay_ShowAlert (Lay_INFO,Txt_Do_not_reload_this_page_because_the_message_will_be_sent_again_);
     }
   else
      Lay_ShowAlert (Lay_WARNING,Txt_The_message_has_not_been_sent_to_any_recipient);

   /***** Show alert about errors on sending message *****/
   if (NumErrors > 1)
     {
      sprintf (Gbl.Message,Txt_There_have_been_X_errors_in_sending_the_message,
               (unsigned) NumErrors);
      Lay_ShowAlert (Lay_ERROR,Gbl.Message);
     }
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

   /***** Request confirmation to remove messages *****/
   if (Gbl.Msg.FilterContent[0])
     {
      if (Gbl.Msg.ShowOnlyUnreadMsgs)
         sprintf (Gbl.Message,Txt_Do_you_really_want_to_delete_the_unread_messages_received_from_USER_X_from_COURSE_Y_related_to_CONTENT_Z,
                  Gbl.Msg.FilterFromTo[0] ? Gbl.Msg.FilterFromTo :
                	                    Txt_any_user,
                  Gbl.Msg.FilterCrsShrtName,Gbl.Msg.FilterContent);
      else
         sprintf (Gbl.Message,Txt_Do_you_really_want_to_delete_all_messages_received_from_USER_X_from_COURSE_Y_related_to_CONTENT_Z,
                  Gbl.Msg.FilterFromTo[0] ? Gbl.Msg.FilterFromTo :
                	                    Txt_any_user,
                  Gbl.Msg.FilterCrsShrtName,Gbl.Msg.FilterContent);
     }
   else
     {
      if (Gbl.Msg.ShowOnlyUnreadMsgs)
         sprintf (Gbl.Message,Txt_Do_you_really_want_to_delete_the_unread_messages_received_from_USER_X_from_COURSE_Y,
                  Gbl.Msg.FilterFromTo[0] ? Gbl.Msg.FilterFromTo :
                	                    Txt_any_user,
                  Gbl.Msg.FilterCrsShrtName);
      else
         sprintf (Gbl.Message,Txt_Do_you_really_want_to_delete_all_messages_received_from_USER_X_from_COURSE_Y,
                  Gbl.Msg.FilterFromTo[0] ? Gbl.Msg.FilterFromTo :
                	                    Txt_any_user,
                  Gbl.Msg.FilterCrsShrtName);
     }
   Lay_ShowAlert (Lay_WARNING,Gbl.Message);

   /***** Form to remove received messages *****/
   Act_FormStart (ActDelAllRcvMsg);
   Msg_PutHiddenParamsMsgsFilters ();
   Lay_PutRemoveButton (Txt_Delete_messages_received);
   Act_FormEnd ();

   /***** Show messages again *****/
   Msg_ShowRecMsgs ();
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

   /***** Request confirmation to remove messages *****/
   if (Gbl.Msg.FilterContent[0])
      sprintf (Gbl.Message,Txt_Do_you_really_want_to_delete_all_messages_sent_to_USER_X_from_COURSE_Y_related_to_CONTENT_Z,
	       Gbl.Msg.FilterFromTo[0] ? Gbl.Msg.FilterFromTo :
					 Txt_any_user,
               Gbl.Msg.FilterCrsShrtName,Gbl.Msg.FilterContent);
   else
      sprintf (Gbl.Message,Txt_Do_you_really_want_to_delete_all_messages_sent_to_USER_X_from_COURSE_Y,
	       Gbl.Msg.FilterFromTo[0] ? Gbl.Msg.FilterFromTo :
					 Txt_any_user,
               Gbl.Msg.FilterCrsShrtName);
   Lay_ShowAlert (Lay_WARNING,Gbl.Message);

   /***** Form to remove sent messages *****/
   Act_FormStart (ActDelAllSntMsg);
   Msg_PutHiddenParamsMsgsFilters ();
   Lay_PutRemoveButton (Txt_Delete_messages_sent);
   Act_FormEnd ();

   /***** Show messages again *****/
   Msg_ShowSntMsgs ();
  }

/*****************************************************************************/
/*********************** Delete all received messages ************************/
/*****************************************************************************/

void Msg_DelAllRecMsgs (void)
  {
   char FilterFromToSubquery[Msg_MAX_LENGTH_MESSAGES_QUERY+1];
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
   char FilterFromToSubquery[Msg_MAX_LENGTH_MESSAGES_QUERY+1];
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
      Lay_ShowAlert (Lay_SUCCESS,Txt_One_message_has_been_deleted);
   else
     {
      sprintf (Gbl.Message,Txt_X_messages_have_been_deleted,
               NumMsgs);
      Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);
     }
  }

/*****************************************************************************/
/**************** Get parameter with course origin of messages ***************/
/*****************************************************************************/

void Msg_GetParamMsgsCrsCod (void)
  {
   extern const char *Txt_any_course;
   char LongStr[1+10+1];
   struct Course Crs;

   Par_GetParToText ("FilterCrsCod",LongStr,1+10);
   Gbl.Msg.FilterCrsCod = Str_ConvertStrCodToLongCod (LongStr);
   if (Gbl.Msg.FilterCrsCod >= 0)	// If origin course specified
     {
      /* Get data of course */
      Crs.CrsCod = Gbl.Msg.FilterCrsCod;
      Crs_GetDataOfCourseByCod (&Crs);

      strcpy (Gbl.Msg.FilterCrsShrtName,Crs.ShrtName);
     }
   else
      strcpy (Gbl.Msg.FilterCrsShrtName,Txt_any_course);
  }

/*****************************************************************************/
/******************* Get parameter with "from"/"to" filter ********************/
/*****************************************************************************/

void Msg_GetParamFilterFromTo (void)
  {
   /***** Get "from"/"to" filter *****/
   Par_GetParToText ("FilterFromTo",Gbl.Msg.FilterFromTo,Usr_MAX_LENGTH_USR_NAME_OR_SURNAME*3);
  }

/*****************************************************************************/
/********************* Get parameter with content filter *********************/
/*****************************************************************************/

void Msg_GetParamFilterContent (void)
  {
   /***** Get content filter *****/
   Par_GetParToText ("FilterContent",Gbl.Msg.FilterContent,Msg_MAX_LENGTH_FILTER_CONTENT);
  }

/*****************************************************************************/
/************************* Make "from"/"to" subquery *************************/
/*****************************************************************************/

static void Msg_MakeFilterFromToSubquery (char *FilterFromToSubquery)
  {
   const char *Ptr;
   char SearchWord[Usr_MAX_LENGTH_USR_NAME_OR_SURNAME+1];

   /***** Split "from"/"to" string into words *****/
   if (Gbl.Msg.FilterFromTo[0])
     {
      Ptr = Gbl.Msg.FilterFromTo;
      strcpy (FilterFromToSubquery," AND CONCAT(usr_data.FirstName,usr_data.Surname1,usr_data.Surname2) LIKE '");
      while (*Ptr)
        {
         Str_GetNextStringUntilSpace (&Ptr,SearchWord,Usr_MAX_LENGTH_USR_NAME_OR_SURNAME);
         if (strlen (FilterFromToSubquery) + strlen (SearchWord) + 512 > Msg_MAX_LENGTH_MESSAGES_QUERY)	// Prevent string overflow
            break;
         strcat (FilterFromToSubquery,"%");
         strcat (FilterFromToSubquery,SearchWord);
        }
      strcat (FilterFromToSubquery,"%'");
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
   Lay_ShowAlert (Lay_SUCCESS,Txt_Message_deleted);

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
   Lay_ShowAlert (Lay_SUCCESS,Txt_Message_deleted);

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
   char Query[256];

   /***** Expand message in sent message table *****/
   sprintf (Query,"UPDATE msg_snt SET Expanded='Y'"
                  " WHERE MsgCod='%ld' AND UsrCod='%ld'",
            MsgCod,Gbl.Usrs.Me.UsrDat.UsrCod);
   DB_QueryUPDATE (Query,"can not expand a sent message");

   /***** Contract all my other messages in sent message table *****/
   sprintf (Query,"UPDATE msg_snt SET Expanded='N'"
                  " WHERE UsrCod='%ld' AND MsgCod<>'%ld'",
            Gbl.Usrs.Me.UsrDat.UsrCod,MsgCod);
   DB_QueryUPDATE (Query,"can not contract a sent message");
  }

/*****************************************************************************/
/************************* Expand a received message *************************/
/*****************************************************************************/

static void Msg_ExpandReceivedMsg (long MsgCod)
  {
   char Query[256];

   /***** Expand message in received message table and mark it as read by me *****/
   sprintf (Query,"UPDATE msg_rcv SET Open='Y',Expanded='Y'"
                  " WHERE MsgCod='%ld' AND UsrCod='%ld'",
            MsgCod,Gbl.Usrs.Me.UsrDat.UsrCod);
   DB_QueryUPDATE (Query,"can not expand a received message");

   /***** Contract all my other messages in received message table *****/
   sprintf (Query,"UPDATE msg_rcv SET Expanded='N'"
                  " WHERE UsrCod='%ld' AND MsgCod<>'%ld'",
            Gbl.Usrs.Me.UsrDat.UsrCod,MsgCod);
   DB_QueryUPDATE (Query,"can not contract a received message");
  }

/*****************************************************************************/
/************************** Contract a sent message **************************/
/*****************************************************************************/

static void Msg_ContractSentMsg (long MsgCod)
  {
   char Query[256];

   /***** Contract message in sent message table *****/
   sprintf (Query,"UPDATE msg_snt SET Expanded='N'"
                  " WHERE MsgCod='%ld' AND UsrCod='%ld'",
            MsgCod,Gbl.Usrs.Me.UsrDat.UsrCod);
   DB_QueryUPDATE (Query,"can not contract a sent message");
  }

/*****************************************************************************/
/************************ Contract a received message ************************/
/*****************************************************************************/

static void Msg_ContractReceivedMsg (long MsgCod)
  {
   char Query[256];

   /***** Contract message in received message table *****/
   sprintf (Query,"UPDATE msg_rcv SET Expanded='N'"
                  " WHERE MsgCod='%ld' AND UsrCod='%ld'",
            MsgCod,Gbl.Usrs.Me.UsrDat.UsrCod);
   DB_QueryUPDATE (Query,"can not contract a received message");
  }

/*****************************************************************************/
/********************** Mark a received message as open **********************/
/*****************************************************************************/

void Msg_SetReceivedMsgAsOpen (long MsgCod,long UsrCod)
  {
   char Query[512];

   /***** Mark message as read by user *****/
   sprintf (Query,"UPDATE msg_rcv SET Open='Y'"
                  " WHERE MsgCod='%ld' AND UsrCod='%ld'",
            MsgCod,UsrCod);
   DB_QueryUPDATE (Query,"can not mark a received message as open");
  }

/*****************************************************************************/
/********************** Insert a message in the database *********************/
/*****************************************************************************/
// Return the code of the new inserted message

static long Msg_InsertNewMsg (const char *Subject,const char *Content,
                              struct Image *Image)
  {
   char *Query;
   long MsgCod;

   /***** Allocate space for query *****/
   if ((Query = malloc (512 +
                        strlen (Subject) +
			strlen (Content) +
			Cry_LENGTH_ENCRYPTED_STR_SHA256_BASE64+
			Img_MAX_BYTES_TITLE)) == NULL)
      Lay_ShowErrorAndExit ("Not enough memory to store database query.");

   /***** Check if image is received and processed *****/
   if (Image->Action == Img_ACTION_NEW_IMAGE &&	// Upload new image
       Image->Status == Img_FILE_PROCESSED)	// The new image received has been processed
      /* Move processed image to definitive directory */
      Img_MoveImageToDefinitiveDirectory (Image);

   /***** Insert message subject and content in the database *****/
   sprintf (Query,"INSERT INTO msg_content"
	          " (Subject,Content,ImageName,ImageTitle,ImageURL)"
                  " VALUES ('%s','%s','%s','%s','%s')",
            Subject,Content,
            Image->Name,
            Image->Title ? Image->Title : "",
            Image->URL   ? Image->URL   : "");
   MsgCod = DB_QueryINSERTandReturnCode (Query,"can not create message");

   /***** Insert message in sent messages *****/
   sprintf (Query,"INSERT INTO msg_snt (MsgCod,CrsCod,UsrCod,Expanded,CreatTime)"
                  " VALUES ('%ld','%ld','%ld','N',NOW())",
            MsgCod,
            Gbl.CurrentCrs.Crs.CrsCod,
            Gbl.Usrs.Me.UsrDat.UsrCod);
   DB_QueryINSERT (Query,"can not create message");

   /***** Free space used for query *****/
   free ((void *) Query);

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
   char Query[Msg_MAX_LENGTH_MESSAGES_QUERY+1];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long MsgNum,NumMsgs;
   long MsgCod;

   /***** Get some of the messages received or sent by this user from database *****/
   Msg_ConstructQueryToSelectSentOrReceivedMsgs (Query,UsrCod,FilterCrsCod,FilterFromToSubquery);
   NumMsgs = DB_QuerySELECT (Query,&mysql_res,"can not get list of messages");

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
   char Query[512];

   /***** Move messages from msg_rcv to msg_rcv_deleted *****/
   /* Insert messages into msg_rcv_deleted */
   sprintf (Query,"INSERT IGNORE INTO msg_rcv_deleted (MsgCod,UsrCod,Notified,Open,Replied)"
                  " SELECT MsgCod,UsrCod,Notified,Open,Replied FROM msg_rcv WHERE UsrCod='%ld'",
            UsrCod);
   DB_QueryINSERT (Query,"can not remove received messages");

   /* Delete messages from msg_rcv *****/
   sprintf (Query,"DELETE FROM msg_rcv WHERE UsrCod='%ld'",UsrCod);
   DB_QueryDELETE (Query,"can not remove received messages");

   /***** Move message from msg_snt to msg_snt_deleted *****/
   /* Insert message into msg_snt_deleted */
   sprintf (Query,"INSERT IGNORE INTO msg_snt_deleted (MsgCod,CrsCod,UsrCod,CreatTime)"
                  " SELECT MsgCod,CrsCod,UsrCod,CreatTime FROM msg_snt WHERE UsrCod='%ld'",
            UsrCod);
   DB_QueryINSERT (Query,"can not remove sent messages");

   /* Delete message from msg_snt *****/
   sprintf (Query,"DELETE FROM msg_snt WHERE UsrCod='%ld'",UsrCod);
   DB_QueryDELETE (Query,"can not remove sent messages");
  }

/*****************************************************************************/
/**** Insert a message y su destinatario in the table of messages received ***/
/*****************************************************************************/

static void Msg_InsertReceivedMsgIntoDB (long MsgCod,long UsrCod,bool NotifyByEmail)
  {
   char Query[512];

   /***** Insert message received in the database *****/
   sprintf (Query,"INSERT INTO msg_rcv"
	          " (MsgCod,UsrCod,Notified,Open,Replied,Expanded)"
                  " VALUES ('%ld','%ld','%c','N','N','N')",
            MsgCod,UsrCod,
            NotifyByEmail ? 'Y' :
        	            'N');
   DB_QueryINSERT (Query,"can not create received message");
  }

/*****************************************************************************/
/******** Update received message by setting Replied field to true ***********/
/*****************************************************************************/

static void Msg_SetReceivedMsgAsReplied (long MsgCod)
  {
   char Query[512];

   /***** Update received message by setting Replied field to true *****/
   sprintf (Query,"UPDATE msg_rcv SET Replied='Y' WHERE MsgCod='%ld' AND UsrCod='%ld'",
            MsgCod,Gbl.Usrs.Me.UsrDat.UsrCod);
   DB_QueryUPDATE (Query,"can not update a received message");
  }

/*****************************************************************************/
/************ Delete a message from the received message table ***************/
/*****************************************************************************/

static void Msg_MoveReceivedMsgToDeleted (long MsgCod,long UsrCod)
  {
   char Query[512];

   /***** Move message from msg_rcv to msg_rcv_deleted *****/
   /* Insert message into msg_rcv_deleted */
   sprintf (Query,"INSERT IGNORE INTO msg_rcv_deleted (MsgCod,UsrCod,Notified,Open,Replied)"
                  " SELECT MsgCod,UsrCod,Notified,Open,Replied"
                  " FROM msg_rcv WHERE MsgCod='%ld' AND UsrCod='%ld'",
            MsgCod,UsrCod);
   DB_QueryINSERT (Query,"can not remove a received message");

   /* Delete message from msg_rcv *****/
   sprintf (Query,"DELETE FROM msg_rcv WHERE MsgCod='%ld' AND UsrCod='%ld'",
            MsgCod,UsrCod);
   DB_QueryDELETE (Query,"can not remove a received message");

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
   char Query[512];

   /***** Move message from msg_snt to msg_snt_deleted *****/
   /* Insert message into msg_snt_deleted */
   sprintf (Query,"INSERT IGNORE INTO msg_snt_deleted (MsgCod,CrsCod,UsrCod,CreatTime)"
                  " SELECT MsgCod,CrsCod,UsrCod,CreatTime"
                  " FROM msg_snt WHERE MsgCod='%ld'",
            MsgCod);
   DB_QueryINSERT (Query,"can not remove a sent message");

   /* Delete message from msg_snt *****/
   sprintf (Query,"DELETE FROM msg_snt WHERE MsgCod='%ld'",MsgCod);
   DB_QueryDELETE (Query,"can not remove a sent message");

   /***** If message content is not longer necessary, move it to msg_content_deleted *****/
   if (Msg_CheckIfReceivedMsgIsDeletedForAllItsRecipients (MsgCod))
      Msg_MoveMsgContentToDeleted (MsgCod);
  }

/*****************************************************************************/
/*************** Delete the subject and content of a message *****************/
/*****************************************************************************/

static void Msg_MoveMsgContentToDeleted (long MsgCod)
  {
   char Query[512];

   /***** Move message from msg_content to msg_content_deleted *****/
   /* Insert message content into msg_content_deleted */
   sprintf (Query,"INSERT IGNORE INTO msg_content_deleted"
	          " (MsgCod,Subject,Content,ImageName,ImageTitle,ImageURL)"
                  " SELECT MsgCod,Subject,Content,ImageName,ImageTitle,ImageURL"
                  " FROM msg_content WHERE MsgCod='%ld'",
            MsgCod);
   DB_QueryINSERT (Query,"can not remove the content of a message");

   /* TODO: Messages in msg_content_deleted older than a certain time
      should be deleted to ensure the protection of personal data */

   /* Delete message from msg_content *****/
   sprintf (Query,"DELETE FROM msg_content WHERE MsgCod='%ld'",MsgCod);
   DB_QueryUPDATE (Query,"can not remove the content of a message");
  }

/*****************************************************************************/
/***** Delete the subject and content of all completely deleted messages *****/
/*****************************************************************************/

void Msg_MoveUnusedMsgsContentToDeleted (void)
  {
   char Query[512];

   /***** Move messages from msg_content to msg_content_deleted *****/
   /* Insert message content into msg_content_deleted */
   sprintf (Query,"INSERT IGNORE INTO msg_content_deleted (MsgCod,Subject,Content)"
                  " SELECT MsgCod,Subject,Content FROM msg_content"
                  " WHERE MsgCod NOT IN (SELECT MsgCod FROM msg_snt)"
                  " AND MsgCod NOT IN (SELECT DISTINCT MsgCod FROM msg_rcv)");
   DB_QueryINSERT (Query,"can not remove the content of some messages");

   /* Messages in msg_content_deleted older than a certain time
      should be deleted to ensure the protection of personal data */

   /* Delete message from msg_content *****/
   sprintf (Query,"DELETE FROM msg_content"
                  " WHERE MsgCod NOT IN (SELECT MsgCod FROM msg_snt)"
                  " AND MsgCod NOT IN (SELECT DISTINCT MsgCod FROM msg_rcv)");
   DB_QueryUPDATE (Query,"can not remove the content of some messages");
  }

/*****************************************************************************/
/******************** Check if a sent message is deleted *********************/
/*****************************************************************************/

static bool Msg_CheckIfSentMsgIsDeleted (long MsgCod)
  {
   char Query[512];

   /***** Get if the message code is in table of sent messages not deleted *****/
   sprintf (Query,"SELECT COUNT(*) FROM msg_snt"
                  " WHERE MsgCod='%ld'",MsgCod);
   return (DB_QueryCOUNT (Query,"can not check if a sent message is deleted") == 0);	// The message has been deleted by its author when it is not present in table of sent messages undeleted
  }

/*****************************************************************************/
/***** Check if a received message has been deleted by all its recipients ****/
/*****************************************************************************/

static bool Msg_CheckIfReceivedMsgIsDeletedForAllItsRecipients (long MsgCod)
  {
   char Query[512];

   /***** Get if the message code is in table of received messages not deleted *****/
   sprintf (Query,"SELECT COUNT(*) FROM msg_rcv"
                  " WHERE MsgCod='%ld'",MsgCod);
   return (DB_QueryCOUNT (Query,"can not check if a received message is deleted by all recipients") == 0);	// The message has been deleted by all its recipients when it is not present in table of received messages undeleted
  }

/*****************************************************************************/
/******** Get number of received messages that haven't been read by me *******/
/*****************************************************************************/

static unsigned Msg_GetNumUnreadMsgs (long FilterCrsCod,const char *FilterFromToSubquery)
  {
   char SubQuery[Msg_MAX_LENGTH_MESSAGES_QUERY+1];
   char Query[Msg_MAX_LENGTH_MESSAGES_QUERY+1];

   /***** Get number of unread messages from database *****/
   if (FilterCrsCod >= 0)	// If origin course selected
     {
      if (FilterFromToSubquery[0])
         sprintf (SubQuery,"SELECT msg_rcv.MsgCod FROM msg_rcv,msg_snt,usr_data"
                           " WHERE msg_rcv.UsrCod='%ld' AND msg_rcv.Open='N'"
                           " AND msg_rcv.MsgCod=msg_snt.MsgCod"
                           " AND msg_snt.CrsCod='%ld'"
                           " AND msg_snt.UsrCod=usr_data.UsrCod%s",
                  Gbl.Usrs.Me.UsrDat.UsrCod,
                  FilterCrsCod,
                  FilterFromToSubquery);
      else
         sprintf (SubQuery,"SELECT msg_rcv.MsgCod FROM msg_rcv,msg_snt"
                           " WHERE msg_rcv.UsrCod='%ld' AND msg_rcv.Open='N'"
                           " AND msg_rcv.MsgCod=msg_snt.MsgCod"
                           " AND msg_snt.CrsCod='%ld'",
                  Gbl.Usrs.Me.UsrDat.UsrCod,
                  FilterCrsCod);
     }
   else	// If no origin course selected
     {
      if (FilterFromToSubquery[0])
         sprintf (SubQuery,"SELECT msg_rcv.MsgCod FROM msg_rcv,msg_snt,usr_data"
                           " WHERE msg_rcv.UsrCod='%ld' AND msg_rcv.Open='N'"
                           " AND msg_rcv.MsgCod=msg_snt.MsgCod"
                           " AND msg_snt.UsrCod=usr_data.UsrCod%s",
                  Gbl.Usrs.Me.UsrDat.UsrCod,
                  FilterFromToSubquery);
      else
         sprintf (SubQuery,"SELECT MsgCod FROM msg_rcv"
                           " WHERE UsrCod='%ld' AND Open='N'",
                  Gbl.Usrs.Me.UsrDat.UsrCod);
     }

   if (Gbl.Msg.FilterContent[0])
      sprintf (Query,"SELECT COUNT(*) FROM msg_content"
                     " WHERE MsgCod IN (%s)"
                     " AND MATCH (Subject,Content) AGAINST ('%s')",
               SubQuery,
               Gbl.Msg.FilterContent);
   else
      sprintf (Query,"SELECT COUNT(*) FROM (%s) AS T",
               SubQuery);
   return (unsigned) DB_QueryCOUNT (Query,"can not get number of unread messages");
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
   /***** Link to view banned users *****/
   if (Msg_GetNumUsrsBannedByMe ())
     {
      fprintf (Gbl.F.Out,"<div class=\"CONTEXT_MENU\">");
      Msg_PutLinkToViewBannedUsers ();
      fprintf (Gbl.F.Out,"</div>");
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
   extern const char *The_ClassFormBold[The_NUM_THEMES];
   extern const char *Txt_Filter;
   extern const char *Txt_Update_messages;
   char FilterFromToSubquery[Msg_MAX_LENGTH_MESSAGES_QUERY+1];
   char Query[Msg_MAX_LENGTH_MESSAGES_QUERY+1];
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
      ActSeeSntMsg
     };
   static const Pag_WhatPaginate_t WhatPaginate[Msg_NUM_TYPES_OF_MSGS] =
     {
      Pag_MESSAGES_RECEIVED,
      Pag_MESSAGES_SENT
     };
   const char *Help[Msg_NUM_TYPES_OF_MSGS] =
     {
      Hlp_MESSAGES_Received,
      Hlp_MESSAGES_Sent
     };
   const char *HelpFilter[Msg_NUM_TYPES_OF_MSGS] =
     {
      Hlp_MESSAGES_Received_filter,
      Hlp_MESSAGES_Sent_filter
     };

   /***** Get the page number *****/
   Pag_GetParamPagNum (WhatPaginate[Gbl.Msg.TypeOfMessages]);

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
   Msg_ConstructQueryToSelectSentOrReceivedMsgs (Query,Gbl.Usrs.Me.UsrDat.UsrCod,Gbl.Msg.FilterCrsCod,FilterFromToSubquery);
   NumRows = DB_QuerySELECT (Query,&mysql_res,"can not get messages");

   Gbl.Msg.NumMsgs = (unsigned) NumRows;

   /***** Start frame with messages *****/
   Lay_StartRoundFrame ("97%",
                        Msg_WriteNumMsgs (NumUnreadMsgs),
                        Msg_PutIconsListMsgs,Help[Gbl.Msg.TypeOfMessages]);

   /***** Filter messages *****/
   /* Start frame with filter */
   Lay_StartRoundFrame (NULL,Txt_Filter,
                        NULL,HelpFilter[Gbl.Msg.TypeOfMessages]);

   /* Form to see messages again */
   Act_FormStart (ActionSee[Gbl.Msg.TypeOfMessages]);

   fprintf (Gbl.F.Out,"<div class=\"CENTER_MIDDLE\">");
   Msg_ShowFormSelectCourseSentOrRecMsgs ();
   if (Gbl.Msg.TypeOfMessages == Msg_MESSAGES_RECEIVED)
      Msg_ShowFormToShowOnlyUnreadMessages ();
   fprintf (Gbl.F.Out,"</div>");
   Msg_ShowFormToFilterMsgs ();

   fprintf (Gbl.F.Out,"<div class=\"CONTEXT_MENU\">");
   Act_LinkFormSubmitAnimated (Txt_Update_messages,
                               The_ClassFormBold[Gbl.Prefs.Theme],
                               NULL);
   Lay_PutCalculateIconWithText (Txt_Update_messages,Txt_Update_messages);
   fprintf (Gbl.F.Out,"</div>");

   Act_FormEnd ();

   /* End frame */
   Lay_EndRoundFrame ();


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
               Gbl.Pag.CurrentPage = (unsigned) (NumRow / Pag_ITEMS_PER_PAGE) + 1;
               break;
              }
           }
        }

      /***** Compute variables related to pagination *****/
      Pagination.NumItems = Gbl.Msg.NumMsgs;
      Pagination.CurrentPage = (int) Gbl.Pag.CurrentPage;
      Pag_CalculatePagination (&Pagination);
      Gbl.Pag.CurrentPage = (unsigned) Pagination.CurrentPage;

      /***** Save my current page in order to show it next time I'll view my received/sent messages *****/
      Pag_SaveLastPageMsgIntoSession (WhatPaginate[Gbl.Msg.TypeOfMessages],
	                              Gbl.Pag.CurrentPage);

      /***** Write links to pages *****/
      if (Pagination.MoreThanOnePage)
         Pag_WriteLinksToPagesCentered (WhatPaginate[Gbl.Msg.TypeOfMessages],
                                        0,&Pagination);

      /***** Show received / sent messages in this page *****/
      fprintf (Gbl.F.Out,"<table class=\"CELLS_PAD_2\""
	                 " style=\"width:100%%;\">");

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

      fprintf (Gbl.F.Out,"</table>");

      /***** Write again links to pages *****/
      if (Pagination.MoreThanOnePage)
         Pag_WriteLinksToPagesCentered (WhatPaginate[Gbl.Msg.TypeOfMessages],
                                        0,&Pagination);
     }

   /***** End frame *****/
   Lay_EndRoundFrame ();

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/********************* Get number of user I have banned **********************/
/*****************************************************************************/

static unsigned long Msg_GetNumUsrsBannedByMe (void)
  {
   char Query[128];

   /***** Get number of users I have banned *****/
   sprintf (Query,"SELECT COUNT(*) FROM msg_banned WHERE ToUsrCod='%ld'",
            Gbl.Usrs.Me.UsrDat.UsrCod);
   return DB_QueryCOUNT (Query,"can not get number of users you have banned");
  }

/*****************************************************************************/
/****************** Put a link (form) to view banned users *******************/
/*****************************************************************************/

static void Msg_PutLinkToViewBannedUsers(void)
  {
   extern const char *Txt_Banned_users;

   Lay_PutContextualLink (ActLstBanUsr,NULL,
                          "stop16x16.gif",
                          Txt_Banned_users,Txt_Banned_users,
                          NULL);
  }

/*****************************************************************************/
/********* Generate a query to select messages received or sent **************/
/*****************************************************************************/

static void Msg_ConstructQueryToSelectSentOrReceivedMsgs (char *Query,long UsrCod,
                                                          long FilterCrsCod,const char *FilterFromToSubquery)
  {
   char SubQuery[Msg_MAX_LENGTH_MESSAGES_QUERY+1];
   char *PtrQuery;
   const char *StrUnreadMsg;

   PtrQuery = Gbl.Msg.FilterContent[0] ? SubQuery :
	                                 Query;

   if (FilterCrsCod > 0)	// If origin course selected
      switch (Gbl.Msg.TypeOfMessages)
        {
         case Msg_MESSAGES_RECEIVED:
            StrUnreadMsg = (Gbl.Msg.ShowOnlyUnreadMsgs ? " AND msg_rcv.Open='N'" :
        	                                         "");
            if (FilterFromToSubquery[0])
               sprintf (PtrQuery,"SELECT msg_rcv.MsgCod"
        	                 " FROM msg_rcv,msg_snt,usr_data"
                                 " WHERE msg_rcv.UsrCod='%ld'%s"
                                 " AND msg_rcv.MsgCod=msg_snt.MsgCod"
                                 " AND msg_snt.CrsCod='%ld'"
                                 " AND msg_snt.UsrCod=usr_data.UsrCod%s"
                                 " ORDER BY msg_rcv.MsgCod DESC",
                        UsrCod,StrUnreadMsg,FilterCrsCod,
                        FilterFromToSubquery);
            else
               sprintf (PtrQuery,"SELECT msg_rcv.MsgCod"
        	                 " FROM msg_rcv,msg_snt"
                                 " WHERE msg_rcv.UsrCod='%ld'%s"
                                 " AND msg_rcv.MsgCod=msg_snt.MsgCod"
                                 " AND msg_snt.CrsCod='%ld'"
                                 " ORDER BY msg_rcv.MsgCod DESC",
                        UsrCod,StrUnreadMsg,
                        FilterCrsCod);
            break;
         case Msg_MESSAGES_SENT:
            if (FilterFromToSubquery[0])
               sprintf (PtrQuery,"SELECT DISTINCT msg_snt.MsgCod"
        	                 " FROM msg_snt,msg_rcv,usr_data"
                                 " WHERE msg_snt.UsrCod='%ld'"
                                 " AND msg_snt.CrsCod='%ld'"
                                 " AND msg_snt.MsgCod=msg_rcv.MsgCod"
                                 " AND msg_rcv.UsrCod=usr_data.UsrCod%s"
                                 " ORDER BY msg_snt.MsgCod DESC",
                        UsrCod,FilterCrsCod,
                        FilterFromToSubquery);
            else
               sprintf (PtrQuery,"SELECT MsgCod"
        	                 " FROM msg_snt"
                                 " WHERE UsrCod='%ld' AND CrsCod='%ld'"
                                 " ORDER BY MsgCod DESC",
                        UsrCod,FilterCrsCod);
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
               sprintf (PtrQuery,"SELECT msg_rcv.MsgCod"
        	                 " FROM msg_rcv,msg_snt,usr_data"
                                 " WHERE msg_rcv.UsrCod='%ld'%s"
                                 " AND msg_rcv.MsgCod=msg_snt.MsgCod"
                                 " AND msg_snt.UsrCod=usr_data.UsrCod%s"
                                 " ORDER BY msg_rcv.MsgCod DESC",
                        UsrCod,StrUnreadMsg,
                        FilterFromToSubquery);
              }
            else
              {
               StrUnreadMsg = (Gbl.Msg.ShowOnlyUnreadMsgs ? " AND Open='N'" :
        	                                            "");
               sprintf (PtrQuery,"SELECT MsgCod"
        	                 " FROM msg_rcv"
                                 " WHERE UsrCod='%ld'%s"
                                 " ORDER BY MsgCod DESC",
                        UsrCod,StrUnreadMsg);
              }
            break;
         case Msg_MESSAGES_SENT:
            if (FilterFromToSubquery[0])
               sprintf (PtrQuery,"SELECT DISTINCT msg_snt.MsgCod"
        	                 " FROM msg_snt,msg_rcv,usr_data"
                                 " WHERE msg_snt.UsrCod='%ld'"
                                 " AND msg_snt.MsgCod=msg_rcv.MsgCod"
                                 " AND msg_rcv.UsrCod=usr_data.UsrCod%s"
                                 " ORDER BY msg_snt.MsgCod DESC",
                        UsrCod,
                        FilterFromToSubquery);
            else
               sprintf (PtrQuery,"SELECT MsgCod"
        	                 " FROM msg_snt"
                                 " WHERE UsrCod='%ld'"
                                 " ORDER BY MsgCod DESC",
                        UsrCod);
            break;
         default: // Not aplicable here
            break;
        }

   if (Gbl.Msg.FilterContent[0])
      sprintf (Query,"SELECT MsgCod"
	             " FROM msg_content"
                     " WHERE MsgCod IN (%s)"
                     " AND MATCH (Subject,Content) AGAINST ('%s')",
               SubQuery,
               Gbl.Msg.FilterContent);
  }

/*****************************************************************************/
/**** Get the number of unique messages sent by any teacher from a course ****/
/*****************************************************************************/

unsigned Msg_GetNumMsgsSentByTchsCrs (long CrsCod)
  {
   char Query[1024];

   /***** Get the number of unique messages sent by any teacher from this course *****/
   sprintf (Query,"SELECT COUNT(*) FROM msg_snt,crs_usr"
                  " WHERE msg_snt.CrsCod='%ld' AND crs_usr.CrsCod='%ld' AND crs_usr.Role='%u'"
                  " AND msg_snt.UsrCod=crs_usr.UsrCod",
                  CrsCod,CrsCod,(unsigned) Rol_TEACHER);
   return (unsigned) DB_QueryCOUNT (Query,"can not get the number of messages sent by teachers");
  }

/*****************************************************************************/
/************** Get the number of unique messages sent by a user *************/
/*****************************************************************************/

unsigned long Msg_GetNumMsgsSentByUsr (long UsrCod)
  {
   char Query[256];

   /***** Get the number of unique messages sent by any teacher from this course *****/
   sprintf (Query,"SELECT"
                  " (SELECT COUNT(*) FROM msg_snt WHERE UsrCod='%ld')"
                  " +"
                  " (SELECT COUNT(*) FROM msg_snt_deleted WHERE UsrCod='%ld')",
                  UsrCod,
                  UsrCod);
   return DB_QueryCOUNT (Query,"can not get the number of messages sent by a user");
  }

/*****************************************************************************/
/******** Get the number of unique messages sent from this location **********/
/******** (all the platform, current degree or current course)      **********/
/*****************************************************************************/

unsigned Msg_GetNumMsgsSent (Sco_Scope_t Scope,Msg_Status_t MsgStatus)
  {
   const char *Table = "msg_snt";
   char Query[1024];

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
      case Sco_SCOPE_SYS:
         sprintf (Query,"SELECT COUNT(*) FROM %s",
                  Table);
         break;
      case Sco_SCOPE_CTY:
         sprintf (Query,"SELECT COUNT(*)"
                        " FROM institutions,centres,degrees,courses,%s"
                        " WHERE institutions.CtyCod='%ld'"
                        " AND institutions.InsCod=centres.InsCod"
                        " AND centres.CtrCod=degrees.CtrCod"
                        " AND degrees.DegCod=courses.DegCod"
                        " AND courses.CrsCod=%s.CrsCod",
                  Table,
                  Gbl.CurrentCty.Cty.CtyCod,
                  Table);
         break;
      case Sco_SCOPE_INS:
         sprintf (Query,"SELECT COUNT(*)"
                        " FROM centres,degrees,courses,%s"
                        " WHERE centres.InsCod='%ld'"
                        " AND centres.CtrCod=degrees.CtrCod"
                        " AND degrees.DegCod=courses.DegCod"
                        " AND courses.CrsCod=%s.CrsCod",
                  Table,
                  Gbl.CurrentIns.Ins.InsCod,
                  Table);
         break;
      case Sco_SCOPE_CTR:
         sprintf (Query,"SELECT COUNT(*)"
                        " FROM degrees,courses,%s"
                        " WHERE degrees.CtrCod='%ld'"
                        " AND degrees.DegCod=courses.DegCod"
                        " AND courses.CrsCod=%s.CrsCod",
                  Table,
                  Gbl.CurrentCtr.Ctr.CtrCod,
                  Table);
         break;
      case Sco_SCOPE_DEG:
         sprintf (Query,"SELECT COUNT(*)"
                        " FROM courses,%s"
                        " WHERE courses.DegCod='%ld'"
                        " AND courses.CrsCod=%s.CrsCod",
                  Table,
                  Gbl.CurrentDeg.Deg.DegCod,
                  Table);
         break;
      case Sco_SCOPE_CRS:
         sprintf (Query,"SELECT COUNT(*)"
                        " FROM %s"
                        " WHERE CrsCod='%ld'",
                  Table,
                  Gbl.CurrentCrs.Crs.CrsCod);
         break;
      default:
	 Lay_ShowErrorAndExit ("Wrong scope.");
	 break;
     }
   return (unsigned) DB_QueryCOUNT (Query,"can not get number of sent messages");
  }

/*****************************************************************************/
/****** Get the number of unique messages received from this location ********/
/****** (all the platform, current degree or current course)          ********/
/*****************************************************************************/

unsigned Msg_GetNumMsgsReceived (Sco_Scope_t Scope,Msg_Status_t MsgStatus)
  {
   char *Table;
   char Query[1024];

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
            case Sco_SCOPE_SYS:
               sprintf (Query,"SELECT COUNT(*) FROM %s",
        	        Table);
               break;
            case Sco_SCOPE_CTY:
               sprintf (Query,"SELECT COUNT(*)"
        	              " FROM institutions,centres,degrees,courses,%s,msg_snt"
                               " WHERE institutions.CtyCod='%ld'"
                               " AND institutions.InsCod=centres.InsCod"
                               " AND centres.CtrCod=degrees.CtrCod"
                               " AND degrees.DegCod=courses.DegCod"
                               " AND courses.CrsCod=msg_snt.CrsCod"
                               " AND msg_snt.MsgCod=%s.MsgCod",
                        Table,
                        Gbl.CurrentCty.Cty.CtyCod,
                        Table);
               break;
            case Sco_SCOPE_INS:
               sprintf (Query,"SELECT COUNT(*)"
        	              " FROM centres,degrees,courses,%s,msg_snt"
                               " WHERE centres.InsCod='%ld'"
                               " AND centres.CtrCod=degrees.CtrCod"
                               " AND degrees.DegCod=courses.DegCod"
                               " AND courses.CrsCod=msg_snt.CrsCod"
                               " AND msg_snt.MsgCod=%s.MsgCod",
                        Table,
                        Gbl.CurrentIns.Ins.InsCod,
                        Table);
               break;
            case Sco_SCOPE_CTR:
               sprintf (Query,"SELECT COUNT(*)"
        	              " FROM degrees,courses,%s,msg_snt"
                               " WHERE degrees.CtrCod='%ld'"
                               " AND degrees.DegCod=courses.DegCod"
                               " AND courses.CrsCod=msg_snt.CrsCod"
                               " AND msg_snt.MsgCod=%s.MsgCod",
                        Table,
                        Gbl.CurrentCtr.Ctr.CtrCod,
                        Table);
               break;
            case Sco_SCOPE_DEG:
               sprintf (Query,"SELECT COUNT(*)"
        	              " FROM courses,%s,msg_snt"
                               " WHERE courses.DegCod='%ld'"
                               " AND courses.CrsCod=msg_snt.CrsCod"
                               " AND msg_snt.MsgCod=%s.MsgCod",
                        Table,
                        Gbl.CurrentDeg.Deg.DegCod,
                        Table);
               break;
            case Sco_SCOPE_CRS:
               sprintf (Query,"SELECT COUNT(*)"
        	              " FROM msg_snt,%s"
                              " WHERE msg_snt.CrsCod='%ld'"
                              " AND msg_snt.MsgCod=%s.MsgCod",
                        Table,
                        Gbl.CurrentCrs.Crs.CrsCod,
                        Table);
               break;
	    default:
	       Lay_ShowErrorAndExit ("Wrong scope.");
	       break;
           }
         break;
      case Msg_STATUS_NOTIFIED:
         switch (Scope)
           {
            case Sco_SCOPE_SYS:
               sprintf (Query,"SELECT "
                              "(SELECT COUNT(*)"
                              " FROM msg_rcv"
                              " WHERE Notified='Y')"
                              " + "
                              "(SELECT COUNT(*)"
                              " FROM msg_rcv_deleted"
                              " WHERE Notified='Y')");
               break;
            case Sco_SCOPE_CTY:
               sprintf (Query,"SELECT "
                              "(SELECT COUNT(*)"
                              " FROM institutions,centres,degrees,courses,msg_snt,msg_rcv"
                              " WHERE institutions.CtyCod='%ld'"
                              " AND institutions.InsCod=centres.InsCod"
                              " AND centres.CtrCod=degrees.CtrCod"
                              " AND degrees.DegCod=courses.DegCod"
                              " AND courses.CrsCod=msg_snt.CrsCod"
                              " AND msg_snt.MsgCod=msg_rcv.MsgCod"
                              " AND msg_rcv.Notified='Y')"
                              " + "
                              "(SELECT COUNT(*)"
                              " FROM institutions,centres,degrees,courses,msg_snt,msg_rcv_deleted"
                              " WHERE institutions.CtyCod='%ld'"
                              " AND institutions.InsCod=centres.InsCod"
                              " AND centres.CtrCod=degrees.CtrCod"
                              " AND degrees.DegCod=courses.DegCod"
                              " AND courses.CrsCod=msg_snt.CrsCod"
                              " AND msg_snt.MsgCod=msg_rcv_deleted.MsgCod"
                              " AND msg_rcv_deleted.Notified='Y')",
                        Gbl.CurrentCty.Cty.CtyCod,
                        Gbl.CurrentCty.Cty.CtyCod);
               break;
            case Sco_SCOPE_INS:
               sprintf (Query,"SELECT "
                              "(SELECT COUNT(*)"
                              " FROM centres,degrees,courses,msg_snt,msg_rcv"
                              " WHERE centres.InsCod='%ld'"
                              " AND centres.CtrCod=degrees.CtrCod"
                              " AND degrees.DegCod=courses.DegCod"
                              " AND courses.CrsCod=msg_snt.CrsCod"
                              " AND msg_snt.MsgCod=msg_rcv.MsgCod"
                              " AND msg_rcv.Notified='Y')"
                              " + "
                              "(SELECT COUNT(*)"
                              " FROM centres,degrees,courses,msg_snt,msg_rcv_deleted"
                              " WHERE centres.InsCod='%ld'"
                              " AND centres.CtrCod=degrees.CtrCod"
                              " AND degrees.DegCod=courses.DegCod"
                              " AND courses.CrsCod=msg_snt.CrsCod"
                              " AND msg_snt.MsgCod=msg_rcv_deleted.MsgCod"
                              " AND msg_rcv_deleted.Notified='Y')",
                        Gbl.CurrentIns.Ins.InsCod,
                        Gbl.CurrentIns.Ins.InsCod);
               break;
            case Sco_SCOPE_CTR:
               sprintf (Query,"SELECT "
                              "(SELECT COUNT(*)"
                              " FROM degrees,courses,msg_snt,msg_rcv"
                              " WHERE degrees.CtrCod='%ld'"
                              " AND degrees.DegCod=courses.DegCod"
                              " AND courses.CrsCod=msg_snt.CrsCod"
                              " AND msg_snt.MsgCod=msg_rcv.MsgCod"
                              " AND msg_rcv.Notified='Y')"
                              " + "
                              "(SELECT COUNT(*)"
                              " FROM degrees,courses,msg_snt,msg_rcv_deleted"
                              " WHERE degrees.CtrCod='%ld'"
                              " AND degrees.DegCod=courses.DegCod"
                              " AND courses.CrsCod=msg_snt.CrsCod"
                              " AND msg_snt.MsgCod=msg_rcv_deleted.MsgCod"
                              " AND msg_rcv_deleted.Notified='Y')",
                        Gbl.CurrentCtr.Ctr.CtrCod,
                        Gbl.CurrentCtr.Ctr.CtrCod);
               break;
            case Sco_SCOPE_DEG:
               sprintf (Query,"SELECT "
                              "(SELECT COUNT(*)"
                              " FROM courses,msg_snt,msg_rcv"
                              " WHERE courses.DegCod='%ld'"
                              " AND courses.CrsCod=msg_snt.CrsCod"
                              " AND msg_snt.MsgCod=msg_rcv.MsgCod"
                              " AND msg_rcv.Notified='Y')"
                              " + "
                              "(SELECT COUNT(*)"
                              " FROM courses,msg_snt,msg_rcv_deleted"
                              " WHERE courses.DegCod='%ld'"
                              " AND courses.CrsCod=msg_snt.CrsCod"
                              " AND msg_snt.MsgCod=msg_rcv_deleted.MsgCod"
                              " AND msg_rcv_deleted.Notified='Y')",
                        Gbl.CurrentDeg.Deg.DegCod,
                        Gbl.CurrentDeg.Deg.DegCod);
               break;
            case Sco_SCOPE_CRS:
               sprintf (Query,"SELECT "
                              "(SELECT COUNT(*)"
                              " FROM msg_snt,msg_rcv"
                              " WHERE msg_snt.CrsCod='%ld'"
                              " AND msg_snt.MsgCod=msg_rcv.MsgCod"
                              " AND msg_rcv.Notified='Y')"
                              " + "
                              "(SELECT COUNT(*)"
                              " FROM msg_snt,msg_rcv_deleted"
                              " WHERE msg_snt.CrsCod='%ld'"
                              " AND msg_snt.MsgCod=msg_rcv_deleted.MsgCod"
                              " AND msg_rcv_deleted.Notified='Y')",
                        Gbl.CurrentCrs.Crs.CrsCod,
                        Gbl.CurrentCrs.Crs.CrsCod);
               break;
	    default:
	       Lay_ShowErrorAndExit ("Wrong scope.");
	       break;
           }
         break;
     }
   return (unsigned) DB_QueryCOUNT (Query,"can not get number of received messages");
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
	       sprintf (Gbl.Title,"1 %s, 1 %s",
			Txt_message_received,Txt_unread_MESSAGE);
	    else
	       sprintf (Gbl.Title,"1 %s",
			Txt_message_received);
	   }
	 else
	   {
	    if (NumUnreadMsgs == 0)
	       sprintf (Gbl.Title,"%u %s",
			Gbl.Msg.NumMsgs,Txt_messages_received);
	    else if (NumUnreadMsgs == 1)
	       sprintf (Gbl.Title,"%u %s, 1 %s",
			Gbl.Msg.NumMsgs,Txt_messages_received,
			Txt_unread_MESSAGE);
	    else
	       sprintf (Gbl.Title,"%u %s, %u %s",
			Gbl.Msg.NumMsgs,Txt_messages_received,
			NumUnreadMsgs,Txt_unread_MESSAGES);
	   }
	 break;
      case Msg_MESSAGES_SENT:
	 if (Gbl.Msg.NumMsgs == 1)
	    sprintf (Gbl.Title,"1 %s",
		     Txt_message_sent);
	 else
	    sprintf (Gbl.Title,"%u %s",
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
   /***** Put icons to remove messages *****/
   switch (Gbl.Msg.TypeOfMessages)
     {
      case Msg_MESSAGES_RECEIVED:
	 if (Gbl.Msg.NumMsgs == 1)
	    Msg_PutIconToRemoveOneRcvMsg ();
	 else if (Gbl.Msg.NumMsgs > 1)
	    Msg_PutIconToRemoveSevRcvMsgs ();
	 break;
      case Msg_MESSAGES_SENT:
	 if (Gbl.Msg.NumMsgs == 1)
	    Msg_PutIconToRemoveOneSntMsg ();
	 else if (Gbl.Msg.NumMsgs > 1)
	    Msg_PutIconToRemoveSevSntMsgs ();
	 break;
     }

   /***** Put icon to show a figure *****/
   Gbl.Stat.FigureType = Sta_MESSAGES;
   Sta_PutIconToShowFigure ();
  }

/*****************************************************************************/
/***************** Put icon to remove one received message *******************/
/*****************************************************************************/

static void Msg_PutIconToRemoveOneRcvMsg (void)
  {
   extern const char *Txt_Remove_this_message;

   Lay_PutContextualLink (ActReqDelAllRcvMsg,Msg_PutHiddenParamsMsgsFilters,
			  "remove-on64x64.png",
			  Txt_Remove_this_message,NULL,
                          NULL);
  }

/*****************************************************************************/
/*************** Put icon to remove several received messages ****************/
/*****************************************************************************/

static void Msg_PutIconToRemoveSevRcvMsgs (void)
  {
   extern const char *Txt_Remove_these_messages;

   Lay_PutContextualLink (ActReqDelAllRcvMsg,Msg_PutHiddenParamsMsgsFilters,
			  "remove-on64x64.png",
			  Txt_Remove_these_messages,NULL,
                          NULL);
  }

/*****************************************************************************/
/******************** Put icon to remove one sent message ********************/
/*****************************************************************************/

static void Msg_PutIconToRemoveOneSntMsg (void)
  {
   extern const char *Txt_Remove_this_message;

   Lay_PutContextualLink (ActReqDelAllSntMsg,Msg_PutHiddenParamsMsgsFilters,
			  "remove-on64x64.png",
			  Txt_Remove_this_message,NULL,
                          NULL);
  }

/*****************************************************************************/
/***************** Put icon to remove several sent messages ******************/
/*****************************************************************************/

static void Msg_PutIconToRemoveSevSntMsgs (void)
  {
   extern const char *Txt_Remove_these_messages;

   Lay_PutContextualLink (ActReqDelAllSntMsg,Msg_PutHiddenParamsMsgsFilters,
			  "remove-on64x64.png",
			  Txt_Remove_these_messages,NULL,
                          NULL);
  }

/*****************************************************************************/
/***** Write a form parameter to specify filter "from"/"to" for messages *****/
/*****************************************************************************/

void Msg_PutHiddenParamsMsgsFilters (void)
  {
   if (Gbl.Msg.FilterCrsCod >= 0)
      Par_PutHiddenParamLong ("FilterCrsCod",Gbl.Msg.FilterCrsCod);
   if (Gbl.Msg.FilterFromTo[0])
      Par_PutHiddenParamString ("FilterFromTo",Gbl.Msg.FilterFromTo);
   if (Gbl.Msg.FilterContent[0])
      Par_PutHiddenParamString ("FilterContent",Gbl.Msg.FilterContent);
   if (Gbl.Msg.ShowOnlyUnreadMsgs)
      Par_PutHiddenParamChar ("OnlyUnreadMsgs",'Y');
  }

/*****************************************************************************/
/********************* Get dictinct courses in my messages *******************/
/*****************************************************************************/

void Msg_GetDistinctCoursesInMyMessages (void)
  {
   char Query[512];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRow,NumRows;
   struct Course Crs;

   /***** Get distinct courses in my messages from database *****/
   switch (Gbl.Msg.TypeOfMessages)
     {
      case Msg_MESSAGES_RECEIVED:
         sprintf (Query,"SELECT DISTINCT courses.CrsCod,courses.ShortName"
                        " FROM msg_rcv,msg_snt,courses"
                        " WHERE msg_rcv.UsrCod='%ld'"
                        " AND msg_rcv.MsgCod=msg_snt.MsgCod"
                        " AND msg_snt.CrsCod=courses.CrsCod"
                        " ORDER BY courses.ShortName",
                  Gbl.Usrs.Me.UsrDat.UsrCod);
         break;
      case Msg_MESSAGES_SENT:
         sprintf (Query,"SELECT DISTINCT courses.CrsCod,courses.ShortName"
                        " FROM msg_snt,courses"
                        " WHERE msg_snt.UsrCod='%ld'"
                        " AND msg_snt.CrsCod=courses.CrsCod"
                        " ORDER BY courses.ShortName",
                  Gbl.Usrs.Me.UsrDat.UsrCod);
         break;
      default: // Not aplicable here
         break;
     }
   NumRows = DB_QuerySELECT (Query,&mysql_res,"can not get distinct courses in your messages");

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
            strcpy (Gbl.Msg.Courses[Gbl.Msg.NumCourses].ShrtName,Crs.ShrtName);
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
   extern const char *The_ClassForm[The_NUM_THEMES];
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
   fprintf (Gbl.F.Out,"<span class=\"%s\">%s </span>"
                      "<select name=\"FilterCrsCod\">"
                      "<option value=\"\"",
            The_ClassForm[Gbl.Prefs.Theme],
            TxtSelector[Gbl.Msg.TypeOfMessages]);
   if (Gbl.Msg.FilterCrsCod < 0)
      fprintf (Gbl.F.Out," selected=\"selected\"");
   fprintf (Gbl.F.Out,">%s</option>",Txt_any_course);

   /***** Write an option for each user's course  *****/
   for (NumOriginCrs = 0;
	NumOriginCrs < Gbl.Msg.NumCourses;
	NumOriginCrs++)
     {
      fprintf (Gbl.F.Out,"<option value=\"%ld\"",Gbl.Msg.Courses[NumOriginCrs].CrsCod);
      if (Gbl.Msg.Courses[NumOriginCrs].CrsCod == Gbl.Msg.FilterCrsCod)
        fprintf (Gbl.F.Out," selected=\"selected\"");	// Select origin course
      fprintf (Gbl.F.Out,">%s</option>",Gbl.Msg.Courses[NumOriginCrs].ShrtName);
     }
   fprintf (Gbl.F.Out,"</select>");
  }

/*****************************************************************************/
/***** Show form to filter "from" and "to" for received or sent messages *****/
/*****************************************************************************/

void Msg_ShowFormToFilterMsgs (void)
  {
   extern const char *The_ClassForm[The_NUM_THEMES];
   extern const char *Txt_MSG_From;
   extern const char *Txt_MSG_To;
   extern const char *Txt_MSG_Message;
   const char *TxtFromTo[Msg_NUM_TYPES_OF_MSGS] =
     {
      Txt_MSG_From,
      Txt_MSG_To
     };

   /***** Table start *****/
   fprintf (Gbl.F.Out,"<table class=\"CELLS_PAD_2\" style=\"margin:0 auto;\">"
	              "<tr>");

   /***** Authors/recipients of the message *****/
   fprintf (Gbl.F.Out,"<td class=\"LEFT_MIDDLE\">"
                      "<label class=\"%s\">"
                      "%s: "
                      "</label>"
                      "<input type=\"text\" name=\"FilterFromTo\""
                      " size=\"20\" maxlength=\"%u\" value=\"%s\" />"
                      "</td>",
            The_ClassForm[Gbl.Prefs.Theme],
            TxtFromTo[Gbl.Msg.TypeOfMessages],
            Usr_MAX_LENGTH_USR_NAME_OR_SURNAME*3,Gbl.Msg.FilterFromTo);

   /***** Authors/recipients of the message *****/
   fprintf (Gbl.F.Out,"<td class=\"LEFT_MIDDLE\">"
                      "<label class=\"%s\">"
                      "%s: "
                      "</label>"
                      "<input type=\"text\" name=\"FilterContent\""
                      " size=\"20\" maxlength=\"%u\" value=\"%s\" />"
                      "</td>",
            The_ClassForm[Gbl.Prefs.Theme],
            Txt_MSG_Message,
            Msg_MAX_LENGTH_FILTER_CONTENT,Gbl.Msg.FilterContent);

   /***** End table *****/
   fprintf (Gbl.F.Out,"</tr>"
                      "</table>");
  }

/*****************************************************************************/
/**** Show form to select whether to show only unread (received) messages ****/
/*****************************************************************************/

static void Msg_ShowFormToShowOnlyUnreadMessages (void)
  {
   extern const char *The_ClassForm[The_NUM_THEMES];
   extern const char *Txt_only_unread_messages;

   /***** Put checkbox to select whether to show only unread (received) messages *****/
   fprintf (Gbl.F.Out,"<input type=\"checkbox\""
	              " name=\"OnlyUnreadMsgs\" value=\"Y\"");
   if (Gbl.Msg.ShowOnlyUnreadMsgs)
      fprintf (Gbl.F.Out," checked=\"checked\"");
   fprintf (Gbl.F.Out," />"
	              "<span class=\"%s\">%s</span>",
            The_ClassForm[Gbl.Prefs.Theme],
            Txt_only_unread_messages);
  }

/*****************************************************************************/
/*********** Get parameter to show only unread (received) messages ***********/
/*****************************************************************************/

static void Msg_GetParamOnlyUnreadMsgs (void)
  {
   char YN[1+1];

   /***** Get parameter to show only unread (received) messages *****/
   Par_GetParToText ("OnlyUnreadMsgs",YN,1);
   Gbl.Msg.ShowOnlyUnreadMsgs = (Str_ConvertToUpperLetter (YN[0]) == 'Y');
  }

/*****************************************************************************/
/***************************** Get data of a message *************************/
/*****************************************************************************/

static void Msg_GetMsgSntData (long MsgCod,long *CrsCod,long *UsrCod,
                               time_t *CreatTimeUTC,char *Subject,bool *Deleted)
  {
   char Query[512];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;

   /***** Get data of message from table msg_snt *****/
   *Deleted = false;
   sprintf (Query,"SELECT CrsCod,UsrCod,UNIX_TIMESTAMP(CreatTime)"
                  " FROM msg_snt WHERE MsgCod='%ld'",
            MsgCod);
   NumRows = DB_QuerySELECT (Query,&mysql_res,"can not get data of a message");

   if (NumRows == 0)   // If not result ==> sent message is deleted
     {
      /***** Get data of message from table msg_snt_deleted *****/
      sprintf (Query,"SELECT CrsCod,UsrCod,UNIX_TIMESTAMP(CreatTime)"
                     " FROM msg_snt_deleted WHERE MsgCod='%ld'",
               MsgCod);
      NumRows = DB_QuerySELECT (Query,&mysql_res,"can not get data of a message");

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

void Msg_GetMsgSubject (long MsgCod,char *Subject)
  {
   char Query[512];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;

   /***** Get subject of message from database *****/
   sprintf (Query,"SELECT Subject FROM msg_content WHERE MsgCod='%ld'",MsgCod);

   /***** Result should have a unique row *****/
   if (DB_QuerySELECT (Query,&mysql_res,"can not get the subject of a message") == 1)
     {
      /***** Get subject *****/
      row = mysql_fetch_row (mysql_res);
      strncpy (Subject,row[0],Cns_MAX_BYTES_SUBJECT);
      Subject[Cns_MAX_BYTES_SUBJECT] = '\0';
     }
   else
      Subject[0] = '\0';

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/*************** Get content and optional image of a message *****************/
/*****************************************************************************/

static void Msg_GetMsgContent (long MsgCod,char *Content,struct Image *Image)
  {
   char Query[256];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;

   /***** Get content of message from database *****/
   sprintf (Query,"SELECT Content,ImageName,ImageTitle,ImageURL"
	          " FROM msg_content WHERE MsgCod='%ld'",
            MsgCod);
   NumRows = DB_QuerySELECT (Query,&mysql_res,"can not get the content of a message");

   /***** Result should have a unique row *****/
   if (NumRows != 1)
      Lay_ShowErrorAndExit ("Error when getting content of a message.");

   /***** Get number of rows *****/
   row = mysql_fetch_row (mysql_res);

   /****** Get content (row[0]) *****/
   strncpy (Content,row[0],Cns_MAX_BYTES_LONG_TEXT);
   Content[Cns_MAX_BYTES_LONG_TEXT] = '\0';

   /****** Get image name (row[1]), title (row[2]) and URL (row[3]) *****/
   Img_GetImageNameTitleAndURLFromRow (row[1],row[2],row[3],Image);

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/********************** Get if a sent message is expanded ********************/
/*****************************************************************************/

static void Msg_GetStatusOfSentMsg (long MsgCod,bool *Expanded)
  {
   char Query[512];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;

   /***** Get if sent message has been replied/expanded from database *****/
   sprintf (Query,"SELECT Expanded FROM msg_snt"
                  " WHERE MsgCod='%ld' AND UsrCod='%ld'",
            MsgCod,Gbl.Usrs.Me.UsrDat.UsrCod);
   NumRows = DB_QuerySELECT (Query,&mysql_res,"can not get if a sent message has been replied/expanded");

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
   char Query[512];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;

   /***** Get if received message has been replied/expanded from database *****/
   sprintf (Query,"SELECT Open,Replied,Expanded FROM msg_rcv"
                  " WHERE MsgCod='%ld' AND UsrCod='%ld'",
            MsgCod,Gbl.Usrs.Me.UsrDat.UsrCod);
   NumRows = DB_QuerySELECT (Query,&mysql_res,"can not get if a received message has been replied/expanded");

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
   extern const char *Txt_MSG_Message;
   struct UsrData UsrDat;
   const char *Title = NULL;	// Initialized to avoid warning
   bool FromThisCrs = false;	// Initialized to avoid warning
   time_t CreatTimeUTC;		// Creation time of a message
   long CrsCod;
   char Subject[Cns_MAX_BYTES_SUBJECT+1];
   char Content[Cns_MAX_BYTES_LONG_TEXT+1];
   struct Image Image;
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

   fprintf (Gbl.F.Out,"<tr>"
	              "<td class=\"%s CENTER_TOP\" style=\"width:20px;\">"
                      "<img src=\"%s/msg-%s16x16.gif\""
                      " alt=\"%s\" title=\"%s\""
                      " class=\"ICO20x20\" />",
            Gbl.Msg.TypeOfMessages == Msg_MESSAGES_RECEIVED ? (Open ? "BG_MSG_BLUE" :
        	                                                      "BG_MSG_GREEN") :
                                                              "BG_MSG_BLUE",

            Gbl.Prefs.IconsURL,
            Gbl.Msg.TypeOfMessages == Msg_MESSAGES_RECEIVED ? (Open ? (Replied ? "replied" :
        	                                                                 "open") :
                                                                      "unread") :
                                                              "fwd",
            Title,Title);

   /***** Form to delete message *****/
   fprintf (Gbl.F.Out,"<br />");
   Msg_PutFormToDeleteMessage (MsgCod);
   fprintf (Gbl.F.Out,"</td>");

   /***** Write message number *****/
   Msg_WriteMsgNumber (MsgNum,!Open);

   /***** Write message author *****/
   Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat);
   Msg_WriteMsgAuthor (&UsrDat,100,11,
	               Open ? "MSG_AUT_BG" :
	        	      "MSG_AUT_BG_NEW",	// Style
	               true,NULL);

   /***** Write subject *****/
   Msg_WriteSentOrReceivedMsgSubject (MsgCod,Subject,Open,Expanded);

   /***** Write date-time *****/
   Msg_WriteMsgDate (CreatTimeUTC,Open ? "MSG_TIT_BG" :
	                                 "MSG_TIT_BG_NEW");

   fprintf (Gbl.F.Out,"</tr>");

   if (Expanded)
     {
      fprintf (Gbl.F.Out,"<tr>"
	                 "<td rowspan=\"3\" colspan=\"3\" class=\"LEFT_TOP\">"
                         "<table class=\"CELLS_PAD_2\">");

      /***** Write course origin of message *****/
      fprintf (Gbl.F.Out,"<tr>"
	                 "<td class=\"LEFT_MIDDLE\">");
      FromThisCrs = Msg_WriteCrsOrgMsg (CrsCod);
      fprintf (Gbl.F.Out,"</td>"
	                 "</tr>");

      /***** Form to reply message *****/
      fprintf (Gbl.F.Out,"<tr>"
	                 "<td class=\"LEFT_MIDDLE\">");
      if (Gbl.Msg.TypeOfMessages == Msg_MESSAGES_RECEIVED &&
	  Gbl.Usrs.Me.LoggedRole >= Rol_VISITOR)
	 // Guests (users without courses) can read messages but not reply them
         Msg_WriteFormToReply (MsgCod,CrsCod,FromThisCrs,Replied,&UsrDat);
      fprintf (Gbl.F.Out,"</td>"
	                 "</tr>"
	                 "</table>"
	                 "</td>");

      /***** Write "From:" *****/
      fprintf (Gbl.F.Out,"<td class=\"RIGHT_TOP\">"
                         "<label class=\"MSG_TIT\">"
	                 "%s: "
                         "</label>"
	                 "</td>"
                         "<td colspan=\"2\" class=\"LEFT_TOP\">",
               Txt_MSG_From);
      Msg_WriteMsgFrom (&UsrDat,Deleted);
      fprintf (Gbl.F.Out,"</td>"
	                 "</tr>");

      /***** Write "To:" *****/
      fprintf (Gbl.F.Out,"<tr>"
	                 "<td class=\"RIGHT_TOP\">"
                         "<label class=\"MSG_TIT\">"
	                 "%s: "
                         "</label>"
	                 "</td>"
                         "<td colspan=\"2\" class=\"LEFT_TOP\">",
               Txt_MSG_To);
      Msg_WriteMsgTo (MsgCod);
      fprintf (Gbl.F.Out,"</td>"
	                 "</tr>");

      /***** Write "Message:" *****/
      fprintf (Gbl.F.Out,"<tr>"
	                 "<td class=\"RIGHT_TOP\">"
                         "<label class=\"MSG_TIT\">"
	                 "%s: "
                         "</label>"
	                 "</td>",
               Txt_MSG_Message);

      /***** Initialize image *****/
      Img_ImageConstructor (&Image);

      /***** Get message content and optional image *****/
      Msg_GetMsgContent (MsgCod,Content,&Image);

      /***** Show content and image *****/
      fprintf (Gbl.F.Out,"<td colspan=\"2\" class=\"MSG_TXT LEFT_TOP\">");
      if (Content[0])
         Msg_WriteMsgContent (Content,Cns_MAX_BYTES_LONG_TEXT,true,false);
      Img_ShowImage (&Image,"MSG_IMG_CONTAINER","MSG_IMG");
      fprintf (Gbl.F.Out,"</td>"
	                 "</tr>");

      /***** Free image *****/
      Img_ImageDestructor (&Image);
     }

   /***** Free memory used for user's data *****/
   Usr_UsrDataDestructor (&UsrDat);
  }

/*****************************************************************************/
/******************** Get subject and content of a message *******************/
/*****************************************************************************/
// This function may be called inside a web service, so don't report error

void Msg_GetNotifMessage (char *SummaryStr,char **ContentStr,long MsgCod,
                          unsigned MaxChars,bool GetContent)
  {
   extern const char *Txt_MSG_Subject;
   char Query[128];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;

   SummaryStr[0] = '\0';	// Return nothing on error

   /***** Get subject of message from database *****/
   sprintf (Query,"SELECT Subject,Content FROM msg_content WHERE MsgCod='%ld'",
            MsgCod);
   if (!mysql_query (&Gbl.mysql,Query))
      if ((mysql_res = mysql_store_result (&Gbl.mysql)) != NULL)
        {
         /***** Result should have a unique row *****/
         if (mysql_num_rows (mysql_res) == 1)
           {
            /***** Get subject and content of the message *****/
            row = mysql_fetch_row (mysql_res);

            /***** Copy subject *****/
            strcpy (SummaryStr,row[0]);
            if (MaxChars)
               Str_LimitLengthHTMLStr (SummaryStr,MaxChars);

            /***** Copy subject *****/
            if (GetContent)
              {
               if ((*ContentStr = (char *) malloc (strlen (row[1])+1)) == NULL)
                  Lay_ShowErrorAndExit ("Error allocating memory for notification content.");
               strcpy (*ContentStr,row[1]);
              }
           }
         mysql_free_result (mysql_res);
        }
  }

/*****************************************************************************/
/************************** Write number of message **************************/
/*****************************************************************************/

void Msg_WriteMsgNumber (unsigned long MsgNum,bool NewMsg)
  {
   fprintf (Gbl.F.Out,"<td class=\"%s CENTER_TOP\" style=\"width:45px;\">"
                      "%lu:"
                      "</td>",
	    NewMsg ? "MSG_TIT_BG_NEW" :
		     "MSG_TIT_BG",
            MsgNum);
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
   fprintf (Gbl.F.Out,"<td class=\"%s LEFT_TOP\">",
            Open ? "MSG_TIT_BG" :
        	   "MSG_TIT_BG_NEW");

   /***** Start form to expand/contract the message *****/
   Act_FormStart (Gbl.Msg.TypeOfMessages == Msg_MESSAGES_RECEIVED ? (Expanded ? ActConRcvMsg :
	                                                                        ActExpRcvMsg) :
                                                                    (Expanded ? ActConSntMsg :
                                                        	                ActExpSntMsg));
   Msg_PutHiddenParamsMsgsFilters ();
   Pag_PutHiddenParamPagNum (Gbl.Pag.CurrentPage);
   Msg_PutHiddenParamMsgCod (MsgCod);
   Act_LinkFormSubmit (Expanded ? Txt_Hide_message :
	                          Txt_See_message,
                       Open ? "MSG_TIT" :
                	      "MSG_TIT_NEW",NULL);

   /***** Write subject *****/
   if (Subject[0])
      fprintf (Gbl.F.Out,"%s",Subject);
   else
      fprintf (Gbl.F.Out,"[%s]",Txt_no_subject);

   /***** End form to expand the message *****/
   fprintf (Gbl.F.Out,"</a>");
   Act_FormEnd ();

   /***** End cell *****/
   fprintf (Gbl.F.Out,"</td>");
  }

/*****************************************************************************/
/************************ Write the author of a message **********************/
/*****************************************************************************/
// Input: UsrDat must hold user's data

void Msg_WriteMsgAuthor (struct UsrData *UsrDat,
                         unsigned WidthOfNameColumn,unsigned MaxCharsInName,
                         const char *Style,bool Enabled,const char *BgColor)
  {
   extern const char *Txt_Unknown_or_without_photo;
   bool ShowPhoto = false;
   char PhotoURL[PATH_MAX+1];
   bool WriteAuthor = false;

   /***** Start first column *****/
   fprintf (Gbl.F.Out,"<td class=\"%s CENTER_TOP",Style);
   if (BgColor)
      fprintf (Gbl.F.Out," %s",BgColor);
   fprintf (Gbl.F.Out,"\" style=\"width:30px;\">");

   /***** Write author name or don't write it? *****/
   if (Enabled)
      if (UsrDat->UsrCod > 0)
         WriteAuthor = true;

   if (WriteAuthor)
     {
      /***** First column with author's photo (if author has a web page, put a link to it) *****/
      ShowPhoto = Pho_ShowUsrPhotoIsAllowed (UsrDat,PhotoURL);
      Pho_ShowUsrPhoto (UsrDat,ShowPhoto ? PhotoURL :
                                           NULL,
                        "PHOTO30x40",Pho_ZOOM,false);
      fprintf (Gbl.F.Out,"</td>");

      /***** Second column with user name (if author has a web page, put a link to it) *****/
      fprintf (Gbl.F.Out,"<td class=\"%s LEFT_TOP",Style);
      if (BgColor)
         fprintf (Gbl.F.Out," %s",BgColor);
      fprintf (Gbl.F.Out,"\" style=\"width:%upx;\">",WidthOfNameColumn);

      /* Restrict length of firstname and surnames */
      Usr_RestrictLengthAndWriteName (UsrDat,MaxCharsInName);
     }
   else
     {
      fprintf (Gbl.F.Out,"<img src=\"%s/usr_bl.jpg\""
	                 " alt=\"%s\" title=\"%s\""
	                 " class=\"PHOTO30x40\" />"
	                 "</td>"
                         "<td class=\"%s LEFT_MIDDLE",
               Gbl.Prefs.IconsURL,
               Txt_Unknown_or_without_photo,Txt_Unknown_or_without_photo,
               Style);
      if (BgColor)
         fprintf (Gbl.F.Out," %s",BgColor);
      fprintf (Gbl.F.Out,"\" style=\"width:%upx;\">",
	       WidthOfNameColumn);
     }

   /***** End seconf column *****/
   fprintf (Gbl.F.Out,"</td>");
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
         if ((FromThisCrs = (CrsCod == Gbl.CurrentCrs.Crs.CrsCod)))	// Message sent from current course
             fprintf (Gbl.F.Out,"<div class=\"MSG_AUT\">"
        	                "(%s)"
        	                "</div>",
                      Txt_from_this_course);
         else	// Message sent from another course
           {
            /* Write course, including link */
            Act_FormGoToStart (ActSeeCrsInf);
            Crs_PutParamCrsCod (Crs.CrsCod);
            fprintf (Gbl.F.Out,"<div class=\"MSG_AUT\">"
        	               "(");
            sprintf (Gbl.Title,Txt_Go_to_X,Crs.FullName);
            Act_LinkFormSubmit (Gbl.Title,"MSG_AUT",NULL);
            fprintf (Gbl.F.Out,"%s</a>)"
        	               "</div>",
        	     Crs.ShrtName);
            Act_FormEnd ();
           }
	}
     }
   if (!ThereIsOrgCrs)	// It's an old message without origin source specified, or is a message sent from none course
      fprintf (Gbl.F.Out,"<div class=\"MSG_AUT\">"
	                 "(%s)"
	                 "</div>",
               Txt_no_course_of_origin);

   return FromThisCrs;
  }

/*****************************************************************************/
/************************* Write form to reply a message *********************/
/*****************************************************************************/

static void Msg_WriteFormToReply (long MsgCod,long CrsCod,
                                  bool FromThisCrs,bool Replied,
                                  const struct UsrData *UsrDat)
  {
   extern const char *The_ClassFormBold[The_NUM_THEMES];
   extern const char *Txt_Reply;
   extern const char *Txt_Reply_again;
   extern const char *Txt_Go_to_course_and_reply;
   extern const char *Txt_Go_to_course_and_reply_again;

   /***** Form start and parameters *****/
   if (FromThisCrs)
      Act_FormStart (ActReqMsgUsr);
   else	// Not the current course ==> go to another course
     {
      Act_FormGoToStart (ActReqMsgUsr);
      Crs_PutParamCrsCod (CrsCod);
     }
   Grp_PutParamAllGroups ();
   Par_PutHiddenParamChar ("IsReply",'Y');
   Msg_PutHiddenParamMsgCod (MsgCod);
   Usr_PutParamUsrCodEncrypted (UsrDat->EncryptedUsrCod);
   Par_PutHiddenParamChar ("ShowOnlyOneRecipient",'Y');

   /****** Link and form end *****/
   Act_LinkFormSubmit (FromThisCrs ? (Replied ? Txt_Reply_again :
						Txt_Reply) :
				     (Replied ? Txt_Go_to_course_and_reply_again :
						Txt_Go_to_course_and_reply),
		       The_ClassFormBold[Gbl.Prefs.Theme],NULL);
   Lay_PutIconWithText ("reply16x16.gif",
                        Replied ? Txt_Reply_again :
			          Txt_Reply,
			NULL);
   fprintf (Gbl.F.Out,"</a>");
   Act_FormEnd ();
  }

/*****************************************************************************/
/************************** Write author of a message ************************/
/*****************************************************************************/

static void Msg_WriteMsgFrom (struct UsrData *UsrDat,bool Deleted)
  {
   extern struct Act_Actions Act_Actions[Act_NUM_ACTIONS];
   extern const char *Txt_MSG_Sent;
   extern const char *Txt_MSG_Sent_and_deleted;
   extern const char *Txt_ROLES_SINGUL_abc[Rol_NUM_ROLES][Usr_NUM_SEXS];
   bool ShowPhoto;
   char PhotoURL[PATH_MAX+1];

   /***** Put an icon to show if user has read the message *****/
   fprintf (Gbl.F.Out,"<table>"
	              "<tr>"
                      "<td class=\"LEFT_MIDDLE\" style=\"width:20px;\">"
                      "<img src=\"%s/%s16x16.gif\""
                      " alt=\"%s\" title=\"%s\""
                      " class=\"ICO20x20\" />"
                      "</td>",
            Gbl.Prefs.IconsURL,
            Deleted ? "msg-fwd-del" :
        	      "msg-fwd",
            Deleted ? Txt_MSG_Sent_and_deleted :
                      Txt_MSG_Sent,
            Deleted ? Txt_MSG_Sent_and_deleted :
                      Txt_MSG_Sent);

   /***** Put user's photo *****/
   fprintf (Gbl.F.Out,"<td class=\"CENTER_MIDDLE\" style=\"width:30px;\">");
   ShowPhoto = (Pho_ShowUsrPhotoIsAllowed (UsrDat,PhotoURL));
   Pho_ShowUsrPhoto (UsrDat,ShowPhoto ? PhotoURL :
                	                NULL,
                     "PHOTO21x28",Pho_ZOOM,false);

   /***** Write user's name *****/
   fprintf (Gbl.F.Out,"</td>"
	              "<td class=\"MSG_AUT LEFT_MIDDLE\">");
   if (UsrDat->UsrCod > 0)
     {
      fprintf (Gbl.F.Out,"%s",UsrDat->FullName);
      if (Act_Actions[Gbl.Action.Act].SuperAction == ActSeeRcvMsg)
	{
         if (Msg_CheckIfUsrIsBanned (UsrDat->UsrCod,Gbl.Usrs.Me.UsrDat.UsrCod))
            // Sender is banned
            Msg_PutFormToUnbanSender (UsrDat);
         else
            // Sender is not banned
            Msg_PutFormToBanSender (UsrDat);
	}
     }
   else
      fprintf (Gbl.F.Out,"[%s]",
               Txt_ROLES_SINGUL_abc[Rol_UNKNOWN][Usr_SEX_UNKNOWN]);	// User not found, likely an old user who has been removed
   fprintf (Gbl.F.Out,"</td>"
	              "</tr>"
	              "</table>");
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
   char Query[2048];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumRcp;
   unsigned NumRecipientsTotal;
   unsigned NumRecipientsKnown;
   unsigned NumRecipientsUnknown;
   unsigned NumRecipientsToShow;
   char YN[1+1];
   struct UsrData UsrDat;
   bool Deleted;
   bool OpenByDst;
   bool UsrValid;
   bool ShowPhoto;
   const char *Title;
   char PhotoURL[PATH_MAX+1];
   static const Act_Action_t ActionSee[Msg_NUM_TYPES_OF_MSGS] =
     {
      ActSeeRcvMsg,
      ActSeeSntMsg
     };

   /***** Get number of recipients of a message from database *****/
   sprintf (Query,"SELECT "
                  "(SELECT COUNT(*) FROM msg_rcv WHERE MsgCod='%ld')"
                  " + "
                  "(SELECT COUNT(*) FROM msg_rcv_deleted WHERE MsgCod='%ld')",
            MsgCod,MsgCod);
   NumRecipientsTotal = (unsigned) DB_QueryCOUNT (Query,"can not get number of recipients");

   /***** Get recipients of a message from database *****/
   sprintf (Query,"(SELECT msg_rcv.UsrCod,'N',msg_rcv.Open,"
                  "usr_data.Surname1 AS S1,usr_data.Surname2 AS S2,usr_data.FirstName AS FN"
                  " FROM msg_rcv,usr_data"
                  " WHERE msg_rcv.MsgCod='%ld' AND msg_rcv.UsrCod=usr_data.UsrCod)"
                  " UNION "
                  "(SELECT msg_rcv_deleted.UsrCod,'Y',msg_rcv_deleted.Open,"
                  "usr_data.Surname1 AS S1,usr_data.Surname2 AS S2,usr_data.FirstName AS FN"
                  " FROM msg_rcv_deleted,usr_data"
                  " WHERE msg_rcv_deleted.MsgCod='%ld' AND msg_rcv_deleted.UsrCod=usr_data.UsrCod)"
                  " ORDER BY S1,S2,FN",
            MsgCod,MsgCod);
   NumRecipientsKnown = (unsigned) DB_QuerySELECT (Query,&mysql_res,"can not get recipients of a message");

   /***** Check number of recipients *****/
   if (NumRecipientsTotal)
     {
      /***** Start table *****/
      fprintf (Gbl.F.Out,"<table>");

      /***** How many recipients will be shown? *****/
      if (NumRecipientsKnown <= Msg_MAX_RECIPIENTS_TO_SHOW)
         NumRecipientsToShow = NumRecipientsKnown;
      else	// A lot of recipients
        {
         /***** Get parameter that indicates if I want to see all recipients *****/
         Par_GetParToText ("SeeAllRcpts",YN,1);
         NumRecipientsToShow = (Str_ConvertToUpperLetter (YN[0]) == 'Y') ? NumRecipientsKnown :
                                                                           Msg_DEF_RECIPIENTS_TO_SHOW;
        }

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
	 UsrValid = Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat);

         /* Put an icon to show if user has read the message */
	 Title = OpenByDst ? (Deleted ? Txt_MSG_Open_and_deleted :
                                        Txt_MSG_Open) :
                             (Deleted ? Txt_MSG_Deleted_without_opening :
                                        Txt_MSG_Unopened);
         fprintf (Gbl.F.Out,"<tr>"
                            "<td class=\"LEFT_MIDDLE\" style=\"width:20px;\">"
                            "<img src=\"%s/%s16x16.gif\""
                            " alt=\"%s\" title=\"%s\""
                            " class=\"ICO20x20\" />"
                            "</td>",
                  Gbl.Prefs.IconsURL,
                  OpenByDst ? (Deleted ? "msg-open-del"   :
                	                 "msg-open") :
                              (Deleted ? "msg-unread-del" :
                        	         "msg-unread"),
                  Title,Title);

         /* Put user's photo */
         fprintf (Gbl.F.Out,"<td class=\"CENTER_TOP\" style=\"width:30px;\">");
         ShowPhoto = (UsrValid ? Pho_ShowUsrPhotoIsAllowed (&UsrDat,PhotoURL) :
                                 false);
         Pho_ShowUsrPhoto (&UsrDat,ShowPhoto ? PhotoURL :
                        	               NULL,
                           "PHOTO21x28",Pho_ZOOM,false);

         /* Write user's name */
         fprintf (Gbl.F.Out,"</td>"
                            "<td class=\"%s LEFT_MIDDLE\">",
                  OpenByDst ? "MSG_AUT" :
                	      "MSG_AUT_NEW");
         if (UsrValid)
            fprintf (Gbl.F.Out,"%s",UsrDat.FullName);
         else
            fprintf (Gbl.F.Out,"[%s]",Txt_unknown_recipient);	// User not found, likely a user who has been removed
         fprintf (Gbl.F.Out,"</td>"
                            "</tr>");
        }

      /***** If any recipients are unknown *****/
      if ((NumRecipientsUnknown = NumRecipientsTotal - NumRecipientsKnown))
         /***** Start form to show all the users *****/
         fprintf (Gbl.F.Out,"<tr>"
                            "<td colspan=\"3\" class=\"MSG_AUT LEFT_MIDDLE\">"
                            "[%u %s]"
                            "</td>"
                            "</tr>",
                  NumRecipientsUnknown,
                  (NumRecipientsUnknown == 1) ?
                  Txt_unknown_recipient :
                  Txt_unknown_recipients);

      /***** If any known recipient is not listed *****/
      if (NumRecipientsToShow < NumRecipientsKnown)
        {
         /***** Start form to show all the users *****/
         fprintf (Gbl.F.Out,"<tr>"
                            "<td colspan=\"3\" class=\"MSG_AUT LEFT_MIDDLE\">");
         Act_FormStart (ActionSee[Gbl.Msg.TypeOfMessages]);
         Msg_PutHiddenParamsMsgsFilters ();
         Pag_PutHiddenParamPagNum (Gbl.Pag.CurrentPage);
         Msg_PutHiddenParamMsgCod (MsgCod);
         Par_PutHiddenParamChar ("SeeAllRcpts",'Y');
         Act_LinkFormSubmit (Txt_View_all_recipients,"MSG_AUT",NULL);
         fprintf (Gbl.F.Out,Txt_and_X_other_recipients,
                  NumRecipientsKnown - NumRecipientsToShow);
         fprintf (Gbl.F.Out,"</a>");
         Act_FormEnd ();
         fprintf (Gbl.F.Out,"</td>"
                            "</tr>");
        }

      /***** Free memory used for user's data *****/
      Usr_UsrDataDestructor (&UsrDat);

      /***** End table *****/
      fprintf (Gbl.F.Out,"</table>");
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
   extern const char *Txt_Today;
   static unsigned UniqueId = 0;

   UniqueId++;

   /***** Start cell *****/
   fprintf (Gbl.F.Out,"<td id=\"date_%u\" class=\"%s RIGHT_TOP\""
	              " style=\"width:106px;\">",
            UniqueId,ClassBackground);

   /***** Write date and time *****/
   fprintf (Gbl.F.Out,"<script type=\"text/javascript\">"
                      "writeLocalDateHMSFromUTC('date_%u',%ld,'&nbsp;','%s');"
                      "</script>",
            UniqueId,(long) TimeUTC,Txt_Today);

   /***** End cell *****/
   fprintf (Gbl.F.Out,"</td>");
  }

/*****************************************************************************/
/************* Put a form to delete a received or sent message ***************/
/*****************************************************************************/

static void Msg_PutFormToDeleteMessage (long MsgCod)
  {
   static const Act_Action_t ActionDel[Msg_NUM_TYPES_OF_MSGS] =
     {
      ActDelRcvMsg,
      ActDelSntMsg
     };

   Act_FormStart (ActionDel[Gbl.Msg.TypeOfMessages]);
   Pag_PutHiddenParamPagNum (Gbl.Pag.CurrentPage);
   Msg_PutHiddenParamMsgCod (MsgCod);
   Msg_PutHiddenParamsMsgsFilters ();
   Lay_PutIconRemove ();
   Act_FormEnd ();
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
      fprintf (Gbl.F.Out,"%s",Content);
  }

/*****************************************************************************/
/*************** Get parameter with the code of a message ********************/
/*****************************************************************************/

void Msg_PutHiddenParamMsgCod (long MsgCod)
  {
   Par_PutHiddenParamLong ("MsgCod",MsgCod);
  }

/*****************************************************************************/
/*************** Get parameter with the code of a message ********************/
/*****************************************************************************/

static long Msg_GetParamMsgCod (void)
  {
   char LongStr[1+10+1];

   /***** Get parameter with code of message *****/
   Par_GetParToText ("MsgCod",LongStr,1+10);
   return Str_ConvertStrCodToLongCod (LongStr);
  }

/*****************************************************************************/
/***************** Put a form to ban the sender of a message *****************/
/*****************************************************************************/

static void Msg_PutFormToBanSender (struct UsrData *UsrDat)
  {
   extern const char *Txt_Sender_permitted_click_to_ban_him;

   Act_FormStart (ActBanUsrMsg);
   Pag_PutHiddenParamPagNum (Gbl.Pag.CurrentPage);
   Usr_PutParamUsrCodEncrypted (UsrDat->EncryptedUsrCod);
   Msg_PutHiddenParamsMsgsFilters ();
   fprintf (Gbl.F.Out,"<input type=\"image\" src=\"%s/open_on16x16.gif\""
	              " alt=\"%s\" title=\"%s\""
	              " class=\"ICO20x20\" style=\"margin-left:12px;\" />",
            Gbl.Prefs.IconsURL,
            Txt_Sender_permitted_click_to_ban_him,
            Txt_Sender_permitted_click_to_ban_him);
   Act_FormEnd ();
  }

/*****************************************************************************/
/**************** Put a form to unban the sender of a message ****************/
/*****************************************************************************/

static void Msg_PutFormToUnbanSender (struct UsrData *UsrDat)
  {
   extern const char *Txt_Sender_banned_click_to_unban_him;

   Act_FormStart (ActUnbUsrMsg);
   Pag_PutHiddenParamPagNum (Gbl.Pag.CurrentPage);
   Usr_PutParamUsrCodEncrypted (UsrDat->EncryptedUsrCod);
   Msg_PutHiddenParamsMsgsFilters ();
   fprintf (Gbl.F.Out,"<span class=\"MSG_AUT\">&nbsp;</span>"
	              "<input type=\"image\" src=\"%s/closed_on16x16.gif\""
	              " alt=\"%s\" title=\"%s\""
	              " class=\"ICO20x20\" />",
            Gbl.Prefs.IconsURL,
            Txt_Sender_banned_click_to_unban_him,
            Txt_Sender_banned_click_to_unban_him);
   Act_FormEnd ();
  }

/*****************************************************************************/
/********* Ban a sender of a message when showing received messages **********/
/*****************************************************************************/

void Msg_BanSenderWhenShowingMsgs (void)
  {
   extern const char *Txt_From_this_time_you_will_not_receive_messages_from_X;
   char Query[256];

   /***** Get user's code from form *****/
   Usr_GetParamOtherUsrCodEncryptedAndGetListIDs ();

   /***** Get password, user type and user's data from database *****/
   if (!Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&Gbl.Usrs.Other.UsrDat))
      Lay_ShowErrorAndExit ("Sender does not exist.");

   /***** Insert pair (sender's code - my code) in table of banned senders if not inserted *****/
   sprintf (Query,"REPLACE INTO msg_banned (FromUsrCod,ToUsrCod)"
                  " VALUES ('%ld','%ld')",
            Gbl.Usrs.Other.UsrDat.UsrCod,Gbl.Usrs.Me.UsrDat.UsrCod);
   DB_QueryREPLACE (Query,"can not ban sender");

   /***** Show alert with the change made *****/
   sprintf (Gbl.Message,Txt_From_this_time_you_will_not_receive_messages_from_X,
            Gbl.Usrs.Other.UsrDat.FullName);
   Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);

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
   char Query[256];

   /***** Get user's code from form *****/
   Usr_GetParamOtherUsrCodEncryptedAndGetListIDs ();

   /***** Get password, user type and user's data from database *****/
   if (!Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&Gbl.Usrs.Other.UsrDat))
      Lay_ShowErrorAndExit ("Sender does not exist.");

   /***** Remove pair (sender's code - my code) from table of banned senders *****/
   sprintf (Query,"DELETE FROM msg_banned"
                  " WHERE FromUsrCod='%ld' AND ToUsrCod='%ld'",
            Gbl.Usrs.Other.UsrDat.UsrCod,Gbl.Usrs.Me.UsrDat.UsrCod);
   DB_QueryDELETE (Query,"can not ban sender");

   /***** Show alert with the change made *****/
   sprintf (Gbl.Message,Txt_From_this_time_you_can_receive_messages_from_X,
            Gbl.Usrs.Other.UsrDat.FullName);
   Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);
  }

/*****************************************************************************/
/**************** Chech if a user is banned by another user ******************/
/*****************************************************************************/

static bool Msg_CheckIfUsrIsBanned (long FromUsrCod,long ToUsrCod)
  {
   char Query[256];

   /***** Get if FromUsrCod is banned by ToUsrCod *****/
   sprintf (Query,"SELECT COUNT(*) FROM msg_banned"
                  " WHERE FromUsrCod='%ld' AND ToUsrCod='%ld'",
            FromUsrCod,ToUsrCod);
   return (DB_QueryCOUNT (Query,"can not check if a user is banned") != 0);
  }

/*****************************************************************************/
/************************ Remove user from banned table **********************/
/*****************************************************************************/

void Msg_RemoveUsrFromBanned (long UsrCod)
  {
   char Query[256];

   /***** Remove pair (sender's code - my code)
          from table of banned senders *****/
   sprintf (Query,"DELETE FROM msg_banned"
                  " WHERE FromUsrCod='%ld' OR ToUsrCod='%ld'",
            UsrCod,UsrCod);
   DB_QueryDELETE (Query,"can not remove user from table of banned users");
  }

/*****************************************************************************/
/*********** List banned users, who can not sent messages to me **************/
/*****************************************************************************/

void Msg_ListBannedUsrs (void)
  {
   extern const char *Txt_You_have_not_banned_any_sender;
   extern const char *Txt_Banned_users;
   extern const char *Txt_Sender_banned_click_to_unban_him;
   char Query[512];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumUsr,NumUsrs;
   struct UsrData UsrDat;
   bool ShowPhoto;
   char PhotoURL[PATH_MAX+1];

   /***** Get my banned users *****/
   sprintf (Query,"SELECT msg_banned.FromUsrCod FROM msg_banned,usr_data"
                  " WHERE msg_banned.ToUsrCod='%ld'"
                  " AND msg_banned.FromUsrCod=usr_data.UsrCod"
                  " ORDER BY usr_data.Surname1,usr_data.Surname2,usr_data.FirstName",
            Gbl.Usrs.Me.UsrDat.UsrCod);
   NumUsrs = (unsigned) DB_QuerySELECT (Query,&mysql_res,"can not get banned users");

   if (NumUsrs == 0)   // If not result ==> sent message is deleted
      Lay_ShowAlert (Lay_INFO,Txt_You_have_not_banned_any_sender);
   else
     {
      /***** Initialize structure with user's data *****/
      Usr_UsrDataConstructor (&UsrDat);

      /***** Start table with list of users *****/
      Lay_StartRoundFrameTable (NULL,Txt_Banned_users,NULL,NULL,2);

      /***** List users *****/
      for (NumUsr = 1;
	   NumUsr <= NumUsrs;
	   NumUsr++)
        {
         row = mysql_fetch_row (mysql_res);

         /* Get user's code (row[0]) */
         UsrDat.UsrCod = Str_ConvertStrCodToLongCod (row[0]);

         /* Get user's data from database */
         if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat))
           {
            /* Put form to unban user */
            fprintf (Gbl.F.Out,"<tr>"
                               "<td class=\"BM\">");
            Act_FormStart (ActUnbUsrLst);
            Usr_PutParamUsrCodEncrypted (UsrDat.EncryptedUsrCod);
            fprintf (Gbl.F.Out,"<input type=\"image\""
        	               " src=\"%s/closed_on16x16.gif\""
        	               " alt=\"%s\" title=\"%s\""
        	               " class=\"ICO20x20\" />",
                     Gbl.Prefs.IconsURL,
                     Txt_Sender_banned_click_to_unban_him,
                     Txt_Sender_banned_click_to_unban_him);
            Act_FormEnd ();
            fprintf (Gbl.F.Out,"</td>");

            /* Show photo */
            fprintf (Gbl.F.Out,"<td class=\"LEFT_MIDDLE\""
        	               " style=\"width:30px;\">");
            ShowPhoto = Pho_ShowUsrPhotoIsAllowed (&UsrDat,PhotoURL);
            Pho_ShowUsrPhoto (&UsrDat,ShowPhoto ? PhotoURL :
                        	                  NULL,
                              "PHOTO21x28",Pho_ZOOM,false);
            fprintf (Gbl.F.Out,"</td>");

            /* Write user's full name */
            fprintf (Gbl.F.Out,"<td class=\"DAT LEFT_MIDDLE\">"
        	               "%s"
        	               "</td>"
                               "</tr>",
                     UsrDat.FullName);
           }
        }

      /***** End of table *****/
      Lay_EndRoundFrameTable ();

      /***** Free memory used for user's data *****/
      Usr_UsrDataDestructor (&UsrDat);
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }
