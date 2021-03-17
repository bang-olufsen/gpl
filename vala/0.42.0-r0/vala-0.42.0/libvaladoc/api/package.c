/* package.c generated by valac, the Vala compiler
 * generated from package.vala, do not modify */

/* package.vala
 *
 * Copyright (C) 2008-2011  Florian Brosch
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
 * 	Florian Brosch <flo.brosch@gmail.com>
 */


#include <glib.h>
#include <glib-object.h>
#include "valadoc.h"
#include <valagee.h>
#include <stdlib.h>
#include <string.h>
#include <vala.h>

enum  {
	VALADOC_API_PACKAGE_0_PROPERTY,
	VALADOC_API_PACKAGE_IS_PACKAGE_PROPERTY,
	VALADOC_API_PACKAGE_NODE_TYPE_PROPERTY,
	VALADOC_API_PACKAGE_NUM_PROPERTIES
};
static GParamSpec* valadoc_api_package_properties[VALADOC_API_PACKAGE_NUM_PROPERTIES];
#define _vala_iterable_unref0(var) ((var == NULL) ? NULL : (var = (vala_iterable_unref (var), NULL)))
#define _vala_map_unref0(var) ((var == NULL) ? NULL : (var = (vala_map_unref (var), NULL)))
#define _g_object_unref0(var) ((var == NULL) ? NULL : (var = (g_object_unref (var), NULL)))
#define _vala_iterator_unref0(var) ((var == NULL) ? NULL : (var = (vala_iterator_unref (var), NULL)))
#define _valadoc_api_signature_builder_unref0(var) ((var == NULL) ? NULL : (var = (valadoc_api_signature_builder_unref (var), NULL)))

struct _ValadocApiPackagePrivate {
	gboolean _is_package;
	ValaArrayList* _dependencies;
	ValaHashMap* deprecated;
};


static gint ValadocApiPackage_private_offset;
static gpointer valadoc_api_package_parent_class = NULL;

G_GNUC_INTERNAL void valadoc_api_package_set_dependency_list (ValadocApiPackage* self,
                                              ValaArrayList* list);
static void valadoc_api_package_set_is_package (ValadocApiPackage* self,
                                         gboolean value);
G_GNUC_INTERNAL void valadoc_api_package_register_deprecated_symbol (ValadocApiPackage* self,
                                                     ValadocApiSymbol* symbol,
                                                     const gchar* version);
static gboolean valadoc_api_package_real_is_browsable (ValadocApiNode* base,
                                                ValadocSettings* settings);
static void valadoc_api_package_real_accept (ValadocApiNode* base,
                                      ValadocApiVisitor* visitor);
static ValadocContentInline* valadoc_api_package_real_build_signature (ValadocApiItem* base);
static void valadoc_api_package_finalize (GObject * obj);
static void _vala_valadoc_api_package_get_property (GObject * object,
                                             guint property_id,
                                             GValue * value,
                                             GParamSpec * pspec);
static void _vala_valadoc_api_package_set_property (GObject * object,
                                             guint property_id,
                                             const GValue * value,
                                             GParamSpec * pspec);


static inline gpointer
valadoc_api_package_get_instance_private (ValadocApiPackage* self)
{
	return G_STRUCT_MEMBER_P (self, ValadocApiPackage_private_offset);
}


static gpointer
_vala_iterable_ref0 (gpointer self)
{
	return self ? vala_iterable_ref (self) : NULL;
}


G_GNUC_INTERNAL void
valadoc_api_package_set_dependency_list (ValadocApiPackage* self,
                                         ValaArrayList* list)
{
	ValaArrayList* _tmp0_;
	g_return_if_fail (self != NULL);
	g_return_if_fail (list != NULL);
	_tmp0_ = _vala_iterable_ref0 (list);
	_vala_iterable_unref0 (self->priv->_dependencies);
	self->priv->_dependencies = _tmp0_;
}


