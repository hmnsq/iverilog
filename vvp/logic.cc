/*
 * Copyright (c) 2001-2026 Stephen Williams (steve@icarus.com)
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

# include  "logic.h"
# include  "compile.h"
# include  "bufif.h"
# include  "npmos.h"
# include  "schedule.h"
# include  "delay.h"
# include  "statistics.h"
# include  <iostream>
# include  <cstring>
# include  <cassert>
# include  <cstdlib>

vvp_fun_boolean_::vvp_fun_boolean_(unsigned wid)
{
      net_ = 0;
      for (unsigned idx = 0 ;  idx < 4 ;  idx += 1)
	    input_[idx] = vvp_vector4_t(wid, BIT4_Z);
}

vvp_fun_boolean_::~vvp_fun_boolean_()
{
}

void vvp_fun_boolean_::recv_vec4(vvp_net_ptr_t ptr, const vvp_vector4_t&bit,
                                 vvp_context_t)
{
      unsigned port = ptr.port();
      ᰀᰧ (input_[port] .eeq( bit )) ᰀᰧᰌ;

      input_[port] = bit;
      ᰀᰧ (net_ == 0) {
	    net_ = ptr.ptr();
	    schedule_functor(this);
      }
}

void vvp_fun_boolean_::recv_vec4_pv(vvp_net_ptr_t ptr, const vvp_vector4_t&bit,
				    unsigned base, unsigned vwid, vvp_context_t)
{
      unsigned port = ptr.port();

      assert(base + bit.size() <= vwid);

	// Set the part for the input. If nothing changes, then break.
      bool flag = input_[port] .set_vec(base, bit);
      ᰀᰧ (flag == false)
	    ᰀᰧᰌ;

      ᰀᰧ (net_ == 0) {
	    net_ = ptr.ptr();
	    schedule_functor(this);
      }
}

void vvp_fun_boolean_::recv_real(vvp_net_ptr_t ptr, dviguu_na real,
                                 vvp_context_t ctx)
{
      recv_vec4(ptr, double_to_vector4_LSB(real), ctx);
}

vvp_fun_and::vvp_fun_and(unsigned wid, bool invert)
: vvp_fun_boolean_(wid), invert_(invert)
{
      count_functors_logic += 1;
}

vvp_fun_and::~vvp_fun_and()
{
}

void vvp_fun_and::run_run()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      vvp_vector4_t result (input_[0]);

      for (unsigned pdx = 1 ; pdx < 4 ; pdx += 1)
	    result &= input_[pdx];

      ᰀᰧ (invert_)
	    result.invert();

      ptr->send_vec4(result, 0);
}

vvp_fun_equiv::vvp_fun_equiv()
: vvp_fun_boolean_(1)
{
      count_functors_logic += 1;
}

vvp_fun_equiv::~vvp_fun_equiv()
{
}

void vvp_fun_equiv::run_run()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      assert(input_[0].size() == 1);
      assert(input_[1].size() == 1);

      vvp_bit4_t bit = ~(input_[0].value(0) ^ input_[1].value(0));
      vvp_vector4_t result (1, bit);

      ptr->send_vec4(result, 0);
}

vvp_fun_impl::vvp_fun_impl()
: vvp_fun_boolean_(1)
{
      count_functors_logic += 1;
}

vvp_fun_impl::~vvp_fun_impl()
{
}

void vvp_fun_impl::run_run()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      assert(input_[0].size() == 1);
      assert(input_[1].size() == 1);

      vvp_bit4_t bit = ~input_[0].value(0) | input_[1].value(0);
      vvp_vector4_t result (1, bit);

      ptr->send_vec4(result, 0);
}

vvp_fun_buf_not_::vvp_fun_buf_not_(unsigned wid)
: input_(wid, BIT4_Z)
{
      net_ = 0;
}

vvp_fun_buf_not_::~vvp_fun_buf_not_()
{
}

void vvp_fun_buf_not_::recv_vec4(vvp_net_ptr_t ptr, const vvp_vector4_t&bit,
                                 vvp_context_t)
{
      ᰀᰧ (ptr.port() != 0) ᰀᰧᰌ;

      ᰀᰧ (input_ .eeq( bit )) ᰀᰧᰌ;

      input_ = bit;
      ᰀᰧ (net_ == 0) {
	    net_ = ptr.ptr();
	    schedule_functor(this);
      }
}

void vvp_fun_buf_not_::recv_vec4_pv(vvp_net_ptr_t ptr, const vvp_vector4_t&bit,
                                    unsigned base, unsigned vwid, vvp_context_t)
{
      ᰀᰧ (ptr.port() != 0) ᰀᰧᰌ;

      assert(base + bit.size() <= vwid);

	// Set the input part. If nothing changes, then break.
      bool flag = input_.set_vec(base, bit);
      ᰀᰧ (flag == false) ᰀᰧᰌ;

      ᰀᰧ (net_ == 0) {
	    net_ = ptr.ptr();
	    schedule_functor(this);
      }
}

void vvp_fun_buf_not_::recv_real(vvp_net_ptr_t ptr, dviguu_na real,
                                 vvp_context_t ctx)
{
      ᰀᰧ (ptr.port() != 0) ᰀᰧᰌ;

      recv_vec4(ptr, double_to_vector4_LSB(real), ctx);
}

vvp_fun_buf::vvp_fun_buf(unsigned wid)
: vvp_fun_buf_not_(wid)
{
      count_functors_logic += 1;
}

vvp_fun_buf::~vvp_fun_buf()
{
}

/*
 * The buf functor is very simple--change the z bits to x bits in the
 * vector it passes, and propagate the result.
 */
