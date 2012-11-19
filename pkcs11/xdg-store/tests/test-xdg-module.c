/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */
/* test-xdg-module.c: A test PKCS#11 module implementation

   Copyright (C) 2010 Stefan Walter
   Copyright (C) 2010 Collabora Ltd

   The Mate Keyring Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   The Mate Keyring Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with the Mate Library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
   Boston, MA 02110-1301, USA.

   Author: Stef Walter <stef@memberwebs.com>
*/

#include "config.h"

#include "test-xdg-module.h"
#include "gkm-xdg-store.h"

#include "gkm/gkm-session.h"
#include "gkm/gkm-module.h"

#include <errno.h>
#include <sys/times.h>

#include <string.h>

static GMutex *mutex = NULL;

GkmModule*  _gkm_xdg_store_get_module_for_testing (void);
GMutex* _gkm_module_get_scary_mutex_that_you_should_not_touch (GkmModule *module);

GkmModule*
test_xdg_module_initialize_and_enter (void)
{
	CK_FUNCTION_LIST_PTR funcs;
	CK_C_INITIALIZE_ARGS args;
	GkmModule *module;
	gchar *string;
	CK_RV rv;

	/* Setup test directory to work in */
	memset (&args, 0, sizeof (args));
	string = g_strdup_printf ("directory='%s'", testing_scratch_directory ());
	args.pReserved = string;
	args.flags = CKF_OS_LOCKING_OK;

	/* Delete all files in this directory */
	testing_scratch_remove_all ();

	/* Copy files from test-data to scratch */
	testing_data_to_scratch ("test-refer-1.trust", NULL);
	testing_data_to_scratch ("test-certificate-1.cer", NULL);
	testing_scratch_empty ("invalid-without-ext");
	testing_scratch_empty ("test-file.unknown");
	testing_scratch_empty ("test-invalid.trust");

	funcs = gkm_xdg_store_get_functions ();
	rv = (funcs->C_Initialize) (&args);
	g_return_val_if_fail (rv == CKR_OK, NULL);

	module = _gkm_xdg_store_get_module_for_testing ();
	g_return_val_if_fail (module, NULL);

	mutex = _gkm_module_get_scary_mutex_that_you_should_not_touch (module);
	test_xdg_module_enter ();

	g_free (string);

	return module;
}

void
test_xdg_module_leave_and_finalize (void)
{
	CK_FUNCTION_LIST_PTR funcs;
	CK_RV rv;

	test_xdg_module_leave ();

	funcs = gkm_xdg_store_get_functions ();
	rv = (funcs->C_Finalize) (NULL);
	g_return_if_fail (rv == CKR_OK);
}

void
test_xdg_module_leave (void)
{
	g_assert (mutex);
	g_mutex_unlock (mutex);
}

void
test_xdg_module_enter (void)
{
	g_assert (mutex);
	g_mutex_lock (mutex);
}

GkmSession*
test_xdg_module_open_session (gboolean writable)
{
	CK_ULONG flags = CKF_SERIAL_SESSION;
	CK_SESSION_HANDLE handle;
	GkmModule *module;
	GkmSession *session;
	CK_RV rv;

	module = _gkm_xdg_store_get_module_for_testing ();
	g_return_val_if_fail (module, NULL);

	if (writable)
		flags |= CKF_RW_SESSION;

	rv = gkm_module_C_OpenSession (module, 1, flags, NULL, NULL, &handle);
	g_assert (rv == CKR_OK);

	session = gkm_module_lookup_session (module, handle);
	g_assert (session);

	return session;
}

/* --------------------------------------------------------------------------------------
 * MODULE TESTS
 */

static GkmModule *module = NULL;
static GkmSession *session = NULL;
static CK_SLOT_ID slot_id = 0;

TESTING_EXTERNAL(xdg_module)
{
	CK_FUNCTION_LIST_PTR funcs = gkm_xdg_store_get_functions ();
	testing_test_p11_module (funcs, "p11-tests.conf");
}

