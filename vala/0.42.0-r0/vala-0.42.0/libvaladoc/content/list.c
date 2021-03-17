/* list.c generated by valac, the Vala compiler
 * generated from list.vala, do not modify */

/* list.vala
 *
 * Copyright (C) 2008-2009 Didier Villevalois
 * Copyright (C) 2008-2012 Florian Brosch
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
 *
 * Author:
 * 	Didier 'Ptitjes Villevalois <ptitjes@free.fr>
 */


#include <glib.h>
#include <glib-object.h>
#include "valadoc.h"
#include <valagee.h>
#include <stdlib.h>
#include <string.h>

enum  {
	VALADOC_CONTENT_LIST_0_PROPERTY,
	VALADOC_CONTENT_LIST_BULLET_PROPERTY,
	VALADOC_CONTENT_LIST_ITEMS_PROPERTY,
	VALADOC_CONTENT_LIST_NUM_PROPERTIES
};
static GParamSpec* valadoc_content_list_properties[VALADOC_CONTENT_LIST_NUM_PROPERTIES];
#define _vala_iterable_unref0(var) ((var == NULL) ? NULL : (var = (vala_iterable_unref (var), NULL)))
#define _g_object_unref0(var) ((var == NULL) ? NULL : (var = (g_object_unref (var), NULL)))
#define _vala_assert(expr, msg) if G_LIKELY (expr) ; else g_assertion_message_expr (G_LOG_DOMAIN, __FILE__, __LINE__, G_STRFUNC, msg);
#define _vala_return_if_fail(expr, msg) if G_LIKELY (expr) ; else { g_return_if_fail_warning (G_LOG_DOMAIN, G_STRFUNC, msg); return; }
#define _vala_return_val_if_fail(expr, msg, val) if G_LIKELY (expr) ; else { g_return_if_fail_warning (G_LOG_DOMAIN, G_STRFUNC, msg); return val; }
#define _vala_warn_if_fail(expr, msg) if G_LIKELY (expr) ; else g_warn_message (G_LOG_DOMAIN, __FILE__, __LINE__, G_STRFUNC, msg);

struct _ValadocContentListPrivate {
	ValadocContentListBullet _bullet;
	ValaList* _items;
};


static gint ValadocContentList_private_offset;
static gpointer valadoc_content_list_parent_class = NULL;
static ValadocContentBlockIface * valadoc_content_list_valadoc_content_block_parent_iface = NULL;

static ValadocContentListBullet* _valadoc_content_list_bullet_dup (ValadocContentListBullet* self);
G_GNUC_INTERNAL ValadocContentList* valadoc_content_list_new (void);
G_GNUC_INTERNAL ValadocContentList* valadoc_content_list_construct (GType object_type);
static void valadoc_content_list_real_check (ValadocContentContentElement* base,
                                      ValadocApiTree* api_root,
                                      ValadocApiNode* container,
                                      const gchar* file_path,
                                      ValadocErrorReporter* reporter,
                                      ValadocSettings* settings);
G_GNUC_INTERNAL void valadoc_content_content_element_set_parent (ValadocContentContentElement* self,
                                                 ValadocContentContentElement* value);
static void valadoc_content_list_real_accept (ValadocContentContentElement* base,
                                       ValadocContentContentVisitor* visitor);
static void valadoc_content_list_real_accept_children (ValadocContentContentElement* base,
                                                ValadocContentContentVisitor* visitor);
static gboolean valadoc_content_list_real_is_empty (ValadocContentContentElement* base);
static ValadocContentContentElement* valadoc_content_list_real_copy (ValadocContentContentElement* base,
                                                              ValadocContentContentElement* new_parent);
static void valadoc_content_list_finalize (GObject * obj);
static void _vala_valadoc_content_list_get_property (GObject * object,
                                              guint property_id,
                                              GValue * value,
                                              GParamSpec * pspec);
