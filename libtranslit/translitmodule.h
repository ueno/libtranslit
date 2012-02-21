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

#ifndef __TRANSLIT_MODULE_H__
#define __TRANSLIT_MODULE_H__

#include <gmodule.h>

G_BEGIN_DECLS

typedef struct _TranslitModule TranslitModule;

GType           translit_module_get_type (void) G_GNUC_CONST;
TranslitModule *translit_module_new      (const gchar *filename);

G_END_DECLS

#endif	/* __TRANSLIT_MODULE_H__ */
