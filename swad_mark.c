// swad_mark.c: marks

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
/********************************** Headers **********************************/
/*****************************************************************************/

#include <linux/limits.h>	// For PATH_MAX
#include <linux/stddef.h>	// For NULL
#include <malloc.h>		// For malloc
#include <string.h>		// For string functions
#include <unistd.h>		// For unlink

#include "swad_database.h"
#include "swad_global.h"
#include "swad_ID.h"
#include "swad_mark.h"
#include "swad_notification.h"
#include "swad_parameter.h"
#include "swad_string.h"
#include "swad_user.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/****************************** Internal types *******************************/
/*****************************************************************************/

typedef enum
  {
   Brw_HEADER = 0,
   Brw_FOOTER = 1,
  } Brw_HeadOrFoot_t;

/*****************************************************************************/
/**************************** Internal constants *****************************/
/*****************************************************************************/

const char *Mrk_HeadOrFootStr[2] =	// Names of columns in database, so don't change!
 {
  "Header",
  "Footer",
 };

#define Mrk_MAX_BYTES_IN_CELL_CONTENT	1024	// Cell of a table containing one or several user's IDs

/*****************************************************************************/
/*************************** Internal prototypes *****************************/
/*****************************************************************************/

static void Mrk_GetNumRowsHeaderAndFooter (struct MarksProperties *Marks);
static void Mrk_ChangeNumRowsHeaderOrFooter (Brw_HeadOrFoot_t HeaderOrFooter);
static bool Mrk_CheckIfCellContainsOnlyIDs (const char *CellContent);
static bool Mrk_GetUsrMarks (FILE *FileUsrMarks,struct UsrData *UsrDat,
                             const char *PathFileAllMarks,
                             struct MarksProperties *Marks);

/*****************************************************************************/
/****************** Add a new entry of marks into database *******************/
/*****************************************************************************/

void Mrk_AddMarksToDB (long FilCod,struct MarksProperties *Marks)
  {
   char Query[256];

   /***** Add file of marks to the database *****/
   sprintf (Query,"INSERT INTO marks_properties (FilCod,%s,%s)"
                  " VALUES ('%ld','%u','%u')",
            Mrk_HeadOrFootStr[Brw_HEADER],
            Mrk_HeadOrFootStr[Brw_FOOTER],
            FilCod,
	    Marks->Header,
	    Marks->Footer);
   DB_QueryINSERT (Query,"can not add properties of marks to database");
  }

/*****************************************************************************/
/********* Write number of header and footer rows of a file of marks *********/
/*****************************************************************************/

void Mrk_GetAndWriteNumRowsHeaderAndFooter (Brw_FileType_t FileType,
                                            const char *PathInTree,
                                            const char *FileName)
  {
   extern const char *The_ClassFormNoWrap[The_NUM_THEMES];
   extern const char *Txt_TABLE_Header;
   extern const char *Txt_TABLE_Footer;
   struct MarksProperties Marks;

   if (FileType == Brw_IS_FOLDER)
      fprintf (Gbl.F.Out,"<td class=\"COLOR%u\"></td>"
                         "<td class=\"COLOR%u\"></td>",
               Gbl.RowEvenOdd,
               Gbl.RowEvenOdd);
   else	// File or link
     {
      /***** Get number of rows in header or footer *****/
      Mrk_GetNumRowsHeaderAndFooter (&Marks);

      /***** Write the number of rows of header *****/
      fprintf (Gbl.F.Out,"<td class=\"%s RIGHT_TOP COLOR%u\">",
               The_ClassFormNoWrap[Gbl.Prefs.Theme],
               Gbl.RowEvenOdd);
      if (Gbl.CurrentCrs.Grps.GrpCod > 0)	// Group zone
        {
         Act_FormStart (ActChgNumRowHeaGrp);
         Grp_PutParamGrpCod (Gbl.CurrentCrs.Grps.GrpCod);
        }
      else					// Course zone
         Act_FormStart (ActChgNumRowHeaCrs);
      fprintf (Gbl.F.Out,"<label>&nbsp;%s: "
                         "<input type=\"text\" name=\"%s\""
                         " size=\"1\" maxlength=\"5\" value=\"%u\""
                         " class=\"%s COLOR%u\""
                         " onchange=\"document.getElementById('%s').submit();\" />"
                         "</label>",
               Txt_TABLE_Header,
               Mrk_HeadOrFootStr[Brw_HEADER],Marks.Header,
               Gbl.FileBrowser.InputStyle,
               Gbl.RowEvenOdd,
               Gbl.Form.Id);
      Brw_PutParamsFileBrowser (ActUnk,
                                PathInTree,FileName,
                                FileType,-1L);
      Act_FormEnd ();
      fprintf (Gbl.F.Out,"</td>");

      /***** Write the number of rows of footer *****/
      fprintf (Gbl.F.Out,"<td class=\"%s RIGHT_TOP COLOR%u\">",
               The_ClassFormNoWrap[Gbl.Prefs.Theme],
               Gbl.RowEvenOdd);
      if (Gbl.CurrentCrs.Grps.GrpCod > 0)	// Group zone
        {
         Act_FormStart (ActChgNumRowFooGrp);
         Grp_PutParamGrpCod (Gbl.CurrentCrs.Grps.GrpCod);
        }
      else					// Course zone
         Act_FormStart (ActChgNumRowFooCrs);
      fprintf (Gbl.F.Out,"<label>&nbsp;%s: "
                         "<input type=\"text\" name=\"%s\""
                         " size=\"1\" maxlength=\"5\" value=\"%u\""
                         " class=\"%s COLOR%u\""
                         " onchange=\"document.getElementById('%s').submit();\" />"
                         "</label>",
               Txt_TABLE_Footer,
               Mrk_HeadOrFootStr[Brw_FOOTER],Marks.Footer,
               Gbl.FileBrowser.InputStyle,
               Gbl.RowEvenOdd,
               Gbl.Form.Id);
      Brw_PutParamsFileBrowser (ActUnk,
                                PathInTree,FileName,
                                FileType,-1L);
      Act_FormEnd ();
      fprintf (Gbl.F.Out,"</td>");
     }
  }

