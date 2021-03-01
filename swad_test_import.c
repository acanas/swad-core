// swad_test_import.c: import and export self-assessment tests using XML files

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
/*********************************** Headers *********************************/
/*****************************************************************************/

#include <stdlib.h>		// For exit, system, malloc, free, etc
#include <string.h>		// For string functions
#include <sys/stat.h>		// For mkdir
#include <sys/types.h>		// For mkdir

#include "swad_box.h"
#include "swad_database.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_HTML.h"
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
/******************************* Private types *******************************/
/*****************************************************************************/

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/************************* Private global variables **************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void TsI_PutParamsExportQsts (void *Test);
static void TsI_PutCreateXMLParam (void);

static void TsI_ExportQuestion (struct Tst_Question *Question,FILE *FileXML);

static void TsI_GetAndWriteTagsXML (long QstCod,FILE *FileXML);
static void TsI_WriteAnswersOfAQstXML (const struct Tst_Question *Question,
                                       FILE *FileXML);
static void TsI_ReadQuestionsFromXMLFileAndStoreInDB (const char *FileNameXML);
static void TsI_ImportQuestionsFromXMLBuffer (const char *XMLBuffer);
static Tst_AnswerType_t TsI_ConvertFromStrAnsTypXMLToAnsTyp (const char *StrAnsTypeXML);
static void TsI_GetAnswerFromXML (struct XMLElement *AnswerElem,
                                  struct Tst_Question *Question);
static void TsI_WriteHeadingListImportedQst (void);
static void TsI_WriteRowImportedQst (struct XMLElement *StemElem,
                                     struct XMLElement *FeedbackElem,
                                     const struct Tst_Question *Question,
                                     bool QuestionExists);

/*****************************************************************************/
/**************** Put a link (form) to export test questions *****************/
/*****************************************************************************/

void TsI_PutIconToExportQuestions (struct Tst_Test *Test)
  {
   extern const char *Txt_Export_questions;

   /***** Put a link to create a file with questions *****/
   Lay_PutContextualLinkOnlyIcon (ActLstTstQst,NULL,
                                  TsI_PutParamsExportQsts,Test,
				  "file-import.svg",
				  Txt_Export_questions);
  }

/*****************************************************************************/
/****************** Put params to export test questions **********************/
/*****************************************************************************/

static void TsI_PutParamsExportQsts (void *Test)
  {
   if (Test)
     {
      Tst_PutParamsEditQst (Test);
      Par_PutHiddenParamChar ("OnlyThisQst",'N');
      Par_PutHiddenParamUnsigned (NULL,"Order",(unsigned) (((struct Tst_Test *) Test)->SelectedOrder));
      TsI_PutCreateXMLParam ();
     }
  }

/*****************************************************************************/
/************************ Parameter to create XML file ***********************/
/*****************************************************************************/

static void TsI_PutCreateXMLParam (void)
  {
   Par_PutHiddenParamChar ("CreateXML",'Y');
  }

bool TsI_GetCreateXMLParamFromForm (void)
  {
   return Par_GetParToBool ("CreateXML");
  }

/*****************************************************************************/
/*************** Put a link (form) to import test questions ******************/
/*****************************************************************************/

void TsI_PutIconToImportQuestions (void)
  {
   extern const char *Txt_Import_questions;

   /***** Put a link to create a file with questions *****/
   Lay_PutContextualLinkOnlyIcon (ActReqImpTstQst,NULL,
                                  NULL,NULL,
				  "file-export.svg",
				  Txt_Import_questions);
  }

/*****************************************************************************/
/*********** Show form to import test questions from an XML file *************/
/*****************************************************************************/

