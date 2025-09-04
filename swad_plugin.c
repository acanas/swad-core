// swad_plugin.c: plugins called from SWAD using web services

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
/*
TODO: Check if web service is called from an authorized IP.
*/
/*****************************************************************************/
/********************************* Headers ***********************************/
/*****************************************************************************/

#define _GNU_SOURCE 		// For asprintf
#include <stddef.h>		// For NULL
#include <stdio.h>		// For asprintf
#include <stdlib.h>		// For calloc, free
#include <string.h>

#include "swad_action_list.h"
#include "swad_API.h"
#include "swad_box.h"
#include "swad_config.h"
#include "swad_constant.h"
#include "swad_cryptography.h"
#include "swad_database.h"
#include "swad_error.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_HTML.h"
#include "swad_language.h"
#include "swad_parameter_code.h"
#include "swad_plugin.h"
#include "swad_plugin_database.h"
#include "swad_session.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/******************************* Private types *******************************/
/*****************************************************************************/

struct Plg_Plugins
  {
   unsigned Num;		// Number of plugins
   struct Plg_Plugin *Lst;	// List of plugins
  };

/*****************************************************************************/
/************************* Private global variables **************************/
/*****************************************************************************/

// Global because it's maintained from a-priori to a-posteriori functions
static struct Plg_Plugin *Plg_EditingPlg;	// Plugin being edited.

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Plg_PutIconToEditPlugins (__attribute__((unused)) void *Args);
static void Plg_PutIconToViewPlugins (__attribute__((unused)) void *Args);
static void Plg_EditPluginsInternal (void);

static void Plg_GetPluginDataFromRow (MYSQL_RES *mysql_res,
                                      struct Plg_Plugin *Plg);

static void Plg_FreeListPlugins (struct Plg_Plugins *Plugins);
static void Plg_ListPluginsForEdition (struct Plg_Plugins *Plugins);
static void Plg_PutParPlgCod (void *PlgCod);
static void Plg_GetListPlugins (struct Plg_Plugins *Plugins);
static void Plg_PutFormToCreatePlugin (void);
static void Plg_PutHeadPlugins (void);

static void Plg_EditingPluginConstructor (void);
static void Plg_EditingPluginDestructor (void);

/*****************************************************************************/
/************************** List available plugins ***************************/
/*****************************************************************************/

void Plg_ListPlugins (void)
  {
   extern const char *Hlp_SYSTEM_Plugins;
   extern const char *Txt_Option_under_development;
   extern const char *Txt_Plugins;
   extern const char *Txt_Plugin;
   unsigned NumPlg;
   struct Plg_Plugin *Plg;
   char URL[WWW_MAX_BYTES_WWW + Cns_BYTES_SESSION_ID + 1];
   char *Icon;
   struct Plg_Plugins Plugins;

   if (Gbl.Usrs.Me.Role.Logged != Rol_SYS_ADM)
     {
      Ale_ShowAlert (Ale_WARNING,Txt_Option_under_development);
      return;
     }

   /***** Get list of plugins *****/
   Plg_GetListPlugins (&Plugins);

   /***** Begin box and table *****/
   Box_BoxTableBegin (Txt_Plugins,
		      Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM ? Plg_PutIconToEditPlugins :
							       NULL,
		      NULL,
		      Hlp_SYSTEM_Plugins,Box_NOT_CLOSABLE,2);

      /***** Write table heading *****/
      HTM_TR_Begin (NULL);
	 HTM_TH_Empty (1);
	 HTM_TH (Txt_Plugin,HTM_HEAD_LEFT);
      HTM_TR_End ();

      /***** Write all plugins *****/
      for (NumPlg = 0;
	   NumPlg < Plugins.Num;
	   NumPlg++)
	{
	 Plg = &Plugins.Lst[NumPlg];

	 snprintf (URL,sizeof (URL),"%s%s",Plg->URL,Gbl.Session.Id);

	 /* Plugin logo */
	 // TODO: Change plugin icons to 32x32
	 HTM_TR_Begin (NULL);

	    HTM_TD_Begin ("class=\"LM DAT_%s\" style=\"width:45px;\"",
			  The_GetSuffix ());
	       HTM_A_Begin ("href=\"%s\" title=\"%s\" target=\"_blank\""
			    " class=\"DAT_%s\"",
			    URL,Plg->Name,The_GetSuffix ());
		  if (asprintf (&Icon,"%s24x24.gif",Plugins.Lst[NumPlg].Logo) < 0)
		     Err_NotEnoughMemoryExit ();
		  HTM_IMG (Cfg_URL_ICON_PLUGINS_PUBLIC,Icon,Plg->Name,
			   "class=\"ICO40x40\"");
		  free (Icon);
	       HTM_A_End ();
	    HTM_TD_End ();

	    HTM_TD_Begin ("class=\"LM DAT_%s\"",The_GetSuffix ());
	       HTM_A_Begin ("href=\"%s\" title=\"%s\" target=\"_blank\""
			    " class=\"DAT_%s\"",
			    URL,Plg->Name,The_GetSuffix ());
		  HTM_Txt (Plg->Name);
	       HTM_A_End ();
	    HTM_TD_End ();

	 HTM_TR_End ();
	}

   /***** End table and box *****/
   Box_BoxTableEnd ();

   /***** Free list of plugins *****/
   Plg_FreeListPlugins (&Plugins);
  }

