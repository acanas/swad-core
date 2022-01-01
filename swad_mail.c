// swad_mail.c: everything related to email

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

#define _GNU_SOURCE 		// For asprintf
#include <stddef.h>		// For NULL
#include <stdio.h>		// For asprintf
#include <stdlib.h>		// For calloc
#include <string.h>		// For string functions
#include <sys/wait.h>		// For the macro WEXITSTATUS
#include <unistd.h>		// For access, lstat, getpid, chdir, symlink, unlink

#include "swad_account.h"
#include "swad_box.h"
#include "swad_database.h"
#include "swad_error.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_HTML.h"
#include "swad_language.h"
#include "swad_mail.h"
#include "swad_mail_database.h"
#include "swad_parameter.h"
#include "swad_QR.h"
#include "swad_tab.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/***************************** Private variables *****************************/
/*****************************************************************************/

static const char *Mai_EMAIL_SECTION_ID = "email_section";
static struct Mail *Mai_EditingMai = NULL;	// Static variable to keep the mail domain being edited

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Mai_GetParamMaiOrder (void);
static void Mai_PutIconToEditMailDomains (__attribute__((unused)) void *Args);
static void Mai_EditMailDomainsInternal (void);
static void Mai_GetListMailDomainsAllowedForNotif (void);
static void Mai_GetMailDomain (const char *Email,
                               char MailDomain[Cns_MAX_BYTES_EMAIL_ADDRESS + 1]);

static void Mai_ListMailDomainsForEdition (void);
static void Mai_PutParamMaiCod (void *MaiCod);

static void Mai_RenameMailDomain (Cns_ShrtOrFullName_t ShrtOrFullName);

static void Mai_PutFormToCreateMailDomain (void);
static void Mai_PutHeadMailDomains (void);

static void Mai_PutFormToSelectUsrsToListEmails (__attribute__((unused)) void *Args);
static void Mai_ListEmails (__attribute__((unused)) void *Args);

static void Mai_ShowFormChangeUsrEmail (bool ItsMe,
				        bool IMustFillInEmail,
				        bool IShouldConfirmEmail);
static void Mai_PutParamsRemoveMyEmail (void *Email);
static void Mai_PutParamsRemoveOtherEmail (void *Email);

static void Mai_RemoveEmail (struct UsrData *UsrDat);
static void Mai_NewUsrEmail (struct UsrData *UsrDat,bool ItsMe);
static void Mai_InsertMailKey (const char Email[Cns_MAX_BYTES_EMAIL_ADDRESS + 1],
                               const char MailKey[Mai_LENGTH_EMAIL_CONFIRM_KEY + 1]);

static void Mai_EditingMailDomainConstructor (void);
static void Mai_EditingMailDomainDestructor (void);

/*****************************************************************************/
/*************************** List all mail domains ***************************/
/*****************************************************************************/

void Mai_SeeMailDomains (void)
  {
   extern const char *Hlp_START_Domains;
   extern const char *The_ClassDat[The_NUM_THEMES];
   extern const char *Txt_Email_domains_allowed_for_notifications;
   extern const char *Txt_EMAIL_DOMAIN_HELP_ORDER[3];
   extern const char *Txt_EMAIL_DOMAIN_ORDER[3];
   Mai_DomainsOrder_t Order;
   unsigned NumMai;

   /***** Get parameter with the type of order in the list of mail domains *****/
   Mai_GetParamMaiOrder ();

   /***** Get list of mail domains *****/
   Mai_GetListMailDomainsAllowedForNotif ();

   /***** Begin box and table *****/
   if (Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM)
      Box_BoxTableBegin (NULL,Txt_Email_domains_allowed_for_notifications,
			 Mai_PutIconToEditMailDomains,NULL,
			 Hlp_START_Domains,Box_NOT_CLOSABLE,2);
   else
      Box_BoxTableBegin (NULL,Txt_Email_domains_allowed_for_notifications,
			 NULL,NULL,
			 Hlp_START_Domains,Box_NOT_CLOSABLE,2);

   /***** Write heading *****/
   HTM_TR_Begin (NULL);
      for (Order  = (Mai_DomainsOrder_t) 0;
	   Order <= (Mai_DomainsOrder_t) (Mai_NUM_ORDERS - 1);
	   Order++)
	{
         HTM_TH_TitleBegin (HTM_HEAD_LEFT);

	    Frm_BeginForm (ActSeeMai);
	       Par_PutHiddenParamUnsigned (NULL,"Order",(unsigned) Order);
	       HTM_BUTTON_OnSubmit_Begin (Txt_EMAIL_DOMAIN_HELP_ORDER[Order],
	                                  "BT_LINK",NULL);
		  if (Order == Gbl.Mails.SelectedOrder)
		     HTM_U_Begin ();
		  HTM_Txt (Txt_EMAIL_DOMAIN_ORDER[Order]);
		  if (Order == Gbl.Mails.SelectedOrder)
		     HTM_U_End ();
	       HTM_BUTTON_End ();
	    Frm_EndForm ();

	 HTM_TH_End ();
	}
   HTM_TR_End ();

   /***** Write all mail domains *****/
   for (NumMai = 0;
	NumMai < Gbl.Mails.Num;
	NumMai++)
     {
      /* Write data of this mail domain */
      HTM_TR_Begin (NULL);

	 HTM_TD_Begin ("class=\"%s LT\"",The_ClassDat[Gbl.Prefs.Theme]);
	    HTM_Txt (Gbl.Mails.Lst[NumMai].Domain);
	 HTM_TD_End ();

	 HTM_TD_Begin ("class=\"%s LT\"",The_ClassDat[Gbl.Prefs.Theme]);
	    HTM_Txt (Gbl.Mails.Lst[NumMai].Info);
	 HTM_TD_End ();

	 HTM_TD_Begin ("class=\"%s RT\"",The_ClassDat[Gbl.Prefs.Theme]);
	    HTM_Unsigned (Gbl.Mails.Lst[NumMai].NumUsrs);
	 HTM_TD_End ();

      HTM_TR_End ();
     }

   /***** End table and box *****/
   Box_BoxTableEnd ();

   /***** Free list of mail domains *****/
   Mai_FreeListMailDomains ();
  }

/*****************************************************************************/
/******* Get parameter with the type or order in list of mail domains ********/
/*****************************************************************************/

static void Mai_GetParamMaiOrder (void)
  {
   Gbl.Mails.SelectedOrder = (Mai_DomainsOrder_t)
	                     Par_GetParToUnsignedLong ("Order",
	                                               0,
	                                               Mai_NUM_ORDERS - 1,
	                                               (unsigned long) Mai_ORDER_DEFAULT);
  }

/*****************************************************************************/
/************************ Put icon to edit mail domains **********************/
/*****************************************************************************/

static void Mai_PutIconToEditMailDomains (__attribute__((unused)) void *Args)
  {
   Ico_PutContextualIconToEdit (ActEdiMai,NULL,
				NULL,NULL);
  }

/*****************************************************************************/
/*********************** Put forms to edit mail domains **********************/
/*****************************************************************************/

void Mai_EditMailDomains (void)
  {
   /***** Mail domain constructor *****/
   Mai_EditingMailDomainConstructor ();

   /***** Edit mail domains *****/
   Mai_EditMailDomainsInternal ();

   /***** Mail domain destructor *****/
   Mai_EditingMailDomainDestructor ();
  }

static void Mai_EditMailDomainsInternal (void)
  {
   /***** Get list of mail domains *****/
   Mai_GetListMailDomainsAllowedForNotif ();

   /***** Put a form to create a new mail *****/
   Mai_PutFormToCreateMailDomain ();

   /***** Forms to edit current mail domains *****/
   if (Gbl.Mails.Num)
      Mai_ListMailDomainsForEdition ();

   /***** Free list of mail domains *****/
   Mai_FreeListMailDomains ();
  }

/*****************************************************************************/
/*************************** List all mail domains ***************************/
/*****************************************************************************/