/*****************************************************************************/
/******** Get number of rows of header and of footer of a file of marks ******/
/*****************************************************************************/

static void Mrk_GetNumRowsHeaderAndFooter (struct MarksProperties *Marks)
  {
   extern const Brw_FileBrowser_t Brw_FileBrowserForDB_files[Brw_NUM_TYPES_FILE_BROWSER];
   long Cod = Brw_GetCodForFiles ();
   char Query[512 + PATH_MAX];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;

   /***** Get number of rows of header and footer from database *****/
   sprintf (Query,"SELECT marks_properties.%s,marks_properties.%s"
	          " FROM files,marks_properties"
                  " WHERE files.FileBrowser='%u' AND files.Cod='%ld' AND files.Path='%s'"
                  " AND files.FilCod=marks_properties.FilCod",
            Mrk_HeadOrFootStr[Brw_HEADER],
            Mrk_HeadOrFootStr[Brw_FOOTER],
            (unsigned) Brw_FileBrowserForDB_files[Gbl.FileBrowser.Type],
            Cod,
	    Gbl.FileBrowser.Priv.FullPathInTree);
   NumRows = DB_QuerySELECT (Query,&mysql_res,"can not get the number of rows in header and footer");

   /***** The result of the query must have only one row *****/
   if (NumRows == 1)
     {
      /***** Get number of header and footer rows *****/
      row = mysql_fetch_row (mysql_res);

      /* Header (row[0]) */
      if (sscanf (row[0],"%u",&(Marks->Header)) != 1)
         Lay_ShowErrorAndExit ("Wrong number of header rows.");

      /* Footer (row[1]) */
      if (sscanf (row[1],"%u",&(Marks->Footer)) != 1)
         Lay_ShowErrorAndExit ("Wrong number of footer rows.");
     }
   else if (NumRows == 0)	// Unknown numbers of header and footer rows
      Marks->Header =
      Marks->Footer = 0;
   else	// Number of entries in database > 1
      Lay_ShowErrorAndExit ("Error when getting the number of rows in header and footer.");

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/********* Change the number of rows of header of a file of marks ************/
/*****************************************************************************/

void Mrk_ChangeNumRowsHeader (void)
  {
   Mrk_ChangeNumRowsHeaderOrFooter (Brw_HEADER);
  }

/*****************************************************************************/
/********** Change the number of rows of footer of a file of marks ***********/
/*****************************************************************************/

void Mrk_ChangeNumRowsFooter (void)
  {
   Mrk_ChangeNumRowsHeaderOrFooter (Brw_FOOTER);
  }

/*****************************************************************************/
/***** Change the number of rows of header or footer of a file of marks ******/
/*****************************************************************************/

static void Mrk_ChangeNumRowsHeaderOrFooter (Brw_HeadOrFoot_t HeaderOrFooter)
  {
   extern const Brw_FileBrowser_t Brw_FileBrowserForDB_files[Brw_NUM_TYPES_FILE_BROWSER];
   extern const char *Txt_The_number_of_rows_is_now_X;
   char UnsignedStr[10 + 1];
   long Cod;
   char Query[512 + PATH_MAX];
   unsigned NumRows;

   /***** Get parameters related to file browser *****/
   Brw_GetParAndInitFileBrowser ();

   /***** Get the number of rows of the header or footer of the table of marks *****/
   Par_GetParToText (Mrk_HeadOrFootStr[HeaderOrFooter],UnsignedStr,10);
   if (sscanf (UnsignedStr,"%u",&NumRows) == 1)
     {
      /***** Update properties of marks in the database *****/
      Cod = Brw_GetCodForFiles ();
      sprintf (Query,"UPDATE marks_properties,files"
	             " SET marks_properties.%s='%u'"
	             " WHERE files.FileBrowser='%u' AND files.Cod='%ld' AND files.Path='%s'"
	             " AND files.FilCod=marks_properties.FilCod",
               Mrk_HeadOrFootStr[HeaderOrFooter],NumRows,
               (unsigned) Brw_FileBrowserForDB_files[Gbl.FileBrowser.Type],
               Cod,
               Gbl.FileBrowser.Priv.FullPathInTree);
      DB_QueryUPDATE (Query,"can not update properties of marks");

      /***** Write message of success *****/
      sprintf (Gbl.Message,Txt_The_number_of_rows_is_now_X,
               NumRows);
      Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);
     }
   else
      Lay_ShowErrorAndExit ("Wrong number of rows.");

   /***** Show again the file browser *****/
   Brw_ShowAgainFileBrowserOrWorks ();
  }

