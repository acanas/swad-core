// swad_hierarchy.c: hierarchy (system, institution, centre, degree, course)

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2020 Antonio Cañas Vargas

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
#include "swad_form.h"
#include "swad_global.h"
#include "swad_HTML.h"
#include "swad_logo.h"

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

/*****************************************************************************/
/*************************** Private constants *******************************/
/*****************************************************************************/

/*****************************************************************************/
/*************************** Private prototypes ******************************/
/*****************************************************************************/

/*****************************************************************************/
/********** List pending institutions, centres, degrees and courses **********/
/*****************************************************************************/

void Hie_SeePending (void)
  {
   /***** Contextual menu *****/
   Mnu_ContextMenuBegin ();
   Crs_PutLinkToRemoveOldCrss ();	// Remove old courses
   Mnu_ContextMenuEnd ();

   /***** List countries with pending institutions *****/
   Cty_SeeCtyWithPendingInss ();

   /***** List institutions with pending centres *****/
   Ins_SeeInsWithPendingCtrs ();

   /***** List centres with pending degrees *****/
   Ctr_SeeCtrWithPendingDegs ();

   /***** List degrees with pending courses *****/
   Deg_SeeDegWithPendingCrss ();
  }

/*****************************************************************************/
/*** Write menu to select country, institution, centre, degree and course ****/
/*****************************************************************************/

void Hie_WriteMenuHierarchy (void)
  {
   extern const char *Txt_Country;
   extern const char *Txt_Institution;
   extern const char *Txt_Centre;
   extern const char *Txt_Degree;
   extern const char *Txt_Course;

   /***** Begin table *****/
   HTM_TABLE_BeginCenterPadding (2);

   /***** Write a 1st selector
          with all the countries *****/
   HTM_TR_Begin (NULL);

   /* Label */
   Frm_LabelColumn ("RT","cty",Txt_Country);

   /* Data */
   HTM_TD_Begin ("class=\"LT\"");
   Cty_WriteSelectorOfCountry ();
   HTM_TD_End ();

   HTM_TR_End ();

   if (Gbl.Hierarchy.Cty.CtyCod > 0)
     {
      /***** Write a 2nd selector
             with the institutions of selected country *****/
      HTM_TR_Begin (NULL);

      /* Label */
      Frm_LabelColumn ("RT","ins",Txt_Institution);

      /* Data */
      HTM_TD_Begin ("class=\"LT\"");
      Ins_WriteSelectorOfInstitution ();
      HTM_TD_End ();

      HTM_TR_End ();

      if (Gbl.Hierarchy.Ins.InsCod > 0)
        {
         /***** Write a 3rd selector
                with all the centres of selected institution *****/
         HTM_TR_Begin (NULL);

         /* Label */
         Frm_LabelColumn ("RT","ctr",Txt_Centre);

         /* Data */
         HTM_TD_Begin ("class=\"LT\"");
         Ctr_WriteSelectorOfCentre ();
         HTM_TD_End ();

         HTM_TR_End ();

         if (Gbl.Hierarchy.Ctr.CtrCod > 0)
           {
            /***** Write a 4th selector
                   with all the degrees of selected centre *****/
            HTM_TR_Begin (NULL);

            /* Label */
            Frm_LabelColumn ("RT","deg",Txt_Degree);

            /* Data */
            HTM_TD_Begin ("class=\"LT\"");
            Deg_WriteSelectorOfDegree ();
            HTM_TD_End ();

            HTM_TR_End ();

	    if (Gbl.Hierarchy.Deg.DegCod > 0)
	      {
	       /***** Write a 5th selector
		      with all the courses of selected degree *****/
	       HTM_TR_Begin (NULL);

	       /* Label */
               Frm_LabelColumn ("RT","crs",Txt_Course);

               /* Data */
	       HTM_TD_Begin ("class=\"LT\"");
	       Crs_WriteSelectorOfCourse ();
	       HTM_TD_End ();

	       HTM_TR_End ();
	      }
           }
        }
     }

   /***** End table *****/
   HTM_TABLE_End ();
  }

/*****************************************************************************/
/************* Write hierarchy breadcrumb in the top of the page *************/
/*****************************************************************************/

