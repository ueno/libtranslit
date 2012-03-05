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

enum
  {
    PROP_0,
    PROP_NAME
  };

G_DEFINE_TYPE (TranslitTransliterator, translit_transliterator, G_TYPE_OBJECT);

#define TRANSLIT_TRANSLITERATOR_GET_PRIVATE(obj)			\
  (G_TYPE_INSTANCE_GET_PRIVATE ((obj), TRANSLIT_TYPE_TRANSLITERATOR, TranslitTransliteratorPrivate))

struct _TranslitTransliteratorPrivate
{
  char *name;
};

typedef struct _TranslitModule TranslitModule;
typedef struct _TranslitModuleClass TranslitModuleClass;

struct _TranslitModule
{
  GTypeModule parent;
  gchar *filename;
  GModule *library;
  gboolean initialized;

  void (*load)   (TranslitModule *module);
  void (*unload) (TranslitModule *module);
};

struct _TranslitModuleClass
{
  GTypeModuleClass parent_class;
};

#define TRANSLIT_TYPE_MODULE (translit_module_get_type())
#define TRANSLIT_MODULE(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), TRANSLIT_TYPE_MODULE, TranslitModule))
#define TRANSLIT_MODULE_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), TRANSLIT_TYPE_MODULE, TranslitModuleClass))
#define TRANSLIT_IS_MODULE(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TRANSLIT_TYPE_MODULE))
#define TRANSLIT_IS_MODULE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), TRANSLIT_TYPE_MODULE))
#define TRANSLIT_MODULE_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), TRANSLIT_TYPE_MODULE, TranslitModuleClass))

static GType           translit_module_get_type (void) G_GNUC_CONST;
static TranslitModule *translit_module_new      (const gchar *filename);

G_DEFINE_TYPE (TranslitModule, translit_module, G_TYPE_TYPE_MODULE);

static gboolean
translit_module_real_load (GTypeModule *gmodule)
{
  TranslitModule *module = TRANSLIT_MODULE (gmodule);

  g_return_val_if_fail (module->filename, FALSE);

  module->library = g_module_open (module->filename,
				   G_MODULE_BIND_LAZY | G_MODULE_BIND_LOCAL);

  if (!module->library)
    {
      g_printerr ("%s\n", g_module_error ());
      return FALSE;
    }

  /* Make sure that the loaded library contains the required methods */
  if (! g_module_symbol (module->library,
                         "translit_module_load",
                         (gpointer) &module->load) ||
      ! g_module_symbol (module->library,
                         "translit_module_unload",
                         (gpointer) &module->unload))
    {
      g_printerr ("%s\n", g_module_error ());
      g_module_close (module->library);

      return FALSE;
    }

  /* Initialize the loaded module */
  module->load (module);
  module->initialized = TRUE;

  return TRUE;
}

static void
translit_module_real_unload (GTypeModule *gmodule)
{
  TranslitModule *module = TRANSLIT_MODULE (gmodule);

  module->unload (module);

  g_module_close (module->library);
  module->library = NULL;

  module->load   = NULL;
  module->unload = NULL;
}

static void
translit_module_finalize (GObject *object)
{
  TranslitModule *module = TRANSLIT_MODULE (object);

  g_free (module->filename);

  G_OBJECT_CLASS (translit_module_parent_class)->finalize (object);
}

static void
translit_module_class_init (TranslitModuleClass *class)
{
  GTypeModuleClass *type_module_class = G_TYPE_MODULE_CLASS (class);
  GObjectClass *object_class = G_OBJECT_CLASS (class);

  type_module_class->load = translit_module_real_load;
  type_module_class->unload = translit_module_real_unload;

  object_class->finalize = translit_module_finalize;
}

static void
translit_module_init (TranslitModule *module)
{
}

static TranslitModule *
translit_module_new (const gchar *filename)
{
  TranslitModule *module;

  g_return_val_if_fail (filename != NULL, NULL);

  module = g_object_new (TRANSLIT_TYPE_MODULE, NULL);
  module->filename = g_strdup (filename);

  return module;
}

static GHashTable *transliterators = NULL;
static GHashTable *transliterator_types = NULL;

GQuark
translit_error_quark (void)
{
  return g_quark_from_static_string ("translit-error-quark");
}

static gchar *
translit_transliterator_real_transliterate (TranslitTransliterator *self,
                                            const gchar            *input,
                                            guint                  *endpos,
                                            GError                **error)
{
  return NULL;
}

static void
translit_transliterator_set_property (GObject      *object,
				      guint         prop_id,
				      const GValue *value,
				      GParamSpec   *pspec)
{
  TranslitTransliterator *trans = TRANSLIT_TRANSLITERATOR (object);

  switch (prop_id)
    {
    case PROP_NAME:
      g_free (trans->priv->name);
      trans->priv->name = g_value_dup_string (value);
      break;
    default:
      g_object_set_property (object,
			     g_param_spec_get_name (pspec),
			     value);
      break;
    }
}

static void
translit_transliterator_get_property (GObject    *object,
				      guint       prop_id,
				      GValue     *value,
				      GParamSpec *pspec)
{
  TranslitTransliterator *trans = TRANSLIT_TRANSLITERATOR (object);

  switch (prop_id)
    {
    case PROP_NAME:
      g_value_set_string (value, trans->priv->name);
      break;
    default:
      g_object_get_property (object,
			     g_param_spec_get_name (pspec),
			     value);
      break;
    }
}

