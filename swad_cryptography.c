// swad_cryptography.c: cryptography

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2026 Antonio Cañas Vargas

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

#include <string.h>		// For string functions
#include <unistd.h>		// For access, lstat, getpid, chdir, symlink

#include "sha2/sha2.h"		// For sha-256 and sha-512 algorithms
#include "swad_constant.h"
#include "swad_cryptography.h"
#include "swad_global.h"
#include "swad_parameter.h"
#include "swad_process.h"

/*****************************************************************************/
/**************************** Private constants ******************************/
/*****************************************************************************/

/***** SHA-256 algorithm *****/
#define BITS_SHA256_ENCRYPTION 256
#define BYTES_SHA256_ENCRYPTION (BITS_SHA256_ENCRYPTION/8)

/***** SHA-512 algorithm *****/
#define BITS_SHA512_ENCRYPTION 512
#define BYTES_SHA512_ENCRYPTION (BITS_SHA512_ENCRYPTION/8)

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern const char Str_BIN_TO_BASE64URL[64 + 1];

/*****************************************************************************/
/************************* Private global variables **************************/
/*****************************************************************************/

static char Cry_UniqueNameEncrypted[Cry_BYTES_ENCRYPTED_STR_SHA256_BASE64 + 1];	// Used for session id, temporary directory names, etc.

/*****************************************************************************/
/*************** Encrypt a plain text using SHA=256 algorithm ****************/
/*****************************************************************************/

// Pointers to PlainText and EncryptedText can point to the same place
void Cry_EncryptSHA256Base64 (const char *PlainText,
                              char EncryptedText[Cry_BYTES_ENCRYPTED_STR_SHA256_BASE64 + 1])
  {
   int i,j;
   unsigned char digest256[SHA256_DIGEST_SIZE];

   /* Encrypt function */
   sha256 ((const unsigned char *) PlainText, strlen (PlainText), digest256);

   /* Copy digest256 into EncryptedText, changing from binary to BASE64 */
   for (i = 0, j = 0;
	i < BYTES_SHA256_ENCRYPTION;
	i += 3)	// 256 bits, 32 bytes
     {
      EncryptedText[j++]    = Str_BIN_TO_BASE64URL [ (digest256[  i  ] & 0xFC) >> 2];
      if (i == (BYTES_SHA256_ENCRYPTION-2))	// i == 30, j == 42, last character to fill in encrypted text
        {
         EncryptedText[j++] = Str_BIN_TO_BASE64URL [((digest256[  i  ] & 0x03) << 4) | ((digest256[i + 1] & 0xF0) >> 4)];
         EncryptedText[ j ] = Str_BIN_TO_BASE64URL [((digest256[i + 1] & 0x0F) << 2)                                 ];
        }
      else
        {
         EncryptedText[j++] = Str_BIN_TO_BASE64URL [((digest256[  i  ] & 0x03) << 4) | ((digest256[i + 1] & 0xF0) >> 4)];
         EncryptedText[j++] = Str_BIN_TO_BASE64URL [((digest256[i + 1] & 0x0F) << 2) | ((digest256[i + 2] & 0xC0) >> 6)];
         EncryptedText[j++] = Str_BIN_TO_BASE64URL [ (digest256[i + 2] & 0x3F)                                       ];
        }
     }
   EncryptedText[Cry_BYTES_ENCRYPTED_STR_SHA256_BASE64] = '\0';
  }

/*****************************************************************************/
/*************** Encrypt a plain text using SHA=512 algorithm ****************/
/*****************************************************************************/

// Pointers to PlainText and EncryptedText can point to the same place
void Cry_EncryptSHA512Base64 (const char *PlainText,
                              char EncryptedText[Cry_LENGTH_ENCRYPTED_STR_SHA512_BASE64 + 1])
  {
   int i;
   int j;
   unsigned char digest512[SHA512_DIGEST_SIZE];

   /* Encrypt function */
   sha512 ((const unsigned char *) PlainText, strlen (PlainText), digest512);

   /* Copy digest512 into EncryptedText, changing from binary to BASE64 */
   for (i = 0, j = 0;
	i < BYTES_SHA512_ENCRYPTION;
	i += 3)	// 512 bits, 64 bytes
     {
      EncryptedText[j++]    = Str_BIN_TO_BASE64URL [ (digest512[  i  ] & 0xFC) >> 2];
      if (i == (BYTES_SHA512_ENCRYPTION-1))	// i == 63, j == 85, last character to fill in encrypted text
         EncryptedText[ j ] = Str_BIN_TO_BASE64URL [ (digest512[  i  ] & 0x03) << 4];
      else
        {
         EncryptedText[j++] = Str_BIN_TO_BASE64URL [((digest512[  i  ] & 0x03) << 4) | ((digest512[i + 1] & 0xF0) >> 4)];
         EncryptedText[j++] = Str_BIN_TO_BASE64URL [((digest512[i + 1] & 0x0F) << 2) | ((digest512[i + 2] & 0xC0) >> 6)];
         EncryptedText[j++] = Str_BIN_TO_BASE64URL [ (digest512[i + 2] & 0x3F)                                       ];
        }
     }
   EncryptedText[Cry_LENGTH_ENCRYPTED_STR_SHA512_BASE64] = '\0';
  }

/*****************************************************************************/
/*** Create a unique name encrypted, different each time function is called **/
/*****************************************************************************/

void Cry_CreateUniqueNameEncrypted (char *UniqueNameEncrypted)
  {
   static unsigned NumCall = 0;	// When this function is called several times in the same execution of the program, each time a new name is created
   char UniqueNamePlain[Cns_MAX_BYTES_IP +
                        Cns_MAX_DIGITS_LONG +
                        Cns_MAX_DIGITS_LONG +
                        Cns_MAX_DIGITS_UINT + 1];

   NumCall++;
   snprintf (UniqueNamePlain,sizeof (UniqueNamePlain),"%s-%lx-%lx-%x",
             Par_GetIP (),
             (long) Dat_GetStartExecutionTimeUTC (),
             (long) Prc_GetPID (),
             NumCall);
   Cry_EncryptSHA256Base64 (UniqueNamePlain,UniqueNameEncrypted);	// Make difficult to guess a unique name
  }

char *Cry_GetUniqueNameEncrypted (void)
  {
   return Cry_UniqueNameEncrypted;
  }
