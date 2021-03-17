/* 
   Copyright (C) 2002, 2003, 2004, 2005, 2006, 2007 Bastien Nocera
   Copyright (C) 2003, 2004 Colin Walters <walters@rhythmbox.org>

   The Gnome Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   The Gnome Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with the Gnome Library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301  USA.

   Author: Bastien Nocera <hadess@hadess.net>
 */

#include "config.h"

#include <string.h>
#include <glib.h>

#ifndef TOTEM_PL_PARSER_MINI
#include "xmlparser.h"

#include "totem-pl-parser.h"
#include "totem-disc.h"
#endif /* !TOTEM_PL_PARSER_MINI */

#include "totem-pl-parser-mini.h"
#include "totem-pl-parser-wm.h"
#include "totem-pl-parser-lines.h"
#include "totem-pl-parser-private.h"

#define ASX_NEEDLE "<ASX"
#define ASX_NEEDLE2 "<asx"
#define ASX_NEEDLE3 "<Asx"

const char *
totem_pl_parser_is_asx (const char *data, gsize len)
{
	if (len == 0)
		return NULL;

	if (len > MIME_READ_CHUNK_SIZE)
		len = MIME_READ_CHUNK_SIZE;

	if (g_strstr_len (data, len, ASX_NEEDLE) != NULL)
		return ASX_MIME_TYPE;
	if (g_strstr_len (data, len, ASX_NEEDLE2) != NULL)
		return ASX_MIME_TYPE;
	if (g_strstr_len (data, len, ASX_NEEDLE3) != NULL)
		return ASX_MIME_TYPE;

	return FALSE;
}

const char *
totem_pl_parser_is_asf (const char *data, gsize len)
{
	if (len == 0)
		return NULL;

	if (g_str_has_prefix (data, "[Reference]") != FALSE
			|| g_str_has_prefix (data, "ASF ") != FALSE
			|| g_str_has_prefix (data, "[Address]") != FALSE) {
		return ASF_REF_MIME_TYPE;
	}

	return totem_pl_parser_is_asx (data, len);
}

#ifndef TOTEM_PL_PARSER_MINI

static TotemPlParserResult
totem_pl_parser_add_asf_reference_parser (TotemPlParser *parser,
                                          GFile *file,
                                          GFile *base_file,
                                          TotemPlParseData *parse_data,
                                          gpointer data)
{
  char *contents, **lines, *ref, *ref2;
  gsize size;

  if (g_file_load_contents (file, NULL, &contents, &size, NULL, NULL) == FALSE)
    return TOTEM_PL_PARSER_RESULT_ERROR;

  lines = g_strsplit_set (contents, "\n\r", 0);
  g_free (contents);

  /* Try to get Ref1 first */
  ref = totem_pl_parser_read_ini_line_string (lines, "Ref1");
  if (ref == NULL) {
    g_strfreev (lines);
    return totem_pl_parser_add_asx (parser, file, base_file, parse_data, data);
  }

  /* change http to mmsh, thanks Microsoft */
  if (g_str_has_prefix (ref, "http") != FALSE)
    memcpy(ref, "mmsh", 4);

  /* Try to get Ref2 */
  ref2 = totem_pl_parser_read_ini_line_string (lines, "Ref2");

  if (ref2 != NULL) {
    /* change http to mmsh in Ref2 as well */
    if (g_str_has_prefix (ref2, "http") != FALSE) {
      memcpy(ref2, "mmsh", 4);
    }
    /* If we found the Ref2 add it */
    totem_pl_parser_add_uri (parser,
                             TOTEM_PL_PARSER_FIELD_URI, ref,
                             TOTEM_PL_PARSER_FIELD_ALTERNATIVE_URI1, ref2,
                             NULL);
    g_free (ref2);
  }
  else {
    /* Didn't find the Ref2, no worries, just add the Ref1 */
    totem_pl_parser_add_one_uri (parser, ref, NULL);
  }

  /* We could do the same for Ref3 but that does not seem to be that common */
  g_free (ref);
  g_strfreev (lines);

  return TOTEM_PL_PARSER_RESULT_SUCCESS;
}

