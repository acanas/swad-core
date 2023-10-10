// swad_link.c: institutional links

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2023 Antonio Cañas Vargas

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

#include "swad_action_list.h"
#include "swad_alert.h"
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
#include "swad_parameter_code.h"

/*****************************************************************************/
/****************************** Private constants ****************************/
/*****************************************************************************/

static const bool Lnk_ICanEditLinks[Rol_NUM_ROLES] =
  {
   /* Users who can edit */
   [Rol_SYS_ADM] = true,
  };

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/******************************* Private types *******************************/
/*****************************************************************************/

struct Lnk_Links
  {
   unsigned Num;		// Number of institutional links
   struct Lnk_Link *Lst;	// List of institutional links
  };

/*****************************************************************************/
/***************************** Private variables *****************************/
/*****************************************************************************/

static struct Lnk_Link *Lnk_EditingLnk = NULL;	// Static variable to keep the link being edited

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Lnk_PutIconsListingLinks (__attribute__((unused)) void *Args);
static void Lnk_PutIconToEditLinks (void);
static void Lnk_WriteListOfLinks (const struct Lnk_Links *Links,const char *Class);

static void Lnk_EditLinksInternal (void);
static void Lnk_PutIconsEditingLinks (__attribute__((unused)) void *Args);

static void Lnk_GetListLinks (struct Lnk_Links *Links);
static void Lnk_GetLinkDataFromRow (MYSQL_RES *mysql_res,struct Lnk_Link *Lnk);

static void Lnk_FreeListLinks (struct Lnk_Links *Links);

static void Lnk_ListLinksForEdition (const struct Lnk_Links *Links);
static void Lnk_PutParLnkCod (void *LnkCod);

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
   struct Lnk_Links Links;

   /***** Get list of links *****/
   Lnk_GetListLinks (&Links);

   /***** Begin box *****/
   Box_BoxBegin (NULL,Txt_Links,
                 Lnk_PutIconsListingLinks,NULL,
		 Hlp_SYSTEM_Links,Box_NOT_CLOSABLE);

      /***** Write all links *****/
      if (Links.Num)	// There are links
	 Lnk_WriteListOfLinks (&Links,"class=\"LIST_LEFT\"");
      else			// No links created
	 Ale_ShowAlert (Ale_INFO,Txt_No_links);

   /***** End box *****/
   Box_BoxEnd ();

   /***** Free list of links *****/
   Lnk_FreeListLinks (&Links);
  }

/*****************************************************************************/
/***************** Put contextual icons in list of links *********************/
/*****************************************************************************/

static void Lnk_PutIconsListingLinks (__attribute__((unused)) void *Args)
  {
   /***** Put icon to edit links *****/
   if (Lnk_ICanEditLinks[Gbl.Usrs.Me.Role.Logged])
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
   extern const char *Txt_Links;
   struct Lnk_Links Links;

   /***** Get list of links *****/
   Lnk_GetListLinks (&Links);

      /***** Write all links *****/
      if (Links.Num)
	{
	 HTM_FIELDSET_Begin ("id=\"institutional_links\" class=\"INS_LNK_%s\"",
			     The_GetSuffix ());
	    HTM_LEGEND (Txt_Links);

	    Lnk_WriteListOfLinks (&Links,NULL);

	 HTM_FIELDSET_End ();
	}

   /***** Free list of links *****/
   Lnk_FreeListLinks (&Links);
  }

/*****************************************************************************/
/*************************** Write list of links *****************************/
/*****************************************************************************/

