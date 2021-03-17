/* valacastexpression.c generated by valac, the Vala compiler
 * generated from valacastexpression.vala, do not modify */

/* valacastexpression.vala
 *
 * Copyright (C) 2006-2011  Jürg Billeter
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

#define _vala_code_node_unref0(var) ((var == NULL) ? NULL : (var = (vala_code_node_unref (var), NULL)))

struct _ValaCastExpressionPrivate {
	gboolean _is_silent_cast;
	gboolean _is_non_null_cast;
	ValaExpression* _inner;
	ValaDataType* _data_type;
};


static gint ValaCastExpression_private_offset;
static gpointer vala_cast_expression_parent_class = NULL;

static void vala_cast_expression_set_is_silent_cast (ValaCastExpression* self,
                                              gboolean value);
static void vala_cast_expression_set_is_non_null_cast (ValaCastExpression* self,
                                                gboolean value);
static void vala_cast_expression_real_accept (ValaCodeNode* base,
                                       ValaCodeVisitor* visitor);
static void vala_cast_expression_real_accept_children (ValaCodeNode* base,
                                                ValaCodeVisitor* visitor);
static void vala_cast_expression_real_replace_expression (ValaCodeNode* base,
                                                   ValaExpression* old_node,
                                                   ValaExpression* new_node);
static gboolean vala_cast_expression_real_is_pure (ValaExpression* base);
static gboolean vala_cast_expression_real_is_accessible (ValaExpression* base,
                                                  ValaSymbol* sym);
static void vala_cast_expression_real_replace_type (ValaCodeNode* base,
                                             ValaDataType* old_type,
                                             ValaDataType* new_type);
static gboolean vala_cast_expression_real_check (ValaCodeNode* base,
                                          ValaCodeContext* context);
static gboolean vala_cast_expression_is_gvariant (ValaCastExpression* self,
                                           ValaCodeContext* context,
                                           ValaDataType* type);
static void vala_cast_expression_real_emit (ValaCodeNode* base,
                                     ValaCodeGenerator* codegen);
static void vala_cast_expression_real_get_defined_variables (ValaCodeNode* base,
                                                      ValaCollection* collection);
static void vala_cast_expression_real_get_used_variables (ValaCodeNode* base,
                                                   ValaCollection* collection);
static gboolean vala_cast_expression_real_is_constant (ValaExpression* base);
static void vala_cast_expression_finalize (ValaCodeNode * obj);


static inline gpointer
vala_cast_expression_get_instance_private (ValaCastExpression* self)
{
	return G_STRUCT_MEMBER_P (self, ValaCastExpression_private_offset);
}


/**
 * Creates a new cast expression.
 *
 * @param inner           expression to be cast
 * @param type_reference  target type
 * @return                newly created cast expression
 */
ValaCastExpression*
vala_cast_expression_construct (GType object_type,
                                ValaExpression* inner,
                                ValaDataType* type_reference,
                                ValaSourceReference* source_reference)
{
	ValaCastExpression* self = NULL;
	g_return_val_if_fail (inner != NULL, NULL);
	g_return_val_if_fail (type_reference != NULL, NULL);
	g_return_val_if_fail (source_reference != NULL, NULL);
	self = (ValaCastExpression*) vala_expression_construct (object_type);
	vala_cast_expression_set_type_reference (self, type_reference);
	vala_code_node_set_source_reference ((ValaCodeNode*) self, source_reference);
	vala_cast_expression_set_is_silent_cast (self, FALSE);
	vala_cast_expression_set_is_non_null_cast (self, FALSE);
	vala_cast_expression_set_inner (self, inner);
	return self;
}


ValaCastExpression*
vala_cast_expression_new (ValaExpression* inner,
                          ValaDataType* type_reference,
                          ValaSourceReference* source_reference)
{
	return vala_cast_expression_construct (VALA_TYPE_CAST_EXPRESSION, inner, type_reference, source_reference);
}


ValaCastExpression*
vala_cast_expression_construct_silent (GType object_type,
                                       ValaExpression* inner,
                                       ValaDataType* type_reference,
                                       ValaSourceReference* source_reference)
{
	ValaCastExpression* self = NULL;
	g_return_val_if_fail (inner != NULL, NULL);
	g_return_val_if_fail (type_reference != NULL, NULL);
	g_return_val_if_fail (source_reference != NULL, NULL);
	self = (ValaCastExpression*) vala_expression_construct (object_type);
	vala_cast_expression_set_type_reference (self, type_reference);
	vala_code_node_set_source_reference ((ValaCodeNode*) self, source_reference);
	vala_cast_expression_set_is_silent_cast (self, TRUE);
	vala_cast_expression_set_is_non_null_cast (self, FALSE);
	vala_cast_expression_set_inner (self, inner);
	return self;
}