void Hie_WriteHierarchyInBreadcrumb (void)
  {
   extern const char *The_ClassBreadcrumb[The_NUM_THEMES];
   extern const char *Txt_System;
   extern const char *Txt_Country;
   extern const char *Txt_Institution;
   extern const char *Txt_Centre;
   extern const char *Txt_Degree;
   const char *ClassTxt = The_ClassBreadcrumb[Gbl.Prefs.Theme];
   char *ClassLink;

   /***** Create CSS class of links *****/
   if (asprintf (&ClassLink,"BT_LINK %s",ClassTxt) < 0)
      Lay_NotEnoughMemoryExit ();

   /***** Form to go to the system *****/
   HTM_DIV_Begin ("class=\"BC %s\"",ClassTxt);
   HTM_NBSP ();

   Frm_StartFormGoTo (ActMnu);
   Par_PutHiddenParamUnsigned (NULL,"NxtTab",(unsigned) TabSys);
   HTM_BUTTON_SUBMIT_Begin (Txt_System,ClassLink,NULL);
   HTM_Txt (Txt_System);
   HTM_BUTTON_End ();
   Frm_EndForm ();

   HTM_DIV_End ();

   if (Gbl.Hierarchy.Cty.CtyCod > 0)		// Country selected...
     {
      HTM_DIV_Begin ("class=\"BC %s\"",ClassTxt);

      /***** Separator *****/
      HTM_Txt ("&nbsp;&gt;&nbsp;");

      /***** Form to go to see institutions of this country *****/
      Frm_StartFormGoTo (ActSeeIns);
      Cty_PutParamCtyCod (Gbl.Hierarchy.Cty.CtyCod);
      HTM_BUTTON_SUBMIT_Begin (Gbl.Hierarchy.Cty.Name[Gbl.Prefs.Language],ClassLink,NULL);
      HTM_Txt (Gbl.Hierarchy.Cty.Name[Gbl.Prefs.Language]);
      HTM_BUTTON_End ();
      Frm_EndForm ();

      HTM_DIV_End ();
     }
   else
     {
      HTM_DIV_Begin ("class=\"BC BC_SEMIOFF %s\"",ClassTxt);

      /***** Separator *****/
      HTM_Txt ("&nbsp;&gt;&nbsp;");

      /***** Form to go to select countries *****/
      Frm_StartFormGoTo (ActSeeCty);
      HTM_BUTTON_SUBMIT_Begin (Txt_Country,ClassLink,NULL);
      HTM_Txt (Txt_Country);
      HTM_BUTTON_End ();
      Frm_EndForm ();

      HTM_DIV_End ();
     }

   if (Gbl.Hierarchy.Ins.InsCod > 0)		// Institution selected...
     {
      HTM_DIV_Begin ("class=\"BC %s\"",ClassTxt);

      /***** Separator *****/
      HTM_Txt ("&nbsp;&gt;&nbsp;");

      /***** Form to see centres of this institution *****/
      Frm_StartFormGoTo (ActSeeCtr);
      Ins_PutParamInsCod (Gbl.Hierarchy.Ins.InsCod);
      HTM_BUTTON_SUBMIT_Begin (Gbl.Hierarchy.Ins.FullName,ClassLink,NULL);
      HTM_Txt (Gbl.Hierarchy.Ins.ShrtName);
      HTM_BUTTON_End ();
      Frm_EndForm ();

      HTM_DIV_End ();
     }
   else if (Gbl.Hierarchy.Cty.CtyCod > 0)
     {
      HTM_DIV_Begin ("class=\"BC BC_SEMIOFF %s\"",ClassTxt);

      /***** Separator *****/
      HTM_Txt ("&nbsp;&gt;&nbsp;");

      /***** Form to go to select institutions *****/
      Frm_StartFormGoTo (ActSeeIns);
      HTM_BUTTON_SUBMIT_Begin (Txt_Institution,ClassLink,NULL);
      HTM_Txt (Txt_Institution);
      HTM_BUTTON_End ();
      Frm_EndForm ();

      HTM_DIV_End ();
     }
   else
     {
      HTM_DIV_Begin ("class=\"BC BC_OFF %s\"",ClassTxt);

      /***** Separator *****/
      HTM_Txt ("&nbsp;&gt;&nbsp;");

      /***** Hidden institution *****/
      HTM_Txt (Txt_Institution);

      HTM_DIV_End ();
     }

   if (Gbl.Hierarchy.Ctr.CtrCod > 0)	// Centre selected...
     {
      HTM_DIV_Begin ("class=\"BC %s\"",ClassTxt);

      /***** Separator *****/
      HTM_Txt ("&nbsp;&gt;&nbsp;");

      /***** Form to see degrees of this centre *****/
      Frm_StartFormGoTo (ActSeeDeg);
      Ctr_PutParamCtrCod (Gbl.Hierarchy.Ctr.CtrCod);
      HTM_BUTTON_SUBMIT_Begin (Gbl.Hierarchy.Ctr.FullName,ClassLink,NULL);
      HTM_Txt (Gbl.Hierarchy.Ctr.ShrtName);
      HTM_BUTTON_End ();
      Frm_EndForm ();

      HTM_DIV_End ();
     }
   else if (Gbl.Hierarchy.Ins.InsCod > 0)
     {
      HTM_DIV_Begin ("class=\"BC BC_SEMIOFF %s\"",ClassTxt);

      /***** Separator *****/
      HTM_Txt ("&nbsp;&gt;&nbsp;");

      /***** Form to go to select centres *****/
      Frm_StartFormGoTo (ActSeeCtr);
      HTM_BUTTON_SUBMIT_Begin (Txt_Centre,ClassLink,NULL);
      HTM_Txt (Txt_Centre);
      HTM_BUTTON_End ();
      Frm_EndForm ();

      HTM_DIV_End ();
     }
   else
     {
      HTM_DIV_Begin ("class=\"BC BC_OFF %s\"",ClassTxt);

      /***** Separator *****/
      HTM_Txt ("&nbsp;&gt;&nbsp;");

      /***** Hidden centre *****/
      HTM_Txt (Txt_Centre);

      HTM_DIV_End ();
     }

   if (Gbl.Hierarchy.Deg.DegCod > 0)	// Degree selected...
     {
      HTM_DIV_Begin ("class=\"BC %s\"",ClassTxt);

      /***** Separator *****/
      HTM_Txt ("&nbsp;&gt;&nbsp;");

      /***** Form to go to see courses of this degree *****/
      Frm_StartFormGoTo (ActSeeCrs);
      Deg_PutParamDegCod (Gbl.Hierarchy.Deg.DegCod);
      HTM_BUTTON_SUBMIT_Begin (Gbl.Hierarchy.Deg.FullName,ClassLink,NULL);
      HTM_Txt (Gbl.Hierarchy.Deg.ShrtName);
      HTM_BUTTON_End ();
      Frm_EndForm ();

      HTM_DIV_End ();
     }
   else if (Gbl.Hierarchy.Ctr.CtrCod > 0)
     {
      HTM_DIV_Begin ("class=\"BC BC_SEMIOFF %s\"",ClassTxt);

      /***** Separator *****/
      HTM_Txt ("&nbsp;&gt;&nbsp;");

      /***** Form to go to select degrees *****/
      Frm_StartFormGoTo (ActSeeDeg);
      HTM_BUTTON_SUBMIT_Begin (Txt_Degree,ClassLink,NULL);
      HTM_Txt (Txt_Degree);
      HTM_BUTTON_End ();
      Frm_EndForm ();

      HTM_DIV_End ();
     }
   else
     {
      HTM_DIV_Begin ("class=\"BC BC_OFF %s\"",ClassTxt);

      /***** Separator *****/
      HTM_Txt ("&nbsp;&gt;&nbsp;");

      /***** Hidden degree *****/
      HTM_Txt (Txt_Degree);

      HTM_DIV_End ();
     }

   HTM_DIV_Begin ("class=\"BC%s %s\"",
		   (Gbl.Hierarchy.Level == Hie_CRS) ? "" :
		  ((Gbl.Hierarchy.Deg.DegCod > 0) ? " BC_SEMIOFF" :
						    " BC_OFF"),
		  ClassTxt);

   /***** Separator *****/
   HTM_Txt ("&nbsp;&gt;&nbsp;");

   HTM_DIV_End ();

   /***** Free memory used for CSS class of links *****/
   free (ClassLink);
  }

