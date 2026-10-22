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

ఉపయోగించి నామాకాశ std;

vvp_cobject::vvp_cobject(స్థిరరాశి class_type*defn)
: defn_(defn), properties_(defn->instance_new())
{
}

vvp_cobject::~vvp_cobject()
{
      defn_->instance_delete(properties_);
      properties_ = 0;
}

శూన్య vvp_cobject::సమితి_vec4(size_t pid, స్థిరరాశి vvp_vector4_t&val)
{
      defn_->సమితి_vec4(properties_, pid, val);
}

శూన్య vvp_cobject::get_vec4(size_t pid, vvp_vector4_t&val)
{
      defn_->get_vec4(properties_, pid, val);
}

శూన్య vvp_cobject::సమితి_real(size_t pid, ద్విగుణ val)
{
      defn_->సమితి_real(properties_, pid, val);
}

ద్విగుణ vvp_cobject::get_real(size_t pid)
{
      తిరిగి defn_->get_real(properties_, pid);
}

శూన్య vvp_cobject::సమితి_string(size_t pid, స్థిరరాశి string&val)
{
      defn_->సమితి_string(properties_, pid, val);
}

string vvp_cobject::get_string(size_t pid)
{
      తిరిగి defn_->get_string(properties_, pid);
}

శూన్య vvp_cobject::సమితి_object(size_t pid, స్థిరరాశి vvp_object_t&val, size_t idx)
{
      defn_->సమితి_object(properties_, pid, val, idx);
}

శూన్య vvp_cobject::get_object(size_t pid, vvp_object_t&val, size_t idx)
{
      తిరిగి defn_->get_object(properties_, pid, val, idx);
}

శూన్య vvp_cobject::shallow_copy(స్థిరరాశి vvp_object*obj)
{
      స్థిరరాశి vvp_cobject*that = dynamic_cast<స్థిరరాశి vvp_cobject*>(obj);
      ప్రతిజ్ఞా(that);

      ప్రతిజ్ఞా(defn_ == that->defn_);

      కొరకు (size_t idx = 0 ; idx < defn_->property_లెక్క() ; idx += 1)
	    defn_->copy_property(properties_, idx, that->properties_);

}
