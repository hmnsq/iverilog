#నిర్వచితం_కాకపోతే IVL_vvp_net_sig_H
#నిర్వచించు IVL_vvp_net_sig_H
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

#నిర్వచితమైతే HAVE_IOSFWD
# include  <iosfwd>
#లేకపోతే
# include  <iostream>
#అయితే_ముగింపు

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


class vvp_fun_signal_పునాది : బహిరంగ vvp_net_fun_t {

    బహిరంగ:
      vvp_fun_signal_పునాది();

      శూన్య deassign();
      శూన్య deassign_pv(గుర్తులేని పునాది, గుర్తులేని wid);

    బహిరంగ:

	/* The %cassign/link instruction needs a place to write the
	   source node of the force, so that subsequent %cassign and
	   %deassign instructions can undo the link as needed. */
      class vvp_net_t*cassign_link;

    రక్షిత:
      bool continuous_కేటాయించు_చురుకైన;
      vvp_vector2_t కేటాయించు_ముసుగు;

    రక్షిత:
	// This is true until at least one propagation happens.
      bool needs_init_;
};

/*
 * Variables and wires can have their values accessed, so this base
 * class offers the unified concept of an accessible value.
 */
class vvp_signal_విలువ {
    బహిరంగ:
      మాయా ~vvp_signal_విలువ() =0;
      మాయా గుర్తులేని విలువ_కొలత() స్థిరరాశి =0;
      మాయా vvp_bit4_t విలువ(గుర్తులేని idx) స్థిరరాశి =0;
      మాయా vvp_scalar_t scalar_విలువ(గుర్తులేని idx) స్థిరరాశి =0;
      మాయా శూన్య vec4_విలువ(vvp_vector4_t&) స్థిరరాశి =0;
      మాయా ద్విగుణ real_విలువ() స్థిరరాశి;

      మాయా శూన్య get_signal_విలువ(నిర్మాణం t_vpi_విలువ*vp);
};

/*
 * This abstract class is a little more specific than the signal_base
 * class, in that it adds vector access methods.
 */
class vvp_fun_signal_vec : బహిరంగ vvp_fun_signal_పునాది {
    బహిరంగ:
      మాయా స్థిరరాశి vvp_vector4_t& vec4_unfiltered_విలువ() స్థిరరాశి =0;
};

class automatic_signal_పునాది : బహిరంగ vvp_signal_విలువ, బహిరంగ vvp_net_fil_t {

    బహిరంగ:
	// Automatic variables cannot be forced or released. Provide
	// stubs that assert.
      మాయా శూన్య release(vvp_net_ptr_t ptr, bool net_flag) override;
      మాయా శూన్య release_pv(vvp_net_ptr_t ptr, గుర్తులేని పునాది, గుర్తులేని wid, bool net_flag) override;

      మాయా గుర్తులేని filter_కొలత() స్థిరరాశి override;
      మాయా శూన్య force_fil_vec4(స్థిరరాశి vvp_vector4_t&val, స్థిరరాశి vvp_vector2_t&ముసుగు) override;
      మాయా శూన్య force_fil_vec8(స్థిరరాశి vvp_vector8_t&val, స్థిరరాశి vvp_vector2_t&ముసుగు) override;
      మాయా శూన్య force_fil_real(ద్విగుణ val, స్థిరరాశి vvp_vector2_t&ముసుగు) override;
      మాయా శూన్య get_విలువ(నిర్మాణం t_vpi_విలువ*విలువ) override;
};

/*
 * Statically allocated vvp_fun_signal4.
 */
class vvp_fun_signal4_sa : బహిరంగ vvp_fun_signal_vec {

    బహిరంగ:
      explicit vvp_fun_signal4_sa(గుర్తులేని wid, vvp_bit4_t init=BIT4_X);

      శూన్య recv_vec4(vvp_net_ptr_t పోర్టు, స్థిరరాశి vvp_vector4_t&బిట్టు,
                     vvp_context_t) override;
      శూన్య recv_vec8(vvp_net_ptr_t పోర్టు, స్థిరరాశి vvp_vector8_t&బిట్టు) override;

