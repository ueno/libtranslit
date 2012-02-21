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

#include <libtranslit/translit.h>
#include <libtranslit/translitmodule.h>

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

typedef struct _TranslitModuleClass TranslitModuleClass;

#define TRANSLIT_TYPE_MODULE (translit_module_get_type())
#define TRANSLIT_MODULE(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), TRANSLIT_TYPE_MODULE, TranslitModule))
#define TRANSLIT_MODULE_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), TRANSLIT_TYPE_MODULE, TranslitModuleClass))
#define TRANSLIT_IS_MODULE(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TRANSLIT_TYPE_MODULE))
#define TRANSLIT_IS_MODULE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), TRANSLIT_TYPE_MODULE))
#define TRANSLIT_MODULE_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), TRANSLIT_TYPE_MODULE, TranslitModuleClass))

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

TranslitModule *
translit_module_new (const gchar *filename)
{
  TranslitModule *module;

  g_return_val_if_fail (filename != NULL, NULL);

  module = g_object_new (TRANSLIT_TYPE_MODULE, NULL);
  module->filename = g_strdup (filename);

  return module;
}
