// swad_map.c: OpenStreetMap maps

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

#define _GNU_SOURCE 		// For asprintf
#include <stdio.h>		// For asprintf

#include "swad_database.h"
#include "swad_HTML.h"
#include "swad_map.h"
#include "swad_string.h"

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static unsigned Map_GetZoomFromDistance (double MaxDistance);

/*****************************************************************************/
/******************************** Leaflet CSS ********************************/
/*****************************************************************************/

/* https://leafletjs.com/examples/quick-start/ */

void Map_LeafletCSS (void)
  {
   HTM_Txt ("<link rel=\"stylesheet\""
	    " href=\"https://unpkg.com/leaflet@1.6.0/dist/leaflet.css\""
	    " integrity=\"sha512-xwE/Az9zrjBIphAcBb3F6JVqxf46+CDLwfLMHloNu6KEQCAWi6HcDUbeOfBIptF7tcCzusKFjFw2yuvEpDL9wQ==\""
	    " crossorigin=\"\" />");
  }

/*****************************************************************************/
/******************************* Leaflet script ******************************/
/*****************************************************************************/

void Map_LeafletScript (void)
  {
   /* Put this AFTER Leaflet's CSS */
   HTM_Txt ("<script src=\"https://unpkg.com/leaflet@1.6.0/dist/leaflet.js\""
	    " integrity=\"sha512-gZwIG9x3wUXg2hdXF6+rVkLF/0Vi9U8D2Ntg4Ga5I5BZpVkVxlJWbSQtXPSiUTtC0TjtGOmxa1AJPuV0CPthew==\""
	    " crossorigin=\"\">"
	    "</script>");
  }

/*****************************************************************************/
/************** Create a map centered in the given coordinates ***************/
/*****************************************************************************/

void Map_CreateMap (const char *ContainerId,
		    const struct Map_Coordinates *Coord,unsigned Zoom)
  {
   /* Let's create a map with pretty Mapbox Streets tiles */
   Str_SetDecimalPointToUS ();		// To write the decimal point as a dot
      HTM_TxtF ("\t"
		"var mymap = L.map('%s').setView([%.15lg, %.15lg], %u);\n",
		ContainerId,Coord->Latitude,Coord->Longitude,Zoom);
   Str_SetDecimalPointToLocal ();	// Return to local system
  }

/*****************************************************************************/
/************************* Add tile layer to our map *************************/
/*****************************************************************************/

#define Map_MAX_ZOOM 20
#define Map_MAPBOX_ACCESS_TOKEN "pk.eyJ1IjoiYWNhbmFzIiwiYSI6ImNrNGFoNXFxOTAzdHozcnA4d3Y0M3BwOGkifQ.uSg754Lv2iZEJg0W2pjiOQ"

void Map_AddTileLayer (void)
  {
   /* Next we'll add a tile layer to add to our map,
      in this case it's a Mapbox Streets tile layer.
      Creating a tile layer usually involves
      setting the URL template for the tile images,
      the attribution text and the maximum zoom level of the layer.
      In this example we'll use the mapbox/streets-v11 tiles
      from Mapbox's Static Tiles API
      (in order to use tiles from Mapbox,
      you must also request an access token).*/
   HTM_TxtF ("\t"
	     "L.tileLayer('https://api.mapbox.com/styles/v1/{id}/tiles/{z}/{x}/{y}?access_token={accessToken}',"
	     " {"
             "attribution:"
             " 'Map data &copy; <a href=\"https://www.openstreetmap.org/\">OpenStreetMap</a> contributors,"
             " <a href=\"https://creativecommons.org/licenses/by-sa/2.0/\">CC-BY-SA</a>,"
             " Imagery &copy; <a href=\"https://www.mapbox.com/\">Mapbox</a>',"
             "maxZoom: %u,"
             "id: 'mapbox/streets-v11',"
             "accessToken: '%s'"
             "}).addTo(mymap);\n",
	     Map_MAX_ZOOM,
	     Map_MAPBOX_ACCESS_TOKEN);
  }

/*****************************************************************************/
/************************** Add a marker to our map **************************/
/*****************************************************************************/

