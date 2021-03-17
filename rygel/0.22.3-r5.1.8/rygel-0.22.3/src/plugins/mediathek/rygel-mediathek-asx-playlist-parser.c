/* rygel-mediathek-asx-playlist-parser.c generated by valac 0.24.0.112-bbf5, the Vala compiler
 * generated from rygel-mediathek-asx-playlist-parser.vala, do not modify */

/*
 * Copyright (C) 2009-2011 Jens Georg
 *
 * Author: Jens Georg <mail@jensge.org>
 *
 * This file is part of Rygel.
 *
 * Rygel is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Rygel is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include <glib.h>
#include <glib-object.h>
#include <stdlib.h>
#include <string.h>
#include <gee.h>
#include <libsoup/soup.h>
#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libgupnp/gupnp.h>
#include <libxml/xpath.h>


#define RYGEL_MEDIATHEK_TYPE_PLAYLIST_PARSER (rygel_mediathek_playlist_parser_get_type ())
#define RYGEL_MEDIATHEK_PLAYLIST_PARSER(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), RYGEL_MEDIATHEK_TYPE_PLAYLIST_PARSER, RygelMediathekPlaylistParser))
#define RYGEL_MEDIATHEK_PLAYLIST_PARSER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), RYGEL_MEDIATHEK_TYPE_PLAYLIST_PARSER, RygelMediathekPlaylistParserClass))
#define RYGEL_MEDIATHEK_IS_PLAYLIST_PARSER(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), RYGEL_MEDIATHEK_TYPE_PLAYLIST_PARSER))
#define RYGEL_MEDIATHEK_IS_PLAYLIST_PARSER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), RYGEL_MEDIATHEK_TYPE_PLAYLIST_PARSER))
#define RYGEL_MEDIATHEK_PLAYLIST_PARSER_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), RYGEL_MEDIATHEK_TYPE_PLAYLIST_PARSER, RygelMediathekPlaylistParserClass))

typedef struct _RygelMediathekPlaylistParser RygelMediathekPlaylistParser;
typedef struct _RygelMediathekPlaylistParserClass RygelMediathekPlaylistParserClass;
typedef struct _RygelMediathekPlaylistParserPrivate RygelMediathekPlaylistParserPrivate;

#define RYGEL_MEDIATHEK_TYPE_ASX_PLAYLIST_PARSER (rygel_mediathek_asx_playlist_parser_get_type ())
#define RYGEL_MEDIATHEK_ASX_PLAYLIST_PARSER(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), RYGEL_MEDIATHEK_TYPE_ASX_PLAYLIST_PARSER, RygelMediathekAsxPlaylistParser))
#define RYGEL_MEDIATHEK_ASX_PLAYLIST_PARSER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), RYGEL_MEDIATHEK_TYPE_ASX_PLAYLIST_PARSER, RygelMediathekAsxPlaylistParserClass))
#define RYGEL_MEDIATHEK_IS_ASX_PLAYLIST_PARSER(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), RYGEL_MEDIATHEK_TYPE_ASX_PLAYLIST_PARSER))
#define RYGEL_MEDIATHEK_IS_ASX_PLAYLIST_PARSER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), RYGEL_MEDIATHEK_TYPE_ASX_PLAYLIST_PARSER))
#define RYGEL_MEDIATHEK_ASX_PLAYLIST_PARSER_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), RYGEL_MEDIATHEK_TYPE_ASX_PLAYLIST_PARSER, RygelMediathekAsxPlaylistParserClass))

typedef struct _RygelMediathekAsxPlaylistParser RygelMediathekAsxPlaylistParser;
typedef struct _RygelMediathekAsxPlaylistParserClass RygelMediathekAsxPlaylistParserClass;
typedef struct _RygelMediathekAsxPlaylistParserPrivate RygelMediathekAsxPlaylistParserPrivate;
#define _g_regex_unref0(var) ((var == NULL) ? NULL : (var = (g_regex_unref (var), NULL)))
#define _g_error_free0(var) ((var == NULL) ? NULL : (var = (g_error_free (var), NULL)))
#define _g_object_unref0(var) ((var == NULL) ? NULL : (var = (g_object_unref (var), NULL)))
#define _g_free0(var) (var = (g_free (var), NULL))
#define _xmlXPathFreeContext0(var) ((var == NULL) ? NULL : (var = (xmlXPathFreeContext (var), NULL)))

typedef enum  {
	RYGEL_MEDIATHEK_VIDEO_ITEM_ERROR_XML_PARSE_ERROR,
	RYGEL_MEDIATHEK_VIDEO_ITEM_ERROR_NETWORK_ERROR
} RygelMediathekVideoItemError;
#define RYGEL_MEDIATHEK_VIDEO_ITEM_ERROR rygel_mediathek_video_item_error_quark ()
struct _RygelMediathekPlaylistParser {
	GObject parent_instance;
	RygelMediathekPlaylistParserPrivate * priv;
};

struct _RygelMediathekPlaylistParserClass {
	GObjectClass parent_class;
	GeeList* (*parse_playlist) (RygelMediathekPlaylistParser* self, const gchar* data, gint length, GError** error);
};

struct _RygelMediathekAsxPlaylistParser {
	RygelMediathekPlaylistParser parent_instance;
	RygelMediathekAsxPlaylistParserPrivate * priv;
};

struct _RygelMediathekAsxPlaylistParserClass {
	RygelMediathekPlaylistParserClass parent_class;
};

struct _RygelMediathekAsxPlaylistParserPrivate {
	GRegex* normalizer;
};


static gpointer rygel_mediathek_asx_playlist_parser_parent_class = NULL;

GType rygel_mediathek_playlist_parser_get_type (void) G_GNUC_CONST;
GQuark rygel_mediathek_video_item_error_quark (void);
GType rygel_mediathek_asx_playlist_parser_get_type (void) G_GNUC_CONST;
#define RYGEL_MEDIATHEK_ASX_PLAYLIST_PARSER_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), RYGEL_MEDIATHEK_TYPE_ASX_PLAYLIST_PARSER, RygelMediathekAsxPlaylistParserPrivate))
enum  {
	RYGEL_MEDIATHEK_ASX_PLAYLIST_PARSER_DUMMY_PROPERTY
};
RygelMediathekAsxPlaylistParser* rygel_mediathek_asx_playlist_parser_new (SoupSession* session);
RygelMediathekAsxPlaylistParser* rygel_mediathek_asx_playlist_parser_construct (GType object_type, SoupSession* session);
static GeeList* rygel_mediathek_asx_playlist_parser_real_parse_playlist (RygelMediathekPlaylistParser* base, const gchar* data, gint length, GError** error);
void xmlXPathFreeObject (xmlXPathObject* object);
static void rygel_mediathek_asx_playlist_parser_finalize (GObject* obj);


RygelMediathekAsxPlaylistParser* rygel_mediathek_asx_playlist_parser_construct (GType object_type, SoupSession* session) {
	RygelMediathekAsxPlaylistParser * self = NULL;
	SoupSession* _tmp0_ = NULL;
	GError * _inner_error_ = NULL;
	g_return_val_if_fail (session != NULL, NULL);
	_tmp0_ = session;
	self = (RygelMediathekAsxPlaylistParser*) g_object_new (object_type, "session", _tmp0_, "playlist-suffix", ".asx", "mime-type", "video/x-ms-wmv", NULL);
	{
		GRegex* _tmp1_ = NULL;
		GRegex* _tmp2_ = NULL;
		GRegex* _tmp3_ = NULL;
		_tmp2_ = g_regex_new ("(<[/]?)([a-zA-Z:]+)", 0, 0, &_inner_error_);
		_tmp1_ = _tmp2_;
		if (G_UNLIKELY (_inner_error_ != NULL)) {
			if (_inner_error_->domain == G_REGEX_ERROR) {
				goto __catch0_g_regex_error;
			}
			g_critical ("file %s: line %d: unexpected error: %s (%s, %d)", __FILE__, __LINE__, _inner_error_->message, g_quark_to_string (_inner_error_->domain), _inner_error_->code);
			g_clear_error (&_inner_error_);
			return NULL;
		}
		_tmp3_ = _tmp1_;
		_tmp1_ = NULL;
		_g_regex_unref0 (self->priv->normalizer);
		self->priv->normalizer = _tmp3_;
		_g_regex_unref0 (_tmp1_);
	}
	goto __finally0;
	__catch0_g_regex_error:
	{
		GError* _error_ = NULL;
		_error_ = _inner_error_;
		_inner_error_ = NULL;
		_g_error_free0 (_error_);
	}
	__finally0:
	if (G_UNLIKELY (_inner_error_ != NULL)) {
		g_critical ("file %s: line %d: uncaught error: %s (%s, %d)", __FILE__, __LINE__, _inner_error_->message, g_quark_to_string (_inner_error_->domain), _inner_error_->code);
		g_clear_error (&_inner_error_);
		return NULL;
	}
	return self;
}


RygelMediathekAsxPlaylistParser* rygel_mediathek_asx_playlist_parser_new (SoupSession* session) {
	return rygel_mediathek_asx_playlist_parser_construct (RYGEL_MEDIATHEK_TYPE_ASX_PLAYLIST_PARSER, session);
}


/**
     * Get and parse the ASX file.
     *
     * This will fetch the ASX file using the soup session configured on
     * configure time.As ASX seems to be a bit inconsistent with regard to tag
     * case, all the tags are converted to lowercase. A XPath query is then used
     * to extract all of the href attributes for every entry in the file.
     *
     * @param uri network location of the ASX file
     * @return a list of uris found in this file
     */
