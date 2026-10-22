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
class class_type : عام __vpiHandle {

    عام:
      بنية inst_x;
      تعريفنوع inst_x*inst_t;

    عام:
      explicit class_type(ثابت std::string&nam, size_t nprop);
      ~class_type() override;

	// This is the name of the class type.
      مضمن ثابت std::string&class_name(فراغ) ثابت { أرجع class_name_; }
	// Number of properties in the class definition.
      مضمن size_t property_کيراءن(فراغ) ثابت { أرجع properties_.size(); }

	// Set the details about the property. This is used during
	// parse of the .vvp file to fill in the details of the
	// property for the class definition.
      فراغ set_property(size_t idx, ثابت std::string&name, ثابت std::string&type, uint64_t array_size);

	// This method is called after all the properties are
	// defined. This calculates information about the definition.
      فراغ finish_setup(فراغ);

    عام:
	// Constructors and destructors for making instances.
      inst_t instance_new() ثابت;
      فراغ instance_delete(inst_t) ثابت;

      فراغ set_vec4(inst_t inst, size_t pid, ثابت vvp_vector4_t&val) ثابت;
      فراغ get_vec4(inst_t inst, size_t pid, vvp_vector4_t&val) ثابت;
      فراغ set_real(inst_t inst, size_t pid, مزدوج val) ثابت;
      مزدوج get_real(inst_t inst, size_t pid) ثابت;
      فراغ set_string(inst_t inst, size_t pid, ثابت std::string&val) ثابت;
      std::string get_string(inst_t inst, size_t pid) ثابت;
      فراغ set_object(inst_t inst, size_t pid, ثابت vvp_object_t&val, size_t idx) ثابت;
      فراغ get_object(inst_t inst, size_t pid, vvp_object_t&val, size_t idx) ثابت;

      فراغ copy_property(inst_t dst, size_t idx, inst_t src) ثابت;

    عام: // VPI related methods
      صحيح get_type_code(فراغ) ثابت override;

    خاص:
      std::string class_name_;

      بنية prop_t {
	    std::string name;
	    class_property_t*type;
      };
      std::vector<prop_t> properties_;
      size_t instance_size_;
};

#endif /* IVL_class_type_H */
