/*
 * mate-keyring
 *
 * Copyright (C) 2008 Stefan Walter
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

#ifndef __GKM_MATE2_PUBLIC_KEY_H__
#define __GKM_MATE2_PUBLIC_KEY_H__

#include <glib-object.h>

#include "gkm/gkm-public-xsa-key.h"

#define GKM_FACTORY_MATE2_PUBLIC_KEY            (gkm_mate2_public_key_get_factory ())

#define GKM_TYPE_MATE2_PUBLIC_KEY               (gkm_mate2_public_key_get_type ())
#define GKM_MATE2_PUBLIC_KEY(obj)               (G_TYPE_CHECK_INSTANCE_CAST ((obj), GKM_TYPE_MATE2_PUBLIC_KEY, GkmMate2PublicKey))
#define GKM_MATE2_PUBLIC_KEY_CLASS(klass)       (G_TYPE_CHECK_CLASS_CAST ((klass), GKM_TYPE_MATE2_PUBLIC_KEY, GkmMate2PublicKeyClass))
#define GKM_IS_MATE2_PUBLIC_KEY(obj)            (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GKM_TYPE_MATE2_PUBLIC_KEY))
#define GKM_IS_MATE2_PUBLIC_KEY_CLASS(klass)    (G_TYPE_CHECK_CLASS_TYPE ((klass), GKM_TYPE_MATE2_PUBLIC_KEY))
#define GKM_MATE2_PUBLIC_KEY_GET_CLASS(obj)     (G_TYPE_INSTANCE_GET_CLASS ((obj), GKM_TYPE_MATE2_PUBLIC_KEY, GkmMate2PublicKeyClass))

typedef struct _GkmMate2PublicKey GkmMate2PublicKey;
typedef struct _GkmMate2PublicKeyClass GkmMate2PublicKeyClass;

struct _GkmMate2PublicKeyClass {
	GkmPublicXsaKeyClass parent_class;
};

GType                gkm_mate2_public_key_get_type               (void);

GkmFactory*          gkm_mate2_public_key_get_factory            (void);

GkmMate2PublicKey*    gkm_mate2_public_key_new                    (const gchar *unique);

#endif /* __GKM_MATE2_PUBLIC_KEY_H__ */
