// swad_mail.c: all the stuff related to email

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2019 Antonio Ca�as Vargas

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
#include <stdlib.h>		// For calloc
#include <string.h>		// For string functions
#include <sys/wait.h>		// For the macro WEXITSTATUS
#include <unistd.h>		// For access, lstat, getpid, chdir, symlink, unlink

#include "swad_account.h"
#include "swad_box.h"
#include "swad_database.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_language.h"
#include "swad_mail.h"
#include "swad_parameter.h"
#include "swad_QR.h"
#include "swad_tab.h"
#include "swad_table.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/***************************** Private constants *****************************/
/*****************************************************************************/

#define Mai_LENGTH_EMAIL_CONFIRM_KEY Cry_BYTES_ENCRYPTED_STR_SHA256_BASE64

/*****************************************************************************/
/******************************* Private types *******************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private variables *****************************/
/*****************************************************************************/

static const char *Mai_EMAIL_SECTION_ID = "email_section";
static struct Mail *Mai_EditingMai = NULL;	// Static variable to keep the mail domain being edited

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Mai_GetParamMaiOrder (void);
static void Mai_PutIconToEditMailDomains (void);
static void Mai_EditMailDomainsInternal (void);
static void Mai_GetListMailDomainsAllowedForNotif (void);
static void Mai_GetMailDomain (const char *Email,char MailDomain[Cns_MAX_BYTES_EMAIL_ADDRESS + 1]);
static bool Mai_CheckIfMailDomainIsAllowedForNotif (const char MailDomain[Cns_MAX_BYTES_EMAIL_ADDRESS + 1]);

static void Mai_ListMailDomainsForEdition (void);
static void Mai_PutParamMaiCod (long MaiCod);

static void Mai_RenameMailDomain (Cns_ShrtOrFullName_t ShrtOrFullName);
static bool Mai_CheckIfMailDomainNameExists (const char *FieldName,const char *Name,long MaiCod);
static void Mai_UpdateMailDomainNameDB (long MaiCod,const char *FieldName,const char *NewMaiName);

static void Mai_PutFormToCreateMailDomain (void);
static void Mai_PutHeadMailDomains (void);
static void Mai_CreateMailDomain (struct Mail *Mai);

static void Mai_ListEmails (void);

static void Mai_ShowFormChangeUsrEmail (const struct UsrData *UsrDat,bool ItsMe,
				        bool IMustFillInEmail,bool IShouldConfirmEmail);

static void Mai_RemoveEmail (struct UsrData *UsrDat);
static void Mai_RemoveEmailFromDB (long UsrCod,const char Email[Cns_MAX_BYTES_EMAIL_ADDRESS + 1]);
static void Mai_NewUsrEmail (struct UsrData *UsrDat,bool ItsMe);
static void Mai_InsertMailKey (const char Email[Cns_MAX_BYTES_EMAIL_ADDRESS + 1],
                               const char MailKey[Mai_LENGTH_EMAIL_CONFIRM_KEY + 1]);

static void Mai_EditingMailDomainConstructor (void);
static void Mai_EditingMailDomainDestructor (void);

/*****************************************************************************/
/************************* List all the mail domains *************************/
/*****************************************************************************/

