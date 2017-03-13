// swad_database.c: database

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2017 Antonio Cañas Vargas

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

#include <linux/stddef.h>	// For NULL
#include <mysql/mysql.h>	// To access MySQL databases
#include <stdio.h>		// For FILE,fprintf

#include "swad_config.h"
#include "swad_database.h"
#include "swad_global.h"
#include "swad_text.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/************************ Internal global variables **************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void DB_CreateTable (const char *Query);

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
   Lay_ShowAlert (Lay_INFO,Txt_Creating_database_tables_if_they_do_not_exist);
   fprintf (Gbl.F.Out,"<ol>");

   /***** Table IP_prefs *****/
/*
mysql> DESCRIBE IP_prefs;
+----------------+------------+------+-----+---------+-------+
| Field          | Type       | Null | Key | Default | Extra |
+----------------+------------+------+-----+---------+-------+
| IP             | char(15)   | NO   | PRI | NULL    |       |
| UsrCod         | int(11)    | NO   | MUL | -1      |       |
| LastChange     | datetime   | NO   | MUL | NULL    |       |
| FirstDayOfWeek | tinyint(4) | NO   |     | 0       |       |
| Theme          | char(16)   | NO   |     | NULL    |       |
| IconSet        | char(16)   | NO   |     | NULL    |       |
| Menu           | tinyint(4) | NO   |     | 0       |       |
| SideCols       | tinyint(4) | NO   |     | NULL    |       |
+----------------+------------+------+-----+---------+-------+
8 rows in set (0.01 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS IP_prefs ("
			"IP CHAR(15) NOT NULL,"		// Cns_MAX_BYTES_IP
			"UsrCod INT NOT NULL DEFAULT -1,"
			"LastChange DATETIME NOT NULL,"
			"FirstDayOfWeek TINYINT NOT NULL DEFAULT 0,"
			"Theme CHAR(16) NOT NULL,"	// The_MAX_BYTES_THEME_ID
			"IconSet CHAR(16) NOT NULL,"	// Ico_MAX_BYTES_ICON_SET_ID
			"Menu TINYINT NOT NULL DEFAULT 0,"
			"SideCols TINYINT NOT NULL,"
		   "PRIMARY KEY (IP),"
		   "INDEX(UsrCod),"
		   "INDEX(LastChange))");

   /***** Table actions *****/
/*
mysql> DESCRIBE actions;
+----------+---------------+------+-----+---------+-------+
| Field    | Type          | Null | Key | Default | Extra |
+----------+---------------+------+-----+---------+-------+
| ActCod   | int(11)       | NO   | PRI | -1      |       |
| Language | char(2)       | NO   | PRI | es      |       |
| Obsolete | enum('N','Y') | NO   |     | N       |       |
| Txt      | varchar(255)  | NO   | MUL | NULL    |       |
+----------+---------------+------+-----+---------+-------+
4 rows in set (0.00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS actions ("
			"ActCod INT NOT NULL DEFAULT -1,"
			"Language CHAR(2) NOT NULL,"
			"Obsolete ENUM('N','Y') NOT NULL DEFAULT 'N',"
			"Txt VARCHAR(255) NOT NULL,"	// Act_MAX_BYTES_ACTION_TXT
		   "UNIQUE INDEX(ActCod,Language),"
		   "INDEX(Txt))");

   /***** Table actions_MFU *****/
/*
mysql> DESCRIBE actions_MFU;
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
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS actions_MFU ("
			"UsrCod INT NOT NULL,"
			"ActCod INT NOT NULL,"
			"Score FLOAT NOT NULL,"
			"LastClick DATETIME NOT NULL,"
		   "UNIQUE INDEX(UsrCod,ActCod))");

/***** Table admin *****/
/*
mysql> DESCRIBE admin;
+--------+-------------------------------+------+-----+---------+-------+
| Field  | Type                          | Null | Key | Default | Extra |
+--------+-------------------------------+------+-----+---------+-------+
| UsrCod | int(11)                       | NO   | PRI | NULL    |       |
| Scope  | enum('Sys','Ins','Ctr','Deg') | NO   | PRI | NULL    |       |
| Cod    | int(11)                       | NO   | PRI | NULL    |       |
+--------+-------------------------------+------+-----+---------+-------+
3 rows in set (0.00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS admin ("
			"UsrCod INT NOT NULL,"
			"Scope ENUM('Sys','Ins','Ctr','Deg') NOT NULL,"
			"Cod INT NOT NULL,"
		   "UNIQUE INDEX(UsrCod,Scope,Cod),"
		   "INDEX (Scope,Cod))");

   /***** Table agendas *****/
/*
mysql> DESCRIBE agendas;
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
9 rows in set (0,00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS agendas ("
			"AgdCod INT NOT NULL AUTO_INCREMENT,"
			"UsrCod INT NOT NULL,"
			"Public ENUM('N','Y') NOT NULL DEFAULT 'N',"
			"Hidden ENUM('N','Y') NOT NULL DEFAULT 'N',"
			"StartTime DATETIME NOT NULL,"
			"EndTime DATETIME NOT NULL,"
			"Event VARCHAR(2047) NOT NULL,"		// Agd_MAX_BYTES_EVENT
			"Location VARCHAR(2047) NOT NULL,"	// Agd_MAX_BYTES_LOCATION
			"Txt TEXT NOT NULL,"
		   "UNIQUE INDEX(AgdCod),"
		   "INDEX(UsrCod,Public,Hidden))");

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
		   "UNIQUE INDEX(AnnCod,UsrCod))");

   /***** Table announcements *****/
/*
mysql> DESCRIBE announcements;
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
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS announcements ("
			"AnnCod INT NOT NULL AUTO_INCREMENT,"
			"Status TINYINT NOT NULL DEFAULT 0,"
			"Roles INT NOT NULL DEFAULT 0,"
			"Subject TEXT NOT NULL,"	// Cns_MAX_BYTES_SUBJECT
			"Content TEXT NOT NULL,"	// Cns_MAX_BYTES_TEXT
		   "UNIQUE INDEX(AnnCod),"
		   "INDEX(Status))");

   /***** Table asg_grp *****/
/*
mysql> DESCRIBE asg_grp;
+--------+---------+------+-----+---------+-------+
| Field  | Type    | Null | Key | Default | Extra |
+--------+---------+------+-----+---------+-------+
| AsgCod | int(11) | NO   | PRI | NULL    |       |
| GrpCod | int(11) | NO   | PRI | NULL    |       |
+--------+---------+------+-----+---------+-------+
2 rows in set (0.00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS asg_grp ("
			"AsgCod INT NOT NULL,"
			"GrpCod INT NOT NULL,"
		   "UNIQUE INDEX(AsgCod,GrpCod))");

   /***** Table assignments *****/
/*
mysql> DESCRIBE assignments;
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
10 rows in set (0,00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS assignments ("
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
		   "INDEX(CrsCod,Hidden))");

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
9 rows in set (0,00 sec)
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
		   "INDEX(CrsCod,Hidden))");

   /***** Table att_grp *****/
/*
mysql> DESCRIBE att_grp;
+--------+---------+------+-----+---------+-------+
| Field  | Type    | Null | Key | Default | Extra |
+--------+---------+------+-----+---------+-------+
| AttCod | int(11) | NO   | PRI | NULL    |       |
| GrpCod | int(11) | NO   | PRI | NULL    |       |
+--------+---------+------+-----+---------+-------+
2 rows in set (0.00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS att_grp ("
			"AttCod INT NOT NULL,"
			"GrpCod INT NOT NULL,"
		   "UNIQUE INDEX(AttCod,GrpCod))");

   /***** Table att_usr *****/
/*
mysql> DESCRIBE att_usr;
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
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS att_usr ("
			"AttCod INT NOT NULL,"
			"UsrCod INT NOT NULL,"
			"Present ENUM('N','Y') NOT NULL DEFAULT 'Y',"
			"CommentStd TEXT NOT NULL,"	// Cns_MAX_BYTES_TEXT
			"CommentTch TEXT NOT NULL,"	// Cns_MAX_BYTES_TEXT
		   "UNIQUE INDEX(AttCod,UsrCod),"
		   "INDEX(UsrCod))");

   /***** Table banners *****/
/*
mysql> DESCRIBE banners;
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
6 rows in set (0,00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS banners ("
			"BanCod INT NOT NULL AUTO_INCREMENT,"
			"Hidden ENUM('N','Y') NOT NULL DEFAULT 'N',"
			"ShortName VARCHAR(511) NOT NULL,"	// Ban_MAX_BYTES_SHRT_NAME
			"FullName VARCHAR(2047) NOT NULL,"	// Ban_MAX_BYTES_FULL_NAME
			"Img VARCHAR(255) NOT NULL,"		// Ban_MAX_BYTES_IMAGE
			"WWW VARCHAR(255) NOT NULL,"		// Cns_MAX_BYTES_WWW
		   "UNIQUE INDEX(BanCod),"
		   "INDEX(Hidden))");

   /***** Table birthdays_today *****/
   // Stores birthdays already congratulated today
/*
mysql> DESCRIBE birthdays_today;
+--------+---------+------+-----+---------+-------+
| Field  | Type    | Null | Key | Default | Extra |
+--------+---------+------+-----+---------+-------+
| UsrCod | int(11) | NO   | PRI | NULL    |       |
| Today  | date    | NO   | MUL | NULL    |       |
+--------+---------+------+-----+---------+-------+
2 rows in set (0.09 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS birthdays_today ("
			"UsrCod INT NOT NULL,"
			"Today DATE NOT NULL,"
		   "UNIQUE INDEX(UsrCod),"
		   "INDEX(Today))");

   /***** Table centres *****/
/*
mysql> DESCRIBE centres;
+------------------+---------------+------+-----+---------+----------------+
| Field            | Type          | Null | Key | Default | Extra          |
+------------------+---------------+------+-----+---------+----------------+
| CtrCod           | int(11)       | NO   | PRI | NULL    | auto_increment |
| InsCod           | int(11)       | NO   | MUL | NULL    |                |
| PlcCod           | int(11)       | NO   | MUL | -1      |                |
| Status           | tinyint(4)    | NO   | MUL | 0       |                |
| RequesterUsrCod  | int(11)       | NO   |     | -1      |                |
| ShortName        | varchar(511)  | NO   |     | NULL    |                |
| FullName         | varchar(2047) | NO   |     | NULL    |                |
| WWW              | varchar(255)  | NO   |     | NULL    |                |
| PhotoAttribution | text          | NO   |     | NULL    |                |
+------------------+---------------+------+-----+---------+----------------+
9 rows in set (0,00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS centres ("
			"CtrCod INT NOT NULL AUTO_INCREMENT,"
			"InsCod INT NOT NULL,"
			"PlcCod INT NOT NULL,"
			"Status TINYINT NOT NULL DEFAULT 0,"
			"RequesterUsrCod INT NOT NULL DEFAULT -1,"
			"ShortName VARCHAR(511) COLLATE latin1_spanish_ci NOT NULL,"	// Hie_MAX_BYTES_SHRT_NAME
			"FullName VARCHAR(2047) COLLATE latin1_spanish_ci NOT NULL,"	// Hie_MAX_BYTES_FULL_NAME
			"WWW VARCHAR(255) NOT NULL,"					// Cns_MAX_BYTES_WWW
			"PhotoAttribution TEXT NOT NULL,"				// Img_MAX_BYTES_ATTRIBUTION
		   "UNIQUE INDEX(CtrCod),"
		   "INDEX(InsCod),"
		   "INDEX(PlcCod),"
		   "INDEX(Status))");

   /***** Table chat *****/
/*
mysql> DESCRIBE chat;
+----------+-------------+------+-----+---------+-------+
| Field    | Type        | Null | Key | Default | Extra |
+----------+-------------+------+-----+---------+-------+
| RoomCode | varchar(16) | NO   | PRI | NULL    |       |
| NumUsrs  | int(11)     | NO   |     | NULL    |       |
+----------+-------------+------+-----+---------+-------+
2 rows in set (0,00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS chat ("\
			"RoomCode VARCHAR(16) NOT NULL,"	// Cht_MAX_BYTES_ROOM_CODE
			"NumUsrs INT NOT NULL,"
		   "UNIQUE INDEX(RoomCode))");

   /***** Table clicks_without_photo *****/
