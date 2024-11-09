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
#if   L==1	// ca
	"Multimedia.es";
#elif L==2	// de
	"Multimedia.en";
#elif L==3	// en
	"Multimedia.en";
#elif L==4	// es
	"Multimedia.es";
#elif L==5	// fr
	"Multimedia.en";
#elif L==6	// gn
	"Multimedia.es";
#elif L==7	// it
	"Multimedia.en";
#elif L==8	// pl
	"Multimedia.en";
#elif L==9	// pt
	"Multimedia.en";
#elif L==10	// tr
	"Multimedia.en";
#endif

/***** START tab *****/

const char *Hlp_START_Search =
#if   L==1	// ca
	"START.Search.es";
#elif L==2	// de
	"START.Search.en";
#elif L==3	// en
	"START.Search.en";
#elif L==4	// es
	"START.Search.es";
#elif L==5	// fr
	"START.Search.en";
#elif L==6	// gn
	"START.Search.es";
#elif L==7	// it
	"START.Search.en";
#elif L==8	// pl
	"START.Search.en";
#elif L==9	// pt
	"START.Search.en";
#elif L==10	// tr
	"START.Search.en";
#endif

const char *Hlp_START_Timeline =
#if   L==1	// ca
	"START.Timeline.es";
#elif L==2	// de
	"START.Timeline.en";
#elif L==3	// en
	"START.Timeline.en";
#elif L==4	// es
	"START.Timeline.es";
#elif L==5	// fr
	"START.Timeline.en";
#elif L==6	// gn
	"START.Timeline.es";
#elif L==7	// it
	"START.Timeline.en";
#elif L==8	// pl
	"START.Timeline.en";
#elif L==9	// pt
	"START.Timeline.en";
#elif L==10	// tr
	"START.Timeline.en";
#endif

const char *Hlp_START_Profiles_view_public_profile =
#if   L==1	// ca
	"START.Profiles.es#ver-perfil-p%C3%BAblico";
#elif L==2	// de
	"START.Profiles.en#view-public-profile";
#elif L==3	// en
	"START.Profiles.en#view-public-profile";
#elif L==4	// es
	"START.Profiles.es#ver-perfil-p%C3%BAblico";
#elif L==5	// fr
	"START.Profiles.en#view-public-profile";
#elif L==6	// gn
	"START.Profiles.es#ver-perfil-p%C3%BAblico";
#elif L==7	// it
	"START.Profiles.en#view-public-profile";
#elif L==8	// pl
	"START.Profiles.en#view-public-profile";
#elif L==9	// pt
	"START.Profiles.en#view-public-profile";
#elif L==10	// tr
	"START.Profiles.en#view-public-profile";
#endif

const char *Hlp_START_Profiles_who_to_follow =
#if   L==1	// ca
	"START.Profiles.es#a-qui%C3%A9n-seguir";
#elif L==2	// de
	"START.Profiles.en#who-to-follow";
#elif L==3	// en
	"START.Profiles.en#who-to-follow";
#elif L==4	// es
	"START.Profiles.es#a-qui%C3%A9n-seguir";
#elif L==5	// fr
	"START.Profiles.en#who-to-follow";
#elif L==6	// gn
	"START.Profiles.es#a-qui%C3%A9n-seguir";
#elif L==7	// it
	"START.Profiles.en#who-to-follow";
#elif L==8	// pl
	"START.Profiles.en#who-to-follow";
#elif L==9	// pt
	"START.Profiles.en#who-to-follow";
#elif L==10	// tr
	"START.Profiles.en#who-to-follow";
#endif

const char *Hlp_START_Calendar =
#if   L==1	// ca
	"START.Calendar.es";
#elif L==2	// de
	"START.Calendar.en";
#elif L==3	// en
	"START.Calendar.en";
#elif L==4	// es
	"START.Calendar.es";
#elif L==5	// fr
	"START.Calendar.en";
#elif L==6	// gn
	"START.Calendar.es";
#elif L==7	// it
	"START.Calendar.en";
#elif L==8	// pl
	"START.Calendar.en";
#elif L==9	// pt
	"START.Calendar.en";
#elif L==10	// tr
	"START.Calendar.en";
#endif

const char *Hlp_START_Notifications =
#if   L==1	// ca
	"START.Notifications.es";
#elif L==2	// de
	"START.Notifications.en";
#elif L==3	// en
	"START.Notifications.en";
#elif L==4	// es
	"START.Notifications.es";
#elif L==5	// fr
	"START.Notifications.en";
#elif L==6	// gn
	"START.Notifications.es";
#elif L==7	// it
	"START.Notifications.en";
#elif L==8	// pl
	"START.Notifications.en";
#elif L==9	// pt
	"START.Notifications.en";
#elif L==10	// tr
	"START.Notifications.en";
#endif

const char *Hlp_START_Domains =
#if   L==1	// ca
	"START.Domains.es";
#elif L==2	// de
	"START.Domains.en";
#elif L==3	// en
	"START.Domains.en";
#elif L==4	// es
	"START.Domains.es";
#elif L==5	// fr
	"START.Domains.en";
#elif L==6	// gn
	"START.Domains.es";
#elif L==7	// it
	"START.Domains.en";
#elif L==8	// pl
	"START.Domains.en";
#elif L==9	// pt
	"START.Domains.en";
#elif L==10	// tr
	"START.Domains.en";
#endif

const char *Hlp_START_Domains_edit =
#if   L==1	// ca
	"START.Domains.es#editar";
#elif L==2	// de
	"START.Domains.en#edit";
#elif L==3	// en
	"START.Domains.en#edit";
#elif L==4	// es
	"START.Domains.es#editar";
#elif L==5	// fr
	"START.Domains.en#edit";
#elif L==6	// gn
	"START.Domains.es#editar";
#elif L==7	// it
	"START.Domains.en#edit";
#elif L==8	// pl
	"START.Domains.en#edit";
#elif L==9	// pt
	"START.Domains.en#edit";
#elif L==10	// tr
	"START.Domains.en#edit";
#endif

/***** SYSTEM tab *****/

const char *Hlp_SYSTEM_Information =
#if   L==1	// ca
	"SYSTEM.Information.es";
#elif L==2	// de
	"SYSTEM.Information.en";
#elif L==3	// en
	"SYSTEM.Information.en";
#elif L==4	// es
	"SYSTEM.Information.es";
#elif L==5	// fr
	"SYSTEM.Information.en";
#elif L==6	// gn
	"SYSTEM.Information.es";
#elif L==7	// it
	"SYSTEM.Information.en";
#elif L==8	// pl
	"SYSTEM.Information.en";
#elif L==9	// pt
	"SYSTEM.Information.en";
#elif L==10	// tr
	"SYSTEM.Information.en";
#endif

const char *Hlp_SYSTEM_Countries =
#if   L==1	// ca
	"SYSTEM.Countries.es";
#elif L==2	// de
	"SYSTEM.Countries.en";
#elif L==3	// en
	"SYSTEM.Countries.en";
#elif L==4	// es
	"SYSTEM.Countries.es";
#elif L==5	// fr
	"SYSTEM.Countries.en";
#elif L==6	// gn
	"SYSTEM.Countries.es";
#elif L==7	// it
	"SYSTEM.Countries.en";
#elif L==8	// pl
	"SYSTEM.Countries.en";
#elif L==9	// pt
	"SYSTEM.Countries.en";
#elif L==10	// tr
	"SYSTEM.Countries.en";
#endif

const char *Hlp_SYSTEM_Pending =
#if   L==1	// ca
	"SYSTEM.Pending.es";
#elif L==2	// de
	"SYSTEM.Pending.en";
#elif L==3	// en
	"SYSTEM.Pending.en";
#elif L==4	// es
	"SYSTEM.Pending.es";
#elif L==5	// fr
	"SYSTEM.Pending.en";
#elif L==6	// gn
	"SYSTEM.Pending.es";
#elif L==7	// it
	"SYSTEM.Pending.en";
#elif L==8	// pl
	"SYSTEM.Pending.en";
#elif L==9	// pt
	"SYSTEM.Pending.en";
#elif L==10	// tr
	"SYSTEM.Pending.en";
#endif

const char *Hlp_SYSTEM_Links =
#if   L==1	// ca
	"SYSTEM.Links.es";
#elif L==2	// de
	"SYSTEM.Links.en";
#elif L==3	// en
	"SYSTEM.Links.en";
#elif L==4	// es
	"SYSTEM.Links.es";
#elif L==5	// fr
	"SYSTEM.Links.en";
#elif L==6	// gn
	"SYSTEM.Links.es";
#elif L==7	// it
	"SYSTEM.Links.en";
#elif L==8	// pl
	"SYSTEM.Links.en";
#elif L==9	// pt
	"SYSTEM.Links.en";
#elif L==10	// tr
	"SYSTEM.Links.en";
#endif

const char *Hlp_SYSTEM_Links_edit =
#if   L==1	// ca
	"SYSTEM.Links.es#editar";
#elif L==2	// de
	"SYSTEM.Links.en#edit";
#elif L==3	// en
	"SYSTEM.Links.en#edit";
#elif L==4	// es
	"SYSTEM.Links.es#editar";
#elif L==5	// fr
	"SYSTEM.Links.en#edit";
#elif L==6	// gn
	"SYSTEM.Links.es#editar";
#elif L==7	// it
	"SYSTEM.Links.en#edit";
#elif L==8	// pl
	"SYSTEM.Links.en#edit";
#elif L==9	// pt
	"SYSTEM.Links.en#edit";
#elif L==10	// tr
	"SYSTEM.Links.en#edit";
#endif

const char *Hlp_SYSTEM_Banners =
#if   L==1	// ca
	"SYSTEM.Banners.es";
#elif L==2	// de
	"SYSTEM.Banners.en";
#elif L==3	// en
	"SYSTEM.Banners.en";
#elif L==4	// es
	"SYSTEM.Banners.es";
#elif L==5	// fr
	"SYSTEM.Banners.en";
#elif L==6	// gn
	"SYSTEM.Banners.es";
#elif L==7	// it
	"SYSTEM.Banners.en";
#elif L==8	// pl
	"SYSTEM.Banners.en";
#elif L==9	// pt
	"SYSTEM.Banners.en";
#elif L==10	// tr
	"SYSTEM.Banners.en";
#endif

const char *Hlp_SYSTEM_Banners_edit =
#if   L==1	// ca
	"SYSTEM.Banners.es#editar";
#elif L==2	// de
	"SYSTEM.Banners.en#edit";
#elif L==3	// en
	"SYSTEM.Banners.en#edit";
#elif L==4	// es
	"SYSTEM.Banners.es#editar";
#elif L==5	// fr
	"SYSTEM.Banners.en#edit";
#elif L==6	// gn
	"SYSTEM.Banners.es#editar";
#elif L==7	// it
	"SYSTEM.Banners.en#edit";
#elif L==8	// pl
	"SYSTEM.Banners.en#edit";
#elif L==9	// pt
	"SYSTEM.Banners.en#edit";
#elif L==10	// tr
	"SYSTEM.Banners.en#edit";
#endif

const char *Hlp_SYSTEM_Plugins =
#if   L==1	// ca
	"SYSTEM.Plugins.es";
#elif L==2	// de
	"SYSTEM.Plugins.en";
#elif L==3	// en
	"SYSTEM.Plugins.en";
#elif L==4	// es
	"SYSTEM.Plugins.es";
#elif L==5	// fr
	"SYSTEM.Plugins.en";
#elif L==6	// gn
	"SYSTEM.Plugins.es";
#elif L==7	// it
	"SYSTEM.Plugins.en";
#elif L==8	// pl
	"SYSTEM.Plugins.en";
#elif L==9	// pt
	"SYSTEM.Plugins.en";
#elif L==10	// tr
	"SYSTEM.Plugins.en";
#endif

const char *Hlp_SYSTEM_Plugins_edit =
#if   L==1	// ca
	"SYSTEM.Plugins.es#editar";
#elif L==2	// de
	"SYSTEM.Plugins.en#edit";
#elif L==3	// en
	"SYSTEM.Plugins.en#edit";
#elif L==4	// es
	"SYSTEM.Plugins.es#editar";
#elif L==5	// fr
	"SYSTEM.Plugins.en#edit";
#elif L==6	// gn
	"SYSTEM.Plugins.es#editar";
#elif L==7	// it
	"SYSTEM.Plugins.en#edit";
#elif L==8	// pl
	"SYSTEM.Plugins.en#edit";
#elif L==9	// pt
	"SYSTEM.Plugins.en#edit";
#elif L==10	// tr
	"SYSTEM.Plugins.en#edit";
#endif

const char *Hlp_SYSTEM_Maintenance_eliminate_old_courses =
#if   L==1	// ca
	"SYSTEM.Maintenance.es#eliminar-asignaturas-antiguas";
#elif L==2	// de
	"SYSTEM.Maintenance.en#eliminate-old-courses";
#elif L==3	// en
	"SYSTEM.Maintenance.en#eliminate-old-courses";
#elif L==4	// es
	"SYSTEM.Maintenance.es#eliminar-asignaturas-antiguas";
#elif L==5	// fr
	"SYSTEM.Maintenance.en#eliminate-old-courses";
#elif L==6	// gn
	"SYSTEM.Maintenance.es#eliminar-asignaturas-antiguas";
#elif L==7	// it
	"SYSTEM.Maintenance.en#eliminate-old-courses";
#elif L==8	// pl
	"SYSTEM.Maintenance.en#eliminate-old-courses";
#elif L==9	// pt
	"SYSTEM.Maintenance.en#eliminate-old-courses";
#elif L==10	// tr
	"SYSTEM.Maintenance.en#eliminate-old-courses";
#endif

/***** COUNTRY tab *****/

const char *Hlp_COUNTRY_Information =
#if   L==1	// ca
	"COUNTRY.Information.es";
#elif L==2	// de
	"COUNTRY.Information.en";
#elif L==3	// en
	"COUNTRY.Information.en";
#elif L==4	// es
	"COUNTRY.Information.es";
#elif L==5	// fr
	"COUNTRY.Information.en";
#elif L==6	// gn
	"COUNTRY.Information.es";
#elif L==7	// it
	"COUNTRY.Information.en";
#elif L==8	// pl
	"COUNTRY.Information.en";
#elif L==9	// pt
	"COUNTRY.Information.en";
#elif L==10	// tr
	"COUNTRY.Information.en";
#endif

const char *Hlp_COUNTRY_Institutions =
#if   L==1	// ca
	"COUNTRY.Institutions.es";
#elif L==2	// de
	"COUNTRY.Institutions.en";
#elif L==3	// en
	"COUNTRY.Institutions.en";
#elif L==4	// es
	"COUNTRY.Institutions.es";
#elif L==5	// fr
	"COUNTRY.Institutions.en";
#elif L==6	// gn
	"COUNTRY.Institutions.es";
#elif L==7	// it
	"COUNTRY.Institutions.en";
#elif L==8	// pl
	"COUNTRY.Institutions.en";
#elif L==9	// pt
	"COUNTRY.Institutions.en";
#elif L==10	// tr
	"COUNTRY.Institutions.en";
#endif

/***** INSTITUTION tab *****/

const char *Hlp_INSTITUTION_Information =
#if   L==1	// ca
	"INSTITUTION.Information.es";
#elif L==2	// de
	"INSTITUTION.Information.en";
#elif L==3	// en
	"INSTITUTION.Information.en";
#elif L==4	// es
	"INSTITUTION.Information.es";
#elif L==5	// fr
	"INSTITUTION.Information.en";
#elif L==6	// gn
	"INSTITUTION.Information.es";
#elif L==7	// it
	"INSTITUTION.Information.en";
#elif L==8	// pl
	"INSTITUTION.Information.en";
#elif L==9	// pt
	"INSTITUTION.Information.en";
#elif L==10	// tr
	"INSTITUTION.Information.en";
#endif

const char *Hlp_INSTITUTION_Centers =
#if   L==1	// ca
	"INSTITUTION.Centers.es";
#elif L==2	// de
	"INSTITUTION.Centers.en";
#elif L==3	// en
	"INSTITUTION.Centers.en";
#elif L==4	// es
	"INSTITUTION.Centers.es";
#elif L==5	// fr
	"INSTITUTION.Centers.en";
#elif L==6	// gn
	"INSTITUTION.Centers.es";
#elif L==7	// it
	"INSTITUTION.Centers.en";
#elif L==8	// pl
	"INSTITUTION.Centers.en";
#elif L==9	// pt
	"INSTITUTION.Centers.en";
#elif L==10	// tr
	"INSTITUTION.Centers.en";
#endif

const char *Hlp_INSTITUTION_Departments =
#if   L==1	// ca
	"INSTITUTION.Departments.es";
#elif L==2	// de
	"INSTITUTION.Departments.en";
#elif L==3	// en
	"INSTITUTION.Departments.en";
#elif L==4	// es
	"INSTITUTION.Departments.es";
#elif L==5	// fr
	"INSTITUTION.Departments.en";
#elif L==6	// gn
	"INSTITUTION.Departments.es";
#elif L==7	// it
	"INSTITUTION.Departments.en";
#elif L==8	// pl
	"INSTITUTION.Departments.en";
#elif L==9	// pt
	"INSTITUTION.Departments.en";
#elif L==10	// tr
	"INSTITUTION.Departments.en";
#endif

const char *Hlp_INSTITUTION_Departments_edit =
#if   L==1	// ca
	"INSTITUTION.Departments.es#editar";
#elif L==2	// de
	"INSTITUTION.Departments.en#edit";
#elif L==3	// en
	"INSTITUTION.Departments.en#edit";
#elif L==4	// es
	"INSTITUTION.Departments.es#editar";
#elif L==5	// fr
	"INSTITUTION.Departments.en#edit";
#elif L==6	// gn
	"INSTITUTION.Departments.es#editar";
#elif L==7	// it
	"INSTITUTION.Departments.en#edit";
#elif L==8	// pl
	"INSTITUTION.Departments.en#edit";
#elif L==9	// pt
	"INSTITUTION.Departments.en#edit";
#elif L==10	// tr
	"INSTITUTION.Departments.en#edit";
#endif

const char *Hlp_INSTITUTION_Places =
#if   L==1	// ca
	"INSTITUTION.Places.es";
#elif L==2	// de
	"INSTITUTION.Places.en";
#elif L==3	// en
	"INSTITUTION.Places.en";
#elif L==4	// es
	"INSTITUTION.Places.es";
#elif L==5	// fr
	"INSTITUTION.Places.en";
#elif L==6	// gn
	"INSTITUTION.Places.es";
#elif L==7	// it
	"INSTITUTION.Places.en";
#elif L==8	// pl
	"INSTITUTION.Places.en";
#elif L==9	// pt
	"INSTITUTION.Places.en";
#elif L==10	// tr
	"INSTITUTION.Places.en";
#endif

const char *Hlp_INSTITUTION_Places_edit =
#if   L==1	// ca
	"INSTITUTION.Places.es#editar";
#elif L==2	// de
	"INSTITUTION.Places.en#edit";
#elif L==3	// en
	"INSTITUTION.Places.en#edit";
#elif L==4	// es
	"INSTITUTION.Places.es#editar";
#elif L==5	// fr
	"INSTITUTION.Places.en#edit";
#elif L==6	// gn
	"INSTITUTION.Places.es#editar";
#elif L==7	// it
	"INSTITUTION.Places.en#edit";
#elif L==8	// pl
	"INSTITUTION.Places.en#edit";
#elif L==9	// pt
	"INSTITUTION.Places.en#edit";
#elif L==10	// tr
	"INSTITUTION.Places.en#edit";
#endif

const char *Hlp_INSTITUTION_Holidays =
#if   L==1	// ca
	"INSTITUTION.Holidays.es";
#elif L==2	// de
	"INSTITUTION.Holidays.en";
#elif L==3	// en
	"INSTITUTION.Holidays.en";
#elif L==4	// es
	"INSTITUTION.Holidays.es";
#elif L==5	// fr
	"INSTITUTION.Holidays.en";
#elif L==6	// gn
	"INSTITUTION.Holidays.es";
#elif L==7	// it
	"INSTITUTION.Holidays.en";
#elif L==8	// pl
	"INSTITUTION.Holidays.en";
#elif L==9	// pt
	"INSTITUTION.Holidays.en";
#elif L==10	// tr
	"INSTITUTION.Holidays.en";
