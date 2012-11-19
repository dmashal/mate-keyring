/*
 * mate-keyring
 *
 * Copyright (C) 2009 Stefan Walter
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

#include "gkm-secret-compat.h"

#include <string.h>

void
gkm_secret_compat_access_free (gpointer data)
{
	GkmSecretAccess *ac = data;
	if (ac) {
		g_free (ac->display_name);
		g_free (ac->pathname);
		g_free (ac);
	}
}

void
gkm_secret_compat_acl_free (gpointer acl)
{
	GList *l;
	for (l = acl; l; l = g_list_next (l))
		gkm_secret_compat_access_free (l->data);
	g_list_free (acl);
}

guint
gkm_secret_compat_parse_item_type (const gchar *value)
{
	if (value == NULL)
		return 0; /* The default */
	if (strcmp (value, "org.freedesktop.Secret.Generic") == 0)
		return 0; /* MATE_KEYRING_ITEM_GENERIC_SECRET */
	if (strcmp (value, "org.mate.keyring.NetworkPassword") == 0)
		return 1; /* MATE_KEYRING_ITEM_NETWORK_PASSWORD */
	if (strcmp (value, "org.mate.keyring.Note") == 0)
		return 2; /* MATE_KEYRING_ITEM_NOTE */
	if (strcmp (value, "org.mate.keyring.ChainedKeyring") == 0)
		return 3; /* MATE_KEYRING_ITEM_CHAINED_KEYRING_PASSWORD */
	if (strcmp (value, "org.mate.keyring.EncryptionKey") == 0)
		return 4; /* MATE_KEYRING_ITEM_ENCRYPTION_KEY_PASSWORD */
	if (strcmp (value, "org.mate.keyring.PkStorage") == 0)
		return 0x100; /* MATE_KEYRING_ITEM_PK_STORAGE */

	/* The default: MATE_KEYRING_ITEM_GENERIC_SECRET */
	return 0;
}

const gchar*
gkm_secret_compat_format_item_type (guint value)
{
	/* Only MATE_KEYRING_ITEM_TYPE_MASK */
	switch (value & 0x0000ffff)
	{
	case 0: /* MATE_KEYRING_ITEM_GENERIC_SECRET */
		return "org.freedesktop.Secret.Generic";
	case 1: /* MATE_KEYRING_ITEM_NETWORK_PASSWORD */
		return "org.mate.keyring.NetworkPassword";
	case 2: /* MATE_KEYRING_ITEM_NOTE */
		return "org.mate.keyring.Note";
	case 3: /* MATE_KEYRING_ITEM_CHAINED_KEYRING_PASSWORD */
		return "org.mate.keyring.ChainedKeyring";
	case 4: /* MATE_KEYRING_ITEM_ENCRYPTION_KEY_PASSWORD */
		return "org.mate.keyring.EncryptionKey";
	case 0x100: /* MATE_KEYRING_ITEM_PK_STORAGE */
		return "org.mate.keyring.PkStorage";
	default:
		return NULL;
	};
}
