// swad_degree.c: degrees

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

#include <stdbool.h>		// For boolean type
#include <stddef.h>		// For NULL
#include <stdlib.h>		// For free
#include <string.h>		// For string functions

#include "swad_database.h"
#include "swad_degree.h"
#include "swad_degree_config.h"
#include "swad_figure.h"
#include "swad_figure_cache.h"
#include "swad_form.h"
#include "swad_forum.h"
#include "swad_global.h"
#include "swad_hierarchy.h"
#include "swad_HTML.h"
#include "swad_logo.h"
#include "swad_message.h"
#include "swad_survey.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/*************************** Public constants ********************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private types *********************************/
/*****************************************************************************/

typedef enum
  {
   Deg_FIRST_YEAR,
   Deg_LAST_YEAR,
  } Deg_FirstOrLastYear_t;

/*****************************************************************************/
/**************************** Private variables ******************************/
/*****************************************************************************/

static struct Deg_Degree *Deg_EditingDeg = NULL;	// Static variable to keep the degree being edited

/*****************************************************************************/
/**************************** Private prototypes *****************************/
/*****************************************************************************/

static void Deg_ListDegreesForEdition (void);
static bool Deg_CheckIfICanEditADegree (struct Deg_Degree *Deg);
static Deg_StatusTxt_t Deg_GetStatusTxtFromStatusBits (Deg_Status_t Status);
static Deg_Status_t Deg_GetStatusBitsFromStatusTxt (Deg_StatusTxt_t StatusTxt);
static void Deg_PutFormToCreateDegree (void);
static void Deg_PutHeadDegreesForSeeing (void);
static void Deg_PutHeadDegreesForEdition (void);
static void Deg_CreateDegree (unsigned Status);

static void Deg_ListDegrees (void);
static bool Deg_CheckIfICanCreateDegrees (void);
static void Deg_PutIconsListingDegrees (__attribute__((unused)) void *Args);
static void Deg_PutIconToEditDegrees (void);
static void Deg_ListOneDegreeForSeeing (struct Deg_Degree *Deg,unsigned NumDeg);

static void Deg_EditDegreesInternal (void);
static void Deg_PutIconsEditingDegrees (__attribute__((unused)) void *Args);

static void Deg_ReceiveFormRequestOrCreateDeg (unsigned Status);
static void Deg_PutParamOtherDegCod (void *DegCod);

static void Deg_GetDataOfDegreeFromRow (struct Deg_Degree *Deg,MYSQL_ROW row);

static void Deg_UpdateDegNameDB (long DegCod,const char *FieldName,const char *NewDegName);

static void Deg_ShowAlertAndButtonToGoToDeg (void);
static void Deg_PutParamGoToDeg (void *DegCod);

static void Deg_EditingDegreeConstructor (void);
static void Deg_EditingDegreeDestructor (void);

/*****************************************************************************/
/******************* List degrees with pending courses ***********************/
/*****************************************************************************/

void Deg_SeeDegWithPendingCrss (void)
  {
   extern const char *Hlp_SYSTEM_Pending;
   extern const char *Txt_Degrees_with_pending_courses;
   extern const char *Txt_Degree;
   extern const char *Txt_Courses_ABBREVIATION;
   extern const char *Txt_There_are_no_degrees_with_requests_for_courses_to_be_confirmed;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumDegs;
   unsigned NumDeg;
   struct Deg_Degree Deg;
   const char *BgColor;

   /***** Get degrees with pending courses *****/
   switch (Gbl.Usrs.Me.Role.Logged)
     {
      case Rol_DEG_ADM:
         NumDegs = (unsigned)
         DB_QuerySELECT (&mysql_res,"can not get degrees with pending courses",
			 "SELECT crs_courses.DegCod,"
			        "COUNT(*)"
			 " FROM usr_admins,"
			       "crs_courses,"
			       "deg_degrees"
			 " WHERE usr_admins.UsrCod=%ld"
			 " AND usr_admins.Scope='%s'"
			 " AND usr_admins.Cod=crs_courses.DegCod"
			 " AND (crs_courses.Status & %u)<>0"
			 " AND crs_courses.DegCod=deg_degrees.DegCod"
			 " GROUP BY crs_courses.DegCod"
			 " ORDER BY deg_degrees.ShortName",
			 Gbl.Usrs.Me.UsrDat.UsrCod,
			 Sco_GetDBStrFromScope (Hie_Lvl_DEG),
			 (unsigned) Crs_STATUS_BIT_PENDING);
         break;
      case Rol_SYS_ADM:
         NumDegs = (unsigned)
         DB_QuerySELECT (&mysql_res,"can not get degrees with pending courses",
			 "SELECT crs_courses.DegCod,"
			        "COUNT(*)"
			 " FROM crs_courses,"
			       "deg_degrees"
			 " WHERE (crs_courses.Status & %u)<>0"
			 " AND crs_courses.DegCod=deg_degrees.DegCod"
			 " GROUP BY crs_courses.DegCod"
			 " ORDER BY deg_degrees.ShortName",
			 (unsigned) Crs_STATUS_BIT_PENDING);
         break;
      default:	// Forbidden for other users
	 return;
     }

   /***** Get degrees *****/
   if (NumDegs)
     {
      /***** Begin box and table *****/
      Box_BoxTableBegin (NULL,Txt_Degrees_with_pending_courses,
                         NULL,NULL,
                         Hlp_SYSTEM_Pending,Box_NOT_CLOSABLE,2);

      /***** Write heading *****/
      HTM_TR_Begin (NULL);

      HTM_TH (1,1,"LM",Txt_Degree);
      HTM_TH (1,1,"RM",Txt_Courses_ABBREVIATION);

      HTM_TR_End ();

      /***** List the degrees *****/
      for (NumDeg = 0;
	   NumDeg < NumDegs;
	   NumDeg++)
        {
         /* Get next degree */
         row = mysql_fetch_row (mysql_res);

         /* Get degree code (row[0]) */
         Deg.DegCod = Str_ConvertStrCodToLongCod (row[0]);
         BgColor = (Deg.DegCod == Gbl.Hierarchy.Deg.DegCod) ? "LIGHT_BLUE" :
                                                               Gbl.ColorRows[Gbl.RowEvenOdd];

         /* Get data of degree */
         Deg_GetDataOfDegreeByCod (&Deg);

         HTM_TR_Begin (NULL);

         /* Degree logo and full name */
         HTM_TD_Begin ("class=\"LM %s\"",BgColor);
         Deg_DrawDegreeLogoAndNameWithLink (&Deg,ActSeeCrs,
                                            "BT_LINK DAT_NOBR","CM");
         HTM_TD_End ();

         /* Number of pending courses (row[1]) */
         HTM_TD_Begin ("class=\"DAT RM %s\"",BgColor);
	 HTM_Txt (row[1]);
         HTM_TD_End ();

         HTM_TR_End ();

         Gbl.RowEvenOdd = 1 - Gbl.RowEvenOdd;
        }

      /***** End table and box *****/
      Box_BoxTableEnd ();
     }
   else
      Ale_ShowAlert (Ale_INFO,Txt_There_are_no_degrees_with_requests_for_courses_to_be_confirmed);

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/******************** Draw degree logo and name with link ********************/
/*****************************************************************************/

void Deg_DrawDegreeLogoAndNameWithLink (struct Deg_Degree *Deg,Act_Action_t Action,
                                        const char *ClassLink,const char *ClassLogo)
  {
   /***** Begin form *****/
   Frm_BeginFormGoTo (Action);
   Deg_PutParamDegCod (Deg->DegCod);

   /***** Link to action *****/
   HTM_BUTTON_SUBMIT_Begin (Hie_BuildGoToMsg (Deg->FullName),ClassLink,NULL);
   Hie_FreeGoToMsg ();

   /***** Degree logo and name *****/
   Lgo_DrawLogo (Hie_Lvl_DEG,Deg->DegCod,Deg->ShrtName,16,ClassLogo,true);
   HTM_TxtF ("&nbsp;%s",Deg->FullName);

   /***** End link *****/
   HTM_BUTTON_End ();

   /***** End form *****/
   Frm_EndForm ();
  }

/*****************************************************************************/
/*************************** Write selector of degree ************************/
/*****************************************************************************/

void Deg_WriteSelectorOfDegree (void)
  {
   extern const char *Txt_Degree;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumDegs;
   unsigned NumDeg;
   long DegCod;

   /***** Begin form *****/
   Frm_BeginFormGoTo (ActSeeCrs);
   if (Gbl.Hierarchy.Ctr.CtrCod > 0)
      HTM_SELECT_Begin (HTM_SUBMIT_ON_CHANGE,
			"id=\"deg\" name=\"deg\" class=\"HIE_SEL\"");
   else
      HTM_SELECT_Begin (HTM_DONT_SUBMIT_ON_CHANGE,
			"id=\"deg\" name=\"deg\" class=\"HIE_SEL\""
			" disabled=\"disabled\"");
   HTM_OPTION (HTM_Type_STRING,"",
	       Gbl.Hierarchy.Deg.DegCod < 0,true,
	       "[%s]",Txt_Degree);

   if (Gbl.Hierarchy.Ctr.CtrCod > 0)
     {
      /***** Get degrees belonging to the current center from database *****/
      NumDegs = (unsigned) DB_QuerySELECT (&mysql_res,"can not get degrees"
						      " of a center",
					   "SELECT DegCod,ShortName"
					   " FROM deg_degrees"
					   " WHERE CtrCod=%ld"
					   " ORDER BY ShortName",
					   Gbl.Hierarchy.Ctr.CtrCod);

      /***** Get degrees of this center *****/
      for (NumDeg = 0;
	   NumDeg < NumDegs;
	   NumDeg++)
        {
         /* Get next degree */
         row = mysql_fetch_row (mysql_res);

         /* Get degree code (row[0]) */
         if ((DegCod = Str_ConvertStrCodToLongCod (row[0])) < 0)
            Lay_ShowErrorAndExit ("Wrong degree.");

         /* Write option */
	 HTM_OPTION (HTM_Type_LONG,&DegCod,
		     Gbl.Hierarchy.Deg.DegCod > 0 &&
                     DegCod == Gbl.Hierarchy.Deg.DegCod,false,
		     "%s",row[1]);
        }

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);
     }

   /***** End form *****/
   HTM_SELECT_End ();
   Frm_EndForm ();
  }

/*****************************************************************************/
/************* Show the degrees belonging to the current center **************/
/*****************************************************************************/

