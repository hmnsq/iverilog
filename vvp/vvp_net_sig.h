#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ IVL_vvp_net_sig_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IVL_vvp_net_sig_H
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

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_IOSFWD
# include  <iosfwd>
#ਨਹੀਂਤਾਂ
# include  <iostream>
#ਜੇ_ਅੰਤ

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


class vvp_fun_signal_ਅਧਾਰ : ਜਨਤਕ vvp_net_fun_t {

    ਜਨਤਕ:
      vvp_fun_signal_ਅਧਾਰ();

      ਖਾਲੀ deassign();
      ਖਾਲੀ deassign_pv(ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ wid);

    ਜਨਤਕ:

	/* The %cassign/link instruction needs a place to write the
	   source node of the force, so that subsequent %cassign and
	   %deassign instructions can undo the link as needed. */
      class vvp_net_t*cassign_link;

    ਸੁਰੱਖਿਅਤ:
      bool continuous_ਸੌਂਪੋ_ਸਰਗਰਮ;
      vvp_vector2_t ਸੌਂਪੋ_ਨਕਾਬ;

    ਸੁਰੱਖਿਅਤ:
	// This is true until at least one propagation happens.
      bool needs_init_;
};

/*
 * Variables and wires can have their values accessed, so this base
 * class offers the unified concept of an accessible value.
 */
class vvp_signal_ਮੁੱਲ {
    ਜਨਤਕ:
      ਆਭਾਸੀ ~vvp_signal_ਮੁੱਲ() =0;
      ਆਭਾਸੀ ਬਿਨਾਨਿਸ਼ਾਨੀ ਮੁੱਲ_ਮਾਪ() ਅਚਲ =0;
      ਆਭਾਸੀ vvp_bit4_t ਮੁੱਲ(ਬਿਨਾਨਿਸ਼ਾਨੀ idx) ਅਚਲ =0;
      ਆਭਾਸੀ vvp_scalar_t scalar_ਮੁੱਲ(ਬਿਨਾਨਿਸ਼ਾਨੀ idx) ਅਚਲ =0;
      ਆਭਾਸੀ ਖਾਲੀ vec4_ਮੁੱਲ(vvp_vector4_t&) ਅਚਲ =0;
      ਆਭਾਸੀ ਦੁੱਗਣਾ real_ਮੁੱਲ() ਅਚਲ;

      ਆਭਾਸੀ ਖਾਲੀ get_signal_ਮੁੱਲ(ਬਣਤਰ t_vpi_ਮੁੱਲ*vp);
};

/*
 * This abstract class is a little more specific than the signal_base
 * class, in that it adds vector access methods.
 */
class vvp_fun_signal_vec : ਜਨਤਕ vvp_fun_signal_ਅਧਾਰ {
    ਜਨਤਕ:
      ਆਭਾਸੀ ਅਚਲ vvp_vector4_t& vec4_unfiltered_ਮੁੱਲ() ਅਚਲ =0;
};

class automatic_signal_ਅਧਾਰ : ਜਨਤਕ vvp_signal_ਮੁੱਲ, ਜਨਤਕ vvp_net_fil_t {

    ਜਨਤਕ:
	// Automatic variables cannot be forced or released. Provide
	// stubs that assert.
      ਆਭਾਸੀ ਖਾਲੀ release(vvp_net_ptr_t ptr, bool net_flag) override;
      ਆਭਾਸੀ ਖਾਲੀ release_pv(vvp_net_ptr_t ptr, ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ wid, bool net_flag) override;

      ਆਭਾਸੀ ਬਿਨਾਨਿਸ਼ਾਨੀ filter_ਮਾਪ() ਅਚਲ override;
      ਆਭਾਸੀ ਖਾਲੀ force_fil_vec4(ਅਚਲ vvp_vector4_t&val, ਅਚਲ vvp_vector2_t&ਨਕਾਬ) override;
      ਆਭਾਸੀ ਖਾਲੀ force_fil_vec8(ਅਚਲ vvp_vector8_t&val, ਅਚਲ vvp_vector2_t&ਨਕਾਬ) override;
      ਆਭਾਸੀ ਖਾਲੀ force_fil_real(ਦੁੱਗਣਾ val, ਅਚਲ vvp_vector2_t&ਨਕਾਬ) override;
      ਆਭਾਸੀ ਖਾਲੀ get_ਮੁੱਲ(ਬਣਤਰ t_vpi_ਮੁੱਲ*ਮੁੱਲ) override;
};