static void
translit_transliterator_finalize (GObject *object)
{
  TranslitTransliterator *trans = TRANSLIT_TRANSLITERATOR (object);

  g_free (trans->priv->name);

  G_OBJECT_CLASS (translit_transliterator_parent_class)->finalize (object);
}

static void
translit_transliterator_class_init (TranslitTransliteratorClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GParamSpec *pspec;

  klass->transliterate = translit_transliterator_real_transliterate;

  object_class->set_property = translit_transliterator_set_property;
  object_class->get_property = translit_transliterator_get_property;
  object_class->finalize = translit_transliterator_finalize;

  g_type_class_add_private (object_class,
			    sizeof (TranslitTransliteratorPrivate));

  /**
   * TranslitTransliterator:name:
   *
   * The transliteration which #TranslitTransliterator supports
   */
  pspec = g_param_spec_string ("name",
			       "name",
			       "Name",
			       NULL,
			       G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
  g_object_class_install_property (object_class, PROP_NAME, pspec);
}

static void
translit_transliterator_init (TranslitTransliterator *self)
{
  self->priv = TRANSLIT_TRANSLITERATOR_GET_PRIVATE (self);
}

/**
 * translit_transliterator_transliterate:
 * @transliterator: a #TranslitTransliterator
 * @input: an input string in UTF-8
 * @endpos: (out) (allow-none): ending position of transliteration (in chars)
 * @error: a #GError
 *
 * Returns: a newly allocated output string
 */
gchar *
translit_transliterator_transliterate (TranslitTransliterator *transliterator,
                                       const gchar            *input,
                                       guint                  *endpos,
                                       GError                **error)
{
  g_return_val_if_fail (TRANSLIT_IS_TRANSLITERATOR (transliterator), NULL);

  if (!g_utf8_validate (input, -1, NULL))
    {
      g_set_error (error,
		   TRANSLIT_ERROR,
		   TRANSLIT_ERROR_INVALID_INPUT,
		   "not a valid UTF-8 sequence");
      return NULL;
    }

  return TRANSLIT_TRANSLITERATOR_GET_CLASS (transliterator)->
    transliterate (transliterator, input, endpos, error);
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
	{
	  g_free (module_filename);
	  return;
	}

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
 * translit_transliterator_get:
 * @backend: backend name (e.g. "m17n")
 * @name: name of the transliterator (e.g. "hi-inscript")
 * @error: a #GError
 *
 * Get a transliterator instance whose name is @name.
 *
 * Returns: (transfer none): a #TranslitTransliterator
 */
TranslitTransliterator *
translit_transliterator_get (const gchar *backend,
			     const gchar *name,
			     GError     **error)
{
  gchar *transliterator_id;
  GType transliterator_type;
  TranslitTransliterator *transliterator = NULL;
  GParameter transliterator_parameters[1] = {
    { "name", { 0 } }
  };
  gpointer data;

  transliterator_id = g_strdup_printf ("%s:%s", backend, name);
  if (transliterators != NULL)
    {
      transliterator = g_hash_table_lookup (transliterators, transliterator_id);
      if (transliterator != NULL)
	{
	  g_free (transliterator_id);
	  return transliterator;
	}
    }

  if (transliterator_types == NULL)
    transliterator_types = g_hash_table_new_full (g_str_hash,
						  g_str_equal,
						  (GDestroyNotify) g_free,
						  NULL);

  data = g_hash_table_lookup (transliterator_types, backend);
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

  data = g_hash_table_lookup (transliterator_types, backend);
  if (data == NULL)
    {
      g_free (transliterator_id);
      g_set_error (error,
		   TRANSLIT_ERROR,
		   TRANSLIT_ERROR_NO_SUCH_BACKEND,
		   "no such backend %s",
		   backend);
      return NULL;
    }

  transliterator_type = GPOINTER_TO_SIZE (data);
  g_value_init (&transliterator_parameters[0].value, G_TYPE_STRING);
  g_value_take_string (&transliterator_parameters[0].value, name);

  if (g_type_is_a (transliterator_type, G_TYPE_INITABLE))
    {
      transliterator = g_initable_newv (transliterator_type,
					G_N_ELEMENTS (transliterator_parameters),
					transliterator_parameters,
					NULL,
					error);
      if (transliterator == NULL)
	{
	  g_free (transliterator_id);
	  return NULL;
	}
    }
  else
    transliterator = g_object_newv (transliterator_type,
				    G_N_ELEMENTS (transliterator_parameters),
				    transliterator_parameters);

  if (transliterators == NULL)
    transliterators = g_hash_table_new_full (g_str_hash,
					     g_str_equal,
					     (GDestroyNotify) g_free,
					     NULL);
  g_hash_table_insert (transliterators, g_strdup (transliterator_id), transliterator);
  g_free (transliterator_id);
  return transliterator;
}

void
translit_implement_transliterator (const gchar *backend, GType type)
{
  g_hash_table_insert (transliterator_types,
		       g_strdup (backend),
		       GSIZE_TO_POINTER (type));
}
