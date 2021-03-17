/* valaccodeforstatement.c generated by valac, the Vala compiler
 * generated from valaccodeforstatement.vala, do not modify */

/* valaccodeforstatement.vala
 *
 * Copyright (C) 2006-2008  Jürg Billeter
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

#define _vala_ccode_node_unref0(var) ((var == NULL) ? NULL : (var = (vala_ccode_node_unref (var), NULL)))
#define _vala_iterable_unref0(var) ((var == NULL) ? NULL : (var = (vala_iterable_unref (var), NULL)))

struct _ValaCCodeForStatementPrivate {
	ValaCCodeExpression* _condition;
	ValaCCodeStatement* _body;
	ValaList* initializer;
	ValaList* iterator;
};


static gint ValaCCodeForStatement_private_offset;
static gpointer vala_ccode_for_statement_parent_class = NULL;

static void vala_ccode_for_statement_real_write (ValaCCodeNode* base,
                                          ValaCCodeWriter* writer);
static void vala_ccode_for_statement_finalize (ValaCCodeNode * obj);


static inline gpointer
vala_ccode_for_statement_get_instance_private (ValaCCodeForStatement* self)
{
	return G_STRUCT_MEMBER_P (self, ValaCCodeForStatement_private_offset);
}


ValaCCodeForStatement*
vala_ccode_for_statement_construct (GType object_type,
                                    ValaCCodeExpression* condition,
                                    ValaCCodeStatement* body)
{
	ValaCCodeForStatement* self = NULL;
	self = (ValaCCodeForStatement*) vala_ccode_statement_construct (object_type);
	vala_ccode_for_statement_set_condition (self, condition);
	vala_ccode_for_statement_set_body (self, body);
	return self;
}


ValaCCodeForStatement*
vala_ccode_for_statement_new (ValaCCodeExpression* condition,
                              ValaCCodeStatement* body)
{
	return vala_ccode_for_statement_construct (VALA_TYPE_CCODE_FOR_STATEMENT, condition, body);
}


/**
 * Appends the specified expression to the list of initializers.
 *
 * @param expr an initializer expression
 */
void
vala_ccode_for_statement_add_initializer (ValaCCodeForStatement* self,
                                          ValaCCodeExpression* expr)
{
	ValaList* _tmp0_;
	g_return_if_fail (self != NULL);
	g_return_if_fail (expr != NULL);
	_tmp0_ = self->priv->initializer;
	vala_collection_add ((ValaCollection*) _tmp0_, expr);
}


/**
 * Appends the specified expression to the iterator.
 *
 * @param expr an iterator expression
 */
void
vala_ccode_for_statement_add_iterator (ValaCCodeForStatement* self,
                                       ValaCCodeExpression* expr)
{
	ValaList* _tmp0_;
	g_return_if_fail (self != NULL);
	g_return_if_fail (expr != NULL);
	_tmp0_ = self->priv->iterator;
	vala_collection_add ((ValaCollection*) _tmp0_, expr);
}


static gpointer
_vala_iterable_ref0 (gpointer self)
{
	return self ? vala_iterable_ref (self) : NULL;
}


