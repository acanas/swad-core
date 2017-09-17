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
	"Search.es";
#elif L==2
	"Search.en";
#elif L==3
	"Search.en";
#elif L==4
	"Search.es";
#elif L==5
	"Search.en";
#elif L==6
	"Search.es";
#elif L==7
	"Search.en";
#elif L==8
	"Search.en";
#elif L==9
	"Search.en";
#endif

const char *Hlp_Calendar =
#if   L==1
	"Calendar.es";
#elif L==2
	"Calendar.en";
#elif L==3
	"Calendar.en";
#elif L==4
	"Calendar.es";
#elif L==5
	"Calendar.en";
#elif L==6
	"Calendar.es";
#elif L==7
	"Calendar.en";
#elif L==8
	"Calendar.en";
#elif L==9
	"Calendar.en";
#endif

/***** SYSTEM tab *****/

const char *Hlp_SYSTEM_Countries =
#if   L==1
	"SYSTEM.Countries.es";
#elif L==2
	"SYSTEM.Countries.en";
#elif L==3
	"SYSTEM.Countries.en";
#elif L==4
	"SYSTEM.Countries.es";
#elif L==5
	"SYSTEM.Countries.en";
#elif L==6
	"SYSTEM.Countries.es";
#elif L==7
	"SYSTEM.Countries.en";
#elif L==8
	"SYSTEM.Countries.en";
#elif L==9
	"SYSTEM.Countries.en";
#endif

const char *Hlp_SYSTEM_Hierarchy_pending =
#if   L==1
	"SYSTEM.Hierarchy.es#pendientes";
#elif L==2
	"SYSTEM.Hierarchy.en#pending";
#elif L==3
	"SYSTEM.Hierarchy.en#pending";
#elif L==4
	"SYSTEM.Hierarchy.es#pendientes";
#elif L==5
	"SYSTEM.Hierarchy.en#pending";
#elif L==6
	"SYSTEM.Hierarchy.es#pendientes";
#elif L==7
	"SYSTEM.Hierarchy.en#pending";
#elif L==8
	"SYSTEM.Hierarchy.en#pending";
#elif L==9
	"SYSTEM.Hierarchy.en#pending";
#endif

const char *Hlp_SYSTEM_Hierarchy_eliminate_old_courses =
#if   L==1
	"SYSTEM.Hierarchy.es#eliminar-asignaturas-antiguas";
#elif L==2
	"SYSTEM.Hierarchy.en#eliminate-old-courses";
#elif L==3
	"SYSTEM.Hierarchy.en#eliminate-old-courses";
#elif L==4
	"SYSTEM.Hierarchy.es#eliminar-asignaturas-antiguas";
#elif L==5
	"SYSTEM.Hierarchy.en#eliminate-old-courses";
#elif L==6
	"SYSTEM.Hierarchy.es#eliminar-asignaturas-antiguas";
#elif L==7
	"SYSTEM.Hierarchy.en#eliminate-old-courses";
#elif L==8
	"SYSTEM.Hierarchy.en#eliminate-old-courses";
#elif L==9
	"SYSTEM.Hierarchy.en#eliminate-old-courses";
#endif

const char *Hlp_SYSTEM_Banners =
#if   L==1
	"SYSTEM.Banners.es";
#elif L==2
	"SYSTEM.Banners.en";
#elif L==3
	"SYSTEM.Banners.en";
#elif L==4
	"SYSTEM.Banners.es";
#elif L==5
	"SYSTEM.Banners.en";
#elif L==6
	"SYSTEM.Banners.es";
#elif L==7
	"SYSTEM.Banners.en";
#elif L==8
	"SYSTEM.Banners.en";
#elif L==9
	"SYSTEM.Banners.en";
#endif

const char *Hlp_SYSTEM_Banners_edit =
#if   L==1
	"SYSTEM.Banners.es#editar";
#elif L==2
	"SYSTEM.Banners.en#edit";
#elif L==3
	"SYSTEM.Banners.en#edit";
#elif L==4
	"SYSTEM.Banners.es#editar";
#elif L==5
	"SYSTEM.Banners.en#edit";
#elif L==6
	"SYSTEM.Banners.es#editar";
#elif L==7
	"SYSTEM.Banners.en#edit";
#elif L==8
	"SYSTEM.Banners.en#edit";
#elif L==9
	"SYSTEM.Banners.en#edit";
#endif

const char *Hlp_SYSTEM_Links =
#if   L==1
	"SYSTEM.Links.es";
#elif L==2
	"SYSTEM.Links.en";
#elif L==3
	"SYSTEM.Links.en";
#elif L==4
	"SYSTEM.Links.es";
#elif L==5
	"SYSTEM.Links.en";
#elif L==6
	"SYSTEM.Links.es";
#elif L==7
	"SYSTEM.Links.en";
#elif L==8
	"SYSTEM.Links.en";
#elif L==9
	"SYSTEM.Links.en";
#endif

const char *Hlp_SYSTEM_Links_edit =
#if   L==1
	"SYSTEM.Links.es#editar";
#elif L==2
	"SYSTEM.Links.en#edit";
#elif L==3
	"SYSTEM.Links.en#edit";
#elif L==4
	"SYSTEM.Links.es#editar";
#elif L==5
	"SYSTEM.Links.en#edit";
#elif L==6
	"SYSTEM.Links.es#editar";
#elif L==7
	"SYSTEM.Links.en#edit";
#elif L==8
	"SYSTEM.Links.en#edit";
#elif L==9
	"SYSTEM.Links.en#edit";
#endif

/***** COUNTRY tab *****/

const char *Hlp_COUNTRY_Information =
#if   L==1
	"COUNTRY.Information.es";
#elif L==2
	"COUNTRY.Information.en";
#elif L==3
	"COUNTRY.Information.en";
#elif L==4
	"COUNTRY.Information.es";
#elif L==5
	"COUNTRY.Information.en";
#elif L==6
	"COUNTRY.Information.es";
#elif L==7
	"COUNTRY.Information.en";
#elif L==8
	"COUNTRY.Information.en";
#elif L==9
	"COUNTRY.Information.en";
#endif

const char *Hlp_COUNTRY_Institutions =
#if   L==1
	"COUNTRY.Institutions.es";
#elif L==2
	"COUNTRY.Institutions.en";
#elif L==3
	"COUNTRY.Institutions.en";
#elif L==4
	"COUNTRY.Institutions.es";
#elif L==5
	"COUNTRY.Institutions.en";
#elif L==6
	"COUNTRY.Institutions.es";
#elif L==7
	"COUNTRY.Institutions.en";
#elif L==8
	"COUNTRY.Institutions.en";
#elif L==9
	"COUNTRY.Institutions.en";
#endif

/***** INSTITUTION tab *****/

const char *Hlp_INSTITUTION_Information =
#if   L==1
	"INSTITUTION.Information.es";
#elif L==2
	"INSTITUTION.Information.en";
#elif L==3
	"INSTITUTION.Information.en";
#elif L==4
	"INSTITUTION.Information.es";
#elif L==5
	"INSTITUTION.Information.en";
#elif L==6
	"INSTITUTION.Information.es";
#elif L==7
	"INSTITUTION.Information.en";
#elif L==8
	"INSTITUTION.Information.en";
#elif L==9
	"INSTITUTION.Information.en";
#endif

const char *Hlp_INSTITUTION_Centres =
#if   L==1
	"INSTITUTION.Centres.es";
#elif L==2
	"INSTITUTION.Centres.en";
#elif L==3
	"INSTITUTION.Centres.en";
#elif L==4
	"INSTITUTION.Centres.es";
#elif L==5
	"INSTITUTION.Centres.en";
#elif L==6
	"INSTITUTION.Centres.es";
#elif L==7
	"INSTITUTION.Centres.en";
#elif L==8
	"INSTITUTION.Centres.en";
#elif L==9
	"INSTITUTION.Centres.en";
#endif

const char *Hlp_INSTITUTION_Departments =
#if   L==1
	"INSTITUTION.Departments.es";
#elif L==2
	"INSTITUTION.Departments.en";
#elif L==3
	"INSTITUTION.Departments.en";
#elif L==4
	"INSTITUTION.Departments.es";
#elif L==5
	"INSTITUTION.Departments.en";
#elif L==6
	"INSTITUTION.Departments.es";
#elif L==7
	"INSTITUTION.Departments.en";
#elif L==8
	"INSTITUTION.Departments.en";
#elif L==9
	"INSTITUTION.Departments.en";
#endif

const char *Hlp_INSTITUTION_Departments_edit =
#if   L==1
	"INSTITUTION.Departments.es#editar";
#elif L==2
	"INSTITUTION.Departments.en#edit";
#elif L==3
	"INSTITUTION.Departments.en#edit";
#elif L==4
	"INSTITUTION.Departments.es#editar";
#elif L==5
	"INSTITUTION.Departments.en#edit";
#elif L==6
	"INSTITUTION.Departments.es#editar";
#elif L==7
	"INSTITUTION.Departments.en#edit";
#elif L==8
	"INSTITUTION.Departments.en#edit";
#elif L==9
	"INSTITUTION.Departments.en#edit";
#endif

const char *Hlp_INSTITUTION_Places =
#if   L==1
	"INSTITUTION.Places.es";
#elif L==2
	"INSTITUTION.Places.en";
#elif L==3
	"INSTITUTION.Places.en";
#elif L==4
	"INSTITUTION.Places.es";
#elif L==5
	"INSTITUTION.Places.en";
#elif L==6
	"INSTITUTION.Places.es";
#elif L==7
	"INSTITUTION.Places.en";
#elif L==8
	"INSTITUTION.Places.en";
#elif L==9
	"INSTITUTION.Places.en";
#endif

const char *Hlp_INSTITUTION_Places_edit =
#if   L==1
	"INSTITUTION.Places.es#editar";
#elif L==2
	"INSTITUTION.Places.en#edit";
#elif L==3
	"INSTITUTION.Places.en#edit";
#elif L==4
	"INSTITUTION.Places.es#editar";
#elif L==5
	"INSTITUTION.Places.en#edit";
#elif L==6
	"INSTITUTION.Places.es#editar";
#elif L==7
	"INSTITUTION.Places.en#edit";
#elif L==8
	"INSTITUTION.Places.en#edit";
#elif L==9
	"INSTITUTION.Places.en#edit";
#endif

const char *Hlp_INSTITUTION_Holidays =
#if   L==1
	"INSTITUTION.Holidays.es";
#elif L==2
	"INSTITUTION.Holidays.en";
#elif L==3
	"INSTITUTION.Holidays.en";
#elif L==4
	"INSTITUTION.Holidays.es";
#elif L==5
	"INSTITUTION.Holidays.en";
#elif L==6
	"INSTITUTION.Holidays.es";
#elif L==7
	"INSTITUTION.Holidays.en";
#elif L==8
	"INSTITUTION.Holidays.en";
#elif L==9
	"INSTITUTION.Holidays.en";
#endif

const char *Hlp_INSTITUTION_Holidays_edit =
#if   L==1
	"INSTITUTION.Holidays.es#editar";
#elif L==2
	"INSTITUTION.Holidays.en#edit";
#elif L==3
	"INSTITUTION.Holidays.en#edit";
#elif L==4
	"INSTITUTION.Holidays.es#editar";
#elif L==5
	"INSTITUTION.Holidays.en#edit";
#elif L==6
	"INSTITUTION.Holidays.es#editar";
#elif L==7
	"INSTITUTION.Holidays.en#edit";
#elif L==8
	"INSTITUTION.Holidays.en#edit";
#elif L==9
	"INSTITUTION.Holidays.en#edit";
#endif

/***** CENTRE tab *****/

const char *Hlp_CENTRE_Information =
#if   L==1
	"CENTRE.Information.es";
#elif L==2
	"CENTRE.Information.en";
#elif L==3
	"CENTRE.Information.en";
#elif L==4
	"CENTRE.Information.es";
#elif L==5
	"CENTRE.Information.en";
#elif L==6
	"CENTRE.Information.es";
#elif L==7
	"CENTRE.Information.en";
#elif L==8
	"CENTRE.Information.en";
#elif L==9
	"CENTRE.Information.en";
#endif

const char *Hlp_CENTRE_DegreeTypes =
#if   L==1
	"CENTRE.DegreeTypes.es";
#elif L==2
	"CENTRE.DegreeTypes.en";
#elif L==3
	"CENTRE.DegreeTypes.en";
#elif L==4
	"CENTRE.DegreeTypes.es";
#elif L==5
	"CENTRE.DegreeTypes.en";
#elif L==6
	"CENTRE.DegreeTypes.es";
#elif L==7
	"CENTRE.DegreeTypes.en";
#elif L==8
	"CENTRE.DegreeTypes.en";
#elif L==9
	"CENTRE.DegreeTypes.en";
#endif

const char *Hlp_CENTRE_DegreeTypes_edit =
#if   L==1
	"CENTRE.DegreeTypes.es#editar";
#elif L==2
	"CENTRE.DegreeTypes.en#edit";
#elif L==3
	"CENTRE.DegreeTypes.en#edit";
#elif L==4
	"CENTRE.DegreeTypes.es#editar";
