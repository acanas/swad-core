// swad_record_database.c: users' record cards, operations with database

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
/********************************* Headers ***********************************/
/*****************************************************************************/

// #define _GNU_SOURCE 		// For asprintf
// #include <linux/limits.h>	// For PATH_MAX
// #include <stddef.h>		// For NULL
// #include <stdio.h>		// For asprintf
// #include <stdlib.h>		// For calloc
// #include <string.h>

// #include "swad_account.h"
// #include "swad_account_database.h"
// #include "swad_action.h"
// #include "swad_box.h"
// #include "swad_config.h"
#include "swad_database.h"
// #include "swad_department.h"
// #include "swad_enrolment_database.h"
// #include "swad_error.h"
// #include "swad_follow_database.h"
// #include "swad_form.h"
// #include "swad_global.h"
// #include "swad_hierarchy_level.h"
// #include "swad_HTML.h"
// #include "swad_ID.h"
// #include "swad_logo.h"
// #include "swad_message.h"
// #include "swad_message_database.h"
// #include "swad_network.h"
// #include "swad_parameter.h"
// #include "swad_photo.h"
// #include "swad_privacy.h"
// #include "swad_QR.h"
// #include "swad_record.h"
#include "swad_record_database.h"
// #include "swad_role.h"
// #include "swad_setting.h"
// #include "swad_timetable.h"
// #include "swad_user.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

// extern struct Globals Gbl;

/*****************************************************************************/
/***************************** Private constants *****************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

/*****************************************************************************/
/********************* Insert text field of course record ********************/
/*****************************************************************************/

void Rec_DB_CreateCrsRecordField (long FieldCod,long UsrCod,const char *Text)
  {
   DB_QueryINSERT ("can not create field of record",
		   "INSERT INTO crs_records"
		   " (FieldCod,UsrCod,Txt)"
		   " VALUES"
		   " (%ld,%ld,'%s')",
		   FieldCod,
		   UsrCod,
		   Text);
  }

/*****************************************************************************/
/********************* Update text field of course record ********************/
/*****************************************************************************/

void Rec_DB_UpdateCrsRecordField (long FieldCod,long UsrCod,const char *Text)
  {
   DB_QueryUPDATE ("can not update field of record",
		   "UPDATE crs_records"
		     " SET Txt='%s'"
		   " WHERE UsrCod=%ld"
		     " AND FieldCod=%ld",
		   Text,
		   UsrCod,
		   FieldCod);
  }

/*****************************************************************************/
/******** Update course record field changing the old name by the new ********/
/*****************************************************************************/

void Rec_DB_UpdateCrsRecordFieldName (long FieldCod,const char NewFieldName[Rec_MAX_BYTES_NAME_FIELD + 1])
  {
   DB_QueryUPDATE ("can not update name of field of record",
		   "UPDATE crs_record_fields"
		     " SET FieldName='%s'"
		   " WHERE FieldCod=%ld",
		   NewFieldName,
		   FieldCod);
   }

/*****************************************************************************/
/*** Update course record field changing the old number of lines by the new **/
/*****************************************************************************/

void Rec_DB_UpdateCrsRecordFieldNumLines (long FieldCod,unsigned NewNumLines)
  {
   DB_QueryUPDATE ("can not update the number of lines of a record field",
		   "UPDATE crs_record_fields"
		     " SET NumLines=%u"
		   " WHERE FieldCod=%ld",
		   NewNumLines,
		   FieldCod);
  }

/*****************************************************************************/
/***** Update course record field changing the old visibility by the new *****/
/*****************************************************************************/

void Rec_DB_UpdateCrsRecordFieldVisibility (long FieldCod,Rec_VisibilityRecordFields_t NewVisibility)
  {
   DB_QueryUPDATE ("can not update the visibility of a record field",
		   "UPDATE crs_record_fields"
		     " SET Visibility=%u"
		   " WHERE FieldCod=%ld",
		   (unsigned) NewVisibility,
		   FieldCod);
  }

/*****************************************************************************/
/************** Get the data of a field of records from its code *************/
/*****************************************************************************/

unsigned Rec_DB_GetFieldByCod (MYSQL_RES **mysql_res,long CrsCod,long FieldCod)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get a field of a record in a course",
		   "SELECT FieldName,"	// row[0]
			  "NumLines,"	// row[1]
			  "Visibility"	// row[2]
		    " FROM crs_record_fields"
		   " WHERE CrsCod=%ld"
		     " AND FieldCod=%ld",
		   CrsCod,
		   FieldCod);
  }

/*****************************************************************************/
/************** Get the text of a field of a record of course ****************/
/*****************************************************************************/

unsigned Rec_DB_GetFieldFromCrsRecord (MYSQL_RES **mysql_res,
                                       long FieldCod,long UsrCod)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get the text"
			     " of a field of a record",
		   "SELECT Txt"		// row[0]
		    " FROM crs_records"
		   " WHERE FieldCod=%ld"
		     " AND UsrCod=%ld",
		   FieldCod,
		   UsrCod);
  }

/*****************************************************************************/
/******************* Remove text of field of course record *******************/
/*****************************************************************************/

void Rec_DB_RemoveCrsRecordField (long FieldCod,long UsrCod)
  {
   DB_QueryDELETE ("can not remove field of record",
		   "DELETE FROM crs_records"
		   " WHERE UsrCod=%ld"
		     " AND FieldCod=%ld",
		   UsrCod,
		   FieldCod);
  }

/*****************************************************************************/
/************ Remove fields of record of a user in current course ************/
/*****************************************************************************/

void Rec_DB_RemoveFieldsCrsRecordInCrs (long UsrCod,long CrsCod)
  {
   DB_QueryDELETE ("can not remove user's record in a course",
		   "DELETE FROM crs_records"
		   " WHERE UsrCod=%ld"
		     " AND FieldCod IN"
		         " (SELECT FieldCod"
		            " FROM crs_record_fields"
		           " WHERE CrsCod=%ld)",
                   UsrCod,
                   CrsCod);
  }

/*****************************************************************************/
/************* Remove fields of record of a user in all courses **************/
/*****************************************************************************/

void Rec_DB_RemoveFieldsCrsRecordAll (long UsrCod)
  {
   DB_QueryDELETE ("can not remove user's records in all courses",
		   "DELETE FROM crs_records"
		   " WHERE UsrCod=%ld",
		   UsrCod);
  }

/*****************************************************************************/
/******************** Remove content of course record cards ******************/
/*****************************************************************************/

void Rec_DB_RemoveCrsRecordContents (long CrsCod)
  {
   DB_QueryDELETE ("can not remove content of cards in a course",
		   "DELETE FROM crs_records"
		   " USING crs_record_fields,"
			  "crs_records"
		   " WHERE crs_record_fields.CrsCod=%ld"
		     " AND crs_record_fields.FieldCod=crs_records.FieldCod",
		   CrsCod);
  }

/*****************************************************************************/
/************ Remove definition of fields in course record cards *************/
/*****************************************************************************/

void Rec_DB_RemoveCrsRecordFields (long CrsCod)
  {
   DB_QueryDELETE ("can not remove fields of cards in a course",
		   "DELETE FROM crs_record_fields"
		   " WHERE CrsCod=%ld",
		   CrsCod);
  }