/*****************************************************************************/
/*************** Write course full name in the top of the page ***************/
/*****************************************************************************/

void Hie_WriteBigNameCtyInsCtrDegCrs (void)
  {
   extern const char *The_ClassCourse[The_NUM_THEMES];
   extern const char *Txt_TAGLINE;

   HTM_TxtF ("<h1 id=\"main_title\" class=\"%s\">",
	     The_ClassCourse[Gbl.Prefs.Theme]);

   /***** Logo *****/
   switch (Gbl.Hierarchy.Level)
     {
      case Hie_SYS:	// System
	 Ico_PutIcon ("swad64x64.png",Cfg_PLATFORM_FULL_NAME,"ICO40x40 TOP_LOGO");
         break;
      case Hie_CTY:	// Country
         Cty_DrawCountryMap (&Gbl.Hierarchy.Cty,"COUNTRY_MAP_TITLE");
         break;
      case Hie_INS:	// Institution
	 Lgo_DrawLogo (Hie_INS,Gbl.Hierarchy.Ins.InsCod,
		       Gbl.Hierarchy.Ins.ShrtName,40,"TOP_LOGO",false);
         break;
      case Hie_CTR:	// Centre
	 Lgo_DrawLogo (Hie_CTR,Gbl.Hierarchy.Ctr.CtrCod,
		       Gbl.Hierarchy.Ctr.ShrtName,40,"TOP_LOGO",false);
         break;
      case Hie_DEG:	// Degree
      case Hie_CRS:	// Course
	 Lgo_DrawLogo (Hie_DEG,Gbl.Hierarchy.Deg.DegCod,
		       Gbl.Hierarchy.Deg.ShrtName,40,"TOP_LOGO",false);
         break;
      default:
	 break;
     }

   /***** Text *****/
   HTM_DIV_Begin ("id=\"big_name_container\"");
   if (Gbl.Hierarchy.Cty.CtyCod > 0)
     {
      HTM_DIV_Begin ("id=\"big_full_name\"");
      HTM_Txt (	(Gbl.Hierarchy.Level == Hie_CRS) ? Gbl.Hierarchy.Crs.FullName :// Full name
	       ((Gbl.Hierarchy.Level == Hie_DEG) ? Gbl.Hierarchy.Deg.FullName :
	       ((Gbl.Hierarchy.Level == Hie_CTR) ? Gbl.Hierarchy.Ctr.FullName :
	       ((Gbl.Hierarchy.Level == Hie_INS) ? Gbl.Hierarchy.Ins.FullName :
	                                           Gbl.Hierarchy.Cty.Name[Gbl.Prefs.Language]))));
      HTM_DIV_End ();

      HTM_DIV_Begin ("class=\"NOT_SHOWN\"");
      HTM_Txt (" / ");	// To separate
      HTM_DIV_End ();

      HTM_DIV_Begin ("id=\"big_short_name\"");
      HTM_Txt (	(Gbl.Hierarchy.Level == Hie_CRS) ? Gbl.Hierarchy.Crs.ShrtName :// Short name
	       ((Gbl.Hierarchy.Level == Hie_DEG) ? Gbl.Hierarchy.Deg.ShrtName :
	       ((Gbl.Hierarchy.Level == Hie_CTR) ? Gbl.Hierarchy.Ctr.ShrtName :
	       ((Gbl.Hierarchy.Level == Hie_INS) ? Gbl.Hierarchy.Ins.ShrtName :
	                                           Gbl.Hierarchy.Cty.Name[Gbl.Prefs.Language]))));
      HTM_DIV_End ();
     }
   else	// No country specified ==> home page
     {
      HTM_DIV_Begin ("id=\"big_full_name\"");	// Full name
      HTM_TxtF ("%s:&nbsp;%s",Cfg_PLATFORM_SHORT_NAME,Txt_TAGLINE);
      HTM_DIV_End ();

      HTM_DIV_Begin ("class=\"NOT_SHOWN\"");
      HTM_Txt (" / ");	// To separate
      HTM_DIV_End ();

      HTM_DIV_Begin ("id=\"big_short_name\"");	// Short name
      HTM_Txt (Cfg_PLATFORM_SHORT_NAME);
      HTM_DIV_End ();
     }
   HTM_DIV_End ();
   HTM_TxtF ("</h1>");
  }

