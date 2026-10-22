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
      માટે (unsigned idx = 0 ;  idx < 4 ;  idx += 1)
	    આગત_2[idx] = vvp_vector4_t(wid, BIT4_Z);
}

vvp_fun_boolean_::~vvp_fun_boolean_()
{
}

શૂન્ય vvp_fun_boolean_::recv_vec4(vvp_net_ptr_t ptr, અચલ vvp_vector4_t&bit,
                                 vvp_context_t)
{
      unsigned port = ptr.port();
      જો (આગત_2[port] .eeq( bit )) પાછા;

      આગત_2[port] = bit;
      જો (net_ == 0) {
	    net_ = ptr.ptr();
	    schedule_functor(આ);
      }
}

શૂન્ય vvp_fun_boolean_::recv_vec4_pv(vvp_net_ptr_t ptr, અચલ vvp_vector4_t&bit,
				    unsigned base, unsigned vwid, vvp_context_t)
{
      unsigned port = ptr.port();

      પ્રતિજ્ઞા(base + bit.size() <= vwid);

	// Set the part for the input. If nothing changes, then break.
      તાર્કિક flag = આગત_2[port] .set_vec(base, bit);
      જો (flag == ખોટું)
	    પાછા;

      જો (net_ == 0) {
	    net_ = ptr.ptr();
	    schedule_functor(આ);
      }
}

શૂન્ય vvp_fun_boolean_::recv_real(vvp_net_ptr_t ptr, દ્વિગુણ real,
                                 vvp_context_t ctx)
{
      recv_vec4(ptr, double_to_vector4_LSB(real), ctx);
}

vvp_fun_and::vvp_fun_and(unsigned wid, તાર્કિક invert)
: vvp_fun_boolean_(wid), invert_(invert)
{
      ગણતરી_functors_logic += 1;
}

vvp_fun_and::~vvp_fun_and()
{
}

શૂન્ય vvp_fun_and::run_run()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      vvp_vector4_t result (આગત_2[0]);

      માટે (unsigned pdx = 1 ; pdx < 4 ; pdx += 1)
	    result &= આગત_2[pdx];

      જો (invert_)
	    result.invert();

      ptr->send_vec4(result, 0);
}

vvp_fun_equiv::vvp_fun_equiv()
: vvp_fun_boolean_(1)
{
      ગણતરી_functors_logic += 1;
}

vvp_fun_equiv::~vvp_fun_equiv()
{
}

શૂન્ય vvp_fun_equiv::run_run()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      પ્રતિજ્ઞા(આગત_2[0].size() == 1);
      પ્રતિજ્ઞા(આગત_2[1].size() == 1);

      vvp_bit4_t bit = ~(આગત_2[0].value(0) ^ આગત_2[1].value(0));
      vvp_vector4_t result (1, bit);

      ptr->send_vec4(result, 0);
}

vvp_fun_impl::vvp_fun_impl()
: vvp_fun_boolean_(1)
{
      ગણતરી_functors_logic += 1;
}

vvp_fun_impl::~vvp_fun_impl()
{
}

શૂન્ય vvp_fun_impl::run_run()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      પ્રતિજ્ઞા(આગત_2[0].size() == 1);
      પ્રતિજ્ઞા(આગત_2[1].size() == 1);

      vvp_bit4_t bit = ~આગત_2[0].value(0) | આગત_2[1].value(0);
      vvp_vector4_t result (1, bit);

      ptr->send_vec4(result, 0);
}

vvp_fun_buf_not_::vvp_fun_buf_not_(unsigned wid)
: આગત_2(wid, BIT4_Z)
{
      net_ = 0;
}

vvp_fun_buf_not_::~vvp_fun_buf_not_()
{
}

શૂન્ય vvp_fun_buf_not_::recv_vec4(vvp_net_ptr_t ptr, અચલ vvp_vector4_t&bit,
                                 vvp_context_t)
{
      જો (ptr.port() != 0) પાછા;

      જો (આગત_2 .eeq( bit )) પાછા;

      આગત_2 = bit;
      જો (net_ == 0) {
	    net_ = ptr.ptr();
	    schedule_functor(આ);
      }
}