void Deg_ShowDegsOfCurrentCtr (void)
  {
   /***** Trivial check *****/
   if (Gbl.Hierarchy.Ctr.CtrCod <= 0)	// No center selected
      return;

   /***** Get list of centers and degrees *****/
   Ctr_GetBasicListOfCenters (Gbl.Hierarchy.Ins.InsCod);
   Deg_GetListDegsInCurrentCtr ();

   /***** Write menu to select country, institution and center *****/
   Hie_WriteMenuHierarchy ();

   /***** Show list of degrees *****/
   Deg_ListDegrees ();

   /***** Free list of degrees and centers *****/
   Deg_FreeListDegs (&Gbl.Hierarchy.Degs);
   Ctr_FreeListCenters ();
  }

/*****************************************************************************/
/********************* List current degrees for edition **********************/
/*****************************************************************************/

static void Deg_ListDegreesForEdition (void)
  {
   extern const char *Txt_DEGREE_STATUS[Deg_NUM_STATUS_TXT];
   unsigned NumDeg;
   struct DegreeType *DegTyp;
   struct Deg_Degree *Deg;
   unsigned NumDegTyp;
   char WWW[Cns_MAX_BYTES_WWW + 1];
   struct UsrData UsrDat;
   bool ICanEdit;
   unsigned NumCrss;
   unsigned NumUsrsInCrssOfDeg;
   Deg_StatusTxt_t StatusTxt;
   unsigned StatusUnsigned;

   /***** Initialize structure with user's data *****/
   Usr_UsrDataConstructor (&UsrDat);

   /***** Write heading *****/
   HTM_TABLE_BeginWidePadding (2);
   Deg_PutHeadDegreesForEdition ();

   /***** List the degrees *****/
   for (NumDeg = 0;
	NumDeg < Gbl.Hierarchy.Degs.Num;
	NumDeg++)
     {
      Deg = &(Gbl.Hierarchy.Degs.Lst[NumDeg]);

      ICanEdit = Deg_CheckIfICanEditADegree (Deg);
      NumCrss = Crs_GetNumCrssInDeg (Deg->DegCod);
      NumUsrsInCrssOfDeg = Usr_GetNumUsrsInCrss (Hie_Lvl_DEG,Deg->DegCod,
						 1 << Rol_STD |
						 1 << Rol_NET |
						 1 << Rol_TCH);	// Any user

      HTM_TR_Begin (NULL);

      /* Put icon to remove degree */
      HTM_TD_Begin ("class=\"BM\"");
      if (!ICanEdit ||
	  NumCrss ||	// Degree has courses ==> deletion forbidden
	  NumUsrsInCrssOfDeg)
         Ico_PutIconRemovalNotAllowed ();
      else
	 Ico_PutContextualIconToRemove (ActRemDeg,NULL,
					Deg_PutParamOtherDegCod,&Deg->DegCod);
      HTM_TD_End ();

      /* Degree code */
      HTM_TD_Begin ("class=\"DAT CODE\"");
      HTM_Long (Deg->DegCod);
      HTM_TD_End ();

      /* Degree logo */
      HTM_TD_Begin ("title=\"%s\" class=\"HIE_LOGO\"",Deg->FullName);
      Lgo_DrawLogo (Hie_Lvl_DEG,Deg->DegCod,Deg->ShrtName,20,NULL,true);
      HTM_TD_End ();

      /* Degree short name */
      HTM_TD_Begin ("class=\"DAT LM\"");
      if (ICanEdit)
	{
	 Frm_BeginForm (ActRenDegSho);
	 Deg_PutParamOtherDegCod (&Deg->DegCod);
	 HTM_INPUT_TEXT ("ShortName",Cns_HIERARCHY_MAX_CHARS_SHRT_NAME,Deg->ShrtName,
	                 HTM_SUBMIT_ON_CHANGE,
			 "class=\"INPUT_SHORT_NAME\"");
	 Frm_EndForm ();
	}
      else
	 HTM_Txt (Deg->ShrtName);
      HTM_TD_End ();

      /* Degree full name */
      HTM_TD_Begin ("class=\"DAT LM\"");
      if (ICanEdit)
	{
	 Frm_BeginForm (ActRenDegFul);
	 Deg_PutParamOtherDegCod (&Deg->DegCod);
	 HTM_INPUT_TEXT ("FullName",Cns_HIERARCHY_MAX_CHARS_FULL_NAME,Deg->FullName,
	                 HTM_SUBMIT_ON_CHANGE,
			 "class=\"INPUT_FULL_NAME\"");
	 Frm_EndForm ();
	}
      else
	 HTM_Txt (Deg->FullName);
      HTM_TD_End ();

      /* Degree type */
      HTM_TD_Begin ("class=\"DAT LM\"");
      if (ICanEdit)
	{
	 Frm_BeginForm (ActChgDegTyp);
	 Deg_PutParamOtherDegCod (&Deg->DegCod);
	 HTM_SELECT_Begin (HTM_SUBMIT_ON_CHANGE,
			   "name=\"OthDegTypCod\" class=\"HIE_SEL_NARROW\"");
	 for (NumDegTyp = 0;
	      NumDegTyp < Gbl.DegTypes.Num;
	      NumDegTyp++)
	   {
	    DegTyp = &Gbl.DegTypes.Lst[NumDegTyp];
	    HTM_OPTION (HTM_Type_LONG,&DegTyp->DegTypCod,
			// Gbl.Hierarchy.Deg.DegCod > 0 &&
			DegTyp->DegTypCod == Deg->DegTypCod,false,
			"%s",DegTyp->DegTypName);
	   }
	 HTM_SELECT_End ();
	 Frm_EndForm ();
	}
      else
	 for (NumDegTyp = 0;
	      NumDegTyp < Gbl.DegTypes.Num;
	      NumDegTyp++)
	    if (Gbl.DegTypes.Lst[NumDegTyp].DegTypCod == Deg->DegTypCod)
	       HTM_Txt (Gbl.DegTypes.Lst[NumDegTyp].DegTypName);
      HTM_TD_End ();

      /* Degree WWW */
      HTM_TD_Begin ("class=\"DAT LM\"");
      if (ICanEdit)
	{
	 Frm_BeginForm (ActChgDegWWW);
	 Deg_PutParamOtherDegCod (&Deg->DegCod);
	 HTM_INPUT_URL ("WWW",Deg->WWW,HTM_SUBMIT_ON_CHANGE,
			"class=\"INPUT_WWW_NARROW\" required=\"required\"");
	 Frm_EndForm ();
	}
      else
	{
         Str_Copy (WWW,Deg->WWW,sizeof (WWW) - 1);
         HTM_DIV_Begin ("class=\"EXTERNAL_WWW_SHORT\"");
         HTM_A_Begin ("href=\"%s\" target=\"_blank\" class=\"DAT\" title=\"%s\"",
		      Deg->WWW,Deg->WWW);
         HTM_Txt (WWW);
         HTM_A_End ();
         HTM_DIV_End ();
	}
      HTM_TD_End ();

      /* Number of courses in this degree */
      HTM_TD_Begin ("class=\"DAT RM\"");
      HTM_Unsigned (NumCrss);
      HTM_TD_End ();

      /* Number of users in courses of this degree */
      HTM_TD_Begin ("class=\"DAT RM\"");
      HTM_Unsigned (NumUsrsInCrssOfDeg);
      HTM_TD_End ();

      /* Degree requester */
      UsrDat.UsrCod = Deg->RequesterUsrCod;
      Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat,Usr_DONT_GET_PREFS);
      HTM_TD_Begin ("class=\"DAT INPUT_REQUESTER LT\"");
      Msg_WriteMsgAuthor (&UsrDat,true,NULL);
      HTM_TD_End ();

      /* Degree status */
      StatusTxt = Deg_GetStatusTxtFromStatusBits (Deg->Status);
      HTM_TD_Begin ("class=\"DAT LM\"");
      if (Gbl.Usrs.Me.Role.Logged >= Rol_CTR_ADM &&
	  StatusTxt == Deg_STATUS_PENDING)
	{
	 Frm_BeginForm (ActChgDegSta);
	 Deg_PutParamOtherDegCod (&Deg->DegCod);
	 HTM_SELECT_Begin (HTM_SUBMIT_ON_CHANGE,
			   "name=\"Status\" class=\"INPUT_STATUS\"");
	 StatusUnsigned = (unsigned) Deg_GetStatusBitsFromStatusTxt (Deg_STATUS_PENDING);
	 HTM_OPTION (HTM_Type_UNSIGNED,&StatusUnsigned,true,false,
		     "%s",Txt_DEGREE_STATUS[Deg_STATUS_PENDING]);
	 StatusUnsigned = (unsigned) Deg_GetStatusBitsFromStatusTxt (Deg_STATUS_ACTIVE);
	 HTM_OPTION (HTM_Type_UNSIGNED,&StatusUnsigned,false,false,
		     "%s",Txt_DEGREE_STATUS[Deg_STATUS_ACTIVE]);
	 HTM_SELECT_End ();
	 Frm_EndForm ();
	}
      else if (StatusTxt != Deg_STATUS_ACTIVE)	// If active ==> do not show anything
	 HTM_Txt (Txt_DEGREE_STATUS[StatusTxt]);
      HTM_TD_End ();
      HTM_TR_End ();
     }

   /***** End table *****/
   HTM_TABLE_End ();

   /***** Free memory used for user's data *****/
   Usr_UsrDataDestructor (&UsrDat);
  }

/*****************************************************************************/
/************** Check if I can edit, remove, etc. a degree *******************/
/*****************************************************************************/

static bool Deg_CheckIfICanEditADegree (struct Deg_Degree *Deg)
  {
   return (bool) (Gbl.Usrs.Me.Role.Logged >= Rol_CTR_ADM ||		// I am a center administrator or higher
                  ((Deg->Status & Deg_STATUS_BIT_PENDING) != 0 &&	// Degree is not yet activated
                   Gbl.Usrs.Me.UsrDat.UsrCod == Deg->RequesterUsrCod));	// I am the requester
  }

/*****************************************************************************/
/******************* Set StatusTxt depending on status bits ******************/
/*****************************************************************************/
// Deg_STATUS_UNKNOWN = 0	// Other
// Deg_STATUS_ACTIVE  = 1	// 00 (Status == 0)
// Deg_STATUS_PENDING = 2	// 01 (Status == Deg_STATUS_BIT_PENDING)
// Deg_STATUS_REMOVED = 3	// 1- (Status & Deg_STATUS_BIT_REMOVED)

