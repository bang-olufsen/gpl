/* valadelegate.c generated by valac, the Vala compiler
 * generated from valadelegate.vala, do not modify */

/* valadelegate.vala
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
#include <stdlib.h>
#include <string.h>

#define _vala_code_node_unref0(var) ((var == NULL) ? NULL : (var = (vala_code_node_unref (var), NULL)))
#define _vala_iterable_unref0(var) ((var == NULL) ? NULL : (var = (vala_iterable_unref (var), NULL)))
#define _g_free0(var) ((var == NULL) ? NULL : (var = (g_free (var), NULL)))
#define _vala_iterator_unref0(var) ((var == NULL) ? NULL : (var = (vala_iterator_unref (var), NULL)))
#define _vala_source_file_unref0(var) ((var == NULL) ? NULL : (var = (vala_source_file_unref (var), NULL)))

struct _ValaDelegatePrivate {
	ValaDataType* _sender_type;
	ValaList* type_parameters;
	ValaList* parameters;
	ValaDataType* _return_type;
	gboolean* _has_target;
};


static gint ValaDelegate_private_offset;
static gpointer vala_delegate_parent_class = NULL;
static ValaCallableIface * vala_delegate_vala_callable_parent_iface = NULL;

static gint vala_delegate_real_get_type_parameter_index (ValaTypeSymbol* base,
                                                  const gchar* name);
static void vala_delegate_real_add_parameter (ValaCallable* base,
                                       ValaParameter* param);
static ValaList* vala_delegate_real_get_parameters (ValaCallable* base);
static void vala_delegate_real_accept (ValaCodeNode* base,
                                ValaCodeVisitor* visitor);
static void vala_delegate_real_accept_children (ValaCodeNode* base,
                                         ValaCodeVisitor* visitor);
static gboolean vala_delegate_real_is_reference_type (ValaTypeSymbol* base);
static void vala_delegate_real_replace_type (ValaCodeNode* base,
                                      ValaDataType* old_type,
                                      ValaDataType* new_type);
static gboolean vala_delegate_real_check (ValaCodeNode* base,
                                   ValaCodeContext* context);
static gboolean* _bool_dup (gboolean* self);
static void vala_delegate_finalize (ValaCodeNode * obj);


static inline gpointer
vala_delegate_get_instance_private (ValaDelegate* self)
{
	return G_STRUCT_MEMBER_P (self, ValaDelegate_private_offset);
}


/**
 * Creates a new delegate.
 *
 * @param name              delegate type name
 * @param return_type       return type
 * @param source_reference  reference to source code
 * @return                  newly created delegate
 */
ValaDelegate*
vala_delegate_construct (GType object_type,
                         const gchar* name,
                         ValaDataType* return_type,
                         ValaSourceReference* source_reference,
                         ValaComment* comment)
{
	ValaDelegate* self = NULL;
	g_return_val_if_fail (return_type != NULL, NULL);
	self = (ValaDelegate*) vala_typesymbol_construct (object_type, name, source_reference, comment);
	vala_callable_set_return_type ((ValaCallable*) self, return_type);
	return self;
}


ValaDelegate*
vala_delegate_new (const gchar* name,
                   ValaDataType* return_type,
                   ValaSourceReference* source_reference,
                   ValaComment* comment)
{
	return vala_delegate_construct (VALA_TYPE_DELEGATE, name, return_type, source_reference, comment);
}


/**
 * Appends the specified parameter to the list of type parameters.
 *
 * @param p a type parameter
 */
void
vala_delegate_add_type_parameter (ValaDelegate* self,
                                  ValaTypeParameter* p)
{
	ValaList* _tmp0_;
	ValaScope* _tmp1_;
	ValaScope* _tmp2_;
	const gchar* _tmp3_;
	const gchar* _tmp4_;
	g_return_if_fail (self != NULL);
	g_return_if_fail (p != NULL);
	_tmp0_ = self->priv->type_parameters;
	vala_collection_add ((ValaCollection*) _tmp0_, p);
	_tmp1_ = vala_symbol_get_scope ((ValaSymbol*) self);
	_tmp2_ = _tmp1_;
	_tmp3_ = vala_symbol_get_name ((ValaSymbol*) p);
	_tmp4_ = _tmp3_;
	vala_scope_add (_tmp2_, _tmp4_, (ValaSymbol*) p);
}


static gpointer
_vala_iterable_ref0 (gpointer self)
{
	return self ? vala_iterable_ref (self) : NULL;
}


ValaList*
vala_delegate_get_type_parameters (ValaDelegate* self)
{
	ValaList* result = NULL;
	ValaList* _tmp0_;
	ValaList* _tmp1_;
	g_return_val_if_fail (self != NULL, NULL);
	_tmp0_ = self->priv->type_parameters;
	_tmp1_ = _vala_iterable_ref0 (_tmp0_);
	result = _tmp1_;
	return result;
}


