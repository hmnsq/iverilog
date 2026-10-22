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

class vvp_cobject : ಸಾರ್ವಜನಿಕ vvp_object {

    ಸಾರ್ವಜನಿಕ:
      explicit vvp_cobject(ಧ್ರುವ class_type*defn);
      ~vvp_cobject() override;

      ಶೂನ್ಯ set_vec4(size_t pid, ಧ್ರುವ vvp_vector4_t&val);
      ಶೂನ್ಯ get_vec4(size_t pid, vvp_vector4_t&val);

      ಶೂನ್ಯ set_real(size_t pid, ದ್ವಿಗುಣ val);
      ದ್ವಿಗುಣ get_real(size_t pid);

      ಶೂನ್ಯ set_string(size_t pid, ಧ್ರುವ std::string&val);
      std::string get_string(size_t pid);

      ಶೂನ್ಯ set_object(size_t pid, ಧ್ರುವ vvp_object_t&val, size_t idx);
      ಶೂನ್ಯ get_object(size_t pid, vvp_object_t&val, size_t idx);

      ಶೂನ್ಯ shallow_copy(ಧ್ರುವ vvp_object*that) override;

    ಗುಪ್ತ:
      ಧ್ರುವ class_type* defn_;
	// For now, only support 32bit bool signed properties.
      class_type::inst_t properties_;
};

#endif /* IVL_vvp_cobject_H */
