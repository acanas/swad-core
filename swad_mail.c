// swad_mail.c: all the stuff related to e-mail

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2015 Antonio Cañas Vargas

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
#include "swad_text.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;
extern struct Act_Actions Act_Actions[Act_NUM_ACTIONS];

/*****************************************************************************/
/***************************** Private constants *****************************/
/*****************************************************************************/

#define Mai_LENGTH_EMAIL_CONFIRM_KEY Cry_LENGTH_ENCRYPTED_STR_SHA256_BASE64

/*****************************************************************************/
/******************************* Private types *******************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private variables *****************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Mai_GetParamMaiOrderType (void);
static void Mai_PutFormToEditMailDomains (void);
static void Mai_GetListMailDomainsAllowedForNotif (void);
static void Mai_ListMailDomainsForEdition (void);
static void Mai_PutParamMaiCod (long MaiCod);
static void Mai_RenameMailDomain (Cns_ShortOrFullName_t ShortOrFullName);
static bool Mai_CheckIfMailDomainNameExists (const char *FieldName,const char *Name,long MaiCod);
static void Mai_PutFormToCreateMailDomain (void);
static void Mai_PutHeadMailDomains (void);
static void Mai_CreateMailDomain (struct Mail *Mai);

static void Mai_RemoveEmailFromDB (const char *Email);
static void Mai_InsertMailKey (const char *Email,const char MailKey[Mai_LENGTH_EMAIL_CONFIRM_KEY+1]);

/*****************************************************************************/
/************************* List all the mail domains *************************/
/*****************************************************************************/

void Mai_SeeMailDomains (void)
  {
   extern const char *Txt_Mail_domains_allowed_for_notifications;
   extern const char *Txt_MAIL_DOMAIN_HELP_ORDER[3];
   extern const char *Txt_MAIL_DOMAIN_ORDER[3];
   Mai_MailDomainsOrderType_t Order;
   unsigned NumMai;

   /***** Get parameter with the type of order in the list of mail domains *****/
   Mai_GetParamMaiOrderType ();

   /***** Get list of mail domains *****/
   Mai_GetListMailDomainsAllowedForNotif ();

   /***** Put link (form) to edit mail domains *****/
   if (Gbl.Usrs.Me.LoggedRole == Rol_SYS_ADM)
      Mai_PutFormToEditMailDomains ();

   /***** Table head *****/
   Lay_StartRoundFrameTable (NULL,2,Txt_Mail_domains_allowed_for_notifications);
   fprintf (Gbl.F.Out,"<tr>");
   for (Order = Mai_ORDER_BY_DOMAIN;
	Order <= Mai_ORDER_BY_USERS;
	Order++)
     {
      fprintf (Gbl.F.Out,"<th class=\"LEFT_MIDDLE\">");
      Act_FormStart (ActSeeMai);
      Par_PutHiddenParamUnsigned ("Order",(unsigned) Order);
      Act_LinkFormSubmit (Txt_MAIL_DOMAIN_HELP_ORDER[Order],"TIT_TBL");
      if (Order == Gbl.Mails.SelectedOrderType)
         fprintf (Gbl.F.Out,"<u>");
      fprintf (Gbl.F.Out,"%s",Txt_MAIL_DOMAIN_ORDER[Order]);
      if (Order == Gbl.Mails.SelectedOrderType)
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

   /***** Table end *****/
   Lay_EndRoundFrameTable ();

   /***** Free list of mail domains *****/
   Mai_FreeListMailDomains ();
  }

/*****************************************************************************/
/******* Get parameter with the type or order in list of mail domains ********/
/*****************************************************************************/

static void Mai_GetParamMaiOrderType (void)
  {
   char UnsignedStr[10+1];
   unsigned UnsignedNum;

   Par_GetParToText ("Order",UnsignedStr,10);
   if (sscanf (UnsignedStr,"%u",&UnsignedNum) == 1)
      Gbl.Mails.SelectedOrderType = (Mai_MailDomainsOrderType_t) UnsignedNum;
   else
      Gbl.Mails.SelectedOrderType = Mai_DEFAULT_ORDER_TYPE;
  }

/*****************************************************************************/
/******************** Put a mail (form) to edit mail domains *****************/
/*****************************************************************************/

static void Mai_PutFormToEditMailDomains (void)
  {
   extern const char *Txt_Edit;

   fprintf (Gbl.F.Out,"<div class=\"CONTEXT_MENU\">");
   Act_PutContextualLink (ActEdiMai,NULL,"edit",Txt_Edit);
   fprintf (Gbl.F.Out,"</div>");
  }

/*****************************************************************************/
/*********************** Put forms to edit mail domains **********************/
/*****************************************************************************/

void Mai_EditMailDomains (void)
  {
   extern const char *Txt_There_are_no_mail_domains;

   /***** Get list of mail domains *****/
   Mai_GetListMailDomainsAllowedForNotif ();

   if (Gbl.Mails.Num)
      /***** Put link (form) to view mail domains *****/
      Lay_PutFormToView (ActSeeMai);
   else
      /***** Help message *****/
      Lay_ShowAlert (Lay_INFO,Txt_There_are_no_mail_domains);

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
   sprintf (Query,"DROP TABLE IF EXISTS T1,T2");
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
   switch (Gbl.Mails.SelectedOrderType)
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
         strcpy (Mai->Domain,row[1]);

         /* Get the mail domain info (row[2]) */
         strcpy (Mai->Info,row[2]);

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
   sprintf (Query,"DROP TABLE IF EXISTS T1,T2");
   if (mysql_query (&Gbl.mysql,Query))
      DB_ExitOnMySQLError ("can not remove temporary tables");
  }

/*****************************************************************************/
/************ Check if a mail domain is allowed for notifications ************/
/*****************************************************************************/

bool Mai_CheckIfMailDomainIsAllowedForNotifications (const char *MailDomain)
  {
   char Query[512];

   /***** Get number of mail_domains with a name from database *****/
   sprintf (Query,"SELECT COUNT(*) FROM mail_domains WHERE Domain='%s'",
            MailDomain);
   return (DB_QueryCOUNT (Query,"can not check if a mail domain is allowed for notifications") != 0);
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
         strcpy (Mai->Domain,row[0]);

         /* Get the full name of the mail (row[1]) */
         strcpy (Mai->Info,row[1]);
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
   extern const char *Txt_Mail_domains_allowed_for_notifications;
   unsigned NumMai;
   struct Mail *Mai;

   Lay_StartRoundFrameTable (NULL,2,Txt_Mail_domains_allowed_for_notifications);

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
               Gbl.FormId);
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
               Gbl.FormId);
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
   char LongStr[1+10+1];

   /***** Get parameter with code of mail *****/
   Par_GetParToText ("MaiCod",LongStr,1+10);
   return Str_ConvertStrCodToLongCod (LongStr);
  }

