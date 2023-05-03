// swad_mark.c: marks

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

#define _GNU_SOURCE 		// For asprintf
#include <linux/limits.h>	// For PATH_MAX
#include <malloc.h>		// For malloc
#include <stddef.h>		// For NULL
#include <stdio.h>		// For asprintf
#include <string.h>		// For string functions
#include <unistd.h>		// For unlink

#include "swad_action_list.h"
#include "swad_alert.h"
#include "swad_database.h"
#include "swad_enrolment_database.h"
#include "swad_error.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_group_database.h"
#include "swad_hierarchy_level.h"
#include "swad_HTML.h"
#include "swad_ID.h"
#include "swad_mark.h"
#include "swad_mark_database.h"
#include "swad_notification.h"
#include "swad_parameter.h"
#include "swad_string.h"
#include "swad_user.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/***************************** Private constants *****************************/
/*****************************************************************************/

static const char *Mrk_HeadOrFootStr[2] =	// Names of fields in forms
  {
   "Header",
   "Footer",
  };

#define Mrk_MAX_BYTES_IN_CELL_CONTENT	1024	// Cell of a table containing one or several user's IDs

/*****************************************************************************/
/**************************** Private prototypes *****************************/
/*****************************************************************************/

static void Mrk_GetNumRowsHeaderAndFooter (struct Mrk_Properties *Marks);
static void Mrk_ChangeNumRowsHeaderOrFooter (Brw_HeadOrFoot_t HeaderOrFooter);
static bool Mrk_CheckIfCellContainsOnlyIDs (const char *CellContent);
static bool Mrk_GetUsrMarks (FILE *FileUsrMarks,struct Usr_Data *UsrDat,
                             const char *PathFileAllMarks,
                             struct Mrk_Properties *Marks);

/*****************************************************************************/
/********* Write number of header and footer rows of a file of marks *********/
/*****************************************************************************/

void Mrk_GetAndWriteNumRowsHeaderAndFooter (void)
  {
   extern const char *Txt_TABLE_Header;
   extern const char *Txt_TABLE_Footer;
   struct Mrk_Properties Marks;
   char StrHeadOrFoot[Cns_MAX_DECIMAL_DIGITS_UINT + 1];

   if (Gbl.FileBrowser.FilFolLnk.Type == Brw_IS_FOLDER)
      HTM_TD_ColouredEmpty (2);
   else	// File or link
     {
      /***** Get number of rows in header or footer *****/
      Mrk_GetNumRowsHeaderAndFooter (&Marks);

      /***** Write the number of rows of header *****/
      HTM_TD_Begin ("class=\"RT FORM_IN_%s NOWRAP %s\"",
		    The_GetSuffix (),
		    The_GetColorRows ());
	 Frm_BeginForm (Gbl.Crs.Grps.GrpCod > 0 ? ActChgNumRowHeaGrp :	// Group zone
						  ActChgNumRowHeaCrs);	// Course zone
	    Brw_PutImplicitParsFileBrowser (&Gbl.FileBrowser.FilFolLnk);
	    HTM_LABEL_Begin (NULL);
	       HTM_TxtF ("&nbsp;%s: ",Txt_TABLE_Header);
	       snprintf (StrHeadOrFoot,sizeof (StrHeadOrFoot),"%u",Marks.Header);
	       HTM_INPUT_TEXT (Mrk_HeadOrFootStr[Brw_HEADER],Cns_MAX_DECIMAL_DIGITS_UINT,StrHeadOrFoot,
			       HTM_SUBMIT_ON_CHANGE,
			       "size=\"1\" class=\"LST_EDIT_ROWS LST_EDIT_%s %s\"",
			       The_GetSuffix (),The_GetColorRows ());
	    HTM_LABEL_End ();
	 Frm_EndForm ();
      HTM_TD_End ();

      /***** Write the number of rows of footer *****/
      HTM_TD_Begin ("class=\"RT FORM_IN_%s NOWRAP %s\"",
		    The_GetSuffix (),
		    The_GetColorRows ());
	 Frm_BeginForm (Gbl.Crs.Grps.GrpCod > 0 ? ActChgNumRowFooGrp :	// Group zone
						  ActChgNumRowFooCrs);	// Course zone
	    Brw_PutImplicitParsFileBrowser (&Gbl.FileBrowser.FilFolLnk);
	    HTM_LABEL_Begin (NULL);
	       HTM_TxtF ("&nbsp;%s: ",Txt_TABLE_Footer);
	       snprintf (StrHeadOrFoot,sizeof (StrHeadOrFoot),"%u",Marks.Footer);
	       HTM_INPUT_TEXT (Mrk_HeadOrFootStr[Brw_FOOTER],Cns_MAX_DECIMAL_DIGITS_UINT,StrHeadOrFoot,
			       HTM_SUBMIT_ON_CHANGE,
			       "size=\"1\" class=\"LST_EDIT_ROWS LST_EDIT_%s %s\"",
			       The_GetSuffix (),The_GetColorRows ());
	    HTM_LABEL_End ();
	 Frm_EndForm ();
      HTM_TD_End ();
     }
  }

