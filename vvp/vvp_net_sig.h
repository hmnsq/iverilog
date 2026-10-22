#جے_بنا_تعریف IVL_vvp_net_sig_H
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

#جے_تعریف_شدہ HAVE_IOSFWD
# include  <iosfwd>
#وإلا
# include  <iostream>
#جے_انت

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


class vvp_fun_signal_ادھار : عام vvp_net_fun_t {

    عام:
      vvp_fun_signal_ادھار();

      فراغ deassign();
      فراغ deassign_pv(بنانشانی ادھار, بنانشانی wid);

    عام:

	/* The %cassign/link instruction needs a place to write the
	   source node of the force, so that subsequent %cassign and
	   %deassign instructions can undo the link as needed. */
      class vvp_net_t*cassign_link;

    محمي:
      bool continuous_سونپو_سرگرم;
      vvp_vector2_t سونپو_نقاب;

    محمي:
	// This is true until at least one propagation happens.
      bool needs_init_;
};

/*
 * Variables and wires can have their values accessed, so this base
 * class offers the unified concept of an accessible value.
 */
class vvp_signal_قدر {
    عام:
      مجازی ~vvp_signal_قدر() =0;
      مجازی بنانشانی قدر_ناپ() ثابت =0;
      مجازی vvp_bit4_t قدر(بنانشانی idx) ثابت =0;
      مجازی vvp_scalar_t scalar_قدر(بنانشانی idx) ثابت =0;
      مجازی فراغ vec4_قدر(vvp_vector4_t&) ثابت =0;
      مجازی مزدوج real_قدر() ثابت;

      مجازی فراغ get_signal_قدر(بنية t_vpi_قدر*vp);
};

/*
 * This abstract class is a little more specific than the signal_base
 * class, in that it adds vector access methods.
 */
class vvp_fun_signal_vec : عام vvp_fun_signal_ادھار {
    عام:
      مجازی ثابت vvp_vector4_t& vec4_unfiltered_قدر() ثابت =0;
};

class automatic_signal_ادھار : عام vvp_signal_قدر, عام vvp_net_fil_t {

    عام:
	// Automatic variables cannot be forced or released. Provide
	// stubs that assert.
      مجازی فراغ release(vvp_net_ptr_t ptr, bool net_flag) override;
      مجازی فراغ release_pv(vvp_net_ptr_t ptr, بنانشانی ادھار, بنانشانی wid, bool net_flag) override;

      مجازی بنانشانی filter_ناپ() ثابت override;
      مجازی فراغ force_fil_vec4(ثابت vvp_vector4_t&val, ثابت vvp_vector2_t&نقاب) override;
      مجازی فراغ force_fil_vec8(ثابت vvp_vector8_t&val, ثابت vvp_vector2_t&نقاب) override;
      مجازی فراغ force_fil_real(مزدوج val, ثابت vvp_vector2_t&نقاب) override;
      مجازی فراغ get_قدر(بنية t_vpi_قدر*قدر) override;
};

/*
 * Statically allocated vvp_fun_signal4.
 */
class vvp_fun_signal4_sa : عام vvp_fun_signal_vec {

    عام:
      explicit vvp_fun_signal4_sa(بنانشانی wid, vvp_bit4_t init=BIT4_X);

      فراغ recv_vec4(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                     vvp_context_t) override;
      فراغ recv_vec8(vvp_net_ptr_t پورٹ, ثابت vvp_vector8_t&ثنائی_ہندسہ) override;

	// Part select variants of above
      فراغ recv_vec4_pv(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
			بنانشانی ادھار, بنانشانی vwid, vvp_context_t) override;
      فراغ recv_vec8_pv(vvp_net_ptr_t پورٹ, ثابت vvp_vector8_t&ثنائی_ہندسہ,
			بنانشانی ادھار, بنانشانی vwid) override;

	// Get information about the vector value.
      ثابت vvp_vector4_t& vec4_unfiltered_قدر() ثابت override;

    خاص:
      vvp_vector4_t bits4_;
};

/*
 * Automatically allocated vvp_fun_signal4.
 */
class vvp_fun_signal4_aa : عام vvp_fun_signal_vec, عام automatic_signal_ادھار, عام automatic_hooks_s {

