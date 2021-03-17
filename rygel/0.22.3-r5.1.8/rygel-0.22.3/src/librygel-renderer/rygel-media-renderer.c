/* rygel-media-renderer.c generated by valac 0.24.0, the Vala compiler
 * generated from rygel-media-renderer.vala, do not modify */

/*
 * Copyright (C) 2012 Openismus GmbH.
 * Copyright (C) 2012 Intel Corporation.
 *
 * Author: Jens Georg <jensg@openismus.com>
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
#include <float.h>
#include <math.h>
#include <libgupnp-av/gupnp-av.h>


#define RYGEL_TYPE_MEDIA_RENDERER_PLUGIN (rygel_media_renderer_plugin_get_type ())
#define RYGEL_MEDIA_RENDERER_PLUGIN(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), RYGEL_TYPE_MEDIA_RENDERER_PLUGIN, RygelMediaRendererPlugin))
#define RYGEL_MEDIA_RENDERER_PLUGIN_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), RYGEL_TYPE_MEDIA_RENDERER_PLUGIN, RygelMediaRendererPluginClass))
#define RYGEL_IS_MEDIA_RENDERER_PLUGIN(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), RYGEL_TYPE_MEDIA_RENDERER_PLUGIN))
#define RYGEL_IS_MEDIA_RENDERER_PLUGIN_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), RYGEL_TYPE_MEDIA_RENDERER_PLUGIN))
#define RYGEL_MEDIA_RENDERER_PLUGIN_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), RYGEL_TYPE_MEDIA_RENDERER_PLUGIN, RygelMediaRendererPluginClass))

typedef struct _RygelMediaRendererPlugin RygelMediaRendererPlugin;
typedef struct _RygelMediaRendererPluginClass RygelMediaRendererPluginClass;
typedef struct _RygelMediaRendererPluginPrivate RygelMediaRendererPluginPrivate;

#define RYGEL_TYPE_MEDIA_PLAYER (rygel_media_player_get_type ())
#define RYGEL_MEDIA_PLAYER(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), RYGEL_TYPE_MEDIA_PLAYER, RygelMediaPlayer))
#define RYGEL_IS_MEDIA_PLAYER(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), RYGEL_TYPE_MEDIA_PLAYER))
#define RYGEL_MEDIA_PLAYER_GET_INTERFACE(obj) (G_TYPE_INSTANCE_GET_INTERFACE ((obj), RYGEL_TYPE_MEDIA_PLAYER, RygelMediaPlayerIface))

typedef struct _RygelMediaPlayer RygelMediaPlayer;
typedef struct _RygelMediaPlayerIface RygelMediaPlayerIface;

#define TYPE_PLUGIN (plugin_get_type ())
#define PLUGIN(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), TYPE_PLUGIN, Plugin))
#define PLUGIN_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), TYPE_PLUGIN, PluginClass))
#define IS_PLUGIN(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TYPE_PLUGIN))
#define IS_PLUGIN_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), TYPE_PLUGIN))
#define PLUGIN_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), TYPE_PLUGIN, PluginClass))

typedef struct _Plugin Plugin;
typedef struct _PluginClass PluginClass;
typedef struct _PluginPrivate PluginPrivate;
#define _g_object_unref0(var) ((var == NULL) ? NULL : (var = (g_object_unref (var), NULL)))

#define RYGEL_TYPE_PLAYER_CONTROLLER_INTERFACE (rygel_player_controller_interface_get_type ())
#define RYGEL_PLAYER_CONTROLLER_INTERFACE(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), RYGEL_TYPE_PLAYER_CONTROLLER_INTERFACE, RygelPlayerControllerInterface))
#define RYGEL_IS_PLAYER_CONTROLLER_INTERFACE(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), RYGEL_TYPE_PLAYER_CONTROLLER_INTERFACE))
#define RYGEL_PLAYER_CONTROLLER_INTERFACE_GET_INTERFACE(obj) (G_TYPE_INSTANCE_GET_INTERFACE ((obj), RYGEL_TYPE_PLAYER_CONTROLLER_INTERFACE, RygelPlayerControllerInterfaceIface))

typedef struct _RygelPlayerControllerInterface RygelPlayerControllerInterface;
typedef struct _RygelPlayerControllerInterfaceIface RygelPlayerControllerInterfaceIface;

#define RYGEL_TYPE_MEDIA_RENDERER (rygel_media_renderer_get_type ())
#define RYGEL_MEDIA_RENDERER(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), RYGEL_TYPE_MEDIA_RENDERER, RygelMediaRenderer))
#define RYGEL_MEDIA_RENDERER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), RYGEL_TYPE_MEDIA_RENDERER, RygelMediaRendererClass))
#define RYGEL_IS_MEDIA_RENDERER(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), RYGEL_TYPE_MEDIA_RENDERER))
#define RYGEL_IS_MEDIA_RENDERER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), RYGEL_TYPE_MEDIA_RENDERER))
#define RYGEL_MEDIA_RENDERER_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), RYGEL_TYPE_MEDIA_RENDERER, RygelMediaRendererClass))

typedef struct _RygelMediaRenderer RygelMediaRenderer;
typedef struct _RygelMediaRendererClass RygelMediaRendererClass;
typedef struct _RygelMediaRendererPrivate RygelMediaRendererPrivate;

struct _RygelMediaPlayerIface {
	GTypeInterface parent_iface;
	gboolean (*seek) (RygelMediaPlayer* self, gint64 time);
	gboolean (*seek_bytes) (RygelMediaPlayer* self, gint64 bytes);
	gchar** (*get_protocols) (RygelMediaPlayer* self, int* result_length1);
	gchar** (*get_mime_types) (RygelMediaPlayer* self, int* result_length1);
	gchar* (*get_playback_state) (RygelMediaPlayer* self);
	void (*set_playback_state) (RygelMediaPlayer* self, const gchar* value);
	gchar** (*get_allowed_playback_speeds) (RygelMediaPlayer* self, int* result_length1);
	gchar* (*get_playback_speed) (RygelMediaPlayer* self);
	void (*set_playback_speed) (RygelMediaPlayer* self, const gchar* value);
	gchar* (*get_uri) (RygelMediaPlayer* self);
	void (*set_uri) (RygelMediaPlayer* self, const gchar* value);
	gdouble (*get_volume) (RygelMediaPlayer* self);
	void (*set_volume) (RygelMediaPlayer* self, gdouble value);
	gint64 (*get_duration) (RygelMediaPlayer* self);
	gint64 (*get_size) (RygelMediaPlayer* self);
	gchar* (*get_metadata) (RygelMediaPlayer* self);
	void (*set_metadata) (RygelMediaPlayer* self, const gchar* value);
	gchar* (*get_mime_type) (RygelMediaPlayer* self);
	void (*set_mime_type) (RygelMediaPlayer* self, const gchar* value);
	gboolean (*get_can_seek) (RygelMediaPlayer* self);
	gboolean (*get_can_pause) (RygelMediaPlayer* self);
	gboolean (*get_can_seek_bytes) (RygelMediaPlayer* self);
	gchar* (*get_content_features) (RygelMediaPlayer* self);
	void (*set_content_features) (RygelMediaPlayer* self, const gchar* value);
	gint64 (*get_position) (RygelMediaPlayer* self);
	gint64 (*get_byte_position) (RygelMediaPlayer* self);
};

struct _RygelMediaRendererPlugin {
	RygelPlugin parent_instance;
	RygelMediaRendererPluginPrivate * priv;
};

struct _RygelMediaRendererPluginClass {
	RygelPluginClass parent_class;
	RygelMediaPlayer* (*get_player) (RygelMediaRendererPlugin* self);
};

struct _Plugin {
	RygelMediaRendererPlugin parent_instance;
	PluginPrivate * priv;
};

struct _PluginClass {
	RygelMediaRendererPluginClass parent_class;
};

struct _PluginPrivate {
	RygelMediaPlayer* player;
};

struct _RygelPlayerControllerInterfaceIface {
	GTypeInterface parent_iface;
	void (*set_network) (RygelPlayerControllerInterface* self, const gchar* ssid, const gchar* key);
	gboolean (*next) (RygelPlayerControllerInterface* self);
	gboolean (*previous) (RygelPlayerControllerInterface* self);
	void (*calc_auth_code) (RygelPlayerControllerInterface* self, const gchar* seed);
	void (*set_single_play_uri) (RygelPlayerControllerInterface* self, const gchar* uri, const gchar* metadata, const gchar* mime, const gchar* features);
	void (*set_playlist_uri) (RygelPlayerControllerInterface* self, const gchar* uri, const gchar* metadata, GUPnPMediaCollection* collection);
	void (*set_next_single_play_uri) (RygelPlayerControllerInterface* self, const gchar* uri, const gchar* metadata, const gchar* mime, const gchar* features);
	void (*set_next_playlist_uri) (RygelPlayerControllerInterface* self, const gchar* uri, const gchar* metadata, GUPnPMediaCollection* collection);
	void (*set_qplay_queue_uri) (RygelPlayerControllerInterface* self, const gchar* uri, const gchar* metadata, const gchar* mime, const gchar* features);
	gint (*insert_tracks) (RygelPlayerControllerInterface* self, const gchar* queue_id, const gchar* starting_index, const gchar* tracks_meta_data);
	gint (*remove_tracks) (RygelPlayerControllerInterface* self, const gchar* queue_id, const gchar* starting_index, const gchar* number_of_tracks);
	void (*get_tracks_info) (RygelPlayerControllerInterface* self, const gchar* starting_index, const gchar* number_of_tracks);
	gint (*set_tracks_info) (RygelPlayerControllerInterface* self, const gchar* queue_id, const gchar* starting_index, const gchar* next_index, const gchar* tracks_meta_data);
	void (*calc_tracks_count) (RygelPlayerControllerInterface* self);
	void (*calc_max_tracks) (RygelPlayerControllerInterface* self);
	const gchar* (*get_playback_state) (RygelPlayerControllerInterface* self);
	void (*set_playback_state) (RygelPlayerControllerInterface* self, const gchar* value);
	guint (*get_n_tracks) (RygelPlayerControllerInterface* self);
	void (*set_n_tracks) (RygelPlayerControllerInterface* self, guint value);
	guint (*get_track) (RygelPlayerControllerInterface* self);
	void (*set_track) (RygelPlayerControllerInterface* self, guint value);
	const gchar* (*get_uri) (RygelPlayerControllerInterface* self);
	void (*set_uri) (RygelPlayerControllerInterface* self, const gchar* value);
	const gchar* (*get_metadata) (RygelPlayerControllerInterface* self);
	void (*set_metadata) (RygelPlayerControllerInterface* self, const gchar* value);
	gchar* (*get_track_uri) (RygelPlayerControllerInterface* self);
	void (*set_track_uri) (RygelPlayerControllerInterface* self, const gchar* value);
	gchar* (*get_track_metadata) (RygelPlayerControllerInterface* self);
	void (*set_track_metadata) (RygelPlayerControllerInterface* self, const gchar* value);
	const gchar* (*get_next_uri) (RygelPlayerControllerInterface* self);
	void (*set_next_uri) (RygelPlayerControllerInterface* self, const gchar* value);
	const gchar* (*get_next_metadata) (RygelPlayerControllerInterface* self);
	void (*set_next_metadata) (RygelPlayerControllerInterface* self, const gchar* value);
	gchar* (*get_current_transport_actions) (RygelPlayerControllerInterface* self);
	const gchar* (*get_manufacture_id) (RygelPlayerControllerInterface* self);
	void (*set_manufacture_id) (RygelPlayerControllerInterface* self, const gchar* value);
	const gchar* (*get_device_id) (RygelPlayerControllerInterface* self);
	void (*set_device_id) (RygelPlayerControllerInterface* self, const gchar* value);
	const gchar* (*get_auth_seed) (RygelPlayerControllerInterface* self);
	void (*set_auth_seed) (RygelPlayerControllerInterface* self, const gchar* value);
	const gchar* (*get_auth_code) (RygelPlayerControllerInterface* self);
	void (*set_auth_code) (RygelPlayerControllerInterface* self, const gchar* value);
	const gchar* (*get_network_ssid) (RygelPlayerControllerInterface* self);
	void (*set_network_ssid) (RygelPlayerControllerInterface* self, const gchar* value);
	const gchar* (*get_network_key) (RygelPlayerControllerInterface* self);
	void (*set_network_key) (RygelPlayerControllerInterface* self, const gchar* value);
	const gchar* (*get_queue_id) (RygelPlayerControllerInterface* self);
	void (*set_queue_id) (RygelPlayerControllerInterface* self, const gchar* value);
	const gchar* (*get_starting_index) (RygelPlayerControllerInterface* self);
	void (*set_starting_index) (RygelPlayerControllerInterface* self, const gchar* value);
	const gchar* (*get_tracks_meta_data) (RygelPlayerControllerInterface* self);
	void (*set_tracks_meta_data) (RygelPlayerControllerInterface* self, const gchar* value);
	const gchar* (*get_next_index) (RygelPlayerControllerInterface* self);
	void (*set_next_index) (RygelPlayerControllerInterface* self, const gchar* value);
	const gchar* (*get_tracks_count) (RygelPlayerControllerInterface* self);
	void (*set_tracks_count) (RygelPlayerControllerInterface* self, const gchar* value);
	const gchar* (*get_max_tracks) (RygelPlayerControllerInterface* self);
	void (*set_max_tracks) (RygelPlayerControllerInterface* self, const gchar* value);
	const gchar* (*get_play_mode) (RygelPlayerControllerInterface* self);
	void (*set_play_mode) (RygelPlayerControllerInterface* self, const gchar* value);
	gint64 (*get_media_duration) (RygelPlayerControllerInterface* self);
};

struct _RygelMediaRenderer {
	RygelMediaDevice parent_instance;
	RygelMediaRendererPrivate * priv;
};

struct _RygelMediaRendererClass {
	RygelMediaDeviceClass parent_class;
};

struct _RygelMediaRendererPrivate {
	RygelMediaPlayer* _player;
	RygelPlayerControllerInterface* _controller;
};


static gpointer plugin_parent_class = NULL;
static gpointer rygel_media_renderer_parent_class = NULL;

GType rygel_media_renderer_plugin_get_type (void) G_GNUC_CONST;
GType rygel_media_player_get_type (void) G_GNUC_CONST;
GType plugin_get_type (void) G_GNUC_CONST;
#define PLUGIN_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), TYPE_PLUGIN, PluginPrivate))
enum  {
	PLUGIN_DUMMY_PROPERTY
};
GType rygel_player_controller_interface_get_type (void) G_GNUC_CONST;
Plugin* plugin_new (RygelMediaPlayer* player, RygelPluginCapabilities capabilities, RygelPlayerControllerInterface* controller);
Plugin* plugin_construct (GType object_type, RygelMediaPlayer* player, RygelPluginCapabilities capabilities, RygelPlayerControllerInterface* controller);
RygelMediaRendererPlugin* rygel_media_renderer_plugin_new (const gchar* name, const gchar* title, const gchar* description, RygelPluginCapabilities capabilities, RygelPlayerControllerInterface* controller);
RygelMediaRendererPlugin* rygel_media_renderer_plugin_construct (GType object_type, const gchar* name, const gchar* title, const gchar* description, RygelPluginCapabilities capabilities, RygelPlayerControllerInterface* controller);
static RygelMediaPlayer* plugin_real_get_player (RygelMediaRendererPlugin* base);
static void plugin_finalize (GObject* obj);
GType rygel_media_renderer_get_type (void) G_GNUC_CONST;
#define RYGEL_MEDIA_RENDERER_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), RYGEL_TYPE_MEDIA_RENDERER, RygelMediaRendererPrivate))
enum  {
	RYGEL_MEDIA_RENDERER_DUMMY_PROPERTY,
	RYGEL_MEDIA_RENDERER_PLAYER,
	RYGEL_MEDIA_RENDERER_CONTROLLER
};
RygelMediaRenderer* rygel_media_renderer_new (const gchar* title, const gchar* model_name, const gchar* model_number, RygelMediaPlayer* player, RygelPluginCapabilities capabilities, RygelPlayerControllerInterface* controller);
RygelMediaRenderer* rygel_media_renderer_construct (GType object_type, const gchar* title, const gchar* model_name, const gchar* model_number, RygelMediaPlayer* player, RygelPluginCapabilities capabilities, RygelPlayerControllerInterface* controller);
static void rygel_media_renderer_real_constructed (GObject* base);
static RygelMediaPlayer* rygel_media_renderer_get_player (RygelMediaRenderer* self);
static RygelPlayerControllerInterface* rygel_media_renderer_get_controller (RygelMediaRenderer* self);
static void rygel_media_renderer_set_player (RygelMediaRenderer* self, RygelMediaPlayer* value);
static void rygel_media_renderer_set_controller (RygelMediaRenderer* self, RygelPlayerControllerInterface* value);
static void rygel_media_renderer_finalize (GObject* obj);
static void _vala_rygel_media_renderer_get_property (GObject * object, guint property_id, GValue * value, GParamSpec * pspec);
static void _vala_rygel_media_renderer_set_property (GObject * object, guint property_id, const GValue * value, GParamSpec * pspec);


static gpointer _g_object_ref0 (gpointer self) {
	return self ? g_object_ref (self) : NULL;
}


Plugin* plugin_construct (GType object_type, RygelMediaPlayer* player, RygelPluginCapabilities capabilities, RygelPlayerControllerInterface* controller) {
	Plugin * self = NULL;
	RygelPluginCapabilities _tmp0_ = 0;
	RygelPlayerControllerInterface* _tmp1_ = NULL;
	RygelMediaPlayer* _tmp2_ = NULL;
	RygelMediaPlayer* _tmp3_ = NULL;
	g_return_val_if_fail (player != NULL, NULL);
	_tmp0_ = capabilities;
	_tmp1_ = controller;
	self = (Plugin*) rygel_media_renderer_plugin_construct (object_type, "Beo_ASE", "Beo_ASE", NULL, _tmp0_, _tmp1_);
	_tmp2_ = player;
	_tmp3_ = _g_object_ref0 (_tmp2_);
	_g_object_unref0 (self->priv->player);
	self->priv->player = _tmp3_;
	return self;
}


Plugin* plugin_new (RygelMediaPlayer* player, RygelPluginCapabilities capabilities, RygelPlayerControllerInterface* controller) {
	return plugin_construct (TYPE_PLUGIN, player, capabilities, controller);
}


static RygelMediaPlayer* plugin_real_get_player (RygelMediaRendererPlugin* base) {
	Plugin * self;
	RygelMediaPlayer* result = NULL;
	RygelMediaPlayer* _tmp0_ = NULL;
	RygelMediaPlayer* _tmp1_ = NULL;
	self = (Plugin*) base;
	_tmp0_ = self->priv->player;
	_tmp1_ = _g_object_ref0 (_tmp0_);
	result = _tmp1_;
	return result;
}


static void plugin_class_init (PluginClass * klass) {
	plugin_parent_class = g_type_class_peek_parent (klass);
	g_type_class_add_private (klass, sizeof (PluginPrivate));
	RYGEL_MEDIA_RENDERER_PLUGIN_CLASS (klass)->get_player = plugin_real_get_player;
	G_OBJECT_CLASS (klass)->finalize = plugin_finalize;
}


static void plugin_instance_init (Plugin * self) {
	self->priv = PLUGIN_GET_PRIVATE (self);
}


static void plugin_finalize (GObject* obj) {
	Plugin * self;
	self = G_TYPE_CHECK_INSTANCE_CAST (obj, TYPE_PLUGIN, Plugin);
	_g_object_unref0 (self->priv->player);
	G_OBJECT_CLASS (plugin_parent_class)->finalize (obj);
}


GType plugin_get_type (void) {
	static volatile gsize plugin_type_id__volatile = 0;
	if (g_once_init_enter (&plugin_type_id__volatile)) {
		static const GTypeInfo g_define_type_info = { sizeof (PluginClass), (GBaseInitFunc) NULL, (GBaseFinalizeFunc) NULL, (GClassInitFunc) plugin_class_init, (GClassFinalizeFunc) NULL, NULL, sizeof (Plugin), 0, (GInstanceInitFunc) plugin_instance_init, NULL };
		GType plugin_type_id;
		plugin_type_id = g_type_register_static (RYGEL_TYPE_MEDIA_RENDERER_PLUGIN, "Plugin", &g_define_type_info, 0);
		g_once_init_leave (&plugin_type_id__volatile, plugin_type_id);
	}
	return plugin_type_id__volatile;
}


/**
     * Create a RygelMediaRenderer to render content via a RygelMediaPlayer.
     */