static void Mai_GetListMailDomainsAllowedForNotif (void)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumMai;
   struct Mail *Mai;

   // Query uses temporary tables for speed
   // Query uses two identical temporary tables...
   // ...because a unique temporary table can not be used twice in the same query

   /***** Create temporary table with all mail domains present in users' emails table *****/
   Mai_DB_RemoveTmpTables ();

   Mai_DB_CreateTmpTables ();

   /***** Get mail domains from database *****/
   if ((Gbl.Mails.Num = Mai_DB_GetMailDomains (&mysql_res))) // Mail domains found...
     {
      /***** Create list with places *****/
      if ((Gbl.Mails.Lst = calloc ((size_t) Gbl.Mails.Num,
                                   sizeof (*Gbl.Mails.Lst))) == NULL)
          Err_NotEnoughMemoryExit ();

      /***** Get the mail domains *****/
      for (NumMai = 0;
	   NumMai < Gbl.Mails.Num;
	   NumMai++)
        {
         Mai = &(Gbl.Mails.Lst[NumMai]);

         /* Get next mail */
         row = mysql_fetch_row (mysql_res);

         /* Get mail code (row[0]) */
         if ((Mai->MaiCod = Str_ConvertStrCodToLongCod (row[0])) <= 0)
            Err_WrongMailDomainExit ();

         /* Get the mail domain (row[1]) and the mail domain info (row[2]) */
         Str_Copy (Mai->Domain,row[1],sizeof (Mai->Domain) - 1);
         Str_Copy (Mai->Info  ,row[2],sizeof (Mai->Info  ) - 1);

         /* Get number of users (row[3]) */
         if (sscanf (row[3],"%u",&(Mai->NumUsrs)) != 1)
            Mai->NumUsrs = 0;
        }
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   /***** Drop temporary table *****/
   Mai_DB_RemoveTmpTables ();
  }

/*****************************************************************************/
/************ Check if user can receive notifications via email **************/
/*****************************************************************************/

bool Mai_CheckIfUsrCanReceiveEmailNotif (const struct UsrData *UsrDat)
  {
   char MailDomain[Cns_MAX_BYTES_EMAIL_ADDRESS + 1];

   /***** Check #1: is my email address confirmed? *****/
   if (!UsrDat->EmailConfirmed)
      return false;

   /***** Check #2: if my mail domain allowed? *****/
   Mai_GetMailDomain (UsrDat->Email,MailDomain);
   return Mai_DB_CheckIfMailDomainIsAllowedForNotif (MailDomain);
  }

/*****************************************************************************/
/********************** Get mailbox from email address ***********************/
/*****************************************************************************/

static void Mai_GetMailDomain (const char *Email,
                               char MailDomain[Cns_MAX_BYTES_EMAIL_ADDRESS + 1])
  {
   const char *Ptr;

   MailDomain[0] = '\0';	// Return empty mailbox on error

   if ((Ptr = strchr (Email,(int) '@')))	// Find first '@' in address
      if (Ptr != Email)				// '@' is not the first character in Email
        {
         Ptr++;					// Skip '@'
         if (strchr (Ptr,(int) '@') == NULL)	// No more '@' found
            Str_Copy (MailDomain,Ptr,Cns_MAX_BYTES_EMAIL_ADDRESS);
        }
  }

/*****************************************************************************/
/***************** Show warning about notifications via email ****************/
/*****************************************************************************/

void Mai_WriteWarningEmailNotifications (void)
  {
   extern const char *Txt_You_can_only_receive_email_notifications_if_;
   extern const char *Txt_TABS_TXT[Tab_NUM_TABS];
   extern const char *Txt_MENU_TITLE[Tab_NUM_TABS][Act_MAX_OPTIONS_IN_MENU_PER_TAB];
   extern const char *Txt_Domains;
   Tab_Tab_t TabMyAccount   = Act_GetTab (ActFrmMyAcc  );
   Tab_Tab_t TabMailDomains = Act_GetTab (ActSeeMai);

   Ale_ShowAlert (Ale_WARNING,Txt_You_can_only_receive_email_notifications_if_,
	          Txt_TABS_TXT  [TabMyAccount  ],
	          Txt_MENU_TITLE[TabMyAccount  ][Act_GetIndexInMenu (ActFrmMyAcc)],
                  Txt_TABS_TXT  [TabMailDomains],
	          Txt_MENU_TITLE[TabMailDomains][Act_GetIndexInMenu (ActSeeMai  )],
	          Txt_Domains);
  }

/*****************************************************************************/
/**************************** Get mail domain data ***************************/
/*****************************************************************************/

void Mai_GetDataOfMailDomainByCod (struct Mail *Mai)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;

   /***** Clear data *****/
   Mai->Domain[0] = Mai->Info[0] = '\0';

   /***** Check if mail code is correct *****/
   if (Mai->MaiCod > 0)
     {
      /***** Get data of a mail domain from database *****/
      if (Mai_DB_GetDataOfMailDomainByCod (&mysql_res,Mai->MaiCod)) // Mail found...
        {
         /* Get row */
         row = mysql_fetch_row (mysql_res);

         /* Get the short & full name of the mail (row[0], row[1]) */
         Str_Copy (Mai->Domain,row[0],sizeof (Mai->Domain) - 1);
         Str_Copy (Mai->Info  ,row[1],sizeof (Mai->Info  ) - 1);
        }

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);
     }
  }

/*****************************************************************************/
/************************** Free list of mail domains ************************/
/*****************************************************************************/

void Mai_FreeListMailDomains (void)
  {
   if (Gbl.Mails.Lst)
     {
      /***** Free memory used by the list of mail domains *****/
      free (Gbl.Mails.Lst);
      Gbl.Mails.Lst = NULL;
      Gbl.Mails.Num = 0;
     }
  }

/*****************************************************************************/
/************************** List all mail domains ****************************/
/*****************************************************************************/

static void Mai_ListMailDomainsForEdition (void)
  {
   extern const char *Hlp_START_Domains_edit;
   extern const char *The_ClassDat[The_NUM_THEMES];
   extern const char *Txt_Email_domains_allowed_for_notifications;
   unsigned NumMai;
   struct Mail *Mai;

   /***** Begin box and table *****/
   Box_BoxTableBegin (NULL,Txt_Email_domains_allowed_for_notifications,
                      NULL,NULL,
                      Hlp_START_Domains_edit,Box_NOT_CLOSABLE,2);

      /***** Write heading *****/
      Mai_PutHeadMailDomains ();

      /***** Write all mail domains *****/
      for (NumMai = 0;
	   NumMai < Gbl.Mails.Num;
	   NumMai++)
	{
	 Mai = &Gbl.Mails.Lst[NumMai];

	 HTM_TR_Begin (NULL);

	    /* Put icon to remove mail */
	    HTM_TD_Begin ("class=\"BM\"");
	       Ico_PutContextualIconToRemove (ActRemMai,NULL,
					      Mai_PutParamMaiCod,&Mai->MaiCod);
	    HTM_TD_End ();

	    /* Mail code */
	    HTM_TD_Begin ("class=\"%s RM\"",The_ClassDat[Gbl.Prefs.Theme]);
	       HTM_Long (Mai->MaiCod);
	    HTM_TD_End ();

	    /* Mail domain */
	    HTM_TD_Begin ("class=\"CM\"");
	       Frm_BeginForm (ActRenMaiSho);
		  Mai_PutParamMaiCod (&Mai->MaiCod);
		  HTM_INPUT_TEXT ("Domain",Cns_MAX_CHARS_EMAIL_ADDRESS,Mai->Domain,
				  HTM_SUBMIT_ON_CHANGE,
				  "size=\"15\"");
	       Frm_EndForm ();
	    HTM_TD_End ();

	    /* Mail domain info */
	    HTM_TD_Begin ("class=\"CM\"");
	       Frm_BeginForm (ActRenMaiFul);
		  Mai_PutParamMaiCod (&Mai->MaiCod);
		  HTM_INPUT_TEXT ("Info",Mai_MAX_CHARS_MAIL_INFO,Mai->Info,
				  HTM_SUBMIT_ON_CHANGE,
				  "size=\"40\"");
	       Frm_EndForm ();
	    HTM_TD_End ();

	    /* Number of users */
	    HTM_TD_Begin ("class=\"%s RM\"",The_ClassDat[Gbl.Prefs.Theme]);
	       HTM_Unsigned (Mai->NumUsrs);
	    HTM_TD_End ();

	 HTM_TR_End ();
	}

   /***** End table and box *****/
   Box_BoxTableEnd ();
  }