	// Part select variants of above
      శూన్య recv_vec4_pv(vvp_net_ptr_t పోర్టు, స్థిరరాశి vvp_vector4_t&బిట్టు,
			గుర్తులేని పునాది, గుర్తులేని vwid, vvp_context_t) override;
      శూన్య recv_vec8_pv(vvp_net_ptr_t పోర్టు, స్థిరరాశి vvp_vector8_t&బిట్టు,
			గుర్తులేని పునాది, గుర్తులేని vwid) override;

	// Get information about the vector value.
      స్థిరరాశి vvp_vector4_t& vec4_unfiltered_విలువ() స్థిరరాశి override;

    అంతరంగ:
      vvp_vector4_t bits4_;
};

/*
 * Automatically allocated vvp_fun_signal4.
 */
class vvp_fun_signal4_aa : బహిరంగ vvp_fun_signal_vec, బహిరంగ automatic_signal_పునాది, బహిరంగ automatic_hooks_s {

    బహిరంగ:
      explicit vvp_fun_signal4_aa(గుర్తులేని wid, vvp_bit4_t init=BIT4_X);
      ~vvp_fun_signal4_aa() override;

      శూన్య alloc_instance(vvp_context_t context) override;
      శూన్య reset_instance(vvp_context_t context) override;
#నిర్వచితమైతే CHECK_WITH_VALGRIND
      శూన్య free_instance(vvp_context_t context) override;
#అయితే_ముగింపు

      శూన్య recv_vec4(vvp_net_ptr_t పోర్టు, స్థిరరాశి vvp_vector4_t&బిట్టు,
                     vvp_context_t context) override;

	// Part select variants of above
      శూన్య recv_vec4_pv(vvp_net_ptr_t పోర్టు, స్థిరరాశి vvp_vector4_t&బిట్టు,
			గుర్తులేని పునాది, గుర్తులేని vwid, vvp_context_t) override;

	// Get information about the vector value.
      గుర్తులేని   విలువ_కొలత() స్థిరరాశి override;
      vvp_bit4_t విలువ(గుర్తులేని idx) స్థిరరాశి override;
      vvp_scalar_t scalar_విలువ(గుర్తులేని idx) స్థిరరాశి override;
      శూన్య vec4_విలువ(vvp_vector4_t&) స్థిరరాశి override;
      స్థిరరాశి vvp_vector4_t& vec4_unfiltered_విలువ() స్థిరరాశి override;

    బహిరంగ: // These objects are only permallocated.
      స్థిర శూన్య* క్రియాకారకం new(std::size_t కొలత) { తిరిగి vvp_net_fun_t::కుప్ప.alloc(కొలత); }
      స్థిర శూన్య క్రియాకారకం delete(శూన్య*obj);

    అంతరంగ:
      గుర్తులేని context_idx_;
      గుర్తులేని కొలత_2;
      vvp_bit4_t init_;
};

class vvp_fun_signal_real : బహిరంగ vvp_fun_signal_పునాది {

    బహిరంగ:
      explicit vvp_fun_signal_real() {};

	// Get information about the vector value.
      మాయా ద్విగుణ real_unfiltered_విలువ() స్థిరరాశి = 0;
};

/*
 * Statically allocated vvp_fun_signal_real.
 */
class vvp_fun_signal_real_sa : బహిరంగ vvp_fun_signal_real {

    బహిరంగ:
      explicit vvp_fun_signal_real_sa();

      శూన్య recv_real(vvp_net_ptr_t పోర్టు, ద్విగుణ బిట్టు,
                     vvp_context_t) override;

	// Get information about the vector value.
      ద్విగుణ real_unfiltered_విలువ() స్థిరరాశి override;

    అంతరంగ:
      ద్విగుణ బిట్లు_2;
};

/*
 * Automatically allocated vvp_fun_signal_real.
 */
class vvp_fun_signal_real_aa : బహిరంగ vvp_fun_signal_real, బహిరంగ automatic_signal_పునాది, బహిరంగ automatic_hooks_s {

    బహిరంగ:
      explicit vvp_fun_signal_real_aa();
      ~vvp_fun_signal_real_aa() override;

