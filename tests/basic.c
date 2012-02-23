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

static void
basic (void)
{
  TranslitTransliterator *transliterator;
  GError *error;

  error = NULL;
  transliterator = translit_transliterator_get ("nonexistent", "nonexistent",
						&error);
  g_assert_error (error,
		  TRANSLIT_ERROR,
		  TRANSLIT_ERROR_NO_SUCH_BACKEND);
  g_error_free (error);

  error = NULL;
  transliterator = translit_transliterator_get ("m17n",
						"nonexistent-nonexistent",
						&error);
  g_assert_error (error,
		  TRANSLIT_ERROR,
		  TRANSLIT_ERROR_LOAD_FAILED);
  g_error_free (error);

  error = NULL;
  transliterator = translit_transliterator_get ("m17n", "hi-nonexistent",
						&error);
  g_assert_error (error,
		  TRANSLIT_ERROR,
		  TRANSLIT_ERROR_LOAD_FAILED);
  g_error_free (error);

  error = NULL;
  transliterator = translit_transliterator_get ("m17n", "hi-inscript",
						&error);
  if (transliterator)
    {
      gchar *output;
      guint endpos;
      GError *error;

      error = NULL;
      output = translit_transliterator_transliterate (transliterator,
						      "a",
						      &endpos,
						      &error);
      g_assert_no_error (error);
      g_assert_cmpint (endpos, ==, 1);
      g_assert_cmpstr (output, ==, "ो");

      g_free (output);
      g_object_unref (transliterator);
    }

  error = NULL;
  transliterator = translit_transliterator_get ("icu", "Latin-Katakana",
						&error);
  if (transliterator)
    {
      gchar *output;
      guint endpos;
      GError *error;

      error = NULL;
      output = translit_transliterator_transliterate (transliterator,
						      "aiueo",
						      &endpos,
						      &error);
      g_assert_no_error (error);
      g_assert_cmpint (endpos, ==, 5);
      g_assert_cmpstr (output, ==, "アイウエオ");

      g_free (output);
      g_object_unref (transliterator);
    }
}

int
main (int argc, char **argv) {
  g_type_init ();
  g_test_init (&argc, &argv, NULL);
  g_test_add_func ("/libtranslit/basic", basic);
  return g_test_run ();
}