/*****************************************************************************/
/******************** Write parameter with code of mail **********************/
/*****************************************************************************/

static void Mai_PutParamMaiCod (void *MaiCod)
  {
   if (MaiCod)
      Par_PutHiddenParamLong (NULL,"MaiCod",*((long *) MaiCod));
  }

/*****************************************************************************/
/*********************** Get parameter with code of mail *********************/
/*****************************************************************************/

long Mai_GetParamMaiCod (void)
  {
   /***** Get code of mail *****/
   return Par_GetParToLong ("MaiCod");
  }

/*****************************************************************************/
/******************************* Remove a mail *******************************/
/*****************************************************************************/

void Mai_RemoveMailDomain (void)
  {
   extern const char *Txt_Email_domain_X_removed;

   /***** Mail domain constructor *****/
   Mai_EditingMailDomainConstructor ();

   /***** Get mail domain code *****/
   if ((Mai_EditingMai->MaiCod = Mai_GetParamMaiCod ()) <= 0)
      Err_WrongMailDomainExit ();

   /***** Get data of the mail domain rom database *****/
   Mai_GetDataOfMailDomainByCod (Mai_EditingMai);

   /***** Remove mail domain *****/
   Mai_DB_RemoveMailDomain (Mai_EditingMai->MaiCod);

   /***** Write message to show the change made *****/
   Ale_CreateAlert (Ale_SUCCESS,NULL,
	            Txt_Email_domain_X_removed,
                    Mai_EditingMai->Domain);
  }

/*****************************************************************************/
/********************* Change the short name of a mail ***********************/
/*****************************************************************************/

void Mai_RenameMailDomainShort (void)
  {
   /***** Mail domain constructor *****/
   Mai_EditingMailDomainConstructor ();

   /***** Rename mail domain *****/
   Mai_RenameMailDomain (Cns_SHRT_NAME);
  }

/*****************************************************************************/
/********************* Change the full name of a mail ************************/
/*****************************************************************************/

void Mai_RenameMailDomainFull (void)
  {
   /***** Mail domain constructor *****/
   Mai_EditingMailDomainConstructor ();

   /***** Rename mail domain *****/
   Mai_RenameMailDomain (Cns_FULL_NAME);
  }

/*****************************************************************************/
/************************* Change the name of a mail *************************/
/*****************************************************************************/

static void Mai_RenameMailDomain (Cns_ShrtOrFullName_t ShrtOrFullName)
  {
   extern const char *Txt_The_email_domain_X_already_exists;
   extern const char *Txt_The_email_domain_X_has_been_renamed_as_Y;
   extern const char *Txt_The_email_domain_X_has_not_changed;
   const char *ParamName = NULL;	// Initialized to avoid warning
   const char *FieldName = NULL;	// Initialized to avoid warning
   unsigned MaxBytes = 0;		// Initialized to avoid warning
   char *CurrentMaiName = NULL;		// Initialized to avoid warning
   char NewMaiName[Mai_MAX_BYTES_MAIL_INFO + 1];

   switch (ShrtOrFullName)
     {
      case Cns_SHRT_NAME:
         ParamName = "Domain";
         FieldName = "Domain";
         MaxBytes = Cns_MAX_BYTES_EMAIL_ADDRESS;
         CurrentMaiName = Mai_EditingMai->Domain;
         break;
      case Cns_FULL_NAME:
         ParamName = "Info";
         FieldName = "Info";
         MaxBytes = Mai_MAX_BYTES_MAIL_INFO;
         CurrentMaiName = Mai_EditingMai->Info;
         break;
     }

   /***** Get parameters from form *****/
   /* Get the code of the mail */
   if ((Mai_EditingMai->MaiCod = Mai_GetParamMaiCod ()) <= 0)
      Err_WrongMailDomainExit ();

   /* Get the new name for the mail */
   Par_GetParToText (ParamName,NewMaiName,MaxBytes);

   /***** Get from the database the old names of the mail *****/
   Mai_GetDataOfMailDomainByCod (Mai_EditingMai);

   /***** Check if new name is empty *****/
   if (NewMaiName[0])
     {
      /***** Check if old and new names are the same
             (this happens when return is pressed without changes) *****/
      if (strcmp (CurrentMaiName,NewMaiName))	// Different names
        {
         /***** If mail was in database... *****/
         if (Mai_DB_CheckIfMailDomainNameExists (ParamName,NewMaiName,Mai_EditingMai->MaiCod))
	    Ale_CreateAlert (Ale_WARNING,Mai_EMAIL_SECTION_ID,
		             Txt_The_email_domain_X_already_exists,
                             NewMaiName);
         else
           {
            /* Update the table changing old name by new name */
            Mai_DB_UpdateMailDomainName (Mai_EditingMai->MaiCod,FieldName,NewMaiName);

            /* Write message to show the change made */
	    Ale_CreateAlert (Ale_SUCCESS,Mai_EMAIL_SECTION_ID,
		             Txt_The_email_domain_X_has_been_renamed_as_Y,
                             CurrentMaiName,NewMaiName);
           }
        }
      else	// The same name
	 Ale_CreateAlert (Ale_INFO,Mai_EMAIL_SECTION_ID,
	                  Txt_The_email_domain_X_has_not_changed,
                          CurrentMaiName);
     }
   else
      Ale_CreateAlertYouCanNotLeaveFieldEmpty ();

   /***** Update name *****/
   Str_Copy (CurrentMaiName,NewMaiName,MaxBytes);
  }

/*****************************************************************************/
/******* Show alerts after changing a mail domain and continue editing *******/
/*****************************************************************************/

void Mai_ContEditAfterChgMai (void)
  {
   /***** Write message to show the change made *****/
   Ale_ShowAlerts (NULL);

   /***** Show the form again *****/
   Mai_EditMailDomainsInternal ();

   /***** Mail domain destructor *****/
   Mai_EditingMailDomainDestructor ();
  }

/*****************************************************************************/
/*********************** Put a form to create a new mail *********************/
/*****************************************************************************/

static void Mai_PutFormToCreateMailDomain (void)
  {
   extern const char *Hlp_START_Domains_edit;
   extern const char *Txt_New_email_domain;
   extern const char *Txt_EMAIL_DOMAIN_ORDER[3];
   extern const char *Txt_Create_email_domain;

   /***** Begin form *****/
   Frm_BeginForm (ActNewMai);

      /***** Begin box and table *****/
      Box_BoxTableBegin (NULL,Txt_New_email_domain,
			 NULL,NULL,
			 Hlp_START_Domains_edit,Box_NOT_CLOSABLE,2);

	 /***** Write heading *****/
	 HTM_TR_Begin (NULL);
	    HTM_TH (1,1,Txt_EMAIL_DOMAIN_ORDER[Mai_ORDER_BY_DOMAIN],"LM");
	    HTM_TH (1,1,Txt_EMAIL_DOMAIN_ORDER[Mai_ORDER_BY_INFO]  ,"LM");
	 HTM_TR_End ();

	 /***** Second row *****/
	 HTM_TR_Begin (NULL);

	    /* Mail domain */
	    HTM_TD_Begin ("class=\"CM\"");
	       HTM_INPUT_TEXT ("Domain",Cns_MAX_CHARS_EMAIL_ADDRESS,Mai_EditingMai->Domain,
			       HTM_DONT_SUBMIT_ON_CHANGE,
			       "size=\"15\" required=\"required\"");
	    HTM_TD_End ();

	    /* Mail domain info */
	    HTM_TD_Begin ("class=\"CM\"");
	       HTM_INPUT_TEXT ("Info",Mai_MAX_CHARS_MAIL_INFO,Mai_EditingMai->Info,
			       HTM_DONT_SUBMIT_ON_CHANGE,
			       "size=\"40\" required=\"required\"");
	    HTM_TD_End ();

	    HTM_TD_Empty (1);

	 HTM_TR_End ();

      /***** End table, send button and end box *****/
      Box_BoxTableWithButtonEnd (Btn_CREATE_BUTTON,Txt_Create_email_domain);

   /***** End form *****/
   Frm_EndForm ();
  }

