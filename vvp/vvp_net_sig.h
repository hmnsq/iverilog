#अगर_अघोषित IVL_vvp_net_sig_H
#घोषणा IVL_vvp_net_sig_H
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

#अगर_घोषित HAVE_IOSFWD
# include  <iosfwd>
#अन्यथा
# include  <iostream>
#पूर्ण_अगर

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


class vvp_fun_signal_base : खुला vvp_net_fun_t {

    खुला:
      vvp_fun_signal_base();

      व्योम deassign();
      व्योम deassign_pv(अचिन्हित base, अचिन्हित wid);

    खुला:

	/* The %cassign/link instruction needs a place to write the
	   source node of the force, so that subsequent %cassign and
	   %deassign instructions can undo the link as needed. */
      class vvp_net_t*cassign_link;

    रक्षित:
      bool continuous_assign_active_;
      vvp_vector2_t assign_mask_;

    रक्षित:
	// This is true until at least one propagation happens.
      bool needs_init_;
};

/*
 * Variables and wires can have their values accessed, so this base
 * class offers the unified concept of an accessible value.
 */
class vvp_signal_मूल्य {
    खुला:
      भव ~vvp_signal_मूल्य() =0;
      भव अचिन्हित मूल्य_आकार() स्थिर =0;
      भव vvp_bit4_t मूल्य(अचिन्हित idx) स्थिर =0;
      भव vvp_scalar_t scalar_मूल्य(अचिन्हित idx) स्थिर =0;
      भव व्योम vec4_मूल्य(vvp_vector4_t&) स्थिर =0;
      भव द्विगुन real_मूल्य() स्थिर;

      भव व्योम get_signal_मूल्य(काष्ठा t_vpi_मूल्य*vp);
};

/*
 * This abstract class is a little more specific than the signal_base
 * class, in that it adds vector access methods.
 */
class vvp_fun_signal_vec : खुला vvp_fun_signal_base {
    खुला:
      भव स्थिर vvp_vector4_t& vec4_unfiltered_मूल्य() स्थिर =0;
};

class automatic_signal_base : खुला vvp_signal_मूल्य, खुला vvp_net_fil_t {

    खुला:
	// Automatic variables cannot be forced or released. Provide
	// stubs that assert.
      भव व्योम release(vvp_net_ptr_t ptr, bool net_flag) override;
      भव व्योम release_pv(vvp_net_ptr_t ptr, अचिन्हित base, अचिन्हित wid, bool net_flag) override;

      भव अचिन्हित filter_आकार() स्थिर override;
      भव व्योम force_fil_vec4(स्थिर vvp_vector4_t&val, स्थिर vvp_vector2_t&mask) override;
      भव व्योम force_fil_vec8(स्थिर vvp_vector8_t&val, स्थिर vvp_vector2_t&mask) override;
      भव व्योम force_fil_real(द्विगुन val, स्थिर vvp_vector2_t&mask) override;
      भव व्योम get_मूल्य(काष्ठा t_vpi_मूल्य*मूल्य) override;
};

/*
 * Statically allocated vvp_fun_signal4.
 */
class vvp_fun_signal4_sa : खुला vvp_fun_signal_vec {

    खुला:
      explicit vvp_fun_signal4_sa(अचिन्हित wid, vvp_bit4_t init=BIT4_X);

      व्योम recv_vec4(vvp_net_ptr_t port, स्थिर vvp_vector4_t&bit,
                     vvp_context_t) override;
      व्योम recv_vec8(vvp_net_ptr_t port, स्थिर vvp_vector8_t&bit) override;

	// Part select variants of above
      व्योम recv_vec4_pv(vvp_net_ptr_t port, स्थिर vvp_vector4_t&bit,
			अचिन्हित base, अचिन्हित vwid, vvp_context_t) override;
      व्योम recv_vec8_pv(vvp_net_ptr_t port, स्थिर vvp_vector8_t&bit,
			अचिन्हित base, अचिन्हित vwid) override;

	// Get information about the vector value.
      स्थिर vvp_vector4_t& vec4_unfiltered_मूल्य() स्थिर override;

    निजी:
      vvp_vector4_t bits4_;
};

/*
 * Automatically allocated vvp_fun_signal4.
 */
