/* valaccodewhilestatement.c generated by valac, the Vala compiler
 * generated from valaccodewhilestatement.vala, do not modify */

/* valaccodewhilestatement.vala
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

#define _vala_ccode_node_unref0(var) ((var == NULL) ? NULL : (var = (vala_ccode_node_unref (var), NULL)))

struct _ValaCCodeWhileStatementPrivate {
	ValaCCodeExpression* _condition;
	ValaCCodeStatement* _body;
};


static gint ValaCCodeWhileStatement_private_offset;
static gpointer vala_ccode_while_statement_parent_class = NULL;

static void vala_ccode_while_statement_real_write (ValaCCodeNode* base,
                                            ValaCCodeWriter* writer);
static void vala_ccode_while_statement_finalize (ValaCCodeNode * obj);


static inline gpointer
vala_ccode_while_statement_get_instance_private (ValaCCodeWhileStatement* self)
{
	return G_STRUCT_MEMBER_P (self, ValaCCodeWhileStatement_private_offset);
}


ValaCCodeWhileStatement*
vala_ccode_while_statement_construct (GType object_type,
                                      ValaCCodeExpression* cond,
                                      ValaCCodeStatement* stmt)
{
	ValaCCodeWhileStatement* self = NULL;
	g_return_val_if_fail (cond != NULL, NULL);
	self = (ValaCCodeWhileStatement*) vala_ccode_statement_construct (object_type);
	vala_ccode_while_statement_set_condition (self, cond);
	vala_ccode_while_statement_set_body (self, stmt);
	return self;
}


ValaCCodeWhileStatement*
vala_ccode_while_statement_new (ValaCCodeExpression* cond,
                                ValaCCodeStatement* stmt)
{
	return vala_ccode_while_statement_construct (VALA_TYPE_CCODE_WHILE_STATEMENT, cond, stmt);
}


static void
vala_ccode_while_statement_real_write (ValaCCodeNode* base,
                                       ValaCCodeWriter* writer)
{
	ValaCCodeWhileStatement * self;
	ValaCCodeLineDirective* _tmp0_;
	ValaCCodeLineDirective* _tmp1_;
	ValaCCodeExpression* _tmp2_;
	ValaCCodeStatement* _tmp3_;
	self = (ValaCCodeWhileStatement*) base;
	g_return_if_fail (writer != NULL);
	_tmp0_ = vala_ccode_node_get_line ((ValaCCodeNode*) self);
	_tmp1_ = _tmp0_;
	vala_ccode_writer_write_indent (writer, _tmp1_);
	vala_ccode_writer_write_string (writer, "while (");
	_tmp2_ = self->priv->_condition;
	vala_ccode_node_write ((ValaCCodeNode*) _tmp2_, writer);
	vala_ccode_writer_write_string (writer, ")");
	_tmp3_ = self->priv->_body;
	vala_ccode_node_write ((ValaCCodeNode*) _tmp3_, writer);
}


ValaCCodeExpression*
vala_ccode_while_statement_get_condition (ValaCCodeWhileStatement* self)
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
vala_ccode_while_statement_set_condition (ValaCCodeWhileStatement* self,
                                          ValaCCodeExpression* value)
{
	ValaCCodeExpression* _tmp0_;
	g_return_if_fail (self != NULL);
	_tmp0_ = _vala_ccode_node_ref0 (value);
	_vala_ccode_node_unref0 (self->priv->_condition);
	self->priv->_condition = _tmp0_;
}


ValaCCodeStatement*
vala_ccode_while_statement_get_body (ValaCCodeWhileStatement* self)
{
	ValaCCodeStatement* result;
	ValaCCodeStatement* _tmp0_;
	g_return_val_if_fail (self != NULL, NULL);
	_tmp0_ = self->priv->_body;
	result = _tmp0_;
	return result;
}


void
vala_ccode_while_statement_set_body (ValaCCodeWhileStatement* self,
                                     ValaCCodeStatement* value)
{
	ValaCCodeStatement* _tmp0_;
	g_return_if_fail (self != NULL);
	_tmp0_ = _vala_ccode_node_ref0 (value);
	_vala_ccode_node_unref0 (self->priv->_body);
	self->priv->_body = _tmp0_;
}


static void
vala_ccode_while_statement_class_init (ValaCCodeWhileStatementClass * klass)
{
	vala_ccode_while_statement_parent_class = g_type_class_peek_parent (klass);
	((ValaCCodeNodeClass *) klass)->finalize = vala_ccode_while_statement_finalize;
	g_type_class_adjust_private_offset (klass, &ValaCCodeWhileStatement_private_offset);
	((ValaCCodeNodeClass *) klass)->write = (void (*) (ValaCCodeNode *, ValaCCodeWriter*)) vala_ccode_while_statement_real_write;
}


static void
vala_ccode_while_statement_instance_init (ValaCCodeWhileStatement * self)
{
	self->priv = vala_ccode_while_statement_get_instance_private (self);
}


static void
vala_ccode_while_statement_finalize (ValaCCodeNode * obj)
{
	ValaCCodeWhileStatement * self;
	self = G_TYPE_CHECK_INSTANCE_CAST (obj, VALA_TYPE_CCODE_WHILE_STATEMENT, ValaCCodeWhileStatement);
	_vala_ccode_node_unref0 (self->priv->_condition);
	_vala_ccode_node_unref0 (self->priv->_body);
	VALA_CCODE_NODE_CLASS (vala_ccode_while_statement_parent_class)->finalize (obj);
}


/**
 * Represents a while iteration statement in the C code.
 */
GType
vala_ccode_while_statement_get_type (void)
{
	static volatile gsize vala_ccode_while_statement_type_id__volatile = 0;
	if (g_once_init_enter (&vala_ccode_while_statement_type_id__volatile)) {
		static const GTypeInfo g_define_type_info = { sizeof (ValaCCodeWhileStatementClass), (GBaseInitFunc) NULL, (GBaseFinalizeFunc) NULL, (GClassInitFunc) vala_ccode_while_statement_class_init, (GClassFinalizeFunc) NULL, NULL, sizeof (ValaCCodeWhileStatement), 0, (GInstanceInitFunc) vala_ccode_while_statement_instance_init, NULL };
		GType vala_ccode_while_statement_type_id;
		vala_ccode_while_statement_type_id = g_type_register_static (VALA_TYPE_CCODE_STATEMENT, "ValaCCodeWhileStatement", &g_define_type_info, 0);
		ValaCCodeWhileStatement_private_offset = g_type_add_instance_private (vala_ccode_while_statement_type_id, sizeof (ValaCCodeWhileStatementPrivate));
		g_once_init_leave (&vala_ccode_while_statement_type_id__volatile, vala_ccode_while_statement_type_id);
	}
	return vala_ccode_while_statement_type_id__volatile;
}


