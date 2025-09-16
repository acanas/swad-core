// swad_user_database.c: users, operations with database

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2025 Antonio Cañas Vargas

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General 3 License as
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
/*********************************** Headers *********************************/
/*****************************************************************************/

#include <stdlib.h>		// For free

#include "swad_action_list.h"
#include "swad_database.h"
#include "swad_error.h"
#include "swad_global.h"
#include "swad_user_database.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/******************************* Update my office ****************************/
/*****************************************************************************/

void Usr_DB_UpdateMyOffice (void)
  {
   DB_QueryUPDATE ("can not update office",
		   "UPDATE usr_data"
		     " SET Office='%s'"
		   " WHERE UsrCod=%ld",
		   Gbl.Usrs.Me.UsrDat.Tch.Office,
		   Gbl.Usrs.Me.UsrDat.UsrCod);
  }

/*****************************************************************************/
/***************************** Update my office phone ************************/
/*****************************************************************************/

void Usr_DB_UpdateMyOfficePhone (void)
  {
   DB_QueryUPDATE ("can not update office phone",
		   "UPDATE usr_data"
		     " SET OfficePhone='%s'"
		   " WHERE UsrCod=%ld",
	           Gbl.Usrs.Me.UsrDat.Tch.OfficePhone,
	           Gbl.Usrs.Me.UsrDat.UsrCod);
  }

/*****************************************************************************/
/************** Check if a user exists with a given user's code **************/
/*****************************************************************************/

Exi_Exist_t Usr_DB_ChkIfUsrCodExists (long UsrCod)
  {
   /***** Trivial check: user's code should be > 0 *****/
   if (UsrCod <= 0)	// Wrong user's code
      return Exi_DOES_NOT_EXIST;

   /***** Check if a user exists in database *****/
   return
   DB_QueryEXISTS ("can not check if a user exists",
		   "SELECT EXISTS"
		   "(SELECT *"
		     " FROM usr_data"
		    " WHERE UsrCod=%ld)",
		   UsrCod);
  }

/*****************************************************************************/
/******** Get encrypted user's code from database using user's code **********/
/*****************************************************************************/
// Input: UsrDat->UsrCod must hold user's code

void Usr_DB_GetEncryptedUsrCodFromUsrCod (long UsrCod,
					  char EncryptedUsrCod[Cry_BYTES_ENCRYPTED_STR_SHA256_BASE64 + 1])
  {
   DB_QuerySELECTString (EncryptedUsrCod,Cry_BYTES_ENCRYPTED_STR_SHA256_BASE64,
			 "can not get encrypted user's code",
		         "SELECT EncryptedUsrCod"
		         " FROM usr_data"
		         " WHERE UsrCod='%ld'",
		         UsrCod);
  }

/*****************************************************************************/
/******** Get user's code from database using encrypted user's code **********/
/*****************************************************************************/
// Input: UsrDat->EncryptedUsrCod must hold user's encrypted code

long Usr_DB_GetUsrCodFromEncryptedUsrCod (const char EncryptedUsrCod[Cry_BYTES_ENCRYPTED_STR_SHA256_BASE64 + 1])
  {
   return
   DB_QuerySELECTCode ("can not get user's code",
		       "SELECT UsrCod"
		        " FROM usr_data"
		       " WHERE EncryptedUsrCod='%s'",
		       EncryptedUsrCod);
  }

/*****************************************************************************/
/***************** Get user's code from database using nickname **************/
/*****************************************************************************/

long Usr_DB_GetUsrCodFromNick (const char *NickWithoutArr)
  {
   return
   DB_QuerySELECTCode ("can not get user's code",
		       "SELECT UsrCod"
		        " FROM usr_nicknames"
		       " WHERE Nickname='%s'",
		       NickWithoutArr);
  }

/*****************************************************************************/
/******** Get user's code from database using nickname and password **********/
/*****************************************************************************/

long Usr_DB_GetUsrCodFromNickPwd (const char *NickWithoutArr,const char *Password)
  {
   return
   DB_QuerySELECTCode ("can not get user's code",
		       "SELECT usr_nicknames.UsrCod"
		        " FROM usr_nicknames,"
			      "usr_data"
		       " WHERE usr_nicknames.Nickname='%s'"
		         " AND usr_nicknames.UsrCod=usr_data.UsrCod"
		         " AND usr_data.Password='%s'",
		       NickWithoutArr,
		       Password);
  }

/*****************************************************************************/
/**************** Get user's code from database using email ******************/
/*****************************************************************************/

long Usr_DB_GetUsrCodFromEmail (const char *Email)
  {
   return
   DB_QuerySELECTCode ("can not get user's code",
		       "SELECT UsrCod"
		        " FROM usr_emails"
		       " WHERE E_mail='%s'",	// TODO: Get only if email confirmed?
		       Email);
  }

/*****************************************************************************/
/********** Get user's code from database using email and password ***********/
/*****************************************************************************/

long Usr_DB_GetUsrCodFromEmailPwd (const char *Email,const char *Password)
  {
   return
   DB_QuerySELECTCode ("can not get user's code",
		       "SELECT usr_emails.UsrCod"
		        " FROM usr_emails,"
		              "usr_data"
		       " WHERE usr_emails.E_mail='%s'"	// TODO: Get only if email confirmed?
			 " AND usr_emails.UsrCod=usr_data.UsrCod"
			 " AND usr_data.Password='%s'",
		       Email,
		       Password);
  }

/*****************************************************************************/
/**************** Get user's code from database using ID *********************/
/*****************************************************************************/

long Usr_DB_GetUsrCodFromID (const char *ID)
  {
   return
   DB_QuerySELECTCode ("can not get user's code",
		       "SELECT UsrCod"
		        " FROM usr_ids"
		       " WHERE UsrID='%s'",	// TODO: Get only if ID confirmed?
		       ID);
  }

/*****************************************************************************/
/*********** Get user's code from database using ID and password *************/
/*****************************************************************************/

long Usr_DB_GetUsrCodFromIDPwd (const char *ID,const char *Password)
  {
   return
   DB_QuerySELECTCode ("can not get user's code",
		       "SELECT usr_ids.UsrCod"
		        " FROM usr_ids,"
			      "usr_data"
		       " WHERE usr_ids.UsrID='%s'"	// TODO: Get only if ID confirmed?
		         " AND usr_ids.UsrCod=usr_data.UsrCod"
		         " AND usr_data.Password='%s'",
		       ID,
		       Password);
  }

/*****************************************************************************/
/************ Get user's data from database giving a user's code *************/
/*****************************************************************************/
// UsrDat->UsrCod must contain an existing user's code

Exi_Exist_t Usr_DB_GetUsrDataFromUsrCod (MYSQL_RES **mysql_res,long UsrCod,
                                         Usr_GetPrefs_t GetPrefs)
  {
   switch (GetPrefs)
     {
      case Usr_DONT_GET_PREFS:
	 return
	 DB_QuerySELECTunique (mysql_res,"can not get user's data",
			       "SELECT EncryptedUsrCod,"	// row[ 0]
				      "Password,"		// row[ 1]
				      "Surname1,"		// row[ 2]
				      "Surname2,"		// row[ 3]
				      "FirstName,"		// row[ 4]
				      "Sex,"			// row[ 5]
				      "Photo,"			// row[ 6]
				      "PhotoVisibility,"	// row[ 7]
				      "BaPrfVisibility,"	// row[ 8]
				      "ExPrfVisibility,"	// row[ 9]
				      "CtyCod,"			// row[10]
				      "InsCtyCod,"		// row[11]
				      "InsCod,"			// row[12]
				      "DptCod,"			// row[13]
				      "CtrCod,"			// row[14]
				      "Office,"			// row[15]
				      "OfficePhone,"		// row[16]
				      "LocalPhone,"		// row[17]
				      "FamilyPhone,"		// row[18]
				      "DATE_FORMAT(Birthday,"
				      "'%%Y%%m%%d'),"		// row[19]
				      "Comments,"		// row[20]
				      "NotifNtfEvents,"		// row[21]
				      "EmailNtfEvents"		// row[22]
				" FROM usr_data"
			       " WHERE UsrCod=%ld",
			       UsrCod);
      case Usr_GET_PREFS:
      default:
	 return
	 DB_QuerySELECTunique (mysql_res,"can not get user's data",
			       "SELECT EncryptedUsrCod,"	// row[ 0]
				      "Password,"		// row[ 1]
				      "Surname1,"		// row[ 2]
				      "Surname2,"		// row[ 3]
				      "FirstName,"		// row[ 4]
				      "Sex,"			// row[ 5]
				      "Photo,"			// row[ 6]
				      "PhotoVisibility,"	// row[ 7]
				      "BaPrfVisibility,"	// row[ 8]
				      "ExPrfVisibility,"	// row[ 9]
				      "CtyCod,"			// row[10]
				      "InsCtyCod,"		// row[11]
				      "InsCod,"			// row[12]
				      "DptCod,"			// row[13]
				      "CtrCod,"			// row[14]
				      "Office,"			// row[15]
				      "OfficePhone,"		// row[16]
				      "LocalPhone,"		// row[17]
				      "FamilyPhone,"		// row[18]
				      "DATE_FORMAT(Birthday,"
				      "'%%Y%%m%%d'),"		// row[19]
				      "Comments,"		// row[20]
				      "NotifNtfEvents,"		// row[21]
				      "EmailNtfEvents,"		// row[22]

				      // Settings (usually not necessary
				      // when getting another user's data)
				      "Language,"		// row[23]
				      "FirstDayOfWeek,"		// row[24]
				      "DateFormat,"		// row[25]
				      "Theme,"			// row[26]
				      "IconSet,"		// row[27]
				      "Menu,"			// row[28]
				      "SideCols,"		// row[29]
				      "PhotoShape,"		// row[30]
				      "ThirdPartyCookies"	// row[31]
				" FROM usr_data"
			       " WHERE UsrCod=%ld",
			       UsrCod);
     }
  }

