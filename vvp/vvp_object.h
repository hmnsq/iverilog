#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ IVL_vvp_object_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IVL_vvp_object_H
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
    ਜਨਤਕ:
      ਸਤਰਅੰਦਰ vvp_object() { ref_cnt_ = 0; ਕੁੱਲ_ਸਰਗਰਮ_cnt += 1; }
      ਆਭਾਸੀ ~vvp_object() =0;

      ਆਭਾਸੀ ਖਾਲੀ shallow_copy(ਅਚਲ vvp_object*that);
      ਆਭਾਸੀ vvp_object* duplicate(ਖਾਲੀ) ਅਚਲ;

      ਸਥਿਰ ਖਾਲੀ cleanup(ਖਾਲੀ);

    ਨਿਜੀ:
      ਮਿੱਤਰ class vvp_object_t;
      ਪੂਰਨ ref_cnt_;

      ਸਥਿਰ ਪੂਰਨ ਕੁੱਲ_ਸਰਗਰਮ_cnt;
};

class vvp_object_t {
    ਜਨਤਕ:
      ਸਤਰਅੰਦਰ vvp_object_t() : ref_(0) { }
      vvp_object_t(ਅਚਲ vvp_object_t&that);
      explicit vvp_object_t(class vvp_object*that);
      ~vvp_object_t();

      vvp_object_t& ਸੰਚਾਲਕ = (ਅਚਲ vvp_object_t&that);
      vvp_object_t& ਸੰਚਾਲਕ = (class vvp_object*that);

      ਖਾਲੀ reset(vvp_object*tgt = 0);

      bool test_nil() ਅਚਲ { ਵਾਪਸ ref_ == 0; }
      ਸਤਰਅੰਦਰ bool ਸੰਚਾਲਕ == (ਅਚਲ vvp_object_t&that) ਅਚਲ
          { ਵਾਪਸ ref_ == that.ref_; }
      ਸਤਰਅੰਦਰ bool ਸੰਚਾਲਕ != (ਅਚਲ vvp_object_t&that) ਅਚਲ
          { ਵਾਪਸ ref_ != that.ref_; }

      ਸਤਰਅੰਦਰ ਖਾਲੀ shallow_copy(ਅਚਲ vvp_object_t&that)
          { ref_->shallow_copy(that.ref_); }
      ਸਤਰਅੰਦਰ vvp_object_t duplicate(ਖਾਲੀ) ਅਚਲ
          { ਵਾਪਸ vvp_object_t(ref_->duplicate()); }

      ਸਾਂਚਾ <class T> T*peek(ਖਾਲੀ) ਅਚਲ;

	/* Stable ordering for array-of-handle sort (object identity). */
      uintptr_t ਛਾਂਟੋ_ਕ੍ਰਮ_ਕੁੰਜੀ(ਖਾਲੀ) ਅਚਲ
      {
	    ਵਾਪਸ reinterpret_cast<uintptr_t>(ref_);
      }

    ਨਿਜੀ:
      class vvp_object*ref_;
};

ਸਤਰਅੰਦਰ vvp_object_t::vvp_object_t(ਅਚਲ vvp_object_t&that)
{
      ref_ = that.ref_;
      ਜੇ (ref_) ref_->ref_cnt_ += 1;
}

ਸਤਰਅੰਦਰ vvp_object_t::vvp_object_t(class vvp_object*tgt)
{
      ਜੇ (tgt) tgt->ref_cnt_ += 1;
      ref_ = tgt;
}

ਸਤਰਅੰਦਰ vvp_object_t::~vvp_object_t()
{
      reset(0);
}

/*
 * This is the workhorse of the vvp_object_t class. It manages the
 * pointer to the referenced object.
 */
ਸਤਰਅੰਦਰ ਖਾਲੀ vvp_object_t::reset(class vvp_object*tgt)
{
      ਜੇ (tgt) tgt->ref_cnt_ += 1;
      ਜੇ (ref_) {
	    ref_->ref_cnt_ -= 1;
	    ਜੇ (ref_->ref_cnt_ <= 0) delete ref_;
	    ref_ = 0;
      }
      ref_ = tgt;
}

ਸਤਰਅੰਦਰ vvp_object_t& vvp_object_t::ਸੰਚਾਲਕ = (ਅਚਲ vvp_object_t&that)
{
      ਜੇ (this == &that) ਵਾਪਸ *this;
      reset(that.ref_);
      ਵਾਪਸ *this;
}

ਸਤਰਅੰਦਰ vvp_object_t& vvp_object_t::ਸੰਚਾਲਕ = (class vvp_object*that)
{
      reset(that);
      ਵਾਪਸ *this;
}

/*
 * This peeks at the actual pointer value in the form of a derived
 * class. It uses dynamic_cast<>() to convert the pointer to the
 * desired type.
 *
 * NOTE: The vvp_object_t object retains ownership of the pointer!
 */
ਸਾਂਚਾ <class T> ਸਤਰਅੰਦਰ T*vvp_object_t::peek(ਖਾਲੀ) ਅਚਲ
{
      ਵਾਪਸ dynamic_cast<T*> (ref_);
}

#ਜੇ_ਅੰਤ /* IVL_vvp_object_H */