static void
vala_ccode_for_statement_real_write (ValaCCodeNode* base,
                                     ValaCCodeWriter* writer)
{
	ValaCCodeForStatement * self;
	gboolean first = FALSE;
	ValaCCodeLineDirective* _tmp0_;
	ValaCCodeLineDirective* _tmp1_;
	ValaCCodeExpression* _tmp16_;
	ValaCCodeStatement* _tmp32_;
	self = (ValaCCodeForStatement*) base;
	g_return_if_fail (writer != NULL);
	_tmp0_ = vala_ccode_node_get_line ((ValaCCodeNode*) self);
	_tmp1_ = _tmp0_;
	vala_ccode_writer_write_indent (writer, _tmp1_);
	vala_ccode_writer_write_string (writer, "for (");
	first = TRUE;
	{
		ValaList* _init_expr_list = NULL;
		ValaList* _tmp2_;
		ValaList* _tmp3_;
		gint _init_expr_size = 0;
		ValaList* _tmp4_;
		gint _tmp5_;
		gint _tmp6_;
		gint _init_expr_index = 0;
		_tmp2_ = self->priv->initializer;
		_tmp3_ = _vala_iterable_ref0 (_tmp2_);
		_init_expr_list = _tmp3_;
		_tmp4_ = _init_expr_list;
		_tmp5_ = vala_collection_get_size ((ValaCollection*) _tmp4_);
		_tmp6_ = _tmp5_;
		_init_expr_size = _tmp6_;
		_init_expr_index = -1;
		while (TRUE) {
			gint _tmp7_;
			gint _tmp8_;
			gint _tmp9_;
			ValaCCodeExpression* init_expr = NULL;
			ValaList* _tmp10_;
			gint _tmp11_;
			gpointer _tmp12_;
			gboolean _tmp13_;
			ValaCCodeExpression* _tmp14_;
			_tmp7_ = _init_expr_index;
			_init_expr_index = _tmp7_ + 1;
			_tmp8_ = _init_expr_index;
			_tmp9_ = _init_expr_size;
			if (!(_tmp8_ < _tmp9_)) {
				break;
			}
			_tmp10_ = _init_expr_list;
			_tmp11_ = _init_expr_index;
			_tmp12_ = vala_list_get (_tmp10_, _tmp11_);
			init_expr = (ValaCCodeExpression*) _tmp12_;
			_tmp13_ = first;
			if (!_tmp13_) {
				vala_ccode_writer_write_string (writer, ", ");
			} else {
				first = FALSE;
			}
			_tmp14_ = init_expr;
			if (_tmp14_ != NULL) {
				ValaCCodeExpression* _tmp15_;
				_tmp15_ = init_expr;
				vala_ccode_node_write ((ValaCCodeNode*) _tmp15_, writer);
			}
			_vala_ccode_node_unref0 (init_expr);
		}
		_vala_iterable_unref0 (_init_expr_list);
	}
	vala_ccode_writer_write_string (writer, "; ");
	_tmp16_ = self->priv->_condition;
	if (_tmp16_ != NULL) {
		ValaCCodeExpression* _tmp17_;
		_tmp17_ = self->priv->_condition;
		vala_ccode_node_write ((ValaCCodeNode*) _tmp17_, writer);
	}
	vala_ccode_writer_write_string (writer, "; ");
	first = TRUE;
	{
		ValaList* _it_expr_list = NULL;
		ValaList* _tmp18_;
		ValaList* _tmp19_;
		gint _it_expr_size = 0;
		ValaList* _tmp20_;
		gint _tmp21_;
		gint _tmp22_;
		gint _it_expr_index = 0;
		_tmp18_ = self->priv->iterator;
		_tmp19_ = _vala_iterable_ref0 (_tmp18_);
		_it_expr_list = _tmp19_;
		_tmp20_ = _it_expr_list;
		_tmp21_ = vala_collection_get_size ((ValaCollection*) _tmp20_);
		_tmp22_ = _tmp21_;
		_it_expr_size = _tmp22_;
		_it_expr_index = -1;
		while (TRUE) {
			gint _tmp23_;
			gint _tmp24_;
			gint _tmp25_;
			ValaCCodeExpression* it_expr = NULL;
			ValaList* _tmp26_;
			gint _tmp27_;
			gpointer _tmp28_;
			gboolean _tmp29_;
			ValaCCodeExpression* _tmp30_;
			_tmp23_ = _it_expr_index;
			_it_expr_index = _tmp23_ + 1;
			_tmp24_ = _it_expr_index;
			_tmp25_ = _it_expr_size;
			if (!(_tmp24_ < _tmp25_)) {
				break;
			}
			_tmp26_ = _it_expr_list;
			_tmp27_ = _it_expr_index;
			_tmp28_ = vala_list_get (_tmp26_, _tmp27_);
			it_expr = (ValaCCodeExpression*) _tmp28_;
			_tmp29_ = first;
			if (!_tmp29_) {
				vala_ccode_writer_write_string (writer, ", ");
			} else {
				first = FALSE;
			}
			_tmp30_ = it_expr;
			if (_tmp30_ != NULL) {
				ValaCCodeExpression* _tmp31_;
				_tmp31_ = it_expr;
				vala_ccode_node_write ((ValaCCodeNode*) _tmp31_, writer);
			}
			_vala_ccode_node_unref0 (it_expr);
		}
		_vala_iterable_unref0 (_it_expr_list);
	}
	vala_ccode_writer_write_string (writer, ")");
	_tmp32_ = self->priv->_body;
	vala_ccode_node_write ((ValaCCodeNode*) _tmp32_, writer);
}


ValaCCodeExpression*
vala_ccode_for_statement_get_condition (ValaCCodeForStatement* self)
{
	ValaCCodeExpression* result;
	ValaCCodeExpression* _tmp0_;
	g_return_val_if_fail (self != NULL, NULL);
	_tmp0_ = self->priv->_condition;
	result = _tmp0_;
	return result;
}


static gpointer
_vala_ccode_node_ref0 (gpointer self)
{
	return self ? vala_ccode_node_ref (self) : NULL;
}


