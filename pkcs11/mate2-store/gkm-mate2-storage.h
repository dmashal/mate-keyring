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

#ifndef __GKM_MATE2_STORAGE_H__
#define __GKM_MATE2_STORAGE_H__

#include <glib-object.h>

#include "gkm/gkm-manager.h"
#include "gkm/gkm-secret.h"
#include "gkm/gkm-store.h"
#include "gkm/gkm-transaction.h"

#define GKM_TYPE_MATE2_STORAGE               (gkm_mate2_storage_get_type ())
#define GKM_MATE2_STORAGE(obj)               (G_TYPE_CHECK_INSTANCE_CAST ((obj), GKM_TYPE_MATE2_STORAGE, GkmMate2Storage))
#define GKM_MATE2_STORAGE_CLASS(klass)       (G_TYPE_CHECK_CLASS_CAST ((klass), GKM_TYPE_MATE2_STORAGE, GkmMate2StorageClass))
#define GKM_IS_MATE2_STORAGE(obj)            (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GKM_TYPE_MATE2_STORAGE))
#define GKM_IS_MATE2_STORAGE_CLASS(klass)    (G_TYPE_CHECK_CLASS_TYPE ((klass), GKM_TYPE_MATE2_STORAGE))
#define GKM_MATE2_STORAGE_GET_CLASS(obj)     (G_TYPE_INSTANCE_GET_CLASS ((obj), GKM_TYPE_MATE2_STORAGE, GkmMate2StorageClass))

typedef struct _GkmMate2Storage GkmMate2Storage;
typedef struct _GkmMate2StorageClass GkmMate2StorageClass;

struct _GkmMate2StorageClass {
	GkmStoreClass parent_class;
};

GType                       gkm_mate2_storage_get_type               (void);

GkmMate2Storage*             gkm_mate2_storage_new                    (GkmModule *module,
                                                                     const gchar *directory);

GkmManager*                 gkm_mate2_storage_get_manager            (GkmMate2Storage *self);

const gchar*                gkm_mate2_storage_get_directory          (GkmMate2Storage *self);

GkmSecret*                  gkm_mate2_storage_get_login              (GkmMate2Storage *self);

gulong                      gkm_mate2_storage_token_flags            (GkmMate2Storage *self);

CK_RV                       gkm_mate2_storage_refresh                (GkmMate2Storage *self);

void                        gkm_mate2_storage_create                 (GkmMate2Storage *self,
                                                                     GkmTransaction *transaction,
                                                                     GkmObject *object);

void                        gkm_mate2_storage_destroy                (GkmMate2Storage *self,
                                                                     GkmTransaction *transaction,
                                                                     GkmObject *object);

void                        gkm_mate2_storage_relock                 (GkmMate2Storage *self,
                                                                     GkmTransaction *transaction,
                                                                     GkmSecret *old_login,
                                                                     GkmSecret *new_login);

CK_RV                       gkm_mate2_storage_unlock                 (GkmMate2Storage *self,
                                                                     GkmSecret *login);

CK_RV                       gkm_mate2_storage_lock                   (GkmMate2Storage *self);

#endif /* __GKM_MATE2_STORAGE_H__ */