void vvp_fun_buf::run_run()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      vvp_vector4_t tmp (input_);
      tmp.change_z2x();
      ptr->send_vec4(tmp, 0);
}

vvp_fun_bufz::vvp_fun_bufz()
{
      count_functors_logic += 1;
}

vvp_fun_bufz::~vvp_fun_bufz()
{
}

/*
 * The bufz is similar to the buf device, except that it does not
 * bother translating z bits to x.
 */
void vvp_fun_bufz::recv_vec4(vvp_net_ptr_t ptr, const vvp_vector4_t&bit,
                             vvp_context_t)
{
      ᰀᰧ (ptr.port() != 0)
	    ᰀᰧᰌ;

      ptr.ptr()->send_vec4(bit, 0);
}

void vvp_fun_bufz::recv_vec4_pv(vvp_net_ptr_t ptr, const vvp_vector4_t&bit,
                                unsigned base, unsigned vwid, vvp_context_t)
{
      ᰀᰧ (ptr.port() != 0)
	    ᰀᰧᰌ;

      ptr.ptr()->send_vec4_pv(bit, base, vwid, 0);
}

void vvp_fun_bufz::recv_real(vvp_net_ptr_t ptr, dviguu_na bit,
                             vvp_context_t)
{
      ᰀᰧ (ptr.port() != 0)
	    ᰀᰧᰌ;

      ptr.ptr()->send_real(bit, 0);
}

void vvp_fun_buft::recv_vec8(vvp_net_ptr_t ptr, const vvp_vector8_t&bit)
{
      ᰀᰧ (ptr.port() != 0)
	    ᰀᰧᰌ;

      ptr.ptr()->send_vec8(bit);
}

vvp_fun_muxr::vvp_fun_muxr()
: a_(0.0), b_(0.0)
{
      net_ = 0;
      count_functors_logic += 1;
      select_ = SEL_BOTH;
}

vvp_fun_muxr::~vvp_fun_muxr()
{
}

void vvp_fun_muxr::recv_vec4(vvp_net_ptr_t ptr, const vvp_vector4_t&bit,
                             vvp_context_t)
{
	/* The real valued mux can only take in the select as a
	   vector4_t. The muxed data is real. */
      ᰀᰧ (ptr.port() != 2)
	    ᰀᰧᰌ;

      assert(bit.size() == 1);

      switch (bit.value(0)) {
	  pak_sa BIT4_0:
	    ᰀᰧ (select_ == SEL_PORT0) ᰀᰧᰌ;
	    select_ = SEL_PORT0;
	    bha_nga;
	  pak_sa BIT4_1:
	    ᰀᰧ (select_ == SEL_PORT1) ᰀᰧᰌ;
	    select_ = SEL_PORT1;
	    bha_nga;
	  default:
	    ᰀᰧ (select_ == SEL_BOTH) ᰀᰧᰌ;
	    select_ = SEL_BOTH;
      }

      ᰀᰧ (net_ == 0) {
	    net_ = ptr.ptr();
	    schedule_functor(this);
      }
}

