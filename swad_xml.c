// swad_xml.c: XML file generation and parsing

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2022 Antonio Ca�as Vargas

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

#include <ctype.h>		// For isspace()
#include <stddef.h>		// For NULL
#include <stdlib.h>		// For free ()
#include <string.h>		// For strlen (), etc.

#include "swad_changelog.h"
#include "swad_error.h"
#include "swad_global.h"
#include "swad_HTML.h"
#include "swad_xml.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void XML_GetElement (struct XMLElement *ParentElem);
static void XML_GetAttributes (struct XMLElement *Elem);
static void XML_SkipSpaces (void);

/*****************************************************************************/
/****** Write the start of an XML file with author and date of creation ******/
/*****************************************************************************/

void XML_WriteStartFile (FILE *FileTgt,const char *Type,bool Credits)
  {
   extern const char *Txt_NEW_LINE;

   fprintf (FileTgt,"<?xml version=\"1.0\" encoding=\"windows-1252\"?>%s"
                    "<%s>%s",
            Txt_NEW_LINE,
            Type,Txt_NEW_LINE);
   if (Credits)
      fprintf (FileTgt,"<generator>%s</generator>%s"
                       "<author>%s</author>%s"
                       "<date>%ld</date>%s",
               Log_PLATFORM_VERSION,Txt_NEW_LINE,
               Gbl.Usrs.Me.UsrDat.FullName,Txt_NEW_LINE,
               time (NULL),Txt_NEW_LINE);
  }

/*****************************************************************************/
/********************* Write the end of an XML file **************************/
/*****************************************************************************/

void XML_WriteEndFile (FILE *FileTgt,const char *Type)
  {
   extern const char *Txt_NEW_LINE;

   fprintf (FileTgt,"</%s>%s",Type,Txt_NEW_LINE);
  }

/*****************************************************************************/
/****************** Get the content of XML root element **********************/
/*****************************************************************************/

void XML_GetTree (const char *XMLBuffer,struct XMLElement **XMLRootElem)
  {
   /***** Allocate space for the root element *****/
   if ((*XMLRootElem = calloc (1,sizeof (**XMLRootElem))) == NULL)
      Err_NotEnoughMemoryExit ();

   Gbl.XMLPtr = XMLBuffer;
   XML_GetElement (*XMLRootElem);
  }

/*****************************************************************************/
/******************* Get the content of an XML element ***********************/
/*****************************************************************************/

