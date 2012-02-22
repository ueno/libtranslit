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

#ifndef __TRANSLIT_FILTER_H__
#define __TRANSLIT_FILTER_H__

#include <glib-object.h>
#include <libtranslit/translittypes.h>

G_BEGIN_DECLS

#define TRANSLIT_TYPE_FILTER (translit_filter_get_type())
#define TRANSLIT_FILTER(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), TRANSLIT_TYPE_FILTER, TranslitFilter))
#define TRANSLIT_FILTER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), TRANSLIT_TYPE_FILTER, TranslitFilterClass))
#define TRANSLIT_IS_FILTER(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TRANSLIT_TYPE_FILTER))
#define TRANSLIT_IS_FILTER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), TRANSLIT_TYPE_FILTER))
#define TRANSLIT_FILTER_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), TRANSLIT_TYPE_FILTER, TranslitFilterClass))

typedef struct _TranslitFilter TranslitFilter;
typedef struct _TranslitFilterClass TranslitFilterClass;
typedef struct _TranslitFilterPrivate TranslitFilterPrivate;

struct _TranslitFilter
{
  /*< private >*/
  GObject parent;
  
  TranslitFilterPrivate *priv;
};

struct _TranslitFilterClass
{
  /*< private >*/
  GObjectClass parent_class;

  /*< public >*/
  gboolean (*filter)      (TranslitFilter      *filter,
                           gchar                ascii,
                           TranslitModifierType modifiers);
  gchar   *(*poll_output) (TranslitFilter      *filter);
};

#define TRANSLIT_MODULE_ERROR translit_module_error_quark ()
typedef enum {
  TRANSLIT_MODULE_ERROR_FAILED
} TranslitModuleErrorEnum;

GType           translit_filter_get_type    (void) G_GNUC_CONST;
gboolean        translit_filter_filter      (TranslitFilter      *filter,
                                             gchar                ascii,
                                             TranslitModifierType modifiers);
gchar          *translit_filter_poll_output (TranslitFilter      *filter);

TranslitFilter *translit_filter_get         (const gchar         *backend,
                                             const gchar         *language,
                                             const gchar         *name);
void            translit_filter_implement_backend
                                            (const gchar         *backend,
                                             GType                type);

G_END_DECLS

#endif	/* __TRANSLIT_FILTER_H__ */
