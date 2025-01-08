// swad_config.h: configuration

#ifndef _SWAD_CFG
#define _SWAD_CFG
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
/** Uncomment one of the following installations of SWAD or create your own **/
/*****************************************************************************/

#define LOCALHOST_UBUNTU	// Comment this line if not applicable
//#define OPENSWAD_ORG		// Comment this line if not applicable
//#define SWAD_UGR_ES		// Comment this line if not applicable
//#define SWADBERRY_UGR_ES	// Comment this line if not applicable

/*****************************************************************************/
/********************************* Headers ***********************************/
/*****************************************************************************/

#include "swad_layout.h"

/*****************************************************************************/
/***************************** Public constants ******************************/
/*****************************************************************************/

/* IMPORTANT:
   - If one of the following paths does not exist, it should be created during installation!
*/
#if defined LOCALHOST_UBUNTU			// localhost (Ubuntu) ******************

#define Cfg_PLATFORM_SHORT_NAME			"SWAD local"
#define Cfg_PLATFORM_FULL_NAME			"SWAD (Shared Workspace At a Distance)"
#define Cfg_PLATFORM_SERVER			"localhost"			// Server name (main part of the URL)
#define Cfg_URL_SWAD_SERVER			"https://localhost/"		// With ending slash
#define Cfg_URL_SWAD_CGI			"https://localhost"		// Without ending slash
#define Cfg_URL_SWAD_PUBLIC			"https://localhost/swad"	// Without ending slash
#define Cfg_PATH_CGI_BIN			"/usr/lib/cgi-bin/swad"		// Directory for this CGI and other commands called by it
#define Cfg_PATH_SWAD_PUBLIC			"/var/www/html/swad"		// Main public directory for public SWAD pages, icons, etc.
#define Cfg_PATH_SWAD_PRIVATE			"/var/www/swad"			// Main private directory for SWAD private data

#define Cfg_PLATFORM_LOGO_SMALL_FILENAME	"swad64x64.png"			// Logo displayed in the top of the page
#define Cfg_PLATFORM_LOGO_SMALL_CLASS		"ICO40x40"
#define Cfg_PLATFORM_LOGO_BIG_FILENAME		"swad140x40.png"
#define Cfg_PLATFORM_LOGO_BIG_CLASS		"ICO140x40"

#define Cfg_PLATFORM_RESPONSIBLE_NAME		"Responsible name"		// Main responsible for the platform
#define Cfg_PLATFORM_RESPONSIBLE_EMAIL		"Responsible email address"	// Main responsible for the platform
#define Cfg_AUTOMATIC_EMAIL_SMTP_SERVER		"Your SMTP server"		// SMTP server for sending automatic emails
#define Cfg_AUTOMATIC_EMAIL_SMTP_PORT		"465"				// SMTP port for sending automatic emails
#define Cfg_AUTOMATIC_EMAIL_FROM		"Sender email address"		// Email address from where automatic emails will be sent
#define Cfg_DATABASE_HOST			"localhost"			// Database host
#define Cfg_DEFAULT_LANGUAGE			Lan_LANGUAGE_ES			// Default language
#define Cfg_LOG_START_YEAR			2012				// Year when the log started in the database

// Footer of page with info about the institution where this platform is installed
#define Cfg_ABOUT_NAME				"Centro de Educaci&oacute;n Virtual de la Universidad Nacional de Asunci&oacute;n"
#define Cfg_ABOUT_URL				"http://www.cevuna.una.py/"
#define Cfg_ABOUT_LOGO				"cevuna.una.py100x32.gif"
#define Cfg_ABOUT_LOGO_WIDTH			125
#define Cfg_ABOUT_LOGO_HEIGHT			40

#define Cfg_TIME_TO_SEND_PENDING_NOTIF 		((time_t) (30UL * 60UL))	// After these seconds after first pending notification, all pending notifications are sent by email

#elif defined OPENSWAD_ORG			// openswad.org ************************

#define Cfg_PLATFORM_SHORT_NAME			"OpenSWAD"
#define Cfg_PLATFORM_FULL_NAME			"OpenSWAD (Open Shared Workspace At a Distance)"
#define Cfg_PLATFORM_SERVER			"openswad.org"				// Server name (main part of the URL)
#define Cfg_URL_SWAD_SERVER			"https://openswad.org/"			// With ending slash
#define Cfg_URL_SWAD_CGI			"https://openswad.org"			// Without ending slash
#define Cfg_URL_SWAD_PUBLIC			"https://openswad.org/swad"		// Without ending slash
#define Cfg_PATH_CGI_BIN			"/var/www/cgi-bin/swad"			// Directory for this CGI and other commands called by it
#define Cfg_PATH_SWAD_PUBLIC			"/var/www/html/swad"			// Main public directory for public SWAD pages, icons, etc.
#define Cfg_PATH_SWAD_PRIVATE			"/var/www/swad"				// Main private directory for SWAD private data