#elif L==5
	"CENTRE.DegreeTypes.en#edit";
#elif L==6
	"CENTRE.DegreeTypes.es#editar";
#elif L==7
	"CENTRE.DegreeTypes.en#edit";
#elif L==8
	"CENTRE.DegreeTypes.en#edit";
#elif L==9
	"CENTRE.DegreeTypes.en#edit";
#endif

const char *Hlp_CENTRE_Degrees =
#if   L==1
	"CENTRE.Degrees.es";
#elif L==2
	"CENTRE.Degrees.en";
#elif L==3
	"CENTRE.Degrees.en";
#elif L==4
	"CENTRE.Degrees.es";
#elif L==5
	"CENTRE.Degrees.en";
#elif L==6
	"CENTRE.Degrees.es";
#elif L==7
	"CENTRE.Degrees.en";
#elif L==8
	"CENTRE.Degrees.en";
#elif L==9
	"CENTRE.Degrees.en";
#endif

/***** DEGREE tab *****/

const char *Hlp_DEGREE_Information =
#if   L==1
	"DEGREE.Information.es";
#elif L==2
	"DEGREE.Information.en";
#elif L==3
	"DEGREE.Information.en";
#elif L==4
	"DEGREE.Information.es";
#elif L==5
	"DEGREE.Information.en";
#elif L==6
	"DEGREE.Information.es";
#elif L==7
	"DEGREE.Information.en";
#elif L==8
	"DEGREE.Information.en";
#elif L==9
	"DEGREE.Information.en";
#endif

const char *Hlp_DEGREE_Courses =
#if   L==1
	"DEGREE.Courses.es";
#elif L==2
	"DEGREE.Courses.en";
#elif L==3
	"DEGREE.Courses.en";
#elif L==4
	"DEGREE.Courses.es";
#elif L==5
	"DEGREE.Courses.en";
#elif L==6
	"DEGREE.Courses.es";
#elif L==7
	"DEGREE.Courses.en";
#elif L==8
	"DEGREE.Courses.en";
#elif L==9
	"DEGREE.Courses.en";
#endif

/***** COURSE tab *****/

const char *Hlp_COURSE_Information =
#if   L==1
	"COURSE.Information.es";
#elif L==2
	"COURSE.Information.en";
#elif L==3
	"COURSE.Information.en";
#elif L==4
	"COURSE.Information.es";
#elif L==5
	"COURSE.Information.en";
#elif L==6
	"COURSE.Information.es";
#elif L==7
	"COURSE.Information.en";
#elif L==8
	"COURSE.Information.en";
#elif L==9
	"COURSE.Information.en";
#endif

const char *Hlp_COURSE_Information_textual_information =
#if   L==1
	"COURSE.Information.es##informaci%C3%B3n-textual";
#elif L==2
	"COURSE.Information.en#textual-information";
#elif L==3
	"COURSE.Information.en#textual-information";
#elif L==4
	"COURSE.Information.es##informaci%C3%B3n-textual";
#elif L==5
	"COURSE.Information.en#textual-information";
#elif L==6
	"COURSE.Information.es##informaci%C3%B3n-textual";
#elif L==7
	"COURSE.Information.en#textual-information";
#elif L==8
	"COURSE.Information.en#textual-information";
#elif L==9
	"COURSE.Information.en#textual-information";
#endif

const char *Hlp_COURSE_Information_edit =
#if   L==1
	"COURSE.Information.es#editar";
#elif L==2
	"COURSE.Information.en#edit";
#elif L==3
	"COURSE.Information.en#edit";
#elif L==4
	"COURSE.Information.es#editar";
#elif L==5
	"COURSE.Information.en#edit";
#elif L==6
	"COURSE.Information.es#editar";
#elif L==7
	"COURSE.Information.en#edit";
#elif L==8
	"COURSE.Information.en#edit";
#elif L==9
	"COURSE.Information.en#edit";
#endif

const char *Hlp_COURSE_Guide =
#if   L==1
	"COURSE.Guide.es";
#elif L==2
	"COURSE.Guide.en";
#elif L==3
	"COURSE.Guide.en";
#elif L==4
	"COURSE.Guide.es";
#elif L==5
	"COURSE.Guide.en";
#elif L==6
	"COURSE.Guide.es";
#elif L==7
	"COURSE.Guide.en";
#elif L==8
	"COURSE.Guide.en";
#elif L==9
	"COURSE.Guide.en";
#endif

const char *Hlp_COURSE_Guide_edit =
#if   L==1
	"COURSE.Guide.es#editar";
#elif L==2
	"COURSE.Guide.en#edit";
#elif L==3
	"COURSE.Guide.en#edit";
#elif L==4
	"COURSE.Guide.es#editar";
#elif L==5
	"COURSE.Guide.en#edit";
#elif L==6
	"COURSE.Guide.es#editar";
#elif L==7
	"COURSE.Guide.en#edit";
#elif L==8
	"COURSE.Guide.en#edit";
#elif L==9
	"COURSE.Guide.en#edit";
#endif

const char *Hlp_COURSE_Syllabus =
#if   L==1
	"COURSE.Syllabus.es";
#elif L==2
	"COURSE.Syllabus.en";
#elif L==3
	"COURSE.Syllabus.en";
#elif L==4
	"COURSE.Syllabus.es";
#elif L==5
	"COURSE.Syllabus.en";
#elif L==6
	"COURSE.Syllabus.es";
#elif L==7
	"COURSE.Syllabus.en";
#elif L==8
	"COURSE.Syllabus.en";
#elif L==9
	"COURSE.Syllabus.en";
#endif

const char *Hlp_COURSE_Syllabus_edit =
#if   L==1
	"COURSE.Syllabus.es#editar";
#elif L==2
	"COURSE.Syllabus.en#edit";
#elif L==3
	"COURSE.Syllabus.en#edit";
#elif L==4
	"COURSE.Syllabus.es#editar";
#elif L==5
	"COURSE.Syllabus.en#edit";
#elif L==6
	"COURSE.Syllabus.es#editar";
#elif L==7
	"COURSE.Syllabus.en#edit";
#elif L==8
	"COURSE.Syllabus.en#edit";
#elif L==9
	"COURSE.Syllabus.en#edit";
#endif

const char *Hlp_COURSE_Timetable =
#if   L==1
	"COURSE.Timetable.en";
#elif L==2
	"COURSE.Timetable.es";
#elif L==3
	"COURSE.Timetable.en";
#elif L==4
	"COURSE.Timetable.es";
#elif L==5
	"COURSE.Timetable.en";
#elif L==6
	"COURSE.Timetable.es";
#elif L==7
	"COURSE.Timetable.en";
#elif L==8
	"COURSE.Timetable.en";
#elif L==9
	"COURSE.Timetable.en";
#endif

const char *Hlp_COURSE_Bibliography =
#if   L==1
	"COURSE.Bibliography.es";
#elif L==2
	"COURSE.Bibliography.en";
#elif L==3
	"COURSE.Bibliography.en";
#elif L==4
	"COURSE.Bibliography.es";
#elif L==5
	"COURSE.Bibliography.en";
#elif L==6
	"COURSE.Bibliography.es";
#elif L==7
	"COURSE.Bibliography.en";
#elif L==8
	"COURSE.Bibliography.en";
#elif L==9
	"COURSE.Bibliography.en";
#endif

const char *Hlp_COURSE_Bibliography_edit =
#if   L==1
	"COURSE.Bibliography.es#editar";
#elif L==2
	"COURSE.Bibliography.en#edit";
#elif L==3
	"COURSE.Bibliography.en#edit";
#elif L==4
	"COURSE.Bibliography.es#editar";
#elif L==5
	"COURSE.Bibliography.en#edit";
#elif L==6
	"COURSE.Bibliography.es#editar";
#elif L==7
	"COURSE.Bibliography.en#edit";
#elif L==8
	"COURSE.Bibliography.en#edit";
#elif L==9
	"COURSE.Bibliography.en#edit";
#endif

const char *Hlp_COURSE_FAQ =
#if   L==1
	"COURSE.FAQ.es";
#elif L==2
	"COURSE.FAQ.en";
#elif L==3
	"COURSE.FAQ.en";
#elif L==4
	"COURSE.FAQ.es";
#elif L==5
	"COURSE.FAQ.en";
#elif L==6
	"COURSE.FAQ.es";
#elif L==7
	"COURSE.FAQ.en";
#elif L==8
	"COURSE.FAQ.en";
#elif L==9
	"COURSE.FAQ.en";
#endif

const char *Hlp_COURSE_FAQ_edit =
#if   L==1
	"COURSE.FAQ.es#editar";
#elif L==2
	"COURSE.FAQ.en#edit";
#elif L==3
	"COURSE.FAQ.en#edit";
#elif L==4
	"COURSE.FAQ.es#editar";
#elif L==5
	"COURSE.FAQ.en#edit";
#elif L==6
	"COURSE.FAQ.es#editar";
#elif L==7
	"COURSE.FAQ.en#edit";
#elif L==8
	"COURSE.FAQ.en#edit";
#elif L==9
	"COURSE.FAQ.en#edit";
#endif

const char *Hlp_COURSE_Links =
#if   L==1
	"COURSE.Links.es";
#elif L==2
	"COURSE.Links.en";
#elif L==3
	"COURSE.Links.en";
#elif L==4
	"COURSE.Links.es";
#elif L==5
	"COURSE.Links.en";
#elif L==6
	"COURSE.Links.es";
#elif L==7
	"COURSE.Links.en";
#elif L==8
	"COURSE.Links.en";
#elif L==9
	"COURSE.Links.en";
#endif

const char *Hlp_COURSE_Links_edit =
#if   L==1
	"COURSE.Links.es#editar";
#elif L==2
	"COURSE.Links.en#edit";
#elif L==3
	"COURSE.Links.en#edit";
#elif L==4
	"COURSE.Links.es#editar";
#elif L==5
	"COURSE.Links.en#edit";
#elif L==6
	"COURSE.Links.es#editar";
#elif L==7
	"COURSE.Links.en#edit";
#elif L==8
	"COURSE.Links.en#edit";
#elif L==9
	"COURSE.Links.en#edit";
#endif

/***** ASSESSMENT tab *****/

const char *Hlp_ASSESSMENT_System =
#if   L==1
	"ASSESSMENT.System.es";
#elif L==2
	"ASSESSMENT.System.en";
#elif L==3
	"ASSESSMENT.System.en";
#elif L==4
	"ASSESSMENT.System.es";
#elif L==5
	"ASSESSMENT.System.en";
#elif L==6
	"ASSESSMENT.System.es";
#elif L==7
	"ASSESSMENT.System.en";
#elif L==8
	"ASSESSMENT.System.en";
#elif L==9
	"ASSESSMENT.System.en";
#endif

const char *Hlp_ASSESSMENT_System_edit =
#if   L==1
	"ASSESSMENT.System.es#editar";
#elif L==2
	"ASSESSMENT.System.en#edit";
#elif L==3
	"ASSESSMENT.System.en#edit";
#elif L==4
	"ASSESSMENT.System.es#editar";
#elif L==5
	"ASSESSMENT.System.en#edit";
#elif L==6
	"ASSESSMENT.System.es#editar";
#elif L==7
	"ASSESSMENT.System.en#edit";
#elif L==8
	"ASSESSMENT.System.en#edit";
#elif L==9
	"ASSESSMENT.System.en#edit";
#endif

const char *Hlp_ASSESSMENT_Assignments =
#if   L==1
	"ASSESSMENT.Assignments.es";
#elif L==2
	"ASSESSMENT.Assignments.en";
#elif L==3
	"ASSESSMENT.Assignments.en";
#elif L==4
	"ASSESSMENT.Assignments.es";
#elif L==5
	"ASSESSMENT.Assignments.en";
#elif L==6
	"ASSESSMENT.Assignments.es";
#elif L==7
	"ASSESSMENT.Assignments.en";
#elif L==8
	"ASSESSMENT.Assignments.en";
#elif L==9
	"ASSESSMENT.Assignments.en";
#endif

const char *Hlp_ASSESSMENT_Assignments_new_assignment =
#if   L==1
	"ASSESSMENT.Assignments.es#nueva-actividad";
#elif L==2
	"ASSESSMENT.Assignments.en#new-assignment";
#elif L==3
	"ASSESSMENT.Assignments.en#new-assignment";
#elif L==4
	"ASSESSMENT.Assignments.es#nueva-actividad";
#elif L==5
	"ASSESSMENT.Assignments.en#new-assignment";
#elif L==6
	"ASSESSMENT.Assignments.es#nueva-actividad";
#elif L==7
	"ASSESSMENT.Assignments.en#new-assignment";
#elif L==8
	"ASSESSMENT.Assignments.en#new-assignment";
#elif L==9
	"ASSESSMENT.Assignments.en#new-assignment";
#endif

const char *Hlp_ASSESSMENT_Assignments_edit_assignment =
#if   L==1
	"ASSESSMENT.Assignments.es#editar-actividad";
#elif L==2
	"ASSESSMENT.Assignments.en#edit-assignment";
#elif L==3
	"ASSESSMENT.Assignments.en#edit-assignment";
#elif L==4
	"ASSESSMENT.Assignments.es#editar-actividad";
