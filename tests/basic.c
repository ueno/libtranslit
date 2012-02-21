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
  TranslitFilter *filter;

  filter = translit_filter_get ("m17n:hi:inscript");
  if (filter)
    {
      gboolean retval;
      gchar *output;

      retval = translit_filter_filter (filter, 'a', 0);
      g_assert_cmpint (retval, ==, 0);

      output = translit_filter_poll_output (filter);
      g_assert_cmpstr (output, ==, "ो");

      g_free (output);
      g_object_unref (filter);
    }
}

int
main (int argc, char **argv) {
  g_type_init ();
  g_test_init (&argc, &argv, NULL);
  g_test_add_func ("/libtranslit/basic", basic);
  return g_test_run ();
}
