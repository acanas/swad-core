// swad_system_link.c: system links

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2025 Antonio Cañas Vargas

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
#include "swad_parameter.h"
#include "swad_parameter_code.h"
#include "swad_system_link.h"
#include "swad_system_link_database.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/******************************* Private types *******************************/
/*****************************************************************************/

struct SysLnk_Links
  {
   unsigned Num;		// Number of institutional links
   struct SysLnk_Link *Lst;	// List of institutional links
  };

/*****************************************************************************/
/***************************** Private variables *****************************/
/*****************************************************************************/

static struct SysLnk_Link *SysLnk_EditingLnk = NULL;	// Static variable to keep the link being edited

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static Usr_Can_t SysLnk_CheckIfICanEditLinks (void);

static void SysLnk_PutIconsListingLinks (__attribute__((unused)) void *Args);
static void SysLnk_PutIconToEditLinks (void);
static void SysLnk_WriteListOfLinks (const struct SysLnk_Links *Links,const char *Class);

static void SysLnk_EditLinksInternal (void);
static void SysLnk_PutIconsEditingLinks (__attribute__((unused)) void *Args);

static void SysLnk_GetListLinks (struct SysLnk_Links *Links);
static void SysLnk_GetLinkDataFromRow (MYSQL_RES *mysql_res,struct SysLnk_Link *Lnk);

static void SysLnk_FreeListLinks (struct SysLnk_Links *Links);

static void SysLnk_ListLinksForEdition (const struct SysLnk_Links *Links);
static void SysLnk_PutParLnkCod (void *LnkCod);

static void SysLnk_RenameLink (Nam_ShrtOrFullName_t ShrtOrFull);

static void SysLnk_PutFormToCreateLink (void);
static void SysLnk_PutHeadLinks (void);

static void SysLnk_EditingLinkConstructor (void);
static void SysLnk_EditingLinkDestructor (void);

/*****************************************************************************/
/************************* Check if I can edit links *************************/
/*****************************************************************************/

static Usr_Can_t SysLnk_CheckIfICanEditLinks (void)
  {
   static Usr_Can_t Lnk_ICanEditLinks[Rol_NUM_ROLES] =
     {
      /* Users who can edit */
      [Rol_SYS_ADM] = Usr_CAN,
     };

   return Lnk_ICanEditLinks[Gbl.Usrs.Me.Role.Logged];
  }

/*****************************************************************************/
/****************************** List all links *******************************/
/*****************************************************************************/

void SysLnk_SeeLinks (void)
  {
   extern const char *Hlp_SYSTEM_Links;
   extern const char *Txt_Links;
   extern const char *Txt_No_links;
   struct SysLnk_Links Links;

   /***** Get list of links *****/
   SysLnk_GetListLinks (&Links);

   /***** Begin box *****/
   Box_BoxBegin (Txt_Links,SysLnk_PutIconsListingLinks,NULL,
		 Hlp_SYSTEM_Links,Box_NOT_CLOSABLE);

      /***** Write all links *****/
      if (Links.Num)	// There are links
	 SysLnk_WriteListOfLinks (&Links,"class=\"LIST_LEFT\"");
      else			// No links created
	 Ale_ShowAlert (Ale_INFO,Txt_No_links);

   /***** End box *****/
   Box_BoxEnd ();

   /***** Free list of links *****/
   SysLnk_FreeListLinks (&Links);
  }

/*****************************************************************************/
/***************** Put contextual icons in list of links *********************/
/*****************************************************************************/

static void SysLnk_PutIconsListingLinks (__attribute__((unused)) void *Args)
  {
   /***** Put icon to edit links *****/
   if (SysLnk_CheckIfICanEditLinks () == Usr_CAN)
      SysLnk_PutIconToEditLinks ();

   /***** Put icon to view banners *****/
   Ban_PutIconToViewBanners ();
  }

/*****************************************************************************/
/************************** Put icon to edit links ***************************/
/*****************************************************************************/

static void SysLnk_PutIconToEditLinks (void)
  {
   Ico_PutContextualIconToEdit (ActEdiLnk,NULL,NULL,NULL);
  }

/*****************************************************************************/
/***************** Write menu with some institutional links ******************/
/*****************************************************************************/