/**
 * Returns a list with all dependencies
 */
ValaCollection*
valadoc_api_package_get_full_dependency_list (ValadocApiPackage* self)
{
	ValaCollection* result = NULL;
	ValaArrayList* list = NULL;
	GEqualFunc _tmp0_;
	ValaArrayList* _tmp1_;
	ValaArrayList* _tmp2_;
	g_return_val_if_fail (self != NULL, NULL);
	_tmp0_ = g_direct_equal;
	_tmp1_ = vala_array_list_new (VALADOC_API_TYPE_PACKAGE, (GBoxedCopyFunc) g_object_ref, (GDestroyNotify) g_object_unref, _tmp0_);
	list = _tmp1_;
	_tmp2_ = self->priv->_dependencies;
	if (_tmp2_ == NULL) {
		result = (ValaCollection*) list;
		return result;
	}
	{
		ValaArrayList* _pkg_list = NULL;
		ValaArrayList* _tmp3_;
		ValaArrayList* _tmp4_;
		gint _pkg_size = 0;
		ValaArrayList* _tmp5_;
		gint _tmp6_;
		gint _tmp7_;
		gint _pkg_index = 0;
		_tmp3_ = self->priv->_dependencies;
		_tmp4_ = _vala_iterable_ref0 (_tmp3_);
		_pkg_list = _tmp4_;
		_tmp5_ = _pkg_list;
		_tmp6_ = vala_collection_get_size ((ValaCollection*) _tmp5_);
		_tmp7_ = _tmp6_;
		_pkg_size = _tmp7_;
		_pkg_index = -1;
		while (TRUE) {
			gint _tmp8_;
			gint _tmp9_;
			gint _tmp10_;
			ValadocApiPackage* pkg = NULL;
			ValaArrayList* _tmp11_;
			gint _tmp12_;
			gpointer _tmp13_;
			ValaArrayList* _tmp14_;
			ValadocApiPackage* _tmp15_;
			ValaCollection* pkg_list = NULL;
			ValadocApiPackage* _tmp18_;
			ValaCollection* _tmp19_;
			_tmp8_ = _pkg_index;
			_pkg_index = _tmp8_ + 1;
			_tmp9_ = _pkg_index;
			_tmp10_ = _pkg_size;
			if (!(_tmp9_ < _tmp10_)) {
				break;
			}
			_tmp11_ = _pkg_list;
			_tmp12_ = _pkg_index;
			_tmp13_ = vala_list_get ((ValaList*) _tmp11_, _tmp12_);
			pkg = (ValadocApiPackage*) _tmp13_;
			_tmp14_ = list;
			_tmp15_ = pkg;
			if (vala_collection_contains ((ValaCollection*) _tmp14_, _tmp15_) == FALSE) {
				ValaArrayList* _tmp16_;
				ValadocApiPackage* _tmp17_;
				_tmp16_ = list;
				_tmp17_ = pkg;
				vala_collection_add ((ValaCollection*) _tmp16_, _tmp17_);
			}
			_tmp18_ = pkg;
			_tmp19_ = valadoc_api_package_get_full_dependency_list (_tmp18_);
			pkg_list = _tmp19_;
			{
				ValaIterator* _pkg2_it = NULL;
				ValaCollection* _tmp20_;
				ValaIterator* _tmp21_;
				_tmp20_ = pkg_list;
				_tmp21_ = vala_iterable_iterator ((ValaIterable*) _tmp20_);
				_pkg2_it = _tmp21_;
				while (TRUE) {
					ValaIterator* _tmp22_;
					ValadocApiPackage* pkg2 = NULL;
					ValaIterator* _tmp23_;
					gpointer _tmp24_;
					ValaArrayList* _tmp25_;
					ValadocApiPackage* _tmp26_;
					_tmp22_ = _pkg2_it;
					if (!vala_iterator_next (_tmp22_)) {
						break;
					}
					_tmp23_ = _pkg2_it;
					_tmp24_ = vala_iterator_get (_tmp23_);
					pkg2 = (ValadocApiPackage*) _tmp24_;
					_tmp25_ = list;
					_tmp26_ = pkg2;
					if (vala_collection_contains ((ValaCollection*) _tmp25_, _tmp26_) == FALSE) {
						ValaArrayList* _tmp27_;
						ValadocApiPackage* _tmp28_;
						_tmp27_ = list;
						_tmp28_ = pkg2;
						vala_collection_add ((ValaCollection*) _tmp27_, _tmp28_);
					}
					_g_object_unref0 (pkg2);
				}
				_vala_iterator_unref0 (_pkg2_it);
			}
			_vala_iterable_unref0 (pkg_list);
			_g_object_unref0 (pkg);
		}
		_vala_iterable_unref0 (_pkg_list);
	}
	result = (ValaCollection*) list;
	return result;
}