void TsI_ShowFormImportQstsFromXML (void)
  {
   extern const char *Hlp_ASSESSMENT_Tests;
   extern const char *The_ClassFormInBox[The_NUM_THEMES];
   extern const char *Txt_Import_questions;
   extern const char *Txt_You_need_an_XML_file_containing_a_list_of_questions;
   extern const char *Txt_XML_file;

   /***** Begin box *****/
   Box_BoxBegin (NULL,Txt_Import_questions,
                 NULL,NULL,
                 Hlp_ASSESSMENT_Tests,Box_NOT_CLOSABLE);

   /***** Write help message *****/
   Ale_ShowAlert (Ale_INFO,Txt_You_need_an_XML_file_containing_a_list_of_questions);

   /***** Write a form to import questions *****/
   Frm_BeginForm (ActImpTstQst);
   HTM_LABEL_Begin ("class=\"%s\"",The_ClassFormInBox[Gbl.Prefs.Theme]);
   HTM_TxtColonNBSP (Txt_XML_file);
   HTM_INPUT_FILE (Fil_NAME_OF_PARAM_FILENAME_ORG,".xml",
                   HTM_SUBMIT_ON_CHANGE,
                   NULL);
   HTM_LABEL_End ();
   Frm_EndForm ();

   /***** End box *****/
   Box_BoxEnd ();
  }

/*****************************************************************************/
/*** Create the XML file with test questions and put a link to download it ***/
/*****************************************************************************/

void TsI_CreateXML (unsigned NumQsts,MYSQL_RES *mysql_res)
  {
   extern const char *The_ClassFormOutBoxBold[The_NUM_THEMES];
   extern const char *Txt_NEW_LINE;
   extern const char *Txt_XML_file;
   char PathPubFile[PATH_MAX + 1];
   FILE *FileXML;
   unsigned NumQst;
   struct Tst_Question Question;
   MYSQL_ROW row;

   /***** Create a temporary public directory
	  used to download the XML file *****/
   Brw_CreateDirDownloadTmp ();

   /***** Create public XML file with the questions *****/
   snprintf (PathPubFile,sizeof (PathPubFile),"%s/%s/%s/test.xml",
             Cfg_PATH_FILE_BROWSER_TMP_PUBLIC,
             Gbl.FileBrowser.TmpPubDir.L,
             Gbl.FileBrowser.TmpPubDir.R);
   if ((FileXML = fopen (PathPubFile,"wb")) == NULL)
      Lay_ShowErrorAndExit ("Can not open target file.");

   /***** Start XML file *****/
   XML_WriteStartFile (FileXML,"test",false);
   fprintf (FileXML,"%s",Txt_NEW_LINE);

   /***** Write rows *****/
   for (NumQst = 0;
	NumQst < NumQsts;
	NumQst++)
     {
      /* Create test question */
      Tst_QstConstructor (&Question);

      /* Get question code (row[0]) */
      row = mysql_fetch_row (mysql_res);
      if ((Question.QstCod = Str_ConvertStrCodToLongCod (row[0])) < 0)
         Lay_ShowErrorAndExit ("Wrong code of question.");

      TsI_ExportQuestion (&Question,FileXML);

      /* Destroy test question */
      Tst_QstDestructor (&Question);
     }

   /***** End XML file *****/
   XML_WriteEndFile (FileXML,"test");

   /***** Close the XML file *****/
   fclose (FileXML);

   /***** Return to start of query result *****/
   mysql_data_seek (mysql_res,0);

   /***** Write the link to XML file *****/
   HTM_A_Begin ("href=\"%s/%s/%s/test.xml\" class=\"%s\" target=\"_blank\"",
	        Cfg_URL_FILE_BROWSER_TMP_PUBLIC,
	        Gbl.FileBrowser.TmpPubDir.L,
	        Gbl.FileBrowser.TmpPubDir.R,
	        The_ClassFormOutBoxBold[Gbl.Prefs.Theme]);
   Ico_PutIconTextLink ("file.svg",
			Txt_XML_file);
   HTM_A_End ();
  }

/*****************************************************************************/
/****************** Write one question into the XML file *********************/
/*****************************************************************************/