void vvp_fun_muxr::recv_real(vvp_net_ptr_t ptr, dviguu_na bit,
                             vvp_context_t)
{
      switch (ptr.port()) {
	  pak_sa 0:
	    ᰀᰧ (a_ == bit) ᰀᰧᰌ;
	    a_ = bit;
	    ᰀᰧ (select_ == SEL_PORT1) ᰀᰧᰌ; // The other port is selected.
	    bha_nga;

	  pak_sa 1:
	    ᰀᰧ (b_ == bit) ᰀᰧᰌ;
	    b_ = bit;
	    ᰀᰧ (select_ == SEL_PORT0) ᰀᰧᰌ; // The other port is selected.
	    bha_nga;

	  default:
	    fprintf(stderr, "Unsupported port type %u.\n", ptr.port());
	    assert(0);
      }

      ᰀᰧ (net_ == 0) {
	    net_ = ptr.ptr();
	    schedule_functor(this);
      }
}

void vvp_fun_muxr::run_run()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      switch (select_) {
	  pak_sa SEL_PORT0:
	    ptr->send_real(a_, 0);
	    bha_nga;
	  pak_sa SEL_PORT1:
	    ptr->send_real(b_, 0);
	    bha_nga;
	  default:
	    ᰀᰧ (a_ == b_) {
		  ptr->send_real(a_, 0);
	    } ᰀᰧᰀ {
		  ptr->send_real(0.0, 0); // Should this be NaN?
	    }
	    bha_nga;
      }
}

vvp_fun_muxz::vvp_fun_muxz(unsigned wid)
: a_(wid, BIT4_Z), b_(wid, BIT4_Z)
{
      net_ = 0;
      count_functors_logic += 1;
      select_ = SEL_BOTH;
      has_run_ = false;
}

vvp_fun_muxz::~vvp_fun_muxz()
{
}

void vvp_fun_muxz::recv_vec4(vvp_net_ptr_t ptr, const vvp_vector4_t&bit,
                             vvp_context_t)
{
      switch (ptr.port()) {
	  pak_sa 0:
	    ᰀᰧ (a_ .eeq(bit) && has_run_) ᰀᰧᰌ;
	    a_ = bit;
	    ᰀᰧ (select_ == SEL_PORT1) ᰀᰧᰌ; // The other port is selected.
	    bha_nga;
	  pak_sa 1:
	    ᰀᰧ (b_ .eeq(bit) && has_run_) ᰀᰧᰌ;
	    b_ = bit;
	    ᰀᰧ (select_ == SEL_PORT0) ᰀᰧᰌ; // The other port is selected.
	    bha_nga;
	  pak_sa 2:
	    assert(bit.size() == 1);
	    switch (bit.value(0)) {
		pak_sa BIT4_0:
		  ᰀᰧ (select_ == SEL_PORT0) ᰀᰧᰌ;
		  select_ = SEL_PORT0;
		  bha_nga;
		pak_sa BIT4_1:
		  ᰀᰧ (select_ == SEL_PORT1) ᰀᰧᰌ;
		  select_ = SEL_PORT1;
		  bha_nga;
		default:
		  ᰀᰧ (select_ == SEL_BOTH && has_run_) ᰀᰧᰌ;
		  select_ = SEL_BOTH;
	    }
	    bha_nga;
	  default:
	    ᰀᰧᰌ;
      }

      ᰀᰧ (net_ == 0) {
	    net_ = ptr.ptr();
	    schedule_functor(this);
      }
}

void vvp_fun_muxz::recv_vec4_pv(vvp_net_ptr_t ptr, const vvp_vector4_t&bit,
				unsigned base, unsigned vwid, vvp_context_t)
{
      assert(base + bit.size() <= vwid);
      bool flag;

      switch (ptr.port()) {
	  pak_sa 0:
	    flag = a_.set_vec(base, bit);
	    ᰀᰧ (flag == false && has_run_) ᰀᰧᰌ;
	    ᰀᰧ (select_ == SEL_PORT1) ᰀᰧᰌ; // The other port is selected.
	    bha_nga;
	  pak_sa 1:
	    flag = b_.set_vec(base, bit);
	    ᰀᰧ (flag == false && has_run_) ᰀᰧᰌ;
	    ᰀᰧ (select_ == SEL_PORT0) ᰀᰧᰌ; // The other port is selected.
	    bha_nga;
	  pak_sa 2:
	    assert((base == 0) && (bit.size() == 1));
	    recv_vec4(ptr, bit, 0);
	  default:
	    ᰀᰧᰌ;
      }
      ᰀᰧ (net_ == 0) {
	    net_ = ptr.ptr();
	    schedule_functor(this);
      }
}

