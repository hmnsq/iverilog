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
#નહીંતર
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


class vvp_fun_signal_base : જાહેર vvp_net_fun_t {

    જાહેર:
      vvp_fun_signal_base();

      શૂન્ય deassign();
      શૂન્ય deassign_pv(unsigned base, unsigned wid);

    જાહેર:

	/* The %cassign/link instruction needs a place to write the
	   source node of the force, so that subsequent %cassign and
	   %deassign instructions can undo the link as needed. */
      class vvp_net_t*cassign_link;

    સુરક્ષિત:
      bool continuous_assign_active_;
      vvp_vector2_t assign_mask_;

    સુરક્ષિત:
	// This is true until at least one propagation happens.
      bool needs_init_;
};

/*
 * Variables and wires can have their values accessed, so this base
 * class offers the unified concept of an accessible value.
 */
class vvp_signal_કિંમત {
    જાહેર:
      આભાસી ~vvp_signal_કિંમત() =0;
      આભાસી unsigned કિંમત_size() અચલ =0;
      આભાસી vvp_bit4_t કિંમત(unsigned idx) અચલ =0;
      આભાસી vvp_scalar_t scalar_કિંમત(unsigned idx) અચલ =0;
      આભાસી શૂન્ય vec4_કિંમત(vvp_vector4_t&) અચલ =0;
      આભાસી દ્વિગુણ real_કિંમત() અચલ;

      આભાસી શૂન્ય get_signal_કિંમત(સંરચના t_vpi_કિંમત*vp);
};

/*
 * This abstract class is a little more specific than the signal_base
 * class, in that it adds vector access methods.
 */
class vvp_fun_signal_vec : જાહેર vvp_fun_signal_base {
    જાહેર:
      આભાસી અચલ vvp_vector4_t& vec4_unfiltered_કિંમત() અચલ =0;
};

class automatic_signal_base : જાહેર vvp_signal_કિંમત, જાહેર vvp_net_fil_t {

    જાહેર:
	// Automatic variables cannot be forced or released. Provide
	// stubs that assert.
      આભાસી શૂન્ય release(vvp_net_ptr_t ptr, bool net_flag) override;
      આભાસી શૂન્ય release_pv(vvp_net_ptr_t ptr, unsigned base, unsigned wid, bool net_flag) override;

      આભાસી unsigned filter_size() અચલ override;
      આભાસી શૂન્ય force_fil_vec4(અચલ vvp_vector4_t&val, અચલ vvp_vector2_t&mask) override;
      આભાસી શૂન્ય force_fil_vec8(અચલ vvp_vector8_t&val, અચલ vvp_vector2_t&mask) override;
      આભાસી શૂન્ય force_fil_real(દ્વિગુણ val, અચલ vvp_vector2_t&mask) override;
      આભાસી શૂન્ય get_કિંમત(સંરચના t_vpi_કિંમત*કિંમત) override;
};

/*
 * Statically allocated vvp_fun_signal4.
 */
class vvp_fun_signal4_sa : જાહેર vvp_fun_signal_vec {

    જાહેર:
      explicit vvp_fun_signal4_sa(unsigned wid, vvp_bit4_t init=BIT4_X);

      શૂન્ય recv_vec4(vvp_net_ptr_t port, અચલ vvp_vector4_t&bit,
                     vvp_context_t) override;
      શૂન્ય recv_vec8(vvp_net_ptr_t port, અચલ vvp_vector8_t&bit) override;

	// Part select variants of above
      શૂન્ય recv_vec4_pv(vvp_net_ptr_t port, અચલ vvp_vector4_t&bit,
			unsigned base, unsigned vwid, vvp_context_t) override;
      શૂન્ય recv_vec8_pv(vvp_net_ptr_t port, અચલ vvp_vector8_t&bit,
			unsigned base, unsigned vwid) override;

	// Get information about the vector value.
      અચલ vvp_vector4_t& vec4_unfiltered_કિંમત() અચલ override;

    ખાનગી:
      vvp_vector4_t bits4_;
};

/*
 * Automatically allocated vvp_fun_signal4.
 */
class vvp_fun_signal4_aa : જાહેર vvp_fun_signal_vec, જાહેર automatic_signal_base, જાહેર automatic_hooks_s {

