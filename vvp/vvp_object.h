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
    ସର୍ବସାଧାରଣ:
      ଧାଡ଼ିଭିତରେ vvp_object() { ref_cnt_ = 0; total_active_cnt_ += 1; }
      ଆଭାସୀ ~vvp_object() =0;

      ଆଭାସୀ ଶୂନ୍ୟ shallow_copy(ଅଚଳ vvp_object*that);
      ଆଭାସୀ vvp_object* duplicate(ଶୂନ୍ୟ) ଅଚଳ;

      ସ୍ଥିର ଶୂନ୍ୟ cleanup(ଶୂନ୍ୟ);

    ବ୍ୟକ୍ତିଗତ:
      friend class vvp_object_t;
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା ref_cnt_;

      ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା total_active_cnt_;
};

class vvp_object_t {
    ସର୍ବସାଧାରଣ:
      ଧାଡ଼ିଭିତରେ vvp_object_t() : ref_(0) { }
      vvp_object_t(ଅଚଳ vvp_object_t&that);
      explicit vvp_object_t(class vvp_object*that);
      ~vvp_object_t();

      vvp_object_t& operator = (ଅଚଳ vvp_object_t&that);
      vvp_object_t& operator = (class vvp_object*that);

      ଶୂନ୍ୟ reset(vvp_object*tgt = 0);

      bool test_nil() ଅଚଳ { ଫେରନ୍ତୁ ref_ == 0; }
      ଧାଡ଼ିଭିତରେ bool operator == (ଅଚଳ vvp_object_t&that) ଅଚଳ
          { ଫେରନ୍ତୁ ref_ == that.ref_; }
      ଧାଡ଼ିଭିତରେ bool operator != (ଅଚଳ vvp_object_t&that) ଅଚଳ
          { ଫେରନ୍ତୁ ref_ != that.ref_; }

      ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ shallow_copy(ଅଚଳ vvp_object_t&that)
          { ref_->shallow_copy(that.ref_); }
      ଧାଡ଼ିଭିତରେ vvp_object_t duplicate(ଶୂନ୍ୟ) ଅଚଳ
          { ଫେରନ୍ତୁ vvp_object_t(ref_->duplicate()); }

      template <class T> T*peek(ଶୂନ୍ୟ) ଅଚଳ;

	/* Stable ordering for array-of-handle sort (object identity). */
      uintptr_t sort_order_ଚାବି(ଶୂନ୍ୟ) ଅଚଳ
      {
	    ଫେରନ୍ତୁ reinterpret_cast<uintptr_t>(ref_);
      }

    ବ୍ୟକ୍ତିଗତ:
      class vvp_object*ref_;
};

ଧାଡ଼ିଭିତରେ vvp_object_t::vvp_object_t(ଅଚଳ vvp_object_t&that)
{
      ref_ = that.ref_;
      ଯଦି (ref_) ref_->ref_cnt_ += 1;
}

ଧାଡ଼ିଭିତରେ vvp_object_t::vvp_object_t(class vvp_object*tgt)
{
      ଯଦି (tgt) tgt->ref_cnt_ += 1;
      ref_ = tgt;
}

ଧାଡ଼ିଭିତରେ vvp_object_t::~vvp_object_t()
{
      reset(0);
}

/*
 * This is the workhorse of the vvp_object_t class. It manages the
 * pointer to the referenced object.
 */
ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ vvp_object_t::reset(class vvp_object*tgt)
{
      ଯଦି (tgt) tgt->ref_cnt_ += 1;
      ଯଦି (ref_) {
	    ref_->ref_cnt_ -= 1;
	    ଯଦି (ref_->ref_cnt_ <= 0) delete ref_;
	    ref_ = 0;
      }
      ref_ = tgt;
}

ଧାଡ଼ିଭିତରେ vvp_object_t& vvp_object_t::operator = (ଅଚଳ vvp_object_t&that)
{
      ଯଦି (this == &that) ଫେରନ୍ତୁ *this;
      reset(that.ref_);
      ଫେରନ୍ତୁ *this;
}

ଧାଡ଼ିଭିତରେ vvp_object_t& vvp_object_t::operator = (class vvp_object*that)
{
      reset(that);
      ଫେରନ୍ତୁ *this;
}

/*
 * This peeks at the actual pointer value in the form of a derived
 * class. It uses dynamic_cast<>() to convert the pointer to the
 * desired type.
 *
 * NOTE: The vvp_object_t object retains ownership of the pointer!
 */
template <class T> ଧାଡ଼ିଭିତରେ T*vvp_object_t::peek(ଶୂନ୍ୟ) ଅଚଳ
{
      ଫେରନ୍ତୁ dynamic_cast<T*> (ref_);
}

#endif /* IVL_vvp_object_H */