      శూన్య alloc_instance(vvp_context_t context) override;
      శూన్య reset_instance(vvp_context_t context) override;
#నిర్వచితమైతే CHECK_WITH_VALGRIND
      శూన్య free_instance(vvp_context_t context) override;
#అయితే_ముగింపు

      శూన్య recv_real(vvp_net_ptr_t పోర్టు, ద్విగుణ బిట్టు,
                     vvp_context_t context) override;

	// Get information about the vector value.
      ద్విగుణ real_unfiltered_విలువ() స్థిరరాశి override;

	// Get information about the vector value.
      గుర్తులేని   విలువ_కొలత() స్థిరరాశి override;
      vvp_bit4_t విలువ(గుర్తులేని idx) స్థిరరాశి override;
      vvp_scalar_t scalar_విలువ(గుర్తులేని idx) స్థిరరాశి override;
      శూన్య vec4_విలువ(vvp_vector4_t&) స్థిరరాశి override;
      ద్విగుణ real_విలువ() స్థిరరాశి override;
      శూన్య get_signal_విలువ(నిర్మాణం t_vpi_విలువ*vp) override;

    బహిరంగ: // These objects are only permallocated.
      స్థిర శూన్య* క్రియాకారకం new(std::size_t కొలత);
      స్థిర శూన్య క్రియాకారకం delete(శూన్య*obj);

    అంతరంగ:
      గుర్తులేని context_idx_;
};


class vvp_fun_signal_string : బహిరంగ vvp_fun_signal_పునాది {

    బహిరంగ:
      explicit vvp_fun_signal_string() {};

      మాయా స్థిరరాశి std::string& get_string() స్థిరరాశి =0;
};

/*
 * Statically allocated vvp_fun_signal_string.
 */
class vvp_fun_signal_string_sa : బహిరంగ vvp_fun_signal_string {

    బహిరంగ:
      explicit vvp_fun_signal_string_sa();

      శూన్య recv_string(vvp_net_ptr_t పోర్టు, స్థిరరాశి std::string&బిట్టు,
		       vvp_context_t context) override;

      స్థిరరాశి std::string& get_string() స్థిరరాశి override;

    అంతరంగ:
      std::string విలువ_2;
};

/*
 * Automatically allocated vvp_fun_signal_real.
 */
class vvp_fun_signal_string_aa : బహిరంగ vvp_fun_signal_string, బహిరంగ automatic_signal_పునాది, బహిరంగ automatic_hooks_s {

    బహిరంగ:
      explicit vvp_fun_signal_string_aa();
      ~vvp_fun_signal_string_aa() override;

      శూన్య alloc_instance(vvp_context_t context) override;
      శూన్య reset_instance(vvp_context_t context) override;
#నిర్వచితమైతే CHECK_WITH_VALGRIND
      శూన్య free_instance(vvp_context_t context) override;
#అయితే_ముగింపు
      శూన్య recv_string(vvp_net_ptr_t పోర్టు, స్థిరరాశి std::string&బిట్టు,
		       vvp_context_t context) override;

	// Get information about the vector value.
      గుర్తులేని   విలువ_కొలత() స్థిరరాశి override;
      vvp_bit4_t విలువ(గుర్తులేని idx) స్థిరరాశి override;
      vvp_scalar_t scalar_విలువ(గుర్తులేని idx) స్థిరరాశి override;
      శూన్య vec4_విలువ(vvp_vector4_t&) స్థిరరాశి override;
      ద్విగుణ real_విలువ() స్థిరరాశి override;
      స్థిరరాశి std::string& get_string() స్థిరరాశి override;
      శూన్య get_signal_విలువ(నిర్మాణం t_vpi_విలువ*vp) override;

    బహిరంగ: // These objects are only permallocated.
      స్థిర శూన్య* క్రియాకారకం new(std::size_t కొలత);
      స్థిర శూన్య క్రియాకారకం delete(శూన్య*obj);

    అంతరంగ:
      గుర్తులేని context_idx_;
};

class vvp_fun_signal_object : బహిరంగ vvp_fun_signal_పునాది {