/*****************************************************************************/
/************************ Receive a new file of marks ************************/
/*****************************************************************************/
// Returns true if the format of the HTML file of marks is correct
// Returns true if the format of the HTML file of marks is wrong
// Gbl.Message will contain feedback text

bool Mrk_CheckFileOfMarks (const char *Path,struct MarksProperties *Marks)
  {
   extern const char *Txt_There_are_more_than_one_table_in_the_file_of_marks;
   extern const char *Txt_Table_not_found_in_the_file_of_marks;
   // extern const char *Txt_X_header_rows_Y_student_rows_and_Z_footer_rows_found;
   char CellContent[Mrk_MAX_BYTES_IN_CELL_CONTENT + 1];
   FILE *FileAllMarks;
   bool EndOfHead = false;
   bool EndOfTable = false;
   bool FileIsCorrect = true;
   unsigned NumRowsStds = 0;

   Marks->Header = Marks->Footer = 0;

   /***** Open file with the table of marks *****/
   if ((FileAllMarks = fopen (Path,"rb")))
     {
      /***** Check if there is a table in the received file ******/
      if (Str_FindStrInFile (FileAllMarks,"<table",Str_NO_SKIP_HTML_COMMENTS))
        {
         // There is a table
         Str_FindStrInFile (FileAllMarks,">",Str_NO_SKIP_HTML_COMMENTS);

         // Only one table is allowed
         if (Str_FindStrInFile (FileAllMarks,"<table",Str_NO_SKIP_HTML_COMMENTS))
           {
            sprintf (Gbl.Message,"%s",Txt_There_are_more_than_one_table_in_the_file_of_marks);
            FileIsCorrect = false;
           }
         else
           {
            /* Return to table start */
            rewind (FileAllMarks);
            Str_FindStrInFile (FileAllMarks,"<table",Str_NO_SKIP_HTML_COMMENTS);
            Str_FindStrInFile (FileAllMarks,">",Str_NO_SKIP_HTML_COMMENTS);

            /* We assume that the structure of the table has several rows of header until the first row of students is found,
	       then it has a number of rows of students, including some dummy rows without students,
               and finally it has several rows of footer from the last row of students until the end of the table */

            /***** Count rows of header *****/
            while (!EndOfHead)
               if (Str_FindStrInFile (FileAllMarks,"<tr",Str_NO_SKIP_HTML_COMMENTS))   // Go to the next row
                 {
        	  // All user's IDs must be in the first column of the row
		  Str_GetCellFromHTMLTableSkipComments (FileAllMarks,CellContent,Mrk_MAX_BYTES_IN_CELL_CONTENT);

		  /* Check if only user's IDs
		     or other stuff found in this table cell */
		  if (Mrk_CheckIfCellContainsOnlyIDs (CellContent))
		    {
		     // Only user's IDs found in this cell
		     EndOfHead = true;
		     NumRowsStds++;
		    }
		  else
		     // Other stuff found ==> continue in header
		     Marks->Header++;
                 }
               else
	          EndOfHead = true;	// No more rows

            /***** Count rows of students and rows of footer *****/
            while (!EndOfTable)
               if (Str_FindStrInFile (FileAllMarks,"<tr",Str_NO_SKIP_HTML_COMMENTS))   // Go to the next row
                 {
        	  // All user's IDs must be in the first column of the row
		  Str_GetCellFromHTMLTableSkipComments (FileAllMarks,CellContent,Mrk_MAX_BYTES_IN_CELL_CONTENT);

		  /* Check if only user's IDs
		     or other stuff found in this table cell */
		  if (Mrk_CheckIfCellContainsOnlyIDs (CellContent))
		    {
		     // Only user's IDs found in this cell
                     NumRowsStds++;
                     Marks->Footer = 0;
		    }
		  else
		     // Other stuff found ==> continue in header
                     Marks->Footer++;
                 }
               else
	          EndOfTable = true;	// No more rows
           }
        }
      else
        {
         sprintf (Gbl.Message,"%s",Txt_Table_not_found_in_the_file_of_marks);
         FileIsCorrect = false;
        }

      /***** The file of marks is no more necessary. Close it. *****/
      fclose (FileAllMarks);
     }
   /*
   if (FileIsCorrect)
      sprintf (Gbl.Message,Txt_X_header_rows_Y_student_rows_and_Z_footer_rows_found,
               Marks->Header,NumRowsStds,Marks->Footer);
   */

   return FileIsCorrect;
  }

