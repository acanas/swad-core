// swad_timeline_database.c: social timeline operations with database

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2021 Antonio Cañas Vargas

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

#include "swad_database.h"
#include "swad_timeline.h"
#include "swad_timeline_database.h"
#include "swad_timeline_publication.h"

/*****************************************************************************/
/****************************** Public constants *****************************/
/*****************************************************************************/

/*****************************************************************************/
/************************* Private constants and types ***********************/
/*****************************************************************************/

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

/*****************************************************************************/
/************************* Private global variables **************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

/*****************************************************************************/
/********************* Get number of comments in a note **********************/
/*****************************************************************************/

unsigned TL_DB_GetNumCommentsInNote (long NotCod)
  {
   return (unsigned)
   DB_QueryCOUNT ("can not get number of comments in a note",
		  "SELECT COUNT(*) FROM tl_pubs"
		  " WHERE NotCod=%ld AND PubType=%u",
		  NotCod,(unsigned) TL_Pub_COMMENT_TO_NOTE);
  }

/*****************************************************************************/
/************** Get initial comments of a note from database *****************/
/*****************************************************************************/

unsigned TL_DB_GetInitialComments (long NotCod,
				   unsigned NumInitialCommentsToGet,
				   MYSQL_RES **mysql_res)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get comments",
		   "SELECT tl_pubs.PubCod,"				// row[0]
			  "tl_pubs.PublisherCod,"			// row[1]
			  "tl_pubs.NotCod,"				// row[2]
			  "UNIX_TIMESTAMP(tl_pubs.TimePublish),"	// row[3]
			  "tl_comments.Txt,"				// row[4]
			  "tl_comments.MedCod"				// row[5]
		   " FROM tl_pubs,tl_comments"
		   " WHERE tl_pubs.NotCod=%ld"
		   " AND tl_pubs.PubType=%u"
		   " AND tl_pubs.PubCod=tl_comments.PubCod"
		   " ORDER BY tl_pubs.PubCod"
		   " LIMIT %lu",
		   NotCod,(unsigned) TL_Pub_COMMENT_TO_NOTE,
		   NumInitialCommentsToGet);
  }

/*****************************************************************************/
/*************** Get final comments of a note from database ******************/
/*****************************************************************************/

unsigned TL_DB_GetFinalComments (long NotCod,
				 unsigned NumFinalCommentsToGet,
				 MYSQL_RES **mysql_res)
  {
   /***** Get final comments of a note from database *****/
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get comments",
		   "SELECT * FROM "
		   "("
		   "SELECT tl_pubs.PubCod,"				// row[0]
			  "tl_pubs.PublisherCod,"			// row[1]
			  "tl_pubs.NotCod,"				// row[2]
			  "UNIX_TIMESTAMP(tl_pubs.TimePublish),"	// row[3]
			  "tl_comments.Txt,"				// row[4]
			  "tl_comments.MedCod"				// row[5]
	          " FROM tl_pubs,tl_comments"
		  " WHERE tl_pubs.NotCod=%ld"
		  " AND tl_pubs.PubType=%u"
		  " AND tl_pubs.PubCod=tl_comments.PubCod"
		  " ORDER BY tl_pubs.PubCod DESC LIMIT %u"
		  ") AS comments"
		  " ORDER BY PubCod",
		  NotCod,(unsigned) TL_Pub_COMMENT_TO_NOTE,
		  NumFinalCommentsToGet);
  }

/*****************************************************************************/
/******************* Insert comment content in database **********************/
/*****************************************************************************/

void TL_DB_InsertCommentContent (long PubCod,
				 const struct TL_Pst_PostContent *Content)
  {
   /***** Insert comment content in database *****/
   DB_QueryINSERT ("can not store comment content",
		   "INSERT INTO tl_comments"
		   " (PubCod,Txt,MedCod)"
		   " VALUES"
		   " (%ld,'%s',%ld)",
		   PubCod,
		   Content->Txt,
		   Content->Media.MedCod);
  }
