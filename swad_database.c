// swad_database.c: database

/*
    SWAD (Shared Workspace At a Distance),
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
/*********************************** Headers *********************************/
/*****************************************************************************/

#define _GNU_SOURCE 		// For vasprintf
#include <mysql/mysql.h>	// To access MySQL databases
#include <stdarg.h>		// For va_start, va_end
#include <stddef.h>		// For NULL
#include <stdio.h>		// For FILE, vasprintf
#include <stdlib.h>		// For free
#include <string.h>		// For strlen

#include "swad_alert.h"
#include "swad_config.h"
#include "swad_database.h"
#include "swad_error.h"
#include "swad_global.h"
#include "swad_HTML.h"
#include "swad_language.h"

/*****************************************************************************/
/************************* Private global variables **************************/
/*****************************************************************************/

static struct
  {
   MYSQL mysql;
   CloOpe_ClosedOrOpen_t IsOpen;
   bool ThereAreLockedTables;
  } DB_Database =
  {
   .IsOpen = CloOpe_CLOSED,
   .ThereAreLockedTables = false,
  };

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void DB_CreateTable (const char *Query);
static unsigned long DB_QuerySELECTusingQueryStr (char *Query,
					          MYSQL_RES **mysql_res,
						  const char *MsgError);

/*****************************************************************************/
/***************************** Database tables *******************************/
/*****************************************************************************/

/*
Database:
shell> mysql -h swad.ugr.es -u swad -p
mysql> CREATE DATABASE IF NOT EXIST swad;
mysql> SHOW databases;
mysql> USE swad
mysql> SHOW tables;
mysql> DROP TABLE table_to_delete;							// Delete a table
mysql> TRUNCATE TABLE table_to_delete;							// Delete all rows
shell> mysqldump -l -h swad.ugr.es -u swad -p swad > copia_base_datos_swad		// Backup of the whole database
mysql> UPDATE table SET CrsCod='new_code' WHERE CrsCod='old_code';			// Change a course code
mysql> ALTER TABLE table ADD COLUMN new_column_definition AFTER Columna;		// Add a new column to a table
mysql> ALTER TABLE table RENAME TO new_table_name;					// Rename a table
MYSQL> ALTER TABLE table MODIFY COLUMN new_column_definition;
mysql> UPDATE msg_content SET Content=REPLACE(Content,'<BR>','<br />');			// Change <BR> by <br /> in messages
*/

/*****************************************************************************/
/****************** Create database tables if not exist **********************/
/*****************************************************************************/

