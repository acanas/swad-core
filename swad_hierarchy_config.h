// swad_hierarchy_config.h: hierarchy (country, institution, center, degree, course) configuration

#ifndef _SWAD_HIE_CFG
#define _SWAD_HIE_CFG
/*
    SWAD (Shared Workspace At a Distance in Spanish),
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

#include "swad_parameter_code.h"

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void HieCfg_Title (bool PutLink,
		   HieLvl_Level_t LogoScope,
		   long LogoCod,
                   char LogoShrtName[Cns_HIERARCHY_MAX_BYTES_SHRT_NAME + 1],
		   char LogoFullName[Cns_HIERARCHY_MAX_BYTES_FULL_NAME + 1],
		   char LogoWWW[Cns_MAX_BYTES_WWW + 1],
		   char TextFullName[Cns_HIERARCHY_MAX_BYTES_FULL_NAME + 1]);
void HieCfg_FullName (bool PutForm,const char *Label,Act_Action_t NextAction,
		      const char FullName[Cns_HIERARCHY_MAX_BYTES_FULL_NAME + 1]);
void HieCfg_ShrtName (bool PutForm,Act_Action_t NextAction,
		      const char ShrtName[Cns_HIERARCHY_MAX_BYTES_SHRT_NAME + 1]);
void HieCfg_WWW (bool PrintView,bool PutForm,Act_Action_t NextAction,
		 const char WWW[Cns_MAX_BYTES_WWW + 1]);
void HieCfg_Shortcut (bool PrintView,ParCod_Param_t ParCode,long HieCod);
void HieCfg_NumCtrs (unsigned NumCtrs,bool PutForm);
void HieCfg_NumCtrsWithMap (unsigned NumCtrs,unsigned NumCtrsWithMap);
void HieCfg_QR (ParCod_Param_t ParCode,long HieCod);
void HieCfg_NumUsrsInCrss (HieLvl_Level_t Level,long Cod,Rol_Role_t Role);

#endif
