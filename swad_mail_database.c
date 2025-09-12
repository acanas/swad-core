// swad_mail_database.c: everything related to email, operations with database

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

#include "swad_database.h"
#include "swad_global.h"
#include "swad_mail.h"
#include "swad_mail_database.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/*************************** Update a user's email ***************************/
/*****************************************************************************/

void Mai_DB_UpdateEmail (long UsrCod,const char NewEmail[Cns_MAX_BYTES_EMAIL_ADDRESS + 1])
  {
   DB_QueryREPLACE ("can not update email",
		    "REPLACE INTO usr_emails"
		    " (UsrCod,E_mail,CreatTime)"
		    " VALUES"
		    " (%ld,'%s',NOW())",
                    UsrCod,
                    NewEmail);
  }

/*****************************************************************************/
/*************************** Confirm a user's email **************************/
/*****************************************************************************/

void Mai_DB_ConfirmEmail (long UsrCod,const char Email[Cns_MAX_BYTES_EMAIL_ADDRESS + 1])
  {
   DB_QueryUPDATE ("can not confirm email",
		   "UPDATE usr_emails"
		     " SET Confirmed='Y'"
		   " WHERE usr_emails.UsrCod=%ld"
		     " AND usr_emails.E_mail='%s'",
		   UsrCod,
		   Email);
  }

/*****************************************************************************/
/********************************* Get my emails *****************************/
/*****************************************************************************/

unsigned Mai_DB_GetMyEmails (MYSQL_RES **mysql_res,long UsrCod)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get old email addresses of a user",
		   "SELECT E_mail,"	// row[0]
		          "Confirmed"	// row[1]
		    " FROM usr_emails"
		   " WHERE UsrCod=%ld"
		" ORDER BY CreatTime DESC",
		   UsrCod);
  }

/*****************************************************************************/
/********** Get email address of a user from his/her user's code *************/
/*****************************************************************************/

Exi_Exist_t Mai_DB_GetEmailFromUsrCod (MYSQL_RES **mysql_res,long UsrCod)
  {
   return
   DB_QuerySELECT (mysql_res,"can not get email address",
		   "SELECT E_mail,"	// row[0]
			  "Confirmed"	// row[1]
		    " FROM usr_emails"
		   " WHERE UsrCod=%ld"
		" ORDER BY CreatTime DESC"
		   " LIMIT 1",
		   UsrCod) ? Exi_EXISTS :
			     Exi_DOES_NOT_EXIST;
  }

/*****************************************************************************/
/************* Get user's code of a user from his/her email ******************/
/*****************************************************************************/
// Returns -1L if email not found

long Mai_DB_GetUsrCodFromEmail (const char Email[Cns_MAX_BYTES_EMAIL_ADDRESS + 1])
  {
   /***** Trivial check 1: email should be not null ******/
   if (!Email)
      return -1L;

   /***** Trivial check 2: email should be not empty ******/
   if (!Email[0])
      return -1L;

   /***** Get user's code from database *****/
   return DB_QuerySELECTCode ("can not get user's code",
			      "SELECT usr_emails.UsrCod"
			       " FROM usr_emails,"
				     "usr_data"
			      " WHERE usr_emails.E_mail='%s'"
				" AND usr_emails.UsrCod=usr_data.UsrCod",
			      Email);
  }

/*****************************************************************************/
/******************** Check if a user's email is confirmed *******************/
/*****************************************************************************/

char Mai_DB_CheckIfEmailIsConfirmed (long UsrCod,const char Email[Cns_MAX_BYTES_EMAIL_ADDRESS + 1])
  {
   char StrConfirmed[1 + 1];

   DB_QuerySELECTString (StrConfirmed,1,"can not check if email is confirmed",
			 "SELECT Confirmed"
			  " FROM usr_emails"
			 " WHERE UsrCod=%ld"
			   " AND E_mail='%s'",
			 UsrCod,
			 Email);

   return StrConfirmed[0];
  }

