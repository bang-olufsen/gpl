/* valacreationmethod.c generated by valac, the Vala compiler
 * generated from valacreationmethod.vala, do not modify */

/* valacreationmethod.vala
 *
 * Copyright (C) 2007-2010  Jürg Billeter
 * Copyright (C) 2007-2008  Raffaele Sandrini
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
 *	Raffaele Sandrini <raffaele@sandrini.ch>
 * 	Jürg Billeter <j@bitron.ch>
 */


#include <glib.h>
#include <glib-object.h>
#include "vala.h"
#include <stdlib.h>
#include <string.h>
#include <valagee.h>

#define _g_free0(var) (var = (g_free (var), NULL))
#define _vala_code_node_unref0(var) ((var == NULL) ? NULL : (var = (vala_code_node_unref (var), NULL)))
#define _vala_iterable_unref0(var) ((var == NULL) ? NULL : (var = (vala_iterable_unref (var), NULL)))
#define _vala_source_file_unref0(var) ((var == NULL) ? NULL : (var = (vala_source_file_unref (var), NULL)))

struct _ValaCreationMethodPrivate {
	gchar* _class_name;
	gboolean _chain_up;
};


static gint ValaCreationMethod_private_offset;
static gpointer vala_creation_method_parent_class = NULL;

static void vala_creation_method_real_accept (ValaCodeNode* base,
                                       ValaCodeVisitor* visitor);
static void vala_creation_method_real_accept_children (ValaCodeNode* base,
                                                ValaCodeVisitor* visitor);
static gboolean vala_creation_method_real_check (ValaCodeNode* base,
                                          ValaCodeContext* context);
static void vala_creation_method_finalize (ValaCodeNode * obj);


static inline gpointer
vala_creation_method_get_instance_private (ValaCreationMethod* self)
{
	return G_STRUCT_MEMBER_P (self, ValaCreationMethod_private_offset);
}


/**
 * Creates a new method.
 *
 * @param name             method name
 * @param source_reference reference to source code
 * @return                 newly created method
 */
ValaCreationMethod*
vala_creation_method_construct (GType object_type,
                                const gchar* class_name,
                                const gchar* name,
                                ValaSourceReference* source_reference,
                                ValaComment* comment)
{
	ValaCreationMethod* self = NULL;
	ValaVoidType* _tmp0_;
	ValaVoidType* _tmp1_;
	_tmp0_ = vala_void_type_new (NULL);
	_tmp1_ = _tmp0_;
	self = (ValaCreationMethod*) vala_method_construct (object_type, name, (ValaDataType*) _tmp1_, source_reference, comment);
	_vala_code_node_unref0 (_tmp1_);
	vala_creation_method_set_class_name (self, class_name);
	return self;
}


ValaCreationMethod*
vala_creation_method_new (const gchar* class_name,
                          const gchar* name,
                          ValaSourceReference* source_reference,
                          ValaComment* comment)
{
	return vala_creation_method_construct (VALA_TYPE_CREATION_METHOD, class_name, name, source_reference, comment);
}


static void
vala_creation_method_real_accept (ValaCodeNode* base,
                                  ValaCodeVisitor* visitor)
{
	ValaCreationMethod * self;
	self = (ValaCreationMethod*) base;
	g_return_if_fail (visitor != NULL);
	vala_code_visitor_visit_creation_method (visitor, self);
}