#endif

const char *Hlp_INSTITUTION_Holidays_edit =
#if   L==1	// ca
	"INSTITUTION.Holidays.es#editar";
#elif L==2	// de
	"INSTITUTION.Holidays.en#edit";
#elif L==3	// en
	"INSTITUTION.Holidays.en#edit";
#elif L==4	// es
	"INSTITUTION.Holidays.es#editar";
#elif L==5	// fr
	"INSTITUTION.Holidays.en#edit";
#elif L==6	// gn
	"INSTITUTION.Holidays.es#editar";
#elif L==7	// it
	"INSTITUTION.Holidays.en#edit";
#elif L==8	// pl
	"INSTITUTION.Holidays.en#edit";
#elif L==9	// pt
	"INSTITUTION.Holidays.en#edit";
#elif L==10	// tr
	"INSTITUTION.Holidays.en#edit";
#endif

/***** CENTER tab *****/

const char *Hlp_CENTER_Information =
#if   L==1	// ca
	"CENTER.Information.es";
#elif L==2	// de
	"CENTER.Information.en";
#elif L==3	// en
	"CENTER.Information.en";
#elif L==4	// es
	"CENTER.Information.es";
#elif L==5	// fr
	"CENTER.Information.en";
#elif L==6	// gn
	"CENTER.Information.es";
#elif L==7	// it
	"CENTER.Information.en";
#elif L==8	// pl
	"CENTER.Information.en";
#elif L==9	// pt
	"CENTER.Information.en";
#elif L==10	// tr
	"CENTER.Information.en";
#endif

const char *Hlp_CENTER_DegreeTypes =
#if   L==1	// ca
	"CENTER.DegreeTypes.es";
#elif L==2	// de
	"CENTER.DegreeTypes.en";
#elif L==3	// en
	"CENTER.DegreeTypes.en";
#elif L==4	// es
	"CENTER.DegreeTypes.es";
#elif L==5	// fr
	"CENTER.DegreeTypes.en";
#elif L==6	// gn
	"CENTER.DegreeTypes.es";
#elif L==7	// it
	"CENTER.DegreeTypes.en";
#elif L==8	// pl
	"CENTER.DegreeTypes.en";
#elif L==9	// pt
	"CENTER.DegreeTypes.en";
#elif L==10	// tr
	"CENTER.DegreeTypes.en";
#endif

const char *Hlp_CENTER_DegreeTypes_edit =
#if   L==1	// ca
	"CENTER.DegreeTypes.es#editar";
#elif L==2	// de
	"CENTER.DegreeTypes.en#edit";
#elif L==3	// en
	"CENTER.DegreeTypes.en#edit";
#elif L==4	// es
	"CENTER.DegreeTypes.es#editar";
#elif L==5	// fr
	"CENTER.DegreeTypes.en#edit";
#elif L==6	// gn
	"CENTER.DegreeTypes.es#editar";
#elif L==7	// it
	"CENTER.DegreeTypes.en#edit";
#elif L==8	// pl
	"CENTER.DegreeTypes.en#edit";
#elif L==9	// pt
	"CENTER.DegreeTypes.en#edit";
#elif L==10	// tr
	"CENTER.DegreeTypes.en#edit";
#endif

const char *Hlp_CENTER_Degrees =
#if   L==1	// ca
	"CENTER.Degrees.es";
#elif L==2	// de
	"CENTER.Degrees.en";
#elif L==3	// en
	"CENTER.Degrees.en";
#elif L==4	// es
	"CENTER.Degrees.es";
#elif L==5	// fr
	"CENTER.Degrees.en";
#elif L==6	// gn
	"CENTER.Degrees.es";
#elif L==7	// it
	"CENTER.Degrees.en";
#elif L==8	// pl
	"CENTER.Degrees.en";
#elif L==9	// pt
	"CENTER.Degrees.en";
#elif L==10	// tr
	"CENTER.Degrees.en";
#endif

const char *Hlp_CENTER_Buildings =
#if   L==1	// ca
	"CENTER.Buildings.es";
#elif L==2	// de
	"CENTER.Buildings.en";
#elif L==3	// en
	"CENTER.Buildings.en";
#elif L==4	// es
	"CENTER.Buildings.es";
#elif L==5	// fr
	"CENTER.Buildings.en";
#elif L==6	// gn
	"CENTER.Buildings.es";
#elif L==7	// it
	"CENTER.Buildings.en";
#elif L==8	// pl
	"CENTER.Buildings.en";
#elif L==9	// pt
	"CENTER.Buildings.en";
#elif L==10	// tr
	"CENTER.Buildings.en";
#endif

const char *Hlp_CENTER_Buildings_edit =
#if   L==1	// ca
	"CENTER.Buildings.es#editar-edificios";
#elif L==2	// de
	"CENTER.Buildings.en#edit-buildings";
#elif L==3	// en
	"CENTER.Buildings.en#edit-buildings";
#elif L==4	// es
	"CENTER.Buildings.es#editar-edificios";
#elif L==5	// fr
	"CENTER.Buildings.en#edit-buildings";
#elif L==6	// gn
	"CENTER.Buildings.es#editar-edificios";
#elif L==7	// it
	"CENTER.Buildings.en#edit-buildings";
#elif L==8	// pl
	"CENTER.Buildings.en#edit-buildings";
#elif L==9	// pt
	"CENTER.Buildings.en#edit-buildings";
#elif L==10	// tr
	"CENTER.Buildings.en#edit-buildings";
#endif

const char *Hlp_CENTER_Rooms =
#if   L==1	// ca
	"CENTER.Rooms.es";
#elif L==2	// de
	"CENTER.Rooms.en";
#elif L==3	// en
	"CENTER.Rooms.en";
#elif L==4	// es
	"CENTER.Rooms.es";
#elif L==5	// fr
	"CENTER.Rooms.en";
#elif L==6	// gn
	"CENTER.Rooms.es";
#elif L==7	// it
	"CENTER.Rooms.en";
#elif L==8	// pl
	"CENTER.Rooms.en";
#elif L==9	// pt
	"CENTER.Rooms.en";
#elif L==10	// tr
	"CENTER.Rooms.en";
#endif

const char *Hlp_CENTER_Rooms_edit =
#if   L==1	// ca
	"CENTER.Rooms.es#editar-salas";
#elif L==2	// de
	"CENTER.Rooms.en#edit-rooms";
#elif L==3	// en
	"CENTER.Rooms.en#edit-rooms";
#elif L==4	// es
	"CENTER.Rooms.es#editar-salas";
#elif L==5	// fr
	"CENTER.Rooms.en#edit-rooms";
#elif L==6	// gn
	"CENTER.Rooms.es#editar-salas";
#elif L==7	// it
	"CENTER.Rooms.en#edit-rooms";
#elif L==8	// pl
	"CENTER.Rooms.en#edit-rooms";
#elif L==9	// pt
	"CENTER.Rooms.en#edit-rooms";
#elif L==10	// tr
	"CENTER.Rooms.en#edit-rooms";
#endif

/***** DEGREE tab *****/

const char *Hlp_DEGREE_Information =
#if   L==1	// ca
	"DEGREE.Information.es";
#elif L==2	// de
	"DEGREE.Information.en";
#elif L==3	// en
	"DEGREE.Information.en";
#elif L==4	// es
	"DEGREE.Information.es";
#elif L==5	// fr
	"DEGREE.Information.en";
#elif L==6	// gn
	"DEGREE.Information.es";
#elif L==7	// it
	"DEGREE.Information.en";
#elif L==8	// pl
	"DEGREE.Information.en";
#elif L==9	// pt
	"DEGREE.Information.en";
#elif L==10	// tr
	"DEGREE.Information.en";
#endif

const char *Hlp_DEGREE_Courses =
#if   L==1	// ca
	"DEGREE.Courses.es";
#elif L==2	// de
	"DEGREE.Courses.en";
#elif L==3	// en
	"DEGREE.Courses.en";
#elif L==4	// es
	"DEGREE.Courses.es";
#elif L==5	// fr
	"DEGREE.Courses.en";
#elif L==6	// gn
	"DEGREE.Courses.es";
#elif L==7	// it
	"DEGREE.Courses.en";
#elif L==8	// pl
	"DEGREE.Courses.en";
#elif L==9	// pt
	"DEGREE.Courses.en";
#elif L==10	// tr
	"DEGREE.Courses.en";
#endif

/***** COURSE tab *****/

const char *Hlp_COURSE_Information =
#if   L==1	// ca
	"COURSE.Information.es";
#elif L==2	// de
	"COURSE.Information.en";
#elif L==3	// en
	"COURSE.Information.en";
#elif L==4	// es
	"COURSE.Information.es";
#elif L==5	// fr
	"COURSE.Information.en";
#elif L==6	// gn
	"COURSE.Information.es";
#elif L==7	// it
	"COURSE.Information.en";
#elif L==8	// pl
	"COURSE.Information.en";
#elif L==9	// pt
	"COURSE.Information.en";
#elif L==10	// tr
	"COURSE.Information.en";
#endif

const char *Hlp_COURSE_Information_textual_information =
#if   L==1	// ca
	"COURSE.Information.es##informaci%C3%B3n-textual";
#elif L==2	// de
	"COURSE.Information.en#textual-information";
#elif L==3	// en
	"COURSE.Information.en#textual-information";
#elif L==4	// es
	"COURSE.Information.es##informaci%C3%B3n-textual";
#elif L==5	// fr
	"COURSE.Information.en#textual-information";
#elif L==6	// gn
	"COURSE.Information.es##informaci%C3%B3n-textual";
#elif L==7	// it
	"COURSE.Information.en#textual-information";
#elif L==8	// pl
	"COURSE.Information.en#textual-information";
#elif L==9	// pt
	"COURSE.Information.en#textual-information";
#elif L==10	// tr
	"COURSE.Information.en#textual-information";
#endif

const char *Hlp_COURSE_Information_edit =
#if   L==1	// ca
	"COURSE.Information.es#editar";
#elif L==2	// de
	"COURSE.Information.en#edit";
#elif L==3	// en
	"COURSE.Information.en#edit";
#elif L==4	// es
	"COURSE.Information.es#editar";
#elif L==5	// fr
	"COURSE.Information.en#edit";
#elif L==6	// gn
	"COURSE.Information.es#editar";
#elif L==7	// it
	"COURSE.Information.en#edit";
#elif L==8	// pl
	"COURSE.Information.en#edit";
#elif L==9	// pt
	"COURSE.Information.en#edit";
#elif L==10	// tr
	"COURSE.Information.en#edit";
#endif

const char *Hlp_COURSE_Program =
#if   L==1	// ca
	"COURSE.Program.es";
#elif L==2	// de
	"COURSE.Program.en";
#elif L==3	// en
	"COURSE.Program.en";
#elif L==4	// es
	"COURSE.Program.es";
#elif L==5	// fr
	"COURSE.Program.en";
#elif L==6	// gn
	"COURSE.Program.es";
#elif L==7	// it
	"COURSE.Program.en";
#elif L==8	// pl
	"COURSE.Program.en";
#elif L==9	// pt
	"COURSE.Program.en";
#elif L==10	// tr
	"COURSE.Program.en";
#endif

const char *Hlp_COURSE_Program_edit =
#if   L==1	// ca
	"COURSE.Program.es#editar";
#elif L==2	// de
	"COURSE.Program.en#edit";
#elif L==3	// en
	"COURSE.Program.en#edit";
#elif L==4	// es
	"COURSE.Program.es#editar";
#elif L==5	// fr
	"COURSE.Program.en#edit";
#elif L==6	// gn
	"COURSE.Program.es#editar";
#elif L==7	// it
	"COURSE.Program.en#edit";
#elif L==8	// pl
	"COURSE.Program.en#edit";
#elif L==9	// pt
	"COURSE.Program.en#edit";
#elif L==10	// tr
	"COURSE.Program.en#edit";
#endif

const char *Hlp_COURSE_Program_resources =
#if   L==1	// ca
	"COURSE.Program.es#recursos";
#elif L==2	// de
	"COURSE.Program.en#resources";
#elif L==3	// en
	"COURSE.Program.en#resources";
#elif L==4	// es
	"COURSE.Program.es#recursos";
#elif L==5	// fr
	"COURSE.Program.en#resources";
#elif L==6	// gn
	"COURSE.Program.es#recursos";
#elif L==7	// it
	"COURSE.Program.en#resources";
#elif L==8	// pl
	"COURSE.Program.en#resources";
#elif L==9	// pt
	"COURSE.Program.en#resources";
#elif L==10	// tr
	"COURSE.Program.en#resources";
#endif

const char *Hlp_COURSE_Program_resource_clipboard =
#if   L==1	// ca
	"COURSE.Program.en#resource-clipboard";
#elif L==2	// de
	"COURSE.Program.en#resource-clipboard";
#elif L==3	// en
	"COURSE.Program.en#resource-clipboard";
#elif L==4	// es
	"COURSE.Program.es#portapapeles-de-recursos";
#elif L==5	// fr
	"COURSE.Program.en#resource-clipboard";
#elif L==6	// gn
	"COURSE.Program.es#portapapeles-de-recursos";
#elif L==7	// it
	"COURSE.Program.en#resource-clipboard";
#elif L==8	// pl
	"COURSE.Program.en#resource-clipboard";
#elif L==9	// pt
	"COURSE.Program.en#resource-clipboard";
#elif L==10	// tr
	"COURSE.Program.en#resource-clipboard";
#endif

const char *Hlp_COURSE_Guide =
#if   L==1	// ca
	"COURSE.Guide.es";
#elif L==2	// de
	"COURSE.Guide.en";
#elif L==3	// en
	"COURSE.Guide.en";
#elif L==4	// es
	"COURSE.Guide.es";
#elif L==5	// fr
	"COURSE.Guide.en";
#elif L==6	// gn
	"COURSE.Guide.es";
#elif L==7	// it
	"COURSE.Guide.en";
#elif L==8	// pl
	"COURSE.Guide.en";
#elif L==9	// pt
	"COURSE.Guide.en";
#elif L==10	// tr
	"COURSE.Guide.en";
#endif

const char *Hlp_COURSE_Guide_edit =
#if   L==1	// ca
	"COURSE.Guide.es#editar";
#elif L==2	// de
	"COURSE.Guide.en#edit";
#elif L==3	// en
	"COURSE.Guide.en#edit";
#elif L==4	// es
	"COURSE.Guide.es#editar";
#elif L==5	// fr
	"COURSE.Guide.en#edit";
#elif L==6	// gn
	"COURSE.Guide.es#editar";
#elif L==7	// it
	"COURSE.Guide.en#edit";
#elif L==8	// pl
	"COURSE.Guide.en#edit";
#elif L==9	// pt
	"COURSE.Guide.en#edit";
#elif L==10	// tr
	"COURSE.Guide.en#edit";
#endif

const char *Hlp_COURSE_Syllabus =
#if   L==1	// ca
	"COURSE.Syllabus.es";
#elif L==2	// de
	"COURSE.Syllabus.en";
#elif L==3	// en
	"COURSE.Syllabus.en";
#elif L==4	// es
	"COURSE.Syllabus.es";
#elif L==5	// fr
	"COURSE.Syllabus.en";
#elif L==6	// gn
	"COURSE.Syllabus.es";
#elif L==7	// it
	"COURSE.Syllabus.en";
#elif L==8	// pl
	"COURSE.Syllabus.en";
#elif L==9	// pt
	"COURSE.Syllabus.en";
#elif L==10	// tr
	"COURSE.Syllabus.en";
#endif

const char *Hlp_COURSE_Syllabus_edit =
#if   L==1	// ca
	"COURSE.Syllabus.es#editar";
#elif L==2	// de
	"COURSE.Syllabus.en#edit";
#elif L==3	// en
	"COURSE.Syllabus.en#edit";
#elif L==4	// es
	"COURSE.Syllabus.es#editar";
#elif L==5	// fr
	"COURSE.Syllabus.en#edit";
#elif L==6	// gn
	"COURSE.Syllabus.es#editar";
#elif L==7	// it
	"COURSE.Syllabus.en#edit";
#elif L==8	// pl
	"COURSE.Syllabus.en#edit";
#elif L==9	// pt
	"COURSE.Syllabus.en#edit";
#elif L==10	// tr
	"COURSE.Syllabus.en#edit";
#endif

const char *Hlp_COURSE_Timetable =
#if   L==1	// ca
	"COURSE.Timetable.en";
#elif L==2	// de
	"COURSE.Timetable.es";
#elif L==3	// en
	"COURSE.Timetable.en";
#elif L==4	// es
	"COURSE.Timetable.es";
#elif L==5	// fr
	"COURSE.Timetable.en";
#elif L==6	// gn
	"COURSE.Timetable.es";
#elif L==7	// it
	"COURSE.Timetable.en";
#elif L==8	// pl
	"COURSE.Timetable.en";
#elif L==9	// pt
	"COURSE.Timetable.en";
#elif L==10	// tr
	"COURSE.Timetable.en";
#endif

const char *Hlp_COURSE_Bibliography =
#if   L==1	// ca
	"COURSE.Bibliography.es";
#elif L==2	// de
	"COURSE.Bibliography.en";
#elif L==3	// en
	"COURSE.Bibliography.en";
#elif L==4	// es
	"COURSE.Bibliography.es";
#elif L==5	// fr
	"COURSE.Bibliography.en";
#elif L==6	// gn
	"COURSE.Bibliography.es";
#elif L==7	// it
	"COURSE.Bibliography.en";
#elif L==8	// pl
	"COURSE.Bibliography.en";
#elif L==9	// pt
	"COURSE.Bibliography.en";
#elif L==10	// tr
	"COURSE.Bibliography.en";
#endif

const char *Hlp_COURSE_Bibliography_edit =
#if   L==1	// ca
	"COURSE.Bibliography.es#editar";
#elif L==2	// de
	"COURSE.Bibliography.en#edit";
#elif L==3	// en
	"COURSE.Bibliography.en#edit";
#elif L==4	// es
	"COURSE.Bibliography.es#editar";
#elif L==5	// fr
	"COURSE.Bibliography.en#edit";
#elif L==6	// gn
	"COURSE.Bibliography.es#editar";
#elif L==7	// it
	"COURSE.Bibliography.en#edit";
#elif L==8	// pl
	"COURSE.Bibliography.en#edit";
#elif L==9	// pt
	"COURSE.Bibliography.en#edit";
#elif L==10	// tr
	"COURSE.Bibliography.en#edit";
#endif

const char *Hlp_COURSE_FAQ =
#if   L==1	// ca
	"COURSE.FAQ.es";
#elif L==2	// de
	"COURSE.FAQ.en";
#elif L==3	// en
	"COURSE.FAQ.en";
#elif L==4	// es
	"COURSE.FAQ.es";
#elif L==5	// fr
	"COURSE.FAQ.en";
#elif L==6	// gn
	"COURSE.FAQ.es";
#elif L==7	// it
	"COURSE.FAQ.en";
#elif L==8	// pl
	"COURSE.FAQ.en";
#elif L==9	// pt
	"COURSE.FAQ.en";
#elif L==10	// tr
	"COURSE.FAQ.en";
#endif

const char *Hlp_COURSE_FAQ_edit =
#if   L==1	// ca
	"COURSE.FAQ.es#editar";
#elif L==2	// de
	"COURSE.FAQ.en#edit";
#elif L==3	// en
	"COURSE.FAQ.en#edit";
#elif L==4	// es
	"COURSE.FAQ.es#editar";
#elif L==5	// fr
	"COURSE.FAQ.en#edit";
#elif L==6	// gn
	"COURSE.FAQ.es#editar";
#elif L==7	// it
	"COURSE.FAQ.en#edit";
#elif L==8	// pl
	"COURSE.FAQ.en#edit";
#elif L==9	// pt
	"COURSE.FAQ.en#edit";
#elif L==10	// tr
	"COURSE.FAQ.en#edit";
#endif

const char *Hlp_COURSE_Links =
#if   L==1	// ca
	"COURSE.Links.es";
#elif L==2	// de
	"COURSE.Links.en";
#elif L==3	// en
	"COURSE.Links.en";
#elif L==4	// es
	"COURSE.Links.es";
#elif L==5	// fr
	"COURSE.Links.en";
#elif L==6	// gn
	"COURSE.Links.es";
#elif L==7	// it
	"COURSE.Links.en";