static Deg_StatusTxt_t Deg_GetStatusTxtFromStatusBits (Deg_Status_t Status)
  {
   if (Status == 0)
      return Deg_STATUS_ACTIVE;
   if (Status == Deg_STATUS_BIT_PENDING)
      return Deg_STATUS_PENDING;
   if (Status & Deg_STATUS_BIT_REMOVED)
      return Deg_STATUS_REMOVED;
   return Deg_STATUS_UNKNOWN;
  }

/*****************************************************************************/
/******************* Set status bits depending on StatusTxt ******************/
/*****************************************************************************/
// Deg_STATUS_UNKNOWN = 0	// Other
// Deg_STATUS_ACTIVE  = 1	// 00 (Status == 0)
// Deg_STATUS_PENDING = 2	// 01 (Status == Deg_STATUS_BIT_PENDING)
// Deg_STATUS_REMOVED = 3	// 1- (Status & Deg_STATUS_BIT_REMOVED)

static Deg_Status_t Deg_GetStatusBitsFromStatusTxt (Deg_StatusTxt_t StatusTxt)
  {
   switch (StatusTxt)
     {
      case Deg_STATUS_UNKNOWN:
      case Deg_STATUS_ACTIVE:
	 return (Deg_Status_t) 0;
      case Deg_STATUS_PENDING:
	 return Deg_STATUS_BIT_PENDING;
      case Deg_STATUS_REMOVED:
	 return Deg_STATUS_BIT_REMOVED;
     }
   return (Deg_Status_t) 0;
  }

/*****************************************************************************/
/*********************** Put a form to create a new degree *******************/
/*****************************************************************************/

static void Deg_PutFormToCreateDegree (void)
  {
   extern const char *Txt_New_degree;
   extern const char *Txt_Create_degree;
   struct DegreeType *DegTyp;
   unsigned NumDegTyp;

   /***** Begin form *****/
   if (Gbl.Usrs.Me.Role.Logged >= Rol_CTR_ADM)
      Frm_BeginForm (ActNewDeg);
   else if (Gbl.Usrs.Me.Role.Max >= Rol_GST)
      Frm_BeginForm (ActReqDeg);
   else
      Lay_NoPermissionExit ();

   /***** Begin box and table *****/
   Box_BoxTableBegin (NULL,Txt_New_degree,
                      NULL,NULL,
                      NULL,Box_NOT_CLOSABLE,2);

   /***** Write heading *****/
   Deg_PutHeadDegreesForEdition ();

   HTM_TR_Begin (NULL);

   /***** Column to remove degree, disabled here *****/
   HTM_TD_Begin ("class=\"BM\"");
   HTM_TD_End ();

   /***** Degree code *****/
   HTM_TD_Begin ("class=\"CODE\"");
   HTM_TD_End ();

   /***** Degree logo *****/
   HTM_TD_Begin ("title=\"%s\" class=\"HIE_LOGO\"",Deg_EditingDeg->FullName);
   Lgo_DrawLogo (Hie_Lvl_DEG,-1L,"",20,NULL,true);
   HTM_TD_End ();

   /***** Degree short name *****/
   HTM_TD_Begin ("class=\"LM\"");
   HTM_INPUT_TEXT ("ShortName",Cns_HIERARCHY_MAX_CHARS_SHRT_NAME,Deg_EditingDeg->ShrtName,
                   HTM_DONT_SUBMIT_ON_CHANGE,
		   "class=\"INPUT_SHORT_NAME\" required=\"required\"");
   HTM_TD_End ();

   /***** Degree full name *****/
   HTM_TD_Begin ("class=\"LM\"");
   HTM_INPUT_TEXT ("FullName",Cns_HIERARCHY_MAX_CHARS_FULL_NAME,Deg_EditingDeg->FullName,
                   HTM_DONT_SUBMIT_ON_CHANGE,
		   "class=\"INPUT_FULL_NAME\" required=\"required\"");
   HTM_TD_End ();

   /***** Degree type *****/
   HTM_TD_Begin ("class=\"LM\"");
   HTM_SELECT_Begin (HTM_DONT_SUBMIT_ON_CHANGE,
		     "name=\"OthDegTypCod\" class=\"HIE_SEL_NARROW\"");
   for (NumDegTyp = 0;
	NumDegTyp < Gbl.DegTypes.Num;
	NumDegTyp++)
     {
      DegTyp = &Gbl.DegTypes.Lst[NumDegTyp];
      HTM_OPTION (HTM_Type_LONG,&DegTyp->DegTypCod,
		  DegTyp->DegTypCod == Deg_EditingDeg->DegTypCod,false,
		  "%s",DegTyp->DegTypName);
     }
   HTM_SELECT_End ();
   HTM_TD_End ();

   /***** Degree WWW *****/
   HTM_TD_Begin ("class=\"LM\"");
   HTM_INPUT_URL ("WWW",Deg_EditingDeg->WWW,HTM_DONT_SUBMIT_ON_CHANGE,
		  "class=\"INPUT_WWW_NARROW\" required=\"required\"");
   HTM_TD_End ();

   /***** Number of courses in this degree *****/
   HTM_TD_Begin ("class=\"DAT RM\"");
   HTM_Unsigned (0);
   HTM_TD_End ();

   /***** Number of users in courses of this degree *****/
   HTM_TD_Begin ("class=\"DAT RM\"");
   HTM_Unsigned (0);
   HTM_TD_End ();

   /***** Degree requester *****/
   HTM_TD_Begin ("class=\"DAT INPUT_REQUESTER LT\"");
   Msg_WriteMsgAuthor (&Gbl.Usrs.Me.UsrDat,true,NULL);
   HTM_TD_End ();

   /***** Degree status *****/
   HTM_TD_Begin ("class=\"DAT LM\"");
   HTM_TD_End ();

   HTM_TR_End ();

   /***** End table, send button and end box *****/
   Box_BoxTableWithButtonEnd (Btn_CREATE_BUTTON,Txt_Create_degree);

   /***** End form *****/
   Frm_EndForm ();
  }

/*****************************************************************************/
/******************** Write header with fields of a degree *******************/
/*****************************************************************************/

static void Deg_PutHeadDegreesForSeeing (void)
  {
   extern const char *Txt_Degree;
   extern const char *Txt_Type;
   extern const char *Txt_Courses_ABBREVIATION;
   extern const char *Txt_ROLES_PLURAL_BRIEF_Abc[Rol_NUM_ROLES];

   HTM_TR_Begin (NULL);

   HTM_TH (1,1,"BM",NULL);
   HTM_TH_Empty (1);
   HTM_TH (1,1,"LM",Txt_Degree);
   HTM_TH (1,1,"LM",Txt_Type);
   HTM_TH (1,1,"RM",Txt_Courses_ABBREVIATION);
   HTM_TH_Begin (1,1,"RM");
   HTM_TxtF ("%s+",Txt_ROLES_PLURAL_BRIEF_Abc[Rol_TCH]);
   HTM_BR ();
   HTM_Txt (Txt_ROLES_PLURAL_BRIEF_Abc[Rol_STD]);
   HTM_TH_End ();
   HTM_TH_Empty (1);

   HTM_TR_End ();
  }

/*****************************************************************************/
/******************** Write header with fields of a degree *******************/
/*****************************************************************************/

static void Deg_PutHeadDegreesForEdition (void)
  {
   extern const char *Txt_Code;
   extern const char *Txt_Short_name_of_the_degree;
   extern const char *Txt_Full_name_of_the_degree;
   extern const char *Txt_Type;
   extern const char *Txt_WWW;
   extern const char *Txt_Courses_ABBREVIATION;
   extern const char *Txt_ROLES_PLURAL_BRIEF_Abc[Rol_NUM_ROLES];
   extern const char *Txt_Requester;

   HTM_TR_Begin (NULL);

   HTM_TH (1,1,"BM",NULL);
   HTM_TH (1,1,"RM",Txt_Code);
   HTM_TH_Empty (1);
   HTM_TH (1,1,"LM",Txt_Short_name_of_the_degree);
   HTM_TH (1,1,"LM",Txt_Full_name_of_the_degree);
   HTM_TH (1,1,"LM",Txt_Type);
   HTM_TH (1,1,"LM",Txt_WWW);
   HTM_TH (1,1,"RM",Txt_Courses_ABBREVIATION);
   HTM_TH_Begin (1,1,"RM");
   HTM_TxtF ("%s+",Txt_ROLES_PLURAL_BRIEF_Abc[Rol_TCH]);
   HTM_BR ();
   HTM_Txt (Txt_ROLES_PLURAL_BRIEF_Abc[Rol_STD]);
   HTM_TH_End ();
   HTM_TH (1,1,"LM",Txt_Requester);
   HTM_TH_Empty (1);

   HTM_TR_End ();
  }

/*****************************************************************************/
/******************** Convert string to year of a degree *********************/
/*****************************************************************************/

unsigned Deg_ConvStrToYear (const char *StrYear)
  {
   int Year;

   if (sscanf (StrYear,"%d",&Year) != 1)
      return 0;
   if (Year < 0)
      return 0;
   if (Year > Deg_MAX_YEARS_PER_DEGREE)
      return Deg_MAX_YEARS_PER_DEGREE;
   return (unsigned) Year;
  }

/*****************************************************************************/
/***************************** Create a new degree ***************************/
/*****************************************************************************/

static void Deg_CreateDegree (unsigned Status)
  {
   /***** Create a new degree *****/
   Deg_EditingDeg->DegCod =
   DB_QueryINSERTandReturnCode ("can not create a new degree",
				"INSERT INTO deg_degrees (CtrCod,DegTypCod,Status,"
				"RequesterUsrCod,ShortName,FullName,WWW)"
				" VALUES (%ld,%ld,%u,%ld,'%s','%s','%s')",
				Deg_EditingDeg->CtrCod,
				Deg_EditingDeg->DegTypCod,
				Status,
				Gbl.Usrs.Me.UsrDat.UsrCod,
				Deg_EditingDeg->ShrtName,
				Deg_EditingDeg->FullName,
				Deg_EditingDeg->WWW);
  }

/*****************************************************************************/
/************** List degrees belonging to the current center *****************/
/*****************************************************************************/

