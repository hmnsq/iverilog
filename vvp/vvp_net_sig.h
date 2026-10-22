#ifndef IVL_vvp_net_sig_H
#define IVL_vvp_net_sig_H
/*
 * Copyright (c) 2004-2026 Stephen Williams (steve@icarus.com)
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

# include  "config.h"
# include  "vpi_user.h"
# include  "vvp_net.h"
# include  "vvp_object.h"
# include  <string>
# include  <cstddef>
# include  <cstdlib>
# include  <cstring>
# include  <new>
# include  <cassert>

#ifdef HAVE_IOSFWD
# include  <iosfwd>
#ଅନ୍ୟଥା
# include  <iostream>
#endif

/* vvp_fun_signal
 * This node is the place holder in a vvp network for signals,
 * including nets of various sort. The output from a signal follows
 * the type of its port-0 input. If vvp_vector4_t values come in
 * through port-0, then vvp_vector4_t values are propagated. If
 * vvp_vector8_t values come in through port-0, then vvp_vector8_t
 * values are propagated. Thus, this node is slightly polymorphic.
 *
 * If the signal is a net (i.e. a wire or tri) then this node will
 * have an input that is the data source. The data source will connect
 * through port-0.
 *
 * If the signal is a reg, then there will be no netlist input, the
 * values will be written by behavioral statements. The %set and
 * %assign statements will write through port-0.
 *
 * In any case, behavioral code is able to read the value that this
 * node last propagated, by using the value() method. That is important
 * functionality of this node.
 *
 * Continuous assignments are made through port-1. When a value is
 * written here, continuous assign mode is activated, and input
 * through port-0 is ignored until continuous assign mode is turned
 * off again. Writing into this port can be done in behavioral code
 * using the %cassign/v instruction, or can be done by the network by
 * hooking the output of a vvp_net_t to this port.
 */


class vvp_fun_signal_base : ସର୍ବସାଧାରଣ vvp_net_fun_t {

    ସର୍ବସାଧାରଣ:
      vvp_fun_signal_base();

      ଶୂନ୍ୟ deassign();
      ଶୂନ୍ୟ deassign_pv(unsigned base, unsigned wid);

    ସର୍ବସାଧାରଣ:

	/* The %cassign/link instruction needs a place to write the
	   source node of the force, so that subsequent %cassign and
	   %deassign instructions can undo the link as needed. */
      class vvp_net_t*cassign_link;

    ସୁରକ୍ଷିତ:
      bool continuous_assign_active_;
      vvp_vector2_t assign_mask_;

    ସୁରକ୍ଷିତ:
	// This is true until at least one propagation happens.
      bool needs_init_;
};

/*
 * Variables and wires can have their values accessed, so this base
 * class offers the unified concept of an accessible value.
 */
class vvp_signal_ମୂଲ୍ୟ {
    ସର୍ବସାଧାରଣ:
      ଆଭାସୀ ~vvp_signal_ମୂଲ୍ୟ() =0;
      ଆଭାସୀ unsigned ମୂଲ୍ୟ_size() ଅଚଳ =0;
      ଆଭାସୀ vvp_bit4_t ମୂଲ୍ୟ(unsigned idx) ଅଚଳ =0;
      ଆଭାସୀ vvp_scalar_t scalar_ମୂଲ୍ୟ(unsigned idx) ଅଚଳ =0;
      ଆଭାସୀ ଶୂନ୍ୟ vec4_ମୂଲ୍ୟ(vvp_vector4_t&) ଅଚଳ =0;
      ଆଭାସୀ ଦ୍ୱିଗୁଣ real_ମୂଲ୍ୟ() ଅଚଳ;

      ଆଭାସୀ ଶୂନ୍ୟ get_signal_ମୂଲ୍ୟ(ସଂରଚନା t_vpi_ମୂଲ୍ୟ*vp);
};

/*
 * This abstract class is a little more specific than the signal_base
 * class, in that it adds vector access methods.
 */
class vvp_fun_signal_vec : ସର୍ବସାଧାରଣ vvp_fun_signal_base {
    ସର୍ବସାଧାରଣ:
      ଆଭାସୀ ଅଚଳ vvp_vector4_t& vec4_unfiltered_ମୂଲ୍ୟ() ଅଚଳ =0;
};