    જાહેર:
      explicit vvp_fun_signal4_aa(unsigned wid, vvp_bit4_t init=BIT4_X);
      ~vvp_fun_signal4_aa() override;

      શૂન્ય alloc_instance(vvp_context_t context) override;
      શૂન્ય reset_instance(vvp_context_t context) override;
#ifdef CHECK_WITH_VALGRIND
      શૂન્ય free_instance(vvp_context_t context) override;
#endif

      શૂન્ય recv_vec4(vvp_net_ptr_t port, અચલ vvp_vector4_t&bit,
                     vvp_context_t context) override;

	// Part select variants of above
      શૂન્ય recv_vec4_pv(vvp_net_ptr_t port, અચલ vvp_vector4_t&bit,
			unsigned base, unsigned vwid, vvp_context_t) override;

	// Get information about the vector value.
      unsigned   કિંમત_size() અચલ override;
      vvp_bit4_t કિંમત(unsigned idx) અચલ override;
      vvp_scalar_t scalar_કિંમત(unsigned idx) અચલ override;
      શૂન્ય vec4_કિંમત(vvp_vector4_t&) અચલ override;
      અચલ vvp_vector4_t& vec4_unfiltered_કિંમત() અચલ override;

    જાહેર: // These objects are only permallocated.
      સ્થિર શૂન્ય* operator new(std::size_t size) { પાછા vvp_net_fun_t::heap_.alloc(size); }
      સ્થિર શૂન્ય operator delete(શૂન્ય*obj);

    ખાનગી:
      unsigned context_idx_;
      unsigned size_;
      vvp_bit4_t init_;
};

class vvp_fun_signal_real : જાહેર vvp_fun_signal_base {

    જાહેર:
      explicit vvp_fun_signal_real() {};

	// Get information about the vector value.
      આભાસી દ્વિગુણ real_unfiltered_કિંમત() અચલ = 0;
};

/*
 * Statically allocated vvp_fun_signal_real.
 */
class vvp_fun_signal_real_sa : જાહેર vvp_fun_signal_real {

    જાહેર:
      explicit vvp_fun_signal_real_sa();

      શૂન્ય recv_real(vvp_net_ptr_t port, દ્વિગુણ bit,
                     vvp_context_t) override;

	// Get information about the vector value.
      દ્વિગુણ real_unfiltered_કિંમત() અચલ override;

    ખાનગી:
      દ્વિગુણ bits_;
};

/*
 * Automatically allocated vvp_fun_signal_real.
 */
class vvp_fun_signal_real_aa : જાહેર vvp_fun_signal_real, જાહેર automatic_signal_base, જાહેર automatic_hooks_s {

    જાહેર:
      explicit vvp_fun_signal_real_aa();
      ~vvp_fun_signal_real_aa() override;

      શૂન્ય alloc_instance(vvp_context_t context) override;
      શૂન્ય reset_instance(vvp_context_t context) override;
#ifdef CHECK_WITH_VALGRIND
      શૂન્ય free_instance(vvp_context_t context) override;
#endif

      શૂન્ય recv_real(vvp_net_ptr_t port, દ્વિગુણ bit,
                     vvp_context_t context) override;

	// Get information about the vector value.
      દ્વિગુણ real_unfiltered_કિંમત() અચલ override;

	// Get information about the vector value.
      unsigned   કિંમત_size() અચલ override;
      vvp_bit4_t કિંમત(unsigned idx) અચલ override;
      vvp_scalar_t scalar_કિંમત(unsigned idx) અચલ override;
      શૂન્ય vec4_કિંમત(vvp_vector4_t&) અચલ override;
      દ્વિગુણ real_કિંમત() અચલ override;
      શૂન્ય get_signal_કિંમત(સંરચના t_vpi_કિંમત*vp) override;

    જાહેર: // These objects are only permallocated.
      સ્થિર શૂન્ય* operator new(std::size_t size);
      સ્થિર શૂન્ય operator delete(શૂન્ય*obj);

    ખાનગી:
      unsigned context_idx_;
};


class vvp_fun_signal_string : જાહેર vvp_fun_signal_base {

    જાહેર:
      explicit vvp_fun_signal_string() {};

      આભાસી અચલ std::string& get_string() અચલ =0;
};

