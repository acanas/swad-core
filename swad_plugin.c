// swad_plugin.c: plugins called from SWAD using web services

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
/*
TODO: Check if web service is called from an authorized IP.
*/
/*****************************************************************************/
/********************************* Headers ***********************************/
/*****************************************************************************/

#include <stdbool.h>		// For boolean type
#include <stddef.h>		// For NULL
#include <stdlib.h>		// For calloc, free
#include <string.h>

#include "swad_API.h"
#include "swad_box.h"
#include "swad_config.h"
#include "swad_constant.h"
#include "swad_cryptography.h"
#include "swad_database.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_HTML.h"
#include "swad_language.h"
#include "swad_parameter.h"
#include "swad_plugin.h"
#include "swad_session.h"

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

static struct Plugin *Plg_EditingPlg = NULL;	// Static variable to keep the plugin being edited

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Plg_PutIconToEditPlugins (__attribute__((unused)) void *Args);
static void Plg_EditPluginsInternal (void);
static void Plg_ListPluginsForEdition (void);
static void Plg_PutParamPlgCod (void *PlgCod);
static void Plg_GetListPlugins (void);
static void Plg_PutFormToCreatePlugin (void);
static void Plg_PutHeadPlugins (void);
static bool Plg_CheckIfPluginNameExists (const char *Name,long PlgCod);
static void Plg_CreatePlugin (struct Plugin *Plg);

static void Plg_EditingPluginConstructor (void);
static void Plg_EditingPluginDestructor (void);

/*****************************************************************************/
/************************** List available plugins ***************************/
/*****************************************************************************/

void Plg_ListPlugins (void)
  {
   extern const char *Txt_Option_under_development;
   extern const char *Txt_Plugins;
   extern const char *Txt_Plugin;
   unsigned NumPlg;
   struct Plugin *Plg;
   char URL[Cns_MAX_BYTES_WWW + Cns_BYTES_SESSION_ID + 1];

   if (Gbl.Usrs.Me.Role.Logged != Rol_SYS_ADM)
     {
      Ale_ShowAlert (Ale_WARNING,Txt_Option_under_development);
      return;
     }

   /***** Get list of plugins *****/
   Plg_GetListPlugins ();

   /***** Begin box and table *****/
   if (Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM)
      Box_BoxTableBegin (NULL,Txt_Plugins,
			 Plg_PutIconToEditPlugins,NULL,
			 NULL,Box_NOT_CLOSABLE,2);
   else
      Box_BoxTableBegin (NULL,Txt_Plugins,
			 NULL,NULL,
			 NULL,Box_NOT_CLOSABLE,2);

   /***** Write table heading *****/
   HTM_TR_Begin (NULL);

   HTM_TH_Empty (1);
   HTM_TH (1,1,"LM",Txt_Plugin);

   HTM_TR_End ();

   /***** Write all the plugins *****/
   for (NumPlg = 0;
	NumPlg < Gbl.Plugins.Num;
	NumPlg++)
     {
      Plg = &(Gbl.Plugins.Lst[NumPlg]);

      snprintf (URL,sizeof (URL),
	        "%s%s",
		Plg->URL,Gbl.Session.Id);

      /* Plugin logo */
      // TODO: Change plugin icons to 32x32
      HTM_TR_Begin (NULL);

      HTM_TD_Begin ("class=\"DAT LM\" style=\"width:45px;\"");
      HTM_A_Begin ("href=\"%s\" title=\"%s\" class=\"DAT\" target=\"_blank\"",
                   URL,Plg->Name);
      HTM_IMG (Cfg_URL_ICON_PLUGINS_PUBLIC,
	       Str_BuildStringStr ("%s24x24.gif",Gbl.Plugins.Lst[NumPlg].Logo),
	       Plg->Name,
	       "class=\"ICO40x40\"");
      Str_FreeString ();
      HTM_A_End ();
      HTM_TD_End ();

      HTM_TD_Begin ("class=\"DAT LM\"");
      HTM_A_Begin ("href=\"%s\" title=\"%s\" class=\"DAT\" target=\"_blank\"",
                   URL,Plg->Name);
      HTM_Txt (Plg->Name);
      HTM_A_End ();
      HTM_TD_End ();

      HTM_TR_End ();
     }

   /***** End table and box *****/
   Box_BoxTableEnd ();

   /***** Free list of plugins *****/
   Plg_FreeListPlugins ();
  }