/*****************************************************************************/
/********************* Write header with fields of a mail ********************/
/*****************************************************************************/

static void Mai_PutHeadMailDomains (void)
  {
   extern const char *Txt_Code;
   extern const char *Txt_EMAIL_DOMAIN_ORDER[3];

   HTM_TR_Begin (NULL);
      HTM_TH (1,1,NULL                                       ,"BM");
      HTM_TH (1,1,Txt_Code                                   ,"RM");
      HTM_TH (1,1,Txt_EMAIL_DOMAIN_ORDER[Mai_ORDER_BY_DOMAIN],"LM");
      HTM_TH (1,1,Txt_EMAIL_DOMAIN_ORDER[Mai_ORDER_BY_INFO  ],"LM");
      HTM_TH (1,1,Txt_EMAIL_DOMAIN_ORDER[Mai_ORDER_BY_USERS ],"RM");
   HTM_TR_End ();
  }

/*****************************************************************************/
/******************* Receive form to create a new mail ***********************/
/*****************************************************************************/

void Mai_ReceiveFormNewMailDomain (void)
  {
   extern const char *Txt_The_email_domain_X_already_exists;
   extern const char *Txt_Created_new_email_domain_X;
   extern const char *Txt_You_must_specify_the_short_name_and_the_full_name_of_the_new_email_domain;

   /***** Mail domain constructor *****/
   Mai_EditingMailDomainConstructor ();

   /***** Get parameters from form *****/
   /* Get mail short name */
   Par_GetParToText ("Domain",Mai_EditingMai->Domain,Cns_MAX_BYTES_EMAIL_ADDRESS);

   /* Get mail full name */
   Par_GetParToText ("Info",Mai_EditingMai->Info,Mai_MAX_BYTES_MAIL_INFO);

   if (Mai_EditingMai->Domain[0] &&
       Mai_EditingMai->Info[0])	// If there's a mail name
     {
      /***** If name of mail was in database... *****/
      if (Mai_DB_CheckIfMailDomainNameExists ("Domain",Mai_EditingMai->Domain,-1L))
         Ale_CreateAlert (Ale_WARNING,NULL,
                          Txt_The_email_domain_X_already_exists,
                          Mai_EditingMai->Domain);
      else if (Mai_DB_CheckIfMailDomainNameExists ("Info",Mai_EditingMai->Info,-1L))
         Ale_CreateAlert (Ale_WARNING,NULL,
                          Txt_The_email_domain_X_already_exists,
                          Mai_EditingMai->Info);
      else	// Add new mail to database
        {
         Mai_DB_CreateMailDomain (Mai_EditingMai);
	 Ale_ShowAlert (Ale_SUCCESS,Txt_Created_new_email_domain_X,
			Mai_EditingMai->Domain);
        }
     }
   else	// If there is not a mail name
      Ale_CreateAlert (Ale_WARNING,NULL,
	               Txt_You_must_specify_the_short_name_and_the_full_name_of_the_new_email_domain);
  }

/*****************************************************************************/
/************************ Request users to list emails ***********************/
/*****************************************************************************/

void Mai_ReqUsrsToListEmails (void)
  {
   Mai_PutFormToSelectUsrsToListEmails (NULL);
  }

static void Mai_PutFormToSelectUsrsToListEmails (__attribute__((unused)) void *Args)
  {
   extern const char *Hlp_COMMUNICATION_Email;
   extern const char *Txt_Email;
   extern const char *Txt_View_email_addresses;

   /***** List users to select some of them *****/
   Usr_PutFormToSelectUsrsToGoToAct (&Gbl.Usrs.Selected,
				     ActMaiUsr,
				     NULL,NULL,
				     Txt_Email,
				     Hlp_COMMUNICATION_Email,
				     Txt_View_email_addresses,
				     false);	// Do not put form with date range
  }

/*****************************************************************************/
/****** Get and check list of selected users, and show users' emails  ********/
/*****************************************************************************/

void Mai_GetSelUsrsAndListEmails (void)
  {
   Usr_GetSelectedUsrsAndGoToAct (&Gbl.Usrs.Selected,
				  Mai_ListEmails,NULL,				// when user(s) selected
                                  Mai_PutFormToSelectUsrsToListEmails,NULL);	// when no user selected
  }

/*****************************************************************************/
/******** List the emails of all students to creates an email message ********/
/*****************************************************************************/

#define Mai_MAX_BYTES_STR_ADDR (256 * 1024 - 1)

static void Mai_ListEmails (__attribute__((unused)) void *Args)
  {
   extern const char *Hlp_COMMUNICATION_Email;
   extern const char *The_ClassFormOutBoxBold[The_NUM_THEMES];
   extern const char *The_ClassDat[The_NUM_THEMES];
   extern const char *Txt_Email_addresses;
   extern const char *Txt_X_users_who_have_email;
   extern const char *Txt_X_users_who_have_accepted_and_who_have_email;
   extern const char *Txt_Create_email_message;
   unsigned NumUsrsWithEmail = 0;
   unsigned NumAcceptedUsrsWithEmail = 0;
   char StrAddresses[Mai_MAX_BYTES_STR_ADDR + 1];	// TODO: Use malloc depending on the number of students
   unsigned int LengthStrAddr = 0;
   struct UsrData UsrDat;
   const char *Ptr;

   /***** Begin the box used to list the emails *****/
   Box_BoxBegin (NULL,Txt_Email_addresses,
                 NULL,NULL,
		 Hlp_COMMUNICATION_Email,Box_NOT_CLOSABLE);

      /***** Begin list with users' email addresses *****/
      HTM_DIV_Begin ("class=\"DAT_SMALL CM\"");

	 /***** Initialize structure with user's data *****/
	 Usr_UsrDataConstructor (&UsrDat);

	 /***** Get email addresses of the selected users *****/
	 StrAddresses[0] = '\0';
	 Ptr = Gbl.Usrs.Selected.List[Rol_UNK];
	 while (*Ptr)
	   {
	    /* Get next user */
	    Par_GetNextStrUntilSeparParamMult (&Ptr,UsrDat.EnUsrCod,
					       Cry_BYTES_ENCRYPTED_STR_SHA256_BASE64);
	    Usr_GetUsrCodFromEncryptedUsrCod (&UsrDat);

	    /* Get user's email */
	    Mai_GetEmailFromUsrCod (&UsrDat);

	    if (UsrDat.Email[0])
	      {
	       NumUsrsWithEmail++;

	       /* Check if users has accepted inscription in current course */
	       UsrDat.Accepted = Enr_CheckIfUsrHasAcceptedInCurrentCrs (&UsrDat);

	       if (UsrDat.Accepted) // If student has email and has accepted
		 {
		  if (NumAcceptedUsrsWithEmail > 0)
		    {
		     HTM_Txt (", ");
		     LengthStrAddr ++;
		     if (LengthStrAddr > Mai_MAX_BYTES_STR_ADDR)
			Err_ShowErrorAndExit ("The space allocated to store email addresses is full.");
		     Str_Concat (StrAddresses,",",sizeof (StrAddresses) - 1);
		    }
		  LengthStrAddr += strlen (UsrDat.Email);
		  if (LengthStrAddr > Mai_MAX_BYTES_STR_ADDR)
		     Err_ShowErrorAndExit ("The space allocated to store email addresses is full.");
		  Str_Concat (StrAddresses,UsrDat.Email,sizeof (StrAddresses) - 1);
		  HTM_A_Begin ("href=\"mailto:%s?subject=%s\"",
			       UsrDat.Email,Gbl.Hierarchy.Crs.FullName);
		     HTM_Txt (UsrDat.Email);
		  HTM_A_End ();

		  NumAcceptedUsrsWithEmail++;
		 }
	      }
	   }

	 /***** Free memory used for user's data *****/
	 Usr_UsrDataDestructor (&UsrDat);

      /***** End list with users' email addresses *****/
      HTM_DIV_End ();

      /***** Show a message with the number of users with email ****/
      HTM_DIV_Begin ("class=\"%s CM\"",The_ClassDat[Gbl.Prefs.Theme]);
	 HTM_TxtF (Txt_X_users_who_have_email,NumUsrsWithEmail);
      HTM_DIV_End ();

      /***** Show a message with the number of users who have accepted and have email ****/
      HTM_DIV_Begin ("class=\"%s CM\"",The_ClassDat[Gbl.Prefs.Theme]);
	 HTM_TxtF (Txt_X_users_who_have_accepted_and_who_have_email,
		   NumAcceptedUsrsWithEmail);
      HTM_DIV_End ();

      /***** Contextual menu *****/
      Mnu_ContextMenuBegin ();

	 /* Open the client email program */
	 HTM_A_Begin ("href=\"mailto:%s?subject=%s&cc=%s&bcc=%s\""
		      " title=\"%s\" class=\"%s\"",
		      Gbl.Usrs.Me.UsrDat.Email,
		      Gbl.Hierarchy.Crs.FullName,
		      Gbl.Usrs.Me.UsrDat.Email,
		      StrAddresses,
		      Txt_Create_email_message,
		      The_ClassFormOutBoxBold[Gbl.Prefs.Theme]);
	    Ico_PutIconTextLink ("marker.svg",Ico_BLACK,
				 Txt_Create_email_message);
	 HTM_A_End ();

      Mnu_ContextMenuEnd ();

   /***** End the box used to list the emails *****/
   Box_BoxEnd ();
  }