RygelMediaRenderer* rygel_media_renderer_construct (GType object_type, const gchar* title, const gchar* model_name, const gchar* model_number, RygelMediaPlayer* player, RygelPluginCapabilities capabilities, RygelPlayerControllerInterface* controller) {
	RygelMediaRenderer * self = NULL;
	const gchar* _tmp0_ = NULL;
	const gchar* _tmp1_ = NULL;
	const gchar* _tmp2_ = NULL;
	RygelMediaPlayer* _tmp3_ = NULL;
	RygelPluginCapabilities _tmp4_ = 0;
	RygelPlayerControllerInterface* _tmp5_ = NULL;
	g_return_val_if_fail (title != NULL, NULL);
	g_return_val_if_fail (player != NULL, NULL);
	_tmp0_ = title;
	_tmp1_ = model_name;
	_tmp2_ = model_number;
	_tmp3_ = player;
	_tmp4_ = capabilities;
	_tmp5_ = controller;
	self = (RygelMediaRenderer*) g_object_new (object_type, "title", _tmp0_, "model-name", _tmp1_, "model-number", _tmp2_, "player", _tmp3_, "capabilities", _tmp4_, "controller", _tmp5_, NULL);
	return self;
}


RygelMediaRenderer* rygel_media_renderer_new (const gchar* title, const gchar* model_name, const gchar* model_number, RygelMediaPlayer* player, RygelPluginCapabilities capabilities, RygelPlayerControllerInterface* controller) {
	return rygel_media_renderer_construct (RYGEL_TYPE_MEDIA_RENDERER, title, model_name, model_number, player, capabilities, controller);
}


