// swad_plugin.c: plugins called from SWAD using web services

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
/*
TODO: Check if web service is called from an authorized IP.
*/
/*****************************************************************************/
/********************************* Headers ***********************************/
/*****************************************************************************/

#include <linux/stddef.h>	// For NULL
#include <stdbool.h>		// For boolean type
#include <stdio.h>		// For fprintf
#include <stdlib.h>		// For calloc, free
#include <string.h>

#include "swad_box.h"
#include "swad_config.h"
#include "swad_constant.h"
#include "swad_cryptography.h"
#include "swad_database.h"
#include "swad_global.h"
#include "swad_parameter.h"
#include "swad_plugin.h"
#include "swad_session.h"
#include "swad_text.h"
#include "swad_web_service.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/***************************** Private constants *****************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Plg_PutIconToEditPlugins (void);
static void Plg_ListPluginsForEdition (void);
static void Plg_PutParamPlgCod (long PlgCod);
static void Plg_GetListPlugins (void);
static void Plg_PutFormToCreatePlugin (void);
static void Plg_PutHeadPlugins (void);
static bool Plg_CheckIfPluginNameExists (const char *Name,long PlgCod);
static void Plg_CreatePlugin (struct Plugin *Plg);

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
   char URL[Cns_MAX_BYTES_WWW + Ses_BYTES_SESSION_ID + 1];

   if (Gbl.Usrs.Me.Role.Logged != Rol_SYS_ADM)
     {
      Ale_ShowAlert (Ale_WARNING,Txt_Option_under_development);
      return;
     }

   /***** Get list of plugins *****/
   Plg_GetListPlugins ();

   /***** Start box and table *****/
   Box_StartBoxTable (NULL,Txt_Plugins,
                      Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM ? Plg_PutIconToEditPlugins :
                                                               NULL,
                      NULL,
		      false,	// Not closable
                      2);

   /***** Write table heading *****/
   fprintf (Gbl.F.Out,"<tr>"
                      "<th style=\"width:40px;\">"
                      "</th>"
                      "<th class=\"LEFT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "</tr>",
            Txt_Plugin);

   /***** Write all the plugins *****/
   for (NumPlg = 0;
	NumPlg < Gbl.Plugins.Num;
	NumPlg++)
     {
      Plg = &(Gbl.Plugins.Lst[NumPlg]);

      sprintf (URL,"%s%s",Plg->URL,Gbl.Session.Id);

      /* Plugin logo */
      // TODO: Change plugin icons to 32x32
      fprintf (Gbl.F.Out,"<tr>"
                         "<td class=\"DAT LEFT_MIDDLE\" style=\"width:45px;\">"
                         "<a href=\"%s\" title=\"%s\" class=\"DAT\" target=\"_blank\">"
                         "<img src=\"%s/%s/%s24x24.gif\""
                         " alt=\"%s\" title=\"%s\""
                         " class=\"ICO40x40\" />"
                         "</a>"
                         "</td>"
                         "<td class=\"DAT LEFT_MIDDLE\">"
                         "<a href=\"%s\" title=\"%s\" class=\"DAT\" target=\"_blank\">"
                         "%s"
                         "</a>"
                         "</td>"
                         "</tr>",
               URL,Plg->Name,
               Gbl.Prefs.IconsURL,Cfg_ICON_FOLDER_PLUGINS,Gbl.Plugins.Lst[NumPlg].Logo,
               Plg->Name,Plg->Name,
               URL,Plg->Name,
               Plg->Name);
     }

   /***** End table and box *****/
   Box_EndBoxTable ();

   /***** Free list of plugins *****/
   Plg_FreeListPlugins ();
  }

/*****************************************************************************/
/*************************** Put icon to edit plugins ************************/
/*****************************************************************************/

static void Plg_PutIconToEditPlugins (void)
  {
   Ico_PutContextualIconToEdit (ActEdiPlg,NULL);
  }

/*****************************************************************************/
/************************** Put forms to edit plugins ************************/
/*****************************************************************************/

void Plg_EditPlugins (void)
  {
   /***** Get list of plugins *****/
   Plg_GetListPlugins ();

   /***** Put a form to create a new plugin *****/
   Plg_PutFormToCreatePlugin ();

   /***** List current plugins *****/
   if (Gbl.Plugins.Num)
      Plg_ListPluginsForEdition ();

   /***** Free list of plugins *****/
   Plg_FreeListPlugins ();
  }