#elif L==8	// pl
	"COURSE.Links.en";
#elif L==9	// pt
	"COURSE.Links.en";
#elif L==10	// tr
	"COURSE.Links.en";
#endif

const char *Hlp_COURSE_Links_edit =
#if   L==1	// ca
	"COURSE.Links.es#editar";
#elif L==2	// de
	"COURSE.Links.en#edit";
#elif L==3	// en
	"COURSE.Links.en#edit";
#elif L==4	// es
	"COURSE.Links.es#editar";
#elif L==5	// fr
	"COURSE.Links.en#edit";
#elif L==6	// gn
	"COURSE.Links.es#editar";
#elif L==7	// it
	"COURSE.Links.en#edit";
#elif L==8	// pl
	"COURSE.Links.en#edit";
#elif L==9	// pt
	"COURSE.Links.en#edit";
#elif L==10	// tr
	"COURSE.Links.en#edit";
#endif

const char *Hlp_COURSE_Assessment =
#if   L==1	// ca
	"COURSE.Assessment.es";
#elif L==2	// de
	"COURSE.Assessment.en";
#elif L==3	// en
	"COURSE.Assessment.en";
#elif L==4	// es
	"COURSE.Assessment.es";
#elif L==5	// fr
	"COURSE.Assessment.en";
#elif L==6	// gn
	"COURSE.Assessment.es";
#elif L==7	// it
	"COURSE.Assessment.en";
#elif L==8	// pl
	"COURSE.Assessment.en";
#elif L==9	// pt
	"COURSE.Assessment.en";
#elif L==10	// tr
	"COURSE.Assessment.en";
#endif

const char *Hlp_COURSE_Assessment_edit =
#if   L==1	// ca
	"COURSE.Assessment.es#editar";
#elif L==2	// de
	"COURSE.Assessment.en#edit";
#elif L==3	// en
	"COURSE.Assessment.en#edit";
#elif L==4	// es
	"COURSE.Assessment.es#editar";
#elif L==5	// fr
	"COURSE.Assessment.en#edit";
#elif L==6	// gn
	"COURSE.Assessment.es#editar";
#elif L==7	// it
	"COURSE.Assessment.en#edit";
#elif L==8	// pl
	"COURSE.Assessment.en#edit";
#elif L==9	// pt
	"COURSE.Assessment.en#edit";
#elif L==10	// tr
	"COURSE.Assessment.en#edit";
#endif

/***** ASSESSMENT tab *****/

const char *Hlp_ASSESSMENT_Assignments =
#if   L==1	// ca
	"ASSESSMENT.Assignments.es";
#elif L==2	// de
	"ASSESSMENT.Assignments.en";
#elif L==3	// en
	"ASSESSMENT.Assignments.en";
#elif L==4	// es
	"ASSESSMENT.Assignments.es";
#elif L==5	// fr
	"ASSESSMENT.Assignments.en";
#elif L==6	// gn
	"ASSESSMENT.Assignments.es";
#elif L==7	// it
	"ASSESSMENT.Assignments.en";
#elif L==8	// pl
	"ASSESSMENT.Assignments.en";
#elif L==9	// pt
	"ASSESSMENT.Assignments.en";
#elif L==10	// tr
	"ASSESSMENT.Assignments.en";
#endif

const char *Hlp_ASSESSMENT_Assignments_edit_assignment =
#if   L==1	// ca
	"ASSESSMENT.Assignments.es#editar-actividad";
#elif L==2	// de
	"ASSESSMENT.Assignments.en#edit-assignment";
#elif L==3	// en
	"ASSESSMENT.Assignments.en#edit-assignment";
#elif L==4	// es
	"ASSESSMENT.Assignments.es#editar-actividad";
#elif L==5	// fr
	"ASSESSMENT.Assignments.en#edit-assignment";
#elif L==6	// gn
	"ASSESSMENT.Assignments.es#editar-actividad";
#elif L==7	// it
	"ASSESSMENT.Assignments.en#edit-assignment";
#elif L==8	// pl
	"ASSESSMENT.Assignments.en#edit-assignment";
#elif L==9	// pt
	"ASSESSMENT.Assignments.en#edit-assignment";
#elif L==10	// tr
	"ASSESSMENT.Assignments.en#edit-assignment";
#endif


const char *Hlp_ASSESSMENT_Projects =
#if   L==1	// ca
	"ASSESSMENT.Projects.es";
#elif L==2	// de
	"ASSESSMENT.Projects.en";
#elif L==3	// en
	"ASSESSMENT.Projects.en";
#elif L==4	// es
	"ASSESSMENT.Projects.es";
#elif L==5	// fr
	"ASSESSMENT.Projects.en";
#elif L==6	// gn
	"ASSESSMENT.Projects.es";
#elif L==7	// it
	"ASSESSMENT.Projects.en";
#elif L==8	// pl
	"ASSESSMENT.Projects.en";
#elif L==9	// pt
	"ASSESSMENT.Projects.en";
#elif L==10	// tr
	"ASSESSMENT.Projects.en";
#endif

const char *Hlp_ASSESSMENT_Projects_edit_project =
#if   L==1	// ca
	"ASSESSMENT.Projects.es#editar-proyecto";
#elif L==2	// de
	"ASSESSMENT.Projects.en#edit-project";
#elif L==3	// en
	"ASSESSMENT.Projects.en#edit-project";
#elif L==4	// es
	"ASSESSMENT.Projects.es#editar-proyecto";
#elif L==5	// fr
	"ASSESSMENT.Projects.en#edit-project";
#elif L==6	// gn
	"ASSESSMENT.Projects.es#editar-proyecto";
#elif L==7	// it
	"ASSESSMENT.Projects.en#edit-project";
#elif L==8	// pl
	"ASSESSMENT.Projects.en#edit-project";
#elif L==9	// pt
	"ASSESSMENT.Projects.en#edit-project";
#elif L==10	// tr
	"ASSESSMENT.Projects.en#edit-project";
#endif

const char *Hlp_ASSESSMENT_Projects_add_user =
#if   L==1	// ca
	"ASSESSMENT.Projects.es#add-user";	// TODO: Link in Spanish
#elif L==2	// de
	"ASSESSMENT.Projects.en#add-user";
#elif L==3	// en
	"ASSESSMENT.Projects.en#add-user";
#elif L==4	// es
	"ASSESSMENT.Projects.es#add-user";	// TODO: Link in Spanish
#elif L==5	// fr
	"ASSESSMENT.Projects.en#add-user";
#elif L==6	// gn
	"ASSESSMENT.Projects.es#add-user";	// TODO: Link in Spanish
#elif L==7	// it
	"ASSESSMENT.Projects.en#add-user";
#elif L==8	// pl
	"ASSESSMENT.Projects.en#add-user";
#elif L==9	// pt
	"ASSESSMENT.Projects.en#add-user";
#elif L==10	// tr
	"ASSESSMENT.Projects.en#add-user";
#endif

const char *Hlp_ASSESSMENT_Calls_for_exams =
#if   L==1	// ca
	"ASSESSMENT.Calls_for_exams.es";
#elif L==2	// de
	"ASSESSMENT.Calls_for_exams.en";
#elif L==3	// en
	"ASSESSMENT.Calls_for_exams.en";
#elif L==4	// es
	"ASSESSMENT.Calls_for_exams.es";
#elif L==5	// fr
	"ASSESSMENT.Calls_for_exams.en";
#elif L==6	// gn
	"ASSESSMENT.Calls_for_exams.es";
#elif L==7	// it
	"ASSESSMENT.Calls_for_exams.en";
#elif L==8	// pl
	"ASSESSMENT.Calls_for_exams.en";
#elif L==9	// pt
	"ASSESSMENT.Calls_for_exams.en";
#elif L==10	// tr
	"ASSESSMENT.Calls_for_exams.en";
#endif

const char *Hlp_ASSESSMENT_Calls_for_exams_new_call =
#if   L==1	// ca
	"ASSESSMENT.Calls_for_exams.es#nueva-convocatoria";
#elif L==2	// de
	"ASSESSMENT.Calls_for_exams.en#new-call";
#elif L==3	// en
	"ASSESSMENT.Calls_for_exams.en#new-call";
#elif L==4	// es
	"ASSESSMENT.Calls_for_exams.es#nueva-convocatoria";
#elif L==5	// fr
	"ASSESSMENT.Calls_for_exams.en#new-call";
#elif L==6	// gn
	"ASSESSMENT.Calls_for_exams.es#nueva-convocatoria";
#elif L==7	// it
	"ASSESSMENT.Calls_for_exams.en#new-call";
#elif L==8	// pl
	"ASSESSMENT.Calls_for_exams.en#new-call";
#elif L==9	// pt
	"ASSESSMENT.Calls_for_exams.en#new-call";
#elif L==10	// tr
	"ASSESSMENT.Calls_for_exams.en#new-call";
#endif

const char *Hlp_ASSESSMENT_Calls_for_exams_edit_call =
#if   L==1	// ca
	"ASSESSMENT.Calls_for_exams.es#editar-convocatoria";
#elif L==2	// de
	"ASSESSMENT.Calls_for_exams.en#edit-call";
#elif L==3	// en
	"ASSESSMENT.Calls_for_exams.en#edit-call";
#elif L==4	// es
	"ASSESSMENT.Calls_for_exams.es#editar-convocatoria";
#elif L==5	// fr
	"ASSESSMENT.Calls_for_exams.en#edit-call";
#elif L==6	// gn
	"ASSESSMENT.Calls_for_exams.es#editar-convocatoria";
#elif L==7	// it
	"ASSESSMENT.Calls_for_exams.en#edit-call";
#elif L==8	// pl
	"ASSESSMENT.Calls_for_exams.en#edit-call";
#elif L==9	// pt
	"ASSESSMENT.Calls_for_exams.en#edit-call";
#elif L==10	// tr
	"ASSESSMENT.Calls_for_exams.en#edit-call";
#endif

const char *Hlp_ASSESSMENT_Questions =
#if   L==1	// ca
	"ASSESSMENT.Questions.es";
#elif L==2	// de
	"ASSESSMENT.Questions.en";
#elif L==3	// en
	"ASSESSMENT.Questions.en";
#elif L==4	// es
	"ASSESSMENT.Questions.es";
#elif L==5	// fr
	"ASSESSMENT.Questions.en";
#elif L==6	// gn
	"ASSESSMENT.Questions.es";
#elif L==7	// it
	"ASSESSMENT.Questions.en";
#elif L==8	// pl
	"ASSESSMENT.Questions.en";
#elif L==9	// pt
	"ASSESSMENT.Questions.en";
#elif L==10	// tr
	"ASSESSMENT.Questions.en";
#endif

const char *Hlp_ASSESSMENT_Questions_writing_a_question =
#if   L==1	// ca
	"ASSESSMENT.Questions.es#escribir-una-pregunta";
#elif L==2	// de
	"ASSESSMENT.Questions.en#writing-a-question";
#elif L==3	// en
	"ASSESSMENT.Questions.en#writing-a-question";
#elif L==4	// es
	"ASSESSMENT.Questions.es#escribir-una-pregunta";
#elif L==5	// fr
	"ASSESSMENT.Questions.en#writing-a-question";
#elif L==6	// gn
	"ASSESSMENT.Questions.es#escribir-una-pregunta";
#elif L==7	// it
	"ASSESSMENT.Questions.en#writing-a-question";
#elif L==8	// pl
	"ASSESSMENT.Questions.en#writing-a-question";
#elif L==9	// pt
	"ASSESSMENT.Questions.en#writing-a-question";
#elif L==10	// tr
	"ASSESSMENT.Questions.en#writing-a-question";
#endif

const char *Hlp_ASSESSMENT_Questions_editing_tags =
#if   L==1	// ca
	"ASSESSMENT.Questions.es#editar-descriptores";
#elif L==2	// de
	"ASSESSMENT.Questions.en#editing-tags";
#elif L==3	// en
	"ASSESSMENT.Questions.en#editing-tags";
#elif L==4	// es
	"ASSESSMENT.Questions.es#editar-descriptores";
#elif L==5	// fr
	"ASSESSMENT.Questions.en#editing-tags";
#elif L==6	// gn
	"ASSESSMENT.Questions.es#editar-descriptores";
#elif L==7	// it
	"ASSESSMENT.Questions.en#editing-tags";
#elif L==8	// pl
	"ASSESSMENT.Questions.en#editing-tags";
#elif L==9	// pt
	"ASSESSMENT.Questions.en#editing-tags";
#elif L==10	// tr
	"ASSESSMENT.Questions.en#editing-tags";
#endif

const char *Hlp_ASSESSMENT_Tests =
#if   L==1	// ca
	"ASSESSMENT.Tests.es";
#elif L==2	// de
	"ASSESSMENT.Tests.en";
#elif L==3	// en
	"ASSESSMENT.Tests.en";
#elif L==4	// es
	"ASSESSMENT.Tests.es";
#elif L==5	// fr
	"ASSESSMENT.Tests.en";
#elif L==6	// gn
	"ASSESSMENT.Tests.es";
#elif L==7	// it
	"ASSESSMENT.Tests.en";
#elif L==8	// pl
	"ASSESSMENT.Tests.en";
#elif L==9	// pt
	"ASSESSMENT.Tests.en";
#elif L==10	// tr
	"ASSESSMENT.Tests.en";
#endif

const char *Hlp_ASSESSMENT_Tests_configuring_tests =
#if   L==1	// ca
	"ASSESSMENT.Tests.es#configurar-test";
#elif L==2	// de
	"ASSESSMENT.Tests.en#configuring-tests";
#elif L==3	// en
	"ASSESSMENT.Tests.en#configuring-tests";
#elif L==4	// es
	"ASSESSMENT.Tests.es#configurar-test";
#elif L==5	// fr
	"ASSESSMENT.Tests.en#configuring-tests";
#elif L==6	// gn
	"ASSESSMENT.Tests.es#configurar-test";
#elif L==7	// it
	"ASSESSMENT.Tests.en#configuring-tests";
#elif L==8	// pl
	"ASSESSMENT.Tests.en#configuring-tests";
#elif L==9	// pt
	"ASSESSMENT.Tests.en#configuring-tests";
#elif L==10	// tr
	"ASSESSMENT.Tests.en#configuring-tests";
#endif

const char *Hlp_ASSESSMENT_Tests_results =
#if   L==1	// ca
	"ASSESSMENT.Tests.es#resultados";
#elif L==2	// de
	"ASSESSMENT.Tests.en#results";
#elif L==3	// en
	"ASSESSMENT.Tests.en#results";
#elif L==4	// es
	"ASSESSMENT.Tests.es#resultados";
#elif L==5	// fr
	"ASSESSMENT.Tests.en#results";
#elif L==6	// gn
	"ASSESSMENT.Tests.es#resultados";
#elif L==7	// it
	"ASSESSMENT.Tests.en#results";
#elif L==8	// pl
	"ASSESSMENT.Tests.en#results";
#elif L==9	// pt
	"ASSESSMENT.Tests.en#results";
#elif L==10	// tr
	"ASSESSMENT.Tests.en#results";
#endif

const char *Hlp_ASSESSMENT_Exams =
#if   L==1	// ca
	"ASSESSMENT.Exams.es";
#elif L==2	// de
	"ASSESSMENT.Exams.en";
#elif L==3	// en
	"ASSESSMENT.Exams.en";
#elif L==4	// es
	"ASSESSMENT.Exams.es";
#elif L==5	// fr
	"ASSESSMENT.Exams.en";
#elif L==6	// gn
	"ASSESSMENT.Exams.es";
#elif L==7	// it
	"ASSESSMENT.Exams.en";
#elif L==8	// pl
	"ASSESSMENT.Exams.en";
#elif L==9	// pt
	"ASSESSMENT.Exams.en";
#elif L==10	// tr
	"ASSESSMENT.Exams.en";
#endif

const char *Hlp_ASSESSMENT_Exams_edit_exam =
#if   L==1	// ca
	"ASSESSMENT.Exams.es#editar-examen";
#elif L==2	// de
	"ASSESSMENT.Exams.en#edit-exam";
#elif L==3	// en
	"ASSESSMENT.Exams.en#edit-exam";
#elif L==4	// es
	"ASSESSMENT.Exams.es#editar-examen";
#elif L==5	// fr
	"ASSESSMENT.Exams.en#edit-exam";
#elif L==6	// gn
	"ASSESSMENT.Exams.es#editar-examen";
#elif L==7	// it
	"ASSESSMENT.Exams.en#edit-exam";
#elif L==8	// pl
	"ASSESSMENT.Exams.en#edit-exam";
#elif L==9	// pt
	"ASSESSMENT.Exams.en#edit-exam";
#elif L==10	// tr
	"ASSESSMENT.Exams.en#edit-exam";
#endif

const char *Hlp_ASSESSMENT_Exams_new_exam =
#if   L==1	// ca
	"ASSESSMENT.Exams.es#nuevo-examen";
#elif L==2	// de
	"ASSESSMENT.Exams.en#new-exam";
#elif L==3	// en
	"ASSESSMENT.Exams.en#new-exam";
#elif L==4	// es
	"ASSESSMENT.Exams.es#nuevo-examen";
#elif L==5	// fr
	"ASSESSMENT.Exams.en#new-exam";
#elif L==6	// gn
	"ASSESSMENT.Exams.es#nuevo-examen";
#elif L==7	// it
	"ASSESSMENT.Exams.en#new-exam";
#elif L==8	// pl
	"ASSESSMENT.Exams.en#new-exam";
#elif L==9	// pt
	"ASSESSMENT.Exams.en#new-exam";
#elif L==10	// tr
	"ASSESSMENT.Exams.en#new-exam";
#endif

const char *Hlp_ASSESSMENT_Exams_question_sets =
#if   L==1	// ca
	"ASSESSMENT.Exams.es#conjuntos-de-preguntas";
#elif L==2	// de
	"ASSESSMENT.Exams.en#question-sets";
#elif L==3	// en
	"ASSESSMENT.Exams.en#question-sets";
#elif L==4	// es
	"ASSESSMENT.Exams.es#conjuntos-de-preguntas";
#elif L==5	// fr
	"ASSESSMENT.Exams.en#question-sets";
#elif L==6	// gn
	"ASSESSMENT.Exams.es#conjuntos-de-preguntas";
#elif L==7	// it
	"ASSESSMENT.Exams.en#question-sets";
#elif L==8	// pl
	"ASSESSMENT.Exams.en#question-sets";
#elif L==9	// pt
	"ASSESSMENT.Exams.en#question-sets";
#elif L==10	// tr
	"ASSESSMENT.Exams.en#question-sets";
#endif

const char *Hlp_ASSESSMENT_Exams_sessions =
#if   L==1	// ca
	"ASSESSMENT.Exams.es#sesiones";
#elif L==2	// de
	"ASSESSMENT.Exams.en#sessions";
#elif L==3	// en
	"ASSESSMENT.Exams.en#sessions";
#elif L==4	// es
	"ASSESSMENT.Exams.es#sesiones";
#elif L==5	// fr
	"ASSESSMENT.Exams.en#sessions";
#elif L==6	// gn
	"ASSESSMENT.Exams.es#sesiones";
#elif L==7	// it
	"ASSESSMENT.Exams.en#sessions";
#elif L==8	// pl
	"ASSESSMENT.Exams.en#sessions";
#elif L==9	// pt
	"ASSESSMENT.Exams.en#sessions";
#elif L==10	// tr
	"ASSESSMENT.Exams.en#sessions";
#endif

const char *Hlp_ASSESSMENT_Exams_questions =
#if   L==1	// ca
	"ASSESSMENT.Exams.es#preguntas";
#elif L==2	// de
	"ASSESSMENT.Exams.en#questions";
#elif L==3	// en
	"ASSESSMENT.Exams.en#questions";
#elif L==4	// es
	"ASSESSMENT.Exams.es#preguntas";
#elif L==5	// fr
	"ASSESSMENT.Exams.en#questions";
#elif L==6	// gn
	"ASSESSMENT.Exams.es#preguntas";
#elif L==7	// it
	"ASSESSMENT.Exams.en#questions";
#elif L==8	// pl
	"ASSESSMENT.Exams.en#questions";
#elif L==9	// pt
	"ASSESSMENT.Exams.en#questions";
#elif L==10	// tr
	"ASSESSMENT.Exams.en#questions";
