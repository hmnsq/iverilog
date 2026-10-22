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
    सार्वजनिक:
      अन्तःस्थ vvp_object() { ref_cnt_ = 0; total_active_cnt_ += 1; }
      आभास ~vvp_object() =0;

      आभास शून्य shallow_copy(ध्रुव vvp_object*that);
      आभास vvp_object* duplicate(शून्य) ध्रुव;

      स्थिर शून्य cleanup(शून्य);

    गुप्त:
      friend class vvp_object_t;
      पूर्णाङ्क ref_cnt_;

      स्थिर पूर्णाङ्क total_active_cnt_;
};

class vvp_object_t {
    सार्वजनिक:
      अन्तःस्थ vvp_object_t() : ref_(0) { }
      vvp_object_t(ध्रुव vvp_object_t&that);
      explicit vvp_object_t(class vvp_object*that);
      ~vvp_object_t();

      vvp_object_t& operator = (ध्रुव vvp_object_t&that);
      vvp_object_t& operator = (class vvp_object*that);

      शून्य reset(vvp_object*tgt = 0);

      bool test_nil() ध्रुव { पटिनिवत्त ref_ == 0; }
      अन्तःस्थ bool operator == (ध्रुव vvp_object_t&that) ध्रुव
          { पटिनिवत्त ref_ == that.ref_; }
      अन्तःस्थ bool operator != (ध्रुव vvp_object_t&that) ध्रुव
          { पटिनिवत्त ref_ != that.ref_; }

      अन्तःस्थ शून्य shallow_copy(ध्रुव vvp_object_t&that)
          { ref_->shallow_copy(that.ref_); }
      अन्तःस्थ vvp_object_t duplicate(शून्य) ध्रुव
          { पटिनिवत्त vvp_object_t(ref_->duplicate()); }

      template <class T> T*peek(शून्य) ध्रुव;

	/* Stable ordering for array-of-handle sort (object identity). */
      uintptr_t sort_order_कुञ्चिका(शून्य) ध्रुव
      {
	    पटिनिवत्त reinterpret_cast<uintptr_t>(ref_);
      }

    गुप्त:
      class vvp_object*ref_;
};

अन्तःस्थ vvp_object_t::vvp_object_t(ध्रुव vvp_object_t&that)
{
      ref_ = that.ref_;
      सचे (ref_) ref_->ref_cnt_ += 1;
}

अन्तःस्थ vvp_object_t::vvp_object_t(class vvp_object*tgt)
{
      सचे (tgt) tgt->ref_cnt_ += 1;
      ref_ = tgt;
}

अन्तःस्थ vvp_object_t::~vvp_object_t()
{
      reset(0);
}

/*
 * This is the workhorse of the vvp_object_t class. It manages the
 * pointer to the referenced object.
 */
अन्तःस्थ शून्य vvp_object_t::reset(class vvp_object*tgt)
{
      सचे (tgt) tgt->ref_cnt_ += 1;
      सचे (ref_) {
	    ref_->ref_cnt_ -= 1;
	    सचे (ref_->ref_cnt_ <= 0) delete ref_;
	    ref_ = 0;
      }
      ref_ = tgt;
}

अन्तःस्थ vvp_object_t& vvp_object_t::operator = (ध्रुव vvp_object_t&that)
{
      सचे (this == &that) पटिनिवत्त *this;
      reset(that.ref_);
      पटिनिवत्त *this;
}

अन्तःस्थ vvp_object_t& vvp_object_t::operator = (class vvp_object*that)
{
      reset(that);
      पटिनिवत्त *this;
}

/*
 * This peeks at the actual pointer value in the form of a derived
 * class. It uses dynamic_cast<>() to convert the pointer to the
 * desired type.
 *
 * NOTE: The vvp_object_t object retains ownership of the pointer!
 */
template <class T> अन्तःस्थ T*vvp_object_t::peek(शून्य) ध्रुव
{
      पटिनिवत्त dynamic_cast<T*> (ref_);
}

#endif /* IVL_vvp_object_H */