static void
vala_creation_method_real_accept_children (ValaCodeNode* base,
                                           ValaCodeVisitor* visitor)
{
	ValaCreationMethod * self;
	ValaBlock* _tmp44_;
	ValaBlock* _tmp45_;
	self = (ValaCreationMethod*) base;
	g_return_if_fail (visitor != NULL);
	{
		ValaList* _param_list = NULL;
		ValaList* _tmp0_;
		gint _param_size = 0;
		ValaList* _tmp1_;
		gint _tmp2_;
		gint _tmp3_;
		gint _param_index = 0;
		_tmp0_ = vala_callable_get_parameters ((ValaCallable*) self);
		_param_list = _tmp0_;
		_tmp1_ = _param_list;
		_tmp2_ = vala_collection_get_size ((ValaCollection*) _tmp1_);
		_tmp3_ = _tmp2_;
		_param_size = _tmp3_;
		_param_index = -1;
		while (TRUE) {
			gint _tmp4_;
			gint _tmp5_;
			gint _tmp6_;
			ValaParameter* param = NULL;
			ValaList* _tmp7_;
			gint _tmp8_;
			gpointer _tmp9_;
			ValaParameter* _tmp10_;
			_tmp4_ = _param_index;
			_param_index = _tmp4_ + 1;
			_tmp5_ = _param_index;
			_tmp6_ = _param_size;
			if (!(_tmp5_ < _tmp6_)) {
				break;
			}
			_tmp7_ = _param_list;
			_tmp8_ = _param_index;
			_tmp9_ = vala_list_get (_tmp7_, _tmp8_);
			param = (ValaParameter*) _tmp9_;
			_tmp10_ = param;
			vala_code_node_accept ((ValaCodeNode*) _tmp10_, visitor);
			_vala_code_node_unref0 (param);
		}
		_vala_iterable_unref0 (_param_list);
	}
	{
		ValaList* _error_type_list = NULL;
		ValaList* _tmp11_;
		gint _error_type_size = 0;
		ValaList* _tmp12_;
		gint _tmp13_;
		gint _tmp14_;
		gint _error_type_index = 0;
		_tmp11_ = vala_code_node_get_error_types ((ValaCodeNode*) self);
		_error_type_list = _tmp11_;
		_tmp12_ = _error_type_list;
		_tmp13_ = vala_collection_get_size ((ValaCollection*) _tmp12_);
		_tmp14_ = _tmp13_;
		_error_type_size = _tmp14_;
		_error_type_index = -1;
		while (TRUE) {
			gint _tmp15_;
			gint _tmp16_;
			gint _tmp17_;
			ValaDataType* error_type = NULL;
			ValaList* _tmp18_;
			gint _tmp19_;
			gpointer _tmp20_;
			ValaDataType* _tmp21_;
			_tmp15_ = _error_type_index;
			_error_type_index = _tmp15_ + 1;
			_tmp16_ = _error_type_index;
			_tmp17_ = _error_type_size;
			if (!(_tmp16_ < _tmp17_)) {
				break;
			}
			_tmp18_ = _error_type_list;
			_tmp19_ = _error_type_index;
			_tmp20_ = vala_list_get (_tmp18_, _tmp19_);
			error_type = (ValaDataType*) _tmp20_;
			_tmp21_ = error_type;
			vala_code_node_accept ((ValaCodeNode*) _tmp21_, visitor);
			_vala_code_node_unref0 (error_type);
		}
		_vala_iterable_unref0 (_error_type_list);
	}
	{
		ValaList* _precondition_list = NULL;
		ValaList* _tmp22_;
		gint _precondition_size = 0;
		ValaList* _tmp23_;
		gint _tmp24_;
		gint _tmp25_;
		gint _precondition_index = 0;
		_tmp22_ = vala_method_get_preconditions ((ValaMethod*) self);
		_precondition_list = _tmp22_;
		_tmp23_ = _precondition_list;
		_tmp24_ = vala_collection_get_size ((ValaCollection*) _tmp23_);
		_tmp25_ = _tmp24_;
		_precondition_size = _tmp25_;
		_precondition_index = -1;
		while (TRUE) {
			gint _tmp26_;
			gint _tmp27_;
			gint _tmp28_;
			ValaExpression* precondition = NULL;
			ValaList* _tmp29_;
			gint _tmp30_;
			gpointer _tmp31_;
			ValaExpression* _tmp32_;
			_tmp26_ = _precondition_index;
			_precondition_index = _tmp26_ + 1;
			_tmp27_ = _precondition_index;
			_tmp28_ = _precondition_size;
			if (!(_tmp27_ < _tmp28_)) {
				break;
			}
			_tmp29_ = _precondition_list;
			_tmp30_ = _precondition_index;
			_tmp31_ = vala_list_get (_tmp29_, _tmp30_);
			precondition = (ValaExpression*) _tmp31_;
			_tmp32_ = precondition;
			vala_code_node_accept ((ValaCodeNode*) _tmp32_, visitor);
			_vala_code_node_unref0 (precondition);
		}
		_vala_iterable_unref0 (_precondition_list);
	}
	{
		ValaList* _postcondition_list = NULL;
		ValaList* _tmp33_;
		gint _postcondition_size = 0;
		ValaList* _tmp34_;
		gint _tmp35_;
		gint _tmp36_;
		gint _postcondition_index = 0;
		_tmp33_ = vala_method_get_postconditions ((ValaMethod*) self);
		_postcondition_list = _tmp33_;
		_tmp34_ = _postcondition_list;
		_tmp35_ = vala_collection_get_size ((ValaCollection*) _tmp34_);
		_tmp36_ = _tmp35_;
		_postcondition_size = _tmp36_;
		_postcondition_index = -1;
		while (TRUE) {
			gint _tmp37_;
			gint _tmp38_;
			gint _tmp39_;
			ValaExpression* postcondition = NULL;
			ValaList* _tmp40_;
			gint _tmp41_;
			gpointer _tmp42_;
			ValaExpression* _tmp43_;
			_tmp37_ = _postcondition_index;
			_postcondition_index = _tmp37_ + 1;
			_tmp38_ = _postcondition_index;
			_tmp39_ = _postcondition_size;
			if (!(_tmp38_ < _tmp39_)) {
				break;
			}
			_tmp40_ = _postcondition_list;
			_tmp41_ = _postcondition_index;
			_tmp42_ = vala_list_get (_tmp40_, _tmp41_);
			postcondition = (ValaExpression*) _tmp42_;
			_tmp43_ = postcondition;
			vala_code_node_accept ((ValaCodeNode*) _tmp43_, visitor);
			_vala_code_node_unref0 (postcondition);
		}
		_vala_iterable_unref0 (_postcondition_list);
	}
	_tmp44_ = vala_subroutine_get_body ((ValaSubroutine*) self);
	_tmp45_ = _tmp44_;
	if (_tmp45_ != NULL) {
		ValaBlock* _tmp46_;
		ValaBlock* _tmp47_;
		_tmp46_ = vala_subroutine_get_body ((ValaSubroutine*) self);
		_tmp47_ = _tmp46_;
		vala_code_node_accept ((ValaCodeNode*) _tmp47_, visitor);
	}
}


static gpointer
_vala_source_file_ref0 (gpointer self)
{
	return self ? vala_source_file_ref (self) : NULL;
}


static gpointer
_vala_code_node_ref0 (gpointer self)
{
	return self ? vala_code_node_ref (self) : NULL;
}