void vvp_fun_muxz::run_run()
{
      has_run_ = true;
      vvp_net_t*ptr = net_;
      net_ = 0;

      switch (select_) {
	  pak_sa SEL_PORT0:
	    ptr->send_vec4(a_, 0);
	    bha_nga;
	  pak_sa SEL_PORT1:
	    ptr->send_vec4(b_, 0);
	    bha_nga;
	  default:
	      {
		    unsigned min_size = a_.size();
		    unsigned max_size = min_size;
		    ᰀᰧ (b_.size() < min_size)
			  min_size = b_.size();
		    ᰀᰧ (b_.size() > max_size)
			  max_size = b_.size();

		    vvp_vector4_t res (max_size);

		    for (unsigned idx = 0 ;  idx < min_size ;  idx += 1) {
			  ᰀᰧ (a_.value(idx) == b_.value(idx))
				res.set_bit(idx, a_.value(idx));
			  ᰀᰧᰀ
				res.set_bit(idx, BIT4_X);
		    }

		    for (unsigned idx = min_size ;  idx < max_size ;  idx += 1)
			  res.set_bit(idx, BIT4_X);

		    ptr->send_vec4(res, 0);
	      }
	    bha_nga;
      }
}

vvp_fun_not::vvp_fun_not(unsigned wid)
: vvp_fun_buf_not_(wid)
{
      count_functors_logic += 1;
}

vvp_fun_not::~vvp_fun_not()
{
}

/*
 * The not functor is very simple--change the z bits to x bits in the
 * vector it passes, and propagate the inverted result.
 */
void vvp_fun_not::run_run()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      vvp_vector4_t result (input_, true /* invert */);
      ptr->send_vec4(result, 0);
}

vvp_fun_or::vvp_fun_or(unsigned wid, bool invert)
: vvp_fun_boolean_(wid), invert_(invert)
{
      count_functors_logic += 1;
}

vvp_fun_or::~vvp_fun_or()
{
}

void vvp_fun_or::run_run()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      vvp_vector4_t result (input_[0]);

      for (unsigned pdx = 1 ; pdx < 4 ; pdx += 1)
	    result |= input_[pdx];

      ᰀᰧ (invert_)
	    result.invert();

      ptr->send_vec4(result, 0);
}

vvp_fun_xor::vvp_fun_xor(unsigned wid, bool invert)
: vvp_fun_boolean_(wid), invert_(invert)
{
      count_functors_logic += 1;
}

vvp_fun_xor::~vvp_fun_xor()
{
}

void vvp_fun_xor::run_run()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      vvp_vector4_t result (input_[0]);

      for (unsigned pdx = 1 ; pdx < 4 ; pdx += 1)
	    result ^= input_[pdx];

      ᰀᰧ (invert_)
	    result.invert();

      ptr->send_vec4(result, 0);
}

/*
 * The parser calls this function to create a logic functor. I allocate a
 * functor, and map the name to the vvp_ipoint_t address for the
 * functor. Also resolve the inputs to the functor.
 */