/*
mysql> DESCRIBE clicks_without_photo;
+-----------+---------+------+-----+---------+-------+
| Field     | Type    | Null | Key | Default | Extra |
+-----------+---------+------+-----+---------+-------+
| UsrCod    | int(11) | NO   | PRI | NULL    |       |
| NumClicks | int(11) | NO   |     | NULL    |       |
+-----------+---------+------+-----+---------+-------+
2 rows in set (0.00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS clicks_without_photo ("
			"UsrCod INT NOT NULL,"
			"NumClicks INT NOT NULL,"
		   "UNIQUE INDEX(UsrCod))");

   /***** Table clipboard *****/
/*
mysql> DESCRIBE clipboard;
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
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS clipboard ("
			"UsrCod INT NOT NULL,"
			"FileBrowser TINYINT NOT NULL,"
			"Cod INT NOT NULL DEFAULT -1,"
			"WorksUsrCod INT NOT NULL,"
			"FileType TINYINT NOT NULL DEFAULT 0,"
			"Path TEXT COLLATE latin1_bin NOT NULL,"	// PATH_MAX
			"CopyTime TIMESTAMP,"
		   "UNIQUE INDEX(UsrCod),"
		   "INDEX(FileBrowser,Cod),"
		   "INDEX(WorksUsrCod))");

   /***** Table connected *****/
/*
mysql> DESCRIBE connected;
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
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS connected ("
			"UsrCod INT NOT NULL,"
			"RoleInLastCrs TINYINT NOT NULL DEFAULT 0,"
			"LastCrsCod INT NOT NULL DEFAULT -1,"
			"LastTime DATETIME NOT NULL,"
		   "UNIQUE INDEX(UsrCod),"
		   "INDEX(RoleInLastCrs),"
		   "INDEX(LastCrsCod))");

   /***** Table countries *****/
/*
mysql> DESCRIBE countries;
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
| WWW_ca         | varchar(255) | NO   |     | NULL    |       |
| WWW_de         | varchar(255) | NO   |     | NULL    |       |
| WWW_en         | varchar(255) | NO   |     | NULL    |       |
| WWW_es         | varchar(255) | NO   |     | NULL    |       |
| WWW_fr         | varchar(255) | NO   |     | NULL    |       |
| WWW_gn         | varchar(255) | NO   |     | NULL    |       |
| WWW_it         | varchar(255) | NO   |     | NULL    |       |
| WWW_pl         | varchar(255) | NO   |     | NULL    |       |
| WWW_pt         | varchar(255) | NO   |     | NULL    |       |
+----------------+--------------+------+-----+---------+-------+
21 rows in set (0,00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS countries ("
			"CtyCod INT NOT NULL,"
			"Alpha2 CHAR(2) NOT NULL,"
			"MapAttribution TEXT NOT NULL,"		// Img_MAX_BYTES_ATTRIBUTION
			"Name_ca VARCHAR(767) NOT NULL,"	// Cty_MAX_BYTES_NAME
			"Name_de VARCHAR(767) NOT NULL,"	// Cty_MAX_BYTES_NAME
			"Name_en VARCHAR(767) NOT NULL,"	// Cty_MAX_BYTES_NAME
			"Name_es VARCHAR(767) NOT NULL,"	// Cty_MAX_BYTES_NAME
			"Name_fr VARCHAR(767) NOT NULL,"	// Cty_MAX_BYTES_NAME
			"Name_gn VARCHAR(767) NOT NULL,"	// Cty_MAX_BYTES_NAME
			"Name_it VARCHAR(767) NOT NULL,"	// Cty_MAX_BYTES_NAME
			"Name_pl VARCHAR(767) NOT NULL,"	// Cty_MAX_BYTES_NAME
			"Name_pt VARCHAR(767) NOT NULL,"	// Cty_MAX_BYTES_NAME
			"WWW_ca VARCHAR(255) NOT NULL,"		// Cns_MAX_BYTES_WWW
			"WWW_de VARCHAR(255) NOT NULL,"		// Cns_MAX_BYTES_WWW
			"WWW_en VARCHAR(255) NOT NULL,"		// Cns_MAX_BYTES_WWW
			"WWW_es VARCHAR(255) NOT NULL,"		// Cns_MAX_BYTES_WWW
			"WWW_fr VARCHAR(255) NOT NULL,"		// Cns_MAX_BYTES_WWW
			"WWW_gn VARCHAR(255) NOT NULL,"		// Cns_MAX_BYTES_WWW
			"WWW_it VARCHAR(255) NOT NULL,"		// Cns_MAX_BYTES_WWW
			"WWW_pl VARCHAR(255) NOT NULL,"		// Cns_MAX_BYTES_WWW
			"WWW_pt VARCHAR(255) NOT NULL,"		// Cns_MAX_BYTES_WWW
		   "UNIQUE INDEX(CtyCod),UNIQUE INDEX(Alpha2),"
		   "INDEX(Name_ca),"
		   "INDEX(Name_de),"
		   "INDEX(Name_en),"
		   "INDEX(Name_es),"
		   "INDEX(Name_fr),"
		   "INDEX(Name_gn),"
		   "INDEX(Name_it),"
		   "INDEX(Name_pl),"
		   "INDEX(Name_pt))");	// ISO 3166-1 country codes

   /***** Table courses *****/
/*
mysql> DESCRIBE courses;
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
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS courses ("
			"CrsCod INT NOT NULL AUTO_INCREMENT,"
			"DegCod INT NOT NULL DEFAULT -1,"
			"Year TINYINT NOT NULL DEFAULT 0,"
			"InsCrsCod CHAR(7) NOT NULL,"
			"Status TINYINT NOT NULL DEFAULT 0,"
			"RequesterUsrCod INT NOT NULL DEFAULT -1,"
			"ShortName VARCHAR(511) COLLATE latin1_spanish_ci NOT NULL,"	// Hie_MAX_BYTES_SHRT_NAME
			"FullName VARCHAR(2047) COLLATE latin1_spanish_ci NOT NULL,"	// Hie_MAX_BYTES_FULL_NAME
			"NumIndicators INT NOT NULL DEFAULT -1,"
		   "UNIQUE INDEX(CrsCod),"
		   "INDEX(DegCod,Year),"
		   "INDEX(Status))");

   /***** Table crs_grp *****/
/*
mysql> DESCRIBE crs_grp;
+-------------+---------------+------+-----+---------+----------------+
| Field       | Type          | Null | Key | Default | Extra          |
+-------------+---------------+------+-----+---------+----------------+
| GrpCod      | int(11)       | NO   | PRI | NULL    | auto_increment |
| GrpTypCod   | int(11)       | NO   | MUL | NULL    |                |
| GrpName     | varchar(2047) | NO   |     | NULL    |                |
| MaxStudents | int(11)       | NO   |     | NULL    |                |
| Open        | enum('N','Y') | NO   |     | N       |                |
| FileZones   | enum('N','Y') | NO   |     | N       |                |
+-------------+---------------+------+-----+---------+----------------+
6 rows in set (0,00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS crs_grp ("
			"GrpCod INT NOT NULL AUTO_INCREMENT,"
			"GrpTypCod INT NOT NULL,"
			"GrpName VARCHAR(2047) NOT NULL,"	// Grp_MAX_BYTES_GROUP_NAME
			"MaxStudents INT NOT NULL,"
			"Open ENUM('N','Y') NOT NULL DEFAULT 'N',"
			"FileZones ENUM('N','Y') NOT NULL DEFAULT 'N',"
		   "UNIQUE INDEX(GrpCod),"
		   "INDEX(GrpTypCod))");

   /***** Table crs_grp_types *****/
/*
mysql> DESCRIBE crs_grp_types;
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
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS crs_grp_types ("
			"GrpTypCod INT NOT NULL AUTO_INCREMENT,"
			"CrsCod INT NOT NULL DEFAULT -1,"
			"GrpTypName VARCHAR(2047) NOT NULL,"	// Grp_MAX_BYTES_GROUP_TYPE_NAME
			"Mandatory ENUM('N','Y') NOT NULL,"
			"Multiple ENUM('N','Y') NOT NULL,"
			"MustBeOpened ENUM('N','Y') NOT NULL DEFAULT 'N',"
			"OpenTime DATETIME NOT NULL,"
		   "UNIQUE INDEX(GrpTypCod),"
		   "INDEX(CrsCod))");

   /***** Table crs_grp_usr *****/
/*
mysql> DESCRIBE crs_grp_usr;
+--------+---------+------+-----+---------+-------+
| Field  | Type    | Null | Key | Default | Extra |
+--------+---------+------+-----+---------+-------+
| GrpCod | int(11) | NO   | PRI | NULL    |       |
| UsrCod | int(11) | NO   | PRI | NULL    |       |
+--------+---------+------+-----+---------+-------+
2 rows in set (0.00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS crs_grp_usr ("
			"GrpCod INT NOT NULL,"
			"UsrCod INT NOT NULL,"
		   "UNIQUE INDEX(GrpCod,UsrCod),"
		   "INDEX(GrpCod),"
		   "INDEX(UsrCod))");

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
			"InfoType ENUM('intro','description','theory','practices','bibliography','FAQ','links','assessment') NOT NULL,"
		   "UNIQUE INDEX(UsrCod,CrsCod,InfoType))");

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
			"InfoType ENUM('intro','description','theory','practices','bibliography','FAQ','links','assessment') NOT NULL,"
			"InfoSrc ENUM('none','editor','plain_text','rich_text','page','URL') NOT NULL,"
			"MustBeRead ENUM('N','Y') NOT NULL DEFAULT 'N',"
		   "UNIQUE INDEX(CrsCod,InfoType))");

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
4 rows in set (0.01 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS crs_info_txt ("
			"CrsCod INT NOT NULL DEFAULT -1,"
			"InfoType ENUM('intro','description','theory','practices','bibliography','FAQ','links','assessment') NOT NULL,"
			"InfoTxtHTML LONGTEXT NOT NULL,"
			"InfoTxtMD LONGTEXT NOT NULL,"
		   "UNIQUE INDEX(CrsCod,InfoType))");

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
		   "INDEX(LastTime))");

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
5 rows in set (0,00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS crs_record_fields ("
			"FieldCod INT NOT NULL AUTO_INCREMENT,"
			"CrsCod INT NOT NULL DEFAULT -1,"
			"FieldName VARCHAR(2047) NOT NULL,"	// Rec_MAX_BYTES_NAME_FIELD
			"NumLines INT NOT NULL,"
			"Visibility TINYINT NOT NULL,"
		   "UNIQUE INDEX(FieldCod),"
		   "INDEX(CrsCod))");

   /***** Table crs_records *****/
/*
mysql> DESCRIBE crs_records;
+----------+---------+------+-----+---------+-------+
| Field    | Type    | Null | Key | Default | Extra |
+----------+---------+------+-----+---------+-------+
| UsrCod   | int(11) | NO   | PRI | NULL    |       |
| FieldCod | int(11) | NO   | PRI | NULL    |       |
| Txt      | text    | NO   |     | NULL    |       |
+----------+---------+------+-----+---------+-------+
3 rows in set (0.00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS crs_records ("
			"FieldCod INT NOT NULL,"
			"UsrCod INT NOT NULL,"
			"Txt TEXT NOT NULL,"	// Cns_MAX_BYTES_TEXT
		   "UNIQUE INDEX(FieldCod,UsrCod))");

   /***** Table crs_usr *****/