void DB_CreateTablesIfNotExist (void)
  {
   extern const char *Txt_Creating_database_tables_if_they_do_not_exist;
   extern const char *Txt_Created_tables_in_the_database_that_did_not_exist;

   /***** Information message *****/
   Ale_ShowAlert (Ale_INFO,Txt_Creating_database_tables_if_they_do_not_exist);
   HTM_OL_Begin ();

   /***** Table act_frequent *****/
/*
mysql> DESCRIBE act_frequent;
+-----------+----------+------+-----+---------+-------+
| Field     | Type     | Null | Key | Default | Extra |
+-----------+----------+------+-----+---------+-------+
| UsrCod    | int(11)  | NO   | PRI | NULL    |       |
| ActCod    | int(11)  | NO   | PRI | NULL    |       |
| Score     | float    | NO   |     | NULL    |       |
| LastClick | datetime | NO   |     | NULL    |       |
+-----------+----------+------+-----+---------+-------+
4 rows in set (0.01 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS act_frequent ("
			"UsrCod INT NOT NULL,"
			"ActCod INT NOT NULL,"
			"Score FLOAT NOT NULL,"
			"LastClick DATETIME NOT NULL,"
		   "UNIQUE INDEX(UsrCod,ActCod)"
		   ") ENGINE=MyISAM");

   /***** Table agd_agendas *****/
/*
mysql> DESCRIBE agd_agendas;
+-----------+---------------+------+-----+---------+----------------+
| Field     | Type          | Null | Key | Default | Extra          |
+-----------+---------------+------+-----+---------+----------------+
| AgdCod    | int(11)       | NO   | PRI | NULL    | auto_increment |
| UsrCod    | int(11)       | NO   | MUL | NULL    |                |
| Public    | enum('N','Y') | NO   |     | N       |                |
| Hidden    | enum('N','Y') | NO   |     | N       |                |
| StartTime | datetime      | NO   |     | NULL    |                |
| EndTime   | datetime      | NO   |     | NULL    |                |
| Event     | varchar(2047) | NO   |     | NULL    |                |
| Location  | varchar(2047) | NO   |     | NULL    |                |
| Txt       | text          | NO   |     | NULL    |                |
+-----------+---------------+------+-----+---------+----------------+
9 rows in set (0.00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS agd_agendas ("
			"AgdCod INT NOT NULL AUTO_INCREMENT,"
			"UsrCod INT NOT NULL,"
			"Public ENUM('N','Y') NOT NULL DEFAULT 'N',"
			"Hidden ENUM('N','Y') NOT NULL DEFAULT 'N',"
			"StartTime DATETIME NOT NULL,"
			"EndTime DATETIME NOT NULL,"
			"Event VARCHAR(2047) NOT NULL,"		// Agd_MAX_BYTES_EVENT
			"Location VARCHAR(2047) NOT NULL,"	// Agd_MAX_BYTES_LOCATION
			"Txt TEXT NOT NULL,"			// Cns_MAX_BYTES_TEXT
		   "UNIQUE INDEX(AgdCod),"
		   "INDEX(UsrCod,Public,Hidden)"
		   ") ENGINE=MyISAM");

   /***** Table ann_announcements *****/
/*
mysql> DESCRIBE ann_announcements;
+---------+------------+------+-----+---------+----------------+
| Field   | Type       | Null | Key | Default | Extra          |
+---------+------------+------+-----+---------+----------------+
| AnnCod  | int(11)    | NO   | PRI | NULL    | auto_increment |
| Status  | tinyint(4) | NO   | MUL | 0       |                |
| Roles   | int(11)    | NO   |     | 0       |                |
| Subject | text       | NO   |     | NULL    |                |
| Content | text       | NO   |     | NULL    |                |
+---------+------------+------+-----+---------+----------------+
5 rows in set (0.00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS ann_announcements ("
			"AnnCod INT NOT NULL AUTO_INCREMENT,"
			"Status TINYINT NOT NULL DEFAULT 0,"
			"Roles INT NOT NULL DEFAULT 0,"
			"Subject TEXT NOT NULL,"	// Cns_MAX_BYTES_SUBJECT
			"Content TEXT NOT NULL,"	// Cns_MAX_BYTES_TEXT
		   "UNIQUE INDEX(AnnCod),"
		   "INDEX(Status)"
		   ") ENGINE=MyISAM");

   /***** Table ann_seen *****/
/*
mysql> DESCRIBE ann_seen;
+--------+---------+------+-----+---------+-------+
| Field  | Type    | Null | Key | Default | Extra |
+--------+---------+------+-----+---------+-------+
| AnnCod | int(11) | NO   | PRI | NULL    |       |
| UsrCod | int(11) | NO   | PRI | NULL    |       |
+--------+---------+------+-----+---------+-------+
2 rows in set (0.00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS ann_seen ("
			"AnnCod INT NOT NULL,"
			"UsrCod INT NOT NULL,"
		   "UNIQUE INDEX(AnnCod,UsrCod)"
		   ") ENGINE=MyISAM");

/***** Table api_keys *****/
/*
mysql> DESCRIBE api_keys;
+----------+----------+------+-----+---------+-------+
| Field    | Type     | Null | Key | Default | Extra |
+----------+----------+------+-----+---------+-------+
| WSKey    | char(43) | NO   | PRI | NULL    |       |
| UsrCod   | int(11)  | NO   | MUL | NULL    |       |
| PlgCod   | int(11)  | NO   | MUL | NULL    |       |
| LastTime | datetime | NO   | MUL | NULL    |       |
+----------+----------+------+-----+---------+-------+
4 rows in set (0.00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS api_keys ("
			"WSKey CHAR(43) NOT NULL,"	// API_BYTES_WS_KEY
			"UsrCod INT NOT NULL,"
			"PlgCod INT NOT NULL,"
			"LastTime DATETIME NOT NULL,"
		   "UNIQUE INDEX(WSKey),"
		   "INDEX(UsrCod),"
		   "INDEX(PlgCod),"
		   "INDEX(LastTime)"
		   ") ENGINE=MyISAM");

   /***** Table asg_assignments *****/
/*
mysql> DESCRIBE asg_assignments;
+-----------+----------------+------+-----+---------+----------------+
| Field     | Type           | Null | Key | Default | Extra          |
+-----------+----------------+------+-----+---------+----------------+
| AsgCod    | int(11)        | NO   | PRI | NULL    | auto_increment |
| CrsCod    | int(11)        | NO   | MUL | -1      |                |
| Hidden    | enum('N','Y')  | NO   |     | N       |                |
| NumNotif  | int(11)        | NO   |     | 0       |                |
| UsrCod    | int(11)        | NO   |     | NULL    |                |
| StartTime | datetime       | NO   |     | NULL    |                |
| EndTime   | datetime       | NO   |     | NULL    |                |
| Title     | varchar(2047)  | NO   |     | NULL    |                |
| Folder    | varbinary(255) | NO   |     | NULL    |                |
| Txt       | text           | NO   |     | NULL    |                |
+-----------+----------------+------+-----+---------+----------------+
10 rows in set (0.00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS asg_assignments ("
			"AsgCod INT NOT NULL AUTO_INCREMENT,"
			"CrsCod INT NOT NULL DEFAULT -1,"
			"Hidden ENUM('N','Y') NOT NULL DEFAULT 'N',"
			"NumNotif INT NOT NULL DEFAULT 0,"
			"UsrCod INT NOT NULL,"
			"StartTime DATETIME NOT NULL,"
			"EndTime DATETIME NOT NULL,"
			"Title VARCHAR(2047) NOT NULL,"		// Asg_MAX_BYTES_ASSIGNMENT_TITLE
			"Folder VARBINARY(255) NOT NULL,"	// Brw_MAX_BYTES_FOLDER
			"Txt TEXT NOT NULL,"			// Cns_MAX_BYTES_TEXT
		   "UNIQUE INDEX(AsgCod),"
		   "INDEX(CrsCod,Hidden)"
		   ") ENGINE=MyISAM");

   /***** Table asg_groups *****/
/*
mysql> DESCRIBE asg_groups;
+--------+---------+------+-----+---------+-------+
| Field  | Type    | Null | Key | Default | Extra |
+--------+---------+------+-----+---------+-------+
| AsgCod | int(11) | NO   | PRI | NULL    |       |
| GrpCod | int(11) | NO   | PRI | NULL    |       |
+--------+---------+------+-----+---------+-------+
2 rows in set (0.00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS asg_groups ("
			"AsgCod INT NOT NULL,"
			"GrpCod INT NOT NULL,"
		   "UNIQUE INDEX(AsgCod,GrpCod)"
		   ") ENGINE=MyISAM");

   /***** Table asg_rubrics *****/
/*
mysql> DESCRIBE asg_rubrics;
+--------+---------+------+-----+---------+-------+
| Field  | Type    | Null | Key | Default | Extra |
+--------+---------+------+-----+---------+-------+
| AsgCod | int(11) | NO   | PRI | NULL    |       |
| RubCod | int(11) | NO   |     | NULL    |       |
+--------+---------+------+-----+---------+-------+
2 rows in set (0.00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS asg_rubrics ("
			"AsgCod INT NOT NULL,"
			"RubCod INT NOT NULL,"
		   "UNIQUE INDEX(AsgCod)"
		   ") ENGINE=MyISAM");

   /***** Table att_events *****/
/*
mysql> DESCRIBE att_events;
+-------------------+---------------+------+-----+---------+----------------+
| Field             | Type          | Null | Key | Default | Extra          |
+-------------------+---------------+------+-----+---------+----------------+
| AttCod            | int(11)       | NO   | PRI | NULL    | auto_increment |
| CrsCod            | int(11)       | NO   | MUL | -1      |                |
| Hidden            | enum('N','Y') | NO   |     | N       |                |
| UsrCod            | int(11)       | NO   |     | NULL    |                |
| StartTime         | datetime      | NO   |     | NULL    |                |
| EndTime           | datetime      | NO   |     | NULL    |                |
| CommentTchVisible | enum('N','Y') | NO   |     | N       |                |
| Title             | varchar(2047) | NO   |     | NULL    |                |
| Txt               | text          | NO   |     | NULL    |                |
+-------------------+---------------+------+-----+---------+----------------+
9 rows in set (0.00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS att_events ("
			"AttCod INT NOT NULL AUTO_INCREMENT,"
			"CrsCod INT NOT NULL DEFAULT -1,"
			"Hidden ENUM('N','Y') NOT NULL DEFAULT 'N',"
			"UsrCod INT NOT NULL,"
			"StartTime DATETIME NOT NULL,"
			"EndTime DATETIME NOT NULL,"
			"CommentTchVisible ENUM('N','Y') NOT NULL DEFAULT 'N',"
			"Title VARCHAR(2047) NOT NULL,"	// Att_MAX_BYTES_ATTENDANCE_EVENT_TITLE
			"Txt TEXT NOT NULL,"		// Cns_MAX_BYTES_TEXT
		   "UNIQUE INDEX(AttCod),"
		   "INDEX(CrsCod,Hidden)"
		   ") ENGINE=MyISAM");

   /***** Table att_groups *****/
/*
mysql> DESCRIBE att_groups;
+--------+---------+------+-----+---------+-------+
| Field  | Type    | Null | Key | Default | Extra |
+--------+---------+------+-----+---------+-------+
| AttCod | int(11) | NO   | PRI | NULL    |       |
| GrpCod | int(11) | NO   | PRI | NULL    |       |
+--------+---------+------+-----+---------+-------+
2 rows in set (0.00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS att_groups ("
			"AttCod INT NOT NULL,"
			"GrpCod INT NOT NULL,"
		   "UNIQUE INDEX(AttCod,GrpCod)"
		   ") ENGINE=MyISAM");

   /***** Table att_users *****/
/*
mysql> DESCRIBE att_users;
+------------+---------------+------+-----+---------+-------+
| Field      | Type          | Null | Key | Default | Extra |
+------------+---------------+------+-----+---------+-------+
| AttCod     | int(11)       | NO   | PRI | NULL    |       |
| UsrCod     | int(11)       | NO   | PRI | NULL    |       |
| Present    | enum('N','Y') | NO   |     | Y       |       |
| CommentStd | text          | NO   |     | NULL    |       |
| CommentTch | text          | NO   |     | NULL    |       |
+------------+---------------+------+-----+---------+-------+
5 rows in set (0.00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS att_users ("
			"AttCod INT NOT NULL,"
			"UsrCod INT NOT NULL,"
			"Present ENUM('N','Y') NOT NULL DEFAULT 'Y',"
			"CommentStd TEXT NOT NULL,"	// Cns_MAX_BYTES_TEXT
			"CommentTch TEXT NOT NULL,"	// Cns_MAX_BYTES_TEXT
		   "UNIQUE INDEX(AttCod,UsrCod),"
		   "INDEX(UsrCod)"
		   ") ENGINE=MyISAM");

   /***** Table ban_banners *****/
/*
mysql> DESCRIBE ban_banners;
+-----------+---------------+------+-----+---------+----------------+
| Field     | Type          | Null | Key | Default | Extra          |
+-----------+---------------+------+-----+---------+----------------+
| BanCod    | int(11)       | NO   | PRI | NULL    | auto_increment |
| Hidden    | enum('N','Y') | NO   | MUL | N       |                |
| ShortName | varchar(511)  | NO   |     | NULL    |                |
| FullName  | varchar(2047) | NO   |     | NULL    |                |
| Img       | varchar(255)  | NO   |     | NULL    |                |
| WWW       | varchar(255)  | NO   |     | NULL    |                |
+-----------+---------------+------+-----+---------+----------------+
6 rows in set (0.00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS ban_banners ("
			"BanCod INT NOT NULL AUTO_INCREMENT,"
			"Hidden ENUM('N','Y') NOT NULL DEFAULT 'N',"
			"ShortName VARCHAR(511) NOT NULL,"	// Nam_MAX_BYTES_SHRT_NAME
			"FullName VARCHAR(2047) NOT NULL,"	// Nam_MAX_BYTES_FULL_NAME
			"Img VARCHAR(255) NOT NULL,"		// Ban_MAX_BYTES_IMAGE
			"WWW VARCHAR(255) NOT NULL,"		// WWW_MAX_BYTES_WWW
		   "UNIQUE INDEX(BanCod),"
		   "INDEX(Hidden)"
		   ") ENGINE=MyISAM");

   /***** Table bld_buildings *****/
/*
mysql> DESCRIBE bld_buildings;
+-----------+---------------+------+-----+---------+----------------+
| Field     | Type          | Null | Key | Default | Extra          |
+-----------+---------------+------+-----+---------+----------------+
| BldCod    | int(11)       | NO   | PRI | NULL    | auto_increment |
| CtrCod    | int(11)       | NO   | MUL | NULL    |                |
| ShortName | varchar(511)  | NO   |     | NULL    |                |
| FullName  | varchar(2047) | NO   |     | NULL    |                |
| Location  | varchar(2047) | NO   |     | NULL    |                |
+-----------+---------------+------+-----+---------+----------------+
5 rows in set (0.00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS bld_buildings ("
			"BldCod INT NOT NULL AUTO_INCREMENT,"
			"CtrCod INT NOT NULL,"
			"ShortName VARCHAR(511) NOT NULL,"	// Nam_MAX_BYTES_SHRT_NAME
			"FullName VARCHAR(2047) NOT NULL,"	// Nam_MAX_BYTES_FULL_NAME
			"Location VARCHAR(2047) NOT NULL,"	// Bld_MAX_BYTES_LOCATION
		   "UNIQUE INDEX(BldCod),"
		   "INDEX(CtrCod)"
		   ") ENGINE=MyISAM");

   /***** Table brw_caches *****/
/*
mysql> DESCRIBE brw_caches;
+-----------+----------+------+-----+---------+-------+
| Field     | Type     | Null | Key | Default | Extra |
+-----------+----------+------+-----+---------+-------+
| SessionId | char(43) | NO   | MUL | NULL    |       |
| PrivPath  | text     | NO   |     | NULL    |       |
| TmpPubDir | text     | NO   |     | NULL    |       |
+-----------+----------+------+-----+---------+-------+
3 rows in set (0.00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS brw_caches ("
			"SessionId CHAR(43) NOT NULL,"				// Cns_BYTES_SESSION_ID
			"PrivPath VARCHAR(4096) COLLATE latin1_bin NOT NULL,"	// PATH_MAX
			"TmpPubDir VARCHAR(4096) COLLATE latin1_bin NOT NULL,"	// PATH_MAX
		   "UNIQUE INDEX(SessionId)"
		   ") ENGINE=MyISAM");

   /***** Table brw_clipboards *****/
/*
mysql> DESCRIBE brw_clipboards;
+-------------+------------+------+-----+-------------------+-----------------------------+
| Field       | Type       | Null | Key | Default           | Extra                       |
+-------------+------------+------+-----+-------------------+-----------------------------+
| UsrCod      | int(11)    | NO   | PRI | NULL              |                             |
| FileBrowser | tinyint(4) | NO   | MUL | NULL              |                             |
| Cod         | int(11)    | NO   |     | -1                |                             |
| WorksUsrCod | int(11)    | NO   | MUL | NULL              |                             |
| FileType    | tinyint(4) | NO   |     | 0                 |                             |
| Path        | text       | NO   |     | NULL              |                             |
| CopyTime    | timestamp  | NO   |     | CURRENT_TIMESTAMP | on update CURRENT_TIMESTAMP |
+-------------+------------+------+-----+-------------------+-----------------------------+
7 rows in set (0.00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS brw_clipboards ("
			"UsrCod INT NOT NULL,"
			"FileBrowser TINYINT NOT NULL,"
			"Cod INT NOT NULL DEFAULT -1,"
			"WorksUsrCod INT NOT NULL,"
			"FileType TINYINT NOT NULL DEFAULT 0,"
			"Path TEXT COLLATE latin1_bin NOT NULL,"	// PATH_MAX
			"CopyTime TIMESTAMP,"
		   "UNIQUE INDEX(UsrCod),"
		   "INDEX(FileBrowser,Cod),"
		   "INDEX(WorksUsrCod)"
		   ") ENGINE=MyISAM");

   /***** Table brw_expanded *****/
/*
mysql> DESCRIBE brw_expanded;
+-------------+------------+------+-----+---------+-------+
| Field       | Type       | Null | Key | Default | Extra |
+-------------+------------+------+-----+---------+-------+
| UsrCod      | int(11)    | NO   | MUL | NULL    |       |
| FileBrowser | tinyint(4) | NO   | MUL | NULL    |       |
| Cod         | int(11)    | NO   |     | -1      |       |
| WorksUsrCod | int(11)    | NO   | MUL | NULL    |       |
| Path        | text       | NO   |     | NULL    |       |
| ClickTime   | datetime   | NO   |     | NULL    |       |
+-------------+------------+------+-----+---------+-------+
6 rows in set (0.00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS brw_expanded ("
			"UsrCod INT NOT NULL,"
			"FileBrowser TINYINT NOT NULL,"
			"Cod INT NOT NULL DEFAULT -1,"
			"WorksUsrCod INT NOT NULL,"
			"Path TEXT COLLATE latin1_bin NOT NULL,"	// PATH_MAX
			"ClickTime DATETIME NOT NULL,"
		   "INDEX(UsrCod,FileBrowser,Cod),"
		   "INDEX(FileBrowser,Cod),"
		   "INDEX(WorksUsrCod)"
		   ") ENGINE=MyISAM");

   /***** Table brw_files *****/
/*
mysql> DESCRIBE brw_files;
+-----------------+---------------+------+-----+---------+----------------+
| Field           | Type          | Null | Key | Default | Extra          |
+-----------------+---------------+------+-----+---------+----------------+
| FilCod          | int(11)       | NO   | PRI | NULL    | auto_increment |
| FileBrowser     | tinyint(4)    | NO   | MUL | NULL    |                |
| Cod             | int(11)       | NO   |     | -1      |                |
| ZoneUsrCod      | int(11)       | NO   | MUL | -1      |                |
| PublisherUsrCod | int(11)       | NO   | MUL | NULL    |                |
| FileType        | tinyint(4)    | NO   |     | 0       |                |
| Path            | text          | NO   |     | NULL    |                |
| Hidden          | enum('N','Y') | NO   |     | N       |                |
| Public          | enum('N','Y') | NO   |     | N       |                |
| License         | tinyint(4)    | NO   |     | 0       |                |
+-----------------+---------------+------+-----+---------+----------------+
10 rows in set (0.00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS brw_files ("
			"FilCod INT NOT NULL AUTO_INCREMENT,"
			"FileBrowser TINYINT NOT NULL,"
			"Cod INT NOT NULL DEFAULT -1,"
			"ZoneUsrCod INT NOT NULL DEFAULT -1,"
			"PublisherUsrCod INT NOT NULL,"
			"FileType TINYINT NOT NULL DEFAULT 0,"
			"Path TEXT COLLATE latin1_bin NOT NULL,"	// PATH_MAX
			"Hidden ENUM('N','Y') NOT NULL DEFAULT 'N',"
			"Public ENUM('N','Y') NOT NULL DEFAULT 'N',"
			"License TINYINT NOT NULL DEFAULT 0,"
		   "UNIQUE INDEX(FilCod),"
		   "INDEX(FileBrowser,Cod,ZoneUsrCod),"
		   "INDEX(ZoneUsrCod),"
		   "INDEX(PublisherUsrCod)"
		   ") ENGINE=MyISAM");

   /***** Table brw_last *****/
/*
mysql> DESCRIBE brw_last;
+-------------+------------+------+-----+---------+-------+
| Field       | Type       | Null | Key | Default | Extra |
+-------------+------------+------+-----+---------+-------+
| UsrCod      | int(11)    | NO   | PRI | NULL    |       |
| FileBrowser | tinyint(4) | NO   | PRI | NULL    |       |
| Cod         | int(11)    | NO   | PRI | -1      |       |
| LastClick   | datetime   | NO   |     | NULL    |       |
+-------------+------------+------+-----+---------+-------+
4 rows in set (0.00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS brw_last ("
			"UsrCod INT NOT NULL,"
			"FileBrowser TINYINT NOT NULL,"
			"Cod INT NOT NULL DEFAULT -1,"
			"LastClick DATETIME NOT NULL,"
		   "UNIQUE INDEX(UsrCod,FileBrowser,Cod)"
		   ") ENGINE=MyISAM");

   /***** Table brw_sizes *****/
/*
mysql> DESCRIBE brw_sizes;
+-------------+------------+------+-----+---------+-------+
| Field       | Type       | Null | Key | Default | Extra |
+-------------+------------+------+-----+---------+-------+
| FileBrowser | tinyint(4) | NO   | PRI | NULL    |       |
| Cod         | int(11)    | NO   | PRI | -1      |       |
| ZoneUsrCod  | int(11)    | NO   | PRI | -1      |       |
| NumLevels   | int(11)    | NO   |     | NULL    |       |
| NumFolders  | int(11)    | NO   |     | NULL    |       |
| NumFiles    | int(11)    | NO   |     | NULL    |       |
| TotalSize   | bigint(20) | NO   |     | NULL    |       |
+-------------+------------+------+-----+---------+-------+
7 rows in set (0.00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS brw_sizes ("
			"FileBrowser TINYINT NOT NULL,"
			"Cod INT NOT NULL DEFAULT -1,"
			"ZoneUsrCod INT NOT NULL DEFAULT -1,"
			"NumLevels INT NOT NULL,"
			"NumFolders INT NOT NULL,"
			"NumFiles INT NOT NULL,"
			"TotalSize BIGINT NOT NULL,"
		   "UNIQUE INDEX(FileBrowser,Cod,ZoneUsrCod),"
		   "INDEX(ZoneUsrCod)"
		   ") ENGINE=MyISAM");

   /***** Table brw_views *****/
/*
mysql> DESCRIBE brw_views;
+----------+---------+------+-----+---------+-------+
| Field    | Type    | Null | Key | Default | Extra |
+----------+---------+------+-----+---------+-------+
| FilCod   | int(11) | NO   | PRI | NULL    |       |
| UsrCod   | int(11) | NO   | PRI | NULL    |       |
| NumViews | int(11) | NO   |     | 0       |       |
+----------+---------+------+-----+---------+-------+
3 rows in set (0.00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS brw_views ("
			"FilCod INT NOT NULL,"
			"UsrCod INT NOT NULL,"
			"NumViews INT NOT NULL DEFAULT 0,"
		   "UNIQUE INDEX(FilCod,UsrCod),"
		   "INDEX(UsrCod)"
		   ") ENGINE=MyISAM");

   /***** Table cfe_exams *****/
/*
mysql> DESCRIBE cfe_exams;
+-------------+---------------+------+-----+---------+----------------+
| Field       | Type          | Null | Key | Default | Extra          |
+-------------+---------------+------+-----+---------+----------------+
| ExaCod      | int(11)       | NO   | PRI | NULL    | auto_increment |
| CrsCod      | int(11)       | NO   | MUL | -1      |                |
| Status      | tinyint(4)    | NO   | MUL | 0       |                |
| NumNotif    | int(11)       | NO   |     | 0       |                |
| CrsFullName | varchar(2047) | NO   |     | NULL    |                |
| Year        | tinyint(4)    | NO   |     | NULL    |                |
| ExamSession | varchar(2047) | NO   |     | NULL    |                |
| CallDate    | datetime      | NO   |     | NULL    |                |
| ExamDate    | datetime      | NO   |     | NULL    |                |
| Duration    | time          | NO   |     | NULL    |                |
| Place       | text          | NO   |     | NULL    |                |
| ExamMode    | text          | NO   |     | NULL    |                |
| Structure   | text          | NO   |     | NULL    |                |
| DocRequired | text          | NO   |     | NULL    |                |
| MatRequired | text          | NO   |     | NULL    |                |
| MatAllowed  | text          | NO   |     | NULL    |                |
| OtherInfo   | text          | NO   |     | NULL    |                |
+-------------+---------------+------+-----+---------+----------------+
17 rows in set (0.00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS cfe_exams ("
			"ExaCod INT NOT NULL AUTO_INCREMENT,"
			"CrsCod INT NOT NULL DEFAULT -1,"
			"Status TINYINT NOT NULL DEFAULT 0,"
			"NumNotif INT NOT NULL DEFAULT 0,"
			"CrsFullName VARCHAR(2047) NOT NULL,"	// Nam_MAX_BYTES_FULL_NAME
			"Year TINYINT NOT NULL,"
			"ExamSession VARCHAR(2047) NOT NULL,"	// Cfe_MAX_BYTES_SESSION
			"CallDate DATETIME NOT NULL,"
			"ExamDate DATETIME NOT NULL,"
			"Duration TIME NOT NULL,"
			"Place TEXT NOT NULL,"			// Cns_MAX_BYTES_TEXT
			"ExamMode TEXT NOT NULL,"		// Cns_MAX_BYTES_TEXT
			"Structure TEXT NOT NULL,"		// Cns_MAX_BYTES_TEXT
			"DocRequired TEXT NOT NULL,"		// Cns_MAX_BYTES_TEXT
			"MatRequired TEXT NOT NULL,"		// Cns_MAX_BYTES_TEXT
			"MatAllowed TEXT NOT NULL,"		// Cns_MAX_BYTES_TEXT
			"OtherInfo TEXT NOT NULL,"		// Cns_MAX_BYTES_TEXT
		   "UNIQUE INDEX(ExaCod),"
		   "INDEX(CrsCod,Status),"
		   "INDEX(Status)"
		   ") ENGINE=MyISAM");

   /***** Table cht_rooms *****/
/*
mysql> DESCRIBE cht_rooms;
+----------+-------------+------+-----+---------+-------+
| Field    | Type        | Null | Key | Default | Extra |
+----------+-------------+------+-----+---------+-------+
| RoomCode | varchar(16) | NO   | PRI | NULL    |       |
| NumUsrs  | int(11)     | NO   |     | NULL    |       |
+----------+-------------+------+-----+---------+-------+
2 rows in set (0.00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS cht_rooms ("
			"RoomCode VARCHAR(16) NOT NULL,"	// Cht_MAX_BYTES_ROOM_CODE
			"NumUsrs INT NOT NULL,"
		   "UNIQUE INDEX(RoomCode)"
		   ") ENGINE=MyISAM");

   /***** Table crs_bibliography *****/
/*
mysql> DESCRIBE crs_bibliography;
+-----------+---------------+------+-----+---------+----------------+
| Field     | Type          | Null | Key | Default | Extra          |
+-----------+---------------+------+-----+---------+----------------+
| ItmCod    | int(11)       | NO   | PRI | NULL    | auto_increment |
| NodCod    | int(11)       | NO   | MUL | -1      |                |
| ItmInd    | int(11)       | NO   |     | 0       |                |
| Hidden    | enum('N','Y') | NO   |     | N       |                |
| Authors   | varchar(2047) | NO   |     | NULL    |                |
| Title     | varchar(2047) | NO   |     | NULL    |                |
| Source    | varchar(2047) | NO   |     | NULL    |                |
| Publisher | varchar(2047) | NO   |     | NULL    |                |
| Date      | varchar(2047) | NO   |     | NULL    |                |
| Id        | varchar(2047) | NO   |     | NULL    |                |
| URL       | varchar(255)  | NO   |     | NULL    |                |
+-----------+---------------+------+-----+---------+----------------+
11 rows in set (0.00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS crs_bibliography ("
			"ItmCod INT NOT NULL AUTO_INCREMENT,"
			"NodCod INT NOT NULL DEFAULT -1,"
			"ItmInd INT NOT NULL DEFAULT 0,"
			"Hidden ENUM('N','Y') NOT NULL DEFAULT 'N',"
			"Authors VARCHAR(2047) NOT NULL,"	// Bib_MAX_BYTES_TITLE
			"Title VARCHAR(2047) NOT NULL,"		// Bib_MAX_BYTES_TITLE
			"Source VARCHAR(2047) NOT NULL,"	// Bib_MAX_BYTES_TITLE
			"Publisher VARCHAR(2047) NOT NULL,"	// Bib_MAX_BYTES_TITLE
			"Date VARCHAR(2047) NOT NULL,"		// Bib_MAX_BYTES_TITLE
			"Id VARCHAR(2047) NOT NULL,"		// Bib_MAX_BYTES_TITLE
			"URL VARCHAR(255) NOT NULL,"		// WWW_MAX_BYTES_WWW
		   "UNIQUE INDEX(ItmCod),"
		   "UNIQUE INDEX(NodCod,ItmInd)"
		   ") ENGINE=MyISAM");

   /***** Table crs_courses *****/
/*
mysql> DESCRIBE crs_courses;
+-----------------+---------------+------+-----+---------+----------------+
| Field           | Type          | Null | Key | Default | Extra          |
+-----------------+---------------+------+-----+---------+----------------+
| CrsCod          | int(11)       | NO   | PRI | NULL    | auto_increment |
| DegCod          | int(11)       | NO   | MUL | -1      |                |
| Year            | tinyint(4)    | NO   |     | 0       |                |
| InsCrsCod       | char(7)       | NO   |     | NULL    |                |
| Status          | tinyint(4)    | NO   | MUL | 0       |                |
| RequesterUsrCod | int(11)       | NO   |     | -1      |                |
| ShortName       | varchar(511)  | NO   |     | NULL    |                |
| FullName        | varchar(2047) | NO   |     | NULL    |                |
| NumIndicators   | int(11)       | NO   |     | -1      |                |
+-----------------+---------------+------+-----+---------+----------------+
9 rows in set (0,00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS crs_courses ("
			"CrsCod INT NOT NULL AUTO_INCREMENT,"
			"DegCod INT NOT NULL DEFAULT -1,"
			"Year TINYINT NOT NULL DEFAULT 0,"
			"InsCrsCod CHAR(7) NOT NULL,"
			"Status TINYINT NOT NULL DEFAULT 0,"
			"RequesterUsrCod INT NOT NULL DEFAULT -1,"
			"ShortName VARCHAR(511) COLLATE latin1_spanish_ci NOT NULL,"	// Nam_MAX_BYTES_SHRT_NAME
			"FullName VARCHAR(2047) COLLATE latin1_spanish_ci NOT NULL,"	// Nam_MAX_BYTES_FULL_NAME
			"NumIndicators INT NOT NULL DEFAULT -1,"
		   "UNIQUE INDEX(CrsCod),"
		   "INDEX(DegCod,Year),"
		   "INDEX(Status)"
		   ") ENGINE=MyISAM");

   /***** Table crs_info_read *****/
/*
mysql> DESCRIBE crs_info_read;
+----------+--------------------------------------------------------------------------------------------+------+-----+---------+-------+
| Field    | Type                                                                                       | Null | Key | Default | Extra |
+----------+--------------------------------------------------------------------------------------------+------+-----+---------+-------+
| UsrCod   | int(11)                                                                                    | NO   | PRI | NULL    |       |
| CrsCod   | int(11)                                                                                    | NO   | PRI | NULL    |       |
| InfoType | enum('intro','description','theory','practices','bibliography','FAQ','links','assessment') | NO   | PRI | NULL    |       |
+----------+--------------------------------------------------------------------------------------------+------+-----+---------+-------+
3 rows in set (0.00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS crs_info_read ("
			"UsrCod INT NOT NULL,"
			"CrsCod INT NOT NULL,"
			"InfoType ENUM("
			"'intro',"
			"'description',"
			"'theory',"
			"'practices',"
			"'bibliography',"
			"'FAQ',"
			"'links',"
			"'assessment') NOT NULL,"
		   "UNIQUE INDEX(UsrCod,CrsCod,InfoType)"
		   ") ENGINE=MyISAM");

   /***** Table crs_info_src *****/
/*
mysql> DESCRIBE crs_info_src;
+------------+--------------------------------------------------------------------------------------------+------+-----+---------+-------+
| Field      | Type                                                                                       | Null | Key | Default | Extra |
+------------+--------------------------------------------------------------------------------------------+------+-----+---------+-------+
| CrsCod     | int(11)                                                                                    | NO   | PRI | -1      |       |
| InfoType   | enum('intro','description','theory','practices','bibliography','FAQ','links','assessment') | NO   | PRI | NULL    |       |
| InfoSrc    | enum('none','editor','plain_text','rich_text','page','URL')                                | NO   |     | NULL    |       |
| MustBeRead | enum('N','Y')                                                                              | NO   |     | N       |       |
+------------+--------------------------------------------------------------------------------------------+------+-----+---------+-------+
4 rows in set (0.00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS crs_info_src ("
			"CrsCod INT NOT NULL DEFAULT -1,"
			"InfoType ENUM("
			"'intro',"
			"'description',"
			"'theory',"
			"'practices',"
			"'bibliography',"
			"'FAQ',"
			"'links',"
			"'assessment') NOT NULL,"
			"InfoSrc ENUM('none','editor','plain_text','rich_text','page','URL') NOT NULL,"
			"MustBeRead ENUM('N','Y') NOT NULL DEFAULT 'N',"
		   "UNIQUE INDEX(CrsCod,InfoType)"
		   ") ENGINE=MyISAM");

   /***** Table crs_info_txt *****/
/*
mysql> DESCRIBE crs_info_txt;
+-------------+--------------------------------------------------------------------------------------------+------+-----+---------+-------+
| Field       | Type                                                                                       | Null | Key | Default | Extra |
+-------------+--------------------------------------------------------------------------------------------+------+-----+---------+-------+
| CrsCod      | int(11)                                                                                    | NO   | PRI | -1      |       |
| InfoType    | enum('intro','description','theory','practices','bibliography','FAQ','links','assessment') | NO   | PRI | NULL    |       |
| InfoTxtHTML | longtext                                                                                   | NO   |     | NULL    |       |
| InfoTxtMD   | longtext                                                                                   | NO   |     | NULL    |       |
+-------------+--------------------------------------------------------------------------------------------+------+-----+---------+-------+
4 rows in set (0.00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS crs_info_txt ("
			"CrsCod INT NOT NULL DEFAULT -1,"
			"InfoType ENUM("
			"'intro',"
			"'description',"
			"'theory',"
			"'practices',"
			"'bibliography',"
			"'FAQ',"
			"'links',"
			"'assessment') NOT NULL,"
			"InfoTxtHTML LONGTEXT NOT NULL,"
			"InfoTxtMD LONGTEXT NOT NULL,"
		   "UNIQUE INDEX(CrsCod,InfoType)"
		   ") ENGINE=MyISAM");

      /***** Table crs_last *****/
/*
mysql> DESCRIBE crs_last;
+----------+----------+------+-----+---------+-------+
| Field    | Type     | Null | Key | Default | Extra |
+----------+----------+------+-----+---------+-------+
| CrsCod   | int(11)  | NO   | PRI | NULL    |       |
| LastTime | datetime | NO   | MUL | NULL    |       |
+----------+----------+------+-----+---------+-------+
2 rows in set (0.00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS crs_last ("
			"CrsCod INT NOT NULL,"
			"LastTime DATETIME NOT NULL,"
		   "UNIQUE INDEX(CrsCod),"
		   "INDEX(LastTime)"
		   ") ENGINE=MyISAM");

   /***** Table crs_links *****/
/*
mysql> DESCRIBE crs_links;
+-------------+---------------+------+-----+---------+----------------+
| Field       | Type          | Null | Key | Default | Extra          |
+-------------+---------------+------+-----+---------+----------------+
| ItmCod      | int(11)       | NO   | PRI | NULL    | auto_increment |
| NodCod      | int(11)       | NO   | MUL | -1      |                |
| ItmInd      | int(11)       | NO   |     | 0       |                |
| Hidden      | enum('N','Y') | NO   |     | N       |                |
| Title       | varchar(2047) | NO   |     | NULL    |                |
| Description | varchar(2047) | NO   |     | NULL    |                |
| WWW         | varchar(255)  | NO   |     | NULL    |                |
+-------------+---------------+------+-----+---------+----------------+
7 rows in set (0.00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS crs_links ("
			"ItmCod INT NOT NULL AUTO_INCREMENT,"
			"NodCod INT NOT NULL DEFAULT -1,"
			"ItmInd INT NOT NULL DEFAULT 0,"
			"Hidden ENUM('N','Y') NOT NULL DEFAULT 'N',"
			"Title VARCHAR(2047) NOT NULL,"		// Lnk_MAX_BYTES_TITLE
			"Description VARCHAR(2047) NOT NULL,"	// Lnk_MAX_BYTES_TITLE
			"WWW VARCHAR(255) NOT NULL,"		// WWW_MAX_BYTES_WWW
		   "UNIQUE INDEX(ItmCod),"
		   "UNIQUE INDEX(NodCod,ItmInd)"
		   ") ENGINE=MyISAM");

   /***** Table crs_record_fields *****/
/*
mysql> DESCRIBE crs_record_fields;
+------------+---------------+------+-----+---------+----------------+
| Field      | Type          | Null | Key | Default | Extra          |
+------------+---------------+------+-----+---------+----------------+
| FieldCod   | int(11)       | NO   | PRI | NULL    | auto_increment |
| CrsCod     | int(11)       | NO   | MUL | -1      |                |
| FieldName  | varchar(2047) | NO   |     | NULL    |                |
| NumLines   | int(11)       | NO   |     | NULL    |                |
| Visibility | tinyint(4)    | NO   |     | NULL    |                |
+------------+---------------+------+-----+---------+----------------+
5 rows in set (0.00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS crs_record_fields ("
			"FieldCod INT NOT NULL AUTO_INCREMENT,"
			"CrsCod INT NOT NULL DEFAULT -1,"
			"FieldName VARCHAR(2047) NOT NULL,"	// Rec_MAX_BYTES_NAME_FIELD
			"NumLines INT NOT NULL,"
			"Visibility TINYINT NOT NULL,"
		   "UNIQUE INDEX(FieldCod),"
		   "INDEX(CrsCod)"
		   ") ENGINE=MyISAM");

   /***** Table crs_records *****/
/*
mysql> DESCRIBE crs_records;
+----------+---------+------+-----+---------+-------+
| Field    | Type    | Null | Key | Default | Extra |
+----------+---------+------+-----+---------+-------+
| FieldCod | int(11) | NO   | PRI | NULL    |       |
| UsrCod   | int(11) | NO   | PRI | NULL    |       |
| Txt      | text    | NO   |     | NULL    |       |
+----------+---------+------+-----+---------+-------+
3 rows in set (0.00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS crs_records ("
			"FieldCod INT NOT NULL,"
			"UsrCod INT NOT NULL,"
			"Txt TEXT NOT NULL,"	// Cns_MAX_BYTES_TEXT
		   "UNIQUE INDEX(FieldCod,UsrCod)"
		   ") ENGINE=MyISAM");

   /***** Table crs_requests *****/
/*
mysql> DESCRIBE crs_requests;
+-------------+------------+------+-----+---------+----------------+
| Field       | Type       | Null | Key | Default | Extra          |
+-------------+------------+------+-----+---------+----------------+
| ReqCod      | int(11)    | NO   | PRI | NULL    | auto_increment |
| CrsCod      | int(11)    | NO   | MUL | -1      |                |
| UsrCod      | int(11)    | NO   | MUL | NULL    |                |
| Role        | tinyint(4) | NO   |     | 0       |                |
| RequestTime | datetime   | NO   |     | NULL    |                |
+-------------+------------+------+-----+---------+----------------+
5 rows in set (0.00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS crs_requests ("
			"ReqCod INT NOT NULL AUTO_INCREMENT,"
			"CrsCod INT NOT NULL DEFAULT -1,"
			"UsrCod INT NOT NULL,"
			"Role TINYINT NOT NULL DEFAULT 0,"
			"RequestTime DATETIME NOT NULL,"
		   "UNIQUE INDEX(ReqCod),"
		   "UNIQUE INDEX(CrsCod,UsrCod),"
		   "INDEX(UsrCod)"
		   ") ENGINE=MyISAM");

   /***** Table crs_user_settings *****/
/*
mysql> DESCRIBE crs_user_settings;
+----------------+---------------------------+------+-----+------------+-------+
| Field          | Type                      | Null | Key | Default    | Extra |
+----------------+---------------------------+------+-----+------------+-------+
| UsrCod         | int(11)                   | NO   | PRI | NULL       |       |
| CrsCod         | int(11)                   | NO   | PRI | NULL       |       |
| LastDowGrpCod  | int(11)                   | NO   |     | -1         |       |
| LastComGrpCod  | int(11)                   | NO   |     | -1         |       |
| LastAssGrpCod  | int(11)                   | NO   |     | -1         |       |
| NumAccTst      | int(11)                   | NO   |     | 0          |       |
| LastAccTst     | datetime                  | NO   |     | NULL       |       |
| NumQstsLastTst | int(11)                   | NO   |     | 0          |       |
| UsrListType    | enum('classphoto','list') | NO   |     | classphoto |       |
| ListWithPhotos | enum('N','Y')             | NO   |     | Y          |       |
+----------------+---------------------------+------+-----+------------+-------+
10 rows in set (0.00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS crs_user_settings ("
			"UsrCod INT NOT NULL,"
			"CrsCod INT NOT NULL,"
			"LastDowGrpCod INT NOT NULL DEFAULT -1,"
			"LastComGrpCod INT NOT NULL DEFAULT -1,"
			"LastAssGrpCod INT NOT NULL DEFAULT -1,"
			"NumAccTst INT NOT NULL DEFAULT 0,"
			"LastAccTst DATETIME NOT NULL,"
			"NumQstsLastTst INT NOT NULL DEFAULT 0,"
			"UsrListType ENUM('classphoto','list') NOT NULL DEFAULT 'classphoto',"
			"ListWithPhotos ENUM('N','Y') NOT NULL DEFAULT 'Y',"
		   "UNIQUE INDEX(UsrCod,CrsCod)"
		   ") ENGINE=MyISAM");

   /***** Table crs_users *****/
/*
mysql> DESCRIBE crs_users;
+----------+---------------+------+-----+---------+-------+
| Field    | Type          | Null | Key | Default | Extra |
+----------+---------------+------+-----+---------+-------+
| CrsCod   | int(11)       | NO   | PRI | NULL    |       |
| UsrCod   | int(11)       | NO   | PRI | NULL    |       |
| Role     | tinyint(4)    | NO   | MUL | 0       |       |
| Accepted | enum('N','Y') | NO   |     | N       |       |
+----------+---------------+------+-----+---------+-------+
4 rows in set (0.00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS crs_users ("
			"CrsCod INT NOT NULL,"
			"UsrCod INT NOT NULL,"
			"Role TINYINT NOT NULL DEFAULT 0,"
			"Accepted ENUM('N','Y') NOT NULL DEFAULT 'N',"
	           "UNIQUE INDEX(CrsCod,UsrCod),"
		   "UNIQUE INDEX(CrsCod,UsrCod,Role),"
		   "UNIQUE INDEX(UsrCod,CrsCod),"
		   "UNIQUE INDEX(UsrCod,CrsCod,Role),"
		   "UNIQUE INDEX(Role,CrsCod,UsrCod),"
		   "UNIQUE INDEX(Role,UsrCod,CrsCod),"
		   "INDEX(CrsCod,Role),"
		   "INDEX(UsrCod,Role)"
		   ") ENGINE=MyISAM");

   /***** Table ctr_centers *****/
/*
mysql> DESCRIBE ctr_centers;
+------------------+---------------+------+-----+---------+----------------+
| Field            | Type          | Null | Key | Default | Extra          |
+------------------+---------------+------+-----+---------+----------------+
| CtrCod           | int(11)       | NO   | PRI | NULL    | auto_increment |
| InsCod           | int(11)       | NO   | MUL | NULL    |                |
| PlcCod           | int(11)       | NO   | MUL | -1      |                |
| Status           | tinyint(4)    | NO   | MUL | 0       |                |
| RequesterUsrCod  | int(11)       | NO   |     | -1      |                |
| Latitude         | double        | NO   |     | 0       |                |
| Longitude        | double        | NO   |     | 0       |                |
| Altitude         | double        | NO   |     | 0       |                |
| ShortName        | varchar(511)  | NO   |     | NULL    |                |
| FullName         | varchar(2047) | NO   |     | NULL    |                |
| WWW              | varchar(255)  | NO   |     | NULL    |                |
| PhotoAttribution | text          | NO   |     | NULL    |                |
+------------------+---------------+------+-----+---------+----------------+
12 rows in set (0.00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS ctr_centers ("
			"CtrCod INT NOT NULL AUTO_INCREMENT,"
			"InsCod INT NOT NULL,"
			"PlcCod INT NOT NULL,"
			"Status TINYINT NOT NULL DEFAULT 0,"
			"RequesterUsrCod INT NOT NULL DEFAULT -1,"
			"Latitude DOUBLE PRECISION NOT NULL DEFAULT 0,"
			"Longitude DOUBLE PRECISION NOT NULL DEFAULT 0,"
			"Altitude DOUBLE PRECISION NOT NULL DEFAULT 0,"
			"ShortName VARCHAR(511) COLLATE latin1_spanish_ci NOT NULL,"	// Nam_MAX_BYTES_SHRT_NAME
			"FullName VARCHAR(2047) COLLATE latin1_spanish_ci NOT NULL,"	// Nam_MAX_BYTES_FULL_NAME
			"WWW VARCHAR(255) NOT NULL,"					// WWW_MAX_BYTES_WWW
			"PhotoAttribution TEXT NOT NULL,"				// Med_MAX_BYTES_ATTRIBUTION
		   "UNIQUE INDEX(CtrCod),"
		   "INDEX(InsCod),"
		   "INDEX(PlcCod),"
		   "INDEX(Status)"
		   ") ENGINE=MyISAM");

   /***** Table cty_countrs *****/
/*
mysql> DESCRIBE cty_countrs;
+----------------+--------------+------+-----+---------+-------+
| Field          | Type         | Null | Key | Default | Extra |
+----------------+--------------+------+-----+---------+-------+
| CtyCod         | int(11)      | NO   | PRI | NULL    |       |
| Alpha2         | char(2)      | NO   | UNI | NULL    |       |
| MapAttribution | text         | NO   |     | NULL    |       |
| Name_ca        | varchar(767) | NO   | MUL | NULL    |       |
| Name_de        | varchar(767) | NO   | MUL | NULL    |       |
| Name_en        | varchar(767) | NO   | MUL | NULL    |       |
| Name_es        | varchar(767) | NO   | MUL | NULL    |       |
| Name_fr        | varchar(767) | NO   | MUL | NULL    |       |
| Name_gn        | varchar(767) | NO   | MUL | NULL    |       |
| Name_it        | varchar(767) | NO   | MUL | NULL    |       |
| Name_pl        | varchar(767) | NO   | MUL | NULL    |       |
| Name_pt        | varchar(767) | NO   | MUL | NULL    |       |
| Name_tr        | varchar(767) | NO   | MUL | NULL    |       |
| WWW_ca         | varchar(255) | NO   |     | NULL    |       |
| WWW_de         | varchar(255) | NO   |     | NULL    |       |
| WWW_en         | varchar(255) | NO   |     | NULL    |       |
| WWW_es         | varchar(255) | NO   |     | NULL    |       |
| WWW_fr         | varchar(255) | NO   |     | NULL    |       |
| WWW_gn         | varchar(255) | NO   |     | NULL    |       |
| WWW_it         | varchar(255) | NO   |     | NULL    |       |
| WWW_pl         | varchar(255) | NO   |     | NULL    |       |
| WWW_pt         | varchar(255) | NO   |     | NULL    |       |
| WWW_tr         | varchar(255) | NO   |     | NULL    |       |
+----------------+--------------+------+-----+---------+-------+
23 rows in set (0.00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS cty_countrs ("
			"CtyCod INT NOT NULL,"
			"Alpha2 CHAR(2) NOT NULL,"
			"MapAttribution TEXT NOT NULL,"		// Med_MAX_BYTES_ATTRIBUTION
			"Name_ca VARCHAR(767) NOT NULL,"	// Cty_MAX_BYTES_NAME
			"Name_de VARCHAR(767) NOT NULL,"	// Cty_MAX_BYTES_NAME
			"Name_en VARCHAR(767) NOT NULL,"	// Cty_MAX_BYTES_NAME
			"Name_es VARCHAR(767) NOT NULL,"	// Cty_MAX_BYTES_NAME
			"Name_fr VARCHAR(767) NOT NULL,"	// Cty_MAX_BYTES_NAME
			"Name_gn VARCHAR(767) NOT NULL,"	// Cty_MAX_BYTES_NAME
			"Name_it VARCHAR(767) NOT NULL,"	// Cty_MAX_BYTES_NAME
			"Name_pl VARCHAR(767) NOT NULL,"	// Cty_MAX_BYTES_NAME
			"Name_pt VARCHAR(767) NOT NULL,"	// Cty_MAX_BYTES_NAME
			"Name_tr VARCHAR(767) NOT NULL,"	// Cty_MAX_BYTES_NAME
			"WWW_ca VARCHAR(255) NOT NULL,"		// WWW_MAX_BYTES_WWW
			"WWW_de VARCHAR(255) NOT NULL,"		// WWW_MAX_BYTES_WWW
			"WWW_en VARCHAR(255) NOT NULL,"		// WWW_MAX_BYTES_WWW
			"WWW_es VARCHAR(255) NOT NULL,"		// WWW_MAX_BYTES_WWW
			"WWW_fr VARCHAR(255) NOT NULL,"		// WWW_MAX_BYTES_WWW
			"WWW_gn VARCHAR(255) NOT NULL,"		// WWW_MAX_BYTES_WWW
			"WWW_it VARCHAR(255) NOT NULL,"		// WWW_MAX_BYTES_WWW
			"WWW_pl VARCHAR(255) NOT NULL,"		// WWW_MAX_BYTES_WWW
			"WWW_pt VARCHAR(255) NOT NULL,"		// WWW_MAX_BYTES_WWW
			"WWW_tr VARCHAR(255) NOT NULL,"		// WWW_MAX_BYTES_WWW
		   "UNIQUE INDEX(CtyCod),UNIQUE INDEX(Alpha2),"
		   "INDEX(Name_ca),"
		   "INDEX(Name_de),"
		   "INDEX(Name_en),"
		   "INDEX(Name_es),"
		   "INDEX(Name_fr),"
		   "INDEX(Name_gn),"
		   "INDEX(Name_it),"
		   "INDEX(Name_pl),"
		   "INDEX(Name_pt),"
		   "INDEX(Name_tr)"
		   ") ENGINE=MyISAM");	// ISO 3166-1 country codes

   /***** Table dbg_debug *****/
/*
mysql> DESCRIBE dbg_debug;
+-----------+----------+------+-----+---------+-------+
| Field     | Type     | Null | Key | Default | Extra |
+-----------+----------+------+-----+---------+-------+
| DebugTime | datetime | NO   | MUL | NULL    |       |
| Txt       | text     | NO   |     | NULL    |       |
+-----------+----------+------+-----+---------+-------+
2 rows in set (0.00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS dbg_debug ("
			"DebugTime DATETIME NOT NULL,"
			"Txt TEXT NOT NULL,"
		   "INDEX(DebugTime)"
		   ") ENGINE=MyISAM");

   /***** Table deg_degrees *****/
/*
mysql> DESCRIBE deg_degrees;
+-----------------+---------------+------+-----+---------+----------------+
| Field           | Type          | Null | Key | Default | Extra          |
+-----------------+---------------+------+-----+---------+----------------+
| DegCod          | int(11)       | NO   | PRI | NULL    | auto_increment |
| CtrCod          | int(11)       | NO   | MUL | NULL    |                |
| DegTypCod       | int(11)       | NO   | MUL | NULL    |                |
| Status          | tinyint(4)    | NO   | MUL | 0       |                |
| RequesterUsrCod | int(11)       | NO   |     | -1      |                |
| ShortName       | varchar(511)  | NO   |     | NULL    |                |
| FullName        | varchar(2047) | NO   |     | NULL    |                |
| WWW             | varchar(255)  | NO   |     | NULL    |                |
+-----------------+---------------+------+-----+---------+----------------+
8 rows in set (0.00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS deg_degrees ("
			"DegCod INT NOT NULL AUTO_INCREMENT,"
			"CtrCod INT NOT NULL,"
			"DegTypCod INT NOT NULL,"
			"Status TINYINT NOT NULL DEFAULT 0,"
			"RequesterUsrCod INT NOT NULL DEFAULT -1,"
			"ShortName VARCHAR(511) COLLATE latin1_spanish_ci NOT NULL,"	// Nam_MAX_BYTES_SHRT_NAME
			"FullName VARCHAR(2047) COLLATE latin1_spanish_ci NOT NULL,"	// Nam_MAX_BYTES_FULL_NAME
			"WWW VARCHAR(255) NOT NULL,"					// WWW_MAX_BYTES_WWW
		   "UNIQUE INDEX(DegCod),"
		   "INDEX(CtrCod),"
		   "INDEX(DegTypCod),"
		   "INDEX(Status)"
		   ") ENGINE=MyISAM");

   /***** Table deg_types *****/
/*
mysql> DESCRIBE deg_types;
+------------+--------------+------+-----+---------+----------------+
| Field      | Type         | Null | Key | Default | Extra          |
+------------+--------------+------+-----+---------+----------------+
| DegTypCod  | int(11)      | NO   | PRI | NULL    | auto_increment |
| DegTypName | varchar(511) | NO   |     | NULL    |                |
+------------+--------------+------+-----+---------+----------------+
2 rows in set (0.00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS deg_types ("
			"DegTypCod INT NOT NULL AUTO_INCREMENT,"
			"DegTypName VARCHAR(511) NOT NULL,"	// Deg_MAX_BYTES_DEGREE_TYPE_NAME
		   "UNIQUE INDEX(DegTypCod)"
		   ") ENGINE=MyISAM");

   /***** Table dpt_departments *****/
/*
mysql> DESCRIBE dpt_departments;
+-----------+---------------+------+-----+---------+----------------+
| Field     | Type          | Null | Key | Default | Extra          |
+-----------+---------------+------+-----+---------+----------------+
| DptCod    | int(11)       | NO   | PRI | NULL    | auto_increment |
| InsCod    | int(11)       | NO   | MUL | NULL    |                |
| ShortName | varchar(511)  | NO   |     | NULL    |                |
| FullName  | varchar(2047) | NO   |     | NULL    |                |
| WWW       | varchar(255)  | NO   |     | NULL    |                |
+-----------+---------------+------+-----+---------+----------------+
5 rows in set (0.00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS dpt_departments ("
			"DptCod INT NOT NULL AUTO_INCREMENT,"
			"InsCod INT NOT NULL,"
			"ShortName VARCHAR(511) NOT NULL,"	// Nam_MAX_BYTES_SHRT_NAME
			"FullName VARCHAR(2047) NOT NULL,"	// Nam_MAX_BYTES_FULL_NAME
			"WWW VARCHAR(255) NOT NULL,"		// WWW_MAX_BYTES_WWW
		   "UNIQUE INDEX(DptCod),"
		   "INDEX(InsCod)"
		   ") ENGINE=MyISAM");

   /***** Table exa_exams *****/
/*
mysql> DESCRIBE exa_exams;
+------------+---------------+------+-----+---------+----------------+
| Field      | Type          | Null | Key | Default | Extra          |
+------------+---------------+------+-----+---------+----------------+
| ExaCod     | int(11)       | NO   | PRI | NULL    | auto_increment |
| CrsCod     | int(11)       | NO   | MUL | -1      |                |
| Hidden     | enum('N','Y') | NO   |     | N       |                |
| UsrCod     | int(11)       | NO   |     | NULL    |                |
| MaxGrade   | double        | NO   |     | 1       |                |
| Visibility | int(11)       | NO   |     | 31      |                |
| Title      | varchar(2047) | NO   |     | NULL    |                |
| Txt        | text          | NO   |     | NULL    |                |
+------------+---------------+------+-----+---------+----------------+
8 rows in set (0.00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS exa_exams ("
			"ExaCod INT NOT NULL AUTO_INCREMENT,"
			"CrsCod INT NOT NULL DEFAULT -1,"
			"Hidden ENUM('N','Y') NOT NULL DEFAULT 'N',"
			"UsrCod INT NOT NULL,"
			"MaxGrade DOUBLE PRECISION NOT NULL DEFAULT 1,"	// Scale from score [0...num.answers] to grade [0...MaxGrade]
			"Visibility INT NOT NULL DEFAULT 0x1f,"
			"Title VARCHAR(2047) NOT NULL,"	// Exa_MAX_BYTES_TITLE
			"Txt TEXT NOT NULL,"		// Cns_MAX_BYTES_TEXT
		   "UNIQUE INDEX(ExaCod),"
		   "INDEX(CrsCod)"
		   ") ENGINE=MyISAM");

   /***** Table exa_groups *****/
/*
mysql> DESCRIBE exa_groups;
+--------+---------+------+-----+---------+-------+
| Field  | Type    | Null | Key | Default | Extra |
+--------+---------+------+-----+---------+-------+
| SesCod | int(11) | NO   | PRI | NULL    |       |
| GrpCod | int(11) | NO   | PRI | NULL    |       |
+--------+---------+------+-----+---------+-------+
2 rows in set (0.00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS exa_groups ("
			"SesCod INT NOT NULL,"
			"GrpCod INT NOT NULL,"
		   "UNIQUE INDEX(SesCod,GrpCod)"
		   ") ENGINE=MyISAM");

   /***** Table exa_log *****/
/*
mysql> DESCRIBE exa_log;
+-----------+---------------+------+-----+---------+-------+
| Field     | Type          | Null | Key | Default | Extra |
+-----------+---------------+------+-----+---------+-------+
| LogCod    | int(11)       | NO   | PRI | NULL    |       |
| PrnCod    | int(11)       | NO   | MUL | NULL    |       |
| ActCod    | int(11)       | NO   |     | NULL    |       |
| QstInd    | int(11)       | NO   |     | -1      |       |
| CanAnswer | enum('N','Y') | NO   |     | N       |       |
| ClickTime | datetime      | NO   | MUL | NULL    |       |
| IP        | char(15)      | NO   |     | NULL    |       |
+-----------+---------------+------+-----+---------+-------+
7 rows in set (0.00 sec)
*/
// TODO: Change NtfCod and LogCod from INT to BIGINT in database tables.
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS exa_log ("
			"LogCod INT NOT NULL,"
			"PrnCod INT NOT NULL,"
			"ActCod INT NOT NULL,"
		        "QstInd INT NOT NULL DEFAULT -1,"
			"CanAnswer ENUM('N','Y') NOT NULL DEFAULT 'N',"
			"ClickTime DATETIME NOT NULL,"
			"IP CHAR(15) NOT NULL,"		// Cns_MAX_BYTES_IP
		   "UNIQUE INDEX(LogCod),"
		   "UNIQUE INDEX(PrnCod,LogCod),"
		   "INDEX(ClickTime)"
		   ") ENGINE=MyISAM");

   /***** Table exa_log_sessions *****/
/*
mysql> DESCRIBE exa_log_sessions;
+-----------+----------+------+-----+---------+-------+
| Field     | Type     | Null | Key | Default | Extra |
+-----------+----------+------+-----+---------+-------+
| LogCod    | int(11)  | NO   | PRI | NULL    |       |
| PrnCod    | int(11)  | NO   | MUL | NULL    |       |
| SessionId | char(43) | NO   |     | NULL    |       |
+-----------+----------+------+-----+---------+-------+
3 rows in set (0.00 sec)
*/
// TODO: Change NtfCod and LogCod from INT to BIGINT in database tables.
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS exa_log_sessions ("
			"LogCod INT NOT NULL,"
			"PrnCod INT NOT NULL,"
			"SessionId CHAR(43) NOT NULL,"	// Cns_BYTES_SESSION_ID
		   "UNIQUE INDEX(LogCod),"
		   "UNIQUE INDEX(PrnCod,LogCod)"
		   ") ENGINE=MyISAM");

   /***** Table exa_log_user_agents *****/
/*
mysql> DESCRIBE exa_log_user_agents;
+-----------+---------+------+-----+---------+-------+
| Field     | Type    | Null | Key | Default | Extra |
+-----------+---------+------+-----+---------+-------+
| LogCod    | int(11) | NO   | PRI | NULL    |       |
| PrnCod    | int(11) | NO   | MUL | NULL    |       |
| UserAgent | text    | NO   |     | NULL    |       |
+-----------+---------+------+-----+---------+-------+
3 rows in set (0.00 sec)
*/
// TODO: Change NtfCod and LogCod from INT to BIGINT in database tables.
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS exa_log_user_agents ("
			"LogCod INT NOT NULL,"
			"PrnCod INT NOT NULL,"
			"UserAgent TEXT NOT NULL,"
		   "UNIQUE INDEX(LogCod),"
		   "UNIQUE INDEX(PrnCod,LogCod)"
		   ") ENGINE=MyISAM");

/***** Table exa_print_questions *****/
/*
mysql> DESCRIBE exa_print_questions;
+---------+---------+------+-----+---------+-------+
| Field   | Type    | Null | Key | Default | Extra |
+---------+---------+------+-----+---------+-------+
| PrnCod  | int(11) | NO   | PRI | NULL    |       |
| QstCod  | int(11) | NO   | PRI | NULL    |       |
| QstInd  | int(11) | NO   |     | NULL    |       |
| SetCod  | int(11) | NO   |     | NULL    |       |
| Score   | double  | NO   |     | 0       |       |
| Indexes | text    | NO   |     | NULL    |       |
| Answers | text    | NO   |     | NULL    |       |
+---------+---------+------+-----+---------+-------+
7 rows in set (0.00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS exa_print_questions ("
			"PrnCod INT NOT NULL,"
			"QstCod INT NOT NULL,"
			"QstInd INT NOT NULL,"
	                "SetCod INT NOT NULL,"
			"Score DOUBLE PRECISION NOT NULL DEFAULT 0,"
			"Indexes TEXT NOT NULL,"	// Tst_MAX_BYTES_INDEXES_ONE_QST
			"Answers TEXT NOT NULL,"	// Tst_MAX_BYTES_ANSWERS_ONE_QST
		   "UNIQUE INDEX(PrnCod,QstCod)"
		   ") ENGINE=MyISAM");

   /***** Table exa_prints *****/
/*
mysql> DESCRIBE exa_prints;
+-----------------+----------+------+-----+---------+----------------+
| Field           | Type     | Null | Key | Default | Extra          |
+-----------------+----------+------+-----+---------+----------------+
| PrnCod          | int(11)  | NO   | PRI | NULL    | auto_increment |
| SesCod          | int(11)  | NO   | MUL | NULL    |                |
| UsrCod          | int(11)  | NO   |     | NULL    |                |
| StartTime       | datetime | NO   |     | NULL    |                |
| EndTime         | datetime | NO   |     | NULL    |                |
| NumQsts         | int(11)  | NO   |     | 0       |                |
| NumQstsNotBlank | int(11)  | NO   |     | 0       |                |
| Score           | double   | NO   |     | 0       |                |
+-----------------+----------+------+-----+---------+----------------+
8 rows in set (0.00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS exa_prints ("
			"PrnCod INT NOT NULL AUTO_INCREMENT,"
			"SesCod INT NOT NULL,"
			"UsrCod INT NOT NULL,"
			"StartTime DATETIME NOT NULL,"
			"EndTime DATETIME NOT NULL,"
			"NumQsts INT NOT NULL DEFAULT 0,"
			"NumQstsNotBlank INT NOT NULL DEFAULT 0,"
			"Score DOUBLE PRECISION NOT NULL DEFAULT 0,"
		   "UNIQUE INDEX(PrnCod),"
		   "UNIQUE INDEX(SesCod,UsrCod)"
		   ") ENGINE=MyISAM");

   /***** Table exa_sessions *****/
/*
mysql> DESCRIBE exa_sessions;
+----------------+-------------------------------+------+-----+---------+----------------+
| Field          | Type                          | Null | Key | Default | Extra          |
+----------------+-------------------------------+------+-----+---------+----------------+
| SesCod         | int(11)                       | NO   | PRI | NULL    | auto_increment |
| ExaCod         | int(11)                       | NO   | MUL | NULL    |                |
| Hidden         | enum('N','Y')                 | NO   |     | N       |                |
| UsrCod         | int(11)                       | NO   |     | NULL    |                |
| Modality       | enum('none','online','paper') | NO   |     | none    |                |
| StartTime      | datetime                      | NO   |     | NULL    |                |
| EndTime        | datetime                      | NO   |     | NULL    |                |
| Title          | varchar(2047)                 | NO   |     | NULL    |                |
| ShowUsrResults | enum('N','Y')                 | NO   |     | N       |                |
| NumCols        | tinyint(4)                    | NO   |     | 0       |                |
| ShowPhotos     | enum('N','Y')                 | NO   |     | Y       |                |
+----------------+-------------------------------+------+-----+---------+----------------+
11 rows in set (0.00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS exa_sessions ("
			"SesCod INT NOT NULL AUTO_INCREMENT,"
			"ExaCod INT NOT NULL,"
			"Hidden ENUM('N','Y') NOT NULL DEFAULT 'N',"
			"UsrCod INT NOT NULL,"
			"Modality ENUM('none','online','paper') NOT NULL DEFAULT 'none',"
			"StartTime DATETIME NOT NULL,"
			"EndTime DATETIME NOT NULL,"
			"Title VARCHAR(2047) NOT NULL,"	// ExaSes_MAX_BYTES_TITLE
			"ShowUsrResults ENUM('N','Y') NOT NULL DEFAULT 'N',"
			"NumCols TINYINT NOT NULL DEFAULT 0,"	// 0 means not set ==> use default
			"ShowPhotos ENUM('N','Y') NOT NULL DEFAULT 'Y',"
		   "UNIQUE INDEX(SesCod),"
		   "INDEX(ExaCod)"
		   ") ENGINE=MyISAM");

   /***** Table exa_set_answers *****/
/*
mysql> DESCRIBE exa_set_answers;
+----------+---------------+------+-----+---------+-------+
| Field    | Type          | Null | Key | Default | Extra |
+----------+---------------+------+-----+---------+-------+
| QstCod   | int(11)       | NO   | PRI | NULL    |       |
| AnsInd   | tinyint(4)    | NO   | PRI | NULL    |       |
| Answer   | text          | NO   |     | NULL    |       |
| Feedback | text          | NO   |     | NULL    |       |
| MedCod   | int(11)       | NO   | MUL | -1      |       |
| Correct  | enum('N','Y') | NO   |     | NULL    |       |
+----------+---------------+------+-----+---------+-------+
6 rows in set (0.00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS exa_set_answers ("
			"QstCod INT NOT NULL,"
			"AnsInd TINYINT NOT NULL,"
			"Answer TEXT NOT NULL,"		// Qst_MAX_BYTES_ANSWER_OR_FEEDBACK
			"Feedback TEXT NOT NULL,"	// Qst_MAX_BYTES_ANSWER_OR_FEEDBACK
			"MedCod INT NOT NULL DEFAULT -1,"
			"Correct ENUM('N','Y') NOT NULL,"
		   "UNIQUE INDEX(QstCod,AnsInd),"
		   "INDEX(MedCod)"
		   ") ENGINE=MyISAM");

   /***** Table exa_set_questions *****/
/*
mysql> DESCRIBE exa_set_questions;
+----------+---------------------------------------------------------------------------+------+-----+---------+----------------+
| Field    | Type                                                                      | Null | Key | Default | Extra          |
+----------+---------------------------------------------------------------------------+------+-----+---------+----------------+
| QstCod   | int(11)                                                                   | NO   | PRI | NULL    | auto_increment |
| SetCod   | int(11)                                                                   | NO   | MUL | NULL    |                |
| Invalid  | enum('N','Y')                                                             | NO   |     | N       |                |
| AnsType  | enum('int','float','true_false','unique_choice','multiple_choice','text') | NO   |     | NULL    |                |
| Shuffle  | enum('N','Y')                                                             | NO   |     | NULL    |                |
| Stem     | text                                                                      | NO   |     | NULL    |                |
| Feedback | text                                                                      | NO   |     | NULL    |                |
| MedCod   | int(11)                                                                   | NO   | MUL | -1      |                |
+----------+---------------------------------------------------------------------------+------+-----+---------+----------------+
8 rows in set (0.00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS exa_set_questions ("
			"QstCod INT NOT NULL AUTO_INCREMENT,"
			"SetCod INT NOT NULL,"
			"Invalid ENUM('N','Y') NOT NULL DEFAULT 'N',"
			"AnsType ENUM ('int','float','true_false','unique_choice','multiple_choice','text') NOT NULL,"
			"Shuffle ENUM('N','Y') NOT NULL,"
			"Stem TEXT NOT NULL,"		// Cns_MAX_BYTES_TEXT
			"Feedback TEXT NOT NULL,"	// Cns_MAX_BYTES_TEXT
			"MedCod INT NOT NULL DEFAULT -1,"
		   "UNIQUE INDEX(QstCod),"
		   "UNIQUE INDEX(SetCod,QstCod),"
		   "INDEX(MedCod)"
		   ") ENGINE=MyISAM");

   /***** Table exa_sets *****/
/*
mysql> DESCRIBE exa_sets;
+----------------+---------------+------+-----+---------+----------------+
| Field          | Type          | Null | Key | Default | Extra          |
+----------------+---------------+------+-----+---------+----------------+
| SetCod         | int(11)       | NO   | PRI | NULL    | auto_increment |
| ExaCod         | int(11)       | NO   | MUL | NULL    |                |
| SetInd         | int(11)       | NO   |     | NULL    |                |
| NumQstsToPrint | int(11)       | NO   |     | 0       |                |
| Title          | varchar(2047) | NO   |     | NULL    |                |
+----------------+---------------+------+-----+---------+----------------+
5 rows in set (0.00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS exa_sets ("
			"SetCod INT NOT NULL AUTO_INCREMENT,"
			"ExaCod INT NOT NULL,"
			"SetInd INT NOT NULL,"
			"NumQstsToPrint INT NOT NULL DEFAULT 0,"
			"Title VARCHAR(2047) NOT NULL,"	// ExaSet_MAX_BYTES_TITLE
		   "UNIQUE INDEX(SetCod),"
		   "UNIQUE INDEX(ExaCod,SetInd)"
		   ") ENGINE=MyISAM");

   /***** Table faq_questions *****/
/*
mysql> DESCRIBE faq_questions;
+----------+---------------+------+-----+---------+----------------+
| Field    | Type          | Null | Key | Default | Extra          |
+----------+---------------+------+-----+---------+----------------+
| ItmCod   | int(11)       | NO   | PRI | NULL    | auto_increment |
| NodCod   | int(11)       | NO   | MUL | -1      |                |
| ItmInd   | int(11)       | NO   |     | 0       |                |
| Hidden   | enum('N','Y') | NO   |     | N       |                |
| Question | varchar(2047) | NO   |     | NULL    |                |
| Answer   | text          | NO   |     | NULL    |                |
+----------+---------------+------+-----+---------+----------------+
6 rows in set (0.00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS faq_questions ("
			"ItmCod INT NOT NULL AUTO_INCREMENT,"
			"NodCod INT NOT NULL DEFAULT -1,"
			"ItmInd INT NOT NULL DEFAULT 0,"
			"Hidden ENUM('N','Y') NOT NULL DEFAULT 'N',"
			"Question VARCHAR(2047) NOT NULL,"	// FAQ_MAX_BYTES_QUESTION
			"Answer TEXT NOT NULL,"			// Cns_MAX_BYTES_TEXT
		   "UNIQUE INDEX(ItmCod),"
		   "UNIQUE INDEX(NodCod,ItmInd)"
		   ") ENGINE=MyISAM");

   /***** Table fig_figures *****/
   /*
mysql> DESCRIBE fig_figures;
+-------------+-------------------------------------------+------+-----+-------------------+-----------------------------+
| Field       | Type                                      | Null | Key | Default           | Extra                       |
+-------------+-------------------------------------------+------+-----+-------------------+-----------------------------+
| Figure      | int(11)                                   | NO   | PRI | NULL              |                             |
| Scope       | enum('Sys','Cty','Ins','Ctr','Deg','Crs') | NO   | PRI | Sys               |                             |
| Cod         | int(11)                                   | NO   | PRI | -1                |                             |
| ValueInt    | int(11)                                   | NO   |     | 0                 |                             |
| ValueDouble | double                                    | NO   |     | 0                 |                             |
| LastUpdate  | timestamp                                 | NO   |     | CURRENT_TIMESTAMP | on update CURRENT_TIMESTAMP |
+-------------+-------------------------------------------+------+-----+-------------------+-----------------------------+
6 rows in set (0.00 sec)
   */
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS fig_figures ("
			"Figure INT NOT NULL,"
			"Scope ENUM('Sys','Cty','Ins','Ctr','Deg','Crs') NOT NULL DEFAULT 'Sys',"
			"Cod INT NOT NULL DEFAULT -1,"
			"ValueInt INT NOT NULL,"
			"ValueDouble DOUBLE PRECISION NOT NULL,"
			"LastUpdate TIMESTAMP,"
		   "UNIQUE INDEX(Figure,Scope,Cod)"
		   ") ENGINE=MyISAM");

   /***** Table fir_banned *****/
/*
mysql> DESCRIBE fir_banned;
+-----------+----------+------+-----+---------+-------+
| Field     | Type     | Null | Key | Default | Extra |
+-----------+----------+------+-----+---------+-------+
| IP        | char(15) | NO   | MUL | NULL    |       |
| BanTime   | datetime | NO   | MUL | NULL    |       |
| UnbanTime | datetime | NO   | MUL | NULL    |       |
+-----------+----------+------+-----+---------+-------+
3 rows in set (0.00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS fir_banned ("
			"IP CHAR(15) NOT NULL,"		// Cns_MAX_BYTES_IP
			"BanTime DATETIME NOT NULL,"
			"UnbanTime DATETIME NOT NULL,"
		   "INDEX(IP,UnbanTime),"
		   "INDEX(BanTime),"
		   "INDEX(UnbanTime)"
		   ") ENGINE=MyISAM");

   /***** Table fir_log *****/
/*
mysql> DESCRIBE fir_log;
+-----------+----------+------+-----+---------+-------+
| Field     | Type     | Null | Key | Default | Extra |
+-----------+----------+------+-----+---------+-------+
| ClickTime | datetime | NO   | MUL | NULL    |       |
| IP        | char(15) | NO   | MUL | NULL    |       |
| UsrCod    | int(11)  | NO   | MUL | -1      |       |
+-----------+----------+------+-----+---------+-------+
3 rows in set (0.00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS fir_log ("
			"ClickTime DATETIME NOT NULL,"
			"IP CHAR(15) NOT NULL,"	// Cns_MAX_BYTES_IP
			"UsrCod INT NOT NULL DEFAULT -1,"
		   "INDEX(ClickTime),"
		   "INDEX(IP),"
		   "INDEX(UsrCod)"
		   ") ENGINE=MyISAM");

   /***** Table for_clipboards *****/
/*
mysql> DESCRIBE for_clipboards;
+------------+-----------+------+-----+-------------------+-----------------------------+
| Field      | Type      | Null | Key | Default           | Extra                       |
+------------+-----------+------+-----+-------------------+-----------------------------+
| ThrCod     | int(11)   | NO   | PRI | NULL              |                             |
| UsrCod     | int(11)   | NO   | UNI | NULL              |                             |
| TimeInsert | timestamp | NO   |     | CURRENT_TIMESTAMP | on update CURRENT_TIMESTAMP |
+------------+-----------+------+-----+-------------------+-----------------------------+
3 rows in set (0.00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS for_clipboards ("
			"ThrCod INT NOT NULL,"
			"UsrCod INT NOT NULL,"
			"TimeInsert TIMESTAMP NOT NULL,"
		   "UNIQUE INDEX(ThrCod),"
		   "UNIQUE INDEX(UsrCod)"
		   ") ENGINE=MyISAM");

   /***** Table for_disabled *****/
/*
mysql> DESCRIBE for_disabled;
+-------------+----------+------+-----+---------+-------+
| Field       | Type     | Null | Key | Default | Extra |
+-------------+----------+------+-----+---------+-------+
| PstCod      | int(11)  | NO   | PRI | NULL    |       |
| UsrCod      | int(11)  | NO   |     | NULL    |       |
| DisableTime | datetime | NO   |     | NULL    |       |
+-------------+----------+------+-----+---------+-------+
3 rows in set (0.00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS for_disabled ("
			"PstCod INT NOT NULL,"
			"UsrCod INT NOT NULL,"
			"DisableTime DATETIME NOT NULL,"
		   "UNIQUE INDEX(PstCod)"
		   ") ENGINE=MyISAM");

   /***** Table for_posts *****/
/*
mysql> DESCRIBE for_posts;
+-----------+----------+------+-----+---------+----------------+
| Field     | Type     | Null | Key | Default | Extra          |
+-----------+----------+------+-----+---------+----------------+
| PstCod    | int(11)  | NO   | PRI | NULL    | auto_increment |
| ThrCod    | int(11)  | NO   | MUL | NULL    |                |
| UsrCod    | int(11)  | NO   | MUL | NULL    |                |
| CreatTime | datetime | NO   | MUL | NULL    |                |
| ModifTime | datetime | NO   | MUL | NULL    |                |
| NumNotif  | int(11)  | NO   |     | 0       |                |
| Subject   | text     | NO   |     | NULL    |                |
| Content   | longtext | NO   |     | NULL    |                |
| MedCod    | int(11)  | NO   | MUL | -1      |                |
+-----------+----------+------+-----+---------+----------------+
9 rows in set (0.00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS for_posts ("
			"PstCod INT NOT NULL AUTO_INCREMENT,"
			"ThrCod INT NOT NULL,"
			"UsrCod INT NOT NULL,"
			"CreatTime DATETIME NOT NULL,"
			"ModifTime DATETIME NOT NULL,"
			"NumNotif INT NOT NULL DEFAULT 0,"
			"Subject TEXT NOT NULL,"			// Cns_MAX_BYTES_SUBJECT
			"Content LONGTEXT NOT NULL,"			// Cns_MAX_BYTES_LONG_TEXT
			"MedCod INT NOT NULL DEFAULT -1,"
		   "UNIQUE INDEX(PstCod),"
		   "INDEX(ThrCod),"
		   "INDEX(UsrCod),"
		   "INDEX(CreatTime),"
		   "INDEX(ModifTime),"
		   "INDEX(MedCod)"
		   ") ENGINE=MyISAM");

   /***** Table for_read *****/
/*
mysql> DESCRIBE for_read;
+----------+----------+------+-----+---------+-------+
| Field    | Type     | Null | Key | Default | Extra |
+----------+----------+------+-----+---------+-------+
| ThrCod   | int(11)  | NO   | PRI | NULL    |       |
| UsrCod   | int(11)  | NO   | PRI | NULL    |       |
| ReadTime | datetime | NO   |     | NULL    |       |
+----------+----------+------+-----+---------+-------+
3 rows in set (0.00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS for_read ("
			"ThrCod INT NOT NULL,"
			"UsrCod INT NOT NULL,"
			"ReadTime DATETIME NOT NULL,"
		   "UNIQUE INDEX(ThrCod,UsrCod)"
		   ") ENGINE=MyISAM");

   /***** Table for_threads *****/
/*
mysql> DESCRIBE for_threads;
+-------------+------------+------+-----+---------+----------------+
| Field       | Type       | Null | Key | Default | Extra          |
+-------------+------------+------+-----+---------+----------------+
| ThrCod      | int(11)    | NO   | PRI | NULL    | auto_increment |
| ForumType   | tinyint(4) | NO   | MUL | NULL    |                |
| HieCod      | int(11)    | NO   | MUL | -1      |                |
| FirstPstCod | int(11)    | NO   | UNI | NULL    |                |
| LastPstCod  | int(11)    | NO   | UNI | NULL    |                |
+-------------+------------+------+-----+---------+----------------+
5 rows in set (0.00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS for_threads ("
			"ThrCod INT NOT NULL AUTO_INCREMENT,"
			"ForumType TINYINT NOT NULL,"
			"HieCod INT NOT NULL DEFAULT -1,"
			"FirstPstCod INT NOT NULL,"
			"LastPstCod INT NOT NULL,"
		   "UNIQUE INDEX(ThrCod),"
		   "INDEX(ForumType),"
		   "INDEX(HieCod),"
		   "UNIQUE INDEX(FirstPstCod),"
		   "UNIQUE INDEX(LastPstCod)"
		   ") ENGINE=MyISAM");

   /***** Table gam_games *****/
/*
mysql> DESCRIBE gam_games;
+------------+---------------+------+-----+---------+----------------+
| Field      | Type          | Null | Key | Default | Extra          |
+------------+---------------+------+-----+---------+----------------+
| GamCod     | int(11)       | NO   | PRI | NULL    | auto_increment |
| CrsCod     | int(11)       | NO   | MUL | -1      |                |
| Hidden     | enum('N','Y') | NO   |     | N       |                |
| UsrCod     | int(11)       | NO   |     | NULL    |                |
| MaxGrade   | double        | NO   |     | 1       |                |
| Visibility | int(11)       | NO   |     | 31      |                |
| Title      | varchar(2047) | NO   |     | NULL    |                |
| Txt        | text          | NO   |     | NULL    |                |
+------------+---------------+------+-----+---------+----------------+
8 rows in set (0.00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS gam_games ("
			"GamCod INT NOT NULL AUTO_INCREMENT,"
			"CrsCod INT NOT NULL DEFAULT -1,"
			"Hidden ENUM('N','Y') NOT NULL DEFAULT 'N',"
			"UsrCod INT NOT NULL,"
			"MaxGrade DOUBLE PRECISION NOT NULL DEFAULT 1,"	// Scale from score [0...num.answers] to grade [0...MaxGrade]
			"Visibility INT NOT NULL DEFAULT 0x1f,"
			"Title VARCHAR(2047) NOT NULL,"	// Gam_MAX_BYTES_TITLE
			"Txt TEXT NOT NULL,"		// Cns_MAX_BYTES_TEXT
		   "UNIQUE INDEX(GamCod),"
		   "INDEX(CrsCod)"
		   ") ENGINE=MyISAM");

   /***** Table gam_questions *****/
/*
mysql> DESCRIBE gam_questions;
+--------+---------+------+-----+---------+-------+
| Field  | Type    | Null | Key | Default | Extra |
+--------+---------+------+-----+---------+-------+
| GamCod | int(11) | NO   | PRI | NULL    |       |
| QstInd | int(11) | NO   | PRI | NULL    |       |
| QstCod | int(11) | NO   |     | NULL    |       |
+--------+---------+------+-----+---------+-------+
3 rows in set (0.00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS gam_questions ("
			"GamCod INT NOT NULL,"
			"QstInd INT NOT NULL,"
			"QstCod INT NOT NULL,"
		   "UNIQUE INDEX(GamCod,QstInd),"
		   "UNIQUE INDEX(GamCod,QstCod)"
		   ") ENGINE=MyISAM");

   /***** Table grp_groups *****/
/*
mysql> DESCRIBE grp_groups;
+-------------+---------------+------+-----+---------+----------------+
| Field       | Type          | Null | Key | Default | Extra          |
+-------------+---------------+------+-----+---------+----------------+
| GrpCod      | int(11)       | NO   | PRI | NULL    | auto_increment |
| GrpTypCod   | int(11)       | NO   | MUL | NULL    |                |
| GrpName     | varchar(2047) | NO   |     | NULL    |                |
| RooCod      | int(11)       | NO   | MUL | -1      |                |
| MaxStudents | int(11)       | NO   |     | NULL    |                |
| Open        | enum('N','Y') | NO   |     | N       |                |
| FileZones   | enum('N','Y') | NO   |     | N       |                |
+-------------+---------------+------+-----+---------+----------------+
7 rows in set (0.00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS grp_groups ("
			"GrpCod INT NOT NULL AUTO_INCREMENT,"
			"GrpTypCod INT NOT NULL,"
			"GrpName VARCHAR(2047) NOT NULL,"	// Grp_MAX_BYTES_GROUP_NAME
			"MaxStudents INT NOT NULL,"
			"RooCod INT NOT NULL DEFAULT -1,"
			"Open ENUM('N','Y') NOT NULL DEFAULT 'N',"
			"FileZones ENUM('N','Y') NOT NULL DEFAULT 'N',"
		   "UNIQUE INDEX(GrpCod),"
		   "INDEX(GrpTypCod),"
		   "INDEX(RooCod)"
		   ") ENGINE=MyISAM");

   /***** Table grp_types *****/
/*
mysql> DESCRIBE grp_types;
+--------------+---------------+------+-----+---------+----------------+
| Field        | Type          | Null | Key | Default | Extra          |
+--------------+---------------+------+-----+---------+----------------+
| GrpTypCod    | int(11)       | NO   | PRI | NULL    | auto_increment |
| CrsCod       | int(11)       | NO   | MUL | -1      |                |
| GrpTypName   | varchar(2047) | NO   |     | NULL    |                |
| Mandatory    | enum('N','Y') | NO   |     | NULL    |                |
| Multiple     | enum('N','Y') | NO   |     | NULL    |                |
| MustBeOpened | enum('N','Y') | NO   |     | N       |                |
| OpenTime     | datetime      | NO   |     | NULL    |                |
+--------------+---------------+------+-----+---------+----------------+
7 rows in set (0,00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS grp_types ("
			"GrpTypCod INT NOT NULL AUTO_INCREMENT,"
			"CrsCod INT NOT NULL DEFAULT -1,"
			"GrpTypName VARCHAR(2047) NOT NULL,"	// Grp_MAX_BYTES_GROUP_TYPE_NAME
			"Mandatory ENUM('N','Y') NOT NULL,"
			"Multiple ENUM('N','Y') NOT NULL,"
			"MustBeOpened ENUM('N','Y') NOT NULL DEFAULT 'N',"
			"OpenTime DATETIME NOT NULL,"
		   "UNIQUE INDEX(GrpTypCod),"
		   "INDEX(CrsCod)"
		   ") ENGINE=MyISAM");

   /***** Table grp_users *****/
/*
mysql> DESCRIBE grp_users;
+--------+---------+------+-----+---------+-------+
| Field  | Type    | Null | Key | Default | Extra |
+--------+---------+------+-----+---------+-------+
| GrpCod | int(11) | NO   | PRI | NULL    |       |
| UsrCod | int(11) | NO   | PRI | NULL    |       |
+--------+---------+------+-----+---------+-------+
2 rows in set (0.00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS grp_users ("
			"GrpCod INT NOT NULL,"
			"UsrCod INT NOT NULL,"
		   "UNIQUE INDEX(GrpCod,UsrCod),"
		   "INDEX(GrpCod),"
		   "INDEX(UsrCod)"
		   ") ENGINE=MyISAM");

   /***** Table hld_holidays *****/
/*
mysql> DESCRIBE hld_holidays;
+-----------+---------------+------+-----+---------+----------------+
| Field     | Type          | Null | Key | Default | Extra          |
+-----------+---------------+------+-----+---------+----------------+
| HldCod    | int(11)       | NO   | PRI | NULL    | auto_increment |
| InsCod    | int(11)       | NO   | MUL | NULL    |                |
| PlcCod    | int(11)       | NO   | MUL | -1      |                |
| HldTyp    | tinyint(4)    | NO   |     | NULL    |                |
| StartDate | date          | NO   |     | NULL    |                |
| EndDate   | date          | NO   |     | NULL    |                |
| Name      | varchar(2047) | NO   |     | NULL    |                |
+-----------+---------------+------+-----+---------+----------------+
7 rows in set (0,00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS hld_holidays ("
			"HldCod INT NOT NULL AUTO_INCREMENT,"
			"InsCod INT NOT NULL,"
			"PlcCod INT NOT NULL DEFAULT -1,"
			"HldTyp TINYINT NOT NULL,"
			"StartDate DATE NOT NULL,"
			"EndDate DATE NOT NULL,"
			"Name VARCHAR(2047) NOT NULL,"	// Hld_MAX_BYTES_HOLIDAY_NAME
		   "UNIQUE INDEX (HldCod),"
		   "INDEX(InsCod),"
		   "INDEX(PlcCod)"
		   ") ENGINE=MyISAM");

   /***** Table ins_instits *****/
/*
mysql> DESCRIBE ins_instits;
+-----------------+---------------+------+-----+---------+----------------+
| Field           | Type          | Null | Key | Default | Extra          |
+-----------------+---------------+------+-----+---------+----------------+
| InsCod          | int(11)       | NO   | PRI | NULL    | auto_increment |
| CtyCod          | int(11)       | NO   | MUL | NULL    |                |
| Status          | tinyint(4)    | NO   | MUL | 0       |                |
| RequesterUsrCod | int(11)       | NO   |     | -1      |                |
| ShortName       | varchar(511)  | NO   |     | NULL    |                |
| FullName        | varchar(2047) | NO   |     | NULL    |                |
| WWW             | varchar(255)  | NO   |     | NULL    |                |
+-----------------+---------------+------+-----+---------+----------------+
7 rows in set (0,00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS ins_instits ("
			"InsCod INT NOT NULL AUTO_INCREMENT,"
			"CtyCod INT NOT NULL,"
			"Status TINYINT NOT NULL DEFAULT 0,"
			"RequesterUsrCod INT NOT NULL DEFAULT -1,"
			"ShortName VARCHAR(511) COLLATE latin1_spanish_ci NOT NULL,"	// Nam_MAX_BYTES_SHRT_NAME
			"FullName VARCHAR(2047) COLLATE latin1_spanish_ci NOT NULL,"	// Nam_MAX_BYTES_FULL_NAME
			"WWW VARCHAR(255) NOT NULL,"					// WWW_MAX_BYTES_WWW
		   "UNIQUE INDEX(InsCod),"
		   "INDEX(CtyCod),"
		   "INDEX(Status)"
		   ") ENGINE=MyISAM");

   /***** Table lnk_links *****/
/*
mysql> DESCRIBE lnk_links;
+-----------+---------------+------+-----+---------+----------------+
| Field     | Type          | Null | Key | Default | Extra          |
+-----------+---------------+------+-----+---------+----------------+
| LnkCod    | int(11)       | NO   | PRI | NULL    | auto_increment |
| ShortName | varchar(511)  | NO   |     | NULL    |                |
| FullName  | varchar(2047) | NO   |     | NULL    |                |
| WWW       | varchar(255)  | NO   |     | NULL    |                |
+-----------+---------------+------+-----+---------+----------------+
4 rows in set (0,00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS lnk_links ("
			"LnkCod INT NOT NULL AUTO_INCREMENT,"
			"ShortName VARCHAR(511) NOT NULL,"	// Nam_MAX_BYTES_SHRT_NAME
			"FullName VARCHAR(2047) NOT NULL,"	// Nam_MAX_BYTES_FULL_NAME
			"WWW VARCHAR(255) NOT NULL,"		// WWW_MAX_BYTES_WWW
		   "UNIQUE INDEX(LnkCod)"
		   ") ENGINE=MyISAM");

   /***** Table log *****/
/*
mysql> DESCRIBE log;
+----------------+------------+------+-----+---------+----------------+
| Field          | Type       | Null | Key | Default | Extra          |
+----------------+------------+------+-----+---------+----------------+
| LogCod         | int(11)    | NO   | PRI | NULL    | auto_increment |
| ActCod         | int(11)    | NO   | MUL | -1      |                |
| CtyCod         | int(11)    | NO   | MUL | -1      |                |
| InsCod         | int(11)    | NO   | MUL | -1      |                |
| CtrCod         | int(11)    | NO   | MUL | -1      |                |
| DegCod         | int(11)    | NO   | MUL | -1      |                |
| CrsCod         | int(11)    | NO   | MUL | -1      |                |
| UsrCod         | int(11)    | NO   | MUL | -1      |                |
| Role           | tinyint(4) | NO   |     | NULL    |                |
| ClickTime      | datetime   | NO   | PRI | NULL    |                |
| TimeToGenerate | int(11)    | NO   |     | NULL    |                |
| TimeToSend     | int(11)    | NO   |     | NULL    |                |
| IP             | char(15)   | NO   |     | NULL    |                |
+----------------+------------+------+-----+---------+----------------+
13 rows in set (0.00 sec)
*/
// TODO: Change NtfCod and LogCod from INT to BIGINT in database tables.
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS log ("
			"LogCod INT NOT NULL AUTO_INCREMENT,"
			"ActCod INT NOT NULL DEFAULT -1,"
			"CtyCod INT NOT NULL DEFAULT -1,"
			"InsCod INT NOT NULL DEFAULT -1,"
			"CtrCod INT NOT NULL DEFAULT -1,"
			"DegCod INT NOT NULL DEFAULT -1,"
			"CrsCod INT NOT NULL DEFAULT -1,"
			"UsrCod INT NOT NULL DEFAULT -1,"
			"Role TINYINT NOT NULL,"
			"ClickTime DATETIME NOT NULL,"
			"TimeToGenerate INT NOT NULL,"
			"TimeToSend INT NOT NULL,"
			"IP CHAR(15) NOT NULL,"
			"PRIMARY KEY(LogCod,ClickTime),"
			"INDEX(ActCod),"
			"INDEX(CtyCod),"
			"INDEX(InsCod),"
			"INDEX(CtrCod),"
			"INDEX(DegCod),"
			"INDEX(CrsCod),"
			"INDEX(UsrCod),"
			"INDEX(ClickTime,Role)"
			") ENGINE=InnoDB"
			" PARTITION BY RANGE (YEAR(ClickTime))"
			" ("
			"PARTITION p2004 VALUES LESS THAN (2005),"
			"PARTITION p2005 VALUES LESS THAN (2006),"
			"PARTITION p2006 VALUES LESS THAN (2007),"
			"PARTITION p2007 VALUES LESS THAN (2008),"
			"PARTITION p2008 VALUES LESS THAN (2009),"
			"PARTITION p2009 VALUES LESS THAN (2010),"
			"PARTITION p2010 VALUES LESS THAN (2011),"
			"PARTITION p2011 VALUES LESS THAN (2012),"
			"PARTITION p2012 VALUES LESS THAN (2013),"
			"PARTITION p2013 VALUES LESS THAN (2014),"
			"PARTITION p2014 VALUES LESS THAN (2015),"
			"PARTITION p2015 VALUES LESS THAN (2016),"
			"PARTITION p2016 VALUES LESS THAN (2017),"
			"PARTITION p2017 VALUES LESS THAN (2018),"
			"PARTITION p2018 VALUES LESS THAN (2019),"
			"PARTITION p2019 VALUES LESS THAN (2020),"
			"PARTITION p2020 VALUES LESS THAN (2021),"
			"PARTITION p2021 VALUES LESS THAN (2022),"
			"PARTITION p2022 VALUES LESS THAN (2023),"
			"PARTITION p2023 VALUES LESS THAN (2024),"
			"PARTITION p2024 VALUES LESS THAN (2025),"
			"PARTITION p2025 VALUES LESS THAN (2026),"
			"PARTITION p2026 VALUES LESS THAN (2027),"
			"PARTITION p2027 VALUES LESS THAN (2028),"
			"PARTITION p2028 VALUES LESS THAN (2029),"
			"PARTITION p2029 VALUES LESS THAN (2030),"
			"PARTITION p2030 VALUES LESS THAN (2031),"
			"PARTITION p2031 VALUES LESS THAN (2032),"
			"PARTITION p2032 VALUES LESS THAN (2033),"
			"PARTITION p2033 VALUES LESS THAN (2034),"
			"PARTITION p2034 VALUES LESS THAN (2035),"
			"PARTITION p2035 VALUES LESS THAN (2036),"
			"PARTITION p2036 VALUES LESS THAN (2037),"
			"PARTITION p2037 VALUES LESS THAN (2038),"
			"PARTITION p2038 VALUES LESS THAN (2039),"
			"PARTITION p2039 VALUES LESS THAN (2040),"
			"PARTITION p2040 VALUES LESS THAN (2041),"
			"PARTITION p2041 VALUES LESS THAN (2042),"
			"PARTITION p2042 VALUES LESS THAN (2043),"
			"PARTITION p2043 VALUES LESS THAN (2044),"
			"PARTITION p2044 VALUES LESS THAN (2045),"
			"PARTITION p2045 VALUES LESS THAN (2046),"
			"PARTITION p2046 VALUES LESS THAN (2047),"
			"PARTITION p2047 VALUES LESS THAN (2048),"
			"PARTITION p2048 VALUES LESS THAN (2049),"
			"PARTITION p2049 VALUES LESS THAN (2050),"
			"PARTITION p2050 VALUES LESS THAN MAXVALUE"
			")");

   /***** Table log_api *****/
/*
mysql> DESCRIBE log_api;
+--------+---------+------+-----+---------+-------+
| Field  | Type    | Null | Key | Default | Extra |
+--------+---------+------+-----+---------+-------+
| LogCod | int(11) | NO   | PRI | NULL    |       |
| PlgCod | int(11) | NO   | MUL | NULL    |       |
| FunCod | int(11) | NO   | MUL | NULL    |       |
+--------+---------+------+-----+---------+-------+
3 rows in set (0.00 sec)
*/
// TODO: Change NtfCod and LogCod from INT to BIGINT in database tables.
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS log_api ("
			"LogCod INT NOT NULL,"
			"PlgCod INT NOT NULL,"
			"FunCod INT NOT NULL,"
		   "UNIQUE INDEX(LogCod),"
		   "INDEX(PlgCod),"
		   "INDEX(FunCod)"
		   ") ENGINE=MyISAM");

   /***** Table log_banners *****/
/*
mysql> DESCRIBE log_banners;
+--------+---------+------+-----+---------+-------+
| Field  | Type    | Null | Key | Default | Extra |
+--------+---------+------+-----+---------+-------+
| LogCod | int(11) | NO   | PRI | NULL    |       |
| BanCod | int(11) | NO   | MUL | NULL    |       |
+--------+---------+------+-----+---------+-------+
2 rows in set (0.00 sec)
*/
// TODO: Change NtfCod and LogCod from INT to BIGINT in database tables.
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS log_banners ("
			"LogCod INT NOT NULL,"
			"BanCod INT NOT NULL,"
		   "UNIQUE INDEX(LogCod),"
		   "INDEX(BanCod)"
		   ") ENGINE=MyISAM");

   /***** Table log_comments *****/
/*
mysql> DESCRIBE log_comments;
+----------+---------+------+-----+---------+-------+
| Field    | Type    | Null | Key | Default | Extra |
+----------+---------+------+-----+---------+-------+
| LogCod   | int(11) | NO   | PRI | NULL    |       |
| Comments | text    | NO   |     | NULL    |       |
+----------+---------+------+-----+---------+-------+
2 rows in set (0,00 sec)
*/
// TODO: Change NtfCod and LogCod from INT to BIGINT in database tables.
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS log_comments ("
			"LogCod INT NOT NULL,"
			"Comments TEXT NOT NULL,"
		   "UNIQUE INDEX(LogCod)"
		   ") ENGINE=MyISAM");

   /***** Table log_recent *****/
/*
mysql> DESCRIBE log_recent;
+----------------+------------+------+-----+---------+-------+
| Field          | Type       | Null | Key | Default | Extra |
+----------------+------------+------+-----+---------+-------+
| LogCod         | int(11)    | NO   | PRI | NULL    |       |
| ActCod         | int(11)    | NO   | MUL | -1      |       |
| CtyCod         | int(11)    | NO   | MUL | -1      |       |
| InsCod         | int(11)    | NO   | MUL | -1      |       |
| CtrCod         | int(11)    | NO   | MUL | -1      |       |
| DegCod         | int(11)    | NO   | MUL | -1      |       |
| CrsCod         | int(11)    | NO   | MUL | -1      |       |
| UsrCod         | int(11)    | NO   | MUL | -1      |       |
| Role           | tinyint(4) | NO   |     | NULL    |       |
| ClickTime      | datetime   | NO   | MUL | NULL    |       |
| TimeToGenerate | int(11)    | NO   |     | NULL    |       |
| TimeToSend     | int(11)    | NO   |     | NULL    |       |
| IP             | char(15)   | NO   |     | NULL    |       |
+----------------+------------+------+-----+---------+-------+
13 rows in set (0.01 sec)
*/
// TODO: Change NtfCod and LogCod from INT to BIGINT in database tables.
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS log_recent ("
			"LogCod INT NOT NULL,"
			"ActCod INT NOT NULL DEFAULT -1,"
			"CtyCod INT NOT NULL DEFAULT -1,"
			"InsCod INT NOT NULL DEFAULT -1,"
			"CtrCod INT NOT NULL DEFAULT -1,"
			"DegCod INT NOT NULL DEFAULT -1,"
			"CrsCod INT NOT NULL DEFAULT -1,"
			"UsrCod INT NOT NULL DEFAULT -1,"
			"Role TINYINT NOT NULL,"
			"ClickTime DATETIME NOT NULL,"
			"TimeToGenerate INT NOT NULL,"
			"TimeToSend INT NOT NULL,"
			"IP CHAR(15) NOT NULL,"	// Cns_MAX_BYTES_IP
		   "UNIQUE INDEX(LogCod),"
		   "INDEX(ActCod),"
		   "INDEX(CtyCod),"
		   "INDEX(InsCod),"
		   "INDEX(CtrCod),"
		   "INDEX(DegCod),"
		   "INDEX(CrsCod),"
		   "INDEX(UsrCod),"
		   "INDEX(ClickTime,Role)"
		   ") ENGINE=MyISAM");

   /***** Table log_search *****/
/*
mysql> DESCRIBE log_search;
+-----------+---------------+------+-----+---------+-------+
| Field     | Type          | Null | Key | Default | Extra |
+-----------+---------------+------+-----+---------+-------+
| LogCod    | int(11)       | NO   | PRI | NULL    |       |
| SearchStr | varchar(2047) | NO   |     | NULL    |       |
+-----------+---------------+------+-----+---------+-------+
2 rows in set (0,00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS log_search ("
			"LogCod INT NOT NULL,"
			"SearchStr VARCHAR(2047) NOT NULL,"	// Sch_MAX_BYTES_STRING_TO_FIND
		   "UNIQUE INDEX(LogCod)"
		   ") ENGINE=MyISAM");

   /***** Table mch_answers *****/
/*
mysql> DESCRIBE mch_answers;
+--------+------------+------+-----+---------+-------+
| Field  | Type       | Null | Key | Default | Extra |
+--------+------------+------+-----+---------+-------+
| MchCod | int(11)    | NO   | PRI | NULL    |       |
| UsrCod | int(11)    | NO   | PRI | NULL    |       |
| QstInd | int(11)    | NO   | PRI | NULL    |       |
| NumOpt | tinyint(4) | NO   |     | NULL    |       |
| AnsInd | tinyint(4) | NO   |     | NULL    |       |
+--------+------------+------+-----+---------+-------+
5 rows in set (0.00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS mch_answers ("
			"MchCod INT NOT NULL,"
	                "UsrCod INT NOT NULL,"
			"QstInd INT NOT NULL,"
			"NumOpt TINYINT NOT NULL,"	// Number of button on screen (Always ordered: 0,1,2,3)
			"AnsInd TINYINT NOT NULL,"	// Answer index (Can be shuffled: 0,3,1,2)
		   "UNIQUE INDEX(MchCod,UsrCod,QstInd)"
		   ") ENGINE=MyISAM");

   /***** Table mch_groups *****/
/*
mysql> DESCRIBE mch_groups;
+--------+---------+------+-----+---------+-------+
| Field  | Type    | Null | Key | Default | Extra |
+--------+---------+------+-----+---------+-------+
| MchCod | int(11) | NO   | PRI | NULL    |       |
| GrpCod | int(11) | NO   | PRI | NULL    |       |
+--------+---------+------+-----+---------+-------+
2 rows in set (0.01 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS mch_groups ("
			"MchCod INT NOT NULL,"
			"GrpCod INT NOT NULL,"
		   "UNIQUE INDEX(MchCod,GrpCod)"
		   ") ENGINE=MyISAM");

   /***** Table mch_indexes *****/
/*
mysql> DESCRIBE mch_indexes;
+---------+---------+------+-----+---------+-------+
| Field   | Type    | Null | Key | Default | Extra |
+---------+---------+------+-----+---------+-------+
| MchCod  | int(11) | NO   | PRI | NULL    |       |
| QstInd  | int(11) | NO   | PRI | NULL    |       |
| Indexes | text    | NO   |     | NULL    |       |
+---------+---------+------+-----+---------+-------+
3 rows in set (0.00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS mch_indexes ("
			"MchCod INT NOT NULL,"
			"QstInd INT NOT NULL,"
			"Indexes TEXT NOT NULL,"	// Tst_MAX_BYTES_INDEXES_ONE_QST
		   "UNIQUE INDEX(MchCod,QstInd)"
		   ") ENGINE=MyISAM");

   /***** Table mch_matches *****/
/*
mysql> DESCRIBE mch_matches;
+----------------+------------------------------------------------+------+-----+---------+----------------+
| Field          | Type                                           | Null | Key | Default | Extra          |
+----------------+------------------------------------------------+------+-----+---------+----------------+
| MchCod         | int(11)                                        | NO   | PRI | NULL    | auto_increment |
| GamCod         | int(11)                                        | NO   | MUL | NULL    |                |
| UsrCod         | int(11)                                        | NO   |     | NULL    |                |
| StartTime      | datetime                                       | NO   |     | NULL    |                |
| EndTime        | datetime                                       | NO   |     | NULL    |                |
| Title          | varchar(2047)                                  | NO   |     | NULL    |                |
| QstInd         | int(11)                                        | NO   |     | 0       |                |
| QstCod         | int(11)                                        | NO   |     | -1      |                |
| Showing        | enum('start','stem','answers','results','end') | NO   |     | start   |                |
| Countdown      | int(11)                                        | NO   |     | -1      |                |
| NumCols        | int(11)                                        | NO   |     | 1       |                |
| ShowQstResults | enum('N','Y')                                  | NO   |     | N       |                |
| ShowUsrResults | enum('N','Y')                                  | NO   |     | N       |                |
+----------------+------------------------------------------------+------+-----+---------+----------------+
13 rows in set (0.00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS mch_matches ("
			"MchCod INT NOT NULL AUTO_INCREMENT,"
			"GamCod INT NOT NULL,"
			"UsrCod INT NOT NULL,"
			"StartTime DATETIME NOT NULL,"
			"EndTime DATETIME NOT NULL,"
			"Title VARCHAR(2047) NOT NULL,"	// Mch_MAX_BYTES_TITLE
			"QstInd INT NOT NULL DEFAULT 0,"
			"QstCod INT NOT NULL DEFAULT -1,"
			"Showing ENUM('start','stem','answers','results','end') NOT NULL DEFAULT 'start',"
			"Countdown INT NOT NULL DEFAULT -1,"
		        "NumCols INT NOT NULL DEFAULT 1,"
			"ShowQstResults ENUM('N','Y') NOT NULL DEFAULT 'N',"
			"ShowUsrResults ENUM('N','Y') NOT NULL DEFAULT 'N',"
		   "UNIQUE INDEX(MchCod),"
		   "INDEX(GamCod)"
		   ") ENGINE=MyISAM");

   /***** Table mch_players *****/
/*
mysql> DESCRIBE mch_players;
+--------+-----------+------+-----+-------------------+-----------------------------+
| Field  | Type      | Null | Key | Default           | Extra                       |
+--------+-----------+------+-----+-------------------+-----------------------------+
| MchCod | int(11)   | NO   | PRI | NULL              |                             |
| UsrCod | int(11)   | NO   | PRI | NULL              |                             |
| TS     | timestamp | NO   |     | CURRENT_TIMESTAMP | on update CURRENT_TIMESTAMP |
+--------+-----------+------+-----+-------------------+-----------------------------+
3 rows in set (0.00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS mch_players ("
			"MchCod INT NOT NULL,"
	                "UsrCod INT NOT NULL,"
		        "TS TIMESTAMP,"
		   "UNIQUE INDEX(MchCod,UsrCod)"
		   ") ENGINE=MyISAM");

   /***** Table mch_playing *****/
/*
mysql> DESCRIBE mch_playing;
+--------+-----------+------+-----+-------------------+-----------------------------+
| Field  | Type      | Null | Key | Default           | Extra                       |
+--------+-----------+------+-----+-------------------+-----------------------------+
| MchCod | int(11)   | NO   | PRI | NULL              |                             |
| TS     | timestamp | NO   |     | CURRENT_TIMESTAMP | on update CURRENT_TIMESTAMP |
+--------+-----------+------+-----+-------------------+-----------------------------+
2 rows in set (0.01 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS mch_playing ("
			"MchCod INT NOT NULL,"
		        "TS TIMESTAMP,"
		   "UNIQUE INDEX(MchCod)"
		   ") ENGINE=MyISAM");

   /***** Table mch_results *****/
/*
mysql> DESCRIBE mch_results;
+-----------------+----------+------+-----+---------+-------+
| Field           | Type     | Null | Key | Default | Extra |
+-----------------+----------+------+-----+---------+-------+
| MchCod          | int(11)  | NO   | PRI | NULL    |       |
| UsrCod          | int(11)  | NO   | PRI | NULL    |       |
| StartTime       | datetime | NO   |     | NULL    |       |
| EndTime         | datetime | NO   |     | NULL    |       |
| NumQsts         | int(11)  | NO   |     | 0       |       |
| NumQstsNotBlank | int(11)  | NO   |     | 0       |       |
| Score           | double   | NO   |     | 0       |       |
+-----------------+----------+------+-----+---------+-------+
7 rows in set (0.00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS mch_results ("
			"MchCod INT NOT NULL,"
			"UsrCod INT NOT NULL,"
			"StartTime DATETIME NOT NULL,"	// Time this user started to answer
			"EndTime DATETIME NOT NULL,"	// Time this user finished to answer
			"NumQsts INT NOT NULL DEFAULT 0,"
			"NumQstsNotBlank INT NOT NULL DEFAULT 0,"
			"Score DOUBLE PRECISION NOT NULL DEFAULT 0,"
		   "UNIQUE INDEX(MchCod,UsrCod)"
		   ") ENGINE=MyISAM");

      /***** Table mch_times *****/
/*
mysql> DESCRIBE mch_times;
+-------------+---------+------+-----+----------+-------+
| Field       | Type    | Null | Key | Default  | Extra |
+-------------+---------+------+-----+----------+-------+
| MchCod      | int(11) | NO   | PRI | NULL     |       |
| QstInd      | int(11) | NO   | PRI | NULL     |       |
| ElapsedTime | time    | NO   |     | 00:00:00 |       |
+-------------+---------+------+-----+----------+-------+
3 rows in set (0.00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS mch_times ("
			"MchCod INT NOT NULL,"
			"QstInd INT NOT NULL,"
			"ElapsedTime TIME NOT NULL DEFAULT 0,"
		   "UNIQUE INDEX(MchCod,QstInd)"
		   ") ENGINE=MyISAM");

   /***** Table med_media *****/
/*
mysql> DESCRIBE med_media;
+--------+---------------------------------------------------------------+------+-----+---------+----------------+
| Field  | Type                                                          | Null | Key | Default | Extra          |
+--------+---------------------------------------------------------------+------+-----+---------+----------------+
| MedCod | int(11)                                                       | NO   | PRI | NULL    | auto_increment |
| Type   | enum('none','jpg','gif','mp4','webm','ogg','youtube','embed') | NO   | MUL | none    |                |
| Name   | varchar(43)                                                   | NO   |     |         |                |
| URL    | varchar(255)                                                  | NO   |     |         |                |
| Title  | varchar(2047)                                                 | NO   |     |         |                |
+--------+---------------------------------------------------------------+------+-----+---------+----------------+
5 rows in set (0.01 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS med_media ("
			"MedCod INT NOT NULL AUTO_INCREMENT,"
			"Type ENUM("
			"'none',"
			"'jpg',"
			"'gif',"
			"'mp4',"
			"'webm',"
			"'ogg',"
			"'youtube',"
			"'embed') NOT NULL DEFAULT 'none',"
			"Name VARCHAR(43) NOT NULL DEFAULT '',"		// Med_BYTES_NAME
			"URL VARCHAR(255) NOT NULL DEFAULT '',"		// WWW_MAX_BYTES_WWW
			"Title VARCHAR(2047) NOT NULL DEFAULT '',"	// Med_MAX_BYTES_TITLE
		   "UNIQUE INDEX(MedCod),"
		   "INDEX(Type)"
		   ") ENGINE=MyISAM");

   /***** Table mrk_marks *****/
/*
mysql> DESCRIBE mrk_marks;
+--------+---------+------+-----+---------+-------+
| Field  | Type    | Null | Key | Default | Extra |
+--------+---------+------+-----+---------+-------+
| FilCod | int(11) | NO   | PRI | NULL    |       |
| Header | int(11) | NO   |     | NULL    |       |
| Footer | int(11) | NO   |     | NULL    |       |
+--------+---------+------+-----+---------+-------+
3 rows in set (0.00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS mrk_marks ("
			"FilCod INT NOT NULL,"
			"Header INT NOT NULL,"
			"Footer INT NOT NULL,"
		   "UNIQUE INDEX(FilCod)"
		   ") ENGINE=MyISAM");

   /***** Table msg_banned *****/
/*
mysql> DESCRIBE msg_banned;
+------------+---------+------+-----+---------+-------+
| Field      | Type    | Null | Key | Default | Extra |
+------------+---------+------+-----+---------+-------+
| FromUsrCod | int(11) | NO   | PRI | NULL    |       |
| ToUsrCod   | int(11) | NO   | PRI | NULL    |       |
+------------+---------+------+-----+---------+-------+
2 rows in set (0.00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS msg_banned ("
			"FromUsrCod INT NOT NULL,"
			"ToUsrCod INT NOT NULL,"
		   "UNIQUE INDEX(FromUsrCod,ToUsrCod)"
		   ") ENGINE=MyISAM");

   /***** Table msg_content *****/
/*
mysql> DESCRIBE msg_content;
+---------+----------+------+-----+---------+----------------+
| Field   | Type     | Null | Key | Default | Extra          |
+---------+----------+------+-----+---------+----------------+
| MsgCod  | int(11)  | NO   | PRI | NULL    | auto_increment |
| Subject | text     | NO   | MUL | NULL    |                |
| Content | longtext | NO   |     | NULL    |                |
| MedCod  | int(11)  | NO   | MUL | -1      |                |
+---------+----------+------+-----+---------+----------------+
4 rows in set (0.00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS msg_content ("
			"MsgCod INT NOT NULL AUTO_INCREMENT,"
			"Subject TEXT NOT NULL,"
			"Content LONGTEXT NOT NULL,"
			"MedCod INT NOT NULL DEFAULT -1,"
		   "UNIQUE INDEX(MsgCod),"
		   "FULLTEXT(Subject,Content),"
		   "INDEX(MedCod)"
		   ") ENGINE=MyISAM");

   /***** Table msg_content_deleted *****/
   /* TODO: Messages in msg_content_deleted older than a certain time
      should be deleted to ensure the protection of personal data */
/*
mysql> DESCRIBE msg_content_deleted;
+---------+----------+------+-----+---------+-------+
| Field   | Type     | Null | Key | Default | Extra |
+---------+----------+------+-----+---------+-------+
| MsgCod  | int(11)  | NO   | PRI | NULL    |       |
| Subject | text     | NO   | MUL | NULL    |       |
| Content | longtext | NO   |     | NULL    |       |
| MedCod  | int(11)  | NO   | MUL | -1      |       |
+---------+----------+------+-----+---------+-------+
4 rows in set (0.00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS msg_content_deleted ("
			"MsgCod INT NOT NULL,"
			"Subject TEXT NOT NULL,"
			"Content LONGTEXT NOT NULL,"
			"MedCod INT NOT NULL DEFAULT -1,"
		   "UNIQUE INDEX(MsgCod),"
		   "FULLTEXT(Subject,Content),"
		   "INDEX(MedCod)"
		   ") ENGINE=MyISAM");

   /***** Table msg_rcv *****/
/*
mysql> DESCRIBE msg_rcv;
+----------+---------------+------+-----+---------+-------+
| Field    | Type          | Null | Key | Default | Extra |
+----------+---------------+------+-----+---------+-------+
| MsgCod   | int(11)       | NO   | PRI | 0       |       |
| UsrCod   | int(11)       | NO   | PRI | NULL    |       |
| Notified | enum('N','Y') | NO   | MUL | N       |       |
| Open     | enum('N','Y') | NO   |     | N       |       |
| Replied  | enum('N','Y') | NO   |     | N       |       |
| Expanded | enum('N','Y') | NO   |     | N       |       |
+----------+---------------+------+-----+---------+-------+
6 rows in set (0.00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS msg_rcv ("
			"MsgCod INT NOT NULL,"
			"UsrCod INT NOT NULL,"
			"Notified ENUM('N','Y') NOT NULL DEFAULT 'N',"
			"Open ENUM('N','Y') NOT NULL DEFAULT 'N',"
			"Replied ENUM('N','Y') NOT NULL DEFAULT 'N',"
			"Expanded ENUM('N','Y') NOT NULL DEFAULT 'N',"
		   "UNIQUE INDEX (UsrCod,MsgCod),"
		   "INDEX(MsgCod),"
		   "INDEX(Notified)"
		   ") ENGINE=MyISAM");

   /***** Table msg_rcv_deleted *****/
/*
mysql> DESCRIBE msg_rcv_deleted;
+----------+---------------+------+-----+---------+-------+
| Field    | Type          | Null | Key | Default | Extra |
+----------+---------------+------+-----+---------+-------+
| MsgCod   | int(11)       | NO   | PRI | NULL    |       |
| UsrCod   | int(11)       | NO   | PRI | NULL    |       |
| Notified | enum('N','Y') | NO   | MUL | N       |       |
| Open     | enum('N','Y') | NO   |     | N       |       |
| Replied  | enum('N','Y') | NO   |     | N       |       |
+----------+---------------+------+-----+---------+-------+
5 rows in set (0.00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS msg_rcv_deleted ("
			"MsgCod INT NOT NULL,"
			"UsrCod INT NOT NULL,"
			"Notified ENUM('N','Y') NOT NULL DEFAULT 'N',"
			"Open ENUM('N','Y') NOT NULL DEFAULT 'N',"
			"Replied ENUM('N','Y') NOT NULL DEFAULT 'N',"
		   "UNIQUE INDEX(UsrCod,MsgCod),"
		   "INDEX(MsgCod),"
		   "INDEX(Notified)"
		   ") ENGINE=MyISAM");

   /***** Table msg_snt *****/
/*
mysql> DESCRIBE msg_snt;
+-----------+---------------+------+-----+---------------------+-------+
| Field     | Type          | Null | Key | Default             | Extra |
+-----------+---------------+------+-----+---------------------+-------+
| MsgCod    | int(11)       | NO   | PRI | NULL                |       |
| CrsCod    | int(11)       | NO   | MUL | -1                  |       |
| UsrCod    | int(11)       | NO   | MUL | NULL                |       |
| Expanded  | enum('N','Y') | NO   |     | N                   |       |
| CreatTime | datetime      | NO   | MUL | 0000-00-00 00:00:00 |       |
+-----------+---------------+------+-----+---------------------+-------+
5 rows in set (0.00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS msg_snt ("
			"MsgCod INT NOT NULL,"
			"CrsCod INT NOT NULL DEFAULT -1,"
			"UsrCod INT NOT NULL,"
			"Expanded ENUM('N','Y') NOT NULL DEFAULT 'N',"
			"CreatTime DATETIME NOT NULL,"
		   "UNIQUE INDEX(MsgCod),"
		   "INDEX(CrsCod),"
		   "INDEX(UsrCod)"
		   ") ENGINE=MyISAM");

   /***** Table msg_snt_deleted *****/
/*
mysql> DESCRIBE msg_snt_deleted;
+-----------+----------+------+-----+---------+-------+
| Field     | Type     | Null | Key | Default | Extra |
+-----------+----------+------+-----+---------+-------+
| MsgCod    | int(11)  | NO   | PRI | NULL    |       |
| CrsCod    | int(11)  | NO   | MUL | -1      |       |
| UsrCod    | int(11)  | NO   | MUL | NULL    |       |
| CreatTime | datetime | NO   | MUL | NULL    |       |
+-----------+----------+------+-----+---------+-------+
4 rows in set (0.00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS msg_snt_deleted ("
			"MsgCod INT NOT NULL,"
			"CrsCod INT NOT NULL DEFAULT -1,"
			"UsrCod INT NOT NULL,"
			"CreatTime DATETIME NOT NULL,"
		   "UNIQUE INDEX(MsgCod),"
		   "INDEX(CrsCod),"
		   "INDEX(UsrCod)"
		   ") ENGINE=MyISAM");

   /***** Table not_deleted *****/
/*
mysql> DESCRIBE not_deleted;
+-----------+---------------+------+-----+---------+-------+
| Field     | Type          | Null | Key | Default | Extra |
+-----------+---------------+------+-----+---------+-------+
| NotCod    | int           | NO   | PRI | NULL    |       |
| CrsCod    | int           | NO   | MUL | -1      |       |
| UsrCod    | int           | NO   | MUL | NULL    |       |
| CreatTime | datetime      | NO   | MUL | NULL    |       |
| Content   | text          | NO   |     | NULL    |       |
| Public    | enum('N','Y') | NO   |     | Y       |       |
| NumNotif  | int           | NO   |     | 0       |       |
+-----------+---------------+------+-----+---------+-------+
7 rows in set (0,00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS not_deleted ("
			"NotCod INT NOT NULL,"
			"CrsCod INT NOT NULL DEFAULT -1,"
			"UsrCod INT NOT NULL,"
			"CreatTime DATETIME NOT NULL,"
			"Content TEXT NOT NULL,"
			"Public ENUM('N','Y') NOT NULL DEFAULT 'Y',"
			"NumNotif INT NOT NULL DEFAULT 0,"
		   "UNIQUE INDEX(NotCod),"
		   "INDEX(CrsCod),"
		   "INDEX(UsrCod),"
		   "INDEX(CreatTime)"
		   ") ENGINE=MyISAM");

   /***** Table not_notices *****/
/*
mysql> DESCRIBE not_notices;
+-----------+---------------+------+-----+---------+----------------+
| Field     | Type          | Null | Key | Default | Extra          |
+-----------+---------------+------+-----+---------+----------------+
| NotCod    | int           | NO   | PRI | NULL    | auto_increment |
| CrsCod    | int           | NO   | MUL | -1      |                |
| UsrCod    | int           | NO   | MUL | NULL    |                |
| CreatTime | datetime      | NO   | MUL | NULL    |                |
| Content   | text          | NO   |     | NULL    |                |
| Public    | enum('N','Y') | NO   |     | Y       |                |
| Status    | tinyint       | NO   | MUL | 0       |                |
| NumNotif  | int           | NO   |     | 0       |                |
+-----------+---------------+------+-----+---------+----------------+
8 rows in set (0,01 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS not_notices ("
			"NotCod INT NOT NULL AUTO_INCREMENT,"
			"CrsCod INT NOT NULL DEFAULT -1,"
			"UsrCod INT NOT NULL,"
			"CreatTime DATETIME NOT NULL,"
			// TODO: "Subject TEXT NOT NULL,"	// Cns_MAX_BYTES_SUBJECT
			// TODO: "Content TEXT NOT NULL,"	// Cns_MAX_BYTES_TEXT
			"Content TEXT NOT NULL,"
			"Public ENUM('N','Y') NOT NULL DEFAULT 'Y',"
			"Status TINYINT NOT NULL DEFAULT 0,"
			"NumNotif INT NOT NULL DEFAULT 0,"
		   "UNIQUE INDEX(NotCod),"
		   "INDEX(CrsCod,Status),"
		   "INDEX(UsrCod),"
		   "INDEX(CreatTime),"
		   "INDEX(Status)"
		   ") ENGINE=MyISAM");

   /***** Table ntf_mail_domains *****/
/*
mysql> DESCRIBE ntf_mail_domains;
+--------+---------------+------+-----+---------+----------------+
| Field  | Type          | Null | Key | Default | Extra          |
+--------+---------------+------+-----+---------+----------------+
| MaiCod | int(11)       | NO   | PRI | NULL    | auto_increment |
| Domain | varchar(255)  | NO   | UNI | NULL    |                |
| Info   | varchar(2047) | NO   |     | NULL    |                |
+--------+---------------+------+-----+---------+----------------+
3 rows in set (0,00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS ntf_mail_domains ("
			"MaiCod INT NOT NULL AUTO_INCREMENT,"
			"Domain VARCHAR(255) NOT NULL,"	// Cns_MAX_BYTES_EMAIL_ADDRESS
			"Info VARCHAR(2047) NOT NULL,"	// Mai_MAX_BYTES_MAIL_INFO
		   "UNIQUE INDEX(MaiCod),"
		   "UNIQUE INDEX(Domain)"
		   ") ENGINE=MyISAM");

   /***** Table ntf_notifications *****/
/*
mysql> DESCRIBE ntf_notifications;
+-------------+------------+------+-----+---------+----------------+
| Field       | Type       | Null | Key | Default | Extra          |
+-------------+------------+------+-----+---------+----------------+
| NtfCod      | int(11)    | NO   | PRI | NULL    | auto_increment |
| NotifyEvent | tinyint(4) | NO   | MUL | NULL    |                |
| ToUsrCod    | int(11)    | NO   | MUL | NULL    |                |
| FromUsrCod  | int(11)    | NO   |     | NULL    |                |
| InsCod      | int(11)    | NO   |     | -1      |                |
| CtrCod      | int(11)    | NO   |     | -1      |                |
| DegCod      | int(11)    | NO   |     | -1      |                |
| CrsCod      | int(11)    | NO   | MUL | -1      |                |
| Cod         | int(11)    | NO   |     | -1      |                |
| TimeNotif   | datetime   | NO   | MUL | NULL    |                |
| Status      | tinyint(4) | NO   |     | 0       |                |
+-------------+------------+------+-----+---------+----------------+
11 rows in set (0.02 sec)
*/
// TODO: Change NtfCod and LogCod from INT to BIGINT in database tables.
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS ntf_notifications ("
			"NtfCod INT NOT NULL AUTO_INCREMENT,"
			"NotifyEvent TINYINT NOT NULL,"
			"ToUsrCod INT NOT NULL,"
			"FromUsrCod INT NOT NULL,"
			"InsCod INT NOT NULL DEFAULT -1,"
			"CtrCod INT NOT NULL DEFAULT -1,"
			"DegCod INT NOT NULL DEFAULT -1,"
			"CrsCod INT NOT NULL DEFAULT -1,"
			"Cod INT NOT NULL DEFAULT -1,"
			"TimeNotif DATETIME NOT NULL,"
			"Status TINYINT NOT NULL DEFAULT 0,"
		   "UNIQUE INDEX(NtfCod),"
		   "INDEX(NotifyEvent),"
		   "INDEX(ToUsrCod),"
		   "INDEX(CrsCod),"
		   "INDEX(TimeNotif)"
		   ") ENGINE=MyISAM");

   /***** Table plc_places *****/
/*
mysql> DESCRIBE plc_places;
+-----------+---------------+------+-----+---------+----------------+
| Field     | Type          | Null | Key | Default | Extra          |
+-----------+---------------+------+-----+---------+----------------+
| PlcCod    | int(11)       | NO   | PRI | NULL    | auto_increment |
| InsCod    | int(11)       | NO   | MUL | NULL    |                |
| ShortName | varchar(511)  | NO   |     | NULL    |                |
| FullName  | varchar(2047) | NO   |     | NULL    |                |
+-----------+---------------+------+-----+---------+----------------+
4 rows in set (0,00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS plc_places ("
			"PlcCod INT NOT NULL AUTO_INCREMENT,"
			"InsCod INT NOT NULL,"
			"ShortName VARCHAR(511) NOT NULL,"	// Nam_MAX_BYTES_SHRT_NAME
			"FullName VARCHAR(2047) NOT NULL,"	// Nam_MAX_BYTES_FULL_NAME
		   "UNIQUE INDEX(PlcCod),"
		   "INDEX(InsCod)"
		   ") ENGINE=MyISAM");

   /***** Table plg_plugins *****/
/*
mysql> DESCRIBE plg_plugins;
+-------------+---------------+------+-----+---------+----------------+
| Field       | Type          | Null | Key | Default | Extra          |
+-------------+---------------+------+-----+---------+----------------+
| PlgCod      | int(11)       | NO   | PRI | NULL    | auto_increment |
| Name        | varchar(511)  | NO   |     | NULL    |                |
| Description | varchar(2047) | NO   |     | NULL    |                |
| Logo        | varchar(31)   | NO   |     | NULL    |                |
| AppKey      | varchar(31)   | NO   |     | NULL    |                |
| URL         | varchar(255)  | NO   |     | NULL    |                |
| IP          | char(15)      | NO   |     | NULL    |                |
+-------------+---------------+------+-----+---------+----------------+
7 rows in set (0,00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS plg_plugins ("
			"PlgCod INT NOT NULL AUTO_INCREMENT,"
			"Name VARCHAR(511) NOT NULL,"		// Plg_MAX_BYTES_PLUGIN_NAME
			"Description VARCHAR(2047) NOT NULL,"	// Plg_MAX_BYTES_PLUGIN_DESCRIPTION
			"Logo VARCHAR(31) NOT NULL,"		// Plg_MAX_BYTES_PLUGIN_LOGO
			"AppKey VARCHAR(31) NOT NULL,"		// Plg_MAX_BYTES_PLUGIN_APP_KEY
			"URL VARCHAR(255) NOT NULL,"		// WWW_MAX_BYTES_WWW
			"IP CHAR(15) NOT NULL,"			// Cns_MAX_BYTES_IP
		   "UNIQUE INDEX(PlgCod)"
		   ") ENGINE=MyISAM");

   /***** Table prg_resources *****/
/*
mysql> DESCRIBE prg_resources;
+--------+-------------------------------------------------------------------------------------------------------------------------------------------------------+------+-----+---------+----------------+
| Field  | Type                                                                                                                                                  | Null | Key | Default | Extra          |
+--------+-------------------------------------------------------------------------------------------------------------------------------------------------------+------+-----+---------+----------------+
| ItmCod | int                                                                                                                                                   | NO   | PRI | NULL    | auto_increment |
| NodCod | int                                                                                                                                                   | NO   | MUL | -1      |                |
| ItmInd | int                                                                                                                                                   | NO   |     | 0       |                |
| Hidden | enum('N','Y')                                                                                                                                         | NO   |     | N       |                |
| Type   | enum('non','inf','gui','lec','pra','bib','faq','lnk','ass','tmt','asg','prj','cfe','tst','exa','gam','rub','doc','mrk','grp','tch','att','for','svy') | NO   |     | non     |                |
| Cod    | int                                                                                                                                                   | NO   |     | -1      |                |
| Title  | varchar(2047)                                                                                                                                         | NO   |     | NULL    |                |
+--------+-------------------------------------------------------------------------------------------------------------------------------------------------------+------+-----+---------+----------------+
7 rows in set (0,00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS prg_resources ("
			"ItmCod INT NOT NULL AUTO_INCREMENT,"
			"NodCod INT NOT NULL DEFAULT -1,"
			"ItmInd INT NOT NULL DEFAULT 0,"
			"Hidden ENUM('N','Y') NOT NULL DEFAULT 'N',"
			"Type ENUM('non','inf','gui','lec','pra','bib','faq',"
				  "'lnk','ass','tmt','asg','prj','cfe','tst',"
				  "'exa','gam','rub','doc','mrk','grp','tch',"
				  "'att','for','svy') NOT NULL DEFAULT 'non',"
			"Title VARCHAR(2047) NOT NULL,"		// Rsc_MAX_BYTES_RESOURCE_TITLE
		   "UNIQUE INDEX(ItmCod),"
		   "UNIQUE INDEX(NodCod,ItmInd)"
		   ") ENGINE=MyISAM");

   /***** Table prj_config *****/
/*
mysql> DESCRIBE prj_config;
+--------------+---------------+------+-----+---------+-------+
| Field        | Type          | Null | Key | Default | Extra |
+--------------+---------------+------+-----+---------+-------+
| CrsCod       | int           | NO   | PRI | -1      |       |
| NETCanCreate | enum('N','Y') | NO   |     | Y       |       |
+--------------+---------------+------+-----+---------+-------+
2 rows in set (0,00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS prj_config ("
			"CrsCod INT NOT NULL DEFAULT -1,"
			"NETCanCreate ENUM('N','Y') NOT NULL DEFAULT 'Y',"
		   "UNIQUE INDEX(CrsCod)"
		   ") ENGINE=MyISAM");

   /***** Table prj_projects *****/
/*
mysql> DESCRIBE prj_projects;
+--------------+--------------------------------------------+------+-----+---------------------+----------------+
| Field        | Type                                       | Null | Key | Default             | Extra          |
+--------------+--------------------------------------------+------+-----+---------------------+----------------+
| PrjCod       | int(11)                                    | NO   | PRI | NULL                | auto_increment |
| CrsCod       | int(11)                                    | NO   | MUL | -1                  |                |
| DptCod       | int(11)                                    | NO   |     | -1                  |                |
| Locked       | enum('N','Y')                              | NO   |     | N                   |                |
| Hidden       | enum('N','Y')                              | NO   |     | N                   |                |
| Assigned     | enum('N','Y')                              | NO   |     | N                   |                |
| NumStds      | int(11)                                    | NO   |     | 1                   |                |
| Proposal     | enum('new','modified','unmodified')        | NO   |     | new                 |                |
| CreatTime    | datetime                                   | NO   |     | NULL                |                |
| ModifTime    | datetime                                   | NO   |     | NULL                |                |
| Title        | varchar(4095)                              | NO   |     | NULL                |                |
| Description  | text                                       | NO   |     | NULL                |                |
| Knowledge    | text                                       | NO   |     | NULL                |                |
| Materials    | text                                       | NO   |     | NULL                |                |
| URL          | varchar(255)                               | NO   |     | NULL                |                |
| ReviewStatus | enum('unreviewed','unapproved','approved') | NO   |     | unreviewed          |                |
| ReviewTime   | datetime                                   | NO   |     | 1970-01-01 01:00:00 |                |
| ReviewTxt    | text                                       | NO   |     | NULL                |                |
+--------------+--------------------------------------------+------+-----+---------------------+----------------+
18 rows in set (0,00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS prj_projects ("
			"PrjCod INT NOT NULL AUTO_INCREMENT,"
			"CrsCod INT NOT NULL DEFAULT -1,"
			"DptCod INT NOT NULL DEFAULT -1,"
			"Locked ENUM('N','Y') NOT NULL DEFAULT 'N',"
			"Hidden ENUM('N','Y') NOT NULL DEFAULT 'N',"
			"Assigned ENUM('N','Y') NOT NULL DEFAULT 'N',"
			"NumStds INT NOT NULL DEFAULT 1,"
			"Proposal ENUM('new','modified','unmodified') NOT NULL DEFAULT 'new',"
			"CreatTime DATETIME NOT NULL,"
			"ModifTime DATETIME NOT NULL,"
			"Title VARCHAR(4095) NOT NULL,"	// Prj_MAX_BYTES_PROJECT_TITLE
			"Description TEXT NOT NULL,"	// Cns_MAX_BYTES_TEXT
			"Knowledge TEXT NOT NULL,"	// Cns_MAX_BYTES_TEXT
			"Materials TEXT NOT NULL,"	// Cns_MAX_BYTES_TEXT
			"URL VARCHAR(255) NOT NULL,"	// WWW_MAX_BYTES_WWW
			"ReviewStatus ENUM('unreviewed','unapproved','approved') NOT NULL DEFAULT 'unreviewed',"
			"ReviewTime DATETIME NOT NULL DEFAULT '1970-01-01 01:00:00',"
			"ReviewTxt TEXT NOT NULL,"	// Cns_MAX_BYTES_TEXT
		   "UNIQUE INDEX(PrjCod),"
		   "INDEX(CrsCod,Hidden),"
		   "INDEX(CrsCod,CreatTime),"
		   "INDEX(CrsCod,ModifTime),"
		   "INDEX(CrsCod,DptCod),"
		   "INDEX(CrsCod,ReviewStatus)"
		   ") ENGINE=MyISAM");

   /***** Table prj_rubrics *****/
/*
mysql> DESCRIBE prj_rubrics;
+--------+-------------------------+------+-----+---------+-------+
| Field  | Type                    | Null | Key | Default | Extra |
+--------+-------------------------+------+-----+---------+-------+
| CrsCod | int                     | NO   | PRI | NULL    |       |
| Type   | enum('tut','evl','gbl') | NO   | PRI | NULL    |       |
| RubCod | int                     | NO   | PRI | NULL    |       |
+--------+-------------------------+------+-----+---------+-------+
3 rows in set (0,00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS prj_rubrics ("
			"CrsCod INT NOT NULL,"
			"Type ENUM('tut','evl','gbl') NOT NULL,"
			"RubCod INT NOT NULL,"
		   "UNIQUE INDEX(CrsCod,Type,RubCod)"
		   ") ENGINE=MyISAM");

   /***** Table prj_users *****/
/*
mysql> DESCRIBE prj_users;
+---------------+------------+------+-----+---------+-------+
| Field         | Type       | Null | Key | Default | Extra |
+---------------+------------+------+-----+---------+-------+
| PrjCod        | int(11)    | NO   | PRI | NULL    |       |
| RoleInProject | tinyint(4) | NO   | PRI | 0       |       |
| UsrCod        | int(11)    | NO   | PRI | NULL    |       |
+---------------+------------+------+-----+---------+-------+
3 rows in set (0,00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS prj_users ("
			"PrjCod INT NOT NULL,"
			"RoleInProject TINYINT NOT NULL DEFAULT 0,"
			"UsrCod INT NOT NULL,"
			"UNIQUE INDEX(PrjCod,RoleInProject,UsrCod)"
		   ") ENGINE=MyISAM");

   /***** Table roo_check_in *****/
/*
mysql> DESCRIBE roo_check_in;
+-------------+----------+------+-----+---------+----------------+
| Field       | Type     | Null | Key | Default | Extra          |
+-------------+----------+------+-----+---------+----------------+
| ChkCod      | int(11)  | NO   | PRI | NULL    | auto_increment |
| UsrCod      | int(11)  | NO   | MUL | NULL    |                |
| RooCod      | int(11)  | NO   |     | NULL    |                |
| CheckInTime | datetime | NO   | MUL | NULL    |                |
+-------------+----------+------+-----+---------+----------------+
4 rows in set (0.00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS roo_check_in ("
			"ChkCod INT NOT NULL AUTO_INCREMENT,"
			"UsrCod INT NOT NULL,"
			"RooCod INT NOT NULL,"
			"CheckInTime DATETIME NOT NULL,"
		   "UNIQUE INDEX(ChkCod),"
		   "INDEX(UsrCod,CheckInTime),"
		   "INDEX(CheckInTime)"
		   ") ENGINE=MyISAM");

   /***** Table roo_macs *****/
/*
mysql> DESCRIBE roo_macs;
+--------+--------+------+-----+---------+----------------+
| Field  | Type   | Null | Key | Default | Extra          |
+--------+--------+------+-----+---------+----------------+
| RooCod | int    | NO   | PRI | NULL    | auto_increment |
| MAC    | bigint | NO   | PRI | NULL    |                |
+--------+--------+------+-----+---------+----------------+
2 rows in set (0.01 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS roo_macs ("
			"RooCod INT NOT NULL,"
			"MAC BIGINT NOT NULL,"	// 12 digits hexadecimal number
		   "UNIQUE INDEX(RooCod,MAC),"
		   "UNIQUE INDEX(MAC,RooCod)"
		   ") ENGINE=MyISAM");

   /***** Table roo_rooms *****/
/*
mysql> DESCRIBE roo_rooms;
+-----------+-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+------+-----+---------+----------------+
| Field     | Type                                                                                                                                                                                                                                                                        | Null | Key | Default | Extra          |
+-----------+-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+------+-----+---------+----------------+
| RooCod    | int                                                                                                                                                                                                                                                                         | NO   | PRI | NULL    | auto_increment |
| CtrCod    | int                                                                                                                                                                                                                                                                         | NO   | MUL | NULL    |                |
| BldCod    | int                                                                                                                                                                                                                                                                         | NO   |     | -1      |                |
| Floor     | int                                                                                                                                                                                                                                                                         | NO   |     | 0       |                |
| Type      | enum('no_type','administration','auditorium','cafeteria','canteen','classroom','concierge','corridor','gym','hall','kindergarten','laboratory','library','office','outdoors','parking','pavilion','room','secretariat','seminar','shop','store','toilets','virtual','yard') | NO   |     | no_type |                |
| ShortName | varchar(511)                                                                                                                                                                                                                                                                | NO   |     | NULL    |                |
| FullName  | varchar(2047)                                                                                                                                                                                                                                                               | NO   |     | NULL    |                |
| Capacity  | int                                                                                                                                                                                                                                                                         | NO   |     | NULL    |                |
+-----------+-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+------+-----+---------+----------------+
8 rows in set (0.00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS roo_rooms ("
			"RooCod INT NOT NULL AUTO_INCREMENT,"
			"CtrCod INT NOT NULL,"
	                "BldCod INT NOT NULL DEFAULT -1,"
	                "Floor INT NOT NULL DEFAULT 0,"
			"Type ENUM("
			"'no_type',"
			"'administration',"
			"'auditorium',"
			"'cafeteria',"
			"'canteen',"
			"'classroom',"
			"'concierge',"
			"'corridor',"
			"'gym',"
			"'hall',"
			"'kindergarten',"
			"'laboratory',"
			"'library',"
			"'office',"
			"'outdoors',"
			"'parking',"
			"'pavilion',"
			"'room',"
			"'secretariat',"
			"'seminar',"
			"'shop',"
			"'store',"
			"'toilets',"
			"'virtual',"
			"'yard') NOT NULL DEFAULT 'no_type',"
			"ShortName VARCHAR(511) NOT NULL,"	// Nam_MAX_BYTES_SHRT_NAME
			"FullName VARCHAR(2047) NOT NULL,"	// Nam_MAX_BYTES_FULL_NAME
			"Capacity INT NOT NULL,"
		   "UNIQUE INDEX(RooCod),"
		   "INDEX(CtrCod,BldCod,Floor)"
		   ") ENGINE=MyISAM");

   /***** Table rsc_clipboards *****/
/*
mysql> DESCRIBE rsc_clipboards;
+----------+-------------------------------------------------------------------------------------------------------------------------------------------------------+------+-----+---------+-------+
| Field    | Type                                                                                                                                                  | Null | Key | Default | Extra |
+----------+-------------------------------------------------------------------------------------------------------------------------------------------------------+------+-----+---------+-------+
| UsrCod   | int                                                                                                                                                   | NO   | PRI | NULL    |       |
| CrsCod   | int                                                                                                                                                   | NO   | PRI | NULL    |       |
| Type     | enum('non','inf','gui','lec','pra','bib','faq','lnk','ass','tmt','asg','prj','cfe','tst','exa','gam','rub','doc','mrk','grp','tch','att','for','svy') | NO   | PRI | non     |       |
| Cod      | int                                                                                                                                                   | NO   | PRI | -1      |       |
| CopyTime | timestamp                                                                                                                                             | YES  | MUL | NULL    |       |
+----------+-------------------------------------------------------------------------------------------------------------------------------------------------------+------+-----+---------+-------+
5 rows in set (0,00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS rsc_clipboards ("
			"UsrCod INT NOT NULL,"
			"CrsCod INT NOT NULL,"
			"Type ENUM('non','inf','gui','lec','pra','bib','faq',"
				  "'lnk','ass','tmt','asg','prj','cfe','tst',"
				  "'exa','gam','rub','doc','mrk','grp','tch',"
				  "'att','for','svy') NOT NULL DEFAULT 'non',"
			"Cod INT NOT NULL DEFAULT -1,"
			"CopyTime TIMESTAMP,"
		   "UNIQUE INDEX(UsrCod,CrsCod,Type,Cod),"
		   "INDEX(CrsCod,Type,Cod),"
		   "INDEX(CopyTime)"
		   ") ENGINE=MyISAM");

   /***** Table rub_criteria *****/
/*
mysql> DESCRIBE rub_criteria;
+--------+-------------------------------------------------------------------------------------------------------------------------------------------------------+------+-----+---------+----------------+
| Field  | Type                                                                                                                                                  | Null | Key | Default | Extra          |
+--------+-------------------------------------------------------------------------------------------------------------------------------------------------------+------+-----+---------+----------------+
| CriCod | int                                                                                                                                                   | NO   | PRI | NULL    | auto_increment |
| RubCod | int                                                                                                                                                   | NO   | MUL | NULL    |                |
| CriInd | int                                                                                                                                                   | NO   |     | NULL    |                |
| Type   | enum('non','inf','gui','lec','pra','bib','faq','lnk','ass','tmt','asg','prj','cfe','tst','exa','gam','rub','doc','mrk','grp','tch','att','for','svy') | NO   |     | non     |                |
| Cod    | int                                                                                                                                                   | NO   |     | -1      |                |
| MinVal | double                                                                                                                                                | NO   |     | 0       |                |
| MaxVal | double                                                                                                                                                | NO   |     | 1       |                |
| Weight | double                                                                                                                                                | NO   |     | 1       |                |
| Title  | varchar(2047)                                                                                                                                         | NO   |     | NULL    |                |
+--------+-------------------------------------------------------------------------------------------------------------------------------------------------------+------+-----+---------+----------------+
9 rows in set (0,00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS rub_criteria ("
			"CriCod INT NOT NULL AUTO_INCREMENT,"
			"RubCod INT NOT NULL,"
			"CriInd INT NOT NULL,"
			"Type ENUM('non','inf','gui','lec','pra','bib','faq',"
				  "'lnk','ass','tmt','asg','prj','cfe','tst',"
				  "'exa','gam','rub','doc','mrk','grp','tch',"
				  "'att','for','svy') NOT NULL DEFAULT 'non',"
			"Cod INT NOT NULL DEFAULT -1,"
			"MinVal DOUBLE PRECISION NOT NULL DEFAULT 0,"
			"MaxVal DOUBLE PRECISION NOT NULL DEFAULT 1,"
			"Weight DOUBLE PRECISION NOT NULL DEFAULT 1,"
			"Title VARCHAR(2047) NOT NULL,"
		   "UNIQUE INDEX(CriCod),"
		   "UNIQUE INDEX(RubCod,CriInd)"
		   ") ENGINE=MyISAM");

   /***** Table rub_rubrics *****/
/*
mysql> DESCRIBE rub_rubrics;
+--------+---------------+------+-----+---------+----------------+
| Field  | Type          | Null | Key | Default | Extra          |
+--------+---------------+------+-----+---------+----------------+
| RubCod | int           | NO   | PRI | NULL    | auto_increment |
| CrsCod | int           | NO   | MUL | -1      |                |
| UsrCod | int           | NO   |     | NULL    |                |
| Title  | varchar(2047) | NO   |     | NULL    |                |
| Txt    | text          | NO   |     | NULL    |                |
+--------+---------------+------+-----+---------+----------------+
5 rows in set (0,00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS rub_rubrics ("
			"RubCod INT NOT NULL AUTO_INCREMENT,"
			"CrsCod INT NOT NULL DEFAULT -1,"
			"UsrCod INT NOT NULL,"
			"Title VARCHAR(2047) NOT NULL,"	// Rub_MAX_BYTES_TITLE
			"Txt TEXT NOT NULL,"		// Cns_MAX_BYTES_TEXT
		   "UNIQUE INDEX(RubCod),"
		   "INDEX(CrsCod)"
		   ") ENGINE=MyISAM");

   /***** Table rub_scores *****/
/*
mysql> DESCRIBE rub_scores;
+--------+-------------------------------------------------------------------------------------------------------------------------------------------------------+------+-----+---------+-------+
| Field  | Type                                                                                                                                                  | Null | Key | Default | Extra |
+--------+-------------------------------------------------------------------------------------------------------------------------------------------------------+------+-----+---------+-------+
| Type   | enum('non','inf','gui','lec','pra','bib','faq','lnk','ass','tmt','asg','prj','cfe','tst','exa','gam','rub','doc','mrk','grp','tch','att','for','svy') | NO   | PRI | non     |       |
| Cod    | int                                                                                                                                                   | NO   | PRI | NULL    |       |
| UsrCod | int                                                                                                                                                   | NO   | PRI | -1      |       |
| CriCod | int                                                                                                                                                   | NO   | PRI | NULL    |       |
| EvlCod | int                                                                                                                                                   | NO   |     | -1      |       |
| Score  | double                                                                                                                                                | NO   |     | 0       |       |
+--------+-------------------------------------------------------------------------------------------------------------------------------------------------------+------+-----+---------+-------+
6 rows in set (0,00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS rub_scores ("
			"Type ENUM('non','inf','gui','lec','pra','bib','faq',"
				  "'lnk','ass','tmt','asg','prj','cfe','tst',"
				  "'exa','gam','rub','doc','mrk','grp','tch',"
				  "'att','for','svy') NOT NULL DEFAULT 'non',"
			"Cod INT NOT NULL,"
			"UsrCod INT NOT NULL DEFAULT -1,"
			"CriCod INT NOT NULL,"
			"EvlCod INT NOT NULL DEFAULT -1,"
			"Score DOUBLE PRECISION NOT NULL DEFAULT 0,"
		   "UNIQUE INDEX(Type,Cod,UsrCod,CriCod)"
		   ") ENGINE=MyISAM");

   /***** Table ses_params *****/
/*
mysql> DESCRIBE ses_params;
+------------+--------------+------+-----+---------+-------+
| Field      | Type         | Null | Key | Default | Extra |
+------------+--------------+------+-----+---------+-------+
| SessionId  | char(43)     | NO   | MUL | NULL    |       |
| ParamName  | varchar(255) | NO   |     | NULL    |       |
| ParamValue | longtext     | NO   |     | NULL    |       |
+------------+--------------+------+-----+---------+-------+
3 rows in set (0.01 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS ses_params ("
			"SessionId CHAR(43) NOT NULL,"	// Cns_BYTES_SESSION_ID
			"ParamName VARCHAR(255) NOT NULL,"
			"ParamValue LONGTEXT NOT NULL,"
		   "INDEX(SessionId)"
		   ") ENGINE=MyISAM");

   /***** Table ses_sessions *****/
/*
mysql> DESCRIBE ses_sessions;
+----------------+---------------+------+-----+---------+-------+
| Field          | Type          | Null | Key | Default | Extra |
+----------------+---------------+------+-----+---------+-------+
| SessionId      | char(43)      | NO   | PRI | NULL    |       |
| UsrCod         | int(11)       | NO   | MUL | NULL    |       |
| Password       | char(86)      | NO   |     | NULL    |       |
| Role           | tinyint(4)    | NO   |     | 0       |       |
| CtyCod         | int(11)       | NO   |     | -1      |       |
| InsCod         | int(11)       | NO   |     | -1      |       |
| CtrCod         | int(11)       | NO   |     | -1      |       |
| DegCod         | int(11)       | NO   |     | -1      |       |
| CrsCod         | int(11)       | NO   |     | -1      |       |
| LastTime       | datetime      | NO   |     | NULL    |       |
| LastRefresh    | datetime      | NO   |     | NULL    |       |
| FirstPubCod    | bigint(20)    | NO   |     | 0       |       |
| LastPubCod     | bigint(20)    | NO   |     | 0       |       |
| LastPageMsgRcv | int(11)       | NO   |     | 1       |       |
| LastPageMsgSnt | int(11)       | NO   |     | 1       |       |
| WhatToSearch   | tinyint(4)    | NO   |     | 0       |       |
| SearchStr      | varchar(2047) | NO   |     |         |       |
| SideCols       | tinyint(4)    | NO   |     | 3       |       |
+----------------+---------------+------+-----+---------+-------+
18 rows in set (0,00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS ses_sessions ("
			"SessionId CHAR(43) NOT NULL,"				// Cns_BYTES_SESSION_ID
			"UsrCod INT NOT NULL,"
			"Password CHAR(86) COLLATE latin1_bin NOT NULL,"	// Pwd_BYTES_ENCRYPTED_PASSWORD
			"Role TINYINT NOT NULL DEFAULT 0,"
			"CtyCod INT NOT NULL DEFAULT -1,"
			"InsCod INT NOT NULL DEFAULT -1,"
			"CtrCod INT NOT NULL DEFAULT -1,"
			"DegCod INT NOT NULL DEFAULT -1,"
			"CrsCod INT NOT NULL DEFAULT -1,"
			"LastTime DATETIME NOT NULL,"
			"LastRefresh DATETIME NOT NULL,"
			"FirstPubCod BIGINT NOT NULL DEFAULT 0,"
			"LastPubCod BIGINT NOT NULL DEFAULT 0,"
			"LastPageMsgRcv INT NOT NULL DEFAULT 1,"
			"LastPageMsgSnt INT NOT NULL DEFAULT 1,"
			"WhatToSearch TINYINT NOT NULL DEFAULT 0,"
			"SearchStr VARCHAR(2047) NOT NULL DEFAULT '',"		// Sch_MAX_BYTES_STRING_TO_FIND
			"SideCols TINYINT NOT NULL DEFAULT 3,"
		   "UNIQUE INDEX(SessionId),"
		   "INDEX(UsrCod)"
		   ") ENGINE=MyISAM");

   /***** Table set_ip_settings *****/
/*
mysql> DESCRIBE set_ip_settings;
+----------------+----------+------+-----+---------+-------+
| Field          | Type     | Null | Key | Default | Extra |
+----------------+----------+------+-----+---------+-------+
| IP             | char(15) | NO   | PRI | NULL    |       |
| UsrCod         | int      | NO   | MUL | -1      |       |
| LastChange     | datetime | NO   | MUL | NULL    |       |
| FirstDayOfWeek | tinyint  | NO   |     | 0       |       |
| DateFormat     | tinyint  | NO   |     | 0       |       |
| Theme          | char(16) | NO   |     | NULL    |       |
| IconSet        | char(16) | NO   |     | NULL    |       |
| Menu           | tinyint  | NO   |     | 0       |       |
| SideCols       | tinyint  | NO   |     | NULL    |       |
| PhotoShape     | tinyint  | NO   |     | 0       |       |
+----------------+----------+------+-----+---------+-------+
10 rows in set (0,01 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS set_ip_settings ("
			"IP CHAR(15) NOT NULL,"		// Cns_MAX_BYTES_IP
			"UsrCod INT NOT NULL DEFAULT -1,"
			"LastChange DATETIME NOT NULL,"
			"FirstDayOfWeek TINYINT NOT NULL DEFAULT 0,"
			"DateFormat TINYINT NOT NULL DEFAULT 0,"
			"Theme CHAR(16) NOT NULL,"	// The_MAX_BYTES_THEME_ID
			"IconSet CHAR(16) NOT NULL,"	// Ico_MAX_BYTES_ICON_SET_ID
			"Menu TINYINT NOT NULL DEFAULT 0,"
			"SideCols TINYINT NOT NULL,"
			"PhotoShape TINYINT NOT NULL DEFAULT 0,"
		   "PRIMARY KEY (IP),"
		   "INDEX(UsrCod),"
		   "INDEX(LastChange)"
		   ") ENGINE=MyISAM");

   /***** Table sta_degrees *****/
/*
mysql> DESCRIBE sta_degrees;
+-----------------------+---------------------------------------+------+-----+---------+-------+
| Field                 | Type                                  | Null | Key | Default | Extra |
+-----------------------+---------------------------------------+------+-----+---------+-------+
| DegCod                | int(11)                               | NO   | PRI | -1      |       |
| Sex                   | enum('unknown','female','male','all') | NO   | PRI | all     |       |
| NumStds               | int(11)                               | NO   |     | NULL    |       |
| NumStdsWithPhoto      | int(11)                               | NO   |     | NULL    |       |
| TimeAvgPhoto          | datetime                              | NO   | MUL | NULL    |       |
| TimeToComputeAvgPhoto | int(11)                               | NO   |     | -1      |       |
+-----------------------+---------------------------------------+------+-----+---------+-------+
6 rows in set (0.00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS sta_degrees ("
			"DegCod INT NOT NULL DEFAULT -1,"
			"Sex ENUM ('unknown','female','male','all') NOT NULL DEFAULT 'all',"
			"NumStds INT NOT NULL,"
			"NumStdsWithPhoto INT NOT NULL,"
			"TimeAvgPhoto DATETIME NOT NULL,"
			"TimeToComputeAvgPhoto INT NOT NULL DEFAULT -1,"
		   "UNIQUE INDEX(DegCod,Sex)"
		   ") ENGINE=MyISAM");

   /***** Table sta_notifications *****/
/*
mysql> DESCRIBE sta_notifications;
+-------------+------------+------+-----+---------+-------+
| Field       | Type       | Null | Key | Default | Extra |
+-------------+------------+------+-----+---------+-------+
| DegCod      | int(11)    | NO   | PRI | NULL    |       |
| CrsCod      | int(11)    | NO   | PRI | NULL    |       |
| NotifyEvent | tinyint(4) | NO   | PRI | NULL    |       |
| NumEvents   | int(11)    | NO   |     | NULL    |       |
| NumMails    | int(11)    | NO   |     | NULL    |       |
+-------------+------------+------+-----+---------+-------+
5 rows in set (0.00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS sta_notifications ("
			"DegCod INT NOT NULL,"
			"CrsCod INT NOT NULL,"
			"NotifyEvent TINYINT NOT NULL,"
			"NumEvents INT NOT NULL,"
			"NumMails INT NOT NULL,"
		   "UNIQUE INDEX(DegCod,CrsCod,NotifyEvent)"
		   ") ENGINE=MyISAM");

   /***** Table svy_answers *****/
/*
mysql> DESCRIBE svy_answers;
+---------+------------+------+-----+---------+-------+
| Field   | Type       | Null | Key | Default | Extra |
+---------+------------+------+-----+---------+-------+
| QstCod  | int(11)    | NO   | PRI | NULL    |       |
| AnsInd  | tinyint(4) | NO   | PRI | NULL    |       |
| NumUsrs | int(11)    | NO   |     | 0       |       |
| Answer  | text       | NO   |     | NULL    |       |
+---------+------------+------+-----+---------+-------+
4 rows in set (0.00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS svy_answers ("
			"QstCod INT NOT NULL,"
			"AnsInd TINYINT NOT NULL,"
			"NumUsrs INT NOT NULL DEFAULT 0,"
			"Answer TEXT NOT NULL,"	// Cns_MAX_BYTES_TEXT
		   "UNIQUE INDEX(QstCod,AnsInd)"
		   ") ENGINE=MyISAM");

   /***** Table svy_comments *****/
/*
mysql> DESCRIBE svy_comments;
+----------+------+------+-----+---------+----------------+
| Field    | Type | Null | Key | Default | Extra          |
+----------+------+------+-----+---------+----------------+
| ComCod   | int  | NO   | PRI | NULL    | auto_increment |
| QstCod   | int  | NO   | MUL | NULL    |                |
| Comments | text | NO   |     | NULL    |                |
+----------+------+------+-----+---------+----------------+
3 rows in set (0,00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS svy_comments ("
			"ComCod INT NOT NULL AUTO_INCREMENT,"
			"QstCod INT NOT NULL,"
			"Comments TEXT NOT NULL,"	// Cns_MAX_BYTES_TEXT
		   "UNIQUE INDEX(ComCod),"
		   "INDEX(QstCod,ComCod)"
		   ") ENGINE=MyISAM");

   /***** Table svy_groups *****/
/*
mysql> DESCRIBE svy_groups;
+--------+---------+------+-----+---------+-------+
| Field  | Type    | Null | Key | Default | Extra |
+--------+---------+------+-----+---------+-------+
| SvyCod | int(11) | NO   | PRI | NULL    |       |
| GrpCod | int(11) | NO   | PRI | NULL    |       |
+--------+---------+------+-----+---------+-------+
2 rows in set (0.01 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS svy_groups ("
			"SvyCod INT NOT NULL,"
			"GrpCod INT NOT NULL,"
		   "UNIQUE INDEX(SvyCod,GrpCod)"
		   ") ENGINE=MyISAM");

   /***** Table svy_questions *****/
/*
mysql> DESCRIBE svy_questions;
+-----------------+-----------------------------------------+------+-----+---------+----------------+
| Field           | Type                                    | Null | Key | Default | Extra          |
+-----------------+-----------------------------------------+------+-----+---------+----------------+
| QstCod          | int                                     | NO   | PRI | NULL    | auto_increment |
| SvyCod          | int                                     | NO   | MUL | NULL    |                |
| QstInd          | int                                     | NO   |     | 0       |                |
| AnsType         | enum('unique_choice','multiple_choice') | NO   |     | NULL    |                |
| CommentsAllowed | enum('N','Y')                           | NO   |     | N       |                |
| Stem            | text                                    | NO   |     | NULL    |                |
+-----------------+-----------------------------------------+------+-----+---------+----------------+
6 rows in set (0,00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS svy_questions ("
			"QstCod INT NOT NULL AUTO_INCREMENT,"
			"SvyCod INT NOT NULL,"
			"QstInd INT NOT NULL DEFAULT 0,"
			"AnsType ENUM ('unique_choice','multiple_choice') NOT NULL,"
			"CommentsAllowed ENUM('N','Y') NOT NULL DEFAULT 'N',"
			"Stem TEXT NOT NULL,"	// Cns_MAX_BYTES_TEXT
		   "UNIQUE INDEX(QstCod),"
		   "INDEX(SvyCod)"
		   ") ENGINE=MyISAM");

   /***** Table svy_surveys *****/
/*
mysql> DESCRIBE svy_surveys;
+-----------+-------------------------------------------+------+-----+---------+----------------+
| Field     | Type                                      | Null | Key | Default | Extra          |
+-----------+-------------------------------------------+------+-----+---------+----------------+
| SvyCod    | int(11)                                   | NO   | PRI | NULL    | auto_increment |
| Scope     | enum('Sys','Cty','Ins','Ctr','Deg','Crs') | NO   | MUL | Sys     |                |
| Cod       | int(11)                                   | NO   |     | -1      |                |
| DegCod    | int(11)                                   | NO   |     | -1      |                |
| CrsCod    | int(11)                                   | NO   |     | -1      |                |
| Hidden    | enum('N','Y')                             | NO   |     | N       |                |
| NumNotif  | int(11)                                   | NO   |     | 0       |                |
| Roles     | int(11)                                   | NO   |     | 0       |                |
| UsrCod    | int(11)                                   | NO   |     | NULL    |                |
| StartTime | datetime                                  | NO   |     | NULL    |                |
| EndTime   | datetime                                  | NO   |     | NULL    |                |
| Title     | varchar(2047)                             | NO   |     | NULL    |                |
| Txt       | text                                      | NO   |     | NULL    |                |
+-----------+-------------------------------------------+------+-----+---------+----------------+
13 rows in set (0,00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS svy_surveys ("
			"SvyCod INT NOT NULL AUTO_INCREMENT,"
			"Scope ENUM('Sys','Cty','Ins','Ctr','Deg','Crs') NOT NULL DEFAULT 'Sys',"
			"Cod INT NOT NULL DEFAULT -1,"
			"Hidden ENUM('N','Y') NOT NULL DEFAULT 'N',"
			"NumNotif INT NOT NULL DEFAULT 0,"
			"Roles INT NOT NULL DEFAULT 0,"
			"UsrCod INT NOT NULL,"
			"StartTime DATETIME NOT NULL,"
			"EndTime DATETIME NOT NULL,"
			"Title VARCHAR(2047) NOT NULL,"	// Svy_MAX_BYTES_SURVEY_TITLE
			"Txt TEXT NOT NULL,"		// Cns_MAX_BYTES_TEXT
		   "UNIQUE INDEX(SvyCod),"
		   "INDEX(Scope,Cod)"
		   ") ENGINE=MyISAM");

   /***** Table svy_users *****/
/*
mysql> DESCRIBE svy_users;
+--------+---------+------+-----+---------+-------+
| Field  | Type    | Null | Key | Default | Extra |
+--------+---------+------+-----+---------+-------+
| SvyCod | int(11) | NO   | PRI | NULL    |       |
| UsrCod | int(11) | NO   | PRI | NULL    |       |
+--------+---------+------+-----+---------+-------+
2 rows in set (0.00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS svy_users ("
			"SvyCod INT NOT NULL,"
			"UsrCod INT NOT NULL,"
		   "UNIQUE INDEX(SvyCod,UsrCod)"
		   ") ENGINE=MyISAM");

   /***** Table tml_comments *****/
/*
mysql> DESCRIBE tml_comments;
+--------+------------+------+-----+---------+-------+
| Field  | Type       | Null | Key | Default | Extra |
+--------+------------+------+-----+---------+-------+
| PubCod | bigint(20) | NO   | PRI | NULL    |       |
| Txt    | longtext   | NO   | MUL | NULL    |       |
| MedCod | int(11)    | NO   | MUL | -1      |       |
+--------+------------+------+-----+---------+-------+
3 rows in set (0.00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS tml_comments ("
			"PubCod BIGINT NOT NULL,"
			"Txt LONGTEXT NOT NULL,"
			"MedCod INT NOT NULL DEFAULT -1,"
		   "UNIQUE INDEX(PubCod),"
		   "FULLTEXT(Txt),"
		   "INDEX(MedCod)"
		   ") ENGINE=MyISAM");

   /***** Table tml_comments_fav *****/
/*
mysql> DESCRIBE tml_comments_fav;
+---------+------------+------+-----+---------+----------------+
| Field   | Type       | Null | Key | Default | Extra          |
+---------+------------+------+-----+---------+----------------+
| FavCod  | bigint(20) | NO   | PRI | NULL    | auto_increment |
| PubCod  | bigint(20) | NO   | MUL | NULL    |                |
| UsrCod  | int(11)    | NO   | MUL | NULL    |                |
| TimeFav | datetime   | NO   |     | NULL    |                |
+---------+------------+------+-----+---------+----------------+
4 rows in set (0.00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS tml_comments_fav ("
			"FavCod BIGINT AUTO_INCREMENT,"
			"PubCod BIGINT NOT NULL,"
			"UsrCod INT NOT NULL,"
			"TimeFav DATETIME NOT NULL,"	// Not used. For future use
		   "UNIQUE INDEX(FavCod),"
		   "UNIQUE INDEX(PubCod,UsrCod),"
		   "INDEX(UsrCod)"
		   ") ENGINE=MyISAM");

   /***** Table tml_notes *****/
/*
mysql> DESCRIBE tml_notes;
+-------------+---------------+------+-----+---------+----------------+
| Field       | Type          | Null | Key | Default | Extra          |
+-------------+---------------+------+-----+---------+----------------+
| NotCod      | bigint(20)    | NO   | PRI | NULL    | auto_increment |
| NoteType    | tinyint(4)    | NO   | MUL | NULL    |                |
| Cod         | int(11)       | NO   |     | -1      |                |
| UsrCod      | int(11)       | NO   | MUL | NULL    |                |
| HieCod      | int(11)       | NO   |     | -1      |                |
| Unavailable | enum('N','Y') | NO   |     | N       |                |
| TimeNote    | datetime      | NO   | MUL | NULL    |                |
+-------------+---------------+------+-----+---------+----------------+
7 rows in set (0.00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS tml_notes ("
			"NotCod BIGINT NOT NULL AUTO_INCREMENT,"
			"NoteType TINYINT NOT NULL,"
			"Cod INT NOT NULL DEFAULT -1,"
			"UsrCod INT NOT NULL,"
			"HieCod INT NOT NULL DEFAULT -1,"
			"Unavailable ENUM('N','Y') NOT NULL DEFAULT 'N',"
			"TimeNote DATETIME NOT NULL,"
		   "UNIQUE INDEX(NotCod),"
		   "INDEX(NoteType,Cod),"
		   "INDEX(UsrCod),"
		   "INDEX(TimeNote)"
		   ") ENGINE=MyISAM");

   /***** Table tml_notes_fav *****/
/*
mysql> DESCRIBE tml_notes_fav;
+---------+------------+------+-----+---------+----------------+
| Field   | Type       | Null | Key | Default | Extra          |
+---------+------------+------+-----+---------+----------------+
| FavCod  | bigint(20) | NO   | PRI | NULL    | auto_increment |
| NotCod  | bigint(20) | NO   | MUL | NULL    |                |
| UsrCod  | int(11)    | NO   | MUL | NULL    |                |
| TimeFav | datetime   | NO   |     | NULL    |                |
+---------+------------+------+-----+---------+----------------+
4 rows in set (0.00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS tml_notes_fav ("
			"FavCod BIGINT AUTO_INCREMENT,"
			"NotCod BIGINT NOT NULL,"
			"UsrCod INT NOT NULL,"
			"TimeFav DATETIME NOT NULL,"	// Not used. For future use
		   "UNIQUE INDEX(FavCod),"
		   "UNIQUE INDEX(NotCod,UsrCod),"
		   "INDEX(UsrCod)"
		   ") ENGINE=MyISAM");

   /***** Table tml_posts *****/
/*
mysql> DESCRIBE tml_posts;
+--------+----------+------+-----+---------+----------------+
| Field  | Type     | Null | Key | Default | Extra          |
+--------+----------+------+-----+---------+----------------+
| PstCod | int(11)  | NO   | PRI | NULL    | auto_increment |
| Txt    | longtext | NO   | MUL | NULL    |                |
| MedCod | int(11)  | NO   | MUL | -1      |                |
+--------+----------+------+-----+---------+----------------+
3 rows in set (0.00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS tml_posts ("
			"PstCod INT NOT NULL AUTO_INCREMENT,"
			"Txt LONGTEXT NOT NULL,"
			"MedCod INT NOT NULL DEFAULT -1,"
		   "UNIQUE INDEX(PstCod),"
		   "FULLTEXT(Txt),"
		   "INDEX(MedCod)"
		   ") ENGINE=MyISAM");

   /***** Table tml_pubs *****/
/*
mysql> DESCRIBE tml_pubs;
+--------------+------------+------+-----+---------+----------------+
| Field        | Type       | Null | Key | Default | Extra          |
+--------------+------------+------+-----+---------+----------------+
| PubCod       | bigint(20) | NO   | PRI | NULL    | auto_increment |
| NotCod       | bigint(20) | NO   | MUL | NULL    |                |
| PublisherCod | int(11)    | NO   | MUL | NULL    |                |
| PubType      | tinyint(4) | NO   | MUL | NULL    |                |
| TimePublish  | datetime   | NO   | MUL | NULL    |                |
+--------------+------------+------+-----+---------+----------------+
5 rows in set (0.00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS tml_pubs ("
			"PubCod BIGINT NOT NULL AUTO_INCREMENT,"
			"NotCod BIGINT NOT NULL,"
			"PublisherCod INT NOT NULL,"
			"PubType TINYINT NOT NULL,"
			"TimePublish DATETIME NOT NULL,"
		   "UNIQUE INDEX(PubCod),"
		   "INDEX(NotCod,PublisherCod,PubType),"
		   "INDEX(PublisherCod),"
		   "INDEX(PubType),"
		   "INDEX(TimePublish)"
		   ") ENGINE=MyISAM");

   /***** Table tml_timelines *****/
/*
mysql> DESCRIBE tml_timelines;
+-----------+------------+------+-----+---------+-------+
| Field     | Type       | Null | Key | Default | Extra |
+-----------+------------+------+-----+---------+-------+
| SessionId | char(43)   | NO   | PRI | NULL    |       |
| NotCod    | bigint(20) | NO   | PRI | NULL    |       |
+-----------+------------+------+-----+---------+-------+
2 rows in set (0.00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS tml_timelines ("
			"SessionId CHAR(43) NOT NULL,"	// Cns_BYTES_SESSION_ID
			"NotCod BIGINT NOT NULL,"
		   "UNIQUE INDEX(SessionId,NotCod)"
		   ") ENGINE=MyISAM");

   /***** Table tmt_courses *****/
/*
mysql> DESCRIBE tmt_courses;
+-----------+------------------------------------+------+-----+---------+-------+
| Field     | Type                               | Null | Key | Default | Extra |
+-----------+------------------------------------+------+-----+---------+-------+
| CrsCod    | int(11)                            | NO   | MUL | -1      |       |
| GrpCod    | int(11)                            | NO   |     | -1      |       |
| Weekday   | tinyint(4)                         | NO   |     | NULL    |       |
| StartTime | time                               | NO   |     | NULL    |       |
| Duration  | time                               | NO   |     | NULL    |       |
| ClassType | enum('free','lecture','practical') | NO   |     | NULL    |       |
| Info      | varchar(2047)                      | NO   |     |         |       |
+-----------+------------------------------------+------+-----+---------+-------+
7 rows in set (0.00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS tmt_courses ("
			"CrsCod INT NOT NULL DEFAULT -1,"
			"GrpCod INT NOT NULL DEFAULT -1,"
			"Weekday TINYINT NOT NULL,"	// 0=Monday...6=Sunday
			"StartTime TIME NOT NULL,"
			"Duration TIME NOT NULL,"
			"ClassType ENUM('free','lecture','practical') NOT NULL,"
			"Info VARCHAR(2047) NOT NULL DEFAULT '',"	// Tmt_MAX_BYTES_INFO
		   "INDEX(CrsCod,GrpCod)"
		   ") ENGINE=MyISAM");

   /***** Table tmt_tutoring *****/
/*
mysql> DESCRIBE tmt_tutoring;
+-----------+---------------+------+-----+---------+-------+
| Field     | Type          | Null | Key | Default | Extra |
+-----------+---------------+------+-----+---------+-------+
| UsrCod    | int(11)       | NO   | MUL | NULL    |       |
| Weekday   | tinyint(4)    | NO   |     | NULL    |       |
| StartTime | time          | NO   |     | NULL    |       |
| Duration  | time          | NO   |     | NULL    |       |
| Info      | varchar(2047) | NO   |     |         |       |
+-----------+---------------+------+-----+---------+-------+
5 rows in set (0.00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS tmt_tutoring ("
			"UsrCod INT NOT NULL,"
			"Weekday TINYINT NOT NULL,"	// 0=Monday...6=Sunday
			"StartTime TIME NOT NULL,"
			"Duration TIME NOT NULL,"
			"Info VARCHAR(2047) NOT NULL DEFAULT '',"	// Tmt_MAX_BYTES_INFO
		   "INDEX(UsrCod)"
		   ") ENGINE=MyISAM");

   /***** Table tre_expanded *****/
/*
mysql> DESCRIBE tre_expanded;
+-----------+----------+------+-----+---------+-------+
| Field     | Type     | Null | Key | Default | Extra |
+-----------+----------+------+-----+---------+-------+
| UsrCod    | int      | NO   | PRI | NULL    |       |
| NodCod    | int      | NO   | PRI | NULL    |       |
| ClickTime | datetime | NO   | MUL | NULL    |       |
+-----------+----------+------+-----+---------+-------+
3 rows in set (0,00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS tre_expanded ("
			"UsrCod INT NOT NULL,"
			"NodCod INT NOT NULL,"
			"ClickTime DATETIME NOT NULL,"
		   "UNIQUE INDEX(UsrCod,NodCod),"
		   "INDEX(NodCod),"
		   "INDEX(ClickTime)"
		   ") ENGINE=MyISAM");

   /***** Table tre_nodes *****/
/*
mysql> DESCRIBE tre_nodes;
+-----------+-------------------------------------------------------------------+------+-----+---------------------+----------------+
| Field     | Type                                                              | Null | Key | Default             | Extra          |
+-----------+-------------------------------------------------------------------+------+-----+---------------------+----------------+
| NodCod    | int                                                               | NO   | PRI | NULL                | auto_increment |
| CrsCod    | int                                                               | NO   | MUL | -1                  |                |
| Type      | enum('unk','inf','prg','gui','lec','pra','bib','faq','lnk','ass') | NO   |     | unk                 |                |
| NodInd    | int                                                               | NO   |     | 0                   |                |
| Level     | int                                                               | NO   |     | 1                   |                |
| Hidden    | enum('N','Y')                                                     | NO   |     | N                   |                |
| UsrCod    | int                                                               | NO   |     | NULL                |                |
| StartTime | datetime                                                          | NO   |     | 1970-01-01 01:00:00 |                |
| EndTime   | datetime                                                          | NO   |     | 1970-01-01 01:00:00 |                |
| Title     | varchar(2047)                                                     | NO   |     | NULL                |                |
| Txt       | text                                                              | NO   |     | NULL                |                |
+-----------+-------------------------------------------------------------------+------+-----+---------------------+----------------+
11 rows in set (0,00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS tre_nodes ("
			"NodCod INT NOT NULL AUTO_INCREMENT,"
			"CrsCod INT NOT NULL DEFAULT -1,"
			"Type ENUM('unk','inf','prg','gui','lec','pra','bib','faq','lnk','ass') NOT NULL DEFAULT 'unk',"
			"NodInd INT NOT NULL DEFAULT 0,"
			"Level INT NOT NULL DEFAULT 1,"
			"Hidden ENUM('N','Y') NOT NULL DEFAULT 'N',"
			"UsrCod INT NOT NULL,"
			"StartTime DATETIME NOT NULL,"
			"EndTime DATETIME NOT NULL,"
			"Title VARCHAR(2047) NOT NULL,"		// Tre_MAX_BYTES_NODE_TITLE
			"Txt TEXT NOT NULL,"			// Cns_MAX_BYTES_TEXT
		   "UNIQUE INDEX(NodCod),"
		   "UNIQUE INDEX(CrsCod,Type,NodInd)"
		   ") ENGINE=MyISAM");

   /***** Table tst_answers *****/
/*
mysql> DESCRIBE tst_answers;
+----------+---------------+------+-----+---------+-------+
| Field    | Type          | Null | Key | Default | Extra |
+----------+---------------+------+-----+---------+-------+
| QstCod   | int(11)       | NO   | MUL | NULL    |       |
| AnsInd   | tinyint(4)    | NO   |     | NULL    |       |
| Answer   | text          | NO   |     | NULL    |       |
| Feedback | text          | NO   |     | NULL    |       |
| MedCod   | int(11)       | NO   | MUL | -1      |       |
| Correct  | enum('N','Y') | NO   |     | NULL    |       |
+----------+---------------+------+-----+---------+-------+
6 rows in set (0.00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS tst_answers ("
			"QstCod INT NOT NULL,"
			"AnsInd TINYINT NOT NULL,"
			"Answer TEXT NOT NULL,"				// Qst_MAX_BYTES_ANSWER_OR_FEEDBACK
			"Feedback TEXT NOT NULL,"			// Qst_MAX_BYTES_ANSWER_OR_FEEDBACK
			"MedCod INT NOT NULL DEFAULT -1,"
			"Correct ENUM('N','Y') NOT NULL,"
		   "INDEX(QstCod),"
		   "INDEX(MedCod)"
		   ") ENGINE=MyISAM");

   /***** Table tst_config *****/
/*
mysql> DESCRIBE tst_config;
+---------------------+-------------------------+------+-----+---------+-------+
| Field               | Type                    | Null | Key | Default | Extra |
+---------------------+-------------------------+------+-----+---------+-------+
| CrsCod              | int(11)                 | NO   | PRI | -1      |       |
| Pluggable           | enum('unknown','N','Y') | NO   |     | unknown |       |
| Min                 | int(11)                 | NO   |     | NULL    |       |
| Def                 | int(11)                 | NO   |     | NULL    |       |
| Max                 | int(11)                 | NO   |     | NULL    |       |
| MinTimeNxtTstPerQst | int(11)                 | NO   |     | 0       |       |
| Visibility          | int(11)                 | NO   |     | 31      |       |
+---------------------+-------------------------+------+-----+---------+-------+
7 rows in set (0.00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS tst_config ("
			"CrsCod INT NOT NULL DEFAULT -1,"
			"Pluggable ENUM('unknown','N','Y') NOT NULL DEFAULT 'unknown',"
			"Min INT NOT NULL,"
			"Def INT NOT NULL,"
			"Max INT NOT NULL,"
			"MinTimeNxtTstPerQst INT NOT NULL DEFAULT 0,"
			"Visibility INT NOT NULL DEFAULT 0x1f,"
		   "UNIQUE INDEX(CrsCod)"
		   ") ENGINE=MyISAM");

/***** Table tst_exam_questions *****/
/*
mysql> DESCRIBE tst_exam_questions;
+---------+---------+------+-----+---------+-------+
| Field   | Type    | Null | Key | Default | Extra |
+---------+---------+------+-----+---------+-------+
| ExaCod  | int(11) | NO   | PRI | NULL    |       |
| QstCod  | int(11) | NO   | PRI | NULL    |       |
| QstInd  | int(11) | NO   |     | NULL    |       |
| Score   | double  | NO   |     | 0       |       |
| Indexes | text    | NO   |     | NULL    |       |
| Answers | text    | NO   |     | NULL    |       |
+---------+---------+------+-----+---------+-------+
6 rows in set (0.00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS tst_exam_questions ("	// TODO: rename as tst_print_questions
			"ExaCod INT NOT NULL,"				// TODO: rename as PrnCod
			"QstCod INT NOT NULL,"
			"QstInd INT NOT NULL,"
			"Score DOUBLE PRECISION NOT NULL DEFAULT 0,"
			"Indexes TEXT NOT NULL,"	// Tst_MAX_BYTES_INDEXES_ONE_QST
			"Answers TEXT NOT NULL,"	// Tst_MAX_BYTES_ANSWERS_ONE_QST
		   "UNIQUE INDEX(ExaCod,QstCod)"
		   ") ENGINE=MyISAM");

   /***** Table tst_exams *****/	// TODO: rename as tst_prints
/*
mysql> DESCRIBE tst_exams;
+-----------------+---------------+------+-----+---------+----------------+
| Field           | Type          | Null | Key | Default | Extra          |
+-----------------+---------------+------+-----+---------+----------------+
| ExaCod          | int(11)       | NO   | PRI | NULL    | auto_increment |
| CrsCod          | int(11)       | NO   | MUL | NULL    |                |
| UsrCod          | int(11)       | NO   |     | NULL    |                |
| StartTime       | datetime      | NO   |     | NULL    |                |
| EndTime         | datetime      | NO   |     | NULL    |                |
| NumQsts         | int(11)       | NO   |     | 0       |                |
| NumQstsNotBlank | int(11)       | NO   |     | 0       |                |
| Sent            | enum('N','Y') | NO   |     | N       |                |
| AllowTeachers   | enum('N','Y') | NO   |     | N       |                |
| Score           | double        | NO   |     | 0       |                |
+-----------------+---------------+------+-----+---------+----------------+
10 rows in set (0.00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS tst_exams ("	// TODO: rename as tst_prints
			"ExaCod INT NOT NULL AUTO_INCREMENT,"	// TODO: rename as PrnCod
			"CrsCod INT NOT NULL,"
			"UsrCod INT NOT NULL,"
			"StartTime DATETIME NOT NULL,"
			"EndTime DATETIME NOT NULL,"
			"NumQsts INT NOT NULL DEFAULT 0,"
			"NumQstsNotBlank INT NOT NULL DEFAULT 0,"
			"Sent ENUM('N','Y') NOT NULL DEFAULT 'N',"
			"AllowTeachers ENUM('N','Y') NOT NULL DEFAULT 'N',"
			"Score DOUBLE PRECISION NOT NULL DEFAULT 0,"
		   "UNIQUE INDEX(ExaCod),"			// TODO: rename as PrnCod
		   "INDEX(CrsCod,UsrCod)"
		   ") ENGINE=MyISAM");

   /***** Table tst_question_tags *****/
/*
mysql> DESCRIBE tst_question_tags;
+--------+------------+------+-----+---------+-------+
| Field  | Type       | Null | Key | Default | Extra |
+--------+------------+------+-----+---------+-------+
| QstCod | int(11)    | NO   | PRI | NULL    |       |
| TagCod | int(11)    | NO   | PRI | NULL    |       |
| TagInd | tinyint(4) | NO   |     | NULL    |       |
+--------+------------+------+-----+---------+-------+
3 rows in set (0.00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS tst_question_tags ("
			"QstCod INT NOT NULL,"
			"TagCod INT NOT NULL,"
			"TagInd TINYINT NOT NULL,"
		   "UNIQUE INDEX(QstCod,TagCod)"
		   ") ENGINE=MyISAM");

   /***** Table tst_questions *****/
/*
mysql> DESCRIBE tst_questions;
+-----------------+---------------------------------------------------------------------------+------+-----+---------+----------------+
| Field           | Type                                                                      | Null | Key | Default | Extra          |
+-----------------+---------------------------------------------------------------------------+------+-----+---------+----------------+
| QstCod          | int(11)                                                                   | NO   | PRI | NULL    | auto_increment |
| CrsCod          | int(11)                                                                   | NO   | MUL | -1      |                |
| EditTime        | datetime                                                                  | NO   |     | NULL    |                |
| AnsType         | enum('int','float','true_false','unique_choice','multiple_choice','text') | NO   |     | NULL    |                |
| Shuffle         | enum('N','Y')                                                             | NO   |     | NULL    |                |
| Stem            | text                                                                      | NO   |     | NULL    |                |
| Feedback        | text                                                                      | NO   |     | NULL    |                |
| MedCod          | int(11)                                                                   | NO   | MUL | -1      |                |
| NumHits         | int(11)                                                                   | NO   |     | 0       |                |
| NumHitsNotBlank | int(11)                                                                   | NO   |     | 0       |                |
| Score           | double                                                                    | NO   |     | 0       |                |
+-----------------+---------------------------------------------------------------------------+------+-----+---------+----------------+
11 rows in set (0.00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS tst_questions ("
			"QstCod INT NOT NULL AUTO_INCREMENT,"
			"CrsCod INT NOT NULL DEFAULT -1,"
			"EditTime DATETIME NOT NULL,"
			"AnsType ENUM ('int','float','true_false','unique_choice','multiple_choice','text') NOT NULL,"
			"Shuffle ENUM('N','Y') NOT NULL,"
			"Stem TEXT NOT NULL,"				// Cns_MAX_BYTES_TEXT
			"Feedback TEXT NOT NULL,"			// Cns_MAX_BYTES_TEXT
			"MedCod INT NOT NULL DEFAULT -1,"
			"NumHits INT NOT NULL DEFAULT 0,"
			"NumHitsNotBlank INT NOT NULL DEFAULT 0,"
			"Score DOUBLE PRECISION NOT NULL DEFAULT 0,"
		   "UNIQUE INDEX(QstCod),"
		   "INDEX(CrsCod,EditTime),"
		   "INDEX(MedCod)"
		   ") ENGINE=MyISAM");

   /***** Table tst_tags *****/
/*
mysql> DESCRIBE tst_tags;
+------------+---------------+------+-----+---------+----------------+
| Field      | Type          | Null | Key | Default | Extra          |
+------------+---------------+------+-----+---------+----------------+
| TagCod     | int(11)       | NO   | PRI | NULL    | auto_increment |
| CrsCod     | int(11)       | NO   | MUL | -1      |                |
| ChangeTime | datetime      | NO   |     | NULL    |                |
| TagTxt     | varchar(2047) | NO   |     | NULL    |                |
| TagHidden  | enum('N','Y') | NO   |     | NULL    |                |
+------------+---------------+------+-----+---------+----------------+
5 rows in set (0,00 sec)
*/
// CrsCod is redundant for speed in querys
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS tst_tags ("
			"TagCod INT NOT NULL AUTO_INCREMENT,"
			"CrsCod INT NOT NULL DEFAULT -1,"
			"ChangeTime DATETIME NOT NULL,"
			"TagTxt VARCHAR(2047) NOT NULL,"	// Tag_MAX_BYTES_TAG
			"TagHidden ENUM('N','Y') NOT NULL,"
		   "UNIQUE INDEX(TagCod),"
		   "INDEX(CrsCod,ChangeTime)"
		   ") ENGINE=MyISAM");

/***** Table usr_admins *****/
/*
mysql> DESCRIBE usr_admins;
+--------+-------------------------------+------+-----+---------+-------+
| Field  | Type                          | Null | Key | Default | Extra |
+--------+-------------------------------+------+-----+---------+-------+
| UsrCod | int(11)                       | NO   | PRI | NULL    |       |
| Scope  | enum('Sys','Ins','Ctr','Deg') | NO   | PRI | NULL    |       |
| Cod    | int(11)                       | NO   | PRI | NULL    |       |
+--------+-------------------------------+------+-----+---------+-------+
3 rows in set (0.00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS usr_admins ("
			"UsrCod INT NOT NULL,"
			"Scope ENUM('Sys','Ins','Ctr','Deg') NOT NULL,"
			"Cod INT NOT NULL,"
		   "UNIQUE INDEX(UsrCod,Scope,Cod),"
		   "INDEX (Scope,Cod)"
		   ") ENGINE=MyISAM");

   /***** Table usr_banned *****/
/*
mysql> DESCRIBE usr_banned;
+--------+---------+------+-----+---------+-------+
| Field  | Type    | Null | Key | Default | Extra |
+--------+---------+------+-----+---------+-------+
| UsrCod | int(11) | NO   | PRI | NULL    |       |
+--------+---------+------+-----+---------+-------+
1 row in set (0.01 sec)

*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS usr_banned ("
			"UsrCod INT NOT NULL,"
		   "UNIQUE INDEX(UsrCod)"
		   ") ENGINE=MyISAM");

   /***** Table usr_birthdays_today *****/
   // Stores birthdays already congratulated today
/*
mysql> DESCRIBE usr_birthdays_today;
+--------+---------+------+-----+---------+-------+
| Field  | Type    | Null | Key | Default | Extra |
+--------+---------+------+-----+---------+-------+
| UsrCod | int(11) | NO   | PRI | NULL    |       |
| Today  | date    | NO   | MUL | NULL    |       |
+--------+---------+------+-----+---------+-------+
2 rows in set (0.09 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS usr_birthdays_today ("
			"UsrCod INT NOT NULL,"
			"Today DATE NOT NULL,"
		   "UNIQUE INDEX(UsrCod),"
		   "INDEX(Today)"
		   ") ENGINE=MyISAM");

   /***** Table usr_clicks_without_photo *****/
/*
mysql> DESCRIBE usr_clicks_without_photo;
+-----------+---------+------+-----+---------+-------+
| Field     | Type    | Null | Key | Default | Extra |
+-----------+---------+------+-----+---------+-------+
| UsrCod    | int(11) | NO   | PRI | NULL    |       |
| NumClicks | int(11) | NO   |     | NULL    |       |
+-----------+---------+------+-----+---------+-------+
2 rows in set (0.00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS usr_clicks_without_photo ("
			"UsrCod INT NOT NULL,"
			"NumClicks INT NOT NULL,"
		   "UNIQUE INDEX(UsrCod)"
		   ") ENGINE=MyISAM");


   /***** Table usr_clipboards *****/
/*
mysql> DESCRIBE usr_clipboards;
+-----------+-----------+------+-----+---------+-------+
| Field     | Type      | Null | Key | Default | Extra |
+-----------+-----------+------+-----+---------+-------+
| UsrCod    | int       | NO   | PRI | NULL    |       |
| OthUsrCod | int       | NO   | PRI | NULL    |       |
| CopyTime  | timestamp | YES  | MUL | NULL    |       |
+-----------+-----------+------+-----+---------+-------+
3 rows in set (0,00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS usr_clipboards ("
			"UsrCod INT NOT NULL,"
			"OthUsrCod INT NOT NULL,"
			"CopyTime TIMESTAMP,"
		   "UNIQUE INDEX(UsrCod,OthUsrCod),"
		   "INDEX(CopyTime)"
		   ") ENGINE=MyISAM");

   /***** Table usr_connected *****/
/*
mysql> DESCRIBE usr_connected;
+---------------+------------+------+-----+---------+-------+
| Field         | Type       | Null | Key | Default | Extra |
+---------------+------------+------+-----+---------+-------+
| UsrCod        | int(11)    | NO   | PRI | NULL    |       |
| RoleInLastCrs | tinyint(4) | NO   | MUL | 0       |       |
| LastCrsCod    | int(11)    | NO   | MUL | -1      |       |
| LastTime      | datetime   | NO   |     | NULL    |       |
+---------------+------------+------+-----+---------+-------+
4 rows in set (0,00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS usr_connected ("
			"UsrCod INT NOT NULL,"
			"RoleInLastCrs TINYINT NOT NULL DEFAULT 0,"
			"LastCrsCod INT NOT NULL DEFAULT -1,"
			"LastTime DATETIME NOT NULL,"
		   "UNIQUE INDEX(UsrCod),"
		   "INDEX(RoleInLastCrs),"
		   "INDEX(LastCrsCod)"
		   ") ENGINE=MyISAM");

   /***** Table usr_data *****/
/*
mysql> DESCRIBE usr_data;
+-------------------+--------------------------------------------------+------+-----+---------+----------------+
| Field             | Type                                             | Null | Key | Default | Extra          |
+-------------------+--------------------------------------------------+------+-----+---------+----------------+
| UsrCod            | int                                              | NO   | PRI | NULL    | auto_increment |
| EncryptedUsrCod   | char(43)                                         | NO   | UNI |         |                |
| Password          | char(86)                                         | NO   |     |         |                |
| Surname1          | varchar(511)                                     | NO   |     |         |                |
| Surname2          | varchar(511)                                     | NO   |     |         |                |
| FirstName         | varchar(511)                                     | NO   |     |         |                |
| Sex               | enum('unknown','female','male')                  | NO   |     | unknown |                |
| Theme             | char(16)                                         | NO   | MUL |         |                |
| IconSet           | char(16)                                         | NO   | MUL |         |                |
| Language          | char(2)                                          | NO   | MUL |         |                |
| FirstDayOfWeek    | tinyint                                          | NO   | MUL | 0       |                |
| DateFormat        | tinyint                                          | NO   | MUL | 0       |                |
| Photo             | char(43)                                         | NO   |     |         |                |
| PhotoVisibility   | enum('unknown','user','course','system','world') | NO   | MUL | unknown |                |
| BaPrfVisibility   | enum('unknown','user','course','system','world') | NO   | MUL | unknown |                |
| ExPrfVisibility   | enum('unknown','user','course','system','world') | NO   | MUL | unknown |                |
| CtyCod            | int                                              | NO   | MUL | -1      |                |
| InsCtyCod         | int                                              | NO   | MUL | -1      |                |
| InsCod            | int                                              | NO   | MUL | -1      |                |
| DptCod            | int                                              | NO   | MUL | -1      |                |
| CtrCod            | int                                              | NO   | MUL | -1      |                |
| Office            | varchar(2047)                                    | NO   |     |         |                |
| OfficePhone       | char(16)                                         | NO   |     |         |                |
| LocalAddress      | varchar(2047)                                    | NO   |     |         |                |
| LocalPhone        | char(16)                                         | NO   |     |         |                |
| FamilyAddress     | varchar(2047)                                    | NO   |     |         |                |
| FamilyPhone       | char(16)                                         | NO   |     |         |                |
| OriginPlace       | varchar(2047)                                    | NO   |     |         |                |
| Birthday          | date                                             | YES  |     | NULL    |                |
| Comments          | text                                             | NO   |     | NULL    |                |
| Menu              | tinyint                                          | NO   | MUL | 0       |                |
| SideCols          | tinyint                                          | NO   | MUL | 3       |                |
| PhotoShape        | tinyint                                          | NO   | MUL | 0       |                |
| ThirdPartyCookies | enum('N','Y')                                    | NO   | MUL | N       |                |
| NotifNtfEvents    | int                                              | NO   |     | 0       |                |
| EmailNtfEvents    | int                                              | NO   |     | 0       |                |
+-------------------+--------------------------------------------------+------+-----+---------+----------------+
36 rows in set (0,00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS usr_data ("
			"UsrCod INT NOT NULL AUTO_INCREMENT,"
			"EncryptedUsrCod CHAR(43) NOT NULL DEFAULT '',"
			"Password CHAR(86) COLLATE latin1_bin NOT NULL DEFAULT '',"		// Pwd_BYTES_ENCRYPTED_PASSWORD
			"Surname1 VARCHAR(511) COLLATE latin1_spanish_ci NOT NULL DEFAULT '',"	// Usr_MAX_BYTES_FIRSTNAME_OR_SURNAME
			"Surname2 VARCHAR(511) COLLATE latin1_spanish_ci NOT NULL DEFAULT '',"	// Usr_MAX_BYTES_FIRSTNAME_OR_SURNAME
			"FirstName VARCHAR(511) COLLATE latin1_spanish_ci NOT NULL DEFAULT '',"	// Usr_MAX_BYTES_FIRSTNAME_OR_SURNAME
			"Sex ENUM ('unknown','female','male') NOT NULL DEFAULT 'unknown',"
			"Theme CHAR(16) NOT NULL DEFAULT '',"
			"IconSet CHAR(16) NOT NULL DEFAULT '',"
			"Language CHAR(2) NOT NULL DEFAULT '',"
			"FirstDayOfWeek TINYINT NOT NULL DEFAULT 0,"
			"DateFormat TINYINT NOT NULL DEFAULT 0,"
			"Photo CHAR(43) NOT NULL DEFAULT '',"					// Cry_BYTES_ENCRYPTED_STR_SHA256_BASE64
			"PhotoVisibility ENUM('unknown','user','course','system','world') NOT NULL DEFAULT 'unknown',"
			"BaPrfVisibility ENUM('unknown','user','course','system','world') NOT NULL DEFAULT 'unknown',"
			"ExPrfVisibility ENUM('unknown','user','course','system','world') NOT NULL DEFAULT 'unknown',"
			"CtyCod INT NOT NULL DEFAULT -1,"
			"InsCtyCod INT NOT NULL DEFAULT -1,"
			"InsCod INT NOT NULL DEFAULT -1,"
			"DptCod INT NOT NULL DEFAULT -1,"
			"CtrCod INT NOT NULL DEFAULT -1,"
			"Office VARCHAR(2047) NOT NULL DEFAULT '',"		// Usr_MAX_BYTES_ADDRESS
			"OfficePhone CHAR(16) NOT NULL DEFAULT '',"		// Usr_MAX_BYTES_PHONE
			"LocalAddress VARCHAR(2047) NOT NULL DEFAULT '',"	// Usr_MAX_BYTES_ADDRESS	// TODO: Remove, not yet used
			"LocalPhone CHAR(16) NOT NULL DEFAULT '',"		// Usr_MAX_BYTES_PHONE
			"FamilyAddress VARCHAR(2047) NOT NULL DEFAULT '',"	// Usr_MAX_BYTES_ADDRESS	// TODO: Remove, not yet used
			"FamilyPhone CHAR(16) NOT NULL DEFAULT '',"		// Usr_MAX_BYTES_PHONE
			"OriginPlace VARCHAR(2047) NOT NULL DEFAULT '',"	// Usr_MAX_BYTES_ADDRESS	// TODO: Remove, not yet used
			"Birthday DATE,"
			"Comments TEXT NOT NULL,"				// Cns_MAX_BYTES_TEXT
			"Menu TINYINT NOT NULL DEFAULT 0,"
			"SideCols TINYINT NOT NULL DEFAULT 3,"
			"PhotoShape TINYINT NOT NULL DEFAULT 0,"
			"ThirdPartyCookies ENUM('N','Y') NOT NULL DEFAULT 'N',"
			"NotifNtfEvents INT NOT NULL DEFAULT 0,"
			"EmailNtfEvents INT NOT NULL DEFAULT 0,"
		   "PRIMARY KEY(UsrCod),"
		   "UNIQUE INDEX(EncryptedUsrCod),"
		   "INDEX(Theme),"
		   "INDEX(IconSet),"
		   "INDEX(Language),"
		   "INDEX(FirstDayOfWeek),"
		   "INDEX(DateFormat),"
		   "INDEX(PhotoVisibility),"
		   "INDEX(BaPrfVisibility),"
		   "INDEX(ExPrfVisibility),"
		   "INDEX(CtyCod),"
		   "INDEX(InsCtyCod),"
		   "INDEX(InsCod),"
		   "INDEX(DptCod),"
		   "INDEX(CtrCod),"
		   "INDEX(Menu),"
		   "INDEX(SideCols),"
		   "INDEX(PhotoShape),"
		   "INDEX(ThirdPartyCookies)"
		   ") ENGINE=MyISAM");

   /***** Table usr_duplicated *****/
/*
mysql> DESCRIBE usr_duplicated;
+-------------+----------+------+-----+---------+-------+
| Field       | Type     | Null | Key | Default | Extra |
+-------------+----------+------+-----+---------+-------+
| UsrCod      | int(11)  | NO   | PRI | NULL    |       |
| InformerCod | int(11)  | NO   | PRI | NULL    |       |
| InformTime  | datetime | NO   |     | NULL    |       |
+-------------+----------+------+-----+---------+-------+
3 rows in set (0.00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS usr_duplicated ("
			"UsrCod INT NOT NULL,"
			"InformerCod INT NOT NULL,"
			"InformTime DATETIME NOT NULL,"
		   "UNIQUE INDEX(UsrCod,InformerCod),"
		   "INDEX(UsrCod)"
		   ") ENGINE=MyISAM");

   /***** Table usr_emails *****/
   /*
mysql> DESCRIBE usr_emails;
+-----------+---------------+------+-----+---------+-------+
| Field     | Type          | Null | Key | Default | Extra |
+-----------+---------------+------+-----+---------+-------+
| UsrCod    | int(11)       | NO   | PRI | NULL    |       |
| E_mail    | varchar(255)  | NO   | PRI | NULL    |       |
| CreatTime | datetime      | NO   |     | NULL    |       |
| Confirmed | enum('N','Y') | NO   |     | N       |       |
+-----------+---------------+------+-----+---------+-------+
4 rows in set (0,00 sec)
   */
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS usr_emails ("
			"UsrCod INT NOT NULL,"
			"E_mail VARCHAR(255) COLLATE latin1_general_ci NOT NULL,"	// Cns_MAX_CHARS_EMAIL_ADDRESS
			"CreatTime DATETIME NOT NULL,"
			"Confirmed ENUM('N','Y') NOT NULL DEFAULT 'N',"
		   "UNIQUE INDEX(UsrCod,E_mail),"
		   "UNIQUE INDEX(E_mail)"
		   ") ENGINE=MyISAM");

   /***** Table usr_figures *****/
   /*
mysql> DESCRIBE usr_figures;
+----------------+----------+------+-----+---------+-------+
| Field          | Type     | Null | Key | Default | Extra |
+----------------+----------+------+-----+---------+-------+
| UsrCod         | int(11)  | NO   | PRI | NULL    |       |
| FirstClickTime | datetime | NO   | MUL | NULL    |       |
| NumClicks      | int(11)  | NO   | MUL | -1      |       |
| NumSocPub      | int(11)  | NO   |     | -1      |       |
| NumFileViews   | int(11)  | NO   |     | -1      |       |
| NumForPst      | int(11)  | NO   |     | -1      |       |
| NumMsgSnt      | int(11)  | NO   |     | -1      |       |
+----------------+----------+------+-----+---------+-------+
7 rows in set (0.01 sec)
   */
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS usr_figures ("
			"UsrCod INT NOT NULL,"
			"FirstClickTime DATETIME NOT NULL,"
			"NumClicks INT NOT NULL DEFAULT -1,"
			"NumSocPub INT NOT NULL DEFAULT -1,"	// TODO: Rename to NumTimelinePubs
			"NumFileViews INT NOT NULL DEFAULT -1,"
			"NumForPst INT NOT NULL DEFAULT -1,"	// TODO: Rename to NumForumPosts
			"NumMsgSnt INT NOT NULL DEFAULT -1,"	// TODO: Rename to NumMessagesSent
		   "PRIMARY KEY(UsrCod),"
		   "INDEX(FirstClickTime),"
		   "INDEX(NumClicks)"
		   ") ENGINE=MyISAM");

   /***** Table usr_follow *****/
   /*
mysql> DESCRIBE usr_follow;
+-------------+----------+------+-----+---------+-------+
| Field       | Type     | Null | Key | Default | Extra |
+-------------+----------+------+-----+---------+-------+
| FollowerCod | int(11)  | NO   | PRI | NULL    |       |
| FollowedCod | int(11)  | NO   | PRI | NULL    |       |
| FollowTime  | datetime | NO   | MUL | NULL    |       |
+-------------+----------+------+-----+---------+-------+
3 rows in set (0.00 sec)
   */
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS usr_follow ("
			"FollowerCod INT NOT NULL,"
			"FollowedCod INT NOT NULL,"
			"FollowTime DATETIME NOT NULL,"
		   "UNIQUE INDEX (FollowerCod,FollowedCod),"
		   "UNIQUE INDEX (FollowedCod,FollowerCod),"
		   "INDEX (FollowTime)"
		   ") ENGINE=MyISAM");

/***** Table usr_ids *****/
/*
mysql> DESCRIBE usr_ids;
+-----------+---------------+------+-----+---------+-------+
| Field     | Type          | Null | Key | Default | Extra |
+-----------+---------------+------+-----+---------+-------+
| UsrCod    | int(11)       | NO   | PRI | NULL    |       |
| UsrID     | char(16)      | NO   | PRI | NULL    |       |
| CreatTime | datetime      | NO   |     | NULL    |       |
| Confirmed | enum('N','Y') | NO   |     | N       |       |
+-----------+---------------+------+-----+---------+-------+
4 rows in set (0.01 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS usr_ids ("
			"UsrCod INT NOT NULL,"
			"UsrID CHAR(16) NOT NULL,"	// ID_MAX_BYTES_USR_ID
			"CreatTime DATETIME NOT NULL,"
			"Confirmed ENUM('N','Y') NOT NULL DEFAULT 'N',"
		   "UNIQUE INDEX(UsrCod,UsrID),"
		   "INDEX(UsrID)"
		   ") ENGINE=MyISAM");

   /***** Table usr_last *****/
/*
mysql> DESCRIBE usr_last;
+--------------+-------------------------------------------------+------+-----+---------+-------+
| Field        | Type                                            | Null | Key | Default | Extra |
+--------------+-------------------------------------------------+------+-----+---------+-------+
| UsrCod       | int(11)                                         | NO   | PRI | NULL    |       |
| WhatToSearch | tinyint(4)                                      | NO   |     | 0       |       |
| LastSco      | enum('Unk','Sys','Cty','Ins','Ctr','Deg','Crs') | NO   |     | Unk     |       |
| LastCod      | int(11)                                         | NO   |     | -1      |       |
| LastAct      | int(11)                                         | NO   |     | -1      |       |
| LastRole     | tinyint(4)                                      | NO   |     | 0       |       |
| LastTime     | datetime                                        | NO   | MUL | NULL    |       |
| LastAccNotif | datetime                                        | NO   |     | NULL    |       |
| TimelineUsrs | tinyint(4)                                      | NO   |     | 0       |       |
+--------------+-------------------------------------------------+------+-----+---------+-------+
9 rows in set (0.01 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS usr_last ("
			"UsrCod INT NOT NULL,"
			"WhatToSearch TINYINT NOT NULL DEFAULT 0,"
			"LastSco ENUM('Unk','Sys','Cty','Ins','Ctr','Deg','Crs') NOT NULL DEFAULT 'Unk',"
			"LastCod INT NOT NULL DEFAULT -1,"
			"LastAct INT NOT NULL DEFAULT -1,"
	 		"LastRole TINYINT NOT NULL DEFAULT 0,"
			"LastTime DATETIME NOT NULL,"
			"LastAccNotif DATETIME NOT NULL,"
			"TimelineUsrs TINYINT NOT NULL DEFAULT 0,"
		   "UNIQUE INDEX(UsrCod),"
		   "INDEX(LastTime)"
		   ") ENGINE=MyISAM");

/***** Table usr_nicknames *****/
/*
mysql> DESCRIBE usr_nicknames;
+-----------+----------+------+-----+---------+-------+
| Field     | Type     | Null | Key | Default | Extra |
+-----------+----------+------+-----+---------+-------+
| UsrCod    | int(11)  | NO   | PRI | NULL    |       |
| Nickname  | char(16) | NO   | PRI | NULL    |       |
| CreatTime | datetime | NO   |     | NULL    |       |
+-----------+----------+------+-----+---------+-------+
3 rows in set (0.01 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS usr_nicknames ("
			"UsrCod INT NOT NULL,"
			"Nickname CHAR(16) COLLATE latin1_spanish_ci NOT NULL,"	// Nck_MAX_BYTES_NICK_WITHOUT_ARROBA
			"CreatTime DATETIME NOT NULL,"
		   "UNIQUE INDEX(UsrCod,Nickname),"
		   "UNIQUE INDEX(Nickname)"
		   ") ENGINE=MyISAM");

   /***** Table usr_pending_emails *****/
/*
mysql> DESCRIBE usr_pending_emails;
+-------------+--------------+------+-----+---------+-------+
| Field       | Type         | Null | Key | Default | Extra |
+-------------+--------------+------+-----+---------+-------+
| UsrCod      | int(11)      | NO   | MUL | NULL    |       |
| E_mail      | varchar(255) | NO   |     | NULL    |       |
| MailKey     | char(43)     | NO   | PRI | NULL    |       |
| DateAndTime | datetime     | NO   |     | NULL    |       |
+-------------+--------------+------+-----+---------+-------+
4 rows in set (0,00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS usr_pending_emails ("
			"UsrCod INT NOT NULL,"
			"E_mail VARCHAR(255) COLLATE latin1_general_ci NOT NULL,"	// Cns_MAX_CHARS_EMAIL_ADDRESS
			"MailKey CHAR(43) COLLATE latin1_bin NOT NULL,"			// Mai_LENGTH_EMAIL_CONFIRM_KEY
			"DateAndTime DATETIME NOT NULL,"
		   "INDEX(UsrCod),"
		   "UNIQUE INDEX(MailKey)"
		   ") ENGINE=MyISAM");

   /***** Table usr_pending_passwd *****/
/*
mysql> DESCRIBE usr_pending_passwd;
+-----------------+----------+------+-----+---------+-------+
| Field           | Type     | Null | Key | Default | Extra |
+-----------------+----------+------+-----+---------+-------+
| UsrCod          | int(11)  | NO   | PRI | NULL    |       |
| PendingPassword | char(86) | NO   |     | NULL    |       |
| DateAndTime     | datetime | NO   |     | NULL    |       |
+-----------------+----------+------+-----+---------+-------+
3 rows in set (0.00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS usr_pending_passwd ("
			"UsrCod INT NOT NULL,"
			"PendingPassword CHAR(86) COLLATE latin1_bin NOT NULL,"	// Pwd_BYTES_ENCRYPTED_PASSWORD
			"DateAndTime DATETIME NOT NULL,"
		   "PRIMARY KEY (UsrCod)"
		   ") ENGINE=MyISAM");

   /***** Table usr_reports *****/
/*
mysql> DESCRIBE usr_reports;
+---------------+--------------+------+-----+---------+----------------+
| Field         | Type         | Null | Key | Default | Extra          |
+---------------+--------------+------+-----+---------+----------------+
| RepCod        | int(11)      | NO   | PRI | NULL    | auto_increment |
| UsrCod        | int(11)      | NO   | MUL | NULL    |                |
| ReportTimeUTC | datetime     | NO   |     | NULL    |                |
| UniqueDirL    | char(2)      | NO   |     | NULL    |                |
| UniqueDirR    | char(41)     | NO   |     | NULL    |                |
| Filename      | varchar(255) | NO   |     | NULL    |                |
| Permalink     | varchar(255) | NO   |     | NULL    |                |
+---------------+--------------+------+-----+---------+----------------+
7 rows in set (0,00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS usr_reports ("
			"RepCod INT NOT NULL AUTO_INCREMENT,"
			"UsrCod INT NOT NULL,"
			"ReportTimeUTC DATETIME NOT NULL,"
			"UniqueDirL CHAR(2) NOT NULL,"	//  2  leftmost chars from a unique 43 chars base64url codified from a unique SHA-256 string
			"UniqueDirR CHAR(41) NOT NULL,"	// 41 rightmost chars from a unique 43 chars base64url codified from a unique SHA-256 string
			"Filename VARCHAR(255) NOT NULL,"	// Report filename, NAME_MAX
			"Permalink VARCHAR(255) NOT NULL,"	// Full URL (permalink), WWW_MAX_BYTES_WWW
		   "UNIQUE INDEX(RepCod),"
		   "INDEX(UsrCod)"
		   ") ENGINE=MyISAM");

/***** Table usr_webs *****/
/*
mysql> DESCRIBE usr_webs;
+--------+---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+------+-----+---------+-------+
| Field  | Type                                                                                                                                                                                                                                                                                                                                        | Null | Key | Default | Extra |
+--------+---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+------+-----+---------+-------+
| UsrCod | int                                                                                                                                                                                                                                                                                                                                         | NO   | PRI | NULL    |       |
| Web    | enum('www','500px','bluesky','delicious','deviantart','diaspora','edmodo','facebook','flickr','foursquare','github','gnusocial','googlescholar','identica','instagram','linkedin','orcid','paperli','pinterest','researchgate','researcherid','scoopit','slideshare','stackoverflow','storify','tumblr','twitch','wikipedia','youtube','x') | NO   | PRI | NULL    |       |
| URL    | varchar(255)                                                                                                                                                                                                                                                                                                                                | NO   |     | NULL    |       |
+--------+---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+------+-----+---------+-------+
3 rows in set (0,00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS usr_webs ("
			"UsrCod INT NOT NULL,"
			"Web ENUM("
			"'www',"
			"'500px',"
			"'bluesky'"
			"'edmodo',"
			"'facebook','flickr','foursquare',"
			"'github','gnusocial','googlescholar',"
			"'identica','instagram',"
			"'linkedin',"
			"'orcid',"
			"'paperli','pinterest',"
			"'researchgate','researcherid',"
			"'scoopit','slideshare','stackoverflow','storify',"
			"'tumblr','twitch','twitter',"
			"'wikipedia',"
			"'youtube',"
			"'x') NOT NULL,"
			"URL VARCHAR(255) NOT NULL,"	// WWW_MAX_BYTES_WWW
		   "UNIQUE INDEX(UsrCod,Web)"
		   ") ENGINE=MyISAM");

   /***** Show success message *****/
   HTM_OL_End ();
   Ale_ShowAlert (Ale_SUCCESS,Txt_Created_tables_in_the_database_that_did_not_exist);
  }

