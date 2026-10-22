#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ IVL_vvp_object_H
#ವ್ಯಾಖ್ಯಾನಿಸು IVL_vvp_object_H
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
      ಸಾಲೊಳಗೆ vvp_object() { ref_cnt_ = 0; ಒಟ್ಟು_ಸಕ್ರಿಯ_cnt += 1; }
      ವಾಸ್ತವಿಕ ~vvp_object() =0;

      ವಾಸ್ತವಿಕ ಶೂನ್ಯ shallow_copy(ಸ್ಥಿರಾಂಕ vvp_object*that);
      ವಾಸ್ತವಿಕ vvp_object* duplicate(ಶೂನ್ಯ) ಸ್ಥಿರಾಂಕ;

      ಸ್ಥಿರ ಶೂನ್ಯ cleanup(ಶೂನ್ಯ);

    ಖಾಸಗಿ:
      ಮಿತ್ರ class vvp_object_t;
      ಪೂರ್ಣಾಂಕ ref_cnt_;

      ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಒಟ್ಟು_ಸಕ್ರಿಯ_cnt;
};

class vvp_object_t {
    ಸಾರ್ವಜನಿಕ:
      ಸಾಲೊಳಗೆ vvp_object_t() : ref_(0) { }
      vvp_object_t(ಸ್ಥಿರಾಂಕ vvp_object_t&that);
      explicit vvp_object_t(class vvp_object*that);
      ~vvp_object_t();

      vvp_object_t& ಕ್ರಿಯಾಕಾರಕ = (ಸ್ಥಿರಾಂಕ vvp_object_t&that);
      vvp_object_t& ಕ್ರಿಯಾಕಾರಕ = (class vvp_object*that);

      ಶೂನ್ಯ reset(vvp_object*tgt = 0);

      bool test_nil() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ref_ == 0; }
      ಸಾಲೊಳಗೆ bool ಕ್ರಿಯಾಕಾರಕ == (ಸ್ಥಿರಾಂಕ vvp_object_t&that) ಸ್ಥಿರಾಂಕ
          { ಹಿಂತಿರುಗಿ ref_ == that.ref_; }
      ಸಾಲೊಳಗೆ bool ಕ್ರಿಯಾಕಾರಕ != (ಸ್ಥಿರಾಂಕ vvp_object_t&that) ಸ್ಥಿರಾಂಕ
          { ಹಿಂತಿರುಗಿ ref_ != that.ref_; }

      ಸಾಲೊಳಗೆ ಶೂನ್ಯ shallow_copy(ಸ್ಥಿರಾಂಕ vvp_object_t&that)
          { ref_->shallow_copy(that.ref_); }
      ಸಾಲೊಳಗೆ vvp_object_t duplicate(ಶೂನ್ಯ) ಸ್ಥಿರಾಂಕ
          { ಹಿಂತಿರುಗಿ vvp_object_t(ref_->duplicate()); }

      ಮಾದರಿ <class T> T*peek(ಶೂನ್ಯ) ಸ್ಥಿರಾಂಕ;

	/* Stable ordering for array-of-handle sort (object identity). */
      uintptr_t ವಿಂಗಡಿಸು_ಕ್ರಮ_ಕೀಲಿ(ಶೂನ್ಯ) ಸ್ಥಿರಾಂಕ
      {
	    ಹಿಂತಿರುಗಿ reinterpret_cast<uintptr_t>(ref_);
      }

    ಖಾಸಗಿ:
      class vvp_object*ref_;
};

ಸಾಲೊಳಗೆ vvp_object_t::vvp_object_t(ಸ್ಥಿರಾಂಕ vvp_object_t&that)
{
      ref_ = that.ref_;
      ಆದರೆ (ref_) ref_->ref_cnt_ += 1;
}

ಸಾಲೊಳಗೆ vvp_object_t::vvp_object_t(class vvp_object*tgt)
{
      ಆದರೆ (tgt) tgt->ref_cnt_ += 1;
      ref_ = tgt;
}

ಸಾಲೊಳಗೆ vvp_object_t::~vvp_object_t()
{
      reset(0);
}

/*
 * This is the workhorse of the vvp_object_t class. It manages the
 * pointer to the referenced object.
 */
ಸಾಲೊಳಗೆ ಶೂನ್ಯ vvp_object_t::reset(class vvp_object*tgt)
{
      ಆದರೆ (tgt) tgt->ref_cnt_ += 1;
      ಆದರೆ (ref_) {
	    ref_->ref_cnt_ -= 1;
	    ಆದರೆ (ref_->ref_cnt_ <= 0) delete ref_;
	    ref_ = 0;
      }
      ref_ = tgt;
}

ಸಾಲೊಳಗೆ vvp_object_t& vvp_object_t::ಕ್ರಿಯಾಕಾರಕ = (ಸ್ಥಿರಾಂಕ vvp_object_t&that)
{
      ಆದರೆ (this == &that) ಹಿಂತಿರುಗಿ *this;
      reset(that.ref_);
      ಹಿಂತಿರುಗಿ *this;
}

ಸಾಲೊಳಗೆ vvp_object_t& vvp_object_t::ಕ್ರಿಯಾಕಾರಕ = (class vvp_object*that)
{
      reset(that);
      ಹಿಂತಿರುಗಿ *this;
}

/*
 * This peeks at the actual pointer value in the form of a derived
 * class. It uses dynamic_cast<>() to convert the pointer to the
 * desired type.
 *
 * NOTE: The vvp_object_t object retains ownership of the pointer!
 */
ಮಾದರಿ <class T> ಸಾಲೊಳಗೆ T*vvp_object_t::peek(ಶೂನ್ಯ) ಸ್ಥಿರಾಂಕ
{
      ಹಿಂತಿರುಗಿ dynamic_cast<T*> (ref_);
}

#ಆದರೆ_ಅಂತ್ಯ /* IVL_vvp_object_H */
