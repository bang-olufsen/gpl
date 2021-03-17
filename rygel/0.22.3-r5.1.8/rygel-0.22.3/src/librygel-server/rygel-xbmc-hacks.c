/* rygel-xbmc-hacks.c generated by valac 0.24.0, the Vala compiler
 * generated from rygel-xbmc-hacks.vala, do not modify */

/*
 * Copyright (C) 2011 Nokia Corporation.
 *
 * Author: Jens Georg <jensg@openismus.com>
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
#include <gio/gio.h>
#include <gee.h>
#include <libsoup/soup.h>


#define RYGEL_TYPE_CLIENT_HACKS (rygel_client_hacks_get_type ())
#define RYGEL_CLIENT_HACKS(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), RYGEL_TYPE_CLIENT_HACKS, RygelClientHacks))
#define RYGEL_CLIENT_HACKS_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), RYGEL_TYPE_CLIENT_HACKS, RygelClientHacksClass))
#define RYGEL_IS_CLIENT_HACKS(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), RYGEL_TYPE_CLIENT_HACKS))
#define RYGEL_IS_CLIENT_HACKS_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), RYGEL_TYPE_CLIENT_HACKS))
#define RYGEL_CLIENT_HACKS_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), RYGEL_TYPE_CLIENT_HACKS, RygelClientHacksClass))

typedef struct _RygelClientHacks RygelClientHacks;
typedef struct _RygelClientHacksClass RygelClientHacksClass;
typedef struct _RygelClientHacksPrivate RygelClientHacksPrivate;

#define RYGEL_TYPE_MEDIA_QUERY_ACTION (rygel_media_query_action_get_type ())
#define RYGEL_MEDIA_QUERY_ACTION(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), RYGEL_TYPE_MEDIA_QUERY_ACTION, RygelMediaQueryAction))
#define RYGEL_MEDIA_QUERY_ACTION_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), RYGEL_TYPE_MEDIA_QUERY_ACTION, RygelMediaQueryActionClass))
#define RYGEL_IS_MEDIA_QUERY_ACTION(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), RYGEL_TYPE_MEDIA_QUERY_ACTION))
#define RYGEL_IS_MEDIA_QUERY_ACTION_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), RYGEL_TYPE_MEDIA_QUERY_ACTION))
#define RYGEL_MEDIA_QUERY_ACTION_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), RYGEL_TYPE_MEDIA_QUERY_ACTION, RygelMediaQueryActionClass))

typedef struct _RygelMediaQueryAction RygelMediaQueryAction;
typedef struct _RygelMediaQueryActionClass RygelMediaQueryActionClass;

#define RYGEL_TYPE_MEDIA_OBJECT (rygel_media_object_get_type ())
#define RYGEL_MEDIA_OBJECT(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), RYGEL_TYPE_MEDIA_OBJECT, RygelMediaObject))
#define RYGEL_MEDIA_OBJECT_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), RYGEL_TYPE_MEDIA_OBJECT, RygelMediaObjectClass))
#define RYGEL_IS_MEDIA_OBJECT(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), RYGEL_TYPE_MEDIA_OBJECT))
#define RYGEL_IS_MEDIA_OBJECT_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), RYGEL_TYPE_MEDIA_OBJECT))
#define RYGEL_MEDIA_OBJECT_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), RYGEL_TYPE_MEDIA_OBJECT, RygelMediaObjectClass))

typedef struct _RygelMediaObject RygelMediaObject;
typedef struct _RygelMediaObjectClass RygelMediaObjectClass;

#define RYGEL_TYPE_MEDIA_CONTAINER (rygel_media_container_get_type ())
#define RYGEL_MEDIA_CONTAINER(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), RYGEL_TYPE_MEDIA_CONTAINER, RygelMediaContainer))
#define RYGEL_MEDIA_CONTAINER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), RYGEL_TYPE_MEDIA_CONTAINER, RygelMediaContainerClass))
#define RYGEL_IS_MEDIA_CONTAINER(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), RYGEL_TYPE_MEDIA_CONTAINER))
#define RYGEL_IS_MEDIA_CONTAINER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), RYGEL_TYPE_MEDIA_CONTAINER))
#define RYGEL_MEDIA_CONTAINER_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), RYGEL_TYPE_MEDIA_CONTAINER, RygelMediaContainerClass))

typedef struct _RygelMediaContainer RygelMediaContainer;
typedef struct _RygelMediaContainerClass RygelMediaContainerClass;

#define RYGEL_TYPE_SEARCHABLE_CONTAINER (rygel_searchable_container_get_type ())
#define RYGEL_SEARCHABLE_CONTAINER(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), RYGEL_TYPE_SEARCHABLE_CONTAINER, RygelSearchableContainer))
#define RYGEL_IS_SEARCHABLE_CONTAINER(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), RYGEL_TYPE_SEARCHABLE_CONTAINER))
#define RYGEL_SEARCHABLE_CONTAINER_GET_INTERFACE(obj) (G_TYPE_INSTANCE_GET_INTERFACE ((obj), RYGEL_TYPE_SEARCHABLE_CONTAINER, RygelSearchableContainerIface))

typedef struct _RygelSearchableContainer RygelSearchableContainer;
typedef struct _RygelSearchableContainerIface RygelSearchableContainerIface;

#define RYGEL_TYPE_SEARCH_EXPRESSION (rygel_search_expression_get_type ())
#define RYGEL_SEARCH_EXPRESSION(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), RYGEL_TYPE_SEARCH_EXPRESSION, RygelSearchExpression))
#define RYGEL_SEARCH_EXPRESSION_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), RYGEL_TYPE_SEARCH_EXPRESSION, RygelSearchExpressionClass))
#define RYGEL_IS_SEARCH_EXPRESSION(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), RYGEL_TYPE_SEARCH_EXPRESSION))
#define RYGEL_IS_SEARCH_EXPRESSION_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), RYGEL_TYPE_SEARCH_EXPRESSION))
#define RYGEL_SEARCH_EXPRESSION_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), RYGEL_TYPE_SEARCH_EXPRESSION, RygelSearchExpressionClass))

typedef struct _RygelSearchExpression RygelSearchExpression;
typedef struct _RygelSearchExpressionClass RygelSearchExpressionClass;

#define RYGEL_TYPE_MEDIA_OBJECTS (rygel_media_objects_get_type ())
#define RYGEL_MEDIA_OBJECTS(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), RYGEL_TYPE_MEDIA_OBJECTS, RygelMediaObjects))
#define RYGEL_MEDIA_OBJECTS_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), RYGEL_TYPE_MEDIA_OBJECTS, RygelMediaObjectsClass))
#define RYGEL_IS_MEDIA_OBJECTS(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), RYGEL_TYPE_MEDIA_OBJECTS))
#define RYGEL_IS_MEDIA_OBJECTS_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), RYGEL_TYPE_MEDIA_OBJECTS))
#define RYGEL_MEDIA_OBJECTS_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), RYGEL_TYPE_MEDIA_OBJECTS, RygelMediaObjectsClass))

typedef struct _RygelMediaObjects RygelMediaObjects;
typedef struct _RygelMediaObjectsClass RygelMediaObjectsClass;

#define RYGEL_TYPE_XBMC_HACKS (rygel_xbmc_hacks_get_type ())
#define RYGEL_XBMC_HACKS(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), RYGEL_TYPE_XBMC_HACKS, RygelXBMCHacks))
#define RYGEL_XBMC_HACKS_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), RYGEL_TYPE_XBMC_HACKS, RygelXBMCHacksClass))
#define RYGEL_IS_XBMC_HACKS(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), RYGEL_TYPE_XBMC_HACKS))
#define RYGEL_IS_XBMC_HACKS_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), RYGEL_TYPE_XBMC_HACKS))
#define RYGEL_XBMC_HACKS_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), RYGEL_TYPE_XBMC_HACKS, RygelXBMCHacksClass))

typedef struct _RygelXBMCHacks RygelXBMCHacks;
typedef struct _RygelXBMCHacksClass RygelXBMCHacksClass;
typedef struct _RygelXBMCHacksPrivate RygelXBMCHacksPrivate;
#define _g_object_unref0(var) ((var == NULL) ? NULL : (var = (g_object_unref (var), NULL)))

#define RYGEL_TYPE_MEDIA_ITEM (rygel_media_item_get_type ())
#define RYGEL_MEDIA_ITEM(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), RYGEL_TYPE_MEDIA_ITEM, RygelMediaItem))
#define RYGEL_MEDIA_ITEM_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), RYGEL_TYPE_MEDIA_ITEM, RygelMediaItemClass))
#define RYGEL_IS_MEDIA_ITEM(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), RYGEL_TYPE_MEDIA_ITEM))
#define RYGEL_IS_MEDIA_ITEM_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), RYGEL_TYPE_MEDIA_ITEM))
#define RYGEL_MEDIA_ITEM_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), RYGEL_TYPE_MEDIA_ITEM, RygelMediaItemClass))

typedef struct _RygelMediaItem RygelMediaItem;
typedef struct _RygelMediaItemClass RygelMediaItemClass;

struct _RygelSearchableContainerIface {
	GTypeInterface parent_iface;
	void (*search) (RygelSearchableContainer* self, RygelSearchExpression* expression, guint offset, guint max_count, const gchar* sort_criteria, GCancellable* cancellable, GAsyncReadyCallback _callback_, gpointer _user_data_);
	RygelMediaObjects* (*search_finish) (RygelSearchableContainer* self, GAsyncResult* _res_, guint* total_matches, GError** error);
	GeeArrayList* (*get_search_classes) (RygelSearchableContainer* self);
	void (*set_search_classes) (RygelSearchableContainer* self, GeeArrayList* value);
};

struct _RygelClientHacks {
	GObject parent_instance;
	RygelClientHacksPrivate * priv;
	GRegex* agent_regex;
};

struct _RygelClientHacksClass {
	GObjectClass parent_class;
	void (*translate_container_id) (RygelClientHacks* self, RygelMediaQueryAction* action, gchar** container_id);
	void (*apply) (RygelClientHacks* self, RygelMediaObject* object);
	void (*filter_sort_criteria) (RygelClientHacks* self, gchar** sort_criteria);
	gboolean (*force_seek) (RygelClientHacks* self);
	void (*search) (RygelClientHacks* self, RygelSearchableContainer* container, RygelSearchExpression* expression, guint offset, guint max_count, const gchar* sort_criteria, GCancellable* cancellable, GAsyncReadyCallback _callback_, gpointer _user_data_);
	RygelMediaObjects* (*search_finish) (RygelClientHacks* self, GAsyncResult* _res_, guint* total_matches, GError** error);
};

struct _RygelXBMCHacks {
	RygelClientHacks parent_instance;
	RygelXBMCHacksPrivate * priv;
};

struct _RygelXBMCHacksClass {
	RygelClientHacksClass parent_class;
};

typedef enum  {
	RYGEL_CLIENT_HACKS_ERROR_NA
} RygelClientHacksError;
#define RYGEL_CLIENT_HACKS_ERROR rygel_client_hacks_error_quark ()

static gpointer rygel_xbmc_hacks_parent_class = NULL;

GType rygel_client_hacks_get_type (void) G_GNUC_CONST;
GType rygel_media_query_action_get_type (void) G_GNUC_CONST;
GType rygel_media_object_get_type (void) G_GNUC_CONST;
GType rygel_media_container_get_type (void) G_GNUC_CONST;
gpointer rygel_search_expression_ref (gpointer instance);
void rygel_search_expression_unref (gpointer instance);
GParamSpec* rygel_param_spec_search_expression (const gchar* name, const gchar* nick, const gchar* blurb, GType object_type, GParamFlags flags);
void rygel_value_set_search_expression (GValue* value, gpointer v_object);
void rygel_value_take_search_expression (GValue* value, gpointer v_object);
gpointer rygel_value_get_search_expression (const GValue* value);
GType rygel_search_expression_get_type (void) G_GNUC_CONST;
GType rygel_media_objects_get_type (void) G_GNUC_CONST;
GType rygel_searchable_container_get_type (void) G_GNUC_CONST;
GType rygel_xbmc_hacks_get_type (void) G_GNUC_CONST;
enum  {
	RYGEL_XBMC_HACKS_DUMMY_PROPERTY
};
#define RYGEL_XBMC_HACKS_AGENT ".*Platinum/.*|.*XBMC/.*"
GQuark rygel_client_hacks_error_quark (void);
RygelXBMCHacks* rygel_xbmc_hacks_new (SoupMessage* message, GError** error);
RygelXBMCHacks* rygel_xbmc_hacks_construct (GType object_type, SoupMessage* message, GError** error);
RygelClientHacks* rygel_client_hacks_construct (GType object_type, const gchar* agent, SoupMessage* message, GError** error);
static void rygel_xbmc_hacks_real_apply (RygelClientHacks* base, RygelMediaObject* object);
GType rygel_media_item_get_type (void) G_GNUC_CONST;
const gchar* rygel_media_item_get_mime_type (RygelMediaItem* self);
void rygel_media_item_set_mime_type (RygelMediaItem* self, const gchar* value);
static gboolean rygel_xbmc_hacks_real_force_seek (RygelClientHacks* base);


RygelXBMCHacks* rygel_xbmc_hacks_construct (GType object_type, SoupMessage* message, GError** error) {
	RygelXBMCHacks * self = NULL;
	SoupMessage* _tmp0_ = NULL;
	GError * _inner_error_ = NULL;
	_tmp0_ = message;
	self = (RygelXBMCHacks*) rygel_client_hacks_construct (object_type, RYGEL_XBMC_HACKS_AGENT, _tmp0_, &_inner_error_);
	if (_inner_error_ != NULL) {
		if (_inner_error_->domain == RYGEL_CLIENT_HACKS_ERROR) {
			g_propagate_error (error, _inner_error_);
			_g_object_unref0 (self);
			return NULL;
		} else {
			g_critical ("file %s: line %d: uncaught error: %s (%s, %d)", __FILE__, __LINE__, _inner_error_->message, g_quark_to_string (_inner_error_->domain), _inner_error_->code);
			g_clear_error (&_inner_error_);
			return NULL;
		}
	}
	return self;
}


RygelXBMCHacks* rygel_xbmc_hacks_new (SoupMessage* message, GError** error) {
	return rygel_xbmc_hacks_construct (RYGEL_TYPE_XBMC_HACKS, message, error);
}


static gpointer _g_object_ref0 (gpointer self) {
	return self ? g_object_ref (self) : NULL;
}


static void rygel_xbmc_hacks_real_apply (RygelClientHacks* base, RygelMediaObject* object) {
	RygelXBMCHacks * self;
	RygelMediaObject* _tmp0_ = NULL;
	RygelMediaItem* item = NULL;
	RygelMediaObject* _tmp1_ = NULL;
	RygelMediaItem* _tmp2_ = NULL;
	gboolean _tmp3_ = FALSE;
	gboolean _tmp4_ = FALSE;
	RygelMediaItem* _tmp5_ = NULL;
	const gchar* _tmp6_ = NULL;
	const gchar* _tmp7_ = NULL;
	self = (RygelXBMCHacks*) base;
	g_return_if_fail (object != NULL);
	_tmp0_ = object;
	if (!G_TYPE_CHECK_INSTANCE_TYPE (_tmp0_, RYGEL_TYPE_MEDIA_ITEM)) {
		return;
	}
	_tmp1_ = object;
	_tmp2_ = _g_object_ref0 (G_TYPE_CHECK_INSTANCE_TYPE (_tmp1_, RYGEL_TYPE_MEDIA_ITEM) ? ((RygelMediaItem*) _tmp1_) : NULL);
	item = _tmp2_;
	_tmp5_ = item;
	_tmp6_ = rygel_media_item_get_mime_type (_tmp5_);
	_tmp7_ = _tmp6_;
	if (g_strcmp0 (_tmp7_, "audio/mp4") == 0) {
		_tmp4_ = TRUE;
	} else {
		RygelMediaItem* _tmp8_ = NULL;
		const gchar* _tmp9_ = NULL;
		const gchar* _tmp10_ = NULL;
		_tmp8_ = item;
		_tmp9_ = rygel_media_item_get_mime_type (_tmp8_);
		_tmp10_ = _tmp9_;
		_tmp4_ = g_strcmp0 (_tmp10_, "audio/3gpp") == 0;
	}
	if (_tmp4_) {
		_tmp3_ = TRUE;
	} else {
		RygelMediaItem* _tmp11_ = NULL;
		const gchar* _tmp12_ = NULL;
		const gchar* _tmp13_ = NULL;
		_tmp11_ = item;
		_tmp12_ = rygel_media_item_get_mime_type (_tmp11_);
		_tmp13_ = _tmp12_;
		_tmp3_ = g_strcmp0 (_tmp13_, "audio/vnd.dlna.adts") == 0;
	}
	if (_tmp3_) {
		RygelMediaItem* _tmp14_ = NULL;
		_tmp14_ = item;
		rygel_media_item_set_mime_type (_tmp14_, "audio/aac");
	}
	_g_object_unref0 (item);
}


static gboolean rygel_xbmc_hacks_real_force_seek (RygelClientHacks* base) {
	RygelXBMCHacks * self;
	gboolean result = FALSE;
	self = (RygelXBMCHacks*) base;
	result = TRUE;
	return result;
}


static void rygel_xbmc_hacks_class_init (RygelXBMCHacksClass * klass) {
	rygel_xbmc_hacks_parent_class = g_type_class_peek_parent (klass);
	RYGEL_CLIENT_HACKS_CLASS (klass)->apply = rygel_xbmc_hacks_real_apply;
	RYGEL_CLIENT_HACKS_CLASS (klass)->force_seek = rygel_xbmc_hacks_real_force_seek;
}


static void rygel_xbmc_hacks_instance_init (RygelXBMCHacks * self) {
}


GType rygel_xbmc_hacks_get_type (void) {
	static volatile gsize rygel_xbmc_hacks_type_id__volatile = 0;
	if (g_once_init_enter (&rygel_xbmc_hacks_type_id__volatile)) {
		static const GTypeInfo g_define_type_info = { sizeof (RygelXBMCHacksClass), (GBaseInitFunc) NULL, (GBaseFinalizeFunc) NULL, (GClassInitFunc) rygel_xbmc_hacks_class_init, (GClassFinalizeFunc) NULL, NULL, sizeof (RygelXBMCHacks), 0, (GInstanceInitFunc) rygel_xbmc_hacks_instance_init, NULL };
		GType rygel_xbmc_hacks_type_id;
		rygel_xbmc_hacks_type_id = g_type_register_static (RYGEL_TYPE_CLIENT_HACKS, "RygelXBMCHacks", &g_define_type_info, 0);
		g_once_init_leave (&rygel_xbmc_hacks_type_id__volatile, rygel_xbmc_hacks_type_id);
	}
	return rygel_xbmc_hacks_type_id__volatile;
}