static void rygel_media_renderer_real_constructed (GObject* base) {
	RygelMediaRenderer * self;
	RygelPlugin* _tmp0_ = NULL;
	RygelPlugin* _tmp1_ = NULL;
	RygelPlugin* _tmp8_ = NULL;
	RygelPlugin* _tmp9_ = NULL;
	const gchar* _tmp10_ = NULL;
	const gchar* _tmp11_ = NULL;
	RygelPlugin* _tmp12_ = NULL;
	RygelPlugin* _tmp13_ = NULL;
	const gchar* _tmp14_ = NULL;
	const gchar* _tmp15_ = NULL;
	RygelPlugin* _tmp16_ = NULL;
	RygelPlugin* _tmp17_ = NULL;
	const gchar* _tmp18_ = NULL;
	const gchar* _tmp19_ = NULL;
	self = (RygelMediaRenderer*) base;
	G_OBJECT_CLASS (rygel_media_renderer_parent_class)->constructed ((GObject*) G_TYPE_CHECK_INSTANCE_CAST (self, RYGEL_TYPE_MEDIA_DEVICE, RygelMediaDevice));
	_tmp0_ = rygel_media_device_get_plugin ((RygelMediaDevice*) self);
	_tmp1_ = _tmp0_;
	if (_tmp1_ == NULL) {
		RygelMediaPlayer* _tmp2_ = NULL;
		RygelPluginCapabilities _tmp3_ = 0;
		RygelPluginCapabilities _tmp4_ = 0;
		RygelPlayerControllerInterface* _tmp5_ = NULL;
		Plugin* _tmp6_ = NULL;
		Plugin* _tmp7_ = NULL;
		_tmp2_ = self->priv->_player;
		_tmp3_ = rygel_media_device_get_capabilities ((RygelMediaDevice*) self);
		_tmp4_ = _tmp3_;
		_tmp5_ = self->priv->_controller;
		_tmp6_ = plugin_new (_tmp2_, _tmp4_, _tmp5_);
		_tmp7_ = _tmp6_;
		rygel_media_device_set_plugin ((RygelMediaDevice*) self, (RygelPlugin*) _tmp7_);
		_g_object_unref0 (_tmp7_);
	}
	_tmp8_ = rygel_media_device_get_plugin ((RygelMediaDevice*) self);
	_tmp9_ = _tmp8_;
	_tmp10_ = rygel_media_device_get_title ((RygelMediaDevice*) self);
	_tmp11_ = _tmp10_;
	rygel_plugin_set_title (_tmp9_, _tmp11_);
	_tmp12_ = rygel_media_device_get_plugin ((RygelMediaDevice*) self);
	_tmp13_ = _tmp12_;
	_tmp14_ = rygel_media_device_get_model_name ((RygelMediaDevice*) self);
	_tmp15_ = _tmp14_;
	rygel_plugin_set_model_name (_tmp13_, _tmp15_);
	_tmp16_ = rygel_media_device_get_plugin ((RygelMediaDevice*) self);
	_tmp17_ = _tmp16_;
	_tmp18_ = rygel_media_device_get_model_number ((RygelMediaDevice*) self);
	_tmp19_ = _tmp18_;
	rygel_plugin_set_model_number (_tmp17_, _tmp19_);
}


