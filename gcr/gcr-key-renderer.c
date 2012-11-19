/*
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

#include "gcr-key-renderer.h"
#include "gcr-display-view.h"
#include "gcr-icons.h"
#include "gcr-renderer.h"
#include "gcr-viewer.h"

#include "egg/egg-hex.h"

#include "gck/gck.h"

#include <gdk/gdk.h>
#include <glib/gi18n-lib.h>

enum {
	PROP_0,
	PROP_LABEL,
	PROP_ATTRIBUTES
};

struct _GcrKeyRendererPrivate {
	guint key_size;
	gchar *label;
	GckAttributes *attributes;
};

static void gcr_key_renderer_renderer_iface (GcrRendererIface *iface);

G_DEFINE_TYPE_WITH_CODE (GcrKeyRenderer, gcr_key_renderer, G_TYPE_OBJECT,
                         G_IMPLEMENT_INTERFACE (GCR_TYPE_RENDERER, gcr_key_renderer_renderer_iface));

/* -----------------------------------------------------------------------------
 * INTERNAL
 */

static gchar*
calculate_label (GcrKeyRenderer *self)
{
	gchar *label;

	if (self->pv->label)
		return g_strdup (self->pv->label);

	if (self->pv->attributes) {
		if (gck_attributes_find_string (self->pv->attributes, CKA_LABEL, &label))
			return label;
	}

	return g_strdup (_("Key"));
}

static gint
calculate_rsa_key_size (GckAttributes *attrs)
{
	GckAttribute *attr;
	gulong bits;

	attr = gck_attributes_find (attrs, CKA_MODULUS);

	/* Calculate the bit length, and remove the complement */
	if (attr != NULL)
		return (attr->length / 2) * 2 * 8;

	if (gck_attributes_find_ulong (attrs, CKA_MODULUS_BITS, &bits))
		return (gint)bits;

	return -1;
}

static guint
calculate_dsa_key_size (GckAttributes *attrs)
{
	GckAttribute *attr;
	gulong bits;

	attr = gck_attributes_find (attrs, CKA_PRIME);

	/* Calculate the bit length, and remove the complement */
	if (attr != NULL)
		return (attr->length / 2) * 2 * 8;

	if (gck_attributes_find_ulong (attrs, CKA_PRIME_BITS, &bits))
		return (gint)bits;

	return -1;
}

static gint
calculate_key_size (GckAttributes *attrs, gulong key_type)
{
	if (key_type == CKK_RSA)
		return calculate_rsa_key_size (attrs);
	else if (key_type == CKK_DSA)
		return calculate_dsa_key_size (attrs);
	else
		return -1;
}

/* -----------------------------------------------------------------------------
 * OBJECT
 */

static void
gcr_key_renderer_init (GcrKeyRenderer *self)
{
	self->pv = (G_TYPE_INSTANCE_GET_PRIVATE (self, GCR_TYPE_KEY_RENDERER, GcrKeyRendererPrivate));
}

static void
gcr_key_renderer_dispose (GObject *obj)
{
	G_OBJECT_CLASS (gcr_key_renderer_parent_class)->dispose (obj);
}

static void
gcr_key_renderer_finalize (GObject *obj)
{
	GcrKeyRenderer *self = GCR_KEY_RENDERER (obj);

	if (self->pv->attributes)
		gck_attributes_unref (self->pv->attributes);
	self->pv->attributes = NULL;

	g_free (self->pv->label);
	self->pv->label = NULL;

	G_OBJECT_CLASS (gcr_key_renderer_parent_class)->finalize (obj);
}

static void
gcr_key_renderer_set_property (GObject *obj, guint prop_id, const GValue *value,
                               GParamSpec *pspec)
{
	GcrKeyRenderer *self = GCR_KEY_RENDERER (obj);

	switch (prop_id) {
	case PROP_LABEL:
		g_free (self->pv->label);
		self->pv->label = g_value_dup_string (value);
		g_object_notify (obj, "label");
		gcr_renderer_emit_data_changed (GCR_RENDERER (self));
		break;
	case PROP_ATTRIBUTES:
		g_return_if_fail (!self->pv->attributes);
		self->pv->attributes = g_value_dup_boxed (value);
		gcr_renderer_emit_data_changed (GCR_RENDERER (self));
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (obj, prop_id, pspec);
		break;
	}
}

static void
gcr_key_renderer_get_property (GObject *obj, guint prop_id, GValue *value,
                               GParamSpec *pspec)
{
	GcrKeyRenderer *self = GCR_KEY_RENDERER (obj);

	switch (prop_id) {
	case PROP_LABEL:
		g_value_take_string (value, calculate_label (self));
		break;
	case PROP_ATTRIBUTES:
		g_value_set_boxed (value, self->pv->attributes);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (obj, prop_id, pspec);
		break;
	}
}