class automatic_signal_base : ସର୍ବସାଧାରଣ vvp_signal_ମୂଲ୍ୟ, ସର୍ବସାଧାରଣ vvp_net_fil_t {

    ସର୍ବସାଧାରଣ:
	// Automatic variables cannot be forced or released. Provide
	// stubs that assert.
      ଆଭାସୀ ଶୂନ୍ୟ release(vvp_net_ptr_t ptr, bool net_flag) override;
      ଆଭାସୀ ଶୂନ୍ୟ release_pv(vvp_net_ptr_t ptr, unsigned base, unsigned wid, bool net_flag) override;

      ଆଭାସୀ unsigned filter_size() ଅଚଳ override;
      ଆଭାସୀ ଶୂନ୍ୟ force_fil_vec4(ଅଚଳ vvp_vector4_t&val, ଅଚଳ vvp_vector2_t&mask) override;
      ଆଭାସୀ ଶୂନ୍ୟ force_fil_vec8(ଅଚଳ vvp_vector8_t&val, ଅଚଳ vvp_vector2_t&mask) override;
      ଆଭାସୀ ଶୂନ୍ୟ force_fil_real(ଦ୍ୱିଗୁଣ val, ଅଚଳ vvp_vector2_t&mask) override;
      ଆଭାସୀ ଶୂନ୍ୟ get_ମୂଲ୍ୟ(ସଂରଚନା t_vpi_ମୂଲ୍ୟ*ମୂଲ୍ୟ) override;
};

/*
 * Statically allocated vvp_fun_signal4.
 */
class vvp_fun_signal4_sa : ସର୍ବସାଧାରଣ vvp_fun_signal_vec {

    ସର୍ବସାଧାରଣ:
      explicit vvp_fun_signal4_sa(unsigned wid, vvp_bit4_t init=BIT4_X);

      ଶୂନ୍ୟ recv_vec4(vvp_net_ptr_t port, ଅଚଳ vvp_vector4_t&bit,
                     vvp_context_t) override;
      ଶୂନ୍ୟ recv_vec8(vvp_net_ptr_t port, ଅଚଳ vvp_vector8_t&bit) override;

	// Part select variants of above
      ଶୂନ୍ୟ recv_vec4_pv(vvp_net_ptr_t port, ଅଚଳ vvp_vector4_t&bit,
			unsigned base, unsigned vwid, vvp_context_t) override;
      ଶୂନ୍ୟ recv_vec8_pv(vvp_net_ptr_t port, ଅଚଳ vvp_vector8_t&bit,
			unsigned base, unsigned vwid) override;

	// Get information about the vector value.
      ଅଚଳ vvp_vector4_t& vec4_unfiltered_ମୂଲ୍ୟ() ଅଚଳ override;

    ବ୍ୟକ୍ତିଗତ:
      vvp_vector4_t bits4_;
};

/*
 * Automatically allocated vvp_fun_signal4.
 */
class vvp_fun_signal4_aa : ସର୍ବସାଧାରଣ vvp_fun_signal_vec, ସର୍ବସାଧାରଣ automatic_signal_base, ସର୍ବସାଧାରଣ automatic_hooks_s {

    ସର୍ବସାଧାରଣ:
      explicit vvp_fun_signal4_aa(unsigned wid, vvp_bit4_t init=BIT4_X);
      ~vvp_fun_signal4_aa() override;

      ଶୂନ୍ୟ alloc_instance(vvp_context_t context) override;
      ଶୂନ୍ୟ reset_instance(vvp_context_t context) override;
#ifdef CHECK_WITH_VALGRIND
      ଶୂନ୍ୟ free_instance(vvp_context_t context) override;
#endif

      ଶୂନ୍ୟ recv_vec4(vvp_net_ptr_t port, ଅଚଳ vvp_vector4_t&bit,
                     vvp_context_t context) override;

	// Part select variants of above
      ଶୂନ୍ୟ recv_vec4_pv(vvp_net_ptr_t port, ଅଚଳ vvp_vector4_t&bit,
			unsigned base, unsigned vwid, vvp_context_t) override;

	// Get information about the vector value.
      unsigned   ମୂଲ୍ୟ_size() ଅଚଳ override;
      vvp_bit4_t ମୂଲ୍ୟ(unsigned idx) ଅଚଳ override;
      vvp_scalar_t scalar_ମୂଲ୍ୟ(unsigned idx) ଅଚଳ override;
      ଶୂନ୍ୟ vec4_ମୂଲ୍ୟ(vvp_vector4_t&) ଅଚଳ override;
      ଅଚଳ vvp_vector4_t& vec4_unfiltered_ମୂଲ୍ୟ() ଅଚଳ override;