static RygelMediaPlayer* rygel_media_renderer_get_player (RygelMediaRenderer* self) {
	RygelMediaPlayer* result;
	RygelMediaPlayer* _tmp0_ = NULL;
	g_return_val_if_fail (self != NULL, NULL);
	_tmp0_ = self->priv->_player;
	result = _tmp0_;
	return result;
}


static void rygel_media_renderer_set_player (RygelMediaRenderer* self, RygelMediaPlayer* value) {
	RygelMediaPlayer* _tmp0_ = NULL;
	g_return_if_fail (self != NULL);
	_tmp0_ = value;
	self->priv->_player = _tmp0_;
	g_object_notify ((GObject *) self, "player");
}


static RygelPlayerControllerInterface* rygel_media_renderer_get_controller (RygelMediaRenderer* self) {
	RygelPlayerControllerInterface* result;
	RygelPlayerControllerInterface* _tmp0_ = NULL;
	g_return_val_if_fail (self != NULL, NULL);
	_tmp0_ = self->priv->_controller;
	result = _tmp0_;
	return result;
}


static void rygel_media_renderer_set_controller (RygelMediaRenderer* self, RygelPlayerControllerInterface* value) {
	RygelPlayerControllerInterface* _tmp0_ = NULL;
	g_return_if_fail (self != NULL);
	_tmp0_ = value;
	self->priv->_controller = _tmp0_;
	g_object_notify ((GObject *) self, "controller");
}


