// swad_test_import.c: import and export self-assessment tests using XML files

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2015 Antonio Cañas Vargas

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

#include <stdlib.h>		// For exit, system, malloc, free, etc
#include <string.h>		// For string functions
#include <sys/stat.h>		// For mkdir
#include <sys/types.h>		// For mkdir

#include "swad_database.h"
#include "swad_global.h"
#include "swad_parameter.h"
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

static void TsI_GetAndWriteTagsXML (long QstCod);
static void TsI_WriteAnswersOfAQstXML (long QstCod);
static void TsI_ReadQuestionsFromXMLFileAndStoreInDB (const char *FileNameXML);
static void TsI_ImportQuestionsFromXMLBuffer (const char *XMLBuffer);
static Tst_AnswerType_t TsI_ConvertFromStrAnsTypXMLToAnsTyp (const char *StrAnsTypeXML);
static bool TsI_CheckIfQuestionExistsInDB (void);
static void TsI_GetAnswerFromXML (struct XMLElement *AnswerElem);
static void TsI_WriteHeadingListImportedQst (void);
static void TsI_WriteEndingListImportedQst (void);
static void TsI_WriteRowImportedQst (struct XMLElement *StemElem,
                                     struct XMLElement *FeedbackElem,
                                     bool QuestionExists);

/*****************************************************************************/
/**************** Put a link (form) to export test questions *****************/
/*****************************************************************************/

void TsI_PutFormToExportQuestions (void)
  {
   extern const char *The_ClassFormBold[The_NUM_THEMES];
   extern const char *Txt_Export_questions_to_XML_file;
   extern const char *Txt_Export_questions;

   /***** Start form *****/
   Act_FormStart (ActLstTstQst);

   /***** Parameters of the form *****/
   Sta_WriteParamsDatesSeeAccesses ();
   Tst_WriteParamEditQst ();
   Par_PutHiddenParamChar ("OnlyThisQst",'N');
   Par_PutHiddenParamUnsigned ("Order",(unsigned) Gbl.Test.SelectedOrderType);
   Par_PutHiddenParamChar ("CreateXML",'Y');

   /***** Put a link to create a file with questions *****/
   Act_LinkFormSubmit (Txt_Export_questions_to_XML_file,The_ClassFormBold[Gbl.Prefs.Theme]);
   Lay_PutIconWithText ("export16x16.gif",
                        Txt_Export_questions_to_XML_file,
                        Txt_Export_questions);
   Act_FormEnd ();
  }

/*****************************************************************************/
/*************** Put a link (form) to import test questions ******************/
/*****************************************************************************/

void TsI_PutFormToImportQuestions (void)
  {
   extern const char *The_ClassFormBold[The_NUM_THEMES];
   extern const char *Txt_Import_questions_from_XML_file;
   extern const char *Txt_Import_questions;

   /***** Start form *****/
   Act_FormStart (ActReqImpTstQst);

   /***** Put a link to create a file with questions *****/
   Act_LinkFormSubmit (Txt_Import_questions_from_XML_file,The_ClassFormBold[Gbl.Prefs.Theme]);
   Lay_PutIconWithText ("import16x16.gif",
                        Txt_Import_questions_from_XML_file,
                        Txt_Import_questions);
   Act_FormEnd ();
  }

/*****************************************************************************/
/*********** Show form to import test questions from an XML file *************/
/*****************************************************************************/

