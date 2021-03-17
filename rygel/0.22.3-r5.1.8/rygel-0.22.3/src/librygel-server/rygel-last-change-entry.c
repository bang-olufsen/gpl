/* rygel-last-change-entry.c generated by valac 0.24.0, the Vala compiler
 * generated from rygel-last-change-entry.vala, do not modify */

/*
 * Copyright (C) 2012 Openismus GmbH
 *
 * Author: Krzesimir Nowak <krnowak@openismus.com>
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


#define RYGEL_TYPE_LAST_CHANGE_ENTRY (rygel_last_change_entry_get_type ())
#define RYGEL_LAST_CHANGE_ENTRY(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), RYGEL_TYPE_LAST_CHANGE_ENTRY, RygelLastChangeEntry))
#define RYGEL_LAST_CHANGE_ENTRY_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), RYGEL_TYPE_LAST_CHANGE_ENTRY, RygelLastChangeEntryClass))
#define RYGEL_IS_LAST_CHANGE_ENTRY(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), RYGEL_TYPE_LAST_CHANGE_ENTRY))
#define RYGEL_IS_LAST_CHANGE_ENTRY_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), RYGEL_TYPE_LAST_CHANGE_ENTRY))
#define RYGEL_LAST_CHANGE_ENTRY_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), RYGEL_TYPE_LAST_CHANGE_ENTRY, RygelLastChangeEntryClass))

typedef struct _RygelLastChangeEntry RygelLastChangeEntry;
typedef struct _RygelLastChangeEntryClass RygelLastChangeEntryClass;
typedef struct _RygelLastChangeEntryPrivate RygelLastChangeEntryPrivate;
#define _g_free0(var) (var = (g_free (var), NULL))
#define _g_string_free0(var) ((var == NULL) ? NULL : (var = (g_string_free (var, TRUE), NULL)))

struct _RygelLastChangeEntry {
	GObject parent_instance;
	RygelLastChangeEntryPrivate * priv;
};

struct _RygelLastChangeEntryClass {
	GObjectClass parent_class;
	gchar* (*additional_info) (RygelLastChangeEntry* self);
};

struct _RygelLastChangeEntryPrivate {
	gchar* tag;
	gchar* id;
	guint update_id;
};


static gpointer rygel_last_change_entry_parent_class = NULL;

GType rygel_last_change_entry_get_type (void) G_GNUC_CONST;
#define RYGEL_LAST_CHANGE_ENTRY_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), RYGEL_TYPE_LAST_CHANGE_ENTRY, RygelLastChangeEntryPrivate))
enum  {
	RYGEL_LAST_CHANGE_ENTRY_DUMMY_PROPERTY
};
RygelLastChangeEntry* rygel_last_change_entry_construct (GType object_type, const gchar* tag, const gchar* id, guint update_id);
gchar* rygel_last_change_entry_additional_info (RygelLastChangeEntry* self);
static gchar* rygel_last_change_entry_real_additional_info (RygelLastChangeEntry* self);
gchar* rygel_last_change_entry_to_string (RygelLastChangeEntry* self);
static void rygel_last_change_entry_finalize (GObject* obj);


RygelLastChangeEntry* rygel_last_change_entry_construct (GType object_type, const gchar* tag, const gchar* id, guint update_id) {
	RygelLastChangeEntry * self = NULL;
	const gchar* _tmp0_ = NULL;
	gchar* _tmp1_ = NULL;
	const gchar* _tmp2_ = NULL;
	gchar* _tmp3_ = NULL;
	guint _tmp4_ = 0U;
	g_return_val_if_fail (tag != NULL, NULL);
	g_return_val_if_fail (id != NULL, NULL);
	self = (RygelLastChangeEntry*) g_object_new (object_type, NULL);
	_tmp0_ = tag;
	_tmp1_ = g_strdup (_tmp0_);
	_g_free0 (self->priv->tag);
	self->priv->tag = _tmp1_;
	_tmp2_ = id;
	_tmp3_ = g_strdup (_tmp2_);
	_g_free0 (self->priv->id);
	self->priv->id = _tmp3_;
	_tmp4_ = update_id;
	self->priv->update_id = _tmp4_;
	return self;
}


static gchar* rygel_last_change_entry_real_additional_info (RygelLastChangeEntry* self) {
	g_critical ("Type `%s' does not implement abstract method `rygel_last_change_entry_additional_info'", g_type_name (G_TYPE_FROM_INSTANCE (self)));
	return NULL;
}


gchar* rygel_last_change_entry_additional_info (RygelLastChangeEntry* self) {
	g_return_val_if_fail (self != NULL, NULL);
	return RYGEL_LAST_CHANGE_ENTRY_GET_CLASS (self)->additional_info (self);
}


gchar* rygel_last_change_entry_to_string (RygelLastChangeEntry* self) {
	gchar* result = NULL;
	GString* str = NULL;
	GString* _tmp0_ = NULL;
	gchar* info = NULL;
	gchar* _tmp1_ = NULL;
	GString* _tmp2_ = NULL;
	const gchar* _tmp3_ = NULL;
	gchar* _tmp4_ = NULL;
	gchar* _tmp5_ = NULL;
	gchar* _tmp6_ = NULL;
	gchar* _tmp7_ = NULL;
	gchar* _tmp8_ = NULL;
	gchar* _tmp9_ = NULL;
	const gchar* _tmp10_ = NULL;
	gchar* _tmp11_ = NULL;
	gchar* _tmp12_ = NULL;
	gchar* _tmp13_ = NULL;
	gchar* _tmp14_ = NULL;
	gchar* _tmp15_ = NULL;
	gchar* _tmp16_ = NULL;
	guint _tmp17_ = 0U;
	gchar* _tmp18_ = NULL;
	gchar* _tmp19_ = NULL;
	gchar* _tmp20_ = NULL;
	gchar* _tmp21_ = NULL;
	gchar* _tmp22_ = NULL;
	gchar* _tmp23_ = NULL;
	const gchar* _tmp24_ = NULL;
	gint _tmp25_ = 0;
	gint _tmp26_ = 0;
	GString* _tmp31_ = NULL;
	GString* _tmp32_ = NULL;
	const gchar* _tmp33_ = NULL;
	gchar* _tmp34_ = NULL;
	g_return_val_if_fail (self != NULL, NULL);
	_tmp0_ = g_string_new ("");
	str = _tmp0_;
	_tmp1_ = rygel_last_change_entry_additional_info (self);
	info = _tmp1_;
	_tmp2_ = str;
	_tmp3_ = self->priv->tag;
	_tmp4_ = g_strconcat ("<", _tmp3_, NULL);
	_tmp5_ = _tmp4_;
	_tmp6_ = g_strconcat (_tmp5_, " ", NULL);
	_tmp7_ = _tmp6_;
	_tmp8_ = g_strconcat (_tmp7_, "objID=\"", NULL);
	_tmp9_ = _tmp8_;
	_tmp10_ = self->priv->id;
	_tmp11_ = g_strconcat (_tmp9_, _tmp10_, NULL);
	_tmp12_ = _tmp11_;
	_tmp13_ = g_strconcat (_tmp12_, "\" ", NULL);
	_tmp14_ = _tmp13_;
	_tmp15_ = g_strconcat (_tmp14_, "updateID=\"", NULL);
	_tmp16_ = _tmp15_;
	_tmp17_ = self->priv->update_id;
	_tmp18_ = g_strdup_printf ("%u", _tmp17_);
	_tmp19_ = _tmp18_;
	_tmp20_ = g_strconcat (_tmp16_, _tmp19_, NULL);
	_tmp21_ = _tmp20_;
	_tmp22_ = g_strconcat (_tmp21_, "\"", NULL);
	_tmp23_ = _tmp22_;
	g_string_append (_tmp2_, _tmp23_);
	_g_free0 (_tmp23_);
	_g_free0 (_tmp21_);
	_g_free0 (_tmp19_);
	_g_free0 (_tmp16_);
	_g_free0 (_tmp14_);
	_g_free0 (_tmp12_);
	_g_free0 (_tmp9_);
	_g_free0 (_tmp7_);
	_g_free0 (_tmp5_);
	_tmp24_ = info;
	_tmp25_ = strlen (_tmp24_);
	_tmp26_ = _tmp25_;
	if (_tmp26_ > 0) {
		GString* _tmp27_ = NULL;
		const gchar* _tmp28_ = NULL;
		gchar* _tmp29_ = NULL;
		gchar* _tmp30_ = NULL;
		_tmp27_ = str;
		_tmp28_ = info;
		_tmp29_ = g_strconcat (" ", _tmp28_, NULL);
		_tmp30_ = _tmp29_;
		g_string_append (_tmp27_, _tmp30_);
		_g_free0 (_tmp30_);
	}
	_tmp31_ = str;
	g_string_append (_tmp31_, "/>");
	_tmp32_ = str;
	_tmp33_ = _tmp32_->str;
	_tmp34_ = g_strdup (_tmp33_);
	result = _tmp34_;
	_g_free0 (info);
	_g_string_free0 (str);
	return result;
}


static void rygel_last_change_entry_class_init (RygelLastChangeEntryClass * klass) {
	rygel_last_change_entry_parent_class = g_type_class_peek_parent (klass);
	g_type_class_add_private (klass, sizeof (RygelLastChangeEntryPrivate));
	RYGEL_LAST_CHANGE_ENTRY_CLASS (klass)->additional_info = rygel_last_change_entry_real_additional_info;
	G_OBJECT_CLASS (klass)->finalize = rygel_last_change_entry_finalize;
}


static void rygel_last_change_entry_instance_init (RygelLastChangeEntry * self) {
	self->priv = RYGEL_LAST_CHANGE_ENTRY_GET_PRIVATE (self);
}


static void rygel_last_change_entry_finalize (GObject* obj) {
	RygelLastChangeEntry * self;
	self = G_TYPE_CHECK_INSTANCE_CAST (obj, RYGEL_TYPE_LAST_CHANGE_ENTRY, RygelLastChangeEntry);
	_g_free0 (self->priv->tag);
	_g_free0 (self->priv->id);
	G_OBJECT_CLASS (rygel_last_change_entry_parent_class)->finalize (obj);
}


GType rygel_last_change_entry_get_type (void) {
	static volatile gsize rygel_last_change_entry_type_id__volatile = 0;
	if (g_once_init_enter (&rygel_last_change_entry_type_id__volatile)) {
		static const GTypeInfo g_define_type_info = { sizeof (RygelLastChangeEntryClass), (GBaseInitFunc) NULL, (GBaseFinalizeFunc) NULL, (GClassInitFunc) rygel_last_change_entry_class_init, (GClassFinalizeFunc) NULL, NULL, sizeof (RygelLastChangeEntry), 0, (GInstanceInitFunc) rygel_last_change_entry_instance_init, NULL };
		GType rygel_last_change_entry_type_id;
		rygel_last_change_entry_type_id = g_type_register_static (G_TYPE_OBJECT, "RygelLastChangeEntry", &g_define_type_info, G_TYPE_FLAG_ABSTRACT);
		g_once_init_leave (&rygel_last_change_entry_type_id__volatile, rygel_last_change_entry_type_id);
	}
	return rygel_last_change_entry_type_id__volatile;
}


