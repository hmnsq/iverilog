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
class class_type : பொது __vpiHandle {

    பொது:
      கட்டமைப்பு inst_x;
      வகைவரை inst_x*inst_t;

    பொது:
      explicit class_type(மாறிலி std::string&nam, size_t nprop);
      ~class_type() override;

	// This is the name of the class type.
      உள்வரி மாறிலி std::string&class_name(வெறுமை) மாறிலி { திரும்பு class_name_; }
	// Number of properties in the class definition.
      உள்வரி size_t property_மொத்தம்(வெறுமை) மாறிலி { திரும்பு properties_.size(); }

	// Set the details about the property. This is used during
	// parse of the .vvp file to fill in the details of the
	// property for the class definition.
      வெறுமை set_property(size_t idx, மாறிலி std::string&name, மாறிலி std::string&type, uint64_t array_size);

	// This method is called after all the properties are
	// defined. This calculates information about the definition.
      வெறுமை finish_setup(வெறுமை);

    பொது:
	// Constructors and destructors for making instances.
      inst_t instance_new() மாறிலி;
      வெறுமை instance_delete(inst_t) மாறிலி;

      வெறுமை set_vec4(inst_t inst, size_t pid, மாறிலி vvp_vector4_t&val) மாறிலி;
      வெறுமை get_vec4(inst_t inst, size_t pid, vvp_vector4_t&val) மாறிலி;
      வெறுமை set_real(inst_t inst, size_t pid, இரட்டை val) மாறிலி;
      இரட்டை get_real(inst_t inst, size_t pid) மாறிலி;
      வெறுமை set_string(inst_t inst, size_t pid, மாறிலி std::string&val) மாறிலி;
      std::string get_string(inst_t inst, size_t pid) மாறிலி;
      வெறுமை set_object(inst_t inst, size_t pid, மாறிலி vvp_object_t&val, size_t idx) மாறிலி;
      வெறுமை get_object(inst_t inst, size_t pid, vvp_object_t&val, size_t idx) மாறிலி;

      வெறுமை copy_property(inst_t dst, size_t idx, inst_t src) மாறிலி;

    பொது: // VPI related methods
      முழு get_type_code(வெறுமை) மாறிலி override;

    தனிப்பட்ட:
      std::string class_name_;

      கட்டமைப்பு prop_t {
	    std::string name;
	    class_property_t*type;
      };
      std::vector<prop_t> properties_;
      size_t instance_size_;
};

#endif /* IVL_class_type_H */
