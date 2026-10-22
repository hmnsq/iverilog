#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ IVL_vvp_net_sig_H
#ವ್ಯಾಖ್ಯಾನಿಸು IVL_vvp_net_sig_H
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

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_IOSFWD
# include  <iosfwd>
#ಇಲ್ಲದಿದ್ದರೆ
# include  <iostream>
#ಆದರೆ_ಅಂತ್ಯ

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


class vvp_fun_signal_ತಳ : ಸಾರ್ವಜನಿಕ vvp_net_fun_t {

    ಸಾರ್ವಜನಿಕ:
      vvp_fun_signal_ತಳ();

      ಶೂನ್ಯ deassign();
      ಶೂನ್ಯ deassign_pv(ಚಿಹ್ನೆರಹಿತ ತಳ, ಚಿಹ್ನೆರಹಿತ wid);

    ಸಾರ್ವಜನಿಕ:

	/* The %cassign/link instruction needs a place to write the
	   source node of the force, so that subsequent %cassign and
	   %deassign instructions can undo the link as needed. */
      class vvp_net_t*cassign_link;

    ಸಂರಕ್ಷಿತ:
      bool continuous_ನಿಯೋಜಿಸು_ಸಕ್ರಿಯ;
      vvp_vector2_t ನಿಯೋಜಿಸು_ಮುಖವಾಡ;

    ಸಂರಕ್ಷಿತ:
	// This is true until at least one propagation happens.
      bool needs_init_;
};

/*
 * Variables and wires can have their values accessed, so this base
 * class offers the unified concept of an accessible value.
 */
class vvp_signal_ಮೌಲ್ಯ {
    ಸಾರ್ವಜನಿಕ:
      ವಾಸ್ತವಿಕ ~vvp_signal_ಮೌಲ್ಯ() =0;
      ವಾಸ್ತವಿಕ ಚಿಹ್ನೆರಹಿತ ಮೌಲ್ಯ_ಗಾತ್ರ() ಸ್ಥಿರಾಂಕ =0;
      ವಾಸ್ತವಿಕ vvp_bit4_t ಮೌಲ್ಯ(ಚಿಹ್ನೆರಹಿತ idx) ಸ್ಥಿರಾಂಕ =0;
      ವಾಸ್ತವಿಕ vvp_scalar_t scalar_ಮೌಲ್ಯ(ಚಿಹ್ನೆರಹಿತ idx) ಸ್ಥಿರಾಂಕ =0;
      ವಾಸ್ತವಿಕ ಶೂನ್ಯ vec4_ಮೌಲ್ಯ(vvp_vector4_t&) ಸ್ಥಿರಾಂಕ =0;
      ವಾಸ್ತವಿಕ ದ್ವಿಗುಣ real_ಮೌಲ್ಯ() ಸ್ಥಿರಾಂಕ;

      ವಾಸ್ತವಿಕ ಶೂನ್ಯ get_signal_ಮೌಲ್ಯ(ರಚನೆ t_vpi_ಮೌಲ್ಯ*vp);
};

/*
 * This abstract class is a little more specific than the signal_base
 * class, in that it adds vector access methods.
 */
class vvp_fun_signal_vec : ಸಾರ್ವಜನಿಕ vvp_fun_signal_ತಳ {
    ಸಾರ್ವಜನಿಕ:
      ವಾಸ್ತವಿಕ ಸ್ಥಿರಾಂಕ vvp_vector4_t& vec4_unfiltered_ಮೌಲ್ಯ() ಸ್ಥಿರಾಂಕ =0;
};

class automatic_signal_ತಳ : ಸಾರ್ವಜನಿಕ vvp_signal_ಮೌಲ್ಯ, ಸಾರ್ವಜನಿಕ vvp_net_fil_t {

    ಸಾರ್ವಜನಿಕ:
	// Automatic variables cannot be forced or released. Provide
	// stubs that assert.
      ವಾಸ್ತವಿಕ ಶೂನ್ಯ release(vvp_net_ptr_t ptr, bool net_flag) override;
      ವಾಸ್ತವಿಕ ಶೂನ್ಯ release_pv(vvp_net_ptr_t ptr, ಚಿಹ್ನೆರಹಿತ ತಳ, ಚಿಹ್ನೆರಹಿತ wid, bool net_flag) override;