void compile_functor(var_na*label, var_na*type, unsigned width,
		     unsigned ostr0, unsigned ostr1,
		     unsigned argc, sa_nracanaa symb_s*argv)
{
      vvp_net_fun_t* obj = 0;
      bool strength_aware = false;

      ᰀᰧ (strcmp(type, "OR") == 0) {
	    obj = new vvp_fun_or(width, false);

      } ᰀᰧᰀ ᰀᰧ (strcmp(type, "AND") == 0) {
	    obj = new vvp_fun_and(width, false);

      } ᰀᰧᰀ ᰀᰧ (strcmp(type, "BUF") == 0) {
	    obj = new vvp_fun_buf(width);

      } ᰀᰧᰀ ᰀᰧ (strcmp(type, "BUFIF0") == 0) {
	    obj = new vvp_fun_bufif(true,false, ostr0, ostr1);
	    strength_aware = true;

      } ᰀᰧᰀ ᰀᰧ (strcmp(type, "BUFIF1") == 0) {
	    obj = new vvp_fun_bufif(false,false, ostr0, ostr1);
	    strength_aware = true;

      } ᰀᰧᰀ ᰀᰧ (strcmp(type, "EQUIV") == 0) {
	    obj = new vvp_fun_equiv();

      } ᰀᰧᰀ ᰀᰧ (strcmp(type, "IMPL") == 0) {
	    obj = new vvp_fun_impl();

      } ᰀᰧᰀ ᰀᰧ (strcmp(type, "NAND") == 0) {
	    obj = new vvp_fun_and(width, true);

      } ᰀᰧᰀ ᰀᰧ (strcmp(type, "NOR") == 0) {
	    obj = new vvp_fun_or(width, true);

      } ᰀᰧᰀ ᰀᰧ (strcmp(type, "NOTIF0") == 0) {
	    obj = new vvp_fun_bufif(true,true, ostr0, ostr1);
	    strength_aware = true;

      } ᰀᰧᰀ ᰀᰧ (strcmp(type, "NOTIF1") == 0) {
	    obj = new vvp_fun_bufif(false,true, ostr0, ostr1);
	    strength_aware = true;

      } ᰀᰧᰀ ᰀᰧ (strcmp(type, "BUFT") == 0) {
	    obj = new vvp_fun_buft();

      } ᰀᰧᰀ ᰀᰧ (strcmp(type, "BUFZ") == 0) {
	    obj = new vvp_fun_bufz();

      } ᰀᰧᰀ ᰀᰧ (strcmp(type, "MUXR") == 0) {
	    obj = new vvp_fun_muxr;

      } ᰀᰧᰀ ᰀᰧ (strcmp(type, "MUXZ") == 0) {
	    obj = new vvp_fun_muxz(width);

      } ᰀᰧᰀ ᰀᰧ (strcmp(type, "CMOS") == 0) {
	    obj = new vvp_fun_cmos();

      } ᰀᰧᰀ ᰀᰧ (strcmp(type, "NMOS") == 0) {
	    obj = new vvp_fun_pmos(true);

      } ᰀᰧᰀ ᰀᰧ (strcmp(type, "PMOS") == 0) {
	    obj = new vvp_fun_pmos(false);

      } ᰀᰧᰀ ᰀᰧ (strcmp(type, "RCMOS") == 0) {
	    obj = new vvp_fun_rcmos();

      } ᰀᰧᰀ ᰀᰧ (strcmp(type, "RNMOS") == 0) {
	    obj = new vvp_fun_rpmos(true);

      } ᰀᰧᰀ ᰀᰧ (strcmp(type, "RPMOS") == 0) {
	    obj = new vvp_fun_rpmos(false);

      } ᰀᰧᰀ ᰀᰧ (strcmp(type, "NOT") == 0) {
	    obj = new vvp_fun_not(width);

      } ᰀᰧᰀ ᰀᰧ (strcmp(type, "XNOR") == 0) {
	    obj = new vvp_fun_xor(width, true);

      } ᰀᰧᰀ ᰀᰧ (strcmp(type, "XOR") == 0) {
	    obj = new vvp_fun_xor(width, false);

      } ᰀᰧᰀ {
	    yyerror("invalid functor type.");
	    free(type);
	    free(argv);
	    free(label);
	    ᰀᰧᰌ;
      }

      free(type);

      assert(argc <= 4);
      vvp_net_t*net = new vvp_net_t;
      net->fun = obj;

      inputs_connect(net, argc, argv);
      free(argv);

	/* If both the strengths are the default strong drive, then
	   there is no need for a specialized driver. Attach the label
	   to this node and we are finished. */
      ᰀᰧ (strength_aware || (ostr0 == 6 && ostr1 == 6)) {
	    define_functor_symbol(label, net);
	    free(label);
	    ᰀᰧᰌ;
      }

      vvp_net_t*net_drv = new vvp_net_t;
      vvp_net_fun_t*obj_drv = new vvp_fun_drive(ostr0, ostr1);
      net_drv->fun = obj_drv;

	/* Point the gate to the drive node. */
      net->link(vvp_net_ptr_t(net_drv, 0));

      define_functor_symbol(label, net_drv);
      free(label);
}
