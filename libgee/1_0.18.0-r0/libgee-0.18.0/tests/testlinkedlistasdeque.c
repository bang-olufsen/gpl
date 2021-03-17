/* testlinkedlistasdeque.c generated by valac 0.26.0, the Vala compiler
 * generated from testlinkedlistasdeque.vala, do not modify */

/* testlinkedlistasdeque.vala
 *
 * Copyright (C) 2009  Didier Villevalois
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
 * Authors:
 * 	Didier 'Ptitjes Villevalois <ptitjes@free.fr>
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

#define TYPE_QUEUE_TESTS (queue_tests_get_type ())
#define QUEUE_TESTS(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), TYPE_QUEUE_TESTS, QueueTests))
#define QUEUE_TESTS_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), TYPE_QUEUE_TESTS, QueueTestsClass))
#define IS_QUEUE_TESTS(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TYPE_QUEUE_TESTS))
#define IS_QUEUE_TESTS_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), TYPE_QUEUE_TESTS))
#define QUEUE_TESTS_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), TYPE_QUEUE_TESTS, QueueTestsClass))

typedef struct _QueueTests QueueTests;
typedef struct _QueueTestsClass QueueTestsClass;
typedef struct _QueueTestsPrivate QueueTestsPrivate;

#define TYPE_DEQUE_TESTS (deque_tests_get_type ())
#define DEQUE_TESTS(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), TYPE_DEQUE_TESTS, DequeTests))
#define DEQUE_TESTS_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), TYPE_DEQUE_TESTS, DequeTestsClass))
#define IS_DEQUE_TESTS(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TYPE_DEQUE_TESTS))
#define IS_DEQUE_TESTS_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), TYPE_DEQUE_TESTS))
#define DEQUE_TESTS_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), TYPE_DEQUE_TESTS, DequeTestsClass))

typedef struct _DequeTests DequeTests;
typedef struct _DequeTestsClass DequeTestsClass;
typedef struct _DequeTestsPrivate DequeTestsPrivate;

#define TYPE_LINKED_LIST_AS_DEQUE_TESTS (linked_list_as_deque_tests_get_type ())
#define LINKED_LIST_AS_DEQUE_TESTS(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), TYPE_LINKED_LIST_AS_DEQUE_TESTS, LinkedListAsDequeTests))
#define LINKED_LIST_AS_DEQUE_TESTS_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), TYPE_LINKED_LIST_AS_DEQUE_TESTS, LinkedListAsDequeTestsClass))
#define IS_LINKED_LIST_AS_DEQUE_TESTS(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TYPE_LINKED_LIST_AS_DEQUE_TESTS))
#define IS_LINKED_LIST_AS_DEQUE_TESTS_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), TYPE_LINKED_LIST_AS_DEQUE_TESTS))
#define LINKED_LIST_AS_DEQUE_TESTS_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), TYPE_LINKED_LIST_AS_DEQUE_TESTS, LinkedListAsDequeTestsClass))

typedef struct _LinkedListAsDequeTests LinkedListAsDequeTests;
typedef struct _LinkedListAsDequeTestsClass LinkedListAsDequeTestsClass;
typedef struct _LinkedListAsDequeTestsPrivate LinkedListAsDequeTestsPrivate;
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

struct _QueueTests {
	CollectionTests parent_instance;
	QueueTestsPrivate * priv;
};

struct _QueueTestsClass {
	CollectionTestsClass parent_class;
};

struct _DequeTests {
	QueueTests parent_instance;
	DequeTestsPrivate * priv;
};

struct _DequeTestsClass {
	QueueTestsClass parent_class;
};

struct _LinkedListAsDequeTests {
	DequeTests parent_instance;
	LinkedListAsDequeTestsPrivate * priv;
};

struct _LinkedListAsDequeTestsClass {
	DequeTestsClass parent_class;
};

typedef void (*GeeTestCaseTestMethod) (void* user_data);

static gpointer linked_list_as_deque_tests_parent_class = NULL;

GType gee_test_case_get_type (void) G_GNUC_CONST;
GType collection_tests_get_type (void) G_GNUC_CONST;
GType queue_tests_get_type (void) G_GNUC_CONST;
GType deque_tests_get_type (void) G_GNUC_CONST;
GType linked_list_as_deque_tests_get_type (void) G_GNUC_CONST;
enum  {
	LINKED_LIST_AS_DEQUE_TESTS_DUMMY_PROPERTY
};
LinkedListAsDequeTests* linked_list_as_deque_tests_new (void);
LinkedListAsDequeTests* linked_list_as_deque_tests_construct (GType object_type);
DequeTests* deque_tests_construct (GType object_type, const gchar* name);
void gee_test_case_add_test (GeeTestCase* self, const gchar* name, GeeTestCaseTestMethod test, void* test_target, GDestroyNotify test_target_destroy_notify);
static void linked_list_as_deque_tests_test_selected_functions (LinkedListAsDequeTests* self);
static void _linked_list_as_deque_tests_test_selected_functions_gee_test_case_test_method (gpointer self);
static void linked_list_as_deque_tests_real_set_up (GeeTestCase* base);
static void linked_list_as_deque_tests_real_tear_down (GeeTestCase* base);


static void _linked_list_as_deque_tests_test_selected_functions_gee_test_case_test_method (gpointer self) {
	linked_list_as_deque_tests_test_selected_functions ((LinkedListAsDequeTests*) self);
}


LinkedListAsDequeTests* linked_list_as_deque_tests_construct (GType object_type) {
	LinkedListAsDequeTests * self = NULL;
	self = (LinkedListAsDequeTests*) deque_tests_construct (object_type, "LinkedList as Deque");
	gee_test_case_add_test ((GeeTestCase*) self, "[LinkedList] selected functions", _linked_list_as_deque_tests_test_selected_functions_gee_test_case_test_method, g_object_ref (self), g_object_unref);
	return self;
}


LinkedListAsDequeTests* linked_list_as_deque_tests_new (void) {
	return linked_list_as_deque_tests_construct (TYPE_LINKED_LIST_AS_DEQUE_TESTS);
}


static void linked_list_as_deque_tests_real_set_up (GeeTestCase* base) {
	LinkedListAsDequeTests * self;
	GeeLinkedList* _tmp0_ = NULL;
	self = (LinkedListAsDequeTests*) base;
	_tmp0_ = gee_linked_list_new (G_TYPE_STRING, (GBoxedCopyFunc) g_strdup, g_free, NULL, NULL, NULL);
	_g_object_unref0 (((CollectionTests*) self)->test_collection);
	((CollectionTests*) self)->test_collection = (GeeCollection*) _tmp0_;
}


static void linked_list_as_deque_tests_real_tear_down (GeeTestCase* base) {
	LinkedListAsDequeTests * self;
	self = (LinkedListAsDequeTests*) base;
	_g_object_unref0 (((CollectionTests*) self)->test_collection);
	((CollectionTests*) self)->test_collection = NULL;
}


static gpointer _g_object_ref0 (gpointer self) {
	return self ? g_object_ref (self) : NULL;
}


static void linked_list_as_deque_tests_test_selected_functions (LinkedListAsDequeTests* self) {
	GeeLinkedList* test_list = NULL;
	GeeCollection* _tmp0_ = NULL;
	GeeLinkedList* _tmp1_ = NULL;
	g_return_if_fail (self != NULL);
	_tmp0_ = ((CollectionTests*) self)->test_collection;
	_tmp1_ = _g_object_ref0 (G_TYPE_CHECK_INSTANCE_TYPE (_tmp0_, GEE_TYPE_LINKED_LIST) ? ((GeeLinkedList*) _tmp0_) : NULL);
	test_list = _tmp1_;
	_vala_assert (test_list != NULL, "test_list != null");
	_g_object_unref0 (test_list);
}


static void linked_list_as_deque_tests_class_init (LinkedListAsDequeTestsClass * klass) {
	linked_list_as_deque_tests_parent_class = g_type_class_peek_parent (klass);
	((GeeTestCaseClass *) klass)->set_up = linked_list_as_deque_tests_real_set_up;
	((GeeTestCaseClass *) klass)->tear_down = linked_list_as_deque_tests_real_tear_down;
}


static void linked_list_as_deque_tests_instance_init (LinkedListAsDequeTests * self) {
}


GType linked_list_as_deque_tests_get_type (void) {
	static volatile gsize linked_list_as_deque_tests_type_id__volatile = 0;
	if (g_once_init_enter (&linked_list_as_deque_tests_type_id__volatile)) {
		static const GTypeInfo g_define_type_info = { sizeof (LinkedListAsDequeTestsClass), (GBaseInitFunc) NULL, (GBaseFinalizeFunc) NULL, (GClassInitFunc) linked_list_as_deque_tests_class_init, (GClassFinalizeFunc) NULL, NULL, sizeof (LinkedListAsDequeTests), 0, (GInstanceInitFunc) linked_list_as_deque_tests_instance_init, NULL };
		GType linked_list_as_deque_tests_type_id;
		linked_list_as_deque_tests_type_id = g_type_register_static (TYPE_DEQUE_TESTS, "LinkedListAsDequeTests", &g_define_type_info, 0);
		g_once_init_leave (&linked_list_as_deque_tests_type_id__volatile, linked_list_as_deque_tests_type_id);
	}
	return linked_list_as_deque_tests_type_id__volatile;
}


