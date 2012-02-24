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
#include <unicode/ustring.h>
#include <unicode/utrans.h>
#include <string.h>
#include <gio/gio.h>

#define TYPE_TRANSLITERATOR_ICU (transliterator_icu_get_type())
#define TRANSLITERATOR_ICU(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), TYPE_TRANSLITERATOR_ICU, TransliteratorIcu))
#define TRANSLITERATOR_ICU_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), TYPE_TRANSLITERATOR_ICU, TransliteratorIcuClass))
#define TRANSLITERATOR_ICU_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), TYPE_TRANSLITERATOR_ICU, TransliteratorIcuClass))

struct _TransliteratorIcu
{
  TranslitTransliterator parent;
  UTransliterator *trans;
};

struct _TransliteratorIcuClass
{
  TranslitTransliteratorClass parent_class;
};

typedef struct _TransliteratorIcu TransliteratorIcu;
typedef struct _TransliteratorIcuClass TransliteratorIcuClass;

static void initable_iface_init (GInitableIface *initable_iface);

G_DEFINE_DYNAMIC_TYPE_EXTENDED (TransliteratorIcu,
				transliterator_icu,
				TRANSLIT_TYPE_TRANSLITERATOR,
				0,
				G_IMPLEMENT_INTERFACE (G_TYPE_INITABLE,
						       initable_iface_init));

static gchar *
ustring_to_utf8 (const UChar *ustr, int32_t ustrLength)
{
  gchar *dest;
  int32_t destLength;
  UErrorCode errorCode;

  errorCode = 0;
  u_strToUTF8 (NULL, 0, &destLength, ustr, ustrLength, &errorCode);
  if (errorCode != U_BUFFER_OVERFLOW_ERROR)
    {
      g_warning ("can't get the number of byte required to convert ustring: %s",
		 u_errorName (errorCode));
      return NULL;
    }
  dest = g_malloc0 (destLength + 1);

  errorCode = 0;
  u_strToUTF8 (dest, destLength + 1, NULL, ustr, ustrLength, &errorCode);
  if (errorCode != U_ZERO_ERROR)
    {
      g_free (dest);
      g_warning ("can't convert ustring to UTF-8 string: %s",
		 u_errorName (errorCode));
      return NULL;
    }

  return dest;
}

static UChar *
ustring_from_utf8 (const gchar *utf8, int32_t *ustrLength)
{
  UChar *dest;
  int32_t destLength, utf8Length = strlen (utf8);
  UErrorCode errorCode;

  errorCode = 0;
  u_strFromUTF8 (NULL, 0, &destLength, utf8, utf8Length, &errorCode);
  if (errorCode != U_BUFFER_OVERFLOW_ERROR)
    {
      g_warning ("can't get the number of chars in UTF-8 string: %s",
		 u_errorName (errorCode));
      return NULL;
    }

  dest = g_malloc0_n (destLength + 1, sizeof(UChar));

  errorCode = 0;
  u_strFromUTF8 (dest, destLength + 1, NULL, utf8, utf8Length, &errorCode);
  if (errorCode != U_ZERO_ERROR)
    {
      g_free (dest);
      g_warning ("can't convert UTF-8 string to ustring: %s",
		 u_errorName (errorCode));
      return NULL;
    }

  *ustrLength = destLength;
  return dest;
}

static gchar *
transliterator_icu_real_transliterate (TranslitTransliterator *self,
                                       const gchar            *input,
                                       guint                  *endpos,
                                       GError                **error)
{
  TransliteratorIcu *icu = TRANSLITERATOR_ICU (self);
  gchar *output;
  gint n_filtered = 0;
  UChar *ustr;
  int32_t ustrLength, ustrCapacity, limit;
  UErrorCode errorCode;

  ustr = ustring_from_utf8 (input, &ustrLength);
  limit = ustrLength;
  ustrCapacity = ustrLength + 1;

  do
    {
      errorCode = 0;
      utrans_transUChars (icu->trans,
			  ustr, &ustrLength, ustrCapacity,
			  0, &limit,
			  &errorCode);
      if (errorCode == U_BUFFER_OVERFLOW_ERROR)
	{
	  ustrCapacity *= 2;
	  ustr = g_realloc (ustr, ustrCapacity);
	}
    }
  while (errorCode == U_BUFFER_OVERFLOW_ERROR);

  if (errorCode != U_ZERO_ERROR)
    {
      g_free (ustr);
      g_set_error (error,
		   TRANSLIT_ERROR,
		   TRANSLIT_ERROR_FAILED,
		   "failed to transliterate: %s", u_errorName (errorCode));
      return NULL;
    }

  output = ustring_to_utf8 (ustr, ustrLength);
  g_free (ustr);

  if (endpos)
    *endpos = limit;

  return output;
}

static void
transliterator_icu_finalize (GObject *object)
{
  TransliteratorIcu *icu = TRANSLITERATOR_ICU (object);

  utrans_close (icu->trans);

  G_OBJECT_CLASS (transliterator_icu_parent_class)->finalize (object);
}

static void
transliterator_icu_class_init (TransliteratorIcuClass *klass)
{
  TranslitTransliteratorClass *transliterator_class = TRANSLIT_TRANSLITERATOR_CLASS (klass);
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
  GParamSpec *pspec;

  transliterator_class->transliterate = transliterator_icu_real_transliterate;

  gobject_class->finalize = transliterator_icu_finalize;
}

static void
transliterator_icu_class_finalize (TransliteratorIcuClass *klass)
{
}

static void
transliterator_icu_init (TransliteratorIcu *self)
{
}

static gboolean
initable_init (GInitable *initable,
	       GCancellable *cancellable,
	       GError **error)
{
  TransliteratorIcu *icu = TRANSLITERATOR_ICU (initable);
  gchar *name;
  UChar *id;
  int32_t idLength;
  UErrorCode errorCode;

  g_object_get (G_OBJECT (initable),
		"name", &name,
		NULL);

  id = ustring_from_utf8 (name, &idLength);
  g_free (name);

  errorCode = 0;
  icu->trans = utrans_openU (id, idLength,
			     UTRANS_FORWARD,
			     NULL, -1,
			     NULL,
			     &errorCode);
  g_free (id);

  if (icu->trans == NULL)
    {
      g_set_error (error,
		   TRANSLIT_ERROR,
		   TRANSLIT_ERROR_LOAD_FAILED,
		   "can't open ICU utrans");
      return FALSE;
    }
  return TRUE;
}

static void
initable_iface_init (GInitableIface *initable_iface)
{
  initable_iface->init = initable_init;
}

void
transliterator_icu_register (GTypeModule *module)
{
  transliterator_icu_register_type (module);
  translit_implement_transliterator ("icu", transliterator_icu_get_type ());
}