/*****************************************************************************/
/******************************* Remove a mail *******************************/
/*****************************************************************************/

void Mai_RemoveMailDomain (void)
  {
   extern const char *Txt_Mail_domain_X_removed;
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
   sprintf (Gbl.Message,Txt_Mail_domain_X_removed,
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
   Mai_RenameMailDomain (Cns_SHORT_NAME);
  }

/*****************************************************************************/
/********************* Change the full name of a mail ************************/
/*****************************************************************************/

void Mai_RenameMailDomainFull (void)
  {
   Mai_RenameMailDomain (Cns_FULL_NAME);
  }

/*****************************************************************************/
/************************ Change the name of a mail *************************/
/*****************************************************************************/

static void Mai_RenameMailDomain (Cns_ShortOrFullName_t ShortOrFullName)
  {
   extern const char *Txt_You_can_not_leave_the_name_of_the_mail_domain_X_empty;
   extern const char *Txt_The_mail_domain_X_already_exists;
   extern const char *Txt_The_mail_domain_X_has_been_renamed_as_Y;
   extern const char *Txt_The_mail_domain_X_has_not_changed;
   char Query[512];
   struct Mail *Mai;
   const char *ParamName = NULL;	// Initialized to avoid warning
   const char *FieldName = NULL;	// Initialized to avoid warning
   unsigned MaxLength = 0;		// Initialized to avoid warning
   char *CurrentMaiName = NULL;		// Initialized to avoid warning
   char NewMaiName[Mai_MAX_LENGTH_MAIL_INFO+1];

   Mai = &Gbl.Mails.EditingMai;
   switch (ShortOrFullName)
     {
      case Cns_SHORT_NAME:
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
      sprintf (Gbl.Message,Txt_You_can_not_leave_the_name_of_the_mail_domain_X_empty,
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
            sprintf (Gbl.Message,Txt_The_mail_domain_X_already_exists,
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
            sprintf (Gbl.Message,Txt_The_mail_domain_X_has_been_renamed_as_Y,
                     CurrentMaiName,NewMaiName);
            Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);
           }
        }
      else	// The same name
        {
         sprintf (Gbl.Message,Txt_The_mail_domain_X_has_not_changed,
                  CurrentMaiName);
         Lay_ShowAlert (Lay_INFO,Gbl.Message);
        }
     }

   /***** Show the form again *****/
   strcpy (CurrentMaiName,NewMaiName);
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
   extern const char *Txt_New_mail_domain;
   extern const char *Txt_MAIL_DOMAIN_ORDER[3];
   extern const char *Txt_Create_mail_domain;
   struct Mail *Mai;

   Mai = &Gbl.Mails.EditingMai;

   /***** Start form *****/
   Act_FormStart (ActNewMai);

   /***** Start of frame *****/
   Lay_StartRoundFrameTable (NULL,2,Txt_New_mail_domain);

   /***** Write heading *****/
   fprintf (Gbl.F.Out,"<tr>"
                      "<th class=\"LEFT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "<th class=\"LEFT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "</tr>",
            Txt_MAIL_DOMAIN_ORDER[Mai_ORDER_BY_DOMAIN],
            Txt_MAIL_DOMAIN_ORDER[Mai_ORDER_BY_INFO  ]);

   /***** Mail domain *****/
   fprintf (Gbl.F.Out,"<tr>"
                      "<td class=\"CENTER_MIDDLE\">"
                      "<input type=\"text\" name=\"Domain\" size=\"15\" maxlength=\"%u\" value=\"%s\" />"
                      "</td>",
            Mai_MAX_LENGTH_MAIL_DOMAIN,Mai->Domain);

   /***** Mail domain info *****/
   fprintf (Gbl.F.Out,"<td class=\"CENTER_MIDDLE\">"
                      "<input type=\"text\" name=\"Info\" size=\"40\" maxlength=\"%u\" value=\"%s\" />"
                      "</td>"
                      "<td></td>"
                      "</tr>",
            Mai_MAX_LENGTH_MAIL_INFO,Mai->Info);


   /***** Send button and end frame *****/
   Lay_EndRoundFrameTableWithButton (Lay_CREATE_BUTTON,Txt_Create_mail_domain);

   /***** End of form *****/
   Act_FormEnd ();
  }

