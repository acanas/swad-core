// swad_link.c: institutional links

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

#include <stddef.h>		// For NULL
#include <stdlib.h>		// For calloc
#include <string.h>		// For string functions

#include "swad_banner.h"
#include "swad_box.h"
#include "swad_constant.h"
#include "swad_database.h"
#include "swad_error.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_HTML.h"
#include "swad_language.h"
#include "swad_link.h"
#include "swad_link_database.h"
#include "swad_parameter.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/***************************** Private variables *****************************/
/*****************************************************************************/

static struct Lnk_Link *Lnk_EditingLnk = NULL;	// Static variable to keep the link being edited

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Lnk_PutIconsListingLinks (__attribute__((unused)) void *Args);
static void Lnk_PutIconToEditLinks (void);
static void Lnk_WriteListOfLinks (const char *Class);

static void Lnk_EditLinksInternal (void);
static void Lnk_PutIconsEditingLinks (__attribute__((unused)) void *Args);

static void Lnk_GetDataOfLink (MYSQL_RES *mysql_res,struct Lnk_Link *Lnk);

static void Lnk_ListLinksForEdition (void);
static void Lnk_PutParamLnkCod (void *LnkCod);

static void Lnk_RenameLink (Cns_ShrtOrFullName_t ShrtOrFullName);

static void Lnk_PutFormToCreateLink (void);
static void Lnk_PutHeadLinks (void);

static void Lnk_EditingLinkConstructor (void);
static void Lnk_EditingLinkDestructor (void);

/*****************************************************************************/
/****************************** List all links *******************************/
/*****************************************************************************/

void Lnk_SeeLinks (void)
  {
   extern const char *Hlp_SYSTEM_Links;
   extern const char *Txt_Links;
   extern const char *Txt_No_links;
   extern const char *Txt_New_link;

   /***** Get list of links *****/
   Lnk_GetListLinks ();

   /***** Begin box *****/
   Box_BoxBegin (NULL,Txt_Links,
                 Lnk_PutIconsListingLinks,NULL,
		 Hlp_SYSTEM_Links,Box_NOT_CLOSABLE);

      /***** Write all links *****/
      if (Gbl.Links.Num)	// There are links
	 Lnk_WriteListOfLinks ("class=\"LIST_LEFT\"");
      else			// No links created
	 Ale_ShowAlert (Ale_INFO,Txt_No_links);

      /***** Button to create link *****/
      if (Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM)
	{
	 Frm_BeginForm (ActEdiLnk);
	    Btn_PutConfirmButton (Txt_New_link);
	 Frm_EndForm ();
	}

   /***** End box *****/
   Box_BoxEnd ();

   /***** Free list of links *****/
   Lnk_FreeListLinks ();
  }

/*****************************************************************************/
/***************** Put contextual icons in list of links *********************/
/*****************************************************************************/

static void Lnk_PutIconsListingLinks (__attribute__((unused)) void *Args)
  {
   /***** Put icon to edit links *****/
   if (Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM)
      Lnk_PutIconToEditLinks ();

   /***** Put icon to view banners *****/
   Ban_PutIconToViewBanners ();
  }

/*****************************************************************************/
/************************** Put icon to edit links ***************************/
/*****************************************************************************/

static void Lnk_PutIconToEditLinks (void)
  {
   Ico_PutContextualIconToEdit (ActEdiLnk,NULL,
                                NULL,NULL);
  }

/*****************************************************************************/
/***************** Write menu with some institutional links ******************/
/*****************************************************************************/

