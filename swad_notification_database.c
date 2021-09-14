// swad_notification_database.c: notifications operations with database

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2021 Antonio Cañas Vargas

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

// #include <stddef.h>		// For NULL
// #include <stdlib.h>		// For system
// #include <string.h>
// #include <sys/wait.h>		// For the macro WEXITSTATUS
// #include <unistd.h>		// For unlink

// #include "swad_action.h"
// #include "swad_box.h"
// #include "swad_call_for_exam.h"
// #include "swad_config.h"
// #include "swad_config.h"
#include "swad_database.h"
// #include "swad_enrolment.h"
// #include "swad_error.h"
// #include "swad_figure.h"
// #include "swad_follow.h"
// #include "swad_form.h"
// #include "swad_forum.h"
// #include "swad_global.h"
// #include "swad_hierarchy.h"
// #include "swad_hierarchy_level.h"
// #include "swad_HTML.h"
// #include "swad_mark.h"
// #include "swad_message.h"
// #include "swad_notice.h"
// #include "swad_notification.h"
#include "swad_notification_database.h"
// #include "swad_parameter.h"
// #include "swad_survey.h"
// #include "swad_timeline.h"
// #include "swad_timeline_notification.h"
// #include "swad_timeline_publication.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/****************************** Public constants *****************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private constants *****************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

/*****************************************************************************/
/************ Set possible notifications of children as removed **************/
/*****************************************************************************/

void Ntf_DB_MarkNotifChildrenOfFolderAsRemoved (Ntf_NotifyEvent_t NotifyEvent,
                                                Brw_FileBrowser_t FileBrowser,
                                                long Cod,const char *Path)
  {
   DB_QueryUPDATE ("can not set notification(s) as removed",
		   "UPDATE ntf_notifications"
		     " SET Status=(Status | %u)"
		   " WHERE NotifyEvent=%u"
		     " AND Cod IN"
			 " (SELECT FilCod"
			    " FROM brw_files"
			   " WHERE FileBrowser=%u"
			     " AND Cod=%ld"
			     " AND Path LIKE '%s/%%')",
		   (unsigned) Ntf_STATUS_BIT_REMOVED,
		   (unsigned) NotifyEvent,
		   (unsigned) FileBrowser,
		   Cod,
		   Path);
  }
