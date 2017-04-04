// swad_help_URL.c: contextual help URLs, depending on the current language

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2017 Antonio Cañas Vargas

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
/****************************** Public constants *****************************/
/*****************************************************************************/

#ifndef L
#define L 3	// English
#endif

const char *Hlp_Search =
#if   L==1
	"Search";
#elif L==2
	"Search";
#elif L==3
	"Search";
#elif L==4
	"Search.es";
#elif L==5
	"Search";
#elif L==6
	"Search";
#elif L==7
	"Search";
#elif L==8
	"Search";
#elif L==9
	"Search";
#endif

const char *Hlp_Calendar =
#if   L==1
	"Calendar";
#elif L==2
	"Calendar";
#elif L==3
	"Calendar";
#elif L==4
	"Calendar";
#elif L==5
	"Calendar";
#elif L==6
	"Calendar";
#elif L==7
	"Calendar";
#elif L==8
	"Calendar";
#elif L==9
	"Calendar";
#endif

/***** SYSTEM tab *****/

const char *Hlp_SYSTEM_Countries =
#if   L==1
	"SYSTEM.Countries";
#elif L==2
	"SYSTEM.Countries";
#elif L==3
	"SYSTEM.Countries";
#elif L==4
	"SYSTEM.Countries.es";
#elif L==5
	"SYSTEM.Countries";
#elif L==6
	"SYSTEM.Countries";
#elif L==7
	"SYSTEM.Countries";
#elif L==8
	"SYSTEM.Countries";
#elif L==9
	"SYSTEM.Countries";
#endif

const char *Hlp_SYSTEM_Hierarchy_pending =
#if   L==1
	"SYSTEM.Hierarchy#pending";
#elif L==2
	"SYSTEM.Hierarchy#pending";
#elif L==3
	"SYSTEM.Hierarchy#pending";
#elif L==4
	"SYSTEM.Hierarchy.es#pendientes";
#elif L==5
	"SYSTEM.Hierarchy#pending";
#elif L==6
	"SYSTEM.Hierarchy#pending";
#elif L==7
	"SYSTEM.Hierarchy#pending";
#elif L==8
	"SYSTEM.Hierarchy#pending";
#elif L==9
	"SYSTEM.Hierarchy#pending";
#endif

const char *Hlp_SYSTEM_Hierarchy_eliminate_old_courses =
#if   L==1
	"SYSTEM.Hierarchy#eliminate-old-courses";
#elif L==2
	"SYSTEM.Hierarchy#eliminate-old-courses";
#elif L==3
	"SYSTEM.Hierarchy#eliminate-old-courses";
#elif L==4
	"SYSTEM.Hierarchy.es#eliminar-asignaturas-antiguas";
#elif L==5
	"SYSTEM.Hierarchy#eliminate-old-courses";
#elif L==6
	"SYSTEM.Hierarchy#eliminate-old-courses";
#elif L==7
	"SYSTEM.Hierarchy#eliminate-old-courses";
#elif L==8
	"SYSTEM.Hierarchy#eliminate-old-courses";
#elif L==9
	"SYSTEM.Hierarchy#eliminate-old-courses";
#endif

const char *Hlp_SYSTEM_Banners =
#if   L==1
	"SYSTEM.Banners";
#elif L==2
	"SYSTEM.Banners";
#elif L==3
	"SYSTEM.Banners";
#elif L==4
	"SYSTEM.Banners.es";
#elif L==5
	"SYSTEM.Banners";
#elif L==6
	"SYSTEM.Banners";
#elif L==7
	"SYSTEM.Banners";
#elif L==8
	"SYSTEM.Banners";
#elif L==9
	"SYSTEM.Banners";
#endif

const char *Hlp_SYSTEM_Banners_edit =
#if   L==1
	"SYSTEM.Banners#edit";
#elif L==2
	"SYSTEM.Banners#edit";
#elif L==3
	"SYSTEM.Banners#edit";
#elif L==4
	"SYSTEM.Banners.es#editar";
#elif L==5
	"SYSTEM.Banners#edit";
#elif L==6
	"SYSTEM.Banners#edit";
#elif L==7
	"SYSTEM.Banners#edit";
#elif L==8
	"SYSTEM.Banners#edit";
#elif L==9
	"SYSTEM.Banners#edit";
#endif

const char *Hlp_SYSTEM_Links =
#if   L==1
	"SYSTEM.Links";
#elif L==2
	"SYSTEM.Links";
#elif L==3
	"SYSTEM.Links";
#elif L==4
	"SYSTEM.Links.es";
#elif L==5
	"SYSTEM.Links";
#elif L==6
	"SYSTEM.Links";
#elif L==7
	"SYSTEM.Links";
#elif L==8
	"SYSTEM.Links";
#elif L==9
	"SYSTEM.Links";
#endif

const char *Hlp_SYSTEM_Links_edit =
#if   L==1
	"SYSTEM.Links#edit";
#elif L==2
	"SYSTEM.Links#edit";
#elif L==3
	"SYSTEM.Links#edit";
#elif L==4
	"SYSTEM.Links.es#editar";
#elif L==5
	"SYSTEM.Links#edit";
#elif L==6
	"SYSTEM.Links#edit";
#elif L==7
	"SYSTEM.Links#edit";
#elif L==8
	"SYSTEM.Links#edit";
#elif L==9
	"SYSTEM.Links#edit";
#endif

/***** COUNTRY tab *****/

const char *Hlp_COUNTRY_Information =
#if   L==1
	"COUNTRY.Information";
#elif L==2
	"COUNTRY.Information";
#elif L==3
	"COUNTRY.Information";
#elif L==4
	"COUNTRY.Information";
#elif L==5
	"COUNTRY.Information";
#elif L==6
	"COUNTRY.Information";
#elif L==7
	"COUNTRY.Information";
#elif L==8
	"COUNTRY.Information";
#elif L==9
	"COUNTRY.Information";
#endif

const char *Hlp_COUNTRY_Institutions =
#if   L==1
	"COUNTRY.Institutions";
#elif L==2
	"COUNTRY.Institutions";
#elif L==3
	"COUNTRY.Institutions";
#elif L==4
	"COUNTRY.Institutions";
#elif L==5
	"COUNTRY.Institutions";
#elif L==6
	"COUNTRY.Institutions";
#elif L==7
	"COUNTRY.Institutions";
#elif L==8
	"COUNTRY.Institutions";
#elif L==9
	"COUNTRY.Institutions";
#endif

/***** INSTITUTION tab *****/

const char *Hlp_INSTITUTION_Information =
#if   L==1
	"INSTITUTION.Information";
#elif L==2
	"INSTITUTION.Information";
#elif L==3
	"INSTITUTION.Information";
#elif L==4
	"INSTITUTION.Information.es";
#elif L==5
	"INSTITUTION.Information";
#elif L==6
	"INSTITUTION.Information";
#elif L==7
	"INSTITUTION.Information";
#elif L==8
	"INSTITUTION.Information";
#elif L==9
	"INSTITUTION.Information";
#endif

const char *Hlp_INSTITUTION_Centres =
#if   L==1
	"INSTITUTION.Centres";
#elif L==2
	"INSTITUTION.Centres";
#elif L==3
	"INSTITUTION.Centres";
#elif L==4
	"INSTITUTION.Centres.es";
#elif L==5
	"INSTITUTION.Centres";
#elif L==6
	"INSTITUTION.Centres";
#elif L==7
	"INSTITUTION.Centres";
#elif L==8
	"INSTITUTION.Centres";
#elif L==9
	"INSTITUTION.Centres";
#endif

const char *Hlp_INSTITUTION_Departments =
#if   L==1
	"INSTITUTION.Departments";
#elif L==2
	"INSTITUTION.Departments";
#elif L==3
	"INSTITUTION.Departments";
#elif L==4
	"INSTITUTION.Departments.es";
#elif L==5
	"INSTITUTION.Departments";
#elif L==6
	"INSTITUTION.Departments";
#elif L==7
	"INSTITUTION.Departments";
#elif L==8
	"INSTITUTION.Departments";
#elif L==9
	"INSTITUTION.Departments";
#endif

const char *Hlp_INSTITUTION_Departments_edit =
#if   L==1
	"INSTITUTION.Departments#edit";
#elif L==2
	"INSTITUTION.Departments#edit";
#elif L==3
	"INSTITUTION.Departments#edit";
#elif L==4
	"INSTITUTION.Departments.es#editar";
#elif L==5
	"INSTITUTION.Departments#edit";
#elif L==6
	"INSTITUTION.Departments#edit";
#elif L==7
	"INSTITUTION.Departments#edit";
#elif L==8
	"INSTITUTION.Departments#edit";
#elif L==9
	"INSTITUTION.Departments#edit";
#endif

const char *Hlp_INSTITUTION_Places =
#if   L==1
	"INSTITUTION.Places";
#elif L==2
	"INSTITUTION.Places";
#elif L==3
	"INSTITUTION.Places";
#elif L==4
	"INSTITUTION.Places.es";
#elif L==5
	"INSTITUTION.Places";
#elif L==6
	"INSTITUTION.Places";
#elif L==7
	"INSTITUTION.Places";
#elif L==8
	"INSTITUTION.Places";
#elif L==9
	"INSTITUTION.Places";
#endif

const char *Hlp_INSTITUTION_Places_edit =
#if   L==1
	"INSTITUTION.Places#edit";
#elif L==2
	"INSTITUTION.Places#edit";
#elif L==3
	"INSTITUTION.Places#edit";
#elif L==4
	"INSTITUTION.Places.es#editar";
#elif L==5
	"INSTITUTION.Places#edit";
#elif L==6
	"INSTITUTION.Places#edit";
#elif L==7
	"INSTITUTION.Places#edit";
#elif L==8
	"INSTITUTION.Places#edit";
#elif L==9
	"INSTITUTION.Places#edit";
#endif

const char *Hlp_INSTITUTION_Holidays =
#if   L==1
	"INSTITUTION.Holidays";
#elif L==2
	"INSTITUTION.Holidays";
#elif L==3
	"INSTITUTION.Holidays";
#elif L==4
	"INSTITUTION.Holidays.es";
#elif L==5
	"INSTITUTION.Holidays";
#elif L==6
	"INSTITUTION.Holidays";
#elif L==7
	"INSTITUTION.Holidays";
#elif L==8
	"INSTITUTION.Holidays";
#elif L==9
	"INSTITUTION.Holidays";
#endif

const char *Hlp_INSTITUTION_Holidays_edit =
#if   L==1
	"INSTITUTION.Holidays#edit";
#elif L==2
	"INSTITUTION.Holidays#edit";
#elif L==3
	"INSTITUTION.Holidays#edit";
#elif L==4
	"INSTITUTION.Holidays.es#editar";
#elif L==5
	"INSTITUTION.Holidays#edit";
#elif L==6
	"INSTITUTION.Holidays#edit";
