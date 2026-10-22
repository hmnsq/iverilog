#اگر_غیر_تعریف_شدہ IVL_vvp_object_H
#تعریف_کرو IVL_vvp_object_H
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
    عوامی:
      اندرونی vvp_object() { ref_cnt_ = 0; کل_فعال_cnt += 1; }
      مجازی ~vvp_object() =0;

      مجازی خالی shallow_copy(ثابت vvp_object*that);
      مجازی vvp_object* duplicate(خالی) ثابت;

      ساکن خالی cleanup(خالی);

    نجی:
      دوست class vvp_object_t;
      صحیح ref_cnt_;

      ساکن صحیح کل_فعال_cnt;
};

class vvp_object_t {
    عوامی:
      اندرونی vvp_object_t() : ref_(0) { }
      vvp_object_t(ثابت vvp_object_t&that);
      explicit vvp_object_t(class vvp_object*that);
      ~vvp_object_t();

      vvp_object_t& عامل = (ثابت vvp_object_t&that);
      vvp_object_t& عامل = (class vvp_object*that);

      خالی reset(vvp_object*tgt = 0);

      bool test_nil() ثابت { واپس ref_ == 0; }
      اندرونی bool عامل == (ثابت vvp_object_t&that) ثابت
          { واپس ref_ == that.ref_; }
      اندرونی bool عامل != (ثابت vvp_object_t&that) ثابت
          { واپس ref_ != that.ref_; }

      اندرونی خالی shallow_copy(ثابت vvp_object_t&that)
          { ref_->shallow_copy(that.ref_); }
      اندرونی vvp_object_t duplicate(خالی) ثابت
          { واپس vvp_object_t(ref_->duplicate()); }

      سانچہ <class T> T*peek(خالی) ثابت;

	/* Stable ordering for array-of-handle sort (object identity). */
      uintptr_t ترتیب_دو_ترتیب_کلید(خالی) ثابت
      {
	    واپس reinterpret_cast<uintptr_t>(ref_);
      }

    نجی:
      class vvp_object*ref_;
};

اندرونی vvp_object_t::vvp_object_t(ثابت vvp_object_t&that)
{
      ref_ = that.ref_;
      اگر (ref_) ref_->ref_cnt_ += 1;
}

اندرونی vvp_object_t::vvp_object_t(class vvp_object*tgt)
{
      اگر (tgt) tgt->ref_cnt_ += 1;
      ref_ = tgt;
}

اندرونی vvp_object_t::~vvp_object_t()
{
      reset(0);
}

/*
 * This is the workhorse of the vvp_object_t class. It manages the
 * pointer to the referenced object.
 */
اندرونی خالی vvp_object_t::reset(class vvp_object*tgt)
{
      اگر (tgt) tgt->ref_cnt_ += 1;
      اگر (ref_) {
	    ref_->ref_cnt_ -= 1;
	    اگر (ref_->ref_cnt_ <= 0) delete ref_;
	    ref_ = 0;
      }
      ref_ = tgt;
}

اندرونی vvp_object_t& vvp_object_t::عامل = (ثابت vvp_object_t&that)
{
      اگر (this == &that) واپس *this;
      reset(that.ref_);
      واپس *this;
}

اندرونی vvp_object_t& vvp_object_t::عامل = (class vvp_object*that)
{
      reset(that);
      واپس *this;
}

/*
 * This peeks at the actual pointer value in the form of a derived
 * class. It uses dynamic_cast<>() to convert the pointer to the
 * desired type.
 *
 * NOTE: The vvp_object_t object retains ownership of the pointer!
 */
سانچہ <class T> اندرونی T*vvp_object_t::peek(خالی) ثابت
{
      واپس dynamic_cast<T*> (ref_);
}

#اگر_ختم /* IVL_vvp_object_H */