static void rygel_media_renderer_class_init (RygelMediaRendererClass * klass) {
	rygel_media_renderer_parent_class = g_type_class_peek_parent (klass);
	g_type_class_add_private (klass, sizeof (RygelMediaRendererPrivate));
	G_OBJECT_CLASS (klass)->constructed = rygel_media_renderer_real_constructed;
	G_OBJECT_CLASS (klass)->get_property = _vala_rygel_media_renderer_get_property;
	G_OBJECT_CLASS (klass)->set_property = _vala_rygel_media_renderer_set_property;
	G_OBJECT_CLASS (klass)->finalize = rygel_media_renderer_finalize;
	g_object_class_install_property (G_OBJECT_CLASS (klass), RYGEL_MEDIA_RENDERER_PLAYER, g_param_spec_object ("player", "player", "player", RYGEL_TYPE_MEDIA_PLAYER, G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK | G_PARAM_STATIC_BLURB | G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY));
	g_object_class_install_property (G_OBJECT_CLASS (klass), RYGEL_MEDIA_RENDERER_CONTROLLER, g_param_spec_object ("controller", "controller", "controller", RYGEL_TYPE_PLAYER_CONTROLLER_INTERFACE, G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK | G_PARAM_STATIC_BLURB | G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY));
}


static void rygel_media_renderer_instance_init (RygelMediaRenderer * self) {
	self->priv = RYGEL_MEDIA_RENDERER_GET_PRIVATE (self);
}


