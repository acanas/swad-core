// swad_plugin_database.h: plugins called from SWAD using web services, operations with database

#ifndef _SWAD_PLG_DB
#define _SWAD_PLG_DB
/*
    SWAD (Shared Workspace At a Distance in Spanish),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2026 Antonio Cañas Vargas

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
/********************************** Headers **********************************/
/*****************************************************************************/

#include "swad_plugin.h"

/*****************************************************************************/
/****************************** Public prototypes ****************************/
/*****************************************************************************/

void Plg_DB_CreatePlugin (const struct Plg_Plugin *Plg);
void Plg_DB_ChangeName (long PlgCod,
                        const char NewPlgName[Plg_MAX_BYTES_PLUGIN_NAME + 1]);
void Plg_DB_ChangeDescription (long PlgCod,
                               const char NewDescription[Plg_MAX_BYTES_PLUGIN_DESCRIPTION + 1]);
void Plg_DB_ChangeLogo (long PlgCod,
                        const char NewLogo[Plg_MAX_BYTES_PLUGIN_LOGO + 1]);
void Plg_DB_ChangeAppKey (long PlgCod,
                          const char NewAppKey[Plg_MAX_BYTES_PLUGIN_APP_KEY + 1]);
void Plg_DB_ChangeURL (long PlgCod,
                       const char NewURL[WWW_MAX_BYTES_WWW + 1]);
void Plg_DB_ChangeIP (long PlgCod,
                      const char NewIP[Cns_MAX_BYTES_IP + 1]);

unsigned Plg_DB_GetListPlugins (MYSQL_RES **mysql_res);
Exi_Exist_t Plg_DB_GetPluginDataByCod (MYSQL_RES **mysql_res,long PlgCod);
long Plg_DB_GetPlgCodFromAppKey (const char *AppKey);
Exi_Exist_t Plg_DB_CheckIfPluginNameExists (const char *Name,long PlgCod);

void Plg_DB_RemovePlugin (long PlgCod);

#endif