/*****************************************************************************/
/******* Check if only user's IDs or other stuff found in a table cell *******/
/*****************************************************************************/

static bool Mrk_CheckIfCellContainsOnlyIDs (const char *CellContent)
  {
   char UsrIDFromTable[ID_MAX_LENGTH_USR_ID + 1];
   const char *Ptr = CellContent;
   bool UsrIDFound = false;
   bool StuffNotUsrIDFound = false;

   /***** Get strings in this table cell
          and check if they look like user's IDs or not *****/
   while (*Ptr && !StuffNotUsrIDFound)
     {
      /* Find next string in text until space, comma or semicolon (leading and trailing spaces are removed) */
      Str_GetNextStringUntilSeparator (&Ptr,UsrIDFromTable,ID_MAX_LENGTH_USR_ID);

      // Users' IDs are always stored internally in capitals and without leading zeros
      Str_RemoveLeadingZeros (UsrIDFromTable);
      Str_ConvertToUpperText (UsrIDFromTable);
      if (UsrIDFromTable[0])	// Something found
	{
	 if (ID_CheckIfUsrIDIsValid (UsrIDFromTable))
	    UsrIDFound = true;
	 else
	    StuffNotUsrIDFound = true;
	}
     }

   /***** Check if only user's IDs
          or other stuff found in this table cell *****/
   return (UsrIDFound && !StuffNotUsrIDFound);
  }

/*****************************************************************************/
/*************************** Show the marks of a user ************************/
/*****************************************************************************/