/*****************************************************************************/
/********** Check if an email matches any of the confirmed emails ************/
/*****************************************************************************/

Exi_Exist_t Mai_DB_CheckIfEmailExistsConfirmed (const char *Email)
  {
   return
   DB_QueryEXISTS ("can not check if email already existed",
		   "SELECT EXISTS"
		   "(SELECT *"
		     " FROM usr_emails"
		    " WHERE E_mail='%s'"
		      " AND Confirmed='Y')",
		   Email);
  }

/*****************************************************************************/
/**** Check if an email matches any of the confirmed emails of other users ***/
/*****************************************************************************/

bool Mai_DB_CheckIfEmailBelongToAnotherUsr (long UsrCod,const char Email[Cns_MAX_BYTES_EMAIL_ADDRESS + 1])
  {
   return
   DB_QueryEXISTS ("can not check if email already existed",
		   "SELECT EXISTS"
		   "(SELECT *"
		     " FROM usr_emails"
		    " WHERE E_mail='%s'"
		      " AND Confirmed='Y'"
		      " AND UsrCod<>%ld)",
		   Email,
		   UsrCod) == Exi_EXISTS;
  }

/*****************************************************************************/
/**************** Remove not confirmed email for other users *****************/
/*****************************************************************************/

void Mai_DB_RemoveNotConfirmedEmailForOtherUsrs (long UsrCod,const char Email[Cns_MAX_BYTES_EMAIL_ADDRESS + 1])
  {
   DB_QueryDELETE ("can not remove not confirmed email for other users",
		   "DELETE FROM usr_emails"
		   " WHERE E_mail='%s'"
		     " AND Confirmed='N'"
		     " AND UsrCod<>%ld",
	           Email,
	           UsrCod);
  }

/*****************************************************************************/
/*************** Remove an old email address from database *******************/
/*****************************************************************************/

void Mai_DB_RemoveEmail (long UsrCod,const char Email[Cns_MAX_BYTES_EMAIL_ADDRESS + 1])
  {
   DB_QueryREPLACE ("can not remove an old email address",
		    "DELETE FROM usr_emails"
		    " WHERE UsrCod=%ld"
		      " AND E_mail='%s'",
                    UsrCod,
                    Email);
  }

/*****************************************************************************/
/**************************** Remove user's emails ***************************/
/*****************************************************************************/

void Mai_DB_RemoveUsrEmails (long UsrCod)
  {
   DB_QueryDELETE ("can not remove user's emails",
		   "DELETE FROM usr_emails"
		   " WHERE UsrCod=%ld",
		   UsrCod);
  }

/*****************************************************************************/
/************************* Insert mail key in database ***********************/
/*****************************************************************************/

void Mai_DB_InsertPendingEmail (const char Email[Cns_MAX_BYTES_EMAIL_ADDRESS + 1],
                                const char MailKey[Mai_LENGTH_EMAIL_CONFIRM_KEY + 1])
  {
   DB_QueryREPLACE ("can not create pending password",
		    "INSERT INTO usr_pending_emails"
		    " (UsrCod,E_mail,MailKey,DateAndTime)"
		    " VALUES"
		    " (%ld,'%s','%s',NOW())",
	            Gbl.Usrs.Me.UsrDat.UsrCod,
	            Email,
	            MailKey);
  }

/*****************************************************************************/
/******************** Get user's code and email from key *********************/
/*****************************************************************************/

Exi_Exist_t Mai_DB_GetPendingEmail (MYSQL_RES **mysql_res,
                                    const char MailKey[Mai_LENGTH_EMAIL_CONFIRM_KEY + 1])
  {
   return
   DB_QuerySELECT (mysql_res,"can not get user's code and email from key",
		   "SELECT UsrCod,"		// row[0]
			  "E_mail"		// row[1]
		    " FROM usr_pending_emails"
		   " WHERE MailKey='%s'",
		   MailKey) ? Exi_EXISTS :
			      Exi_DOES_NOT_EXIST;
  }

