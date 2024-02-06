// swad_help_URL.c: contextual help URLs, depending on the current language

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
/****************************** Public constants *****************************/
/*****************************************************************************/

#ifndef L
#define L 3	// English
#endif

const char *Hlp_Multimedia =
#if   L==1
	"Multimedia.es";
#elif L==2
	"Multimedia.en";
#elif L==3
	"Multimedia.en";
#elif L==4
	"Multimedia.es";
#elif L==5
	"Multimedia.en";
#elif L==6
	"Multimedia.es";
#elif L==7
	"Multimedia.en";
#elif L==8
	"Multimedia.en";
#elif L==9
	"Multimedia.en";
#elif L==10	// tr
	"Multimedia.en";
#endif

/***** START tab *****/

const char *Hlp_START_Search =
#if   L==1
	"START.Search.es";
#elif L==2
	"START.Search.en";
#elif L==3
	"START.Search.en";
#elif L==4
	"START.Search.es";
#elif L==5
	"START.Search.en";
#elif L==6
	"START.Search.es";
#elif L==7
	"START.Search.en";
#elif L==8
	"START.Search.en";
#elif L==9
	"START.Search.en";
#elif L==10	// tr
	"START.Search.en";
#endif

const char *Hlp_START_Timeline =
#if   L==1
	"START.Timeline.es";
#elif L==2
	"START.Timeline.en";
#elif L==3
	"START.Timeline.en";
#elif L==4
	"START.Timeline.es";
#elif L==5
	"START.Timeline.en";
#elif L==6
	"START.Timeline.es";
#elif L==7
	"START.Timeline.en";
#elif L==8
	"START.Timeline.en";
#elif L==9
	"START.Timeline.en";
#elif L==10	// tr
	"START.Timeline.en";
#endif

const char *Hlp_START_Profiles_view_public_profile =
#if   L==1
	"START.Profiles.es#ver-perfil-p%C3%BAblico";
#elif L==2
	"START.Profiles.en#view-public-profile";
#elif L==3
	"START.Profiles.en#view-public-profile";
#elif L==4
	"START.Profiles.es#ver-perfil-p%C3%BAblico";
#elif L==5
	"START.Profiles.en#view-public-profile";
#elif L==6
	"START.Profiles.es#ver-perfil-p%C3%BAblico";
#elif L==7
	"START.Profiles.en#view-public-profile";
#elif L==8
	"START.Profiles.en#view-public-profile";
#elif L==9
	"START.Profiles.en#view-public-profile";
#elif L==10	// tr
	"START.Profiles.en#view-public-profile";
#endif

const char *Hlp_START_Profiles_who_to_follow =
#if   L==1
	"START.Profiles.es#a-qui%C3%A9n-seguir";
#elif L==2
	"START.Profiles.en#who-to-follow";
#elif L==3
	"START.Profiles.en#who-to-follow";
#elif L==4
	"START.Profiles.es#a-qui%C3%A9n-seguir";
#elif L==5
	"START.Profiles.en#who-to-follow";
#elif L==6
	"START.Profiles.es#a-qui%C3%A9n-seguir";
#elif L==7
	"START.Profiles.en#who-to-follow";
#elif L==8
	"START.Profiles.en#who-to-follow";
#elif L==9
	"START.Profiles.en#who-to-follow";
#elif L==10	// tr
	"START.Profiles.en#who-to-follow";
#endif

const char *Hlp_START_Calendar =
#if   L==1
	"START.Calendar.es";
#elif L==2
	"START.Calendar.en";
#elif L==3
	"START.Calendar.en";
#elif L==4
	"START.Calendar.es";
#elif L==5
	"START.Calendar.en";
#elif L==6
	"START.Calendar.es";
#elif L==7
	"START.Calendar.en";
#elif L==8
	"START.Calendar.en";
#elif L==9
	"START.Calendar.en";
#elif L==10	// tr
	"START.Calendar.en";
#endif

const char *Hlp_START_Notifications =
#if   L==1
	"START.Notifications.es";
#elif L==2
	"START.Notifications.en";
#elif L==3
	"START.Notifications.en";
#elif L==4
	"START.Notifications.es";
#elif L==5
	"START.Notifications.en";
#elif L==6
	"START.Notifications.es";
#elif L==7
	"START.Notifications.en";
#elif L==8
	"START.Notifications.en";
#elif L==9
	"START.Notifications.en";
#elif L==10	// tr
	"START.Notifications.en";
#endif

const char *Hlp_START_Domains =
#if   L==1
	"START.Domains.es";
#elif L==2
	"START.Domains.en";
#elif L==3
	"START.Domains.en";
#elif L==4
	"START.Domains.es";
#elif L==5
	"START.Domains.en";
#elif L==6
	"START.Domains.es";
#elif L==7
	"START.Domains.en";
#elif L==8
	"START.Domains.en";
#elif L==9
	"START.Domains.en";
#elif L==10	// tr
	"START.Domains.en";
#endif

const char *Hlp_START_Domains_edit =
#if   L==1
	"START.Domains.es#editar";
#elif L==2
	"START.Domains.en#edit";
#elif L==3
	"START.Domains.en#edit";
#elif L==4
	"START.Domains.es#editar";
#elif L==5
	"START.Domains.en#edit";
#elif L==6
	"START.Domains.es#editar";
#elif L==7
	"START.Domains.en#edit";
#elif L==8
	"START.Domains.en#edit";
#elif L==9
	"START.Domains.en#edit";
#elif L==10	// tr
	"START.Domains.en#edit";
#endif

/***** SYSTEM tab *****/

const char *Hlp_SYSTEM_Information =
#if   L==1
	"SYSTEM.Information.es";
#elif L==2
	"SYSTEM.Information.en";
#elif L==3
	"SYSTEM.Information.en";
#elif L==4
	"SYSTEM.Information.es";
#elif L==5
	"SYSTEM.Information.en";
#elif L==6
	"SYSTEM.Information.es";
#elif L==7
	"SYSTEM.Information.en";
#elif L==8
	"SYSTEM.Information.en";
#elif L==9
	"SYSTEM.Information.en";
#elif L==10	// tr
	"SYSTEM.Information.en";
#endif

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
#elif L==10	// tr
	"SYSTEM.Countries.en";
#endif

const char *Hlp_SYSTEM_Pending =
#if   L==1
	"SYSTEM.Pending.es";
#elif L==2
	"SYSTEM.Pending.en";
#elif L==3
	"SYSTEM.Pending.en";
#elif L==4
	"SYSTEM.Pending.es";
#elif L==5
	"SYSTEM.Pending.en";
#elif L==6
	"SYSTEM.Pending.es";
#elif L==7
	"SYSTEM.Pending.en";
#elif L==8
	"SYSTEM.Pending.en";
#elif L==9
	"SYSTEM.Pending.en";
#elif L==10	// tr
	"SYSTEM.Pending.en";
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
#elif L==10	// tr
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
#elif L==10	// tr
	"SYSTEM.Links.en#edit";
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
#elif L==10	// tr
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
#elif L==10	// tr
	"SYSTEM.Banners.en#edit";
#endif

const char *Hlp_SYSTEM_Plugins =
#if   L==1
	"SYSTEM.Plugins.es";
#elif L==2
	"SYSTEM.Plugins.en";
#elif L==3
	"SYSTEM.Plugins.en";
#elif L==4
	"SYSTEM.Plugins.es";
#elif L==5
	"SYSTEM.Plugins.en";
#elif L==6
	"SYSTEM.Plugins.es";
#elif L==7
	"SYSTEM.Plugins.en";
#elif L==8
	"SYSTEM.Plugins.en";
#elif L==9
	"SYSTEM.Plugins.en";
#elif L==10	// tr
	"SYSTEM.Plugins.en";
#endif

const char *Hlp_SYSTEM_Plugins_edit =
#if   L==1
	"SYSTEM.Plugins.es#editar";
#elif L==2
	"SYSTEM.Plugins.en#edit";
#elif L==3
	"SYSTEM.Plugins.en#edit";
#elif L==4
	"SYSTEM.Plugins.es#editar";
#elif L==5
	"SYSTEM.Plugins.en#edit";
#elif L==6
	"SYSTEM.Plugins.es#editar";
#elif L==7
	"SYSTEM.Plugins.en#edit";
#elif L==8
	"SYSTEM.Plugins.en#edit";
#elif L==9
	"SYSTEM.Plugins.en#edit";
#elif L==10	// tr
	"SYSTEM.Plugins.en#edit";
#endif

const char *Hlp_SYSTEM_Maintenance_eliminate_old_courses =
#if   L==1
	"SYSTEM.Maintenance.es#eliminar-asignaturas-antiguas";
#elif L==2
	"SYSTEM.Maintenance.en#eliminate-old-courses";
#elif L==3
	"SYSTEM.Maintenance.en#eliminate-old-courses";
#elif L==4
	"SYSTEM.Maintenance.es#eliminar-asignaturas-antiguas";
#elif L==5
	"SYSTEM.Maintenance.en#eliminate-old-courses";
#elif L==6
	"SYSTEM.Maintenance.es#eliminar-asignaturas-antiguas";
#elif L==7
	"SYSTEM.Maintenance.en#eliminate-old-courses";
#elif L==8
	"SYSTEM.Maintenance.en#eliminate-old-courses";
#elif L==9
	"SYSTEM.Maintenance.en#eliminate-old-courses";
#elif L==10	// tr
	"SYSTEM.Maintenance.en#eliminate-old-courses";
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
#elif L==10	// tr
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
#elif L==10	// tr
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
#elif L==10	// tr
	"INSTITUTION.Information.en";
#endif

const char *Hlp_INSTITUTION_Centers =
#if   L==1
	"INSTITUTION.Centers.es";
#elif L==2
	"INSTITUTION.Centers.en";
#elif L==3
	"INSTITUTION.Centers.en";
#elif L==4
	"INSTITUTION.Centers.es";
#elif L==5
	"INSTITUTION.Centers.en";
#elif L==6
	"INSTITUTION.Centers.es";
#elif L==7
	"INSTITUTION.Centers.en";
#elif L==8
	"INSTITUTION.Centers.en";
#elif L==9
	"INSTITUTION.Centers.en";
#elif L==10	// tr
	"INSTITUTION.Centers.en";
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
#elif L==10	// tr
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
#elif L==10	// tr
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
#elif L==10	// tr
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
#elif L==10	// tr
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
#elif L==10	// tr
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
#elif L==10	// tr
	"INSTITUTION.Holidays.en#edit";
#endif

/***** CENTER tab *****/

const char *Hlp_CENTER_Information =
#if   L==1
	"CENTER.Information.es";
#elif L==2
	"CENTER.Information.en";
#elif L==3
	"CENTER.Information.en";
#elif L==4
	"CENTER.Information.es";
#elif L==5
	"CENTER.Information.en";
#elif L==6
	"CENTER.Information.es";
#elif L==7
	"CENTER.Information.en";
#elif L==8
	"CENTER.Information.en";
#elif L==9
	"CENTER.Information.en";
#elif L==10	// tr
	"CENTER.Information.en";
#endif

const char *Hlp_CENTER_DegreeTypes =
#if   L==1
	"CENTER.DegreeTypes.es";
#elif L==2
	"CENTER.DegreeTypes.en";
#elif L==3
	"CENTER.DegreeTypes.en";
#elif L==4
	"CENTER.DegreeTypes.es";
#elif L==5
	"CENTER.DegreeTypes.en";
#elif L==6
	"CENTER.DegreeTypes.es";
#elif L==7
	"CENTER.DegreeTypes.en";
#elif L==8
	"CENTER.DegreeTypes.en";
#elif L==9
	"CENTER.DegreeTypes.en";
#elif L==10	// tr
	"CENTER.DegreeTypes.en";
#endif

const char *Hlp_CENTER_DegreeTypes_edit =
#if   L==1
	"CENTER.DegreeTypes.es#editar";
#elif L==2
	"CENTER.DegreeTypes.en#edit";
