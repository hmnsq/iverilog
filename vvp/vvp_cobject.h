#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ IVL_vvp_cobject_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IVL_vvp_cobject_H
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

class vvp_cobject : ਜਨਤਕ vvp_object {

    ਜਨਤਕ:
      explicit vvp_cobject(ਅਚਲ class_type*defn);
      ~vvp_cobject() override;

      ਖਾਲੀ ਸੈੱਟ_vec4(size_t pid, ਅਚਲ vvp_vector4_t&val);
      ਖਾਲੀ get_vec4(size_t pid, vvp_vector4_t&val);

      ਖਾਲੀ ਸੈੱਟ_real(size_t pid, ਦੁੱਗਣਾ val);
      ਦੁੱਗਣਾ get_real(size_t pid);

      ਖਾਲੀ ਸੈੱਟ_string(size_t pid, ਅਚਲ std::string&val);
      std::string get_string(size_t pid);

      ਖਾਲੀ ਸੈੱਟ_object(size_t pid, ਅਚਲ vvp_object_t&val, size_t idx);
      ਖਾਲੀ get_object(size_t pid, vvp_object_t&val, size_t idx);

      ਖਾਲੀ shallow_copy(ਅਚਲ vvp_object*that) override;

    ਨਿਜੀ:
      ਅਚਲ class_type* defn_;
	// For now, only support 32bit bool signed properties.
      class_type::inst_t properties_;
};

#ਜੇ_ਅੰਤ /* IVL_vvp_cobject_H */