/*****************************************************************************/
/****************** Create a database table if not exists ********************/
/*****************************************************************************/

static void DB_CreateTable (const char *Query)
  {
   HTM_LI_Begin ("class=\"DAT_%s\"",The_GetSuffix ());
      HTM_Txt (Query);
   HTM_LI_End ();

   if (mysql_query (&DB_Database.mysql,Query))
      DB_ExitOnMySQLError ("can not create table");
  }

/*****************************************************************************/
/********************* Open connection to the database ***********************/
/*****************************************************************************/

void DB_OpenDBConnection (void)
  {
   if (mysql_init (&DB_Database.mysql) == NULL)
      Err_ShowErrorAndExit ("Can not init MySQL.");

   if (mysql_real_connect (&DB_Database.mysql,Cfg_DATABASE_HOST,
	                   Cfg_DATABASE_USER,Cfg_GetDatabasePassword (),
	                   Cfg_DATABASE_DBNAME,0,NULL,0) == NULL)
      DB_ExitOnMySQLError ("can not connect to database");

   DB_Database.IsOpen = CloOpe_OPEN;
  }

/*****************************************************************************/
/************************ Check if database is open **************************/
/*****************************************************************************/

CloOpe_ClosedOrOpen_t DB_CheckIfDatabaseIsOpen (void)
  {
   return DB_Database.IsOpen;
  }

