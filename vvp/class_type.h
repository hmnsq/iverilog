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
class class_type : সার্ৱজনিক __vpiHandle {

    সার্ৱজনিক:
      সংরচনা inst_x;
      প্রকারনির্দেশ inst_x*inst_t;

    সার্ৱজনিক:
      explicit class_type(ধ্রুৱ std::string&nam, size_t nprop);
      ~class_type() override;

	// This is the name of the class type.
      অন্তঃস্থ ধ্রুৱ std::string&class_name(শূন্য) ধ্রুৱ { ঘূৰাই class_name_; }
	// Number of properties in the class definition.
      অন্তঃস্থ size_t property_গুনতি(শূন্য) ধ্রুৱ { ঘূৰাই properties_.size(); }

	// Set the details about the property. This is used during
	// parse of the .vvp file to fill in the details of the
	// property for the class definition.
      শূন্য set_property(size_t idx, ধ্রুৱ std::string&name, ধ্রুৱ std::string&type, uint64_t array_size);

	// This method is called after all the properties are
	// defined. This calculates information about the definition.
      শূন্য finish_setup(শূন্য);

    সার্ৱজনিক:
	// Constructors and destructors for making instances.
      inst_t instance_new() ধ্রুৱ;
      শূন্য instance_delete(inst_t) ধ্রুৱ;

      শূন্য set_vec4(inst_t inst, size_t pid, ধ্রুৱ vvp_vector4_t&val) ধ্রুৱ;
      শূন্য get_vec4(inst_t inst, size_t pid, vvp_vector4_t&val) ধ্রুৱ;
      শূন্য set_real(inst_t inst, size_t pid, দ্ৱিগুণ val) ধ্রুৱ;
      দ্ৱিগুণ get_real(inst_t inst, size_t pid) ধ্রুৱ;
      শূন্য set_string(inst_t inst, size_t pid, ধ্রুৱ std::string&val) ধ্রুৱ;
      std::string get_string(inst_t inst, size_t pid) ধ্রুৱ;
      শূন্য set_object(inst_t inst, size_t pid, ধ্রুৱ vvp_object_t&val, size_t idx) ধ্রুৱ;
      শূন্য get_object(inst_t inst, size_t pid, vvp_object_t&val, size_t idx) ধ্রুৱ;

      শূন্য copy_property(inst_t dst, size_t idx, inst_t src) ধ্রুৱ;

    সার্ৱজনিক: // VPI related methods
      পূর্ণাঙ্ক get_type_code(শূন্য) ধ্রুৱ override;

    গুপ্ত:
      std::string class_name_;

      সংরচনা prop_t {
	    std::string name;
	    class_property_t*type;
      };
      std::vector<prop_t> properties_;
      size_t instance_size_;
};

#endif /* IVL_class_type_H */