#endif

const char *Hlp_ASSESSMENT_Exams_answer_exam =
#if   L==1	// ca
	"ASSESSMENT.Exams.es#responder-examen";
#elif L==2	// de
	"ASSESSMENT.Exams.en#answer-exam";
#elif L==3	// en
	"ASSESSMENT.Exams.en#answer-exam";
#elif L==4	// es
	"ASSESSMENT.Exams.es#responder-examen";
#elif L==5	// fr
	"ASSESSMENT.Exams.en#answer-exam";
#elif L==6	// gn
	"ASSESSMENT.Exams.es#responder-examen";
#elif L==7	// it
	"ASSESSMENT.Exams.en#answer-exam";
#elif L==8	// pl
	"ASSESSMENT.Exams.en#answer-exam";
#elif L==9	// pt
	"ASSESSMENT.Exams.en#answer-exam";
#elif L==10	// tr
	"ASSESSMENT.Exams.en#answer-exam";
#endif

const char *Hlp_ASSESSMENT_Exams_results =
#if   L==1	// ca
	"ASSESSMENT.Exams.es#resultados";
#elif L==2	// de
	"ASSESSMENT.Exams.en#results";
#elif L==3	// en
	"ASSESSMENT.Exams.en#results";
#elif L==4	// es
	"ASSESSMENT.Exams.es#resultados";
#elif L==5	// fr
	"ASSESSMENT.Exams.en#results";
#elif L==6	// gn
	"ASSESSMENT.Exams.es#resultados";
#elif L==7	// it
	"ASSESSMENT.Exams.en#results";
#elif L==8	// pl
	"ASSESSMENT.Exams.en#results";
#elif L==9	// pt
	"ASSESSMENT.Exams.en#results";
#elif L==10	// tr
	"ASSESSMENT.Exams.en#results";
#endif

const char *Hlp_ASSESSMENT_Games =
#if   L==1	// ca
	"ASSESSMENT.Games.es";
#elif L==2	// de
	"ASSESSMENT.Games.en";
#elif L==3	// en
	"ASSESSMENT.Games.en";
#elif L==4	// es
	"ASSESSMENT.Games.es";
#elif L==5	// fr
	"ASSESSMENT.Games.en";
#elif L==6	// gn
	"ASSESSMENT.Games.es";
#elif L==7	// it
	"ASSESSMENT.Games.en";
#elif L==8	// pl
	"ASSESSMENT.Games.en";
#elif L==9	// pt
	"ASSESSMENT.Games.en";
#elif L==10	// tr
	"ASSESSMENT.Games.en";
#endif

const char *Hlp_ASSESSMENT_Games_edit_game =
#if   L==1	// ca
	"ASSESSMENT.Games.es#editar-juego";
#elif L==2	// de
	"ASSESSMENT.Games.en#edit-game";
#elif L==3	// en
	"ASSESSMENT.Games.en#edit-game";
#elif L==4	// es
	"ASSESSMENT.Games.es#editar-juego";
#elif L==5	// fr
	"ASSESSMENT.Games.en#edit-game";
#elif L==6	// gn
	"ASSESSMENT.Games.es#editar-juego";
#elif L==7	// it
	"ASSESSMENT.Games.en#edit-game";
#elif L==8	// pl
	"ASSESSMENT.Games.en#edit-game";
#elif L==9	// pt
	"ASSESSMENT.Games.en#edit-game";
#elif L==10	// tr
	"ASSESSMENT.Games.en#edit-game";
#endif

const char *Hlp_ASSESSMENT_Games_new_game =
#if   L==1	// ca
	"ASSESSMENT.Games.es#nuevo-juego";
#elif L==2	// de
	"ASSESSMENT.Games.en#new-game";
#elif L==3	// en
	"ASSESSMENT.Games.en#new-game";
#elif L==4	// es
	"ASSESSMENT.Games.es#nuevo-juego";
#elif L==5	// fr
	"ASSESSMENT.Games.en#new-game";
#elif L==6	// gn
	"ASSESSMENT.Games.es#nuevo-juego";
#elif L==7	// it
	"ASSESSMENT.Games.en#new-game";
#elif L==8	// pl
	"ASSESSMENT.Games.en#new-game";
#elif L==9	// pt
	"ASSESSMENT.Games.en#new-game";
#elif L==10	// tr
	"ASSESSMENT.Games.en#new-game";
#endif

const char *Hlp_ASSESSMENT_Games_matches =
#if   L==1	// ca
	"ASSESSMENT.Games.es#partidas";
#elif L==2	// de
	"ASSESSMENT.Games.en#matches";
#elif L==3	// en
	"ASSESSMENT.Games.en#matches";
#elif L==4	// es
	"ASSESSMENT.Games.es#partidas";
#elif L==5	// fr
	"ASSESSMENT.Games.en#matches";
#elif L==6	// gn
	"ASSESSMENT.Games.es#partidas";
#elif L==7	// it
	"ASSESSMENT.Games.en#matches";
#elif L==8	// pl
	"ASSESSMENT.Games.en#matches";
#elif L==9	// pt
	"ASSESSMENT.Games.en#matches";
#elif L==10	// tr
	"ASSESSMENT.Games.en#matches";
#endif

const char *Hlp_ASSESSMENT_Games_questions =
#if   L==1	// ca
	"ASSESSMENT.Games.es#preguntas";
#elif L==2	// de
	"ASSESSMENT.Games.en#questions";
#elif L==3	// en
	"ASSESSMENT.Games.en#questions";
#elif L==4	// es
	"ASSESSMENT.Games.es#preguntas";
#elif L==5	// fr
	"ASSESSMENT.Games.en#questions";
#elif L==6	// gn
	"ASSESSMENT.Games.es#preguntas";
#elif L==7	// it
	"ASSESSMENT.Games.en#questions";
#elif L==8	// pl
	"ASSESSMENT.Games.en#questions";
#elif L==9	// pt
	"ASSESSMENT.Games.en#questions";
#elif L==10	// tr
	"ASSESSMENT.Games.en#questions";
#endif

const char *Hlp_ASSESSMENT_Games_results =
#if   L==1	// ca
	"ASSESSMENT.Games.es#resultados";
#elif L==2	// de
	"ASSESSMENT.Games.en#results";
#elif L==3	// en
	"ASSESSMENT.Games.en#results";
#elif L==4	// es
	"ASSESSMENT.Games.es#resultados";
#elif L==5	// fr
	"ASSESSMENT.Games.en#results";
#elif L==6	// gn
	"ASSESSMENT.Games.es#resultados";
#elif L==7	// it
	"ASSESSMENT.Games.en#results";
#elif L==8	// pl
	"ASSESSMENT.Games.en#results";
#elif L==9	// pt
	"ASSESSMENT.Games.en#results";
#elif L==10	// tr
	"ASSESSMENT.Games.en#results";
#endif

const char *Hlp_ASSESSMENT_Rubrics =
#if   L==1	// ca
	"ASSESSMENT.Rubrics.es";
#elif L==2	// de
	"ASSESSMENT.Rubrics.en";
#elif L==3	// en
	"ASSESSMENT.Rubrics.en";
#elif L==4	// es
	"ASSESSMENT.Rubrics.es";
#elif L==5	// fr
	"ASSESSMENT.Rubrics.en";
#elif L==6	// gn
	"ASSESSMENT.Rubrics.es";
#elif L==7	// it
	"ASSESSMENT.Rubrics.en";
#elif L==8	// pl
	"ASSESSMENT.Rubrics.en";
#elif L==9	// pt
	"ASSESSMENT.Rubrics.en";
#elif L==10	// tr
	"ASSESSMENT.Rubrics.en";
#endif

const char *Hlp_ASSESSMENT_Rubrics_edit_rubric =
#if   L==1	// ca
	"ASSESSMENT.Rubrics.es#editar-r%C3%BAbrica";
#elif L==2	// de
	"ASSESSMENT.Rubrics.en#edit-rubric";
#elif L==3	// en
	"ASSESSMENT.Rubrics.en#edit-rubric";
#elif L==4	// es
	"ASSESSMENT.Rubrics.es#editar-r%C3%BAbrica";
#elif L==5	// fr
	"ASSESSMENT.Rubrics.en#edit-rubric";
#elif L==6	// gn
	"ASSESSMENT.Rubrics.es#editar-r%C3%BAbrica";
#elif L==7	// it
	"ASSESSMENT.Rubrics.en#edit-rubric";
#elif L==8	// pl
	"ASSESSMENT.Rubrics.en#edit-rubric";
#elif L==9	// pt
	"ASSESSMENT.Rubrics.en#edit-rubric";
#elif L==10	// tr
	"ASSESSMENT.Rubrics.en#edit-rubric";
#endif

const char *Hlp_ASSESSMENT_Rubrics_new_rubric =
#if   L==1	// ca
	"ASSESSMENT.Rubrics.es#nuevo-r%C3%BAbrica";
#elif L==2	// de
	"ASSESSMENT.Rubrics.en#new-rubric";
#elif L==3	// en
	"ASSESSMENT.Rubrics.en#new-rubric";
#elif L==4	// es
	"ASSESSMENT.Rubrics.es#nueva-r%C3%BAbrica";
#elif L==5	// fr
	"ASSESSMENT.Rubrics.en#new-rubric";
#elif L==6	// gn
	"ASSESSMENT.Rubrics.es#nueva-r%C3%BAbrica";
#elif L==7	// it
	"ASSESSMENT.Rubrics.en#new-rubric";
#elif L==8	// pl
	"ASSESSMENT.Rubrics.en#new-rubric";
#elif L==9	// pt
	"ASSESSMENT.Rubrics.en#new-rubric";
#elif L==10	// tr
	"ASSESSMENT.Rubrics.en#new-rubric";
#endif

const char *Hlp_ASSESSMENT_Rubrics_criteria =
#if   L==1	// ca
	"ASSESSMENT.Rubrics.es#criteria";
#elif L==2	// de
	"ASSESSMENT.Rubrics.en#criteria";
#elif L==3	// en
	"ASSESSMENT.Rubrics.en#criteria";
#elif L==4	// es
	"ASSESSMENT.Rubrics.es#criterios";
#elif L==5	// fr
	"ASSESSMENT.Rubrics.en#criteria";
#elif L==6	// gn
	"ASSESSMENT.Rubrics.es#criterios";
#elif L==7	// it
	"ASSESSMENT.Rubrics.en#criteria";
#elif L==8	// pl
	"ASSESSMENT.Rubrics.en#criteria";
#elif L==9	// pt
	"ASSESSMENT.Rubrics.en#criteria";
#elif L==10	// tr
	"ASSESSMENT.Rubrics.en#criteria";
#endif

const char *Hlp_ASSESSMENT_Rubrics_resource_clipboard =
#if   L==1	// ca
	"ASSESSMENT.Rubrics.es#resource-clipboard";
#elif L==2	// de
	"ASSESSMENT.Rubrics.en#resource-clipboard";
#elif L==3	// en
	"ASSESSMENT.Rubrics.en#resource-clipboard";
#elif L==4	// es
	"ASSESSMENT.Rubrics.es#portapapeles-de-recursos";
#elif L==5	// fr
	"ASSESSMENT.Rubrics.en#resource-clipboard";
#elif L==6	// gn
	"ASSESSMENT.Rubrics.es#portapapeles-de-recursos";
#elif L==7	// it
	"ASSESSMENT.Rubrics.en#resource-clipboard";
#elif L==8	// pl
	"ASSESSMENT.Rubrics.en#resource-clipboard";
#elif L==9	// pt
	"ASSESSMENT.Rubrics.en#resource-clipboard";
#elif L==10	// tr
	"ASSESSMENT.Rubrics.en#resource-clipboard";
#endif

/***** FILES tab *****/

const char *Hlp_FILES_Documents =
#if   L==1	// ca
	"FILES.Documents.es";
#elif L==2	// de
	"FILES.Documents.en";
#elif L==3	// en
	"FILES.Documents.en";
#elif L==4	// es
	"FILES.Documents.es";
#elif L==5	// fr
	"FILES.Documents.en";
#elif L==6	// gn
	"FILES.Documents.es";
#elif L==7	// it
	"FILES.Documents.en";
#elif L==8	// pl
	"FILES.Documents.en";
#elif L==9	// pt
	"FILES.Documents.en";
#elif L==10	// tr
	"FILES.Documents.en";
#endif

const char *Hlp_FILES_Private =
#if   L==1	// ca
	"FILES.Private.es";
#elif L==2	// de
	"FILES.Private.en";
#elif L==3	// en
	"FILES.Private.en";
#elif L==4	// es
	"FILES.Private.es";
#elif L==5	// fr
	"FILES.Private.en";
#elif L==6	// gn
	"FILES.Private.es";
#elif L==7	// it
	"FILES.Private.en";
#elif L==8	// pl
	"FILES.Private.en";
#elif L==9	// pt
	"FILES.Private.en";
#elif L==10	// tr
	"FILES.Private.en";
#endif

const char *Hlp_FILES_Shared =
#if   L==1	// ca
	"FILES.Shared.es";
#elif L==2	// de
	"FILES.Shared.en";
#elif L==3	// en
	"FILES.Shared.en";
#elif L==4	// es
	"FILES.Shared.es";
#elif L==5	// fr
	"FILES.Shared.en";
#elif L==6	// gn
	"FILES.Shared.es";
#elif L==7	// it
	"FILES.Shared.en";
#elif L==8	// pl
	"FILES.Shared.en";
#elif L==9	// pt
	"FILES.Shared.en";
#elif L==10	// tr
	"FILES.Shared.en";
#endif

const char *Hlp_FILES_Homework_for_students =
#if   L==1	// ca
	"FILES.Homework.es#para-estudiantes";
#elif L==2	// de
	"FILES.Homework.en#for-students";
#elif L==3	// en
	"FILES.Homework.en#for-students";
#elif L==4	// es
	"FILES.Homework.es#para-estudiantes";
#elif L==5	// fr
	"FILES.Homework.en#for-students";
#elif L==6	// gn
	"FILES.Homework.es#para-estudiantes";
#elif L==7	// it
	"FILES.Homework.en#for-students";
#elif L==8	// pl
	"FILES.Homework.en#for-students";
#elif L==9	// pt
	"FILES.Homework.en#for-students";
#elif L==10	// tr
	"FILES.Homework.en#for-students";
#endif

const char *Hlp_FILES_Homework_for_teachers =
#if   L==1	// ca
	"FILES.Homework.es#para-profesores";
#elif L==2	// de
	"FILES.Homework.en#for-teachers";
#elif L==3	// en
	"FILES.Homework.en#for-teachers";
#elif L==4	// es
	"FILES.Homework.es#para-profesores";
#elif L==5	// fr
	"FILES.Homework.en#for-teachers";
#elif L==6	// gn
	"FILES.Homework.es#para-profesores";
#elif L==7	// it
	"FILES.Homework.en#for-teachers";
#elif L==8	// pl
	"FILES.Homework.en#for-teachers";
#elif L==9	// pt
	"FILES.Homework.en#for-teachers";
#elif L==10	// tr
	"FILES.Homework.en#for-teachers";
#endif

const char *Hlp_FILES_Marks =
#if   L==1	// ca
	"FILES.Marks.es";
#elif L==2	// de
	"FILES.Marks.en";
#elif L==3	// en
	"FILES.Marks.en";
#elif L==4	// es
	"FILES.Marks.es";
#elif L==5	// fr
	"FILES.Marks.en";
#elif L==6	// gn
	"FILES.Marks.es";
#elif L==7	// it
	"FILES.Marks.en";
#elif L==8	// pl
	"FILES.Marks.en";
#elif L==9	// pt
	"FILES.Marks.en";
#elif L==10	// tr
	"FILES.Marks.en";
#endif

const char *Hlp_FILES_Briefcase =
#if   L==1	// ca
	"FILES.Briefcase.es";
#elif L==2	// de
	"FILES.Briefcase.en";
#elif L==3	// en
	"FILES.Briefcase.en";
#elif L==4	// es
	"FILES.Briefcase.es";
#elif L==5	// fr
	"FILES.Briefcase.en";
#elif L==6	// gn
	"FILES.Briefcase.es";
#elif L==7	// it
	"FILES.Briefcase.en";
#elif L==8	// pl
	"FILES.Briefcase.en";
#elif L==9	// pt
	"FILES.Briefcase.en";
#elif L==10	// tr
	"FILES.Briefcase.en";
#endif

/***** USERS tab *****/

const char *Hlp_USERS_Groups =
#if   L==1	// ca
	"USERS.Groups.es";
#elif L==2	// de
	"USERS.Groups.en";
#elif L==3	// en
	"USERS.Groups.en";
#elif L==4	// es
	"USERS.Groups.es";
#elif L==5	// fr
	"USERS.Groups.en";
#elif L==6	// gn
	"USERS.Groups.es";
#elif L==7	// it
	"USERS.Groups.en";
#elif L==8	// pl
	"USERS.Groups.en";
#elif L==9	// pt
	"USERS.Groups.en";
#elif L==10	// tr
	"USERS.Groups.en";
#endif

const char *Hlp_USERS_Clipboard =
#if   L==1	// ca
	"USERS.Clipboard.es";
#elif L==2	// de
	"USERS.Clipboard.en";
#elif L==3	// en
	"USERS.Clipboard.en";
#elif L==4	// es
	"USERS.Clipboard.es";
#elif L==5	// fr
	"USERS.Clipboard.en";
#elif L==6	// gn
	"USERS.Clipboard.es";
#elif L==7	// it
	"USERS.Clipboard.en";
#elif L==8	// pl
	"USERS.Clipboard.en";
#elif L==9	// pt
	"USERS.Clipboard.en";
#elif L==10	// tr
	"USERS.Clipboard.en";
#endif

const char *Hlp_USERS_Students =
#if   L==1	// ca
	"USERS.Students.es";
#elif L==2	// de
	"USERS.Students.en";
#elif L==3	// en
	"USERS.Students.en";
#elif L==4	// es
	"USERS.Students.es";
#elif L==5	// fr
	"USERS.Students.en";
#elif L==6	// gn
	"USERS.Students.es";
#elif L==7	// it
	"USERS.Students.en";
#elif L==8	// pl
	"USERS.Students.en";
#elif L==9	// pt
	"USERS.Students.en";
#elif L==10	// tr
	"USERS.Students.en";
#endif

const char *Hlp_USERS_Students_shared_record_card =
#if   L==1	// ca
	"USERS.Students.es#ficha-compartida";
#elif L==2	// de
	"USERS.Students.en#shared-record-card";
#elif L==3	// en
	"USERS.Students.en#shared-record-card";
#elif L==4	// es
	"USERS.Students.es#ficha-compartida";
#elif L==5	// fr
	"USERS.Students.en#shared-record-card";
#elif L==6	// gn
	"USERS.Students.es#ficha-compartida";
#elif L==7	// it
	"USERS.Students.en#shared-record-card";
#elif L==8	// pl
	"USERS.Students.en#shared-record-card";
#elif L==9	// pt
	"USERS.Students.en#shared-record-card";
#elif L==10	// tr
	"USERS.Students.en#shared-record-card";
#endif

const char *Hlp_USERS_Students_course_record_card =
#if   L==1	// ca
	"USERS.Students.es#ficha-de-la-asignatura";
#elif L==2	// de
	"USERS.Students.en#course-record-card";
#elif L==3	// en
	"USERS.Students.en#course-record-card";
#elif L==4	// es
	"USERS.Students.es#ficha-de-la-asignatura";
#elif L==5	// fr
	"USERS.Students.en#course-record-card";
#elif L==6	// gn
	"USERS.Students.es#ficha-de-la-asignatura";
#elif L==7	// it
	"USERS.Students.en#course-record-card";
#elif L==8	// pl
	"USERS.Students.en#course-record-card";
#elif L==9	// pt
	"USERS.Students.en#course-record-card";
#elif L==10	// tr
	"USERS.Students.en#course-record-card";
#endif

const char *Hlp_USERS_Administration_administer_one_user =
#if   L==1	// ca
	"USERS.Administration.es#administrar-un-usuario";
#elif L==2	// de
	"USERS.Administration.en#administer-one-user";
#elif L==3	// en
	"USERS.Administration.en#administer-one-user";
#elif L==4	// es
	"USERS.Administration.es#administrar-un-usuario";
