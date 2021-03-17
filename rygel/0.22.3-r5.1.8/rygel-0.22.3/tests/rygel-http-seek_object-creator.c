/* rygel-http-seek_object-creator.c generated by valac 0.24.0.112-bbf5, the Vala compiler
 * generated from rygel-http-seek_object-creator.vala, do not modify */

/*
 * Copyright (C) 2008-2009 Nokia Corporation.
 * Copyright (C) 2012 Intel Corporation.
 *
 * Author: Zeeshan Ali (Khattak) <zeeshanak@gnome.org>
 *                               <zeeshan.ali@nokia.com>
 *         Jens Georg <jensg@openismus.com>
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
#include <glib/gi18n-lib.h>


#define RYGEL_TYPE_HTTP_SEEK_TYPE (rygel_http_seek_type_get_type ())

#define RYGEL_TYPE_HTTP_SEEK (rygel_http_seek_get_type ())
#define RYGEL_HTTP_SEEK(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), RYGEL_TYPE_HTTP_SEEK, RygelHTTPSeek))
#define RYGEL_HTTP_SEEK_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), RYGEL_TYPE_HTTP_SEEK, RygelHTTPSeekClass))
#define RYGEL_IS_HTTP_SEEK(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), RYGEL_TYPE_HTTP_SEEK))
#define RYGEL_IS_HTTP_SEEK_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), RYGEL_TYPE_HTTP_SEEK))
#define RYGEL_HTTP_SEEK_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), RYGEL_TYPE_HTTP_SEEK, RygelHTTPSeekClass))

typedef struct _RygelHTTPSeek RygelHTTPSeek;
typedef struct _RygelHTTPSeekClass RygelHTTPSeekClass;
typedef struct _RygelHTTPSeekPrivate RygelHTTPSeekPrivate;
#define _g_object_unref0(var) ((var == NULL) ? NULL : (var = (g_object_unref (var), NULL)))

typedef enum  {
	RYGEL_HTTP_SEEK_ERROR_INVALID_RANGE = SOUP_STATUS_BAD_REQUEST,
	RYGEL_HTTP_SEEK_ERROR_OUT_OF_RANGE = SOUP_STATUS_REQUESTED_RANGE_NOT_SATISFIABLE
} RygelHTTPSeekError;
#define RYGEL_HTTP_SEEK_ERROR rygel_http_seek_error_quark ()
typedef enum  {
	RYGEL_HTTP_SEEK_TYPE_BYTE,
	RYGEL_HTTP_SEEK_TYPE_TIME
} RygelHTTPSeekType;

struct _RygelHTTPSeek {
	GObject parent_instance;
	RygelHTTPSeekPrivate * priv;
};

struct _RygelHTTPSeekClass {
	GObjectClass parent_class;
	void (*add_response_headers) (RygelHTTPSeek* self);
};

struct _RygelHTTPSeekPrivate {
	RygelHTTPSeekType _seek_type;
	SoupMessage* _msg;
	gint64 _start;
	gint64 _stop;
	gint64 _step;
	gint64 _length;
	gint64 _total_length;
};


static gpointer rygel_http_seek_parent_class = NULL;

GQuark rygel_http_seek_error_quark (void);
GType rygel_http_seek_type_get_type (void) G_GNUC_CONST;
GType rygel_http_seek_get_type (void) G_GNUC_CONST;
#define RYGEL_HTTP_SEEK_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), RYGEL_TYPE_HTTP_SEEK, RygelHTTPSeekPrivate))
enum  {
	RYGEL_HTTP_SEEK_DUMMY_PROPERTY,
	RYGEL_HTTP_SEEK_SEEK_TYPE,
	RYGEL_HTTP_SEEK_MSG,
	RYGEL_HTTP_SEEK_START,
	RYGEL_HTTP_SEEK_STOP,
	RYGEL_HTTP_SEEK_STEP,
	RYGEL_HTTP_SEEK_LENGTH,
	RYGEL_HTTP_SEEK_TOTAL_LENGTH
};
RygelHTTPSeek* rygel_http_seek_construct (GType object_type, SoupMessage* msg, gint64 start, gint64 stop, gint64 step, gint64 total_length, GError** error);
static void rygel_http_seek_set_msg (RygelHTTPSeek* self, SoupMessage* value);
static void rygel_http_seek_set_start (RygelHTTPSeek* self, gint64 value);
static void rygel_http_seek_set_stop (RygelHTTPSeek* self, gint64 value);
gint64 rygel_http_seek_get_length (RygelHTTPSeek* self);
static void rygel_http_seek_set_length (RygelHTTPSeek* self, gint64 value);
static void rygel_http_seek_set_total_length (RygelHTTPSeek* self, gint64 value);
void rygel_http_seek_add_response_headers (RygelHTTPSeek* self);
static void rygel_http_seek_real_add_response_headers (RygelHTTPSeek* self);
RygelHTTPSeekType rygel_http_seek_get_seek_type (RygelHTTPSeek* self);
void rygel_http_seek_set_seek_type (RygelHTTPSeek* self, RygelHTTPSeekType value);
SoupMessage* rygel_http_seek_get_msg (RygelHTTPSeek* self);
gint64 rygel_http_seek_get_start (RygelHTTPSeek* self);
gint64 rygel_http_seek_get_stop (RygelHTTPSeek* self);
gint64 rygel_http_seek_get_step (RygelHTTPSeek* self);
static void rygel_http_seek_set_step (RygelHTTPSeek* self, gint64 value);
gint64 rygel_http_seek_get_total_length (RygelHTTPSeek* self);
static void rygel_http_seek_finalize (GObject* obj);
static void _vala_rygel_http_seek_get_property (GObject * object, guint property_id, GValue * value, GParamSpec * pspec);
static void _vala_rygel_http_seek_set_property (GObject * object, guint property_id, const GValue * value, GParamSpec * pspec);


GQuark rygel_http_seek_error_quark (void) {
	return g_quark_from_static_string ("rygel_http_seek_error-quark");
}


GType rygel_http_seek_type_get_type (void) {
	static volatile gsize rygel_http_seek_type_type_id__volatile = 0;
	if (g_once_init_enter (&rygel_http_seek_type_type_id__volatile)) {
		static const GEnumValue values[] = {{RYGEL_HTTP_SEEK_TYPE_BYTE, "RYGEL_HTTP_SEEK_TYPE_BYTE", "byte"}, {RYGEL_HTTP_SEEK_TYPE_TIME, "RYGEL_HTTP_SEEK_TYPE_TIME", "time"}, {0, NULL, NULL}};
		GType rygel_http_seek_type_type_id;
		rygel_http_seek_type_type_id = g_enum_register_static ("RygelHTTPSeekType", values);
		g_once_init_leave (&rygel_http_seek_type_type_id__volatile, rygel_http_seek_type_type_id);
	}
	return rygel_http_seek_type_type_id__volatile;
}


RygelHTTPSeek* rygel_http_seek_construct (GType object_type, SoupMessage* msg, gint64 start, gint64 stop, gint64 step, gint64 total_length, GError** error) {
	RygelHTTPSeek * self = NULL;
	SoupMessage* _tmp0_ = NULL;
	gint64 _tmp1_ = 0LL;
	gint64 _tmp2_ = 0LL;
	gint64 _tmp3_ = 0LL;
	gint64 _tmp4_ = 0LL;
	gboolean _tmp5_ = FALSE;
	gint64 _tmp6_ = 0LL;
	gboolean _tmp12_ = FALSE;
	gint64 _tmp13_ = 0LL;
	gint64 _tmp19_ = 0LL;
	gint64 _tmp24_ = 0LL;
	gint64 _tmp25_ = 0LL;
	gint64 _tmp26_ = 0LL;
	GError * _inner_error_ = NULL;
	g_return_val_if_fail (msg != NULL, NULL);
	self = (RygelHTTPSeek*) g_object_new (object_type, NULL);
	_tmp0_ = msg;
	rygel_http_seek_set_msg (self, _tmp0_);
	_tmp1_ = start;
	rygel_http_seek_set_start (self, _tmp1_);
	_tmp2_ = stop;
	rygel_http_seek_set_stop (self, _tmp2_);
	_tmp3_ = self->priv->_length;
	rygel_http_seek_set_length (self, _tmp3_);
	_tmp4_ = total_length;
	rygel_http_seek_set_total_length (self, _tmp4_);
	_tmp6_ = start;
	if (_tmp6_ < ((gint64) 0)) {
		_tmp5_ = TRUE;
	} else {
		gint64 _tmp7_ = 0LL;
		gint64 _tmp8_ = 0LL;
		_tmp7_ = start;
		_tmp8_ = total_length;
		_tmp5_ = _tmp7_ >= _tmp8_;
	}
	if (_tmp5_) {
		const gchar* _tmp9_ = NULL;
		gint64 _tmp10_ = 0LL;
		GError* _tmp11_ = NULL;
		_tmp9_ = _ ("Out Of Range Start '%ld'");
		_tmp10_ = start;
		_tmp11_ = g_error_new (RYGEL_HTTP_SEEK_ERROR, RYGEL_HTTP_SEEK_ERROR_OUT_OF_RANGE, _tmp9_, _tmp10_);
		_inner_error_ = _tmp11_;
		if (_inner_error_->domain == RYGEL_HTTP_SEEK_ERROR) {
			g_propagate_error (error, _inner_error_);
			_g_object_unref0 (self);
			return NULL;
		} else {
			g_critical ("file %s: line %d: uncaught error: %s (%s, %d)", __FILE__, __LINE__, _inner_error_->message, g_quark_to_string (_inner_error_->domain), _inner_error_->code);
			g_clear_error (&_inner_error_);
			return NULL;
		}
	}
	_tmp13_ = stop;
	if (_tmp13_ < ((gint64) 0)) {
		_tmp12_ = TRUE;
	} else {
		gint64 _tmp14_ = 0LL;
		gint64 _tmp15_ = 0LL;
		_tmp14_ = stop;
		_tmp15_ = total_length;
		_tmp12_ = _tmp14_ >= _tmp15_;
	}
	if (_tmp12_) {
		const gchar* _tmp16_ = NULL;
		gint64 _tmp17_ = 0LL;
		GError* _tmp18_ = NULL;
		_tmp16_ = _ ("Out Of Range Stop '%ld'");
		_tmp17_ = stop;
		_tmp18_ = g_error_new (RYGEL_HTTP_SEEK_ERROR, RYGEL_HTTP_SEEK_ERROR_OUT_OF_RANGE, _tmp16_, _tmp17_);
		_inner_error_ = _tmp18_;
		if (_inner_error_->domain == RYGEL_HTTP_SEEK_ERROR) {
			g_propagate_error (error, _inner_error_);
			_g_object_unref0 (self);
			return NULL;
		} else {
			g_critical ("file %s: line %d: uncaught error: %s (%s, %d)", __FILE__, __LINE__, _inner_error_->message, g_quark_to_string (_inner_error_->domain), _inner_error_->code);
			g_clear_error (&_inner_error_);
			return NULL;
		}
	}
	_tmp19_ = self->priv->_length;
	if (_tmp19_ > ((gint64) 0)) {
		gint64 _tmp20_ = 0LL;
		gint64 _tmp21_ = 0LL;
		gint64 _tmp22_ = 0LL;
		gint64 _tmp23_ = 0LL;
		_tmp20_ = stop;
		_tmp21_ = start;
		_tmp22_ = self->priv->_length;
		_tmp23_ = CLAMP (_tmp20_, _tmp21_ + 1, _tmp22_ - 1);
		rygel_http_seek_set_stop (self, _tmp23_);
	}
	_tmp24_ = stop;
	_tmp25_ = step;
	_tmp26_ = start;
	rygel_http_seek_set_length (self, (_tmp24_ + _tmp25_) - _tmp26_);
	return self;
}


static void rygel_http_seek_real_add_response_headers (RygelHTTPSeek* self) {
	g_critical ("Type `%s' does not implement abstract method `rygel_http_seek_add_response_headers'", g_type_name (G_TYPE_FROM_INSTANCE (self)));
	return;
}


void rygel_http_seek_add_response_headers (RygelHTTPSeek* self) {
	g_return_if_fail (self != NULL);
	RYGEL_HTTP_SEEK_GET_CLASS (self)->add_response_headers (self);
}


RygelHTTPSeekType rygel_http_seek_get_seek_type (RygelHTTPSeek* self) {
	RygelHTTPSeekType result;
	RygelHTTPSeekType _tmp0_ = 0;
	g_return_val_if_fail (self != NULL, 0);
	_tmp0_ = self->priv->_seek_type;
	result = _tmp0_;
	return result;
}


void rygel_http_seek_set_seek_type (RygelHTTPSeek* self, RygelHTTPSeekType value) {
	RygelHTTPSeekType _tmp0_ = 0;
	g_return_if_fail (self != NULL);
	_tmp0_ = value;
	self->priv->_seek_type = _tmp0_;
	g_object_notify ((GObject *) self, "seek-type");
}


SoupMessage* rygel_http_seek_get_msg (RygelHTTPSeek* self) {
	SoupMessage* result;
	SoupMessage* _tmp0_ = NULL;
	g_return_val_if_fail (self != NULL, NULL);
	_tmp0_ = self->priv->_msg;
	result = _tmp0_;
	return result;
}


static gpointer _g_object_ref0 (gpointer self) {
	return self ? g_object_ref (self) : NULL;
}


static void rygel_http_seek_set_msg (RygelHTTPSeek* self, SoupMessage* value) {
	SoupMessage* _tmp0_ = NULL;
	SoupMessage* _tmp1_ = NULL;
	g_return_if_fail (self != NULL);
	_tmp0_ = value;
	_tmp1_ = _g_object_ref0 (_tmp0_);
	_g_object_unref0 (self->priv->_msg);
	self->priv->_msg = _tmp1_;
	g_object_notify ((GObject *) self, "msg");
}


gint64 rygel_http_seek_get_start (RygelHTTPSeek* self) {
	gint64 result;
	gint64 _tmp0_ = 0LL;
	g_return_val_if_fail (self != NULL, 0LL);
	_tmp0_ = self->priv->_start;
	result = _tmp0_;
	return result;
}


static void rygel_http_seek_set_start (RygelHTTPSeek* self, gint64 value) {
	gint64 _tmp0_ = 0LL;
	g_return_if_fail (self != NULL);
	_tmp0_ = value;
	self->priv->_start = _tmp0_;
	g_object_notify ((GObject *) self, "start");
}


gint64 rygel_http_seek_get_stop (RygelHTTPSeek* self) {
	gint64 result;
	gint64 _tmp0_ = 0LL;
	g_return_val_if_fail (self != NULL, 0LL);
	_tmp0_ = self->priv->_stop;
	result = _tmp0_;
	return result;
}


static void rygel_http_seek_set_stop (RygelHTTPSeek* self, gint64 value) {
	gint64 _tmp0_ = 0LL;
	g_return_if_fail (self != NULL);
	_tmp0_ = value;
	self->priv->_stop = _tmp0_;
	g_object_notify ((GObject *) self, "stop");
}


gint64 rygel_http_seek_get_step (RygelHTTPSeek* self) {
	gint64 result;
	gint64 _tmp0_ = 0LL;
	g_return_val_if_fail (self != NULL, 0LL);
	_tmp0_ = self->priv->_step;
	result = _tmp0_;
	return result;
}


static void rygel_http_seek_set_step (RygelHTTPSeek* self, gint64 value) {
	gint64 _tmp0_ = 0LL;
	g_return_if_fail (self != NULL);
	_tmp0_ = value;
	self->priv->_step = _tmp0_;
	g_object_notify ((GObject *) self, "step");
}


gint64 rygel_http_seek_get_length (RygelHTTPSeek* self) {
	gint64 result;
	gint64 _tmp0_ = 0LL;
	g_return_val_if_fail (self != NULL, 0LL);
	_tmp0_ = self->priv->_length;
	result = _tmp0_;
	return result;
}


static void rygel_http_seek_set_length (RygelHTTPSeek* self, gint64 value) {
	gint64 _tmp0_ = 0LL;
	g_return_if_fail (self != NULL);
	_tmp0_ = value;
	self->priv->_length = _tmp0_;
	g_object_notify ((GObject *) self, "length");
}


gint64 rygel_http_seek_get_total_length (RygelHTTPSeek* self) {
	gint64 result;
	gint64 _tmp0_ = 0LL;
	g_return_val_if_fail (self != NULL, 0LL);
	_tmp0_ = self->priv->_total_length;
	result = _tmp0_;
	return result;
}


static void rygel_http_seek_set_total_length (RygelHTTPSeek* self, gint64 value) {
	gint64 _tmp0_ = 0LL;
	g_return_if_fail (self != NULL);
	_tmp0_ = value;
	self->priv->_total_length = _tmp0_;
	g_object_notify ((GObject *) self, "total-length");
}


static void rygel_http_seek_class_init (RygelHTTPSeekClass * klass) {
	rygel_http_seek_parent_class = g_type_class_peek_parent (klass);
	g_type_class_add_private (klass, sizeof (RygelHTTPSeekPrivate));
	RYGEL_HTTP_SEEK_CLASS (klass)->add_response_headers = rygel_http_seek_real_add_response_headers;
	G_OBJECT_CLASS (klass)->get_property = _vala_rygel_http_seek_get_property;
	G_OBJECT_CLASS (klass)->set_property = _vala_rygel_http_seek_set_property;
	G_OBJECT_CLASS (klass)->finalize = rygel_http_seek_finalize;
	/**
	     * Identifies whether this is a class Range request or a DLNA-specific
	     * "TimeSeekRange.dlna.org" request.
	     */
	g_object_class_install_property (G_OBJECT_CLASS (klass), RYGEL_HTTP_SEEK_SEEK_TYPE, g_param_spec_enum ("seek-type", "seek-type", "seek-type", RYGEL_TYPE_HTTP_SEEK_TYPE, 0, G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK | G_PARAM_STATIC_BLURB | G_PARAM_READABLE | G_PARAM_WRITABLE));
	g_object_class_install_property (G_OBJECT_CLASS (klass), RYGEL_HTTP_SEEK_MSG, g_param_spec_object ("msg", "msg", "msg", soup_message_get_type (), G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK | G_PARAM_STATIC_BLURB | G_PARAM_READABLE));
	/**
	     * The start of the range as a number of bytes (classic) or as microseconds 
	     * (DLNA-specific). See seek_type.
	     */
	g_object_class_install_property (G_OBJECT_CLASS (klass), RYGEL_HTTP_SEEK_START, g_param_spec_int64 ("start", "start", "start", G_MININT64, G_MAXINT64, 0, G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK | G_PARAM_STATIC_BLURB | G_PARAM_READABLE));
	/**
	     * The end of the range as a number of bytes (classic) or as microseconds 
	     * (DLNA-specific). See seek_type.
	     */
	g_object_class_install_property (G_OBJECT_CLASS (klass), RYGEL_HTTP_SEEK_STOP, g_param_spec_int64 ("stop", "stop", "stop", G_MININT64, G_MAXINT64, 0, G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK | G_PARAM_STATIC_BLURB | G_PARAM_READABLE));
	/**
	     * Either 1 byte (classic) or as 1000 G_TIME_SPAN_MILLISECOND microseconds 
	     * (DLNA-specific). See seek_type.
	     */
	g_object_class_install_property (G_OBJECT_CLASS (klass), RYGEL_HTTP_SEEK_STEP, g_param_spec_int64 ("step", "step", "step", G_MININT64, G_MAXINT64, 0, G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK | G_PARAM_STATIC_BLURB | G_PARAM_READABLE));
	/**
	     * The length of the range as a number of bytes (classic) or as microseconds 
	     * (DLNA-specific). See seek_type.
	     */
	g_object_class_install_property (G_OBJECT_CLASS (klass), RYGEL_HTTP_SEEK_LENGTH, g_param_spec_int64 ("length", "length", "length", G_MININT64, G_MAXINT64, 0, G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK | G_PARAM_STATIC_BLURB | G_PARAM_READABLE));
	/**
	     * The length of the media file as a number of bytes (classic) or as microseconds 
	     * (DLNA-specific). See seek_type.
	     */
	g_object_class_install_property (G_OBJECT_CLASS (klass), RYGEL_HTTP_SEEK_TOTAL_LENGTH, g_param_spec_int64 ("total-length", "total-length", "total-length", G_MININT64, G_MAXINT64, 0, G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK | G_PARAM_STATIC_BLURB | G_PARAM_READABLE));
}