      ವಾಸ್ತವಿಕ ಚಿಹ್ನೆರಹಿತ filter_ಗಾತ್ರ() ಸ್ಥಿರಾಂಕ override;
      ವಾಸ್ತವಿಕ ಶೂನ್ಯ force_fil_vec4(ಸ್ಥಿರಾಂಕ vvp_vector4_t&val, ಸ್ಥಿರಾಂಕ vvp_vector2_t&ಮುಖವಾಡ) override;
      ವಾಸ್ತವಿಕ ಶೂನ್ಯ force_fil_vec8(ಸ್ಥಿರಾಂಕ vvp_vector8_t&val, ಸ್ಥಿರಾಂಕ vvp_vector2_t&ಮುಖವಾಡ) override;
      ವಾಸ್ತವಿಕ ಶೂನ್ಯ force_fil_real(ದ್ವಿಗುಣ val, ಸ್ಥಿರಾಂಕ vvp_vector2_t&ಮುಖವಾಡ) override;
      ವಾಸ್ತವಿಕ ಶೂನ್ಯ get_ಮೌಲ್ಯ(ರಚನೆ t_vpi_ಮೌಲ್ಯ*ಮೌಲ್ಯ) override;
};

/*
 * Statically allocated vvp_fun_signal4.
 */
class vvp_fun_signal4_sa : ಸಾರ್ವಜನಿಕ vvp_fun_signal_vec {

    ಸಾರ್ವಜನಿಕ:
      explicit vvp_fun_signal4_sa(ಚಿಹ್ನೆರಹಿತ wid, vvp_bit4_t init=BIT4_X);

      ಶೂನ್ಯ recv_vec4(vvp_net_ptr_t ಪೋರ್ಟ್, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
                     vvp_context_t) override;
      ಶೂನ್ಯ recv_vec8(vvp_net_ptr_t ಪೋರ್ಟ್, ಸ್ಥಿರಾಂಕ vvp_vector8_t&ಬಿಟ್) override;

	// Part select variants of above
      ಶೂನ್ಯ recv_vec4_pv(vvp_net_ptr_t ಪೋರ್ಟ್, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
			ಚಿಹ್ನೆರಹಿತ ತಳ, ಚಿಹ್ನೆರಹಿತ vwid, vvp_context_t) override;
      ಶೂನ್ಯ recv_vec8_pv(vvp_net_ptr_t ಪೋರ್ಟ್, ಸ್ಥಿರಾಂಕ vvp_vector8_t&ಬಿಟ್,
			ಚಿಹ್ನೆರಹಿತ ತಳ, ಚಿಹ್ನೆರಹಿತ vwid) override;

	// Get information about the vector value.
      ಸ್ಥಿರಾಂಕ vvp_vector4_t& vec4_unfiltered_ಮೌಲ್ಯ() ಸ್ಥಿರಾಂಕ override;

    ಖಾಸಗಿ:
      vvp_vector4_t bits4_;
};

/*
 * Automatically allocated vvp_fun_signal4.
 */
class vvp_fun_signal4_aa : ಸಾರ್ವಜನಿಕ vvp_fun_signal_vec, ಸಾರ್ವಜನಿಕ automatic_signal_ತಳ, ಸಾರ್ವಜನಿಕ automatic_hooks_s {

    ಸಾರ್ವಜನಿಕ:
      explicit vvp_fun_signal4_aa(ಚಿಹ್ನೆರಹಿತ wid, vvp_bit4_t init=BIT4_X);
      ~vvp_fun_signal4_aa() override;

      ಶೂನ್ಯ alloc_instance(vvp_context_t context) override;
      ಶೂನ್ಯ reset_instance(vvp_context_t context) override;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECK_WITH_VALGRIND
      ಶೂನ್ಯ free_instance(vvp_context_t context) override;
#ಆದರೆ_ಅಂತ್ಯ

      ಶೂನ್ಯ recv_vec4(vvp_net_ptr_t ಪೋರ್ಟ್, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
                     vvp_context_t context) override;

	// Part select variants of above
      ಶೂನ್ಯ recv_vec4_pv(vvp_net_ptr_t ಪೋರ್ಟ್, ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್,
			ಚಿಹ್ನೆರಹಿತ ತಳ, ಚಿಹ್ನೆರಹಿತ vwid, vvp_context_t) override;

	// Get information about the vector value.
      ಚಿಹ್ನೆರಹಿತ   ಮೌಲ್ಯ_ಗಾತ್ರ() ಸ್ಥಿರಾಂಕ override;
      vvp_bit4_t ಮೌಲ್ಯ(ಚಿಹ್ನೆರಹಿತ idx) ಸ್ಥಿರಾಂಕ override;
      vvp_scalar_t scalar_ಮೌಲ್ಯ(ಚಿಹ್ನೆರಹಿತ idx) ಸ್ಥಿರಾಂಕ override;
      ಶೂನ್ಯ vec4_ಮೌಲ್ಯ(vvp_vector4_t&) ಸ್ಥಿರಾಂಕ override;
      ಸ್ಥಿರಾಂಕ vvp_vector4_t& vec4_unfiltered_ಮೌಲ್ಯ() ಸ್ಥಿರಾಂಕ override;

