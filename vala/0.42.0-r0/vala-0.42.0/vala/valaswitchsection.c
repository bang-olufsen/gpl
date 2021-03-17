/* valaswitchsection.c generated by valac, the Vala compiler
 * generated from valaswitchsection.vala, do not modify */

/* valaswitchsection.vala
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

struct _ValaSwitchSectionPrivate {
	ValaList* labels;
};


static gint ValaSwitchSection_private_offset;
static gpointer vala_switch_section_parent_class = NULL;

static void vala_switch_section_real_accept (ValaCodeNode* base,
                                      ValaCodeVisitor* visitor);
static void vala_switch_section_real_accept_children (ValaCodeNode* base,
                                               ValaCodeVisitor* visitor);
static gboolean vala_switch_section_real_check (ValaCodeNode* base,
                                         ValaCodeContext* context);
static void vala_switch_section_real_emit (ValaCodeNode* base,
                                    ValaCodeGenerator* codegen);
static void vala_switch_section_finalize (ValaCodeNode * obj);


static inline gpointer
vala_switch_section_get_instance_private (ValaSwitchSection* self)
{
	return G_STRUCT_MEMBER_P (self, ValaSwitchSection_private_offset);
}


/**
 * Creates a new switch section.
 *
 * @param source_reference reference to source code
 * @return                 newly created switch section
 */
ValaSwitchSection*
vala_switch_section_construct (GType object_type,
                               ValaSourceReference* source_reference)
{
	ValaSwitchSection* self = NULL;
	self = (ValaSwitchSection*) vala_block_construct (object_type, source_reference);
	return self;
}


ValaSwitchSection*
vala_switch_section_new (ValaSourceReference* source_reference)
{
	return vala_switch_section_construct (VALA_TYPE_SWITCH_SECTION, source_reference);
}


/**
 * Appends the specified label to the list of switch labels.
 *
 * @param label a switch label
 */
void
vala_switch_section_add_label (ValaSwitchSection* self,
                               ValaSwitchLabel* label)
{
	ValaList* _tmp0_;
	gint _tmp1_;
	gint _tmp2_;
	ValaList* _tmp5_;
	g_return_if_fail (self != NULL);
	g_return_if_fail (label != NULL);
	_tmp0_ = self->priv->labels;
	_tmp1_ = vala_collection_get_size ((ValaCollection*) _tmp0_);
	_tmp2_ = _tmp1_;
	if (_tmp2_ == 0) {
		ValaSourceReference* _tmp3_;
		ValaSourceReference* _tmp4_;
		_tmp3_ = vala_code_node_get_source_reference ((ValaCodeNode*) label);
		_tmp4_ = _tmp3_;
		vala_code_node_set_source_reference ((ValaCodeNode*) self, _tmp4_);
	}
	_tmp5_ = self->priv->labels;
	vala_collection_add ((ValaCollection*) _tmp5_, label);
	vala_switch_label_set_section (label, self);
}


/**
 * Returns a copy of the list of switch labels.
 *
 * @return switch label list
 */
static gpointer
_vala_iterable_ref0 (gpointer self)
{
	return self ? vala_iterable_ref (self) : NULL;
}


ValaList*
vala_switch_section_get_labels (ValaSwitchSection* self)
{
	ValaList* result = NULL;
	ValaList* _tmp0_;
	ValaList* _tmp1_;
	g_return_val_if_fail (self != NULL, NULL);
	_tmp0_ = self->priv->labels;
	_tmp1_ = _vala_iterable_ref0 (_tmp0_);
	result = _tmp1_;
	return result;
}