/*****************************************************************************/
/********************* Close connection to the database **********************/
/*****************************************************************************/

void DB_CloseDBConnection (void)
  {
   if (DB_CheckIfDatabaseIsOpen () == CloOpe_OPEN)
     {
      mysql_close (&DB_Database.mysql);	// Close the connection to the database
      DB_Database.IsOpen = CloOpe_CLOSED;
     }
  }

/*****************************************************************************/
/********************** Build a query to be used later ***********************/
/*****************************************************************************/

void DB_BuildQuery (char **Query,const char *fmt,...)
  {
   va_list ap;
   int NumBytesPrinted;

   if (*Query != NULL)
      Err_ShowErrorAndExit ("Error building query.");

   va_start (ap,fmt);
   NumBytesPrinted = vasprintf (Query,fmt,ap);
   va_end (ap);
   if (NumBytesPrinted < 0)	// -1 if no memory or any other error
      Err_NotEnoughMemoryExit ();
  }

/*****************************************************************************/
/******************** Make a SELECT query from database **********************/
/*****************************************************************************/

unsigned long DB_QuerySELECT (MYSQL_RES **mysql_res,const char *MsgError,
                              const char *fmt,...)
  {
   va_list ap;
   int NumBytesPrinted;
   char *Query;

   /***** Create query string *****/
   va_start (ap,fmt);
   NumBytesPrinted = vasprintf (&Query,fmt,ap);
   va_end (ap);
   if (NumBytesPrinted < 0)	// -1 if no memory or any other error
      Err_NotEnoughMemoryExit ();

   /***** Do SELECT query *****/
   return DB_QuerySELECTusingQueryStr (Query,mysql_res,MsgError);
  }