    ಸಾರ್ವಜನಿಕ: // These objects are only permallocated.
      ಸ್ಥಿರ ಶೂನ್ಯ* ಕ್ರಿಯಾಕಾರಕ new(std::size_t ಗಾತ್ರ_2) { ಹಿಂತಿರುಗಿ vvp_net_fun_t::ರಾಶಿ.alloc(ಗಾತ್ರ_2); }
      ಸ್ಥಿರ ಶೂನ್ಯ ಕ್ರಿಯಾಕಾರಕ delete(ಶೂನ್ಯ*obj);

    ಖಾಸಗಿ:
      ಚಿಹ್ನೆರಹಿತ context_idx_;
      ಚಿಹ್ನೆರಹಿತ ಗಾತ್ರ_3;
      vvp_bit4_t init_;
};

class vvp_fun_signal_real : ಸಾರ್ವಜನಿಕ vvp_fun_signal_ತಳ {

    ಸಾರ್ವಜನಿಕ:
      explicit vvp_fun_signal_real() {};

	// Get information about the vector value.
      ವಾಸ್ತವಿಕ ದ್ವಿಗುಣ real_unfiltered_ಮೌಲ್ಯ() ಸ್ಥಿರಾಂಕ = 0;
};

/*
 * Statically allocated vvp_fun_signal_real.
 */
class vvp_fun_signal_real_sa : ಸಾರ್ವಜನಿಕ vvp_fun_signal_real {

    ಸಾರ್ವಜನಿಕ:
      explicit vvp_fun_signal_real_sa();

      ಶೂನ್ಯ recv_real(vvp_net_ptr_t ಪೋರ್ಟ್, ದ್ವಿಗುಣ ಬಿಟ್,
                     vvp_context_t) override;

	// Get information about the vector value.
      ದ್ವಿಗುಣ real_unfiltered_ಮೌಲ್ಯ() ಸ್ಥಿರಾಂಕ override;

    ಖಾಸಗಿ:
      ದ್ವಿಗುಣ ಬಿಟ್ಟುಗಳು_2;
};

/*
 * Automatically allocated vvp_fun_signal_real.
 */
class vvp_fun_signal_real_aa : ಸಾರ್ವಜನಿಕ vvp_fun_signal_real, ಸಾರ್ವಜನಿಕ automatic_signal_ತಳ, ಸಾರ್ವಜನಿಕ automatic_hooks_s {

    ಸಾರ್ವಜನಿಕ:
      explicit vvp_fun_signal_real_aa();
      ~vvp_fun_signal_real_aa() override;

      ಶೂನ್ಯ alloc_instance(vvp_context_t context) override;
      ಶೂನ್ಯ reset_instance(vvp_context_t context) override;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECK_WITH_VALGRIND
      ಶೂನ್ಯ free_instance(vvp_context_t context) override;
#ಆದರೆ_ಅಂತ್ಯ

      ಶೂನ್ಯ recv_real(vvp_net_ptr_t ಪೋರ್ಟ್, ದ್ವಿಗುಣ ಬಿಟ್,
                     vvp_context_t context) override;

	// Get information about the vector value.
      ದ್ವಿಗುಣ real_unfiltered_ಮೌಲ್ಯ() ಸ್ಥಿರಾಂಕ override;

	// Get information about the vector value.
      ಚಿಹ್ನೆರಹಿತ   ಮೌಲ್ಯ_ಗಾತ್ರ() ಸ್ಥಿರಾಂಕ override;
      vvp_bit4_t ಮೌಲ್ಯ(ಚಿಹ್ನೆರಹಿತ idx) ಸ್ಥಿರಾಂಕ override;
      vvp_scalar_t scalar_ಮೌಲ್ಯ(ಚಿಹ್ನೆರಹಿತ idx) ಸ್ಥಿರಾಂಕ override;
      ಶೂನ್ಯ vec4_ಮೌಲ್ಯ(vvp_vector4_t&) ಸ್ಥಿರಾಂಕ override;
      ದ್ವಿಗುಣ real_ಮೌಲ್ಯ() ಸ್ಥಿರಾಂಕ override;
      ಶೂನ್ಯ get_signal_ಮೌಲ್ಯ(ರಚನೆ t_vpi_ಮೌಲ್ಯ*vp) override;

