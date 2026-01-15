// swad_firewall.h: firewall to mitigate denial of service attacks

#ifndef _SWAD_FW
#define _SWAD_FW
/*
    SWAD (Shared Workspace At a Distance in Spanish),
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
/************************** Public types and constants ***********************/
/*****************************************************************************/

/* The maximum number of clicks from the same IP-user in the interval
   should be large enough to prevent an IP from being banned
   due to automatic refresh when the user is viewing the last clicks. */
#define Fw_CHECK_INTERVAL		((time_t)(30UL))	// Check clicks in the last 30 seconds
#define Fw_MAX_CLICKS_IN_INTERVAL	150			// Maximum of 150 clicks allowed in 30 seconds
								// (5 clicks/s sustained for 30 s)

#define Fw_TIME_BANNED			((time_t)(60UL*60UL))	// Ban IP for 1 hour

#define Fw_TIME_TO_DELETE_OLD_CLICKS	Fw_CHECK_INTERVAL	// Remove clicks older than these seconds

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void Fir_CheckFirewallAndExitIfBanned (void);
void Fir_CheckFirewallAndExitIfTooManyRequests (void);

#endif