Exi_Exist_t DB_QuerySELECTunique (MYSQL_RES **mysql_res,const char *MsgError,
                                  const char *fmt,...)
  {
   va_list ap;
   int NumBytesPrinted;
   char *Query;

   /***** Create query string *****/
   va_start (ap,fmt);
   NumBytesPrinted = vasprintf (&Query,fmt,ap);
   va_end (ap);
   if (NumBytesPrinted < 0)	// -1 if no memory or any other error
      Err_NotEnoughMemoryExit ();

   /***** Do SELECT query *****/
   return DB_QuerySELECTusingQueryStr (Query,mysql_res,MsgError) ? Exi_EXISTS :
								   Exi_DOES_NOT_EXIST;
  }

/*****************************************************************************/
/*** Make a SELECT query for a unique row with one long code from database ***/
/*****************************************************************************/

long DB_QuerySELECTCode (const char *MsgError,
                         const char *fmt,...)
  {
   MYSQL_RES *mysql_res;
   va_list ap;
   int NumBytesPrinted;
   char *Query;
   long Cod;

   /***** Create query string *****/
   va_start (ap,fmt);
   NumBytesPrinted = vasprintf (&Query,fmt,ap);
   va_end (ap);
   if (NumBytesPrinted < 0)	// -1 if no memory or any other error
      Err_NotEnoughMemoryExit ();

   /***** Do SELECT query *****/
   if (DB_QuerySELECTusingQueryStr (Query,&mysql_res,MsgError))	// Row found
      Cod = DB_GetNextCode (mysql_res);
   else
      Cod = -1L;

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return Cod;
  }