void Lnk_WriteMenuWithInstitutionalLinks (void)
  {
   extern const char *The_Colors[The_NUM_THEMES];
   extern const char *Txt_Links;

   /***** Get list of links *****/
   Lnk_GetListLinks ();

   /***** Write all links *****/
   if (Gbl.Links.Num)
     {
      HTM_DIV_Begin ("id=\"institutional_links\" class=\"INS_LNK_%s\"",
                     The_Colors[Gbl.Prefs.Theme]);

	 Frm_BeginForm (ActSeeLnk);
	    HTM_BUTTON_OnSubmit_Begin (Txt_Links,"BT_LINK",NULL);
	       HTM_TxtF ("%s",Txt_Links);
	    HTM_BUTTON_End ();
	 Frm_EndForm ();

	 Lnk_WriteListOfLinks (NULL);

      HTM_DIV_End ();
     }

   /***** Free list of links *****/
   Lnk_FreeListLinks ();
  }

/*****************************************************************************/
/*************************** Write list of links *****************************/
/*****************************************************************************/

static void Lnk_WriteListOfLinks (const char *Class)
  {
   extern const char *The_Colors[The_NUM_THEMES];
   unsigned NumLnk;

   /***** List start *****/
   HTM_UL_Begin (Class);

      /***** Write all links *****/
      for (NumLnk = 0;
	   NumLnk < Gbl.Links.Num;
	   NumLnk++)
	{
	 /* Write data of this link */
	 HTM_LI_Begin ("class=\"ICO_HIGHLIGHT INS_LNK\"");
	    HTM_A_Begin ("href=\"%s\" title=\"%s\""
		         " class=\"INS_LNK_%s\" target=\"_blank\"",
			 Gbl.Links.Lst[NumLnk].WWW,
			 Gbl.Links.Lst[NumLnk].FullName,
			 The_Colors[Gbl.Prefs.Theme]);
	       HTM_Txt (Gbl.Links.Lst[NumLnk].ShrtName);
	    HTM_A_End ();
	 HTM_LI_End ();
	}

   /***** List end *****/
   HTM_UL_End ();
  }

/*****************************************************************************/
/************************** Put forms to edit links **************************/
/*****************************************************************************/

void Lnk_EditLinks (void)
  {
   /***** Link constructor *****/
   Lnk_EditingLinkConstructor ();

   /***** Edit links *****/
   Lnk_EditLinksInternal ();

   /***** Link destructor *****/
   Lnk_EditingLinkDestructor ();
  }

static void Lnk_EditLinksInternal (void)
  {
   extern const char *Hlp_SYSTEM_Links_edit;
   extern const char *Txt_Links;

   /***** Get list of links *****/
   Lnk_GetListLinks ();

   /***** Begin box *****/
   Box_BoxBegin (NULL,Txt_Links,
                 Lnk_PutIconsEditingLinks,NULL,
                 Hlp_SYSTEM_Links_edit,Box_NOT_CLOSABLE);

      /***** Put a form to create a new link *****/
      Lnk_PutFormToCreateLink ();

      /***** Forms to edit current links *****/
      if (Gbl.Links.Num)
	 Lnk_ListLinksForEdition ();

   /***** End box *****/
   Box_BoxEnd ();

   /***** Free list of links *****/
   Lnk_FreeListLinks ();
  }

/*****************************************************************************/
/******************** Put contextual icons to view links *********************/
/*****************************************************************************/

static void Lnk_PutIconsEditingLinks (__attribute__((unused)) void *Args)
  {
   /***** Put icon to view links *****/
   Lnk_PutIconToViewLinks ();

   /***** Put icon to view banners *****/
   Ban_PutIconToViewBanners ();
  }

/*****************************************************************************/
/************************** Put icon to view links ***************************/
/*****************************************************************************/

void Lnk_PutIconToViewLinks (void)
  {
   extern const char *Txt_Links;

   Lay_PutContextualLinkOnlyIcon (ActSeeLnk,NULL,
                                  NULL,NULL,
				  "link.svg",Ico_BLACK,
				  Txt_Links);
  }

/*****************************************************************************/
/****************************** List all links *******************************/
/*****************************************************************************/