/*****************************************************************************/
/**************** Check whether an email address if valid ********************/
/*****************************************************************************/

bool Mai_CheckIfEmailIsValid (const char *Email)
  {
   unsigned Length = strlen (Email);
   unsigned LastPosArroba = Length - 4;
   const char *Ptr;
   unsigned Pos;
   bool ArrobaFound = false;

   /***** An email address must have a number of characters
          5 <= Length <= Cns_MAX_BYTES_EMAIL_ADDRESS *****/
   if (Length < 5 ||
       Length > Cns_MAX_BYTES_EMAIL_ADDRESS)
      return false;

   /***** An email address can have digits, letters, '.', '-' and '_';
          and must have one and only '@' (not in any position) *****/
   for (Ptr = Email, Pos = 0;
        *Ptr;
        Ptr++)
      if ((*Ptr >= 'a' && *Ptr <= 'z') ||
          (*Ptr >= 'A' && *Ptr <= 'Z') ||
          (*Ptr >= '0' && *Ptr <= '9') ||
          *Ptr == '.' ||
          *Ptr == '-' ||
          *Ptr == '_')
	 Pos++;
      else if (*Ptr == '@')
	{
	 if (ArrobaFound)
	    return false;
	 /* Example: a@b.c
	             01234
	             Length = 5
	             LastPosArroba = 5 - 4 = 1 */
	 if (Pos == 0 || Pos > LastPosArroba)
	    return false;
	 ArrobaFound = true;
	}
      else
         return false;

   return ArrobaFound;
  }

/*****************************************************************************/
/********** Get email address of a user from his/her user's code *************/
/*****************************************************************************/

bool Mai_GetEmailFromUsrCod (struct UsrData *UsrDat)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   bool Found;

   /***** Get current (last updated) user's nickname from database *****/
   if (Mai_DB_GetEmailFromUsrCod (&mysql_res,UsrDat->UsrCod))
     {
      /* Get email */
      row = mysql_fetch_row (mysql_res);
      Str_Copy (UsrDat->Email,row[0],sizeof (UsrDat->Email) - 1);
      UsrDat->EmailConfirmed = (row[1][0] == 'Y');
      Found = true;
     }
   else
     {
      UsrDat->Email[0] = '\0';
      UsrDat->EmailConfirmed = false;
      Found = false;
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return Found;
  }

/*****************************************************************************/
/*********************** Show form to change my email ************************/
/*****************************************************************************/

void Mai_ShowFormChangeMyEmail (bool IMustFillInEmail,bool IShouldConfirmEmail)
  {
   extern const char *Hlp_PROFILE_Account;
   extern const char *Txt_Email;
   char StrRecordWidth[Cns_MAX_DECIMAL_DIGITS_UINT + 2 + 1];

   /***** Begin section *****/
   HTM_SECTION_Begin (Mai_EMAIL_SECTION_ID);

      /***** Begin box *****/
      snprintf (StrRecordWidth,sizeof (StrRecordWidth),"%upx",Rec_RECORD_WIDTH);
      Box_BoxBegin (StrRecordWidth,Txt_Email,
		    Acc_PutLinkToRemoveMyAccount,NULL,
		    Hlp_PROFILE_Account,Box_NOT_CLOSABLE);

	 /***** Show form to change email *****/
	 Mai_ShowFormChangeUsrEmail (true,	// ItsMe
				     IMustFillInEmail,
				     IShouldConfirmEmail);

      /***** End box *****/
      Box_BoxEnd ();

   /***** End section *****/
   HTM_SECTION_End ();
  }

/*****************************************************************************/
/****************** Show form to change another user's email *****************/
/*****************************************************************************/

void Mai_ShowFormChangeOtherUsrEmail (void)
  {
   extern const char *Hlp_PROFILE_Account;
   extern const char *Txt_Email;
   char StrRecordWidth[Cns_MAX_DECIMAL_DIGITS_UINT + 2 + 1];

   /***** Begin section *****/
   HTM_SECTION_Begin (Mai_EMAIL_SECTION_ID);

      /***** Begin box *****/
      snprintf (StrRecordWidth,sizeof (StrRecordWidth),"%upx",Rec_RECORD_WIDTH);
      Box_BoxBegin (StrRecordWidth,Txt_Email,
		    NULL,NULL,
		    Hlp_PROFILE_Account,Box_NOT_CLOSABLE);

	 /***** Show form to change email *****/
	 Mai_ShowFormChangeUsrEmail (false,	// ItsMe
				     false,	// IMustFillInEmail
				     false);	// IShouldConfirmEmail

      /***** End box *****/
      Box_BoxEnd ();

   /***** End section *****/
   HTM_SECTION_End ();
  }

/*****************************************************************************/
/********************** Show form to change user's email *********************/
/*****************************************************************************/

