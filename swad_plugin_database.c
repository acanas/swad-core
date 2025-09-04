// swad_plugin_database.c: plugins called from SWAD using web services, operations with database

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

#include "swad_database.h"
#include "swad_plugin_database.h"

/*****************************************************************************/
/***************************** Create a new plugin ***************************/
/*****************************************************************************/

void Plg_DB_CreatePlugin (const struct Plg_Plugin *Plg)
  {
   DB_QueryINSERT ("can not create plugin",
		   "INSERT INTO plg_plugins"
		   " (Name,Description,Logo,AppKey,URL,IP)"
		   " VALUES"
		   " ('%s','%s','%s','%s','%s','%s')",
                   Plg->Name,
                   Plg->Description,
                   Plg->Logo,
                   Plg->AppKey,
                   Plg->URL,
                   Plg->IP);
  }

/*****************************************************************************/
/******************************* Rename a plugin *****************************/
/*****************************************************************************/

void Plg_DB_ChangeName (long PlgCod,
                        const char NewPlgName[Plg_MAX_BYTES_PLUGIN_NAME + 1])
  {
   DB_QueryUPDATE ("can not update the name of a plugin",
		   "UPDATE plg_plugins"
		     " SET Name='%s'"
		   " WHERE PlgCod=%ld",
		   NewPlgName,
		   PlgCod);
  }

/*****************************************************************************/
/******************* Change the description of a plugin **********************/
/*****************************************************************************/

void Plg_DB_ChangeDescription (long PlgCod,
                               const char NewDescription[Plg_MAX_BYTES_PLUGIN_DESCRIPTION + 1])
  {
   DB_QueryUPDATE ("can not update the description of a plugin",
		   "UPDATE plg_plugins"
		     " SET Description='%s'"
		   " WHERE PlgCod=%ld",
		   NewDescription,
		   PlgCod);
  }

/*****************************************************************************/
/*********************** Change the logo of a plugin *************************/
/*****************************************************************************/

void Plg_DB_ChangeLogo (long PlgCod,
                        const char NewLogo[Plg_MAX_BYTES_PLUGIN_LOGO + 1])
  {
   DB_QueryUPDATE ("can not update the logo of a plugin",
		   "UPDATE plg_plugins"
		     " SET Logo='%s'"
		   " WHERE PlgCod=%ld",
		   NewLogo,
		   PlgCod);
  }

/*****************************************************************************/
/***************** Change the application key of a plugin ********************/
/*****************************************************************************/

void Plg_DB_ChangeAppKey (long PlgCod,
                          const char NewAppKey[Plg_MAX_BYTES_PLUGIN_APP_KEY + 1])
  {
   DB_QueryUPDATE ("can not update the application key of a plugin",
		   "UPDATE plg_plugins"
		     " SET AppKey='%s'"
		   " WHERE PlgCod=%ld",
		   NewAppKey,
		   PlgCod);
  }

/*****************************************************************************/
/************************* Change the URL of a plugin ************************/
/*****************************************************************************/

void Plg_DB_ChangeURL (long PlgCod,
                       const char NewURL[WWW_MAX_BYTES_WWW + 1])
  {
   DB_QueryUPDATE ("can not update the URL of a plugin",
		   "UPDATE plg_plugins"
		     " SET URL='%s'"
		   " WHERE PlgCod=%ld",
		   NewURL,
		   PlgCod);
  }

/*****************************************************************************/
/************************** Change the IP of a plugin ************************/
/*****************************************************************************/

void Plg_DB_ChangeIP (long PlgCod,
                      const char NewIP[Cns_MAX_BYTES_IP + 1])
  {
   DB_QueryUPDATE ("can not update the IP address of a plugin",
		   "UPDATE plg_plugins"
		     " SET IP='%s'"
		   " WHERE PlgCod=%ld",
		   NewIP,
		   PlgCod);
  }

/*****************************************************************************/
/************************* Get list of current plugins ***********************/
/*****************************************************************************/

unsigned Plg_DB_GetListPlugins (MYSQL_RES **mysql_res)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get plugins",
		   "SELECT PlgCod,"		// row[0]
			  "Name,"		// row[1]
			  "Description,"	// row[2]
			  "Logo,"		// row[3]
			  "AppKey,"		// row[4]
			  "URL,"		// row[5]
			  "IP"			// row[6]
		    " FROM plg_plugins"
		" ORDER BY Name");
  }

/*****************************************************************************/
/*************************** Get data of a plugin ****************************/
/*****************************************************************************/

Exi_Exist_t Plg_DB_GetPluginDataByCod (MYSQL_RES **mysql_res,long PlgCod)
  {
   return
   DB_QuerySELECT (mysql_res,"can not get data of a plugin",
		   "SELECT PlgCod,"		// row[0]
			  "Name,"		// row[1]
			  "Description,"	// row[2]
			  "Logo,"		// row[3]
			  "AppKey,"		// row[4]
			  "URL,"		// row[5]
			  "IP"			// row[6]
		    " FROM plg_plugins"
		   " WHERE PlgCod=%ld",
		   PlgCod) ? Exi_EXISTS :
			     Exi_DOES_NOT_EXIST;
  }

/*****************************************************************************/
/****** Check if the application key of the requester of a web service *******/
/****** is one of the application keys allowed in the plugins          *******/
/*****************************************************************************/

long Plg_DB_GetPlgCodFromAppKey (const char *AppKey)
  {
   return DB_QuerySELECTCode ("can not check application key",
			      "SELECT PlgCod"
			       " FROM plg_plugins"
			      " WHERE AppKey='%s'",
			      AppKey);
  }

/*****************************************************************************/
/******************** Check if the name of plugin exists *********************/
/*****************************************************************************/

Exi_Exist_t Plg_DB_CheckIfPluginNameExists (const char *Name,long PlgCod)
  {
   return
   DB_QueryEXISTS ("can not check if the name of a plugin already existed",
		   "SELECT EXISTS"
		   "(SELECT *"
		     " FROM plg_plugins"
		    " WHERE Name='%s'"
		      " AND PlgCod<>%ld)",
		   Name,
		   PlgCod);
  }

/*****************************************************************************/
/******************************* Remove a plugin *****************************/
/*****************************************************************************/

void Plg_DB_RemovePlugin (long PlgCod)
  {
   DB_QueryDELETE ("can not remove a plugin",
		   "DELETE FROM plg_plugins"
		   " WHERE PlgCod=%ld",
		   PlgCod);
  }
