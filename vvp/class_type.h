#ifndef IVL_class_type_H
#define IVL_class_type_H
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
class class_type : عمومی __vpiHandle {

    عمومی:
      ساختار inst_x;
      تعریفنوع inst_x*inst_t;

    عمومی:
      explicit class_type(ثابت std::string&nam, size_t nprop);
      ~class_type() override;

	// This is the name of the class type.
      درونخطی ثابت std::string&class_name(تهی) ثابت { بازگشت class_name_; }
	// Number of properties in the class definition.
      درونخطی size_t property_شمار(تهی) ثابت { بازگشت properties_.size(); }

	// Set the details about the property. This is used during
	// parse of the .vvp file to fill in the details of the
	// property for the class definition.
      تهی set_property(size_t idx, ثابت std::string&name, ثابت std::string&type, uint64_t array_size);

	// This method is called after all the properties are
	// defined. This calculates information about the definition.
      تهی finish_setup(تهی);

    عمومی:
	// Constructors and destructors for making instances.
      inst_t instance_new() ثابت;
      تهی instance_delete(inst_t) ثابت;

      تهی set_vec4(inst_t inst, size_t pid, ثابت vvp_vector4_t&val) ثابت;
      تهی get_vec4(inst_t inst, size_t pid, vvp_vector4_t&val) ثابت;
      تهی set_real(inst_t inst, size_t pid, دوگانه val) ثابت;
      دوگانه get_real(inst_t inst, size_t pid) ثابت;
      تهی set_string(inst_t inst, size_t pid, ثابت std::string&val) ثابت;
      std::string get_string(inst_t inst, size_t pid) ثابت;
      تهی set_object(inst_t inst, size_t pid, ثابت vvp_object_t&val, size_t idx) ثابت;
      تهی get_object(inst_t inst, size_t pid, vvp_object_t&val, size_t idx) ثابت;

      تهی copy_property(inst_t dst, size_t idx, inst_t src) ثابت;

    عمومی: // VPI related methods
      صحیح get_type_code(تهی) ثابت override;

    خصوصی:
      std::string class_name_;

      ساختار prop_t {
	    std::string name;
	    class_property_t*type;
      };
      std::vector<prop_t> properties_;
      size_t instance_size_;
};

#endif /* IVL_class_type_H */
