// swad_hierarchy.c: hierarchy (system, institution, centre, degree, course)

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2019 Antonio Cañas Vargas

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

#include <stdio.h>		// For fprintf, etc.
#include <string.h>		// For string functions

#include "swad_config.h"
#include "swad_degree.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_logo.h"
#include "swad_table.h"
#include "swad_theme.h"

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
/**************************** Private constants ******************************/
/*****************************************************************************/

/*****************************************************************************/
/**************************** Private prototypes *****************************/
/*****************************************************************************/

/*****************************************************************************/
/********** List pending institutions, centres, degrees and courses **********/
/*****************************************************************************/

void Hie_SeePending (void)
  {
   /***** Put contextual links *****/
   fprintf (Gbl.F.Out,"<div class=\"CONTEXT_MENU\">");

   /* Put link to remove old courses */
   Crs_PutLinkToRemoveOldCrss ();

   fprintf (Gbl.F.Out,"</div>");

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
   extern const char *The_ClassForm[The_NUM_THEMES];
   extern const char *Txt_Country;
   extern const char *Txt_Institution;
   extern const char *Txt_Centre;
   extern const char *Txt_Degree;
   extern const char *Txt_Course;

   /***** Start table *****/
   Tbl_StartTableCenter (2);

   /***** Write a 1st selector
          with all the countries *****/
   fprintf (Gbl.F.Out,"<tr>"
                      "<td class=\"RIGHT_MIDDLE\">"
                      "<label for=\"cty\" class=\"%s\">%s:</label>"
                      "</td>"
                      "<td class=\"LEFT_MIDDLE\">",
            The_ClassForm[Gbl.Prefs.Theme],Txt_Country);
   Cty_WriteSelectorOfCountry ();
   fprintf (Gbl.F.Out,"</td>"
	              "</tr>");

   if (Gbl.CurrentCty.Cty.CtyCod > 0)
     {
      /***** Write a 2nd selector
             with the institutions of selected country *****/
      fprintf (Gbl.F.Out,"<tr>"
                         "<td class=\"RIGHT_MIDDLE\">"
                         "<label for=\"ins\" class=\"%s\">%s:</label>"
                         "</td>"
                         "<td class=\"LEFT_MIDDLE\">",
               The_ClassForm[Gbl.Prefs.Theme],Txt_Institution);
      Ins_WriteSelectorOfInstitution ();
      fprintf (Gbl.F.Out,"</td>"
	                 "</tr>");

      if (Gbl.CurrentIns.Ins.InsCod > 0)
        {
         /***** Write a 3rd selector
                with all the centres of selected institution *****/
         fprintf (Gbl.F.Out,"<tr>"
                            "<td class=\"RIGHT_MIDDLE\">"
                            "<label for=\"ctr\" class=\"%s\">%s:</label>"
                            "</td>"
                            "<td class=\"LEFT_MIDDLE\">",
                  The_ClassForm[Gbl.Prefs.Theme],Txt_Centre);
         Ctr_WriteSelectorOfCentre ();
         fprintf (Gbl.F.Out,"</td>"
                            "</tr>");

         if (Gbl.CurrentCtr.Ctr.CtrCod > 0)
           {
            /***** Write a 4th selector
                   with all the degrees of selected centre *****/
            fprintf (Gbl.F.Out,"<tr>"
                               "<td class=\"RIGHT_MIDDLE\">"
                               "<label for=\"deg\" class=\"%s\">%s:</label>"
                               "</td>"
                               "<td class=\"LEFT_MIDDLE\">",
                     The_ClassForm[Gbl.Prefs.Theme],Txt_Degree);
            Deg_WriteSelectorOfDegree ();
            fprintf (Gbl.F.Out,"</td>"
        	               "</tr>");

	    if (Gbl.CurrentDeg.Deg.DegCod > 0)
	      {
	       /***** Write a 5th selector
		      with all the courses of selected degree *****/
	       fprintf (Gbl.F.Out,"<tr>"
				  "<td class=\"RIGHT_MIDDLE\">"
                                  "<label for=\"crs\" class=\"%s\">%s:</label>"
				  "</td>"
				  "<td class=\"LEFT_MIDDLE\">",
			The_ClassForm[Gbl.Prefs.Theme],Txt_Course);
	       Crs_WriteSelectorOfCourse ();
	       fprintf (Gbl.F.Out,"</td>"
				  "</tr>");
	      }
           }
        }
     }

   /***** End table *****/
   Tbl_EndTable ();
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

   /***** Form to go to the system *****/
   fprintf (Gbl.F.Out,"<div class=\"BC %s\">&nbsp;",ClassTxt);

   Frm_StartFormGoTo (ActMnu);
   Par_PutHiddenParamUnsigned ("NxtTab",(unsigned) TabSys);
   Frm_LinkFormSubmit (Txt_System,ClassTxt,NULL);
   fprintf (Gbl.F.Out,"%s</a>",Txt_System);
   Frm_EndForm ();

   fprintf (Gbl.F.Out,"</div>");

   if (Gbl.CurrentCty.Cty.CtyCod > 0)		// Country selected...
     {
      fprintf (Gbl.F.Out,"<div class=\"BC %s\">",ClassTxt);

      /***** Separator *****/
      fprintf (Gbl.F.Out,"&nbsp;&gt;&nbsp;");

      /***** Form to go to see institutions of this country *****/
      Frm_StartFormGoTo (ActSeeIns);
      Cty_PutParamCtyCod (Gbl.CurrentCty.Cty.CtyCod);
      Frm_LinkFormSubmit (Gbl.CurrentCty.Cty.Name[Gbl.Prefs.Language],ClassTxt,NULL);
      fprintf (Gbl.F.Out,"%s</a>",Gbl.CurrentCty.Cty.Name[Gbl.Prefs.Language]);
      Frm_EndForm ();

      fprintf (Gbl.F.Out,"</div>");
     }
   else
     {
      fprintf (Gbl.F.Out,"<div class=\"BC BC_SEMIOFF %s\">",ClassTxt);

      /***** Separator *****/
      fprintf (Gbl.F.Out,"&nbsp;&gt;&nbsp;");

      /***** Form to go to select countries *****/
      Frm_StartFormGoTo (ActSeeCty);
      Frm_LinkFormSubmit (Txt_Country,ClassTxt,NULL);
      fprintf (Gbl.F.Out,"%s</a>",Txt_Country);
      Frm_EndForm ();

      fprintf (Gbl.F.Out,"</div>");
     }

   if (Gbl.CurrentIns.Ins.InsCod > 0)		// Institution selected...
     {
      fprintf (Gbl.F.Out,"<div class=\"BC %s\">",ClassTxt);

      /***** Separator *****/
      fprintf (Gbl.F.Out,"&nbsp;&gt;&nbsp;");

      /***** Form to see centres of this institution *****/
      Frm_StartFormGoTo (ActSeeCtr);
      Ins_PutParamInsCod (Gbl.CurrentIns.Ins.InsCod);
      Frm_LinkFormSubmit (Gbl.CurrentIns.Ins.FullName,ClassTxt,NULL);
      fprintf (Gbl.F.Out,"%s</a>",Gbl.CurrentIns.Ins.ShrtName);
      Frm_EndForm ();

      fprintf (Gbl.F.Out,"</div>");
     }
   else if (Gbl.CurrentCty.Cty.CtyCod > 0)
     {
      fprintf (Gbl.F.Out,"<div class=\"BC BC_SEMIOFF %s\">",ClassTxt);

      /***** Separator *****/
      fprintf (Gbl.F.Out,"&nbsp;&gt;&nbsp;");

      /***** Form to go to select institutions *****/
      Frm_StartFormGoTo (ActSeeIns);
      Frm_LinkFormSubmit (Txt_Institution,ClassTxt,NULL);
      fprintf (Gbl.F.Out,"%s</a>",Txt_Institution);
      Frm_EndForm ();

      fprintf (Gbl.F.Out,"</div>");
     }
   else
     {
      fprintf (Gbl.F.Out,"<div class=\"BC BC_OFF %s\">",ClassTxt);

      /***** Separator *****/
      fprintf (Gbl.F.Out,"&nbsp;&gt;&nbsp;");

      /***** Hidden institution *****/
      fprintf (Gbl.F.Out,"%s",Txt_Institution);

      fprintf (Gbl.F.Out,"</div>");
     }

   if (Gbl.CurrentCtr.Ctr.CtrCod > 0)	// Centre selected...
     {
      fprintf (Gbl.F.Out,"<div class=\"BC %s\">",ClassTxt);

      /***** Separator *****/
      fprintf (Gbl.F.Out,"&nbsp;&gt;&nbsp;");

      /***** Form to see degrees of this centre *****/
      Frm_StartFormGoTo (ActSeeDeg);
      Ctr_PutParamCtrCod (Gbl.CurrentCtr.Ctr.CtrCod);
      Frm_LinkFormSubmit (Gbl.CurrentCtr.Ctr.FullName,ClassTxt,NULL);
      fprintf (Gbl.F.Out,"%s</a>",Gbl.CurrentCtr.Ctr.ShrtName);
      Frm_EndForm ();

      fprintf (Gbl.F.Out,"</div>");
     }
   else if (Gbl.CurrentIns.Ins.InsCod > 0)
     {
      fprintf (Gbl.F.Out,"<div class=\"BC BC_SEMIOFF %s\">",ClassTxt);

      /***** Separator *****/
      fprintf (Gbl.F.Out,"&nbsp;&gt;&nbsp;");

      /***** Form to go to select centres *****/
      Frm_StartFormGoTo (ActSeeCtr);
      Frm_LinkFormSubmit (Txt_Centre,ClassTxt,NULL);
      fprintf (Gbl.F.Out,"%s</a>",Txt_Centre);
      Frm_EndForm ();

      fprintf (Gbl.F.Out,"</div>");
     }
   else
     {
      fprintf (Gbl.F.Out,"<div class=\"BC BC_OFF %s\">",ClassTxt);

      /***** Separator *****/
      fprintf (Gbl.F.Out,"&nbsp;&gt;&nbsp;");

      /***** Hidden centre *****/
      fprintf (Gbl.F.Out,"%s",Txt_Centre);

      fprintf (Gbl.F.Out,"</div>");
     }

   if (Gbl.CurrentDeg.Deg.DegCod > 0)	// Degree selected...
     {
      fprintf (Gbl.F.Out,"<div class=\"BC %s\">",ClassTxt);

      /***** Separator *****/
      fprintf (Gbl.F.Out,"&nbsp;&gt;&nbsp;");

      /***** Form to go to see courses of this degree *****/
      Frm_StartFormGoTo (ActSeeCrs);
      Deg_PutParamDegCod (Gbl.CurrentDeg.Deg.DegCod);
      Frm_LinkFormSubmit (Gbl.CurrentDeg.Deg.FullName,ClassTxt,NULL);
      fprintf (Gbl.F.Out,"%s</a>",Gbl.CurrentDeg.Deg.ShrtName);
      Frm_EndForm ();

      fprintf (Gbl.F.Out,"</div>");
     }
   else if (Gbl.CurrentCtr.Ctr.CtrCod > 0)
     {
      fprintf (Gbl.F.Out,"<div class=\"BC BC_SEMIOFF %s\">",ClassTxt);

      /***** Separator *****/
      fprintf (Gbl.F.Out,"&nbsp;&gt;&nbsp;");

      /***** Form to go to select degrees *****/
      Frm_StartFormGoTo (ActSeeDeg);
      Frm_LinkFormSubmit (Txt_Degree,ClassTxt,NULL);
      fprintf (Gbl.F.Out,"%s</a>",Txt_Degree);
      Frm_EndForm ();

      fprintf (Gbl.F.Out,"</div>");
     }
   else
     {
      fprintf (Gbl.F.Out,"<div class=\"BC BC_OFF %s\">",ClassTxt);

      /***** Separator *****/
      fprintf (Gbl.F.Out,"&nbsp;&gt;&nbsp;");

      /***** Hidden degree *****/
      fprintf (Gbl.F.Out,"%s",Txt_Degree);

      fprintf (Gbl.F.Out,"</div>");
     }

   fprintf (Gbl.F.Out,"<div class=\"BC%s %s\">",
	     (Gbl.CurrentCrs.Crs.CrsCod > 0) ? "" :
            ((Gbl.CurrentDeg.Deg.DegCod > 0) ? " BC_SEMIOFF" :
		                               " BC_OFF"),
            ClassTxt);

   /***** Separator *****/
   fprintf (Gbl.F.Out,"&nbsp;&gt;&nbsp;");

   fprintf (Gbl.F.Out,"</div>");
  }

