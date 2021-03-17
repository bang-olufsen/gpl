/* valaccodeinitializerlist.c generated by valac, the Vala compiler
 * generated from valaccodeinitializerlist.vala, do not modify */

/* valaccodeinitializerlist.vala
 *
 * Copyright (C) 2006-2007  Jürg Billeter
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
 * 	Jürg Billeter <j@bitron.ch>
 */


#include <glib.h>
#include <glib-object.h>
#include "valaccode.h"
#include <valagee.h>

#define _vala_iterable_unref0(var) ((var == NULL) ? NULL : (var = (vala_iterable_unref (var), NULL)))
#define _vala_ccode_node_unref0(var) ((var == NULL) ? NULL : (var = (vala_ccode_node_unref (var), NULL)))

struct _ValaCCodeInitializerListPrivate {
	ValaList* initializers;
};


static gint ValaCCodeInitializerList_private_offset;
static gpointer vala_ccode_initializer_list_parent_class = NULL;

static void vala_ccode_initializer_list_real_write (ValaCCodeNode* base,
                                             ValaCCodeWriter* writer);
static void vala_ccode_initializer_list_finalize (ValaCCodeNode * obj);


static inline gpointer
vala_ccode_initializer_list_get_instance_private (ValaCCodeInitializerList* self)
{
	return G_STRUCT_MEMBER_P (self, ValaCCodeInitializerList_private_offset);
}


/**
 * Appends the specified expression to this initializer list.
 *
 * @param expr an expression
 */
void
vala_ccode_initializer_list_append (ValaCCodeInitializerList* self,
                                    ValaCCodeExpression* expr)
{
	ValaList* _tmp0_;
	g_return_if_fail (self != NULL);
	g_return_if_fail (expr != NULL);
	_tmp0_ = self->priv->initializers;
	vala_collection_add ((ValaCollection*) _tmp0_, expr);
}


static gpointer
_vala_iterable_ref0 (gpointer self)
{
	return self ? vala_iterable_ref (self) : NULL;
}


static void
vala_ccode_initializer_list_real_write (ValaCCodeNode* base,
                                        ValaCCodeWriter* writer)
{
	ValaCCodeInitializerList * self;
	gboolean first = FALSE;
	self = (ValaCCodeInitializerList*) base;
	g_return_if_fail (writer != NULL);
	vala_ccode_writer_write_string (writer, "{");
	first = TRUE;
	{
		ValaList* _expr_list = NULL;
		ValaList* _tmp0_;
		ValaList* _tmp1_;
		gint _expr_size = 0;
		ValaList* _tmp2_;
		gint _tmp3_;
		gint _tmp4_;
		gint _expr_index = 0;
		_tmp0_ = self->priv->initializers;
		_tmp1_ = _vala_iterable_ref0 (_tmp0_);
		_expr_list = _tmp1_;
		_tmp2_ = _expr_list;
		_tmp3_ = vala_collection_get_size ((ValaCollection*) _tmp2_);
		_tmp4_ = _tmp3_;
		_expr_size = _tmp4_;
		_expr_index = -1;
		while (TRUE) {
			gint _tmp5_;
			gint _tmp6_;
			gint _tmp7_;
			ValaCCodeExpression* expr = NULL;
			ValaList* _tmp8_;
			gint _tmp9_;
			gpointer _tmp10_;
			gboolean _tmp11_;
			ValaCCodeExpression* _tmp12_;
			_tmp5_ = _expr_index;
			_expr_index = _tmp5_ + 1;
			_tmp6_ = _expr_index;
			_tmp7_ = _expr_size;
			if (!(_tmp6_ < _tmp7_)) {
				break;
			}
			_tmp8_ = _expr_list;
			_tmp9_ = _expr_index;
			_tmp10_ = vala_list_get (_tmp8_, _tmp9_);
			expr = (ValaCCodeExpression*) _tmp10_;
			_tmp11_ = first;
			if (!_tmp11_) {
				vala_ccode_writer_write_string (writer, ", ");
			} else {
				first = FALSE;
			}
			_tmp12_ = expr;
			if (_tmp12_ != NULL) {
				ValaCCodeExpression* _tmp13_;
				_tmp13_ = expr;
				vala_ccode_node_write ((ValaCCodeNode*) _tmp13_, writer);
			}
			_vala_ccode_node_unref0 (expr);
		}
		_vala_iterable_unref0 (_expr_list);
	}
	vala_ccode_writer_write_string (writer, "}");
}