/*
 * Statically allocated vvp_fun_signal_string.
 */
class vvp_fun_signal_string_sa : જાહેર vvp_fun_signal_string {

    જાહેર:
      explicit vvp_fun_signal_string_sa();

      શૂન્ય recv_string(vvp_net_ptr_t port, અચલ std::string&bit,
		       vvp_context_t context) override;

      અચલ std::string& get_string() અચલ override;

    ખાનગી:
      std::string કિંમત_2;
};

/*
 * Automatically allocated vvp_fun_signal_real.
 */
class vvp_fun_signal_string_aa : જાહેર vvp_fun_signal_string, જાહેર automatic_signal_base, જાહેર automatic_hooks_s {

    જાહેર:
      explicit vvp_fun_signal_string_aa();
      ~vvp_fun_signal_string_aa() override;

      શૂન્ય alloc_instance(vvp_context_t context) override;
      શૂન્ય reset_instance(vvp_context_t context) override;
#ifdef CHECK_WITH_VALGRIND
      શૂન્ય free_instance(vvp_context_t context) override;
#endif
      શૂન્ય recv_string(vvp_net_ptr_t port, અચલ std::string&bit,
		       vvp_context_t context) override;

	// Get information about the vector value.
      unsigned   કિંમત_size() અચલ override;
      vvp_bit4_t કિંમત(unsigned idx) અચલ override;
      vvp_scalar_t scalar_કિંમત(unsigned idx) અચલ override;
      શૂન્ય vec4_કિંમત(vvp_vector4_t&) અચલ override;
      દ્વિગુણ real_કિંમત() અચલ override;
      અચલ std::string& get_string() અચલ override;
      શૂન્ય get_signal_કિંમત(સંરચના t_vpi_કિંમત*vp) override;

    જાહેર: // These objects are only permallocated.
      સ્થિર શૂન્ય* operator new(std::size_t size);
      સ્થિર શૂન્ય operator delete(શૂન્ય*obj);

    ખાનગી:
      unsigned context_idx_;
};

class vvp_fun_signal_object : જાહેર vvp_fun_signal_base {

    જાહેર:
      explicit vvp_fun_signal_object(unsigned size) { size_ = size; };
      unsigned size() અચલ { પાછા size_; }

      આભાસી vvp_object_t get_object() અચલ =0;
    ખાનગી:
      unsigned size_;
};

/*
 * Statically allocated vvp_fun_signal_string.
 */
class vvp_fun_signal_object_sa : જાહેર vvp_fun_signal_object {

    જાહેર:
      explicit vvp_fun_signal_object_sa(unsigned size);

      શૂન્ય recv_object(vvp_net_ptr_t port, vvp_object_t bit,
		    vvp_context_t context) override;

      vvp_object_t get_object() અચલ override;

    ખાનગી:
      vvp_object_t કિંમત_2;
};

/*
 * Automatically allocated vvp_fun_signal_real.
 */
class vvp_fun_signal_object_aa : જાહેર vvp_fun_signal_object, જાહેર automatic_signal_base, જાહેર automatic_hooks_s {

    જાહેર:
      explicit vvp_fun_signal_object_aa(unsigned size);
      ~vvp_fun_signal_object_aa() override;

      શૂન્ય alloc_instance(vvp_context_t context) override;
      શૂન્ય reset_instance(vvp_context_t context) override;
#ifdef CHECK_WITH_VALGRIND
      શૂન્ય free_instance(vvp_context_t context) override;
#endif

      શૂન્ય recv_object(vvp_net_ptr_t port, vvp_object_t bit,
		    vvp_context_t context) override;

	// Get information about the vector value.
      unsigned   કિંમત_size() અચલ override;
      vvp_bit4_t કિંમત(unsigned idx) અચલ override;
      vvp_scalar_t scalar_કિંમત(unsigned idx) અચલ override;
      શૂન્ય vec4_કિંમત(vvp_vector4_t&) અચલ override;
	//double real_value() const;
	//void get_signal_value(struct t_vpi_value*vp);

      vvp_object_t get_object() અચલ override;

    જાહેર: // These objects are only permallocated.
      સ્થિર શૂન્ય* operator new(std::size_t size);
      સ્થિર શૂન્ય operator delete(શૂન્ય*obj);