#elif L==3
	"CENTER.DegreeTypes.en#edit";
#elif L==4
	"CENTER.DegreeTypes.es#editar";
#elif L==5
	"CENTER.DegreeTypes.en#edit";
#elif L==6
	"CENTER.DegreeTypes.es#editar";
#elif L==7
	"CENTER.DegreeTypes.en#edit";
#elif L==8
	"CENTER.DegreeTypes.en#edit";
#elif L==9
	"CENTER.DegreeTypes.en#edit";
#elif L==10	// tr
	"CENTER.DegreeTypes.en#edit";
#endif

const char *Hlp_CENTER_Degrees =
#if   L==1
	"CENTER.Degrees.es";
#elif L==2
	"CENTER.Degrees.en";
#elif L==3
	"CENTER.Degrees.en";
#elif L==4
	"CENTER.Degrees.es";
#elif L==5
	"CENTER.Degrees.en";
#elif L==6
	"CENTER.Degrees.es";
#elif L==7
	"CENTER.Degrees.en";
#elif L==8
	"CENTER.Degrees.en";
#elif L==9
	"CENTER.Degrees.en";
#elif L==10	// tr
	"CENTER.Degrees.en";
#endif

const char *Hlp_CENTER_Buildings =
#if   L==1
	"CENTER.Buildings.es";
#elif L==2
	"CENTER.Buildings.en";
#elif L==3
	"CENTER.Buildings.en";
#elif L==4
	"CENTER.Buildings.es";
#elif L==5
	"CENTER.Buildings.en";
#elif L==6
	"CENTER.Buildings.es";
#elif L==7
	"CENTER.Buildings.en";
#elif L==8
	"CENTER.Buildings.en";
#elif L==9
	"CENTER.Buildings.en";
#elif L==10	// tr
	"CENTER.Buildings.en";
#endif

const char *Hlp_CENTER_Buildings_edit =
#if   L==1
	"CENTER.Buildings.es#editar-edificios";
#elif L==2
	"CENTER.Buildings.en#edit-buildings";
#elif L==3
	"CENTER.Buildings.en#edit-buildings";
#elif L==4
	"CENTER.Buildings.es#editar-edificios";
#elif L==5
	"CENTER.Buildings.en#edit-buildings";
#elif L==6
	"CENTER.Buildings.es#editar-edificios";
#elif L==7
	"CENTER.Buildings.en#edit-buildings";
#elif L==8
	"CENTER.Buildings.en#edit-buildings";
#elif L==9
	"CENTER.Buildings.en#edit-buildings";
#elif L==10	// tr
	"CENTER.Buildings.en#edit-buildings";
#endif

const char *Hlp_CENTER_Rooms =
#if   L==1
	"CENTER.Rooms.es";
#elif L==2
	"CENTER.Rooms.en";
#elif L==3
	"CENTER.Rooms.en";
#elif L==4
	"CENTER.Rooms.es";
#elif L==5
	"CENTER.Rooms.en";
#elif L==6
	"CENTER.Rooms.es";
#elif L==7
	"CENTER.Rooms.en";
#elif L==8
	"CENTER.Rooms.en";
#elif L==9
	"CENTER.Rooms.en";
#elif L==10	// tr
	"CENTER.Rooms.en";
#endif

const char *Hlp_CENTER_Rooms_edit =
#if   L==1
	"CENTER.Rooms.es#editar-salas";
#elif L==2
	"CENTER.Rooms.en#edit-rooms";
#elif L==3
	"CENTER.Rooms.en#edit-rooms";
#elif L==4
	"CENTER.Rooms.es#editar-salas";
#elif L==5
	"CENTER.Rooms.en#edit-rooms";
#elif L==6
	"CENTER.Rooms.es#editar-salas";
#elif L==7
	"CENTER.Rooms.en#edit-rooms";
#elif L==8
	"CENTER.Rooms.en#edit-rooms";
#elif L==9
	"CENTER.Rooms.en#edit-rooms";
#elif L==10	// tr
	"CENTER.Rooms.en#edit-rooms";
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
#elif L==10	// tr
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
#elif L==10	// tr
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
#elif L==10	// tr
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
#elif L==10	// tr
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
#elif L==10	// tr
	"COURSE.Information.en#edit";
#endif

const char *Hlp_COURSE_Program =
#if   L==1
	"COURSE.Program.es";
#elif L==2
	"COURSE.Program.en";
#elif L==3
	"COURSE.Program.en";
#elif L==4
	"COURSE.Program.es";
#elif L==5
	"COURSE.Program.en";
#elif L==6
	"COURSE.Program.es";
#elif L==7
	"COURSE.Program.en";
#elif L==8
	"COURSE.Program.en";
#elif L==9
	"COURSE.Program.en";
#elif L==10	// tr
	"COURSE.Program.en";
#endif

const char *Hlp_COURSE_Program_resource_clipboard =
#if   L==1
	"COURSE.Program.en#resource-clipboard";
#elif L==2
	"COURSE.Program.en#resource-clipboard";
#elif L==3
	"COURSE.Program.en#resource-clipboard";
#elif L==4
	"COURSE.Program.es#portapapeles-de-recursos";
#elif L==5
	"COURSE.Program.en#resource-clipboard";
#elif L==6
	"COURSE.Program.es#portapapeles-de-recursos";
#elif L==7
	"COURSE.Program.en#resource-clipboard";
#elif L==8
	"COURSE.Program.en#resource-clipboard";
#elif L==9
	"COURSE.Program.en#resource-clipboard";
#elif L==10	// tr
	"COURSE.Program.en#resource-clipboard";
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
#elif L==10	// tr
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
#elif L==10	// tr
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
#elif L==10	// tr
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
#elif L==10	// tr
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
#elif L==10	// tr
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
#elif L==10	// tr
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
#elif L==10	// tr
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
#elif L==10	// tr
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
#elif L==10	// tr
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
#elif L==10	// tr
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
#elif L==10	// tr
	"COURSE.Links.en#edit";
#endif

const char *Hlp_COURSE_Assessment =
#if   L==1
	"COURSE.Assessment.es";
#elif L==2
	"COURSE.Assessment.en";
#elif L==3
	"COURSE.Assessment.en";
#elif L==4
	"COURSE.Assessment.es";
#elif L==5
	"COURSE.Assessment.en";
#elif L==6
	"COURSE.Assessment.es";
#elif L==7
	"COURSE.Assessment.en";
#elif L==8
	"COURSE.Assessment.en";
#elif L==9
	"COURSE.Assessment.en";
#elif L==10	// tr
	"COURSE.Assessment.en";
#endif

const char *Hlp_COURSE_Assessment_edit =
#if   L==1
	"COURSE.Assessment.es#editar";
#elif L==2
	"COURSE.Assessment.en#edit";
#elif L==3
	"COURSE.Assessment.en#edit";
#elif L==4
	"COURSE.Assessment.es#editar";
#elif L==5
	"COURSE.Assessment.en#edit";
#elif L==6
	"COURSE.Assessment.es#editar";
#elif L==7
	"COURSE.Assessment.en#edit";
#elif L==8
	"COURSE.Assessment.en#edit";
#elif L==9
	"COURSE.Assessment.en#edit";
#elif L==10	// tr
	"COURSE.Assessment.en#edit";
#endif

/***** ASSESSMENT tab *****/

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
#elif L==10	// tr
	"ASSESSMENT.Assignments.en";
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
#elif L==10	// tr
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
#elif L==10	// tr
	"ASSESSMENT.Projects.en";
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
#elif L==10	// tr
	"ASSESSMENT.Projects.en#edit-project";
#endif

const char *Hlp_ASSESSMENT_Projects_add_user =
#if   L==1
	"ASSESSMENT.Projects.es#add-user";	// TODO: Link in Spanish
#elif L==2
	"ASSESSMENT.Projects.en#add-user";
#elif L==3
	"ASSESSMENT.Projects.en#add-user";
#elif L==4
	"ASSESSMENT.Projects.es#add-user";	// TODO: Link in Spanish
#elif L==5
	"ASSESSMENT.Projects.en#add-user";
#elif L==6
	"ASSESSMENT.Projects.es#add-user";	// TODO: Link in Spanish
#elif L==7
	"ASSESSMENT.Projects.en#add-user";
#elif L==8
	"ASSESSMENT.Projects.en#add-user";
#elif L==9
	"ASSESSMENT.Projects.en#add-user";
#elif L==10	// tr
	"ASSESSMENT.Projects.en#add-user";
#endif

const char *Hlp_ASSESSMENT_Calls_for_exams =
#if   L==1
	"ASSESSMENT.Calls_for_exams.es";
#elif L==2
	"ASSESSMENT.Calls_for_exams.en";
#elif L==3
	"ASSESSMENT.Calls_for_exams.en";
#elif L==4
	"ASSESSMENT.Calls_for_exams.es";
#elif L==5
	"ASSESSMENT.Calls_for_exams.en";
#elif L==6
	"ASSESSMENT.Calls_for_exams.es";
#elif L==7
	"ASSESSMENT.Calls_for_exams.en";
#elif L==8
	"ASSESSMENT.Calls_for_exams.en";
#elif L==9
	"ASSESSMENT.Calls_for_exams.en";
#elif L==10	// tr
	"ASSESSMENT.Calls_for_exams.en";
#endif

const char *Hlp_ASSESSMENT_Calls_for_exams_new_call =
#if   L==1
	"ASSESSMENT.Calls_for_exams.es#nueva-convocatoria";
#elif L==2
	"ASSESSMENT.Calls_for_exams.en#new-call";
#elif L==3
	"ASSESSMENT.Calls_for_exams.en#new-call";
#elif L==4
	"ASSESSMENT.Calls_for_exams.es#nueva-convocatoria";
#elif L==5
	"ASSESSMENT.Calls_for_exams.en#new-call";
#elif L==6
	"ASSESSMENT.Calls_for_exams.es#nueva-convocatoria";
#elif L==7
	"ASSESSMENT.Calls_for_exams.en#new-call";
#elif L==8
	"ASSESSMENT.Calls_for_exams.en#new-call";
#elif L==9
	"ASSESSMENT.Calls_for_exams.en#new-call";
#elif L==10	// tr
	"ASSESSMENT.Calls_for_exams.en#new-call";
#endif

const char *Hlp_ASSESSMENT_Calls_for_exams_edit_call =
#if   L==1
	"ASSESSMENT.Calls_for_exams.es#editar-convocatoria";
#elif L==2
	"ASSESSMENT.Calls_for_exams.en#edit-call";
#elif L==3
	"ASSESSMENT.Calls_for_exams.en#edit-call";
#elif L==4
	"ASSESSMENT.Calls_for_exams.es#editar-convocatoria";
#elif L==5
	"ASSESSMENT.Calls_for_exams.en#edit-call";
#elif L==6
	"ASSESSMENT.Calls_for_exams.es#editar-convocatoria";
#elif L==7
	"ASSESSMENT.Calls_for_exams.en#edit-call";
#elif L==8
	"ASSESSMENT.Calls_for_exams.en#edit-call";
#elif L==9
	"ASSESSMENT.Calls_for_exams.en#edit-call";
#elif L==10	// tr
	"ASSESSMENT.Calls_for_exams.en#edit-call";
#endif

const char *Hlp_ASSESSMENT_Questions =
#if   L==1
	"ASSESSMENT.Questions.es";
#elif L==2
	"ASSESSMENT.Questions.en";
#elif L==3
	"ASSESSMENT.Questions.en";
#elif L==4
	"ASSESSMENT.Questions.es";
#elif L==5
	"ASSESSMENT.Questions.en";
#elif L==6
	"ASSESSMENT.Questions.es";
#elif L==7
	"ASSESSMENT.Questions.en";
#elif L==8
	"ASSESSMENT.Questions.en";
#elif L==9
	"ASSESSMENT.Questions.en";
#elif L==10	// tr
	"ASSESSMENT.Questions.en";
#endif

const char *Hlp_ASSESSMENT_Questions_writing_a_question =
#if   L==1
	"ASSESSMENT.Questions.es#escribir-una-pregunta";
