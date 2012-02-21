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

G_DEFINE_TYPE (TranslitFilter, translit_filter, G_TYPE_OBJECT);

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
translit_filter_class_init (TranslitFilterClass *klass)
{
  klass->filter = translit_filter_real_filter;
  klass->poll_output = translit_filter_real_poll_output;
}

static void
translit_filter_init (TranslitFilter *self)
{
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

static gboolean
is_valid_module_name (const gchar *basename)
{
  gboolean result;

#if !defined(G_OS_WIN32) && !defined(G_WITH_CYGWIN)
  if (!g_str_has_prefix (basename, "lib") ||
      !g_str_has_suffix (basename, ".so"))
    return FALSE;
#else
  if (!g_str_has_suffix (basename, ".dll"))
    return FALSE;
#endif

  return TRUE;
}

static void
load_all_modules (const char *dirname)
{
  GDir *dir;
  const gchar *name;

  if (!g_module_supported ())
    return;

  dir = g_dir_open (dirname, 0, NULL);
  if (!dir)
    return;

  while ((name = g_dir_read_name (dir)))
    {
      if (is_valid_module_name (name))
	{
	  TranslitModule *module;
	  gchar *path;

	  path = g_build_filename (dirname, name, NULL);
	  module = translit_module_new (path);

	  if (!g_type_module_use (G_TYPE_MODULE (module)))
	    {
	      g_printerr ("Failed to load module: %s\n", path);
	      g_object_unref (module);
	      g_free (path);
	      continue;
	    }
	  g_free (path);
	}
    }
  g_dir_close (dir);
}

/**
 * translit_filter_get:
 * @name: a name of filter
 *
 * Get a filter instance whose name is @name.
 *
 * Returns: (transfer none): a #TranslitFilter
 */
TranslitFilter *
translit_filter_get (const gchar *name)
{
  gchar **strv;
  gpointer data;
  TranslitFilter *filter = NULL;

  if (filters != NULL)
    {
      filter = g_hash_table_lookup (filters, name);
      if (filter != NULL)
	return filter;
    }

  if (filter_types == NULL)
    {
      const char *module_path;

      filter_types = g_hash_table_new_full (g_str_hash,
					    g_str_equal,
					    (GDestroyNotify) g_free,
					    NULL);
      module_path = g_getenv ("TRANSLIT_MODULE_PATH");
      if (module_path)
	{
	  char **paths = g_strsplit (module_path, G_SEARCHPATH_SEPARATOR_S, 0);
	  gint i;
	  for (i = 0; paths[i] != NULL; i++)
	    load_all_modules (paths[i]);
	  g_strfreev (paths);
	}
      else
	load_all_modules (MODULEDIR);
    }

  strv = g_strsplit (name, ":", -1);
  if (g_strv_length (strv) != 3)
    return NULL;

  data = g_hash_table_lookup (filter_types, strv[0]);
  if (data != NULL)
    {
      GType type = GPOINTER_TO_SIZE (data);
      if (g_type_is_a (type, G_TYPE_INITABLE))
	filter = g_initable_new (type,
				 NULL,
				 NULL,
				 "language", strv[1],
				 "name", strv[2],
				 NULL);
      else
	filter = g_object_new (type,
			       "language", strv[1],
			       "name", strv[2],
			       NULL);
      if (filter != NULL)
	{
	  if (filters == NULL)
	    filters = g_hash_table_new_full (g_str_hash,
					     g_str_equal,
					     (GDestroyNotify) g_free,
					     NULL);
	  g_hash_table_insert (filters, g_strdup (name), filter);
	}
    }
  g_strfreev (strv);
  return filter;
}

void
translit_filter_implement (const gchar *name, GType type)
{
  g_hash_table_insert (filter_types, g_strdup (name), GSIZE_TO_POINTER (type));
}