/*****************************************************************************/
/********************* Write header with fields of a mail ********************/
/*****************************************************************************/

static void Mai_PutHeadMailDomains (void)
  {
   extern const char *Txt_Code;
   extern const char *Txt_MAIL_DOMAIN_ORDER[3];

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
            Txt_MAIL_DOMAIN_ORDER[Mai_ORDER_BY_DOMAIN],
            Txt_MAIL_DOMAIN_ORDER[Mai_ORDER_BY_INFO  ],
            Txt_MAIL_DOMAIN_ORDER[Mai_ORDER_BY_USERS ]);
  }

/*****************************************************************************/
/******************* Receive form to create a new mail ***********************/
/*****************************************************************************/

void Mai_RecFormNewMailDomain (void)
  {
   extern const char *Txt_The_mail_domain_X_already_exists;
   extern const char *Txt_You_must_specify_the_short_name_and_the_full_name_of_the_new_mail_domain;
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
         sprintf (Gbl.Message,Txt_The_mail_domain_X_already_exists,
                  Mai->Domain);
         Lay_ShowAlert (Lay_WARNING,Gbl.Message);
        }
      else if (Mai_CheckIfMailDomainNameExists ("Info",Mai->Info,-1L))
        {
         sprintf (Gbl.Message,Txt_The_mail_domain_X_already_exists,
                  Mai->Info);
         Lay_ShowAlert (Lay_WARNING,Gbl.Message);
        }
      else	// Add new mail to database
         Mai_CreateMailDomain (Mai);
     }
   else	// If there is not a mail name
      Lay_ShowAlert (Lay_WARNING,Txt_You_must_specify_the_short_name_and_the_full_name_of_the_new_mail_domain);

   /***** Show the form again *****/
   Mai_EditMailDomains ();
  }