static void rygel_http_seek_instance_init (RygelHTTPSeek * self) {
	self->priv = RYGEL_HTTP_SEEK_GET_PRIVATE (self);
}


static void rygel_http_seek_finalize (GObject* obj) {
	RygelHTTPSeek * self;
	self = G_TYPE_CHECK_INSTANCE_CAST (obj, RYGEL_TYPE_HTTP_SEEK, RygelHTTPSeek);
	_g_object_unref0 (self->priv->_msg);
	G_OBJECT_CLASS (rygel_http_seek_parent_class)->finalize (obj);
}


/**
 * HTTPSeek is an abstract representation of a ranged HTTP request.
 *
 * It can be one of:
 *
 *  - The classic Range request (seek_type == HTTPSeekType.BYTE), with start and stop in bytes.
 *  - The DLNA-Specific "TimeSeekRange.dlna.org" request (seek_type == HTTPSeekType.TIME) with start and stop in microseconds.
 */
GType rygel_http_seek_get_type (void) {
	static volatile gsize rygel_http_seek_type_id__volatile = 0;
	if (g_once_init_enter (&rygel_http_seek_type_id__volatile)) {
		static const GTypeInfo g_define_type_info = { sizeof (RygelHTTPSeekClass), (GBaseInitFunc) NULL, (GBaseFinalizeFunc) NULL, (GClassInitFunc) rygel_http_seek_class_init, (GClassFinalizeFunc) NULL, NULL, sizeof (RygelHTTPSeek), 0, (GInstanceInitFunc) rygel_http_seek_instance_init, NULL };
		GType rygel_http_seek_type_id;
		rygel_http_seek_type_id = g_type_register_static (G_TYPE_OBJECT, "RygelHTTPSeek", &g_define_type_info, G_TYPE_FLAG_ABSTRACT);
		g_once_init_leave (&rygel_http_seek_type_id__volatile, rygel_http_seek_type_id);
	}
	return rygel_http_seek_type_id__volatile;
}