void Lnk_GetListLinks (void)
  {
   MYSQL_RES *mysql_res;
   unsigned NumLnk;

   if (Gbl.DB.DatabaseIsOpen)
     {
      /***** Get institutional links from database *****/
      if ((Gbl.Links.Num = Lnk_DB_GetLinks (&mysql_res))) // Links found...
	{
	 /***** Create list with places *****/
	 if ((Gbl.Links.Lst = calloc ((size_t) Gbl.Links.Num,
	                              sizeof (*Gbl.Links.Lst))) == NULL)
	     Err_NotEnoughMemoryExit ();

	 /***** Get the links *****/
	 for (NumLnk = 0;
	      NumLnk < Gbl.Links.Num;
	      NumLnk++)
	    /* Get next link */
	    Lnk_GetDataOfLink (mysql_res,&(Gbl.Links.Lst[NumLnk]));
	}

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);
     }
  }

/*****************************************************************************/
/**************************** Get link full name *****************************/
/*****************************************************************************/

void Lnk_GetDataOfLinkByCod (struct Lnk_Link *Lnk)
  {
   MYSQL_RES *mysql_res;

   /***** Clear data *****/
   Lnk->ShrtName[0] =
   Lnk->FullName[0] =
   Lnk->WWW[0] = '\0';

   /***** Check if link code is correct *****/
   if (Lnk->LnkCod > 0)
     {
      /***** Get data of an institutional link from database *****/
      if (Lnk_DB_GetDataOfLinkByCod (&mysql_res,Lnk->LnkCod)) // Link found...
	 Lnk_GetDataOfLink (mysql_res,Lnk);

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);
     }
  }

/*****************************************************************************/
/**************************** Get data of link *******************************/
/*****************************************************************************/

static void Lnk_GetDataOfLink (MYSQL_RES *mysql_res,struct Lnk_Link *Lnk)
  {
   MYSQL_ROW row;

   /***** Get row *****/
   row = mysql_fetch_row (mysql_res);
   /*
   row[0]	LnkCod
   row[1]	ShortName
   row[2]	FullName
   row[3]	WWW
   */
   /***** Get the short name (row[1]),
	      the full name (row[2])
          and the URL (row[3]) of the link *****/
   Str_Copy (Lnk->ShrtName,row[1],sizeof (Lnk->ShrtName) - 1);
   Str_Copy (Lnk->FullName,row[2],sizeof (Lnk->FullName) - 1);
   Str_Copy (Lnk->WWW     ,row[3],sizeof (Lnk->WWW     ) - 1);
  }

/*****************************************************************************/
/**************************** Free list of links *****************************/
/*****************************************************************************/

void Lnk_FreeListLinks (void)
  {
   if (Gbl.Links.Lst)
     {
      /***** Free memory used by the list of links *****/
      free (Gbl.Links.Lst);
      Gbl.Links.Lst = NULL;
      Gbl.Links.Num = 0;
     }
  }

/*****************************************************************************/
/****************************** List all links *******************************/
/*****************************************************************************/

