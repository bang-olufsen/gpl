/* rygel-m3u-playlist.c generated by valac 0.24.0, the Vala compiler
 * generated from rygel-m3u-playlist.vala, do not modify */

/*
 * Copyright (C) 2013 Jens Georg.
 *
 * Authors: Jens Georg <mail@jensge.org>
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
#include <gee.h>
#include <libgupnp-av/gupnp-av.h>
#include <stdlib.h>
#include <string.h>
#include <glib/gi18n-lib.h>


#define RYGEL_TYPE_M3_UPLAY_LIST (rygel_m3_uplay_list_get_type ())
#define RYGEL_M3_UPLAY_LIST(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), RYGEL_TYPE_M3_UPLAY_LIST, RygelM3UPlayList))
#define RYGEL_M3_UPLAY_LIST_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), RYGEL_TYPE_M3_UPLAY_LIST, RygelM3UPlayListClass))
#define RYGEL_IS_M3_UPLAY_LIST(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), RYGEL_TYPE_M3_UPLAY_LIST))
#define RYGEL_IS_M3_UPLAY_LIST_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), RYGEL_TYPE_M3_UPLAY_LIST))
#define RYGEL_M3_UPLAY_LIST_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), RYGEL_TYPE_M3_UPLAY_LIST, RygelM3UPlayListClass))

typedef struct _RygelM3UPlayList RygelM3UPlayList;
typedef struct _RygelM3UPlayListClass RygelM3UPlayListClass;
typedef struct _RygelM3UPlayListPrivate RygelM3UPlayListPrivate;
#define _g_object_unref0(var) ((var == NULL) ? NULL : (var = (g_object_unref (var), NULL)))
#define __g_list_free__g_object_unref0_0(var) ((var == NULL) ? NULL : (var = (_g_list_free__g_object_unref0_ (var), NULL)))
#define _g_string_free0(var) ((var == NULL) ? NULL : (var = (g_string_free (var, TRUE), NULL)))

struct _RygelM3UPlayList {
	GObject parent_instance;
	RygelM3UPlayListPrivate * priv;
};

struct _RygelM3UPlayListClass {
	GObjectClass parent_class;
};

struct _RygelM3UPlayListPrivate {
	GeeLinkedList* items;
	GUPnPDIDLLiteWriter* writer;
};


static gpointer rygel_m3_uplay_list_parent_class = NULL;

GType rygel_m3_uplay_list_get_type (void) G_GNUC_CONST;
#define RYGEL_M3_UPLAY_LIST_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), RYGEL_TYPE_M3_UPLAY_LIST, RygelM3UPlayListPrivate))
enum  {
	RYGEL_M3_UPLAY_LIST_DUMMY_PROPERTY
};
RygelM3UPlayList* rygel_m3_uplay_list_new (void);
RygelM3UPlayList* rygel_m3_uplay_list_construct (GType object_type);
static void rygel_m3_uplay_list_real_constructed (GObject* base);
GUPnPDIDLLiteItem* rygel_m3_uplay_list_add_item (RygelM3UPlayList* self);
gchar* rygel_m3_uplay_list_get_string (RygelM3UPlayList* self);
static void _g_object_unref0_ (gpointer var);
static void _g_list_free__g_object_unref0_ (GList* self);
static void rygel_m3_uplay_list_finalize (GObject* obj);


RygelM3UPlayList* rygel_m3_uplay_list_construct (GType object_type) {
	RygelM3UPlayList * self = NULL;
	self = (RygelM3UPlayList*) g_object_new (object_type, NULL);
	return self;
}


RygelM3UPlayList* rygel_m3_uplay_list_new (void) {
	return rygel_m3_uplay_list_construct (RYGEL_TYPE_M3_UPLAY_LIST);
}


static void rygel_m3_uplay_list_real_constructed (GObject* base) {
	RygelM3UPlayList * self;
	GeeLinkedList* _tmp0_ = NULL;
	GUPnPDIDLLiteWriter* _tmp1_ = NULL;
	self = (RygelM3UPlayList*) base;
	_tmp0_ = gee_linked_list_new (gupnp_didl_lite_item_get_type (), (GBoxedCopyFunc) g_object_ref, g_object_unref, NULL, NULL, NULL);
	_g_object_unref0 (self->priv->items);
	self->priv->items = _tmp0_;
	_tmp1_ = gupnp_didl_lite_writer_new (NULL);
	_g_object_unref0 (self->priv->writer);
	self->priv->writer = _tmp1_;
}


GUPnPDIDLLiteItem* rygel_m3_uplay_list_add_item (RygelM3UPlayList* self) {
	GUPnPDIDLLiteItem* result = NULL;
	GeeLinkedList* _tmp0_ = NULL;
	GUPnPDIDLLiteWriter* _tmp1_ = NULL;
	GUPnPDIDLLiteItem* _tmp2_ = NULL;
	GUPnPDIDLLiteItem* _tmp3_ = NULL;
	GeeLinkedList* _tmp4_ = NULL;
	gpointer _tmp5_ = NULL;
	g_return_val_if_fail (self != NULL, NULL);
	_tmp0_ = self->priv->items;
	_tmp1_ = self->priv->writer;
	_tmp2_ = gupnp_didl_lite_writer_add_item (_tmp1_);
	_tmp3_ = _tmp2_;
	gee_abstract_collection_add ((GeeAbstractCollection*) _tmp0_, _tmp3_);
	_g_object_unref0 (_tmp3_);
	_tmp4_ = self->priv->items;
	_tmp5_ = gee_linked_list_last (_tmp4_);
	result = (GUPnPDIDLLiteItem*) _tmp5_;
	return result;
}


static gpointer _g_object_ref0 (gpointer self) {
	return self ? g_object_ref (self) : NULL;
}


static void _g_object_unref0_ (gpointer var) {
	(var == NULL) ? NULL : (var = (g_object_unref (var), NULL));
}


static void _g_list_free__g_object_unref0_ (GList* self) {
	g_list_foreach (self, (GFunc) _g_object_unref0_, NULL);
	g_list_free (self);
}


gchar* rygel_m3_uplay_list_get_string (RygelM3UPlayList* self) {
	gchar* result = NULL;
	GString* builder = NULL;
	GString* _tmp0_ = NULL;
	GString* _tmp42_ = NULL;
	const gchar* _tmp43_ = NULL;
	gchar* _tmp44_ = NULL;
	g_return_val_if_fail (self != NULL, NULL);
	_tmp0_ = g_string_new ("#EXTM3U\r\n");
	builder = _tmp0_;
	{
		GeeLinkedList* _item_list = NULL;
		GeeLinkedList* _tmp1_ = NULL;
		GeeLinkedList* _tmp2_ = NULL;
		gint _item_size = 0;
		GeeLinkedList* _tmp3_ = NULL;
		gint _tmp4_ = 0;
		gint _tmp5_ = 0;
		gint _item_index = 0;
		_tmp1_ = self->priv->items;
		_tmp2_ = _g_object_ref0 (_tmp1_);
		_item_list = _tmp2_;
		_tmp3_ = _item_list;
		_tmp4_ = gee_abstract_collection_get_size ((GeeCollection*) _tmp3_);
		_tmp5_ = _tmp4_;
		_item_size = _tmp5_;
		_item_index = -1;
		while (TRUE) {
			gint _tmp6_ = 0;
			gint _tmp7_ = 0;
			gint _tmp8_ = 0;
			GUPnPDIDLLiteItem* item = NULL;
			GeeLinkedList* _tmp9_ = NULL;
			gint _tmp10_ = 0;
			gpointer _tmp11_ = NULL;
			GList* resources = NULL;
			GUPnPDIDLLiteItem* _tmp12_ = NULL;
			GList* _tmp13_ = NULL;
			GList* _tmp14_ = NULL;
			_tmp6_ = _item_index;
			_item_index = _tmp6_ + 1;
			_tmp7_ = _item_index;
			_tmp8_ = _item_size;
			if (!(_tmp7_ < _tmp8_)) {
				break;
			}
			_tmp9_ = _item_list;
			_tmp10_ = _item_index;
			_tmp11_ = gee_abstract_list_get ((GeeAbstractList*) _tmp9_, _tmp10_);
			item = (GUPnPDIDLLiteItem*) _tmp11_;
			_tmp12_ = item;
			_tmp13_ = gupnp_didl_lite_object_get_resources ((GUPnPDIDLLiteObject*) _tmp12_);
			resources = _tmp13_;
			_tmp14_ = resources;
			if (_tmp14_ != NULL) {
				GList* authors = NULL;
				GUPnPDIDLLiteItem* _tmp15_ = NULL;
				GList* _tmp16_ = NULL;
				GString* _tmp17_ = NULL;
				GList* _tmp18_ = NULL;
				gconstpointer _tmp19_ = NULL;
				glong _tmp20_ = 0L;
				glong _tmp21_ = 0L;
				GList* _tmp22_ = NULL;
				const gchar* _tmp29_ = NULL;
				GUPnPDIDLLiteItem* _tmp30_ = NULL;
				const gchar* _tmp31_ = NULL;
				const gchar* _tmp32_ = NULL;
				GString* _tmp34_ = NULL;
				GString* _tmp35_ = NULL;
				GString* _tmp36_ = NULL;
				GList* _tmp37_ = NULL;
				gconstpointer _tmp38_ = NULL;
				const gchar* _tmp39_ = NULL;
				const gchar* _tmp40_ = NULL;
				GString* _tmp41_ = NULL;
				_tmp15_ = item;
				_tmp16_ = gupnp_didl_lite_object_get_artists ((GUPnPDIDLLiteObject*) _tmp15_);
				authors = _tmp16_;
				_tmp17_ = builder;
				_tmp18_ = resources;
				_tmp19_ = _tmp18_->data;
				_tmp20_ = gupnp_didl_lite_resource_get_duration ((GUPnPDIDLLiteResource*) _tmp19_);
				_tmp21_ = _tmp20_;
				g_string_append_printf (_tmp17_, "#EXTINF:%ld,", _tmp21_);
				_tmp22_ = authors;
				if (_tmp22_ != NULL) {
					const gchar* _tmp23_ = NULL;
					GList* _tmp24_ = NULL;
					gconstpointer _tmp25_ = NULL;
					const gchar* _tmp26_ = NULL;
					GString* _tmp28_ = NULL;
					_tmp24_ = authors;
					_tmp25_ = _tmp24_->data;
					_tmp26_ = gupnp_didl_lite_contributor_get_name ((GUPnPDIDLLiteContributor*) _tmp25_);
					_tmp23_ = _tmp26_;
					if (_tmp23_ == NULL) {
						const gchar* _tmp27_ = NULL;
						_tmp27_ = _ ("Unknown");
						_tmp23_ = _tmp27_;
					}
					_tmp28_ = builder;
					g_string_append_printf (_tmp28_, "%s - ", _tmp23_);
				}
				_tmp30_ = item;
				_tmp31_ = gupnp_didl_lite_object_get_title ((GUPnPDIDLLiteObject*) _tmp30_);
				_tmp32_ = _tmp31_;
				_tmp29_ = _tmp32_;
				if (_tmp29_ == NULL) {
					const gchar* _tmp33_ = NULL;
					_tmp33_ = _ ("Unknown");
					_tmp29_ = _tmp33_;
				}
				_tmp34_ = builder;
				g_string_append (_tmp34_, _tmp29_);
				_tmp35_ = builder;
				g_string_append (_tmp35_, "\r\n");
				_tmp36_ = builder;
				_tmp37_ = resources;
				_tmp38_ = _tmp37_->data;
				_tmp39_ = gupnp_didl_lite_resource_get_uri ((GUPnPDIDLLiteResource*) _tmp38_);
				_tmp40_ = _tmp39_;
				g_string_append (_tmp36_, _tmp40_);
				_tmp41_ = builder;
				g_string_append (_tmp41_, "\r\n");
				__g_list_free__g_object_unref0_0 (authors);
			}
			__g_list_free__g_object_unref0_0 (resources);
			_g_object_unref0 (item);
		}
		_g_object_unref0 (_item_list);
	}
	_tmp42_ = builder;
	_tmp43_ = _tmp42_->str;
	_tmp44_ = g_strdup (_tmp43_);
	result = _tmp44_;
	_g_string_free0 (builder);
	return result;
}


static void rygel_m3_uplay_list_class_init (RygelM3UPlayListClass * klass) {
	rygel_m3_uplay_list_parent_class = g_type_class_peek_parent (klass);
	g_type_class_add_private (klass, sizeof (RygelM3UPlayListPrivate));
	G_OBJECT_CLASS (klass)->constructed = rygel_m3_uplay_list_real_constructed;
	G_OBJECT_CLASS (klass)->finalize = rygel_m3_uplay_list_finalize;
}


static void rygel_m3_uplay_list_instance_init (RygelM3UPlayList * self) {
	self->priv = RYGEL_M3_UPLAY_LIST_GET_PRIVATE (self);
}


static void rygel_m3_uplay_list_finalize (GObject* obj) {
	RygelM3UPlayList * self;
	self = G_TYPE_CHECK_INSTANCE_CAST (obj, RYGEL_TYPE_M3_UPLAY_LIST, RygelM3UPlayList);
	_g_object_unref0 (self->priv->items);
	_g_object_unref0 (self->priv->writer);
	G_OBJECT_CLASS (rygel_m3_uplay_list_parent_class)->finalize (obj);
}


/**
 * Serializer class that serializes to an EXTM3U playlist for use with normal
 * media players or UPnP Renderers that don't support DIDL_S.
 *
 * For the description of the EXTM3U format, see
 * http://en.wikipedia.org/wiki/M3U#Extended_M3U_directives
 */
GType rygel_m3_uplay_list_get_type (void) {
	static volatile gsize rygel_m3_uplay_list_type_id__volatile = 0;
	if (g_once_init_enter (&rygel_m3_uplay_list_type_id__volatile)) {
		static const GTypeInfo g_define_type_info = { sizeof (RygelM3UPlayListClass), (GBaseInitFunc) NULL, (GBaseFinalizeFunc) NULL, (GClassInitFunc) rygel_m3_uplay_list_class_init, (GClassFinalizeFunc) NULL, NULL, sizeof (RygelM3UPlayList), 0, (GInstanceInitFunc) rygel_m3_uplay_list_instance_init, NULL };
		GType rygel_m3_uplay_list_type_id;
		rygel_m3_uplay_list_type_id = g_type_register_static (G_TYPE_OBJECT, "RygelM3UPlayList", &g_define_type_info, 0);
		g_once_init_leave (&rygel_m3_uplay_list_type_id__volatile, rygel_m3_uplay_list_type_id);
	}
	return rygel_m3_uplay_list_type_id__volatile;
}


