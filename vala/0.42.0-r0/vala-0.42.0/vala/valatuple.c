/* valatuple.c generated by valac, the Vala compiler
 * generated from valatuple.vala, do not modify */

/* valatuple.vala
 *
 * Copyright (C) 2006-2010  Jürg Billeter
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
#include "vala.h"
#include <valagee.h>

#define _vala_iterable_unref0(var) ((var == NULL) ? NULL : (var = (vala_iterable_unref (var), NULL)))
#define _vala_code_node_unref0(var) ((var == NULL) ? NULL : (var = (vala_code_node_unref (var), NULL)))

struct _ValaTuplePrivate {
	ValaList* expression_list;
};


static gint ValaTuple_private_offset;
static gpointer vala_tuple_parent_class = NULL;

static void vala_tuple_real_accept_children (ValaCodeNode* base,
                                      ValaCodeVisitor* visitor);
static void vala_tuple_real_accept (ValaCodeNode* base,
                             ValaCodeVisitor* visitor);
static gboolean vala_tuple_real_is_pure (ValaExpression* base);
static void vala_tuple_real_replace_expression (ValaCodeNode* base,
                                         ValaExpression* old_node,
                                         ValaExpression* new_node);
static gboolean vala_tuple_real_check (ValaCodeNode* base,
                                ValaCodeContext* context);
static void vala_tuple_real_emit (ValaCodeNode* base,
                           ValaCodeGenerator* codegen);
static void vala_tuple_finalize (ValaCodeNode * obj);


static inline gpointer
vala_tuple_get_instance_private (ValaTuple* self)
{
	return G_STRUCT_MEMBER_P (self, ValaTuple_private_offset);
}


ValaTuple*
vala_tuple_construct (GType object_type,
                      ValaSourceReference* source_reference)
{
	ValaTuple* self = NULL;
	self = (ValaTuple*) vala_expression_construct (object_type);
	vala_code_node_set_source_reference ((ValaCodeNode*) self, source_reference);
	return self;
}


ValaTuple*
vala_tuple_new (ValaSourceReference* source_reference)
{
	return vala_tuple_construct (VALA_TYPE_TUPLE, source_reference);
}


static gpointer
_vala_iterable_ref0 (gpointer self)
{
	return self ? vala_iterable_ref (self) : NULL;
}


static void
vala_tuple_real_accept_children (ValaCodeNode* base,
                                 ValaCodeVisitor* visitor)
{
	ValaTuple * self;
	self = (ValaTuple*) base;
	g_return_if_fail (visitor != NULL);
	{
		ValaList* _expr_list = NULL;
		ValaList* _tmp0_;
		ValaList* _tmp1_;
		gint _expr_size = 0;
		ValaList* _tmp2_;
		gint _tmp3_;
		gint _tmp4_;
		gint _expr_index = 0;
		_tmp0_ = self->priv->expression_list;
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
			ValaExpression* expr = NULL;
			ValaList* _tmp8_;
			gint _tmp9_;
			gpointer _tmp10_;
			ValaExpression* _tmp11_;
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
			expr = (ValaExpression*) _tmp10_;
			_tmp11_ = expr;
			vala_code_node_accept ((ValaCodeNode*) _tmp11_, visitor);
			_vala_code_node_unref0 (expr);
		}
		_vala_iterable_unref0 (_expr_list);
	}
}


static void
vala_tuple_real_accept (ValaCodeNode* base,
                        ValaCodeVisitor* visitor)
{
	ValaTuple * self;
	self = (ValaTuple*) base;
	g_return_if_fail (visitor != NULL);
	vala_code_visitor_visit_tuple (visitor, self);
	vala_code_visitor_visit_expression (visitor, (ValaExpression*) self);
}


void
vala_tuple_add_expression (ValaTuple* self,
                           ValaExpression* expr)
{
	ValaList* _tmp0_;
	g_return_if_fail (self != NULL);
	g_return_if_fail (expr != NULL);
	_tmp0_ = self->priv->expression_list;
	vala_collection_add ((ValaCollection*) _tmp0_, expr);
}


ValaList*
vala_tuple_get_expressions (ValaTuple* self)
{
	ValaList* result = NULL;
	ValaList* _tmp0_;
	ValaList* _tmp1_;
	g_return_val_if_fail (self != NULL, NULL);
	_tmp0_ = self->priv->expression_list;
	_tmp1_ = _vala_iterable_ref0 (_tmp0_);
	result = _tmp1_;
	return result;
}


static gboolean
vala_tuple_real_is_pure (ValaExpression* base)
{
	ValaTuple * self;
	gboolean result = FALSE;
	self = (ValaTuple*) base;
	result = FALSE;
	return result;
}


static void
vala_tuple_real_replace_expression (ValaCodeNode* base,
                                    ValaExpression* old_node,
                                    ValaExpression* new_node)
{
	ValaTuple * self;
	self = (ValaTuple*) base;
	g_return_if_fail (old_node != NULL);
	g_return_if_fail (new_node != NULL);
	{
		gint i = 0;
		i = 0;
		{
			gboolean _tmp0_ = FALSE;
			_tmp0_ = TRUE;
			while (TRUE) {
				gint _tmp2_;
				ValaList* _tmp3_;
				gint _tmp4_;
				gint _tmp5_;
				ValaList* _tmp6_;
				gint _tmp7_;
				gpointer _tmp8_;
				ValaExpression* _tmp9_;
				gboolean _tmp10_;
				if (!_tmp0_) {
					gint _tmp1_;
					_tmp1_ = i;
					i = _tmp1_ + 1;
				}
				_tmp0_ = FALSE;
				_tmp2_ = i;
				_tmp3_ = self->priv->expression_list;
				_tmp4_ = vala_collection_get_size ((ValaCollection*) _tmp3_);
				_tmp5_ = _tmp4_;
				if (!(_tmp2_ < _tmp5_)) {
					break;
				}
				_tmp6_ = self->priv->expression_list;
				_tmp7_ = i;
				_tmp8_ = vala_list_get (_tmp6_, _tmp7_);
				_tmp9_ = (ValaExpression*) _tmp8_;
				_tmp10_ = _tmp9_ == old_node;
				_vala_code_node_unref0 (_tmp9_);
				if (_tmp10_) {
					ValaList* _tmp11_;
					gint _tmp12_;
					_tmp11_ = self->priv->expression_list;
					_tmp12_ = i;
					vala_list_set (_tmp11_, _tmp12_, new_node);
				}
			}
		}
	}
}


static gboolean
vala_tuple_real_check (ValaCodeNode* base,
                       ValaCodeContext* context)
{
	ValaTuple * self;
	gboolean result = FALSE;
	gboolean _tmp0_;
	gboolean _tmp1_;
	ValaSourceReference* _tmp4_;
	ValaSourceReference* _tmp5_;
	self = (ValaTuple*) base;
	g_return_val_if_fail (context != NULL, FALSE);
	_tmp0_ = vala_code_node_get_checked ((ValaCodeNode*) self);
	_tmp1_ = _tmp0_;
	if (_tmp1_) {
		gboolean _tmp2_;
		gboolean _tmp3_;
		_tmp2_ = vala_code_node_get_error ((ValaCodeNode*) self);
		_tmp3_ = _tmp2_;
		result = !_tmp3_;
		return result;
	}
	vala_code_node_set_checked ((ValaCodeNode*) self, TRUE);
	_tmp4_ = vala_code_node_get_source_reference ((ValaCodeNode*) self);
	_tmp5_ = _tmp4_;
	vala_report_error (_tmp5_, "tuples are not supported");
	vala_code_node_set_error ((ValaCodeNode*) self, TRUE);
	result = FALSE;
	return result;
}


static void
vala_tuple_real_emit (ValaCodeNode* base,
                      ValaCodeGenerator* codegen)
{
	ValaTuple * self;
	self = (ValaTuple*) base;
	g_return_if_fail (codegen != NULL);
	{
		ValaList* _expr_list = NULL;
		ValaList* _tmp0_;
		ValaList* _tmp1_;
		gint _expr_size = 0;
		ValaList* _tmp2_;
		gint _tmp3_;
		gint _tmp4_;
		gint _expr_index = 0;
		_tmp0_ = self->priv->expression_list;
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
			ValaExpression* expr = NULL;
			ValaList* _tmp8_;
			gint _tmp9_;
			gpointer _tmp10_;
			ValaExpression* _tmp11_;
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
			expr = (ValaExpression*) _tmp10_;
			_tmp11_ = expr;
			vala_code_node_emit ((ValaCodeNode*) _tmp11_, codegen);
			_vala_code_node_unref0 (expr);
		}
		_vala_iterable_unref0 (_expr_list);
	}
	vala_code_visitor_visit_tuple ((ValaCodeVisitor*) codegen, self);
	vala_code_visitor_visit_expression ((ValaCodeVisitor*) codegen, (ValaExpression*) self);
}


static void
vala_tuple_class_init (ValaTupleClass * klass)
{
	vala_tuple_parent_class = g_type_class_peek_parent (klass);
	((ValaCodeNodeClass *) klass)->finalize = vala_tuple_finalize;
	g_type_class_adjust_private_offset (klass, &ValaTuple_private_offset);
	((ValaCodeNodeClass *) klass)->accept_children = (void (*) (ValaCodeNode *, ValaCodeVisitor*)) vala_tuple_real_accept_children;
	((ValaCodeNodeClass *) klass)->accept = (void (*) (ValaCodeNode *, ValaCodeVisitor*)) vala_tuple_real_accept;
	((ValaExpressionClass *) klass)->is_pure = (gboolean (*) (ValaExpression *)) vala_tuple_real_is_pure;
	((ValaCodeNodeClass *) klass)->replace_expression = (void (*) (ValaCodeNode *, ValaExpression*, ValaExpression*)) vala_tuple_real_replace_expression;
	((ValaCodeNodeClass *) klass)->check = (gboolean (*) (ValaCodeNode *, ValaCodeContext*)) vala_tuple_real_check;
	((ValaCodeNodeClass *) klass)->emit = (void (*) (ValaCodeNode *, ValaCodeGenerator*)) vala_tuple_real_emit;
}


static void
vala_tuple_instance_init (ValaTuple * self)
{
	GEqualFunc _tmp0_;
	ValaArrayList* _tmp1_;
	self->priv = vala_tuple_get_instance_private (self);
	_tmp0_ = g_direct_equal;
	_tmp1_ = vala_array_list_new (VALA_TYPE_EXPRESSION, (GBoxedCopyFunc) vala_code_node_ref, (GDestroyNotify) vala_code_node_unref, _tmp0_);
	self->priv->expression_list = (ValaList*) _tmp1_;
}


static void
vala_tuple_finalize (ValaCodeNode * obj)
{
	ValaTuple * self;
	self = G_TYPE_CHECK_INSTANCE_CAST (obj, VALA_TYPE_TUPLE, ValaTuple);
	_vala_iterable_unref0 (self->priv->expression_list);
	VALA_CODE_NODE_CLASS (vala_tuple_parent_class)->finalize (obj);
}


/**
 * Represents a fixed-length sequence of expressions in the source code.
 */
GType
vala_tuple_get_type (void)
{
	static volatile gsize vala_tuple_type_id__volatile = 0;
	if (g_once_init_enter (&vala_tuple_type_id__volatile)) {
		static const GTypeInfo g_define_type_info = { sizeof (ValaTupleClass), (GBaseInitFunc) NULL, (GBaseFinalizeFunc) NULL, (GClassInitFunc) vala_tuple_class_init, (GClassFinalizeFunc) NULL, NULL, sizeof (ValaTuple), 0, (GInstanceInitFunc) vala_tuple_instance_init, NULL };
		GType vala_tuple_type_id;
		vala_tuple_type_id = g_type_register_static (VALA_TYPE_EXPRESSION, "ValaTuple", &g_define_type_info, 0);
		ValaTuple_private_offset = g_type_add_instance_private (vala_tuple_type_id, sizeof (ValaTuplePrivate));
		g_once_init_leave (&vala_tuple_type_id__volatile, vala_tuple_type_id);
	}
	return vala_tuple_type_id__volatile;
}