#elif L==5
	"ASSESSMENT.Assignments.en#edit-assignment";
#elif L==6
	"ASSESSMENT.Assignments.es#editar-actividad";
#elif L==7
	"ASSESSMENT.Assignments.en#edit-assignment";
#elif L==8
	"ASSESSMENT.Assignments.en#edit-assignment";
#elif L==9
	"ASSESSMENT.Assignments.en#edit-assignment";
#endif


const char *Hlp_ASSESSMENT_Projects =
#if   L==1
	"ASSESSMENT.Projects.es";
#elif L==2
	"ASSESSMENT.Projects.en";
#elif L==3
	"ASSESSMENT.Projects.en";
#elif L==4
	"ASSESSMENT.Projects.es";
#elif L==5
	"ASSESSMENT.Projects.en";
#elif L==6
	"ASSESSMENT.Projects.es";
#elif L==7
	"ASSESSMENT.Projects.en";
#elif L==8
	"ASSESSMENT.Projects.en";
#elif L==9
	"ASSESSMENT.Projects.en";
#endif

const char *Hlp_ASSESSMENT_Projects_new_project =
#if   L==1
	"ASSESSMENT.Projects.es#nuevo-proyecto";
#elif L==2
	"ASSESSMENT.Projects.en#new-project";
#elif L==3
	"ASSESSMENT.Projects.en#new-project";
#elif L==4
	"ASSESSMENT.Projects.es#nuevo-proyecto";
#elif L==5
	"ASSESSMENT.Projects.en#new-project";
#elif L==6
	"ASSESSMENT.Projects.es#nuevo-proyecto";
#elif L==7
	"ASSESSMENT.Projects.en#new-project";
#elif L==8
	"ASSESSMENT.Projects.en#new-project";
#elif L==9
	"ASSESSMENT.Projects.en#new-project";
#endif

const char *Hlp_ASSESSMENT_Projects_edit_project =
#if   L==1
	"ASSESSMENT.Projects.es#editar-proyecto";
#elif L==2
	"ASSESSMENT.Projects.en#edit-project";
#elif L==3
	"ASSESSMENT.Projects.en#edit-project";
#elif L==4
	"ASSESSMENT.Projects.es#editar-proyecto";
#elif L==5
	"ASSESSMENT.Projects.en#edit-project";
#elif L==6
	"ASSESSMENT.Projects.es#editar-proyecto";
#elif L==7
	"ASSESSMENT.Projects.en#edit-project";
#elif L==8
	"ASSESSMENT.Projects.en#edit-project";
#elif L==9
	"ASSESSMENT.Projects.en#edit-project";
#endif

const char *Hlp_ASSESSMENT_Tests =
#if   L==1
	"ASSESSMENT.Tests.es";
#elif L==2
	"ASSESSMENT.Tests.en";
#elif L==3
	"ASSESSMENT.Tests.en";
#elif L==4
	"ASSESSMENT.Tests.es";
#elif L==5
	"ASSESSMENT.Tests.en";
#elif L==6
	"ASSESSMENT.Tests.es";
#elif L==7
	"ASSESSMENT.Tests.en";
#elif L==8
	"ASSESSMENT.Tests.en";
#elif L==9
	"ASSESSMENT.Tests.en";
#endif

const char *Hlp_ASSESSMENT_Tests_test_results =
#if   L==1
	"ASSESSMENT.Tests.es#resultados-de-tests";
#elif L==2
	"ASSESSMENT.Tests.en#test-results";
#elif L==3
	"ASSESSMENT.Tests.en#test-results";
#elif L==4
	"ASSESSMENT.Tests.es#resultados-de-tests";
#elif L==5
	"ASSESSMENT.Tests.en#test-results";
#elif L==6
	"ASSESSMENT.Tests.es#resultados-de-tests";
#elif L==7
	"ASSESSMENT.Tests.en#test-results";
#elif L==8
	"ASSESSMENT.Tests.en#test-results";
#elif L==9
	"ASSESSMENT.Tests.en#test-results";
#endif

const char *Hlp_ASSESSMENT_Surveys =
#if   L==1
	"ASSESSMENT.Surveys.es";
#elif L==2
	"ASSESSMENT.Surveys.en";
#elif L==3
	"ASSESSMENT.Surveys.en";
#elif L==4
	"ASSESSMENT.Surveys.es";
#elif L==5
	"ASSESSMENT.Surveys.en";
#elif L==6
	"ASSESSMENT.Surveys.es";
#elif L==7
	"ASSESSMENT.Surveys.en";
#elif L==8
	"ASSESSMENT.Surveys.en";
#elif L==9
	"ASSESSMENT.Surveys.en";
#endif

const char *Hlp_ASSESSMENT_Surveys_new_survey =
#if   L==1
	"ASSESSMENT.Surveys.es#nueva-encuesta";
#elif L==2
	"ASSESSMENT.Surveys.en#new-survey";
#elif L==3
	"ASSESSMENT.Surveys.en#new-survey";
#elif L==4
	"ASSESSMENT.Surveys.es#nueva-encuesta";
#elif L==5
	"ASSESSMENT.Surveys.en#new-survey";
#elif L==6
	"ASSESSMENT.Surveys.es#nueva-encuesta";
#elif L==7
	"ASSESSMENT.Surveys.en#new-survey";
#elif L==8
	"ASSESSMENT.Surveys.en#new-survey";
#elif L==9
	"ASSESSMENT.Surveys.en#new-survey";
#endif

const char *Hlp_ASSESSMENT_Surveys_edit_survey =
#if   L==1
	"ASSESSMENT.Surveys.es#editar-encuesta";
#elif L==2
	"ASSESSMENT.Surveys.en#edit-survey";
#elif L==3
	"ASSESSMENT.Surveys.en#edit-survey";
#elif L==4
	"ASSESSMENT.Surveys.es#editar-encuesta";
#elif L==5
	"ASSESSMENT.Surveys.en#edit-survey";
#elif L==6
	"ASSESSMENT.Surveys.es#editar-encuesta";
#elif L==7
	"ASSESSMENT.Surveys.en#edit-survey";
#elif L==8
	"ASSESSMENT.Surveys.en#edit-survey";
#elif L==9
	"ASSESSMENT.Surveys.en#edit-survey";
#endif

const char *Hlp_ASSESSMENT_Surveys_questions =
#if   L==1
	"ASSESSMENT.Surveys.es#preguntas";
#elif L==2
	"ASSESSMENT.Surveys.en#questions";
#elif L==3
	"ASSESSMENT.Surveys.en#questions";
#elif L==4
	"ASSESSMENT.Surveys.es#preguntas";
#elif L==5
	"ASSESSMENT.Surveys.en#questions";
#elif L==6
	"ASSESSMENT.Surveys.es#preguntas";
#elif L==7
	"ASSESSMENT.Surveys.en#questions";
#elif L==8
	"ASSESSMENT.Surveys.en#questions";
#elif L==9
	"ASSESSMENT.Surveys.en#questions";
#endif

const char *Hlp_ASSESSMENT_Games =
#if   L==1
	"ASSESSMENT.Games.es";
#elif L==2
	"ASSESSMENT.Games.en";
#elif L==3
	"ASSESSMENT.Games.en";
#elif L==4
	"ASSESSMENT.Games.es";
#elif L==5
	"ASSESSMENT.Games.en";
#elif L==6
	"ASSESSMENT.Games.es";
#elif L==7
	"ASSESSMENT.Games.en";
#elif L==8
	"ASSESSMENT.Games.en";
#elif L==9
	"ASSESSMENT.Games.en";
#endif

const char *Hlp_ASSESSMENT_Games_edit_game =
#if   L==1
	"ASSESSMENT.Games.es#editar-juego";
#elif L==2
	"ASSESSMENT.Games.en#edit-game";
#elif L==3
	"ASSESSMENT.Games.en#edit-game";
#elif L==4
	"ASSESSMENT.Games.es#editar-juego";
#elif L==5
	"ASSESSMENT.Games.en#edit-game";
#elif L==6
	"ASSESSMENT.Games.es#editar-juego";
#elif L==7
	"ASSESSMENT.Games.en#edit-game";
#elif L==8
	"ASSESSMENT.Games.en#edit-game";
#elif L==9
	"ASSESSMENT.Games.en#edit-game";
#endif

const char *Hlp_ASSESSMENT_Games_new_game =
#if   L==1
	"ASSESSMENT.Games.es#nueva-juego";
#elif L==2
	"ASSESSMENT.Games.en#new-game";
#elif L==3
	"ASSESSMENT.Games.en#new-game";
#elif L==4
	"ASSESSMENT.Games.es#nueva-juego";
#elif L==5
	"ASSESSMENT.Games.en#new-game";
#elif L==6
	"ASSESSMENT.Games.es#nueva-juego";
#elif L==7
	"ASSESSMENT.Games.en#new-game";
#elif L==8
	"ASSESSMENT.Games.en#new-game";
#elif L==9
	"ASSESSMENT.Games.en#new-game";
#endif

const char *Hlp_ASSESSMENT_Games_edit_juego =
#if   L==1
	"ASSESSMENT.Games.es#editar-juego";
#elif L==2
	"ASSESSMENT.Games.en#edit-juego";
#elif L==3
	"ASSESSMENT.Games.en#edit-juego";
#elif L==4
	"ASSESSMENT.Games.es#editar-juego";
#elif L==5
	"ASSESSMENT.Games.en#edit-juego";
#elif L==6
	"ASSESSMENT.Games.es#editar-juego";
#elif L==7
	"ASSESSMENT.Games.en#edit-juego";
#elif L==8
	"ASSESSMENT.Games.en#edit-juego";
#elif L==9
	"ASSESSMENT.Games.en#edit-juego";
#endif

const char *Hlp_ASSESSMENT_Games_questions =
#if   L==1
	"ASSESSMENT.Games.es#preguntas";
#elif L==2
	"ASSESSMENT.Games.en#questions";
#elif L==3
	"ASSESSMENT.Games.en#questions";
#elif L==4
	"ASSESSMENT.Games.es#preguntas";
#elif L==5
	"ASSESSMENT.Games.en#questions";
#elif L==6
	"ASSESSMENT.Games.es#preguntas";
#elif L==7
	"ASSESSMENT.Games.en#questions";
#elif L==8
	"ASSESSMENT.Games.en#questions";
#elif L==9
	"ASSESSMENT.Games.en#questions";
#endif

const char *Hlp_ASSESSMENT_Announcements =
#if   L==1
	"ASSESSMENT.Announcements.es";
#elif L==2
	"ASSESSMENT.Announcements.en";
#elif L==3
	"ASSESSMENT.Announcements.en";
#elif L==4
	"ASSESSMENT.Announcements.es";
#elif L==5
	"ASSESSMENT.Announcements.en";
#elif L==6
	"ASSESSMENT.Announcements.es";
#elif L==7
	"ASSESSMENT.Announcements.en";
#elif L==8
	"ASSESSMENT.Announcements.en";
#elif L==9
	"ASSESSMENT.Announcements.en";
#endif

const char *Hlp_ASSESSMENT_Announcements_new_announcement =
#if   L==1
	"ASSESSMENT.Announcements.es#nueva-convocatoria";
#elif L==2
	"ASSESSMENT.Announcements.en#new-announcement";
#elif L==3
	"ASSESSMENT.Announcements.en#new-announcement";
#elif L==4
	"ASSESSMENT.Announcements.es#nueva-convocatoria";
#elif L==5
	"ASSESSMENT.Announcements.en#new-announcement";
#elif L==6
	"ASSESSMENT.Announcements.es#nueva-convocatoria";
#elif L==7
	"ASSESSMENT.Announcements.en#new-announcement";
#elif L==8
	"ASSESSMENT.Announcements.en#new-announcement";
#elif L==9
	"ASSESSMENT.Announcements.en#new-announcement";
#endif

const char *Hlp_ASSESSMENT_Announcements_edit_announcement =
#if   L==1
	"ASSESSMENT.Announcements.es#editar-convocatoria";
#elif L==2
	"ASSESSMENT.Announcements.en#edit-announcement";
#elif L==3
	"ASSESSMENT.Announcements.en#edit-announcement";
#elif L==4
	"ASSESSMENT.Announcements.es#editar-convocatoria";
#elif L==5
	"ASSESSMENT.Announcements.en#edit-announcement";
#elif L==6
	"ASSESSMENT.Announcements.es#editar-convocatoria";
#elif L==7
	"ASSESSMENT.Announcements.en#edit-announcement";
#elif L==8
	"ASSESSMENT.Announcements.en#edit-announcement";
#elif L==9
	"ASSESSMENT.Announcements.en#edit-announcement";
#endif

/***** FILES tab *****/

const char *Hlp_FILES_Documents =
#if   L==1
	"FILES.Documents.es";
#elif L==2
	"FILES.Documents.en";
#elif L==3
	"FILES.Documents.en";
#elif L==4
	"FILES.Documents.es";
#elif L==5
	"FILES.Documents.en";
#elif L==6
	"FILES.Documents.es";
#elif L==7
	"FILES.Documents.en";
#elif L==8
	"FILES.Documents.en";
#elif L==9
	"FILES.Documents.en";
#endif