static gint
vala_delegate_real_get_type_parameter_index (ValaTypeSymbol* base,
                                             const gchar* name)
{
	ValaDelegate * self;
	gint result = 0;
	gint i = 0;
	self = (ValaDelegate*) base;
	g_return_val_if_fail (name != NULL, 0);
	i = 0;
	{
		ValaList* _parameter_list = NULL;
		ValaList* _tmp0_;
		ValaList* _tmp1_;
		gint _parameter_size = 0;
		ValaList* _tmp2_;
		gint _tmp3_;
		gint _tmp4_;
		gint _parameter_index = 0;
		_tmp0_ = self->priv->type_parameters;
		_tmp1_ = _vala_iterable_ref0 (_tmp0_);
		_parameter_list = _tmp1_;
		_tmp2_ = _parameter_list;
		_tmp3_ = vala_collection_get_size ((ValaCollection*) _tmp2_);
		_tmp4_ = _tmp3_;
		_parameter_size = _tmp4_;
		_parameter_index = -1;
		while (TRUE) {
			gint _tmp5_;
			gint _tmp6_;
			gint _tmp7_;
			ValaTypeParameter* parameter = NULL;
			ValaList* _tmp8_;
			gint _tmp9_;
			gpointer _tmp10_;
			ValaTypeParameter* _tmp11_;
			const gchar* _tmp12_;
			const gchar* _tmp13_;
			gint _tmp14_;
			_tmp5_ = _parameter_index;
			_parameter_index = _tmp5_ + 1;
			_tmp6_ = _parameter_index;
			_tmp7_ = _parameter_size;
			if (!(_tmp6_ < _tmp7_)) {
				break;
			}
			_tmp8_ = _parameter_list;
			_tmp9_ = _parameter_index;
			_tmp10_ = vala_list_get (_tmp8_, _tmp9_);
			parameter = (ValaTypeParameter*) _tmp10_;
			_tmp11_ = parameter;
			_tmp12_ = vala_symbol_get_name ((ValaSymbol*) _tmp11_);
			_tmp13_ = _tmp12_;
			if (g_strcmp0 (_tmp13_, name) == 0) {
				result = i;
				_vala_code_node_unref0 (parameter);
				_vala_iterable_unref0 (_parameter_list);
				return result;
			}
			_tmp14_ = i;
			i = _tmp14_ + 1;
			_vala_code_node_unref0 (parameter);
		}
		_vala_iterable_unref0 (_parameter_list);
	}
	result = -1;
	return result;
}


/**
 * Appends paramater to this callback function.
 *
 * @param param a formal parameter
 */
static void
vala_delegate_real_add_parameter (ValaCallable* base,
                                  ValaParameter* param)
{
	ValaDelegate * self;
	ValaList* _tmp0_;
	ValaScope* _tmp1_;
	ValaScope* _tmp2_;
	const gchar* _tmp3_;
	const gchar* _tmp4_;
	self = (ValaDelegate*) base;
	g_return_if_fail (param != NULL);
	_tmp0_ = self->priv->parameters;
	vala_collection_add ((ValaCollection*) _tmp0_, param);
	_tmp1_ = vala_symbol_get_scope ((ValaSymbol*) self);
	_tmp2_ = _tmp1_;
	_tmp3_ = vala_symbol_get_name ((ValaSymbol*) param);
	_tmp4_ = _tmp3_;
	vala_scope_add (_tmp2_, _tmp4_, (ValaSymbol*) param);
}


/**
 * Return copy of parameter list.
 *
 * @return parameter list
 */
static ValaList*
vala_delegate_real_get_parameters (ValaCallable* base)
{
	ValaDelegate * self;
	ValaList* result = NULL;
	ValaList* _tmp0_;
	ValaList* _tmp1_;
	self = (ValaDelegate*) base;
	_tmp0_ = self->priv->parameters;
	_tmp1_ = _vala_iterable_ref0 (_tmp0_);
	result = _tmp1_;
	return result;
}


/**
 * Checks whether the arguments and return type of the specified method
 * matches this callback.
 *
 * @param m a method
 * @return  true if the specified method is compatible to this callback
 */