/*****************************************************************************/
/*************** Write course full name in the top of the page ***************/
/*****************************************************************************/

void Hie_WriteBigNameCtyInsCtrDegCrs (void)
  {
   extern const char *The_ClassCourse[The_NUM_THEMES];
   extern const char *Txt_TAGLINE;

   fprintf (Gbl.F.Out,"<h1 id=\"main_title\" class=\"%s\">",
	    The_ClassCourse[Gbl.Prefs.Theme]);

   /***** Logo *****/
   if (Gbl.CurrentCrs.Crs.CrsCod > 0 ||
       Gbl.CurrentDeg.Deg.DegCod > 0)
      Log_DrawLogo (Sco_SCOPE_DEG,Gbl.CurrentDeg.Deg.DegCod,
		    Gbl.CurrentDeg.Deg.ShrtName,40,"TOP_LOGO",false);
   else if (Gbl.CurrentCtr.Ctr.CtrCod > 0)
      Log_DrawLogo (Sco_SCOPE_CTR,Gbl.CurrentCtr.Ctr.CtrCod,
		    Gbl.CurrentCtr.Ctr.ShrtName,40,"TOP_LOGO",false);
   else if (Gbl.CurrentIns.Ins.InsCod > 0)
      Log_DrawLogo (Sco_SCOPE_INS,Gbl.CurrentIns.Ins.InsCod,
		    Gbl.CurrentIns.Ins.ShrtName,40,"TOP_LOGO",false);
   else if (Gbl.CurrentCty.Cty.CtyCod > 0)
      Cty_DrawCountryMap (&Gbl.CurrentCty.Cty,"COUNTRY_MAP_TITLE");
   else
      fprintf (Gbl.F.Out,"<img src=\"%s/swad64x64.png\""
                         " alt=\"%s\" title=\"%s\""
                         " class=\"ICO40x40 TOP_LOGO\" />",
               Gbl.Prefs.URLIcons,
               Cfg_PLATFORM_SHORT_NAME,Cfg_PLATFORM_FULL_NAME);

   /***** Text *****/
   fprintf (Gbl.F.Out,"<div id=\"big_name_container\">");
   if (Gbl.CurrentCty.Cty.CtyCod > 0)
      fprintf (Gbl.F.Out,"<div id=\"big_full_name\">"
			 "%s"	// Full name
			 "</div>"
			 "<div class=\"NOT_SHOWN\">"
			 " / "	// To separate
			 "</div>"
			 "<div id=\"big_short_name\">"
			 "%s"	// Short name
			 "</div>",
		(Gbl.CurrentCrs.Crs.CrsCod > 0) ? Gbl.CurrentCrs.Crs.FullName :
	       ((Gbl.CurrentDeg.Deg.DegCod > 0) ? Gbl.CurrentDeg.Deg.FullName :
	       ((Gbl.CurrentCtr.Ctr.CtrCod > 0) ? Gbl.CurrentCtr.Ctr.FullName :
	       ((Gbl.CurrentIns.Ins.InsCod > 0) ? Gbl.CurrentIns.Ins.FullName :
	                                          Gbl.CurrentCty.Cty.Name[Gbl.Prefs.Language]))),
		(Gbl.CurrentCrs.Crs.CrsCod > 0) ? Gbl.CurrentCrs.Crs.ShrtName :
	       ((Gbl.CurrentDeg.Deg.DegCod > 0) ? Gbl.CurrentDeg.Deg.ShrtName :
	       ((Gbl.CurrentCtr.Ctr.CtrCod > 0) ? Gbl.CurrentCtr.Ctr.ShrtName :
	       ((Gbl.CurrentIns.Ins.InsCod > 0) ? Gbl.CurrentIns.Ins.ShrtName :
	                                          Gbl.CurrentCty.Cty.Name[Gbl.Prefs.Language]))));
   else	// No country specified ==> home page
      fprintf (Gbl.F.Out,"<div id=\"big_full_name\">"
			 "%s: %s"	// Full name
			 "</div>"
			 "<div class=\"NOT_SHOWN\">"
			 " / "		// To separate
			 "</div>"
			 "<div id=\"big_short_name\">"
			 "%s"		// Short name
			 "</div>",
	       Cfg_PLATFORM_SHORT_NAME,Txt_TAGLINE,
	       Cfg_PLATFORM_SHORT_NAME);
   fprintf (Gbl.F.Out,"</div>"
	              "</h1>");
  }

