// swad_error.h: exit on error

#ifndef _SWAD_ERR
#define _SWAD_ERR
/*
    SWAD (Shared Workspace At a Distance in Spanish),
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
/****************************** Public prototypes ****************************/
/*****************************************************************************/

void Err_NotEnoughMemoryExit (void);
void Err_PathTooLongExit (void);
void Err_QuerySizeExceededExit (void);

void Err_WrongActionExit (void);
void Err_WrongHierarchyLevelExit (void);
void Err_WrongCountrExit (void);
void Err_WrongInstitExit (void);
void Err_WrongCenterExit (void);
void Err_WrongDegTypExit (void);
void Err_WrongDegreeExit (void);
void Err_WrongCourseExit (void);
void Err_WrongDepartmentExit (void);
void Err_WrongPlaceExit (void);
void Err_WrongStatusExit (void);
void Err_WrongDateExit (void);
void Err_WrongPluginExit (void);
void Err_WrongItemsListExit (void);
void Err_WrongItemExit (void);
void Err_WrongSyllabusExit (void);
void Err_WrongFileBrowserExit (void);

void Err_FileFolderNotFoundExit (void);

void Err_WrongCopySrcExit (void);
void Err_WrongNumberOfRowsExit (void);
void Err_WrongGrpTypExit (void);
void Err_WrongGroupExit (void);
void Err_WrongTypeExit (void);
void Err_WrongBannerExit (void);
void Err_WrongLinkExit (void);
void Err_WrongHolidayExit (void);
void Err_WrongBuildingExit (void);
void Err_WrongRoomExit (void);
void Err_WrongRecordFieldExit (void);
void Err_WrongAssignmentExit (void);
void Err_WrongProjectExit (void);
void Err_WrongCallForExamExit (void);
void Err_WrongTagExit (void);
void Err_WrongTestExit (void);
void Err_WrongExamExit (void);
void Err_WrongSetExit (void);
void Err_WrongQuestionExit (void);
void Err_WrongQuestionIndexExit (void);
void Err_WrongAnswerExit (void);
void Err_WrongAnswerIndexExit (void);
void Err_WrongAnswerTypeExit (void);
void Err_WrongExamSessionExit (void);
void Err_WrongGameExit (void);
void Err_WrongMatchExit (void);
void Err_RecursiveRubric (void);
void Err_WrongRubricExit (void);
void Err_WrongCriterionExit (void);
void Err_WrongCriterionIndexExit (void);
void Err_WrongSurveyExit (void);
void Err_WrongWhoExit (void);
void Err_WrongCodeExit (void);
void Err_WrongEventExit (void);
void Err_WrongAnnouncementExit (void);
void Err_WrongNoticeExit (void);
void Err_WrongForumExit (void);
void Err_WrongThreadExit (void);
void Err_WrongPostExit (void);
void Err_WrongMessageExit (void);
void Err_WrongMailDomainExit (void);
void Err_WrongRoleExit (void);
void Err_WrongUserExit (void);

void Err_NoPermission (void);
void Err_NoPermissionExit (void);
void Err_ShowErrorAndExit (const char *Txt);

#endif