#elif L==7
	"INSTITUTION.Holidays#edit";
#elif L==8
	"INSTITUTION.Holidays#edit";
#elif L==9
	"INSTITUTION.Holidays#edit";
#endif

/***** CENTRE tab *****/

const char *Hlp_CENTRE_Information =
#if   L==1
	"CENTRE.Information";
#elif L==2
	"CENTRE.Information";
#elif L==3
	"CENTRE.Information";
#elif L==4
	"CENTRE.Information";
#elif L==5
	"CENTRE.Information";
#elif L==6
	"CENTRE.Information";
#elif L==7
	"CENTRE.Information";
#elif L==8
	"CENTRE.Information";
#elif L==9
	"CENTRE.Information";
#endif

const char *Hlp_CENTRE_DegreeTypes =
#if   L==1
	"CENTRE.DegreeTypes";
#elif L==2
	"CENTRE.DegreeTypes";
#elif L==3
	"CENTRE.DegreeTypes";
#elif L==4
	"CENTRE.DegreeTypes";
#elif L==5
	"CENTRE.DegreeTypes";
#elif L==6
	"CENTRE.DegreeTypes";
#elif L==7
	"CENTRE.DegreeTypes";
#elif L==8
	"CENTRE.DegreeTypes";
#elif L==9
	"CENTRE.DegreeTypes";
#endif

const char *Hlp_CENTRE_DegreeTypes_edit =
#if   L==1
	"CENTRE.DegreeTypes#edit";
#elif L==2
	"CENTRE.DegreeTypes#edit";
#elif L==3
	"CENTRE.DegreeTypes#edit";
#elif L==4
	"CENTRE.DegreeTypes#edit";
#elif L==5
	"CENTRE.DegreeTypes#edit";
#elif L==6
	"CENTRE.DegreeTypes#edit";
#elif L==7
	"CENTRE.DegreeTypes#edit";
#elif L==8
	"CENTRE.DegreeTypes#edit";
#elif L==9
	"CENTRE.DegreeTypes#edit";
#endif

const char *Hlp_CENTRE_Degrees =
#if   L==1
	"CENTRE.Degrees";
#elif L==2
	"CENTRE.Degrees";
#elif L==3
	"CENTRE.Degrees";
#elif L==4
	"CENTRE.Degrees";
#elif L==5
	"CENTRE.Degrees";
#elif L==6
	"CENTRE.Degrees";
#elif L==7
	"CENTRE.Degrees";
#elif L==8
	"CENTRE.Degrees";
#elif L==9
	"CENTRE.Degrees";
#endif

/***** DEGREE tab *****/

const char *Hlp_DEGREE_Information =
#if   L==1
	"DEGREE.Information";
#elif L==2
	"DEGREE.Information";
#elif L==3
	"DEGREE.Information";
#elif L==4
	"DEGREE.Information";
#elif L==5
	"DEGREE.Information";
#elif L==6
	"DEGREE.Information";
#elif L==7
	"DEGREE.Information";
#elif L==8
	"DEGREE.Information";
#elif L==9
	"DEGREE.Information";
#endif

const char *Hlp_DEGREE_Courses =
#if   L==1
	"DEGREE.Courses";
#elif L==2
	"DEGREE.Courses";
#elif L==3
	"DEGREE.Courses";
#elif L==4
	"DEGREE.Courses";
#elif L==5
	"DEGREE.Courses";
#elif L==6
	"DEGREE.Courses";
#elif L==7
	"DEGREE.Courses";
#elif L==8
	"DEGREE.Courses";
#elif L==9
	"DEGREE.Courses";
#endif

/***** COURSE tab *****/

const char *Hlp_COURSE_Information =
#if   L==1
	"COURSE.Information";
#elif L==2
	"COURSE.Information";
#elif L==3
	"COURSE.Information";
#elif L==4
	"COURSE.Information";
#elif L==5
	"COURSE.Information";
#elif L==6
	"COURSE.Information";
#elif L==7
	"COURSE.Information";
#elif L==8
	"COURSE.Information";
#elif L==9
	"COURSE.Information";
#endif

const char *Hlp_COURSE_Information_textual_information =
#if   L==1
	"COURSE.Information#textual-information";
#elif L==2
	"COURSE.Information#textual-information";
#elif L==3
	"COURSE.Information#textual-information";
#elif L==4
	"COURSE.Information#textual-information";
#elif L==5
	"COURSE.Information#textual-information";
#elif L==6
	"COURSE.Information#textual-information";
#elif L==7
	"COURSE.Information#textual-information";
#elif L==8
	"COURSE.Information#textual-information";
#elif L==9
	"COURSE.Information#textual-information";
#endif

const char *Hlp_COURSE_Information_edit =
#if   L==1
	"COURSE.Information#edit";
#elif L==2
	"COURSE.Information#edit";
#elif L==3
	"COURSE.Information#edit";
#elif L==4
	"COURSE.Information#edit";
#elif L==5
	"COURSE.Information#edit";
#elif L==6
	"COURSE.Information#edit";
#elif L==7
	"COURSE.Information#edit";
#elif L==8
	"COURSE.Information#edit";
#elif L==9
	"COURSE.Information#edit";
#endif

const char *Hlp_COURSE_Guide =
#if   L==1
	"COURSE.Guide";
#elif L==2
	"COURSE.Guide";
#elif L==3
	"COURSE.Guide";
#elif L==4
	"COURSE.Guide";
#elif L==5
	"COURSE.Guide";
#elif L==6
	"COURSE.Guide";
#elif L==7
	"COURSE.Guide";
#elif L==8
	"COURSE.Guide";
#elif L==9
	"COURSE.Guide";
#endif

const char *Hlp_COURSE_Guide_edit =
#if   L==1
	"COURSE.Guide#edit";
#elif L==2
	"COURSE.Guide#edit";
#elif L==3
	"COURSE.Guide#edit";
#elif L==4
	"COURSE.Guide#edit";
#elif L==5
	"COURSE.Guide#edit";
#elif L==6
	"COURSE.Guide#edit";
#elif L==7
	"COURSE.Guide#edit";
#elif L==8
	"COURSE.Guide#edit";
#elif L==9
	"COURSE.Guide#edit";
#endif

const char *Hlp_COURSE_Syllabus =
#if   L==1
	"COURSE.Syllabus";
#elif L==2
	"COURSE.Syllabus";
#elif L==3
	"COURSE.Syllabus";
#elif L==4
	"COURSE.Syllabus";
#elif L==5
	"COURSE.Syllabus";
#elif L==6
	"COURSE.Syllabus";
#elif L==7
	"COURSE.Syllabus";
#elif L==8
	"COURSE.Syllabus";
#elif L==9
	"COURSE.Syllabus";
#endif

const char *Hlp_COURSE_Syllabus_edit =
#if   L==1
	"COURSE.Syllabus#edit";
#elif L==2
	"COURSE.Syllabus#edit";
#elif L==3
	"COURSE.Syllabus#edit";
#elif L==4
	"COURSE.Syllabus#edit";
#elif L==5
	"COURSE.Syllabus#edit";
#elif L==6
	"COURSE.Syllabus#edit";
#elif L==7
	"COURSE.Syllabus#edit";
#elif L==8
	"COURSE.Syllabus#edit";
#elif L==9
	"COURSE.Syllabus#edit";
#endif

const char *Hlp_COURSE_Timetable =
#if   L==1
	"COURSE.Timetable";
#elif L==2
	"COURSE.Timetable";
#elif L==3
	"COURSE.Timetable";
#elif L==4
	"COURSE.Timetable";
#elif L==5
	"COURSE.Timetable";
#elif L==6
	"COURSE.Timetable";
#elif L==7
	"COURSE.Timetable";
#elif L==8
	"COURSE.Timetable";
#elif L==9
	"COURSE.Timetable";
#endif

const char *Hlp_COURSE_Bibliography =
#if   L==1
	"COURSE.Bibliography";
#elif L==2
	"COURSE.Bibliography";
#elif L==3
	"COURSE.Bibliography";
#elif L==4
	"COURSE.Bibliography";
#elif L==5
	"COURSE.Bibliography";
#elif L==6
	"COURSE.Bibliography";
#elif L==7
	"COURSE.Bibliography";
#elif L==8
	"COURSE.Bibliography";
#elif L==9
	"COURSE.Bibliography";
#endif

const char *Hlp_COURSE_Bibliography_edit =
#if   L==1
	"COURSE.Bibliography#edit";
#elif L==2
	"COURSE.Bibliography#edit";
#elif L==3
	"COURSE.Bibliography#edit";
#elif L==4
	"COURSE.Bibliography#edit";
#elif L==5
	"COURSE.Bibliography#edit";
#elif L==6
	"COURSE.Bibliography#edit";
#elif L==7
	"COURSE.Bibliography#edit";
#elif L==8
	"COURSE.Bibliography#edit";
#elif L==9
	"COURSE.Bibliography#edit";
#endif

const char *Hlp_COURSE_FAQ =
#if   L==1
	"COURSE.FAQ";
#elif L==2
	"COURSE.FAQ";
#elif L==3
	"COURSE.FAQ";
#elif L==4
	"COURSE.FAQ";
#elif L==5
	"COURSE.FAQ";
#elif L==6
	"COURSE.FAQ";
#elif L==7
	"COURSE.FAQ";
#elif L==8
	"COURSE.FAQ";
#elif L==9
	"COURSE.FAQ";
#endif

const char *Hlp_COURSE_FAQ_edit =
#if   L==1
	"COURSE.FAQ#edit";
#elif L==2
	"COURSE.FAQ#edit";
#elif L==3
	"COURSE.FAQ#edit";
#elif L==4
	"COURSE.FAQ#edit";
#elif L==5
	"COURSE.FAQ#edit";
#elif L==6
	"COURSE.FAQ#edit";
#elif L==7
	"COURSE.FAQ#edit";
#elif L==8
	"COURSE.FAQ#edit";
#elif L==9
	"COURSE.FAQ#edit";
#endif

const char *Hlp_COURSE_Links =
#if   L==1
	"COURSE.Links";
#elif L==2
	"COURSE.Links";
#elif L==3
	"COURSE.Links";
#elif L==4
	"COURSE.Links";
#elif L==5
	"COURSE.Links";
#elif L==6
	"COURSE.Links";
#elif L==7
	"COURSE.Links";
#elif L==8
	"COURSE.Links";
#elif L==9
	"COURSE.Links";
#endif

const char *Hlp_COURSE_Links_edit =
#if   L==1
	"COURSE.Links#edit";
#elif L==2
	"COURSE.Links#edit";
#elif L==3
	"COURSE.Links#edit";
#elif L==4
	"COURSE.Links#edit";
#elif L==5
	"COURSE.Links#edit";
#elif L==6
	"COURSE.Links#edit";
#elif L==7
	"COURSE.Links#edit";
#elif L==8
	"COURSE.Links#edit";
#elif L==9
	"COURSE.Links#edit";
