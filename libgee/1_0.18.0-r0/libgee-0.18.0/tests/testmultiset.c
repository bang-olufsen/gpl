/* testmultiset.c generated by valac 0.26.0, the Vala compiler
 * generated from testmultiset.vala, do not modify */

/* testmultiset.vala
 *
 * Copyright (C) 2008  Jürg Billeter
 * Copyright (C) 2009  Didier Villevalois, Julien Peeters
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
 * 	Didier 'Ptitjes' Villevalois <ptitjes@free.fr>
 * 	Julien Peeters <contact@julienpeeters.fr>
 */

#include <glib.h>
#include <glib-object.h>
#include <gee.h>
#include <stdlib.h>
#include <string.h>


#define GEE_TYPE_TEST_CASE (gee_test_case_get_type ())
#define GEE_TEST_CASE(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), GEE_TYPE_TEST_CASE, GeeTestCase))
#define GEE_TEST_CASE_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), GEE_TYPE_TEST_CASE, GeeTestCaseClass))
#define GEE_IS_TEST_CASE(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GEE_TYPE_TEST_CASE))
#define GEE_IS_TEST_CASE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GEE_TYPE_TEST_CASE))
#define GEE_TEST_CASE_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), GEE_TYPE_TEST_CASE, GeeTestCaseClass))

typedef struct _GeeTestCase GeeTestCase;
typedef struct _GeeTestCaseClass GeeTestCaseClass;
typedef struct _GeeTestCasePrivate GeeTestCasePrivate;

#define TYPE_COLLECTION_TESTS (collection_tests_get_type ())
#define COLLECTION_TESTS(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), TYPE_COLLECTION_TESTS, CollectionTests))
#define COLLECTION_TESTS_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), TYPE_COLLECTION_TESTS, CollectionTestsClass))
#define IS_COLLECTION_TESTS(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TYPE_COLLECTION_TESTS))
#define IS_COLLECTION_TESTS_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), TYPE_COLLECTION_TESTS))
#define COLLECTION_TESTS_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), TYPE_COLLECTION_TESTS, CollectionTestsClass))

typedef struct _CollectionTests CollectionTests;
typedef struct _CollectionTestsClass CollectionTestsClass;
typedef struct _CollectionTestsPrivate CollectionTestsPrivate;

#define TYPE_MULTI_SET_TESTS (multi_set_tests_get_type ())
#define MULTI_SET_TESTS(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), TYPE_MULTI_SET_TESTS, MultiSetTests))
#define MULTI_SET_TESTS_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), TYPE_MULTI_SET_TESTS, MultiSetTestsClass))
#define IS_MULTI_SET_TESTS(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TYPE_MULTI_SET_TESTS))
#define IS_MULTI_SET_TESTS_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), TYPE_MULTI_SET_TESTS))
#define MULTI_SET_TESTS_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), TYPE_MULTI_SET_TESTS, MultiSetTestsClass))

typedef struct _MultiSetTests MultiSetTests;
typedef struct _MultiSetTestsClass MultiSetTestsClass;
typedef struct _MultiSetTestsPrivate MultiSetTestsPrivate;
#define _g_object_unref0(var) ((var == NULL) ? NULL : (var = (g_object_unref (var), NULL)))
#define _vala_assert(expr, msg) if G_LIKELY (expr) ; else g_assertion_message_expr (G_LOG_DOMAIN, __FILE__, __LINE__, G_STRFUNC, msg);

struct _GeeTestCase {
	GObject parent_instance;
	GeeTestCasePrivate * priv;
};

struct _GeeTestCaseClass {
	GObjectClass parent_class;
	void (*set_up) (GeeTestCase* self);
	void (*tear_down) (GeeTestCase* self);
};

struct _CollectionTests {
	GeeTestCase parent_instance;
	CollectionTestsPrivate * priv;
	GeeCollection* test_collection;
};

struct _CollectionTestsClass {
	GeeTestCaseClass parent_class;
};

struct _MultiSetTests {
	CollectionTests parent_instance;
	MultiSetTestsPrivate * priv;
};

struct _MultiSetTestsClass {
	CollectionTestsClass parent_class;
};

typedef void (*GeeTestCaseTestMethod) (void* user_data);

static gpointer multi_set_tests_parent_class = NULL;