/*****************************************************************************/
/********** Get some user's data from database giving a user's code **********/
/*****************************************************************************/
// UsrDat->UsrCod must contain an existing user's code

Exi_Exist_t Usr_DB_GetSomeUsrDataFromUsrCod (MYSQL_RES **mysql_res,long UsrCod)
  {
   return
   DB_QuerySELECTunique (mysql_res,"can not get user's data",
			 "SELECT Surname1,"				// row[0]
				"Surname2,"				// row[1]
				"FirstName,"				// row[2]
				"Photo,"				// row[3]
				"DATE_FORMAT(Birthday,'%%Y%%m%%d')"	// row[4]
			  " FROM usr_data"
			 " WHERE UsrCod=%ld",
			 UsrCod);
  }

/*****************************************************************************/
/************************ Get user name from database ************************/
/*****************************************************************************/
// UsrDat->UsrCod must contain an existing user's code

void Usr_DB_GetUsrName (long UsrCod,char *Title,size_t TitleSize)
  {
   DB_QuerySELECTString (Title,TitleSize,"can not get user name",
		         "SELECT CONCAT(usr_data.FirstName,' ',"
				       "usr_data.Surname1,' ',"
				       "usr_data.Surname2)"
			  " FROM usr_data"
		         " WHERE UsrCod=%ld",
		         UsrCod);
  }

/*****************************************************************************/
/****** Check if a string is found in first name or surnames of anybody ******/
/*****************************************************************************/

bool Usr_DB_FindStrInUsrsNames (const char *Str)
  {
   return
   DB_QueryEXISTS ("can not check if a string matches a first name or a surname",
		   "SELECT EXISTS"
		   "(SELECT *"
		     " FROM usr_data"
		    " WHERE FirstName='%s'"
		       " OR Surname1='%s'"
		       " OR Surname2='%s')",
		   Str,
		   Str,
		   Str) == Exi_EXISTS;
  }

/*****************************************************************************/
/*********** Get list of users with a given role in a given scope ************/
/*****************************************************************************/
// Role can be:
// - Rol_STD	Student
// - Rol_NET	Non-editing teacher
// - Rol_TCH	Teacher

void Usr_DB_BuildQueryToGetUsrsLst (Hie_Level_t HieLvl,Rol_Role_t Role,
				    char **Query)
  {
   const char *QueryFields =
      "DISTINCT "
      "usr_data.UsrCod,"		// row[ 0]
      "usr_data.EncryptedUsrCod,"	// row[ 1]
      "usr_data.Password,"		// row[ 2]
      "usr_data.Surname1,"		// row[ 3]
      "usr_data.Surname2,"		// row[ 4]
      "usr_data.FirstName,"		// row[ 5]
      "usr_data.Sex,"			// row[ 6]
      "usr_data.Photo,"			// row[ 7]
      "usr_data.PhotoVisibility,"	// row[ 8]
      "usr_data.CtyCod,"		// row[ 9]
      "usr_data.InsCod";		// row[10]
   static const char *OrderBySubQuery =
      " ORDER BY usr_data.Surname1,"
		"usr_data.Surname2,"
		"usr_data.FirstName,"
		"usr_data.UsrCod";

   /***** Build query *****/
   switch (HieLvl)
     {
      case Hie_SYS:
	 /* Get users in courses from the whole platform */
	 DB_BuildQuery (Query,
		        "SELECT %s"
		         " FROM usr_data,"
		               "crs_users"
		        " WHERE usr_data.UsrCod=crs_users.UsrCod"
		          " AND crs_users.Role=%u"
		           " %s",
		        QueryFields,
		        (unsigned) Role,
		        OrderBySubQuery);
	 break;
      case Hie_CTY:
	 /* Get users in courses from the current country */
	 DB_BuildQuery (Query,
			"SELECT %s"
			 " FROM usr_data,"
			       "crs_users,"
			       "crs_courses,"
			       "deg_degrees,"
			       "ctr_centers,"
			       "ins_instits"
			" WHERE usr_data.UsrCod=crs_users.UsrCod"
			  " AND crs_users.Role=%u"
			  " AND crs_users.CrsCod=crs_courses.CrsCod"
			  " AND crs_courses.DegCod=deg_degrees.DegCod"
			  " AND deg_degrees.CtrCod=ctr_centers.CtrCod"
			  " AND ctr_centers.InsCod=ins_instits.InsCod"
			  " AND ins_instits.CtyCod=%ld"
			   " %s",
			QueryFields,
			(unsigned) Role,
			Gbl.Hierarchy.Node[Hie_CTY].HieCod,
			OrderBySubQuery);
	 break;
      case Hie_INS:
	 /* Get users in courses from the current institution */
	 DB_BuildQuery (Query,
			"SELECT %s"
			 " FROM usr_data,"
			       "crs_users,"
			       "crs_courses,"
			       "deg_degrees,"
			       "ctr_centers"
			" WHERE usr_data.UsrCod=crs_users.UsrCod"
			  " AND crs_users.Role=%u"
			  " AND crs_users.CrsCod=crs_courses.CrsCod"
			  " AND crs_courses.DegCod=deg_degrees.DegCod"
			  " AND deg_degrees.CtrCod=ctr_centers.CtrCod"
			  " AND ctr_centers.InsCod=%ld"
			   " %s",
			QueryFields,
			(unsigned) Role,
			Gbl.Hierarchy.Node[Hie_INS].HieCod,
			OrderBySubQuery);
	 break;
      case Hie_CTR:
	 /* Get users in courses from the current center */
	 DB_BuildQuery (Query,
			"SELECT %s"
			 " FROM usr_data,"
			       "crs_users,"
			       "crs_courses,"
			       "deg_degrees"
			" WHERE usr_data.UsrCod=crs_users.UsrCod"
			  " AND crs_users.Role=%u"
			  " AND crs_users.CrsCod=crs_courses.CrsCod"
			  " AND crs_courses.DegCod=deg_degrees.DegCod"
			  " AND deg_degrees.CtrCod=%ld"
		     " ORDER BY usr_data.Surname1,"
			       "usr_data.Surname2,"
			       "usr_data.FirstName,"
			       "usr_data.UsrCod",
			QueryFields,
			(unsigned) Role,
			Gbl.Hierarchy.Node[Hie_CTR].HieCod);
	 break;
      case Hie_DEG:
	 /* Get users in courses from the current degree */
	 DB_BuildQuery (Query,
			"SELECT %s"
		 	 " FROM usr_data,"
			       "crs_users,"
			       "crs_courses"
			" WHERE usr_data.UsrCod=crs_users.UsrCod"
			  " AND crs_users.Role=%u"
			  " AND crs_users.CrsCod=crs_courses.CrsCod"
			  " AND crs_courses.DegCod=%ld"
		     " ORDER BY usr_data.Surname1,"
			       "usr_data.Surname2,"
			       "usr_data.FirstName,"
			       "usr_data.UsrCod",
			QueryFields,
			(unsigned) Role,
			Gbl.Hierarchy.Node[Hie_DEG].HieCod);
	 break;
      case Hie_CRS:
	 /* Get users from the current course */
	 Usr_DB_BuildQueryToGetUsrsLstCrs (Query,Role);
	 break;
      default:
	 Err_WrongHierarchyLevelExit ();
	 break;
     }
/*
   if (Gbl.Usrs.Me.Roles.LoggedRole == Rol_SYS_ADM)
      Lay_ShowAlert (Lay_INFO,Query);
*/
  }

