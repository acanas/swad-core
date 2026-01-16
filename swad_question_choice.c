// swad_question_choice.c: test/exam/game choice questions

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

#include "swad_constant.h"
#include "swad_cryptography.h"
#include "swad_exam_sheet.h"
#include "swad_question_choice.h"

/*****************************************************************************/
/***************************** Public constants ******************************/
/*****************************************************************************/

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

/*****************************************************************************/
/********** Write single or multiple choice answer in a test print ***********/
/*****************************************************************************/

void QstCho_WriteTstFillAns (const struct Qst_PrintedQuestion *PrintedQst,
                             unsigned QstInd,struct Qst_Question *Qst)
  {
   unsigned NumOpt;
   unsigned Indexes[Qst_MAX_OPTIONS_PER_QUESTION];	// Indexes of all answers of this question
   HTM_Attributes_t UsrAnswers[Qst_MAX_OPTIONS_PER_QUESTION];
   char StrAns[3 + Cns_MAX_DIGITS_UINT + 1];	// "Ansxx...x"
   char Id[3 + Cns_MAX_DIGITS_UINT + 1 + Cns_MAX_DIGITS_UINT + 1];	// "Ansxx...x_yy...y"

   /***** Change format of answers text *****/
   Qst_ChangeFormatOptionsText (Qst);

   /***** Get indexes for this question from string *****/
   Qst_GetIndexesFromStr (PrintedQst->StrIndexes,Indexes);

   /***** Get the user's answers for this question from string *****/
   Qst_GetAnswersFromStr (PrintedQst->Answer.Str,UsrAnswers);

   /***** Begin table *****/
   HTM_TABLE_BeginPadding (2);

      for (NumOpt = 0;
	   NumOpt < Qst->Answer.NumOptions;
	   NumOpt++)
	{
	 /***** Indexes are 0 1 2 3... if no shuffle
		or 3 1 0 2... (example) if shuffle *****/
	 HTM_TR_Begin (NULL);

	    /***** Write selectors and letter of this option *****/
	    /* Initially user has not answered the question ==> initially all answers will be blank.
	       If the user does not confirm the submission of their exam ==>
	       ==> the exam may be half filled ==> the answers displayed will be those selected by the user. */
	    HTM_TD_Begin ("class=\"LT\"");

	       snprintf (StrAns,sizeof (StrAns),"Ans%010u",QstInd);
	       snprintf (Id,sizeof (Id),"%s_%u",StrAns,NumOpt);
	       switch (Qst->Answer.Type)
	         {
	          case Qst_ANS_UNIQUE_CHOICE:
		     HTM_INPUT_RADIO (StrAns,
				      UsrAnswers[Indexes[NumOpt]],
				      "id=\"%s\" value=\"%u\""
				      " onclick=\"selectUnselectRadio(this,false,this.form.Ans%010u,%u);\"",
				      Id,Indexes[NumOpt],
				      QstInd,Qst->Answer.NumOptions);
	             break;
	          case Qst_ANS_MULTIPLE_CHOICE:
		     HTM_INPUT_CHECKBOX (StrAns,
					 UsrAnswers[Indexes[NumOpt]],
					 "id=\"%s\" value=\"%u\"",
					 Id,Indexes[NumOpt]);
	             break;
	          default:
	             Err_WrongAnswerTypeExit ();
	             break;
	         }

	    HTM_TD_End ();

	    HTM_TD_Begin ("class=\"LT\"");
	       HTM_LABEL_Begin ("for=\"Ans%010u_%u\" class=\"Qst_TXT_%s\"",
	                        QstInd,NumOpt,The_GetSuffix ());
		  HTM_Option (NumOpt); HTM_CloseParenthesis (); HTM_NBSP ();
	       HTM_LABEL_End ();
	    HTM_TD_End ();

	    /***** Write the option text *****/
	    HTM_TD_Begin ("class=\"LT\"");
	       HTM_LABEL_Begin ("for=\"Ans%010u_%u\" class=\"Qst_TXT_%s\"",
	                        QstInd,NumOpt,The_GetSuffix ());
		  HTM_Txt (Qst->Answer.Options[Indexes[NumOpt]].Text);
	       HTM_LABEL_End ();
	       Med_ShowMedia (&Qst->Answer.Options[Indexes[NumOpt]].Media,
			      "Tst_MED_SHOW_CONT","Tst_MED_SHOW");
	    HTM_TD_End ();

	 HTM_TR_End ();
	}

   /***** End table *****/
   HTM_TABLE_End ();
  }

