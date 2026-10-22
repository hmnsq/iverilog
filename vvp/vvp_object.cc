/*
 * Copyright (c) 2012-2020 Stephen Williams (steve@icarus.com)
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

# include  "vvp_object.h"
# include  "vvp_net.h"
# include  <iostream>
# include  <typeinfo>

using নামাকাশ std;

পূর্ণাঙ্ক vvp_object::total_active_cnt_ = 0;

শূন্য vvp_object::cleanup(শূন্য)
{
}

vvp_object::~vvp_object()
{
      total_active_cnt_ -= 1;
}

শূন্য vvp_object::shallow_copy(ধ্রুব vvp_object*)
{
      cerr << "XXXX shallow_copy(vvp_object_t) not implemented for " << typeid(*এতত্).name() << endl;
      প্রতিজ্ঞা(0);
}

vvp_object* vvp_object::duplicate(শূন্য) ধ্রুব
{
      cerr << "XXXX duplicate() not implemented for " << typeid(*এতত্).name() << endl;
      প্রতিজ্ঞা(0);
      প্রত্যাগম 0;
}
