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
#include <m17n.h>
#include <gio/gio.h>
#include <string.h>

#define TYPE_TRANSLITERATOR_M17N (transliterator_m17n_get_type())
#define TRANSLITERATOR_M17N(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), TYPE_TRANSLITERATOR_M17N, TransliteratorM17n))
#define TRANSLITERATOR_M17N_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), TYPE_TRANSLITERATOR_M17N, TransliteratorM17nClass))
#define TRANSLITERATOR_M17N_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), TYPE_TRANSLITERATOR_M17N, TransliteratorM17nClass))

struct _TransliteratorM17n
{
  TranslitTransliterator parent;
  MInputMethod *im;
  MInputContext *ic;
};

struct _TransliteratorM17nClass
{
  TranslitTransliteratorClass parent_class;
};

typedef struct _TransliteratorM17n TransliteratorM17n;
typedef struct _TransliteratorM17nClass TransliteratorM17nClass;

static void initable_iface_init (GInitableIface *initable_iface);

G_DEFINE_DYNAMIC_TYPE_EXTENDED (TransliteratorM17n,
				transliterator_m17n,
				TRANSLIT_TYPE_TRANSLITERATOR,
				0,
				G_IMPLEMENT_INTERFACE (G_TYPE_INITABLE,
						       initable_iface_init));

static MConverter *utf8_converter;

static char *
mtext_to_utf8 (MText *mt)
{
  char *buf;
  size_t cap;

  mconv_reset_converter (utf8_converter);
  cap = (mtext_len (mt) + 1) * 6;
  buf = (char *) malloc (cap);
  mconv_rebind_buffer (utf8_converter, buf, cap);
  mconv_encode (utf8_converter, mt);

  buf[utf8_converter->nbytes] = '\0';

  return buf;
}

static gchar *
transliterator_m17n_real_transliterate (TranslitTransliterator *self,
                                        const gchar            *input,
                                        guint                  *endpos,
                                        GError                **error)
{
  TransliteratorM17n *m17n = TRANSLITERATOR_M17N (self);
  const gchar *p;
  GString *string;
  gchar *output;
  gint n_filtered = 0;

  string = g_string_sized_new (strlen (input));
  minput_reset_ic (m17n->ic);
  for (p = input; *p != '\0'; p = g_utf8_next_char (p))
    {
      gunichar uc = g_utf8_get_char (p);
      MSymbol symbol;
      gint length;
      gchar *utf8;
      gint retval;

      length = g_unichar_to_utf8 (uc, NULL);
      utf8 = g_slice_alloc0 (length + 1);
      g_unichar_to_utf8 (uc, utf8);
      symbol = msymbol (utf8);
      g_slice_free1 (length, utf8);

      retval = minput_filter (m17n->ic, symbol, NULL);
      if (retval == 0)
	{
	  MText *mt = mtext ();

	  retval = minput_lookup (m17n->ic, symbol, NULL, mt);

	  if (mtext_len (mt) > 0) {
	    output = mtext_to_utf8 (mt);
	    g_string_append (string, output);
	    g_free (output);
	  }

	  if (retval)
	    g_string_append_unichar (string, uc);

	  m17n_object_unref (mt);
	  n_filtered = 0;
	}
      else
	n_filtered++;
    }

  output = mtext_to_utf8 (m17n->ic->preedit);
  g_string_append (string, output);
  g_free (output);

  if (endpos)
    *endpos = g_utf8_strlen (input, -1) - n_filtered;

  return g_string_free (string, FALSE);
}

static void
transliterator_m17n_finalize (GObject *object)
{
  TransliteratorM17n *m17n = TRANSLITERATOR_M17N (object);

  if (m17n->ic)
    minput_destroy_ic (m17n->ic);
  if (m17n->im)
    minput_close_im (m17n->im);

  G_OBJECT_CLASS (transliterator_m17n_parent_class)->finalize (object);
}

static void
transliterator_m17n_class_init (TransliteratorM17nClass *klass)
{
  TranslitTransliteratorClass *transliterator_class = TRANSLIT_TRANSLITERATOR_CLASS (klass);
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
  GParamSpec *pspec;

  transliterator_class->transliterate = transliterator_m17n_real_transliterate;

  gobject_class->finalize = transliterator_m17n_finalize;

  M17N_INIT ();
  utf8_converter = mconv_buffer_converter (Mcoding_utf_8, NULL, 0);
}

static void
transliterator_m17n_class_finalize (TransliteratorM17nClass *klass)
{
  mconv_free_converter (utf8_converter);
  M17N_FINI ();
}

static void
transliterator_m17n_init (TransliteratorM17n *self)
{
}

static gboolean
initable_init (GInitable *initable,
	       GCancellable *cancellable,
	       GError **error)
{
  TransliteratorM17n *m17n = TRANSLITERATOR_M17N (initable);
  gchar *name, **strv;

  g_object_get (G_OBJECT (initable),
		"name", &name,
		NULL);

  strv = g_strsplit (name, "-", 2);
  g_return_val_if_fail (g_strv_length (strv) == 2, FALSE);

  m17n->im = minput_open_im (msymbol (strv[0]),
			     msymbol (strv[1]),
			     NULL);
  g_free (name);
  g_strfreev (strv);

  if (m17n->im)
    {
      m17n->ic = minput_create_ic (m17n->im, NULL);
      return TRUE;
    }
  g_set_error (error,
	       TRANSLIT_ERROR,
	       TRANSLIT_ERROR_LOAD_FAILED,
	       "can't open m17n IM");
  return FALSE;
}

static void
initable_iface_init (GInitableIface *initable_iface)
{
  initable_iface->init = initable_init;
}

void
transliterator_m17n_register (GTypeModule *module)
{
  transliterator_m17n_register_type (module);
  translit_implement_transliterator ("m17n", transliterator_m17n_get_type ());
}
