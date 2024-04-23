// swad_network_database.h: users' webs and social networks, operations with database

#ifndef _SWAD_NET_DB
#define _SWAD_NET_DB
/*
    SWAD (Shared Workspace At a Distance in Spanish),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2024 Antonio Cañas Vargas

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

#include "swad_constant.h"

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void Net_DB_UpdateMyWeb (Net_WebsAndSocialNetworks_t NumURL,
                         const char URL[WWW_MAX_BYTES_WWW + 1]);

void Net_DB_GetURL (long UsrCod,Net_WebsAndSocialNetworks_t NumURL,
                    char URL[WWW_MAX_BYTES_WWW + 1]);
unsigned Net_DB_GetWebAndSocialNetworksStats (MYSQL_RES **mysql_res);

void Net_DB_RemoveMyWeb (Net_WebsAndSocialNetworks_t NumURL);
void Net_DB_RemoveUsrWebs (long UsrCod);

#endif