ValaCollection*
valadoc_api_package_get_dependency_list (ValadocApiPackage* self)
{
	ValaCollection* result = NULL;
	ValaArrayList* _tmp0_;
	ValaArrayList* _tmp3_;
	ValaCollection* _tmp4_;
	g_return_val_if_fail (self != NULL, NULL);
	_tmp0_ = self->priv->_dependencies;
	if (_tmp0_ == NULL) {
		GEqualFunc _tmp1_;
		ValaArrayList* _tmp2_;
		_tmp1_ = g_direct_equal;
		_tmp2_ = vala_array_list_new (VALADOC_API_TYPE_PACKAGE, (GBoxedCopyFunc) g_object_ref, (GDestroyNotify) g_object_unref, _tmp1_);
		result = (ValaCollection*) _tmp2_;
		return result;
	}
	_tmp3_ = self->priv->_dependencies;
	_tmp4_ = _vala_iterable_ref0 ((ValaCollection*) _tmp3_);
	result = _tmp4_;
	return result;
}


ValadocApiPackage*
valadoc_api_package_construct (GType object_type,
                               const gchar* name,
                               gboolean is_package,
                               ValaCodeNode* data)
{
	ValadocApiPackage * self = NULL;
	g_return_val_if_fail (name != NULL, NULL);
	self = (ValadocApiPackage*) valadoc_api_node_construct (object_type, NULL, NULL, name, data);
	valadoc_api_package_set_is_package (self, is_package);
	valadoc_api_item_set_parent ((ValadocApiItem*) self, NULL);
	return self;
}


ValadocApiPackage*
valadoc_api_package_new (const gchar* name,
                         gboolean is_package,
                         ValaCodeNode* data)
{
	return valadoc_api_package_construct (VALADOC_API_TYPE_PACKAGE, name, is_package, data);
}


