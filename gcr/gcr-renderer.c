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

#include "gcr-renderer.h"

#include "gck/gck.h"

#include <gtk/gtk.h>

enum {
	DATA_CHANGED,
	LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] = { 0 };

typedef struct _GcrRegistered {
	GckAttributes *attrs;
	GType renderer_type;
} GcrRegistered;

static GArray *registered_renderers = NULL;
static gboolean registered_sorted = FALSE;

static void
gcr_renderer_base_init (gpointer gobject_iface)
{
	static gboolean initialized = FALSE;
	if (!initialized) {

		g_object_interface_install_property (gobject_iface,
		         g_param_spec_string ("label", "Label", "The label for the renderer",
		                              "", G_PARAM_READWRITE));

		g_object_interface_install_property (gobject_iface,
		         g_param_spec_boxed ("attributes", "Attributes", "The data displayed in the renderer",
		                             GCK_TYPE_ATTRIBUTES, G_PARAM_READWRITE));

		signals[DATA_CHANGED] = g_signal_new ("data-changed", GCR_TYPE_RENDERER, G_SIGNAL_RUN_LAST,
		                                      G_STRUCT_OFFSET (GcrRendererIface, data_changed),
		                                      NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE, 0);

		initialized = TRUE;
	}
}

GType
gcr_renderer_get_type (void)
{
	static GType type = 0;
	if (!type) {
		static const GTypeInfo info = {
			sizeof (GcrRendererIface),
			gcr_renderer_base_init,  /* base init */
			NULL,                    /* base finalize */
		};
		type = g_type_register_static (G_TYPE_INTERFACE, "GcrRendererIface", &info, 0);
	}

	return type;
}

void
gcr_renderer_render (GcrRenderer *self, GcrViewer *viewer)
{
	g_return_if_fail (GCR_IS_RENDERER (self));
	g_return_if_fail (GCR_RENDERER_GET_INTERFACE (self)->render);
	GCR_RENDERER_GET_INTERFACE (self)->render (self, viewer);
}

void
gcr_renderer_emit_data_changed (GcrRenderer *self)
{
	g_return_if_fail (GCR_IS_RENDERER (self));
	g_signal_emit (self, signals[DATA_CHANGED], 0);
}

static gint
sort_registered_by_n_attrs (gconstpointer a, gconstpointer b)
{
	const GcrRegistered *ra = a;
	const GcrRegistered *rb = b;
	gulong na, nb;

	g_assert (a);
	g_assert (b);

	na = gck_attributes_count (ra->attrs);
	nb = gck_attributes_count (rb->attrs);

	/* Note we're sorting in reverse order */
	if (na < nb)
		return 1;
	return (na == nb) ? 0 : -1;
}

GcrRenderer*
gcr_renderer_create (const gchar *label, GckAttributes *attrs)
{
	GcrRegistered *registered;
	gboolean matched;
	gulong n_attrs;
	gulong j;
	gsize i;

	g_return_val_if_fail (attrs, NULL);

	if (!registered_renderers)
		return NULL;

	if (!registered_sorted) {
		g_array_sort (registered_renderers, sort_registered_by_n_attrs);
		registered_sorted = TRUE;
	}

	for (i = 0; i < registered_renderers->len; ++i) {
		registered = &(g_array_index (registered_renderers, GcrRegistered, i));
		n_attrs = gck_attributes_count (registered->attrs);

		matched = TRUE;

		for (j = 0; j < n_attrs; ++j) {
			if (!gck_attributes_contains (attrs, gck_attributes_at (registered->attrs, j))) {
				matched = FALSE;
				break;
			}
		}

		if (matched)
			return g_object_new (registered->renderer_type, "label", label,
			                     "attributes", attrs, NULL);
	}

	return NULL;
}

void
gcr_renderer_register (GType renderer_type, GckAttributes *attrs)
{
	GcrRegistered registered;

	if (!registered_renderers)
		registered_renderers = g_array_new (FALSE, FALSE, sizeof (GcrRegistered));

	registered.renderer_type = renderer_type;
	registered.attrs = gck_attributes_ref (attrs);
	g_array_append_val (registered_renderers, registered);
	registered_sorted = FALSE;
}
