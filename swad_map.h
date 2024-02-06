// swad_map.h: OpenStreetMap maps

#ifndef _SWAD_MAP
#define _SWAD_MAP
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

#include <stdbool.h>		// For boolean type

/*****************************************************************************/
/************************** Public types and constants ***********************/
/*****************************************************************************/

struct Map_Coordinates
  {
   double Latitude;
   double Longitude;
   double Altitude;
  };

/*****************************************************************************/
/****************************** Public prototypes ****************************/
/*****************************************************************************/

void Map_LeafletCSS (void);
void Map_LeafletScript (void);
void Map_CreateMap (const char *ContainerId,
		    const struct Map_Coordinates *Coord,unsigned Zoom);
void Map_AddTileLayer (void);
void Map_AddMarker (const struct Map_Coordinates *Coord);
void Map_AddPopup (const char *Title,const char *Subtitle,bool Open);
void Map_GetCoordAndZoom (struct Map_Coordinates *Coord,unsigned *Zoom,
			  const char *Query);
double Map_GetLatitudeFromStr (char *Str);
double Map_GetLongitudeFromStr (char *Str);
double Map_GetAltitudeFromStr (char *Str);

bool Map_CheckIfCoordAreAvailable (const struct Map_Coordinates *Coord);

#endif