    ಸಾರ್ವಜನಿಕ: // These objects are only permallocated.
      ಸ್ಥಿರ ಶೂನ್ಯ* ಕ್ರಿಯಾಕಾರಕ new(std::size_t ಗಾತ್ರ_2);
      ಸ್ಥಿರ ಶೂನ್ಯ ಕ್ರಿಯಾಕಾರಕ delete(ಶೂನ್ಯ*obj);

    ಖಾಸಗಿ:
      ಚಿಹ್ನೆರಹಿತ context_idx_;
};


class vvp_fun_signal_string : ಸಾರ್ವಜನಿಕ vvp_fun_signal_ತಳ {

    ಸಾರ್ವಜನಿಕ:
      explicit vvp_fun_signal_string() {};

      ವಾಸ್ತವಿಕ ಸ್ಥಿರಾಂಕ std::string& get_string() ಸ್ಥಿರಾಂಕ =0;
};

/*
 * Statically allocated vvp_fun_signal_string.
 */
class vvp_fun_signal_string_sa : ಸಾರ್ವಜನಿಕ vvp_fun_signal_string {

    ಸಾರ್ವಜನಿಕ:
      explicit vvp_fun_signal_string_sa();

      ಶೂನ್ಯ recv_string(vvp_net_ptr_t ಪೋರ್ಟ್, ಸ್ಥಿರಾಂಕ std::string&ಬಿಟ್,
		       vvp_context_t context) override;

      ಸ್ಥಿರಾಂಕ std::string& get_string() ಸ್ಥಿರಾಂಕ override;

    ಖಾಸಗಿ:
      std::string ಮೌಲ್ಯ_2;
};

/*
 * Automatically allocated vvp_fun_signal_real.
 */
class vvp_fun_signal_string_aa : ಸಾರ್ವಜನಿಕ vvp_fun_signal_string, ಸಾರ್ವಜನಿಕ automatic_signal_ತಳ, ಸಾರ್ವಜನಿಕ automatic_hooks_s {

    ಸಾರ್ವಜನಿಕ:
      explicit vvp_fun_signal_string_aa();
      ~vvp_fun_signal_string_aa() override;

      ಶೂನ್ಯ alloc_instance(vvp_context_t context) override;
      ಶೂನ್ಯ reset_instance(vvp_context_t context) override;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECK_WITH_VALGRIND
      ಶೂನ್ಯ free_instance(vvp_context_t context) override;
#ಆದರೆ_ಅಂತ್ಯ
      ಶೂನ್ಯ recv_string(vvp_net_ptr_t ಪೋರ್ಟ್, ಸ್ಥಿರಾಂಕ std::string&ಬಿಟ್,
		       vvp_context_t context) override;

	// Get information about the vector value.
      ಚಿಹ್ನೆರಹಿತ   ಮೌಲ್ಯ_ಗಾತ್ರ() ಸ್ಥಿರಾಂಕ override;
      vvp_bit4_t ಮೌಲ್ಯ(ಚಿಹ್ನೆರಹಿತ idx) ಸ್ಥಿರಾಂಕ override;
      vvp_scalar_t scalar_ಮೌಲ್ಯ(ಚಿಹ್ನೆರಹಿತ idx) ಸ್ಥಿರಾಂಕ override;
      ಶೂನ್ಯ vec4_ಮೌಲ್ಯ(vvp_vector4_t&) ಸ್ಥಿರಾಂಕ override;
      ದ್ವಿಗುಣ real_ಮೌಲ್ಯ() ಸ್ಥಿರಾಂಕ override;
      ಸ್ಥಿರಾಂಕ std::string& get_string() ಸ್ಥಿರಾಂಕ override;
      ಶೂನ್ಯ get_signal_ಮೌಲ್ಯ(ರಚನೆ t_vpi_ಮೌಲ್ಯ*vp) override;

    ಸಾರ್ವಜನಿಕ: // These objects are only permallocated.
      ಸ್ಥಿರ ಶೂನ್ಯ* ಕ್ರಿಯಾಕಾರಕ new(std::size_t ಗಾತ್ರ_2);
      ಸ್ಥಿರ ಶೂನ್ಯ ಕ್ರಿಯಾಕಾರಕ delete(ಶೂನ್ಯ*obj);

    ಖಾಸಗಿ:
      ಚಿಹ್ನೆರಹಿತ context_idx_;
};

class vvp_fun_signal_object : ಸಾರ್ವಜನಿಕ vvp_fun_signal_ತಳ {