static TotemPlParserResult
totem_pl_parser_add_asf_parser (TotemPlParser *parser,
				GFile *file,
				GFile *base_file,
				TotemPlParseData *parse_data,
				gpointer data)
{
	TotemPlParserResult retval = TOTEM_PL_PARSER_RESULT_UNHANDLED;
	char *contents, *ref;
	gsize size;

	/* NSC files are handled directly by GStreamer */
	if (g_str_has_prefix (data, "[Address]") != FALSE)
		return TOTEM_PL_PARSER_RESULT_UNHANDLED;

	if (g_str_has_prefix (data, "ASF ") == FALSE) {
		return totem_pl_parser_add_asf_reference_parser (parser, file, base_file, parse_data, data);
	}

	if (g_file_load_contents (file, NULL, &contents, &size, NULL, NULL) == FALSE)
		return TOTEM_PL_PARSER_RESULT_ERROR;

	if (size <= 4) {
		g_free (contents);
		return TOTEM_PL_PARSER_RESULT_ERROR;
	}

	/* Skip 'ASF ' */
	ref = contents + 4;
	if (g_str_has_prefix (ref, "http") != FALSE) {
		memcpy(ref, "mmsh", 4);
		totem_pl_parser_add_one_uri (parser, ref, NULL);
		retval = TOTEM_PL_PARSER_RESULT_SUCCESS;
	}

	g_free (contents);

	return retval;
}

static gboolean
parse_asx_entry (TotemPlParser *parser, GFile *base_file, xml_node_t *parent, TotemPlParseData *parse_data)
{
  xml_node_t *node;
  TotemPlParserResult retval = TOTEM_PL_PARSER_RESULT_SUCCESS;
  GFile *resolved;
  char *resolved_uri, *resolved_uri2, *resolved_uri3, *resolved_uri4;
  const char *uri, *alternativeURI1, *alternativeURI2, *alternativeURI3;
  const char *title, *duration, *starttime, *author;
  const char *moreinfo, *abstract, *copyright;

  title = NULL;
  uri = NULL;
  alternativeURI1 = NULL;
  alternativeURI2 = NULL;
  alternativeURI3 = NULL;
  duration = NULL;
  starttime = NULL;
  moreinfo = NULL;
  abstract = NULL;
  copyright = NULL;
  author = NULL;

  resolved_uri2 = NULL;
  resolved_uri3 = NULL;
  resolved_uri4 = NULL;

  for (node = parent->child; node != NULL; node = node->next) {
    if (node->name == NULL)
      continue;

    /* ENTRY can only have one title node but multiple REFs */
    if (g_ascii_strcasecmp (node->name, "ref") == 0) {
      const char *tmp;

      tmp = xml_parser_get_property (node, "href");
      if (tmp == NULL)
        continue;

      /* FIXME, should we prefer mms streams, or non-mms?
       * See bug #352559 */
      if (uri == NULL) {
        uri = tmp;
      }
      /* Primary URI already found */
      else {
        /* If the 1st alternative not yet found, store it */
        if (alternativeURI1 == NULL) {
          alternativeURI1 = tmp;
        }
        /* 1st alternative URI also already found */
        else {
          /* If the 2nd alternative not yet found, store it */
          if (alternativeURI2 == NULL) {
            alternativeURI2 = tmp;
          }
          else {
            /* If the 3rd alternative not yet found, store it */
            if (alternativeURI3 == NULL) {
              alternativeURI3 = tmp;
            }
          }
          /* Could also parse more alternatives here... */
          /* But haven't seen more that 3 alternatives yet */
        }
      }

      continue;
    }

    if (g_ascii_strcasecmp (node->name, "title") == 0)
      title = node->data;

    if (g_ascii_strcasecmp (node->name, "author") == 0)
      author = node->data;

    if (g_ascii_strcasecmp (node->name, "moreinfo") == 0) {
      const char *tmp;

      tmp = xml_parser_get_property (node, "href");
      if (tmp == NULL)
        continue;
      moreinfo = tmp;
    }

    if (g_ascii_strcasecmp (node->name, "copyright") == 0)
      copyright = node->data;

    if (g_ascii_strcasecmp (node->name, "abstract") == 0)
      abstract = node->data;

    if (g_ascii_strcasecmp (node->name, "duration") == 0) {
      const char *tmp;

      tmp = xml_parser_get_property (node, "value");
      if (tmp == NULL)
        continue;
      duration = tmp;
    }

    if (g_ascii_strcasecmp (node->name, "starttime") == 0) {
      const char *tmp;

      tmp = xml_parser_get_property (node, "value");
      if (tmp == NULL)
        continue;
      starttime = tmp;
    }

    if (g_ascii_strcasecmp (node->name, "param") == 0) {
      const char *name, *value;

      name = xml_parser_get_property (node, "name");
      if (name == NULL || g_ascii_strcasecmp (name, "showwhilebuffering") != 0)
        continue;
      value = xml_parser_get_property (node, "value");
      if (value == NULL || g_ascii_strcasecmp (value, "true") != 0)
        continue;

      /* We ignore items that are the buffering images */
      retval = TOTEM_PL_PARSER_RESULT_IGNORED;
      goto bail;
    }
  }

  if (uri == NULL)
    return TOTEM_PL_PARSER_RESULT_ERROR;

  resolved_uri = totem_pl_parser_resolve_uri (base_file, uri);
  resolved = g_file_new_for_uri (resolved_uri);
  g_free (resolved_uri);

  /* Resolve the 1st alternative URI */
  if (alternativeURI1 != NULL)
  {
    resolved_uri2 = totem_pl_parser_resolve_uri (base_file, alternativeURI1);
  }
  /* Resolve the 2nd alternative URI */
  if (alternativeURI2 != NULL)
  {
    resolved_uri3 = totem_pl_parser_resolve_uri (base_file, alternativeURI2);
  }
  /* Resolve the 3rd alternative URI */
  if (alternativeURI3 != NULL)
  {
    resolved_uri4 = totem_pl_parser_resolve_uri (base_file, alternativeURI3);
  }

  /* .asx files can contain references to other .asx files */
  retval = totem_pl_parser_parse_internal (parser, resolved, NULL, parse_data);
  if (retval != TOTEM_PL_PARSER_RESULT_SUCCESS) {
    totem_pl_parser_add_uri (parser,
                             TOTEM_PL_PARSER_FIELD_FILE, resolved,
                             TOTEM_PL_PARSER_FIELD_ALTERNATIVE_URI1, resolved_uri2,
                             TOTEM_PL_PARSER_FIELD_ALTERNATIVE_URI2, resolved_uri3,
                             TOTEM_PL_PARSER_FIELD_ALTERNATIVE_URI3, resolved_uri4,
                             TOTEM_PL_PARSER_FIELD_TITLE, title,
                             TOTEM_PL_PARSER_FIELD_ABSTRACT, abstract,
                             TOTEM_PL_PARSER_FIELD_COPYRIGHT, copyright,
                             TOTEM_PL_PARSER_FIELD_AUTHOR, author,
                             TOTEM_PL_PARSER_FIELD_STARTTIME, starttime,
                             TOTEM_PL_PARSER_FIELD_DURATION, duration,
                             TOTEM_PL_PARSER_FIELD_MOREINFO, moreinfo,
                             NULL);
    retval = TOTEM_PL_PARSER_RESULT_SUCCESS;
  }
  g_object_unref (resolved);

  /* if NULL g_free will just return */
  g_free (resolved_uri2);
  g_free (resolved_uri3);
  g_free (resolved_uri4);

bail:
  return retval;
}