#elif L==5	// fr
	"USERS.Administration.en#administer-one-user";
#elif L==6	// gn
	"USERS.Administration.es#administrar-un-usuario";
#elif L==7	// it
	"USERS.Administration.en#administer-one-user";
#elif L==8	// pl
	"USERS.Administration.en#administer-one-user";
#elif L==9	// pt
	"USERS.Administration.en#administer-one-user";
#elif L==10	// tr
	"USERS.Administration.en#administer-one-user";
#endif

const char *Hlp_USERS_Administration_administer_multiple_users =
#if   L==1	// ca
	"USERS.Administration.es#administrar-varios-usuarios";
#elif L==2	// de
	"USERS.Administration.en#administer-multiple-users";
#elif L==3	// en
	"USERS.Administration.en#administer-multiple-users";
#elif L==4	// es
	"USERS.Administration.es#administrar-varios-usuarios";
#elif L==5	// fr
	"USERS.Administration.en#administer-multiple-users";
#elif L==6	// gn
	"USERS.Administration.es#administrar-varios-usuarios";
#elif L==7	// it
	"USERS.Administration.en#administer-multiple-users";
#elif L==8	// pl
	"USERS.Administration.en#administer-multiple-users";
#elif L==9	// pt
	"USERS.Administration.en#administer-multiple-users";
#elif L==10	// tr
	"USERS.Administration.en#administer-multiple-users";
#endif

const char *Hlp_USERS_Administration_remove_all_students =
#if   L==1	// ca
	"USERS.Administration.es#eliminar-todos-los-estudiantes";
#elif L==2	// de
	"USERS.Administration.en#remove-all-students";
#elif L==3	// en
	"USERS.Administration.en#remove-all-students";
#elif L==4	// es
	"USERS.Administration.es#eliminar-todos-los-estudiantes";
#elif L==5	// fr
	"USERS.Administration.en#remove-all-students";
#elif L==6	// gn
	"USERS.Administration.es#eliminar-todos-los-estudiantes";
#elif L==7	// it
	"USERS.Administration.en#remove-all-students";
#elif L==8	// pl
	"USERS.Administration.en#remove-all-students";
#elif L==9	// pt
	"USERS.Administration.en#remove-all-students";
#elif L==10	// tr
	"USERS.Administration.en#remove-all-students";
#endif

const char *Hlp_USERS_Teachers =
#if   L==1	// ca
	"USERS.Teachers.es";
#elif L==2	// de
	"USERS.Teachers.en";
#elif L==3	// en
	"USERS.Teachers.en";
#elif L==4	// es
	"USERS.Teachers.es";
#elif L==5	// fr
	"USERS.Teachers.en";
#elif L==6	// gn
	"USERS.Teachers.es";
#elif L==7	// it
	"USERS.Teachers.en";
#elif L==8	// pl
	"USERS.Teachers.en";
#elif L==9	// pt
	"USERS.Teachers.en";
#elif L==10	// tr
	"USERS.Teachers.en";
#endif

const char *Hlp_USERS_Teachers_shared_record_card =
#if   L==1	// ca
	"USERS.Teachers.es#ficha-compartida";
#elif L==2	// de
	"USERS.Teachers.en#shared-record-card";
#elif L==3	// en
	"USERS.Teachers.en#shared-record-card";
#elif L==4	// es
	"USERS.Teachers.es#ficha-compartida";
#elif L==5	// fr
	"USERS.Teachers.en#shared-record-card";
#elif L==6	// gn
	"USERS.Teachers.es#ficha-compartida";
#elif L==7	// it
	"USERS.Teachers.en#shared-record-card";
#elif L==8	// pl
	"USERS.Teachers.en#shared-record-card";
#elif L==9	// pt
	"USERS.Teachers.en#shared-record-card";
#elif L==10	// tr
	"USERS.Teachers.en#shared-record-card";
#endif

const char *Hlp_USERS_Teachers_timetable =
#if   L==1	// ca
	"USERS.Teachers.es#horario";
#elif L==2	// de
	"USERS.Teachers.en#timetable";
#elif L==3	// en
	"USERS.Teachers.en#timetable";
#elif L==4	// es
	"USERS.Teachers.es#horario";
#elif L==5	// fr
	"USERS.Teachers.en#timetable";
#elif L==6	// gn
	"USERS.Teachers.es#horario";
#elif L==7	// it
	"USERS.Teachers.en#timetable";
#elif L==8	// pl
	"USERS.Teachers.en#timetable";
#elif L==9	// pt
	"USERS.Teachers.en#timetable";
#elif L==10	// tr
	"USERS.Teachers.en#timetable";
#endif

const char *Hlp_USERS_Administrators =
#if   L==1	// ca
	"USERS.Administrators.es";
#elif L==2	// de
	"USERS.Administrators.en";
#elif L==3	// en
	"USERS.Administrators.en";
#elif L==4	// es
	"USERS.Administrators.es";
#elif L==5	// fr
	"USERS.Administrators.en";
#elif L==6	// gn
	"USERS.Administrators.es";
#elif L==7	// it
	"USERS.Administrators.en";
#elif L==8	// pl
	"USERS.Administrators.en";
#elif L==9	// pt
	"USERS.Administrators.en";
#elif L==10	// tr
	"USERS.Administrators.en";
#endif

const char *Hlp_USERS_Guests =
#if   L==1	// ca
	"USERS.Guests.es";
#elif L==2	// de
	"USERS.Guests.en";
#elif L==3	// en
	"USERS.Guests.en";
#elif L==4	// es
	"USERS.Guests.es";
#elif L==5	// fr
	"USERS.Guests.en";
#elif L==6	// gn
	"USERS.Guests.es";
#elif L==7	// it
	"USERS.Guests.en";
#elif L==8	// pl
	"USERS.Guests.en";
#elif L==9	// pt
	"USERS.Guests.en";
#elif L==10	// tr
	"USERS.Guests.en";
#endif

const char *Hlp_USERS_Duplicates_possibly_duplicate_users =
#if   L==1	// ca
	"USERS.Duplicates.es#usuarios-posiblemente-duplicados";
#elif L==2	// de
	"USERS.Duplicates.en#possibly-duplicate-users";
#elif L==3	// en
	"USERS.Duplicates.en#possibly-duplicate-users";
#elif L==4	// es
	"USERS.Duplicates.es#usuarios-posiblemente-duplicados";
#elif L==5	// fr
	"USERS.Duplicates.en#possibly-duplicate-users";
#elif L==6	// gn
	"USERS.Duplicates.es#usuarios-posiblemente-duplicados";
#elif L==7	// it
	"USERS.Duplicates.en#possibly-duplicate-users";
#elif L==8	// pl
	"USERS.Duplicates.en#possibly-duplicate-users";
#elif L==9	// pt
	"USERS.Duplicates.en#possibly-duplicate-users";
#elif L==10	// tr
	"USERS.Duplicates.en#possibly-duplicate-users";
#endif

const char *Hlp_USERS_Duplicates_similar_users =
#if   L==1	// ca
	"USERS.Duplicates.es#usuarios-similares";
#elif L==2	// de
	"USERS.Duplicates.en#similar-users";
#elif L==3	// en
	"USERS.Duplicates.en#similar-users";
#elif L==4	// es
	"USERS.Duplicates.es#usuarios-similares";
#elif L==5	// fr
	"USERS.Duplicates.en#similar-users";
#elif L==6	// gn
	"USERS.Duplicates.es#usuarios-similares";
#elif L==7	// it
	"USERS.Duplicates.en#similar-users";
#elif L==8	// pl
	"USERS.Duplicates.en#similar-users";
#elif L==9	// pt
	"USERS.Duplicates.en#similar-users";
#elif L==10	// tr
	"USERS.Duplicates.en#similar-users";
#endif

const char *Hlp_USERS_Attendance =
#if   L==1	// ca
	"USERS.Attendance.es";
#elif L==2	// de
	"USERS.Attendance.en";
#elif L==3	// en
	"USERS.Attendance.en";
#elif L==4	// es
	"USERS.Attendance.es";
#elif L==5	// fr
	"USERS.Attendance.en";
#elif L==6	// gn
	"USERS.Attendance.es";
#elif L==7	// it
	"USERS.Attendance.en";
#elif L==8	// pl
	"USERS.Attendance.en";
#elif L==9	// pt
	"USERS.Attendance.en";
#elif L==10	// tr
	"USERS.Attendance.en";
#endif

const char *Hlp_USERS_Attendance_new_event =
#if   L==1	// ca
	"USERS.Attendance.es#nuevo-evento";
#elif L==2	// de
	"USERS.Attendance.en#new-event";
#elif L==3	// en
	"USERS.Attendance.en#new-event";
#elif L==4	// es
	"USERS.Attendance.es#nuevo-evento";
#elif L==5	// fr
	"USERS.Attendance.en#new-event";
#elif L==6	// gn
	"USERS.Attendance.es#nuevo-evento";
#elif L==7	// it
	"USERS.Attendance.en#new-event";
#elif L==8	// pl
	"USERS.Attendance.en#new-event";
#elif L==9	// pt
	"USERS.Attendance.en#new-event";
#elif L==10	// tr
	"USERS.Attendance.en#new-event";
#endif

const char *Hlp_USERS_Attendance_edit_event =
#if   L==1	// ca
	"USERS.Attendance.es#editar-evento";
#elif L==2	// de
	"USERS.Attendance.en#edit-event";
#elif L==3	// en
	"USERS.Attendance.en#edit-event";
#elif L==4	// es
	"USERS.Attendance.es#editar-evento";
#elif L==5	// fr
	"USERS.Attendance.en#edit-event";
#elif L==6	// gn
	"USERS.Attendance.es#editar-evento";
#elif L==7	// it
	"USERS.Attendance.en#edit-event";
#elif L==8	// pl
	"USERS.Attendance.en#edit-event";
#elif L==9	// pt
	"USERS.Attendance.en#edit-event";
#elif L==10	// tr
	"USERS.Attendance.en#edit-event";
#endif

const char *Hlp_USERS_Attendance_attendance_list =
#if   L==1	// ca
	"USERS.Attendance.es#lista-de-asistencia";
#elif L==2	// de
	"USERS.Attendance.en#attendance-list";
#elif L==3	// en
	"USERS.Attendance.en#attendance-list";
#elif L==4	// es
	"USERS.Attendance.es#lista-de-asistencia";
#elif L==5	// fr
	"USERS.Attendance.en#attendance-list";
#elif L==6	// gn
	"USERS.Attendance.es#lista-de-asistencia";
#elif L==7	// it
	"USERS.Attendance.en#attendance-list";
#elif L==8	// pl
	"USERS.Attendance.en#attendance-list";
#elif L==9	// pt
	"USERS.Attendance.en#attendance-list";
#elif L==10	// tr
	"USERS.Attendance.en#attendance-list";
#endif

const char *Hlp_USERS_SignUp =
#if   L==1	// ca
	"USERS.SignUp.es";
#elif L==2	// de
	"USERS.SignUp.en";
#elif L==3	// en
	"USERS.SignUp.en";
#elif L==4	// es
	"USERS.SignUp.es";
#elif L==5	// fr
	"USERS.SignUp.en";
#elif L==6	// gn
	"USERS.SignUp.es";
#elif L==7	// it
	"USERS.SignUp.en";
#elif L==8	// pl
	"USERS.SignUp.en";
#elif L==9	// pt
	"USERS.SignUp.en";
#elif L==10	// tr
	"USERS.SignUp.en";
#endif

const char *Hlp_USERS_SignUp_confirm_enrolment =
#if   L==1	// ca
	"USERS.SignUp.es#confirmar-inscripci%C3%B3n";
#elif L==2	// de
	"USERS.SignUp.en#confirm_enrolment";
#elif L==3	// en
	"USERS.SignUp.en#confirm_enrolment";
#elif L==4	// es
	"USERS.SignUp.es#confirmar-inscripci%C3%B3n";
#elif L==5	// fr
	"USERS.SignUp.en#confirm_enrolment";
#elif L==6	// gn
	"USERS.SignUp.es#confirmar-inscripci%C3%B3n";
#elif L==7	// it
	"USERS.SignUp.en#confirm_enrolment";
#elif L==8	// pl
	"USERS.SignUp.en#confirm_enrolment";
#elif L==9	// pt
	"USERS.SignUp.en#confirm_enrolment";
#elif L==10	// tr
	"USERS.SignUp.en#confirm_enrolment";
#endif

const char *Hlp_USERS_Requests =
#if   L==1	// ca
	"USERS.Requests.es";
#elif L==2	// de
	"USERS.Requests.en";
#elif L==3	// en
	"USERS.Requests.en";
#elif L==4	// es
	"USERS.Requests.es";
#elif L==5	// fr
	"USERS.Requests.en";
#elif L==6	// gn
	"USERS.Requests.es";
#elif L==7	// it
	"USERS.Requests.en";
#elif L==8	// pl
	"USERS.Requests.en";
#elif L==9	// pt
	"USERS.Requests.en";
#elif L==10	// tr
	"USERS.Requests.en";
#endif

const char *Hlp_USERS_Connected =
#if   L==1	// ca
	"USERS.Connected.es";
#elif L==2	// de
	"USERS.Connected.en";
#elif L==3	// en
	"USERS.Connected.en";
#elif L==4	// es
	"USERS.Connected.es";
#elif L==5	// fr
	"USERS.Connected.en";
#elif L==6	// gn
	"USERS.Connected.es";
#elif L==7	// it
	"USERS.Connected.en";
#elif L==8	// pl
	"USERS.Connected.en";
#elif L==9	// pt
	"USERS.Connected.en";
#elif L==10	// tr
	"USERS.Connected.en";
#endif

const char *Hlp_USERS_Connected_last_clicks =
#if   L==1	// ca
	"USERS.Connected.es#%C3%9Altimos-clics";
#elif L==2	// de
	"USERS.Connected.en#last-clicks";
#elif L==3	// en
	"USERS.Connected.en#last-clicks";
#elif L==4	// es
	"USERS.Connected.es#%C3%9Altimos-clics";
#elif L==5	// fr
	"USERS.Connected.en#last-clicks";
#elif L==6	// gn
	"USERS.Connected.es#%C3%9Altimos-clics";
#elif L==7	// it
	"USERS.Connected.en#last-clicks";
#elif L==8	// pl
	"USERS.Connected.en#last-clicks";
#elif L==9	// pt
	"USERS.Connected.en#last-clicks";
#elif L==10	// tr
	"USERS.Connected.en#last-clicks";
#endif

/***** COMMUNICATION tab *****/

const char *Hlp_COMMUNICATION_Announcements =
#if   L==1	// ca
	"COMMUNICATION.Announcements.es";
#elif L==2	// de
	"COMMUNICATION.Announcements.en";
#elif L==3	// en
	"COMMUNICATION.Announcements.en";
#elif L==4	// es
	"COMMUNICATION.Announcements.es";
#elif L==5	// fr
	"COMMUNICATION.Announcements.en";
#elif L==6	// gn
	"COMMUNICATION.Announcements.es";
#elif L==7	// it
	"COMMUNICATION.Announcements.en";
#elif L==8	// pl
	"COMMUNICATION.Announcements.en";
#elif L==9	// pt
	"COMMUNICATION.Announcements.en";
#elif L==10	// tr
	"COMMUNICATION.Announcements.en";
#endif

const char *Hlp_COMMUNICATION_Notices =
#if   L==1	// ca
	"COMMUNICATION.Notices.es";
#elif L==2	// de
	"COMMUNICATION.Notices.en";
#elif L==3	// en
	"COMMUNICATION.Notices.en";
#elif L==4	// es
	"COMMUNICATION.Notices.es";
#elif L==5	// fr
	"COMMUNICATION.Notices.en";
#elif L==6	// gn
	"COMMUNICATION.Notices.es";
#elif L==7	// it
	"COMMUNICATION.Notices.en";
#elif L==8	// pl
	"COMMUNICATION.Notices.en";
#elif L==9	// pt
	"COMMUNICATION.Notices.en";
#elif L==10	// tr
	"COMMUNICATION.Notices.en";
#endif

const char *Hlp_COMMUNICATION_Forums =
#if   L==1	// ca
	"COMMUNICATION.Forums.es";
#elif L==2	// de
	"COMMUNICATION.Forums.en";
#elif L==3	// en
	"COMMUNICATION.Forums.en";
#elif L==4	// es
	"COMMUNICATION.Forums.es";
#elif L==5	// fr
	"COMMUNICATION.Forums.en";
#elif L==6	// gn
	"COMMUNICATION.Forums.es";
#elif L==7	// it
	"COMMUNICATION.Forums.en";
#elif L==8	// pl
	"COMMUNICATION.Forums.en";
#elif L==9	// pt
	"COMMUNICATION.Forums.en";
#elif L==10	// tr
	"COMMUNICATION.Forums.en";
#endif

const char *Hlp_COMMUNICATION_Forums_threads =
#if   L==1	// ca
	"COMMUNICATION.Forums.es#discusiones";
#elif L==2	// de
	"COMMUNICATION.Forums.en#threads";
#elif L==3	// en
	"COMMUNICATION.Forums.en#threads";
#elif L==4	// es
	"COMMUNICATION.Forums.es#discusiones";
#elif L==5	// fr
	"COMMUNICATION.Forums.en#threads";
#elif L==6	// gn
	"COMMUNICATION.Forums.es#discusiones";
#elif L==7	// it
	"COMMUNICATION.Forums.en#threads";
#elif L==8	// pl
	"COMMUNICATION.Forums.en#threads";
#elif L==9	// pt
	"COMMUNICATION.Forums.en#threads";
#elif L==10	// tr
	"COMMUNICATION.Forums.en#threads";
#endif

const char *Hlp_COMMUNICATION_Forums_new_thread =
#if   L==1	// ca
	"COMMUNICATION.Forums.es#nueva-discusi%C3%B3n";
#elif L==2	// de
	"COMMUNICATION.Forums.en#new-thread";
#elif L==3	// en
	"COMMUNICATION.Forums.en#new-thread";
#elif L==4	// es
	"COMMUNICATION.Forums.es#nueva-discusi%C3%B3n";
#elif L==5	// fr
	"COMMUNICATION.Forums.en#new-thread";
#elif L==6	// gn
	"COMMUNICATION.Forums.es#nueva-discusi%C3%B3n";
#elif L==7	// it
	"COMMUNICATION.Forums.en#new-thread";
#elif L==8	// pl
	"COMMUNICATION.Forums.en#new-thread";
#elif L==9	// pt
	"COMMUNICATION.Forums.en#new-thread";
#elif L==10	// tr
	"COMMUNICATION.Forums.en#new-thread";
#endif

const char *Hlp_COMMUNICATION_Forums_posts =
#if   L==1	// ca
	"COMMUNICATION.Forums.es#comentarios";
#elif L==2	// de
	"COMMUNICATION.Forums.en#posts";
#elif L==3	// en
	"COMMUNICATION.Forums.en#posts";
#elif L==4	// es
	"COMMUNICATION.Forums.es#comentarios";
#elif L==5	// fr
	"COMMUNICATION.Forums.en#posts";
#elif L==6	// gn
	"COMMUNICATION.Forums.es#comentarios";
#elif L==7	// it
	"COMMUNICATION.Forums.en#posts";
#elif L==8	// pl
	"COMMUNICATION.Forums.en#posts";
#elif L==9	// pt
	"COMMUNICATION.Forums.en#posts";
#elif L==10	// tr
	"COMMUNICATION.Forums.en#posts";
#endif

const char *Hlp_COMMUNICATION_Forums_new_post =
#if   L==1	// ca
	"COMMUNICATION.Forums.es#nuevo-comentario";
#elif L==2	// de
	"COMMUNICATION.Forums.en#new-post";
#elif L==3	// en
	"COMMUNICATION.Forums.en#new-post";
#elif L==4	// es
	"COMMUNICATION.Forums.es#nuevo-comentario";
#elif L==5	// fr
	"COMMUNICATION.Forums.en#new-post";
#elif L==6	// gn
	"COMMUNICATION.Forums.es#nuevo-comentario";
#elif L==7	// it
	"COMMUNICATION.Forums.en#new-post";
#elif L==8	// pl
	"COMMUNICATION.Forums.en#new-post";
#elif L==9	// pt
	"COMMUNICATION.Forums.en#new-post";
#elif L==10	// tr
	"COMMUNICATION.Forums.en#new-post";
