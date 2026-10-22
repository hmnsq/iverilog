#اگر_غیر_تعریف_شدہ IVL_vvp_net_sig_H
#تعریف_کرو IVL_vvp_net_sig_H
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

#اگر_تعریف_شدہ HAVE_IOSFWD
# include  <iosfwd>
#ورنہ
# include  <iostream>
#اگر_ختم

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


class vvp_fun_signal_بنیاد : عوامی vvp_net_fun_t {

    عوامی:
      vvp_fun_signal_بنیاد();

      خالی deassign();
      خالی deassign_pv(غیرعلامتی بنیاد, غیرعلامتی wid);

    عوامی:

	/* The %cassign/link instruction needs a place to write the
	   source node of the force, so that subsequent %cassign and
	   %deassign instructions can undo the link as needed. */
      class vvp_net_t*cassign_link;

    محفوظ:
      bool continuous_تفویض_فعال;
      vvp_vector2_t تفویض_نقاب;

    محفوظ:
	// This is true until at least one propagation happens.
      bool needs_init_;
};

/*
 * Variables and wires can have their values accessed, so this base
 * class offers the unified concept of an accessible value.
 */
class vvp_signal_قدر {
    عوامی:
      مجازی ~vvp_signal_قدر() =0;
      مجازی غیرعلامتی قدر_سائز() ثابت =0;
      مجازی vvp_bit4_t قدر(غیرعلامتی idx) ثابت =0;
      مجازی vvp_scalar_t scalar_قدر(غیرعلامتی idx) ثابت =0;
      مجازی خالی vec4_قدر(vvp_vector4_t&) ثابت =0;
      مجازی دوہرا real_قدر() ثابت;

      مجازی خالی get_signal_قدر(ساخت t_vpi_قدر*vp);
};

/*
 * This abstract class is a little more specific than the signal_base
 * class, in that it adds vector access methods.
 */
class vvp_fun_signal_vec : عوامی vvp_fun_signal_بنیاد {
    عوامی:
      مجازی ثابت vvp_vector4_t& vec4_unfiltered_قدر() ثابت =0;
};

class automatic_signal_بنیاد : عوامی vvp_signal_قدر, عوامی vvp_net_fil_t {

    عوامی:
	// Automatic variables cannot be forced or released. Provide
	// stubs that assert.
      مجازی خالی release(vvp_net_ptr_t ptr, bool net_flag) override;
      مجازی خالی release_pv(vvp_net_ptr_t ptr, غیرعلامتی بنیاد, غیرعلامتی wid, bool net_flag) override;

      مجازی غیرعلامتی filter_سائز() ثابت override;
      مجازی خالی force_fil_vec4(ثابت vvp_vector4_t&val, ثابت vvp_vector2_t&نقاب) override;
      مجازی خالی force_fil_vec8(ثابت vvp_vector8_t&val, ثابت vvp_vector2_t&نقاب) override;
      مجازی خالی force_fil_real(دوہرا val, ثابت vvp_vector2_t&نقاب) override;
      مجازی خالی get_قدر(ساخت t_vpi_قدر*قدر) override;
};

/*
 * Statically allocated vvp_fun_signal4.
 */
class vvp_fun_signal4_sa : عوامی vvp_fun_signal_vec {

    عوامی:
      explicit vvp_fun_signal4_sa(غیرعلامتی wid, vvp_bit4_t init=BIT4_X);

      خالی recv_vec4(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                     vvp_context_t) override;
      خالی recv_vec8(vvp_net_ptr_t پورٹ, ثابت vvp_vector8_t&ثنائی_ہندسہ) override;

	// Part select variants of above
      خالی recv_vec4_pv(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
			غیرعلامتی بنیاد, غیرعلامتی vwid, vvp_context_t) override;
      خالی recv_vec8_pv(vvp_net_ptr_t پورٹ, ثابت vvp_vector8_t&ثنائی_ہندسہ,
			غیرعلامتی بنیاد, غیرعلامتی vwid) override;

	// Get information about the vector value.
      ثابت vvp_vector4_t& vec4_unfiltered_قدر() ثابت override;

    نجی:
      vvp_vector4_t bits4_;
};

/*
 * Automatically allocated vvp_fun_signal4.
 */