#define Cfg_PLATFORM_LOGO_SMALL_FILENAME	"swad64x64.png"				// Logo displayed in the top of the page
#define Cfg_PLATFORM_LOGO_SMALL_CLASS		"ICO40x40"
#define Cfg_PLATFORM_LOGO_BIG_FILENAME		"openswad220x40.png"
#define Cfg_PLATFORM_LOGO_BIG_CLASS		"ICO220x40"

#define Cfg_PLATFORM_RESPONSIBLE_NAME		"Antonio Cañas Vargas"			// Main responsible for the platform
#define Cfg_PLATFORM_RESPONSIBLE_EMAIL		"webmaster@openswad.org"		// Main responsible for the platform
#define Cfg_AUTOMATIC_EMAIL_SMTP_SERVER		"mail.openswad.org"			// SMTP server for sending automatic emails
#define Cfg_AUTOMATIC_EMAIL_SMTP_PORT		"465"					// SMTP port for sending automatic emails
#define Cfg_AUTOMATIC_EMAIL_FROM		"acanas@openswad.org"			// Email address from where automatic emails will be sent
#define Cfg_DATABASE_HOST			"localhost"				// Database host
#define Cfg_DEFAULT_LANGUAGE			Lan_LANGUAGE_EN				// Default language
#define Cfg_LOG_START_YEAR			2012					// Year when the log started in the database

// Footer of page with info about the institution where this platform is installed
#define Cfg_ABOUT_NAME				"OpenSWAD"
#define Cfg_ABOUT_URL				"http://openswad.org/"
#define Cfg_ABOUT_LOGO				"swad32x32.gif"
#define Cfg_ABOUT_LOGO_WIDTH			32
#define Cfg_ABOUT_LOGO_HEIGHT			32

#define Cfg_TIME_TO_SEND_PENDING_NOTIF 		((time_t) (30UL * 60UL))	// After these seconds after first pending notification, all pending notifications are sent by email

#elif defined SWAD_UGR_ES			// swad.ugr.es *************************

#define Cfg_PLATFORM_SHORT_NAME			"SWAD"
#define Cfg_PLATFORM_FULL_NAME			"SWAD (Shared Workspace At a Distance)"
#define Cfg_PLATFORM_SERVER			"swad.ugr.es"				// Server name (main part of the URL)
#define Cfg_URL_SWAD_SERVER			"https://swad.ugr.es/"			// With ending slash
#define Cfg_URL_SWAD_CGI			"https://swad.ugr.es"			// Without ending slash
#define Cfg_URL_SWAD_PUBLIC			"https://swad.ugr.es/swad"		// Without ending slash
#define Cfg_PATH_CGI_BIN			"/var/www/cgi-bin"			// Directory for this CGI and other commands called by it
#define Cfg_PATH_SWAD_PUBLIC			"/var/www/html/swad"			// Main public directory for public SWAD pages, icons, etc.
#define Cfg_PATH_SWAD_PRIVATE			"/var/www/swad"				// Main private directory for SWAD private data

#define Cfg_PLATFORM_LOGO_SMALL_FILENAME	"swad-ugr160x80.png"				// Logo displayed in the top of the page
#define Cfg_PLATFORM_LOGO_SMALL_CLASS		"ICO80x40"
#define Cfg_PLATFORM_LOGO_BIG_FILENAME		"swad-ugr530x80.png"
#define Cfg_PLATFORM_LOGO_BIG_CLASS		"ICO265x40"

#define Cfg_PLATFORM_RESPONSIBLE_NAME		"UGR"					// Main responsible for the platform
#define Cfg_PLATFORM_RESPONSIBLE_EMAIL		"swad@ugr.es"				// Main responsible for the platform
#define Cfg_AUTOMATIC_EMAIL_SMTP_SERVER		"smtp.ugr.es"				// SMTP server for sending automatic emails
#define Cfg_AUTOMATIC_EMAIL_SMTP_PORT		"587"					// SMTP port for sending automatic emails
#define Cfg_AUTOMATIC_EMAIL_FROM		"swad@ugr.es"				// Email address from where automatic emails will be sent
#define Cfg_DATABASE_HOST			"localhost"				// Database host
#define Cfg_DEFAULT_LANGUAGE			Lan_LANGUAGE_ES				// Default language
#define Cfg_LOG_START_YEAR			2005					// Year when the log started in the database