#endif

/***** ASSESSMENT tab *****/

const char *Hlp_ASSESSMENT_System =
#if   L==1
	"ASSESSMENT.System";
#elif L==2
	"ASSESSMENT.System";
#elif L==3
	"ASSESSMENT.System";
#elif L==4
	"ASSESSMENT.System";
#elif L==5
	"ASSESSMENT.System";
#elif L==6
	"ASSESSMENT.System";
#elif L==7
	"ASSESSMENT.System";
#elif L==8
	"ASSESSMENT.System";
#elif L==9
	"ASSESSMENT.System";
#endif

const char *Hlp_ASSESSMENT_System_edit =
#if   L==1
	"ASSESSMENT.System#edit";
#elif L==2
	"ASSESSMENT.System#edit";
#elif L==3
	"ASSESSMENT.System#edit";
#elif L==4
	"ASSESSMENT.System#edit";
#elif L==5
	"ASSESSMENT.System#edit";
#elif L==6
	"ASSESSMENT.System#edit";
#elif L==7
	"ASSESSMENT.System#edit";
#elif L==8
	"ASSESSMENT.System#edit";
#elif L==9
	"ASSESSMENT.System#edit";
#endif

const char *Hlp_ASSESSMENT_Assignments =
#if   L==1
	"ASSESSMENT.Assignments";
#elif L==2
	"ASSESSMENT.Assignments";
#elif L==3
	"ASSESSMENT.Assignments";
#elif L==4
	"ASSESSMENT.Assignments";
#elif L==5
	"ASSESSMENT.Assignments";
#elif L==6
	"ASSESSMENT.Assignments";
#elif L==7
	"ASSESSMENT.Assignments";
#elif L==8
	"ASSESSMENT.Assignments";
#elif L==9
	"ASSESSMENT.Assignments";
#endif

const char *Hlp_ASSESSMENT_Assignments_new_assignment =
#if   L==1
	"ASSESSMENT.Assignments#new-assignment";
#elif L==2
	"ASSESSMENT.Assignments#new-assignment";
#elif L==3
	"ASSESSMENT.Assignments#new-assignment";
#elif L==4
	"ASSESSMENT.Assignments#new-assignment";
#elif L==5
	"ASSESSMENT.Assignments#new-assignment";
#elif L==6
	"ASSESSMENT.Assignments#new-assignment";
#elif L==7
	"ASSESSMENT.Assignments#new-assignment";
#elif L==8
	"ASSESSMENT.Assignments#new-assignment";
#elif L==9
	"ASSESSMENT.Assignments#new-assignment";
#endif

const char *Hlp_ASSESSMENT_Assignments_edit_assignment =
#if   L==1
	"ASSESSMENT.Assignments#edit-assignment";
#elif L==2
	"ASSESSMENT.Assignments#edit-assignment";
#elif L==3
	"ASSESSMENT.Assignments#edit-assignment";
#elif L==4
	"ASSESSMENT.Assignments#edit-assignment";
#elif L==5
	"ASSESSMENT.Assignments#edit-assignment";
#elif L==6
	"ASSESSMENT.Assignments#edit-assignment";
#elif L==7
	"ASSESSMENT.Assignments#edit-assignment";
#elif L==8
	"ASSESSMENT.Assignments#edit-assignment";
#elif L==9
	"ASSESSMENT.Assignments#edit-assignment";
#endif

const char *Hlp_ASSESSMENT_Tests =
#if   L==1
	"ASSESSMENT.Tests";
#elif L==2
	"ASSESSMENT.Tests";
#elif L==3
	"ASSESSMENT.Tests";
#elif L==4
	"ASSESSMENT.Tests";
#elif L==5
	"ASSESSMENT.Tests";
#elif L==6
	"ASSESSMENT.Tests";
#elif L==7
	"ASSESSMENT.Tests";
#elif L==8
	"ASSESSMENT.Tests";
#elif L==9
	"ASSESSMENT.Tests";
#endif

const char *Hlp_ASSESSMENT_Tests_test_results =
#if   L==1
	"ASSESSMENT.Tests#test-results";
#elif L==2
	"ASSESSMENT.Tests#test-results";
#elif L==3
	"ASSESSMENT.Tests#test-results";
#elif L==4
	"ASSESSMENT.Tests#test-results";
#elif L==5
	"ASSESSMENT.Tests#test-results";
#elif L==6
	"ASSESSMENT.Tests#test-results";
#elif L==7
	"ASSESSMENT.Tests#test-results";
#elif L==8
	"ASSESSMENT.Tests#test-results";
#elif L==9
	"ASSESSMENT.Tests#test-results";
#endif

const char *Hlp_ASSESSMENT_Announcements =
#if   L==1
	"ASSESSMENT.Announcements";
#elif L==2
	"ASSESSMENT.Announcements";
#elif L==3
	"ASSESSMENT.Announcements";
#elif L==4
	"ASSESSMENT.Announcements";
#elif L==5
	"ASSESSMENT.Announcements";
#elif L==6
	"ASSESSMENT.Announcements";
#elif L==7
	"ASSESSMENT.Announcements";
#elif L==8
	"ASSESSMENT.Announcements";
#elif L==9
	"ASSESSMENT.Announcements";
#endif

const char *Hlp_ASSESSMENT_Announcements_new_announcement =
#if   L==1
	"ASSESSMENT.Announcements#new-announcement";
#elif L==2
	"ASSESSMENT.Announcements#new-announcement";
#elif L==3
	"ASSESSMENT.Announcements#new-announcement";
#elif L==4
	"ASSESSMENT.Announcements#new-announcement";
#elif L==5
	"ASSESSMENT.Announcements#new-announcement";
#elif L==6
	"ASSESSMENT.Announcements#new-announcement";
#elif L==7
	"ASSESSMENT.Announcements#new-announcement";
#elif L==8
	"ASSESSMENT.Announcements#new-announcement";
#elif L==9
	"ASSESSMENT.Announcements#new-announcement";
#endif

const char *Hlp_ASSESSMENT_Announcements_edit_announcement =
#if   L==1
	"ASSESSMENT.Announcements#edit-announcement";
#elif L==2
	"ASSESSMENT.Announcements#edit-announcement";
#elif L==3
	"ASSESSMENT.Announcements#edit-announcement";
#elif L==4
	"ASSESSMENT.Announcements#edit-announcement";
#elif L==5
	"ASSESSMENT.Announcements#edit-announcement";
#elif L==6
	"ASSESSMENT.Announcements#edit-announcement";
#elif L==7
	"ASSESSMENT.Announcements#edit-announcement";
#elif L==8
	"ASSESSMENT.Announcements#edit-announcement";
#elif L==9
	"ASSESSMENT.Announcements#edit-announcement";
#endif

/***** FILES tab *****/

const char *Hlp_FILES_Documents =
#if   L==1
	"FILES.Documents";
#elif L==2
	"FILES.Documents";
#elif L==3
	"FILES.Documents";
#elif L==4
	"FILES.Documents.es";
#elif L==5
	"FILES.Documents";
#elif L==6
	"FILES.Documents";
#elif L==7
	"FILES.Documents";
#elif L==8
	"FILES.Documents";
#elif L==9
	"FILES.Documents";
#endif

const char *Hlp_FILES_Private =
#if   L==1
	"FILES.Private";
#elif L==2
	"FILES.Private";
#elif L==3
	"FILES.Private";
#elif L==4
	"FILES.Private.es";
#elif L==5
	"FILES.Private";
#elif L==6
	"FILES.Private";
#elif L==7
	"FILES.Private";
#elif L==8
	"FILES.Private";
#elif L==9
	"FILES.Private";
#endif

const char *Hlp_FILES_Shared =
#if   L==1
	"FILES.Shared";
#elif L==2
	"FILES.Shared";
#elif L==3
	"FILES.Shared";
#elif L==4
	"FILES.Shared.es";
#elif L==5
	"FILES.Shared";
#elif L==6
	"FILES.Shared";
#elif L==7
	"FILES.Shared";
#elif L==8
	"FILES.Shared";
#elif L==9
	"FILES.Shared";
#endif

const char *Hlp_FILES_Homework_for_students =
#if   L==1
	"FILES.Homework#for-students";
#elif L==2
	"FILES.Homework#for-students";
#elif L==3
	"FILES.Homework#for-students";
#elif L==4
	"FILES.Homework.es#para-estudiantes";
#elif L==5
	"FILES.Homework#for-students";
#elif L==6
	"FILES.Homework#for-students";
#elif L==7
	"FILES.Homework#for-students";
#elif L==8
	"FILES.Homework#for-students";
#elif L==9
	"FILES.Homework#for-students";
#endif

const char *Hlp_FILES_Homework_for_teachers =
#if   L==1
	"FILES.Homework#for-teachers";
#elif L==2
	"FILES.Homework#for-teachers";
#elif L==3
	"FILES.Homework#for-teachers";
#elif L==4
	"FILES.Homework.es#para-profesores";
#elif L==5
	"FILES.Homework#for-teachers";
#elif L==6
	"FILES.Homework#for-teachers";
#elif L==7
	"FILES.Homework#for-teachers";
#elif L==8
	"FILES.Homework#for-teachers";
#elif L==9
	"FILES.Homework#for-teachers";
#endif

const char *Hlp_FILES_Marks =
#if   L==1
	"FILES.Marks";
#elif L==2
	"FILES.Marks";
#elif L==3
	"FILES.Marks";
#elif L==4
	"FILES.Marks.es";
#elif L==5
	"FILES.Marks";
#elif L==6
	"FILES.Marks";
#elif L==7
	"FILES.Marks";
#elif L==8
	"FILES.Marks";
#elif L==9
	"FILES.Marks";
#endif

/***** USERS tab *****/

const char *Hlp_USERS_Groups =
#if   L==1
	"USERS.Groups";
#elif L==2
	"USERS.Groups";
#elif L==3
	"USERS.Groups";
#elif L==4
	"USERS.Groups.es";
#elif L==5
	"USERS.Groups";
#elif L==6
	"USERS.Groups";
#elif L==7
	"USERS.Groups";
#elif L==8
	"USERS.Groups";
#elif L==9
	"USERS.Groups";
#endif

const char *Hlp_USERS_Students =
#if   L==1
	"USERS.Students";
#elif L==2
	"USERS.Students";
#elif L==3
	"USERS.Students";
#elif L==4
	"USERS.Students.es";
#elif L==5
	"USERS.Students";
#elif L==6
	"USERS.Students";
#elif L==7
	"USERS.Students";
#elif L==8
	"USERS.Students";
#elif L==9
	"USERS.Students";
#endif

const char *Hlp_USERS_Students_shared_record_card =
#if   L==1
	"USERS.Students#shared-record-card";
#elif L==2
	"USERS.Students#shared-record-card";
#elif L==3
	"USERS.Students#shared-record-card";
#elif L==4
	"USERS.Students.es#ficha-compartida";
