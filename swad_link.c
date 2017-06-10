// swad_link.c: institutional links

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2017 Antonio Cañas Vargas

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

#include "swad_box.h"
#include "swad_constant.h"
#include "swad_database.h"
#include "swad_global.h"
#include "swad_link.h"
#include "swad_parameter.h"
#include "swad_text.h"

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

static void Lnk_PutIconToEditLinks (void);
static void Lnk_WriteListOfLinks (void);

static void Lnk_PutIconToViewLinks (void);
static void Lnk_ListLinksForEdition (void);
static void Lnk_PutParamLnkCod (long LnkCod);

static void Lnk_RenameLink (Cns_ShrtOrFullName_t ShrtOrFullName);
static bool Lnk_CheckIfLinkNameExists (const char *FieldName,const char *Name,long LnkCod);
static void Lnk_UpdateLnkNameDB (long LnkCod,const char *FieldName,const char *NewLnkName);

static void Lnk_PutFormToCreateLink (void);
static void Lnk_PutHeadLinks (void);
static void Lnk_CreateLink (struct Link *Lnk);

/*****************************************************************************/
/************************** Put link to view links ***************************/
/*****************************************************************************/

void Lnk_PutLinkToViewLinks (void)
  {
   extern const char *Txt_Links;

   Lay_PutContextualLink (ActSeeLnk,NULL,NULL,
                          "link64x64.gif",
                          Txt_Links,Txt_Links,
                          NULL);
  }

/*****************************************************************************/
/*************************** List all the links ******************************/
/*****************************************************************************/

void Lnk_SeeLinks (void)
  {
   extern const char *Hlp_SYSTEM_Links;
   extern const char *Txt_Links;
   extern const char *Txt_No_links;
   extern const char *Txt_New_link;

   /***** Put contextual links *****/
   fprintf (Gbl.F.Out,"<div class=\"CONTEXT_MENU\">");

   /* Put link to view banners */
   Ban_PutLinkToViewBanners ();

   fprintf (Gbl.F.Out,"</div>");

   /***** Get list of links *****/
   Lnk_GetListLinks ();

   /***** Start frame *****/
   Box_StartBox (NULL,Txt_Links,
			Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM ? Lnk_PutIconToEditLinks :
								 NULL,
			Hlp_SYSTEM_Links,
                        false);	// Not closable

   /***** Write all links *****/
   if (Gbl.Links.Num)	// There are links
      Lnk_WriteListOfLinks ();
   else			// No links created
      Ale_ShowAlert (Ale_INFO,Txt_No_links);

   /***** Button to create link *****/
   if (Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM)
     {
      Act_FormStart (ActEdiLnk);
      Lay_PutConfirmButton (Txt_New_link);
      Act_FormEnd ();
     }

   /***** End frame *****/
   Box_EndBox ();

   /***** Free list of links *****/
   Lnk_FreeListLinks ();
  }

/*****************************************************************************/
/************************** Put icon to edit links ***************************/
/*****************************************************************************/

static void Lnk_PutIconToEditLinks (void)
  {
   Lay_PutContextualIconToEdit (ActEdiLnk,NULL);
  }

/*****************************************************************************/
/***************** Write menu with some institutional links ******************/
/*****************************************************************************/

void Lnk_WriteMenuWithInstitutionalLinks (void)
  {
   extern const char *Txt_Links;

   /***** Get list of links *****/
   Lnk_GetListLinks ();

   /***** Write all the links *****/
   if (Gbl.Links.Num)
     {
      fprintf (Gbl.F.Out,"<div id=\"institutional_links\">");

      Act_FormStart (ActSeeLnk);
      Act_LinkFormSubmit (Txt_Links,NULL,NULL);
      fprintf (Gbl.F.Out," %s"
			 "</a>",
	       Txt_Links);
      Act_FormEnd ();

      Lnk_WriteListOfLinks ();
      fprintf (Gbl.F.Out,"</div>");
     }

   /***** Free list of links *****/
   Lnk_FreeListLinks ();
  }

/*****************************************************************************/
/*************************** Write list of links *****************************/
/*****************************************************************************/