gboolean
vala_switch_section_has_default_label (ValaSwitchSection* self)
{
	gboolean result = FALSE;
	g_return_val_if_fail (self != NULL, FALSE);
	{
		ValaList* _label_list = NULL;
		ValaList* _tmp0_;
		ValaList* _tmp1_;
		gint _label_size = 0;
		ValaList* _tmp2_;
		gint _tmp3_;
		gint _tmp4_;
		gint _label_index = 0;
		_tmp0_ = self->priv->labels;
		_tmp1_ = _vala_iterable_ref0 (_tmp0_);
		_label_list = _tmp1_;
		_tmp2_ = _label_list;
		_tmp3_ = vala_collection_get_size ((ValaCollection*) _tmp2_);
		_tmp4_ = _tmp3_;
		_label_size = _tmp4_;
		_label_index = -1;
		while (TRUE) {
			gint _tmp5_;
			gint _tmp6_;
			gint _tmp7_;
			ValaSwitchLabel* label = NULL;
			ValaList* _tmp8_;
			gint _tmp9_;
			gpointer _tmp10_;
			ValaSwitchLabel* _tmp11_;
			ValaExpression* _tmp12_;
			ValaExpression* _tmp13_;
			_tmp5_ = _label_index;
			_label_index = _tmp5_ + 1;
			_tmp6_ = _label_index;
			_tmp7_ = _label_size;
			if (!(_tmp6_ < _tmp7_)) {
				break;
			}
			_tmp8_ = _label_list;
			_tmp9_ = _label_index;
			_tmp10_ = vala_list_get (_tmp8_, _tmp9_);
			label = (ValaSwitchLabel*) _tmp10_;
			_tmp11_ = label;
			_tmp12_ = vala_switch_label_get_expression (_tmp11_);
			_tmp13_ = _tmp12_;
			if (_tmp13_ == NULL) {
				result = TRUE;
				_vala_code_node_unref0 (label);
				_vala_iterable_unref0 (_label_list);
				return result;
			}
			_vala_code_node_unref0 (label);
		}
		_vala_iterable_unref0 (_label_list);
	}
	result = FALSE;
	return result;
}


static void
vala_switch_section_real_accept (ValaCodeNode* base,
                                 ValaCodeVisitor* visitor)
{
	ValaSwitchSection * self;
	self = (ValaSwitchSection*) base;
	g_return_if_fail (visitor != NULL);
	vala_code_visitor_visit_switch_section (visitor, self);
}


static void
vala_switch_section_real_accept_children (ValaCodeNode* base,
                                          ValaCodeVisitor* visitor)
{
	ValaSwitchSection * self;
	self = (ValaSwitchSection*) base;
	g_return_if_fail (visitor != NULL);
	{
		ValaList* _label_list = NULL;
		ValaList* _tmp0_;
		ValaList* _tmp1_;
		gint _label_size = 0;
		ValaList* _tmp2_;
		gint _tmp3_;
		gint _tmp4_;
		gint _label_index = 0;
		_tmp0_ = self->priv->labels;
		_tmp1_ = _vala_iterable_ref0 (_tmp0_);
		_label_list = _tmp1_;
		_tmp2_ = _label_list;
		_tmp3_ = vala_collection_get_size ((ValaCollection*) _tmp2_);
		_tmp4_ = _tmp3_;
		_label_size = _tmp4_;
		_label_index = -1;
		while (TRUE) {
			gint _tmp5_;
			gint _tmp6_;
			gint _tmp7_;
			ValaSwitchLabel* label = NULL;
			ValaList* _tmp8_;
			gint _tmp9_;
			gpointer _tmp10_;
			ValaSwitchLabel* _tmp11_;
			_tmp5_ = _label_index;
			_label_index = _tmp5_ + 1;
			_tmp6_ = _label_index;
			_tmp7_ = _label_size;
			if (!(_tmp6_ < _tmp7_)) {
				break;
			}
			_tmp8_ = _label_list;
			_tmp9_ = _label_index;
			_tmp10_ = vala_list_get (_tmp8_, _tmp9_);
			label = (ValaSwitchLabel*) _tmp10_;
			_tmp11_ = label;
			vala_code_node_accept ((ValaCodeNode*) _tmp11_, visitor);
			_vala_code_node_unref0 (label);
		}
		_vala_iterable_unref0 (_label_list);
	}
	{
		ValaList* _st_list = NULL;
		ValaList* _tmp12_;
		gint _st_size = 0;
		ValaList* _tmp13_;
		gint _tmp14_;
		gint _tmp15_;
		gint _st_index = 0;
		_tmp12_ = vala_block_get_statements ((ValaBlock*) self);
		_st_list = _tmp12_;
		_tmp13_ = _st_list;
		_tmp14_ = vala_collection_get_size ((ValaCollection*) _tmp13_);
		_tmp15_ = _tmp14_;
		_st_size = _tmp15_;
		_st_index = -1;
		while (TRUE) {
			gint _tmp16_;
			gint _tmp17_;
			gint _tmp18_;
			ValaStatement* st = NULL;
			ValaList* _tmp19_;
			gint _tmp20_;
			gpointer _tmp21_;
			ValaStatement* _tmp22_;
			_tmp16_ = _st_index;
			_st_index = _tmp16_ + 1;
			_tmp17_ = _st_index;
			_tmp18_ = _st_size;
			if (!(_tmp17_ < _tmp18_)) {
				break;
			}
			_tmp19_ = _st_list;
			_tmp20_ = _st_index;
			_tmp21_ = vala_list_get (_tmp19_, _tmp20_);
			st = (ValaStatement*) _tmp21_;
			_tmp22_ = st;
			vala_code_node_accept ((ValaCodeNode*) _tmp22_, visitor);
			_vala_code_node_unref0 (st);
		}
		_vala_iterable_unref0 (_st_list);
	}
}