/*****************************************************************************/
/******* Build query to get list with data of users in current course ********/
/*****************************************************************************/

#define Usr_DB_MAX_BYTES_QUERY_GET_LIST_USRS (16 * 1024 - 1)

void Usr_DB_BuildQueryToGetUsrsLstCrs (char **Query,Rol_Role_t Role)
  {
   unsigned NumPositiveCods = 0;
   unsigned NumNegativeCods = 0;
   char LongStr[Cns_MAX_DIGITS_LONG + 1];
   unsigned NumGrpSel;
   long GrpCod;
   unsigned NumGrpTyp;
   bool *AddStdsWithoutGroupOf;
   const char *QueryFields =
      "usr_data.UsrCod,"
      "usr_data.EncryptedUsrCod,"
      "usr_data.Password,"
      "usr_data.Surname1,"
      "usr_data.Surname2,"
      "usr_data.FirstName,"
      "usr_data.Sex,"
      "usr_data.Photo,"
      "usr_data.PhotoVisibility,"
      "usr_data.CtyCod,"
      "usr_data.InsCod,"
      "crs_users.Role,"
      "crs_users.Accepted";
   /*
   row[ 0]: usr_data.UsrCod
   row[ 1]: usr_data.EncryptedUsrCod
   row[ 2]: usr_data.Password (used to check if a teacher can edit user's data)
   row[ 3]: usr_data.Surname1
   row[ 4]: usr_data.Surname2
   row[ 5]: usr_data.FirstName
   row[ 6]: usr_data.Sex
   row[ 7]: usr_data.Photo
   row[ 8]: usr_data.PhotoVisibility
   row[ 9]: usr_data.CtyCod
   row[10]: usr_data.InsCod
   row[11]: crs_users.Role	(only if Scope == Hie_CRS)
   row[12]: crs_users.Accepted	(only if Scope == Hie_CRS)
   */

   /***** If there are no groups selected, don't do anything *****/
   if (!Gbl.Crs.Grps.AllGrpsSel &&
       !Gbl.Crs.Grps.LstGrpsSel.NumGrps)
     {
      *Query = NULL;
      return;
     }

   /***** Allocate space for query *****/
   if ((*Query = malloc (Usr_DB_MAX_BYTES_QUERY_GET_LIST_USRS + 1)) == NULL)
      Err_NotEnoughMemoryExit ();

   /***** Create query for users in the course *****/
   if (Gbl.Action.Act == ActReqMsgUsr)        // Selecting users to write a message
      snprintf (*Query,Usr_DB_MAX_BYTES_QUERY_GET_LIST_USRS + 1,
	        "SELECT %s"
	         " FROM crs_users,"
	               "usr_data"
	        " WHERE crs_users.CrsCod=%ld"
	          " AND crs_users.Role=%u"
	          " AND crs_users.UsrCod NOT IN"
	              " (SELECT ToUsrCod"
	                 " FROM msg_banned"
	                " WHERE FromUsrCod=%ld)"
	          " AND crs_users.UsrCod=usr_data.UsrCod",        // Do not get banned users
      	        QueryFields,
                Gbl.Hierarchy.Node[Hie_CRS].HieCod,
                (unsigned) Role,
                Gbl.Usrs.Me.UsrDat.UsrCod);
   else
      snprintf (*Query,Usr_DB_MAX_BYTES_QUERY_GET_LIST_USRS + 1,
	        "SELECT %s"
	         " FROM crs_users,"
	               "usr_data"
	        " WHERE crs_users.CrsCod=%ld"
	          " AND crs_users.Role=%u"
	          " AND crs_users.UsrCod=usr_data.UsrCod",
	        QueryFields,
                Gbl.Hierarchy.Node[Hie_CRS].HieCod,
                (unsigned) Role);

   /***** Select users in selected groups *****/
   if (!Gbl.Crs.Grps.AllGrpsSel)
     {
      /***** Get list of groups types in current course *****/
      Grp_GetListGrpTypesInCurrentCrs (Grp_GRP_TYPES_WITH_GROUPS);

      /***** Allocate memory for list of booleans AddStdsWithoutGroupOf *****/
      if ((AddStdsWithoutGroupOf = calloc (Gbl.Crs.Grps.GrpTypes.NumGrpTypes,
                                           sizeof (*AddStdsWithoutGroupOf))) == NULL)
         Err_NotEnoughMemoryExit ();

      /***** Initialize vector of booleans that indicates whether it's necessary add to the list
             the students who don't belong to any group of each type *****/
      for (NumGrpTyp = 0;
           NumGrpTyp < Gbl.Crs.Grps.GrpTypes.NumGrpTypes;
           NumGrpTyp++)
         AddStdsWithoutGroupOf[NumGrpTyp] = false;

      /***** Create query with the students who belong to the groups selected *****/
      if (Gbl.Crs.Grps.LstGrpsSel.NumGrps)        // If there are groups selected...
        {
         /* Check if there are positive and negative codes in the list */
         for (NumGrpSel = 0;
              NumGrpSel < Gbl.Crs.Grps.LstGrpsSel.NumGrps;
              NumGrpSel++)
            if ((GrpCod = Gbl.Crs.Grps.LstGrpsSel.GrpCods[NumGrpSel]) > 0)
               NumPositiveCods++;
            else
               for (NumGrpTyp = 0;
                    NumGrpTyp < Gbl.Crs.Grps.GrpTypes.NumGrpTypes;
                    NumGrpTyp++)
                  if (Gbl.Crs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp].GrpTypCod == -GrpCod)
                    {
                     AddStdsWithoutGroupOf[NumGrpTyp] = true;
                     break;
                    }
         /* If there are positive codes, add the students who belong to groups with those codes */
         if (NumPositiveCods)
           {
            Str_Concat (*Query," AND (crs_users.UsrCod IN"
			       " (SELECT DISTINCT "
			                "UsrCod"
			          " FROM grp_users"
			         " WHERE",
                        Usr_DB_MAX_BYTES_QUERY_GET_LIST_USRS);
            NumPositiveCods = 0;
            for (NumGrpSel = 0;
                 NumGrpSel < Gbl.Crs.Grps.LstGrpsSel.NumGrps;
                 NumGrpSel++)
               if ((GrpCod = Gbl.Crs.Grps.LstGrpsSel.GrpCods[NumGrpSel]) > 0)
                 {
                  Str_Concat (*Query,NumPositiveCods ? " OR GrpCod='" :
                				       " GrpCod='",
                	      Usr_DB_MAX_BYTES_QUERY_GET_LIST_USRS);
                  snprintf (LongStr,sizeof (LongStr),"%ld",GrpCod);
                  Str_Concat (*Query,LongStr,Usr_DB_MAX_BYTES_QUERY_GET_LIST_USRS);
                  Str_Concat (*Query,"'",Usr_DB_MAX_BYTES_QUERY_GET_LIST_USRS);
                  NumPositiveCods++;
                 }
            Str_Concat (*Query,")",Usr_DB_MAX_BYTES_QUERY_GET_LIST_USRS);
           }
        }

      /***** Create a query with the students who don't belong to any group *****/
      for (NumGrpTyp = 0;
           NumGrpTyp < Gbl.Crs.Grps.GrpTypes.NumGrpTypes;
           NumGrpTyp++)
         if (AddStdsWithoutGroupOf[NumGrpTyp])
           {
            if (NumPositiveCods || NumNegativeCods)
               Str_Concat (*Query," OR ",Usr_DB_MAX_BYTES_QUERY_GET_LIST_USRS);
            else
               Str_Concat (*Query," AND (",Usr_DB_MAX_BYTES_QUERY_GET_LIST_USRS);
            /* Select all students of the course who don't belong to any group of type GrpTypCod */
            Str_Concat (*Query,"crs_users.UsrCod NOT IN"
			       " (SELECT DISTINCT "
			                "grp_users.UsrCod"
			          " FROM grp_groups,"
			                "grp_users"
			         " WHERE grp_groups.GrpTypCod='",
                        Usr_DB_MAX_BYTES_QUERY_GET_LIST_USRS);
            snprintf (LongStr,sizeof (LongStr),"%ld",
		      Gbl.Crs.Grps.GrpTypes.LstGrpTypes[NumGrpTyp].GrpTypCod);
            Str_Concat (*Query,LongStr,Usr_DB_MAX_BYTES_QUERY_GET_LIST_USRS);
            Str_Concat (*Query,"' AND grp_groups.GrpCod=grp_users.GrpCod)",
                        Usr_DB_MAX_BYTES_QUERY_GET_LIST_USRS);
            NumNegativeCods++;
           }
      if (NumPositiveCods ||
          NumNegativeCods)
         Str_Concat (*Query,")",Usr_DB_MAX_BYTES_QUERY_GET_LIST_USRS);

      /***** Free memory used by the list of booleans AddStdsWithoutGroupOf *****/
      free (AddStdsWithoutGroupOf);

      /***** Free list of groups types in current course *****/
      Grp_FreeListGrpTypesAndGrps ();
     }

   /***** The last part of the query is for ordering the list *****/
   Str_Concat (*Query," ORDER BY usr_data.Surname1,"
		                "usr_data.Surname2,"
		                "usr_data.FirstName,"
		                "usr_data.UsrCod",
	       Usr_DB_MAX_BYTES_QUERY_GET_LIST_USRS);
  }

