// swad_process.c: process in which this program runs

/*
    SWAD (Shared Workspace At a Distance),
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

#include "swad_process.h"

/*****************************************************************************/
/***************************** Private variables *****************************/
/*****************************************************************************/

static pid_t Prc_PID;	// PID of current process

/*****************************************************************************/
/************ Set/Get current PID (process identification number) ************/
/*****************************************************************************/

void Prc_SetPID (void)
  {
   Prc_PID = getpid ();
  }

pid_t Prc_GetPID (void)
  {
   return Prc_PID;
  }