GType gee_test_case_get_type (void) G_GNUC_CONST;
GType collection_tests_get_type (void) G_GNUC_CONST;
GType multi_set_tests_get_type (void) G_GNUC_CONST;
enum  {
	MULTI_SET_TESTS_DUMMY_PROPERTY
};
MultiSetTests* multi_set_tests_construct (GType object_type, const gchar* name);
CollectionTests* collection_tests_construct (GType object_type, const gchar* name);
void gee_test_case_add_test (GeeTestCase* self, const gchar* name, GeeTestCaseTestMethod test, void* test_target, GDestroyNotify test_target_destroy_notify);
static void multi_set_tests_test_duplicates_are_retained (MultiSetTests* self);
static void _multi_set_tests_test_duplicates_are_retained_gee_test_case_test_method (gpointer self);


static void _multi_set_tests_test_duplicates_are_retained_gee_test_case_test_method (gpointer self) {
	multi_set_tests_test_duplicates_are_retained ((MultiSetTests*) self);
}


MultiSetTests* multi_set_tests_construct (GType object_type, const gchar* name) {
	MultiSetTests * self = NULL;
	const gchar* _tmp0_ = NULL;
	g_return_val_if_fail (name != NULL, NULL);
	_tmp0_ = name;
	self = (MultiSetTests*) collection_tests_construct (object_type, _tmp0_);
	gee_test_case_add_test ((GeeTestCase*) self, "[MultiSet] duplicates are retained", _multi_set_tests_test_duplicates_are_retained_gee_test_case_test_method, g_object_ref (self), g_object_unref);
	return self;
}


static gpointer _g_object_ref0 (gpointer self) {
	return self ? g_object_ref (self) : NULL;
}