/*****************************************************************************/
/************ Build query to get list with data of administrators ************/
/*****************************************************************************/

void Usr_DB_BuildQueryToGetAdmsLst (Hie_Level_t HieLvl,char **Query)
  {
   static const char *QueryFields =
      "UsrCod,"			// row[ 0]
      "EncryptedUsrCod,"	// row[ 1]
      "Password,"		// row[ 2]
      "Surname1,"		// row[ 3]
      "Surname2,"		// row[ 4]
      "FirstName,"		// row[ 5]
      "Sex,"			// row[ 6]
      "Photo,"			// row[ 7]
      "PhotoVisibility,"	// row[ 8]
      "CtyCod,"			// row[ 9]
      "InsCod";			// row[10]
   static const char *OrderBySubQuery =
      " ORDER BY Surname1,"
		"Surname2,"
		"FirstName,"
		"UsrCod";

   /***** Build query *****/
   // Important: it is better to use:
   // SELECT... WHERE UsrCod IN (SELECT...) OR UsrCod IN (SELECT...) <-- fast
   // instead of using or with different joins:
   // SELECT... WHERE (...) OR (...) <-- very slow
   switch (HieLvl)
     {
      case Hie_SYS:	// All admins
	 DB_BuildQuery (Query,
			"SELECT %s"
			 " FROM usr_data"
			" WHERE UsrCod IN "
			       "(SELECT DISTINCT "
			               "UsrCod"
				 " FROM usr_admins)"
			    "%s",
			QueryFields,
			OrderBySubQuery);
         break;
      case Hie_CTY:	// System admins
			// and admins of the institutions, centers and degrees in the current country
         DB_BuildQuery (Query,
                        "SELECT %s"
                         " FROM usr_data"
			" WHERE UsrCod IN "
			       "(SELECT UsrCod"
			         " FROM usr_admins"
			        " WHERE Scope='%s')"
			   " OR UsrCod IN "
			       "(SELECT usr_admins.UsrCod"
			         " FROM usr_admins,"
				       "ins_instits"
				" WHERE usr_admins.Scope='%s'"
			          " AND usr_admins.Cod=ins_instits.InsCod"
			          " AND ins_instits.CtyCod=%ld)"
			   " OR UsrCod IN "
			"(SELECT usr_admins.UsrCod"
			  " FROM usr_admins,"
			        "ctr_centers,"
			        "ins_instits"
			 " WHERE usr_admins.Scope='%s'"
			   " AND usr_admins.Cod=ctr_centers.CtrCod"
			   " AND ctr_centers.InsCod=ins_instits.InsCod"
			   " AND ins_instits.CtyCod=%ld)"
			    " OR UsrCod IN "
				"(SELECT usr_admins.UsrCod"
				  " FROM usr_admins,"
				        "deg_degrees,"
					"ctr_centers,"
					"ins_instits"
				 " WHERE usr_admins.Scope='%s'"
				   " AND usr_admins.Cod=deg_degrees.DegCod"
				   " AND deg_degrees.CtrCod=ctr_centers.CtrCod"
				   " AND ctr_centers.InsCod=ins_instits.InsCod"
				   " AND ins_instits.CtyCod=%ld)"
			    " %s",
			QueryFields,
			Hie_GetDBStrFromLevel (Hie_SYS),
			Hie_GetDBStrFromLevel (Hie_INS),Gbl.Hierarchy.Node[Hie_CTY].HieCod,
			Hie_GetDBStrFromLevel (Hie_CTR),Gbl.Hierarchy.Node[Hie_CTY].HieCod,
			Hie_GetDBStrFromLevel (Hie_DEG),Gbl.Hierarchy.Node[Hie_CTY].HieCod,
			OrderBySubQuery);
         break;
      case Hie_INS:	// System admins,
			// admins of the current institution,
			// and admins of the centers and degrees in the current institution
         DB_BuildQuery (Query,
                        "SELECT %s"
                         " FROM usr_data"
			" WHERE UsrCod IN "
			       "(SELECT UsrCod"
			         " FROM usr_admins"
			        " WHERE Scope='%s')"
			   " OR UsrCod IN "
			       "(SELECT UsrCod"
			         " FROM usr_admins"
			        " WHERE Scope='%s'"
			          " AND Cod=%ld)"
			   " OR UsrCod IN "
			       "(SELECT usr_admins.UsrCod"
			         " FROM usr_admins,"
				       "ctr_centers"
			        " WHERE usr_admins.Scope='%s'"
			          " AND usr_admins.Cod=ctr_centers.CtrCod"
			          " AND ctr_centers.InsCod=%ld)"
			   " OR UsrCod IN "
			       "(SELECT usr_admins.UsrCod"
			         " FROM usr_admins,"
				       "deg_degrees,"
				       "ctr_centers"
			        " WHERE usr_admins.Scope='%s'"
			          " AND usr_admins.Cod=deg_degrees.DegCod"
			          " AND deg_degrees.CtrCod=ctr_centers.CtrCod"
			          " AND ctr_centers.InsCod=%ld)"
			    "%s",
			QueryFields,
			Hie_GetDBStrFromLevel (Hie_SYS),
			Hie_GetDBStrFromLevel (Hie_INS),Gbl.Hierarchy.Node[Hie_INS].HieCod,
			Hie_GetDBStrFromLevel (Hie_CTR),Gbl.Hierarchy.Node[Hie_INS].HieCod,
			Hie_GetDBStrFromLevel (Hie_DEG),Gbl.Hierarchy.Node[Hie_INS].HieCod,
			OrderBySubQuery);
         break;
      case Hie_CTR:	// System admins,
			// admins of the current institution,
			// admins and the current center,
			// and admins of the degrees in the current center
	 DB_BuildQuery (Query,
			"SELECT %s"
			 " FROM usr_data"
			" WHERE UsrCod IN "
			       "(SELECT UsrCod"
			         " FROM usr_admins"
			        " WHERE Scope='%s')"
			   " OR UsrCod IN "
			       "(SELECT UsrCod"
			         " FROM usr_admins"
			        " WHERE Scope='%s'"
			          " AND Cod=%ld)"
			   " OR UsrCod IN "
			       "(SELECT UsrCod"
			         " FROM usr_admins"
			        " WHERE Scope='%s'"
			          " AND Cod=%ld)"
			   " OR UsrCod IN "
			       "(SELECT usr_admins.UsrCod"
			        " FROM usr_admins,"
				      "deg_degrees"
			       " WHERE usr_admins.Scope='%s'"
			         " AND usr_admins.Cod=deg_degrees.DegCod"
			         " AND deg_degrees.CtrCod=%ld)"
			    "%s",
			QueryFields,
			Hie_GetDBStrFromLevel (Hie_SYS),
			Hie_GetDBStrFromLevel (Hie_INS),Gbl.Hierarchy.Node[Hie_INS].HieCod,
			Hie_GetDBStrFromLevel (Hie_CTR),Gbl.Hierarchy.Node[Hie_CTR].HieCod,
			Hie_GetDBStrFromLevel (Hie_DEG),Gbl.Hierarchy.Node[Hie_CTR].HieCod,
			OrderBySubQuery);
         break;
      case Hie_DEG:	// System admins
			// and admins of the current institution, center or degree
         DB_BuildQuery (Query,
			"SELECT %s"
			 " FROM usr_data"
			" WHERE UsrCod IN "
			       "(SELECT UsrCod"
			         " FROM usr_admins"
			        " WHERE Scope='%s')"
			   " OR UsrCod IN "
			       "(SELECT UsrCod"
			         " FROM usr_admins"
			        " WHERE Scope='%s'"
			          " AND Cod=%ld)"
			   " OR UsrCod IN "
			       "(SELECT UsrCod"
			         " FROM usr_admins"
			        " WHERE Scope='%s'"
			          " AND Cod=%ld)"
			   " OR UsrCod IN "
			       "(SELECT UsrCod"
			         " FROM usr_admins"
			        " WHERE Scope='%s'"
			          " AND Cod=%ld)"
			    "%s",
			QueryFields,
			Hie_GetDBStrFromLevel (Hie_SYS),
			Hie_GetDBStrFromLevel (Hie_INS),Gbl.Hierarchy.Node[Hie_INS].HieCod,
			Hie_GetDBStrFromLevel (Hie_CTR),Gbl.Hierarchy.Node[Hie_CTR].HieCod,
			Hie_GetDBStrFromLevel (Hie_DEG),Gbl.Hierarchy.Node[Hie_DEG].HieCod,
			OrderBySubQuery);
         break;
      default:        // not aplicable
	 Err_WrongHierarchyLevelExit ();
         break;
     }
  }