static void Mai_ShowFormChangeUsrEmail (bool ItsMe,
				        bool IMustFillInEmail,
				        bool IShouldConfirmEmail)
  {
   extern const char *The_ClassDat[The_NUM_THEMES];
   extern const char *The_ClassInput[The_NUM_THEMES];
   extern const char *Txt_Before_going_to_any_other_option_you_must_fill_in_your_email_address;
   extern const char *Txt_Please_confirm_your_email_address;
   extern const char *Txt_Current_email;
   extern const char *Txt_Other_emails;
   extern const char *Txt_Email_X_confirmed;
   extern const char *Txt_Confirm_email;
   extern const char *Txt_Use_this_email;
   extern const char *Txt_New_email;
   extern const char *Txt_Email;
   extern const char *Txt_Change_email;
   extern const char *Txt_Save_changes;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumEmails;
   unsigned NumEmail;
   bool Confirmed;
   char *Icon;
   static const struct
     {
      Act_Action_t Remove;
      Act_Action_t New;
     } NextAction[Rol_NUM_ROLES] =
     {
      [Rol_UNK	  ] = {ActRemMaiOth,ActNewMaiOth},
      [Rol_GST	  ] = {ActRemMaiOth,ActNewMaiOth},
      [Rol_USR    ] = {ActRemMaiOth,ActNewMaiOth},
      [Rol_STD	  ] = {ActRemMaiStd,ActNewMaiStd},
      [Rol_NET	  ] = {ActRemMaiTch,ActNewMaiTch},
      [Rol_TCH	  ] = {ActRemMaiTch,ActNewMaiTch},
      [Rol_DEG_ADM] = {ActRemMaiOth,ActNewMaiOth},
      [Rol_CTR_ADM] = {ActRemMaiOth,ActNewMaiOth},
      [Rol_INS_ADM] = {ActRemMaiOth,ActNewMaiOth},
      [Rol_SYS_ADM] = {ActRemMaiOth,ActNewMaiOth},
     };
   const struct UsrData *UsrDat = (ItsMe ? &Gbl.Usrs.Me.UsrDat :
	                                   &Gbl.Usrs.Other.UsrDat);

   /***** Show possible alerts *****/
   Ale_ShowAlerts (Mai_EMAIL_SECTION_ID);

   /***** Help message *****/
   if (IMustFillInEmail)
      Ale_ShowAlert (Ale_WARNING,Txt_Before_going_to_any_other_option_you_must_fill_in_your_email_address);
   else if (IShouldConfirmEmail)
      Ale_ShowAlert (Ale_WARNING,Txt_Please_confirm_your_email_address);

   /***** Get my emails *****/
   NumEmails = Mai_DB_GetMyEmails (&mysql_res,UsrDat->UsrCod);

   /***** Begin table *****/
   HTM_TABLE_BeginWidePadding (2);

      /***** List emails *****/
      for (NumEmail  = 1;
	   NumEmail <= NumEmails;
	   NumEmail++)
	{
	 /* Get email */
	 row = mysql_fetch_row (mysql_res);
	 Confirmed = (row[1][0] == 'Y');

	 if (NumEmail == 1)
	   {
	    HTM_TR_Begin (NULL);

	       /* The first mail is the current one */
	       /* Label */
	       Frm_LabelColumn ("REC_C1_BOT RT",NULL,Txt_Current_email);

	       /* Data */
	       HTM_TD_Begin ("class=\"REC_C2_BOT LT USR_ID\"");
	   }
	 else if (NumEmail == 2)
	   {
	    HTM_TR_Begin (NULL);

	       /* Label */
	       Frm_LabelColumn ("REC_C1_BOT RT",NULL,Txt_Other_emails);

	       /* Data */
	       HTM_TD_Begin ("class=\"REC_C2_BOT LT %s\"",
	                     The_ClassDat[Gbl.Prefs.Theme]);
	   }

	 /* Form to remove email */
	 if (ItsMe)
	    Ico_PutContextualIconToRemove (ActRemMyMai,Mai_EMAIL_SECTION_ID,
					   Mai_PutParamsRemoveMyEmail,row[0]);
	 else
	    Ico_PutContextualIconToRemove (NextAction[UsrDat->Roles.InCurrentCrs].Remove,Mai_EMAIL_SECTION_ID,
					   Mai_PutParamsRemoveOtherEmail,row[0]);

	 /* Email */
	 HTM_Txt (row[0]);

	 /* Email confirmed? */
	 if (Confirmed)
	   {
	    if (asprintf (&Icon,Txt_Email_X_confirmed,row[0]) < 0)
	       Err_NotEnoughMemoryExit ();
	    Ico_PutIcon ("check-circle.svg",Ico_GREEN,Icon,"ICO16x16");
	    free (Icon);
	   }

	 /* Form to change user's email */
	 if (NumEmail > 1 || (ItsMe && !Confirmed))
	   {
	    HTM_BR ();
	    if (ItsMe)
	       Frm_BeginFormAnchor (ActChgMyMai,Mai_EMAIL_SECTION_ID);
	    else
	      {
	       Frm_BeginFormAnchor (NextAction[UsrDat->Roles.InCurrentCrs].New,Mai_EMAIL_SECTION_ID);
		  Usr_PutParamUsrCodEncrypted (UsrDat->EnUsrCod);
	      }
	       Par_PutHiddenParamString (NULL,"NewEmail",row[0]);
	       Btn_PutConfirmButtonInline ((ItsMe && NumEmail == 1) ? Txt_Confirm_email :
								      Txt_Use_this_email);
	    Frm_EndForm ();
	   }

	 if (NumEmail == 1 ||
	     NumEmail == NumEmails)
	   {
	       HTM_TD_End ();
	    HTM_TR_End ();
	   }
	 else
	    HTM_BR ();
	}

      /***** Form to enter new email *****/
      HTM_TR_Begin (NULL);

      /* Label */
      Frm_LabelColumn ("REC_C1_BOT RT","NewEmail",
		       NumEmails ? Txt_New_email :	// A new email
				   Txt_Email);	// The first email

      /* Data */
      HTM_TD_Begin ("class=\"REC_C2_BOT LT %s\"",The_ClassDat[Gbl.Prefs.Theme]);
	 if (ItsMe)
	    Frm_BeginFormAnchor (ActChgMyMai,Mai_EMAIL_SECTION_ID);
	 else
	   {
	    Frm_BeginFormAnchor (NextAction[UsrDat->Roles.InCurrentCrs].New,Mai_EMAIL_SECTION_ID);
	       Usr_PutParamUsrCodEncrypted (UsrDat->EnUsrCod);
	   }
	    HTM_INPUT_EMAIL ("NewEmail",Cns_MAX_CHARS_EMAIL_ADDRESS,Gbl.Usrs.Me.UsrDat.Email,
			     "id=\"NewEmail\" class=\"%s\" size=\"18\"",
			     The_ClassInput[Gbl.Prefs.Theme]);
	    HTM_BR ();
	    Btn_PutCreateButtonInline (NumEmails ? Txt_Change_email :	// User already has an email address
						   Txt_Save_changes);		// User has no email address yet
	 Frm_EndForm ();
      HTM_TD_End ();

      HTM_TR_End ();

   /***** End table *****/
   HTM_TABLE_End ();
  }

static void Mai_PutParamsRemoveMyEmail (void *Email)
  {
   if (Email)
      Par_PutHiddenParamString (NULL,"Email",Email);
  }

static void Mai_PutParamsRemoveOtherEmail (void *Email)
  {
   if (Email)
     {
      Usr_PutParamUsrCodEncrypted (Gbl.Usrs.Other.UsrDat.EnUsrCod);
      Par_PutHiddenParamString (NULL,"Email",Email);
     }
  }

/*****************************************************************************/
/******************** Remove one of my user's emails *************************/
/*****************************************************************************/

void Mai_RemoveMyUsrEmail (void)
  {
   /***** Remove user's email *****/
   Mai_RemoveEmail (&Gbl.Usrs.Me.UsrDat);

   /***** Show my account again *****/
   Acc_ShowFormChgMyAccount ();
  }

/*****************************************************************************/
/**************** Remove one of the user's IDs of another user ***************/
/*****************************************************************************/

void Mai_RemoveOtherUsrEmail (void)
  {
   /***** Get other user's code from form and get user's data *****/
   if (Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ())
     {
      if (Usr_ICanEditOtherUsr (&Gbl.Usrs.Other.UsrDat))
	{
	 /***** Remove user's email *****/
	 Mai_RemoveEmail (&Gbl.Usrs.Other.UsrDat);

	 /***** Show form again *****/
	 Acc_ShowFormChgOtherUsrAccount ();
	}
      else
	 Ale_ShowAlertUserNotFoundOrYouDoNotHavePermission ();
     }
   else		// User not found
      Ale_ShowAlertUserNotFoundOrYouDoNotHavePermission ();
  }

/*****************************************************************************/
/************************** Remove email address *****************************/
/*****************************************************************************/

static void Mai_RemoveEmail (struct UsrData *UsrDat)
  {
   extern const char *Txt_Email_X_removed;
   char Email[Cns_MAX_BYTES_EMAIL_ADDRESS + 1];

   if (Usr_ICanEditOtherUsr (UsrDat))
     {
      /***** Get new email from form *****/
      Par_GetParToText ("Email",Email,Cns_MAX_BYTES_EMAIL_ADDRESS);

      /***** Remove one of user's old email addresses *****/
      Mai_DB_RemoveEmail (UsrDat->UsrCod,Email);

      /***** Create alert *****/
      Ale_CreateAlert (Ale_SUCCESS,Mai_EMAIL_SECTION_ID,
	               Txt_Email_X_removed,
		       Email);

      /***** Update list of emails *****/
      Mai_GetEmailFromUsrCod (UsrDat);
     }
   else
      Ale_ShowAlertUserNotFoundOrYouDoNotHavePermission ();
  }

/*****************************************************************************/
/************************* New user's email for me ***************************/
/*****************************************************************************/