class vvp_fun_signal4_aa : عوامی vvp_fun_signal_vec, عوامی automatic_signal_بنیاد, عوامی automatic_hooks_s {

    عوامی:
      explicit vvp_fun_signal4_aa(غیرعلامتی wid, vvp_bit4_t init=BIT4_X);
      ~vvp_fun_signal4_aa() override;

      خالی alloc_instance(vvp_context_t context) override;
      خالی reset_instance(vvp_context_t context) override;
#اگر_تعریف_شدہ CHECK_WITH_VALGRIND
      خالی free_instance(vvp_context_t context) override;
#اگر_ختم

      خالی recv_vec4(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                     vvp_context_t context) override;

	// Part select variants of above
      خالی recv_vec4_pv(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
			غیرعلامتی بنیاد, غیرعلامتی vwid, vvp_context_t) override;

	// Get information about the vector value.
      غیرعلامتی   قدر_سائز() ثابت override;
      vvp_bit4_t قدر(غیرعلامتی idx) ثابت override;
      vvp_scalar_t scalar_قدر(غیرعلامتی idx) ثابت override;
      خالی vec4_قدر(vvp_vector4_t&) ثابت override;
      ثابت vvp_vector4_t& vec4_unfiltered_قدر() ثابت override;

    عوامی: // These objects are only permallocated.
      ساکن خالی* عامل new(std::size_t سائز) { واپس vvp_net_fun_t::ڈھیر.alloc(سائز); }
      ساکن خالی عامل delete(خالی*obj);

    نجی:
      غیرعلامتی context_idx_;
      غیرعلامتی سائز_2;
      vvp_bit4_t init_;
};

class vvp_fun_signal_real : عوامی vvp_fun_signal_بنیاد {

    عوامی:
      explicit vvp_fun_signal_real() {};

	// Get information about the vector value.
      مجازی دوہرا real_unfiltered_قدر() ثابت = 0;
};

/*
 * Statically allocated vvp_fun_signal_real.
 */
class vvp_fun_signal_real_sa : عوامی vvp_fun_signal_real {

    عوامی:
      explicit vvp_fun_signal_real_sa();

      خالی recv_real(vvp_net_ptr_t پورٹ, دوہرا ثنائی_ہندسہ,
                     vvp_context_t) override;

	// Get information about the vector value.
      دوہرا real_unfiltered_قدر() ثابت override;

    نجی:
      دوہرا بٹ_2;
};

/*
 * Automatically allocated vvp_fun_signal_real.
 */
class vvp_fun_signal_real_aa : عوامی vvp_fun_signal_real, عوامی automatic_signal_بنیاد, عوامی automatic_hooks_s {

    عوامی:
      explicit vvp_fun_signal_real_aa();
      ~vvp_fun_signal_real_aa() override;

      خالی alloc_instance(vvp_context_t context) override;
      خالی reset_instance(vvp_context_t context) override;
#اگر_تعریف_شدہ CHECK_WITH_VALGRIND
      خالی free_instance(vvp_context_t context) override;
#اگر_ختم

      خالی recv_real(vvp_net_ptr_t پورٹ, دوہرا ثنائی_ہندسہ,
                     vvp_context_t context) override;

	// Get information about the vector value.
      دوہرا real_unfiltered_قدر() ثابت override;

	// Get information about the vector value.
      غیرعلامتی   قدر_سائز() ثابت override;
      vvp_bit4_t قدر(غیرعلامتی idx) ثابت override;
      vvp_scalar_t scalar_قدر(غیرعلامتی idx) ثابت override;
      خالی vec4_قدر(vvp_vector4_t&) ثابت override;
      دوہرا real_قدر() ثابت override;
      خالی get_signal_قدر(ساخت t_vpi_قدر*vp) override;

    عوامی: // These objects are only permallocated.
      ساکن خالی* عامل new(std::size_t سائز);
      ساکن خالی عامل delete(خالی*obj);

    نجی:
      غیرعلامتی context_idx_;
};


class vvp_fun_signal_string : عوامی vvp_fun_signal_بنیاد {

    عوامی:
      explicit vvp_fun_signal_string() {};

      مجازی ثابت std::string& get_string() ثابت =0;
};

/*
 * Statically allocated vvp_fun_signal_string.
 */
class vvp_fun_signal_string_sa : عوامی vvp_fun_signal_string {