/*****************************************************************************/
/*** Make a SELECT query for a unique row with one unsigned from database ****/
/*****************************************************************************/

unsigned DB_QuerySELECTUnsigned (const char *MsgError,
                                 const char *fmt,...)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   va_list ap;
   int NumBytesPrinted;
   char *Query;
   unsigned UnsignedNum = 0;

   /***** Create query string *****/
   va_start (ap,fmt);
   NumBytesPrinted = vasprintf (&Query,fmt,ap);
   va_end (ap);
   if (NumBytesPrinted < 0)	// -1 if no memory or any other error
      Err_NotEnoughMemoryExit ();

   /***** Do SELECT query *****/
   if (DB_QuerySELECTusingQueryStr (Query,&mysql_res,MsgError))	// Row found
     {
      row = mysql_fetch_row (mysql_res);
      if (row[0])
	 UnsignedNum = Str_ConvertStrToUnsigned (row[0]);
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return UnsignedNum;
  }

/*****************************************************************************/
/**** Make a SELECT query for a unique row with one double from database *****/
/*****************************************************************************/

double DB_QuerySELECTDouble (const char *MsgError,
                             const char *fmt,...)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   va_list ap;
   int NumBytesPrinted;
   char *Query;
   double DoubleNum = 0.0;	// Default value

   /***** Create query string *****/
   va_start (ap,fmt);
   NumBytesPrinted = vasprintf (&Query,fmt,ap);
   va_end (ap);
   if (NumBytesPrinted < 0)	// -1 if no memory or any other error
      Err_NotEnoughMemoryExit ();

   /***** Do SELECT query *****/
   if (DB_QuerySELECTusingQueryStr (Query,&mysql_res,MsgError))	// Row found
     {
      row = mysql_fetch_row (mysql_res);
      if (Str_GetDoubleFromStr (row[0],&DoubleNum) == Err_ERROR)
	 DoubleNum = 0.0;
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return DoubleNum;
  }

