/* rygel-reference-creator.c generated by valac 0.24.0, the Vala compiler
 * generated from rygel-reference-creator.vala, do not modify */

/*
 * Copyright (C) 2013 Intel Corporation.
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
#include <rygel-core.h>
#include <stdlib.h>
#include <string.h>
#include <libgupnp/gupnp.h>
#include <gio/gio.h>
#include <glib/gi18n-lib.h>
#include <gee.h>


#define RYGEL_TYPE_REFERENCE_CREATOR (rygel_reference_creator_get_type ())
#define RYGEL_REFERENCE_CREATOR(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), RYGEL_TYPE_REFERENCE_CREATOR, RygelReferenceCreator))
#define RYGEL_REFERENCE_CREATOR_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), RYGEL_TYPE_REFERENCE_CREATOR, RygelReferenceCreatorClass))
#define RYGEL_IS_REFERENCE_CREATOR(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), RYGEL_TYPE_REFERENCE_CREATOR))
#define RYGEL_IS_REFERENCE_CREATOR_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), RYGEL_TYPE_REFERENCE_CREATOR))
#define RYGEL_REFERENCE_CREATOR_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), RYGEL_TYPE_REFERENCE_CREATOR, RygelReferenceCreatorClass))

typedef struct _RygelReferenceCreator RygelReferenceCreator;
typedef struct _RygelReferenceCreatorClass RygelReferenceCreatorClass;
typedef struct _RygelReferenceCreatorPrivate RygelReferenceCreatorPrivate;

#define RYGEL_TYPE_CONTENT_DIRECTORY (rygel_content_directory_get_type ())
#define RYGEL_CONTENT_DIRECTORY(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), RYGEL_TYPE_CONTENT_DIRECTORY, RygelContentDirectory))
#define RYGEL_CONTENT_DIRECTORY_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), RYGEL_TYPE_CONTENT_DIRECTORY, RygelContentDirectoryClass))
#define RYGEL_IS_CONTENT_DIRECTORY(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), RYGEL_TYPE_CONTENT_DIRECTORY))
#define RYGEL_IS_CONTENT_DIRECTORY_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), RYGEL_TYPE_CONTENT_DIRECTORY))
#define RYGEL_CONTENT_DIRECTORY_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), RYGEL_TYPE_CONTENT_DIRECTORY, RygelContentDirectoryClass))

typedef struct _RygelContentDirectory RygelContentDirectory;
typedef struct _RygelContentDirectoryClass RygelContentDirectoryClass;
#define _g_object_unref0(var) ((var == NULL) ? NULL : (var = (g_object_unref (var), NULL)))
#define __vala_GUPnPServiceAction_free0(var) ((var == NULL) ? NULL : (var = (_vala_GUPnPServiceAction_free (var), NULL)))
#define _g_free0(var) (var = (g_free (var), NULL))
typedef struct _RygelContentDirectoryPrivate RygelContentDirectoryPrivate;

#define RYGEL_TYPE_TRANSCODE_MANAGER (rygel_transcode_manager_get_type ())
#define RYGEL_TRANSCODE_MANAGER(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), RYGEL_TYPE_TRANSCODE_MANAGER, RygelTranscodeManager))
#define RYGEL_TRANSCODE_MANAGER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), RYGEL_TYPE_TRANSCODE_MANAGER, RygelTranscodeManagerClass))
#define RYGEL_IS_TRANSCODE_MANAGER(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), RYGEL_TYPE_TRANSCODE_MANAGER))
#define RYGEL_IS_TRANSCODE_MANAGER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), RYGEL_TYPE_TRANSCODE_MANAGER))
#define RYGEL_TRANSCODE_MANAGER_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), RYGEL_TYPE_TRANSCODE_MANAGER, RygelTranscodeManagerClass))

typedef struct _RygelTranscodeManager RygelTranscodeManager;
typedef struct _RygelTranscodeManagerClass RygelTranscodeManagerClass;

#define RYGEL_TYPE_HTTP_SERVER (rygel_http_server_get_type ())
#define RYGEL_HTTP_SERVER(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), RYGEL_TYPE_HTTP_SERVER, RygelHTTPServer))
#define RYGEL_HTTP_SERVER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), RYGEL_TYPE_HTTP_SERVER, RygelHTTPServerClass))
#define RYGEL_IS_HTTP_SERVER(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), RYGEL_TYPE_HTTP_SERVER))
#define RYGEL_IS_HTTP_SERVER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), RYGEL_TYPE_HTTP_SERVER))
#define RYGEL_HTTP_SERVER_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), RYGEL_TYPE_HTTP_SERVER, RygelHTTPServerClass))

typedef struct _RygelHTTPServer RygelHTTPServer;
typedef struct _RygelHTTPServerClass RygelHTTPServerClass;

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

#define RYGEL_TYPE_WRITABLE_CONTAINER (rygel_writable_container_get_type ())
#define RYGEL_WRITABLE_CONTAINER(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), RYGEL_TYPE_WRITABLE_CONTAINER, RygelWritableContainer))
#define RYGEL_IS_WRITABLE_CONTAINER(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), RYGEL_TYPE_WRITABLE_CONTAINER))
#define RYGEL_WRITABLE_CONTAINER_GET_INTERFACE(obj) (G_TYPE_INSTANCE_GET_INTERFACE ((obj), RYGEL_TYPE_WRITABLE_CONTAINER, RygelWritableContainerIface))

typedef struct _RygelWritableContainer RygelWritableContainer;
typedef struct _RygelWritableContainerIface RygelWritableContainerIface;

#define RYGEL_TYPE_MEDIA_ITEM (rygel_media_item_get_type ())
#define RYGEL_MEDIA_ITEM(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), RYGEL_TYPE_MEDIA_ITEM, RygelMediaItem))
#define RYGEL_MEDIA_ITEM_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), RYGEL_TYPE_MEDIA_ITEM, RygelMediaItemClass))
#define RYGEL_IS_MEDIA_ITEM(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), RYGEL_TYPE_MEDIA_ITEM))
#define RYGEL_IS_MEDIA_ITEM_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), RYGEL_TYPE_MEDIA_ITEM))
#define RYGEL_MEDIA_ITEM_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), RYGEL_TYPE_MEDIA_ITEM, RygelMediaItemClass))

typedef struct _RygelMediaItem RygelMediaItem;
typedef struct _RygelMediaItemClass RygelMediaItemClass;
#define _g_error_free0(var) ((var == NULL) ? NULL : (var = (g_error_free (var), NULL)))
typedef struct _RygelReferenceCreatorRunData RygelReferenceCreatorRunData;
typedef struct _RygelReferenceCreatorFetchContainerData RygelReferenceCreatorFetchContainerData;

struct _RygelReferenceCreator {
	GObject parent_instance;
	RygelReferenceCreatorPrivate * priv;
	gchar* container_id;
	gchar* object_id;
};

struct _RygelReferenceCreatorClass {
	GObjectClass parent_class;
};

struct _RygelReferenceCreatorPrivate {
	RygelContentDirectory* content_directory;
	GUPnPServiceAction* action;
	GCancellable* _cancellable;
};

struct _RygelContentDirectory {
	GUPnPService parent_instance;
	RygelContentDirectoryPrivate * priv;
	gchar* feature_list;
	RygelHTTPServer* http_server;
	RygelMediaContainer* root_container;
	GCancellable* cancellable;
	guint32 system_update_id;
};

struct _RygelContentDirectoryClass {
	GUPnPServiceClass parent_class;
};

typedef enum  {
	RYGEL_CONTENT_DIRECTORY_ERROR_NO_SUCH_OBJECT = 701,
	RYGEL_CONTENT_DIRECTORY_ERROR_INVALID_CURRENT_TAG_VALUE = 702,
	RYGEL_CONTENT_DIRECTORY_ERROR_INVALID_NEW_TAG_VALUE = 703,
	RYGEL_CONTENT_DIRECTORY_ERROR_REQUIRED_TAG = 704,
	RYGEL_CONTENT_DIRECTORY_ERROR_READ_ONLY_TAG = 705,
	RYGEL_CONTENT_DIRECTORY_ERROR_PARAMETER_MISMATCH = 706,
	RYGEL_CONTENT_DIRECTORY_ERROR_INVALID_SEARCH_CRITERIA = 708,
	RYGEL_CONTENT_DIRECTORY_ERROR_INVALID_SORT_CRITERIA = 709,
	RYGEL_CONTENT_DIRECTORY_ERROR_NO_SUCH_CONTAINER = 710,
	RYGEL_CONTENT_DIRECTORY_ERROR_RESTRICTED_OBJECT = 711,
	RYGEL_CONTENT_DIRECTORY_ERROR_BAD_METADATA = 712,
	RYGEL_CONTENT_DIRECTORY_ERROR_RESTRICTED_PARENT = 713,
	RYGEL_CONTENT_DIRECTORY_ERROR_NO_SUCH_FILE_TRANSFER = 717,
	RYGEL_CONTENT_DIRECTORY_ERROR_NO_SUCH_DESTINATION_RESOURCE = 718,
	RYGEL_CONTENT_DIRECTORY_ERROR_CANT_PROCESS = 720,
	RYGEL_CONTENT_DIRECTORY_ERROR_OUTDATED_OBJECT_METADATA = 728,
	RYGEL_CONTENT_DIRECTORY_ERROR_INVALID_ARGS = 402
} RygelContentDirectoryError;
#define RYGEL_CONTENT_DIRECTORY_ERROR rygel_content_directory_error_quark ()
struct _RygelWritableContainerIface {
	GTypeInterface parent_iface;
	void (*add_item) (RygelWritableContainer* self, RygelMediaItem* item, GCancellable* cancellable, GAsyncReadyCallback _callback_, gpointer _user_data_);
	void (*add_item_finish) (RygelWritableContainer* self, GAsyncResult* _res_, GError** error);
	void (*add_container) (RygelWritableContainer* self, RygelMediaContainer* container, GCancellable* cancellable, GAsyncReadyCallback _callback_, gpointer _user_data_);
	void (*add_container_finish) (RygelWritableContainer* self, GAsyncResult* _res_, GError** error);
	void (*add_reference) (RygelWritableContainer* self, RygelMediaObject* object, GCancellable* cancellable, GAsyncReadyCallback _callback_, gpointer _user_data_);
	gchar* (*add_reference_finish) (RygelWritableContainer* self, GAsyncResult* _res_, GError** error);
	void (*remove_item) (RygelWritableContainer* self, const gchar* id, GCancellable* cancellable, GAsyncReadyCallback _callback_, gpointer _user_data_);
	void (*remove_item_finish) (RygelWritableContainer* self, GAsyncResult* _res_, GError** error);
	void (*remove_container) (RygelWritableContainer* self, const gchar* id, GCancellable* cancellable, GAsyncReadyCallback _callback_, gpointer _user_data_);
	void (*remove_container_finish) (RygelWritableContainer* self, GAsyncResult* _res_, GError** error);
	GeeArrayList* (*get_create_classes) (RygelWritableContainer* self);
	void (*set_create_classes) (RygelWritableContainer* self, GeeArrayList* value);
};

struct _RygelReferenceCreatorRunData {
	int _state_;
	GObject* _source_object_;
	GAsyncResult* _res_;
	GSimpleAsyncResult* _async_result;
	RygelReferenceCreator* self;
	RygelMediaContainer* root_container;
	RygelContentDirectory* _tmp0_;
	RygelMediaContainer* _tmp1_;
	RygelMediaContainer* _tmp2_;
	RygelMediaObject* object;
	RygelMediaContainer* _tmp3_;
	const gchar* _tmp4_;
	GCancellable* _tmp5_;
	GCancellable* _tmp6_;
	RygelMediaObject* _tmp7_;
	RygelMediaObject* _tmp8_;
	const gchar* _tmp9_;
	GError* _tmp10_;
	RygelWritableContainer* container;
	RygelWritableContainer* _tmp11_;
	gchar* new_id;
	RygelWritableContainer* _tmp12_;
	RygelMediaObject* _tmp13_;
	GCancellable* _tmp14_;
	GCancellable* _tmp15_;
	gchar* _tmp16_;
	GUPnPServiceAction* _tmp17_;
	const gchar* _tmp18_;
	GUPnPServiceAction* _tmp19_;
	GError* _error_;
	GError* _tmp20_;
	GUPnPServiceAction* _tmp21_;
	GError* _tmp22_;
	gint _tmp23_;
	GError* _tmp24_;
	const gchar* _tmp25_;
	GUPnPServiceAction* _tmp26_;
	GError* _tmp27_;
	const gchar* _tmp28_;
	const gchar* _tmp29_;
	const gchar* _tmp30_;
	GError* _tmp31_;
	const gchar* _tmp32_;
	GError * _inner_error_;
};

struct _RygelReferenceCreatorFetchContainerData {
	int _state_;
	GObject* _source_object_;
	GAsyncResult* _res_;
	GSimpleAsyncResult* _async_result;
	RygelReferenceCreator* self;
	RygelWritableContainer* result;
	RygelMediaObject* media_object;
	RygelMediaContainer* root_container;
	RygelContentDirectory* _tmp0_;
	RygelMediaContainer* _tmp1_;
	RygelMediaContainer* _tmp2_;
	RygelMediaObject* _tmp3_;
	RygelMediaContainer* _tmp4_;
	const gchar* _tmp5_;
	GCancellable* _tmp6_;
	GCancellable* _tmp7_;
	RygelMediaObject* _tmp8_;
	RygelMediaObject* _tmp9_;
	gboolean _tmp10_;
	RygelMediaObject* _tmp11_;
	RygelMediaObject* _tmp12_;
	const gchar* _tmp13_;
	GError* _tmp14_;
	RygelMediaObject* _tmp15_;
	const gchar* _tmp16_;
	RygelMediaObject* _tmp17_;
	const gchar* _tmp18_;
	const gchar* _tmp19_;
	GError* _tmp20_;
	RygelMediaObject* _tmp21_;
	RygelWritableContainer* _tmp22_;
	GError * _inner_error_;
};


static gpointer rygel_reference_creator_parent_class = NULL;
static RygelStateMachineIface* rygel_reference_creator_rygel_state_machine_parent_iface = NULL;

GType rygel_reference_creator_get_type (void) G_GNUC_CONST;
GType rygel_content_directory_get_type (void) G_GNUC_CONST;
#define RYGEL_REFERENCE_CREATOR_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), RYGEL_TYPE_REFERENCE_CREATOR, RygelReferenceCreatorPrivate))
enum  {
	RYGEL_REFERENCE_CREATOR_DUMMY_PROPERTY,
	RYGEL_REFERENCE_CREATOR_CANCELLABLE
};
static void _vala_GUPnPServiceAction_free (GUPnPServiceAction* self);
RygelReferenceCreator* rygel_reference_creator_new (RygelContentDirectory* content_dir, GUPnPServiceAction* action);
RygelReferenceCreator* rygel_reference_creator_construct (GType object_type, RygelContentDirectory* content_dir, GUPnPServiceAction* action);
GType rygel_transcode_manager_get_type (void) G_GNUC_CONST;
GType rygel_http_server_get_type (void) G_GNUC_CONST;
GType rygel_media_object_get_type (void) G_GNUC_CONST;
GType rygel_media_container_get_type (void) G_GNUC_CONST;
void rygel_reference_creator_parse_arguments (RygelReferenceCreator* self, GError** error);
GQuark rygel_content_directory_error_quark (void);
static void rygel_reference_creator_real_run_data_free (gpointer _data);
static void rygel_reference_creator_real_run (RygelStateMachine* base, GAsyncReadyCallback _callback_, gpointer _user_data_);
static gboolean rygel_reference_creator_real_run_co (RygelReferenceCreatorRunData* _data_);
void rygel_media_container_find_object (RygelMediaContainer* self, const gchar* id, GCancellable* cancellable, GAsyncReadyCallback _callback_, gpointer _user_data_);
RygelMediaObject* rygel_media_container_find_object_finish (RygelMediaContainer* self, GAsyncResult* _res_, GError** error);
static void rygel_reference_creator_run_ready (GObject* source_object, GAsyncResult* _res_, gpointer _user_data_);
GType rygel_media_item_get_type (void) G_GNUC_CONST;
GType rygel_writable_container_get_type (void) G_GNUC_CONST;
static void rygel_reference_creator_fetch_container (RygelReferenceCreator* self, GAsyncReadyCallback _callback_, gpointer _user_data_);
static RygelWritableContainer* rygel_reference_creator_fetch_container_finish (RygelReferenceCreator* self, GAsyncResult* _res_, GError** error);
void rygel_writable_container_add_reference (RygelWritableContainer* self, RygelMediaObject* object, GCancellable* cancellable, GAsyncReadyCallback _callback_, gpointer _user_data_);
gchar* rygel_writable_container_add_reference_finish (RygelWritableContainer* self, GAsyncResult* _res_, GError** error);
static void rygel_reference_creator_fetch_container_data_free (gpointer _data);
static gboolean rygel_reference_creator_fetch_container_co (RygelReferenceCreatorFetchContainerData* _data_);
static void rygel_reference_creator_fetch_container_ready (GObject* source_object, GAsyncResult* _res_, gpointer _user_data_);
const gchar* rygel_media_object_get_id (RygelMediaObject* self);
static void rygel_reference_creator_finalize (GObject* obj);
static void _vala_rygel_reference_creator_get_property (GObject * object, guint property_id, GValue * value, GParamSpec * pspec);
static void _vala_rygel_reference_creator_set_property (GObject * object, guint property_id, const GValue * value, GParamSpec * pspec);


static void _vala_GUPnPServiceAction_free (GUPnPServiceAction* self) {
	g_boxed_free (gupnp_service_action_get_type (), self);
}


static gpointer _g_object_ref0 (gpointer self) {
	return self ? g_object_ref (self) : NULL;
}


RygelReferenceCreator* rygel_reference_creator_construct (GType object_type, RygelContentDirectory* content_dir, GUPnPServiceAction* action) {
	RygelReferenceCreator * self = NULL;
	RygelContentDirectory* _tmp0_ = NULL;
	RygelContentDirectory* _tmp1_ = NULL;
	RygelContentDirectory* _tmp2_ = NULL;
	GCancellable* _tmp3_ = NULL;
	GUPnPServiceAction* _tmp4_ = NULL;
	g_return_val_if_fail (content_dir != NULL, NULL);
	g_return_val_if_fail (action != NULL, NULL);
	self = (RygelReferenceCreator*) g_object_new (object_type, NULL);
	_tmp0_ = content_dir;
	_tmp1_ = _g_object_ref0 (_tmp0_);
	_g_object_unref0 (self->priv->content_directory);
	self->priv->content_directory = _tmp1_;
	_tmp2_ = content_dir;
	_tmp3_ = _tmp2_->cancellable;
	rygel_state_machine_set_cancellable ((RygelStateMachine*) self, _tmp3_);
	_tmp4_ = action;
	action = NULL;
	__vala_GUPnPServiceAction_free0 (self->priv->action);
	self->priv->action = _tmp4_;
	__vala_GUPnPServiceAction_free0 (action);
	return self;
}


RygelReferenceCreator* rygel_reference_creator_new (RygelContentDirectory* content_dir, GUPnPServiceAction* action) {
	return rygel_reference_creator_construct (RYGEL_TYPE_REFERENCE_CREATOR, content_dir, action);
}


void rygel_reference_creator_parse_arguments (RygelReferenceCreator* self, GError** error) {
	GUPnPServiceAction* _tmp0_ = NULL;
	const gchar* _tmp1_ = NULL;
	const gchar* _tmp4_ = NULL;
	GError * _inner_error_ = NULL;
	g_return_if_fail (self != NULL);
	_tmp0_ = self->priv->action;
	gupnp_service_action_get (_tmp0_, "ContainerID", G_TYPE_STRING, &self->container_id, "ObjectID", G_TYPE_STRING, &self->object_id, NULL);
	_tmp1_ = self->container_id;
	if (_tmp1_ == NULL) {
		const gchar* _tmp2_ = NULL;
		GError* _tmp3_ = NULL;
		_tmp2_ = _ ("'ContainerID' agument missing.");
		_tmp3_ = g_error_new_literal (RYGEL_CONTENT_DIRECTORY_ERROR, RYGEL_CONTENT_DIRECTORY_ERROR_INVALID_ARGS, _tmp2_);
		_inner_error_ = _tmp3_;
		g_propagate_error (error, _inner_error_);
		return;
	}
	_tmp4_ = self->object_id;
	if (_tmp4_ == NULL) {
		const gchar* _tmp5_ = NULL;
		GError* _tmp6_ = NULL;
		_tmp5_ = _ ("'ObjectID' argument missing.");
		_tmp6_ = g_error_new_literal (RYGEL_CONTENT_DIRECTORY_ERROR, RYGEL_CONTENT_DIRECTORY_ERROR_INVALID_ARGS, _tmp5_);
		_inner_error_ = _tmp6_;
		g_propagate_error (error, _inner_error_);
		return;
	}
}


static void rygel_reference_creator_real_run_data_free (gpointer _data) {
	RygelReferenceCreatorRunData* _data_;
	_data_ = _data;
	_g_object_unref0 (_data_->self);
	g_slice_free (RygelReferenceCreatorRunData, _data_);
}


static void rygel_reference_creator_real_run (RygelStateMachine* base, GAsyncReadyCallback _callback_, gpointer _user_data_) {
	RygelReferenceCreator * self;
	RygelReferenceCreatorRunData* _data_;
	RygelReferenceCreator* _tmp0_ = NULL;
	self = (RygelReferenceCreator*) base;
	_data_ = g_slice_new0 (RygelReferenceCreatorRunData);
	_data_->_async_result = g_simple_async_result_new (G_OBJECT (self), _callback_, _user_data_, rygel_reference_creator_real_run);
	g_simple_async_result_set_op_res_gpointer (_data_->_async_result, _data_, rygel_reference_creator_real_run_data_free);
	_tmp0_ = _g_object_ref0 (self);
	_data_->self = _tmp0_;
	rygel_reference_creator_real_run_co (_data_);
}


static void rygel_reference_creator_real_run_finish (RygelStateMachine* base, GAsyncResult* _res_) {
	RygelReferenceCreatorRunData* _data_;
	_data_ = g_simple_async_result_get_op_res_gpointer (G_SIMPLE_ASYNC_RESULT (_res_));
}


static void rygel_reference_creator_run_ready (GObject* source_object, GAsyncResult* _res_, gpointer _user_data_) {
	RygelReferenceCreatorRunData* _data_;
	_data_ = _user_data_;
	_data_->_source_object_ = source_object;
	_data_->_res_ = _res_;
	rygel_reference_creator_real_run_co (_data_);
}


static gboolean rygel_reference_creator_real_run_co (RygelReferenceCreatorRunData* _data_) {
	switch (_data_->_state_) {
		case 0:
		goto _state_0;
		case 1:
		goto _state_1;
		case 2:
		goto _state_2;
		case 3:
		goto _state_3;
		default:
		g_assert_not_reached ();
	}
	_state_0:
	{
		rygel_reference_creator_parse_arguments (_data_->self, &_data_->_inner_error_);
		if (_data_->_inner_error_ != NULL) {
			goto __catch60_g_error;
		}
		_data_->_tmp0_ = NULL;
		_data_->_tmp0_ = _data_->self->priv->content_directory;
		_data_->_tmp1_ = NULL;
		_data_->_tmp1_ = _data_->_tmp0_->root_container;
		_data_->_tmp2_ = NULL;
		_data_->_tmp2_ = _g_object_ref0 (_data_->_tmp1_);
		_data_->root_container = _data_->_tmp2_;
		_data_->_tmp3_ = NULL;
		_data_->_tmp3_ = _data_->root_container;
		_data_->_tmp4_ = NULL;
		_data_->_tmp4_ = _data_->self->object_id;
		_data_->_tmp5_ = NULL;
		_data_->_tmp5_ = rygel_state_machine_get_cancellable ((RygelStateMachine*) _data_->self);
		_data_->_tmp6_ = NULL;
		_data_->_tmp6_ = _data_->_tmp5_;
		_data_->_state_ = 1;
		rygel_media_container_find_object (_data_->_tmp3_, _data_->_tmp4_, _data_->_tmp6_, rygel_reference_creator_run_ready, _data_);
		return FALSE;
		_state_1:
		_data_->_tmp7_ = NULL;
		_data_->_tmp7_ = rygel_media_container_find_object_finish (_data_->_tmp3_, _data_->_res_, &_data_->_inner_error_);
		_data_->object = _data_->_tmp7_;
		if (_data_->_inner_error_ != NULL) {
			_g_object_unref0 (_data_->root_container);
			goto __catch60_g_error;
		}
		_data_->_tmp8_ = NULL;
		_data_->_tmp8_ = _data_->object;
		if (_data_->_tmp8_ == NULL) {
			_data_->_tmp9_ = NULL;
			_data_->_tmp9_ = _ ("No such object");
			_data_->_tmp10_ = NULL;
			_data_->_tmp10_ = g_error_new_literal (RYGEL_CONTENT_DIRECTORY_ERROR, RYGEL_CONTENT_DIRECTORY_ERROR_NO_SUCH_OBJECT, _data_->_tmp9_);
			_data_->_inner_error_ = _data_->_tmp10_;
			_g_object_unref0 (_data_->object);
			_g_object_unref0 (_data_->root_container);
			goto __catch60_g_error;
		}
		_data_->_state_ = 2;
		rygel_reference_creator_fetch_container (_data_->self, rygel_reference_creator_run_ready, _data_);
		return FALSE;
		_state_2:
		_data_->_tmp11_ = NULL;
		_data_->_tmp11_ = rygel_reference_creator_fetch_container_finish (_data_->self, _data_->_res_, &_data_->_inner_error_);
		_data_->container = _data_->_tmp11_;
		if (_data_->_inner_error_ != NULL) {
			_g_object_unref0 (_data_->object);
			_g_object_unref0 (_data_->root_container);
			goto __catch60_g_error;
		}
		_data_->_tmp12_ = NULL;
		_data_->_tmp12_ = _data_->container;
		_data_->_tmp13_ = NULL;
		_data_->_tmp13_ = _data_->object;
		_data_->_tmp14_ = NULL;
		_data_->_tmp14_ = rygel_state_machine_get_cancellable ((RygelStateMachine*) _data_->self);
		_data_->_tmp15_ = NULL;
		_data_->_tmp15_ = _data_->_tmp14_;
		_data_->_state_ = 3;
		rygel_writable_container_add_reference (_data_->_tmp12_, _data_->_tmp13_, _data_->_tmp15_, rygel_reference_creator_run_ready, _data_);
		return FALSE;
		_state_3:
		_data_->_tmp16_ = NULL;
		_data_->_tmp16_ = rygel_writable_container_add_reference_finish (_data_->_tmp12_, _data_->_res_, &_data_->_inner_error_);
		_data_->new_id = _data_->_tmp16_;
		if (_data_->_inner_error_ != NULL) {
			_g_object_unref0 (_data_->container);
			_g_object_unref0 (_data_->object);
			_g_object_unref0 (_data_->root_container);
			goto __catch60_g_error;
		}
		_data_->_tmp17_ = NULL;
		_data_->_tmp17_ = _data_->self->priv->action;
		_data_->_tmp18_ = NULL;
		_data_->_tmp18_ = _data_->new_id;
		gupnp_service_action_set (_data_->_tmp17_, "NewID", G_TYPE_STRING, _data_->_tmp18_, NULL);
		_data_->_tmp19_ = NULL;
		_data_->_tmp19_ = _data_->self->priv->action;
		gupnp_service_action_return (_data_->_tmp19_);
		g_signal_emit_by_name ((RygelStateMachine*) _data_->self, "completed");
		_g_free0 (_data_->new_id);
		_g_object_unref0 (_data_->container);
		_g_object_unref0 (_data_->object);
		_g_object_unref0 (_data_->root_container);
	}
	goto __finally60;
	__catch60_g_error:
	{
		_data_->_error_ = _data_->_inner_error_;
		_data_->_inner_error_ = NULL;
		_data_->_tmp20_ = NULL;
		_data_->_tmp20_ = _data_->_error_;
		if (_data_->_tmp20_->domain == RYGEL_CONTENT_DIRECTORY_ERROR) {
			_data_->_tmp21_ = NULL;
			_data_->_tmp21_ = _data_->self->priv->action;
			_data_->_tmp22_ = NULL;
			_data_->_tmp22_ = _data_->_error_;
			_data_->_tmp23_ = 0;
			_data_->_tmp23_ = _data_->_tmp22_->code;
			_data_->_tmp24_ = NULL;
			_data_->_tmp24_ = _data_->_error_;
			_data_->_tmp25_ = NULL;
			_data_->_tmp25_ = _data_->_tmp24_->message;
			gupnp_service_action_return_error (_data_->_tmp21_, (guint) _data_->_tmp23_, _data_->_tmp25_);
		} else {
			_data_->_tmp26_ = NULL;
			_data_->_tmp26_ = _data_->self->priv->action;
			_data_->_tmp27_ = NULL;
			_data_->_tmp27_ = _data_->_error_;
			_data_->_tmp28_ = NULL;
			_data_->_tmp28_ = _data_->_tmp27_->message;
			gupnp_service_action_return_error (_data_->_tmp26_, (guint) 402, _data_->_tmp28_);
		}
		_data_->_tmp29_ = NULL;
		_data_->_tmp29_ = _ ("Failed to create object under '%s': %s");
		_data_->_tmp30_ = NULL;
		_data_->_tmp30_ = _data_->self->container_id;
		_data_->_tmp31_ = NULL;
		_data_->_tmp31_ = _data_->_error_;
		_data_->_tmp32_ = NULL;
		_data_->_tmp32_ = _data_->_tmp31_->message;
		g_warning (_data_->_tmp29_, _data_->_tmp30_, _data_->_tmp32_);
		g_signal_emit_by_name ((RygelStateMachine*) _data_->self, "completed");
		_g_error_free0 (_data_->_error_);
		if (_data_->_state_ == 0) {
			g_simple_async_result_complete_in_idle (_data_->_async_result);
		} else {
			g_simple_async_result_complete (_data_->_async_result);
		}
		g_object_unref (_data_->_async_result);
		return FALSE;
	}
	__finally60:
	if (_data_->_inner_error_ != NULL) {
		g_critical ("file %s: line %d: uncaught error: %s (%s, %d)", __FILE__, __LINE__, _data_->_inner_error_->message, g_quark_to_string (_data_->_inner_error_->domain), _data_->_inner_error_->code);
		g_clear_error (&_data_->_inner_error_);
		return FALSE;
	}
	if (_data_->_state_ == 0) {
		g_simple_async_result_complete_in_idle (_data_->_async_result);
	} else {
		g_simple_async_result_complete (_data_->_async_result);
	}
	g_object_unref (_data_->_async_result);
	return FALSE;
}


static void rygel_reference_creator_fetch_container_data_free (gpointer _data) {
	RygelReferenceCreatorFetchContainerData* _data_;
	_data_ = _data;
	_g_object_unref0 (_data_->result);
	_g_object_unref0 (_data_->self);
	g_slice_free (RygelReferenceCreatorFetchContainerData, _data_);
}


static void rygel_reference_creator_fetch_container (RygelReferenceCreator* self, GAsyncReadyCallback _callback_, gpointer _user_data_) {
	RygelReferenceCreatorFetchContainerData* _data_;
	RygelReferenceCreator* _tmp0_ = NULL;
	_data_ = g_slice_new0 (RygelReferenceCreatorFetchContainerData);
	_data_->_async_result = g_simple_async_result_new (G_OBJECT (self), _callback_, _user_data_, rygel_reference_creator_fetch_container);
	g_simple_async_result_set_op_res_gpointer (_data_->_async_result, _data_, rygel_reference_creator_fetch_container_data_free);
	_tmp0_ = _g_object_ref0 (self);
	_data_->self = _tmp0_;
	rygel_reference_creator_fetch_container_co (_data_);
}


static RygelWritableContainer* rygel_reference_creator_fetch_container_finish (RygelReferenceCreator* self, GAsyncResult* _res_, GError** error) {
	RygelWritableContainer* result;
	RygelReferenceCreatorFetchContainerData* _data_;
	if (g_simple_async_result_propagate_error (G_SIMPLE_ASYNC_RESULT (_res_), error)) {
		return NULL;
	}
	_data_ = g_simple_async_result_get_op_res_gpointer (G_SIMPLE_ASYNC_RESULT (_res_));
	result = _data_->result;
	_data_->result = NULL;
	return result;
}


/**
     * Get the container to create the item in.
     *
     * This will either try to fetch the container supplied by the caller or
     * search for a container if the caller supplied the "DLNA.ORG_AnyContainer"
     * id.
     *
     * @return an instance of WritableContainer matching the criteria
     * @throws ContentDirectoryError for various problems
     */
