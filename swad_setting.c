// swad_setting.c: user's settings / preferences

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2023 Antonio Cañas Vargas

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
/********************************** Headers **********************************/
/*****************************************************************************/

#include <stdbool.h>		// For boolean type
#include <stddef.h>		// For NULL
#include <string.h>

#include "swad_action_list.h"
#include "swad_box.h"
#include "swad_calendar.h"
#include "swad_config.h"
#include "swad_cookie.h"
#include "swad_database.h"
#include "swad_date.h"
#include "swad_error.h"
#include "swad_figure.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_HTML.h"
#include "swad_language.h"
#include "swad_layout.h"
#include "swad_menu.h"
#include "swad_notification.h"
#include "swad_parameter.h"
#include "swad_photo.h"
#include "swad_privacy.h"
#include "swad_setting.h"
#include "swad_setting_database.h"

/*****************************************************************************/
/*************** External global variables from others modules ***************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Set_PutIconsToSelectSideCols (void);
static void Set_PutIconsSideColumns (__attribute__((unused)) void *Args);

static void Set_GetAndUpdateUsrListType (void);
static void Set_GetUsrListTypeFromForm (void);
static void Set_GetMyUsrListTypeFromDB (void);

static void Set_GetParColsClassPhotoFromForm (void);

static void Set_GetAndUpdatePrefAboutListWithPhotos (void);
static bool Set_GetParListWithPhotosFromForm (void);

/*****************************************************************************/
/***************************** Edit settings ******************************/
/*****************************************************************************/

void Set_EditSettings (void)
  {
   extern const char *Hlp_PROFILE_Settings_internationalization;
   extern const char *Hlp_PROFILE_Settings_design;
   extern const char *Txt_Internationalization;
   extern const char *Txt_Design;

   /***** Internationalization: language, first day of week, date format *****/
   Box_BoxBegin (Txt_Internationalization,NULL,NULL,
                 Hlp_PROFILE_Settings_internationalization,Box_NOT_CLOSABLE);

      HTM_DIV_Begin ("class=\"FRAME_INLINE\"");
	 Lan_PutBoxToSelectLanguage ();		// 1. Language
      HTM_DIV_End ();

      HTM_DIV_Begin ("class=\"FRAME_INLINE\"");
	 Cal_PutIconsToSelectFirstDayOfWeek ();	// 2. First day of week
      HTM_DIV_End ();

      HTM_DIV_Begin ("class=\"FRAME_INLINE\"");
	 Dat_PutBoxToSelectDateFormat ();	// 3. Date format
      HTM_DIV_End ();

   Box_BoxEnd ();

   /***** Design: icon set, menu, theme, side columns *****/
   Box_BoxBegin (Txt_Design,NULL,NULL,
                 Hlp_PROFILE_Settings_design,Box_NOT_CLOSABLE);

      HTM_DIV_Begin ("class=\"FRAME_INLINE\"");
	 Ico_PutIconsToSelectIconSet ();		// 4. Icon set
      HTM_DIV_End ();

      HTM_DIV_Begin ("class=\"FRAME_INLINE\"");
	 Mnu_PutIconsToSelectMenu ();			// 5. Menu
      HTM_DIV_End ();

      HTM_DIV_Begin ("class=\"FRAME_INLINE\"");
	 The_PutIconsToSelectTheme ();			// 6. Theme
      HTM_DIV_End ();

      HTM_DIV_Begin ("class=\"FRAME_INLINE\"");
	 Set_PutIconsToSelectSideCols ();		// 7. Side columns
      HTM_DIV_End ();

      HTM_DIV_Begin ("class=\"FRAME_INLINE\"");
	 PhoSha_PutIconsToSelectPhotoShape ();		// 8. User photos
      HTM_DIV_End ();

   Box_BoxEnd ();

   if (Gbl.Usrs.Me.Logged)
     {
      /***** Form to set my settings on privacy *****/
      Pri_EditMyPrivacy ();

      /***** Form to set my settings on cookies *****/
      Coo_EditMyPrefsOnCookies ();

      /***** Automatic email to notify of new events *****/
      Ntf_PutFormChangeNotifSentByEMail ();
     }
  }