static gboolean
parse_asx_entryref (TotemPlParser *parser, GFile *base_file, xml_node_t *node, TotemPlParseData *parse_data)
{
	TotemPlParserResult retval = TOTEM_PL_PARSER_RESULT_SUCCESS;
	const char *uri;
	GFile *resolved;
	char *resolved_uri;

	uri = xml_parser_get_property (node, "href");

	if (uri == NULL)
		return TOTEM_PL_PARSER_RESULT_ERROR;

	resolved_uri = totem_pl_parser_resolve_uri (base_file, uri);
	resolved = g_file_new_for_uri (resolved_uri);
	g_free (resolved_uri);

	/* .asx files can contain references to other .asx files */
	retval = totem_pl_parser_parse_internal (parser, resolved, NULL, parse_data);
	if (retval != TOTEM_PL_PARSER_RESULT_SUCCESS) {
		totem_pl_parser_add_uri (parser,
					 TOTEM_PL_PARSER_FIELD_FILE, resolved,
					 NULL);
		retval = TOTEM_PL_PARSER_RESULT_SUCCESS;
	}
	g_object_unref (resolved);

	return retval;
}

static gboolean
parse_asx_entries (TotemPlParser *parser, const char *uri, GFile *base_file, xml_node_t *parent, TotemPlParseData *parse_data)
{
	char *title = NULL;
	GFile *new_base;
	xml_node_t *node;
	TotemPlParserResult retval = TOTEM_PL_PARSER_RESULT_ERROR;

	new_base = NULL;

	/* Loop to look for playlist information first */
	for (node = parent->child; node != NULL; node = node->next) {
		if (node->name == NULL)
			continue;
		if (g_ascii_strcasecmp (node->name, "title") == 0) {
			g_free (title);
			title = g_strdup (node->data);
			totem_pl_parser_add_uri (parser,
						 TOTEM_PL_PARSER_FIELD_IS_PLAYLIST, TRUE,
						 TOTEM_PL_PARSER_FIELD_URI, uri,
						 TOTEM_PL_PARSER_FIELD_TITLE, title,
						 NULL);
		}
		if (g_ascii_strcasecmp (node->name, "base") == 0) {
			const char *str;
			str = xml_parser_get_property (node, "href");
			if (str != NULL) {
				if (new_base != NULL)
					g_object_unref (new_base);
				new_base = g_file_new_for_uri (str);
			}
		}
	}

	/* Restart for the entries now */
	for (node = parent->child; node != NULL; node = node->next) {
		if (node->name == NULL)
			continue;

		if (g_ascii_strcasecmp (node->name, "entry") == 0) {
			/* Whee! found an entry here, find the REF and TITLE */
			if (parse_asx_entry (parser, new_base ? new_base : base_file, node, parse_data) != FALSE)
				retval = TOTEM_PL_PARSER_RESULT_SUCCESS;
		}
		if (g_ascii_strcasecmp (node->name, "entryref") == 0) {
			/* Found an entryref, extract the REF attribute */
			if (parse_asx_entryref (parser, new_base ? new_base : base_file, node, parse_data) != FALSE)
				retval = TOTEM_PL_PARSER_RESULT_SUCCESS;
		}
		if (g_ascii_strcasecmp (node->name, "repeat") == 0) {
			/* Repeat at the top-level */
			if (parse_asx_entries (parser, uri, new_base ? new_base : base_file, node, parse_data) != FALSE)
				retval = TOTEM_PL_PARSER_RESULT_SUCCESS;
		}
	}

	if (new_base != NULL)
		g_object_unref (new_base);
	if (title != NULL)
		totem_pl_parser_playlist_end (parser, uri);
	g_free (title);

	return retval;
}