/*****************************************************************************/
/*************************** Put icon to edit plugins ************************/
/*****************************************************************************/

static void Plg_PutIconToEditPlugins (__attribute__((unused)) void *Args)
  {
   Ico_PutContextualIconToEdit (ActEdiPlg,NULL,
				NULL,NULL);
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
   extern const char *Txt_Plugins;

   /***** Get list of plugins *****/
   Plg_GetListPlugins ();

   /***** Begin box *****/
   Box_BoxBegin (NULL,Txt_Plugins,
                 NULL,NULL,
                 NULL,Box_NOT_CLOSABLE);

   /***** Put a form to create a new plugin *****/
   Plg_PutFormToCreatePlugin ();

   /***** List current plugins *****/
   if (Gbl.Plugins.Num)
      Plg_ListPluginsForEdition ();

   /***** End box *****/
   Box_BoxEnd ();

   /***** Free list of plugins *****/
   Plg_FreeListPlugins ();
  }

/*****************************************************************************/
/************************* Get list of current plugins ***********************/
/*****************************************************************************/

static void Plg_GetListPlugins (void)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;
   unsigned NumPlg;
   struct Plugin *Plg;

   /***** Get plugins from database *****/
   NumRows = DB_QuerySELECT (&mysql_res,"can not get plugins",
			     "SELECT PlgCod,Name,Description,Logo,AppKey,URL,IP"
			     " FROM plugins ORDER BY Name");

   /***** Count number of rows in result *****/
   if (NumRows) // Plugins found...
     {
      Gbl.Plugins.Num = (unsigned) NumRows;

      /***** Create list with plugins *****/
      if ((Gbl.Plugins.Lst = (struct Plugin *) calloc ((size_t) Gbl.Plugins.Num,sizeof (struct Plugin))) == NULL)
         Lay_NotEnoughMemoryExit ();

      /***** Get the plugins *****/
      for (NumPlg = 0;
	   NumPlg < Gbl.Plugins.Num;
	   NumPlg++)
        {
         Plg = &(Gbl.Plugins.Lst[NumPlg]);

         /* Get next plugin */
         row = mysql_fetch_row (mysql_res);

         /* Get plugin code (row[0]) */
         if ((Plg->PlgCod = Str_ConvertStrCodToLongCod (row[0])) < 0)
            Lay_ShowErrorAndExit ("Wrong code of plugin.");

         /* Get the name of the plugin (row[1]) */
         Str_Copy (Plg->Name,row[1],
                   Plg_MAX_BYTES_PLUGIN_NAME);

         /* Get the description of the plugin (row[2]) */
         Str_Copy (Plg->Description,row[2],
                   Plg_MAX_BYTES_PLUGIN_DESCRIPTION);

         /* Get the logo of the plugin (row[3]) */
         Str_Copy (Plg->Logo,row[3],
                   Plg_MAX_BYTES_PLUGIN_LOGO);

         /* Get the application key of the plugin (row[4]) */
         Str_Copy (Plg->AppKey,row[4],
                   Plg_MAX_BYTES_PLUGIN_APP_KEY);

         /* Get the URL of the plugin (row[5]) */
         Str_Copy (Plg->URL,row[5],
                   Cns_MAX_BYTES_WWW);

         /* Get the IP of the plugin (row[6]) */
         Str_Copy (Plg->IP,row[6],
                   Cns_MAX_BYTES_IP);
        }
     }
   else
      Gbl.Plugins.Num = 0;

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/*************************** Get data of a plugin ****************************/
/*****************************************************************************/

