// swad_mail.c: everything related to email

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2024 Antonio Cañas Vargas

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
#include "swad_action_list.h"
#include "swad_alert.h"
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
/******************************* Private types *******************************/
/*****************************************************************************/

struct Mai_Mails
  {
   unsigned Num;		// Number of mail domains
   struct Mail *Lst;		// List of mail domains
   Mai_DomainsOrder_t SelectedOrder;
  };

/*****************************************************************************/
/***************************** Private variables *****************************/
/*****************************************************************************/

static const char *Mai_EMAIL_SECTION_ID = "email_section";
static struct Mail *Mai_EditingMai = NULL;	// Static variable to keep the mail domain being edited

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static Mai_DomainsOrder_t Mai_GetParMaiOrder (void);
static void Mai_PutIconToEditMailDomains (__attribute__((unused)) void *Args);
static void Mai_EditMailDomainsInternal (void);
static void Mai_GetListMailDomainsAllowedForNotif (struct Mai_Mails *Mails);
static void Mai_GetMailDomain (const char *Email,
                               char MailDomain[Cns_MAX_BYTES_EMAIL_ADDRESS + 1]);
static void Mai_GetMailDomainDataFromRow (MYSQL_RES *mysql_res,
                                          struct Mail *Mai,
                                          bool GetNumUsrs);
static void Mai_FreeListMailDomains (struct Mai_Mails *Mails);

static void Mai_ListMailDomainsForEdition (const struct Mai_Mails *Mails);
static void Mai_PutParMaiCod (void *MaiCod);

static void Mai_RenameMailDomain (Nam_ShrtOrFullName_t ShrtOrFull);

static void Mai_PutFormToCreateMailDomain (void);
static void Mai_PutHeadMailDomains (void);

static void Mai_PutFormToSelectUsrsToListEmails (__attribute__((unused)) void *Args);
static void Mai_ListEmails (__attribute__((unused)) void *Args);

static void Mai_ShowFormChangeUsrEmail (Usr_MeOrOther_t MeOrOther,
				        bool IMustFillInEmail,
				        bool IShouldConfirmEmail);
static void Mai_PutParsRemoveMyEmail (void *Email);
static void Mai_PutParsRemoveOtherEmail (void *Email);

static void Mai_RemoveEmail (struct Usr_Data *UsrDat);
static void Mai_ChangeUsrEmail (struct Usr_Data *UsrDat,Usr_MeOrOther_t MeOrOther);
static void Mai_InsertMailKey (const char Email[Cns_MAX_BYTES_EMAIL_ADDRESS + 1],
                               const char MailKey[Mai_LENGTH_EMAIL_CONFIRM_KEY + 1]);

static void Mai_InitializeMailDomainList (struct Mai_Mails *Mails);

static void Mai_EditingMailDomainConstructor (void);
static void Mai_EditingMailDomainDestructor (void);

/*****************************************************************************/
/*************************** List all mail domains ***************************/
/*****************************************************************************/

void Mai_SeeMailDomains (void)
  {
   extern const char *Hlp_START_Domains;
   extern const char *Txt_Email_domains_allowed_for_notifications;
   extern const char *Txt_EMAIL_DOMAIN_HELP_ORDER[3];
   extern const char *Txt_EMAIL_DOMAIN_ORDER[3];
   Mai_DomainsOrder_t Order;
   unsigned NumMai;
   struct Mai_Mails Mails;

   /***** Initialize mail domain list *****/
   Mai_InitializeMailDomainList (&Mails);

   /***** Get parameter with the type of order in the list of mail domains *****/
   Mails.SelectedOrder = Mai_GetParMaiOrder ();

   /***** Get list of mail domains *****/
   Mai_GetListMailDomainsAllowedForNotif (&Mails);

   /***** Begin box and table *****/
   Box_BoxTableBegin (Txt_Email_domains_allowed_for_notifications,
		      Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM ? Mai_PutIconToEditMailDomains :
							       NULL,NULL,
		      Hlp_START_Domains,Box_NOT_CLOSABLE,2);

   /***** Write heading *****/
   HTM_TR_Begin (NULL);
      for (Order  = (Mai_DomainsOrder_t) 0;
	   Order <= (Mai_DomainsOrder_t) (Mai_NUM_ORDERS - 1);
	   Order++)
	{
         HTM_TH_Begin (HTM_HEAD_LEFT);

	    Frm_BeginForm (ActSeeMai);
	       Par_PutParUnsigned (NULL,"Order",(unsigned) Order);
	       HTM_BUTTON_Submit_Begin (Txt_EMAIL_DOMAIN_HELP_ORDER[Order],
	                                "class=\"BT_LINK\"");
		  if (Order == Mails.SelectedOrder)
		     HTM_U_Begin ();
		  HTM_Txt (Txt_EMAIL_DOMAIN_ORDER[Order]);
		  if (Order == Mails.SelectedOrder)
		     HTM_U_End ();
	       HTM_BUTTON_End ();
	    Frm_EndForm ();

	 HTM_TH_End ();
	}
   HTM_TR_End ();

   /***** Write all mail domains *****/
   for (NumMai = 0;
	NumMai < Mails.Num;
	NumMai++)
     {
      /* Write data of this mail domain */
      HTM_TR_Begin (NULL);

	 HTM_TD_Begin ("class=\"LT DAT_%s\"",The_GetSuffix ());
	    HTM_Txt (Mails.Lst[NumMai].Domain);
	 HTM_TD_End ();

	 HTM_TD_Begin ("class=\"LT DAT_%s\"",The_GetSuffix ());
	    HTM_Txt (Mails.Lst[NumMai].Info);
	 HTM_TD_End ();

	 HTM_TD_Begin ("class=\"RT DAT_%s\"",The_GetSuffix ());
	    HTM_Unsigned (Mails.Lst[NumMai].NumUsrs);
	 HTM_TD_End ();

      HTM_TR_End ();
     }

   /***** End table and box *****/
   Box_BoxTableEnd ();

   /***** Free list of mail domains *****/
   Mai_FreeListMailDomains (&Mails);
  }

