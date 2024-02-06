// swad_figure_database.h: figures (global stats) operations with database

#ifndef _SWAD_FIG_DB
#define _SWAD_FIG_DB
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

#include <mysql/mysql.h>	// To access MySQL databases

#include "swad_figure_cache.h"
#include "swad_hierarchy_type.h"

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void Fig_DB_UpdateUnsignedFigureIntoCache (FigCch_FigureCached_t Figure,
                                           Hie_Level_t Level,long HieCod,
                                           unsigned Value);
void Fig_DB_UpdateDoubleFigureIntoCache (FigCch_FigureCached_t Figure,
                                         Hie_Level_t Level,long HieCod,
                                         double Value);

unsigned Fig_DB_GetFigureFromCache (MYSQL_RES **mysql_res,
                                    FigCch_FigureCached_t Figure,
                                    Hie_Level_t Level,long HieCod,
                                    FigCch_Type_t Type,time_t TimeCached);

#endif
