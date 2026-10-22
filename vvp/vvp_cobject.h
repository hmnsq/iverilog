#جيڪڏهن_وصف_نه_ٿيل IVL_vvp_cobject_H
#وصف_ڪريو IVL_vvp_cobject_H
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

class vvp_cobject : عام vvp_object {

    عام:
      explicit vvp_cobject(ثابت class_type*defn);
      ~vvp_cobject() override;

      فراغ سيٽ_vec4(size_t pid, ثابت vvp_vector4_t&val);
      فراغ get_vec4(size_t pid, vvp_vector4_t&val);

      فراغ سيٽ_real(size_t pid, مزدوج val);
      مزدوج get_real(size_t pid);

      فراغ سيٽ_string(size_t pid, ثابت std::string&val);
      std::string get_string(size_t pid);

      فراغ سيٽ_object(size_t pid, ثابت vvp_object_t&val, size_t idx);
      فراغ get_object(size_t pid, vvp_object_t&val, size_t idx);

      فراغ shallow_copy(ثابت vvp_object*that) override;

    خاص:
      ثابت class_type* defn_;
	// For now, only support 32bit bool signed properties.
      class_type::inst_t properties_;
};

#جيڪڏهن_پورو /* IVL_vvp_cobject_H */