#elif L==2
	"ASSESSMENT.Questions.en#writing-a-question";
#elif L==3
	"ASSESSMENT.Questions.en#writing-a-question";
#elif L==4
	"ASSESSMENT.Questions.es#escribir-una-pregunta";
#elif L==5
	"ASSESSMENT.Questions.en#writing-a-question";
#elif L==6
	"ASSESSMENT.Questions.es#escribir-una-pregunta";
#elif L==7
	"ASSESSMENT.Questions.en#writing-a-question";
#elif L==8
	"ASSESSMENT.Questions.en#writing-a-question";
#elif L==9
	"ASSESSMENT.Questions.en#writing-a-question";
#elif L==10	// tr
	"ASSESSMENT.Questions.en#writing-a-question";
#endif

const char *Hlp_ASSESSMENT_Questions_editing_tags =
#if   L==1
	"ASSESSMENT.Questions.es#editar-descriptores";
#elif L==2
	"ASSESSMENT.Questions.en#editing-tags";
#elif L==3
	"ASSESSMENT.Questions.en#editing-tags";
#elif L==4
	"ASSESSMENT.Questions.es#editar-descriptores";
#elif L==5
	"ASSESSMENT.Questions.en#editing-tags";
#elif L==6
	"ASSESSMENT.Questions.es#editar-descriptores";
#elif L==7
	"ASSESSMENT.Questions.en#editing-tags";
#elif L==8
	"ASSESSMENT.Questions.en#editing-tags";
#elif L==9
	"ASSESSMENT.Questions.en#editing-tags";
#elif L==10	// tr
	"ASSESSMENT.Questions.en#editing-tags";
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
#elif L==10	// tr
	"ASSESSMENT.Tests.en";
#endif

const char *Hlp_ASSESSMENT_Tests_configuring_tests =
#if   L==1
	"ASSESSMENT.Tests.es#configurar-test";
#elif L==2
	"ASSESSMENT.Tests.en#configuring-tests";
#elif L==3
	"ASSESSMENT.Tests.en#configuring-tests";
#elif L==4
	"ASSESSMENT.Tests.es#configurar-test";
#elif L==5
	"ASSESSMENT.Tests.en#configuring-tests";
#elif L==6
	"ASSESSMENT.Tests.es#configurar-test";
#elif L==7
	"ASSESSMENT.Tests.en#configuring-tests";
#elif L==8
	"ASSESSMENT.Tests.en#configuring-tests";
#elif L==9
	"ASSESSMENT.Tests.en#configuring-tests";
#elif L==10	// tr
	"ASSESSMENT.Tests.en#configuring-tests";
#endif

const char *Hlp_ASSESSMENT_Tests_results =
#if   L==1
	"ASSESSMENT.Tests.es#resultados";
#elif L==2
	"ASSESSMENT.Tests.en#results";
#elif L==3
	"ASSESSMENT.Tests.en#results";
#elif L==4
	"ASSESSMENT.Tests.es#resultados";
#elif L==5
	"ASSESSMENT.Tests.en#results";
#elif L==6
	"ASSESSMENT.Tests.es#resultados";
#elif L==7
	"ASSESSMENT.Tests.en#results";
#elif L==8
	"ASSESSMENT.Tests.en#results";
#elif L==9
	"ASSESSMENT.Tests.en#results";
#elif L==10	// tr
	"ASSESSMENT.Tests.en#results";
#endif

const char *Hlp_ASSESSMENT_Exams =
#if   L==1
	"ASSESSMENT.Exams.es";
#elif L==2
	"ASSESSMENT.Exams.en";
#elif L==3
	"ASSESSMENT.Exams.en";
#elif L==4
	"ASSESSMENT.Exams.es";
#elif L==5
	"ASSESSMENT.Exams.en";
#elif L==6
	"ASSESSMENT.Exams.es";
#elif L==7
	"ASSESSMENT.Exams.en";
#elif L==8
	"ASSESSMENT.Exams.en";
#elif L==9
	"ASSESSMENT.Exams.en";
#elif L==10	// tr
	"ASSESSMENT.Exams.en";
#endif

const char *Hlp_ASSESSMENT_Exams_edit_exam =
#if   L==1
	"ASSESSMENT.Exams.es#editar-examen";
#elif L==2
	"ASSESSMENT.Exams.en#edit-exam";
#elif L==3
	"ASSESSMENT.Exams.en#edit-exam";
#elif L==4
	"ASSESSMENT.Exams.es#editar-examen";
#elif L==5
	"ASSESSMENT.Exams.en#edit-exam";
#elif L==6
	"ASSESSMENT.Exams.es#editar-examen";
#elif L==7
	"ASSESSMENT.Exams.en#edit-exam";
#elif L==8
	"ASSESSMENT.Exams.en#edit-exam";
#elif L==9
	"ASSESSMENT.Exams.en#edit-exam";
#elif L==10	// tr
	"ASSESSMENT.Exams.en#edit-exam";
#endif

const char *Hlp_ASSESSMENT_Exams_new_exam =
#if   L==1
	"ASSESSMENT.Exams.es#nuevo-examen";
#elif L==2
	"ASSESSMENT.Exams.en#new-exam";
#elif L==3
	"ASSESSMENT.Exams.en#new-exam";
#elif L==4
	"ASSESSMENT.Exams.es#nuevo-examen";
#elif L==5
	"ASSESSMENT.Exams.en#new-exam";
#elif L==6
	"ASSESSMENT.Exams.es#nuevo-examen";
#elif L==7
	"ASSESSMENT.Exams.en#new-exam";
#elif L==8
	"ASSESSMENT.Exams.en#new-exam";
#elif L==9
	"ASSESSMENT.Exams.en#new-exam";
#elif L==10	// tr
	"ASSESSMENT.Exams.en#new-exam";
#endif

const char *Hlp_ASSESSMENT_Exams_question_sets =
#if   L==1
	"ASSESSMENT.Exams.es#conjuntos-de-preguntas";
#elif L==2
	"ASSESSMENT.Exams.en#question-sets";
#elif L==3
	"ASSESSMENT.Exams.en#question-sets";
#elif L==4
	"ASSESSMENT.Exams.es#conjuntos-de-preguntas";
#elif L==5
	"ASSESSMENT.Exams.en#question-sets";
#elif L==6
	"ASSESSMENT.Exams.es#conjuntos-de-preguntas";
#elif L==7
	"ASSESSMENT.Exams.en#question-sets";
#elif L==8
	"ASSESSMENT.Exams.en#question-sets";
#elif L==9
	"ASSESSMENT.Exams.en#question-sets";
#elif L==10	// tr
	"ASSESSMENT.Exams.en#question-sets";
#endif

const char *Hlp_ASSESSMENT_Exams_sessions =
#if   L==1
	"ASSESSMENT.Exams.es#sesiones";
#elif L==2
	"ASSESSMENT.Exams.en#sessions";
#elif L==3
	"ASSESSMENT.Exams.en#sessions";
#elif L==4
	"ASSESSMENT.Exams.es#sesiones";
#elif L==5
	"ASSESSMENT.Exams.en#sessions";
#elif L==6
	"ASSESSMENT.Exams.es#sesiones";
#elif L==7
	"ASSESSMENT.Exams.en#sessions";
#elif L==8
	"ASSESSMENT.Exams.en#sessions";
#elif L==9
	"ASSESSMENT.Exams.en#sessions";
#elif L==10	// tr
	"ASSESSMENT.Exams.en#sessions";
#endif

const char *Hlp_ASSESSMENT_Exams_questions =
#if   L==1
	"ASSESSMENT.Exams.es#preguntas";
#elif L==2
	"ASSESSMENT.Exams.en#questions";
#elif L==3
	"ASSESSMENT.Exams.en#questions";
#elif L==4
	"ASSESSMENT.Exams.es#preguntas";
#elif L==5
	"ASSESSMENT.Exams.en#questions";
#elif L==6
	"ASSESSMENT.Exams.es#preguntas";
#elif L==7
	"ASSESSMENT.Exams.en#questions";
#elif L==8
	"ASSESSMENT.Exams.en#questions";
#elif L==9
	"ASSESSMENT.Exams.en#questions";
#elif L==10	// tr
	"ASSESSMENT.Exams.en#questions";
#endif

const char *Hlp_ASSESSMENT_Exams_answer_exam =
#if   L==1
	"ASSESSMENT.Exams.es#responder-examen";
#elif L==2
	"ASSESSMENT.Exams.en#answer-exam";
#elif L==3
	"ASSESSMENT.Exams.en#answer-exam";
#elif L==4
	"ASSESSMENT.Exams.es#responder-examen";
#elif L==5
	"ASSESSMENT.Exams.en#answer-exam";
#elif L==6
	"ASSESSMENT.Exams.es#responder-examen";
#elif L==7
	"ASSESSMENT.Exams.en#answer-exam";
#elif L==8
	"ASSESSMENT.Exams.en#answer-exam";
#elif L==9
	"ASSESSMENT.Exams.en#answer-exam";
#elif L==10	// tr
	"ASSESSMENT.Exams.en#answer-exam";
#endif

const char *Hlp_ASSESSMENT_Exams_results =
#if   L==1
	"ASSESSMENT.Exams.es#resultados";
#elif L==2
	"ASSESSMENT.Exams.en#results";
#elif L==3
	"ASSESSMENT.Exams.en#results";
#elif L==4
	"ASSESSMENT.Exams.es#resultados";
#elif L==5
	"ASSESSMENT.Exams.en#results";
#elif L==6
	"ASSESSMENT.Exams.es#resultados";
#elif L==7
	"ASSESSMENT.Exams.en#results";
#elif L==8
	"ASSESSMENT.Exams.en#results";
#elif L==9
	"ASSESSMENT.Exams.en#results";
#elif L==10	// tr
	"ASSESSMENT.Exams.en#results";
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
#elif L==10	// tr
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
#elif L==10	// tr
	"ASSESSMENT.Games.en#edit-game";
#endif

const char *Hlp_ASSESSMENT_Games_new_game =
#if   L==1
	"ASSESSMENT.Games.es#nuevo-juego";
#elif L==2
	"ASSESSMENT.Games.en#new-game";
#elif L==3
	"ASSESSMENT.Games.en#new-game";
#elif L==4
	"ASSESSMENT.Games.es#nuevo-juego";
#elif L==5
	"ASSESSMENT.Games.en#new-game";
#elif L==6
	"ASSESSMENT.Games.es#nuevo-juego";
#elif L==7
	"ASSESSMENT.Games.en#new-game";
#elif L==8
	"ASSESSMENT.Games.en#new-game";
#elif L==9
	"ASSESSMENT.Games.en#new-game";
#elif L==10	// tr
	"ASSESSMENT.Games.en#new-game";
#endif

const char *Hlp_ASSESSMENT_Games_matches =
#if   L==1
	"ASSESSMENT.Games.es#partidas";
#elif L==2
	"ASSESSMENT.Games.en#matches";
#elif L==3
	"ASSESSMENT.Games.en#matches";
#elif L==4
	"ASSESSMENT.Games.es#partidas";
#elif L==5
	"ASSESSMENT.Games.en#matches";
#elif L==6
	"ASSESSMENT.Games.es#partidas";
#elif L==7
	"ASSESSMENT.Games.en#matches";
#elif L==8
	"ASSESSMENT.Games.en#matches";
#elif L==9
	"ASSESSMENT.Games.en#matches";
#elif L==10	// tr
	"ASSESSMENT.Games.en#matches";
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
#elif L==10	// tr
	"ASSESSMENT.Games.en#questions";
#endif

const char *Hlp_ASSESSMENT_Games_results =
#if   L==1
	"ASSESSMENT.Games.es#resultados";
#elif L==2
	"ASSESSMENT.Games.en#results";
#elif L==3
	"ASSESSMENT.Games.en#results";
#elif L==4
	"ASSESSMENT.Games.es#resultados";
#elif L==5
	"ASSESSMENT.Games.en#results";