    ખાનગી:
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

class vvp_wire_base  : જાહેર vvp_net_fil_t, જાહેર vvp_signal_કિંમત {

    જાહેર:
      vvp_wire_base();
      ~vvp_wire_base() override;

        // Support for $countdrivers
      આભાસી vvp_bit4_t driven_કિંમત(unsigned idx) અચલ;
      આભાસી bool is_forced(unsigned idx) અચલ;
};

class vvp_wire_vec4 : જાહેર vvp_wire_base {

    જાહેર:
      vvp_wire_vec4(unsigned wid, vvp_bit4_t init);

	// The main filter behavior for this class. These methods take
	// the value that the node is driven to, and applies the force
	// filters. In wires, this also saves the driven value, so
	// that when a force is released, we can revert to the driven value.
      prop_t filter_vec4(અચલ vvp_vector4_t&bit, vvp_vector4_t&rep,
			 unsigned base, unsigned vwid) override;
      prop_t filter_vec8(અચલ vvp_vector8_t&val, vvp_vector8_t&rep,
			 unsigned base, unsigned vwid) override;

	// Abstract methods from vvp_vpi_callback
      શૂન્ય get_કિંમત(સંરચના t_vpi_કિંમત*કિંમત) override;
	// Abstract methods from vvp_net_fit_t
      unsigned filter_size() અચલ override;
      શૂન્ય force_fil_vec4(અચલ vvp_vector4_t&val, અચલ vvp_vector2_t&mask) override;
      શૂન્ય force_fil_vec8(અચલ vvp_vector8_t&val, અચલ vvp_vector2_t&mask) override;
      શૂન્ય force_fil_real(દ્વિગુણ val, અચલ vvp_vector2_t&mask) override;
      શૂન્ય release(vvp_net_ptr_t ptr, bool net_flag) override;
      શૂન્ય release_pv(vvp_net_ptr_t ptr, unsigned base, unsigned wid, bool net_flag) override;

	// Implementation of vvp_signal_value methods
      unsigned કિંમત_size() અચલ override;
      vvp_bit4_t કિંમત(unsigned idx) અચલ override;
      vvp_scalar_t scalar_કિંમત(unsigned idx) અચલ override;
      શૂન્ય vec4_કિંમત(vvp_vector4_t&) અચલ override;

        // Support for $countdrivers
      vvp_bit4_t driven_કિંમત(unsigned idx) અચલ override;
      bool is_forced(unsigned idx) અચલ override;

    ખાનગી:
      vvp_bit4_t filtered_કિંમત(unsigned idx) અચલ;

    ખાનગી:
      bool needs_init_;
      vvp_vector4_t bits4_; // The tracked driven value
      vvp_vector4_t force4_; // the value being forced
};

class vvp_wire_vec8 : જાહેર vvp_wire_base {

    જાહેર:
      explicit vvp_wire_vec8(unsigned wid);

	// The main filter behavior for this class
      prop_t filter_vec4(અચલ vvp_vector4_t&bit, vvp_vector4_t&rep,
			 unsigned base, unsigned vwid) override;
      prop_t filter_vec8(અચલ vvp_vector8_t&val, vvp_vector8_t&rep,
			 unsigned base, unsigned vwid) override;

	// island ports use this method to filter arbitrary values
	// through the force filter.
      prop_t filter_આગત_vec8(અચલ vvp_vector8_t&val, vvp_vector8_t&rep) અચલ;


	// Abstract methods from vvp_vpi_callback
      શૂન્ય get_કિંમત(સંરચના t_vpi_કિંમત*કિંમત) override;
	// Abstract methods from vvp_net_fit_t
      unsigned filter_size() અચલ override;
      શૂન્ય force_fil_vec4(અચલ vvp_vector4_t&val, અચલ vvp_vector2_t&mask) override;
      શૂન્ય force_fil_vec8(અચલ vvp_vector8_t&val, અચલ vvp_vector2_t&mask) override;
      શૂન્ય force_fil_real(દ્વિગુણ val, અચલ vvp_vector2_t&mask) override;
      શૂન્ય release(vvp_net_ptr_t ptr, bool net_flag) override;
      શૂન્ય release_pv(vvp_net_ptr_t ptr, unsigned base, unsigned wid, bool net_flag) override;

