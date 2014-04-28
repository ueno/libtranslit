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
basic_load (void)
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
}

static void
basic_m17n (void)
{
  TranslitTransliterator *transliterator;
  GError *error;

  error = NULL;
  transliterator = translit_transliterator_get ("m17n", "hi-inscript",
						&error);
  g_assert_no_error (error);

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
  transliterator = translit_transliterator_get ("m17n", "da-post",
						&error);
  g_assert_no_error (error);

  if (transliterator)
    {
      gchar *output;
      guint endpos;
      GError *error;

      error = NULL;
      output = translit_transliterator_transliterate (transliterator,
						      "oeaae'aeaaa",
						      &endpos,
						      &error);
      g_assert_no_error (error);
      g_assert_cmpint (endpos, ==, 11);
      g_assert_cmpstr (output, ==, "øåéæaa");

      g_free (output);
      g_object_unref (transliterator);
    }

  error = NULL;
  transliterator = translit_transliterator_get ("m17n", "t-latn-post",
						&error);
  g_assert_no_error (error);

  if (transliterator)
    {
      gchar *output;
      guint endpos;
      GError *error;

      error = NULL;
      output = translit_transliterator_transliterate (transliterator,
						      "a/a//",
						      &endpos,
						      &error);
      g_assert_no_error (error);
      g_assert_cmpint (endpos, ==, 5);
      g_assert_cmpstr (output, ==, "åa/");

      g_free (output);

      output = translit_transliterator_transliterate (transliterator,
						      "a/",
						      &endpos,
						      &error);
      g_assert_no_error (error);
      g_assert_cmpint (endpos, ==, 2);
      g_assert_cmpstr (output, ==, "å");

      g_free (output);
      g_object_unref (transliterator);
    }

  error = NULL;
  transliterator = translit_transliterator_get ("m17n", "ja-anthy",
						&error);
  g_assert_no_error (error);

  if (transliterator)
    {
      gchar *output;
      guint endpos;
      GError *error;

      error = NULL;
      output = translit_transliterator_transliterate (transliterator,
						      "kakikukeko",
						      &endpos,
						      &error);
      g_assert_no_error (error);
      g_assert_cmpint (endpos, ==, 10);
      g_assert_cmpstr (output, ==, "かきくけこ");

      g_free (output);

      output = translit_transliterator_transliterate (transliterator,
						      "kakikukek",
						      &endpos,
						      &error);
      g_assert_no_error (error);
      g_assert_cmpint (endpos, ==, 9);
      g_assert_cmpstr (output, ==, "かきくけk");

      g_free (output);
      g_object_unref (transliterator);
    }
}

static void
basic_icu (void)
{
  TranslitTransliterator *transliterator;
  GError *error;

  error = NULL;
  transliterator = translit_transliterator_get ("icu", "Latin-Katakana",
						&error);
  g_assert_no_error (error);

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

      output = translit_transliterator_transliterate (transliterator,
						      "kakikukeko",
						      &endpos,
						      &error);
      g_assert_no_error (error);
      g_assert_cmpint (endpos, ==, 10);
      g_assert_cmpstr (output, ==, "カキクケコ");

      g_free (output);

      output = translit_transliterator_transliterate (transliterator,
						      "kakikukek",
						      &endpos,
						      &error);
      g_assert_no_error (error);
      g_assert_cmpint (endpos, ==, 9);
      g_assert_cmpstr (output, ==, "カキクケク");

      g_free (output);

      output = translit_transliterator_transliterate (transliterator,
						      "tachitsuteto",
						      &endpos,
						      &error);
      g_assert_no_error (error);
      g_assert_cmpint (endpos, ==, 12);
      g_assert_cmpstr (output, ==, "タチツテト");

      g_free (output);

      output = translit_transliterator_transliterate (transliterator,
						      "tachitsutet",
						      &endpos,
						      &error);
      g_assert_no_error (error);
      g_assert_cmpint (endpos, ==, 11);
      g_assert_cmpstr (output, ==, "タチツテテ");

      g_free (output);

      output = translit_transliterator_transliterate (transliterator,
						      "tachitsutec",
						      &endpos,
						      &error);
      g_assert_no_error (error);
      g_assert_cmpint (endpos, ==, 11);
      g_assert_cmpstr (output, ==, "タチツテク");

      g_free (output);

      output = translit_transliterator_transliterate (transliterator,
						      "tachitetsut",
						      &endpos,
						      &error);
      g_assert_no_error (error);
      g_assert_cmpint (endpos, ==, 11);
      g_assert_cmpstr (output, ==, "タチテツテ");

      g_free (output);
      g_object_unref (transliterator);
    }

  error = NULL;
  transliterator = translit_transliterator_get ("icu", "Russian-Latin/BGN",
						&error);
  g_assert_no_error (error);

  if (transliterator)
    {
      gchar *output;
      guint endpos;
      GError *error;

      error = NULL;
      output = translit_transliterator_transliterate (transliterator,
						      "Ф",
						      &endpos,
						      &error);
      g_assert_no_error (error);
      g_assert_cmpint (endpos, ==, 1);
      g_assert_cmpstr (output, ==, "F");

      g_free (output);

      output = translit_transliterator_transliterate (transliterator,
						      "Щ",
						      &endpos,
						      &error);
      g_assert_no_error (error);
      g_assert_cmpint (endpos, ==, 1);
      g_assert_cmpstr (output, ==, "SHCH");

      g_free (output);

      g_object_unref (transliterator);
    }

  error = NULL;
  transliterator = translit_transliterator_get ("icu", "Hiragana-Latin",
						&error);
  g_assert_no_error (error);

  if (transliterator)
    {
      gchar *output;
      guint endpos;
      GError *error;

      error = NULL;
      output = translit_transliterator_transliterate (transliterator,
						      "か",
						      &endpos,
						      &error);
      g_assert_no_error (error);
      g_assert_cmpint (endpos, ==, 1);
      g_assert_cmpstr (output, ==, "ka");

      g_free (output);

      output = translit_transliterator_transliterate (transliterator,
						      "かきくけこ",
						      &endpos,
						      &error);
      g_assert_no_error (error);
      g_assert_cmpint (endpos, ==, 5);
      g_assert_cmpstr (output, ==, "kakikukeko");

      g_free (output);

      g_object_unref (transliterator);
    }

  error = NULL;
  transliterator = translit_transliterator_get ("icu", "Devanagari-Latin",
						&error);
  g_assert_no_error (error);

  if (transliterator)
    {
      gchar *output;
      guint endpos;
      GError *error;

      error = NULL;
      output = translit_transliterator_transliterate (transliterator,
						      "सॉ",
						      &endpos,
						      &error);
      g_assert_no_error (error);
      g_assert_cmpint (endpos, ==, 2);
      g_assert_cmpstr (output, ==, "sŏ");

      g_free (output);

      output = translit_transliterator_transliterate (transliterator,
						      "सॉफ्टवेअरचे",
						      &endpos,
						      &error);
      g_assert_no_error (error);
      g_assert_cmpint (endpos, ==, 11);
      g_assert_cmpstr (output, ==, "sŏphṭavē'aracē");

      g_free (output);

      g_object_unref (transliterator);
    }

}

int
main (int argc, char **argv) {
  g_type_init ();
  g_test_init (&argc, &argv, NULL);
  g_test_add_func ("/libtranslit/basic/load", basic_load);
  g_test_add_func ("/libtranslit/basic/m17n", basic_m17n);
  g_test_add_func ("/libtranslit/basic/icu", basic_icu);
  return g_test_run ();
}