    عام:
      explicit vvp_fun_signal4_aa(بنانشانی wid, vvp_bit4_t init=BIT4_X);
      ~vvp_fun_signal4_aa() override;

      فراغ alloc_instance(vvp_context_t context) override;
      فراغ reset_instance(vvp_context_t context) override;
#جے_تعریف_شدہ CHECK_WITH_VALGRIND
      فراغ free_instance(vvp_context_t context) override;
#جے_انت

      فراغ recv_vec4(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                     vvp_context_t context) override;

	// Part select variants of above
      فراغ recv_vec4_pv(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
			بنانشانی ادھار, بنانشانی vwid, vvp_context_t) override;

	// Get information about the vector value.
      بنانشانی   قدر_ناپ() ثابت override;
      vvp_bit4_t قدر(بنانشانی idx) ثابت override;
      vvp_scalar_t scalar_قدر(بنانشانی idx) ثابت override;
      فراغ vec4_قدر(vvp_vector4_t&) ثابت override;
      ثابت vvp_vector4_t& vec4_unfiltered_قدر() ثابت override;

    عام: // These objects are only permallocated.
      ساكن فراغ* عامل new(std::size_t ناپ) { أرجع vvp_net_fun_t::ڈھیر.alloc(ناپ); }
      ساكن فراغ عامل delete(فراغ*obj);

    خاص:
      بنانشانی context_idx_;
      بنانشانی ناپ_2;
      vvp_bit4_t init_;
};

class vvp_fun_signal_real : عام vvp_fun_signal_ادھار {

    عام:
      explicit vvp_fun_signal_real() {};

	// Get information about the vector value.
      مجازی مزدوج real_unfiltered_قدر() ثابت = 0;
};

/*
 * Statically allocated vvp_fun_signal_real.
 */
class vvp_fun_signal_real_sa : عام vvp_fun_signal_real {

    عام:
      explicit vvp_fun_signal_real_sa();

      فراغ recv_real(vvp_net_ptr_t پورٹ, مزدوج ثنائی_ہندسہ,
                     vvp_context_t) override;

	// Get information about the vector value.
      مزدوج real_unfiltered_قدر() ثابت override;

    خاص:
      مزدوج بٹ_2;
};

/*
 * Automatically allocated vvp_fun_signal_real.
 */
class vvp_fun_signal_real_aa : عام vvp_fun_signal_real, عام automatic_signal_ادھار, عام automatic_hooks_s {

    عام:
      explicit vvp_fun_signal_real_aa();
      ~vvp_fun_signal_real_aa() override;

      فراغ alloc_instance(vvp_context_t context) override;
      فراغ reset_instance(vvp_context_t context) override;
#جے_تعریف_شدہ CHECK_WITH_VALGRIND
      فراغ free_instance(vvp_context_t context) override;
#جے_انت

      فراغ recv_real(vvp_net_ptr_t پورٹ, مزدوج ثنائی_ہندسہ,
                     vvp_context_t context) override;

	// Get information about the vector value.
      مزدوج real_unfiltered_قدر() ثابت override;

	// Get information about the vector value.
      بنانشانی   قدر_ناپ() ثابت override;
      vvp_bit4_t قدر(بنانشانی idx) ثابت override;
      vvp_scalar_t scalar_قدر(بنانشانی idx) ثابت override;
      فراغ vec4_قدر(vvp_vector4_t&) ثابت override;
      مزدوج real_قدر() ثابت override;
      فراغ get_signal_قدر(بنية t_vpi_قدر*vp) override;

    عام: // These objects are only permallocated.
      ساكن فراغ* عامل new(std::size_t ناپ);
      ساكن فراغ عامل delete(فراغ*obj);

    خاص:
      بنانشانی context_idx_;
};


class vvp_fun_signal_string : عام vvp_fun_signal_ادھار {

    عام:
      explicit vvp_fun_signal_string() {};

      مجازی ثابت std::string& get_string() ثابت =0;
};

/*
 * Statically allocated vvp_fun_signal_string.
 */
class vvp_fun_signal_string_sa : عام vvp_fun_signal_string {