ValaCCodeInitializerList*
vala_ccode_initializer_list_construct (GType object_type)
{
	ValaCCodeInitializerList* self = NULL;
	self = (ValaCCodeInitializerList*) vala_ccode_expression_construct (object_type);
	return self;
}


ValaCCodeInitializerList*
vala_ccode_initializer_list_new (void)
{
	return vala_ccode_initializer_list_construct (VALA_TYPE_CCODE_INITIALIZER_LIST);
}


static void
vala_ccode_initializer_list_class_init (ValaCCodeInitializerListClass * klass)
{
	vala_ccode_initializer_list_parent_class = g_type_class_peek_parent (klass);
	((ValaCCodeNodeClass *) klass)->finalize = vala_ccode_initializer_list_finalize;
	g_type_class_adjust_private_offset (klass, &ValaCCodeInitializerList_private_offset);
	((ValaCCodeNodeClass *) klass)->write = (void (*) (ValaCCodeNode *, ValaCCodeWriter*)) vala_ccode_initializer_list_real_write;
}


static void
vala_ccode_initializer_list_instance_init (ValaCCodeInitializerList * self)
{
	GEqualFunc _tmp0_;
	ValaArrayList* _tmp1_;
	self->priv = vala_ccode_initializer_list_get_instance_private (self);
	_tmp0_ = g_direct_equal;
	_tmp1_ = vala_array_list_new (VALA_TYPE_CCODE_EXPRESSION, (GBoxedCopyFunc) vala_ccode_node_ref, (GDestroyNotify) vala_ccode_node_unref, _tmp0_);
	self->priv->initializers = (ValaList*) _tmp1_;
}


static void
vala_ccode_initializer_list_finalize (ValaCCodeNode * obj)
{
	ValaCCodeInitializerList * self;
	self = G_TYPE_CHECK_INSTANCE_CAST (obj, VALA_TYPE_CCODE_INITIALIZER_LIST, ValaCCodeInitializerList);
	_vala_iterable_unref0 (self->priv->initializers);
	VALA_CCODE_NODE_CLASS (vala_ccode_initializer_list_parent_class)->finalize (obj);
}


/**
 * Represents a struct or array initializer list in the C code.
 */
GType
vala_ccode_initializer_list_get_type (void)
{
	static volatile gsize vala_ccode_initializer_list_type_id__volatile = 0;
	if (g_once_init_enter (&vala_ccode_initializer_list_type_id__volatile)) {
		static const GTypeInfo g_define_type_info = { sizeof (ValaCCodeInitializerListClass), (GBaseInitFunc) NULL, (GBaseFinalizeFunc) NULL, (GClassInitFunc) vala_ccode_initializer_list_class_init, (GClassFinalizeFunc) NULL, NULL, sizeof (ValaCCodeInitializerList), 0, (GInstanceInitFunc) vala_ccode_initializer_list_instance_init, NULL };
		GType vala_ccode_initializer_list_type_id;
		vala_ccode_initializer_list_type_id = g_type_register_static (VALA_TYPE_CCODE_EXPRESSION, "ValaCCodeInitializerList", &g_define_type_info, 0);
		ValaCCodeInitializerList_private_offset = g_type_add_instance_private (vala_ccode_initializer_list_type_id, sizeof (ValaCCodeInitializerListPrivate));
		g_once_init_leave (&vala_ccode_initializer_list_type_id__volatile, vala_ccode_initializer_list_type_id);
	}
	return vala_ccode_initializer_list_type_id__volatile;
}