static void _vala_valadoc_content_list_set_property (GObject * object,
                                              guint property_id,
                                              const GValue * value,
                                              GParamSpec * pspec);


static inline gpointer
valadoc_content_list_get_instance_private (ValadocContentList* self)
{
	return G_STRUCT_MEMBER_P (self, ValadocContentList_private_offset);
}


static ValadocContentListBullet*
_valadoc_content_list_bullet_dup (ValadocContentListBullet* self)
{
	ValadocContentListBullet* dup;
	dup = g_new0 (ValadocContentListBullet, 1);
	memcpy (dup, self, sizeof (ValadocContentListBullet));
	return dup;
}


static gpointer
__valadoc_content_list_bullet_dup0 (gpointer self)
{
	return self ? _valadoc_content_list_bullet_dup (self) : NULL;
}


ValadocContentListBullet*
valadoc_content_list_bullet_from_string (const gchar* str)
{
	ValadocContentListBullet* result = NULL;
	const gchar* _tmp0_;
	GQuark _tmp2_ = 0U;
	static GQuark _tmp1_label0 = 0;
	static GQuark _tmp1_label1 = 0;
	static GQuark _tmp1_label2 = 0;
	static GQuark _tmp1_label3 = 0;
	static GQuark _tmp1_label4 = 0;
	static GQuark _tmp1_label5 = 0;
	static GQuark _tmp1_label6 = 0;
	static GQuark _tmp1_label7 = 0;
	_tmp0_ = str;
	_tmp2_ = (NULL == _tmp0_) ? 0 : g_quark_from_string (_tmp0_);
	if (_tmp2_ == ((0 != _tmp1_label0) ? _tmp1_label0 : (_tmp1_label0 = g_quark_from_static_string ("none")))) {
		switch (0) {
			default:
			{
				ValadocContentListBullet _tmp3_;
				ValadocContentListBullet* _tmp4_;
				_tmp3_ = VALADOC_CONTENT_LIST_BULLET_NONE;
				_tmp4_ = __valadoc_content_list_bullet_dup0 (&_tmp3_);
				result = _tmp4_;
				return result;
			}
		}
	} else if (_tmp2_ == ((0 != _tmp1_label1) ? _tmp1_label1 : (_tmp1_label1 = g_quark_from_static_string ("unordered")))) {
		switch (0) {
			default:
			{
				ValadocContentListBullet _tmp5_;
				ValadocContentListBullet* _tmp6_;
				_tmp5_ = VALADOC_CONTENT_LIST_BULLET_UNORDERED;
				_tmp6_ = __valadoc_content_list_bullet_dup0 (&_tmp5_);
				result = _tmp6_;
				return result;
			}
		}
	} else if (_tmp2_ == ((0 != _tmp1_label2) ? _tmp1_label2 : (_tmp1_label2 = g_quark_from_static_string ("ordered")))) {
		switch (0) {
			default:
			{
				ValadocContentListBullet _tmp7_;
				ValadocContentListBullet* _tmp8_;
				_tmp7_ = VALADOC_CONTENT_LIST_BULLET_ORDERED;
				_tmp8_ = __valadoc_content_list_bullet_dup0 (&_tmp7_);
				result = _tmp8_;
				return result;
			}
		}
	} else if (_tmp2_ == ((0 != _tmp1_label3) ? _tmp1_label3 : (_tmp1_label3 = g_quark_from_static_string ("ordered-number")))) {
		switch (0) {
			default:
			{
				ValadocContentListBullet _tmp9_;
				ValadocContentListBullet* _tmp10_;
				_tmp9_ = VALADOC_CONTENT_LIST_BULLET_ORDERED_NUMBER;
				_tmp10_ = __valadoc_content_list_bullet_dup0 (&_tmp9_);
				result = _tmp10_;
				return result;
			}
		}
	} else if (_tmp2_ == ((0 != _tmp1_label4) ? _tmp1_label4 : (_tmp1_label4 = g_quark_from_static_string ("ordered-lower-case-alpa")))) {
		switch (0) {
			default:
			{
				ValadocContentListBullet _tmp11_;
				ValadocContentListBullet* _tmp12_;
				_tmp11_ = VALADOC_CONTENT_LIST_BULLET_ORDERED_LOWER_CASE_ALPHA;
				_tmp12_ = __valadoc_content_list_bullet_dup0 (&_tmp11_);
				result = _tmp12_;
				return result;
			}
		}
	} else if (_tmp2_ == ((0 != _tmp1_label5) ? _tmp1_label5 : (_tmp1_label5 = g_quark_from_static_string ("ordered-upper-case-alpha")))) {
		switch (0) {
			default:
			{
				ValadocContentListBullet _tmp13_;
				ValadocContentListBullet* _tmp14_;
				_tmp13_ = VALADOC_CONTENT_LIST_BULLET_ORDERED_UPPER_CASE_ALPHA;
				_tmp14_ = __valadoc_content_list_bullet_dup0 (&_tmp13_);
				result = _tmp14_;
				return result;
			}
		}
	} else if (_tmp2_ == ((0 != _tmp1_label6) ? _tmp1_label6 : (_tmp1_label6 = g_quark_from_static_string ("ordered-lower-case-roman")))) {
		switch (0) {
			default:
			{
				ValadocContentListBullet _tmp15_;
				ValadocContentListBullet* _tmp16_;
				_tmp15_ = VALADOC_CONTENT_LIST_BULLET_ORDERED_LOWER_CASE_ROMAN;
				_tmp16_ = __valadoc_content_list_bullet_dup0 (&_tmp15_);
				result = _tmp16_;
				return result;
			}
		}
	} else if (_tmp2_ == ((0 != _tmp1_label7) ? _tmp1_label7 : (_tmp1_label7 = g_quark_from_static_string ("ordered-upper-case-roman")))) {
		switch (0) {
			default:
			{
				ValadocContentListBullet _tmp17_;
				ValadocContentListBullet* _tmp18_;
				_tmp17_ = VALADOC_CONTENT_LIST_BULLET_ORDERED_UPPER_CASE_ROMAN;
				_tmp18_ = __valadoc_content_list_bullet_dup0 (&_tmp17_);
				result = _tmp18_;
				return result;
			}
		}
	}
	result = NULL;
	return result;
}