G_GNUC_INTERNAL void
valadoc_api_package_register_deprecated_symbol (ValadocApiPackage* self,
                                                ValadocApiSymbol* symbol,
                                                const gchar* version)
{
	ValaHashMap* _tmp0_;
	ValaArrayList* list = NULL;
	ValaHashMap* _tmp5_;
	gpointer _tmp6_;
	ValaArrayList* _tmp7_;
	ValaArrayList* _tmp12_;
	g_return_if_fail (self != NULL);
	g_return_if_fail (symbol != NULL);
	_tmp0_ = self->priv->deprecated;
	if (_tmp0_ == NULL) {
		GHashFunc _tmp1_;
		GEqualFunc _tmp2_;
		GEqualFunc _tmp3_;
		ValaHashMap* _tmp4_;
		_tmp1_ = g_str_hash;
		_tmp2_ = g_str_equal;
		_tmp3_ = g_direct_equal;
		_tmp4_ = vala_hash_map_new (G_TYPE_STRING, (GBoxedCopyFunc) g_strdup, (GDestroyNotify) g_free, VALA_TYPE_ARRAY_LIST, (GBoxedCopyFunc) vala_iterable_ref, (GDestroyNotify) vala_iterable_unref, _tmp1_, _tmp2_, _tmp3_);
		_vala_map_unref0 (self->priv->deprecated);
		self->priv->deprecated = _tmp4_;
	}
	if (version == NULL) {
		version = "0xdeadbeef";
	}
	_tmp5_ = self->priv->deprecated;
	_tmp6_ = vala_map_get ((ValaMap*) _tmp5_, version);
	list = (ValaArrayList*) _tmp6_;
	_tmp7_ = list;
	if (_tmp7_ == NULL) {
		GEqualFunc _tmp8_;
		ValaArrayList* _tmp9_;
		ValaHashMap* _tmp10_;
		ValaArrayList* _tmp11_;
		_tmp8_ = g_direct_equal;
		_tmp9_ = vala_array_list_new (VALADOC_API_TYPE_SYMBOL, (GBoxedCopyFunc) g_object_ref, (GDestroyNotify) g_object_unref, _tmp8_);
		_vala_iterable_unref0 (list);
		list = _tmp9_;
		_tmp10_ = self->priv->deprecated;
		_tmp11_ = list;
		vala_map_set ((ValaMap*) _tmp10_, version, _tmp11_);
	}
	_tmp12_ = list;
	vala_collection_add ((ValaCollection*) _tmp12_, symbol);
	_vala_iterable_unref0 (list);
}


static gpointer
_vala_map_ref0 (gpointer self)
{
	return self ? vala_map_ref (self) : NULL;
}


ValaMap*
valadoc_api_package_get_deprecated_symbols (ValadocApiPackage* self)
{
	ValaMap* result = NULL;
	ValaHashMap* _tmp0_;
	ValaHashMap* _tmp5_;
	ValaMap* _tmp6_;
	g_return_val_if_fail (self != NULL, NULL);
	_tmp0_ = self->priv->deprecated;
	if (_tmp0_ == NULL) {
		GHashFunc _tmp1_;
		GEqualFunc _tmp2_;
		GEqualFunc _tmp3_;
		ValaHashMap* _tmp4_;
		_tmp1_ = g_str_hash;
		_tmp2_ = g_str_equal;
		_tmp3_ = g_direct_equal;
		_tmp4_ = vala_hash_map_new (G_TYPE_STRING, (GBoxedCopyFunc) g_strdup, (GDestroyNotify) g_free, VALA_TYPE_COLLECTION, (GBoxedCopyFunc) vala_iterable_ref, (GDestroyNotify) vala_iterable_unref, _tmp1_, _tmp2_, _tmp3_);
		result = (ValaMap*) _tmp4_;
		return result;
	}
	_tmp5_ = self->priv->deprecated;
	_tmp6_ = _vala_map_ref0 ((ValaMap*) _tmp5_);
	result = _tmp6_;
	return result;
}


/**
 * {@inheritDoc}
 */
static gboolean
valadoc_api_package_real_is_browsable (ValadocApiNode* base,
                                       ValadocSettings* settings)
{
	ValadocApiPackage * self;
	gboolean result = FALSE;
	gboolean _tmp0_ = FALSE;
	gboolean _tmp1_;
	self = (ValadocApiPackage*) base;
	g_return_val_if_fail (settings != NULL, FALSE);
	_tmp1_ = self->priv->_is_package;
	if (_tmp1_) {
		gboolean _tmp2_;
		_tmp2_ = settings->with_deps;
		_tmp0_ = _tmp2_ == FALSE;
	} else {
		_tmp0_ = FALSE;
	}
	result = !_tmp0_;
	return result;
}


/**
 * {@inheritDoc}
 */
static void
valadoc_api_package_real_accept (ValadocApiNode* base,
                                 ValadocApiVisitor* visitor)
{
	ValadocApiPackage * self;
	self = (ValadocApiPackage*) base;
	g_return_if_fail (visitor != NULL);
	valadoc_api_visitor_visit_package (visitor, self);
}


