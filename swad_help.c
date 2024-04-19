// swad_help.c: contextual help to enhance usability

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2024 Antonio Cañas Vargas

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General 3 License as
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
#include <stdlib.h>		// For free

#include "swad_action.h"
#include "swad_action_list.h"
#include "swad_alert.h"
#include "swad_box.h"
#include "swad_config.h"
#include "swad_error.h"
#include "swad_form.h"
#include "swad_global.h"
#include "swad_help.h"
#include "swad_hierarchy_type.h"
#include "swad_HTML.h"
#include "swad_user.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Hlp_ShowRowHelpWhatWouldYouLikeToDo (const char *Description,
                                                 Act_Action_t Action,
                                                 Btn_Button_t Button,
                                                 const char *TxtButton);

/*****************************************************************************/
/*************************** Show contextual help ****************************/
/*****************************************************************************/

void Hlp_ShowHelpWhatWouldYouLikeToDo (void)
  {
   extern const char *Txt_You_are_not_enroled_in_any_course[Usr_NUM_SEXS];
   extern const char *Txt_You_can_search_for_courses_select_them_and_request_your_enrolment_in_them;
   extern const char *Txt_If_you_can_not_find_your_institution_your_center_your_degree_or_your_courses_you_can_create_them;
   extern const char *Txt_What_would_you_like_to_do;
   extern const char *Txt_Register_students_in_COURSE_X;
   extern const char *Txt_Register_students;
   extern const char *Txt_Go_to_one_of_my_courses;
   extern const char *Txt_My_courses;
   extern const char *Txt_Sign_up;
   extern const char *Txt_Remove_me_from_THE_COURSE_X;
   extern const char *Txt_Remove_me;
   extern const char *Txt_Register_me_in_X;
   extern const char *Txt_HIERARCHY_PLURAL_Abc[Hie_NUM_LEVELS];
   extern const char *Txt_Select_or_create_one_course_in_X;
   extern const char *Txt_Select_or_create_one_degree_in_X;
   extern const char *Txt_Select_or_create_one_center_in_X;
   extern const char *Txt_Select_or_create_one_institution_in_X;
   extern const char *Txt_Select_one_country_in_X;
   extern const char *Txt_Upload_my_picture;
   extern const char *Txt_Upload_photo;
   extern const char *Txt_Log_in;
   extern const char *Txt_New_on_PLATFORM_Sign_up;
   extern const char *Txt_Actions[ActLst_NUM_ACTIONS];
   static Act_Action_t ActionsRemoveMe[Rol_NUM_ROLES] =
     {
      [Rol_UNK    ] = ActUnk,
      [Rol_GST    ] = ActUnk,
      [Rol_USR    ] = ActUnk,
      [Rol_STD    ] = ActRemMe_Std,
      [Rol_NET    ] = ActRemMe_NET,
      [Rol_TCH    ] = ActRemMe_Tch,
      [Rol_DEG_ADM] = ActUnk,
      [Rol_CTR_ADM] = ActUnk,
      [Rol_INS_ADM] = ActUnk,
      [Rol_SYS_ADM] = ActUnk,
     };
   static struct
     {
      Act_Action_t Action;
      const char **Description;
     } SelectOrCreate[Hie_NUM_LEVELS]=
     {
      [Hie_SYS] = {ActSeeCty,&Txt_Select_one_country_in_X		},
      [Hie_CTY] = {ActSeeIns,&Txt_Select_or_create_one_institution_in_X	},
      [Hie_INS] = {ActSeeCtr,&Txt_Select_or_create_one_center_in_X	},
      [Hie_CTR] = {ActSeeDeg,&Txt_Select_or_create_one_degree_in_X	},
      [Hie_DEG] = {ActSeeCrs,&Txt_Select_or_create_one_course_in_X	},
     };
   char *Description;

   /***** Alert message *****/
   if (Gbl.Usrs.Me.Logged &&
       !Gbl.Usrs.Me.Hierarchy[Hie_CRS].Num)
      Ale_ShowAlert (Ale_INFO,"%s<br />%s<br />%s",
	             Txt_You_are_not_enroled_in_any_course[Gbl.Usrs.Me.UsrDat.Sex],
	             Txt_You_can_search_for_courses_select_them_and_request_your_enrolment_in_them,
	             Txt_If_you_can_not_find_your_institution_your_center_your_degree_or_your_courses_you_can_create_them);

   /***** Begin box and table *****/
   HTM_DIV_Begin (NULL);
      Box_BoxTableBegin (Txt_What_would_you_like_to_do,NULL,NULL,
			 NULL,Box_CLOSABLE,2);

	 if (Gbl.Usrs.Me.Logged)		// I am logged
	   {
	    if (Gbl.Hierarchy.Level == Hie_CRS)	// Course selected
	      {
	       switch (Gbl.Usrs.Me.IBelongToCurrent[Hie_CRS])
	         {
	          case Usr_BELONG:		// I belong to this course
		     if (Gbl.Action.Act != ActLogIn &&
			 Gbl.Action.Act != ActLogInNew &&
			 Gbl.Action.Act != ActLogInLan)	// I am not just logged
			if (ActionsRemoveMe[Gbl.Usrs.Me.UsrDat.Roles.InCurrentCrs] != ActUnk)
			  {
			   /***** Request my removing from this course *****/
			   if (asprintf (&Description,Txt_Remove_me_from_THE_COURSE_X,
					 Gbl.Hierarchy.Node[Hie_CRS].ShrtName) < 0)
			      Err_NotEnoughMemoryExit ();
			   Hlp_ShowRowHelpWhatWouldYouLikeToDo (Description,
								ActionsRemoveMe[Gbl.Usrs.Me.UsrDat.Roles.InCurrentCrs],
								Btn_REMOVE_BUTTON,Txt_Remove_me);
			   free (Description);
			  }
	             break;
	          case Usr_DONT_BELONG:	// I do not belong to this course
	          default:
		     /***** Request my registration in this course *****/
		     if (asprintf (&Description,Txt_Register_me_in_X,
				   Gbl.Hierarchy.Node[Hie_CRS].ShrtName) < 0)
			Err_NotEnoughMemoryExit ();
		     Hlp_ShowRowHelpWhatWouldYouLikeToDo (Description,
							  ActReqSignUp,
							  Btn_CREATE_BUTTON,Txt_Sign_up);
		     free (Description);
		     break;
	         }
	      }

	    if (Gbl.Usrs.Me.Hierarchy[Hie_CRS].Num)	// I am enroled in some courses
	      {
	       if (Gbl.Hierarchy.Level == Hie_CRS &&				// Course selected
		   Gbl.Usrs.Me.UsrDat.Roles.InCurrentCrs == Rol_TCH)	// I am a teacher in current course
		  if (!Enr_GetCachedNumUsrsInCrss (Hie_CRS,Gbl.Hierarchy.Node[Hie_CRS].HieCod,
						   1 << Rol_STD))		// Current course probably has no students
		    {
		     /***** Request students enrolment *****/
		     if (asprintf (&Description,Txt_Register_students_in_COURSE_X,
				   Gbl.Hierarchy.Node[Hie_CRS].ShrtName) < 0)
			Err_NotEnoughMemoryExit ();
		     Hlp_ShowRowHelpWhatWouldYouLikeToDo (Description,
							  ActReqEnrSevStd,
							  Btn_CREATE_BUTTON,Txt_Register_students);
		     free (Description);
		    }

	       if (Gbl.Action.Act != ActMyCrs)	// I am not seeing the action to list my courses
		  /***** Request list my courses *****/
		  Hlp_ShowRowHelpWhatWouldYouLikeToDo (Txt_Go_to_one_of_my_courses,
						       ActMyCrs,
						       Btn_CONFIRM_BUTTON,Txt_My_courses);
	      }

	    /***** Go to list of hierarchy subnodes
		   to select or create a new one *****/
	    if (Gbl.Hierarchy.Level >= Hie_SYS &&
		Gbl.Hierarchy.Level <= Hie_DEG)
	      {
	       if (asprintf (&Description,*(SelectOrCreate[Gbl.Hierarchy.Level].Description),
			     Gbl.Hierarchy.Node[Gbl.Hierarchy.Level].ShrtName) < 0)
		  Err_NotEnoughMemoryExit ();
	       Hlp_ShowRowHelpWhatWouldYouLikeToDo (Description,
						    SelectOrCreate[Gbl.Hierarchy.Level].Action,
						    Btn_CONFIRM_BUTTON,
						    Txt_HIERARCHY_PLURAL_Abc[Gbl.Hierarchy.Level + 1]);
	       free (Description);
	      }

	    if (!Gbl.Usrs.Me.MyPhotoExists)		// I have no photo
	       /***** Upload my photo *****/
	       Hlp_ShowRowHelpWhatWouldYouLikeToDo (Txt_Upload_my_picture,
						    ActReqMyPho,
						    Btn_CREATE_BUTTON,Txt_Upload_photo);
	   }
	 else					// I am not logged
	   {
	    /***** Log in *****/
	    Hlp_ShowRowHelpWhatWouldYouLikeToDo (Txt_Log_in,
						 ActFrmLogIn,
						 Btn_CONFIRM_BUTTON,Txt_Log_in);

	    /***** Sign up *****/
	    if (asprintf (&Description,Txt_New_on_PLATFORM_Sign_up,
			  Cfg_PLATFORM_SHORT_NAME) < 0)
	       Err_NotEnoughMemoryExit ();
	    Hlp_ShowRowHelpWhatWouldYouLikeToDo (Description,
						 ActFrmMyAcc,
						 Btn_CREATE_BUTTON,Txt_Actions[ActCreUsrAcc]);
	    free (Description);
	   }

      /***** End table and box *****/
      Box_BoxTableEnd ();
   HTM_DIV_End ();
  }

/*****************************************************************************/
/*************************** Show contextual help ****************************/
/*****************************************************************************/

static void Hlp_ShowRowHelpWhatWouldYouLikeToDo (const char *Description,
                                                 Act_Action_t Action,
                                                 Btn_Button_t Button,
                                                 const char *TxtButton)
  {
   HTM_TR_Begin (NULL);

      /***** Description *****/
      HTM_TD_Begin ("class=\"RM DAT_%s\"",The_GetSuffix ());
	 HTM_TxtColon (Description);
      HTM_TD_End ();

      /***** Button *****/
      HTM_TD_Begin ("class=\"LM\"");
	 Frm_BeginForm (Action);
	    Btn_PutButtonInline (Button,TxtButton);
	 Frm_EndForm ();
      HTM_TD_End ();

   HTM_TR_End ();
  }