static void _vala_rygel_http_seek_get_property (GObject * object, guint property_id, GValue * value, GParamSpec * pspec) {
	RygelHTTPSeek * self;
	self = G_TYPE_CHECK_INSTANCE_CAST (object, RYGEL_TYPE_HTTP_SEEK, RygelHTTPSeek);
	switch (property_id) {
		case RYGEL_HTTP_SEEK_SEEK_TYPE:
		g_value_set_enum (value, rygel_http_seek_get_seek_type (self));
		break;
		case RYGEL_HTTP_SEEK_MSG:
		g_value_set_object (value, rygel_http_seek_get_msg (self));
		break;
		case RYGEL_HTTP_SEEK_START:
		g_value_set_int64 (value, rygel_http_seek_get_start (self));
		break;
		case RYGEL_HTTP_SEEK_STOP:
		g_value_set_int64 (value, rygel_http_seek_get_stop (self));
		break;
		case RYGEL_HTTP_SEEK_STEP:
		g_value_set_int64 (value, rygel_http_seek_get_step (self));
		break;
		case RYGEL_HTTP_SEEK_LENGTH:
		g_value_set_int64 (value, rygel_http_seek_get_length (self));
		break;
		case RYGEL_HTTP_SEEK_TOTAL_LENGTH:
		g_value_set_int64 (value, rygel_http_seek_get_total_length (self));
		break;
		default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
		break;
	}
}