/*****************************************************************************/
/************************ Get list with data of guests ***********************/
/*****************************************************************************/

void Usr_DB_BuildQueryToGetGstsLst (Hie_Level_t HieLvl,char **Query)
  {
   static const char *QueryFields =
      "UsrCod,"			// row[ 0]
      "EncryptedUsrCod,"	// row[ 1]
      "Password,"		// row[ 2]
      "Surname1,"		// row[ 3]
      "Surname2,"		// row[ 4]
      "FirstName,"		// row[ 5]
      "Sex,"			// row[ 6]
      "Photo,"			// row[ 7]
      "PhotoVisibility,"	// row[ 8]
      "CtyCod,"			// row[ 9]
      "InsCod";			// row[10]
   static const char *OrderBySubQuery =
      " ORDER BY Surname1,"
		"Surname2,"
		"FirstName,"
		"UsrCod";

   /***** Build query *****/
   switch (HieLvl)
     {
      case Hie_SYS:
	 DB_BuildQuery (Query,
         		"SELECT %s"
         	 	 " FROM usr_data"
			" WHERE UsrCod NOT IN"
			      " (SELECT UsrCod"
			         " FROM crs_users)"
			    "%s",
			QueryFields,
			OrderBySubQuery);
         break;
      case Hie_CTY:
	 DB_BuildQuery (Query,
			"SELECT %s"
			 " FROM usr_data"
			" WHERE (CtyCod=%ld"
			       " OR"
			       " InsCtyCod=%ld)"
			  " AND UsrCod NOT IN"
			      " (SELECT UsrCod"
			         " FROM crs_users)"
			    "%s",
			QueryFields,
			Gbl.Hierarchy.Node[Hie_CTY].HieCod,
			Gbl.Hierarchy.Node[Hie_CTY].HieCod,
			OrderBySubQuery);
         break;
      case Hie_INS:
	 DB_BuildQuery (Query,
			"SELECT %s"
			 " FROM usr_data"
			" WHERE InsCod=%ld"
			  " AND UsrCod NOT IN"
			      " (SELECT UsrCod"
			         " FROM crs_users)"
			    "%s",
			QueryFields,
			Gbl.Hierarchy.Node[Hie_INS].HieCod,
			OrderBySubQuery);
         break;
      case Hie_CTR:
	 DB_BuildQuery (Query,
			"SELECT %s"
			 " FROM usr_data"
			" WHERE CtrCod=%ld"
			  " AND UsrCod NOT IN"
			      " (SELECT UsrCod"
			         " FROM crs_users)"
			    "%s",
			QueryFields,
			Gbl.Hierarchy.Node[Hie_CTR].HieCod,
			OrderBySubQuery);
         break;
      default:        // not aplicable
	 Err_WrongHierarchyLevelExit ();
	 break;	// Not reached
     }
  }

/*****************************************************************************/
/*********** Search for users with a given role in current scope *************/
/*****************************************************************************/