void SysLnk_WriteMenuWithSystemLinks (void)
  {
   extern const char *Txt_Links;
   struct SysLnk_Links Links;

   /***** Get list of links *****/
   SysLnk_GetListLinks (&Links);

      /***** Write all links *****/
      if (Links.Num)
	{
	 HTM_FIELDSET_Begin ("id=\"institutional_links\" class=\"INS_LNK_%s\"",
			     The_GetSuffix ());
	    HTM_LEGEND (Txt_Links);

	    SysLnk_WriteListOfLinks (&Links,NULL);

	 HTM_FIELDSET_End ();
	}

   /***** Free list of links *****/
   SysLnk_FreeListLinks (&Links);
  }

/*****************************************************************************/
/*************************** Write list of links *****************************/
/*****************************************************************************/

static void SysLnk_WriteListOfLinks (const struct SysLnk_Links *Links,const char *Class)
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

void SysLnk_EditLinks (void)
  {
   /***** Link constructor *****/
   SysLnk_EditingLinkConstructor ();

   /***** Edit links *****/
   SysLnk_EditLinksInternal ();

   /***** Link destructor *****/
   SysLnk_EditingLinkDestructor ();
  }

static void SysLnk_EditLinksInternal (void)
  {
   extern const char *Hlp_SYSTEM_Links_edit;
   extern const char *Txt_Links;
   struct SysLnk_Links Links;

   /***** Get list of links *****/
   SysLnk_GetListLinks (&Links);

   /***** Begin box *****/
   Box_BoxBegin (Txt_Links,SysLnk_PutIconsEditingLinks,NULL,
                 Hlp_SYSTEM_Links_edit,Box_NOT_CLOSABLE);

      /***** Put a form to create a new link *****/
      SysLnk_PutFormToCreateLink ();

      /***** Forms to edit current links *****/
      if (Links.Num)
	 SysLnk_ListLinksForEdition (&Links);

   /***** End box *****/
   Box_BoxEnd ();

   /***** Free list of links *****/
   SysLnk_FreeListLinks (&Links);
  }

/*****************************************************************************/
/******************** Put contextual icons to view links *********************/
/*****************************************************************************/

static void SysLnk_PutIconsEditingLinks (__attribute__((unused)) void *Args)
  {
   /***** Put icon to view links *****/
   Ico_PutContextualIconToView (ActSeeLnk,NULL,NULL,NULL);

   /***** Put icon to view banners *****/
   Ban_PutIconToViewBanners ();
  }

/*****************************************************************************/
/************************** Put icon to view links ***************************/
/*****************************************************************************/

void SysLnk_PutIconToViewLinks (void)
  {
   Lay_PutContextualLinkOnlyIcon (ActSeeLnk,NULL,
                                  NULL,NULL,
				  "up-right-from-square.svg",Ico_BLACK);
  }

/*****************************************************************************/
/****************************** List all links *******************************/
/*****************************************************************************/

static void SysLnk_GetListLinks (struct SysLnk_Links *Links)
  {
   MYSQL_RES *mysql_res;
   unsigned NumLnk;

   /***** Reset links *****/
   Links->Num = 0;
   Links->Lst = NULL;

   if (DB_CheckIfDatabaseIsOpen () == CloOpe_OPEN)
     {
      /***** Get institutional links from database *****/
      if ((Links->Num = SysLnk_DB_GetLinks (&mysql_res))) // Links found...
	{
	 /***** Create list with places *****/
	 if ((Links->Lst = calloc ((size_t) Links->Num,
	                           sizeof (struct SysLnk_Link))) == NULL)
	     Err_NotEnoughMemoryExit ();

	 /***** Get the links *****/
	 for (NumLnk = 0;
	      NumLnk < Links->Num;
	      NumLnk++)
	    SysLnk_GetLinkDataFromRow (mysql_res,&Links->Lst[NumLnk]);
	}

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);
     }
  }

/*****************************************************************************/
/**************************** Get link full name *****************************/
/*****************************************************************************/

void SysLnk_GetLinkDataByCod (struct SysLnk_Link *Lnk)
  {
   MYSQL_RES *mysql_res;

   /***** Clear data *****/
   Lnk->ShrtName[0] =
   Lnk->FullName[0] =
   Lnk->WWW[0] = '\0';

   /***** Check if link code is correct *****/
   if (Lnk->LnkCod > 0)
     {
      /***** Get data of a system link from database *****/
      if (SysLnk_DB_GetLinkDataByCod (&mysql_res,Lnk->LnkCod) == Exi_EXISTS) // Link found...
	 SysLnk_GetLinkDataFromRow (mysql_res,Lnk);

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);
     }
  }