// Footer of page with info about the institution where this platform is installed
#define Cfg_ABOUT_NAME				"Universidad de Granada"
#define Cfg_ABOUT_URL				"http://www.ugr.es/"
#define Cfg_ABOUT_LOGO				"ugr200x64.png"
#define Cfg_ABOUT_LOGO_WIDTH			100
#define Cfg_ABOUT_LOGO_HEIGHT			32

#define Cfg_TIME_TO_SEND_PENDING_NOTIF 		((time_t) (30UL * 60UL))	// After these seconds after first pending notification, all pending notifications are sent by email

#elif defined SWADBERRY_UGR_ES			// swadberry.ugr.es ******************

#define Cfg_PLATFORM_SHORT_NAME			"SWADberry"
#define Cfg_PLATFORM_FULL_NAME			"SWADberry (Shared Workspace At a Distance on a Raspberry Pi)"
#define Cfg_PLATFORM_SERVER			"swadberry.ugr.es"		// Server name (main part of the URL)
#define Cfg_URL_SWAD_SERVER			"https://swadberry.ugr.es/"	// With ending slash
#define Cfg_URL_SWAD_CGI			"https://swadberry.ugr.es"	// Without ending slash
#define Cfg_URL_SWAD_PUBLIC			"https://swadberry.ugr.es/swad"	// Without ending slash
#define Cfg_PATH_CGI_BIN			"/usr/lib/cgi-bin/swad"		// Directory for this CGI and other commands called by it
#define Cfg_PATH_SWAD_PUBLIC			"/var/www/html/swad"		// Main public directory for public SWAD pages, icons, etc.
#define Cfg_PATH_SWAD_PRIVATE			"/var/www/swad"			// Main private directory for SWAD private data

#define Cfg_PLATFORM_LOGO_SMALL_FILENAME	"swad64x64.png"			// Logo displayed in the top of the page
#define Cfg_PLATFORM_LOGO_SMALL_CLASS		"ICO40x40"
#define Cfg_PLATFORM_LOGO_BIG_FILENAME		"SWADberry212x40.png"
#define Cfg_PLATFORM_LOGO_BIG_CLASS		"ICO212x40"

#define Cfg_PLATFORM_RESPONSIBLE_NAME		"ATC - UGR"				// Main responsible for the platform
#define Cfg_PLATFORM_RESPONSIBLE_EMAIL		"swad@ugr.es"				// Main responsible for the platform
#define Cfg_AUTOMATIC_EMAIL_SMTP_SERVER		"smtp.ugr.es"				// SMTP server for sending automatic emails
#define Cfg_AUTOMATIC_EMAIL_SMTP_PORT		"587"					// SMTP port for sending automatic emails
#define Cfg_AUTOMATIC_EMAIL_FROM		"swad@ugr.es"				// Email address from where automatic emails will be sent
#define Cfg_DATABASE_HOST			"localhost"				// Database host
#define Cfg_DEFAULT_LANGUAGE			Lan_LANGUAGE_ES				// Default language
#define Cfg_LOG_START_YEAR			2016					// Year when the log started in the database

// Footer of page with info about the institution where this platform is installed
#define Cfg_ABOUT_NAME				"Dpt. Arquitectura y Tecnolog&iacute;a de Computadores"
#define Cfg_ABOUT_URL				"http://atc.ugr.es/"
#define Cfg_ABOUT_LOGO				"atc64x64.png"
#define Cfg_ABOUT_LOGO_WIDTH			32
#define Cfg_ABOUT_LOGO_HEIGHT			32

#define Cfg_TIME_TO_SEND_PENDING_NOTIF 		((time_t) (30UL * 60UL))	// After these seconds after first pending notification, all pending notifications are sent by email

#endif

/* Database */
#define Cfg_DATABASE_DBNAME		"swad"				// Database name
#define Cfg_DATABASE_USER		"swad"				// Database user

/* Email */
#define Cfg_AUTOMATIC_EMAIL_PASSWORD	DONT_PUBLISH_SMTP_PASSWORD	// Email user's password for automatic emails

/* SWADroid */
#define Cfg_SWADROID_URL		"https://play.google.com/store/apps/details?id=es.ugr.swad.swadroid"
#define Cfg_SWADROID_ICO		"SWADroid300x650.png"

/* About SWAD */
#define Cfg_ABOUT_SWAD_URL		"https://github.com/acanas/swad-core/wiki/UserGuide.en"

/* Link to download Java: keep updated! */
#define Cfg_JAVA_URL			"http://www.java.com/es/download/"
#define Cfg_JAVA_NAME			"Java Runtime Environment"