#elif L==5
	"USERS.Students#shared-record-card";
#elif L==6
	"USERS.Students#shared-record-card";
#elif L==7
	"USERS.Students#shared-record-card";
#elif L==8
	"USERS.Students#shared-record-card";
#elif L==9
	"USERS.Students#shared-record-card";
#endif

const char *Hlp_USERS_Students_course_record_card =
#if   L==1
	"USERS.Students#course-record-card";
#elif L==2
	"USERS.Students#course-record-card";
#elif L==3
	"USERS.Students#course-record-card";
#elif L==4
	"USERS.Students.es#ficha-de-la-asignatura";
#elif L==5
	"USERS.Students#course-record-card";
#elif L==6
	"USERS.Students#course-record-card";
#elif L==7
	"USERS.Students#course-record-card";
#elif L==8
	"USERS.Students#course-record-card";
#elif L==9
	"USERS.Students#course-record-card";
#endif

const char *Hlp_USERS_Administration_administer_one_user =
#if   L==1
	"USERS.Administration#administer-one-user";
#elif L==2
	"USERS.Administration#administer-one-user";
#elif L==3
	"USERS.Administration#administer-one-user";
#elif L==4
	"USERS.Administration.es#administrar-un-usuario";
#elif L==5
	"USERS.Administration#administer-one-user";
#elif L==6
	"USERS.Administration#administer-one-user";
#elif L==7
	"USERS.Administration#administer-one-user";
#elif L==8
	"USERS.Administration#administer-one-user";
#elif L==9
	"USERS.Administration#administer-one-user";
#endif

const char *Hlp_USERS_Administration_administer_multiple_users =
#if   L==1
	"USERS.Administration#administer-multiple-users";
#elif L==2
	"USERS.Administration#administer-multiple-users";
#elif L==3
	"USERS.Administration#administer-multiple-users";
#elif L==4
	"USERS.Administration.es#administrar-varios-usuarios";
#elif L==5
	"USERS.Administration#administer-multiple-users";
#elif L==6
	"USERS.Administration#administer-multiple-users";
#elif L==7
	"USERS.Administration#administer-multiple-users";
#elif L==8
	"USERS.Administration#administer-multiple-users";
#elif L==9
	"USERS.Administration#administer-multiple-users";
#endif

const char *Hlp_USERS_Administration_remove_all_students =
#if   L==1
	"USERS.Administration#remove-all-students";
#elif L==2
	"USERS.Administration#remove-all-students";
#elif L==3
	"USERS.Administration#remove-all-students";
#elif L==4
	"USERS.Administration.es#eliminar-todos-los-estudiantes";
#elif L==5
	"USERS.Administration#remove-all-students";
#elif L==6
	"USERS.Administration#remove-all-students";
#elif L==7
	"USERS.Administration#remove-all-students";
#elif L==8
	"USERS.Administration#remove-all-students";
#elif L==9
	"USERS.Administration#remove-all-students";
#endif

const char *Hlp_USERS_Teachers =
#if   L==1
	"USERS.Teachers";
#elif L==2
	"USERS.Teachers";
#elif L==3
	"USERS.Teachers";
#elif L==4
	"USERS.Teachers.es";
#elif L==5
	"USERS.Teachers";
#elif L==6
	"USERS.Teachers";
#elif L==7
	"USERS.Teachers";
#elif L==8
	"USERS.Teachers";
#elif L==9
	"USERS.Teachers";
#endif

const char *Hlp_USERS_Teachers_shared_record_card =
#if   L==1
	"USERS.Teachers#shared-record-card";
#elif L==2
	"USERS.Teachers#shared-record-card";
#elif L==3
	"USERS.Teachers#shared-record-card";
#elif L==4
	"USERS.Teachers.es#ficha-compartida";
#elif L==5
	"USERS.Teachers#shared-record-card";
#elif L==6
	"USERS.Teachers#shared-record-card";
#elif L==7
	"USERS.Teachers#shared-record-card";
#elif L==8
	"USERS.Teachers#shared-record-card";
#elif L==9
	"USERS.Teachers#shared-record-card";
#endif

const char *Hlp_USERS_Teachers_timetable =
#if   L==1
	"USERS.Teachers#timetable";
#elif L==2
	"USERS.Teachers#timetable";
#elif L==3
	"USERS.Teachers#timetable";
#elif L==4
	"USERS.Teachers.es#horario";
#elif L==5
	"USERS.Teachers#timetable";
#elif L==6
	"USERS.Teachers#timetable";
#elif L==7
	"USERS.Teachers#timetable";
#elif L==8
	"USERS.Teachers#timetable";
#elif L==9
	"USERS.Teachers#timetable";
#endif

const char *Hlp_USERS_Administrators =
#if   L==1
	"USERS.Administrators";
#elif L==2
	"USERS.Administrators";
#elif L==3
	"USERS.Administrators";
#elif L==4
	"USERS.Administrators.es";
#elif L==5
	"USERS.Administrators";
#elif L==6
	"USERS.Administrators";
#elif L==7
	"USERS.Administrators";
#elif L==8
	"USERS.Administrators";
#elif L==9
	"USERS.Administrators";
#endif

const char *Hlp_USERS_Guests =
#if   L==1
	"USERS.Guests";
#elif L==2
	"USERS.Guests";
#elif L==3
	"USERS.Guests";
#elif L==4
	"USERS.Guests.es";
#elif L==5
	"USERS.Guests";
#elif L==6
	"USERS.Guests";
#elif L==7
	"USERS.Guests";
#elif L==8
	"USERS.Guests";
#elif L==9
	"USERS.Guests";
#endif

const char *Hlp_USERS_Duplicates_possibly_duplicate_users =
#if   L==1
	"USERS.Duplicates#possibly-duplicate-users";
#elif L==2
	"USERS.Duplicates#possibly-duplicate-users";
#elif L==3
	"USERS.Duplicates#possibly-duplicate-users";
#elif L==4
	"USERS.Duplicates.es#usuarios-posiblemente-duplicados";
#elif L==5
	"USERS.Duplicates#possibly-duplicate-users";
#elif L==6
	"USERS.Duplicates#possibly-duplicate-users";
#elif L==7
	"USERS.Duplicates#possibly-duplicate-users";
#elif L==8
	"USERS.Duplicates#possibly-duplicate-users";
#elif L==9
	"USERS.Duplicates#possibly-duplicate-users";
#endif

const char *Hlp_USERS_Duplicates_similar_users =
#if   L==1
	"USERS.Duplicates#similar-users";
#elif L==2
	"USERS.Duplicates#similar-users";
#elif L==3
	"USERS.Duplicates#similar-users";
#elif L==4
	"USERS.Duplicates.es#usuarios-similares";
#elif L==5
	"USERS.Duplicates#similar-users";
#elif L==6
	"USERS.Duplicates#similar-users";
#elif L==7
	"USERS.Duplicates#similar-users";
#elif L==8
	"USERS.Duplicates#similar-users";
#elif L==9
	"USERS.Duplicates#similar-users";
#endif

const char *Hlp_USERS_Attendance =
#if   L==1
	"USERS.Attendance";
#elif L==2
	"USERS.Attendance";
#elif L==3
	"USERS.Attendance";
#elif L==4
	"USERS.Attendance.es";
#elif L==5
	"USERS.Attendance";
#elif L==6
	"USERS.Attendance";
#elif L==7
	"USERS.Attendance";
#elif L==8
	"USERS.Attendance";
#elif L==9
	"USERS.Attendance";
#endif

const char *Hlp_USERS_Attendance_new_event =
#if   L==1
	"USERS.Attendance#new-event";
#elif L==2
	"USERS.Attendance#new-event";
#elif L==3
	"USERS.Attendance#new-event";
#elif L==4
	"USERS.Attendance.es#nuevo-evento";
#elif L==5
	"USERS.Attendance#new-event";
#elif L==6
	"USERS.Attendance#new-event";
#elif L==7
	"USERS.Attendance#new-event";
#elif L==8
	"USERS.Attendance#new-event";
#elif L==9
	"USERS.Attendance#new-event";
#endif

const char *Hlp_USERS_Attendance_edit_event =
#if   L==1
	"USERS.Attendance#edit-event";
#elif L==2
	"USERS.Attendance#edit-event";
#elif L==3
	"USERS.Attendance#edit-event";
#elif L==4
	"USERS.Attendance.es#editar-evento";
#elif L==5
	"USERS.Attendance#edit-event";
#elif L==6
	"USERS.Attendance#edit-event";
#elif L==7
	"USERS.Attendance#edit-event";
#elif L==8
	"USERS.Attendance#edit-event";
#elif L==9
	"USERS.Attendance#edit-event";
#endif

const char *Hlp_USERS_Attendance_attendance_list =
#if   L==1
	"USERS.Attendance#attendance-list";
#elif L==2
	"USERS.Attendance#attendance-list";
#elif L==3
	"USERS.Attendance#attendance-list";
#elif L==4
	"USERS.Attendance.es#lista-de-asistencia";
#elif L==5
	"USERS.Attendance#attendance-list";
#elif L==6
	"USERS.Attendance#attendance-list";
#elif L==7
	"USERS.Attendance#attendance-list";
#elif L==8
	"USERS.Attendance#attendance-list";
#elif L==9
	"USERS.Attendance#attendance-list";
#endif

const char *Hlp_USERS_SignUp =
#if   L==1
	"USERS.SignUp";
#elif L==2
	"USERS.SignUp";
#elif L==3
	"USERS.SignUp";
#elif L==4
	"USERS.SignUp.es";
#elif L==5
	"USERS.SignUp";
#elif L==6
	"USERS.SignUp";
#elif L==7
	"USERS.SignUp";
#elif L==8
	"USERS.SignUp";
#elif L==9
	"USERS.SignUp";
#endif

const char *Hlp_USERS_SignUp_confirm_enrolment =
#if   L==1
	"USERS.SignUp#confirm_enrolment";
#elif L==2
	"USERS.SignUp#confirm_enrolment";
#elif L==3
	"USERS.SignUp#confirm_enrolment";
#elif L==4
	"USERS.SignUp.es#confirmar-inscripci%C3%B3n";
#elif L==5
	"USERS.SignUp#confirm_enrolment";
#elif L==6
	"USERS.SignUp#confirm_enrolment";
#elif L==7
	"USERS.SignUp#confirm_enrolment";
#elif L==8
	"USERS.SignUp#confirm_enrolment";
#elif L==9
	"USERS.SignUp#confirm_enrolment";
#endif

const char *Hlp_USERS_Requests =
#if   L==1
	"USERS.Requests";
#elif L==2
	"USERS.Requests";
#elif L==3
	"USERS.Requests";
#elif L==4
	"USERS.Requests.es";
#elif L==5
	"USERS.Requests";
#elif L==6
	"USERS.Requests";
#elif L==7
	"USERS.Requests";
#elif L==8
	"USERS.Requests";