/*
 * Statically allocated vvp_fun_signal4.
 */
class vvp_fun_signal4_sa : ਜਨਤਕ vvp_fun_signal_vec {

    ਜਨਤਕ:
      explicit vvp_fun_signal4_sa(ਬਿਨਾਨਿਸ਼ਾਨੀ wid, vvp_bit4_t init=BIT4_X);

      ਖਾਲੀ recv_vec4(vvp_net_ptr_t ਪੋਰਟ, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                     vvp_context_t) override;
      ਖਾਲੀ recv_vec8(vvp_net_ptr_t ਪੋਰਟ, ਅਚਲ vvp_vector8_t&ਦੋਆਧਾਰੀ_ਅੰਕ) override;

	// Part select variants of above
      ਖਾਲੀ recv_vec4_pv(vvp_net_ptr_t ਪੋਰਟ, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
			ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ vwid, vvp_context_t) override;
      ਖਾਲੀ recv_vec8_pv(vvp_net_ptr_t ਪੋਰਟ, ਅਚਲ vvp_vector8_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
			ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ vwid) override;

	// Get information about the vector value.
      ਅਚਲ vvp_vector4_t& vec4_unfiltered_ਮੁੱਲ() ਅਚਲ override;

    ਨਿਜੀ:
      vvp_vector4_t bits4_;
};

/*
 * Automatically allocated vvp_fun_signal4.
 */
class vvp_fun_signal4_aa : ਜਨਤਕ vvp_fun_signal_vec, ਜਨਤਕ automatic_signal_ਅਧਾਰ, ਜਨਤਕ automatic_hooks_s {

    ਜਨਤਕ:
      explicit vvp_fun_signal4_aa(ਬਿਨਾਨਿਸ਼ਾਨੀ wid, vvp_bit4_t init=BIT4_X);
      ~vvp_fun_signal4_aa() override;

      ਖਾਲੀ alloc_instance(vvp_context_t context) override;
      ਖਾਲੀ reset_instance(vvp_context_t context) override;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_WITH_VALGRIND
      ਖਾਲੀ free_instance(vvp_context_t context) override;
#ਜੇ_ਅੰਤ

      ਖਾਲੀ recv_vec4(vvp_net_ptr_t ਪੋਰਟ, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
                     vvp_context_t context) override;

	// Part select variants of above
      ਖਾਲੀ recv_vec4_pv(vvp_net_ptr_t ਪੋਰਟ, ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ,
			ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ vwid, vvp_context_t) override;

	// Get information about the vector value.
      ਬਿਨਾਨਿਸ਼ਾਨੀ   ਮੁੱਲ_ਮਾਪ() ਅਚਲ override;
      vvp_bit4_t ਮੁੱਲ(ਬਿਨਾਨਿਸ਼ਾਨੀ idx) ਅਚਲ override;
      vvp_scalar_t scalar_ਮੁੱਲ(ਬਿਨਾਨਿਸ਼ਾਨੀ idx) ਅਚਲ override;
      ਖਾਲੀ vec4_ਮੁੱਲ(vvp_vector4_t&) ਅਚਲ override;
      ਅਚਲ vvp_vector4_t& vec4_unfiltered_ਮੁੱਲ() ਅਚਲ override;

    ਜਨਤਕ: // These objects are only permallocated.
      ਸਥਿਰ ਖਾਲੀ* ਸੰਚਾਲਕ new(std::size_t ਮਾਪ) { ਵਾਪਸ vvp_net_fun_t::ਢੇਰ.alloc(ਮਾਪ); }
      ਸਥਿਰ ਖਾਲੀ ਸੰਚਾਲਕ delete(ਖਾਲੀ*obj);

    ਨਿਜੀ:
      ਬਿਨਾਨਿਸ਼ਾਨੀ context_idx_;
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਮਾਪ_2;
      vvp_bit4_t init_;
};

class vvp_fun_signal_real : ਜਨਤਕ vvp_fun_signal_ਅਧਾਰ {

    ਜਨਤਕ:
      explicit vvp_fun_signal_real() {};

	// Get information about the vector value.
      ਆਭਾਸੀ ਦੁੱਗਣਾ real_unfiltered_ਮੁੱਲ() ਅਚਲ = 0;
};

/*
 * Statically allocated vvp_fun_signal_real.
 */