/* Users */
#define Cfg_MAX_USRS_IN_LIST			       30000	// If the number of users in a list is greater than this, don't show the list
#define Cfg_MIN_NUM_USERS_TO_CONFIRM_SHOW_BIG_LIST      1000	// If the number of users in a list is greater than this, ask me for confirmation before showing the list
#define Cfg_MIN_PHOTOS_TO_SHOW_AVERAGE			  10	// If the number of students with photo in a degree is less than this, don't show average photo of the degree
#define Cfg_MAX_RECIPIENTS				 250	// A student can not send a message to more than this number of recipients
#define Cfg_MAX_CONNECTED_SHOWN				  10	// Show (in right column) only these connected users with more recent activity

/* Courses */
#define Cfg_MIN_NUM_COURSES_TO_CONFIRM_SHOW_BIG_LIST    1000	// If the number of courses in a list is greater than this, ask me for confirmation before showing the list

/* Layout */
#define Cfg_DEFAULT_COLUMNS		Lay_HIDE_BOTH_COLUMNS

/*****************************************************************************/
/*********************** Directories, folder and files ***********************/
/*****************************************************************************/

/*                                                                      /
                                                                        |
                                                                        |
                                                                        |
                                                                       var
                                                                        |
                          +---------------------------------------------+---------------------------------------------------+
                          |                                                                                                 |
                         www                                                                                               lib
                          |                                                                                                 |
   +----------------------+--------------------------------------------------+                                              |
   |                      |                                                  |                                              |
 cgi-bin                 html (public)                                     swad (private)                                  mysql (HD 15000 rpm)
   |                      |                                                  |                                              |
   |         +---+----+---+-------------+------------------+---+        +----+-+----+-----+----+----+---------+----+        +
   |         |   |    |                 |                  |   |        |      |    |     |    |    |         |    |        |
 swad (CGI) crs chat photo             icon               tmp logo     crs    out param photo test tmp       usr  zip   swad (database)
             |        |                 |                  |            |                 |         |         |
             |  +-----+---+   +-----+---+--+------+        +            |                tmp      +-+-+   +-+-+---+
             |  |         |   |     |   |  |      |        |            |                         | | |   | | |...|
             | links     tmp  |  filext | plugin  |    tmp.dirs.        |                       tmp.dirs. 0 1 2   9
             |            |  degree institution theme      |            |                           |     | | |...|
             |            +                                +            |                         +-+-+         +-+-+
             |          processing                       links          |                         | | |         | | |
             |          of                               to             |                        tmp. links      xxxxxxx9
             |          received                         download       |                        to                 |
             |          photos                           files          |                        assignments        +
             |                                                          |                        and                |
             |                                                          |                        works           maletin
           +-+-+                                                      +-+-+                      for
           | | |...                                                   | | |                      compression
         course code                                               course code
             |                                                          |
        +----+----+              +----------+------+-------------+------+---+-----------+------------+
        |...      |              |          |      |             |          |           |            |
   bibliografia  rss       calificaciones comun descarga        grp        pra         lec          usr
                  +            +-+-+      +-+-+  +-+-+       +---+---+      +           +          +-+--------+
                  |            | | |      | | |  | | |       |       |      |           |          |          |
              index.rss         ...        ...    ...      cód.    cód. syllabus.xml syllabus.xml  ID         ID
                                                             +       +                        +----+---+
                                                             |       |                        |        |
                                                           comun   comun                actividades trabajos
                                                           +-+-+   +-+-+                    +-+-+    +-+-+
                                                           | | |   | | |                    | | |    | | |
                                                            ...     ...                      ...      ...
*/

/* Config file */
#define Cfg_FILE_CONFIG				"swad.cfg"
#define Cfg_MAX_BYTES_DATABASE_PASSWORD		256
#define Cfg_MAX_BYTES_SMTP_PASSWORD		256

/* Folders for institutions, inside public and private swad directories */
#define Cfg_FOLDER_INS				"ins"			// If not exists, it should be created during installation inside swad private and public directory!
#define Cfg_PATH_INS_PRIVATE			Cfg_PATH_SWAD_PRIVATE "/" Cfg_FOLDER_INS
#define Cfg_PATH_INS_PUBLIC			Cfg_PATH_SWAD_PUBLIC "/" Cfg_FOLDER_INS

/* Folders for centers, inside public and private swad directories */
#define Cfg_FOLDER_CTR				"ctr"			// If not exists, it should be created during installation inside swad private and public directory!
#define Cfg_PATH_CTR_PRIVATE			Cfg_PATH_SWAD_PRIVATE "/" Cfg_FOLDER_CTR
#define Cfg_PATH_CTR_PUBLIC			Cfg_PATH_SWAD_PUBLIC "/" Cfg_FOLDER_CTR
#define Cfg_URL_CTR_PUBLIC			Cfg_URL_SWAD_PUBLIC "/" Cfg_FOLDER_CTR