    ସର୍ବସାଧାରଣ: // These objects are only permallocated.
      ସ୍ଥିର ଶୂନ୍ୟ* operator new(std::size_t size) { ଫେରନ୍ତୁ vvp_net_fun_t::heap_.alloc(size); }
      ସ୍ଥିର ଶୂନ୍ୟ operator delete(ଶୂନ୍ୟ*obj);

    ବ୍ୟକ୍ତିଗତ:
      unsigned context_idx_;
      unsigned size_;
      vvp_bit4_t init_;
};

class vvp_fun_signal_real : ସର୍ବସାଧାରଣ vvp_fun_signal_base {

    ସର୍ବସାଧାରଣ:
      explicit vvp_fun_signal_real() {};

	// Get information about the vector value.
      ଆଭାସୀ ଦ୍ୱିଗୁଣ real_unfiltered_ମୂଲ୍ୟ() ଅଚଳ = 0;
};

/*
 * Statically allocated vvp_fun_signal_real.
 */
class vvp_fun_signal_real_sa : ସର୍ବସାଧାରଣ vvp_fun_signal_real {

    ସର୍ବସାଧାରଣ:
      explicit vvp_fun_signal_real_sa();

      ଶୂନ୍ୟ recv_real(vvp_net_ptr_t port, ଦ୍ୱିଗୁଣ bit,
                     vvp_context_t) override;

	// Get information about the vector value.
      ଦ୍ୱିଗୁଣ real_unfiltered_ମୂଲ୍ୟ() ଅଚଳ override;

    ବ୍ୟକ୍ତିଗତ:
      ଦ୍ୱିଗୁଣ bits_;
};

/*
 * Automatically allocated vvp_fun_signal_real.
 */
class vvp_fun_signal_real_aa : ସର୍ବସାଧାରଣ vvp_fun_signal_real, ସର୍ବସାଧାରଣ automatic_signal_base, ସର୍ବସାଧାରଣ automatic_hooks_s {

    ସର୍ବସାଧାରଣ:
      explicit vvp_fun_signal_real_aa();
      ~vvp_fun_signal_real_aa() override;

      ଶୂନ୍ୟ alloc_instance(vvp_context_t context) override;
      ଶୂନ୍ୟ reset_instance(vvp_context_t context) override;
#ifdef CHECK_WITH_VALGRIND
      ଶୂନ୍ୟ free_instance(vvp_context_t context) override;
#endif

      ଶୂନ୍ୟ recv_real(vvp_net_ptr_t port, ଦ୍ୱିଗୁଣ bit,
                     vvp_context_t context) override;

	// Get information about the vector value.
      ଦ୍ୱିଗୁଣ real_unfiltered_ମୂଲ୍ୟ() ଅଚଳ override;

	// Get information about the vector value.
      unsigned   ମୂଲ୍ୟ_size() ଅଚଳ override;
      vvp_bit4_t ମୂଲ୍ୟ(unsigned idx) ଅଚଳ override;
      vvp_scalar_t scalar_ମୂଲ୍ୟ(unsigned idx) ଅଚଳ override;
      ଶୂନ୍ୟ vec4_ମୂଲ୍ୟ(vvp_vector4_t&) ଅଚଳ override;
      ଦ୍ୱିଗୁଣ real_ମୂଲ୍ୟ() ଅଚଳ override;
      ଶୂନ୍ୟ get_signal_ମୂଲ୍ୟ(ସଂରଚନା t_vpi_ମୂଲ୍ୟ*vp) override;

    ସର୍ବସାଧାରଣ: // These objects are only permallocated.
      ସ୍ଥିର ଶୂନ୍ୟ* operator new(std::size_t size);
      ସ୍ଥିର ଶୂନ୍ୟ operator delete(ଶୂନ୍ୟ*obj);

    ବ୍ୟକ୍ତିଗତ:
      unsigned context_idx_;
};


class vvp_fun_signal_string : ସର୍ବସାଧାରଣ vvp_fun_signal_base {

    ସର୍ବସାଧାରଣ:
      explicit vvp_fun_signal_string() {};