const char *Hlp_FILES_Private =
#if   L==1
	"FILES.Private.es";
#elif L==2
	"FILES.Private.en";
#elif L==3
	"FILES.Private.en";
#elif L==4
	"FILES.Private.es";
#elif L==5
	"FILES.Private.en";
#elif L==6
	"FILES.Private.es";
#elif L==7
	"FILES.Private.en";
#elif L==8
	"FILES.Private.en";
#elif L==9
	"FILES.Private.en";
#endif

const char *Hlp_FILES_Shared =
#if   L==1
	"FILES.Shared.es";
#elif L==2
	"FILES.Shared.en";
#elif L==3
	"FILES.Shared.en";
#elif L==4
	"FILES.Shared.es";
#elif L==5
	"FILES.Shared.en";
#elif L==6
	"FILES.Shared.es";
#elif L==7
	"FILES.Shared.en";
#elif L==8
	"FILES.Shared.en";
#elif L==9
	"FILES.Shared.en";
#endif

const char *Hlp_FILES_Homework_for_students =
#if   L==1
	"FILES.Homework.es#para-estudiantes";
#elif L==2
	"FILES.Homework.en#for-students";
#elif L==3
	"FILES.Homework.en#for-students";
#elif L==4
	"FILES.Homework.es#para-estudiantes";
#elif L==5
	"FILES.Homework.en#for-students";
#elif L==6
	"FILES.Homework.es#para-estudiantes";
#elif L==7
	"FILES.Homework.en#for-students";
#elif L==8
	"FILES.Homework.en#for-students";
#elif L==9
	"FILES.Homework.en#for-students";
#endif

const char *Hlp_FILES_Homework_for_teachers =
#if   L==1
	"FILES.Homework.es#para-profesores";
#elif L==2
	"FILES.Homework.en#for-teachers";
#elif L==3
	"FILES.Homework.en#for-teachers";
#elif L==4
	"FILES.Homework.es#para-profesores";
#elif L==5
	"FILES.Homework.en#for-teachers";
#elif L==6
	"FILES.Homework.es#para-profesores";
#elif L==7
	"FILES.Homework.en#for-teachers";
#elif L==8
	"FILES.Homework.en#for-teachers";
#elif L==9
	"FILES.Homework.en#for-teachers";
#endif

const char *Hlp_FILES_Marks =
#if   L==1
	"FILES.Marks.es";
#elif L==2
	"FILES.Marks.en";
#elif L==3
	"FILES.Marks.en";
#elif L==4
	"FILES.Marks.es";
#elif L==5
	"FILES.Marks.en";
#elif L==6
	"FILES.Marks.es";
#elif L==7
	"FILES.Marks.en";
#elif L==8
	"FILES.Marks.en";
#elif L==9
	"FILES.Marks.en";
#endif

/***** USERS tab *****/

const char *Hlp_USERS_Groups =
#if   L==1
	"USERS.Groups.es";
#elif L==2
	"USERS.Groups.en";
#elif L==3
	"USERS.Groups.en";
#elif L==4
	"USERS.Groups.es";
#elif L==5
	"USERS.Groups.en";
#elif L==6
	"USERS.Groups.es";
#elif L==7
	"USERS.Groups.en";
#elif L==8
	"USERS.Groups.en";
#elif L==9
	"USERS.Groups.en";
#endif

const char *Hlp_USERS_Students =
#if   L==1
	"USERS.Students.es";
#elif L==2
	"USERS.Students.en";
#elif L==3
	"USERS.Students.en";
#elif L==4
	"USERS.Students.es";
#elif L==5
	"USERS.Students.en";
#elif L==6
	"USERS.Students.es";
#elif L==7
	"USERS.Students.en";
#elif L==8
	"USERS.Students.en";
#elif L==9
	"USERS.Students.en";
#endif

const char *Hlp_USERS_Students_shared_record_card =
#if   L==1
	"USERS.Students.es#ficha-compartida";
#elif L==2
	"USERS.Students.en#shared-record-card";
#elif L==3
	"USERS.Students.en#shared-record-card";
#elif L==4
	"USERS.Students.es#ficha-compartida";
#elif L==5
	"USERS.Students.en#shared-record-card";
#elif L==6
	"USERS.Students.es#ficha-compartida";
#elif L==7
	"USERS.Students.en#shared-record-card";
#elif L==8
	"USERS.Students.en#shared-record-card";
#elif L==9
	"USERS.Students.en#shared-record-card";
#endif

const char *Hlp_USERS_Students_course_record_card =
#if   L==1
	"USERS.Students.es#ficha-de-la-asignatura";
#elif L==2
	"USERS.Students.en#course-record-card";
#elif L==3
	"USERS.Students.en#course-record-card";
#elif L==4
	"USERS.Students.es#ficha-de-la-asignatura";
#elif L==5
	"USERS.Students.en#course-record-card";
#elif L==6
	"USERS.Students.es#ficha-de-la-asignatura";
#elif L==7
	"USERS.Students.en#course-record-card";
#elif L==8
	"USERS.Students.en#course-record-card";
#elif L==9
	"USERS.Students.en#course-record-card";
#endif

const char *Hlp_USERS_Administration_administer_one_user =
#if   L==1
	"USERS.Administration.es#administrar-un-usuario";
#elif L==2
	"USERS.Administration.en#administer-one-user";
#elif L==3
	"USERS.Administration.en#administer-one-user";
#elif L==4
	"USERS.Administration.es#administrar-un-usuario";
#elif L==5
	"USERS.Administration.en#administer-one-user";
#elif L==6
	"USERS.Administration.es#administrar-un-usuario";
#elif L==7
	"USERS.Administration.en#administer-one-user";
#elif L==8
	"USERS.Administration.en#administer-one-user";
#elif L==9
	"USERS.Administration.en#administer-one-user";
#endif

const char *Hlp_USERS_Administration_administer_multiple_users =
#if   L==1
	"USERS.Administration.es#administrar-varios-usuarios";
#elif L==2
	"USERS.Administration.en#administer-multiple-users";
#elif L==3
	"USERS.Administration.en#administer-multiple-users";
#elif L==4
	"USERS.Administration.es#administrar-varios-usuarios";
#elif L==5
	"USERS.Administration.en#administer-multiple-users";
#elif L==6
	"USERS.Administration.es#administrar-varios-usuarios";
#elif L==7
	"USERS.Administration.en#administer-multiple-users";
#elif L==8
	"USERS.Administration.en#administer-multiple-users";
#elif L==9
	"USERS.Administration.en#administer-multiple-users";
#endif

const char *Hlp_USERS_Administration_remove_all_students =
#if   L==1
	"USERS.Administration.es#eliminar-todos-los-estudiantes";
#elif L==2
	"USERS.Administration.en#remove-all-students";
#elif L==3
	"USERS.Administration.en#remove-all-students";
#elif L==4
	"USERS.Administration.es#eliminar-todos-los-estudiantes";
#elif L==5
	"USERS.Administration.en#remove-all-students";
#elif L==6
	"USERS.Administration.es#eliminar-todos-los-estudiantes";
#elif L==7
	"USERS.Administration.en#remove-all-students";
#elif L==8
	"USERS.Administration.en#remove-all-students";
#elif L==9
	"USERS.Administration.en#remove-all-students";
#endif

const char *Hlp_USERS_Teachers =
#if   L==1
	"USERS.Teachers.es";
#elif L==2
	"USERS.Teachers.en";
#elif L==3
	"USERS.Teachers.en";
#elif L==4
	"USERS.Teachers.es";
#elif L==5
	"USERS.Teachers.en";
#elif L==6
	"USERS.Teachers.es";
#elif L==7
	"USERS.Teachers.en";
#elif L==8
	"USERS.Teachers.en";
#elif L==9
	"USERS.Teachers.en";
#endif

const char *Hlp_USERS_Teachers_shared_record_card =
#if   L==1
	"USERS.Teachers.es#ficha-compartida";
#elif L==2
	"USERS.Teachers.en#shared-record-card";
#elif L==3
	"USERS.Teachers.en#shared-record-card";
#elif L==4
	"USERS.Teachers.es#ficha-compartida";
#elif L==5
	"USERS.Teachers.en#shared-record-card";
#elif L==6
	"USERS.Teachers.es#ficha-compartida";
#elif L==7
	"USERS.Teachers.en#shared-record-card";
#elif L==8
	"USERS.Teachers.en#shared-record-card";
#elif L==9
	"USERS.Teachers.en#shared-record-card";
#endif

const char *Hlp_USERS_Teachers_timetable =
#if   L==1
	"USERS.Teachers.es#horario";
#elif L==2
	"USERS.Teachers.en#timetable";
#elif L==3
	"USERS.Teachers.en#timetable";
#elif L==4
	"USERS.Teachers.es#horario";
#elif L==5
	"USERS.Teachers.en#timetable";
#elif L==6
	"USERS.Teachers.es#horario";
#elif L==7
	"USERS.Teachers.en#timetable";
#elif L==8
	"USERS.Teachers.en#timetable";
#elif L==9
	"USERS.Teachers.en#timetable";
#endif

const char *Hlp_USERS_Administrators =
#if   L==1
	"USERS.Administrators.es";
#elif L==2
	"USERS.Administrators.en";
#elif L==3
	"USERS.Administrators.en";
#elif L==4
	"USERS.Administrators.es";
#elif L==5
	"USERS.Administrators.en";
#elif L==6
	"USERS.Administrators.es";
#elif L==7
	"USERS.Administrators.en";
#elif L==8
	"USERS.Administrators.en";
#elif L==9
	"USERS.Administrators.en";
#endif

const char *Hlp_USERS_Guests =
#if   L==1
	"USERS.Guests.es";
#elif L==2
	"USERS.Guests.en";
#elif L==3
	"USERS.Guests.en";
#elif L==4
	"USERS.Guests.es";
#elif L==5
	"USERS.Guests.en";
#elif L==6
	"USERS.Guests.es";
#elif L==7
	"USERS.Guests.en";
#elif L==8
	"USERS.Guests.en";
#elif L==9
	"USERS.Guests.en";
#endif

const char *Hlp_USERS_Duplicates_possibly_duplicate_users =
#if   L==1
	"USERS.Duplicates.es#usuarios-posiblemente-duplicados";
#elif L==2
	"USERS.Duplicates.en#possibly-duplicate-users";
#elif L==3
	"USERS.Duplicates.en#possibly-duplicate-users";
#elif L==4
	"USERS.Duplicates.es#usuarios-posiblemente-duplicados";
#elif L==5
	"USERS.Duplicates.en#possibly-duplicate-users";
#elif L==6
	"USERS.Duplicates.es#usuarios-posiblemente-duplicados";
#elif L==7
	"USERS.Duplicates.en#possibly-duplicate-users";
#elif L==8
	"USERS.Duplicates.en#possibly-duplicate-users";
#elif L==9
	"USERS.Duplicates.en#possibly-duplicate-users";
#endif

const char *Hlp_USERS_Duplicates_similar_users =
#if   L==1
	"USERS.Duplicates.es#usuarios-similares";
#elif L==2
	"USERS.Duplicates.en#similar-users";
#elif L==3
	"USERS.Duplicates.en#similar-users";
#elif L==4
	"USERS.Duplicates.es#usuarios-similares";
#elif L==5
	"USERS.Duplicates.en#similar-users";
#elif L==6
	"USERS.Duplicates.es#usuarios-similares";
#elif L==7
	"USERS.Duplicates.en#similar-users";
#elif L==8
	"USERS.Duplicates.en#similar-users";
#elif L==9
	"USERS.Duplicates.en#similar-users";
#endif

const char *Hlp_USERS_Attendance =
#if   L==1
	"USERS.Attendance.es";
#elif L==2
	"USERS.Attendance.en";
#elif L==3
	"USERS.Attendance.en";
#elif L==4
	"USERS.Attendance.es";
#elif L==5
	"USERS.Attendance.en";
#elif L==6
	"USERS.Attendance.es";
#elif L==7
	"USERS.Attendance.en";
#elif L==8
	"USERS.Attendance.en";
#elif L==9
	"USERS.Attendance.en";
#endif

const char *Hlp_USERS_Attendance_new_event =
#if   L==1
	"USERS.Attendance.es#nuevo-evento";
#elif L==2
	"USERS.Attendance.en#new-event";
#elif L==3
	"USERS.Attendance.en#new-event";
#elif L==4
	"USERS.Attendance.es#nuevo-evento";
#elif L==5
	"USERS.Attendance.en#new-event";
#elif L==6
	"USERS.Attendance.es#nuevo-evento";
#elif L==7
	"USERS.Attendance.en#new-event";
#elif L==8
	"USERS.Attendance.en#new-event";
#elif L==9
	"USERS.Attendance.en#new-event";
#endif

const char *Hlp_USERS_Attendance_edit_event =
#if   L==1
	"USERS.Attendance.es#editar-evento";
#elif L==2
	"USERS.Attendance.en#edit-event";
#elif L==3
	"USERS.Attendance.en#edit-event";
#elif L==4
	"USERS.Attendance.es#editar-evento";
#elif L==5
	"USERS.Attendance.en#edit-event";
#elif L==6
	"USERS.Attendance.es#editar-evento";
