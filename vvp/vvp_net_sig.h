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
#അല്ലെങ്കിൽ
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


class vvp_fun_signal_base : പൊതു vvp_net_fun_t {

    പൊതു:
      vvp_fun_signal_base();

      ശൂന്യം deassign();
      ശൂന്യം deassign_pv(unsigned base, unsigned wid);

    പൊതു:

	/* The %cassign/link instruction needs a place to write the
	   source node of the force, so that subsequent %cassign and
	   %deassign instructions can undo the link as needed. */
      class vvp_net_t*cassign_link;

    സംരക്ഷിത:
      bool continuous_assign_active_;
      vvp_vector2_t assign_mask_;

    സംരക്ഷിത:
	// This is true until at least one propagation happens.
      bool needs_init_;
};

/*
 * Variables and wires can have their values accessed, so this base
 * class offers the unified concept of an accessible value.
 */
class vvp_signal_മൂല്യം {
    പൊതു:
      മായാ ~vvp_signal_മൂല്യം() =0;
      മായാ unsigned മൂല്യം_size() സ്ഥിരം =0;
      മായാ vvp_bit4_t മൂല്യം(unsigned idx) സ്ഥിരം =0;
      മായാ vvp_scalar_t scalar_മൂല്യം(unsigned idx) സ്ഥിരം =0;
      മായാ ശൂന്യം vec4_മൂല്യം(vvp_vector4_t&) സ്ഥിരം =0;
      മായാ ഇരട്ട real_മൂല്യം() സ്ഥിരം;

      മായാ ശൂന്യം get_signal_മൂല്യം(ഘടന t_vpi_മൂല്യം*vp);
};

/*
 * This abstract class is a little more specific than the signal_base
 * class, in that it adds vector access methods.
 */
class vvp_fun_signal_vec : പൊതു vvp_fun_signal_base {
    പൊതു:
      മായാ സ്ഥിരം vvp_vector4_t& vec4_unfiltered_മൂല്യം() സ്ഥിരം =0;
};

class automatic_signal_base : പൊതു vvp_signal_മൂല്യം, പൊതു vvp_net_fil_t {

    പൊതു:
	// Automatic variables cannot be forced or released. Provide
	// stubs that assert.
      മായാ ശൂന്യം release(vvp_net_ptr_t ptr, bool net_flag) override;
      മായാ ശൂന്യം release_pv(vvp_net_ptr_t ptr, unsigned base, unsigned wid, bool net_flag) override;

      മായാ unsigned filter_size() സ്ഥിരം override;
      മായാ ശൂന്യം force_fil_vec4(സ്ഥിരം vvp_vector4_t&val, സ്ഥിരം vvp_vector2_t&mask) override;
      മായാ ശൂന്യം force_fil_vec8(സ്ഥിരം vvp_vector8_t&val, സ്ഥിരം vvp_vector2_t&mask) override;
      മായാ ശൂന്യം force_fil_real(ഇരട്ട val, സ്ഥിരം vvp_vector2_t&mask) override;
      മായാ ശൂന്യം get_മൂല്യം(ഘടന t_vpi_മൂല്യം*മൂല്യം) override;
};

/*
 * Statically allocated vvp_fun_signal4.
 */
class vvp_fun_signal4_sa : പൊതു vvp_fun_signal_vec {

    പൊതു:
      explicit vvp_fun_signal4_sa(unsigned wid, vvp_bit4_t init=BIT4_X);

      ശൂന്യം recv_vec4(vvp_net_ptr_t port, സ്ഥിരം vvp_vector4_t&bit,
                     vvp_context_t) override;
      ശൂന്യം recv_vec8(vvp_net_ptr_t port, സ്ഥിരം vvp_vector8_t&bit) override;