static void XML_GetElement (struct XMLElement *ParentElem)
  {
   struct XMLElement *ChildElem;
   const char *StartContent;
   const char *Ptr;
   size_t ContentLength;
   size_t EndTagNameLength;
   size_t TagLength;
   char ErrorTxt[128];

   /*
   <parent...>  element content  </parent>
              ^
             Gbl.XMLPtr
   */
   /* Skip spaces */
   XML_SkipSpaces ();

   StartContent = Gbl.XMLPtr;
   ContentLength = strcspn (StartContent,"<");
   Gbl.XMLPtr += ContentLength;

   while (*Gbl.XMLPtr == '<')	// For each child until parent end tag
     {
      Gbl.XMLPtr++;
      if (*Gbl.XMLPtr == '/')	// Parent end tag
        {
         /*
         <parent>  element content  </parent>
                                     ^
                                    Gbl.XMLPtr
         */
         /***** Check tag name *****/
         Gbl.XMLPtr++;
         /*
         <parent>  element content  </parent>
                                      ^
                                     Gbl.XMLPtr
         */
         EndTagNameLength = strcspn (Gbl.XMLPtr,">");
         if (ParentElem->TagNameLength != EndTagNameLength)
           {
            snprintf (ErrorTxt,sizeof (ErrorTxt),
	              "XML syntax error. Expect end tag &lt;/%s&gt;.",
		      ParentElem->TagName);
            Err_ShowErrorAndExit (ErrorTxt);
           }
         if (strncmp (ParentElem->TagName,Gbl.XMLPtr,EndTagNameLength))	// XML tags are case sensitive
           {
            snprintf (ErrorTxt,sizeof (ErrorTxt),
	              "XML syntax error. Expect end tag &lt;/%s&gt;.",
		      ParentElem->TagName);
            Err_ShowErrorAndExit (ErrorTxt);
           }

         // End of parent element found!
         Gbl.XMLPtr += EndTagNameLength;
         Gbl.XMLPtr++;
         /*
         <parent>  element content  </parent>
                                             ^
                                            Gbl.XMLPtr
         */
         /* Remove trailing spaces in content */
         for (Ptr = StartContent + ContentLength - 1;
              ContentLength;
              ContentLength--, Ptr--)
            if (!isspace ((int) *Ptr))
               break;

         /* Copy content */
         if (ContentLength)
           {
            if ((ParentElem->Content = malloc (ContentLength + 1)) == NULL)
               Err_NotEnoughMemoryExit ();
            strncpy (ParentElem->Content,StartContent,ContentLength);
            ParentElem->Content[ContentLength] = '\0';
            ParentElem->ContentLength = ContentLength;
           }

         return;
        }
      else if (*Gbl.XMLPtr == '!' ||
               *Gbl.XMLPtr == '?')	// Skip <!...> and <?...>
        {
         TagLength = strcspn (Gbl.XMLPtr,">");
         Gbl.XMLPtr += TagLength;
         if (*Gbl.XMLPtr == '>')
           Gbl.XMLPtr++;
        }
      else		// New start tag
        {
         /*
         <parent><child attribute1="value" attribute2="value">...</child>...</parent>
                  ^
                 Gbl.XMLPtr
         */
         /***** Allocate space for the child element *****/
         if ((ChildElem = calloc (1,sizeof (*ChildElem))) == NULL)
            Err_NotEnoughMemoryExit ();

         /***** Adjust XML elements pointers *****/
         if (ParentElem->FirstChild)	// This child is a brother of a former child
            ParentElem->LastChild->NextBrother = ChildElem;
         else				// This child is the first child
            ParentElem->FirstChild = ChildElem;
         ParentElem->LastChild = ChildElem;

         /***** Get child tag name *****/
         ChildElem->TagNameLength = strcspn (Gbl.XMLPtr,">/ \t");
         if ((ChildElem->TagName = malloc (ChildElem->TagNameLength + 1)) == NULL)
            Err_NotEnoughMemoryExit ();
         strncpy (ChildElem->TagName,Gbl.XMLPtr,ChildElem->TagNameLength);
         ChildElem->TagName[ChildElem->TagNameLength] = '\0';
         Gbl.XMLPtr += ChildElem->TagNameLength;

         /*
         <parent><child attribute1="value" attribute2="value">...</child>...</parent>
                       ^
                      Gbl.XMLPtr
         */
         /* Check if end of start tag found */
         if (*Gbl.XMLPtr == '>')	// End of start tag
           {
            Gbl.XMLPtr++;
            /*
            <parent><child>...</child>...</parent>
                           ^
                          Gbl.XMLPtr
            */
            XML_GetElement (ChildElem);
           }
         else if (*Gbl.XMLPtr == '/')	// Unary tag?
           {
            Gbl.XMLPtr++;
            /*
            <parent><child/>...</parent>
                           ^
                          Gbl.XMLPtr
            */
            if (*Gbl.XMLPtr != '>')	// Here it should be the end of start tag
               Err_ShowErrorAndExit ("XML syntax error. Expect &gt; ending unary tag.");
            Gbl.XMLPtr++;
            /*
            <parent><child/>...</parent>
                            ^
                           Gbl.XMLPtr
            */
           }
         else	// Begin of an attribute
           {
            /*
            <parent><child attribute1="value" attribute2="value">...</child>...</parent>
                          ^
                         Gbl.XMLPtr
            */
            XML_GetAttributes (ChildElem);
            /*
            <parent><child attribute1="value" attribute2="value">...</child>...</parent>
                                                                 ^
                                                                Gbl.XMLPtr
            */
            XML_GetElement (ChildElem);
           }
        }

      /* Skip spaces after the ">" character of the tag */
      XML_SkipSpaces ();
     }
  }