      ଆଭାସୀ ଅଚଳ std::string& get_string() ଅଚଳ =0;
};

/*
 * Statically allocated vvp_fun_signal_string.
 */
class vvp_fun_signal_string_sa : ସର୍ବସାଧାରଣ vvp_fun_signal_string {

    ସର୍ବସାଧାରଣ:
      explicit vvp_fun_signal_string_sa();

      ଶୂନ୍ୟ recv_string(vvp_net_ptr_t port, ଅଚଳ std::string&bit,
		       vvp_context_t context) override;

      ଅଚଳ std::string& get_string() ଅଚଳ override;

    ବ୍ୟକ୍ତିଗତ:
      std::string ମୂଲ୍ୟ_2;
};

/*
 * Automatically allocated vvp_fun_signal_real.
 */
class vvp_fun_signal_string_aa : ସର୍ବସାଧାରଣ vvp_fun_signal_string, ସର୍ବସାଧାରଣ automatic_signal_base, ସର୍ବସାଧାରଣ automatic_hooks_s {

    ସର୍ବସାଧାରଣ:
      explicit vvp_fun_signal_string_aa();
      ~vvp_fun_signal_string_aa() override;

      ଶୂନ୍ୟ alloc_instance(vvp_context_t context) override;
      ଶୂନ୍ୟ reset_instance(vvp_context_t context) override;
#ifdef CHECK_WITH_VALGRIND
      ଶୂନ୍ୟ free_instance(vvp_context_t context) override;
#endif
      ଶୂନ୍ୟ recv_string(vvp_net_ptr_t port, ଅଚଳ std::string&bit,
		       vvp_context_t context) override;

	// Get information about the vector value.
      unsigned   ମୂଲ୍ୟ_size() ଅଚଳ override;
      vvp_bit4_t ମୂଲ୍ୟ(unsigned idx) ଅଚଳ override;
      vvp_scalar_t scalar_ମୂଲ୍ୟ(unsigned idx) ଅଚଳ override;
      ଶୂନ୍ୟ vec4_ମୂଲ୍ୟ(vvp_vector4_t&) ଅଚଳ override;
      ଦ୍ୱିଗୁଣ real_ମୂଲ୍ୟ() ଅଚଳ override;
      ଅଚଳ std::string& get_string() ଅଚଳ override;
      ଶୂନ୍ୟ get_signal_ମୂଲ୍ୟ(ସଂରଚନା t_vpi_ମୂଲ୍ୟ*vp) override;

    ସର୍ବସାଧାରଣ: // These objects are only permallocated.
      ସ୍ଥିର ଶୂନ୍ୟ* operator new(std::size_t size);
      ସ୍ଥିର ଶୂନ୍ୟ operator delete(ଶୂନ୍ୟ*obj);

    ବ୍ୟକ୍ତିଗତ:
      unsigned context_idx_;
};

class vvp_fun_signal_object : ସର୍ବସାଧାରଣ vvp_fun_signal_base {

    ସର୍ବସାଧାରଣ:
      explicit vvp_fun_signal_object(unsigned size) { size_ = size; };
      unsigned size() ଅଚଳ { ଫେରନ୍ତୁ size_; }

      ଆଭାସୀ vvp_object_t get_object() ଅଚଳ =0;
    ବ୍ୟକ୍ତିଗତ:
      unsigned size_;
};

/*
 * Statically allocated vvp_fun_signal_string.
 */
class vvp_fun_signal_object_sa : ସର୍ବସାଧାରଣ vvp_fun_signal_object {

    ସର୍ବସାଧାରଣ:
      explicit vvp_fun_signal_object_sa(unsigned size);

      ଶୂନ୍ୟ recv_object(vvp_net_ptr_t port, vvp_object_t bit,
		    vvp_context_t context) override;

      vvp_object_t get_object() ଅଚଳ override;

    ବ୍ୟକ୍ତିଗତ:
      vvp_object_t ମୂଲ୍ୟ_2;
};

/*
 * Automatically allocated vvp_fun_signal_real.
 */
class vvp_fun_signal_object_aa : ସର୍ବସାଧାରଣ vvp_fun_signal_object, ସର୍ବସାଧାରଣ automatic_signal_base, ସର୍ବସାଧାରଣ automatic_hooks_s {

    ସର୍ବସାଧାରଣ:
      explicit vvp_fun_signal_object_aa(unsigned size);
      ~vvp_fun_signal_object_aa() override;