/* Folders for centers, inside public and private swad directories */
#define Cfg_FOLDER_DEG				"deg"			// If not exists, it should be created during installation inside swad private and public directory!
#define Cfg_PATH_DEG_PRIVATE			Cfg_PATH_SWAD_PRIVATE "/" Cfg_FOLDER_DEG
#define Cfg_PATH_DEG_PUBLIC			Cfg_PATH_SWAD_PUBLIC "/" Cfg_FOLDER_DEG

/* Folders for courses, inside public and private swad directories */
#define Cfg_FOLDER_CRS 				"crs"			// If not exists, it should be created during installation inside swad private and public directory!
#define Cfg_PATH_CRS_PRIVATE			Cfg_PATH_SWAD_PRIVATE "/" Cfg_FOLDER_CRS
#define Cfg_PATH_CRS_PUBLIC			Cfg_PATH_SWAD_PUBLIC "/" Cfg_FOLDER_CRS
#define Cfg_URL_CRS_PUBLIC			Cfg_URL_SWAD_PUBLIC "/" Cfg_FOLDER_CRS

/* Folders for groups, inside public and private swad directories */
#define Cfg_FOLDER_GRP 				"grp"			// Created automatically the first time it is accessed

/* Folder for users, inside private swad directory */
#define Cfg_FOLDER_USR 				"usr"			// Created automatically the first time it is accessed
#define Cfg_PATH_USR_PRIVATE			Cfg_PATH_SWAD_PRIVATE "/" Cfg_FOLDER_USR

/* Folders for projects, inside public and private swad directories */
#define Cfg_FOLDER_PRJ 				"prj"			// Created automatically the first time it is accessed

/* Folder for temporary HTML output of this CGI, inside private swad directory */
#define Cfg_FOLDER_OUT 				"out"			// Created automatically the first time it is accessed
#define Cfg_PATH_OUT_PRIVATE			Cfg_PATH_SWAD_PRIVATE "/" Cfg_FOLDER_OUT

/* Folder for temporary public links to file zones, used when displaying file browsers, inside public swad directory */
#define Cfg_FOLDER_FILE_BROWSER_TMP		"tmp"			// Created automatically the first time it is accessed
#define Cfg_PATH_FILE_BROWSER_TMP_PUBLIC	Cfg_PATH_SWAD_PUBLIC "/" Cfg_FOLDER_FILE_BROWSER_TMP
#define Cfg_URL_FILE_BROWSER_TMP_PUBLIC		Cfg_URL_SWAD_PUBLIC "/" Cfg_FOLDER_FILE_BROWSER_TMP

/* Folder where temporary files are created for students' marks, inside private swad directory */
#define Cfg_FOLDER_MARK				"mark"			// Created automatically the first time it is accessed
#define Cfg_PATH_MARK_PRIVATE			Cfg_PATH_SWAD_PRIVATE "/" Cfg_FOLDER_MARK

/* Folder for temporary XML files received to import test questions, inside private swad directory */
#define Cfg_FOLDER_TEST				"test"			// Created automatically the first time it is accessed
#define Cfg_PATH_TEST_PRIVATE			Cfg_PATH_SWAD_PRIVATE "/" Cfg_FOLDER_TEST

/* Folder for compression of assignments and works into a zip files, inside private swad directory */
#define Cfg_FOLDER_ZIP				"zip"			// Created automatically the first time it is accessed
#define Cfg_PATH_ZIP_PRIVATE			Cfg_PATH_SWAD_PRIVATE "/" Cfg_FOLDER_ZIP

/* Folders for images/videos inside public and private swad directories */
#define Cfg_FOLDER_MEDIA			"med"			// Created automatically the first time it is accessed
#define Cfg_PATH_MEDIA_PRIVATE			Cfg_PATH_SWAD_PRIVATE "/" Cfg_FOLDER_MEDIA
/* Folders for temporary images/videos inside media directories */
#define Cfg_FOLDER_MEDIA_TMP			"tmp"			// Created automatically the first time it is accessed
#define Cfg_PATH_MEDIA_TMP_PRIVATE		Cfg_PATH_MEDIA_PRIVATE "/" Cfg_FOLDER_MEDIA_TMP

