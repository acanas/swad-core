// swad_mail.c: all the stuff related to email

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2017 Antonio Cañas Vargas

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
#include "swad_database.h"
#include "swad_global.h"
#include "swad_mail.h"
#include "swad_parameter.h"
#include "swad_QR.h"
#include "swad_tab.h"
#include "swad_text.h"

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

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Mai_GetParamMaiOrder (void);
static void Mai_PutIconToEditMailDomains (void);
static void Mai_GetListMailDomainsAllowedForNotif (void);
static void Mai_GetMailBox (const char *Email,char MailBox[Usr_MAX_BYTES_USR_EMAIL + 1]);
static bool Mai_CheckIfMailDomainIsAllowedForNotif (const char *MailDomain);

static void Mai_ListMailDomainsForEdition (void);
static void Mai_PutParamMaiCod (long MaiCod);
static void Mai_RenameMailDomain (Cns_ShrtOrFullName_t ShrtOrFullName);
static bool Mai_CheckIfMailDomainNameExists (const char *FieldName,const char *Name,long MaiCod);
static void Mai_PutFormToCreateMailDomain (void);
static void Mai_PutHeadMailDomains (void);
static void Mai_CreateMailDomain (struct Mail *Mai);

static void Mai_RemoveEmail (struct UsrData *UsrDat);
static void Mai_RemoveEmailFromDB (long UsrCod,const char *Email);
static void Mai_NewUsrEmail (struct UsrData *UsrDat,bool ItsMe);
static void Mai_InsertMailKey (const char *Email,
                               const char MailKey[Mai_LENGTH_EMAIL_CONFIRM_KEY + 1]);

/*****************************************************************************/
/************************* List all the mail domains *************************/
/*****************************************************************************/

void Mai_SeeMailDomains (void)
  {
   extern const char *Hlp_SYSTEM_Domains;
   extern const char *Txt_Email_domains_allowed_for_notifications;
   extern const char *Txt_EMAIL_DOMAIN_HELP_ORDER[3];
   extern const char *Txt_EMAIL_DOMAIN_ORDER[3];
   Mai_DomainsOrder_t Order;
   unsigned NumMai;

   /***** Get parameter with the type of order in the list of mail domains *****/
   Mai_GetParamMaiOrder ();

   /***** Get list of mail domains *****/
   Mai_GetListMailDomainsAllowedForNotif ();

   /***** Table head *****/
   Lay_StartRoundFrameTable (NULL,Txt_Email_domains_allowed_for_notifications,
                             Gbl.Usrs.Me.LoggedRole == Rol_SYS_ADM ? Mai_PutIconToEditMailDomains :
                                                                     NULL,
                             Hlp_SYSTEM_Domains,2);
   fprintf (Gbl.F.Out,"<tr>");
   for (Order = Mai_ORDER_BY_DOMAIN;
	Order <= Mai_ORDER_BY_USERS;
	Order++)
     {
      fprintf (Gbl.F.Out,"<th class=\"LEFT_MIDDLE\">");
      Act_FormStart (ActSeeMai);
      Par_PutHiddenParamUnsigned ("Order",(unsigned) Order);
      Act_LinkFormSubmit (Txt_EMAIL_DOMAIN_HELP_ORDER[Order],"TIT_TBL",NULL);
      if (Order == Gbl.Mails.SelectedOrder)
         fprintf (Gbl.F.Out,"<u>");
      fprintf (Gbl.F.Out,"%s",Txt_EMAIL_DOMAIN_ORDER[Order]);
      if (Order == Gbl.Mails.SelectedOrder)
         fprintf (Gbl.F.Out,"</u>");
      fprintf (Gbl.F.Out,"</a>");
      Act_FormEnd ();
      fprintf (Gbl.F.Out,"</th>");
     }
   fprintf (Gbl.F.Out,"</tr>");

   /***** Write all the mail domains *****/
   for (NumMai = 0;
	NumMai < Gbl.Mails.Num;
	NumMai++)
      /* Write data of this mail domain */
      fprintf (Gbl.F.Out,"<tr>"
                         "<td class=\"DAT LEFT_TOP\">"
                         "%s"
                         "</td>"
                         "<td class=\"DAT LEFT_TOP\">"
                         "%s"
                         "</td>"
                         "<td class=\"DAT RIGHT_TOP\">"
                         "%u"
                         "</td>"
                         "</tr>",
               Gbl.Mails.Lst[NumMai].Domain,
               Gbl.Mails.Lst[NumMai].Info,
               Gbl.Mails.Lst[NumMai].NumUsrs);

   /***** End table *****/
   Lay_EndRoundFrameTable ();

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
   extern const char *Txt_Edit;

   Lay_PutContextualLink (ActEdiMai,NULL,
                          "edit64x64.png",
                          Txt_Edit,NULL,
                          NULL);
  }

/*****************************************************************************/
/*********************** Put forms to edit mail domains **********************/
/*****************************************************************************/

void Mai_EditMailDomains (void)
  {
   extern const char *Txt_There_are_no_email_domains;

   /***** Get list of mail domains *****/
   Mai_GetListMailDomainsAllowedForNotif ();

   if (!Gbl.Mails.Num)
      /***** Help message *****/
      Lay_ShowAlert (Lay_INFO,Txt_There_are_no_email_domains);

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
   char OrderBySubQuery[256];
   char Query[1024];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;
   unsigned NumMai;
   struct Mail *Mai;

   // Query uses temporary tables for speed
   // Query uses two identical temporary tables...
   // ...because a unique temporary table can not be used twice in the same query

   /***** Create temporary table with all the mail domains present in users' emails table *****/
   sprintf (Query,"DROP TEMPORARY TABLE IF EXISTS T1,T2");
   if (mysql_query (&Gbl.mysql,Query))
      DB_ExitOnMySQLError ("can not remove temporary tables");

   sprintf (Query,"CREATE TEMPORARY TABLE T1 ENGINE=MEMORY"
	          " SELECT SUBSTRING_INDEX(E_mail,'@',-1) AS Domain,COUNT(*) as N"
	          " FROM usr_emails GROUP BY Domain");
   if (mysql_query (&Gbl.mysql,Query))
      DB_ExitOnMySQLError ("can not create temporary table");

   sprintf (Query,"CREATE TEMPORARY TABLE T2 ENGINE=MEMORY SELECT * FROM T1");
   if (mysql_query (&Gbl.mysql,Query))
      DB_ExitOnMySQLError ("can not create temporary table");

   /***** Get mail domains from database *****/
   switch (Gbl.Mails.SelectedOrder)
     {
      case Mai_ORDER_BY_DOMAIN:
         sprintf (OrderBySubQuery,"Domain,Info,N DESC");
         break;
      case Mai_ORDER_BY_INFO:
         sprintf (OrderBySubQuery,"Info,Domain,N DESC");
         break;
      case Mai_ORDER_BY_USERS:
         sprintf (OrderBySubQuery,"N DESC,Info,Domain");
         break;
     }
   sprintf (Query,"(SELECT mail_domains.MaiCod,mail_domains.Domain AS Domain,mail_domains.Info AS Info,T1.N AS N"
                  " FROM mail_domains,T1"
                  " WHERE mail_domains.Domain=T1.Domain COLLATE 'latin1_bin')"
                  " UNION "
                  "(SELECT MaiCod,Domain,Info,0 AS N"
                  " FROM mail_domains"
                  " WHERE Domain NOT IN (SELECT Domain COLLATE 'latin1_bin' FROM T2))"
                  " ORDER BY %s",	// COLLATE necessary to avoid error in comparisons
            OrderBySubQuery);

   if ((NumRows = DB_QuerySELECT (Query,&mysql_res,"can not get mail domains"))) // Mail domains found...
     {
      Gbl.Mails.Num = (unsigned) NumRows;

      /***** Create list with places *****/
      if ((Gbl.Mails.Lst = (struct Mail *) calloc (NumRows,sizeof (struct Mail))) == NULL)
          Lay_ShowErrorAndExit ("Not enough memory to store mail domains.");

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
                   Mai_MAX_LENGTH_MAIL_DOMAIN);

         /* Get the mail domain info (row[2]) */
         Str_Copy (Mai->Info,row[2],
                   Mai_MAX_LENGTH_MAIL_INFO);

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
   sprintf (Query,"DROP TEMPORARY TABLE IF EXISTS T1,T2");
   if (mysql_query (&Gbl.mysql,Query))
      DB_ExitOnMySQLError ("can not remove temporary tables");
  }

