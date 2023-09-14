// swad_help.c: contextual help to enhance usability

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2023 Antonio Cañas Vargas

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
#include "swad_hierarchy_level.h"
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
   extern const char *Txt_Select_create_course_in_X;
   extern const char *Txt_Select_or_create_one_course_in_X;
   extern const char *Txt_Courses;
   extern const char *Txt_Select_or_create_another_degree_in_X;
   extern const char *Txt_Select_or_create_one_degree_in_X;
   extern const char *Txt_Degrees;
   extern const char *Txt_Select_or_create_another_center_in_X;
   extern const char *Txt_Select_or_create_one_center_in_X;
   extern const char *Txt_Centers;
   extern const char *Txt_Select_or_create_another_institution_in_X;
   extern const char *Txt_Select_or_create_one_institution_in_X;
   extern const char *Txt_Institutions;
   extern const char *Txt_Select_another_country;
   extern const char *Txt_Select_one_country;
   extern const char *Txt_Countries;
   extern const char *Txt_Upload_my_picture;
   extern const char *Txt_Upload_photo;
   extern const char *Txt_Log_in;
   extern const char *Txt_New_on_PLATFORM_Sign_up;
   extern const char *Txt_Actions[ActLst_NUM_ACTIONS];
   static const Act_Action_t ActionsRemoveMe[Rol_NUM_ROLES] =
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
   char *Description;

   /***** Alert message *****/
   if (Gbl.Usrs.Me.Logged &&
       !Gbl.Usrs.Me.MyCrss.Num)
      Ale_ShowAlert (Ale_INFO,"%s<br />%s<br />%s",
	             Txt_You_are_not_enroled_in_any_course[Gbl.Usrs.Me.UsrDat.Sex],
	             Txt_You_can_search_for_courses_select_them_and_request_your_enrolment_in_them,
	             Txt_If_you_can_not_find_your_institution_your_center_your_degree_or_your_courses_you_can_create_them);

   /***** Begin box and table *****/
   Box_BoxTableBegin (NULL,Txt_What_would_you_like_to_do,
                      NULL,NULL,
                      NULL,Box_CLOSABLE,2);

      if (Gbl.Usrs.Me.Logged)		// I am logged
	{
	 if (Gbl.Hierarchy.Level == HieLvl_CRS)	// Course selected
	   {
	    if (Gbl.Usrs.Me.IBelongToCurrentCrs)	// I belong to this course
	      {
	       if (Gbl.Action.Act != ActLogIn &&
		   Gbl.Action.Act != ActLogInNew &&
		   Gbl.Action.Act != ActLogInLan)	// I am not just logged
		  if (ActionsRemoveMe[Gbl.Usrs.Me.UsrDat.Roles.InCurrentCrs] != ActUnk)
		    {
		     /* Request my removing from this course */
		     if (asprintf (&Description,Txt_Remove_me_from_THE_COURSE_X,
		                   Gbl.Hierarchy.Crs.ShrtName) < 0)
			Err_NotEnoughMemoryExit ();
		     Hlp_ShowRowHelpWhatWouldYouLikeToDo (Description,
							  ActionsRemoveMe[Gbl.Usrs.Me.UsrDat.Roles.InCurrentCrs],
							  Btn_REMOVE_BUTTON,Txt_Remove_me);
		     free (Description);
		    }
	      }
	    else					// I do not belong to this course
	      {
	       /* Request my registration in this course */
	       if (asprintf (&Description,Txt_Register_me_in_X,
			     Gbl.Hierarchy.Crs.ShrtName) < 0)
		  Err_NotEnoughMemoryExit ();
	       Hlp_ShowRowHelpWhatWouldYouLikeToDo (Description,
						    ActReqSignUp,
						    Btn_CREATE_BUTTON,Txt_Sign_up);
	       free (Description);
	      }
	   }

	 if (Gbl.Usrs.Me.MyCrss.Num)	// I am enroled in some courses
	   {
	    if (Gbl.Hierarchy.Level == HieLvl_CRS &&				// Course selected
		Gbl.Usrs.Me.UsrDat.Roles.InCurrentCrs == Rol_TCH)	// I am a teacher in current course
	       if (!Enr_GetCachedNumUsrsInCrss (HieLvl_CRS,Gbl.Hierarchy.Crs.Cod,
						1 << Rol_STD))		// Current course probably has no students
		 {
		  /* Request students enrolment */
		  if (asprintf (&Description,Txt_Register_students_in_COURSE_X,
				Gbl.Hierarchy.Crs.ShrtName) < 0)
		     Err_NotEnoughMemoryExit ();
		  Hlp_ShowRowHelpWhatWouldYouLikeToDo (Description,
						       ActReqEnrSevStd,
						       Btn_CREATE_BUTTON,Txt_Register_students);
		  free (Description);
		 }

	    if (Gbl.Action.Act != ActMyCrs)	// I am not seeing the action to list my courses
	       /* Request list my courses */
	       Hlp_ShowRowHelpWhatWouldYouLikeToDo (Txt_Go_to_one_of_my_courses,
						    ActMyCrs,
						    Btn_CONFIRM_BUTTON,Txt_My_courses);
	   }

	 if (Gbl.Hierarchy.Deg.Cod > 0)	// Degree selected
	   {
	    /* Select a course */
	    if (asprintf (&Description,Gbl.Hierarchy.Level == HieLvl_CRS ? Txt_Select_create_course_in_X :
									   Txt_Select_or_create_one_course_in_X,
			  Gbl.Hierarchy.Deg.ShrtName) < 0)
	       Err_NotEnoughMemoryExit ();
	    Hlp_ShowRowHelpWhatWouldYouLikeToDo (Description,
						 ActSeeCrs,
						 Btn_CONFIRM_BUTTON,Txt_Courses);
	    free (Description);
	   }
	 else if (Gbl.Hierarchy.Ctr.Cod > 0)	// Center selected
	   {
	    /* Select a degree */
	    if (asprintf (&Description,Gbl.Hierarchy.Deg.Cod > 0 ? Txt_Select_or_create_another_degree_in_X :
								      Txt_Select_or_create_one_degree_in_X,
			  Gbl.Hierarchy.Ctr.ShrtName) < 0)
	       Err_NotEnoughMemoryExit ();
	    Hlp_ShowRowHelpWhatWouldYouLikeToDo (Description,
						 ActSeeDeg,
						 Btn_CONFIRM_BUTTON,Txt_Degrees);
	    free (Description);
	   }
	 else if (Gbl.Hierarchy.Ins.Cod > 0)	// Institution selected
	   {
	    /* Select a center */
	    if (asprintf (&Description,Gbl.Hierarchy.Ctr.Cod > 0 ? Txt_Select_or_create_another_center_in_X :
								      Txt_Select_or_create_one_center_in_X,
			  Gbl.Hierarchy.Ins.ShrtName) < 0)
	       Err_NotEnoughMemoryExit ();
	    Hlp_ShowRowHelpWhatWouldYouLikeToDo (Description,
						 ActSeeCtr,
						 Btn_CONFIRM_BUTTON,Txt_Centers);
	    free (Description);
	   }
	 else if (Gbl.Hierarchy.Cty.Cod > 0)	// Country selected
	   {
	    /* Select an institution */
	    if (asprintf (&Description,Gbl.Hierarchy.Ins.Cod > 0 ? Txt_Select_or_create_another_institution_in_X :
								      Txt_Select_or_create_one_institution_in_X,
			  Gbl.Hierarchy.Cty.Name[Gbl.Prefs.Language]) < 0)
	       Err_NotEnoughMemoryExit ();
	    Hlp_ShowRowHelpWhatWouldYouLikeToDo (Description,
						 ActSeeIns,
						 Btn_CONFIRM_BUTTON,Txt_Institutions);
	    free (Description);
	   }
	 else
	    /* Select a country */
	    Hlp_ShowRowHelpWhatWouldYouLikeToDo (Gbl.Hierarchy.Cty.Cod > 0 ? Txt_Select_another_country :
										Txt_Select_one_country,
						 ActSeeCty,
						 Btn_CONFIRM_BUTTON,Txt_Countries);

	 if (!Gbl.Usrs.Me.MyPhotoExists)		// I have no photo
	    Hlp_ShowRowHelpWhatWouldYouLikeToDo (Txt_Upload_my_picture,
						 ActReqMyPho,
						 Btn_CREATE_BUTTON,Txt_Upload_photo);
	}
      else					// I am not logged
	{
	 /* Log in */
	 Hlp_ShowRowHelpWhatWouldYouLikeToDo (Txt_Log_in,
					      ActFrmLogIn,
					      Btn_CONFIRM_BUTTON,Txt_Log_in);

	 /* Sign up */
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