class vvp_fun_signal_real_sa : ਜਨਤਕ vvp_fun_signal_real {

    ਜਨਤਕ:
      explicit vvp_fun_signal_real_sa();

      ਖਾਲੀ recv_real(vvp_net_ptr_t ਪੋਰਟ, ਦੁੱਗਣਾ ਦੋਆਧਾਰੀ_ਅੰਕ,
                     vvp_context_t) override;

	// Get information about the vector value.
      ਦੁੱਗਣਾ real_unfiltered_ਮੁੱਲ() ਅਚਲ override;

    ਨਿਜੀ:
      ਦੁੱਗਣਾ ਬਿੱਟ_2;
};

/*
 * Automatically allocated vvp_fun_signal_real.
 */
class vvp_fun_signal_real_aa : ਜਨਤਕ vvp_fun_signal_real, ਜਨਤਕ automatic_signal_ਅਧਾਰ, ਜਨਤਕ automatic_hooks_s {

    ਜਨਤਕ:
      explicit vvp_fun_signal_real_aa();
      ~vvp_fun_signal_real_aa() override;

      ਖਾਲੀ alloc_instance(vvp_context_t context) override;
      ਖਾਲੀ reset_instance(vvp_context_t context) override;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_WITH_VALGRIND
      ਖਾਲੀ free_instance(vvp_context_t context) override;
#ਜੇ_ਅੰਤ

      ਖਾਲੀ recv_real(vvp_net_ptr_t ਪੋਰਟ, ਦੁੱਗਣਾ ਦੋਆਧਾਰੀ_ਅੰਕ,
                     vvp_context_t context) override;

	// Get information about the vector value.
      ਦੁੱਗਣਾ real_unfiltered_ਮੁੱਲ() ਅਚਲ override;

	// Get information about the vector value.
      ਬਿਨਾਨਿਸ਼ਾਨੀ   ਮੁੱਲ_ਮਾਪ() ਅਚਲ override;
      vvp_bit4_t ਮੁੱਲ(ਬਿਨਾਨਿਸ਼ਾਨੀ idx) ਅਚਲ override;
      vvp_scalar_t scalar_ਮੁੱਲ(ਬਿਨਾਨਿਸ਼ਾਨੀ idx) ਅਚਲ override;
      ਖਾਲੀ vec4_ਮੁੱਲ(vvp_vector4_t&) ਅਚਲ override;
      ਦੁੱਗਣਾ real_ਮੁੱਲ() ਅਚਲ override;
      ਖਾਲੀ get_signal_ਮੁੱਲ(ਬਣਤਰ t_vpi_ਮੁੱਲ*vp) override;

    ਜਨਤਕ: // These objects are only permallocated.
      ਸਥਿਰ ਖਾਲੀ* ਸੰਚਾਲਕ new(std::size_t ਮਾਪ);
      ਸਥਿਰ ਖਾਲੀ ਸੰਚਾਲਕ delete(ਖਾਲੀ*obj);

    ਨਿਜੀ:
      ਬਿਨਾਨਿਸ਼ਾਨੀ context_idx_;
};


class vvp_fun_signal_string : ਜਨਤਕ vvp_fun_signal_ਅਧਾਰ {

    ਜਨਤਕ:
      explicit vvp_fun_signal_string() {};

      ਆਭਾਸੀ ਅਚਲ std::string& get_string() ਅਚਲ =0;
};

/*
 * Statically allocated vvp_fun_signal_string.
 */
class vvp_fun_signal_string_sa : ਜਨਤਕ vvp_fun_signal_string {

    ਜਨਤਕ:
      explicit vvp_fun_signal_string_sa();

      ਖਾਲੀ recv_string(vvp_net_ptr_t ਪੋਰਟ, ਅਚਲ std::string&ਦੋਆਧਾਰੀ_ਅੰਕ,
		       vvp_context_t context) override;

      ਅਚਲ std::string& get_string() ਅਚਲ override;

    ਨਿਜੀ:
      std::string ਮੁੱਲ_2;
};

/*
 * Automatically allocated vvp_fun_signal_real.
 */
class vvp_fun_signal_string_aa : ਜਨਤਕ vvp_fun_signal_string, ਜਨਤਕ automatic_signal_ਅਧਾਰ, ਜਨਤਕ automatic_hooks_s {

    ਜਨਤਕ:
      explicit vvp_fun_signal_string_aa();
      ~vvp_fun_signal_string_aa() override;