void QstCho_WriteTstPrntAns (const struct Qst_PrintedQuestion *PrintedQst,
			     struct Qst_Question *Qst,
			     Usr_Can_t ICanView[TstVis_NUM_ITEMS_VISIBILITY],
			     const char *ClassTxt,
			     const char *ClassFeedback)
  {
   extern struct Qst_AnswerDisplay Qst_AnswerDisplay[Qst_NUM_WRONG_CORRECT];
   extern const char *Txt_TST_Answer_given_by_the_user;
   extern const char *Txt_TST_Answer_given_by_the_teachers;
   struct Answer
     {
      char *Class;
      char *Str;
     };
   static struct Qst_AnswerDisplay AnsNotVisible =
     {
      .ClassTch = "Qst_ANS_0",
      .ClassStd = "Qst_ANS_0",
      .Symbol   = "&bull;"
     };
   unsigned NumOpt;
   Qst_WrongOrCorrect_t WrongOrCorrect;
   unsigned Indexes[Qst_MAX_OPTIONS_PER_QUESTION];	// Indexes of all answers of this question
   HTM_Attributes_t UsrAnswers[Qst_MAX_OPTIONS_PER_QUESTION];
   const struct Qst_AnswerDisplay *Ans;

   /***** Change format of answers text *****/
   Qst_ChangeFormatOptionsText (Qst);

   /***** Change format of answers feedback *****/
   if (ICanView[TstVis_VISIBLE_FEEDBACK_TXT] == Usr_CAN)
      Qst_ChangeFormatOptionsFeedback (Qst);

   /***** Get indexes for this question from string *****/
   Qst_GetIndexesFromStr (PrintedQst->StrIndexes,Indexes);

   /***** Get the user's answers for this question from string *****/
   Qst_GetAnswersFromStr (PrintedQst->Answer.Str,UsrAnswers);

   /***** Write answers (one row per answer) *****/
   for (NumOpt = 0;
	NumOpt < Qst->Answer.NumOptions;
	NumOpt++)
     {
      WrongOrCorrect = Qst->Answer.Options[Indexes[NumOpt]].Correct;

      HTM_TR_Begin (NULL);

	 /* Draw icon depending on user's answer */
	 if (UsrAnswers[Indexes[NumOpt]] == HTM_CHECKED)	// This answer has been selected by the user
	   {
	    switch (ICanView[TstVis_VISIBLE_CORRECT_ANSWER])
	      {
	       case Usr_CAN:
		  Ans = &Qst_AnswerDisplay[WrongOrCorrect];
		  break;
	       case Usr_CAN_NOT:
	       default:
		  Ans = &AnsNotVisible;
		  break;
	      }
	    HTM_TD_Begin ("class=\"CT %s_%s\" title=\"%s\"",
			  Ans->ClassStd,The_GetSuffix (),
			  Txt_TST_Answer_given_by_the_user);
	       HTM_Txt (Ans->Symbol);
	    HTM_TD_End ();
	   }
	 else	// This answer has NOT been selected by the user
	    HTM_TD_Empty (1);

	 /* Draw icon that indicates whether the answer is correct */
	 switch (ICanView[TstVis_VISIBLE_CORRECT_ANSWER])
	   {
	    case Usr_CAN:
	       switch (WrongOrCorrect)
		 {
		  case Qst_CORRECT:
		     HTM_TD_Begin ("class=\"CT %s_%s\" title=\"%s\"",
				   Qst_AnswerDisplay[Qst_BLANK].ClassTch,
				   The_GetSuffix (),
				   Txt_TST_Answer_given_by_the_teachers);
			HTM_Txt ("&bull;");
		     HTM_TD_End ();
		     break;
		  case Qst_WRONG:
		  default:
		     HTM_TD_Empty (1);
		     break;
		 }
	       break;
	    case Usr_CAN_NOT:
	    default:
	       HTM_TD_Begin ("class=\"CT %s_%s\"",
			     Qst_AnswerDisplay[Qst_BLANK].ClassTch,
			     The_GetSuffix ());
		  Ico_PutIconNotVisible ();
	       HTM_TD_End ();
	       break;
	   }

	 /* Answer letter (a, b, c,...) */
	 HTM_TD_Begin ("class=\"LT %s_%s\"",ClassTxt,The_GetSuffix ());
	    HTM_Option (NumOpt); HTM_CloseParenthesis (); HTM_NBSP ();
	 HTM_TD_End ();

	 /* Answer text and feedback */
	 HTM_TD_Begin ("class=\"LT\"");

	    HTM_DIV_Begin ("class=\"%s_%s\"",ClassTxt,The_GetSuffix ());
	       switch (ICanView[TstVis_VISIBLE_QST_ANS_TXT])
		 {
		  case Usr_CAN:
		     HTM_Txt (Qst->Answer.Options[Indexes[NumOpt]].Text);
		     Med_ShowMedia (&Qst->Answer.Options[Indexes[NumOpt]].Media,
				    "Tst_MED_SHOW_CONT","Tst_MED_SHOW");
		     break;
		  case Usr_CAN_NOT:
		  default:
		     Ico_PutIconNotVisible ();
		     break;
		 }
	    HTM_DIV_End ();

	    if (ICanView[TstVis_VISIBLE_CORRECT_ANSWER] == Usr_CAN)
	       if (Qst->Answer.Options[Indexes[NumOpt]].Feedback)
		  if (Qst->Answer.Options[Indexes[NumOpt]].Feedback[0])
		    {
		     HTM_DIV_Begin ("class=\"%s_%s\"",
				    ClassFeedback,The_GetSuffix ());
			HTM_Txt (Qst->Answer.Options[Indexes[NumOpt]].Feedback);
		     HTM_DIV_End ();
		    }

	 HTM_TD_End ();

      HTM_TR_End ();
     }
  }