ValaCastExpression*
vala_cast_expression_new_silent (ValaExpression* inner,
                                 ValaDataType* type_reference,
                                 ValaSourceReference* source_reference)
{
	return vala_cast_expression_construct_silent (VALA_TYPE_CAST_EXPRESSION, inner, type_reference, source_reference);
}


ValaCastExpression*
vala_cast_expression_construct_non_null (GType object_type,
                                         ValaExpression* inner,
                                         ValaSourceReference* source_reference)
{
	ValaCastExpression* self = NULL;
	g_return_val_if_fail (inner != NULL, NULL);
	g_return_val_if_fail (source_reference != NULL, NULL);
	self = (ValaCastExpression*) vala_expression_construct (object_type);
	vala_cast_expression_set_inner (self, inner);
	vala_cast_expression_set_is_non_null_cast (self, TRUE);
	vala_code_node_set_source_reference ((ValaCodeNode*) self, source_reference);
	return self;
}


ValaCastExpression*
vala_cast_expression_new_non_null (ValaExpression* inner,
                                   ValaSourceReference* source_reference)
{
	return vala_cast_expression_construct_non_null (VALA_TYPE_CAST_EXPRESSION, inner, source_reference);
}


static void
vala_cast_expression_real_accept (ValaCodeNode* base,
                                  ValaCodeVisitor* visitor)
{
	ValaCastExpression * self;
	self = (ValaCastExpression*) base;
	g_return_if_fail (visitor != NULL);
	vala_code_visitor_visit_cast_expression (visitor, self);
	vala_code_visitor_visit_expression (visitor, (ValaExpression*) self);
}


static void
vala_cast_expression_real_accept_children (ValaCodeNode* base,
                                           ValaCodeVisitor* visitor)
{
	ValaCastExpression * self;
	ValaExpression* _tmp0_;
	ValaExpression* _tmp1_;
	gboolean _tmp2_;
	self = (ValaCastExpression*) base;
	g_return_if_fail (visitor != NULL);
	_tmp0_ = vala_cast_expression_get_inner (self);
	_tmp1_ = _tmp0_;
	vala_code_node_accept ((ValaCodeNode*) _tmp1_, visitor);
	_tmp2_ = self->priv->_is_non_null_cast;
	if (!_tmp2_) {
		ValaDataType* _tmp3_;
		ValaDataType* _tmp4_;
		_tmp3_ = vala_cast_expression_get_type_reference (self);
		_tmp4_ = _tmp3_;
		vala_code_node_accept ((ValaCodeNode*) _tmp4_, visitor);
	}
}


static void
vala_cast_expression_real_replace_expression (ValaCodeNode* base,
                                              ValaExpression* old_node,
                                              ValaExpression* new_node)
{
	ValaCastExpression * self;
	ValaExpression* _tmp0_;
	ValaExpression* _tmp1_;
	self = (ValaCastExpression*) base;
	g_return_if_fail (old_node != NULL);
	g_return_if_fail (new_node != NULL);
	_tmp0_ = vala_cast_expression_get_inner (self);
	_tmp1_ = _tmp0_;
	if (_tmp1_ == old_node) {
		vala_cast_expression_set_inner (self, new_node);
	}
}


static gboolean
vala_cast_expression_real_is_pure (ValaExpression* base)
{
	ValaCastExpression * self;
	gboolean result = FALSE;
	ValaExpression* _tmp0_;
	ValaExpression* _tmp1_;
	self = (ValaCastExpression*) base;
	_tmp0_ = vala_cast_expression_get_inner (self);
	_tmp1_ = _tmp0_;
	result = vala_expression_is_pure (_tmp1_);
	return result;
}


static gboolean
vala_cast_expression_real_is_accessible (ValaExpression* base,
                                         ValaSymbol* sym)
{
	ValaCastExpression * self;
	gboolean result = FALSE;
	ValaExpression* _tmp0_;
	ValaExpression* _tmp1_;
	self = (ValaCastExpression*) base;
	g_return_val_if_fail (sym != NULL, FALSE);
	_tmp0_ = vala_cast_expression_get_inner (self);
	_tmp1_ = _tmp0_;
	result = vala_expression_is_accessible (_tmp1_, sym);
	return result;
}