TESTING_SETUP(xdg_module_setup)
{
	CK_SESSION_INFO info;
	CK_RV rv;

	module = test_xdg_module_initialize_and_enter ();
	session = test_xdg_module_open_session (TRUE);

	rv = gkm_module_C_Login (module, gkm_session_get_handle (session), CKU_USER, NULL, 0);
	gkm_assert_cmprv (rv, ==, CKR_OK);

	rv = gkm_session_C_GetSessionInfo (session, &info);
	gkm_assert_cmprv (rv, ==, CKR_OK);

	slot_id = info.slotID;
}

TESTING_TEARDOWN(xdg_module_teardown)
{
	test_xdg_module_leave_and_finalize ();
	module = NULL;
	session = NULL;
}

TESTING_TEST (xdg_module_find_twice_is_same)
{
	CK_OBJECT_HANDLE objects[256];
	CK_ULONG n_objects;
	CK_ULONG n_check;
	CK_RV rv;

	rv = gkm_session_C_FindObjectsInit (session, NULL, 0);
	gkm_assert_cmprv (rv, ==, CKR_OK);
	rv = gkm_session_C_FindObjects (session, objects, G_N_ELEMENTS (objects), &n_objects);
	gkm_assert_cmprv (rv, ==, CKR_OK);
	rv = gkm_session_C_FindObjectsFinal (session);
	gkm_assert_cmprv (rv, ==, CKR_OK);

	gkm_assert_cmpulong (n_objects, >, 0);

	/* Update the time on the file */
	testing_scratch_touch ("test-refer-1.trust", 1);

	rv = gkm_session_C_FindObjectsInit (session, NULL, 0);
	gkm_assert_cmprv (rv, ==, CKR_OK);
	rv = gkm_session_C_FindObjects (session, objects, G_N_ELEMENTS (objects), &n_check);
	gkm_assert_cmprv (rv, ==, CKR_OK);
	rv = gkm_session_C_FindObjectsFinal (session);
	gkm_assert_cmprv (rv, ==, CKR_OK);

	/* Should have same objects after reload */
	gkm_assert_cmpulong (n_check, ==, n_objects);
}

TESTING_TEST (xdg_module_file_becomes_invalid)
{
	CK_OBJECT_HANDLE objects[256];
	CK_ULONG n_objects;
	CK_ULONG n_check;
	CK_RV rv;

	rv = gkm_session_C_FindObjectsInit (session, NULL, 0);
	gkm_assert_cmprv (rv, ==, CKR_OK);
	rv = gkm_session_C_FindObjects (session, objects, G_N_ELEMENTS (objects), &n_objects);
	gkm_assert_cmprv (rv, ==, CKR_OK);
	rv = gkm_session_C_FindObjectsFinal (session);
	gkm_assert_cmprv (rv, ==, CKR_OK);

	gkm_assert_cmpulong (n_objects, >, 0);

	/* Overwrite the file with empty */
	testing_scratch_empty ("test-refer-1.trust");
	testing_scratch_touch ("test-refer-1.trust", 2);

	rv = gkm_session_C_FindObjectsInit (session, NULL, 0);
	gkm_assert_cmprv (rv, ==, CKR_OK);
	rv = gkm_session_C_FindObjects (session, objects, G_N_ELEMENTS (objects), &n_check);
	gkm_assert_cmprv (rv, ==, CKR_OK);
	rv = gkm_session_C_FindObjectsFinal (session);
	gkm_assert_cmprv (rv, ==, CKR_OK);

	/* Should have less objects */
	gkm_assert_cmpulong (n_check, <, n_objects);
}

TESTING_TEST (xdg_module_file_remove)
{
	CK_OBJECT_HANDLE objects[256];
	CK_ULONG n_objects;
	CK_ULONG n_check;
	CK_RV rv;

	rv = gkm_session_C_FindObjectsInit (session, NULL, 0);
	gkm_assert_cmprv (rv, ==, CKR_OK);
	rv = gkm_session_C_FindObjects (session, objects, G_N_ELEMENTS (objects), &n_objects);
	gkm_assert_cmprv (rv, ==, CKR_OK);
	rv = gkm_session_C_FindObjectsFinal (session);
	gkm_assert_cmprv (rv, ==, CKR_OK);

	gkm_assert_cmpulong (n_objects, >, 0);

	/* This file goes away */
	testing_scratch_remove ("test-refer-1.trust");

	rv = gkm_session_C_FindObjectsInit (session, NULL, 0);
	gkm_assert_cmprv (rv, ==, CKR_OK);
	rv = gkm_session_C_FindObjects (session, objects, G_N_ELEMENTS (objects), &n_check);
	gkm_assert_cmprv (rv, ==, CKR_OK);
	rv = gkm_session_C_FindObjectsFinal (session);
	gkm_assert_cmprv (rv, ==, CKR_OK);

	/* Should have less objects */
	gkm_assert_cmpulong (n_check, <, n_objects);
}