void
vala_ccode_for_statement_set_condition (ValaCCodeForStatement* self,
                                        ValaCCodeExpression* value)
{
	ValaCCodeExpression* _tmp0_;
	g_return_if_fail (self != NULL);
	_tmp0_ = _vala_ccode_node_ref0 (value);
	_vala_ccode_node_unref0 (self->priv->_condition);
	self->priv->_condition = _tmp0_;
}


ValaCCodeStatement*
vala_ccode_for_statement_get_body (ValaCCodeForStatement* self)
{
	ValaCCodeStatement* result;
	ValaCCodeStatement* _tmp0_;
	g_return_val_if_fail (self != NULL, NULL);
	_tmp0_ = self->priv->_body;
	result = _tmp0_;
	return result;
}


void
vala_ccode_for_statement_set_body (ValaCCodeForStatement* self,
                                   ValaCCodeStatement* value)
{
	ValaCCodeStatement* _tmp0_;
	g_return_if_fail (self != NULL);
	_tmp0_ = _vala_ccode_node_ref0 (value);
	_vala_ccode_node_unref0 (self->priv->_body);
	self->priv->_body = _tmp0_;
}


static void
vala_ccode_for_statement_class_init (ValaCCodeForStatementClass * klass)
{
	vala_ccode_for_statement_parent_class = g_type_class_peek_parent (klass);
	((ValaCCodeNodeClass *) klass)->finalize = vala_ccode_for_statement_finalize;
	g_type_class_adjust_private_offset (klass, &ValaCCodeForStatement_private_offset);
	((ValaCCodeNodeClass *) klass)->write = (void (*) (ValaCCodeNode *, ValaCCodeWriter*)) vala_ccode_for_statement_real_write;
}


static void
vala_ccode_for_statement_instance_init (ValaCCodeForStatement * self)
{
	GEqualFunc _tmp0_;
	ValaArrayList* _tmp1_;
	GEqualFunc _tmp2_;
	ValaArrayList* _tmp3_;
	self->priv = vala_ccode_for_statement_get_instance_private (self);
	_tmp0_ = g_direct_equal;
	_tmp1_ = vala_array_list_new (VALA_TYPE_CCODE_EXPRESSION, (GBoxedCopyFunc) vala_ccode_node_ref, (GDestroyNotify) vala_ccode_node_unref, _tmp0_);
	self->priv->initializer = (ValaList*) _tmp1_;
	_tmp2_ = g_direct_equal;
	_tmp3_ = vala_array_list_new (VALA_TYPE_CCODE_EXPRESSION, (GBoxedCopyFunc) vala_ccode_node_ref, (GDestroyNotify) vala_ccode_node_unref, _tmp2_);
	self->priv->iterator = (ValaList*) _tmp3_;
}


static void
vala_ccode_for_statement_finalize (ValaCCodeNode * obj)
{
	ValaCCodeForStatement * self;
	self = G_TYPE_CHECK_INSTANCE_CAST (obj, VALA_TYPE_CCODE_FOR_STATEMENT, ValaCCodeForStatement);
	_vala_ccode_node_unref0 (self->priv->_condition);
	_vala_ccode_node_unref0 (self->priv->_body);
	_vala_iterable_unref0 (self->priv->initializer);
	_vala_iterable_unref0 (self->priv->iterator);
	VALA_CCODE_NODE_CLASS (vala_ccode_for_statement_parent_class)->finalize (obj);
}


/**
 * Represents a for iteration statement in the C code.
 */
GType
vala_ccode_for_statement_get_type (void)
{
	static volatile gsize vala_ccode_for_statement_type_id__volatile = 0;
	if (g_once_init_enter (&vala_ccode_for_statement_type_id__volatile)) {
		static const GTypeInfo g_define_type_info = { sizeof (ValaCCodeForStatementClass), (GBaseInitFunc) NULL, (GBaseFinalizeFunc) NULL, (GClassInitFunc) vala_ccode_for_statement_class_init, (GClassFinalizeFunc) NULL, NULL, sizeof (ValaCCodeForStatement), 0, (GInstanceInitFunc) vala_ccode_for_statement_instance_init, NULL };
		GType vala_ccode_for_statement_type_id;
		vala_ccode_for_statement_type_id = g_type_register_static (VALA_TYPE_CCODE_STATEMENT, "ValaCCodeForStatement", &g_define_type_info, 0);
		ValaCCodeForStatement_private_offset = g_type_add_instance_private (vala_ccode_for_statement_type_id, sizeof (ValaCCodeForStatementPrivate));
		g_once_init_leave (&vala_ccode_for_statement_type_id__volatile, vala_ccode_for_statement_type_id);
	}
	return vala_ccode_for_statement_type_id__volatile;
}