#elif L==9
	"USERS.Requests";
#endif

const char *Hlp_USERS_Connected =
#if   L==1
	"USERS.Connected";
#elif L==2
	"USERS.Connected";
#elif L==3
	"USERS.Connected";
#elif L==4
	"USERS.Connected.es";
#elif L==5
	"USERS.Connected";
#elif L==6
	"USERS.Connected";
#elif L==7
	"USERS.Connected";
#elif L==8
	"USERS.Connected";
#elif L==9
	"USERS.Connected";
#endif

const char *Hlp_USERS_Connected_last_clicks =
#if   L==1
	"USERS.Connected#last-clicks";
#elif L==2
	"USERS.Connected#last-clicks";
#elif L==3
	"USERS.Connected#last-clicks";
#elif L==4
	"USERS.Connected.es#%C3%9Altimos-clics";
#elif L==5
	"USERS.Connected#last-clicks";
#elif L==6
	"USERS.Connected#last-clicks";
#elif L==7
	"USERS.Connected#last-clicks";
#elif L==8
	"USERS.Connected#last-clicks";
#elif L==9
	"USERS.Connected#last-clicks";
#endif

/***** SOCIAL tab *****/

const char *Hlp_SOCIAL_Timeline =
#if   L==1
	"SOCIAL.Timeline";
#elif L==2
	"SOCIAL.Timeline";
#elif L==3
	"SOCIAL.Timeline";
#elif L==4
	"SOCIAL.Timeline.es";
#elif L==5
	"SOCIAL.Timeline";
#elif L==6
	"SOCIAL.Timeline";
#elif L==7
	"SOCIAL.Timeline";
#elif L==8
	"SOCIAL.Timeline";
#elif L==9
	"SOCIAL.Timeline";
#endif

const char *Hlp_SOCIAL_Profiles_view_public_profile =
#if   L==1
	"SOCIAL.Profiles#view-public-profile";
#elif L==2
	"SOCIAL.Profiles#view-public-profile";
#elif L==3
	"SOCIAL.Profiles#view-public-profile";
#elif L==4
	"SOCIAL.Profiles.es#ver-perfil-p%C3%BAblico";
#elif L==5
	"SOCIAL.Profiles#view-public-profile";
#elif L==6
	"SOCIAL.Profiles#view-public-profile";
#elif L==7
	"SOCIAL.Profiles#view-public-profile";
#elif L==8
	"SOCIAL.Profiles#view-public-profile";
#elif L==9
	"SOCIAL.Profiles#view-public-profile";
#endif

const char *Hlp_SOCIAL_Profiles_who_to_follow =
#if   L==1
	"SOCIAL.Profiles#who-to-follow";
#elif L==2
	"SOCIAL.Profiles#who-to-follow";
#elif L==3
	"SOCIAL.Profiles#who-to-follow";
#elif L==4
	"SOCIAL.Profiles.es#a-qui%C3%A9n-seguir";
#elif L==5
	"SOCIAL.Profiles#who-to-follow";
#elif L==6
	"SOCIAL.Profiles#who-to-follow";
#elif L==7
	"SOCIAL.Profiles#who-to-follow";
#elif L==8
	"SOCIAL.Profiles#who-to-follow";
#elif L==9
	"SOCIAL.Profiles#who-to-follow";
#endif

const char *Hlp_SOCIAL_Forums =
#if   L==1
	"SOCIAL.Forums";
#elif L==2
	"SOCIAL.Forums";
#elif L==3
	"SOCIAL.Forums";
#elif L==4
	"SOCIAL.Forums.es";
#elif L==5
	"SOCIAL.Forums";
#elif L==6
	"SOCIAL.Forums";
#elif L==7
	"SOCIAL.Forums";
#elif L==8
	"SOCIAL.Forums";
#elif L==9
	"SOCIAL.Forums";
#endif

/***** MESSAGES tab *****/

const char *Hlp_MESSAGES_Notifications =
#if   L==1
	"MESSAGES.Notifications";
#elif L==2
	"MESSAGES.Notifications";
#elif L==3
	"MESSAGES.Notifications";
#elif L==4
	"MESSAGES.Notifications.es";
#elif L==5
	"MESSAGES.Notifications";
#elif L==6
	"MESSAGES.Notifications";
#elif L==7
	"MESSAGES.Notifications";
#elif L==8
	"MESSAGES.Notifications";
#elif L==9
	"MESSAGES.Notifications";
#endif

const char *Hlp_MESSAGES_Domains =
#if   L==1
	"MESSAGES.Domains";
#elif L==2
	"MESSAGES.Domains";
#elif L==3
	"MESSAGES.Domains";
#elif L==4
	"MESSAGES.Domains.es";
#elif L==5
	"MESSAGES.Domains";
#elif L==6
	"MESSAGES.Domains";
#elif L==7
	"MESSAGES.Domains";
#elif L==8
	"MESSAGES.Domains";
#elif L==9
	"MESSAGES.Domains";
#endif

const char *Hlp_MESSAGES_Domains_edit =
#if   L==1
	"MESSAGES.Domains#edit";
#elif L==2
	"MESSAGES.Domains#edit";
#elif L==3
	"MESSAGES.Domains#edit";
#elif L==4
	"MESSAGES.Domains.es#editar";
#elif L==5
	"MESSAGES.Domains#edit";
#elif L==6
	"MESSAGES.Domains#edit";
#elif L==7
	"MESSAGES.Domains#edit";
#elif L==8
	"MESSAGES.Domains#edit";
#elif L==9
	"MESSAGES.Domains#edit";
#endif

const char *Hlp_MESSAGES_Announcements =
#if   L==1
	"MESSAGES.Announcements";
#elif L==2
	"MESSAGES.Announcements";
#elif L==3
	"MESSAGES.Announcements";
#elif L==4
	"MESSAGES.Announcements.es";
#elif L==5
	"MESSAGES.Announcements";
#elif L==6
	"MESSAGES.Announcements";
#elif L==7
	"MESSAGES.Announcements";
#elif L==8
	"MESSAGES.Announcements";
#elif L==9
	"MESSAGES.Announcements";
#endif

const char *Hlp_MESSAGES_Notices =
#if   L==1
	"MESSAGES.Notices";
#elif L==2
	"MESSAGES.Notices";
#elif L==3
	"MESSAGES.Notices";
#elif L==4
	"MESSAGES.Notices.es";
#elif L==5
	"MESSAGES.Notices";
#elif L==6
	"MESSAGES.Notices";
#elif L==7
	"MESSAGES.Notices";
#elif L==8
	"MESSAGES.Notices";
#elif L==9
	"MESSAGES.Notices";
#endif

const char *Hlp_MESSAGES_Write =
#if   L==1
	"MESSAGES.Write";
#elif L==2
	"MESSAGES.Write";
#elif L==3
	"MESSAGES.Write";
#elif L==4
	"MESSAGES.Write.es";
#elif L==5
	"MESSAGES.Write";
#elif L==6
	"MESSAGES.Write";
#elif L==7
	"MESSAGES.Write";
#elif L==8
	"MESSAGES.Write";
#elif L==9
	"MESSAGES.Write";
#endif

const char *Hlp_MESSAGES_Received =
#if   L==1
	"MESSAGES.Received";
#elif L==2
	"MESSAGES.Received";
#elif L==3
	"MESSAGES.Received";
#elif L==4
	"MESSAGES.Received.es";
#elif L==5
	"MESSAGES.Received";
#elif L==6
	"MESSAGES.Received";
#elif L==7
	"MESSAGES.Received";
#elif L==8
	"MESSAGES.Received";
#elif L==9
	"MESSAGES.Received";
#endif

const char *Hlp_MESSAGES_Received_filter =
#if   L==1
	"MESSAGES.Received#filter";
#elif L==2
	"MESSAGES.Received#filter";
#elif L==3
	"MESSAGES.Received#filter";
#elif L==4
	"MESSAGES.Received.es#filtro";
#elif L==5
	"MESSAGES.Received#filter";
#elif L==6
	"MESSAGES.Received#filter";
#elif L==7
	"MESSAGES.Received#filter";
#elif L==8
	"MESSAGES.Received#filter";
#elif L==9
	"MESSAGES.Received#filter";
#endif

const char *Hlp_MESSAGES_Sent =
#if   L==1
	"MESSAGES.Sent";
#elif L==2
	"MESSAGES.Sent";
#elif L==3
	"MESSAGES.Sent";
#elif L==4
	"MESSAGES.Sent.es";
#elif L==5
	"MESSAGES.Sent";
#elif L==6
	"MESSAGES.Sent";
#elif L==7
	"MESSAGES.Sent";
#elif L==8
	"MESSAGES.Sent";
#elif L==9
	"MESSAGES.Sent";
#endif

const char *Hlp_MESSAGES_Sent_filter =
#if   L==1
	"MESSAGES.Sent#filter";
#elif L==2
	"MESSAGES.Sent#filter";
#elif L==3
	"MESSAGES.Sent#filter";
#elif L==4
	"MESSAGES.Sent.es#filtro";
#elif L==5
	"MESSAGES.Sent#filter";
#elif L==6
	"MESSAGES.Sent#filter";
#elif L==7
	"MESSAGES.Sent#filter";
#elif L==8
	"MESSAGES.Sent#filter";
#elif L==9
	"MESSAGES.Sent#filter";
#endif

const char *Hlp_MESSAGES_Email =
#if   L==1
	"MESSAGES.Email";
#elif L==2
	"MESSAGES.Email";
#elif L==3
	"MESSAGES.Email";
#elif L==4
	"MESSAGES.Email.es";
#elif L==5
	"MESSAGES.Email";
#elif L==6
	"MESSAGES.Email";
#elif L==7
	"MESSAGES.Email";
#elif L==8
	"MESSAGES.Email";
#elif L==9
	"MESSAGES.Email";
#endif

/***** STATS tab *****/

const char *Hlp_STATS_Surveys =
#if   L==1
	"STATS.Surveys";
#elif L==2
	"STATS.Surveys";
#elif L==3
	"STATS.Surveys";
#elif L==4
	"STATS.Surveys.es";
#elif L==5
	"STATS.Surveys";
#elif L==6
	"STATS.Surveys";
#elif L==7
	"STATS.Surveys";
#elif L==8
	"STATS.Surveys";
#elif L==9
	"STATS.Surveys";
#endif

const char *Hlp_STATS_Surveys_new_survey =
#if   L==1
	"STATS.Surveys#new-survey";
#elif L==2
	"STATS.Surveys#new-survey";
#elif L==3
	"STATS.Surveys#new-survey";
#elif L==4
	"STATS.Surveys.es#nueva-encuesta";
#elif L==5
	"STATS.Surveys#new-survey";
#elif L==6
	"STATS.Surveys#new-survey";
#elif L==7
	"STATS.Surveys#new-survey";
#elif L==8
	"STATS.Surveys#new-survey";