#elif L==6
	"ASSESSMENT.Games.es#resultados";
#elif L==7
	"ASSESSMENT.Games.en#results";
#elif L==8
	"ASSESSMENT.Games.en#results";
#elif L==9
	"ASSESSMENT.Games.en#results";
#elif L==10	// tr
	"ASSESSMENT.Games.en#results";
#endif

const char *Hlp_ASSESSMENT_Rubrics =
#if   L==1
	"ASSESSMENT.Rubrics.es";
#elif L==2
	"ASSESSMENT.Rubrics.en";
#elif L==3
	"ASSESSMENT.Rubrics.en";
#elif L==4
	"ASSESSMENT.Rubrics.es";
#elif L==5
	"ASSESSMENT.Rubrics.en";
#elif L==6
	"ASSESSMENT.Rubrics.es";
#elif L==7
	"ASSESSMENT.Rubrics.en";
#elif L==8
	"ASSESSMENT.Rubrics.en";
#elif L==9
	"ASSESSMENT.Rubrics.en";
#elif L==10	// tr
	"ASSESSMENT.Rubrics.en";
#endif

const char *Hlp_ASSESSMENT_Rubrics_edit_rubric =
#if   L==1
	"ASSESSMENT.Rubrics.es#editar-r%C3%BAbrica";
#elif L==2
	"ASSESSMENT.Rubrics.en#edit-rubric";
#elif L==3
	"ASSESSMENT.Rubrics.en#edit-rubric";
#elif L==4
	"ASSESSMENT.Rubrics.es#editar-r%C3%BAbrica";
#elif L==5
	"ASSESSMENT.Rubrics.en#edit-rubric";
#elif L==6
	"ASSESSMENT.Rubrics.es#editar-r%C3%BAbrica";
#elif L==7
	"ASSESSMENT.Rubrics.en#edit-rubric";
#elif L==8
	"ASSESSMENT.Rubrics.en#edit-rubric";
#elif L==9
	"ASSESSMENT.Rubrics.en#edit-rubric";
#elif L==10	// tr
	"ASSESSMENT.Rubrics.en#edit-rubric";
#endif

const char *Hlp_ASSESSMENT_Rubrics_new_rubric =
#if   L==1
	"ASSESSMENT.Rubrics.es#nuevo-r%C3%BAbrica";
#elif L==2
	"ASSESSMENT.Rubrics.en#new-rubric";
#elif L==3
	"ASSESSMENT.Rubrics.en#new-rubric";
#elif L==4
	"ASSESSMENT.Rubrics.es#nueva-r%C3%BAbrica";
#elif L==5
	"ASSESSMENT.Rubrics.en#new-rubric";
#elif L==6
	"ASSESSMENT.Rubrics.es#nueva-r%C3%BAbrica";
#elif L==7
	"ASSESSMENT.Rubrics.en#new-rubric";
#elif L==8
	"ASSESSMENT.Rubrics.en#new-rubric";
#elif L==9
	"ASSESSMENT.Rubrics.en#new-rubric";
#elif L==10	// tr
	"ASSESSMENT.Rubrics.en#new-rubric";
#endif

const char *Hlp_ASSESSMENT_Rubrics_criteria =
#if   L==1
	"ASSESSMENT.Rubrics.es#criteria";
#elif L==2
	"ASSESSMENT.Rubrics.en#criteria";
#elif L==3
	"ASSESSMENT.Rubrics.en#criteria";
#elif L==4
	"ASSESSMENT.Rubrics.es#criterios";
#elif L==5
	"ASSESSMENT.Rubrics.en#criteria";
#elif L==6
	"ASSESSMENT.Rubrics.es#criterios";
#elif L==7
	"ASSESSMENT.Rubrics.en#criteria";
#elif L==8
	"ASSESSMENT.Rubrics.en#criteria";
#elif L==9
	"ASSESSMENT.Rubrics.en#criteria";
#elif L==10	// tr
	"ASSESSMENT.Rubrics.en#criteria";
#endif

const char *Hlp_ASSESSMENT_Rubrics_resource_clipboard =
#if   L==1
	"ASSESSMENT.Rubrics.es#resource-clipboard";
#elif L==2
	"ASSESSMENT.Rubrics.en#resource-clipboard";
#elif L==3
	"ASSESSMENT.Rubrics.en#resource-clipboard";
#elif L==4
	"ASSESSMENT.Rubrics.es#portapapeles-de-recursos";
#elif L==5
	"ASSESSMENT.Rubrics.en#resource-clipboard";
#elif L==6
	"ASSESSMENT.Rubrics.es#portapapeles-de-recursos";
#elif L==7
	"ASSESSMENT.Rubrics.en#resource-clipboard";
#elif L==8
	"ASSESSMENT.Rubrics.en#resource-clipboard";
#elif L==9
	"ASSESSMENT.Rubrics.en#resource-clipboard";
#elif L==10	// tr
	"ASSESSMENT.Rubrics.en#resource-clipboard";
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
#elif L==10	// tr
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
#elif L==10	// tr
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
#elif L==10	// tr
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
#elif L==10	// tr
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
#elif L==10	// tr
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
#elif L==10	// tr
	"FILES.Marks.en";
#endif

const char *Hlp_FILES_Briefcase =
#if   L==1
	"FILES.Briefcase.es";
#elif L==2
	"FILES.Briefcase.en";
#elif L==3
	"FILES.Briefcase.en";
#elif L==4
	"FILES.Briefcase.es";
#elif L==5
	"FILES.Briefcase.en";
#elif L==6
	"FILES.Briefcase.es";
#elif L==7
	"FILES.Briefcase.en";
#elif L==8
	"FILES.Briefcase.en";
#elif L==9
	"FILES.Briefcase.en";
#elif L==10	// tr
	"FILES.Briefcase.en";
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
#elif L==10	// tr
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
#elif L==10	// tr
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
#elif L==10	// tr
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
#elif L==10	// tr
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
#elif L==10	// tr
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
#elif L==10	// tr
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
#elif L==10	// tr
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
#elif L==10	// tr
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
#elif L==10	// tr
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
#elif L==10	// tr
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
#elif L==10	// tr
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
#elif L==10	// tr
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
#elif L==10	// tr
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
#elif L==10	// tr
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
#elif L==10	// tr
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
#elif L==10	// tr
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
#elif L==10	// tr
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
#elif L==10	// tr
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
#elif L==10	// tr
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
#elif L==10	// tr
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
#elif L==10	// tr
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
#elif L==10	// tr
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
#elif L==10	// tr
	"USERS.Connected.en#last-clicks";
#endif

/***** COMMUNICATION tab *****/

const char *Hlp_COMMUNICATION_Announcements =
#if   L==1
	"COMMUNICATION.Announcements.es";
#elif L==2
	"COMMUNICATION.Announcements.en";
#elif L==3
	"COMMUNICATION.Announcements.en";
#elif L==4
	"COMMUNICATION.Announcements.es";
#elif L==5
	"COMMUNICATION.Announcements.en";
#elif L==6
	"COMMUNICATION.Announcements.es";
#elif L==7
	"COMMUNICATION.Announcements.en";
#elif L==8
	"COMMUNICATION.Announcements.en";
#elif L==9
	"COMMUNICATION.Announcements.en";
#elif L==10	// tr
	"COMMUNICATION.Announcements.en";
#endif

const char *Hlp_COMMUNICATION_Notices =
#if   L==1
	"COMMUNICATION.Notices.es";
#elif L==2
	"COMMUNICATION.Notices.en";
#elif L==3
	"COMMUNICATION.Notices.en";
#elif L==4
	"COMMUNICATION.Notices.es";
#elif L==5
	"COMMUNICATION.Notices.en";
#elif L==6
	"COMMUNICATION.Notices.es";
#elif L==7
	"COMMUNICATION.Notices.en";
#elif L==8
	"COMMUNICATION.Notices.en";
#elif L==9
	"COMMUNICATION.Notices.en";
#elif L==10	// tr
	"COMMUNICATION.Notices.en";
#endif

const char *Hlp_COMMUNICATION_Forums =
#if   L==1
	"COMMUNICATION.Forums.es";
#elif L==2
	"COMMUNICATION.Forums.en";
#elif L==3
	"COMMUNICATION.Forums.en";
#elif L==4
	"COMMUNICATION.Forums.es";
#elif L==5
	"COMMUNICATION.Forums.en";
#elif L==6
	"COMMUNICATION.Forums.es";
#elif L==7
	"COMMUNICATION.Forums.en";
#elif L==8
	"COMMUNICATION.Forums.en";
#elif L==9
	"COMMUNICATION.Forums.en";
#elif L==10	// tr
	"COMMUNICATION.Forums.en";
#endif

const char *Hlp_COMMUNICATION_Forums_threads =
#if   L==1
	"COMMUNICATION.Forums.es#discusiones";
#elif L==2
	"COMMUNICATION.Forums.en#threads";
#elif L==3
	"COMMUNICATION.Forums.en#threads";
#elif L==4
	"COMMUNICATION.Forums.es#discusiones";
#elif L==5
	"COMMUNICATION.Forums.en#threads";
#elif L==6
	"COMMUNICATION.Forums.es#discusiones";
#elif L==7
	"COMMUNICATION.Forums.en#threads";
#elif L==8
	"COMMUNICATION.Forums.en#threads";
#elif L==9
	"COMMUNICATION.Forums.en#threads";
#elif L==10	// tr
	"COMMUNICATION.Forums.en#threads";
#endif

const char *Hlp_COMMUNICATION_Forums_new_thread =
#if   L==1
	"COMMUNICATION.Forums.es#nueva-discusi%C3%B3n";
#elif L==2
	"COMMUNICATION.Forums.en#new-thread";
#elif L==3
	"COMMUNICATION.Forums.en#new-thread";
#elif L==4
	"COMMUNICATION.Forums.es#nueva-discusi%C3%B3n";
#elif L==5
	"COMMUNICATION.Forums.en#new-thread";
#elif L==6
	"COMMUNICATION.Forums.es#nueva-discusi%C3%B3n";
#elif L==7
	"COMMUNICATION.Forums.en#new-thread";
#elif L==8
	"COMMUNICATION.Forums.en#new-thread";
#elif L==9
	"COMMUNICATION.Forums.en#new-thread";
#elif L==10	// tr
	"COMMUNICATION.Forums.en#new-thread";
#endif

const char *Hlp_COMMUNICATION_Forums_posts =
#if   L==1
	"COMMUNICATION.Forums.es#comentarios";
#elif L==2
	"COMMUNICATION.Forums.en#posts";
#elif L==3
	"COMMUNICATION.Forums.en#posts";
#elif L==4
	"COMMUNICATION.Forums.es#comentarios";
#elif L==5
	"COMMUNICATION.Forums.en#posts";
#elif L==6
	"COMMUNICATION.Forums.es#comentarios";
#elif L==7
	"COMMUNICATION.Forums.en#posts";
#elif L==8
	"COMMUNICATION.Forums.en#posts";
#elif L==9
	"COMMUNICATION.Forums.en#posts";
#elif L==10	// tr
	"COMMUNICATION.Forums.en#posts";
#endif

const char *Hlp_COMMUNICATION_Forums_new_post =
#if   L==1
	"COMMUNICATION.Forums.es#nuevo-comentario";
#elif L==2
	"COMMUNICATION.Forums.en#new-post";
#elif L==3
	"COMMUNICATION.Forums.en#new-post";
#elif L==4
	"COMMUNICATION.Forums.es#nuevo-comentario";
#elif L==5
	"COMMUNICATION.Forums.en#new-post";
#elif L==6
	"COMMUNICATION.Forums.es#nuevo-comentario";
#elif L==7
	"COMMUNICATION.Forums.en#new-post";
#elif L==8
	"COMMUNICATION.Forums.en#new-post";
#elif L==9
	"COMMUNICATION.Forums.en#new-post";
#elif L==10	// tr
	"COMMUNICATION.Forums.en#new-post";
#endif