static gpointer
_vala_code_node_ref0 (gpointer self)
{
	return self ? vala_code_node_ref (self) : NULL;
}


static gboolean
vala_switch_section_real_check (ValaCodeNode* base,
                                ValaCodeContext* context)
{
	ValaSwitchSection * self;
	gboolean result = FALSE;
	gboolean _tmp0_;
	gboolean _tmp1_;
	ValaSemanticAnalyzer* _tmp15_;
	ValaSemanticAnalyzer* _tmp16_;
	ValaSymbol* _tmp17_;
	ValaSymbol* _tmp18_;
	ValaScope* _tmp19_;
	ValaScope* _tmp20_;
	ValaSymbol* old_symbol = NULL;
	ValaSemanticAnalyzer* _tmp21_;
	ValaSemanticAnalyzer* _tmp22_;
	ValaSymbol* _tmp23_;
	ValaSymbol* _tmp24_;
	ValaSymbol* _tmp25_;
	ValaBlock* old_insert_block = NULL;
	ValaSemanticAnalyzer* _tmp26_;
	ValaSemanticAnalyzer* _tmp27_;
	ValaBlock* _tmp28_;
	ValaBlock* _tmp29_;
	ValaSemanticAnalyzer* _tmp30_;
	ValaSemanticAnalyzer* _tmp31_;
	ValaSemanticAnalyzer* _tmp32_;
	ValaSemanticAnalyzer* _tmp33_;
	ValaBlock* _tmp34_;
	ValaSemanticAnalyzer* _tmp70_;
	ValaSemanticAnalyzer* _tmp71_;
	ValaSymbol* _tmp72_;
	ValaSemanticAnalyzer* _tmp73_;
	ValaSemanticAnalyzer* _tmp74_;
	ValaBlock* _tmp75_;
	ValaBlock* _tmp76_;
	gboolean _tmp77_;
	gboolean _tmp78_;
	self = (ValaSwitchSection*) base;
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
	{
		ValaList* _label_list = NULL;
		ValaList* _tmp4_;
		gint _label_size = 0;
		ValaList* _tmp5_;
		gint _tmp6_;
		gint _tmp7_;
		gint _label_index = 0;
		_tmp4_ = vala_switch_section_get_labels (self);
		_label_list = _tmp4_;
		_tmp5_ = _label_list;
		_tmp6_ = vala_collection_get_size ((ValaCollection*) _tmp5_);
		_tmp7_ = _tmp6_;
		_label_size = _tmp7_;
		_label_index = -1;
		while (TRUE) {
			gint _tmp8_;
			gint _tmp9_;
			gint _tmp10_;
			ValaSwitchLabel* label = NULL;
			ValaList* _tmp11_;
			gint _tmp12_;
			gpointer _tmp13_;
			ValaSwitchLabel* _tmp14_;
			_tmp8_ = _label_index;
			_label_index = _tmp8_ + 1;
			_tmp9_ = _label_index;
			_tmp10_ = _label_size;
			if (!(_tmp9_ < _tmp10_)) {
				break;
			}
			_tmp11_ = _label_list;
			_tmp12_ = _label_index;
			_tmp13_ = vala_list_get (_tmp11_, _tmp12_);
			label = (ValaSwitchLabel*) _tmp13_;
			_tmp14_ = label;
			vala_code_node_check ((ValaCodeNode*) _tmp14_, context);
			_vala_code_node_unref0 (label);
		}
		_vala_iterable_unref0 (_label_list);
	}
	_tmp15_ = vala_code_context_get_analyzer (context);
	_tmp16_ = _tmp15_;
	_tmp17_ = vala_semantic_analyzer_get_current_symbol (_tmp16_);
	_tmp18_ = _tmp17_;
	_tmp19_ = vala_symbol_get_scope (_tmp18_);
	_tmp20_ = _tmp19_;
	vala_symbol_set_owner ((ValaSymbol*) self, _tmp20_);
	_tmp21_ = vala_code_context_get_analyzer (context);
	_tmp22_ = _tmp21_;
	_tmp23_ = vala_semantic_analyzer_get_current_symbol (_tmp22_);
	_tmp24_ = _tmp23_;
	_tmp25_ = _vala_code_node_ref0 (_tmp24_);
	old_symbol = _tmp25_;
	_tmp26_ = vala_code_context_get_analyzer (context);
	_tmp27_ = _tmp26_;
	_tmp28_ = _tmp27_->insert_block;
	_tmp29_ = _vala_code_node_ref0 (_tmp28_);
	old_insert_block = _tmp29_;
	_tmp30_ = vala_code_context_get_analyzer (context);
	_tmp31_ = _tmp30_;
	vala_semantic_analyzer_set_current_symbol (_tmp31_, (ValaSymbol*) self);
	_tmp32_ = vala_code_context_get_analyzer (context);
	_tmp33_ = _tmp32_;
	_tmp34_ = _vala_code_node_ref0 ((ValaBlock*) self);
	_vala_code_node_unref0 (_tmp33_->insert_block);
	_tmp33_->insert_block = _tmp34_;
	{
		ValaList* _st_list = NULL;
		ValaList* _tmp35_;
		gint _st_size = 0;
		ValaList* _tmp36_;
		gint _tmp37_;
		gint _tmp38_;
		gint _st_index = 0;
		_tmp35_ = vala_block_get_statements ((ValaBlock*) self);
		_st_list = _tmp35_;
		_tmp36_ = _st_list;
		_tmp37_ = vala_collection_get_size ((ValaCollection*) _tmp36_);
		_tmp38_ = _tmp37_;
		_st_size = _tmp38_;
		_st_index = -1;
		while (TRUE) {
			gint _tmp39_;
			gint _tmp40_;
			gint _tmp41_;
			ValaStatement* st = NULL;
			ValaList* _tmp42_;
			gint _tmp43_;
			gpointer _tmp44_;
			ValaStatement* _tmp45_;
			_tmp39_ = _st_index;
			_st_index = _tmp39_ + 1;
			_tmp40_ = _st_index;
			_tmp41_ = _st_size;
			if (!(_tmp40_ < _tmp41_)) {
				break;
			}
			_tmp42_ = _st_list;
			_tmp43_ = _st_index;
			_tmp44_ = vala_list_get (_tmp42_, _tmp43_);
			st = (ValaStatement*) _tmp44_;
			_tmp45_ = st;
			vala_code_node_check ((ValaCodeNode*) _tmp45_, context);
			_vala_code_node_unref0 (st);
		}
		_vala_iterable_unref0 (_st_list);
	}
	{
		ValaList* _local_list = NULL;
		ValaList* _tmp46_;
		gint _local_size = 0;
		ValaList* _tmp47_;
		gint _tmp48_;
		gint _tmp49_;
		gint _local_index = 0;
		_tmp46_ = vala_block_get_local_variables ((ValaBlock*) self);
		_local_list = _tmp46_;
		_tmp47_ = _local_list;
		_tmp48_ = vala_collection_get_size ((ValaCollection*) _tmp47_);
		_tmp49_ = _tmp48_;
		_local_size = _tmp49_;
		_local_index = -1;
		while (TRUE) {
			gint _tmp50_;
			gint _tmp51_;
			gint _tmp52_;
			ValaLocalVariable* local = NULL;
			ValaList* _tmp53_;
			gint _tmp54_;
			gpointer _tmp55_;
			ValaLocalVariable* _tmp56_;
			_tmp50_ = _local_index;
			_local_index = _tmp50_ + 1;
			_tmp51_ = _local_index;
			_tmp52_ = _local_size;
			if (!(_tmp51_ < _tmp52_)) {
				break;
			}
			_tmp53_ = _local_list;
			_tmp54_ = _local_index;
			_tmp55_ = vala_list_get (_tmp53_, _tmp54_);
			local = (ValaLocalVariable*) _tmp55_;
			_tmp56_ = local;
			vala_symbol_set_active ((ValaSymbol*) _tmp56_, FALSE);
			_vala_code_node_unref0 (local);
		}
		_vala_iterable_unref0 (_local_list);
	}
	{
		ValaList* _stmt_list = NULL;
		ValaList* _tmp57_;
		gint _stmt_size = 0;
		ValaList* _tmp58_;
		gint _tmp59_;
		gint _tmp60_;
		gint _stmt_index = 0;
		_tmp57_ = vala_block_get_statements ((ValaBlock*) self);
		_stmt_list = _tmp57_;
		_tmp58_ = _stmt_list;
		_tmp59_ = vala_collection_get_size ((ValaCollection*) _tmp58_);
		_tmp60_ = _tmp59_;
		_stmt_size = _tmp60_;
		_stmt_index = -1;
		while (TRUE) {
			gint _tmp61_;
			gint _tmp62_;
			gint _tmp63_;
			ValaStatement* stmt = NULL;
			ValaList* _tmp64_;
			gint _tmp65_;
			gpointer _tmp66_;
			ValaStatement* _tmp67_;
			ValaList* _tmp68_;
			ValaList* _tmp69_;
			_tmp61_ = _stmt_index;
			_stmt_index = _tmp61_ + 1;
			_tmp62_ = _stmt_index;
			_tmp63_ = _stmt_size;
			if (!(_tmp62_ < _tmp63_)) {
				break;
			}
			_tmp64_ = _stmt_list;
			_tmp65_ = _stmt_index;
			_tmp66_ = vala_list_get (_tmp64_, _tmp65_);
			stmt = (ValaStatement*) _tmp66_;
			_tmp67_ = stmt;
			_tmp68_ = vala_code_node_get_error_types ((ValaCodeNode*) _tmp67_);
			_tmp69_ = _tmp68_;
			vala_code_node_add_error_types ((ValaCodeNode*) self, _tmp69_);
			_vala_iterable_unref0 (_tmp69_);
			_vala_code_node_unref0 (stmt);
		}
		_vala_iterable_unref0 (_stmt_list);
	}
	_tmp70_ = vala_code_context_get_analyzer (context);
	_tmp71_ = _tmp70_;
	_tmp72_ = old_symbol;
	vala_semantic_analyzer_set_current_symbol (_tmp71_, _tmp72_);
	_tmp73_ = vala_code_context_get_analyzer (context);
	_tmp74_ = _tmp73_;
	_tmp75_ = old_insert_block;
	_tmp76_ = _vala_code_node_ref0 (_tmp75_);
	_vala_code_node_unref0 (_tmp74_->insert_block);
	_tmp74_->insert_block = _tmp76_;
	_tmp77_ = vala_code_node_get_error ((ValaCodeNode*) self);
	_tmp78_ = _tmp77_;
	result = !_tmp78_;
	_vala_code_node_unref0 (old_insert_block);
	_vala_code_node_unref0 (old_symbol);
	return result;
}