/*****************************************************************************/
/****** Write unique / multiple choice answer in an exam question sheet ******/
/*****************************************************************************/

void QstCho_WriteExaBlnkQstOptions (const struct ExaPrn_Print *Print,
				    unsigned QstInd,struct Qst_Question *Qst)
  {
   unsigned NumOpt;
   unsigned Indexes[Qst_MAX_OPTIONS_PER_QUESTION];	// Indexes of all answers of this question

   /***** Change format of answers text *****/
   Qst_ChangeFormatOptionsText (Qst);

   /***** Get indexes for this question from string *****/
   Qst_GetIndexesFromStr (Print->PrintedQsts[QstInd].StrIndexes,Indexes);

   /***** Begin table *****/
   HTM_TABLE_BeginPadding (2);

      for (NumOpt = 0;
	   NumOpt < Qst->Answer.NumOptions;
	   NumOpt++)
	{
	 /***** Indexes are 0 1 2 3... if no shuffle
		or 3 1 0 2... (example) if shuffle *****/
	 HTM_TR_Begin (NULL);

	    /***** Write letter of this option *****/
	    HTM_TD_Begin ("class=\"LT\"");
	       HTM_LABEL_Begin ("class=\"Qst_TXT_%s\"",The_GetSuffix ());
		  HTM_Option (NumOpt); HTM_CloseParenthesis (); HTM_NBSP ();
	       HTM_LABEL_End ();
	    HTM_TD_End ();

	    /***** Write the option text *****/
	    HTM_TD_Begin ("class=\"LT\"");
	       HTM_LABEL_Begin ("class=\"Qst_TXT_%s\"",The_GetSuffix ());
		  HTM_Txt (Qst->Answer.Options[Indexes[NumOpt]].Text);
	       HTM_LABEL_End ();
	       Med_ShowMedia (&Qst->Answer.Options[Indexes[NumOpt]].Media,
			      "Tst_MED_SHOW_CONT","Tst_MED_SHOW");
	    HTM_TD_End ();

	 HTM_TR_End ();
	}

   /***** End table *****/
   HTM_TABLE_End ();
  }

/*****************************************************************************/
/******* Write unique / multiple choice answer in an exam answer sheet *******/
/*****************************************************************************/