void Usr_DB_BuildQueryToSearchListUsrs (Hie_Level_t HieLvl,Rol_Role_t Role,
					char **Query)
  {
   char SubQueryRole[64];
   static const char *QueryFields =
      "DISTINCT "
      "usr_data.UsrCod,"		// row[ 0]
      "usr_data.EncryptedUsrCod,"	// row[ 1]
      "usr_data.Password,"		// row[ 2]
      "usr_data.Surname1,"		// row[ 3]
      "usr_data.Surname2,"		// row[ 4]
      "usr_data.FirstName,"		// row[ 5]
      "usr_data.Sex,"			// row[ 6]
      "usr_data.Photo,"			// row[ 7]
      "usr_data.PhotoVisibility,"	// row[ 8]
      "usr_data.CtyCod,"		// row[ 9]
      "usr_data.InsCod";		// row[10]
   static const char *OrderBySubQuery =
             "usr_candidate_users.UsrCod=usr_data.UsrCod"
      " ORDER BY usr_data.Surname1,"
		"usr_data.Surname2,"
		"usr_data.FirstName,"
		"usr_data.UsrCod";

   /***** Build query *****/
   // if Scope is course ==> 3 columns are retrieved: UsrCod, Sex, Accepted
   //               else ==> 2 columns are retrieved: UsrCod, Sex
   // Search is faster (aproximately x2) using a temporary table to store users found in the whole platform
   switch (Role)
     {
      case Rol_UNK:	// Here Rol_UNK means any rol (role does not matter)
	 switch (HieLvl)
	   {
	    case Hie_SYS:
	       /* Search users from the whole platform */
	       DB_BuildQuery (Query,
			      "SELECT %s"
			      " FROM usr_candidate_users,usr_data"
			      " WHERE %s",
			      QueryFields,
			      OrderBySubQuery);
	       break;
	    case Hie_CTY:
	       /* Search users in courses from the current country */
	       DB_BuildQuery (Query,
			      "SELECT %s"
			       " FROM usr_candidate_users,"
			             "crs_users,"
			             "crs_courses,"
			             "deg_degrees,"
			             "ctr_centers,"
			             "ins_instits,"
			             "usr_data"
			      " WHERE usr_candidate_users.UsrCod=crs_users.UsrCod"
			        " AND crs_users.CrsCod=crs_courses.CrsCod"
			        " AND crs_courses.DegCod=deg_degrees.DegCod"
			        " AND deg_degrees.CtrCod=ctr_centers.CtrCod"
			        " AND ctr_centers.InsCod=ins_instits.InsCod"
			        " AND ins_instits.CtyCod=%ld"
			        " AND %s",
			      QueryFields,
			      Gbl.Hierarchy.Node[Hie_CTY].HieCod,
			      OrderBySubQuery);
	       break;
	    case Hie_INS:
	       /* Search users in courses from the current institution */
	       DB_BuildQuery (Query,
			      "SELECT %s"
			       " FROM usr_candidate_users,"
			             "crs_users,"
			             "crs_courses,"
			             "deg_degrees,"
			             "ctr_centers,"
			             "usr_data"
			      " WHERE usr_candidate_users.UsrCod=crs_users.UsrCod"
			        " AND crs_users.CrsCod=crs_courses.CrsCod"
			        " AND crs_courses.DegCod=deg_degrees.DegCod"
			        " AND deg_degrees.CtrCod=ctr_centers.CtrCod"
			        " AND ctr_centers.InsCod=%ld"
			        " AND %s",
			      QueryFields,
			      Gbl.Hierarchy.Node[Hie_INS].HieCod,
			      OrderBySubQuery);
	       break;
	    case Hie_CTR:
	       /* Search users in courses from the current center */
	       DB_BuildQuery (Query,
			      "SELECT %s"
			       " FROM usr_candidate_users,"
			             "crs_users,"
			             "crs_courses,"
			             "deg_degrees,"
			            " usr_data"
			      " WHERE usr_candidate_users.UsrCod=crs_users.UsrCod"
			        " AND crs_users.CrsCod=crs_courses.CrsCod"
			        " AND crs_courses.DegCod=deg_degrees.DegCod"
			        " AND deg_degrees.CtrCod=%ld"
			        " AND %s",
			      QueryFields,
			      Gbl.Hierarchy.Node[Hie_CTR].HieCod,
			      OrderBySubQuery);
	       break;
	    case Hie_DEG:
	       /* Search users in courses from the current degree */
	       DB_BuildQuery (Query,
			      "SELECT %s"
			       " FROM usr_candidate_users,"
			             "crs_users,"
			             "crs_courses,"
			             "usr_data"
			      " WHERE usr_candidate_users.UsrCod=crs_users.UsrCod"
			        " AND crs_users.CrsCod=crs_courses.CrsCod"
			        " AND crs_courses.DegCod=%ld"
			        " AND %s",
			      QueryFields,
			      Gbl.Hierarchy.Node[Hie_DEG].HieCod,
			      OrderBySubQuery);
	       break;
	    case Hie_CRS:
	       /* Search users in courses from the current course */
	       DB_BuildQuery (Query,
			      "SELECT %s,"
			             "crs_users.Role,"		// row[11]
			             "crs_users.Accepted"	// row[12]
			       " FROM usr_candidate_users,"
			             "crs_users,"
			             "usr_data"
			      " WHERE usr_candidate_users.UsrCod=crs_users.UsrCod"
			        " AND crs_users.CrsCod=%ld"
			        " AND %s",
			      QueryFields,
			      Gbl.Hierarchy.Node[Hie_CRS].HieCod,
			      OrderBySubQuery);
	       break;
	    default:
	       Err_WrongHierarchyLevelExit ();
	       break;
	   }
         break;
      case Rol_GST:	// Guests (scope is not used)
	 /* Search users with no courses */
	 DB_BuildQuery (Query,
			"SELECT %s"
			 " FROM usr_candidate_users,"
			       "usr_data"
			" WHERE usr_candidate_users.UsrCod NOT IN"
			      " (SELECT UsrCod"
			         " FROM crs_users)"
			  " AND %s",
			QueryFields,
			OrderBySubQuery);
	 break;
      case Rol_STD:	// Student
      case Rol_NET:	// Non-editing teacher
      case Rol_TCH:	// Teacher
	 /*
	    To achieve maximum speed, it's important to do the things in this order:
	    1) Search for user's name (UsrQuery) getting candidate users
	    2) Filter the candidate users according to scope
	 */
	 switch (Role)
	   {
	    case Rol_STD:	// Student
	       sprintf (SubQueryRole," AND crs_users.Role=%u",
			(unsigned) Rol_STD);
	       break;
	    case Rol_NET:	// Non-editing teacher
	    case Rol_TCH:	// or teacher
	       sprintf (SubQueryRole," AND crs_users.Role IN (%u,%u)",
			(unsigned) Rol_NET,
			(unsigned) Rol_TCH);
	       break;
	    default:
	       SubQueryRole[0] = '\0';
	       break;
	   }
	 switch (HieLvl)
	   {
	    case Hie_SYS:
	       /* Search users in courses from the whole platform */
	       DB_BuildQuery (Query,
			      "SELECT %s"
			       " FROM usr_candidate_users,"
			             "crs_users,"
			             "usr_data"
			      " WHERE usr_candidate_users.UsrCod=crs_users.UsrCod"
			          "%s"
			        " AND %s",
			      QueryFields,
			      SubQueryRole,
			      OrderBySubQuery);
	       break;
	    case Hie_CTY:
	       /* Search users in courses from the current country */
	       DB_BuildQuery (Query,
			      "SELECT %s"
			       " FROM usr_candidate_users,"
			             "crs_users,"
			             "crs_courses,"
			             "deg_degrees,"
			             "ctr_centers,"
			             "ins_instits,"
			             "usr_data"
			      " WHERE usr_candidate_users.UsrCod=crs_users.UsrCod"
			          "%s"
			        " AND crs_users.CrsCod=crs_courses.CrsCod"
			        " AND crs_courses.DegCod=deg_degrees.DegCod"
			        " AND deg_degrees.CtrCod=ctr_centers.CtrCod"
			        " AND ctr_centers.InsCod=ins_instits.InsCod"
			        " AND ins_instits.CtyCod=%ld"
			        " AND %s",
			      QueryFields,
			      SubQueryRole,
			      Gbl.Hierarchy.Node[Hie_CTY].HieCod,
			      OrderBySubQuery);
	       break;
	    case Hie_INS:
	       /* Search users in courses from the current institution */
	       DB_BuildQuery (Query,
			      "SELECT %s"
			       " FROM usr_candidate_users,"
			             "crs_users,"
			             "crs_courses,"
			             "deg_degrees,"
			             "ctr_centers,"
			             "usr_data"
			      " WHERE usr_candidate_users.UsrCod=crs_users.UsrCod"
			          "%s"
			        " AND crs_users.CrsCod=crs_courses.CrsCod"
			        " AND crs_courses.DegCod=deg_degrees.DegCod"
			        " AND deg_degrees.CtrCod=ctr_centers.CtrCod"
			        " AND ctr_centers.InsCod=%ld"
			        " AND %s",
			      QueryFields,
			      SubQueryRole,
			      Gbl.Hierarchy.Node[Hie_INS].HieCod,
			      OrderBySubQuery);
	       break;
	    case Hie_CTR:
	       /* Search users in courses from the current center */
	       DB_BuildQuery (Query,
			      "SELECT %s"
			       " FROM usr_candidate_users,"
			             "crs_users,"
			             "crs_courses,"
			             "deg_degrees,"
			             "usr_data"
			      " WHERE usr_candidate_users.UsrCod=crs_users.UsrCod"
			          "%s"
			        " AND crs_users.CrsCod=crs_courses.CrsCod"
			        " AND crs_courses.DegCod=deg_degrees.DegCod"
			        " AND deg_degrees.CtrCod=%ld"
			        " AND %s",
			      QueryFields,
			      SubQueryRole,
			      Gbl.Hierarchy.Node[Hie_CTR].HieCod,
			      OrderBySubQuery);
	       break;
	    case Hie_DEG:
	       /* Search users in courses from the current degree */
	       DB_BuildQuery (Query,
			      "SELECT %s"
			       " FROM usr_candidate_users,"
			             "crs_users,"
			             "crs_courses,"
			             "usr_data"
			      " WHERE usr_candidate_users.UsrCod=crs_users.UsrCod"
			          "%s"
			        " AND crs_users.CrsCod=crs_courses.CrsCod"
			        " AND crs_courses.DegCod=%ld"
			        " AND %s",
			      QueryFields,
			      SubQueryRole,
			      Gbl.Hierarchy.Node[Hie_DEG].HieCod,
			      OrderBySubQuery);
	       break;
	    case Hie_CRS:
	       /* Search users in courses from the current course */
	       DB_BuildQuery (Query,
			      "SELECT %s,"
			             "crs_users.Role,"
			             "crs_users.Accepted"
			       " FROM usr_candidate_users,"
			             "crs_users,"
			             "usr_data"
			      " WHERE usr_candidate_users.UsrCod=crs_users.UsrCod"
			          "%s"
			        " AND crs_users.CrsCod=%ld"
			        " AND %s",
			      QueryFields,
			      SubQueryRole,
			      Gbl.Hierarchy.Node[Hie_CRS].HieCod,
			      OrderBySubQuery);
	       break;
	    default:
	       Err_WrongHierarchyLevelExit ();
	       break;
	   }
	 break;
      default:
	 Err_WrongRoleExit ();
	 break;
     }

   // if (Gbl.Usrs.Me.Roles.LoggedRole == Rol_SYS_ADM)
   //   Lay_ShowAlert (Lay_INFO,Query);
  }

/*****************************************************************************/
/****** Build query to get the user's codes of all students of a degree ******/
/*****************************************************************************/