      ਖਾਲੀ alloc_instance(vvp_context_t context) override;
      ਖਾਲੀ reset_instance(vvp_context_t context) override;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_WITH_VALGRIND
      ਖਾਲੀ free_instance(vvp_context_t context) override;
#ਜੇ_ਅੰਤ
      ਖਾਲੀ recv_string(vvp_net_ptr_t ਪੋਰਟ, ਅਚਲ std::string&ਦੋਆਧਾਰੀ_ਅੰਕ,
		       vvp_context_t context) override;

	// Get information about the vector value.
      ਬਿਨਾਨਿਸ਼ਾਨੀ   ਮੁੱਲ_ਮਾਪ() ਅਚਲ override;
      vvp_bit4_t ਮੁੱਲ(ਬਿਨਾਨਿਸ਼ਾਨੀ idx) ਅਚਲ override;
      vvp_scalar_t scalar_ਮੁੱਲ(ਬਿਨਾਨਿਸ਼ਾਨੀ idx) ਅਚਲ override;
      ਖਾਲੀ vec4_ਮੁੱਲ(vvp_vector4_t&) ਅਚਲ override;
      ਦੁੱਗਣਾ real_ਮੁੱਲ() ਅਚਲ override;
      ਅਚਲ std::string& get_string() ਅਚਲ override;
      ਖਾਲੀ get_signal_ਮੁੱਲ(ਬਣਤਰ t_vpi_ਮੁੱਲ*vp) override;

    ਜਨਤਕ: // These objects are only permallocated.
      ਸਥਿਰ ਖਾਲੀ* ਸੰਚਾਲਕ new(std::size_t ਮਾਪ);
      ਸਥਿਰ ਖਾਲੀ ਸੰਚਾਲਕ delete(ਖਾਲੀ*obj);

    ਨਿਜੀ:
      ਬਿਨਾਨਿਸ਼ਾਨੀ context_idx_;
};

class vvp_fun_signal_object : ਜਨਤਕ vvp_fun_signal_ਅਧਾਰ {

    ਜਨਤਕ:
      explicit vvp_fun_signal_object(ਬਿਨਾਨਿਸ਼ਾਨੀ ਮਾਪ) { ਮਾਪ_2 = ਮਾਪ; };
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਮਾਪ() ਅਚਲ { ਵਾਪਸ ਮਾਪ_2; }

      ਆਭਾਸੀ vvp_object_t get_object() ਅਚਲ =0;
    ਨਿਜੀ:
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਮਾਪ_2;
};

/*
 * Statically allocated vvp_fun_signal_string.
 */
class vvp_fun_signal_object_sa : ਜਨਤਕ vvp_fun_signal_object {

    ਜਨਤਕ:
      explicit vvp_fun_signal_object_sa(ਬਿਨਾਨਿਸ਼ਾਨੀ ਮਾਪ);

      ਖਾਲੀ recv_object(vvp_net_ptr_t ਪੋਰਟ, vvp_object_t ਦੋਆਧਾਰੀ_ਅੰਕ,
		    vvp_context_t context) override;

      vvp_object_t get_object() ਅਚਲ override;

    ਨਿਜੀ:
      vvp_object_t ਮੁੱਲ_2;
};

/*
 * Automatically allocated vvp_fun_signal_real.
 */
class vvp_fun_signal_object_aa : ਜਨਤਕ vvp_fun_signal_object, ਜਨਤਕ automatic_signal_ਅਧਾਰ, ਜਨਤਕ automatic_hooks_s {

    ਜਨਤਕ:
      explicit vvp_fun_signal_object_aa(ਬਿਨਾਨਿਸ਼ਾਨੀ ਮਾਪ);
      ~vvp_fun_signal_object_aa() override;

      ਖਾਲੀ alloc_instance(vvp_context_t context) override;
      ਖਾਲੀ reset_instance(vvp_context_t context) override;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_WITH_VALGRIND
      ਖਾਲੀ free_instance(vvp_context_t context) override;
#ਜੇ_ਅੰਤ

      ਖਾਲੀ recv_object(vvp_net_ptr_t ਪੋਰਟ, vvp_object_t ਦੋਆਧਾਰੀ_ਅੰਕ,
		    vvp_context_t context) override;