    బహిరంగ:
      explicit vvp_fun_signal_object(గుర్తులేని కొలత) { కొలత_2 = కొలత; };
      గుర్తులేని కొలత() స్థిరరాశి { తిరిగి కొలత_2; }

      మాయా vvp_object_t get_object() స్థిరరాశి =0;
    అంతరంగ:
      గుర్తులేని కొలత_2;
};

/*
 * Statically allocated vvp_fun_signal_string.
 */
class vvp_fun_signal_object_sa : బహిరంగ vvp_fun_signal_object {

    బహిరంగ:
      explicit vvp_fun_signal_object_sa(గుర్తులేని కొలత);

      శూన్య recv_object(vvp_net_ptr_t పోర్టు, vvp_object_t బిట్టు,
		    vvp_context_t context) override;

      vvp_object_t get_object() స్థిరరాశి override;

    అంతరంగ:
      vvp_object_t విలువ_2;
};

/*
 * Automatically allocated vvp_fun_signal_real.
 */
class vvp_fun_signal_object_aa : బహిరంగ vvp_fun_signal_object, బహిరంగ automatic_signal_పునాది, బహిరంగ automatic_hooks_s {

    బహిరంగ:
      explicit vvp_fun_signal_object_aa(గుర్తులేని కొలత);
      ~vvp_fun_signal_object_aa() override;

      శూన్య alloc_instance(vvp_context_t context) override;
      శూన్య reset_instance(vvp_context_t context) override;
#నిర్వచితమైతే CHECK_WITH_VALGRIND
      శూన్య free_instance(vvp_context_t context) override;
#అయితే_ముగింపు

      శూన్య recv_object(vvp_net_ptr_t పోర్టు, vvp_object_t బిట్టు,
		    vvp_context_t context) override;

	// Get information about the vector value.
      గుర్తులేని   విలువ_కొలత() స్థిరరాశి override;
      vvp_bit4_t విలువ(గుర్తులేని idx) స్థిరరాశి override;
      vvp_scalar_t scalar_విలువ(గుర్తులేని idx) స్థిరరాశి override;
      శూన్య vec4_విలువ(vvp_vector4_t&) స్థిరరాశి override;
	//double real_value() const;
	//void get_signal_value(struct t_vpi_value*vp);

      vvp_object_t get_object() స్థిరరాశి override;

    బహిరంగ: // These objects are only permallocated.
      స్థిర శూన్య* క్రియాకారకం new(std::size_t కొలత);
      స్థిర శూన్య క్రియాకారకం delete(శూన్య*obj);

    అంతరంగ:
      గుర్తులేని context_idx_;
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

class vvp_wire_పునాది  : బహిరంగ vvp_net_fil_t, బహిరంగ vvp_signal_విలువ {

    బహిరంగ:
      vvp_wire_పునాది();
      ~vvp_wire_పునాది() override;

        // Support for $countdrivers
      మాయా vvp_bit4_t driven_విలువ(గుర్తులేని idx) స్థిరరాశి;
      మాయా bool is_forced(గుర్తులేని idx) స్థిరరాశి;
};

class vvp_wire_vec4 : బహిరంగ vvp_wire_పునాది {

    బహిరంగ:
      vvp_wire_vec4(గుర్తులేని wid, vvp_bit4_t init);

	// The main filter behavior for this class. These methods take
	// the value that the node is driven to, and applies the force
	// filters. In wires, this also saves the driven value, so
	// that when a force is released, we can revert to the driven value.
      prop_t filter_vec4(స్థిరరాశి vvp_vector4_t&బిట్టు, vvp_vector4_t&rep,
			 గుర్తులేని పునాది, గుర్తులేని vwid) override;
      prop_t filter_vec8(స్థిరరాశి vvp_vector8_t&val, vvp_vector8_t&rep,
			 గుర్తులేని పునాది, గుర్తులేని vwid) override;

