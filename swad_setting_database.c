// swad_setting.c: user's settings / preferences, operations with database

/*
    SWAD (Shared Workspace At a Distance),
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
/********************************** Headers **********************************/
/*****************************************************************************/

#include "swad_database.h"
#include "swad_global.h"
#include "swad_parameter.h"

/*****************************************************************************/
/*************** External global variables from others modules ***************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/****************************** Public constants *****************************/
/*****************************************************************************/

const char *Set_DB_StringsUsrListTypes[Set_NUM_USR_LIST_TYPES] =
  {
   [Set_USR_LIST_UNKNOWN       ] = "",
   [Set_USR_LIST_AS_CLASS_PHOTO] = "classphoto",
   [Set_USR_LIST_AS_LISTING    ] = "list",
  };

/*****************************************************************************/
/**************** Update my language to the current language *****************/
/*****************************************************************************/

void Set_DB_UpdateMySettingsAboutLanguage (void)
  {
   extern const char *Lan_STR_LANG_ID[1 + Lan_NUM_LANGUAGES];

   DB_QueryUPDATE ("can not update your language",
		   "UPDATE usr_data"
		     " SET Language='%s'"
		   " WHERE UsrCod=%ld",
	           Lan_STR_LANG_ID[Gbl.Prefs.Language],
	           Gbl.Usrs.Me.UsrDat.UsrCod);
  }

/*****************************************************************************/
/******************** Update my settings about date format *******************/
/*****************************************************************************/

void Set_DB_UpdateMySettingsAboutDateFormat (Dat_Format_t DateFormat)
  {
   DB_QueryUPDATE ("can not update your setting about date format",
		   "UPDATE usr_data"
		     " SET DateFormat=%u"
		   " WHERE UsrCod=%ld",
		   (unsigned) DateFormat,
		   Gbl.Usrs.Me.UsrDat.UsrCod);
  }

/*****************************************************************************/
/***************** Update my settings about first day of week ****************/
/*****************************************************************************/

void Set_DB_UpdateMySettingsAboutFirstDayOfWeek (unsigned FirstDayOfWeek)
  {
   DB_QueryUPDATE ("can not update your setting about first day of week",
		   "UPDATE usr_data"
		     " SET FirstDayOfWeek=%u"
		   " WHERE UsrCod=%ld",
		   FirstDayOfWeek,
		   Gbl.Usrs.Me.UsrDat.UsrCod);
  }

/*****************************************************************************/
/********************* Update my settings about icon set *********************/
/*****************************************************************************/

void Set_DB_UpdateMySettingsAboutIconSet (const char *IconSetId)
  {
   DB_QueryUPDATE ("can not update your setting about icon set",
		   "UPDATE usr_data"
		     " SET IconSet='%s'"
		   " WHERE UsrCod=%ld",
		   IconSetId,
		   Gbl.Usrs.Me.UsrDat.UsrCod);
  }

/*****************************************************************************/
/***************** Update my settings about first day of week ****************/
/*****************************************************************************/

void Set_DB_UpdateMySettingsAboutMenu (Mnu_Menu_t Menu)
  {
   DB_QueryUPDATE ("can not update your setting about menu",
		   "UPDATE usr_data"
		     " SET Menu=%u"
		   " WHERE UsrCod=%ld",
		   (unsigned) Menu,
		   Gbl.Usrs.Me.UsrDat.UsrCod);
  }

/*****************************************************************************/
/*********************** Update my settings about theme **********************/
/*****************************************************************************/

void Set_DB_UpdateMySettingsAboutTheme (const char *ThemeId)
  {
   DB_QueryUPDATE ("can not update your setting about theme",
		   "UPDATE usr_data"
		     " SET Theme='%s'"
		   " WHERE UsrCod=%ld",
		   ThemeId,
		   Gbl.Usrs.Me.UsrDat.UsrCod);
  }

/*****************************************************************************/
/************** Update layout of side colums on user data table **************/
/*****************************************************************************/

void Set_DB_UpdateMySettingsAboutSideCols (void)
  {
   DB_QueryUPDATE ("can not update your setting about side columns",
		   "UPDATE usr_data"
		     " SET SideCols=%u"
		   " WHERE UsrCod=%ld",
		   Gbl.Prefs.SideCols,
		   Gbl.Usrs.Me.UsrDat.UsrCod);
  }

/*****************************************************************************/
/**************** Update user photo shape on user data table *****************/
/*****************************************************************************/

void Set_DB_UpdateMySettingsAboutPhotoShape (void)
  {
   DB_QueryUPDATE ("can not update your setting about user photo shape",
		   "UPDATE usr_data"
		     " SET PhotoShape=%u"
		   " WHERE UsrCod=%ld",
		   (unsigned) Gbl.Prefs.PhotoShape,
		   Gbl.Usrs.Me.UsrDat.UsrCod);
  }