#elif L==7
	"USERS.Attendance.en#edit-event";
#elif L==8
	"USERS.Attendance.en#edit-event";
#elif L==9
	"USERS.Attendance.en#edit-event";
#endif

const char *Hlp_USERS_Attendance_attendance_list =
#if   L==1
	"USERS.Attendance.es#lista-de-asistencia";
#elif L==2
	"USERS.Attendance.en#attendance-list";
#elif L==3
	"USERS.Attendance.en#attendance-list";
#elif L==4
	"USERS.Attendance.es#lista-de-asistencia";
#elif L==5
	"USERS.Attendance.en#attendance-list";
#elif L==6
	"USERS.Attendance.es#lista-de-asistencia";
#elif L==7
	"USERS.Attendance.en#attendance-list";
#elif L==8
	"USERS.Attendance.en#attendance-list";
#elif L==9
	"USERS.Attendance.en#attendance-list";
#endif

const char *Hlp_USERS_SignUp =
#if   L==1
	"USERS.SignUp.es";
#elif L==2
	"USERS.SignUp.en";
#elif L==3
	"USERS.SignUp.en";
#elif L==4
	"USERS.SignUp.es";
#elif L==5
	"USERS.SignUp.en";
#elif L==6
	"USERS.SignUp.es";
#elif L==7
	"USERS.SignUp.en";
#elif L==8
	"USERS.SignUp.en";
#elif L==9
	"USERS.SignUp.en";
#endif

const char *Hlp_USERS_SignUp_confirm_enrolment =
#if   L==1
	"USERS.SignUp.es#confirmar-inscripci%C3%B3n";
#elif L==2
	"USERS.SignUp.en#confirm_enrolment";
#elif L==3
	"USERS.SignUp.en#confirm_enrolment";
#elif L==4
	"USERS.SignUp.es#confirmar-inscripci%C3%B3n";
#elif L==5
	"USERS.SignUp.en#confirm_enrolment";
#elif L==6
	"USERS.SignUp.es#confirmar-inscripci%C3%B3n";
#elif L==7
	"USERS.SignUp.en#confirm_enrolment";
#elif L==8
	"USERS.SignUp.en#confirm_enrolment";
#elif L==9
	"USERS.SignUp.en#confirm_enrolment";
#endif

const char *Hlp_USERS_Requests =
#if   L==1
	"USERS.Requests.es";
#elif L==2
	"USERS.Requests.en";
#elif L==3
	"USERS.Requests.en";
#elif L==4
	"USERS.Requests.es";
#elif L==5
	"USERS.Requests.en";
#elif L==6
	"USERS.Requests.es";
#elif L==7
	"USERS.Requests.en";
#elif L==8
	"USERS.Requests.en";
#elif L==9
	"USERS.Requests.en";
#endif

const char *Hlp_USERS_Connected =
#if   L==1
	"USERS.Connected.es";
#elif L==2
	"USERS.Connected.en";
#elif L==3
	"USERS.Connected.en";
#elif L==4
	"USERS.Connected.es";
#elif L==5
	"USERS.Connected.en";
#elif L==6
	"USERS.Connected.es";
#elif L==7
	"USERS.Connected.en";
#elif L==8
	"USERS.Connected.en";
#elif L==9
	"USERS.Connected.en";
#endif

const char *Hlp_USERS_Connected_last_clicks =
#if   L==1
	"USERS.Connected.es#%C3%9Altimos-clics";
#elif L==2
	"USERS.Connected.en#last-clicks";
#elif L==3
	"USERS.Connected.en#last-clicks";
#elif L==4
	"USERS.Connected.es#%C3%9Altimos-clics";
#elif L==5
	"USERS.Connected.en#last-clicks";
#elif L==6
	"USERS.Connected.es#%C3%9Altimos-clics";
#elif L==7
	"USERS.Connected.en#last-clicks";
#elif L==8
	"USERS.Connected.en#last-clicks";
#elif L==9
	"USERS.Connected.en#last-clicks";
#endif

/***** SOCIAL tab *****/

const char *Hlp_SOCIAL_Timeline =
#if   L==1
	"SOCIAL.Timeline.es";
#elif L==2
	"SOCIAL.Timeline.en";
#elif L==3
	"SOCIAL.Timeline.en";
#elif L==4
	"SOCIAL.Timeline.es";
#elif L==5
	"SOCIAL.Timeline.en";
#elif L==6
	"SOCIAL.Timeline.es";
#elif L==7
	"SOCIAL.Timeline.en";
#elif L==8
	"SOCIAL.Timeline.en";
#elif L==9
	"SOCIAL.Timeline.en";
#endif

const char *Hlp_SOCIAL_Profiles_view_public_profile =
#if   L==1
	"SOCIAL.Profiles.es#ver-perfil-p%C3%BAblico";
#elif L==2
	"SOCIAL.Profiles.en#view-public-profile";
#elif L==3
	"SOCIAL.Profiles.en#view-public-profile";
#elif L==4
	"SOCIAL.Profiles.es#ver-perfil-p%C3%BAblico";
#elif L==5
	"SOCIAL.Profiles.en#view-public-profile";
#elif L==6
	"SOCIAL.Profiles.es#ver-perfil-p%C3%BAblico";
#elif L==7
	"SOCIAL.Profiles.en#view-public-profile";
#elif L==8
	"SOCIAL.Profiles.en#view-public-profile";
#elif L==9
	"SOCIAL.Profiles.en#view-public-profile";
#endif

const char *Hlp_SOCIAL_Profiles_who_to_follow =
#if   L==1
	"SOCIAL.Profiles.es#a-qui%C3%A9n-seguir";
#elif L==2
	"SOCIAL.Profiles.en#who-to-follow";
#elif L==3
	"SOCIAL.Profiles.en#who-to-follow";
#elif L==4
	"SOCIAL.Profiles.es#a-qui%C3%A9n-seguir";
#elif L==5
	"SOCIAL.Profiles.en#who-to-follow";
#elif L==6
	"SOCIAL.Profiles.es#a-qui%C3%A9n-seguir";
#elif L==7
	"SOCIAL.Profiles.en#who-to-follow";
#elif L==8
	"SOCIAL.Profiles.en#who-to-follow";
#elif L==9
	"SOCIAL.Profiles.en#who-to-follow";
#endif

const char *Hlp_SOCIAL_Forums =
#if   L==1
	"SOCIAL.Forums.es";
#elif L==2
	"SOCIAL.Forums.en";
#elif L==3
	"SOCIAL.Forums.en";
#elif L==4
	"SOCIAL.Forums.es";
#elif L==5
	"SOCIAL.Forums.en";
#elif L==6
	"SOCIAL.Forums.es";
#elif L==7
	"SOCIAL.Forums.en";
#elif L==8
	"SOCIAL.Forums.en";
#elif L==9
	"SOCIAL.Forums.en";
#endif

const char *Hlp_SOCIAL_Forums_threads =
#if   L==1
	"SOCIAL.Forums.es#discusiones";
#elif L==2
	"SOCIAL.Forums.en#threads";
#elif L==3
	"SOCIAL.Forums.en#threads";
#elif L==4
	"SOCIAL.Forums.es#discusiones";
#elif L==5
	"SOCIAL.Forums.en#threads";
#elif L==6
	"SOCIAL.Forums.es#discusiones";
#elif L==7
	"SOCIAL.Forums.en#threads";
#elif L==8
	"SOCIAL.Forums.en#threads";
#elif L==9
	"SOCIAL.Forums.en#threads";
#endif

const char *Hlp_SOCIAL_Forums_new_thread =
#if   L==1
	"SOCIAL.Forums.es#nueva-discusi%C3%B3n";
#elif L==2
	"SOCIAL.Forums.en#new-thread";
#elif L==3
	"SOCIAL.Forums.en#new-thread";
#elif L==4
	"SOCIAL.Forums.es#nueva-discusi%C3%B3n";
#elif L==5
	"SOCIAL.Forums.en#new-thread";
#elif L==6
	"SOCIAL.Forums.es#nueva-discusi%C3%B3n";
#elif L==7
	"SOCIAL.Forums.en#new-thread";
#elif L==8
	"SOCIAL.Forums.en#new-thread";
#elif L==9
	"SOCIAL.Forums.en#new-thread";
#endif

const char *Hlp_SOCIAL_Forums_posts =
#if   L==1
	"SOCIAL.Forums.es#comentarios";
#elif L==2
	"SOCIAL.Forums.en#posts";
#elif L==3
	"SOCIAL.Forums.en#posts";
#elif L==4
	"SOCIAL.Forums.es#comentarios";
#elif L==5
	"SOCIAL.Forums.en#posts";
#elif L==6
	"SOCIAL.Forums.es#comentarios";
#elif L==7
	"SOCIAL.Forums.en#posts";
#elif L==8
	"SOCIAL.Forums.en#posts";
#elif L==9
	"SOCIAL.Forums.en#posts";
#endif

const char *Hlp_SOCIAL_Forums_new_post =
#if   L==1
	"SOCIAL.Forums.es#nuevo-comentario";
#elif L==2
	"SOCIAL.Forums.en#new-post";
#elif L==3
	"SOCIAL.Forums.en#new-post";
#elif L==4
	"SOCIAL.Forums.es#nuevo-comentario";
#elif L==5
	"SOCIAL.Forums.en#new-post";
#elif L==6
	"SOCIAL.Forums.es#nuevo-comentario";
#elif L==7
	"SOCIAL.Forums.en#new-post";
#elif L==8
	"SOCIAL.Forums.en#new-post";
#elif L==9
	"SOCIAL.Forums.en#new-post";
#endif

/***** MESSAGES tab *****/

const char *Hlp_MESSAGES_Notifications =
#if   L==1
	"MESSAGES.Notifications.es";
#elif L==2
	"MESSAGES.Notifications.en";
#elif L==3
	"MESSAGES.Notifications.en";
#elif L==4
	"MESSAGES.Notifications.es";
#elif L==5
	"MESSAGES.Notifications.en";
#elif L==6
	"MESSAGES.Notifications.es";
#elif L==7
	"MESSAGES.Notifications.en";
#elif L==8
	"MESSAGES.Notifications.en";
#elif L==9
	"MESSAGES.Notifications.en";
#endif

const char *Hlp_MESSAGES_Domains =
#if   L==1
	"MESSAGES.Domains.es";
#elif L==2
	"MESSAGES.Domains.en";
#elif L==3
	"MESSAGES.Domains.en";
#elif L==4
	"MESSAGES.Domains.es";
#elif L==5
	"MESSAGES.Domains.en";
#elif L==6
	"MESSAGES.Domains.es";
#elif L==7
	"MESSAGES.Domains.en";
#elif L==8
	"MESSAGES.Domains.en";
#elif L==9
	"MESSAGES.Domains.en";
#endif

const char *Hlp_MESSAGES_Domains_edit =
#if   L==1
	"MESSAGES.Domains.es#editar";
#elif L==2
	"MESSAGES.Domains.en#edit";
#elif L==3
	"MESSAGES.Domains.en#edit";
#elif L==4
	"MESSAGES.Domains.es#editar";
#elif L==5
	"MESSAGES.Domains.en#edit";
#elif L==6
	"MESSAGES.Domains.es#editar";
#elif L==7
	"MESSAGES.Domains.en#edit";
#elif L==8
	"MESSAGES.Domains.en#edit";
#elif L==9
	"MESSAGES.Domains.en#edit";
#endif

const char *Hlp_MESSAGES_Announcements =
#if   L==1
	"MESSAGES.Announcements.es";
#elif L==2
	"MESSAGES.Announcements.en";
#elif L==3
	"MESSAGES.Announcements.en";
#elif L==4
	"MESSAGES.Announcements.es";
#elif L==5
	"MESSAGES.Announcements.en";
#elif L==6
	"MESSAGES.Announcements.es";
#elif L==7
	"MESSAGES.Announcements.en";
#elif L==8
	"MESSAGES.Announcements.en";
#elif L==9
	"MESSAGES.Announcements.en";
#endif

const char *Hlp_MESSAGES_Notices =
#if   L==1
	"MESSAGES.Notices.es";
#elif L==2
	"MESSAGES.Notices.en";
#elif L==3
	"MESSAGES.Notices.en";
#elif L==4
	"MESSAGES.Notices.es";
#elif L==5
	"MESSAGES.Notices.en";
#elif L==6
	"MESSAGES.Notices.es";
#elif L==7
	"MESSAGES.Notices.en";
#elif L==8
	"MESSAGES.Notices.en";
#elif L==9
	"MESSAGES.Notices.en";
#endif

const char *Hlp_MESSAGES_Write =
#if   L==1
	"MESSAGES.Write.es";
#elif L==2
	"MESSAGES.Write.en";
#elif L==3
	"MESSAGES.Write.en";
#elif L==4
	"MESSAGES.Write.es";
#elif L==5
	"MESSAGES.Write.en";
#elif L==6
	"MESSAGES.Write.es";
#elif L==7
	"MESSAGES.Write.en";
#elif L==8
	"MESSAGES.Write.en";
#elif L==9
	"MESSAGES.Write.en";
#endif

const char *Hlp_MESSAGES_Received =
#if   L==1
	"MESSAGES.Received.es";
