// swad_mail_type.h: types related to email

#ifndef _SWAD_MAI_TYP
#define _SWAD_MAI_TYP
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

#include "swad_constant.h"

/*****************************************************************************/
/************************** Public types and constants ***********************/
/*****************************************************************************/

#define Mai_MAX_CHARS_MAIL_INFO		(128 - 1)	// 127
#define Mai_MAX_BYTES_MAIL_INFO		((Mai_MAX_CHARS_MAIL_INFO + 1) * Cns_MAX_BYTES_PER_CHAR - 1)	// 2047

#define Mai_LENGTH_EMAIL_CONFIRM_KEY	Cry_BYTES_ENCRYPTED_STR_SHA256_BASE64

typedef enum
  {
   Mai_NOT_CONFIRMED	= 0,
   Mai_CONFIRMED	= 1,
  } Mai_Confirmed_t;

#define Mai_NUM_ORDERS 3
typedef enum
  {
   Mai_ORDER_BY_DOMAIN = 0,
   Mai_ORDER_BY_INFO   = 1,
   Mai_ORDER_BY_USERS  = 2,
  } Mai_DomainsOrder_t;
#define Mai_ORDER_DEFAULT Mai_ORDER_BY_USERS

struct Mail
  {
   long MaiCod;
   char Domain[Cns_MAX_BYTES_EMAIL_ADDRESS + 1];
   char Info[Mai_MAX_BYTES_MAIL_INFO + 1];
   unsigned NumUsrs;
  };

#endif