/*****************************************************************************/
/************************** Create a new mail domain *************************/
/*****************************************************************************/

static void Mai_CreateMailDomain (struct Mail *Mai)
  {
   extern const char *Txt_Created_new_mail_domain_X;
   char Query[1024];

   /***** Create a new mail *****/
   sprintf (Query,"INSERT INTO mail_domains (Domain,Info) VALUES ('%s','%s')",
            Mai->Domain,Mai->Info);
   DB_QueryINSERT (Query,"can not create mail domain");

   /***** Write success message *****/
   sprintf (Gbl.Message,Txt_Created_new_mail_domain_X,
            Mai->Domain);
   Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);
  }

/*****************************************************************************/
/**************** Check whether an e-mail address if valid *******************/
/*****************************************************************************/

bool Mai_CheckIfEmailIsValid (const char *Email)
  {
   unsigned Length = strlen (Email);
   unsigned LastPosArroba = Length - 4;
   const char *Ptr;
   unsigned Pos;
   bool ArrobaFound = false;

   /***** An e-mail address must have a number of characters
          5 <= Length <= Mai_MAX_BYTES_USR_EMAIL *****/
   if (Length < 5 ||
       Length > Usr_MAX_BYTES_USR_EMAIL)
      return false;

   /***** An e-mail address can have digits, letters, '.', '-' and '_';
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
/********** Get e-mail address of a user from his/her user's code ************/
/*****************************************************************************/

bool Mai_GetEmailFromUsrCod (long UsrCod,char *Email,bool *Confirmed)
  {
   char Query[256];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;
   bool Found;

   /***** Get current (last updated) user's nickname from database *****/
   sprintf (Query,"SELECT E_mail,Confirmed FROM usr_emails"
	          " WHERE UsrCod='%ld' ORDER BY CreatTime DESC LIMIT 1",
	    UsrCod);
   NumRows = DB_QuerySELECT (Query,&mysql_res,"can not get e-mail address");

   if (NumRows == 0)
     {
      *Email = '\0';
      *Confirmed = false;
      Found = false;
     }
   else
     {
      row = mysql_fetch_row (mysql_res);

      /* Get e-mail */
      strcpy (Email,row[0]);

      *Confirmed = (Str_ConvertToUpperLetter (row[1][0]) == 'Y');

      Found = true;
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return Found;
  }

/*****************************************************************************/
/************* Get user's code of a user from his/her e-mail *****************/
/*****************************************************************************/
// Returns -1L if e-mail not found

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
	    /* User not found for this e-mail ==> set user's code to void */
	    UsrCod = -1L;
	 else if (NumUsrs == 1)	// One user found
	   {
	    /* Get row */
	    row = mysql_fetch_row (mysql_res);

	    /* Get user's code */
	    UsrCod = Str_ConvertStrCodToLongCod (row[0]);
	   }
	 else	// NumRows > 1 ==> impossible, an e-mail can not be reapeated
	    Lay_ShowErrorAndExit ("Internal error: e-mail is repeated in database.");

	 /***** Free structure that stores the query result *****/
	 DB_FreeMySQLResult (&mysql_res);
	}

   return UsrCod;
  }

/*****************************************************************************/
/*********************** Show form to change my e-mail ***********************/
/*****************************************************************************/

