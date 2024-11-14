// swad_project_database.c: projects (final degree projects, thesis), operations with database

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
#include <stdio.h>		// For asprintf
#include <stdlib.h>		// For free

#include "swad_database.h"
#include "swad_error.h"
#include "swad_global.h"
#include "swad_project.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/************************** Public constants and types ***********************/
/*****************************************************************************/

/***** Enum field in database for type of rubric *****/
const char *Prj_DB_RubricType[PrjCfg_NUM_RUBRIC_TYPES] =
  {
   [PrjCfg_RUBRIC_TUT] = "tut",
   [PrjCfg_RUBRIC_EVL] = "evl",
   [PrjCfg_RUBRIC_GBL] = "gbl",
  };

/***** Enum field in database for types of proposal *****/
const char *Prj_DB_Proposal[Prj_NUM_PROPOSAL_TYPES] =
  {
   [Prj_PROPOSAL_NEW       ] = "new",
   [Prj_PROPOSAL_MODIFIED  ] = "modified",
   [Prj_PROPOSAL_UNMODIFIED] = "unmodified",
  };

/***** Enum field in database for review status *****/
const char *Prj_DB_ReviewStatus[Prj_NUM_REVIEW_STATUS] =
  {
   [Prj_UNREVIEWED] = "unreviewed",
   [Prj_UNAPPROVED] = "unapproved",
   [Prj_APPROVED  ] = "approved",
  };

/*****************************************************************************/
/**************************** Lock project edition ***************************/
/*****************************************************************************/

void Prj_DB_LockProjectEdition (long PrjCod)
  {
   DB_QueryUPDATE ("can not lock project edition",
		   "UPDATE prj_projects"
		     " SET Locked='Y'"
		   " WHERE PrjCod=%ld"
		     " AND CrsCod=%ld",
		   PrjCod,
		   Gbl.Hierarchy.Node[Hie_CRS].HieCod);
  }

/*****************************************************************************/
/************************** Unlock project edition ***************************/
/*****************************************************************************/

void Prj_DB_UnlockProjectEdition (long PrjCod)
  {
   DB_QueryUPDATE ("can not lock project edition",
		   "UPDATE prj_projects"
		     " SET Locked='N'"
		   " WHERE PrjCod=%ld"
		     " AND CrsCod=%ld",	// Extra check
		   PrjCod,
		   Gbl.Hierarchy.Node[Hie_CRS].HieCod);
  }

/*****************************************************************************/
/************************** Create a new project *****************************/
/*****************************************************************************/

long Prj_DB_CreateProject (const struct Prj_Project *Prj)
  {
   extern const char HidVis_YN[HidVis_NUM_HIDDEN_VISIBLE];

   return
   DB_QueryINSERTandReturnCode ("can not create new project",
				"INSERT INTO prj_projects"
				" (CrsCod,DptCod,Hidden,Assigned,NumStds,Proposal,"
				  "CreatTime,ModifTime,"
				  "Title,Description,Knowledge,Materials,URL,"
				  "ReviewStatus,ReviewTime,ReviewTxt)"
				" VALUES"
				" (%ld,%ld,'%c','%c',%u,'%s',"
				  "FROM_UNIXTIME(%ld),FROM_UNIXTIME(%ld),"
				  "'%s','%s','%s','%s','%s',"
				  "'%s',FROM_UNIXTIME(%ld),'%s')",
				Gbl.Hierarchy.Node[Hie_CRS].HieCod,
				Prj->DptCod,
				HidVis_YN[Prj->Hidden],
				Prj->Assigned == Prj_ASSIGNED ? 'Y' :
								'N',
				Prj->NumStds,
				Prj_DB_Proposal[Prj->Proposal],
				Prj->CreatTime,
				Prj->ModifTime,
				Prj->Title,
				Prj->Description,
				Prj->Knowledge,
				Prj->Materials,
				Prj->URL,
				Prj_DB_ReviewStatus[Prj->Review.Status],
				Prj->Review.Time,
				Prj->Review.Txt);
  }

/*****************************************************************************/
/*********************** Update an existing project **************************/
/*****************************************************************************/

void Prj_DB_UpdateProject (const struct Prj_Project *Prj)
  {
   extern const char HidVis_YN[HidVis_NUM_HIDDEN_VISIBLE];

   DB_QueryUPDATE ("can not update project",
		   "UPDATE prj_projects"
		     " SET DptCod=%ld,"
		          "Hidden='%c',"
		          "Assigned='%c',"
		          "NumStds=%u,"
		          "Proposal='%s',"
		          "ModifTime=FROM_UNIXTIME(%ld),"
		          "Title='%s',"
		          "Description='%s',"
		          "Knowledge='%s',"
		          "Materials='%s',"
		          "URL='%s'"
		   " WHERE PrjCod=%ld"
		     " AND CrsCod=%ld",	// Extra check
	           Prj->DptCod,
	           HidVis_YN[Prj->Hidden],
	           Prj->Assigned == Prj_ASSIGNED ? 'Y' :
						   'N',
	           Prj->NumStds,
	           Prj_DB_Proposal[Prj->Proposal],
	           Prj->ModifTime,
	           Prj->Title,
	           Prj->Description,
	           Prj->Knowledge,
	           Prj->Materials,
	           Prj->URL,
	           Prj->PrjCod,
	           Gbl.Hierarchy.Node[Hie_CRS].HieCod);
  }

/*****************************************************************************/
/****************************** Add user to project **************************/
/*****************************************************************************/

void Prj_DB_AddUsrToPrj (long PrjCod,Prj_RoleInProject_t RoleInProject,long UsrCod)
  {
   DB_QueryREPLACE ("can not add user to project",
		    "REPLACE INTO prj_users"
		    " (PrjCod,RoleInProject,UsrCod)"
		    " VALUES"
		    " (%ld,%u,%ld)",
		    PrjCod,
		    (unsigned) RoleInProject,
		    UsrCod);
  }