const gchar*
valadoc_content_list_bullet_to_string (ValadocContentListBullet self)
{
	const gchar* result = NULL;
	switch (self) {
		case VALADOC_CONTENT_LIST_BULLET_NONE:
		{
			result = "none";
			return result;
		}
		case VALADOC_CONTENT_LIST_BULLET_UNORDERED:
		{
			result = "unordered";
			return result;
		}
		case VALADOC_CONTENT_LIST_BULLET_ORDERED:
		{
			result = "ordered";
			return result;
		}
		case VALADOC_CONTENT_LIST_BULLET_ORDERED_NUMBER:
		{
			result = "ordered-number";
			return result;
		}
		case VALADOC_CONTENT_LIST_BULLET_ORDERED_LOWER_CASE_ALPHA:
		{
			result = "ordered-lower-case-alpa";
			return result;
		}
		case VALADOC_CONTENT_LIST_BULLET_ORDERED_UPPER_CASE_ALPHA:
		{
			result = "ordered-upper-case-alpha";
			return result;
		}
		case VALADOC_CONTENT_LIST_BULLET_ORDERED_LOWER_CASE_ROMAN:
		{
			result = "ordered-lower-case-roman";
			return result;
		}
		case VALADOC_CONTENT_LIST_BULLET_ORDERED_UPPER_CASE_ROMAN:
		{
			result = "ordered-upper-case-roman";
			return result;
		}
		default:
		break;
	}
	_vala_assert (TRUE, "true");
	result = "";
	return result;
}