/*
mysql> DESCRIBE crs_usr;
+----------------+---------------------------+------+-----+------------+-------+
| Field          | Type                      | Null | Key | Default    | Extra |
+----------------+---------------------------+------+-----+------------+-------+
| CrsCod         | int(11)                   | NO   | PRI | -1         |       |
| UsrCod         | int(11)                   | NO   | PRI | NULL       |       |
| Role           | tinyint(4)                | NO   | PRI | 0          |       |
| Accepted       | enum('N','Y')             | NO   |     | N          |       |
| LastDowGrpCod  | int(11)                   | NO   |     | -1         |       |
| LastComGrpCod  | int(11)                   | NO   |     | -1         |       |
| LastAssGrpCod  | int(11)                   | NO   |     | -1         |       |
| NumAccTst      | int(11)                   | NO   |     | 0          |       |
| LastAccTst     | datetime                  | NO   |     | NULL       |       |
| NumQstsLastTst | int(11)                   | NO   |     | 0          |       |
| UsrListType    | enum('classphoto','list') | NO   |     | classphoto |       |
| ColsClassPhoto | tinyint(4)                | NO   |     | NULL       |       |
| ListWithPhotos | enum('N','Y')             | NO   |     | Y          |       |
+----------------+---------------------------+------+-----+------------+-------+
13 rows in set (0.00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS crs_usr ("
			"CrsCod INT NOT NULL DEFAULT -1,"
			"UsrCod INT NOT NULL,"
			"Role TINYINT NOT NULL DEFAULT 0,"
			"Accepted ENUM('N','Y') NOT NULL DEFAULT 'N',"
			"LastDowGrpCod INT NOT NULL DEFAULT -1,"
			"LastComGrpCod INT NOT NULL DEFAULT -1,"
			"LastAssGrpCod INT NOT NULL DEFAULT -1,"
			"NumAccTst INT NOT NULL DEFAULT 0,"
			"LastAccTst DATETIME NOT NULL,"
			"NumQstsLastTst INT NOT NULL DEFAULT 0,"
			"UsrListType ENUM('classphoto','list') NOT NULL DEFAULT 'classphoto',"
			"ColsClassPhoto TINYINT NOT NULL,"
			"ListWithPhotos ENUM('N','Y') NOT NULL DEFAULT 'Y',"
		   "UNIQUE INDEX(CrsCod,UsrCod,Role),"
		   "UNIQUE INDEX(UsrCod,CrsCod,Role),"
		   "INDEX(CrsCod,Role),"
		   "INDEX(UsrCod,Role))");

   /***** Table crs_usr_requests *****/
/*
mysql> DESCRIBE crs_usr_requests;
+-------------+------------+------+-----+---------+----------------+
| Field       | Type       | Null | Key | Default | Extra          |
+-------------+------------+------+-----+---------+----------------+
| ReqCod      | int(11)    | NO   | PRI | NULL    | auto_increment |
| CrsCod      | int(11)    | NO   | MUL | -1      |                |
| UsrCod      | int(11)    | NO   | MUL | NULL    |                |
| Role        | tinyint(4) | NO   |     | 0       |                |
| RequestTime | datetime   | NO   |     | NULL    |                |
+-------------+------------+------+-----+---------+----------------+
5 rows in set (0.01 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS crs_usr_requests ("
			"ReqCod INT NOT NULL AUTO_INCREMENT,"
			"CrsCod INT NOT NULL DEFAULT -1,"
			"UsrCod INT NOT NULL,"
			"Role TINYINT NOT NULL DEFAULT 0,"
			"RequestTime DATETIME NOT NULL,"
		   "UNIQUE INDEX(ReqCod),"
		   "UNIQUE INDEX(CrsCod,UsrCod),"
		   "INDEX(UsrCod))");

   /***** Table deg_types *****/
/*
mysql> DESCRIBE deg_types;
+------------+--------------+------+-----+---------+----------------+
| Field      | Type         | Null | Key | Default | Extra          |
+------------+--------------+------+-----+---------+----------------+
| DegTypCod  | int(11)      | NO   | PRI | NULL    | auto_increment |
| DegTypName | varchar(511) | NO   |     | NULL    |                |
+------------+--------------+------+-----+---------+----------------+
2 rows in set (0,00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS deg_types ("
			"DegTypCod INT NOT NULL AUTO_INCREMENT,"
			"DegTypName VARCHAR(511) NOT NULL,"	// Deg_MAX_BYTES_DEGREE_TYPE_NAME
		   "UNIQUE INDEX(DegTypCod))");

   /***** Table degrees *****/
/*
mysql> DESCRIBE degrees;
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
8 rows in set (0,00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS degrees ("
			"DegCod INT NOT NULL AUTO_INCREMENT,"
			"CtrCod INT NOT NULL,"
			"DegTypCod INT NOT NULL,"
			"Status TINYINT NOT NULL DEFAULT 0,"
			"RequesterUsrCod INT NOT NULL DEFAULT -1,"
			"ShortName VARCHAR(511) COLLATE latin1_spanish_ci NOT NULL,"	// Hie_MAX_BYTES_SHRT_NAME
			"FullName VARCHAR(2047) COLLATE latin1_spanish_ci NOT NULL,"	// Hie_MAX_BYTES_FULL_NAME
			"WWW VARCHAR(255) NOT NULL,"					// Cns_MAX_BYTES_WWW
		   "UNIQUE INDEX(DegCod),"
		   "INDEX(CtrCod),"
		   "INDEX(DegTypCod),"
		   "INDEX(Status))");

   /***** Table departments *****/
/*
mysql> DESCRIBE departments;
+-----------+---------------+------+-----+---------+----------------+
| Field     | Type          | Null | Key | Default | Extra          |
+-----------+---------------+------+-----+---------+----------------+
| DptCod    | int(11)       | NO   | PRI | NULL    | auto_increment |
| InsCod    | int(11)       | NO   | MUL | NULL    |                |
| ShortName | varchar(511)  | NO   |     | NULL    |                |
| FullName  | varchar(2047) | NO   |     | NULL    |                |
| WWW       | varchar(255)  | NO   |     | NULL    |                |
+-----------+---------------+------+-----+---------+----------------+
5 rows in set (0,00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS departments ("
			"DptCod INT NOT NULL AUTO_INCREMENT,"
			"InsCod INT NOT NULL,"
			"ShortName VARCHAR(511) NOT NULL,"	// Hie_MAX_BYTES_SHRT_NAME
			"FullName VARCHAR(2047) NOT NULL,"	// Hie_MAX_BYTES_FULL_NAME
			"WWW VARCHAR(255) NOT NULL,"		// Cns_MAX_BYTES_WWW
		   "UNIQUE INDEX(DptCod),"
		   "INDEX(InsCod))");

   /***** Table exam_announcements *****/
/*
mysql> DESCRIBE exam_announcements;
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
17 rows in set (0,00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS exam_announcements ("
			"ExaCod INT NOT NULL AUTO_INCREMENT,"
			"CrsCod INT NOT NULL DEFAULT -1,"
			"Status TINYINT NOT NULL DEFAULT 0,"
			"NumNotif INT NOT NULL DEFAULT 0,"
			"CrsFullName VARCHAR(2047) NOT NULL,"	// Hie_MAX_BYTES_FULL_NAME
			"Year TINYINT NOT NULL,"
			"ExamSession VARCHAR(2047) NOT NULL,"	// Exa_MAX_BYTES_SESSION
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
		   "INDEX(Status))");

   /***** Table expanded_folders *****/
/*
mysql> DESCRIBE expanded_folders;
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
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS expanded_folders ("
			"UsrCod INT NOT NULL,"
			"FileBrowser TINYINT NOT NULL,"
			"Cod INT NOT NULL DEFAULT -1,"
			"WorksUsrCod INT NOT NULL,"
			"Path TEXT COLLATE latin1_bin NOT NULL,"	// PATH_MAX
			"ClickTime DATETIME NOT NULL,"
		   "INDEX(UsrCod,FileBrowser,Cod),"
		   "INDEX(FileBrowser,Cod),"
		   "INDEX(WorksUsrCod))");

   /***** Table file_browser_last *****/
/*
mysql> DESCRIBE file_browser_last;
+-------------+------------+------+-----+---------+-------+
| Field       | Type       | Null | Key | Default | Extra |
+-------------+------------+------+-----+---------+-------+
| UsrCod      | int(11)    | NO   | PRI | NULL    |       |
| FileBrowser | tinyint(4) | NO   | PRI | NULL    |       |
| Cod         | int(11)    | NO   | PRI | -1      |       |
| LastClick   | datetime   | NO   |     | NULL    |       |
+-------------+------------+------+-----+---------+-------+
4 rows in set (0.01 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS file_browser_last ("
			"UsrCod INT NOT NULL,"
			"FileBrowser TINYINT NOT NULL,"
			"Cod INT NOT NULL DEFAULT -1,"
		   "LastClick DATETIME NOT NULL,"
		   "UNIQUE INDEX(UsrCod,FileBrowser,Cod))");

   /***** Table file_browser_size *****/
/*
mysql> DESCRIBE file_browser_size;
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
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS file_browser_size ("
			"FileBrowser TINYINT NOT NULL,"
			"Cod INT NOT NULL DEFAULT -1,"
			"ZoneUsrCod INT NOT NULL DEFAULT -1,"
			"NumLevels INT NOT NULL,"
			"NumFolders INT NOT NULL,"
			"NumFiles INT NOT NULL,"
			"TotalSize BIGINT NOT NULL,"
		   "UNIQUE INDEX(FileBrowser,Cod,ZoneUsrCod),"
		   "INDEX(ZoneUsrCod))");

   /***** Table file_view *****/
/*
mysql> DESCRIBE file_view;
+----------+---------+------+-----+---------+-------+
| Field    | Type    | Null | Key | Default | Extra |
+----------+---------+------+-----+---------+-------+
| FilCod   | int(11) | NO   | PRI | NULL    |       |
| UsrCod   | int(11) | NO   | PRI | NULL    |       |
| NumViews | int(11) | NO   |     | 0       |       |
+----------+---------+------+-----+---------+-------+
3 rows in set (0.00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS file_view ("
			"FilCod INT NOT NULL,"
			"UsrCod INT NOT NULL,"
			"NumViews INT NOT NULL DEFAULT 0,"
		   "UNIQUE INDEX(FilCod,UsrCod),"
		   "INDEX(UsrCod))");

   /***** Table files *****/
/*
mysql> DESCRIBE files;
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
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS files ("
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
		   "INDEX(PublisherUsrCod))");

   /***** Table forum_disabled_post *****/
/*
mysql> DESCRIBE forum_disabled_post;
+-------------+----------+------+-----+---------+-------+
| Field       | Type     | Null | Key | Default | Extra |
+-------------+----------+------+-----+---------+-------+
| PstCod      | int(11)  | NO   | PRI | NULL    |       |
| UsrCod      | int(11)  | NO   |     | NULL    |       |
| DisableTime | datetime | NO   |     | NULL    |       |
+-------------+----------+------+-----+---------+-------+
3 rows in set (0.00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS forum_disabled_post ("
			"PstCod INT NOT NULL,"
			"UsrCod INT NOT NULL,"
			"DisableTime DATETIME NOT NULL,"
		   "UNIQUE INDEX(PstCod))");

   /***** Table forum_post *****/
/*
mysql> DESCRIBE forum_post;
+------------+---------------+------+-----+---------+----------------+
| Field      | Type          | Null | Key | Default | Extra          |
+------------+---------------+------+-----+---------+----------------+
| PstCod     | int(11)       | NO   | PRI | NULL    | auto_increment |
| ThrCod     | int(11)       | NO   | MUL | NULL    |                |
| UsrCod     | int(11)       | NO   | MUL | NULL    |                |
| CreatTime  | datetime      | NO   | MUL | NULL    |                |
| ModifTime  | datetime      | NO   | MUL | NULL    |                |
| NumNotif   | int(11)       | NO   |     | 0       |                |
| Subject    | text          | NO   |     | NULL    |                |
| Content    | longtext      | NO   |     | NULL    |                |
| ImageName  | varchar(43)   | NO   |     | NULL    |                |
| ImageTitle | varchar(2047) | NO   |     | NULL    |                |
| ImageURL   | varchar(255)  | NO   |     | NULL    |                |
+------------+---------------+------+-----+---------+----------------+
11 rows in set (0,00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS forum_post ("
			"PstCod INT NOT NULL AUTO_INCREMENT,"
			"ThrCod INT NOT NULL,"
			"UsrCod INT NOT NULL,"
			"CreatTime DATETIME NOT NULL,"
			"ModifTime DATETIME NOT NULL,"
			"NumNotif INT NOT NULL DEFAULT 0,"
			"Subject TEXT NOT NULL,"		// Cns_MAX_BYTES_SUBJECT
			"Content LONGTEXT NOT NULL,"		// Cns_MAX_BYTES_LONG_TEXT
			"ImageName VARCHAR(43) NOT NULL,"	// Cry_BYTES_ENCRYPTED_STR_SHA256_BASE64
			"ImageTitle VARCHAR(2047) NOT NULL,"	// Img_MAX_BYTES_TITLE
			"ImageURL VARCHAR(255) NOT NULL,"	// Cns_MAX_BYTES_WWW
		   "UNIQUE INDEX(PstCod),"
		   "INDEX(ThrCod),"
		   "INDEX(UsrCod),"
		   "INDEX(CreatTime),"
		   "INDEX(ModifTime))");

   /***** Table forum_thr_clip *****/