gboolean
vala_delegate_matches_method (ValaDelegate* self,
                              ValaMethod* m,
                              ValaDataType* dt)
{
	gboolean result = FALSE;
	gboolean _tmp0_ = FALSE;
	gboolean _tmp1_;
	gboolean _tmp2_;
	ValaDataType* _tmp5_;
	ValaDataType* _tmp6_;
	ValaDataType* _tmp7_;
	ValaDataType* _tmp8_;
	ValaDataType* _tmp9_;
	ValaDataType* _tmp10_;
	gboolean _tmp11_;
	ValaList* method_params = NULL;
	ValaList* _tmp12_;
	ValaIterator* method_params_it = NULL;
	ValaList* _tmp13_;
	ValaIterator* _tmp14_;
	gboolean _tmp15_ = FALSE;
	ValaDataType* _tmp16_;
	gboolean first = FALSE;
	ValaIterator* _tmp60_;
	ValaList* error_types = NULL;
	ValaList* _tmp61_;
	ValaList* method_error_types = NULL;
	ValaList* _tmp62_;
	gboolean _tmp63_ = FALSE;
	ValaList* _tmp64_;
	gint _tmp65_;
	gint _tmp66_;
	g_return_val_if_fail (self != NULL, FALSE);
	g_return_val_if_fail (m != NULL, FALSE);
	g_return_val_if_fail (dt != NULL, FALSE);
	_tmp1_ = vala_method_get_coroutine (m);
	_tmp2_ = _tmp1_;
	if (_tmp2_) {
		ValaSymbol* _tmp3_;
		ValaSymbol* _tmp4_;
		_tmp3_ = vala_symbol_get_parent_symbol ((ValaSymbol*) self);
		_tmp4_ = _tmp3_;
		_tmp0_ = !G_TYPE_CHECK_INSTANCE_TYPE (_tmp4_, VALA_TYPE_SIGNAL);
	} else {
		_tmp0_ = FALSE;
	}
	if (_tmp0_) {
		result = FALSE;
		return result;
	}
	_tmp5_ = vala_callable_get_return_type ((ValaCallable*) m);
	_tmp6_ = _tmp5_;
	_tmp7_ = vala_callable_get_return_type ((ValaCallable*) self);
	_tmp8_ = _tmp7_;
	_tmp9_ = vala_data_type_get_actual_type (_tmp8_, dt, NULL, (ValaCodeNode*) self);
	_tmp10_ = _tmp9_;
	_tmp11_ = !vala_data_type_stricter (_tmp6_, _tmp10_);
	_vala_code_node_unref0 (_tmp10_);
	if (_tmp11_) {
		result = FALSE;
		return result;
	}
	_tmp12_ = vala_callable_get_parameters ((ValaCallable*) m);
	method_params = _tmp12_;
	_tmp13_ = method_params;
	_tmp14_ = vala_iterable_iterator ((ValaIterable*) _tmp13_);
	method_params_it = _tmp14_;
	_tmp16_ = self->priv->_sender_type;
	if (_tmp16_ != NULL) {
		ValaList* _tmp17_;
		gint _tmp18_;
		gint _tmp19_;
		ValaList* _tmp20_;
		gint _tmp21_;
		gint _tmp22_;
		_tmp17_ = method_params;
		_tmp18_ = vala_collection_get_size ((ValaCollection*) _tmp17_);
		_tmp19_ = _tmp18_;
		_tmp20_ = self->priv->parameters;
		_tmp21_ = vala_collection_get_size ((ValaCollection*) _tmp20_);
		_tmp22_ = _tmp21_;
		_tmp15_ = _tmp19_ == (_tmp22_ + 1);
	} else {
		_tmp15_ = FALSE;
	}
	if (_tmp15_) {
		ValaIterator* _tmp23_;
		ValaParameter* method_param = NULL;
		ValaIterator* _tmp24_;
		gpointer _tmp25_;
		ValaDataType* _tmp26_;
		ValaParameter* _tmp27_;
		ValaDataType* _tmp28_;
		ValaDataType* _tmp29_;
		_tmp23_ = method_params_it;
		vala_iterator_next (_tmp23_);
		_tmp24_ = method_params_it;
		_tmp25_ = vala_iterator_get (_tmp24_);
		method_param = (ValaParameter*) _tmp25_;
		_tmp26_ = self->priv->_sender_type;
		_tmp27_ = method_param;
		_tmp28_ = vala_variable_get_variable_type ((ValaVariable*) _tmp27_);
		_tmp29_ = _tmp28_;
		if (!vala_data_type_stricter (_tmp26_, _tmp29_)) {
			result = FALSE;
			_vala_code_node_unref0 (method_param);
			_vala_iterator_unref0 (method_params_it);
			_vala_iterable_unref0 (method_params);
			return result;
		}
		_vala_code_node_unref0 (method_param);
	}
	first = TRUE;
	{
		ValaList* _param_list = NULL;
		ValaList* _tmp30_;
		ValaList* _tmp31_;
		gint _param_size = 0;
		ValaList* _tmp32_;
		gint _tmp33_;
		gint _tmp34_;
		gint _param_index = 0;
		_tmp30_ = self->priv->parameters;
		_tmp31_ = _vala_iterable_ref0 (_tmp30_);
		_param_list = _tmp31_;
		_tmp32_ = _param_list;
		_tmp33_ = vala_collection_get_size ((ValaCollection*) _tmp32_);
		_tmp34_ = _tmp33_;
		_param_size = _tmp34_;
		_param_index = -1;
		while (TRUE) {
			gint _tmp35_;
			gint _tmp36_;
			gint _tmp37_;
			ValaParameter* param = NULL;
			ValaList* _tmp38_;
			gint _tmp39_;
			gpointer _tmp40_;
			gboolean _tmp41_ = FALSE;
			gboolean _tmp42_ = FALSE;
			gboolean _tmp43_;
			ValaIterator* _tmp48_;
			ValaParameter* method_param = NULL;
			ValaIterator* _tmp49_;
			gpointer _tmp50_;
			ValaParameter* _tmp51_;
			ValaDataType* _tmp52_;
			ValaDataType* _tmp53_;
			ValaDataType* _tmp54_;
			ValaDataType* _tmp55_;
			ValaParameter* _tmp56_;
			ValaDataType* _tmp57_;
			ValaDataType* _tmp58_;
			gboolean _tmp59_;
			_tmp35_ = _param_index;
			_param_index = _tmp35_ + 1;
			_tmp36_ = _param_index;
			_tmp37_ = _param_size;
			if (!(_tmp36_ < _tmp37_)) {
				break;
			}
			_tmp38_ = _param_list;
			_tmp39_ = _param_index;
			_tmp40_ = vala_list_get (_tmp38_, _tmp39_);
			param = (ValaParameter*) _tmp40_;
			_tmp43_ = first;
			if (_tmp43_) {
				ValaMemberBinding _tmp44_;
				ValaMemberBinding _tmp45_;
				_tmp44_ = vala_method_get_binding (m);
				_tmp45_ = _tmp44_;
				_tmp42_ = _tmp45_ == VALA_MEMBER_BINDING_INSTANCE;
			} else {
				_tmp42_ = FALSE;
			}
			if (_tmp42_) {
				gboolean _tmp46_;
				gboolean _tmp47_;
				_tmp46_ = vala_delegate_get_has_target (self);
				_tmp47_ = _tmp46_;
				_tmp41_ = !_tmp47_;
			} else {
				_tmp41_ = FALSE;
			}
			if (_tmp41_) {
				first = FALSE;
				_vala_code_node_unref0 (param);
				continue;
			}
			_tmp48_ = method_params_it;
			if (!vala_iterator_next (_tmp48_)) {
				_vala_code_node_unref0 (param);
				break;
			}
			_tmp49_ = method_params_it;
			_tmp50_ = vala_iterator_get (_tmp49_);
			method_param = (ValaParameter*) _tmp50_;
			_tmp51_ = param;
			_tmp52_ = vala_variable_get_variable_type ((ValaVariable*) _tmp51_);
			_tmp53_ = _tmp52_;
			_tmp54_ = vala_data_type_get_actual_type (_tmp53_, dt, NULL, (ValaCodeNode*) self);
			_tmp55_ = _tmp54_;
			_tmp56_ = method_param;
			_tmp57_ = vala_variable_get_variable_type ((ValaVariable*) _tmp56_);
			_tmp58_ = _tmp57_;
			_tmp59_ = !vala_data_type_stricter (_tmp55_, _tmp58_);
			_vala_code_node_unref0 (_tmp55_);
			if (_tmp59_) {
				result = FALSE;
				_vala_code_node_unref0 (method_param);
				_vala_code_node_unref0 (param);
				_vala_iterable_unref0 (_param_list);
				_vala_iterator_unref0 (method_params_it);
				_vala_iterable_unref0 (method_params);
				return result;
			}
			_vala_code_node_unref0 (method_param);
			_vala_code_node_unref0 (param);
		}
		_vala_iterable_unref0 (_param_list);
	}
	_tmp60_ = method_params_it;
	if (vala_iterator_next (_tmp60_)) {
		result = FALSE;
		_vala_iterator_unref0 (method_params_it);
		_vala_iterable_unref0 (method_params);
		return result;
	}
	_tmp61_ = vala_code_node_get_error_types ((ValaCodeNode*) self);
	error_types = _tmp61_;
	_tmp62_ = vala_code_node_get_error_types ((ValaCodeNode*) m);
	method_error_types = _tmp62_;
	_tmp64_ = error_types;
	_tmp65_ = vala_collection_get_size ((ValaCollection*) _tmp64_);
	_tmp66_ = _tmp65_;
	if (_tmp66_ > 0) {
		ValaList* _tmp67_;
		gint _tmp68_;
		gint _tmp69_;
		_tmp67_ = method_error_types;
		_tmp68_ = vala_collection_get_size ((ValaCollection*) _tmp67_);
		_tmp69_ = _tmp68_;
		_tmp63_ = _tmp69_ == 0;
	} else {
		_tmp63_ = FALSE;
	}
	if (_tmp63_) {
		result = FALSE;
		_vala_iterable_unref0 (method_error_types);
		_vala_iterable_unref0 (error_types);
		_vala_iterator_unref0 (method_params_it);
		_vala_iterable_unref0 (method_params);
		return result;
	}
	{
		ValaList* _method_error_type_list = NULL;
		ValaList* _tmp70_;
		ValaList* _tmp71_;
		gint _method_error_type_size = 0;
		ValaList* _tmp72_;
		gint _tmp73_;
		gint _tmp74_;
		gint _method_error_type_index = 0;
		_tmp70_ = method_error_types;
		_tmp71_ = _vala_iterable_ref0 (_tmp70_);
		_method_error_type_list = _tmp71_;
		_tmp72_ = _method_error_type_list;
		_tmp73_ = vala_collection_get_size ((ValaCollection*) _tmp72_);
		_tmp74_ = _tmp73_;
		_method_error_type_size = _tmp74_;
		_method_error_type_index = -1;
		while (TRUE) {
			gint _tmp75_;
			gint _tmp76_;
			gint _tmp77_;
			ValaDataType* method_error_type = NULL;
			ValaList* _tmp78_;
			gint _tmp79_;
			gpointer _tmp80_;
			gboolean match = FALSE;
			gboolean _tmp94_;
			_tmp75_ = _method_error_type_index;
			_method_error_type_index = _tmp75_ + 1;
			_tmp76_ = _method_error_type_index;
			_tmp77_ = _method_error_type_size;
			if (!(_tmp76_ < _tmp77_)) {
				break;
			}
			_tmp78_ = _method_error_type_list;
			_tmp79_ = _method_error_type_index;
			_tmp80_ = vala_list_get (_tmp78_, _tmp79_);
			method_error_type = (ValaDataType*) _tmp80_;
			match = FALSE;
			{
				ValaList* _delegate_error_type_list = NULL;
				ValaList* _tmp81_;
				ValaList* _tmp82_;
				gint _delegate_error_type_size = 0;
				ValaList* _tmp83_;
				gint _tmp84_;
				gint _tmp85_;
				gint _delegate_error_type_index = 0;
				_tmp81_ = error_types;
				_tmp82_ = _vala_iterable_ref0 (_tmp81_);
				_delegate_error_type_list = _tmp82_;
				_tmp83_ = _delegate_error_type_list;
				_tmp84_ = vala_collection_get_size ((ValaCollection*) _tmp83_);
				_tmp85_ = _tmp84_;
				_delegate_error_type_size = _tmp85_;
				_delegate_error_type_index = -1;
				while (TRUE) {
					gint _tmp86_;
					gint _tmp87_;
					gint _tmp88_;
					ValaDataType* delegate_error_type = NULL;
					ValaList* _tmp89_;
					gint _tmp90_;
					gpointer _tmp91_;
					ValaDataType* _tmp92_;
					ValaDataType* _tmp93_;
					_tmp86_ = _delegate_error_type_index;
					_delegate_error_type_index = _tmp86_ + 1;
					_tmp87_ = _delegate_error_type_index;
					_tmp88_ = _delegate_error_type_size;
					if (!(_tmp87_ < _tmp88_)) {
						break;
					}
					_tmp89_ = _delegate_error_type_list;
					_tmp90_ = _delegate_error_type_index;
					_tmp91_ = vala_list_get (_tmp89_, _tmp90_);
					delegate_error_type = (ValaDataType*) _tmp91_;
					_tmp92_ = method_error_type;
					_tmp93_ = delegate_error_type;
					if (vala_data_type_compatible (_tmp92_, _tmp93_)) {
						match = TRUE;
						_vala_code_node_unref0 (delegate_error_type);
						break;
					}
					_vala_code_node_unref0 (delegate_error_type);
				}
				_vala_iterable_unref0 (_delegate_error_type_list);
			}
			_tmp94_ = match;
			if (!_tmp94_) {
				result = FALSE;
				_vala_code_node_unref0 (method_error_type);
				_vala_iterable_unref0 (_method_error_type_list);
				_vala_iterable_unref0 (method_error_types);
				_vala_iterable_unref0 (error_types);
				_vala_iterator_unref0 (method_params_it);
				_vala_iterable_unref0 (method_params);
				return result;
			}
			_vala_code_node_unref0 (method_error_type);
		}
		_vala_iterable_unref0 (_method_error_type_list);
	}
	result = TRUE;
	_vala_iterable_unref0 (method_error_types);
	_vala_iterable_unref0 (error_types);
	_vala_iterator_unref0 (method_params_it);
	_vala_iterable_unref0 (method_params);
	return result;
}