static void Deg_ListDegrees (void)
  {
   extern const char *Hlp_CENTER_Degrees;
   extern const char *Txt_Degrees_of_CENTER_X;
   extern const char *Txt_No_degrees;
   extern const char *Txt_Create_another_degree;
   extern const char *Txt_Create_degree;
   unsigned NumDeg;

   /***** Begin box *****/
   Box_BoxBegin (NULL,Str_BuildStringStr (Txt_Degrees_of_CENTER_X,
				          Gbl.Hierarchy.Ctr.ShrtName),
		 Deg_PutIconsListingDegrees,NULL,
                 Hlp_CENTER_Degrees,Box_NOT_CLOSABLE);
   Str_FreeString ();

   if (Gbl.Hierarchy.Degs.Num)	// There are degrees in the current center
     {
      /***** Write heading *****/
      HTM_TABLE_BeginWideMarginPadding (2);
      Deg_PutHeadDegreesForSeeing ();

      /***** List the degrees *****/
      for (NumDeg = 0;
	   NumDeg < Gbl.Hierarchy.Degs.Num;
	   NumDeg++)
	 Deg_ListOneDegreeForSeeing (&(Gbl.Hierarchy.Degs.Lst[NumDeg]),NumDeg + 1);

      /***** End table *****/
      HTM_TABLE_End ();
     }
   else	// No degrees created in the current center
      Ale_ShowAlert (Ale_INFO,Txt_No_degrees);

   /***** Button to create degree *****/
   if (Deg_CheckIfICanCreateDegrees ())
     {
      Frm_BeginForm (ActEdiDeg);
      Btn_PutConfirmButton (Gbl.Hierarchy.Degs.Num ? Txt_Create_another_degree :
	                                                 Txt_Create_degree);
      Frm_EndForm ();
     }

   /***** End box *****/
   Box_BoxEnd ();
  }

/*****************************************************************************/
/********************** Check if I can create degrees ************************/
/*****************************************************************************/

static bool Deg_CheckIfICanCreateDegrees (void)
  {
   return (bool) (Gbl.Usrs.Me.Role.Logged >= Rol_GST);
  }

/*****************************************************************************/
/***************** Put contextual icons in list of degrees *******************/
/*****************************************************************************/

static void Deg_PutIconsListingDegrees (__attribute__((unused)) void *Args)
  {
   /***** Put icon to edit degrees *****/
   if (Deg_CheckIfICanCreateDegrees ())
      Deg_PutIconToEditDegrees ();

   /***** Put icon to view degree types *****/
   DT_PutIconToViewDegreeTypes ();

   /***** Put icon to show a figure *****/
   Fig_PutIconToShowFigure (Fig_HIERARCHY);
  }

/*****************************************************************************/
/********************** Put link (form) to edit degrees **********************/
/*****************************************************************************/

static void Deg_PutIconToEditDegrees (void)
  {
   Ico_PutContextualIconToEdit (ActEdiDeg,NULL,
                                NULL,NULL);
  }

/*****************************************************************************/
/************************ List one degree for seeing *************************/
/*****************************************************************************/

static void Deg_ListOneDegreeForSeeing (struct Deg_Degree *Deg,unsigned NumDeg)
  {
   extern const char *Txt_DEGREE_With_courses;
   extern const char *Txt_DEGREE_Without_courses;
   extern const char *Txt_DEGREE_STATUS[Deg_NUM_STATUS_TXT];
   struct DegreeType DegTyp;
   const char *TxtClassNormal;
   const char *TxtClassStrong;
   const char *BgColor;
   unsigned NumCrss = Crs_GetCachedNumCrssInDeg (Deg->DegCod);
   Deg_StatusTxt_t StatusTxt;

   /***** Get data of type of degree of this degree *****/
   DegTyp.DegTypCod = Deg->DegTypCod;
   if (!DT_GetDataOfDegreeTypeByCod (&DegTyp))
      Lay_ShowErrorAndExit ("Code of type of degree not found.");

   if (Deg->Status & Deg_STATUS_BIT_PENDING)
     {
      TxtClassNormal = "DAT_LIGHT";
      TxtClassStrong = "BT_LINK LT DAT_LIGHT";
     }
   else
     {
      TxtClassNormal = "DAT";
      TxtClassStrong = "BT_LINK LT DAT_N";
     }
   BgColor = (Deg->DegCod == Gbl.Hierarchy.Deg.DegCod) ? "LIGHT_BLUE" :
                                                         Gbl.ColorRows[Gbl.RowEvenOdd];

   HTM_TR_Begin (NULL);

   /***** Put tip if degree has courses *****/
   HTM_TD_Begin ("class=\"%s CM %s\" title=\"%s\"",
		 TxtClassNormal,BgColor,
		 NumCrss ? Txt_DEGREE_With_courses :
			   Txt_DEGREE_Without_courses);
   HTM_Txt (NumCrss ? "&check;" :
		      "&nbsp;");
   HTM_TD_End ();

   /***** Number of degree in this list *****/
   HTM_TD_Begin ("class=\"%s RM %s\"",TxtClassNormal,BgColor);
   HTM_Unsigned (NumDeg);
   HTM_TD_End ();

   /***** Degree logo and name *****/
   HTM_TD_Begin ("class=\"LM %s\"",BgColor);
   Deg_DrawDegreeLogoAndNameWithLink (Deg,ActSeeCrs,
                                      TxtClassStrong,"CM");
   HTM_TD_End ();

   /***** Type of degree *****/
   HTM_TD_Begin ("class=\"%s LM %s\"",TxtClassNormal,BgColor);
   HTM_Txt (DegTyp.DegTypName);
   HTM_TD_End ();

   /***** Current number of courses in this degree *****/
   HTM_TD_Begin ("class=\"%s RM %s\"",TxtClassNormal,BgColor);
   HTM_Unsigned (NumCrss);
   HTM_TD_End ();

   /***** Number of users in courses of this degree *****/
   HTM_TD_Begin ("class=\"%s RM %s\"",TxtClassNormal,BgColor);
   HTM_Unsigned (Usr_GetCachedNumUsrsInCrss (Hie_Lvl_DEG,Deg->DegCod,
				             1 << Rol_STD |
				             1 << Rol_NET |
				             1 << Rol_TCH));	// Any user
   HTM_TD_End ();

   /***** Degree status *****/
   StatusTxt = Deg_GetStatusTxtFromStatusBits (Deg->Status);
   HTM_TD_Begin ("class=\"%s LM %s\"",TxtClassNormal,BgColor);
   if (StatusTxt != Deg_STATUS_ACTIVE) // If active ==> do not show anything
      HTM_Txt (Txt_DEGREE_STATUS[StatusTxt]);
   HTM_TD_End ();

   HTM_TR_End ();

   Gbl.RowEvenOdd = 1 - Gbl.RowEvenOdd;
  }

/*****************************************************************************/
/*************************** Put forms to edit degrees ***********************/
/*****************************************************************************/

void Deg_EditDegrees (void)
  {
   /***** Degree constructor *****/
   Deg_EditingDegreeConstructor ();

   /***** Edit degrees *****/
   Deg_EditDegreesInternal ();

   /***** Degree destructor *****/
   Deg_EditingDegreeDestructor ();
  }

static void Deg_EditDegreesInternal (void)
  {
   extern const char *Hlp_CENTER_Degrees;
   extern const char *Txt_Degrees_of_CENTER_X;
   extern const char *Txt_No_types_of_degree;

   /***** Get list of degrees in the current center *****/
   Deg_GetListDegsInCurrentCtr ();

   /***** Get list of degree types *****/
   DT_GetListDegreeTypes (Hie_Lvl_SYS,DT_ORDER_BY_DEGREE_TYPE);

   /***** Write menu to select country, institution and center *****/
   Hie_WriteMenuHierarchy ();

   /***** Begin box *****/
   Box_BoxBegin (NULL,Str_BuildStringStr (Txt_Degrees_of_CENTER_X,
				          Gbl.Hierarchy.Ctr.ShrtName),
		 Deg_PutIconsEditingDegrees,NULL,
                 Hlp_CENTER_Degrees,Box_NOT_CLOSABLE);
   Str_FreeString ();

   if (Gbl.DegTypes.Num)
     {
      /***** Put a form to create a new degree *****/
      Deg_PutFormToCreateDegree ();

      /***** Forms to edit current degrees *****/
      if (Gbl.Hierarchy.Degs.Num)
         Deg_ListDegreesForEdition ();
     }
   else	// No degree types
     {
      /***** Warning message *****/
      Ale_ShowAlert (Ale_WARNING,Txt_No_types_of_degree);

      /***** Form to create the first degree type *****/
      if (DT_CheckIfICanCreateDegreeTypes ())
         DT_EditDegreeTypes ();
     }

   /***** End box *****/
   Box_BoxEnd ();

   /***** Free list of degree types *****/
   DT_FreeListDegreeTypes ();

   /***** Free list of degrees in the current center *****/
   Deg_FreeListDegs (&Gbl.Hierarchy.Degs);
  }

/*****************************************************************************/
/**************** Put contextual icons in edition of degrees *****************/
/*****************************************************************************/

static void Deg_PutIconsEditingDegrees (__attribute__((unused)) void *Args)
  {
   /***** Put icon to view degrees *****/
   Deg_PutIconToViewDegrees ();

   /***** Put icon to view types of degree *****/
   DT_PutIconToViewDegreeTypes ();

   /***** Put icon to show a figure *****/
   Fig_PutIconToShowFigure (Fig_HIERARCHY);
  }

/*****************************************************************************/
/*********************** Put icon to view degrees ****************************/
/*****************************************************************************/

void Deg_PutIconToViewDegrees (void)
  {
   extern const char *Txt_Degrees;

   Lay_PutContextualLinkOnlyIcon (ActSeeDeg,NULL,
                                  NULL,NULL,
				  "graduation-cap.svg",
				  Txt_Degrees);
  }

/*****************************************************************************/
/********** Create a list with all the degrees that have students ************/
/*****************************************************************************/

void Deg_GetListAllDegsWithStds (struct ListDegrees *Degs)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumDeg;

   /***** Get degrees admin by me from database *****/
   Degs->Num = (unsigned)
   DB_QuerySELECT (&mysql_res,"can not get degrees admin by you",
		   "SELECT DISTINCTROW "
			  "deg_degrees.DegCod,"			// row[0]
			  "deg_degrees.CtrCod,"			// row[1]
			  "deg_degrees.DegTypCod,"		// row[2]
			  "deg_degrees.Status,"			// row[3]
			  "deg_degrees.RequesterUsrCod,"	// row[4]
			  "deg_degrees.ShortName,"		// row[5]
			  "deg_degrees.FullName,"		// row[6]
			  "deg_degrees.WWW"			// row[7]
		    " FROM deg_degrees,"
		          "crs_courses,"
		          "crs_users"
		   " WHERE deg_degrees.DegCod=crs_courses.DegCod"
		     " AND crs_courses.CrsCod=crs_users.CrsCod"
		     " AND crs_users.Role=%u"
		   " ORDER BY deg_degrees.ShortName",
		   (unsigned) Rol_STD);

   if (Degs->Num) // Degrees found...
     {
      /***** Create list with degrees *****/
      if ((Degs->Lst = calloc (Degs->Num,sizeof (*Degs->Lst))) == NULL)
         Lay_NotEnoughMemoryExit ();

      /***** Get the degrees *****/
      for (NumDeg = 0;
	   NumDeg < Degs->Num;
	   NumDeg++)
        {
         /* Get next degree */
         row = mysql_fetch_row (mysql_res);
         Deg_GetDataOfDegreeFromRow (&(Degs->Lst[NumDeg]),row);
        }
     }
   else
      Degs->Lst = NULL;

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/************ Get a list with the degrees of the current center **************/
/*****************************************************************************/