/*****************************************************************************/
/**************** Copy last hierarchy to current hierarchy *******************/
/*****************************************************************************/

void Hie_SetHierarchyFromUsrLastHierarchy (void)
  {
   /***** Initialize all codes to -1 *****/
   Hie_ResetHierarchy ();

   /***** Copy last hierarchy scope and code to current hierarchy *****/
   switch (Gbl.Usrs.Me.UsrLast.LastHie.Scope)
     {
      case Hie_CTY:	// Country
         Gbl.Hierarchy.Cty.CtyCod = Gbl.Usrs.Me.UsrLast.LastHie.Cod;
	 break;
      case Hie_INS:	// Institution
         Gbl.Hierarchy.Ins.InsCod = Gbl.Usrs.Me.UsrLast.LastHie.Cod;
	 break;
      case Hie_CTR:	// Centre
         Gbl.Hierarchy.Ctr.CtrCod = Gbl.Usrs.Me.UsrLast.LastHie.Cod;
	 break;
      case Hie_DEG:	// Degree
         Gbl.Hierarchy.Deg.DegCod = Gbl.Usrs.Me.UsrLast.LastHie.Cod;
	 break;
      case Hie_CRS:	// Course
         Gbl.Hierarchy.Crs.CrsCod = Gbl.Usrs.Me.UsrLast.LastHie.Cod;
	 break;
      default:
	 break;
     }

   /****** Initialize again current course, degree, centre... ******/
   Hie_InitHierarchy ();
  }

