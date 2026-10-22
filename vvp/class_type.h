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
class class_type : ಸಾರ್ವಜನಿಕ __vpiHandle {

    ಸಾರ್ವಜನಿಕ:
      ಸಂರಚನಾ inst_x;
      ಪ್ರಕಾರನಿರ್ದೇಶ inst_x*inst_t;

    ಸಾರ್ವಜನಿಕ:
      explicit class_type(ಧ್ರುವ std::string&nam, size_t nprop);
      ~class_type() override;

	// This is the name of the class type.
      ಅನ್ತಃಸ್ಥ ಧ್ರುವ std::string&class_name(ಶೂನ್ಯ) ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ class_name_; }
	// Number of properties in the class definition.
      ಅನ್ತಃಸ್ಥ size_t property_ಎಣಿಕೆ(ಶೂನ್ಯ) ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ properties_.size(); }

	// Set the details about the property. This is used during
	// parse of the .vvp file to fill in the details of the
	// property for the class definition.
      ಶೂನ್ಯ set_property(size_t idx, ಧ್ರುವ std::string&name, ಧ್ರುವ std::string&type, uint64_t array_size);

	// This method is called after all the properties are
	// defined. This calculates information about the definition.
      ಶೂನ್ಯ finish_setup(ಶೂನ್ಯ);

    ಸಾರ್ವಜನಿಕ:
	// Constructors and destructors for making instances.
      inst_t instance_new() ಧ್ರುವ;
      ಶೂನ್ಯ instance_delete(inst_t) ಧ್ರುವ;

      ಶೂನ್ಯ set_vec4(inst_t inst, size_t pid, ಧ್ರುವ vvp_vector4_t&val) ಧ್ರುವ;
      ಶೂನ್ಯ get_vec4(inst_t inst, size_t pid, vvp_vector4_t&val) ಧ್ರುವ;
      ಶೂನ್ಯ set_real(inst_t inst, size_t pid, ದ್ವಿಗುಣ val) ಧ್ರುವ;
      ದ್ವಿಗುಣ get_real(inst_t inst, size_t pid) ಧ್ರುವ;
      ಶೂನ್ಯ set_string(inst_t inst, size_t pid, ಧ್ರುವ std::string&val) ಧ್ರುವ;
      std::string get_string(inst_t inst, size_t pid) ಧ್ರುವ;
      ಶೂನ್ಯ set_object(inst_t inst, size_t pid, ಧ್ರುವ vvp_object_t&val, size_t idx) ಧ್ರುವ;
      ಶೂನ್ಯ get_object(inst_t inst, size_t pid, vvp_object_t&val, size_t idx) ಧ್ರುವ;

      ಶೂನ್ಯ copy_property(inst_t dst, size_t idx, inst_t src) ಧ್ರುವ;

    ಸಾರ್ವಜನಿಕ: // VPI related methods
      ಪೂರ್ಣಾಙ್ಕ get_type_code(ಶೂನ್ಯ) ಧ್ರುವ override;

    ಗುಪ್ತ:
      std::string class_name_;

      ಸಂರಚನಾ prop_t {
	    std::string name;
	    class_property_t*type;
      };
      std::vector<prop_t> properties_;
      size_t instance_size_;
};

#endif /* IVL_class_type_H */