/*****************************************************************************/
/****************************** Hide a project *******************************/
/*****************************************************************************/

void Prj_DB_HideOrUnhideProject (long PrjCod,
				 HidVis_HiddenOrVisible_t HiddenOrVisible)
  {
   extern const char HidVis_YN[HidVis_NUM_HIDDEN_VISIBLE];

   DB_QueryUPDATE ("can not hide/unhide project",
		   "UPDATE prj_projects"
		     " SET Hidden='%c'"
		   " WHERE PrjCod=%ld"
		     " AND CrsCod=%ld",	// Extra check
		   HidVis_YN[HiddenOrVisible],
		   PrjCod,
		   Gbl.Hierarchy.Node[Hie_CRS].HieCod);
  }

/*****************************************************************************/
/******************* Update review of an existing project ********************/
/*****************************************************************************/

void Prj_DB_UpdateReview (const struct Prj_Project *Prj)
  {
   DB_QueryUPDATE ("can not update project review",
		   "UPDATE prj_projects"
		     " SET ReviewStatus='%s',"
		          "ReviewTime=NOW(),"
		          "ReviewTxt='%s'"
		   " WHERE PrjCod=%ld"
		     " AND CrsCod=%ld",	// Extra check
	           Prj_DB_ReviewStatus[Prj->Review.Status],
		   Prj->Review.Txt,
	           Prj->PrjCod,
	           Gbl.Hierarchy.Node[Hie_CRS].HieCod);
  }

/*****************************************************************************/
/******************************** Get projects *******************************/
/*****************************************************************************/