/*****************************************************************************/
/******* Get parameter with the type or order in list of mail domains ********/
/*****************************************************************************/

static Mai_DomainsOrder_t Mai_GetParMaiOrder (void)
  {
   return (Mai_DomainsOrder_t)
	  Par_GetParUnsignedLong ("Order",
				  0,
				  Mai_NUM_ORDERS - 1,
				  (unsigned long) Mai_ORDER_DEFAULT);
  }

/*****************************************************************************/
/************************ Put icon to edit mail domains **********************/
/*****************************************************************************/

static void Mai_PutIconToEditMailDomains (__attribute__((unused)) void *Args)
  {
   Ico_PutContextualIconToEdit (ActEdiMai,NULL,NULL,NULL);
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
   struct Mai_Mails Mails;

   /***** Initialize mail domain list *****/
   Mai_InitializeMailDomainList (&Mails);

   /***** Get list of mail domains *****/
   Mai_GetListMailDomainsAllowedForNotif (&Mails);

   /***** Forms to edit mail domains *****/
   Mai_ListMailDomainsForEdition (&Mails);

   /***** Free list of mail domains *****/
   Mai_FreeListMailDomains (&Mails);
  }

/*****************************************************************************/
/*************************** List all mail domains ***************************/
/*****************************************************************************/

static void Mai_GetListMailDomainsAllowedForNotif (struct Mai_Mails *Mails)
  {
   MYSQL_RES *mysql_res;
   unsigned NumMai;

   // Query uses temporary tables for speed
   // Query uses two identical temporary tables...
   // ...because a unique temporary table can not be used twice in the same query

   /***** Create temporary table with all mail domains present in users' emails table *****/
   Mai_DB_RemoveTmpTables ();

   Mai_DB_CreateTmpTables ();

   /***** Get mail domains from database *****/
   if ((Mails->Num = Mai_DB_GetMailDomains (&mysql_res,Mails->SelectedOrder))) // Mail domains found...
     {
      /***** Create list with places *****/
      if ((Mails->Lst = calloc ((size_t) Mails->Num,
                                sizeof (struct Mail))) == NULL)
          Err_NotEnoughMemoryExit ();

      /***** Get the mail domains *****/
      for (NumMai = 0;
	   NumMai < Mails->Num;
	   NumMai++)
         Mai_GetMailDomainDataFromRow (mysql_res,&Mails->Lst[NumMai],
	                               true);	// Get number of users
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   /***** Drop temporary table *****/
   Mai_DB_RemoveTmpTables ();
  }

/*****************************************************************************/
/************ Check if user can receive notifications via email **************/
/*****************************************************************************/

bool Mai_CheckIfUsrCanReceiveEmailNotif (const struct Usr_Data *UsrDat)
  {
   char MailDomain[Cns_MAX_BYTES_EMAIL_ADDRESS + 1];

   /***** Check #1: is email empty or filled? *****/
   if (!UsrDat->Email[0])
      return false;

   /***** Check #2: is email address confirmed? *****/
   if (!UsrDat->EmailConfirmed)
      return false;

   /***** Check #3: check if there is mail domain *****/
   Mai_GetMailDomain (UsrDat->Email,MailDomain);
   if (!MailDomain[0])
      return false;

   /***** Check #4: is mail domain allowed? *****/
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
   extern const char *Txt_Domains;

   Ale_ShowAlert (Ale_WARNING,Txt_You_can_only_receive_email_notifications_if_,
	          Tab_GetTxt (Act_GetTab (ActFrmMyAcc)),Act_GetTitleAction (ActFrmMyAcc),
                  Tab_GetTxt (Act_GetTab (ActSeeMai  )),Act_GetTitleAction (ActSeeMai  ),
	          Txt_Domains);
  }

/*****************************************************************************/
/******************* Get mail domain data using its code *********************/
/*****************************************************************************/

void Mai_GetMailDomainDataByCod (struct Mail *Mai)
  {
   MYSQL_RES *mysql_res;

   /***** Clear data *****/
   Mai->Domain[0] = Mai->Info[0] = '\0';

   /***** Check if mail code is correct *****/
   if (Mai->MaiCod > 0)
     {
      /***** Get data of a mail domain from database *****/
      if (Mai_DB_GetMailDomainDataByCod (&mysql_res,Mai->MaiCod)) // Mail found...
	 Mai_GetMailDomainDataFromRow (mysql_res,Mai,
	                               false);	// Don't get number of users

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);
     }
  }

/*****************************************************************************/
/****************** Get mail domain data from database row *******************/
/*****************************************************************************/

static void Mai_GetMailDomainDataFromRow (MYSQL_RES *mysql_res,
                                          struct Mail *Mai,
                                          bool GetNumUsrs)
  {
   MYSQL_ROW row;

   /***** Get next row from result *****/
   row = mysql_fetch_row (mysql_res);

   /***** Get mail code (row[0]) *****/
   if ((Mai->MaiCod = Str_ConvertStrCodToLongCod (row[0])) <= 0)
      Err_WrongMailDomainExit ();

   /***** Get the mail domain (row[1]) and the mail domain info (row[2]) *****/
   Str_Copy (Mai->Domain,row[1],sizeof (Mai->Domain) - 1);
   Str_Copy (Mai->Info  ,row[2],sizeof (Mai->Info  ) - 1);

   /***** Get number of users (row[3]) *****/
   if (GetNumUsrs)
     {
      if (sscanf (row[3],"%u",&(Mai->NumUsrs)) != 1)
	 Mai->NumUsrs = 0;
     }
   else
      Mai->NumUsrs = 0;
  }

/*****************************************************************************/
/************************** Free list of mail domains ************************/
/*****************************************************************************/

static void Mai_FreeListMailDomains (struct Mai_Mails *Mails)
  {
   if (Mails->Num && Mails->Lst)
     {
      free (Mails->Lst);
      Mails->Lst = NULL;
      Mails->Num = 0;
     }
  }