static void rygel_reference_creator_fetch_container_ready (GObject* source_object, GAsyncResult* _res_, gpointer _user_data_) {
	RygelReferenceCreatorFetchContainerData* _data_;
	_data_ = _user_data_;
	_data_->_source_object_ = source_object;
	_data_->_res_ = _res_;
	rygel_reference_creator_fetch_container_co (_data_);
}


static gboolean rygel_reference_creator_fetch_container_co (RygelReferenceCreatorFetchContainerData* _data_) {
	switch (_data_->_state_) {
		case 0:
		goto _state_0;
		case 1:
		goto _state_1;
		default:
		g_assert_not_reached ();
	}
	_state_0:
	_data_->media_object = NULL;
	_data_->_tmp0_ = NULL;
	_data_->_tmp0_ = _data_->self->priv->content_directory;
	_data_->_tmp1_ = NULL;
	_data_->_tmp1_ = _data_->_tmp0_->root_container;
	_data_->_tmp2_ = NULL;
	_data_->_tmp2_ = _g_object_ref0 (_data_->_tmp1_);
	_data_->root_container = _data_->_tmp2_;
	_data_->_tmp4_ = NULL;
	_data_->_tmp4_ = _data_->root_container;
	_data_->_tmp5_ = NULL;
	_data_->_tmp5_ = _data_->self->container_id;
	_data_->_tmp6_ = NULL;
	_data_->_tmp6_ = rygel_state_machine_get_cancellable ((RygelStateMachine*) _data_->self);
	_data_->_tmp7_ = NULL;
	_data_->_tmp7_ = _data_->_tmp6_;
	_data_->_state_ = 1;
	rygel_media_container_find_object (_data_->_tmp4_, _data_->_tmp5_, _data_->_tmp7_, rygel_reference_creator_fetch_container_ready, _data_);
	return FALSE;
	_state_1:
	_data_->_tmp8_ = NULL;
	_data_->_tmp8_ = rygel_media_container_find_object_finish (_data_->_tmp4_, _data_->_res_, &_data_->_inner_error_);
	_data_->_tmp3_ = _data_->_tmp8_;
	if (_data_->_inner_error_ != NULL) {
		g_simple_async_result_set_from_error (_data_->_async_result, _data_->_inner_error_);
		g_error_free (_data_->_inner_error_);
		_g_object_unref0 (_data_->root_container);
		_g_object_unref0 (_data_->media_object);
		if (_data_->_state_ == 0) {
			g_simple_async_result_complete_in_idle (_data_->_async_result);
		} else {
			g_simple_async_result_complete (_data_->_async_result);
		}
		g_object_unref (_data_->_async_result);
		return FALSE;
	}
	_data_->_tmp9_ = NULL;
	_data_->_tmp9_ = _data_->_tmp3_;
	_data_->_tmp3_ = NULL;
	_g_object_unref0 (_data_->media_object);
	_data_->media_object = _data_->_tmp9_;
	_data_->_tmp11_ = NULL;
	_data_->_tmp11_ = _data_->media_object;
	if (_data_->_tmp11_ == NULL) {
		_data_->_tmp10_ = TRUE;
	} else {
		_data_->_tmp12_ = NULL;
		_data_->_tmp12_ = _data_->media_object;
		_data_->_tmp10_ = !G_TYPE_CHECK_INSTANCE_TYPE (_data_->_tmp12_, RYGEL_TYPE_MEDIA_CONTAINER);
	}
	if (_data_->_tmp10_) {
		_data_->_tmp13_ = NULL;
		_data_->_tmp13_ = _ ("No such object");
		_data_->_tmp14_ = NULL;
		_data_->_tmp14_ = g_error_new_literal (RYGEL_CONTENT_DIRECTORY_ERROR, RYGEL_CONTENT_DIRECTORY_ERROR_NO_SUCH_CONTAINER, _data_->_tmp13_);
		_data_->_inner_error_ = _data_->_tmp14_;
		g_simple_async_result_set_from_error (_data_->_async_result, _data_->_inner_error_);
		g_error_free (_data_->_inner_error_);
		_g_object_unref0 (_data_->_tmp3_);
		_g_object_unref0 (_data_->root_container);
		_g_object_unref0 (_data_->media_object);
		if (_data_->_state_ == 0) {
			g_simple_async_result_complete_in_idle (_data_->_async_result);
		} else {
			g_simple_async_result_complete (_data_->_async_result);
		}
		g_object_unref (_data_->_async_result);
		return FALSE;
	} else {
		_data_->_tmp15_ = NULL;
		_data_->_tmp15_ = _data_->media_object;
		if (!G_TYPE_CHECK_INSTANCE_TYPE (_data_->_tmp15_, RYGEL_TYPE_WRITABLE_CONTAINER)) {
			_data_->_tmp16_ = NULL;
			_data_->_tmp16_ = _ ("Object creation in %s not allowed");
			_data_->_tmp17_ = NULL;
			_data_->_tmp17_ = _data_->media_object;
			_data_->_tmp18_ = NULL;
			_data_->_tmp18_ = rygel_media_object_get_id (_data_->_tmp17_);
			_data_->_tmp19_ = NULL;
			_data_->_tmp19_ = _data_->_tmp18_;
			_data_->_tmp20_ = NULL;
			_data_->_tmp20_ = g_error_new (RYGEL_CONTENT_DIRECTORY_ERROR, RYGEL_CONTENT_DIRECTORY_ERROR_RESTRICTED_PARENT, _data_->_tmp16_, _data_->_tmp19_);
			_data_->_inner_error_ = _data_->_tmp20_;
			g_simple_async_result_set_from_error (_data_->_async_result, _data_->_inner_error_);
			g_error_free (_data_->_inner_error_);
			_g_object_unref0 (_data_->_tmp3_);
			_g_object_unref0 (_data_->root_container);
			_g_object_unref0 (_data_->media_object);
			if (_data_->_state_ == 0) {
				g_simple_async_result_complete_in_idle (_data_->_async_result);
			} else {
				g_simple_async_result_complete (_data_->_async_result);
			}
			g_object_unref (_data_->_async_result);
			return FALSE;
		}
	}
	_data_->_tmp21_ = NULL;
	_data_->_tmp21_ = _data_->media_object;
	_data_->_tmp22_ = NULL;
	_data_->_tmp22_ = _g_object_ref0 (G_TYPE_CHECK_INSTANCE_TYPE (_data_->_tmp21_, RYGEL_TYPE_WRITABLE_CONTAINER) ? ((RygelWritableContainer*) _data_->_tmp21_) : NULL);
	_data_->result = _data_->_tmp22_;
	_g_object_unref0 (_data_->_tmp3_);
	_g_object_unref0 (_data_->root_container);
	_g_object_unref0 (_data_->media_object);
	if (_data_->_state_ == 0) {
		g_simple_async_result_complete_in_idle (_data_->_async_result);
	} else {
		g_simple_async_result_complete (_data_->_async_result);
	}
	g_object_unref (_data_->_async_result);
	return FALSE;
	_g_object_unref0 (_data_->_tmp3_);
	_g_object_unref0 (_data_->root_container);
	_g_object_unref0 (_data_->media_object);
	if (_data_->_state_ == 0) {
		g_simple_async_result_complete_in_idle (_data_->_async_result);
	} else {
		g_simple_async_result_complete (_data_->_async_result);
	}
	g_object_unref (_data_->_async_result);
	return FALSE;
}