void Deg_GetListDegsInCurrentCtr (void)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;
   unsigned NumDeg;
   struct Deg_Degree *Deg;

   /***** Get degrees of the current center from database *****/
   NumRows = DB_QuerySELECT (&mysql_res,"can not get degrees of a center",
			     "SELECT DegCod,"		// row[0]
			            "CtrCod,"		// row[1]
			            "DegTypCod,"	// row[2]
			            "Status,"		// row[3]
			            "RequesterUsrCod,"	// row[4]
			            "ShortName,"	// row[5]
			            "FullName,"		// row[6]
			            "WWW"		// row[7]
			     " FROM deg_degrees"
			     " WHERE CtrCod=%ld"
			     " ORDER BY FullName",
			     Gbl.Hierarchy.Ctr.CtrCod);

   /***** Count number of rows in result *****/
   if (NumRows) // Degrees found...
     {
      Gbl.Hierarchy.Degs.Num = (unsigned) NumRows;

      /***** Create list with degrees of this center *****/
      if ((Gbl.Hierarchy.Degs.Lst = calloc (Gbl.Hierarchy.Degs.Num,
                                            sizeof (*Gbl.Hierarchy.Degs.Lst))) == NULL)
         Lay_NotEnoughMemoryExit ();

      /***** Get the degrees of this center *****/
      for (NumDeg = 0;
	   NumDeg < Gbl.Hierarchy.Degs.Num;
	   NumDeg++)
        {
         Deg = &Gbl.Hierarchy.Degs.Lst[NumDeg];

         /* Get next degree */
         row = mysql_fetch_row (mysql_res);
         Deg_GetDataOfDegreeFromRow (Deg,row);
        }
     }
   else
      Gbl.Hierarchy.Degs.Num = 0;

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/*************************** Free list of degrees ****************************/
/*****************************************************************************/

void Deg_FreeListDegs (struct ListDegrees *Degs)
  {
   if (Degs->Lst)
     {
      free (Degs->Lst);
      Degs->Lst = NULL;
      Degs->Num = 0;
     }
  }

/*****************************************************************************/
/****************** Receive form to request a new degree *********************/
/*****************************************************************************/

void Deg_ReceiveFormReqDeg (void)
  {
   /***** Degree constructor *****/
   Deg_EditingDegreeConstructor ();

   /***** Receive form to request a new degree *****/
   Deg_ReceiveFormRequestOrCreateDeg ((unsigned) Deg_STATUS_BIT_PENDING);
  }

/*****************************************************************************/
/******************* Receive form to create a new degree *********************/
/*****************************************************************************/

void Deg_ReceiveFormNewDeg (void)
  {
   /***** Degree constructor *****/
   Deg_EditingDegreeConstructor ();

   /***** Receive form to create a new degree *****/
   Deg_ReceiveFormRequestOrCreateDeg (0);
  }

/*****************************************************************************/
/******************* Receive form to create a new degree *********************/
/*****************************************************************************/

static void Deg_ReceiveFormRequestOrCreateDeg (unsigned Status)
  {
   extern const char *Txt_The_degree_X_already_exists;
   extern const char *Txt_Created_new_degree_X;
   extern const char *Txt_You_must_specify_the_web_address_of_the_new_degree;
   extern const char *Txt_You_must_specify_the_short_name_and_the_full_name_of_the_new_degree;

   /***** Get parameters from form *****/
   /* Set degree center */
   Deg_EditingDeg->CtrCod = Gbl.Hierarchy.Ctr.CtrCod;

   /* Get degree short name */
   Par_GetParToText ("ShortName",Deg_EditingDeg->ShrtName,Cns_HIERARCHY_MAX_BYTES_SHRT_NAME);

   /* Get degree full name */
   Par_GetParToText ("FullName",Deg_EditingDeg->FullName,Cns_HIERARCHY_MAX_BYTES_FULL_NAME);

   /* Get degree type */
   Deg_EditingDeg->DegTypCod = DT_GetAndCheckParamOtherDegTypCod (1);

   /* Get degree WWW */
   Par_GetParToText ("WWW",Deg_EditingDeg->WWW,Cns_MAX_BYTES_WWW);

   if (Deg_EditingDeg->ShrtName[0] &&
       Deg_EditingDeg->FullName[0])	// If there's a degree name
     {
      if (Deg_EditingDeg->WWW[0])
	{
	 /***** If name of degree was in database... *****/
	 if (Deg_CheckIfDegNameExistsInCtr ("ShortName",Deg_EditingDeg->ShrtName,
	                                    -1L,Deg_EditingDeg->CtrCod))
	    Ale_CreateAlert (Ale_WARNING,NULL,
		             Txt_The_degree_X_already_exists,
		             Deg_EditingDeg->ShrtName);
	 else if (Deg_CheckIfDegNameExistsInCtr ("FullName",Deg_EditingDeg->FullName,
	                                         -1L,Deg_EditingDeg->CtrCod))
	    Ale_CreateAlert (Ale_WARNING,NULL,
		             Txt_The_degree_X_already_exists,
		             Deg_EditingDeg->FullName);
	 else	// Add new degree to database
	   {
	    Deg_CreateDegree (Status);
	    Ale_CreateAlert (Ale_SUCCESS,NULL,
			     Txt_Created_new_degree_X,
			     Deg_EditingDeg->FullName);
	   }
	}
      else	// If there is not a degree logo or web
	 Ale_CreateAlert (Ale_WARNING,NULL,
	                  Txt_You_must_specify_the_web_address_of_the_new_degree);
     }
   else	// If there is not a degree name
      Ale_CreateAlert (Ale_WARNING,NULL,
	               Txt_You_must_specify_the_short_name_and_the_full_name_of_the_new_degree);
  }

/*****************************************************************************/
/************************ Request removing of a degree ***********************/
/*****************************************************************************/

void Deg_RemoveDegree (void)
  {
   extern const char *Txt_To_remove_a_degree_you_must_first_remove_all_courses_in_the_degree;
   extern const char *Txt_Degree_X_removed;

   /***** Degree constructor *****/
   Deg_EditingDegreeConstructor ();

   /***** Get degree code *****/
   Deg_EditingDeg->DegCod = Deg_GetAndCheckParamOtherDegCod (1);

   /***** Get data of degree *****/
   Deg_GetDataOfDegreeByCod (Deg_EditingDeg);

   /***** Check if this degree has courses *****/
   if (Crs_GetNumCrssInDeg (Deg_EditingDeg->DegCod))	// Degree has courses ==> don't remove
      Ale_CreateAlert (Ale_WARNING,NULL,
	               Txt_To_remove_a_degree_you_must_first_remove_all_courses_in_the_degree);
   else	// Degree has no courses ==> remove it
     {
      /***** Remove degree *****/
      Deg_RemoveDegreeCompletely (Deg_EditingDeg->DegCod);

      /***** Write message to show the change made *****/
      Ale_CreateAlert (Ale_SUCCESS,NULL,
		       Txt_Degree_X_removed,
                       Deg_EditingDeg->FullName);

      Deg_EditingDeg->DegCod = -1L;	// To not showing button to go to degree
     }
  }

/*****************************************************************************/
/******************** Write parameter with code of degree ********************/
/*****************************************************************************/

void Deg_PutParamDegCod (long DegCod)
  {
   Par_PutHiddenParamLong (NULL,"deg",DegCod);
  }

/*****************************************************************************/
/******************** Write parameter with code of degree ********************/
/*****************************************************************************/

static void Deg_PutParamOtherDegCod (void *DegCod)
  {
   if (DegCod)
      Par_PutHiddenParamLong (NULL,"OthDegCod",*((long *) DegCod));
  }

/*****************************************************************************/
/********************* Get parameter with code of degree *********************/
/*****************************************************************************/

long Deg_GetAndCheckParamOtherDegCod (long MinCodAllowed)
  {
   long DegCod;

   /***** Get and check parameter with code of degree *****/
   if ((DegCod = Par_GetParToLong ("OthDegCod")) < MinCodAllowed)
      Lay_ShowErrorAndExit ("Code of degree is missing or invalid.");

   return DegCod;
  }

/*****************************************************************************/
/********************* Get data of a degree from its code ********************/
/*****************************************************************************/
// Returns true if degree found

bool Deg_GetDataOfDegreeByCod (struct Deg_Degree *Deg)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   bool DegFound = false;

   /***** Clear data *****/
   Deg->CtrCod          = -1L;
   Deg->DegTypCod       = -1L;
   Deg->Status          = (Deg_Status_t) 0;
   Deg->RequesterUsrCod = -1L;
   Deg->ShrtName[0]     = '\0';
   Deg->FullName[0]     = '\0';
   Deg->WWW[0]          = '\0';

   /***** Check if degree code is correct *****/
   if (Deg->DegCod > 0)
     {
      /***** Get data of a degree from database *****/
      if (DB_QuerySELECT (&mysql_res,"can not get data of a degree",
			  "SELECT DegCod,"		// row[0]
			         "CtrCod,"		// row[1]
			         "DegTypCod,"		// row[2]
			         "Status,"		// row[3]
			         "RequesterUsrCod,"	// row[4]
			         "ShortName,"		// row[5]
			         "FullName,"		// row[6]
			         "WWW"			// row[7]
			  " FROM deg_degrees WHERE DegCod=%ld",
			  Deg->DegCod)) // Degree found...
	{
	 /***** Get data of degree *****/
	 row = mysql_fetch_row (mysql_res);
	 Deg_GetDataOfDegreeFromRow (Deg,row);

         /* Set return value */
	 DegFound = true;
	}

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);
     }

   return DegFound;
  }

/*****************************************************************************/
/********** Get data of a degree from a row resulting of a query *************/
/*****************************************************************************/