/*****************************************************************************/
/*************************** Remove pending email ****************************/
/*****************************************************************************/

void Mai_DB_RemovePendingEmailForOtherUsrs (long UsrCod,const char Email[Cns_MAX_BYTES_EMAIL_ADDRESS + 1])
  {
   DB_QueryDELETE ("can not remove pending email for other users",
		   "DELETE FROM usr_pending_emails"
		   " WHERE E_mail='%s'"
		     " AND UsrCod<>%ld",
	           Email,
	           UsrCod);
  }

/*****************************************************************************/
/*************************** Remove pending email ****************************/
/*****************************************************************************/

void Mai_DB_RemovePendingEmail (const char MailKey[Mai_LENGTH_EMAIL_CONFIRM_KEY + 1])
  {
   DB_QueryDELETE ("can not remove an email key",
		   "DELETE FROM usr_pending_emails"
		   " WHERE MailKey='%s'",
		   MailKey);
  }

/*****************************************************************************/
/************** Remove a given user from list of pending emails **************/
/*****************************************************************************/

void Mai_DB_RemoveUsrPendingEmails (long UsrCod)
  {
   DB_QueryDELETE ("can not remove pending user's emails",
		   "DELETE FROM usr_pending_emails"
		   " WHERE UsrCod=%ld",
		   UsrCod);
  }

/*****************************************************************************/
/**************** Remove expired pending emails from database ****************/
/*****************************************************************************/

void Mai_DB_RemoveExpiredPendingEmails (void)
  {
   DB_QueryDELETE ("can not remove old pending mail keys",
		   "DELETE LOW_PRIORITY FROM usr_pending_emails"
		   " WHERE DateAndTime<FROM_UNIXTIME(UNIX_TIMESTAMP()-%lu)",
                   Cfg_TIME_TO_DELETE_OLD_PENDING_EMAILS);
  }

/*****************************************************************************/
/*** Create temporary tables with all mail domains in users' emails table ****/
/*****************************************************************************/

void Mai_DB_CreateTmpTables (void)
  {
   DB_CreateTmpTable ("CREATE TEMPORARY TABLE T1 ENGINE=MEMORY"
		      " SELECT SUBSTRING_INDEX(E_mail,'@',-1) AS Domain,"
			      "COUNT(*) as N"
		        " FROM usr_emails"
		    " GROUP BY Domain");

   DB_CreateTmpTable ("CREATE TEMPORARY TABLE T2 ENGINE=MEMORY"
		      " SELECT *"
		        " FROM T1");
  }

/*****************************************************************************/
/************************** Create a new mail domain *************************/
/*****************************************************************************/

void Mai_DB_CreateMailDomain (const struct Mail *Mai)
  {
   DB_QueryINSERT ("can not create mail domain",
		   "INSERT INTO ntf_mail_domains"
		   " (Domain,Info)"
		   " VALUES"
		   " ('%s','%s')",
	           Mai->Domain,
	           Mai->Info);
  }

/*****************************************************************************/
/****************** Update name in table of mail domains *********************/
/*****************************************************************************/

void Mai_DB_UpdateMailDomainName (long MaiCod,
                                  const char *FldName,const char *NewMaiName)
  {
   DB_QueryUPDATE ("can not update the name of a mail domain",
		   "UPDATE ntf_mail_domains"
		     " SET %s='%s'"
		   " WHERE MaiCod=%ld",
	           FldName,NewMaiName,
	           MaiCod);
  }

/*****************************************************************************/
/****************************** Get mail domains *****************************/
/*****************************************************************************/