/**
 * {@inheritDoc}
 */
static ValadocContentInline*
valadoc_api_package_real_build_signature (ValadocApiItem* base)
{
	ValadocApiPackage * self;
	ValadocContentInline* result = NULL;
	ValadocApiSignatureBuilder* _tmp0_;
	ValadocApiSignatureBuilder* _tmp1_;
	ValadocApiSignatureBuilder* _tmp2_;
	const gchar* _tmp3_;
	const gchar* _tmp4_;
	ValadocApiSignatureBuilder* _tmp5_;
	ValadocContentRun* _tmp6_;
	ValadocContentInline* _tmp7_;
	self = (ValadocApiPackage*) base;
	_tmp0_ = valadoc_api_signature_builder_new ();
	_tmp1_ = _tmp0_;
	_tmp2_ = valadoc_api_signature_builder_append_keyword (_tmp1_, "package", TRUE);
	_tmp3_ = valadoc_api_node_get_name ((ValadocApiNode*) self);
	_tmp4_ = _tmp3_;
	_tmp5_ = valadoc_api_signature_builder_append (_tmp2_, _tmp4_, TRUE);
	_tmp6_ = valadoc_api_signature_builder_get (_tmp5_);
	_tmp7_ = (ValadocContentInline*) _tmp6_;
	_valadoc_api_signature_builder_unref0 (_tmp1_);
	result = _tmp7_;
	return result;
}


gboolean
valadoc_api_package_get_is_package (ValadocApiPackage* self)
{
	gboolean result;
	gboolean _tmp0_;
	g_return_val_if_fail (self != NULL, FALSE);
	_tmp0_ = self->priv->_is_package;
	result = _tmp0_;
	return result;
}


static void
valadoc_api_package_set_is_package (ValadocApiPackage* self,
                                    gboolean value)
{
	g_return_if_fail (self != NULL);
	if (valadoc_api_package_get_is_package (self) != value) {
		self->priv->_is_package = value;
		g_object_notify_by_pspec ((GObject *) self, valadoc_api_package_properties[VALADOC_API_PACKAGE_IS_PACKAGE_PROPERTY]);
	}
}


static ValadocApiNodeType
valadoc_api_package_real_get_node_type (ValadocApiNode* base)
{
	ValadocApiNodeType result;
	ValadocApiPackage* self;
	self = (ValadocApiPackage*) base;
	result = VALADOC_API_NODE_TYPE_PACKAGE;
	return result;
}


static void
valadoc_api_package_class_init (ValadocApiPackageClass * klass)
{
	valadoc_api_package_parent_class = g_type_class_peek_parent (klass);
	g_type_class_adjust_private_offset (klass, &ValadocApiPackage_private_offset);
	((ValadocApiNodeClass *) klass)->is_browsable = (gboolean (*) (ValadocApiNode *, ValadocSettings*)) valadoc_api_package_real_is_browsable;
	((ValadocApiNodeClass *) klass)->accept = (void (*) (ValadocApiNode *, ValadocApiVisitor*)) valadoc_api_package_real_accept;
	((ValadocApiItemClass *) klass)->build_signature = (ValadocContentInline* (*) (ValadocApiItem *)) valadoc_api_package_real_build_signature;
	VALADOC_API_NODE_CLASS (klass)->get_node_type = valadoc_api_package_real_get_node_type;
	G_OBJECT_CLASS (klass)->get_property = _vala_valadoc_api_package_get_property;
	G_OBJECT_CLASS (klass)->set_property = _vala_valadoc_api_package_set_property;
	G_OBJECT_CLASS (klass)->finalize = valadoc_api_package_finalize;
	/**
	 * Specifies whether this package is a dependency
	 */
	g_object_class_install_property (G_OBJECT_CLASS (klass), VALADOC_API_PACKAGE_IS_PACKAGE_PROPERTY, valadoc_api_package_properties[VALADOC_API_PACKAGE_IS_PACKAGE_PROPERTY] = g_param_spec_boolean ("is-package", "is-package", "is-package", FALSE, G_PARAM_STATIC_STRINGS | G_PARAM_READABLE));
	/**
	 * {@inheritDoc}
	 */
	g_object_class_install_property (G_OBJECT_CLASS (klass), VALADOC_API_PACKAGE_NODE_TYPE_PROPERTY, valadoc_api_package_properties[VALADOC_API_PACKAGE_NODE_TYPE_PROPERTY] = g_param_spec_enum ("node-type", "node-type", "node-type", VALADOC_API_TYPE_NODE_TYPE, 0, G_PARAM_STATIC_STRINGS | G_PARAM_READABLE));
}