static void
vala_cast_expression_real_replace_type (ValaCodeNode* base,
                                        ValaDataType* old_type,
                                        ValaDataType* new_type)
{
	ValaCastExpression * self;
	ValaDataType* _tmp0_;
	ValaDataType* _tmp1_;
	self = (ValaCastExpression*) base;
	g_return_if_fail (old_type != NULL);
	g_return_if_fail (new_type != NULL);
	_tmp0_ = vala_cast_expression_get_type_reference (self);
	_tmp1_ = _tmp0_;
	if (_tmp1_ == old_type) {
		vala_cast_expression_set_type_reference (self, new_type);
	}
}


static gboolean
vala_cast_expression_real_check (ValaCodeNode* base,
                                 ValaCodeContext* context)
{
	ValaCastExpression * self;
	gboolean result = FALSE;
	gboolean _tmp0_;
	gboolean _tmp1_;
	ValaExpression* _tmp4_;
	ValaExpression* _tmp5_;
	ValaExpression* _tmp6_;
	ValaExpression* _tmp7_;
	ValaDataType* _tmp8_;
	ValaDataType* _tmp9_;
	gboolean _tmp12_;
	ValaDataType* _tmp21_;
	ValaDataType* _tmp22_;
	gboolean _tmp23_ = FALSE;
	ValaDataType* _tmp24_;
	ValaDataType* _tmp25_;
	ValaDataType* _tmp44_;
	ValaDataType* _tmp45_;
	ValaDataType* _tmp46_;
	ValaDataType* _tmp47_;
	ValaExpression* _tmp48_;
	ValaExpression* _tmp49_;
	ValaDataType* _tmp50_;
	ValaDataType* _tmp51_;
	gboolean _tmp52_;
	gboolean _tmp53_;
	ValaDataType* _tmp54_;
	ValaDataType* _tmp55_;
	ValaExpression* _tmp56_;
	ValaExpression* _tmp57_;
	ValaDataType* _tmp58_;
	ValaDataType* _tmp59_;
	gboolean _tmp60_;
	gboolean _tmp61_;
	gboolean _tmp62_;
	gboolean _tmp65_ = FALSE;
	ValaExpression* _tmp66_;
	ValaExpression* _tmp67_;
	ValaDataType* _tmp68_;
	ValaDataType* _tmp69_;
	ValaExpression* _tmp74_;
	ValaExpression* _tmp75_;
	ValaExpression* _tmp76_;
	ValaExpression* _tmp77_;
	ValaDataType* _tmp78_;
	ValaDataType* _tmp79_;
	ValaDataType* _tmp80_;
	ValaDataType* _tmp81_;
	gboolean _tmp82_;
	gboolean _tmp83_;
	self = (ValaCastExpression*) base;
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
	_tmp4_ = vala_cast_expression_get_inner (self);
	_tmp5_ = _tmp4_;
	if (!vala_code_node_check ((ValaCodeNode*) _tmp5_, context)) {
		vala_code_node_set_error ((ValaCodeNode*) self, TRUE);
		result = FALSE;
		return result;
	}
	_tmp6_ = vala_cast_expression_get_inner (self);
	_tmp7_ = _tmp6_;
	_tmp8_ = vala_expression_get_value_type (_tmp7_);
	_tmp9_ = _tmp8_;
	if (_tmp9_ == NULL) {
		ValaSourceReference* _tmp10_;
		ValaSourceReference* _tmp11_;
		_tmp10_ = vala_code_node_get_source_reference ((ValaCodeNode*) self);
		_tmp11_ = _tmp10_;
		vala_report_error (_tmp11_, "Invalid cast expression");
		vala_code_node_set_error ((ValaCodeNode*) self, TRUE);
		result = FALSE;
		return result;
	}
	_tmp12_ = self->priv->_is_non_null_cast;
	if (_tmp12_) {
		ValaExpression* _tmp13_;
		ValaExpression* _tmp14_;
		ValaDataType* _tmp15_;
		ValaDataType* _tmp16_;
		ValaDataType* _tmp17_;
		ValaDataType* _tmp18_;
		ValaDataType* _tmp19_;
		ValaDataType* _tmp20_;
		_tmp13_ = vala_cast_expression_get_inner (self);
		_tmp14_ = _tmp13_;
		_tmp15_ = vala_expression_get_value_type (_tmp14_);
		_tmp16_ = _tmp15_;
		_tmp17_ = vala_data_type_copy (_tmp16_);
		_tmp18_ = _tmp17_;
		vala_cast_expression_set_type_reference (self, _tmp18_);
		_vala_code_node_unref0 (_tmp18_);
		_tmp19_ = vala_cast_expression_get_type_reference (self);
		_tmp20_ = _tmp19_;
		vala_data_type_set_nullable (_tmp20_, FALSE);
	}
	_tmp21_ = vala_cast_expression_get_type_reference (self);
	_tmp22_ = _tmp21_;
	vala_code_node_check ((ValaCodeNode*) _tmp22_, context);
	_tmp24_ = vala_cast_expression_get_type_reference (self);
	_tmp25_ = _tmp24_;
	if (G_TYPE_CHECK_INSTANCE_TYPE (_tmp25_, VALA_TYPE_DELEGATE_TYPE)) {
		ValaExpression* _tmp26_;
		ValaExpression* _tmp27_;
		ValaDataType* _tmp28_;
		ValaDataType* _tmp29_;
		_tmp26_ = vala_cast_expression_get_inner (self);
		_tmp27_ = _tmp26_;
		_tmp28_ = vala_expression_get_value_type (_tmp27_);
		_tmp29_ = _tmp28_;
		_tmp23_ = G_TYPE_CHECK_INSTANCE_TYPE (_tmp29_, VALA_TYPE_METHOD_TYPE);
	} else {
		_tmp23_ = FALSE;
	}
	if (_tmp23_) {
		ValaDataType* _tmp30_;
		ValaDataType* _tmp31_;
		_tmp30_ = vala_expression_get_target_type ((ValaExpression*) self);
		_tmp31_ = _tmp30_;
		if (_tmp31_ != NULL) {
			ValaExpression* _tmp32_;
			ValaExpression* _tmp33_;
			ValaDataType* _tmp34_;
			ValaDataType* _tmp35_;
			ValaDataType* _tmp36_;
			ValaDataType* _tmp37_;
			gboolean _tmp38_;
			gboolean _tmp39_;
			_tmp32_ = vala_cast_expression_get_inner (self);
			_tmp33_ = _tmp32_;
			_tmp34_ = vala_expression_get_value_type (_tmp33_);
			_tmp35_ = _tmp34_;
			_tmp36_ = vala_expression_get_target_type ((ValaExpression*) self);
			_tmp37_ = _tmp36_;
			_tmp38_ = vala_data_type_get_value_owned (_tmp37_);
			_tmp39_ = _tmp38_;
			vala_data_type_set_value_owned (_tmp35_, _tmp39_);
		} else {
			ValaExpression* _tmp40_;
			ValaExpression* _tmp41_;
			ValaDataType* _tmp42_;
			ValaDataType* _tmp43_;
			_tmp40_ = vala_cast_expression_get_inner (self);
			_tmp41_ = _tmp40_;
			_tmp42_ = vala_expression_get_value_type (_tmp41_);
			_tmp43_ = _tmp42_;
			vala_data_type_set_value_owned (_tmp43_, TRUE);
		}
	}
	_tmp44_ = vala_cast_expression_get_type_reference (self);
	_tmp45_ = _tmp44_;
	vala_expression_set_value_type ((ValaExpression*) self, _tmp45_);
	_tmp46_ = vala_expression_get_value_type ((ValaExpression*) self);
	_tmp47_ = _tmp46_;
	_tmp48_ = vala_cast_expression_get_inner (self);
	_tmp49_ = _tmp48_;
	_tmp50_ = vala_expression_get_value_type (_tmp49_);
	_tmp51_ = _tmp50_;
	_tmp52_ = vala_data_type_get_value_owned (_tmp51_);
	_tmp53_ = _tmp52_;
	vala_data_type_set_value_owned (_tmp47_, _tmp53_);
	_tmp54_ = vala_expression_get_value_type ((ValaExpression*) self);
	_tmp55_ = _tmp54_;
	_tmp56_ = vala_cast_expression_get_inner (self);
	_tmp57_ = _tmp56_;
	_tmp58_ = vala_expression_get_value_type (_tmp57_);
	_tmp59_ = _tmp58_;
	_tmp60_ = vala_data_type_get_floating_reference (_tmp59_);
	_tmp61_ = _tmp60_;
	vala_data_type_set_floating_reference (_tmp55_, _tmp61_);
	_tmp62_ = self->priv->_is_silent_cast;
	if (_tmp62_) {
		ValaDataType* _tmp63_;
		ValaDataType* _tmp64_;
		_tmp63_ = vala_expression_get_value_type ((ValaExpression*) self);
		_tmp64_ = _tmp63_;
		vala_data_type_set_nullable (_tmp64_, TRUE);
	}
	_tmp66_ = vala_cast_expression_get_inner (self);
	_tmp67_ = _tmp66_;
	_tmp68_ = vala_expression_get_value_type (_tmp67_);
	_tmp69_ = _tmp68_;
	if (vala_cast_expression_is_gvariant (self, context, _tmp69_)) {
		ValaDataType* _tmp70_;
		ValaDataType* _tmp71_;
		_tmp70_ = vala_expression_get_value_type ((ValaExpression*) self);
		_tmp71_ = _tmp70_;
		_tmp65_ = !vala_cast_expression_is_gvariant (self, context, _tmp71_);
	} else {
		_tmp65_ = FALSE;
	}
	if (_tmp65_) {
		ValaDataType* _tmp72_;
		ValaDataType* _tmp73_;
		_tmp72_ = vala_expression_get_value_type ((ValaExpression*) self);
		_tmp73_ = _tmp72_;
		vala_data_type_set_value_owned (_tmp73_, TRUE);
	}
	_tmp74_ = vala_cast_expression_get_inner (self);
	_tmp75_ = _tmp74_;
	_tmp76_ = vala_cast_expression_get_inner (self);
	_tmp77_ = _tmp76_;
	_tmp78_ = vala_expression_get_value_type (_tmp77_);
	_tmp79_ = _tmp78_;
	_tmp80_ = vala_data_type_copy (_tmp79_);
	_tmp81_ = _tmp80_;
	vala_expression_set_target_type (_tmp75_, _tmp81_);
	_vala_code_node_unref0 (_tmp81_);
	_tmp82_ = vala_code_node_get_error ((ValaCodeNode*) self);
	_tmp83_ = _tmp82_;
	result = !_tmp83_;
	return result;
}