unsigned Mai_DB_GetMailDomains (MYSQL_RES **mysql_res,Mai_DomainsOrder_t SelectedOrder)
  {
   static const char *OrderBySubQuery[Mai_NUM_ORDERS] =
     {
      [Mai_ORDER_BY_DOMAIN] = "Domain,"
	                      "Info,"
	                      "N DESC",
      [Mai_ORDER_BY_INFO  ] = "Info,"
	                      "Domain,"
	                      "N DESC",
      [Mai_ORDER_BY_USERS ] = "N DESC,"
	                      "Info,"
	                      "Domain",
     };

   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get mail domains",
		   "(SELECT ntf_mail_domains.MaiCod,"		// row[0]
			   "ntf_mail_domains.Domain AS Domain,"	// row[1]
			   "ntf_mail_domains.Info AS Info,"	// row[2]
			   "T1.N AS N"				// row[3]
		     " FROM ntf_mail_domains,"
		           "T1"
		    " WHERE ntf_mail_domains.Domain=T1.Domain COLLATE 'latin1_bin')"
		    " UNION "
		   "(SELECT MaiCod,"				// row[0]
			   "Domain,"				// row[1]
			   "Info,"				// row[2]
			   "0 AS N"				// row[3]
		     " FROM ntf_mail_domains"
		    " WHERE Domain NOT IN"
			  " (SELECT Domain COLLATE 'latin1_bin'"
			     " FROM T2))"
		 " ORDER BY %s",	// COLLATE necessary to avoid error in comparisons
		   OrderBySubQuery[SelectedOrder]);
  }


/*****************************************************************************/
/**************************** Get mail domain data ***************************/
/*****************************************************************************/

Exi_Exist_t Mai_DB_GetMailDomainDataByCod (MYSQL_RES **mysql_res,long MaiCod)
  {
   return
   DB_QuerySELECT (mysql_res,"can not get mail domain",
		   "SELECT MaiCod,"	// row[0]
			  "Domain,"	// row[1]
			  "Info"	// row[2]
		    " FROM ntf_mail_domains"
		   " WHERE MaiCod=%ld",
		   MaiCod) ? Exi_EXISTS :
			     Exi_DOES_NOT_EXIST;
  }

/*****************************************************************************/
/********************** Check if the name of mail exists *********************/
/*****************************************************************************/

Exi_Exist_t Mai_DB_CheckIfMailDomainNameExists (const char *FldName,const char *Name,long MaiCod)
  {
   return
   DB_QueryEXISTS ("can not check if the name of a mail domain already existed",
		   "SELECT EXISTS"
		   "(SELECT *"
		     " FROM ntf_mail_domains"
		    " WHERE %s='%s'"
		      " AND MaiCod<>%ld)",
		   FldName,Name,
		   MaiCod);
  }

/*****************************************************************************/
/************ Check if a mail domain is allowed for notifications ************/
/*****************************************************************************/

bool Mai_DB_CheckIfMailDomainIsAllowedForNotif (const char MailDomain[Cns_MAX_BYTES_EMAIL_ADDRESS + 1])
  {
   return
   DB_QueryEXISTS ("can not check if a mail domain is allowed for notifications",
		   "SELECT EXISTS"
		   "(SELECT *"
		     " FROM ntf_mail_domains"
		    " WHERE Domain='%s')",
		   MailDomain) == Exi_EXISTS;
  }

/*****************************************************************************/
/***************************** Remove mail domain ****************************/
/*****************************************************************************/

void Mai_DB_RemoveMailDomain (long MaiCod)
  {
   DB_QueryDELETE ("can not remove a mail domain",
		   "DELETE FROM ntf_mail_domains"
		   " WHERE MaiCod=%ld",
		   MaiCod);
  }

/*****************************************************************************/
/*** Remove temporary tables with all mail domains in users' emails table ****/
/*****************************************************************************/

void Mai_DB_RemoveTmpTables (void)
  {
   DB_DropTmpTable ("T1");
   DB_DropTmpTable ("T2");
  }