unsigned Prj_DB_GetListProjects (MYSQL_RES **mysql_res,
                                 const struct Prj_Projects *Projects,
                                 const char *UsrsSubQuery)	// NULL if no users
  {
   /* Role in project subquery */
   /*
   static const char *RolPrjSubQuery[16] =				// EVL TUT STD UNK
     {									//  3   2   1   0
      [0b0000] = "",							//  ·   ·   ·   ·
      [0b0010] = " AND (prj_users.RoleInProject IN "	"(1)"	   ")",	//  ·   ·   1   ·
      [0b0100] = " AND (prj_users.RoleInProject IN "	  "(2)"	   ")",	//  ·   1   ·   ·
      [0b0110] = " AND (prj_users.RoleInProject IN "	"(1,2)"	   ")",	//  ·   1   1   ·
      [0b1000] = " AND (prj_users.RoleInProject IN "	    "(3)"  ")",	//  1   ·   ·   ·
      [0b1010] = " AND (prj_users.RoleInProject IN "	"(1,""3)"  ")",	//  1   ·   1   ·
      [0b1100] = " AND (prj_users.RoleInProject IN "	  "(2,3)"  ")",	//  1   1   ·   ·
      [0b1110] = " AND (prj_users.RoleInProject IN "	"(1,2,3)"  ")",	//  1   1   1   ·
     };
   */
   static const char *RolPrjSubQuery[16] =									// EVL TUT STD UNK
     {														//  3   2   1   0
      [0b0000] = "",												//  ·   ·   ·   ·
      [0b0001] = " AND ("                                	  	    "prj_users.RoleInProject IS NULL)",	//  ·   ·   ·   1
      [0b0010] = " AND (prj_users.RoleInProject IN "	"(1)"						   ")",	//  ·   ·   1   ·
      [0b0011] = " AND (prj_users.RoleInProject IN "	"(1)"		" OR prj_users.RoleInProject IS NULL)",	//  ·   ·   1   1
      [0b0100] = " AND (prj_users.RoleInProject IN "	  "(2)"						   ")",	//  ·   1   ·   ·
      [0b0101] = " AND (prj_users.RoleInProject IN "	  "(2)"		" OR prj_users.RoleInProject IS NULL)",	//  ·   1   ·   1
      [0b0110] = " AND (prj_users.RoleInProject IN "	"(1,2)"						   ")",	//  ·   1   1   ·
      [0b0111] = " AND (prj_users.RoleInProject IN "	"(1,2)"		" OR prj_users.RoleInProject IS NULL)",	//  ·   1   1   1
      [0b1000] = " AND (prj_users.RoleInProject IN "	    "(3)"					   ")",	//  1   ·   ·   ·
      [0b1001] = " AND (prj_users.RoleInProject IN "	    "(3)"	" OR prj_users.RoleInProject IS NULL)",	//  1   ·   ·   1
      [0b1010] = " AND (prj_users.RoleInProject IN "	"(1,""3)"					   ")",	//  1   ·   1   ·
      [0b1011] = " AND (prj_users.RoleInProject IN "	"(1,""3)"	" OR prj_users.RoleInProject IS NULL)",	//  1   ·   1   1
      [0b1100] = " AND (prj_users.RoleInProject IN "	  "(2,3)"					   ")",	//  1   1   ·   ·
      [0b1101] = " AND (prj_users.RoleInProject IN "	  "(2,3)"	" OR prj_users.RoleInProject IS NULL)",	//  1   1   ·   1
      [0b1110] = " AND (prj_users.RoleInProject IN "	"(1,2,3)"					   ")",	//  1   1   1   ·
      [0b1111] = "",												//  1   1   1   1
     };
   char *AssignSubQuery;
   char *HidVisSubQuery;
   char *ReviewSubQuery;
   char *DptCodSubQuery;
   static const char *OrderBySubQuery[Prj_NUM_ORDERS] =
     {
      [Prj_ORDER_START_TIME] = "prj_projects.CreatTime DESC,"
			       "prj_projects.ModifTime DESC,"
			       "prj_projects.Title",
      [Prj_ORDER_END_TIME  ] = "prj_projects.ModifTime DESC,"
			       "prj_projects.CreatTime DESC,"
			       "prj_projects.Title",
      [Prj_ORDER_TITLE     ] = "prj_projects.Title,"
			       "prj_projects.CreatTime DESC,"
			       "prj_projects.ModifTime DESC",
      [Prj_ORDER_DEPARTMENT] = "dpt_departments.FullName,"
			       "prj_projects.CreatTime DESC,"
			       "prj_projects.ModifTime DESC,"
			       "prj_projects.Title",
     };
   unsigned NumPrjsFromDB = 0;

   /* Assigned subquery */
   switch (Projects->Filter.Assign)
     {
      case (1 << Prj_NONASSIG):	// Non-assigned projects
	 if (asprintf (&AssignSubQuery," AND prj_projects.Assigned='N'") < 0)
	    Err_NotEnoughMemoryExit ();
	 break;
      case (1 << Prj_ASSIGNED):	// Assigned projects
	 if (asprintf (&AssignSubQuery," AND prj_projects.Assigned='Y'") < 0)
	    Err_NotEnoughMemoryExit ();
	 break;
      default:			// All projects
	 if (asprintf (&AssignSubQuery,"%s","") < 0)
	    Err_NotEnoughMemoryExit ();
	 break;
     }

   /* Hidden subquery */
   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_STD:	// Students can view only visible projects
	 if (asprintf (&HidVisSubQuery," AND prj_projects.Hidden='N'") < 0)
	    Err_NotEnoughMemoryExit ();
	 break;
      case Rol_NET:
      case Rol_TCH:
      case Rol_SYS_ADM:
	 switch (Projects->Filter.Hidden)
	   {
	    case (1 << HidVis_HIDDEN):	// Hidden projects
	       if (asprintf (&HidVisSubQuery," AND prj_projects.Hidden='Y'") < 0)
		  Err_NotEnoughMemoryExit ();
	       break;
	    case (1 << HidVis_VISIBLE):	// Visible projects
	       if (asprintf (&HidVisSubQuery," AND prj_projects.Hidden='N'") < 0)
		  Err_NotEnoughMemoryExit ();
	       break;
	    default:			// All projects
	       if (asprintf (&HidVisSubQuery,"%s","") < 0)
		  Err_NotEnoughMemoryExit ();
	       break;
	   }
	 break;
      default:
	 Err_WrongRoleExit ();
	 break;
     }

   /* Review status subquery */
   switch (Projects->Filter.Review)
     {
      case (1 << Prj_UNREVIEWED):	// Unreviewed projects
	 if (asprintf (&ReviewSubQuery," AND prj_projects.ReviewStatus='%s'",
	               Prj_DB_ReviewStatus[Prj_UNREVIEWED]) < 0)
	    Err_NotEnoughMemoryExit ();
	 break;
      case (1 << Prj_UNAPPROVED):	// Unapproved projects
	 if (asprintf (&ReviewSubQuery," AND prj_projects.ReviewStatus='%s'",
	               Prj_DB_ReviewStatus[Prj_UNAPPROVED]) < 0)
	    Err_NotEnoughMemoryExit ();
	 break;
      case (1 << Prj_APPROVED):		// Approved projects
	 if (asprintf (&ReviewSubQuery," AND prj_projects.ReviewStatus='%s'",
	               Prj_DB_ReviewStatus[Prj_APPROVED  ]) < 0)
	    Err_NotEnoughMemoryExit ();
	 break;
      case (1 << Prj_UNREVIEWED |
	    1 << Prj_UNAPPROVED):	// Unreviewed and unapproved projects
	 if (asprintf (&ReviewSubQuery," AND prj_projects.ReviewStatus IN ('%s','%s')",
	               Prj_DB_ReviewStatus[Prj_UNREVIEWED],
	               Prj_DB_ReviewStatus[Prj_UNAPPROVED]) < 0)
	    Err_NotEnoughMemoryExit ();
	 break;
      case (1 << Prj_UNREVIEWED |
	    1 << Prj_APPROVED):		// Unreviewed and approved projects
	 if (asprintf (&ReviewSubQuery," AND prj_projects.ReviewStatus IN ('%s','%s')",
	               Prj_DB_ReviewStatus[Prj_UNREVIEWED],
	               Prj_DB_ReviewStatus[Prj_APPROVED  ]) < 0)
	    Err_NotEnoughMemoryExit ();
	 break;
      case (1 << Prj_UNAPPROVED |
	    1 << Prj_APPROVED):		// Unapproved and approved projects
	 if (asprintf (&ReviewSubQuery," AND prj_projects.ReviewStatus IN ('%s','%s')",
	               Prj_DB_ReviewStatus[Prj_UNAPPROVED],
	               Prj_DB_ReviewStatus[Prj_APPROVED  ]) < 0)
	    Err_NotEnoughMemoryExit ();
	 break;
      default:				// All projects
	 if (asprintf (&ReviewSubQuery,"%s","") < 0)
	    Err_NotEnoughMemoryExit ();
	 break;
     }

   /* Department subquery */
   if (Projects->Filter.DptCod >= 0)
     {
      if (asprintf (&DptCodSubQuery," AND prj_projects.DptCod=%ld",
		    Projects->Filter.DptCod) < 0)
	 Err_NotEnoughMemoryExit ();
     }
   else	// Any department
     {
      if (asprintf (&DptCodSubQuery,"%s","") < 0)
	 Err_NotEnoughMemoryExit ();
     }

   /* Query */
   switch (Projects->Filter.Who)
     {
      case Usr_WHO_NONE:
	 /* Get list of projects without users */
	 switch (Projects->SelectedOrder)
	   {
	    case Prj_ORDER_START_TIME:
	    case Prj_ORDER_END_TIME:
	    case Prj_ORDER_TITLE:
	       NumPrjsFromDB = (unsigned)
	       DB_QuerySELECT (mysql_res,"can not get projects",
			       "SELECT prj_projects.PrjCod"
				" FROM prj_projects"
			       " WHERE prj_projects.CrsCod=%ld"
				 "%s"
				 "%s"
				 "%s"
				 "%s"
				 " AND prj_projects.PrjCod NOT IN (SELECT PrjCod FROM prj_users)"
			    " GROUP BY prj_projects.PrjCod"	// To not repeat projects (DISTINCT can not be used)
			    " ORDER BY %s",
			       Gbl.Hierarchy.Node[Hie_CRS].HieCod,
			       AssignSubQuery,
			       HidVisSubQuery,
			       ReviewSubQuery,
			       DptCodSubQuery,
			       OrderBySubQuery[Projects->SelectedOrder]);
	       break;
	    case Prj_ORDER_DEPARTMENT:
	       NumPrjsFromDB = (unsigned)
	       DB_QuerySELECT (mysql_res,"can not get projects",
			       "SELECT prj_projects.PrjCod"
				" FROM prj_projects LEFT JOIN dpt_departments"
				  " ON prj_projects.DptCod=dpt_departments.DptCod"
	                                          " LEFT JOIN prj_users"
				  " ON prj_projects.PrjCod=prj_users.PrjCod"
			       " WHERE prj_projects.CrsCod=%ld"
				 "%s"
				 "%s"
				 "%s"
				 "%s"
				 " AND prj_projects.PrjCod=prj_users.PrjCod"
				 " AND prj_users.UsrCod=%ld"
		                 "%s"
			    " GROUP BY prj_projects.PrjCod"	// To not repeat projects (DISTINCT can not be used)
			    " ORDER BY %s",
			       Gbl.Hierarchy.Node[Hie_CRS].HieCod,
			       AssignSubQuery,
			       HidVisSubQuery,
			       ReviewSubQuery,
			       DptCodSubQuery,
			       Gbl.Usrs.Me.UsrDat.UsrCod,
			       RolPrjSubQuery[Projects->Filter.RolPrj],
			       OrderBySubQuery[Projects->SelectedOrder]);
	       break;
	   }
	 break;
      case Usr_WHO_ME:	 /* Get list of my projects */
	 switch (Projects->SelectedOrder)
	   {
	    case Prj_ORDER_START_TIME:
	    case Prj_ORDER_END_TIME:
	    case Prj_ORDER_TITLE:
	       NumPrjsFromDB = (unsigned)
	       DB_QuerySELECT (mysql_res,"can not get projects",
			       "SELECT prj_projects.PrjCod"
				" FROM prj_projects,"
				      "prj_users"
			       " WHERE prj_projects.CrsCod=%ld"
				 "%s"
				 "%s"
				 "%s"
				 "%s"
				 " AND prj_projects.PrjCod=prj_users.PrjCod"
				 " AND prj_users.UsrCod=%ld"
		                 "%s"
			    " GROUP BY prj_projects.PrjCod"	// To not repeat projects (DISTINCT can not be used)
			    " ORDER BY %s",
			       Gbl.Hierarchy.Node[Hie_CRS].HieCod,
			       AssignSubQuery,
			       HidVisSubQuery,
			       ReviewSubQuery,
			       DptCodSubQuery,
			       Gbl.Usrs.Me.UsrDat.UsrCod,
			       RolPrjSubQuery[Projects->Filter.RolPrj],
			       OrderBySubQuery[Projects->SelectedOrder]);
	       break;
	    case Prj_ORDER_DEPARTMENT:
	       NumPrjsFromDB = (unsigned)
	       DB_QuerySELECT (mysql_res,"can not get projects",
			       "SELECT prj_projects.PrjCod"
				" FROM prj_projects LEFT JOIN dpt_departments"
				  " ON prj_projects.DptCod=dpt_departments.DptCod"
	                                          " LEFT JOIN prj_users"
				  " ON prj_projects.PrjCod=prj_users.PrjCod"
			       " WHERE prj_projects.CrsCod=%ld"
				 "%s"
				 "%s"
				 "%s"
				 "%s"
				 " AND prj_projects.PrjCod=prj_users.PrjCod"
				 " AND prj_users.UsrCod=%ld"
		                 "%s"
			    " GROUP BY prj_projects.PrjCod"	// To not repeat projects (DISTINCT can not be used)
			    " ORDER BY %s",
			       Gbl.Hierarchy.Node[Hie_CRS].HieCod,
			       AssignSubQuery,
			       HidVisSubQuery,
			       ReviewSubQuery,
			       DptCodSubQuery,
			       Gbl.Usrs.Me.UsrDat.UsrCod,
			       RolPrjSubQuery[Projects->Filter.RolPrj],
			       OrderBySubQuery[Projects->SelectedOrder]);
	       break;
	   }
	 break;
      case Usr_WHO_SELECTED:	    /* Get list of projects associated to selected users */
	 if (UsrsSubQuery)
	   {
	    switch (Projects->SelectedOrder)
	      {
	       case Prj_ORDER_START_TIME:
	       case Prj_ORDER_END_TIME:
	       case Prj_ORDER_TITLE:
		  NumPrjsFromDB = (unsigned)
		  DB_QuerySELECT (mysql_res,"can not get projects",
				  "SELECT prj_projects.PrjCod"
				   " FROM prj_projects,"
					 "prj_users"
				  " WHERE prj_projects.CrsCod=%ld"
				    "%s"
				    "%s"
				    "%s"
				    "%s"
				    " AND prj_projects.PrjCod=prj_users.PrjCod"
				    " AND prj_users.UsrCod IN (%s)"
		                    "%s"
			       " GROUP BY prj_projects.PrjCod"	// To not repeat projects (DISTINCT can not be used)
			       " ORDER BY %s",
				  Gbl.Hierarchy.Node[Hie_CRS].HieCod,
				  AssignSubQuery,
				  HidVisSubQuery,
			          ReviewSubQuery,
				  DptCodSubQuery,
				  UsrsSubQuery,
			          RolPrjSubQuery[Projects->Filter.RolPrj],
				  OrderBySubQuery[Projects->SelectedOrder]);
		  break;
	       case Prj_ORDER_DEPARTMENT:
		  NumPrjsFromDB = (unsigned)
		  DB_QuerySELECT (mysql_res,"can not get projects",
				  "SELECT prj_projects.PrjCod"
				   " FROM prj_projects LEFT JOIN dpt_departments"
				     " ON prj_projects.DptCod=dpt_departments.DptCod"
	                                             " LEFT JOIN prj_users"
				     " ON prj_projects.PrjCod=prj_users.PrjCod"
				  " WHERE prj_projects.CrsCod=%ld"
				      "%s"
				      "%s"
				      "%s"
				      "%s"
				    " AND prj_projects.PrjCod=prj_users.PrjCod"
				    " AND prj_users.UsrCod IN (%s)"
		                    "%s"
			       " GROUP BY prj_projects.PrjCod"	// To not repeat projects (DISTINCT can not be used)
			       " ORDER BY %s",
				  Gbl.Hierarchy.Node[Hie_CRS].HieCod,
				  AssignSubQuery,
				  HidVisSubQuery,
			          ReviewSubQuery,
				  DptCodSubQuery,
				  UsrsSubQuery,
			          RolPrjSubQuery[Projects->Filter.RolPrj],
				  OrderBySubQuery[Projects->SelectedOrder]);
		  break;
	      }
	   }
	 break;
      case Usr_WHO_ALL:	 /* Get list of projects associated to any user */
	 switch (Projects->SelectedOrder)
	   {
	    case Prj_ORDER_START_TIME:
	    case Prj_ORDER_END_TIME:
	    case Prj_ORDER_TITLE:
	       NumPrjsFromDB = (unsigned)
	       DB_QuerySELECT (mysql_res,"can not get projects",
			       "SELECT prj_projects.PrjCod"
				" FROM prj_projects LEFT JOIN prj_users"
				  " ON prj_projects.PrjCod=prj_users.PrjCod"
			       " WHERE prj_projects.CrsCod=%ld"
				   "%s"
				   "%s"
				   "%s"
				   "%s"
		                   "%s"
			    " GROUP BY prj_projects.PrjCod"	// To not repeat projects (DISTINCT can not be used)
			    " ORDER BY %s",
			       Gbl.Hierarchy.Node[Hie_CRS].HieCod,
			       AssignSubQuery,
			       HidVisSubQuery,
			       ReviewSubQuery,
			       DptCodSubQuery,
			       RolPrjSubQuery[Projects->Filter.RolPrj],
			       OrderBySubQuery[Projects->SelectedOrder]);
	       break;
	    case Prj_ORDER_DEPARTMENT:
	       NumPrjsFromDB = (unsigned)
	       DB_QuerySELECT (mysql_res,"can not get projects",
			       "SELECT prj_projects.PrjCod"
				" FROM prj_projects LEFT JOIN dpt_departments"
				  " ON prj_projects.DptCod=dpt_departments.DptCod"
	                                          " LEFT JOIN prj_users"
				  " ON prj_projects.PrjCod=prj_users.PrjCod"
			       " WHERE prj_projects.CrsCod=%ld"
				   "%s"
				   "%s"
				   "%s"
				   "%s"
		                   "%s"
			    " GROUP BY prj_projects.PrjCod"	// To not repeat projects (DISTINCT can not be used)
			    " ORDER BY %s",
			       Gbl.Hierarchy.Node[Hie_CRS].HieCod,
			       AssignSubQuery,
			       HidVisSubQuery,
			       ReviewSubQuery,
			       DptCodSubQuery,
			       RolPrjSubQuery[Projects->Filter.RolPrj],
			       OrderBySubQuery[Projects->SelectedOrder]);
	       break;
	   }
	 break;
      default:
	 Err_WrongWhoExit ();
	 break;
     }

   /* Free allocated memory for subqueries */
   free (DptCodSubQuery);
   free (ReviewSubQuery);
   free (HidVisSubQuery);
   free (AssignSubQuery);

   return NumPrjsFromDB;
  }

