// swad_test_import.c: import and export self-assessment tests using XML files

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2018 Antonio Cañas Vargas

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

#define _GNU_SOURCE 		// For asprintf
#include <stdio.h>		// For asprintf
#include <stdlib.h>		// For exit, system, malloc, free, etc
#include <string.h>		// For string functions
#include <sys/stat.h>		// For mkdir
#include <sys/types.h>		// For mkdir

#include "swad_box.h"
#include "swad_database.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_parameter.h"
#include "swad_table.h"
#include "swad_test.h"
#include "swad_xml.h"

/*****************************************************************************/
/***************************** Public constants ******************************/
/*****************************************************************************/

/*****************************************************************************/
/**************************** Private constants ******************************/
/*****************************************************************************/

/*****************************************************************************/
/******************************* Internal types ******************************/
/*****************************************************************************/

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/************************* Internal global variables *************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Internal prototypes ***************************/
/*****************************************************************************/

static void TsI_PutParamsExportQsts (void);

static void TsI_GetAndWriteTagsXML (long QstCod);
static void TsI_WriteAnswersOfAQstXML (long QstCod);
static void TsI_ReadQuestionsFromXMLFileAndStoreInDB (const char *FileNameXML);
static void TsI_ImportQuestionsFromXMLBuffer (const char *XMLBuffer);
static Tst_AnswerType_t TsI_ConvertFromStrAnsTypXMLToAnsTyp (const char *StrAnsTypeXML);
static bool TsI_CheckIfQuestionExistsInDB (void);
static void TsI_GetAnswerFromXML (struct XMLElement *AnswerElem);
static void TsI_WriteHeadingListImportedQst (void);
static void TsI_WriteRowImportedQst (struct XMLElement *StemElem,
                                     struct XMLElement *FeedbackElem,
                                     bool QuestionExists);

/*****************************************************************************/
/**************** Put a link (form) to export test questions *****************/
/*****************************************************************************/

void TsI_PutFormToExportQuestions (void)
  {
   extern const char *Txt_Export_questions_to_XML_file;
   extern const char *Txt_Export_questions;

   /***** Put a link to create a file with questions *****/
   Lay_PutContextualLink (ActLstTstQst,NULL,TsI_PutParamsExportQsts,
		          "export16x16.gif",
		          Txt_Export_questions_to_XML_file,Txt_Export_questions,
		          NULL);
  }

/*****************************************************************************/
/****************** Put params to export test questions **********************/
/*****************************************************************************/

static void TsI_PutParamsExportQsts (void)
  {
   Sta_WriteParamsDatesSeeAccesses ();
   Tst_WriteParamEditQst ();
   Par_PutHiddenParamChar ("OnlyThisQst",'N');
   Par_PutHiddenParamUnsigned ("Order",(unsigned) Gbl.Test.SelectedOrder);
   Par_PutHiddenParamChar ("CreateXML",'Y');
  }

/*****************************************************************************/
/*************** Put a link (form) to import test questions ******************/
/*****************************************************************************/

void TsI_PutFormToImportQuestions (void)
  {
   extern const char *Txt_Import_questions_from_XML_file;
   extern const char *Txt_Import_questions;

   /***** Put a link to create a file with questions *****/
   Lay_PutContextualLink (ActReqImpTstQst,NULL,NULL,
		          "import16x16.gif",
		          Txt_Import_questions_from_XML_file,
                          Txt_Import_questions,
		          NULL);
  }

/*****************************************************************************/
/*********** Show form to import test questions from an XML file *************/
/*****************************************************************************/

void TsI_ShowFormImportQstsFromXML (void)
  {
   extern const char *Hlp_ASSESSMENT_Tests;
   extern const char *The_ClassForm[The_NUM_THEMES];
   extern const char *Txt_Import_questions;
   extern const char *Txt_You_need_an_XML_file_containing_a_list_of_questions;
   extern const char *Txt_XML_file;

   /***** Start box *****/
   Box_StartBox (NULL,Txt_Import_questions,NULL,
                 Hlp_ASSESSMENT_Tests,Box_NOT_CLOSABLE);

   /***** Write help message *****/
   Ale_ShowAlert (Ale_INFO,Txt_You_need_an_XML_file_containing_a_list_of_questions);

   /***** Write a form to import questions *****/
   Frm_StartForm (ActImpTstQst);
   fprintf (Gbl.F.Out,"<label class=\"%s\">"
                      "%s:&nbsp;"
                      "<input type=\"file\" name=\"%s\" accept=\".xml\""
                      " onchange=\"document.getElementById('%s').submit();\" />"
                      "</label>",
            The_ClassForm[Gbl.Prefs.Theme],
            Txt_XML_file,
            Fil_NAME_OF_PARAM_FILENAME_ORG,
            Gbl.Form.Id);
   Frm_EndForm ();

   /***** End box *****/
   Box_EndBox ();
  }

/*****************************************************************************/
/*** Create the XML file with test questions and put a link to download it ***/
/*****************************************************************************/