/*
mysql> DESCRIBE forum_thr_clip;
+------------+-----------+------+-----+-------------------+-------+
| Field      | Type      | Null | Key | Default           | Extra |
+------------+-----------+------+-----+-------------------+-------+
| ThrCod     | int(11)   | NO   | PRI | NULL              |       |
| UsrCod     | int(11)   | NO   | UNI | NULL              |       |
| TimeInsert | timestamp | NO   |     | CURRENT_TIMESTAMP |       |
+------------+-----------+------+-----+-------------------+-------+
3 rows in set (0.00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS forum_thr_clip ("
			"ThrCod INT NOT NULL,"
			"UsrCod INT NOT NULL,"
			"TimeInsert TIMESTAMP NOT NULL,"
		   "UNIQUE INDEX(ThrCod),"
		   "UNIQUE INDEX(UsrCod))");

   /***** Table forum_thr_read *****/
/*
mysql> DESCRIBE forum_thr_read;
+----------+----------+------+-----+---------------------+-------+
| Field    | Type     | Null | Key | Default             | Extra |
+----------+----------+------+-----+---------------------+-------+
| ThrCod   | int(11)  | NO   | PRI | 0                   |       |
| UsrCod   | int(11)  | NO   | PRI | NULL                |       |
| ReadTime | datetime | NO   |     | 0000-00-00 00:00:00 |       |
+----------+----------+------+-----+---------------------+-------+
3 rows in set (0.00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS forum_thr_read ("
			"ThrCod INT NOT NULL,"
			"UsrCod INT NOT NULL,"
			"ReadTime DATETIME NOT NULL,"
		   "UNIQUE INDEX(ThrCod,UsrCod))");

   /***** Table forum_thread *****/
/*
mysql> DESCRIBE forum_thread;
+-------------+------------+------+-----+---------+----------------+
| Field       | Type       | Null | Key | Default | Extra          |
+-------------+------------+------+-----+---------+----------------+
| ThrCod      | int(11)    | NO   | PRI | NULL    | auto_increment |
| ForumType   | tinyint(4) | NO   | MUL | 0       |                |
| Location    | int(11)    | NO   | MUL | -1      |                |
| FirstPstCod | int(11)    | NO   | UNI | NULL    |                |
| LastPstCod  | int(11)    | NO   | UNI | NULL    |                |
+-------------+------------+------+-----+---------+----------------+
5 rows in set (0.00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS forum_thread ("
			"ThrCod INT NOT NULL AUTO_INCREMENT,"
			"ForumType TINYINT NOT NULL,"
			"Location INT NOT NULL DEFAULT -1,"
			"FirstPstCod INT NOT NULL,"
			"LastPstCod INT NOT NULL,"
		   "UNIQUE INDEX(ThrCod),"
		   "INDEX(ForumType),"
		   "INDEX(Location),"
		   "UNIQUE INDEX(FirstPstCod),"
		   "UNIQUE INDEX(LastPstCod))");

   /***** Table hidden_params *****/
/*
mysql> DESCRIBE hidden_params;
+------------+--------------+------+-----+---------+-------+
| Field      | Type         | Null | Key | Default | Extra |
+------------+--------------+------+-----+---------+-------+
| SessionId  | char(43)     | NO   | MUL | NULL    |       |
| Action     | int(11)      | NO   |     | NULL    |       |
| ParamName  | varchar(255) | NO   |     | NULL    |       |
| ParamValue | text         | NO   |     | NULL    |       |
+------------+--------------+------+-----+---------+-------+
4 rows in set (0.00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS hidden_params ("
			"SessionId CHAR(43) NOT NULL,"
			"Action INT NOT NULL,"
			"ParamName VARCHAR(255) NOT NULL,"
			"ParamValue TEXT NOT NULL,"
		   "INDEX(SessionId,Action))");

   /***** Table holidays *****/
/*
mysql> DESCRIBE holidays;
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
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS holidays ("
			"HldCod INT NOT NULL AUTO_INCREMENT,"
			"InsCod INT NOT NULL,"
			"PlcCod INT NOT NULL DEFAULT -1,"
			"HldTyp TINYINT NOT NULL,"
			"StartDate DATE NOT NULL,"
			"EndDate DATE NOT NULL,"
			"Name VARCHAR(2047) NOT NULL,"	// Hld_MAX_BYTES_HOLIDAY_NAME
		   "UNIQUE INDEX (HldCod),"
		   "INDEX(InsCod),"
		   "INDEX(PlcCod))");

   /***** Table institutions *****/
/*
mysql> DESCRIBE institutions;
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
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS institutions ("
			"InsCod INT NOT NULL AUTO_INCREMENT,"
			"CtyCod INT NOT NULL,"
			"Status TINYINT NOT NULL DEFAULT 0,"
			"RequesterUsrCod INT NOT NULL DEFAULT -1,"
			"ShortName VARCHAR(511) COLLATE latin1_spanish_ci NOT NULL,"	// Hie_MAX_BYTES_SHRT_NAME
			"FullName VARCHAR(2047) COLLATE latin1_spanish_ci NOT NULL,"	// Hie_MAX_BYTES_FULL_NAME
			"WWW VARCHAR(255) NOT NULL,"					// Cns_MAX_BYTES_WWW
		   "UNIQUE INDEX(InsCod),"
		   "INDEX(CtyCod),"
		   "INDEX(Status))");

   /***** Table links *****/
/*
mysql> DESCRIBE links;
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
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS links ("
			"LnkCod INT NOT NULL AUTO_INCREMENT,"
			"ShortName VARCHAR(511) NOT NULL,"	// Lnk_MAX_BYTES_LINK_SHRT_NAME
			"FullName VARCHAR(2047) NOT NULL,"	// Lnk_MAX_BYTES_LINK_FULL_NAME
			"WWW VARCHAR(255) NOT NULL,"		// Cns_MAX_BYTES_WWW
		   "UNIQUE INDEX(LnkCod))");

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
		   "INDEX(BanCod))");

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
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS log_comments ("
			"LogCod INT NOT NULL,"
			"Comments TEXT NOT NULL,"
		   "UNIQUE INDEX(LogCod))");

   /***** Table log_full *****/
/*
mysql> DESCRIBE log_full;
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
| ClickTime      | datetime   | NO   | MUL | NULL    |                |
| TimeToGenerate | int(11)    | NO   |     | NULL    |                |
| TimeToSend     | int(11)    | NO   |     | NULL    |                |
| IP             | char(15)   | NO   |     | NULL    |                |
+----------------+------------+------+-----+---------+----------------+
13 rows in set (0.01 sec)
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
			"IP CHAR(15) NOT NULL,"	// Cns_MAX_CHARS_IP
		   "UNIQUE INDEX(LogCod),"
		   "INDEX(ActCod),"
		   "INDEX(CtyCod),"
		   "INDEX(InsCod),"
		   "INDEX(CtrCod),"
		   "INDEX(DegCod),"
		   "INDEX(CrsCod),"
		   "INDEX(UsrCod),"
		   "INDEX(ClickTime,Role))");

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
			"IP CHAR(15) NOT NULL,"	// Cns_MAX_CHARS_IP
		   "UNIQUE INDEX(LogCod),"
		   "INDEX(ActCod),"
		   "INDEX(CtyCod),"
		   "INDEX(InsCod),"
		   "INDEX(CtrCod),"
		   "INDEX(DegCod),"
		   "INDEX(CrsCod),"
		   "INDEX(UsrCod),"
		   "INDEX(ClickTime,Role))");

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
		   "UNIQUE INDEX(LogCod))");

   /***** Table log_ws *****/
/*
mysql> DESCRIBE log_ws;
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
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS log_ws ("
			"LogCod INT NOT NULL,"
			"PlgCod INT NOT NULL,"
			"FunCod INT NOT NULL,"
		   "UNIQUE INDEX(LogCod),"
		   "INDEX(PlgCod),"
		   "INDEX(FunCod))");

   /***** Table mail_domains *****/
/*
mysql> DESCRIBE mail_domains;
+--------+---------------+------+-----+---------+----------------+
| Field  | Type          | Null | Key | Default | Extra          |
+--------+---------------+------+-----+---------+----------------+
| MaiCod | int(11)       | NO   | PRI | NULL    | auto_increment |
| Domain | varchar(255)  | NO   | UNI | NULL    |                |
| Info   | varchar(2047) | NO   |     | NULL    |                |
+--------+---------------+------+-----+---------+----------------+
3 rows in set (0,00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS mail_domains ("
			"MaiCod INT NOT NULL AUTO_INCREMENT,"
			"Domain VARCHAR(255) NOT NULL,"	// Cns_MAX_BYTES_EMAIL
			"Info VARCHAR(2047) NOT NULL,"	// Mai_MAX_BYTES_MAIL_INFO
		   "UNIQUE INDEX(MaiCod),"
		   "UNIQUE INDEX(Domain))");

   /***** Table marks_properties *****/
/*
mysql> DESCRIBE marks_properties;
+--------+---------+------+-----+---------+-------+
| Field  | Type    | Null | Key | Default | Extra |
+--------+---------+------+-----+---------+-------+
| FilCod | int(11) | NO   | PRI | NULL    |       |
| Header | int(11) | NO   |     | NULL    |       |
| Footer | int(11) | NO   |     | NULL    |       |
+--------+---------+------+-----+---------+-------+
3 rows in set (0.00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS marks_properties ("
                   "FilCod INT NOT NULL,"
                   "Header INT NOT NULL,"
                   "Footer INT NOT NULL,"
                   "UNIQUE INDEX(FilCod))");

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
                   "UNIQUE INDEX(FromUsrCod,ToUsrCod))");

   /***** Table msg_content *****/
/*
mysql> DESCRIBE msg_content;
+------------+--------------+------+-----+---------+----------------+
| Field      | Type         | Null | Key | Default | Extra          |
+------------+--------------+------+-----+---------+----------------+
| MsgCod     | int(11)      | NO   | PRI | NULL    | auto_increment |
| Subject    | text         | NO   | MUL | NULL    |                |
| Content    | longtext     | NO   |     | NULL    |                |
| ImageName  | varchar(43)  | NO   |     |         |                |
| ImageTitle | varchar(255) | NO   |     |         |                |
| ImageURL   | varchar(255) | NO   |     |         |                |
+------------+--------------+------+-----+---------+----------------+
6 rows in set (0,00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS msg_content ("
                   "MsgCod INT NOT NULL AUTO_INCREMENT,"
                   "Subject TEXT NOT NULL,"
                   "Content LONGTEXT NOT NULL,"
                   "ImageName VARCHAR(43) NOT NULL DEFAULT '',"
                   "ImageTitle VARCHAR(255) NOT NULL DEFAULT '',"
                   "ImageURL VARCHAR(255) NOT NULL DEFAULT '',"
                   "UNIQUE INDEX(MsgCod),"
                   "FULLTEXT(Subject,Content)) ENGINE = MYISAM;");

   /***** Table msg_content_deleted *****/
