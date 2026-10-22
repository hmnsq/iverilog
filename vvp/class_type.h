#నిర్వచితం_కాకపోతే IVL_class_type_H
#నిర్వచించు IVL_class_type_H
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
class class_type : బహిరంగ __vpiHandle {

    బహిరంగ:
      నిర్మాణం inst_x;
      రకనిర్వచనం inst_x*inst_t;

    బహిరంగ:
      explicit class_type(స్థిరరాశి std::string&nam, size_t nprop);
      ~class_type() override;

	// This is the name of the class type.
      అంతఃవరుస స్థిరరాశి std::string&class_name(శూన్య) స్థిరరాశి { తిరిగి class_name_; }
	// Number of properties in the class definition.
      అంతఃవరుస size_t property_లెక్క(శూన్య) స్థిరరాశి { తిరిగి properties_.కొలత(); }

	// Set the details about the property. This is used during
	// parse of the .vvp file to fill in the details of the
	// property for the class definition.
      శూన్య సమితి_property(size_t idx, స్థిరరాశి std::string&name, స్థిరరాశి std::string&type, uint64_t array_కొలత_2);

	// This method is called after all the properties are
	// defined. This calculates information about the definition.
      శూన్య ముగింపు_అమరిక(శూన్య);

    బహిరంగ:
	// Constructors and destructors for making instances.
      inst_t instance_new() స్థిరరాశి;
      శూన్య instance_delete(inst_t) స్థిరరాశి;

      శూన్య సమితి_vec4(inst_t inst, size_t pid, స్థిరరాశి vvp_vector4_t&val) స్థిరరాశి;
      శూన్య get_vec4(inst_t inst, size_t pid, vvp_vector4_t&val) స్థిరరాశి;
      శూన్య సమితి_real(inst_t inst, size_t pid, ద్విగుణ val) స్థిరరాశి;
      ద్విగుణ get_real(inst_t inst, size_t pid) స్థిరరాశి;
      శూన్య సమితి_string(inst_t inst, size_t pid, స్థిరరాశి std::string&val) స్థిరరాశి;
      std::string get_string(inst_t inst, size_t pid) స్థిరరాశి;
      శూన్య సమితి_object(inst_t inst, size_t pid, స్థిరరాశి vvp_object_t&val, size_t idx) స్థిరరాశి;
      శూన్య get_object(inst_t inst, size_t pid, vvp_object_t&val, size_t idx) స్థిరరాశి;

      శూన్య copy_property(inst_t dst, size_t idx, inst_t src) స్థిరరాశి;

    బహిరంగ: // VPI related methods
      పూర్ణాంకం get_type_సంకేతం(శూన్య) స్థిరరాశి override;

    అంతరంగ:
      std::string class_name_;

      నిర్మాణం prop_t {
	    std::string name;
	    class_property_t*type;
      };
      std::సదిశ<prop_t> properties_;
      size_t instance_కొలత_2;
};

#అయితే_ముగింపు /* IVL_class_type_H */
