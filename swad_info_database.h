// swad_info_database.h: info about course, operations with database

#ifndef _SWAD_INF_DB
#define _SWAD_INF_DB
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

#include "swad_info.h"

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

//------------------------------- Info type -----------------------------------
Inf_Type_t Inf_DB_ConvertFromStrDBToInfoType (const char *StrInfoTypeDB);

//------------------------------ Info source ----------------------------------
void Inf_DB_SetInfoSrc (Inf_Src_t InfoSrc);
unsigned Inf_DB_GetInfoSrc (MYSQL_RES **mysql_res,
                            long CrsCod,Inf_Type_t InfoType);
unsigned Inf_DB_GetInfoSrcAndMustBeRead (MYSQL_RES **mysql_res,
                                         long CrsCod,Inf_Type_t InfoType);
Inf_Src_t Inf_DB_ConvertFromStrDBToInfoSrc (const char *StrInfoSrcDB);

//------------------------------- Info text -----------------------------------
void Inf_DB_SetInfoTxt (const char *InfoTxtHTML,const char *InfoTxtMD);

unsigned Inf_DB_GetInfoTxt (MYSQL_RES **mysql_res,
                            long CrsCod,Inf_Type_t InfoType);

//------------------------- Info read by students? ----------------------------
void Inf_DB_SetForceRead (bool MustBeRead);
void Inf_DB_SetIHaveRead (bool IHaveRead);

bool Inf_DB_CheckIfIHaveReadInfo (void);
unsigned Inf_DB_GetInfoTypesfIMustReadInfo (MYSQL_RES **mysql_res);

void Inf_DB_RemoveUsrFromCrsInfoRead (long UsrCod,long CrsCod);

#endif