static GeeList* rygel_mediathek_asx_playlist_parser_real_parse_playlist (RygelMediathekPlaylistParser* base, const gchar* data, gint length, GError** error) {
	RygelMediathekAsxPlaylistParser * self;
	GeeList* result = NULL;
	GeeArrayList* uris = NULL;
	GError * _inner_error_ = NULL;
	self = (RygelMediathekAsxPlaylistParser*) base;
	g_return_val_if_fail (data != NULL, NULL);
	uris = NULL;
	{
		gchar* normalized_content = NULL;
		GRegex* _tmp0_ = NULL;
		const gchar* _tmp1_ = NULL;
		gint _tmp2_ = 0;
		gchar* _tmp3_ = NULL;
		xmlDoc* doc = NULL;
		const gchar* _tmp4_ = NULL;
		const gchar* _tmp5_ = NULL;
		gint _tmp6_ = 0;
		gint _tmp7_ = 0;
		xmlDoc* _tmp8_ = NULL;
		xmlDoc* _tmp9_ = NULL;
		GUPnPXMLDoc* doc_guard = NULL;
		xmlDoc* _tmp11_ = NULL;
		GUPnPXMLDoc* _tmp12_ = NULL;
		xmlXPathContext* context = NULL;
		GUPnPXMLDoc* _tmp13_ = NULL;
		xmlDoc* _tmp14_ = NULL;
		xmlXPathContext* _tmp15_ = NULL;
		xmlXPathObject* xpath_object = NULL;
		xmlXPathContext* _tmp16_ = NULL;
		xmlXPathObject* _tmp17_ = NULL;
		xmlXPathObject* _tmp18_ = NULL;
		xmlXPathObjectType _tmp19_ = 0;
		GeeArrayList* _tmp22_ = NULL;
		xmlXPathObject* _tmp37_ = NULL;
		_tmp0_ = self->priv->normalizer;
		_tmp1_ = data;
		_tmp2_ = length;
		_tmp3_ = g_regex_replace (_tmp0_, _tmp1_, (gssize) _tmp2_, 0, "\\1\\L\\2\\E", 0, &_inner_error_);
		normalized_content = _tmp3_;
		if (G_UNLIKELY (_inner_error_ != NULL)) {
			if (_inner_error_->domain == G_REGEX_ERROR) {
				goto __catch1_g_regex_error;
			}
			_g_object_unref0 (uris);
			g_critical ("file %s: line %d: unexpected error: %s (%s, %d)", __FILE__, __LINE__, _inner_error_->message, g_quark_to_string (_inner_error_->domain), _inner_error_->code);
			g_clear_error (&_inner_error_);
			return NULL;
		}
		_tmp4_ = normalized_content;
		_tmp5_ = normalized_content;
		_tmp6_ = strlen (_tmp5_);
		_tmp7_ = _tmp6_;
		_tmp8_ = xmlParseMemory (_tmp4_, (gint) _tmp7_);
		doc = _tmp8_;
		_tmp9_ = doc;
		if (_tmp9_ == NULL) {
			GError* _tmp10_ = NULL;
			_tmp10_ = g_error_new_literal (RYGEL_MEDIATHEK_VIDEO_ITEM_ERROR, RYGEL_MEDIATHEK_VIDEO_ITEM_ERROR_XML_PARSE_ERROR, "Could not parse playlist");
			_inner_error_ = _tmp10_;
			_g_free0 (normalized_content);
			if (_inner_error_->domain == G_REGEX_ERROR) {
				goto __catch1_g_regex_error;
			}
			goto __finally1;
		}
		_tmp11_ = doc;
		_tmp12_ = gupnp_xml_doc_new (_tmp11_);
		doc_guard = _tmp12_;
		_tmp13_ = doc_guard;
		_tmp14_ = _tmp13_->doc;
		_tmp15_ = xmlXPathNewContext (_tmp14_);
		context = _tmp15_;
		_tmp16_ = context;
		_tmp17_ = xmlXPathEval ((xmlChar*) "/asx/entry/ref/@href", _tmp16_);
		xpath_object = _tmp17_;
		_tmp18_ = xpath_object;
		_tmp19_ = _tmp18_->type;
		if (_tmp19_ != XPATH_NODESET) {
			xmlXPathObject* _tmp20_ = NULL;
			GError* _tmp21_ = NULL;
			_tmp20_ = xpath_object;
			xmlXPathFreeObject (_tmp20_);
			_tmp21_ = g_error_new_literal (RYGEL_MEDIATHEK_VIDEO_ITEM_ERROR, RYGEL_MEDIATHEK_VIDEO_ITEM_ERROR_XML_PARSE_ERROR, "No uris found in playlist");
			_inner_error_ = _tmp21_;
			_xmlXPathFreeContext0 (context);
			_g_object_unref0 (doc_guard);
			_g_free0 (normalized_content);
			if (_inner_error_->domain == G_REGEX_ERROR) {
				goto __catch1_g_regex_error;
			}
			goto __finally1;
		}
		_tmp22_ = gee_array_list_new (G_TYPE_STRING, (GBoxedCopyFunc) g_strdup, g_free, NULL, NULL, NULL);
		_g_object_unref0 (uris);
		uris = _tmp22_;
		{
			gint i = 0;
			i = 0;
			{
				gboolean _tmp23_ = FALSE;
				_tmp23_ = TRUE;
				while (TRUE) {
					gint _tmp25_ = 0;
					xmlXPathObject* _tmp26_ = NULL;
					xmlNodeSet* _tmp27_ = NULL;
					gint _tmp28_ = 0;
					xmlNode* item = NULL;
					xmlXPathObject* _tmp29_ = NULL;
					xmlNodeSet* _tmp30_ = NULL;
					gint _tmp31_ = 0;
					xmlNode* _tmp32_ = NULL;
					GeeArrayList* _tmp33_ = NULL;
					xmlNode* _tmp34_ = NULL;
					xmlNode* _tmp35_ = NULL;
					const gchar* _tmp36_ = NULL;
					if (!_tmp23_) {
						gint _tmp24_ = 0;
						_tmp24_ = i;
						i = _tmp24_ + 1;
					}
					_tmp23_ = FALSE;
					_tmp25_ = i;
					_tmp26_ = xpath_object;
					_tmp27_ = _tmp26_->nodesetval;
					_tmp28_ = xmlXPathNodeSetGetLength (_tmp27_);
					if (!(_tmp25_ < _tmp28_)) {
						break;
					}
					_tmp29_ = xpath_object;
					_tmp30_ = _tmp29_->nodesetval;
					_tmp31_ = i;
					_tmp32_ = xmlXPathNodeSetItem (_tmp30_, _tmp31_);
					item = _tmp32_;
					_tmp33_ = uris;
					_tmp34_ = item;
					_tmp35_ = _tmp34_->children;
					_tmp36_ = _tmp35_->content;
					gee_abstract_collection_add ((GeeAbstractCollection*) _tmp33_, _tmp36_);
				}
			}
		}
		_tmp37_ = xpath_object;
		xmlXPathFreeObject (_tmp37_);
		_xmlXPathFreeContext0 (context);
		_g_object_unref0 (doc_guard);
		_g_free0 (normalized_content);
	}
	goto __finally1;
	__catch1_g_regex_error:
	{
		GError* _error_ = NULL;
		GError* _tmp38_ = NULL;
		_error_ = _inner_error_;
		_inner_error_ = NULL;
		_tmp38_ = g_error_new_literal (RYGEL_MEDIATHEK_VIDEO_ITEM_ERROR, RYGEL_MEDIATHEK_VIDEO_ITEM_ERROR_XML_PARSE_ERROR, "Failed to normalize");
		_inner_error_ = _tmp38_;
		_g_error_free0 (_error_);
		goto __finally1;
	}
	__finally1:
	if (G_UNLIKELY (_inner_error_ != NULL)) {
		if (_inner_error_->domain == RYGEL_MEDIATHEK_VIDEO_ITEM_ERROR) {
			g_propagate_error (error, _inner_error_);
			_g_object_unref0 (uris);
			return NULL;
		} else {
			_g_object_unref0 (uris);
			g_critical ("file %s: line %d: uncaught error: %s (%s, %d)", __FILE__, __LINE__, _inner_error_->message, g_quark_to_string (_inner_error_->domain), _inner_error_->code);
			g_clear_error (&_inner_error_);
			return NULL;
		}
	}
	result = (GeeList*) uris;
	return result;
}


