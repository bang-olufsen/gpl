/* listiterator.c generated by valac 0.27.1.26-9b1a5, the Vala compiler
 * generated from listiterator.vala, do not modify */

/* listiterator.vala
 *
 * Copyright (C) 2009  Didier Villevalois
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.

 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
 *
 * Author:
 * 	Didier 'Ptitjes Villevalois <ptitjes@free.fr>
 */

#include <glib.h>
#include <glib-object.h>


#define GEE_TYPE_TRAVERSABLE (gee_traversable_get_type ())
#define GEE_TRAVERSABLE(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), GEE_TYPE_TRAVERSABLE, GeeTraversable))
#define GEE_IS_TRAVERSABLE(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GEE_TYPE_TRAVERSABLE))
#define GEE_TRAVERSABLE_GET_INTERFACE(obj) (G_TYPE_INSTANCE_GET_INTERFACE ((obj), GEE_TYPE_TRAVERSABLE, GeeTraversableIface))

typedef struct _GeeTraversable GeeTraversable;
typedef struct _GeeTraversableIface GeeTraversableIface;

#define GEE_TRAVERSABLE_TYPE_STREAM (gee_traversable_stream_get_type ())

#define GEE_TYPE_LAZY (gee_lazy_get_type ())
#define GEE_LAZY(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), GEE_TYPE_LAZY, GeeLazy))
#define GEE_LAZY_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), GEE_TYPE_LAZY, GeeLazyClass))
#define GEE_IS_LAZY(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GEE_TYPE_LAZY))
#define GEE_IS_LAZY_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GEE_TYPE_LAZY))
#define GEE_LAZY_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), GEE_TYPE_LAZY, GeeLazyClass))

typedef struct _GeeLazy GeeLazy;
typedef struct _GeeLazyClass GeeLazyClass;

#define GEE_TYPE_ITERATOR (gee_iterator_get_type ())
#define GEE_ITERATOR(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), GEE_TYPE_ITERATOR, GeeIterator))
#define GEE_IS_ITERATOR(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GEE_TYPE_ITERATOR))
#define GEE_ITERATOR_GET_INTERFACE(obj) (G_TYPE_INSTANCE_GET_INTERFACE ((obj), GEE_TYPE_ITERATOR, GeeIteratorIface))

typedef struct _GeeIterator GeeIterator;
typedef struct _GeeIteratorIface GeeIteratorIface;

#define GEE_TYPE_LIST_ITERATOR (gee_list_iterator_get_type ())
#define GEE_LIST_ITERATOR(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), GEE_TYPE_LIST_ITERATOR, GeeListIterator))
#define GEE_IS_LIST_ITERATOR(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GEE_TYPE_LIST_ITERATOR))
#define GEE_LIST_ITERATOR_GET_INTERFACE(obj) (G_TYPE_INSTANCE_GET_INTERFACE ((obj), GEE_TYPE_LIST_ITERATOR, GeeListIteratorIface))

typedef struct _GeeListIterator GeeListIterator;
typedef struct _GeeListIteratorIface GeeListIteratorIface;

typedef gboolean (*GeeForallFunc) (gpointer g, void* user_data);
typedef enum  {
	GEE_TRAVERSABLE_STREAM_YIELD,
	GEE_TRAVERSABLE_STREAM_CONTINUE,
	GEE_TRAVERSABLE_STREAM_END,
	GEE_TRAVERSABLE_STREAM_WAIT
} GeeTraversableStream;

typedef GeeTraversableStream (*GeeStreamFunc) (GeeTraversableStream state, GeeLazy* g, GeeLazy** lazy, void* user_data);
typedef gpointer (*GeeFoldFunc) (gpointer g, gpointer a, void* user_data);
typedef gpointer (*GeeMapFunc) (gpointer g, void* user_data);
typedef gboolean (*GeePredicate) (gconstpointer g, void* user_data);
typedef GeeIterator* (*GeeFlatMapFunc) (gpointer g, void* user_data);
struct _GeeTraversableIface {
	GTypeInterface parent_iface;
	GType (*get_g_type) (GeeTraversable* self);
	GBoxedCopyFunc (*get_g_dup_func) (GeeTraversable* self);
	GDestroyNotify (*get_g_destroy_func) (GeeTraversable* self);
	gboolean (*foreach) (GeeTraversable* self, GeeForallFunc f, void* f_target);
	GeeIterator* (*stream) (GeeTraversable* self, GType a_type, GBoxedCopyFunc a_dup_func, GDestroyNotify a_destroy_func, GeeStreamFunc f, void* f_target, GDestroyNotify f_target_destroy_notify);
	gpointer (*fold) (GeeTraversable* self, GType a_type, GBoxedCopyFunc a_dup_func, GDestroyNotify a_destroy_func, GeeFoldFunc f, void* f_target, gpointer seed);
	GeeIterator* (*map) (GeeTraversable* self, GType a_type, GBoxedCopyFunc a_dup_func, GDestroyNotify a_destroy_func, GeeMapFunc f, void* f_target);
	GeeIterator* (*scan) (GeeTraversable* self, GType a_type, GBoxedCopyFunc a_dup_func, GDestroyNotify a_destroy_func, GeeFoldFunc f, void* f_target, gpointer seed);
	GeeIterator* (*filter) (GeeTraversable* self, GeePredicate pred, void* pred_target, GDestroyNotify pred_target_destroy_notify);
	GeeIterator* (*chop) (GeeTraversable* self, gint offset, gint length);
	GType (*get_element_type) (GeeTraversable* self);
	GeeIterator* (*flat_map) (GeeTraversable* self, GType a_type, GBoxedCopyFunc a_dup_func, GDestroyNotify a_destroy_func, GeeFlatMapFunc f, void* f_target, GDestroyNotify f_target_destroy_notify);
	GeeIterator** (*tee) (GeeTraversable* self, guint forks, int* result_length1);
};