TESTING_TEST (xdg_create_and_add_object)
{
	CK_OBJECT_HANDLE object = 0;
	CK_OBJECT_CLASS klass = CKO_CERTIFICATE;
	CK_CERTIFICATE_TYPE ctype = CKC_X_509;
	CK_BBOOL tval = CK_TRUE;
	gpointer data;
	gsize n_data;
	CK_RV rv;

	CK_ATTRIBUTE attrs[] = {
		{ CKA_VALUE, NULL, 0 },
		{ CKA_CLASS, &klass, sizeof (klass) },
		{ CKA_TOKEN, &tval, sizeof (tval) },
		{ CKA_CERTIFICATE_TYPE, &ctype, sizeof (ctype) }
	};

	data = testing_data_read ("test-certificate-2.cer", &n_data);
	attrs[0].pValue = data;
	attrs[0].ulValueLen = n_data;

	rv = gkm_session_C_CreateObject (session, attrs, G_N_ELEMENTS (attrs), &object);
	gkm_assert_cmprv (rv, ==, CKR_OK);
	gkm_assert_cmpulong (object, !=, 0);
}

TESTING_TEST (xdg_destroy_object)
{
	CK_OBJECT_HANDLE object = 0;
	CK_CERTIFICATE_TYPE ctype = CKC_X_509;
	CK_ULONG n_objects = 0;
	CK_BBOOL tval = CK_TRUE;
	CK_RV rv;

	CK_ATTRIBUTE attrs[] = {
		{ CKA_CERTIFICATE_TYPE, &ctype, sizeof (ctype) },
		{ CKA_TOKEN, &tval, sizeof (tval) }
	};

	rv = gkm_session_C_FindObjectsInit (session, attrs, G_N_ELEMENTS (attrs));
	gkm_assert_cmprv (rv, ==, CKR_OK);
	rv = gkm_session_C_FindObjects (session, &object, 1, &n_objects);
	gkm_assert_cmprv (rv, ==, CKR_OK);
	gkm_assert_cmpulong (n_objects, ==, 1);
	rv = gkm_session_C_FindObjectsFinal (session);
	gkm_assert_cmprv (rv, ==, CKR_OK);

	/* Destroy this object, which should be stored on the disk */
	rv = gkm_session_C_DestroyObject (session, object);
	gkm_assert_cmprv (rv, ==, CKR_OK);

	/* Make sure it's really gone */
	rv = gkm_session_C_DestroyObject (session, object);
	gkm_assert_cmprv (rv, ==, CKR_OBJECT_HANDLE_INVALID);
}

TESTING_TEST (xdg_get_slot_info)
{
	CK_SLOT_INFO info;
	const gchar *str;
	CK_RV rv;

	rv = gkm_module_C_GetSlotInfo (module, slot_id, &info);
	gkm_assert_cmprv (rv, ==, CKR_OK);

	str = g_strstr_len ((gchar*)info.slotDescription, sizeof (info.slotDescription),
	                    "User Key Storage");
	g_assert (str != NULL);
}

TESTING_TEST (xdg_get_token_info)
{
	CK_TOKEN_INFO info;
	const gchar *str;
	CK_RV rv;

	rv = gkm_module_C_GetTokenInfo (module, slot_id, &info);
	gkm_assert_cmprv (rv, ==, CKR_OK);

	str = g_strstr_len ((gchar*)info.label, sizeof (info.label),
	                    "User Key Storage");
	g_assert (str != NULL);
}