static void rygel_mediathek_asx_playlist_parser_class_init (RygelMediathekAsxPlaylistParserClass * klass) {
	rygel_mediathek_asx_playlist_parser_parent_class = g_type_class_peek_parent (klass);
	g_type_class_add_private (klass, sizeof (RygelMediathekAsxPlaylistParserPrivate));
	RYGEL_MEDIATHEK_PLAYLIST_PARSER_CLASS (klass)->parse_playlist = rygel_mediathek_asx_playlist_parser_real_parse_playlist;
	G_OBJECT_CLASS (klass)->finalize = rygel_mediathek_asx_playlist_parser_finalize;
}


static void rygel_mediathek_asx_playlist_parser_instance_init (RygelMediathekAsxPlaylistParser * self) {
	self->priv = RYGEL_MEDIATHEK_ASX_PLAYLIST_PARSER_GET_PRIVATE (self);
}


static void rygel_mediathek_asx_playlist_parser_finalize (GObject* obj) {
	RygelMediathekAsxPlaylistParser * self;
	self = G_TYPE_CHECK_INSTANCE_CAST (obj, RYGEL_MEDIATHEK_TYPE_ASX_PLAYLIST_PARSER, RygelMediathekAsxPlaylistParser);
	_g_regex_unref0 (self->priv->normalizer);
	G_OBJECT_CLASS (rygel_mediathek_asx_playlist_parser_parent_class)->finalize (obj);
}


