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
    જાહેર:
      અંતઃરેખ vvp_object() { ref_cnt_ = 0; total_active_cnt_ += 1; }
      આભાસી ~vvp_object() =0;

      આભાસી શૂન્ય shallow_copy(અચલ vvp_object*that);
      આભાસી vvp_object* duplicate(શૂન્ય) અચલ;

      સ્થિર શૂન્ય cleanup(શૂન્ય);

    ખાનગી:
      friend class vvp_object_t;
      પૂર્ણાંક ref_cnt_;

      સ્થિર પૂર્ણાંક total_active_cnt_;
};

class vvp_object_t {
    જાહેર:
      અંતઃરેખ vvp_object_t() : ref_(0) { }
      vvp_object_t(અચલ vvp_object_t&that);
      explicit vvp_object_t(class vvp_object*that);
      ~vvp_object_t();

      vvp_object_t& operator = (અચલ vvp_object_t&that);
      vvp_object_t& operator = (class vvp_object*that);

      શૂન્ય reset(vvp_object*tgt = 0);

      bool test_nil() અચલ { પાછા ref_ == 0; }
      અંતઃરેખ bool operator == (અચલ vvp_object_t&that) અચલ
          { પાછા ref_ == that.ref_; }
      અંતઃરેખ bool operator != (અચલ vvp_object_t&that) અચલ
          { પાછા ref_ != that.ref_; }

      અંતઃરેખ શૂન્ય shallow_copy(અચલ vvp_object_t&that)
          { ref_->shallow_copy(that.ref_); }
      અંતઃરેખ vvp_object_t duplicate(શૂન્ય) અચલ
          { પાછા vvp_object_t(ref_->duplicate()); }

      template <class T> T*peek(શૂન્ય) અચલ;

	/* Stable ordering for array-of-handle sort (object identity). */
      uintptr_t sort_order_ચાવી(શૂન્ય) અચલ
      {
	    પાછા reinterpret_cast<uintptr_t>(ref_);
      }

    ખાનગી:
      class vvp_object*ref_;
};

અંતઃરેખ vvp_object_t::vvp_object_t(અચલ vvp_object_t&that)
{
      ref_ = that.ref_;
      જો (ref_) ref_->ref_cnt_ += 1;
}

અંતઃરેખ vvp_object_t::vvp_object_t(class vvp_object*tgt)
{
      જો (tgt) tgt->ref_cnt_ += 1;
      ref_ = tgt;
}

અંતઃરેખ vvp_object_t::~vvp_object_t()
{
      reset(0);
}

/*
 * This is the workhorse of the vvp_object_t class. It manages the
 * pointer to the referenced object.
 */
અંતઃરેખ શૂન્ય vvp_object_t::reset(class vvp_object*tgt)
{
      જો (tgt) tgt->ref_cnt_ += 1;
      જો (ref_) {
	    ref_->ref_cnt_ -= 1;
	    જો (ref_->ref_cnt_ <= 0) delete ref_;
	    ref_ = 0;
      }
      ref_ = tgt;
}

અંતઃરેખ vvp_object_t& vvp_object_t::operator = (અચલ vvp_object_t&that)
{
      જો (this == &that) પાછા *this;
      reset(that.ref_);
      પાછા *this;
}

અંતઃરેખ vvp_object_t& vvp_object_t::operator = (class vvp_object*that)
{
      reset(that);
      પાછા *this;
}

/*
 * This peeks at the actual pointer value in the form of a derived
 * class. It uses dynamic_cast<>() to convert the pointer to the
 * desired type.
 *
 * NOTE: The vvp_object_t object retains ownership of the pointer!
 */
template <class T> અંતઃરેખ T*vvp_object_t::peek(શૂન્ય) અચલ
{
      પાછા dynamic_cast<T*> (ref_);
}

#endif /* IVL_vvp_object_H */