#elif L==9
	"STATS.Surveys#new-survey";
#endif

const char *Hlp_STATS_Surveys_edit_survey =
#if   L==1
	"STATS.Surveys#edit-survey";
#elif L==2
	"STATS.Surveys#edit-survey";
#elif L==3
	"STATS.Surveys#edit-survey";
#elif L==4
	"STATS.Surveys.es#editar-encuesta";
#elif L==5
	"STATS.Surveys#edit-survey";
#elif L==6
	"STATS.Surveys#edit-survey";
#elif L==7
	"STATS.Surveys#edit-survey";
#elif L==8
	"STATS.Surveys#edit-survey";
#elif L==9
	"STATS.Surveys#edit-survey";
#endif

const char *Hlp_STATS_Surveys_questions =
#if   L==1
	"STATS.Surveys#questions";
#elif L==2
	"STATS.Surveys#questions";
#elif L==3
	"STATS.Surveys#questions";
#elif L==4
	"STATS.Surveys.es#preguntas";
#elif L==5
	"STATS.Surveys#questions";
#elif L==6
	"STATS.Surveys#questions";
#elif L==7
	"STATS.Surveys#questions";
#elif L==8
	"STATS.Surveys#questions";
#elif L==9
	"STATS.Surveys#questions";
#endif

const char *Hlp_STATS_Figures =
#if   L==1
	"STATS.Figures";
#elif L==2
	"STATS.Figures";
#elif L==3
	"STATS.Figures";
#elif L==4
	"STATS.Figures.es";
#elif L==5
	"STATS.Figures";
#elif L==6
	"STATS.Figures";
#elif L==7
	"STATS.Figures";
#elif L==8
	"STATS.Figures";
#elif L==9
	"STATS.Figures";
#endif

const char *Hlp_STATS_Figures_users =
#if   L==1
	"STATS.Figures#users";
#elif L==2
	"STATS.Figures#users";
#elif L==3
	"STATS.Figures#users";
#elif L==4
	"STATS.Figures.es#usuarios";
#elif L==5
	"STATS.Figures#users";
#elif L==6
	"STATS.Figures#users";
#elif L==7
	"STATS.Figures#users";
#elif L==8
	"STATS.Figures#users";
#elif L==9
	"STATS.Figures#users";
#endif

const char *Hlp_STATS_Figures_ranking =
#if   L==1
	"STATS.Figures#ranking";
#elif L==2
	"STATS.Figures#ranking";
#elif L==3
	"STATS.Figures#ranking";
#elif L==4
	"STATS.Figures.es#ranking";
#elif L==5
	"STATS.Figures#ranking";
#elif L==6
	"STATS.Figures#ranking";
#elif L==7
	"STATS.Figures#ranking";
#elif L==8
	"STATS.Figures#ranking";
#elif L==9
	"STATS.Figures#ranking";
#endif

const char *Hlp_STATS_Figures_hierarchy =
#if   L==1
	"STATS.Figures#hierarchy";
#elif L==2
	"STATS.Figures#hierarchy";
#elif L==3
	"STATS.Figures#hierarchy";
#elif L==4
	"STATS.Figures.es#jerarqu%C3%ADa";
#elif L==5
	"STATS.Figures#hierarchy";
#elif L==6
	"STATS.Figures#hierarchy";
#elif L==7
	"STATS.Figures#hierarchy";
#elif L==8
	"STATS.Figures#hierarchy";
#elif L==9
	"STATS.Figures#hierarchy";
#endif

const char *Hlp_STATS_Figures_institutions =
#if   L==1
	"STATS.Figures#institutions";
#elif L==2
	"STATS.Figures#institutions";
#elif L==3
	"STATS.Figures#institutions";
#elif L==4
	"STATS.Figures.es#instituciones";
#elif L==5
	"STATS.Figures#institutions";
#elif L==6
	"STATS.Figures#institutions";
#elif L==7
	"STATS.Figures#institutions";
#elif L==8
	"STATS.Figures#institutions";
#elif L==9
	"STATS.Figures#institutions";
#endif

const char *Hlp_STATS_Figures_types_of_degree =
#if   L==1
	"STATS.Figures#types-of-degree";
#elif L==2
	"STATS.Figures#types-of-degree";
#elif L==3
	"STATS.Figures#types-of-degree";
#elif L==4
	"STATS.Figures.es#tipos-de-titulaci%C3%B3n";
#elif L==5
	"STATS.Figures#types-of-degree";
#elif L==6
	"STATS.Figures#types-of-degree";
#elif L==7
	"STATS.Figures#types-of-degree";
#elif L==8
	"STATS.Figures#types-of-degree";
#elif L==9
	"STATS.Figures#types-of-degree";
#endif

const char *Hlp_STATS_Figures_folders_and_files =
#if   L==1
	"STATS.Figures#folders-and-files";
#elif L==2
	"STATS.Figures#folders-and-files";
#elif L==3
	"STATS.Figures#folders-and-files";
#elif L==4
	"STATS.Figures.es#carpetas-y-archivos";
#elif L==5
	"STATS.Figures#folders-and-files";
#elif L==6
	"STATS.Figures#folders-and-files";
#elif L==7
	"STATS.Figures#folders-and-files";
#elif L==8
	"STATS.Figures#folders-and-files";
#elif L==9
	"STATS.Figures#folders-and-files";
#endif

const char *Hlp_STATS_Figures_open_educational_resources_oer =
#if   L==1
	"STATS.Figures#open-educational-resources-oer";
#elif L==2
	"STATS.Figures#open-educational-resources-oer";
#elif L==3
	"STATS.Figures#open-educational-resources-oer";
#elif L==4
	"STATS.Figures.es#recursos-educativos-abiertos-oer";
#elif L==5
	"STATS.Figures#open-educational-resources-oer";
#elif L==6
	"STATS.Figures#open-educational-resources-oer";
#elif L==7
	"STATS.Figures#open-educational-resources-oer";
#elif L==8
	"STATS.Figures#open-educational-resources-oer";
#elif L==9
	"STATS.Figures#open-educational-resources-oer";
#endif

const char *Hlp_STATS_Figures_assignments =
#if   L==1
	"STATS.Figures#assignments";
#elif L==2
	"STATS.Figures#assignments";
#elif L==3
	"STATS.Figures#assignments";
#elif L==4
	"STATS.Figures.es#actividades";
#elif L==5
	"STATS.Figures#assignments";
#elif L==6
	"STATS.Figures#assignments";
#elif L==7
	"STATS.Figures#assignments";
#elif L==8
	"STATS.Figures#assignments";
#elif L==9
	"STATS.Figures#assignments";
#endif

const char *Hlp_STATS_Figures_tests =
#if   L==1
	"STATS.Figures#tests";
#elif L==2
	"STATS.Figures#tests";
#elif L==3
	"STATS.Figures#tests";
#elif L==4
	"STATS.Figures.es#tests";
#elif L==5
	"STATS.Figures#tests";
#elif L==6
	"STATS.Figures#tests";
#elif L==7
	"STATS.Figures#tests";
#elif L==8
	"STATS.Figures#tests";
#elif L==9
	"STATS.Figures#tests";
#endif

const char *Hlp_STATS_Figures_timeline =
#if   L==1
	"STATS.Figures#timeline";
#elif L==2
	"STATS.Figures#timeline";
#elif L==3
	"STATS.Figures#timeline";
#elif L==4
	"STATS.Figures.es#timeline";
#elif L==5
	"STATS.Figures#timeline";
#elif L==6
	"STATS.Figures#timeline";
#elif L==7
	"STATS.Figures#timeline";
#elif L==8
	"STATS.Figures#timeline";
#elif L==9
	"STATS.Figures#timeline";
#endif

const char *Hlp_STATS_Figures_followed_followers =
#if   L==1
	"STATS.Figures#followed--followers";
#elif L==2
	"STATS.Figures#followed--followers";
#elif L==3
	"STATS.Figures#followed--followers";
#elif L==4
	"STATS.Figures.es#seguidos--seguidores";
#elif L==5
	"STATS.Figures#followed--followers";
#elif L==6
	"STATS.Figures#followed--followers";
#elif L==7
	"STATS.Figures#followed--followers";
#elif L==8
	"STATS.Figures#followed--followers";
#elif L==9
	"STATS.Figures#followed--followers";
#endif

const char *Hlp_STATS_Figures_forums =
#if   L==1
	"STATS.Figures#forums";
#elif L==2
	"STATS.Figures#forums";
#elif L==3
	"STATS.Figures#forums";
#elif L==4
	"STATS.Figures.es#foros";
#elif L==5
	"STATS.Figures#forums";
#elif L==6
	"STATS.Figures#forums";
#elif L==7
	"STATS.Figures#forums";
#elif L==8
	"STATS.Figures#forums";
#elif L==9
	"STATS.Figures#forums";
#endif

const char *Hlp_STATS_Figures_notifications =
#if   L==1
	"STATS.Figures#notifications";
#elif L==2
	"STATS.Figures#notifications";
#elif L==3
	"STATS.Figures#notifications";
#elif L==4
	"STATS.Figures.es#notificaciones";
#elif L==5
	"STATS.Figures#notifications";
#elif L==6
	"STATS.Figures#notifications";
#elif L==7
	"STATS.Figures#notifications";
#elif L==8
	"STATS.Figures#notifications";
#elif L==9
	"STATS.Figures#notifications";
#endif

const char *Hlp_STATS_Figures_notices =
#if   L==1
	"STATS.Figures#notices";
#elif L==2
	"STATS.Figures#notices";
#elif L==3
	"STATS.Figures#notices";
#elif L==4
	"STATS.Figures.es#avisos";
#elif L==5
	"STATS.Figures#notices";
#elif L==6
	"STATS.Figures#notices";
#elif L==7
	"STATS.Figures#notices";
#elif L==8
	"STATS.Figures#notices";
#elif L==9
	"STATS.Figures#notices";
#endif

const char *Hlp_STATS_Figures_messages =
#if   L==1
	"STATS.Figures#messages";
#elif L==2
	"STATS.Figures#messages";
#elif L==3
	"STATS.Figures#messages";
#elif L==4
	"STATS.Figures.es#mensajes";
#elif L==5
	"STATS.Figures#messages";
#elif L==6
	"STATS.Figures#messages";
#elif L==7
	"STATS.Figures#messages";
#elif L==8
	"STATS.Figures#messages";
#elif L==9
	"STATS.Figures#messages";
#endif

const char *Hlp_STATS_Figures_surveys =
#if   L==1
	"STATS.Figures#surveys";
#elif L==2
	"STATS.Figures#surveys";
#elif L==3
	"STATS.Figures#surveys";
#elif L==4
	"STATS.Figures.es#encuestas";
#elif L==5
	"STATS.Figures#surveys";
#elif L==6
	"STATS.Figures#surveys";
#elif L==7
	"STATS.Figures#surveys";
#elif L==8
	"STATS.Figures#surveys";
