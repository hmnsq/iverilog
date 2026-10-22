#ifndef IVL_vvp_cobject_H
#define IVL_vvp_cobject_H
/*
 * Copyright (c) 2012-2025 Stephen Williams (steve@icarus.com)
 *
 *    This source code is free software; you can redistribute it
 *    and/or modify it in source code form under the terms of the GNU
 *    General Public License as published by the Free Software
 *    Foundation; either version 2 of the License, or (at your option)
 *    any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program; if not, write to the Free Software
 *    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

# include  <string>
# include  <stdint.h>
# include  "vvp_object.h"
# include  "class_type.h"

class vvp_vector4_t;

class vvp_cobject : সার্বজনিক vvp_object {

    সার্বজনিক:
      explicit vvp_cobject(ধ্রুব class_type*defn);
      ~vvp_cobject() override;

      শূন্য set_vec4(size_t pid, ধ্রুব vvp_vector4_t&val);
      শূন্য get_vec4(size_t pid, vvp_vector4_t&val);

      শূন্য set_real(size_t pid, দ্বিগুণ val);
      দ্বিগুণ get_real(size_t pid);

      শূন্য set_string(size_t pid, ধ্রুব std::string&val);
      std::string get_string(size_t pid);

      শূন্য set_object(size_t pid, ধ্রুব vvp_object_t&val, size_t idx);
      শূন্য get_object(size_t pid, vvp_object_t&val, size_t idx);

      শূন্য shallow_copy(ধ্রুব vvp_object*that) override;

    গুপ্ত:
      ধ্রুব class_type* defn_;
	// For now, only support 32bit bool signed properties.
      class_type::inst_t properties_;
};

#endif /* IVL_vvp_cobject_H */