static GCancellable* rygel_reference_creator_real_get_cancellable (RygelStateMachine* base) {
	GCancellable* result;
	RygelReferenceCreator* self;
	GCancellable* _tmp0_ = NULL;
	self = (RygelReferenceCreator*) base;
	_tmp0_ = self->priv->_cancellable;
	result = _tmp0_;
	return result;
}


static void rygel_reference_creator_real_set_cancellable (RygelStateMachine* base, GCancellable* value) {
	RygelReferenceCreator* self;
	GCancellable* _tmp0_ = NULL;
	GCancellable* _tmp1_ = NULL;
	self = (RygelReferenceCreator*) base;
	_tmp0_ = value;
	_tmp1_ = _g_object_ref0 (_tmp0_);
	_g_object_unref0 (self->priv->_cancellable);
	self->priv->_cancellable = _tmp1_;
	g_object_notify ((GObject *) self, "cancellable");
}


static void rygel_reference_creator_class_init (RygelReferenceCreatorClass * klass) {
	rygel_reference_creator_parent_class = g_type_class_peek_parent (klass);
	g_type_class_add_private (klass, sizeof (RygelReferenceCreatorPrivate));
	G_OBJECT_CLASS (klass)->get_property = _vala_rygel_reference_creator_get_property;
	G_OBJECT_CLASS (klass)->set_property = _vala_rygel_reference_creator_set_property;
	G_OBJECT_CLASS (klass)->finalize = rygel_reference_creator_finalize;
	g_object_class_install_property (G_OBJECT_CLASS (klass), RYGEL_REFERENCE_CREATOR_CANCELLABLE, g_param_spec_object ("cancellable", "cancellable", "cancellable", g_cancellable_get_type (), G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK | G_PARAM_STATIC_BLURB | G_PARAM_READABLE | G_PARAM_WRITABLE));
}