/*****************************************************************************/
/***** Make a SELECT query for a unique row with one role from database ******/
/*****************************************************************************/

Rol_Role_t DB_QuerySELECTRole (const char *MsgError,
                               const char *fmt,...)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   va_list ap;
   int NumBytesPrinted;
   char *Query;
   Rol_Role_t Role = Rol_UNK;

   /***** Create query string *****/
   va_start (ap,fmt);
   NumBytesPrinted = vasprintf (&Query,fmt,ap);
   va_end (ap);
   if (NumBytesPrinted < 0)	// -1 if no memory or any other error
      Err_NotEnoughMemoryExit ();

   /***** Do SELECT query *****/
   if (DB_QuerySELECTusingQueryStr (Query,&mysql_res,MsgError))	// Row found
     {
      row = mysql_fetch_row (mysql_res);
      if (row[0])
	 Role = Rol_ConvertUnsignedStrToRole (row[0]);
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return Role;
  }

/*****************************************************************************/
/**** Make a SELECT query for a unique row with one string from database *****/
/*****************************************************************************/
// StrSize does not include the ending byte '\0'

void DB_QuerySELECTString (char *Str,size_t StrSize,const char *MsgError,
                           const char *fmt,...)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   va_list ap;
   int NumBytesPrinted;
   char *Query;

   /***** Create query string *****/
   va_start (ap,fmt);
   NumBytesPrinted = vasprintf (&Query,fmt,ap);
   va_end (ap);
   if (NumBytesPrinted < 0)	// -1 if no memory or any other error
      Err_NotEnoughMemoryExit ();

   /***** Do SELECT query *****/
   Str[0] = '\0';
   if (DB_QuerySELECTusingQueryStr (Query,&mysql_res,MsgError) == 1)	// Row found
     {
      row = mysql_fetch_row (mysql_res);
      Str_Copy (Str,row[0],StrSize);
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   /*
   if (TooBig)
     {
      snprintf (ErrorTxt,sizeof (ErrorTxt),
	        "Too large string from database,"
                " it exceed the maximum allowed size (%zu bytes).",
                StrSize);
      Err_ShowErrorAndExit (ErrorTxt);
     }
   */
  }