	// Get information about the vector value.
      ਬਿਨਾਨਿਸ਼ਾਨੀ   ਮੁੱਲ_ਮਾਪ() ਅਚਲ override;
      vvp_bit4_t ਮੁੱਲ(ਬਿਨਾਨਿਸ਼ਾਨੀ idx) ਅਚਲ override;
      vvp_scalar_t scalar_ਮੁੱਲ(ਬਿਨਾਨਿਸ਼ਾਨੀ idx) ਅਚਲ override;
      ਖਾਲੀ vec4_ਮੁੱਲ(vvp_vector4_t&) ਅਚਲ override;
	//double real_value() const;
	//void get_signal_value(struct t_vpi_value*vp);

      vvp_object_t get_object() ਅਚਲ override;

    ਜਨਤਕ: // These objects are only permallocated.
      ਸਥਿਰ ਖਾਲੀ* ਸੰਚਾਲਕ new(std::size_t ਮਾਪ);
      ਸਥਿਰ ਖਾਲੀ ਸੰਚਾਲਕ delete(ਖਾਲੀ*obj);

    ਨਿਜੀ:
      ਬਿਨਾਨਿਸ਼ਾਨੀ context_idx_;
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

class vvp_wire_ਅਧਾਰ  : ਜਨਤਕ vvp_net_fil_t, ਜਨਤਕ vvp_signal_ਮੁੱਲ {

    ਜਨਤਕ:
      vvp_wire_ਅਧਾਰ();
      ~vvp_wire_ਅਧਾਰ() override;

        // Support for $countdrivers
      ਆਭਾਸੀ vvp_bit4_t driven_ਮੁੱਲ(ਬਿਨਾਨਿਸ਼ਾਨੀ idx) ਅਚਲ;
      ਆਭਾਸੀ bool is_forced(ਬਿਨਾਨਿਸ਼ਾਨੀ idx) ਅਚਲ;
};

class vvp_wire_vec4 : ਜਨਤਕ vvp_wire_ਅਧਾਰ {

    ਜਨਤਕ:
      vvp_wire_vec4(ਬਿਨਾਨਿਸ਼ਾਨੀ wid, vvp_bit4_t init);

	// The main filter behavior for this class. These methods take
	// the value that the node is driven to, and applies the force
	// filters. In wires, this also saves the driven value, so
	// that when a force is released, we can revert to the driven value.
      prop_t filter_vec4(ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ, vvp_vector4_t&rep,
			 ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ vwid) override;
      prop_t filter_vec8(ਅਚਲ vvp_vector8_t&val, vvp_vector8_t&rep,
			 ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ vwid) override;

	// Abstract methods from vvp_vpi_callback
      ਖਾਲੀ get_ਮੁੱਲ(ਬਣਤਰ t_vpi_ਮੁੱਲ*ਮੁੱਲ) override;
	// Abstract methods from vvp_net_fit_t
      ਬਿਨਾਨਿਸ਼ਾਨੀ filter_ਮਾਪ() ਅਚਲ override;
      ਖਾਲੀ force_fil_vec4(ਅਚਲ vvp_vector4_t&val, ਅਚਲ vvp_vector2_t&ਨਕਾਬ) override;
      ਖਾਲੀ force_fil_vec8(ਅਚਲ vvp_vector8_t&val, ਅਚਲ vvp_vector2_t&ਨਕਾਬ) override;
      ਖਾਲੀ force_fil_real(ਦੁੱਗਣਾ val, ਅਚਲ vvp_vector2_t&ਨਕਾਬ) override;
      ਖਾਲੀ release(vvp_net_ptr_t ptr, bool net_flag) override;
      ਖਾਲੀ release_pv(vvp_net_ptr_t ptr, ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ wid, bool net_flag) override;

	// Implementation of vvp_signal_value methods
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਮੁੱਲ_ਮਾਪ() ਅਚਲ override;
      vvp_bit4_t ਮੁੱਲ(ਬਿਨਾਨਿਸ਼ਾਨੀ idx) ਅਚਲ override;
      vvp_scalar_t scalar_ਮੁੱਲ(ਬਿਨਾਨਿਸ਼ਾਨੀ idx) ਅਚਲ override;
      ਖਾਲੀ vec4_ਮੁੱਲ(vvp_vector4_t&) ਅਚਲ override;

        // Support for $countdrivers
      vvp_bit4_t driven_ਮੁੱਲ(ਬਿਨਾਨਿਸ਼ਾਨੀ idx) ਅਚਲ override;
      bool is_forced(ਬਿਨਾਨਿਸ਼ਾਨੀ idx) ਅਚਲ override;

