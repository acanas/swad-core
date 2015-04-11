// swad_link.c: institutional links

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2015 Antonio Cañas Vargas

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

static void Lnk_PutFormToEditLinks (void);
static void Lnk_ListLinksForEdition (void);
static void Lnk_PutParamLnkCod (long LnkCod);
static void Lnk_RenameLink (Cns_ShortOrFullName_t ShortOrFullName);
static bool Lnk_CheckIfLinkNameExists (const char *FieldName,const char *Name,long LnkCod);
static void Lnk_PutFormToCreateLink (void);
static void Lnk_PutHeadLinks (void);
static void Lnk_CreateLink (struct Link *Lnk);

/*****************************************************************************/
/*************************** List all the links ******************************/
/*****************************************************************************/

void Lnk_SeeLinks (void)
  {
   extern const char *Txt_Links;
   unsigned NumLnk;

   /***** Get list of links *****/
   Lnk_GetListLinks ();

   /***** Put link (form) to edit links *****/
   if (Gbl.Usrs.Me.LoggedRole == Rol_SYS_ADM)
      Lnk_PutFormToEditLinks ();

   /***** Table head *****/
   if (Gbl.Links.Num)
     {
      Lay_StartRoundFrameTable10 (NULL,2,Txt_Links);

      /***** Write all the links *****/
      for (NumLnk = 0;
	   NumLnk < Gbl.Links.Num;
	   NumLnk++)
	 /* Write data of this link */
	 fprintf (Gbl.F.Out,"<tr>"
			    "<td class=\"INS_LNK\" style=\"text-align:left;\">"
			    "<a href=\"%s\" title=\"%s\" class=\"INS_LNK\" target=\"_blank\">"
			    "%s"
			    "</a>"
			    "</td>"
			    "</tr>",
		  Gbl.Links.Lst[NumLnk].WWW,
		  Gbl.Links.Lst[NumLnk].FullName,
		  Gbl.Links.Lst[NumLnk].ShortName);

      /***** Table end *****/
      Lay_EndRoundFrameTable10 (Lay_NO_BUTTON,NULL);
     }

   /***** Free list of links *****/
   Lnk_FreeListLinks ();
  }

/*****************************************************************************/
/********************** Put a link (form) to edit links **********************/
/*****************************************************************************/

static void Lnk_PutFormToEditLinks (void)
  {
   extern const char *Txt_Edit;

   fprintf (Gbl.F.Out,"<div class=\"CONTEXT_MENU\">");
   Act_PutContextualLink (ActEdiLnk,NULL,"edit",Txt_Edit);
   fprintf (Gbl.F.Out,"</div>");
  }

/*****************************************************************************/
/************************** Put forms to edit links **************************/
/*****************************************************************************/

void Lnk_EditLinks (void)
  {
   extern const char *Txt_There_are_no_links;

   /***** Get list of links *****/
   Lnk_GetListLinks ();

   if (Gbl.Links.Num)
      /***** Put link (form) to view links *****/
      Lay_PutFormToView (ActSeeLnk);
   else
      /***** Help message *****/
      Lay_ShowAlert (Lay_INFO,Txt_There_are_no_links);

   /***** Put a form to create a new link *****/
   Lnk_PutFormToCreateLink ();

   /***** Forms to edit current links *****/
   if (Gbl.Links.Num)
      Lnk_ListLinksForEdition ();

   /***** Free list of links *****/
   Lnk_FreeListLinks ();
  }

/*****************************************************************************/
/**************************** List all the links *****************************/
/*****************************************************************************/