      ଶୂନ୍ୟ alloc_instance(vvp_context_t context) override;
      ଶୂନ୍ୟ reset_instance(vvp_context_t context) override;
#ifdef CHECK_WITH_VALGRIND
      ଶୂନ୍ୟ free_instance(vvp_context_t context) override;
#endif

      ଶୂନ୍ୟ recv_object(vvp_net_ptr_t port, vvp_object_t bit,
		    vvp_context_t context) override;

	// Get information about the vector value.
      unsigned   ମୂଲ୍ୟ_size() ଅଚଳ override;
      vvp_bit4_t ମୂଲ୍ୟ(unsigned idx) ଅଚଳ override;
      vvp_scalar_t scalar_ମୂଲ୍ୟ(unsigned idx) ଅଚଳ override;
      ଶୂନ୍ୟ vec4_ମୂଲ୍ୟ(vvp_vector4_t&) ଅଚଳ override;
	//double real_value() const;
	//void get_signal_value(struct t_vpi_value*vp);

      vvp_object_t get_object() ଅଚଳ override;

    ସର୍ବସାଧାରଣ: // These objects are only permallocated.
      ସ୍ଥିର ଶୂନ୍ୟ* operator new(std::size_t size);
      ସ୍ଥିର ଶୂନ୍ୟ operator delete(ଶୂନ୍ୟ*obj);

    ବ୍ୟକ୍ତିଗତ:
      unsigned context_idx_;
};


/* vvp_wire
 * The vvp_wire is different from vvp_variable objects in that it
 * exists only as a filter. The vvp_wire class tree is for
 * implementing Verilog wires/nets (as opposed to regs/variables).
 *
 *   vvp_vpi_callback
 *          |
 *          |
 *    vvp_net_fil_t   vvp_signal_value
 *          |               |
 *           \             /
 *            \           /
 *             \         /
 *            vvp_wire_base
 */

class vvp_wire_base  : ସର୍ବସାଧାରଣ vvp_net_fil_t, ସର୍ବସାଧାରଣ vvp_signal_ମୂଲ୍ୟ {

    ସର୍ବସାଧାରଣ:
      vvp_wire_base();
      ~vvp_wire_base() override;

        // Support for $countdrivers
      ଆଭାସୀ vvp_bit4_t driven_ମୂଲ୍ୟ(unsigned idx) ଅଚଳ;
      ଆଭାସୀ bool is_forced(unsigned idx) ଅଚଳ;
};

class vvp_wire_vec4 : ସର୍ବସାଧାରଣ vvp_wire_base {

    ସର୍ବସାଧାରଣ:
      vvp_wire_vec4(unsigned wid, vvp_bit4_t init);

	// The main filter behavior for this class. These methods take
	// the value that the node is driven to, and applies the force
	// filters. In wires, this also saves the driven value, so
	// that when a force is released, we can revert to the driven value.
      prop_t filter_vec4(ଅଚଳ vvp_vector4_t&bit, vvp_vector4_t&rep,
			 unsigned base, unsigned vwid) override;
      prop_t filter_vec8(ଅଚଳ vvp_vector8_t&val, vvp_vector8_t&rep,
			 unsigned base, unsigned vwid) override;

	// Abstract methods from vvp_vpi_callback
      ଶୂନ୍ୟ get_ମୂଲ୍ୟ(ସଂରଚନା t_vpi_ମୂଲ୍ୟ*ମୂଲ୍ୟ) override;
	// Abstract methods from vvp_net_fit_t
      unsigned filter_size() ଅଚଳ override;
      ଶୂନ୍ୟ force_fil_vec4(ଅଚଳ vvp_vector4_t&val, ଅଚଳ vvp_vector2_t&mask) override;
      ଶୂନ୍ୟ force_fil_vec8(ଅଚଳ vvp_vector8_t&val, ଅଚଳ vvp_vector2_t&mask) override;
      ଶୂନ୍ୟ force_fil_real(ଦ୍ୱିଗୁଣ val, ଅଚଳ vvp_vector2_t&mask) override;
      ଶୂନ୍ୟ release(vvp_net_ptr_t ptr, bool net_flag) override;
      ଶୂନ୍ୟ release_pv(vvp_net_ptr_t ptr, unsigned base, unsigned wid, bool net_flag) override;