static void rygel_reference_creator_rygel_state_machine_interface_init (RygelStateMachineIface * iface) {
	rygel_reference_creator_rygel_state_machine_parent_iface = g_type_interface_peek_parent (iface);
	iface->run = (void (*)(RygelStateMachine*)) rygel_reference_creator_real_run;
	iface->run_finish = rygel_reference_creator_real_run_finish;
	iface->get_cancellable = rygel_reference_creator_real_get_cancellable;
	iface->set_cancellable = rygel_reference_creator_real_set_cancellable;
}


static void rygel_reference_creator_instance_init (RygelReferenceCreator * self) {
	self->priv = RYGEL_REFERENCE_CREATOR_GET_PRIVATE (self);
}


static void rygel_reference_creator_finalize (GObject* obj) {
	RygelReferenceCreator * self;
	self = G_TYPE_CHECK_INSTANCE_CAST (obj, RYGEL_TYPE_REFERENCE_CREATOR, RygelReferenceCreator);
	_g_object_unref0 (self->priv->content_directory);
	__vala_GUPnPServiceAction_free0 (self->priv->action);
	_g_object_unref0 (self->priv->_cancellable);
	_g_free0 (self->container_id);
	_g_free0 (self->object_id);
	G_OBJECT_CLASS (rygel_reference_creator_parent_class)->finalize (obj);
}