bool Plg_GetDataOfPluginByCod (struct Plugin *Plg)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;
   bool PluginFound;

   /***** Clear data *****/
   Plg->Name[0]        =
   Plg->Description[0] =
   Plg->Logo[0]        =
   Plg->URL[0]         =
   Plg->IP[0]          = '\0';

   /***** Check if plugin code is correct *****/
   if (Plg->PlgCod <= 0)
      return false;
   // Plg->PlgCod > 0

   /***** Get data of a plugin from database *****/
   NumRows = DB_QuerySELECT (&mysql_res,"can not get data of a plugin",
			     "SELECT Name,Description,Logo,AppKey,URL,IP"
			     " FROM plugins"
			     " WHERE PlgCod=%ld",
			     Plg->PlgCod);

   /***** Count number of rows in result *****/
   if (NumRows) // Plugin found...
     {
      PluginFound = true;

      /* Get row */
      row = mysql_fetch_row (mysql_res);

      /* Get the name of the plugin (row[0]) */
      Str_Copy (Plg->Name,row[0],
                Plg_MAX_BYTES_PLUGIN_NAME);

      /* Get the description of the plugin (row[1]) */
      Str_Copy (Plg->Description,row[1],
                Plg_MAX_BYTES_PLUGIN_DESCRIPTION);

      /* Get the logo of the plugin (row[2]) */
      Str_Copy (Plg->Logo,row[2],
                Plg_MAX_BYTES_PLUGIN_LOGO);

      /* Get the application key of the plugin (row[3]) */
      Str_Copy (Plg->AppKey,row[3],
                Plg_MAX_BYTES_PLUGIN_APP_KEY);

      /* Get the URL of the plugin (row[4]) */
      Str_Copy (Plg->URL,row[4],
                Cns_MAX_BYTES_WWW);

      /* Get the IP of the plugin (row[5]) */
      Str_Copy (Plg->IP,row[5],
                Cns_MAX_BYTES_IP);
     }
   else
      PluginFound = false;

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return PluginFound;
  }

/*****************************************************************************/
/*************************** Free list of plugins ****************************/
/*****************************************************************************/

void Plg_FreeListPlugins (void)
  {
   if (Gbl.Plugins.Lst)
     {
      /***** Free memory used by the list of plugins *****/
      free (Gbl.Plugins.Lst);
      Gbl.Plugins.Lst = NULL;
      Gbl.Plugins.Num = 0;
     }
  }

/*****************************************************************************/
/**************************** List all the plugins ***************************/
/*****************************************************************************/