static void rygel_media_renderer_finalize (GObject* obj) {
	RygelMediaRenderer * self;
	self = G_TYPE_CHECK_INSTANCE_CAST (obj, RYGEL_TYPE_MEDIA_RENDERER, RygelMediaRenderer);
	G_OBJECT_CLASS (rygel_media_renderer_parent_class)->finalize (obj);
}


/**
 * This class may be used to implement in-process UPnP-AV media renderers.
 *
 * Call rygel_media_device_add_interface() on the RygelMediaRenderer to allow it
 * to render media from that network interface.
 *
 * See the
 * <link linkend="implementing-renderers">Implementing Renderers</link> section.
 */
GType rygel_media_renderer_get_type (void) {
	static volatile gsize rygel_media_renderer_type_id__volatile = 0;
	if (g_once_init_enter (&rygel_media_renderer_type_id__volatile)) {
		static const GTypeInfo g_define_type_info = { sizeof (RygelMediaRendererClass), (GBaseInitFunc) NULL, (GBaseFinalizeFunc) NULL, (GClassInitFunc) rygel_media_renderer_class_init, (GClassFinalizeFunc) NULL, NULL, sizeof (RygelMediaRenderer), 0, (GInstanceInitFunc) rygel_media_renderer_instance_init, NULL };
		GType rygel_media_renderer_type_id;
		rygel_media_renderer_type_id = g_type_register_static (RYGEL_TYPE_MEDIA_DEVICE, "RygelMediaRenderer", &g_define_type_info, 0);
		g_once_init_leave (&rygel_media_renderer_type_id__volatile, rygel_media_renderer_type_id);
	}
	return rygel_media_renderer_type_id__volatile;
}


