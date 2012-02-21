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

#ifndef __TRANSLIT_TYPES_H__
#define __TRANSLIT_TYPES_H__

typedef enum
{
  TRANSLIT_SHIFT_MASK    = 1 << 0,
  TRANSLIT_LOCK_MASK	    = 1 << 1,
  TRANSLIT_CONTROL_MASK  = 1 << 2,
  TRANSLIT_MOD1_MASK	    = 1 << 3,
  TRANSLIT_MOD2_MASK	    = 1 << 4,
  TRANSLIT_MOD3_MASK	    = 1 << 5,
  TRANSLIT_MOD4_MASK	    = 1 << 6,
  TRANSLIT_MOD5_MASK	    = 1 << 7,
  TRANSLIT_BUTTON1_MASK  = 1 << 8,
  TRANSLIT_BUTTON2_MASK  = 1 << 9,
  TRANSLIT_BUTTON3_MASK  = 1 << 10,
  TRANSLIT_BUTTON4_MASK  = 1 << 11,
  TRANSLIT_BUTTON5_MASK  = 1 << 12,

  /* The next few modifiers are used by XKB, so we skip to the end.
   * Bits 15 - 25 are currently unused. Bit 29 is used internally.
   */
  
  TRANSLIT_SUPER_MASK    = 1 << 26,
  TRANSLIT_HYPER_MASK    = 1 << 27,
  TRANSLIT_META_MASK     = 1 << 28,
  
  TRANSLIT_RELEASE_MASK  = 1 << 30,

  TRANSLIT_MODIFIER_MASK = 0x5c001fff
} TranslitModifierType;

#endif	/* __TRANSLIT_TYPES_H__ */