/*****************************************************************************/
/******************* Get the attributes of a start tag ***********************/
/*****************************************************************************/

static void XML_GetAttributes (struct XMLElement *Elem)
  {
   struct XMLAttribute *Attribute;
   bool EndOfStartTag = false;
   char ErrorTxt[256];

   /*
   <parent><child attribute1="value" attribute2="value">...</child>...</parent>
                 ^
                 |
                Gbl.XMLPtr
   */
   do
     {
      /* Skip spaces */
      XML_SkipSpaces ();

      if (*Gbl.XMLPtr == '/')	// End of unary tag?
        {
         Gbl.XMLPtr++;
         if (*Gbl.XMLPtr == '>')
           {
            Gbl.XMLPtr++;
            EndOfStartTag = true;
           }
         else
            Err_ShowErrorAndExit ("XML syntax error. Expect &gt; ending unary tag with attributes.");
        }
      else if (*Gbl.XMLPtr == '>')	// End of start tag?
        {
         Gbl.XMLPtr++;
         EndOfStartTag = true;
        }
      else if (*Gbl.XMLPtr == '\0')
         Err_ShowErrorAndExit ("XML syntax error. Unexpected end of file.");
      else
        {
         /* Begin of attribute name:
         <parent><child attribute1="value" attribute2="value">...</child>...</parent>
                        ^
                        |
                       Gbl.XMLPtr
         */
         /***** Allocate space for the attribute *****/
         if ((Attribute = calloc (1,sizeof (*Attribute))) == NULL)
            Err_NotEnoughMemoryExit ();

         /***** Adjust XML element and attribute pointers *****/
         if (Elem->FirstAttribute)	// This attribute is a brother of a former attribute in current element
            Elem->LastAttribute->Next = Attribute;
         else				// This child is the first child
            Elem->FirstAttribute = Attribute;
         Elem->LastAttribute = Attribute;

         /***** Get attribute name *****/
         Attribute->AttributeNameLength = strcspn (Gbl.XMLPtr,"=");
         if ((Attribute->AttributeName = malloc (Attribute->AttributeNameLength + 1)) == NULL)
            Err_NotEnoughMemoryExit ();
         strncpy (Attribute->AttributeName,Gbl.XMLPtr,Attribute->AttributeNameLength);
         Attribute->AttributeName[Attribute->AttributeNameLength] = '\0';
         Gbl.XMLPtr += Attribute->AttributeNameLength;
         /* End of attribute name:
         <parent><child attribute1="value" attribute2="value">...</child>...</parent>
                                  ^
                                  |
                                 Gbl.XMLPtr
         */
         /***** Get attribute content *****/
         Gbl.XMLPtr++;
         if (*Gbl.XMLPtr == '\"')
           {
            Gbl.XMLPtr++;
            Attribute->ContentLength = strcspn (Gbl.XMLPtr,"\"");
           }
         else if (*Gbl.XMLPtr == '\'')
           {
            Gbl.XMLPtr++;
            Attribute->ContentLength = strcspn (Gbl.XMLPtr,"'");
           }
         else
           {
            snprintf (ErrorTxt,sizeof (ErrorTxt),
	              "XML syntax error after attribute &quot;%s&quot;"
	              " inside element &quot;%s&quot;.",
                      Attribute->AttributeName,Elem->TagName);
            Err_ShowErrorAndExit (ErrorTxt);
           }

         if ((Attribute->Content = malloc (Attribute->ContentLength + 1)) == NULL)
            Err_NotEnoughMemoryExit ();
         strncpy (Attribute->Content,Gbl.XMLPtr,Attribute->ContentLength);
         Attribute->Content[Attribute->ContentLength] = '\0';
         Gbl.XMLPtr += Attribute->ContentLength;

         Gbl.XMLPtr++;
         /* End of attribute content
         <parent><child attribute1="value" attribute2="value">...</child>...</parent>
                                          ^
                                          |
                                         Gbl.XMLPtr
         */
        }
     }
   while (!EndOfStartTag);
  }

/*****************************************************************************/
/****************** Skip spaces while parsing XML buffer *********************/
/*****************************************************************************/