	// Implementation of vvp_signal_value methods
      unsigned ମୂଲ୍ୟ_size() ଅଚଳ override;
      vvp_bit4_t ମୂଲ୍ୟ(unsigned idx) ଅଚଳ override;
      vvp_scalar_t scalar_ମୂଲ୍ୟ(unsigned idx) ଅଚଳ override;
      ଶୂନ୍ୟ vec4_ମୂଲ୍ୟ(vvp_vector4_t&) ଅଚଳ override;

        // Support for $countdrivers
      vvp_bit4_t driven_ମୂଲ୍ୟ(unsigned idx) ଅଚଳ override;
      bool is_forced(unsigned idx) ଅଚଳ override;

    ବ୍ୟକ୍ତିଗତ:
      vvp_bit4_t filtered_ମୂଲ୍ୟ(unsigned idx) ଅଚଳ;

    ବ୍ୟକ୍ତିଗତ:
      bool needs_init_;
      vvp_vector4_t bits4_; // The tracked driven value
      vvp_vector4_t force4_; // the value being forced
};

class vvp_wire_vec8 : ସର୍ବସାଧାରଣ vvp_wire_base {

    ସର୍ବସାଧାରଣ:
      explicit vvp_wire_vec8(unsigned wid);

	// The main filter behavior for this class
      prop_t filter_vec4(ଅଚଳ vvp_vector4_t&bit, vvp_vector4_t&rep,
			 unsigned base, unsigned vwid) override;
      prop_t filter_vec8(ଅଚଳ vvp_vector8_t&val, vvp_vector8_t&rep,
			 unsigned base, unsigned vwid) override;

	// island ports use this method to filter arbitrary values
	// through the force filter.
      prop_t filter_ନିବେଶ_vec8(ଅଚଳ vvp_vector8_t&val, vvp_vector8_t&rep) ଅଚଳ;


	// Abstract methods from vvp_vpi_callback
      ଶୂନ୍ୟ get_ମୂଲ୍ୟ(ସଂରଚନା t_vpi_ମୂଲ୍ୟ*ମୂଲ୍ୟ) override;
	// Abstract methods from vvp_net_fit_t
      unsigned filter_size() ଅଚଳ override;
      ଶୂନ୍ୟ force_fil_vec4(ଅଚଳ vvp_vector4_t&val, ଅଚଳ vvp_vector2_t&mask) override;
      ଶୂନ୍ୟ force_fil_vec8(ଅଚଳ vvp_vector8_t&val, ଅଚଳ vvp_vector2_t&mask) override;
      ଶୂନ୍ୟ force_fil_real(ଦ୍ୱିଗୁଣ val, ଅଚଳ vvp_vector2_t&mask) override;
      ଶୂନ୍ୟ release(vvp_net_ptr_t ptr, bool net_flag) override;
      ଶୂନ୍ୟ release_pv(vvp_net_ptr_t ptr, unsigned base, unsigned wid, bool net_flag) override;

	// Implementation of vvp_signal_value methods
      unsigned ମୂଲ୍ୟ_size() ଅଚଳ override;
      vvp_bit4_t ମୂଲ୍ୟ(unsigned idx) ଅଚଳ override;
      vvp_scalar_t scalar_ମୂଲ୍ୟ(unsigned idx) ଅଚଳ override;
      ଶୂନ୍ୟ vec4_ମୂଲ୍ୟ(vvp_vector4_t&) ଅଚଳ override;
	// This is new to vvp_wire_vec8
      vvp_vector8_t vec8_ମୂଲ୍ୟ() ଅଚଳ;

        // Support for $countdrivers
      vvp_bit4_t driven_ମୂଲ୍ୟ(unsigned idx) ଅଚଳ override;
      bool is_forced(unsigned idx) ଅଚଳ override;

    ବ୍ୟକ୍ତିଗତ:
      vvp_scalar_t filtered_ମୂଲ୍ୟ(unsigned idx) ଅଚଳ;

    ବ୍ୟକ୍ତିଗତ:
      bool needs_init_;
      vvp_vector8_t bits8_;
      vvp_vector8_t force8_; // the value being forced
};

class vvp_wire_real : ସର୍ବସାଧାରଣ vvp_wire_base {

    ସର୍ବସାଧାରଣ:
      explicit vvp_wire_real(ଶୂନ୍ୟ);