void Mai_ShowFormChangeUsrEmail (void)
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

   /***** Get my e-mails *****/
   sprintf (Query,"SELECT E_mail,Confirmed FROM usr_emails"
                  " WHERE UsrCod='%ld'"
                  " ORDER BY CreatTime DESC",
            Gbl.Usrs.Me.UsrDat.UsrCod);
   NumEmails = (unsigned) DB_QuerySELECT (Query,&mysql_res,"can not get old e-mail addresses of a user");

   /***** List my e-mails *****/
   for (NumEmail = 1;
	NumEmail <= NumEmails;
	NumEmail++)
     {
      /* Get e-mail */
      row = mysql_fetch_row (mysql_res);
      Confirmed = (Str_ConvertToUpperLetter (row[1][0]) == 'Y');

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

	 /* Form to remove old e-mail */
	 Act_FormStart (ActRemOldMai);
	 fprintf (Gbl.F.Out,"<input type=\"hidden\" name=\"Email\" value=\"%s\" />",
		  row[0]);
	 Lay_PutIconRemove ();
	 Act_FormEnd ();
	}

      /* E-mail */
      fprintf (Gbl.F.Out,"<span class=\"%s\">%s</span>",
	       NumEmail == 1 ? "USR_ID" :
		               "DAT",
               row[0]);

      /* E-mail confirmed? */
      if (Confirmed)
	{
	 sprintf (Gbl.Title,Txt_Email_X_confirmed,row[0]);
	 fprintf (Gbl.F.Out,"<img src=\"%s/ok_green16x16.gif\""
			    " alt=\"%s\" title=\"%s\""
			    " class=\"ICON16x16\" />",
		  Gbl.Prefs.IconsURL,
		  Gbl.Title,Gbl.Title);
	}

      /* Link to QR code */
      // if (NumEmail == 1 && Gbl.Usrs.Me.UsrDat.Email[0] && Confirmed)
      //    QR_PutLinkToPrintMyQRCode (QR_EMAIL);

      fprintf (Gbl.F.Out,"</div>");

      /* Form to change the email */
      if (NumEmail > 1 || !Confirmed)
	{
	 Act_FormStart (ActChgMai);
	 fprintf (Gbl.F.Out,"<input type=\"hidden\" name=\"NewEmail\" value=\"%s\" />",
		  row[0]);	// E-mail
         Lay_PutConfirmButtonInline (NumEmail == 1 ? Txt_Confirm_email :
			                             Txt_Use_this_email);
	 Act_FormEnd ();
	 fprintf (Gbl.F.Out,"</td>");
	}

      fprintf (Gbl.F.Out,"</td>"
			 "</tr>");
     }

   /***** Form to enter new e-mail *****/
   fprintf (Gbl.F.Out,"<tr>"
                      "<td class=\"%s RIGHT_MIDDLE\">"
                      "%s:"
                      "</td>"
                      "<td class=\"LEFT_MIDDLE\">",
            The_ClassForm[Gbl.Prefs.Theme],
            NumEmails ? Txt_New_email :	// A new e-mail
        	        Txt_Email);	// The first e-mail
   Act_FormStart (ActChgMai);
   fprintf (Gbl.F.Out,"<div class=\"FORM_ACCOUNT\">"
	              "<input type=\"text\" name=\"NewEmail\""
	              " size=\"20\" maxlength=\"%u\" value=\"%s\" />"
	              "</div>",
            Usr_MAX_BYTES_USR_EMAIL,
            Gbl.Usrs.Me.UsrDat.Email);
   Lay_PutCreateButtonInline (NumEmails ? Txt_Change_email :	// I already have an e-mail address
        	                          Txt_Save);		// I have no e-mail address yet);
   Act_FormEnd ();
   fprintf (Gbl.F.Out,"</td>"
	              "</tr>");
  }

/*****************************************************************************/
/************************** Remove e-mail address ****************************/
/*****************************************************************************/

void Mai_RemoveEmail (void)
  {
   extern const char *Txt_Email_X_removed;
   extern const char *Txt_You_can_not_delete_your_current_email;
   char Email[Usr_MAX_BYTES_USR_EMAIL+1];

   /***** Get new e-mail from form *****/
   Par_GetParToText ("Email",Email,Usr_MAX_BYTES_USR_EMAIL);

   if (strcasecmp (Email,Gbl.Usrs.Me.UsrDat.Email))	// Only if not my current e-mail
     {
      /***** Remove one of my old e-mail addresses *****/
      Mai_RemoveEmailFromDB (Email);

      /***** Show message *****/
      sprintf (Gbl.Message,Txt_Email_X_removed,Email);
      Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);
     }
   else
      Lay_ShowAlert (Lay_WARNING,Txt_You_can_not_delete_your_current_email);

   /***** Show my account again *****/
   Acc_ShowFormChangeMyAccount ();
  }

/*****************************************************************************/
/*************** Remove an old e-mail address from database ******************/
/*****************************************************************************/

static void Mai_RemoveEmailFromDB (const char *Email)
  {
   char Query[1024];

   /***** Remove an old e-mail address *****/
   sprintf (Query,"DELETE FROM usr_emails"
                  " WHERE UsrCod='%ld' AND E_mail='%s'",
            Gbl.Usrs.Me.UsrDat.UsrCod,Email);
   DB_QueryREPLACE (Query,"can not remove an old e-mail address");
  }

