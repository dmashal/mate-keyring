/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */
/* egg-asn1.h - ASN.1/DER parsing and encoding routines

   Copyright (C) 2009 Stefan Walter

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

#ifndef EGG_ASN1X_H_
#define EGG_ASN1X_H_

#include <glib.h>
#include <libtasn1.h>

#ifndef HAVE_EGG_ALLOCATOR
typedef void* (*EggAllocator) (void* p, gsize);
#define HAVE_EGG_ALLOCATOR
#endif

typedef gboolean (*EggAsn1xEncoder) (gpointer data, guchar *buf, gsize n_buf);

struct static_struct_asn;

GNode*              egg_asn1x_create                 (const struct static_struct_asn *defs,
                                                      const gchar *type);

GNode*              egg_asn1x_create_quark           (const struct static_struct_asn *defs,
                                                      GQuark type);

GNode*              egg_asn1x_create_and_decode      (const struct static_struct_asn *defs,
                                                      const gchar *type,
                                                      gconstpointer data,
                                                      gsize n_data);

void                egg_asn1x_dump                   (GNode *asn);

void                egg_asn1x_clear                  (GNode *asn);

gboolean            egg_asn1x_decode                 (GNode *asn,
                                                      gconstpointer data,
                                                      gsize n_data);

gboolean            egg_asn1x_validate               (GNode *asn);

gpointer            egg_asn1x_encode                 (GNode *asn,
                                                      EggAllocator allocator,
                                                      gsize *n_data);

const gchar*        egg_asn1x_message                (GNode *asn);

GNode*              egg_asn1x_node                   (GNode *asn,
                                                      ...) G_GNUC_NULL_TERMINATED;

const gchar*        egg_asn1x_name                   (GNode *asn);

guint               egg_asn1x_count                  (GNode *node);

GNode*              egg_asn1x_append                 (GNode *node);

gboolean            egg_asn1x_have                   (GNode *node);

GNode*              egg_asn1x_get_choice             (GNode *node);

gboolean            egg_asn1x_set_choice             (GNode *node,
                                                      GNode *choice);

gboolean            egg_asn1x_get_boolean            (GNode *node,
                                                      gboolean *value);

gboolean            egg_asn1x_set_boolean            (GNode *node,
                                                      gboolean value);

GQuark              egg_asn1x_get_enumerated         (GNode *node);

gboolean            egg_asn1x_set_enumerated         (GNode *node,
                                                      GQuark value);

gboolean            egg_asn1x_get_integer_as_ulong   (GNode *node,
                                                      gulong *value);

gboolean            egg_asn1x_set_integer_as_ulong   (GNode *node,
                                                      gulong value);

gpointer            egg_asn1x_get_integer_as_raw     (GNode *node,
                                                      EggAllocator allocator,
                                                      gsize *n_data);

gboolean            egg_asn1x_set_integer_as_raw     (GNode *node,
                                                      gpointer data,
                                                      gsize n_data,
                                                      GDestroyNotify destroy);

gconstpointer       egg_asn1x_get_raw_value          (GNode *node,
                                                      gsize *n_content);

gboolean            egg_asn1x_set_raw_value          (GNode *node,
                                                      gsize length,
                                                      EggAsn1xEncoder encoder,
                                                      gpointer user_data,
                                                      GDestroyNotify destroy);

gconstpointer       egg_asn1x_get_raw_element        (GNode *node,
                                                      gsize *n_data);

gboolean            egg_asn1x_set_raw_element        (GNode *node,
                                                      gpointer user_data,
                                                      gsize n_data,
                                                      GDestroyNotify destroy);

guchar*             egg_asn1x_get_string_as_raw      (GNode *node,
                                                      EggAllocator allocator,
                                                      gsize *n_string);

gboolean            egg_asn1x_set_string_as_raw      (GNode *node,
                                                      guchar *data,
                                                      gsize n_data,
                                                      GDestroyNotify destroy);

guchar*             egg_asn1x_get_bits_as_raw        (GNode *node,
                                                      EggAllocator allocator,
                                                      guint *n_bits);

gboolean            egg_asn1x_set_bits_as_raw        (GNode *node,
                                                      guchar *data,
                                                      guint n_bits,
                                                      GDestroyNotify destroy);

gboolean            egg_asn1x_get_bits_as_ulong      (GNode *node,
                                                      gulong *value,
                                                      guint *n_bits);

gboolean            egg_asn1x_set_bits_as_ulong      (GNode *node,
                                                      gulong value,
                                                      guint n_bits);

gchar*              egg_asn1x_get_string_as_utf8     (GNode *node,
                                                      EggAllocator allocator);

gboolean            egg_asn1x_set_string_as_utf8     (GNode *node,
                                                      gchar *data,
                                                      GDestroyNotify destroy);

glong               egg_asn1x_get_time_as_long       (GNode *node);

gboolean            egg_asn1x_set_time_as_long       (GNode *node,
                                                      glong time);

gboolean            egg_asn1x_get_time_as_date       (GNode *node,
                                                      GDate *date);

gboolean            egg_asn1x_set_time_as_date       (GNode *node,
                                                      GDate *date);

GQuark              egg_asn1x_get_oid_as_quark       (GNode *node);

gboolean            egg_asn1x_set_oid_as_quark       (GNode *node,
                                                      GQuark oid);

gchar*              egg_asn1x_get_oid_as_string      (GNode *node);

gboolean            egg_asn1x_set_oid_as_string      (GNode *node,
                                                      const gchar *oid);

void                egg_asn1x_destroy                (gpointer asn);

glong               egg_asn1x_parse_time_general     (const gchar *time,
                                                      gssize n_time);

glong               egg_asn1x_parse_time_utc         (const gchar *time,
                                                      gssize n_time);

gssize              egg_asn1x_element_length         (gconstpointer data,
                                                      gsize n_data);

gconstpointer       egg_asn1x_element_content        (gconstpointer data,
                                                      gsize n_data,
                                                      gsize *n_content);

#define             egg_asn1x_assert(expr, node) \
	do { if G_LIKELY(expr) ; else \
		g_assertion_message (G_LOG_DOMAIN, __FILE__, __LINE__, G_STRFUNC, \
		                     egg_asn1x_message (node) ? \
		                     egg_asn1x_message (node) : "[no message]"); } while(0)

#define             egg_asn1x_assert_not_reached(node) \
		g_assertion_message (G_LOG_DOMAIN, __FILE__, __LINE__, G_STRFUNC, \
		                     egg_asn1x_message (node) ? \
		                     egg_asn1x_message (node) : "[no message]")

#endif /*EGG_ASN1X_H_*/
