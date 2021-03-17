/* rygel-external-dummy-container.c generated by valac 0.24.0, the Vala compiler
 * generated from rygel-external-dummy-container.vala, do not modify */

/*
 * Copyright (C) 2009,2010 Jens Georg <mail@jensge.org>.
 * Copyright (C) 2010 Nokia Corporation.
 *
 * Author: Jens Georg <mail@jensge.org>
 * Author: Zeeshan Ali (Khattak) <zeeshanak@gnome.org>
 *                               <zeeshan.ali@nokia.com>
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
#include <stdlib.h>
#include <string.h>
#include <gio/gio.h>


#define RYGEL_EXTERNAL_TYPE_DUMMY_CONTAINER (rygel_external_dummy_container_get_type ())
#define RYGEL_EXTERNAL_DUMMY_CONTAINER(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), RYGEL_EXTERNAL_TYPE_DUMMY_CONTAINER, RygelExternalDummyContainer))
#define RYGEL_EXTERNAL_DUMMY_CONTAINER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), RYGEL_EXTERNAL_TYPE_DUMMY_CONTAINER, RygelExternalDummyContainerClass))
#define RYGEL_EXTERNAL_IS_DUMMY_CONTAINER(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), RYGEL_EXTERNAL_TYPE_DUMMY_CONTAINER))
#define RYGEL_EXTERNAL_IS_DUMMY_CONTAINER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), RYGEL_EXTERNAL_TYPE_DUMMY_CONTAINER))
#define RYGEL_EXTERNAL_DUMMY_CONTAINER_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), RYGEL_EXTERNAL_TYPE_DUMMY_CONTAINER, RygelExternalDummyContainerClass))

typedef struct _RygelExternalDummyContainer RygelExternalDummyContainer;
typedef struct _RygelExternalDummyContainerClass RygelExternalDummyContainerClass;
typedef struct _RygelExternalDummyContainerPrivate RygelExternalDummyContainerPrivate;
#define _g_free0(var) (var = (g_free (var), NULL))
#define _g_object_unref0(var) ((var == NULL) ? NULL : (var = (g_object_unref (var), NULL)))
typedef struct _RygelExternalDummyContainerGetChildrenData RygelExternalDummyContainerGetChildrenData;
typedef struct _RygelExternalDummyContainerFindObjectData RygelExternalDummyContainerFindObjectData;

struct _RygelExternalDummyContainer {
	RygelMediaContainer parent_instance;
	RygelExternalDummyContainerPrivate * priv;
};

struct _RygelExternalDummyContainerClass {
	RygelMediaContainerClass parent_class;
};

struct _RygelExternalDummyContainerGetChildrenData {
	int _state_;
	GObject* _source_object_;
	GAsyncResult* _res_;
	GSimpleAsyncResult* _async_result;
	RygelExternalDummyContainer* self;
	guint offset;
	guint max_count;
	gchar* sort_criteria;
	GCancellable* cancellable;
	RygelMediaObjects* result;
	RygelMediaObjects* _tmp0_;
};

struct _RygelExternalDummyContainerFindObjectData {
	int _state_;
	GObject* _source_object_;
	GAsyncResult* _res_;
	GSimpleAsyncResult* _async_result;
	RygelExternalDummyContainer* self;
	gchar* id;
	GCancellable* cancellable;
	RygelMediaObject* result;
};


static gpointer rygel_external_dummy_container_parent_class = NULL;

GType rygel_external_dummy_container_get_type (void) G_GNUC_CONST;
enum  {
	RYGEL_EXTERNAL_DUMMY_CONTAINER_DUMMY_PROPERTY
};
RygelExternalDummyContainer* rygel_external_dummy_container_new (const gchar* id, const gchar* title, guint child_count, RygelMediaContainer* parent);
RygelExternalDummyContainer* rygel_external_dummy_container_construct (GType object_type, const gchar* id, const gchar* title, guint child_count, RygelMediaContainer* parent);
static void rygel_external_dummy_container_real_get_children_data_free (gpointer _data);
static void rygel_external_dummy_container_real_get_children (RygelMediaContainer* base, guint offset, guint max_count, const gchar* sort_criteria, GCancellable* cancellable, GAsyncReadyCallback _callback_, gpointer _user_data_);
static gboolean rygel_external_dummy_container_real_get_children_co (RygelExternalDummyContainerGetChildrenData* _data_);
static void rygel_external_dummy_container_real_find_object_data_free (gpointer _data);
static void rygel_external_dummy_container_real_find_object (RygelMediaContainer* base, const gchar* id, GCancellable* cancellable, GAsyncReadyCallback _callback_, gpointer _user_data_);
static gboolean rygel_external_dummy_container_real_find_object_co (RygelExternalDummyContainerFindObjectData* _data_);


RygelExternalDummyContainer* rygel_external_dummy_container_construct (GType object_type, const gchar* id, const gchar* title, guint child_count, RygelMediaContainer* parent) {
	RygelExternalDummyContainer * self = NULL;
	const gchar* _tmp0_ = NULL;
	RygelMediaContainer* _tmp1_ = NULL;
	const gchar* _tmp2_ = NULL;
	guint _tmp3_ = 0U;
	g_return_val_if_fail (id != NULL, NULL);
	g_return_val_if_fail (title != NULL, NULL);
	_tmp0_ = id;
	_tmp1_ = parent;
	_tmp2_ = title;
	_tmp3_ = child_count;
	self = (RygelExternalDummyContainer*) rygel_media_container_construct (object_type, _tmp0_, _tmp1_, _tmp2_, (gint) _tmp3_);
	return self;
}


RygelExternalDummyContainer* rygel_external_dummy_container_new (const gchar* id, const gchar* title, guint child_count, RygelMediaContainer* parent) {
	return rygel_external_dummy_container_construct (RYGEL_EXTERNAL_TYPE_DUMMY_CONTAINER, id, title, child_count, parent);
}


static void rygel_external_dummy_container_real_get_children_data_free (gpointer _data) {
	RygelExternalDummyContainerGetChildrenData* _data_;
	_data_ = _data;
	_g_free0 (_data_->sort_criteria);
	_g_object_unref0 (_data_->cancellable);
	_g_object_unref0 (_data_->result);
	_g_object_unref0 (_data_->self);
	g_slice_free (RygelExternalDummyContainerGetChildrenData, _data_);
}


static gpointer _g_object_ref0 (gpointer self) {
	return self ? g_object_ref (self) : NULL;
}


static void rygel_external_dummy_container_real_get_children (RygelMediaContainer* base, guint offset, guint max_count, const gchar* sort_criteria, GCancellable* cancellable, GAsyncReadyCallback _callback_, gpointer _user_data_) {
	RygelExternalDummyContainer * self;
	RygelExternalDummyContainerGetChildrenData* _data_;
	RygelExternalDummyContainer* _tmp0_ = NULL;
	guint _tmp1_ = 0U;
	guint _tmp2_ = 0U;
	const gchar* _tmp3_ = NULL;
	gchar* _tmp4_ = NULL;
	GCancellable* _tmp5_ = NULL;
	GCancellable* _tmp6_ = NULL;
	self = (RygelExternalDummyContainer*) base;
	_data_ = g_slice_new0 (RygelExternalDummyContainerGetChildrenData);
	_data_->_async_result = g_simple_async_result_new (G_OBJECT (self), _callback_, _user_data_, rygel_external_dummy_container_real_get_children);
	g_simple_async_result_set_op_res_gpointer (_data_->_async_result, _data_, rygel_external_dummy_container_real_get_children_data_free);
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
	rygel_external_dummy_container_real_get_children_co (_data_);
}


static RygelMediaObjects* rygel_external_dummy_container_real_get_children_finish (RygelMediaContainer* base, GAsyncResult* _res_, GError** error) {
	RygelMediaObjects* result;
	RygelExternalDummyContainerGetChildrenData* _data_;
	if (g_simple_async_result_propagate_error (G_SIMPLE_ASYNC_RESULT (_res_), error)) {
		return NULL;
	}
	_data_ = g_simple_async_result_get_op_res_gpointer (G_SIMPLE_ASYNC_RESULT (_res_));
	result = _data_->result;
	_data_->result = NULL;
	return result;
}


static gboolean rygel_external_dummy_container_real_get_children_co (RygelExternalDummyContainerGetChildrenData* _data_) {
	switch (_data_->_state_) {
		case 0:
		goto _state_0;
		default:
		g_assert_not_reached ();
	}
	_state_0:
	_data_->_tmp0_ = NULL;
	_data_->_tmp0_ = rygel_media_objects_new ();
	_data_->result = _data_->_tmp0_;
	if (_data_->_state_ == 0) {
		g_simple_async_result_complete_in_idle (_data_->_async_result);
	} else {
		g_simple_async_result_complete (_data_->_async_result);
	}
	g_object_unref (_data_->_async_result);
	return FALSE;
	if (_data_->_state_ == 0) {
		g_simple_async_result_complete_in_idle (_data_->_async_result);
	} else {
		g_simple_async_result_complete (_data_->_async_result);
	}
	g_object_unref (_data_->_async_result);
	return FALSE;
}


static void rygel_external_dummy_container_real_find_object_data_free (gpointer _data) {
	RygelExternalDummyContainerFindObjectData* _data_;
	_data_ = _data;
	_g_free0 (_data_->id);
	_g_object_unref0 (_data_->cancellable);
	_g_object_unref0 (_data_->result);
	_g_object_unref0 (_data_->self);
	g_slice_free (RygelExternalDummyContainerFindObjectData, _data_);
}


static void rygel_external_dummy_container_real_find_object (RygelMediaContainer* base, const gchar* id, GCancellable* cancellable, GAsyncReadyCallback _callback_, gpointer _user_data_) {
	RygelExternalDummyContainer * self;
	RygelExternalDummyContainerFindObjectData* _data_;
	RygelExternalDummyContainer* _tmp0_ = NULL;
	const gchar* _tmp1_ = NULL;
	gchar* _tmp2_ = NULL;
	GCancellable* _tmp3_ = NULL;
	GCancellable* _tmp4_ = NULL;
	self = (RygelExternalDummyContainer*) base;
	_data_ = g_slice_new0 (RygelExternalDummyContainerFindObjectData);
	_data_->_async_result = g_simple_async_result_new (G_OBJECT (self), _callback_, _user_data_, rygel_external_dummy_container_real_find_object);
	g_simple_async_result_set_op_res_gpointer (_data_->_async_result, _data_, rygel_external_dummy_container_real_find_object_data_free);
	_tmp0_ = _g_object_ref0 (self);
	_data_->self = _tmp0_;
	_tmp1_ = id;
	_tmp2_ = g_strdup (_tmp1_);
	_g_free0 (_data_->id);
	_data_->id = _tmp2_;
	_tmp3_ = cancellable;
	_tmp4_ = _g_object_ref0 (_tmp3_);
	_g_object_unref0 (_data_->cancellable);
	_data_->cancellable = _tmp4_;
	rygel_external_dummy_container_real_find_object_co (_data_);
}


static RygelMediaObject* rygel_external_dummy_container_real_find_object_finish (RygelMediaContainer* base, GAsyncResult* _res_, GError** error) {
	RygelMediaObject* result;
	RygelExternalDummyContainerFindObjectData* _data_;
	if (g_simple_async_result_propagate_error (G_SIMPLE_ASYNC_RESULT (_res_), error)) {
		return NULL;
	}
	_data_ = g_simple_async_result_get_op_res_gpointer (G_SIMPLE_ASYNC_RESULT (_res_));
	result = _data_->result;
	_data_->result = NULL;
	return result;
}


static gboolean rygel_external_dummy_container_real_find_object_co (RygelExternalDummyContainerFindObjectData* _data_) {
	switch (_data_->_state_) {
		case 0:
		goto _state_0;
		default:
		g_assert_not_reached ();
	}
	_state_0:
	_data_->result = NULL;
	if (_data_->_state_ == 0) {
		g_simple_async_result_complete_in_idle (_data_->_async_result);
	} else {
		g_simple_async_result_complete (_data_->_async_result);
	}
	g_object_unref (_data_->_async_result);
	return FALSE;
	if (_data_->_state_ == 0) {
		g_simple_async_result_complete_in_idle (_data_->_async_result);
	} else {
		g_simple_async_result_complete (_data_->_async_result);
	}
	g_object_unref (_data_->_async_result);
	return FALSE;
}


static void rygel_external_dummy_container_class_init (RygelExternalDummyContainerClass * klass) {
	rygel_external_dummy_container_parent_class = g_type_class_peek_parent (klass);
	RYGEL_MEDIA_CONTAINER_CLASS (klass)->get_children = rygel_external_dummy_container_real_get_children;
	RYGEL_MEDIA_CONTAINER_CLASS (klass)->get_children_finish = rygel_external_dummy_container_real_get_children_finish;
	RYGEL_MEDIA_CONTAINER_CLASS (klass)->find_object = rygel_external_dummy_container_real_find_object;
	RYGEL_MEDIA_CONTAINER_CLASS (klass)->find_object_finish = rygel_external_dummy_container_real_find_object_finish;
}


static void rygel_external_dummy_container_instance_init (RygelExternalDummyContainer * self) {
}


/**
 * This is a dummy container used to satisfy rygel when creating objects for
 * search resuts.
 */
GType rygel_external_dummy_container_get_type (void) {
	static volatile gsize rygel_external_dummy_container_type_id__volatile = 0;
	if (g_once_init_enter (&rygel_external_dummy_container_type_id__volatile)) {
		static const GTypeInfo g_define_type_info = { sizeof (RygelExternalDummyContainerClass), (GBaseInitFunc) NULL, (GBaseFinalizeFunc) NULL, (GClassInitFunc) rygel_external_dummy_container_class_init, (GClassFinalizeFunc) NULL, NULL, sizeof (RygelExternalDummyContainer), 0, (GInstanceInitFunc) rygel_external_dummy_container_instance_init, NULL };
		GType rygel_external_dummy_container_type_id;
		rygel_external_dummy_container_type_id = g_type_register_static (RYGEL_TYPE_MEDIA_CONTAINER, "RygelExternalDummyContainer", &g_define_type_info, 0);
		g_once_init_leave (&rygel_external_dummy_container_type_id__volatile, rygel_external_dummy_container_type_id);
	}
	return rygel_external_dummy_container_type_id__volatile;
}


