#ifndef IVL_vvp_object_H
#define IVL_vvp_object_H
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

# include  <cstdint>
# include  <stdlib.h>

/*
 * A vvp_object is a garbage collected object such as a darray or
 * class object. The vvp_object class is a virtual base class and not
 * generally used directly. Instead, use the vvp_object_t object as a
 * smart pointer. This makes garbage collection automatic.
 */
class vvp_object {
    பொது:
      உள்வரி vvp_object() { ref_cnt_ = 0; total_active_cnt_ += 1; }
      மெய்நிகர் ~vvp_object() =0;

      மெய்நிகர் வெறுமை shallow_copy(மாறிலி vvp_object*that);
      மெய்நிகர் vvp_object* duplicate(வெறுமை) மாறிலி;

      நிலையான வெறுமை cleanup(வெறுமை);

    தனிப்பட்ட:
      friend class vvp_object_t;
      முழு ref_cnt_;

      நிலையான முழு total_active_cnt_;
};

class vvp_object_t {
    பொது:
      உள்வரி vvp_object_t() : ref_(0) { }
      vvp_object_t(மாறிலி vvp_object_t&that);
      explicit vvp_object_t(class vvp_object*that);
      ~vvp_object_t();

      vvp_object_t& operator = (மாறிலி vvp_object_t&that);
      vvp_object_t& operator = (class vvp_object*that);

      வெறுமை reset(vvp_object*tgt = 0);

      bool test_nil() மாறிலி { திரும்பு ref_ == 0; }
      உள்வரி bool operator == (மாறிலி vvp_object_t&that) மாறிலி
          { திரும்பு ref_ == that.ref_; }
      உள்வரி bool operator != (மாறிலி vvp_object_t&that) மாறிலி
          { திரும்பு ref_ != that.ref_; }

      உள்வரி வெறுமை shallow_copy(மாறிலி vvp_object_t&that)
          { ref_->shallow_copy(that.ref_); }
      உள்வரி vvp_object_t duplicate(வெறுமை) மாறிலி
          { திரும்பு vvp_object_t(ref_->duplicate()); }

      template <class T> T*peek(வெறுமை) மாறிலி;

	/* Stable ordering for array-of-handle sort (object identity). */
      uintptr_t sort_order_சாவி(வெறுமை) மாறிலி
      {
	    திரும்பு reinterpret_cast<uintptr_t>(ref_);
      }

    தனிப்பட்ட:
      class vvp_object*ref_;
};

உள்வரி vvp_object_t::vvp_object_t(மாறிலி vvp_object_t&that)
{
      ref_ = that.ref_;
      எனில் (ref_) ref_->ref_cnt_ += 1;
}

உள்வரி vvp_object_t::vvp_object_t(class vvp_object*tgt)
{
      எனில் (tgt) tgt->ref_cnt_ += 1;
      ref_ = tgt;
}

உள்வரி vvp_object_t::~vvp_object_t()
{
      reset(0);
}

/*
 * This is the workhorse of the vvp_object_t class. It manages the
 * pointer to the referenced object.
 */
உள்வரி வெறுமை vvp_object_t::reset(class vvp_object*tgt)
{
      எனில் (tgt) tgt->ref_cnt_ += 1;
      எனில் (ref_) {
	    ref_->ref_cnt_ -= 1;
	    எனில் (ref_->ref_cnt_ <= 0) delete ref_;
	    ref_ = 0;
      }
      ref_ = tgt;
}

உள்வரி vvp_object_t& vvp_object_t::operator = (மாறிலி vvp_object_t&that)
{
      எனில் (this == &that) திரும்பு *this;
      reset(that.ref_);
      திரும்பு *this;
}

உள்வரி vvp_object_t& vvp_object_t::operator = (class vvp_object*that)
{
      reset(that);
      திரும்பு *this;
}

/*
 * This peeks at the actual pointer value in the form of a derived
 * class. It uses dynamic_cast<>() to convert the pointer to the
 * desired type.
 *
 * NOTE: The vvp_object_t object retains ownership of the pointer!
 */
template <class T> உள்வரி T*vvp_object_t::peek(வெறுமை) மாறிலி
{
      திரும்பு dynamic_cast<T*> (ref_);
}

#endif /* IVL_vvp_object_H */