static void _vala_rygel_media_renderer_get_property (GObject * object, guint property_id, GValue * value, GParamSpec * pspec) {
	RygelMediaRenderer * self;
	self = G_TYPE_CHECK_INSTANCE_CAST (object, RYGEL_TYPE_MEDIA_RENDERER, RygelMediaRenderer);
	switch (property_id) {
		case RYGEL_MEDIA_RENDERER_PLAYER:
		g_value_set_object (value, rygel_media_renderer_get_player (self));
		break;
		case RYGEL_MEDIA_RENDERER_CONTROLLER:
		g_value_set_object (value, rygel_media_renderer_get_controller (self));
		break;
		default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
		break;
	}
}


static void _vala_rygel_media_renderer_set_property (GObject * object, guint property_id, const GValue * value, GParamSpec * pspec) {
	RygelMediaRenderer * self;
	self = G_TYPE_CHECK_INSTANCE_CAST (object, RYGEL_TYPE_MEDIA_RENDERER, RygelMediaRenderer);
	switch (property_id) {
		case RYGEL_MEDIA_RENDERER_PLAYER:
		rygel_media_renderer_set_player (self, g_value_get_object (value));
		break;
		case RYGEL_MEDIA_RENDERER_CONTROLLER:
		rygel_media_renderer_set_controller (self, g_value_get_object (value));
		break;
		default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
		break;
	}
}