static void Lnk_WriteListOfLinks (void)
  {
   unsigned NumLnk;

   /***** List start *****/
   fprintf (Gbl.F.Out,"<ul class=\"LIST_LEFT\">");

   /***** Write all links *****/
   for (NumLnk = 0;
	NumLnk < Gbl.Links.Num;
	NumLnk++)
      /* Write data of this link */
      fprintf (Gbl.F.Out,"<li class=\"INS_LNK\">"
			 "<a href=\"%s\" title=\"%s\" class=\"INS_LNK\""
			 " target=\"_blank\">"
			 "%s"
			 "</a>"
			 "</li>",
	       Gbl.Links.Lst[NumLnk].WWW,
	       Gbl.Links.Lst[NumLnk].FullName,
	       Gbl.Links.Lst[NumLnk].ShrtName);

   /***** List end *****/
   fprintf (Gbl.F.Out,"</ul>");
  }

/*****************************************************************************/
/************************** Put forms to edit links **************************/
/*****************************************************************************/

void Lnk_EditLinks (void)
  {
   extern const char *Hlp_SYSTEM_Links_edit;
   extern const char *Txt_Links;

   /***** Put contextual links *****/
   fprintf (Gbl.F.Out,"<div class=\"CONTEXT_MENU\">");

   /* Put link to view banners */
   Ban_PutLinkToViewBanners ();

   fprintf (Gbl.F.Out,"</div>");

   /***** Get list of links *****/
   Lnk_GetListLinks ();

   /***** Start frame *****/
   Box_StartBox (NULL,Txt_Links,Lnk_PutIconToViewLinks,
                        Hlp_SYSTEM_Links_edit,
                        false);	// Not closable

   /***** Put a form to create a new link *****/
   Lnk_PutFormToCreateLink ();

   /***** Forms to edit current links *****/
   if (Gbl.Links.Num)
      Lnk_ListLinksForEdition ();

   /***** End frame *****/
   Box_EndBox ();

   /***** Free list of links *****/
   Lnk_FreeListLinks ();
  }

/*****************************************************************************/
/**************************** List all the links *****************************/
/*****************************************************************************/

void Lnk_GetListLinks (void)
  {
   char Query[256];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;
   unsigned NumLnk;
   struct Link *Lnk;

   if (Gbl.DB.DatabaseIsOpen)
     {
      /***** Get institutional links from database *****/
      sprintf (Query,"SELECT LnkCod,ShortName,FullName,WWW"
	             " FROM links ORDER BY ShortName");
      NumRows = DB_QuerySELECT (Query,&mysql_res,"can not get institutional links");

      if (NumRows) // Places found...
	{
	 Gbl.Links.Num = (unsigned) NumRows;

	 /***** Create list with places *****/
	 if ((Gbl.Links.Lst = (struct Link *) calloc (NumRows,sizeof (struct Link))) == NULL)
	     Lay_ShowErrorAndExit ("Not enough memory to store institutional links.");

	 /***** Get the links *****/
	 for (NumLnk = 0;
	      NumLnk < Gbl.Links.Num;
	      NumLnk++)
	   {
	    Lnk = &(Gbl.Links.Lst[NumLnk]);

	    /* Get next link */
	    row = mysql_fetch_row (mysql_res);

	    /* Get link code (row[0]) */
	    if ((Lnk->LnkCod = Str_ConvertStrCodToLongCod (row[0])) < 0)
	       Lay_ShowErrorAndExit ("Wrong code of institutional link.");

	    /* Get the short name of the link (row[1]) */
	    Str_Copy (Lnk->ShrtName,row[1],
	              Lnk_MAX_BYTES_LINK_SHRT_NAME);

	    /* Get the full name of the link (row[2]) */
	    Str_Copy (Lnk->FullName,row[2],
	              Lnk_MAX_BYTES_LINK_FULL_NAME);

	    /* Get the URL of the link (row[3]) */
	    Str_Copy (Lnk->WWW,row[3],
	              Cns_MAX_BYTES_WWW);
	   }
	}
      else
	 Gbl.Links.Num = 0;

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);
     }
  }