	// Part select variants of above
      ശൂന്യം recv_vec4_pv(vvp_net_ptr_t port, സ്ഥിരം vvp_vector4_t&bit,
			unsigned base, unsigned vwid, vvp_context_t) override;
      ശൂന്യം recv_vec8_pv(vvp_net_ptr_t port, സ്ഥിരം vvp_vector8_t&bit,
			unsigned base, unsigned vwid) override;

	// Get information about the vector value.
      സ്ഥിരം vvp_vector4_t& vec4_unfiltered_മൂല്യം() സ്ഥിരം override;

    സ്വകാര്യ:
      vvp_vector4_t bits4_;
};

/*
 * Automatically allocated vvp_fun_signal4.
 */
class vvp_fun_signal4_aa : പൊതു vvp_fun_signal_vec, പൊതു automatic_signal_base, പൊതു automatic_hooks_s {

    പൊതു:
      explicit vvp_fun_signal4_aa(unsigned wid, vvp_bit4_t init=BIT4_X);
      ~vvp_fun_signal4_aa() override;

      ശൂന്യം alloc_instance(vvp_context_t context) override;
      ശൂന്യം reset_instance(vvp_context_t context) override;
#ifdef CHECK_WITH_VALGRIND
      ശൂന്യം free_instance(vvp_context_t context) override;
#endif

      ശൂന്യം recv_vec4(vvp_net_ptr_t port, സ്ഥിരം vvp_vector4_t&bit,
                     vvp_context_t context) override;

	// Part select variants of above
      ശൂന്യം recv_vec4_pv(vvp_net_ptr_t port, സ്ഥിരം vvp_vector4_t&bit,
			unsigned base, unsigned vwid, vvp_context_t) override;

	// Get information about the vector value.
      unsigned   മൂല്യം_size() സ്ഥിരം override;
      vvp_bit4_t മൂല്യം(unsigned idx) സ്ഥിരം override;
      vvp_scalar_t scalar_മൂല്യം(unsigned idx) സ്ഥിരം override;
      ശൂന്യം vec4_മൂല്യം(vvp_vector4_t&) സ്ഥിരം override;
      സ്ഥിരം vvp_vector4_t& vec4_unfiltered_മൂല്യം() സ്ഥിരം override;

    പൊതു: // These objects are only permallocated.
      സ്ഥിതം ശൂന്യം* operator new(std::size_t size) { തിരികെ vvp_net_fun_t::heap_.alloc(size); }
      സ്ഥിതം ശൂന്യം operator delete(ശൂന്യം*obj);

    സ്വകാര്യ:
      unsigned context_idx_;
      unsigned size_;
      vvp_bit4_t init_;
};

class vvp_fun_signal_real : പൊതു vvp_fun_signal_base {

    പൊതു:
      explicit vvp_fun_signal_real() {};

	// Get information about the vector value.
      മായാ ഇരട്ട real_unfiltered_മൂല്യം() സ്ഥിരം = 0;
};

/*
 * Statically allocated vvp_fun_signal_real.
 */
class vvp_fun_signal_real_sa : പൊതു vvp_fun_signal_real {

    പൊതു:
      explicit vvp_fun_signal_real_sa();

      ശൂന്യം recv_real(vvp_net_ptr_t port, ഇരട്ട bit,
                     vvp_context_t) override;

	// Get information about the vector value.
      ഇരട്ട real_unfiltered_മൂല്യം() സ്ഥിരം override;

    സ്വകാര്യ:
      ഇരട്ട bits_;
};

/*
 * Automatically allocated vvp_fun_signal_real.
 */
class vvp_fun_signal_real_aa : പൊതു vvp_fun_signal_real, പൊതു automatic_signal_base, പൊതു automatic_hooks_s {

    പൊതു:
      explicit vvp_fun_signal_real_aa();
      ~vvp_fun_signal_real_aa() override;

      ശൂന്യം alloc_instance(vvp_context_t context) override;
      ശൂന്യം reset_instance(vvp_context_t context) override;
#ifdef CHECK_WITH_VALGRIND
      ശൂന്യം free_instance(vvp_context_t context) override;
#endif