/*****************************************************************************/
/************************* Get list of current plugins ***********************/
/*****************************************************************************/

static void Plg_GetListPlugins (void)
  {
   char Query[256];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;
   unsigned NumPlg;
   struct Plugin *Plg;

   /***** Get plugins from database *****/
   sprintf (Query,"SELECT PlgCod,Name,Description,Logo,AppKey,URL,IP"
                  " FROM plugins ORDER BY Name");
   NumRows = DB_QuerySELECT (Query,&mysql_res,"can not get plugins");

   /***** Count number of rows in result *****/
   if (NumRows) // Plugins found...
     {
      Gbl.Plugins.Num = (unsigned) NumRows;

      /***** Create list with plugins *****/
      if ((Gbl.Plugins.Lst = (struct Plugin *) calloc ((size_t) Gbl.Plugins.Num,sizeof (struct Plugin))) == NULL)
          Lay_ShowErrorAndExit ("Not enough memory to store plugins.");

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
   char Query[256];
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
   sprintf (Query,"SELECT Name,Description,Logo,AppKey,URL,IP"
                  " FROM plugins"
                  " WHERE PlgCod=%ld",
            Plg->PlgCod);
   NumRows = DB_QuerySELECT (Query,&mysql_res,"can not get data of a plugin");

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
      free ((void *) Gbl.Plugins.Lst);
      Gbl.Plugins.Lst = NULL;
      Gbl.Plugins.Num = 0;
     }
  }

/*****************************************************************************/
/**************************** List all the plugins ***************************/
/*****************************************************************************/