/*****************************************************************************/
/********************* Get project data using its code ***********************/
/*****************************************************************************/

unsigned Prj_DB_GetProjectDataByCod (MYSQL_RES **mysql_res,long PrjCod)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get project data",
		   "SELECT PrjCod,"			// row[ 0]
			  "CrsCod,"			// row[ 1]
			  "DptCod,"			// row[ 2]
			  "Locked,"			// row[ 3]
			  "Hidden,"			// row[ 4]
			  "Assigned,"			// row[ 5]
			  "NumStds,"			// row[ 6]
			  "Proposal,"			// row[ 7]
			  "UNIX_TIMESTAMP(CreatTime),"	// row[ 8]
			  "UNIX_TIMESTAMP(ModifTime),"	// row[ 9]
			  "Title,"			// row[10]
			  "Description,"		// row[11]
			  "Knowledge,"			// row[12]
			  "Materials,"			// row[13]
			  "URL,"			// row[14]
			  "ReviewStatus,"		// row[15]
			  "UNIX_TIMESTAMP(ReviewTime),"	// row[16]
			  "ReviewTxt"			// row[17]
		    " FROM prj_projects"
		   " WHERE PrjCod=%ld"
		     " AND CrsCod=%ld",	// Extra check
		   PrjCod,
		   Gbl.Hierarchy.Node[Hie_CRS].HieCod);
  }