/*****************************************************************************/
/************ Check if user can receive notifications via email **************/
/*****************************************************************************/

bool Mai_CheckIfUsrCanReceiveEmailNotif (const struct UsrData *UsrDat)
  {
   char MailDomain[Usr_MAX_BYTES_USR_EMAIL + 1];

   /***** Check #1: is my email address confirmed? *****/
   if (!UsrDat->EmailConfirmed)
      return false;

   /***** Check #2: if my mail domain allowed? *****/
   Mai_GetMailBox (UsrDat->Email,MailDomain);
   return Mai_CheckIfMailDomainIsAllowedForNotif (MailDomain);
  }

/*****************************************************************************/
/********************** Get mailbox from email address ***********************/
/*****************************************************************************/

static void Mai_GetMailBox (const char *Email,char MailBox[Usr_MAX_BYTES_USR_EMAIL + 1])
  {
   const char *Ptr;

   MailBox[0] = '\0';	// Return empty mailbox on error

   if ((Ptr = strchr (Email,(int) '@')))	// Find first '@' in address
      if (Ptr != Email)				// '@' is not the first character in Email
        {
         Ptr++;					// Skip '@'
         if (strchr (Ptr,(int) '@') == NULL)	// No more '@' found
            Str_Copy (MailBox,Ptr,
                      Usr_MAX_BYTES_USR_EMAIL);
        }
  }

/*****************************************************************************/
/************ Check if a mail domain is allowed for notifications ************/
/*****************************************************************************/

static bool Mai_CheckIfMailDomainIsAllowedForNotif (const char *MailDomain)
  {
   char Query[512];

   /***** Get number of mail_domains with a name from database *****/
   sprintf (Query,"SELECT COUNT(*) FROM mail_domains WHERE Domain='%s'",
            MailDomain);
   return (DB_QueryCOUNT (Query,"can not check if a mail domain is allowed for notifications") != 0);
  }

/*****************************************************************************/
/***************** Show warning about notifications via email ****************/
/*****************************************************************************/

void Mai_WriteWarningEmailNotifications (void)
  {
   extern struct Act_Actions Act_Actions[Act_NUM_ACTIONS];
   extern const char *Txt_You_can_only_receive_email_notifications_if_;
   extern const char *Txt_TABS_TXT[Tab_NUM_TABS];
   extern const char *Txt_MENU_TITLE[Tab_NUM_TABS][Act_MAX_OPTIONS_IN_MENU_PER_TAB];
   Act_Action_t SuperActionMyAccount   = Act_Actions[ActFrmMyAcc].SuperAction;
   Act_Action_t SuperActionMailDomains = Act_Actions[ActSeeMai  ].SuperAction;
   Tab_Tab_t TabMyAccount   = Act_Actions[SuperActionMyAccount  ].Tab;
   Tab_Tab_t TabMailDomains = Act_Actions[SuperActionMailDomains].Tab;

   sprintf (Gbl.Message,Txt_You_can_only_receive_email_notifications_if_,
	    Txt_TABS_TXT[TabMyAccount  ],
	    Txt_MENU_TITLE[TabMyAccount  ][Act_Actions[SuperActionMyAccount  ].IndexInMenu],
            Txt_TABS_TXT[TabMailDomains],
	    Txt_MENU_TITLE[TabMailDomains][Act_Actions[SuperActionMailDomains].IndexInMenu]);
   Lay_ShowAlert (Lay_WARNING,Gbl.Message);
  }

/*****************************************************************************/
/**************************** Get mail domain data ***************************/
/*****************************************************************************/