/*****************************************************************************/
/******************** Get settings changed from current IP *******************/
/*****************************************************************************/

void Set_GetSettingsFromIP (void)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   const char *IP = Par_GetIP ();

   if (IP[0])
     {
      /***** Get settings from database *****/
      if (Set_DB_GetSettingsFromIP (&mysql_res))
	{
	 row = mysql_fetch_row (mysql_res);

	 /* Get first day of week (row[0]),
	        date format (row[1]),
	        theme (row[2]),
	        icon set (row[3]),
	        menu (row[4]),
	        if user wants to show side columns (row[5]),
	        user photo shape (row[6]) */
	 Gbl.Prefs.FirstDayOfWeek = Cal_GetFirstDayOfWeekFromStr (row[0]);
	 Gbl.Prefs.DateFormat     = Dat_GetDateFormatFromStr (row[1]);
	 Gbl.Prefs.Theme          = The_GetThemeFromStr (row[2]);
	 Gbl.Prefs.IconSet        = Ico_GetIconSetFromStr (row[3]);
	 Gbl.Prefs.Menu           = Mnu_GetMenuFromStr (row[4]);
	 Gbl.Prefs.SideCols       = Set_GetSideColsFromStr (row[5]);
	 Gbl.Prefs.PhotoShape     = PhoSha_GetShapeFromStr (row[6]);
	}

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);
     }
  }

/*****************************************************************************/
/************************ Set settings from current IP ***********************/
/*****************************************************************************/

void Set_SetSettingsFromIP (void)
  {
   /***** Update IP settings for current IP in database *****/
   Set_DB_UpdateMyIPSettingsForCurrentIP ();

   /***** If I am logged, update my IP settings in database for all my IP's *****/
   if (Gbl.Usrs.Me.Logged)
      Set_DB_UpdateMyIPSettingsForAllMyIPs ();
  }

/*****************************************************************************/
/************ Put icons to select the layout of the side columns *************/
/*****************************************************************************/

static void Set_PutIconsToSelectSideCols (void)
  {
   extern const char *Hlp_PROFILE_Settings_columns;
   extern const char *Txt_Columns;
   extern const char *Txt_LAYOUT_SIDE_COLUMNS[4];
   unsigned SideCols;
   char Icon[32 + 1];

   Box_BoxBegin (Txt_Columns,Set_PutIconsSideColumns,NULL,
                 Hlp_PROFILE_Settings_columns,Box_NOT_CLOSABLE);
      Set_BeginSettingsHead ();
	 Set_BeginOneSettingSelector ();
	 for (SideCols  = 0;
	      SideCols <= Lay_SHOW_BOTH_COLUMNS;
	      SideCols++)
	   {
	    Set_BeginPref (SideCols == Gbl.Prefs.SideCols);
	       Frm_BeginForm (ActChgCol);
		  Par_PutParUnsigned (NULL,"SideCols",SideCols);
		  snprintf (Icon,sizeof (Icon),"layout%u%u_32x20.gif",
			    SideCols >> 1,SideCols & 1);
		  Ico_PutSettingIconLink (Icon,Ico_UNCHANGED,Txt_LAYOUT_SIDE_COLUMNS[SideCols]);
	       Frm_EndForm ();
	    Set_EndPref ();
	   }
	 Set_EndOneSettingSelector ();
      Set_EndSettingsHead ();
   Box_BoxEnd ();
  }

/*****************************************************************************/
/************** Put contextual icons in side-columns setting *****************/
/*****************************************************************************/

static void Set_PutIconsSideColumns (__attribute__((unused)) void *Args)
  {
   /***** Put icon to show a figure *****/
   Fig_PutIconToShowFigure (Fig_SIDE_COLUMNS);
  }