static void _vala_rygel_http_seek_set_property (GObject * object, guint property_id, const GValue * value, GParamSpec * pspec) {
	RygelHTTPSeek * self;
	self = G_TYPE_CHECK_INSTANCE_CAST (object, RYGEL_TYPE_HTTP_SEEK, RygelHTTPSeek);
	switch (property_id) {
		case RYGEL_HTTP_SEEK_SEEK_TYPE:
		rygel_http_seek_set_seek_type (self, g_value_get_enum (value));
		break;
		case RYGEL_HTTP_SEEK_MSG:
		rygel_http_seek_set_msg (self, g_value_get_object (value));
		break;
		case RYGEL_HTTP_SEEK_START:
		rygel_http_seek_set_start (self, g_value_get_int64 (value));
		break;
		case RYGEL_HTTP_SEEK_STOP:
		rygel_http_seek_set_stop (self, g_value_get_int64 (value));
		break;
		case RYGEL_HTTP_SEEK_STEP:
		rygel_http_seek_set_step (self, g_value_get_int64 (value));
		break;
		case RYGEL_HTTP_SEEK_LENGTH:
		rygel_http_seek_set_length (self, g_value_get_int64 (value));
		break;
		case RYGEL_HTTP_SEEK_TOTAL_LENGTH:
		rygel_http_seek_set_total_length (self, g_value_get_int64 (value));
		break;
		default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
		break;
	}
}


