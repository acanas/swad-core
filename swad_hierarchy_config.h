// swad_hierarchy_config.h: hierarchy (country, institution, center, degree, course) configuration

#ifndef _SWAD_HIE_CFG
#define _SWAD_HIE_CFG
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
/********************************* Headers ***********************************/
/*****************************************************************************/

#include "swad_parameter_code.h"

/*****************************************************************************/
/************************** Public constants and types ***********************/
/*****************************************************************************/

typedef enum
  {
   Hie_DONT_PUT_LINK,
   Hie_PUT_LINK,
  } Hie_PutLink_t;

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void HieCfg_Title (Hie_PutLink_t PutLink,Hie_Level_t HieLvl);
void HieCfg_Name (Frm_PutForm_t PutForm,Hie_Level_t HieLvl,
		  Nam_ShrtOrFullName_t ShrtOrFull);
void HieCfg_WWW (Vie_ViewType_t ViewType,Frm_PutForm_t PutForm,
		 Act_Action_t NextAction,const char WWW[WWW_MAX_BYTES_WWW + 1]);
void HieCfg_Shortcut (Vie_ViewType_t ViewType,ParCod_Param_t ParCode,long HieCod);
void HieCfg_NumCtrs (unsigned NumCtrs,Frm_PutForm_t PutForm);
void HieCfg_NumCtrsWithMap (unsigned NumCtrs,unsigned NumCtrsWithMap);
void HieCfg_QR (ParCod_Param_t ParCode,long HieCod);
void HieCfg_NumUsrsInCrss (Hie_Level_t HieLvl,long HieCod,Rol_Role_t Role);

#endif
