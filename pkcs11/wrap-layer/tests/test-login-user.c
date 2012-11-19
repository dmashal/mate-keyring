/*
 * mate-keyring
 *
 * Copyright (C) 2010 Stefan Walter
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

#include "config.h"

#include "test-suite.h"

#include "gkm/gkm-mock.h"
#include "gkm/gkm-test.h"

#include "wrap-layer/gkm-wrap-layer.h"

#include "ui/gku-prompt.h"

static CK_FUNCTION_LIST prompt_login_functions;
static CK_FUNCTION_LIST_PTR module = NULL;
static CK_SESSION_HANDLE session = 0;

TESTING_SETUP (login_user)
{
	CK_FUNCTION_LIST_PTR funcs;
	CK_SLOT_ID slot_id;
	CK_ULONG n_slots = 1;
	CK_RV rv;

	/* Always start off with test functions */
	rv = gkm_mock_C_GetFunctionList (&funcs);
	gkm_assert_cmprv (rv, ==, CKR_OK);
	memcpy (&prompt_login_functions, funcs, sizeof (prompt_login_functions));

	gkm_wrap_layer_reset_modules ();
	gkm_wrap_layer_add_module (&prompt_login_functions);
	module = gkm_wrap_layer_get_functions ();

	gku_prompt_dummy_prepare_response ();

	/* Open a session */
	rv = (module->C_Initialize) (NULL);
	gkm_assert_cmprv (rv, ==, CKR_OK);

	rv = (module->C_GetSlotList) (CK_TRUE, &slot_id, &n_slots);
	gkm_assert_cmprv (rv, ==, CKR_OK);

	rv = (module->C_OpenSession) (slot_id, CKF_SERIAL_SESSION, NULL, NULL, &session);
	gkm_assert_cmprv (rv, ==, CKR_OK);
}

TESTING_TEARDOWN (login_user)
{
	CK_RV rv;

	g_assert (!gku_prompt_dummy_have_response ());

	rv = (module->C_CloseSession) (session);
	gkm_assert_cmprv (rv, ==, CKR_OK);
	session = 0;

	rv = (module->C_Finalize) (NULL);
	gkm_assert_cmprv (rv, ==, CKR_OK);
	module = NULL;
}

TESTING_TEST (login_fail_unsupported_so)
{
	CK_RV rv;

	rv = (module->C_Login) (session, CKU_SO, NULL, 0);
	gkm_assert_cmprv (rv, ==, CKR_PIN_INCORRECT);
}

TESTING_TEST (login_skip_prompt_because_pin)
{
	CK_RV rv;

	rv = (module->C_Login) (session, CKU_USER, (guchar*)"booo", 4);
	gkm_assert_cmprv (rv, ==, CKR_OK);
}

TESTING_TEST (login_user_ok_password)
{
	CK_RV rv;

	gku_prompt_dummy_queue_ok_password ("booo");

	rv = (module->C_Login) (session, CKU_USER, NULL, 0);
	gkm_assert_cmprv (rv, ==, CKR_OK);
}

TESTING_TEST (login_user_bad_password_then_cancel)
{
	CK_RV rv;

	gku_prompt_dummy_queue_ok_password ("bad password");
	gku_prompt_dummy_queue_no ();

	rv = (module->C_Login) (session, CKU_USER, NULL, 0);
	gkm_assert_cmprv (rv, ==, CKR_PIN_INCORRECT);
}

TESTING_TEST (login_user_cancel_immediately)
{
	CK_RV rv;

	gku_prompt_dummy_queue_no ();

	rv = (module->C_Login) (session, CKU_USER, NULL, 0);
	gkm_assert_cmprv (rv, ==, CKR_PIN_INCORRECT);
}

TESTING_TEST (login_user_fail_get_session_info)
{
	CK_RV rv;

	prompt_login_functions.C_GetSessionInfo = gkm_mock_fail_C_GetSessionInfo;
	rv = (module->C_Login) (session, CKU_USER, NULL, 0);
	gkm_assert_cmprv (rv, ==, CKR_PIN_INCORRECT);
}

TESTING_TEST (login_user_fail_get_token_info)
{
	CK_RV rv;

	prompt_login_functions.C_GetTokenInfo = gkm_mock_fail_C_GetTokenInfo;
	rv = (module->C_Login) (session, CKU_USER, NULL, 0);
	gkm_assert_cmprv (rv, ==, CKR_PIN_INCORRECT);
}