    عوامی:
      explicit vvp_fun_signal_string_sa();

      خالی recv_string(vvp_net_ptr_t پورٹ, ثابت std::string&ثنائی_ہندسہ,
		       vvp_context_t context) override;

      ثابت std::string& get_string() ثابت override;

    نجی:
      std::string قدر_2;
};

/*
 * Automatically allocated vvp_fun_signal_real.
 */
class vvp_fun_signal_string_aa : عوامی vvp_fun_signal_string, عوامی automatic_signal_بنیاد, عوامی automatic_hooks_s {

    عوامی:
      explicit vvp_fun_signal_string_aa();
      ~vvp_fun_signal_string_aa() override;

      خالی alloc_instance(vvp_context_t context) override;
      خالی reset_instance(vvp_context_t context) override;
#اگر_تعریف_شدہ CHECK_WITH_VALGRIND
      خالی free_instance(vvp_context_t context) override;
#اگر_ختم
      خالی recv_string(vvp_net_ptr_t پورٹ, ثابت std::string&ثنائی_ہندسہ,
		       vvp_context_t context) override;

	// Get information about the vector value.
      غیرعلامتی   قدر_سائز() ثابت override;
      vvp_bit4_t قدر(غیرعلامتی idx) ثابت override;
      vvp_scalar_t scalar_قدر(غیرعلامتی idx) ثابت override;
      خالی vec4_قدر(vvp_vector4_t&) ثابت override;
      دوہرا real_قدر() ثابت override;
      ثابت std::string& get_string() ثابت override;
      خالی get_signal_قدر(ساخت t_vpi_قدر*vp) override;

    عوامی: // These objects are only permallocated.
      ساکن خالی* عامل new(std::size_t سائز);
      ساکن خالی عامل delete(خالی*obj);

    نجی:
      غیرعلامتی context_idx_;
};

class vvp_fun_signal_object : عوامی vvp_fun_signal_بنیاد {

    عوامی:
      explicit vvp_fun_signal_object(غیرعلامتی سائز) { سائز_2 = سائز; };
      غیرعلامتی سائز() ثابت { واپس سائز_2; }

      مجازی vvp_object_t get_object() ثابت =0;
    نجی:
      غیرعلامتی سائز_2;
};

/*
 * Statically allocated vvp_fun_signal_string.
 */
class vvp_fun_signal_object_sa : عوامی vvp_fun_signal_object {

    عوامی:
      explicit vvp_fun_signal_object_sa(غیرعلامتی سائز);

      خالی recv_object(vvp_net_ptr_t پورٹ, vvp_object_t ثنائی_ہندسہ,
		    vvp_context_t context) override;

      vvp_object_t get_object() ثابت override;

    نجی:
      vvp_object_t قدر_2;
};

/*
 * Automatically allocated vvp_fun_signal_real.
 */
class vvp_fun_signal_object_aa : عوامی vvp_fun_signal_object, عوامی automatic_signal_بنیاد, عوامی automatic_hooks_s {

    عوامی:
      explicit vvp_fun_signal_object_aa(غیرعلامتی سائز);
      ~vvp_fun_signal_object_aa() override;

      خالی alloc_instance(vvp_context_t context) override;
      خالی reset_instance(vvp_context_t context) override;
#اگر_تعریف_شدہ CHECK_WITH_VALGRIND
      خالی free_instance(vvp_context_t context) override;
#اگر_ختم

      خالی recv_object(vvp_net_ptr_t پورٹ, vvp_object_t ثنائی_ہندسہ,
		    vvp_context_t context) override;

	// Get information about the vector value.
      غیرعلامتی   قدر_سائز() ثابت override;
      vvp_bit4_t قدر(غیرعلامتی idx) ثابت override;
      vvp_scalar_t scalar_قدر(غیرعلامتی idx) ثابت override;
      خالی vec4_قدر(vvp_vector4_t&) ثابت override;
	//double real_value() const;
	//void get_signal_value(struct t_vpi_value*vp);

      vvp_object_t get_object() ثابت override;

    عوامی: // These objects are only permallocated.
      ساکن خالی* عامل new(std::size_t سائز);
      ساکن خالی عامل delete(خالی*obj);