class vvp_fun_signal4_aa : खुला vvp_fun_signal_vec, खुला automatic_signal_base, खुला automatic_hooks_s {

    खुला:
      explicit vvp_fun_signal4_aa(अचिन्हित wid, vvp_bit4_t init=BIT4_X);
      ~vvp_fun_signal4_aa() override;

      व्योम alloc_instance(vvp_context_t context) override;
      व्योम reset_instance(vvp_context_t context) override;
#अगर_घोषित CHECK_WITH_VALGRIND
      व्योम free_instance(vvp_context_t context) override;
#पूर्ण_अगर

      व्योम recv_vec4(vvp_net_ptr_t port, स्थिर vvp_vector4_t&bit,
                     vvp_context_t context) override;

	// Part select variants of above
      व्योम recv_vec4_pv(vvp_net_ptr_t port, स्थिर vvp_vector4_t&bit,
			अचिन्हित base, अचिन्हित vwid, vvp_context_t) override;

	// Get information about the vector value.
      अचिन्हित   मूल्य_आकार() स्थिर override;
      vvp_bit4_t मूल्य(अचिन्हित idx) स्थिर override;
      vvp_scalar_t scalar_मूल्य(अचिन्हित idx) स्थिर override;
      व्योम vec4_मूल्य(vvp_vector4_t&) स्थिर override;
      स्थिर vvp_vector4_t& vec4_unfiltered_मूल्य() स्थिर override;

    खुला: // These objects are only permallocated.
      अटल व्योम* चालक new(std::माप_प्रकार आकार) { वापस vvp_net_fun_t::heap_.alloc(आकार); }
      अटल व्योम चालक delete(व्योम*obj);

    निजी:
      अचिन्हित context_idx_;
      अचिन्हित आकार_2;
      vvp_bit4_t init_;
};

class vvp_fun_signal_real : खुला vvp_fun_signal_base {

    खुला:
      explicit vvp_fun_signal_real() {};

	// Get information about the vector value.
      भव द्विगुन real_unfiltered_मूल्य() स्थिर = 0;
};

/*
 * Statically allocated vvp_fun_signal_real.
 */
class vvp_fun_signal_real_sa : खुला vvp_fun_signal_real {

    खुला:
      explicit vvp_fun_signal_real_sa();

      व्योम recv_real(vvp_net_ptr_t port, द्विगुन bit,
                     vvp_context_t) override;

	// Get information about the vector value.
      द्विगुन real_unfiltered_मूल्य() स्थिर override;

    निजी:
      द्विगुन bits_;
};

/*
 * Automatically allocated vvp_fun_signal_real.
 */
class vvp_fun_signal_real_aa : खुला vvp_fun_signal_real, खुला automatic_signal_base, खुला automatic_hooks_s {

    खुला:
      explicit vvp_fun_signal_real_aa();
      ~vvp_fun_signal_real_aa() override;

      व्योम alloc_instance(vvp_context_t context) override;
      व्योम reset_instance(vvp_context_t context) override;
#अगर_घोषित CHECK_WITH_VALGRIND
      व्योम free_instance(vvp_context_t context) override;
#पूर्ण_अगर

      व्योम recv_real(vvp_net_ptr_t port, द्विगुन bit,
                     vvp_context_t context) override;

	// Get information about the vector value.
      द्विगुन real_unfiltered_मूल्य() स्थिर override;

	// Get information about the vector value.
      अचिन्हित   मूल्य_आकार() स्थिर override;
      vvp_bit4_t मूल्य(अचिन्हित idx) स्थिर override;
      vvp_scalar_t scalar_मूल्य(अचिन्हित idx) स्थिर override;
      व्योम vec4_मूल्य(vvp_vector4_t&) स्थिर override;
      द्विगुन real_मूल्य() स्थिर override;
      व्योम get_signal_मूल्य(काष्ठा t_vpi_मूल्य*vp) override;

    खुला: // These objects are only permallocated.
      अटल व्योम* चालक new(std::माप_प्रकार आकार);
      अटल व्योम चालक delete(व्योम*obj);

    निजी:
      अचिन्हित context_idx_;
};


class vvp_fun_signal_string : खुला vvp_fun_signal_base {

    खुला:
      explicit vvp_fun_signal_string() {};

      भव स्थिर std::string& get_string() स्थिर =0;
};