#elif L==9
	"STATS.Figures#surveys";
#endif

const char *Hlp_STATS_Figures_webs_social_networks =
#if   L==1
	"STATS.Figures#webs--social-networks";
#elif L==2
	"STATS.Figures#webs--social-networks";
#elif L==3
	"STATS.Figures#webs--social-networks";
#elif L==4
	"STATS.Figures.es#webs--redes-sociales";
#elif L==5
	"STATS.Figures#webs--social-networks";
#elif L==6
	"STATS.Figures#webs--social-networks";
#elif L==7
	"STATS.Figures#webs--social-networks";
#elif L==8
	"STATS.Figures#webs--social-networks";
#elif L==9
	"STATS.Figures#webs--social-networks";
#endif

const char *Hlp_STATS_Figures_language =
#if   L==1
	"STATS.Figures#language";
#elif L==2
	"STATS.Figures#language";
#elif L==3
	"STATS.Figures#language";
#elif L==4
	"STATS.Figures.es#idioma";
#elif L==5
	"STATS.Figures#language";
#elif L==6
	"STATS.Figures#language";
#elif L==7
	"STATS.Figures#language";
#elif L==8
	"STATS.Figures#language";
#elif L==9
	"STATS.Figures#language";
#endif

const char *Hlp_STATS_Figures_icons =
#if   L==1
	"STATS.Figures#icons";
#elif L==2
	"STATS.Figures#icons";
#elif L==3
	"STATS.Figures#icons";
#elif L==4
	"STATS.Figures.es#iconos";
#elif L==5
	"STATS.Figures#icons";
#elif L==6
	"STATS.Figures#icons";
#elif L==7
	"STATS.Figures#icons";
#elif L==8
	"STATS.Figures#icons";
#elif L==9
	"STATS.Figures#icons";
#endif

const char *Hlp_STATS_Figures_menu =
#if   L==1
	"STATS.Figures#menu";
#elif L==2
	"STATS.Figures#menu";
#elif L==3
	"STATS.Figures#menu";
#elif L==4
	"STATS.Figures.es#men%C3%BA";
#elif L==5
	"STATS.Figures#menu";
#elif L==6
	"STATS.Figures#menu";
#elif L==7
	"STATS.Figures#menu";
#elif L==8
	"STATS.Figures#menu";
#elif L==9
	"STATS.Figures#menu";
#endif

const char *Hlp_STATS_Figures_calendar =
#if   L==1
	"STATS.Figures#calendar";
#elif L==2
	"STATS.Figures#calendar";
#elif L==3
	"STATS.Figures#calendar";
#elif L==4
	"STATS.Figures.es#calendario";
#elif L==5
	"STATS.Figures#calendar";
#elif L==6
	"STATS.Figures#calendar";
#elif L==7
	"STATS.Figures#calendar";
#elif L==8
	"STATS.Figures#calendar";
#elif L==9
	"STATS.Figures#calendar";
#endif

const char *Hlp_STATS_Figures_theme =
#if   L==1
	"STATS.Figures#theme";
#elif L==2
	"STATS.Figures#theme";
#elif L==3
	"STATS.Figures#theme";
#elif L==4
	"STATS.Figures.es#tema";
#elif L==5
	"STATS.Figures#theme";
#elif L==6
	"STATS.Figures#theme";
#elif L==7
	"STATS.Figures#theme";
#elif L==8
	"STATS.Figures#theme";
#elif L==9
	"STATS.Figures#theme";
#endif

const char *Hlp_STATS_Figures_columns =
#if   L==1
	"STATS.Figures#columns";
#elif L==2
	"STATS.Figures#columns";
#elif L==3
	"STATS.Figures#columns";
#elif L==4
	"STATS.Figures.es#columnas";
#elif L==5
	"STATS.Figures#columns";
#elif L==6
	"STATS.Figures#columns";
#elif L==7
	"STATS.Figures#columns";
#elif L==8
	"STATS.Figures#columns";
#elif L==9
	"STATS.Figures#columns";
#endif

const char *Hlp_STATS_Figures_privacy =
#if   L==1
	"STATS.Figures#privacy";
#elif L==2
	"STATS.Figures#privacy";
#elif L==3
	"STATS.Figures#privacy";
#elif L==4
	"STATS.Figures.es#privacidad";
#elif L==5
	"STATS.Figures#privacy";
#elif L==6
	"STATS.Figures#privacy";
#elif L==7
	"STATS.Figures#privacy";
#elif L==8
	"STATS.Figures#privacy";
#elif L==9
	"STATS.Figures#privacy";
#endif

const char *Hlp_STATS_Degrees =
#if   L==1
	"STATS.Degrees";
#elif L==2
	"STATS.Degrees";
#elif L==3
	"STATS.Degrees";
#elif L==4
	"STATS.Degrees.es";
#elif L==5
	"STATS.Degrees";
#elif L==6
	"STATS.Degrees";
#elif L==7
	"STATS.Degrees";
#elif L==8
	"STATS.Degrees";
#elif L==9
	"STATS.Degrees";
#endif

const char *Hlp_STATS_Indicators =
#if   L==1
	"STATS.Indicators";
#elif L==2
	"STATS.Indicators";
#elif L==3
	"STATS.Indicators";
#elif L==4
	"STATS.Indicators.es";
#elif L==5
	"STATS.Indicators";
#elif L==6
	"STATS.Indicators";
#elif L==7
	"STATS.Indicators";
#elif L==8
	"STATS.Indicators";
#elif L==9
	"STATS.Indicators";
#endif

const char *Hlp_STATS_Visits_global_visits =
#if   L==1
	"STATS.Visits#global-visits";
#elif L==2
	"STATS.Visits#global-visits";
#elif L==3
	"STATS.Visits#global-visits";
#elif L==4
	"STATS.Visits.es#accesos-globales";
#elif L==5
	"STATS.Visits#global-visits";
#elif L==6
	"STATS.Visits#global-visits";
#elif L==7
	"STATS.Visits#global-visits";
#elif L==8
	"STATS.Visits#global-visits";
#elif L==9
	"STATS.Visits#global-visits";
#endif

const char *Hlp_STATS_Visits_visits_to_course =
#if   L==1
	"STATS.Visits#visits-to-course";
#elif L==2
	"STATS.Visits#visits-to-course";
#elif L==3
	"STATS.Visits#visits-to-course";
#elif L==4
	"STATS.Visits.es#accesos-a-la-asignatura";
#elif L==5
	"STATS.Visits#visits-to-course";
#elif L==6
	"STATS.Visits#visits-to-course";
#elif L==7
	"STATS.Visits#visits-to-course";
#elif L==8
	"STATS.Visits#visits-to-course";
#elif L==9
	"STATS.Visits#visits-to-course";
#endif

const char *Hlp_STATS_Report =
#if   L==1
	"STATS.Report";
#elif L==2
	"STATS.Report";
#elif L==3
	"STATS.Report";
#elif L==4
	"STATS.Report.es";
#elif L==5
	"STATS.Report";
#elif L==6
	"STATS.Report";
#elif L==7
	"STATS.Report";
#elif L==8
	"STATS.Report";
#elif L==9
	"STATS.Report";
#endif

const char *Hlp_STATS_Frequent =
#if   L==1
	"STATS.Frequent";
#elif L==2
	"STATS.Frequent";
#elif L==3
	"STATS.Frequent";
#elif L==4
	"STATS.Frequent.es";
#elif L==5
	"STATS.Frequent";
#elif L==6
	"STATS.Frequent";
#elif L==7
	"STATS.Frequent";
#elif L==8
	"STATS.Frequent";
#elif L==9
	"STATS.Frequent";
#endif

/***** PROFILE tab *****/

const char *Hlp_PROFILE_LogIn =
#if   L==1
	"PROFILE.LogIn";
#elif L==2
	"PROFILE.LogIn";
#elif L==3
	"PROFILE.LogIn";
#elif L==4
	"PROFILE.LogIn.es";
#elif L==5
	"PROFILE.LogIn";
#elif L==6
	"PROFILE.LogIn";
#elif L==7
	"PROFILE.LogIn";
#elif L==8
	"PROFILE.LogIn";
#elif L==9
	"PROFILE.LogIn";
#endif

const char *Hlp_PROFILE_SignUp =
#if   L==1
	"PROFILE.SignUp";
#elif L==2
	"PROFILE.SignUp";
#elif L==3
	"PROFILE.SignUp";
#elif L==4
	"PROFILE.SignUp.es";
#elif L==5
	"PROFILE.SignUp";
#elif L==6
	"PROFILE.SignUp";
#elif L==7
	"PROFILE.SignUp";
#elif L==8
	"PROFILE.SignUp";
#elif L==9
	"PROFILE.SignUp";
#endif

const char *Hlp_PROFILE_Account =
#if   L==1
	"PROFILE.Account";
#elif L==2
	"PROFILE.Account";
#elif L==3
	"PROFILE.Account";
#elif L==4
	"PROFILE.Account.es";
#elif L==5
	"PROFILE.Account";
#elif L==6
	"PROFILE.Account";
#elif L==7
	"PROFILE.Account";
#elif L==8
	"PROFILE.Account";
#elif L==9
	"PROFILE.Account";
#endif

const char *Hlp_PROFILE_Account_email =
#if   L==1
	"PROFILE.Account#email";
#elif L==2
	"PROFILE.Account#email";
#elif L==3
	"PROFILE.Account#email";
#elif L==4
	"PROFILE.Account.es#correo";
#elif L==5
	"PROFILE.Account#email";
#elif L==6
	"PROFILE.Account#email";
#elif L==7
	"PROFILE.Account#email";
#elif L==8
	"PROFILE.Account#email";
#elif L==9
	"PROFILE.Account#email";
#endif

const char *Hlp_PROFILE_Session_role =
#if   L==1
	"PROFILE.Session#role";
#elif L==2
	"PROFILE.Session#role";
#elif L==3
	"PROFILE.Session#role";
#elif L==4
	"PROFILE.Session.es#rol";
#elif L==5
	"PROFILE.Session#role";
#elif L==6
	"PROFILE.Session#role";
#elif L==7
	"PROFILE.Session#role";
#elif L==8
	"PROFILE.Session#role";
#elif L==9
	"PROFILE.Session#role";
#endif

const char *Hlp_PROFILE_Password =
#if   L==1
	"PROFILE.Password";
#elif L==2
	"PROFILE.Password";
#elif L==3
	"PROFILE.Password";
#elif L==4
	"PROFILE.Password.es";
#elif L==5
	"PROFILE.Password";
#elif L==6
	"PROFILE.Password";
#elif L==7
	"PROFILE.Password";
#elif L==8
	"PROFILE.Password";
#elif L==9
	"PROFILE.Password";
#endif

const char *Hlp_PROFILE_Courses =
#if   L==1
	"PROFILE.Courses";
#elif L==2
	"PROFILE.Courses";
