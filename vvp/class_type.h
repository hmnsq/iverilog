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
class class_type : सार्वजनिक __vpiHandle {

    सार्वजनिक:
      संरचना inst_x;
      प्रकारनिर्देश inst_x*inst_t;

    सार्वजनिक:
      explicit class_type(ध्रुव std::string&nam, size_t nprop);
      ~class_type() override;

	// This is the name of the class type.
      अन्तःस्थ ध्रुव std::string&class_name(शून्य) ध्रुव { पटिनिवत्त class_name_; }
	// Number of properties in the class definition.
      अन्तःस्थ size_t property_गणनम्(शून्य) ध्रुव { पटिनिवत्त properties_.size(); }

	// Set the details about the property. This is used during
	// parse of the .vvp file to fill in the details of the
	// property for the class definition.
      शून्य set_property(size_t idx, ध्रुव std::string&name, ध्रुव std::string&type, uint64_t array_size);

	// This method is called after all the properties are
	// defined. This calculates information about the definition.
      शून्य finish_setup(शून्य);

    सार्वजनिक:
	// Constructors and destructors for making instances.
      inst_t instance_new() ध्रुव;
      शून्य instance_delete(inst_t) ध्रुव;

      शून्य set_vec4(inst_t inst, size_t pid, ध्रुव vvp_vector4_t&val) ध्रुव;
      शून्य get_vec4(inst_t inst, size_t pid, vvp_vector4_t&val) ध्रुव;
      शून्य set_real(inst_t inst, size_t pid, द्विगुण val) ध्रुव;
      द्विगुण get_real(inst_t inst, size_t pid) ध्रुव;
      शून्य set_string(inst_t inst, size_t pid, ध्रुव std::string&val) ध्रुव;
      std::string get_string(inst_t inst, size_t pid) ध्रुव;
      शून्य set_object(inst_t inst, size_t pid, ध्रुव vvp_object_t&val, size_t idx) ध्रुव;
      शून्य get_object(inst_t inst, size_t pid, vvp_object_t&val, size_t idx) ध्रुव;

      शून्य copy_property(inst_t dst, size_t idx, inst_t src) ध्रुव;

    सार्वजनिक: // VPI related methods
      पूर्णाङ्क get_type_code(शून्य) ध्रुव override;

    गुप्त:
      std::string class_name_;

      संरचना prop_t {
	    std::string name;
	    class_property_t*type;
      };
      std::vector<prop_t> properties_;
      size_t instance_size_;
};

#endif /* IVL_class_type_H */