/*****************************************************************************/
/**************************** Get data of link *******************************/
/*****************************************************************************/

static void SysLnk_GetLinkDataFromRow (MYSQL_RES *mysql_res,struct SysLnk_Link *Lnk)
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

static void SysLnk_FreeListLinks (struct SysLnk_Links *Links)
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

static void SysLnk_ListLinksForEdition (const struct SysLnk_Links *Links)
  {
   static Act_Action_t ActionRename[Nam_NUM_SHRT_FULL_NAMES] =
     {
      [Nam_SHRT_NAME] = ActRenLnkSho,
      [Nam_FULL_NAME] = ActRenLnkFul,
     };
   unsigned NumLnk;
   struct SysLnk_Link *Lnk;
   const char *Names[Nam_NUM_SHRT_FULL_NAMES];

   /***** Begin table *****/
   HTM_TABLE_Begin ("TBL_SCROLL");

      /***** Table head *****/
      SysLnk_PutHeadLinks ();

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
					      SysLnk_PutParLnkCod,&Lnk->LnkCod);
	    HTM_TD_End ();

	    /* Link code */
	    HTM_TD_Begin ("class=\"CODE DAT_%s\"",The_GetSuffix ());
	       HTM_Long (Lnk->LnkCod);
	    HTM_TD_End ();

	    /* Link short name and full name */
	    Names[Nam_SHRT_NAME] = Lnk->ShrtName;
	    Names[Nam_FULL_NAME] = Lnk->FullName;
	    Nam_ExistingShortAndFullNames (ActionRename,
				           ParCod_Lnk,Lnk->LnkCod,
				           Names,
				           Frm_PUT_FORM);

	    /* Link WWW */
	    HTM_TD_Begin ("class=\"LM\"");
	       Frm_BeginForm (ActChgLnkWWW);
	       ParCod_PutPar (ParCod_Lnk,Lnk->LnkCod);
		  HTM_INPUT_URL ("WWW",Lnk->WWW,
				 HTM_REQUIRED | HTM_SUBMIT_ON_CHANGE,
				 "class=\"INPUT_WWW INPUT_%s\"",The_GetSuffix ());
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

static void SysLnk_PutParLnkCod (void *LnkCod)
  {
   if (LnkCod)
      ParCod_PutPar (ParCod_Lnk,*((long *) LnkCod));
  }

/*****************************************************************************/
/******************************* Remove a link *******************************/
/*****************************************************************************/

void SysLnk_RemoveLink (void)
  {
   extern const char *Txt_Link_removed;

   /***** Link constructor *****/
   SysLnk_EditingLinkConstructor ();

   /***** Get link code *****/
   SysLnk_EditingLnk->LnkCod = ParCod_GetAndCheckPar (ParCod_Lnk);

   /***** Remove link *****/
   SysLnk_DB_RemoveLink (SysLnk_EditingLnk->LnkCod);

   /***** Write message to show the change made *****/
   Ale_CreateAlert (Ale_SUCCESS,NULL,Txt_Link_removed);
  }

/*****************************************************************************/
/********************* Change the short name of a link ***********************/
/*****************************************************************************/

void SysLnk_RenameLinkShort (void)
  {
   /***** Link constructor *****/
   SysLnk_EditingLinkConstructor ();

   /***** Rename link *****/
   SysLnk_RenameLink (Nam_SHRT_NAME);
  }

/*****************************************************************************/
/********************* Change the full name of a link ************************/
/*****************************************************************************/

void SysLnk_RenameLinkFull (void)
  {
   /***** Link constructor *****/
   SysLnk_EditingLinkConstructor ();

   /***** Rename link *****/
   SysLnk_RenameLink (Nam_FULL_NAME);
  }

/*****************************************************************************/
/************************ Change the name of a link **************************/
/*****************************************************************************/