/* Folders for users' photos inside public and private swad directories */
#define Cfg_FOLDER_PHOTO			"photo"			// Created automatically the first time it is accessed
#define Cfg_PATH_PHOTO_PRIVATE			Cfg_PATH_SWAD_PRIVATE "/" Cfg_FOLDER_PHOTO
#define Cfg_PATH_PHOTO_PUBLIC			Cfg_PATH_SWAD_PUBLIC "/" Cfg_FOLDER_PHOTO
#define Cfg_URL_PHOTO_PUBLIC			Cfg_URL_SWAD_PUBLIC "/" Cfg_FOLDER_PHOTO
/* Folders for temporary users' photos inside photos directories */
#define Cfg_FOLDER_PHOTO_TMP			"tmp"			// Created automatically the first time it is accessed
#define Cfg_PATH_PHOTO_TMP_PRIVATE		Cfg_PATH_PHOTO_PRIVATE "/" Cfg_FOLDER_PHOTO_TMP
#define Cfg_PATH_PHOTO_TMP_PUBLIC		Cfg_PATH_PHOTO_PUBLIC "/" Cfg_FOLDER_PHOTO_TMP
#define Cfg_URL_PHOTO_TMP_PUBLIC		Cfg_URL_PHOTO_PUBLIC "/" Cfg_FOLDER_PHOTO_TMP

/* Folder for reports, inside public swad directory */
#define Cfg_FOLDER_REP 				"rep"			// Created automatically the first time it is accessed
#define Cfg_PATH_REP_PUBLIC			Cfg_PATH_SWAD_PUBLIC "/" Cfg_FOLDER_REP
#define Cfg_URL_REP_PUBLIC			Cfg_URL_SWAD_PUBLIC "/" Cfg_FOLDER_REP

/* Folder for banners, inside public swad directory */
#define Cfg_FOLDER_BANNER			"banner"		// If not exists, it should be created during installation inside swad public directory!
#define Cfg_URL_BANNER_PUBLIC			Cfg_URL_SWAD_PUBLIC "/" Cfg_FOLDER_BANNER

#define Cfg_FOLDER_DEGREE_PHOTO_MEDIAN		"mdn_all"
#define Cfg_FOLDER_DEGREE_PHOTO_AVERAGE		"avg_all"

/* Folders with icons. They should be created with icons during installation */
#define Cfg_FOLDER_PUBLIC_ICON			"icon"
#define Cfg_PATH_ICON_PUBLIC			Cfg_PATH_SWAD_PUBLIC "/" Cfg_FOLDER_PUBLIC_ICON
#define Cfg_URL_ICON_PUBLIC			Cfg_URL_SWAD_PUBLIC "/" Cfg_FOLDER_PUBLIC_ICON

#define Cfg_ICON_FOLDER_THEMES			"theme"		// Directory with all images for each theme
#define Cfg_URL_ICON_THEMES_PUBLIC		Cfg_URL_ICON_PUBLIC "/" Cfg_ICON_FOLDER_THEMES

#define Cfg_ICON_FOLDER_SETS			"iconset"	// Directory with all icons for each icon set
#define Cfg_PATH_ICON_SETS_PUBLIC		Cfg_PATH_ICON_PUBLIC "/" Cfg_ICON_FOLDER_SETS
#define Cfg_URL_ICON_SETS_PUBLIC		Cfg_URL_ICON_PUBLIC "/" Cfg_ICON_FOLDER_SETS

#define Cfg_ICON_FOLDER_COUNTRIES		"country"	// Directory with icons for countries
#define Cfg_PATH_ICON_COUNTRIES_PUBLIC		Cfg_PATH_ICON_PUBLIC "/" Cfg_ICON_FOLDER_COUNTRIES
#define Cfg_URL_ICON_COUNTRIES_PUBLIC		Cfg_URL_ICON_PUBLIC "/" Cfg_ICON_FOLDER_COUNTRIES

#define Cfg_ICON_FOLDER_FILEXT			"filext"	// Directory with icons for file extensions
#define CfG_URL_ICON_FILEXT_PUBLIC		Cfg_URL_ICON_PUBLIC "/" Cfg_ICON_FOLDER_FILEXT

#define Cfg_ICON_FOLDER_PLUGINS			"plugin"	// Directory with icons for plugins
#define Cfg_URL_ICON_PLUGINS_PUBLIC		Cfg_URL_ICON_PUBLIC "/" Cfg_ICON_FOLDER_PLUGINS

/* Folders and file used to store syllabus */
#define Cfg_SYLLABUS_FOLDER_LECTURES		"lec"
#define Cfg_SYLLABUS_FOLDER_PRACTICALS		"pra"
#define Cfg_SYLLABUS_FILENAME			"syllabus.xml"