static void Lnk_ListLinksForEdition (void)
  {
   extern const char *The_ClassDat[The_NUM_THEMES];
   extern const char *The_ClassInput[The_NUM_THEMES];
   unsigned NumLnk;
   struct Lnk_Link *Lnk;

   /***** Begin table *****/
   HTM_TABLE_BeginWidePadding (2);

      /***** Table head *****/
      Lnk_PutHeadLinks ();

      /***** Write all links *****/
      for (NumLnk = 0;
	   NumLnk < Gbl.Links.Num;
	   NumLnk++)
	{
	 Lnk = &Gbl.Links.Lst[NumLnk];

	 HTM_TR_Begin (NULL);

	    /* Put icon to remove link */
	    HTM_TD_Begin ("class=\"BM\"");
	       Ico_PutContextualIconToRemove (ActRemLnk,NULL,
					      Lnk_PutParamLnkCod,&Lnk->LnkCod);
	    HTM_TD_End ();

	    /* Link code */
	    HTM_TD_Begin ("class=\"%s RM\"",The_ClassDat[Gbl.Prefs.Theme]);
	       HTM_Long (Lnk->LnkCod);
	    HTM_TD_End ();

	    /* Link short name */
	    HTM_TD_Begin ("class=\"CM\"");
	       Frm_BeginForm (ActRenLnkSho);
		  Lnk_PutParamLnkCod (&Lnk->LnkCod);
		  HTM_INPUT_TEXT ("ShortName",Lnk_MAX_CHARS_LINK_SHRT_NAME,Lnk->ShrtName,
				  HTM_SUBMIT_ON_CHANGE,
				  "class=\"INPUT_SHORT_NAME %s\""
				  " required=\"required\"",
				  The_ClassInput[Gbl.Prefs.Theme]);
	       Frm_EndForm ();
	    HTM_TD_End ();

	    /* Link full name */
	    HTM_TD_Begin ("class=\"CM\"");
	       Frm_BeginForm (ActRenLnkFul);
		  Lnk_PutParamLnkCod (&Lnk->LnkCod);
		  HTM_INPUT_TEXT ("FullName",Lnk_MAX_CHARS_LINK_FULL_NAME,Lnk->FullName,
				  HTM_SUBMIT_ON_CHANGE,
				  "class=\"INPUT_FULL_NAME %s\""
				  " required=\"required\"",
				  The_ClassInput[Gbl.Prefs.Theme]);
	       Frm_EndForm ();
	    HTM_TD_End ();

	    /* Link WWW */
	    HTM_TD_Begin ("class=\"CM\"");
	       Frm_BeginForm (ActChgLnkWWW);
	       Lnk_PutParamLnkCod (&Lnk->LnkCod);
		  HTM_INPUT_URL ("WWW",Lnk->WWW,HTM_SUBMIT_ON_CHANGE,
				 "class=\"INPUT_WWW_NARROW %s\""
				 " required=\"required\"",
				 The_ClassInput[Gbl.Prefs.Theme]);
	       Frm_EndForm ();
	    HTM_TD_End ();

	 HTM_TR_End ();
	}

   /***** End table *****/
   HTM_TABLE_End ();
  }

/*****************************************************************************/
/******************** Write parameter with code of link **********************/
/*****************************************************************************/

static void Lnk_PutParamLnkCod (void *LnkCod)
  {
   if (LnkCod)
      Par_PutHiddenParamLong (NULL,"LnkCod",*((long *) LnkCod));
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

   /***** Link constructor *****/
   Lnk_EditingLinkConstructor ();

   /***** Get link code *****/
   if ((Lnk_EditingLnk->LnkCod = Lnk_GetParamLnkCod ()) <= 0)
      Err_WrongLinkExit ();

   /***** Get data of the link from database *****/
   Lnk_GetDataOfLinkByCod (Lnk_EditingLnk);

   /***** Remove link *****/
   Lnk_DB_RemoveLink (Lnk_EditingLnk->LnkCod);

   /***** Write message to show the change made *****/
   Ale_CreateAlert (Ale_SUCCESS,NULL,
	            Txt_Link_X_removed,
                    Lnk_EditingLnk->ShrtName);
  }

/*****************************************************************************/
/********************* Change the short name of a link ***********************/
/*****************************************************************************/

void Lnk_RenameLinkShort (void)
  {
   /***** Link constructor *****/
   Lnk_EditingLinkConstructor ();

   /***** Rename link *****/
   Lnk_RenameLink (Cns_SHRT_NAME);
  }

/*****************************************************************************/
/********************* Change the full name of a link ************************/
/*****************************************************************************/

void Lnk_RenameLinkFull (void)
  {
   /***** Link constructor *****/
   Lnk_EditingLinkConstructor ();

   /***** Rename link *****/
   Lnk_RenameLink (Cns_FULL_NAME);
  }

/*****************************************************************************/
/************************ Change the name of a link **************************/
/*****************************************************************************/