/*****************************************************************************/
/**** Initialize current country, institution, centre, degree and course *****/
/*****************************************************************************/

void Hie_InitHierarchy (void)
  {
   /***** If course code is available, get course data *****/
   if (Gbl.CurrentCrs.Crs.CrsCod > 0)
     {
      if (Crs_GetDataOfCourseByCod (&Gbl.CurrentCrs.Crs))		// Course found
         Gbl.CurrentDeg.Deg.DegCod = Gbl.CurrentCrs.Crs.DegCod;
      else
        {
         Gbl.CurrentIns.Ins.InsCod =
         Gbl.CurrentCtr.Ctr.CtrCod =
         Gbl.CurrentDeg.Deg.DegCod =
         Gbl.CurrentCrs.Crs.CrsCod = -1L;
        }
     }

   /***** If degree code is available, get degree data *****/
   if (Gbl.CurrentDeg.Deg.DegCod > 0)
     {
      if (Deg_GetDataOfDegreeByCod (&Gbl.CurrentDeg.Deg))	// Degree found
	{
	 Gbl.CurrentCtr.Ctr.CtrCod          = Gbl.CurrentDeg.Deg.CtrCod;
         Gbl.CurrentDegTyp.DegTyp.DegTypCod = Gbl.CurrentDeg.Deg.DegTypCod;
         Gbl.CurrentIns.Ins.InsCod = Deg_GetInsCodOfDegreeByCod (Gbl.CurrentDeg.Deg.DegCod);

         /***** Degree type is available, so get degree type data *****/
         if (!DT_GetDataOfDegreeTypeByCod (&Gbl.CurrentDegTyp.DegTyp))	// Degree type not found
           {
	    Gbl.CurrentIns.Ins.InsCod =
	    Gbl.CurrentCtr.Ctr.CtrCod =
	    Gbl.CurrentDeg.Deg.DegTypCod =
	    Gbl.CurrentDeg.Deg.DegCod =
	    Gbl.CurrentCrs.Crs.CrsCod = -1L;
           }
	}
      else
        {
         Gbl.CurrentIns.Ins.InsCod =
         Gbl.CurrentCtr.Ctr.CtrCod =
         Gbl.CurrentDeg.Deg.DegCod =
         Gbl.CurrentCrs.Crs.CrsCod = -1L;
        }
     }

   /***** If centre code is available, get centre data *****/
   if (Gbl.CurrentCtr.Ctr.CtrCod > 0)
     {
      if (Ctr_GetDataOfCentreByCod (&Gbl.CurrentCtr.Ctr))	// Centre found
         Gbl.CurrentIns.Ins.InsCod = Gbl.CurrentCtr.Ctr.InsCod;
      else
         Gbl.CurrentCtr.Ctr.CtrCod = -1L;
     }

   /***** If institution code is available, get institution data *****/
   if (Gbl.CurrentIns.Ins.InsCod > 0)
     {
      if (Ins_GetDataOfInstitutionByCod (&Gbl.CurrentIns.Ins,Ins_GET_BASIC_DATA))	// Institution found
	 Gbl.CurrentCty.Cty.CtyCod = Gbl.CurrentIns.Ins.CtyCod;
      else
        {
         Gbl.CurrentCty.Cty.CtyCod =
         Gbl.CurrentIns.Ins.InsCod =
         Gbl.CurrentCtr.Ctr.CtrCod =
         Gbl.CurrentDeg.Deg.DegCod =
         Gbl.CurrentCrs.Crs.CrsCod = -1L;
        }
     }

   /***** If country code is available, get country data *****/
   if (Gbl.CurrentCty.Cty.CtyCod > 0)
     {
      if (!Cty_GetDataOfCountryByCod (&Gbl.CurrentCty.Cty,Cty_GET_BASIC_DATA))	// Country not found
        {
         Gbl.CurrentCty.Cty.CtyCod =
         Gbl.CurrentIns.Ins.InsCod =
         Gbl.CurrentCtr.Ctr.CtrCod =
         Gbl.CurrentDeg.Deg.DegCod =
         Gbl.CurrentCrs.Crs.CrsCod = -1L;
        }
     }

   /***** Initialize default fields for edition to current values *****/
   Gbl.Inss.EditingIns.CtyCod    = Gbl.CurrentCty.Cty.CtyCod;
   Gbl.Ctrs.EditingCtr.InsCod    =
   Gbl.Dpts.EditingDpt.InsCod    = Gbl.CurrentIns.Ins.InsCod;
   Gbl.Degs.EditingDeg.CtrCod    = Gbl.CurrentCtr.Ctr.CtrCod;
   Gbl.Degs.EditingDeg.DegTypCod = Gbl.CurrentDegTyp.DegTyp.DegTypCod;

   /***** Initialize paths *****/
   if (Gbl.CurrentCrs.Crs.CrsCod > 0)
     {
      /***** Paths of course directories *****/
      snprintf (Gbl.CurrentCrs.PathPriv,sizeof (Gbl.CurrentCrs.PathPriv),
	        "%s/%s/%ld",
	        Cfg_PATH_SWAD_PRIVATE,Cfg_FOLDER_CRS,Gbl.CurrentCrs.Crs.CrsCod);
      snprintf (Gbl.CurrentCrs.PathRelPubl,sizeof (Gbl.CurrentCrs.PathRelPubl),
	        "%s/%s/%ld",
	        Cfg_PATH_SWAD_PUBLIC ,Cfg_FOLDER_CRS,Gbl.CurrentCrs.Crs.CrsCod);
      snprintf (Gbl.CurrentCrs.PathURLPubl,sizeof (Gbl.CurrentCrs.PathURLPubl),
	        "%s/%s/%ld",
	        Cfg_URL_SWAD_PUBLIC,Cfg_FOLDER_CRS,Gbl.CurrentCrs.Crs.CrsCod);

      /***** If any of the course directories does not exist, create it *****/
      if (!Fil_CheckIfPathExists (Gbl.CurrentCrs.PathPriv))
	 Fil_CreateDirIfNotExists (Gbl.CurrentCrs.PathPriv);
      if (!Fil_CheckIfPathExists (Gbl.CurrentCrs.PathRelPubl))
	 Fil_CreateDirIfNotExists (Gbl.CurrentCrs.PathRelPubl);

      /***** Count number of groups in current course
             (used in some actions) *****/
      Gbl.CurrentCrs.Grps.NumGrps = Grp_CountNumGrpsInCurrentCrs ();
     }
  }