/* Main folders in file browsers */
#define Cfg_CRS_INFO_INFORMATION		"intro"		// TODO: Change to inf
#define Cfg_CRS_INFO_TEACHING_GUIDE		"description"	// TODO: Change to gui
#define Cfg_CRS_INFO_LECTURES			"teoria"	// TODO: Change to lec
#define Cfg_CRS_INFO_PRACTICALS			"practicas"	// TODO: Change to pra
#define Cfg_CRS_INFO_BIBLIOGRAPHY		"bibliografia"	// TODO: Change to bib
#define Cfg_CRS_INFO_FAQ			"FAQ"		// TODO: Change to faq
#define Cfg_CRS_INFO_LINKS			"enlaces"	// TODO: Change to lnk
#define Cfg_CRS_INFO_ASSESSMENT			"evaluacion"	// TODO: Change to ass

/* RSS folders and files */
#define Cfg_RSS_FOLDER				"rss"
#define Cfg_RSS_FILE				"rss.xml"
#define Cfg_RSS_LANGUAGE			"es-es"

/* Data protection policy folder
   This folder, and an index.html file inside it, must be created during installation */
#define Cfg_DATA_PROTECTION_FOLDER		"data_protection"
#define Cfg_URL_DATA_PROTECTION_PUBLIC		Cfg_URL_SWAD_PUBLIC "/" Cfg_DATA_PROTECTION_FOLDER

/* HTML file weitten at foot of page */
#define Cfg_PATH_AND_FILE_REL_HTML_PRIVATE	"./foot.html"

/* Comment the following line if you do not want a local copy of MathJax */
#define Cfg_MATHJAX_LOCAL

/*****************************************************************************/
/************************ Commands called by this CGI ************************/
/*****************************************************************************/

/* Template for chat window. It should be copied during installation */
#define Cfg_PATH_AND_FILE_REL_CHAT_PRIVATE		"./chat.html"

/* Command to make image processing / face detection, programmed by Daniel J. Calandria-Hernández */
// %s must be substituted by temporary file with the image received:
#define Cfg_COMMAND_FACE_DETECTION			"./fotomaton cascade.xml %s 540"

/* Commands to compute the average photo of a degree */
#define Cfg_COMMAND_DEGREE_PHOTO_MEDIAN			"./foto_mediana"
#define Cfg_COMMAND_DEGREE_PHOTO_AVERAGE		"./foto_promedio"

/* Command to send automatic emails, programmed by Antonio F. Díaz-García and Antonio Cañas-Vargas */
#define Cfg_COMMAND_SEND_AUTOMATIC_EMAIL		"./swad_smtp.py"

/*****************************************************************************/
/******************************** Time periods *******************************/
/*****************************************************************************/

#define Cfg_LOG_START_MONTH	  01	// Month where the log started in the database
#define Cfg_LOG_START_DAY	  01	// Day   where the log started in the database

#define Cfg_DAYS_IN_RECENT_LOG				 15	// Only accesses in these last days + 1 are stored in recent log.
								// Important!!! Must be 1 <= Cfg_DAYS_IN_RECENT_LOG <= 29
#define Cfg_TIMES_PER_SECOND_REFRESH_CONNECTED		  2	// Execute this CGI to refresh connected users about these times per second
#define Cfg_MIN_TIME_TO_REFRESH_CONNECTED		((time_t)(                     60UL))	// Refresh period of connected users in seconds
#define Cfg_MAX_TIME_TO_REFRESH_CONNECTED		((time_t)(              15UL * 60UL))	// Refresh period of connected users in seconds
#define Cfg_TIME_TO_CLOSE_SESSION_FROM_LAST_REFRESH	((time_t)(Cfg_MAX_TIME_TO_REFRESH_CONNECTED * 4))	// After these seconds without refresh of connected users, session is closed
#define Cfg_TIME_TO_CLOSE_SESSION_FROM_LAST_CLICK	((time_t)(          8 * 60UL * 60UL))	// After these seconds without user's clicks, session is closed

#define Cfg_TIME_TO_REFRESH_TIMELINE			((time_t)(             2UL * 1000UL))	// Initial refresh period of social timeline in miliseconds
												// This delay is increased 1 second on each refresh

#define Cfg_SECONDS_TO_REFRESH_MATCH_TCH		1					// Refresh period of match being played in seconds (for teachers)
#define Cfg_SECONDS_TO_REFRESH_MATCH_STD		3					// Refresh period of match being played in seconds (for students)
#define Cfg_TIME_TO_REFRESH_MATCH_TCH			((time_t)(Cfg_SECONDS_TO_REFRESH_MATCH_TCH * 1000UL))	// Refresh period of match being played in miliseconds (for teachers)
#define Cfg_TIME_TO_REFRESH_MATCH_STD			((time_t)(Cfg_SECONDS_TO_REFRESH_MATCH_STD * 1000UL))	// Refresh period of match being played in miliseconds (for students)