/*****************************************************************************/
/********************* Get project title from database ***********************/
/*****************************************************************************/

void Prj_DB_GetProjectTitle (long PrjCod,char *Title,size_t TitleSize)
  {
   DB_QuerySELECTString (Title,TitleSize,"can not get project title",
		         "SELECT Title"	// row[0]
			  " FROM prj_projects"
		         " WHERE PrjCod=%ld"
		           " AND CrsCod=%ld",	// Extra check
			 PrjCod,
			 Gbl.Hierarchy.Node[Hie_CRS].HieCod);
  }

/*****************************************************************************/
/******************* Get some project data to check faults *******************/
/*****************************************************************************/

unsigned Prj_DB_GetPrjDataToCheckFaults (MYSQL_RES **mysql_res,long PrjCod)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get project data",
		   "SELECT Assigned='Y',"					// row[0] = 0 / 1
			  "NumStds,"						// row[1] =
			  "Title<>'',"						// row[2] = 0 / 1
			  "Description<>'',"					// row[3] = 0 / 1
			  "ReviewStatus='%s',"					// row[4] = 0 / 1
			  "ReviewStatus<>'%s' AND ModifTime>ReviewTime"		// row[5] = 0 / 1
		    " FROM prj_projects"
		   " WHERE PrjCod=%ld",
		   Prj_DB_ReviewStatus[Prj_UNAPPROVED],
		   Prj_DB_ReviewStatus[Prj_UNREVIEWED],
		   PrjCod);
  }