    نجی:
      غیرعلامتی context_idx_;
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

class vvp_wire_بنیاد  : عوامی vvp_net_fil_t, عوامی vvp_signal_قدر {

    عوامی:
      vvp_wire_بنیاد();
      ~vvp_wire_بنیاد() override;

        // Support for $countdrivers
      مجازی vvp_bit4_t driven_قدر(غیرعلامتی idx) ثابت;
      مجازی bool is_forced(غیرعلامتی idx) ثابت;
};

class vvp_wire_vec4 : عوامی vvp_wire_بنیاد {

    عوامی:
      vvp_wire_vec4(غیرعلامتی wid, vvp_bit4_t init);

	// The main filter behavior for this class. These methods take
	// the value that the node is driven to, and applies the force
	// filters. In wires, this also saves the driven value, so
	// that when a force is released, we can revert to the driven value.
      prop_t filter_vec4(ثابت vvp_vector4_t&ثنائی_ہندسہ, vvp_vector4_t&rep,
			 غیرعلامتی بنیاد, غیرعلامتی vwid) override;
      prop_t filter_vec8(ثابت vvp_vector8_t&val, vvp_vector8_t&rep,
			 غیرعلامتی بنیاد, غیرعلامتی vwid) override;

	// Abstract methods from vvp_vpi_callback
      خالی get_قدر(ساخت t_vpi_قدر*قدر) override;
	// Abstract methods from vvp_net_fit_t
      غیرعلامتی filter_سائز() ثابت override;
      خالی force_fil_vec4(ثابت vvp_vector4_t&val, ثابت vvp_vector2_t&نقاب) override;
      خالی force_fil_vec8(ثابت vvp_vector8_t&val, ثابت vvp_vector2_t&نقاب) override;
      خالی force_fil_real(دوہرا val, ثابت vvp_vector2_t&نقاب) override;
      خالی release(vvp_net_ptr_t ptr, bool net_flag) override;
      خالی release_pv(vvp_net_ptr_t ptr, غیرعلامتی بنیاد, غیرعلامتی wid, bool net_flag) override;

	// Implementation of vvp_signal_value methods
      غیرعلامتی قدر_سائز() ثابت override;
      vvp_bit4_t قدر(غیرعلامتی idx) ثابت override;
      vvp_scalar_t scalar_قدر(غیرعلامتی idx) ثابت override;
      خالی vec4_قدر(vvp_vector4_t&) ثابت override;

        // Support for $countdrivers
      vvp_bit4_t driven_قدر(غیرعلامتی idx) ثابت override;
      bool is_forced(غیرعلامتی idx) ثابت override;

    نجی:
      vvp_bit4_t filtered_قدر(غیرعلامتی idx) ثابت;

    نجی:
      bool needs_init_;
      vvp_vector4_t bits4_; // The tracked driven value
      vvp_vector4_t force4_; // the value being forced
};

class vvp_wire_vec8 : عوامی vvp_wire_بنیاد {

    عوامی:
      explicit vvp_wire_vec8(غیرعلامتی wid);

	// The main filter behavior for this class
      prop_t filter_vec4(ثابت vvp_vector4_t&ثنائی_ہندسہ, vvp_vector4_t&rep,
			 غیرعلامتی بنیاد, غیرعلامتی vwid) override;
      prop_t filter_vec8(ثابت vvp_vector8_t&val, vvp_vector8_t&rep,
			 غیرعلامتی بنیاد, غیرعلامتی vwid) override;

	// island ports use this method to filter arbitrary values
	// through the force filter.
      prop_t filter_داخلہ_vec8(ثابت vvp_vector8_t&val, vvp_vector8_t&rep) ثابت;


	// Abstract methods from vvp_vpi_callback
      خالی get_قدر(ساخت t_vpi_قدر*قدر) override;
	// Abstract methods from vvp_net_fit_t
      غیرعلامتی filter_سائز() ثابت override;
      خالی force_fil_vec4(ثابت vvp_vector4_t&val, ثابت vvp_vector2_t&نقاب) override;
      خالی force_fil_vec8(ثابت vvp_vector8_t&val, ثابت vvp_vector2_t&نقاب) override;
      خالی force_fil_real(دوہرا val, ثابت vvp_vector2_t&نقاب) override;
      خالی release(vvp_net_ptr_t ptr, bool net_flag) override;
      خالی release_pv(vvp_net_ptr_t ptr, غیرعلامتی بنیاد, غیرعلامتی wid, bool net_flag) override;