/*****************************************************************************/
/**** Initialize current country, institution, centre, degree and course *****/
/*****************************************************************************/

void Hie_InitHierarchy (void)
  {
   /***** If course code is available, get course data *****/
   if (Gbl.Hierarchy.Crs.CrsCod > 0)
     {
      if (Crs_GetDataOfCourseByCod (&Gbl.Hierarchy.Crs))	// Course found
         Gbl.Hierarchy.Deg.DegCod = Gbl.Hierarchy.Crs.DegCod;
      else
         Hie_ResetHierarchy ();
     }

   /***** If degree code is available, get degree data *****/
   if (Gbl.Hierarchy.Deg.DegCod > 0)
     {
      if (Deg_GetDataOfDegreeByCod (&Gbl.Hierarchy.Deg))	// Degree found
	{
	 Gbl.Hierarchy.Ctr.CtrCod = Gbl.Hierarchy.Deg.CtrCod;
         Gbl.Hierarchy.Ins.InsCod = Deg_GetInsCodOfDegreeByCod (Gbl.Hierarchy.Deg.DegCod);
	}
      else
         Hie_ResetHierarchy ();
     }

   /***** If centre code is available, get centre data *****/
   if (Gbl.Hierarchy.Ctr.CtrCod > 0)
     {
      if (Ctr_GetDataOfCentreByCod (&Gbl.Hierarchy.Ctr))	// Centre found
         Gbl.Hierarchy.Ins.InsCod = Gbl.Hierarchy.Ctr.InsCod;
      else
         Hie_ResetHierarchy ();
     }

   /***** If institution code is available, get institution data *****/
   if (Gbl.Hierarchy.Ins.InsCod > 0)
     {
      if (Ins_GetDataOfInstitutionByCod (&Gbl.Hierarchy.Ins))	// Institution found
	 Gbl.Hierarchy.Cty.CtyCod = Gbl.Hierarchy.Ins.CtyCod;
      else
         Hie_ResetHierarchy ();
     }

   /***** If country code is available, get country data *****/
   if (Gbl.Hierarchy.Cty.CtyCod > 0)
     {
      if (!Cty_GetDataOfCountryByCod (&Gbl.Hierarchy.Cty,Cty_GET_BASIC_DATA))		// Country not found
         Hie_ResetHierarchy ();
     }

   /***** Set current hierarchy level and code
          depending on course code, degree code, etc. *****/
   if      (Gbl.Hierarchy.Crs.CrsCod > 0)	// Course selected
     {
      Gbl.Hierarchy.Level = Hie_CRS;
      Gbl.Hierarchy.Cod = Gbl.Hierarchy.Crs.CrsCod;
     }
   else if (Gbl.Hierarchy.Deg.DegCod > 0)	// Degree selected
     {
      Gbl.Hierarchy.Level = Hie_DEG;
      Gbl.Hierarchy.Cod = Gbl.Hierarchy.Deg.DegCod;
     }
   else if (Gbl.Hierarchy.Ctr.CtrCod > 0)	// Centre selected
     {
      Gbl.Hierarchy.Level = Hie_CTR;
      Gbl.Hierarchy.Cod = Gbl.Hierarchy.Ctr.CtrCod;
     }
   else if (Gbl.Hierarchy.Ins.InsCod > 0)	// Institution selected
     {
      Gbl.Hierarchy.Level = Hie_INS;
      Gbl.Hierarchy.Cod = Gbl.Hierarchy.Ins.InsCod;
     }
   else if (Gbl.Hierarchy.Cty.CtyCod > 0)	// Country selected
     {
      Gbl.Hierarchy.Level = Hie_CTY;
      Gbl.Hierarchy.Cod = Gbl.Hierarchy.Cty.CtyCod;
     }
   else
     {
      Gbl.Hierarchy.Level = Hie_SYS;
      Gbl.Hierarchy.Cod = -1L;
     }

   /***** Initialize paths *****/
   if (Gbl.Hierarchy.Level == Hie_CRS)	// Course selected
     {
      /***** Paths of course directories *****/
      snprintf (Gbl.Crs.PathPriv,sizeof (Gbl.Crs.PathPriv),
	        "%s/%ld",
	        Cfg_PATH_CRS_PRIVATE,Gbl.Hierarchy.Crs.CrsCod);
      snprintf (Gbl.Crs.PathRelPubl,sizeof (Gbl.Crs.PathRelPubl),
	        "%s/%ld",
	        Cfg_PATH_CRS_PUBLIC,Gbl.Hierarchy.Crs.CrsCod);
      snprintf (Gbl.Crs.PathURLPubl,sizeof (Gbl.Crs.PathURLPubl),
	        "%s/%ld",
	        Cfg_URL_CRS_PUBLIC,Gbl.Hierarchy.Crs.CrsCod);

      /***** If any of the course directories does not exist, create it *****/
      if (!Fil_CheckIfPathExists (Gbl.Crs.PathPriv))
	 Fil_CreateDirIfNotExists (Gbl.Crs.PathPriv);
      if (!Fil_CheckIfPathExists (Gbl.Crs.PathRelPubl))
	 Fil_CreateDirIfNotExists (Gbl.Crs.PathRelPubl);

      /***** Count number of groups in current course
             (used in some actions) *****/
      Gbl.Crs.Grps.NumGrps = Grp_CountNumGrpsInCurrentCrs ();
     }
  }