    ಸಾರ್ವಜನಿಕ:
      explicit vvp_fun_signal_object(ಚಿಹ್ನೆರಹಿತ ಗಾತ್ರ_2) { ಗಾತ್ರ_3 = ಗಾತ್ರ_2; };
      ಚಿಹ್ನೆರಹಿತ ಗಾತ್ರ_2() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ಗಾತ್ರ_3; }

      ವಾಸ್ತವಿಕ vvp_object_t get_object() ಸ್ಥಿರಾಂಕ =0;
    ಖಾಸಗಿ:
      ಚಿಹ್ನೆರಹಿತ ಗಾತ್ರ_3;
};

/*
 * Statically allocated vvp_fun_signal_string.
 */
class vvp_fun_signal_object_sa : ಸಾರ್ವಜನಿಕ vvp_fun_signal_object {

    ಸಾರ್ವಜನಿಕ:
      explicit vvp_fun_signal_object_sa(ಚಿಹ್ನೆರಹಿತ ಗಾತ್ರ_2);

      ಶೂನ್ಯ recv_object(vvp_net_ptr_t ಪೋರ್ಟ್, vvp_object_t ಬಿಟ್,
		    vvp_context_t context) override;

      vvp_object_t get_object() ಸ್ಥಿರಾಂಕ override;

    ಖಾಸಗಿ:
      vvp_object_t ಮೌಲ್ಯ_2;
};

/*
 * Automatically allocated vvp_fun_signal_real.
 */
class vvp_fun_signal_object_aa : ಸಾರ್ವಜನಿಕ vvp_fun_signal_object, ಸಾರ್ವಜನಿಕ automatic_signal_ತಳ, ಸಾರ್ವಜನಿಕ automatic_hooks_s {

    ಸಾರ್ವಜನಿಕ:
      explicit vvp_fun_signal_object_aa(ಚಿಹ್ನೆರಹಿತ ಗಾತ್ರ_2);
      ~vvp_fun_signal_object_aa() override;

      ಶೂನ್ಯ alloc_instance(vvp_context_t context) override;
      ಶೂನ್ಯ reset_instance(vvp_context_t context) override;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECK_WITH_VALGRIND
      ಶೂನ್ಯ free_instance(vvp_context_t context) override;
#ಆದರೆ_ಅಂತ್ಯ

      ಶೂನ್ಯ recv_object(vvp_net_ptr_t ಪೋರ್ಟ್, vvp_object_t ಬಿಟ್,
		    vvp_context_t context) override;

	// Get information about the vector value.
      ಚಿಹ್ನೆರಹಿತ   ಮೌಲ್ಯ_ಗಾತ್ರ() ಸ್ಥಿರಾಂಕ override;
      vvp_bit4_t ಮೌಲ್ಯ(ಚಿಹ್ನೆರಹಿತ idx) ಸ್ಥಿರಾಂಕ override;
      vvp_scalar_t scalar_ಮೌಲ್ಯ(ಚಿಹ್ನೆರಹಿತ idx) ಸ್ಥಿರಾಂಕ override;
      ಶೂನ್ಯ vec4_ಮೌಲ್ಯ(vvp_vector4_t&) ಸ್ಥಿರಾಂಕ override;
	//double real_value() const;
	//void get_signal_value(struct t_vpi_value*vp);

      vvp_object_t get_object() ಸ್ಥಿರಾಂಕ override;

    ಸಾರ್ವಜನಿಕ: // These objects are only permallocated.
      ಸ್ಥಿರ ಶೂನ್ಯ* ಕ್ರಿಯಾಕಾರಕ new(std::size_t ಗಾತ್ರ_2);
      ಸ್ಥಿರ ಶೂನ್ಯ ಕ್ರಿಯಾಕಾರಕ delete(ಶೂನ್ಯ*obj);

    ಖಾಸಗಿ:
      ಚಿಹ್ನೆರಹಿತ context_idx_;
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

class vvp_wire_ತಳ  : ಸಾರ್ವಜನಿಕ vvp_net_fil_t, ಸಾರ್ವಜನಿಕ vvp_signal_ಮೌಲ್ಯ {

    ಸಾರ್ವಜನಿಕ:
      vvp_wire_ತಳ();
      ~vvp_wire_ತಳ() override;

        // Support for $countdrivers
      ವಾಸ್ತವಿಕ vvp_bit4_t driven_ಮೌಲ್ಯ(ಚಿಹ್ನೆರಹಿತ idx) ಸ್ಥಿರಾಂಕ;
      ವಾಸ್ತವಿಕ bool is_forced(ಚಿಹ್ನೆರಹಿತ idx) ಸ್ಥಿರಾಂಕ;
};

class vvp_wire_vec4 : ಸಾರ್ವಜನಿಕ vvp_wire_ತಳ {