	// Implementation of vvp_signal_value methods
      غیرعلامتی قدر_سائز() ثابت override;
      vvp_bit4_t قدر(غیرعلامتی idx) ثابت override;
      vvp_scalar_t scalar_قدر(غیرعلامتی idx) ثابت override;
      خالی vec4_قدر(vvp_vector4_t&) ثابت override;
	// This is new to vvp_wire_vec8
      vvp_vector8_t vec8_قدر() ثابت;

        // Support for $countdrivers
      vvp_bit4_t driven_قدر(غیرعلامتی idx) ثابت override;
      bool is_forced(غیرعلامتی idx) ثابت override;

    نجی:
      vvp_scalar_t filtered_قدر(غیرعلامتی idx) ثابت;

    نجی:
      bool needs_init_;
      vvp_vector8_t bits8_;
      vvp_vector8_t force8_; // the value being forced
};

class vvp_wire_real : عوامی vvp_wire_بنیاد {

    عوامی:
      explicit vvp_wire_real(خالی);

	// The main filter behavior for this class
      prop_t filter_real(دوہرا&ثنائی_ہندسہ) override;

	// Abstract methods from vvp_vpi_callback
      خالی get_قدر(ساخت t_vpi_قدر*قدر) override;
	// Abstract methods from vvp_net_fit_t
      غیرعلامتی filter_سائز() ثابت override;
      خالی force_fil_vec4(ثابت vvp_vector4_t&val, ثابت vvp_vector2_t&نقاب) override;
      خالی force_fil_vec8(ثابت vvp_vector8_t&val, ثابت vvp_vector2_t&نقاب) override;
      خالی force_fil_real(دوہرا val, ثابت vvp_vector2_t&نقاب) override;
      خالی release(vvp_net_ptr_t ptr, bool net_flag) override;
      خالی release_pv(vvp_net_ptr_t ptr, غیرعلامتی بنیاد, غیرعلامتی wid, bool net_flag) override;

	// Implementation of vvp_signal_value methods
      غیرعلامتی قدر_سائز() ثابت override;
      vvp_bit4_t قدر(غیرعلامتی idx) ثابت override;
      vvp_scalar_t scalar_قدر(غیرعلامتی idx) ثابت override;
      خالی vec4_قدر(vvp_vector4_t&) ثابت override;
      دوہرا real_قدر() ثابت override;

      خالی get_signal_قدر(ساخت t_vpi_قدر*vp) override;

    نجی:
      دوہرا ثنائی_ہندسہ_2;
      دوہرا force_;
      bool needs_init_ = true;
};

#اگر 0
class vvp_wire_string : عوامی vvp_wire_بنیاد {

    عوامی:
      explicit vvp_wire_string(خالی);

	// Abstract methods from vvp_vpi_callback
      خالی get_قدر(ساخت t_vpi_قدر*قدر);
	// Abstract methods from vvp_net_fil_t
      غیرعلامتی filter_سائز() ثابت;
      خالی force_fil_vec4(ثابت vvp_vector4_t&val, ثابت vvp_vector2_t&نقاب);
      خالی force_fil_vec8(ثابت vvp_vector8_t&val, ثابت vvp_vector2_t&نقاب);
      خالی force_fil_real(دوہرا val, ثابت vvp_vector2_t&نقاب);
      خالی release(vvp_net_ptr_t ptr, bool net_flag);
      خالی release_pv(vvp_net_ptr_t ptr, غیرعلامتی بنیاد, غیرعلامتی wid, bool net_flag);

	// Implementation of vvp_signal_value methods
      غیرعلامتی قدر_سائز() ثابت;
      vvp_bit4_t قدر(غیرعلامتی idx) ثابت;
      vvp_scalar_t scalar_قدر(غیرعلامتی idx) ثابت;
      خالی vec4_قدر(vvp_vector4_t&) ثابت;
      دوہرا real_قدر() ثابت;

      خالی get_signal_قدر(ساخت t_vpi_قدر*vp);

    نجی:
      std::string قدر_2;
};
#اگر_ختم

#اگر_ختم /* IVL_vvp_net_sig_H */