    ਨਿਜੀ:
      vvp_bit4_t filtered_ਮੁੱਲ(ਬਿਨਾਨਿਸ਼ਾਨੀ idx) ਅਚਲ;

    ਨਿਜੀ:
      bool needs_init_;
      vvp_vector4_t bits4_; // The tracked driven value
      vvp_vector4_t force4_; // the value being forced
};

class vvp_wire_vec8 : ਜਨਤਕ vvp_wire_ਅਧਾਰ {

    ਜਨਤਕ:
      explicit vvp_wire_vec8(ਬਿਨਾਨਿਸ਼ਾਨੀ wid);

	// The main filter behavior for this class
      prop_t filter_vec4(ਅਚਲ vvp_vector4_t&ਦੋਆਧਾਰੀ_ਅੰਕ, vvp_vector4_t&rep,
			 ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ vwid) override;
      prop_t filter_vec8(ਅਚਲ vvp_vector8_t&val, vvp_vector8_t&rep,
			 ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ vwid) override;

	// island ports use this method to filter arbitrary values
	// through the force filter.
      prop_t filter_ਨਿਵੇਸ਼_vec8(ਅਚਲ vvp_vector8_t&val, vvp_vector8_t&rep) ਅਚਲ;


	// Abstract methods from vvp_vpi_callback
      ਖਾਲੀ get_ਮੁੱਲ(ਬਣਤਰ t_vpi_ਮੁੱਲ*ਮੁੱਲ) override;
	// Abstract methods from vvp_net_fit_t
      ਬਿਨਾਨਿਸ਼ਾਨੀ filter_ਮਾਪ() ਅਚਲ override;
      ਖਾਲੀ force_fil_vec4(ਅਚਲ vvp_vector4_t&val, ਅਚਲ vvp_vector2_t&ਨਕਾਬ) override;
      ਖਾਲੀ force_fil_vec8(ਅਚਲ vvp_vector8_t&val, ਅਚਲ vvp_vector2_t&ਨਕਾਬ) override;
      ਖਾਲੀ force_fil_real(ਦੁੱਗਣਾ val, ਅਚਲ vvp_vector2_t&ਨਕਾਬ) override;
      ਖਾਲੀ release(vvp_net_ptr_t ptr, bool net_flag) override;
      ਖਾਲੀ release_pv(vvp_net_ptr_t ptr, ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ wid, bool net_flag) override;

	// Implementation of vvp_signal_value methods
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਮੁੱਲ_ਮਾਪ() ਅਚਲ override;
      vvp_bit4_t ਮੁੱਲ(ਬਿਨਾਨਿਸ਼ਾਨੀ idx) ਅਚਲ override;
      vvp_scalar_t scalar_ਮੁੱਲ(ਬਿਨਾਨਿਸ਼ਾਨੀ idx) ਅਚਲ override;
      ਖਾਲੀ vec4_ਮੁੱਲ(vvp_vector4_t&) ਅਚਲ override;
	// This is new to vvp_wire_vec8
      vvp_vector8_t vec8_ਮੁੱਲ() ਅਚਲ;

        // Support for $countdrivers
      vvp_bit4_t driven_ਮੁੱਲ(ਬਿਨਾਨਿਸ਼ਾਨੀ idx) ਅਚਲ override;
      bool is_forced(ਬਿਨਾਨਿਸ਼ਾਨੀ idx) ਅਚਲ override;

    ਨਿਜੀ:
      vvp_scalar_t filtered_ਮੁੱਲ(ਬਿਨਾਨਿਸ਼ਾਨੀ idx) ਅਚਲ;

    ਨਿਜੀ:
      bool needs_init_;
      vvp_vector8_t bits8_;
      vvp_vector8_t force8_; // the value being forced
};

class vvp_wire_real : ਜਨਤਕ vvp_wire_ਅਧਾਰ {

    ਜਨਤਕ:
      explicit vvp_wire_real(ਖਾਲੀ);

	// The main filter behavior for this class
      prop_t filter_real(ਦੁੱਗਣਾ&ਦੋਆਧਾਰੀ_ਅੰਕ) override;