static gboolean
vala_cast_expression_is_gvariant (ValaCastExpression* self,
                                  ValaCodeContext* context,
                                  ValaDataType* type)
{
	gboolean result = FALSE;
	gboolean _tmp0_ = FALSE;
	ValaTypeSymbol* _tmp1_;
	ValaTypeSymbol* _tmp2_;
	g_return_val_if_fail (self != NULL, FALSE);
	g_return_val_if_fail (context != NULL, FALSE);
	g_return_val_if_fail (type != NULL, FALSE);
	_tmp1_ = vala_data_type_get_data_type (type);
	_tmp2_ = _tmp1_;
	if (_tmp2_ != NULL) {
		ValaTypeSymbol* _tmp3_;
		ValaTypeSymbol* _tmp4_;
		ValaSemanticAnalyzer* _tmp5_;
		ValaSemanticAnalyzer* _tmp6_;
		ValaObjectType* _tmp7_;
		ValaTypeSymbol* _tmp8_;
		ValaTypeSymbol* _tmp9_;
		_tmp3_ = vala_data_type_get_data_type (type);
		_tmp4_ = _tmp3_;
		_tmp5_ = vala_code_context_get_analyzer (context);
		_tmp6_ = _tmp5_;
		_tmp7_ = _tmp6_->gvariant_type;
		_tmp8_ = vala_data_type_get_data_type ((ValaDataType*) _tmp7_);
		_tmp9_ = _tmp8_;
		_tmp0_ = vala_typesymbol_is_subtype_of (_tmp4_, _tmp9_);
	} else {
		_tmp0_ = FALSE;
	}
	result = _tmp0_;
	return result;
}


