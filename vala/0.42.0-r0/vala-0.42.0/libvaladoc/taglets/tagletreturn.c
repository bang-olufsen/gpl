/* tagletreturn.c generated by valac, the Vala compiler
 * generated from tagletreturn.vala, do not modify */

/* taglet.vala
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
#include <stdlib.h>
#include <string.h>
#include <valagee.h>

enum  {
	VALADOC_TAGLETS_RETURN_0_PROPERTY,
	VALADOC_TAGLETS_RETURN_NUM_PROPERTIES
};
static GParamSpec* valadoc_taglets_return_properties[VALADOC_TAGLETS_RETURN_NUM_PROPERTIES];
#define _g_object_unref0(var) ((var == NULL) ? NULL : (var = (g_object_unref (var), NULL)))
#define _g_free0(var) (var = (g_free (var), NULL))
#define _vala_iterable_unref0(var) ((var == NULL) ? NULL : (var = (vala_iterable_unref (var), NULL)))


static gpointer valadoc_taglets_return_parent_class = NULL;
static ValadocContentTagletIface * valadoc_taglets_return_valadoc_content_taglet_parent_iface = NULL;
static ValadocContentBlockIface * valadoc_taglets_return_valadoc_content_block_parent_iface = NULL;

static ValadocRule* valadoc_taglets_return_real_get_parser_rule (ValadocContentTaglet* base,
                                                          ValadocRule* run_rule);
static void valadoc_taglets_return_real_check (ValadocContentContentElement* base,
                                        ValadocApiTree* api_root,
                                        ValadocApiNode* container,
                                        const gchar* file_path,
                                        ValadocErrorReporter* reporter,
                                        ValadocSettings* settings);
static void valadoc_taglets_return_real_accept (ValadocContentContentElement* base,
                                         ValadocContentContentVisitor* visitor);
static ValaList* valadoc_taglets_return_real_get_inheritable_documentation (ValadocContentTaglet* base);
static gboolean valadoc_taglets_return_real_inheritable (ValadocContentTaglet* base,
                                                  ValadocContentTaglet* taglet);
static ValadocContentContentElement* valadoc_taglets_return_real_copy (ValadocContentContentElement* base,
                                                                ValadocContentContentElement* new_parent);
G_GNUC_INTERNAL void valadoc_content_content_element_set_parent (ValadocContentContentElement* self,
                                                 ValadocContentContentElement* value);
G_GNUC_INTERNAL ValadocContentBlockContent* valadoc_content_block_content_construct (GType object_type);


static gpointer
_g_object_ref0 (gpointer self)
{
	return self ? g_object_ref (self) : NULL;
}


static ValadocRule*
valadoc_taglets_return_real_get_parser_rule (ValadocContentTaglet* base,
                                             ValadocRule* run_rule)
{
	ValadocTagletsReturn * self;
	ValadocRule* result = NULL;
	ValadocRule* _tmp0_;
	self = (ValadocTagletsReturn*) base;
	g_return_val_if_fail (run_rule != NULL, NULL);
	_tmp0_ = _g_object_ref0 (run_rule);
	result = _tmp0_;
	return result;
}


static void
valadoc_taglets_return_real_check (ValadocContentContentElement* base,
                                   ValadocApiTree* api_root,
                                   ValadocApiNode* container,
                                   const gchar* file_path,
                                   ValadocErrorReporter* reporter,
                                   ValadocSettings* settings)
{
	ValadocTagletsReturn * self;
	ValadocApiTypeReference* type_ref = NULL;
	gboolean creation_method = FALSE;
	gboolean _tmp12_ = FALSE;
	gboolean _tmp13_ = FALSE;
	ValadocApiTypeReference* _tmp14_;
	self = (ValadocTagletsReturn*) base;
	g_return_if_fail (api_root != NULL);
	g_return_if_fail (container != NULL);
	g_return_if_fail (file_path != NULL);
	g_return_if_fail (reporter != NULL);
	g_return_if_fail (settings != NULL);
	type_ref = NULL;
	creation_method = FALSE;
	if (G_TYPE_CHECK_INSTANCE_TYPE (container, VALADOC_API_TYPE_METHOD)) {
		gboolean _tmp0_;
		gboolean _tmp1_;
		ValadocApiTypeReference* _tmp2_;
		ValadocApiTypeReference* _tmp3_;
		ValadocApiTypeReference* _tmp4_;
		_tmp0_ = valadoc_api_method_get_is_constructor (G_TYPE_CHECK_INSTANCE_CAST (container, VALADOC_API_TYPE_METHOD, ValadocApiMethod));
		_tmp1_ = _tmp0_;
		creation_method = _tmp1_;
		_tmp2_ = valadoc_api_callable_get_return_type ((ValadocApiCallable*) G_TYPE_CHECK_INSTANCE_CAST (container, VALADOC_API_TYPE_METHOD, ValadocApiMethod));
		_tmp3_ = _tmp2_;
		_tmp4_ = _g_object_ref0 (_tmp3_);
		_g_object_unref0 (type_ref);
		type_ref = _tmp4_;
	} else {
		if (G_TYPE_CHECK_INSTANCE_TYPE (container, VALADOC_API_TYPE_CALLABLE)) {
			ValadocApiTypeReference* _tmp5_;
			ValadocApiTypeReference* _tmp6_;
			ValadocApiTypeReference* _tmp7_;
			_tmp5_ = valadoc_api_callable_get_return_type (G_TYPE_CHECK_INSTANCE_CAST (container, VALADOC_API_TYPE_CALLABLE, ValadocApiCallable));
			_tmp6_ = _tmp5_;
			_tmp7_ = _g_object_ref0 (_tmp6_);
			_g_object_unref0 (type_ref);
			type_ref = _tmp7_;
		} else {
			gchar* _tmp8_;
			gchar* _tmp9_;
			gchar* _tmp10_;
			gchar* _tmp11_;
			_tmp8_ = valadoc_api_node_get_full_name (container);
			_tmp9_ = _tmp8_;
			_tmp10_ = g_strdup_printf ("%s: %s: @return", file_path, _tmp9_);
			_tmp11_ = _tmp10_;
			valadoc_error_reporter_simple_warning (reporter, _tmp11_, "@return used outside method/delegate/signal context");
			_g_free0 (_tmp11_);
			_g_free0 (_tmp9_);
		}
	}
	_tmp14_ = type_ref;
	if (_tmp14_ != NULL) {
		ValadocApiTypeReference* _tmp15_;
		ValadocApiItem* _tmp16_;
		ValadocApiItem* _tmp17_;
		_tmp15_ = type_ref;
		_tmp16_ = valadoc_api_typereference_get_data_type (_tmp15_);
		_tmp17_ = _tmp16_;
		_tmp13_ = _tmp17_ == NULL;
	} else {
		_tmp13_ = FALSE;
	}
	if (_tmp13_) {
		gboolean _tmp18_;
		_tmp18_ = creation_method;
		_tmp12_ = !_tmp18_;
	} else {
		_tmp12_ = FALSE;
	}
	if (_tmp12_) {
		gchar* _tmp19_;
		gchar* _tmp20_;
		gchar* _tmp21_;
		gchar* _tmp22_;
		_tmp19_ = valadoc_api_node_get_full_name (container);
		_tmp20_ = _tmp19_;
		_tmp21_ = g_strdup_printf ("%s: %s: @return", file_path, _tmp20_);
		_tmp22_ = _tmp21_;
		valadoc_error_reporter_simple_warning (reporter, _tmp22_, "Return description declared for void function");
		_g_free0 (_tmp22_);
		_g_free0 (_tmp20_);
	}
	VALADOC_CONTENT_CONTENT_ELEMENT_CLASS (valadoc_taglets_return_parent_class)->check ((ValadocContentContentElement*) G_TYPE_CHECK_INSTANCE_CAST (self, VALADOC_CONTENT_TYPE_BLOCK_CONTENT, ValadocContentBlockContent), api_root, container, file_path, reporter, settings);
	_g_object_unref0 (type_ref);
}


static void
valadoc_taglets_return_real_accept (ValadocContentContentElement* base,
                                    ValadocContentContentVisitor* visitor)
{
	ValadocTagletsReturn * self;
	self = (ValadocTagletsReturn*) base;
	g_return_if_fail (visitor != NULL);
	valadoc_content_content_visitor_visit_taglet (visitor, (ValadocContentTaglet*) self);
}


static gpointer
_vala_iterable_ref0 (gpointer self)
{
	return self ? vala_iterable_ref (self) : NULL;
}


static ValaList*
valadoc_taglets_return_real_get_inheritable_documentation (ValadocContentTaglet* base)
{
	ValadocTagletsReturn * self;
	ValaList* result = NULL;
	ValaList* _tmp0_;
	ValaList* _tmp1_;
	ValaList* _tmp2_;
	self = (ValadocTagletsReturn*) base;
	_tmp0_ = valadoc_content_block_content_get_content ((ValadocContentBlockContent*) self);
	_tmp1_ = _tmp0_;
	_tmp2_ = _vala_iterable_ref0 (_tmp1_);
	result = _tmp2_;
	return result;
}


static gboolean
valadoc_taglets_return_real_inheritable (ValadocContentTaglet* base,
                                         ValadocContentTaglet* taglet)
{
	ValadocTagletsReturn * self;
	gboolean result = FALSE;
	self = (ValadocTagletsReturn*) base;
	g_return_val_if_fail (taglet != NULL, FALSE);
	result = G_TYPE_CHECK_INSTANCE_TYPE (taglet, VALADOC_TAGLETS_TYPE_RETURN);
	return result;
}


static ValadocContentContentElement*
valadoc_taglets_return_real_copy (ValadocContentContentElement* base,
                                  ValadocContentContentElement* new_parent)
{
	ValadocTagletsReturn * self;
	ValadocContentContentElement* result = NULL;
	ValadocTagletsReturn* ret = NULL;
	ValadocTagletsReturn* _tmp0_;
	ValadocTagletsReturn* _tmp1_;
	self = (ValadocTagletsReturn*) base;
	_tmp0_ = valadoc_taglets_return_new ();
	ret = _tmp0_;
	_tmp1_ = ret;
	valadoc_content_content_element_set_parent ((ValadocContentContentElement*) _tmp1_, new_parent);
	{
		ValaList* _element_list = NULL;
		ValaList* _tmp2_;
		ValaList* _tmp3_;
		ValaList* _tmp4_;
		gint _element_size = 0;
		ValaList* _tmp5_;
		gint _tmp6_;
		gint _tmp7_;
		gint _element_index = 0;
		_tmp2_ = valadoc_content_block_content_get_content ((ValadocContentBlockContent*) self);
		_tmp3_ = _tmp2_;
		_tmp4_ = _vala_iterable_ref0 (_tmp3_);
		_element_list = _tmp4_;
		_tmp5_ = _element_list;
		_tmp6_ = vala_collection_get_size ((ValaCollection*) _tmp5_);
		_tmp7_ = _tmp6_;
		_element_size = _tmp7_;
		_element_index = -1;
		while (TRUE) {
			gint _tmp8_;
			gint _tmp9_;
			gint _tmp10_;
			ValadocContentBlock* element = NULL;
			ValaList* _tmp11_;
			gint _tmp12_;
			gpointer _tmp13_;
			ValadocContentBlock* copy = NULL;
			ValadocContentBlock* _tmp14_;
			ValadocTagletsReturn* _tmp15_;
			ValadocContentContentElement* _tmp16_;
			ValadocContentBlock* _tmp17_;
			ValadocTagletsReturn* _tmp18_;
			ValaList* _tmp19_;
			ValaList* _tmp20_;
			ValadocContentBlock* _tmp21_;
			_tmp8_ = _element_index;
			_element_index = _tmp8_ + 1;
			_tmp9_ = _element_index;
			_tmp10_ = _element_size;
			if (!(_tmp9_ < _tmp10_)) {
				break;
			}
			_tmp11_ = _element_list;
			_tmp12_ = _element_index;
			_tmp13_ = vala_list_get (_tmp11_, _tmp12_);
			element = (ValadocContentBlock*) _tmp13_;
			_tmp14_ = element;
			_tmp15_ = ret;
			_tmp16_ = valadoc_content_content_element_copy ((ValadocContentContentElement*) _tmp14_, (ValadocContentContentElement*) _tmp15_);
			_tmp17_ = G_TYPE_CHECK_INSTANCE_TYPE (_tmp16_, VALADOC_CONTENT_TYPE_BLOCK) ? ((ValadocContentBlock*) _tmp16_) : NULL;
			if (_tmp17_ == NULL) {
				_g_object_unref0 (_tmp16_);
			}
			copy = _tmp17_;
			_tmp18_ = ret;
			_tmp19_ = valadoc_content_block_content_get_content ((ValadocContentBlockContent*) _tmp18_);
			_tmp20_ = _tmp19_;
			_tmp21_ = copy;
			vala_collection_add ((ValaCollection*) _tmp20_, _tmp21_);
			_g_object_unref0 (copy);
			_g_object_unref0 (element);
		}
		_vala_iterable_unref0 (_element_list);
	}
	result = (ValadocContentContentElement*) ret;
	return result;
}


ValadocTagletsReturn*
valadoc_taglets_return_construct (GType object_type)
{
	ValadocTagletsReturn * self = NULL;
	self = (ValadocTagletsReturn*) valadoc_content_block_content_construct (object_type);
	return self;
}


ValadocTagletsReturn*
valadoc_taglets_return_new (void)
{
	return valadoc_taglets_return_construct (VALADOC_TAGLETS_TYPE_RETURN);
}


static void
valadoc_taglets_return_class_init (ValadocTagletsReturnClass * klass)
{
	valadoc_taglets_return_parent_class = g_type_class_peek_parent (klass);
	((ValadocContentContentElementClass *) klass)->check = (void (*) (ValadocContentContentElement *, ValadocApiTree*, ValadocApiNode*, const gchar*, ValadocErrorReporter*, ValadocSettings*)) valadoc_taglets_return_real_check;
	((ValadocContentContentElementClass *) klass)->accept = (void (*) (ValadocContentContentElement *, ValadocContentContentVisitor*)) valadoc_taglets_return_real_accept;
	((ValadocContentContentElementClass *) klass)->copy = (ValadocContentContentElement* (*) (ValadocContentContentElement *, ValadocContentContentElement*)) valadoc_taglets_return_real_copy;
}


static void
valadoc_taglets_return_valadoc_content_taglet_interface_init (ValadocContentTagletIface * iface)
{
	valadoc_taglets_return_valadoc_content_taglet_parent_iface = g_type_interface_peek_parent (iface);
	iface->get_parser_rule = (ValadocRule* (*) (ValadocContentTaglet *, ValadocRule*)) valadoc_taglets_return_real_get_parser_rule;
	iface->get_inheritable_documentation = (ValaList* (*) (ValadocContentTaglet *)) valadoc_taglets_return_real_get_inheritable_documentation;
	iface->inheritable = (gboolean (*) (ValadocContentTaglet *, ValadocContentTaglet*)) valadoc_taglets_return_real_inheritable;
}


static void
valadoc_taglets_return_valadoc_content_block_interface_init (ValadocContentBlockIface * iface)
{
	valadoc_taglets_return_valadoc_content_block_parent_iface = g_type_interface_peek_parent (iface);
}


static void
valadoc_taglets_return_instance_init (ValadocTagletsReturn * self)
{
}


GType
valadoc_taglets_return_get_type (void)
{
	static volatile gsize valadoc_taglets_return_type_id__volatile = 0;
	if (g_once_init_enter (&valadoc_taglets_return_type_id__volatile)) {
		static const GTypeInfo g_define_type_info = { sizeof (ValadocTagletsReturnClass), (GBaseInitFunc) NULL, (GBaseFinalizeFunc) NULL, (GClassInitFunc) valadoc_taglets_return_class_init, (GClassFinalizeFunc) NULL, NULL, sizeof (ValadocTagletsReturn), 0, (GInstanceInitFunc) valadoc_taglets_return_instance_init, NULL };
		static const GInterfaceInfo valadoc_content_taglet_info = { (GInterfaceInitFunc) valadoc_taglets_return_valadoc_content_taglet_interface_init, (GInterfaceFinalizeFunc) NULL, NULL};
		static const GInterfaceInfo valadoc_content_block_info = { (GInterfaceInitFunc) valadoc_taglets_return_valadoc_content_block_interface_init, (GInterfaceFinalizeFunc) NULL, NULL};
		GType valadoc_taglets_return_type_id;
		valadoc_taglets_return_type_id = g_type_register_static (VALADOC_CONTENT_TYPE_BLOCK_CONTENT, "ValadocTagletsReturn", &g_define_type_info, 0);
		g_type_add_interface_static (valadoc_taglets_return_type_id, VALADOC_CONTENT_TYPE_TAGLET, &valadoc_content_taglet_info);
		g_type_add_interface_static (valadoc_taglets_return_type_id, VALADOC_CONTENT_TYPE_BLOCK, &valadoc_content_block_info);
		g_once_init_leave (&valadoc_taglets_return_type_id__volatile, valadoc_taglets_return_type_id);
	}
	return valadoc_taglets_return_type_id__volatile;
}