શૂન્ય vvp_fun_buf_not_::recv_vec4_pv(vvp_net_ptr_t ptr, અચલ vvp_vector4_t&bit,
                                    unsigned base, unsigned vwid, vvp_context_t)
{
      જો (ptr.port() != 0) પાછા;

      પ્રતિજ્ઞા(base + bit.size() <= vwid);

	// Set the input part. If nothing changes, then break.
      તાર્કિક flag = આગત_2.set_vec(base, bit);
      જો (flag == ખોટું) પાછા;

      જો (net_ == 0) {
	    net_ = ptr.ptr();
	    schedule_functor(આ);
      }
}

શૂન્ય vvp_fun_buf_not_::recv_real(vvp_net_ptr_t ptr, દ્વિગુણ real,
                                 vvp_context_t ctx)
{
      જો (ptr.port() != 0) પાછા;

      recv_vec4(ptr, double_to_vector4_LSB(real), ctx);
}

vvp_fun_buf::vvp_fun_buf(unsigned wid)
: vvp_fun_buf_not_(wid)
{
      ગણતરી_functors_logic += 1;
}

vvp_fun_buf::~vvp_fun_buf()
{
}

/*
 * The buf functor is very simple--change the z bits to x bits in the
 * vector it passes, and propagate the result.
 */
શૂન્ય vvp_fun_buf::run_run()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      vvp_vector4_t tmp (આગત_2);
      tmp.change_z2x();
      ptr->send_vec4(tmp, 0);
}

vvp_fun_bufz::vvp_fun_bufz()
{
      ગણતરી_functors_logic += 1;
}

vvp_fun_bufz::~vvp_fun_bufz()
{
}

/*
 * The bufz is similar to the buf device, except that it does not
 * bother translating z bits to x.
 */
શૂન્ય vvp_fun_bufz::recv_vec4(vvp_net_ptr_t ptr, અચલ vvp_vector4_t&bit,
                             vvp_context_t)
{
      જો (ptr.port() != 0)
	    પાછા;

      ptr.ptr()->send_vec4(bit, 0);
}

શૂન્ય vvp_fun_bufz::recv_vec4_pv(vvp_net_ptr_t ptr, અચલ vvp_vector4_t&bit,
                                unsigned base, unsigned vwid, vvp_context_t)
{
      જો (ptr.port() != 0)
	    પાછા;

      ptr.ptr()->send_vec4_pv(bit, base, vwid, 0);
}

શૂન્ય vvp_fun_bufz::recv_real(vvp_net_ptr_t ptr, દ્વિગુણ bit,
                             vvp_context_t)
{
      જો (ptr.port() != 0)
	    પાછા;

      ptr.ptr()->send_real(bit, 0);
}

શૂન્ય vvp_fun_buft::recv_vec8(vvp_net_ptr_t ptr, અચલ vvp_vector8_t&bit)
{
      જો (ptr.port() != 0)
	    પાછા;

      ptr.ptr()->send_vec8(bit);
}

vvp_fun_muxr::vvp_fun_muxr()
: a_(0.0), b_(0.0)
{
      net_ = 0;
      ગણતરી_functors_logic += 1;
      select_ = SEL_BOTH;
}

vvp_fun_muxr::~vvp_fun_muxr()
{
}

શૂન્ય vvp_fun_muxr::recv_vec4(vvp_net_ptr_t ptr, અચલ vvp_vector4_t&bit,
                             vvp_context_t)
{
	/* The real valued mux can only take in the select as a
	   vector4_t. The muxed data is real. */
      જો (ptr.port() != 2)
	    પાછા;

      પ્રતિજ્ઞા(bit.size() == 1);

      પસંદગી (bit.value(0)) {
	  સ્થિતિ BIT4_0:
	    જો (select_ == SEL_PORT0) પાછા;
	    select_ = SEL_PORT0;
	    તોડો;
	  સ્થિતિ BIT4_1:
	    જો (select_ == SEL_PORT1) પાછા;
	    select_ = SEL_PORT1;
	    તોડો;
	  મૂળભૂત:
	    જો (select_ == SEL_BOTH) પાછા;
	    select_ = SEL_BOTH;
      }

      જો (net_ == 0) {
	    net_ = ptr.ptr();
	    schedule_functor(આ);
      }
}