/*****************************************************************************/
/******** Get number of rows of header and of footer of a file of marks ******/
/*****************************************************************************/

static void Mrk_GetNumRowsHeaderAndFooter (struct Mrk_Properties *Marks)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;

   /***** Get number of rows of header and footer from database *****/
   /* There should be a single file in database.
      If, due to an error, there is more than one file,
      get the number of rows of the more recent file. */
   if (Mrk_DB_GetNumRowsHeaderAndFooter (&mysql_res) == 1)
     {
      /***** Get number of header and footer rows *****/
      row = mysql_fetch_row (mysql_res);

      /* Header (row[0]) */
      if (sscanf (row[0],"%u",&(Marks->Header)) != 1)
         Err_WrongNumberOfRowsExit ();

      /* Footer (row[1]) */
      if (sscanf (row[1],"%u",&(Marks->Footer)) != 1)
         Err_WrongNumberOfRowsExit ();
     }
   else	// Unknown numbers of header and footer rows
      Marks->Header =
      Marks->Footer = 0;

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
   extern const char *Txt_The_number_of_rows_is_now_X;
   char UnsignedStr[Cns_MAX_DECIMAL_DIGITS_UINT + 1];
   unsigned NumRows;

   /***** Get parameters related to file browser *****/
   Brw_GetParAndInitFileBrowser ();

   /***** Get the number of rows of the header or footer of the table of marks *****/
   Par_GetParText (Mrk_HeadOrFootStr[HeaderOrFooter],UnsignedStr,Cns_MAX_DECIMAL_DIGITS_UINT);
   if (sscanf (UnsignedStr,"%u",&NumRows) == 1)
     {
      /***** Update properties of marks in the database *****/
      Mrk_DB_ChangeNumRowsHeaderOrFooter (HeaderOrFooter,NumRows);

      /***** Write message of success *****/
      Ale_ShowAlert (Ale_SUCCESS,Txt_The_number_of_rows_is_now_X,
                     NumRows);
     }
   else
      Err_WrongNumberOfRowsExit ();

   /***** Show again the file browser *****/
   Brw_ShowFileBrowserNormal ();
  }

/*****************************************************************************/
/************************ Receive a new file of marks ************************/
/*****************************************************************************/
// Returns true if the format of the HTML file of marks is correct
// Returns true if the format of the HTML file of marks is wrong

bool Mrk_CheckFileOfMarks (const char *Path,struct Mrk_Properties *Marks)
  {
   extern const char *Txt_There_are_more_than_one_table_in_the_file_of_marks;
   extern const char *Txt_Table_not_found_in_the_file_of_marks;
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
            Ale_CreateAlert (Ale_WARNING,NULL,
        		     Txt_There_are_more_than_one_table_in_the_file_of_marks);
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
         Ale_CreateAlert (Ale_WARNING,NULL,
			  Txt_Table_not_found_in_the_file_of_marks);
         FileIsCorrect = false;
        }

      /***** The file of marks is no more necessary. Close it. *****/
      fclose (FileAllMarks);
     }

   return FileIsCorrect;
  }

/*****************************************************************************/
/******* Check if only user's IDs or other stuff found in a table cell *******/
/*****************************************************************************/