const char *Hlp_COMMUNICATION_Messages_write =
#if   L==1
	"COMMUNICATION.Write.es";
#elif L==2
	"COMMUNICATION.Write.en";
#elif L==3
	"COMMUNICATION.Write.en";
#elif L==4
	"COMMUNICATION.Write.es";
#elif L==5
	"COMMUNICATION.Write.en";
#elif L==6
	"COMMUNICATION.Write.es";
#elif L==7
	"COMMUNICATION.Write.en";
#elif L==8
	"COMMUNICATION.Write.en";
#elif L==9
	"COMMUNICATION.Write.en";
#elif L==10	// tr
	"COMMUNICATION.Write.en";
#endif

const char *Hlp_COMMUNICATION_Messages_received =
#if   L==1
	"COMMUNICATION.Received.es";
#elif L==2
	"COMMUNICATION.Received.en";
#elif L==3
	"COMMUNICATION.Received.en";
#elif L==4
	"COMMUNICATION.Received.es";
#elif L==5
	"COMMUNICATION.Received.en";
#elif L==6
	"COMMUNICATION.Received.es";
#elif L==7
	"COMMUNICATION.Received.en";
#elif L==8
	"COMMUNICATION.Received.en";
#elif L==9
	"COMMUNICATION.Received.en";
#elif L==10	// tr
	"COMMUNICATION.Received.en";
#endif

const char *Hlp_COMMUNICATION_Messages_received_filter =
#if   L==1
	"COMMUNICATION.Received.es#filtro";
#elif L==2
	"COMMUNICATION.Received.en#filter";
#elif L==3
	"COMMUNICATION.Received.en#filter";
#elif L==4
	"COMMUNICATION.Received.es#filtro";
#elif L==5
	"COMMUNICATION.Received.en#filter";
#elif L==6
	"COMMUNICATION.Received.es#filtro";
#elif L==7
	"COMMUNICATION.Received.en#filter";
#elif L==8
	"COMMUNICATION.Received.en#filter";
#elif L==9
	"COMMUNICATION.Received.en#filter";
#elif L==10	// tr
	"COMMUNICATION.Received.en#filter";
#endif

const char *Hlp_COMMUNICATION_Messages_sent =
#if   L==1
	"COMMUNICATION.Sent.es";
#elif L==2
	"COMMUNICATION.Sent.en";
#elif L==3
	"COMMUNICATION.Sent.en";
#elif L==4
	"COMMUNICATION.Sent.es";
#elif L==5
	"COMMUNICATION.Sent.en";
#elif L==6
	"COMMUNICATION.Sent.es";
#elif L==7
	"COMMUNICATION.Sent.en";
#elif L==8
	"COMMUNICATION.Sent.en";
#elif L==9
	"COMMUNICATION.Sent.en";
#elif L==10	// tr
	"COMMUNICATION.Sent.en";
#endif

const char *Hlp_COMMUNICATION_Messages_sent_filter =
#if   L==1
	"COMMUNICATION.Sent.es#filtro";
#elif L==2
	"COMMUNICATION.Sent.en#filter";
#elif L==3
	"COMMUNICATION.Sent.en#filter";
#elif L==4
	"COMMUNICATION.Sent.es#filtro";
#elif L==5
	"COMMUNICATION.Sent.en#filter";
#elif L==6
	"COMMUNICATION.Sent.es#filtro";
#elif L==7
	"COMMUNICATION.Sent.en#filter";
#elif L==8
	"COMMUNICATION.Sent.en#filter";
#elif L==9
	"COMMUNICATION.Sent.en#filter";
#elif L==10	// tr
	"COMMUNICATION.Sent.en#filter";
#endif

const char *Hlp_COMMUNICATION_Email =
#if   L==1
	"COMMUNICATION.Email.es";
#elif L==2
	"COMMUNICATION.Email.en";
#elif L==3
	"COMMUNICATION.Email.en";
#elif L==4
	"COMMUNICATION.Email.es";
#elif L==5
	"COMMUNICATION.Email.en";
#elif L==6
	"COMMUNICATION.Email.es";
#elif L==7
	"COMMUNICATION.Email.en";
#elif L==8
	"COMMUNICATION.Email.en";
#elif L==9
	"COMMUNICATION.Email.en";
#elif L==10	// tr
	"COMMUNICATION.Email.en";
#endif

/***** ANALYTICS tab *****/

const char *Hlp_ANALYTICS_Figures =
#if   L==1
	"ANALYTICS.Figures.es";
#elif L==2
	"ANALYTICS.Figures.en";
#elif L==3
	"ANALYTICS.Figures.en";
#elif L==4
	"ANALYTICS.Figures.es";
#elif L==5
	"ANALYTICS.Figures.en";
#elif L==6
	"ANALYTICS.Figures.es";
#elif L==7
	"ANALYTICS.Figures.en";
#elif L==8
	"ANALYTICS.Figures.en";
#elif L==9
	"ANALYTICS.Figures.en";
#elif L==10	// tr
	"ANALYTICS.Figures.en";
#endif

const char *Hlp_ANALYTICS_Figures_users =
#if   L==1
	"ANALYTICS.Figures.es#usuarios";
#elif L==2
	"ANALYTICS.Figures.en#users";
#elif L==3
	"ANALYTICS.Figures.en#users";
#elif L==4
	"ANALYTICS.Figures.es#usuarios";
#elif L==5
	"ANALYTICS.Figures.en#users";
#elif L==6
	"ANALYTICS.Figures.es#usuarios";
#elif L==7
	"ANALYTICS.Figures.en#users";
#elif L==8
	"ANALYTICS.Figures.en#users";
#elif L==9
	"ANALYTICS.Figures.en#users";
#elif L==10	// tr
	"ANALYTICS.Figures.en#users";
#endif

const char *Hlp_ANALYTICS_Figures_ranking =
#if   L==1
	"ANALYTICS.Figures.es#ranking";
#elif L==2
	"ANALYTICS.Figures.en#ranking";
#elif L==3
	"ANALYTICS.Figures.en#ranking";
#elif L==4
	"ANALYTICS.Figures.es#ranking";
#elif L==5
	"ANALYTICS.Figures.en#ranking";
#elif L==6
	"ANALYTICS.Figures.es#ranking";
#elif L==7
	"ANALYTICS.Figures.en#ranking";
#elif L==8
	"ANALYTICS.Figures.en#ranking";
#elif L==9
	"ANALYTICS.Figures.en#ranking";
#elif L==10	// tr
	"ANALYTICS.Figures.en#ranking";
#endif

const char *Hlp_ANALYTICS_Figures_hierarchy =
#if   L==1
	"ANALYTICS.Figures.es#jerarqu%C3%ADa";
#elif L==2
	"ANALYTICS.Figures.en#hierarchy";
#elif L==3
	"ANALYTICS.Figures.en#hierarchy";
#elif L==4
	"ANALYTICS.Figures.es#jerarqu%C3%ADa";
#elif L==5
	"ANALYTICS.Figures.en#hierarchy";
#elif L==6
	"ANALYTICS.Figures.es#jerarqu%C3%ADa";
#elif L==7
	"ANALYTICS.Figures.en#hierarchy";
#elif L==8
	"ANALYTICS.Figures.en#hierarchy";
#elif L==9
	"ANALYTICS.Figures.en#hierarchy";
#elif L==10	// tr
	"ANALYTICS.Figures.en#hierarchy";
#endif

const char *Hlp_ANALYTICS_Figures_institutions =
#if   L==1
	"ANALYTICS.Figures.es#instituciones";
#elif L==2
	"ANALYTICS.Figures.en#institutions";
#elif L==3
	"ANALYTICS.Figures.en#institutions";
#elif L==4
	"ANALYTICS.Figures.es#instituciones";
#elif L==5
	"ANALYTICS.Figures.en#institutions";
#elif L==6
	"ANALYTICS.Figures.es#instituciones";
#elif L==7
	"ANALYTICS.Figures.en#institutions";
#elif L==8
	"ANALYTICS.Figures.en#institutions";
#elif L==9
	"ANALYTICS.Figures.en#institutions";
#elif L==10	// tr
	"ANALYTICS.Figures.en#institutions";
#endif

const char *Hlp_ANALYTICS_Figures_types_of_degree =
#if   L==1
	"ANALYTICS.Figures.es#tipos-de-titulaci%C3%B3n";
#elif L==2
	"ANALYTICS.Figures.en#types-of-degree";
#elif L==3
	"ANALYTICS.Figures.en#types-of-degree";
#elif L==4
	"ANALYTICS.Figures.es#tipos-de-titulaci%C3%B3n";
#elif L==5
	"ANALYTICS.Figures.en#types-of-degree";
#elif L==6
	"ANALYTICS.Figures.es#tipos-de-titulaci%C3%B3n";
#elif L==7
	"ANALYTICS.Figures.en#types-of-degree";
#elif L==8
	"ANALYTICS.Figures.en#types-of-degree";
#elif L==9
	"ANALYTICS.Figures.en#types-of-degree";
#elif L==10	// tr
	"ANALYTICS.Figures.en#types-of-degree";
#endif

const char *Hlp_ANALYTICS_Figures_folders_and_files =
#if   L==1
	"ANALYTICS.Figures.es#carpetas-y-archivos";
#elif L==2
	"ANALYTICS.Figures.en#folders-and-files";
#elif L==3
	"ANALYTICS.Figures.en#folders-and-files";
#elif L==4
	"ANALYTICS.Figures.es#carpetas-y-archivos";
#elif L==5
	"ANALYTICS.Figures.en#folders-and-files";
#elif L==6
	"ANALYTICS.Figures.es#carpetas-y-archivos";
#elif L==7
	"ANALYTICS.Figures.en#folders-and-files";
#elif L==8
	"ANALYTICS.Figures.en#folders-and-files";
#elif L==9
	"ANALYTICS.Figures.en#folders-and-files";
#elif L==10	// tr
	"ANALYTICS.Figures.en#folders-and-files";
#endif

const char *Hlp_ANALYTICS_Figures_open_educational_resources_oer =
#if   L==1
	"ANALYTICS.Figures.es#recursos-educativos-abiertos-oer";
#elif L==2
	"ANALYTICS.Figures.en#open-educational-resources-oer";
#elif L==3
	"ANALYTICS.Figures.en#open-educational-resources-oer";
#elif L==4
	"ANALYTICS.Figures.es#recursos-educativos-abiertos-oer";
#elif L==5
	"ANALYTICS.Figures.en#open-educational-resources-oer";
#elif L==6
	"ANALYTICS.Figures.es#recursos-educativos-abiertos-oer";
#elif L==7
	"ANALYTICS.Figures.en#open-educational-resources-oer";
#elif L==8
	"ANALYTICS.Figures.en#open-educational-resources-oer";
#elif L==9
	"ANALYTICS.Figures.en#open-educational-resources-oer";
#elif L==10	// tr
	"ANALYTICS.Figures.en#open-educational-resources-oer";
#endif

const char *Hlp_ANALYTICS_Figures_course_programs =
#if   L==1
	"ANALYTICS.Figures.es#programas-de-asignaturas";
#elif L==2
	"ANALYTICS.Figures.en#course-programs";
#elif L==3
	"ANALYTICS.Figures.en#course-programs";
#elif L==4
	"ANALYTICS.Figures.es#programas-de-asignaturas";
#elif L==5
	"ANALYTICS.Figures.en#course-programs";
#elif L==6
	"ANALYTICS.Figures.es#programas-de-asignaturas";
#elif L==7
	"ANALYTICS.Figures.en#course-programs";
#elif L==8
	"ANALYTICS.Figures.en#course-programs";
#elif L==9
	"ANALYTICS.Figures.en#course-programs";
#elif L==10	// tr
	"ANALYTICS.Figures.en#course-programs";
#endif

const char *Hlp_ANALYTICS_Figures_assignments =
#if   L==1
	"ANALYTICS.Figures.es#actividades";
#elif L==2
	"ANALYTICS.Figures.en#assignments";
#elif L==3
	"ANALYTICS.Figures.en#assignments";