	// The main filter behavior for this class
      prop_t filter_real(ଦ୍ୱିଗୁଣ&bit) override;

	// Abstract methods from vvp_vpi_callback
      ଶୂନ୍ୟ get_ମୂଲ୍ୟ(ସଂରଚନା t_vpi_ମୂଲ୍ୟ*ମୂଲ୍ୟ) override;
	// Abstract methods from vvp_net_fit_t
      unsigned filter_size() ଅଚଳ override;
      ଶୂନ୍ୟ force_fil_vec4(ଅଚଳ vvp_vector4_t&val, ଅଚଳ vvp_vector2_t&mask) override;
      ଶୂନ୍ୟ force_fil_vec8(ଅଚଳ vvp_vector8_t&val, ଅଚଳ vvp_vector2_t&mask) override;
      ଶୂନ୍ୟ force_fil_real(ଦ୍ୱିଗୁଣ val, ଅଚଳ vvp_vector2_t&mask) override;
      ଶୂନ୍ୟ release(vvp_net_ptr_t ptr, bool net_flag) override;
      ଶୂନ୍ୟ release_pv(vvp_net_ptr_t ptr, unsigned base, unsigned wid, bool net_flag) override;

	// Implementation of vvp_signal_value methods
      unsigned ମୂଲ୍ୟ_size() ଅଚଳ override;
      vvp_bit4_t ମୂଲ୍ୟ(unsigned idx) ଅଚଳ override;
      vvp_scalar_t scalar_ମୂଲ୍ୟ(unsigned idx) ଅଚଳ override;
      ଶୂନ୍ୟ vec4_ମୂଲ୍ୟ(vvp_vector4_t&) ଅଚଳ override;
      ଦ୍ୱିଗୁଣ real_ମୂଲ୍ୟ() ଅଚଳ override;

      ଶୂନ୍ୟ get_signal_ମୂଲ୍ୟ(ସଂରଚନା t_vpi_ମୂଲ୍ୟ*vp) override;

    ବ୍ୟକ୍ତିଗତ:
      ଦ୍ୱିଗୁଣ bit_;
      ଦ୍ୱିଗୁଣ force_;
      bool needs_init_ = true;
};

#ଯଦି 0
class vvp_wire_string : ସର୍ବସାଧାରଣ vvp_wire_base {

    ସର୍ବସାଧାରଣ:
      explicit vvp_wire_string(ଶୂନ୍ୟ);

	// Abstract methods from vvp_vpi_callback
      ଶୂନ୍ୟ get_ମୂଲ୍ୟ(ସଂରଚନା t_vpi_ମୂଲ୍ୟ*ମୂଲ୍ୟ);
	// Abstract methods from vvp_net_fil_t
      unsigned filter_size() ଅଚଳ;
      ଶୂନ୍ୟ force_fil_vec4(ଅଚଳ vvp_vector4_t&val, ଅଚଳ vvp_vector2_t&mask);
      ଶୂନ୍ୟ force_fil_vec8(ଅଚଳ vvp_vector8_t&val, ଅଚଳ vvp_vector2_t&mask);
      ଶୂନ୍ୟ force_fil_real(ଦ୍ୱିଗୁଣ val, ଅଚଳ vvp_vector2_t&mask);
      ଶୂନ୍ୟ release(vvp_net_ptr_t ptr, bool net_flag);
      ଶୂନ୍ୟ release_pv(vvp_net_ptr_t ptr, unsigned base, unsigned wid, bool net_flag);

	// Implementation of vvp_signal_value methods
      unsigned ମୂଲ୍ୟ_size() ଅଚଳ;
      vvp_bit4_t ମୂଲ୍ୟ(unsigned idx) ଅଚଳ;
      vvp_scalar_t scalar_ମୂଲ୍ୟ(unsigned idx) ଅଚଳ;
      ଶୂନ୍ୟ vec4_ମୂଲ୍ୟ(vvp_vector4_t&) ଅଚଳ;
      ଦ୍ୱିଗୁଣ real_ମୂଲ୍ୟ() ଅଚଳ;

      ଶୂନ୍ୟ get_signal_ମୂଲ୍ୟ(ସଂରଚନା t_vpi_ମୂଲ୍ୟ*vp);

    ବ୍ୟକ୍ତିଗତ:
      std::string ମୂଲ୍ୟ_2;
};
#endif

#endif /* IVL_vvp_net_sig_H */