	// Abstract methods from vvp_vpi_callback
      శూన్య get_విలువ(నిర్మాణం t_vpi_విలువ*విలువ) override;
	// Abstract methods from vvp_net_fit_t
      గుర్తులేని filter_కొలత() స్థిరరాశి override;
      శూన్య force_fil_vec4(స్థిరరాశి vvp_vector4_t&val, స్థిరరాశి vvp_vector2_t&ముసుగు) override;
      శూన్య force_fil_vec8(స్థిరరాశి vvp_vector8_t&val, స్థిరరాశి vvp_vector2_t&ముసుగు) override;
      శూన్య force_fil_real(ద్విగుణ val, స్థిరరాశి vvp_vector2_t&ముసుగు) override;
      శూన్య release(vvp_net_ptr_t ptr, bool net_flag) override;
      శూన్య release_pv(vvp_net_ptr_t ptr, గుర్తులేని పునాది, గుర్తులేని wid, bool net_flag) override;

	// Implementation of vvp_signal_value methods
      గుర్తులేని విలువ_కొలత() స్థిరరాశి override;
      vvp_bit4_t విలువ(గుర్తులేని idx) స్థిరరాశి override;
      vvp_scalar_t scalar_విలువ(గుర్తులేని idx) స్థిరరాశి override;
      శూన్య vec4_విలువ(vvp_vector4_t&) స్థిరరాశి override;

        // Support for $countdrivers
      vvp_bit4_t driven_విలువ(గుర్తులేని idx) స్థిరరాశి override;
      bool is_forced(గుర్తులేని idx) స్థిరరాశి override;

    అంతరంగ:
      vvp_bit4_t filtered_విలువ(గుర్తులేని idx) స్థిరరాశి;

    అంతరంగ:
      bool needs_init_;
      vvp_vector4_t bits4_; // The tracked driven value
      vvp_vector4_t force4_; // the value being forced
};

class vvp_wire_vec8 : బహిరంగ vvp_wire_పునాది {

    బహిరంగ:
      explicit vvp_wire_vec8(గుర్తులేని wid);

	// The main filter behavior for this class
      prop_t filter_vec4(స్థిరరాశి vvp_vector4_t&బిట్టు, vvp_vector4_t&rep,
			 గుర్తులేని పునాది, గుర్తులేని vwid) override;
      prop_t filter_vec8(స్థిరరాశి vvp_vector8_t&val, vvp_vector8_t&rep,
			 గుర్తులేని పునాది, గుర్తులేని vwid) override;

	// island ports use this method to filter arbitrary values
	// through the force filter.
      prop_t filter_ఆదానం_vec8(స్థిరరాశి vvp_vector8_t&val, vvp_vector8_t&rep) స్థిరరాశి;


	// Abstract methods from vvp_vpi_callback
      శూన్య get_విలువ(నిర్మాణం t_vpi_విలువ*విలువ) override;
	// Abstract methods from vvp_net_fit_t
      గుర్తులేని filter_కొలత() స్థిరరాశి override;
      శూన్య force_fil_vec4(స్థిరరాశి vvp_vector4_t&val, స్థిరరాశి vvp_vector2_t&ముసుగు) override;
      శూన్య force_fil_vec8(స్థిరరాశి vvp_vector8_t&val, స్థిరరాశి vvp_vector2_t&ముసుగు) override;
      శూన్య force_fil_real(ద్విగుణ val, స్థిరరాశి vvp_vector2_t&ముసుగు) override;
      శూన్య release(vvp_net_ptr_t ptr, bool net_flag) override;
      శూన్య release_pv(vvp_net_ptr_t ptr, గుర్తులేని పునాది, గుర్తులేని wid, bool net_flag) override;

	// Implementation of vvp_signal_value methods
      గుర్తులేని విలువ_కొలత() స్థిరరాశి override;
      vvp_bit4_t విలువ(గుర్తులేని idx) స్థిరరాశి override;
      vvp_scalar_t scalar_విలువ(గుర్తులేని idx) స్థిరరాశి override;
      శూన్య vec4_విలువ(vvp_vector4_t&) స్థిరరాశి override;
	// This is new to vvp_wire_vec8
      vvp_vector8_t vec8_విలువ() స్థిరరాశి;

        // Support for $countdrivers
      vvp_bit4_t driven_విలువ(గుర్తులేని idx) స్థిరరాశి override;
      bool is_forced(గుర్తులేని idx) స్థిరరాశి override;

