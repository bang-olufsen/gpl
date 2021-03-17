/* rygel-media-export-leaf-query-container.c generated by valac 0.24.0, the Vala compiler
 * generated from rygel-media-export-leaf-query-container.vala, do not modify */

/*
 * Copyright (C) 2011 Jens Georg <mail@jensge.org>.
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
#include <rygel-server.h>
#include <gio/gio.h>
#include <stdlib.h>
#include <string.h>
#include <gee.h>
#include <glib/gi18n-lib.h>


#define RYGEL_MEDIA_EXPORT_TYPE_DB_CONTAINER (rygel_media_export_db_container_get_type ())
#define RYGEL_MEDIA_EXPORT_DB_CONTAINER(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), RYGEL_MEDIA_EXPORT_TYPE_DB_CONTAINER, RygelMediaExportDBContainer))
#define RYGEL_MEDIA_EXPORT_DB_CONTAINER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), RYGEL_MEDIA_EXPORT_TYPE_DB_CONTAINER, RygelMediaExportDBContainerClass))
#define RYGEL_MEDIA_EXPORT_IS_DB_CONTAINER(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), RYGEL_MEDIA_EXPORT_TYPE_DB_CONTAINER))
#define RYGEL_MEDIA_EXPORT_IS_DB_CONTAINER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), RYGEL_MEDIA_EXPORT_TYPE_DB_CONTAINER))
#define RYGEL_MEDIA_EXPORT_DB_CONTAINER_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), RYGEL_MEDIA_EXPORT_TYPE_DB_CONTAINER, RygelMediaExportDBContainerClass))

typedef struct _RygelMediaExportDBContainer RygelMediaExportDBContainer;
typedef struct _RygelMediaExportDBContainerClass RygelMediaExportDBContainerClass;
typedef struct _RygelMediaExportDBContainerPrivate RygelMediaExportDBContainerPrivate;

#define RYGEL_MEDIA_EXPORT_TYPE_MEDIA_CACHE (rygel_media_export_media_cache_get_type ())
#define RYGEL_MEDIA_EXPORT_MEDIA_CACHE(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), RYGEL_MEDIA_EXPORT_TYPE_MEDIA_CACHE, RygelMediaExportMediaCache))
#define RYGEL_MEDIA_EXPORT_MEDIA_CACHE_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), RYGEL_MEDIA_EXPORT_TYPE_MEDIA_CACHE, RygelMediaExportMediaCacheClass))
#define RYGEL_MEDIA_EXPORT_IS_MEDIA_CACHE(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), RYGEL_MEDIA_EXPORT_TYPE_MEDIA_CACHE))
#define RYGEL_MEDIA_EXPORT_IS_MEDIA_CACHE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), RYGEL_MEDIA_EXPORT_TYPE_MEDIA_CACHE))
#define RYGEL_MEDIA_EXPORT_MEDIA_CACHE_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), RYGEL_MEDIA_EXPORT_TYPE_MEDIA_CACHE, RygelMediaExportMediaCacheClass))

typedef struct _RygelMediaExportMediaCache RygelMediaExportMediaCache;
typedef struct _RygelMediaExportMediaCacheClass RygelMediaExportMediaCacheClass;

#define RYGEL_MEDIA_EXPORT_TYPE_QUERY_CONTAINER (rygel_media_export_query_container_get_type ())
#define RYGEL_MEDIA_EXPORT_QUERY_CONTAINER(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), RYGEL_MEDIA_EXPORT_TYPE_QUERY_CONTAINER, RygelMediaExportQueryContainer))
#define RYGEL_MEDIA_EXPORT_QUERY_CONTAINER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), RYGEL_MEDIA_EXPORT_TYPE_QUERY_CONTAINER, RygelMediaExportQueryContainerClass))
#define RYGEL_MEDIA_EXPORT_IS_QUERY_CONTAINER(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), RYGEL_MEDIA_EXPORT_TYPE_QUERY_CONTAINER))
#define RYGEL_MEDIA_EXPORT_IS_QUERY_CONTAINER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), RYGEL_MEDIA_EXPORT_TYPE_QUERY_CONTAINER))
#define RYGEL_MEDIA_EXPORT_QUERY_CONTAINER_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), RYGEL_MEDIA_EXPORT_TYPE_QUERY_CONTAINER, RygelMediaExportQueryContainerClass))

typedef struct _RygelMediaExportQueryContainer RygelMediaExportQueryContainer;
typedef struct _RygelMediaExportQueryContainerClass RygelMediaExportQueryContainerClass;
typedef struct _RygelMediaExportQueryContainerPrivate RygelMediaExportQueryContainerPrivate;

#define RYGEL_MEDIA_EXPORT_TYPE_LEAF_QUERY_CONTAINER (rygel_media_export_leaf_query_container_get_type ())
#define RYGEL_MEDIA_EXPORT_LEAF_QUERY_CONTAINER(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), RYGEL_MEDIA_EXPORT_TYPE_LEAF_QUERY_CONTAINER, RygelMediaExportLeafQueryContainer))
#define RYGEL_MEDIA_EXPORT_LEAF_QUERY_CONTAINER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), RYGEL_MEDIA_EXPORT_TYPE_LEAF_QUERY_CONTAINER, RygelMediaExportLeafQueryContainerClass))
#define RYGEL_MEDIA_EXPORT_IS_LEAF_QUERY_CONTAINER(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), RYGEL_MEDIA_EXPORT_TYPE_LEAF_QUERY_CONTAINER))
#define RYGEL_MEDIA_EXPORT_IS_LEAF_QUERY_CONTAINER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), RYGEL_MEDIA_EXPORT_TYPE_LEAF_QUERY_CONTAINER))
#define RYGEL_MEDIA_EXPORT_LEAF_QUERY_CONTAINER_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), RYGEL_MEDIA_EXPORT_TYPE_LEAF_QUERY_CONTAINER, RygelMediaExportLeafQueryContainerClass))

typedef struct _RygelMediaExportLeafQueryContainer RygelMediaExportLeafQueryContainer;
typedef struct _RygelMediaExportLeafQueryContainerClass RygelMediaExportLeafQueryContainerClass;
typedef struct _RygelMediaExportLeafQueryContainerPrivate RygelMediaExportLeafQueryContainerPrivate;
#define _g_free0(var) (var = (g_free (var), NULL))
#define _g_object_unref0(var) ((var == NULL) ? NULL : (var = (g_object_unref (var), NULL)))
#define _g_regex_unref0(var) ((var == NULL) ? NULL : (var = (g_regex_unref (var), NULL)))
#define _g_error_free0(var) ((var == NULL) ? NULL : (var = (g_error_free (var), NULL)))
typedef struct _RygelMediaExportLeafQueryContainerGetChildrenData RygelMediaExportLeafQueryContainerGetChildrenData;

struct _RygelMediaExportDBContainer {
	RygelMediaContainer parent_instance;
	RygelMediaExportDBContainerPrivate * priv;
	RygelMediaExportMediaCache* media_db;
};

struct _RygelMediaExportDBContainerClass {
	RygelMediaContainerClass parent_class;
	gint (*count_children) (RygelMediaExportDBContainer* self);
	void (*search) (RygelMediaExportDBContainer* self, RygelSearchExpression* expression, guint offset, guint max_count, const gchar* sort_criteria, GCancellable* cancellable, GAsyncReadyCallback _callback_, gpointer _user_data_);
	RygelMediaObjects* (*search_finish) (RygelMediaExportDBContainer* self, GAsyncResult* _res_, guint* total_matches, GError** error);
};

struct _RygelMediaExportQueryContainer {
	RygelMediaExportDBContainer parent_instance;
	RygelMediaExportQueryContainerPrivate * priv;
};

struct _RygelMediaExportQueryContainerClass {
	RygelMediaExportDBContainerClass parent_class;
};

struct _RygelMediaExportLeafQueryContainer {
	RygelMediaExportQueryContainer parent_instance;
	RygelMediaExportLeafQueryContainerPrivate * priv;
};

struct _RygelMediaExportLeafQueryContainerClass {
	RygelMediaExportQueryContainerClass parent_class;
};

struct _RygelMediaExportLeafQueryContainerGetChildrenData {
	int _state_;
	GObject* _source_object_;
	GAsyncResult* _res_;
	GSimpleAsyncResult* _async_result;
	RygelMediaExportLeafQueryContainer* self;
	guint offset;
	guint max_count;
	gchar* sort_criteria;
	GCancellable* cancellable;
	RygelMediaObjects* result;
	guint total_matches;
	RygelMediaObjects* children;
	RygelMediaExportMediaCache* _tmp0_;
	RygelSearchExpression* _tmp1_;
	RygelSearchExpression* _tmp2_;
	const gchar* _tmp3_;
	guint _tmp4_;
	guint _tmp5_;
	guint _tmp6_;
	RygelMediaObjects* _tmp7_;
	RygelMediaObjects* _child_list;
	RygelMediaObjects* _tmp8_;
	gint _child_size;
	RygelMediaObjects* _tmp9_;
	gint _tmp10_;
	gint _tmp11_;
	gint _child_index;
	gint _tmp12_;
	gint _tmp13_;
	gint _tmp14_;
	RygelMediaObject* child;
	RygelMediaObjects* _tmp15_;
	gint _tmp16_;
	gpointer _tmp17_;
	gchar* container_id;
	const gchar* _tmp18_;
	const gchar* _tmp19_;
	gchar* _tmp20_;
	gchar* _tmp21_;
	gchar* _tmp22_;
	gchar* _tmp23_;
	RygelMediaObject* _tmp24_;
	RygelMediaObject* _tmp25_;
	const gchar* _tmp26_;
	const gchar* _tmp27_;
	RygelMediaObject* _tmp28_;
	const gchar* _tmp29_;
	gchar* _tmp30_;
	gchar* _tmp31_;
	RygelMediaObject* _tmp32_;
	const gchar* _tmp33_;
	const gchar* _tmp34_;
	gchar* _tmp35_;
	gchar* _tmp36_;
	RygelMediaObject* _tmp37_;
	GError * _inner_error_;
};


static gpointer rygel_media_export_leaf_query_container_parent_class = NULL;

GType rygel_media_export_db_container_get_type (void) G_GNUC_CONST;
GType rygel_media_export_media_cache_get_type (void) G_GNUC_CONST;
GType rygel_media_export_query_container_get_type (void) G_GNUC_CONST;
GType rygel_media_export_leaf_query_container_get_type (void) G_GNUC_CONST;
enum  {
	RYGEL_MEDIA_EXPORT_LEAF_QUERY_CONTAINER_DUMMY_PROPERTY
};
RygelMediaExportLeafQueryContainer* rygel_media_export_leaf_query_container_new (RygelSearchExpression* expression, const gchar* id, const gchar* name);
RygelMediaExportLeafQueryContainer* rygel_media_export_leaf_query_container_construct (GType object_type, RygelSearchExpression* expression, const gchar* id, const gchar* name);
static void rygel_media_export_leaf_query_container_real_get_children_data_free (gpointer _data);
static void rygel_media_export_leaf_query_container_real_get_children (RygelMediaContainer* base, guint offset, guint max_count, const gchar* sort_criteria, GCancellable* cancellable, GAsyncReadyCallback _callback_, gpointer _user_data_);
static gboolean rygel_media_export_leaf_query_container_real_get_children_co (RygelMediaExportLeafQueryContainerGetChildrenData* _data_);
RygelMediaObjects* rygel_media_export_media_cache_get_objects_by_search_expression (RygelMediaExportMediaCache* self, RygelSearchExpression* expression, const gchar* container_id, const gchar* sort_criteria, guint offset, guint max_count, guint* total_matches, GError** error);
RygelSearchExpression* rygel_media_export_query_container_get_expression (RygelMediaExportQueryContainer* self);
#define RYGEL_MEDIA_EXPORT_QUERY_CONTAINER_ITEM_PREFIX "virtual-id:"
#define RYGEL_MEDIA_EXPORT_QUERY_CONTAINER_PREFIX "virtual-container:"
static gint rygel_media_export_leaf_query_container_real_count_children (RygelMediaExportDBContainer* base);
glong rygel_media_export_media_cache_get_object_count_by_search_expression (RygelMediaExportMediaCache* self, RygelSearchExpression* expression, const gchar* container_id, GError** error);


RygelMediaExportLeafQueryContainer* rygel_media_export_leaf_query_container_construct (GType object_type, RygelSearchExpression* expression, const gchar* id, const gchar* name) {
	RygelMediaExportLeafQueryContainer * self = NULL;
	const gchar* _tmp0_ = NULL;
	const gchar* _tmp1_ = NULL;
	RygelSearchExpression* _tmp2_ = NULL;
	g_return_val_if_fail (expression != NULL, NULL);
	g_return_val_if_fail (id != NULL, NULL);
	g_return_val_if_fail (name != NULL, NULL);
	_tmp0_ = id;
	_tmp1_ = name;
	_tmp2_ = expression;
	self = (RygelMediaExportLeafQueryContainer*) g_object_new (object_type, "id", _tmp0_, "title", _tmp1_, "parent", NULL, "child-count", 0, "expression", _tmp2_, NULL);
	return self;
}


RygelMediaExportLeafQueryContainer* rygel_media_export_leaf_query_container_new (RygelSearchExpression* expression, const gchar* id, const gchar* name) {
	return rygel_media_export_leaf_query_container_construct (RYGEL_MEDIA_EXPORT_TYPE_LEAF_QUERY_CONTAINER, expression, id, name);
}


static void rygel_media_export_leaf_query_container_real_get_children_data_free (gpointer _data) {
	RygelMediaExportLeafQueryContainerGetChildrenData* _data_;
	_data_ = _data;
	_g_free0 (_data_->sort_criteria);
	_g_object_unref0 (_data_->cancellable);
	_g_object_unref0 (_data_->result);
	_g_object_unref0 (_data_->self);
	g_slice_free (RygelMediaExportLeafQueryContainerGetChildrenData, _data_);
}


static gpointer _g_object_ref0 (gpointer self) {
	return self ? g_object_ref (self) : NULL;
}


static void rygel_media_export_leaf_query_container_real_get_children (RygelMediaContainer* base, guint offset, guint max_count, const gchar* sort_criteria, GCancellable* cancellable, GAsyncReadyCallback _callback_, gpointer _user_data_) {
	RygelMediaExportLeafQueryContainer * self;
	RygelMediaExportLeafQueryContainerGetChildrenData* _data_;
	RygelMediaExportLeafQueryContainer* _tmp0_ = NULL;
	guint _tmp1_ = 0U;
	guint _tmp2_ = 0U;
	const gchar* _tmp3_ = NULL;
	gchar* _tmp4_ = NULL;
	GCancellable* _tmp5_ = NULL;
	GCancellable* _tmp6_ = NULL;
	self = (RygelMediaExportLeafQueryContainer*) base;
	_data_ = g_slice_new0 (RygelMediaExportLeafQueryContainerGetChildrenData);
	_data_->_async_result = g_simple_async_result_new (G_OBJECT (self), _callback_, _user_data_, rygel_media_export_leaf_query_container_real_get_children);
	g_simple_async_result_set_op_res_gpointer (_data_->_async_result, _data_, rygel_media_export_leaf_query_container_real_get_children_data_free);
	_tmp0_ = _g_object_ref0 (self);
	_data_->self = _tmp0_;
	_tmp1_ = offset;
	_data_->offset = _tmp1_;
	_tmp2_ = max_count;
	_data_->max_count = _tmp2_;
	_tmp3_ = sort_criteria;
	_tmp4_ = g_strdup (_tmp3_);
	_g_free0 (_data_->sort_criteria);
	_data_->sort_criteria = _tmp4_;
	_tmp5_ = cancellable;
	_tmp6_ = _g_object_ref0 (_tmp5_);
	_g_object_unref0 (_data_->cancellable);
	_data_->cancellable = _tmp6_;
	rygel_media_export_leaf_query_container_real_get_children_co (_data_);
}


static RygelMediaObjects* rygel_media_export_leaf_query_container_real_get_children_finish (RygelMediaContainer* base, GAsyncResult* _res_, GError** error) {
	RygelMediaObjects* result;
	RygelMediaExportLeafQueryContainerGetChildrenData* _data_;
	if (g_simple_async_result_propagate_error (G_SIMPLE_ASYNC_RESULT (_res_), error)) {
		return NULL;
	}
	_data_ = g_simple_async_result_get_op_res_gpointer (G_SIMPLE_ASYNC_RESULT (_res_));
	result = _data_->result;
	_data_->result = NULL;
	return result;
}


static gchar* string_replace (const gchar* self, const gchar* old, const gchar* replacement) {
	gchar* result = NULL;
	GError * _inner_error_ = NULL;
	g_return_val_if_fail (self != NULL, NULL);
	g_return_val_if_fail (old != NULL, NULL);
	g_return_val_if_fail (replacement != NULL, NULL);
	{
		GRegex* regex = NULL;
		const gchar* _tmp0_ = NULL;
		gchar* _tmp1_ = NULL;
		gchar* _tmp2_ = NULL;
		GRegex* _tmp3_ = NULL;
		GRegex* _tmp4_ = NULL;
		gchar* _tmp5_ = NULL;
		GRegex* _tmp6_ = NULL;
		const gchar* _tmp7_ = NULL;
		gchar* _tmp8_ = NULL;
		gchar* _tmp9_ = NULL;
		_tmp0_ = old;
		_tmp1_ = g_regex_escape_string (_tmp0_, -1);
		_tmp2_ = _tmp1_;
		_tmp3_ = g_regex_new (_tmp2_, 0, 0, &_inner_error_);
		_tmp4_ = _tmp3_;
		_g_free0 (_tmp2_);
		regex = _tmp4_;
		if (_inner_error_ != NULL) {
			if (_inner_error_->domain == G_REGEX_ERROR) {
				goto __catch55_g_regex_error;
			}
			g_critical ("file %s: line %d: unexpected error: %s (%s, %d)", __FILE__, __LINE__, _inner_error_->message, g_quark_to_string (_inner_error_->domain), _inner_error_->code);
			g_clear_error (&_inner_error_);
			return NULL;
		}
		_tmp6_ = regex;
		_tmp7_ = replacement;
		_tmp8_ = g_regex_replace_literal (_tmp6_, self, (gssize) (-1), 0, _tmp7_, 0, &_inner_error_);
		_tmp5_ = _tmp8_;
		if (_inner_error_ != NULL) {
			_g_regex_unref0 (regex);
			if (_inner_error_->domain == G_REGEX_ERROR) {
				goto __catch55_g_regex_error;
			}
			_g_regex_unref0 (regex);
			g_critical ("file %s: line %d: unexpected error: %s (%s, %d)", __FILE__, __LINE__, _inner_error_->message, g_quark_to_string (_inner_error_->domain), _inner_error_->code);
			g_clear_error (&_inner_error_);
			return NULL;
		}
		_tmp9_ = _tmp5_;
		_tmp5_ = NULL;
		result = _tmp9_;
		_g_free0 (_tmp5_);
		_g_regex_unref0 (regex);
		return result;
	}
	goto __finally55;
	__catch55_g_regex_error:
	{
		GError* e = NULL;
		e = _inner_error_;
		_inner_error_ = NULL;
		g_assert_not_reached ();
		_g_error_free0 (e);
	}
	__finally55:
	if (_inner_error_ != NULL) {
		g_critical ("file %s: line %d: uncaught error: %s (%s, %d)", __FILE__, __LINE__, _inner_error_->message, g_quark_to_string (_inner_error_->domain), _inner_error_->code);
		g_clear_error (&_inner_error_);
		return NULL;
	}
}


static gboolean rygel_media_export_leaf_query_container_real_get_children_co (RygelMediaExportLeafQueryContainerGetChildrenData* _data_) {
	switch (_data_->_state_) {
		case 0:
		goto _state_0;
		default:
		g_assert_not_reached ();
	}
	_state_0:
	_data_->_tmp0_ = NULL;
	_data_->_tmp0_ = ((RygelMediaExportDBContainer*) _data_->self)->media_db;
	_data_->_tmp1_ = NULL;
	_data_->_tmp1_ = rygel_media_export_query_container_get_expression ((RygelMediaExportQueryContainer*) _data_->self);
	_data_->_tmp2_ = NULL;
	_data_->_tmp2_ = _data_->_tmp1_;
	_data_->_tmp3_ = NULL;
	_data_->_tmp3_ = _data_->sort_criteria;
	_data_->_tmp4_ = 0U;
	_data_->_tmp4_ = _data_->offset;
	_data_->_tmp5_ = 0U;
	_data_->_tmp5_ = _data_->max_count;
	_data_->_tmp6_ = 0U;
	_data_->_tmp7_ = NULL;
	_data_->_tmp7_ = rygel_media_export_media_cache_get_objects_by_search_expression (_data_->_tmp0_, _data_->_tmp2_, "0", _data_->_tmp3_, _data_->_tmp4_, _data_->_tmp5_, &_data_->_tmp6_, &_data_->_inner_error_);
	_data_->total_matches = _data_->_tmp6_;
	_data_->children = _data_->_tmp7_;
	if (_data_->_inner_error_ != NULL) {
		g_simple_async_result_set_from_error (_data_->_async_result, _data_->_inner_error_);
		g_error_free (_data_->_inner_error_);
		if (_data_->_state_ == 0) {
			g_simple_async_result_complete_in_idle (_data_->_async_result);
		} else {
			g_simple_async_result_complete (_data_->_async_result);
		}
		g_object_unref (_data_->_async_result);
		return FALSE;
	}
	{
		_data_->_tmp8_ = NULL;
		_data_->_tmp8_ = _g_object_ref0 (_data_->children);
		_data_->_child_list = _data_->_tmp8_;
		_data_->_tmp9_ = NULL;
		_data_->_tmp9_ = _data_->_child_list;
		_data_->_tmp10_ = 0;
		_data_->_tmp10_ = gee_abstract_collection_get_size ((GeeCollection*) _data_->_tmp9_);
		_data_->_tmp11_ = 0;
		_data_->_tmp11_ = _data_->_tmp10_;
		_data_->_child_size = _data_->_tmp11_;
		_data_->_child_index = -1;
		while (TRUE) {
			_data_->_tmp12_ = 0;
			_data_->_tmp12_ = _data_->_child_index;
			_data_->_child_index = _data_->_tmp12_ + 1;
			_data_->_tmp13_ = 0;
			_data_->_tmp13_ = _data_->_child_index;
			_data_->_tmp14_ = 0;
			_data_->_tmp14_ = _data_->_child_size;
			if (!(_data_->_tmp13_ < _data_->_tmp14_)) {
				break;
			}
			_data_->_tmp15_ = NULL;
			_data_->_tmp15_ = _data_->_child_list;
			_data_->_tmp16_ = 0;
			_data_->_tmp16_ = _data_->_child_index;
			_data_->_tmp17_ = NULL;
			_data_->_tmp17_ = gee_abstract_list_get ((GeeAbstractList*) _data_->_tmp15_, _data_->_tmp16_);
			_data_->child = (RygelMediaObject*) _data_->_tmp17_;
			_data_->_tmp18_ = NULL;
			_data_->_tmp18_ = rygel_media_object_get_id ((RygelMediaObject*) _data_->self);
			_data_->_tmp19_ = NULL;
			_data_->_tmp19_ = _data_->_tmp18_;
			_data_->_tmp20_ = NULL;
			_data_->_tmp20_ = string_replace (_data_->_tmp19_, RYGEL_MEDIA_EXPORT_QUERY_CONTAINER_PREFIX, "");
			_data_->_tmp21_ = NULL;
			_data_->_tmp21_ = _data_->_tmp20_;
			_data_->_tmp22_ = NULL;
			_data_->_tmp22_ = g_strconcat (RYGEL_MEDIA_EXPORT_QUERY_CONTAINER_ITEM_PREFIX, _data_->_tmp21_, NULL);
			_data_->_tmp23_ = NULL;
			_data_->_tmp23_ = _data_->_tmp22_;
			_g_free0 (_data_->_tmp21_);
			_data_->container_id = _data_->_tmp23_;
			_data_->_tmp24_ = NULL;
			_data_->_tmp24_ = _data_->child;
			_data_->_tmp25_ = NULL;
			_data_->_tmp25_ = _data_->child;
			_data_->_tmp26_ = NULL;
			_data_->_tmp26_ = rygel_media_object_get_id (_data_->_tmp25_);
			_data_->_tmp27_ = NULL;
			_data_->_tmp27_ = _data_->_tmp26_;
			rygel_media_object_set_ref_id (_data_->_tmp24_, _data_->_tmp27_);
			_data_->_tmp28_ = NULL;
			_data_->_tmp28_ = _data_->child;
			_data_->_tmp29_ = NULL;
			_data_->_tmp29_ = _data_->container_id;
			_data_->_tmp30_ = NULL;
			_data_->_tmp30_ = g_strconcat (_data_->_tmp29_, ":", NULL);
			_data_->_tmp31_ = NULL;
			_data_->_tmp31_ = _data_->_tmp30_;
			_data_->_tmp32_ = NULL;
			_data_->_tmp32_ = _data_->child;
			_data_->_tmp33_ = NULL;
			_data_->_tmp33_ = rygel_media_object_get_ref_id (_data_->_tmp32_);
			_data_->_tmp34_ = NULL;
			_data_->_tmp34_ = _data_->_tmp33_;
			_data_->_tmp35_ = NULL;
			_data_->_tmp35_ = g_strconcat (_data_->_tmp31_, _data_->_tmp34_, NULL);
			_data_->_tmp36_ = NULL;
			_data_->_tmp36_ = _data_->_tmp35_;
			rygel_media_object_set_id (_data_->_tmp28_, _data_->_tmp36_);
			_g_free0 (_data_->_tmp36_);
			_g_free0 (_data_->_tmp31_);
			_data_->_tmp37_ = NULL;
			_data_->_tmp37_ = _data_->child;
			rygel_media_object_set_parent_ref (_data_->_tmp37_, (RygelMediaContainer*) _data_->self);
			_g_free0 (_data_->container_id);
			_g_object_unref0 (_data_->child);
		}
		_g_object_unref0 (_data_->_child_list);
	}
	_data_->result = _data_->children;
	if (_data_->_state_ == 0) {
		g_simple_async_result_complete_in_idle (_data_->_async_result);
	} else {
		g_simple_async_result_complete (_data_->_async_result);
	}
	g_object_unref (_data_->_async_result);
	return FALSE;
	_g_object_unref0 (_data_->children);
	if (_data_->_state_ == 0) {
		g_simple_async_result_complete_in_idle (_data_->_async_result);
	} else {
		g_simple_async_result_complete (_data_->_async_result);
	}
	g_object_unref (_data_->_async_result);
	return FALSE;
}


static gint rygel_media_export_leaf_query_container_real_count_children (RygelMediaExportDBContainer* base) {
	RygelMediaExportLeafQueryContainer * self;
	gint result = 0;
	GError * _inner_error_ = NULL;
	self = (RygelMediaExportLeafQueryContainer*) base;
	{
		glong _tmp0_ = 0L;
		RygelMediaExportMediaCache* _tmp1_ = NULL;
		RygelSearchExpression* _tmp2_ = NULL;
		RygelSearchExpression* _tmp3_ = NULL;
		glong _tmp4_ = 0L;
		_tmp1_ = ((RygelMediaExportDBContainer*) self)->media_db;
		_tmp2_ = rygel_media_export_query_container_get_expression ((RygelMediaExportQueryContainer*) self);
		_tmp3_ = _tmp2_;
		_tmp4_ = rygel_media_export_media_cache_get_object_count_by_search_expression (_tmp1_, _tmp3_, NULL, &_inner_error_);
		_tmp0_ = _tmp4_;
		if (_inner_error_ != NULL) {
			goto __catch56_g_error;
		}
		result = (gint) _tmp0_;
		return result;
	}
	goto __finally56;
	__catch56_g_error:
	{
		GError* _error_ = NULL;
		const gchar* _tmp5_ = NULL;
		GError* _tmp6_ = NULL;
		const gchar* _tmp7_ = NULL;
		_error_ = _inner_error_;
		_inner_error_ = NULL;
		_tmp5_ = _ ("Failed to get child count of query container: %s");
		_tmp6_ = _error_;
		_tmp7_ = _tmp6_->message;
		g_warning (_tmp5_, _tmp7_);
		result = 0;
		_g_error_free0 (_error_);
		return result;
	}
	__finally56:
	g_critical ("file %s: line %d: uncaught error: %s (%s, %d)", __FILE__, __LINE__, _inner_error_->message, g_quark_to_string (_inner_error_->domain), _inner_error_->code);
	g_clear_error (&_inner_error_);
	return 0;
}


static void rygel_media_export_leaf_query_container_class_init (RygelMediaExportLeafQueryContainerClass * klass) {
	rygel_media_export_leaf_query_container_parent_class = g_type_class_peek_parent (klass);
	RYGEL_MEDIA_CONTAINER_CLASS (klass)->get_children = rygel_media_export_leaf_query_container_real_get_children;
	RYGEL_MEDIA_CONTAINER_CLASS (klass)->get_children_finish = rygel_media_export_leaf_query_container_real_get_children_finish;
	RYGEL_MEDIA_EXPORT_DB_CONTAINER_CLASS (klass)->count_children = rygel_media_export_leaf_query_container_real_count_children;
}


static void rygel_media_export_leaf_query_container_instance_init (RygelMediaExportLeafQueryContainer * self) {
}


GType rygel_media_export_leaf_query_container_get_type (void) {
	static volatile gsize rygel_media_export_leaf_query_container_type_id__volatile = 0;
	if (g_once_init_enter (&rygel_media_export_leaf_query_container_type_id__volatile)) {
		static const GTypeInfo g_define_type_info = { sizeof (RygelMediaExportLeafQueryContainerClass), (GBaseInitFunc) NULL, (GBaseFinalizeFunc) NULL, (GClassInitFunc) rygel_media_export_leaf_query_container_class_init, (GClassFinalizeFunc) NULL, NULL, sizeof (RygelMediaExportLeafQueryContainer), 0, (GInstanceInitFunc) rygel_media_export_leaf_query_container_instance_init, NULL };
		GType rygel_media_export_leaf_query_container_type_id;
		rygel_media_export_leaf_query_container_type_id = g_type_register_static (RYGEL_MEDIA_EXPORT_TYPE_QUERY_CONTAINER, "RygelMediaExportLeafQueryContainer", &g_define_type_info, 0);
		g_once_init_leave (&rygel_media_export_leaf_query_container_type_id__volatile, rygel_media_export_leaf_query_container_type_id);
	}
	return rygel_media_export_leaf_query_container_type_id__volatile;
}