static void Plg_ListPluginsForEdition (void)
  {
   extern const char *Txt_Plugins;
   unsigned NumPlg;
   struct Plugin *Plg;

   /***** Table head *****/
   Box_StartBoxTable (NULL,Txt_Plugins,NULL,
                      NULL,
		      false,	// Not closable
                      2);
   Plg_PutHeadPlugins ();

   /***** Write all the plugins *****/
   for (NumPlg = 0;
	NumPlg < Gbl.Plugins.Num;
	NumPlg++)
     {
      Plg = &Gbl.Plugins.Lst[NumPlg];

      /* Put icon to remove plugin */
      fprintf (Gbl.F.Out,"<tr>"
	                 "<td class=\"BM\">");
      Act_FormStart (ActRemPlg);
      Plg_PutParamPlgCod (Plg->PlgCod);
      Ico_PutIconRemove ();
      Act_FormEnd ();
      fprintf (Gbl.F.Out,"</td>");

      /* Plugin code */
      fprintf (Gbl.F.Out,"<td class=\"DAT RIGHT_MIDDLE\">"
	                 "%ld"
	                 "</td>",
               Plg->PlgCod);

      /* Plugin logo */
      // TODO: Change plugin icons to 32x32
      fprintf (Gbl.F.Out,"<td class=\"CENTER_MIDDLE\" style=\"width:45px;\">"
                         "<img src=\"%s/%s/%s24x24.gif\""
                         " alt=\"%s\" title=\"%s\""
                         " class=\"ICO40x40\" />"
                         "</td>",
               Gbl.Prefs.IconsURL,Cfg_ICON_FOLDER_PLUGINS,
               Gbl.Plugins.Lst[NumPlg].Logo,
               Gbl.Plugins.Lst[NumPlg].Name,
               Gbl.Plugins.Lst[NumPlg].Name);

      /* Plugin name */
      fprintf (Gbl.F.Out,"<td class=\"CENTER_MIDDLE\">");
      Act_FormStart (ActRenPlg);
      Plg_PutParamPlgCod (Plg->PlgCod);
      fprintf (Gbl.F.Out,"<input type=\"text\" name=\"Name\""
	                 " size=\"10\" maxlength=\"%u\" value=\"%s\""
                         " onchange=\"document.getElementById('%s').submit();\" />",
               Plg_MAX_CHARS_PLUGIN_NAME,Plg->Name,Gbl.Form.Id);
      Act_FormEnd ();
      fprintf (Gbl.F.Out,"</td>");

      /* Plugin description */
      fprintf (Gbl.F.Out,"<td class=\"CENTER_MIDDLE\">");
      Act_FormStart (ActChgPlgDes);
      Plg_PutParamPlgCod (Plg->PlgCod);
      fprintf (Gbl.F.Out,"<input type=\"text\" name=\"Description\""
	                 " size=\"30\" maxlength=\"%u\" value=\"%s\""
                         " onchange=\"document.getElementById('%s').submit();\" />",
               Plg_MAX_CHARS_PLUGIN_DESCRIPTION,Plg->Description,Gbl.Form.Id);
      Act_FormEnd ();
      fprintf (Gbl.F.Out,"</td>");

      /* Plugin logo */
      fprintf (Gbl.F.Out,"<td class=\"CENTER_MIDDLE\">");
      Act_FormStart (ActChgPlgLog);
      Plg_PutParamPlgCod (Plg->PlgCod);
      fprintf (Gbl.F.Out,"<input type=\"text\" name=\"Logo\""
	                 " size=\"4\" maxlength=\"%u\" value=\"%s\""
                         " onchange=\"document.getElementById('%s').submit();\" />",
               Plg_MAX_CHARS_PLUGIN_LOGO,Plg->Logo,Gbl.Form.Id);
      Act_FormEnd ();
      fprintf (Gbl.F.Out,"</td>");

      /* Plugin application key */
      fprintf (Gbl.F.Out,"<td class=\"CENTER_MIDDLE\">");
      Act_FormStart (ActChgPlgAppKey);
      Plg_PutParamPlgCod (Plg->PlgCod);
      fprintf (Gbl.F.Out,"<input type=\"text\" name=\"AppKey\""
	                 " size=\"16\" maxlength=\"%u\" value=\"%s\""
                         " onchange=\"document.getElementById('%s').submit();\" />",
               Plg_MAX_CHARS_PLUGIN_APP_KEY,Plg->AppKey,Gbl.Form.Id);
      Act_FormEnd ();
      fprintf (Gbl.F.Out,"</td>");

      /* Plugin URL */
      fprintf (Gbl.F.Out,"<td class=\"CENTER_MIDDLE\">");
      Act_FormStart (ActChgPlgURL);
      Plg_PutParamPlgCod (Plg->PlgCod);
      fprintf (Gbl.F.Out,"<input type=\"url\" name=\"URL\""
	                 " size=\"15\" maxlength=\"%u\" value=\"%s\""
                         " onchange=\"document.getElementById('%s').submit();\" />",
               Cns_MAX_CHARS_WWW,Plg->URL,Gbl.Form.Id);
      Act_FormEnd ();
      fprintf (Gbl.F.Out,"</td>");

      /* Plugin IP */
      fprintf (Gbl.F.Out,"<td class=\"CENTER_MIDDLE\">");
      Act_FormStart (ActChgPlgIP);
      Plg_PutParamPlgCod (Plg->PlgCod);
      fprintf (Gbl.F.Out,"<input type=\"text\" name=\"IP\""
	                 " size=\"10\" maxlength=\"%u\" value=\"%s\""
                         " onchange=\"document.getElementById('%s').submit();\" />",
               Cns_MAX_CHARS_IP,Plg->IP,
               Gbl.Form.Id);
      Act_FormEnd ();
      fprintf (Gbl.F.Out,"</td>"
                         "</tr>");
     }

   Box_EndBoxTable ();
  }

/*****************************************************************************/
/******************* Write parameter with code of plugin *********************/
/*****************************************************************************/

