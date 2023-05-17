// swad_project_config.h: projects configuration

#ifndef _SWAD_PRJ_CFG
#define _SWAD_PRJ_CFG
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

#include "swad_project.h"

/*****************************************************************************/
/************************** Public types and constants ***********************/
/*****************************************************************************/

#define PrjCfg_NET_CAN_CREATE_DEFAULT true

#define PrjCfg_NUM_RUBRIC_TYPES 4
typedef enum
  {
   PrjCfg_RUBRIC_ERR = 0,
   PrjCfg_RUBRIC_TUT = 1,
   PrjCfg_RUBRIC_EVL = 2,
   PrjCfg_RUBRIC_GBL = 3,
  } PrjCfg_RubricType_t;

struct PrjCfg_Config
  {
   bool NETCanCreate;
  };

struct PrgCfg_ListRubCods
  {
   long *RubCods;
   unsigned NumRubrics;
  };

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void PrjCfg_GetConfig (struct PrjCfg_Config *Config);

bool PrjCfg_CheckIfICanConfig (void);
void PrjCfg_ShowFormConfig (void);
PrjCfg_RubricType_t PrjCfg_GetRubricFromString (const char *Str);
void PrjCfg_ChangeNETCanCreate (void);
void PrjCfg_ChangeRubricsOfType (void);

#endif