void TsI_CreateXML (unsigned long NumRows,MYSQL_RES *mysql_res)
  {
   extern const char *The_ClassFormBold[The_NUM_THEMES];
   extern const char *Tst_StrAnswerTypesXML[Tst_NUM_ANS_TYPES];
   extern const char *Txt_NEW_LINE;
   extern const char *Txt_XML_file;
   char PathPubFile[PATH_MAX + 1];
   unsigned long NumRow;
   MYSQL_ROW row;
   long QstCod;

   /***** Create a temporary public directory
	  used to download the XML file *****/
   Brw_CreateDirDownloadTmp ();

   /***** Create public XML file with the questions *****/
   snprintf (PathPubFile,sizeof (PathPubFile),
	     "%s/%s/%s/test.xml",
             Cfg_PATH_SWAD_PUBLIC,Cfg_FOLDER_FILE_BROWSER_TMP,
             Gbl.FileBrowser.TmpPubDir);
   if ((Gbl.Test.XML.FileXML = fopen (PathPubFile,"wb")) == NULL)
      Lay_ShowErrorAndExit ("Can not open target file.");

   /***** Start XML file *****/
   XML_WriteStartFile (Gbl.Test.XML.FileXML,"test",false);
   fprintf (Gbl.Test.XML.FileXML,"%s",Txt_NEW_LINE);

   /***** Write rows *****/
   for (NumRow = 0;
	NumRow < NumRows;
	NumRow++)
     {
      row = mysql_fetch_row (mysql_res);
      /*
      row[ 0] QstCod
      row[ 1] UNIX_TIMESTAMP(EditTime)
      row[ 2] AnsType
      row[ 3] Shuffle
      row[ 4] Stem
      row[ 5] Feedback
      row[ 6] ImageName
      row[ 7] ImageTitle
      row[ 8] ImageURL
      row[ 9] NumHits
      row[10] NumHitsNotBlank
      row[11] Score
      */
      /* row[0] holds the code of the question */
      if ((QstCod = Str_ConvertStrCodToLongCod (row[0])) < 0)
         Lay_ShowErrorAndExit ("Wrong code of question.");

      /* Write the question type (row[2]) */
      Gbl.Test.AnswerType = Tst_ConvertFromStrAnsTypDBToAnsTyp (row[2]);
      fprintf (Gbl.Test.XML.FileXML,"<question type=\"%s\">%s",
               Tst_StrAnswerTypesXML[Gbl.Test.AnswerType],Txt_NEW_LINE);

      /* Write the question tags */
      fprintf (Gbl.Test.XML.FileXML,"<tags>%s",Txt_NEW_LINE);
      TsI_GetAndWriteTagsXML (QstCod);
      fprintf (Gbl.Test.XML.FileXML,"</tags>%s",Txt_NEW_LINE);

      /* Write the stem (row[4]), that is in HTML format */
      fprintf (Gbl.Test.XML.FileXML,"<stem>%s</stem>%s",
               row[4],Txt_NEW_LINE);

      /* Write the feedback (row[5]), that is in HTML format */
      if (row[5])
	 if (row[5][0])
	    fprintf (Gbl.Test.XML.FileXML,"<feedback>%s</feedback>%s",
		     row[5],Txt_NEW_LINE);

      /* Write the answers of this question.
         Shuffle can be enabled or disabled (row[3]) */
      fprintf (Gbl.Test.XML.FileXML,"<answer");
      if (Gbl.Test.AnswerType == Tst_ANS_UNIQUE_CHOICE ||
          Gbl.Test.AnswerType == Tst_ANS_MULTIPLE_CHOICE)
         fprintf (Gbl.Test.XML.FileXML," shuffle=\"%s\"",
                  (row[3][0] == 'Y') ? "yes" :
                	               "no");
      fprintf (Gbl.Test.XML.FileXML,">");
      TsI_WriteAnswersOfAQstXML (QstCod);
      fprintf (Gbl.Test.XML.FileXML,"</answer>%s",Txt_NEW_LINE);

      /* End question */
      fprintf (Gbl.Test.XML.FileXML,"</question>%s%s",
               Txt_NEW_LINE,Txt_NEW_LINE);
     }

   /***** End XML file *****/
   XML_WriteEndFile (Gbl.Test.XML.FileXML,"test");

   /***** Close the XML file *****/
   fclose (Gbl.Test.XML.FileXML);

   /***** Return to start of query result *****/
   mysql_data_seek (mysql_res,0);

   /***** Write the link to XML file *****/
   fprintf (Gbl.F.Out,"<a href=\"%s/%s/%s/test.xml\" class=\"%s\" target=\"_blank\">",
            Cfg_URL_SWAD_PUBLIC,Cfg_FOLDER_FILE_BROWSER_TMP,
            Gbl.FileBrowser.TmpPubDir,
            The_ClassFormBold[Gbl.Prefs.Theme]);
   Ico_PutIconWithText ("file64x64.gif",Txt_XML_file,Txt_XML_file);
   fprintf (Gbl.F.Out,"</a>");
  }

/*****************************************************************************/
/************* Get and write tags of a question into the XML file ************/
/*****************************************************************************/

static void TsI_GetAndWriteTagsXML (long QstCod)
  {
   extern const char *Txt_NEW_LINE;
   unsigned long NumRow;
   unsigned long NumRows;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;

   if ((NumRows = Tst_GetTagsQst (QstCod,&mysql_res)))	// Result: TagTxt
      /***** Write the tags *****/
      for (NumRow = 1;
	   NumRow <= NumRows;
	   NumRow++)
        {
         row = mysql_fetch_row (mysql_res);
         fprintf (Gbl.Test.XML.FileXML,"<tag>%s</tag>%s",
                  row[0],Txt_NEW_LINE);
        }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
 }

/*****************************************************************************/
/**************** Get and write the answers of a test question ***************/
/*****************************************************************************/