static void Lnk_RenameLink (Cns_ShrtOrFullName_t ShrtOrFullName)
  {
   extern const char *Txt_The_link_X_already_exists;
   extern const char *Txt_The_link_X_has_been_renamed_as_Y;
   extern const char *Txt_The_name_of_the_link_X_has_not_changed;
   const char *ParamName = NULL;	// Initialized to avoid warning
   const char *FieldName = NULL;	// Initialized to avoid warning
   unsigned MaxBytes = 0;		// Initialized to avoid warning
   char *CurrentLnkName = NULL;		// Initialized to avoid warning
   char NewLnkName[Lnk_MAX_BYTES_LINK_FULL_NAME + 1];

   switch (ShrtOrFullName)
     {
      case Cns_SHRT_NAME:
         ParamName = "ShortName";
         FieldName = "ShortName";
         MaxBytes = Lnk_MAX_BYTES_LINK_SHRT_NAME;
         CurrentLnkName = Lnk_EditingLnk->ShrtName;
         break;
      case Cns_FULL_NAME:
         ParamName = "FullName";
         FieldName = "FullName";
         MaxBytes = Lnk_MAX_BYTES_LINK_FULL_NAME;
         CurrentLnkName = Lnk_EditingLnk->FullName;
         break;
     }

   /***** Get parameters from form *****/
   /* Get the code of the link */
   if ((Lnk_EditingLnk->LnkCod = Lnk_GetParamLnkCod ()) <= 0)
      Err_WrongLinkExit ();

   /* Get the new name for the link */
   Par_GetParToText (ParamName,NewLnkName,MaxBytes);

   /***** Get link data from the database *****/
   Lnk_GetDataOfLinkByCod (Lnk_EditingLnk);

   /***** Check if new name is empty *****/
   if (NewLnkName[0])
     {
      /***** Check if old and new names are the same
             (this happens when return is pressed without changes) *****/
      if (strcmp (CurrentLnkName,NewLnkName))	// Different names
        {
         /***** If link was in database... *****/
         if (Lnk_DB_CheckIfLinkNameExists (ParamName,NewLnkName,Lnk_EditingLnk->LnkCod))
            Ale_CreateAlert (Ale_WARNING,NULL,
        	             Txt_The_link_X_already_exists,
                             NewLnkName);
         else
           {
            /* Update the table changing old name by new name */
            Lnk_DB_UpdateLnkName (Lnk_EditingLnk->LnkCod,FieldName,NewLnkName);

            /* Write message to show the change made */
            Ale_CreateAlert (Ale_SUCCESS,NULL,
        	             Txt_The_link_X_has_been_renamed_as_Y,
                             CurrentLnkName,NewLnkName);
           }
        }
      else	// The same name
         Ale_CreateAlert (Ale_INFO,NULL,
                          Txt_The_name_of_the_link_X_has_not_changed,
                          CurrentLnkName);
     }
   else
      Ale_CreateAlertYouCanNotLeaveFieldEmpty ();

   /***** Update name *****/
   Str_Copy (CurrentLnkName,NewLnkName,MaxBytes);
  }

/*****************************************************************************/
/**************** Change the WWW of an institutional link ********************/
/*****************************************************************************/

void Lnk_ChangeLinkWWW (void)
  {
   extern const char *Txt_The_new_web_address_is_X;
   char NewWWW[Cns_MAX_BYTES_WWW + 1];

   /***** Link constructor *****/
   Lnk_EditingLinkConstructor ();

   /***** Get parameters from form *****/
   /* Get the code of the link */
   if ((Lnk_EditingLnk->LnkCod = Lnk_GetParamLnkCod ()) <= 0)
      Err_WrongLinkExit ();

   /* Get the new WWW for the link */
   Par_GetParToText ("WWW",NewWWW,Cns_MAX_BYTES_WWW);

   /***** Get link data from the database *****/
   Lnk_GetDataOfLinkByCod (Lnk_EditingLnk);

   /***** Check if new WWW is empty *****/
   if (NewWWW[0])
     {
      /***** Update the table changing old WWW by new WWW *****/
      Lnk_DB_UpdateLnkWWW (Lnk_EditingLnk->LnkCod,NewWWW);

      /***** Message to show the change made *****/
      Ale_CreateAlert (Ale_SUCCESS,NULL,
	               Txt_The_new_web_address_is_X,
                       NewWWW);
     }
   else
      Ale_CreateAlertYouCanNotLeaveFieldEmpty ();

   /***** Update web *****/
   Str_Copy (Lnk_EditingLnk->WWW,NewWWW,sizeof (Lnk_EditingLnk->WWW) - 1);
  }