static void
vala_delegate_real_accept (ValaCodeNode* base,
                           ValaCodeVisitor* visitor)
{
	ValaDelegate * self;
	self = (ValaDelegate*) base;
	g_return_if_fail (visitor != NULL);
	vala_code_visitor_visit_delegate (visitor, self);
}


static void
vala_delegate_real_accept_children (ValaCodeNode* base,
                                    ValaCodeVisitor* visitor)
{
	ValaDelegate * self;
	ValaDataType* _tmp12_;
	ValaDataType* _tmp13_;
	self = (ValaDelegate*) base;
	g_return_if_fail (visitor != NULL);
	{
		ValaList* _p_list = NULL;
		ValaList* _tmp0_;
		ValaList* _tmp1_;
		gint _p_size = 0;
		ValaList* _tmp2_;
		gint _tmp3_;
		gint _tmp4_;
		gint _p_index = 0;
		_tmp0_ = self->priv->type_parameters;
		_tmp1_ = _vala_iterable_ref0 (_tmp0_);
		_p_list = _tmp1_;
		_tmp2_ = _p_list;
		_tmp3_ = vala_collection_get_size ((ValaCollection*) _tmp2_);
		_tmp4_ = _tmp3_;
		_p_size = _tmp4_;
		_p_index = -1;
		while (TRUE) {
			gint _tmp5_;
			gint _tmp6_;
			gint _tmp7_;
			ValaTypeParameter* p = NULL;
			ValaList* _tmp8_;
			gint _tmp9_;
			gpointer _tmp10_;
			ValaTypeParameter* _tmp11_;
			_tmp5_ = _p_index;
			_p_index = _tmp5_ + 1;
			_tmp6_ = _p_index;
			_tmp7_ = _p_size;
			if (!(_tmp6_ < _tmp7_)) {
				break;
			}
			_tmp8_ = _p_list;
			_tmp9_ = _p_index;
			_tmp10_ = vala_list_get (_tmp8_, _tmp9_);
			p = (ValaTypeParameter*) _tmp10_;
			_tmp11_ = p;
			vala_code_node_accept ((ValaCodeNode*) _tmp11_, visitor);
			_vala_code_node_unref0 (p);
		}
		_vala_iterable_unref0 (_p_list);
	}
	_tmp12_ = vala_callable_get_return_type ((ValaCallable*) self);
	_tmp13_ = _tmp12_;
	vala_code_node_accept ((ValaCodeNode*) _tmp13_, visitor);
	{
		ValaList* _param_list = NULL;
		ValaList* _tmp14_;
		ValaList* _tmp15_;
		gint _param_size = 0;
		ValaList* _tmp16_;
		gint _tmp17_;
		gint _tmp18_;
		gint _param_index = 0;
		_tmp14_ = self->priv->parameters;
		_tmp15_ = _vala_iterable_ref0 (_tmp14_);
		_param_list = _tmp15_;
		_tmp16_ = _param_list;
		_tmp17_ = vala_collection_get_size ((ValaCollection*) _tmp16_);
		_tmp18_ = _tmp17_;
		_param_size = _tmp18_;
		_param_index = -1;
		while (TRUE) {
			gint _tmp19_;
			gint _tmp20_;
			gint _tmp21_;
			ValaParameter* param = NULL;
			ValaList* _tmp22_;
			gint _tmp23_;
			gpointer _tmp24_;
			ValaParameter* _tmp25_;
			_tmp19_ = _param_index;
			_param_index = _tmp19_ + 1;
			_tmp20_ = _param_index;
			_tmp21_ = _param_size;
			if (!(_tmp20_ < _tmp21_)) {
				break;
			}
			_tmp22_ = _param_list;
			_tmp23_ = _param_index;
			_tmp24_ = vala_list_get (_tmp22_, _tmp23_);
			param = (ValaParameter*) _tmp24_;
			_tmp25_ = param;
			vala_code_node_accept ((ValaCodeNode*) _tmp25_, visitor);
			_vala_code_node_unref0 (param);
		}
		_vala_iterable_unref0 (_param_list);
	}
	{
		ValaList* _error_type_list = NULL;
		ValaList* _tmp26_;
		gint _error_type_size = 0;
		ValaList* _tmp27_;
		gint _tmp28_;
		gint _tmp29_;
		gint _error_type_index = 0;
		_tmp26_ = vala_code_node_get_error_types ((ValaCodeNode*) self);
		_error_type_list = _tmp26_;
		_tmp27_ = _error_type_list;
		_tmp28_ = vala_collection_get_size ((ValaCollection*) _tmp27_);
		_tmp29_ = _tmp28_;
		_error_type_size = _tmp29_;
		_error_type_index = -1;
		while (TRUE) {
			gint _tmp30_;
			gint _tmp31_;
			gint _tmp32_;
			ValaDataType* error_type = NULL;
			ValaList* _tmp33_;
			gint _tmp34_;
			gpointer _tmp35_;
			ValaDataType* _tmp36_;
			_tmp30_ = _error_type_index;
			_error_type_index = _tmp30_ + 1;
			_tmp31_ = _error_type_index;
			_tmp32_ = _error_type_size;
			if (!(_tmp31_ < _tmp32_)) {
				break;
			}
			_tmp33_ = _error_type_list;
			_tmp34_ = _error_type_index;
			_tmp35_ = vala_list_get (_tmp33_, _tmp34_);
			error_type = (ValaDataType*) _tmp35_;
			_tmp36_ = error_type;
			vala_code_node_accept ((ValaCodeNode*) _tmp36_, visitor);
			_vala_code_node_unref0 (error_type);
		}
		_vala_iterable_unref0 (_error_type_list);
	}
}