static gboolean
vala_creation_method_real_check (ValaCodeNode* base,
                                 ValaCodeContext* context)
{
	ValaCreationMethod * self;
	gboolean result = FALSE;
	gboolean _tmp0_;
	gboolean _tmp1_;
	gboolean _tmp4_ = FALSE;
	const gchar* _tmp5_;
	ValaSourceFile* old_source_file = NULL;
	ValaSemanticAnalyzer* _tmp22_;
	ValaSemanticAnalyzer* _tmp23_;
	ValaSourceFile* _tmp24_;
	ValaSourceFile* _tmp25_;
	ValaSourceFile* _tmp26_;
	ValaSymbol* old_symbol = NULL;
	ValaSemanticAnalyzer* _tmp27_;
	ValaSemanticAnalyzer* _tmp28_;
	ValaSymbol* _tmp29_;
	ValaSymbol* _tmp30_;
	ValaSymbol* _tmp31_;
	ValaSourceReference* _tmp32_;
	ValaSourceReference* _tmp33_;
	ValaSemanticAnalyzer* _tmp40_;
	ValaSemanticAnalyzer* _tmp41_;
	gint i = 0;
	ValaBlock* _tmp98_;
	ValaBlock* _tmp99_;
	ValaSemanticAnalyzer* _tmp224_;
	ValaSemanticAnalyzer* _tmp225_;
	ValaSourceFile* _tmp226_;
	ValaSemanticAnalyzer* _tmp227_;
	ValaSemanticAnalyzer* _tmp228_;
	ValaSymbol* _tmp229_;
	gboolean _tmp230_ = FALSE;
	gboolean _tmp231_ = FALSE;
	gboolean _tmp232_;
	gboolean _tmp233_;
	ValaBlock* _tmp244_;
	ValaBlock* _tmp245_;
	gboolean _tmp283_;
	gboolean _tmp284_;
	self = (ValaCreationMethod*) base;
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
	_tmp5_ = self->priv->_class_name;
	if (_tmp5_ != NULL) {
		const gchar* _tmp6_;
		ValaSymbol* _tmp7_;
		ValaSymbol* _tmp8_;
		const gchar* _tmp9_;
		const gchar* _tmp10_;
		_tmp6_ = self->priv->_class_name;
		_tmp7_ = vala_symbol_get_parent_symbol ((ValaSymbol*) self);
		_tmp8_ = _tmp7_;
		_tmp9_ = vala_symbol_get_name (_tmp8_);
		_tmp10_ = _tmp9_;
		_tmp4_ = g_strcmp0 (_tmp6_, _tmp10_) != 0;
	} else {
		_tmp4_ = FALSE;
	}
	if (_tmp4_) {
		ValaSourceReference* _tmp11_;
		ValaSourceReference* _tmp12_;
		ValaSemanticAnalyzer* _tmp13_;
		ValaSemanticAnalyzer* _tmp14_;
		ValaSymbol* _tmp15_;
		ValaSymbol* _tmp16_;
		gchar* _tmp17_;
		gchar* _tmp18_;
		const gchar* _tmp19_;
		gchar* _tmp20_;
		gchar* _tmp21_;
		_tmp11_ = vala_code_node_get_source_reference ((ValaCodeNode*) self);
		_tmp12_ = _tmp11_;
		_tmp13_ = vala_code_context_get_analyzer (context);
		_tmp14_ = _tmp13_;
		_tmp15_ = vala_semantic_analyzer_get_current_symbol (_tmp14_);
		_tmp16_ = _tmp15_;
		_tmp17_ = vala_symbol_get_full_name (_tmp16_);
		_tmp18_ = _tmp17_;
		_tmp19_ = self->priv->_class_name;
		_tmp20_ = g_strdup_printf ("missing return type in method `%s.%s´", _tmp18_, _tmp19_);
		_tmp21_ = _tmp20_;
		vala_report_error (_tmp12_, _tmp21_);
		_g_free0 (_tmp21_);
		_g_free0 (_tmp18_);
		vala_code_node_set_error ((ValaCodeNode*) self, TRUE);
		result = FALSE;
		return result;
	}
	_tmp22_ = vala_code_context_get_analyzer (context);
	_tmp23_ = _tmp22_;
	_tmp24_ = vala_semantic_analyzer_get_current_source_file (_tmp23_);
	_tmp25_ = _tmp24_;
	_tmp26_ = _vala_source_file_ref0 (_tmp25_);
	old_source_file = _tmp26_;
	_tmp27_ = vala_code_context_get_analyzer (context);
	_tmp28_ = _tmp27_;
	_tmp29_ = vala_semantic_analyzer_get_current_symbol (_tmp28_);
	_tmp30_ = _tmp29_;
	_tmp31_ = _vala_code_node_ref0 (_tmp30_);
	old_symbol = _tmp31_;
	_tmp32_ = vala_code_node_get_source_reference ((ValaCodeNode*) self);
	_tmp33_ = _tmp32_;
	if (_tmp33_ != NULL) {
		ValaSemanticAnalyzer* _tmp34_;
		ValaSemanticAnalyzer* _tmp35_;
		ValaSourceReference* _tmp36_;
		ValaSourceReference* _tmp37_;
		ValaSourceFile* _tmp38_;
		ValaSourceFile* _tmp39_;
		_tmp34_ = vala_code_context_get_analyzer (context);
		_tmp35_ = _tmp34_;
		_tmp36_ = vala_code_node_get_source_reference ((ValaCodeNode*) self);
		_tmp37_ = _tmp36_;
		_tmp38_ = vala_source_reference_get_file (_tmp37_);
		_tmp39_ = _tmp38_;
		vala_semantic_analyzer_set_current_source_file (_tmp35_, _tmp39_);
	}
	_tmp40_ = vala_code_context_get_analyzer (context);
	_tmp41_ = _tmp40_;
	vala_semantic_analyzer_set_current_symbol (_tmp41_, (ValaSymbol*) self);
	i = 0;
	{
		ValaList* _param_list = NULL;
		ValaList* _tmp42_;
		gint _param_size = 0;
		ValaList* _tmp43_;
		gint _tmp44_;
		gint _tmp45_;
		gint _param_index = 0;
		_tmp42_ = vala_callable_get_parameters ((ValaCallable*) self);
		_param_list = _tmp42_;
		_tmp43_ = _param_list;
		_tmp44_ = vala_collection_get_size ((ValaCollection*) _tmp43_);
		_tmp45_ = _tmp44_;
		_param_size = _tmp45_;
		_param_index = -1;
		while (TRUE) {
			gint _tmp46_;
			gint _tmp47_;
			gint _tmp48_;
			ValaParameter* param = NULL;
			ValaList* _tmp49_;
			gint _tmp50_;
			gpointer _tmp51_;
			ValaParameter* _tmp52_;
			gboolean _tmp53_ = FALSE;
			gboolean _tmp54_ = FALSE;
			gint _tmp55_;
			gint _tmp64_;
			_tmp46_ = _param_index;
			_param_index = _tmp46_ + 1;
			_tmp47_ = _param_index;
			_tmp48_ = _param_size;
			if (!(_tmp47_ < _tmp48_)) {
				break;
			}
			_tmp49_ = _param_list;
			_tmp50_ = _param_index;
			_tmp51_ = vala_list_get (_tmp49_, _tmp50_);
			param = (ValaParameter*) _tmp51_;
			_tmp52_ = param;
			vala_code_node_check ((ValaCodeNode*) _tmp52_, context);
			_tmp55_ = i;
			if (_tmp55_ == 0) {
				ValaParameter* _tmp56_;
				gboolean _tmp57_;
				gboolean _tmp58_;
				_tmp56_ = param;
				_tmp57_ = vala_parameter_get_ellipsis (_tmp56_);
				_tmp58_ = _tmp57_;
				_tmp54_ = _tmp58_;
			} else {
				_tmp54_ = FALSE;
			}
			if (_tmp54_) {
				ValaBlock* _tmp59_;
				ValaBlock* _tmp60_;
				_tmp59_ = vala_subroutine_get_body ((ValaSubroutine*) self);
				_tmp60_ = _tmp59_;
				_tmp53_ = _tmp60_ != NULL;
			} else {
				_tmp53_ = FALSE;
			}
			if (_tmp53_) {
				ValaParameter* _tmp61_;
				ValaSourceReference* _tmp62_;
				ValaSourceReference* _tmp63_;
				vala_code_node_set_error ((ValaCodeNode*) self, TRUE);
				_tmp61_ = param;
				_tmp62_ = vala_code_node_get_source_reference ((ValaCodeNode*) _tmp61_);
				_tmp63_ = _tmp62_;
				vala_report_error (_tmp63_, "Named parameter required before `...'");
			}
			_tmp64_ = i;
			i = _tmp64_ + 1;
			_vala_code_node_unref0 (param);
		}
		_vala_iterable_unref0 (_param_list);
	}
	{
		ValaList* _error_type_list = NULL;
		ValaList* _tmp65_;
		gint _error_type_size = 0;
		ValaList* _tmp66_;
		gint _tmp67_;
		gint _tmp68_;
		gint _error_type_index = 0;
		_tmp65_ = vala_code_node_get_error_types ((ValaCodeNode*) self);
		_error_type_list = _tmp65_;
		_tmp66_ = _error_type_list;
		_tmp67_ = vala_collection_get_size ((ValaCollection*) _tmp66_);
		_tmp68_ = _tmp67_;
		_error_type_size = _tmp68_;
		_error_type_index = -1;
		while (TRUE) {
			gint _tmp69_;
			gint _tmp70_;
			gint _tmp71_;
			ValaDataType* error_type = NULL;
			ValaList* _tmp72_;
			gint _tmp73_;
			gpointer _tmp74_;
			ValaDataType* _tmp75_;
			_tmp69_ = _error_type_index;
			_error_type_index = _tmp69_ + 1;
			_tmp70_ = _error_type_index;
			_tmp71_ = _error_type_size;
			if (!(_tmp70_ < _tmp71_)) {
				break;
			}
			_tmp72_ = _error_type_list;
			_tmp73_ = _error_type_index;
			_tmp74_ = vala_list_get (_tmp72_, _tmp73_);
			error_type = (ValaDataType*) _tmp74_;
			_tmp75_ = error_type;
			vala_code_node_check ((ValaCodeNode*) _tmp75_, context);
			_vala_code_node_unref0 (error_type);
		}
		_vala_iterable_unref0 (_error_type_list);
	}
	{
		ValaList* _precondition_list = NULL;
		ValaList* _tmp76_;
		gint _precondition_size = 0;
		ValaList* _tmp77_;
		gint _tmp78_;
		gint _tmp79_;
		gint _precondition_index = 0;
		_tmp76_ = vala_method_get_preconditions ((ValaMethod*) self);
		_precondition_list = _tmp76_;
		_tmp77_ = _precondition_list;
		_tmp78_ = vala_collection_get_size ((ValaCollection*) _tmp77_);
		_tmp79_ = _tmp78_;
		_precondition_size = _tmp79_;
		_precondition_index = -1;
		while (TRUE) {
			gint _tmp80_;
			gint _tmp81_;
			gint _tmp82_;
			ValaExpression* precondition = NULL;
			ValaList* _tmp83_;
			gint _tmp84_;
			gpointer _tmp85_;
			ValaExpression* _tmp86_;
			_tmp80_ = _precondition_index;
			_precondition_index = _tmp80_ + 1;
			_tmp81_ = _precondition_index;
			_tmp82_ = _precondition_size;
			if (!(_tmp81_ < _tmp82_)) {
				break;
			}
			_tmp83_ = _precondition_list;
			_tmp84_ = _precondition_index;
			_tmp85_ = vala_list_get (_tmp83_, _tmp84_);
			precondition = (ValaExpression*) _tmp85_;
			_tmp86_ = precondition;
			vala_code_node_check ((ValaCodeNode*) _tmp86_, context);
			_vala_code_node_unref0 (precondition);
		}
		_vala_iterable_unref0 (_precondition_list);
	}
	{
		ValaList* _postcondition_list = NULL;
		ValaList* _tmp87_;
		gint _postcondition_size = 0;
		ValaList* _tmp88_;
		gint _tmp89_;
		gint _tmp90_;
		gint _postcondition_index = 0;
		_tmp87_ = vala_method_get_postconditions ((ValaMethod*) self);
		_postcondition_list = _tmp87_;
		_tmp88_ = _postcondition_list;
		_tmp89_ = vala_collection_get_size ((ValaCollection*) _tmp88_);
		_tmp90_ = _tmp89_;
		_postcondition_size = _tmp90_;
		_postcondition_index = -1;
		while (TRUE) {
			gint _tmp91_;
			gint _tmp92_;
			gint _tmp93_;
			ValaExpression* postcondition = NULL;
			ValaList* _tmp94_;
			gint _tmp95_;
			gpointer _tmp96_;
			ValaExpression* _tmp97_;
			_tmp91_ = _postcondition_index;
			_postcondition_index = _tmp91_ + 1;
			_tmp92_ = _postcondition_index;
			_tmp93_ = _postcondition_size;
			if (!(_tmp92_ < _tmp93_)) {
				break;
			}
			_tmp94_ = _postcondition_list;
			_tmp95_ = _postcondition_index;
			_tmp96_ = vala_list_get (_tmp94_, _tmp95_);
			postcondition = (ValaExpression*) _tmp96_;
			_tmp97_ = postcondition;
			vala_code_node_check ((ValaCodeNode*) _tmp97_, context);
			_vala_code_node_unref0 (postcondition);
		}
		_vala_iterable_unref0 (_postcondition_list);
	}
	_tmp98_ = vala_subroutine_get_body ((ValaSubroutine*) self);
	_tmp99_ = _tmp98_;
	if (_tmp99_ != NULL) {
		ValaBlock* _tmp100_;
		ValaBlock* _tmp101_;
		ValaClass* cl = NULL;
		ValaSymbol* _tmp102_;
		ValaSymbol* _tmp103_;
		ValaClass* _tmp104_;
		gboolean _tmp105_ = FALSE;
		gboolean _tmp106_ = FALSE;
		gboolean _tmp107_;
		_tmp100_ = vala_subroutine_get_body ((ValaSubroutine*) self);
		_tmp101_ = _tmp100_;
		vala_code_node_check ((ValaCodeNode*) _tmp101_, context);
		_tmp102_ = vala_symbol_get_parent_symbol ((ValaSymbol*) self);
		_tmp103_ = _tmp102_;
		_tmp104_ = _vala_code_node_ref0 (G_TYPE_CHECK_INSTANCE_TYPE (_tmp103_, VALA_TYPE_CLASS) ? ((ValaClass*) _tmp103_) : NULL);
		cl = _tmp104_;
		_tmp107_ = self->priv->_chain_up;
		if (!_tmp107_) {
			ValaClass* _tmp108_;
			_tmp108_ = cl;
			_tmp106_ = _tmp108_ != NULL;
		} else {
			_tmp106_ = FALSE;
		}
		if (_tmp106_) {
			ValaClass* _tmp109_;
			ValaClass* _tmp110_;
			ValaClass* _tmp111_;
			_tmp109_ = cl;
			_tmp110_ = vala_class_get_base_class (_tmp109_);
			_tmp111_ = _tmp110_;
			_tmp105_ = _tmp111_ != NULL;
		} else {
			_tmp105_ = FALSE;
		}
		if (_tmp105_) {
			gboolean _tmp112_ = FALSE;
			gboolean _tmp113_ = FALSE;
			ValaProfile _tmp114_;
			ValaProfile _tmp115_;
			_tmp114_ = vala_code_context_get_profile (context);
			_tmp115_ = _tmp114_;
			if (_tmp115_ == VALA_PROFILE_GOBJECT) {
				ValaClass* _tmp116_;
				ValaClass* _tmp117_;
				ValaClass* _tmp118_;
				ValaCreationMethod* _tmp119_;
				ValaCreationMethod* _tmp120_;
				_tmp116_ = cl;
				_tmp117_ = vala_class_get_base_class (_tmp116_);
				_tmp118_ = _tmp117_;
				_tmp119_ = vala_class_get_default_construction_method (_tmp118_);
				_tmp120_ = _tmp119_;
				_tmp113_ = _tmp120_ != NULL;
			} else {
				_tmp113_ = FALSE;
			}
			if (_tmp113_) {
				ValaClass* _tmp121_;
				ValaClass* _tmp122_;
				ValaClass* _tmp123_;
				ValaCreationMethod* _tmp124_;
				ValaCreationMethod* _tmp125_;
				gboolean _tmp126_;
				gboolean _tmp127_;
				_tmp121_ = cl;
				_tmp122_ = vala_class_get_base_class (_tmp121_);
				_tmp123_ = _tmp122_;
				_tmp124_ = vala_class_get_default_construction_method (_tmp123_);
				_tmp125_ = _tmp124_;
				_tmp126_ = vala_method_get_has_construct_function ((ValaMethod*) _tmp125_);
				_tmp127_ = _tmp126_;
				_tmp112_ = !_tmp127_;
			} else {
				_tmp112_ = FALSE;
			}
			if (_tmp112_) {
				ValaBlock* old_insert_block = NULL;
				ValaSemanticAnalyzer* _tmp128_;
				ValaSemanticAnalyzer* _tmp129_;
				ValaBlock* _tmp130_;
				ValaBlock* _tmp131_;
				ValaSemanticAnalyzer* _tmp132_;
				ValaSemanticAnalyzer* _tmp133_;
				ValaBlock* _tmp134_;
				ValaBlock* _tmp135_;
				ValaSemanticAnalyzer* _tmp136_;
				ValaSemanticAnalyzer* _tmp137_;
				ValaBlock* _tmp138_;
				ValaBlock* _tmp139_;
				ValaBlock* _tmp140_;
				ValaExpressionStatement* stmt = NULL;
				ValaSourceReference* _tmp141_;
				ValaSourceReference* _tmp142_;
				ValaMemberAccess* _tmp143_;
				ValaMemberAccess* _tmp144_;
				ValaSourceReference* _tmp145_;
				ValaSourceReference* _tmp146_;
				ValaMemberAccess* _tmp147_;
				ValaMemberAccess* _tmp148_;
				ValaSourceReference* _tmp149_;
				ValaSourceReference* _tmp150_;
				ValaMethodCall* _tmp151_;
				ValaMethodCall* _tmp152_;
				ValaSourceReference* _tmp153_;
				ValaSourceReference* _tmp154_;
				ValaExpressionStatement* _tmp155_;
				ValaExpressionStatement* _tmp156_;
				ValaBlock* _tmp157_;
				ValaBlock* _tmp158_;
				ValaExpressionStatement* _tmp159_;
				ValaExpressionStatement* _tmp160_;
				ValaSemanticAnalyzer* _tmp161_;
				ValaSemanticAnalyzer* _tmp162_;
				ValaSemanticAnalyzer* _tmp163_;
				ValaSemanticAnalyzer* _tmp164_;
				ValaBlock* _tmp165_;
				ValaBlock* _tmp166_;
				_tmp128_ = vala_code_context_get_analyzer (context);
				_tmp129_ = _tmp128_;
				_tmp130_ = _tmp129_->insert_block;
				_tmp131_ = _vala_code_node_ref0 (_tmp130_);
				old_insert_block = _tmp131_;
				_tmp132_ = vala_code_context_get_analyzer (context);
				_tmp133_ = _tmp132_;
				_tmp134_ = vala_subroutine_get_body ((ValaSubroutine*) self);
				_tmp135_ = _tmp134_;
				vala_semantic_analyzer_set_current_symbol (_tmp133_, (ValaSymbol*) _tmp135_);
				_tmp136_ = vala_code_context_get_analyzer (context);
				_tmp137_ = _tmp136_;
				_tmp138_ = vala_subroutine_get_body ((ValaSubroutine*) self);
				_tmp139_ = _tmp138_;
				_tmp140_ = _vala_code_node_ref0 (_tmp139_);
				_vala_code_node_unref0 (_tmp137_->insert_block);
				_tmp137_->insert_block = _tmp140_;
				_tmp141_ = vala_code_node_get_source_reference ((ValaCodeNode*) self);
				_tmp142_ = _tmp141_;
				_tmp143_ = vala_member_access_new_simple ("GLib", _tmp142_);
				_tmp144_ = _tmp143_;
				_tmp145_ = vala_code_node_get_source_reference ((ValaCodeNode*) self);
				_tmp146_ = _tmp145_;
				_tmp147_ = vala_member_access_new ((ValaExpression*) _tmp144_, "Object", _tmp146_);
				_tmp148_ = _tmp147_;
				_tmp149_ = vala_code_node_get_source_reference ((ValaCodeNode*) self);
				_tmp150_ = _tmp149_;
				_tmp151_ = vala_method_call_new ((ValaExpression*) _tmp148_, _tmp150_);
				_tmp152_ = _tmp151_;
				_tmp153_ = vala_code_node_get_source_reference ((ValaCodeNode*) self);
				_tmp154_ = _tmp153_;
				_tmp155_ = vala_expression_statement_new ((ValaExpression*) _tmp152_, _tmp154_);
				_tmp156_ = _tmp155_;
				_vala_code_node_unref0 (_tmp152_);
				_vala_code_node_unref0 (_tmp148_);
				_vala_code_node_unref0 (_tmp144_);
				stmt = _tmp156_;
				_tmp157_ = vala_subroutine_get_body ((ValaSubroutine*) self);
				_tmp158_ = _tmp157_;
				_tmp159_ = stmt;
				vala_block_insert_statement (_tmp158_, 0, (ValaStatement*) _tmp159_);
				_tmp160_ = stmt;
				vala_code_node_check ((ValaCodeNode*) _tmp160_, context);
				_tmp161_ = vala_code_context_get_analyzer (context);
				_tmp162_ = _tmp161_;
				vala_semantic_analyzer_set_current_symbol (_tmp162_, (ValaSymbol*) self);
				_tmp163_ = vala_code_context_get_analyzer (context);
				_tmp164_ = _tmp163_;
				_tmp165_ = old_insert_block;
				_tmp166_ = _vala_code_node_ref0 (_tmp165_);
				_vala_code_node_unref0 (_tmp164_->insert_block);
				_tmp164_->insert_block = _tmp166_;
				_vala_code_node_unref0 (stmt);
				_vala_code_node_unref0 (old_insert_block);
			} else {
				gboolean _tmp167_ = FALSE;
				ValaClass* _tmp168_;
				ValaClass* _tmp169_;
				ValaClass* _tmp170_;
				ValaCreationMethod* _tmp171_;
				ValaCreationMethod* _tmp172_;
				_tmp168_ = cl;
				_tmp169_ = vala_class_get_base_class (_tmp168_);
				_tmp170_ = _tmp169_;
				_tmp171_ = vala_class_get_default_construction_method (_tmp170_);
				_tmp172_ = _tmp171_;
				if (_tmp172_ == NULL) {
					_tmp167_ = TRUE;
				} else {
					ValaClass* _tmp173_;
					ValaClass* _tmp174_;
					ValaClass* _tmp175_;
					ValaCreationMethod* _tmp176_;
					ValaCreationMethod* _tmp177_;
					ValaSymbolAccessibility _tmp178_;
					ValaSymbolAccessibility _tmp179_;
					_tmp173_ = cl;
					_tmp174_ = vala_class_get_base_class (_tmp173_);
					_tmp175_ = _tmp174_;
					_tmp176_ = vala_class_get_default_construction_method (_tmp175_);
					_tmp177_ = _tmp176_;
					_tmp178_ = vala_symbol_get_access ((ValaSymbol*) _tmp177_);
					_tmp179_ = _tmp178_;
					_tmp167_ = _tmp179_ == VALA_SYMBOL_ACCESSIBILITY_PRIVATE;
				}
				if (_tmp167_) {
					ValaSourceReference* _tmp180_;
					ValaSourceReference* _tmp181_;
					_tmp180_ = vala_code_node_get_source_reference ((ValaCodeNode*) self);
					_tmp181_ = _tmp180_;
					vala_report_error (_tmp181_, "unable to chain up to private base constructor");
				} else {
					ValaClass* _tmp182_;
					ValaClass* _tmp183_;
					ValaClass* _tmp184_;
					ValaCreationMethod* _tmp185_;
					ValaCreationMethod* _tmp186_;
					_tmp182_ = cl;
					_tmp183_ = vala_class_get_base_class (_tmp182_);
					_tmp184_ = _tmp183_;
					_tmp185_ = vala_class_get_default_construction_method (_tmp184_);
					_tmp186_ = _tmp185_;
					if (vala_method_get_required_arguments ((ValaMethod*) _tmp186_) > 0) {
						ValaSourceReference* _tmp187_;
						ValaSourceReference* _tmp188_;
						_tmp187_ = vala_code_node_get_source_reference ((ValaCodeNode*) self);
						_tmp188_ = _tmp187_;
						vala_report_error (_tmp188_, "unable to chain up to base constructor requiring arguments");
					} else {
						ValaBlock* old_insert_block = NULL;
						ValaSemanticAnalyzer* _tmp189_;
						ValaSemanticAnalyzer* _tmp190_;
						ValaBlock* _tmp191_;
						ValaBlock* _tmp192_;
						ValaSemanticAnalyzer* _tmp193_;
						ValaSemanticAnalyzer* _tmp194_;
						ValaBlock* _tmp195_;
						ValaBlock* _tmp196_;
						ValaSemanticAnalyzer* _tmp197_;
						ValaSemanticAnalyzer* _tmp198_;
						ValaBlock* _tmp199_;
						ValaBlock* _tmp200_;
						ValaBlock* _tmp201_;
						ValaExpressionStatement* stmt = NULL;
						ValaSourceReference* _tmp202_;
						ValaSourceReference* _tmp203_;
						ValaBaseAccess* _tmp204_;
						ValaBaseAccess* _tmp205_;
						ValaSourceReference* _tmp206_;
						ValaSourceReference* _tmp207_;
						ValaMethodCall* _tmp208_;
						ValaMethodCall* _tmp209_;
						ValaSourceReference* _tmp210_;
						ValaSourceReference* _tmp211_;
						ValaExpressionStatement* _tmp212_;
						ValaExpressionStatement* _tmp213_;
						ValaBlock* _tmp214_;
						ValaBlock* _tmp215_;
						ValaExpressionStatement* _tmp216_;
						ValaExpressionStatement* _tmp217_;
						ValaSemanticAnalyzer* _tmp218_;
						ValaSemanticAnalyzer* _tmp219_;
						ValaSemanticAnalyzer* _tmp220_;
						ValaSemanticAnalyzer* _tmp221_;
						ValaBlock* _tmp222_;
						ValaBlock* _tmp223_;
						_tmp189_ = vala_code_context_get_analyzer (context);
						_tmp190_ = _tmp189_;
						_tmp191_ = _tmp190_->insert_block;
						_tmp192_ = _vala_code_node_ref0 (_tmp191_);
						old_insert_block = _tmp192_;
						_tmp193_ = vala_code_context_get_analyzer (context);
						_tmp194_ = _tmp193_;
						_tmp195_ = vala_subroutine_get_body ((ValaSubroutine*) self);
						_tmp196_ = _tmp195_;
						vala_semantic_analyzer_set_current_symbol (_tmp194_, (ValaSymbol*) _tmp196_);
						_tmp197_ = vala_code_context_get_analyzer (context);
						_tmp198_ = _tmp197_;
						_tmp199_ = vala_subroutine_get_body ((ValaSubroutine*) self);
						_tmp200_ = _tmp199_;
						_tmp201_ = _vala_code_node_ref0 (_tmp200_);
						_vala_code_node_unref0 (_tmp198_->insert_block);
						_tmp198_->insert_block = _tmp201_;
						_tmp202_ = vala_code_node_get_source_reference ((ValaCodeNode*) self);
						_tmp203_ = _tmp202_;
						_tmp204_ = vala_base_access_new (_tmp203_);
						_tmp205_ = _tmp204_;
						_tmp206_ = vala_code_node_get_source_reference ((ValaCodeNode*) self);
						_tmp207_ = _tmp206_;
						_tmp208_ = vala_method_call_new ((ValaExpression*) _tmp205_, _tmp207_);
						_tmp209_ = _tmp208_;
						_tmp210_ = vala_code_node_get_source_reference ((ValaCodeNode*) self);
						_tmp211_ = _tmp210_;
						_tmp212_ = vala_expression_statement_new ((ValaExpression*) _tmp209_, _tmp211_);
						_tmp213_ = _tmp212_;
						_vala_code_node_unref0 (_tmp209_);
						_vala_code_node_unref0 (_tmp205_);
						stmt = _tmp213_;
						_tmp214_ = vala_subroutine_get_body ((ValaSubroutine*) self);
						_tmp215_ = _tmp214_;
						_tmp216_ = stmt;
						vala_block_insert_statement (_tmp215_, 0, (ValaStatement*) _tmp216_);
						_tmp217_ = stmt;
						vala_code_node_check ((ValaCodeNode*) _tmp217_, context);
						_tmp218_ = vala_code_context_get_analyzer (context);
						_tmp219_ = _tmp218_;
						vala_semantic_analyzer_set_current_symbol (_tmp219_, (ValaSymbol*) self);
						_tmp220_ = vala_code_context_get_analyzer (context);
						_tmp221_ = _tmp220_;
						_tmp222_ = old_insert_block;
						_tmp223_ = _vala_code_node_ref0 (_tmp222_);
						_vala_code_node_unref0 (_tmp221_->insert_block);
						_tmp221_->insert_block = _tmp223_;
						_vala_code_node_unref0 (stmt);
						_vala_code_node_unref0 (old_insert_block);
					}
				}
			}
		}
		_vala_code_node_unref0 (cl);
	}
	_tmp224_ = vala_code_context_get_analyzer (context);
	_tmp225_ = _tmp224_;
	_tmp226_ = old_source_file;
	vala_semantic_analyzer_set_current_source_file (_tmp225_, _tmp226_);
	_tmp227_ = vala_code_context_get_analyzer (context);
	_tmp228_ = _tmp227_;
	_tmp229_ = old_symbol;
	vala_semantic_analyzer_set_current_symbol (_tmp228_, _tmp229_);
	_tmp232_ = vala_method_get_is_abstract ((ValaMethod*) self);
	_tmp233_ = _tmp232_;
	if (_tmp233_) {
		_tmp231_ = TRUE;
	} else {
		gboolean _tmp234_;
		gboolean _tmp235_;
		_tmp234_ = vala_method_get_is_virtual ((ValaMethod*) self);
		_tmp235_ = _tmp234_;
		_tmp231_ = _tmp235_;
	}
	if (_tmp231_) {
		_tmp230_ = TRUE;
	} else {
		gboolean _tmp236_;
		gboolean _tmp237_;
		_tmp236_ = vala_method_get_overrides ((ValaMethod*) self);
		_tmp237_ = _tmp236_;
		_tmp230_ = _tmp237_;
	}
	if (_tmp230_) {
		ValaSourceReference* _tmp238_;
		ValaSourceReference* _tmp239_;
		gchar* _tmp240_;
		gchar* _tmp241_;
		gchar* _tmp242_;
		gchar* _tmp243_;
		_tmp238_ = vala_code_node_get_source_reference ((ValaCodeNode*) self);
		_tmp239_ = _tmp238_;
		_tmp240_ = vala_symbol_get_full_name ((ValaSymbol*) self);
		_tmp241_ = _tmp240_;
		_tmp242_ = g_strdup_printf ("The creation method `%s' cannot be marked as override, virtual, or abs" \
"tract", _tmp241_);
		_tmp243_ = _tmp242_;
		vala_report_error (_tmp239_, _tmp243_);
		_g_free0 (_tmp243_);
		_g_free0 (_tmp241_);
		result = FALSE;
		_vala_code_node_unref0 (old_symbol);
		_vala_source_file_unref0 (old_source_file);
		return result;
	}
	_tmp244_ = vala_subroutine_get_body ((ValaSubroutine*) self);
	_tmp245_ = _tmp244_;
	if (_tmp245_ != NULL) {
		{
			ValaList* _body_error_type_list = NULL;
			ValaBlock* _tmp246_;
			ValaBlock* _tmp247_;
			ValaList* _tmp248_;
			gint _body_error_type_size = 0;
			ValaList* _tmp249_;
			gint _tmp250_;
			gint _tmp251_;
			gint _body_error_type_index = 0;
			_tmp246_ = vala_subroutine_get_body ((ValaSubroutine*) self);
			_tmp247_ = _tmp246_;
			_tmp248_ = vala_code_node_get_error_types ((ValaCodeNode*) _tmp247_);
			_body_error_type_list = _tmp248_;
			_tmp249_ = _body_error_type_list;
			_tmp250_ = vala_collection_get_size ((ValaCollection*) _tmp249_);
			_tmp251_ = _tmp250_;
			_body_error_type_size = _tmp251_;
			_body_error_type_index = -1;
			while (TRUE) {
				gint _tmp252_;
				gint _tmp253_;
				gint _tmp254_;
				ValaDataType* body_error_type = NULL;
				ValaList* _tmp255_;
				gint _tmp256_;
				gpointer _tmp257_;
				gboolean can_propagate_error = FALSE;
				gboolean _tmp270_ = FALSE;
				gboolean _tmp271_;
				_tmp252_ = _body_error_type_index;
				_body_error_type_index = _tmp252_ + 1;
				_tmp253_ = _body_error_type_index;
				_tmp254_ = _body_error_type_size;
				if (!(_tmp253_ < _tmp254_)) {
					break;
				}
				_tmp255_ = _body_error_type_list;
				_tmp256_ = _body_error_type_index;
				_tmp257_ = vala_list_get (_tmp255_, _tmp256_);
				body_error_type = (ValaDataType*) _tmp257_;
				can_propagate_error = FALSE;
				{
					ValaList* _method_error_type_list = NULL;
					ValaList* _tmp258_;
					gint _method_error_type_size = 0;
					ValaList* _tmp259_;
					gint _tmp260_;
					gint _tmp261_;
					gint _method_error_type_index = 0;
					_tmp258_ = vala_code_node_get_error_types ((ValaCodeNode*) self);
					_method_error_type_list = _tmp258_;
					_tmp259_ = _method_error_type_list;
					_tmp260_ = vala_collection_get_size ((ValaCollection*) _tmp259_);
					_tmp261_ = _tmp260_;
					_method_error_type_size = _tmp261_;
					_method_error_type_index = -1;
					while (TRUE) {
						gint _tmp262_;
						gint _tmp263_;
						gint _tmp264_;
						ValaDataType* method_error_type = NULL;
						ValaList* _tmp265_;
						gint _tmp266_;
						gpointer _tmp267_;
						ValaDataType* _tmp268_;
						ValaDataType* _tmp269_;
						_tmp262_ = _method_error_type_index;
						_method_error_type_index = _tmp262_ + 1;
						_tmp263_ = _method_error_type_index;
						_tmp264_ = _method_error_type_size;
						if (!(_tmp263_ < _tmp264_)) {
							break;
						}
						_tmp265_ = _method_error_type_list;
						_tmp266_ = _method_error_type_index;
						_tmp267_ = vala_list_get (_tmp265_, _tmp266_);
						method_error_type = (ValaDataType*) _tmp267_;
						_tmp268_ = body_error_type;
						_tmp269_ = method_error_type;
						if (vala_data_type_compatible (_tmp268_, _tmp269_)) {
							can_propagate_error = TRUE;
						}
						_vala_code_node_unref0 (method_error_type);
					}
					_vala_iterable_unref0 (_method_error_type_list);
				}
				_tmp271_ = can_propagate_error;
				if (!_tmp271_) {
					ValaDataType* _tmp272_;
					gboolean _tmp273_;
					gboolean _tmp274_;
					_tmp272_ = body_error_type;
					_tmp273_ = vala_error_type_get_dynamic_error (G_TYPE_CHECK_INSTANCE_CAST (_tmp272_, VALA_TYPE_ERROR_TYPE, ValaErrorType));
					_tmp274_ = _tmp273_;
					_tmp270_ = !_tmp274_;
				} else {
					_tmp270_ = FALSE;
				}
				if (_tmp270_) {
					ValaDataType* _tmp275_;
					ValaSourceReference* _tmp276_;
					ValaSourceReference* _tmp277_;
					ValaDataType* _tmp278_;
					gchar* _tmp279_;
					gchar* _tmp280_;
					gchar* _tmp281_;
					gchar* _tmp282_;
					_tmp275_ = body_error_type;
					_tmp276_ = vala_code_node_get_source_reference ((ValaCodeNode*) _tmp275_);
					_tmp277_ = _tmp276_;
					_tmp278_ = body_error_type;
					_tmp279_ = vala_code_node_to_string ((ValaCodeNode*) _tmp278_);
					_tmp280_ = _tmp279_;
					_tmp281_ = g_strdup_printf ("unhandled error `%s'", _tmp280_);
					_tmp282_ = _tmp281_;
					vala_report_warning (_tmp277_, _tmp282_);
					_g_free0 (_tmp282_);
					_g_free0 (_tmp280_);
				}
				_vala_code_node_unref0 (body_error_type);
			}
			_vala_iterable_unref0 (_body_error_type_list);
		}
	}
	_tmp283_ = vala_code_node_get_error ((ValaCodeNode*) self);
	_tmp284_ = _tmp283_;
	result = !_tmp284_;
	_vala_code_node_unref0 (old_symbol);
	_vala_source_file_unref0 (old_source_file);
	return result;
}


