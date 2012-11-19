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

#ifndef __GKM_ROOTS_CERTIFICATE_H__
#define __GKM_ROOTS_CERTIFICATE_H__

#include <glib-object.h>

#include "gkm/gkm-certificate.h"
#include "gkm-roots-trust.h"

#define GKM_TYPE_ROOTS_CERTIFICATE               (gkm_roots_certificate_get_type ())
#define GKM_ROOTS_CERTIFICATE(obj)               (G_TYPE_CHECK_INSTANCE_CAST ((obj), GKM_TYPE_ROOTS_CERTIFICATE, GkmRootsCertificate))
#define GKM_ROOTS_CERTIFICATE_CLASS(klass)       (G_TYPE_CHECK_CLASS_CAST ((klass), GKM_TYPE_ROOTS_CERTIFICATE, GkmRootsCertificateClass))
#define GKM_IS_ROOTS_CERTIFICATE(obj)            (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GKM_TYPE_ROOTS_CERTIFICATE))
#define GKM_IS_ROOTS_CERTIFICATE_CLASS(klass)    (G_TYPE_CHECK_CLASS_TYPE ((klass), GKM_TYPE_ROOTS_CERTIFICATE))
#define GKM_ROOTS_CERTIFICATE_GET_CLASS(obj)     (G_TYPE_INSTANCE_GET_CLASS ((obj), GKM_TYPE_ROOTS_CERTIFICATE, GkmRootsCertificateClass))

typedef struct _GkmRootsCertificate GkmRootsCertificate;
typedef struct _GkmRootsCertificateClass GkmRootsCertificateClass;

struct _GkmRootsCertificateClass {
	GkmCertificateClass parent_class;
};

GType                 gkm_roots_certificate_get_type               (void);

GkmRootsCertificate*  gkm_roots_certificate_new                    (GkmModule *module,
                                                                    const gchar *hash,
                                                                    const gchar *path);

const gchar*          gkm_roots_certificate_get_unique             (GkmRootsCertificate *self);

const gchar*          gkm_roots_certificate_get_path               (GkmRootsCertificate *self);

#endif /* __GKM_ROOTS_CERTIFICATE_H__ */