void Mai_SeeMailDomains (void)
  {
   extern const char *Hlp_START_Domains;
   extern const char *Txt_Email_domains_allowed_for_notifications;
   extern const char *Txt_EMAIL_DOMAIN_HELP_ORDER[3];
   extern const char *Txt_EMAIL_DOMAIN_ORDER[3];
   Mai_DomainsOrder_t Order;
   unsigned NumMai;

   /***** Get parameter with the type of order in the list of mail domains *****/
   Mai_GetParamMaiOrder ();

   /***** Get list of mail domains *****/
   Mai_GetListMailDomainsAllowedForNotif ();

   /***** Start box and table *****/
   Box_StartBoxTable (NULL,Txt_Email_domains_allowed_for_notifications,
                      Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM ? Mai_PutIconToEditMailDomains :
                                                               NULL,
                      Hlp_START_Domains,Box_NOT_CLOSABLE,2);

   /***** Write heading *****/
   Tbl_TR_Begin (NULL);
   for (Order = Mai_ORDER_BY_DOMAIN;
	Order <= Mai_ORDER_BY_USERS;
	Order++)
     {
      Tbl_TH_Begin ("class=\"LEFT_MIDDLE\"");
      Frm_StartForm (ActSeeMai);
      Par_PutHiddenParamUnsigned ("Order",(unsigned) Order);
      Frm_LinkFormSubmit (Txt_EMAIL_DOMAIN_HELP_ORDER[Order],"TIT_TBL",NULL);
      if (Order == Gbl.Mails.SelectedOrder)
         fprintf (Gbl.F.Out,"<u>");
      fprintf (Gbl.F.Out,"%s",Txt_EMAIL_DOMAIN_ORDER[Order]);
      if (Order == Gbl.Mails.SelectedOrder)
         fprintf (Gbl.F.Out,"</u>");
      fprintf (Gbl.F.Out,"</a>");
      Frm_EndForm ();
      Tbl_TH_End ();
     }
   Tbl_TR_End ();

   /***** Write all the mail domains *****/
   for (NumMai = 0;
	NumMai < Gbl.Mails.Num;
	NumMai++)
     {
      /* Write data of this mail domain */
      Tbl_TR_Begin (NULL);

      Tbl_TD_Begin ("class=\"DAT LEFT_TOP\"");
      fprintf (Gbl.F.Out,"%s",Gbl.Mails.Lst[NumMai].Domain);
      Tbl_TD_End ();

      Tbl_TD_Begin ("class=\"DAT LEFT_TOP\"");
      fprintf (Gbl.F.Out,"%s",Gbl.Mails.Lst[NumMai].Info);
      Tbl_TD_End ();

      Tbl_TD_Begin ("class=\"DAT RIGHT_TOP\"");
      fprintf (Gbl.F.Out,"%u",Gbl.Mails.Lst[NumMai].NumUsrs);
      Tbl_TD_End ();

      Tbl_TR_End ();
     }

   /***** End table and box *****/
   Box_EndBoxTable ();

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

static void Mai_PutIconToEditMailDomains (void)
  {
   Ico_PutContextualIconToEdit (ActEdiMai,NULL);
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
/************************* List all the mail domains *************************/
/*****************************************************************************/

static void Mai_GetListMailDomainsAllowedForNotif (void)
  {
   static const char *OrderBySubQuery[Mai_NUM_ORDERS] =
     {
      "Domain,Info,N DESC",	// Mai_ORDER_BY_DOMAIN
      "Info,Domain,N DESC",	// Mai_ORDER_BY_INFO
      "N DESC,Info,Domain",	// Mai_ORDER_BY_USERS
     };
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;
   unsigned NumMai;
   struct Mail *Mai;

   // Query uses temporary tables for speed
   // Query uses two identical temporary tables...
   // ...because a unique temporary table can not be used twice in the same query

   /***** Create temporary table with all the mail domains present in users' emails table *****/
   DB_Query ("can not remove temporary tables",
	     "DROP TEMPORARY TABLE IF EXISTS T1,T2");

   DB_Query ("can not create temporary table",
	     "CREATE TEMPORARY TABLE T1 ENGINE=MEMORY"
	     " SELECT SUBSTRING_INDEX(E_mail,'@',-1) AS Domain,COUNT(*) as N"
    	     " FROM usr_emails GROUP BY Domain");

   DB_Query ("can not create temporary table",
	     "CREATE TEMPORARY TABLE T2 ENGINE=MEMORY SELECT * FROM T1");

   /***** Get mail domains from database *****/
   NumRows = DB_QuerySELECT (&mysql_res,"can not get mail domains",
			     "(SELECT mail_domains.MaiCod,"
				     "mail_domains.Domain AS Domain,"
				     "mail_domains.Info AS Info,"
				     "T1.N AS N"
			     " FROM mail_domains,T1"
			     " WHERE mail_domains.Domain=T1.Domain COLLATE 'latin1_bin')"
			     " UNION "
			     "(SELECT MaiCod,"
				     "Domain,"
				     "Info,"
				     "0 AS N"
			     " FROM mail_domains"
			     " WHERE Domain NOT IN"
			     " (SELECT Domain COLLATE 'latin1_bin' FROM T2))"
			     " ORDER BY %s",	// COLLATE necessary to avoid error in comparisons
			     OrderBySubQuery[Gbl.Mails.SelectedOrder]);

   if (NumRows) // Mail domains found...
     {
      Gbl.Mails.Num = (unsigned) NumRows;

      /***** Create list with places *****/
      if ((Gbl.Mails.Lst = (struct Mail *) calloc (NumRows,sizeof (struct Mail))) == NULL)
          Lay_NotEnoughMemoryExit ();

      /***** Get the mail domains *****/
      for (NumMai = 0;
	   NumMai < Gbl.Mails.Num;
	   NumMai++)
        {
         Mai = &(Gbl.Mails.Lst[NumMai]);

         /* Get next mail */
         row = mysql_fetch_row (mysql_res);

         /* Get mail code (row[0]) */
         if ((Mai->MaiCod = Str_ConvertStrCodToLongCod (row[0])) < 0)
            Lay_ShowErrorAndExit ("Wrong code of mail domain.");

         /* Get the mail domain (row[1]) */
         Str_Copy (Mai->Domain,row[1],
                   Cns_MAX_BYTES_EMAIL_ADDRESS);

         /* Get the mail domain info (row[2]) */
         Str_Copy (Mai->Info,row[2],
                   Mai_MAX_BYTES_MAIL_INFO);

         /* Get number of users (row[3]) */
         if (sscanf (row[3],"%u",&(Mai->NumUsrs)) != 1)
            Mai->NumUsrs = 0;
        }
     }
   else
      Gbl.Mails.Num = 0;

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   /***** Drop temporary table *****/
   DB_Query ("can not remove temporary tables",
	     "DROP TEMPORARY TABLE IF EXISTS T1,T2");
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
   return Mai_CheckIfMailDomainIsAllowedForNotif (MailDomain);
  }

/*****************************************************************************/
/********************** Get mailbox from email address ***********************/
/*****************************************************************************/

static void Mai_GetMailDomain (const char *Email,char MailDomain[Cns_MAX_BYTES_EMAIL_ADDRESS + 1])
  {
   const char *Ptr;

   MailDomain[0] = '\0';	// Return empty mailbox on error

   if ((Ptr = strchr (Email,(int) '@')))	// Find first '@' in address
      if (Ptr != Email)				// '@' is not the first character in Email
        {
         Ptr++;					// Skip '@'
         if (strchr (Ptr,(int) '@') == NULL)	// No more '@' found
            Str_Copy (MailDomain,Ptr,
                      Cns_MAX_BYTES_EMAIL_ADDRESS);
        }
  }

/*****************************************************************************/
/************ Check if a mail domain is allowed for notifications ************/
/*****************************************************************************/

static bool Mai_CheckIfMailDomainIsAllowedForNotif (const char MailDomain[Cns_MAX_BYTES_EMAIL_ADDRESS + 1])
  {
   /***** Get number of mail_domains with a name from database *****/
   return (DB_QueryCOUNT ("can not check if a mail domain"
			  " is allowed for notifications",
			  "SELECT COUNT(*) FROM mail_domains"
			  " WHERE Domain='%s'",
			  MailDomain) != 0);
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
   unsigned long NumRows;

   /***** Clear data *****/
   Mai->Domain[0] = Mai->Info[0] = '\0';

   /***** Check if mail code is correct *****/
   if (Mai->MaiCod > 0)
     {
      /***** Get data of a mail domain from database *****/
      NumRows = DB_QuerySELECT (&mysql_res,"can not get data"
					   " of a mail domain",
				"SELECT Domain,Info FROM mail_domains"
				" WHERE MaiCod=%ld",
				Mai->MaiCod);

      if (NumRows) // Mail found...
        {
         /* Get row */
         row = mysql_fetch_row (mysql_res);

         /* Get the short name of the mail (row[0]) */
         Str_Copy (Mai->Domain,row[0],
                   Cns_MAX_BYTES_EMAIL_ADDRESS);

         /* Get the full name of the mail (row[1]) */
         Str_Copy (Mai->Info,row[1],
                   Mai_MAX_BYTES_MAIL_INFO);
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
      free ((void *) Gbl.Mails.Lst);
      Gbl.Mails.Lst = NULL;
      Gbl.Mails.Num = 0;
     }
  }

/*****************************************************************************/
/************************ List all the mail domains **************************/
/*****************************************************************************/

static void Mai_ListMailDomainsForEdition (void)
  {
   extern const char *Hlp_START_Domains_edit;
   extern const char *Txt_Email_domains_allowed_for_notifications;
   unsigned NumMai;
   struct Mail *Mai;

   /***** Start box and table *****/
   Box_StartBoxTable (NULL,Txt_Email_domains_allowed_for_notifications,NULL,
                      Hlp_START_Domains_edit,Box_NOT_CLOSABLE,2);

   /***** Write heading *****/
   Mai_PutHeadMailDomains ();

   /***** Write all the mail domains *****/
   for (NumMai = 0;
	NumMai < Gbl.Mails.Num;
	NumMai++)
     {
      Mai = &Gbl.Mails.Lst[NumMai];

      Tbl_TR_Begin (NULL);

      /* Put icon to remove mail */
      Tbl_TD_Begin ("class=\"BM\"");
      Frm_StartForm (ActRemMai);
      Mai_PutParamMaiCod (Mai->MaiCod);
      Ico_PutIconRemove ();
      Frm_EndForm ();
      Tbl_TD_End ();

      /* Mail code */
      Tbl_TD_Begin ("class=\"DAT RIGHT_MIDDLE\"");
      fprintf (Gbl.F.Out,"%ld",Mai->MaiCod);
      Tbl_TD_End ();

      /* Mail domain */
      Tbl_TD_Begin ("class=\"CENTER_MIDDLE\"");
      Frm_StartForm (ActRenMaiSho);
      Mai_PutParamMaiCod (Mai->MaiCod);
      fprintf (Gbl.F.Out,"<input type=\"text\" name=\"Domain\""
	                 " size=\"15\" maxlength=\"%u\" value=\"%s\""
                         " onchange=\"document.getElementById('%s').submit();\" />",
               Cns_MAX_CHARS_EMAIL_ADDRESS,Mai->Domain,
               Gbl.Form.Id);
      Frm_EndForm ();
      Tbl_TD_End ();

      /* Mail domain info */
      Tbl_TD_Begin ("class=\"CENTER_MIDDLE\"");
      Frm_StartForm (ActRenMaiFul);
      Mai_PutParamMaiCod (Mai->MaiCod);
      fprintf (Gbl.F.Out,"<input type=\"text\" name=\"Info\""
	                 " size=\"40\" maxlength=\"%u\" value=\"%s\""
                         " onchange=\"document.getElementById('%s').submit();\" />",
               Mai_MAX_CHARS_MAIL_INFO,Mai->Info,
               Gbl.Form.Id);
      Frm_EndForm ();
      Tbl_TD_End ();

      /* Number of users */
      Tbl_TD_Begin ("class=\"DAT RIGHT_MIDDLE\"");
      fprintf (Gbl.F.Out,"%u",Mai->NumUsrs);
      Tbl_TD_End ();

      Tbl_TR_End ();
     }

   /***** End table and box *****/
   Box_EndBoxTable ();
  }

/*****************************************************************************/
/******************** Write parameter with code of mail **********************/
/*****************************************************************************/

static void Mai_PutParamMaiCod (long MaiCod)
  {
   Par_PutHiddenParamLong ("MaiCod",MaiCod);
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

   /***** Get mail code *****/
   if ((Mai_EditingMai->MaiCod = Mai_GetParamMaiCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of mail domain is missing.");

   /***** Get data of the mail from database *****/
   Mai_GetDataOfMailDomainByCod (Mai_EditingMai);

   /***** Remove mail *****/
   DB_QueryDELETE ("can not remove a mail domain",
		   "DELETE FROM mail_domains WHERE MaiCod=%ld",
		   Mai_EditingMai->MaiCod);

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
   extern const char *Txt_You_can_not_leave_the_name_of_the_email_domain_X_empty;
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
   if ((Mai_EditingMai->MaiCod = Mai_GetParamMaiCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of mail domain is missing.");

   /* Get the new name for the mail */
   Par_GetParToText (ParamName,NewMaiName,MaxBytes);

   /***** Get from the database the old names of the mail *****/
   Mai_GetDataOfMailDomainByCod (Mai_EditingMai);

   /***** Check if new name is empty *****/
   if (!NewMaiName[0])
      Ale_CreateAlert (Ale_WARNING,Mai_EMAIL_SECTION_ID,
	               Txt_You_can_not_leave_the_name_of_the_email_domain_X_empty,
		       CurrentMaiName);
   else
     {
      /***** Check if old and new names are the same
             (this happens when return is pressed without changes) *****/
      if (strcmp (CurrentMaiName,NewMaiName))	// Different names
        {
         /***** If mail was in database... *****/
         if (Mai_CheckIfMailDomainNameExists (ParamName,NewMaiName,Mai_EditingMai->MaiCod))
	    Ale_CreateAlert (Ale_WARNING,Mai_EMAIL_SECTION_ID,
		             Txt_The_email_domain_X_already_exists,
                             NewMaiName);
         else
           {
            /* Update the table changing old name by new name */
            Mai_UpdateMailDomainNameDB (Mai_EditingMai->MaiCod,FieldName,NewMaiName);

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

   /***** Update name *****/
   Str_Copy (CurrentMaiName,NewMaiName,
             MaxBytes);
  }

/*****************************************************************************/
/********************** Check if the name of mail exists *********************/
/*****************************************************************************/

static bool Mai_CheckIfMailDomainNameExists (const char *FieldName,const char *Name,long MaiCod)
  {
   /***** Get number of mail_domains with a name from database *****/
   return (DB_QueryCOUNT ("can not check if the name"
			  " of a mail domain already existed",
			  "SELECT COUNT(*) FROM mail_domains"
			  " WHERE %s='%s' AND MaiCod<>%ld",
			  FieldName,Name,MaiCod) != 0);
  }

/*****************************************************************************/
/****************** Update name in table of mail domains *********************/
/*****************************************************************************/

static void Mai_UpdateMailDomainNameDB (long MaiCod,const char *FieldName,const char *NewMaiName)
  {
   /***** Update mail domain changing old name by new name */
   DB_QueryUPDATE ("can not update the name of a mail domain",
		   "UPDATE mail_domains SET %s='%s' WHERE MaiCod=%ld",
	           FieldName,NewMaiName,MaiCod);
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

   /***** Start form *****/
   Frm_StartForm (ActNewMai);

   /***** Start box and table *****/
   Box_StartBoxTable (NULL,Txt_New_email_domain,NULL,
                      Hlp_START_Domains_edit,Box_NOT_CLOSABLE,2);

   /***** Write heading *****/
   Tbl_TR_Begin (NULL);

   Tbl_TH_Begin ("class=\"LEFT_MIDDLE\"");
   fprintf (Gbl.F.Out,"%s",Txt_EMAIL_DOMAIN_ORDER[Mai_ORDER_BY_DOMAIN]);
   Tbl_TH_End ();

   Tbl_TH_Begin ("class=\"LEFT_MIDDLE\"");
   fprintf (Gbl.F.Out,"%s",Txt_EMAIL_DOMAIN_ORDER[Mai_ORDER_BY_INFO]);
   Tbl_TH_End ();

   Tbl_TR_End ();

   Tbl_TR_Begin (NULL);

   /***** Mail domain *****/
   Tbl_TD_Begin ("class=\"CENTER_MIDDLE\"");
   fprintf (Gbl.F.Out,"<input type=\"text\" name=\"Domain\""
                      " size=\"15\" maxlength=\"%u\" value=\"%s\""
                      " required=\"required\" />",
            Cns_MAX_CHARS_EMAIL_ADDRESS,Mai_EditingMai->Domain);
   Tbl_TD_End ();

   /***** Mail domain info *****/
   Tbl_TD_Begin ("class=\"CENTER_MIDDLE\"");
   fprintf (Gbl.F.Out,"<input type=\"text\" name=\"Info\""
                      " size=\"40\" maxlength=\"%u\" value=\"%s\""
                      " required=\"required\" />",
            Mai_MAX_CHARS_MAIL_INFO,Mai_EditingMai->Info);
   Tbl_TD_End ();

   Tbl_TD_Empty (1);

   Tbl_TR_End ();

   /***** End table, send button and end box *****/
   Box_EndBoxTableWithButton (Btn_CREATE_BUTTON,Txt_Create_email_domain);

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

   Tbl_TR_Begin (NULL);

   Tbl_TH_Begin ("class=\"BM\"");
   Tbl_TH_End ();

   Tbl_TH_Begin ("class=\"RIGHT_MIDDLE\"");
   fprintf (Gbl.F.Out,"%s",Txt_Code);
   Tbl_TH_End ();

   Tbl_TH_Begin ("class=\"LEFT_MIDDLE\"");
   fprintf (Gbl.F.Out,"%s",Txt_EMAIL_DOMAIN_ORDER[Mai_ORDER_BY_DOMAIN]);
   Tbl_TH_End ();

   Tbl_TH_Begin ("class=\"LEFT_MIDDLE\"");
   fprintf (Gbl.F.Out,"%s",Txt_EMAIL_DOMAIN_ORDER[Mai_ORDER_BY_INFO  ]);
   Tbl_TH_End ();

   Tbl_TH_Begin ("class=\"RIGHT_MIDDLE\"");
   fprintf (Gbl.F.Out,"%s",Txt_EMAIL_DOMAIN_ORDER[Mai_ORDER_BY_USERS ]);
   Tbl_TH_End ();

   Tbl_TR_End ();
  }

/*****************************************************************************/
/******************* Receive form to create a new mail ***********************/
/*****************************************************************************/

void Mai_RecFormNewMailDomain (void)
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
      if (Mai_CheckIfMailDomainNameExists ("Domain",Mai_EditingMai->Domain,-1L))
         Ale_CreateAlert (Ale_WARNING,NULL,
                          Txt_The_email_domain_X_already_exists,
                          Mai_EditingMai->Domain);
      else if (Mai_CheckIfMailDomainNameExists ("Info",Mai_EditingMai->Info,-1L))
         Ale_CreateAlert (Ale_WARNING,NULL,
                          Txt_The_email_domain_X_already_exists,
                          Mai_EditingMai->Info);
      else	// Add new mail to database
        {
         Mai_CreateMailDomain (Mai_EditingMai);
	 Ale_ShowAlert (Ale_SUCCESS,Txt_Created_new_email_domain_X,
			Mai_EditingMai->Domain);
        }
     }
   else	// If there is not a mail name
      Ale_CreateAlert (Ale_WARNING,NULL,
	               Txt_You_must_specify_the_short_name_and_the_full_name_of_the_new_email_domain);
  }

/*****************************************************************************/
/************************** Create a new mail domain *************************/
/*****************************************************************************/

static void Mai_CreateMailDomain (struct Mail *Mai)
  {
   /***** Create a new mail *****/
   DB_QueryINSERT ("can not create mail domain",
		   "INSERT INTO mail_domains"
		   " (Domain,Info)"
		   " VALUES"
		   " ('%s','%s')",
	           Mai->Domain,Mai->Info);
  }

/*****************************************************************************/
/************** Request edition of works of users of the course **************/
/*****************************************************************************/

void Mai_ReqUsrsToListEmails (void)
  {
   extern const char *Hlp_MESSAGES_Email;
   extern const char *Txt_Email;
   extern const char *Txt_View_email_addresses;

   /***** List users to select some of them *****/
   Usr_PutFormToSelectUsrsToGoToAct (ActMaiUsr,NULL,
				     Txt_Email,
	                             Hlp_MESSAGES_Email,
	                             Txt_View_email_addresses);
  }

/*****************************************************************************/
/****** Get and check list of selected users, and show users' emails  ********/
/*****************************************************************************/

void Mai_GetSelectedUsrsAndListEmails (void)
  {
   Usr_GetSelectedUsrsAndGoToAct (Mai_ListEmails,		// when user(s) selected
                                  Mai_ReqUsrsToListEmails);	// when no user selected
  }

/*****************************************************************************/
/****** List the emails of all the students to creates an email message ******/
/*****************************************************************************/

#define Mai_MAX_BYTES_STR_ADDR (256 * 1024 - 1)

static void Mai_ListEmails (void)
  {
   extern const char *Hlp_MESSAGES_Email;
   extern const char *The_ClassFormOutBoxBold[The_NUM_THEMES];
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

   /***** Start the box used to list the emails *****/
   Box_StartBox (NULL,Txt_Email_addresses,NULL,
		 Hlp_MESSAGES_Email,Box_NOT_CLOSABLE);

   /***** Start list with users' email addresses *****/
   fprintf (Gbl.F.Out,"<div class=\"DAT_SMALL CENTER_MIDDLE\">");

   /***** Initialize structure with user's data *****/
   Usr_UsrDataConstructor (&UsrDat);

   /***** Get email addresses of the selected users *****/
   StrAddresses[0] = '\0';
   Ptr = Gbl.Usrs.Selected.List[Rol_UNK];
   while (*Ptr)
     {
      /* Get next user */
      Par_GetNextStrUntilSeparParamMult (&Ptr,UsrDat.EncryptedUsrCod,
					 Cry_BYTES_ENCRYPTED_STR_SHA256_BASE64);
      Usr_GetUsrCodFromEncryptedUsrCod (&UsrDat);

      /* Get user's email */
      Mai_GetEmailFromUsrCod (&UsrDat);

      if (UsrDat.Email[0])
	{
	 NumUsrsWithEmail++;

	 /* Check if users has accepted inscription in current course */
	 UsrDat.Accepted = Usr_CheckIfUsrHasAcceptedInCurrentCrs (&UsrDat);

	 if (UsrDat.Accepted) // If student has email and has accepted
	   {
	    if (NumAcceptedUsrsWithEmail > 0)
	      {
	       fprintf (Gbl.F.Out,", ");
	       LengthStrAddr ++;
	       if (LengthStrAddr > Mai_MAX_BYTES_STR_ADDR)
		  Lay_ShowErrorAndExit ("The space allocated to store email addresses is full.");
	       Str_Concat (StrAddresses,",",
			   Mai_MAX_BYTES_STR_ADDR);
	      }
	    LengthStrAddr += strlen (UsrDat.Email);
	    if (LengthStrAddr > Mai_MAX_BYTES_STR_ADDR)
	       Lay_ShowErrorAndExit ("The space allocated to store email addresses is full.");
	    Str_Concat (StrAddresses,UsrDat.Email,
			Mai_MAX_BYTES_STR_ADDR);
	    fprintf (Gbl.F.Out,"<a href=\"mailto:%s?subject=%s\">%s</a>",
		     UsrDat.Email,Gbl.Hierarchy.Crs.FullName,UsrDat.Email);

	    NumAcceptedUsrsWithEmail++;
	   }
	}
     }

   /***** Free memory used for user's data *****/
   Usr_UsrDataDestructor (&UsrDat);

   /***** End list with users' email addresses *****/
   fprintf (Gbl.F.Out,"</div>");

   /***** Show a message with the number of users with email ****/
   fprintf (Gbl.F.Out,"<div class=\"DAT CENTER_MIDDLE\">");
   fprintf (Gbl.F.Out,Txt_X_users_who_have_email,
	    NumUsrsWithEmail);
   fprintf (Gbl.F.Out,"</div>");

   /***** Show a message with the number of users who have accepted and have email ****/
   fprintf (Gbl.F.Out,"<div class=\"DAT CENTER_MIDDLE\">");
   fprintf (Gbl.F.Out,Txt_X_users_who_have_accepted_and_who_have_email,
	    NumAcceptedUsrsWithEmail);
   fprintf (Gbl.F.Out,"</div>");

   /***** Icon to open the client email program *****/
   fprintf (Gbl.F.Out,"<div class=\"CONTEXT_MENU\">"
		      "<a href=\"mailto:%s?subject=%s&cc=%s&bcc=%s\""
		      " title=\"%s\" class=\"%s\">",
	    Gbl.Usrs.Me.UsrDat.Email,
	    Gbl.Hierarchy.Crs.FullName,
	    Gbl.Usrs.Me.UsrDat.Email,
	    StrAddresses,
	    Txt_Create_email_message,
	    The_ClassFormOutBoxBold[Gbl.Prefs.Theme]);
   Ico_PutIconTextLink ("marker.svg",
			Txt_Create_email_message);
   fprintf (Gbl.F.Out,"</a>"
		      "</div>");

   /***** End the box used to list the emails *****/
   Box_EndBox ();
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
   unsigned long NumRows;
   bool Found;

   /***** Get current (last updated) user's nickname from database *****/
   NumRows = DB_QuerySELECT (&mysql_res,"can not get email address",
			     "SELECT E_mail,Confirmed FROM usr_emails"
			     " WHERE UsrCod=%ld"
			     " ORDER BY CreatTime DESC LIMIT 1",
			     UsrDat->UsrCod);

   if (NumRows == 0)
     {
      UsrDat->Email[0] = '\0';
      UsrDat->EmailConfirmed = false;
      Found = false;
     }
   else
     {
      /* Get email */
      row = mysql_fetch_row (mysql_res);
      Str_Copy (UsrDat->Email,row[0],
                Cns_MAX_BYTES_EMAIL_ADDRESS);
      UsrDat->EmailConfirmed = (row[1][0] == 'Y');
      Found = true;
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return Found;
  }

/*****************************************************************************/
/************* Get user's code of a user from his/her email ******************/
/*****************************************************************************/
// Returns -1L if email not found

long Mai_GetUsrCodFromEmail (const char Email[Cns_MAX_BYTES_EMAIL_ADDRESS + 1])
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumUsrs;
   long UsrCod = -1L;

   if (Email)
      if (Email[0])
	{
	 /***** Get user's code from database *****/
	 /* Check if user code from table usr_emails is also in table usr_data */
	 NumUsrs = (unsigned) DB_QuerySELECT (&mysql_res,"can not get user's code",
					      "SELECT usr_emails.UsrCod"
					      " FROM usr_emails,usr_data"
					      " WHERE usr_emails.E_mail='%s'"
					      " AND usr_emails.UsrCod=usr_data.UsrCod",
					      Email);
	 if (NumUsrs == 0)
	    /* User not found for this email ==> set user's code to void */
	    UsrCod = -1L;
	 else if (NumUsrs == 1)	// One user found
	   {
	    /* Get row */
	    row = mysql_fetch_row (mysql_res);

	    /* Get user's code */
	    UsrCod = Str_ConvertStrCodToLongCod (row[0]);
	   }
	 else	// NumRows > 1 ==> impossible, an email can not be reapeated
	    Lay_ShowErrorAndExit ("Internal error: email is repeated in database.");

	 /***** Free structure that stores the query result *****/
	 DB_FreeMySQLResult (&mysql_res);
	}

   return UsrCod;
  }

/*****************************************************************************/
/*********************** Show form to change my email ************************/
/*****************************************************************************/

void Mai_ShowFormChangeMyEmail (bool IMustFillInEmail,bool IShouldConfirmEmail)
  {
   extern const char *Hlp_PROFILE_Account;
   extern const char *Txt_Email;
   char StrRecordWidth[10 + 1];

   /***** Start section *****/
   Lay_StartSection (Mai_EMAIL_SECTION_ID);

   /***** Start box *****/
   snprintf (StrRecordWidth,sizeof (StrRecordWidth),
	     "%upx",
	     Rec_RECORD_WIDTH);
   Box_StartBox (StrRecordWidth,Txt_Email,Acc_PutLinkToRemoveMyAccount,
                 Hlp_PROFILE_Account,Box_NOT_CLOSABLE);

   /***** Show form to change email *****/
   Mai_ShowFormChangeUsrEmail (&Gbl.Usrs.Me.UsrDat,
			       true,	// ItsMe
			       IMustFillInEmail,IShouldConfirmEmail);

   /***** End box *****/
   Box_EndBox ();

   /***** End section *****/
   Lay_EndSection ();
  }

/*****************************************************************************/
/****************** Show form to change another user's email *****************/
/*****************************************************************************/

void Mai_ShowFormChangeOtherUsrEmail (void)
  {
   extern const char *Hlp_PROFILE_Account;
   extern const char *Txt_Email;
   char StrRecordWidth[10 + 1];

   /***** Start section *****/
   Lay_StartSection (Mai_EMAIL_SECTION_ID);

   /***** Start box *****/
   snprintf (StrRecordWidth,sizeof (StrRecordWidth),
	     "%upx",
	     Rec_RECORD_WIDTH);
   Box_StartBox (StrRecordWidth,Txt_Email,NULL,
                 Hlp_PROFILE_Account,Box_NOT_CLOSABLE);

   /***** Show form to change email *****/
   Mai_ShowFormChangeUsrEmail (&Gbl.Usrs.Other.UsrDat,
			       false,	// ItsMe
			       false,	// IMustFillInEmail
			       false);	// IShouldConfirmEmail

   /***** End box *****/
   Box_EndBox ();

   /***** End section *****/
   Lay_EndSection ();
  }

/*****************************************************************************/
/********************** Show form to change user's email *********************/
/*****************************************************************************/

static void Mai_ShowFormChangeUsrEmail (const struct UsrData *UsrDat,bool ItsMe,
				        bool IMustFillInEmail,bool IShouldConfirmEmail)
  {
   extern const char *The_ClassFormInBox[The_NUM_THEMES];
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
   Act_Action_t NextAction;

   /***** Show possible alerts *****/
   Ale_ShowAlerts (Mai_EMAIL_SECTION_ID);

   /***** Help message *****/
   if (IMustFillInEmail)
      Ale_ShowAlert (Ale_WARNING,Txt_Before_going_to_any_other_option_you_must_fill_in_your_email_address);
   else if (IShouldConfirmEmail)
      Ale_ShowAlert (Ale_WARNING,Txt_Please_confirm_your_email_address);

   /***** Get my emails *****/
   NumEmails = (unsigned) DB_QuerySELECT (&mysql_res,"can not get"
						     " old email addresses"
						     " of a user",
					  "SELECT E_mail,Confirmed"
					  " FROM usr_emails"
					  " WHERE UsrCod=%ld"
					  " ORDER BY CreatTime DESC",
					  UsrDat->UsrCod);

   /***** Start table *****/
   Tbl_TABLE_BeginWidePadding (2);

   /***** List emails *****/
   for (NumEmail = 1;
	NumEmail <= NumEmails;
	NumEmail++)
     {
      /* Get email */
      row = mysql_fetch_row (mysql_res);
      Confirmed = (row[1][0] == 'Y');

      Tbl_TR_Begin (NULL);

      if (NumEmail == 1)
	{
	 /* The first mail is the current one */
	 Tbl_TD_Begin ("class=\"REC_C1_BOT RIGHT_TOP\"");
	 fprintf (Gbl.F.Out,"<label for=\"Email\" class=\"%s\">"
			    "%s:"
			    "</label>",
		  The_ClassFormInBox[Gbl.Prefs.Theme],
		  Txt_Current_email);
	 Tbl_TD_End ();

	 Tbl_TD_Begin ("class=\"REC_C2_BOT LEFT_TOP USR_ID\"");
	}
      else	// NumEmail >= 2
	{
	 if (NumEmail == 2)
	   {
	    Tbl_TD_Begin ("rowspan=\"%u\" class=\"REC_C1_BOT RIGHT_TOP\"",
		          NumEmails - 1);
	    fprintf (Gbl.F.Out,"<label for=\"Email\" class=\"%s\">"
			       "%s:"
			       "</label>",
		     The_ClassFormInBox[Gbl.Prefs.Theme],
		     Txt_Other_emails);
	    Tbl_TD_End ();
	   }

	 Tbl_TD_Begin ("class=\"REC_C2_BOT LEFT_TOP DAT\"");
	}

      /* Form to remove email */
      if (ItsMe)
	 Frm_StartFormAnchor (ActRemMyMai,Mai_EMAIL_SECTION_ID);
      else
	{
	 switch (UsrDat->Roles.InCurrentCrs.Role)
	   {
	    case Rol_STD:
	       NextAction = ActRemMaiStd;
	       break;
	    case Rol_NET:
	    case Rol_TCH:
	       NextAction = ActRemMaiTch;
	       break;
	    default:	// Guest, user or admin
	       NextAction = ActRemMaiOth;
	       break;
	   }
	 Frm_StartFormAnchor (NextAction,Mai_EMAIL_SECTION_ID);
	 Usr_PutParamUsrCodEncrypted (UsrDat->EncryptedUsrCod);
	}
      fprintf (Gbl.F.Out,"<input type=\"hidden\" name=\"Email\""
	                 " value=\"%s\" />",
	       row[0]);
      Ico_PutIconRemove ();
      Frm_EndForm ();

      /* Email */
      fprintf (Gbl.F.Out,"%s",row[0]);

      /* Email confirmed? */
      if (Confirmed)
	{
	 snprintf (Gbl.Title,sizeof (Gbl.Title),
	           Txt_Email_X_confirmed,
		   row[0]);
	 fprintf (Gbl.F.Out,"<img src=\"%s/check-circle.svg\""
			    " alt=\"%s\" title=\"%s\""
			    " class=\"ICO16x16\" />",
		  Cfg_URL_ICON_PUBLIC,
		  Gbl.Title,Gbl.Title);
	}

      /* Form to change user's email */
      if (NumEmail > 1 || (ItsMe && !Confirmed))
	{
         fprintf (Gbl.F.Out,"<br />");
	 if (ItsMe)
	    Frm_StartFormAnchor (ActChgMyMai,Mai_EMAIL_SECTION_ID);
	 else
	   {
	    switch (UsrDat->Roles.InCurrentCrs.Role)
	      {
	       case Rol_STD:
		  NextAction = ActNewMaiStd;
		  break;
	       case Rol_NET:
	       case Rol_TCH:
		  NextAction = ActNewMaiTch;
		  break;
	       default:	// Guest, user or admin
		  NextAction = ActNewMaiOth;
		  break;
	      }
	    Frm_StartFormAnchor (NextAction,Mai_EMAIL_SECTION_ID);
	    Usr_PutParamUsrCodEncrypted (UsrDat->EncryptedUsrCod);
	   }
	 fprintf (Gbl.F.Out,"<input type=\"hidden\" name=\"NewEmail\" value=\"%s\" />",
		  row[0]);	// Email
         Btn_PutConfirmButtonInline ((ItsMe && NumEmail == 1) ? Txt_Confirm_email :
			                                        Txt_Use_this_email);
	 Frm_EndForm ();
	 Tbl_TD_End ();
	}

      Tbl_TD_End ();
      Tbl_TR_End ();
     }

   /***** Form to enter new email *****/
   Tbl_TR_Begin (NULL);

   Tbl_TD_Begin ("class=\"REC_C1_BOT RIGHT_TOP\"");
   fprintf (Gbl.F.Out,"<label for=\"NewEmail\" class=\"%s\">%s:</label>",
            The_ClassFormInBox[Gbl.Prefs.Theme],
            NumEmails ? Txt_New_email :	// A new email
        	        Txt_Email);	// The first email
   Tbl_TD_End ();

   Tbl_TD_Begin ("class=\"REC_C2_BOT LEFT_TOP DAT\"");
   if (ItsMe)
      Frm_StartFormAnchor (ActChgMyMai,Mai_EMAIL_SECTION_ID);
   else
     {
      switch (UsrDat->Roles.InCurrentCrs.Role)
	{
	 case Rol_STD:
	    NextAction = ActNewMaiStd;
	    break;
	 case Rol_NET:
	 case Rol_TCH:
	    NextAction = ActNewMaiTch;
	    break;
	 default:	// Guest, user or admin
	    NextAction = ActNewMaiOth;
	    break;
	}
      Frm_StartFormAnchor (NextAction,Mai_EMAIL_SECTION_ID);
      Usr_PutParamUsrCodEncrypted (UsrDat->EncryptedUsrCod);
     }
   fprintf (Gbl.F.Out,"<input type=\"email\" id=\"NewEmail\" name=\"NewEmail\""
	              " size=\"18\" maxlength=\"%u\" value=\"%s\" />"
	              "<br />",
            Cns_MAX_CHARS_EMAIL_ADDRESS,
            Gbl.Usrs.Me.UsrDat.Email);
   Btn_PutCreateButtonInline (NumEmails ? Txt_Change_email :	// User already has an email address
        	                          Txt_Save_changes);		// User has no email address yet
   Frm_EndForm ();
   Tbl_TD_End ();

   Tbl_TR_End ();

   /***** End table *****/
   Tbl_TABLE_End ();
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
      Mai_RemoveEmailFromDB (UsrDat->UsrCod,Email);

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
/*************** Remove an old email address from database *******************/
/*****************************************************************************/

static void Mai_RemoveEmailFromDB (long UsrCod,const char Email[Cns_MAX_BYTES_EMAIL_ADDRESS + 1])
  {
   /***** Remove an old email address *****/
   DB_QueryREPLACE ("can not remove an old email address",
		    "DELETE FROM usr_emails"
		    " WHERE UsrCod=%ld AND E_mail='%s'",
                    UsrCod,Email);
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
   bool ItsMe;

   /***** Get other user's code from form and get user's data *****/
   if (Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ())
     {
      if (Usr_ICanEditOtherUsr (&Gbl.Usrs.Other.UsrDat))
	{
	 /***** New user's ID *****/
	 ItsMe = Usr_ItsMe (Gbl.Usrs.Other.UsrDat.UsrCod);
	 Mai_NewUsrEmail (&Gbl.Usrs.Other.UsrDat,ItsMe);

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
   if (DB_QueryCOUNT ("can not check if email already existed",
		      "SELECT COUNT(*) FROM usr_emails"
		      " WHERE E_mail='%s' AND Confirmed='Y'"
		      " AND UsrCod<>%ld",
		      NewEmail,UsrDat->UsrCod))	// An email of another user is the same that my email
      return false;	// Don't update

   /***** Delete email (not confirmed) for other users *****/
   DB_QueryDELETE ("can not remove pending email for other users",
		   "DELETE FROM pending_emails"
		   " WHERE E_mail='%s' AND UsrCod<>%ld",
	           NewEmail,UsrDat->UsrCod);

   DB_QueryDELETE ("can not remove not confirmed email for other users",
		   "DELETE FROM usr_emails"
		   " WHERE E_mail='%s' AND Confirmed='N'"
		   " AND UsrCod<>%ld",
	           NewEmail,UsrDat->UsrCod);

   /***** Update email in database *****/
   DB_QueryREPLACE ("can not update email",
		    "REPLACE INTO usr_emails"
		    " (UsrCod,E_mail,CreatTime)"
		    " VALUES"
		    " (%ld,'%s',NOW())",
                    UsrDat->UsrCod,NewEmail);

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
   char Command[2048 +
		Cfg_MAX_BYTES_SMTP_PASSWORD +
		Cns_MAX_BYTES_EMAIL_ADDRESS +
		PATH_MAX]; // Command to execute for sending an email
   int ReturnCode;

   /***** Create temporary file for mail content *****/
   Mai_CreateFileNameMail ();

   /***** Write mail content into file and close file *****/
   /* Welcome note */
   Mai_WriteWelcomeNoteEMail (&Gbl.Usrs.Me.UsrDat);

   /* Store encrypted key in database */
   Mai_InsertMailKey (Gbl.Usrs.Me.UsrDat.Email,Gbl.UniqueNameEncrypted);

   /* Message body */
   fprintf (Gbl.Msg.FileMail,
	    Txt_If_you_just_request_from_X_the_confirmation_of_your_email_Y_NO_HTML,
	    Cfg_URL_SWAD_CGI,Gbl.Usrs.Me.UsrDat.Email,
            Cfg_URL_SWAD_CGI,Act_GetActCod (ActCnfMai),Gbl.UniqueNameEncrypted,
            Cfg_URL_SWAD_CGI);

   /* Footer note */
   Mai_WriteFootNoteEMail (Gbl.Prefs.Language);

   fclose (Gbl.Msg.FileMail);

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
             Gbl.Msg.FileNameMail);
   ReturnCode = system (Command);
   if (ReturnCode == -1)
      Lay_ShowErrorAndExit ("Error when running script to send email.");

   /***** Remove temporary file *****/
   unlink (Gbl.Msg.FileNameMail);

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
   DB_QueryDELETE ("can not remove old pending mail keys",
		   "DELETE LOW_PRIORITY FROM pending_emails"
		   " WHERE DateAndTime<FROM_UNIXTIME(UNIX_TIMESTAMP()-%lu)",
                   Cfg_TIME_TO_DELETE_OLD_PENDING_EMAILS);

   /***** Insert mail key in database *****/
   DB_QueryREPLACE ("can not create pending password",
		    "INSERT INTO pending_emails"
		    " (UsrCod,E_mail,MailKey,DateAndTime)"
		    " VALUES"
		    " (%ld,'%s','%s',NOW())",
	            Gbl.Usrs.Me.UsrDat.UsrCod,
	            Email,
	            MailKey);
  }

/*****************************************************************************/
/************************* Confirm my email address **************************/
/*****************************************************************************/

void Mai_ConfirmEmail (void)
  {
   extern const char *Txt_Email_X_has_already_been_confirmed_before;
   extern const char *Txt_The_email_X_has_been_confirmed;
   extern const char *Txt_The_email_address_has_not_been_confirmed;
   extern const char *Txt_Failed_email_confirmation_key;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   char MailKey[Mai_LENGTH_EMAIL_CONFIRM_KEY + 1];
   long UsrCod;
   char Email[Cns_MAX_BYTES_EMAIL_ADDRESS + 1];
   bool KeyIsCorrect;
   bool Confirmed;

   /***** Get parameter Key *****/
   Par_GetParToText ("key",MailKey,Mai_LENGTH_EMAIL_CONFIRM_KEY);

   /***** Get user's code and email from key *****/
   if (DB_QuerySELECT (&mysql_res,"can not get user's code and email from key",
		       "SELECT UsrCod,E_mail FROM pending_emails"
		       " WHERE MailKey='%s'",
		       MailKey))
     {
      row = mysql_fetch_row (mysql_res);

      /* Get user's code */
      UsrCod = Str_ConvertStrCodToLongCod (row[0]);

      /* Get user's email */
      Str_Copy (Email,row[1],
                Cns_MAX_BYTES_EMAIL_ADDRESS);

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
      /***** Delete this key *****/
      DB_QueryDELETE ("can not remove an email key",
		      "DELETE FROM pending_emails WHERE MailKey='%s'",
		      MailKey);

      /***** Check user's code and email
             and get if email is already confirmed *****/
      if (DB_QuerySELECT (&mysql_res,"can not check if email is confirmed",
			  "SELECT Confirmed FROM usr_emails"
			  " WHERE UsrCod=%ld AND E_mail='%s'",
			  UsrCod,Email))
	{
	 Confirmed = false;
	 if (row)
            if (row[0])
               Confirmed = (row[0][0] == 'Y');

         /***** Confirm email *****/
         if (Confirmed)
            Ale_ShowAlert (Ale_SUCCESS,Txt_Email_X_has_already_been_confirmed_before,
		           Email);
         else
           {
	    DB_QueryUPDATE ("can not confirm email",
			    "UPDATE usr_emails SET Confirmed='Y'"
			    " WHERE usr_emails.UsrCod=%ld"
			    " AND usr_emails.E_mail='%s'",
		            UsrCod,Email);

            Ale_ShowAlert (Ale_SUCCESS,Txt_The_email_X_has_been_confirmed,
		           Email);
           }
	}
      else
	 Ale_ShowAlert (Ale_WARNING,Txt_The_email_address_has_not_been_confirmed);

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);
     }
   else
      Ale_ShowAlert (Ale_WARNING,Txt_Failed_email_confirmation_key);

   /***** Form to log in *****/
   Usr_WriteFormLogin (ActLogIn,NULL);
  }

/*****************************************************************************/
/****************** Create temporary file for mail content *******************/
/*****************************************************************************/

void Mai_CreateFileNameMail (void)
  {
   snprintf (Gbl.Msg.FileNameMail,sizeof (Gbl.Msg.FileNameMail),
	     "%s/%s_mail.txt",
             Cfg_PATH_OUT_PRIVATE,Gbl.UniqueNameEncrypted);
   if ((Gbl.Msg.FileMail = fopen (Gbl.Msg.FileNameMail,"wb")) == NULL)
      Lay_ShowErrorAndExit ("Can not open file to send email.");
  }

/*****************************************************************************/
/************ Write a welcome note heading the automatic email ***************/
/*****************************************************************************/

void Mai_WriteWelcomeNoteEMail (struct UsrData *UsrDat)
  {
   extern const char *Txt_Dear_NO_HTML[Usr_NUM_SEXS][1 + Lan_NUM_LANGUAGES];
   extern const char *Txt_user_NO_HTML[Usr_NUM_SEXS][1 + Lan_NUM_LANGUAGES];

   fprintf (Gbl.Msg.FileMail,"%s %s:\n",
            Txt_Dear_NO_HTML[UsrDat->Sex][UsrDat->Prefs.Language],
            UsrDat->FirstName[0] ? UsrDat->FirstName :
                                   Txt_user_NO_HTML[UsrDat->Sex][UsrDat->Prefs.Language]);
  }

/*****************************************************************************/
/****************** Write a foot note in the automatic email *****************/
/*****************************************************************************/

void Mai_WriteFootNoteEMail (Lan_Language_t Language)
  {
   extern const char *Txt_Please_do_not_reply_to_this_automatically_generated_email_NO_HTML[1 + Lan_NUM_LANGUAGES];

   fprintf (Gbl.Msg.FileMail,"%s\n"
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
   bool ItsMe = Usr_ItsMe (UsrDat->UsrCod);

   if (ItsMe)
      return true;

   /***** Check if I have permission to see another user's email *****/
   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_STD:
	 /* If I am a student in the current course,
	    I can see the email of confirmed teachers */
	 return (UsrDat->Roles.InCurrentCrs.Role == Rol_NET ||	// A non-editing teacher
	         UsrDat->Roles.InCurrentCrs.Role == Rol_TCH) &&	// or a teacher
	         UsrDat->Accepted;				// who accepted registration
      case Rol_NET:
      case Rol_TCH:
	 /* If I am a teacher in the current course,
	    I can see the email of confirmed students and teachers */
         return Usr_CheckIfUsrBelongsToCurrentCrs (UsrDat) &&	// A user belonging to the current course
	        UsrDat->Accepted;			// who accepted registration
      case Rol_DEG_ADM:
	 /* If I am an administrator of current degree,
	    I only can see the user's email of users from current degree */
	 return Usr_CheckIfUsrBelongsToDeg (UsrDat->UsrCod,
	                                    Gbl.Hierarchy.Deg.DegCod);
      case Rol_CTR_ADM:
	 /* If I am an administrator of current centre,
	    I only can see the user's email of users from current centre */
	 return Usr_CheckIfUsrBelongsToCtr (UsrDat->UsrCod,
	                                    Gbl.Hierarchy.Ctr.CtrCod);
      case Rol_INS_ADM:
	 /* If I am an administrator of current institution,
	    I only can see the user's email of users from current institution */
	 return Usr_CheckIfUsrBelongsToIns (UsrDat->UsrCod,
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
      Lay_ShowErrorAndExit ("Error initializing mail domain.");

   /***** Allocate memory for mail domain *****/
   if ((Mai_EditingMai = (struct Mail *) malloc (sizeof (struct Mail))) == NULL)
      Lay_ShowErrorAndExit ("Error allocating memory for mail domain.");

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
      free ((void *) Mai_EditingMai);
      Mai_EditingMai = NULL;
     }
  }