#define Cfg_TIME_TO_REFRESH_LAST_CLICKS			((time_t)(             1UL * 1000UL))	// Refresh period of last clicks in miliseconds

#define Cfg_TIME_TO_CHANGE_BANNER			((time_t)(               2UL * 60UL))	// After these seconds, change banner
#define Cfg_NUMBER_OF_BANNERS				1					// Number of banners to show simultaneously

#define Cfg_TIME_TO_DELETE_API_KEY		((time_t)( 7UL * 24UL * 60UL * 60UL))	// After these seconds, a web service key is removed

#define Cfg_TIME_TO_DELETE_HTML_OUTPUT			((time_t)(              30UL * 60UL))	// Remove the HTML output files older than these seconds

#define Cfg_TIME_TO_ABORT_FILE_UPLOAD			((time_t)(              55UL * 60UL))	// After these seconds uploading data, abort upload.

#define Cfg_TIME_TO_DELETE_BROWSER_TMP_FILES		((time_t)(        2UL * 60UL * 60UL))  	// Temporary files are deleted after these seconds
#define Cfg_TIME_TO_DELETE_BROWSER_EXPANDED_FOLDERS	((time_t)( 7UL * 24UL * 60UL * 60UL))	// Past these seconds, remove expired expanded folders
#define Cfg_TIME_TO_DELETE_BROWSER_ZIP_FILES		((time_t)(        2UL * 60UL * 60UL))  	// Temporary zip files are deleted after these seconds
#define Cfg_TIME_TO_DELETE_BROWSER_CLIPBOARD		((time_t)(              30UL * 60UL))	// Paths older than these seconds are removed from clipboard

#define Cfg_TIME_TO_DELETE_USER_CLIPBOARD		((time_t)(              30UL * 60UL))	// User clipboards older than these seconds are removed from clipboard

#define Cfg_TIME_TO_DELETE_MARKS_TMP_FILES		((time_t)(        2UL * 60UL * 60UL))  	// Temporary files with students' marks are deleted after these seconds

#define Cfg_TIME_TO_DELETE_PARAMS_TO_COMMANDS		((time_t)(              15UL * 60UL))  	// Temporary files with params to commands are deleted after these seconds

#define Cfg_TIME_TO_DELETE_MEDIA_TMP_FILES		((time_t)(        2UL * 60UL * 60UL))  	// Temporary files related to images after these seconds
#define Cfg_TIME_TO_DELETE_PHOTOS_TMP_FILES		((time_t)(        2UL * 60UL * 60UL))  	// Temporary files related to photos after these seconds

#define Cfg_TIME_TO_DELETE_TEST_TMP_FILES		((time_t)(        2UL * 60UL * 60UL))  	// Temporary files related to imported test questions after these seconds

#define Cfg_TIME_TO_DELETE_ENROLMENT_REQUESTS		((time_t)(30UL * 24UL * 60UL * 60UL))	// Past these seconds, remove expired enrolment requests

#define Cfg_TIME_TO_DELETE_THREAD_CLIPBOARD		((time_t)(              30UL * 60UL))	// Threads older than these seconds are removed from clipboard

#define Cfg_TIME_TO_DELETE_OLD_PENDING_PASSWORDS 	((time_t)(       24UL * 60UL * 60UL))	// Past these seconds, remove expired pending passwords
#define Cfg_TIME_TO_DELETE_OLD_PENDING_EMAILS	 	((time_t)(       24UL * 60UL * 60UL))	// Past these seconds, remove expired pending emails

#define Cfg_TIME_TO_DELETE_IP_PREFS			((time_t)( 7UL * 24UL * 60UL * 60UL))	// Past these seconds, remove expired IP settings

#define Cfg_TIME_TO_DELETE_OLD_NOTIF 			((time_t)(30UL * 24UL * 60UL * 60UL))	// Past these seconds, remove expired notifications

#define Cfg_MIN_TIME_TO_RECOMPUTE_AVG_PHOTO		((time_t)(          0 * 60UL * 60UL))	// After these seconds, users can recompute the average photos of a degree

#define Cfg_MAX_TIME_TO_REMEMBER_LAST_ACTION_ON_LOGIN	((time_t)(        2UL * 60UL * 60UL))	// On login, if interval since last action is less than this time, remember last action

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void Cfg_GetConfigFromFile (void);

const char *Cfg_GetDatabasePassword (void);
const char *Cfg_GetSMTPPassword (void);

#endif