#elif L==2
	"MESSAGES.Received.en";
#elif L==3
	"MESSAGES.Received.en";
#elif L==4
	"MESSAGES.Received.es";
#elif L==5
	"MESSAGES.Received.en";
#elif L==6
	"MESSAGES.Received.es";
#elif L==7
	"MESSAGES.Received.en";
#elif L==8
	"MESSAGES.Received.en";
#elif L==9
	"MESSAGES.Received.en";
#endif

const char *Hlp_MESSAGES_Received_filter =
#if   L==1
	"MESSAGES.Received.es#filtro";
#elif L==2
	"MESSAGES.Received.en#filter";
#elif L==3
	"MESSAGES.Received.en#filter";
#elif L==4
	"MESSAGES.Received.es#filtro";
#elif L==5
	"MESSAGES.Received.en#filter";
#elif L==6
	"MESSAGES.Received.es#filtro";
#elif L==7
	"MESSAGES.Received.en#filter";
#elif L==8
	"MESSAGES.Received.en#filter";
#elif L==9
	"MESSAGES.Received.en#filter";
#endif

const char *Hlp_MESSAGES_Sent =
#if   L==1
	"MESSAGES.Sent.es";
#elif L==2
	"MESSAGES.Sent.en";
#elif L==3
	"MESSAGES.Sent.en";
#elif L==4
	"MESSAGES.Sent.es";
#elif L==5
	"MESSAGES.Sent.en";
#elif L==6
	"MESSAGES.Sent.es";
#elif L==7
	"MESSAGES.Sent.en";
#elif L==8
	"MESSAGES.Sent.en";
#elif L==9
	"MESSAGES.Sent.en";
#endif

const char *Hlp_MESSAGES_Sent_filter =
#if   L==1
	"MESSAGES.Sent.es#filtro";
#elif L==2
	"MESSAGES.Sent.en#filter";
#elif L==3
	"MESSAGES.Sent.en#filter";
#elif L==4
	"MESSAGES.Sent.es#filtro";
#elif L==5
	"MESSAGES.Sent.en#filter";
#elif L==6
	"MESSAGES.Sent.es#filtro";
#elif L==7
	"MESSAGES.Sent.en#filter";
#elif L==8
	"MESSAGES.Sent.en#filter";
#elif L==9
	"MESSAGES.Sent.en#filter";
#endif

const char *Hlp_MESSAGES_Email =
#if   L==1
	"MESSAGES.Email.es";
#elif L==2
	"MESSAGES.Email.en";
#elif L==3
	"MESSAGES.Email.en";
#elif L==4
	"MESSAGES.Email.es";
#elif L==5
	"MESSAGES.Email.en";
#elif L==6
	"MESSAGES.Email.es";
#elif L==7
	"MESSAGES.Email.en";
#elif L==8
	"MESSAGES.Email.en";
#elif L==9
	"MESSAGES.Email.en";
#endif

/***** STATS tab *****/

const char *Hlp_STATS_Figures =
#if   L==1
	"STATS.Figures.es";
#elif L==2
	"STATS.Figures.en";
#elif L==3
	"STATS.Figures.en";
#elif L==4
	"STATS.Figures.es";
#elif L==5
	"STATS.Figures.en";
#elif L==6
	"STATS.Figures.es";
#elif L==7
	"STATS.Figures.en";
#elif L==8
	"STATS.Figures.en";
#elif L==9
	"STATS.Figures.en";
#endif

const char *Hlp_STATS_Figures_users =
#if   L==1
	"STATS.Figures.es#usuarios";
#elif L==2
	"STATS.Figures.en#users";
#elif L==3
	"STATS.Figures.en#users";
#elif L==4
	"STATS.Figures.es#usuarios";
#elif L==5
	"STATS.Figures.en#users";
#elif L==6
	"STATS.Figures.es#usuarios";
#elif L==7
	"STATS.Figures.en#users";
#elif L==8
	"STATS.Figures.en#users";
#elif L==9
	"STATS.Figures.en#users";
#endif

const char *Hlp_STATS_Figures_ranking =
#if   L==1
	"STATS.Figures.es#ranking";
#elif L==2
	"STATS.Figures.en#ranking";
#elif L==3
	"STATS.Figures.en#ranking";
#elif L==4
	"STATS.Figures.es#ranking";
#elif L==5
	"STATS.Figures.en#ranking";
#elif L==6
	"STATS.Figures.es#ranking";
#elif L==7
	"STATS.Figures.en#ranking";
#elif L==8
	"STATS.Figures.en#ranking";
#elif L==9
	"STATS.Figures.en#ranking";
#endif

const char *Hlp_STATS_Figures_hierarchy =
#if   L==1
	"STATS.Figures.es#jerarqu%C3%ADa";
#elif L==2
	"STATS.Figures.en#hierarchy";
#elif L==3
	"STATS.Figures.en#hierarchy";
#elif L==4
	"STATS.Figures.es#jerarqu%C3%ADa";
#elif L==5
	"STATS.Figures.en#hierarchy";
#elif L==6
	"STATS.Figures.es#jerarqu%C3%ADa";
#elif L==7
	"STATS.Figures.en#hierarchy";
#elif L==8
	"STATS.Figures.en#hierarchy";
#elif L==9
	"STATS.Figures.en#hierarchy";
#endif

const char *Hlp_STATS_Figures_institutions =
#if   L==1
	"STATS.Figures.es#instituciones";
#elif L==2
	"STATS.Figures.en#institutions";
#elif L==3
	"STATS.Figures.en#institutions";
#elif L==4
	"STATS.Figures.es#instituciones";
#elif L==5
	"STATS.Figures.en#institutions";
#elif L==6
	"STATS.Figures.es#instituciones";
#elif L==7
	"STATS.Figures.en#institutions";
#elif L==8
	"STATS.Figures.en#institutions";
#elif L==9
	"STATS.Figures.en#institutions";
#endif

const char *Hlp_STATS_Figures_types_of_degree =
#if   L==1
	"STATS.Figures.es#tipos-de-titulaci%C3%B3n";
#elif L==2
	"STATS.Figures.en#types-of-degree";
#elif L==3
	"STATS.Figures.en#types-of-degree";
#elif L==4
	"STATS.Figures.es#tipos-de-titulaci%C3%B3n";
#elif L==5
	"STATS.Figures.en#types-of-degree";
#elif L==6
	"STATS.Figures.es#tipos-de-titulaci%C3%B3n";
#elif L==7
	"STATS.Figures.en#types-of-degree";
#elif L==8
	"STATS.Figures.en#types-of-degree";
#elif L==9
	"STATS.Figures.en#types-of-degree";
#endif

const char *Hlp_STATS_Figures_folders_and_files =
#if   L==1
	"STATS.Figures.es#carpetas-y-archivos";
#elif L==2
	"STATS.Figures.en#folders-and-files";
#elif L==3
	"STATS.Figures.en#folders-and-files";
#elif L==4
	"STATS.Figures.es#carpetas-y-archivos";
#elif L==5
	"STATS.Figures.en#folders-and-files";
#elif L==6
	"STATS.Figures.es#carpetas-y-archivos";
#elif L==7
	"STATS.Figures.en#folders-and-files";
#elif L==8
	"STATS.Figures.en#folders-and-files";
#elif L==9
	"STATS.Figures.en#folders-and-files";
#endif

const char *Hlp_STATS_Figures_open_educational_resources_oer =
#if   L==1
	"STATS.Figures.es#recursos-educativos-abiertos-oer";
#elif L==2
	"STATS.Figures.en#open-educational-resources-oer";
#elif L==3
	"STATS.Figures.en#open-educational-resources-oer";
#elif L==4
	"STATS.Figures.es#recursos-educativos-abiertos-oer";
#elif L==5
	"STATS.Figures.en#open-educational-resources-oer";
#elif L==6
	"STATS.Figures.es#recursos-educativos-abiertos-oer";
#elif L==7
	"STATS.Figures.en#open-educational-resources-oer";
#elif L==8
	"STATS.Figures.en#open-educational-resources-oer";
#elif L==9
	"STATS.Figures.en#open-educational-resources-oer";
#endif

const char *Hlp_STATS_Figures_assignments =
#if   L==1
	"STATS.Figures.es#actividades";
#elif L==2
	"STATS.Figures.en#assignments";
#elif L==3
	"STATS.Figures.en#assignments";
#elif L==4
	"STATS.Figures.es#actividades";
#elif L==5
	"STATS.Figures.en#assignments";
#elif L==6
	"STATS.Figures.es#actividades";
#elif L==7
	"STATS.Figures.en#assignments";
#elif L==8
	"STATS.Figures.en#assignments";
#elif L==9
	"STATS.Figures.en#assignments";
#endif

const char *Hlp_STATS_Figures_tests =
#if   L==1
	"STATS.Figures.es#tests";
#elif L==2
	"STATS.Figures.en#tests";
#elif L==3
	"STATS.Figures.en#tests";
#elif L==4
	"STATS.Figures.es#tests";
#elif L==5
	"STATS.Figures.en#tests";
#elif L==6
	"STATS.Figures.es#tests";
#elif L==7
	"STATS.Figures.en#tests";
#elif L==8
	"STATS.Figures.en#tests";
#elif L==9
	"STATS.Figures.en#tests";
#endif

const char *Hlp_STATS_Figures_timeline =
#if   L==1
	"STATS.Figures.es#timeline";
#elif L==2
	"STATS.Figures.en#timeline";
#elif L==3
	"STATS.Figures.en#timeline";
#elif L==4
	"STATS.Figures.es#timeline";
#elif L==5
	"STATS.Figures.en#timeline";
#elif L==6
	"STATS.Figures.es#timeline";
#elif L==7
	"STATS.Figures.en#timeline";
#elif L==8
	"STATS.Figures.en#timeline";
#elif L==9
	"STATS.Figures.en#timeline";
#endif

const char *Hlp_STATS_Figures_followed_followers =
#if   L==1
	"STATS.Figures.es#seguidos--seguidores";
#elif L==2
	"STATS.Figures.en#followed--followers";
#elif L==3
	"STATS.Figures.en#followed--followers";
#elif L==4
	"STATS.Figures.es#seguidos--seguidores";
#elif L==5
	"STATS.Figures.en#followed--followers";
#elif L==6
	"STATS.Figures.es#seguidos--seguidores";
#elif L==7
	"STATS.Figures.en#followed--followers";
#elif L==8
	"STATS.Figures.en#followed--followers";
#elif L==9
	"STATS.Figures.en#followed--followers";
#endif

const char *Hlp_STATS_Figures_forums =
#if   L==1
	"STATS.Figures.es#foros";
#elif L==2
	"STATS.Figures.en#forums";
#elif L==3
	"STATS.Figures.en#forums";
#elif L==4
	"STATS.Figures.es#foros";
#elif L==5
	"STATS.Figures.en#forums";
#elif L==6
	"STATS.Figures.es#foros";
#elif L==7
	"STATS.Figures.en#forums";
#elif L==8
	"STATS.Figures.en#forums";
#elif L==9
	"STATS.Figures.en#forums";
#endif

const char *Hlp_STATS_Figures_notifications =
#if   L==1
	"STATS.Figures.es#notificaciones";
#elif L==2
	"STATS.Figures.en#notifications";
#elif L==3
	"STATS.Figures.en#notifications";
#elif L==4
	"STATS.Figures.es#notificaciones";
#elif L==5
	"STATS.Figures.en#notifications";
#elif L==6
	"STATS.Figures.es#notificaciones";
#elif L==7
	"STATS.Figures.en#notifications";
#elif L==8
	"STATS.Figures.en#notifications";
#elif L==9
	"STATS.Figures.en#notifications";
#endif

const char *Hlp_STATS_Figures_notices =
#if   L==1
	"STATS.Figures.es#avisos";
#elif L==2
	"STATS.Figures.en#notices";
#elif L==3
	"STATS.Figures.en#notices";
#elif L==4
	"STATS.Figures.es#avisos";
#elif L==5
	"STATS.Figures.en#notices";
#elif L==6
	"STATS.Figures.es#avisos";
#elif L==7
	"STATS.Figures.en#notices";
#elif L==8
	"STATS.Figures.en#notices";
#elif L==9
	"STATS.Figures.en#notices";
#endif

const char *Hlp_STATS_Figures_messages =
#if   L==1
	"STATS.Figures.es#mensajes";
#elif L==2
	"STATS.Figures.en#messages";
#elif L==3
	"STATS.Figures.en#messages";
#elif L==4
	"STATS.Figures.es#mensajes";
#elif L==5
	"STATS.Figures.en#messages";
#elif L==6
	"STATS.Figures.es#mensajes";
#elif L==7
	"STATS.Figures.en#messages";
#elif L==8
	"STATS.Figures.en#messages";
#elif L==9
	"STATS.Figures.en#messages";
#endif

const char *Hlp_STATS_Figures_surveys =
#if   L==1
	"STATS.Figures.es#encuestas";
#elif L==2
	"STATS.Figures.en#surveys";
#elif L==3
	"STATS.Figures.en#surveys";
#elif L==4
	"STATS.Figures.es#encuestas";
