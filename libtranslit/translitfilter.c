/*
 * Copyright (C) 2012 Daiki Ueno <ueno@unixuser.org>
 * Copyright (C) 2012 Red Hat, Inc.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <gio/gio.h>
#include <libtranslit/translit.h>
#include <libtranslit/translitmodule.h>

enum
  {
    PROP_0,
    PROP_LANGUAGE,
    PROP_NAME
  };

G_DEFINE_TYPE (TranslitFilter, translit_filter, G_TYPE_OBJECT);

#define TRANSLIT_FILTER_GET_PRIVATE(obj) \
  (G_TYPE_INSTANCE_GET_PRIVATE ((obj), TRANSLIT_TYPE_FILTER, TranslitFilterPrivate))

struct _TranslitFilterPrivate
{
  char *language;
  char *name;
};

static GHashTable *filters = NULL;
static GHashTable *filter_types = NULL;

static gboolean
translit_filter_real_filter (TranslitFilter      *self,
                             gchar                ascii,
                             TranslitModifierType modifiers)
{
  return FALSE;
}

static gchar *
translit_filter_real_poll_output (TranslitFilter *self)
{
  return NULL;
}

static void
translit_filter_set_property (GObject      *object,
                              guint         prop_id,
                              const GValue *value,
                              GParamSpec   *pspec)
{
  TranslitFilterPrivate *priv = TRANSLIT_FILTER_GET_PRIVATE (object);

  switch (prop_id)
    {
    case PROP_LANGUAGE:
      g_free (priv->language);
      priv->language = g_value_dup_string (value);
      break;
    case PROP_NAME:
      g_free (priv->name);
      priv->name = g_value_dup_string (value);
      break;
    default:
      g_object_set_property (object,
			     g_param_spec_get_name (pspec),
			     value);
      break;
    }
}

static void
translit_filter_get_property (GObject    *object,
                              guint       prop_id,
                              GValue     *value,
                              GParamSpec *pspec)
{
  TranslitFilterPrivate *priv = TRANSLIT_FILTER_GET_PRIVATE (object);

  switch (prop_id)
    {
    case PROP_LANGUAGE:
      g_value_set_string (value, priv->language);
      break;
    case PROP_NAME:
      g_value_set_string (value, priv->name);
      break;
    default:
      g_object_get_property (object,
			     g_param_spec_get_name (pspec),
			     value);
      break;
    }
}

static void
translit_filter_finalize (GObject *object)
{
  TranslitFilterPrivate *priv = TRANSLIT_FILTER_GET_PRIVATE (object);

  g_free (priv->language);
  g_free (priv->name);

  G_OBJECT_CLASS (translit_filter_parent_class)->finalize (object);
}

static void
translit_filter_class_init (TranslitFilterClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GParamSpec *pspec;

  klass->filter = translit_filter_real_filter;
  klass->poll_output = translit_filter_real_poll_output;

  object_class->set_property = translit_filter_set_property;
  object_class->get_property = translit_filter_get_property;
  object_class->finalize = translit_filter_finalize;

  g_type_class_add_private (object_class,
			    sizeof (TranslitFilterPrivate));

  /**
   * TranslitFilter:language:
   *
   * The language which #TranslitFilter supports
   */
  pspec = g_param_spec_string ("language",
			       "language",
			       "Language",
			       NULL,
			       G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
  g_object_class_install_property (object_class, PROP_LANGUAGE, pspec);

  /**
   * TranslitFilter:name:
   *
   * The transliteration which #TranslitFilter supports
   */
  pspec = g_param_spec_string ("name",
			       "name",
			       "Name",
			       NULL,
			       G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
  g_object_class_install_property (object_class, PROP_NAME, pspec);
}

static void
translit_filter_init (TranslitFilter *self)
{
  self->priv = TRANSLIT_FILTER_GET_PRIVATE (self);
}

/**
 * translit_filter_filter:
 * @filter: a #TranslitFilter
 * @ascii: an ASCII char input
 * @modifiers: modifier mask
 *
 * Returns: %TRUE if the input is filtered, %FALSE otherwise
 */
gboolean
translit_filter_filter (TranslitFilter      *filter,
                        gchar                ascii,
                        TranslitModifierType modifiers)
{
  return TRANSLIT_FILTER_GET_CLASS (filter)->filter (filter, ascii, modifiers);
}

/**
 * translit_filter_poll_output:
 * @filter: a #TranslitFilter
 *
 * Get output string kept in @filter.
 * Returns: a string
 */
gchar *
translit_filter_poll_output (TranslitFilter *filter)
{
  return TRANSLIT_FILTER_GET_CLASS (filter)->poll_output (filter);
}

static gchar *
build_module_filename (const gchar *name)
{
#if !defined(G_OS_WIN32) && !defined(G_WITH_CYGWIN)
  return g_strdup_printf ("libtranslit%s.so", name);
#else
  return g_strdup_printf ("translit%s.dll", name);
#endif
}

static void
load_module (const gchar **paths, const char *module_name)
{
  const gchar *name;
  gchar *path, *module_filename;

  if (!g_module_supported ())
    return;

  module_filename = build_module_filename (module_name);

  for (; *paths; paths++)
    {
      GDir *dir;

      dir = g_dir_open (*paths, 0, NULL);
      if (!dir)
	return;

      while ((name = g_dir_read_name (dir)))
	{
	  if (g_strcmp0 (name, module_filename) == 0)
	    {
	      TranslitModule *module;
	      gchar *path;

	      path = g_build_filename (*paths, name, NULL);
	      module = translit_module_new (path);

	      if (g_type_module_use (G_TYPE_MODULE (module)))
		{
		  g_free (path);
		  g_dir_close (dir);
		  g_free (module_filename);
		  return;
		}

	      g_printerr ("Failed to load module: %s\n", path);
	      g_object_unref (module);
	      g_free (path);
	    }
	}
      g_dir_close (dir);
    }
  g_free (module_filename);
}

/**
 * translit_filter_get:
 * @backend: backend name (e.g. "m17n")
 * @language: language code (e.g. "hi")
 * @name: name of the filter (e.g. "inscript")
 *
 * Get a filter instance whose name is @name.
 *
 * Returns: (transfer none): a #TranslitFilter
 */
TranslitFilter *
translit_filter_get (const gchar *backend,
                     const gchar *language,
                     const gchar *name)
{
  gchar *filter_id;
  gpointer data;
  TranslitFilter *filter = NULL;

  filter_id = g_strdup_printf ("%s:%s:%s", backend, language, name);
  if (filters != NULL)
    {
      filter = g_hash_table_lookup (filters, filter_id);
      if (filter != NULL)
	{
	  g_free (filter_id);
	  return filter;
	}
    }

  if (filter_types == NULL)
    filter_types = g_hash_table_new_full (g_str_hash,
					  g_str_equal,
					  (GDestroyNotify) g_free,
					  NULL);

  data = g_hash_table_lookup (filter_types, backend);
  if (data == NULL)
    {
      const gchar *module_path;
      gchar **paths;
      gchar *default_paths[] = { MODULEDIR, NULL };

      module_path = g_getenv ("TRANSLIT_MODULE_PATH");
      if (module_path)
	paths = g_strsplit (module_path, G_SEARCHPATH_SEPARATOR_S, 0);
      else
	paths = g_strdupv (default_paths);
      load_module ((const gchar **) paths, backend);
      g_strfreev (paths);
    }

  data = g_hash_table_lookup (filter_types, backend);
  if (data != NULL)
    {
      GType type = GPOINTER_TO_SIZE (data);
      GParameter parameters[2] = {
	{ "language", G_VALUE_INIT },
	{ "name", G_VALUE_INIT }
      };
      g_value_init (&parameters[0].value, G_TYPE_STRING);
      g_value_set_string (&parameters[0].value, language);
      g_value_init (&parameters[1].value, G_TYPE_STRING);
      g_value_set_string (&parameters[1].value, name);

      if (g_type_is_a (type, G_TYPE_INITABLE))
	{
	  GError *error = NULL;
	  filter = g_initable_newv (type,
				    G_N_ELEMENTS (parameters),
				    parameters,
				    NULL,
				    &error);
	  if (filter == NULL)
	    g_printerr ("can't initialize filter backend %s: %s\n",
			backend, error->message);
	}
      else
	filter = g_object_newv (type,
				G_N_ELEMENTS (parameters),
				parameters);

      if (filter != NULL)
	{
	  if (filters == NULL)
	    filters = g_hash_table_new_full (g_str_hash,
					     g_str_equal,
					     (GDestroyNotify) g_free,
					     NULL);
	  g_hash_table_insert (filters, g_strdup (filter_id), filter);
	}
    }
  g_free (filter_id);
  return filter;
}

void
translit_filter_implement_backend (const gchar *backend, GType type)
{
  g_hash_table_insert (filter_types,
		       g_strdup (backend),
		       GSIZE_TO_POINTER (type));
}

GQuark
translit_module_error_quark (void)
{
  return g_quark_from_static_string ("translit-module-error-quark");
}