void May_NewMyUsrEmail (void)
  {
   /***** Remove user's email *****/
   Mai_NewUsrEmail (&Gbl.Usrs.Me.UsrDat,
		    true);	// It's me

   /***** Show my account again *****/
   Acc_ShowFormChgMyAccount ();
  }

/*****************************************************************************/
/********************* New user's email for another user *********************/
/*****************************************************************************/

void Mai_NewOtherUsrEmail (void)
  {
   /***** Get other user's code from form and get user's data *****/
   if (Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ())
     {
      if (Usr_ICanEditOtherUsr (&Gbl.Usrs.Other.UsrDat))
	{
	 /***** New user's ID *****/
	 Mai_NewUsrEmail (&Gbl.Usrs.Other.UsrDat,
	                  Usr_ItsMe (Gbl.Usrs.Other.UsrDat.UsrCod));

	 /***** Show form again *****/
	 Acc_ShowFormChgOtherUsrAccount ();
	}
      else
	 Ale_ShowAlertUserNotFoundOrYouDoNotHavePermission ();
     }
   else		// User not found
      Ale_ShowAlertUserNotFoundOrYouDoNotHavePermission ();
  }

/*****************************************************************************/
/************************* Update my email address ***************************/
/*****************************************************************************/

static void Mai_NewUsrEmail (struct UsrData *UsrDat,bool ItsMe)
  {
   extern const char *Txt_The_email_address_X_matches_one_previously_registered;
   extern const char *Txt_The_email_address_X_has_been_registered_successfully;
   extern const char *Txt_The_email_address_X_had_been_registered_by_another_user;
   extern const char *Txt_The_email_address_entered_X_is_not_valid;
   char NewEmail[Cns_MAX_BYTES_EMAIL_ADDRESS + 1];

   if (Usr_ICanEditOtherUsr (UsrDat))
     {
      /***** Get new email from form *****/
      Par_GetParToText ("NewEmail",NewEmail,Cns_MAX_BYTES_EMAIL_ADDRESS);

      if (Mai_CheckIfEmailIsValid (NewEmail))	// New email is valid
	{
	 /***** Check if new email exists in database *****/
	 if (UsrDat->EmailConfirmed &&
	     !strcmp (UsrDat->Email,NewEmail)) // User's current confirmed email match exactly the new email
	    Ale_CreateAlert (Ale_WARNING,Mai_EMAIL_SECTION_ID,
		             Txt_The_email_address_X_matches_one_previously_registered,
			     NewEmail);
	 else
	   {
	    if (Mai_UpdateEmailInDB (UsrDat,NewEmail))
	      {
	       /***** Email updated sucessfully *****/
	       Ale_CreateAlert (Ale_SUCCESS,Mai_EMAIL_SECTION_ID,
		                Txt_The_email_address_X_has_been_registered_successfully,
			        NewEmail);

	       /***** Update list of emails *****/
	       Mai_GetEmailFromUsrCod (UsrDat);

	       /***** Send message via email
		      to confirm the new email address *****/
	       if (ItsMe)
		  Mai_SendMailMsgToConfirmEmail ();
	      }
	    else
	       Ale_CreateAlert (Ale_WARNING,Mai_EMAIL_SECTION_ID,
		                Txt_The_email_address_X_had_been_registered_by_another_user,
				NewEmail);
	   }
	}
      else	// New email is not valid
         Ale_CreateAlert (Ale_WARNING,Mai_EMAIL_SECTION_ID,
                          Txt_The_email_address_entered_X_is_not_valid,
		          NewEmail);
     }
   else
      Ale_CreateAlertUserNotFoundOrYouDoNotHavePermission ();
  }

/*****************************************************************************/
/************************ Update email in database ***************************/
/*****************************************************************************/
// Return true if email is successfully updated
// Return false if email can not be updated beacuse it is registered by another user

bool Mai_UpdateEmailInDB (const struct UsrData *UsrDat,const char NewEmail[Cns_MAX_BYTES_EMAIL_ADDRESS + 1])
  {
   /***** Check if the new email matches any of the confirmed emails of other users *****/
   if (Mai_DB_CheckIfEmailBelongToAnotherUsr (UsrDat->UsrCod,NewEmail))	// An email of another user is the same that my email
      return false;	// Don't update

   /***** Delete email (not confirmed) for other users *****/
   Mai_DB_RemovePendingEmailForOtherUsrs (UsrDat->UsrCod,NewEmail);
   Mai_DB_RemoveNotConfirmedEmailForOtherUsrs (UsrDat->UsrCod,NewEmail);

   /***** Update email in database *****/
   Mai_DB_UpdateEmail (UsrDat->UsrCod,NewEmail);

   return true;	// Successfully updated
  }

/*****************************************************************************/
/************** Send mail message to confirm my email address ****************/
/*****************************************************************************/
// Return true on success
// Return false on error

bool Mai_SendMailMsgToConfirmEmail (void)
  {
   extern const char *Txt_If_you_just_request_from_X_the_confirmation_of_your_email_Y_NO_HTML;
   extern const char *Txt_Confirmation_of_your_email_NO_HTML;
   extern const char *Txt_A_message_has_been_sent_to_email_address_X_to_confirm_that_address;
   extern const char *Txt_There_was_a_problem_sending_an_email_automatically;
   char FileNameMail[PATH_MAX + 1];
   FILE *FileMail;
   char Command[2048 +
		Cfg_MAX_BYTES_SMTP_PASSWORD +
		Cns_MAX_BYTES_EMAIL_ADDRESS +
		PATH_MAX]; // Command to execute for sending an email
   int ReturnCode;

   /***** Create temporary file for mail content *****/
   Mai_CreateFileNameMail (FileNameMail,&FileMail);

   /***** Write mail content into file and close file *****/
   /* Welcome note */
   Mai_WriteWelcomeNoteEMail (FileMail,&Gbl.Usrs.Me.UsrDat);

   /* Store encrypted key in database */
   Mai_InsertMailKey (Gbl.Usrs.Me.UsrDat.Email,Gbl.UniqueNameEncrypted);

   /* Message body */
   fprintf (FileMail,
	    Txt_If_you_just_request_from_X_the_confirmation_of_your_email_Y_NO_HTML,
	    Cfg_URL_SWAD_CGI,Gbl.Usrs.Me.UsrDat.Email,
            Cfg_URL_SWAD_CGI,Act_GetActCod (ActCnfMai),Gbl.UniqueNameEncrypted,
            Cfg_URL_SWAD_CGI);

   /* Footer note */
   Mai_WriteFootNoteEMail (FileMail,Gbl.Prefs.Language);

   fclose (FileMail);

   /***** Call the script to send an email *****/
   snprintf (Command,sizeof (Command),
	     "%s \"%s\" \"%s\" \"%s\" \"%s\" \"%s\" \"[%s] %s\" \"%s\"",
             Cfg_COMMAND_SEND_AUTOMATIC_EMAIL,
             Cfg_AUTOMATIC_EMAIL_SMTP_SERVER,
	     Cfg_AUTOMATIC_EMAIL_SMTP_PORT,
             Cfg_AUTOMATIC_EMAIL_FROM,
             Gbl.Config.SMTPPassword,
             Gbl.Usrs.Me.UsrDat.Email,
             Cfg_PLATFORM_SHORT_NAME,Txt_Confirmation_of_your_email_NO_HTML,
             FileNameMail);
   ReturnCode = system (Command);
   if (ReturnCode == -1)
      Err_ShowErrorAndExit ("Error when running script to send email.");

   /***** Remove temporary file *****/
   unlink (FileNameMail);

   /***** Write message depending on return code *****/
   ReturnCode = WEXITSTATUS(ReturnCode);
   switch (ReturnCode)
     {
      case 0: // Message sent successfully
         Gbl.Usrs.Me.ConfirmEmailJustSent = true;
	 Ale_CreateAlert (Ale_SUCCESS,Mai_EMAIL_SECTION_ID,
	                  Txt_A_message_has_been_sent_to_email_address_X_to_confirm_that_address,
	   	          Gbl.Usrs.Me.UsrDat.Email);
         return true;
      case 1:
	 Ale_CreateAlert (Ale_ERROR,Mai_EMAIL_SECTION_ID,
	                  Txt_There_was_a_problem_sending_an_email_automatically);
         return false;
      default:
	 Ale_CreateAlert (Ale_ERROR,Mai_EMAIL_SECTION_ID,
	                  "Internal error: an email message has not been sent successfully."
			  " Error code returned by the script: %d",
			  ReturnCode);
         return false;
     }
  }