#elif L==5
	"STATS.Figures.en#surveys";
#elif L==6
	"STATS.Figures.es#encuestas";
#elif L==7
	"STATS.Figures.en#surveys";
#elif L==8
	"STATS.Figures.en#surveys";
#elif L==9
	"STATS.Figures.en#surveys";
#endif

const char *Hlp_STATS_Figures_webs_social_networks =
#if   L==1
	"STATS.Figures.es#webs--redes-sociales";
#elif L==2
	"STATS.Figures.en#webs--social-networks";
#elif L==3
	"STATS.Figures.en#webs--social-networks";
#elif L==4
	"STATS.Figures.es#webs--redes-sociales";
#elif L==5
	"STATS.Figures.en#webs--social-networks";
#elif L==6
	"STATS.Figures.es#webs--redes-sociales";
#elif L==7
	"STATS.Figures.en#webs--social-networks";
#elif L==8
	"STATS.Figures.en#webs--social-networks";
#elif L==9
	"STATS.Figures.en#webs--social-networks";
#endif

const char *Hlp_STATS_Figures_language =
#if   L==1
	"STATS.Figures.es#idioma";
#elif L==2
	"STATS.Figures.en#language";
#elif L==3
	"STATS.Figures.en#language";
#elif L==4
	"STATS.Figures.es#idioma";
#elif L==5
	"STATS.Figures.en#language";
#elif L==6
	"STATS.Figures.es#idioma";
#elif L==7
	"STATS.Figures.en#language";
#elif L==8
	"STATS.Figures.en#language";
#elif L==9
	"STATS.Figures.en#language";
#endif

const char *Hlp_STATS_Figures_calendar =
#if   L==1
	"STATS.Figures.es#calendario";
#elif L==2
	"STATS.Figures.en#calendar";
#elif L==3
	"STATS.Figures.en#calendar";
#elif L==4
	"STATS.Figures.es#calendario";
#elif L==5
	"STATS.Figures.en#calendar";
#elif L==6
	"STATS.Figures.es#calendario";
#elif L==7
	"STATS.Figures.en#calendar";
#elif L==8
	"STATS.Figures.en#calendar";
#elif L==9
	"STATS.Figures.en#calendar";
#endif

const char *Hlp_STATS_Figures_dates =
#if   L==1
	"STATS.Figures.es#fechas";
#elif L==2
	"STATS.Figures.en#dates";
#elif L==3
	"STATS.Figures.en#dates";
#elif L==4
	"STATS.Figures.es#fechas";
#elif L==5
	"STATS.Figures.en#dates";
#elif L==6
	"STATS.Figures.es#fechas";
#elif L==7
	"STATS.Figures.en#dates";
#elif L==8
	"STATS.Figures.en#dates";
#elif L==9
	"STATS.Figures.en#dates";
#endif

const char *Hlp_STATS_Figures_icons =
#if   L==1
	"STATS.Figures.es#iconos";
#elif L==2
	"STATS.Figures.en#icons";
#elif L==3
	"STATS.Figures.en#icons";
#elif L==4
	"STATS.Figures.es#iconos";
#elif L==5
	"STATS.Figures.en#icons";
#elif L==6
	"STATS.Figures.es#iconos";
#elif L==7
	"STATS.Figures.en#icons";
#elif L==8
	"STATS.Figures.en#icons";
#elif L==9
	"STATS.Figures.en#icons";
#endif

const char *Hlp_STATS_Figures_menu =
#if   L==1
	"STATS.Figures.es#men%C3%BA";
#elif L==2
	"STATS.Figures.en#menu";
#elif L==3
	"STATS.Figures.en#menu";
#elif L==4
	"STATS.Figures.es#men%C3%BA";
#elif L==5
	"STATS.Figures.en#menu";
#elif L==6
	"STATS.Figures.es#men%C3%BA";
#elif L==7
	"STATS.Figures.en#menu";
#elif L==8
	"STATS.Figures.en#menu";
#elif L==9
	"STATS.Figures.en#menu";
#endif

const char *Hlp_STATS_Figures_theme =
#if   L==1
	"STATS.Figures.es#tema";
#elif L==2
	"STATS.Figures.en#theme";
#elif L==3
	"STATS.Figures.en#theme";
#elif L==4
	"STATS.Figures.es#tema";
#elif L==5
	"STATS.Figures.en#theme";
#elif L==6
	"STATS.Figures.es#tema";
#elif L==7
	"STATS.Figures.en#theme";
#elif L==8
	"STATS.Figures.en#theme";
#elif L==9
	"STATS.Figures.en#theme";
#endif

const char *Hlp_STATS_Figures_columns =
#if   L==1
	"STATS.Figures.es#columnas";
#elif L==2
	"STATS.Figures.en#columns";
#elif L==3
	"STATS.Figures.en#columns";
#elif L==4
	"STATS.Figures.es#columnas";
#elif L==5
	"STATS.Figures.en#columns";
#elif L==6
	"STATS.Figures.es#columnas";
#elif L==7
	"STATS.Figures.en#columns";
#elif L==8
	"STATS.Figures.en#columns";
#elif L==9
	"STATS.Figures.en#columns";
#endif

const char *Hlp_STATS_Figures_privacy =
#if   L==1
	"STATS.Figures.es#privacidad";
#elif L==2
	"STATS.Figures.en#privacy";
#elif L==3
	"STATS.Figures.en#privacy";
#elif L==4
	"STATS.Figures.es#privacidad";
#elif L==5
	"STATS.Figures.en#privacy";
#elif L==6
	"STATS.Figures.es#privacidad";
#elif L==7
	"STATS.Figures.en#privacy";
#elif L==8
	"STATS.Figures.en#privacy";
#elif L==9
	"STATS.Figures.en#privacy";
#endif

const char *Hlp_STATS_Degrees =
#if   L==1
	"STATS.Degrees.es";
#elif L==2
	"STATS.Degrees.en";
#elif L==3
	"STATS.Degrees.en";
#elif L==4
	"STATS.Degrees.es";
#elif L==5
	"STATS.Degrees.en";
#elif L==6
	"STATS.Degrees.es";
#elif L==7
	"STATS.Degrees.en";
#elif L==8
	"STATS.Degrees.en";
#elif L==9
	"STATS.Degrees.en";
#endif

const char *Hlp_STATS_Indicators =
#if   L==1
	"STATS.Indicators.es";
#elif L==2
	"STATS.Indicators.en";
#elif L==3
	"STATS.Indicators.en";
#elif L==4
	"STATS.Indicators.es";
#elif L==5
	"STATS.Indicators.en";
#elif L==6
	"STATS.Indicators.es";
#elif L==7
	"STATS.Indicators.en";
#elif L==8
	"STATS.Indicators.en";
#elif L==9
	"STATS.Indicators.en";
#endif

const char *Hlp_STATS_Visits_global_visits =
#if   L==1
	"STATS.Visits.es#accesos-globales";
#elif L==2
	"STATS.Visits.en#global-visits";
#elif L==3
	"STATS.Visits.en#global-visits";
#elif L==4
	"STATS.Visits.es#accesos-globales";
#elif L==5
	"STATS.Visits.en#global-visits";
#elif L==6
	"STATS.Visits.es#accesos-globales";
#elif L==7
	"STATS.Visits.en#global-visits";
#elif L==8
	"STATS.Visits.en#global-visits";
#elif L==9
	"STATS.Visits.en#global-visits";
#endif

const char *Hlp_STATS_Visits_visits_to_course =
#if   L==1
	"STATS.Visits.es#accesos-a-la-asignatura";
#elif L==2
	"STATS.Visits.en#visits-to-course";
#elif L==3
	"STATS.Visits.en#visits-to-course";
#elif L==4
	"STATS.Visits.es#accesos-a-la-asignatura";
#elif L==5
	"STATS.Visits.en#visits-to-course";
#elif L==6
	"STATS.Visits.es#accesos-a-la-asignatura";
#elif L==7
	"STATS.Visits.en#visits-to-course";
#elif L==8
	"STATS.Visits.en#visits-to-course";
#elif L==9
	"STATS.Visits.en#visits-to-course";
#endif

const char *Hlp_STATS_Report =
#if   L==1
	"STATS.Report.es";
#elif L==2
	"STATS.Report.en";
#elif L==3
	"STATS.Report.en";
#elif L==4
	"STATS.Report.es";
#elif L==5
	"STATS.Report.en";
#elif L==6
	"STATS.Report.es";
#elif L==7
	"STATS.Report.en";
#elif L==8
	"STATS.Report.en";
#elif L==9
	"STATS.Report.en";
#endif

const char *Hlp_STATS_Frequent =
#if   L==1
	"STATS.Frequent.es";
#elif L==2
	"STATS.Frequent.en";
#elif L==3
	"STATS.Frequent.en";
#elif L==4
	"STATS.Frequent.es";
#elif L==5
	"STATS.Frequent.en";
#elif L==6
	"STATS.Frequent.es";
#elif L==7
	"STATS.Frequent.en";
#elif L==8
	"STATS.Frequent.en";
#elif L==9
	"STATS.Frequent.en";
#endif

/***** PROFILE tab *****/

const char *Hlp_PROFILE_LogIn =
#if   L==1
	"PROFILE.LogIn.es";
#elif L==2
	"PROFILE.LogIn.en";
#elif L==3
	"PROFILE.LogIn.en";
#elif L==4
	"PROFILE.LogIn.es";
#elif L==5
	"PROFILE.LogIn.en";
#elif L==6
	"PROFILE.LogIn.es";
#elif L==7
	"PROFILE.LogIn.en";
#elif L==8
	"PROFILE.LogIn.en";
#elif L==9
	"PROFILE.LogIn.en";
#endif

const char *Hlp_PROFILE_SignUp =
#if   L==1
	"PROFILE.SignUp.es";
#elif L==2
	"PROFILE.SignUp.en";
#elif L==3
	"PROFILE.SignUp.en";
#elif L==4
	"PROFILE.SignUp.es";
#elif L==5
	"PROFILE.SignUp.en";
#elif L==6
	"PROFILE.SignUp.es";
#elif L==7
	"PROFILE.SignUp.en";
#elif L==8
	"PROFILE.SignUp.en";
#elif L==9
	"PROFILE.SignUp.en";
#endif

const char *Hlp_PROFILE_Account =
#if   L==1
	"PROFILE.Account.es";
#elif L==2
	"PROFILE.Account.en";
#elif L==3
	"PROFILE.Account.en";
#elif L==4
	"PROFILE.Account.es";
#elif L==5
	"PROFILE.Account.en";
#elif L==6
	"PROFILE.Account.es";
#elif L==7
	"PROFILE.Account.en";
#elif L==8
	"PROFILE.Account.en";
#elif L==9
	"PROFILE.Account.en";
#endif

const char *Hlp_PROFILE_Account_email =
#if   L==1
	"PROFILE.Account.es#correo";
#elif L==2
	"PROFILE.Account.en#email";
#elif L==3
	"PROFILE.Account.en#email";
#elif L==4
	"PROFILE.Account.es#correo";
#elif L==5
	"PROFILE.Account.en#email";
#elif L==6
	"PROFILE.Account.es#correo";
#elif L==7
	"PROFILE.Account.en#email";
#elif L==8
	"PROFILE.Account.en#email";
#elif L==9
	"PROFILE.Account.en#email";
#endif

const char *Hlp_PROFILE_Session_role =
#if   L==1
	"PROFILE.Session.es#rol";
#elif L==2
	"PROFILE.Session.en#role";
#elif L==3
	"PROFILE.Session.en#role";
#elif L==4
	"PROFILE.Session.es#rol";
#elif L==5
	"PROFILE.Session.en#role";
#elif L==6
	"PROFILE.Session.es#rol";
#elif L==7
	"PROFILE.Session.en#role";
#elif L==8
	"PROFILE.Session.en#role";
#elif L==9
	"PROFILE.Session.en#role";
#endif

const char *Hlp_PROFILE_Password =
#if   L==1
	"PROFILE.Password.es";
#elif L==2
	"PROFILE.Password.en";
#elif L==3
	"PROFILE.Password.en";
#elif L==4
	"PROFILE.Password.es";
#elif L==5
	"PROFILE.Password.en";
#elif L==6
	"PROFILE.Password.es";
#elif L==7
	"PROFILE.Password.en";
#elif L==8
	"PROFILE.Password.en";
#elif L==9
	"PROFILE.Password.en";
#endif

const char *Hlp_PROFILE_Courses =
#if   L==1
	"PROFILE.Courses.es";
#elif L==2
	"PROFILE.Courses.en";
#elif L==3
	"PROFILE.Courses.en";
#elif L==4
	"PROFILE.Courses.es";
#elif L==5
	"PROFILE.Courses.en";
#elif L==6
	"PROFILE.Courses.es";
#elif L==7
	"PROFILE.Courses.en";
#elif L==8
	"PROFILE.Courses.en";
#elif L==9
	"PROFILE.Courses.en";
#endif

const char *Hlp_PROFILE_Timetable =
#if   L==1
	"PROFILE.Timetable.es";
#elif L==2
	"PROFILE.Timetable.en";
#elif L==3
	"PROFILE.Timetable.en";