      ശൂന്യം recv_real(vvp_net_ptr_t port, ഇരട്ട bit,
                     vvp_context_t context) override;

	// Get information about the vector value.
      ഇരട്ട real_unfiltered_മൂല്യം() സ്ഥിരം override;

	// Get information about the vector value.
      unsigned   മൂല്യം_size() സ്ഥിരം override;
      vvp_bit4_t മൂല്യം(unsigned idx) സ്ഥിരം override;
      vvp_scalar_t scalar_മൂല്യം(unsigned idx) സ്ഥിരം override;
      ശൂന്യം vec4_മൂല്യം(vvp_vector4_t&) സ്ഥിരം override;
      ഇരട്ട real_മൂല്യം() സ്ഥിരം override;
      ശൂന്യം get_signal_മൂല്യം(ഘടന t_vpi_മൂല്യം*vp) override;

    പൊതു: // These objects are only permallocated.
      സ്ഥിതം ശൂന്യം* operator new(std::size_t size);
      സ്ഥിതം ശൂന്യം operator delete(ശൂന്യം*obj);

    സ്വകാര്യ:
      unsigned context_idx_;
};


class vvp_fun_signal_string : പൊതു vvp_fun_signal_base {

    പൊതു:
      explicit vvp_fun_signal_string() {};

      മായാ സ്ഥിരം std::string& get_string() സ്ഥിരം =0;
};

/*
 * Statically allocated vvp_fun_signal_string.
 */
class vvp_fun_signal_string_sa : പൊതു vvp_fun_signal_string {

    പൊതു:
      explicit vvp_fun_signal_string_sa();

      ശൂന്യം recv_string(vvp_net_ptr_t port, സ്ഥിരം std::string&bit,
		       vvp_context_t context) override;

      സ്ഥിരം std::string& get_string() സ്ഥിരം override;

    സ്വകാര്യ:
      std::string മൂല്യം_2;
};

/*
 * Automatically allocated vvp_fun_signal_real.
 */
class vvp_fun_signal_string_aa : പൊതു vvp_fun_signal_string, പൊതു automatic_signal_base, പൊതു automatic_hooks_s {

    പൊതു:
      explicit vvp_fun_signal_string_aa();
      ~vvp_fun_signal_string_aa() override;

      ശൂന്യം alloc_instance(vvp_context_t context) override;
      ശൂന്യം reset_instance(vvp_context_t context) override;
#ifdef CHECK_WITH_VALGRIND
      ശൂന്യം free_instance(vvp_context_t context) override;
#endif
      ശൂന്യം recv_string(vvp_net_ptr_t port, സ്ഥിരം std::string&bit,
		       vvp_context_t context) override;

	// Get information about the vector value.
      unsigned   മൂല്യം_size() സ്ഥിരം override;
      vvp_bit4_t മൂല്യം(unsigned idx) സ്ഥിരം override;
      vvp_scalar_t scalar_മൂല്യം(unsigned idx) സ്ഥിരം override;
      ശൂന്യം vec4_മൂല്യം(vvp_vector4_t&) സ്ഥിരം override;
      ഇരട്ട real_മൂല്യം() സ്ഥിരം override;
      സ്ഥിരം std::string& get_string() സ്ഥിരം override;
      ശൂന്യം get_signal_മൂല്യം(ഘടന t_vpi_മൂല്യം*vp) override;

    പൊതു: // These objects are only permallocated.
      സ്ഥിതം ശൂന്യം* operator new(std::size_t size);
      സ്ഥിതം ശൂന്യം operator delete(ശൂന്യം*obj);

    സ്വകാര്യ:
      unsigned context_idx_;
};

class vvp_fun_signal_object : പൊതു vvp_fun_signal_base {

    പൊതു:
      explicit vvp_fun_signal_object(unsigned size) { size_ = size; };
      unsigned size() സ്ഥിരം { തിരികെ size_; }

