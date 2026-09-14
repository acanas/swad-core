#!/usr/bin/python
#
# swad_smtp.py: script called from SWAD email_to send e-mail
#
##########################################################################
#
#   SWAD (Shared Workspace At a Distance,
#   "Web System for Teaching Support" in English),
#   is a web platform developed at the University of Granada (Spain),
#   and used email_to support university teaching.
#   Copyright (C) 1999-2014 Antonio Canas-Vargas
#   & Daniel J. Calandria-Hernandez,
#   University of Granada (SPAIN) (acanas@ugr.es)
#
#   This program is free software: you can redistribute it and/or modify
#   it under the terms of the GNU Affero General Public License as
#   published by the Free Software Foundation, either version 3 of the
#   License, or (at your option) any later version.
#
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU Affero General Public License for more details.
#
#   You should have received a copy of the GNU Affero General Public License
#   along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
##########################################################################
#
#    This file has been based on a script from Antonio F. Diaz-Garcia

import os.path
import sys
from email.utils import formatdate
from smtplib import SMTP
from smtplib import SMTPException

# Read arguments
if len(sys.argv) < 8:
	#	Error, use: swad_smtp smtp_server smtp_port email_from email_password email_to email_subject email_content_filename
	sys.exit(2)

smtp_server = sys.argv[1]
smtp_port = int (sys.argv[2])
email_from = sys.argv[3]
email_password = sys.argv[4]
email_to = [sys.argv[5]]
if sys.version_info[0] >= 3:
	email_subject = sys.argv[6].encode('utf-8', 'surrogateescape').decode('iso-8859-1')	# En Python 3
else:
	email_subject = sys.argv[6]								# En Python 2
email_content_filename = sys.argv[7]

# Read content from file
if not os.path.exists(email_content_filename):
	sys.exit(3)
if sys.version_info[0] >= 3:
	email_content_file = open (email_content_filename,'rb')		# En Python 3
	email_txt = email_content_file.read().decode('iso-8859-1')	# En Python 3
else:
	email_content_file = open (email_content_filename,'r')		# En Python 2
	email_txt = email_content_file.read()				# En Python 2
email_content_file.close()

# Compose message
email_date = formatdate()
msg = ("From: %s\r\nTo: %s\r\nContent-type: text/plain; charset=iso-8859-1\r\nSubject: %s\r\nDate: %s\r\n\r\n"
       % (email_from, ", ".join(email_to),email_subject,email_date))
msg = msg + email_txt
if sys.version_info[0] >= 3:
	msg = msg.encode('iso-8859-1')					# En Python 3

try:
	# Create SMTP object
	smtpObj = SMTP(smtp_server, smtp_port)

	# Identify yourself email_to SMTP server
	smtpObj.ehlo()

	# Put SMTP connection in TLS mode and call ehlo again
	smtpObj.starttls()
	smtpObj.ehlo()

	# Login email_to service
	smtpObj.login(user=email_from, password=email_password)

	# Send email
	smtpObj.sendmail(email_from, email_to, msg)

	# Close connection and session.
	smtpObj.quit()
	
except SMTPException:
	sys.exit(1)
