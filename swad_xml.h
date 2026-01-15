// swad_xml.h: XML file generation and parsing

#ifndef _SWAD_XML
#define _SWAD_XML
/*
    SWAD (Shared Workspace At a Distance in Spanish),
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
/********************************** Headers **********************************/
/*****************************************************************************/

#include <stdio.h>	// For FILE *

/*****************************************************************************/
/******************************** Public types *******************************/
/*****************************************************************************/

struct XMLAttribute
  {
   struct XMLAttribute *Next;
   char *AttributeName;
   size_t AttributeNameLength;
   char *Content;
   size_t ContentLength;
  };

struct XMLElement
  {
   struct XMLElement *FirstChild;
   struct XMLElement *LastChild;
   struct XMLElement *NextBrother;
   struct XMLAttribute *FirstAttribute;
   struct XMLAttribute *LastAttribute;
   char *TagName;
   size_t TagNameLength;
   char *Content;
   size_t ContentLength;
  };

/*****************************************************************************/
/****************************** Public prototypes ****************************/
/*****************************************************************************/

void XML_WriteStartFile (FILE *FileTgt,const char *Type);
void XML_WriteEndFile (FILE *FileTgt,const char *Type);

void XML_GetTree (const char *XMLBuffer,struct XMLElement **XMLRootElem);
void XML_PrintTree (struct XMLElement *ParentElem);
bool XML_GetAttributteYesNoFromXMLTree (struct XMLAttribute *Attribute);
void XML_FreeTree (struct XMLElement *ParentElem);

#endif
