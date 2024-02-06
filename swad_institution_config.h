// swad_institution_config.h: configuration of current institution

#ifndef _SWAD_INS_CFG
#define _SWAD_INS_CFG
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
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void InsCfg_ShowConfiguration (void);
void InsCfg_PrintConfiguration (void);

void InsCfg_ReqLogo (void);
void InsCfg_ReceiveLogo (void);
void InsCfg_RemoveLogo (void);
void InsCfg_ChangeInsCty (void);
void InsCfg_RenameInsShort (void);
void InsCfg_RenameInsFull (void);
void InsCfg_ContEditAfterChgIns (void);
void InsCfg_ChangeInsWWW (void);

#endif