/*
mysql> DESCRIBE msg_content_deleted;                                                                    +------------+--------------+------+-----+---------+-------+
| Field      | Type         | Null | Key | Default | Extra |
+------------+--------------+------+-----+---------+-------+
| MsgCod     | int(11)      | NO   | PRI | NULL    |       |
| Subject    | text         | NO   | MUL | NULL    |       |
| Content    | longtext     | NO   |     | NULL    |       |
| ImageName  | varchar(43)  | NO   |     |         |       |
| ImageTitle | varchar(255) | NO   |     |         |       |
| ImageURL   | varchar(255) | NO   |     |         |       |
+------------+--------------+------+-----+---------+-------+
6 rows in set (0,00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS msg_content_deleted ("
                   "MsgCod INT NOT NULL,"
                   "Subject TEXT NOT NULL,"
                   "Content LONGTEXT NOT NULL,"
                   "ImageName VARCHAR(43) NOT NULL DEFAULT '',"
                   "ImageTitle VARCHAR(255) NOT NULL DEFAULT '',"
                   "ImageURL VARCHAR(255) NOT NULL DEFAULT '',"
                   "UNIQUE INDEX(MsgCod),"
                   "FULLTEXT(Subject,Content)) ENGINE = MYISAM;");

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
                   "INDEX(Notified))");

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
                   "INDEX(Notified))");

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
                   "INDEX(UsrCod))");

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
                   "INDEX(UsrCod))");

   /***** Table notices *****/
/*
mysql> DESCRIBE notices;
+-----------+------------+------+-----+---------+----------------+
| Field     | Type       | Null | Key | Default | Extra          |
+-----------+------------+------+-----+---------+----------------+
| NotCod    | int(11)    | NO   | PRI | NULL    | auto_increment |
| CrsCod    | int(11)    | NO   | MUL | -1      |                |
| UsrCod    | int(11)    | NO   | MUL | NULL    |                |
| CreatTime | datetime   | NO   | MUL | NULL    |                |
| Content   | text       | NO   |     | NULL    |                |
| Status    | tinyint(4) | NO   | MUL | 0       |                |
| NumNotif  | int(11)    | NO   |     | 0       |                |
+-----------+------------+------+-----+---------+----------------+
7 rows in set (0.00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS notices ("
                   "NotCod INT NOT NULL AUTO_INCREMENT,"
                   "CrsCod INT NOT NULL DEFAULT -1,"
                   "UsrCod INT NOT NULL,"
                   "CreatTime DATETIME NOT NULL,"
                   "Content TEXT NOT NULL,"
                   "Status TINYINT NOT NULL DEFAULT 0,"
                   "NumNotif INT NOT NULL DEFAULT 0,"
                   "UNIQUE INDEX(NotCod),"
                   "INDEX(CrsCod,Status),"
                   "INDEX(UsrCod),"
                   "INDEX(CreatTime),"
                   "INDEX(Status))");

   /***** Table notices_deleted *****/
/*
mysql> DESCRIBE notices_deleted;
+-----------+----------+------+-----+---------+-------+
| Field     | Type     | Null | Key | Default | Extra |
+-----------+----------+------+-----+---------+-------+
| NotCod    | int(11)  | NO   | PRI | NULL    |       |
| CrsCod    | int(11)  | NO   | MUL | -1      |       |
| UsrCod    | int(11)  | NO   | MUL | NULL    |       |
| CreatTime | datetime | NO   | MUL | NULL    |       |
| Content   | text     | NO   |     | NULL    |       |
| NumNotif  | int(11)  | NO   |     | 0       |       |
+-----------+----------+------+-----+---------+-------+
6 rows in set (0.01 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS notices_deleted ("
                   "NotCod INT NOT NULL,"
                   "CrsCod INT NOT NULL DEFAULT -1,"
                   "UsrCod INT NOT NULL,"
                   "CreatTime DATETIME NOT NULL,"
                   "Content TEXT NOT NULL,"
                   "NumNotif INT NOT NULL DEFAULT 0,"
                   "UNIQUE INDEX(NotCod),"
                   "INDEX(CrsCod),"
                   "INDEX(UsrCod),"
                   "INDEX(CreatTime))");

   /***** Table notif *****/
/*
mysql> DESCRIBE notif;
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
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS notif ("
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
                   "INDEX(TimeNotif))");

   /***** Table pending_emails *****/
/*
MariaDB [swad]> DESCRIBE pending_emails;
+-------------+--------------+------+-----+---------+-------+
| Field       | Type         | Null | Key | Default | Extra |
+-------------+--------------+------+-----+---------+-------+
| UsrCod      | int(11)      | NO   | MUL | NULL    |       |
| E_mail      | varchar(127) | NO   |     | NULL    |       |
| MailKey     | char(43)     | NO   | PRI | NULL    |       |
| DateAndTime | datetime     | NO   |     | NULL    |       |
+-------------+--------------+------+-----+---------+-------+
4 rows in set (0.00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS pending_emails ("
                   "UsrCod INT NOT NULL,"
                   "E_mail VARCHAR(127) COLLATE latin1_general_ci NOT NULL,"
                   "MailKey CHAR(43) COLLATE latin1_bin NOT NULL,"
                   "DateAndTime DATETIME NOT NULL,"
                   "INDEX(UsrCod),"
                   "UNIQUE INDEX(MailKey))");

   /***** Table pending_passwd *****/
/*
mysql> DESCRIBE pending_passwd;
+-----------------+----------+------+-----+---------+-------+
| Field           | Type     | Null | Key | Default | Extra |
+-----------------+----------+------+-----+---------+-------+
| UsrCod          | int(11)  | NO   | PRI | NULL    |       |
| PendingPassword | char(86) | NO   |     | NULL    |       |
| DateAndTime     | datetime | NO   |     | NULL    |       |
+-----------------+----------+------+-----+---------+-------+
3 rows in set (0.00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS pending_passwd ("
                   "UsrCod INT NOT NULL,"
                   "PendingPassword CHAR(86) COLLATE latin1_bin NOT NULL,"
                   "DateAndTime DATETIME NOT NULL,"
                   "PRIMARY KEY (UsrCod))");

   /***** Table places *****/
/*
mysql> DESCRIBE places;
+-----------+--------------+------+-----+---------+----------------+
| Field     | Type         | Null | Key | Default | Extra          |
+-----------+--------------+------+-----+---------+----------------+
| PlcCod    | int(11)      | NO   | PRI | NULL    | auto_increment |
| InsCod    | int(11)      | NO   | MUL | NULL    |                |
| ShortName | varchar(32)  | NO   |     | NULL    |                |
| FullName  | varchar(127) | NO   |     | NULL    |                |
+-----------+--------------+------+-----+---------+----------------+
4 rows in set (0.00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS places ("
                   "PlcCod INT NOT NULL AUTO_INCREMENT,"
                   "InsCod INT NOT NULL,"
                   "ShortName VARCHAR(32) NOT NULL,"
                   "FullName VARCHAR(127) NOT NULL,"
                   "UNIQUE INDEX(PlcCod),"
                   "INDEX(InsCod))");

   /***** Table plugins *****/
/*
mysql> DESCRIBE plugins;
+-------------+--------------+------+-----+---------+----------------+
| Field       | Type         | Null | Key | Default | Extra          |
+-------------+--------------+------+-----+---------+----------------+
| PlgCod      | int(11)      | NO   | PRI | NULL    | auto_increment |
| Name        | varchar(127) | NO   |     | NULL    |                |
| Description | text         | NO   |     | NULL    |                |
| Logo        | varchar(16)  | NO   |     | NULL    |                |
| AppKey      | varchar(16)  | NO   |     | NULL    |                |
| URL         | varchar(255) | NO   |     | NULL    |                |
| IP          | char(15)     | NO   |     | NULL    |                |
+-------------+--------------+------+-----+---------+----------------+
7 rows in set (0.01 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS plugins ("
                   "PlgCod INT NOT NULL AUTO_INCREMENT,"
                   "Name VARCHAR(127) NOT NULL,"
                   "Description TEXT NOT NULL,"
                   "Logo VARCHAR(16) NOT NULL,"
                   "AppKey VARCHAR(16) NOT NULL,"
                   "URL VARCHAR(255) NOT NULL,"
                   "IP CHAR(15) NOT NULL,"
                   "UNIQUE INDEX(PlgCod))");

   /***** Table sessions *****/
/*
mysql> DESCRIBE sessions;
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
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS sessions ("
                   "SessionId CHAR(43) NOT NULL,"
                   "UsrCod INT NOT NULL,"
                   "Password CHAR(86) COLLATE latin1_bin NOT NULL,"
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
		   "SearchStr VARCHAR(2047) NOT NULL DEFAULT '',"	// Sch_MAX_BYTES_STRING_TO_FIND
                   "SideCols TINYINT NOT NULL DEFAULT 3,"
                   "UNIQUE INDEX(SessionId),"
                   "INDEX(UsrCod))");

   /***** Table social_comments *****/
/*
mysql> DESCRIBE social_comments;
+------------+--------------+------+-----+---------+-------+
| Field      | Type         | Null | Key | Default | Extra |
+------------+--------------+------+-----+---------+-------+
| PubCod     | bigint(20)   | NO   | PRI | NULL    |       |
| Content    | longtext     | NO   | MUL | NULL    |       |
| ImageName  | varchar(43)  | NO   |     | NULL    |       |
| ImageTitle | varchar(255) | NO   |     | NULL    |       |
| ImageURL   | varchar(255) | NO   |     | NULL    |       |
+------------+--------------+------+-----+---------+-------+
5 rows in set (0.00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS social_comments ("
	           "PubCod BIGINT NOT NULL,"
                   "Content LONGTEXT NOT NULL,"
                   "ImageName VARCHAR(43) NOT NULL,"
                   "ImageTitle VARCHAR(255) NOT NULL,"
                   "ImageURL VARCHAR(255) NOT NULL,"
                   "UNIQUE INDEX(PubCod),"
                   "FULLTEXT(Content)) ENGINE = MYISAM;");

   /***** Table social_comments_fav *****/
/*
mysql> DESCRIBE social_comments_fav;
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
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS social_comments_fav ("
	           "FavCod BIGINT AUTO_INCREMENT,"
	           "PubCod BIGINT NOT NULL,"
                   "UsrCod INT NOT NULL,"
	           "TimeFav DATETIME NOT NULL,"	// Not used. For future use
	           "UNIQUE INDEX(FavCod),"
                   "UNIQUE INDEX(PubCod,UsrCod),"
                   "INDEX(UsrCod))");

   /***** Table social_notes *****/
/*
mysql> DESCRIBE social_notes;
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
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS social_notes ("
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
                   "INDEX(TimeNote))");

   /***** Table social_notes_fav *****/
/*
mysql> DESCRIBE social_notes_fav;
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
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS social_notes_fav ("
	           "FavCod BIGINT AUTO_INCREMENT,"
	           "NotCod BIGINT NOT NULL,"
                   "UsrCod INT NOT NULL,"
	           "TimeFav DATETIME NOT NULL,"	// Not used. For future use
	           "UNIQUE INDEX(FavCod),"
                   "UNIQUE INDEX(NotCod,UsrCod),"
                   "INDEX(UsrCod))");

   /***** Table social_posts *****/
/*
mysql> DESCRIBE social_posts;
+------------+--------------+------+-----+---------+----------------+
| Field      | Type         | Null | Key | Default | Extra          |
+------------+--------------+------+-----+---------+----------------+
| PstCod     | int(11)      | NO   | PRI | NULL    | auto_increment |
| Content    | longtext     | NO   | MUL | NULL    |                |
| ImageName  | varchar(43)  | NO   |     | NULL    |                |
| ImageTitle | varchar(255) | NO   |     | NULL    |                |
| ImageURL   | varchar(255) | NO   |     | NULL    |                |
+------------+--------------+------+-----+---------+----------------+
5 rows in set (0.00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS social_posts ("
                   "PubCod INT NOT NULL AUTO_INCREMENT,"
                   "Content LONGTEXT NOT NULL,"
                   "ImageName VARCHAR(43) NOT NULL,"
                   "ImageTitle VARCHAR(255) NOT NULL,"
                   "ImageURL VARCHAR(255) NOT NULL,"
                   "UNIQUE INDEX(PubCod),"
                   "FULLTEXT(Content)) ENGINE = MYISAM;");

   /***** Table social_pubs *****/
