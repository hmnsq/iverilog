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
      explicit class_type(अचर std::string&nam, माप_प्रकार nprop);
      ~class_type() override;

	// This is the name of the class type.
      अन्तःस्थ अचर std::string&class_name(शून्य) अचर { फर्काऊ class_name_; }
	// Number of properties in the class definition.
      अन्तःस्थ माप_प्रकार property_गन्ती(शून्य) अचर { फर्काऊ properties_.size(); }

	// Set the details about the property. This is used during
	// parse of the .vvp file to fill in the details of the
	// property for the class definition.
      शून्य set_property(माप_प्रकार idx, अचर std::string&name, अचर std::string&type, uint64_t array_size);

	// This method is called after all the properties are
	// defined. This calculates information about the definition.
      शून्य finish_setup(शून्य);

    सार्वजनिक:
	// Constructors and destructors for making instances.
      inst_t instance_new() अचर;
      शून्य instance_delete(inst_t) अचर;

      शून्य set_vec4(inst_t inst, माप_प्रकार pid, अचर vvp_vector4_t&val) अचर;
      शून्य get_vec4(inst_t inst, माप_प्रकार pid, vvp_vector4_t&val) अचर;
      शून्य set_real(inst_t inst, माप_प्रकार pid, द्विगुण val) अचर;
      द्विगुण get_real(inst_t inst, माप_प्रकार pid) अचर;
      शून्य set_string(inst_t inst, माप_प्रकार pid, अचर std::string&val) अचर;
      std::string get_string(inst_t inst, माप_प्रकार pid) अचर;
      शून्य set_object(inst_t inst, माप_प्रकार pid, अचर vvp_object_t&val, माप_प्रकार idx) अचर;
      शून्य get_object(inst_t inst, माप_प्रकार pid, vvp_object_t&val, माप_प्रकार idx) अचर;

      शून्य copy_property(inst_t dst, माप_प्रकार idx, inst_t src) अचर;

    सार्वजनिक: // VPI related methods
      पूर्णाङ्क get_type_code(शून्य) अचर override;

    गुप्त:
      std::string class_name_;

      संरचना prop_t {
	    std::string name;
	    class_property_t*type;
      };
      std::vector<prop_t> properties_;
      माप_प्रकार instance_size_;
};

#endif /* IVL_class_type_H */