static void Lnk_WriteListOfLinks (const struct Lnk_Links *Links,const char *Class)
  {
   unsigned NumLnk;

   /***** List start *****/
   HTM_UL_Begin (Class);

      /***** Write all links *****/
      for (NumLnk = 0;
	   NumLnk < Links->Num;
	   NumLnk++)
	{
	 /* Write data of this link */
	 HTM_LI_Begin ("class=\"ICO_HIGHLIGHT INS_LNK\"");
	    HTM_A_Begin ("href=\"%s\" title=\"%s\""
		         " class=\"INS_LNK_%s\" target=\"_blank\"",
			 Links->Lst[NumLnk].WWW,
			 Links->Lst[NumLnk].FullName,
			 The_GetSuffix ());
	       HTM_Txt (Links->Lst[NumLnk].ShrtName);
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
   struct Lnk_Links Links;

   /***** Get list of links *****/
   Lnk_GetListLinks (&Links);

   /***** Begin box *****/
   Box_BoxBegin (NULL,Txt_Links,
                 Lnk_PutIconsEditingLinks,NULL,
                 Hlp_SYSTEM_Links_edit,Box_NOT_CLOSABLE);

      /***** Put a form to create a new link *****/
      Lnk_PutFormToCreateLink ();

      /***** Forms to edit current links *****/
      if (Links.Num)
	 Lnk_ListLinksForEdition (&Links);

   /***** End box *****/
   Box_BoxEnd ();

   /***** Free list of links *****/
   Lnk_FreeListLinks (&Links);
  }

/*****************************************************************************/
/******************** Put contextual icons to view links *********************/
/*****************************************************************************/

static void Lnk_PutIconsEditingLinks (__attribute__((unused)) void *Args)
  {
   /***** Put icon to view links *****/
   Ico_PutContextualIconToView (ActSeeLnk,NULL,
				NULL,NULL);

   /***** Put icon to view banners *****/
   Ban_PutIconToViewBanners ();
  }

/*****************************************************************************/
/************************** Put icon to view links ***************************/
/*****************************************************************************/

void Lnk_PutIconToViewLinks (void)
  {
   Lay_PutContextualLinkOnlyIcon (ActSeeLnk,NULL,
                                  NULL,NULL,
				  "up-right-from-square.svg",Ico_BLACK);
  }

/*****************************************************************************/
/****************************** List all links *******************************/
/*****************************************************************************/

static void Lnk_GetListLinks (struct Lnk_Links *Links)
  {
   MYSQL_RES *mysql_res;
   unsigned NumLnk;

   /***** Reset links *****/
   Links->Num = 0;
   Links->Lst = NULL;

   if (DB_CheckIfDatabaseIsOpen ())
     {
      /***** Get institutional links from database *****/
      if ((Links->Num = Lnk_DB_GetLinks (&mysql_res))) // Links found...
	{
	 /***** Create list with places *****/
	 if ((Links->Lst = calloc ((size_t) Links->Num,
	                           sizeof (struct Lnk_Link))) == NULL)
	     Err_NotEnoughMemoryExit ();

	 /***** Get the links *****/
	 for (NumLnk = 0;
	      NumLnk < Links->Num;
	      NumLnk++)
	    Lnk_GetLinkDataFromRow (mysql_res,&Links->Lst[NumLnk]);
	}

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);
     }
  }

/*****************************************************************************/
/**************************** Get link full name *****************************/
/*****************************************************************************/

void Lnk_GetLinkDataByCod (struct Lnk_Link *Lnk)
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
      if (Lnk_DB_GetLinkDataByCod (&mysql_res,Lnk->LnkCod)) // Link found...
	 Lnk_GetLinkDataFromRow (mysql_res,Lnk);

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);
     }
  }

/*****************************************************************************/
/**************************** Get data of link *******************************/
/*****************************************************************************/