void Usr_DB_BuildQueryToGetUnorderedStdsCodesInDeg (long DegCod,char **Query)
  {
   DB_BuildQuery (Query,
		  "SELECT DISTINCT "
		         "usr_data.UsrCod,"		// row[ 0]
			 "usr_data.EncryptedUsrCod,"	// row[ 1]
			 "usr_data.Password,"		// row[ 2]
			 "usr_data.Surname1,"		// row[ 3]
			 "usr_data.Surname2,"		// row[ 4]
			 "usr_data.FirstName,"		// row[ 5]
			 "usr_data.Sex,"		// row[ 6]
			 "usr_data.Photo,"		// row[ 7]
			 "usr_data.PhotoVisibility,"	// row[ 8]
			 "usr_data.CtyCod,"		// row[ 9]
			 "usr_data.InsCod"		// row[10]
		   " FROM crs_courses,"
		         "crs_users,"
		         "usr_data"
		  " WHERE crs_courses.DegCod=%ld"
		    " AND crs_courses.CrsCod=crs_users.CrsCod"
		    " AND crs_users.Role=%u"
		    " AND crs_users.UsrCod=usr_data.UsrCod",
		  DegCod,
		  (unsigned) Rol_STD);
  }

/*****************************************************************************/
/************** Get number of users who have chosen an option ****************/
/*****************************************************************************/

unsigned Usr_DB_GetNumUsrsWhoChoseAnOption (Hie_Level_t HieLvl,const char *SubQuery)
  {
   switch (HieLvl)
     {
      case Hie_SYS:
	 return (unsigned)
	 DB_QueryCOUNT ("can not get the number of users who have chosen an option",
		        "SELECT COUNT(*)"
		         " FROM usr_data"
		        " WHERE %s",
		        SubQuery);
      case Hie_CTY:
	 return (unsigned)
	 DB_QueryCOUNT ("can not get the number of users who have chosen an option",
		        "SELECT COUNT(DISTINCT usr_data.UsrCod)"
		         " FROM ins_instits,"
			       "ctr_centers,"
			       "deg_degrees,"
			       "crs_courses,"
			       "crs_users,"
			       "usr_data"
		        " WHERE ins_instits.CtyCod=%ld"
		          " AND ins_instits.InsCod=ctr_centers.InsCod"
		          " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
		          " AND deg_degrees.DegCod=crs_courses.DegCod"
		          " AND crs_courses.CrsCod=crs_users.CrsCod"
		          " AND crs_users.UsrCod=usr_data.UsrCod"
		          " AND %s",
		        Gbl.Hierarchy.Node[Hie_CTY].HieCod,SubQuery);
      case Hie_INS:
	 return (unsigned)
	 DB_QueryCOUNT ("can not get the number of users who have chosen an option",
		        "SELECT COUNT(DISTINCT usr_data.UsrCod)"
		         " FROM ctr_centers,"
		               "deg_degrees,"
		               "crs_courses,"
		               "crs_users,"
		               "usr_data"
		        " WHERE ctr_centers.InsCod=%ld"
		          " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
		          " AND deg_degrees.DegCod=crs_courses.DegCod"
		          " AND crs_courses.CrsCod=crs_users.CrsCod"
		          " AND crs_users.UsrCod=usr_data.UsrCod"
		          " AND %s",
		        Gbl.Hierarchy.Node[Hie_INS].HieCod,SubQuery);
      case Hie_CTR:
	 return (unsigned)
	 DB_QueryCOUNT ("can not get the number of users who have chosen an option",
		        "SELECT COUNT(DISTINCT usr_data.UsrCod)"
		         " FROM deg_degrees,"
		               "crs_courses,"
		               "crs_users,"
		               "usr_data"
		        " WHERE deg_degrees.CtrCod=%ld"
		          " AND deg_degrees.DegCod=crs_courses.DegCod"
		          " AND crs_courses.CrsCod=crs_users.CrsCod"
		          " AND crs_users.UsrCod=usr_data.UsrCod"
		          " AND %s",
		        Gbl.Hierarchy.Node[Hie_CTR].HieCod,SubQuery);
      case Hie_DEG:
	 return (unsigned)
	 DB_QueryCOUNT ("can not get the number of users who have chosen an option",
		        "SELECT COUNT(DISTINCT usr_data.UsrCod)"
		         " FROM crs_courses,"
		               "crs_users,"
		               "usr_data"
		        " WHERE crs_courses.DegCod=%ld"
		          " AND crs_courses.CrsCod=crs_users.CrsCod"
		          " AND crs_users.UsrCod=usr_data.UsrCod"
		          " AND %s",
		        Gbl.Hierarchy.Node[Hie_DEG].HieCod,SubQuery);
      case Hie_CRS:
	 return (unsigned)
	 DB_QueryCOUNT ("can not get the number of users who have chosen an option",
		        "SELECT COUNT(DISTINCT usr_data.UsrCod)"
		         " FROM crs_users,"
		               "usr_data"
		        " WHERE crs_users.CrsCod=%ld"
		          " AND crs_users.UsrCod=usr_data.UsrCod"
		          " AND %s",
		        Gbl.Hierarchy.Node[Hie_CRS].HieCod,SubQuery);
      default:
	 Err_WrongHierarchyLevelExit ();
	 return 0;	// Not reached
     }
  }

/*****************************************************************************/
/************************* Get old users from database ***********************/
/*****************************************************************************/

unsigned Usr_DB_GetOldUsrs (MYSQL_RES **mysql_res,time_t SecondsWithoutAccess)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get old users",
		   "SELECT UsrCod"
		    " FROM (SELECT UsrCod"
			    " FROM usr_last"
			   " WHERE LastTime<FROM_UNIXTIME(UNIX_TIMESTAMP()-%llu)"
			   " UNION "
			   "SELECT UsrCod"
			    " FROM usr_data"
			   " WHERE UsrCod NOT IN"
			         " (SELECT UsrCod"
				    " FROM usr_last)"
			  ") AS candidate_usrs"
		   " WHERE UsrCod NOT IN"
		         " (SELECT DISTINCT "
		                  "UsrCod"
			    " FROM crs_users)",
		   (unsigned long long) SecondsWithoutAccess);
  }

/*****************************************************************************/
/*************** Create temporary table with candidate users *****************/
/*****************************************************************************/

void Usr_DB_CreateTmpTableAndSearchCandidateUsrs (const char SearchQuery[Sch_MAX_BYTES_SEARCH_QUERY + 1])
  {
   /***** Create temporary table with candidate users *****/
   /*
      - Search is faster (aproximately x2) using temporary tables.
      - Searching for names is made in the whole platform
        and stored in this table.
   */
   DB_CreateTmpTable ("CREATE TEMPORARY TABLE usr_candidate_users"
		      " (UsrCod INT NOT NULL,UNIQUE INDEX(UsrCod)) ENGINE=MEMORY"
		      " SELECT UsrCod"
		        " FROM usr_data"
		       " WHERE %s",
		      SearchQuery);
  }

/*****************************************************************************/
/***************** Drop temporary table with candidate users *****************/
/*****************************************************************************/

void Usr_DB_DropTmpTableWithCandidateUsrs (void)
  {
   DB_DropTmpTable ("usr_candidate_users");
  }

/*****************************************************************************/
/****************************** Remove user's data ***************************/
/*****************************************************************************/

void Usr_DB_RemoveUsrData (long UsrCod)
  {
   /***** Remove user's data *****/
   DB_QueryDELETE ("can not remove user's data",
		   "DELETE FROM usr_data"
		   " WHERE UsrCod=%ld",
		   UsrCod);
  }

/*****************************************************************************/
/*************** Create new entry for my last data in database ***************/
/*****************************************************************************/

void Usr_DB_InsertMyLastData (void)
  {
   /***** Insert my last accessed course, tab and time of click in database *****/
   DB_QueryINSERT ("can not insert last user's data",
		   "INSERT INTO usr_last"
	           " (UsrCod,WhatToSearch,"
	             "LastSco,LastCod,LastAct,LastRole,LastTime,LastAccNotif)"
                   " VALUES"
                   " (%ld,%u,"
                     "'%s',%ld,%ld,%u,NOW(),FROM_UNIXTIME(%ld))",
		   Gbl.Usrs.Me.UsrDat.UsrCod,
		   (unsigned) Sch_SEARCH_ALL,
		   Hie_GetDBStrFromLevel (Gbl.Hierarchy.HieLvl),
		   Gbl.Hierarchy.Node[Gbl.Hierarchy.HieLvl].HieCod,
		   Act_GetActCod (Gbl.Action.Act),
		   (unsigned) Gbl.Usrs.Me.Role.Logged,
		   (long) (time_t) 0);	// The user never accessed to notifications
  }