void TsI_ShowFormImportQstsFromXML (void)
  {
   extern const char *The_ClassForm[The_NUM_THEMES];
   extern const char *Txt_You_need_an_XML_file_containing_a_list_of_questions_to_import;
   extern const char *Txt_Upload_file;
   extern const char *Txt_XML_file;

   fprintf (Gbl.F.Out,"<div class=\"CENTER_MIDDLE\">");

   /***** Write help message *****/
   Lay_ShowAlert (Lay_INFO,Txt_You_need_an_XML_file_containing_a_list_of_questions_to_import);

   /***** Write a form to import questions *****/
   Act_FormStart (ActImpTstQst);
   fprintf (Gbl.F.Out,"<table style=\"margin:0 auto;\">"
                      "<tr>"
                      "<td class=\"%s RIGHT_MIDDLE\">"
                      "%s:"
                      "</td>"
                      "<td class=\"LEFT_MIDDLE\">"
	              "<input type=\"file\" name=\"%s\""
	              " size=\"40\" maxlength=\"100\" value=\"\" />"
	              "</td>"
                      "</tr>"
                      "<tr>"
	              "<td colspan=\"2\" class=\"CENTER_MIDDLE\">",
            The_ClassForm[Gbl.Prefs.Theme],
            Txt_XML_file,
            Fil_NAME_OF_PARAM_FILENAME_ORG);
   Lay_PutCreateButton (Txt_Upload_file);
   fprintf (Gbl.F.Out,"</td>"
	              "</tr>"
                      "</table>");
   Act_FormEnd ();
   fprintf (Gbl.F.Out,"</div>");
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
   char PathFileBrowserTmp[PATH_MAX+1];
   char PathPubDirTmp[PATH_MAX+1];
   char PathPubFile[PATH_MAX+1];
   unsigned long NumRow;
   MYSQL_ROW row;
   long QstCod;

   /***** Create a temporary public directory used to download the XML file *****/
   /* If the public directory does not exist, create it */
   sprintf (PathFileBrowserTmp,"%s/%s",Cfg_PATH_SWAD_PUBLIC,Cfg_FOLDER_FILE_BROWSER_TMP);
   Fil_CreateDirIfNotExists (PathFileBrowserTmp);

   /* First of all, we remove the oldest temporary directories.
      Such temporary directories have been created by me or by other users.
      This is a bit sloppy, but they must be removed by someone.
      Here "oldest" means more than x time from their creation */
   Fil_RemoveOldTmpFiles (PathFileBrowserTmp,Cfg_TIME_TO_DELETE_BROWSER_TMP_FILES,false);

   /* Create a new temporary directory */
   strcpy (Gbl.FileBrowser.TmpPubDir,Gbl.UniqueNameEncrypted);
   sprintf (PathPubDirTmp,"%s/%s",PathFileBrowserTmp,Gbl.FileBrowser.TmpPubDir);
   if (mkdir (PathPubDirTmp,(mode_t) 0xFFF))
      Lay_ShowErrorAndExit ("Can not create a temporary folder for download.");

   /***** Create public XML file with the questions *****/
   sprintf (PathPubFile,"%s/test.xml",PathPubDirTmp);
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
                  Str_ConvertToUpperLetter (row[3][0]) == 'Y' ? "yes" :
                	                                        "no");
      fprintf (Gbl.Test.XML.FileXML,">");
      TsI_WriteAnswersOfAQstXML (QstCod);
      fprintf (Gbl.Test.XML.FileXML,"</answer>%s",Txt_NEW_LINE);

      /* End question */
      fprintf (Gbl.Test.XML.FileXML,"</question>%s%s",Txt_NEW_LINE,Txt_NEW_LINE);
     }

   /***** End XML file *****/
   XML_WriteEndFile (Gbl.Test.XML.FileXML,"test");

   /***** Close the XML file *****/
   fclose (Gbl.Test.XML.FileXML);

   /***** Return to start of query result *****/
   mysql_data_seek (mysql_res, 0);

   /***** Write the link to XML file *****/
   fprintf (Gbl.F.Out,"<a href=\"%s/%s/%s/test.xml\" class=\"%s\" target=\"_blank\">",
            Cfg_HTTPS_URL_SWAD_PUBLIC,Cfg_FOLDER_FILE_BROWSER_TMP,
            Gbl.FileBrowser.TmpPubDir,
            The_ClassFormBold[Gbl.Prefs.Theme]);
   Lay_PutIconWithText ("file64x64.gif",
                        Txt_XML_file,
                        Txt_XML_file);
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
                        Str_ConvertToUpperLetter (row[2][0]) == 'Y' ? "yes" :
                                                                      "no");
            fprintf (Gbl.Test.XML.FileXML,">%s"
        	                          "<text>%s</text>%s",
                     Txt_NEW_LINE,
                     row[1],Txt_NEW_LINE);
            if (row[3])
	       if (row[3][0])
		  fprintf (Gbl.Test.XML.FileXML,"<feedback>%s</feedback>%s",
			   row[3],Txt_NEW_LINE);
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
   char PathTestPriv[PATH_MAX+1];
   char FileNameXMLSrc[PATH_MAX+1];
   char FileNameXMLTmp[PATH_MAX+1];	// Full name (including path and .xml) of the destination temporary file
   char MIMEType[Brw_MAX_BYTES_MIME_TYPE+1];
   bool CorrectType;

   /***** Creates directory if not exists *****/
   sprintf (PathTestPriv,"%s/%s",Cfg_PATH_SWAD_PRIVATE,Cfg_FOLDER_TEST);
   Fil_CreateDirIfNotExists (PathTestPriv);

   /***** Remove old files *****/
   Fil_RemoveOldTmpFiles (PathTestPriv,Cfg_TIME_TO_DELETE_TEST_TMP_FILES,false);

   /***** First of all, copy in disk the file received from stdin (really from Gbl.F.Tmp) *****/
   Fil_StartReceptionOfFile (FileNameXMLSrc,MIMEType);

   /* Check if the file type is image/jpeg or image/pjpeg or application/octet-stream */
   CorrectType = true;
   if (strcmp (MIMEType,"text/xml"))
      if (strcmp (MIMEType,"application/xml"))
	 if (strcmp (MIMEType,"application/octet-stream"))
	    if (strcmp (MIMEType,"application/octetstream"))
	       if (strcmp (MIMEType,"application/octet"))
                  CorrectType = false;

   if (CorrectType)
     {
      /* End the reception of XML in a temporary file */
      sprintf (FileNameXMLTmp,"%s/%s.jpg",PathTestPriv,Gbl.UniqueNameEncrypted);
      if (Fil_EndReceptionOfFile (FileNameXMLTmp))
         /***** Get questions from XML file and store them in database *****/
         TsI_ReadQuestionsFromXMLFileAndStoreInDB (FileNameXMLTmp);
      else
         Lay_ShowAlert (Lay_WARNING,"Error uploading file.");
     }
   else
     {
      sprintf (Gbl.Message,Txt_The_file_is_not_X,"xml");
      Lay_ShowAlert (Lay_WARNING,Gbl.Message);
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
   if ((XMLBuffer = malloc (FileSize + 1)) == NULL)
      Lay_ShowErrorAndExit ("Not enough memory for XML buffer.");
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
   char Stem[Cns_MAX_BYTES_TEXT+1];
   char Feedback[Cns_MAX_BYTES_TEXT+1];

   /***** Allocate and get XML tree *****/
   XML_GetTree (XMLBuffer,&RootElem);

   /***** Print XML tree *****/
   Lay_WriteTitle (Txt_XML_file_content);
   fprintf (Gbl.F.Out,"<div class=\"CENTER_MIDDLE\">"
	              "<textarea cols=\"60\" rows=\"4\">");
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
   if (!TestElem)
      Lay_ShowErrorAndExit ("Root element &lt;test&gt; not found.");

   /* Current element is <test> */

   /***** Write heading of list of imported questions *****/
   Lay_WriteTitle (Txt_Imported_questions);
   TsI_WriteHeadingListImportedQst ();

   /***** For each question... *****/
   for (QuestionElem = TestElem->FirstChild;
	QuestionElem != NULL;
	QuestionElem = QuestionElem->NextBrother)
     {
      if (!strcmp (QuestionElem->TagName,"question"))
        {
         /***** Initialize new question to zero *****/
         Tst_InitQst ();

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
         Gbl.Test.NumTags = 0;
         for (TagsElem = QuestionElem->FirstChild;
              TagsElem != NULL;
              TagsElem = TagsElem->NextBrother)
            if (!strcmp (TagsElem->TagName,"tags"))
              {
               for (TagElem = TagsElem->FirstChild;
                    TagElem != NULL && Gbl.Test.NumTags < Tst_MAX_TAGS_PER_QUESTION;
                    TagElem = TagElem->NextBrother)
                  if (!strcmp (TagElem->TagName,"tag"))
                    {
                     if (TagElem->Content)
                       {
                        strncpy (Gbl.Test.TagText[Gbl.Test.NumTags],TagElem->Content,Tst_MAX_BYTES_TAG);
                        Gbl.Test.TagText[Gbl.Test.NumTags][Tst_MAX_BYTES_TAG] = '\0';
                        Gbl.Test.NumTags++;
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
        	  strncpy (Stem,StemElem->Content,Cns_MAX_BYTES_TEXT);
        	  Stem[Cns_MAX_BYTES_TEXT] = '\0';
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
        	  strncpy (Feedback,FeedbackElem->Content,Cns_MAX_BYTES_TEXT);
        	  Feedback[Cns_MAX_BYTES_TEXT] = '\0';
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

         /***** Free answers *****/
         Tst_FreeTextChoiceAnswers ();
        }
     }

   /***** Write ending of list of imported questions *****/
   TsI_WriteEndingListImportedQst ();

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
   char *QueryQst;
   MYSQL_RES *mysql_res_qst;
   char QueryAns[256];
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
   /* Allocate space for query */
   if ((QueryQst = malloc (256+Gbl.Test.Stem.Length)) == NULL)
      Lay_ShowErrorAndExit ("Not enough memory to store database query.");

   /* Make database query */
   sprintf (QueryQst,"SELECT QstCod FROM tst_questions"
                     " WHERE CrsCod='%ld' AND AnsType='%s' AND Stem='%s'",
            Gbl.CurrentCrs.Crs.CrsCod,
            Tst_StrAnswerTypesDB[Gbl.Test.AnswerType],
            Gbl.Test.Stem.Text);
   NumQstsWithThisStem = (unsigned) DB_QuerySELECT (QueryQst,&mysql_res_qst,"can not check if a question exists");

   /* Free space user for query */
   free ((void *) QueryQst);

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
         sprintf (QueryAns,"SELECT Answer FROM tst_answers"
                           " WHERE QstCod='%ld' ORDER BY AnsInd",
                  QstCod);
         NumOptsExistingQstInDB = (unsigned) DB_QuerySELECT (QueryAns,&mysql_res_ans,"can not get the answer of a question");

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
            Lay_ShowErrorAndExit (Gbl.Message);

         if (AnswerElem->Content)
           {
            strncpy (Gbl.Test.Answer.Options[0].Text,AnswerElem->Content,
                     Tst_MAX_BYTES_ANSWER_OR_FEEDBACK);
            Gbl.Test.Answer.Options[0].Text[Tst_MAX_BYTES_ANSWER_OR_FEEDBACK] = '\0';
           }
         break;
      case Tst_ANS_FLOAT:
         if (!Tst_AllocateTextChoiceAnswer (0))
            Lay_ShowErrorAndExit (Gbl.Message);
         if (!Tst_AllocateTextChoiceAnswer (1))
            Lay_ShowErrorAndExit (Gbl.Message);

         for (LowerUpperElem = AnswerElem->FirstChild;
              LowerUpperElem != NULL;
              LowerUpperElem = LowerUpperElem->NextBrother)
            if (!strcmp (LowerUpperElem->TagName,"lower"))
              {
               if (LowerUpperElem->Content)
                 {
                  strncpy (Gbl.Test.Answer.Options[0].Text,LowerUpperElem->Content,
                           Tst_MAX_BYTES_ANSWER_OR_FEEDBACK);
                  Gbl.Test.Answer.Options[0].Text[Tst_MAX_BYTES_ANSWER_OR_FEEDBACK] = '\0';
                 }
               break;	// Only first element "lower"
              }
         for (LowerUpperElem = AnswerElem->FirstChild;
              LowerUpperElem != NULL;
              LowerUpperElem = LowerUpperElem->NextBrother)
            if (!strcmp (LowerUpperElem->TagName,"upper"))
              {
               if (LowerUpperElem->Content)
                 {
                  strncpy (Gbl.Test.Answer.Options[1].Text,LowerUpperElem->Content,
                           Tst_MAX_BYTES_ANSWER_OR_FEEDBACK);
                  Gbl.Test.Answer.Options[1].Text[Tst_MAX_BYTES_ANSWER_OR_FEEDBACK] = '\0';
                 }
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
                  Lay_ShowErrorAndExit (Gbl.Message);

	       for (TextElem = OptionElem->FirstChild;
		    TextElem != NULL;
		    TextElem = TextElem->NextBrother)
		  if (!strcmp (TextElem->TagName,"text"))
		    {
		     if (TextElem->Content)
		       {
			strncpy (Gbl.Test.Answer.Options[NumOpt].Text,TextElem->Content,
			         Tst_MAX_BYTES_ANSWER_OR_FEEDBACK);
			Gbl.Test.Answer.Options[NumOpt].Text[Tst_MAX_BYTES_ANSWER_OR_FEEDBACK] = '\0';

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
			strncpy (Gbl.Test.Answer.Options[NumOpt].Feedback,FeedbackElem->Content,
			         Tst_MAX_BYTES_ANSWER_OR_FEEDBACK);
			Gbl.Test.Answer.Options[NumOpt].Feedback[Tst_MAX_BYTES_ANSWER_OR_FEEDBACK] = '\0';

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

   /***** Table start *****/
   Lay_StartRoundFrameTable (NULL,2,NULL);

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
/*************** Write end of list of imported test questions ****************/
/*****************************************************************************/

static void TsI_WriteEndingListImportedQst (void)
  {
   /***** Table end *****/
   Lay_EndRoundFrameTable ();
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
   extern const char *Txt_TEST_Correct_answer;
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
                      " class=\"ICON16x16\" />"
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
   if (Gbl.Test.NumTags)
     {
      /***** Write the tags *****/
      fprintf (Gbl.F.Out,"<table>");
      for (NumTag = 0;
	   NumTag < Gbl.Test.NumTags;
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
                  ClassData,Gbl.Test.TagText[NumTag]);
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
                            " class=\"ICON16x16\" />",
                  Gbl.Prefs.IconsURL,
                  QuestionExists ? "ok_off" :
                	           "ok_on",
                  Txt_TEST_Correct_answer,
                  Txt_TEST_Correct_answer);
   fprintf (Gbl.F.Out,"</td>");

   /***** Write the stem and the answers *****/
   fprintf (Gbl.F.Out,"<td class=\"LEFT_TOP COLOR%u\">",
            Gbl.RowEvenOdd);
   Tst_WriteQstStem (Stem,ClassStem);
   Tst_WriteQstFeedback (Feedback,"TEST_EDI_LIGHT");
   switch (Gbl.Test.AnswerType)
     {
      case Tst_ANS_INT:
         fprintf (Gbl.F.Out,"<span class=\"%s\"><tt>(%ld)</tt></span>",
                  ClassStem,Gbl.Test.Answer.Integer);
         break;
      case Tst_ANS_FLOAT:
         fprintf (Gbl.F.Out,"<span class=\"%s\"><tt>([%lg; %lg])</tt></span>",
                  ClassStem,Gbl.Test.Answer.FloatingPoint[0],Gbl.Test.Answer.FloatingPoint[1]);
         break;
      case Tst_ANS_TRUE_FALSE:
         fprintf (Gbl.F.Out,"<span class=\"%s\"><tt>(",ClassStem);
         Tst_WriteAnsTF (Gbl.Test.Answer.TF);
         fprintf (Gbl.F.Out,")</tt></span>");
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
        	               Str_MAX_LENGTH_SPEC_CHAR_HTML;
            if ((AnswerText = malloc (AnswerTextLength+1)) == NULL)
               Lay_ShowErrorAndExit ("Not enough memory to store answer.");
            strncpy (AnswerText,Gbl.Test.Answer.Options[NumOpt].Text,AnswerTextLength);
            AnswerText[AnswerTextLength] = '\0';
            Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
                              AnswerText,AnswerTextLength,false);

            /* Convert the feedback, that is in HTML, to rigorous HTML */
            AnswerFeedbackLength = 0;
            AnswerFeedback = NULL;
            if (Gbl.Test.Answer.Options[NumOpt].Feedback)
               if (Gbl.Test.Answer.Options[NumOpt].Feedback[0])
		 {
	          AnswerFeedbackLength = strlen (Gbl.Test.Answer.Options[NumOpt].Feedback) *
					 Str_MAX_LENGTH_SPEC_CHAR_HTML;
	          if ((AnswerFeedback = malloc (AnswerFeedbackLength+1)) == NULL)
		     Lay_ShowErrorAndExit ("Not enough memory to store feedback.");
		  strncpy (AnswerFeedback,Gbl.Test.Answer.Options[NumOpt].Feedback,AnswerFeedbackLength);
		  AnswerFeedback[AnswerFeedbackLength] = '\0';
		  Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
		                    AnswerFeedback,AnswerFeedbackLength,false);
		 }

            /* Put an icon that indicates whether the answer is correct or wrong */
            fprintf (Gbl.F.Out,"<tr>"
        	               "<td class=\"BT%u\">",Gbl.RowEvenOdd);
            if (Gbl.Test.Answer.Options[NumOpt].Correct)
               fprintf (Gbl.F.Out,"<img src=\"%s/%s16x16.gif\""
        	                  " alt=\"%s\" title=\"%s\""
        	                  " class=\"ICON16x16\" />",
                        Gbl.Prefs.IconsURL,
                        QuestionExists ? "ok_off" :
                                         "ok_on",
                        Txt_TEST_Correct_answer,
                        Txt_TEST_Correct_answer);
            fprintf (Gbl.F.Out,"</td>");

            /* Write the number of option */
            fprintf (Gbl.F.Out,"<td class=\"%s LEFT_TOP\">"
        	               "%c)&nbsp;"
        	               "</td>",
                     ClassData,'a' + (char) NumOpt);

            /* Write the text and the feedback of the answer */
            fprintf (Gbl.F.Out,"<td class=\"LEFT_TOP\">"
        	               "<p class=\"%s\">"
        	               "<tt>%s</tt>"
        	               "</p>",
                     ClassStem,AnswerText);
            if (AnswerFeedbackLength)
	       fprintf (Gbl.F.Out,"<p class=\"TEST_EDI_LIGHT\">"
		                  "<tt>%s</tt>"
		                  "</p>",
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