static void
valadoc_api_package_instance_init (ValadocApiPackage * self)
{
	self->priv = valadoc_api_package_get_instance_private (self);
}


static void
valadoc_api_package_finalize (GObject * obj)
{
	ValadocApiPackage * self;
	self = G_TYPE_CHECK_INSTANCE_CAST (obj, VALADOC_API_TYPE_PACKAGE, ValadocApiPackage);
	_vala_iterable_unref0 (self->priv->_dependencies);
	_vala_map_unref0 (self->priv->deprecated);
	G_OBJECT_CLASS (valadoc_api_package_parent_class)->finalize (obj);
}


GType
valadoc_api_package_get_type (void)
{
	static volatile gsize valadoc_api_package_type_id__volatile = 0;
	if (g_once_init_enter (&valadoc_api_package_type_id__volatile)) {
		static const GTypeInfo g_define_type_info = { sizeof (ValadocApiPackageClass), (GBaseInitFunc) NULL, (GBaseFinalizeFunc) NULL, (GClassInitFunc) valadoc_api_package_class_init, (GClassFinalizeFunc) NULL, NULL, sizeof (ValadocApiPackage), 0, (GInstanceInitFunc) valadoc_api_package_instance_init, NULL };
		GType valadoc_api_package_type_id;
		valadoc_api_package_type_id = g_type_register_static (VALADOC_API_TYPE_NODE, "ValadocApiPackage", &g_define_type_info, 0);
		ValadocApiPackage_private_offset = g_type_add_instance_private (valadoc_api_package_type_id, sizeof (ValadocApiPackagePrivate));
		g_once_init_leave (&valadoc_api_package_type_id__volatile, valadoc_api_package_type_id);
	}
	return valadoc_api_package_type_id__volatile;
}


static void
_vala_valadoc_api_package_get_property (GObject * object,
                                        guint property_id,
                                        GValue * value,
                                        GParamSpec * pspec)
{
	ValadocApiPackage * self;
	self = G_TYPE_CHECK_INSTANCE_CAST (object, VALADOC_API_TYPE_PACKAGE, ValadocApiPackage);
	switch (property_id) {
		case VALADOC_API_PACKAGE_IS_PACKAGE_PROPERTY:
		g_value_set_boolean (value, valadoc_api_package_get_is_package (self));
		break;
		case VALADOC_API_PACKAGE_NODE_TYPE_PROPERTY:
		g_value_set_enum (value, valadoc_api_node_get_node_type ((ValadocApiNode*) self));
		break;
		default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
		break;
	}
}


static void
_vala_valadoc_api_package_set_property (GObject * object,
                                        guint property_id,
                                        const GValue * value,
                                        GParamSpec * pspec)
{
	ValadocApiPackage * self;
	self = G_TYPE_CHECK_INSTANCE_CAST (object, VALADOC_API_TYPE_PACKAGE, ValadocApiPackage);
	switch (property_id) {
		case VALADOC_API_PACKAGE_IS_PACKAGE_PROPERTY:
		valadoc_api_package_set_is_package (self, g_value_get_boolean (value));
		break;
		default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
		break;
	}
}