/*****************************************************************************/
/*************** Get number of users with a role in a project ****************/
/*****************************************************************************/

unsigned Prj_DB_GetNumUsrsInPrj (long PrjCod,Prj_RoleInProject_t RoleInProject)
  {
   return (unsigned)
   DB_QueryCOUNT ("can not get number of users in project",
		  "SELECT COUNT(UsrCod)"
		   " FROM prj_users"
		  " WHERE PrjCod=%ld"
		    " AND RoleInProject=%u",
		  PrjCod,
		  (unsigned) RoleInProject);
  }

/*****************************************************************************/
/******************** Get users with a role in a project *********************/
/*****************************************************************************/

unsigned Prj_DB_GetUsrsInPrj (MYSQL_RES **mysql_res,
                              long PrjCod,Prj_RoleInProject_t RoleInProject)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get users in project",
		   "SELECT prj_users.UsrCod,"		// row[0]
			  "usr_data.Surname1 AS S1,"	// row[1]
			  "usr_data.Surname2 AS S2,"	// row[2]
			  "usr_data.FirstName AS FN"	// row[3]
		    " FROM prj_users,"
			  "usr_data"
		   " WHERE prj_users.PrjCod=%ld"
		     " AND prj_users.RoleInProject=%u"
		     " AND prj_users.UsrCod=usr_data.UsrCod"
		" ORDER BY S1,"
			  "S2,"
			  "FN",
		   PrjCod,
		   (unsigned) RoleInProject);
  }

/*****************************************************************************/
/************************ Get my roles in a project **************************/
/*****************************************************************************/

unsigned Prj_DB_GetMyRolesInPrj (MYSQL_RES **mysql_res,long PrjCod)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get my roles in project",
		   "SELECT RoleInProject"	// row[0]
		    " FROM prj_users"
		   " WHERE PrjCod=%ld"
		     " AND UsrCod=%ld",
		   PrjCod,
		   Gbl.Usrs.Me.UsrDat.UsrCod);
  }

/*****************************************************************************/
/************************** Get course of a project **************************/
/*****************************************************************************/

long Prj_DB_GetCrsOfPrj (long PrjCod)
  {
   /***** Trivial check: project code should be > 0 *****/
   if (PrjCod <= 0)
      return -1L;

   /***** Get course code from database *****/
   return DB_QuerySELECTCode ("can not get project course",
			      "SELECT CrsCod"		// row[0]
			       " FROM prj_projects"
			      " WHERE PrjCod=%ld",
			      PrjCod); // Project found...
  }

/*****************************************************************************/
/******************** Get number of courses with projects ********************/
/*****************************************************************************/
// Returns the number of courses with projects
// in this location (all the platform, current degree or current course)

unsigned Prj_DB_GetNumCoursesWithProjects (Hie_Level_t Level)
  {
   /***** Get number of courses with projects from database *****/
   switch (Level)
     {
      case Hie_SYS:
	 return (unsigned)
         DB_QueryCOUNT ("can not get number of courses with projects",
			"SELECT COUNT(DISTINCT CrsCod)"
			 " FROM prj_projects"
			" WHERE CrsCod>0");
      case Hie_CTY:
         return (unsigned)
         DB_QueryCOUNT ("can not get number of courses with projects",
			"SELECT COUNT(DISTINCT prj_projects.CrsCod)"
			 " FROM ins_instits,"
			       "ctr_centers,"
			       "deg_degrees,"
			       "crs_courses,"
			       "prj_projects"
			" WHERE ins_instits.CtyCod=%ld"
			  " AND ins_instits.InsCod=ctr_centers.InsCod"
			  " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
			  " AND deg_degrees.DegCod=crs_courses.DegCod"
			  " AND crs_courses.CrsCod=prj_projects.CrsCod",
                        Gbl.Hierarchy.Node[Hie_CTY].HieCod);
      case Hie_INS:
         return (unsigned)
         DB_QueryCOUNT ("can not get number of courses with projects",
			"SELECT COUNT(DISTINCT prj_projects.CrsCod)"
			 " FROM ctr_centers,"
			       "deg_degrees,"
			       "crs_courses,"
			       "prj_projects"
			" WHERE ctr_centers.InsCod=%ld"
			  " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
			  " AND deg_degrees.DegCod=crs_courses.DegCod"
			  " AND crs_courses.CrsCod=prj_projects.CrsCod",
                        Gbl.Hierarchy.Node[Hie_INS].HieCod);
      case Hie_CTR:
         return (unsigned)
         DB_QueryCOUNT ("can not get number of courses with projects",
			"SELECT COUNT(DISTINCT prj_projects.CrsCod)"
			 " FROM deg_degrees,"
			       "crs_courses,"
			      "prj_projects"
			" WHERE deg_degrees.CtrCod=%ld"
			  " AND deg_degrees.DegCod=crs_courses.DegCod"
			  " AND crs_courses.CrsCod=prj_projects.CrsCod",
                        Gbl.Hierarchy.Node[Hie_CTR].HieCod);
      case Hie_DEG:
         return (unsigned)
         DB_QueryCOUNT ("can not get number of courses with projects",
			"SELECT COUNT(DISTINCT prj_projects.CrsCod)"
			 " FROM crs_courses,"
			       "prj_projects"
			" WHERE crs_courses.DegCod=%ld"
			  " AND crs_courses.CrsCod=prj_projects.CrsCod",
                        Gbl.Hierarchy.Node[Hie_DEG].HieCod);
      case Hie_CRS:
         return (unsigned)
         DB_QueryCOUNT ("can not get number of courses with projects",
			"SELECT COUNT(DISTINCT CrsCod)"
			 " FROM prj_projects"
			" WHERE CrsCod=%ld",
			Gbl.Hierarchy.Node[Hie_CRS].HieCod);
      default:
	 Err_WrongHierarchyLevelExit ();
	 return 0;	// Not reached
     }
  }