static void multi_set_tests_test_duplicates_are_retained (MultiSetTests* self) {
	GeeMultiSet* test_multi_set = NULL;
	GeeCollection* _tmp0_ = NULL;
	GeeMultiSet* _tmp1_ = NULL;
	gint _tmp2_ = 0;
	gboolean _tmp3_ = FALSE;
	gboolean _tmp4_ = FALSE;
	gint _tmp5_ = 0;
	gint _tmp6_ = 0;
	gint _tmp7_ = 0;
	gboolean _tmp8_ = FALSE;
	gboolean _tmp9_ = FALSE;
	gint _tmp10_ = 0;
	gint _tmp11_ = 0;
	gint _tmp12_ = 0;
	gboolean _tmp13_ = FALSE;
	gboolean _tmp14_ = FALSE;
	gint _tmp15_ = 0;
	gint _tmp16_ = 0;
	gint _tmp17_ = 0;
	gboolean _tmp18_ = FALSE;
	gboolean _tmp19_ = FALSE;
	gint _tmp20_ = 0;
	gint _tmp21_ = 0;
	gint _tmp22_ = 0;
	gboolean _tmp23_ = FALSE;
	gboolean _tmp24_ = FALSE;
	gint _tmp25_ = 0;
	gint _tmp26_ = 0;
	gint _tmp27_ = 0;
	gboolean _tmp28_ = FALSE;
	gboolean _tmp29_ = FALSE;
	gint _tmp30_ = 0;
	gint _tmp31_ = 0;
	gint _tmp32_ = 0;
	g_return_if_fail (self != NULL);
	_tmp0_ = ((CollectionTests*) self)->test_collection;
	_tmp1_ = _g_object_ref0 (G_TYPE_CHECK_INSTANCE_TYPE (_tmp0_, GEE_TYPE_MULTI_SET) ? ((GeeMultiSet*) _tmp0_) : NULL);
	test_multi_set = _tmp1_;
	_vala_assert (test_multi_set != NULL, "test_multi_set != null");
	_tmp2_ = gee_multi_set_count (test_multi_set, "one");
	_vala_assert (_tmp2_ == 0, "test_multi_set.count (\"one\") == 0");
	_tmp3_ = gee_collection_add ((GeeCollection*) test_multi_set, "one");
	_vala_assert (_tmp3_, "test_multi_set.add (\"one\")");
	_tmp4_ = gee_collection_contains ((GeeCollection*) test_multi_set, "one");
	_vala_assert (_tmp4_, "test_multi_set.contains (\"one\")");
	_tmp5_ = gee_collection_get_size ((GeeCollection*) test_multi_set);
	_tmp6_ = _tmp5_;
	_vala_assert (_tmp6_ == 1, "test_multi_set.size == 1");
	_tmp7_ = gee_multi_set_count (test_multi_set, "one");
	_vala_assert (_tmp7_ == 1, "test_multi_set.count (\"one\") == 1");
	_tmp8_ = gee_collection_add ((GeeCollection*) test_multi_set, "one");
	_vala_assert (_tmp8_, "test_multi_set.add (\"one\")");
	_tmp9_ = gee_collection_contains ((GeeCollection*) test_multi_set, "one");
	_vala_assert (_tmp9_, "test_multi_set.contains (\"one\")");
	_tmp10_ = gee_collection_get_size ((GeeCollection*) test_multi_set);
	_tmp11_ = _tmp10_;
	_vala_assert (_tmp11_ == 2, "test_multi_set.size == 2");
	_tmp12_ = gee_multi_set_count (test_multi_set, "one");
	_vala_assert (_tmp12_ == 2, "test_multi_set.count (\"one\") == 2");
	_tmp13_ = gee_collection_add ((GeeCollection*) test_multi_set, "one");
	_vala_assert (_tmp13_, "test_multi_set.add (\"one\")");
	_tmp14_ = gee_collection_contains ((GeeCollection*) test_multi_set, "one");
	_vala_assert (_tmp14_, "test_multi_set.contains (\"one\")");
	_tmp15_ = gee_collection_get_size ((GeeCollection*) test_multi_set);
	_tmp16_ = _tmp15_;
	_vala_assert (_tmp16_ == 3, "test_multi_set.size == 3");
	_tmp17_ = gee_multi_set_count (test_multi_set, "one");
	_vala_assert (_tmp17_ == 3, "test_multi_set.count (\"one\") == 3");
	_tmp18_ = gee_collection_remove ((GeeCollection*) test_multi_set, "one");
	_vala_assert (_tmp18_, "test_multi_set.remove (\"one\")");
	_tmp19_ = gee_collection_contains ((GeeCollection*) test_multi_set, "one");
	_vala_assert (_tmp19_, "test_multi_set.contains (\"one\")");
	_tmp20_ = gee_collection_get_size ((GeeCollection*) test_multi_set);
	_tmp21_ = _tmp20_;
	_vala_assert (_tmp21_ == 2, "test_multi_set.size == 2");
	_tmp22_ = gee_multi_set_count (test_multi_set, "one");
	_vala_assert (_tmp22_ == 2, "test_multi_set.count (\"one\") == 2");
	_tmp23_ = gee_collection_remove ((GeeCollection*) test_multi_set, "one");
	_vala_assert (_tmp23_, "test_multi_set.remove (\"one\")");
	_tmp24_ = gee_collection_contains ((GeeCollection*) test_multi_set, "one");
	_vala_assert (_tmp24_, "test_multi_set.contains (\"one\")");
	_tmp25_ = gee_collection_get_size ((GeeCollection*) test_multi_set);
	_tmp26_ = _tmp25_;
	_vala_assert (_tmp26_ == 1, "test_multi_set.size == 1");
	_tmp27_ = gee_multi_set_count (test_multi_set, "one");
	_vala_assert (_tmp27_ == 1, "test_multi_set.count (\"one\") == 1");
	_tmp28_ = gee_collection_remove ((GeeCollection*) test_multi_set, "one");
	_vala_assert (_tmp28_, "test_multi_set.remove (\"one\")");
	_tmp29_ = gee_collection_contains ((GeeCollection*) test_multi_set, "one");
	_vala_assert (!_tmp29_, "!test_multi_set.contains (\"one\")");
	_tmp30_ = gee_collection_get_size ((GeeCollection*) test_multi_set);
	_tmp31_ = _tmp30_;
	_vala_assert (_tmp31_ == 0, "test_multi_set.size == 0");
	_tmp32_ = gee_multi_set_count (test_multi_set, "one");
	_vala_assert (_tmp32_ == 0, "test_multi_set.count (\"one\") == 0");
	_g_object_unref0 (test_multi_set);
}


static void multi_set_tests_class_init (MultiSetTestsClass * klass) {
	multi_set_tests_parent_class = g_type_class_peek_parent (klass);
}


static void multi_set_tests_instance_init (MultiSetTests * self) {
}


GType multi_set_tests_get_type (void) {
	static volatile gsize multi_set_tests_type_id__volatile = 0;
	if (g_once_init_enter (&multi_set_tests_type_id__volatile)) {
		static const GTypeInfo g_define_type_info = { sizeof (MultiSetTestsClass), (GBaseInitFunc) NULL, (GBaseFinalizeFunc) NULL, (GClassInitFunc) multi_set_tests_class_init, (GClassFinalizeFunc) NULL, NULL, sizeof (MultiSetTests), 0, (GInstanceInitFunc) multi_set_tests_instance_init, NULL };
		GType multi_set_tests_type_id;
		multi_set_tests_type_id = g_type_register_static (TYPE_COLLECTION_TESTS, "MultiSetTests", &g_define_type_info, G_TYPE_FLAG_ABSTRACT);
		g_once_init_leave (&multi_set_tests_type_id__volatile, multi_set_tests_type_id);
	}
	return multi_set_tests_type_id__volatile;
}


