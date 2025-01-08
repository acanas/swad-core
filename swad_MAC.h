// swad_MAC.h: MAC addresses

#ifndef _SWAD_MAC
#define _SWAD_MAC
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
/************************** Public types and constants ***********************/
/*****************************************************************************/

#define MAC_NUM_BYTES 		6
#define MAC_LENGTH_MAC_ADDRESS (MAC_NUM_BYTES * 3 - 1)	// xx:xx:xx:xx:xx:xx

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void MAC_ListMACAddresses (unsigned NumMACs,MYSQL_RES **mysql_res);
void MAC_EditMACAddresses (long RooCod,const char *Anchor,
                           unsigned NumMACs,MYSQL_RES **mysql_res);

unsigned long long MAC_GetMACnumFromForm (const char *ParName);
void MAC_MACnumToMACstr (unsigned long long MACnum,char MACstr[MAC_LENGTH_MAC_ADDRESS + 1]);

#endif