static void Deg_GetDataOfDegreeFromRow (struct Deg_Degree *Deg,MYSQL_ROW row)
  {
   /***** Get degree code (row[0]) *****/
   if ((Deg->DegCod = Str_ConvertStrCodToLongCod (row[0])) < 0)
      Lay_ShowErrorAndExit ("Wrong code of degree.");

   /***** Get center code (row[1]) *****/
   Deg->CtrCod = Str_ConvertStrCodToLongCod (row[1]);

   /***** Get the code of the degree type (row[2]) *****/
   Deg->DegTypCod = Str_ConvertStrCodToLongCod (row[2]);

   /* Get course status (row[3]) */
   if (sscanf (row[3],"%u",&(Deg->Status)) != 1)
      Lay_ShowErrorAndExit ("Wrong degree status.");

   /* Get requester user's code (row[4]) */
   Deg->RequesterUsrCod = Str_ConvertStrCodToLongCod (row[4]);

   /***** Get degree short name (row[5]), full name (row[6]) and WWW (row[7]) *****/
   Str_Copy (Deg->ShrtName,row[5],sizeof (Deg->ShrtName) - 1);
   Str_Copy (Deg->FullName,row[6],sizeof (Deg->FullName) - 1);
   Str_Copy (Deg->WWW     ,row[7],sizeof (Deg->WWW     ) - 1);
  }

/*****************************************************************************/
/************* Get the short name of a degree from its code ******************/
/*****************************************************************************/

void Deg_GetShortNameOfDegreeByCod (struct Deg_Degree *Deg)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;

   Deg->ShrtName[0] = '\0';
   if (Deg->DegCod > 0)
     {
      /***** Get the short name of a degree from database *****/
      if (DB_QuerySELECT (&mysql_res,"can not get the short name of a degree",
			  "SELECT ShortName FROM deg_degrees WHERE DegCod=%ld",
			  Deg->DegCod) == 1)
	{
	 /***** Get the short name of this degree *****/
	 row = mysql_fetch_row (mysql_res);
	 Str_Copy (Deg->ShrtName,row[0],sizeof (Deg->ShrtName) - 1);
	}

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);
     }
  }

/*****************************************************************************/
/************* Get the center code of a degree from its code *****************/
/*****************************************************************************/

long Deg_GetCtrCodOfDegreeByCod (long DegCod)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   long CtrCod = -1L;

   if (DegCod > 0)
     {
      /***** Get the center code of a degree from database *****/
      if (DB_QuerySELECT (&mysql_res,"can not get the center of a degree",
			  "SELECT CtrCod FROM deg_degrees WHERE DegCod=%ld",
			  DegCod) == 1)
	{
	 /***** Get the center code of this degree *****/
	 row = mysql_fetch_row (mysql_res);
	 CtrCod = Str_ConvertStrCodToLongCod (row[0]);
	}

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);
     }

   return CtrCod;
  }

/*****************************************************************************/
/********** Get the institution code of a degree from its code ***************/
/*****************************************************************************/

long Deg_GetInsCodOfDegreeByCod (long DegCod)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   long InsCod = -1L;

   if (DegCod > 0)
     {
      /***** Get the institution code of a degree from database *****/
      if (DB_QuerySELECT (&mysql_res,"can not get the institution of a degree",
			  "SELECT ctr_centers.InsCod"
			  " FROM deg_degrees,"
			        "ctr_centers"
			  " WHERE deg_degrees.DegCod=%ld"
			  " AND deg_degrees.CtrCod=ctr_centers.CtrCod",
			  DegCod) == 1)
	{
	 /***** Get the institution code of this degree *****/
	 row = mysql_fetch_row (mysql_res);
	 InsCod = Str_ConvertStrCodToLongCod (row[0]);
	}

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);
     }

   return InsCod;
  }

/*****************************************************************************/
/***************************** Remove a degree *******************************/
/*****************************************************************************/

void Deg_RemoveDegreeCompletely (long DegCod)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;
   unsigned long NumRow;
   long CrsCod;
   char PathDeg[PATH_MAX + 1];

   /***** Get courses of a degree from database *****/
   NumRows = DB_QuerySELECT (&mysql_res,"can not get courses of a degree",
			     "SELECT CrsCod"
			     " FROM crs_courses"
			     " WHERE DegCod=%ld",
			     DegCod);

   /* Get courses in this degree */
   for (NumRow = 0;
	NumRow < NumRows;
	NumRow++)
     {
      /* Get next course */
      row = mysql_fetch_row (mysql_res);

      /* Get course code (row[0]) */
      if ((CrsCod = Str_ConvertStrCodToLongCod (row[0])) < 0)
         Lay_ShowErrorAndExit ("Wrong code of course.");

      /* Remove course */
      Crs_RemoveCourseCompletely (CrsCod);
     }

   /* Free structure that stores the query result */
   DB_FreeMySQLResult (&mysql_res);

   /***** Remove all the threads and posts in forums of the degree *****/
   For_RemoveForums (Hie_Lvl_DEG,DegCod);

   /***** Remove surveys of the degree *****/
   Svy_RemoveSurveys (Hie_Lvl_DEG,DegCod);

   /***** Remove information related to files in degree *****/
   Brw_RemoveDegFilesFromDB (DegCod);

   /***** Remove directories of the degree *****/
   snprintf (PathDeg,sizeof (PathDeg),"%s/%02u/%u",
	     Cfg_PATH_DEG_PUBLIC,
	     (unsigned) (DegCod % 100),
	     (unsigned)  DegCod);
   Fil_RemoveTree (PathDeg);

   /***** Remove administrators of this degree *****/
   DB_QueryDELETE ("can not remove administrators of a degree",
		   "DELETE FROM usr_admins WHERE Scope='%s' AND Cod=%ld",
                   Sco_GetDBStrFromScope (Hie_Lvl_DEG),DegCod);

   /***** Remove the degree *****/
   DB_QueryDELETE ("can not remove a degree",
		   "DELETE FROM deg_degrees WHERE DegCod=%ld",
		   DegCod);

   /***** Flush caches *****/
   Crs_FlushCacheNumCrssInDeg ();

   /***** Delete all the degrees in sta_degrees table not present in degrees table *****/
   Pho_RemoveObsoleteStatDegrees ();
  }

/*****************************************************************************/
/************************ Change the name of a degree ************************/
/*****************************************************************************/

void Deg_RenameDegreeShort (void)
  {
   /***** Degree constructor *****/
   Deg_EditingDegreeConstructor ();

   /***** Rename degree *****/
   Deg_EditingDeg->DegCod = Deg_GetAndCheckParamOtherDegCod (1);
   Deg_RenameDegree (Deg_EditingDeg,Cns_SHRT_NAME);
  }

void Deg_RenameDegreeFull (void)
  {
   /***** Degree constructor *****/
   Deg_EditingDegreeConstructor ();

   /***** Rename degree *****/
   Deg_EditingDeg->DegCod = Deg_GetAndCheckParamOtherDegCod (1);
   Deg_RenameDegree (Deg_EditingDeg,Cns_FULL_NAME);
  }

/*****************************************************************************/
/************************ Change the name of a degree ************************/
/*****************************************************************************/

void Deg_RenameDegree (struct Deg_Degree *Deg,Cns_ShrtOrFullName_t ShrtOrFullName)
  {
   extern const char *Txt_The_degree_X_already_exists;
   extern const char *Txt_The_name_of_the_degree_X_has_changed_to_Y;
   extern const char *Txt_The_name_of_the_degree_X_has_not_changed;
   const char *ParamName = NULL;	// Initialized to avoid warning
   const char *FieldName = NULL;	// Initialized to avoid warning
   unsigned MaxBytes = 0;		// Initialized to avoid warning
   char *CurrentDegName = NULL;		// Initialized to avoid warning
   char NewDegName[Cns_HIERARCHY_MAX_BYTES_FULL_NAME + 1];

   switch (ShrtOrFullName)
     {
      case Cns_SHRT_NAME:
         ParamName = "ShortName";
         FieldName = "ShortName";
         MaxBytes = Cns_HIERARCHY_MAX_BYTES_SHRT_NAME;
         CurrentDegName = Deg->ShrtName;
         break;
      case Cns_FULL_NAME:
         ParamName = "FullName";
         FieldName = "FullName";
         MaxBytes = Cns_HIERARCHY_MAX_BYTES_FULL_NAME;
         CurrentDegName = Deg->FullName;
         break;
     }

   /***** Get parameters from form *****/
   /* Get the new name for the degree */
   Par_GetParToText (ParamName,NewDegName,MaxBytes);

   /***** Get data of degree *****/
   Deg_GetDataOfDegreeByCod (Deg);

   /***** Check if new name is empty *****/
   if (NewDegName[0])
     {
      /***** Check if old and new names are the same
             (this happens when return is pressed without changes) *****/
      if (strcmp (CurrentDegName,NewDegName))	// Different names
        {
         /***** If degree was in database... *****/
         if (Deg_CheckIfDegNameExistsInCtr (ParamName,NewDegName,Deg->DegCod,Deg->CtrCod))
            Ale_CreateAlert (Ale_WARNING,NULL,
        	             Txt_The_degree_X_already_exists,
		             NewDegName);
         else
           {
            /* Update the table changing old name by new name */
            Deg_UpdateDegNameDB (Deg->DegCod,FieldName,NewDegName);

            /* Write message to show the change made */
            Ale_CreateAlert (Ale_SUCCESS,NULL,
        	             Txt_The_name_of_the_degree_X_has_changed_to_Y,
                             CurrentDegName,NewDegName);

	    /* Change current degree name in order to display it properly */
	    Str_Copy (CurrentDegName,NewDegName,MaxBytes);
           }
        }
      else	// The same name
         Ale_CreateAlert (Ale_INFO,NULL,
                          Txt_The_name_of_the_degree_X_has_not_changed,
                          CurrentDegName);
     }
   else
      Ale_CreateAlertYouCanNotLeaveFieldEmpty ();
  }

/*****************************************************************************/
/********************* Check if the name of degree exists ********************/
/*****************************************************************************/

bool Deg_CheckIfDegNameExistsInCtr (const char *FieldName,const char *Name,
                                    long DegCod,long CtrCod)
  {
   /***** Get number of degrees with a type and a name from database *****/
   return (DB_QueryCOUNT ("can not check if the name of a degree"
			  " already existed",
			  "SELECT COUNT(*) FROM deg_degrees"
			  " WHERE CtrCod=%ld AND %s='%s' AND DegCod<>%ld",
			  CtrCod,FieldName,Name,DegCod) != 0);
  }