/*****************************************************************************/
/********** Show alerts after changing a link and continue editing ***********/
/*****************************************************************************/

void Lnk_ContEditAfterChgLnk (void)
  {
   /***** Write message to show the change made *****/
   Ale_ShowAlerts (NULL);

   /***** Show the form again *****/
   Lnk_EditLinksInternal ();

   /***** Link destructor *****/
   Lnk_EditingLinkDestructor ();
  }

/*****************************************************************************/
/********************* Put a form to create a new link ***********************/
/*****************************************************************************/

static void Lnk_PutFormToCreateLink (void)
  {
   extern const char *Hlp_SYSTEM_Links_edit;
   extern const char *The_ClassInput[The_NUM_THEMES];
   extern const char *Txt_New_link;
   extern const char *Txt_Create_link;

   /***** Begin form *****/
   Frm_BeginForm (ActNewLnk);

      /***** Begin box and table *****/
      Box_BoxTableBegin (NULL,Txt_New_link,
			 NULL,NULL,
			 Hlp_SYSTEM_Links_edit,Box_NOT_CLOSABLE,2);

	 /***** Write heading *****/
	 Lnk_PutHeadLinks ();

	 HTM_TR_Begin (NULL);

	    /***** Link code *****/
	    HTM_TD_Begin ("class=\"BM\"");
	    HTM_TD_End ();

	    HTM_TD_Empty (1);

	    /***** Link short name *****/
	    HTM_TD_Begin ("class=\"CM\"");
	       HTM_INPUT_TEXT ("ShortName",Lnk_MAX_CHARS_LINK_SHRT_NAME,Lnk_EditingLnk->ShrtName,
			       HTM_DONT_SUBMIT_ON_CHANGE,
			       "class=\"INPUT_SHORT_NAME %s\""
			       " required=\"required\"",
			       The_ClassInput[Gbl.Prefs.Theme]);
	    HTM_TD_End ();

	    /***** Link full name *****/
	    HTM_TD_Begin ("class=\"CM\"");
	       HTM_INPUT_TEXT ("FullName",Lnk_MAX_CHARS_LINK_FULL_NAME,Lnk_EditingLnk->FullName,
			       HTM_DONT_SUBMIT_ON_CHANGE,
			       "class=\"INPUT_FULL_NAME %s\""
			       " required=\"required\"",
			       The_ClassInput[Gbl.Prefs.Theme]);
	    HTM_TD_End ();

	    /***** Link WWW *****/
	    HTM_TD_Begin ("class=\"CM\"");
	       HTM_INPUT_URL ("WWW",Lnk_EditingLnk->WWW,HTM_DONT_SUBMIT_ON_CHANGE,
			      "class=\"INPUT_WWW_NARROW %s\""
			      " required=\"required\"",
			      The_ClassInput[Gbl.Prefs.Theme]);
	    HTM_TD_End ();

	 HTM_TR_End ();

      /***** End table, send button and end box *****/
      Box_BoxTableWithButtonEnd (Btn_CREATE_BUTTON,Txt_Create_link);

   /***** End form *****/
   Frm_EndForm ();
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

   HTM_TR_Begin (NULL);
      HTM_TH_Span (NULL     ,HTM_HEAD_CENTER,1,1,"BT");
      HTM_TH (Txt_Code      ,HTM_HEAD_RIGHT );
      HTM_TH (Txt_Short_name,HTM_HEAD_LEFT  );
      HTM_TH (Txt_Full_name ,HTM_HEAD_LEFT  );
      HTM_TH (Txt_WWW       ,HTM_HEAD_LEFT  );
   HTM_TR_End ();
  }