/*****************************************************************************/
/************************* Update my e-mail address **************************/
/*****************************************************************************/

void Mai_UpdateEmail (void)
  {
   extern const char *Txt_The_email_address_X_matches_the_one_you_had_previously_registered;
   extern const char *Txt_Your_email_address_X_has_been_registered_successfully;
   extern const char *Txt_A_message_has_been_sent_to_email_address_X_to_confirm_that_address;
   extern const char *Txt_The_email_address_X_had_been_registered_by_another_user;
   extern const char *Txt_The_email_address_entered_X_is_not_valid;
   char NewEmail[Usr_MAX_BYTES_USR_EMAIL+1];

   /***** Get new e-mail from form *****/
   Par_GetParToText ("NewEmail",NewEmail,Usr_MAX_BYTES_USR_EMAIL);

   if (Mai_CheckIfEmailIsValid (NewEmail))	// New e-mail is valid
     {
      /***** Check if new e-mail exists in database *****/
      if (Gbl.Usrs.Me.UsrDat.EmailConfirmed &&
	  !strcmp (Gbl.Usrs.Me.UsrDat.Email,NewEmail)) // My current confirmed email match exactly the new email
        {
	 sprintf (Gbl.Message,Txt_The_email_address_X_matches_the_one_you_had_previously_registered,
		  NewEmail);
	 Lay_ShowAlert (Lay_WARNING,Gbl.Message);
        }
      else
	{
	 if (Mai_UpdateEmailInDB (&Gbl.Usrs.Me.UsrDat,NewEmail))
	   {
	    /* E-mail updated sucessfully */
	    strcpy (Gbl.Usrs.Me.UsrDat.Email,NewEmail);
	    Gbl.Usrs.Me.UsrDat.EmailConfirmed = false;

            sprintf (Gbl.Message,Txt_Your_email_address_X_has_been_registered_successfully,
		     NewEmail);
	    Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);

            /* Send message via email to confirm the new email address */
            if (Mai_SendMailMsgToConfirmEmail ())
              {
	       sprintf (Gbl.Message,Txt_A_message_has_been_sent_to_email_address_X_to_confirm_that_address,
			Gbl.Usrs.Me.UsrDat.Email);
	       Lay_ShowAlert (Lay_INFO,Gbl.Message);
              }
	   }
	 else
           {
	    sprintf (Gbl.Message,Txt_The_email_address_X_had_been_registered_by_another_user,
	             NewEmail);
	    Lay_ShowAlert (Lay_WARNING,Gbl.Message);
	   }
        }
     }
   else	// New e-mail is not valid
     {
      sprintf (Gbl.Message,Txt_The_email_address_entered_X_is_not_valid,
               NewEmail);
      Lay_ShowAlert (Lay_WARNING,Gbl.Message);
     }

   /***** Show my account again *****/
   Acc_ShowFormChangeMyAccount ();
  }

/*****************************************************************************/
/************************ Update e-mail in database **************************/
/*****************************************************************************/
// Return true if e-mail is successfully updated
// Return false if e-mail can not be updated beacuse it is registered by another user

bool Mai_UpdateEmailInDB (struct UsrData *UsrDat,const char *NewEmail)
  {
   char Query[1024];

   /***** Check if the new e-mail matches any of the confirmed e-mails of other users *****/
   sprintf (Query,"SELECT COUNT(*) FROM usr_emails"
                  " WHERE E_mail='%s' AND Confirmed='Y'"
		  " AND UsrCod<>'%ld'",
	    NewEmail,Gbl.Usrs.Me.UsrDat.UsrCod);
   if (DB_QueryCOUNT (Query,"can not check if e-mail already existed"))	// An e-mail of another user is the same that my e-mail
      return false;	// Don't update

   /***** Delete e-mail (not confirmed) for other users *****/
   sprintf (Query,"DELETE FROM pending_emails"
                  " WHERE E_mail='%s' AND UsrCod<>'%ld'",
	    NewEmail,Gbl.Usrs.Me.UsrDat.UsrCod);
   DB_QueryDELETE (Query,"can not remove pending e-mail for other users");

   sprintf (Query,"DELETE FROM usr_emails"
                  " WHERE E_mail='%s' AND Confirmed='N'"
		  " AND UsrCod<>'%ld'",
	    NewEmail,Gbl.Usrs.Me.UsrDat.UsrCod);
   DB_QueryDELETE (Query,"can not remove not confirmed e-mail for other users");

   /***** Update e-mail in database *****/
   sprintf (Query,"REPLACE INTO usr_emails"
                  " (UsrCod,E_mail,CreatTime) VALUES ('%ld','%s',NOW())",
            UsrDat->UsrCod,NewEmail);
   DB_QueryREPLACE (Query,"can not update e-mail");

   return true;	// Successfully updated
  }