GType rygel_reference_creator_get_type (void) {
	static volatile gsize rygel_reference_creator_type_id__volatile = 0;
	if (g_once_init_enter (&rygel_reference_creator_type_id__volatile)) {
		static const GTypeInfo g_define_type_info = { sizeof (RygelReferenceCreatorClass), (GBaseInitFunc) NULL, (GBaseFinalizeFunc) NULL, (GClassInitFunc) rygel_reference_creator_class_init, (GClassFinalizeFunc) NULL, NULL, sizeof (RygelReferenceCreator), 0, (GInstanceInitFunc) rygel_reference_creator_instance_init, NULL };
		static const GInterfaceInfo rygel_state_machine_info = { (GInterfaceInitFunc) rygel_reference_creator_rygel_state_machine_interface_init, (GInterfaceFinalizeFunc) NULL, NULL};
		GType rygel_reference_creator_type_id;
		rygel_reference_creator_type_id = g_type_register_static (G_TYPE_OBJECT, "RygelReferenceCreator", &g_define_type_info, 0);
		g_type_add_interface_static (rygel_reference_creator_type_id, RYGEL_TYPE_STATE_MACHINE, &rygel_state_machine_info);
		g_once_init_leave (&rygel_reference_creator_type_id__volatile, rygel_reference_creator_type_id);
	}
	return rygel_reference_creator_type_id__volatile;
}


static void _vala_rygel_reference_creator_get_property (GObject * object, guint property_id, GValue * value, GParamSpec * pspec) {
	RygelReferenceCreator * self;
	self = G_TYPE_CHECK_INSTANCE_CAST (object, RYGEL_TYPE_REFERENCE_CREATOR, RygelReferenceCreator);
	switch (property_id) {
		case RYGEL_REFERENCE_CREATOR_CANCELLABLE:
		g_value_set_object (value, rygel_state_machine_get_cancellable ((RygelStateMachine*) self));
		break;
		default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
		break;
	}
}


static void _vala_rygel_reference_creator_set_property (GObject * object, guint property_id, const GValue * value, GParamSpec * pspec) {
	RygelReferenceCreator * self;
	self = G_TYPE_CHECK_INSTANCE_CAST (object, RYGEL_TYPE_REFERENCE_CREATOR, RygelReferenceCreator);
	switch (property_id) {
		case RYGEL_REFERENCE_CREATOR_CANCELLABLE:
		rygel_state_machine_set_cancellable ((RygelStateMachine*) self, g_value_get_object (value));
		break;
		default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
		break;
	}
}