static void Lnk_GetLinkDataFromRow (MYSQL_RES *mysql_res,struct Lnk_Link *Lnk)
  {
   MYSQL_ROW row;

   /***** Get next row from result *****/
   row = mysql_fetch_row (mysql_res);
   /*
   row[0]	LnkCod
   row[1]	ShortName
   row[2]	FullName
   row[3]	WWW
   */
   /***** Get plugin code (row[0]) *****/
   if ((Lnk->LnkCod = Str_ConvertStrCodToLongCod (row[0])) <= 0)
      Err_WrongLinkExit ();

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

static void Lnk_FreeListLinks (struct Lnk_Links *Links)
  {
   if (Links->Num && Links->Lst)
     {
      free (Links->Lst);
      Links->Lst = NULL;
      Links->Num = 0;
     }
  }

/*****************************************************************************/
/****************************** List all links *******************************/
/*****************************************************************************/

static void Lnk_ListLinksForEdition (const struct Lnk_Links *Links)
  {
   static Act_Action_t ActionRename[Cns_NUM_SHRT_FULL_NAMES] =
     {
      [Cns_SHRT_NAME] = ActRenLnkSho,
      [Cns_FULL_NAME] = ActRenLnkFul,
     };
   unsigned NumLnk;
   struct Lnk_Link *Lnk;
   const char *Names[Cns_NUM_SHRT_FULL_NAMES];

   /***** Begin table *****/
   HTM_TABLE_BeginWidePadding (2);

      /***** Table head *****/
      Lnk_PutHeadLinks ();

      /***** Write all links *****/
      for (NumLnk = 0;
	   NumLnk < Links->Num;
	   NumLnk++)
	{
	 Lnk = &Links->Lst[NumLnk];

	 HTM_TR_Begin (NULL);

	    /* Put icon to remove link */
	    HTM_TD_Begin ("class=\"BM\"");
	       Ico_PutContextualIconToRemove (ActRemLnk,NULL,
					      Lnk_PutParLnkCod,&Lnk->LnkCod);
	    HTM_TD_End ();

	    /* Link code */
	    HTM_TD_Begin ("class=\"CODE DAT_%s\"",The_GetSuffix ());
	       HTM_Long (Lnk->LnkCod);
	    HTM_TD_End ();

	    /* Link short name and full name */
	    Names[Cns_SHRT_NAME] = Lnk->ShrtName;
	    Names[Cns_FULL_NAME] = Lnk->FullName;
	    Frm_ExistingShortAndFullNames (ActionRename,
				           ParCod_Lnk,Lnk->LnkCod,
				           Names,
				           true);	// Put form

	    /* Link WWW */
	    HTM_TD_Begin ("class=\"CM\"");
	       Frm_BeginForm (ActChgLnkWWW);
	       ParCod_PutPar (ParCod_Lnk,Lnk->LnkCod);
		  HTM_INPUT_URL ("WWW",Lnk->WWW,HTM_SUBMIT_ON_CHANGE,
				 "class=\"INPUT_WWW_NARROW INPUT_%s\""
				 " required=\"required\"",
				 The_GetSuffix ());
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

static void Lnk_PutParLnkCod (void *LnkCod)
  {
   if (LnkCod)
      ParCod_PutPar (ParCod_Lnk,*((long *) LnkCod));
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
   Lnk_EditingLnk->LnkCod = ParCod_GetAndCheckPar (ParCod_Lnk);

   /***** Get data of the link from database *****/
   Lnk_GetLinkDataByCod (Lnk_EditingLnk);

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
   extern const char *Cns_ParShrtOrFullName[Cns_NUM_SHRT_FULL_NAMES];
   extern const char *Cns_FldShrtOrFullName[Cns_NUM_SHRT_FULL_NAMES];
   extern unsigned Cns_MaxBytesShrtOrFullName[Cns_NUM_SHRT_FULL_NAMES];
   extern const char *Txt_The_link_X_already_exists;
   extern const char *Txt_The_link_X_has_been_renamed_as_Y;
   extern const char *Txt_The_name_X_has_not_changed;
   char *CurrentName[Cns_NUM_SHRT_FULL_NAMES] =
     {
      [Cns_SHRT_NAME] = Lnk_EditingLnk->ShrtName,
      [Cns_FULL_NAME] = Lnk_EditingLnk->FullName,
     };
   char NewName[Cns_MAX_BYTES_FULL_NAME + 1];

   /***** Get parameters from form *****/
   /* Get the code of the link */
   Lnk_EditingLnk->LnkCod = ParCod_GetAndCheckPar (ParCod_Lnk);

   /* Get the new name for the link */
   Par_GetParShrtOrFullName (ShrtOrFullName,NewName);

   /***** Get link data from the database *****/
   Lnk_GetLinkDataByCod (Lnk_EditingLnk);

   /***** Check if new name is empty *****/
   if (NewName[0])
     {
      /***** Check if old and new names are the same
             (this happens when return is pressed without changes) *****/
      if (strcmp (CurrentName[ShrtOrFullName],NewName))	// Different names
        {
         /***** If link was in database... *****/
         if (Lnk_DB_CheckIfLinkNameExists (Cns_ParShrtOrFullName[ShrtOrFullName],
					   NewName,Lnk_EditingLnk->LnkCod))
            Ale_CreateAlert (Ale_WARNING,NULL,
        	             Txt_The_link_X_already_exists,
                             NewName);
         else
           {
            /* Update the table changing old name by new name */
            Lnk_DB_UpdateLnkName (Lnk_EditingLnk->LnkCod,
        			  Cns_FldShrtOrFullName[ShrtOrFullName],NewName);

            /* Write message to show the change made */
            Ale_CreateAlert (Ale_SUCCESS,NULL,
        	             Txt_The_link_X_has_been_renamed_as_Y,
                             CurrentName[ShrtOrFullName],NewName);
           }
        }
      else	// The same name
         Ale_CreateAlert (Ale_INFO,NULL,
                          Txt_The_name_X_has_not_changed,
                          CurrentName[ShrtOrFullName]);
     }
   else
      Ale_CreateAlertYouCanNotLeaveFieldEmpty ();

   /***** Update name *****/
   Str_Copy (CurrentName[ShrtOrFullName],NewName,
	     Cns_MaxBytesShrtOrFullName[ShrtOrFullName]);
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
   Lnk_EditingLnk->LnkCod = ParCod_GetAndCheckPar (ParCod_Lnk);

   /* Get the new WWW for the link */
   Par_GetParText ("WWW",NewWWW,Cns_MAX_BYTES_WWW);

   /***** Get link data from the database *****/
   Lnk_GetLinkDataByCod (Lnk_EditingLnk);

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
   const char *Names[Cns_NUM_SHRT_FULL_NAMES] =
     {
      [Cns_SHRT_NAME] = Lnk_EditingLnk->ShrtName,
      [Cns_FULL_NAME] = Lnk_EditingLnk->FullName,
     };

   /***** Begin form to create *****/
   Frm_BeginFormTable (ActNewLnk,NULL,NULL,NULL);

      /***** Write heading *****/
      Lnk_PutHeadLinks ();

      HTM_TR_Begin (NULL);

	 /***** Column to remove link, disabled here *****/
	 HTM_TD_Begin ("class=\"BM\"");
	 HTM_TD_End ();

	 /***** Link code *****/
	 HTM_TD_Begin ("class=\"CODE\"");
	 HTM_TD_End ();

	 /***** Link short name and full name *****/
	 Frm_NewShortAndFullNames (Names);

	 /***** Link WWW *****/
	 HTM_TD_Begin ("class=\"CM\"");
	    HTM_INPUT_URL ("WWW",Lnk_EditingLnk->WWW,HTM_DONT_SUBMIT_ON_CHANGE,
			   "class=\"INPUT_WWW_NARROW INPUT_%s\""
			   " required=\"required\"",
			   The_GetSuffix ());
	 HTM_TD_End ();

      HTM_TR_End ();

   /***** End form to create *****/
   Frm_EndFormTable (Btn_CREATE_BUTTON);
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
   extern const char *Cns_ParShrtOrFullName[Cns_NUM_SHRT_FULL_NAMES];
   extern const char *Cns_FldShrtOrFullName[Cns_NUM_SHRT_FULL_NAMES];
   extern unsigned Cns_MaxBytesShrtOrFullName[Cns_NUM_SHRT_FULL_NAMES];
   extern const char *Txt_The_link_X_already_exists;
   extern const char *Txt_You_must_specify_the_web_address;
   extern const char *Txt_Created_new_link_X;
   Cns_ShrtOrFullName_t ShrtOrFullName;
   bool Exists;
   char *Names[Cns_NUM_SHRT_FULL_NAMES] =
     {
      [Cns_SHRT_NAME] = Lnk_EditingLnk->ShrtName,
      [Cns_FULL_NAME] = Lnk_EditingLnk->FullName,
     };

   /***** Link constructor *****/
   Lnk_EditingLinkConstructor ();

   /***** Get parameters from form *****/
   /* Get link short name and full name */
   Par_GetParsShrtAndFullName (Names);

   /* Get link URL */
   Par_GetParText ("WWW",Lnk_EditingLnk->WWW,Cns_MAX_BYTES_WWW);

   if (Lnk_EditingLnk->ShrtName[0] &&
       Lnk_EditingLnk->FullName[0])	// If there's a link name
     {
      /***** If name of link was in database... *****/
      for (ShrtOrFullName  = Cns_SHRT_NAME, Exists = false;
	   ShrtOrFullName <= Cns_FULL_NAME && !Exists;
	   ShrtOrFullName++)
	 if (Lnk_DB_CheckIfLinkNameExists (Cns_FldShrtOrFullName[ShrtOrFullName],
					   Names[ShrtOrFullName],-1L))
	   {
	    Ale_CreateAlert (Ale_WARNING,NULL,
			     Txt_The_link_X_already_exists,
			     Names[ShrtOrFullName]);
	    Exists = true;
	   }
      if (!Exists)
	{
	 if (!Lnk_EditingLnk->WWW[0])
	    Ale_CreateAlert (Ale_WARNING,NULL,
			     Txt_You_must_specify_the_web_address);
	 else	// Add new link to database
	   {
	    Lnk_DB_CreateLink (Lnk_EditingLnk);
	    Ale_CreateAlert (Ale_SUCCESS,NULL,
			     Txt_Created_new_link_X,
			     Lnk_EditingLnk->ShrtName);
	   }
	}
     }
   else	// If there is not a link name
      Ale_CreateAlertYouMustSpecifyShrtNameAndFullName ();
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
