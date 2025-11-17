// swad_indicator.h: indicators of courses

#ifndef _SWAD_IND
#define _SWAD_IND
/*
    SWAD (Shared Workspace At a Distance in Spanish),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2025 Antonio Cañas Vargas

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

#include "swad_info.h"

/*****************************************************************************/
/************************** Public types and constants ***********************/
/*****************************************************************************/

#define Ind_NUM_INDICATORS 5
#define Ind_MAX_SIZE_INDICATORS_CHECKED ((1 + Ind_NUM_INDICATORS) * (10 + 1))

#define Ind_NUM_STATUS 3
typedef enum
  {
   Ind_COURSE_NOTHING_OK,
   Ind_COURSE_PARTIALLY_OK,
   Ind_COURSE_ALL_OK,
  } Ind_CrsStatus_t;

struct Ind_IndicatorsCrs
  {
   struct
     {
      Exi_Exist_t Syllabus;
      Exi_Exist_t Assignment;
      Exi_Exist_t OnlineTutoring;
      Exi_Exist_t Material;
      Exi_Exist_t Assessment;
     } Exist;

   Inf_Src_t TeachingGuideSrc;
   Inf_Src_t SyllabusLecSrc;
   Inf_Src_t SyllabusPraSrc;

   unsigned NumAssignments;
   unsigned NumFilesAssignments;
   unsigned NumFilesWorks;

   unsigned NumThreads;
   unsigned NumPosts;
   unsigned NumUsrsToBeNotifiedByEMail;
   unsigned NumMsgsSentByTchs;

   unsigned NumFilesInDocumentZones;
   unsigned NumFilesInSharedZones;

   Inf_Src_t AssessmentSrc;

   unsigned NumIndicators;
   Ind_CrsStatus_t CrsStatus;
  };

struct Ind_Indicators
  {
   Hie_Level_t HieLvl;
   long DegTypCod;
   long DptCod;
   char StrChecked[Ind_MAX_SIZE_INDICATORS_CHECKED + 1];
   HTM_Attributes_t Checked[1 + Ind_NUM_INDICATORS];
  };

/*****************************************************************************/
/***************************** Public prototypes *****************************/
/*****************************************************************************/

void Ind_ReqIndicatorsCourses (void);
void Ind_ShowIndicatorsCourses (void);
int Ind_GetNumIndicatorsCrsFromDB (long HieCod);
void Ind_ComputeAndStoreIndicatorsCrs (long HieCod,int NumIndicatorsFromDB,
                                       struct Ind_IndicatorsCrs *IndicatorsCrs);

#endif