static bool Mrk_CheckIfCellContainsOnlyIDs (const char *CellContent)
  {
   char UsrIDFromTable[ID_MAX_BYTES_USR_ID + 1];
   const char *Ptr = CellContent;
   bool UsrIDFound = false;
   bool StuffNotUsrIDFound = false;

   /***** Get strings in this table cell
          and check if they look like user's IDs or not *****/
   while (*Ptr && !StuffNotUsrIDFound)
     {
      /* Find next string in text until space, comma or semicolon (leading and trailing spaces are removed) */
      Str_GetNextStringUntilSeparator (&Ptr,UsrIDFromTable,ID_MAX_BYTES_USR_ID);

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

static bool Mrk_GetUsrMarks (FILE *FileUsrMarks,struct Usr_Data *UsrDat,
                             const char *PathFileAllMarks,
                             struct Mrk_Properties *Marks)
  {
   extern const char *Txt_THE_USER_X_is_not_found_in_the_file_of_marks;
   unsigned Row;
   char CellContent[Mrk_MAX_BYTES_IN_CELL_CONTENT + 1];
   const char *Ptr;
   char UsrIDFromTable[ID_MAX_BYTES_USR_ID + 1];
   FILE *FileAllMarks;
   unsigned NumID;
   bool UsrIDFound;
   bool EndOfTable;

   /***** Open HTML file with the table of marks *****/
   if (!(FileAllMarks = fopen (PathFileAllMarks,"rb")))
     {  // Can't open the file with the table of marks
      Ale_CreateAlert (Ale_ERROR,NULL,
		       "Can not open file of marks.");
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
	    /* Find next string in text until separator (leading and trailing spaces are removed) */
            Str_GetNextStringUntilSeparator (&Ptr,UsrIDFromTable,ID_MAX_BYTES_USR_ID);

	    // Users' IDs are always stored internally in capitals and without leading zeros
	    Str_RemoveLeadingZeros (UsrIDFromTable);
	    Str_ConvertToUpperText (UsrIDFromTable);

	    if (ID_CheckIfUsrIDIsValid (UsrIDFromTable))
	       // A valid user's ID is found in the first column of table, and stored in UsrIDFromTable.
	       // Compare UsrIDFromTable with all confirmed user's IDs in list
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
	       Str_GetNextStringUntilSeparator (&Ptr,UsrIDFromTable,ID_MAX_BYTES_USR_ID);

	       // Users' IDs are always stored internally in capitals and without leading zeros
	       Str_RemoveLeadingZeros (UsrIDFromTable);
	       Str_ConvertToUpperText (UsrIDFromTable);
	       if (ID_CheckIfUsrIDIsValid (UsrIDFromTable))
		  // A valid user's ID is found in the first column of table, and stored in UsrIDFromTable.
		  // Compare UsrIDFromTable with all confirmed user's IDs in list
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
   Ale_CreateAlert (Ale_WARNING,NULL,
		    Txt_THE_USER_X_is_not_found_in_the_file_of_marks,
		    UsrDat->FullName);
   return false;
  }

/*****************************************************************************/
/*************************** Show the marks of a user ************************/
/*****************************************************************************/

void Mrk_ShowMyMarks (void)
  {
   struct Mrk_Properties Marks;
   char FileNameUsrMarks[PATH_MAX + 1];
   FILE *FileUsrMarks;
   char PathPrivate[PATH_MAX + 1 +
		    PATH_MAX + 1];
   struct Usr_Data *UsrDat;
   bool UsrIsOK = true;

   /***** Get parameters related to file browser *****/
   Brw_GetParAndInitFileBrowser ();

   /***** Get the path of the file of marks *****/
   Brw_SetFullPathInTree ();
   snprintf (PathPrivate,sizeof (PathPrivate),"%s/%s",
             Gbl.FileBrowser.Priv.PathAboveRootFolder,
             Gbl.FileBrowser.FilFolLnk.Full);

   /***** Get number of rows of header or footer *****/
   Mrk_GetNumRowsHeaderAndFooter (&Marks);

   /***** Set the student whose marks will be shown *****/
   if (Gbl.Usrs.Me.Role.Logged == Rol_STD)	// If I am logged as student...
      UsrDat = &Gbl.Usrs.Me.UsrDat;		// ...use my list of IDs
   else						// If I am logged as non-editing teacher, teacher or admin
     {
      /* Select a random student from the course */
      if (Gbl.Crs.Grps.GrpCod > 0)	// Group zone
        {
         if (Grp_DB_CountNumUsrsInGrp (Rol_STD,Gbl.Crs.Grps.GrpCod))	// If there are students in this group
           {
            Gbl.Usrs.Other.UsrDat.UsrCod = Grp_DB_GetRamdomStdFromGrp (Gbl.Crs.Grps.GrpCod);
            UsrDat = &Gbl.Usrs.Other.UsrDat;
           }
         else
            UsrIsOK = false;
        }
      else				// Course zone
        {
	 if (Enr_GetNumUsrsInCrss (HieLvl_CRS,Gbl.Hierarchy.Crs.CrsCod,
				   1 << Rol_STD))	// If there are students in this course
           {
            Gbl.Usrs.Other.UsrDat.UsrCod = Enr_DB_GetRamdomStdFromCrs (Gbl.Hierarchy.Crs.CrsCod);
            UsrDat = &Gbl.Usrs.Other.UsrDat;
           }
         else
            UsrIsOK = false;
        }
     }

   if (UsrIsOK)
     {
      /***** Get list of user's IDs *****/
      Usr_GetAllUsrDataFromUsrCod (UsrDat,
                                   Usr_DONT_GET_PREFS,
                                   Usr_DONT_GET_ROLE_IN_CURRENT_CRS);

      /***** Create temporal file to store my marks (in HTML) *****/
      /* If the private directory does not exist, create it */
      Fil_CreateDirIfNotExists (Cfg_PATH_MARK_PRIVATE);

      /* Create a new temporary file *****/
      snprintf (FileNameUsrMarks,sizeof (FileNameUsrMarks),"%s/%s.html",
		Cfg_PATH_MARK_PRIVATE,Cry_GetUniqueNameEncrypted ());
      if ((FileUsrMarks = fopen (FileNameUsrMarks,"wb")) == NULL)
         Err_ShowErrorAndExit ("Can not open file for my marks.");

      /***** Show my marks *****/
      if (Mrk_GetUsrMarks (FileUsrMarks,UsrDat,PathPrivate,&Marks))
        {
         fclose (FileUsrMarks);
         if ((FileUsrMarks = fopen (FileNameUsrMarks,"rb")) == NULL)
            Err_ShowErrorAndExit ("Can not open file with my marks.");

         /* Begin HTML output */
         /*  Do not write charset here.
             Instead, delegate to the meta directive
             (example: <meta http-equiv=Content-Type content="text/html; charset=utf-8">)
             that is typically included in the HTML document header. */
         HTM_Txt ("Content-type: text/html\r\n\r\n"); // Two \r\n are necessary
         Gbl.Layout.HTMLStartWritten = true;

         /* Copy HTML to output file */
         Fil_FastCopyOfOpenFiles (FileUsrMarks,Fil_GetOutputFile ());
         fclose (FileUsrMarks);

         Gbl.Layout.DivsEndWritten = Gbl.Layout.HTMLEndWritten = true;
        }
      else	// Problems in table of marks or user's ID not found
        {
         fclose (FileUsrMarks);
	 Ale_ShowAlerts (NULL);
        }

      unlink (FileNameUsrMarks);	// File with marks is no longer necessary
     }
   else
      /***** Show warning indicating no students found *****/
      Usr_ShowWarningNoUsersFound (Rol_STD);
  }

/*****************************************************************************/
/******************** Put my marks into a notification ***********************/
/*****************************************************************************/

void Mrk_GetNotifMyMarks (char SummaryStr[Ntf_MAX_BYTES_SUMMARY + 1],
                          char **ContentStr,
                          long MrkCod,long UsrCod,bool GetContent)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   struct Usr_Data UsrDat;
   unsigned UnsignedNum;
   Brw_FileBrowser_t FileBrowser;
   long Cod;
   long InsCod;	// Not applicable here
   long CtrCod;	// Not applicable here
   long DegCod;	// Not applicable here
   long CrsCod;
   long GrpCod;
   struct Mrk_Properties Marks;
   char FullPathInTreeFromDBMarksTable[PATH_MAX + 1];
   char PathUntilFileName[PATH_MAX + 1];
   char FileName[NAME_MAX + 1];
   char PathMarks[PATH_MAX + 1 + Cns_MAX_DECIMAL_DIGITS_LONG + 1 + 3 + 1 + Cns_MAX_DECIMAL_DIGITS_LONG + 1 + PATH_MAX + 1];
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

   /***** Get marks data from database *****/
   if (Mrk_DB_GetMarksDataByCod (&mysql_res,MrkCod) == 1)	// Result should have a unique row
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
		sizeof (FullPathInTreeFromDBMarksTable) - 1);
      Str_SplitFullPathIntoPathAndFileName (FullPathInTreeFromDBMarksTable,
					    PathUntilFileName,
					    FileName);
      Str_Copy (SummaryStr,FileName,Cns_MAX_BYTES_TEXT);

      if (GetContent)
	{
	 /* Header (row[3]) */
	 if (sscanf (row[3],"%u",&(Marks.Header)) != 1)
	    Err_WrongNumberOfRowsExit ();

	 /* Footer (row[4]) */
	 if (sscanf (row[4],"%u",&(Marks.Footer)) != 1)
	    Err_WrongNumberOfRowsExit ();

	 if (UsrDat.IDs.Num)
	   {
	    if (GrpCod > 0)
	       snprintf (PathMarks,sizeof (PathMarks),"%s/%ld/grp/%ld/%s",
			 Cfg_PATH_CRS_PRIVATE,CrsCod,GrpCod,
			 FullPathInTreeFromDBMarksTable);
	    else
	       snprintf (PathMarks,sizeof (PathMarks),"%s/%ld/%s",
			 Cfg_PATH_CRS_PRIVATE,CrsCod,
			 FullPathInTreeFromDBMarksTable);

	    /***** Create temporal file to store my marks (in HTML) *****/
	    /* If the private directory does not exist, create it */
	    Fil_CreateDirIfNotExists (Cfg_PATH_MARK_PRIVATE);

	    /* Create a new temporary file *****/
	    snprintf (FileNameUsrMarks,sizeof (FileNameUsrMarks),"%s/%s.html",
		      Cfg_PATH_MARK_PRIVATE,Cry_GetUniqueNameEncrypted ());
	    if ((FileUsrMarks = fopen (FileNameUsrMarks,"wb")))
	      {
	       /***** Get user's marks *****/
	       if (Mrk_GetUsrMarks (FileUsrMarks,&UsrDat,PathMarks,&Marks))
		 {
		  SizeOfMyMarks = (size_t) ftell (FileUsrMarks);
		  fclose (FileUsrMarks);

		  Length = 9 + SizeOfMyMarks + 3;
		  if ((*ContentStr = malloc (Length + 1)))
		    {
		     /* 9 starting chars */
		     Str_Copy (*ContentStr,"<![CDATA[",9);

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
		     Str_Copy (Ptr,"]]>",3);
		    }
		 }
	       else
		 {
		  fclose (FileUsrMarks);
		  if (asprintf (ContentStr,"<![CDATA[%s]]>",
				Ale_GetTextOfLastAlert ()) < 0)
		     Err_NotEnoughMemoryExit ();
	          Ale_ResetAllAlerts ();
		 }
	      }
	    else
	      {
	       if (asprintf (ContentStr,"<![CDATA[%s]]>",
			     "Can not open file of marks.") < 0)
		  Err_NotEnoughMemoryExit ();
	      }
	    unlink (FileNameUsrMarks);	// File with marks is no longer necessary
	   }
	 else
	   {
	    if (asprintf (ContentStr,"<![CDATA[%s]]>",
		          "User's IDs not found!") < 0)
	       Err_NotEnoughMemoryExit ();
	   }
	}
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   /***** Free memory used for user's data *****/
   Usr_UsrDataDestructor (&UsrDat);
  }