#elif L==3
	"PROFILE.Courses";
#elif L==4
	"PROFILE.Courses.es";
#elif L==5
	"PROFILE.Courses";
#elif L==6
	"PROFILE.Courses";
#elif L==7
	"PROFILE.Courses";
#elif L==8
	"PROFILE.Courses";
#elif L==9
	"PROFILE.Courses";
#endif

const char *Hlp_PROFILE_Timetable =
#if   L==1
	"PROFILE.Timetable";
#elif L==2
	"PROFILE.Timetable";
#elif L==3
	"PROFILE.Timetable";
#elif L==4
	"PROFILE.Timetable.es";
#elif L==5
	"PROFILE.Timetable";
#elif L==6
	"PROFILE.Timetable";
#elif L==7
	"PROFILE.Timetable";
#elif L==8
	"PROFILE.Timetable";
#elif L==9
	"PROFILE.Timetable";
#endif

const char *Hlp_PROFILE_Agenda =
#if   L==1
	"PROFILE.Agenda";
#elif L==2
	"PROFILE.Agenda";
#elif L==3
	"PROFILE.Agenda";
#elif L==4
	"PROFILE.Agenda.es";
#elif L==5
	"PROFILE.Agenda";
#elif L==6
	"PROFILE.Agenda";
#elif L==7
	"PROFILE.Agenda";
#elif L==8
	"PROFILE.Agenda";
#elif L==9
	"PROFILE.Agenda";
#endif

const char *Hlp_PROFILE_Agenda_new_event =
#if   L==1
	"PROFILE.Agenda#new-event";
#elif L==2
	"PROFILE.Agenda#new-event";
#elif L==3
	"PROFILE.Agenda#new-event";
#elif L==4
	"PROFILE.Agenda.es#nuevo-evento";
#elif L==5
	"PROFILE.Agenda#new-event";
#elif L==6
	"PROFILE.Agenda#new-event";
#elif L==7
	"PROFILE.Agenda#new-event";
#elif L==8
	"PROFILE.Agenda#new-event";
#elif L==9
	"PROFILE.Agenda#new-event";
#endif

const char *Hlp_PROFILE_Agenda_edit_event =
#if   L==1
	"PROFILE.Agenda#edit-event";
#elif L==2
	"PROFILE.Agenda#edit-event";
#elif L==3
	"PROFILE.Agenda#edit-event";
#elif L==4
	"PROFILE.Agenda.es#editar-evento";
#elif L==5
	"PROFILE.Agenda#edit-event";
#elif L==6
	"PROFILE.Agenda#edit-event";
#elif L==7
	"PROFILE.Agenda#edit-event";
#elif L==8
	"PROFILE.Agenda#edit-event";
#elif L==9
	"PROFILE.Agenda#edit-event";
#endif

const char *Hlp_PROFILE_Agenda_public_agenda =
#if   L==1
	"PROFILE.Agenda#public-agenda";
#elif L==2
	"PROFILE.Agenda#public-agenda";
#elif L==3
	"PROFILE.Agenda#public-agenda";
#elif L==4
	"PROFILE.Agenda.es#agenda-p%C3%BAblica";
#elif L==5
	"PROFILE.Agenda#public-agenda";
#elif L==6
	"PROFILE.Agenda#public-agenda";
#elif L==7
	"PROFILE.Agenda#public-agenda";
#elif L==8
	"PROFILE.Agenda#public-agenda";
#elif L==9
	"PROFILE.Agenda#public-agenda";
#endif

const char *Hlp_PROFILE_Record =
#if   L==1
	"PROFILE.Record";
#elif L==2
	"PROFILE.Record";
#elif L==3
	"PROFILE.Record";
#elif L==4
	"PROFILE.Record.es";
#elif L==5
	"PROFILE.Record";
#elif L==6
	"PROFILE.Record";
#elif L==7
	"PROFILE.Record";
#elif L==8
	"PROFILE.Record";
#elif L==9
	"PROFILE.Record";
#endif

const char *Hlp_PROFILE_Institution =
#if   L==1
	"PROFILE.Institution";
#elif L==2
	"PROFILE.Institution";
#elif L==3
	"PROFILE.Institution";
#elif L==4
	"PROFILE.Institution.es";
#elif L==5
	"PROFILE.Institution";
#elif L==6
	"PROFILE.Institution";
#elif L==7
	"PROFILE.Institution";
#elif L==8
	"PROFILE.Institution";
#elif L==9
	"PROFILE.Institution";
#endif

const char *Hlp_PROFILE_Webs =
#if   L==1
	"PROFILE.Webs";
#elif L==2
	"PROFILE.Webs";
#elif L==3
	"PROFILE.Webs";
#elif L==4
	"PROFILE.Webs.es";
#elif L==5
	"PROFILE.Webs";
#elif L==6
	"PROFILE.Webs";
#elif L==7
	"PROFILE.Webs";
#elif L==8
	"PROFILE.Webs";
#elif L==9
	"PROFILE.Webs";
#endif

const char *Hlp_PROFILE_Photo =
#if   L==1
	"PROFILE.Photo";
#elif L==2
	"PROFILE.Photo";
#elif L==3
	"PROFILE.Photo";
#elif L==4
	"PROFILE.Photo.es";
#elif L==5
	"PROFILE.Photo";
#elif L==6
	"PROFILE.Photo";
#elif L==7
	"PROFILE.Photo";
#elif L==8
	"PROFILE.Photo";
#elif L==9
	"PROFILE.Photo";
#endif

const char *Hlp_PROFILE_Preferences_language =
#if   L==1
	"PROFILE.Preferences#language";
#elif L==2
	"PROFILE.Preferences#language";
#elif L==3
	"PROFILE.Preferences#language";
#elif L==4
	"PROFILE.Preferences.es#idioma";
#elif L==5
	"PROFILE.Preferences#language";
#elif L==6
	"PROFILE.Preferences#language";
#elif L==7
	"PROFILE.Preferences#language";
#elif L==8
	"PROFILE.Preferences#language";
#elif L==9
	"PROFILE.Preferences#language";
#endif

const char *Hlp_PROFILE_Preferences_calendar =
#if   L==1
	"PROFILE.Preferences#calendar";
#elif L==2
	"PROFILE.Preferences#calendar";
#elif L==3
	"PROFILE.Preferences#calendar";
#elif L==4
	"PROFILE.Preferences.es#calendario";
#elif L==5
	"PROFILE.Preferences#calendar";
#elif L==6
	"PROFILE.Preferences#calendar";
#elif L==7
	"PROFILE.Preferences#calendar";
#elif L==8
	"PROFILE.Preferences#calendar";
#elif L==9
	"PROFILE.Preferences#calendar";
#endif

const char *Hlp_PROFILE_Preferences_icons =
#if   L==1
	"PROFILE.Preferences#icons";
#elif L==2
	"PROFILE.Preferences#icons";
#elif L==3
	"PROFILE.Preferences#icons";
#elif L==4
	"PROFILE.Preferences.es#iconos";
#elif L==5
	"PROFILE.Preferences#icons";
#elif L==6
	"PROFILE.Preferences#icons";
#elif L==7
	"PROFILE.Preferences#icons";
#elif L==8
	"PROFILE.Preferences#icons";
#elif L==9
	"PROFILE.Preferences#icons";
#endif

const char *Hlp_PROFILE_Preferences_theme =
#if   L==1
	"PROFILE.Preferences#theme";
#elif L==2
	"PROFILE.Preferences#theme";
#elif L==3
	"PROFILE.Preferences#theme";
#elif L==4
	"PROFILE.Preferences.es#tema";
#elif L==5
	"PROFILE.Preferences#theme";
#elif L==6
	"PROFILE.Preferences#theme";
#elif L==7
	"PROFILE.Preferences#theme";
#elif L==8
	"PROFILE.Preferences#theme";
#elif L==9
	"PROFILE.Preferences#theme";
#endif

const char *Hlp_PROFILE_Preferences_menu =
#if   L==1
	"PROFILE.Preferences#menu";
#elif L==2
	"PROFILE.Preferences#menu";
#elif L==3
	"PROFILE.Preferences#menu";
#elif L==4
	"PROFILE.Preferences.es#men%C3%BA";
#elif L==5
	"PROFILE.Preferences#menu";
#elif L==6
	"PROFILE.Preferences#menu";
#elif L==7
	"PROFILE.Preferences#menu";
#elif L==8
	"PROFILE.Preferences#menu";
#elif L==9
	"PROFILE.Preferences#menu";
#endif

const char *Hlp_PROFILE_Preferences_columns =
#if   L==1
	"PROFILE.Preferences#columns";
#elif L==2
	"PROFILE.Preferences#columns";
#elif L==3
	"PROFILE.Preferences#columns";
#elif L==4
	"PROFILE.Preferences.es#columnas";
#elif L==5
	"PROFILE.Preferences#columns";
#elif L==6
	"PROFILE.Preferences#columns";
#elif L==7
	"PROFILE.Preferences#columns";
#elif L==8
	"PROFILE.Preferences#columns";
#elif L==9
	"PROFILE.Preferences#columns";
#endif

const char *Hlp_PROFILE_Preferences_privacy =
#if   L==1
	"PROFILE.Preferences#privacy";
#elif L==2
	"PROFILE.Preferences#privacy";
#elif L==3
	"PROFILE.Preferences#privacy";
#elif L==4
	"PROFILE.Preferences.es#privacidad";
#elif L==5
	"PROFILE.Preferences#privacy";
#elif L==6
	"PROFILE.Preferences#privacy";
#elif L==7
	"PROFILE.Preferences#privacy";
#elif L==8
	"PROFILE.Preferences#privacy";
#elif L==9
	"PROFILE.Preferences#privacy";
#endif

const char *Hlp_PROFILE_Preferences_notifications =
#if   L==1
	"PROFILE.Preferences#notifications";
#elif L==2
	"PROFILE.Preferences#notifications";
#elif L==3
	"PROFILE.Preferences#notifications";
#elif L==4
	"PROFILE.Preferences.es#notificaciones";
#elif L==5
	"PROFILE.Preferences#notifications";
#elif L==6
	"PROFILE.Preferences#notifications";
#elif L==7
	"PROFILE.Preferences#notifications";
#elif L==8
	"PROFILE.Preferences#notifications";
#elif L==9
	"PROFILE.Preferences#notifications";
#endif

const char *Hlp_PROFILE_Briefcase =
#if   L==1
	"PROFILE.Briefcase";
#elif L==2
	"PROFILE.Briefcase";
#elif L==3
	"PROFILE.Briefcase";
#elif L==4
	"PROFILE.Briefcase.es";
#elif L==5
	"PROFILE.Briefcase";
#elif L==6
	"PROFILE.Briefcase";
#elif L==7
	"PROFILE.Briefcase";
#elif L==8
	"PROFILE.Briefcase";
#elif L==9
	"PROFILE.Briefcase";
#endif