static gboolean
vala_delegate_real_is_reference_type (ValaTypeSymbol* base)
{
	ValaDelegate * self;
	gboolean result = FALSE;
	self = (ValaDelegate*) base;
	result = FALSE;
	return result;
}


static void
vala_delegate_real_replace_type (ValaCodeNode* base,
                                 ValaDataType* old_type,
                                 ValaDataType* new_type)
{
	ValaDelegate * self;
	ValaDataType* _tmp0_;
	ValaDataType* _tmp1_;
	ValaList* error_types = NULL;
	ValaList* _tmp2_;
	self = (ValaDelegate*) base;
	g_return_if_fail (old_type != NULL);
	g_return_if_fail (new_type != NULL);
	_tmp0_ = vala_callable_get_return_type ((ValaCallable*) self);
	_tmp1_ = _tmp0_;
	if (_tmp1_ == old_type) {
		vala_callable_set_return_type ((ValaCallable*) self, new_type);
		return;
	}
	_tmp2_ = vala_code_node_get_error_types ((ValaCodeNode*) self);
	error_types = _tmp2_;
	{
		gint i = 0;
		i = 0;
		{
			gboolean _tmp3_ = FALSE;
			_tmp3_ = TRUE;
			while (TRUE) {
				gint _tmp5_;
				ValaList* _tmp6_;
				gint _tmp7_;
				gint _tmp8_;
				ValaList* _tmp9_;
				gint _tmp10_;
				gpointer _tmp11_;
				ValaDataType* _tmp12_;
				gboolean _tmp13_;
				if (!_tmp3_) {
					gint _tmp4_;
					_tmp4_ = i;
					i = _tmp4_ + 1;
				}
				_tmp3_ = FALSE;
				_tmp5_ = i;
				_tmp6_ = error_types;
				_tmp7_ = vala_collection_get_size ((ValaCollection*) _tmp6_);
				_tmp8_ = _tmp7_;
				if (!(_tmp5_ < _tmp8_)) {
					break;
				}
				_tmp9_ = error_types;
				_tmp10_ = i;
				_tmp11_ = vala_list_get (_tmp9_, _tmp10_);
				_tmp12_ = (ValaDataType*) _tmp11_;
				_tmp13_ = _tmp12_ == old_type;
				_vala_code_node_unref0 (_tmp12_);
				if (_tmp13_) {
					ValaList* _tmp14_;
					gint _tmp15_;
					_tmp14_ = error_types;
					_tmp15_ = i;
					vala_list_set (_tmp14_, _tmp15_, new_type);
					_vala_iterable_unref0 (error_types);
					return;
				}
			}
		}
	}
	_vala_iterable_unref0 (error_types);
}