static void
vala_switch_section_real_emit (ValaCodeNode* base,
                               ValaCodeGenerator* codegen)
{
	ValaSwitchSection * self;
	self = (ValaSwitchSection*) base;
	g_return_if_fail (codegen != NULL);
	{
		ValaList* _label_list = NULL;
		ValaList* _tmp0_;
		ValaList* _tmp1_;
		gint _label_size = 0;
		ValaList* _tmp2_;
		gint _tmp3_;
		gint _tmp4_;
		gint _label_index = 0;
		_tmp0_ = self->priv->labels;
		_tmp1_ = _vala_iterable_ref0 (_tmp0_);
		_label_list = _tmp1_;
		_tmp2_ = _label_list;
		_tmp3_ = vala_collection_get_size ((ValaCollection*) _tmp2_);
		_tmp4_ = _tmp3_;
		_label_size = _tmp4_;
		_label_index = -1;
		while (TRUE) {
			gint _tmp5_;
			gint _tmp6_;
			gint _tmp7_;
			ValaSwitchLabel* label = NULL;
			ValaList* _tmp8_;
			gint _tmp9_;
			gpointer _tmp10_;
			ValaSwitchLabel* _tmp11_;
			_tmp5_ = _label_index;
			_label_index = _tmp5_ + 1;
			_tmp6_ = _label_index;
			_tmp7_ = _label_size;
			if (!(_tmp6_ < _tmp7_)) {
				break;
			}
			_tmp8_ = _label_list;
			_tmp9_ = _label_index;
			_tmp10_ = vala_list_get (_tmp8_, _tmp9_);
			label = (ValaSwitchLabel*) _tmp10_;
			_tmp11_ = label;
			vala_code_node_emit ((ValaCodeNode*) _tmp11_, codegen);
			_vala_code_node_unref0 (label);
		}
		_vala_iterable_unref0 (_label_list);
	}
	VALA_CODE_NODE_CLASS (vala_switch_section_parent_class)->emit ((ValaCodeNode*) G_TYPE_CHECK_INSTANCE_CAST (self, VALA_TYPE_BLOCK, ValaBlock), codegen);
}