શૂન્ય vvp_fun_muxr::recv_real(vvp_net_ptr_t ptr, દ્વિગુણ bit,
                             vvp_context_t)
{
      પસંદગી (ptr.port()) {
	  સ્થિતિ 0:
	    જો (a_ == bit) પાછા;
	    a_ = bit;
	    જો (select_ == SEL_PORT1) પાછા; // The other port is selected.
	    તોડો;

	  સ્થિતિ 1:
	    જો (b_ == bit) પાછા;
	    b_ = bit;
	    જો (select_ == SEL_PORT0) પાછા; // The other port is selected.
	    તોડો;

	  મૂળભૂત:
	    fprintf(stderr, "Unsupported port type %u.\n", ptr.port());
	    પ્રતિજ્ઞા(0);
      }

      જો (net_ == 0) {
	    net_ = ptr.ptr();
	    schedule_functor(આ);
      }
}

શૂન્ય vvp_fun_muxr::run_run()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      પસંદગી (select_) {
	  સ્થિતિ SEL_PORT0:
	    ptr->send_real(a_, 0);
	    તોડો;
	  સ્થિતિ SEL_PORT1:
	    ptr->send_real(b_, 0);
	    તોડો;
	  મૂળભૂત:
	    જો (a_ == b_) {
		  ptr->send_real(a_, 0);
	    } નહીંતર {
		  ptr->send_real(0.0, 0); // Should this be NaN?
	    }
	    તોડો;
      }
}

vvp_fun_muxz::vvp_fun_muxz(unsigned wid)
: a_(wid, BIT4_Z), b_(wid, BIT4_Z)
{
      net_ = 0;
      ગણતરી_functors_logic += 1;
      select_ = SEL_BOTH;
      has_run_ = ખોટું;
}

vvp_fun_muxz::~vvp_fun_muxz()
{
}

શૂન્ય vvp_fun_muxz::recv_vec4(vvp_net_ptr_t ptr, અચલ vvp_vector4_t&bit,
                             vvp_context_t)
{
      પસંદગી (ptr.port()) {
	  સ્થિતિ 0:
	    જો (a_ .eeq(bit) && has_run_) પાછા;
	    a_ = bit;
	    જો (select_ == SEL_PORT1) પાછા; // The other port is selected.
	    તોડો;
	  સ્થિતિ 1:
	    જો (b_ .eeq(bit) && has_run_) પાછા;
	    b_ = bit;
	    જો (select_ == SEL_PORT0) પાછા; // The other port is selected.
	    તોડો;
	  સ્થિતિ 2:
	    પ્રતિજ્ઞા(bit.size() == 1);
	    પસંદગી (bit.value(0)) {
		સ્થિતિ BIT4_0:
		  જો (select_ == SEL_PORT0) પાછા;
		  select_ = SEL_PORT0;
		  તોડો;
		સ્થિતિ BIT4_1:
		  જો (select_ == SEL_PORT1) પાછા;
		  select_ = SEL_PORT1;
		  તોડો;
		મૂળભૂત:
		  જો (select_ == SEL_BOTH && has_run_) પાછા;
		  select_ = SEL_BOTH;
	    }
	    તોડો;
	  મૂળભૂત:
	    પાછા;
      }

      જો (net_ == 0) {
	    net_ = ptr.ptr();
	    schedule_functor(આ);
      }
}

શૂન્ય vvp_fun_muxz::recv_vec4_pv(vvp_net_ptr_t ptr, અચલ vvp_vector4_t&bit,
				unsigned base, unsigned vwid, vvp_context_t)
{
      પ્રતિજ્ઞા(base + bit.size() <= vwid);
      તાર્કિક flag;

      પસંદગી (ptr.port()) {
	  સ્થિતિ 0:
	    flag = a_.set_vec(base, bit);
	    જો (flag == ખોટું && has_run_) પાછા;
	    જો (select_ == SEL_PORT1) પાછા; // The other port is selected.
	    તોડો;
	  સ્થિતિ 1:
	    flag = b_.set_vec(base, bit);
	    જો (flag == ખોટું && has_run_) પાછા;
	    જો (select_ == SEL_PORT0) પાછા; // The other port is selected.
	    તોડો;
	  સ્થિતિ 2:
	    પ્રતિજ્ઞા((base == 0) && (bit.size() == 1));
	    recv_vec4(ptr, bit, 0);
	  મૂળભૂત:
	    પાછા;
      }
      જો (net_ == 0) {
	    net_ = ptr.ptr();
	    schedule_functor(આ);
      }
}