void Map_AddMarker (const struct Map_Coordinates *Coord)
  {
   Str_SetDecimalPointToUS ();		// To write the decimal point as a dot
      HTM_TxtF ("\t"
		"var marker = L.marker([%.15lg, %.15lg]).addTo(mymap);\n",
		Coord->Latitude,Coord->Longitude);
   Str_SetDecimalPointToLocal ();	// Return to local system
  }

/*****************************************************************************/
/************************* Add a marker to our map ***************************/
/*****************************************************************************/

void Map_AddPopup (const char *Title,const char *Subtitle,bool Open)
  {
   /* The bindPopup method attaches a popup with the specified HTML content
      to your marker so the popup appears when you click on the object,
      and the openPopup method (for markers only)
      immediately opens the attached popup. */
   HTM_TxtF ("\t"
	     "marker.bindPopup(\"<strong>%s</strong><br />%s\")",
	     Title,Subtitle);
   if (Open)
      HTM_Txt (".openPopup()");
   HTM_Txt (";\n");
  }

/*****************************************************************************/
/********* Get average coordinates of centers in current institution *********/
/*****************************************************************************/

void Map_GetCoordAndZoom (struct Map_Coordinates *Coord,unsigned *Zoom,
			  const char *Query)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   double MaxDistance;

   /***** Get average coordinates *****/
   if (DB_QuerySELECT (&mysql_res,"can not get coordinates",
		       "%s",Query))
     {
      /* Get row */
      row = mysql_fetch_row (mysql_res);

      /* Get latitude (row[0]), longitude (row[1]) and maximum distance (row[2]) */
      Coord->Latitude  = Map_GetLatitudeFromStr (row[0]);
      Coord->Longitude = Map_GetLongitudeFromStr (row[1]);
      MaxDistance      = Str_GetDoubleFromStr (row[2]);
     }
   else
      Coord->Latitude  =
      Coord->Longitude =
      MaxDistance      = 0.0;

   /***** Convert distance to zoom *****/
   *Zoom = Map_GetZoomFromDistance (MaxDistance);

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/************************ Get latitude from a string *************************/
/*****************************************************************************/
// This function may change Str on wrong double

double Map_GetLatitudeFromStr (char *Str)
  {
   double Latitude = Str_GetDoubleFromStr (Str);

   if (Latitude < -90.0)
      return -90.0;	// South Pole
   if (Latitude > 90.0)
      return 90.0;	// North Pole
   return Latitude;
  }

/*****************************************************************************/
/*********************** Get longitude from a string *************************/
/*****************************************************************************/
// This function may change Str on wrong double

double Map_GetLongitudeFromStr (char *Str)
  {
   double Longitude = Str_GetDoubleFromStr (Str);

   if (Longitude < -180.0)
      return -180.0;	// West
   if (Longitude > 180.0)
      return 180.0;	// East
   return Longitude;
  }

/*****************************************************************************/
/************************ Get altitude from a string *************************/
/*****************************************************************************/
// This function may change Str on wrong double

double Map_GetAltitudeFromStr (char *Str)
  {
   double Altitude = Str_GetDoubleFromStr (Str);

   if (Altitude < -413.0)
      return -413.0;	// Dead Sea shore
   if (Altitude > 8848.0)
      return 8848.0;	// Mount Everest
   return Altitude;
  }

/*****************************************************************************/
/*********************** Get zoom level from distance ************************/
/*****************************************************************************/

static unsigned Map_GetZoomFromDistance (double MaxDistance)
  {
   /***** Convert distance to zoom *****/
   return (MaxDistance <   0.01) ? 15 :
         ((MaxDistance <   0.1 ) ? 12 :
         ((MaxDistance <   1.0 ) ?  8 :
         ((MaxDistance <  10.0 ) ?  5 :
         ((MaxDistance <  50.0 ) ?  2 :
                                    1))));
  }

/*****************************************************************************/
/********************* Check if coordinates are avilable *********************/
/*****************************************************************************/

bool Map_CheckIfCoordAreAvailable (const struct Map_Coordinates *Coord)
  {
   /***** Coordinates 0, 0 means not set ==> don't show map *****/
   return Coord->Latitude ||
          Coord->Longitude;
  }