static void
gcr_key_renderer_class_init (GcrKeyRendererClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
	GckAttributes *registered;

	gcr_key_renderer_parent_class = g_type_class_peek_parent (klass);
	g_type_class_add_private (klass, sizeof (GcrKeyRendererPrivate));

	gobject_class->dispose = gcr_key_renderer_dispose;
	gobject_class->finalize = gcr_key_renderer_finalize;
	gobject_class->set_property = gcr_key_renderer_set_property;
	gobject_class->get_property = gcr_key_renderer_get_property;

	g_object_class_override_property (gobject_class, PROP_LABEL, "label");
	g_object_class_override_property (gobject_class, PROP_ATTRIBUTES, "attributes");

	_gcr_icons_register ();

	/* Register this as a view which can be loaded */
	registered = gck_attributes_new ();
	gck_attributes_add_ulong (registered, CKA_CLASS, CKO_PRIVATE_KEY);
	gcr_renderer_register (GCR_TYPE_KEY_RENDERER, registered);
	gck_attributes_unref (registered);
}

static void
gcr_key_renderer_real_render (GcrRenderer *renderer, GcrViewer *viewer)
{
	GcrKeyRenderer *self;
	GcrDisplayView *view;
	const gchar *text = "";
	gchar *display;
	gulong klass;
	gulong key_type;
	gint size;

	self = GCR_KEY_RENDERER (renderer);

	if (GCR_IS_DISPLAY_VIEW (viewer)) {
		view = GCR_DISPLAY_VIEW (viewer);

	} else {
		g_warning ("GcrKeyRenderer only works with internal specific "
		           "GcrViewer returned by gcr_viewer_new().");
		return;
	}

	_gcr_display_view_clear (view, renderer);

	if (!self->pv->attributes)
		return;

	_gcr_display_view_set_stock_image (view, renderer, GTK_STOCK_DIALOG_AUTHENTICATION);

	if (!gck_attributes_find_ulong (self->pv->attributes, CKA_CLASS, &klass) ||
	    !gck_attributes_find_ulong (self->pv->attributes, CKA_KEY_TYPE, &key_type)) {
		g_warning ("private key does not have the CKA_CLASS and CKA_KEY_TYPE attributes");
		return;
	}

	display = calculate_label (self);
	_gcr_display_view_append_title (view, renderer, display);
	g_free (display);

	if (klass == CKO_PRIVATE_KEY) {
		if (key_type == CKK_RSA)
			text = _("Private RSA Key");
		else if (key_type == CKK_DSA)
			text = _("Private DSA Key");
		else
			text = _("Private Key");
	} else if (klass == CKO_PUBLIC_KEY) {
		if (key_type == CKK_RSA)
			text = _("Public DSA Key");
		else if (key_type == CKK_DSA)
			text = _("Public DSA Key");
		else
			text = _("Public Key");
	}

	_gcr_display_view_append_content (view, renderer, text, NULL);

	size = calculate_key_size (self->pv->attributes, key_type);
	if (size >= 0) {
		display = g_strdup_printf (ngettext ("%d bit", "%d bits", size), size);
		_gcr_display_view_append_content (view, renderer, _("Strength"), display);
		g_free (display);
	}

	_gcr_display_view_start_details (view, renderer);


	if (key_type == CKK_RSA)
		text = _("RSA");
	else if (key_type == CKK_DSA)
		text = _("DSA");
	else
		text = _("Unknown");
	_gcr_display_view_append_value (view, renderer, _("Algorithm"), text, FALSE);

	size = calculate_key_size (self->pv->attributes, key_type);
	if (size < 0)
		display = g_strdup (_("Unknown"));
	else
		display = g_strdup_printf ("%d", size);
	_gcr_display_view_append_value (view, renderer, _("Size"), display, FALSE);
	g_free (display);

	/* TODO: We need to have consistent key fingerprints. */
	_gcr_display_view_append_value (view, renderer, _("Fingerprint"), "XX XX XX XX XX XX XX XX XX XX", TRUE);
}

static void
gcr_key_renderer_renderer_iface (GcrRendererIface *iface)
{
	iface->render = gcr_key_renderer_real_render;
}

/* -----------------------------------------------------------------------------
 * PUBLIC
 */

GcrKeyRenderer*
gcr_key_renderer_new (const gchar *label, GckAttributes *attrs)
{
	return g_object_new (GCR_TYPE_KEY_RENDERER, "label", label, "attributes", attrs, NULL);
}

void
gcr_key_renderer_set_attributes (GcrKeyRenderer *self, GckAttributes *attrs)
{
	g_return_if_fail (GCR_IS_KEY_RENDERER (self));

	if (self->pv->attributes)
		gck_attributes_unref (self->pv->attributes);
	self->pv->attributes = attrs;
	if (self->pv->attributes)
		gck_attributes_ref (self->pv->attributes);

	g_object_notify (G_OBJECT (self), "attributes");
	gcr_renderer_emit_data_changed (GCR_RENDERER (self));
}

GckAttributes*
gcr_key_renderer_get_attributes (GcrKeyRenderer *self)
{
	g_return_val_if_fail (GCR_IS_KEY_RENDERER (self), NULL);
	return self->pv->attributes;
}