TotemPlParserResult
totem_pl_parser_add_asx (TotemPlParser *parser,
			 GFile *file,
			 GFile *base_file,
			 TotemPlParseData *parse_data,
			 gpointer data)
{
	xml_node_t* doc;
	char *contents, *uri;
	gsize size;
	TotemPlParserResult retval = TOTEM_PL_PARSER_RESULT_UNHANDLED;

	if (data != NULL && totem_pl_parser_is_uri_list (data, strlen (data)) != FALSE) {
		return totem_pl_parser_add_ram (parser, file, parse_data, data);
	}

	if (g_file_load_contents (file, NULL, &contents, &size, NULL, NULL) == FALSE)
		return TOTEM_PL_PARSER_RESULT_ERROR;

	doc = totem_pl_parser_parse_xml_relaxed (contents, size);
	if (doc == NULL) {
		g_free (contents);
		return TOTEM_PL_PARSER_RESULT_ERROR;
	}

	/* If the document has no name */
	if (doc->name == NULL
	    || g_ascii_strcasecmp (doc->name , "asx") != 0) {
		g_free (contents);
		xml_parser_free_tree (doc);
		return TOTEM_PL_PARSER_RESULT_ERROR;
	}

	uri = g_file_get_uri (file);

	if (parse_asx_entries (parser, uri, base_file, doc, parse_data) != FALSE)
		retval = TOTEM_PL_PARSER_RESULT_SUCCESS;

	g_free (uri);
	g_free (contents);
	xml_parser_free_tree (doc);

	return retval;
}

TotemPlParserResult
totem_pl_parser_add_asf (TotemPlParser *parser,
			 GFile *file,
			 GFile *base_file,
			 TotemPlParseData *parse_data,
			 gpointer data)
{
	if (data == NULL) {
		totem_pl_parser_add_one_file (parser, file, NULL);
		return TOTEM_PL_PARSER_RESULT_SUCCESS;
	}

	if (totem_pl_parser_is_asf (data, strlen (data)) == FALSE) {
		totem_pl_parser_add_one_file (parser, file, NULL);
		return TOTEM_PL_PARSER_RESULT_SUCCESS;
	}

	return totem_pl_parser_add_asf_parser (parser, file, base_file, parse_data, data);
}

#endif /* !TOTEM_PL_PARSER_MINI */

