#अगर_अघोषित IVL_class_type_H
#घोषणा IVL_class_type_H
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
class class_type : खुला __vpiHandle {

    खुला:
      काष्ठा inst_x;
      प्रकार inst_x*inst_t;

    खुला:
      explicit class_type(स्थिर std::string&nam, माप_प्रकार nprop);
      ~class_type() override;

	// This is the name of the class type.
      अंतरभूत स्थिर std::string&class_name(व्योम) स्थिर { वापस class_name_; }
	// Number of properties in the class definition.
      अंतरभूत माप_प्रकार property_गिनती(व्योम) स्थिर { वापस properties_.आकार(); }

	// Set the details about the property. This is used during
	// parse of the .vvp file to fill in the details of the
	// property for the class definition.
      व्योम set_property(माप_प्रकार idx, स्थिर std::string&name, स्थिर std::string&type, uint64_t array_आकार_2);

	// This method is called after all the properties are
	// defined. This calculates information about the definition.
      व्योम finish_setup(व्योम);

    खुला:
	// Constructors and destructors for making instances.
      inst_t instance_new() स्थिर;
      व्योम instance_delete(inst_t) स्थिर;

      व्योम set_vec4(inst_t inst, माप_प्रकार pid, स्थिर vvp_vector4_t&val) स्थिर;
      व्योम get_vec4(inst_t inst, माप_प्रकार pid, vvp_vector4_t&val) स्थिर;
      व्योम set_real(inst_t inst, माप_प्रकार pid, द्विगुन val) स्थिर;
      द्विगुन get_real(inst_t inst, माप_प्रकार pid) स्थिर;
      व्योम set_string(inst_t inst, माप_प्रकार pid, स्थिर std::string&val) स्थिर;
      std::string get_string(inst_t inst, माप_प्रकार pid) स्थिर;
      व्योम set_object(inst_t inst, माप_प्रकार pid, स्थिर vvp_object_t&val, माप_प्रकार idx) स्थिर;
      व्योम get_object(inst_t inst, माप_प्रकार pid, vvp_object_t&val, माप_प्रकार idx) स्थिर;

      व्योम copy_property(inst_t dst, माप_प्रकार idx, inst_t src) स्थिर;

    खुला: // VPI related methods
      पूर्णांक get_type_code(व्योम) स्थिर override;

    निजी:
      std::string class_name_;

      काष्ठा prop_t {
	    std::string name;
	    class_property_t*type;
      };
      std::vector<prop_t> properties_;
      माप_प्रकार instance_आकार_2;
};

#पूर्ण_अगर /* IVL_class_type_H */