static void TsI_WriteAnswersOfAQstXML (long QstCod)
  {
   extern const char *Txt_NEW_LINE;
   unsigned NumOpt;
   unsigned i;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   double FloatNum[2];

   Gbl.Test.Answer.NumOptions = Tst_GetAnswersQst (QstCod,&mysql_res,false);	// Result: AnsInd,Answer,Correct
   /*
   row[ 0] AnsInd
   row[ 1] Answer
   row[ 2] Feedback
   row[ 3] ImageName
   row[ 4] ImageTitle
   row[ 5] ImageURL
   row[ 6] Correct
   */
   /***** Write the answers *****/
   switch (Gbl.Test.AnswerType)
     {
      case Tst_ANS_INT:
         Tst_CheckIfNumberOfAnswersIsOne ();
         row = mysql_fetch_row (mysql_res);
         fprintf (Gbl.Test.XML.FileXML,"%ld",
                  Tst_GetIntAnsFromStr (row[1]));
         break;
      case Tst_ANS_FLOAT:
	 if (Gbl.Test.Answer.NumOptions != 2)
            Lay_ShowErrorAndExit ("Wrong float range.");

         for (i = 0;
              i < 2;
              i++)
           {
            row = mysql_fetch_row (mysql_res);
            FloatNum[i] = Tst_GetFloatAnsFromStr (row[1]);
           }
         fprintf (Gbl.Test.XML.FileXML,"%s"
                                       "<lower>%lg</lower>%s"
                                       "<upper>%lg</upper>%s",
                  Txt_NEW_LINE,
                  FloatNum[0],Txt_NEW_LINE,
                  FloatNum[1],Txt_NEW_LINE);
         break;
      case Tst_ANS_TRUE_FALSE:
         Tst_CheckIfNumberOfAnswersIsOne ();
         row = mysql_fetch_row (mysql_res);
         fprintf (Gbl.Test.XML.FileXML,"%s",
                  row[1][0] == 'T' ? "true" :
                	             "false");
         break;
      case Tst_ANS_UNIQUE_CHOICE:
      case Tst_ANS_MULTIPLE_CHOICE:
      case Tst_ANS_TEXT:
         fprintf (Gbl.Test.XML.FileXML,"%s",Txt_NEW_LINE);
         for (NumOpt = 0;
              NumOpt < Gbl.Test.Answer.NumOptions;
              NumOpt++)
           {
            row = mysql_fetch_row (mysql_res);

            /* Write the answer (row[1]), that is in HTML */
            fprintf (Gbl.Test.XML.FileXML,"<option");
            if (Gbl.Test.AnswerType != Tst_ANS_TEXT)
               fprintf (Gbl.Test.XML.FileXML," correct=\"%s\"",
                        (row[6][0] == 'Y') ? "yes" :
                                             "no");
            fprintf (Gbl.Test.XML.FileXML,">%s"
        	                          "<text>%s</text>%s",
                     Txt_NEW_LINE,
                     row[1],Txt_NEW_LINE);

            /* Write the feedback (row[2]) */
            if (row[2])
	       if (row[2][0])
		  fprintf (Gbl.Test.XML.FileXML,"<feedback>%s</feedback>%s",
			   row[2],Txt_NEW_LINE);
            fprintf (Gbl.Test.XML.FileXML,"</option>%s",
                     Txt_NEW_LINE);
           }
	 break;
      default:
         break;
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/************ Get questions from XML and store them in database **************/
/*****************************************************************************/

void TsI_ImportQstsFromXML (void)
  {
   extern const char *Txt_The_file_is_not_X;
   char PathTestPriv[PATH_MAX + 1];
   struct Param *Param;
   char FileNameXMLSrc[PATH_MAX + 1];
   char FileNameXMLTmp[PATH_MAX + 1];	// Full name (including path and .xml) of the destination temporary file
   char MIMEType[Brw_MAX_BYTES_MIME_TYPE + 1];
   bool WrongType = false;

   /***** Creates directory if not exists *****/
   snprintf (PathTestPriv,sizeof (PathTestPriv),
	     "%s/%s",
	     Cfg_PATH_SWAD_PRIVATE,Cfg_FOLDER_TEST);
   Fil_CreateDirIfNotExists (PathTestPriv);

   /***** Remove old files *****/
   Fil_RemoveOldTmpFiles (PathTestPriv,Cfg_TIME_TO_DELETE_TEST_TMP_FILES,false);

   /***** First of all, copy in disk the file received from stdin (really from Gbl.F.Tmp) *****/
   Param = Fil_StartReceptionOfFile (Fil_NAME_OF_PARAM_FILENAME_ORG,
                                     FileNameXMLSrc,MIMEType);

   /* Check if the file type is XML */
   if (strcmp (MIMEType,"text/xml"))
      if (strcmp (MIMEType,"application/xml"))
	 if (strcmp (MIMEType,"application/octet-stream"))
	    if (strcmp (MIMEType,"application/octetstream"))
	       if (strcmp (MIMEType,"application/octet"))
                  WrongType = true;

   if (WrongType)
     {
      snprintf (Gbl.Alert.Txt,sizeof (Gbl.Alert.Txt),
	        Txt_The_file_is_not_X,
		"xml");
      Ale_ShowAlert (Ale_WARNING,Gbl.Alert.Txt);
     }
   else
     {
      /* End the reception of XML in a temporary file */
      snprintf (FileNameXMLTmp,sizeof (FileNameXMLTmp),
	        "%s/%s.xml",
		PathTestPriv,Gbl.UniqueNameEncrypted);
      if (Fil_EndReceptionOfFile (FileNameXMLTmp,Param))
         /***** Get questions from XML file and store them in database *****/
         TsI_ReadQuestionsFromXMLFileAndStoreInDB (FileNameXMLTmp);
      else
         Ale_ShowAlert (Ale_WARNING,"Error copying file.");
     }
  }

/*****************************************************************************/
/********** Get questions from XML file and store them in database ***********/
/*****************************************************************************/

static void TsI_ReadQuestionsFromXMLFileAndStoreInDB (const char *FileNameXML)
  {
   char *XMLBuffer;
   unsigned long FileSize;

   /***** Open file *****/
   if ((Gbl.Test.XML.FileXML = fopen (FileNameXML,"rb")) == NULL)
      Lay_ShowErrorAndExit ("Can not open XML file.");

   /***** Compute file size *****/
   fseek (Gbl.Test.XML.FileXML,0L,SEEK_END);
   FileSize = (unsigned long) ftell (Gbl.Test.XML.FileXML);
   fseek (Gbl.Test.XML.FileXML,0L,SEEK_SET);

   /***** Allocate memory for XML buffer *****/
   if ((XMLBuffer = (char *) malloc (FileSize + 1)) == NULL)
      Lay_NotEnoughMemoryExit ();
   else
     {
      /***** Read file contents into XML buffer *****/
      if (fread ((void *) XMLBuffer,sizeof (char),(size_t) FileSize,Gbl.Test.XML.FileXML))
         XMLBuffer[FileSize] = '\0';
      else
         XMLBuffer[0] = '\0';

      /***** Import questions from XML buffer *****/
      TsI_ImportQuestionsFromXMLBuffer (XMLBuffer);

      free (XMLBuffer);
     }

   /***** Close file *****/
   fclose (Gbl.Test.XML.FileXML);
  }

/*****************************************************************************/
/******************** Import questions from XML buffer ***********************/
/*****************************************************************************/

static void TsI_ImportQuestionsFromXMLBuffer (const char *XMLBuffer)
  {
   extern const char *Hlp_ASSESSMENT_Tests;
   extern const char *Txt_XML_file_content;
   extern const char *Txt_Imported_questions;
   struct XMLElement *RootElem;
   struct XMLElement *TestElem = NULL;
   struct XMLElement *QuestionElem;
   struct XMLElement *TagsElem;
   struct XMLElement *TagElem;
   struct XMLElement *StemElem;
   struct XMLElement *FeedbackElem;
   struct XMLElement *AnswerElem;
   struct XMLAttribute *Attribute;
   bool AnswerTypeFound;
   bool QuestionExists;
   char Stem[Cns_MAX_BYTES_TEXT + 1];
   char Feedback[Cns_MAX_BYTES_TEXT + 1];

   /***** Allocate and get XML tree *****/
   XML_GetTree (XMLBuffer,&RootElem);

   /***** Start box *****/
   Box_StartBox (NULL,Txt_Imported_questions,NULL,
                 Hlp_ASSESSMENT_Tests,Box_NOT_CLOSABLE);

   /***** Print XML tree *****/
   fprintf (Gbl.F.Out,"<div class=\"CENTER_MIDDLE\""
	              " style=\"padding-bottom:20px;\">"
	              "<textarea title=\"%s\" cols=\"60\" rows=\"5\""
	              " spellcheck=\"false\" readonly>",
	    Txt_XML_file_content);
   XML_PrintTree (RootElem);
   fprintf (Gbl.F.Out,"</textarea>"
                      "</div>");

   /***** Get questions from XML tree and print them *****/
   /* Go to <test> element */
   if (RootElem->FirstChild)
     {
      TestElem = RootElem->FirstChild;
      if (strcmp (TestElem->TagName,"test"))	// <test> must be at level 1
         TestElem = NULL;
     }
   if (TestElem)
     {
      /* Current element is <test> */

      /***** Write heading of list of imported questions *****/
      Tbl_StartTableWideMargin (2);
      TsI_WriteHeadingListImportedQst ();

      /***** For each question... *****/
      for (QuestionElem = TestElem->FirstChild;
	   QuestionElem != NULL;
	   QuestionElem = QuestionElem->NextBrother)
	{
	 if (!strcmp (QuestionElem->TagName,"question"))
	   {
	    /***** Create test question *****/
	    Tst_QstConstructor ();

	    /* Get type of questions (in mandatory attribute "type") */
	    AnswerTypeFound = false;
	    for (Attribute = QuestionElem->FirstAttribute;
		 Attribute != NULL;
		 Attribute = Attribute->Next)
	       if (!strcmp (Attribute->AttributeName,"type"))
		 {
		  Gbl.Test.AnswerType = TsI_ConvertFromStrAnsTypXMLToAnsTyp (Attribute->Content);
		  AnswerTypeFound = true;
		  break;	// Only first attribute "type"
		 }
	    if (!AnswerTypeFound)
	       Lay_ShowErrorAndExit ("Wrong type of answer.");

	    /* Get tags */
	    for (TagsElem = QuestionElem->FirstChild, Gbl.Test.Tags.Num = 0;
		 TagsElem != NULL;
		 TagsElem = TagsElem->NextBrother)
	       if (!strcmp (TagsElem->TagName,"tags"))
		 {
		  for (TagElem = TagsElem->FirstChild;
		       TagElem != NULL && Gbl.Test.Tags.Num < Tst_MAX_TAGS_PER_QUESTION;
		       TagElem = TagElem->NextBrother)
		     if (!strcmp (TagElem->TagName,"tag"))
		       {
			if (TagElem->Content)
			  {
			   Str_Copy (Gbl.Test.Tags.Txt[Gbl.Test.Tags.Num],
			             TagElem->Content,
			             Tst_MAX_BYTES_TAG);
			   Gbl.Test.Tags.Num++;
			  }
		       }
		  break;	// Only first element "tags"
		 }

	    /* Get stem (mandatory) */
	    for (StemElem = QuestionElem->FirstChild;
		 StemElem != NULL;
		 StemElem = StemElem->NextBrother)
	       if (!strcmp (StemElem->TagName,"stem"))
		 {
		  if (StemElem->Content)
		    {
		     /* Convert stem from text to HTML (in database stem is stored in HTML) */
		     Str_Copy (Stem,StemElem->Content,
		               Cns_MAX_BYTES_TEXT);
		     Str_ChangeFormat (Str_FROM_TEXT,Str_TO_HTML,
				       Stem,Cns_MAX_BYTES_TEXT,true);

		     Gbl.Test.Stem.Text   = Stem;
		     Gbl.Test.Stem.Length = strlen (Stem);
		    }
		  break;	// Only first element "stem"
		 }

	    /* Get feedback (optional) */
	    for (FeedbackElem = QuestionElem->FirstChild;
		 FeedbackElem != NULL;
		 FeedbackElem = FeedbackElem->NextBrother)
	       if (!strcmp (FeedbackElem->TagName,"feedback"))
		 {
		  if (FeedbackElem->Content)
		    {
		     /* Convert feedback from text to HTML (in database feedback is stored in HTML) */
		     Str_Copy (Feedback,FeedbackElem->Content,
		               Cns_MAX_BYTES_TEXT);
		     Str_ChangeFormat (Str_FROM_TEXT,Str_TO_HTML,
				       Feedback,Cns_MAX_BYTES_TEXT,true);

		     Gbl.Test.Feedback.Text   = Feedback;
		     Gbl.Test.Feedback.Length = strlen (Feedback);
		    }
		  break;	// Only first element "feedback"
		 }

	    /* Get shuffle. By default, shuffle is false. */
	    Gbl.Test.Shuffle = false;
	    for (AnswerElem = QuestionElem->FirstChild;
		 AnswerElem != NULL;
		 AnswerElem = AnswerElem->NextBrother)
	       if (!strcmp (AnswerElem->TagName,"answer"))
		 {
		  if (Gbl.Test.AnswerType == Tst_ANS_UNIQUE_CHOICE ||
		      Gbl.Test.AnswerType == Tst_ANS_MULTIPLE_CHOICE)
		     /* Get whether shuffle answers (in attribute "shuffle") */
		     for (Attribute = AnswerElem->FirstAttribute;
			  Attribute != NULL;
			  Attribute = Attribute->Next)
			if (!strcmp (Attribute->AttributeName,"shuffle"))
			  {
			   Gbl.Test.Shuffle = XML_GetAttributteYesNoFromXMLTree (Attribute);
			   break;	// Only first attribute "shuffle"
			  }
		  break;	// Only first element "answer"
		 }

	    /* Get answer (mandatory) */
	    TsI_GetAnswerFromXML (AnswerElem);

	    /* Make sure that tags, text and answer are not empty */
	    if (Tst_CheckIfQstFormatIsCorrectAndCountNumOptions ())
	      {
	       /* Check if question already exists in database */
	       QuestionExists = TsI_CheckIfQuestionExistsInDB ();

	       /* Write row with this imported question */
	       TsI_WriteRowImportedQst (StemElem,FeedbackElem,QuestionExists);

	       /***** If a new question ==> insert question, tags and answer in the database *****/
	       if (!QuestionExists)
		 {
		  Gbl.Test.QstCod = -1L;
		  Tst_InsertOrUpdateQstTagsAnsIntoDB ();
		 }
	      }

	    /***** Destroy test question *****/
	    Tst_QstDestructor ();
	   }
	}

      Tbl_EndTable ();
     }
   else	// TestElem not found
      Ale_ShowAlert (Ale_ERROR,"Root element &lt;test&gt; not found.");

   /***** End table *****/
   Box_EndBox ();

   /***** Free XML tree *****/
   XML_FreeTree (RootElem);
  }

/*****************************************************************************/
/***** Convert a string with the type of answer in XML to type of answer *****/
/*****************************************************************************/

static Tst_AnswerType_t TsI_ConvertFromStrAnsTypXMLToAnsTyp (const char *StrAnsTypeXML)
  {
   extern const char *Tst_StrAnswerTypesXML[Tst_NUM_ANS_TYPES];
   Tst_AnswerType_t AnsType;

   if (StrAnsTypeXML != NULL)
      for (AnsType = (Tst_AnswerType_t) 0;
	   AnsType < Tst_NUM_ANS_TYPES;
	   AnsType++)
	 // comparison must be case insensitive, because users can edit XML
         if (!strcasecmp (StrAnsTypeXML,Tst_StrAnswerTypesXML[AnsType]))
            return AnsType;

   Lay_ShowErrorAndExit ("Wrong type of answer.");
   return (Tst_AnswerType_t) 0;	// Not reached
  }

/*****************************************************************************/
/**************** Get answer inside an XML question elements *****************/
/*****************************************************************************/

static bool TsI_CheckIfQuestionExistsInDB (void)
  {
   extern const char *Tst_StrAnswerTypesDB[Tst_NUM_ANS_TYPES];
   MYSQL_RES *mysql_res_qst;
   MYSQL_RES *mysql_res_ans;
   MYSQL_ROW row;
   bool IdenticalQuestionFound = false;
   bool IdenticalAnswers;
   unsigned NumQst;
   unsigned NumQstsWithThisStem;
   unsigned NumOpt;
   unsigned NumOptsExistingQstInDB;
   long QstCod;
   unsigned i;

   /***** Check if stem exists *****/
   NumQstsWithThisStem =
   (unsigned) DB_QuerySELECT (&mysql_res_qst,"can not check"
					     " if a question exists",
			      "SELECT QstCod FROM tst_questions"
			      " WHERE CrsCod=%ld AND AnsType='%s' AND Stem='%s'",
			      Gbl.CurrentCrs.Crs.CrsCod,
			      Tst_StrAnswerTypesDB[Gbl.Test.AnswerType],
			      Gbl.Test.Stem.Text);

   if (NumQstsWithThisStem)	// There are questions in database with the same stem that the one of this question
     {
      /***** Check if the answer exists in any of the questions with the same stem *****/
      /* For each question with the same stem */
      for (NumQst = 0;
           !IdenticalQuestionFound && NumQst < NumQstsWithThisStem;
           NumQst++)
        {
         row = mysql_fetch_row (mysql_res_qst);
         if ((QstCod = Str_ConvertStrCodToLongCod (row[0])) < 0)
            Lay_ShowErrorAndExit ("Wrong code of question.");

         /* Get answers from this question */
         NumOptsExistingQstInDB =
         (unsigned) DB_QuerySELECT (&mysql_res_ans,"can not get the answer"
						   " of a question",
				    "SELECT Answer FROM tst_answers"
				    " WHERE QstCod=%ld ORDER BY AnsInd",
				    QstCod);

         switch (Gbl.Test.AnswerType)
           {
            case Tst_ANS_INT:
               row = mysql_fetch_row (mysql_res_ans);
               IdenticalQuestionFound = (Tst_GetIntAnsFromStr (row[0]) == Gbl.Test.Answer.Integer);
               break;
            case Tst_ANS_FLOAT:
               for (IdenticalAnswers = true, i = 0;
                    IdenticalAnswers && i < 2;
                    i++)
                 {
                  row = mysql_fetch_row (mysql_res_ans);
                  IdenticalAnswers = (Tst_GetFloatAnsFromStr (row[0]) == Gbl.Test.Answer.FloatingPoint[i]);
                 }
               IdenticalQuestionFound = IdenticalAnswers;
               break;
            case Tst_ANS_TRUE_FALSE:
               row = mysql_fetch_row (mysql_res_ans);
               IdenticalQuestionFound = (Str_ConvertToUpperLetter (row[0][0]) == Gbl.Test.Answer.TF);
               break;
            case Tst_ANS_UNIQUE_CHOICE:
            case Tst_ANS_MULTIPLE_CHOICE:
            case Tst_ANS_TEXT:
               if (NumOptsExistingQstInDB == Gbl.Test.Answer.NumOptions)
                 {
                  for (IdenticalAnswers = true, NumOpt = 0;
                       IdenticalAnswers && NumOpt < NumOptsExistingQstInDB;
                       NumOpt++)
                    {
                     row = mysql_fetch_row (mysql_res_ans);

                     if (strcasecmp (row[0],Gbl.Test.Answer.Options[NumOpt].Text))
                        IdenticalAnswers = false;
                    }
                 }
               else	// Different number of answers (options)
                  IdenticalAnswers = false;
               IdenticalQuestionFound = IdenticalAnswers;
               break;
            default:
               break;
           }

         /* Free structure that stores the query result for answers */
         DB_FreeMySQLResult (&mysql_res_ans);
        }
     }
   else	// Stem does not exist
      IdenticalQuestionFound = false;

   /* Free structure that stores the query result for questions */
   DB_FreeMySQLResult (&mysql_res_qst);

   return IdenticalQuestionFound;
  }

/*****************************************************************************/
/**************** Get answer inside an XML question elements *****************/
/*****************************************************************************/
// Answer is mandatory

static void TsI_GetAnswerFromXML (struct XMLElement *AnswerElem)
  {
   struct XMLElement *OptionElem;
   struct XMLElement *TextElem;
   struct XMLElement *FeedbackElem;
   struct XMLElement *LowerUpperElem;
   struct XMLAttribute *Attribute;
   unsigned NumOpt;

   switch (Gbl.Test.AnswerType)
     {
      case Tst_ANS_INT:
         if (!Tst_AllocateTextChoiceAnswer (0))
            Lay_ShowErrorAndExit (Gbl.Alert.Txt);

         if (AnswerElem->Content)
            Str_Copy (Gbl.Test.Answer.Options[0].Text,AnswerElem->Content,
                      Tst_MAX_BYTES_ANSWER_OR_FEEDBACK);
         break;
      case Tst_ANS_FLOAT:
         if (!Tst_AllocateTextChoiceAnswer (0))
            Lay_ShowErrorAndExit (Gbl.Alert.Txt);
         if (!Tst_AllocateTextChoiceAnswer (1))
            Lay_ShowErrorAndExit (Gbl.Alert.Txt);

         for (LowerUpperElem = AnswerElem->FirstChild;
              LowerUpperElem != NULL;
              LowerUpperElem = LowerUpperElem->NextBrother)
            if (!strcmp (LowerUpperElem->TagName,"lower"))
              {
               if (LowerUpperElem->Content)
                  Str_Copy (Gbl.Test.Answer.Options[0].Text,
                            LowerUpperElem->Content,
                            Tst_MAX_BYTES_ANSWER_OR_FEEDBACK);
               break;	// Only first element "lower"
              }
         for (LowerUpperElem = AnswerElem->FirstChild;
              LowerUpperElem != NULL;
              LowerUpperElem = LowerUpperElem->NextBrother)
            if (!strcmp (LowerUpperElem->TagName,"upper"))
              {
               if (LowerUpperElem->Content)
                  Str_Copy (Gbl.Test.Answer.Options[1].Text,
                            LowerUpperElem->Content,
                            Tst_MAX_BYTES_ANSWER_OR_FEEDBACK);
               break;	// Only first element "upper"
              }
         break;
      case Tst_ANS_TRUE_FALSE:
	 // Comparisons must be case insensitive, because users can edit XML
         if (!AnswerElem->Content)
            Gbl.Test.Answer.TF = ' ';
         else if (!strcasecmp (AnswerElem->Content,"true")  ||
                  !strcasecmp (AnswerElem->Content,"T")     ||
                  !strcasecmp (AnswerElem->Content,"yes")   ||
                  !strcasecmp (AnswerElem->Content,"Y"))
            Gbl.Test.Answer.TF = 'T';
         else if (!strcasecmp (AnswerElem->Content,"false") ||
                  !strcasecmp (AnswerElem->Content,"F")     ||
                  !strcasecmp (AnswerElem->Content,"no")    ||
                  !strcasecmp (AnswerElem->Content,"N"))
            Gbl.Test.Answer.TF = 'F';
         else
            Gbl.Test.Answer.TF = ' ';
         break;
      case Tst_ANS_UNIQUE_CHOICE:
      case Tst_ANS_MULTIPLE_CHOICE:
      case Tst_ANS_TEXT:
         /* Get options */
         for (OptionElem = AnswerElem->FirstChild, NumOpt = 0;
              OptionElem != NULL && NumOpt < Tst_MAX_OPTIONS_PER_QUESTION;
              OptionElem = OptionElem->NextBrother, NumOpt++)
            if (!strcmp (OptionElem->TagName,"option"))
              {
               if (!Tst_AllocateTextChoiceAnswer (NumOpt))
                  Lay_ShowErrorAndExit (Gbl.Alert.Txt);

	       for (TextElem = OptionElem->FirstChild;
		    TextElem != NULL;
		    TextElem = TextElem->NextBrother)
		  if (!strcmp (TextElem->TagName,"text"))
		    {
		     if (TextElem->Content)
		       {
			Str_Copy (Gbl.Test.Answer.Options[NumOpt].Text,
			          TextElem->Content,
			          Tst_MAX_BYTES_ANSWER_OR_FEEDBACK);

			/* Convert answer from text to HTML (in database answer text is stored in HTML) */
			Str_ChangeFormat (Str_FROM_TEXT,Str_TO_HTML,
			                  Gbl.Test.Answer.Options[NumOpt].Text,Tst_MAX_BYTES_ANSWER_OR_FEEDBACK,true);
		       }
		     break;	// Only first element "text"
		    }

	       for (FeedbackElem = OptionElem->FirstChild;
		    FeedbackElem != NULL;
		    FeedbackElem = FeedbackElem->NextBrother)
		  if (!strcmp (FeedbackElem->TagName,"feedback"))
		    {
		     if (FeedbackElem->Content)
		       {
			Str_Copy (Gbl.Test.Answer.Options[NumOpt].Feedback,
			          FeedbackElem->Content,
			          Tst_MAX_BYTES_ANSWER_OR_FEEDBACK);

			/* Convert feedback from text to HTML (in database answer feedback is stored in HTML) */
			Str_ChangeFormat (Str_FROM_TEXT,Str_TO_HTML,
			                  Gbl.Test.Answer.Options[NumOpt].Feedback,Tst_MAX_BYTES_ANSWER_OR_FEEDBACK,true);
		       }
		     break;	// Only first element "feedback"
		    }

	       if (Gbl.Test.AnswerType == Tst_ANS_TEXT)
		  Gbl.Test.Answer.Options[NumOpt].Correct = true;
	       else
		  /* Check if option is correct or wrong */
		  for (Attribute = OptionElem->FirstAttribute;
		       Attribute != NULL;
		       Attribute = Attribute->Next)
		     if (!strcmp (Attribute->AttributeName,"correct"))
		       {
			Gbl.Test.Answer.Options[NumOpt].Correct = XML_GetAttributteYesNoFromXMLTree (Attribute);
			break;	// Only first attribute "correct"
		       }
               }
         break;
      default:
         break;
     }
  }

/*****************************************************************************/
/************* Write heading of list of imported test questions **************/
/*****************************************************************************/

static void TsI_WriteHeadingListImportedQst (void)
  {
   extern const char *Txt_No_INDEX;
   extern const char *Txt_Tags;
   extern const char *Txt_Type;
   extern const char *Txt_Shuffle;
   extern const char *Txt_Question;

   /***** Write the heading *****/
   fprintf (Gbl.F.Out,"<tr>"
                      "<th></th>"
                      "<th class=\"CENTER_TOP\">"
                      "%s"
                      "</th>"
                      "<th class=\"CENTER_TOP\">"
                      "%s"
                      "</th>"
                      "<th class=\"CENTER_TOP\">"
                      "%s"
                      "</th>"
                      "<th class=\"CENTER_TOP\">"
                      "%s"
                      "</th>"
                      "<th class=\"LEFT_TOP\">"
                      "%s"
                      "</th>"
                      "</tr>",
            Txt_No_INDEX,
            Txt_Tags,
            Txt_Type,
            Txt_Shuffle,
            Txt_Question);
  }

/*****************************************************************************/
/**************** Write a row with one imported test question ****************/
/*****************************************************************************/

static void TsI_WriteRowImportedQst (struct XMLElement *StemElem,
                                     struct XMLElement *FeedbackElem,
                                     bool QuestionExists)
  {
   extern const char *Txt_Existing_question;
   extern const char *Txt_New_question;
   extern const char *Txt_no_tags;
   extern const char *Txt_TST_STR_ANSWER_TYPES[Tst_NUM_ANS_TYPES];
   extern const char *Txt_TST_Answer_given_by_the_teachers;
   static unsigned NumQst = 0;
   static unsigned NumNonExistingQst = 0;
   const char *Stem = (StemElem != NULL) ? StemElem->Content :
	                                   "";
   const char *Feedback = (FeedbackElem != NULL) ? FeedbackElem->Content :
	                                           "";
   unsigned NumTag;
   unsigned NumOpt;
   char *AnswerText;
   size_t AnswerTextLength;
   char *AnswerFeedback;
   size_t AnswerFeedbackLength;
   const char *ClassData = QuestionExists ? "DAT_SMALL_LIGHT" :
	                                    "DAT_SMALL";
   const char *ClassStem = QuestionExists ? "TEST_EDI_LIGHT" :
	                                    "TEST_EDI";

   Gbl.RowEvenOdd = NumQst % 2;
   NumQst++;

   /***** Put icon to indicate that a question does not exist in database *****/
   fprintf (Gbl.F.Out,"<tr>"
	              "<td class=\"BT%u CENTER_TOP\">"
                      "<img src=\"%s/%s16x16.gif\""
                      " alt=\"%s\" title=\"%s\""
                      " class=\"ICO20x20\" />"
                      "</td>",
            Gbl.RowEvenOdd,
            Gbl.Prefs.IconsURL,
            QuestionExists ? "tr" :
        	             "ok_green",
            QuestionExists ? Txt_Existing_question :
        	             Txt_New_question,
            QuestionExists ? Txt_Existing_question :
        	             Txt_New_question);

   /***** Write number of question *****/
   fprintf (Gbl.F.Out,"<td class=\"%s CENTER_TOP COLOR%u\">",
            ClassData,Gbl.RowEvenOdd);
   if (!QuestionExists)
      fprintf (Gbl.F.Out,"%u&nbsp;",++NumNonExistingQst);
   fprintf (Gbl.F.Out,"</td>");

   /***** Write the question tags *****/
   fprintf (Gbl.F.Out,"<td class=\"LEFT_TOP COLOR%u\">",
            Gbl.RowEvenOdd);
   if (Gbl.Test.Tags.Num)
     {
      /***** Write the tags *****/
      fprintf (Gbl.F.Out,"<table>");
      for (NumTag = 0;
	   NumTag < Gbl.Test.Tags.Num;
	   NumTag++)
         fprintf (Gbl.F.Out,"<tr>"
                            "<td class=\"%s LEFT_TOP\">"
                            "&nbsp;&#8226;&nbsp;"
                            "</td>"
                            "<td class=\"%s LEFT_TOP\">"
                            "%s"
                            "</td>"
                            "</tr>",
                  ClassData,
                  ClassData,Gbl.Test.Tags.Txt[NumTag]);
      fprintf (Gbl.F.Out,"</table>");
     }
   else	// no tags for this question
      fprintf (Gbl.F.Out,"<span class=\"%s\">&nbsp;(%s)&nbsp;</span>",
               ClassData,Txt_no_tags);

   fprintf (Gbl.F.Out,"</td>");

   /***** Write the question type *****/
   fprintf (Gbl.F.Out,"<td class=\"%s CENTER_TOP COLOR%u\">"
	              "%s&nbsp;"
	              "</td>",
	    ClassData,Gbl.RowEvenOdd,
            Txt_TST_STR_ANSWER_TYPES[Gbl.Test.AnswerType]);

   /***** Write if shuffle is enabled *****/
   fprintf (Gbl.F.Out,"<td class=\"CENTER_TOP COLOR%u\">",
	    Gbl.RowEvenOdd);
   if (Gbl.Test.AnswerType == Tst_ANS_UNIQUE_CHOICE ||
       Gbl.Test.AnswerType == Tst_ANS_MULTIPLE_CHOICE)
      /* Put an icon that indicates whether shuffle is enabled or not */
      if (Gbl.Test.Shuffle)
         fprintf (Gbl.F.Out,"<img src=\"%s/%s16x16.gif\""
                            " alt=\"%s\" title=\"%s\""
                            " class=\"ICO20x20\" />",
                  Gbl.Prefs.IconsURL,
                  QuestionExists ? "ok_off" :
                	           "ok_on",
                  Txt_TST_Answer_given_by_the_teachers,
                  Txt_TST_Answer_given_by_the_teachers);
   fprintf (Gbl.F.Out,"</td>");

   /***** Write the stem and the answers *****/
   fprintf (Gbl.F.Out,"<td class=\"LEFT_TOP COLOR%u\">",
            Gbl.RowEvenOdd);
   Tst_WriteQstStem (Stem,ClassStem);
   Tst_WriteQstFeedback (Feedback,"TEST_EDI_LIGHT");
   switch (Gbl.Test.AnswerType)
     {
      case Tst_ANS_INT:
         fprintf (Gbl.F.Out,"<span class=\"%s\">(%ld)</span>",
                  ClassStem,Gbl.Test.Answer.Integer);
         break;
      case Tst_ANS_FLOAT:
         fprintf (Gbl.F.Out,"<span class=\"%s\">([%lg; %lg])</span>",
                  ClassStem,Gbl.Test.Answer.FloatingPoint[0],Gbl.Test.Answer.FloatingPoint[1]);
         break;
      case Tst_ANS_TRUE_FALSE:
         fprintf (Gbl.F.Out,"<span class=\"%s\">(",ClassStem);
         Tst_WriteAnsTF (Gbl.Test.Answer.TF);
         fprintf (Gbl.F.Out,")</span>");
         break;
      case Tst_ANS_UNIQUE_CHOICE:
      case Tst_ANS_MULTIPLE_CHOICE:
      case Tst_ANS_TEXT:
         fprintf (Gbl.F.Out,"<table>");
         for (NumOpt = 0;
              NumOpt < Gbl.Test.Answer.NumOptions;
              NumOpt++)
           {
            /* Convert the answer, that is in HTML, to rigorous HTML */
            AnswerTextLength = strlen (Gbl.Test.Answer.Options[NumOpt].Text) *
        	               Str_MAX_BYTES_PER_CHAR;
            if ((AnswerText = (char *) malloc (AnswerTextLength + 1)) == NULL)
               Lay_NotEnoughMemoryExit ();
            Str_Copy (AnswerText,Gbl.Test.Answer.Options[NumOpt].Text,
                      AnswerTextLength);
            Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
                              AnswerText,AnswerTextLength,false);

            /* Convert the feedback, that is in HTML, to rigorous HTML */
            AnswerFeedbackLength = 0;
            AnswerFeedback = NULL;
            if (Gbl.Test.Answer.Options[NumOpt].Feedback)
               if (Gbl.Test.Answer.Options[NumOpt].Feedback[0])
		 {
	          AnswerFeedbackLength = strlen (Gbl.Test.Answer.Options[NumOpt].Feedback) *
					 Str_MAX_BYTES_PER_CHAR;
	          if ((AnswerFeedback = (char *) malloc (AnswerFeedbackLength + 1)) == NULL)
		     Lay_NotEnoughMemoryExit ();
		  Str_Copy (AnswerFeedback,
		            Gbl.Test.Answer.Options[NumOpt].Feedback,
		            AnswerFeedbackLength);
		  Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
		                    AnswerFeedback,AnswerFeedbackLength,false);
		 }

            /* Put an icon that indicates whether the answer is correct or wrong */
            fprintf (Gbl.F.Out,"<tr>"
        	               "<td class=\"BT%u\">",Gbl.RowEvenOdd);
            if (Gbl.Test.Answer.Options[NumOpt].Correct)
               fprintf (Gbl.F.Out,"<img src=\"%s/%s16x16.gif\""
        	                  " alt=\"%s\" title=\"%s\""
        	                  " class=\"ICO20x20\" />",
                        Gbl.Prefs.IconsURL,
                        QuestionExists ? "ok_off" :
                                         "ok_on",
                        Txt_TST_Answer_given_by_the_teachers,
                        Txt_TST_Answer_given_by_the_teachers);
            fprintf (Gbl.F.Out,"</td>");

            /* Write the number of option */
            fprintf (Gbl.F.Out,"<td class=\"%s LEFT_TOP\">"
        	               "%c)&nbsp;"
        	               "</td>",
                     ClassData,'a' + (char) NumOpt);

            /* Write the text and the feedback of the answer */
            fprintf (Gbl.F.Out,"<td class=\"LEFT_TOP\">"
        	               "<div class=\"%s\">"
        	               "%s"
        	               "</div>",
                     ClassStem,AnswerText);
            if (AnswerFeedbackLength)
	       fprintf (Gbl.F.Out,"<div class=\"TEST_EDI_LIGHT\">"
		                  "%s"
		                  "</div>",
			AnswerFeedback);
            fprintf (Gbl.F.Out,"</td>"
        	               "</tr>");

	    /* Free memory allocated for the answer and the feedback */
	    free ((void *) AnswerText);
            if (AnswerFeedbackLength)
	       free ((void *) AnswerFeedback);
           }
         fprintf (Gbl.F.Out,"</table>");
	 break;
      default:
         break;
     }
   fprintf (Gbl.F.Out,"</td>"
	              "</tr>");
  }