static void
vala_switch_section_class_init (ValaSwitchSectionClass * klass)
{
	vala_switch_section_parent_class = g_type_class_peek_parent (klass);
	((ValaCodeNodeClass *) klass)->finalize = vala_switch_section_finalize;
	g_type_class_adjust_private_offset (klass, &ValaSwitchSection_private_offset);
	((ValaCodeNodeClass *) klass)->accept = (void (*) (ValaCodeNode *, ValaCodeVisitor*)) vala_switch_section_real_accept;
	((ValaCodeNodeClass *) klass)->accept_children = (void (*) (ValaCodeNode *, ValaCodeVisitor*)) vala_switch_section_real_accept_children;
	((ValaCodeNodeClass *) klass)->check = (gboolean (*) (ValaCodeNode *, ValaCodeContext*)) vala_switch_section_real_check;
	((ValaCodeNodeClass *) klass)->emit = (void (*) (ValaCodeNode *, ValaCodeGenerator*)) vala_switch_section_real_emit;
}


static void
vala_switch_section_instance_init (ValaSwitchSection * self)
{
	GEqualFunc _tmp0_;
	ValaArrayList* _tmp1_;
	self->priv = vala_switch_section_get_instance_private (self);
	_tmp0_ = g_direct_equal;
	_tmp1_ = vala_array_list_new (VALA_TYPE_SWITCH_LABEL, (GBoxedCopyFunc) vala_code_node_ref, (GDestroyNotify) vala_code_node_unref, _tmp0_);
	self->priv->labels = (ValaList*) _tmp1_;
}


