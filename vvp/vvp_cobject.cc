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

using നാമാകാശ std;

vvp_cobject::vvp_cobject(സ്ഥിരം class_type*defn)
: defn_(defn), properties_(defn->instance_new())
{
}

vvp_cobject::~vvp_cobject()
{
      defn_->instance_delete(properties_);
      properties_ = 0;
}

ശൂന്യം vvp_cobject::set_vec4(size_t pid, സ്ഥിരം vvp_vector4_t&val)
{
      defn_->set_vec4(properties_, pid, val);
}

ശൂന്യം vvp_cobject::get_vec4(size_t pid, vvp_vector4_t&val)
{
      defn_->get_vec4(properties_, pid, val);
}

ശൂന്യം vvp_cobject::set_real(size_t pid, ഇരട്ട val)
{
      defn_->set_real(properties_, pid, val);
}

ഇരട്ട vvp_cobject::get_real(size_t pid)
{
      തിരികെ defn_->get_real(properties_, pid);
}

ശൂന്യം vvp_cobject::set_string(size_t pid, സ്ഥിരം string&val)
{
      defn_->set_string(properties_, pid, val);
}

string vvp_cobject::get_string(size_t pid)
{
      തിരികെ defn_->get_string(properties_, pid);
}

ശൂന്യം vvp_cobject::set_object(size_t pid, സ്ഥിരം vvp_object_t&val, size_t idx)
{
      defn_->set_object(properties_, pid, val, idx);
}

ശൂന്യം vvp_cobject::get_object(size_t pid, vvp_object_t&val, size_t idx)
{
      തിരികെ defn_->get_object(properties_, pid, val, idx);
}

ശൂന്യം vvp_cobject::shallow_copy(സ്ഥിരം vvp_object*obj)
{
      സ്ഥിരം vvp_cobject*that = dynamic_cast<സ്ഥിരം vvp_cobject*>(obj);
      പ്രതിജ്ഞാ(that);

      പ്രതിജ്ഞാ(defn_ == that->defn_);

      വേണ്ടി (size_t idx = 0 ; idx < defn_->property_എണ്ണം() ; idx += 1)
	    defn_->copy_property(properties_, idx, that->properties_);

}