/*
mysql> DESCRIBE social_pubs;
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
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS social_pubs ("
	           "PubCod BIGINT NOT NULL AUTO_INCREMENT,"
                   "NotCod BIGINT NOT NULL,"
                   "PublisherCod INT NOT NULL,"
	           "PubType TINYINT NOT NULL,"
                   "TimePublish DATETIME NOT NULL,"
                   "UNIQUE INDEX(PubCod),"
                   "INDEX(NotCod,PublisherCod,PubType),"
                   "INDEX(PublisherCod),"
                   "INDEX(PubType),"
                   "INDEX(TimePublish))");

   /***** Table social_timelines *****/
/*
mysql> DESCRIBE social_timelines;
+-----------+------------+------+-----+---------+-------+
| Field     | Type       | Null | Key | Default | Extra |
+-----------+------------+------+-----+---------+-------+
| SessionId | char(43)   | NO   | PRI | NULL    |       |
| NotCod    | bigint(20) | NO   | PRI | NULL    |       |
+-----------+------------+------+-----+---------+-------+
2 rows in set (0.00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS social_timelines ("
                   "SessionId CHAR(43) NOT NULL,"
                   "NotCod BIGINT NOT NULL,"
                   "UNIQUE INDEX(SessionId,NotCod))");

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
                   "UNIQUE INDEX(DegCod,Sex))");

   /***** Table sta_notif *****/
/*
mysql> DESCRIBE sta_notif;
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
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS sta_notif ("
                   "DegCod INT NOT NULL,"
                   "CrsCod INT NOT NULL,"
                   "NotifyEvent TINYINT NOT NULL,"
                   "NumEvents INT NOT NULL,"
                   "NumMails INT NOT NULL,"
                   "UNIQUE INDEX(DegCod,CrsCod,NotifyEvent))");

   /***** Table surveys *****/
/*
mysql> DESCRIBE surveys;
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
| Title     | varchar(255)                              | NO   |     | NULL    |                |
| Txt       | text                                      | NO   |     | NULL    |                |
+-----------+-------------------------------------------+------+-----+---------+----------------+
13 rows in set (0,00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS surveys ("
                   "SvyCod INT NOT NULL AUTO_INCREMENT,"
	           "Scope ENUM('Sys','Cty','Ins','Ctr','Deg','Crs') NOT NULL DEFAULT 'Sys',"
	           "Cod INT NOT NULL DEFAULT -1,"
                   "Hidden ENUM('N','Y') NOT NULL DEFAULT 'N',"
                   "NumNotif INT NOT NULL DEFAULT 0,"
                   "Roles INT NOT NULL DEFAULT 0,"
                   "UsrCod INT NOT NULL,"
                   "StartTime DATETIME NOT NULL,"
                   "EndTime DATETIME NOT NULL,"
                   "Title VARCHAR(255) NOT NULL,"
                   "Txt TEXT NOT NULL,"
                   "UNIQUE INDEX(SvyCod),"
	           "INDEX(Scope,Cod))");

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
                   "Answer TEXT NOT NULL,"
                   "UNIQUE INDEX(QstCod,AnsInd))");

   /***** Table svy_grp *****/
/*
mysql> DESCRIBE svy_grp;
+--------+---------+------+-----+---------+-------+
| Field  | Type    | Null | Key | Default | Extra |
+--------+---------+------+-----+---------+-------+
| SvyCod | int(11) | NO   | PRI | NULL    |       |
| GrpCod | int(11) | NO   | PRI | NULL    |       |
+--------+---------+------+-----+---------+-------+
2 rows in set (0.01 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS svy_grp ("
                   "SvyCod INT NOT NULL,"
                   "GrpCod INT NOT NULL,"
                   "UNIQUE INDEX(SvyCod,GrpCod))");

   /***** Table svy_questions *****/
/*
mysql> DESCRIBE svy_questions;
+---------+-----------------------------------------+------+-----+---------+----------------+
| Field   | Type                                    | Null | Key | Default | Extra          |
+---------+-----------------------------------------+------+-----+---------+----------------+
| QstCod  | int(11)                                 | NO   | PRI | NULL    | auto_increment |
| SvyCod  | int(11)                                 | NO   | MUL | NULL    |                |
| QstInd  | int(11)                                 | NO   |     | 0       |                |
| AnsType | enum('unique_choice','multiple_choice') | NO   |     | NULL    |                |
| Stem    | text                                    | NO   |     | NULL    |                |
+---------+-----------------------------------------+------+-----+---------+----------------+
5 rows in set (0.00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS svy_questions ("
                   "QstCod INT NOT NULL AUTO_INCREMENT,"
                   "SvyCod INT NOT NULL,"
                   "QstInd INT NOT NULL DEFAULT 0,"
                   "AnsType ENUM ('unique_choice','multiple_choice') NOT NULL,"
                   "Stem TEXT NOT NULL,"
                   "UNIQUE INDEX(QstCod),"
                   "INDEX(SvyCod))");

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
                   "UNIQUE INDEX(SvyCod,UsrCod))");

   /***** Table timetable_crs *****/
/*
mysql> DESCRIBE timetable_crs;
+-----------+------------------------------------+------+-----+---------+-------+
| Field     | Type                               | Null | Key | Default | Extra |
+-----------+------------------------------------+------+-----+---------+-------+
| CrsCod    | int(11)                            | NO   | MUL | -1      |       |
| GrpCod    | int(11)                            | NO   |     | -1      |       |
| Day       | enum('L','M','X','J','V','S','D')  | NO   |     | NULL    |       |
| Hour      | tinyint(4)                         | NO   |     | NULL    |       |
| Duration  | tinyint(4)                         | NO   |     | NULL    |       |
| ClassType | enum('libre','teoria','practicas') | NO   |     | NULL    |       |
| Place     | varchar(127)                       | NO   |     | NULL    |       |
| GroupName | varchar(255)                       | NO   |     | NULL    |       |
+-----------+------------------------------------+------+-----+---------+-------+
8 rows in set (0.01 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS timetable_crs ("
                   "CrsCod INT NOT NULL DEFAULT -1,"
                   "GrpCod INT NOT NULL DEFAULT -1,"
                   "Day ENUM('L','M','X','J','V','S','D') NOT NULL,"
                   "Hour TINYINT NOT NULL,"
                   "Duration TINYINT NOT NULL,"
                   "ClassType ENUM('libre','teoria','practicas') NOT NULL,"
                   "Place VARCHAR(127) NOT NULL,"
                   "GroupName VARCHAR(255) NOT NULL,"
                   "INDEX(CrsCod,GrpCod))");

   /***** Table timetable_tut *****/
/*
mysql> DESCRIBE timetable_tut;
+----------+-----------------------------------+------+-----+---------+-------+
| Field    | Type                              | Null | Key | Default | Extra |
+----------+-----------------------------------+------+-----+---------+-------+
| UsrCod   | int(11)                           | NO   | MUL | NULL    |       |
| Day      | enum('L','M','X','J','V','S','D') | NO   |     | NULL    |       |
| Hour     | tinyint(4)                        | NO   |     | NULL    |       |
| Duration | tinyint(4)                        | NO   |     | NULL    |       |
| Place    | varchar(127)                      | NO   |     | NULL    |       |
+----------+-----------------------------------+------+-----+---------+-------+
5 rows in set (0.01 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS timetable_tut ("
                   "UsrCod INT NOT NULL,"
                   "Day ENUM('L','M','X','J','V','S','D') NOT NULL,"
                   "Hour TINYINT NOT NULL,"
                   "Duration TINYINT NOT NULL,"
                   "Place VARCHAR(127) NOT NULL,"
                   "INDEX(UsrCod))");

   /***** Table tst_answers *****/
/*
mysql> DESCRIBE tst_answers;
+------------+---------------+------+-----+---------+-------+
| Field      | Type          | Null | Key | Default | Extra |
+------------+---------------+------+-----+---------+-------+
| QstCod     | int(11)       | NO   | MUL | NULL    |       |
| AnsInd     | tinyint(4)    | NO   |     | NULL    |       |
| Answer     | text          | NO   |     | NULL    |       |
| Feedback   | text          | NO   |     | NULL    |       |
| ImageName  | varchar(43)   | NO   |     | NULL    |       |
| ImageTitle | varchar(255)  | NO   |     | NULL    |       |
| ImageURL   | varchar(255)  | NO   |     | NULL    |       |
| Correct    | enum('N','Y') | NO   |     | NULL    |       |
+------------+---------------+------+-----+---------+-------+
8 rows in set (0.01 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS tst_answers ("
                   "QstCod INT NOT NULL,"
                   "AnsInd TINYINT NOT NULL,"
                   "Answer TEXT NOT NULL,"
                   "Feedback TEXT NOT NULL,"
                   "ImageName VARCHAR(43) NOT NULL,"
                   "ImageTitle VARCHAR(255) NOT NULL,"
                   "ImageURL VARCHAR(255) NOT NULL,"
                   "Correct ENUM('N','Y') NOT NULL,"
                   "INDEX(QstCod))");

   /***** Table tst_config *****/
/*
mysql> DESCRIBE tst_config;
+---------------------+------------------------------------------------------------------------------+------+-----+---------+-------+
| Field               | Type                                                                         | Null | Key | Default | Extra |
+---------------------+------------------------------------------------------------------------------+------+-----+---------+-------+
| CrsCod              | int(11)                                                                      | NO   | PRI | -1      |       |
| Pluggable           | enum('unknown','N','Y')                                                      | NO   |     | unknown |       |
| Min                 | int(11)                                                                      | NO   |     | NULL    |       |
| Def                 | int(11)                                                                      | NO   |     | NULL    |       |
| Max                 | int(11)                                                                      | NO   |     | NULL    |       |
| MinTimeNxtTstPerQst | int(11)                                                                      | NO   |     | 0       |       |
| Feedback            | enum('nothing','total_result','each_result','each_good_bad','full_feedback') | NO   |     | NULL    |       |
+---------------------+------------------------------------------------------------------------------+------+-----+---------+-------+
7 rows in set (0,00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS tst_config ("
                   "CrsCod INT NOT NULL DEFAULT -1,"
                   "Pluggable ENUM('unknown','N','Y') NOT NULL DEFAULT 'unknown',"
                   "Min INT NOT NULL,"
                   "Def INT NOT NULL,"
                   "Max INT NOT NULL,"
                   "MinTimeNxtTstPerQst INT NOT NULL DEFAULT 0,"
                   "Feedback ENUM('nothing','total_result','each_result','each_good_bad','full_feedback') NOT NULL,"
                   "UNIQUE INDEX(CrsCod))");

/***** Table tst_exam_questions *****/
/*
mysql> DESCRIBE tst_exam_questions;
+---------+---------+------+-----+---------+-------+
| Field   | Type    | Null | Key | Default | Extra |
+---------+---------+------+-----+---------+-------+
| TstCod  | int(11) | NO   | MUL | NULL    |       |
| QstCod  | int(11) | NO   |     | NULL    |       |
| QstInd  | int(11) | NO   |     | NULL    |       |
| Score   | double  | NO   |     | 0       |       |
| Indexes | text    | NO   |     | NULL    |       |
| Answers | text    | NO   |     | NULL    |       |
+---------+---------+------+-----+---------+-------+
6 rows in set (0.00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS tst_exam_questions ("
                   "TstCod INT NOT NULL,"
                   "QstCod INT NOT NULL,"
                   "QstInd INT NOT NULL,"
                   "Score DOUBLE PRECISION NOT NULL DEFAULT 0,"
                   "Indexes TEXT NOT NULL,"
                   "Answers TEXT NOT NULL,"
                   "INDEX(TstCod,QstCod))");

   /***** Table tst_exams *****/
