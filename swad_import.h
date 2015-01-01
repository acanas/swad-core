// swad_import.h: import courses and students from another web application using web services

#ifndef _SWAD_IMP
#define _SWAD_IMP
/*
    SWAD (Shared Workspace At a Distance in Spanish),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2015 Antonio Cañas Vargas

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

#include "swad_group.h"

/*****************************************************************************/
/***************************** Public constants ******************************/
/*****************************************************************************/

#define Imp_MAX_BYTES_LIST_EXTERNAL_GRP_CODS (10*1024)

/*****************************************************************************/
/******************************* Public types ********************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void Imp_GetImpUsrAndSession (void);
void Imp_ImportStdsFromAnImpGrp (long ImpGrpCod,struct ListCodGrps *LstGrps,unsigned *NumUsrsRegistered);
void Imp_ListMyImpGrpsAndStdsNoForm (void);
void Imp_ListMyImpGrpsAndStdsForm (void);
void Imp_InsertImpSessionInDB (void);
bool Imp_GetImpSessionData (void);

#endif
