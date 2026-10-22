#अगर_अघोषित IVL_vvp_cobject_H
#घोषणा IVL_vvp_cobject_H
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

class vvp_cobject : खुला vvp_object {

    खुला:
      explicit vvp_cobject(स्थिर class_type*defn);
      ~vvp_cobject() override;

      व्योम set_vec4(माप_प्रकार pid, स्थिर vvp_vector4_t&val);
      व्योम get_vec4(माप_प्रकार pid, vvp_vector4_t&val);

      व्योम set_real(माप_प्रकार pid, द्विगुन val);
      द्विगुन get_real(माप_प्रकार pid);

      व्योम set_string(माप_प्रकार pid, स्थिर std::string&val);
      std::string get_string(माप_प्रकार pid);

      व्योम set_object(माप_प्रकार pid, स्थिर vvp_object_t&val, माप_प्रकार idx);
      व्योम get_object(माप_प्रकार pid, vvp_object_t&val, माप_प्रकार idx);

      व्योम shallow_copy(स्थिर vvp_object*that) override;

    निजी:
      स्थिर class_type* defn_;
	// For now, only support 32bit bool signed properties.
      class_type::inst_t properties_;
};

#पूर्ण_अगर /* IVL_vvp_cobject_H */
