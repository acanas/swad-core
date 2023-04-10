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

/*****************************************************************************/
/************************** Public types and constants ***********************/
/*****************************************************************************/

/***** Configuration *****/
#define PrjCfg_NET_CAN_CREATE_DEFAULT true

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void PrjCfg_GetConfig (struct Prj_Projects *Projects);

bool PrjCfg_CheckIfICanConfig (void);
void PrjCfg_ShowFormConfig (void);
void PrjCfg_ReceiveConfig (void);

#endif