      മായാ vvp_object_t get_object() സ്ഥിരം =0;
    സ്വകാര്യ:
      unsigned size_;
};

/*
 * Statically allocated vvp_fun_signal_string.
 */
class vvp_fun_signal_object_sa : പൊതു vvp_fun_signal_object {

    പൊതു:
      explicit vvp_fun_signal_object_sa(unsigned size);

      ശൂന്യം recv_object(vvp_net_ptr_t port, vvp_object_t bit,
		    vvp_context_t context) override;

      vvp_object_t get_object() സ്ഥിരം override;

    സ്വകാര്യ:
      vvp_object_t മൂല്യം_2;
};

/*
 * Automatically allocated vvp_fun_signal_real.
 */
class vvp_fun_signal_object_aa : പൊതു vvp_fun_signal_object, പൊതു automatic_signal_base, പൊതു automatic_hooks_s {

    പൊതു:
      explicit vvp_fun_signal_object_aa(unsigned size);
      ~vvp_fun_signal_object_aa() override;

      ശൂന്യം alloc_instance(vvp_context_t context) override;
      ശൂന്യം reset_instance(vvp_context_t context) override;
#ifdef CHECK_WITH_VALGRIND
      ശൂന്യം free_instance(vvp_context_t context) override;
#endif

      ശൂന്യം recv_object(vvp_net_ptr_t port, vvp_object_t bit,
		    vvp_context_t context) override;

	// Get information about the vector value.
      unsigned   മൂല്യം_size() സ്ഥിരം override;
      vvp_bit4_t മൂല്യം(unsigned idx) സ്ഥിരം override;
      vvp_scalar_t scalar_മൂല്യം(unsigned idx) സ്ഥിരം override;
      ശൂന്യം vec4_മൂല്യം(vvp_vector4_t&) സ്ഥിരം override;
	//double real_value() const;
	//void get_signal_value(struct t_vpi_value*vp);

      vvp_object_t get_object() സ്ഥിരം override;

    പൊതു: // These objects are only permallocated.
      സ്ഥിതം ശൂന്യം* operator new(std::size_t size);
      സ്ഥിതം ശൂന്യം operator delete(ശൂന്യം*obj);

    സ്വകാര്യ:
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

class vvp_wire_base  : പൊതു vvp_net_fil_t, പൊതു vvp_signal_മൂല്യം {

    പൊതു:
      vvp_wire_base();
      ~vvp_wire_base() override;

        // Support for $countdrivers
      മായാ vvp_bit4_t driven_മൂല്യം(unsigned idx) സ്ഥിരം;
      മായാ bool is_forced(unsigned idx) സ്ഥിരം;
};

class vvp_wire_vec4 : പൊതു vvp_wire_base {

    പൊതു:
      vvp_wire_vec4(unsigned wid, vvp_bit4_t init);

	// The main filter behavior for this class. These methods take
	// the value that the node is driven to, and applies the force
	// filters. In wires, this also saves the driven value, so
	// that when a force is released, we can revert to the driven value.
      prop_t filter_vec4(സ്ഥിരം vvp_vector4_t&bit, vvp_vector4_t&rep,
			 unsigned base, unsigned vwid) override;
      prop_t filter_vec8(സ്ഥിരം vvp_vector8_t&val, vvp_vector8_t&rep,
			 unsigned base, unsigned vwid) override;

	// Abstract methods from vvp_vpi_callback
      ശൂന്യം get_മൂല്യം(ഘടന t_vpi_മൂല്യം*മൂല്യം) override;
	// Abstract methods from vvp_net_fit_t
      unsigned filter_size() സ്ഥിരം override;
      ശൂന്യം force_fil_vec4(സ്ഥിരം vvp_vector4_t&val, സ്ഥിരം vvp_vector2_t&mask) override;
      ശൂന്യം force_fil_vec8(സ്ഥിരം vvp_vector8_t&val, സ്ഥിരം vvp_vector2_t&mask) override;
      ശൂന്യം force_fil_real(ഇരട്ട val, സ്ഥിരം vvp_vector2_t&mask) override;
      ശൂന്യം release(vvp_net_ptr_t ptr, bool net_flag) override;
      ശൂന്യം release_pv(vvp_net_ptr_t ptr, unsigned base, unsigned wid, bool net_flag) override;