const gchar*
vala_creation_method_get_class_name (ValaCreationMethod* self)
{
	const gchar* result;
	const gchar* _tmp0_;
	g_return_val_if_fail (self != NULL, NULL);
	_tmp0_ = self->priv->_class_name;
	result = _tmp0_;
	return result;
}


void
vala_creation_method_set_class_name (ValaCreationMethod* self,
                                     const gchar* value)
{
	gchar* _tmp0_;
	g_return_if_fail (self != NULL);
	_tmp0_ = g_strdup (value);
	_g_free0 (self->priv->_class_name);
	self->priv->_class_name = _tmp0_;
}


gboolean
vala_creation_method_get_chain_up (ValaCreationMethod* self)
{
	gboolean result;
	gboolean _tmp0_;
	g_return_val_if_fail (self != NULL, FALSE);
	_tmp0_ = self->priv->_chain_up;
	result = _tmp0_;
	return result;
}


void
vala_creation_method_set_chain_up (ValaCreationMethod* self,
                                   gboolean value)
{
	g_return_if_fail (self != NULL);
	self->priv->_chain_up = value;
}


static void
vala_creation_method_class_init (ValaCreationMethodClass * klass)
{
	vala_creation_method_parent_class = g_type_class_peek_parent (klass);
	((ValaCodeNodeClass *) klass)->finalize = vala_creation_method_finalize;
	g_type_class_adjust_private_offset (klass, &ValaCreationMethod_private_offset);
	((ValaCodeNodeClass *) klass)->accept = (void (*) (ValaCodeNode *, ValaCodeVisitor*)) vala_creation_method_real_accept;
	((ValaCodeNodeClass *) klass)->accept_children = (void (*) (ValaCodeNode *, ValaCodeVisitor*)) vala_creation_method_real_accept_children;
	((ValaCodeNodeClass *) klass)->check = (gboolean (*) (ValaCodeNode *, ValaCodeContext*)) vala_creation_method_real_check;
}


