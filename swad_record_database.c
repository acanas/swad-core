// swad_record_database.c: users' record cards, operations with database

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
/********************************* Headers ***********************************/
/*****************************************************************************/

#include "swad_database.h"
#include "swad_record_database.h"

/*****************************************************************************/
/************************* Create a field of record **************************/
/*****************************************************************************/

void Rec_DB_CreateField (long CrsCod,const struct RecordField *Field)
  {
   DB_QueryINSERT ("can not create field of record",
		   "INSERT INTO crs_record_fields"
		   " (CrsCod,FieldName,NumLines,Visibility)"
		   " VALUES"
		   " (%ld,'%s',%u,%u)",
	           CrsCod,
	           Field->Name,
	           Field->NumLines,
	           (unsigned) Field->Visibility);
  }

/*****************************************************************************/
/********************* Insert text field of course record ********************/
/*****************************************************************************/

void Rec_DB_CreateFieldContent (long FldCod,long UsrCod,const char *Text)
  {
   DB_QueryINSERT ("can not create field of record",
		   "INSERT INTO crs_records"
		   " (FieldCod,UsrCod,Txt)"
		   " VALUES"
		   " (%ld,%ld,'%s')",
		   FldCod,
		   UsrCod,
		   Text);
  }

/*****************************************************************************/
/********************* Update text field of course record ********************/
/*****************************************************************************/

void Rec_DB_UpdateFieldTxt (long FldCod,long UsrCod,const char *Text)
  {
   DB_QueryUPDATE ("can not update field of record",
		   "UPDATE crs_records"
		     " SET Txt='%s'"
		   " WHERE UsrCod=%ld"
		     " AND FieldCod=%ld",
		   Text,
		   UsrCod,
		   FldCod);
  }

/*****************************************************************************/
/******** Update course record field changing the old name by the new ********/
/*****************************************************************************/

void Rec_DB_UpdateFieldName (long FldCod,const char NewFieldName[Rec_MAX_BYTES_NAME_FIELD + 1])
  {
   DB_QueryUPDATE ("can not update name of field of record",
		   "UPDATE crs_record_fields"
		     " SET FieldName='%s'"
		   " WHERE FieldCod=%ld",
		   NewFieldName,
		   FldCod);
   }

/*****************************************************************************/
/*** Update course record field changing the old number of lines by the new **/
/*****************************************************************************/

void Rec_DB_UpdateFieldNumLines (long FldCod,unsigned NewNumLines)
  {
   DB_QueryUPDATE ("can not update the number of lines of a record field",
		   "UPDATE crs_record_fields"
		     " SET NumLines=%u"
		   " WHERE FieldCod=%ld",
		   NewNumLines,
		   FldCod);
  }

/*****************************************************************************/
/***** Update course record field changing the old visibility by the new *****/
/*****************************************************************************/

void Rec_DB_UpdateFieldVisibility (long FldCod,Rec_VisibilityRecordFields_t NewVisibility)
  {
   DB_QueryUPDATE ("can not update the visibility of a record field",
		   "UPDATE crs_record_fields"
		     " SET Visibility=%u"
		   " WHERE FieldCod=%ld",
		   (unsigned) NewVisibility,
		   FldCod);
  }

/*****************************************************************************/
/*************** Get the number of records with a field filled ***************/
/*****************************************************************************/

unsigned Rec_DB_CountNumRecordsWithFieldContent (long FldCod)
  {
   return (unsigned)
   DB_QueryCOUNT ("can not get number of records with a given field not empty",
		  "SELECT COUNT(*)"
		   " FROM crs_records"
		  " WHERE FieldCod=%ld",
		  FldCod);
  }


/*****************************************************************************/
/******* Get the fields of records already present in current course *********/
/*****************************************************************************/

unsigned Rec_DB_GetAllFieldsInCrs (MYSQL_RES **mysql_res,long CrsCod)
  {
   return (unsigned)
   DB_QuerySELECT (mysql_res,"can not get fields of records in a course",
		   "SELECT FieldCod,"	// row[0]
			  "FieldName,"	// row[1]
			  "NumLines,"	// row[2]
			  "Visibility"	// row[3]
		    " FROM crs_record_fields"
		   " WHERE CrsCod=%ld"
		" ORDER BY FieldName",
		   CrsCod);
  }

/*****************************************************************************/
/************** Get the data of a field of records from its code *************/
/*****************************************************************************/

unsigned Rec_DB_GetFieldByCod (MYSQL_RES **mysql_res,long CrsCod,long FldCod)
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
		   FldCod);
  }

/*****************************************************************************/
/************** Get the text of a field of a record of course ****************/
/*****************************************************************************/

Exi_Exist_t Rec_DB_GetFieldTxtFromUsrRecord (MYSQL_RES **mysql_res,
                                             long FldCod,long UsrCod)
  {
   return
   DB_QuerySELECT (mysql_res,"can not get the text"
			     " of a field of a record",
		   "SELECT Txt"		// row[0]
		    " FROM crs_records"
		   " WHERE FieldCod=%ld"
		     " AND UsrCod=%ld",
		   FldCod,
		   UsrCod) ? Exi_EXISTS :
			     Exi_DOES_NOT_EXIST;
  }

/*****************************************************************************/
/******************* Remove field from all students' records *****************/
/*****************************************************************************/

void Rec_DB_RemoveFieldContentFromAllUsrsRecords (long FldCod)
  {
   DB_QueryDELETE ("can not remove field from all students' records",
		   "DELETE FROM crs_records"
		   " WHERE FieldCod=%ld",
                   FldCod);
  }

/*****************************************************************************/
/******************* Remove text of field of course record *******************/
/*****************************************************************************/

void Rec_DB_RemoveFieldContentFromUsrRecord (long FldCod,long UsrCod)
  {
   DB_QueryDELETE ("can not remove field of record",
		   "DELETE FROM crs_records"
		   " WHERE UsrCod=%ld"
		     " AND FieldCod=%ld",
		   UsrCod,
		   FldCod);
  }

/*****************************************************************************/
/************ Remove fields of record of a user in current course ************/
/*****************************************************************************/

void Rec_DB_RemoveAllFieldContentsFromUsrRecordInCrs (long UsrCod,long CrsCod)
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

void Rec_DB_RemoveAllFieldContentsFromUsrRecords (long UsrCod)
  {
   DB_QueryDELETE ("can not remove user's records in all courses",
		   "DELETE FROM crs_records"
		   " WHERE UsrCod=%ld",
		   UsrCod);
  }

/*****************************************************************************/
/******************** Remove content of course record cards ******************/
/*****************************************************************************/

void Rec_DB_RemoveAllFieldContentsInCrs (long CrsCod)
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
/*************************** Remove record field *****************************/
/*****************************************************************************/

void Rec_DB_RemoveField (long FldCod)
  {
   DB_QueryDELETE ("can not remove field of record",
		   "DELETE FROM crs_record_fields"
		   " WHERE FieldCod=%ld",
                   FldCod);
  }

/*****************************************************************************/
/************ Remove definition of fields in course record cards *************/
/*****************************************************************************/

void Rec_DB_RemoveAllFieldsInCrs (long CrsCod)
  {
   DB_QueryDELETE ("can not remove fields of cards in a course",
		   "DELETE FROM crs_record_fields"
		   " WHERE CrsCod=%ld",
		   CrsCod);
  }
