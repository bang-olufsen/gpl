/* rygel-changelog.c generated by valac 0.24.0, the Vala compiler
 * generated from rygel-changelog.vala, do not modify */

/*
 * Copyright (C) 2008 OpenedHand Ltd.
 * Copyright (C) 2009 Nokia Corporation.
 *
 * Author: Jorn Baayen <jorn@openedhand.com>
 *         Zeeshan Ali (Khattak) <zeeshanak@gnome.org>
 *                               <zeeshan.ali@nokia.com>
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
#include <libgupnp/gupnp.h>


#define RYGEL_TYPE_CHANGE_LOG (rygel_change_log_get_type ())
#define RYGEL_CHANGE_LOG(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), RYGEL_TYPE_CHANGE_LOG, RygelChangeLog))
#define RYGEL_CHANGE_LOG_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), RYGEL_TYPE_CHANGE_LOG, RygelChangeLogClass))
#define RYGEL_IS_CHANGE_LOG(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), RYGEL_TYPE_CHANGE_LOG))
#define RYGEL_IS_CHANGE_LOG_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), RYGEL_TYPE_CHANGE_LOG))
#define RYGEL_CHANGE_LOG_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), RYGEL_TYPE_CHANGE_LOG, RygelChangeLogClass))

typedef struct _RygelChangeLog RygelChangeLog;
typedef struct _RygelChangeLogClass RygelChangeLogClass;
typedef struct _RygelChangeLogPrivate RygelChangeLogPrivate;
#define _g_free0(var) (var = (g_free (var), NULL))
#define _g_string_free0(var) ((var == NULL) ? NULL : (var = (g_string_free (var, TRUE), NULL)))
#define _g_object_unref0(var) ((var == NULL) ? NULL : (var = (g_object_unref (var), NULL)))

struct _RygelChangeLog {
	GObject parent_instance;
	RygelChangeLogPrivate * priv;
	GWeakRef service;
};

struct _RygelChangeLogClass {
	GObjectClass parent_class;
};

struct _RygelChangeLogPrivate {
	gchar* service_ns;
	GString* str;
	GeeHashMap* hash;
	guint timeout_id;
};


static gpointer rygel_change_log_parent_class = NULL;

GType rygel_change_log_get_type (void) G_GNUC_CONST;
#define RYGEL_CHANGE_LOG_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), RYGEL_TYPE_CHANGE_LOG, RygelChangeLogPrivate))
enum  {
	RYGEL_CHANGE_LOG_DUMMY_PROPERTY
};
RygelChangeLog* rygel_change_log_new (GUPnPService* service, const gchar* service_ns);
RygelChangeLog* rygel_change_log_construct (GType object_type, GUPnPService* service, const gchar* service_ns);
static gboolean rygel_change_log_timeout (RygelChangeLog* self);
gchar* rygel_change_log_finish (RygelChangeLog* self);
static void rygel_change_log_ensure_timeout (RygelChangeLog* self);
static gboolean _rygel_change_log_timeout_gsource_func (gpointer self);
void rygel_change_log_log (RygelChangeLog* self, const gchar* variable, const gchar* value);
void rygel_change_log_log_with_channel (RygelChangeLog* self, const gchar* variable, const gchar* value, const gchar* channel);
static void rygel_change_log_finalize (GObject* obj);


RygelChangeLog* rygel_change_log_construct (GType object_type, GUPnPService* service, const gchar* service_ns) {
	RygelChangeLog * self = NULL;
	GUPnPService* _tmp0_ = NULL;
	const gchar* _tmp1_ = NULL;
	gchar* _tmp2_ = NULL;
	GString* _tmp3_ = NULL;
	GeeHashMap* _tmp4_ = NULL;
	g_return_val_if_fail (service_ns != NULL, NULL);
	self = (RygelChangeLog*) g_object_new (object_type, NULL);
	_tmp0_ = service;
	g_weak_ref_clear (&self->service);
	g_weak_ref_init (&self->service, (GObject*) _tmp0_);
	_tmp1_ = service_ns;
	_tmp2_ = g_strdup (_tmp1_);
	_g_free0 (self->priv->service_ns);
	self->priv->service_ns = _tmp2_;
	_tmp3_ = g_string_new ("");
	_g_string_free0 (self->priv->str);
	self->priv->str = _tmp3_;
	_tmp4_ = gee_hash_map_new (G_TYPE_STRING, (GBoxedCopyFunc) g_strdup, g_free, G_TYPE_STRING, (GBoxedCopyFunc) g_strdup, g_free, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
	_g_object_unref0 (self->priv->hash);
	self->priv->hash = _tmp4_;
	return self;
}


RygelChangeLog* rygel_change_log_new (GUPnPService* service, const gchar* service_ns) {
	return rygel_change_log_construct (RYGEL_TYPE_CHANGE_LOG, service, service_ns);
}


static gboolean rygel_change_log_timeout (RygelChangeLog* self) {
	gboolean result = FALSE;
	GUPnPService* service = NULL;
	GObject* _tmp0_ = NULL;
	GUPnPService* _tmp1_ = NULL;
	GUPnPService* _tmp2_ = NULL;
	gchar* _tmp3_ = NULL;
	gchar* _tmp4_ = NULL;
	GeeHashMap* _tmp5_ = NULL;
	GString* _tmp6_ = NULL;
	g_return_val_if_fail (self != NULL, FALSE);
	_tmp0_ = g_weak_ref_get (&self->service);
	service = G_TYPE_CHECK_INSTANCE_CAST (_tmp0_, gupnp_service_get_type (), GUPnPService);
	_tmp1_ = service;
	if (_tmp1_ == NULL) {
		result = FALSE;
		_g_object_unref0 (service);
		return result;
	}
	_tmp2_ = service;
	_tmp3_ = rygel_change_log_finish (self);
	_tmp4_ = _tmp3_;
	gupnp_service_notify (_tmp2_, "LastChange", G_TYPE_STRING, _tmp4_, NULL);
	_g_free0 (_tmp4_);
	g_debug ("rygel-changelog.vala:60: LastChange sent");
	_tmp5_ = self->priv->hash;
	gee_abstract_map_clear ((GeeAbstractMap*) _tmp5_);
	_tmp6_ = self->priv->str;
	g_string_erase (_tmp6_, (gssize) 0, (gssize) (-1));
	self->priv->timeout_id = (guint) 0;
	result = FALSE;
	_g_object_unref0 (service);
	return result;
}


static gboolean _rygel_change_log_timeout_gsource_func (gpointer self) {
	gboolean result;
	result = rygel_change_log_timeout ((RygelChangeLog*) self);
	return result;
}


static void rygel_change_log_ensure_timeout (RygelChangeLog* self) {
	GObject* _tmp0_ = NULL;
	GObject* _tmp1_ = NULL;
	gboolean _tmp2_ = FALSE;
	g_return_if_fail (self != NULL);
	_tmp0_ = g_weak_ref_get (&self->service);
	_tmp1_ = _tmp0_;
	_tmp2_ = _tmp1_ != NULL;
	_g_object_unref0 (_tmp1_);
	if (_tmp2_) {
		guint _tmp3_ = 0U;
		guint _tmp5_ = 0U;
		_tmp3_ = self->priv->timeout_id;
		if (_tmp3_ != ((guint) 0)) {
			guint _tmp4_ = 0U;
			_tmp4_ = self->priv->timeout_id;
			g_source_remove (_tmp4_);
		} else {
			g_debug ("rygel-changelog.vala:76: Setting up timeout for LastChange");
		}
		_tmp5_ = g_timeout_add_full (G_PRIORITY_DEFAULT, (guint) 200, _rygel_change_log_timeout_gsource_func, g_object_ref (self), g_object_unref);
		self->priv->timeout_id = _tmp5_;
	}
}


void rygel_change_log_log (RygelChangeLog* self, const gchar* variable, const gchar* value) {
	const gchar* _tmp0_ = NULL;
	const gchar* _tmp1_ = NULL;
	GeeHashMap* _tmp2_ = NULL;
	const gchar* _tmp3_ = NULL;
	const gchar* _tmp4_ = NULL;
	const gchar* _tmp5_ = NULL;
	gchar* _tmp6_ = NULL;
	gchar* _tmp7_ = NULL;
	gchar* _tmp8_ = NULL;
	gchar* _tmp9_ = NULL;
	g_return_if_fail (self != NULL);
	g_return_if_fail (variable != NULL);
	g_return_if_fail (value != NULL);
	_tmp0_ = variable;
	_tmp1_ = value;
	g_debug ("rygel-changelog.vala:83: '%s = %s' logged", _tmp0_, _tmp1_);
	_tmp2_ = self->priv->hash;
	_tmp3_ = variable;
	_tmp4_ = variable;
	_tmp5_ = value;
	_tmp6_ = g_markup_escape_text (_tmp5_, (gssize) (-1));
	_tmp7_ = _tmp6_;
	_tmp8_ = g_strdup_printf ("<%s val=\"%s\"/>", _tmp4_, _tmp7_);
	_tmp9_ = _tmp8_;
	gee_abstract_map_set ((GeeAbstractMap*) _tmp2_, _tmp3_, _tmp9_);
	_g_free0 (_tmp9_);
	_g_free0 (_tmp7_);
	rygel_change_log_ensure_timeout (self);
}


void rygel_change_log_log_with_channel (RygelChangeLog* self, const gchar* variable, const gchar* value, const gchar* channel) {
	GeeHashMap* _tmp0_ = NULL;
	const gchar* _tmp1_ = NULL;
	const gchar* _tmp2_ = NULL;
	const gchar* _tmp3_ = NULL;
	gchar* _tmp4_ = NULL;
	gchar* _tmp5_ = NULL;
	const gchar* _tmp6_ = NULL;
	gchar* _tmp7_ = NULL;
	gchar* _tmp8_ = NULL;
	gchar* _tmp9_ = NULL;
	gchar* _tmp10_ = NULL;
	g_return_if_fail (self != NULL);
	g_return_if_fail (variable != NULL);
	g_return_if_fail (value != NULL);
	g_return_if_fail (channel != NULL);
	_tmp0_ = self->priv->hash;
	_tmp1_ = variable;
	_tmp2_ = variable;
	_tmp3_ = value;
	_tmp4_ = g_markup_escape_text (_tmp3_, (gssize) (-1));
	_tmp5_ = _tmp4_;
	_tmp6_ = channel;
	_tmp7_ = g_markup_escape_text (_tmp6_, (gssize) (-1));
	_tmp8_ = _tmp7_;
	_tmp9_ = g_strdup_printf ("<%s val=\"%s\" channel=\"%s\"/>", _tmp2_, _tmp5_, _tmp8_);
	_tmp10_ = _tmp9_;
	gee_abstract_map_set ((GeeAbstractMap*) _tmp0_, _tmp1_, _tmp10_);
	_g_free0 (_tmp10_);
	_g_free0 (_tmp8_);
	_g_free0 (_tmp5_);
	rygel_change_log_ensure_timeout (self);
}


gchar* rygel_change_log_finish (RygelChangeLog* self) {
	gchar* result = NULL;
	GString* _tmp0_ = NULL;
	const gchar* _tmp1_ = NULL;
	gchar* _tmp2_ = NULL;
	gchar* _tmp3_ = NULL;
	gchar* _tmp4_ = NULL;
	gchar* _tmp5_ = NULL;
	GString* _tmp18_ = NULL;
	GString* _tmp19_ = NULL;
	const gchar* _tmp20_ = NULL;
	gchar* _tmp21_ = NULL;
	g_return_val_if_fail (self != NULL, NULL);
	_tmp0_ = self->priv->str;
	_tmp1_ = self->priv->service_ns;
	_tmp2_ = g_strconcat ("<Event xmlns=\"", _tmp1_, NULL);
	_tmp3_ = _tmp2_;
	_tmp4_ = g_strconcat (_tmp3_, "\"><InstanceID val=\"0\">", NULL);
	_tmp5_ = _tmp4_;
	g_string_append (_tmp0_, _tmp5_);
	_g_free0 (_tmp5_);
	_g_free0 (_tmp3_);
	{
		GeeIterator* _line_it = NULL;
		GeeHashMap* _tmp6_ = NULL;
		GeeCollection* _tmp7_ = NULL;
		GeeCollection* _tmp8_ = NULL;
		GeeCollection* _tmp9_ = NULL;
		GeeIterator* _tmp10_ = NULL;
		GeeIterator* _tmp11_ = NULL;
		_tmp6_ = self->priv->hash;
		_tmp7_ = gee_abstract_map_get_values ((GeeMap*) _tmp6_);
		_tmp8_ = _tmp7_;
		_tmp9_ = _tmp8_;
		_tmp10_ = gee_iterable_iterator ((GeeIterable*) _tmp9_);
		_tmp11_ = _tmp10_;
		_g_object_unref0 (_tmp9_);
		_line_it = _tmp11_;
		while (TRUE) {
			GeeIterator* _tmp12_ = NULL;
			gboolean _tmp13_ = FALSE;
			gchar* line = NULL;
			GeeIterator* _tmp14_ = NULL;
			gpointer _tmp15_ = NULL;
			GString* _tmp16_ = NULL;
			const gchar* _tmp17_ = NULL;
			_tmp12_ = _line_it;
			_tmp13_ = gee_iterator_next (_tmp12_);
			if (!_tmp13_) {
				break;
			}
			_tmp14_ = _line_it;
			_tmp15_ = gee_iterator_get (_tmp14_);
			line = (gchar*) _tmp15_;
			_tmp16_ = self->priv->str;
			_tmp17_ = line;
			g_string_append (_tmp16_, _tmp17_);
			_g_free0 (line);
		}
		_g_object_unref0 (_line_it);
	}
	_tmp18_ = self->priv->str;
	g_string_append (_tmp18_, "</InstanceID></Event>");
	_tmp19_ = self->priv->str;
	_tmp20_ = _tmp19_->str;
	_tmp21_ = g_strdup (_tmp20_);
	result = _tmp21_;
	return result;
}


static void rygel_change_log_class_init (RygelChangeLogClass * klass) {
	rygel_change_log_parent_class = g_type_class_peek_parent (klass);
	g_type_class_add_private (klass, sizeof (RygelChangeLogPrivate));
	G_OBJECT_CLASS (klass)->finalize = rygel_change_log_finalize;
}


static void rygel_change_log_instance_init (RygelChangeLog * self) {
	self->priv = RYGEL_CHANGE_LOG_GET_PRIVATE (self);
	self->priv->timeout_id = (guint) 0;
}


static void rygel_change_log_finalize (GObject* obj) {
	RygelChangeLog * self;
	guint _tmp0_ = 0U;
	self = G_TYPE_CHECK_INSTANCE_CAST (obj, RYGEL_TYPE_CHANGE_LOG, RygelChangeLog);
	_tmp0_ = self->priv->timeout_id;
	if (_tmp0_ != ((guint) 0)) {
		guint _tmp1_ = 0U;
		_tmp1_ = self->priv->timeout_id;
		g_source_remove (_tmp1_);
	}
	g_weak_ref_clear (&self->service);
	_g_free0 (self->priv->service_ns);
	_g_string_free0 (self->priv->str);
	_g_object_unref0 (self->priv->hash);
	G_OBJECT_CLASS (rygel_change_log_parent_class)->finalize (obj);
}


GType rygel_change_log_get_type (void) {
	static volatile gsize rygel_change_log_type_id__volatile = 0;
	if (g_once_init_enter (&rygel_change_log_type_id__volatile)) {
		static const GTypeInfo g_define_type_info = { sizeof (RygelChangeLogClass), (GBaseInitFunc) NULL, (GBaseFinalizeFunc) NULL, (GClassInitFunc) rygel_change_log_class_init, (GClassFinalizeFunc) NULL, NULL, sizeof (RygelChangeLog), 0, (GInstanceInitFunc) rygel_change_log_instance_init, NULL };
		GType rygel_change_log_type_id;
		rygel_change_log_type_id = g_type_register_static (G_TYPE_OBJECT, "RygelChangeLog", &g_define_type_info, 0);
		g_once_init_leave (&rygel_change_log_type_id__volatile, rygel_change_log_type_id);
	}
	return rygel_change_log_type_id__volatile;
}



