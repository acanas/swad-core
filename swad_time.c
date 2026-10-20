// swad_time.c: timekeeping

/*
    SWAD (Shared Workspace At a Distance),
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

#include <time.h>		// For time

#include "swad_error.h"
#include "swad_time.h"

/*****************************************************************************/
/**************************** Private prototypes *****************************/
/*****************************************************************************/

static struct timespec partial_start;
static struct timespec partial_end;
static struct timespec global_start;
static struct timespec global_end;

void Tim_StartPartialTiming (void)
  {
   if (clock_gettime (CLOCK_REALTIME,&partial_start))
      Err_ShowErrorAndExit ("Error in clock_gettime");
  }

long Tim_StopPartialTiming (void)
  {
   if (clock_gettime (CLOCK_REALTIME,&partial_end))
      Err_ShowErrorAndExit ("Error in clock_gettime");
   return (long) (partial_end.tv_sec  - partial_start.tv_sec) * 1000000000L +
                  partial_end.tv_nsec - partial_start.tv_nsec;
  }

void Tim_StartGlobalTiming (void)
  {
   if (clock_gettime (CLOCK_REALTIME,&global_start))
      Err_ShowErrorAndExit ("Error in clock_gettime");
  }

long Tim_StopGlobalTiming (void)
  {
   if (clock_gettime (CLOCK_REALTIME,&global_end))
      Err_ShowErrorAndExit ("Error in clock_gettime");
   return (long) (global_end.tv_sec  - global_start.tv_sec) * 1000000000L +
                  global_end.tv_nsec - global_start.tv_nsec;
  }

/* Example:

#include "swad_time.h"

   static long globl_nsec = 0L;
   static long local_nsec = 0L;

   Tim_StartGlobalTiming ();

   Tim_StartPartialTiming ();
   piece of code to time
   local_nsec += Tim_StopPartialTiming ();

   global_nsec = Tim_StopGlobalTiming ();

   if (Gbl.Usrs.Me.Role.Logged == Rol_SYS_ADM)
     {
      Ale_ShowAlert (Ale_DEBUG,"local nsec = %ld ms",local_nsec / 1000000);
      Ale_ShowAlert (Ale_DEBUG,"global nsec = %ld ms",globl_nsec / 1000000);
     }
*/