#endif

const char *Hlp_COMMUNICATION_Messages_write =
#if   L==1	// ca
	"COMMUNICATION.Write.es";
#elif L==2	// de
	"COMMUNICATION.Write.en";
#elif L==3	// en
	"COMMUNICATION.Write.en";
#elif L==4	// es
	"COMMUNICATION.Write.es";
#elif L==5	// fr
	"COMMUNICATION.Write.en";
#elif L==6	// gn
	"COMMUNICATION.Write.es";
#elif L==7	// it
	"COMMUNICATION.Write.en";
#elif L==8	// pl
	"COMMUNICATION.Write.en";
#elif L==9	// pt
	"COMMUNICATION.Write.en";
#elif L==10	// tr
	"COMMUNICATION.Write.en";
#endif

const char *Hlp_COMMUNICATION_Messages_received =
#if   L==1	// ca
	"COMMUNICATION.Received.es";
#elif L==2	// de
	"COMMUNICATION.Received.en";
#elif L==3	// en
	"COMMUNICATION.Received.en";
#elif L==4	// es
	"COMMUNICATION.Received.es";
#elif L==5	// fr
	"COMMUNICATION.Received.en";
#elif L==6	// gn
	"COMMUNICATION.Received.es";
#elif L==7	// it
	"COMMUNICATION.Received.en";
#elif L==8	// pl
	"COMMUNICATION.Received.en";
#elif L==9	// pt
	"COMMUNICATION.Received.en";
#elif L==10	// tr
	"COMMUNICATION.Received.en";
#endif

const char *Hlp_COMMUNICATION_Messages_sent =
#if   L==1	// ca
	"COMMUNICATION.Sent.es";
#elif L==2	// de
	"COMMUNICATION.Sent.en";
#elif L==3	// en
	"COMMUNICATION.Sent.en";
#elif L==4	// es
	"COMMUNICATION.Sent.es";
#elif L==5	// fr
	"COMMUNICATION.Sent.en";
#elif L==6	// gn
	"COMMUNICATION.Sent.es";
#elif L==7	// it
	"COMMUNICATION.Sent.en";
#elif L==8	// pl
	"COMMUNICATION.Sent.en";
#elif L==9	// pt
	"COMMUNICATION.Sent.en";
#elif L==10	// tr
	"COMMUNICATION.Sent.en";
#endif

const char *Hlp_COMMUNICATION_Email =
#if   L==1	// ca
	"COMMUNICATION.Email.es";
#elif L==2	// de
	"COMMUNICATION.Email.en";
#elif L==3	// en
	"COMMUNICATION.Email.en";
#elif L==4	// es
	"COMMUNICATION.Email.es";
#elif L==5	// fr
	"COMMUNICATION.Email.en";
#elif L==6	// gn
	"COMMUNICATION.Email.es";
#elif L==7	// it
	"COMMUNICATION.Email.en";
#elif L==8	// pl
	"COMMUNICATION.Email.en";
#elif L==9	// pt
	"COMMUNICATION.Email.en";
#elif L==10	// tr
	"COMMUNICATION.Email.en";
#endif

/***** ANALYTICS tab *****/

const char *Hlp_ANALYTICS_Figures =
#if   L==1	// ca
	"ANALYTICS.Figures.es";
#elif L==2	// de
	"ANALYTICS.Figures.en";
#elif L==3	// en
	"ANALYTICS.Figures.en";
#elif L==4	// es
	"ANALYTICS.Figures.es";
#elif L==5	// fr
	"ANALYTICS.Figures.en";
#elif L==6	// gn
	"ANALYTICS.Figures.es";
#elif L==7	// it
	"ANALYTICS.Figures.en";
#elif L==8	// pl
	"ANALYTICS.Figures.en";
#elif L==9	// pt
	"ANALYTICS.Figures.en";
#elif L==10	// tr
	"ANALYTICS.Figures.en";
#endif

const char *Hlp_ANALYTICS_Figures_users =
#if   L==1	// ca
	"ANALYTICS.Figures.es#usuarios";
#elif L==2	// de
	"ANALYTICS.Figures.en#users";
#elif L==3	// en
	"ANALYTICS.Figures.en#users";
#elif L==4	// es
	"ANALYTICS.Figures.es#usuarios";
#elif L==5	// fr
	"ANALYTICS.Figures.en#users";
#elif L==6	// gn
	"ANALYTICS.Figures.es#usuarios";
#elif L==7	// it
	"ANALYTICS.Figures.en#users";
#elif L==8	// pl
	"ANALYTICS.Figures.en#users";
#elif L==9	// pt
	"ANALYTICS.Figures.en#users";
#elif L==10	// tr
	"ANALYTICS.Figures.en#users";
#endif

const char *Hlp_ANALYTICS_Figures_ranking =
#if   L==1	// ca
	"ANALYTICS.Figures.es#ranking";
#elif L==2	// de
	"ANALYTICS.Figures.en#ranking";
#elif L==3	// en
	"ANALYTICS.Figures.en#ranking";
#elif L==4	// es
	"ANALYTICS.Figures.es#ranking";
#elif L==5	// fr
	"ANALYTICS.Figures.en#ranking";
#elif L==6	// gn
	"ANALYTICS.Figures.es#ranking";
#elif L==7	// it
	"ANALYTICS.Figures.en#ranking";
#elif L==8	// pl
	"ANALYTICS.Figures.en#ranking";
#elif L==9	// pt
	"ANALYTICS.Figures.en#ranking";
#elif L==10	// tr
	"ANALYTICS.Figures.en#ranking";
#endif

const char *Hlp_ANALYTICS_Figures_hierarchy =
#if   L==1	// ca
	"ANALYTICS.Figures.es#jerarqu%C3%ADa";
#elif L==2	// de
	"ANALYTICS.Figures.en#hierarchy";
#elif L==3	// en
	"ANALYTICS.Figures.en#hierarchy";
#elif L==4	// es
	"ANALYTICS.Figures.es#jerarqu%C3%ADa";
#elif L==5	// fr
	"ANALYTICS.Figures.en#hierarchy";
#elif L==6	// gn
	"ANALYTICS.Figures.es#jerarqu%C3%ADa";
#elif L==7	// it
	"ANALYTICS.Figures.en#hierarchy";
#elif L==8	// pl
	"ANALYTICS.Figures.en#hierarchy";
#elif L==9	// pt
	"ANALYTICS.Figures.en#hierarchy";
#elif L==10	// tr
	"ANALYTICS.Figures.en#hierarchy";
#endif

const char *Hlp_ANALYTICS_Figures_institutions =
#if   L==1	// ca
	"ANALYTICS.Figures.es#instituciones";
#elif L==2	// de
	"ANALYTICS.Figures.en#institutions";
#elif L==3	// en
	"ANALYTICS.Figures.en#institutions";
#elif L==4	// es
	"ANALYTICS.Figures.es#instituciones";
#elif L==5	// fr
	"ANALYTICS.Figures.en#institutions";
#elif L==6	// gn
	"ANALYTICS.Figures.es#instituciones";
#elif L==7	// it
	"ANALYTICS.Figures.en#institutions";
#elif L==8	// pl
	"ANALYTICS.Figures.en#institutions";
#elif L==9	// pt
	"ANALYTICS.Figures.en#institutions";
#elif L==10	// tr
	"ANALYTICS.Figures.en#institutions";
#endif

const char *Hlp_ANALYTICS_Figures_types_of_degree =
#if   L==1	// ca
	"ANALYTICS.Figures.es#tipos-de-titulaci%C3%B3n";
#elif L==2	// de
	"ANALYTICS.Figures.en#types-of-degree";
#elif L==3	// en
	"ANALYTICS.Figures.en#types-of-degree";
#elif L==4	// es
	"ANALYTICS.Figures.es#tipos-de-titulaci%C3%B3n";
#elif L==5	// fr
	"ANALYTICS.Figures.en#types-of-degree";
#elif L==6	// gn
	"ANALYTICS.Figures.es#tipos-de-titulaci%C3%B3n";
#elif L==7	// it
	"ANALYTICS.Figures.en#types-of-degree";
#elif L==8	// pl
	"ANALYTICS.Figures.en#types-of-degree";
#elif L==9	// pt
	"ANALYTICS.Figures.en#types-of-degree";
#elif L==10	// tr
	"ANALYTICS.Figures.en#types-of-degree";
#endif

const char *Hlp_ANALYTICS_Figures_folders_and_files =
#if   L==1	// ca
	"ANALYTICS.Figures.es#carpetas-y-archivos";
#elif L==2	// de
	"ANALYTICS.Figures.en#folders-and-files";
#elif L==3	// en
	"ANALYTICS.Figures.en#folders-and-files";
#elif L==4	// es
	"ANALYTICS.Figures.es#carpetas-y-archivos";
#elif L==5	// fr
	"ANALYTICS.Figures.en#folders-and-files";
#elif L==6	// gn
	"ANALYTICS.Figures.es#carpetas-y-archivos";
#elif L==7	// it
	"ANALYTICS.Figures.en#folders-and-files";
#elif L==8	// pl
	"ANALYTICS.Figures.en#folders-and-files";
#elif L==9	// pt
	"ANALYTICS.Figures.en#folders-and-files";
#elif L==10	// tr
	"ANALYTICS.Figures.en#folders-and-files";
#endif

const char *Hlp_ANALYTICS_Figures_open_educational_resources_oer =
#if   L==1	// ca
	"ANALYTICS.Figures.es#recursos-educativos-abiertos-oer";
#elif L==2	// de
	"ANALYTICS.Figures.en#open-educational-resources-oer";
#elif L==3	// en
	"ANALYTICS.Figures.en#open-educational-resources-oer";
#elif L==4	// es
	"ANALYTICS.Figures.es#recursos-educativos-abiertos-oer";
#elif L==5	// fr
	"ANALYTICS.Figures.en#open-educational-resources-oer";
#elif L==6	// gn
	"ANALYTICS.Figures.es#recursos-educativos-abiertos-oer";
#elif L==7	// it
	"ANALYTICS.Figures.en#open-educational-resources-oer";
#elif L==8	// pl
	"ANALYTICS.Figures.en#open-educational-resources-oer";
#elif L==9	// pt
	"ANALYTICS.Figures.en#open-educational-resources-oer";
#elif L==10	// tr
	"ANALYTICS.Figures.en#open-educational-resources-oer";
#endif

const char *Hlp_ANALYTICS_Figures_course_programs =
#if   L==1	// ca
	"ANALYTICS.Figures.es#programas-de-asignaturas";
#elif L==2	// de
	"ANALYTICS.Figures.en#course-programs";
#elif L==3	// en
	"ANALYTICS.Figures.en#course-programs";
#elif L==4	// es
	"ANALYTICS.Figures.es#programas-de-asignaturas";
#elif L==5	// fr
	"ANALYTICS.Figures.en#course-programs";
#elif L==6	// gn
	"ANALYTICS.Figures.es#programas-de-asignaturas";
#elif L==7	// it
	"ANALYTICS.Figures.en#course-programs";
#elif L==8	// pl
	"ANALYTICS.Figures.en#course-programs";
#elif L==9	// pt
	"ANALYTICS.Figures.en#course-programs";
#elif L==10	// tr
	"ANALYTICS.Figures.en#course-programs";
#endif

const char *Hlp_ANALYTICS_Figures_assignments =
#if   L==1	// ca
	"ANALYTICS.Figures.es#actividades";
#elif L==2	// de
	"ANALYTICS.Figures.en#assignments";
#elif L==3	// en
	"ANALYTICS.Figures.en#assignments";
#elif L==4	// es
	"ANALYTICS.Figures.es#actividades";
#elif L==5	// fr
	"ANALYTICS.Figures.en#assignments";
#elif L==6	// gn
	"ANALYTICS.Figures.es#actividades";
#elif L==7	// it
	"ANALYTICS.Figures.en#assignments";
#elif L==8	// pl
	"ANALYTICS.Figures.en#assignments";
#elif L==9	// pt
	"ANALYTICS.Figures.en#assignments";
#elif L==10	// tr
	"ANALYTICS.Figures.en#assignments";
#endif

const char *Hlp_ANALYTICS_Figures_projects =
#if   L==1	// ca
	"ANALYTICS.Figures.es#proyectos";
#elif L==2	// de
	"ANALYTICS.Figures.en#projects";
#elif L==3	// en
	"ANALYTICS.Figures.en#projects";
#elif L==4	// es
	"ANALYTICS.Figures.es#proyectos";
#elif L==5	// fr
	"ANALYTICS.Figures.en#projects";
#elif L==6	// gn
	"ANALYTICS.Figures.es#proyectos";
#elif L==7	// it
	"ANALYTICS.Figures.en#projects";
#elif L==8	// pl
	"ANALYTICS.Figures.en#projects";
#elif L==9	// pt
	"ANALYTICS.Figures.en#projects";
#elif L==10	// tr
	"ANALYTICS.Figures.en#projects";
#endif

const char *Hlp_ANALYTICS_Figures_tests =
#if   L==1	// ca
	"ANALYTICS.Figures.es#tests";
#elif L==2	// de
	"ANALYTICS.Figures.en#tests";
#elif L==3	// en
	"ANALYTICS.Figures.en#tests";
#elif L==4	// es
	"ANALYTICS.Figures.es#tests";
#elif L==5	// fr
	"ANALYTICS.Figures.en#tests";
#elif L==6	// gn
	"ANALYTICS.Figures.es#tests";
#elif L==7	// it
	"ANALYTICS.Figures.en#tests";
#elif L==8	// pl
	"ANALYTICS.Figures.en#tests";
#elif L==9	// pt
	"ANALYTICS.Figures.en#tests";
#elif L==10	// tr
	"ANALYTICS.Figures.en#tests";
#endif

const char *Hlp_ANALYTICS_Figures_exams =
#if   L==1	// ca
	"ANALYTICS.Figures.es#ex%C3%A1menes";
#elif L==2	// de
	"ANALYTICS.Figures.en#exams";
#elif L==3	// en
	"ANALYTICS.Figures.en#exams";
#elif L==4	// es
	"ANALYTICS.Figures.es#ex%C3%A1menes";
#elif L==5	// fr
	"ANALYTICS.Figures.en#exams";
#elif L==6	// gn
	"ANALYTICS.Figures.es#ex%C3%A1menes";
#elif L==7	// it
	"ANALYTICS.Figures.en#exams";
#elif L==8	// pl
	"ANALYTICS.Figures.en#exams";
#elif L==9	// pt
	"ANALYTICS.Figures.en#exams";
#elif L==10	// tr
	"ANALYTICS.Figures.en#exams";
#endif

const char *Hlp_ANALYTICS_Figures_games =
#if   L==1	// ca
	"ANALYTICS.Figures.es#juegos";
#elif L==2	// de
	"ANALYTICS.Figures.en#games";
#elif L==3	// en
	"ANALYTICS.Figures.en#games";
#elif L==4	// es
	"ANALYTICS.Figures.es#juegos";
#elif L==5	// fr
	"ANALYTICS.Figures.en#games";
#elif L==6	// gn
	"ANALYTICS.Figures.es#juegos";
#elif L==7	// it
	"ANALYTICS.Figures.en#games";
#elif L==8	// pl
	"ANALYTICS.Figures.en#games";
#elif L==9	// pt
	"ANALYTICS.Figures.en#games";
#elif L==10	// tr
	"ANALYTICS.Figures.en#games";
#endif

const char *Hlp_ANALYTICS_Figures_rubrics =
#if   L==1	// ca
	"ANALYTICS.Figures.es#rubricas";
#elif L==2	// de
	"ANALYTICS.Figures.en#rubrics";
#elif L==3	// en
	"ANALYTICS.Figures.en#rubrics";
#elif L==4	// es
	"ANALYTICS.Figures.es#rubricas";
#elif L==5	// fr
	"ANALYTICS.Figures.en#rubrics";
#elif L==6	// gn
	"ANALYTICS.Figures.es#rubricas";
#elif L==7	// it
	"ANALYTICS.Figures.en#rubrics";
#elif L==8	// pl
	"ANALYTICS.Figures.en#rubrics";
#elif L==9	// pt
	"ANALYTICS.Figures.en#rubrics";
#elif L==10	// tr
	"ANALYTICS.Figures.en#rubrics";
#endif

const char *Hlp_ANALYTICS_Figures_surveys =
#if   L==1	// ca
	"ANALYTICS.Figures.es#encuestas";
#elif L==2	// de
	"ANALYTICS.Figures.en#surveys";
#elif L==3	// en
	"ANALYTICS.Figures.en#surveys";
#elif L==4	// es
	"ANALYTICS.Figures.es#encuestas";
#elif L==5	// fr
	"ANALYTICS.Figures.en#surveys";
#elif L==6	// gn
	"ANALYTICS.Figures.es#encuestas";
#elif L==7	// it
	"ANALYTICS.Figures.en#surveys";
#elif L==8	// pl
	"ANALYTICS.Figures.en#surveys";
#elif L==9	// pt
	"ANALYTICS.Figures.en#surveys";
#elif L==10	// tr
	"ANALYTICS.Figures.en#surveys";
#endif

const char *Hlp_ANALYTICS_Figures_timeline =
#if   L==1	// ca
	"ANALYTICS.Figures.es#timeline";
#elif L==2	// de
	"ANALYTICS.Figures.en#timeline";
#elif L==3	// en
	"ANALYTICS.Figures.en#timeline";
#elif L==4	// es
	"ANALYTICS.Figures.es#timeline";
#elif L==5	// fr
	"ANALYTICS.Figures.en#timeline";
#elif L==6	// gn
	"ANALYTICS.Figures.es#timeline";
#elif L==7	// it
	"ANALYTICS.Figures.en#timeline";
#elif L==8	// pl
	"ANALYTICS.Figures.en#timeline";
#elif L==9	// pt
	"ANALYTICS.Figures.en#timeline";
#elif L==10	// tr
	"ANALYTICS.Figures.en#timeline";
#endif

const char *Hlp_ANALYTICS_Figures_followed_followers =
#if   L==1	// ca
	"ANALYTICS.Figures.es#seguidos--seguidores";
#elif L==2	// de
	"ANALYTICS.Figures.en#followed--followers";
#elif L==3	// en
	"ANALYTICS.Figures.en#followed--followers";
#elif L==4	// es
	"ANALYTICS.Figures.es#seguidos--seguidores";
#elif L==5	// fr
	"ANALYTICS.Figures.en#followed--followers";
#elif L==6	// gn
	"ANALYTICS.Figures.es#seguidos--seguidores";
#elif L==7	// it
	"ANALYTICS.Figures.en#followed--followers";
#elif L==8	// pl
	"ANALYTICS.Figures.en#followed--followers";
#elif L==9	// pt
	"ANALYTICS.Figures.en#followed--followers";
#elif L==10	// tr
	"ANALYTICS.Figures.en#followed--followers";
#endif

const char *Hlp_ANALYTICS_Figures_forums =
#if   L==1	// ca
	"ANALYTICS.Figures.es#foros";
#elif L==2	// de
	"ANALYTICS.Figures.en#forums";
#elif L==3	// en
	"ANALYTICS.Figures.en#forums";
#elif L==4	// es
	"ANALYTICS.Figures.es#foros";
#elif L==5	// fr
	"ANALYTICS.Figures.en#forums";
#elif L==6	// gn
	"ANALYTICS.Figures.es#foros";
#elif L==7	// it
	"ANALYTICS.Figures.en#forums";
#elif L==8	// pl
	"ANALYTICS.Figures.en#forums";
#elif L==9	// pt
	"ANALYTICS.Figures.en#forums";
#elif L==10	// tr
	"ANALYTICS.Figures.en#forums";
#endif

const char *Hlp_ANALYTICS_Figures_notifications =
#if   L==1	// ca
	"ANALYTICS.Figures.es#notificaciones";
#elif L==2	// de
	"ANALYTICS.Figures.en#notifications";
#elif L==3	// en
	"ANALYTICS.Figures.en#notifications";
#elif L==4	// es
	"ANALYTICS.Figures.es#notificaciones";
#elif L==5	// fr
	"ANALYTICS.Figures.en#notifications";
#elif L==6	// gn
	"ANALYTICS.Figures.es#notificaciones";
#elif L==7	// it
	"ANALYTICS.Figures.en#notifications";