શૂન્ય vvp_fun_muxz::run_run()
{
      has_run_ = સાચું;
      vvp_net_t*ptr = net_;
      net_ = 0;

      પસંદગી (select_) {
	  સ્થિતિ SEL_PORT0:
	    ptr->send_vec4(a_, 0);
	    તોડો;
	  સ્થિતિ SEL_PORT1:
	    ptr->send_vec4(b_, 0);
	    તોડો;
	  મૂળભૂત:
	      {
		    unsigned min_size = a_.size();
		    unsigned max_size = min_size;
		    જો (b_.size() < min_size)
			  min_size = b_.size();
		    જો (b_.size() > max_size)
			  max_size = b_.size();

		    vvp_vector4_t res (max_size);

		    માટે (unsigned idx = 0 ;  idx < min_size ;  idx += 1) {
			  જો (a_.value(idx) == b_.value(idx))
				res.set_bit(idx, a_.value(idx));
			  નહીંતર
				res.set_bit(idx, BIT4_X);
		    }

		    માટે (unsigned idx = min_size ;  idx < max_size ;  idx += 1)
			  res.set_bit(idx, BIT4_X);

		    ptr->send_vec4(res, 0);
	      }
	    તોડો;
      }
}

vvp_fun_not::vvp_fun_not(unsigned wid)
: vvp_fun_buf_not_(wid)
{
      ગણતરી_functors_logic += 1;
}

vvp_fun_not::~vvp_fun_not()
{
}

/*
 * The not functor is very simple--change the z bits to x bits in the
 * vector it passes, and propagate the inverted result.
 */
શૂન્ય vvp_fun_not::run_run()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      vvp_vector4_t result (આગત_2, સાચું /* invert */);
      ptr->send_vec4(result, 0);
}

vvp_fun_or::vvp_fun_or(unsigned wid, તાર્કિક invert)
: vvp_fun_boolean_(wid), invert_(invert)
{
      ગણતરી_functors_logic += 1;
}

vvp_fun_or::~vvp_fun_or()
{
}

શૂન્ય vvp_fun_or::run_run()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      vvp_vector4_t result (આગત_2[0]);

      માટે (unsigned pdx = 1 ; pdx < 4 ; pdx += 1)
	    result |= આગત_2[pdx];

      જો (invert_)
	    result.invert();

      ptr->send_vec4(result, 0);
}

vvp_fun_xor::vvp_fun_xor(unsigned wid, તાર્કિક invert)
: vvp_fun_boolean_(wid), invert_(invert)
{
      ગણતરી_functors_logic += 1;
}

vvp_fun_xor::~vvp_fun_xor()
{
}

શૂન્ય vvp_fun_xor::run_run()
{
      vvp_net_t*ptr = net_;
      net_ = 0;

      vvp_vector4_t result (આગત_2[0]);

      માટે (unsigned pdx = 1 ; pdx < 4 ; pdx += 1)
	    result ^= આગત_2[pdx];

      જો (invert_)
	    result.invert();

      ptr->send_vec4(result, 0);
}

/*
 * The parser calls this function to create a logic functor. I allocate a
 * functor, and map the name to the vvp_ipoint_t address for the
 * functor. Also resolve the inputs to the functor.
 */

