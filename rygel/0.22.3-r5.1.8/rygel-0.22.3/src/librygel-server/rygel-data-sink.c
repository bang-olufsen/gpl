/* rygel-data-sink.c generated by valac 0.24.0, the Vala compiler
 * generated from rygel-data-sink.vala, do not modify */

/*
 * Copyright (C) 2012 Intel Corporation.
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
#include <libsoup/soup.h>


#define RYGEL_TYPE_DATA_SINK (rygel_data_sink_get_type ())
#define RYGEL_DATA_SINK(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), RYGEL_TYPE_DATA_SINK, RygelDataSink))
#define RYGEL_DATA_SINK_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), RYGEL_TYPE_DATA_SINK, RygelDataSinkClass))
#define RYGEL_IS_DATA_SINK(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), RYGEL_TYPE_DATA_SINK))
#define RYGEL_IS_DATA_SINK_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), RYGEL_TYPE_DATA_SINK))
#define RYGEL_DATA_SINK_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), RYGEL_TYPE_DATA_SINK, RygelDataSinkClass))

typedef struct _RygelDataSink RygelDataSink;
typedef struct _RygelDataSinkClass RygelDataSinkClass;
typedef struct _RygelDataSinkPrivate RygelDataSinkPrivate;

#define RYGEL_TYPE_DATA_SOURCE (rygel_data_source_get_type ())
#define RYGEL_DATA_SOURCE(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), RYGEL_TYPE_DATA_SOURCE, RygelDataSource))
#define RYGEL_IS_DATA_SOURCE(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), RYGEL_TYPE_DATA_SOURCE))
#define RYGEL_DATA_SOURCE_GET_INTERFACE(obj) (G_TYPE_INSTANCE_GET_INTERFACE ((obj), RYGEL_TYPE_DATA_SOURCE, RygelDataSourceIface))

typedef struct _RygelDataSource RygelDataSource;
typedef struct _RygelDataSourceIface RygelDataSourceIface;

#define RYGEL_TYPE_HTTP_SEEK (rygel_http_seek_get_type ())
#define RYGEL_HTTP_SEEK(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), RYGEL_TYPE_HTTP_SEEK, RygelHTTPSeek))
#define RYGEL_HTTP_SEEK_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), RYGEL_TYPE_HTTP_SEEK, RygelHTTPSeekClass))
#define RYGEL_IS_HTTP_SEEK(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), RYGEL_TYPE_HTTP_SEEK))
#define RYGEL_IS_HTTP_SEEK_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), RYGEL_TYPE_HTTP_SEEK))
#define RYGEL_HTTP_SEEK_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), RYGEL_TYPE_HTTP_SEEK, RygelHTTPSeekClass))

typedef struct _RygelHTTPSeek RygelHTTPSeek;
typedef struct _RygelHTTPSeekClass RygelHTTPSeekClass;
#define _g_object_unref0(var) ((var == NULL) ? NULL : (var = (g_object_unref (var), NULL)))

#define RYGEL_TYPE_HTTP_BYTE_SEEK (rygel_http_byte_seek_get_type ())
#define RYGEL_HTTP_BYTE_SEEK(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), RYGEL_TYPE_HTTP_BYTE_SEEK, RygelHTTPByteSeek))
#define RYGEL_HTTP_BYTE_SEEK_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), RYGEL_TYPE_HTTP_BYTE_SEEK, RygelHTTPByteSeekClass))
#define RYGEL_IS_HTTP_BYTE_SEEK(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), RYGEL_TYPE_HTTP_BYTE_SEEK))
#define RYGEL_IS_HTTP_BYTE_SEEK_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), RYGEL_TYPE_HTTP_BYTE_SEEK))
#define RYGEL_HTTP_BYTE_SEEK_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), RYGEL_TYPE_HTTP_BYTE_SEEK, RygelHTTPByteSeekClass))

typedef struct _RygelHTTPByteSeek RygelHTTPByteSeek;
typedef struct _RygelHTTPByteSeekClass RygelHTTPByteSeekClass;

struct _RygelDataSink {
	GObject parent_instance;
	RygelDataSinkPrivate * priv;
};

struct _RygelDataSinkClass {
	GObjectClass parent_class;
};

struct _RygelDataSourceIface {
	GTypeInterface parent_iface;
	void (*start) (RygelDataSource* self, RygelHTTPSeek* offsets, GError** error);
	void (*freeze) (RygelDataSource* self);
	void (*thaw) (RygelDataSource* self);
	void (*stop) (RygelDataSource* self);
};

struct _RygelDataSinkPrivate {
	RygelDataSource* source;
	SoupServer* server;
	SoupMessage* message;
	gint64 chunks_buffered;
	gint64 bytes_sent;
	gint64 max_bytes;
};


static gpointer rygel_data_sink_parent_class = NULL;

GType rygel_data_sink_get_type (void) G_GNUC_CONST;
GType rygel_http_seek_get_type (void) G_GNUC_CONST;
GType rygel_data_source_get_type (void) G_GNUC_CONST;
#define RYGEL_DATA_SINK_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), RYGEL_TYPE_DATA_SINK, RygelDataSinkPrivate))
enum  {
	RYGEL_DATA_SINK_DUMMY_PROPERTY
};
#define RYGEL_DATA_SINK_MAX_BUFFERED_CHUNKS ((guint) 32)
#define RYGEL_DATA_SINK_MIN_BUFFERED_CHUNKS ((guint) 4)
RygelDataSink* rygel_data_sink_new (RygelDataSource* source, SoupServer* server, SoupMessage* message, RygelHTTPSeek* offsets);
RygelDataSink* rygel_data_sink_construct (GType object_type, RygelDataSource* source, SoupServer* server, SoupMessage* message, RygelHTTPSeek* offsets);
GType rygel_http_byte_seek_get_type (void) G_GNUC_CONST;
gint64 rygel_http_seek_get_length (RygelHTTPSeek* self);
static void rygel_data_sink_on_data_available (RygelDataSink* self, guint8* buffer, int buffer_length1);
static void _rygel_data_sink_on_data_available_rygel_data_source_data_available (RygelDataSource* _sender, guint8* data, int data_length1, gpointer self);
static void rygel_data_sink_on_wrote_chunk (RygelDataSink* self, SoupMessage* msg);
static void _rygel_data_sink_on_wrote_chunk_soup_message_wrote_chunk (SoupMessage* _sender, gpointer self);
void rygel_data_source_thaw (RygelDataSource* self);
void rygel_data_source_freeze (RygelDataSource* self);
static void rygel_data_sink_finalize (GObject* obj);


static gpointer _g_object_ref0 (gpointer self) {
	return self ? g_object_ref (self) : NULL;
}


static void _rygel_data_sink_on_data_available_rygel_data_source_data_available (RygelDataSource* _sender, guint8* data, int data_length1, gpointer self) {
	rygel_data_sink_on_data_available ((RygelDataSink*) self, data, data_length1);
}


static void _rygel_data_sink_on_wrote_chunk_soup_message_wrote_chunk (SoupMessage* _sender, gpointer self) {
	rygel_data_sink_on_wrote_chunk ((RygelDataSink*) self, _sender);
}


RygelDataSink* rygel_data_sink_construct (GType object_type, RygelDataSource* source, SoupServer* server, SoupMessage* message, RygelHTTPSeek* offsets) {
	RygelDataSink * self = NULL;
	RygelDataSource* _tmp0_ = NULL;
	RygelDataSource* _tmp1_ = NULL;
	SoupServer* _tmp2_ = NULL;
	SoupServer* _tmp3_ = NULL;
	SoupMessage* _tmp4_ = NULL;
	SoupMessage* _tmp5_ = NULL;
	gint64 _tmp6_ = 0LL;
	gboolean _tmp7_ = FALSE;
	RygelHTTPSeek* _tmp8_ = NULL;
	RygelDataSource* _tmp13_ = NULL;
	SoupMessage* _tmp14_ = NULL;
	g_return_val_if_fail (source != NULL, NULL);
	g_return_val_if_fail (server != NULL, NULL);
	g_return_val_if_fail (message != NULL, NULL);
	self = (RygelDataSink*) g_object_new (object_type, NULL);
	_tmp0_ = source;
	_tmp1_ = _g_object_ref0 (_tmp0_);
	_g_object_unref0 (self->priv->source);
	self->priv->source = _tmp1_;
	_tmp2_ = server;
	_tmp3_ = _g_object_ref0 (_tmp2_);
	_g_object_unref0 (self->priv->server);
	self->priv->server = _tmp3_;
	_tmp4_ = message;
	_tmp5_ = _g_object_ref0 (_tmp4_);
	_g_object_unref0 (self->priv->message);
	self->priv->message = _tmp5_;
	self->priv->chunks_buffered = (gint64) 0;
	self->priv->bytes_sent = (gint64) 0;
	_tmp6_ = G_MAXINT64;
	self->priv->max_bytes = _tmp6_;
	_tmp8_ = offsets;
	if (_tmp8_ != NULL) {
		RygelHTTPSeek* _tmp9_ = NULL;
		_tmp9_ = offsets;
		_tmp7_ = G_TYPE_CHECK_INSTANCE_TYPE (_tmp9_, RYGEL_TYPE_HTTP_BYTE_SEEK);
	} else {
		_tmp7_ = FALSE;
	}
	if (_tmp7_) {
		RygelHTTPSeek* _tmp10_ = NULL;
		gint64 _tmp11_ = 0LL;
		gint64 _tmp12_ = 0LL;
		_tmp10_ = offsets;
		_tmp11_ = rygel_http_seek_get_length (_tmp10_);
		_tmp12_ = _tmp11_;
		self->priv->max_bytes = _tmp12_;
	}
	_tmp13_ = self->priv->source;
	g_signal_connect_object (_tmp13_, "data-available", (GCallback) _rygel_data_sink_on_data_available_rygel_data_source_data_available, self, 0);
	_tmp14_ = self->priv->message;
	g_signal_connect_object (_tmp14_, "wrote-chunk", (GCallback) _rygel_data_sink_on_wrote_chunk_soup_message_wrote_chunk, self, 0);
	return self;
}


RygelDataSink* rygel_data_sink_new (RygelDataSource* source, SoupServer* server, SoupMessage* message, RygelHTTPSeek* offsets) {
	return rygel_data_sink_construct (RYGEL_TYPE_DATA_SINK, source, server, message, offsets);
}


static void rygel_data_sink_on_wrote_chunk (RygelDataSink* self, SoupMessage* msg) {
	gint64 _tmp0_ = 0LL;
	gint64 _tmp1_ = 0LL;
	g_return_if_fail (self != NULL);
	g_return_if_fail (msg != NULL);
	_tmp0_ = self->priv->chunks_buffered;
	self->priv->chunks_buffered = _tmp0_ - 1;
	_tmp1_ = self->priv->chunks_buffered;
	if (_tmp1_ < ((gint64) RYGEL_DATA_SINK_MIN_BUFFERED_CHUNKS)) {
		RygelDataSource* _tmp2_ = NULL;
		_tmp2_ = self->priv->source;
		rygel_data_source_thaw (_tmp2_);
	}
}


static void rygel_data_sink_on_data_available (RygelDataSink* self, guint8* buffer, int buffer_length1) {
	gint64 left = 0LL;
	gint64 _tmp0_ = 0LL;
	gint64 _tmp1_ = 0LL;
	gint64 _tmp2_ = 0LL;
	gint64 to_send = 0LL;
	guint8* _tmp3_ = NULL;
	gint _tmp3__length1 = 0;
	gint64 _tmp4_ = 0LL;
	gint64 _tmp5_ = 0LL;
	SoupMessage* _tmp6_ = NULL;
	SoupMessageBody* _tmp7_ = NULL;
	guint8* _tmp8_ = NULL;
	gint _tmp8__length1 = 0;
	gint64 _tmp9_ = 0LL;
	gint64 _tmp10_ = 0LL;
	gint64 _tmp11_ = 0LL;
	gint64 _tmp12_ = 0LL;
	SoupServer* _tmp13_ = NULL;
	SoupMessage* _tmp14_ = NULL;
	gint64 _tmp15_ = 0LL;
	g_return_if_fail (self != NULL);
	_tmp0_ = self->priv->max_bytes;
	_tmp1_ = self->priv->bytes_sent;
	left = _tmp0_ - _tmp1_;
	_tmp2_ = left;
	if (_tmp2_ <= ((gint64) 0)) {
		return;
	}
	_tmp3_ = buffer;
	_tmp3__length1 = buffer_length1;
	_tmp4_ = left;
	_tmp5_ = MIN ((gint64) _tmp3__length1, _tmp4_);
	to_send = _tmp5_;
	_tmp6_ = self->priv->message;
	_tmp7_ = _tmp6_->response_body;
	_tmp8_ = buffer;
	_tmp8__length1 = buffer_length1;
	_tmp9_ = to_send;
	soup_message_body_append (_tmp7_, SOUP_MEMORY_COPY, _tmp8_ + 0, (gsize) (((gint) _tmp9_) - 0));
	_tmp10_ = self->priv->chunks_buffered;
	self->priv->chunks_buffered = _tmp10_ + 1;
	_tmp11_ = self->priv->bytes_sent;
	_tmp12_ = to_send;
	self->priv->bytes_sent = _tmp11_ + _tmp12_;
	_tmp13_ = self->priv->server;
	_tmp14_ = self->priv->message;
	soup_server_unpause_message (_tmp13_, _tmp14_);
	_tmp15_ = self->priv->chunks_buffered;
	if (_tmp15_ > ((gint64) RYGEL_DATA_SINK_MAX_BUFFERED_CHUNKS)) {
		RygelDataSource* _tmp16_ = NULL;
		_tmp16_ = self->priv->source;
		rygel_data_source_freeze (_tmp16_);
	}
}


static void rygel_data_sink_class_init (RygelDataSinkClass * klass) {
	rygel_data_sink_parent_class = g_type_class_peek_parent (klass);
	g_type_class_add_private (klass, sizeof (RygelDataSinkPrivate));
	G_OBJECT_CLASS (klass)->finalize = rygel_data_sink_finalize;
}


static void rygel_data_sink_instance_init (RygelDataSink * self) {
	self->priv = RYGEL_DATA_SINK_GET_PRIVATE (self);
}


static void rygel_data_sink_finalize (GObject* obj) {
	RygelDataSink * self;
	self = G_TYPE_CHECK_INSTANCE_CAST (obj, RYGEL_TYPE_DATA_SINK, RygelDataSink);
	_g_object_unref0 (self->priv->source);
	_g_object_unref0 (self->priv->server);
	_g_object_unref0 (self->priv->message);
	G_OBJECT_CLASS (rygel_data_sink_parent_class)->finalize (obj);
}


/**
 * Class that converts the push DataSource into the pull required by libsoup.
 */
GType rygel_data_sink_get_type (void) {
	static volatile gsize rygel_data_sink_type_id__volatile = 0;
	if (g_once_init_enter (&rygel_data_sink_type_id__volatile)) {
		static const GTypeInfo g_define_type_info = { sizeof (RygelDataSinkClass), (GBaseInitFunc) NULL, (GBaseFinalizeFunc) NULL, (GClassInitFunc) rygel_data_sink_class_init, (GClassFinalizeFunc) NULL, NULL, sizeof (RygelDataSink), 0, (GInstanceInitFunc) rygel_data_sink_instance_init, NULL };
		GType rygel_data_sink_type_id;
		rygel_data_sink_type_id = g_type_register_static (G_TYPE_OBJECT, "RygelDataSink", &g_define_type_info, 0);
		g_once_init_leave (&rygel_data_sink_type_id__volatile, rygel_data_sink_type_id);
	}
	return rygel_data_sink_type_id__volatile;
}