	// Abstract methods from vvp_vpi_callback
      ਖਾਲੀ get_ਮੁੱਲ(ਬਣਤਰ t_vpi_ਮੁੱਲ*ਮੁੱਲ) override;
	// Abstract methods from vvp_net_fit_t
      ਬਿਨਾਨਿਸ਼ਾਨੀ filter_ਮਾਪ() ਅਚਲ override;
      ਖਾਲੀ force_fil_vec4(ਅਚਲ vvp_vector4_t&val, ਅਚਲ vvp_vector2_t&ਨਕਾਬ) override;
      ਖਾਲੀ force_fil_vec8(ਅਚਲ vvp_vector8_t&val, ਅਚਲ vvp_vector2_t&ਨਕਾਬ) override;
      ਖਾਲੀ force_fil_real(ਦੁੱਗਣਾ val, ਅਚਲ vvp_vector2_t&ਨਕਾਬ) override;
      ਖਾਲੀ release(vvp_net_ptr_t ptr, bool net_flag) override;
      ਖਾਲੀ release_pv(vvp_net_ptr_t ptr, ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ wid, bool net_flag) override;

	// Implementation of vvp_signal_value methods
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਮੁੱਲ_ਮਾਪ() ਅਚਲ override;
      vvp_bit4_t ਮੁੱਲ(ਬਿਨਾਨਿਸ਼ਾਨੀ idx) ਅਚਲ override;
      vvp_scalar_t scalar_ਮੁੱਲ(ਬਿਨਾਨਿਸ਼ਾਨੀ idx) ਅਚਲ override;
      ਖਾਲੀ vec4_ਮੁੱਲ(vvp_vector4_t&) ਅਚਲ override;
      ਦੁੱਗਣਾ real_ਮੁੱਲ() ਅਚਲ override;

      ਖਾਲੀ get_signal_ਮੁੱਲ(ਬਣਤਰ t_vpi_ਮੁੱਲ*vp) override;

    ਨਿਜੀ:
      ਦੁੱਗਣਾ ਦੋਆਧਾਰੀ_ਅੰਕ_2;
      ਦੁੱਗਣਾ force_;
      bool needs_init_ = true;
};

#ਜੇ 0
class vvp_wire_string : ਜਨਤਕ vvp_wire_ਅਧਾਰ {

    ਜਨਤਕ:
      explicit vvp_wire_string(ਖਾਲੀ);

	// Abstract methods from vvp_vpi_callback
      ਖਾਲੀ get_ਮੁੱਲ(ਬਣਤਰ t_vpi_ਮੁੱਲ*ਮੁੱਲ);
	// Abstract methods from vvp_net_fil_t
      ਬਿਨਾਨਿਸ਼ਾਨੀ filter_ਮਾਪ() ਅਚਲ;
      ਖਾਲੀ force_fil_vec4(ਅਚਲ vvp_vector4_t&val, ਅਚਲ vvp_vector2_t&ਨਕਾਬ);
      ਖਾਲੀ force_fil_vec8(ਅਚਲ vvp_vector8_t&val, ਅਚਲ vvp_vector2_t&ਨਕਾਬ);
      ਖਾਲੀ force_fil_real(ਦੁੱਗਣਾ val, ਅਚਲ vvp_vector2_t&ਨਕਾਬ);
      ਖਾਲੀ release(vvp_net_ptr_t ptr, bool net_flag);
      ਖਾਲੀ release_pv(vvp_net_ptr_t ptr, ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ, ਬਿਨਾਨਿਸ਼ਾਨੀ wid, bool net_flag);

	// Implementation of vvp_signal_value methods
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਮੁੱਲ_ਮਾਪ() ਅਚਲ;
      vvp_bit4_t ਮੁੱਲ(ਬਿਨਾਨਿਸ਼ਾਨੀ idx) ਅਚਲ;
      vvp_scalar_t scalar_ਮੁੱਲ(ਬਿਨਾਨਿਸ਼ਾਨੀ idx) ਅਚਲ;
      ਖਾਲੀ vec4_ਮੁੱਲ(vvp_vector4_t&) ਅਚਲ;
      ਦੁੱਗਣਾ real_ਮੁੱਲ() ਅਚਲ;

      ਖਾਲੀ get_signal_ਮੁੱਲ(ਬਣਤਰ t_vpi_ਮੁੱਲ*vp);

    ਨਿਜੀ:
      std::string ਮੁੱਲ_2;
};
#ਜੇ_ਅੰਤ

#ਜੇ_ਅੰਤ /* IVL_vvp_net_sig_H */