static void
vala_switch_section_finalize (ValaCodeNode * obj)
{
	ValaSwitchSection * self;
	self = G_TYPE_CHECK_INSTANCE_CAST (obj, VALA_TYPE_SWITCH_SECTION, ValaSwitchSection);
	_vala_iterable_unref0 (self->priv->labels);
	VALA_CODE_NODE_CLASS (vala_switch_section_parent_class)->finalize (obj);
}


/**
 * Represents a switch section in the source code.
 */
GType
vala_switch_section_get_type (void)
{
	static volatile gsize vala_switch_section_type_id__volatile = 0;
	if (g_once_init_enter (&vala_switch_section_type_id__volatile)) {
		static const GTypeInfo g_define_type_info = { sizeof (ValaSwitchSectionClass), (GBaseInitFunc) NULL, (GBaseFinalizeFunc) NULL, (GClassInitFunc) vala_switch_section_class_init, (GClassFinalizeFunc) NULL, NULL, sizeof (ValaSwitchSection), 0, (GInstanceInitFunc) vala_switch_section_instance_init, NULL };
		GType vala_switch_section_type_id;
		vala_switch_section_type_id = g_type_register_static (VALA_TYPE_BLOCK, "ValaSwitchSection", &g_define_type_info, 0);
		ValaSwitchSection_private_offset = g_type_add_instance_private (vala_switch_section_type_id, sizeof (ValaSwitchSectionPrivate));
		g_once_init_leave (&vala_switch_section_type_id__volatile, vala_switch_section_type_id);
	}
	return vala_switch_section_type_id__volatile;
}


