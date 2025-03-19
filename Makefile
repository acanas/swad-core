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
#   Copyright (C) 1999-2025 Antonio Cañas Vargas                              #
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

OBJS = swad_account.o swad_account_database.o swad_action.o swad_action_list.o \
       swad_admin.o swad_admin_database.o swad_agenda.o swad_agenda_database.o \
       swad_alert.o swad_announcement.o swad_announcement_database.o \
       swad_API.o swad_API_database.o swad_assignment.o \
       swad_assignment_database.o swad_assignment_resource.o swad_attendance.o \
       swad_attendance_database.o swad_attendance_resource.o swad_autolink.o \
       swad_banner.o swad_banner_database.o swad_bibliography.o \
       swad_bibliography_database.o swad_box.o swad_browser.o \
       swad_browser_database.o swad_browser_resource.o swad_browser_size.o \
       swad_building.o swad_building_database.o swad_button.o \
       swad_calendar.o swad_call_for_exam.o swad_call_for_exam_resource.o \
       swad_call_for_exam_database.o swad_center.o swad_center_config.o \
       swad_center_database.o swad_chat.o swad_chat_database.o \
       swad_closed_open.o swad_config.o swad_connected.o \
       swad_connected_database.o swad_cookie.o \
       swad_cookie_database.o swad_country.o swad_country_config.o \
       swad_country_database.o swad_course.o swad_course_config.o \
       swad_course_database.o swad_cryptography.o \
       swad_database.o swad_date.o swad_degree.o swad_degree_config.o \
       swad_degree_database.o swad_degree_type.o swad_deny_allow.o \
       swad_department.o swad_department_database.o swad_duplicate.o \
       swad_duplicate_database.o \
       swad_enrolment.o swad_enrolment_database.o swad_error.o swad_exam.o \
       swad_exam_database.o swad_exam_log.o swad_exam_print.o \
       swad_exam_resource.o swad_exam_result.o swad_exam_session.o \
       swad_exam_set.o \
       swad_FAQ.o swad_FAQ_database.o swad_figure.o swad_figure_cache.o \
       swad_figure_database.o swad_file.o swad_file_database.o \
       swad_file_extension.o swad_file_MIME.o swad_firewall.o \
       swad_firewall_database.o swad_follow.o swad_follow_database.o \
       swad_form.o swad_forum.o swad_forum_database.o swad_forum_resource.o \
       swad_game.o swad_game_database.o swad_game_resource.o swad_global.o \
       swad_group.o swad_group_resource.o swad_group_database.o \
       swad_help.o swad_hidden_visible.o swad_hierarchy.o \
       swad_hierarchy_config.o swad_hierarchy_database.o swad_holiday.o \
       swad_holiday_database.o swad_HTML.o \
       swad_icon.o swad_ID.o swad_ID_database.o swad_indicator.o \
       swad_indicator_database.o swad_info.o swad_info_database.o \
       swad_info_resource.o swad_institution.o swad_institution_config.o \
       swad_institution_database.o \
       swad_language.o swad_layout.o swad_link.o swad_link_database.o \
       swad_log.o swad_log_database.o swad_logo.o \
       swad_MAC.o swad_mail.o swad_mail_database.o swad_main.o \
       swad_maintenance.o swad_map.o swad_mark.o swad_mark_database.o \
       swad_match.o swad_match_database.o swad_match_print.o \
       swad_match_result.o swad_media.o swad_media_database.o swad_menu.o \
       swad_message.o swad_message_database.o swad_MFU.o swad_MFU_database.o \
       swad_name.o swad_network.o swad_network_database.o swad_nickname.o \
       swad_nickname_database.o swad_notice.o swad_notice_database.o \
       swad_notification.o swad_notification_database.o \
       swad_pagination.o swad_parameter.o swad_parameter_code.o \
       swad_password.o swad_password_database.o swad_photo.o \
       swad_photo_database.o swad_photo_shape.o swad_place.o \
       swad_place_database.o swad_plugin.o swad_plugin_database.o \
       swad_privacy.o swad_private_public.o swad_process.o swad_profile.o \
       swad_profile_database.o swad_program.o swad_program_resource.o \
       swad_project.o swad_project_config.o swad_project_database.o \
       swad_project_resource.o \
       swad_question.o swad_question_database.o swad_question_import.o \
       swad_QR.o \
       swad_record.o swad_record_database.o swad_report.o \
       swad_report_database.o swad_resource.o swad_resource_database.o \
       swad_role.o swad_role_database.o swad_room.o swad_room_database.o \
       swad_RSS.o swad_rubric.o swad_rubric_criteria.o swad_rubric_database.o \
       swad_rubric_resource.o \
       swad_scope.o swad_search.o swad_session.o swad_session_database.o \
       swad_setting.o swad_setting_database.o swad_statistic.o \
       swad_statistic_database.o swad_string.o swad_survey.o \
       swad_survey_database.o swad_survey_resource.o swad_system_config.o \
       swad_system_link.o swad_system_link_database.o \
       swad_tab.o swad_tag.o swad_tag_database.o swad_tag_resource.o \
       swad_test.o swad_test_database.o swad_test_config.o swad_test_print.o \
       swad_test_resource.o swad_test_visibility.o swad_theme.o \
       swad_timeline.o swad_timeline_comment.o swad_timeline_database.o \
       swad_timeline_favourite.o swad_timeline_form.o swad_timeline_note.o \
       swad_timeline_notification.o swad_timeline_post.o \
       swad_timeline_publication.o swad_timeline_share.o swad_timeline_user.o \
       swad_timeline_who.o swad_timetable.o swad_timetable_database.o \
       swad_timetable_resource.o swad_tree.o swad_tree_database.o \
       swad_tree_specific.o \
       swad_user.o swad_user_clipboard.o swad_user_database.o \
       swad_user_resource.o \
       swad_wrong_correct.o \
       swad_xml.o \
       swad_zip.o
SOAPOBJS = soap/soapC.o soap/soapServer.o
SHAOBJS = sha2/sha2.o
CC = gcc

# LIBS when using MySQL:
#LIBS = -lmysqlclient -lz -L/usr/lib64/mysql -lm -lgsoap

# LIBS when using MariaDB (also valid with MySQL):
LIBS = -lssl -lcrypto -lpthread -lrt -lmysqlclient -lz -L/usr/lib64/mysql -lm -lgsoap

# CFLAGS = -Wall -Wextra -mtune=native -O2 -s
CFLAGS = -Wall -Wextra -mtune=native -O2

all: swad_ca swad_de swad_en swad_es swad_fr swad_gn swad_it swad_pl swad_pt swad_tr

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

swad_tr: $(OBJS) $(SOAPOBJS)
	$(CC) $(CFLAGS) -c -D L=10 swad_help_URL.c swad_text.c swad_text_action.c swad_text_no_html.c
	$(CC) $(CFLAGS) -o $@ $(OBJS) swad_help_URL.o swad_text.o swad_text_action.o swad_text_no_html.o $(SOAPOBJS) $(SHAOBJS) $(LIBS)
	chmod a+x $@

.PHONY: clean

clean:
	rm -f swad swad_ca swad_de swad_en swad_es swad_fr swad_gn swad_it swad_pl swad_pt swad_tr swad_help_URL.o swad_text.o swad_text_no_html.o swad_text_action.o $(OBJS) 
