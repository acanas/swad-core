// swad_program.c: course program

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2024 Antonio Cañas Vargas

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

#include "swad_box.h"
#include "swad_figure.h"
#include "swad_global.h"
#include "swad_program.h"
#include "swad_tree.h"
#include "swad_tree_database.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/******************************* Show program ********************************/
/*****************************************************************************/

void Prg_ShowProgram (void)
  {
   (void) Tre_ShowTree ();
  }

/*****************************************************************************/
/********************** Show stats about schedule items **********************/
/*****************************************************************************/

void Prg_GetAndShowCourseProgramStats (void)
  {
   extern const char *Hlp_ANALYTICS_Figures_course_programs;
   extern const char *Txt_FIGURE_TYPES[Fig_NUM_FIGURES];
   extern const char *Txt_Number_of_BR_program_items;
   extern const char *Txt_Number_of_BR_courses_with_BR_program_items;
   extern const char *Txt_Average_number_BR_of_items_BR_per_course;
   unsigned NumNodes;
   unsigned NumCoursesWithNodes;

   /***** Get the number of tree nodes from this location *****/
   if ((NumNodes = Tre_DB_GetNumNodes (Tre_PROGRAM,Gbl.Scope.Current)))
      NumCoursesWithNodes = Tre_DB_GetNumCoursesWithNodes (Tre_PROGRAM,Gbl.Scope.Current);
   else
      NumCoursesWithNodes = 0;

   /***** Begin box and table *****/
   Box_BoxTableBegin (Txt_FIGURE_TYPES[Fig_COURSE_PROGRAMS],NULL,NULL,
                      Hlp_ANALYTICS_Figures_course_programs,Box_NOT_CLOSABLE,2);

      /***** Write table heading *****/
      HTM_TR_Begin (NULL);
	 HTM_TH (Txt_Number_of_BR_program_items                ,HTM_HEAD_RIGHT);
	 HTM_TH (Txt_Number_of_BR_courses_with_BR_program_items,HTM_HEAD_RIGHT);
	 HTM_TH (Txt_Average_number_BR_of_items_BR_per_course  ,HTM_HEAD_RIGHT);
      HTM_TR_End ();

      /***** Write number of nodes *****/
      HTM_TR_Begin (NULL);
	 HTM_TD_Unsigned (NumNodes);
	 HTM_TD_Unsigned (NumCoursesWithNodes);
	 HTM_TD_Ratio (NumNodes,NumCoursesWithNodes);
      HTM_TR_End ();

   /***** End table and box *****/
   Box_BoxTableEnd ();
  }