static gpointer
_vala_source_file_ref0 (gpointer self)
{
	return self ? vala_source_file_ref (self) : NULL;
}


static gboolean
vala_delegate_real_check (ValaCodeNode* base,
                          ValaCodeContext* context)
{
	ValaDelegate * self;
	gboolean result = FALSE;
	gboolean _tmp0_;
	gboolean _tmp1_;
	ValaSourceFile* old_source_file = NULL;
	ValaSemanticAnalyzer* _tmp4_;
	ValaSemanticAnalyzer* _tmp5_;
	ValaSourceFile* _tmp6_;
	ValaSourceFile* _tmp7_;
	ValaSourceFile* _tmp8_;
	ValaSourceReference* _tmp9_;
	ValaSourceReference* _tmp10_;
	ValaDataType* _tmp29_;
	ValaDataType* _tmp30_;
	ValaSemanticAnalyzer* _tmp54_;
	ValaSemanticAnalyzer* _tmp55_;
	ValaSourceFile* _tmp56_;
	gboolean _tmp57_;
	gboolean _tmp58_;
	self = (ValaDelegate*) base;
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
	_tmp4_ = vala_code_context_get_analyzer (context);
	_tmp5_ = _tmp4_;
	_tmp6_ = vala_semantic_analyzer_get_current_source_file (_tmp5_);
	_tmp7_ = _tmp6_;
	_tmp8_ = _vala_source_file_ref0 (_tmp7_);
	old_source_file = _tmp8_;
	_tmp9_ = vala_code_node_get_source_reference ((ValaCodeNode*) self);
	_tmp10_ = _tmp9_;
	if (_tmp10_ != NULL) {
		ValaSemanticAnalyzer* _tmp11_;
		ValaSemanticAnalyzer* _tmp12_;
		ValaSourceReference* _tmp13_;
		ValaSourceReference* _tmp14_;
		ValaSourceFile* _tmp15_;
		ValaSourceFile* _tmp16_;
		_tmp11_ = vala_code_context_get_analyzer (context);
		_tmp12_ = _tmp11_;
		_tmp13_ = vala_code_node_get_source_reference ((ValaCodeNode*) self);
		_tmp14_ = _tmp13_;
		_tmp15_ = vala_source_reference_get_file (_tmp14_);
		_tmp16_ = _tmp15_;
		vala_semantic_analyzer_set_current_source_file (_tmp12_, _tmp16_);
	}
	{
		ValaList* _p_list = NULL;
		ValaList* _tmp17_;
		ValaList* _tmp18_;
		gint _p_size = 0;
		ValaList* _tmp19_;
		gint _tmp20_;
		gint _tmp21_;
		gint _p_index = 0;
		_tmp17_ = self->priv->type_parameters;
		_tmp18_ = _vala_iterable_ref0 (_tmp17_);
		_p_list = _tmp18_;
		_tmp19_ = _p_list;
		_tmp20_ = vala_collection_get_size ((ValaCollection*) _tmp19_);
		_tmp21_ = _tmp20_;
		_p_size = _tmp21_;
		_p_index = -1;
		while (TRUE) {
			gint _tmp22_;
			gint _tmp23_;
			gint _tmp24_;
			ValaTypeParameter* p = NULL;
			ValaList* _tmp25_;
			gint _tmp26_;
			gpointer _tmp27_;
			ValaTypeParameter* _tmp28_;
			_tmp22_ = _p_index;
			_p_index = _tmp22_ + 1;
			_tmp23_ = _p_index;
			_tmp24_ = _p_size;
			if (!(_tmp23_ < _tmp24_)) {
				break;
			}
			_tmp25_ = _p_list;
			_tmp26_ = _p_index;
			_tmp27_ = vala_list_get (_tmp25_, _tmp26_);
			p = (ValaTypeParameter*) _tmp27_;
			_tmp28_ = p;
			vala_code_node_check ((ValaCodeNode*) _tmp28_, context);
			_vala_code_node_unref0 (p);
		}
		_vala_iterable_unref0 (_p_list);
	}
	_tmp29_ = vala_callable_get_return_type ((ValaCallable*) self);
	_tmp30_ = _tmp29_;
	vala_code_node_check ((ValaCodeNode*) _tmp30_, context);
	{
		ValaList* _param_list = NULL;
		ValaList* _tmp31_;
		ValaList* _tmp32_;
		gint _param_size = 0;
		ValaList* _tmp33_;
		gint _tmp34_;
		gint _tmp35_;
		gint _param_index = 0;
		_tmp31_ = self->priv->parameters;
		_tmp32_ = _vala_iterable_ref0 (_tmp31_);
		_param_list = _tmp32_;
		_tmp33_ = _param_list;
		_tmp34_ = vala_collection_get_size ((ValaCollection*) _tmp33_);
		_tmp35_ = _tmp34_;
		_param_size = _tmp35_;
		_param_index = -1;
		while (TRUE) {
			gint _tmp36_;
			gint _tmp37_;
			gint _tmp38_;
			ValaParameter* param = NULL;
			ValaList* _tmp39_;
			gint _tmp40_;
			gpointer _tmp41_;
			ValaParameter* _tmp42_;
			_tmp36_ = _param_index;
			_param_index = _tmp36_ + 1;
			_tmp37_ = _param_index;
			_tmp38_ = _param_size;
			if (!(_tmp37_ < _tmp38_)) {
				break;
			}
			_tmp39_ = _param_list;
			_tmp40_ = _param_index;
			_tmp41_ = vala_list_get (_tmp39_, _tmp40_);
			param = (ValaParameter*) _tmp41_;
			_tmp42_ = param;
			vala_code_node_check ((ValaCodeNode*) _tmp42_, context);
			_vala_code_node_unref0 (param);
		}
		_vala_iterable_unref0 (_param_list);
	}
	{
		ValaList* _error_type_list = NULL;
		ValaList* _tmp43_;
		gint _error_type_size = 0;
		ValaList* _tmp44_;
		gint _tmp45_;
		gint _tmp46_;
		gint _error_type_index = 0;
		_tmp43_ = vala_code_node_get_error_types ((ValaCodeNode*) self);
		_error_type_list = _tmp43_;
		_tmp44_ = _error_type_list;
		_tmp45_ = vala_collection_get_size ((ValaCollection*) _tmp44_);
		_tmp46_ = _tmp45_;
		_error_type_size = _tmp46_;
		_error_type_index = -1;
		while (TRUE) {
			gint _tmp47_;
			gint _tmp48_;
			gint _tmp49_;
			ValaDataType* error_type = NULL;
			ValaList* _tmp50_;
			gint _tmp51_;
			gpointer _tmp52_;
			ValaDataType* _tmp53_;
			_tmp47_ = _error_type_index;
			_error_type_index = _tmp47_ + 1;
			_tmp48_ = _error_type_index;
			_tmp49_ = _error_type_size;
			if (!(_tmp48_ < _tmp49_)) {
				break;
			}
			_tmp50_ = _error_type_list;
			_tmp51_ = _error_type_index;
			_tmp52_ = vala_list_get (_tmp50_, _tmp51_);
			error_type = (ValaDataType*) _tmp52_;
			_tmp53_ = error_type;
			vala_code_node_check ((ValaCodeNode*) _tmp53_, context);
			_vala_code_node_unref0 (error_type);
		}
		_vala_iterable_unref0 (_error_type_list);
	}
	_tmp54_ = vala_code_context_get_analyzer (context);
	_tmp55_ = _tmp54_;
	_tmp56_ = old_source_file;
	vala_semantic_analyzer_set_current_source_file (_tmp55_, _tmp56_);
	_tmp57_ = vala_code_node_get_error ((ValaCodeNode*) self);
	_tmp58_ = _tmp57_;
	result = !_tmp58_;
	_vala_source_file_unref0 (old_source_file);
	return result;
}


