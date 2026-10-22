#నిర్వచితం_కాకపోతే IVL_vvp_object_H
#నిర్వచించు IVL_vvp_object_H
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
    బహిరంగ:
      అంతఃవరుస vvp_object() { ref_cnt_ = 0; మొత్తం_చురుకైన_cnt += 1; }
      మాయా ~vvp_object() =0;

      మాయా శూన్య shallow_copy(స్థిరరాశి vvp_object*that);
      మాయా vvp_object* duplicate(శూన్య) స్థిరరాశి;

      స్థిర శూన్య cleanup(శూన్య);

    అంతరంగ:
      మిత్ర class vvp_object_t;
      పూర్ణాంకం ref_cnt_;

      స్థిర పూర్ణాంకం మొత్తం_చురుకైన_cnt;
};

class vvp_object_t {
    బహిరంగ:
      అంతఃవరుస vvp_object_t() : ref_(0) { }
      vvp_object_t(స్థిరరాశి vvp_object_t&that);
      explicit vvp_object_t(class vvp_object*that);
      ~vvp_object_t();

      vvp_object_t& క్రియాకారకం = (స్థిరరాశి vvp_object_t&that);
      vvp_object_t& క్రియాకారకం = (class vvp_object*that);

      శూన్య reset(vvp_object*tgt = 0);

      bool test_nil() స్థిరరాశి { తిరిగి ref_ == 0; }
      అంతఃవరుస bool క్రియాకారకం == (స్థిరరాశి vvp_object_t&that) స్థిరరాశి
          { తిరిగి ref_ == that.ref_; }
      అంతఃవరుస bool క్రియాకారకం != (స్థిరరాశి vvp_object_t&that) స్థిరరాశి
          { తిరిగి ref_ != that.ref_; }

      అంతఃవరుస శూన్య shallow_copy(స్థిరరాశి vvp_object_t&that)
          { ref_->shallow_copy(that.ref_); }
      అంతఃవరుస vvp_object_t duplicate(శూన్య) స్థిరరాశి
          { తిరిగి vvp_object_t(ref_->duplicate()); }

      మూస <class T> T*peek(శూన్య) స్థిరరాశి;

	/* Stable ordering for array-of-handle sort (object identity). */
      uintptr_t క్రమీకరించు_క్రమం_కీలకం(శూన్య) స్థిరరాశి
      {
	    తిరిగి reinterpret_cast<uintptr_t>(ref_);
      }

    అంతరంగ:
      class vvp_object*ref_;
};

అంతఃవరుస vvp_object_t::vvp_object_t(స్థిరరాశి vvp_object_t&that)
{
      ref_ = that.ref_;
      ఒకవేళ (ref_) ref_->ref_cnt_ += 1;
}

అంతఃవరుస vvp_object_t::vvp_object_t(class vvp_object*tgt)
{
      ఒకవేళ (tgt) tgt->ref_cnt_ += 1;
      ref_ = tgt;
}

అంతఃవరుస vvp_object_t::~vvp_object_t()
{
      reset(0);
}

/*
 * This is the workhorse of the vvp_object_t class. It manages the
 * pointer to the referenced object.
 */
అంతఃవరుస శూన్య vvp_object_t::reset(class vvp_object*tgt)
{
      ఒకవేళ (tgt) tgt->ref_cnt_ += 1;
      ఒకవేళ (ref_) {
	    ref_->ref_cnt_ -= 1;
	    ఒకవేళ (ref_->ref_cnt_ <= 0) delete ref_;
	    ref_ = 0;
      }
      ref_ = tgt;
}

అంతఃవరుస vvp_object_t& vvp_object_t::క్రియాకారకం = (స్థిరరాశి vvp_object_t&that)
{
      ఒకవేళ (this == &that) తిరిగి *this;
      reset(that.ref_);
      తిరిగి *this;
}

అంతఃవరుస vvp_object_t& vvp_object_t::క్రియాకారకం = (class vvp_object*that)
{
      reset(that);
      తిరిగి *this;
}

/*
 * This peeks at the actual pointer value in the form of a derived
 * class. It uses dynamic_cast<>() to convert the pointer to the
 * desired type.
 *
 * NOTE: The vvp_object_t object retains ownership of the pointer!
 */
మూస <class T> అంతఃవరుస T*vvp_object_t::peek(శూన్య) స్థిరరాశి
{
      తిరిగి dynamic_cast<T*> (ref_);
}

#అయితే_ముగింపు /* IVL_vvp_object_H */