/*****************************************************************************/
/*********************** Change layout of side columns ***********************/
/*****************************************************************************/

void Set_ChangeSideCols (void)
  {
   /***** Get param side-columns *****/
   Gbl.Prefs.SideCols = Set_GetParSideCols ();

   /***** Store side colums in database *****/
   if (Gbl.Usrs.Me.Logged)
      Set_DB_UpdateMySettingsAboutSideCols ();

   /***** Set settings from current IP *****/
   Set_SetSettingsFromIP ();
  }

/*****************************************************************************/
/************** Get parameter used to show/hide side columns *****************/
/*****************************************************************************/

unsigned Set_GetParSideCols (void)
  {
   return (unsigned) Par_GetParUnsignedLong ("SideCols",
                                             0,
                                             Lay_SHOW_BOTH_COLUMNS,
                                             Cfg_DEFAULT_COLUMNS);
  }

/*****************************************************************************/
/*************************** Get menu from string ****************************/
/*****************************************************************************/

unsigned Set_GetSideColsFromStr (const char *Str)
  {
   unsigned UnsignedNum;

   if (sscanf (Str,"%u",&UnsignedNum) == 1)
      if (UnsignedNum <= Lay_SHOW_BOTH_COLUMNS)
         return UnsignedNum;

   return Cfg_DEFAULT_COLUMNS;
  }

/*****************************************************************************/
/**************** Put hidden parameters with type of list, *******************/
/**************** number of columns in class photo         *******************/
/**************** and preference about viewing photos      *******************/
/*****************************************************************************/

void Set_PutParsPrefsAboutUsrList (void)
  {
   Set_PutParUsrListType (Gbl.Usrs.Me.ListType);
   Set_PutParColsClassPhoto ();
   Set_PutParListWithPhotos ();
  }

/*****************************************************************************/
/****************** Get and update type of list,        **********************/
/****************** number of columns in class photo    **********************/
/****************** and preference about viewing photos **********************/
/*****************************************************************************/

void Set_GetAndUpdatePrefsAboutUsrList (void)
  {
   /***** Get and update type of list *****/
   Set_GetAndUpdateUsrListType ();

   /***** Get and update number of columns in class photo *****/
   Set_GetAndUpdateColsClassPhoto ();

   /***** Get and update preference about viewing photos *****/
   Set_GetAndUpdatePrefAboutListWithPhotos ();
  }

/*****************************************************************************/
/****************** Get from form the type of users' list ********************/
/*****************************************************************************/

static void Set_GetAndUpdateUsrListType (void)
  {
   /***** Get type of list used to select users from form *****/
   Set_GetUsrListTypeFromForm ();

   if (Gbl.Usrs.Me.ListType != Set_USR_LIST_UNKNOWN)
      /* Save in the database the type of list preferred by me */
      Set_DB_UpdateMyUsrListType ();
   else
      /* If parameter can't be retrieved from,
         get my preference from database */
      Set_GetMyUsrListTypeFromDB ();
  }

/*****************************************************************************/
/************* Put a hidden parameter with the users' list type **************/
/*****************************************************************************/

void Set_PutParUsrListType (Set_ShowUsrsType_t ListType)
  {
   Par_PutParUnsigned (NULL,"UsrListType",(unsigned) ListType);
  }

/*****************************************************************************/
/****************** Get from form the type of users' list ********************/
/*****************************************************************************/

static void Set_GetUsrListTypeFromForm (void)
  {
   Gbl.Usrs.Me.ListType = (Set_ShowUsrsType_t)
	                  Par_GetParUnsignedLong ("UsrListType",
                                                  0,
                                                  Set_NUM_USR_LIST_TYPES - 1,
                                                  (unsigned long) Set_USR_LIST_UNKNOWN);
  }