/*****************************************************************************/
/***************** Update my settings about photo visibility *****************/
/*****************************************************************************/

void Set_DB_UpdateUsrSettingsAboutPhotoVisibility (const struct Usr_Data *UsrDat)
  {
   extern const char *Pri_VisibilityDB[Pri_NUM_OPTIONS_PRIVACY];

   DB_QueryUPDATE ("can not update user's settings",
		   "UPDATE usr_data"
		     " SET PhotoVisibility='%s'"
		   " WHERE UsrCod=%ld",
		   Pri_VisibilityDB[UsrDat->PhotoVisibility],
		   UsrDat->UsrCod);
  }

/*****************************************************************************/
/************ Update my settings about public profile visibility *************/
/*****************************************************************************/

void Set_DB_UpdateMySettingsAboutBasicProfile (void)
  {
   extern const char *Pri_VisibilityDB[Pri_NUM_OPTIONS_PRIVACY];

   DB_QueryUPDATE ("can not update your setting"
		   " about public profile visibility",
		   "UPDATE usr_data"
		     " SET BaPrfVisibility='%s'"
		   " WHERE UsrCod=%ld",
                   Pri_VisibilityDB[Gbl.Usrs.Me.UsrDat.BaPrfVisibility],
                   Gbl.Usrs.Me.UsrDat.UsrCod);
  }

void Set_DB_UpdateMySettingsAboutExtendedProfile (void)
  {
   extern const char *Pri_VisibilityDB[Pri_NUM_OPTIONS_PRIVACY];

   DB_QueryUPDATE ("can not update your setting"
		   " about public profile visibility",
		   "UPDATE usr_data"
		     " SET ExPrfVisibility='%s'"
		   " WHERE UsrCod=%ld",
                   Pri_VisibilityDB[Gbl.Usrs.Me.UsrDat.ExPrfVisibility],
                   Gbl.Usrs.Me.UsrDat.UsrCod);
  }

/*****************************************************************************/
/******************* Update my settings about notify events ******************/
/*****************************************************************************/

void Set_DB_UpdateMySettingsAboutNotifyEvents (void)
  {
   DB_QueryUPDATE ("can not update user's settings",
		   "UPDATE usr_data"
		     " SET NotifNtfEvents=%u,"
		          "EmailNtfEvents=%u"
		   " WHERE UsrCod=%ld",
	           Gbl.Usrs.Me.UsrDat.NtfEvents.CreateNotif,
	           Gbl.Usrs.Me.UsrDat.NtfEvents.SendEmail,
	           Gbl.Usrs.Me.UsrDat.UsrCod);
  }

/*****************************************************************************/
/********************* Get my language from database *************************/
/*****************************************************************************/

unsigned Set_DB_GetMyLanguage (MYSQL_RES **mysql_res)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get user's language",
		   "SELECT Language"	// row[0]
		    " FROM usr_data"
		   " WHERE UsrCod=%ld",
		   Gbl.Usrs.Me.UsrDat.UsrCod);
  }

/*****************************************************************************/
/************ Register last prefs in current course in database **************/
/*****************************************************************************/

void Set_DB_InsertUsrInCrsSettings (long UsrCod,long HieCod)
  {
   DB_QueryINSERT ("can not register user in course",
		   "INSERT INTO crs_user_settings"
		   " (UsrCod,CrsCod,"
		     "LastDowGrpCod,LastComGrpCod,LastAssGrpCod,"
		     "NumAccTst,LastAccTst,NumQstsLastTst,"
		     "UsrListType,ColsClassPhoto,ListWithPhotos)"
		   " VALUES"
		   " (%ld,%ld,"
		     "-1,-1,-1,"
		     "0,FROM_UNIXTIME(%ld),0,"
		     "'%s',%u,'%c')",
	           UsrCod,
	           HieCod,
	           (long) (time_t) 0,	// The user never accessed to tests in this course
	           Set_DB_StringsUsrListTypes[Set_SHOW_USRS_TYPE_DEFAULT],
	           Usr_CLASS_PHOTO_COLS_DEF,
	           Usr_LIST_WITH_PHOTOS_DEF ? 'Y' :
					      'N');
  }

/*****************************************************************************/
/******** Update the group of my last access to a file browser zone **********/
/*****************************************************************************/