    ಸಾರ್ವಜನಿಕ:
      vvp_wire_vec4(ಚಿಹ್ನೆರಹಿತ wid, vvp_bit4_t init);

	// The main filter behavior for this class. These methods take
	// the value that the node is driven to, and applies the force
	// filters. In wires, this also saves the driven value, so
	// that when a force is released, we can revert to the driven value.
      prop_t filter_vec4(ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್, vvp_vector4_t&rep,
			 ಚಿಹ್ನೆರಹಿತ ತಳ, ಚಿಹ್ನೆರಹಿತ vwid) override;
      prop_t filter_vec8(ಸ್ಥಿರಾಂಕ vvp_vector8_t&val, vvp_vector8_t&rep,
			 ಚಿಹ್ನೆರಹಿತ ತಳ, ಚಿಹ್ನೆರಹಿತ vwid) override;

	// Abstract methods from vvp_vpi_callback
      ಶೂನ್ಯ get_ಮೌಲ್ಯ(ರಚನೆ t_vpi_ಮೌಲ್ಯ*ಮೌಲ್ಯ) override;
	// Abstract methods from vvp_net_fit_t
      ಚಿಹ್ನೆರಹಿತ filter_ಗಾತ್ರ() ಸ್ಥಿರಾಂಕ override;
      ಶೂನ್ಯ force_fil_vec4(ಸ್ಥಿರಾಂಕ vvp_vector4_t&val, ಸ್ಥಿರಾಂಕ vvp_vector2_t&ಮುಖವಾಡ) override;
      ಶೂನ್ಯ force_fil_vec8(ಸ್ಥಿರಾಂಕ vvp_vector8_t&val, ಸ್ಥಿರಾಂಕ vvp_vector2_t&ಮುಖವಾಡ) override;
      ಶೂನ್ಯ force_fil_real(ದ್ವಿಗುಣ val, ಸ್ಥಿರಾಂಕ vvp_vector2_t&ಮುಖವಾಡ) override;
      ಶೂನ್ಯ release(vvp_net_ptr_t ptr, bool net_flag) override;
      ಶೂನ್ಯ release_pv(vvp_net_ptr_t ptr, ಚಿಹ್ನೆರಹಿತ ತಳ, ಚಿಹ್ನೆರಹಿತ wid, bool net_flag) override;

	// Implementation of vvp_signal_value methods
      ಚಿಹ್ನೆರಹಿತ ಮೌಲ್ಯ_ಗಾತ್ರ() ಸ್ಥಿರಾಂಕ override;
      vvp_bit4_t ಮೌಲ್ಯ(ಚಿಹ್ನೆರಹಿತ idx) ಸ್ಥಿರಾಂಕ override;
      vvp_scalar_t scalar_ಮೌಲ್ಯ(ಚಿಹ್ನೆರಹಿತ idx) ಸ್ಥಿರಾಂಕ override;
      ಶೂನ್ಯ vec4_ಮೌಲ್ಯ(vvp_vector4_t&) ಸ್ಥಿರಾಂಕ override;

        // Support for $countdrivers
      vvp_bit4_t driven_ಮೌಲ್ಯ(ಚಿಹ್ನೆರಹಿತ idx) ಸ್ಥಿರಾಂಕ override;
      bool is_forced(ಚಿಹ್ನೆರಹಿತ idx) ಸ್ಥಿರಾಂಕ override;

    ಖಾಸಗಿ:
      vvp_bit4_t filtered_ಮೌಲ್ಯ(ಚಿಹ್ನೆರಹಿತ idx) ಸ್ಥಿರಾಂಕ;

    ಖಾಸಗಿ:
      bool needs_init_;
      vvp_vector4_t bits4_; // The tracked driven value
      vvp_vector4_t force4_; // the value being forced
};

class vvp_wire_vec8 : ಸಾರ್ವಜನಿಕ vvp_wire_ತಳ {

    ಸಾರ್ವಜನಿಕ:
      explicit vvp_wire_vec8(ಚಿಹ್ನೆರಹಿತ wid);

	// The main filter behavior for this class
      prop_t filter_vec4(ಸ್ಥಿರಾಂಕ vvp_vector4_t&ಬಿಟ್, vvp_vector4_t&rep,
			 ಚಿಹ್ನೆರಹಿತ ತಳ, ಚಿಹ್ನೆರಹಿತ vwid) override;
      prop_t filter_vec8(ಸ್ಥಿರಾಂಕ vvp_vector8_t&val, vvp_vector8_t&rep,
			 ಚಿಹ್ನೆರಹಿತ ತಳ, ಚಿಹ್ನೆರಹಿತ vwid) override;