/*****************************************************************************/
/******************* Receive form to create a new link ***********************/
/*****************************************************************************/

void Lnk_ReceiveFormNewLink (void)
  {
   extern const char *Txt_The_link_X_already_exists;
   extern const char *Txt_You_must_specify_the_URL_of_the_new_link;
   extern const char *Txt_Created_new_link_X;
   extern const char *Txt_You_must_specify_the_short_name_and_the_full_name_of_the_new_link;

   /***** Link constructor *****/
   Lnk_EditingLinkConstructor ();

   /***** Get parameters from form *****/
   /* Get link short name */
   Par_GetParToText ("ShortName",Lnk_EditingLnk->ShrtName,Lnk_MAX_BYTES_LINK_SHRT_NAME);

   /* Get link full name */
   Par_GetParToText ("FullName",Lnk_EditingLnk->FullName,Lnk_MAX_BYTES_LINK_FULL_NAME);

   /* Get link URL */
   Par_GetParToText ("WWW",Lnk_EditingLnk->WWW,Cns_MAX_BYTES_WWW);

   if (Lnk_EditingLnk->ShrtName[0] &&
       Lnk_EditingLnk->FullName[0])	// If there's a link name
     {
      /***** If name of link was in database... *****/
      if (Lnk_DB_CheckIfLinkNameExists ("ShortName",Lnk_EditingLnk->ShrtName,-1L))
         Ale_CreateAlert (Ale_WARNING,NULL,
                          Txt_The_link_X_already_exists,
                          Lnk_EditingLnk->ShrtName);
      else if (Lnk_DB_CheckIfLinkNameExists ("FullName",Lnk_EditingLnk->FullName,-1L))
         Ale_CreateAlert (Ale_WARNING,NULL,
                          Txt_The_link_X_already_exists,
                          Lnk_EditingLnk->FullName);
      else if (!Lnk_EditingLnk->WWW[0])
         Ale_CreateAlert (Ale_WARNING,NULL,
                          Txt_You_must_specify_the_URL_of_the_new_link);
      else	// Add new link to database
        {
         Lnk_DB_CreateLink (Lnk_EditingLnk);
      	 Ale_CreateAlert (Ale_SUCCESS,NULL,
      	                  Txt_Created_new_link_X,
			  Lnk_EditingLnk->ShrtName);
        }
     }
   else	// If there is not a link name
      Ale_CreateAlert (Ale_WARNING,NULL,
	               Txt_You_must_specify_the_short_name_and_the_full_name_of_the_new_link);
  }

/*****************************************************************************/
/************************* Place constructor/destructor **********************/
/*****************************************************************************/

static void Lnk_EditingLinkConstructor (void)
  {
   /***** Pointer must be NULL *****/
   if (Lnk_EditingLnk != NULL)
      Err_WrongLinkExit ();

   /***** Allocate memory for link *****/
   if ((Lnk_EditingLnk = malloc (sizeof (*Lnk_EditingLnk))) == NULL)
      Err_NotEnoughMemoryExit ();

   /***** Reset link *****/
   Lnk_EditingLnk->LnkCod      = -1L;
   Lnk_EditingLnk->ShrtName[0] = '\0';
   Lnk_EditingLnk->FullName[0] = '\0';
   Lnk_EditingLnk->WWW[0]      = '\0';
  }

static void Lnk_EditingLinkDestructor (void)
  {
   /***** Free memory used for link *****/
   if (Lnk_EditingLnk != NULL)
     {
      free (Lnk_EditingLnk);
      Lnk_EditingLnk = NULL;
     }
  }