/*****************************************************************************/
/************************* Insert mail hey in database ***********************/
/*****************************************************************************/

static void Mai_InsertMailKey (const char Email[Cns_MAX_BYTES_EMAIL_ADDRESS + 1],
                               const char MailKey[Mai_LENGTH_EMAIL_CONFIRM_KEY + 1])
  {
   /***** Remove expired pending emails from database *****/
   Mai_DB_RemoveExpiredPendingEmails ();

   /***** Insert mail key in database *****/
   Mai_DB_InsertPendingEmail (Email,MailKey);
  }

/*****************************************************************************/
/************************* Confirm my email address **************************/
/*****************************************************************************/

void Mai_ConfirmEmail (void)
  {
   extern const char *Txt_Failed_email_confirmation_key;
   extern const char *Txt_Email_X_has_already_been_confirmed_before;
   extern const char *Txt_The_email_X_has_been_confirmed;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   char MailKey[Mai_LENGTH_EMAIL_CONFIRM_KEY + 1];
   long UsrCod;
   char Email[Cns_MAX_BYTES_EMAIL_ADDRESS + 1];
   bool KeyIsCorrect;

   /***** Get parameter Key *****/
   Par_GetParToText ("key",MailKey,Mai_LENGTH_EMAIL_CONFIRM_KEY);

   /***** Get user's code and email from key *****/
   if (Mai_DB_GetPendingEmail (&mysql_res,MailKey))
     {
      row = mysql_fetch_row (mysql_res);

      /* Get user's code (row[0]) */
      UsrCod = Str_ConvertStrCodToLongCod (row[0]);

      /* Get user's email (row[1]) */
      Str_Copy (Email,row[1],sizeof (Email) - 1);

      KeyIsCorrect = true;
     }
   else
     {
      row = NULL;
      UsrCod = -1L;
      KeyIsCorrect = false;
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   if (KeyIsCorrect)
     {
      /***** Delete this pending email *****/
      Mai_DB_RemovePendingEmail (MailKey);

      /***** Check user's code and email
             and get if email is already confirmed *****/
      switch (Mai_DB_CheckIfEmailIsConfirmed (UsrCod,Email))
        {
	 case '\0':
            Ale_ShowAlert (Ale_WARNING,Txt_Failed_email_confirmation_key);
            break;
	 case 'Y':
            Ale_ShowAlert (Ale_SUCCESS,Txt_Email_X_has_already_been_confirmed_before,
		           Email);
            break;
	 default:
            /***** Confirm email *****/
	    Mai_DB_ConfirmEmail (UsrCod,Email);
            Ale_ShowAlert (Ale_SUCCESS,Txt_The_email_X_has_been_confirmed,
		           Email);
            break;
        }
     }

   /***** Form to log in *****/
   Usr_WriteFormLogin (ActLogIn,NULL);
  }

/*****************************************************************************/
/****************** Create temporary file for mail content *******************/
/*****************************************************************************/

void Mai_CreateFileNameMail (char FileNameMail[PATH_MAX + 1],FILE **FileMail)
  {
   snprintf (FileNameMail,PATH_MAX + 1,"%s/%s_mail.txt",
             Cfg_PATH_OUT_PRIVATE,Gbl.UniqueNameEncrypted);
   if ((*FileMail = fopen (FileNameMail,"wb")) == NULL)
      Err_ShowErrorAndExit ("Can not open file to send email.");
  }

/*****************************************************************************/
/************ Write a welcome note heading the automatic email ***************/
/*****************************************************************************/

void Mai_WriteWelcomeNoteEMail (FILE *FileMail,struct UsrData *UsrDat)
  {
   extern const char *Txt_Dear_NO_HTML[Usr_NUM_SEXS][1 + Lan_NUM_LANGUAGES];
   extern const char *Txt_user_NO_HTML[Usr_NUM_SEXS][1 + Lan_NUM_LANGUAGES];

   fprintf (FileMail,"%s %s:\n",
            Txt_Dear_NO_HTML[UsrDat->Sex][UsrDat->Prefs.Language],
            UsrDat->FrstName[0] ? UsrDat->FrstName :
                                   Txt_user_NO_HTML[UsrDat->Sex][UsrDat->Prefs.Language]);
  }

/*****************************************************************************/
/****************** Write a foot note in the automatic email *****************/
/*****************************************************************************/

void Mai_WriteFootNoteEMail (FILE *FileMail,Lan_Language_t Language)
  {
   extern const char *Txt_Please_do_not_reply_to_this_automatically_generated_email_NO_HTML[1 + Lan_NUM_LANGUAGES];

   fprintf (FileMail,"%s\n"
                     "%s\n"
                     "%s\n",
            Txt_Please_do_not_reply_to_this_automatically_generated_email_NO_HTML[Language],
            Cfg_PLATFORM_SHORT_NAME,
            Cfg_URL_SWAD_CGI);
  }

/*****************************************************************************/
/**************** Check if I can see another user's email ********************/
/*****************************************************************************/

bool Mai_ICanSeeOtherUsrEmail (const struct UsrData *UsrDat)
  {
   /***** I can see my email *****/
   if (Usr_ItsMe (UsrDat->UsrCod))
      return true;

   /***** Check if I have permission to see another user's email *****/
   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_STD:
	 /* If I am a student in the current course,
	    I can see the email of confirmed teachers */
	 return (UsrDat->Roles.InCurrentCrs == Rol_NET ||	// A non-editing teacher
	         UsrDat->Roles.InCurrentCrs == Rol_TCH) &&	// or a teacher
	         UsrDat->Accepted;				// who accepted registration
      case Rol_NET:
      case Rol_TCH:
	 /* If I am a teacher in the current course,
	    I can see the email of confirmed students and teachers */
         return Enr_CheckIfUsrBelongsToCurrentCrs (UsrDat) &&	// A user belonging to the current course
	        UsrDat->Accepted;			// who accepted registration
      case Rol_DEG_ADM:
	 /* If I am an administrator of current degree,
	    I only can see the user's email of users from current degree */
	 return Deg_CheckIfUsrBelongsToDeg (UsrDat->UsrCod,
	                                    Gbl.Hierarchy.Deg.DegCod);
      case Rol_CTR_ADM:
	 /* If I am an administrator of current center,
	    I only can see the user's email of users from current center */
	 return Ctr_CheckIfUsrBelongsToCtr (UsrDat->UsrCod,
	                                    Gbl.Hierarchy.Ctr.CtrCod);
      case Rol_INS_ADM:
	 /* If I am an administrator of current institution,
	    I only can see the user's email of users from current institution */
	 return Ins_CheckIfUsrBelongsToIns (UsrDat->UsrCod,
	                                    Gbl.Hierarchy.Ins.InsCod);
      case Rol_SYS_ADM:
	 return true;
      default:
	 return false;
     }
  }

/*****************************************************************************/
/********************** Mail domain constructor/destructor *******************/
/*****************************************************************************/

static void Mai_EditingMailDomainConstructor (void)
  {
   /***** Pointer must be NULL *****/
   if (Mai_EditingMai != NULL)
      Err_WrongMailDomainExit ();

   /***** Allocate memory for mail domain *****/
   if ((Mai_EditingMai = malloc (sizeof (*Mai_EditingMai))) == NULL)
      Err_NotEnoughMemoryExit ();

   /***** Reset place *****/
   Mai_EditingMai->MaiCod    = -1L;
   Mai_EditingMai->Domain[0] = '\0';
   Mai_EditingMai->Info[0]   = '\0';
   Mai_EditingMai->NumUsrs   = 0;
  }

static void Mai_EditingMailDomainDestructor (void)
  {
   /***** Free memory used for mail domain *****/
   if (Mai_EditingMai != NULL)
     {
      free (Mai_EditingMai);
      Mai_EditingMai = NULL;
     }
  }
