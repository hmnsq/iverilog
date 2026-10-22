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
    ಸಾರ್ವಜನಿಕ:
      ಅನ್ತಃಸ್ಥ vvp_object() { ref_cnt_ = 0; total_active_cnt_ += 1; }
      ಆಭಾಸ ~vvp_object() =0;

      ಆಭಾಸ ಶೂನ್ಯ shallow_copy(ಧ್ರುವ vvp_object*that);
      ಆಭಾಸ vvp_object* duplicate(ಶೂನ್ಯ) ಧ್ರುವ;

      ಸ್ಥಿರ ಶೂನ್ಯ cleanup(ಶೂನ್ಯ);

    ಗುಪ್ತ:
      friend class vvp_object_t;
      ಪೂರ್ಣಾಙ್ಕ ref_cnt_;

      ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ total_active_cnt_;
};

class vvp_object_t {
    ಸಾರ್ವಜನಿಕ:
      ಅನ್ತಃಸ್ಥ vvp_object_t() : ref_(0) { }
      vvp_object_t(ಧ್ರುವ vvp_object_t&that);
      explicit vvp_object_t(class vvp_object*that);
      ~vvp_object_t();

      vvp_object_t& operator = (ಧ್ರುವ vvp_object_t&that);
      vvp_object_t& operator = (class vvp_object*that);

      ಶೂನ್ಯ reset(vvp_object*tgt = 0);

      bool test_nil() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ ref_ == 0; }
      ಅನ್ತಃಸ್ಥ bool operator == (ಧ್ರುವ vvp_object_t&that) ಧ್ರುವ
          { ಪ್ರತ್ಯಾಗಮ ref_ == that.ref_; }
      ಅನ್ತಃಸ್ಥ bool operator != (ಧ್ರುವ vvp_object_t&that) ಧ್ರುವ
          { ಪ್ರತ್ಯಾಗಮ ref_ != that.ref_; }

      ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ shallow_copy(ಧ್ರುವ vvp_object_t&that)
          { ref_->shallow_copy(that.ref_); }
      ಅನ್ತಃಸ್ಥ vvp_object_t duplicate(ಶೂನ್ಯ) ಧ್ರುವ
          { ಪ್ರತ್ಯಾಗಮ vvp_object_t(ref_->duplicate()); }

      template <class T> T*peek(ಶೂನ್ಯ) ಧ್ರುವ;

	/* Stable ordering for array-of-handle sort (object identity). */
      uintptr_t sort_order_ಕೀಲಿ(ಶೂನ್ಯ) ಧ್ರುವ
      {
	    ಪ್ರತ್ಯಾಗಮ reinterpret_cast<uintptr_t>(ref_);
      }

    ಗುಪ್ತ:
      class vvp_object*ref_;
};

ಅನ್ತಃಸ್ಥ vvp_object_t::vvp_object_t(ಧ್ರುವ vvp_object_t&that)
{
      ref_ = that.ref_;
      ಯದಿ (ref_) ref_->ref_cnt_ += 1;
}

ಅನ್ತಃಸ್ಥ vvp_object_t::vvp_object_t(class vvp_object*tgt)
{
      ಯದಿ (tgt) tgt->ref_cnt_ += 1;
      ref_ = tgt;
}

ಅನ್ತಃಸ್ಥ vvp_object_t::~vvp_object_t()
{
      reset(0);
}

/*
 * This is the workhorse of the vvp_object_t class. It manages the
 * pointer to the referenced object.
 */
ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ vvp_object_t::reset(class vvp_object*tgt)
{
      ಯದಿ (tgt) tgt->ref_cnt_ += 1;
      ಯದಿ (ref_) {
	    ref_->ref_cnt_ -= 1;
	    ಯದಿ (ref_->ref_cnt_ <= 0) delete ref_;
	    ref_ = 0;
      }
      ref_ = tgt;
}

ಅನ್ತಃಸ್ಥ vvp_object_t& vvp_object_t::operator = (ಧ್ರುವ vvp_object_t&that)
{
      ಯದಿ (this == &that) ಪ್ರತ್ಯಾಗಮ *this;
      reset(that.ref_);
      ಪ್ರತ್ಯಾಗಮ *this;
}

ಅನ್ತಃಸ್ಥ vvp_object_t& vvp_object_t::operator = (class vvp_object*that)
{
      reset(that);
      ಪ್ರತ್ಯಾಗಮ *this;
}

/*
 * This peeks at the actual pointer value in the form of a derived
 * class. It uses dynamic_cast<>() to convert the pointer to the
 * desired type.
 *
 * NOTE: The vvp_object_t object retains ownership of the pointer!
 */
template <class T> ಅನ್ತಃಸ್ಥ T*vvp_object_t::peek(ಶೂನ್ಯ) ಧ್ರುವ
{
      ಪ್ರತ್ಯಾಗಮ dynamic_cast<T*> (ref_);
}

#endif /* IVL_vvp_object_H */
