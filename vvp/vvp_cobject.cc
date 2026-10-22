/*
 * Copyright (c) 2012-2013 Stephen Williams (steve@icarus.com)
 *
 *    This source code is free software; you can redistribute it
 *    and/or modify it in source code form under the terms of the GNU
 *    General Public License as published by the Free Software
 *    Foundation; either version 2 of the License, or (at your option)
 *    any later version.
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

# include  "vvp_cobject.h"
# include  "class_type.h"
# include  <iostream>
# include  <cassert>

using namespace std;

vvp_cobject::vvp_cobject(स्थिर class_type*defn)
: defn_(defn), properties_(defn->instance_new())
{
}

vvp_cobject::~vvp_cobject()
{
      defn_->instance_delete(properties_);
      properties_ = 0;
}

व्योम vvp_cobject::set_vec4(माप_प्रकार pid, स्थिर vvp_vector4_t&val)
{
      defn_->set_vec4(properties_, pid, val);
}

व्योम vvp_cobject::get_vec4(माप_प्रकार pid, vvp_vector4_t&val)
{
      defn_->get_vec4(properties_, pid, val);
}

व्योम vvp_cobject::set_real(माप_प्रकार pid, द्विगुन val)
{
      defn_->set_real(properties_, pid, val);
}

द्विगुन vvp_cobject::get_real(माप_प्रकार pid)
{
      वापस defn_->get_real(properties_, pid);
}

व्योम vvp_cobject::set_string(माप_प्रकार pid, स्थिर string&val)
{
      defn_->set_string(properties_, pid, val);
}

string vvp_cobject::get_string(माप_प्रकार pid)
{
      वापस defn_->get_string(properties_, pid);
}

व्योम vvp_cobject::set_object(माप_प्रकार pid, स्थिर vvp_object_t&val, माप_प्रकार idx)
{
      defn_->set_object(properties_, pid, val, idx);
}

व्योम vvp_cobject::get_object(माप_प्रकार pid, vvp_object_t&val, माप_प्रकार idx)
{
      वापस defn_->get_object(properties_, pid, val, idx);
}

व्योम vvp_cobject::shallow_copy(स्थिर vvp_object*obj)
{
      स्थिर vvp_cobject*that = dynamic_cast<स्थिर vvp_cobject*>(obj);
      निश्चित(that);

      निश्चित(defn_ == that->defn_);

      क्रम (माप_प्रकार idx = 0 ; idx < defn_->property_गिनती() ; idx += 1)
	    defn_->copy_property(properties_, idx, that->properties_);

}