#elif L==4
	"ANALYTICS.Figures.es#actividades";
#elif L==5
	"ANALYTICS.Figures.en#assignments";
#elif L==6
	"ANALYTICS.Figures.es#actividades";
#elif L==7
	"ANALYTICS.Figures.en#assignments";
#elif L==8
	"ANALYTICS.Figures.en#assignments";
#elif L==9
	"ANALYTICS.Figures.en#assignments";
#elif L==10	// tr
	"ANALYTICS.Figures.en#assignments";
#endif

const char *Hlp_ANALYTICS_Figures_projects =
#if   L==1
	"ANALYTICS.Figures.es#proyectos";
#elif L==2
	"ANALYTICS.Figures.en#projects";
#elif L==3
	"ANALYTICS.Figures.en#projects";
#elif L==4
	"ANALYTICS.Figures.es#proyectos";
#elif L==5
	"ANALYTICS.Figures.en#projects";
#elif L==6
	"ANALYTICS.Figures.es#proyectos";
#elif L==7
	"ANALYTICS.Figures.en#projects";
#elif L==8
	"ANALYTICS.Figures.en#projects";
#elif L==9
	"ANALYTICS.Figures.en#projects";
#elif L==10	// tr
	"ANALYTICS.Figures.en#projects";
#endif

const char *Hlp_ANALYTICS_Figures_tests =
#if   L==1
	"ANALYTICS.Figures.es#tests";
#elif L==2
	"ANALYTICS.Figures.en#tests";
#elif L==3
	"ANALYTICS.Figures.en#tests";
#elif L==4
	"ANALYTICS.Figures.es#tests";
#elif L==5
	"ANALYTICS.Figures.en#tests";
#elif L==6
	"ANALYTICS.Figures.es#tests";
#elif L==7
	"ANALYTICS.Figures.en#tests";
#elif L==8
	"ANALYTICS.Figures.en#tests";
#elif L==9
	"ANALYTICS.Figures.en#tests";
#elif L==10	// tr
	"ANALYTICS.Figures.en#tests";
#endif

const char *Hlp_ANALYTICS_Figures_exams =
#if   L==1
	"ANALYTICS.Figures.es#ex%C3%A1menes";
#elif L==2
	"ANALYTICS.Figures.en#exams";
#elif L==3
	"ANALYTICS.Figures.en#exams";
#elif L==4
	"ANALYTICS.Figures.es#ex%C3%A1menes";
#elif L==5
	"ANALYTICS.Figures.en#exams";
#elif L==6
	"ANALYTICS.Figures.es#ex%C3%A1menes";
#elif L==7
	"ANALYTICS.Figures.en#exams";
#elif L==8
	"ANALYTICS.Figures.en#exams";
#elif L==9
	"ANALYTICS.Figures.en#exams";
#elif L==10	// tr
	"ANALYTICS.Figures.en#exams";
#endif

const char *Hlp_ANALYTICS_Figures_games =
#if   L==1
	"ANALYTICS.Figures.es#juegos";
#elif L==2
	"ANALYTICS.Figures.en#games";
#elif L==3
	"ANALYTICS.Figures.en#games";
#elif L==4
	"ANALYTICS.Figures.es#juegos";
#elif L==5
	"ANALYTICS.Figures.en#games";
#elif L==6
	"ANALYTICS.Figures.es#juegos";
#elif L==7
	"ANALYTICS.Figures.en#games";
#elif L==8
	"ANALYTICS.Figures.en#games";
#elif L==9
	"ANALYTICS.Figures.en#games";
#elif L==10	// tr
	"ANALYTICS.Figures.en#games";
#endif

const char *Hlp_ANALYTICS_Figures_rubrics =
#if   L==1
	"ANALYTICS.Figures.es#rubricas";
#elif L==2
	"ANALYTICS.Figures.en#rubrics";
#elif L==3
	"ANALYTICS.Figures.en#rubrics";
#elif L==4
	"ANALYTICS.Figures.es#rubricas";
#elif L==5
	"ANALYTICS.Figures.en#rubrics";
#elif L==6
	"ANALYTICS.Figures.es#rubricas";
#elif L==7
	"ANALYTICS.Figures.en#rubrics";
#elif L==8
	"ANALYTICS.Figures.en#rubrics";
#elif L==9
	"ANALYTICS.Figures.en#rubrics";
#elif L==10	// tr
	"ANALYTICS.Figures.en#rubrics";
#endif

const char *Hlp_ANALYTICS_Figures_surveys =
#if   L==1
	"ANALYTICS.Figures.es#encuestas";
#elif L==2
	"ANALYTICS.Figures.en#surveys";
#elif L==3
	"ANALYTICS.Figures.en#surveys";
#elif L==4
	"ANALYTICS.Figures.es#encuestas";
#elif L==5
	"ANALYTICS.Figures.en#surveys";
#elif L==6
	"ANALYTICS.Figures.es#encuestas";
#elif L==7
	"ANALYTICS.Figures.en#surveys";
#elif L==8
	"ANALYTICS.Figures.en#surveys";
#elif L==9
	"ANALYTICS.Figures.en#surveys";
#elif L==10	// tr
	"ANALYTICS.Figures.en#surveys";
#endif

const char *Hlp_ANALYTICS_Figures_timeline =
#if   L==1
	"ANALYTICS.Figures.es#timeline";
#elif L==2
	"ANALYTICS.Figures.en#timeline";
#elif L==3
	"ANALYTICS.Figures.en#timeline";
#elif L==4
	"ANALYTICS.Figures.es#timeline";
#elif L==5
	"ANALYTICS.Figures.en#timeline";
#elif L==6
	"ANALYTICS.Figures.es#timeline";
#elif L==7
	"ANALYTICS.Figures.en#timeline";
#elif L==8
	"ANALYTICS.Figures.en#timeline";
#elif L==9
	"ANALYTICS.Figures.en#timeline";
#elif L==10	// tr
	"ANALYTICS.Figures.en#timeline";
#endif

const char *Hlp_ANALYTICS_Figures_followed_followers =
#if   L==1
	"ANALYTICS.Figures.es#seguidos--seguidores";
#elif L==2
	"ANALYTICS.Figures.en#followed--followers";
#elif L==3
	"ANALYTICS.Figures.en#followed--followers";
#elif L==4
	"ANALYTICS.Figures.es#seguidos--seguidores";
#elif L==5
	"ANALYTICS.Figures.en#followed--followers";
#elif L==6
	"ANALYTICS.Figures.es#seguidos--seguidores";
#elif L==7
	"ANALYTICS.Figures.en#followed--followers";
#elif L==8
	"ANALYTICS.Figures.en#followed--followers";
#elif L==9
	"ANALYTICS.Figures.en#followed--followers";
#elif L==10	// tr
	"ANALYTICS.Figures.en#followed--followers";
#endif

const char *Hlp_ANALYTICS_Figures_forums =
#if   L==1
	"ANALYTICS.Figures.es#foros";
#elif L==2
	"ANALYTICS.Figures.en#forums";
#elif L==3
	"ANALYTICS.Figures.en#forums";
#elif L==4
	"ANALYTICS.Figures.es#foros";
#elif L==5
	"ANALYTICS.Figures.en#forums";
#elif L==6
	"ANALYTICS.Figures.es#foros";
#elif L==7
	"ANALYTICS.Figures.en#forums";
#elif L==8
	"ANALYTICS.Figures.en#forums";
#elif L==9
	"ANALYTICS.Figures.en#forums";
#elif L==10	// tr
	"ANALYTICS.Figures.en#forums";
#endif

const char *Hlp_ANALYTICS_Figures_notifications =
#if   L==1
	"ANALYTICS.Figures.es#notificaciones";
#elif L==2
	"ANALYTICS.Figures.en#notifications";
#elif L==3
	"ANALYTICS.Figures.en#notifications";
#elif L==4
	"ANALYTICS.Figures.es#notificaciones";
#elif L==5
	"ANALYTICS.Figures.en#notifications";
#elif L==6
	"ANALYTICS.Figures.es#notificaciones";
#elif L==7
	"ANALYTICS.Figures.en#notifications";
#elif L==8
	"ANALYTICS.Figures.en#notifications";
#elif L==9
	"ANALYTICS.Figures.en#notifications";
#elif L==10	// tr
	"ANALYTICS.Figures.en#notifications";
#endif

const char *Hlp_ANALYTICS_Figures_notices =
#if   L==1
	"ANALYTICS.Figures.es#avisos";
#elif L==2
	"ANALYTICS.Figures.en#notices";
#elif L==3
	"ANALYTICS.Figures.en#notices";
#elif L==4
	"ANALYTICS.Figures.es#avisos";
#elif L==5
	"ANALYTICS.Figures.en#notices";
#elif L==6
	"ANALYTICS.Figures.es#avisos";
#elif L==7
	"ANALYTICS.Figures.en#notices";
#elif L==8
	"ANALYTICS.Figures.en#notices";
#elif L==9
	"ANALYTICS.Figures.en#notices";
#elif L==10	// tr
	"ANALYTICS.Figures.en#notices";
#endif

const char *Hlp_ANALYTICS_Figures_messages =
#if   L==1
	"ANALYTICS.Figures.es#mensajes";
#elif L==2
	"ANALYTICS.Figures.en#messages";
#elif L==3
	"ANALYTICS.Figures.en#messages";
#elif L==4
	"ANALYTICS.Figures.es#mensajes";
#elif L==5
	"ANALYTICS.Figures.en#messages";
#elif L==6
	"ANALYTICS.Figures.es#mensajes";
#elif L==7
	"ANALYTICS.Figures.en#messages";
#elif L==8
	"ANALYTICS.Figures.en#messages";
#elif L==9
	"ANALYTICS.Figures.en#messages";
#elif L==10	// tr
	"ANALYTICS.Figures.en#messages";
#endif

const char *Hlp_ANALYTICS_Figures_agendas =
#if   L==1
	"ANALYTICS.Figures.es#agendas";
#elif L==2
	"ANALYTICS.Figures.en#agendas";
#elif L==3
	"ANALYTICS.Figures.en#agendas";
#elif L==4
	"ANALYTICS.Figures.es#agendas";
#elif L==5
	"ANALYTICS.Figures.en#agendas";
#elif L==6
	"ANALYTICS.Figures.es#agendas";
#elif L==7
	"ANALYTICS.Figures.en#agendas";
#elif L==8
	"ANALYTICS.Figures.en#agendas";
#elif L==9
	"ANALYTICS.Figures.en#agendas";
#elif L==10	// tr
	"ANALYTICS.Figures.en#agendas";
#endif

const char *Hlp_ANALYTICS_Figures_webs_social_networks =
#if   L==1
	"ANALYTICS.Figures.es#webs--redes-sociales";
#elif L==2
	"ANALYTICS.Figures.en#webs--social-networks";
#elif L==3
	"ANALYTICS.Figures.en#webs--social-networks";
#elif L==4
	"ANALYTICS.Figures.es#webs--redes-sociales";
#elif L==5
	"ANALYTICS.Figures.en#webs--social-networks";
#elif L==6
	"ANALYTICS.Figures.es#webs--redes-sociales";
#elif L==7
	"ANALYTICS.Figures.en#webs--social-networks";
#elif L==8
	"ANALYTICS.Figures.en#webs--social-networks";
#elif L==9
	"ANALYTICS.Figures.en#webs--social-networks";
#elif L==10	// tr
	"ANALYTICS.Figures.en#webs--social-networks";
#endif

const char *Hlp_ANALYTICS_Figures_language =
#if   L==1
	"ANALYTICS.Figures.es#idioma";
#elif L==2
	"ANALYTICS.Figures.en#language";
#elif L==3
	"ANALYTICS.Figures.en#language";
#elif L==4
	"ANALYTICS.Figures.es#idioma";
#elif L==5
	"ANALYTICS.Figures.en#language";
#elif L==6
	"ANALYTICS.Figures.es#idioma";
#elif L==7
	"ANALYTICS.Figures.en#language";