	// island ports use this method to filter arbitrary values
	// through the force filter.
      prop_t filter_ಒಳಹರಿವು_vec8(ಸ್ಥಿರಾಂಕ vvp_vector8_t&val, vvp_vector8_t&rep) ಸ್ಥಿರಾಂಕ;


	// Abstract methods from vvp_vpi_callback
      ಶೂನ್ಯ get_ಮೌಲ್ಯ(ರಚನೆ t_vpi_ಮೌಲ್ಯ*ಮೌಲ್ಯ) override;
	// Abstract methods from vvp_net_fit_t
      ಚಿಹ್ನೆರಹಿತ filter_ಗಾತ್ರ() ಸ್ಥಿರಾಂಕ override;
      ಶೂನ್ಯ force_fil_vec4(ಸ್ಥಿರಾಂಕ vvp_vector4_t&val, ಸ್ಥಿರಾಂಕ vvp_vector2_t&ಮುಖವಾಡ) override;
      ಶೂನ್ಯ force_fil_vec8(ಸ್ಥಿರಾಂಕ vvp_vector8_t&val, ಸ್ಥಿರಾಂಕ vvp_vector2_t&ಮುಖವಾಡ) override;
      ಶೂನ್ಯ force_fil_real(ದ್ವಿಗುಣ val, ಸ್ಥಿರಾಂಕ vvp_vector2_t&ಮುಖವಾಡ) override;
      ಶೂನ್ಯ release(vvp_net_ptr_t ptr, bool net_flag) override;
      ಶೂನ್ಯ release_pv(vvp_net_ptr_t ptr, ಚಿಹ್ನೆರಹಿತ ತಳ, ಚಿಹ್ನೆರಹಿತ wid, bool net_flag) override;

	// Implementation of vvp_signal_value methods
      ಚಿಹ್ನೆರಹಿತ ಮೌಲ್ಯ_ಗಾತ್ರ() ಸ್ಥಿರಾಂಕ override;
      vvp_bit4_t ಮೌಲ್ಯ(ಚಿಹ್ನೆರಹಿತ idx) ಸ್ಥಿರಾಂಕ override;
      vvp_scalar_t scalar_ಮೌಲ್ಯ(ಚಿಹ್ನೆರಹಿತ idx) ಸ್ಥಿರಾಂಕ override;
      ಶೂನ್ಯ vec4_ಮೌಲ್ಯ(vvp_vector4_t&) ಸ್ಥಿರಾಂಕ override;
	// This is new to vvp_wire_vec8
      vvp_vector8_t vec8_ಮೌಲ್ಯ() ಸ್ಥಿರಾಂಕ;

        // Support for $countdrivers
      vvp_bit4_t driven_ಮೌಲ್ಯ(ಚಿಹ್ನೆರಹಿತ idx) ಸ್ಥಿರಾಂಕ override;
      bool is_forced(ಚಿಹ್ನೆರಹಿತ idx) ಸ್ಥಿರಾಂಕ override;

    ಖಾಸಗಿ:
      vvp_scalar_t filtered_ಮೌಲ್ಯ(ಚಿಹ್ನೆರಹಿತ idx) ಸ್ಥಿರಾಂಕ;

    ಖಾಸಗಿ:
      bool needs_init_;
      vvp_vector8_t bits8_;
      vvp_vector8_t force8_; // the value being forced
};

class vvp_wire_real : ಸಾರ್ವಜನಿಕ vvp_wire_ತಳ {

    ಸಾರ್ವಜನಿಕ:
      explicit vvp_wire_real(ಶೂನ್ಯ);

	// The main filter behavior for this class
      prop_t filter_real(ದ್ವಿಗುಣ&ಬಿಟ್) override;

	// Abstract methods from vvp_vpi_callback
      ಶೂನ್ಯ get_ಮೌಲ್ಯ(ರಚನೆ t_vpi_ಮೌಲ್ಯ*ಮೌಲ್ಯ) override;
	// Abstract methods from vvp_net_fit_t
      ಚಿಹ್ನೆರಹಿತ filter_ಗಾತ್ರ() ಸ್ಥಿರಾಂಕ override;
      ಶೂನ್ಯ force_fil_vec4(ಸ್ಥಿರಾಂಕ vvp_vector4_t&val, ಸ್ಥಿರಾಂಕ vvp_vector2_t&ಮುಖವಾಡ) override;
      ಶೂನ್ಯ force_fil_vec8(ಸ್ಥಿರಾಂಕ vvp_vector8_t&val, ಸ್ಥಿರಾಂಕ vvp_vector2_t&ಮುಖವಾಡ) override;
      ಶೂನ್ಯ force_fil_real(ದ್ವಿಗುಣ val, ಸ್ಥಿರಾಂಕ vvp_vector2_t&ಮುಖವಾಡ) override;
      ಶೂನ್ಯ release(vvp_net_ptr_t ptr, bool net_flag) override;
      ಶೂನ್ಯ release_pv(vvp_net_ptr_t ptr, ಚಿಹ್ನೆರಹಿತ ತಳ, ಚಿಹ್ನೆರಹಿತ wid, bool net_flag) override;