static ValaDataType*
vala_delegate_real_get_return_type (ValaCallable* base)
{
	ValaDataType* result;
	ValaDelegate* self;
	ValaDataType* _tmp0_;
	self = (ValaDelegate*) base;
	_tmp0_ = self->priv->_return_type;
	result = _tmp0_;
	return result;
}


static gpointer
_vala_code_node_ref0 (gpointer self)
{
	return self ? vala_code_node_ref (self) : NULL;
}


static void
vala_delegate_real_set_return_type (ValaCallable* base,
                                    ValaDataType* value)
{
	ValaDelegate* self;
	ValaDataType* _tmp0_;
	ValaDataType* _tmp1_;
	self = (ValaDelegate*) base;
	_tmp0_ = _vala_code_node_ref0 (value);
	_vala_code_node_unref0 (self->priv->_return_type);
	self->priv->_return_type = _tmp0_;
	_tmp1_ = self->priv->_return_type;
	vala_code_node_set_parent_node ((ValaCodeNode*) _tmp1_, (ValaCodeNode*) self);
}


static gboolean*
_bool_dup (gboolean* self)
{
	gboolean* dup;
	dup = g_new0 (gboolean, 1);
	memcpy (dup, self, sizeof (gboolean));
	return dup;
}


static gpointer
__bool_dup0 (gpointer self)
{
	return self ? _bool_dup (self) : NULL;
}


gboolean
vala_delegate_get_has_target (ValaDelegate* self)
{
	gboolean result;
	gboolean* _tmp0_;
	gboolean* _tmp3_;
	g_return_val_if_fail (self != NULL, FALSE);
	_tmp0_ = self->priv->_has_target;
	if (_tmp0_ == NULL) {
		gboolean _tmp1_;
		gboolean* _tmp2_;
		_tmp1_ = vala_code_node_get_attribute_bool ((ValaCodeNode*) self, "CCode", "has_target", TRUE);
		_tmp2_ = __bool_dup0 (&_tmp1_);
		_g_free0 (self->priv->_has_target);
		self->priv->_has_target = _tmp2_;
	}
	_tmp3_ = self->priv->_has_target;
	result = *_tmp3_;
	return result;
}