#elif L==8
	"ANALYTICS.Figures.en#language";
#elif L==9
	"ANALYTICS.Figures.en#language";
#elif L==10	// tr
	"ANALYTICS.Figures.en#language";
#endif

const char *Hlp_ANALYTICS_Figures_calendar =
#if   L==1
	"ANALYTICS.Figures.es#calendario";
#elif L==2
	"ANALYTICS.Figures.en#calendar";
#elif L==3
	"ANALYTICS.Figures.en#calendar";
#elif L==4
	"ANALYTICS.Figures.es#calendario";
#elif L==5
	"ANALYTICS.Figures.en#calendar";
#elif L==6
	"ANALYTICS.Figures.es#calendario";
#elif L==7
	"ANALYTICS.Figures.en#calendar";
#elif L==8
	"ANALYTICS.Figures.en#calendar";
#elif L==9
	"ANALYTICS.Figures.en#calendar";
#elif L==10	// tr
	"ANALYTICS.Figures.en#calendar";
#endif

const char *Hlp_ANALYTICS_Figures_dates =
#if   L==1
	"ANALYTICS.Figures.es#fechas";
#elif L==2
	"ANALYTICS.Figures.en#dates";
#elif L==3
	"ANALYTICS.Figures.en#dates";
#elif L==4
	"ANALYTICS.Figures.es#fechas";
#elif L==5
	"ANALYTICS.Figures.en#dates";
#elif L==6
	"ANALYTICS.Figures.es#fechas";
#elif L==7
	"ANALYTICS.Figures.en#dates";
#elif L==8
	"ANALYTICS.Figures.en#dates";
#elif L==9
	"ANALYTICS.Figures.en#dates";
#elif L==10	// tr
	"ANALYTICS.Figures.en#dates";
#endif

const char *Hlp_ANALYTICS_Figures_icons =
#if   L==1
	"ANALYTICS.Figures.es#iconos";
#elif L==2
	"ANALYTICS.Figures.en#icons";
#elif L==3
	"ANALYTICS.Figures.en#icons";
#elif L==4
	"ANALYTICS.Figures.es#iconos";
#elif L==5
	"ANALYTICS.Figures.en#icons";
#elif L==6
	"ANALYTICS.Figures.es#iconos";
#elif L==7
	"ANALYTICS.Figures.en#icons";
#elif L==8
	"ANALYTICS.Figures.en#icons";
#elif L==9
	"ANALYTICS.Figures.en#icons";
#elif L==10	// tr
	"ANALYTICS.Figures.en#icons";
#endif

const char *Hlp_ANALYTICS_Figures_menu =
#if   L==1
	"ANALYTICS.Figures.es#men%C3%BA";
#elif L==2
	"ANALYTICS.Figures.en#menu";
#elif L==3
	"ANALYTICS.Figures.en#menu";
#elif L==4
	"ANALYTICS.Figures.es#men%C3%BA";
#elif L==5
	"ANALYTICS.Figures.en#menu";
#elif L==6
	"ANALYTICS.Figures.es#men%C3%BA";
#elif L==7
	"ANALYTICS.Figures.en#menu";
#elif L==8
	"ANALYTICS.Figures.en#menu";
#elif L==9
	"ANALYTICS.Figures.en#menu";
#elif L==10	// tr
	"ANALYTICS.Figures.en#menu";
#endif

const char *Hlp_ANALYTICS_Figures_theme =
#if   L==1
	"ANALYTICS.Figures.es#tema";
#elif L==2
	"ANALYTICS.Figures.en#theme";
#elif L==3
	"ANALYTICS.Figures.en#theme";
#elif L==4
	"ANALYTICS.Figures.es#tema";
#elif L==5
	"ANALYTICS.Figures.en#theme";
#elif L==6
	"ANALYTICS.Figures.es#tema";
#elif L==7
	"ANALYTICS.Figures.en#theme";
#elif L==8
	"ANALYTICS.Figures.en#theme";
#elif L==9
	"ANALYTICS.Figures.en#theme";
#elif L==10	// tr
	"ANALYTICS.Figures.en#theme";
#endif

const char *Hlp_ANALYTICS_Figures_columns =
#if   L==1
	"ANALYTICS.Figures.es#columnas";
#elif L==2
	"ANALYTICS.Figures.en#columns";
#elif L==3
	"ANALYTICS.Figures.en#columns";
#elif L==4
	"ANALYTICS.Figures.es#columnas";
#elif L==5
	"ANALYTICS.Figures.en#columns";
#elif L==6
	"ANALYTICS.Figures.es#columnas";
#elif L==7
	"ANALYTICS.Figures.en#columns";
#elif L==8
	"ANALYTICS.Figures.en#columns";
#elif L==9
	"ANALYTICS.Figures.en#columns";
#elif L==10	// tr
	"ANALYTICS.Figures.en#columns";
#endif

const char *Hlp_ANALYTICS_Figures_user_photos =
#if   L==1
	"ANALYTICS.Figures.es#fotos-de-usuarios";
#elif L==2
	"ANALYTICS.Figures.en#user-photos";
#elif L==3
	"ANALYTICS.Figures.en#user-photos";
#elif L==4
	"ANALYTICS.Figures.es#fotos-de-usuarios";
#elif L==5
	"ANALYTICS.Figures.en#user-photos";
#elif L==6
	"ANALYTICS.Figures.es#fotos-de-usuarios";
#elif L==7
	"ANALYTICS.Figures.en#user-photos";
#elif L==8
	"ANALYTICS.Figures.en#user-photos";
#elif L==9
	"ANALYTICS.Figures.en#user-photos";
#elif L==10	// tr
	"ANALYTICS.Figures.en#user-photos";
#endif

const char *Hlp_ANALYTICS_Figures_privacy =
#if   L==1
	"ANALYTICS.Figures.es#privacidad";
#elif L==2
	"ANALYTICS.Figures.en#privacy";
#elif L==3
	"ANALYTICS.Figures.en#privacy";
#elif L==4
	"ANALYTICS.Figures.es#privacidad";
#elif L==5
	"ANALYTICS.Figures.en#privacy";
#elif L==6
	"ANALYTICS.Figures.es#privacidad";
#elif L==7
	"ANALYTICS.Figures.en#privacy";
#elif L==8
	"ANALYTICS.Figures.en#privacy";
#elif L==9
	"ANALYTICS.Figures.en#privacy";
#elif L==10	// tr
	"ANALYTICS.Figures.en#privacy";
#endif

const char *Hlp_ANALYTICS_Figures_cookies =
#if   L==1
	"ANALYTICS.Figures.es#cookies";
#elif L==2
	"ANALYTICS.Figures.en#cookies";
#elif L==3
	"ANALYTICS.Figures.en#cookies";
#elif L==4
	"ANALYTICS.Figures.es#cookies";
#elif L==5
	"ANALYTICS.Figures.en#cookies";
#elif L==6
	"ANALYTICS.Figures.es#cookies";
#elif L==7
	"ANALYTICS.Figures.en#cookies";
#elif L==8
	"ANALYTICS.Figures.en#cookies";
#elif L==9
	"ANALYTICS.Figures.en#privacy";
#elif L==10	// tr
	"ANALYTICS.Figures.en#privacy";
#endif

const char *Hlp_ANALYTICS_Degrees =
#if   L==1
	"ANALYTICS.Degrees.es";
#elif L==2
	"ANALYTICS.Degrees.en";
#elif L==3
	"ANALYTICS.Degrees.en";
#elif L==4
	"ANALYTICS.Degrees.es";
#elif L==5
	"ANALYTICS.Degrees.en";
#elif L==6
	"ANALYTICS.Degrees.es";
#elif L==7
	"ANALYTICS.Degrees.en";
#elif L==8
	"ANALYTICS.Degrees.en";
#elif L==9
	"ANALYTICS.Degrees.en";
#elif L==10	// tr
	"ANALYTICS.Degrees.en";
#endif

const char *Hlp_ANALYTICS_Indicators =
#if   L==1
	"ANALYTICS.Indicators.es";
#elif L==2
	"ANALYTICS.Indicators.en";
#elif L==3
	"ANALYTICS.Indicators.en";
#elif L==4
	"ANALYTICS.Indicators.es";
#elif L==5
	"ANALYTICS.Indicators.en";
#elif L==6
	"ANALYTICS.Indicators.es";
#elif L==7
	"ANALYTICS.Indicators.en";
#elif L==8
	"ANALYTICS.Indicators.en";
#elif L==9
	"ANALYTICS.Indicators.en";
#elif L==10	// tr
	"ANALYTICS.Indicators.en";
#endif

const char *Hlp_ANALYTICS_Visits_global_visits =
#if   L==1
	"ANALYTICS.Visits.es#accesos-globales";
#elif L==2
	"ANALYTICS.Visits.en#global-visits";
#elif L==3
	"ANALYTICS.Visits.en#global-visits";
#elif L==4
	"ANALYTICS.Visits.es#accesos-globales";
#elif L==5
	"ANALYTICS.Visits.en#global-visits";
#elif L==6
	"ANALYTICS.Visits.es#accesos-globales";
#elif L==7
	"ANALYTICS.Visits.en#global-visits";
#elif L==8
	"ANALYTICS.Visits.en#global-visits";
#elif L==9
	"ANALYTICS.Visits.en#global-visits";
#elif L==10	// tr
	"ANALYTICS.Visits.en#global-visits";
#endif

const char *Hlp_ANALYTICS_Visits_visits_to_course =
#if   L==1
	"ANALYTICS.Visits.es#accesos-a-la-asignatura";
#elif L==2
	"ANALYTICS.Visits.en#visits-to-course";
#elif L==3
	"ANALYTICS.Visits.en#visits-to-course";
#elif L==4
	"ANALYTICS.Visits.es#accesos-a-la-asignatura";
#elif L==5
	"ANALYTICS.Visits.en#visits-to-course";
#elif L==6
	"ANALYTICS.Visits.es#accesos-a-la-asignatura";
#elif L==7
	"ANALYTICS.Visits.en#visits-to-course";
#elif L==8
	"ANALYTICS.Visits.en#visits-to-course";
#elif L==9
	"ANALYTICS.Visits.en#visits-to-course";
#elif L==10	// tr
	"ANALYTICS.Visits.en#visits-to-course";
#endif

const char *Hlp_ANALYTICS_Surveys =
#if   L==1
	"ANALYTICS.Surveys.es";
#elif L==2
	"ANALYTICS.Surveys.en";
#elif L==3
	"ANALYTICS.Surveys.en";
#elif L==4
	"ANALYTICS.Surveys.es";
#elif L==5
	"ANALYTICS.Surveys.en";
#elif L==6
	"ANALYTICS.Surveys.es";
#elif L==7
	"ANALYTICS.Surveys.en";
#elif L==8
	"ANALYTICS.Surveys.en";
#elif L==9
	"ANALYTICS.Surveys.en";
#elif L==10	// tr
	"ANALYTICS.Surveys.en";
#endif

const char *Hlp_ANALYTICS_Surveys_edit_survey =
#if   L==1
	"ANALYTICS.Surveys.es#editar-encuesta";
#elif L==2
	"ANALYTICS.Surveys.en#edit-survey";
#elif L==3
	"ANALYTICS.Surveys.en#edit-survey";
#elif L==4
	"ANALYTICS.Surveys.es#editar-encuesta";
#elif L==5
	"ANALYTICS.Surveys.en#edit-survey";
#elif L==6
	"ANALYTICS.Surveys.es#editar-encuesta";
#elif L==7
	"ANALYTICS.Surveys.en#edit-survey";
#elif L==8
	"ANALYTICS.Surveys.en#edit-survey";
#elif L==9
	"ANALYTICS.Surveys.en#edit-survey";
#elif L==10	// tr
	"ANALYTICS.Surveys.en#edit-survey";
#endif

const char *Hlp_ANALYTICS_Surveys_questions =
#if   L==1
	"ANALYTICS.Surveys.es#preguntas";
#elif L==2
	"ANALYTICS.Surveys.en#questions";
