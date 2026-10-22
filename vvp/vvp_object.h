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
    عام:
      مضمن vvp_object() { ref_cnt_ = 0; total_active_cnt_ += 1; }
      virtual ~vvp_object() =0;

      virtual فراغ shallow_copy(ثابت vvp_object*that);
      virtual vvp_object* duplicate(فراغ) ثابت;

      ساكن فراغ cleanup(فراغ);

    خاص:
      friend class vvp_object_t;
      صحيح ref_cnt_;

      ساكن صحيح total_active_cnt_;
};

class vvp_object_t {
    عام:
      مضمن vvp_object_t() : ref_(0) { }
      vvp_object_t(ثابت vvp_object_t&that);
      explicit vvp_object_t(class vvp_object*that);
      ~vvp_object_t();

      vvp_object_t& operator = (ثابت vvp_object_t&that);
      vvp_object_t& operator = (class vvp_object*that);

      فراغ reset(vvp_object*tgt = 0);

      bool test_nil() ثابت { ܦܢܐ ref_ == 0; }
      مضمن bool operator == (ثابت vvp_object_t&that) ثابت
          { ܦܢܐ ref_ == that.ref_; }
      مضمن bool operator != (ثابت vvp_object_t&that) ثابت
          { ܦܢܐ ref_ != that.ref_; }

      مضمن فراغ shallow_copy(ثابت vvp_object_t&that)
          { ref_->shallow_copy(that.ref_); }
      مضمن vvp_object_t duplicate(فراغ) ثابت
          { ܦܢܐ vvp_object_t(ref_->duplicate()); }

      template <class T> T*peek(فراغ) ثابت;

	/* Stable ordering for array-of-handle sort (object identity). */
      uintptr_t sort_order_key(فراغ) ثابت
      {
	    ܦܢܐ reinterpret_cast<uintptr_t>(ref_);
      }

    خاص:
      class vvp_object*ref_;
};

مضمن vvp_object_t::vvp_object_t(ثابت vvp_object_t&that)
{
      ref_ = that.ref_;
      ܐܢ (ref_) ref_->ref_cnt_ += 1;
}

مضمن vvp_object_t::vvp_object_t(class vvp_object*tgt)
{
      ܐܢ (tgt) tgt->ref_cnt_ += 1;
      ref_ = tgt;
}

مضمن vvp_object_t::~vvp_object_t()
{
      reset(0);
}

/*
 * This is the workhorse of the vvp_object_t class. It manages the
 * pointer to the referenced object.
 */
مضمن فراغ vvp_object_t::reset(class vvp_object*tgt)
{
      ܐܢ (tgt) tgt->ref_cnt_ += 1;
      ܐܢ (ref_) {
	    ref_->ref_cnt_ -= 1;
	    ܐܢ (ref_->ref_cnt_ <= 0) delete ref_;
	    ref_ = 0;
      }
      ref_ = tgt;
}

مضمن vvp_object_t& vvp_object_t::operator = (ثابت vvp_object_t&that)
{
      ܐܢ (this == &that) ܦܢܐ *this;
      reset(that.ref_);
      ܦܢܐ *this;
}

مضمن vvp_object_t& vvp_object_t::operator = (class vvp_object*that)
{
      reset(that);
      ܦܢܐ *this;
}

/*
 * This peeks at the actual pointer value in the form of a derived
 * class. It uses dynamic_cast<>() to convert the pointer to the
 * desired type.
 *
 * NOTE: The vvp_object_t object retains ownership of the pointer!
 */
template <class T> مضمن T*vvp_object_t::peek(فراغ) ثابت
{
      ܦܢܐ dynamic_cast<T*> (ref_);
}

#endif /* IVL_vvp_object_H */