/**
 * This class is a simple ASX playlist parser
 *
 * It does nothing but extracting all href tags from an ASX
 * and ignore all of the other information that may be in it
 *
 * This parser is //only// intended to work with the simple
 * ASX files presented by the ZDF Mediathek streaming server
 */
GType rygel_mediathek_asx_playlist_parser_get_type (void) {
	static volatile gsize rygel_mediathek_asx_playlist_parser_type_id__volatile = 0;
	if (g_once_init_enter (&rygel_mediathek_asx_playlist_parser_type_id__volatile)) {
		static const GTypeInfo g_define_type_info = { sizeof (RygelMediathekAsxPlaylistParserClass), (GBaseInitFunc) NULL, (GBaseFinalizeFunc) NULL, (GClassInitFunc) rygel_mediathek_asx_playlist_parser_class_init, (GClassFinalizeFunc) NULL, NULL, sizeof (RygelMediathekAsxPlaylistParser), 0, (GInstanceInitFunc) rygel_mediathek_asx_playlist_parser_instance_init, NULL };
		GType rygel_mediathek_asx_playlist_parser_type_id;
		rygel_mediathek_asx_playlist_parser_type_id = g_type_register_static (RYGEL_MEDIATHEK_TYPE_PLAYLIST_PARSER, "RygelMediathekAsxPlaylistParser", &g_define_type_info, 0);
		g_once_init_leave (&rygel_mediathek_asx_playlist_parser_type_id__volatile, rygel_mediathek_asx_playlist_parser_type_id);
	}
	return rygel_mediathek_asx_playlist_parser_type_id__volatile;
}