	// Implementation of vvp_signal_value methods
      ಚಿಹ್ನೆರಹಿತ ಮೌಲ್ಯ_ಗಾತ್ರ() ಸ್ಥಿರಾಂಕ override;
      vvp_bit4_t ಮೌಲ್ಯ(ಚಿಹ್ನೆರಹಿತ idx) ಸ್ಥಿರಾಂಕ override;
      vvp_scalar_t scalar_ಮೌಲ್ಯ(ಚಿಹ್ನೆರಹಿತ idx) ಸ್ಥಿರಾಂಕ override;
      ಶೂನ್ಯ vec4_ಮೌಲ್ಯ(vvp_vector4_t&) ಸ್ಥಿರಾಂಕ override;
      ದ್ವಿಗುಣ real_ಮೌಲ್ಯ() ಸ್ಥಿರಾಂಕ override;

      ಶೂನ್ಯ get_signal_ಮೌಲ್ಯ(ರಚನೆ t_vpi_ಮೌಲ್ಯ*vp) override;

    ಖಾಸಗಿ:
      ದ್ವಿಗುಣ ಬಿಟ್_2;
      ದ್ವಿಗುಣ force_;
      bool needs_init_ = true;
};

#ಆದರೆ 0
class vvp_wire_string : ಸಾರ್ವಜನಿಕ vvp_wire_ತಳ {

    ಸಾರ್ವಜನಿಕ:
      explicit vvp_wire_string(ಶೂನ್ಯ);

	// Abstract methods from vvp_vpi_callback
      ಶೂನ್ಯ get_ಮೌಲ್ಯ(ರಚನೆ t_vpi_ಮೌಲ್ಯ*ಮೌಲ್ಯ);
	// Abstract methods from vvp_net_fil_t
      ಚಿಹ್ನೆರಹಿತ filter_ಗಾತ್ರ() ಸ್ಥಿರಾಂಕ;
      ಶೂನ್ಯ force_fil_vec4(ಸ್ಥಿರಾಂಕ vvp_vector4_t&val, ಸ್ಥಿರಾಂಕ vvp_vector2_t&ಮುಖವಾಡ);
      ಶೂನ್ಯ force_fil_vec8(ಸ್ಥಿರಾಂಕ vvp_vector8_t&val, ಸ್ಥಿರಾಂಕ vvp_vector2_t&ಮುಖವಾಡ);
      ಶೂನ್ಯ force_fil_real(ದ್ವಿಗುಣ val, ಸ್ಥಿರಾಂಕ vvp_vector2_t&ಮುಖವಾಡ);
      ಶೂನ್ಯ release(vvp_net_ptr_t ptr, bool net_flag);
      ಶೂನ್ಯ release_pv(vvp_net_ptr_t ptr, ಚಿಹ್ನೆರಹಿತ ತಳ, ಚಿಹ್ನೆರಹಿತ wid, bool net_flag);

	// Implementation of vvp_signal_value methods
      ಚಿಹ್ನೆರಹಿತ ಮೌಲ್ಯ_ಗಾತ್ರ() ಸ್ಥಿರಾಂಕ;
      vvp_bit4_t ಮೌಲ್ಯ(ಚಿಹ್ನೆರಹಿತ idx) ಸ್ಥಿರಾಂಕ;
      vvp_scalar_t scalar_ಮೌಲ್ಯ(ಚಿಹ್ನೆರಹಿತ idx) ಸ್ಥಿರಾಂಕ;
      ಶೂನ್ಯ vec4_ಮೌಲ್ಯ(vvp_vector4_t&) ಸ್ಥಿರಾಂಕ;
      ದ್ವಿಗುಣ real_ಮೌಲ್ಯ() ಸ್ಥಿರಾಂಕ;

      ಶೂನ್ಯ get_signal_ಮೌಲ್ಯ(ರಚನೆ t_vpi_ಮೌಲ್ಯ*vp);

    ಖಾಸಗಿ:
      std::string ಮೌಲ್ಯ_2;
};
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ_ಅಂತ್ಯ /* IVL_vvp_net_sig_H */