void QstCho_WriteExaFillAns (const struct ExaPrn_Print *Print,
			     unsigned QstInd,struct Qst_Question *Qst)
  {
   static const char *InputType[Qst_NUM_ANS_TYPES] =
     {
      [Qst_ANS_UNIQUE_CHOICE  ] = "radio",
      [Qst_ANS_MULTIPLE_CHOICE] = "checkbox",
     };
   unsigned NumOpt;
   unsigned Indexes[Qst_MAX_OPTIONS_PER_QUESTION];	// Indexes of all answers of this question
   HTM_Attributes_t UsrAnswers[Qst_MAX_OPTIONS_PER_QUESTION];
   char Id[3 + Cns_MAX_DIGITS_UINT + 1];	// "Ansxx...x"

   /***** Change format of answers text *****/
   Qst_ChangeFormatOptionsText (Qst);

   /***** Get indexes for this question from string *****/
   Qst_GetIndexesFromStr (Print->PrintedQsts[QstInd].StrIndexes,Indexes);

   /***** Get the user's answers for this question from string *****/
   Qst_GetAnswersFromStr (Print->PrintedQsts[QstInd].Answer.Str,
			     UsrAnswers);

   /***** Begin table *****/
   HTM_TABLE_BeginPadding (2);

      for (NumOpt = 0;
	   NumOpt < Qst->Answer.NumOptions;
	   NumOpt++)
	{
	 /***** Indexes are 0 1 2 3... if no shuffle
		or 3 1 0 2... (example) if shuffle *****/
	 HTM_TR_Begin (NULL);

	    /***** Write selectors and letter of this option *****/
	    HTM_TD_Begin ("class=\"LT\"");
	       snprintf (Id,sizeof (Id),"Ans%010u",QstInd);
	       HTM_TxtF ("<input type=\"%s\" id=\"%s_%u\" name=\"Ans\" value=\"%u\"",
			 InputType[Qst->Answer.Type],
			 Id,NumOpt,Indexes[NumOpt]);
	       if ((UsrAnswers[Indexes[NumOpt]] & HTM_CHECKED))
		  HTM_Txt (" checked");
	       ExaPrn_WriteJSToUpdateExamPrint (Print,QstInd,Id,(int) NumOpt);
	       HTM_ElementEnd ();
	    HTM_TD_End ();

	    HTM_TD_Begin ("class=\"LT\"");
	       HTM_LABEL_Begin ("for=\"%s_%u\" class=\"Qst_TXT_%s\"",
	                        Id,NumOpt,The_GetSuffix ());
		  HTM_Option (NumOpt); HTM_CloseParenthesis (); HTM_NBSP ();
	       HTM_LABEL_End ();
	    HTM_TD_End ();

	    /***** Write the option text *****/
	    HTM_TD_Begin ("class=\"LT\"");
	       HTM_LABEL_Begin ("for=\"%s_%u\" class=\"Qst_TXT_%s\"",
	                        Id,NumOpt,The_GetSuffix ());
		  HTM_Txt (Qst->Answer.Options[Indexes[NumOpt]].Text);
	       HTM_LABEL_End ();
	       Med_ShowMedia (&Qst->Answer.Options[Indexes[NumOpt]].Media,
			      "Tst_MED_SHOW_CONT","Tst_MED_SHOW");
	    HTM_TD_End ();

	 HTM_TR_End ();
	}

   /***** End table *****/
   HTM_TABLE_End ();
  }

void QstCho_WriteExaBlnkAns (const struct Qst_Question *Qst)
  {
   extern struct Qst_AnswerDisplay Qst_AnswerDisplay[Qst_NUM_WRONG_CORRECT];
   unsigned NumOpt;

   /***** Write blank answers (one column per answer) *****/
   for (NumOpt = 0;
	NumOpt < Qst->Answer.NumOptions;
	NumOpt++)
     {
      /* Write option letter */
      HTM_TD_Begin ("class=\"Exa_ANSWER_CHOICE %s_%s\"",
		    Qst_AnswerDisplay[Qst_BLANK].ClassStd,The_GetSuffix ());
	 HTM_Option (NumOpt);
      HTM_TD_End ();
     }
  }

void QstCho_WriteExaCorrAns (const struct ExaPrn_Print *Print,
			     unsigned QstInd,struct Qst_Question *Qst)
  {
   extern struct Qst_AnswerDisplay Qst_AnswerDisplay[Qst_NUM_WRONG_CORRECT];
   unsigned Indexes[Qst_MAX_OPTIONS_PER_QUESTION];	// Indexes of all answers of this question
   unsigned NumOpt;

   /***** Get indexes for this question from string *****/
   Qst_GetIndexesFromStr (Print->PrintedQsts[QstInd].StrIndexes,Indexes);

   /***** Write correct answers (one column per answer) *****/
   for (NumOpt = 0;
	NumOpt < Qst->Answer.NumOptions;
	NumOpt++)
     {
      /* Write option letter */
      HTM_TD_Begin ("class=\"Exa_ANSWER_CHOICE %s_%s\"",
		    Qst_AnswerDisplay[Qst->Answer.Options[Indexes[NumOpt]].Correct].ClassTch,
		    The_GetSuffix ());
	 HTM_Option (NumOpt);
      HTM_TD_End ();
     }
  }

