#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ IVL_class_type_H
#ವ್ಯಾಖ್ಯಾನಿಸು IVL_class_type_H
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
      ರಚನೆ inst_x;
      ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ inst_x*inst_t;

    ಸಾರ್ವಜನಿಕ:
      explicit class_type(ಸ್ಥಿರಾಂಕ std::string&nam, size_t nprop);
      ~class_type() override;

	// This is the name of the class type.
      ಸಾಲೊಳಗೆ ಸ್ಥಿರಾಂಕ std::string&class_name(ಶೂನ್ಯ) ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ class_name_; }
	// Number of properties in the class definition.
      ಸಾಲೊಳಗೆ size_t property_ಎಣಿಕೆ(ಶೂನ್ಯ) ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ properties_.ಗಾತ್ರ_2(); }

	// Set the details about the property. This is used during
	// parse of the .vvp file to fill in the details of the
	// property for the class definition.
      ಶೂನ್ಯ ಗಣ_property(size_t idx, ಸ್ಥಿರಾಂಕ std::string&name, ಸ್ಥಿರಾಂಕ std::string&type, uint64_t array_ಗಾತ್ರ_2);

	// This method is called after all the properties are
	// defined. This calculates information about the definition.
      ಶೂನ್ಯ ಮುಕ್ತಾಯ_ಸಿದ್ಧತೆ(ಶೂನ್ಯ);

    ಸಾರ್ವಜನಿಕ:
	// Constructors and destructors for making instances.
      inst_t instance_new() ಸ್ಥಿರಾಂಕ;
      ಶೂನ್ಯ instance_delete(inst_t) ಸ್ಥಿರಾಂಕ;

      ಶೂನ್ಯ ಗಣ_vec4(inst_t inst, size_t pid, ಸ್ಥಿರಾಂಕ vvp_vector4_t&val) ಸ್ಥಿರಾಂಕ;
      ಶೂನ್ಯ get_vec4(inst_t inst, size_t pid, vvp_vector4_t&val) ಸ್ಥಿರಾಂಕ;
      ಶೂನ್ಯ ಗಣ_real(inst_t inst, size_t pid, ದ್ವಿಗುಣ val) ಸ್ಥಿರಾಂಕ;
      ದ್ವಿಗುಣ get_real(inst_t inst, size_t pid) ಸ್ಥಿರಾಂಕ;
      ಶೂನ್ಯ ಗಣ_string(inst_t inst, size_t pid, ಸ್ಥಿರಾಂಕ std::string&val) ಸ್ಥಿರಾಂಕ;
      std::string get_string(inst_t inst, size_t pid) ಸ್ಥಿರಾಂಕ;
      ಶೂನ್ಯ ಗಣ_object(inst_t inst, size_t pid, ಸ್ಥಿರಾಂಕ vvp_object_t&val, size_t idx) ಸ್ಥಿರಾಂಕ;
      ಶೂನ್ಯ get_object(inst_t inst, size_t pid, vvp_object_t&val, size_t idx) ಸ್ಥಿರಾಂಕ;

      ಶೂನ್ಯ copy_property(inst_t dst, size_t idx, inst_t src) ಸ್ಥಿರಾಂಕ;

    ಸಾರ್ವಜನಿಕ: // VPI related methods
      ಪೂರ್ಣಾಂಕ get_type_ಸಂಕೇತ(ಶೂನ್ಯ) ಸ್ಥಿರಾಂಕ override;

    ಖಾಸಗಿ:
      std::string class_name_;

      ರಚನೆ prop_t {
	    std::string name;
	    class_property_t*type;
      };
      std::ಸದಿಶ<prop_t> properties_;
      size_t instance_ಗಾತ್ರ_2;
};

#ಆದರೆ_ಅಂತ್ಯ /* IVL_class_type_H */