    عام:
      explicit vvp_fun_signal_string_sa();

      فراغ recv_string(vvp_net_ptr_t پورٹ, ثابت std::string&ثنائی_ہندسہ,
		       vvp_context_t context) override;

      ثابت std::string& get_string() ثابت override;

    خاص:
      std::string قدر_2;
};

/*
 * Automatically allocated vvp_fun_signal_real.
 */
class vvp_fun_signal_string_aa : عام vvp_fun_signal_string, عام automatic_signal_ادھار, عام automatic_hooks_s {

    عام:
      explicit vvp_fun_signal_string_aa();
      ~vvp_fun_signal_string_aa() override;

      فراغ alloc_instance(vvp_context_t context) override;
      فراغ reset_instance(vvp_context_t context) override;
#جے_تعریف_شدہ CHECK_WITH_VALGRIND
      فراغ free_instance(vvp_context_t context) override;
#جے_انت
      فراغ recv_string(vvp_net_ptr_t پورٹ, ثابت std::string&ثنائی_ہندسہ,
		       vvp_context_t context) override;

	// Get information about the vector value.
      بنانشانی   قدر_ناپ() ثابت override;
      vvp_bit4_t قدر(بنانشانی idx) ثابت override;
      vvp_scalar_t scalar_قدر(بنانشانی idx) ثابت override;
      فراغ vec4_قدر(vvp_vector4_t&) ثابت override;
      مزدوج real_قدر() ثابت override;
      ثابت std::string& get_string() ثابت override;
      فراغ get_signal_قدر(بنية t_vpi_قدر*vp) override;

    عام: // These objects are only permallocated.
      ساكن فراغ* عامل new(std::size_t ناپ);
      ساكن فراغ عامل delete(فراغ*obj);

    خاص:
      بنانشانی context_idx_;
};

class vvp_fun_signal_object : عام vvp_fun_signal_ادھار {

    عام:
      explicit vvp_fun_signal_object(بنانشانی ناپ) { ناپ_2 = ناپ; };
      بنانشانی ناپ() ثابت { أرجع ناپ_2; }

      مجازی vvp_object_t get_object() ثابت =0;
    خاص:
      بنانشانی ناپ_2;
};

/*
 * Statically allocated vvp_fun_signal_string.
 */
class vvp_fun_signal_object_sa : عام vvp_fun_signal_object {

    عام:
      explicit vvp_fun_signal_object_sa(بنانشانی ناپ);

      فراغ recv_object(vvp_net_ptr_t پورٹ, vvp_object_t ثنائی_ہندسہ,
		    vvp_context_t context) override;

      vvp_object_t get_object() ثابت override;

    خاص:
      vvp_object_t قدر_2;
};

/*
 * Automatically allocated vvp_fun_signal_real.
 */
class vvp_fun_signal_object_aa : عام vvp_fun_signal_object, عام automatic_signal_ادھار, عام automatic_hooks_s {

    عام:
      explicit vvp_fun_signal_object_aa(بنانشانی ناپ);
      ~vvp_fun_signal_object_aa() override;

      فراغ alloc_instance(vvp_context_t context) override;
      فراغ reset_instance(vvp_context_t context) override;
#جے_تعریف_شدہ CHECK_WITH_VALGRIND
      فراغ free_instance(vvp_context_t context) override;
#جے_انت

      فراغ recv_object(vvp_net_ptr_t پورٹ, vvp_object_t ثنائی_ہندسہ,
		    vvp_context_t context) override;

	// Get information about the vector value.
      بنانشانی   قدر_ناپ() ثابت override;
      vvp_bit4_t قدر(بنانشانی idx) ثابت override;
      vvp_scalar_t scalar_قدر(بنانشانی idx) ثابت override;
      فراغ vec4_قدر(vvp_vector4_t&) ثابت override;
	//double real_value() const;
	//void get_signal_value(struct t_vpi_value*vp);

      vvp_object_t get_object() ثابت override;

    عام: // These objects are only permallocated.
      ساكن فراغ* عامل new(std::size_t ناپ);
      ساكن فراغ عامل delete(فراغ*obj);