struct _GeeIteratorIface {
	GTypeInterface parent_iface;
	gboolean (*next) (GeeIterator* self);
	gboolean (*has_next) (GeeIterator* self);
	gpointer (*get) (GeeIterator* self);
	void (*remove) (GeeIterator* self);
	gboolean (*get_valid) (GeeIterator* self);
	gboolean (*get_read_only) (GeeIterator* self);
};

struct _GeeListIteratorIface {
	GTypeInterface parent_iface;
	void (*set) (GeeListIterator* self, gconstpointer item);
	void (*add) (GeeListIterator* self, gconstpointer item);
	gint (*index) (GeeListIterator* self);
};



GType gee_traversable_stream_get_type (void) G_GNUC_CONST;
gpointer gee_lazy_ref (gpointer instance);
void gee_lazy_unref (gpointer instance);
GParamSpec* gee_param_spec_lazy (const gchar* name, const gchar* nick, const gchar* blurb, GType object_type, GParamFlags flags);
void gee_value_set_lazy (GValue* value, gpointer v_object);
void gee_value_take_lazy (GValue* value, gpointer v_object);
gpointer gee_value_get_lazy (const GValue* value);
GType gee_lazy_get_type (void) G_GNUC_CONST;
GType gee_traversable_get_type (void) G_GNUC_CONST;
GType gee_iterator_get_type (void) G_GNUC_CONST;
GType gee_list_iterator_get_type (void) G_GNUC_CONST;
void gee_list_iterator_set (GeeListIterator* self, gconstpointer item);
void gee_list_iterator_add (GeeListIterator* self, gconstpointer item);
gint gee_list_iterator_index (GeeListIterator* self);


/**
 * Sets the current item in the iteration to the specified new item.
 */
void gee_list_iterator_set (GeeListIterator* self, gconstpointer item) {
	g_return_if_fail (self != NULL);
	GEE_LIST_ITERATOR_GET_INTERFACE (self)->set (self, item);
}


/**
 * Adds the specified item after the current item in the iteration. The
 * iterator is moved to the point of the new added item.
 *
 * Please note that if iterator points in-between elements the element
 * is added after the current element and iterator point on it.
 */
void gee_list_iterator_add (GeeListIterator* self, gconstpointer item) {
	g_return_if_fail (self != NULL);
	GEE_LIST_ITERATOR_GET_INTERFACE (self)->add (self, item);
}


/**
 * Returns the current index in the iteration.
 *
 * @return the current index
 */
gint gee_list_iterator_index (GeeListIterator* self) {
	g_return_val_if_fail (self != NULL, 0);
	return GEE_LIST_ITERATOR_GET_INTERFACE (self)->index (self);
}


static void gee_list_iterator_base_init (GeeListIteratorIface * iface) {
	static gboolean initialized = FALSE;
	if (!initialized) {
		initialized = TRUE;
	}
}


/**
 * A list iterator. This supports bi-directionnal and index-based iteration.
 */
GType gee_list_iterator_get_type (void) {
	static volatile gsize gee_list_iterator_type_id__volatile = 0;
	if (g_once_init_enter (&gee_list_iterator_type_id__volatile)) {
		static const GTypeInfo g_define_type_info = { sizeof (GeeListIteratorIface), (GBaseInitFunc) gee_list_iterator_base_init, (GBaseFinalizeFunc) NULL, (GClassInitFunc) NULL, (GClassFinalizeFunc) NULL, NULL, 0, 0, (GInstanceInitFunc) NULL, NULL };
		GType gee_list_iterator_type_id;
		gee_list_iterator_type_id = g_type_register_static (G_TYPE_INTERFACE, "GeeListIterator", &g_define_type_info, 0);
		g_type_interface_add_prerequisite (gee_list_iterator_type_id, GEE_TYPE_ITERATOR);
		g_once_init_leave (&gee_list_iterator_type_id__volatile, gee_list_iterator_type_id);
	}
	return gee_list_iterator_type_id__volatile;
}