/*****************************************************************************/
/************** Get my preference about type of users' list ******************/
/*****************************************************************************/

static void Set_GetMyUsrListTypeFromDB (void)
  {
   extern const char *Set_DB_StringsUsrListTypes[Set_NUM_USR_LIST_TYPES];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   Set_ShowUsrsType_t ListType;

   /***** Get type of listing of users from database *****/
   if (Set_DB_GetMyUsrListType (&mysql_res))		// Should be one only row
     {
      /* Get type of users' listing used to select some of them */
      Gbl.Usrs.Me.ListType = Set_SHOW_USRS_TYPE_DEFAULT;
      row = mysql_fetch_row (mysql_res);
      if (row[0])
         for (ListType  = (Set_ShowUsrsType_t) 0;
              ListType <= (Set_ShowUsrsType_t) (Set_NUM_USR_LIST_TYPES - 1);
              ListType++)
            if (!strcasecmp (row[0],Set_DB_StringsUsrListTypes[ListType]))
              {
               Gbl.Usrs.Me.ListType = ListType;
               break;
              }
     }
   else		// If I am an administrator or superuser
		// and I don't belong to current course,
		// then the result will be the default
      Gbl.Usrs.Me.ListType = Set_SHOW_USRS_TYPE_DEFAULT;

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/************* Get and update number of columns in class photo ***************/
/*****************************************************************************/

void Set_GetAndUpdateColsClassPhoto (void)
  {
   /***** Get the number of columns in class photo from form *****/
   Set_GetParColsClassPhotoFromForm ();

   if (Gbl.Usrs.ClassPhoto.Cols)
      /* Save the number of columns into the database */
      Set_DB_UpdateMyColsClassPhoto ();
   else
      /* If parameter can't be retrieved from form,
         get my preference from database */
      Set_GetMyColsClassPhotoFromDB ();
  }

/*****************************************************************************/
/** Get my prefs. about number of colums in class photo for current course ***/
/*****************************************************************************/

void Set_GetMyColsClassPhotoFromDB (void)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;

   Gbl.Usrs.ClassPhoto.Cols = Usr_CLASS_PHOTO_COLS_DEF;

   /***** If user logged and course selected... *****/
   if (Gbl.Usrs.Me.Logged &&
       Gbl.Hierarchy.Level == Hie_CRS)	// Course selected
     {
      /***** Get number of columns in class photo from database *****/
      if (Set_DB_GetMyColsClassPhoto (&mysql_res))
        {
         /* Get number of columns in class photo */
         row = mysql_fetch_row (mysql_res);
         if (row[0])
            if (sscanf (row[0],"%u",&Gbl.Usrs.ClassPhoto.Cols) == 1)
               if (Gbl.Usrs.ClassPhoto.Cols < 1 ||
                   Gbl.Usrs.ClassPhoto.Cols > Usr_CLASS_PHOTO_COLS_MAX)
                  Gbl.Usrs.ClassPhoto.Cols = Usr_CLASS_PHOTO_COLS_DEF;
        }

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);
     }
  }

/*****************************************************************************/
/****** Put a hidden parameter with the number of colums in class photo ******/
/*****************************************************************************/

void Set_PutParColsClassPhoto (void)
  {
   Par_PutParUnsigned (NULL,"ColsClassPhoto",Gbl.Usrs.ClassPhoto.Cols);
  }

/*****************************************************************************/
/************* Get from form the number of colums in class photo *************/
/*****************************************************************************/

static void Set_GetParColsClassPhotoFromForm (void)
  {
   Gbl.Usrs.ClassPhoto.Cols = (unsigned)
	                      Par_GetParUnsignedLong ("ColsClassPhoto",
                                                      1,
                                                      Usr_CLASS_PHOTO_COLS_MAX,
                                                      0);
  }

/*****************************************************************************/
/********** Get and update preference about photos in users' list ************/
/*****************************************************************************/