void Set_DB_UpdateGrpMyLastAccZone (const char *FieldNameDB,long GrpCod)
  {
   DB_QueryUPDATE ("can not update the group of the last access to a file browser",
		   "UPDATE crs_user_settings"
		     " SET %s=%ld"
		   " WHERE UsrCod=%ld"
		     " AND CrsCod=%ld",
                   FieldNameDB,GrpCod,
                   Gbl.Usrs.Me.UsrDat.UsrCod,
                   Gbl.Hierarchy.Node[Hie_CRS].HieCod);
  }

/*****************************************************************************/
/***************** Save my preference about type of users' list **************/
/*****************************************************************************/

void Set_DB_UpdateMyUsrListType (void)
  {
   DB_QueryUPDATE ("can not update type of listing",
		   "UPDATE crs_user_settings"
		     " SET UsrListType='%s'"
                   " WHERE UsrCod=%ld"
                     " AND CrsCod=%ld",
		   Set_DB_StringsUsrListTypes[Gbl.Usrs.Me.ListType],
		   Gbl.Usrs.Me.UsrDat.UsrCod,
		   Gbl.Hierarchy.Node[Hie_CRS].HieCod);
  }

/*****************************************************************************/
/** Save my prefs. about number of colums in class photo for current course **/
/*****************************************************************************/

void Set_DB_UpdateMyColsClassPhoto (unsigned Cols)
  {
   if (Gbl.Usrs.Me.Logged &&
       Gbl.Hierarchy.HieLvl == Hie_CRS)	// Course selected
      /***** Update number of colums in class photo for current course *****/
      DB_QueryUPDATE ("can not update number of columns in class photo",
		      "UPDATE crs_user_settings"
		        " SET ColsClassPhoto=%u"
                      " WHERE UsrCod=%ld"
                        " AND CrsCod=%ld",
		      Cols,
		      Gbl.Usrs.Me.UsrDat.UsrCod,
		      Gbl.Hierarchy.Node[Hie_CRS].HieCod);
  }

/*****************************************************************************/
/**** Save my preference about photos in users' list for current course ******/
/*****************************************************************************/

void Set_DB_UpdateMyPrefAboutListWithPhotosPhoto (bool WithPhotos)
  {
   if (Gbl.Usrs.Me.Logged &&
       Gbl.Hierarchy.HieLvl == Hie_CRS)	// Course selected
      /***** Update number of colums in class photo for current course *****/
      DB_QueryUPDATE ("can not update your preference about photos in listing",
		      "UPDATE crs_user_settings"
		        " SET ListWithPhotos='%c'"
                      " WHERE UsrCod=%ld"
                        " AND CrsCod=%ld",
		      WithPhotos ? 'Y' :
				   'N',
		      Gbl.Usrs.Me.UsrDat.UsrCod,
		      Gbl.Hierarchy.Node[Hie_CRS].HieCod);
  }

/*****************************************************************************/
/*************** Get my type of listing of users from database ***************/
/*****************************************************************************/

unsigned Set_DB_GetMyUsrListType (MYSQL_RES **mysql_res)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get type of listing of users",
		   "SELECT UsrListType"	// row[0]
		    " FROM crs_user_settings"
		   " WHERE UsrCod=%ld"
		     " AND CrsCod=%ld",
		   Gbl.Usrs.Me.UsrDat.UsrCod,
		   Gbl.Hierarchy.Node[Hie_CRS].HieCod);
  }

/*****************************************************************************/
/*********** Get my number of columns in class photo from database ***********/
/*****************************************************************************/

unsigned Set_DB_GetMyColsClassPhoto (MYSQL_RES **mysql_res)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get number of columns in class photo",
		   "SELECT ColsClassPhoto"	// row[0]
		    " FROM crs_user_settings"
		   " WHERE UsrCod=%ld"
		     " AND CrsCod=%ld",
		   Gbl.Usrs.Me.UsrDat.UsrCod,
		   Gbl.Hierarchy.Node[Hie_CRS].HieCod);
  }

/*****************************************************************************/
/********* Get my preference about listing with photos from database *********/
/*****************************************************************************/

unsigned Set_DB_GetMyPrefAboutListWithPhotosPhoto (MYSQL_RES **mysql_res)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not check if listing of users"
			     " should show photos",
		   "SELECT ListWithPhotos"	// row[0]
		    " FROM crs_user_settings"
		   " WHERE UsrCod=%ld"
		     " AND CrsCod=%ld",
		   Gbl.Usrs.Me.UsrDat.UsrCod,
		   Gbl.Hierarchy.Node[Hie_CRS].HieCod);
  }

/*****************************************************************************/
/****************** Remove a user from a courses setting *********************/
/*****************************************************************************/

