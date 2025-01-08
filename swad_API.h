// swad_API.h: SWAD web API provided to external plugins

#ifndef _SWAD_API
#define _SWAD_API
/*
    SWAD (Shared Workspace At a Distance in Spanish),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2025 Antonio Cañas Vargas

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

#include "swad_cryptography.h"

/*****************************************************************************/
/***************************** Public constants ******************************/
/*****************************************************************************/

#define API_NUM_FUNCTIONS 35

#define API_BYTES_KEY Cry_BYTES_ENCRYPTED_STR_SHA256_BASE64

#define API_MAX_BYTES_DEGREES_STR (1024 - 1)

/*****************************************************************************/
/******************************* Public types ********************************/
/*****************************************************************************/

// Add new functions at the end
typedef enum
  {
   API_unknown			=  0,	// unknown function
   API_loginBySessionKey	=  1,
   API_loginByUserPassword	=  2,	// deprecated
   API_loginByUserPasswordKey	=  3,
   API_getCourses		=  4,
   API_getUsers			=  5,
   API_getNotifications		=  6,
   API_getTestConfig		=  7,
   API_getTests			=  8,
   API_sendMessage		=  9,
   API_sendNotice		= 10,
   API_getDirectoryTree		= 11,
   API_getGroups		= 12,
   API_getGroupTypes		= 13,
   API_sendMyGroups		= 14,
   API_getFile			= 15,
   API_markNotificationsAsRead	= 16,
   API_getNewPassword		= 17,
   API_getCourseInfo		= 18,
   API_getAttendanceEvents	= 19,
   API_sendAttendanceEvent	= 20,
   API_getAttendanceUsers	= 21,
   API_sendAttendanceUsers	= 22,
   API_createAccount		= 23,
   API_getMarks			= 24,
   API_getTrivialQuestion       = 25,
   API_findUsers		= 26,
   API_removeAttendanceEvent	= 27,
   API_getGames			= 28,
   API_getMatches		= 29,
   API_getMatchStatus		= 30,
   API_answerMatchQuestion	= 31,
   API_getLocation		= 32,
   API_sendMyLocation		= 33,
   API_getLastLocation		= 34,
   API_getAvailableRoles	= 35,
  } API_Function_t;

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

struct soap *API_Get_gSOAP_RuntimeEnv (void);
void API_WebService (void);
void API_Exit (const char *DetailErrorMessage);
const char *API_GetFunctionNameFromFunCod (long FunCod);

#endif