/*****************************************************************************/
/*********** Make a SELECT query from database using query string ************/
/*****************************************************************************/

static unsigned long DB_QuerySELECTusingQueryStr (char *Query,
					          MYSQL_RES **mysql_res,
						  const char *MsgError)
  {
   int Result;

   /***** Check that query string pointer
          does point to an allocated string *****/
   if (Query == NULL)
      Err_ShowErrorAndExit ("Wrong query string.");

   /***** Query database and free query string pointer *****/
   Result = mysql_query (&DB_Database.mysql,Query);	// Returns 0 on success
   free (Query);
   if (Result)
      DB_ExitOnMySQLError (MsgError);

   /***** Store query result *****/
   if ((*mysql_res = mysql_store_result (&DB_Database.mysql)) == NULL)
      DB_ExitOnMySQLError (MsgError);

   /***** Return number of rows of result *****/
   return (unsigned long) mysql_num_rows (*mysql_res);
  }

/*****************************************************************************/
/********************** Get next code from query result **********************/
/*****************************************************************************/
/* Each row of the result should hold a code (long) */

long DB_GetNextCode (MYSQL_RES *mysql_res)
  {
   MYSQL_ROW row;

   /***** Get next row from query result *****/
   row = mysql_fetch_row (mysql_res);

   /***** row[0] should hold a code (long) ******/
   return Str_ConvertStrCodToLongCod (row[0]);
  }

/*****************************************************************************/
/**************** Make a SELECT COUNT query from database ********************/
/*****************************************************************************/

unsigned long DB_GetNumRowsTable (const char *Table)
  {
   /***** Get total number of rows from database *****/
   return DB_QueryCOUNT ("can not get number of rows in table",
			 "SELECT COUNT(*)"
			  " FROM %s",
			 Table);
  }

unsigned long DB_QueryCOUNT (const char *MsgError,const char *fmt,...)
  {
   va_list ap;
   int NumBytesPrinted;
   char *Query;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;

   va_start (ap,fmt);
   NumBytesPrinted = vasprintf (&Query,fmt,ap);
   va_end (ap);
   if (NumBytesPrinted < 0)	// -1 if no memory or any other error
      Err_NotEnoughMemoryExit ();

   /***** Make query "SELECT COUNT(*) FROM..." *****/
   DB_QuerySELECTusingQueryStr (Query,&mysql_res,MsgError);

   /***** Get number of rows *****/
   row = mysql_fetch_row (mysql_res);
   if (sscanf (row[0],"%lu",&NumRows) != 1)
      Err_ShowErrorAndExit ("Error when counting number of rows.");

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return NumRows;
  }

Exi_Exist_t DB_QueryEXISTS (const char *MsgError,const char *fmt,...)
  {
   va_list ap;
   int NumBytesPrinted;
   char *Query;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   Exi_Exist_t Exists = Exi_DOES_NOT_EXIST;

   va_start (ap,fmt);
   NumBytesPrinted = vasprintf (&Query,fmt,ap);
   va_end (ap);
   if (NumBytesPrinted < 0)	// -1 if no memory or any other error
      Err_NotEnoughMemoryExit ();

   /***** Make query "SELECT EXISTS (...)" *****/
   if (DB_QuerySELECTusingQueryStr (Query,&mysql_res,MsgError))
     {
      row = mysql_fetch_row (mysql_res);
      if (row[0][0] == '1')
	 Exists = Exi_EXISTS;
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return Exists;
  }

/*****************************************************************************/
/******************** Make an INSERT query in database ***********************/
/*****************************************************************************/

void DB_QueryINSERT (const char *MsgError,const char *fmt,...)
  {
   va_list ap;
   int NumBytesPrinted;
   char *Query;
   int Result;

   va_start (ap,fmt);
   NumBytesPrinted = vasprintf (&Query,fmt,ap);
   va_end (ap);
   if (NumBytesPrinted < 0)	// -1 if no memory or any other error
      Err_NotEnoughMemoryExit ();

   /***** Query database and free query string pointer *****/
   Result = mysql_query (&DB_Database.mysql,Query);	// Returns 0 on success
   free (Query);
   if (Result)
      DB_ExitOnMySQLError (MsgError);
  }

/*****************************************************************************/
/** Make an INSERT query in database and return code of last inserted item ***/
/*****************************************************************************/

long DB_QueryINSERTandReturnCode (const char *MsgError,const char *fmt,...)
  {
   va_list ap;
   int NumBytesPrinted;
   char *Query;
   int Result;

   va_start (ap,fmt);
   NumBytesPrinted = vasprintf (&Query,fmt,ap);
   va_end (ap);
   if (NumBytesPrinted < 0)	// -1 if no memory or any other error
      Err_NotEnoughMemoryExit ();

   /***** Query database and free query string pointer *****/
   Result = mysql_query (&DB_Database.mysql,Query);	// Returns 0 on success
   free (Query);
   if (Result)
      DB_ExitOnMySQLError (MsgError);

   /***** Return the code of the inserted item *****/
   return (long) mysql_insert_id (&DB_Database.mysql);
  }

/*****************************************************************************/
/******************** Make an REPLACE query in database **********************/
/*****************************************************************************/

void DB_QueryREPLACE (const char *MsgError,const char *fmt,...)
  {
   va_list ap;
   int NumBytesPrinted;
   char *Query;
   int Result;

   va_start (ap,fmt);
   NumBytesPrinted = vasprintf (&Query,fmt,ap);
   va_end (ap);
   if (NumBytesPrinted < 0)	// -1 if no memory or any other error
      Err_NotEnoughMemoryExit ();

   /***** Query database and free query string pointer *****/
   Result = mysql_query (&DB_Database.mysql,Query);	// Returns 0 on success
   free (Query);
   if (Result)
      DB_ExitOnMySQLError (MsgError);
  }

/*****************************************************************************/
/******************** Make a UPDATE query from database **********************/
/*****************************************************************************/

void DB_QueryUPDATE (const char *MsgError,const char *fmt,...)
  {
   va_list ap;
   int NumBytesPrinted;
   char *Query;
   int Result;

   va_start (ap,fmt);
   NumBytesPrinted = vasprintf (&Query,fmt,ap);
   va_end (ap);
   if (NumBytesPrinted < 0)	// -1 if no memory or any other error
      Err_NotEnoughMemoryExit ();

   /***** Query database and free query string pointer *****/
   Result = mysql_query (&DB_Database.mysql,Query);	// Returns 0 on success
   free (Query);
   if (Result)
      DB_ExitOnMySQLError (MsgError);
   }

/*****************************************************************************/
/******************** Make a DELETE query from database **********************/
/*****************************************************************************/

void DB_QueryDELETE (const char *MsgError,const char *fmt,...)
  {
   va_list ap;
   int NumBytesPrinted;
   char *Query;
   int Result;

   va_start (ap,fmt);
   NumBytesPrinted = vasprintf (&Query,fmt,ap);
   va_end (ap);
   if (NumBytesPrinted < 0)	// -1 if no memory or any other error
      Err_NotEnoughMemoryExit ();

   /***** Query database and free query string pointer *****/
   Result = mysql_query (&DB_Database.mysql,Query);	// Returns 0 on success
   free (Query);
   if (Result)
      DB_ExitOnMySQLError (MsgError);
  }

/*****************************************************************************/
/**************************** Create temporary table *************************/
/*****************************************************************************/

void DB_CreateTmpTable (const char *fmt,...)
  {
   va_list ap;
   int NumBytesPrinted;
   char *Query;
   int Result;

   va_start (ap,fmt);
   NumBytesPrinted = vasprintf (&Query,fmt,ap);
   va_end (ap);
   if (NumBytesPrinted < 0)	// -1 if no memory or any other error
      Err_NotEnoughMemoryExit ();

   /***** Query database and free query string pointer *****/
   Result = mysql_query (&DB_Database.mysql,Query);	// Returns 0 on success
   free (Query);
   if (Result)
      DB_ExitOnMySQLError ("can not create temporary table");
  }

void DB_DropTmpTable (const char *Table)
  {
   DB_Query ("can not remove temporary table",
	     "DROP TEMPORARY TABLE IF EXISTS %s",Table);
  }

/*****************************************************************************/
/**************** Make other kind of query from database *********************/
/*****************************************************************************/

void DB_Query (const char *MsgError,const char *fmt,...)
  {
   va_list ap;
   int NumBytesPrinted;
   char *Query;
   int Result;

   va_start (ap,fmt);
   NumBytesPrinted = vasprintf (&Query,fmt,ap);
   va_end (ap);
   if (NumBytesPrinted < 0)	// -1 if no memory or any other error
      Err_NotEnoughMemoryExit ();

   /***** Query database and free query string pointer *****/
   Result = mysql_query (&DB_Database.mysql,Query);	// Returns 0 on success
   free (Query);
   if (Result)
      DB_ExitOnMySQLError (MsgError);
  }

/*****************************************************************************/
/********** Free structure that stores the result of a SELECT query **********/
/*****************************************************************************/

void DB_FreeMySQLResult (MYSQL_RES **mysql_res)
  {
   if (mysql_res)
      if (*mysql_res)
	{
	 mysql_free_result (*mysql_res);
	 *mysql_res = NULL;
	}
  }

/*****************************************************************************/
/*********** Abort program due to an error in the MySQL database *************/
/*****************************************************************************/

void DB_ExitOnMySQLError (const char *Message)
  {
   char BigErrorMsg[64 * 1024];

   snprintf (BigErrorMsg,sizeof (BigErrorMsg),"Database error: %s (%s).",
             Message,mysql_error (&DB_Database.mysql));
   Err_ShowErrorAndExit (BigErrorMsg);
  }

/*****************************************************************************/
/*********** Set variable that indicates there are locked tables *************/
/*****************************************************************************/

void DB_SetThereAreLockedTables (void)
  {
   DB_Database.ThereAreLockedTables = true;
  }

/*****************************************************************************/
/********** Unlock tables to make the exchange of items atomic ***************/
/*****************************************************************************/

void DB_UnlockTables (void)
  {
   if (DB_Database.ThereAreLockedTables)
     {
      DB_Database.ThereAreLockedTables = false;	// Set to false before the following unlock...
					// ...to not retry the unlock if error in unlocking
      DB_Query ("can not unlock tables",
		"UNLOCK TABLES");
     }
  }