    خاص:
      بنانشانی context_idx_;
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

class vvp_wire_ادھار  : عام vvp_net_fil_t, عام vvp_signal_قدر {

    عام:
      vvp_wire_ادھار();
      ~vvp_wire_ادھار() override;

        // Support for $countdrivers
      مجازی vvp_bit4_t driven_قدر(بنانشانی idx) ثابت;
      مجازی bool is_forced(بنانشانی idx) ثابت;
};

class vvp_wire_vec4 : عام vvp_wire_ادھار {

    عام:
      vvp_wire_vec4(بنانشانی wid, vvp_bit4_t init);

	// The main filter behavior for this class. These methods take
	// the value that the node is driven to, and applies the force
	// filters. In wires, this also saves the driven value, so
	// that when a force is released, we can revert to the driven value.
      prop_t filter_vec4(ثابت vvp_vector4_t&ثنائی_ہندسہ, vvp_vector4_t&rep,
			 بنانشانی ادھار, بنانشانی vwid) override;
      prop_t filter_vec8(ثابت vvp_vector8_t&val, vvp_vector8_t&rep,
			 بنانشانی ادھار, بنانشانی vwid) override;

	// Abstract methods from vvp_vpi_callback
      فراغ get_قدر(بنية t_vpi_قدر*قدر) override;
	// Abstract methods from vvp_net_fit_t
      بنانشانی filter_ناپ() ثابت override;
      فراغ force_fil_vec4(ثابت vvp_vector4_t&val, ثابت vvp_vector2_t&نقاب) override;
      فراغ force_fil_vec8(ثابت vvp_vector8_t&val, ثابت vvp_vector2_t&نقاب) override;
      فراغ force_fil_real(مزدوج val, ثابت vvp_vector2_t&نقاب) override;
      فراغ release(vvp_net_ptr_t ptr, bool net_flag) override;
      فراغ release_pv(vvp_net_ptr_t ptr, بنانشانی ادھار, بنانشانی wid, bool net_flag) override;

	// Implementation of vvp_signal_value methods
      بنانشانی قدر_ناپ() ثابت override;
      vvp_bit4_t قدر(بنانشانی idx) ثابت override;
      vvp_scalar_t scalar_قدر(بنانشانی idx) ثابت override;
      فراغ vec4_قدر(vvp_vector4_t&) ثابت override;

        // Support for $countdrivers
      vvp_bit4_t driven_قدر(بنانشانی idx) ثابت override;
      bool is_forced(بنانشانی idx) ثابت override;

    خاص:
      vvp_bit4_t filtered_قدر(بنانشانی idx) ثابت;

    خاص:
      bool needs_init_;
      vvp_vector4_t bits4_; // The tracked driven value
      vvp_vector4_t force4_; // the value being forced
};

class vvp_wire_vec8 : عام vvp_wire_ادھار {

    عام:
      explicit vvp_wire_vec8(بنانشانی wid);

	// The main filter behavior for this class
      prop_t filter_vec4(ثابت vvp_vector4_t&ثنائی_ہندسہ, vvp_vector4_t&rep,
			 بنانشانی ادھار, بنانشانی vwid) override;
      prop_t filter_vec8(ثابت vvp_vector8_t&val, vvp_vector8_t&rep,
			 بنانشانی ادھار, بنانشانی vwid) override;

	// island ports use this method to filter arbitrary values
	// through the force filter.
      prop_t filter_داخلہ_vec8(ثابت vvp_vector8_t&val, vvp_vector8_t&rep) ثابت;


	// Abstract methods from vvp_vpi_callback
      فراغ get_قدر(بنية t_vpi_قدر*قدر) override;
	// Abstract methods from vvp_net_fit_t
      بنانشانی filter_ناپ() ثابت override;
      فراغ force_fil_vec4(ثابت vvp_vector4_t&val, ثابت vvp_vector2_t&نقاب) override;
      فراغ force_fil_vec8(ثابت vvp_vector8_t&val, ثابت vvp_vector2_t&نقاب) override;
      فراغ force_fil_real(مزدوج val, ثابت vvp_vector2_t&نقاب) override;
      فراغ release(vvp_net_ptr_t ptr, bool net_flag) override;
      فراغ release_pv(vvp_net_ptr_t ptr, بنانشانی ادھار, بنانشانی wid, bool net_flag) override;