	// Implementation of vvp_signal_value methods
      unsigned മൂല്യം_size() സ്ഥിരം override;
      vvp_bit4_t മൂല്യം(unsigned idx) സ്ഥിരം override;
      vvp_scalar_t scalar_മൂല്യം(unsigned idx) സ്ഥിരം override;
      ശൂന്യം vec4_മൂല്യം(vvp_vector4_t&) സ്ഥിരം override;

        // Support for $countdrivers
      vvp_bit4_t driven_മൂല്യം(unsigned idx) സ്ഥിരം override;
      bool is_forced(unsigned idx) സ്ഥിരം override;

    സ്വകാര്യ:
      vvp_bit4_t filtered_മൂല്യം(unsigned idx) സ്ഥിരം;

    സ്വകാര്യ:
      bool needs_init_;
      vvp_vector4_t bits4_; // The tracked driven value
      vvp_vector4_t force4_; // the value being forced
};

class vvp_wire_vec8 : പൊതു vvp_wire_base {

    പൊതു:
      explicit vvp_wire_vec8(unsigned wid);

	// The main filter behavior for this class
      prop_t filter_vec4(സ്ഥിരം vvp_vector4_t&bit, vvp_vector4_t&rep,
			 unsigned base, unsigned vwid) override;
      prop_t filter_vec8(സ്ഥിരം vvp_vector8_t&val, vvp_vector8_t&rep,
			 unsigned base, unsigned vwid) override;

	// island ports use this method to filter arbitrary values
	// through the force filter.
      prop_t filter_നിവേശം_vec8(സ്ഥിരം vvp_vector8_t&val, vvp_vector8_t&rep) സ്ഥിരം;


	// Abstract methods from vvp_vpi_callback
      ശൂന്യം get_മൂല്യം(ഘടന t_vpi_മൂല്യം*മൂല്യം) override;
	// Abstract methods from vvp_net_fit_t
      unsigned filter_size() സ്ഥിരം override;
      ശൂന്യം force_fil_vec4(സ്ഥിരം vvp_vector4_t&val, സ്ഥിരം vvp_vector2_t&mask) override;
      ശൂന്യം force_fil_vec8(സ്ഥിരം vvp_vector8_t&val, സ്ഥിരം vvp_vector2_t&mask) override;
      ശൂന്യം force_fil_real(ഇരട്ട val, സ്ഥിരം vvp_vector2_t&mask) override;
      ശൂന്യം release(vvp_net_ptr_t ptr, bool net_flag) override;
      ശൂന്യം release_pv(vvp_net_ptr_t ptr, unsigned base, unsigned wid, bool net_flag) override;

	// Implementation of vvp_signal_value methods
      unsigned മൂല്യം_size() സ്ഥിരം override;
      vvp_bit4_t മൂല്യം(unsigned idx) സ്ഥിരം override;
      vvp_scalar_t scalar_മൂല്യം(unsigned idx) സ്ഥിരം override;
      ശൂന്യം vec4_മൂല്യം(vvp_vector4_t&) സ്ഥിരം override;
	// This is new to vvp_wire_vec8
      vvp_vector8_t vec8_മൂല്യം() സ്ഥിരം;

        // Support for $countdrivers
      vvp_bit4_t driven_മൂല്യം(unsigned idx) സ്ഥിരം override;
      bool is_forced(unsigned idx) സ്ഥിരം override;

    സ്വകാര്യ:
      vvp_scalar_t filtered_മൂല്യം(unsigned idx) സ്ഥിരം;

