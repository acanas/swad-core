// swad_setting.c: user's settings / preferences

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2021 Antonio Cañas Vargas

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
#include "swad_notification.h"
#include "swad_menu.h"
#include "swad_parameter.h"
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

static void Set_PutIconsToSelectUsrPhotos (void);
static void Set_PutIconsUsrPhotos (__attribute__((unused)) void *Args);

static void Set_GetAndUpdateUsrListType (void);
static void Set_GetUsrListTypeFromForm (void);
static void Set_GetMyUsrListTypeFromDB (void);

static void Set_GetParamColsClassPhotoFromForm (void);

static void Set_GetAndUpdatePrefAboutListWithPhotos (void);
static bool Set_GetParamListWithPhotosFromForm (void);

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
   Box_BoxBegin (NULL,Txt_Internationalization,
                 NULL,NULL,
                 Hlp_PROFILE_Settings_internationalization,Box_NOT_CLOSABLE);

      HTM_DIV_Begin ("class=\"FRAME_INLINE\"");
	 Lan_PutBoxToSelectLanguage ();		// 1. Language
      HTM_DIV_End ();

      HTM_DIV_Begin ("class=\"FRAME_INLINE\"");
	 Cal_PutIconsToSelectFirstDayOfWeek ();	// 2. First day of week
      HTM_DIV_End ();

      HTM_DIV_Begin ("class=\"FRAME_INLINE\"");
	 Dat_PutBoxToSelectDateFormat ();		// 3. Date format
      HTM_DIV_End ();

   Box_BoxEnd ();

   /***** Design: icon set, menu, theme, side columns *****/
   Box_BoxBegin (NULL,Txt_Design,
                 NULL,NULL,
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
	 Set_PutIconsToSelectUsrPhotos ();		// 8. User photos
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

   if (Gbl.IP[0])
     {
      /***** Get settings from database *****/
      if (Set_DB_GetSettingsFromIP (&mysql_res))
	{
	 row = mysql_fetch_row (mysql_res);

	 /* Get first day of week (row[0]) */
	 Gbl.Prefs.FirstDayOfWeek = Cal_GetFirstDayOfWeekFromStr (row[0]);

	 /* Get date format (row[1]) */
	 Gbl.Prefs.DateFormat = Dat_GetDateFormatFromStr (row[1]);

	 /* Get theme (row[2]) */
	 Gbl.Prefs.Theme = The_GetThemeFromStr (row[2]);

	 /* Get icon set (row[3]) */
	 Gbl.Prefs.IconSet = Ico_GetIconSetFromStr (row[3]);

	 /* Get menu (row[4]) */
	 Gbl.Prefs.Menu = Mnu_GetMenuFromStr (row[4]);

	 /* Get if user wants to show side columns (row[5]) */
	 if (sscanf (row[5],"%u",&Gbl.Prefs.SideCols) == 1)
	   {
	    if (Gbl.Prefs.SideCols > Lay_SHOW_BOTH_COLUMNS)
	       Gbl.Prefs.SideCols = Cfg_DEFAULT_COLUMNS;
	   }
	 else
	    Gbl.Prefs.SideCols = Cfg_DEFAULT_COLUMNS;

	 /* Get user photo shape (row[6]) */
	 if (sscanf (row[6],"%u",&Gbl.Prefs.UsrPhotos) == 1)
	   {
	    if (Gbl.Prefs.UsrPhotos >= Set_NUM_USR_PHOTOS)
	       Gbl.Prefs.UsrPhotos = Set_USR_PHOTOS_DEFAULT;
	   }
	 else
	    Gbl.Prefs.UsrPhotos = Set_USR_PHOTOS_DEFAULT;
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
   /***** Update settings from current IP in database *****/
   Set_DB_UpdateSettingsFromIP ();

   /***** If I am logged, update my settings in database for all my IP's *****/
   if (Gbl.Usrs.Me.Logged)
      Set_DB_UpdateMySettingsFromIP ();
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

   Box_BoxBegin (NULL,Txt_Columns,
                 Set_PutIconsSideColumns,NULL,
                 Hlp_PROFILE_Settings_columns,Box_NOT_CLOSABLE);
      Set_BeginSettingsHead ();
	 Set_BeginOneSettingSelector ();
	 for (SideCols  = 0;
	      SideCols <= Lay_SHOW_BOTH_COLUMNS;
	      SideCols++)
	   {
	    HTM_DIV_Begin ("class=\"%s\"",SideCols == Gbl.Prefs.SideCols ? "PREF_ON" :
									   "PREF_OFF");
	       Frm_BeginForm (ActChgCol);
		  Par_PutHiddenParamUnsigned (NULL,"SideCols",SideCols);
		  snprintf (Icon,sizeof (Icon),"layout%u%u_32x20.gif",
			    SideCols >> 1,SideCols & 1);
		  Ico_PutSettingIconLink (Icon,Txt_LAYOUT_SIDE_COLUMNS[SideCols]);
	       Frm_EndForm ();
	    HTM_DIV_End ();
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
   Gbl.Prefs.SideCols = Set_GetParamSideCols ();

   /***** Store side colums in database *****/
   if (Gbl.Usrs.Me.Logged)
      Set_DB_UpdateMySettingsAboutSideCols ();

   /***** Set settings from current IP *****/
   Set_SetSettingsFromIP ();
  }

/*****************************************************************************/
/************** Get parameter used to show/hide side columns *****************/
/*****************************************************************************/

unsigned Set_GetParamSideCols (void)
  {
   return (unsigned) Par_GetParToUnsignedLong ("SideCols",
                                               0,
                                               Lay_SHOW_BOTH_COLUMNS,
                                               Cfg_DEFAULT_COLUMNS);
  }

/*****************************************************************************/
/******************* Put icons to select user photo shape ********************/
/*****************************************************************************/

static void Set_PutIconsToSelectUsrPhotos (void)
  {
   extern const char *Hlp_PROFILE_Settings_user_photos;
   extern const char *Txt_User_photos;
   extern const char *Txt_USER_PHOTOS[Set_NUM_USR_PHOTOS];
   static const char *ClassPhoto[Set_NUM_USR_PHOTOS] =
     {
      [Set_USR_PHOTO_CIRCLE   ] = "ICO_HIGHLIGHT PHOTOC15x20B",
      [Set_USR_PHOTO_ELLIPSE  ] = "ICO_HIGHLIGHT PHOTOE15x20B",
      [Set_USR_PHOTO_RECTANGLE] = "ICO_HIGHLIGHT PHOTOR15x20B",
     };
   Set_UsrPhotos_t UsrPhotos;

   Box_BoxBegin (NULL,Txt_User_photos,
                 Set_PutIconsUsrPhotos,NULL,
                 Hlp_PROFILE_Settings_user_photos,Box_NOT_CLOSABLE);
      Set_BeginSettingsHead ();
	 Set_BeginOneSettingSelector ();
	 for (UsrPhotos  = (Set_UsrPhotos_t) 0;
	      UsrPhotos <= (Set_UsrPhotos_t) (Set_NUM_USR_PHOTOS - 1);
	      UsrPhotos++)
	   {
	    HTM_DIV_Begin ("class=\"%s\"",UsrPhotos == Gbl.Prefs.UsrPhotos ? "PREF_ON" :
									     "PREF_OFF");
	       Frm_BeginForm (ActChgUsrPho);
		  Par_PutHiddenParamUnsigned (NULL,"UsrPhotos",UsrPhotos);
		  HTM_INPUT_IMAGE (Cfg_URL_ICON_PUBLIC,"user.svg",Txt_USER_PHOTOS[UsrPhotos],ClassPhoto[UsrPhotos]);
	       Frm_EndForm ();
	    HTM_DIV_End ();
	   }
	 Set_EndOneSettingSelector ();
      Set_EndSettingsHead ();
   Box_BoxEnd ();
  }

/*****************************************************************************/
/************** Put contextual icons in side-columns setting *****************/
/*****************************************************************************/

static void Set_PutIconsUsrPhotos (__attribute__((unused)) void *Args)
  {
   /***** Put icon to show a figure *****/
   Fig_PutIconToShowFigure (Fig_USER_PHOTOS);
  }

/*****************************************************************************/
/************************** Change user photo shape **************************/
/*****************************************************************************/

void Set_ChangeUsrPhotos (void)
  {
   /***** Get param with user photo shape *****/
   Gbl.Prefs.UsrPhotos = Set_GetParamUsrPhotos ();

   /***** Store side colums in database *****/
   if (Gbl.Usrs.Me.Logged)
      Set_DB_UpdateMySettingsAboutUsrPhotos ();

   /***** Set settings from current IP *****/
   Set_SetSettingsFromIP ();
  }

/*****************************************************************************/
/************** Get parameter used to change user photo shape ****************/
/*****************************************************************************/

Set_UsrPhotos_t Set_GetParamUsrPhotos (void)
  {
   return (Set_UsrPhotos_t) Par_GetParToUnsignedLong ("UsrPhotos",
						      (Set_UsrPhotos_t) 0,
						      (Set_UsrPhotos_t) (Set_NUM_USR_PHOTOS - 1),
						      Set_USR_PHOTOS_DEFAULT);
  }

/*****************************************************************************/
/**************** Put hidden parameters with type of list, *******************/
/**************** number of columns in class photo         *******************/
/**************** and preference about viewing photos      *******************/
/*****************************************************************************/

void Set_PutParamsPrefsAboutUsrList (void)
  {
   Set_PutParamUsrListType (Gbl.Usrs.Me.ListType);
   Set_PutParamColsClassPhoto ();
   Set_PutParamListWithPhotos ();
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

void Set_PutParamUsrListType (Set_ShowUsrsType_t ListType)
  {
   Par_PutHiddenParamUnsigned (NULL,"UsrListType",(unsigned) ListType);
  }

/*****************************************************************************/
/****************** Get from form the type of users' list ********************/
/*****************************************************************************/

static void Set_GetUsrListTypeFromForm (void)
  {
   Gbl.Usrs.Me.ListType = (Set_ShowUsrsType_t)
	                  Par_GetParToUnsignedLong ("UsrListType",
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
   Set_GetParamColsClassPhotoFromForm ();

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
       Gbl.Hierarchy.Level == HieLvl_CRS)	// Course selected
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

void Set_PutParamColsClassPhoto (void)
  {
   Par_PutHiddenParamUnsigned (NULL,"ColsClassPhoto",Gbl.Usrs.ClassPhoto.Cols);
  }

/*****************************************************************************/
/************* Get from form the number of colums in class photo *************/
/*****************************************************************************/

static void Set_GetParamColsClassPhotoFromForm (void)
  {
   Gbl.Usrs.ClassPhoto.Cols = (unsigned)
	                      Par_GetParToUnsignedLong ("ColsClassPhoto",
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
   if (Set_GetParamListWithPhotosFromForm ())
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

void Set_PutParamListWithPhotos (void)
  {
   Par_PutHiddenParamChar ("WithPhotosExists",'Y');
   Par_PutHiddenParamChar ("WithPhotos",
                           Gbl.Usrs.Listing.WithPhotos ? 'Y' :
                        	                         'N');
  }

/*****************************************************************************/
/********* Get from form the preference about photos in users' list **********/
/*****************************************************************************/

static bool Set_GetParamListWithPhotosFromForm (void)
  {
   /***** Get if exists parameter with preference about photos in users' list *****/
   if (Par_GetParToBool ("WithPhotosExists"))
     {
      /***** Parameter with preference about photos in users' list exists, so get it *****/
      Gbl.Usrs.Listing.WithPhotos = Par_GetParToBool ("WithPhotos");
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
   if (Gbl.Usrs.Me.Logged && Gbl.Hierarchy.Crs.CrsCod)
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
