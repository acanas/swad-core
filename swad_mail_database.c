// swad_mail_database.c: all the stuff related to email, operations with database

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

// #include <stddef.h>		// For NULL
// #include <stdlib.h>		// For calloc
// #include <string.h>		// For string functions
// #include <sys/wait.h>		// For the macro WEXITSTATUS
// #include <unistd.h>		// For access, lstat, getpid, chdir, symlink, unlink

// #include "swad_account.h"
// #include "swad_box.h"
#include "swad_database.h"
// #include "swad_error.h"
// #include "swad_form.h"
#include "swad_global.h"
// #include "swad_HTML.h"
// #include "swad_language.h"
#include "swad_mail.h"
#include "swad_mail_database.h"
// #include "swad_parameter.h"
// #include "swad_QR.h"
// #include "swad_tab.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/***************************** Private constants *****************************/
/*****************************************************************************/

/*****************************************************************************/
/******************************* Private types *******************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private variables *****************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

/*****************************************************************************/
/* Create temporary tables with all the mail domains in users' emails table **/
/*****************************************************************************/

void Mai_DB_CreateTmpTables (void)
  {
   DB_Query ("can not create temporary table",
	     "CREATE TEMPORARY TABLE T1 ENGINE=MEMORY"
	     " SELECT SUBSTRING_INDEX(E_mail,'@',-1) AS Domain,"
	             "COUNT(*) as N"
    	       " FROM usr_emails"
    	      " GROUP BY Domain");

   DB_Query ("can not create temporary table",
	     "CREATE TEMPORARY TABLE T2 ENGINE=MEMORY"
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
                                  const char *FieldName,const char *NewMaiName)
  {
   DB_QueryUPDATE ("can not update the name of a mail domain",
		   "UPDATE ntf_mail_domains"
		     " SET %s='%s'"
		   " WHERE MaiCod=%ld",
	           FieldName,NewMaiName,
	           MaiCod);
  }

/*****************************************************************************/
/****************************** Get mail domains *****************************/
/*****************************************************************************/

unsigned Mai_DB_GetMailDomains (MYSQL_RES **mysql_res)
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
		   OrderBySubQuery[Gbl.Mails.SelectedOrder]);
  }


/*****************************************************************************/
/**************************** Get mail domain data ***************************/
/*****************************************************************************/

unsigned Mai_DB_GetDataOfMailDomainByCod (MYSQL_RES **mysql_res,long MaiCod)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get data of a mail domain",
		   "SELECT Domain,"	// row[0]
			  "Info"		// row[1]
		    " FROM ntf_mail_domains"
		   " WHERE MaiCod=%ld",
		   MaiCod);
  }

/*****************************************************************************/
/********************** Check if the name of mail exists *********************/
/*****************************************************************************/

bool Mai_DB_CheckIfMailDomainNameExists (const char *FieldName,const char *Name,long MaiCod)
  {
   /***** Get number of mail_domains with a name from database *****/
   return (DB_QueryCOUNT ("can not check if the name"
			  " of a mail domain already existed",
			  "SELECT COUNT(*)"
			   " FROM ntf_mail_domains"
			  " WHERE %s='%s'"
			    " AND MaiCod<>%ld",
			  FieldName,Name,
			  MaiCod) != 0);
  }

/*****************************************************************************/
/************ Check if a mail domain is allowed for notifications ************/
/*****************************************************************************/

bool Mai_DB_CheckIfMailDomainIsAllowedForNotif (const char MailDomain[Cns_MAX_BYTES_EMAIL_ADDRESS + 1])
  {
   /***** Get number of mail_domains with a name from database *****/
   return (DB_QueryCOUNT ("can not check if a mail domain"
			  " is allowed for notifications",
			  "SELECT COUNT(*)"
			   " FROM ntf_mail_domains"
			  " WHERE Domain='%s'",
			  MailDomain) != 0);
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
/* Remove temporary tables with all the mail domains in users' emails table **/
/*****************************************************************************/

void Mai_DB_RemoveTmpTables (void)
  {
   DB_Query ("can not remove temporary tables",
	     "DROP TEMPORARY TABLE IF EXISTS T1,"
	                                    "T2");
  }