/*****************************************************************************/
/*************************** Put icon to edit plugins ************************/
/*****************************************************************************/

static void Plg_PutIconToEditPlugins (__attribute__((unused)) void *Args)
  {
   Ico_PutContextualIconToEdit (ActEdiPlg,NULL,NULL,NULL);
  }

/*****************************************************************************/
/*************************** Put icon to view plugins ************************/
/*****************************************************************************/

static void Plg_PutIconToViewPlugins (__attribute__((unused)) void *Args)
  {
   Ico_PutContextualIconToView (ActSeePlg,NULL,NULL,NULL);
  }

/*****************************************************************************/
/************************** Put forms to edit plugins ************************/
/*****************************************************************************/

void Plg_EditPlugins (void)
  {
   /***** Plugin constructor *****/
   Plg_EditingPluginConstructor ();

   /***** Edit plugins *****/
   Plg_EditPluginsInternal ();

   /***** Plugin destructor *****/
   Plg_EditingPluginDestructor ();
  }

static void Plg_EditPluginsInternal (void)
  {
   extern const char *Hlp_SYSTEM_Plugins_edit;
   extern const char *Txt_Plugins;
   struct Plg_Plugins Plugins;

   /***** Get list of plugins *****/
   Plg_GetListPlugins (&Plugins);

   /***** Begin box *****/
   Box_BoxBegin (Txt_Plugins,Plg_PutIconToViewPlugins,NULL,
                 Hlp_SYSTEM_Plugins_edit,Box_NOT_CLOSABLE);

      /***** Put a form to create a new plugin *****/
      Plg_PutFormToCreatePlugin ();

      /***** List current plugins *****/
      if (Plugins.Num)
	 Plg_ListPluginsForEdition (&Plugins);

   /***** End box *****/
   Box_BoxEnd ();

   /***** Free list of plugins *****/
   Plg_FreeListPlugins (&Plugins);
  }

/*****************************************************************************/
/************************* Get list of current plugins ***********************/
/*****************************************************************************/