/*
 * Statically allocated vvp_fun_signal_string.
 */
class vvp_fun_signal_string_sa : खुला vvp_fun_signal_string {

    खुला:
      explicit vvp_fun_signal_string_sa();

      व्योम recv_string(vvp_net_ptr_t port, स्थिर std::string&bit,
		       vvp_context_t context) override;

      स्थिर std::string& get_string() स्थिर override;

    निजी:
      std::string मूल्य_2;
};

/*
 * Automatically allocated vvp_fun_signal_real.
 */
class vvp_fun_signal_string_aa : खुला vvp_fun_signal_string, खुला automatic_signal_base, खुला automatic_hooks_s {

    खुला:
      explicit vvp_fun_signal_string_aa();
      ~vvp_fun_signal_string_aa() override;

      व्योम alloc_instance(vvp_context_t context) override;
      व्योम reset_instance(vvp_context_t context) override;
#अगर_घोषित CHECK_WITH_VALGRIND
      व्योम free_instance(vvp_context_t context) override;
#पूर्ण_अगर
      व्योम recv_string(vvp_net_ptr_t port, स्थिर std::string&bit,
		       vvp_context_t context) override;

	// Get information about the vector value.
      अचिन्हित   मूल्य_आकार() स्थिर override;
      vvp_bit4_t मूल्य(अचिन्हित idx) स्थिर override;
      vvp_scalar_t scalar_मूल्य(अचिन्हित idx) स्थिर override;
      व्योम vec4_मूल्य(vvp_vector4_t&) स्थिर override;
      द्विगुन real_मूल्य() स्थिर override;
      स्थिर std::string& get_string() स्थिर override;
      व्योम get_signal_मूल्य(काष्ठा t_vpi_मूल्य*vp) override;

    खुला: // These objects are only permallocated.
      अटल व्योम* चालक new(std::माप_प्रकार आकार);
      अटल व्योम चालक delete(व्योम*obj);

    निजी:
      अचिन्हित context_idx_;
};

class vvp_fun_signal_object : खुला vvp_fun_signal_base {

    खुला:
      explicit vvp_fun_signal_object(अचिन्हित आकार) { आकार_2 = आकार; };
      अचिन्हित आकार() स्थिर { वापस आकार_2; }

      भव vvp_object_t get_object() स्थिर =0;
    निजी:
      अचिन्हित आकार_2;
};

/*
 * Statically allocated vvp_fun_signal_string.
 */
class vvp_fun_signal_object_sa : खुला vvp_fun_signal_object {

    खुला:
      explicit vvp_fun_signal_object_sa(अचिन्हित आकार);

      व्योम recv_object(vvp_net_ptr_t port, vvp_object_t bit,
		    vvp_context_t context) override;

      vvp_object_t get_object() स्थिर override;

    निजी:
      vvp_object_t मूल्य_2;
};

/*
 * Automatically allocated vvp_fun_signal_real.
 */
class vvp_fun_signal_object_aa : खुला vvp_fun_signal_object, खुला automatic_signal_base, खुला automatic_hooks_s {

    खुला:
      explicit vvp_fun_signal_object_aa(अचिन्हित आकार);
      ~vvp_fun_signal_object_aa() override;

      व्योम alloc_instance(vvp_context_t context) override;
      व्योम reset_instance(vvp_context_t context) override;
#अगर_घोषित CHECK_WITH_VALGRIND
      व्योम free_instance(vvp_context_t context) override;
#पूर्ण_अगर

      व्योम recv_object(vvp_net_ptr_t port, vvp_object_t bit,
		    vvp_context_t context) override;

	// Get information about the vector value.
      अचिन्हित   मूल्य_आकार() स्थिर override;
      vvp_bit4_t मूल्य(अचिन्हित idx) स्थिर override;
      vvp_scalar_t scalar_मूल्य(अचिन्हित idx) स्थिर override;
      व्योम vec4_मूल्य(vvp_vector4_t&) स्थिर override;
	//double real_value() const;
	//void get_signal_value(struct t_vpi_value*vp);

      vvp_object_t get_object() स्थिर override;

    खुला: // These objects are only permallocated.
      अटल व्योम* चालक new(std::माप_प्रकार आकार);
      अटल व्योम चालक delete(व्योम*obj);

