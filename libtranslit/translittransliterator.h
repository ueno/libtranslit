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

#ifndef __TRANSLIT_TRANSLITERATOR_H__
#define __TRANSLIT_TRANSLITERATOR_H__

#include <glib-object.h>

G_BEGIN_DECLS

#define TRANSLIT_TYPE_TRANSLITERATOR (translit_transliterator_get_type())
#define TRANSLIT_TRANSLITERATOR(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), TRANSLIT_TYPE_TRANSLITERATOR, TranslitTransliterator))
#define TRANSLIT_TRANSLITERATOR_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), TRANSLIT_TYPE_TRANSLITERATOR, TranslitTransliteratorClass))
#define TRANSLIT_IS_TRANSLITERATOR(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TRANSLIT_TYPE_TRANSLITERATOR))
#define TRANSLIT_IS_TRANSLITERATOR_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), TRANSLIT_TYPE_TRANSLITERATOR))
#define TRANSLIT_TRANSLITERATOR_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), TRANSLIT_TYPE_TRANSLITERATOR, TranslitTransliteratorClass))

typedef struct _TranslitTransliterator TranslitTransliterator;
typedef struct _TranslitTransliteratorClass TranslitTransliteratorClass;
typedef struct _TranslitTransliteratorPrivate TranslitTransliteratorPrivate;

struct _TranslitTransliterator
{
  /*< private >*/
  GObject parent;
  
  TranslitTransliteratorPrivate *priv;
};

struct _TranslitTransliteratorClass
{
  /*< private >*/
  GObjectClass parent_class;

  /*< public >*/
  gchar *(*transliterate) (TranslitTransliterator *transliterator,
                           const gchar            *input,
                           guint                  *endpos,
                           GError                **error);
};

GQuark translit_error_quark (void);

#define TRANSLIT_ERROR translit_error_quark ()
typedef enum {
  TRANSLIT_ERROR_NO_SUCH_BACKEND,
  TRANSLIT_ERROR_LOAD_FAILED,
  TRANSLIT_ERROR_INVALID_INPUT,
  TRANSLIT_ERROR_FAILED
} TranslitErrorEnum;

GType                   translit_transliterator_get_type
                        (void) G_GNUC_CONST;
gchar                  *translit_transliterator_transliterate
                        (TranslitTransliterator *transliterator,
                         const gchar            *input,
                         guint                  *endpos,
                         GError                **error);

TranslitTransliterator *translit_transliterator_get
                        (const gchar            *backend,
                         const gchar            *name,
                         GError                **error);
void                    translit_implement_transliterator
                        (const gchar            *backend,
                         GType                   type);

G_END_DECLS

#endif	/* __TRANSLIT_TRANSLITERATOR_H__ */
