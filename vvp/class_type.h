#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ IVL_class_type_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IVL_class_type_H
/*
 * Copyright (c) 2012-2025 Stephen Williams (steve@icarus.com)
 *
 *    This source code is free software; you can redistribute it
 *    and/or modify it in source code form under the terms of the GNU
 *    General Public License as published by the Free Software
 *    Foundation; either version 2 of the License, or (at your option)
 *    any later version
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
# include  <vector>
# include  "vpi_priv.h"

class class_property_t;
class vvp_vector4_t;

/*
 * This represents the TYPE information for a class. A %new operator
 * uses this information to figure out how to construct an actual
 * instance.
 */
class class_type : ਜਨਤਕ __vpiHandle {

    ਜਨਤਕ:
      ਬਣਤਰ inst_x;
      ਕਿਸਮਪਰਿਭਾਸ਼ਾ inst_x*inst_t;

    ਜਨਤਕ:
      explicit class_type(ਅਚਲ std::string&nam, size_t nprop);
      ~class_type() override;

	// This is the name of the class type.
      ਸਤਰਅੰਦਰ ਅਚਲ std::string&class_name(ਖਾਲੀ) ਅਚਲ { ਵਾਪਸ class_name_; }
	// Number of properties in the class definition.
      ਸਤਰਅੰਦਰ size_t property_ਗਿਣਤੀ(ਖਾਲੀ) ਅਚਲ { ਵਾਪਸ properties_.ਮਾਪ(); }

	// Set the details about the property. This is used during
	// parse of the .vvp file to fill in the details of the
	// property for the class definition.
      ਖਾਲੀ ਸੈੱਟ_property(size_t idx, ਅਚਲ std::string&name, ਅਚਲ std::string&type, uint64_t array_ਮਾਪ_2);

	// This method is called after all the properties are
	// defined. This calculates information about the definition.
      ਖਾਲੀ ਅੰਤ_ਤਿਆਰੀ(ਖਾਲੀ);

    ਜਨਤਕ:
	// Constructors and destructors for making instances.
      inst_t instance_new() ਅਚਲ;
      ਖਾਲੀ instance_delete(inst_t) ਅਚਲ;

      ਖਾਲੀ ਸੈੱਟ_vec4(inst_t inst, size_t pid, ਅਚਲ vvp_vector4_t&val) ਅਚਲ;
      ਖਾਲੀ get_vec4(inst_t inst, size_t pid, vvp_vector4_t&val) ਅਚਲ;
      ਖਾਲੀ ਸੈੱਟ_real(inst_t inst, size_t pid, ਦੁੱਗਣਾ val) ਅਚਲ;
      ਦੁੱਗਣਾ get_real(inst_t inst, size_t pid) ਅਚਲ;
      ਖਾਲੀ ਸੈੱਟ_string(inst_t inst, size_t pid, ਅਚਲ std::string&val) ਅਚਲ;
      std::string get_string(inst_t inst, size_t pid) ਅਚਲ;
      ਖਾਲੀ ਸੈੱਟ_object(inst_t inst, size_t pid, ਅਚਲ vvp_object_t&val, size_t idx) ਅਚਲ;
      ਖਾਲੀ get_object(inst_t inst, size_t pid, vvp_object_t&val, size_t idx) ਅਚਲ;

      ਖਾਲੀ copy_property(inst_t dst, size_t idx, inst_t src) ਅਚਲ;

    ਜਨਤਕ: // VPI related methods
      ਪੂਰਨ get_type_ਕੋਡ(ਖਾਲੀ) ਅਚਲ override;

    ਨਿਜੀ:
      std::string class_name_;

      ਬਣਤਰ prop_t {
	    std::string name;
	    class_property_t*type;
      };
      std::ਸਦਿਸ਼<prop_t> properties_;
      size_t instance_ਮਾਪ_2;
};

#ਜੇ_ਅੰਤ /* IVL_class_type_H */