static void
vala_cast_expression_real_emit (ValaCodeNode* base,
                                ValaCodeGenerator* codegen)
{
	ValaCastExpression * self;
	ValaExpression* _tmp0_;
	ValaExpression* _tmp1_;
	self = (ValaCastExpression*) base;
	g_return_if_fail (codegen != NULL);
	_tmp0_ = vala_cast_expression_get_inner (self);
	_tmp1_ = _tmp0_;
	vala_code_node_emit ((ValaCodeNode*) _tmp1_, codegen);
	vala_code_visitor_visit_cast_expression ((ValaCodeVisitor*) codegen, self);
	vala_code_visitor_visit_expression ((ValaCodeVisitor*) codegen, (ValaExpression*) self);
}


static void
vala_cast_expression_real_get_defined_variables (ValaCodeNode* base,
                                                 ValaCollection* collection)
{
	ValaCastExpression * self;
	ValaExpression* _tmp0_;
	ValaExpression* _tmp1_;
	self = (ValaCastExpression*) base;
	g_return_if_fail (collection != NULL);
	_tmp0_ = vala_cast_expression_get_inner (self);
	_tmp1_ = _tmp0_;
	vala_code_node_get_defined_variables ((ValaCodeNode*) _tmp1_, collection);
}


static void
vala_cast_expression_real_get_used_variables (ValaCodeNode* base,
                                              ValaCollection* collection)
{
	ValaCastExpression * self;
	ValaExpression* _tmp0_;
	ValaExpression* _tmp1_;
	self = (ValaCastExpression*) base;
	g_return_if_fail (collection != NULL);
	_tmp0_ = vala_cast_expression_get_inner (self);
	_tmp1_ = _tmp0_;
	vala_code_node_get_used_variables ((ValaCodeNode*) _tmp1_, collection);
}