/*
mysql> DESCRIBE tst_exams;
+-----------------+---------------+------+-----+---------+----------------+
| Field           | Type          | Null | Key | Default | Extra          |
+-----------------+---------------+------+-----+---------+----------------+
| TstCod          | int(11)       | NO   | PRI | NULL    | auto_increment |
| CrsCod          | int(11)       | NO   | MUL | NULL    |                |
| UsrCod          | int(11)       | NO   |     | NULL    |                |
| AllowTeachers   | enum('N','Y') | NO   |     | N       |                |
| TstTime         | datetime      | NO   |     | NULL    |                |
| NumQsts         | int(11)       | NO   |     | 0       |                |
| NumQstsNotBlank | int(11)       | NO   |     | 0       |                |
| Score           | double        | NO   |     | 0       |                |
+-----------------+---------------+------+-----+---------+----------------+
8 rows in set (0.05 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS tst_exams ("
                   "TstCod INT NOT NULL AUTO_INCREMENT,"
                   "CrsCod INT NOT NULL,"
                   "UsrCod INT NOT NULL,"
                   "AllowTeachers ENUM('N','Y') NOT NULL DEFAULT 'N',"
                   "TstTime DATETIME NOT NULL,"
                   "NumQsts INT NOT NULL DEFAULT 0,"
                   "NumQstsNotBlank INT NOT NULL DEFAULT 0,"
                   "Score DOUBLE PRECISION NOT NULL DEFAULT 0,"
                   "UNIQUE INDEX(TstCod),"
                   "INDEX(CrsCod,UsrCod))");

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
                   "UNIQUE INDEX(QstCod,TagCod))");

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
| ImageName       | varchar(43)                                                               | NO   |     | NULL    |                |
| ImageTitle      | varchar(255)                                                              | NO   |     | NULL    |                |
| ImageURL        | varchar(255)                                                              | NO   |     | NULL    |                |
| NumHits         | int(11)                                                                   | NO   |     | 0       |                |
| NumHitsNotBlank | int(11)                                                                   | NO   |     | 0       |                |
| Score           | double                                                                    | NO   |     | 0       |                |
+-----------------+---------------------------------------------------------------------------+------+-----+---------+----------------+
13 rows in set (0.00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS tst_questions ("
                   "QstCod INT NOT NULL AUTO_INCREMENT,"
                   "CrsCod INT NOT NULL DEFAULT -1,"
                   "EditTime DATETIME NOT NULL,"
                   "AnsType ENUM ('int','float','true_false','unique_choice','multiple_choice','text') NOT NULL,"
                   "Shuffle ENUM('N','Y') NOT NULL,"
                   "Stem TEXT NOT NULL,"
                   "Feedback TEXT NOT NULL,"
                   "ImageName VARCHAR(43) NOT NULL,"
                   "ImageTitle VARCHAR(255) NOT NULL,"
                   "NumHits INT NOT NULL DEFAULT 0,"
                   "NumHitsNotBlank INT NOT NULL DEFAULT 0,"
                   "Score DOUBLE PRECISION NOT NULL DEFAULT 0,"
                   "UNIQUE INDEX(QstCod),"
                   "INDEX(CrsCod,EditTime))");

   /***** Table tst_status *****/
/*
mysql> DESCRIBE tst_status;
+-----------+------------+------+-----+---------+-------+
| Field     | Type       | Null | Key | Default | Extra |
+-----------+------------+------+-----+---------+-------+
| SessionId | char(43)   | NO   | PRI | NULL    |       |
| CrsCod    | int(11)    | NO   | PRI | NULL    |       |
| NumTst    | int(11)    | NO   | PRI | NULL    |       |
| Status    | tinyint(4) | NO   |     | NULL    |       |
+-----------+------------+------+-----+---------+-------+
4 rows in set (0.00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS tst_status ("
                   "SessionId CHAR(43) NOT NULL,"
                   "CrsCod INT NOT NULL,"
                   "NumTst INT NOT NULL,"
                   "Status TINYINT NOT NULL,"
                   "UNIQUE INDEX(SessionId,CrsCod,NumTst))");

   /***** Table tst_tags *****/
/*
mysql> DESCRIBE tst_tags;
+------------+---------------+------+-----+---------+----------------+
| Field      | Type          | Null | Key | Default | Extra          |
+------------+---------------+------+-----+---------+----------------+
| TagCod     | int(11)       | NO   | PRI | NULL    | auto_increment |
| CrsCod     | int(11)       | NO   | MUL | -1      |                |
| ChangeTime | datetime      | NO   |     | NULL    |                |
| TagTxt     | varchar(255)  | NO   | MUL | NULL    |                |
| TagHidden  | enum('N','Y') | NO   |     | NULL    |                |
+------------+---------------+------+-----+---------+----------------+
5 rows in set (0.00 sec)
*/
// CrsCod is redundant for speed in querys
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS tst_tags ("
                   "TagCod INT NOT NULL AUTO_INCREMENT,"
                   "CrsCod INT NOT NULL DEFAULT -1,"
                   "ChangeTime DATETIME NOT NULL,"
                   "TagTxt VARCHAR(255) NOT NULL,"
                   "TagHidden ENUM('N','Y') NOT NULL,"
                   "UNIQUE INDEX(TagCod),"
                   "INDEX(CrsCod,ChangeTime),"
                   "INDEX(TagTxt))");

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
		   "UNIQUE INDEX(UsrCod))");

   /***** Table usr_data *****/
/*
mysql> DESCRIBE usr_data;
+-------------------+--------------------------------------------------+------+-----+---------+----------------+
| Field             | Type                                             | Null | Key | Default | Extra          |
+-------------------+--------------------------------------------------+------+-----+---------+----------------+
| UsrCod            | int(11)                                          | NO   | PRI | NULL    | auto_increment |
| EncryptedUsrCod   | char(43)                                         | NO   | UNI |         |                |
| Password          | char(86)                                         | NO   |     |         |                |
| Surname1          | varchar(32)                                      | NO   |     |         |                |
| Surname2          | varchar(32)                                      | NO   |     |         |                |
| FirstName         | varchar(32)                                      | NO   |     |         |                |
| Sex               | enum('unknown','female','male')                  | NO   |     | unknown |                |
| Theme             | char(16)                                         | NO   | MUL |         |                |
| IconSet           | char(16)                                         | NO   | MUL |         |                |
| Language          | char(2)                                          | NO   | MUL |         |                |
| FirstDayOfWeek    | tinyint(4)                                       | NO   | MUL | 0       |                |
| Photo             | char(43)                                         | NO   |     |         |                |
| PhotoVisibility   | enum('unknown','user','course','system','world') | NO   | MUL | unknown |                |
| ProfileVisibility | enum('unknown','user','course','system','world') | NO   | MUL | unknown |                |
| CtyCod            | int(11)                                          | NO   | MUL | -1      |                |
| InsCtyCod         | int(11)                                          | NO   | MUL | -1      |                |
| InsCod            | int(11)                                          | NO   | MUL | -1      |                |
| DptCod            | int(11)                                          | NO   | MUL | -1      |                |
| CtrCod            | int(11)                                          | NO   | MUL | -1      |                |
| Office            | varchar(127)                                     | NO   |     |         |                |
| OfficePhone       | char(16)                                         | NO   |     |         |                |
| LocalAddress      | varchar(127)                                     | NO   |     |         |                |
| LocalPhone        | char(16)                                         | NO   |     |         |                |
| FamilyAddress     | varchar(127)                                     | NO   |     |         |                |
| FamilyPhone       | char(16)                                         | NO   |     |         |                |
| OriginPlace       | varchar(127)                                     | NO   |     |         |                |
| Birthday          | date                                             | YES  |     | NULL    |                |
| Comments          | text                                             | NO   |     | NULL    |                |
| Menu              | tinyint(4)                                       | NO   | MUL | 0       |                |
| SideCols          | tinyint(4)                                       | NO   | MUL | 3       |                |
| NotifNtfEvents    | int(11)                                          | NO   |     | 0       |                |
| EmailNtfEvents    | int(11)                                          | NO   |     | 0       |                |
+-------------------+--------------------------------------------------+------+-----+---------+----------------+
32 rows in set (0.00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS usr_data ("
                   "UsrCod INT NOT NULL AUTO_INCREMENT,"
                   "EncryptedUsrCod CHAR(43) NOT NULL DEFAULT '',"
                   "Password CHAR(86) COLLATE latin1_bin NOT NULL DEFAULT '',"
                   "Surname1 VARCHAR(32) COLLATE latin1_spanish_ci NOT NULL DEFAULT '',"
                   "Surname2 VARCHAR(32) COLLATE latin1_spanish_ci NOT NULL DEFAULT '',"
                   "FirstName VARCHAR(32) COLLATE latin1_spanish_ci NOT NULL DEFAULT '',"
                   "Sex ENUM ('unknown','female','male') NOT NULL DEFAULT 'unknown',"
                   "Theme CHAR(16) NOT NULL DEFAULT '',"
                   "IconSet CHAR(16) NOT NULL DEFAULT '',"
                   "Language CHAR(2) NOT NULL DEFAULT '',"
		   "FirstDayOfWeek TINYINT NOT NULL DEFAULT 0,"
                   "Photo CHAR(43) NOT NULL DEFAULT '',"
		   "PhotoVisibility ENUM('unknown','user','course','system','world') NOT NULL DEFAULT 'unknown',"
		   "ProfileVisibility ENUM('unknown','user','course','system','world') NOT NULL DEFAULT 'unknown',"
                   "CtyCod INT NOT NULL DEFAULT -1,"
                   "InsCtyCod INT NOT NULL DEFAULT -1,"
                   "InsCod INT NOT NULL DEFAULT -1,"
                   "DptCod INT NOT NULL DEFAULT -1,"
                   "CtrCod INT NOT NULL DEFAULT -1,"
                   "Office VARCHAR(127) NOT NULL DEFAULT '',"
                   "OfficePhone CHAR(16) NOT NULL DEFAULT '',"
                   "LocalAddress VARCHAR(127) NOT NULL DEFAULT '',"
                   "LocalPhone CHAR(16) NOT NULL DEFAULT '',"
                   "FamilyAddress VARCHAR(127) NOT NULL DEFAULT '',"
                   "FamilyPhone CHAR(16) NOT NULL DEFAULT '',"
                   "OriginPlace VARCHAR(127) NOT NULL DEFAULT '',"
                   "Birthday DATE,"
                   "Comments TEXT NOT NULL,"
                   "Menu TINYINT NOT NULL DEFAULT 0,"
                   "SideCols TINYINT NOT NULL DEFAULT 3,"
                   "NotifNtfEvents INT NOT NULL DEFAULT 0,"
                   "EmailNtfEvents INT NOT NULL DEFAULT 0,"
                   "PRIMARY KEY(UsrCod),"
                   "UNIQUE INDEX(EncryptedUsrCod),"
                   "INDEX(Theme),"
                   "INDEX(IconSet),"
                   "INDEX(Language),"
                   "INDEX(FirstDayOfWeek),"
		   "INDEX(PhotoVisibility),"
		   "INDEX(ProfileVisibility),"
                   "INDEX(CtyCod),"
                   "INDEX(InsCtyCod),"
                   "INDEX(InsCod),"
                   "INDEX(DptCod),"
                   "INDEX(CtrCod),"
                   "INDEX(Menu),"
                   "INDEX(SideCols))");

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
	           "INDEX(UsrCod))");

   /***** Table usr_emails *****/
   /*
mysql> DESCRIBE usr_emails;
+-----------+---------------+------+-----+---------+-------+
| Field     | Type          | Null | Key | Default | Extra |
+-----------+---------------+------+-----+---------+-------+
| UsrCod    | int(11)       | NO   | PRI | NULL    |       |
| E_mail    | varchar(127)  | NO   | PRI | NULL    |       |
| CreatTime | datetime      | NO   |     | NULL    |       |
| Confirmed | enum('N','Y') | NO   |     | N       |       |
+-----------+---------------+------+-----+---------+-------+
4 rows in set (0.01 sec)
   */
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS usr_emails ("
                   "UsrCod INT NOT NULL,"
                   "E_mail VARCHAR(127) COLLATE latin1_general_ci NOT NULL,"
                   "CreatTime DATETIME NOT NULL,"
                   "Confirmed ENUM('N','Y') NOT NULL DEFAULT 'N',"
                   "UNIQUE INDEX(UsrCod,E_mail),"
                   "UNIQUE INDEX(E_mail))");

   /***** Table usr_figures *****/
   /*
mysql> DESCRIBE usr_figures;
+----------------+----------+------+-----+---------+-------+
| Field          | Type     | Null | Key | Default | Extra |
+----------------+----------+------+-----+---------+-------+
| UsrCod         | int(11)  | NO   | PRI | NULL    |       |
| FirstClickTime | datetime | NO   | MUL | NULL    |       |
| NumClicks      | int(11)  | NO   | MUL | -1      |       |
| NumFileViews   | int(11)  | NO   |     | -1      |       |
| NumForPst      | int(11)  | NO   |     | -1      |       |
| NumMsgSnt      | int(11)  | NO   |     | -1      |       |
+----------------+----------+------+-----+---------+-------+
6 rows in set (0.01 sec)
   */
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS usr_figures ("
	           "UsrCod INT NOT NULL,"
	           "FirstClickTime DATETIME NOT NULL,"
	           "NumClicks INT NOT NULL DEFAULT -1,"
	           "NumFileViews INT NOT NULL DEFAULT -1,"
	           "NumForPst INT NOT NULL DEFAULT -1,"
	           "NumMsgSnt INT NOT NULL DEFAULT -1,"
	           "PRIMARY KEY(UsrCod),"
	           "INDEX(FirstClickTime),"
	           "INDEX(NumClicks))");

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
	           "INDEX (FollowTime))");