/*****************************************************************************/
/********************** Show form for changing my password *******************/
/*****************************************************************************/
// Return true on success
// Return false on error

bool Mai_SendMailMsgToConfirmEmail (void)
  {
   extern const char *Txt_If_you_just_request_from_X_the_confirmation_of_your_email_Y_NO_HTML;
   extern const char *Txt_Confirmation_of_your_email_NO_HTML;
   extern const char *Txt_There_was_a_problem_sending_an_email_automatically;
   char Command[2048]; // Command to execute for sending an e-mail
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
	    Cfg_HTTPS_URL_SWAD_CGI,Gbl.Usrs.Me.UsrDat.Email,
            Cfg_HTTPS_URL_SWAD_CGI,Act_Actions[ActCnfMai].ActCod,Gbl.UniqueNameEncrypted,
            Cfg_HTTPS_URL_SWAD_CGI);

   /* Footer note */
   Mai_WriteFootNoteEMail (Gbl.Prefs.Language);

   fclose (Gbl.Msg.FileMail);

   /***** Call the script to send an e-mail *****/
   sprintf (Command,"%s \"%s\" \"%s\" \"%s\" \"%s\" \"%s\" \"[%s] %s\" \"%s\"",
            Cfg_COMMAND_SEND_AUTOMATIC_E_MAIL,
            Cfg_AUTOMATIC_EMAIL_SMTP_SERVER,
	    Cfg_AUTOMATIC_EMAIL_SMTP_PORT,
            Cfg_AUTOMATIC_EMAIL_FROM,
            Gbl.Config.SMTPPassword,
            Gbl.Usrs.Me.UsrDat.Email,
            Cfg_PLATFORM_SHORT_NAME,Txt_Confirmation_of_your_email_NO_HTML,
            Gbl.Msg.FileNameMail);
   ReturnCode = system (Command);
   if (ReturnCode == -1)
      Lay_ShowErrorAndExit ("Error when running script to send e-mail.");

   /***** Remove temporary file *****/
   unlink (Gbl.Msg.FileNameMail);

   /***** Write message depending on return code *****/
   ReturnCode = WEXITSTATUS(ReturnCode);
   switch (ReturnCode)
     {
      case 0: // Message sent successfully
         return true;
      case 1:
         Lay_ShowAlert (Lay_WARNING,Txt_There_was_a_problem_sending_an_email_automatically);
         // Lay_ShowAlert (Lay_ERROR,Command);
         return false;
      default:
         sprintf (Gbl.Message,"Internal error: an email message has not been sent successfully."
                              " Error code returned by the script: %d",
                  ReturnCode);
         Lay_ShowAlert (Lay_ERROR,Gbl.Message);
         // Lay_ShowAlert (Lay_ERROR,Command);
         return false;
     }
  }

/*****************************************************************************/
/************************* Set my pending password ***************************/
/*****************************************************************************/