/*****************************************************************************/
/***************** Update degree name in table of degrees ********************/
/*****************************************************************************/

static void Deg_UpdateDegNameDB (long DegCod,const char *FieldName,const char *NewDegName)
  {
   /***** Update degree changing old name by new name *****/
   DB_QueryUPDATE ("can not update the name of a degree",
		   "UPDATE deg_degrees SET %s='%s' WHERE DegCod=%ld",
	           FieldName,NewDegName,DegCod);
  }

/*****************************************************************************/
/************************ Change the type of a degree ************************/
/*****************************************************************************/

void Deg_ChangeDegreeType (void)
  {
   extern const char *Txt_The_type_of_degree_of_the_degree_X_has_changed;
   long NewDegTypCod;

   /***** Degree constructor *****/
   Deg_EditingDegreeConstructor ();

   /***** Get parameters from form *****/
   /* Get degree code */
   Deg_EditingDeg->DegCod = Deg_GetAndCheckParamOtherDegCod (1);

   /* Get the new degree type */
   NewDegTypCod = DT_GetAndCheckParamOtherDegTypCod (1);

   /***** Get data of degree *****/
   Deg_GetDataOfDegreeByCod (Deg_EditingDeg);

   /***** Update the table of degrees changing old type by new type *****/
   DB_QueryUPDATE ("can not update the type of a degree",
		   "UPDATE deg_degrees SET DegTypCod=%ld WHERE DegCod=%ld",
	           NewDegTypCod,Deg_EditingDeg->DegCod);
   Deg_EditingDeg->DegTypCod = NewDegTypCod;

   /***** Create alert to show the change made
          and put button to go to degree changed *****/
   Ale_CreateAlert (Ale_SUCCESS,NULL,
	            Txt_The_type_of_degree_of_the_degree_X_has_changed,
	            Deg_EditingDeg->FullName);
  }

/*****************************************************************************/
/************************* Change the WWW of a degree ************************/
/*****************************************************************************/

void Deg_ChangeDegWWW (void)
  {
   extern const char *Txt_The_new_web_address_is_X;
   char NewWWW[Cns_MAX_BYTES_WWW + 1];

   /***** Degree constructor *****/
   Deg_EditingDegreeConstructor ();

   /***** Get parameters from form *****/
   /* Get the code of the degree */
   Deg_EditingDeg->DegCod = Deg_GetAndCheckParamOtherDegCod (1);

   /* Get the new WWW for the degree */
   Par_GetParToText ("WWW",NewWWW,Cns_MAX_BYTES_WWW);

   /***** Get data of degree *****/
   Deg_GetDataOfDegreeByCod (Deg_EditingDeg);

   /***** Check if new WWW is empty *****/
   if (NewWWW[0])
     {
      /***** Update the table changing old WWW by new WWW *****/
      Deg_UpdateDegWWWDB (Deg_EditingDeg->DegCod,NewWWW);
      Str_Copy (Deg_EditingDeg->WWW,NewWWW,sizeof (Deg_EditingDeg->WWW) - 1);

      /***** Write alert to show the change made
	     and put button to go to degree changed *****/
      Ale_CreateAlert (Ale_SUCCESS,NULL,
	               Txt_The_new_web_address_is_X,
		       NewWWW);
     }
   else
      Ale_CreateAlertYouCanNotLeaveFieldEmpty ();
  }

/*****************************************************************************/
/**************** Update database changing old WWW by new WWW ****************/
/*****************************************************************************/

void Deg_UpdateDegWWWDB (long DegCod,const char NewWWW[Cns_MAX_BYTES_WWW + 1])
  {
   /***** Update database changing old WWW by new WWW *****/
   DB_QueryUPDATE ("can not update the web of a degree",
		   "UPDATE deg_degrees SET WWW='%s' WHERE DegCod=%ld",
	           NewWWW,DegCod);
  }

/*****************************************************************************/
/*********************** Change the status of a degree ***********************/
/*****************************************************************************/

void Deg_ChangeDegStatus (void)
  {
   extern const char *Txt_The_status_of_the_degree_X_has_changed;
   Deg_Status_t Status;
   Deg_StatusTxt_t StatusTxt;

   /***** Degree constructor *****/
   Deg_EditingDegreeConstructor ();

   /***** Get parameters from form *****/
   /* Get degree code */
   Deg_EditingDeg->DegCod = Deg_GetAndCheckParamOtherDegCod (1);

   /* Get parameter with status */
   Status = (Deg_Status_t)
	    Par_GetParToUnsignedLong ("Status",
	                              0,
	                              (unsigned long) Deg_MAX_STATUS,
                                      (unsigned long) Deg_WRONG_STATUS);
   if (Status == Deg_WRONG_STATUS)
      Lay_ShowErrorAndExit ("Wrong status.");
   StatusTxt = Deg_GetStatusTxtFromStatusBits (Status);
   Status = Deg_GetStatusBitsFromStatusTxt (StatusTxt);	// New status

   /***** Get data of degree *****/
   Deg_GetDataOfDegreeByCod (Deg_EditingDeg);

   /***** Update status in table of degrees *****/
   DB_QueryUPDATE ("can not update the status of a degree",
		   "UPDATE deg_degrees SET Status=%u WHERE DegCod=%ld",
                   (unsigned) Status,Deg_EditingDeg->DegCod);
   Deg_EditingDeg->Status = Status;

   /***** Write alert to show the change made
	  and put button to go to degree changed *****/
   Ale_CreateAlert (Ale_SUCCESS,NULL,
	            Txt_The_status_of_the_degree_X_has_changed,
                    Deg_EditingDeg->ShrtName);
  }

/*****************************************************************************/
/********* Show alerts after changing a degree and continue editing **********/
/*****************************************************************************/

void Deg_ContEditAfterChgDeg (void)
  {
   /***** Write message to show the change made
	  and put button to go to degree changed *****/
   Deg_ShowAlertAndButtonToGoToDeg ();

   /***** Show the form again *****/
   Deg_EditDegreesInternal ();

   /***** Degree destructor *****/
   Deg_EditingDegreeDestructor ();
  }

/*****************************************************************************/
/***************** Write message to show the change made  ********************/
/***************** and put button to go to degree changed ********************/
/*****************************************************************************/

static void Deg_ShowAlertAndButtonToGoToDeg (void)
  {
   // If the degree being edited is different to the current one...
   if (Deg_EditingDeg->DegCod != Gbl.Hierarchy.Deg.DegCod)
     {
      /***** Alert with button to go to degree *****/
      Ale_ShowLastAlertAndButton (ActSeeCrs,NULL,NULL,
                                  Deg_PutParamGoToDeg,&Deg_EditingDeg->DegCod,
                                  Btn_CONFIRM_BUTTON,
				  Hie_BuildGoToMsg (Deg_EditingDeg->ShrtName));
      Hie_FreeGoToMsg ();
     }
   else
      /***** Alert *****/
      Ale_ShowAlerts (NULL);
  }

static void Deg_PutParamGoToDeg (void *DegCod)
  {
   if (DegCod)
      Deg_PutParamDegCod (*((long *) DegCod));
  }

/*****************************************************************************/
/*********** Show a form for sending a logo of the current degree ************/
/*****************************************************************************/

void Deg_RequestLogo (void)
  {
   Lgo_RequestLogo (Hie_Lvl_DEG);
  }

/*****************************************************************************/
/***************** Receive the logo of the current degree ********************/
/*****************************************************************************/

void Deg_ReceiveLogo (void)
  {
   Lgo_ReceiveLogo (Hie_Lvl_DEG);
  }

/*****************************************************************************/
/****************** Remove the logo of the current degree ********************/
/*****************************************************************************/

void Deg_RemoveLogo (void)
  {
   Lgo_RemoveLogo (Hie_Lvl_DEG);
  }

/*****************************************************************************/
/*********************** Get total number of degrees *************************/
/*****************************************************************************/

unsigned Deg_GetCachedNumDegsInSys (void)
  {
   unsigned NumDegs;

   /***** Get number of degrees from cache *****/
   if (!FigCch_GetFigureFromCache (FigCch_NUM_DEGS,Hie_Lvl_SYS,-1L,
                                   FigCch_UNSIGNED,&NumDegs))
     {
      /***** Get current number of degrees from database and update cache *****/
      NumDegs = (unsigned) DB_GetNumRowsTable ("deg_degrees");
      FigCch_UpdateFigureIntoCache (FigCch_NUM_DEGS,Hie_Lvl_SYS,-1L,
                                    FigCch_UNSIGNED,&NumDegs);
     }

   return NumDegs;
  }

/*****************************************************************************/
/********************* Get number of degrees in a country ********************/
/*****************************************************************************/

void Deg_FlushCacheNumDegsInCty (void)
  {
   Gbl.Cache.NumDegsInCty.CtyCod  = -1L;
   Gbl.Cache.NumDegsInCty.NumDegs = 0;
  }

unsigned Deg_GetNumDegsInCty (long CtyCod)
  {
   /***** 1. Fast check: Trivial case *****/
   if (CtyCod <= 0)
      return 0;

   /***** 2. Fast check: If cached... *****/
   if (CtyCod == Gbl.Cache.NumDegsInCty.CtyCod)
      return Gbl.Cache.NumDegsInCty.NumDegs;

   /***** 3. Slow: number of degrees in a country from database *****/
   Gbl.Cache.NumDegsInCty.CtyCod  = CtyCod;
   Gbl.Cache.NumDegsInCty.NumDegs = (unsigned)
   DB_QueryCOUNT ("can not get the number of degrees in a country",
		  "SELECT COUNT(*)"
		  " FROM ins_instits,"
		        "ctr_centers,"
		        "deg_degrees"
		  " WHERE ins_instits.CtyCod=%ld"
		  " AND ins_instits.InsCod=ctr_centers.InsCod"
		  " AND ctr_centers.CtrCod=deg_degrees.CtrCod",
		  CtyCod);
   FigCch_UpdateFigureIntoCache (FigCch_NUM_DEGS,Hie_Lvl_CTY,Gbl.Cache.NumDegsInCty.CtyCod,
				 FigCch_UNSIGNED,&Gbl.Cache.NumDegsInCty.NumDegs);
   return Gbl.Cache.NumDegsInCty.NumDegs;
  }