GType
valadoc_content_list_bullet_get_type (void)
{
	static volatile gsize valadoc_content_list_bullet_type_id__volatile = 0;
	if (g_once_init_enter (&valadoc_content_list_bullet_type_id__volatile)) {
		static const GEnumValue values[] = {{VALADOC_CONTENT_LIST_BULLET_NONE, "VALADOC_CONTENT_LIST_BULLET_NONE", "none"}, {VALADOC_CONTENT_LIST_BULLET_UNORDERED, "VALADOC_CONTENT_LIST_BULLET_UNORDERED", "unordered"}, {VALADOC_CONTENT_LIST_BULLET_ORDERED, "VALADOC_CONTENT_LIST_BULLET_ORDERED", "ordered"}, {VALADOC_CONTENT_LIST_BULLET_ORDERED_NUMBER, "VALADOC_CONTENT_LIST_BULLET_ORDERED_NUMBER", "ordered-number"}, {VALADOC_CONTENT_LIST_BULLET_ORDERED_LOWER_CASE_ALPHA, "VALADOC_CONTENT_LIST_BULLET_ORDERED_LOWER_CASE_ALPHA", "ordered-lower-case-alpha"}, {VALADOC_CONTENT_LIST_BULLET_ORDERED_UPPER_CASE_ALPHA, "VALADOC_CONTENT_LIST_BULLET_ORDERED_UPPER_CASE_ALPHA", "ordered-upper-case-alpha"}, {VALADOC_CONTENT_LIST_BULLET_ORDERED_LOWER_CASE_ROMAN, "VALADOC_CONTENT_LIST_BULLET_ORDERED_LOWER_CASE_ROMAN", "ordered-lower-case-roman"}, {VALADOC_CONTENT_LIST_BULLET_ORDERED_UPPER_CASE_ROMAN, "VALADOC_CONTENT_LIST_BULLET_ORDERED_UPPER_CASE_ROMAN", "ordered-upper-case-roman"}, {0, NULL, NULL}};
		GType valadoc_content_list_bullet_type_id;
		valadoc_content_list_bullet_type_id = g_enum_register_static ("ValadocContentListBullet", values);
		g_once_init_leave (&valadoc_content_list_bullet_type_id__volatile, valadoc_content_list_bullet_type_id);
	}
	return valadoc_content_list_bullet_type_id__volatile;
}


G_GNUC_INTERNAL ValadocContentList*
valadoc_content_list_construct (GType object_type)
{
	ValadocContentList * self = NULL;
	GEqualFunc _tmp0_;
	ValaArrayList* _tmp1_;
	self = (ValadocContentList*) valadoc_content_content_element_construct (object_type);
	self->priv->_bullet = VALADOC_CONTENT_LIST_BULLET_NONE;
	_tmp0_ = g_direct_equal;
	_tmp1_ = vala_array_list_new (VALADOC_CONTENT_TYPE_LIST_ITEM, (GBoxedCopyFunc) g_object_ref, (GDestroyNotify) g_object_unref, _tmp0_);
	_vala_iterable_unref0 (self->priv->_items);
	self->priv->_items = (ValaList*) _tmp1_;
	return self;
}


G_GNUC_INTERNAL ValadocContentList*
valadoc_content_list_new (void)
{
	return valadoc_content_list_construct (VALADOC_CONTENT_TYPE_LIST);
}


static gpointer
_vala_iterable_ref0 (gpointer self)
{
	return self ? vala_iterable_ref (self) : NULL;
}