    అంతరంగ:
      vvp_scalar_t filtered_విలువ(గుర్తులేని idx) స్థిరరాశి;

    అంతరంగ:
      bool needs_init_;
      vvp_vector8_t bits8_;
      vvp_vector8_t force8_; // the value being forced
};

class vvp_wire_real : బహిరంగ vvp_wire_పునాది {

    బహిరంగ:
      explicit vvp_wire_real(శూన్య);

	// The main filter behavior for this class
      prop_t filter_real(ద్విగుణ&బిట్టు) override;

	// Abstract methods from vvp_vpi_callback
      శూన్య get_విలువ(నిర్మాణం t_vpi_విలువ*విలువ) override;
	// Abstract methods from vvp_net_fit_t
      గుర్తులేని filter_కొలత() స్థిరరాశి override;
      శూన్య force_fil_vec4(స్థిరరాశి vvp_vector4_t&val, స్థిరరాశి vvp_vector2_t&ముసుగు) override;
      శూన్య force_fil_vec8(స్థిరరాశి vvp_vector8_t&val, స్థిరరాశి vvp_vector2_t&ముసుగు) override;
      శూన్య force_fil_real(ద్విగుణ val, స్థిరరాశి vvp_vector2_t&ముసుగు) override;
      శూన్య release(vvp_net_ptr_t ptr, bool net_flag) override;
      శూన్య release_pv(vvp_net_ptr_t ptr, గుర్తులేని పునాది, గుర్తులేని wid, bool net_flag) override;

	// Implementation of vvp_signal_value methods
      గుర్తులేని విలువ_కొలత() స్థిరరాశి override;
      vvp_bit4_t విలువ(గుర్తులేని idx) స్థిరరాశి override;
      vvp_scalar_t scalar_విలువ(గుర్తులేని idx) స్థిరరాశి override;
      శూన్య vec4_విలువ(vvp_vector4_t&) స్థిరరాశి override;
      ద్విగుణ real_విలువ() స్థిరరాశి override;

      శూన్య get_signal_విలువ(నిర్మాణం t_vpi_విలువ*vp) override;

    అంతరంగ:
      ద్విగుణ బిట్టు_2;
      ద్విగుణ force_;
      bool needs_init_ = true;
};

#ఒకవేళ 0
class vvp_wire_string : బహిరంగ vvp_wire_పునాది {

    బహిరంగ:
      explicit vvp_wire_string(శూన్య);

	// Abstract methods from vvp_vpi_callback
      శూన్య get_విలువ(నిర్మాణం t_vpi_విలువ*విలువ);
	// Abstract methods from vvp_net_fil_t
      గుర్తులేని filter_కొలత() స్థిరరాశి;
      శూన్య force_fil_vec4(స్థిరరాశి vvp_vector4_t&val, స్థిరరాశి vvp_vector2_t&ముసుగు);
      శూన్య force_fil_vec8(స్థిరరాశి vvp_vector8_t&val, స్థిరరాశి vvp_vector2_t&ముసుగు);
      శూన్య force_fil_real(ద్విగుణ val, స్థిరరాశి vvp_vector2_t&ముసుగు);
      శూన్య release(vvp_net_ptr_t ptr, bool net_flag);
      శూన్య release_pv(vvp_net_ptr_t ptr, గుర్తులేని పునాది, గుర్తులేని wid, bool net_flag);

	// Implementation of vvp_signal_value methods
      గుర్తులేని విలువ_కొలత() స్థిరరాశి;
      vvp_bit4_t విలువ(గుర్తులేని idx) స్థిరరాశి;
      vvp_scalar_t scalar_విలువ(గుర్తులేని idx) స్థిరరాశి;
      శూన్య vec4_విలువ(vvp_vector4_t&) స్థిరరాశి;
      ద్విగుణ real_విలువ() స్థిరరాశి;

      శూన్య get_signal_విలువ(నిర్మాణం t_vpi_విలువ*vp);

    అంతరంగ:
      std::string విలువ_2;
};
#అయితే_ముగింపు

#అయితే_ముగింపు /* IVL_vvp_net_sig_H */