static void Plg_GetListPlugins (struct Plg_Plugins *Plugins)
  {
   MYSQL_RES *mysql_res;
   unsigned NumPlg;

   /***** Get plugins from database *****/
   if ((Plugins->Num = Plg_DB_GetListPlugins (&mysql_res))) // Plugins found...
     {
      /***** Create list with plugins *****/
      if ((Plugins->Lst = calloc ((size_t) Plugins->Num,
                                  sizeof (struct Plg_Plugin))) == NULL)
         Err_NotEnoughMemoryExit ();

      /***** Get the plugins *****/
      for (NumPlg = 0;
	   NumPlg < Plugins->Num;
	   NumPlg++)
         Plg_GetPluginDataFromRow (mysql_res,&Plugins->Lst[NumPlg]);
     }
   else
      Plugins->Lst = NULL;

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/******************** Get data of a plugin using its code ********************/
/*****************************************************************************/

Exi_Exist_t Plg_GetPluginDataByCod (struct Plg_Plugin *Plg)
  {
   MYSQL_RES *mysql_res;
   Exi_Exist_t PlgExists;

   /***** Clear data *****/
   Plg->Name[0]        =
   Plg->Description[0] =
   Plg->Logo[0]        =
   Plg->URL[0]         =
   Plg->IP[0]          = '\0';

   /***** Check if plugin code is correct *****/
   if (Plg->PlgCod <= 0)
      return Exi_DOES_NOT_EXIST;
   // Plg->PlgCod > 0

   /***** Get data of a plugin from database *****/
   PlgExists = Plg_DB_GetPluginDataByCod (&mysql_res,Plg->PlgCod);
   if (PlgExists == Exi_EXISTS) // Plugin found...
      Plg_GetPluginDataFromRow (mysql_res,Plg);

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return PlgExists;
  }

/*****************************************************************************/
/****************** Get data of a plugin from database row *******************/
/*****************************************************************************/

static void Plg_GetPluginDataFromRow (MYSQL_RES *mysql_res,
                                      struct Plg_Plugin *Plg)
  {
   MYSQL_ROW row;

   /***** Get next row from result *****/
   row = mysql_fetch_row (mysql_res);

   /***** Get plugin code (row[0]) *****/
   if ((Plg->PlgCod = Str_ConvertStrCodToLongCod (row[0])) <= 0)
      Err_WrongPluginExit ();

   /***** Get name (row[1]), description (row[2), logo (row[3]),
          application key (row[4]), URL (row[5])
          and IP (row[6]) of the plugin *****/
   Str_Copy (Plg->Name       ,row[1],sizeof (Plg->Name       ) - 1);
   Str_Copy (Plg->Description,row[2],sizeof (Plg->Description) - 1);
   Str_Copy (Plg->Logo       ,row[3],sizeof (Plg->Logo       ) - 1);
   Str_Copy (Plg->AppKey     ,row[4],sizeof (Plg->AppKey     ) - 1);
   Str_Copy (Plg->URL        ,row[5],sizeof (Plg->URL        ) - 1);
   Str_Copy (Plg->IP         ,row[6],sizeof (Plg->IP         ) - 1);
  }

/*****************************************************************************/
/*************************** Free list of plugins ****************************/
/*****************************************************************************/

static void Plg_FreeListPlugins (struct Plg_Plugins *Plugins)
  {
   if (Plugins->Num && Plugins->Lst)
     {
      free (Plugins->Lst);
      Plugins->Lst = NULL;
      Plugins->Num = 0;
     }
  }

/*****************************************************************************/
/****************************** List all plugins *****************************/
/*****************************************************************************/

static void Plg_ListPluginsForEdition (struct Plg_Plugins *Plugins)
  {
   unsigned NumPlg;
   struct Plg_Plugin *Plg;
   char *Icon;

   /***** Begin table *****/
   HTM_TABLE_Begin ("TBL_SCROLL");

      /***** Write heading *****/
      Plg_PutHeadPlugins ();

      /***** Write all plugins *****/
      for (NumPlg = 0;
	   NumPlg < Plugins->Num;
	   NumPlg++)
	{
	 Plg = &Plugins->Lst[NumPlg];

	 /* Row begin */
	 HTM_TR_Begin (NULL);

	    /* Put icon to remove plugin */
	    HTM_TD_Begin ("class=\"BM\"");
	       Ico_PutContextualIconToRemove (ActRemPlg,NULL,
					      Plg_PutParPlgCod,&Plg->PlgCod);
	    HTM_TD_End ();

	    /* Plugin code */
	    HTM_TD_Begin ("class=\"CODE DAT_%s\"",The_GetSuffix ());
	       HTM_Long (Plg->PlgCod);
	    HTM_TD_End ();

	    /* Plugin logo */
	    // TODO: Change plugin icons to 32x32
	    HTM_TD_Begin ("class=\"CM\" style=\"width:45px;\"");
	       if (asprintf (&Icon,"%s24x24.gif",Plugins->Lst[NumPlg].Logo) < 0)
		  Err_NotEnoughMemoryExit ();
	       HTM_IMG (Cfg_URL_ICON_PLUGINS_PUBLIC,Icon,Plugins->Lst[NumPlg].Name,
			"class=\"ICO40x40\"");
	       free (Icon);
	    HTM_TD_End ();

	    /* Plugin name */
	    HTM_TD_Begin ("class=\"LM\"");
	       Frm_BeginForm (ActRenPlg);
		  ParCod_PutPar (ParCod_Plg,Plg->PlgCod);
		  HTM_INPUT_TEXT ("Name",Plg_MAX_CHARS_PLUGIN_NAME,Plg->Name,
				  HTM_SUBMIT_ON_CHANGE,
				  "size=\"8\" class=\"INPUT_%s\"",
				  The_GetSuffix ());
	       Frm_EndForm ();
	    HTM_TD_End ();

	    /* Plugin description */
	    HTM_TD_Begin ("class=\"LM\"");
	       Frm_BeginForm (ActChgPlgDes);
		  ParCod_PutPar (ParCod_Plg,Plg->PlgCod);
		  HTM_INPUT_TEXT ("Description",Plg_MAX_CHARS_PLUGIN_DESCRIPTION,Plg->Description,
				  HTM_SUBMIT_ON_CHANGE,
				  "size=\"16\" class=\"INPUT_%s\"",
				  The_GetSuffix ());
	       Frm_EndForm ();
	    HTM_TD_End ();

	    /* Plugin logo */
	    HTM_TD_Begin ("class=\"LM\"");
	       Frm_BeginForm (ActChgPlgLog);
		  ParCod_PutPar (ParCod_Plg,Plg->PlgCod);
		  HTM_INPUT_TEXT ("Logo",Plg_MAX_CHARS_PLUGIN_LOGO,Plg->Logo,
				  HTM_SUBMIT_ON_CHANGE,
				  "size=\"4\" class=\"INPUT_%s\"",
				  The_GetSuffix ());
	       Frm_EndForm ();
	    HTM_TD_End ();

	    /* Plugin application key */
	    HTM_TD_Begin ("class=\"LM\"");
	       Frm_BeginForm (ActChgPlgAppKey);
		  ParCod_PutPar (ParCod_Plg,Plg->PlgCod);
		  HTM_INPUT_TEXT ("AppKey",Plg_MAX_CHARS_PLUGIN_APP_KEY,Plg->AppKey,
				  HTM_SUBMIT_ON_CHANGE,
				  "size=\"8\" class=\"INPUT_%s\"",
				  The_GetSuffix ());
	       Frm_EndForm ();
	    HTM_TD_End ();

	    /* Plugin URL */
	    HTM_TD_Begin ("class=\"LM\"");
	       Frm_BeginForm (ActChgPlgURL);
		  ParCod_PutPar (ParCod_Plg,Plg->PlgCod);
		  HTM_INPUT_URL ("URL",Plg->URL,
				 HTM_SUBMIT_ON_CHANGE,
				 "size=\"8\" class=\"INPUT_%s\"",The_GetSuffix ());
	       Frm_EndForm ();
	    HTM_TD_End ();

	    /* Plugin IP address */
	    HTM_TD_Begin ("class=\"LM\"");
	       Frm_BeginForm (ActChgPlgIP);
		  ParCod_PutPar (ParCod_Plg,Plg->PlgCod);
		  HTM_INPUT_TEXT ("IP",Cns_MAX_CHARS_IP,Plg->IP,
				  HTM_SUBMIT_ON_CHANGE,
				  "size=\"8\" class=\"INPUT_%s\"",The_GetSuffix ());
	       Frm_EndForm ();
	    HTM_TD_End ();

	 /* Row end */
	 HTM_TR_End ();
	}

   /***** End table *****/
   HTM_TABLE_End ();
  }

/*****************************************************************************/
/******************* Write parameter with code of plugin *********************/
/*****************************************************************************/

static void Plg_PutParPlgCod (void *PlgCod)
  {
   if (PlgCod)
      ParCod_PutPar (ParCod_Plg,*((long *) PlgCod));
  }

/*****************************************************************************/
/******************************* Remove a plugin *****************************/
/*****************************************************************************/

void Plg_RemovePlugin (void)
  {
   extern const char *Txt_Plugin_X_removed;
   __attribute__((unused)) Exi_Exist_t PlgExists;

   /***** Plugin constructor *****/
   Plg_EditingPluginConstructor ();

   /***** Get plugin code *****/
   Plg_EditingPlg->PlgCod = ParCod_GetAndCheckPar (ParCod_Plg);

   /***** Get data of the plugin from database *****/
   PlgExists = Plg_GetPluginDataByCod (Plg_EditingPlg);

   /***** Remove plugin *****/
   Plg_DB_RemovePlugin (Plg_EditingPlg->PlgCod);

   /***** Write message to show the change made *****/
   Ale_CreateAlert (Ale_SUCCESS,NULL,
	            Txt_Plugin_X_removed,
                    Plg_EditingPlg->Name);
  }

/*****************************************************************************/
/************************ Change the name of a plugin ************************/
/*****************************************************************************/

void Plg_RenamePlugin (void)
  {
   extern const char *Txt_The_plugin_X_already_exists;
   extern const char *Txt_The_plugin_X_has_been_renamed_as_Y;
   extern const char *Txt_The_name_X_has_not_changed;
   char NewPlgName[Plg_MAX_BYTES_PLUGIN_NAME + 1];
   __attribute__((unused)) Exi_Exist_t PlgExists;

   /***** Plugin constructor *****/
   Plg_EditingPluginConstructor ();

   /***** Get parameters from form *****/
   /* Get the code of the plugin */
   Plg_EditingPlg->PlgCod = ParCod_GetAndCheckPar (ParCod_Plg);

   /* Get the new name for the plugin */
   Par_GetParText ("Name",NewPlgName,Plg_MAX_BYTES_PLUGIN_NAME);

   /***** Get plugin data from the database *****/
   PlgExists = Plg_GetPluginDataByCod (Plg_EditingPlg);

   /***** Check if new name is empty *****/
   if (NewPlgName[0])
     {
      /***** Check if old and new names are the same
             (this happens when return is pressed without changes) *****/
      if (strcmp (Plg_EditingPlg->Name,NewPlgName))	// Different names
         /***** If plugin was in database... *****/
         switch (Plg_DB_CheckIfPluginNameExists (NewPlgName,
						 Plg_EditingPlg->PlgCod))
	   {
	    case Exi_EXISTS:
	       Ale_CreateAlert (Ale_WARNING,NULL,
				Txt_The_plugin_X_already_exists,NewPlgName);
	       break;
	    case Exi_DOES_NOT_EXIST:
	    default:
	       /* Update the table changing old name by new name */
	       Plg_DB_ChangeName (Plg_EditingPlg->PlgCod,NewPlgName);

	       /* Write message to show the change made */
	       Ale_CreateAlert (Ale_SUCCESS,NULL,
				Txt_The_plugin_X_has_been_renamed_as_Y,
				Plg_EditingPlg->Name,NewPlgName);
	       break;
	   }
      else	// The same name
         Ale_CreateAlert (Ale_INFO,NULL,
                          Txt_The_name_X_has_not_changed,Plg_EditingPlg->Name);
     }
   else
      Ale_CreateAlertYouCanNotLeaveFieldEmpty ();

   /***** Update name *****/
   Str_Copy (Plg_EditingPlg->Name,NewPlgName,sizeof (Plg_EditingPlg->Name) - 1);
  }

/*****************************************************************************/
/******************* Change the description of a plugin **********************/
/*****************************************************************************/

void Plg_ChangePlgDesc (void)
  {
   extern const char *Txt_The_new_description_is_X;
   char NewDescription[Plg_MAX_BYTES_PLUGIN_DESCRIPTION + 1];
   __attribute__((unused)) Exi_Exist_t PlgExists;

   /***** Plugin constructor *****/
   Plg_EditingPluginConstructor ();

   /***** Get parameters from form *****/
   /* Get the code of the plugin */
   Plg_EditingPlg->PlgCod = ParCod_GetAndCheckPar (ParCod_Plg);

   /* Get the new description for the plugin */
   Par_GetParText ("Description",NewDescription,Plg_MAX_BYTES_PLUGIN_DESCRIPTION);

   /***** Get plugin data from the database *****/
   PlgExists = Plg_GetPluginDataByCod (Plg_EditingPlg);

   /***** Check if new description is empty *****/
   if (NewDescription[0])
     {
      /* Update the table changing old description by new description */
      Plg_DB_ChangeDescription (Plg_EditingPlg->PlgCod,NewDescription);

      /* Write message to show the change made */
      Ale_CreateAlert (Ale_SUCCESS,NULL,
	               Txt_The_new_description_is_X,
                       NewDescription);
     }
   else
      Ale_CreateAlertYouCanNotLeaveFieldEmpty ();

   /***** Update description *****/
   Str_Copy (Plg_EditingPlg->Description,NewDescription,
             sizeof (Plg_EditingPlg->Description) - 1);
  }

/*****************************************************************************/
/************************ Change the logo of a plugin ************************/
/*****************************************************************************/

void Plg_ChangePlgLogo (void)
  {
   extern const char *Txt_The_new_logo_is_X;
   char NewLogo[Plg_MAX_BYTES_PLUGIN_LOGO + 1];
   __attribute__((unused)) Exi_Exist_t PlgExists;

   /***** Plugin constructor *****/
   Plg_EditingPluginConstructor ();

   /***** Get parameters from form *****/
   /* Get the code of the plugin */
   Plg_EditingPlg->PlgCod = ParCod_GetAndCheckPar (ParCod_Plg);

   /* Get the new logo for the plugin */
   Par_GetParText ("Logo",NewLogo,Plg_MAX_BYTES_PLUGIN_LOGO);

   /***** Get plugin data from the database *****/
   PlgExists = Plg_GetPluginDataByCod (Plg_EditingPlg);

   /***** Check if new logo is empty *****/
   if (NewLogo[0])
     {
      /* Update the table changing old logo by new logo */
      Plg_DB_ChangeLogo (Plg_EditingPlg->PlgCod,NewLogo);

      /* Write message to show the change made */
      Ale_CreateAlert (Ale_SUCCESS,NULL,
	               Txt_The_new_logo_is_X,
                       NewLogo);
     }
   else
      Ale_CreateAlertYouCanNotLeaveFieldEmpty ();

   /***** Update logo *****/
   Str_Copy (Plg_EditingPlg->Logo,NewLogo,sizeof (Plg_EditingPlg->Logo) - 1);
  }

/*****************************************************************************/
/****************** Change the application key of a plugin *******************/
/*****************************************************************************/

void Plg_ChangePlgAppKey (void)
  {
   extern const char *Txt_The_new_application_key_is_X;
   char NewAppKey[Plg_MAX_BYTES_PLUGIN_APP_KEY + 1];
   __attribute__((unused)) Exi_Exist_t PlgExists;

   /***** Plugin constructor *****/
   Plg_EditingPluginConstructor ();

   /***** Get parameters from form *****/
   /* Get the code of the plugin */
   Plg_EditingPlg->PlgCod = ParCod_GetAndCheckPar (ParCod_Plg);

   /* Get the new logo for the plugin */
   Par_GetParText ("AppKey",NewAppKey,Plg_MAX_BYTES_PLUGIN_APP_KEY);

   /***** Get plugin data from the database *****/
   PlgExists = Plg_GetPluginDataByCod (Plg_EditingPlg);

   /***** Check if new logo is empty *****/
   if (NewAppKey[0])
     {
      /* Update the table changing old application key by new application key */
      Plg_DB_ChangeAppKey (Plg_EditingPlg->PlgCod,NewAppKey);

      /* Write message to show the change made */
      Ale_CreateAlert (Ale_SUCCESS,NULL,
		       Txt_The_new_application_key_is_X,
                       NewAppKey);
     }
   else
      Ale_CreateAlertYouCanNotLeaveFieldEmpty ();

   /***** Update app key *****/
   Str_Copy (Plg_EditingPlg->AppKey,NewAppKey,sizeof (Plg_EditingPlg->AppKey) - 1);
  }

/*****************************************************************************/
/************************ Change the URL of a plugin *************************/
/*****************************************************************************/

void Plg_ChangePlgURL (void)
  {
   extern const char *Txt_The_new_URL_is_X;
   char NewURL[WWW_MAX_BYTES_WWW + 1];
   __attribute__((unused)) Exi_Exist_t PlgExists;

   /***** Plugin constructor *****/
   Plg_EditingPluginConstructor ();

   /***** Get parameters from form *****/
   /* Get the code of the plugin */
   Plg_EditingPlg->PlgCod = ParCod_GetAndCheckPar (ParCod_Plg);

   /* Get the new URL for the plugin */
   Par_GetParText ("URL",NewURL,WWW_MAX_BYTES_WWW);

   /***** Get plugin data from the database *****/
   PlgExists = Plg_GetPluginDataByCod (Plg_EditingPlg);

   /***** Check if new URL is empty *****/
   if (NewURL[0])
     {
      /* Update the table changing old WWW by new WWW */
      Plg_DB_ChangeURL (Plg_EditingPlg->PlgCod,NewURL);

      /* Write message to show the change made */
      Ale_CreateAlert (Ale_SUCCESS,NULL,
	               Txt_The_new_URL_is_X,
                       NewURL);
     }
   else
      Ale_CreateAlertYouCanNotLeaveFieldEmpty ();

   /***** Update URL *****/
   Str_Copy (Plg_EditingPlg->URL,NewURL,sizeof (Plg_EditingPlg->URL) - 1);
  }

/*****************************************************************************/
/************************* Change the IP of a plugin *************************/
/*****************************************************************************/

void Plg_ChangePlgIP (void)
  {
   extern const char *Txt_The_new_IP_address_is_X;
   char NewIP[Cns_MAX_BYTES_IP + 1];
   __attribute__((unused)) Exi_Exist_t PlgExists;

   /***** Plugin constructor *****/
   Plg_EditingPluginConstructor ();

   /***** Get parameters from form *****/
   /* Get the code of the plugin */
   Plg_EditingPlg->PlgCod = ParCod_GetAndCheckPar (ParCod_Plg);

   /* Get the new IP for the plugin */
   Par_GetParText ("IP",NewIP,Cns_MAX_BYTES_IP);

   /***** Get plugin data from the database *****/
   PlgExists = Plg_GetPluginDataByCod (Plg_EditingPlg);

   /***** Check if new IP is empty *****/
   if (NewIP[0])
     {
      /* Update the table changing old IP by new IP */
      Plg_DB_ChangeIP (Plg_EditingPlg->PlgCod,NewIP);

      /* Write message to show the change made */
      Ale_CreateAlert (Ale_SUCCESS,NULL,
	               Txt_The_new_IP_address_is_X,
                       NewIP);
     }
   else
      Ale_CreateAlertYouCanNotLeaveFieldEmpty ();

   /***** Update IP *****/
   Str_Copy (Plg_EditingPlg->IP,NewIP,sizeof (Plg_EditingPlg->IP) - 1);
  }

/*****************************************************************************/
/********* Show alerts after changing a plugin and continue editing **********/
/*****************************************************************************/

void Plg_ContEditAfterChgPlg (void)
  {
   /***** Write message to show the change made *****/
   Ale_ShowAlerts (NULL);

   /***** Show the form again *****/
   Plg_EditPluginsInternal ();

   /***** Plugin destructor *****/
   Plg_EditingPluginDestructor ();
  }

/*****************************************************************************/
/********************* Put a form to create a new plugin *********************/
/*****************************************************************************/

static void Plg_PutFormToCreatePlugin (void)
  {
   /***** Begin form to create *****/
   Frm_BeginFormTable (ActNewPlg,NULL,NULL,NULL,"TBL_SCROLL");

      /***** Write heading *****/
      Plg_PutHeadPlugins ();

      /***** Row begin *****/
      HTM_TR_Begin (NULL);

	 /***** Column to remove plugin, disabled here *****/
	 HTM_TD_Begin ("class=\"BM\"");
	 HTM_TD_End ();

	 /***** Plugin code */
	 HTM_TD_Begin ("class=\"CODE\"");
	 HTM_TD_End ();

	 /***** Plugin logo *****/
	 // TODO: Change plugin icons to 32x32
	 HTM_TD_Begin ("style=\"width:45px;\"");
	 HTM_TD_End ();

	 /***** Plugin name *****/
	 HTM_TD_Begin ("class=\"LM\"");
	    HTM_INPUT_TEXT ("Name",Plg_MAX_CHARS_PLUGIN_NAME,Plg_EditingPlg->Name,
			    HTM_REQUIRED,
			    "size=\"8\" class=\"INPUT_%s\"",The_GetSuffix ());
	 HTM_TD_End ();

	 /***** Plugin description *****/
	 HTM_TD_Begin ("class=\"LM\"");
	    HTM_INPUT_TEXT ("Description",Plg_MAX_CHARS_PLUGIN_DESCRIPTION,
			    Plg_EditingPlg->Description,
			    HTM_REQUIRED,
			    "size=\"16\" class=\"INPUT_%s\"",The_GetSuffix ());
	 HTM_TD_End ();

	 /***** Plugin logo *****/
	 HTM_TD_Begin ("class=\"LM\"");
	    HTM_INPUT_TEXT ("Logo",Plg_MAX_CHARS_PLUGIN_LOGO,Plg_EditingPlg->Logo,
			    HTM_REQUIRED,
			    "size=\"4\" class=\"INPUT_%s\"",The_GetSuffix ());
	 HTM_TD_End ();

	 /***** Plugin application key *****/
	 HTM_TD_Begin ("class=\"LM\"");
	    HTM_INPUT_TEXT ("AppKey",Plg_MAX_CHARS_PLUGIN_APP_KEY,Plg_EditingPlg->AppKey,
			    HTM_REQUIRED,
			    "size=\"8\" class=\"INPUT_%s\"",The_GetSuffix ());
	 HTM_TD_End ();

	 /***** Plugin URL *****/
	 HTM_TD_Begin ("class=\"LM\"");
	    HTM_INPUT_URL ("URL",Plg_EditingPlg->URL,
			   HTM_REQUIRED,
			   "size=\"8\" class=\"INPUT_%s\"",The_GetSuffix ());
	 HTM_TD_End ();

	 /***** Plugin IP address *****/
	 HTM_TD_Begin ("class=\"LM\"");
	    HTM_INPUT_TEXT ("IP",Cns_MAX_CHARS_IP,Plg_EditingPlg->IP,
			    HTM_REQUIRED,
			    "size=\"8\" class=\"INPUT_%s\"",The_GetSuffix ());
	 HTM_TD_End ();

      /***** Row end *****/
      HTM_TR_End ();

   /***** End form to create *****/
   Frm_EndFormTable (Btn_CREATE);
  }

/*****************************************************************************/
/******************** Write header with fields of a plugin *******************/
/*****************************************************************************/

static void Plg_PutHeadPlugins (void)
  {
   extern const char *Txt_Code;
   extern const char *Txt_Name;
   extern const char *Txt_Description;
   extern const char *Txt_Logo;
   extern const char *Txt_Application_key;
   extern const char *Txt_URL;
   extern const char *Txt_IP;

   HTM_TR_Begin (NULL);
      HTM_TH_Empty (1);
      HTM_TH (Txt_Code           ,HTM_HEAD_RIGHT);
      HTM_TH_Empty (1);
      HTM_TH (Txt_Name           ,HTM_HEAD_LEFT );
      HTM_TH (Txt_Description    ,HTM_HEAD_LEFT );
      HTM_TH (Txt_Logo           ,HTM_HEAD_LEFT );
      HTM_TH (Txt_Application_key,HTM_HEAD_LEFT );
      HTM_TH (Txt_URL            ,HTM_HEAD_LEFT );
      HTM_TH (Txt_IP             ,HTM_HEAD_LEFT );
   HTM_TR_End ();
  }

/*****************************************************************************/
/****************** Receive form to create a new plugin **********************/
/*****************************************************************************/

void Plg_ReceiveNewPlg (void)
  {
   extern const char *Txt_The_plugin_X_already_exists;
   extern const char *Txt_Created_new_plugin_X;
   extern const char *Txt_You_must_specify_the_logo_the_application_key_the_URL_and_the_IP_address_of_the_new_plugin;
   extern const char *Txt_You_must_specify_the_name;

   /***** Plugin constructor *****/
   Plg_EditingPluginConstructor ();

   /***** Get parameters from form *****/
   /* Get plugin name, description, logo, application key, URL, IP address */
   Par_GetParText ("Name"       ,Plg_EditingPlg->Name       ,Plg_MAX_BYTES_PLUGIN_NAME);
   Par_GetParText ("Description",Plg_EditingPlg->Description,Plg_MAX_BYTES_PLUGIN_DESCRIPTION);
   Par_GetParText ("Logo"       ,Plg_EditingPlg->Logo       ,Plg_MAX_BYTES_PLUGIN_LOGO);
   Par_GetParText ("AppKey"     ,Plg_EditingPlg->AppKey     ,Plg_MAX_BYTES_PLUGIN_APP_KEY);
   Par_GetParText ("URL"        ,Plg_EditingPlg->URL        ,WWW_MAX_BYTES_WWW);
   Par_GetParText ("IP"         ,Plg_EditingPlg->IP         ,Cns_MAX_BYTES_IP);

   if (Plg_EditingPlg->Name[0])	// If there's a plugin name
     {
      if (Plg_EditingPlg->Logo[0]   &&
	  Plg_EditingPlg->AppKey[0] &&
	  Plg_EditingPlg->URL[0]    &&
	  Plg_EditingPlg->IP[0])
         /***** If name of plugin was in database... *****/
         switch (Plg_DB_CheckIfPluginNameExists (Plg_EditingPlg->Name,-1L))
	   {
	    case Exi_EXISTS:
	       Ale_CreateAlert (Ale_WARNING,NULL,Txt_The_plugin_X_already_exists,
				Plg_EditingPlg->Name);
	       break;
	    case Exi_DOES_NOT_EXIST:
	    default:
	       /* Add new plugin to database */
	       Plg_DB_CreatePlugin (Plg_EditingPlg);
	       Ale_CreateAlert (Ale_SUCCESS,NULL,Txt_Created_new_plugin_X,
				Plg_EditingPlg->Name);
	       break;
	   }
      else	// If there is not a logo, a URL or a IP
         Ale_CreateAlert (Ale_WARNING,NULL,
                          Txt_You_must_specify_the_logo_the_application_key_the_URL_and_the_IP_address_of_the_new_plugin);
     }
   else	// If there is not a plugin name
      Ale_CreateAlert (Ale_WARNING,NULL,
	               Txt_You_must_specify_the_name);
  }

/*****************************************************************************/
/************************ Plugin constructor/destructor **********************/
/*****************************************************************************/

static void Plg_EditingPluginConstructor (void)
  {
   /***** Allocate memory for plugin *****/
   if ((Plg_EditingPlg = malloc (sizeof (struct Plg_Plugin))) == NULL)
      Err_NotEnoughMemoryExit ();

   /***** Reset plugin *****/
   Plg_EditingPlg->PlgCod         = -1L;
   Plg_EditingPlg->Name[0]        = '\0';
   Plg_EditingPlg->Description[0] = '\0';
   Plg_EditingPlg->Logo[0]        = '\0';
   Plg_EditingPlg->AppKey[0]      = '\0';
   Plg_EditingPlg->URL[0]         = '\0';
   Plg_EditingPlg->IP[0]          = '\0';
  }

static void Plg_EditingPluginDestructor (void)
  {
   /***** Free memory used for plugin *****/
   if (Plg_EditingPlg != NULL)
     {
      free (Plg_EditingPlg);
      Plg_EditingPlg = NULL;
     }
  }