static void
valadoc_content_list_real_check (ValadocContentContentElement* base,
                                 ValadocApiTree* api_root,
                                 ValadocApiNode* container,
                                 const gchar* file_path,
                                 ValadocErrorReporter* reporter,
                                 ValadocSettings* settings)
{
	ValadocContentList * self;
	self = (ValadocContentList*) base;
	g_return_if_fail (api_root != NULL);
	g_return_if_fail (container != NULL);
	g_return_if_fail (file_path != NULL);
	g_return_if_fail (reporter != NULL);
	g_return_if_fail (settings != NULL);
	{
		ValaList* _element_list = NULL;
		ValaList* _tmp0_;
		ValaList* _tmp1_;
		gint _element_size = 0;
		ValaList* _tmp2_;
		gint _tmp3_;
		gint _tmp4_;
		gint _element_index = 0;
		_tmp0_ = self->priv->_items;
		_tmp1_ = _vala_iterable_ref0 (_tmp0_);
		_element_list = _tmp1_;
		_tmp2_ = _element_list;
		_tmp3_ = vala_collection_get_size ((ValaCollection*) _tmp2_);
		_tmp4_ = _tmp3_;
		_element_size = _tmp4_;
		_element_index = -1;
		while (TRUE) {
			gint _tmp5_;
			gint _tmp6_;
			gint _tmp7_;
			ValadocContentListItem* element = NULL;
			ValaList* _tmp8_;
			gint _tmp9_;
			gpointer _tmp10_;
			ValadocContentListItem* _tmp11_;
			ValadocContentListItem* _tmp12_;
			_tmp5_ = _element_index;
			_element_index = _tmp5_ + 1;
			_tmp6_ = _element_index;
			_tmp7_ = _element_size;
			if (!(_tmp6_ < _tmp7_)) {
				break;
			}
			_tmp8_ = _element_list;
			_tmp9_ = _element_index;
			_tmp10_ = vala_list_get (_tmp8_, _tmp9_);
			element = (ValadocContentListItem*) _tmp10_;
			_tmp11_ = element;
			valadoc_content_content_element_set_parent ((ValadocContentContentElement*) _tmp11_, (ValadocContentContentElement*) self);
			_tmp12_ = element;
			valadoc_content_content_element_check ((ValadocContentContentElement*) _tmp12_, api_root, container, file_path, reporter, settings);
			_g_object_unref0 (element);
		}
		_vala_iterable_unref0 (_element_list);
	}
}


static void
valadoc_content_list_real_accept (ValadocContentContentElement* base,
                                  ValadocContentContentVisitor* visitor)
{
	ValadocContentList * self;
	self = (ValadocContentList*) base;
	g_return_if_fail (visitor != NULL);
	valadoc_content_content_visitor_visit_list (visitor, self);
}


static void
valadoc_content_list_real_accept_children (ValadocContentContentElement* base,
                                           ValadocContentContentVisitor* visitor)
{
	ValadocContentList * self;
	self = (ValadocContentList*) base;
	g_return_if_fail (visitor != NULL);
	{
		ValaList* _element_list = NULL;
		ValaList* _tmp0_;
		ValaList* _tmp1_;
		gint _element_size = 0;
		ValaList* _tmp2_;
		gint _tmp3_;
		gint _tmp4_;
		gint _element_index = 0;
		_tmp0_ = self->priv->_items;
		_tmp1_ = _vala_iterable_ref0 (_tmp0_);
		_element_list = _tmp1_;
		_tmp2_ = _element_list;
		_tmp3_ = vala_collection_get_size ((ValaCollection*) _tmp2_);
		_tmp4_ = _tmp3_;
		_element_size = _tmp4_;
		_element_index = -1;
		while (TRUE) {
			gint _tmp5_;
			gint _tmp6_;
			gint _tmp7_;
			ValadocContentListItem* element = NULL;
			ValaList* _tmp8_;
			gint _tmp9_;
			gpointer _tmp10_;
			ValadocContentListItem* _tmp11_;
			_tmp5_ = _element_index;
			_element_index = _tmp5_ + 1;
			_tmp6_ = _element_index;
			_tmp7_ = _element_size;
			if (!(_tmp6_ < _tmp7_)) {
				break;
			}
			_tmp8_ = _element_list;
			_tmp9_ = _element_index;
			_tmp10_ = vala_list_get (_tmp8_, _tmp9_);
			element = (ValadocContentListItem*) _tmp10_;
			_tmp11_ = element;
			valadoc_content_content_element_accept ((ValadocContentContentElement*) _tmp11_, visitor);
			_g_object_unref0 (element);
		}
		_vala_iterable_unref0 (_element_list);
	}
}