static bool Mrk_GetUsrMarks (FILE *FileUsrMarks,struct UsrData *UsrDat,
                             const char *PathFileAllMarks,
                             struct MarksProperties *Marks)
  {
   extern const char *Txt_THE_USER_X_is_not_found_in_the_file_of_marks;
   unsigned Row;
   char CellContent[Mrk_MAX_BYTES_IN_CELL_CONTENT + 1];
   const char *Ptr;
   char UsrIDFromTable[ID_MAX_LENGTH_USR_ID + 1];
   FILE *FileAllMarks;
   unsigned NumID;
   bool UsrIDFound;
   bool EndOfTable;

   /***** Open HTML file with the table of marks *****/
   if (!(FileAllMarks = fopen (PathFileAllMarks,"rb")))
     {  // Can't open the file with the table of marks
      sprintf (Gbl.Message,"%s","Can not open file of marks.");	// TODO: Need translation!
      return false;
     }

   /***** Check if it exists a user's ID in the first column of the table of marks *****/
   /* Jump to table start */
   Str_FindStrInFile (FileAllMarks,"<table",Str_NO_SKIP_HTML_COMMENTS);
   Str_FindStrInFile (FileAllMarks,">",Str_NO_SKIP_HTML_COMMENTS);

   /* Skip header */
   for (Row = 1;
	Row <= Marks->Header;
	Row++)
      Str_FindStrInFile (FileAllMarks,"<tr",Str_NO_SKIP_HTML_COMMENTS);   // Go to the next row

   /* Get user's IDs from table row by row until footer */
   UsrIDFound = EndOfTable = false;
   while (!UsrIDFound && !EndOfTable)
      if (Str_FindStrInFile (FileAllMarks,"<tr",Str_NO_SKIP_HTML_COMMENTS))   // Go to the next row
        {
         // All user's IDs must be in the first column of the row
	 Str_GetCellFromHTMLTableSkipComments (FileAllMarks,CellContent,Mrk_MAX_BYTES_IN_CELL_CONTENT);

	 /* Get user's IDs */
         Ptr = CellContent;
	 while (*Ptr && !UsrIDFound)
	   {
	    /* Find next string in text until comma or semicolon (leading and trailing spaces are removed) */
            Str_GetNextStringUntilSeparator (&Ptr,UsrIDFromTable,ID_MAX_LENGTH_USR_ID);

	    // Users' IDs are always stored internally in capitals and without leading zeros
	    Str_RemoveLeadingZeros (UsrIDFromTable);
	    Str_ConvertToUpperText (UsrIDFromTable);

	    if (ID_CheckIfUsrIDIsValid (UsrIDFromTable))
	       // A valid user's ID is found in the first column of table, and stored in UsrIDFromTable.
	       // Compare UsrIDFromTable with all the confirmed user's IDs in list
	       for (NumID = 0;
		    NumID < UsrDat->IDs.Num && !UsrIDFound;
		    NumID++)
		  if (UsrDat->IDs.List[NumID].Confirmed)
		     if (!strcasecmp (UsrDat->IDs.List[NumID].ID,UsrIDFromTable))
			UsrIDFound = true;
	   }
        }
      else
         EndOfTable = true;	// No more rows

   if (UsrIDFound)
     {
      /***** Write all until the header (included) *****/
      /* Go to start of file */
      rewind (FileAllMarks);

      /* Write until table start */
      Str_WriteUntilStrFoundInFileIncludingStr (FileUsrMarks,FileAllMarks,"<table",Str_NO_SKIP_HTML_COMMENTS);
      Str_WriteUntilStrFoundInFileIncludingStr (FileUsrMarks,FileAllMarks,">",Str_NO_SKIP_HTML_COMMENTS);

      /* Write header */
      for (Row = 1;
	   Row <= Marks->Header;
	   Row++)
	 Str_WriteUntilStrFoundInFileIncludingStr (FileUsrMarks,FileAllMarks,"</tr>",Str_NO_SKIP_HTML_COMMENTS);

      /****** Write the row corresponding to the student *****/
      /* Find user's ID */
      UsrIDFound = EndOfTable = false;
      while (!UsrIDFound && !EndOfTable)
	 if (Str_FindStrInFile (FileAllMarks,"<tr",Str_NO_SKIP_HTML_COMMENTS))   // Go to the next row
	   {
	    // All user's IDs must be in the first column of the row
	    Str_GetCellFromHTMLTableSkipComments (FileAllMarks,CellContent,Mrk_MAX_BYTES_IN_CELL_CONTENT);

	    /* Get user's IDs */
	    Ptr = CellContent;
	    while (*Ptr && !UsrIDFound)
	      {
	       /* Find next string in text until comma or semicolon (leading and trailing spaces are removed) */
	       Str_GetNextStringUntilSeparator (&Ptr,UsrIDFromTable,ID_MAX_LENGTH_USR_ID);

	       // Users' IDs are always stored internally in capitals and without leading zeros
	       Str_RemoveLeadingZeros (UsrIDFromTable);
	       Str_ConvertToUpperText (UsrIDFromTable);
	       if (ID_CheckIfUsrIDIsValid (UsrIDFromTable))
		  // A valid user's ID is found in the first column of table, and stored in UsrIDFromTable.
		  // Compare UsrIDFromTable with all the confirmed user's IDs in list
		  for (NumID = 0;
		       NumID < UsrDat->IDs.Num && !UsrIDFound;
		       NumID++)
		     if (UsrDat->IDs.List[NumID].Confirmed)
			if (!strcasecmp (UsrDat->IDs.List[NumID].ID,UsrIDFromTable))
			   UsrIDFound = true;
	      }
	   }
	 else
	    EndOfTable = true;	// No more rows

      if (UsrIDFound)	// This should happen always, because the check was already made
	{
	 /* Find backward until "<tr" */
	 Str_FindStrInFileBack (FileAllMarks,"<tr",Str_NO_SKIP_HTML_COMMENTS);

	 /* Write until "</tr>" */
	 Str_WriteUntilStrFoundInFileIncludingStr (FileUsrMarks,FileAllMarks,"</tr>",Str_NO_SKIP_HTML_COMMENTS);

	 /***** Write the footer and all until the end *****/
	 /* Find the footer of the table */
	 Str_FindStrInFile (FileAllMarks,"</table>",Str_NO_SKIP_HTML_COMMENTS);
	 Str_FindStrInFileBack (FileAllMarks,"</table>",Str_NO_SKIP_HTML_COMMENTS);

	 for (Row = 1;
	      Row <= Marks->Footer;
	      Row++)
	    Str_FindStrInFileBack (FileAllMarks,"<tr",Str_NO_SKIP_HTML_COMMENTS);

	 /* Write the footer of the table */
	 for (Row = 1;
	      Row <= Marks->Footer;
	      Row++)
	    Str_WriteUntilStrFoundInFileIncludingStr (FileUsrMarks,FileAllMarks,"</tr>",Str_NO_SKIP_HTML_COMMENTS);

	 /* Write the end */
	 Str_FindStrInFile (FileAllMarks,"</table>",Str_NO_SKIP_HTML_COMMENTS);
	 Str_FindStrInFileBack (FileAllMarks,"</table>",Str_NO_SKIP_HTML_COMMENTS);
	 Fil_FastCopyOfOpenFiles (FileAllMarks,FileUsrMarks);

	 /***** The file of marks is no longer needed. Close it. *****/
	 fclose (FileAllMarks);

	 return true;
	}
     }

   /***** User's ID not found in table *****/
   fclose (FileAllMarks);
   sprintf (Gbl.Message,Txt_THE_USER_X_is_not_found_in_the_file_of_marks,
	    UsrDat->FullName);
   return false;
  }