void QstCho_WriteExaReadAns (const struct ExaPrn_Print *Print,
			     unsigned QstInd,struct Qst_Question *Qst)
  {
   extern struct Qst_AnswerDisplay Qst_AnswerDisplay[Qst_NUM_WRONG_CORRECT];
   unsigned NumOpt;
   Qst_WrongOrCorrect_t WrongOrCorrect;
   unsigned Indexes[Qst_MAX_OPTIONS_PER_QUESTION];	// Indexes of all answers of this question
   HTM_Attributes_t UsrAnswers[Qst_MAX_OPTIONS_PER_QUESTION];

   /***** Get indexes for this question from string *****/
   Qst_GetIndexesFromStr (Print->PrintedQsts[QstInd].StrIndexes,Indexes);

   /***** Get the user's answers for this question from string *****/
   Qst_GetAnswersFromStr (Print->PrintedQsts[QstInd].Answer.Str,UsrAnswers);

   /***** Write online answers (one column per answer) *****/
   for (NumOpt = 0;
	NumOpt < Qst->Answer.NumOptions;
	NumOpt++)
     {
      WrongOrCorrect = Qst->Answer.Options[Indexes[NumOpt]].Correct;

      /* Write letter depending on user's answer */
      if (UsrAnswers[Indexes[NumOpt]] == HTM_CHECKED)	// This answer has been selected by the user
	{
	 HTM_TD_Begin ("class=\"Exa_ANSWER_CHOICE %s_%s\"",
		       Qst_AnswerDisplay[WrongOrCorrect].ClassStd,The_GetSuffix ());
	    HTM_Txt (Qst_AnswerDisplay[WrongOrCorrect].Symbol);
	}
      else	// This answer has NOT been selected by the user
	{
	 HTM_TD_Begin ("class=\"Exa_ANSWER_CHOICE %s_%s\"",
		       Qst_AnswerDisplay[Qst_BLANK].ClassStd,The_GetSuffix ());
	    HTM_NBSP ();
	}
      HTM_TD_End ();
     }
  }

void QstCho_WriteExaEditAns (const struct ExaPrn_Print *Print,
			     unsigned QstInd,struct Qst_Question *Qst)
  {
   static const char *InputType[Qst_NUM_ANS_TYPES] =
     {
      [Qst_ANS_UNIQUE_CHOICE  ] = "radio",
      [Qst_ANS_MULTIPLE_CHOICE] = "checkbox",
     };
   unsigned NumOpt;
   char Id[3 + 1 + Cry_BYTES_ENCRYPTED_STR_SHA256_BASE64 + 1 + Cns_MAX_DIGITS_UINT + 1];	// "Ans_encryptedusercode_xx...x"
   unsigned Indexes[Qst_MAX_OPTIONS_PER_QUESTION];	// Indexes of all answers of this question
   HTM_Attributes_t UsrAnswers[Qst_MAX_OPTIONS_PER_QUESTION];

   /***** Get indexes for this question from string *****/
   Qst_GetIndexesFromStr (Print->PrintedQsts[QstInd].StrIndexes,Indexes);

   /***** Get the user's answers for this question from string *****/
   Qst_GetAnswersFromStr (Print->PrintedQsts[QstInd].Answer.Str,UsrAnswers);

   /***** Write input field for the answer *****/
   for (NumOpt = 0;
	NumOpt < Qst->Answer.NumOptions;
	NumOpt++)
     {
      HTM_TD_Begin ("class=\"Exa_ANSWER_CHOICE\"");

         snprintf (Id,sizeof (Id),"Ans_%s_%010u",Print->EnUsrCod,QstInd);
	 HTM_TxtF ("<input type=\"%s\" id=\"%s_%u\" name=\"Ans\" value=\"%u\"",
		   InputType[Qst->Answer.Type],Id,NumOpt,Indexes[NumOpt]);
	 if ((UsrAnswers[Indexes[NumOpt]] & HTM_CHECKED))
	    HTM_Txt (" checked");
	 ExaShe_WriteJSToUpdateSheet (Print,QstInd,Id,(int) NumOpt);
	 HTM_ElementEnd ();

      HTM_TD_End ();
     }
  }