	// Implementation of vvp_signal_value methods
      بنانشانی قدر_ناپ() ثابت override;
      vvp_bit4_t قدر(بنانشانی idx) ثابت override;
      vvp_scalar_t scalar_قدر(بنانشانی idx) ثابت override;
      فراغ vec4_قدر(vvp_vector4_t&) ثابت override;
	// This is new to vvp_wire_vec8
      vvp_vector8_t vec8_قدر() ثابت;

        // Support for $countdrivers
      vvp_bit4_t driven_قدر(بنانشانی idx) ثابت override;
      bool is_forced(بنانشانی idx) ثابت override;

    خاص:
      vvp_scalar_t filtered_قدر(بنانشانی idx) ثابت;

    خاص:
      bool needs_init_;
      vvp_vector8_t bits8_;
      vvp_vector8_t force8_; // the value being forced
};

class vvp_wire_real : عام vvp_wire_ادھار {

    عام:
      explicit vvp_wire_real(فراغ);

	// The main filter behavior for this class
      prop_t filter_real(مزدوج&ثنائی_ہندسہ) override;

	// Abstract methods from vvp_vpi_callback
      فراغ get_قدر(بنية t_vpi_قدر*قدر) override;
	// Abstract methods from vvp_net_fit_t
      بنانشانی filter_ناپ() ثابت override;
      فراغ force_fil_vec4(ثابت vvp_vector4_t&val, ثابت vvp_vector2_t&نقاب) override;
      فراغ force_fil_vec8(ثابت vvp_vector8_t&val, ثابت vvp_vector2_t&نقاب) override;
      فراغ force_fil_real(مزدوج val, ثابت vvp_vector2_t&نقاب) override;
      فراغ release(vvp_net_ptr_t ptr, bool net_flag) override;
      فراغ release_pv(vvp_net_ptr_t ptr, بنانشانی ادھار, بنانشانی wid, bool net_flag) override;

	// Implementation of vvp_signal_value methods
      بنانشانی قدر_ناپ() ثابت override;
      vvp_bit4_t قدر(بنانشانی idx) ثابت override;
      vvp_scalar_t scalar_قدر(بنانشانی idx) ثابت override;
      فراغ vec4_قدر(vvp_vector4_t&) ثابت override;
      مزدوج real_قدر() ثابت override;

      فراغ get_signal_قدر(بنية t_vpi_قدر*vp) override;

    خاص:
      مزدوج ثنائی_ہندسہ_2;
      مزدوج force_;
      bool needs_init_ = true;
};

#إذا 0
class vvp_wire_string : عام vvp_wire_ادھار {

    عام:
      explicit vvp_wire_string(فراغ);

	// Abstract methods from vvp_vpi_callback
      فراغ get_قدر(بنية t_vpi_قدر*قدر);
	// Abstract methods from vvp_net_fil_t
      بنانشانی filter_ناپ() ثابت;
      فراغ force_fil_vec4(ثابت vvp_vector4_t&val, ثابت vvp_vector2_t&نقاب);
      فراغ force_fil_vec8(ثابت vvp_vector8_t&val, ثابت vvp_vector2_t&نقاب);
      فراغ force_fil_real(مزدوج val, ثابت vvp_vector2_t&نقاب);
      فراغ release(vvp_net_ptr_t ptr, bool net_flag);
      فراغ release_pv(vvp_net_ptr_t ptr, بنانشانی ادھار, بنانشانی wid, bool net_flag);

	// Implementation of vvp_signal_value methods
      بنانشانی قدر_ناپ() ثابت;
      vvp_bit4_t قدر(بنانشانی idx) ثابت;
      vvp_scalar_t scalar_قدر(بنانشانی idx) ثابت;
      فراغ vec4_قدر(vvp_vector4_t&) ثابت;
      مزدوج real_قدر() ثابت;

      فراغ get_signal_قدر(بنية t_vpi_قدر*vp);

    خاص:
      std::string قدر_2;
};
#جے_انت

#جے_انت /* IVL_vvp_net_sig_H */