/*****************************************************************************/
/*************************** Show the marks of a user ************************/
/*****************************************************************************/

void Mrk_ShowMyMarks (void)
  {
   struct MarksProperties Marks;
   char FileNameUsrMarks[PATH_MAX + 1];
   FILE *FileUsrMarks;
   char PathMarksPriv[PATH_MAX + 1];
   char PathPrivate[PATH_MAX + 1];
   struct UsrData *UsrDat;
   bool UsrIsOK = true;

   /***** Get parameters related to file browser *****/
   Brw_GetParAndInitFileBrowser ();

   /***** Get the path of the file of marks *****/
   Brw_SetFullPathInTree (Gbl.FileBrowser.Priv.PathInTreeUntilFilFolLnk,
	                  Gbl.FileBrowser.FilFolLnkName);
   sprintf (PathPrivate,"%s/%s",Gbl.FileBrowser.Priv.PathAboveRootFolder,Gbl.FileBrowser.Priv.FullPathInTree);

   /***** Get number of rows of header or footer *****/
   Mrk_GetNumRowsHeaderAndFooter (&Marks);

   /***** Set the student whose marks will be shown *****/
   if (Gbl.Usrs.Me.LoggedRole == Rol_STUDENT)	// If I am logged as student...
      UsrDat = &Gbl.Usrs.Me.UsrDat;		// ...use my list of IDs
   else						// If I am logged as teacher, administrator, superuser...
     {
      /* Select a random student from the course */
      if (Gbl.CurrentCrs.Grps.GrpCod > 0)	// Group zone
        {
         if (Grp_CountNumStdsInGrp (Gbl.CurrentCrs.Grps.GrpCod))
           {
            Gbl.Usrs.Other.UsrDat.UsrCod = Usr_GetRamdomStdFromGrp (Gbl.CurrentCrs.Grps.GrpCod);
            UsrDat = &Gbl.Usrs.Other.UsrDat;
           }
         else
           {
            UsrIsOK = false;
            Usr_ShowWarningNoUsersFound (Rol_STUDENT);
           }
        }
      else					// Course zone
        {
         if (Gbl.CurrentCrs.Crs.NumStds)	// If there are students in this course
           {
            Gbl.Usrs.Other.UsrDat.UsrCod = Usr_GetRamdomStdFromCrs (Gbl.CurrentCrs.Crs.CrsCod);
            UsrDat = &Gbl.Usrs.Other.UsrDat;
           }
         else
           {
            UsrIsOK = false;
            Usr_ShowWarningNoUsersFound (Rol_STUDENT);
           }
        }
     }

   if (UsrIsOK)
     {
      /***** Get list of user's IDs *****/
      Usr_GetAllUsrDataFromUsrCod (UsrDat);

      /***** Create temporal file to store my marks (in HTML) *****/
      /* If the private directory does not exist, create it */
      sprintf (PathMarksPriv,"%s/%s",
               Cfg_PATH_SWAD_PRIVATE,Cfg_FOLDER_MARK);
      Fil_CreateDirIfNotExists (PathMarksPriv);

      /* First of all, we remove the oldest temporary files.
         Such temporary files have been created by me or by other users.
         This is a bit sloppy, but they must be removed by someone.
         Here "oldest" means more than x time from their creation */
      Fil_RemoveOldTmpFiles (PathMarksPriv,Cfg_TIME_TO_DELETE_MARKS_TMP_FILES,false);

      /* Create a new temporary file *****/
      sprintf (FileNameUsrMarks,"%s/%s.html",PathMarksPriv,Gbl.UniqueNameEncrypted);
      if ((FileUsrMarks = fopen (FileNameUsrMarks,"wb")) == NULL)
         Lay_ShowErrorAndExit ("Can not open file for my marks.");

      /***** Show my marks *****/
      if (Mrk_GetUsrMarks (FileUsrMarks,UsrDat,PathPrivate,&Marks))
        {
         fclose (FileUsrMarks);
         if ((FileUsrMarks = fopen (FileNameUsrMarks,"rb")) == NULL)
            Lay_ShowErrorAndExit ("Can not open file with my marks.");

         /* Start HTML output */
         fprintf (Gbl.F.Out,"Content-type: text/html; charset=windows-1252\r\n\r\n"); // Two \r\n aree necessary
         Gbl.Layout.HTMLStartWritten = true;

         /* Copy HTML to output file */
         Fil_FastCopyOfOpenFiles (FileUsrMarks,Gbl.F.Out);
         fclose (FileUsrMarks);

         Gbl.Layout.DivsEndWritten = Gbl.Layout.HTMLEndWritten = true;
        }
      else	// Problems in table of marks or user's ID not found
        {
         fclose (FileUsrMarks);
	 Lay_ShowAlert (Lay_WARNING,Gbl.Message);
        }

      unlink (FileNameUsrMarks);	// File with marks is no longer necessary
     }
  }