/*****************************************************************************/
/******* Reset current country, institution, centre, degree and course *******/
/*****************************************************************************/

void Hie_ResetHierarchy (void)
  {
   /***** Country *****/
   Gbl.Hierarchy.Cty.CtyCod = -1L;

   /***** Institution *****/
   Gbl.Hierarchy.Ins.InsCod = -1L;

   /***** Centre *****/
   Gbl.Hierarchy.Ctr.CtrCod = -1L;
   Gbl.Hierarchy.Ctr.InsCod = -1L;
   Gbl.Hierarchy.Ctr.PlcCod = -1L;

   /***** Degree *****/
   Gbl.Hierarchy.Deg.DegCod = -1L;

   /***** Course *****/
   Gbl.Hierarchy.Crs.CrsCod = -1L;

   /***** Hierarchy level and code *****/
   Gbl.Hierarchy.Level = Hie_UNK;
   Gbl.Hierarchy.Cod   = -1L;
  }

/*****************************************************************************/
/***** Write institutions, centres and degrees administrated by an admin *****/
/*****************************************************************************/

void Hie_GetAndWriteInsCtrDegAdminBy (long UsrCod,unsigned ColSpan)
  {
   extern const char *Txt_all_degrees;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumRow;
   unsigned NumRows;
   struct Instit Ins;
   struct Centre Ctr;
   struct Degree Deg;

   /***** Get institutions, centres, degrees admin by user from database *****/
   NumRows = (unsigned) DB_QuerySELECT (&mysql_res,"can not get institutions,"
						   " centres, degrees"
						   " admin by a user",
				        "(SELECT %u AS S,-1 AS Cod,'' AS FullName"
				        " FROM admin"
				        " WHERE UsrCod=%ld"
				        " AND Scope='%s')"
				        " UNION "
				        "(SELECT %u AS S,admin.Cod,institutions.FullName"
				        " FROM admin,institutions"
				        " WHERE admin.UsrCod=%ld"
				        " AND admin.Scope='%s'"
				        " AND admin.Cod=institutions.InsCod)"
				        " UNION "
				        "(SELECT %u AS S,admin.Cod,centres.FullName"
				        " FROM admin,centres"
				        " WHERE admin.UsrCod=%ld"
				        " AND admin.Scope='%s'"
				        " AND admin.Cod=centres.CtrCod)"
				        " UNION "
				        "(SELECT %u AS S,admin.Cod,degrees.FullName"
				        " FROM admin,degrees"
				        " WHERE admin.UsrCod=%ld"
				        " AND admin.Scope='%s'"
				        " AND admin.Cod=degrees.DegCod)"
				        " ORDER BY S,FullName",
				        (unsigned) Hie_SYS,UsrCod,Sco_GetDBStrFromScope (Hie_SYS),
				        (unsigned) Hie_INS,UsrCod,Sco_GetDBStrFromScope (Hie_INS),
				        (unsigned) Hie_CTR,UsrCod,Sco_GetDBStrFromScope (Hie_CTR),
				        (unsigned) Hie_DEG,UsrCod,Sco_GetDBStrFromScope (Hie_DEG));
   if (NumRows)
      /***** Get the list of degrees *****/
      for (NumRow = 1;
	   NumRow <= NumRows;
	   NumRow++)
	{
         HTM_TR_Begin (NULL);

         /***** Indent *****/
         HTM_TD_Begin ("class=\"RT COLOR%u\"",Gbl.RowEvenOdd);
         Ico_PutIcon (NumRow == NumRows ? "subend20x20.gif" :
                	                  "submid20x20.gif",
		      "","ICO25x25");
         HTM_TD_End ();

         /***** Write institution, centre, degree *****/
         HTM_TD_Begin ("colspan=\"%u\" class=\"DAT_SMALL_NOBR LT COLOR%u\"",
                       ColSpan - 1,Gbl.RowEvenOdd);

         /* Get next institution, centre, degree */
         row = mysql_fetch_row (mysql_res);

	 /* Get scope */
	 switch (Sco_GetScopeFromUnsignedStr (row[0]))
	   {
	    case Hie_SYS:	// System
	       Ico_PutIcon ("swad64x64.png",Txt_all_degrees,"ICO16x16");
	       HTM_TxtF ("&nbsp;%s",Txt_all_degrees);
	       break;
	    case Hie_INS:	// Institution
	       Ins.InsCod = Str_ConvertStrCodToLongCod (row[1]);
	       if (Ins.InsCod > 0)
		 {
		  /* Get data of institution */
		  Ins_GetDataOfInstitutionByCod (&Ins);

		  /* Write institution logo and name */
		  Ins_DrawInstitutionLogoAndNameWithLink (&Ins,ActSeeInsInf,
						          "BT_LINK DAT_SMALL_NOBR","LT");
		 }
	       break;
	    case Hie_CTR:	// Centre
	       Ctr.CtrCod = Str_ConvertStrCodToLongCod (row[1]);
	       if (Ctr.CtrCod > 0)
		 {
		  /* Get data of centre */
		  Ctr_GetDataOfCentreByCod (&Ctr);

		  /* Write centre logo and name */
		  Ctr_DrawCentreLogoAndNameWithLink (&Ctr,ActSeeCtrInf,
						     "BT_LINK DAT_SMALL_NOBR","LT");
		 }
	       break;
	    case Hie_DEG:	// Degree
	       Deg.DegCod = Str_ConvertStrCodToLongCod (row[1]);
	       if (Deg.DegCod > 0)
		 {
		  /* Get data of degree */
		  Deg_GetDataOfDegreeByCod (&Deg);

		  /* Write degree logo and name */
		  Deg_DrawDegreeLogoAndNameWithLink (&Deg,ActSeeDegInf,
						     "BT_LINK DAT_SMALL_NOBR","LT");
		 }
	       break;
	    default:	// There are no administrators in other scopes
	       Lay_WrongScopeExit ();
	       break;
           }
         HTM_TD_End ();

         HTM_TR_End ();
        }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/********************* Build a "Go to <where>" message ***********************/
/*****************************************************************************/
// Where is a hierarchy member (country, institution, centre, degree or course
// Hie_FreeGoToMsg() must be called after calling this function

char *Hie_BuildGoToMsg (const char *Where)
  {
   extern const char *Txt_Go_to_X;

   return Str_BuildStringStr (Txt_Go_to_X,Where);
  }

void Hie_FreeGoToMsg (void)
  {
   Str_FreeString ();
  }