/*****************************************************************************/
/************************** Get number of projects ***************************/
/*****************************************************************************/
// Returns the number of projects in this location

unsigned Prj_DB_GetNumProjects (Hie_Level_t Level)
  {
   /***** Get number of projects from database *****/
   switch (Level)
     {
      case Hie_SYS:
         return (unsigned)
         DB_QueryCOUNT ("can not get number of projects",
			"SELECT COUNT(*)"
                         " FROM prj_projects"
                        " WHERE CrsCod>0");
      case Hie_CTY:
         return (unsigned)
         DB_QueryCOUNT ("can not get number of projects",
			"SELECT COUNT(*)"
			 " FROM ins_instits,"
			       "ctr_centers,"
			       "deg_degrees,"
			       "crs_courses,"
			       "prj_projects"
			" WHERE ins_instits.CtyCod=%ld"
			  " AND ins_instits.InsCod=ctr_centers.InsCod"
			  " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
			  " AND deg_degrees.DegCod=crs_courses.DegCod"
			  " AND crs_courses.CrsCod=prj_projects.CrsCod",
                        Gbl.Hierarchy.Node[Hie_CTY].HieCod);
      case Hie_INS:
         return (unsigned)
         DB_QueryCOUNT ("can not get number of projects",
			"SELECT COUNT(*)"
			 " FROM ctr_centers,"
			       "deg_degrees,"
			       "crs_courses,"
			       "prj_projects"
			" WHERE ctr_centers.InsCod=%ld"
			  " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
			  " AND deg_degrees.DegCod=crs_courses.DegCod"
			  " AND crs_courses.CrsCod=prj_projects.CrsCod",
                        Gbl.Hierarchy.Node[Hie_INS].HieCod);
      case Hie_CTR:
         return (unsigned)
         DB_QueryCOUNT ("can not get number of projects",
			"SELECT COUNT(*)"
			 " FROM deg_degrees,"
			       "crs_courses,"
			       "prj_projects"
			" WHERE deg_degrees.CtrCod=%ld"
			  " AND deg_degrees.DegCod=crs_courses.DegCod"
			  " AND crs_courses.CrsCod=prj_projects.CrsCod",
                        Gbl.Hierarchy.Node[Hie_CTR].HieCod);
      case Hie_DEG:
         return (unsigned)
         DB_QueryCOUNT ("can not get number of projects",
			"SELECT COUNT(*)"
			 " FROM crs_courses,"
			       "prj_projects"
			" WHERE crs_courses.DegCod=%ld"
			  " AND crs_courses.CrsCod=prj_projects.CrsCod",
                        Gbl.Hierarchy.Node[Hie_DEG].HieCod);
      case Hie_CRS:
         return (unsigned)
         DB_QueryCOUNT ("can not get number of projects",
			"SELECT COUNT(*)"
			 " FROM prj_projects"
			" WHERE CrsCod=%ld",
                        Gbl.Hierarchy.Node[Hie_CRS].HieCod);
      default:
	 Err_WrongHierarchyLevelExit ();
	 return 0;	// Not reached
     }
  }

/*****************************************************************************/
/************************ Remove user from a project *************************/
/*****************************************************************************/

void Prj_DB_RemoveUsrFromPrj (long PrjCod,Prj_RoleInProject_t RoleInPrj,long UsrCod)
  {
   DB_QueryDELETE ("can not remove a user from a project",
		   "DELETE FROM prj_users"
		   " WHERE PrjCod=%ld"
		     " AND RoleInProject=%u"
		     " AND UsrCod=%ld",
		   PrjCod,
		   (unsigned) RoleInPrj,
		   UsrCod);
  }

/*****************************************************************************/
/******************* Remove user from all his/her projects *******************/
/*****************************************************************************/

void Prj_DB_RemoveUsrFromProjects (long UsrCod)
  {
   DB_QueryDELETE ("can not remove user from projects",
		   "DELETE FROM prj_users"
		   " WHERE UsrCod=%ld",
		   UsrCod);
  }

/*****************************************************************************/
/********************* Remove all users from a project ***********************/
/*****************************************************************************/

void Prj_DB_RemoveUsrsFromPrj (long PrjCod)
  {
   DB_QueryDELETE ("can not remove project",
		   "DELETE FROM prj_users"
		   " USING prj_projects,"
			  "prj_users"
		   " WHERE prj_projects.PrjCod=%ld"
		     " AND prj_projects.CrsCod=%ld"	// Extra check
		     " AND prj_projects.PrjCod=prj_users.PrjCod",
		   PrjCod,
		   Gbl.Hierarchy.Node[Hie_CRS].HieCod);
  }