/*****************************************************************************/
/******************** Put my marks into a notification ***********************/
/*****************************************************************************/
// This function may be called inside a web service, so don't report error

void Mrk_GetNotifMyMarks (char SummaryStr[Cns_MAX_BYTES_TEXT + 1],
                          char **ContentStr,
                          long MrkCod,long UsrCod,
                          unsigned MaxChars,bool GetContent)
  {
   char Query[512];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   struct UsrData UsrDat;
   unsigned UnsignedNum;
   Brw_FileBrowser_t FileBrowser;
   long Cod;
   long InsCod;	// Not applicable here
   long CtrCod;	// Not applicable here
   long DegCod;	// Not applicable here
   long CrsCod;
   long GrpCod;
   struct MarksProperties Marks;
   char FullPathInTreeFromDBMarksTable[PATH_MAX + 1];
   char PathUntilFileName[PATH_MAX + 1];
   char FileName[NAME_MAX + 1];
   char PathMarksPriv[PATH_MAX + 1];
   char PathMarks[PATH_MAX + 1];
   char FileNameUsrMarks[PATH_MAX + 1];
   FILE *FileUsrMarks;
   size_t SizeOfMyMarks;
   size_t Length;
   size_t i;
   char *Ptr;

   SummaryStr[0] = '\0';	// Return nothing on error

   /***** Initialize structure with user's data *****/
   Usr_UsrDataConstructor (&UsrDat);

   /***** Get user's ID from user's code *****/
   UsrDat.UsrCod = UsrCod;
   ID_GetListIDsFromUsrCod (&UsrDat);

   /***** Get subject of message from database *****/
   sprintf (Query,"SELECT files.FileBrowser,files.Cod,files.Path,"
	          "marks_properties.Header,marks_properties.Footer"
	          " FROM files,marks_properties"
	          " WHERE files.FilCod='%ld'"
	          " AND files.FilCod=marks_properties.FilCod",
	    MrkCod);
   if (!mysql_query (&Gbl.mysql,Query))
      if ((mysql_res = mysql_store_result (&Gbl.mysql)) != NULL)
        {
         /***** Result should have a unique row *****/
         if (mysql_num_rows (mysql_res) == 1)
           {
            /***** Get data of this file of marks *****/
            row = mysql_fetch_row (mysql_res);

	    /* Get file browser type in database (row[0]) */
	    FileBrowser = Brw_UNKNOWN;
	    if (sscanf (row[0],"%u",&UnsignedNum) == 1)
	       if (UnsignedNum < Brw_NUM_TYPES_FILE_BROWSER)
		  FileBrowser = (Brw_FileBrowser_t) UnsignedNum;

            /* Course/group code (row[1]) */
            Cod = Str_ConvertStrCodToLongCod (row[1]);
            Brw_GetCrsGrpFromFileMetadata (FileBrowser,Cod,
                                           &InsCod,&CtrCod,&DegCod,&CrsCod,&GrpCod);

            /* Path (row[2]) */
            Str_Copy (FullPathInTreeFromDBMarksTable,row[2],
                      PATH_MAX);
            Str_SplitFullPathIntoPathAndFileName (FullPathInTreeFromDBMarksTable,
        	                                  PathUntilFileName,
        	                                  FileName);
            Str_Copy (SummaryStr,FileName,
                      Cns_MAX_BYTES_TEXT);
            if (MaxChars)
               Str_LimitLengthHTMLStr (SummaryStr,MaxChars);

            if (GetContent)
              {
               /* Header (row[3]) */
               if (sscanf (row[3],"%u",&(Marks.Header)) != 1)
                  Lay_ShowErrorAndExit ("Wrong number of header rows.");

               /* Footer (row[4]) */
               if (sscanf (row[4],"%u",&(Marks.Footer)) != 1)
                  Lay_ShowErrorAndExit ("Wrong number of footer rows.");

               if (UsrDat.IDs.Num)
                 {
                  if (GrpCod > 0)
                     sprintf (PathMarks,"%s/%s/%ld/grp/%ld/%s",
                              Cfg_PATH_SWAD_PRIVATE,Cfg_FOLDER_CRS,CrsCod,GrpCod,
                              FullPathInTreeFromDBMarksTable);
                  else
                     sprintf (PathMarks,"%s/%s/%ld/%s",
                              Cfg_PATH_SWAD_PRIVATE,Cfg_FOLDER_CRS,CrsCod,
                              FullPathInTreeFromDBMarksTable);

                  /***** Create temporal file to store my marks (in HTML) *****/
                  /* If the private directory does not exist, create it */
                  sprintf (PathMarksPriv,"%s/%s",
                           Cfg_PATH_SWAD_PRIVATE,Cfg_FOLDER_MARK);
                  Fil_CreateDirIfNotExists (PathMarksPriv);

                  /* First of all, we remove the oldest temporary files.
                     Such temporary files have been created by me or by other users.
                     This is a bit sloppy, but they must be removed by someone.
                     Here "oldest" means more than x time from their creation */
                  Fil_RemoveOldTmpFiles (PathMarksPriv,Cfg_TIME_TO_DELETE_MARKS_TMP_FILES,false);

                  /* Create a new temporary file *****/
                  sprintf (FileNameUsrMarks,"%s/%s.html",PathMarksPriv,Gbl.UniqueNameEncrypted);
                  if ((FileUsrMarks = fopen (FileNameUsrMarks,"wb")))
                    {
                     /***** Get user's marks *****/
                     if (Mrk_GetUsrMarks (FileUsrMarks,&UsrDat,PathMarks,&Marks))
                       {
                        SizeOfMyMarks = (size_t) ftell (FileUsrMarks);
                        fclose (FileUsrMarks);

                        Length = 9 + SizeOfMyMarks + 3;
                        if ((*ContentStr = (char *) malloc (Length + 1)))
                          {
                           /* 9 starting chars */
                           Str_Copy (*ContentStr,"<![CDATA[",
                                     9);

                           /* Content */
                           Ptr = (*ContentStr) + 9;
                           if ((FileUsrMarks = fopen (FileNameUsrMarks,"rb")))
                             {
                              for (i = 0;
                        	   i < SizeOfMyMarks;
                        	   i++)
                                 *Ptr++ = (char) fgetc (FileUsrMarks);
                              fclose (FileUsrMarks);
                             }

                           /* 3 ending chars */
                           Str_Copy (Ptr,"]]>",
                                     3);
                          }
                       }
                     else
                       {
                        fclose (FileUsrMarks);
                        if ((*ContentStr = (char *) malloc (9 + strlen (Gbl.Message) + 3 + 1)))
                           sprintf (*ContentStr,"<![CDATA[%s]]>",Gbl.Message);
                       }
                    }
                  else
                    {
                     sprintf (Gbl.Message,"%s","Can not open file with user's marks!");	// TODO: Need translation!
                     if ((*ContentStr = (char *) malloc (9 + strlen (Gbl.Message) + 3 + 1)))
                        sprintf (*ContentStr,"<![CDATA[%s]]>",Gbl.Message);
                    }
                  unlink (FileNameUsrMarks);	// File with marks is no longer necessary
                 }
               else
                 {
                  sprintf (Gbl.Message,"%s","User's IDs not found!");	// TODO: Need translation!
                  if ((*ContentStr = (char *) malloc (9 + strlen (Gbl.Message) + 3 + 1)))
                     sprintf (*ContentStr,"<![CDATA[%s]]>",Gbl.Message);
                 }
              }
           }

         mysql_free_result (mysql_res);
        }

   /***** Free memory used for user's data *****/
   Usr_UsrDataDestructor (&UsrDat);
  }