static void TsI_ExportQuestion (struct Tst_Question *Question,FILE *FileXML)
  {
   extern const char *Tst_StrAnswerTypesXML[Tst_NUM_ANS_TYPES];
   extern const char *Txt_NEW_LINE;

   if (Tst_GetQstDataFromDB (Question))
     {
      /***** Write the answer type *****/
      fprintf (FileXML,"<question type=\"%s\">%s",
               Tst_StrAnswerTypesXML[Question->Answer.Type],Txt_NEW_LINE);

      /***** Write the question tags *****/
      fprintf (FileXML,"<tags>%s",Txt_NEW_LINE);
      TsI_GetAndWriteTagsXML (Question->QstCod,FileXML);
      fprintf (FileXML,"</tags>%s",Txt_NEW_LINE);

      /***** Write the stem, that is in HTML format *****/
      fprintf (FileXML,"<stem>%s</stem>%s",
               Question->Stem,Txt_NEW_LINE);

      /***** Write the feedback, that is in HTML format *****/
      if (Question->Feedback[0])
	 fprintf (FileXML,"<feedback>%s</feedback>%s",
		  Question->Feedback,Txt_NEW_LINE);

      /***** Write the answers of this question.
             Shuffle can be enabled or disabled *****/
      fprintf (FileXML,"<answer");
      if (Question->Answer.Type == Tst_ANS_UNIQUE_CHOICE ||
          Question->Answer.Type == Tst_ANS_MULTIPLE_CHOICE)
         fprintf (FileXML," shuffle=\"%s\"",
                  Question->Answer.Shuffle ? "yes" :
                	                     "no");
      fprintf (FileXML,">");
      TsI_WriteAnswersOfAQstXML (Question,FileXML);
      fprintf (FileXML,"</answer>%s",Txt_NEW_LINE);

      /***** End question *****/
      fprintf (FileXML,"</question>%s%s",
               Txt_NEW_LINE,Txt_NEW_LINE);
     }
  }

/*****************************************************************************/
/************* Get and write tags of a question into the XML file ************/
/*****************************************************************************/

static void TsI_GetAndWriteTagsXML (long QstCod,FILE *FileXML)
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
         fprintf (FileXML,"<tag>%s</tag>%s",
                  row[0],Txt_NEW_LINE);
        }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
 }

/*****************************************************************************/
/**************** Get and write the answers of a test question ***************/
/*****************************************************************************/

static void TsI_WriteAnswersOfAQstXML (const struct Tst_Question *Question,
                                       FILE *FileXML)
  {
   extern const char *Txt_NEW_LINE;
   unsigned NumOpt;

   /***** Write answers *****/
   switch (Question->Answer.Type)
     {
      case Tst_ANS_INT:
         fprintf (FileXML,"%ld",Question->Answer.Integer);
         break;
      case Tst_ANS_FLOAT:
         fprintf (FileXML,"%s"
                          "<lower>%.15lg</lower>%s"
                          "<upper>%.15lg</upper>%s",
                  Txt_NEW_LINE,
                  Question->Answer.FloatingPoint[0],Txt_NEW_LINE,
                  Question->Answer.FloatingPoint[1],Txt_NEW_LINE);
         break;
      case Tst_ANS_TRUE_FALSE:
         fprintf (FileXML,"%s",
                  Question->Answer.TF == 'T' ? "true" :
                	                       "false");
         break;
      case Tst_ANS_UNIQUE_CHOICE:
      case Tst_ANS_MULTIPLE_CHOICE:
      case Tst_ANS_TEXT:
         fprintf (FileXML,"%s",Txt_NEW_LINE);
         for (NumOpt = 0;
              NumOpt < Question->Answer.NumOptions;
              NumOpt++)
           {
            /* Start answer */
            fprintf (FileXML,"<option");

            /* Write whether the answer is correct or not */
            if (Question->Answer.Type != Tst_ANS_TEXT)
               fprintf (FileXML," correct=\"%s\"",
                        Question->Answer.Options[NumOpt].Correct ? "yes" :
                                                                   "no");

            fprintf (FileXML,">%s",Txt_NEW_LINE);

            /* Write the answer, that is in HTML */
            fprintf (FileXML,"<text>%s</text>%s",
                     Question->Answer.Options[NumOpt].Text,Txt_NEW_LINE);

            /* Write the feedback */
            if (Question->Answer.Options[NumOpt].Feedback)
	       if (Question->Answer.Options[NumOpt].Feedback[0])
		  fprintf (FileXML,"<feedback>%s</feedback>%s",
			   Question->Answer.Options[NumOpt].Feedback,Txt_NEW_LINE);

            /* End answer */
            fprintf (FileXML,"</option>%s",
                     Txt_NEW_LINE);
           }
	 break;
      default:
         break;
     }
  }

/*****************************************************************************/
/************ Get questions from XML and store them in database **************/
/*****************************************************************************/

