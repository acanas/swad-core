// swad_test_import.c: import and export self-assessment tests using XML files

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

#include <stdlib.h>		// For exit, system, malloc, free, etc
#include <string.h>		// For string functions
#include <sys/stat.h>		// For mkdir
#include <sys/types.h>		// For mkdir

#include "swad_action_list.h"
#include "swad_alert.h"
#include "swad_box.h"
#include "swad_database.h"
#include "swad_error.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_HTML.h"
#include "swad_parameter.h"
#include "swad_question_tf.h"
#include "swad_tag_database.h"
#include "swad_test.h"
#include "swad_xml.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void QstImp_PutParsExportQsts (void *Questions);
static void QstImp_PutCreateXMLPar (void);

static void QstImp_ExportQuestion (struct Qst_Question *Qst,FILE *FileXML);

static void QstImp_GetAndWriteTagsXML (long QstCod,FILE *FileXML);
static void QstImp_WriteAnswersOfAQstXML (const struct Qst_Question *Qst,
                                          FILE *FileXML);
static void QstImp_ReadQuestionsFromXMLFileAndStoreInDB (const char *FileNameXML);
static void QstImp_ImportQuestionsFromXMLBuffer (const char *XMLBuffer);
static Qst_AnswerType_t QstImp_ConvertFromStrAnsTypXMLToAnsTyp (const char *StrAnsTypeXML);
static void QstImp_GetAnswerFromXML (struct XMLElement *AnswerElem,
                                     struct Qst_Question *Qst);
static void QstImp_WriteHeadingListImportedQst (void);
static void QstImp_WriteRowImportedQst (struct XMLElement *StemElem,
                                        struct XMLElement *FeedbackElem,
                                        const struct Qst_Question *Qst,
                                        Exi_Exist_t QuestionExists);

/*****************************************************************************/
/**************** Put a link (form) to export test questions *****************/
/*****************************************************************************/

void QstImp_PutIconToExportQuestions (struct Qst_Questions *Qsts)
  {
   Lay_PutContextualLinkOnlyIcon (ActExpTstQst,NULL,
                                  QstImp_PutParsExportQsts,Qsts,
				  "file-import.svg",Ico_BLACK);
  }

/*****************************************************************************/
/****************** Put params to export test questions **********************/
/*****************************************************************************/

static void QstImp_PutParsExportQsts (void *Questions)
  {
   if (Questions)
     {
      Qst_PutParsEditQst (Questions);
      Par_PutParChar ("OnlyThisQst",'N');
      Par_PutParUnsigned (NULL,"Order",(unsigned) (((struct Qst_Questions *) Questions)->SelectedOrder));
      QstImp_PutCreateXMLPar ();
     }
  }

/*****************************************************************************/
/************************ Parameter to create XML file ***********************/
/*****************************************************************************/

static void QstImp_PutCreateXMLPar (void)
  {
   Par_PutParChar ("CreateXML",'Y');
  }

bool QstImp_GetCreateXMLParFromForm (void)
  {
   return Par_GetParBool ("CreateXML");
  }

/*****************************************************************************/
/*************** Put a link (form) to import test questions ******************/
/*****************************************************************************/

void QstImp_PutIconToImportQuestions (void)
  {
   Lay_PutContextualLinkOnlyIcon (ActReqImpTstQst,NULL,
                                  NULL,NULL,
				  "file-export.svg",Ico_BLACK);
  }

/*****************************************************************************/
/*********** Show form to import test questions from an XML file *************/
/*****************************************************************************/