static void SysLnk_RenameLink (Nam_ShrtOrFullName_t ShrtOrFull)
  {
   extern const char *Nam_Fields[Nam_NUM_SHRT_FULL_NAMES];
   extern unsigned Nam_MaxBytes[Nam_NUM_SHRT_FULL_NAMES];
   extern const char *Txt_X_already_exists;
   extern const char *Txt_The_link_X_has_been_renamed_as_Y;
   extern const char *Txt_The_name_X_has_not_changed;
   char *CurrentName[Nam_NUM_SHRT_FULL_NAMES] =
     {
      [Nam_SHRT_NAME] = SysLnk_EditingLnk->ShrtName,
      [Nam_FULL_NAME] = SysLnk_EditingLnk->FullName,
     };
   char NewName[Nam_MAX_BYTES_FULL_NAME + 1];

   /***** Get parameters from form *****/
   /* Get the code of the link */
   SysLnk_EditingLnk->LnkCod = ParCod_GetAndCheckPar (ParCod_Lnk);

   /* Get the new name for the link */
   Nam_GetParShrtOrFullName (ShrtOrFull,NewName);

   /***** Get link data from the database *****/
   SysLnk_GetLinkDataByCod (SysLnk_EditingLnk);

   /***** Check if new name is empty *****/
   if (NewName[0])
     {
      /***** Check if old and new names are the same
             (this happens when return is pressed without changes) *****/
      if (strcmp (CurrentName[ShrtOrFull],NewName))	// Different names
         /***** If link was in database... *****/
         switch (SysLnk_DB_CheckIfLinkNameExists (Nam_Fields[ShrtOrFull],
						  NewName,SysLnk_EditingLnk->LnkCod,
						  -1L,	// Unused
						  0))	// Unused
	   {
	    case Exi_EXISTS:
	       Ale_CreateAlert (Ale_WARNING,NULL,Txt_X_already_exists,NewName);
	       break;
	    case Exi_DOES_NOT_EXIST:
	    default:
	       /* Update the table changing old name by new name */
	       SysLnk_DB_UpdateLnkName (SysLnk_EditingLnk->LnkCod,
				        Nam_Fields[ShrtOrFull],NewName);

	       /* Write message to show the change made */
	       Ale_CreateAlert (Ale_SUCCESS,NULL,
				Txt_The_link_X_has_been_renamed_as_Y,
				CurrentName[ShrtOrFull],NewName);
	       break;
	   }
      else	// The same name
         Ale_CreateAlert (Ale_INFO,NULL,Txt_The_name_X_has_not_changed,
                          CurrentName[ShrtOrFull]);
     }
   else
      Ale_CreateAlertYouCanNotLeaveFieldEmpty ();

   /***** Update name *****/
   Str_Copy (CurrentName[ShrtOrFull],NewName,Nam_MaxBytes[ShrtOrFull]);
  }

/*****************************************************************************/
/******************** Change the WWW of a system link ************************/
/*****************************************************************************/

void SysLnk_ChangeLinkWWW (void)
  {
   extern const char *Txt_The_new_web_address_is_X;
   char NewWWW[WWW_MAX_BYTES_WWW + 1];

   /***** Link constructor *****/
   SysLnk_EditingLinkConstructor ();

   /***** Get parameters from form *****/
   /* Get the code of the link */
   SysLnk_EditingLnk->LnkCod = ParCod_GetAndCheckPar (ParCod_Lnk);

   /* Get the new WWW for the link */
   Par_GetParText ("WWW",NewWWW,WWW_MAX_BYTES_WWW);

   /***** Get link data from the database *****/
   SysLnk_GetLinkDataByCod (SysLnk_EditingLnk);

   /***** Check if new WWW is empty *****/
   if (NewWWW[0])
     {
      /***** Update the table changing old WWW by new WWW *****/
      SysLnk_DB_UpdateLnkWWW (SysLnk_EditingLnk->LnkCod,NewWWW);

      /***** Message to show the change made *****/
      Ale_CreateAlert (Ale_SUCCESS,NULL,
	               Txt_The_new_web_address_is_X,
                       NewWWW);
     }
   else
      Ale_CreateAlertYouCanNotLeaveFieldEmpty ();

   /***** Update web *****/
   Str_Copy (SysLnk_EditingLnk->WWW,NewWWW,sizeof (SysLnk_EditingLnk->WWW) - 1);
  }

/*****************************************************************************/
/********** Show alerts after changing a link and continue editing ***********/
/*****************************************************************************/

void SysLnk_ContEditAfterChgLnk (void)
  {
   /***** Write message to show the change made *****/
   Ale_ShowAlerts (NULL);

   /***** Show the form again *****/
   SysLnk_EditLinksInternal ();

   /***** Link destructor *****/
   SysLnk_EditingLinkDestructor ();
  }

/*****************************************************************************/
/********************* Put a form to create a new link ***********************/
/*****************************************************************************/