	// Implementation of vvp_signal_value methods
      unsigned કિંમત_size() અચલ override;
      vvp_bit4_t કિંમત(unsigned idx) અચલ override;
      vvp_scalar_t scalar_કિંમત(unsigned idx) અચલ override;
      શૂન્ય vec4_કિંમત(vvp_vector4_t&) અચલ override;
	// This is new to vvp_wire_vec8
      vvp_vector8_t vec8_કિંમત() અચલ;

        // Support for $countdrivers
      vvp_bit4_t driven_કિંમત(unsigned idx) અચલ override;
      bool is_forced(unsigned idx) અચલ override;

    ખાનગી:
      vvp_scalar_t filtered_કિંમત(unsigned idx) અચલ;

    ખાનગી:
      bool needs_init_;
      vvp_vector8_t bits8_;
      vvp_vector8_t force8_; // the value being forced
};

class vvp_wire_real : જાહેર vvp_wire_base {

    જાહેર:
      explicit vvp_wire_real(શૂન્ય);

	// The main filter behavior for this class
      prop_t filter_real(દ્વિગુણ&bit) override;

	// Abstract methods from vvp_vpi_callback
      શૂન્ય get_કિંમત(સંરચના t_vpi_કિંમત*કિંમત) override;
	// Abstract methods from vvp_net_fit_t
      unsigned filter_size() અચલ override;
      શૂન્ય force_fil_vec4(અચલ vvp_vector4_t&val, અચલ vvp_vector2_t&mask) override;
      શૂન્ય force_fil_vec8(અચલ vvp_vector8_t&val, અચલ vvp_vector2_t&mask) override;
      શૂન્ય force_fil_real(દ્વિગુણ val, અચલ vvp_vector2_t&mask) override;
      શૂન્ય release(vvp_net_ptr_t ptr, bool net_flag) override;
      શૂન્ય release_pv(vvp_net_ptr_t ptr, unsigned base, unsigned wid, bool net_flag) override;

	// Implementation of vvp_signal_value methods
      unsigned કિંમત_size() અચલ override;
      vvp_bit4_t કિંમત(unsigned idx) અચલ override;
      vvp_scalar_t scalar_કિંમત(unsigned idx) અચલ override;
      શૂન્ય vec4_કિંમત(vvp_vector4_t&) અચલ override;
      દ્વિગુણ real_કિંમત() અચલ override;

      શૂન્ય get_signal_કિંમત(સંરચના t_vpi_કિંમત*vp) override;

    ખાનગી:
      દ્વિગુણ bit_;
      દ્વિગુણ force_;
      bool needs_init_ = true;
};

#જો 0
class vvp_wire_string : જાહેર vvp_wire_base {

    જાહેર:
      explicit vvp_wire_string(શૂન્ય);

	// Abstract methods from vvp_vpi_callback
      શૂન્ય get_કિંમત(સંરચના t_vpi_કિંમત*કિંમત);
	// Abstract methods from vvp_net_fil_t
      unsigned filter_size() અચલ;
      શૂન્ય force_fil_vec4(અચલ vvp_vector4_t&val, અચલ vvp_vector2_t&mask);
      શૂન્ય force_fil_vec8(અચલ vvp_vector8_t&val, અચલ vvp_vector2_t&mask);
      શૂન્ય force_fil_real(દ્વિગુણ val, અચલ vvp_vector2_t&mask);
      શૂન્ય release(vvp_net_ptr_t ptr, bool net_flag);
      શૂન્ય release_pv(vvp_net_ptr_t ptr, unsigned base, unsigned wid, bool net_flag);

	// Implementation of vvp_signal_value methods
      unsigned કિંમત_size() અચલ;
      vvp_bit4_t કિંમત(unsigned idx) અચલ;
      vvp_scalar_t scalar_કિંમત(unsigned idx) અચલ;
      શૂન્ય vec4_કિંમત(vvp_vector4_t&) અચલ;
      દ્વિગુણ real_કિંમત() અચલ;

      શૂન્ય get_signal_કિંમત(સંરચના t_vpi_કિંમત*vp);

    ખાનગી:
      std::string કિંમત_2;
};
#endif

#endif /* IVL_vvp_net_sig_H */