void TsI_ImportQstsFromXML (void)
  {
   extern const char *Txt_The_file_is_not_X;
   struct Param *Param;
   char FileNameXMLSrc[PATH_MAX + 1];
   char FileNameXMLTmp[PATH_MAX + 1];	// Full name (including path and .xml) of the destination temporary file
   char MIMEType[Brw_MAX_BYTES_MIME_TYPE + 1];
   bool WrongType = false;

   /***** Creates directory if not exists *****/
   Fil_CreateDirIfNotExists (Cfg_PATH_TEST_PRIVATE);

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
      Ale_ShowAlert (Ale_WARNING,Txt_The_file_is_not_X,
		     "xml");
   else
     {
      /* End the reception of XML in a temporary file */
      snprintf (FileNameXMLTmp,sizeof (FileNameXMLTmp),"%s/%s.xml",
		Cfg_PATH_TEST_PRIVATE,Gbl.UniqueNameEncrypted);
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
   FILE *FileXML;
   char *XMLBuffer;
   unsigned long FileSize;

   /***** Open file *****/
   if ((FileXML = fopen (FileNameXML,"rb")) == NULL)
      Lay_ShowErrorAndExit ("Can not open XML file.");

   /***** Compute file size *****/
   fseek (FileXML,0L,SEEK_END);
   FileSize = (unsigned long) ftell (FileXML);
   fseek (FileXML,0L,SEEK_SET);

   /***** Allocate memory for XML buffer *****/
   if ((XMLBuffer = malloc (FileSize + 1)) == NULL)
      Lay_NotEnoughMemoryExit ();
   else
     {
      /***** Read file contents into XML buffer *****/
      if (fread (XMLBuffer,sizeof (char),(size_t) FileSize,FileXML))
         XMLBuffer[FileSize] = '\0';
      else
         XMLBuffer[0] = '\0';

      /***** Import questions from XML buffer *****/
      TsI_ImportQuestionsFromXMLBuffer (XMLBuffer);

      free (XMLBuffer);
     }

   /***** Close file *****/
   fclose (FileXML);
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
   struct Tst_Question Question;
   bool QuestionExists;
   bool AnswerTypeFound;

   /***** Allocate and get XML tree *****/
   XML_GetTree (XMLBuffer,&RootElem);

   /***** Begin box *****/
   Box_BoxBegin (NULL,Txt_Imported_questions,
                 NULL,NULL,
                 Hlp_ASSESSMENT_Tests,Box_NOT_CLOSABLE);

   /***** Print XML tree *****/
   HTM_DIV_Begin ("class=\"TEST_FILE_CONTENT\"");
   HTM_TEXTAREA_Begin ("title=\"%s\" cols=\"60\" rows=\"5\""
	               " spellcheck=\"false\" readonly",
	               Txt_XML_file_content);
   XML_PrintTree (RootElem);
   HTM_TEXTAREA_End ();
   HTM_DIV_End ();

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
      HTM_TABLE_BeginWideMarginPadding (5);
      TsI_WriteHeadingListImportedQst ();

      /***** For each question... *****/
      for (QuestionElem = TestElem->FirstChild;
	   QuestionElem != NULL;
	   QuestionElem = QuestionElem->NextBrother)
	{
	 if (!strcmp (QuestionElem->TagName,"question"))
	   {
	    /***** Create test question *****/
	    Tst_QstConstructor (&Question);

	    /* Get answer type (in mandatory attribute "type") */
	    AnswerTypeFound = false;
	    for (Attribute = QuestionElem->FirstAttribute;
		 Attribute != NULL;
		 Attribute = Attribute->Next)
	       if (!strcmp (Attribute->AttributeName,"type"))
		 {
		  Question.Answer.Type = TsI_ConvertFromStrAnsTypXMLToAnsTyp (Attribute->Content);
		  AnswerTypeFound = true;
		  break;	// Only first attribute "type"
		 }

	    if (AnswerTypeFound)
	      {
	       /* Get tags */
	       for (TagsElem = QuestionElem->FirstChild, Question.Tags.Num = 0;
		    TagsElem != NULL;
		    TagsElem = TagsElem->NextBrother)
		  if (!strcmp (TagsElem->TagName,"tags"))
		    {
		     for (TagElem = TagsElem->FirstChild;
			  TagElem != NULL && Question.Tags.Num < Tag_MAX_TAGS_PER_QUESTION;
			  TagElem = TagElem->NextBrother)
			if (!strcmp (TagElem->TagName,"tag"))
			  {
			   if (TagElem->Content)
			     {
			      Str_Copy (Question.Tags.Txt[Question.Tags.Num],
					TagElem->Content,
					sizeof (Question.Tags.Txt[Question.Tags.Num]) - 1);
			      Question.Tags.Num++;
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
			Str_Copy (Question.Stem,StemElem->Content,Cns_MAX_BYTES_TEXT);
			Str_ChangeFormat (Str_FROM_TEXT,Str_TO_HTML,
					  Question.Stem,Cns_MAX_BYTES_TEXT,true);
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
			Str_Copy (Question.Feedback,FeedbackElem->Content,Cns_MAX_BYTES_TEXT);
			Str_ChangeFormat (Str_FROM_TEXT,Str_TO_HTML,
					  Question.Feedback,Cns_MAX_BYTES_TEXT,true);
		       }
		     break;	// Only first element "feedback"
		    }

	       /* Get shuffle. By default, shuffle is false. */
	       Question.Answer.Shuffle = false;
	       for (AnswerElem = QuestionElem->FirstChild;
		    AnswerElem != NULL;
		    AnswerElem = AnswerElem->NextBrother)
		  if (!strcmp (AnswerElem->TagName,"answer"))
		    {
		     if (Question.Answer.Type == Tst_ANS_UNIQUE_CHOICE ||
			 Question.Answer.Type == Tst_ANS_MULTIPLE_CHOICE)
			/* Get whether shuffle answers (in attribute "shuffle") */
			for (Attribute = AnswerElem->FirstAttribute;
			     Attribute != NULL;
			     Attribute = Attribute->Next)
			   if (!strcmp (Attribute->AttributeName,"shuffle"))
			     {
			      Question.Answer.Shuffle = XML_GetAttributteYesNoFromXMLTree (Attribute);
			      break;	// Only first attribute "shuffle"
			     }
		     break;	// Only first element "answer"
		    }

	       /* Get answer (mandatory) */
	       TsI_GetAnswerFromXML (AnswerElem,&Question);

	       /* Make sure that tags, text and answer are not empty */
	       if (Tst_CheckIfQstFormatIsCorrectAndCountNumOptions (&Question))
		 {
		  /* Check if question already exists in database */
		  QuestionExists = Tst_CheckIfQuestionExistsInDB (&Question);

		  /* Write row with this imported question */
		  TsI_WriteRowImportedQst (StemElem,FeedbackElem,
		                           &Question,QuestionExists);

		  /***** If a new question ==> insert question, tags and answer in the database *****/
		  if (!QuestionExists)
		    {
		     Question.QstCod = -1L;
		     Tst_InsertOrUpdateQstTagsAnsIntoDB (&Question);
		     if (Question.QstCod <= 0)
			Lay_ShowErrorAndExit ("Can not create question.");
		    }
		 }
	      }
	    else	// Answer type not found
	       Lay_ShowErrorAndExit ("Wrong type of answer.");

	    /***** Destroy test question *****/
	    Tst_QstDestructor (&Question);
	   }
	}

      HTM_TABLE_End ();
     }
   else	// TestElem not found
      Ale_ShowAlert (Ale_ERROR,"Root element &lt;test&gt; not found.");

   /***** End table *****/
   Box_BoxEnd ();

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
      for (AnsType  = (Tst_AnswerType_t) 0;
	   AnsType <= (Tst_AnswerType_t) (Tst_NUM_ANS_TYPES - 1);
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
// Answer is mandatory

static void TsI_GetAnswerFromXML (struct XMLElement *AnswerElem,
                                  struct Tst_Question *Question)
  {
   struct XMLElement *OptionElem;
   struct XMLElement *TextElem;
   struct XMLElement *FeedbackElem;
   struct XMLElement *LowerUpperElem;
   struct XMLAttribute *Attribute;
   unsigned NumOpt;

   switch (Question->Answer.Type)
     {
      case Tst_ANS_INT:
         if (!Tst_AllocateTextChoiceAnswer (Question,0))
	    /* Abort on error */
	    Ale_ShowAlertsAndExit ();

         if (AnswerElem->Content)
            Str_Copy (Question->Answer.Options[0].Text,AnswerElem->Content,
                      Tst_MAX_BYTES_ANSWER_OR_FEEDBACK);
         break;
      case Tst_ANS_FLOAT:
         if (!Tst_AllocateTextChoiceAnswer (Question,0))
	    /* Abort on error */
	    Ale_ShowAlertsAndExit ();
         if (!Tst_AllocateTextChoiceAnswer (Question,1))
	    /* Abort on error */
	    Ale_ShowAlertsAndExit ();

         for (LowerUpperElem = AnswerElem->FirstChild;
              LowerUpperElem != NULL;
              LowerUpperElem = LowerUpperElem->NextBrother)
            if (!strcmp (LowerUpperElem->TagName,"lower"))
              {
               if (LowerUpperElem->Content)
                  Str_Copy (Question->Answer.Options[0].Text,
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
                  Str_Copy (Question->Answer.Options[1].Text,
                            LowerUpperElem->Content,
                            Tst_MAX_BYTES_ANSWER_OR_FEEDBACK);
               break;	// Only first element "upper"
              }
         break;
      case Tst_ANS_TRUE_FALSE:
	 // Comparisons must be case insensitive, because users can edit XML
         if (!AnswerElem->Content)
            Question->Answer.TF = ' ';
         else if (!strcasecmp (AnswerElem->Content,"true")  ||
                  !strcasecmp (AnswerElem->Content,"T")     ||
                  !strcasecmp (AnswerElem->Content,"yes")   ||
                  !strcasecmp (AnswerElem->Content,"Y"))
            Question->Answer.TF = 'T';
         else if (!strcasecmp (AnswerElem->Content,"false") ||
                  !strcasecmp (AnswerElem->Content,"F")     ||
                  !strcasecmp (AnswerElem->Content,"no")    ||
                  !strcasecmp (AnswerElem->Content,"N"))
            Question->Answer.TF = 'F';
         else
            Question->Answer.TF = ' ';
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
               if (!Tst_AllocateTextChoiceAnswer (Question,NumOpt))
		  /* Abort on error */
		  Ale_ShowAlertsAndExit ();

	       for (TextElem = OptionElem->FirstChild;
		    TextElem != NULL;
		    TextElem = TextElem->NextBrother)
		  if (!strcmp (TextElem->TagName,"text"))
		    {
		     if (TextElem->Content)
		       {
			Str_Copy (Question->Answer.Options[NumOpt].Text,
			          TextElem->Content,
			          Tst_MAX_BYTES_ANSWER_OR_FEEDBACK);

			/* Convert answer from text to HTML (in database answer text is stored in HTML) */
			Str_ChangeFormat (Str_FROM_TEXT,Str_TO_HTML,
			                  Question->Answer.Options[NumOpt].Text,
			                  Tst_MAX_BYTES_ANSWER_OR_FEEDBACK,true);
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
			Str_Copy (Question->Answer.Options[NumOpt].Feedback,
			          FeedbackElem->Content,
			          Tst_MAX_BYTES_ANSWER_OR_FEEDBACK);

			/* Convert feedback from text to HTML (in database answer feedback is stored in HTML) */
			Str_ChangeFormat (Str_FROM_TEXT,Str_TO_HTML,
			                  Question->Answer.Options[NumOpt].Feedback,
			                  Tst_MAX_BYTES_ANSWER_OR_FEEDBACK,true);
		       }
		     break;	// Only first element "feedback"
		    }

	       if (Question->Answer.Type == Tst_ANS_TEXT)
		  Question->Answer.Options[NumOpt].Correct = true;
	       else
		  /* Check if option is correct or wrong */
		  for (Attribute = OptionElem->FirstAttribute;
		       Attribute != NULL;
		       Attribute = Attribute->Next)
		     if (!strcmp (Attribute->AttributeName,"correct"))
		       {
			Question->Answer.Options[NumOpt].Correct = XML_GetAttributteYesNoFromXMLTree (Attribute);
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
   HTM_TR_Begin (NULL);

   HTM_TH_Empty (1);

   HTM_TH (1,1,"CT",Txt_No_INDEX);
   HTM_TH (1,1,"CT",Txt_Tags);
   HTM_TH (1,1,"CT",Txt_Type);
   HTM_TH (1,1,"CT",Txt_Shuffle);
   HTM_TH (1,1,"LT",Txt_Question);

   HTM_TR_End ();
  }

/*****************************************************************************/
/**************** Write a row with one imported test question ****************/
/*****************************************************************************/

static void TsI_WriteRowImportedQst (struct XMLElement *StemElem,
                                     struct XMLElement *FeedbackElem,
                                     const struct Tst_Question *Question,
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
   const char *ClassStem = QuestionExists ? "TEST_TXT_LIGHT" :
	                                    "TEST_TXT";

   Gbl.RowEvenOdd = NumQst % 2;
   NumQst++;

   HTM_TR_Begin (NULL);

   /***** Put icon to indicate that a question does not exist in database *****/
   HTM_TD_Begin ("class=\"BT%u CT\"",Gbl.RowEvenOdd);
   Ico_PutIcon (QuestionExists ? "tr16x16.gif" :
        	                 "check-circle.svg",
		QuestionExists ? Txt_Existing_question :
        	                 Txt_New_question,
		"CONTEXT_ICO_16x16");
   HTM_TD_End ();

   /***** Write number of question *****/
   HTM_TD_Begin ("class=\"%s CT COLOR%u\"",ClassData,Gbl.RowEvenOdd);
   if (!QuestionExists)
      HTM_TxtF ("%u&nbsp;",++NumNonExistingQst);
   HTM_TD_End ();

   /***** Write the question tags *****/
   HTM_TD_Begin ("class=\"LT COLOR%u\"",Gbl.RowEvenOdd);
   if (Question->Tags.Num)
     {
      /***** Write the tags *****/
      HTM_TABLE_Begin (NULL);
      for (NumTag = 0;
	   NumTag < Question->Tags.Num;
	   NumTag++)
	{
         HTM_TR_Begin (NULL);

	 HTM_TD_Begin ("class=\"%s LT\"",ClassData);
	 HTM_TxtF ("&nbsp;%s&nbsp;","&#8226;");
	 HTM_TD_End ();

         HTM_TD_Begin ("class=\"%s LT\"",ClassData);
         HTM_Txt (Question->Tags.Txt[NumTag]);
         HTM_TD_End ();

	 HTM_TR_End ();
	}
      HTM_TABLE_End ();
     }
   else	// no tags for this question
     {
      HTM_SPAN_Begin ("class=\"%s\"",ClassData);
      HTM_TxtF ("&nbsp;(%s)&nbsp;",Txt_no_tags);
      HTM_SPAN_End ();
     }

   HTM_TD_End ();

   /***** Write the question type *****/
   HTM_TD_Begin ("class=\"%s CT COLOR%u\"",ClassData,Gbl.RowEvenOdd);
   HTM_TxtF ("%s&nbsp;",Txt_TST_STR_ANSWER_TYPES[Question->Answer.Type]);
   HTM_TD_End ();

   /***** Write if shuffle is enabled *****/
   HTM_TD_Begin ("class=\"CT COLOR%u\"",Gbl.RowEvenOdd);
   if (Question->Answer.Type == Tst_ANS_UNIQUE_CHOICE ||
       Question->Answer.Type == Tst_ANS_MULTIPLE_CHOICE)
      /* Put an icon that indicates whether shuffle is enabled or not */
      if (Question->Answer.Shuffle)
	 Ico_PutIcon ("check.svg",Txt_TST_Answer_given_by_the_teachers,
		      QuestionExists ? "ICO_HIDDEN ICO16x16" :
                	               "ICO16x16");
   HTM_TD_End ();

   /***** Write the stem and the answers *****/
   HTM_TD_Begin ("class=\"LT COLOR%u\"",Gbl.RowEvenOdd);
   Tst_WriteQstStem (Stem,ClassStem,
		     true);	// Visible
   Tst_WriteQstFeedback (Feedback,"TEST_TXT_LIGHT");
   switch (Question->Answer.Type)
     {
      case Tst_ANS_INT:
         HTM_SPAN_Begin ("class=\"%s\"",ClassStem);
         HTM_TxtF ("(%ld)",Question->Answer.Integer);
         HTM_SPAN_End ();
         break;
      case Tst_ANS_FLOAT:
         HTM_SPAN_Begin ("class=\"%s\"",ClassStem);
         HTM_Txt ("([");
         HTM_Double (Question->Answer.FloatingPoint[0]);
         HTM_Txt ("; ");
         HTM_Double (Question->Answer.FloatingPoint[1]);
         HTM_Txt ("])");
         HTM_SPAN_End ();
         break;
      case Tst_ANS_TRUE_FALSE:
         HTM_SPAN_Begin ("class=\"%s\"",ClassStem);
         HTM_Txt ("(");
         Tst_WriteAnsTF (Question->Answer.TF);
         HTM_Txt (")");
         HTM_SPAN_End ();
         break;
      case Tst_ANS_UNIQUE_CHOICE:
      case Tst_ANS_MULTIPLE_CHOICE:
      case Tst_ANS_TEXT:
         HTM_TABLE_Begin (NULL);
         for (NumOpt = 0;
              NumOpt < Question->Answer.NumOptions;
              NumOpt++)
           {
            /* Convert the answer, that is in HTML, to rigorous HTML */
            AnswerTextLength = strlen (Question->Answer.Options[NumOpt].Text) *
        	               Str_MAX_BYTES_PER_CHAR;
            if ((AnswerText = malloc (AnswerTextLength + 1)) == NULL)
               Lay_NotEnoughMemoryExit ();
            Str_Copy (AnswerText,Question->Answer.Options[NumOpt].Text,
                      AnswerTextLength);
            Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
                              AnswerText,AnswerTextLength,false);

            /* Convert the feedback, that is in HTML, to rigorous HTML */
            AnswerFeedbackLength = 0;
            AnswerFeedback = NULL;
            if (Question->Answer.Options[NumOpt].Feedback)
               if (Question->Answer.Options[NumOpt].Feedback[0])
		 {
	          AnswerFeedbackLength = strlen (Question->Answer.Options[NumOpt].Feedback) *
					 Str_MAX_BYTES_PER_CHAR;
	          if ((AnswerFeedback = malloc (AnswerFeedbackLength + 1)) == NULL)
		     Lay_NotEnoughMemoryExit ();
		  Str_Copy (AnswerFeedback,
		            Question->Answer.Options[NumOpt].Feedback,
		            AnswerFeedbackLength);
		  Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
		                    AnswerFeedback,AnswerFeedbackLength,false);
		 }

            HTM_TR_Begin (NULL);

            /* Put an icon that indicates whether the answer is correct or wrong */
            HTM_TD_Begin ("class=\"BT%u\"",Gbl.RowEvenOdd);
            if (Question->Answer.Options[NumOpt].Correct)
               Ico_PutIcon ("check.svg",Txt_TST_Answer_given_by_the_teachers,
		            QuestionExists ? "ICO_HIDDEN CONTEXT_ICO_16x16" :
                	                     "CONTEXT_ICO_16x16");
            HTM_TD_End ();

            /* Write the number of option */
            HTM_TD_Begin ("class=\"%s LT\"",ClassData);
            HTM_TxtF ("%c)&nbsp;",'a' + (char) NumOpt);
            HTM_TD_End ();

            /* Write the text and the feedback of the answer */
            HTM_TD_Begin ("class=\"LT\"");

            HTM_DIV_Begin ("class=\"%s\"",ClassStem);
            HTM_Txt (AnswerText);
            HTM_DIV_End ();

            if (AnswerFeedbackLength)
              {
	       HTM_DIV_Begin ("class=\"TEST_TXT_LIGHT\"");
	       HTM_Txt (AnswerFeedback);
	       HTM_DIV_End ();
              }

            HTM_TD_End ();

            HTM_TR_End ();

	    /* Free memory allocated for the answer and the feedback */
	    free (AnswerText);
            if (AnswerFeedbackLength)
	       free (AnswerFeedback);
           }
         HTM_TABLE_End ();
	 break;
      default:
         break;
     }
   HTM_TD_End ();
   HTM_TR_End ();
  }