    സ്വകാര്യ:
      bool needs_init_;
      vvp_vector8_t bits8_;
      vvp_vector8_t force8_; // the value being forced
};

class vvp_wire_real : പൊതു vvp_wire_base {

    പൊതു:
      explicit vvp_wire_real(ശൂന്യം);

	// The main filter behavior for this class
      prop_t filter_real(ഇരട്ട&bit) override;

	// Abstract methods from vvp_vpi_callback
      ശൂന്യം get_മൂല്യം(ഘടന t_vpi_മൂല്യം*മൂല്യം) override;
	// Abstract methods from vvp_net_fit_t
      unsigned filter_size() സ്ഥിരം override;
      ശൂന്യം force_fil_vec4(സ്ഥിരം vvp_vector4_t&val, സ്ഥിരം vvp_vector2_t&mask) override;
      ശൂന്യം force_fil_vec8(സ്ഥിരം vvp_vector8_t&val, സ്ഥിരം vvp_vector2_t&mask) override;
      ശൂന്യം force_fil_real(ഇരട്ട val, സ്ഥിരം vvp_vector2_t&mask) override;
      ശൂന്യം release(vvp_net_ptr_t ptr, bool net_flag) override;
      ശൂന്യം release_pv(vvp_net_ptr_t ptr, unsigned base, unsigned wid, bool net_flag) override;

	// Implementation of vvp_signal_value methods
      unsigned മൂല്യം_size() സ്ഥിരം override;
      vvp_bit4_t മൂല്യം(unsigned idx) സ്ഥിരം override;
      vvp_scalar_t scalar_മൂല്യം(unsigned idx) സ്ഥിരം override;
      ശൂന്യം vec4_മൂല്യം(vvp_vector4_t&) സ്ഥിരം override;
      ഇരട്ട real_മൂല്യം() സ്ഥിരം override;

      ശൂന്യം get_signal_മൂല്യം(ഘടന t_vpi_മൂല്യം*vp) override;

    സ്വകാര്യ:
      ഇരട്ട bit_;
      ഇരട്ട force_;
      bool needs_init_ = true;
};

#എങ്കിൽ 0
class vvp_wire_string : പൊതു vvp_wire_base {

    പൊതു:
      explicit vvp_wire_string(ശൂന്യം);

	// Abstract methods from vvp_vpi_callback
      ശൂന്യം get_മൂല്യം(ഘടന t_vpi_മൂല്യം*മൂല്യം);
	// Abstract methods from vvp_net_fil_t
      unsigned filter_size() സ്ഥിരം;
      ശൂന്യം force_fil_vec4(സ്ഥിരം vvp_vector4_t&val, സ്ഥിരം vvp_vector2_t&mask);
      ശൂന്യം force_fil_vec8(സ്ഥിരം vvp_vector8_t&val, സ്ഥിരം vvp_vector2_t&mask);
      ശൂന്യം force_fil_real(ഇരട്ട val, സ്ഥിരം vvp_vector2_t&mask);
      ശൂന്യം release(vvp_net_ptr_t ptr, bool net_flag);
      ശൂന്യം release_pv(vvp_net_ptr_t ptr, unsigned base, unsigned wid, bool net_flag);

	// Implementation of vvp_signal_value methods
      unsigned മൂല്യം_size() സ്ഥിരം;
      vvp_bit4_t മൂല്യം(unsigned idx) സ്ഥിരം;
      vvp_scalar_t scalar_മൂല്യം(unsigned idx) സ്ഥിരം;
      ശൂന്യം vec4_മൂല്യം(vvp_vector4_t&) സ്ഥിരം;
      ഇരട്ട real_മൂല്യം() സ്ഥിരം;

      ശൂന്യം get_signal_മൂല്യം(ഘടന t_vpi_മൂല്യം*vp);

    സ്വകാര്യ:
      std::string മൂല്യം_2;
};
#endif

#endif /* IVL_vvp_net_sig_H */