#elif L==4
	"PROFILE.Timetable.es";
#elif L==5
	"PROFILE.Timetable.en";
#elif L==6
	"PROFILE.Timetable.es";
#elif L==7
	"PROFILE.Timetable.en";
#elif L==8
	"PROFILE.Timetable.en";
#elif L==9
	"PROFILE.Timetable.en";
#endif

const char *Hlp_PROFILE_Agenda =
#if   L==1
	"PROFILE.Agenda.es";
#elif L==2
	"PROFILE.Agenda.en";
#elif L==3
	"PROFILE.Agenda.en";
#elif L==4
	"PROFILE.Agenda.es";
#elif L==5
	"PROFILE.Agenda.en";
#elif L==6
	"PROFILE.Agenda.es";
#elif L==7
	"PROFILE.Agenda.en";
#elif L==8
	"PROFILE.Agenda.en";
#elif L==9
	"PROFILE.Agenda.en";
#endif

const char *Hlp_PROFILE_Agenda_new_event =
#if   L==1
	"PROFILE.Agenda.es#nuevo-evento";
#elif L==2
	"PROFILE.Agenda.en#new-event";
#elif L==3
	"PROFILE.Agenda.en#new-event";
#elif L==4
	"PROFILE.Agenda.es#nuevo-evento";
#elif L==5
	"PROFILE.Agenda.en#new-event";
#elif L==6
	"PROFILE.Agenda.es#nuevo-evento";
#elif L==7
	"PROFILE.Agenda.en#new-event";
#elif L==8
	"PROFILE.Agenda.en#new-event";
#elif L==9
	"PROFILE.Agenda.en#new-event";
#endif

const char *Hlp_PROFILE_Agenda_edit_event =
#if   L==1
	"PROFILE.Agenda.es#editar-evento";
#elif L==2
	"PROFILE.Agenda.en#edit-event";
#elif L==3
	"PROFILE.Agenda.en#edit-event";
#elif L==4
	"PROFILE.Agenda.es#editar-evento";
#elif L==5
	"PROFILE.Agenda.en#edit-event";
#elif L==6
	"PROFILE.Agenda.es#editar-evento";
#elif L==7
	"PROFILE.Agenda.en#edit-event";
#elif L==8
	"PROFILE.Agenda.en#edit-event";
#elif L==9
	"PROFILE.Agenda.en#edit-event";
#endif

const char *Hlp_PROFILE_Agenda_public_agenda =
#if   L==1
	"PROFILE.Agenda.es#agenda-p%C3%BAblica";
#elif L==2
	"PROFILE.Agenda.en#public-agenda";
#elif L==3
	"PROFILE.Agenda.en#public-agenda";
#elif L==4
	"PROFILE.Agenda.es#agenda-p%C3%BAblica";
#elif L==5
	"PROFILE.Agenda.en#public-agenda";
#elif L==6
	"PROFILE.Agenda.es#agenda-p%C3%BAblica";
#elif L==7
	"PROFILE.Agenda.en#public-agenda";
#elif L==8
	"PROFILE.Agenda.en#public-agenda";
#elif L==9
	"PROFILE.Agenda.en#public-agenda";
#endif

const char *Hlp_PROFILE_Record =
#if   L==1
	"PROFILE.Record.es";
#elif L==2
	"PROFILE.Record.en";
#elif L==3
	"PROFILE.Record.en";
#elif L==4
	"PROFILE.Record.es";
#elif L==5
	"PROFILE.Record.en";
#elif L==6
	"PROFILE.Record.es";
#elif L==7
	"PROFILE.Record.en";
#elif L==8
	"PROFILE.Record.en";
#elif L==9
	"PROFILE.Record.en";
#endif

const char *Hlp_PROFILE_Institution =
#if   L==1
	"PROFILE.Institution.es";
#elif L==2
	"PROFILE.Institution.en";
#elif L==3
	"PROFILE.Institution.en";
#elif L==4
	"PROFILE.Institution.es";
#elif L==5
	"PROFILE.Institution.en";
#elif L==6
	"PROFILE.Institution.es";
#elif L==7
	"PROFILE.Institution.en";
#elif L==8
	"PROFILE.Institution.en";
#elif L==9
	"PROFILE.Institution.en";
#endif

const char *Hlp_PROFILE_Webs =
#if   L==1
	"PROFILE.Webs.es";
#elif L==2
	"PROFILE.Webs.en";
#elif L==3
	"PROFILE.Webs.en";
#elif L==4
	"PROFILE.Webs.es";
#elif L==5
	"PROFILE.Webs.en";
#elif L==6
	"PROFILE.Webs.es";
#elif L==7
	"PROFILE.Webs.en";
#elif L==8
	"PROFILE.Webs.en";
#elif L==9
	"PROFILE.Webs.en";
#endif

const char *Hlp_PROFILE_Photo =
#if   L==1
	"PROFILE.Photo.es";
#elif L==2
	"PROFILE.Photo.en";
#elif L==3
	"PROFILE.Photo.en";
#elif L==4
	"PROFILE.Photo.es";
#elif L==5
	"PROFILE.Photo.en";
#elif L==6
	"PROFILE.Photo.es";
#elif L==7
	"PROFILE.Photo.en";
#elif L==8
	"PROFILE.Photo.en";
#elif L==9
	"PROFILE.Photo.en";
#endif

const char *Hlp_PROFILE_Preferences_internationalization =
#if   L==1
	"PROFILE.Preferences.es#internacionalizaci%C3%B3n";
#elif L==2
	"PROFILE.Preferences.en#internationalization";
#elif L==3
	"PROFILE.Preferences.en#internationalization";
#elif L==4
	"PROFILE.Preferences.es#internacionalizaci%C3%B3n";
#elif L==5
	"PROFILE.Preferences.en#internationalization";
#elif L==6
	"PROFILE.Preferences.es#internacionalizaci%C3%B3n";
#elif L==7
	"PROFILE.Preferences.en#internationalization";
#elif L==8
	"PROFILE.Preferences.en#internationalization";
#elif L==9
	"PROFILE.Preferences.en#internationalization";
#endif

const char *Hlp_PROFILE_Preferences_language =
#if   L==1
	"PROFILE.Preferences.es#idioma";
#elif L==2
	"PROFILE.Preferences.en#language";
#elif L==3
	"PROFILE.Preferences.en#language";
#elif L==4
	"PROFILE.Preferences.es#idioma";
#elif L==5
	"PROFILE.Preferences.en#language";
#elif L==6
	"PROFILE.Preferences.es#idioma";
#elif L==7
	"PROFILE.Preferences.en#language";
#elif L==8
	"PROFILE.Preferences.en#language";
#elif L==9
	"PROFILE.Preferences.en#language";
#endif

const char *Hlp_PROFILE_Preferences_calendar =
#if   L==1
	"PROFILE.Preferences.es#calendario";
#elif L==2
	"PROFILE.Preferences.en#calendar";
#elif L==3
	"PROFILE.Preferences.en#calendar";
#elif L==4
	"PROFILE.Preferences.es#calendario";
#elif L==5
	"PROFILE.Preferences.en#calendar";
#elif L==6
	"PROFILE.Preferences.es#calendario";
#elif L==7
	"PROFILE.Preferences.en#calendar";
#elif L==8
	"PROFILE.Preferences.en#calendar";
#elif L==9
	"PROFILE.Preferences.en#calendar";
#endif

const char *Hlp_PROFILE_Preferences_dates =
#if   L==1
	"PROFILE.Preferences.es#fechas";
#elif L==2
	"PROFILE.Preferences.en#dates";
#elif L==3
	"PROFILE.Preferences.en#dates";
#elif L==4
	"PROFILE.Preferences.es#fechas";
#elif L==5
	"PROFILE.Preferences.en#dates";
#elif L==6
	"PROFILE.Preferences.es#fechas";
#elif L==7
	"PROFILE.Preferences.en#dates";
#elif L==8
	"PROFILE.Preferences.en#dates";
#elif L==9
	"PROFILE.Preferences.en#dates";
#endif

const char *Hlp_PROFILE_Preferences_design =
#if   L==1
	"PROFILE.Preferences.es#dise%C3%B1o";
#elif L==2
	"PROFILE.Preferences.en#design";
#elif L==3
	"PROFILE.Preferences.en#design";
#elif L==4
	"PROFILE.Preferences.es#dise%C3%B1o";
#elif L==5
	"PROFILE.Preferences.en#design";
#elif L==6
	"PROFILE.Preferences.es#dise%C3%B1o";
#elif L==7
	"PROFILE.Preferences.en#design";
#elif L==8
	"PROFILE.Preferences.en#design";
#elif L==9
	"PROFILE.Preferences.en#design";
#endif

const char *Hlp_PROFILE_Preferences_icons =
#if   L==1
	"PROFILE.Preferences.es#iconos";
#elif L==2
	"PROFILE.Preferences.en#icons";
#elif L==3
	"PROFILE.Preferences.en#icons";
#elif L==4
	"PROFILE.Preferences.es#iconos";
#elif L==5
	"PROFILE.Preferences.en#icons";
#elif L==6
	"PROFILE.Preferences.es#iconos";
#elif L==7
	"PROFILE.Preferences.en#icons";
#elif L==8
	"PROFILE.Preferences.en#icons";
#elif L==9
	"PROFILE.Preferences.en#icons";
#endif

const char *Hlp_PROFILE_Preferences_theme =
#if   L==1
	"PROFILE.Preferences.es#tema";
#elif L==2
	"PROFILE.Preferences.en#theme";
#elif L==3
	"PROFILE.Preferences.en#theme";
#elif L==4
	"PROFILE.Preferences.es#tema";
#elif L==5
	"PROFILE.Preferences.en#theme";
#elif L==6
	"PROFILE.Preferences.es#tema";
#elif L==7
	"PROFILE.Preferences.en#theme";
#elif L==8
	"PROFILE.Preferences.en#theme";
#elif L==9
	"PROFILE.Preferences.en#theme";
#endif

const char *Hlp_PROFILE_Preferences_menu =
#if   L==1
	"PROFILE.Preferences.es#men%C3%BA";
#elif L==2
	"PROFILE.Preferences.en#menu";
#elif L==3
	"PROFILE.Preferences.en#menu";
#elif L==4
	"PROFILE.Preferences.es#men%C3%BA";
#elif L==5
	"PROFILE.Preferences.en#menu";
#elif L==6
	"PROFILE.Preferences.es#men%C3%BA";
#elif L==7
	"PROFILE.Preferences.en#menu";
#elif L==8
	"PROFILE.Preferences.en#menu";
#elif L==9
	"PROFILE.Preferences.en#menu";
#endif

const char *Hlp_PROFILE_Preferences_columns =
#if   L==1
	"PROFILE.Preferences.es#columnas";
#elif L==2
	"PROFILE.Preferences.en#columns";
#elif L==3
	"PROFILE.Preferences.en#columns";
#elif L==4
	"PROFILE.Preferences.es#columnas";
#elif L==5
	"PROFILE.Preferences.en#columns";
#elif L==6
	"PROFILE.Preferences.es#columnas";
#elif L==7
	"PROFILE.Preferences.en#columns";
#elif L==8
	"PROFILE.Preferences.en#columns";
#elif L==9
	"PROFILE.Preferences.en#columns";
#endif

const char *Hlp_PROFILE_Preferences_privacy =
#if   L==1
	"PROFILE.Preferences.es#privacidad";
#elif L==2
	"PROFILE.Preferences.en#privacy";
#elif L==3
	"PROFILE.Preferences.en#privacy";
#elif L==4
	"PROFILE.Preferences.es#privacidad";
#elif L==5
	"PROFILE.Preferences.en#privacy";
#elif L==6
	"PROFILE.Preferences.es#privacidad";
#elif L==7
	"PROFILE.Preferences.en#privacy";
#elif L==8
	"PROFILE.Preferences.en#privacy";
#elif L==9
	"PROFILE.Preferences.en#privacy";
#endif

const char *Hlp_PROFILE_Preferences_notifications =
#if   L==1
	"PROFILE.Preferences.es#notificaciones";
#elif L==2
	"PROFILE.Preferences.en#notifications";
#elif L==3
	"PROFILE.Preferences.en#notifications";
#elif L==4
	"PROFILE.Preferences.es#notificaciones";
#elif L==5
	"PROFILE.Preferences.en#notifications";
#elif L==6
	"PROFILE.Preferences.es#notificaciones";
#elif L==7
	"PROFILE.Preferences.en#notifications";
#elif L==8
	"PROFILE.Preferences.en#notifications";
#elif L==9
	"PROFILE.Preferences.en#notifications";
#endif

const char *Hlp_PROFILE_Briefcase =
#if   L==1
	"PROFILE.Briefcase.es";
#elif L==2
	"PROFILE.Briefcase.en";
#elif L==3
	"PROFILE.Briefcase.en";
#elif L==4
	"PROFILE.Briefcase.es";
#elif L==5
	"PROFILE.Briefcase.en";
#elif L==6
	"PROFILE.Briefcase.es";
#elif L==7
	"PROFILE.Briefcase.en";
#elif L==8
	"PROFILE.Briefcase.en";
#elif L==9
	"PROFILE.Briefcase.en";
#endif