static gboolean
valadoc_content_list_real_is_empty (ValadocContentContentElement* base)
{
	ValadocContentList * self;
	gboolean result = FALSE;
	ValaList* _tmp0_;
	gint _tmp1_;
	gint _tmp2_;
	self = (ValadocContentList*) base;
	_tmp0_ = self->priv->_items;
	_tmp1_ = vala_collection_get_size ((ValaCollection*) _tmp0_);
	_tmp2_ = _tmp1_;
	result = _tmp2_ == 0;
	return result;
}


static ValadocContentContentElement*
valadoc_content_list_real_copy (ValadocContentContentElement* base,
                                ValadocContentContentElement* new_parent)
{
	ValadocContentList * self;
	ValadocContentContentElement* result = NULL;
	ValadocContentList* list = NULL;
	ValadocContentList* _tmp0_;
	ValadocContentList* _tmp1_;
	ValadocContentList* _tmp2_;
	ValadocContentListBullet _tmp3_;
	self = (ValadocContentList*) base;
	_tmp0_ = valadoc_content_list_new ();
	list = _tmp0_;
	_tmp1_ = list;
	valadoc_content_content_element_set_parent ((ValadocContentContentElement*) _tmp1_, new_parent);
	_tmp2_ = list;
	_tmp3_ = self->priv->_bullet;
	valadoc_content_list_set_bullet (_tmp2_, _tmp3_);
	{
		ValaList* _item_list = NULL;
		ValaList* _tmp4_;
		ValaList* _tmp5_;
		ValaList* _tmp6_;
		gint _item_size = 0;
		ValaList* _tmp7_;
		gint _tmp8_;
		gint _tmp9_;
		gint _item_index = 0;
		_tmp4_ = valadoc_content_list_get_items (self);
		_tmp5_ = _tmp4_;
		_tmp6_ = _vala_iterable_ref0 (_tmp5_);
		_item_list = _tmp6_;
		_tmp7_ = _item_list;
		_tmp8_ = vala_collection_get_size ((ValaCollection*) _tmp7_);
		_tmp9_ = _tmp8_;
		_item_size = _tmp9_;
		_item_index = -1;
		while (TRUE) {
			gint _tmp10_;
			gint _tmp11_;
			gint _tmp12_;
			ValadocContentListItem* item = NULL;
			ValaList* _tmp13_;
			gint _tmp14_;
			gpointer _tmp15_;
			ValadocContentListItem* copy = NULL;
			ValadocContentListItem* _tmp16_;
			ValadocContentList* _tmp17_;
			ValadocContentContentElement* _tmp18_;
			ValadocContentListItem* _tmp19_;
			ValadocContentList* _tmp20_;
			ValaList* _tmp21_;
			ValaList* _tmp22_;
			ValadocContentListItem* _tmp23_;
			_tmp10_ = _item_index;
			_item_index = _tmp10_ + 1;
			_tmp11_ = _item_index;
			_tmp12_ = _item_size;
			if (!(_tmp11_ < _tmp12_)) {
				break;
			}
			_tmp13_ = _item_list;
			_tmp14_ = _item_index;
			_tmp15_ = vala_list_get (_tmp13_, _tmp14_);
			item = (ValadocContentListItem*) _tmp15_;
			_tmp16_ = item;
			_tmp17_ = list;
			_tmp18_ = valadoc_content_content_element_copy ((ValadocContentContentElement*) _tmp16_, (ValadocContentContentElement*) _tmp17_);
			_tmp19_ = G_TYPE_CHECK_INSTANCE_TYPE (_tmp18_, VALADOC_CONTENT_TYPE_LIST_ITEM) ? ((ValadocContentListItem*) _tmp18_) : NULL;
			if (_tmp19_ == NULL) {
				_g_object_unref0 (_tmp18_);
			}
			copy = _tmp19_;
			_tmp20_ = list;
			_tmp21_ = valadoc_content_list_get_items (_tmp20_);
			_tmp22_ = _tmp21_;
			_tmp23_ = copy;
			vala_collection_add ((ValaCollection*) _tmp22_, _tmp23_);
			_g_object_unref0 (copy);
			_g_object_unref0 (item);
		}
		_vala_iterable_unref0 (_item_list);
	}
	result = (ValadocContentContentElement*) list;
	return result;
}