void
vala_delegate_set_has_target (ValaDelegate* self,
                              gboolean value)
{
	gboolean* _tmp0_;
	g_return_if_fail (self != NULL);
	_tmp0_ = __bool_dup0 (&value);
	_g_free0 (self->priv->_has_target);
	self->priv->_has_target = _tmp0_;
	if (value) {
		vala_code_node_remove_attribute_argument ((ValaCodeNode*) self, "CCode", "has_target");
	} else {
		vala_code_node_set_attribute_bool ((ValaCodeNode*) self, "CCode", "has_target", FALSE, NULL);
	}
}


ValaDataType*
vala_delegate_get_sender_type (ValaDelegate* self)
{
	ValaDataType* result;
	ValaDataType* _tmp0_;
	g_return_val_if_fail (self != NULL, NULL);
	_tmp0_ = self->priv->_sender_type;
	result = _tmp0_;
	return result;
}


void
vala_delegate_set_sender_type (ValaDelegate* self,
                               ValaDataType* value)
{
	ValaDataType* _tmp0_;
	g_return_if_fail (self != NULL);
	_tmp0_ = _vala_code_node_ref0 (value);
	_vala_code_node_unref0 (self->priv->_sender_type);
	self->priv->_sender_type = _tmp0_;
}


static void
vala_delegate_class_init (ValaDelegateClass * klass)
{
	vala_delegate_parent_class = g_type_class_peek_parent (klass);
	((ValaCodeNodeClass *) klass)->finalize = vala_delegate_finalize;
	g_type_class_adjust_private_offset (klass, &ValaDelegate_private_offset);
	((ValaTypeSymbolClass *) klass)->get_type_parameter_index = (gint (*) (ValaTypeSymbol *, const gchar*)) vala_delegate_real_get_type_parameter_index;
	((ValaCodeNodeClass *) klass)->accept = (void (*) (ValaCodeNode *, ValaCodeVisitor*)) vala_delegate_real_accept;
	((ValaCodeNodeClass *) klass)->accept_children = (void (*) (ValaCodeNode *, ValaCodeVisitor*)) vala_delegate_real_accept_children;
	((ValaTypeSymbolClass *) klass)->is_reference_type = (gboolean (*) (ValaTypeSymbol *)) vala_delegate_real_is_reference_type;
	((ValaCodeNodeClass *) klass)->replace_type = (void (*) (ValaCodeNode *, ValaDataType*, ValaDataType*)) vala_delegate_real_replace_type;
	((ValaCodeNodeClass *) klass)->check = (gboolean (*) (ValaCodeNode *, ValaCodeContext*)) vala_delegate_real_check;
}


static void
vala_delegate_vala_callable_interface_init (ValaCallableIface * iface)
{
	vala_delegate_vala_callable_parent_iface = g_type_interface_peek_parent (iface);
	iface->add_parameter = (void (*) (ValaCallable *, ValaParameter*)) vala_delegate_real_add_parameter;
	iface->get_parameters = (ValaList* (*) (ValaCallable *)) vala_delegate_real_get_parameters;
	iface->get_return_type = vala_delegate_real_get_return_type;
	iface->set_return_type = vala_delegate_real_set_return_type;
}


static void
vala_delegate_instance_init (ValaDelegate * self)
{
	GEqualFunc _tmp0_;
	ValaArrayList* _tmp1_;
	GEqualFunc _tmp2_;
	ValaArrayList* _tmp3_;
	self->priv = vala_delegate_get_instance_private (self);
	_tmp0_ = g_direct_equal;
	_tmp1_ = vala_array_list_new (VALA_TYPE_TYPEPARAMETER, (GBoxedCopyFunc) vala_code_node_ref, (GDestroyNotify) vala_code_node_unref, _tmp0_);
	self->priv->type_parameters = (ValaList*) _tmp1_;
	_tmp2_ = g_direct_equal;
	_tmp3_ = vala_array_list_new (VALA_TYPE_PARAMETER, (GBoxedCopyFunc) vala_code_node_ref, (GDestroyNotify) vala_code_node_unref, _tmp2_);
	self->priv->parameters = (ValaList*) _tmp3_;
}


static void
vala_delegate_finalize (ValaCodeNode * obj)
{
	ValaDelegate * self;
	self = G_TYPE_CHECK_INSTANCE_CAST (obj, VALA_TYPE_DELEGATE, ValaDelegate);
	_vala_code_node_unref0 (self->priv->_sender_type);
	_vala_iterable_unref0 (self->priv->type_parameters);
	_vala_iterable_unref0 (self->priv->parameters);
	_vala_code_node_unref0 (self->priv->_return_type);
	_g_free0 (self->priv->_has_target);
	VALA_CODE_NODE_CLASS (vala_delegate_parent_class)->finalize (obj);
}


/**
 * Represents a function callback type.
 */
GType
vala_delegate_get_type (void)
{
	static volatile gsize vala_delegate_type_id__volatile = 0;
	if (g_once_init_enter (&vala_delegate_type_id__volatile)) {
		static const GTypeInfo g_define_type_info = { sizeof (ValaDelegateClass), (GBaseInitFunc) NULL, (GBaseFinalizeFunc) NULL, (GClassInitFunc) vala_delegate_class_init, (GClassFinalizeFunc) NULL, NULL, sizeof (ValaDelegate), 0, (GInstanceInitFunc) vala_delegate_instance_init, NULL };
		static const GInterfaceInfo vala_callable_info = { (GInterfaceInitFunc) vala_delegate_vala_callable_interface_init, (GInterfaceFinalizeFunc) NULL, NULL};
		GType vala_delegate_type_id;
		vala_delegate_type_id = g_type_register_static (VALA_TYPE_TYPESYMBOL, "ValaDelegate", &g_define_type_info, 0);
		g_type_add_interface_static (vala_delegate_type_id, VALA_TYPE_CALLABLE, &vala_callable_info);
		ValaDelegate_private_offset = g_type_add_instance_private (vala_delegate_type_id, sizeof (ValaDelegatePrivate));
		g_once_init_leave (&vala_delegate_type_id__volatile, vala_delegate_type_id);
	}
	return vala_delegate_type_id__volatile;
}