/***** Table usr_IDs *****/
/*
mysql> DESCRIBE usr_IDs;
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
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS usr_IDs ("
                   "UsrCod INT NOT NULL,"
                   "UsrID CHAR(16) NOT NULL,"
                   "CreatTime DATETIME NOT NULL,"
                   "Confirmed ENUM('N','Y') NOT NULL DEFAULT 'N',"
                   "UNIQUE INDEX(UsrCod,UsrID),"
                   "INDEX(UsrID))");

   /***** Table usr_last *****/
/*
mysql> DESCRIBE usr_last;
+--------------+------------+------+-----+---------+-------+
| Field        | Type       | Null | Key | Default | Extra |
+--------------+------------+------+-----+---------+-------+
| UsrCod       | int(11)    | NO   | PRI | NULL    |       |
| WhatToSearch | tinyint(4) | NO   |     | 0       |       |
| LastCrs      | int(11)    | NO   |     | -1      |       |
| LastTab      | tinyint(4) | NO   |     | NULL    |       |
| LastTime     | datetime   | NO   | MUL | NULL    |       |
| LastAccNotif | datetime   | NO   |     | NULL    |       |
+--------------+------------+------+-----+---------+-------+
6 rows in set (0.00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS usr_last ("
                   "UsrCod INT NOT NULL,"
                   "WhatToSearch TINYINT NOT NULL DEFAULT 0,"
                   "LastCrs INT NOT NULL DEFAULT -1,"
                   "LastTab TINYINT NOT NULL,"
                   "LastTime DATETIME NOT NULL,"
                   "LastAccNotif DATETIME NOT NULL,"
                   "UNIQUE INDEX(UsrCod),"
                   "INDEX(LastTime))");

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
                   "Nickname CHAR(16) COLLATE latin1_spanish_ci NOT NULL,"
                   "CreatTime DATETIME NOT NULL,"
                   "UNIQUE INDEX(UsrCod,Nickname),"
                   "UNIQUE INDEX(Nickname))");

   /***** Table usr_report *****/
/*
mysql> DESCRIBE usr_report;
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
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS usr_report ("
                   "RepCod INT NOT NULL AUTO_INCREMENT,"
		   "UsrCod INT NOT NULL,"
	           "ReportTimeUTC DATETIME NOT NULL,"
	           "UniqueDirL CHAR(2) NOT NULL,"	//  2  leftmost chars from a unique 43 chars base64url codified from a unique SHA-256 string
	           "UniqueDirR CHAR(41) NOT NULL,"	// 41 rightmost chars from a unique 43 chars base64url codified from a unique SHA-256 string
	           "Filename VARCHAR(255) NOT NULL,"	// Report filename
	           "Permalink VARCHAR(255) NOT NULL,"	// Full URL (permalink)
	           "UNIQUE INDEX(RepCod),"
		   "INDEX(UsrCod))");

/***** Table usr_webs *****/
/*
mysql> DESCRIBE usr_webs;
+--------+-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+------+-----+---------+-------+
| Field  | Type                                                                                                                                                                                                                                                                                    | Null | Key | Default | Extra |
+--------+-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+------+-----+---------+-------+
| UsrCod | int(11)                                                                                                                                                                                                                                                                                 | NO   | PRI | NULL    |       |
| Web    | enum('www','500px','delicious','deviantart','diaspora','edmodo','facebook','flickr','foursquare','github','googleplus','googlescholar','instagram','linkedin','paperli','pinterest','quitter','researchgate','scoopit','slideshare','storify','tumblr','twitter','wikipedia','youtube') | NO   | PRI | NULL    |       |
| URL    | varchar(255)                                                                                                                                                                                                                                                                            | NO   |     | NULL    |       |
+--------+-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+------+-----+---------+-------+
3 rows in set (0.00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS usr_webs ("
                   "UsrCod INT NOT NULL,"
                   "Web ENUM("
                   "'www',"
                   "'500px',"
                   "'delicious','deviantart','diaspora',"
                   "'edmodo',"
                   "'facebook','flickr','foursquare',"
                   "'github','gnusocial','googleplus','googlescholar',"
                   "'identica','instagram',"
                   "'linkedin',"
		   "'orcid',"
                   "'paperli','pinterest',"
	           "'quitter',"
                   "'researchgate','researcherid',"
                   "'scoopit','slideshare','stackoverflow','storify',"
                   "'tumblr','twitter',"
                   "'wikipedia',"
                   "'youtube') NOT NULL,"
                   "URL VARCHAR(255) NOT NULL,"
                   "UNIQUE INDEX(UsrCod,Web))");

/***** Table ws_keys *****/
/*
mysql> DESCRIBE ws_keys;
+----------+----------+------+-----+---------+-------+
| Field    | Type     | Null | Key | Default | Extra |
+----------+----------+------+-----+---------+-------+
| WSKey    | char(43) | NO   | PRI | NULL    |       |
| UsrCod   | int(11)  | NO   | MUL | NULL    |       |
| PlgCod   | int(11)  | NO   |     | NULL    |       |
| LastTime | datetime | NO   | MUL | NULL    |       |
+----------+----------+------+-----+---------+-------+
4 rows in set (0.00 sec)
*/
   DB_CreateTable ("CREATE TABLE IF NOT EXISTS ws_keys ("
                    "WSKey CHAR(43) NOT NULL,"
                    "UsrCod INT NOT NULL,"
                    "PlgCod INT NOT NULL,"
                    "LastTime DATETIME NOT NULL,"
                    "UNIQUE INDEX(WSKey),"
                    "INDEX(UsrCod),"
                    "INDEX(PlgCod),"
                    "INDEX(LastTime))");

   /***** Show success message *****/
   fprintf (Gbl.F.Out,"</ol>");
   Lay_ShowAlert (Lay_SUCCESS,Txt_Created_tables_in_the_database_that_did_not_exist);
  }

/*****************************************************************************/
/****************** Create a database table if not exists ********************/
/*****************************************************************************/

static void DB_CreateTable (const char *Query)
  {
   fprintf (Gbl.F.Out,"<li class=\"DAT\">%s</li>",Query);
   if (mysql_query (&Gbl.mysql,Query))
      DB_ExitOnMySQLError ("can not create table");
  }

/*****************************************************************************/
/********************* Open connection to the database ***********************/
/*****************************************************************************/

void DB_OpenDBConnection (void)
  {
   if (mysql_init (&Gbl.mysql) == NULL)
      Lay_ShowErrorAndExit ("Can not init MySQL.");

   if (mysql_real_connect (&Gbl.mysql,Cfg_DATABASE_HOST,
	                   Cfg_DATABASE_USER,Gbl.Config.DatabasePassword,
	                   Cfg_DATABASE_DBNAME,0,NULL,0) == NULL)
      DB_ExitOnMySQLError ("can not connect to database");

   Gbl.DB.DatabaseIsOpen = true;
  }

/*****************************************************************************/
/********************* Close connection to the database **********************/
/*****************************************************************************/

void DB_CloseDBConnection (void)
  {
   if (Gbl.DB.DatabaseIsOpen)
     {
      mysql_close (&Gbl.mysql);	// Close the connection to the database
      Gbl.DB.DatabaseIsOpen = false;
     }
  }

/*****************************************************************************/
/******************** Make a SELECT query from database **********************/
/*****************************************************************************/

unsigned long DB_QuerySELECT (const char *Query,MYSQL_RES **mysql_res,const char *MsgError)
  {
   /***** Query database *****/
   if (mysql_query (&Gbl.mysql,Query))
      DB_ExitOnMySQLError (MsgError);

   /***** Store query result *****/
   if ((*mysql_res = mysql_store_result (&Gbl.mysql)) == NULL)
      DB_ExitOnMySQLError (MsgError);

   /***** Return number of rows of result *****/
   return (unsigned long) mysql_num_rows (*mysql_res);
  }

/*****************************************************************************/
/**************** Make a SELECT COUNT query from database ********************/
/*****************************************************************************/

unsigned long DB_QueryCOUNT (const char *Query,const char *MsgError)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;

   /***** Make query "SELECT COUNT(*) FROM..." *****/
   DB_QuerySELECT (Query,&mysql_res,MsgError);

   /***** Get number of rows *****/
   row = mysql_fetch_row (mysql_res);
   if (sscanf (row[0],"%lu",&NumRows) != 1)
      Lay_ShowErrorAndExit ("Error when counting number of rows.");

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return NumRows;
  }

/*****************************************************************************/
/******************** Make an INSERT query in database ***********************/
/*****************************************************************************/

void DB_QueryINSERT (const char *Query,const char *MsgError)
  {
   /***** Query database *****/
   if (mysql_query (&Gbl.mysql,Query))
      DB_ExitOnMySQLError (MsgError);
  }

/*****************************************************************************/
/** Make an INSERT query in database and return code of last inserted item ***/
/*****************************************************************************/

long DB_QueryINSERTandReturnCode (const char *Query,const char *MsgError)
  {
   /***** Query database *****/
   if (mysql_query (&Gbl.mysql,Query))
      DB_ExitOnMySQLError (MsgError);

   /***** Return the code of the inserted item *****/
   return (long) mysql_insert_id (&Gbl.mysql);
  }

/*****************************************************************************/
/******************** Make an REPLACE query in database **********************/
/*****************************************************************************/

void DB_QueryREPLACE (const char *Query,const char *MsgError)
  {
   /***** Query database *****/
   if (mysql_query (&Gbl.mysql,Query))
      DB_ExitOnMySQLError (MsgError);
  }

/*****************************************************************************/
/******************** Make a UPDATE query from database **********************/
/*****************************************************************************/

void DB_QueryUPDATE (const char *Query,const char *MsgError)
  {
   /***** Query database *****/
   if (mysql_query (&Gbl.mysql,Query))
      DB_ExitOnMySQLError (MsgError);

   /***** Return number of rows updated *****/
   //return (unsigned long) mysql_affected_rows (&Gbl.mysql);
  }

/*****************************************************************************/
/******************** Make a DELETE query from database **********************/
/*****************************************************************************/

void DB_QueryDELETE (const char *Query,const char *MsgError)
  {
   /***** Query database *****/
   if (mysql_query (&Gbl.mysql,Query))
      DB_ExitOnMySQLError (MsgError);
  }

/*****************************************************************************/
/**************** Make other kind of query from database *********************/
/*****************************************************************************/

void DB_Query (const char *Query,const char *MsgError)
  {
   /***** Query database *****/
   if (mysql_query (&Gbl.mysql,Query))
      DB_ExitOnMySQLError (MsgError);
  }

/*****************************************************************************/
/********** Free structure that stores the result of a SELECT query **********/
/*****************************************************************************/

void DB_FreeMySQLResult (MYSQL_RES **mysql_res)
  {
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
   char BigErrorMsg[1024 * 1024];

   sprintf (BigErrorMsg,"Database error: %s (%s).",
            Message,mysql_error (&Gbl.mysql));
   Lay_ShowErrorAndExit (BigErrorMsg);
  }