    निजी:
      अचिन्हित context_idx_;
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

class vvp_wire_base  : खुला vvp_net_fil_t, खुला vvp_signal_मूल्य {

    खुला:
      vvp_wire_base();
      ~vvp_wire_base() override;

        // Support for $countdrivers
      भव vvp_bit4_t driven_मूल्य(अचिन्हित idx) स्थिर;
      भव bool is_forced(अचिन्हित idx) स्थिर;
};

class vvp_wire_vec4 : खुला vvp_wire_base {

    खुला:
      vvp_wire_vec4(अचिन्हित wid, vvp_bit4_t init);

	// The main filter behavior for this class. These methods take
	// the value that the node is driven to, and applies the force
	// filters. In wires, this also saves the driven value, so
	// that when a force is released, we can revert to the driven value.
      prop_t filter_vec4(स्थिर vvp_vector4_t&bit, vvp_vector4_t&rep,
			 अचिन्हित base, अचिन्हित vwid) override;
      prop_t filter_vec8(स्थिर vvp_vector8_t&val, vvp_vector8_t&rep,
			 अचिन्हित base, अचिन्हित vwid) override;

	// Abstract methods from vvp_vpi_callback
      व्योम get_मूल्य(काष्ठा t_vpi_मूल्य*मूल्य) override;
	// Abstract methods from vvp_net_fit_t
      अचिन्हित filter_आकार() स्थिर override;
      व्योम force_fil_vec4(स्थिर vvp_vector4_t&val, स्थिर vvp_vector2_t&mask) override;
      व्योम force_fil_vec8(स्थिर vvp_vector8_t&val, स्थिर vvp_vector2_t&mask) override;
      व्योम force_fil_real(द्विगुन val, स्थिर vvp_vector2_t&mask) override;
      व्योम release(vvp_net_ptr_t ptr, bool net_flag) override;
      व्योम release_pv(vvp_net_ptr_t ptr, अचिन्हित base, अचिन्हित wid, bool net_flag) override;

	// Implementation of vvp_signal_value methods
      अचिन्हित मूल्य_आकार() स्थिर override;
      vvp_bit4_t मूल्य(अचिन्हित idx) स्थिर override;
      vvp_scalar_t scalar_मूल्य(अचिन्हित idx) स्थिर override;
      व्योम vec4_मूल्य(vvp_vector4_t&) स्थिर override;

        // Support for $countdrivers
      vvp_bit4_t driven_मूल्य(अचिन्हित idx) स्थिर override;
      bool is_forced(अचिन्हित idx) स्थिर override;

    निजी:
      vvp_bit4_t filtered_मूल्य(अचिन्हित idx) स्थिर;

    निजी:
      bool needs_init_;
      vvp_vector4_t bits4_; // The tracked driven value
      vvp_vector4_t force4_; // the value being forced
};

class vvp_wire_vec8 : खुला vvp_wire_base {

    खुला:
      explicit vvp_wire_vec8(अचिन्हित wid);

	// The main filter behavior for this class
      prop_t filter_vec4(स्थिर vvp_vector4_t&bit, vvp_vector4_t&rep,
			 अचिन्हित base, अचिन्हित vwid) override;
      prop_t filter_vec8(स्थिर vvp_vector8_t&val, vvp_vector8_t&rep,
			 अचिन्हित base, अचिन्हित vwid) override;

	// island ports use this method to filter arbitrary values
	// through the force filter.
      prop_t filter_निवेश_vec8(स्थिर vvp_vector8_t&val, vvp_vector8_t&rep) स्थिर;


	// Abstract methods from vvp_vpi_callback
      व्योम get_मूल्य(काष्ठा t_vpi_मूल्य*मूल्य) override;
	// Abstract methods from vvp_net_fit_t
      अचिन्हित filter_आकार() स्थिर override;
      व्योम force_fil_vec4(स्थिर vvp_vector4_t&val, स्थिर vvp_vector2_t&mask) override;
      व्योम force_fil_vec8(स्थिर vvp_vector8_t&val, स्थिर vvp_vector2_t&mask) override;
      व्योम force_fil_real(द्विगुन val, स्थिर vvp_vector2_t&mask) override;
      व्योम release(vvp_net_ptr_t ptr, bool net_flag) override;
      व्योम release_pv(vvp_net_ptr_t ptr, अचिन्हित base, अचिन्हित wid, bool net_flag) override;