static void
vala_creation_method_instance_init (ValaCreationMethod * self)
{
	self->priv = vala_creation_method_get_instance_private (self);
}


static void
vala_creation_method_finalize (ValaCodeNode * obj)
{
	ValaCreationMethod * self;
	self = G_TYPE_CHECK_INSTANCE_CAST (obj, VALA_TYPE_CREATION_METHOD, ValaCreationMethod);
	_g_free0 (self->priv->_class_name);
	VALA_CODE_NODE_CLASS (vala_creation_method_parent_class)->finalize (obj);
}


/**
 * Represents a type creation method.
 */
GType
vala_creation_method_get_type (void)
{
	static volatile gsize vala_creation_method_type_id__volatile = 0;
	if (g_once_init_enter (&vala_creation_method_type_id__volatile)) {
		static const GTypeInfo g_define_type_info = { sizeof (ValaCreationMethodClass), (GBaseInitFunc) NULL, (GBaseFinalizeFunc) NULL, (GClassInitFunc) vala_creation_method_class_init, (GClassFinalizeFunc) NULL, NULL, sizeof (ValaCreationMethod), 0, (GInstanceInitFunc) vala_creation_method_instance_init, NULL };
		GType vala_creation_method_type_id;
		vala_creation_method_type_id = g_type_register_static (VALA_TYPE_METHOD, "ValaCreationMethod", &g_define_type_info, 0);
		ValaCreationMethod_private_offset = g_type_add_instance_private (vala_creation_method_type_id, sizeof (ValaCreationMethodPrivate));
		g_once_init_leave (&vala_creation_method_type_id__volatile, vala_creation_method_type_id);
	}
	return vala_creation_method_type_id__volatile;
}