void Set_DB_RemUsrFromCrsSettings (long UsrCod,long HieCod)
  {
   DB_QueryDELETE ("can not remove a user from a course",
		   "DELETE FROM crs_user_settings"
		   " WHERE UsrCod=%ld"
		     " AND CrsCod=%ld",
		   UsrCod,
		   HieCod);
  }

/*****************************************************************************/
/***************** Remove a user from all courses settings *******************/
/*****************************************************************************/

void Set_DB_RemUsrFromAllCrssSettings (long UsrCod)
  {
   DB_QueryDELETE ("can not remove a user from all courses",
		   "DELETE FROM crs_user_settings"
		   " WHERE UsrCod=%ld",
		   UsrCod);
  }

/*****************************************************************************/
/*************** Remove all users from settings in a course ******************/
/*****************************************************************************/

void Set_DB_RemAllUsrsFromCrsSettings (long HieCod)
  {
   DB_QueryDELETE ("can not remove users from a course settings",
		   "DELETE FROM crs_user_settings"
		   " WHERE CrsCod=%ld",
		   HieCod);
  }

/*****************************************************************************/
/**************** Update settings from current IP in database ****************/
/*****************************************************************************/

void Set_DB_UpdateMyIPSettingsForCurrentIP (void)
  {
   extern const char *The_ThemeId[The_NUM_THEMES];
   extern const char *Ico_IconSetId[Ico_NUM_ICON_SETS];

   DB_QueryREPLACE ("can not store settings from current IP address",
		    "REPLACE INTO set_ip_settings"
		    " (IP,UsrCod,LastChange,"
		      "FirstDayOfWeek,DateFormat,Theme,IconSet,Menu,SideCols,PhotoShape)"
		    " VALUES"
		    " ('%s',%ld,NOW(),"
		      "%u,%u,'%s','%s',%u,%u,%u)",
	            Par_GetIP (),
	            Gbl.Usrs.Me.UsrDat.UsrCod,
	            Gbl.Prefs.FirstDayOfWeek,
	            (unsigned) Gbl.Prefs.DateFormat,
	            The_ThemeId[Gbl.Prefs.Theme],
	            Ico_IconSetId[Gbl.Prefs.IconSet],
	            (unsigned) Gbl.Prefs.Menu,
	            Gbl.Prefs.SideCols,
	            (unsigned) Gbl.Prefs.PhotoShape);
  }

/*****************************************************************************/
/**************** Update my settings from all IP in database *****************/
/*****************************************************************************/

void Set_DB_UpdateMyIPSettingsForAllMyIPs (void)
  {
   extern const char *The_ThemeId[The_NUM_THEMES];
   extern const char *Ico_IconSetId[Ico_NUM_ICON_SETS];

   DB_QueryUPDATE ("can not update your settings",
		   "UPDATE set_ip_settings"
		     " SET FirstDayOfWeek=%u,"
			  "DateFormat=%u,"
			  "Theme='%s',"
			  "IconSet='%s',"
			  "Menu=%u,"
			  "SideCols=%u,"
			  "PhotoShape=%u"
		   " WHERE UsrCod=%ld",
		   Gbl.Prefs.FirstDayOfWeek,
		   (unsigned) Gbl.Prefs.DateFormat,
		   The_ThemeId[Gbl.Prefs.Theme],
		   Ico_IconSetId[Gbl.Prefs.IconSet],
		   (unsigned) Gbl.Prefs.Menu,
		   Gbl.Prefs.SideCols,
		   (unsigned) Gbl.Prefs.PhotoShape,
		   Gbl.Usrs.Me.UsrDat.UsrCod);
   }

/*****************************************************************************/
/******************** Get settings changed from current IP *******************/
/*****************************************************************************/

unsigned Set_DB_GetSettingsFromIP (MYSQL_RES **mysql_res)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get settings",
		   "SELECT FirstDayOfWeek,"	// row[0]
			  "DateFormat,"		// row[1]
			  "Theme,"		// row[2]
			  "IconSet,"		// row[3]
			  "Menu,"		// row[4]
			  "SideCols,"		// row[5]
			  "PhotoShape"		// row[6]
		    " FROM set_ip_settings"
		   " WHERE IP='%s'",
		   Par_GetIP ());
  }

/*****************************************************************************/
/*********************** Remove old settings from IP *************************/
/*****************************************************************************/

void Set_DB_RemoveOldSettingsFromIP (void)
  {
   /***** Remove old settings *****/
   DB_QueryDELETE ("can not remove old settings",
		   "DELETE LOW_PRIORITY FROM set_ip_settings"
		   " WHERE LastChange<FROM_UNIXTIME(UNIX_TIMESTAMP()-%lu)",
                   Cfg_TIME_TO_DELETE_IP_PREFS);
  }