#elif L==8	// pl
	"ANALYTICS.Figures.en#notifications";
#elif L==9	// pt
	"ANALYTICS.Figures.en#notifications";
#elif L==10	// tr
	"ANALYTICS.Figures.en#notifications";
#endif

const char *Hlp_ANALYTICS_Figures_notices =
#if   L==1	// ca
	"ANALYTICS.Figures.es#avisos";
#elif L==2	// de
	"ANALYTICS.Figures.en#notices";
#elif L==3	// en
	"ANALYTICS.Figures.en#notices";
#elif L==4	// es
	"ANALYTICS.Figures.es#avisos";
#elif L==5	// fr
	"ANALYTICS.Figures.en#notices";
#elif L==6	// gn
	"ANALYTICS.Figures.es#avisos";
#elif L==7	// it
	"ANALYTICS.Figures.en#notices";
#elif L==8	// pl
	"ANALYTICS.Figures.en#notices";
#elif L==9	// pt
	"ANALYTICS.Figures.en#notices";
#elif L==10	// tr
	"ANALYTICS.Figures.en#notices";
#endif

const char *Hlp_ANALYTICS_Figures_messages =
#if   L==1	// ca
	"ANALYTICS.Figures.es#mensajes";
#elif L==2	// de
	"ANALYTICS.Figures.en#messages";
#elif L==3	// en
	"ANALYTICS.Figures.en#messages";
#elif L==4	// es
	"ANALYTICS.Figures.es#mensajes";
#elif L==5	// fr
	"ANALYTICS.Figures.en#messages";
#elif L==6	// gn
	"ANALYTICS.Figures.es#mensajes";
#elif L==7	// it
	"ANALYTICS.Figures.en#messages";
#elif L==8	// pl
	"ANALYTICS.Figures.en#messages";
#elif L==9	// pt
	"ANALYTICS.Figures.en#messages";
#elif L==10	// tr
	"ANALYTICS.Figures.en#messages";
#endif

const char *Hlp_ANALYTICS_Figures_agendas =
#if   L==1	// ca
	"ANALYTICS.Figures.es#agendas";
#elif L==2	// de
	"ANALYTICS.Figures.en#agendas";
#elif L==3	// en
	"ANALYTICS.Figures.en#agendas";
#elif L==4	// es
	"ANALYTICS.Figures.es#agendas";
#elif L==5	// fr
	"ANALYTICS.Figures.en#agendas";
#elif L==6	// gn
	"ANALYTICS.Figures.es#agendas";
#elif L==7	// it
	"ANALYTICS.Figures.en#agendas";
#elif L==8	// pl
	"ANALYTICS.Figures.en#agendas";
#elif L==9	// pt
	"ANALYTICS.Figures.en#agendas";
#elif L==10	// tr
	"ANALYTICS.Figures.en#agendas";
#endif

const char *Hlp_ANALYTICS_Figures_webs_social_networks =
#if   L==1	// ca
	"ANALYTICS.Figures.es#webs--redes-sociales";
#elif L==2	// de
	"ANALYTICS.Figures.en#webs--social-networks";
#elif L==3	// en
	"ANALYTICS.Figures.en#webs--social-networks";
#elif L==4	// es
	"ANALYTICS.Figures.es#webs--redes-sociales";
#elif L==5	// fr
	"ANALYTICS.Figures.en#webs--social-networks";
#elif L==6	// gn
	"ANALYTICS.Figures.es#webs--redes-sociales";
#elif L==7	// it
	"ANALYTICS.Figures.en#webs--social-networks";
#elif L==8	// pl
	"ANALYTICS.Figures.en#webs--social-networks";
#elif L==9	// pt
	"ANALYTICS.Figures.en#webs--social-networks";
#elif L==10	// tr
	"ANALYTICS.Figures.en#webs--social-networks";
#endif

const char *Hlp_ANALYTICS_Figures_language =
#if   L==1	// ca
	"ANALYTICS.Figures.es#idioma";
#elif L==2	// de
	"ANALYTICS.Figures.en#language";
#elif L==3	// en
	"ANALYTICS.Figures.en#language";
#elif L==4	// es
	"ANALYTICS.Figures.es#idioma";
#elif L==5	// fr
	"ANALYTICS.Figures.en#language";
#elif L==6	// gn
	"ANALYTICS.Figures.es#idioma";
#elif L==7	// it
	"ANALYTICS.Figures.en#language";
#elif L==8	// pl
	"ANALYTICS.Figures.en#language";
#elif L==9	// pt
	"ANALYTICS.Figures.en#language";
#elif L==10	// tr
	"ANALYTICS.Figures.en#language";
#endif

const char *Hlp_ANALYTICS_Figures_calendar =
#if   L==1	// ca
	"ANALYTICS.Figures.es#calendario";
#elif L==2	// de
	"ANALYTICS.Figures.en#calendar";
#elif L==3	// en
	"ANALYTICS.Figures.en#calendar";
#elif L==4	// es
	"ANALYTICS.Figures.es#calendario";
#elif L==5	// fr
	"ANALYTICS.Figures.en#calendar";
#elif L==6	// gn
	"ANALYTICS.Figures.es#calendario";
#elif L==7	// it
	"ANALYTICS.Figures.en#calendar";
#elif L==8	// pl
	"ANALYTICS.Figures.en#calendar";
#elif L==9	// pt
	"ANALYTICS.Figures.en#calendar";
#elif L==10	// tr
	"ANALYTICS.Figures.en#calendar";
#endif

const char *Hlp_ANALYTICS_Figures_dates =
#if   L==1	// ca
	"ANALYTICS.Figures.es#fechas";
#elif L==2	// de
	"ANALYTICS.Figures.en#dates";
#elif L==3	// en
	"ANALYTICS.Figures.en#dates";
#elif L==4	// es
	"ANALYTICS.Figures.es#fechas";
#elif L==5	// fr
	"ANALYTICS.Figures.en#dates";
#elif L==6	// gn
	"ANALYTICS.Figures.es#fechas";
#elif L==7	// it
	"ANALYTICS.Figures.en#dates";
#elif L==8	// pl
	"ANALYTICS.Figures.en#dates";
#elif L==9	// pt
	"ANALYTICS.Figures.en#dates";
#elif L==10	// tr
	"ANALYTICS.Figures.en#dates";
#endif

const char *Hlp_ANALYTICS_Figures_icons =
#if   L==1	// ca
	"ANALYTICS.Figures.es#iconos";
#elif L==2	// de
	"ANALYTICS.Figures.en#icons";
#elif L==3	// en
	"ANALYTICS.Figures.en#icons";
#elif L==4	// es
	"ANALYTICS.Figures.es#iconos";
#elif L==5	// fr
	"ANALYTICS.Figures.en#icons";
#elif L==6	// gn
	"ANALYTICS.Figures.es#iconos";
#elif L==7	// it
	"ANALYTICS.Figures.en#icons";
#elif L==8	// pl
	"ANALYTICS.Figures.en#icons";
#elif L==9	// pt
	"ANALYTICS.Figures.en#icons";
#elif L==10	// tr
	"ANALYTICS.Figures.en#icons";
#endif

const char *Hlp_ANALYTICS_Figures_menu =
#if   L==1	// ca
	"ANALYTICS.Figures.es#men%C3%BA";
#elif L==2	// de
	"ANALYTICS.Figures.en#menu";
#elif L==3	// en
	"ANALYTICS.Figures.en#menu";
#elif L==4	// es
	"ANALYTICS.Figures.es#men%C3%BA";
#elif L==5	// fr
	"ANALYTICS.Figures.en#menu";
#elif L==6	// gn
	"ANALYTICS.Figures.es#men%C3%BA";
#elif L==7	// it
	"ANALYTICS.Figures.en#menu";
#elif L==8	// pl
	"ANALYTICS.Figures.en#menu";
#elif L==9	// pt
	"ANALYTICS.Figures.en#menu";
#elif L==10	// tr
	"ANALYTICS.Figures.en#menu";
#endif

const char *Hlp_ANALYTICS_Figures_theme =
#if   L==1	// ca
	"ANALYTICS.Figures.es#tema";
#elif L==2	// de
	"ANALYTICS.Figures.en#theme";
#elif L==3	// en
	"ANALYTICS.Figures.en#theme";
#elif L==4	// es
	"ANALYTICS.Figures.es#tema";
#elif L==5	// fr
	"ANALYTICS.Figures.en#theme";
#elif L==6	// gn
	"ANALYTICS.Figures.es#tema";
#elif L==7	// it
	"ANALYTICS.Figures.en#theme";
#elif L==8	// pl
	"ANALYTICS.Figures.en#theme";
#elif L==9	// pt
	"ANALYTICS.Figures.en#theme";
#elif L==10	// tr
	"ANALYTICS.Figures.en#theme";
#endif

const char *Hlp_ANALYTICS_Figures_columns =
#if   L==1	// ca
	"ANALYTICS.Figures.es#columnas";
#elif L==2	// de
	"ANALYTICS.Figures.en#columns";
#elif L==3	// en
	"ANALYTICS.Figures.en#columns";
#elif L==4	// es
	"ANALYTICS.Figures.es#columnas";
#elif L==5	// fr
	"ANALYTICS.Figures.en#columns";
#elif L==6	// gn
	"ANALYTICS.Figures.es#columnas";
#elif L==7	// it
	"ANALYTICS.Figures.en#columns";
#elif L==8	// pl
	"ANALYTICS.Figures.en#columns";
#elif L==9	// pt
	"ANALYTICS.Figures.en#columns";
#elif L==10	// tr
	"ANALYTICS.Figures.en#columns";
#endif

const char *Hlp_ANALYTICS_Figures_user_photos =
#if   L==1	// ca
	"ANALYTICS.Figures.es#fotos-de-usuarios";
#elif L==2	// de
	"ANALYTICS.Figures.en#user-photos";
#elif L==3	// en
	"ANALYTICS.Figures.en#user-photos";
#elif L==4	// es
	"ANALYTICS.Figures.es#fotos-de-usuarios";
#elif L==5	// fr
	"ANALYTICS.Figures.en#user-photos";
#elif L==6	// gn
	"ANALYTICS.Figures.es#fotos-de-usuarios";
#elif L==7	// it
	"ANALYTICS.Figures.en#user-photos";
#elif L==8	// pl
	"ANALYTICS.Figures.en#user-photos";
#elif L==9	// pt
	"ANALYTICS.Figures.en#user-photos";
#elif L==10	// tr
	"ANALYTICS.Figures.en#user-photos";
#endif

const char *Hlp_ANALYTICS_Figures_privacy =
#if   L==1	// ca
	"ANALYTICS.Figures.es#privacidad";
#elif L==2	// de
	"ANALYTICS.Figures.en#privacy";
#elif L==3	// en
	"ANALYTICS.Figures.en#privacy";
#elif L==4	// es
	"ANALYTICS.Figures.es#privacidad";
#elif L==5	// fr
	"ANALYTICS.Figures.en#privacy";
#elif L==6	// gn
	"ANALYTICS.Figures.es#privacidad";
#elif L==7	// it
	"ANALYTICS.Figures.en#privacy";
#elif L==8	// pl
	"ANALYTICS.Figures.en#privacy";
#elif L==9	// pt
	"ANALYTICS.Figures.en#privacy";
#elif L==10	// tr
	"ANALYTICS.Figures.en#privacy";
#endif

const char *Hlp_ANALYTICS_Figures_cookies =
#if   L==1	// ca
	"ANALYTICS.Figures.es#cookies";
#elif L==2	// de
	"ANALYTICS.Figures.en#cookies";
#elif L==3	// en
	"ANALYTICS.Figures.en#cookies";
#elif L==4	// es
	"ANALYTICS.Figures.es#cookies";
#elif L==5	// fr
	"ANALYTICS.Figures.en#cookies";
#elif L==6	// gn
	"ANALYTICS.Figures.es#cookies";
#elif L==7	// it
	"ANALYTICS.Figures.en#cookies";
#elif L==8	// pl
	"ANALYTICS.Figures.en#cookies";
#elif L==9	// pt
	"ANALYTICS.Figures.en#privacy";
#elif L==10	// tr
	"ANALYTICS.Figures.en#privacy";
#endif

const char *Hlp_ANALYTICS_Degrees =
#if   L==1	// ca
	"ANALYTICS.Degrees.es";
#elif L==2	// de
	"ANALYTICS.Degrees.en";
#elif L==3	// en
	"ANALYTICS.Degrees.en";
#elif L==4	// es
	"ANALYTICS.Degrees.es";
#elif L==5	// fr
	"ANALYTICS.Degrees.en";
#elif L==6	// gn
	"ANALYTICS.Degrees.es";
#elif L==7	// it
	"ANALYTICS.Degrees.en";
#elif L==8	// pl
	"ANALYTICS.Degrees.en";
#elif L==9	// pt
	"ANALYTICS.Degrees.en";
#elif L==10	// tr
	"ANALYTICS.Degrees.en";
#endif

const char *Hlp_ANALYTICS_Indicators =
#if   L==1	// ca
	"ANALYTICS.Indicators.es";
#elif L==2	// de
	"ANALYTICS.Indicators.en";
#elif L==3	// en
	"ANALYTICS.Indicators.en";
#elif L==4	// es
	"ANALYTICS.Indicators.es";
#elif L==5	// fr
	"ANALYTICS.Indicators.en";
#elif L==6	// gn
	"ANALYTICS.Indicators.es";
#elif L==7	// it
	"ANALYTICS.Indicators.en";
#elif L==8	// pl
	"ANALYTICS.Indicators.en";
#elif L==9	// pt
	"ANALYTICS.Indicators.en";
#elif L==10	// tr
	"ANALYTICS.Indicators.en";
#endif

const char *Hlp_ANALYTICS_Visits_global_visits =
#if   L==1	// ca
	"ANALYTICS.Visits.es#accesos-globales";
#elif L==2	// de
	"ANALYTICS.Visits.en#global-visits";
#elif L==3	// en
	"ANALYTICS.Visits.en#global-visits";
#elif L==4	// es
	"ANALYTICS.Visits.es#accesos-globales";
#elif L==5	// fr
	"ANALYTICS.Visits.en#global-visits";
#elif L==6	// gn
	"ANALYTICS.Visits.es#accesos-globales";
#elif L==7	// it
	"ANALYTICS.Visits.en#global-visits";
#elif L==8	// pl
	"ANALYTICS.Visits.en#global-visits";
#elif L==9	// pt
	"ANALYTICS.Visits.en#global-visits";
#elif L==10	// tr
	"ANALYTICS.Visits.en#global-visits";
#endif

const char *Hlp_ANALYTICS_Visits_visits_to_course =
#if   L==1	// ca
	"ANALYTICS.Visits.es#accesos-a-la-asignatura";
#elif L==2	// de
	"ANALYTICS.Visits.en#visits-to-course";
#elif L==3	// en
	"ANALYTICS.Visits.en#visits-to-course";
#elif L==4	// es
	"ANALYTICS.Visits.es#accesos-a-la-asignatura";
#elif L==5	// fr
	"ANALYTICS.Visits.en#visits-to-course";
#elif L==6	// gn
	"ANALYTICS.Visits.es#accesos-a-la-asignatura";
#elif L==7	// it
	"ANALYTICS.Visits.en#visits-to-course";
#elif L==8	// pl
	"ANALYTICS.Visits.en#visits-to-course";
#elif L==9	// pt
	"ANALYTICS.Visits.en#visits-to-course";
#elif L==10	// tr
	"ANALYTICS.Visits.en#visits-to-course";
#endif

const char *Hlp_ANALYTICS_Surveys =
#if   L==1	// ca
	"ANALYTICS.Surveys.es";
#elif L==2	// de
	"ANALYTICS.Surveys.en";
#elif L==3	// en
	"ANALYTICS.Surveys.en";
#elif L==4	// es
	"ANALYTICS.Surveys.es";
#elif L==5	// fr
	"ANALYTICS.Surveys.en";
#elif L==6	// gn
	"ANALYTICS.Surveys.es";
#elif L==7	// it
	"ANALYTICS.Surveys.en";
#elif L==8	// pl
	"ANALYTICS.Surveys.en";
#elif L==9	// pt
	"ANALYTICS.Surveys.en";
#elif L==10	// tr
	"ANALYTICS.Surveys.en";
#endif

const char *Hlp_ANALYTICS_Surveys_edit_survey =
#if   L==1	// ca
	"ANALYTICS.Surveys.es#editar-encuesta";
#elif L==2	// de
	"ANALYTICS.Surveys.en#edit-survey";
#elif L==3	// en
	"ANALYTICS.Surveys.en#edit-survey";
#elif L==4	// es
	"ANALYTICS.Surveys.es#editar-encuesta";
#elif L==5	// fr
	"ANALYTICS.Surveys.en#edit-survey";
#elif L==6	// gn
	"ANALYTICS.Surveys.es#editar-encuesta";
#elif L==7	// it
	"ANALYTICS.Surveys.en#edit-survey";
#elif L==8	// pl
	"ANALYTICS.Surveys.en#edit-survey";
#elif L==9	// pt
	"ANALYTICS.Surveys.en#edit-survey";
#elif L==10	// tr
	"ANALYTICS.Surveys.en#edit-survey";
#endif

const char *Hlp_ANALYTICS_Surveys_questions =
#if   L==1	// ca
	"ANALYTICS.Surveys.es#preguntas";
#elif L==2	// de
	"ANALYTICS.Surveys.en#questions";
#elif L==3	// en
	"ANALYTICS.Surveys.en#questions";
#elif L==4	// es
	"ANALYTICS.Surveys.es#preguntas";
#elif L==5	// fr
	"ANALYTICS.Surveys.en#questions";
#elif L==6	// gn
	"ANALYTICS.Surveys.es#preguntas";
#elif L==7	// it
	"ANALYTICS.Surveys.en#questions";
#elif L==8	// pl
	"ANALYTICS.Surveys.en#questions";
#elif L==9	// pt
	"ANALYTICS.Surveys.en#questions";
#elif L==10	// tr
	"ANALYTICS.Surveys.en#questions";
#endif

const char *Hlp_ANALYTICS_Report =
#if   L==1	// ca
	"ANALYTICS.Report.es";
#elif L==2	// de
	"ANALYTICS.Report.en";
#elif L==3	// en
	"ANALYTICS.Report.en";
#elif L==4	// es
	"ANALYTICS.Report.es";
#elif L==5	// fr
	"ANALYTICS.Report.en";
#elif L==6	// gn
	"ANALYTICS.Report.es";
#elif L==7	// it
	"ANALYTICS.Report.en";
#elif L==8	// pl
	"ANALYTICS.Report.en";
#elif L==9	// pt
	"ANALYTICS.Report.en";
#elif L==10	// tr
	"ANALYTICS.Report.en";
#endif

const char *Hlp_ANALYTICS_Frequent =
#if   L==1	// ca
	"ANALYTICS.Frequent.es";
#elif L==2	// de
	"ANALYTICS.Frequent.en";
#elif L==3	// en
	"ANALYTICS.Frequent.en";
#elif L==4	// es
	"ANALYTICS.Frequent.es";
#elif L==5	// fr
	"ANALYTICS.Frequent.en";
#elif L==6	// gn
	"ANALYTICS.Frequent.es";
#elif L==7	// it
	"ANALYTICS.Frequent.en";
#elif L==8	// pl
	"ANALYTICS.Frequent.en";
#elif L==9	// pt
	"ANALYTICS.Frequent.en";
#elif L==10	// tr
	"ANALYTICS.Frequent.en";
#endif

/***** PROFILE tab *****/

const char *Hlp_PROFILE_LogIn =
#if   L==1	// ca
	"PROFILE.LogIn.es";
#elif L==2	// de
	"PROFILE.LogIn.en";
#elif L==3	// en
	"PROFILE.LogIn.en";
#elif L==4	// es
	"PROFILE.LogIn.es";
#elif L==5	// fr
	"PROFILE.LogIn.en";
#elif L==6	// gn
	"PROFILE.LogIn.es";
#elif L==7	// it
	"PROFILE.LogIn.en";
#elif L==8	// pl
	"PROFILE.LogIn.en";
#elif L==9	// pt
	"PROFILE.LogIn.en";
#elif L==10	// tr
	"PROFILE.LogIn.en";
#endif

const char *Hlp_PROFILE_SignUp =
#if   L==1	// ca
	"PROFILE.SignUp.es";