/*****************************************************************************/
/************************** List all mail domains ****************************/
/*****************************************************************************/

static void Mai_ListMailDomainsForEdition (const struct Mai_Mails *Mails)
  {
   extern const char *Hlp_START_Domains_edit;
   extern const char *Txt_Email_domains_allowed_for_notifications;
   unsigned NumMai;
   struct Mail *Mai;

   /***** Begin box *****/
   Box_BoxBegin (Txt_Email_domains_allowed_for_notifications,NULL,NULL,
                 Hlp_START_Domains_edit,Box_NOT_CLOSABLE);

      /***** Put a form to create a new mail *****/
      Mai_PutFormToCreateMailDomain ();

      if (Mails->Num)
	{
         /***** Begin table *****/
         HTM_TABLE_Begin ("TBL_SCROLL");

	    /***** Write heading *****/
	    Mai_PutHeadMailDomains ();

	    /***** Write all mail domains *****/
	    for (NumMai = 0;
		 NumMai < Mails->Num;
		 NumMai++)
	      {
	       Mai = &Mails->Lst[NumMai];

	       HTM_TR_Begin (NULL);

		  /* Put icon to remove mail domain */
		  HTM_TD_Begin ("class=\"BM\"");
		     Ico_PutContextualIconToRemove (ActRemMai,NULL,
						    Mai_PutParMaiCod,&Mai->MaiCod);
		  HTM_TD_End ();

		  /* Mail domain code */
		  HTM_TD_Begin ("class=\"CODE DAT_%s\"",The_GetSuffix ());
		     HTM_Long (Mai->MaiCod);
		  HTM_TD_End ();

		  /* Mail domain */
		  HTM_TD_Begin ("class=\"LM\"");
		     Frm_BeginForm (ActRenMaiSho);
			ParCod_PutPar (ParCod_Mai,Mai->MaiCod);
			HTM_INPUT_TEXT ("Domain",Cns_MAX_CHARS_EMAIL_ADDRESS,Mai->Domain,
					HTM_ENABLED,HTM_SUBMIT_ON_CHANGE,
					"size=\"15\" class=\"INPUT_%s\"",
					The_GetSuffix ());
		     Frm_EndForm ();
		  HTM_TD_End ();

		  /* Mail domain info */
		  HTM_TD_Begin ("class=\"LM\"");
		     Frm_BeginForm (ActRenMaiFul);
			ParCod_PutPar (ParCod_Mai,Mai->MaiCod);
			HTM_INPUT_TEXT ("Info",Mai_MAX_CHARS_MAIL_INFO,Mai->Info,
					HTM_ENABLED,HTM_SUBMIT_ON_CHANGE,
					"size=\"40\" class=\"INPUT_%s\"",
					The_GetSuffix ());
		     Frm_EndForm ();
		  HTM_TD_End ();

		  /* Number of users */
		  HTM_TD_Unsigned (Mai->NumUsrs);

	       HTM_TR_End ();
	   }

	 /***** End table *****/
	 HTM_TABLE_End ();
	}

   /***** End box *****/
   Box_BoxEnd ();
  }

/*****************************************************************************/
/******************** Write parameter with code of mail **********************/
/*****************************************************************************/

static void Mai_PutParMaiCod (void *MaiCod)
  {
   if (MaiCod)
      ParCod_PutPar (ParCod_Mai,*((long *) MaiCod));
  }

/*****************************************************************************/
/******************************* Remove a mail *******************************/
/*****************************************************************************/