static void XML_SkipSpaces (void)
  {
   while (isspace ((int) *Gbl.XMLPtr))
      Gbl.XMLPtr++;
  }

/*****************************************************************************/
/**************************** Print an XML element ***************************/
/*****************************************************************************/

void XML_PrintTree (struct XMLElement *ParentElem)
  {
   static int Level = -1;
   struct XMLElement *ChildElem;
   struct XMLElement *NextBrother;
   struct XMLAttribute *Attribute;
   int i;

   Level++;

   /***** Print start tag *****/
   if (Level > 0)
     {
      for (i = 1;
	   i < Level;
	   i++)
         HTM_Txt ("   ");
      HTM_TxtF ("&lt;%s",ParentElem->TagName ? ParentElem->TagName :
        	                               "");

      /* Print attributes */
      for (Attribute = ParentElem->FirstAttribute;
	   Attribute != NULL;
	   Attribute = Attribute->Next)
         HTM_TxtF (" %s=&quot;%s&quot;",
                   Attribute->AttributeName,
                   Attribute->Content);

      HTM_Txt ("&gt;\n");

      /***** Print content *****/
      if (ParentElem->Content)
        {
         for (i = 1;
              i < Level;
              i++)
            HTM_Txt ("   ");
         HTM_TxtF ("%s\n",ParentElem->Content);
        }
     }

   /***** Print children *****/
   ChildElem = ParentElem->FirstChild;
   while (ChildElem)
     {
      NextBrother = ChildElem->NextBrother;
      XML_PrintTree (ChildElem);
      ChildElem = NextBrother;
     }

   /***** Print end tag *****/
   if (Level > 0)
     {
      for (i = 1;
	   i < Level;
	   i++)
         HTM_Txt ("   ");
      HTM_TxtF ("&lt;/%s&gt;\n",ParentElem->TagName ? ParentElem->TagName :
        	                                      "");
     }

   Level--;
  }

/*****************************************************************************/
/********** Get attribute "yes"/"no" from an XML element in a tree ***********/
/*****************************************************************************/

bool XML_GetAttributteYesNoFromXMLTree (struct XMLAttribute *Attribute)
  {
   if (!Attribute->Content)
      Err_ShowErrorAndExit ("XML attribute yes/no not found.");
   if (!strcasecmp (Attribute->Content,"yes") ||
       !strcasecmp (Attribute->Content,"y"))	// case insensitive, because users can edit XML
      return true;
   if (!strcasecmp (Attribute->Content,"no") ||
       !strcasecmp (Attribute->Content,"n"))	// case insensitive, because users can edit XML
      return false;
   Err_ShowErrorAndExit ("XML attribute yes/no not found.");
   return false;	// Not reached
  }

/*****************************************************************************/
/************ Free the memory allocated for an XML element *******************/
/*****************************************************************************/

void XML_FreeTree (struct XMLElement *ParentElem)
  {
   struct XMLElement *ChildElem;
   struct XMLElement *NextElemBrother;
   struct XMLAttribute *Attribute;
   struct XMLAttribute *NextAttrib;

   /***** Free memory allocated for children *****/
   ChildElem = ParentElem->FirstChild;
   while (ChildElem)
     {
      NextElemBrother = ChildElem->NextBrother;
      XML_FreeTree (ChildElem);
      ChildElem = NextElemBrother;
     }

   /***** Free memory allocated for TagName *****/
   if (ParentElem->TagName)
     {
      free (ParentElem->TagName);
      ParentElem->TagName = NULL;
      ParentElem->TagNameLength = (size_t) 0;
     }

   /***** Free memory allocated for attributes *****/
   Attribute = ParentElem->FirstAttribute;
   while (Attribute)
     {
      NextAttrib = Attribute->Next;
      free (Attribute->AttributeName);
      free (Attribute->Content);
      free (Attribute);
      Attribute = NextAttrib;
     }

   /***** Free memory allocated for Content *****/
   if (ParentElem->Content)
     {
      free (ParentElem->Content);
      ParentElem->Content = NULL;
      ParentElem->ContentLength = (size_t) 0;
     }
  }