static void Mai_InsertMailKey (const char *Email,const char MailKey[Mai_LENGTH_EMAIL_CONFIRM_KEY+1])
  {
   char Query[512+Mai_LENGTH_EMAIL_CONFIRM_KEY];

   /***** Remove expired pending passwords from database *****/
   sprintf (Query,"DELETE FROM pending_emails"
                  " WHERE (UNIX_TIMESTAMP() > UNIX_TIMESTAMP(DateAndTime)+'%ld')",
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
/************************* Confirm my e-mail address *************************/
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
   char MailKey[Mai_LENGTH_EMAIL_CONFIRM_KEY+1];
   long UsrCod;
   char Email[Usr_MAX_BYTES_USR_EMAIL+1];
   bool KeyIsCorrect = false;
   bool Confirmed;

   /***** Get parameter Key *****/
   Par_GetParToText ("key",MailKey,Mai_LENGTH_EMAIL_CONFIRM_KEY);

   /***** Get user's code and e-mail from key *****/
   sprintf (Query,"SELECT UsrCod,E_mail FROM pending_emails"
                  " WHERE MailKey='%s'",MailKey);
   if (DB_QuerySELECT (Query,&mysql_res,"can not get user's code and e-mail from key"))
     {
      row = mysql_fetch_row (mysql_res);

      /* Get user's code */
      UsrCod = Str_ConvertStrCodToLongCod (row[0]);

      /* Get user's e-mail */
      strcpy (Email,row[1]);

      KeyIsCorrect = true;
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   if (KeyIsCorrect)
     {
      /***** Delete this key *****/
      sprintf (Query,"DELETE FROM pending_emails"
                     " WHERE MailKey='%s'",MailKey);
      DB_QueryDELETE (Query,"can not remove an e-mail key");

      /***** Check user's code and e-mail
             and get if e-mail is already confirmed *****/
      sprintf (Query,"SELECT Confirmed FROM usr_emails"
		     " WHERE UsrCod='%ld' AND E_mail='%s'",
	       UsrCod,Email);
      if (DB_QuerySELECT (Query,&mysql_res,"can not get user's code and e-mail"))
	{
         Confirmed = (Str_ConvertToUpperLetter (row[0][0]) == 'Y');

         /***** Confirm e-mail *****/
         if (Confirmed)
	    sprintf (Gbl.Message,Txt_Email_X_has_already_been_confirmed_before,
		     Email);
         else
           {
	    sprintf (Query,"UPDATE usr_emails SET Confirmed='Y'"
			   " WHERE usr_emails.UsrCod='%ld'"
			   " AND usr_emails.E_mail='%s'",
		     UsrCod,Email);
	    DB_QueryUPDATE (Query,"can not confirm e-mail");
	    sprintf (Gbl.Message,Txt_The_email_X_has_been_confirmed,Email);
	    strcat (Gbl.Message,".");
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
   Usr_WriteFormLogin ();
  }

/*****************************************************************************/
/****************** Create temporary file for mail content *******************/
/*****************************************************************************/

void Mai_CreateFileNameMail (void)
  {
   sprintf (Gbl.Msg.FileNameMail,"%s/%s/%s_mail.txt",
            Cfg_PATH_SWAD_PRIVATE,Cfg_FOLDER_OUT,Gbl.UniqueNameEncrypted);
   if ((Gbl.Msg.FileMail = fopen (Gbl.Msg.FileNameMail,"wb")) == NULL)
      Lay_ShowErrorAndExit ("Can not open file to send e-mail.");
  }

/*****************************************************************************/
/************ Write a welcome note heading the automatic e-mail **************/
/*****************************************************************************/

void Mai_WriteWelcomeNoteEMail (struct UsrData *UsrDat)
  {
   extern const char *Txt_Dear_NO_HTML[Usr_NUM_SEXS][Txt_NUM_LANGUAGES];
   extern const char *Txt_user_NO_HTML[Usr_NUM_SEXS][Txt_NUM_LANGUAGES];

   fprintf (Gbl.Msg.FileMail,"%s %s:\n",
            Txt_Dear_NO_HTML[UsrDat->Sex][UsrDat->Prefs.Language],
            UsrDat->FirstName[0] ? UsrDat->FirstName :
                                   Txt_user_NO_HTML[UsrDat->Sex][UsrDat->Prefs.Language]);
  }

/*****************************************************************************/
/****************** Write a foot note in the automatic e-mail ****************/
/*****************************************************************************/

void Mai_WriteFootNoteEMail (Txt_Language_t Language)
  {
   extern const char *Txt_Please_do_not_reply_to_this_automatically_generated_email_NO_HTML[Txt_NUM_LANGUAGES];

   fprintf (Gbl.Msg.FileMail,"%s\n"
                             "%s\n"
                             "%s\n",
            Txt_Please_do_not_reply_to_this_automatically_generated_email_NO_HTML[Language],
            Cfg_PLATFORM_SHORT_NAME,
            Cfg_HTTPS_URL_SWAD_CGI);
  }
