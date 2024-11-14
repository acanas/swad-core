// swad_announcement_database.c: Global announcements operations with database

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2024 Antonio Cañas Vargas

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

#include "swad_announcement.h"
#include "swad_announcement_database.h"
#include "swad_database.h"
#include "swad_global.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/************************** Select all announcements *************************/
/*****************************************************************************/
// Returns the number of announcements got

unsigned Ann_DB_GetAllAnnouncements (MYSQL_RES **mysql_res)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get announcements",
		   "SELECT AnnCod,"	// row[0]
			  "Status,"	// row[1]
			  "Roles,"	// row[2]
			  "Subject,"	// row[3]
			  "Content"	// row[4]
		    " FROM ann_announcements"
		" ORDER BY AnnCod DESC");
  }

/*****************************************************************************/
/******************** Select only announcements I can see ********************/
/*****************************************************************************/
// Returns the number of announcements got

unsigned Ann_DB_GetAnnouncementsICanSee (MYSQL_RES **mysql_res)
  {
   Rol_GetRolesInAllCrss (&Gbl.Usrs.Me.UsrDat);

   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get announcements",
		   "SELECT AnnCod,"	// row[0]
			  "Status,"	// row[1]
			  "Roles,"	// row[2]
			  "Subject,"	// row[3]
			  "Content"	// row[4]
		    " FROM ann_announcements"
		   " WHERE (Roles&%u)<>0 "	// All my roles in different courses
		" ORDER BY AnnCod DESC",
		   (unsigned) Gbl.Usrs.Me.UsrDat.Roles.InCrss);
  }

/*****************************************************************************/
/************ Select only active announcements for unknown users *************/
/*****************************************************************************/
// Returns the number of announcements got

unsigned Ann_DB_GetAnnouncementsForUnknownUsers (MYSQL_RES **mysql_res)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get announcements",
		   "SELECT AnnCod,"	// row[0]
			  "Status,"	// row[1]
			  "Roles,"	// row[2]
			  "Subject,"	// row[3]
			  "Content"	// row[4]
		    " FROM ann_announcements"
		   " WHERE Status=%u"
		     " AND (Roles&%u)<>0 "
		" ORDER BY AnnCod DESC",
		   (unsigned) Ann_ACTIVE_ANNOUNCEMENT,
		   (unsigned) (1 << Rol_UNK));
  }

/*****************************************************************************/
/************************ Select announcements not seen **********************/
/*****************************************************************************/
// Returns the number of announcements got

unsigned Ann_DB_GetAnnouncementsNotSeen (MYSQL_RES **mysql_res)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get announcements",
		   "SELECT AnnCod,"	// row[0]
			  "Status,"	// row[1]
			  "Roles,"	// row[2]
			  "Subject,"	// row[3]
			  "Content"	// row[4]
		    " FROM ann_announcements"
		   " WHERE Status=%u"
		     " AND (Roles&%u)<>0 "	// All my roles in different courses
		     " AND AnnCod NOT IN"
			 " (SELECT AnnCod"
			    " FROM ann_seen"
			   " WHERE UsrCod=%ld)"
		" ORDER BY AnnCod DESC",	// Newest first
		   (unsigned) Ann_ACTIVE_ANNOUNCEMENT,
		   (unsigned) Gbl.Usrs.Me.UsrDat.Roles.InCrss,
		   Gbl.Usrs.Me.UsrDat.UsrCod);
 }

/*****************************************************************************/
/************************ Create a new announcement **************************/
/*****************************************************************************/

void Ann_DB_CreateAnnouncement (unsigned Roles,
                                const char *Subject,const char *Content)
  {
   DB_QueryINSERT ("can not create announcement",
		   "INSERT INTO ann_announcements"
		   " (Roles,Subject,Content)"
		   " VALUES"
		   " (%u,'%s','%s')",
                   Roles,
                   Subject,
                   Content);
  }

/*****************************************************************************/
/********************* Set global announcement as hidden *********************/
/*****************************************************************************/

void Ann_DB_HideAnnouncement (long AnnCod)
  {
   DB_QueryUPDATE ("can not hide announcement",
		   "UPDATE ann_announcements"
		     " SET Status=%u"
		   " WHERE AnnCod=%ld",
                   (unsigned) Ann_OBSOLETE_ANNOUNCEMENT,
                   AnnCod);
  }

/*****************************************************************************/
/******************** Set global announcement as unhidden ********************/
/*****************************************************************************/

void Ann_DB_UnhideAnnouncement (long AnnCod)
  {
   DB_QueryUPDATE ("can not unhide announcement",
		   "UPDATE ann_announcements"
		     " SET Status=%u"
		   " WHERE AnnCod=%ld",
                   (unsigned) Ann_ACTIVE_ANNOUNCEMENT,
                   AnnCod);
  }

/*****************************************************************************/
/**************** Remove users who have seen an announcement *****************/
/*****************************************************************************/

void Ann_DB_RemoveUsrsWhoSawAnnouncement (long AnnCod)
  {
   DB_QueryDELETE ("can not remove users who saw announcement",
		   "DELETE FROM ann_seen"
		   " WHERE AnnCod=%ld",
		   AnnCod);
  }

/*****************************************************************************/
/************************** Remove an announcement ***************************/
/*****************************************************************************/

void Ann_DB_RemoveAnnouncement (long AnnCod)
  {
   DB_QueryDELETE ("can not remove announcement",
		   "DELETE FROM ann_announcements"
		   " WHERE AnnCod=%ld",
		   AnnCod);
  }

/*****************************************************************************/
/******************** Mark an announcement as seen by me *********************/
/*****************************************************************************/

void Ann_DB_MarkAnnouncementAsSeenByMe (long AnnCod)
  {
   DB_QueryREPLACE ("can not mark announcement as seen",
		    "REPLACE INTO ann_seen"
		    " (AnnCod,UsrCod)"
		    " VALUES"
		    " (%ld,%ld)",
                    AnnCod,
                    Gbl.Usrs.Me.UsrDat.UsrCod);
  }

/*****************************************************************************/
/******************** Remove user from seen announcements ********************/
/*****************************************************************************/

void Ann_DB_RemoveUsrFromSeenAnnouncements (long UsrCod)
  {
   DB_QueryDELETE ("can not remove user from seen announcements",
		   "DELETE FROM ann_seen"
		   " WHERE UsrCod=%ld",
		   UsrCod);
  }