unsigned Deg_GetCachedNumDegsInCty (long CtyCod)
  {
   unsigned NumDegs;

   /***** Get number of degrees from cache *****/
   if (!FigCch_GetFigureFromCache (FigCch_NUM_DEGS,Hie_Lvl_CTY,CtyCod,
				   FigCch_UNSIGNED,&NumDegs))
      /***** Get current number of degrees from database and update cache *****/
      NumDegs = Deg_GetNumDegsInCty (CtyCod);

   return NumDegs;
  }

/*****************************************************************************/
/****************** Get number of degrees in an institution ******************/
/*****************************************************************************/

void Deg_FlushCacheNumDegsInIns (void)
  {
   Gbl.Cache.NumDegsInIns.InsCod  = -1L;
   Gbl.Cache.NumDegsInIns.NumDegs = 0;
  }

unsigned Deg_GetNumDegsInIns (long InsCod)
  {
   /***** 1. Fast check: Trivial case *****/
   if (InsCod <= 0)
      return 0;

   /***** 2. Fast check: If cached... *****/
   if (InsCod == Gbl.Cache.NumDegsInIns.InsCod)
      return Gbl.Cache.NumDegsInIns.NumDegs;

   /***** 3. Slow: number of degrees in an institution from database *****/
   Gbl.Cache.NumDegsInIns.InsCod  = InsCod;
   Gbl.Cache.NumDegsInIns.NumDegs = (unsigned)
   DB_QueryCOUNT ("can not get the number of degrees in an institution",
		  "SELECT COUNT(*)"
		  " FROM ctr_centers,"
		        "deg_degrees"
		  " WHERE ctr_centers.InsCod=%ld"
		  " AND ctr_centers.CtrCod=deg_degrees.CtrCod",
		  InsCod);
   FigCch_UpdateFigureIntoCache (FigCch_NUM_DEGS,Hie_Lvl_INS,Gbl.Cache.NumDegsInIns.InsCod,
				 FigCch_UNSIGNED,&Gbl.Cache.NumDegsInIns.NumDegs);
   return Gbl.Cache.NumDegsInIns.NumDegs;
  }

unsigned Deg_GetCachedNumDegsInIns (long InsCod)
  {
   unsigned NumDegs;

   /***** Get number of degrees from cache *****/
   if (!FigCch_GetFigureFromCache (FigCch_NUM_DEGS,Hie_Lvl_INS,InsCod,
				   FigCch_UNSIGNED,&NumDegs))
      /***** Get current number of degrees from database and update cache *****/
      NumDegs = Deg_GetNumDegsInIns (InsCod);

   return NumDegs;
  }

/*****************************************************************************/
/******************** Get number of degrees in a center **********************/
/*****************************************************************************/

void Deg_FlushCacheNumDegsInCtr (void)
  {
   Gbl.Cache.NumDegsInCtr.CtrCod  = -1L;
   Gbl.Cache.NumDegsInCtr.NumDegs = 0;
  }

unsigned Deg_GetNumDegsInCtr (long CtrCod)
  {
   /***** 1. Fast check: Trivial case *****/
   if (CtrCod <= 0)
      return 0;

   /***** 2. Fast check: If cached... *****/
   if (CtrCod == Gbl.Cache.NumDegsInCtr.CtrCod)
      return Gbl.Cache.NumDegsInCtr.NumDegs;

   /***** 3. Slow: number of degrees in a center from database *****/
   Gbl.Cache.NumDegsInCtr.CtrCod  = CtrCod;
   Gbl.Cache.NumDegsInCtr.NumDegs =
   (unsigned) DB_QueryCOUNT ("can not get the number of degrees in a center",
			     "SELECT COUNT(*) FROM deg_degrees"
			     " WHERE CtrCod=%ld",
			     CtrCod);
   FigCch_UpdateFigureIntoCache (FigCch_NUM_DEGS,Hie_Lvl_CTR,Gbl.Cache.NumDegsInCtr.CtrCod,
				 FigCch_UNSIGNED,&Gbl.Cache.NumDegsInCtr.NumDegs);
   return Gbl.Cache.NumDegsInCtr.NumDegs;
  }

unsigned Deg_GetCachedNumDegsInCtr (long CtrCod)
  {
   unsigned NumDegs;

   /***** Get number of degrees from cache *****/
   if (!FigCch_GetFigureFromCache (FigCch_NUM_DEGS,Hie_Lvl_CTR,CtrCod,
				   FigCch_UNSIGNED,&NumDegs))
      /***** Get current number of degrees from database and update cache *****/
      NumDegs = Deg_GetNumDegsInCtr (CtrCod);

   return NumDegs;
  }

/*****************************************************************************/
/********************* Get number of centers with courses ********************/
/*****************************************************************************/

unsigned Deg_GetCachedNumDegsWithCrss (const char *SubQuery,
                                       Hie_Lvl_Level_t Scope,long Cod)
  {
   unsigned NumDegsWithCrss;

   /***** Get number of degrees with courses from cache *****/
   if (!FigCch_GetFigureFromCache (FigCch_NUM_DEGS_WITH_CRSS,Scope,Cod,
				   FigCch_UNSIGNED,&NumDegsWithCrss))
     {
      /***** Get current number of degrees with courses from database and update cache *****/
      NumDegsWithCrss = (unsigned)
      DB_QueryCOUNT ("can not get number of degrees with courses",
		     "SELECT COUNT(DISTINCT deg_degrees.DegCod)"
		     " FROM ins_instits,"
		           "ctr_centers,"
		           "deg_degrees,"
		           "crs_courses"
		     " WHERE %sinstitutions.InsCod=ctr_centers.InsCod"
		     " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
		     " AND deg_degrees.DegCod=crs_courses.DegCod",
		     SubQuery);
      FigCch_UpdateFigureIntoCache (FigCch_NUM_DEGS_WITH_CRSS,Scope,Cod,
				    FigCch_UNSIGNED,&NumDegsWithCrss);
     }

   return NumDegsWithCrss;
  }

/*****************************************************************************/
/********************* Get number of degrees with users **********************/
/*****************************************************************************/

unsigned Deg_GetCachedNumDegsWithUsrs (Rol_Role_t Role,const char *SubQuery,
                                       Hie_Lvl_Level_t Scope,long Cod)
  {
   static const FigCch_FigureCached_t FigureDegs[Rol_NUM_ROLES] =
     {
      [Rol_STD] = FigCch_NUM_DEGS_WITH_STDS,	// Students
      [Rol_NET] = FigCch_NUM_DEGS_WITH_NETS,	// Non-editing teachers
      [Rol_TCH] = FigCch_NUM_DEGS_WITH_TCHS,	// Teachers
     };
   unsigned NumDegsWithUsrs;

   /***** Get number of degrees with users from cache *****/
   if (!FigCch_GetFigureFromCache (FigureDegs[Role],Scope,Cod,
				   FigCch_UNSIGNED,&NumDegsWithUsrs))
     {
      /***** Get current number of degrees with users from database and update cache *****/
      NumDegsWithUsrs = (unsigned)
      DB_QueryCOUNT ("can not get number of degrees with users",
		     "SELECT COUNT(DISTINCT deg_degrees.DegCod)"
		      " FROM ins_instits,"
		            "ctr_centers,"
		            "deg_degrees,"
		            "crs_courses,"
		            "crs_users"
		     " WHERE %sinstitutions.InsCod=ctr_centers.InsCod"
		       " AND ctr_centers.CtrCod=deg_degrees.CtrCod"
		       " AND deg_degrees.DegCod=crs_courses.DegCod"
		       " AND crs_courses.CrsCod=crs_users.CrsCod"
		       " AND crs_users.Role=%u",
		     SubQuery,(unsigned) Role);
      FigCch_UpdateFigureIntoCache (FigureDegs[Role],Scope,Cod,
				    FigCch_UNSIGNED,&NumDegsWithUsrs);
     }

   return NumDegsWithUsrs;
  }

/*****************************************************************************/
/**************************** List degrees found *****************************/
/*****************************************************************************/

void Deg_ListDegsFound (MYSQL_RES **mysql_res,unsigned NumDegs)
  {
   extern const char *Txt_degree;
   extern const char *Txt_degrees;
   MYSQL_ROW row;
   unsigned NumDeg;
   struct Deg_Degree Deg;

   /***** Query database *****/
   if (NumDegs)
     {
      /***** Begin box and table *****/
      /* Number of degrees found */
      Box_BoxTableBegin (NULL,Str_BuildStringLongStr ((long) NumDegs,
						      (NumDegs == 1) ? Txt_degree :
								       Txt_degrees),
			 NULL,NULL,
			 NULL,Box_NOT_CLOSABLE,2);
      Str_FreeString ();

      /***** Write heading *****/
      Deg_PutHeadDegreesForSeeing ();

      /***** List the degrees (one row per degree) *****/
      for (NumDeg  = 1;
	   NumDeg <= NumDegs;
	   NumDeg++)
	{
	 /* Get next degree */
	 row = mysql_fetch_row (*mysql_res);

	 /* Get degree code (row[0]) */
	 Deg.DegCod = Str_ConvertStrCodToLongCod (row[0]);

	 /* Get data of degree */
	 Deg_GetDataOfDegreeByCod (&Deg);

	 /* Write data of this degree */
	 Deg_ListOneDegreeForSeeing (&Deg,NumDeg);
	}

      /***** End table and box *****/
      Box_BoxTableEnd ();
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (mysql_res);
  }

/*****************************************************************************/
/************************ Degree constructor/destructor **********************/
/*****************************************************************************/

static void Deg_EditingDegreeConstructor (void)
  {
   /***** Pointer must be NULL *****/
   if (Deg_EditingDeg != NULL)
      Lay_ShowErrorAndExit ("Error initializing degree.");

   /***** Allocate memory for degree *****/
   if ((Deg_EditingDeg = malloc (sizeof (*Deg_EditingDeg))) == NULL)
      Lay_NotEnoughMemoryExit ();

   /***** Reset degree *****/
   Deg_EditingDeg->DegCod          = -1L;
   Deg_EditingDeg->DegTypCod       = -1L;
   Deg_EditingDeg->CtrCod          = -1L;
   Deg_EditingDeg->Status          = (Deg_Status_t) 0;
   Deg_EditingDeg->RequesterUsrCod = -1L;
   Deg_EditingDeg->ShrtName[0]     = '\0';
   Deg_EditingDeg->FullName[0]     = '\0';
   Deg_EditingDeg->WWW[0]          = '\0';
  }

static void Deg_EditingDegreeDestructor (void)
  {
   /***** Free memory used for degree *****/
   if (Deg_EditingDeg != NULL)
     {
      free (Deg_EditingDeg);
      Deg_EditingDeg = NULL;
     }
  }