static void SysLnk_PutFormToCreateLink (void)
  {
   const char *Names[Nam_NUM_SHRT_FULL_NAMES];

   /***** Begin form to create *****/
   Frm_BeginFormTable (ActNewLnk,NULL,NULL,NULL,"TBL_SCROLL");

      /***** Write heading *****/
      SysLnk_PutHeadLinks ();

      HTM_TR_Begin (NULL);

	 /***** Column to remove link, disabled here *****/
	 HTM_TD_Begin ("class=\"BM\"");
	 HTM_TD_End ();

	 /***** Link code *****/
	 HTM_TD_Begin ("class=\"CODE\"");
	 HTM_TD_End ();

	 /***** Link short name and full name *****/
	 Names[Nam_SHRT_NAME] = SysLnk_EditingLnk->ShrtName;
	 Names[Nam_FULL_NAME] = SysLnk_EditingLnk->FullName;
	 Nam_NewShortAndFullNames (Names);

	 /***** Link WWW *****/
	 HTM_TD_Begin ("class=\"LM\"");
	    HTM_INPUT_URL ("WWW",SysLnk_EditingLnk->WWW,
			   HTM_REQUIRED,
			   "class=\"INPUT_WWW INPUT_%s\"",The_GetSuffix ());
	 HTM_TD_End ();

      HTM_TR_End ();

   /***** End form to create *****/
   Frm_EndFormTable (Btn_CREATE);
  }

/*****************************************************************************/
/******************** Write header with fields of a link *********************/
/*****************************************************************************/

static void SysLnk_PutHeadLinks (void)
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

void SysLnk_ReceiveNewLink (void)
  {
   extern const char *Txt_You_must_specify_the_web_address;
   extern const char *Txt_Created_new_link_X;
   char *Names[Nam_NUM_SHRT_FULL_NAMES];

   /***** Link constructor *****/
   SysLnk_EditingLinkConstructor ();

   /***** Get parameters from form *****/
   /* Get link short name and full name */
   Names[Nam_SHRT_NAME] = SysLnk_EditingLnk->ShrtName;
   Names[Nam_FULL_NAME] = SysLnk_EditingLnk->FullName;
   Nam_GetParsShrtAndFullName (Names);

   /* Get link URL */
   Par_GetParText ("WWW",SysLnk_EditingLnk->WWW,WWW_MAX_BYTES_WWW);

   if (SysLnk_EditingLnk->ShrtName[0] &&
       SysLnk_EditingLnk->FullName[0])	// If there's a link name
     {
      /***** If name of link was in database... *****/
      if (Nam_CheckIfNameExists (SysLnk_DB_CheckIfLinkNameExists,
				 (const char **) Names,
				 -1L,
				 -1L,				// Unused
				 0) == Exi_DOES_NOT_EXIST)	// Unused
	{
	 if (!SysLnk_EditingLnk->WWW[0])
	    Ale_CreateAlert (Ale_WARNING,NULL,
			     Txt_You_must_specify_the_web_address);
	 else	// Add new link to database
	   {
	    SysLnk_DB_CreateLink (SysLnk_EditingLnk);
	    Ale_CreateAlert (Ale_SUCCESS,NULL,Txt_Created_new_link_X,
			     Names[Nam_FULL_NAME]);
	   }
	}
     }
   else	// If there is not a link name
      Ale_CreateAlertYouMustSpecifyShrtNameAndFullName ();
  }

/*****************************************************************************/
/************************* Place constructor/destructor **********************/
/*****************************************************************************/

static void SysLnk_EditingLinkConstructor (void)
  {
   /***** Pointer must be NULL *****/
   if (SysLnk_EditingLnk != NULL)
      Err_WrongLinkExit ();

   /***** Allocate memory for link *****/
   if ((SysLnk_EditingLnk = malloc (sizeof (*SysLnk_EditingLnk))) == NULL)
      Err_NotEnoughMemoryExit ();

   /***** Reset link *****/
   SysLnk_EditingLnk->LnkCod      = -1L;
   SysLnk_EditingLnk->ShrtName[0] = '\0';
   SysLnk_EditingLnk->FullName[0] = '\0';
   SysLnk_EditingLnk->WWW[0]      = '\0';
  }

static void SysLnk_EditingLinkDestructor (void)
  {
   /***** Free memory used for link *****/
   if (SysLnk_EditingLnk != NULL)
     {
      free (SysLnk_EditingLnk);
      SysLnk_EditingLnk = NULL;
     }
  }