#elif L==3
	"ANALYTICS.Surveys.en#questions";
#elif L==4
	"ANALYTICS.Surveys.es#preguntas";
#elif L==5
	"ANALYTICS.Surveys.en#questions";
#elif L==6
	"ANALYTICS.Surveys.es#preguntas";
#elif L==7
	"ANALYTICS.Surveys.en#questions";
#elif L==8
	"ANALYTICS.Surveys.en#questions";
#elif L==9
	"ANALYTICS.Surveys.en#questions";
#elif L==10	// tr
	"ANALYTICS.Surveys.en#questions";
#endif

const char *Hlp_ANALYTICS_Report =
#if   L==1
	"ANALYTICS.Report.es";
#elif L==2
	"ANALYTICS.Report.en";
#elif L==3
	"ANALYTICS.Report.en";
#elif L==4
	"ANALYTICS.Report.es";
#elif L==5
	"ANALYTICS.Report.en";
#elif L==6
	"ANALYTICS.Report.es";
#elif L==7
	"ANALYTICS.Report.en";
#elif L==8
	"ANALYTICS.Report.en";
#elif L==9
	"ANALYTICS.Report.en";
#elif L==10	// tr
	"ANALYTICS.Report.en";
#endif

const char *Hlp_ANALYTICS_Frequent =
#if   L==1
	"ANALYTICS.Frequent.es";
#elif L==2
	"ANALYTICS.Frequent.en";
#elif L==3
	"ANALYTICS.Frequent.en";
#elif L==4
	"ANALYTICS.Frequent.es";
#elif L==5
	"ANALYTICS.Frequent.en";
#elif L==6
	"ANALYTICS.Frequent.es";
#elif L==7
	"ANALYTICS.Frequent.en";
#elif L==8
	"ANALYTICS.Frequent.en";
#elif L==9
	"ANALYTICS.Frequent.en";
#elif L==10	// tr
	"ANALYTICS.Frequent.en";
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
#elif L==10	// tr
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
#elif L==10	// tr
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
#elif L==10	// tr
	"PROFILE.Account.en";
#endif
/*
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
#elif L==10	// tr
	"PROFILE.Account.en#email";
#endif
*/
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
#elif L==10	// tr
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
#elif L==10	// tr
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
#elif L==10	// tr
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
#elif L==10	// tr
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
#elif L==10	// tr
	"PROFILE.Agenda.en";
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
#elif L==10	// tr
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
#elif L==10	// tr
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
#elif L==10	// tr
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
#elif L==10	// tr
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
#elif L==10	// tr
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
#elif L==10	// tr
	"PROFILE.Photo.en";
#endif

const char *Hlp_PROFILE_Settings_internationalization =
#if   L==1
	"PROFILE.Settings.es#internacionalizaci%C3%B3n";
#elif L==2
	"PROFILE.Settings.en#internationalization";
#elif L==3
	"PROFILE.Settings.en#internationalization";
#elif L==4
	"PROFILE.Settings.es#internacionalizaci%C3%B3n";
#elif L==5
	"PROFILE.Settings.en#internationalization";
#elif L==6
	"PROFILE.Settings.es#internacionalizaci%C3%B3n";
#elif L==7
	"PROFILE.Settings.en#internationalization";
#elif L==8
	"PROFILE.Settings.en#internationalization";
#elif L==9
	"PROFILE.Settings.en#internationalization";
#elif L==10	// tr
	"PROFILE.Settings.en#internationalization";
#endif

const char *Hlp_PROFILE_Settings_language =
#if   L==1
	"PROFILE.Settings.es#idioma";
#elif L==2
	"PROFILE.Settings.en#language";
#elif L==3
	"PROFILE.Settings.en#language";
#elif L==4
	"PROFILE.Settings.es#idioma";
#elif L==5
	"PROFILE.Settings.en#language";
#elif L==6
	"PROFILE.Settings.es#idioma";
#elif L==7
	"PROFILE.Settings.en#language";
#elif L==8
	"PROFILE.Settings.en#language";
#elif L==9
	"PROFILE.Settings.en#language";
#elif L==10	// tr
	"PROFILE.Settings.en#language";
#endif

const char *Hlp_PROFILE_Settings_calendar =
#if   L==1
	"PROFILE.Settings.es#calendario";
#elif L==2
	"PROFILE.Settings.en#calendar";
#elif L==3
	"PROFILE.Settings.en#calendar";
#elif L==4
	"PROFILE.Settings.es#calendario";
#elif L==5
	"PROFILE.Settings.en#calendar";
#elif L==6
	"PROFILE.Settings.es#calendario";
#elif L==7
	"PROFILE.Settings.en#calendar";
#elif L==8
	"PROFILE.Settings.en#calendar";
#elif L==9
	"PROFILE.Settings.en#calendar";
#elif L==10	// tr
	"PROFILE.Settings.en#calendar";
#endif

const char *Hlp_PROFILE_Settings_dates =
#if   L==1
	"PROFILE.Settings.es#fechas";
#elif L==2
	"PROFILE.Settings.en#dates";
#elif L==3
	"PROFILE.Settings.en#dates";
#elif L==4
	"PROFILE.Settings.es#fechas";
#elif L==5
	"PROFILE.Settings.en#dates";
#elif L==6
	"PROFILE.Settings.es#fechas";
#elif L==7
	"PROFILE.Settings.en#dates";
#elif L==8
	"PROFILE.Settings.en#dates";
#elif L==9
	"PROFILE.Settings.en#dates";
#elif L==10	// tr
	"PROFILE.Settings.en#dates";
#endif

const char *Hlp_PROFILE_Settings_design =
#if   L==1
	"PROFILE.Settings.es#dise%C3%B1o";
#elif L==2
	"PROFILE.Settings.en#design";
#elif L==3
	"PROFILE.Settings.en#design";
#elif L==4
	"PROFILE.Settings.es#dise%C3%B1o";
#elif L==5
	"PROFILE.Settings.en#design";
#elif L==6
	"PROFILE.Settings.es#dise%C3%B1o";
#elif L==7
	"PROFILE.Settings.en#design";
#elif L==8
	"PROFILE.Settings.en#design";
#elif L==9
	"PROFILE.Settings.en#design";
#elif L==10	// tr
	"PROFILE.Settings.en#design";
#endif

const char *Hlp_PROFILE_Settings_icons =
#if   L==1
	"PROFILE.Settings.es#iconos";
#elif L==2
	"PROFILE.Settings.en#icons";
#elif L==3
	"PROFILE.Settings.en#icons";
#elif L==4
	"PROFILE.Settings.es#iconos";
#elif L==5
	"PROFILE.Settings.en#icons";
#elif L==6
	"PROFILE.Settings.es#iconos";
#elif L==7
	"PROFILE.Settings.en#icons";
#elif L==8
	"PROFILE.Settings.en#icons";
#elif L==9
	"PROFILE.Settings.en#icons";
#elif L==10	// tr
	"PROFILE.Settings.en#icons";
#endif

const char *Hlp_PROFILE_Settings_theme =
#if   L==1
	"PROFILE.Settings.es#tema";
#elif L==2
	"PROFILE.Settings.en#theme";
#elif L==3
	"PROFILE.Settings.en#theme";
#elif L==4
	"PROFILE.Settings.es#tema";
#elif L==5
	"PROFILE.Settings.en#theme";
#elif L==6
	"PROFILE.Settings.es#tema";
#elif L==7
	"PROFILE.Settings.en#theme";
#elif L==8
	"PROFILE.Settings.en#theme";
#elif L==9
	"PROFILE.Settings.en#theme";
#elif L==10	// tr
	"PROFILE.Settings.en#theme";
#endif

const char *Hlp_PROFILE_Settings_menu =
#if   L==1
	"PROFILE.Settings.es#men%C3%BA";
#elif L==2
	"PROFILE.Settings.en#menu";
#elif L==3
	"PROFILE.Settings.en#menu";
#elif L==4
	"PROFILE.Settings.es#men%C3%BA";
#elif L==5
	"PROFILE.Settings.en#menu";
#elif L==6
	"PROFILE.Settings.es#men%C3%BA";
#elif L==7
	"PROFILE.Settings.en#menu";
#elif L==8
	"PROFILE.Settings.en#menu";
#elif L==9
	"PROFILE.Settings.en#menu";
#elif L==10	// tr
	"PROFILE.Settings.en#menu";
#endif

const char *Hlp_PROFILE_Settings_columns =
#if   L==1
	"PROFILE.Settings.es#columnas";
#elif L==2
	"PROFILE.Settings.en#columns";
#elif L==3
	"PROFILE.Settings.en#columns";
#elif L==4
	"PROFILE.Settings.es#columnas";
#elif L==5
	"PROFILE.Settings.en#columns";
#elif L==6
	"PROFILE.Settings.es#columnas";
#elif L==7
	"PROFILE.Settings.en#columns";
#elif L==8
	"PROFILE.Settings.en#columns";
#elif L==9
	"PROFILE.Settings.en#columns";
#elif L==10	// tr
	"PROFILE.Settings.en#columns";
#endif

const char *Hlp_PROFILE_Settings_user_photos =
#if   L==1
	"PROFILE.Settings.es#fotos-de-usuarios";
#elif L==2
	"PROFILE.Settings.en#user-photos";
#elif L==3
	"PROFILE.Settings.en#user-photos";
#elif L==4
	"PROFILE.Settings.es#fotos-de-usuarios";
#elif L==5
	"PROFILE.Settings.en#user-photos";
#elif L==6
	"PROFILE.Settings.es#fotos-de-usuarios";
#elif L==7
	"PROFILE.Settings.en#user-photos";
#elif L==8
	"PROFILE.Settings.en#user-photos";
#elif L==9
	"PROFILE.Settings.en#user-photos";
#elif L==10	// tr
	"PROFILE.Settings.en#user-photos";
#endif

const char *Hlp_PROFILE_Settings_privacy =
#if   L==1
	"PROFILE.Settings.es#privacidad";
#elif L==2
	"PROFILE.Settings.en#privacy";
#elif L==3
	"PROFILE.Settings.en#privacy";
#elif L==4
	"PROFILE.Settings.es#privacidad";
#elif L==5
	"PROFILE.Settings.en#privacy";
#elif L==6
	"PROFILE.Settings.es#privacidad";
#elif L==7
	"PROFILE.Settings.en#privacy";
#elif L==8
	"PROFILE.Settings.en#privacy";
#elif L==9
	"PROFILE.Settings.en#privacy";
#elif L==10	// tr
	"PROFILE.Settings.en#privacy";
#endif

const char *Hlp_PROFILE_Settings_cookies =
#if   L==1
	"PROFILE.Settings.es#cookies";
#elif L==2
	"PROFILE.Settings.en#cookies";
#elif L==3
	"PROFILE.Settings.en#cookies";
#elif L==4
	"PROFILE.Settings.es#cookies";
#elif L==5
	"PROFILE.Settings.en#cookies";
#elif L==6
	"PROFILE.Settings.es#cookies";
#elif L==7
	"PROFILE.Settings.en#cookies";
#elif L==8
	"PROFILE.Settings.en#cookies";
#elif L==9
	"PROFILE.Settings.en#cookies";
#elif L==10	// tr
	"PROFILE.Settings.en#cookies";
#endif

const char *Hlp_PROFILE_Settings_notifications =
#if   L==1
	"PROFILE.Settings.es#notificaciones";
#elif L==2
	"PROFILE.Settings.en#notifications";
#elif L==3
	"PROFILE.Settings.en#notifications";
#elif L==4
	"PROFILE.Settings.es#notificaciones";
#elif L==5
	"PROFILE.Settings.en#notifications";
#elif L==6
	"PROFILE.Settings.es#notificaciones";
#elif L==7
	"PROFILE.Settings.en#notifications";
#elif L==8
	"PROFILE.Settings.en#notifications";
#elif L==9
	"PROFILE.Settings.en#notifications";
#elif L==10	// tr
	"PROFILE.Settings.en#notifications";
#endif