#elif L==2	// de
	"PROFILE.SignUp.en";
#elif L==3	// en
	"PROFILE.SignUp.en";
#elif L==4	// es
	"PROFILE.SignUp.es";
#elif L==5	// fr
	"PROFILE.SignUp.en";
#elif L==6	// gn
	"PROFILE.SignUp.es";
#elif L==7	// it
	"PROFILE.SignUp.en";
#elif L==8	// pl
	"PROFILE.SignUp.en";
#elif L==9	// pt
	"PROFILE.SignUp.en";
#elif L==10	// tr
	"PROFILE.SignUp.en";
#endif

const char *Hlp_PROFILE_Account =
#if   L==1	// ca
	"PROFILE.Account.es";
#elif L==2	// de
	"PROFILE.Account.en";
#elif L==3	// en
	"PROFILE.Account.en";
#elif L==4	// es
	"PROFILE.Account.es";
#elif L==5	// fr
	"PROFILE.Account.en";
#elif L==6	// gn
	"PROFILE.Account.es";
#elif L==7	// it
	"PROFILE.Account.en";
#elif L==8	// pl
	"PROFILE.Account.en";
#elif L==9	// pt
	"PROFILE.Account.en";
#elif L==10	// tr
	"PROFILE.Account.en";
#endif
/*
const char *Hlp_PROFILE_Account_email =
#if   L==1	// ca
	"PROFILE.Account.es#correo";
#elif L==2	// de
	"PROFILE.Account.en#email";
#elif L==3	// en
	"PROFILE.Account.en#email";
#elif L==4	// es
	"PROFILE.Account.es#correo";
#elif L==5	// fr
	"PROFILE.Account.en#email";
#elif L==6	// gn
	"PROFILE.Account.es#correo";
#elif L==7	// it
	"PROFILE.Account.en#email";
#elif L==8	// pl
	"PROFILE.Account.en#email";
#elif L==9	// pt
	"PROFILE.Account.en#email";
#elif L==10	// tr
	"PROFILE.Account.en#email";
#endif
*/
const char *Hlp_PROFILE_Session_role =
#if   L==1	// ca
	"PROFILE.Session.es#rol";
#elif L==2	// de
	"PROFILE.Session.en#role";
#elif L==3	// en
	"PROFILE.Session.en#role";
#elif L==4	// es
	"PROFILE.Session.es#rol";
#elif L==5	// fr
	"PROFILE.Session.en#role";
#elif L==6	// gn
	"PROFILE.Session.es#rol";
#elif L==7	// it
	"PROFILE.Session.en#role";
#elif L==8	// pl
	"PROFILE.Session.en#role";
#elif L==9	// pt
	"PROFILE.Session.en#role";
#elif L==10	// tr
	"PROFILE.Session.en#role";
#endif

const char *Hlp_PROFILE_Password =
#if   L==1	// ca
	"PROFILE.Password.es";
#elif L==2	// de
	"PROFILE.Password.en";
#elif L==3	// en
	"PROFILE.Password.en";
#elif L==4	// es
	"PROFILE.Password.es";
#elif L==5	// fr
	"PROFILE.Password.en";
#elif L==6	// gn
	"PROFILE.Password.es";
#elif L==7	// it
	"PROFILE.Password.en";
#elif L==8	// pl
	"PROFILE.Password.en";
#elif L==9	// pt
	"PROFILE.Password.en";
#elif L==10	// tr
	"PROFILE.Password.en";
#endif

const char *Hlp_PROFILE_Courses =
#if   L==1	// ca
	"PROFILE.Courses.es";
#elif L==2	// de
	"PROFILE.Courses.en";
#elif L==3	// en
	"PROFILE.Courses.en";
#elif L==4	// es
	"PROFILE.Courses.es";
#elif L==5	// fr
	"PROFILE.Courses.en";
#elif L==6	// gn
	"PROFILE.Courses.es";
#elif L==7	// it
	"PROFILE.Courses.en";
#elif L==8	// pl
	"PROFILE.Courses.en";
#elif L==9	// pt
	"PROFILE.Courses.en";
#elif L==10	// tr
	"PROFILE.Courses.en";
#endif

const char *Hlp_PROFILE_Timetable =
#if   L==1	// ca
	"PROFILE.Timetable.es";
#elif L==2	// de
	"PROFILE.Timetable.en";
#elif L==3	// en
	"PROFILE.Timetable.en";
#elif L==4	// es
	"PROFILE.Timetable.es";
#elif L==5	// fr
	"PROFILE.Timetable.en";
#elif L==6	// gn
	"PROFILE.Timetable.es";
#elif L==7	// it
	"PROFILE.Timetable.en";
#elif L==8	// pl
	"PROFILE.Timetable.en";
#elif L==9	// pt
	"PROFILE.Timetable.en";
#elif L==10	// tr
	"PROFILE.Timetable.en";
#endif

const char *Hlp_PROFILE_Agenda =
#if   L==1	// ca
	"PROFILE.Agenda.es";
#elif L==2	// de
	"PROFILE.Agenda.en";
#elif L==3	// en
	"PROFILE.Agenda.en";
#elif L==4	// es
	"PROFILE.Agenda.es";
#elif L==5	// fr
	"PROFILE.Agenda.en";
#elif L==6	// gn
	"PROFILE.Agenda.es";
#elif L==7	// it
	"PROFILE.Agenda.en";
#elif L==8	// pl
	"PROFILE.Agenda.en";
#elif L==9	// pt
	"PROFILE.Agenda.en";
#elif L==10	// tr
	"PROFILE.Agenda.en";
#endif

const char *Hlp_PROFILE_Agenda_edit_event =
#if   L==1	// ca
	"PROFILE.Agenda.es#editar-evento";
#elif L==2	// de
	"PROFILE.Agenda.en#edit-event";
#elif L==3	// en
	"PROFILE.Agenda.en#edit-event";
#elif L==4	// es
	"PROFILE.Agenda.es#editar-evento";
#elif L==5	// fr
	"PROFILE.Agenda.en#edit-event";
#elif L==6	// gn
	"PROFILE.Agenda.es#editar-evento";
#elif L==7	// it
	"PROFILE.Agenda.en#edit-event";
#elif L==8	// pl
	"PROFILE.Agenda.en#edit-event";
#elif L==9	// pt
	"PROFILE.Agenda.en#edit-event";
#elif L==10	// tr
	"PROFILE.Agenda.en#edit-event";
#endif

const char *Hlp_PROFILE_Agenda_public_agenda =
#if   L==1	// ca
	"PROFILE.Agenda.es#agenda-p%C3%BAblica";
#elif L==2	// de
	"PROFILE.Agenda.en#public-agenda";
#elif L==3	// en
	"PROFILE.Agenda.en#public-agenda";
#elif L==4	// es
	"PROFILE.Agenda.es#agenda-p%C3%BAblica";
#elif L==5	// fr
	"PROFILE.Agenda.en#public-agenda";
#elif L==6	// gn
	"PROFILE.Agenda.es#agenda-p%C3%BAblica";
#elif L==7	// it
	"PROFILE.Agenda.en#public-agenda";
#elif L==8	// pl
	"PROFILE.Agenda.en#public-agenda";
#elif L==9	// pt
	"PROFILE.Agenda.en#public-agenda";
#elif L==10	// tr
	"PROFILE.Agenda.en#public-agenda";
#endif

const char *Hlp_PROFILE_Record =
#if   L==1	// ca
	"PROFILE.Record.es";
#elif L==2	// de
	"PROFILE.Record.en";
#elif L==3	// en
	"PROFILE.Record.en";
#elif L==4	// es
	"PROFILE.Record.es";
#elif L==5	// fr
	"PROFILE.Record.en";
#elif L==6	// gn
	"PROFILE.Record.es";
#elif L==7	// it
	"PROFILE.Record.en";
#elif L==8	// pl
	"PROFILE.Record.en";
#elif L==9	// pt
	"PROFILE.Record.en";
#elif L==10	// tr
	"PROFILE.Record.en";
#endif

const char *Hlp_PROFILE_Institution =
#if   L==1	// ca
	"PROFILE.Institution.es";
#elif L==2	// de
	"PROFILE.Institution.en";
#elif L==3	// en
	"PROFILE.Institution.en";
#elif L==4	// es
	"PROFILE.Institution.es";
#elif L==5	// fr
	"PROFILE.Institution.en";
#elif L==6	// gn
	"PROFILE.Institution.es";
#elif L==7	// it
	"PROFILE.Institution.en";
#elif L==8	// pl
	"PROFILE.Institution.en";
#elif L==9	// pt
	"PROFILE.Institution.en";
#elif L==10	// tr
	"PROFILE.Institution.en";
#endif

const char *Hlp_PROFILE_Webs =
#if   L==1	// ca
	"PROFILE.Webs.es";
#elif L==2	// de
	"PROFILE.Webs.en";
#elif L==3	// en
	"PROFILE.Webs.en";
#elif L==4	// es
	"PROFILE.Webs.es";
#elif L==5	// fr
	"PROFILE.Webs.en";
#elif L==6	// gn
	"PROFILE.Webs.es";
#elif L==7	// it
	"PROFILE.Webs.en";
#elif L==8	// pl
	"PROFILE.Webs.en";
#elif L==9	// pt
	"PROFILE.Webs.en";
#elif L==10	// tr
	"PROFILE.Webs.en";
#endif

const char *Hlp_PROFILE_Photo =
#if   L==1	// ca
	"PROFILE.Photo.es";
#elif L==2	// de
	"PROFILE.Photo.en";
#elif L==3	// en
	"PROFILE.Photo.en";
#elif L==4	// es
	"PROFILE.Photo.es";
#elif L==5	// fr
	"PROFILE.Photo.en";
#elif L==6	// gn
	"PROFILE.Photo.es";
#elif L==7	// it
	"PROFILE.Photo.en";
#elif L==8	// pl
	"PROFILE.Photo.en";
#elif L==9	// pt
	"PROFILE.Photo.en";
#elif L==10	// tr
	"PROFILE.Photo.en";
#endif

const char *Hlp_PROFILE_Settings_internationalization =
#if   L==1	// ca
	"PROFILE.Settings.es#internacionalizaci%C3%B3n";
#elif L==2	// de
	"PROFILE.Settings.en#internationalization";
#elif L==3	// en
	"PROFILE.Settings.en#internationalization";
#elif L==4	// es
	"PROFILE.Settings.es#internacionalizaci%C3%B3n";
#elif L==5	// fr
	"PROFILE.Settings.en#internationalization";
#elif L==6	// gn
	"PROFILE.Settings.es#internacionalizaci%C3%B3n";
#elif L==7	// it
	"PROFILE.Settings.en#internationalization";
#elif L==8	// pl
	"PROFILE.Settings.en#internationalization";
#elif L==9	// pt
	"PROFILE.Settings.en#internationalization";
#elif L==10	// tr
	"PROFILE.Settings.en#internationalization";
#endif

const char *Hlp_PROFILE_Settings_language =
#if   L==1	// ca
	"PROFILE.Settings.es#idioma";
#elif L==2	// de
	"PROFILE.Settings.en#language";
#elif L==3	// en
	"PROFILE.Settings.en#language";
#elif L==4	// es
	"PROFILE.Settings.es#idioma";
#elif L==5	// fr
	"PROFILE.Settings.en#language";
#elif L==6	// gn
	"PROFILE.Settings.es#idioma";
#elif L==7	// it
	"PROFILE.Settings.en#language";
#elif L==8	// pl
	"PROFILE.Settings.en#language";
#elif L==9	// pt
	"PROFILE.Settings.en#language";
#elif L==10	// tr
	"PROFILE.Settings.en#language";
#endif

const char *Hlp_PROFILE_Settings_calendar =
#if   L==1	// ca
	"PROFILE.Settings.es#calendario";
#elif L==2	// de
	"PROFILE.Settings.en#calendar";
#elif L==3	// en
	"PROFILE.Settings.en#calendar";
#elif L==4	// es
	"PROFILE.Settings.es#calendario";
#elif L==5	// fr
	"PROFILE.Settings.en#calendar";
#elif L==6	// gn
	"PROFILE.Settings.es#calendario";
#elif L==7	// it
	"PROFILE.Settings.en#calendar";
#elif L==8	// pl
	"PROFILE.Settings.en#calendar";
#elif L==9	// pt
	"PROFILE.Settings.en#calendar";
#elif L==10	// tr
	"PROFILE.Settings.en#calendar";
#endif

const char *Hlp_PROFILE_Settings_dates =
#if   L==1	// ca
	"PROFILE.Settings.es#fechas";
#elif L==2	// de
	"PROFILE.Settings.en#dates";
#elif L==3	// en
	"PROFILE.Settings.en#dates";
#elif L==4	// es
	"PROFILE.Settings.es#fechas";
#elif L==5	// fr
	"PROFILE.Settings.en#dates";
#elif L==6	// gn
	"PROFILE.Settings.es#fechas";
#elif L==7	// it
	"PROFILE.Settings.en#dates";
#elif L==8	// pl
	"PROFILE.Settings.en#dates";
#elif L==9	// pt
	"PROFILE.Settings.en#dates";
#elif L==10	// tr
	"PROFILE.Settings.en#dates";
#endif

const char *Hlp_PROFILE_Settings_design =
#if   L==1	// ca
	"PROFILE.Settings.es#dise%C3%B1o";
#elif L==2	// de
	"PROFILE.Settings.en#design";
#elif L==3	// en
	"PROFILE.Settings.en#design";
#elif L==4	// es
	"PROFILE.Settings.es#dise%C3%B1o";
#elif L==5	// fr
	"PROFILE.Settings.en#design";
#elif L==6	// gn
	"PROFILE.Settings.es#dise%C3%B1o";
#elif L==7	// it
	"PROFILE.Settings.en#design";
#elif L==8	// pl
	"PROFILE.Settings.en#design";
#elif L==9	// pt
	"PROFILE.Settings.en#design";
#elif L==10	// tr
	"PROFILE.Settings.en#design";
#endif

const char *Hlp_PROFILE_Settings_icons =
#if   L==1	// ca
	"PROFILE.Settings.es#iconos";
#elif L==2	// de
	"PROFILE.Settings.en#icons";
#elif L==3	// en
	"PROFILE.Settings.en#icons";
#elif L==4	// es
	"PROFILE.Settings.es#iconos";
#elif L==5	// fr
	"PROFILE.Settings.en#icons";
#elif L==6	// gn
	"PROFILE.Settings.es#iconos";
#elif L==7	// it
	"PROFILE.Settings.en#icons";
#elif L==8	// pl
	"PROFILE.Settings.en#icons";
#elif L==9	// pt
	"PROFILE.Settings.en#icons";
#elif L==10	// tr
	"PROFILE.Settings.en#icons";
#endif

const char *Hlp_PROFILE_Settings_theme =
#if   L==1	// ca
	"PROFILE.Settings.es#tema";
#elif L==2	// de
	"PROFILE.Settings.en#theme";
#elif L==3	// en
	"PROFILE.Settings.en#theme";
#elif L==4	// es
	"PROFILE.Settings.es#tema";
#elif L==5	// fr
	"PROFILE.Settings.en#theme";
#elif L==6	// gn
	"PROFILE.Settings.es#tema";
#elif L==7	// it
	"PROFILE.Settings.en#theme";
#elif L==8	// pl
	"PROFILE.Settings.en#theme";
#elif L==9	// pt
	"PROFILE.Settings.en#theme";
#elif L==10	// tr
	"PROFILE.Settings.en#theme";
#endif

const char *Hlp_PROFILE_Settings_menu =
#if   L==1	// ca
	"PROFILE.Settings.es#men%C3%BA";
#elif L==2	// de
	"PROFILE.Settings.en#menu";
#elif L==3	// en
	"PROFILE.Settings.en#menu";
#elif L==4	// es
	"PROFILE.Settings.es#men%C3%BA";
#elif L==5	// fr
	"PROFILE.Settings.en#menu";
#elif L==6	// gn
	"PROFILE.Settings.es#men%C3%BA";
#elif L==7	// it
	"PROFILE.Settings.en#menu";
#elif L==8	// pl
	"PROFILE.Settings.en#menu";
#elif L==9	// pt
	"PROFILE.Settings.en#menu";
#elif L==10	// tr
	"PROFILE.Settings.en#menu";
#endif

const char *Hlp_PROFILE_Settings_columns =
#if   L==1	// ca
	"PROFILE.Settings.es#columnas";
#elif L==2	// de
	"PROFILE.Settings.en#columns";
#elif L==3	// en
	"PROFILE.Settings.en#columns";
#elif L==4	// es
	"PROFILE.Settings.es#columnas";
#elif L==5	// fr
	"PROFILE.Settings.en#columns";
#elif L==6	// gn
	"PROFILE.Settings.es#columnas";
#elif L==7	// it
	"PROFILE.Settings.en#columns";
#elif L==8	// pl
	"PROFILE.Settings.en#columns";
#elif L==9	// pt
	"PROFILE.Settings.en#columns";
#elif L==10	// tr
	"PROFILE.Settings.en#columns";
#endif

const char *Hlp_PROFILE_Settings_user_photos =
#if   L==1	// ca
	"PROFILE.Settings.es#fotos-de-usuarios";
#elif L==2	// de
	"PROFILE.Settings.en#user-photos";
#elif L==3	// en
	"PROFILE.Settings.en#user-photos";
#elif L==4	// es
	"PROFILE.Settings.es#fotos-de-usuarios";
#elif L==5	// fr
	"PROFILE.Settings.en#user-photos";
#elif L==6	// gn
	"PROFILE.Settings.es#fotos-de-usuarios";
#elif L==7	// it
	"PROFILE.Settings.en#user-photos";
#elif L==8	// pl
	"PROFILE.Settings.en#user-photos";
#elif L==9	// pt
	"PROFILE.Settings.en#user-photos";
#elif L==10	// tr
	"PROFILE.Settings.en#user-photos";
#endif

const char *Hlp_PROFILE_Settings_privacy =
#if   L==1	// ca
	"PROFILE.Settings.es#privacidad";
#elif L==2	// de
	"PROFILE.Settings.en#privacy";
#elif L==3	// en
	"PROFILE.Settings.en#privacy";
#elif L==4	// es
	"PROFILE.Settings.es#privacidad";
#elif L==5	// fr
	"PROFILE.Settings.en#privacy";
#elif L==6	// gn
	"PROFILE.Settings.es#privacidad";
#elif L==7	// it
	"PROFILE.Settings.en#privacy";
#elif L==8	// pl
	"PROFILE.Settings.en#privacy";
#elif L==9	// pt
	"PROFILE.Settings.en#privacy";
#elif L==10	// tr
	"PROFILE.Settings.en#privacy";
#endif

const char *Hlp_PROFILE_Settings_cookies =
#if   L==1	// ca
	"PROFILE.Settings.es#cookies";
#elif L==2	// de
	"PROFILE.Settings.en#cookies";
#elif L==3	// en
	"PROFILE.Settings.en#cookies";
#elif L==4	// es
	"PROFILE.Settings.es#cookies";
#elif L==5	// fr
	"PROFILE.Settings.en#cookies";
#elif L==6	// gn
	"PROFILE.Settings.es#cookies";
#elif L==7	// it
	"PROFILE.Settings.en#cookies";
#elif L==8	// pl
	"PROFILE.Settings.en#cookies";
#elif L==9	// pt
	"PROFILE.Settings.en#cookies";
#elif L==10	// tr
	"PROFILE.Settings.en#cookies";
#endif

const char *Hlp_PROFILE_Settings_notifications =
#if   L==1	// ca
	"PROFILE.Settings.es#notificaciones";
#elif L==2	// de
	"PROFILE.Settings.en#notifications";
#elif L==3	// en
	"PROFILE.Settings.en#notifications";
#elif L==4	// es
	"PROFILE.Settings.es#notificaciones";
#elif L==5	// fr
	"PROFILE.Settings.en#notifications";
#elif L==6	// gn
	"PROFILE.Settings.es#notificaciones";
#elif L==7	// it
	"PROFILE.Settings.en#notifications";
#elif L==8	// pl
	"PROFILE.Settings.en#notifications";
#elif L==9	// pt
	"PROFILE.Settings.en#notifications";
#elif L==10	// tr
	"PROFILE.Settings.en#notifications";
#endif