શૂન્ય compile_functor(અક્ષર*label, અક્ષર*type, unsigned width,
		     unsigned ostr0, unsigned ostr1,
		     unsigned argc, સંરચના symb_s*argv)
{
      vvp_net_fun_t* obj = 0;
      તાર્કિક strength_aware = ખોટું;

      જો (strcmp(type, "OR") == 0) {
	    obj = નવું vvp_fun_or(width, ખોટું);

      } નહીંતર જો (strcmp(type, "AND") == 0) {
	    obj = નવું vvp_fun_and(width, ખોટું);

      } નહીંતર જો (strcmp(type, "BUF") == 0) {
	    obj = નવું vvp_fun_buf(width);

      } નહીંતર જો (strcmp(type, "BUFIF0") == 0) {
	    obj = નવું vvp_fun_bufif(સાચું,ખોટું, ostr0, ostr1);
	    strength_aware = સાચું;

      } નહીંતર જો (strcmp(type, "BUFIF1") == 0) {
	    obj = નવું vvp_fun_bufif(ખોટું,ખોટું, ostr0, ostr1);
	    strength_aware = સાચું;

      } નહીંતર જો (strcmp(type, "EQUIV") == 0) {
	    obj = નવું vvp_fun_equiv();

      } નહીંતર જો (strcmp(type, "IMPL") == 0) {
	    obj = નવું vvp_fun_impl();

      } નહીંતર જો (strcmp(type, "NAND") == 0) {
	    obj = નવું vvp_fun_and(width, સાચું);

      } નહીંતર જો (strcmp(type, "NOR") == 0) {
	    obj = નવું vvp_fun_or(width, સાચું);

      } નહીંતર જો (strcmp(type, "NOTIF0") == 0) {
	    obj = નવું vvp_fun_bufif(સાચું,સાચું, ostr0, ostr1);
	    strength_aware = સાચું;

      } નહીંતર જો (strcmp(type, "NOTIF1") == 0) {
	    obj = નવું vvp_fun_bufif(ખોટું,સાચું, ostr0, ostr1);
	    strength_aware = સાચું;

      } નહીંતર જો (strcmp(type, "BUFT") == 0) {
	    obj = નવું vvp_fun_buft();

      } નહીંતર જો (strcmp(type, "BUFZ") == 0) {
	    obj = નવું vvp_fun_bufz();

      } નહીંતર જો (strcmp(type, "MUXR") == 0) {
	    obj = નવું vvp_fun_muxr;

      } નહીંતર જો (strcmp(type, "MUXZ") == 0) {
	    obj = નવું vvp_fun_muxz(width);

      } નહીંતર જો (strcmp(type, "CMOS") == 0) {
	    obj = નવું vvp_fun_cmos();

      } નહીંતર જો (strcmp(type, "NMOS") == 0) {
	    obj = નવું vvp_fun_pmos(સાચું);

      } નહીંતર જો (strcmp(type, "PMOS") == 0) {
	    obj = નવું vvp_fun_pmos(ખોટું);

      } નહીંતર જો (strcmp(type, "RCMOS") == 0) {
	    obj = નવું vvp_fun_rcmos();

      } નહીંતર જો (strcmp(type, "RNMOS") == 0) {
	    obj = નવું vvp_fun_rpmos(સાચું);

      } નહીંતર જો (strcmp(type, "RPMOS") == 0) {
	    obj = નવું vvp_fun_rpmos(ખોટું);

      } નહીંતર જો (strcmp(type, "NOT") == 0) {
	    obj = નવું vvp_fun_not(width);

      } નહીંતર જો (strcmp(type, "XNOR") == 0) {
	    obj = નવું vvp_fun_xor(width, સાચું);

      } નહીંતર જો (strcmp(type, "XOR") == 0) {
	    obj = નવું vvp_fun_xor(width, ખોટું);

      } નહીંતર {
	    yyerror("invalid functor type.");
	    મુક્ત(type);
	    મુક્ત(argv);
	    મુક્ત(label);
	    પાછા;
      }

      મુક્ત(type);

      પ્રતિજ્ઞા(argc <= 4);
      vvp_net_t*net = નવું vvp_net_t;
      net->fun = obj;

      inputs_connect(net, argc, argv);
      મુક્ત(argv);

	/* If both the strengths are the default strong drive, then
	   there is no need for a specialized driver. Attach the label
	   to this node and we are finished. */
      જો (strength_aware || (ostr0 == 6 && ostr1 == 6)) {
	    define_functor_પ્રતીક(label, net);
	    મુક્ત(label);
	    પાછા;
      }

      vvp_net_t*net_drv = નવું vvp_net_t;
      vvp_net_fun_t*obj_drv = નવું vvp_fun_drive(ostr0, ostr1);
      net_drv->fun = obj_drv;

	/* Point the gate to the drive node. */
      net->link(vvp_net_ptr_t(net_drv, 0));

      define_functor_પ્રતીક(label, net_drv);
      મુક્ત(label);
}