void Mai_GetDataOfMailDomainByCod (struct Mail *Mai)
  {
   char Query[1024];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;

   /***** Clear data *****/
   Mai->Domain[0] = Mai->Info[0] = '\0';

   /***** Check if mail code is correct *****/
   if (Mai->MaiCod > 0)
     {
      /***** Get data of a mail domain from database *****/
      sprintf (Query,"SELECT Domain,Info FROM mail_domains WHERE MaiCod='%ld'",
               Mai->MaiCod);
      NumRows = DB_QuerySELECT (Query,&mysql_res,"can not get data of a mail domain");

      if (NumRows) // Mail found...
        {
         /* Get row */
         row = mysql_fetch_row (mysql_res);

         /* Get the short name of the mail (row[0]) */
         Str_Copy (Mai->Domain,row[0],
                   Mai_MAX_LENGTH_MAIL_DOMAIN);

         /* Get the full name of the mail (row[1]) */
         Str_Copy (Mai->Info,row[1],
                   Mai_MAX_LENGTH_MAIL_INFO);
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
   extern const char *Hlp_SYSTEM_Domains_edit;
   extern const char *Txt_Email_domains_allowed_for_notifications;
   unsigned NumMai;
   struct Mail *Mai;

   Lay_StartRoundFrameTable (NULL,Txt_Email_domains_allowed_for_notifications,
                             NULL,Hlp_SYSTEM_Domains_edit,2);

   /***** Table head *****/
   Mai_PutHeadMailDomains ();

   /***** Write all the mail domains *****/
   for (NumMai = 0;
	NumMai < Gbl.Mails.Num;
	NumMai++)
     {
      Mai = &Gbl.Mails.Lst[NumMai];

      /* Put icon to remove mail */
      fprintf (Gbl.F.Out,"<tr>"
	                 "<td class=\"BM\">");
      Act_FormStart (ActRemMai);
      Mai_PutParamMaiCod (Mai->MaiCod);
      Lay_PutIconRemove ();
      Act_FormEnd ();
      fprintf (Gbl.F.Out,"</td>");

      /* Mail code */
      fprintf (Gbl.F.Out,"<td class=\"DAT RIGHT_MIDDLE\">"
	                 "%ld"
	                 "</td>",
               Mai->MaiCod);

      /* Mail domain */
      fprintf (Gbl.F.Out,"<td class=\"CENTER_MIDDLE\">");
      Act_FormStart (ActRenMaiSho);
      Mai_PutParamMaiCod (Mai->MaiCod);
      fprintf (Gbl.F.Out,"<input type=\"text\" name=\"Domain\""
	                 " size=\"15\" maxlength=\"%u\" value=\"%s\""
                         " onchange=\"document.getElementById('%s').submit();\" />",
               Mai_MAX_LENGTH_MAIL_DOMAIN,Mai->Domain,
               Gbl.Form.Id);
      Act_FormEnd ();
      fprintf (Gbl.F.Out,"</td>");

      /* Mail domain info */
      fprintf (Gbl.F.Out,"<td class=\"CENTER_MIDDLE\">");
      Act_FormStart (ActRenMaiFul);
      Mai_PutParamMaiCod (Mai->MaiCod);
      fprintf (Gbl.F.Out,"<input type=\"text\" name=\"Info\""
	                 " size=\"40\" maxlength=\"%u\" value=\"%s\""
                         " onchange=\"document.getElementById('%s').submit();\" />",
               Mai_MAX_LENGTH_MAIL_INFO,Mai->Info,
               Gbl.Form.Id);
      Act_FormEnd ();
      fprintf (Gbl.F.Out,"</td>");

      /* Number of users */
      fprintf (Gbl.F.Out,"<td class=\"DAT RIGHT_MIDDLE\">"
	                 "%u"
	                 "</td>"
	                 "</tr>",
               Mai->NumUsrs);
     }

   Lay_EndRoundFrameTable ();
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
   char Query[512];
   struct Mail Mai;

   /***** Get mail code *****/
   if ((Mai.MaiCod = Mai_GetParamMaiCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of mail domain is missing.");

   /***** Get data of the mail from database *****/
   Mai_GetDataOfMailDomainByCod (&Mai);

   /***** Remove mail *****/
   sprintf (Query,"DELETE FROM mail_domains WHERE MaiCod='%ld'",Mai.MaiCod);
   DB_QueryDELETE (Query,"can not remove a mail domain");

   /***** Write message to show the change made *****/
   sprintf (Gbl.Message,Txt_Email_domain_X_removed,
            Mai.Domain);
   Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);

   /***** Show the form again *****/
   Mai_EditMailDomains ();
  }

/*****************************************************************************/
/********************* Change the short name of a mail ***********************/
/*****************************************************************************/

void Mai_RenameMailDomainShort (void)
  {
   Mai_RenameMailDomain (Cns_SHRT_NAME);
  }

/*****************************************************************************/
/********************* Change the full name of a mail ************************/
/*****************************************************************************/

void Mai_RenameMailDomainFull (void)
  {
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
   char Query[512];
   struct Mail *Mai;
   const char *ParamName = NULL;	// Initialized to avoid warning
   const char *FieldName = NULL;	// Initialized to avoid warning
   unsigned MaxLength = 0;		// Initialized to avoid warning
   char *CurrentMaiName = NULL;		// Initialized to avoid warning
   char NewMaiName[Mai_MAX_LENGTH_MAIL_INFO + 1];

   Mai = &Gbl.Mails.EditingMai;
   switch (ShrtOrFullName)
     {
      case Cns_SHRT_NAME:
         ParamName = "Domain";
         FieldName = "Domain";
         MaxLength = Mai_MAX_LENGTH_MAIL_DOMAIN;
         CurrentMaiName = Mai->Domain;
         break;
      case Cns_FULL_NAME:
         ParamName = "Info";
         FieldName = "Info";
         MaxLength = Mai_MAX_LENGTH_MAIL_INFO;
         CurrentMaiName = Mai->Info;
         break;
     }

   /***** Get parameters from form *****/
   /* Get the code of the mail */
   if ((Mai->MaiCod = Mai_GetParamMaiCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of mail domain is missing.");

   /* Get the new name for the mail */
   Par_GetParToText (ParamName,NewMaiName,MaxLength);

   /***** Get from the database the old names of the mail *****/
   Mai_GetDataOfMailDomainByCod (Mai);

   /***** Check if new name is empty *****/
   if (!NewMaiName[0])
     {
      sprintf (Gbl.Message,Txt_You_can_not_leave_the_name_of_the_email_domain_X_empty,
               CurrentMaiName);
      Lay_ShowAlert (Lay_WARNING,Gbl.Message);
     }
   else
     {
      /***** Check if old and new names are the same (this happens when user press enter with no changes in the form) *****/
      if (strcmp (CurrentMaiName,NewMaiName))	// Different names
        {
         /***** If mail was in database... *****/
         if (Mai_CheckIfMailDomainNameExists (ParamName,NewMaiName,Mai->MaiCod))
           {
            sprintf (Gbl.Message,Txt_The_email_domain_X_already_exists,
                     NewMaiName);
            Lay_ShowAlert (Lay_WARNING,Gbl.Message);
           }
         else
           {
            /* Update the table changing old name by new name */
            sprintf (Query,"UPDATE mail_domains SET %s='%s' WHERE MaiCod='%ld'",
                     FieldName,NewMaiName,Mai->MaiCod);
            DB_QueryUPDATE (Query,"can not update the name of a mail domain");

            /***** Write message to show the change made *****/
            sprintf (Gbl.Message,Txt_The_email_domain_X_has_been_renamed_as_Y,
                     CurrentMaiName,NewMaiName);
            Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);
           }
        }
      else	// The same name
        {
         sprintf (Gbl.Message,Txt_The_email_domain_X_has_not_changed,
                  CurrentMaiName);
         Lay_ShowAlert (Lay_INFO,Gbl.Message);
        }
     }

   /***** Show the form again *****/
   Str_Copy (CurrentMaiName,NewMaiName,
             MaxLength);
   Mai_EditMailDomains ();
  }

/*****************************************************************************/
/********************** Check if the name of mail exists *********************/
/*****************************************************************************/

static bool Mai_CheckIfMailDomainNameExists (const char *FieldName,const char *Name,long MaiCod)
  {
   char Query[512];

   /***** Get number of mail_domains with a name from database *****/
   sprintf (Query,"SELECT COUNT(*) FROM mail_domains WHERE %s='%s' AND MaiCod<>'%ld'",
            FieldName,Name,MaiCod);
   return (DB_QueryCOUNT (Query,"can not check if the name of a mail domain already existed") != 0);
  }

/*****************************************************************************/
/*********************** Put a form to create a new mail *********************/
/*****************************************************************************/

static void Mai_PutFormToCreateMailDomain (void)
  {
   extern const char *Hlp_SYSTEM_Domains_edit;
   extern const char *Txt_New_email_domain;
   extern const char *Txt_EMAIL_DOMAIN_ORDER[3];
   extern const char *Txt_Create_email_domain;
   struct Mail *Mai;

   Mai = &Gbl.Mails.EditingMai;

   /***** Start form *****/
   Act_FormStart (ActNewMai);

   /***** Start of frame *****/
   Lay_StartRoundFrameTable (NULL,Txt_New_email_domain,
                             NULL,Hlp_SYSTEM_Domains_edit,2);

   /***** Write heading *****/
   fprintf (Gbl.F.Out,"<tr>"
                      "<th class=\"LEFT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "<th class=\"LEFT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "</tr>",
            Txt_EMAIL_DOMAIN_ORDER[Mai_ORDER_BY_DOMAIN],
            Txt_EMAIL_DOMAIN_ORDER[Mai_ORDER_BY_INFO  ]);

   /***** Mail domain *****/
   fprintf (Gbl.F.Out,"<tr>"
                      "<td class=\"CENTER_MIDDLE\">"
                      "<input type=\"text\" name=\"Domain\""
                      " size=\"15\" maxlength=\"%u\" value=\"%s\""
                      " required=\"required\" />"
                      "</td>",
            Mai_MAX_LENGTH_MAIL_DOMAIN,Mai->Domain);

   /***** Mail domain info *****/
   fprintf (Gbl.F.Out,"<td class=\"CENTER_MIDDLE\">"
                      "<input type=\"text\" name=\"Info\""
                      " size=\"40\" maxlength=\"%u\" value=\"%s\""
                      " required=\"required\" />"
                      "</td>"
                      "<td></td>"
                      "</tr>",
            Mai_MAX_LENGTH_MAIL_INFO,Mai->Info);


   /***** Send button and end frame *****/
   Lay_EndRoundFrameTableWithButton (Lay_CREATE_BUTTON,Txt_Create_email_domain);

   /***** End of form *****/
   Act_FormEnd ();
  }

/*****************************************************************************/
/********************* Write header with fields of a mail ********************/
/*****************************************************************************/

static void Mai_PutHeadMailDomains (void)
  {
   extern const char *Txt_Code;
   extern const char *Txt_EMAIL_DOMAIN_ORDER[3];

   fprintf (Gbl.F.Out,"<tr>"
                      "<th class=\"BM\"></th>"
                      "<th class=\"RIGHT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "<th class=\"LEFT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "<th class=\"LEFT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "<th class=\"RIGHT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "</tr>",
            Txt_Code,
            Txt_EMAIL_DOMAIN_ORDER[Mai_ORDER_BY_DOMAIN],
            Txt_EMAIL_DOMAIN_ORDER[Mai_ORDER_BY_INFO  ],
            Txt_EMAIL_DOMAIN_ORDER[Mai_ORDER_BY_USERS ]);
  }

/*****************************************************************************/
/******************* Receive form to create a new mail ***********************/
/*****************************************************************************/

void Mai_RecFormNewMailDomain (void)
  {
   extern const char *Txt_The_email_domain_X_already_exists;
   extern const char *Txt_You_must_specify_the_short_name_and_the_full_name_of_the_new_email_domain;
   struct Mail *Mai;

   Mai = &Gbl.Mails.EditingMai;

   /***** Get parameters from form *****/
   /* Get mail short name */
   Par_GetParToText ("Domain",Mai->Domain,Mai_MAX_LENGTH_MAIL_DOMAIN);

   /* Get mail full name */
   Par_GetParToText ("Info",Mai->Info,Mai_MAX_LENGTH_MAIL_INFO);

   if (Mai->Domain[0] && Mai->Info[0])	// If there's a mail name
     {
      /***** If name of mail was in database... *****/
      if (Mai_CheckIfMailDomainNameExists ("Domain",Mai->Domain,-1L))
        {
         sprintf (Gbl.Message,Txt_The_email_domain_X_already_exists,
                  Mai->Domain);
         Lay_ShowAlert (Lay_WARNING,Gbl.Message);
        }
      else if (Mai_CheckIfMailDomainNameExists ("Info",Mai->Info,-1L))
        {
         sprintf (Gbl.Message,Txt_The_email_domain_X_already_exists,
                  Mai->Info);
         Lay_ShowAlert (Lay_WARNING,Gbl.Message);
        }
      else	// Add new mail to database
         Mai_CreateMailDomain (Mai);
     }
   else	// If there is not a mail name
      Lay_ShowAlert (Lay_WARNING,Txt_You_must_specify_the_short_name_and_the_full_name_of_the_new_email_domain);

   /***** Show the form again *****/
   Mai_EditMailDomains ();
  }

/*****************************************************************************/
/************************** Create a new mail domain *************************/
/*****************************************************************************/

static void Mai_CreateMailDomain (struct Mail *Mai)
  {
   extern const char *Txt_Created_new_email_domain_X;
   char Query[1024];

   /***** Create a new mail *****/
   sprintf (Query,"INSERT INTO mail_domains (Domain,Info) VALUES ('%s','%s')",
            Mai->Domain,Mai->Info);
   DB_QueryINSERT (Query,"can not create mail domain");

   /***** Write success message *****/
   sprintf (Gbl.Message,Txt_Created_new_email_domain_X,
            Mai->Domain);
   Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);
  }

/*****************************************************************************/
/****** List the emails of all the students to creates an email message ******/
/*****************************************************************************/

#define Mai_MAX_LENGTH_STR_ADDR (32 * 5000)

void Mai_ListEmails (void)
  {
   extern const char *Hlp_MESSAGES_Email;
   extern const char *The_ClassFormBold[The_NUM_THEMES];
   extern const char *Txt_Students_who_have_accepted_and_who_have_email;
   extern const char *Txt_X_students_who_have_email;
   extern const char *Txt_X_students_who_have_accepted_and_who_have_email;
   extern const char *Txt_Create_email_message;
   unsigned NumUsr;
   unsigned NumStdsWithEmail;
   unsigned NumAcceptedStdsWithEmail;
   char StrAddresses[Mai_MAX_LENGTH_STR_ADDR + 1];
   unsigned int LengthStrAddr = 0;
   struct UsrData UsrDat;

   /***** Get groups to show ******/
   Grp_GetParCodsSeveralGrpsToShowUsrs ();

   /***** Get and order list of students in this course *****/
   Usr_GetListUsrs (Rol_STUDENT,Sco_SCOPE_CRS);

   /***** Start of the frame used to list the emails *****/
   Lay_StartRoundFrame (NULL,
			Txt_Students_who_have_accepted_and_who_have_email,
			NULL,Hlp_MESSAGES_Email);

   /***** Form to select groups *****/
   Grp_ShowFormToSelectSeveralGroups (ActMaiStd);

   if (Gbl.Usrs.LstUsrs[Rol_STUDENT].NumUsrs)
     {
      if (Usr_GetIfShowBigList (Gbl.Usrs.LstUsrs[Rol_STUDENT].NumUsrs,NULL))
        {
         /***** Initialize structure with user's data *****/
         Usr_UsrDataConstructor (&UsrDat);

         /***** List the students' email addresses *****/
         fprintf (Gbl.F.Out,"<div class=\"DAT_SMALL LEFT_MIDDLE\">");
         for (NumUsr = 0, NumStdsWithEmail = 0, NumAcceptedStdsWithEmail = 0,
              StrAddresses[0] = '\0';
              NumUsr < Gbl.Usrs.LstUsrs[Rol_STUDENT].NumUsrs;
              NumUsr++)
           {
	    /* Copy user's basic data from list */
	    Usr_CopyBasicUsrDataFromList (&UsrDat,&Gbl.Usrs.LstUsrs[Rol_STUDENT].Lst[NumUsr]);

	    /* Get user's email */
            Mai_GetEmailFromUsrCod (&UsrDat);

	    if (UsrDat.Email[0])
	      {
	       NumStdsWithEmail++;
	       if (UsrDat.Accepted) // If student has email and has accepted
		 {
		  if (NumAcceptedStdsWithEmail > 0)
		    {
		     fprintf (Gbl.F.Out,", ");
		     LengthStrAddr ++;
		     if (LengthStrAddr > Mai_MAX_LENGTH_STR_ADDR)
			Lay_ShowErrorAndExit ("The space allocated to store email addresses is full.");
		     Str_Concat (StrAddresses,",",
		                 Mai_MAX_LENGTH_STR_ADDR);
		    }
		  LengthStrAddr += strlen (UsrDat.Email);
		  if (LengthStrAddr > Mai_MAX_LENGTH_STR_ADDR)
		     Lay_ShowErrorAndExit ("The space allocated to store email addresses is full.");
		  Str_Concat (StrAddresses,UsrDat.Email,
		              Mai_MAX_LENGTH_STR_ADDR);
		  fprintf (Gbl.F.Out,"<a href=\"mailto:%s?subject=%s\">%s</a>",
			   UsrDat.Email,Gbl.CurrentCrs.Crs.FullName,UsrDat.Email);

		  NumAcceptedStdsWithEmail++;
		 }
	      }
           }
         fprintf (Gbl.F.Out,"</div>");

         /***** Free memory used for user's data *****/
         Usr_UsrDataDestructor (&UsrDat);

         /***** Show a message with the number of students with email ****/
         fprintf (Gbl.F.Out,"<div class=\"DAT CENTER_MIDDLE\">");
         fprintf (Gbl.F.Out,Txt_X_students_who_have_email,
                  NumStdsWithEmail,
                  ((float) NumStdsWithEmail /
                   (float) Gbl.Usrs.LstUsrs[Rol_STUDENT].NumUsrs) * 100.0,
                  Gbl.Usrs.LstUsrs[Rol_STUDENT].NumUsrs);
         fprintf (Gbl.F.Out,"</div>");

         /***** Show a message with the number of students who have accepted and have email ****/
         fprintf (Gbl.F.Out,"<div class=\"DAT CENTER_MIDDLE\">");
         fprintf (Gbl.F.Out,Txt_X_students_who_have_accepted_and_who_have_email,
                  NumAcceptedStdsWithEmail,
                  ((float) NumAcceptedStdsWithEmail /
                   (float) Gbl.Usrs.LstUsrs[Rol_STUDENT].NumUsrs) * 100.0,
                  Gbl.Usrs.LstUsrs[Rol_STUDENT].NumUsrs);
         fprintf (Gbl.F.Out,"</div>");

         /***** Icon to open the client email program *****/
         fprintf (Gbl.F.Out,"<div class=\"CONTEXT_MENU\">"
                            "<a href=\"mailto:%s?subject=%s&cc=%s&bcc=%s\""
                            " title=\"%s\" class=\"%s\">",
                  Gbl.Usrs.Me.UsrDat.Email,
	          Gbl.CurrentCrs.Crs.FullName,
	          Gbl.Usrs.Me.UsrDat.Email,
	          StrAddresses,
                  Txt_Create_email_message,
                  The_ClassFormBold[Gbl.Prefs.Theme]);
         Lay_PutIconWithText ("editnewmsg16x16.gif",
                              Txt_Create_email_message,
                              Txt_Create_email_message);
         fprintf (Gbl.F.Out,"</a>"
                            "</div>");
        }
     }
   else
      Usr_ShowWarningNoUsersFound (Rol_STUDENT);

   /***** End of the frame used to list the emails *****/
   Lay_EndRoundFrame ();

   /***** Free memory for students list *****/
   Usr_FreeUsrsList (Rol_STUDENT);

   /***** Free memory for list of selected groups *****/
   Grp_FreeListCodSelectedGrps ();
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
          5 <= Length <= Usr_MAX_BYTES_USR_EMAIL *****/
   if (Length < 5 ||
       Length > Usr_MAX_BYTES_USR_EMAIL)
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
   char Query[256];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;
   bool Found;

   /***** Get current (last updated) user's nickname from database *****/
   sprintf (Query,"SELECT E_mail,Confirmed FROM usr_emails"
	          " WHERE UsrCod='%ld' ORDER BY CreatTime DESC LIMIT 1",
	    UsrDat->UsrCod);
   NumRows = DB_QuerySELECT (Query,&mysql_res,"can not get email address");

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
                Usr_MAX_BYTES_USR_EMAIL);
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

long Mai_GetUsrCodFromEmail (const char *Email)
  {
   char Query[1024];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumUsrs;
   long UsrCod = -1L;

   if (Email)
      if (Email[0])
	{
	 /***** Get user's code from database *****/
	 /* Check if user code from table usr_emails is also in table usr_data */
	 sprintf (Query,"SELECT usr_emails.UsrCod FROM usr_emails,usr_data"
			" WHERE usr_emails.E_mail='%s'"
			" AND usr_emails.UsrCod=usr_data.UsrCod",
		  Email);
	 NumUsrs = (unsigned) DB_QuerySELECT (Query,&mysql_res,"can not get user's code");
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
/********** Put a link to the action used to change user's emails ************/
/*****************************************************************************/

void Mai_PutLinkToChangeOtherUsrEmails (void)
  {
   extern const char *Txt_Change_email;

   /***** Link for changing the password *****/
   if (Gbl.Usrs.Other.UsrDat.UsrCod == Gbl.Usrs.Me.UsrDat.UsrCod)	// It's me
      Lay_PutContextualLink (ActFrmMyAcc,NULL,
                             "msg64x64.gif",
			     Txt_Change_email,Txt_Change_email,
                             NULL);
   else									// Not me
      Lay_PutContextualLink ( Gbl.Usrs.Other.UsrDat.RoleInCurrentCrsDB == Rol_STUDENT ? ActFrmMaiStd :
	                     (Gbl.Usrs.Other.UsrDat.RoleInCurrentCrsDB == Rol_TEACHER ? ActFrmMaiTch :
	                	                                                        ActFrmMaiOth),
                             Usr_PutParamOtherUsrCodEncrypted,
                             "msg64x64.gif",
                             Txt_Change_email,Txt_Change_email,
                             NULL);
  }

/*****************************************************************************/
/*********** Show form to the change the email of another user ***************/
/*****************************************************************************/

void Mai_ShowFormOthEmail (void)
  {
   extern const char *Txt_Email;
   extern const char *Txt_User_not_found_or_you_do_not_have_permission_;

   /***** Get user whose password must be changed *****/
   if (Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ())
     {
      if (Usr_ICanEditOtherUsr (&Gbl.Usrs.Other.UsrDat))
	{
	 /***** Start frame *****/
         Lay_StartRoundFrame (NULL,Txt_Email,NULL,NULL);

	 /***** Show user's record *****/
	 Rec_ShowSharedUsrRecord (Rec_SHA_RECORD_LIST,&Gbl.Usrs.Other.UsrDat);

	 /***** Form with the user's email *****/
	 fprintf (Gbl.F.Out,"<table class=\"CELLS_PAD_2\" style=\"margin:0 auto;\">");
	 Mai_ShowFormChangeUsrEmail (&Gbl.Usrs.Other.UsrDat,
                                     (Gbl.Usrs.Other.UsrDat.UsrCod == Gbl.Usrs.Me.UsrDat.UsrCod));	// It's me?
	 fprintf (Gbl.F.Out,"</table>");

         /***** End frame *****/
         Lay_EndRoundFrame ();
	}
      else
	 Lay_ShowAlert (Lay_WARNING,Txt_User_not_found_or_you_do_not_have_permission_);
     }
   else		// User not found
      Lay_ShowAlert (Lay_WARNING,Txt_User_not_found_or_you_do_not_have_permission_);
  }

/*****************************************************************************/
/*********************** Show form to change my email ************************/
/*****************************************************************************/

void Mai_ShowFormChangeUsrEmail (const struct UsrData *UsrDat,bool ItsMe)
  {
   extern const char *The_ClassForm[The_NUM_THEMES];
   extern const char *Txt_Current_email;
   extern const char *Txt_Other_emails;
   extern const char *Txt_Email_X_confirmed;
   extern const char *Txt_Confirm_email;
   extern const char *Txt_Use_this_email;
   extern const char *Txt_New_email;
   extern const char *Txt_Email;
   extern const char *Txt_Change_email;
   extern const char *Txt_Save;
   char Query[256];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumEmails;
   unsigned NumEmail;
   bool Confirmed;

   /***** Get my emails *****/
   sprintf (Query,"SELECT E_mail,Confirmed FROM usr_emails"
                  " WHERE UsrCod='%ld'"
                  " ORDER BY CreatTime DESC",
            UsrDat->UsrCod);
   NumEmails = (unsigned) DB_QuerySELECT (Query,&mysql_res,"can not get old email addresses of a user");

   /***** List my emails *****/
   for (NumEmail = 1;
	NumEmail <= NumEmails;
	NumEmail++)
     {
      /* Get email */
      row = mysql_fetch_row (mysql_res);
      Confirmed = (row[1][0] == 'Y');

      if (NumEmail == 1)
	 /* The first mail is the current one */
	 fprintf (Gbl.F.Out,"<tr>"
			    "<td class=\"%s RIGHT_MIDDLE\">"
			    "%s:"
			    "</td>"
			    "<td class=\"LEFT_MIDDLE\">"
			    "<div class=\"FORM_ACCOUNT\">",
		  The_ClassForm[Gbl.Prefs.Theme],Txt_Current_email);
      else	// NumEmail >= 2
	{
	 fprintf (Gbl.F.Out,"<tr>");
	 if (NumEmail == 2)
	    fprintf (Gbl.F.Out,"<td rowspan=\"%u\" class=\"%s RIGHT_TOP\">"
			       "%s:",
		     NumEmails - 1,
		     The_ClassForm[Gbl.Prefs.Theme],
		     Txt_Other_emails);
	 fprintf (Gbl.F.Out,"</td>"
			    "<td class=\"LEFT_TOP\">"
			    "<div class=\"FORM_ACCOUNT\">");
	}

      /* Form to remove email */
      if (ItsMe)
	 Act_FormStart (ActRemMaiMe);
      else
	{
	 Act_FormStart ( UsrDat->RoleInCurrentCrsDB == Rol_STUDENT ? ActRemMaiStd :
			(UsrDat->RoleInCurrentCrsDB == Rol_TEACHER ? ActRemMaiTch :
								     ActRemMaiOth));	// Guest, visitor or admin
	 Usr_PutParamUsrCodEncrypted (UsrDat->EncryptedUsrCod);
	}
      fprintf (Gbl.F.Out,"<input type=\"hidden\" name=\"Email\" value=\"%s\" />",
	       row[0]);
      Lay_PutIconRemove ();
      Act_FormEnd ();

      /* Email */
      fprintf (Gbl.F.Out,"<span class=\"%s\">%s</span>",
	       NumEmail == 1 ? "USR_ID" :
		               "DAT",
               row[0]);

      /* Email confirmed? */
      if (Confirmed)
	{
	 sprintf (Gbl.Title,Txt_Email_X_confirmed,row[0]);
	 fprintf (Gbl.F.Out,"<img src=\"%s/ok_green16x16.gif\""
			    " alt=\"%s\" title=\"%s\""
			    " class=\"ICO20x20\" />",
		  Gbl.Prefs.IconsURL,
		  Gbl.Title,Gbl.Title);
	}

      fprintf (Gbl.F.Out,"</div>");

      /* Form to change user's email */
      if (NumEmail > 1 || (ItsMe && !Confirmed))
	{
	 if (ItsMe)
	    Act_FormStart (ActNewMaiMe);
	 else
	   {
	    Act_FormStart ( UsrDat->RoleInCurrentCrsDB == Rol_STUDENT ? ActNewMaiStd :
			   (UsrDat->RoleInCurrentCrsDB == Rol_TEACHER ? ActNewMaiTch :
									ActNewMaiOth));	// Guest, visitor or admin
	    Usr_PutParamUsrCodEncrypted (UsrDat->EncryptedUsrCod);
	   }
	 fprintf (Gbl.F.Out,"<input type=\"hidden\" name=\"NewEmail\" value=\"%s\" />",
		  row[0]);	// Email
         Lay_PutConfirmButtonInline ((ItsMe && NumEmail == 1) ? Txt_Confirm_email :
			                                        Txt_Use_this_email);
	 Act_FormEnd ();
	 fprintf (Gbl.F.Out,"</td>");
	}

      fprintf (Gbl.F.Out,"</td>"
			 "</tr>");
     }

   /***** Form to enter new email *****/
   fprintf (Gbl.F.Out,"<tr>"
                      "<td class=\"RIGHT_MIDDLE\">"
                      "<label for=\"NewEmail\" class=\"%s\">%s:</label>"
                      "</td>"
                      "<td class=\"LEFT_MIDDLE\">",
            The_ClassForm[Gbl.Prefs.Theme],
            NumEmails ? Txt_New_email :	// A new email
        	        Txt_Email);	// The first email
   if (ItsMe)
      Act_FormStart (ActNewMaiMe);
   else
     {
      Act_FormStart ( UsrDat->RoleInCurrentCrsDB == Rol_STUDENT ? ActNewMaiStd :
		     (UsrDat->RoleInCurrentCrsDB == Rol_TEACHER ? ActNewMaiTch :
								  ActNewMaiOth));	// Guest, visitor or admin
      Usr_PutParamUsrCodEncrypted (UsrDat->EncryptedUsrCod);
     }
   fprintf (Gbl.F.Out,"<div class=\"FORM_ACCOUNT\">"
	              "<input type=\"email\" id=\"NewEmail\" name=\"NewEmail\""
	              " size=\"18\" maxlength=\"%u\" value=\"%s\" />"
	              "</div>",
            Usr_MAX_BYTES_USR_EMAIL,
            Gbl.Usrs.Me.UsrDat.Email);
   Lay_PutCreateButtonInline (NumEmails ? Txt_Change_email :	// User already has an email address
        	                          Txt_Save);		// User has no email address yet
   Act_FormEnd ();
   fprintf (Gbl.F.Out,"</td>"
	              "</tr>");
  }

/*****************************************************************************/
/******************** Remove one of my user's emails *************************/
/*****************************************************************************/

void Mai_RemoveMyUsrEmail (void)
  {
   /***** Remove user's email *****/
   Mai_RemoveEmail (&Gbl.Usrs.Me.UsrDat);

   /***** Show my account again *****/
   Acc_ShowFormChangeMyAccount ();
  }

/*****************************************************************************/
/**************** Remove one of the user's IDs of another user ***************/
/*****************************************************************************/

void Mai_RemoveOtherUsrEmail (void)
  {
   extern const char *Txt_User_not_found_or_you_do_not_have_permission_;

   /***** Get other user's code from form and get user's data *****/
   if (Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ())
     {
      /***** Remove user's email *****/
      Mai_RemoveEmail (&Gbl.Usrs.Other.UsrDat);

      /***** Show user's record *****/
      Rec_ShowSharedUsrRecord (Rec_SHA_RECORD_LIST,&Gbl.Usrs.Other.UsrDat);
     }
   else		// User not found
      Lay_ShowAlert (Lay_WARNING,Txt_User_not_found_or_you_do_not_have_permission_);
  }

/*****************************************************************************/
/************************** Remove email address *****************************/
/*****************************************************************************/

static void Mai_RemoveEmail (struct UsrData *UsrDat)
  {
   extern const char *Txt_Email_X_removed;
   extern const char *Txt_User_not_found_or_you_do_not_have_permission_;
   char Email[Usr_MAX_BYTES_USR_EMAIL + 1];

   if (Usr_ICanEditOtherUsr (UsrDat))
     {
      /***** Get new email from form *****/
      Par_GetParToText ("Email",Email,Usr_MAX_BYTES_USR_EMAIL);

      /***** Remove one of user's old email addresses *****/
      Mai_RemoveEmailFromDB (UsrDat->UsrCod,Email);

      /***** Show message *****/
      sprintf (Gbl.Message,Txt_Email_X_removed,Email);
      Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);

      /***** Update list of emails *****/
      Mai_GetEmailFromUsrCod (UsrDat);
     }
   else
      Lay_ShowAlert (Lay_WARNING,Txt_User_not_found_or_you_do_not_have_permission_);
  }

/*****************************************************************************/
/*************** Remove an old email address from database *******************/
/*****************************************************************************/

static void Mai_RemoveEmailFromDB (long UsrCod,const char *Email)
  {
   char Query[1024];

   /***** Remove an old email address *****/
   sprintf (Query,"DELETE FROM usr_emails"
                  " WHERE UsrCod='%ld' AND E_mail='%s'",
            UsrCod,Email);
   DB_QueryREPLACE (Query,"can not remove an old email address");
  }

/*****************************************************************************/
/************************* New user's email for me ***************************/
/*****************************************************************************/

void May_NewMyUsrEmail (void)
  {
   /***** Remove user's email *****/
   Mai_NewUsrEmail (&Gbl.Usrs.Me.UsrDat,true);	// It's me

   /***** Show my account again *****/
   Acc_ShowFormChangeMyAccount ();
  }

/*****************************************************************************/
/********************* New user's email for another user *********************/
/*****************************************************************************/

void Mai_NewOtherUsrEmail (void)
  {
   extern const char *Txt_User_not_found_or_you_do_not_have_permission_;

   /***** Get other user's code from form and get user's data *****/
   if (Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ())
     {
      /***** New user's ID *****/
      Mai_NewUsrEmail (&Gbl.Usrs.Other.UsrDat,
		       (Gbl.Usrs.Other.UsrDat.UsrCod == Gbl.Usrs.Me.UsrDat.UsrCod));	// It's me?

      /***** Show user's record *****/
      Rec_ShowSharedUsrRecord (Rec_SHA_RECORD_LIST,&Gbl.Usrs.Other.UsrDat);
     }
   else		// User not found
      Lay_ShowAlert (Lay_WARNING,Txt_User_not_found_or_you_do_not_have_permission_);
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
   extern const char *Txt_User_not_found_or_you_do_not_have_permission_;
   char NewEmail[Usr_MAX_BYTES_USR_EMAIL + 1];

   if (Usr_ICanEditOtherUsr (UsrDat))
     {
      /***** Get new email from form *****/
      Par_GetParToText ("NewEmail",NewEmail,Usr_MAX_BYTES_USR_EMAIL);

      if (Mai_CheckIfEmailIsValid (NewEmail))	// New email is valid
	{
	 /***** Check if new email exists in database *****/
	 if (UsrDat->EmailConfirmed &&
	     !strcmp (UsrDat->Email,NewEmail)) // User's current confirmed email match exactly the new email
	   {
	    sprintf (Gbl.Message,Txt_The_email_address_X_matches_one_previously_registered,
		     NewEmail);
	    Lay_ShowAlert (Lay_WARNING,Gbl.Message);
	   }
	 else
	   {
	    if (Mai_UpdateEmailInDB (UsrDat,NewEmail))
	      {
	       /***** Email updated sucessfully *****/
	       sprintf (Gbl.Message,Txt_The_email_address_X_has_been_registered_successfully,
			NewEmail);
	       Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);

	       /***** Update list of emails *****/
	       Mai_GetEmailFromUsrCod (UsrDat);

	       /***** Send message via email
		      to confirm the new email address *****/
	       if (ItsMe)
		  Mai_SendMailMsgToConfirmEmail ();
	      }
	    else
	      {
	       sprintf (Gbl.Message,Txt_The_email_address_X_had_been_registered_by_another_user,
			NewEmail);
	       Lay_ShowAlert (Lay_WARNING,Gbl.Message);
	      }
	   }
	}
      else	// New email is not valid
	{
	 sprintf (Gbl.Message,Txt_The_email_address_entered_X_is_not_valid,
		  NewEmail);
	 Lay_ShowAlert (Lay_WARNING,Gbl.Message);
	}
     }
   else
      Lay_ShowAlert (Lay_WARNING,Txt_User_not_found_or_you_do_not_have_permission_);
  }

/*****************************************************************************/
/************************ Update email in database ***************************/
/*****************************************************************************/
// Return true if email is successfully updated
// Return false if email can not be updated beacuse it is registered by another user

bool Mai_UpdateEmailInDB (const struct UsrData *UsrDat,const char *NewEmail)
  {
   char Query[1024];

   /***** Check if the new email matches any of the confirmed emails of other users *****/
   sprintf (Query,"SELECT COUNT(*) FROM usr_emails"
                  " WHERE E_mail='%s' AND Confirmed='Y'"
		  " AND UsrCod<>'%ld'",
	    NewEmail,UsrDat->UsrCod);
   if (DB_QueryCOUNT (Query,"can not check if email already existed"))	// An email of another user is the same that my email
      return false;	// Don't update

   /***** Delete email (not confirmed) for other users *****/
   sprintf (Query,"DELETE FROM pending_emails"
                  " WHERE E_mail='%s' AND UsrCod<>'%ld'",
	    NewEmail,UsrDat->UsrCod);
   DB_QueryDELETE (Query,"can not remove pending email for other users");

   sprintf (Query,"DELETE FROM usr_emails"
                  " WHERE E_mail='%s' AND Confirmed='N'"
		  " AND UsrCod<>'%ld'",
	    NewEmail,UsrDat->UsrCod);
   DB_QueryDELETE (Query,"can not remove not confirmed email for other users");

   /***** Update email in database *****/
   sprintf (Query,"REPLACE INTO usr_emails"
                  " (UsrCod,E_mail,CreatTime) VALUES ('%ld','%s',NOW())",
            UsrDat->UsrCod,NewEmail);
   DB_QueryREPLACE (Query,"can not update email");

   return true;	// Successfully updated
  }

/*****************************************************************************/
/********** Put button to go to check and confirm my email address ***********/
/*****************************************************************************/

void Mai_PutButtonToCheckEmailAddress (void)
  {
   extern const char *Hlp_PROFILE_Account_email;
   extern const char *Txt_Email_unconfirmed;
   extern const char *Txt_Please_check_and_confirm_your_email_address;
   extern const char *Txt_Check;

   /***** Start form *****/
   Act_FormStart (ActFrmMyAcc);

   /***** Frame with button to go to account *****/
   Lay_StartRoundFrame (NULL,Txt_Email_unconfirmed,
                        NULL,Hlp_PROFILE_Account_email);
   Lay_ShowAlert (Lay_WARNING,Txt_Please_check_and_confirm_your_email_address);
   Lay_EndRoundFrameWithButton (Lay_CONFIRM_BUTTON,Txt_Check);

   /***** End form *****/
   Act_FormEnd ();
  }

/*****************************************************************************/
/************** Send mail message to confirm my email address ****************/
/*****************************************************************************/
// Return true on success
// Return false on error

bool Mai_SendMailMsgToConfirmEmail (void)
  {
   extern struct Act_Actions Act_Actions[Act_NUM_ACTIONS];
   extern const char *Txt_If_you_just_request_from_X_the_confirmation_of_your_email_Y_NO_HTML;
   extern const char *Txt_Confirmation_of_your_email_NO_HTML;
   extern const char *Txt_There_was_a_problem_sending_an_email_automatically;
   char Command[2048]; // Command to execute for sending an email
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
            Cfg_URL_SWAD_CGI,Act_Actions[ActCnfMai].ActCod,Gbl.UniqueNameEncrypted,
            Cfg_URL_SWAD_CGI);

   /* Footer note */
   Mai_WriteFootNoteEMail (Gbl.Prefs.Language);

   fclose (Gbl.Msg.FileMail);

   /***** Call the script to send an email *****/
   sprintf (Command,"%s \"%s\" \"%s\" \"%s\" \"%s\" \"%s\" \"[%s] %s\" \"%s\"",
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
         return true;
      case 1:
         Lay_ShowAlert (Lay_WARNING,Txt_There_was_a_problem_sending_an_email_automatically);
         return false;
      default:
         sprintf (Gbl.Message,"Internal error: an email message has not been sent successfully."
                              " Error code returned by the script: %d",
                  ReturnCode);
         Lay_ShowAlert (Lay_ERROR,Gbl.Message);
         return false;
     }
  }

/*****************************************************************************/
/******* Show alert to report that confirmation email has been sent **********/
/*****************************************************************************/

void Mai_ShowMsgConfirmEmailHasBeenSent (void)
  {
   extern const char *Txt_A_message_has_been_sent_to_email_address_X_to_confirm_that_address;

   sprintf (Gbl.Message,Txt_A_message_has_been_sent_to_email_address_X_to_confirm_that_address,
	    Gbl.Usrs.Me.UsrDat.Email);
   Lay_ShowAlert (Lay_INFO,Gbl.Message);
  }

/*****************************************************************************/
/************************* Insert mail hey in database ***********************/
/*****************************************************************************/

static void Mai_InsertMailKey (const char *Email,
                               const char MailKey[Mai_LENGTH_EMAIL_CONFIRM_KEY + 1])
  {
   char Query[512 + Mai_LENGTH_EMAIL_CONFIRM_KEY];

   /***** Remove expired pending emails from database *****/
   sprintf (Query,"DELETE FROM pending_emails"
                  " WHERE DateAndTime<FROM_UNIXTIME(UNIX_TIMESTAMP()-'%lu')",
            Cfg_TIME_TO_DELETE_OLD_PENDING_EMAILS);
   DB_QueryDELETE (Query,"can not remove old pending mail keys");

   /***** Insert mail key in database *****/
   sprintf (Query,"INSERT INTO pending_emails (UsrCod,E_mail,MailKey,DateAndTime)"
                  " VALUES ('%ld','%s','%s',NOW())",
            Gbl.Usrs.Me.UsrDat.UsrCod,
            Email,
            MailKey);
   DB_QueryREPLACE (Query,"can not create pending password");
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
   char Query[1024];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   char MailKey[Mai_LENGTH_EMAIL_CONFIRM_KEY + 1];
   long UsrCod;
   char Email[Usr_MAX_BYTES_USR_EMAIL + 1];
   bool KeyIsCorrect = false;
   bool Confirmed;

   /***** Get parameter Key *****/
   Par_GetParToText ("key",MailKey,Mai_LENGTH_EMAIL_CONFIRM_KEY);

   /***** Get user's code and email from key *****/
   sprintf (Query,"SELECT UsrCod,E_mail FROM pending_emails"
                  " WHERE MailKey='%s'",MailKey);
   if (DB_QuerySELECT (Query,&mysql_res,"can not get user's code and email from key"))
     {
      row = mysql_fetch_row (mysql_res);

      /* Get user's code */
      UsrCod = Str_ConvertStrCodToLongCod (row[0]);

      /* Get user's email */
      Str_Copy (Email,row[1],
                Usr_MAX_BYTES_USR_EMAIL);

      KeyIsCorrect = true;
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   if (KeyIsCorrect)
     {
      /***** Delete this key *****/
      sprintf (Query,"DELETE FROM pending_emails"
                     " WHERE MailKey='%s'",MailKey);
      DB_QueryDELETE (Query,"can not remove an email key");

      /***** Check user's code and email
             and get if email is already confirmed *****/
      sprintf (Query,"SELECT Confirmed FROM usr_emails"
		     " WHERE UsrCod='%ld' AND E_mail='%s'",
	       UsrCod,Email);
      if (DB_QuerySELECT (Query,&mysql_res,"can not get user's code and email"))
	{
         Confirmed = (row[0][0] == 'Y');

         /***** Confirm email *****/
         if (Confirmed)
	    sprintf (Gbl.Message,Txt_Email_X_has_already_been_confirmed_before,
		     Email);
         else
           {
	    sprintf (Query,"UPDATE usr_emails SET Confirmed='Y'"
			   " WHERE usr_emails.UsrCod='%ld'"
			   " AND usr_emails.E_mail='%s'",
		     UsrCod,Email);
	    DB_QueryUPDATE (Query,"can not confirm email");

	    sprintf (Gbl.Message,Txt_The_email_X_has_been_confirmed,Email);
           }
         Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);
	}
      else
	 Lay_ShowAlert (Lay_WARNING,Txt_The_email_address_has_not_been_confirmed);

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);
     }
   else
      Lay_ShowAlert (Lay_WARNING,Txt_Failed_email_confirmation_key);

   /***** Form to log in *****/
   Usr_WriteFormLogin (ActLogIn,NULL);
  }

/*****************************************************************************/
/****************** Create temporary file for mail content *******************/
/*****************************************************************************/

void Mai_CreateFileNameMail (void)
  {
   sprintf (Gbl.Msg.FileNameMail,"%s/%s/%s_mail.txt",
            Cfg_PATH_SWAD_PRIVATE,Cfg_FOLDER_OUT,Gbl.UniqueNameEncrypted);
   if ((Gbl.Msg.FileMail = fopen (Gbl.Msg.FileNameMail,"wb")) == NULL)
      Lay_ShowErrorAndExit ("Can not open file to send email.");
  }

/*****************************************************************************/
/************ Write a welcome note heading the automatic email ***************/
/*****************************************************************************/

void Mai_WriteWelcomeNoteEMail (struct UsrData *UsrDat)
  {
   extern const char *Txt_Dear_NO_HTML[Usr_NUM_SEXS][1 + Txt_NUM_LANGUAGES];
   extern const char *Txt_user_NO_HTML[Usr_NUM_SEXS][1 + Txt_NUM_LANGUAGES];

   fprintf (Gbl.Msg.FileMail,"%s %s:\n",
            Txt_Dear_NO_HTML[UsrDat->Sex][UsrDat->Prefs.Language],
            UsrDat->FirstName[0] ? UsrDat->FirstName :
                                   Txt_user_NO_HTML[UsrDat->Sex][UsrDat->Prefs.Language]);
  }

/*****************************************************************************/
/****************** Write a foot note in the automatic email *****************/
/*****************************************************************************/

void Mai_WriteFootNoteEMail (Txt_Language_t Language)
  {
   extern const char *Txt_Please_do_not_reply_to_this_automatically_generated_email_NO_HTML[1 + Txt_NUM_LANGUAGES];

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
   if (UsrDat->UsrCod == Gbl.Usrs.Me.UsrDat.UsrCod)	// It's me
      return true;

   /* Check if I have permission to see another user's email */
   switch (Gbl.Usrs.Me.LoggedRole)
     {
      case Rol_STUDENT:
	 /* If I am a student in the current course,
	    I can see the email of confirmed teachers */
	 return (UsrDat->RoleInCurrentCrsDB == Rol_TEACHER &&	// A teacher
	         UsrDat->Accepted);				// who accepted registration
      case Rol_TEACHER:
	 /* If I am a teacher in the current course,
	    I can see the email of confirmed students and teachers */
         return (UsrDat->RoleInCurrentCrsDB == Rol_STUDENT ||	// A student
                 UsrDat->RoleInCurrentCrsDB == Rol_TEACHER) &&	// or a teacher
	         UsrDat->Accepted;				// who accepted registration
      case Rol_DEG_ADM:
	 /* If I am an administrator of current degree,
	    I only can see the user's email of users from current degree */
	 return Usr_CheckIfUsrBelongsToDeg (UsrDat->UsrCod,
	                                    Gbl.CurrentDeg.Deg.DegCod);
      case Rol_CTR_ADM:
	 /* If I am an administrator of current centre,
	    I only can see the user's email of users from current centre */
	 return Usr_CheckIfUsrBelongsToCtr (UsrDat->UsrCod,
	                                    Gbl.CurrentCtr.Ctr.CtrCod);
      case Rol_INS_ADM:
	 /* If I am an administrator of current institution,
	    I only can see the user's email of users from current institution */
	 return Usr_CheckIfUsrBelongsToIns (UsrDat->UsrCod,
	                                    Gbl.CurrentIns.Ins.InsCod);
      case Rol_SYS_ADM:
	 return true;
      default:
	 return false;
     }
  }