/*****************************************************************************/
/************** Remove all users from all projects in a course ***************/
/*****************************************************************************/

void Prj_DB_RemoveUsrsFromCrsPrjs (long CrsCod)
  {
   DB_QueryDELETE ("can not remove all projects in a course",
		   "DELETE FROM prj_users"
		   " USING prj_projects,"
		          "prj_users"
		   " WHERE prj_projects.CrsCod=%ld"
		     " AND prj_projects.PrjCod=prj_users.PrjCod",
                   CrsCod);
  }

/*****************************************************************************/
/*************** Remove configuration of projects in the course **************/
/*****************************************************************************/

void Prj_DB_RemoveConfigOfCrsPrjs (long CrsCod)
  {
   DB_QueryDELETE ("can not remove configuration of projects in a course",
		   "DELETE FROM prj_config"
		   " WHERE CrsCod=%ld",
		   CrsCod);
  }

/*****************************************************************************/
/********** Remove associations of rubrics to projects in the course *********/
/*****************************************************************************/

void Prj_DB_RemoveRubricsOfCrsPrjs (long CrsCod)
  {
   DB_QueryDELETE ("can not remove rubrics of projects in a course",
		   "DELETE FROM prj_rubrics"
		   " WHERE CrsCod=%ld",
		   CrsCod);
  }

/*****************************************************************************/
/******************************* Remove project ******************************/
/*****************************************************************************/

void Prj_DB_RemovePrj (long PrjCod)
  {
   DB_QueryDELETE ("can not remove project",
		   "DELETE FROM prj_projects"
		   " WHERE PrjCod=%ld"
		     " AND CrsCod=%ld",	// Extra check
		   PrjCod,
		   Gbl.Hierarchy.Node[Hie_CRS].HieCod);
  }

/*****************************************************************************/
/********************* Remove all projects in a course ***********************/
/*****************************************************************************/

void Prj_DB_RemoveCrsPrjs (long CrsCod)
  {
   DB_QueryDELETE ("can not remove all projects of a course",
		   "DELETE FROM prj_projects"
		   " WHERE CrsCod=%ld",
		   CrsCod);
  }

/*****************************************************************************/
/********** Update whether non-editing teachers can create projects **********/
/*****************************************************************************/

void Prj_DB_UpdateNETCanCreate (const struct Prj_Projects *Projects)
  {
   DB_QueryREPLACE ("can not save configuration of projects",
		    "REPLACE INTO prj_config"
	            " (CrsCod,NETCanCreate)"
                    " VALUES"
                    " (%ld,'%c')",
		    Gbl.Hierarchy.Node[Hie_CRS].HieCod,
		    Projects->Config.NETCanCreate ? 'Y' :
			                            'N');
  }

/*****************************************************************************/
/********* Update rubrics associated to projects for current course **********/
/*****************************************************************************/

void Prj_DB_UpdateRubrics (PrjCfg_RubricType_t RubricType,
                           const struct PrgCfg_ListRubCods *ListRubCods)
  {
   unsigned RubCod;

   /***** Delete all rubric codes of this type *****/
   DB_QueryDELETE ("can not save configuration of projects",
		   "DELETE FROM prj_rubrics"
		        " WHERE CrsCod=%ld"
		          " AND Type='%s'",
		   Gbl.Hierarchy.Node[Hie_CRS].HieCod,
		   Prj_DB_RubricType[RubricType]);

   /***** Insert specified rubric codes of this type *****/
   for (RubCod  = 0;
	RubCod <= ListRubCods->NumRubrics;
	RubCod++)
      DB_QueryREPLACE ("can not save configuration of projects",
		       "REPLACE INTO prj_rubrics"
		       " (CrsCod,Type,RubCod)"
		       " VALUES"
		       " (%ld,'%s',%ld)",
		       Gbl.Hierarchy.Node[Hie_CRS].HieCod,
		       Prj_DB_RubricType[RubricType],
		       ListRubCods->RubCods[RubCod]);
  }

/*****************************************************************************/
/************** Get configuration of projects for current course *************/
/*****************************************************************************/

unsigned Prj_DB_GetConfig (MYSQL_RES **mysql_res)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get project configuration",
		   "SELECT NETCanCreate"	// row[0]
		    " FROM prj_config"
		   " WHERE CrsCod=%ld",
		   Gbl.Hierarchy.Node[Hie_CRS].HieCod);
  }

/*****************************************************************************/
/******************* Get project rubrics for current course ******************/
/*****************************************************************************/

unsigned Prj_DB_GetRubricsOfType (MYSQL_RES **mysql_res,
                                  PrjCfg_RubricType_t RubricType)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get project rubrics",
		   "SELECT prj_rubrics.RubCod"	// row[0]
		    " FROM prj_rubrics,"
		          "rub_rubrics"
		   " WHERE prj_rubrics.CrsCod=%ld"
		     " AND prj_rubrics.Type='%s'"
		     " AND prj_rubrics.RubCod=rub_rubrics.RubCod"
		" ORDER BY rub_rubrics.Title",
		   Gbl.Hierarchy.Node[Hie_CRS].HieCod,
		   Prj_DB_RubricType[RubricType]);
  }

/*****************************************************************************/
/*** Get type of project rubric in the current course given a rubric code ****/
/*****************************************************************************/

PrjCfg_RubricType_t Prj_DB_GetWichRubricFromRubCod (long RubCod)
  {
   char StrTypeDB[32];

   DB_QuerySELECTString (StrTypeDB,sizeof (StrTypeDB) - 1,
                         "can not get which rubric",
			 "SELECT Type"	// row[0]
			  " FROM prj_rubrics"
			 " WHERE CrsCod=%ld"
			   " AND RubCod=%ld",
			 Gbl.Hierarchy.Node[Hie_CRS].HieCod,
			 RubCod);
   return PrjCfg_GetRubricFromString (StrTypeDB);
  }