static void Plg_ListPluginsForEdition (void)
  {
   unsigned NumPlg;
   struct Plugin *Plg;

   /***** Write heading *****/
   HTM_TABLE_BeginWidePadding (2);
   Plg_PutHeadPlugins ();

   /***** Write all the plugins *****/
   for (NumPlg = 0;
	NumPlg < Gbl.Plugins.Num;
	NumPlg++)
     {
      Plg = &Gbl.Plugins.Lst[NumPlg];

      /* Row begin */
      HTM_TR_Begin (NULL);

      /* Put icon to remove plugin */
      HTM_TD_Begin ("class=\"BM\"");
      Ico_PutContextualIconToRemove (ActRemPlg,NULL,
				     Plg_PutParamPlgCod,&Plg->PlgCod);
      HTM_TD_End ();

      /* Plugin code */
      HTM_TD_Begin ("class=\"DAT RM\"");
      HTM_Long (Plg->PlgCod);
      HTM_TD_End ();

      /* Plugin logo */
      // TODO: Change plugin icons to 32x32
      HTM_TD_Begin ("class=\"CM\" style=\"width:45px;\"");
      HTM_IMG (Cfg_URL_ICON_PLUGINS_PUBLIC,
	       Str_BuildStringStr ("%s24x24.gif",Gbl.Plugins.Lst[NumPlg].Logo),
	       Gbl.Plugins.Lst[NumPlg].Name,
	       "class=\"ICO40x40\"");
      Str_FreeString ();
      HTM_TD_End ();

      /* Plugin name */
      HTM_TD_Begin ("class=\"CM\"");
      Frm_StartForm (ActRenPlg);
      Plg_PutParamPlgCod (&Plg->PlgCod);
      HTM_INPUT_TEXT ("Name",Plg_MAX_CHARS_PLUGIN_NAME,Plg->Name,
                      HTM_SUBMIT_ON_CHANGE,
		      "size=\"10\"");
      Frm_EndForm ();
      HTM_TD_End ();

      /* Plugin description */
      HTM_TD_Begin ("class=\"CM\"");
      Frm_StartForm (ActChgPlgDes);
      Plg_PutParamPlgCod (&Plg->PlgCod);
      HTM_INPUT_TEXT ("Description",Plg_MAX_CHARS_PLUGIN_DESCRIPTION,Plg->Description,
                      HTM_SUBMIT_ON_CHANGE,
		      "size=\"30\"");
      Frm_EndForm ();
      HTM_TD_End ();

      /* Plugin logo */
      HTM_TD_Begin ("class=\"CM\"");
      Frm_StartForm (ActChgPlgLog);
      Plg_PutParamPlgCod (&Plg->PlgCod);
      HTM_INPUT_TEXT ("Logo",Plg_MAX_CHARS_PLUGIN_LOGO,Plg->Logo,
                      HTM_SUBMIT_ON_CHANGE,
		      "size=\"4\"");
      Frm_EndForm ();
      HTM_TD_End ();

      /* Plugin application key */
      HTM_TD_Begin ("class=\"CM\"");
      Frm_StartForm (ActChgPlgAppKey);
      Plg_PutParamPlgCod (&Plg->PlgCod);
      HTM_INPUT_TEXT ("AppKey",Plg_MAX_CHARS_PLUGIN_APP_KEY,Plg->AppKey,
                      HTM_SUBMIT_ON_CHANGE,
		      "size=\"16\"");
      Frm_EndForm ();
      HTM_TD_End ();

      /* Plugin URL */
      HTM_TD_Begin ("class=\"CM\"");
      Frm_StartForm (ActChgPlgURL);
      Plg_PutParamPlgCod (&Plg->PlgCod);
      HTM_INPUT_URL ("URL",Plg->URL,HTM_SUBMIT_ON_CHANGE,
		     "size=\"15\"");
      Frm_EndForm ();
      HTM_TD_End ();

      /* Plugin IP */
      HTM_TD_Begin ("class=\"CM\"");
      Frm_StartForm (ActChgPlgIP);
      Plg_PutParamPlgCod (&Plg->PlgCod);
      HTM_INPUT_TEXT ("IP",Cns_MAX_CHARS_IP,Plg->IP,HTM_SUBMIT_ON_CHANGE,
		      "size=\"10\"");
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

static void Plg_PutParamPlgCod (void *PlgCod)
  {
   if (PlgCod)
      Par_PutHiddenParamLong (NULL,"PlgCod",*((long *) PlgCod));
  }

/*****************************************************************************/
/********************* Get parameter with code of plugin *********************/
/*****************************************************************************/

long Plg_GetParamPlgCod (void)
  {
   /***** Get code of plugin *****/
   return Par_GetParToLong ("PlgCod");
  }

/*****************************************************************************/
/******************************* Remove a plugin *****************************/
/*****************************************************************************/

void Plg_RemovePlugin (void)
  {
   extern const char *Txt_Plugin_X_removed;

   /***** Plugin constructor *****/
   Plg_EditingPluginConstructor ();

   /***** Get plugin code *****/
   if ((Plg_EditingPlg->PlgCod = Plg_GetParamPlgCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of plugin is missing.");

   /***** Get data of the plugin from database *****/
   Plg_GetDataOfPluginByCod (Plg_EditingPlg);

   /***** Remove plugin *****/
   DB_QueryDELETE ("can not remove a plugin",
		   "DELETE FROM plugins WHERE PlgCod=%ld",
		   Plg_EditingPlg->PlgCod);

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
   extern const char *Txt_The_name_of_the_plugin_X_has_not_changed;
   char NewPlgName[Plg_MAX_BYTES_PLUGIN_NAME + 1];

   /***** Plugin constructor *****/
   Plg_EditingPluginConstructor ();

   /***** Get parameters from form *****/
   /* Get the code of the plugin */
   if ((Plg_EditingPlg->PlgCod = Plg_GetParamPlgCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of plugin is missing.");

   /* Get the new name for the plugin */
   Par_GetParToText ("Name",NewPlgName,Plg_MAX_BYTES_PLUGIN_NAME);

   /***** Get plugin data from the database *****/
   Plg_GetDataOfPluginByCod (Plg_EditingPlg);

   /***** Check if new name is empty *****/
   if (NewPlgName[0])
     {
      /***** Check if old and new names are the same
             (this happens when return is pressed without changes) *****/
      if (strcmp (Plg_EditingPlg->Name,NewPlgName))	// Different names
        {
         /***** If plugin was in database... *****/
         if (Plg_CheckIfPluginNameExists (NewPlgName,Plg_EditingPlg->PlgCod))
            Ale_CreateAlert (Ale_WARNING,NULL,
        	             Txt_The_plugin_X_already_exists,
                             NewPlgName);
         else
           {
            /* Update the table changing old name by new name */
            DB_QueryUPDATE ("can not update the name of a plugin",
        		    "UPDATE plugins SET Name='%s' WHERE PlgCod=%ld",
                            NewPlgName,Plg_EditingPlg->PlgCod);

            /***** Write message to show the change made *****/
            Ale_CreateAlert (Ale_SUCCESS,NULL,
        	             Txt_The_plugin_X_has_been_renamed_as_Y,
                             Plg_EditingPlg->Name,NewPlgName);
           }
        }
      else	// The same name
         Ale_CreateAlert (Ale_INFO,NULL,
                          Txt_The_name_of_the_plugin_X_has_not_changed,
                          Plg_EditingPlg->Name);
     }
   else
      Ale_CreateAlertYouCanNotLeaveFieldEmpty ();

   /***** Update name *****/
   Str_Copy (Plg_EditingPlg->Name,NewPlgName,
             Plg_MAX_BYTES_PLUGIN_NAME);
  }

/*****************************************************************************/
/******************** Check if the name of plugin exists *********************/
/*****************************************************************************/

static bool Plg_CheckIfPluginNameExists (const char *Name,long PlgCod)
  {
   /***** Get number of plugins with a name from database *****/
   return (DB_QueryCOUNT ("can not check if the name of a plugin"
			  " already existed",
			  "SELECT COUNT(*) FROM plugins"
			  " WHERE Name='%s' AND PlgCod<>%ld",
			  Name,PlgCod) != 0);
  }

/*****************************************************************************/
/******************* Change the description of a plugin **********************/
/*****************************************************************************/

void Plg_ChangePlgDescription (void)
  {
   extern const char *Txt_The_new_description_is_X;
   char NewDescription[Plg_MAX_BYTES_PLUGIN_DESCRIPTION + 1];

   /***** Plugin constructor *****/
   Plg_EditingPluginConstructor ();

   /***** Get parameters from form *****/
   /* Get the code of the plugin */
   if ((Plg_EditingPlg->PlgCod = Plg_GetParamPlgCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of plugin is missing.");

   /* Get the new description for the plugin */
   Par_GetParToText ("Description",NewDescription,Plg_MAX_BYTES_PLUGIN_DESCRIPTION);

   /***** Get plugin data from the database *****/
   Plg_GetDataOfPluginByCod (Plg_EditingPlg);

   /***** Check if new description is empty *****/
   if (NewDescription[0])
     {
      /* Update the table changing old description by new description */
      DB_QueryUPDATE ("can not update the description of a plugin",
		      "UPDATE plugins SET Description='%s' WHERE PlgCod=%ld",
                      NewDescription,Plg_EditingPlg->PlgCod);

      /***** Write message to show the change made *****/
      Ale_CreateAlert (Ale_SUCCESS,NULL,
	               Txt_The_new_description_is_X,
                       NewDescription);
     }
   else
      Ale_CreateAlertYouCanNotLeaveFieldEmpty ();

   /***** Update description *****/
   Str_Copy (Plg_EditingPlg->Description,NewDescription,
             Plg_MAX_BYTES_PLUGIN_DESCRIPTION);
  }

/*****************************************************************************/
/************************ Change the logo of a plugin ************************/
/*****************************************************************************/

void Plg_ChangePlgLogo (void)
  {
   extern const char *Txt_The_new_logo_is_X;
   char NewLogo[Plg_MAX_BYTES_PLUGIN_LOGO + 1];

   /***** Plugin constructor *****/
   Plg_EditingPluginConstructor ();

   /***** Get parameters from form *****/
   /* Get the code of the plugin */
   if ((Plg_EditingPlg->PlgCod = Plg_GetParamPlgCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of plugin is missing.");

   /* Get the new logo for the plugin */
   Par_GetParToText ("Logo",NewLogo,Plg_MAX_BYTES_PLUGIN_LOGO);

   /***** Get plugin data from the database *****/
   Plg_GetDataOfPluginByCod (Plg_EditingPlg);

   /***** Check if new logo is empty *****/
   if (NewLogo[0])
     {
      /* Update the table changing old logo by new logo */
      DB_QueryUPDATE ("can not update the logo of a plugin",
		      "UPDATE plugins SET Logo='%s' WHERE PlgCod=%ld",
                      NewLogo,Plg_EditingPlg->PlgCod);

      /***** Write message to show the change made *****/
      Ale_CreateAlert (Ale_SUCCESS,NULL,
	               Txt_The_new_logo_is_X,
                       NewLogo);
     }
   else
      Ale_CreateAlertYouCanNotLeaveFieldEmpty ();

   /***** Update logo *****/
   Str_Copy (Plg_EditingPlg->Logo,NewLogo,
             Plg_MAX_BYTES_PLUGIN_LOGO);
  }

/*****************************************************************************/
/****************** Change the application key of a plugin *******************/
/*****************************************************************************/

void Plg_ChangePlgAppKey (void)
  {
   extern const char *Txt_The_new_logo_is_X;			// TODO: !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
   char NewAppKey[Plg_MAX_BYTES_PLUGIN_APP_KEY + 1];

   /***** Plugin constructor *****/
   Plg_EditingPluginConstructor ();

   /***** Get parameters from form *****/
   /* Get the code of the plugin */
   if ((Plg_EditingPlg->PlgCod = Plg_GetParamPlgCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of plugin is missing.");

   /* Get the new logo for the plugin */
   Par_GetParToText ("AppKey",NewAppKey,Plg_MAX_BYTES_PLUGIN_APP_KEY);

   /***** Get plugin data from the database *****/
   Plg_GetDataOfPluginByCod (Plg_EditingPlg);

   /***** Check if new logo is empty *****/
   if (NewAppKey[0])
     {
      /* Update the table changing old application key by new application key */
      DB_QueryUPDATE ("can not update the application key of a plugin",
		      "UPDATE plugins SET AppKey='%s' WHERE PlgCod=%ld",
                      NewAppKey,Plg_EditingPlg->PlgCod);

      /***** Write message to show the change made *****/
      Ale_CreateAlert (Ale_SUCCESS,NULL,
		       Txt_The_new_logo_is_X,			// TODO!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                       NewAppKey);
     }
   else
      Ale_CreateAlertYouCanNotLeaveFieldEmpty ();

   /***** Update app key *****/
   Str_Copy (Plg_EditingPlg->AppKey,NewAppKey,
             Plg_MAX_BYTES_PLUGIN_APP_KEY);
  }

/*****************************************************************************/
/************************ Change the URL of a plugin *************************/
/*****************************************************************************/

void Plg_ChangePlgURL (void)
  {
   extern const char *Txt_The_new_URL_is_X;
   char NewURL[Cns_MAX_BYTES_WWW + 1];

   /***** Plugin constructor *****/
   Plg_EditingPluginConstructor ();

   /***** Get parameters from form *****/
   /* Get the code of the plugin */
   if ((Plg_EditingPlg->PlgCod = Plg_GetParamPlgCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of plugin is missing.");

   /* Get the new URL for the plugin */
   Par_GetParToText ("URL",NewURL,Cns_MAX_BYTES_WWW);

   /***** Get plugin data from the database *****/
   Plg_GetDataOfPluginByCod (Plg_EditingPlg);

   /***** Check if new URL is empty *****/
   if (NewURL[0])
     {
      /* Update the table changing old WWW by new WWW */
      DB_QueryUPDATE ("can not update the URL of a plugin",
		      "UPDATE plugins SET URL='%s' WHERE PlgCod=%ld",
                      NewURL,Plg_EditingPlg->PlgCod);

      /***** Write message to show the change made *****/
      Ale_CreateAlert (Ale_SUCCESS,NULL,
	               Txt_The_new_URL_is_X,
                       NewURL);
     }
   else
      Ale_CreateAlertYouCanNotLeaveFieldEmpty ();

   /***** Update URL *****/
   Str_Copy (Plg_EditingPlg->URL,NewURL,
             Cns_MAX_BYTES_WWW);
  }

/*****************************************************************************/
/************************* Change the IP of a plugin *************************/
/*****************************************************************************/

void Plg_ChangePlgIP (void)
  {
   extern const char *Txt_The_new_IP_address_is_X;
   char NewIP[Cns_MAX_BYTES_IP + 1];

   /***** Plugin constructor *****/
   Plg_EditingPluginConstructor ();

   /***** Get parameters from form *****/
   /* Get the code of the plugin */
   if ((Plg_EditingPlg->PlgCod = Plg_GetParamPlgCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of plugin is missing.");

   /* Get the new IP for the plugin */
   Par_GetParToText ("IP",NewIP,Cns_MAX_BYTES_IP);

   /***** Get plugin data from the database *****/
   Plg_GetDataOfPluginByCod (Plg_EditingPlg);

   /***** Check if new IP is empty *****/
   if (NewIP[0])
     {
      /* Update the table changing old IP by new IP */
      DB_QueryUPDATE ("can not update the IP address of a plugin",
		      "UPDATE plugins SET IP='%s' WHERE PlgCod=%ld",
                      NewIP,Plg_EditingPlg->PlgCod);

      /***** Write message to show the change made *****/
      Ale_CreateAlert (Ale_SUCCESS,NULL,
	               Txt_The_new_IP_address_is_X,
                       NewIP);
     }
   else
      Ale_CreateAlertYouCanNotLeaveFieldEmpty ();

   /***** Update IP *****/
   Str_Copy (Plg_EditingPlg->IP,NewIP,
             Cns_MAX_BYTES_IP);
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
   extern const char *Txt_New_plugin;
   extern const char *Txt_Name;
   extern const char *Txt_Description;
   extern const char *Txt_Logo;
   extern const char *Txt_Application_key;
   extern const char *Txt_URL;
   extern const char *Txt_IP;
   extern const char *Txt_Create_plugin;

   /***** Begin form *****/
   Frm_StartForm (ActNewPlg);

   /***** Begin box and table *****/
   Box_BoxTableBegin (NULL,Txt_New_plugin,
                      NULL,NULL,
                      NULL,Box_NOT_CLOSABLE,2);

   /***** Write heading *****/
   HTM_TR_Begin (NULL);

   HTM_TH (1,1,"LM",Txt_Name);
   HTM_TH (1,1,"LM",Txt_Description);
   HTM_TH (1,1,"LM",Txt_Logo);
   HTM_TH (1,1,"LM",Txt_Application_key);
   HTM_TH (1,1,"LM",Txt_URL);
   HTM_TH (1,1,"LM",Txt_IP);

   HTM_TR_End ();

   /***** Row begin *****/
   HTM_TR_Begin (NULL);

   /***** Plugin name *****/
   HTM_TD_Begin ("class=\"CM\"");
   HTM_INPUT_TEXT ("Name",Plg_MAX_CHARS_PLUGIN_NAME,Plg_EditingPlg->Name,
                   HTM_DONT_SUBMIT_ON_CHANGE,
		   "size=\"10\" required=\"required\"");
   HTM_TD_End ();

   /***** Plugin description *****/
   HTM_TD_Begin ("class=\"CM\"");
   HTM_INPUT_TEXT ("Description",Plg_MAX_CHARS_PLUGIN_DESCRIPTION,
		   Plg_EditingPlg->Description,
		   HTM_DONT_SUBMIT_ON_CHANGE,
		   "size=\"30\" required=\"required\"");
   HTM_TD_End ();

   /***** Plugin logo *****/
   HTM_TD_Begin ("class=\"CM\"");
   HTM_INPUT_TEXT ("Logo",Plg_MAX_CHARS_PLUGIN_LOGO,Plg_EditingPlg->Logo,
                   HTM_DONT_SUBMIT_ON_CHANGE,
		   "size=\"4\" required=\"required\"");
   HTM_TD_End ();

   /***** Plugin application key *****/
   HTM_TD_Begin ("class=\"CM\"");
   HTM_INPUT_TEXT ("AppKey",Plg_MAX_CHARS_PLUGIN_APP_KEY,Plg_EditingPlg->AppKey,
                   HTM_DONT_SUBMIT_ON_CHANGE,
		   "size=\"16\" required=\"required\"");
   HTM_TD_End ();

   /***** Plugin URL *****/
   HTM_TD_Begin ("class=\"CM\"");
   HTM_INPUT_URL ("URL",Plg_EditingPlg->URL,HTM_DONT_SUBMIT_ON_CHANGE,
		  "size=\"15\" required=\"required\"");
   HTM_TD_End ();

   /***** Plugin IP address *****/
   HTM_TD_Begin ("class=\"CM\"");
   HTM_INPUT_TEXT ("IP",Cns_MAX_CHARS_IP,Plg_EditingPlg->IP,
                   HTM_DONT_SUBMIT_ON_CHANGE,
		   "size=\"10\" required=\"required\"");
   HTM_TD_End ();

   /***** Row end *****/
   HTM_TR_End ();

   /***** End table, send button and end box *****/
   Box_BoxTableWithButtonEnd (Btn_CREATE_BUTTON,Txt_Create_plugin);

   /***** End form *****/
   Frm_EndForm ();
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
   HTM_TH (1,1,"RM",Txt_Code);
   HTM_TH_Empty (1);
   HTM_TH (1,1,"LM",Txt_Name);
   HTM_TH (1,1,"LM",Txt_Description);
   HTM_TH (1,1,"LM",Txt_Logo);
   HTM_TH (1,1,"LM",Txt_Application_key);
   HTM_TH (1,1,"LM",Txt_URL);
   HTM_TH (1,1,"LM",Txt_IP);

   HTM_TR_End ();
  }

/*****************************************************************************/
/****************** Receive form to create a new plugin **********************/
/*****************************************************************************/

void Plg_ReceiveFormNewPlg (void)
  {
   extern const char *Txt_The_plugin_X_already_exists;
   extern const char *Txt_Created_new_plugin_X;
   extern const char *Txt_You_must_specify_the_logo_the_application_key_the_URL_and_the_IP_address_of_the_new_plugin;
   extern const char *Txt_You_must_specify_the_name_and_the_description_of_the_new_plugin;

   /***** Plugin constructor *****/
   Plg_EditingPluginConstructor ();

   /***** Get parameters from form *****/
   /* Get plugin name */
   Par_GetParToText ("Name",Plg_EditingPlg->Name,Plg_MAX_BYTES_PLUGIN_NAME);

   /* Get plugin description */
   Par_GetParToText ("Description",Plg_EditingPlg->Description,Plg_MAX_BYTES_PLUGIN_DESCRIPTION);

   /* Get plugin logo */
   Par_GetParToText ("Logo",Plg_EditingPlg->Logo,Plg_MAX_BYTES_PLUGIN_LOGO);

   /* Get plugin application key */
   Par_GetParToText ("AppKey",Plg_EditingPlg->AppKey,Plg_MAX_BYTES_PLUGIN_APP_KEY);

   /* Get plugin URL */
   Par_GetParToText ("URL",Plg_EditingPlg->URL,Cns_MAX_BYTES_WWW);

   /* Get plugin IP address */
   Par_GetParToText ("IP",Plg_EditingPlg->IP,Cns_MAX_BYTES_IP);

   if (Plg_EditingPlg->Name[0])	// If there's a plugin name
     {
      if (Plg_EditingPlg->Logo[0]   &&
	  Plg_EditingPlg->AppKey[0] &&
	  Plg_EditingPlg->URL[0]    &&
	  Plg_EditingPlg->IP[0])
        {
         /***** If name of plugin was in database... *****/
         if (Plg_CheckIfPluginNameExists (Plg_EditingPlg->Name,-1L))
            Ale_CreateAlert (Ale_WARNING,NULL,
        	             Txt_The_plugin_X_already_exists,
                             Plg_EditingPlg->Name);
         else	// Add new plugin to database
           {
            Plg_CreatePlugin (Plg_EditingPlg);
	    Ale_CreateAlert (Ale_SUCCESS,NULL,
		             Txt_Created_new_plugin_X,
			     Plg_EditingPlg->Name);
           }
        }
      else	// If there is not a logo, a URL or a IP
         Ale_CreateAlert (Ale_WARNING,NULL,
                          Txt_You_must_specify_the_logo_the_application_key_the_URL_and_the_IP_address_of_the_new_plugin);
     }
   else	// If there is not a plugin name
      Ale_CreateAlert (Ale_WARNING,NULL,
	               Txt_You_must_specify_the_name_and_the_description_of_the_new_plugin);
  }

/*****************************************************************************/
/***************************** Create a new plugin ***************************/
/*****************************************************************************/

static void Plg_CreatePlugin (struct Plugin *Plg)
  {
   /***** Create a new plugin *****/
   DB_QueryINSERT ("can not create plugin",
		   "INSERT INTO plugins"
		   " (Name,Description,Logo,"
		   "AppKey,URL,IP)"
		   " VALUES"
		   " ('%s','%s','%s',"
		   "'%s','%s','%s')",
                   Plg->Name,Plg->Description,Plg->Logo,
		   Plg->AppKey,Plg->URL,Plg->IP);
  }

/*****************************************************************************/
/************************ Plugin constructor/destructor **********************/
/*****************************************************************************/

static void Plg_EditingPluginConstructor (void)
  {
   /***** Pointer must be NULL *****/
   if (Plg_EditingPlg != NULL)
      Lay_ShowErrorAndExit ("Error initializing plugin.");

   /***** Allocate memory for plugin *****/
   if ((Plg_EditingPlg = (struct Plugin *) malloc (sizeof (struct Plugin))) == NULL)
      Lay_ShowErrorAndExit ("Error allocating memory for plugin.");

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