/*****************************************************************************/
/**************************** Get link full name *****************************/
/*****************************************************************************/

void Lnk_GetDataOfLinkByCod (struct Link *Lnk)
  {
   char Query[256];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;

   /***** Clear data *****/
   Lnk->ShrtName[0] = Lnk->FullName[0] = Lnk->WWW[0] = '\0';

   /***** Check if link code is correct *****/
   if (Lnk->LnkCod > 0)
     {
      /***** Get data of an institutional link from database *****/
      sprintf (Query,"SELECT ShortName,FullName,WWW FROM links"
	             " WHERE LnkCod=%ld",
               Lnk->LnkCod);
      NumRows = DB_QuerySELECT (Query,&mysql_res,"can not get data of an institutional link");

      if (NumRows) // Link found...
        {
         /* Get row */
         row = mysql_fetch_row (mysql_res);

         /* Get the short name of the link (row[0]) */
         Str_Copy (Lnk->ShrtName,row[0],
                   Lnk_MAX_BYTES_LINK_SHRT_NAME);

         /* Get the full name of the link (row[1]) */
         Str_Copy (Lnk->FullName,row[1],
                   Lnk_MAX_BYTES_LINK_FULL_NAME);

         /* Get the URL of the link (row[2]) */
         Str_Copy (Lnk->WWW,row[2],
                   Cns_MAX_BYTES_WWW);
        }

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);
     }
  }

/*****************************************************************************/
/**************************** Free list of links *****************************/
/*****************************************************************************/

void Lnk_FreeListLinks (void)
  {
   if (Gbl.Links.Lst)
     {
      /***** Free memory used by the list of links *****/
      free ((void *) Gbl.Links.Lst);
      Gbl.Links.Lst = NULL;
      Gbl.Links.Num = 0;
     }
  }

/*****************************************************************************/
/***************** Put contextual icons in edition of links ******************/
/*****************************************************************************/

static void Lnk_PutIconToViewLinks (void)
  {
   Lay_PutContextualIconToView (ActSeeLnk,NULL);
  }

/*****************************************************************************/
/*************************** List all the links ******************************/
/*****************************************************************************/

static void Lnk_ListLinksForEdition (void)
  {
   unsigned NumLnk;
   struct Link *Lnk;

   /***** Start table *****/
   Lay_StartTableWide (2);

   /***** Table head *****/
   Lnk_PutHeadLinks ();

   /***** Write all the links *****/
   for (NumLnk = 0;
	NumLnk < Gbl.Links.Num;
	NumLnk++)
     {
      Lnk = &Gbl.Links.Lst[NumLnk];

      /* Put icon to remove link */
      fprintf (Gbl.F.Out,"<tr>"
	                 "<td class=\"BM\">");
      Act_FormStart (ActRemLnk);
      Lnk_PutParamLnkCod (Lnk->LnkCod);
      Lay_PutIconRemove ();
      Act_FormEnd ();
      fprintf (Gbl.F.Out,"</td>");

      /* Link code */
      fprintf (Gbl.F.Out,"<td class=\"DAT RIGHT_MIDDLE\">"
	                 "%ld"
	                 "</td>",
               Lnk->LnkCod);

      /* Link short name */
      fprintf (Gbl.F.Out,"<td class=\"CENTER_MIDDLE\">");
      Act_FormStart (ActRenLnkSho);
      Lnk_PutParamLnkCod (Lnk->LnkCod);
      fprintf (Gbl.F.Out,"<input type=\"text\" name=\"ShortName\""
	                 " maxlength=\"%u\" value=\"%s\""
                         " class=\"INPUT_SHORT_NAME\""
                         " onchange=\"document.getElementById('%s').submit();\" />",
               Lnk_MAX_CHARS_LINK_SHRT_NAME,Lnk->ShrtName,
               Gbl.Form.Id);
      Act_FormEnd ();
      fprintf (Gbl.F.Out,"</td>");

      /* Link full name */
      fprintf (Gbl.F.Out,"<td class=\"CENTER_MIDDLE\">");
      Act_FormStart (ActRenLnkFul);
      Lnk_PutParamLnkCod (Lnk->LnkCod);
      fprintf (Gbl.F.Out,"<input type=\"text\" name=\"FullName\""
	                 " maxlength=\"%u\" value=\"%s\""
                         " class=\"INPUT_FULL_NAME\""
                         " onchange=\"document.getElementById('%s').submit();\" />",
               Lnk_MAX_CHARS_LINK_FULL_NAME,Lnk->FullName,
               Gbl.Form.Id);
      Act_FormEnd ();
      fprintf (Gbl.F.Out,"</td>");

      /* Link WWW */
      fprintf (Gbl.F.Out,"<td class=\"CENTER_MIDDLE\">");
      Act_FormStart (ActChgLnkWWW);
      Lnk_PutParamLnkCod (Lnk->LnkCod);
      fprintf (Gbl.F.Out,"<input type=\"url\" name=\"WWW\""
	                 " maxlength=\"%u\" value=\"%s\""
                         " class=\"INPUT_WWW\""
                         " onchange=\"document.getElementById('%s').submit();\" />",
               Cns_MAX_CHARS_WWW,Lnk->WWW,
               Gbl.Form.Id);
      Act_FormEnd ();
      fprintf (Gbl.F.Out,"</td>"
                         "</tr>");
     }

   /***** End table *****/
   Lay_EndTable ();
  }

