// swad_web_service.h: SWAD web service provided to external plugins

#ifndef _SWAD_SVC
#define _SWAD_SVC
/*
    SWAD (Shared Workspace At a Distance in Spanish),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2016 Antonio Cañas Vargas

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
/***************************** Public constants ******************************/
/*****************************************************************************/

#define Svc_NUM_FUNCTIONS 25

/*****************************************************************************/
/******************************* Public types ********************************/
/*****************************************************************************/

// Add new functions at the end
typedef enum
  {
   Svc_unknown			=  0,	// unknown function
   Svc_loginBySessionKey	=  1,
   Svc_loginByUserPassword	=  2,	// deprecated
   Svc_loginByUserPasswordKey	=  3,
   Svc_getCourses		=  4,
   Svc_getUsers			=  5,
   Svc_getNotifications		=  6,
   Svc_getTestConfig		=  7,
   Svc_getTests			=  8,
   Svc_sendMessage		=  9,
   Svc_sendNotice		= 10,
   Svc_getDirectoryTree		= 11,
   Svc_getGroups		= 12,
   Svc_getGroupTypes		= 13,
   Svc_sendMyGroups		= 14,
   Svc_getFile			= 15,
   Svc_markNotificationsAsRead	= 16,
   Svc_getNewPassword		= 17,
   Svc_getCourseInfo		= 18,
   Svc_getAttendanceEvents	= 19,
   Svc_sendAttendanceEvent	= 20,
   Svc_getAttendanceUsers	= 21,
   Svc_sendAttendanceUsers	= 22,
   Svc_createAccount		= 23,
   Svc_getMarks			= 24,
   Svc_getTrivialQuestion       = 25,
  } Svc_Function_t;

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void Svc_WebService (void);
void Svc_Exit (const char *DetailErrorMessage);
const char *Svc_GetFunctionNameFromFunCod (long FunCod);

#endif