	// Implementation of vvp_signal_value methods
      अचिन्हित मूल्य_आकार() स्थिर override;
      vvp_bit4_t मूल्य(अचिन्हित idx) स्थिर override;
      vvp_scalar_t scalar_मूल्य(अचिन्हित idx) स्थिर override;
      व्योम vec4_मूल्य(vvp_vector4_t&) स्थिर override;
	// This is new to vvp_wire_vec8
      vvp_vector8_t vec8_मूल्य() स्थिर;

        // Support for $countdrivers
      vvp_bit4_t driven_मूल्य(अचिन्हित idx) स्थिर override;
      bool is_forced(अचिन्हित idx) स्थिर override;

    निजी:
      vvp_scalar_t filtered_मूल्य(अचिन्हित idx) स्थिर;

    निजी:
      bool needs_init_;
      vvp_vector8_t bits8_;
      vvp_vector8_t force8_; // the value being forced
};

class vvp_wire_real : खुला vvp_wire_base {

    खुला:
      explicit vvp_wire_real(व्योम);

	// The main filter behavior for this class
      prop_t filter_real(द्विगुन&bit) override;

	// Abstract methods from vvp_vpi_callback
      व्योम get_मूल्य(काष्ठा t_vpi_मूल्य*मूल्य) override;
	// Abstract methods from vvp_net_fit_t
      अचिन्हित filter_आकार() स्थिर override;
      व्योम force_fil_vec4(स्थिर vvp_vector4_t&val, स्थिर vvp_vector2_t&mask) override;
      व्योम force_fil_vec8(स्थिर vvp_vector8_t&val, स्थिर vvp_vector2_t&mask) override;
      व्योम force_fil_real(द्विगुन val, स्थिर vvp_vector2_t&mask) override;
      व्योम release(vvp_net_ptr_t ptr, bool net_flag) override;
      व्योम release_pv(vvp_net_ptr_t ptr, अचिन्हित base, अचिन्हित wid, bool net_flag) override;

	// Implementation of vvp_signal_value methods
      अचिन्हित मूल्य_आकार() स्थिर override;
      vvp_bit4_t मूल्य(अचिन्हित idx) स्थिर override;
      vvp_scalar_t scalar_मूल्य(अचिन्हित idx) स्थिर override;
      व्योम vec4_मूल्य(vvp_vector4_t&) स्थिर override;
      द्विगुन real_मूल्य() स्थिर override;

      व्योम get_signal_मूल्य(काष्ठा t_vpi_मूल्य*vp) override;

    निजी:
      द्विगुन bit_;
      द्विगुन force_;
      bool needs_init_ = true;
};

#अगर 0
class vvp_wire_string : खुला vvp_wire_base {

    खुला:
      explicit vvp_wire_string(व्योम);

	// Abstract methods from vvp_vpi_callback
      व्योम get_मूल्य(काष्ठा t_vpi_मूल्य*मूल्य);
	// Abstract methods from vvp_net_fil_t
      अचिन्हित filter_आकार() स्थिर;
      व्योम force_fil_vec4(स्थिर vvp_vector4_t&val, स्थिर vvp_vector2_t&mask);
      व्योम force_fil_vec8(स्थिर vvp_vector8_t&val, स्थिर vvp_vector2_t&mask);
      व्योम force_fil_real(द्विगुन val, स्थिर vvp_vector2_t&mask);
      व्योम release(vvp_net_ptr_t ptr, bool net_flag);
      व्योम release_pv(vvp_net_ptr_t ptr, अचिन्हित base, अचिन्हित wid, bool net_flag);

	// Implementation of vvp_signal_value methods
      अचिन्हित मूल्य_आकार() स्थिर;
      vvp_bit4_t मूल्य(अचिन्हित idx) स्थिर;
      vvp_scalar_t scalar_मूल्य(अचिन्हित idx) स्थिर;
      व्योम vec4_मूल्य(vvp_vector4_t&) स्थिर;
      द्विगुन real_मूल्य() स्थिर;

      व्योम get_signal_मूल्य(काष्ठा t_vpi_मूल्य*vp);

    निजी:
      std::string मूल्य_2;
};
#पूर्ण_अगर

#पूर्ण_अगर /* IVL_vvp_net_sig_H */