ValadocContentListBullet
valadoc_content_list_get_bullet (ValadocContentList* self)
{
	ValadocContentListBullet result;
	ValadocContentListBullet _tmp0_;
	g_return_val_if_fail (self != NULL, 0);
	_tmp0_ = self->priv->_bullet;
	result = _tmp0_;
	return result;
}


void
valadoc_content_list_set_bullet (ValadocContentList* self,
                                 ValadocContentListBullet value)
{
	g_return_if_fail (self != NULL);
	if (valadoc_content_list_get_bullet (self) != value) {
		self->priv->_bullet = value;
		g_object_notify_by_pspec ((GObject *) self, valadoc_content_list_properties[VALADOC_CONTENT_LIST_BULLET_PROPERTY]);
	}
}


ValaList*
valadoc_content_list_get_items (ValadocContentList* self)
{
	ValaList* result;
	ValaList* _tmp0_;
	g_return_val_if_fail (self != NULL, NULL);
	_tmp0_ = self->priv->_items;
	result = _tmp0_;
	return result;
}


static void
valadoc_content_list_class_init (ValadocContentListClass * klass)
{
	valadoc_content_list_parent_class = g_type_class_peek_parent (klass);
	g_type_class_adjust_private_offset (klass, &ValadocContentList_private_offset);
	((ValadocContentContentElementClass *) klass)->check = (void (*) (ValadocContentContentElement *, ValadocApiTree*, ValadocApiNode*, const gchar*, ValadocErrorReporter*, ValadocSettings*)) valadoc_content_list_real_check;
	((ValadocContentContentElementClass *) klass)->accept = (void (*) (ValadocContentContentElement *, ValadocContentContentVisitor*)) valadoc_content_list_real_accept;
	((ValadocContentContentElementClass *) klass)->accept_children = (void (*) (ValadocContentContentElement *, ValadocContentContentVisitor*)) valadoc_content_list_real_accept_children;
	((ValadocContentContentElementClass *) klass)->is_empty = (gboolean (*) (ValadocContentContentElement *)) valadoc_content_list_real_is_empty;
	((ValadocContentContentElementClass *) klass)->copy = (ValadocContentContentElement* (*) (ValadocContentContentElement *, ValadocContentContentElement*)) valadoc_content_list_real_copy;
	G_OBJECT_CLASS (klass)->get_property = _vala_valadoc_content_list_get_property;
	G_OBJECT_CLASS (klass)->set_property = _vala_valadoc_content_list_set_property;
	G_OBJECT_CLASS (klass)->finalize = valadoc_content_list_finalize;
	g_object_class_install_property (G_OBJECT_CLASS (klass), VALADOC_CONTENT_LIST_BULLET_PROPERTY, valadoc_content_list_properties[VALADOC_CONTENT_LIST_BULLET_PROPERTY] = g_param_spec_enum ("bullet", "bullet", "bullet", VALADOC_CONTENT_LIST_TYPE_BULLET, 0, G_PARAM_STATIC_STRINGS | G_PARAM_READABLE | G_PARAM_WRITABLE));
	g_object_class_install_property (G_OBJECT_CLASS (klass), VALADOC_CONTENT_LIST_ITEMS_PROPERTY, valadoc_content_list_properties[VALADOC_CONTENT_LIST_ITEMS_PROPERTY] = vala_param_spec_iterable ("items", "items", "items", VALA_TYPE_LIST, G_PARAM_STATIC_STRINGS | G_PARAM_READABLE));
}