void Lnk_GetListLinks (void)
  {
   char Query[512];
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
	    strcpy (Lnk->ShortName,row[1]);

	    /* Get the full name of the link (row[2]) */
	    strcpy (Lnk->FullName,row[2]);

	    /* Get the URL of the link (row[3]) */
	    strcpy (Lnk->WWW,row[3]);
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
   char Query[1024];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;

   /***** Clear data *****/
   Lnk->ShortName[0] = Lnk->FullName[0] = Lnk->WWW[0] = '\0';

   /***** Check if link code is correct *****/
   if (Lnk->LnkCod > 0)
     {
      /***** Get data of an institutional link from database *****/
      sprintf (Query,"SELECT ShortName,FullName,WWW FROM links WHERE LnkCod='%ld'",
               Lnk->LnkCod);
      NumRows = DB_QuerySELECT (Query,&mysql_res,"can not get data of an institutional link");

      if (NumRows) // Link found...
        {
         /* Get row */
         row = mysql_fetch_row (mysql_res);

         /* Get the short name of the link (row[0]) */
         strcpy (Lnk->ShortName,row[0]);

         /* Get the full name of the link (row[1]) */
         strcpy (Lnk->FullName,row[1]);

         /* Get the URL of the link (row[2]) */
         strcpy (Lnk->WWW,row[2]);
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
/*************************** List all the links ******************************/
/*****************************************************************************/

static void Lnk_ListLinksForEdition (void)
  {
   extern const char *Txt_Links;
   extern const char *Txt_Remove_link;
   unsigned NumLnk;
   struct Link *Lnk;

   Lay_StartRoundFrameTable10 (NULL,2,Txt_Links);

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
      fprintf (Gbl.F.Out,"<input type=\"image\" src=\"%s/delon16x16.gif\""
	                 " alt=\"%s\" title=\"%s\" class=\"ICON16x16\" />",
               Gbl.Prefs.IconsURL,
               Txt_Remove_link,
               Txt_Remove_link);
      Act_FormEnd ();
      fprintf (Gbl.F.Out,"</td>");

      /* Link code */
      fprintf (Gbl.F.Out,"<td class=\"DAT\" style=\"text-align:right;\">"
	                 "%ld&nbsp;"
	                 "</td>",
               Lnk->LnkCod);

      /* Link short name */
      fprintf (Gbl.F.Out,"<td style=\"text-align:center;"
	                 " vertical-align:middle;\">");
      Act_FormStart (ActRenLnkSho);
      Lnk_PutParamLnkCod (Lnk->LnkCod);
      fprintf (Gbl.F.Out,"<input type=\"text\" name=\"ShortName\" size=\"15\" maxlength=\"%u\" value=\"%s\""
                         " onchange=\"javascript:document.getElementById('%s').submit();\" />",
               Lnk_MAX_LENGTH_LINK_SHORT_NAME,Lnk->ShortName,
               Gbl.FormId);
      Act_FormEnd ();
      fprintf (Gbl.F.Out,"</td>");

      /* Link full name */
      fprintf (Gbl.F.Out,"<td style=\"text-align:center;"
	                 " vertical-align:middle;\">");
      Act_FormStart (ActRenLnkFul);
      Lnk_PutParamLnkCod (Lnk->LnkCod);
      fprintf (Gbl.F.Out,"<input type=\"text\" name=\"FullName\" size=\"40\" maxlength=\"%u\" value=\"%s\""
                         " onchange=\"javascript:document.getElementById('%s').submit();\" />",
               Lnk_MAX_LENGTH_LINK_FULL_NAME,Lnk->FullName,
               Gbl.FormId);
      Act_FormEnd ();
      fprintf (Gbl.F.Out,"</td>");

      /* Link WWW */
      fprintf (Gbl.F.Out,"<td style=\"text-align:center;"
	                 " vertical-align:middle;\">");
      Act_FormStart (ActChgLnkWWW);
      Lnk_PutParamLnkCod (Lnk->LnkCod);
      fprintf (Gbl.F.Out,"<input type=\"text\" name=\"WWW\" size=\"40\" maxlength=\"%u\" value=\"%s\""
                         " onchange=\"javascript:document.getElementById('%s').submit();\" />",
               Cns_MAX_LENGTH_WWW,Lnk->WWW,
               Gbl.FormId);
      Act_FormEnd ();
      fprintf (Gbl.F.Out,"</td>"
                         "</tr>");
     }

   Lay_EndRoundFrameTable10 (Lay_NO_BUTTON,NULL);
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
   char LongStr[1+10+1];

   /***** Get parameter with code of link *****/
   Par_GetParToText ("LnkCod",LongStr,1+10);
   return Str_ConvertStrCodToLongCod (LongStr);
  }

/*****************************************************************************/
/******************************* Remove a link *******************************/
/*****************************************************************************/

void Lnk_RemoveLink (void)
  {
   extern const char *Txt_Link_X_removed;
   char Query[512];
   struct Link Lnk;

   /***** Get link code *****/
   if ((Lnk.LnkCod = Lnk_GetParamLnkCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of institutional link is missing.");

   /***** Get data of the link from database *****/
   Lnk_GetDataOfLinkByCod (&Lnk);

   /***** Remove link *****/
   sprintf (Query,"DELETE FROM links WHERE LnkCod='%ld'",Lnk.LnkCod);
   DB_QueryDELETE (Query,"can not remove an institutional link");

   /***** Write message to show the change made *****/
   sprintf (Gbl.Message,Txt_Link_X_removed,
            Lnk.ShortName);
   Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);

   /***** Show the form again *****/
   Lnk_EditLinks ();
  }

/*****************************************************************************/
/********************* Change the short name of a link ***********************/
/*****************************************************************************/

void Lnk_RenameLinkShort (void)
  {
   Lnk_RenameLink (Cns_SHORT_NAME);
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

static void Lnk_RenameLink (Cns_ShortOrFullName_t ShortOrFullName)
  {
   extern const char *Txt_You_can_not_leave_the_name_of_the_link_X_empty;
   extern const char *Txt_The_link_X_already_exists;
   extern const char *Txt_The_link_X_has_been_renamed_as_Y;
   extern const char *Txt_The_name_of_the_link_X_has_not_changed;
   char Query[512];
   struct Link *Lnk;
   const char *ParamName = NULL;	// Initialized to avoid warning
   const char *FieldName = NULL;	// Initialized to avoid warning
   unsigned MaxLength = 0;		// Initialized to avoid warning
   char *CurrentLnkName = NULL;		// Initialized to avoid warning
   char NewLnkName[Lnk_MAX_LENGTH_LINK_FULL_NAME+1];

   Lnk = &Gbl.Links.EditingLnk;
   switch (ShortOrFullName)
     {
      case Cns_SHORT_NAME:
         ParamName = "ShortName";
         FieldName = "ShortName";
         MaxLength = Lnk_MAX_LENGTH_LINK_SHORT_NAME;
         CurrentLnkName = Lnk->ShortName;
         break;
      case Cns_FULL_NAME:
         ParamName = "FullName";
         FieldName = "FullName";
         MaxLength = Lnk_MAX_LENGTH_LINK_FULL_NAME;
         CurrentLnkName = Lnk->FullName;
         break;
     }

   /***** Get parameters from form *****/
   /* Get the code of the link */
   if ((Lnk->LnkCod = Lnk_GetParamLnkCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of institutional link is missing.");

   /* Get the new name for the link */
   Par_GetParToText (ParamName,NewLnkName,MaxLength);

   /***** Get from the database the old names of the link *****/
   Lnk_GetDataOfLinkByCod (Lnk);

   /***** Check if new name is empty *****/
   if (!NewLnkName[0])
     {
      sprintf (Gbl.Message,Txt_You_can_not_leave_the_name_of_the_link_X_empty,
               CurrentLnkName);
      Lay_ShowAlert (Lay_WARNING,Gbl.Message);
     }
   else
     {
      /***** Check if old and new names are the same (this happens when user press enter with no changes in the form) *****/
      if (strcmp (CurrentLnkName,NewLnkName))	// Different names
        {
         /***** If link was in database... *****/
         if (Lnk_CheckIfLinkNameExists (ParamName,NewLnkName,Lnk->LnkCod))
           {
            sprintf (Gbl.Message,Txt_The_link_X_already_exists,
                     NewLnkName);
            Lay_ShowAlert (Lay_WARNING,Gbl.Message);
           }
         else
           {
            /* Update the table changing old name by new name */
            sprintf (Query,"UPDATE links SET %s='%s' WHERE LnkCod='%ld'",
                     FieldName,NewLnkName,Lnk->LnkCod);
            DB_QueryUPDATE (Query,"can not update the name of an institutional link");

            /***** Write message to show the change made *****/
            sprintf (Gbl.Message,Txt_The_link_X_has_been_renamed_as_Y,
                     CurrentLnkName,NewLnkName);
            Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);
           }
        }
      else	// The same name
        {
         sprintf (Gbl.Message,Txt_The_name_of_the_link_X_has_not_changed,
                  CurrentLnkName);
         Lay_ShowAlert (Lay_INFO,Gbl.Message);
        }
     }

   /***** Show the form again *****/
   strcpy (CurrentLnkName,NewLnkName);
   Lnk_EditLinks ();
  }

/*****************************************************************************/
/********************** Check if the name of link exists *********************/
/*****************************************************************************/

static bool Lnk_CheckIfLinkNameExists (const char *FieldName,const char *Name,long LnkCod)
  {
   char Query[512];

   /***** Get number of links with a name from database *****/
   sprintf (Query,"SELECT COUNT(*) FROM links WHERE %s='%s' AND LnkCod<>'%ld'",
            FieldName,Name,LnkCod);
   return (DB_QueryCOUNT (Query,"can not check if the name of an institutional link already existed") != 0);
  }

/*****************************************************************************/
/**************** Change the WWW of an institutional link ********************/
/*****************************************************************************/

void Lnk_ChangeLinkWWW (void)
  {
   extern const char *Txt_The_new_web_address_is_X;
   extern const char *Txt_You_can_not_leave_the_web_address_empty;
   struct Link *Lnk;
   char Query[256+Cns_MAX_LENGTH_WWW];
   char NewWWW[Cns_MAX_LENGTH_WWW+1];

   Lnk = &Gbl.Links.EditingLnk;

   /***** Get parameters from form *****/
   /* Get the code of the link */
   if ((Lnk->LnkCod = Lnk_GetParamLnkCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of institutional link is missing.");

   /* Get the new WWW for the link */
   Par_GetParToText ("WWW",NewWWW,Cns_MAX_LENGTH_WWW);

   /***** Check if new WWW is empty *****/
   if (NewWWW[0])
     {
      /* Update the table changing old WWW by new WWW */
      sprintf (Query,"UPDATE links SET WWW='%s' WHERE LnkCod='%ld'",
               NewWWW,Lnk->LnkCod);
      DB_QueryUPDATE (Query,"can not update the web of an institutional link");

      /***** Write message to show the change made *****/
      sprintf (Gbl.Message,Txt_The_new_web_address_is_X,
               NewWWW);
      Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);
     }
   else
     Lay_ShowAlert (Lay_WARNING,Txt_You_can_not_leave_the_web_address_empty);

   /***** Show the form again *****/
   strcpy (Lnk->WWW,NewWWW);
   Lnk_EditLinks ();
  }

/*****************************************************************************/
/********************* Put a form to create a new link ***********************/
/*****************************************************************************/

static void Lnk_PutFormToCreateLink (void)
  {
   extern const char *Txt_New_link;
   extern const char *Txt_Create_link;
   struct Link *Lnk;

   Lnk = &Gbl.Links.EditingLnk;

   /***** Start form *****/
   Act_FormStart (ActNewLnk);

   /***** Start of frame *****/
   Lay_StartRoundFrameTable10 (NULL,2,Txt_New_link);

   /***** Write heading *****/
   Lnk_PutHeadLinks ();

   /***** Link code *****/
   fprintf (Gbl.F.Out,"<tr>"
                      "<td class=\"BM\"></td>"
                      "<td></td>");

   /***** Link short name *****/
   fprintf (Gbl.F.Out,"<td style=\"text-align:center; vertical-align:middle;\">"
                      "<input type=\"text\" name=\"ShortName\" size=\"15\" maxlength=\"%u\" value=\"%s\" />"
                      "</td>",
            Lnk_MAX_LENGTH_LINK_SHORT_NAME,Lnk->ShortName);

   /***** Link full name *****/
   fprintf (Gbl.F.Out,"<td style=\"text-align:center; vertical-align:middle;\">"
                      "<input type=\"text\" name=\"FullName\" size=\"40\" maxlength=\"%u\" value=\"%s\" />"
                      "</td>",
            Lnk_MAX_LENGTH_LINK_FULL_NAME,Lnk->FullName);

   /***** Link WWW *****/
   fprintf (Gbl.F.Out,"<td style=\"text-align:center; vertical-align:middle;\">"
                      "<input type=\"text\" name=\"WWW\" size=\"40\" maxlength=\"%u\" value=\"%s\" />"
                      "</td>"
                      "</tr>",
            Cns_MAX_LENGTH_WWW,Lnk->WWW);

   /***** Send button and end frame *****/
   Lay_EndRoundFrameTable10 (Lay_CREATE_BUTTON,Txt_Create_link);

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
                      "<td class=\"BM\"></td>"
                      "<td class=\"TIT_TBL\" style=\"text-align:right;\">"
                      "%s"
                      "</td>"
                      "<td class=\"TIT_TBL\" style=\"text-align:left;\">"
                      "%s"
                      "</td>"
                      "<td class=\"TIT_TBL\" style=\"text-align:left;\">"
                      "%s"
                      "</td>"
                      "<td class=\"TIT_TBL\" style=\"text-align:left;\">"
                      "%s"
                      "</td>"
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
   Par_GetParToText ("ShortName",Lnk->ShortName,Lnk_MAX_LENGTH_LINK_SHORT_NAME);

   /* Get link full name */
   Par_GetParToText ("FullName",Lnk->FullName,Lnk_MAX_LENGTH_LINK_FULL_NAME);

   /* Get link URL */
   Par_GetParToText ("WWW",Lnk->WWW,Cns_MAX_LENGTH_WWW);

   if (Lnk->ShortName[0] && Lnk->FullName[0])	// If there's a link name
     {
      /***** If name of link was in database... *****/
      if (Lnk_CheckIfLinkNameExists ("ShortName",Lnk->ShortName,-1L))
        {
         sprintf (Gbl.Message,Txt_The_link_X_already_exists,
                  Lnk->ShortName);
         Lay_ShowAlert (Lay_WARNING,Gbl.Message);
        }
      else if (Lnk_CheckIfLinkNameExists ("FullName",Lnk->FullName,-1L))
        {
         sprintf (Gbl.Message,Txt_The_link_X_already_exists,
                  Lnk->FullName);
         Lay_ShowAlert (Lay_WARNING,Gbl.Message);
        }
      else if (!Lnk->WWW[0])
         Lay_ShowAlert (Lay_WARNING,Txt_You_must_specify_the_URL_of_the_new_link);
      else	// Add new link to database
         Lnk_CreateLink (Lnk);
     }
   else	// If there is not a link name
      Lay_ShowAlert (Lay_WARNING,Txt_You_must_specify_the_short_name_and_the_full_name_of_the_new_link);

   /***** Show the form again *****/
   Lnk_EditLinks ();
  }

/*****************************************************************************/
/**************************** Create a new link ******************************/
/*****************************************************************************/

static void Lnk_CreateLink (struct Link *Lnk)
  {
   extern const char *Txt_Created_new_link_X;
   char Query[1024];

   /***** Create a new link *****/
   sprintf (Query,"INSERT INTO links (ShortName,FullName,WWW)"
                  " VALUES ('%s','%s','%s')",
            Lnk->ShortName,Lnk->FullName,Lnk->WWW);
   DB_QueryINSERT (Query,"can not create institutional link");

   /***** Write success message *****/
   sprintf (Gbl.Message,Txt_Created_new_link_X,
            Lnk->ShortName);
   Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);
  }

/*****************************************************************************/
/***************** Write menu with some institutional links ******************/
/*****************************************************************************/

void Lnk_WriteMenuWithInstitutionalLinks (void)
  {
   unsigned NumLnk;

   /***** Get list of links *****/
   Lnk_GetListLinks ();

   if (Gbl.Links.Num)
     {
      /***** Header *****/
      fprintf (Gbl.F.Out,"<tr>"
			 "<td class=\"INS_LNK\" style=\"width:120px;"
			 " text-align:center;\">"
			 "<div id=\"institutional_links\">"
			 "<table style=\"width:110px;\">");

      /***** Write all the links *****/
      for (NumLnk = 0;
	   NumLnk < Gbl.Links.Num;
	   NumLnk++)
	 /* Write data of this link */
	 fprintf (Gbl.F.Out,"<tr>"
			    "<td style=\"width:110px; text-align:left;\">"
			    "<a href=\"%s\" title=\"%s\" class=\"INS_LNK\" target=\"_blank\">"
			    "%s"
			    "</a>"
			    "</td>"
			    "</tr>",
		  Gbl.Links.Lst[NumLnk].WWW,
		  Gbl.Links.Lst[NumLnk].FullName,
		  Gbl.Links.Lst[NumLnk].ShortName);

      /***** End table *****/
      fprintf (Gbl.F.Out,"</table>"
			 "</div>"
			 "</td>"
			 "</tr>");
     }

   /***** Free list of links *****/
   Lnk_FreeListLinks ();
  }