/*****************************************************************************/
/********* Update my last accessed course, tab and time in database **********/
/*****************************************************************************/

void Usr_DB_UpdateMyLastData (void)
  {
   /***** Update my last accessed course, tab and time of click in database *****/
   // WhatToSearch, LastAccNotif remain unchanged
   DB_QueryUPDATE ("can not update last user's data",
		   "UPDATE usr_last"
		     " SET LastSco='%s',"
			  "LastCod=%ld,"
			  "LastAct=%ld,"
			  "LastRole=%u,"
			  "LastTime=NOW()"
		   " WHERE UsrCod=%ld",
		   Hie_GetDBStrFromLevel (Gbl.Hierarchy.HieLvl),
		   Gbl.Hierarchy.Node[Gbl.Hierarchy.HieLvl].HieCod,
		   Act_GetActCod (Gbl.Action.Act),
		   (unsigned) Gbl.Usrs.Me.Role.Logged,
		   Gbl.Usrs.Me.UsrDat.UsrCod);
  }

/*****************************************************************************/
/********************** Update my last type of search ************************/
/*****************************************************************************/

void Usr_DB_UpdateMyLastWhatToSearch (Sch_WhatToSearch_t WhatToSearch)
  {
   // WhatToSearch is stored in usr_last for next time I log in
   // In other existing sessions distinct to this, WhatToSearch will remain unchanged
   DB_QueryUPDATE ("can not update type of search in user's last data",
		   "UPDATE usr_last"
		     " SET WhatToSearch=%u"
		   " WHERE UsrCod=%ld",
		   (unsigned) WhatToSearch,
		   Gbl.Usrs.Me.UsrDat.UsrCod);
  }

/*****************************************************************************/
/************** Check if it exists an entry for me in last data **************/
/*****************************************************************************/

bool Usr_DB_CheckMyLastData (void)
  {
   return
   DB_QueryEXISTS ("can not check last user's data",
		   "SELECT EXISTS"
		   "(SELECT *"
		     " FROM usr_last"
		    " WHERE UsrCod=%ld)",
		   Gbl.Usrs.Me.UsrDat.UsrCod) == Exi_EXISTS;
  }

/*****************************************************************************/
/********** Get user's last data from database giving a user's code **********/
/*****************************************************************************/

Exi_Exist_t Usr_DB_GetMyLastData (MYSQL_RES **mysql_res)
  {
   return
   DB_QuerySELECTunique (mysql_res,"can not get user's last data",
			 "SELECT WhatToSearch,"			// row[0]
				"LastSco,"			// row[1]
				"LastCod,"			// row[2]
				"LastAct,"			// row[3]
				"LastRole,"			// row[4]
				"UNIX_TIMESTAMP(LastTime),"	// row[5]
				"UNIX_TIMESTAMP(LastAccNotif)"	// row[6]
			  " FROM usr_last"
			 " WHERE UsrCod=%ld",
			 Gbl.Usrs.Me.UsrDat.UsrCod);
  }

/*****************************************************************************/
/*************************** Remove user's last data *************************/
/*****************************************************************************/

void Usr_DB_RemoveUsrLastData (long UsrCod)
  {
   DB_QueryDELETE ("can not remove user's last data",
		   "DELETE FROM usr_last"
		   " WHERE UsrCod=%ld",
		   UsrCod);
  }

/*****************************************************************************/
/*** Insert my user's code in the table of birthdays already congratulated ***/
/*****************************************************************************/

void Usr_DB_MarkMyBirthdayAsCongratulated (void)
  {
   DB_QueryINSERT ("can not insert birthday",
		   "INSERT INTO usr_birthdays_today"
	           " (UsrCod,Today)"
	           " VALUES"
	           " (%ld,CURDATE())",
		   Gbl.Usrs.Me.UsrDat.UsrCod);
  }

/*****************************************************************************/
/*************** Check if my birthday is already congratulated ***************/
/*****************************************************************************/

bool Usr_DB_CheckIfMyBirthdayHasNotBeenCongratulated (void)
  {
   return
   DB_QueryEXISTS ("can not check if my birthday has been congratulated",
		   "SELECT EXISTS"
		   "(SELECT *"
		     " FROM usr_birthdays_today"
		    " WHERE UsrCod=%ld)",
		   Gbl.Usrs.Me.UsrDat.UsrCod) == Exi_EXISTS;
  }

/*****************************************************************************/
/**************************** Delete old birthdays ***************************/
/*****************************************************************************/

void Usr_DB_DeleteOldBirthdays (void)
  {
   DB_QueryDELETE ("can not delete old birthdays",
		   "DELETE FROM usr_birthdays_today"
		   " WHERE Today<>CURDATE()");
  }

/*****************************************************************************/
/********************* Copy user into my users' clipboard ********************/
/*****************************************************************************/

void Usr_DB_CopyToClipboard (long OthUsrCod)
  {
   DB_QueryREPLACE ("can not copy user to clipboard",
		    "REPLACE INTO usr_clipboards"
		    " (UsrCod,OthUsrCod,CopyTime)"
		    " VALUES"
		    " (%ld,%ld,NOW())",
		    Gbl.Usrs.Me.UsrDat.UsrCod,
		    OthUsrCod);
  }


/*****************************************************************************/
/**************** Get number of users in my users' clipboard *****************/
/*****************************************************************************/

unsigned Usr_DB_GetNumUsrsInMyClipboard (void)
  {
   return (unsigned)
   DB_QueryCOUNT ("can not get resources",
		  "SELECT COUNT(*)"
		   " FROM usr_clipboards"
		  " WHERE UsrCod=%ld",
		  Gbl.Usrs.Me.UsrDat.UsrCod);
  }

/*****************************************************************************/
/********** Build query to get list of users in my users' clipboard **********/
/*****************************************************************************/

void Usr_DB_BuildQueryToGetUsrsInMyClipboard (char **Query)
  {
   /*
   row[ 0]: usr_data.UsrCod
   row[ 1]: usr_data.EncryptedUsrCod
   row[ 2]: usr_data.Password (used to check if a teacher can edit user's data)
   row[ 3]: usr_data.Surname1
   row[ 4]: usr_data.Surname2
   row[ 5]: usr_data.FirstName
   row[ 6]: usr_data.Sex
   row[ 7]: usr_data.Photo
   row[ 8]: usr_data.PhotoVisibility
   row[ 9]: usr_data.CtyCod
   row[10]: usr_data.InsCod
   */
   DB_BuildQuery (Query,
		  "SELECT usr_data.UsrCod,"
		         "usr_data.EncryptedUsrCod,"
		         "usr_data.Password,"
		         "usr_data.Surname1,"
		         "usr_data.Surname2,"
		         "usr_data.FirstName,"
		         "usr_data.Sex,"
		         "usr_data.Photo,"
		         "usr_data.PhotoVisibility,"
		         "usr_data.CtyCod,"
		         "usr_data.InsCod"
		   " FROM usr_clipboards,"
		         "usr_data"
		  " WHERE usr_clipboards.UsrCod=%ld"
		    " AND usr_clipboards.OthUsrCod=usr_data.UsrCod"
	       " ORDER BY usr_data.Surname1,"
		         "usr_data.Surname2,"
		         "usr_data.FirstName,"
		         "usr_data.UsrCod",
		  Gbl.Usrs.Me.UsrDat.UsrCod);
  }

/*****************************************************************************/
/************************** Remove my users' clipboard ***********************/
/*****************************************************************************/

void Usr_DB_RemoveMyClipboard (void)
  {
   DB_QueryDELETE ("can not remove user clipboard",
		   "DELETE FROM usr_clipboards"
		   " WHERE UsrCod=%ld",
	           Gbl.Usrs.Me.UsrDat.UsrCod);
  }

/*****************************************************************************/
/****************** Remove expired clipboards (from all users) ***************/
/*****************************************************************************/

void Usr_DB_RemoveExpiredClipboards (void)
  {
   DB_QueryDELETE ("can not remove old user clipboards",
		   "DELETE LOW_PRIORITY FROM usr_clipboards"
		   " WHERE UsrCod IN"
		   " (SELECT UsrCod"
		      " FROM (SELECT DISTINCT UsrCod"
		              " FROM usr_clipboards"
		              " WHERE CopyTime<FROM_UNIXTIME(UNIX_TIMESTAMP()-%lu))"
		              " AS usr_old_clipboards)",
                   Cfg_TIME_TO_DELETE_USER_CLIPBOARD);
  }
