##########################################################################
#
# Makefile to compile SWAD core
#
###############################################################################
#                                                                             #
#   SWAD (Shared Workspace At a Distance),                                    #
#   is a web platform developed at the University of Granada (Spain),         #
#   and used to support university teaching.                                  #
#                                                                             #
#   This file is part of SWAD core.                                           #
#   Copyright (C) 1999-2020 Antonio Cañas Vargas                              #
#                                                                             #
#   This program is free software: you can redistribute it and/or modify      #
#   it under the terms of the GNU Affero General Public License as            #
#   published by the Free Software Foundation, either version 3 of the        #
#   License, or (at your option) any later version.                           #
#                                                                             #
#   This program is distributed in the hope that it will be useful,           #
#   but WITHOUT ANY WARRANTY; without even the implied warranty of            #
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             #
#   GNU Affero General Public License for more details.                       #
#                                                                             #
#   You should have received a copy of the GNU Affero General Public License  #
#   along with this program.  If not, see <http://www.gnu.org/licenses/>.     #
#                                                                             #
###############################################################################

OBJS = swad_account.o swad_action.o swad_agenda.o swad_alert.o \
       swad_announcement.o swad_API.o swad_assignment.o swad_attendance.o \
       swad_banner.o swad_box.o swad_button.o \
       swad_calendar.o swad_centre.o swad_centre_config.o swad_chat.o \
       swad_classroom.o swad_config.o swad_connected.o swad_cookie.o \
       swad_country.o swad_country_config.o swad_course.o \
       swad_course_config.o swad_cryptography.o \
       swad_database.o swad_date.o swad_degree.o swad_degree_config.o \
       swad_degree_type.o swad_department.o swad_duplicate.o \
       swad_enrolment.o swad_exam.o \
       swad_figure.o swad_file.o swad_file_browser.o swad_file_extension.o \
       swad_file_MIME.o swad_firewall.o swad_follow.o swad_form.o \
       swad_forum.o \
       swad_game.o swad_global.o swad_group.o \
       swad_help.o swad_hierarchy.o swad_hierarchy_config.o swad_holiday.o \
       swad_HTML.o \
       swad_icon.o swad_ID.o swad_indicator.o swad_info.o swad_institution.o \
       swad_institution_config.o \
       swad_language.o swad_layout.o swad_link.o swad_log.o swad_logo.o \
       swad_mail.o swad_main.o swad_maintenance.o swad_map.o swad_mark.o \
       swad_match.o swad_match_result.o swad_media.o swad_menu.o \
       swad_message.o swad_MFU.o \
       swad_network.o swad_nickname.o swad_notice.o swad_notification.o \
       swad_pagination.o swad_parameter.o swad_password.o swad_photo.o \
       swad_place.o swad_plugin.o swad_privacy.o swad_profile.o \
       swad_program.o swad_project.o \
       swad_QR.o \
       swad_record.o swad_report.o swad_role.o swad_RSS.o \
       swad_scope.o swad_search.o swad_session.o swad_setting.o \
       swad_statistic.o swad_string.o swad_survey.o swad_syllabus.o \
       swad_system_config.o \
       swad_tab.o swad_test.o swad_test_config.o swad_test_exam.o \
       swad_test_import.o swad_test_visibility.o swad_theme.o swad_timeline.o \
       swad_timetable.o \
       swad_user.o \
       swad_xml.o \
       swad_zip.o
SOAPOBJS = soap/soapC.o soap/soapServer.o
SHAOBJS = sha2/sha2.o
CC = gcc

# LIBS when using MySQL:
#LIBS = -lmysqlclient -lz -L/usr/lib64/mysql -lm -lgsoap

# LIBS when using MariaDB (also valid with MySQL):
LIBS = -lssl -lcrypto -lpthread -lrt -lmysqlclient -lz -L/usr/lib64/mysql -lm -lgsoap

CFLAGS = -Wall -Wextra -mtune=native -O2 -s

all: swad_ca swad_de swad_en swad_es swad_fr swad_gn swad_it swad_pl swad_pt