static gboolean
vala_cast_expression_real_is_constant (ValaExpression* base)
{
	ValaCastExpression * self;
	gboolean result = FALSE;
	ValaExpression* _tmp0_;
	ValaExpression* _tmp1_;
	self = (ValaCastExpression*) base;
	_tmp0_ = vala_cast_expression_get_inner (self);
	_tmp1_ = _tmp0_;
	result = vala_expression_is_constant (_tmp1_);
	return result;
}


ValaExpression*
vala_cast_expression_get_inner (ValaCastExpression* self)
{
	ValaExpression* result;
	ValaExpression* _tmp0_;
	g_return_val_if_fail (self != NULL, NULL);
	_tmp0_ = self->priv->_inner;
	result = _tmp0_;
	return result;
}


static gpointer
_vala_code_node_ref0 (gpointer self)
{
	return self ? vala_code_node_ref (self) : NULL;
}


void
vala_cast_expression_set_inner (ValaCastExpression* self,
                                ValaExpression* value)
{
	ValaExpression* _tmp0_;
	ValaExpression* _tmp1_;
	g_return_if_fail (self != NULL);
	_tmp0_ = _vala_code_node_ref0 (value);
	_vala_code_node_unref0 (self->priv->_inner);
	self->priv->_inner = _tmp0_;
	_tmp1_ = self->priv->_inner;
	vala_code_node_set_parent_node ((ValaCodeNode*) _tmp1_, (ValaCodeNode*) self);
}


ValaDataType*
vala_cast_expression_get_type_reference (ValaCastExpression* self)
{
	ValaDataType* result;
	ValaDataType* _tmp0_;
	g_return_val_if_fail (self != NULL, NULL);
	_tmp0_ = self->priv->_data_type;
	result = _tmp0_;
	return result;
}


void
vala_cast_expression_set_type_reference (ValaCastExpression* self,
                                         ValaDataType* value)
{
	ValaDataType* _tmp0_;
	ValaDataType* _tmp1_;
	g_return_if_fail (self != NULL);
	_tmp0_ = _vala_code_node_ref0 (value);
	_vala_code_node_unref0 (self->priv->_data_type);
	self->priv->_data_type = _tmp0_;
	_tmp1_ = self->priv->_data_type;
	vala_code_node_set_parent_node ((ValaCodeNode*) _tmp1_, (ValaCodeNode*) self);
}


gboolean
vala_cast_expression_get_is_silent_cast (ValaCastExpression* self)
{
	gboolean result;
	gboolean _tmp0_;
	g_return_val_if_fail (self != NULL, FALSE);
	_tmp0_ = self->priv->_is_silent_cast;
	result = _tmp0_;
	return result;
}


static void
vala_cast_expression_set_is_silent_cast (ValaCastExpression* self,
                                         gboolean value)
{
	g_return_if_fail (self != NULL);
	self->priv->_is_silent_cast = value;
}


gboolean
vala_cast_expression_get_is_non_null_cast (ValaCastExpression* self)
{
	gboolean result;
	gboolean _tmp0_;
	g_return_val_if_fail (self != NULL, FALSE);
	_tmp0_ = self->priv->_is_non_null_cast;
	result = _tmp0_;
	return result;
}