static void
valadoc_content_list_valadoc_content_block_interface_init (ValadocContentBlockIface * iface)
{
	valadoc_content_list_valadoc_content_block_parent_iface = g_type_interface_peek_parent (iface);
}


static void
valadoc_content_list_instance_init (ValadocContentList * self)
{
	self->priv = valadoc_content_list_get_instance_private (self);
}


static void
valadoc_content_list_finalize (GObject * obj)
{
	ValadocContentList * self;
	self = G_TYPE_CHECK_INSTANCE_CAST (obj, VALADOC_CONTENT_TYPE_LIST, ValadocContentList);
	_vala_iterable_unref0 (self->priv->_items);
	G_OBJECT_CLASS (valadoc_content_list_parent_class)->finalize (obj);
}


GType
valadoc_content_list_get_type (void)
{
	static volatile gsize valadoc_content_list_type_id__volatile = 0;
	if (g_once_init_enter (&valadoc_content_list_type_id__volatile)) {
		static const GTypeInfo g_define_type_info = { sizeof (ValadocContentListClass), (GBaseInitFunc) NULL, (GBaseFinalizeFunc) NULL, (GClassInitFunc) valadoc_content_list_class_init, (GClassFinalizeFunc) NULL, NULL, sizeof (ValadocContentList), 0, (GInstanceInitFunc) valadoc_content_list_instance_init, NULL };
		static const GInterfaceInfo valadoc_content_block_info = { (GInterfaceInitFunc) valadoc_content_list_valadoc_content_block_interface_init, (GInterfaceFinalizeFunc) NULL, NULL};
		GType valadoc_content_list_type_id;
		valadoc_content_list_type_id = g_type_register_static (VALADOC_CONTENT_TYPE_CONTENT_ELEMENT, "ValadocContentList", &g_define_type_info, 0);
		g_type_add_interface_static (valadoc_content_list_type_id, VALADOC_CONTENT_TYPE_BLOCK, &valadoc_content_block_info);
		ValadocContentList_private_offset = g_type_add_instance_private (valadoc_content_list_type_id, sizeof (ValadocContentListPrivate));
		g_once_init_leave (&valadoc_content_list_type_id__volatile, valadoc_content_list_type_id);
	}
	return valadoc_content_list_type_id__volatile;
}


static void
_vala_valadoc_content_list_get_property (GObject * object,
                                         guint property_id,
                                         GValue * value,
                                         GParamSpec * pspec)
{
	ValadocContentList * self;
	self = G_TYPE_CHECK_INSTANCE_CAST (object, VALADOC_CONTENT_TYPE_LIST, ValadocContentList);
	switch (property_id) {
		case VALADOC_CONTENT_LIST_BULLET_PROPERTY:
		g_value_set_enum (value, valadoc_content_list_get_bullet (self));
		break;
		case VALADOC_CONTENT_LIST_ITEMS_PROPERTY:
		vala_value_set_iterable (value, valadoc_content_list_get_items (self));
		break;
		default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
		break;
	}
}


static void
_vala_valadoc_content_list_set_property (GObject * object,
                                         guint property_id,
                                         const GValue * value,
                                         GParamSpec * pspec)
{
	ValadocContentList * self;
	self = G_TYPE_CHECK_INSTANCE_CAST (object, VALADOC_CONTENT_TYPE_LIST, ValadocContentList);
	switch (property_id) {
		case VALADOC_CONTENT_LIST_BULLET_PROPERTY:
		valadoc_content_list_set_bullet (self, g_value_get_enum (value));
		break;
		default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
		break;
	}
}