swad_ca: $(OBJS) $(SOAPOBJS) $(SHAOBJS)
	$(CC) $(CFLAGS) -c -D L=1 swad_help_URL.c swad_text.c swad_text_action.c swad_text_no_html.c
	$(CC) $(CFLAGS) -o $@ $(OBJS) swad_help_URL.o swad_text.o swad_text_action.o swad_text_no_html.o $(SOAPOBJS) $(SHAOBJS) $(LIBS)
	chmod a+x $@

swad_de: $(OBJS) $(SOAPOBJS)
	$(CC) $(CFLAGS) -c -D L=2 swad_help_URL.c swad_text.c swad_text_action.c swad_text_no_html.c
	$(CC) $(CFLAGS) -o $@ $(OBJS) swad_help_URL.o swad_text.o swad_text_action.o swad_text_no_html.o $(SOAPOBJS) $(SHAOBJS) $(LIBS)
	chmod a+x $@

swad_en: $(OBJS) $(SOAPOBJS)
	$(CC) $(CFLAGS) -c -D L=3 swad_help_URL.c swad_text.c swad_text_action.c swad_text_no_html.c
	$(CC) $(CFLAGS) -o $@ $(OBJS) swad_help_URL.o swad_text.o swad_text_action.o swad_text_no_html.o $(SOAPOBJS) $(SHAOBJS) $(LIBS)
	chmod a+x $@

swad_es: $(OBJS) $(SOAPOBJS)
	$(CC) $(CFLAGS) -c -D L=4 swad_help_URL.c swad_text.c swad_text_action.c swad_text_no_html.c
	$(CC) $(CFLAGS) -o $@ $(OBJS) swad_help_URL.o swad_text.o swad_text_action.o swad_text_no_html.o $(SOAPOBJS) $(SHAOBJS) $(LIBS)
	chmod a+x $@

swad_fr: $(OBJS) $(SOAPOBJS)
	$(CC) $(CFLAGS) -c -D L=5 swad_help_URL.c swad_text.c swad_text_action.c swad_text_no_html.c
	$(CC) $(CFLAGS) -o $@ $(OBJS) swad_help_URL.o swad_text.o swad_text_action.o swad_text_no_html.o $(SOAPOBJS) $(SHAOBJS) $(LIBS)
	chmod a+x $@

swad_gn: $(OBJS) $(SOAPOBJS)
	$(CC) $(CFLAGS) -c -D L=6 swad_help_URL.c swad_text.c swad_text_action.c swad_text_no_html.c
	$(CC) $(CFLAGS) -o $@ $(OBJS) swad_help_URL.o swad_text.o swad_text_action.o swad_text_no_html.o $(SOAPOBJS) $(SHAOBJS) $(LIBS)
	chmod a+x $@

swad_it: $(OBJS) $(SOAPOBJS)
	$(CC) $(CFLAGS) -c -D L=7 swad_help_URL.c swad_text.c swad_text_action.c swad_text_no_html.c
	$(CC) $(CFLAGS) -o $@ $(OBJS) swad_help_URL.o swad_text.o swad_text_action.o swad_text_no_html.o $(SOAPOBJS) $(SHAOBJS) $(LIBS)
	chmod a+x $@

swad_pl: $(OBJS) $(SOAPOBJS)
	$(CC) $(CFLAGS) -c -D L=8 swad_help_URL.c swad_text.c swad_text_action.c swad_text_no_html.c
	$(CC) $(CFLAGS) -o $@ $(OBJS) swad_help_URL.o swad_text.o swad_text_action.o swad_text_no_html.o $(SOAPOBJS) $(SHAOBJS) $(LIBS)
	chmod a+x $@

swad_pt: $(OBJS) $(SOAPOBJS)
	$(CC) $(CFLAGS) -c -D L=9 swad_help_URL.c swad_text.c swad_text_action.c swad_text_no_html.c
	$(CC) $(CFLAGS) -o $@ $(OBJS) swad_help_URL.o swad_text.o swad_text_action.o swad_text_no_html.o $(SOAPOBJS) $(SHAOBJS) $(LIBS)
	chmod a+x $@

.PHONY: clean

clean:
	rm -f swad swad_ca swad_de swad_en swad_es swad_fr swad_gn swad_it swad_pl swad_pt swad_help_URL.o swad_text.o swad_text_no_html.o swad_text_action.o $(OBJS) 
