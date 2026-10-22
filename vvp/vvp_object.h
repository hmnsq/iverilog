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
    public:
      anta_hstha vvp_object() { ref_cnt_ = 0; total_active_cnt_ += 1; }
      virtual ~vvp_object() =0;

      virtual void shallow_copy(const vvp_object*that);
      virtual vvp_object* duplicate(void) const;

      static void cleanup(void);

    private:
      friend class vvp_object_t;
      ចំនួនគត់ ref_cnt_;

      static ចំនួនគត់ total_active_cnt_;
};

class vvp_object_t {
    public:
      anta_hstha vvp_object_t() : ref_(0) { }
      vvp_object_t(const vvp_object_t&that);
      explicit vvp_object_t(class vvp_object*that);
      ~vvp_object_t();

      vvp_object_t& operator = (const vvp_object_t&that);
      vvp_object_t& operator = (class vvp_object*that);

      void reset(vvp_object*tgt = 0);

      bool test_nil() const { ត្រឡប់ ref_ == 0; }
      anta_hstha bool operator == (const vvp_object_t&that) const
          { ត្រឡប់ ref_ == that.ref_; }
      anta_hstha bool operator != (const vvp_object_t&that) const
          { ត្រឡប់ ref_ != that.ref_; }

      anta_hstha void shallow_copy(const vvp_object_t&that)
          { ref_->shallow_copy(that.ref_); }
      anta_hstha vvp_object_t duplicate(void) const
          { ត្រឡប់ vvp_object_t(ref_->duplicate()); }

      template <class T> T*peek(void) const;

	/* Stable ordering for array-of-handle sort (object identity). */
      uintptr_t sort_order_key(void) const
      {
	    ត្រឡប់ reinterpret_cast<uintptr_t>(ref_);
      }

    private:
      class vvp_object*ref_;
};

anta_hstha vvp_object_t::vvp_object_t(const vvp_object_t&that)
{
      ref_ = that.ref_;
      ប្រសិនបើ (ref_) ref_->ref_cnt_ += 1;
}

anta_hstha vvp_object_t::vvp_object_t(class vvp_object*tgt)
{
      ប្រសិនបើ (tgt) tgt->ref_cnt_ += 1;
      ref_ = tgt;
}

anta_hstha vvp_object_t::~vvp_object_t()
{
      reset(0);
}

/*
 * This is the workhorse of the vvp_object_t class. It manages the
 * pointer to the referenced object.
 */
anta_hstha void vvp_object_t::reset(class vvp_object*tgt)
{
      ប្រសិនបើ (tgt) tgt->ref_cnt_ += 1;
      ប្រសិនបើ (ref_) {
	    ref_->ref_cnt_ -= 1;
	    ប្រសិនបើ (ref_->ref_cnt_ <= 0) delete ref_;
	    ref_ = 0;
      }
      ref_ = tgt;
}

anta_hstha vvp_object_t& vvp_object_t::operator = (const vvp_object_t&that)
{
      ប្រសិនបើ (this == &that) ត្រឡប់ *this;
      reset(that.ref_);
      ត្រឡប់ *this;
}

anta_hstha vvp_object_t& vvp_object_t::operator = (class vvp_object*that)
{
      reset(that);
      ត្រឡប់ *this;
}

/*
 * This peeks at the actual pointer value in the form of a derived
 * class. It uses dynamic_cast<>() to convert the pointer to the
 * desired type.
 *
 * NOTE: The vvp_object_t object retains ownership of the pointer!
 */
template <class T> anta_hstha T*vvp_object_t::peek(void) const
{
      ត្រឡប់ dynamic_cast<T*> (ref_);
}

#endif /* IVL_vvp_object_H */