static void Plg_PutParamPlgCod (long PlgCod)
  {
   Par_PutHiddenParamLong ("PlgCod",PlgCod);
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
   char Query[128];
   struct Plugin Plg;

   /***** Get plugin code *****/
   if ((Plg.PlgCod = Plg_GetParamPlgCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of plugin is missing.");

   /***** Get data of the plugin from database *****/
   Plg_GetDataOfPluginByCod (&Plg);

   /***** Remove plugin *****/
   sprintf (Query,"DELETE FROM plugins WHERE PlgCod=%ld",
            Plg.PlgCod);
   DB_QueryDELETE (Query,"can not remove a plugin");

   /***** Write message to show the change made *****/
   sprintf (Gbl.Alert.Txt,Txt_Plugin_X_removed,
            Plg.Name);
   Ale_ShowAlert (Ale_SUCCESS,Gbl.Alert.Txt);

   /***** Show the form again *****/
   Plg_EditPlugins ();
  }

/*****************************************************************************/
/************************ Change the name of a plugin ************************/
/*****************************************************************************/

void Plg_RenamePlugin (void)
  {
   extern const char *Txt_You_can_not_leave_the_name_of_the_plugin_X_empty;
   extern const char *Txt_The_plugin_X_already_exists;
   extern const char *Txt_The_plugin_X_has_been_renamed_as_Y;
   extern const char *Txt_The_name_of_the_plugin_X_has_not_changed;
   char Query[128 + Plg_MAX_BYTES_PLUGIN_NAME];
   struct Plugin *Plg;
   char NewPlgName[Plg_MAX_BYTES_PLUGIN_NAME + 1];

   Plg = &Gbl.Plugins.EditingPlg;

   /***** Get parameters from form *****/
   /* Get the code of the plugin */
   if ((Plg->PlgCod = Plg_GetParamPlgCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of plugin is missing.");

   /* Get the new name for the plugin */
   Par_GetParToText ("Name",NewPlgName,Plg_MAX_BYTES_PLUGIN_NAME);

   /***** Get from the database the old name of the plugin *****/
   Plg_GetDataOfPluginByCod (Plg);

   /***** Check if new name is empty *****/
   if (!NewPlgName[0])
     {
      sprintf (Gbl.Alert.Txt,Txt_You_can_not_leave_the_name_of_the_plugin_X_empty,
               Plg->Name);
      Ale_ShowAlert (Ale_WARNING,Gbl.Alert.Txt);
     }
   else
     {
      /***** Check if old and new names are the same (this happens when user press enter with no changes in the form) *****/
      if (strcmp (Plg->Name,NewPlgName))	// Different names
        {
         /***** If plugin was in database... *****/
         if (Plg_CheckIfPluginNameExists (NewPlgName,Plg->PlgCod))
           {
            sprintf (Gbl.Alert.Txt,Txt_The_plugin_X_already_exists,
                     NewPlgName);
            Ale_ShowAlert (Ale_WARNING,Gbl.Alert.Txt);
           }
         else
           {
            /* Update the table changing old name by new name */
            sprintf (Query,"UPDATE plugins SET Name='%s' WHERE PlgCod=%ld",
                     NewPlgName,Plg->PlgCod);
            DB_QueryUPDATE (Query,"can not update the name of a plugin");

            /***** Write message to show the change made *****/
            sprintf (Gbl.Alert.Txt,Txt_The_plugin_X_has_been_renamed_as_Y,
                     Plg->Name,NewPlgName);
            Ale_ShowAlert (Ale_SUCCESS,Gbl.Alert.Txt);
           }
        }
      else	// The same name
        {
         sprintf (Gbl.Alert.Txt,Txt_The_name_of_the_plugin_X_has_not_changed,
                  Plg->Name);
         Ale_ShowAlert (Ale_INFO,Gbl.Alert.Txt);
        }
     }

   /***** Show the form again *****/
   Str_Copy (Plg->Name,NewPlgName,
             Plg_MAX_BYTES_PLUGIN_NAME);
   Plg_EditPlugins ();
  }

/*****************************************************************************/
/******************** Check if the name of plugin exists *********************/
/*****************************************************************************/

static bool Plg_CheckIfPluginNameExists (const char *Name,long PlgCod)
  {
   char Query[256 + Plg_MAX_BYTES_PLUGIN_NAME];

   /***** Get number of plugins with a name from database *****/
   sprintf (Query,"SELECT COUNT(*) FROM plugins"
	          " WHERE Name='%s' AND PlgCod<>%ld",
            Name,PlgCod);
   return (DB_QueryCOUNT (Query,"can not check if the name of a plugin already existed") != 0);
  }

/*****************************************************************************/
/******************* Change the description of a plugin **********************/
/*****************************************************************************/

void Plg_ChangePlgDescription (void)
  {
   extern const char *Txt_The_new_description_is_X;
   extern const char *Txt_You_can_not_leave_the_description_empty;
   struct Plugin *Plg;
   char Query[128 + Plg_MAX_BYTES_PLUGIN_DESCRIPTION];
   char NewDescription[Plg_MAX_BYTES_PLUGIN_DESCRIPTION + 1];

   Plg = &Gbl.Plugins.EditingPlg;

   /***** Get parameters from form *****/
   /* Get the code of the plugin */
   if ((Plg->PlgCod = Plg_GetParamPlgCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of plugin is missing.");

   /* Get the new description for the plugin */
   Par_GetParToText ("Description",NewDescription,Plg_MAX_BYTES_PLUGIN_DESCRIPTION);

   /***** Check if new description is empty *****/
   if (NewDescription[0])
     {
      /* Update the table changing old description by new description */
      sprintf (Query,"UPDATE plugins SET Description='%s' WHERE PlgCod=%ld",
               NewDescription,Plg->PlgCod);
      DB_QueryUPDATE (Query,"can not update the description of a plugin");

      /***** Write message to show the change made *****/
      sprintf (Gbl.Alert.Txt,Txt_The_new_description_is_X,
               NewDescription);
      Ale_ShowAlert (Ale_SUCCESS,Gbl.Alert.Txt);
     }
   else
     {
      sprintf (Gbl.Alert.Txt,"%s",Txt_You_can_not_leave_the_description_empty);
      Ale_ShowAlert (Ale_WARNING,Gbl.Alert.Txt);
     }

   /***** Show the form again *****/
   Str_Copy (Plg->Description,NewDescription,
             Plg_MAX_BYTES_PLUGIN_DESCRIPTION);
   Plg_EditPlugins ();
  }

/*****************************************************************************/
/************************ Change the logo of a plugin ************************/
/*****************************************************************************/

void Plg_ChangePlgLogo (void)
  {
   extern const char *Txt_The_new_logo_is_X;
   extern const char *Txt_You_can_not_leave_the_logo_empty;
   struct Plugin *Plg;
   char Query[128 + Plg_MAX_BYTES_PLUGIN_LOGO];
   char NewLogo[Plg_MAX_BYTES_PLUGIN_LOGO + 1];

   Plg = &Gbl.Plugins.EditingPlg;

   /***** Get parameters from form *****/
   /* Get the code of the plugin */
   if ((Plg->PlgCod = Plg_GetParamPlgCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of plugin is missing.");

   /* Get the new logo for the plugin */
   Par_GetParToText ("Logo",NewLogo,Plg_MAX_BYTES_PLUGIN_LOGO);

   /***** Check if new logo is empty *****/
   if (NewLogo[0])
     {
      /* Update the table changing old logo by new logo */
      sprintf (Query,"UPDATE plugins SET Logo='%s' WHERE PlgCod=%ld",
               NewLogo,Plg->PlgCod);
      DB_QueryUPDATE (Query,"can not update the logo of a plugin");

      /***** Write message to show the change made *****/
      sprintf (Gbl.Alert.Txt,Txt_The_new_logo_is_X,
               NewLogo);
      Ale_ShowAlert (Ale_SUCCESS,Gbl.Alert.Txt);
     }
   else
      Ale_ShowAlert (Ale_WARNING,Txt_You_can_not_leave_the_logo_empty);

   /***** Show the form again *****/
   Str_Copy (Plg->Logo,NewLogo,
             Plg_MAX_BYTES_PLUGIN_LOGO);
   Plg_EditPlugins ();
  }

/*****************************************************************************/
/****************** Change the application key of a plugin *******************/
/*****************************************************************************/

void Plg_ChangePlgAppKey (void)
  {
   extern const char *Txt_The_new_logo_is_X;	// TODO: !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
   extern const char *Txt_You_can_not_leave_the_logo_empty;// TODO: !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
   struct Plugin *Plg;
   char Query[128 + Plg_MAX_BYTES_PLUGIN_APP_KEY];
   char NewAppKey[Plg_MAX_BYTES_PLUGIN_APP_KEY + 1];

   Plg = &Gbl.Plugins.EditingPlg;

   /***** Get parameters from form *****/
   /* Get the code of the plugin */
   if ((Plg->PlgCod = Plg_GetParamPlgCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of plugin is missing.");

   /* Get the new logo for the plugin */
   Par_GetParToText ("AppKey",NewAppKey,Plg_MAX_BYTES_PLUGIN_APP_KEY);

   /***** Check if new logo is empty *****/
   if (NewAppKey[0])
     {
      /* Update the table changing old application key by new application key */
      sprintf (Query,"UPDATE plugins SET AppKey='%s' WHERE PlgCod=%ld",
               NewAppKey,Plg->PlgCod);
      DB_QueryUPDATE (Query,"can not update the application key of a plugin");

      /***** Write message to show the change made *****/
      sprintf (Gbl.Alert.Txt,Txt_The_new_logo_is_X,	// TODO!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
               NewAppKey);
      Ale_ShowAlert (Ale_SUCCESS,Gbl.Alert.Txt);
     }
   else
      Ale_ShowAlert (Ale_WARNING,Txt_You_can_not_leave_the_logo_empty);	// TODO!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

   /***** Show the form again *****/
   Str_Copy (Plg->AppKey,NewAppKey,
             Plg_MAX_BYTES_PLUGIN_APP_KEY);
   Plg_EditPlugins ();
  }

/*****************************************************************************/
/************************ Change the URL of a plugin *************************/
/*****************************************************************************/

void Plg_ChangePlgURL (void)
  {
   extern const char *Txt_The_new_URL_is_X;
   extern const char *Txt_You_can_not_leave_the_URL_empty;
   struct Plugin *Plg;
   char Query[128 + Cns_MAX_BYTES_WWW];
   char NewURL[Cns_MAX_BYTES_WWW + 1];

   Plg = &Gbl.Plugins.EditingPlg;

   /***** Get parameters from form *****/
   /* Get the code of the plugin */
   if ((Plg->PlgCod = Plg_GetParamPlgCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of plugin is missing.");

   /* Get the new URL for the plugin */
   Par_GetParToText ("URL",NewURL,Cns_MAX_BYTES_WWW);

   /***** Check if new URL is empty *****/
   if (NewURL[0])
     {
      /* Update the table changing old WWW by new WWW */
      sprintf (Query,"UPDATE plugins SET URL='%s' WHERE PlgCod=%ld",
               NewURL,Plg->PlgCod);
      DB_QueryUPDATE (Query,"can not update the URL of a plugin");

      /***** Write message to show the change made *****/
      sprintf (Gbl.Alert.Txt,Txt_The_new_URL_is_X,
               NewURL);
      Ale_ShowAlert (Ale_SUCCESS,Gbl.Alert.Txt);
     }
   else
      Ale_ShowAlert (Ale_WARNING,Txt_You_can_not_leave_the_URL_empty);

   /***** Show the form again *****/
   Str_Copy (Plg->URL,NewURL,
             Cns_MAX_BYTES_WWW);
   Plg_EditPlugins ();
  }

/*****************************************************************************/
/************************* Change the IP of a plugin *************************/
/*****************************************************************************/

void Plg_ChangePlgIP (void)
  {
   extern const char *Txt_The_new_IP_address_is_X;
   extern const char *Txt_You_can_not_leave_the_IP_address_empty;
   struct Plugin *Plg;
   char Query[128 + Cns_MAX_BYTES_IP];
   char NewIP[Cns_MAX_BYTES_IP + 1];

   Plg = &Gbl.Plugins.EditingPlg;

   /***** Get parameters from form *****/
   /* Get the code of the plugin */
   if ((Plg->PlgCod = Plg_GetParamPlgCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of plugin is missing.");

   /* Get the new IP for the plugin */
   Par_GetParToText ("IP",NewIP,Cns_MAX_BYTES_IP);

   /***** Check if new IP is empty *****/
   if (NewIP[0])
     {
      /* Update the table changing old IP by new IP */
      sprintf (Query,"UPDATE plugins SET IP='%s' WHERE PlgCod=%ld",
               NewIP,Plg->PlgCod);
      DB_QueryUPDATE (Query,"can not update the IP address of a plugin");

      /***** Write message to show the change made *****/
      sprintf (Gbl.Alert.Txt,Txt_The_new_IP_address_is_X,
               NewIP);
      Ale_ShowAlert (Ale_SUCCESS,Gbl.Alert.Txt);
     }
   else
      Ale_ShowAlert (Ale_WARNING,Txt_You_can_not_leave_the_IP_address_empty);

   /***** Show the form again *****/
   Str_Copy (Plg->IP,NewIP,
             Cns_MAX_BYTES_IP);
   Plg_EditPlugins ();
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
   struct Plugin *Plg;

   Plg = &Gbl.Plugins.EditingPlg;

   /***** Start form *****/
   Act_FormStart (ActNewPlg);

   /***** Start of frame *****/
   Box_StartBoxTable (NULL,Txt_New_plugin,NULL,
                      NULL,
	              false,	// Not closable
                      2);

   /***** Write heading *****/
   fprintf (Gbl.F.Out,"<tr>"
                      "<th class=\"LEFT_MIDDLE\">"
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
                      "<th class=\"LEFT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "<th class=\"LEFT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "</tr>",
            Txt_Name,
            Txt_Description,
            Txt_Logo,
            Txt_Application_key,
            Txt_URL,
            Txt_IP);

   /***** Plugin name *****/
   fprintf (Gbl.F.Out,"<td class=\"CENTER_MIDDLE\">"
                      "<input type=\"text\" name=\"Name\""
                      " size=\"10\" maxlength=\"%u\" value=\"%s\""
                      " required=\"required\" />"
                      "</td>",
            Plg_MAX_CHARS_PLUGIN_NAME,Plg->Name);

   /***** Plugin description *****/
   fprintf (Gbl.F.Out,"<td class=\"CENTER_MIDDLE\">"
                      "<input type=\"text\" name=\"Description\""
                      " size=\"30\" maxlength=\"%u\" value=\"%s\""
                      " required=\"required\" />"
                      "</td>",
            Plg_MAX_CHARS_PLUGIN_DESCRIPTION,Plg->Description);

   /***** Plugin logo *****/
   fprintf (Gbl.F.Out,"<td class=\"CENTER_MIDDLE\">"
                      "<input type=\"text\" name=\"Logo\""
                      " size=\"4\" maxlength=\"%u\" value=\"%s\""
                      " required=\"required\" />"
                      "</td>",
            Plg_MAX_CHARS_PLUGIN_LOGO,Plg->Logo);

   /***** Plugin application key *****/
   fprintf (Gbl.F.Out,"<td class=\"CENTER_MIDDLE\">"
                      "<input type=\"text\" name=\"AppKey\""
                      " size=\"16\" maxlength=\"%u\" value=\"%s\""
                      " required=\"required\" />"
		      "</td>",
	    Plg_MAX_CHARS_PLUGIN_APP_KEY,Plg->AppKey);

   /***** Plugin URL *****/
   fprintf (Gbl.F.Out,"<td class=\"CENTER_MIDDLE\">"
                      "<input type=\"url\" name=\"URL\""
                      " size=\"15\" maxlength=\"%u\" value=\"%s\""
                      " required=\"required\" />"
                      "</td>",
            Cns_MAX_CHARS_WWW,Plg->URL);

   /***** Plugin IP address *****/
   fprintf (Gbl.F.Out,"<td class=\"CENTER_MIDDLE\">"
                      "<input type=\"text\" name=\"IP\""
                      " size=\"10\" maxlength=\"%u\" value=\"%s\""
                      " required=\"required\" />"
                      "</td>"
                      "</tr>",
            Cns_MAX_CHARS_IP,Plg->IP);

   /***** Send button and end frame *****/
   Box_EndBoxTableWithButton (Btn_CREATE_BUTTON,Txt_Create_plugin);

   /***** End form *****/
   Act_FormEnd ();
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

   fprintf (Gbl.F.Out,"<tr>"
                      "<th></th>"
                      "<th class=\"RIGHT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "<th style=\"width:35px;\"></th>"
                      "<th class=\"LEFT_MIDDLE\">"
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
                      "<th class=\"LEFT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "<th class=\"LEFT_MIDDLE\">"
                      "%s"
                      "</th>"
                      "</tr>",
            Txt_Code,
            Txt_Name,
            Txt_Description,
            Txt_Logo,
            Txt_Application_key,
            Txt_URL,
            Txt_IP);
  }

/*****************************************************************************/
/****************** Receive form to create a new plugin **********************/
/*****************************************************************************/

void Plg_RecFormNewPlg (void)
  {
   extern const char *Txt_The_plugin_X_already_exists;
   extern const char *Txt_You_must_specify_the_logo_the_application_key_the_URL_and_the_IP_address_of_the_new_plugin;
   extern const char *Txt_You_must_specify_the_name_and_the_description_of_the_new_plugin;
   struct Plugin *Plg;

   Plg = &Gbl.Plugins.EditingPlg;

   /***** Get parameters from form *****/
   /* Get plugin name */
   Par_GetParToText ("Name",Plg->Name,Plg_MAX_BYTES_PLUGIN_NAME);

   /* Get plugin description */
   Par_GetParToText ("Description",Plg->Description,Plg_MAX_BYTES_PLUGIN_DESCRIPTION);

   /* Get plugin logo */
   Par_GetParToText ("Logo",Plg->Logo,Plg_MAX_BYTES_PLUGIN_LOGO);

   /* Get plugin application key */
   Par_GetParToText ("AppKey",Plg->AppKey,Plg_MAX_BYTES_PLUGIN_APP_KEY);

   /* Get plugin URL */
   Par_GetParToText ("URL",Plg->URL,Cns_MAX_BYTES_WWW);

   /* Get plugin IP address */
   Par_GetParToText ("IP",Plg->IP,Cns_MAX_BYTES_IP);

   if (Plg->Name[0])	// If there's a plugin name
     {
      if (Plg->Logo[0] && Plg->AppKey[0] && Plg->URL[0] && Plg->IP[0])
        {
         /***** If name of plugin was in database... *****/
         if (Plg_CheckIfPluginNameExists (Plg->Name,-1L))
           {
            sprintf (Gbl.Alert.Txt,Txt_The_plugin_X_already_exists,
                     Plg->Name);
            Ale_ShowAlert (Ale_WARNING,Gbl.Alert.Txt);
           }
         else	// Add new plugin to database
            Plg_CreatePlugin (Plg);
        }
      else	// If there is not a logo, a URL or a IP
         Ale_ShowAlert (Ale_WARNING,Txt_You_must_specify_the_logo_the_application_key_the_URL_and_the_IP_address_of_the_new_plugin);
     }
   else	// If there is not a plugin name
      Ale_ShowAlert (Ale_WARNING,Txt_You_must_specify_the_name_and_the_description_of_the_new_plugin);

   /***** Show the form again *****/
   Plg_EditPlugins ();
  }

/*****************************************************************************/
/***************************** Create a new plugin ***************************/
/*****************************************************************************/

static void Plg_CreatePlugin (struct Plugin *Plg)
  {
   extern const char *Txt_Created_new_plugin_X;
   char Query[512 + Plg_MAX_BYTES_PLUGIN_NAME +
                    Plg_MAX_BYTES_PLUGIN_DESCRIPTION +
                    Plg_MAX_BYTES_PLUGIN_LOGO +
                    Plg_MAX_BYTES_PLUGIN_APP_KEY +
                    Cns_MAX_BYTES_WWW +
                    Cns_MAX_BYTES_IP];

   /***** Create a new plugin *****/
   sprintf (Query,"INSERT INTO plugins"
	          " (Name,Description,Logo,AppKey,URL,IP)"
                  " VALUES"
                  " ('%s','%s','%s','%s','%s','%s')",
            Plg->Name,Plg->Description,Plg->Logo,Plg->AppKey,Plg->URL,Plg->IP);
   DB_QueryINSERT (Query,"can not create plugin");

   /***** Write success message *****/
   sprintf (Gbl.Alert.Txt,Txt_Created_new_plugin_X,
            Plg->Name);
   Ale_ShowAlert (Ale_SUCCESS,Gbl.Alert.Txt);
  }

/*****************************************************************************/
/******** Function called when a web service if required by a plugin *********/
/*****************************************************************************/

void Plg_WebService (void)
  {
   /***** Call soap service *****/
   Svc_WebService ();

   /***** All the output is made, so don't write anymore *****/
   Gbl.Layout.DivsEndWritten = Gbl.Layout.HTMLEndWritten = true;
  }