void Mai_RemoveMailDom (void)
  {
   extern const char *Txt_Email_domain_X_removed;

   /***** Mail domain constructor *****/
   Mai_EditingMailDomainConstructor ();

   /***** Get mail domain code *****/
   Mai_EditingMai->MaiCod = ParCod_GetAndCheckPar (ParCod_Mai);

   /***** Get data of the mail domain rom database *****/
   Mai_GetMailDomainDataByCod (Mai_EditingMai);

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

void Mai_RenameMailDomShrt (void)
  {
   /***** Mail domain constructor *****/
   Mai_EditingMailDomainConstructor ();

   /***** Rename mail domain *****/
   Mai_RenameMailDomain (Nam_SHRT_NAME);
  }

/*****************************************************************************/
/********************* Change the full name of a mail ************************/
/*****************************************************************************/

void Mai_RenameMailDomFull (void)
  {
   /***** Mail domain constructor *****/
   Mai_EditingMailDomainConstructor ();

   /***** Rename mail domain *****/
   Mai_RenameMailDomain (Nam_FULL_NAME);
  }

/*****************************************************************************/
/************************* Change the name of a mail *************************/
/*****************************************************************************/

static void Mai_RenameMailDomain (Nam_ShrtOrFullName_t ShrtOrFull)
  {
   extern const char *Txt_The_email_domain_X_already_exists;
   extern const char *Txt_The_email_domain_X_has_been_renamed_as_Y;
   extern const char *Txt_The_email_domain_X_has_not_changed;
   static const char *ParName[Nam_NUM_SHRT_FULL_NAMES] =
     {
      [Nam_SHRT_NAME] = "Domain",
      [Nam_FULL_NAME] = "Info",
     };
   static const char *FldName[Nam_NUM_SHRT_FULL_NAMES] =
     {
      [Nam_SHRT_NAME] = "Domain",
      [Nam_FULL_NAME] = "Info",
     };
   static unsigned MaxBytes[Nam_NUM_SHRT_FULL_NAMES] =
     {
      [Nam_SHRT_NAME] = Cns_MAX_BYTES_EMAIL_ADDRESS,
      [Nam_FULL_NAME] = Mai_MAX_BYTES_MAIL_INFO,
     };
   char *CurrentName[Nam_NUM_SHRT_FULL_NAMES] =
     {
      [Nam_SHRT_NAME] = Mai_EditingMai->Domain,
      [Nam_FULL_NAME] = Mai_EditingMai->Info,
     };
   char NewName[Mai_MAX_BYTES_MAIL_INFO + 1];

   /***** Get parameters from form *****/
   /* Get the code of the mail */
   Mai_EditingMai->MaiCod = ParCod_GetAndCheckPar (ParCod_Mai);

   /* Get the new name for the mail */
   Par_GetParText (ParName[ShrtOrFull],NewName,MaxBytes[ShrtOrFull]);

   /***** Get from the database the old names of the mail *****/
   Mai_GetMailDomainDataByCod (Mai_EditingMai);

   /***** Check if new name is empty *****/
   if (NewName[0])
     {
      /***** Check if old and new names are the same
             (this happens when return is pressed without changes) *****/
      if (strcmp (CurrentName[ShrtOrFull],NewName))	// Different names
        {
         /***** If mail was in database... *****/
         if (Mai_DB_CheckIfMailDomainNameExists (FldName[ShrtOrFull],NewName,
						 Mai_EditingMai->MaiCod))
	    Ale_CreateAlert (Ale_WARNING,Mai_EMAIL_SECTION_ID,
		             Txt_The_email_domain_X_already_exists,
                             NewName);
         else
           {
            /* Update the table changing old name by new name */
            Mai_DB_UpdateMailDomainName (Mai_EditingMai->MaiCod,FldName[ShrtOrFull],NewName);

            /* Write message to show the change made */
	    Ale_CreateAlert (Ale_SUCCESS,Mai_EMAIL_SECTION_ID,
		             Txt_The_email_domain_X_has_been_renamed_as_Y,
                             CurrentName[ShrtOrFull],NewName);
           }
        }
      else	// The same name
	 Ale_CreateAlert (Ale_INFO,Mai_EMAIL_SECTION_ID,
	                  Txt_The_email_domain_X_has_not_changed,
                          CurrentName[ShrtOrFull]);
     }
   else
      Ale_CreateAlertYouCanNotLeaveFieldEmpty ();

   /***** Update name *****/
   Str_Copy (CurrentName[ShrtOrFull],NewName,MaxBytes[ShrtOrFull]);
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
   /***** Begin form to create *****/
   Frm_BeginFormTable (ActNewMai,NULL,NULL,NULL,"TBL_SCROLL");

      /***** Write heading *****/
      Mai_PutHeadMailDomains ();

      /***** Second row *****/
      HTM_TR_Begin (NULL);

	 /* Column to remove mail domain, disabled here */
	 HTM_TD_Begin ("class=\"BM\"");
	 HTM_TD_End ();

	 /* Mail domain code */
	 HTM_TD_Begin ("class=\"CODE\"");
	 HTM_TD_End ();

	 /* Mail domain */
	 HTM_TD_Begin ("class=\"LM\"");
	    HTM_INPUT_TEXT ("Domain",Cns_MAX_CHARS_EMAIL_ADDRESS,Mai_EditingMai->Domain,
			    HTM_ENABLED,HTM_DONT_SUBMIT_ON_CHANGE,
			    "size=\"15\" class=\"INPUT_%s\" required=\"required\"",
			    The_GetSuffix ());
	 HTM_TD_End ();

	 /* Mail domain info */
	 HTM_TD_Begin ("class=\"LM\"");
	    HTM_INPUT_TEXT ("Info",Mai_MAX_CHARS_MAIL_INFO,Mai_EditingMai->Info,
			    HTM_ENABLED,HTM_DONT_SUBMIT_ON_CHANGE,
			    "size=\"40\" class=\"INPUT_%s\" required=\"required\"",
			    The_GetSuffix ());
	 HTM_TD_End ();

	 /* Number of users */
	 HTM_TD_Unsigned (0);

      HTM_TR_End ();

   /***** End form to create *****/
   Frm_EndFormTable (Btn_CREATE_BUTTON);
  }

/*****************************************************************************/
/********************* Write header with fields of a mail ********************/
/*****************************************************************************/

static void Mai_PutHeadMailDomains (void)
  {
   extern const char *Txt_Code;
   extern const char *Txt_EMAIL_DOMAIN_ORDER[3];

   HTM_TR_Begin (NULL);
      HTM_TH_Span (NULL                                  ,HTM_HEAD_CENTER,1,1,"BT");
      HTM_TH (Txt_Code                                   ,HTM_HEAD_RIGHT );
      HTM_TH (Txt_EMAIL_DOMAIN_ORDER[Mai_ORDER_BY_DOMAIN],HTM_HEAD_LEFT  );
      HTM_TH (Txt_EMAIL_DOMAIN_ORDER[Mai_ORDER_BY_INFO  ],HTM_HEAD_LEFT  );
      HTM_TH (Txt_EMAIL_DOMAIN_ORDER[Mai_ORDER_BY_USERS ],HTM_HEAD_RIGHT );
   HTM_TR_End ();
  }

/*****************************************************************************/
/******************* Receive form to create a new mail ***********************/
/*****************************************************************************/

void Mai_ReceiveNewMailDom (void)
  {
   extern const char *Txt_The_email_domain_X_already_exists;
   extern const char *Txt_Created_new_email_domain_X;

   /***** Mail domain constructor *****/
   Mai_EditingMailDomainConstructor ();

   /***** Get parameters from form *****/
   /* Get mail short name */
   Par_GetParText ("Domain",Mai_EditingMai->Domain,Cns_MAX_BYTES_EMAIL_ADDRESS);

   /* Get mail full name */
   Par_GetParText ("Info",Mai_EditingMai->Info,Mai_MAX_BYTES_MAIL_INFO);

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
      Ale_CreateAlertYouMustSpecifyShrtNameAndFullName ();
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
				     Frm_DONT_PUT_FORM);	// Do not put form with date range
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
   extern const char *Txt_Email_addresses;
   extern const char *Txt_X_users_who_have_email;
   extern const char *Txt_X_users_who_have_accepted_and_who_have_email;
   extern const char *Txt_Create_email_message;
   unsigned NumUsrsWithEmail = 0;
   unsigned NumAcceptedUsrsWithEmail = 0;
   char StrAddresses[Mai_MAX_BYTES_STR_ADDR + 1];	// TODO: Use malloc depending on the number of students
   unsigned int LengthStrAddr = 0;
   struct Usr_Data UsrDat;
   const char *Ptr;

   /***** Begin the box used to list the emails *****/
   Box_BoxBegin (Txt_Email_addresses,NULL,NULL,
		 Hlp_COMMUNICATION_Email,Box_NOT_CLOSABLE);

      /***** Begin list with users' email addresses *****/
      HTM_DIV_Begin ("class=\"CM DAT_SMALL_%s\"",The_GetSuffix ());

	 /***** Initialize structure with user's data *****/
	 Usr_UsrDataConstructor (&UsrDat);

	 /***** Get email addresses of the selected users *****/
	 StrAddresses[0] = '\0';
	 Ptr = Gbl.Usrs.Selected.List[Rol_UNK];
	 while (*Ptr)
	   {
	    /* Get next user */
	    Par_GetNextStrUntilSeparParMult (&Ptr,UsrDat.EnUsrCod,
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
			       UsrDat.Email,Gbl.Hierarchy.Node[Hie_CRS].FullName);
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
      HTM_DIV_Begin ("class=\"CM DAT_%s\"",The_GetSuffix ());
	 HTM_TxtF (Txt_X_users_who_have_email,NumUsrsWithEmail);
      HTM_DIV_End ();

      /***** Show a message with the number of users who have accepted and have email ****/
      HTM_DIV_Begin ("class=\"CM DAT_%s\"",The_GetSuffix ());
	 HTM_TxtF (Txt_X_users_who_have_accepted_and_who_have_email,
		   NumAcceptedUsrsWithEmail);
      HTM_DIV_End ();

      /***** Contextual menu *****/
      Mnu_ContextMenuBegin ();

	 /* Open the client email program */
	 HTM_A_Begin ("href=\"mailto:%s?subject=%s&cc=%s&bcc=%s\""
		      " title=\"%s\" class=\"FORM_OUT_%s BOLD\"",
		      Gbl.Usrs.Me.UsrDat.Email,
		      Gbl.Hierarchy.Node[Hie_CRS].FullName,
		      Gbl.Usrs.Me.UsrDat.Email,
		      StrAddresses,
		      Txt_Create_email_message,
		      The_GetSuffix ());
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

bool Mai_GetEmailFromUsrCod (struct Usr_Data *UsrDat)
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
/******************** Put form to request the new email **********************/
/*****************************************************************************/

void Mai_PutFormToGetNewEmail (const char *NewEmail)
  {
   extern const char *Txt_Email;
   extern const char *Txt_HELP_email;

   /***** Email *****/
   HTM_TR_Begin (NULL);

      /* Label */
      Frm_LabelColumn ("Frm_C1 RM","NewEmail",Txt_Email);

      /* Data */
      HTM_TD_Begin ("class=\"Frm_C2 LM\"");
	 HTM_INPUT_EMAIL ("NewEmail",Cns_MAX_CHARS_EMAIL_ADDRESS,NewEmail,
			  "id=\"NewEmail\" size=\"16\" placeholder=\"%s\""
			  " class=\"Frm_C2_INPUT INPUT_%s\""
			  " required=\"required\"",
			  Txt_HELP_email,
			  The_GetSuffix ());
      HTM_TD_End ();

   HTM_TR_End ();
  }

/*****************************************************************************/
/*********************** Show form to change my email ************************/
/*****************************************************************************/

void Mai_ShowFormChangeMyEmail (bool IMustFillInEmail,bool IShouldConfirmEmail)
  {
   extern const char *Hlp_PROFILE_Account;
   extern const char *Txt_Email;

   /***** Begin section *****/
   HTM_SECTION_Begin (Mai_EMAIL_SECTION_ID);

      /***** Begin box *****/
      Box_BoxBegin (Txt_Email,Acc_PutLinkToRemoveMyAccount,NULL,
		    Hlp_PROFILE_Account,Box_NOT_CLOSABLE);

	 /***** Show form to change email *****/
	 Mai_ShowFormChangeUsrEmail (Usr_ME,
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

   /***** Begin section *****/
   HTM_SECTION_Begin (Mai_EMAIL_SECTION_ID);

      /***** Begin box *****/
      Box_BoxBegin (Txt_Email,NULL,NULL,Hlp_PROFILE_Account,Box_NOT_CLOSABLE);

	 /***** Show form to change email *****/
	 Mai_ShowFormChangeUsrEmail (Usr_OTHER,
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

static void Mai_ShowFormChangeUsrEmail (Usr_MeOrOther_t MeOrOther,
				        bool IMustFillInEmail,
				        bool IShouldConfirmEmail)
  {
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
      Act_Action_t Change;
     } NextAction[Rol_NUM_ROLES] =
     {
      [Rol_UNK	  ] = {ActRemMaiOth,ActChgMaiOth},
      [Rol_GST	  ] = {ActRemMaiOth,ActChgMaiOth},
      [Rol_USR    ] = {ActRemMaiOth,ActChgMaiOth},
      [Rol_STD	  ] = {ActRemMaiStd,ActChgMaiStd},
      [Rol_NET	  ] = {ActRemMaiTch,ActChgMaiTch},
      [Rol_TCH	  ] = {ActRemMaiTch,ActChgMaiTch},
      [Rol_DEG_ADM] = {ActRemMaiOth,ActChgMaiOth},
      [Rol_CTR_ADM] = {ActRemMaiOth,ActChgMaiOth},
      [Rol_INS_ADM] = {ActRemMaiOth,ActChgMaiOth},
      [Rol_SYS_ADM] = {ActRemMaiOth,ActChgMaiOth},
     };
   static const struct Usr_Data *UsrDat[Usr_NUM_ME_OR_OTHER] =
     {
      [Usr_ME   ] = &Gbl.Usrs.Me.UsrDat,
      [Usr_OTHER] = &Gbl.Usrs.Other.UsrDat
     };
   static void (*FuncParsRemove[Usr_NUM_ME_OR_OTHER]) (void *ID) =
     {
      [Usr_ME   ] = Mai_PutParsRemoveMyEmail,
      [Usr_OTHER] = Mai_PutParsRemoveOtherEmail
     };
   struct
     {
      Act_Action_t Remove;
      Act_Action_t Change;
     } ActMail[Rol_NUM_ROLES] =
     {
      [Usr_ME   ] = {.Remove = ActRemMyMai,
	             .Change = ActChgMyMai},
      [Usr_OTHER] = {.Remove = NextAction[Gbl.Usrs.Other.UsrDat.Roles.InCurrentCrs].Remove,
	             .Change = NextAction[Gbl.Usrs.Other.UsrDat.Roles.InCurrentCrs].Change}
     };

   /***** Show possible alerts *****/
   Ale_ShowAlerts (Mai_EMAIL_SECTION_ID);

   /***** Help message *****/
   if (IMustFillInEmail)
      Ale_ShowAlert (Ale_WARNING,Txt_Before_going_to_any_other_option_you_must_fill_in_your_email_address);
   else if (IShouldConfirmEmail)
      Ale_ShowAlert (Ale_WARNING,Txt_Please_confirm_your_email_address);

   /***** Get my emails *****/
   NumEmails = Mai_DB_GetMyEmails (&mysql_res,UsrDat[MeOrOther]->UsrCod);

   /***** Begin table *****/
   HTM_TABLE_BeginPadding (2);

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
	       Frm_LabelColumn ("Frm_C1 RT",NULL,Txt_Current_email);

	       /* Data */
	       HTM_TD_Begin ("class=\"Frm_C2 LT DAT_STRONG_%s\"",The_GetSuffix ());
	   }
	 else if (NumEmail == 2)
	   {
	    HTM_TR_Begin (NULL);

	       /* Label */
	       Frm_LabelColumn ("Frm_C1 RT",NULL,Txt_Other_emails);

	       /* Data */
	       HTM_TD_Begin ("class=\"Frm_C2 LT DAT_%s\"",The_GetSuffix ());
	   }

	 /* Form to remove email */
	 Ico_PutContextualIconToRemove (ActMail[MeOrOther].Remove,Mai_EMAIL_SECTION_ID,
					FuncParsRemove[MeOrOther],row[0]);

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
	 if (NumEmail > 1 || (MeOrOther == Usr_ME && !Confirmed))
	   {
	    HTM_BR ();
	    Frm_BeginFormAnchor (ActMail[MeOrOther].Change,Mai_EMAIL_SECTION_ID);
               if (MeOrOther == Usr_OTHER)
	          Usr_PutParUsrCodEncrypted (Gbl.Usrs.Other.UsrDat.EnUsrCod);
	       Par_PutParString (NULL,"NewEmail",row[0]);
	       Btn_PutConfirmButtonInline ((MeOrOther == Usr_ME && NumEmail == 1) ? Txt_Confirm_email :
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
	 Frm_LabelColumn ("Frm_C1 RT","NewEmail",
			  NumEmails ? Txt_New_email :	// A new email
				      Txt_Email);	// The first email

	 /* Data */
	 HTM_TD_Begin ("class=\"Frm_C2 LT DAT_%s\"",The_GetSuffix ());
	    Frm_BeginFormAnchor (ActMail[MeOrOther].Change,Mai_EMAIL_SECTION_ID);
	       if (MeOrOther == Usr_OTHER)
		  Usr_PutParUsrCodEncrypted (Gbl.Usrs.Other.UsrDat.EnUsrCod);
	       HTM_INPUT_EMAIL ("NewEmail",Cns_MAX_CHARS_EMAIL_ADDRESS,UsrDat[MeOrOther]->Email,
				"id=\"NewEmail\""
				" class=\"Frm_C2_INPUT INPUT_%s\" size=\"16\"",
				The_GetSuffix ());
	       HTM_BR ();
	       Btn_PutCreateButtonInline (NumEmails ? Txt_Change_email :	// User already has an email address
						      Txt_Save_changes);	// User has no email address yet
	    Frm_EndForm ();
	 HTM_TD_End ();

      HTM_TR_End ();

   /***** End table *****/
   HTM_TABLE_End ();
  }

static void Mai_PutParsRemoveMyEmail (void *Email)
  {
   if (Email)
      Par_PutParString (NULL,"Email",Email);
  }

static void Mai_PutParsRemoveOtherEmail (void *Email)
  {
   if (Email)
     {
      Usr_PutParUsrCodEncrypted (Gbl.Usrs.Other.UsrDat.EnUsrCod);
      Par_PutParString (NULL,"Email",Email);
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
   if (Usr_GetParOtherUsrCodEncryptedAndGetUsrData ())
      switch (Usr_CheckIfICanEditOtherUsr (&Gbl.Usrs.Other.UsrDat))
	{
	 case Usr_CAN:
	    /***** Remove user's email *****/
	    Mai_RemoveEmail (&Gbl.Usrs.Other.UsrDat);

	    /***** Show form again *****/
	    Acc_ShowFormChgOtherUsrAccount ();
	    break;
	 case Usr_CAN_NOT:
	 default:
	    Ale_ShowAlertUserNotFoundOrYouDoNotHavePermission ();
	    break;
	}
   else		// User not found
      Ale_ShowAlertUserNotFoundOrYouDoNotHavePermission ();
  }

/*****************************************************************************/
/************************** Remove email address *****************************/
/*****************************************************************************/

static void Mai_RemoveEmail (struct Usr_Data *UsrDat)
  {
   extern const char *Txt_Email_X_removed;
   char Email[Cns_MAX_BYTES_EMAIL_ADDRESS + 1];

   switch (Usr_CheckIfICanEditOtherUsr (UsrDat))
     {
      case Usr_CAN:
	 /***** Get new email from form *****/
	 Par_GetParText ("Email",Email,Cns_MAX_BYTES_EMAIL_ADDRESS);

	 /***** Remove one of user's old email addresses *****/
	 Mai_DB_RemoveEmail (UsrDat->UsrCod,Email);

	 /***** Create alert *****/
	 Ale_CreateAlert (Ale_SUCCESS,Mai_EMAIL_SECTION_ID,
			  Txt_Email_X_removed,
			  Email);

	 /***** Update list of emails *****/
	 Mai_GetEmailFromUsrCod (UsrDat);
	 break;
      case Usr_CAN_NOT:
      default:
	 Ale_ShowAlertUserNotFoundOrYouDoNotHavePermission ();
	 break;
     }
  }

/*****************************************************************************/
/************************* New user's email for me ***************************/
/*****************************************************************************/

void May_NewMyUsrEmail (void)
  {
   /***** Remove user's email *****/
   Mai_ChangeUsrEmail (&Gbl.Usrs.Me.UsrDat,Usr_ME);

   /***** Show my account again *****/
   Acc_ShowFormChgMyAccount ();
  }

/*****************************************************************************/
/************************ Change another user's email ************************/
/*****************************************************************************/

void Mai_ChangeOtherUsrEmail (void)
  {
   /***** Get other user's code from form and get user's data *****/
   if (Usr_GetParOtherUsrCodEncryptedAndGetUsrData ())
      switch (Usr_CheckIfICanEditOtherUsr (&Gbl.Usrs.Other.UsrDat))
	{
	 case Usr_CAN:
	    /***** Change user's ID *****/
	    Mai_ChangeUsrEmail (&Gbl.Usrs.Other.UsrDat,
				Usr_ItsMe (Gbl.Usrs.Other.UsrDat.UsrCod));

	    /***** Show form again *****/
	    Acc_ShowFormChgOtherUsrAccount ();
	    break;
	 case Usr_CAN_NOT:
	 default:
	    Ale_ShowAlertUserNotFoundOrYouDoNotHavePermission ();
	    break;
	}
   else		// User not found
      Ale_ShowAlertUserNotFoundOrYouDoNotHavePermission ();
  }

/*****************************************************************************/
/*************************** Change email address ****************************/
/*****************************************************************************/

static void Mai_ChangeUsrEmail (struct Usr_Data *UsrDat,Usr_MeOrOther_t MeOrOther)
  {
   extern const char *Txt_The_email_address_X_matches_one_previously_registered;
   extern const char *Txt_The_email_address_X_has_been_registered_successfully;
   extern const char *Txt_The_email_address_X_had_been_registered_by_another_user;
   extern const char *Txt_The_email_address_entered_X_is_not_valid;
   char NewEmail[Cns_MAX_BYTES_EMAIL_ADDRESS + 1];

   switch (Usr_CheckIfICanEditOtherUsr (UsrDat))
     {
      case Usr_CAN:
	 /***** Get new email from form *****/
	 Par_GetParText ("NewEmail",NewEmail,Cns_MAX_BYTES_EMAIL_ADDRESS);

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
		  if (MeOrOther == Usr_ME)
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
	 break;
      case Usr_CAN_NOT:
      default:
	 Ale_CreateAlertUserNotFoundOrYouDoNotHavePermission ();
	 break;
     }
  }

/*****************************************************************************/
/************************ Update email in database ***************************/
/*****************************************************************************/
// Return true if email is successfully updated
// Return false if email can not be updated beacuse it is registered by another user

bool Mai_UpdateEmailInDB (const struct Usr_Data *UsrDat,const char NewEmail[Cns_MAX_BYTES_EMAIL_ADDRESS + 1])
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
   extern const char *Txt_If_you_just_requested_from_X_the_confirmation_of_your_email_Y_NO_HTML;
   extern const char *Txt_Confirmation_of_your_email_NO_HTML;
   extern const char *Txt_A_message_has_been_sent_to_email_address_X_to_confirm_that_address;
   extern const char *Txt_There_was_a_problem_sending_an_email_automatically;
   char FileNameMail[PATH_MAX + 1];
   FILE *FileMail;
   Lan_Language_t ToUsrLanguage;
   const char *UniqueNameEncrypted = Cry_GetUniqueNameEncrypted ();
   int ReturnCode;

   /***** Create temporary file for mail content *****/
   Mai_CreateFileNameMail (FileNameMail,&FileMail);

   /***** If I have no language, set language to current language *****/
   ToUsrLanguage = Gbl.Usrs.Me.UsrDat.Prefs.Language;
   if (ToUsrLanguage == Lan_LANGUAGE_UNKNOWN)
      ToUsrLanguage = Gbl.Prefs.Language;

   /***** Write mail content into file and close file *****/
   /* Welcome note */
   Mai_WriteWelcomeNoteEMail (FileMail,&Gbl.Usrs.Me.UsrDat,ToUsrLanguage);

   /* Store encrypted key in database */
   Mai_InsertMailKey (Gbl.Usrs.Me.UsrDat.Email,UniqueNameEncrypted);

   /* Message body */
   fprintf (FileMail,
	    Txt_If_you_just_requested_from_X_the_confirmation_of_your_email_Y_NO_HTML,
	    Cfg_URL_SWAD_CGI,Gbl.Usrs.Me.UsrDat.Email,
            Cfg_URL_SWAD_CGI,Act_GetActCod (ActCnfMai),UniqueNameEncrypted,
            Cfg_URL_SWAD_CGI);

   /* Footer note */
   Mai_WriteFootNoteEMail (FileMail,ToUsrLanguage);

   fclose (FileMail);

   /***** Call the script to send an email *****/
   ReturnCode = Mai_SendMailMsg (FileNameMail,
                                 Txt_Confirmation_of_your_email_NO_HTML,
                                 Gbl.Usrs.Me.UsrDat.Email);

   /***** Remove temporary file *****/
   unlink (FileNameMail);

   /***** Write message depending on return code *****/
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
   Par_GetParText ("key",MailKey,Mai_LENGTH_EMAIL_CONFIRM_KEY);

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
             Cfg_PATH_OUT_PRIVATE,Cry_GetUniqueNameEncrypted ());
   if ((*FileMail = fopen (FileNameMail,"wb")) == NULL)
      Err_ShowErrorAndExit ("Can not open file to send email.");
  }

/*****************************************************************************/
/************ Write a welcome note heading the automatic email ***************/
/*****************************************************************************/

void Mai_WriteWelcomeNoteEMail (FILE *FileMail,const struct Usr_Data *UsrDat,
                                Lan_Language_t ToUsrLanguage)
  {
   extern const char *Txt_Dear_NO_HTML[Usr_NUM_SEXS][1 + Lan_NUM_LANGUAGES];
   extern const char *Txt_user_NO_HTML[Usr_NUM_SEXS][1 + Lan_NUM_LANGUAGES];

   fprintf (FileMail,"%s %s:\n",
            Txt_Dear_NO_HTML[UsrDat->Sex][ToUsrLanguage],
            UsrDat->FrstName[0] ? UsrDat->FrstName :
                                  Txt_user_NO_HTML[UsrDat->Sex][ToUsrLanguage]);
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

Usr_Can_t Mai_ICanSeeOtherUsrEmail (const struct Usr_Data *UsrDat)
  {
   /***** I can see my email *****/
   if (Usr_ItsMe (UsrDat->UsrCod) == Usr_ME)
      return Usr_CAN;

   /***** Check if I have permission to see another user's email *****/
   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_STD:
	 /* If I am a student in the current course,
	    I can see the email of confirmed teachers */
	 return ((UsrDat->Roles.InCurrentCrs == Rol_NET ||	// A non-editing teacher
	          UsrDat->Roles.InCurrentCrs == Rol_TCH) &&	// or a teacher
	          UsrDat->Accepted) ? Usr_CAN :		// who accepted registration
	        		      Usr_CAN_NOT;
      case Rol_NET:
      case Rol_TCH:
	 /* If I am a teacher in the current course,
	    I can see the email of confirmed students and teachers */
         return (Enr_CheckIfUsrBelongsToCurrentCrs (UsrDat) == Usr_BELONG &&	// A user belonging to the current course
	         UsrDat->Accepted) ? Usr_CAN :					// who accepted registration
				     Usr_CAN_NOT;
      case Rol_DEG_ADM:
	 /* If I am an administrator of current degree,
	    I only can see the user's email of users from current degree */
	 return (Hie_CheckIfUsrBelongsTo (Hie_DEG,UsrDat->UsrCod,
	                                  Gbl.Hierarchy.Node[Hie_DEG].HieCod,
	                                  true) == Usr_BELONG) ? Usr_CAN :	// count only accepted courses
	                                			 Usr_CAN_NOT;
      case Rol_CTR_ADM:
	 /* If I am an administrator of current center,
	    I only can see the user's email of users from current center */
	 return (Hie_CheckIfUsrBelongsTo (Hie_CTR,UsrDat->UsrCod,
	                                  Gbl.Hierarchy.Node[Hie_CTR].HieCod,
	                                  true) == Usr_BELONG) ? Usr_CAN :	// count only accepted courses
	                                			 Usr_CAN_NOT;
      case Rol_INS_ADM:
	 /* If I am an administrator of current institution,
	    I only can see the user's email of users from current institution */
	 return (Hie_CheckIfUsrBelongsTo (Hie_INS,UsrDat->UsrCod,
	                                  Gbl.Hierarchy.Node[Hie_INS].HieCod,
	                                  true) == Usr_BELONG) ? Usr_CAN :	// count only accepted courses
								 Usr_CAN_NOT;
      case Rol_SYS_ADM:
	 return Usr_CAN;
      default:
	 return Usr_CAN_NOT;
     }
  }

/*****************************************************************************/
/************************* Initialize mail domain list ***********************/
/*****************************************************************************/

static void Mai_InitializeMailDomainList (struct Mai_Mails *Mails)
  {
   Mails->Num = 0;
   Mails->Lst = NULL;
   Mails->SelectedOrder = Mai_ORDER_DEFAULT;
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

/*****************************************************************************/
/***************************** Send mail message *****************************/
/*****************************************************************************/
// Return 0 on success
// Return != 0 on error

int Mai_SendMailMsg (const char FileNameMail[PATH_MAX + 1],
                     const char *Subject,
                     const char ToEmail[Cns_MAX_BYTES_EMAIL_ADDRESS + 1])
  {
   char Command[2048 +
		Cfg_MAX_BYTES_SMTP_PASSWORD +
		Cns_MAX_BYTES_EMAIL_ADDRESS +
		PATH_MAX]; // Command to execute for sending an email
   int ReturnCode;

   /***** Call the script to send an email *****/
   snprintf (Command,sizeof (Command),
	     "%s \"%s\" \"%s\" \"%s\" \"%s\" \"%s\" \"[%s] %s\" \"%s\"",
             Cfg_COMMAND_SEND_AUTOMATIC_EMAIL,
             Cfg_AUTOMATIC_EMAIL_SMTP_SERVER,
	     Cfg_AUTOMATIC_EMAIL_SMTP_PORT,
             Cfg_AUTOMATIC_EMAIL_FROM,
             Cfg_GetSMTPPassword (),
             ToEmail,
             Cfg_PLATFORM_SHORT_NAME,Subject,
             FileNameMail);
   ReturnCode = system (Command);
   if (ReturnCode == -1)	// The value returned is -1 on error
      Err_ShowErrorAndExit ("Error when running script to send email.");

   /* The exit code of the command will be WEXITSTATUS (ReturnCode) */
   return WEXITSTATUS (ReturnCode);
  }