void QstImp_ShowFormImpQstsFromXML (void)
  {
   extern const char *Hlp_ASSESSMENT_Tests;
   extern const char *Txt_Import_questions;
   extern const char *Txt_You_need_an_XML_file_containing_a_list_of_questions;
   extern const char *Txt_XML_file;

   /***** Begin box *****/
   Box_BoxBegin (Txt_Import_questions,NULL,NULL,
                 Hlp_ASSESSMENT_Tests,Box_NOT_CLOSABLE);

      /***** Write help message *****/
      Ale_ShowAlert (Ale_INFO,Txt_You_need_an_XML_file_containing_a_list_of_questions);

      /***** Write a form to import questions *****/
      Frm_BeginForm (ActImpTstQst);
	 HTM_LABEL_Begin ("class=\"FORM_IN_%s\"",The_GetSuffix ());
	    HTM_Txt (Txt_XML_file); HTM_Colon (); HTM_NBSP ();
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

void QstImp_CreateXML (unsigned NumQsts,MYSQL_RES *mysql_res)
  {
   extern const char *Txt_NEW_LINE;
   extern const char *Txt_XML_file;
   char PathPubFile[PATH_MAX + 1];
   FILE *FileXML;
   unsigned NumQst;
   struct Qst_Question Qst;
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
      Err_ShowErrorAndExit ("Can not open target file.");

   /***** Begin XML file *****/
   XML_WriteStartFile (FileXML,"test");
   fprintf (FileXML,"%s",Txt_NEW_LINE);

   /***** Write rows *****/
   for (NumQst = 0;
	NumQst < NumQsts;
	NumQst++)
     {
      /* Create question */
      Qst_QstConstructor (&Qst);

	 /* Get question code (row[0]) */
	 row = mysql_fetch_row (mysql_res);
	 if ((Qst.QstCod = Str_ConvertStrCodToLongCod (row[0])) <= 0)
	    Err_WrongQuestionExit ();

	 QstImp_ExportQuestion (&Qst,FileXML);

      /* Destroy question */
      Qst_QstDestructor (&Qst);
     }

   /***** End XML file *****/
   XML_WriteEndFile (FileXML,"test");

   /***** Close the XML file *****/
   fclose (FileXML);

   /***** Return to start of query result *****/
   mysql_data_seek (mysql_res,0);

   /***** Write the link to XML file *****/
   HTM_A_Begin ("href=\"%s/%s/%s/test.xml\" target=\"_blank\""
	        " class=\"FORM_OUT_%s BOLD\"",
	        Cfg_URL_FILE_BROWSER_TMP_PUBLIC,
	        Gbl.FileBrowser.TmpPubDir.L,
	        Gbl.FileBrowser.TmpPubDir.R,
	        The_GetSuffix ());
      Ico_PutIconTextLink ("file.svg",Ico_BLACK,Txt_XML_file);
   HTM_A_End ();
  }

/*****************************************************************************/
/****************** Write one question into the XML file *********************/
/*****************************************************************************/

static void QstImp_ExportQuestion (struct Qst_Question *Qst,FILE *FileXML)
  {
   extern const char *Qst_StrAnswerTypesXML[Qst_NUM_ANS_TYPES];
   extern const char *Txt_NEW_LINE;
   static const char *ShuffleYesNo[Qst_NUM_SHUFFLE] =
     {
      [Qst_DONT_SHUFFLE] = "no",
      [Qst_SHUFFLE     ] = "yes"
     };

   if (Qst_GetQstDataByCod (Qst) == Exi_EXISTS)
     {
      /***** Write the answer type *****/
      fprintf (FileXML,"<question type=\"%s\">%s",
               Qst_StrAnswerTypesXML[Qst->Answer.Type],Txt_NEW_LINE);

      /***** Write the question tags *****/
      fprintf (FileXML,"<tags>%s",Txt_NEW_LINE);
      QstImp_GetAndWriteTagsXML (Qst->QstCod,FileXML);
      fprintf (FileXML,"</tags>%s",Txt_NEW_LINE);

      /***** Write the stem, that is in HTML format *****/
      fprintf (FileXML,"<stem>%s</stem>%s",
               Qst->Stem,Txt_NEW_LINE);

      /***** Write the feedback, that is in HTML format *****/
      if (Qst->Feedback[0])
	 fprintf (FileXML,"<feedback>%s</feedback>%s",
		  Qst->Feedback,Txt_NEW_LINE);

      /***** Write the answers of this question.
             Shuffle can be enabled or disabled *****/
      fprintf (FileXML,"<answer");
      if (Qst->Answer.Type == Qst_ANS_UNIQUE_CHOICE ||
          Qst->Answer.Type == Qst_ANS_MULTIPLE_CHOICE)
         fprintf (FileXML," shuffle=\"%s\"",
                  ShuffleYesNo[Qst->Answer.Shuffle]);
      fprintf (FileXML,">");
      QstImp_WriteAnswersOfAQstXML (Qst,FileXML);
      fprintf (FileXML,"</answer>%s",Txt_NEW_LINE);

      /***** End question *****/
      fprintf (FileXML,"</question>%s%s",
               Txt_NEW_LINE,Txt_NEW_LINE);
     }
  }

/*****************************************************************************/
/************* Get and write tags of a question into the XML file ************/
/*****************************************************************************/

static void QstImp_GetAndWriteTagsXML (long QstCod,FILE *FileXML)
  {
   extern const char *Txt_NEW_LINE;
   unsigned NumTags;
   unsigned NumTag;
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;

   if ((NumTags = Tag_DB_GetTagsQst (&mysql_res,QstCod)))	// Result: TagTxt
      /***** Write the tags *****/
      for (NumTag  = 1;
	   NumTag <= NumTags;
	   NumTag++)
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

static void QstImp_WriteAnswersOfAQstXML (const struct Qst_Question *Qst,
                                          FILE *FileXML)
  {
   extern const char *Txt_NEW_LINE;
   static const char *Correct_YesNo[Qst_NUM_WRONG_CORRECT] =
     {
      [Qst_BLANK  ] = "",
      [Qst_WRONG  ] = "No",
      [Qst_CORRECT] = "Yes",
     };
   unsigned NumOpt;

   /***** Write answers *****/
   switch (Qst->Answer.Type)
     {
      case Qst_ANS_INT:
         fprintf (FileXML,"%ld",Qst->Answer.Integer);
         break;
      case Qst_ANS_FLOAT:
	 Str_SetDecimalPointToUS ();	// To write the decimal point as a dot
         fprintf (FileXML,"%s"
                          "<lower>%.15lg</lower>%s"
                          "<upper>%.15lg</upper>%s",
                  Txt_NEW_LINE,
                  Qst->Answer.FloatingPoint[0],Txt_NEW_LINE,
                  Qst->Answer.FloatingPoint[1],Txt_NEW_LINE);
	 Str_SetDecimalPointToLocal ();	// Return to local system
         break;
      case Qst_ANS_TRUE_FALSE:
         fprintf (FileXML,"%s",
                  Qst->Answer.OptionTF == QstTF__OPTION_TRUE ? "true" :
							    "false");
         break;
      case Qst_ANS_UNIQUE_CHOICE:
      case Qst_ANS_MULTIPLE_CHOICE:
      case Qst_ANS_TEXT:
         fprintf (FileXML,"%s",Txt_NEW_LINE);
         for (NumOpt = 0;
              NumOpt < Qst->Answer.NumOpts;
              NumOpt++)
           {
            /* Begin answer */
            fprintf (FileXML,"<option");

            /* Write whether the answer is correct or not */
            if (Qst->Answer.Type != Qst_ANS_TEXT)
               fprintf (FileXML," correct=\"%s\"",
                        Correct_YesNo[Qst->Answer.Options[NumOpt].Correct]);

            fprintf (FileXML,">%s",Txt_NEW_LINE);

            /* Write the answer, that is in HTML */
            fprintf (FileXML,"<text>%s</text>%s",
                     Qst->Answer.Options[NumOpt].Text,Txt_NEW_LINE);

            /* Write the feedback */
            if (Qst->Answer.Options[NumOpt].Feedback)
	       if (Qst->Answer.Options[NumOpt].Feedback[0])
		  fprintf (FileXML,"<feedback>%s</feedback>%s",
			   Qst->Answer.Options[NumOpt].Feedback,Txt_NEW_LINE);

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

void QstImp_ImpQstsFromXML (void)
  {
   extern const char *Txt_The_file_is_not_X;
   struct Par_Param *Par;
   char FileNameXMLSrc[PATH_MAX + 1];
   char FileNameXMLTmp[PATH_MAX + 1];	// Full name (including path and .xml) of the destination temporary file
   char MIMEType[Brw_MAX_BYTES_MIME_TYPE + 1];
   Err_SuccessOrError_t SuccessOrError;

   /***** Creates directory if not exists *****/
   Fil_CreateDirIfNotExists (Cfg_PATH_TEST_PRIVATE);

   /***** First of all, copy in disk the file received *****/
   Par = Fil_StartReceptionOfFile (Fil_NAME_OF_PARAM_FILENAME_ORG,
                                   FileNameXMLSrc,MIMEType);

   /* Check if the file type is XML */
   SuccessOrError = Err_SUCCESS;
   if (strcmp (MIMEType,"text/xml"))
      if (strcmp (MIMEType,"application/xml"))
	 if (strcmp (MIMEType,"application/octet-stream"))
	    if (strcmp (MIMEType,"application/octetstream"))
	       if (strcmp (MIMEType,"application/octet"))
                  SuccessOrError = Err_ERROR;

   switch (SuccessOrError)
     {
      case Err_SUCCESS:
	 /* End the reception of XML in a temporary file */
	 snprintf (FileNameXMLTmp,sizeof (FileNameXMLTmp),"%s/%s.xml",
		   Cfg_PATH_TEST_PRIVATE,Cry_GetUniqueNameEncrypted ());
	 switch (Fil_EndReceptionOfFile (FileNameXMLTmp,Par))
	   {
	    case Err_SUCCESS:
	       /***** Get questions from XML file and store them in database *****/
	       QstImp_ReadQuestionsFromXMLFileAndStoreInDB (FileNameXMLTmp);
	       break;
	    case Err_ERROR:
	    default:
	       Ale_ShowAlert (Ale_WARNING,"Error copying file.");
	       break;
	   }
	 break;
      case Err_ERROR:
      default:
	 Ale_ShowAlert (Ale_WARNING,Txt_The_file_is_not_X,"xml");
	 break;
     }
  }

/*****************************************************************************/
/********** Get questions from XML file and store them in database ***********/
/*****************************************************************************/

static void QstImp_ReadQuestionsFromXMLFileAndStoreInDB (const char *FileNameXML)
  {
   FILE *FileXML;
   char *XMLBuffer;
   unsigned long FileSize;

   /***** Open file *****/
   if ((FileXML = fopen (FileNameXML,"rb")) == NULL)
      Err_ShowErrorAndExit ("Can not open XML file.");

   /***** Compute file size *****/
   fseek (FileXML,0L,SEEK_END);
   FileSize = (unsigned long) ftell (FileXML);
   fseek (FileXML,0L,SEEK_SET);

   /***** Allocate memory for XML buffer *****/
   if ((XMLBuffer = malloc (FileSize + 1)) == NULL)
      Err_NotEnoughMemoryExit ();
   else
     {
      /***** Read file contents into XML buffer *****/
      if (fread (XMLBuffer,sizeof (char),(size_t) FileSize,FileXML))
         XMLBuffer[FileSize] = '\0';
      else
         XMLBuffer[0] = '\0';

      /***** Import questions from XML buffer *****/
      QstImp_ImportQuestionsFromXMLBuffer (XMLBuffer);

      free (XMLBuffer);
     }

   /***** Close file *****/
   fclose (FileXML);
  }

/*****************************************************************************/
/******************** Import questions from XML buffer ***********************/
/*****************************************************************************/

static void QstImp_ImportQuestionsFromXMLBuffer (const char *XMLBuffer)
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
   struct Qst_Question Qst;
   Exi_Exist_t QstExists;
   Err_SuccessOrError_t AnswerTypeFound;

   /***** Allocate and get XML tree *****/
   XML_GetTree (XMLBuffer,&RootElem);

   /***** Begin box *****/
   Box_BoxBegin (Txt_Imported_questions,NULL,NULL,
                 Hlp_ASSESSMENT_Tests,Box_NOT_CLOSABLE);

      /***** Print XML tree *****/
      HTM_DIV_Begin ("class=\"Tst_FILE_CONTENT\"");
	 HTM_TEXTAREA_Begin (HTM_DISABLED,
			     "title=\"%s\" cols=\"60\" rows=\"5\""
			     " spellcheck=\"false\"",Txt_XML_file_content);
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

	 /***** Begin table *****/
	 HTM_TABLE_BeginWideMarginPadding (5);

	    /***** Write heading of list of imported questions *****/
	    QstImp_WriteHeadingListImportedQst ();

	    /***** For each question... *****/
	    for (QuestionElem  = TestElem->FirstChild;
		 QuestionElem != NULL;
		 QuestionElem  = QuestionElem->NextBrother)
	      {
	       if (!strcmp (QuestionElem->TagName,"question"))
		 {
		  /***** Create question *****/
		  Qst_QstConstructor (&Qst);

		     /* Get answer type (in mandatory attribute "type") */
		     AnswerTypeFound = Err_ERROR;
		     for (Attribute  = QuestionElem->FirstAttribute;
			  Attribute != NULL;
			  Attribute  = Attribute->Next)
			if (!strcmp (Attribute->AttributeName,"type"))
			  {
			   Qst.Answer.Type = QstImp_ConvertFromStrAnsTypXMLToAnsTyp (Attribute->Content);
			   AnswerTypeFound = Err_SUCCESS;
			   break;	// Only first attribute "type"
			  }

		     switch (AnswerTypeFound)
		       {
		        case Err_SUCCESS:
			   /* Get tags */
			   for (TagsElem  = QuestionElem->FirstChild, Qst.Tags.Num = 0;
				TagsElem != NULL;
				TagsElem  = TagsElem->NextBrother)
			      if (!strcmp (TagsElem->TagName,"tags"))
				{
				 for (TagElem = TagsElem->FirstChild;
				      TagElem != NULL && Qst.Tags.Num < Tag_MAX_TAGS_PER_QUESTION;
				      TagElem = TagElem->NextBrother)
				    if (!strcmp (TagElem->TagName,"tag"))
				      {
				       if (TagElem->Content)
					 {
					  Str_Copy (Qst.Tags.Txt[Qst.Tags.Num],
						    TagElem->Content,
						    sizeof (Qst.Tags.Txt[Qst.Tags.Num]) - 1);
					  Qst.Tags.Num++;
					 }
				      }
				 break;	// Only first element "tags"
				}

			   /* Get stem (mandatory) */
			   for (StemElem  = QuestionElem->FirstChild;
				StemElem != NULL;
				StemElem  = StemElem->NextBrother)
			      if (!strcmp (StemElem->TagName,"stem"))
				{
				 if (StemElem->Content)
				   {
				    /* Convert stem from text to HTML (in database stem is stored in HTML) */
				    Str_Copy (Qst.Stem,StemElem->Content,
					      Cns_MAX_BYTES_TEXT);
				    Str_ChangeFormat (Str_FROM_TEXT,Str_TO_HTML,
						      Qst.Stem,Cns_MAX_BYTES_TEXT,
						      Str_REMOVE_SPACES);
				   }
				 break;	// Only first element "stem"
				}

			   /* Get feedback (optional) */
			   for (FeedbackElem  = QuestionElem->FirstChild;
				FeedbackElem != NULL;
				FeedbackElem  = FeedbackElem->NextBrother)
			      if (!strcmp (FeedbackElem->TagName,"feedback"))
				{
				 if (FeedbackElem->Content)
				   {
				    /* Convert feedback from text to HTML (in database feedback is stored in HTML) */
				    Str_Copy (Qst.Feedback,FeedbackElem->Content,
					      Cns_MAX_BYTES_TEXT);
				    Str_ChangeFormat (Str_FROM_TEXT,Str_TO_HTML,
						      Qst.Feedback,Cns_MAX_BYTES_TEXT,
						      Str_REMOVE_SPACES);
				   }
				 break;	// Only first element "feedback"
				}

			   /* Get shuffle. By default, shuffle is false. */
			   Qst.Answer.Shuffle = Qst_DONT_SHUFFLE;
			   for (AnswerElem  = QuestionElem->FirstChild;
				AnswerElem != NULL;
				AnswerElem  = AnswerElem->NextBrother)
			      if (!strcmp (AnswerElem->TagName,"answer"))
				{
				 if (Qst.Answer.Type == Qst_ANS_UNIQUE_CHOICE ||
				     Qst.Answer.Type == Qst_ANS_MULTIPLE_CHOICE)
				    /* Get whether shuffle answers (in attribute "shuffle") */
				    for (Attribute  = AnswerElem->FirstAttribute;
					 Attribute != NULL;
					 Attribute  = Attribute->Next)
				       if (!strcmp (Attribute->AttributeName,"shuffle"))
					 {
					  Qst.Answer.Shuffle = XML_GetAttributteYesNoFromXMLTree (Attribute) ? Qst_SHUFFLE :
														    Qst_DONT_SHUFFLE;
					  break;	// Only first attribute "shuffle"
					 }
				 break;	// Only first element "answer"
				}

			   /* Get answer (mandatory) */
			   QstImp_GetAnswerFromXML (AnswerElem,&Qst);

			   /* Make sure that tags, text and answer are not empty */
			   if (Qst_CheckIfQstFormatIsCorrectAndCountNumOptions (&Qst) == Err_SUCCESS)
			     {
			      /* Check if question already exists in database */
			      QstExists = Qst_CheckIfQuestionExistsInDB (&Qst);

			      /* Write row with this imported question */
			      QstImp_WriteRowImportedQst (StemElem,FeedbackElem,
							  &Qst,QstExists);

			      /***** If a new question ==> insert question, tags and answer in the database *****/
			      if (QstExists == Exi_DOES_NOT_EXIST)
				{
				 Qst.QstCod = -1L;
				 Qst_InsertOrUpdateQstTagsAnsIntoDB (&Qst);
				 if (Qst.QstCod <= 0)
				    Err_ShowErrorAndExit ("Can not create question.");
				}
			     }
			   break;
		        case Err_ERROR:	// Answer type not found
		        default:
			   Err_WrongAnswerExit ();
		           break;
		       }

		  /***** Destroy question *****/
		  Qst_QstDestructor (&Qst);
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

static Qst_AnswerType_t QstImp_ConvertFromStrAnsTypXMLToAnsTyp (const char *StrAnsTypeXML)
  {
   extern const char *Qst_StrAnswerTypesXML[Qst_NUM_ANS_TYPES];
   Qst_AnswerType_t AnsType;

   if (StrAnsTypeXML != NULL)
      for (AnsType  = (Qst_AnswerType_t) 0;
	   AnsType <= (Qst_AnswerType_t) (Qst_NUM_ANS_TYPES - 1);
	   AnsType++)
	 // comparison must be case insensitive, because users can edit XML
         if (!strcasecmp (StrAnsTypeXML,Qst_StrAnswerTypesXML[AnsType]))
            return AnsType;

   Err_WrongAnswerExit ();
   return (Qst_AnswerType_t) 0;	// Not reached
  }

/*****************************************************************************/
/**************** Get answer inside an XML question elements *****************/
/*****************************************************************************/
// Answer is mandatory

static void QstImp_GetAnswerFromXML (struct XMLElement *AnswerElem,
                                     struct Qst_Question *Qst)
  {
   struct XMLElement *OptionElem;
   struct XMLElement *TextElem;
   struct XMLElement *FeedbackElem;
   struct XMLElement *LowerUpperElem;
   struct XMLAttribute *Attribute;
   unsigned NumOpt;

   switch (Qst->Answer.Type)
     {
      case Qst_ANS_INT:
         if (Qst_AllocateTextChoiceAnswer (Qst,0) == Err_ERROR)
	    /* Abort on error */
	    Ale_ShowAlertsAndExit ();

         if (AnswerElem->Content)
            Str_Copy (Qst->Answer.Options[0].Text,AnswerElem->Content,
                      Qst_MAX_BYTES_ANSWER_OR_FEEDBACK);
         break;
      case Qst_ANS_FLOAT:
         if (Qst_AllocateTextChoiceAnswer (Qst,0) == Err_ERROR)
	    /* Abort on error */
	    Ale_ShowAlertsAndExit ();
         if (Qst_AllocateTextChoiceAnswer (Qst,1) == Err_ERROR)
	    /* Abort on error */
	    Ale_ShowAlertsAndExit ();

         for (LowerUpperElem  = AnswerElem->FirstChild;
              LowerUpperElem != NULL;
              LowerUpperElem  = LowerUpperElem->NextBrother)
            if (!strcmp (LowerUpperElem->TagName,"lower"))
              {
               if (LowerUpperElem->Content)
                  Str_Copy (Qst->Answer.Options[0].Text,
                            LowerUpperElem->Content,
                            Qst_MAX_BYTES_ANSWER_OR_FEEDBACK);
               break;	// Only first element "lower"
              }
         for (LowerUpperElem  = AnswerElem->FirstChild;
              LowerUpperElem != NULL;
              LowerUpperElem  = LowerUpperElem->NextBrother)
            if (!strcmp (LowerUpperElem->TagName,"upper"))
              {
               if (LowerUpperElem->Content)
                  Str_Copy (Qst->Answer.Options[1].Text,
                            LowerUpperElem->Content,
                            Qst_MAX_BYTES_ANSWER_OR_FEEDBACK);
               break;	// Only first element "upper"
              }
         break;
      case Qst_ANS_TRUE_FALSE:
	 // Comparisons must be case insensitive, because users can edit XML
         Qst->Answer.OptionTF = QstTF__OPTION_EMPTY;
         if (AnswerElem->Content)
           {
            if (!strcasecmp (AnswerElem->Content,"true")  ||
                !strcasecmp (AnswerElem->Content,"T")     ||
                !strcasecmp (AnswerElem->Content,"yes")   ||
                !strcasecmp (AnswerElem->Content,"Y"))
               Qst->Answer.OptionTF = QstTF__OPTION_TRUE;
	    else if (!strcasecmp (AnswerElem->Content,"false") ||
		     !strcasecmp (AnswerElem->Content,"F")     ||
		     !strcasecmp (AnswerElem->Content,"no")    ||
		     !strcasecmp (AnswerElem->Content,"N"))
	       Qst->Answer.OptionTF = QstTF__OPTION_FALSE;
           }
         break;
      case Qst_ANS_UNIQUE_CHOICE:
      case Qst_ANS_MULTIPLE_CHOICE:
      case Qst_ANS_TEXT:
         /* Get options */
         for (OptionElem = AnswerElem->FirstChild, NumOpt = 0;
              OptionElem != NULL && NumOpt < Qst_MAX_OPTS_PER_QST;
              OptionElem = OptionElem->NextBrother, NumOpt++)
            if (!strcmp (OptionElem->TagName,"option"))
              {
               if (Qst_AllocateTextChoiceAnswer (Qst,NumOpt) == Err_ERROR)
		  /* Abort on error */
		  Ale_ShowAlertsAndExit ();

	       for (TextElem = OptionElem->FirstChild;
		    TextElem != NULL;
		    TextElem = TextElem->NextBrother)
		  if (!strcmp (TextElem->TagName,"text"))
		    {
		     if (TextElem->Content)
		       {
			Str_Copy (Qst->Answer.Options[NumOpt].Text,
			          TextElem->Content,
			          Qst_MAX_BYTES_ANSWER_OR_FEEDBACK);

			/* Convert answer from text to HTML (in database answer text is stored in HTML) */
			Str_ChangeFormat (Str_FROM_TEXT,Str_TO_HTML,
			                  Qst->Answer.Options[NumOpt].Text,
			                  Qst_MAX_BYTES_ANSWER_OR_FEEDBACK,
			                  Str_REMOVE_SPACES);
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
			Str_Copy (Qst->Answer.Options[NumOpt].Feedback,
			          FeedbackElem->Content,
			          Qst_MAX_BYTES_ANSWER_OR_FEEDBACK);

			/* Convert feedback from text to HTML (in database answer feedback is stored in HTML) */
			Str_ChangeFormat (Str_FROM_TEXT,Str_TO_HTML,
			                  Qst->Answer.Options[NumOpt].Feedback,
			                  Qst_MAX_BYTES_ANSWER_OR_FEEDBACK,
			                  Str_REMOVE_SPACES);
		       }
		     break;	// Only first element "feedback"
		    }

	       if (Qst->Answer.Type == Qst_ANS_TEXT)
		  Qst->Answer.Options[NumOpt].Correct = Qst_CORRECT;
	       else
		  /* Check if option is correct or wrong */
		  for (Attribute  = OptionElem->FirstAttribute;
		       Attribute != NULL;
		       Attribute  = Attribute->Next)
		     if (!strcmp (Attribute->AttributeName,"correct"))
		       {
			Qst->Answer.Options[NumOpt].Correct = XML_GetAttributteYesNoFromXMLTree (Attribute) ? Qst_CORRECT :
														   Qst_WRONG;
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

static void QstImp_WriteHeadingListImportedQst (void)
  {
   extern const char *Txt_No_INDEX;
   extern const char *Txt_Tags;
   extern const char *Txt_Type;
   extern const char *Txt_Shuffle;
   extern const char *Txt_Question;

   /***** Write the heading *****/
   HTM_TR_Begin (NULL);
      HTM_TH_Empty (1);
      HTM_TH (Txt_No_INDEX,HTM_HEAD_CENTER);
      HTM_TH (Txt_Tags    ,HTM_HEAD_CENTER);
      HTM_TH (Txt_Type    ,HTM_HEAD_CENTER);
      HTM_TH (Txt_Shuffle ,HTM_HEAD_CENTER);
      HTM_TH (Txt_Question,HTM_HEAD_LEFT  );
   HTM_TR_End ();
  }

/*****************************************************************************/
/**************** Write a row with one imported test question ****************/
/*****************************************************************************/

static void QstImp_WriteRowImportedQst (struct XMLElement *StemElem,
                                        struct XMLElement *FeedbackElem,
                                        const struct Qst_Question *Qst,
                                        Exi_Exist_t QuestionExists)
  {
   extern const char *Txt_New_question;
   extern const char *Txt_Existing_question;
   extern const char *Txt_no_tags;
   extern const char *Txt_TST_STR_ANSWER_TYPES[Qst_NUM_ANS_TYPES];
   extern const char *Txt_TST_Answer_given_by_the_teachers;
   static unsigned NumQst = 0;
   static unsigned NumNonExistingQst = 0;
   static struct
     {
      const char *Icon;
      Ico_Color_t Color;
      const char **Title;
      const char *ClassData;
      const char *ClassStem;
      const char *ClassShuffle;
      const char *ClassCorrect;
     } Layout[Exi_NUM_EXIST] =
     {
      [Exi_DOES_NOT_EXIST] =
        {
         .Icon		= "check-circle.svg",
         .Color		= Ico_UNCHANGED,
         .Title		= &Txt_New_question,
         .ClassData	= "DAT_SMALL",
         .ClassStem	= "Qst_TXT",
         .ClassShuffle	= "ICO16x16",
         .ClassCorrect	= "CONTEXT_ICO16x16",
        },
      [Exi_EXISTS] =
        {
         .Icon		= "tr16x16.gif",
         .Color		= Ico_GREEN,
         .Title		= &Txt_Existing_question,
         .ClassData	= "DAT_SMALL_LIGHT",
         .ClassStem	= "Qst_TXT_LIGHT",
         .ClassShuffle	= "ICO_HIDDEN ICO16x16",
         .ClassCorrect	= "ICO_HIDDEN CONTEXT_ICO16x16",
        },
     };
   const char *Stem = StemElem != NULL ? StemElem->Content :
	                                 "";
   const char *Feedback = FeedbackElem != NULL ? FeedbackElem->Content :
	                                         "";
   unsigned NumTag;
   unsigned NumOpt;
   char *AnswerText;
   size_t AnswerTextLength;
   char *AnswerFeedback;
   size_t AnswerFeedbackLength;
   const char *ClassData = Layout[QuestionExists].ClassData;
   const char *ClassStem = Layout[QuestionExists].ClassStem;

   NumQst++;

   HTM_TR_Begin (NULL);

      /***** Put icon to indicate that a question does not exist in database *****/
      HTM_TD_Begin ("class=\"BT %s\"",The_GetColorRows ());
	 Ico_PutIcon ( Layout[QuestionExists].Icon,
		       Layout[QuestionExists].Color,
		      *Layout[QuestionExists].Title,"CONTEXT_ICO16x16");
      HTM_TD_End ();

      /***** Write number of question *****/
      HTM_TD_Begin ("class=\"CT %s_%s %s\"",
                    ClassData,The_GetSuffix (),The_GetColorRows ());
	 if (QuestionExists == Exi_DOES_NOT_EXIST)
	   {
	    HTM_Unsigned (++NumNonExistingQst); HTM_NBSP ();
	   }
      HTM_TD_End ();

      /***** Write the question tags *****/
      HTM_TD_Begin ("class=\"LT %s\"",The_GetColorRows ());

	 if (Qst->Tags.Num)
	   {
	    /***** Write the tags *****/
	    HTM_TABLE_Begin (NULL);
	       for (NumTag = 0;
		    NumTag < Qst->Tags.Num;
		    NumTag++)
		 {
		  HTM_TR_Begin (NULL);

		     HTM_TD_Begin ("class=\"%s LT\"",ClassData);
			HTM_NBSP (); HTM_Txt ("&#8226;"); HTM_NBSP ();
		     HTM_TD_End ();

		     HTM_TD_Begin ("class=\"%s LT\"",ClassData);
			HTM_Txt (Qst->Tags.Txt[NumTag]);
		     HTM_TD_End ();

		  HTM_TR_End ();
		 }
	    HTM_TABLE_End ();
	   }
	 else	// no tags for this question
	   {
	    HTM_SPAN_Begin ("class=\"%s\"",ClassData);
	       HTM_NBSP (); HTM_ParTxtPar (Txt_no_tags); HTM_NBSP ();
	    HTM_SPAN_End ();
	   }

      HTM_TD_End ();

      /***** Write the question type *****/
      HTM_TD_Begin ("class=\"%s CT %s\"",ClassData,The_GetColorRows ());
	 HTM_Txt (Txt_TST_STR_ANSWER_TYPES[Qst->Answer.Type]);
	 HTM_NBSP ();
      HTM_TD_End ();

      /***** Write if shuffle is enabled *****/
      HTM_TD_Begin ("class=\"CT %s\"",The_GetColorRows ());
	 if (Qst->Answer.Type == Qst_ANS_UNIQUE_CHOICE ||
	     Qst->Answer.Type == Qst_ANS_MULTIPLE_CHOICE)
	    /* Put an icon that indicates whether shuffle is enabled or not */
	    if (Qst->Answer.Shuffle == Qst_SHUFFLE)
	       Ico_PutIcon ("check.svg",Ico_BLACK,
	                    Txt_TST_Answer_given_by_the_teachers,
			    Layout[QuestionExists].ClassShuffle);
      HTM_TD_End ();

      /***** Write the stem and the answers *****/
      HTM_TD_Begin ("class=\"LT %s\"",The_GetColorRows ());
	 Qst_WriteQstStem (Stem,ClassStem,HidVis_VISIBLE);
	 Qst_WriteQstFeedback (Feedback,"Qst_TXT_LIGHT");
	 switch (Qst->Answer.Type)
	   {
	    case Qst_ANS_INT:
	       HTM_SPAN_Begin ("class=\"%s\"",ClassStem);
	          HTM_OpenParenthesis ();
		     HTM_UnsignedLong (Qst->Answer.Integer);
		  HTM_CloseParenthesis ();
	       HTM_SPAN_End ();
	       break;
	    case Qst_ANS_FLOAT:
	       HTM_SPAN_Begin ("class=\"%s\"",ClassStem);
	          HTM_OpenParenthesis ();
	             HTM_DoubleRange (Qst->Answer.FloatingPoint[0],
	        	              Qst->Answer.FloatingPoint[1]);
		  HTM_CloseParenthesis ();
	       HTM_SPAN_End ();
	       break;
	    case Qst_ANS_TRUE_FALSE:
	       HTM_SPAN_Begin ("class=\"%s\"",ClassStem);
		  HTM_OpenParenthesis ();
		     QstTF__WriteAnsTF (Qst->Answer.OptionTF);
		  HTM_CloseParenthesis ();
	       HTM_SPAN_End ();
	       break;
	    case Qst_ANS_UNIQUE_CHOICE:
	    case Qst_ANS_MULTIPLE_CHOICE:
	    case Qst_ANS_TEXT:
	       HTM_TABLE_Begin (NULL);
		  for (NumOpt = 0;
		       NumOpt < Qst->Answer.NumOpts;
		       NumOpt++)
		    {
		     /* Convert the answer, that is in HTML, to rigorous HTML */
		     AnswerTextLength = strlen (Qst->Answer.Options[NumOpt].Text) *
					Cns_MAX_BYTES_PER_CHAR;
		     if ((AnswerText = malloc (AnswerTextLength + 1)) == NULL)
			Err_NotEnoughMemoryExit ();
		     Str_Copy (AnswerText,Qst->Answer.Options[NumOpt].Text,
			       AnswerTextLength);
		     Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
				       AnswerText,AnswerTextLength,
				       Str_DONT_REMOVE_SPACES);

		     /* Convert the feedback, that is in HTML, to rigorous HTML */
		     AnswerFeedbackLength = 0;
		     AnswerFeedback = NULL;
		     if (Qst->Answer.Options[NumOpt].Feedback)
			if (Qst->Answer.Options[NumOpt].Feedback[0])
			  {
			   AnswerFeedbackLength = strlen (Qst->Answer.Options[NumOpt].Feedback) *
						  Cns_MAX_BYTES_PER_CHAR;
			   if ((AnswerFeedback = malloc (AnswerFeedbackLength + 1)) == NULL)
			      Err_NotEnoughMemoryExit ();
			   Str_Copy (AnswerFeedback,
				     Qst->Answer.Options[NumOpt].Feedback,
				     AnswerFeedbackLength);
			   Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
					     AnswerFeedback,AnswerFeedbackLength,
					     Str_DONT_REMOVE_SPACES);
			  }

		     HTM_TR_Begin (NULL);

			/* Put an icon that indicates whether the answer is correct or wrong */
			HTM_TD_Begin ("class=\"BT %s\"",The_GetColorRows ());
			   if (Qst->Answer.Options[NumOpt].Correct == Qst_CORRECT)
			      Ico_PutIcon ("check.svg",Ico_BLACK,
			                   Txt_TST_Answer_given_by_the_teachers,
					   Layout[QuestionExists].ClassCorrect);
			HTM_TD_End ();

			/* Write the number of option */
			HTM_TD_Begin ("class=\"%s LT\"",ClassData);
			   HTM_Option (NumOpt); HTM_CloseParenthesis (); HTM_NBSP ();
			HTM_TD_End ();

			/* Write the text and the feedback of the answer */
			HTM_TD_Begin ("class=\"LT\"");

			   HTM_DIV_Begin ("class=\"%s_%s\"",
			                  ClassStem,The_GetSuffix ());
			      HTM_Txt (AnswerText);
			   HTM_DIV_End ();

			   if (AnswerFeedbackLength)
			     {
			      HTM_DIV_Begin ("class=\"Qst_TXT_LIGHT_%s\"",
			                     The_GetSuffix ());
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

   The_ChangeRowColor ();
  }
