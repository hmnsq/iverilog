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
class class_type : ସର୍ବସାଧାରଣ __vpiHandle {

    ସର୍ବସାଧାରଣ:
      ସଂରଚନା inst_x;
      ପ୍ରକାରବ୍ୟାଖ୍ୟା inst_x*inst_t;

    ସର୍ବସାଧାରଣ:
      explicit class_type(ଅଚଳ std::string&nam, size_t nprop);
      ~class_type() override;

	// This is the name of the class type.
      ଧାଡ଼ିଭିତରେ ଅଚଳ std::string&class_name(ଶୂନ୍ୟ) ଅଚଳ { ଫେରନ୍ତୁ class_name_; }
	// Number of properties in the class definition.
      ଧାଡ଼ିଭିତରେ size_t property_ଗଣନା(ଶୂନ୍ୟ) ଅଚଳ { ଫେରନ୍ତୁ properties_.size(); }

	// Set the details about the property. This is used during
	// parse of the .vvp file to fill in the details of the
	// property for the class definition.
      ଶୂନ୍ୟ set_property(size_t idx, ଅଚଳ std::string&name, ଅଚଳ std::string&type, uint64_t array_size);

	// This method is called after all the properties are
	// defined. This calculates information about the definition.
      ଶୂନ୍ୟ finish_setup(ଶୂନ୍ୟ);

    ସର୍ବସାଧାରଣ:
	// Constructors and destructors for making instances.
      inst_t instance_new() ଅଚଳ;
      ଶୂନ୍ୟ instance_delete(inst_t) ଅଚଳ;

      ଶୂନ୍ୟ set_vec4(inst_t inst, size_t pid, ଅଚଳ vvp_vector4_t&val) ଅଚଳ;
      ଶୂନ୍ୟ get_vec4(inst_t inst, size_t pid, vvp_vector4_t&val) ଅଚଳ;
      ଶୂନ୍ୟ set_real(inst_t inst, size_t pid, ଦ୍ୱିଗୁଣ val) ଅଚଳ;
      ଦ୍ୱିଗୁଣ get_real(inst_t inst, size_t pid) ଅଚଳ;
      ଶୂନ୍ୟ set_string(inst_t inst, size_t pid, ଅଚଳ std::string&val) ଅଚଳ;
      std::string get_string(inst_t inst, size_t pid) ଅଚଳ;
      ଶୂନ୍ୟ set_object(inst_t inst, size_t pid, ଅଚଳ vvp_object_t&val, size_t idx) ଅଚଳ;
      ଶୂନ୍ୟ get_object(inst_t inst, size_t pid, vvp_object_t&val, size_t idx) ଅଚଳ;

      ଶୂନ୍ୟ copy_property(inst_t dst, size_t idx, inst_t src) ଅଚଳ;

    ସର୍ବସାଧାରଣ: // VPI related methods
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_type_code(ଶୂନ୍ୟ) ଅଚଳ override;

    ବ୍ୟକ୍ତିଗତ:
      std::string class_name_;

      ସଂରଚନା prop_t {
	    std::string name;
	    class_property_t*type;
      };
      std::vector<prop_t> properties_;
      size_t instance_size_;
};

#endif /* IVL_class_type_H */