static void
vala_cast_expression_set_is_non_null_cast (ValaCastExpression* self,
                                           gboolean value)
{
	g_return_if_fail (self != NULL);
	self->priv->_is_non_null_cast = value;
}


static void
vala_cast_expression_class_init (ValaCastExpressionClass * klass)
{
	vala_cast_expression_parent_class = g_type_class_peek_parent (klass);
	((ValaCodeNodeClass *) klass)->finalize = vala_cast_expression_finalize;
	g_type_class_adjust_private_offset (klass, &ValaCastExpression_private_offset);
	((ValaCodeNodeClass *) klass)->accept = (void (*) (ValaCodeNode *, ValaCodeVisitor*)) vala_cast_expression_real_accept;
	((ValaCodeNodeClass *) klass)->accept_children = (void (*) (ValaCodeNode *, ValaCodeVisitor*)) vala_cast_expression_real_accept_children;
	((ValaCodeNodeClass *) klass)->replace_expression = (void (*) (ValaCodeNode *, ValaExpression*, ValaExpression*)) vala_cast_expression_real_replace_expression;
	((ValaExpressionClass *) klass)->is_pure = (gboolean (*) (ValaExpression *)) vala_cast_expression_real_is_pure;
	((ValaExpressionClass *) klass)->is_accessible = (gboolean (*) (ValaExpression *, ValaSymbol*)) vala_cast_expression_real_is_accessible;
	((ValaCodeNodeClass *) klass)->replace_type = (void (*) (ValaCodeNode *, ValaDataType*, ValaDataType*)) vala_cast_expression_real_replace_type;
	((ValaCodeNodeClass *) klass)->check = (gboolean (*) (ValaCodeNode *, ValaCodeContext*)) vala_cast_expression_real_check;
	((ValaCodeNodeClass *) klass)->emit = (void (*) (ValaCodeNode *, ValaCodeGenerator*)) vala_cast_expression_real_emit;
	((ValaCodeNodeClass *) klass)->get_defined_variables = (void (*) (ValaCodeNode *, ValaCollection*)) vala_cast_expression_real_get_defined_variables;
	((ValaCodeNodeClass *) klass)->get_used_variables = (void (*) (ValaCodeNode *, ValaCollection*)) vala_cast_expression_real_get_used_variables;
	((ValaExpressionClass *) klass)->is_constant = (gboolean (*) (ValaExpression *)) vala_cast_expression_real_is_constant;
}


static void
vala_cast_expression_instance_init (ValaCastExpression * self)
{
	self->priv = vala_cast_expression_get_instance_private (self);
}


static void
vala_cast_expression_finalize (ValaCodeNode * obj)
{
	ValaCastExpression * self;
	self = G_TYPE_CHECK_INSTANCE_CAST (obj, VALA_TYPE_CAST_EXPRESSION, ValaCastExpression);
	_vala_code_node_unref0 (self->priv->_inner);
	_vala_code_node_unref0 (self->priv->_data_type);
	VALA_CODE_NODE_CLASS (vala_cast_expression_parent_class)->finalize (obj);
}


/**
 * Represents a type cast in the source code.
 */
GType
vala_cast_expression_get_type (void)
{
	static volatile gsize vala_cast_expression_type_id__volatile = 0;
	if (g_once_init_enter (&vala_cast_expression_type_id__volatile)) {
		static const GTypeInfo g_define_type_info = { sizeof (ValaCastExpressionClass), (GBaseInitFunc) NULL, (GBaseFinalizeFunc) NULL, (GClassInitFunc) vala_cast_expression_class_init, (GClassFinalizeFunc) NULL, NULL, sizeof (ValaCastExpression), 0, (GInstanceInitFunc) vala_cast_expression_instance_init, NULL };
		GType vala_cast_expression_type_id;
		vala_cast_expression_type_id = g_type_register_static (VALA_TYPE_EXPRESSION, "ValaCastExpression", &g_define_type_info, 0);
		ValaCastExpression_private_offset = g_type_add_instance_private (vala_cast_expression_type_id, sizeof (ValaCastExpressionPrivate));
		g_once_init_leave (&vala_cast_expression_type_id__volatile, vala_cast_expression_type_id);
	}
	return vala_cast_expression_type_id__volatile;
}