/*****************************************************************************/
/******************** Write parameter with code of link **********************/
/*****************************************************************************/

static void Lnk_PutParamLnkCod (long LnkCod)
  {
   Par_PutHiddenParamLong ("LnkCod",LnkCod);
  }

/*****************************************************************************/
/********************* Get parameter with code of link ***********************/
/*****************************************************************************/

long Lnk_GetParamLnkCod (void)
  {
   /***** Get code of link *****/
   return Par_GetParToLong ("LnkCod");
  }

/*****************************************************************************/
/******************************* Remove a link *******************************/
/*****************************************************************************/

void Lnk_RemoveLink (void)
  {
   extern const char *Txt_Link_X_removed;
   char Query[128];
   struct Link Lnk;

   /***** Get link code *****/
   if ((Lnk.LnkCod = Lnk_GetParamLnkCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of institutional link is missing.");

   /***** Get data of the link from database *****/
   Lnk_GetDataOfLinkByCod (&Lnk);

   /***** Remove link *****/
   sprintf (Query,"DELETE FROM links WHERE LnkCod=%ld",
            Lnk.LnkCod);
   DB_QueryDELETE (Query,"can not remove an institutional link");

   /***** Write message to show the change made *****/
   sprintf (Gbl.Alert.Txt,Txt_Link_X_removed,
            Lnk.ShrtName);
   Ale_ShowAlert (Ale_SUCCESS,Gbl.Alert.Txt);

   /***** Show the form again *****/
   Lnk_EditLinks ();
  }

/*****************************************************************************/
/********************* Change the short name of a link ***********************/
/*****************************************************************************/

void Lnk_RenameLinkShort (void)
  {
   Lnk_RenameLink (Cns_SHRT_NAME);
  }

/*****************************************************************************/
/********************* Change the full name of a link ************************/
/*****************************************************************************/

void Lnk_RenameLinkFull (void)
  {
   Lnk_RenameLink (Cns_FULL_NAME);
  }

/*****************************************************************************/
/************************ Change the name of a link **************************/
/*****************************************************************************/

static void Lnk_RenameLink (Cns_ShrtOrFullName_t ShrtOrFullName)
  {
   extern const char *Txt_You_can_not_leave_the_name_of_the_link_X_empty;
   extern const char *Txt_The_link_X_already_exists;
   extern const char *Txt_The_link_X_has_been_renamed_as_Y;
   extern const char *Txt_The_name_of_the_link_X_has_not_changed;
   struct Link *Lnk;
   const char *ParamName = NULL;	// Initialized to avoid warning
   const char *FieldName = NULL;	// Initialized to avoid warning
   unsigned MaxBytes = 0;		// Initialized to avoid warning
   char *CurrentLnkName = NULL;		// Initialized to avoid warning
   char NewLnkName[Lnk_MAX_BYTES_LINK_FULL_NAME + 1];

   Lnk = &Gbl.Links.EditingLnk;
   switch (ShrtOrFullName)
     {
      case Cns_SHRT_NAME:
         ParamName = "ShortName";
         FieldName = "ShortName";
         MaxBytes = Lnk_MAX_BYTES_LINK_SHRT_NAME;
         CurrentLnkName = Lnk->ShrtName;
         break;
      case Cns_FULL_NAME:
         ParamName = "FullName";
         FieldName = "FullName";
         MaxBytes = Lnk_MAX_BYTES_LINK_FULL_NAME;
         CurrentLnkName = Lnk->FullName;
         break;
     }

   /***** Get parameters from form *****/
   /* Get the code of the link */
   if ((Lnk->LnkCod = Lnk_GetParamLnkCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of institutional link is missing.");

   /* Get the new name for the link */
   Par_GetParToText (ParamName,NewLnkName,MaxBytes);

   /***** Get from the database the old names of the link *****/
   Lnk_GetDataOfLinkByCod (Lnk);

   /***** Check if new name is empty *****/
   if (!NewLnkName[0])
     {
      sprintf (Gbl.Alert.Txt,Txt_You_can_not_leave_the_name_of_the_link_X_empty,
               CurrentLnkName);
      Ale_ShowAlert (Ale_WARNING,Gbl.Alert.Txt);
     }
   else
     {
      /***** Check if old and new names are the same (this happens when user press enter with no changes in the form) *****/
      if (strcmp (CurrentLnkName,NewLnkName))	// Different names
        {
         /***** If link was in database... *****/
         if (Lnk_CheckIfLinkNameExists (ParamName,NewLnkName,Lnk->LnkCod))
           {
            sprintf (Gbl.Alert.Txt,Txt_The_link_X_already_exists,
                     NewLnkName);
            Ale_ShowAlert (Ale_WARNING,Gbl.Alert.Txt);
           }
         else
           {
            /* Update the table changing old name by new name */
            Lnk_UpdateLnkNameDB (Lnk->LnkCod,FieldName,NewLnkName);

            /* Write message to show the change made */
            sprintf (Gbl.Alert.Txt,Txt_The_link_X_has_been_renamed_as_Y,
                     CurrentLnkName,NewLnkName);
            Ale_ShowAlert (Ale_SUCCESS,Gbl.Alert.Txt);
           }
        }
      else	// The same name
        {
         sprintf (Gbl.Alert.Txt,Txt_The_name_of_the_link_X_has_not_changed,
                  CurrentLnkName);
         Ale_ShowAlert (Ale_INFO,Gbl.Alert.Txt);
        }
     }

   /***** Show the form again *****/
   Str_Copy (CurrentLnkName,NewLnkName,
             MaxBytes);
   Lnk_EditLinks ();
  }

/*****************************************************************************/
/********************** Check if the name of link exists *********************/
/*****************************************************************************/

static bool Lnk_CheckIfLinkNameExists (const char *FieldName,const char *Name,long LnkCod)
  {
   char Query[256 + Lnk_MAX_BYTES_LINK_FULL_NAME];

   /***** Get number of links with a name from database *****/
   sprintf (Query,"SELECT COUNT(*) FROM links WHERE %s='%s' AND LnkCod<>%ld",
            FieldName,Name,LnkCod);
   return (DB_QueryCOUNT (Query,"can not check if the name of an institutional link already existed") != 0);
  }

/*****************************************************************************/
/************ Update link name in table of institutional links ***************/
/*****************************************************************************/

static void Lnk_UpdateLnkNameDB (long LnkCod,const char *FieldName,const char *NewLnkName)
  {
   char Query[128 + Lnk_MAX_BYTES_LINK_FULL_NAME];

   /***** Update institutional link changing old name by new name */
   sprintf (Query,"UPDATE links SET %s='%s' WHERE LnkCod=%ld",
	    FieldName,NewLnkName,LnkCod);
   DB_QueryUPDATE (Query,"can not update the name of an institutional link");
  }

/*****************************************************************************/
/**************** Change the WWW of an institutional link ********************/
/*****************************************************************************/

void Lnk_ChangeLinkWWW (void)
  {
   extern const char *Txt_The_new_web_address_is_X;
   extern const char *Txt_You_can_not_leave_the_web_address_empty;
   struct Link *Lnk;
   char Query[256 + Cns_MAX_BYTES_WWW];
   char NewWWW[Cns_MAX_BYTES_WWW + 1];

   Lnk = &Gbl.Links.EditingLnk;

   /***** Get parameters from form *****/
   /* Get the code of the link */
   if ((Lnk->LnkCod = Lnk_GetParamLnkCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of institutional link is missing.");

   /* Get the new WWW for the link */
   Par_GetParToText ("WWW",NewWWW,Cns_MAX_BYTES_WWW);

   /***** Check if new WWW is empty *****/
   if (NewWWW[0])
     {
      /* Update the table changing old WWW by new WWW */
      sprintf (Query,"UPDATE links SET WWW='%s' WHERE LnkCod=%ld",
               NewWWW,Lnk->LnkCod);
      DB_QueryUPDATE (Query,"can not update the web of an institutional link");

      /***** Write message to show the change made *****/
      sprintf (Gbl.Alert.Txt,Txt_The_new_web_address_is_X,
               NewWWW);
      Ale_ShowAlert (Ale_SUCCESS,Gbl.Alert.Txt);
     }
   else
     Ale_ShowAlert (Ale_WARNING,Txt_You_can_not_leave_the_web_address_empty);

   /***** Show the form again *****/
   Str_Copy (Lnk->WWW,NewWWW,
             Cns_MAX_BYTES_WWW);
   Lnk_EditLinks ();
  }

/*****************************************************************************/
/********************* Put a form to create a new link ***********************/
/*****************************************************************************/

static void Lnk_PutFormToCreateLink (void)
  {
   extern const char *Hlp_SYSTEM_Links_edit;
   extern const char *Txt_New_link;
   extern const char *Txt_Create_link;
   struct Link *Lnk;

   Lnk = &Gbl.Links.EditingLnk;

   /***** Start form *****/
   Act_FormStart (ActNewLnk);

   /***** Start of frame *****/
   Box_StartBoxTable (NULL,Txt_New_link,NULL,
                             Hlp_SYSTEM_Links_edit,
			     false,	// Not closable
                             2);

   /***** Write heading *****/
   Lnk_PutHeadLinks ();

   /***** Link code *****/
   fprintf (Gbl.F.Out,"<tr>"
                      "<td class=\"BM\"></td>"
                      "<td></td>");

   /***** Link short name *****/
   fprintf (Gbl.F.Out,"<td class=\"CENTER_MIDDLE\">"
                      "<input type=\"text\" name=\"ShortName\""
                      " maxlength=\"%u\" value=\"%s\""
                      " class=\"INPUT_SHORT_NAME\""
                      " required=\"required\" />"
                      "</td>",
            Lnk_MAX_CHARS_LINK_SHRT_NAME,Lnk->ShrtName);

   /***** Link full name *****/
   fprintf (Gbl.F.Out,"<td class=\"CENTER_MIDDLE\">"
                      "<input type=\"text\" name=\"FullName\""
                      " maxlength=\"%u\" value=\"%s\""
                      " class=\"INPUT_FULL_NAME\""
                      " required=\"required\" />"
                      "</td>",
            Lnk_MAX_CHARS_LINK_FULL_NAME,Lnk->FullName);

   /***** Link WWW *****/
   fprintf (Gbl.F.Out,"<td class=\"CENTER_MIDDLE\">"
                      "<input type=\"url\" name=\"WWW\""
                      " maxlength=\"%u\" value=\"%s\""
                      " class=\"INPUT_WWW\""
                      " required=\"required\" />"
                      "</td>"
                      "</tr>",
            Cns_MAX_CHARS_WWW,Lnk->WWW);

   /***** Send button and end frame *****/
   Box_EndBoxTableWithButton (Lay_CREATE_BUTTON,Txt_Create_link);

   /***** End of form *****/
   Act_FormEnd ();
  }

/*****************************************************************************/
/******************** Write header with fields of a link *********************/
/*****************************************************************************/

static void Lnk_PutHeadLinks (void)
  {
   extern const char *Txt_Code;
   extern const char *Txt_Short_name;
   extern const char *Txt_Full_name;
   extern const char *Txt_WWW;

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
                      "<th class=\"LEFT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "</tr>",
            Txt_Code,
            Txt_Short_name,
            Txt_Full_name,
            Txt_WWW);
  }

/*****************************************************************************/
/******************* Receive form to create a new link ***********************/
/*****************************************************************************/

void Lnk_RecFormNewLink (void)
  {
   extern const char *Txt_The_link_X_already_exists;
   extern const char *Txt_You_must_specify_the_URL_of_the_new_link;
   extern const char *Txt_You_must_specify_the_short_name_and_the_full_name_of_the_new_link;
   struct Link *Lnk;

   Lnk = &Gbl.Links.EditingLnk;

   /***** Get parameters from form *****/
   /* Get link short name */
   Par_GetParToText ("ShortName",Lnk->ShrtName,Lnk_MAX_BYTES_LINK_SHRT_NAME);

   /* Get link full name */
   Par_GetParToText ("FullName",Lnk->FullName,Lnk_MAX_BYTES_LINK_FULL_NAME);

   /* Get link URL */
   Par_GetParToText ("WWW",Lnk->WWW,Cns_MAX_BYTES_WWW);

   if (Lnk->ShrtName[0] && Lnk->FullName[0])	// If there's a link name
     {
      /***** If name of link was in database... *****/
      if (Lnk_CheckIfLinkNameExists ("ShortName",Lnk->ShrtName,-1L))
        {
         sprintf (Gbl.Alert.Txt,Txt_The_link_X_already_exists,
                  Lnk->ShrtName);
         Ale_ShowAlert (Ale_WARNING,Gbl.Alert.Txt);
        }
      else if (Lnk_CheckIfLinkNameExists ("FullName",Lnk->FullName,-1L))
        {
         sprintf (Gbl.Alert.Txt,Txt_The_link_X_already_exists,
                  Lnk->FullName);
         Ale_ShowAlert (Ale_WARNING,Gbl.Alert.Txt);
        }
      else if (!Lnk->WWW[0])
         Ale_ShowAlert (Ale_WARNING,Txt_You_must_specify_the_URL_of_the_new_link);
      else	// Add new link to database
         Lnk_CreateLink (Lnk);
     }
   else	// If there is not a link name
      Ale_ShowAlert (Ale_WARNING,Txt_You_must_specify_the_short_name_and_the_full_name_of_the_new_link);

   /***** Show the form again *****/
   Lnk_EditLinks ();
  }

/*****************************************************************************/
/**************************** Create a new link ******************************/
/*****************************************************************************/

static void Lnk_CreateLink (struct Link *Lnk)
  {
   extern const char *Txt_Created_new_link_X;
   char Query[256 +
              Lnk_MAX_BYTES_LINK_SHRT_NAME +
              Lnk_MAX_BYTES_LINK_FULL_NAME +
              Cns_MAX_BYTES_WWW];

   /***** Create a new link *****/
   sprintf (Query,"INSERT INTO links"
	          " (ShortName,FullName,WWW)"
                  " VALUES"
                  " ('%s','%s','%s')",
            Lnk->ShrtName,Lnk->FullName,Lnk->WWW);
   DB_QueryINSERT (Query,"can not create institutional link");

   /***** Write success message *****/
   sprintf (Gbl.Alert.Txt,Txt_Created_new_link_X,
            Lnk->ShrtName);
   Ale_ShowAlert (Ale_SUCCESS,Gbl.Alert.Txt);
  }
