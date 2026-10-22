#अगर_अघोषित IVL_vvp_object_H
#घोषणा IVL_vvp_object_H
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
    खुला:
      अंतरभूत vvp_object() { ref_cnt_ = 0; योग_active_cnt += 1; }
      भव ~vvp_object() =0;

      भव व्योम shallow_copy(स्थिर vvp_object*that);
      भव vvp_object* duplicate(व्योम) स्थिर;

      अटल व्योम cleanup(व्योम);

    निजी:
      मित्र class vvp_object_t;
      पूर्णांक ref_cnt_;

      अटल पूर्णांक योग_active_cnt;
};

class vvp_object_t {
    खुला:
      अंतरभूत vvp_object_t() : ref_(0) { }
      vvp_object_t(स्थिर vvp_object_t&that);
      explicit vvp_object_t(class vvp_object*that);
      ~vvp_object_t();

      vvp_object_t& चालक = (स्थिर vvp_object_t&that);
      vvp_object_t& चालक = (class vvp_object*that);

      व्योम reset(vvp_object*tgt = 0);

      bool test_nil() स्थिर { वापस ref_ == 0; }
      अंतरभूत bool चालक == (स्थिर vvp_object_t&that) स्थिर
          { वापस ref_ == that.ref_; }
      अंतरभूत bool चालक != (स्थिर vvp_object_t&that) स्थिर
          { वापस ref_ != that.ref_; }

      अंतरभूत व्योम shallow_copy(स्थिर vvp_object_t&that)
          { ref_->shallow_copy(that.ref_); }
      अंतरभूत vvp_object_t duplicate(व्योम) स्थिर
          { वापस vvp_object_t(ref_->duplicate()); }

      ढाँचा <class T> T*peek(व्योम) स्थिर;

	/* Stable ordering for array-of-handle sort (object identity). */
      uintptr_t sort_order_कुंजी(व्योम) स्थिर
      {
	    वापस reinterpret_cast<uintptr_t>(ref_);
      }

    निजी:
      class vvp_object*ref_;
};

अंतरभूत vvp_object_t::vvp_object_t(स्थिर vvp_object_t&that)
{
      ref_ = that.ref_;
      अगर (ref_) ref_->ref_cnt_ += 1;
}

अंतरभूत vvp_object_t::vvp_object_t(class vvp_object*tgt)
{
      अगर (tgt) tgt->ref_cnt_ += 1;
      ref_ = tgt;
}

अंतरभूत vvp_object_t::~vvp_object_t()
{
      reset(0);
}

/*
 * This is the workhorse of the vvp_object_t class. It manages the
 * pointer to the referenced object.
 */
अंतरभूत व्योम vvp_object_t::reset(class vvp_object*tgt)
{
      अगर (tgt) tgt->ref_cnt_ += 1;
      अगर (ref_) {
	    ref_->ref_cnt_ -= 1;
	    अगर (ref_->ref_cnt_ <= 0) delete ref_;
	    ref_ = 0;
      }
      ref_ = tgt;
}

अंतरभूत vvp_object_t& vvp_object_t::चालक = (स्थिर vvp_object_t&that)
{
      अगर (this == &that) वापस *this;
      reset(that.ref_);
      वापस *this;
}

अंतरभूत vvp_object_t& vvp_object_t::चालक = (class vvp_object*that)
{
      reset(that);
      वापस *this;
}

/*
 * This peeks at the actual pointer value in the form of a derived
 * class. It uses dynamic_cast<>() to convert the pointer to the
 * desired type.
 *
 * NOTE: The vvp_object_t object retains ownership of the pointer!
 */
ढाँचा <class T> अंतरभूत T*vvp_object_t::peek(व्योम) स्थिर
{
      वापस dynamic_cast<T*> (ref_);
}

#पूर्ण_अगर /* IVL_vvp_object_H */