static void Set_GetAndUpdatePrefAboutListWithPhotos (void)
  {
   /***** Get my preference about photos in users' list from form *****/
   if (Set_GetParListWithPhotosFromForm ())
      /* Save preference about photos in users' list into the database */
      Set_DB_UpdateMyPrefAboutListWithPhotosPhoto ();
   else
      /* If parameter can't be retrieved from form,
         get my preference from database */
      Set_GetMyPrefAboutListWithPhotosFromDB ();
  }

/*****************************************************************************/
/** Put a hidden parameter with the preference about photos in users' list ***/
/*****************************************************************************/

void Set_PutParListWithPhotos (void)
  {
   Par_PutParChar ("WithPhotosExists",'Y');
   Par_PutParChar ("WithPhotos",
                           Gbl.Usrs.Listing.WithPhotos ? 'Y' :
                        	                         'N');
  }

/*****************************************************************************/
/********* Get from form the preference about photos in users' list **********/
/*****************************************************************************/

static bool Set_GetParListWithPhotosFromForm (void)
  {
   /***** Get if exists parameter with preference about photos in users' list *****/
   if (Par_GetParBool ("WithPhotosExists"))
     {
      /***** Parameter with preference about photos in users' list exists, so get it *****/
      Gbl.Usrs.Listing.WithPhotos = Par_GetParBool ("WithPhotos");
      return true;
     }

   Gbl.Usrs.Listing.WithPhotos = Usr_LIST_WITH_PHOTOS_DEF;
   return false;
  }

/*****************************************************************************/
/***** Get my preference about photos in users' list for current course ******/
/*****************************************************************************/

void Set_GetMyPrefAboutListWithPhotosFromDB (void)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;

   Gbl.Usrs.Listing.WithPhotos = Usr_LIST_WITH_PHOTOS_DEF;

   /***** If no user logged or not course selected... *****/
   if (Gbl.Usrs.Me.Logged && Gbl.Hierarchy.Node[Hie_CRS].HieCod)
     {
      /***** Get if listing of users must show photos from database *****/
      if (Set_DB_GetMyPrefAboutListWithPhotosPhoto (&mysql_res))
        {
         /* Get number of columns in class photo */
         Gbl.Usrs.Listing.WithPhotos = Usr_LIST_WITH_PHOTOS_DEF;
         row = mysql_fetch_row (mysql_res);
         Gbl.Usrs.Listing.WithPhotos = (row[0][0] == 'Y');
        }

      /***** Free structure that stores the query result *****/
      DB_FreeMySQLResult (&mysql_res);
     }
  }

/*****************************************************************************/
/*********** Head to select one or several settings using icons **************/
/*****************************************************************************/
/*
                +-- Container for several pref. selectors ---+
                |  +One pref.selector+  +One pref.selector+  |
                |  | +-----+ +-----+ |  | +-----+ +-----+ |  |
                |  | |Icon | |Icon | |  | |Icon | |Icon | |  |
                |  | |opt.A| |opt.B| |  | |opt.A| |opt.B| |  |
                |  | +-----+ +-----+ |  | +-----+ +-----+ |  |
                |  +-----------------+  +-----------------+  |
                +--------------------------------------------+
*/
void Set_BeginSettingsHead (void)
  {
   HTM_DIV_Begin ("class=\"PREF_CONTS\"");
  }

void Set_EndSettingsHead (void)
  {
   HTM_DIV_End ();
  }

void Set_BeginOneSettingSelector (void)
  {
   HTM_DIV_Begin ("class=\"PREF_CONT\"");
  }

void Set_EndOneSettingSelector (void)
  {
   HTM_DIV_End ();
  }

void Set_BeginPref (bool Condition)
  {
   HTM_DIV_Begin ("class=\"%s\"",
                  Condition ? "PREF_ON" :
                	      "PREF_OFF");
  }

void Set_EndPref (void)
  {
   HTM_DIV_End ();
  }
