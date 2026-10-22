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
# include  "vvp_net.h"
# include  "vvp_net_sig.h"
# include  "vvp_island.h"
# include  "vpi_priv.h"
# include  "resolv.h"
# include  "schedule.h"
# include  "statistics.h"
# include  <cstdio>
# include  <cstring>
# include  <cstdlib>
# include  <iostream>
# include  <typeinfo>
# include  <climits>
# include  <cmath>
# include  <cassert>
#నిర్వచితమైతే CHECK_WITH_VALGRIND
# include  <valgrind/memcheck.h>
# include  <map>
# include  "sfunc.h"
# include  "udp.h"
# include  "ivl_alloc.h"
#అయితే_ముగింపు

ఉపయోగించి నామాకాశ std;

/* This is the size of an unsigned long in bits. This is just a
   convenience macro. */
# define CPU_మాట_బిట్లు (8*పరిమాణం(గుర్తులేని దీర్ఘ))
# define శిఖరం_బిట్టు (1UL << (CPU_మాట_బిట్లు-1))

permaheap vvp_net_fun_t::కుప్ప;
permaheap vvp_net_fil_t::కుప్ప;

// Allocate around 1Megabyte/chunk.
స్థిర స్థిరరాశి size_t VVP_NET_CHUNK = 1024*1024/పరిమాణం(vvp_net_t);
స్థిర vvp_net_t*vvp_net_alloc_పట్టిక = ఖాళీ;
#నిర్వచితమైతే CHECK_WITH_VALGRIND
స్థిర vvp_net_t **vvp_net_pool = ఖాళీ;
స్థిర గుర్తులేని vvp_net_pool_లెక్క = 0;
#అయితే_ముగింపు
స్థిర size_t vvp_net_alloc_మిగిలిన = 0;
// For statistics, count the vvp_nets allocated and the bytes of alloc
// chunks allocated.
గుర్తులేని దీర్ఘ లెక్క_vvp_nets = 0;
size_t కొలత_vvp_nets = 0;

శూన్య* vvp_net_t::క్రియాకారకం కొత్త (size_t size)
{
      ప్రతిజ్ఞా(size == పరిమాణం(vvp_net_t));
      ఒకవేళ (vvp_net_alloc_మిగిలిన == 0) {
	    vvp_net_alloc_పట్టిక = ::కొత్త vvp_net_t[VVP_NET_CHUNK];
	    vvp_net_alloc_మిగిలిన = VVP_NET_CHUNK;
	    కొలత_vvp_nets += size*VVP_NET_CHUNK;
#నిర్వచితమైతే CHECK_WITH_VALGRIND
	    VALGRIND_MAKE_MEM_NOACCESS(vvp_net_alloc_పట్టిక, size*VVP_NET_CHUNK);
	    VALGRIND_CREATE_MEMPOOL(vvp_net_alloc_పట్టిక, 0, 0);
	    vvp_net_pool_లెక్క += 1;
	    vvp_net_pool = static_cast<vvp_net_t **>(realloc(vvp_net_pool,
	                   vvp_net_pool_లెక్క*పరిమాణం(vvp_net_t **)));
	    vvp_net_pool[vvp_net_pool_లెక్క-1] = vvp_net_alloc_పట్టిక;
#అయితే_ముగింపు
      }

      vvp_net_t*return_this = vvp_net_alloc_పట్టిక;
#నిర్వచితమైతే CHECK_WITH_VALGRIND
      VALGRIND_MEMPOOL_ALLOC(vvp_net_pool[vvp_net_pool_లెక్క-1],
                             return_this, size);
      return_this->pool = vvp_net_pool[vvp_net_pool_లెక్క-1];
#అయితే_ముగింపు
      vvp_net_alloc_పట్టిక += 1;
      vvp_net_alloc_మిగిలిన -= 1;
      లెక్క_vvp_nets += 1;
      తిరిగి return_this;
}

#నిర్వచితమైతే CHECK_WITH_VALGRIND
స్థిర map<vvp_net_t*, తార్కిక> vvp_net_map;
స్థిర map<sfunc_core*, తార్కిక> sfunc_map;
స్థిర map<vvp_udp_fun_core*, తార్కిక> udp_map;
స్థిర map<resolv_core*, తార్కిక> resolv_map;
స్థిర vvp_net_t **local_net_pool = 0;
స్థిర గుర్తులేని local_net_pool_లెక్క = 0;

శూన్య pool_local_net(vvp_net_t*net)
{
      local_net_pool_లెక్క += 1;
      local_net_pool = static_cast<vvp_net_t **>(realloc(local_net_pool,
                       local_net_pool_లెక్క*పరిమాణం(vvp_net_t **)));
      local_net_pool[local_net_pool_లెక్క-1] = net;
}

శూన్య vvp_net_delete(vvp_net_t *అంశం)
{
      vvp_net_map[అంశం] = నిజం;
      ఒకవేళ (sfunc_core*tmp = dynamic_cast<sfunc_core*> (అంశం->fun)) {
	    sfunc_map[tmp] = నిజం;
      }
      ఒకవేళ (vvp_udp_fun_core*tmp = dynamic_cast<vvp_udp_fun_core*> (అంశం->fun)) {
	    udp_map[tmp] = నిజం;
      }
      ఒకవేళ (resolv_core*tmp = dynamic_cast<resolv_core*> (అంశం->fun)) {
	    resolv_map[tmp] = నిజం;
      }
	/* Only delete static object variables. */
      ఒకవేళ (vvp_fun_signal_object_sa*tmp = dynamic_cast<vvp_fun_signal_object_sa*> (అంశం->fun)) {
	    తొలగించు tmp;
      }
	/* Only delete static string variables. */
      ఒకవేళ (vvp_fun_signal_string_sa*tmp = dynamic_cast<vvp_fun_signal_string_sa*> (అంశం->fun)) {
	    తొలగించు tmp;
      }
}

శూన్య vvp_net_pool_delete()
{
      గుర్తులేని దీర్ఘ vvp_nets_del = 0;

      కొరకు (గుర్తులేని idx = 0; idx < local_net_pool_లెక్క; idx += 1) {
	    vvp_net_delete(local_net_pool[idx]);
      }
      విడుదల(local_net_pool);
      local_net_pool = 0;
      local_net_pool_లెక్క = 0;

      map<vvp_net_t*, తార్కిక>::iterator iter;
      కొరకు (iter = vvp_net_map.begin(); iter != vvp_net_map.చివర(); ++ iter ) {
	    vvp_nets_del += 1;
	    VALGRIND_MEMPOOL_FREE(iter->మొదటి->pool, iter->మొదటి);
      }
      vvp_net_map.clear();

      map<sfunc_core*, తార్కిక>::iterator siter;
      కొరకు (siter = sfunc_map.begin(); siter != sfunc_map.చివర(); ++ siter ) {
	    తొలగించు siter->మొదటి;
      }
      sfunc_map.clear();

      map<vvp_udp_fun_core*, తార్కిక>::iterator uiter;
      కొరకు (uiter = udp_map.begin(); uiter != udp_map.చివర(); ++ uiter ) {
	    తొలగించు uiter->మొదటి;
      }
      udp_map.clear();

      map<resolv_core*, తార్కిక>::iterator riter;
      కొరకు (riter = resolv_map.begin(); riter != resolv_map.చివర(); ++ riter ) {
	    తొలగించు riter->మొదటి;
      }
      resolv_map.clear();

      ఒకవేళ (RUNNING_ON_VALGRIND && (vvp_nets_del != లెక్క_vvp_nets)) {
	    fflush(ఖాళీ);
	    VALGRIND_PRINTF("Error: vvp missed deleting %ld of %lu net(s).",
	                    (దీర్ఘ) లెక్క_vvp_nets - vvp_nets_del,
	                    లెక్క_vvp_nets);
      }

      కొరకు (గుర్తులేని idx = 0; idx < vvp_net_pool_లెక్క; idx += 1) {
	    VALGRIND_DESTROY_MEMPOOL(vvp_net_pool[idx]);
	    ::తొలగించు [] vvp_net_pool[idx];
      }
      విడుదల(vvp_net_pool);
      vvp_net_pool = ఖాళీ;
      vvp_net_pool_లెక్క = 0;
}
#అయితే_ముగింపు

శూన్య vvp_net_t::క్రియాకారకం తొలగించు(శూన్య*)
{
      ప్రతిజ్ఞా(0);
}

vvp_net_t::vvp_net_t()
: బహిర్గతం_2(vvp_net_ptr_t(0,0))
{
      fun = 0;
      fil = 0;
}

శూన్య vvp_net_t::link(vvp_net_ptr_t పోర్టు_గమ్యం_link)
{
      vvp_net_t*net = పోర్టు_గమ్యం_link.ptr();

	// Connect nodes with vvp_fun_modpath_src to the head of the
	// linked list, so that vvp_fun_modpath_src are always evaluated
	// before their respective vvp_fun_modpath
      ఒకవేళ (dynamic_cast<vvp_fun_modpath_src*>(net->fun)) {
	    net->పోర్టు[పోర్టు_గమ్యం_link.పోర్టు()] = బహిర్గతం_2;
	    బహిర్గతం_2 = పోర్టు_గమ్యం_link;
	// All other nodes connect after the last vvp_fun_modpath_src
      } లేకపోతే {
	      // List is empty or first node has no vvp_fun_modpath_src
	      // then just insert the node into the head of linked list
	    ఒకవేళ (బహిర్గతం_2.ptr() == శూన్యసూచిక || ! dynamic_cast<vvp_fun_modpath_src*>(బహిర్గతం_2.ptr()->fun)) {
		  net->పోర్టు[పోర్టు_గమ్యం_link.పోర్టు()] = బహిర్గతం_2;
		  బహిర్గతం_2 = పోర్టు_గమ్యం_link;
		  తిరిగి;
	    }

	    vvp_net_ptr_t cur = బహిర్గతం_2;
	    vvp_net_ptr_t prev = vvp_net_ptr_t(శూన్యసూచిక,0);

	      // Traverse through linked list
	    అయితే (dynamic_cast<vvp_fun_modpath_src*>(cur.ptr()->fun)) {
		  prev = cur;
		  cur = cur.ptr()->పోర్టు[cur.పోర్టు()];
		  ఒకవేళ (! cur.ptr()) ఆపు; // End of list
	    }

	    ప్రతిజ్ఞా(prev.ptr());

	      // Insert after last vvp_fun_modpath_src (or end of list)
	    net->పోర్టు[పోర్టు_గమ్యం_link.పోర్టు()] = cur;
	    prev.ptr()->పోర్టు[prev.పోర్టు()] = పోర్టు_గమ్యం_link;
      }
}

/*
 * Unlink a ptr object from the driver. The input is the driver in the
 * form of a vvp_net_t pointer. The .out member of that object is the
 * driver. The dst_ptr argument is the receiver pin to be located and
 * removed from the fan-out list.
 */
శూన్య vvp_net_t::unlink(vvp_net_ptr_t dst_ptr)
{
      vvp_net_t*net = dst_ptr.ptr();
      గుర్తులేని net_పోర్టు = dst_ptr.పోర్టు();

      ఒకవేళ (బహిర్గతం_2 == dst_ptr) {
	      /* If the drive fan-out list starts with this pointer,
		 then the unlink is easy. Pull the list forward. */
	    బహిర్గతం_2 = net->పోర్టు[net_పోర్టు];
      } లేకపోతే ఒకవేళ (! బహిర్గతం_2.nil()) {
	      /* Scan the linked list, looking for the net_ptr_t
		 pointer *before* the one we wish to remove. */
	    vvp_net_ptr_t cur = బహిర్గతం_2;
	    ప్రతిజ్ఞా(!cur.nil());
	    vvp_net_t*cur_net = cur.ptr();
	    గుర్తులేని cur_పోర్టు = cur.పోర్టు();
	    అయితే (cur_net && cur_net->పోర్టు[cur_పోర్టు] != dst_ptr) {
		  cur = cur_net->పోర్టు[cur_పోర్టు];
		  cur_net = cur.ptr();
		  cur_పోర్టు = cur.పోర్టు();
	    }
	      /* Unlink. */
	    ఒకవేళ (cur_net) cur_net->పోర్టు[cur_పోర్టు] = net->పోర్టు[net_పోర్టు];
      }

      net->పోర్టు[net_పోర్టు] = vvp_net_ptr_t(0,0);
}

శూన్య vvp_net_t::లెక్క_drivers(గుర్తులేని idx, గుర్తులేని counts[4])
{
      counts[0] = 0;
      counts[1] = 0;
      counts[2] = 0;
      counts[3] = 0;

        /* $countdrivers can only be used on wires. */
      vvp_wire_పునాది*wire=dynamic_cast<vvp_wire_పునాది*>(fil);
      ప్రతిజ్ఞా(wire);

      ఒకవేళ (wire->is_forced(idx))
            counts[3] = 1;

        /* If the net has multiple drivers, we need to interrogate the
           resolver network to get the driven values. */
      ఒకవేళ (resolv_core*resolver = dynamic_cast<resolv_core*>(fun)) {
            resolver->లెక్క_drivers(idx, counts);
            తిరిగి;
      }
      ఒకవేళ (vvp_island_పోర్టు*resolver = dynamic_cast<vvp_island_పోర్టు*>(fun)) {
            resolver->లెక్క_drivers(idx, counts);
            తిరిగి;
      }

        /* If the functor is not a resolver, there is only one driver, so
           we can just interrogate the filter to get the driven value. */
      update_driver_counts(wire->driven_విలువ(idx), counts);
}

శూన్య vvp_net_fun_t::క్రియాకారకం తొలగించు(శూన్య*)
{
#నిర్వచితం_కాకపోతే CHECK_WITH_VALGRIND
      ప్రతిజ్ఞా(0);
#అయితే_ముగింపు
}


vvp_net_fil_t::vvp_net_fil_t()
{
      force_link_ = 0;
      force_propagate_ = అబద్ధం;
      లెక్క_filters += 1;
}

vvp_net_fil_t::~vvp_net_fil_t()
{
      ప్రతిజ్ఞా(force_link_ == 0);
}

vvp_net_fil_t::prop_t vvp_net_fil_t::filter_vec4(స్థిరరాశి vvp_vector4_t&,
                                                 vvp_vector4_t&,
                                                 గుర్తులేని, గుర్తులేని)
{
      తిరిగి PROP;
}

vvp_net_fil_t::prop_t vvp_net_fil_t::filter_vec8(స్థిరరాశి vvp_vector8_t&,
                                                 vvp_vector8_t&,
                                                 గుర్తులేని, గుర్తులేని)
{
      తిరిగి PROP;
}

vvp_net_fil_t::prop_t vvp_net_fil_t::filter_real(ద్విగుణ&)
{
      తిరిగి PROP;
}

vvp_net_fil_t::prop_t vvp_net_fil_t::filter_string(స్థిరరాశి string&)
{
      తిరిగి PROP;
}

vvp_net_fil_t::prop_t vvp_net_fil_t::filter_object(vvp_object_t&)
{
      తిరిగి PROP;
}

శూన్య vvp_net_fil_t::force_ముసుగు(స్థిరరాశి vvp_vector2_t&ముసుగు)
{
      ఒకవేళ (force_ముసుగు_2.size() == 0)
	    force_ముసుగు_2 = vvp_vector2_t(vvp_vector2_t::FILL0, ముసుగు.size());

      ప్రతిజ్ఞా(force_ముసుగు_2.size() == ముసుగు.size());
      కొరకు (గుర్తులేని idx = 0 ; idx < ముసుగు.size() ; idx += 1) {
	    ఒకవేళ (ముసుగు.value(idx) == 0)
		  కొనసాగు;

	    force_ముసుగు_2.సమితి_బిట్టు(idx, 1);
	    force_propagate_ = నిజం;
      }
}

శూన్య vvp_net_fil_t::release_ముసుగు(స్థిరరాశి vvp_vector2_t&ముసుగు)
{
      ఒకవేళ (force_ముసుగు_2.size() == 0)
	    తిరిగి;

      ప్రతిజ్ఞా(force_ముసుగు_2.size() == ముసుగు.size());
      కొరకు (గుర్తులేని idx = 0 ; idx < ముసుగు.size() ; idx += 1) {
	    ఒకవేళ (ముసుగు.value(idx))
		  force_ముసుగు_2.సమితి_బిట్టు(idx, 0);
      }

      ఒకవేళ (force_ముసుగు_2.is_zero())
	    force_ముసుగు_2 = vvp_vector2_t();
}

/*
 * Force link/unlink uses a thunk vvp_net_t node with a vvp_fun_force
 * functor to translate the net values to filter commands. The ports
 * of this vvp_net_t object are use a little differently:
 *
 *     port[3]  - Point to the destination node where the forced
 *                filter resides.
 *
 *     port[2]  - Point to the input node that drives port[0] for use
 *                by the unlink method.
 *
 *     port[0]  - This is the normal input.
 */
శూన్య vvp_net_fil_t::force_link(vvp_net_t*dst, vvp_net_t*src)
{
      ప్రతిజ్ఞా(dst->fil == ఇది);

      ఒకవేళ (force_link_ == 0) {
	    force_link_ = కొత్త vvp_net_t;
	      // Use port[3] to hold the force destination.
	    force_link_->పోర్టు[3] = vvp_net_ptr_t(dst, 0);
	    force_link_->పోర్టు[2] = vvp_net_ptr_t(0,0);
	    force_link_->fun = కొత్త vvp_fun_force;
      }

      force_unlink();
      ప్రతిజ్ఞా(force_link_->పోర్టు[2] == vvp_net_ptr_t(0,0));

	// Use port[2] to hold the force source.
      force_link_->పోర్టు[2] = vvp_net_ptr_t(src,0);

      vvp_net_ptr_t dst_ptr(force_link_, 0);
      src->link(dst_ptr);
}

శూన్య vvp_net_fil_t::క్రియాకారకం తొలగించు(శూన్య*)
{
      ప్రతిజ్ఞా(0);
}

శూన్య vvp_net_fil_t::force_unlink(శూన్య)
{
      ఒకవేళ (force_link_ == 0) తిరిగి;
      vvp_net_t*src = force_link_->పోర్టు[2].ptr();
      ఒకవేళ (src == 0) తిరిగి;

      src->unlink(vvp_net_ptr_t(force_link_,0));
      force_link_->పోర్టు[2] = vvp_net_ptr_t(0,0);
}

/* *** BIT operations *** */
vvp_bit4_t కూడు_with_వాహకం(vvp_bit4_t a, vvp_bit4_t b, vvp_bit4_t&c)
{
      ఒకవేళ (bit4_is_xz(a) || bit4_is_xz(b) || bit4_is_xz(c)) {
	    c = BIT4_X;
	    తిరిగి BIT4_X;
      }

	// NOTE: This relies on the facts that XZ values have been
	// weeded out, and that BIT4_1 is 1 and BIT4_0 is 0.
      పూర్ణాంకం కూడిక = (పూర్ణాంకం)a + (పూర్ణాంకం)b + (పూర్ణాంకం)c;

      ఎంపిక (కూడిక) {
	  స్థితి 0:
	      // c must already be 0.
	    తిరిగి BIT4_0;
	  స్థితి 1:
	    c = BIT4_0;
	    తిరిగి BIT4_1;
	  స్థితి 2:
	    c = BIT4_1;
	    తిరిగి BIT4_0;
	  స్థితి 3:
	    c = BIT4_1;
	    తిరిగి BIT4_1;
      }
      fprintf(stderr, "Incorrect result %d.\n", కూడిక);
      ప్రతిజ్ఞా(0);
      తిరిగి BIT4_X;
}

vvp_bit4_t scalar_గమ్యం_bit4(PLI_INT32 scalar)
{
      ఎంపిక(scalar) {
	  స్థితి vpi0:
	    తిరిగి BIT4_0;
	  స్థితి vpi1:
	    తిరిగి BIT4_1;
	  స్థితి vpiX:
	    తిరిగి BIT4_X;
	  స్థితి vpiZ:
	    తిరిగి BIT4_Z;
      }
      fprintf(stderr, "Unsupported scalar value %d.\n", (పూర్ణాంకం)scalar);
      ప్రతిజ్ఞా(0);
      తిరిగి BIT4_X;
}

vvp_bit4_t క్రియాకారకం ^ (vvp_bit4_t a, vvp_bit4_t b)
{
      ఒకవేళ (bit4_is_xz(a))
	    తిరిగి BIT4_X;
      ఒకవేళ (bit4_is_xz(b))
	    తిరిగి BIT4_X;
      ఒకవేళ (a == BIT4_0)
	    తిరిగి b;
      ఒకవేళ (b == BIT4_0)
	    తిరిగి a;
      తిరిగి BIT4_0;
}

ostream& క్రియాకారకం<<(ostream&out, vvp_bit4_t బిట్టు)
{
      ఎంపిక (బిట్టు) {
	  స్థితి BIT4_0:
	    out << "0";
	    ఆపు;
	  స్థితి BIT4_1:
	    out << "1";
	    ఆపు;
	  స్థితి BIT4_X:
	    out << "X";
	    ఆపు;
	  స్థితి BIT4_Z:
	    out << "Z";
	    ఆపు;
	  అప్రమేయ:
	    out << "?";
	    ఆపు;
      }
      తిరిగి out;
}

రకనిర్వచనం గుర్తులేని పొట్టి అంచు_t;

అంతఃవరుస అంచు_t VVP_అంచు(vvp_bit4_t from, vvp_bit4_t to)
{
      తిరిగి 1 << ((from << 2) | to);
}

స్థిరరాశి అంచు_t vvp_అంచు_posedge
      = VVP_అంచు(BIT4_0,BIT4_1)
      | VVP_అంచు(BIT4_0,BIT4_X)
      | VVP_అంచు(BIT4_0,BIT4_Z)
      | VVP_అంచు(BIT4_X,BIT4_1)
      | VVP_అంచు(BIT4_Z,BIT4_1)
      ;

స్థిరరాశి అంచు_t vvp_అంచు_negedge
      = VVP_అంచు(BIT4_1,BIT4_0)
      | VVP_అంచు(BIT4_1,BIT4_X)
      | VVP_అంచు(BIT4_1,BIT4_Z)
      | VVP_అంచు(BIT4_X,BIT4_0)
      | VVP_అంచు(BIT4_Z,BIT4_0)
      ;

పూర్ణాంకం అంచు(vvp_bit4_t from, vvp_bit4_t to)
{
      అంచు_t ముసుగు = VVP_అంచు(from, to);
      ఒకవేళ (ముసుగు & vvp_అంచు_posedge)
	    తిరిగి 1;
      ఒకవేళ (ముసుగు & vvp_అంచు_negedge)
	    తిరిగి -1;
      తిరిగి 0;
}

గుర్తులేని దీర్ఘ multiply_with_వాహకం(గుర్తులేని దీర్ఘ a, గుర్తులేని దీర్ఘ b,
				  గుర్తులేని దీర్ఘ&వాహకం)
{
      స్థిరరాశి గుర్తులేని దీర్ఘ ముసుగు = (1UL << (CPU_మాట_బిట్లు/2)) - 1;
      గుర్తులేని దీర్ఘ a0 = a & ముసుగు;
      గుర్తులేని దీర్ఘ a1 = (a >> (CPU_మాట_బిట్లు/2)) & ముసుగు;
      గుర్తులేని దీర్ఘ b0 = b & ముసుగు;
      గుర్తులేని దీర్ఘ b1 = (b >> (CPU_మాట_బిట్లు/2)) & ముసుగు;

      గుర్తులేని దీర్ఘ tmp = a0 * b0;

      గుర్తులేని దీర్ఘ r00 = tmp & ముసుగు;
      గుర్తులేని దీర్ఘ c00 = (tmp >> (CPU_మాట_బిట్లు/2)) & ముసుగు;

      tmp = a0 * b1;

      గుర్తులేని దీర్ఘ r01 = tmp & ముసుగు;
      గుర్తులేని దీర్ఘ c01 = (tmp >> (CPU_మాట_బిట్లు/2)) & ముసుగు;

      tmp = a1 * b0;

      గుర్తులేని దీర్ఘ r10 = tmp & ముసుగు;
      గుర్తులేని దీర్ఘ c10 = (tmp >> (CPU_మాట_బిట్లు/2)) & ముసుగు;

      tmp = a1 * b1;

      గుర్తులేని దీర్ఘ r11 = tmp & ముసుగు;
      గుర్తులేని దీర్ఘ c11 = (tmp >> (CPU_మాట_బిట్లు/2)) & ముసుగు;

      గుర్తులేని దీర్ఘ r1 = c00 + r01 + r10;
      గుర్తులేని దీర్ఘ r2 = (r1 >> (CPU_మాట_బిట్లు/2)) & ముసుగు;
      r1 &= ముసుగు;
      r2 += c01 + c10 + r11;
      గుర్తులేని దీర్ఘ r3 = (r2 >> (CPU_మాట_బిట్లు/2)) & ముసుగు;
      r2 &= ముసుగు;
      r3 += c11;
      r3 &= ముసుగు;

      వాహకం = (r3 << (CPU_మాట_బిట్లు/2)) + r2;
      తిరిగి (r1 << (CPU_మాట_బిట్లు/2)) + r00;
}


శూన్య vvp_send_vec8(vvp_net_ptr_t ptr, స్థిరరాశి vvp_vector8_t&val)
{
      అయితే (vvp_net_t*cur = ptr.ptr()) {
	    vvp_net_ptr_t తదుపరి_val = cur->పోర్టు[ptr.పోర్టు()];

	    ఒకవేళ (cur->fun)
		  cur->fun->recv_vec8(ptr, val);

	    ptr = తదుపరి_val;
      }
}

శూన్య vvp_send_real(vvp_net_ptr_t ptr, ద్విగుణ val, vvp_context_t context)
{
      అయితే (vvp_net_t*cur = ptr.ptr()) {
	    vvp_net_ptr_t తదుపరి_val = cur->పోర్టు[ptr.పోర్టు()];

	    ఒకవేళ (cur->fun)
		  cur->fun->recv_real(ptr, val, context);

	    ptr = తదుపరి_val;
      }
}

స్థిరరాశి vvp_vector4_t vvp_vector4_t::nil;

శూన్య vvp_vector4_t::copy_బిట్లు(స్థిరరాశి vvp_vector4_t&that)
{

      ఒకవేళ (కొలత_2 == that.కొలత_2) {
	    ఒకవేళ (కొలత_2 > బిట్లు_చొప్పున_మాట) {
		  గుర్తులేని పదాలు = (కొలత_2+బిట్లు_చొప్పున_మాట-1) / బిట్లు_చొప్పున_మాట;
		  కొరకు (గుర్తులేని idx = 0 ;  idx < పదాలు ;  idx += 1)
			abits_ptr_[idx] = that.abits_ptr_[idx];
		  కొరకు (గుర్తులేని idx = 0 ;  idx < పదాలు ;  idx += 1)
			bbits_ptr_[idx] = that.bbits_ptr_[idx];
	    } లేకపోతే {
		  abits_val_ = that.abits_val_;
		  bbits_val_ = that.bbits_val_;
	    }
	    తిరిగి;
      }

	/* Now we know that the sizes of this and that are definitely
	   different. We can use that in code below. In any case, we
	   need to copy only the smaller of the sizes. */

	/* If source and destination are both short, then mask/copy
	   the bit values. */
      ఒకవేళ (కొలత_2 <= బిట్లు_చొప్పున_మాట && that.కొలత_2 <= బిట్లు_చొప్పున_మాట) {
	    గుర్తులేని బిట్లు_గమ్యం_copy = (that.కొలత_2 < కొలత_2) ? that.కొలత_2 : కొలత_2;
	    గుర్తులేని దీర్ఘ ముసుగు = ((బిట్లు_గమ్యం_copy < బిట్లు_చొప్పున_మాట) ? (1UL << బిట్లు_గమ్యం_copy) :
	                                                           (0UL)) - 1UL;
	    abits_val_ &= ~ముసుగు;
	    bbits_val_ &= ~ముసుగు;
	    abits_val_ |= that.abits_val_&ముసుగు;
	    bbits_val_ |= that.bbits_val_&ముసుగు;
	    తిరిగి;
      }

	/* Now we know that either source or destination are long. If
	   the destination is short, then mask/copy from the low word
	   of the long source. */
      ఒకవేళ (కొలత_2 <= బిట్లు_చొప్పున_మాట) {
	    abits_val_ = that.abits_ptr_[0];
	    bbits_val_ = that.bbits_ptr_[0];
	    ఒకవేళ (కొలత_2 < బిట్లు_చొప్పున_మాట) {
		  గుర్తులేని దీర్ఘ ముసుగు = (1UL << కొలత_2) - 1UL;
		  abits_val_ &= ముసుగు;
		  bbits_val_ &= ముసుగు;
	    }
	    తిరిగి;
      }

	/* Now we know that the destination must be long. If the
	   source is short, then mask/copy from its value. */
      ఒకవేళ (that.కొలత_2 <= బిట్లు_చొప్పున_మాట) {
	    గుర్తులేని దీర్ఘ ముసుగు;
	    ఒకవేళ (that.కొలత_2 < బిట్లు_చొప్పున_మాట) {
		  ముసుగు = (1UL << that.కొలత_2) - 1UL;
		  abits_ptr_[0] &= ~ముసుగు;
		  bbits_ptr_[0] &= ~ముసుగు;
	    } లేకపోతే {
		  ముసుగు = -1UL;
	    }
	    abits_ptr_[0] |= that.abits_val_&ముసుగు;
	    bbits_ptr_[0] |= that.bbits_val_&ముసుగు;
	    తిరిగి;
      }

	/* Finally, we know that source and destination are long. copy
	   words until we get to the last. */
      గుర్తులేని బిట్లు_గమ్యం_copy = (that.కొలత_2 < కొలత_2) ? that.కొలత_2 : కొలత_2;
      గుర్తులేని మాట = 0;
      అయితే (బిట్లు_గమ్యం_copy >= బిట్లు_చొప్పున_మాట) {
	    abits_ptr_[మాట] = that.abits_ptr_[మాట];
	    bbits_ptr_[మాట] = that.bbits_ptr_[మాట];
	    బిట్లు_గమ్యం_copy -= బిట్లు_చొప్పున_మాట;
	    మాట += 1;
      }
      ఒకవేళ (బిట్లు_గమ్యం_copy > 0) {
	    గుర్తులేని దీర్ఘ ముసుగు = (1UL << బిట్లు_గమ్యం_copy) - 1UL;
	    abits_ptr_[మాట] &= ~ముసుగు;
	    bbits_ptr_[మాట] &= ~ముసుగు;
	    abits_ptr_[మాట] |= that.abits_ptr_[మాట] & ముసుగు;
	    bbits_ptr_[మాట] |= that.bbits_ptr_[మాట] & ముసుగు;
      }
}

/*
 * This function should ONLY BE CALLED FROM vvp_vector4_t::copy_from_,
 * as it performs part of that functions tasks.
 */
శూన్య vvp_vector4_t::copy_from_big_(స్థిరరాశి vvp_vector4_t&that)
{
      గుర్తులేని పదాలు = (కొలత_2+బిట్లు_చొప్పున_మాట-1) / బిట్లు_చొప్పున_మాట;
      abits_ptr_ = కొత్త గుర్తులేని దీర్ఘ[2*పదాలు];
      bbits_ptr_ = abits_ptr_ + పదాలు;

      కొరకు (గుర్తులేని idx = 0 ;  idx < పదాలు ;  idx += 1)
	    abits_ptr_[idx] = that.abits_ptr_[idx];
      కొరకు (గుర్తులేని idx = 0 ;  idx < పదాలు ;  idx += 1)
	    bbits_ptr_[idx] = that.bbits_ptr_[idx];
}

/*
 * The copy_inverted_from_ method is just like the copy_from_ method,
 * except that we combine that with an invert. This allows the ~ and
 * the assignment to be blended in many common cases.
 */
శూన్య vvp_vector4_t::copy_inverted_from_(స్థిరరాశి vvp_vector4_t&that)
{
      కొలత_2 = that.కొలత_2;
      ఒకవేళ (కొలత_2 > బిట్లు_చొప్పున_మాట) {
	    గుర్తులేని పదాలు = (కొలత_2+బిట్లు_చొప్పున_మాట-1) / బిట్లు_చొప్పున_మాట;
	    abits_ptr_ = కొత్త గుర్తులేని దీర్ఘ[2*పదాలు];
	    bbits_ptr_ = abits_ptr_ + పదాలు;

	    గుర్తులేని మిగిలిన = కొలత_2;
	    గుర్తులేని idx = 0;
	    అయితే (మిగిలిన >= బిట్లు_చొప్పున_మాట) {
		  abits_ptr_[idx] = that.bbits_ptr_[idx] | ~that.abits_ptr_[idx];
		  idx += 1;
		  మిగిలిన -= బిట్లు_చొప్పున_మాట;
	    }
	    ఒకవేళ (మిగిలిన > 0) {
		  గుర్తులేని దీర్ఘ ముసుగు = (1UL<<మిగిలిన) - 1UL;
		  abits_ptr_[idx] = ముసుగు & (that.bbits_ptr_[idx] | ~that.abits_ptr_[idx]);
	    }

	    కొరకు (idx = 0 ;  idx < పదాలు ;  idx += 1)
		  bbits_ptr_[idx] = that.bbits_ptr_[idx];

      } లేకపోతే {
	    గుర్తులేని దీర్ఘ ముసుగు = (కొలత_2<బిట్లు_చొప్పున_మాట)? (1UL<<కొలత_2)-1UL : -1UL;
	    abits_val_ = ముసుగు & (that.bbits_val_ | ~that.abits_val_);
	    bbits_val_ = that.bbits_val_;
      }
}

/* Make sure to set size_ before calling this routine. */
శూన్య vvp_vector4_t::allocate_పదాలు(గుర్తులేని దీర్ఘ inita, గుర్తులేని దీర్ఘ initb)
{
      ఒకవేళ (కొలత_2 > బిట్లు_చొప్పున_మాట) {
	    గుర్తులేని cnt = (కొలత_2 + బిట్లు_చొప్పున_మాట - 1) / బిట్లు_చొప్పున_మాట;
	    abits_ptr_ = కొత్త గుర్తులేని దీర్ఘ[2*cnt];
	    bbits_ptr_ = abits_ptr_ + cnt;
	    కొరకు (గుర్తులేని idx = 0 ;  idx < cnt ;  idx += 1)
		  abits_ptr_[idx] = inita;
	    కొరకు (గుర్తులేని idx = 0 ;  idx < cnt ;  idx += 1)
		  bbits_ptr_[idx] = initb;

      } లేకపోతే {
	    abits_val_ = inita;
	    bbits_val_ = initb;
      }
}

vvp_vector4_t::vvp_vector4_t(గుర్తులేని కొలత_3, ద్విగుణ val)
: కొలత_2(కొలత_3)
{
      తార్కిక is_neg = అబద్ధం;
      ద్విగుణ fraction;
      పూర్ణాంకం exponent;

	/* We return 'bx for a NaN or +/- infinity. */
      ఒకవేళ (val != val || (val && (val == 0.5*val)))  {
	    allocate_పదాలు(మాట_X_ABITS, మాట_X_BBITS);
	    తిరిగి;
      }

	/* Convert to a positive result. */
      ఒకవేళ (val < 0.0) {
	    is_neg = నిజం;
	    val = -val;
      }
      allocate_పదాలు(మాట_0_ABITS, మాట_0_BBITS);

	/* Get the exponent and fractional part of the number. */
      fraction = frexp(val, &exponent);

	/* If the value is small enough just use lround(). */
      ఒకవేళ (exponent < బిట్లు_చొప్పున_మాట-2) {
	    ఒకవేళ (is_neg) ఇది->invert();  // Invert the bits if negative.
	    దీర్ఘ sval = lround(val);
	    ఒకవేళ (is_neg) sval = -sval;
	      /* This requires that 0 and 1 have the same bbit value. */
	    ఒకవేళ (కొలత_2 > బిట్లు_చొప్పున_మాట) {
		  abits_ptr_[0] = sval;
	    } లేకపోతే {
		  abits_val_ = sval;
	    }
	    తిరిగి;
      }

      గుర్తులేని nwords = (exponent-1) / బిట్లు_చొప్పున_మాట;
      గుర్తులేని my_పదాలు = (కొలత_2 + బిట్లు_చొప్పున_మాట - 1) / బిట్లు_చొప్పున_మాట - 1;

      fraction = ldexp(fraction, (exponent-1) % బిట్లు_చొప్పున_మాట + 1);

	/* Skip any leading bits. */
      కొరకు (పూర్ణాంకం idx = (గుర్తుగల) nwords; idx > (గుర్తుగల) my_పదాలు; idx -=1) {
	    గుర్తులేని దీర్ఘ బిట్లు = (గుర్తులేని దీర్ఘ) fraction;
	    fraction = fraction - (ద్విగుణ) బిట్లు;
	    fraction = ldexp(fraction, బిట్లు_చొప్పున_మాట);
      }

	/* Convert the remaining bits as appropriate. */
      ఒకవేళ (my_పదాలు == 0) {
		  గుర్తులేని దీర్ఘ బిట్లు = (గుర్తులేని దీర్ఘ) fraction;
		  abits_val_ = బిట్లు;
		  fraction = fraction - (ద్విగుణ) బిట్లు;
		    /* Round any fractional part up. */
		  ఒకవేళ (fraction >= 0.5) *ఇది += (int64_t) 1;
      } లేకపోతే {
	    ఒకవేళ (nwords < my_పదాలు) my_పదాలు = nwords;
	    కొరకు (పూర్ణాంకం idx = (గుర్తుగల)my_పదాలు; idx >= 0; idx -= 1) {
		  గుర్తులేని దీర్ఘ బిట్లు = (గుర్తులేని దీర్ఘ) fraction;
		  abits_ptr_[idx] = బిట్లు;
		  fraction = fraction - (ద్విగుణ) బిట్లు;
		  fraction = ldexp(fraction, బిట్లు_చొప్పున_మాట);
	    }
	      /* Round any fractional part up. */
	    ఒకవేళ (fraction >= ldexp(0.5, బిట్లు_చొప్పున_మాట)) *ఇది += (int64_t) 1;
       }

	/* Convert to a negative number if needed. */
      ఒకవేళ (is_neg) {
	    ఇది->invert();
	    *ఇది += (int64_t) 1;
      }
}

vvp_vector4_t::vvp_vector4_t(స్థిరరాశి vvp_vector4_t&that,
			    గుర్తులేని adr, గుర్తులేని wid)
{
	// Set up and initialize the destination.
      కొలత_2 = wid;
      allocate_పదాలు(మాట_X_ABITS, మాట_X_BBITS);

	// Special case: selecting from far beyond the source vector,
	// to the result is all X bits. We're done.
      ఒకవేళ (adr >= that.కొలత_2)
	    తిరిగి;

	// Special case: The source is not quite big enough to supply
	// all bits, so get the bits that we can. The remainder will
	// be left at BIT4_X.
      ఒకవేళ ((adr + wid) > that.కొలత_2) {
	    గుర్తులేని use_wid = that.కొలత_2 - adr;
	    కొరకు (గుర్తులేని idx = 0 ; idx < use_wid ; idx += 1)
		  సమితి_బిట్టు(idx, that.value(adr+idx));

	    తిరిగి;
      }

	// At the point, we know that the source part is entirely
	// contained in the source vector.
	// assert((adr + wid) <= that.size_);

      ఒకవేళ (wid > బిట్లు_చొప్పున_మాట) {
	      /* In this case, the subvector and the source vector are
		 long. Do the transfer reasonably efficiently. */
	    గుర్తులేని ptr = adr / బిట్లు_చొప్పున_మాట;
	    గుర్తులేని దీర్ఘ off = adr % బిట్లు_చొప్పున_మాట;
	    గుర్తులేని దీర్ఘ noff = బిట్లు_చొప్పున_మాట - off;
	    గుర్తులేని దీర్ఘ lmask = (1UL << off) - 1UL;
	    గుర్తులేని trans = 0;
	    గుర్తులేని dst = 0;
	    అయితే (trans < wid) {
		    // The low bits of the result.
		  abits_ptr_[dst] = (that.abits_ptr_[ptr] & ~lmask) >> off;
		  bbits_ptr_[dst] = (that.bbits_ptr_[ptr] & ~lmask) >> off;
		  trans += noff;

		  ఒకవేళ (trans >= wid)
			ఆపు;

		  ptr += 1;

		    // The high bits of the result. Skip this if the
		    // source and destination are perfectly aligned.
		  ఒకవేళ (noff != బిట్లు_చొప్పున_మాట) {
			abits_ptr_[dst] |= (that.abits_ptr_[ptr]&lmask) << noff;
			bbits_ptr_[dst] |= (that.bbits_ptr_[ptr]&lmask) << noff;
			trans += off;
		  }

		  dst += 1;
	    }

      } లేకపోతే ఒకవేళ (that.కొలత_2 > బిట్లు_చొప్పున_మాట) {
	      /* In this case, the subvector fits in a single word,
		 but the source is large. */
	    గుర్తులేని ptr = adr / బిట్లు_చొప్పున_మాట;
	    గుర్తులేని దీర్ఘ off = adr % బిట్లు_చొప్పున_మాట;
	    గుర్తులేని trans = బిట్లు_చొప్పున_మాట - off;
	    ఒకవేళ (trans > wid)
		  trans = wid;

	    ఒకవేళ (trans == బిట్లు_చొప్పున_మాట) {
		    // Very special case: Copy exactly 1 perfectly
		    // aligned word.
		  abits_val_ = that.abits_ptr_[ptr];
		  bbits_val_ = that.bbits_ptr_[ptr];

	    } లేకపోతే {
		    // lmask is the low bits of the destination,
		    // masked into the source.
		  గుర్తులేని దీర్ఘ lmask = (1UL<<trans) - 1UL;
		  lmask <<= off;

		    // The low bits of the result.
		  abits_val_ = (that.abits_ptr_[ptr] & lmask) >> off;
		  bbits_val_ = (that.bbits_ptr_[ptr] & lmask) >> off;

		  ఒకవేళ (trans < wid) {
			  // If there are more bits, then get them
			  // from the bottom of the next word of the
			  // source.
			గుర్తులేని దీర్ఘ hmask = (1UL << (wid-trans)) - 1UL;

			  // The high bits of the result.
			abits_val_ |= (that.abits_ptr_[ptr+1]&hmask) << trans;
			bbits_val_ |= (that.bbits_ptr_[ptr+1]&hmask) << trans;
		  }
	    }

      } లేకపోతే ఒకవేళ (కొలత_2 == బిట్లు_చొప్పున_మాట) {
	      /* We know that source and destination are short. If the
		 destination is a full word, then we know the copy is
		 aligned and complete. */
	    abits_val_ = that.abits_val_;
	    bbits_val_ = that.bbits_val_;

      } లేకపోతే {
	      /* Finally, the source and destination vectors are both
		 short, so there is a single mask/shift/copy. */
	    గుర్తులేని దీర్ఘ ముసుగు = (1UL << కొలత_2) - 1UL;
	    ముసుగు <<= adr;

	    abits_val_ = (that.abits_val_ & ముసుగు) >> adr;
	    bbits_val_ = (that.bbits_val_ & ముసుగు) >> adr;
      }

}

/*
 * Change the size of the vvp_vector4_t vector to the new size. Copy
 * the old values, as many as well fit, into the new vector.
 */
శూన్య vvp_vector4_t::resize(గుర్తులేని newsize, vvp_bit4_t pad_బిట్టు)
{
      ఒకవేళ (కొలత_2 == newsize)
	    తిరిగి;

      గుర్తులేని cnt = (కొలత_2 + బిట్లు_చొప్పున_మాట - 1) / బిట్లు_చొప్పున_మాట;
      గుర్తులేని దీర్ఘ మాట_pad_abits = 0;
      గుర్తులేని దీర్ఘ మాట_pad_bbits = 0;
      ఎంపిక (pad_బిట్టు) {
	  స్థితి BIT4_0:
	    మాట_pad_abits = మాట_0_ABITS;
	    మాట_pad_bbits = మాట_0_BBITS;
	    ఆపు;
	  స్థితి BIT4_1:
	    మాట_pad_abits = మాట_1_ABITS;
	    మాట_pad_bbits = మాట_1_BBITS;
	    ఆపు;
	  స్థితి BIT4_X:
	    మాట_pad_abits = మాట_X_ABITS;
	    మాట_pad_bbits = మాట_X_BBITS;
	    ఆపు;
	  స్థితి BIT4_Z:
	    మాట_pad_abits = మాట_Z_ABITS;
	    మాట_pad_bbits = మాట_Z_BBITS;
	    ఆపు;
      }

      ఒకవేళ (newsize > బిట్లు_చొప్పున_మాట) {
	    గుర్తులేని newcnt = (newsize + బిట్లు_చొప్పున_మాట - 1) / బిట్లు_చొప్పున_మాట;
	    ఒకవేళ (newcnt == cnt) {
		    // If the word count doesn't change, then there is
		    // no need for re-allocation so we are done now.
		  ఒకవేళ (newsize > కొలత_2) {
			ఒకవేళ (గుర్తులేని fill = కొలత_2 % బిట్లు_చొప్పున_మాట) {
			      abits_ptr_[cnt-1] &= ~((-1UL) << fill);
			      bbits_ptr_[cnt-1] &= ~((-1UL) << fill);
			      abits_ptr_[cnt-1] |= మాట_pad_abits << fill;
			      bbits_ptr_[cnt-1] |= మాట_pad_bbits << fill;
			}
		  }
		  కొలత_2 = newsize;
		  తిరిగి;
	    }

	    గుర్తులేని దీర్ఘ*newbits = కొత్త గుర్తులేని దీర్ఘ[2*newcnt];

	    ఒకవేళ (cnt > 1) {
		  గుర్తులేని trans = cnt;
		  ఒకవేళ (trans > newcnt)
			trans = newcnt;

		  కొరకు (గుర్తులేని idx = 0 ;  idx < trans ;  idx += 1)
			newbits[idx] = abits_ptr_[idx];
		  కొరకు (గుర్తులేని idx = 0 ;  idx < trans ;  idx += 1)
			newbits[newcnt+idx] = bbits_ptr_[idx];

		  తొలగించు[]abits_ptr_;

	    } లేకపోతే {
		  newbits[0] = abits_val_;
		  newbits[newcnt] = bbits_val_;
	    }

	    ఒకవేళ (newsize > కొలత_2) {
		  ఒకవేళ (గుర్తులేని fill = కొలత_2 % బిట్లు_చొప్పున_మాట) {
			newbits[cnt-1] &= ~((-1UL) << fill);
			newbits[cnt-1] |= మాట_pad_abits << fill;
			newbits[newcnt+cnt-1] &= ~((-1UL) << fill);
			newbits[newcnt+cnt-1] |= మాట_pad_bbits << fill;
		  }
		  కొరకు (గుర్తులేని idx = cnt ;  idx < newcnt ;  idx += 1)
			newbits[idx] = మాట_pad_abits;
		  కొరకు (గుర్తులేని idx = cnt ;  idx < newcnt ;  idx += 1)
			newbits[newcnt+idx] = మాట_pad_bbits;
	    }

	    కొలత_2 = newsize;
	    abits_ptr_ = newbits;
	    bbits_ptr_ = newbits + newcnt;

      } లేకపోతే {
	    ఒకవేళ (cnt > 1) {
		  గుర్తులేని దీర్ఘ newvala = abits_ptr_[0];
		  గుర్తులేని దీర్ఘ newvalb = bbits_ptr_[0];
		  తొలగించు[]abits_ptr_;
		  abits_val_ = newvala;
		  bbits_val_ = newvalb;
	    }

	    ఒకవేళ (newsize > కొలత_2) {
		  abits_val_ &= ~((-1UL) << కొలత_2);
		  bbits_val_ &= ~((-1UL) << కొలత_2);
		  abits_val_ |= మాట_pad_abits << కొలత_2;
		  bbits_val_ |= మాట_pad_bbits << కొలత_2;
	    }

	    కొలత_2 = newsize;
      }
}


గుర్తులేని దీర్ఘ* vvp_vector4_t::subarray(గుర్తులేని adr, గుర్తులేని wid, తార్కిక xz_గమ్యం_0) స్థిరరాశి
{
      స్థిరరాశి గుర్తులేని BIT2_చొప్పున_మాట = 8*పరిమాణం(గుర్తులేని దీర్ఘ);
      గుర్తులేని awid = (wid + BIT2_చొప్పున_మాట - 1) / (BIT2_చొప్పున_మాట);
      గుర్తులేని దీర్ఘ*val = కొత్త గుర్తులేని దీర్ఘ[awid];

      కొరకు (గుర్తులేని idx = 0 ;  idx < awid ;  idx += 1)
	    val[idx] = 0;

      ఒకవేళ (కొలత_2 <= బిట్లు_చొప్పున_మాట) {
	      /* Handle the special case that the array is small. The
		 entire value of the vector4 is within the xbits_val_
		 so we know that the result is a single word, the
		 source is a single word, and we just have to loop
		 through that word. */
	    గుర్తులేని దీర్ఘ atmp = abits_val_ >> adr;
	    గుర్తులేని దీర్ఘ btmp = bbits_val_ >> adr;
	    ఒకవేళ (wid < BIT2_చొప్పున_మాట) {
		  atmp &= (1UL << wid) - 1;
		  btmp &= (1UL << wid) - 1;
	    }
	    ఒకవేళ (btmp) {
		  ఒకవేళ (xz_గమ్యం_0)
			atmp &= ~btmp;
		  లేకపోతే
			వెళ్ళు x_బహిర్గతం;
	    }

	    val[0] = atmp;

      } లేకపోతే {

	    గుర్తులేని val_ptr = 0;
	    గుర్తులేని val_off = 0;

	      /* Get the first word we are scanning. We may in fact be
		 somewhere in the middle of that word. */
	    అయితే (wid > 0) {
		  గుర్తులేని దీర్ఘ atmp = abits_ptr_[adr/బిట్లు_చొప్పున_మాట];
		  గుర్తులేని దీర్ఘ btmp = bbits_ptr_[adr/బిట్లు_చొప్పున_మాట];
		  గుర్తులేని దీర్ఘ off = adr%బిట్లు_చొప్పున_మాట;
		  atmp >>= off;
		  btmp >>= off;

		  గుర్తులేని దీర్ఘ trans = బిట్లు_చొప్పున_మాట - off;
		  ఒకవేళ (trans > (BIT2_చొప్పున_మాట - val_off))
			trans = BIT2_చొప్పున_మాట - val_off;
		  ఒకవేళ (wid < trans)
			trans = wid;
		  ఒకవేళ (trans < BIT2_చొప్పున_మాట) {
			atmp &= (1UL << trans) - 1;
			btmp &= (1UL << trans) - 1;
		  }
		  ఒకవేళ (btmp) {
			ఒకవేళ (xz_గమ్యం_0)
			      atmp &= ~btmp;
			లేకపోతే
			      వెళ్ళు x_బహిర్గతం;
		  }

		  val[val_ptr] |= atmp << val_off;
		  adr += trans;
		  wid -= trans;
		  val_off += trans;
		  ఒకవేళ (val_off == BIT2_చొప్పున_మాట) {
			val_ptr += 1;
			val_off = 0;
		  }
	    }
      }

      తిరిగి val;

 x_బహిర్గతం:
      తొలగించు[]val;
      తిరిగి 0;
}

శూన్య vvp_vector4_t::setarray(గుర్తులేని adr, గుర్తులేని wid, స్థిరరాశి గుర్తులేని దీర్ఘ*val)
{
      ప్రతిజ్ఞా(adr+wid <= కొలత_2);

      స్థిరరాశి గుర్తులేని BIT2_చొప్పున_మాట = 8*పరిమాణం(గుర్తులేని దీర్ఘ);

      ఒకవేళ (కొలత_2 <= బిట్లు_చొప్పున_మాట) {
	      // We know here that both the source and the target are
	      // within a single word. Write the bits into the
	      // abits_val_ directly.

	    ప్రతిజ్ఞా(BIT2_చొప్పున_మాట <= బిట్లు_చొప్పున_మాట);
	    గుర్తులేని దీర్ఘ lmask = (1UL << adr) - 1UL;
	    గుర్తులేని దీర్ఘ hmask = ((adr+wid) < బిట్లు_చొప్పున_మాట)
		  ? -1UL << (adr+wid)
		  : 0;
	    గుర్తులేని దీర్ఘ ముసుగు = ~(hmask | lmask);

	    abits_val_ &= ~ముసుగు;
	    bbits_val_ &= ~ముసుగు;

	    abits_val_ |= ముసుగు & (val[0] << adr);

      } లేకపోతే {
	      // The general case, there are multiple words of
	      // destination, and possibly multiple words of source
	      // data. Shift and mask as we go.
	    గుర్తులేని off = adr % బిట్లు_చొప్పున_మాట;
	    గుర్తులేని ptr = adr / బిట్లు_చొప్పున_మాట;
	    గుర్తులేని val_off = 0;
	    గుర్తులేని val_ptr = 0;
	    అయితే (wid > 0) {
		  గుర్తులేని trans = wid;
		  ఒకవేళ (trans > (BIT2_చొప్పున_మాట-val_off))
			trans = BIT2_చొప్పున_మాట-val_off;
		  ఒకవేళ (trans > (బిట్లు_చొప్పున_మాట-off))
			trans = బిట్లు_చొప్పున_మాట-off;

		  గుర్తులేని దీర్ఘ lmask = (1UL << off) - 1UL;
		  గుర్తులేని దీర్ఘ hmask = ((off+trans) < బిట్లు_చొప్పున_మాట)
			? -1UL << (off+trans)
			: 0;
		  గుర్తులేని దీర్ఘ ముసుగు = ~(hmask | lmask);

		  abits_ptr_[ptr] &= ~ముసుగు;
		  bbits_ptr_[ptr] &= ~ముసుగు;
		  ఒకవేళ (val_off >= off)
			abits_ptr_[ptr] |= ముసుగు & (val[val_ptr] >> (val_off-off));
		  లేకపోతే
			abits_ptr_[ptr] |= ముసుగు & (val[val_ptr] << (off-val_off));

		  wid -= trans;
		  val_off += trans;
		  ఒకవేళ (val_off == BIT2_చొప్పున_మాట) {
			val_off = 0;
			val_ptr += 1;
		  }
		  off += trans;
		  ఒకవేళ (off == బిట్లు_చొప్పున_మాట) {
			off = 0;
			ptr += 1;
		  }
	    }
      }
}

/*
 * Set the bits of that vector, which must be a subset of this vector,
 * into the addressed part of this vector. Use bit masking and word
 * copies to go as fast as reasonably possible.
 */
తార్కిక vvp_vector4_t::సమితి_vec(గుర్తులేని adr, స్థిరరాశి vvp_vector4_t&that)
{
      ప్రతిజ్ఞా(adr+that.కొలత_2  <= కొలత_2);
      తార్కిక diff_flag = అబద్ధం;

      ఒకవేళ (కొలత_2 <= బిట్లు_చొప్పున_మాట) {

	      /* The destination vector (me!) is within a bits_val_
		 word, so the subvector is certainly within a
		 bits_val_ word. Therefore, the entire operation is a
		 matter of writing the bits of that into the addressed
		 bits of this. The mask below is calculated to be 1
		 for all the bits that are to come from that. Do the
		 job by some shifting, masking and OR. */

	    గుర్తులేని దీర్ఘ lmask = (1UL << adr) - 1;
	    గుర్తులేని దీర్ఘ hmask;
	    గుర్తులేని దీర్ఘ hshift = adr+that.కొలత_2;
	    ఒకవేళ (hshift >= బిట్లు_చొప్పున_మాట)
		  hmask = -1UL;
	    లేకపోతే
		  hmask = (1UL << (adr+that.కొలత_2)) - 1;
	    గుర్తులేని దీర్ఘ ముసుగు = hmask & ~lmask;

	    గుర్తులేని దీర్ఘ tmp = (that.abits_val_<<adr)&ముసుగు;
	    ఒకవేళ ((abits_val_&ముసుగు) != tmp) {
		  diff_flag = నిజం;
		  abits_val_ = (abits_val_ & ~ముసుగు) | tmp;
	    }
	    tmp = (that.bbits_val_<<adr) & ముసుగు;
	    ఒకవేళ ((bbits_val_&ముసుగు) != tmp) {
		  diff_flag = నిజం;
		  bbits_val_ = (bbits_val_ & ~ముసుగు) | tmp;
	    }

      } లేకపోతే ఒకవేళ (that.కొలత_2 <= బిట్లు_చొప్పున_మాట) {

	      /* This vector is more than a word, but that vector is
		 still small. Write into the destination, possibly
		 spanning two destination words, depending on whether
		 the source vector spans a word transition. */
	    గుర్తులేని దీర్ఘ dptr = adr / బిట్లు_చొప్పున_మాట;
	    గుర్తులేని దీర్ఘ doff = adr % బిట్లు_చొప్పున_మాట;

	    గుర్తులేని దీర్ఘ lmask = (1UL << doff) - 1;
	    గుర్తులేని దీర్ఘ hshift = doff+that.కొలత_2;
	    గుర్తులేని దీర్ఘ hmask;
	    ఒకవేళ (hshift >= బిట్లు_చొప్పున_మాట)
		  hmask = -1UL;
	    లేకపోతే
		  hmask = (1UL << hshift) - 1UL;

	    గుర్తులేని దీర్ఘ ముసుగు = hmask & ~lmask;
	    గుర్తులేని దీర్ఘ tmp;

	    tmp = (that.abits_val_ << doff) & ముసుగు;
	    ఒకవేళ ((abits_ptr_[dptr] & ముసుగు) != tmp) {
		  diff_flag = నిజం;
		  abits_ptr_[dptr] = (abits_ptr_[dptr] & ~ముసుగు) | tmp;
	    }
	    tmp = (that.bbits_val_ << doff) & ముసుగు;
	    ఒకవేళ ((bbits_ptr_[dptr] & ముసుగు) != tmp) {
		  diff_flag = నిజం;
		  bbits_ptr_[dptr] = (bbits_ptr_[dptr] & ~ముసుగు) | tmp;
	    }

	    ఒకవేళ ((doff + that.కొలత_2) > బిట్లు_చొప్పున_మాట) {
		  గుర్తులేని తోక = doff + that.కొలత_2 - బిట్లు_చొప్పున_మాట;
		  ముసుగు = (1UL << తోక) - 1;

		  dptr += 1;
		  tmp = (that.abits_val_ >> (that.కొలత_2-తోక)) & ముసుగు;
		  ఒకవేళ ((abits_ptr_[dptr] & ముసుగు) != tmp) {
			diff_flag = నిజం;
			abits_ptr_[dptr] = (abits_ptr_[dptr] & ~ముసుగు) | tmp;
		  }
		  tmp = (that.bbits_val_ >> (that.కొలత_2-తోక)) & ముసుగు;
		  ఒకవేళ ((bbits_ptr_[dptr] & ముసుగు) != tmp) {
			diff_flag = నిజం;
			bbits_ptr_[dptr] = (bbits_ptr_[dptr] & ~ముసుగు) | tmp;
		  }
	    }

      } లేకపోతే ఒకవేళ (adr%బిట్లు_చొప్పున_మాట == 0) {

	      /* In this case, both vectors are long, but the
		 destination is neatly aligned. That means all but the
		 last word can be simply copied with no masking. */

	    గుర్తులేని remain = that.కొలత_2;
	    గుర్తులేని sptr = 0;
	    గుర్తులేని dptr = adr / బిట్లు_చొప్పున_మాట;
	    అయితే (remain >= బిట్లు_చొప్పున_మాట) {
		  ఒకవేళ (abits_ptr_[dptr] != that.abits_ptr_[sptr]) {
			diff_flag = నిజం;
			abits_ptr_[dptr] = that.abits_ptr_[sptr];
		  }
		  ఒకవేళ (bbits_ptr_[dptr] != that.bbits_ptr_[sptr]) {
			diff_flag = నిజం;
			bbits_ptr_[dptr] = that.bbits_ptr_[sptr];
		  }
		  dptr += 1;
		  sptr += 1;
		  remain -= బిట్లు_చొప్పున_మాట;
	    }

	    ఒకవేళ (remain > 0) {
		  గుర్తులేని దీర్ఘ ముసుగు = (1UL << remain) - 1;
		  గుర్తులేని దీర్ఘ tmp;

		  tmp = that.abits_ptr_[sptr] & ముసుగు;
		  ఒకవేళ ((abits_ptr_[dptr] & ముసుగు) != tmp) {
			diff_flag = నిజం;
			abits_ptr_[dptr] = (abits_ptr_[dptr] & ~ముసుగు) | tmp;
		  }
		  tmp = that.bbits_ptr_[sptr] & ముసుగు;
		  ఒకవేళ ((bbits_ptr_[dptr] & ముసుగు) != tmp) {
			diff_flag = నిజం;
			bbits_ptr_[dptr] = (bbits_ptr_[dptr] & ~ముసుగు) | tmp;
		  }
	    }

      } లేకపోతే {

	      /* We know that there are two long vectors, and we know
		 that the destination is definitely NOT aligned. */

	    గుర్తులేని remain = that.కొలత_2;
	    గుర్తులేని sptr = 0;
	    గుర్తులేని dptr = adr / బిట్లు_చొప్పున_మాట;
	    గుర్తులేని doff = adr % బిట్లు_చొప్పున_మాట;
	    గుర్తులేని దీర్ఘ lmask = (1UL << doff) - 1;
	    గుర్తులేని ndoff = బిట్లు_చొప్పున_మాట - doff;
	    అయితే (remain >= బిట్లు_చొప్పున_మాట) {
		  గుర్తులేని దీర్ఘ tmp;

		  tmp = (that.abits_ptr_[sptr] << doff) & ~lmask;
		  ఒకవేళ ((abits_ptr_[dptr] & ~lmask) != tmp) {
			diff_flag = నిజం;
			abits_ptr_[dptr] = (abits_ptr_[dptr] & lmask) | tmp;
		  }
		  tmp = (that.bbits_ptr_[sptr] << doff) & ~lmask;
		  ఒకవేళ ((bbits_ptr_[dptr] & ~lmask) != tmp) {
			diff_flag = నిజం;
			bbits_ptr_[dptr] = (bbits_ptr_[dptr] & lmask) | tmp;
		  }
		  dptr += 1;

		  tmp = (that.abits_ptr_[sptr] >> ndoff) & lmask;
		  ఒకవేళ ((abits_ptr_[dptr] & lmask) != tmp) {
			diff_flag = నిజం;
			abits_ptr_[dptr] = (abits_ptr_[dptr] & ~lmask) | tmp;
		  }
		  tmp = (that.bbits_ptr_[sptr] >> ndoff) & lmask;
		  ఒకవేళ ((bbits_ptr_[dptr] & lmask) != tmp) {
			diff_flag = నిజం;
			bbits_ptr_[dptr] = (bbits_ptr_[dptr] & ~lmask) | tmp;
		  }

		  remain -= బిట్లు_చొప్పున_మాట;
		  sptr += 1;
	    }

	    ఒకవేళ (remain > 0) {
		  గుర్తులేని దీర్ఘ hshift = doff+remain;
		  గుర్తులేని దీర్ఘ hmask;
		  ఒకవేళ (hshift >= బిట్లు_చొప్పున_మాట)
			hmask = -1UL;
		  లేకపోతే
			hmask = (1UL << (doff+remain)) - 1;

		  గుర్తులేని దీర్ఘ ముసుగు = hmask & ~lmask;
		  గుర్తులేని దీర్ఘ tmp;

		  tmp = (that.abits_ptr_[sptr] << doff) & ముసుగు;
		  ఒకవేళ ((abits_ptr_[dptr] & ముసుగు) != tmp) {
			diff_flag = నిజం;
			abits_ptr_[dptr] = (abits_ptr_[dptr] & ~ముసుగు) | tmp;
		  }
		  tmp = (that.bbits_ptr_[sptr] << doff) & ముసుగు;
		  ఒకవేళ ((bbits_ptr_[dptr] & ముసుగు) != tmp) {
			diff_flag = నిజం;
			bbits_ptr_[dptr] = (bbits_ptr_[dptr] & ~ముసుగు) | tmp;
		  }

		  ఒకవేళ ((doff + remain) > బిట్లు_చొప్పున_మాట) {
			గుర్తులేని తోక = doff + remain - బిట్లు_చొప్పున_మాట;
			ఒకవేళ (తోక >= బిట్లు_చొప్పున_మాట)
			      ముసుగు = -1UL;
			లేకపోతే
			      ముసుగు = (1UL << తోక) - 1;

			dptr += 1;

			tmp = (that.abits_ptr_[sptr] >> (remain-తోక))&ముసుగు;
			ఒకవేళ ((abits_ptr_[dptr] & ముసుగు) != tmp) {
			      diff_flag = నిజం;
			      abits_ptr_[dptr] = (abits_ptr_[dptr] & ~ముసుగు) | tmp;
			}
			tmp = (that.bbits_ptr_[sptr] >> (remain-తోక))&ముసుగు;
			ఒకవేళ ((bbits_ptr_[dptr] & ముసుగు) != tmp) {
			      diff_flag = నిజం;
			      bbits_ptr_[dptr] = (bbits_ptr_[dptr] & ~ముసుగు) | tmp;
			}
		  }
	    }
      }

      తిరిగి diff_flag;
}

/*
 * Add that vector to this vector. Do it in the Verilog way, which
 * means if we detect any X or Z bits, change the entire results to
 * all X.
 *
 * Assume both vectors are the same size.
 */
శూన్య vvp_vector4_t::కూడు(స్థిరరాశి vvp_vector4_t&that)
{
      ప్రతిజ్ఞా(కొలత_2 == that.కొలత_2);

      ఒకవేళ (కొలత_2 < బిట్లు_చొప్పున_మాట) {
	    గుర్తులేని దీర్ఘ ముసుగు = ~(-1UL << కొలత_2);
	    ఒకవేళ ((bbits_val_|that.bbits_val_) & ముసుగు) {
		  abits_val_ |= ముసుగు;
		  bbits_val_ |= ముసుగు;
		  తిరిగి;
	    }

	    abits_val_ += that.abits_val_;
	    abits_val_ &= ముసుగు;
	    తిరిగి;
      }

      ఒకవేళ (కొలత_2 == బిట్లు_చొప్పున_మాట) {
	    ఒకవేళ (bbits_val_ | that.bbits_val_) {
		  abits_val_ = మాట_X_ABITS;
		  bbits_val_ = మాట_X_BBITS;
	    } లేకపోతే {
		  abits_val_ += that.abits_val_;
	    }
	    తిరిగి;
      }

      పూర్ణాంకం cnt = కొలత_2 / బిట్లు_చొప్పున_మాట;
      గుర్తులేని దీర్ఘ వాహకం = 0;
      కొరకు (పూర్ణాంకం idx = 0 ; idx < cnt ; idx += 1) {
	    ఒకవేళ (bbits_ptr_[idx] | that.bbits_ptr_[idx])
		  వెళ్ళు x_బహిర్గతం;

	    abits_ptr_[idx] = కూడు_with_వాహకం(abits_ptr_[idx], that.abits_ptr_[idx], వాహకం);
      }

      ఒకవేళ (గుర్తులేని తోక = కొలత_2 % బిట్లు_చొప్పున_మాట) {
	    గుర్తులేని దీర్ఘ ముసుగు = ~( -1UL << తోక );
	    ఒకవేళ ((bbits_ptr_[cnt] | that.bbits_ptr_[cnt])&ముసుగు)
		  వెళ్ళు x_బహిర్గతం;

	    abits_ptr_[cnt] = కూడు_with_వాహకం(abits_ptr_[cnt], that.abits_ptr_[cnt], వాహకం);
	    abits_ptr_[cnt] &= ముసుగు;
      }

      తిరిగి;

 x_బహిర్గతం:
      కొరకు (పూర్ణాంకం idx = 0 ; idx < cnt ; idx += 1) {
	    abits_ptr_[idx] = మాట_X_ABITS;
	    bbits_ptr_[idx] = మాట_X_BBITS;
      }
      ఒకవేళ (గుర్తులేని తోక = కొలత_2%బిట్లు_చొప్పున_మాట) {
	    గుర్తులేని దీర్ఘ ముసుగు = ~( -1UL << తోక );
	    abits_ptr_[cnt] = మాట_X_ABITS&ముసుగు;
	    bbits_ptr_[cnt] = మాట_X_BBITS&ముసుగు;
      }
}

శూన్య vvp_vector4_t::sub(స్థిరరాశి vvp_vector4_t&that)
{
      ప్రతిజ్ఞా(కొలత_2 == that.కొలత_2);

      ఒకవేళ (కొలత_2 < బిట్లు_చొప్పున_మాట) {
	    గుర్తులేని దీర్ఘ ముసుగు = ~(-1UL << కొలత_2);
	    ఒకవేళ ((bbits_val_|that.bbits_val_) & ముసుగు) {
		  abits_val_ |= ముసుగు;
		  bbits_val_ |= ముసుగు;
		  తిరిగి;
	    }

	    abits_val_ -= that.abits_val_;
	    abits_val_ &= ముసుగు;
	    తిరిగి;
      }

      ఒకవేళ (కొలత_2 == బిట్లు_చొప్పున_మాట) {
	    ఒకవేళ (bbits_val_ | that.bbits_val_) {
		  abits_val_ = మాట_X_ABITS;
		  bbits_val_ = మాట_X_BBITS;
	    } లేకపోతే {
		  abits_val_ -= that.abits_val_;
	    }
	    తిరిగి;
      }

      పూర్ణాంకం cnt = కొలత_2 / బిట్లు_చొప్పున_మాట;
      గుర్తులేని దీర్ఘ వాహకం = 1;
      కొరకు (పూర్ణాంకం idx = 0 ; idx < cnt ; idx += 1) {
	    ఒకవేళ (bbits_ptr_[idx] | that.bbits_ptr_[idx])
		  వెళ్ళు x_బహిర్గతం;

	    abits_ptr_[idx] = కూడు_with_వాహకం(abits_ptr_[idx], ~that.abits_ptr_[idx], వాహకం);
      }

      ఒకవేళ (గుర్తులేని తోక = కొలత_2 % బిట్లు_చొప్పున_మాట) {
	    గుర్తులేని దీర్ఘ ముసుగు = ~( -1UL << తోక );
	    ఒకవేళ ((bbits_ptr_[cnt] | that.bbits_ptr_[cnt])&ముసుగు)
		  వెళ్ళు x_బహిర్గతం;

	    abits_ptr_[cnt] = కూడు_with_వాహకం(abits_ptr_[cnt], ~that.abits_ptr_[cnt], వాహకం);
	    abits_ptr_[cnt] &= ముసుగు;
      }

      తిరిగి;

 x_బహిర్గతం:
      కొరకు (పూర్ణాంకం idx = 0 ; idx < cnt ; idx += 1) {
	    abits_ptr_[idx] = మాట_X_ABITS;
	    bbits_ptr_[idx] = మాట_X_BBITS;
      }
      ఒకవేళ (గుర్తులేని తోక = కొలత_2%బిట్లు_చొప్పున_మాట) {
	    గుర్తులేని దీర్ఘ ముసుగు = ~( -1UL << తోక );
	    abits_ptr_[cnt] = మాట_X_ABITS&ముసుగు;
	    bbits_ptr_[cnt] = మాట_X_BBITS&ముసుగు;
      }

}

శూన్య vvp_vector4_t::mov(గుర్తులేని dst, గుర్తులేని src, గుర్తులేని cnt)
{
      ప్రతిజ్ఞా(dst+cnt <= కొలత_2);
      ప్రతిజ్ఞా(src+cnt <= కొలత_2);

      ఒకవేళ (కొలత_2 <= బిట్లు_చొప్పున_మాట) {
	    గుర్తులేని దీర్ఘ vmask = (1UL << cnt) - 1;
	    గుర్తులేని దీర్ఘ tmp;

	    tmp = (abits_val_ >> src) & vmask;
	    abits_val_ &= ~ (vmask << dst);
	    abits_val_ |= tmp << dst;

	    tmp = (bbits_val_ >> src) & vmask;
	    bbits_val_ &= ~ (vmask << dst);
	    bbits_val_ |= tmp << dst;

      } లేకపోతే {
	    గుర్తులేని sptr = src / బిట్లు_చొప్పున_మాట;
	    గుర్తులేని dptr = dst / బిట్లు_చొప్పున_మాట;
	    గుర్తులేని soff = src % బిట్లు_చొప్పున_మాట;
	    గుర్తులేని doff = dst % బిట్లు_చొప్పున_మాట;

	    అయితే (cnt > 0) {
		  గుర్తులేని trans = cnt;
		  ఒకవేళ ((soff+trans) > బిట్లు_చొప్పున_మాట)
			trans = బిట్లు_చొప్పున_మాట - soff;

		  ఒకవేళ ((doff+trans) > బిట్లు_చొప్పున_మాట)
			trans = బిట్లు_చొప్పున_మాట - doff;

		  ఒకవేళ (trans == బిట్లు_చొప్పున_మాట) {
			  // Special case: the transfer count is
			  // exactly an entire word. For this to be
			  // true, it must also be true that the
			  // pointers are aligned. The work is easy,
			abits_ptr_[dptr] = abits_ptr_[sptr];
			bbits_ptr_[dptr] = bbits_ptr_[sptr];
			dptr += 1;
			sptr += 1;
			cnt -= బిట్లు_చొప్పున_మాట;
			కొనసాగు;
		  }

		    // Here we know that either the source or
		    // destination is unaligned, and also we know that
		    // the count is less than a full word.
		  గుర్తులేని దీర్ఘ vmask = (1UL << trans) - 1;
		  గుర్తులేని దీర్ఘ tmp;

		  tmp = (abits_ptr_[sptr] >> soff) & vmask;
		  abits_ptr_[dptr] &= ~ (vmask << doff);
		  abits_ptr_[dptr] |= tmp << doff;

		  tmp = (bbits_ptr_[sptr] >> soff) & vmask;
		  bbits_ptr_[dptr] &= ~ (vmask << doff);
		  bbits_ptr_[dptr] |= tmp << doff;

		  cnt -= trans;
		  soff += trans;
		  ఒకవేళ (soff >= బిట్లు_చొప్పున_మాట) {
			soff = 0;
			sptr += 1;
		  }
		  doff += trans;
		  ఒకవేళ (doff >= బిట్లు_చొప్పున_మాట) {
			doff = 0;
			dptr += 1;
		  }
	    }
      }
}

శూన్య vvp_vector4_t::mul(స్థిరరాశి vvp_vector4_t&that)
{
      ప్రతిజ్ఞా(కొలత_2 == that.కొలత_2);

      ఒకవేళ (కొలత_2 < బిట్లు_చొప్పున_మాట) {
	    గుర్తులేని దీర్ఘ ముసుగు = ~(-1UL << కొలత_2);
	    ఒకవేళ ((bbits_val_|that.bbits_val_) & ముసుగు) {
		  abits_val_ |= ముసుగు;
		  bbits_val_ |= ముసుగు;
		  తిరిగి;
	    }

	    abits_val_ *= that.abits_val_;
	    abits_val_ &= ముసుగు;
	    తిరిగి;
      }

      ఒకవేళ (కొలత_2 == బిట్లు_చొప్పున_మాట) {
	    ఒకవేళ (bbits_val_ || that.bbits_val_) {
		  abits_val_ = మాట_X_ABITS;
		  bbits_val_ = మాట_X_BBITS;
	    } లేకపోతే {
		  abits_val_ *= that.abits_val_;
	    }
	    తిరిగి;
      }

      స్థిరరాశి పూర్ణాంకం cnt = (కొలత_2+బిట్లు_చొప్పున_మాట-1) / బిట్లు_చొప్పున_మాట;

      గుర్తులేని దీర్ఘ ముసుగు;
      ఒకవేళ (గుర్తులేని తోక = కొలత_2%బిట్లు_చొప్పున_మాట) {
	    ముసుగు = ~( -1UL << తోక );
      } లేకపోతే {
	    ముసుగు = ~0UL;
      }

	// Check for any XZ values ahead of time in a first pass. If
	// we find any, then force the entire result to be X and be
	// done.
      కొరకు (పూర్ణాంకం idx = 0 ; idx < cnt ; idx += 1) {
	    గుర్తులేని దీర్ఘ lval = bbits_ptr_[idx];
	    గుర్తులేని దీర్ఘ rval = that.bbits_ptr_[idx];
	    ఒకవేళ (idx == (cnt-1)) {
		  lval &= ముసుగు;
		  rval &= ముసుగు;
	    }
	    ఒకవేళ (lval || rval) {
		  కొరకు (పూర్ణాంకం xdx = 0 ; xdx < cnt-1 ; xdx += 1) {
			abits_ptr_[xdx] = మాట_X_ABITS;
			bbits_ptr_[xdx] = మాట_X_BBITS;
		  }
		  abits_ptr_[cnt-1] = మాట_X_ABITS & ముసుగు;
		  bbits_ptr_[cnt-1] = మాట_X_BBITS & ముసుగు;
		  తిరిగి;
	    }
      }

	// Calculate the result into a res array. We need to keep is
	// separate from the "this" array because we are making
	// multiple passes.
      గుర్తులేని దీర్ఘ*res = కొత్త గుర్తులేని దీర్ఘ[cnt];
      కొరకు (పూర్ణాంకం idx = 0 ; idx < cnt ; idx += 1)
	    res[idx] = 0;

      కొరకు (పూర్ణాంకం mul_a = 0 ; mul_a < cnt ; mul_a += 1) {
	    గుర్తులేని దీర్ఘ lval = abits_ptr_[mul_a];
	    ఒకవేళ (mul_a == (cnt-1))
		  lval &= ముసుగు;

	    కొరకు (పూర్ణాంకం mul_b = 0 ; mul_b < (cnt-mul_a) ; mul_b += 1) {
		  గుర్తులేని దీర్ఘ rval = that.abits_ptr_[mul_b];
		  ఒకవేళ (mul_b == (cnt-1))
			rval &= ముసుగు;

		  గుర్తులేని దీర్ఘ కూడిక;
		  గుర్తులేని దీర్ఘ tmp = multiply_with_వాహకం(lval, rval, కూడిక);
		  పూర్ణాంకం పునాది = mul_a + mul_b;
		  గుర్తులేని దీర్ఘ వాహకం = 0;
		  res[పునాది] = కూడు_with_వాహకం(res[పునాది], tmp, వాహకం);
		  కొరకు (పూర్ణాంకం కూడు_idx = పునాది+1 ; కూడు_idx < cnt ; కూడు_idx += 1) {
			res[కూడు_idx] = కూడు_with_వాహకం(res[కూడు_idx], కూడిక, వాహకం);
			కూడిక = 0;
		  }
	    }
      }

	// Replace the "this" value with the calculated result. We
	// know a-priori that the bbits are zero and unchanged.
      res[cnt-1] &= ముసుగు;
      కొరకు (పూర్ణాంకం idx = 0 ; idx < cnt ; idx += 1)
	    abits_ptr_[idx] = res[idx];

      తొలగించు[]res;
      తిరిగి;


}

తార్కిక vvp_vector4_t::eeq(స్థిరరాశి vvp_vector4_t&that) స్థిరరాశి
{
      ఒకవేళ (కొలత_2 != that.కొలత_2)
	    తిరిగి అబద్ధం;

      ఒకవేళ (కొలత_2 < బిట్లు_చొప్పున_మాట) {
	    గుర్తులేని దీర్ఘ ముసుగు = (1UL << కొలత_2) - 1;
	    తిరిగి (abits_val_&ముసుగు) == (that.abits_val_&ముసుగు)
		  && (bbits_val_&ముసుగు) == (that.bbits_val_&ముసుగు);
      }

      ఒకవేళ (కొలత_2 == బిట్లు_చొప్పున_మాట) {
	    తిరిగి (abits_val_ == that.abits_val_)
		  && (bbits_val_ == that.bbits_val_);
      }

      గుర్తులేని పదాలు = కొలత_2 / బిట్లు_చొప్పున_మాట;
      కొరకు (గుర్తులేని idx = 0 ;  idx < పదాలు ;  idx += 1) {
	    ఒకవేళ (abits_ptr_[idx] != that.abits_ptr_[idx])
		  తిరిగి అబద్ధం;
	    ఒకవేళ (bbits_ptr_[idx] != that.bbits_ptr_[idx])
		  తిరిగి అబద్ధం;
      }

      గుర్తులేని దీర్ఘ ముసుగు = కొలత_2%బిట్లు_చొప్పున_మాట;
      ఒకవేళ (ముసుగు > 0) {
	    ముసుగు = (1UL << ముసుగు) - 1;
	    తిరిగి (abits_ptr_[పదాలు]&ముసుగు) == (that.abits_ptr_[పదాలు]&ముసుగు)
		  && (bbits_ptr_[పదాలు]&ముసుగు) == (that.bbits_ptr_[పదాలు]&ముసుగు);
      }

      తిరిగి నిజం;
}

తార్కిక vvp_vector4_t::eq_xz(స్థిరరాశి vvp_vector4_t&that) స్థిరరాశి
{
      ఒకవేళ (కొలత_2 != that.కొలత_2)
	    తిరిగి అబద్ధం;

      ఒకవేళ (కొలత_2 < బిట్లు_చొప్పున_మాట) {
	    గుర్తులేని దీర్ఘ ముసుగు = (1UL << కొలత_2) - 1;
	    తిరిగి ((abits_val_|bbits_val_)&ముసుగు) == ((that.abits_val_|that.bbits_val_)&ముసుగు)
		  && (bbits_val_&ముసుగు) == (that.bbits_val_&ముసుగు);
      }

      ఒకవేళ (కొలత_2 == బిట్లు_చొప్పున_మాట) {
	    తిరిగి ((abits_val_|bbits_val_) == (that.abits_val_|that.bbits_val_))
		  && (bbits_val_ == that.bbits_val_);
      }

      గుర్తులేని పదాలు = కొలత_2 / బిట్లు_చొప్పున_మాట;
      కొరకు (గుర్తులేని idx = 0 ;  idx < పదాలు ;  idx += 1) {
	    ఒకవేళ ((abits_ptr_[idx]|bbits_ptr_[idx]) != (that.abits_ptr_[idx]|that.bbits_ptr_[idx]))
		  తిరిగి అబద్ధం;
	    ఒకవేళ (bbits_ptr_[idx] != that.bbits_ptr_[idx])
		  తిరిగి అబద్ధం;
      }

      గుర్తులేని దీర్ఘ ముసుగు = కొలత_2%బిట్లు_చొప్పున_మాట;
      ఒకవేళ (ముసుగు > 0) {
	    ముసుగు = (1UL << ముసుగు) - 1;
	    తిరిగి ((abits_ptr_[పదాలు]|bbits_ptr_[పదాలు])&ముసుగు) == ((that.abits_ptr_[పదాలు]|that.bbits_ptr_[పదాలు])&ముసుగు)
		  && (bbits_ptr_[పదాలు]&ముసుగు) == (that.bbits_ptr_[పదాలు]&ముసుగు);
      }

      తిరిగి నిజం;
}

తార్కిక vvp_vector4_t::has_xz() స్థిరరాశి
{
      ఒకవేళ (కొలత_2 < బిట్లు_చొప్పున_మాట) {
	    గుర్తులేని దీర్ఘ ముసుగు = -1UL >> (బిట్లు_చొప్పున_మాట - కొలత_2);
	    తిరిగి bbits_val_&ముసుగు;
      }

      ఒకవేళ (కొలత_2 == బిట్లు_చొప్పున_మాట) {
	    తిరిగి bbits_val_;
      }

      గుర్తులేని పదాలు = కొలత_2 / బిట్లు_చొప్పున_మాట;
      కొరకు (గుర్తులేని idx = 0 ; idx < పదాలు ; idx += 1) {
	    ఒకవేళ (bbits_ptr_[idx])
		  తిరిగి నిజం;
      }

      గుర్తులేని దీర్ఘ ముసుగు = కొలత_2%బిట్లు_చొప్పున_మాట;
      ఒకవేళ (ముసుగు > 0) {
	    ముసుగు = -1UL >> (బిట్లు_చొప్పున_మాట - ముసుగు);
	    తిరిగి bbits_ptr_[పదాలు]&ముసుగు;
      }

      తిరిగి అబద్ధం;
}

శూన్య vvp_vector4_t::change_z2x()
{
	// This method relies on the fact that both BIT4_X and BIT4_Z
	// have the bbit set in the vector4 encoding, and also that
	// the BIT4_X has abit set in the vector4 encoding. By simply
	// or-ing the bbit into the abit, BIT4_X and BIT4_Z both
	// become BIT4_X.

      ఒకవేళ (కొలత_2 <= బిట్లు_చొప్పున_మాట) {
	    abits_val_ |= bbits_val_;
      } లేకపోతే {
	    గుర్తులేని పదాలు = (కొలత_2+బిట్లు_చొప్పున_మాట-1) / బిట్లు_చొప్పున_మాట;
	    కొరకు (గుర్తులేని idx = 0 ;  idx < పదాలు ;  idx += 1)
		  abits_ptr_[idx] |= bbits_ptr_[idx];
      }
}

శూన్య vvp_vector4_t::fill_బిట్లు(vvp_bit4_t బిట్టు)
{
	/* note: this relies on the bit encoding for the vvp_bit4_t. */
      స్థిర స్థిరరాశి గుర్తులేని దీర్ఘ init_atable[4] = {
	    మాట_0_ABITS,
	    మాట_1_ABITS,
	    మాట_Z_ABITS,
	    మాట_X_ABITS };
      స్థిర స్థిరరాశి గుర్తులేని దీర్ఘ init_btable[4] = {
	    మాట_0_BBITS,
	    మాట_1_BBITS,
	    మాట_Z_BBITS,
	    మాట_X_BBITS };

      ఒకవేళ (కొలత_2 <= బిట్లు_చొప్పున_మాట) {
	    abits_val_ = init_atable[బిట్టు];
	    bbits_val_ = init_btable[బిట్టు];
      } లేకపోతే {
	    గుర్తులేని పదాలు = (కొలత_2 + బిట్లు_చొప్పున_మాట - 1) / బిట్లు_చొప్పున_మాట;
	    కొరకు (గుర్తులేని idx = 0; idx < పదాలు; idx += 1) {
		  abits_ptr_[idx] = init_atable[బిట్టు];
		  bbits_ptr_[idx] = init_btable[బిట్టు];
	    }
      }
}

అక్షరం* vvp_vector4_t::as_string(అక్షరం*buf, size_t buf_len) స్థిరరాశి
{
      అక్షరం*res = buf;
      *buf++ = 'C';
      *buf++ = '4';
      *buf++ = '<';
      buf_len -= 3;

      కొరకు (గుర్తులేని idx = 0 ;  idx < size() && buf_len >= 2 ;  idx += 1) {
	    ఎంపిక (value(size()-idx-1)) {
		స్థితి BIT4_0:
		  *buf++ = '0';
		  ఆపు;
		స్థితి BIT4_1:
		  *buf++ = '1';
		  ఆపు;
		స్థితి BIT4_X:
		  *buf++ = 'x';
		  ఆపు;
		స్థితి BIT4_Z:
		  *buf++ = 'z';
	    }
	    buf_len -= 1;
      }

      *buf++ = '>';
      *buf++ = 0;
      తిరిగి res;
}

శూన్య vvp_vector4_t::invert()
{
      ఒకవేళ (కొలత_2 <= బిట్లు_చొప్పున_మాట) {
	    గుర్తులేని దీర్ఘ ముసుగు = (కొలత_2<బిట్లు_చొప్పున_మాట)? (1UL<<కొలత_2)-1UL : -1UL;
	    abits_val_ = ముసుగు & ~abits_val_;
	    abits_val_ |= bbits_val_;
      } లేకపోతే {
	    గుర్తులేని మిగిలిన = కొలత_2;
	    గుర్తులేని idx = 0;
	    అయితే (మిగిలిన >= బిట్లు_చొప్పున_మాట) {
		  abits_ptr_[idx] = ~abits_ptr_[idx];
		  abits_ptr_[idx] |= bbits_ptr_[idx];
		  idx += 1;
		  మిగిలిన -= బిట్లు_చొప్పున_మాట;
	    }
	    ఒకవేళ (మిగిలిన > 0) {
		  గుర్తులేని దీర్ఘ ముసుగు = (1UL<<మిగిలిన) - 1UL;
		  abits_ptr_[idx] = ముసుగు & ~abits_ptr_[idx];
		  abits_ptr_[idx] |= bbits_ptr_[idx];
	    }
      }
}

#నిర్వచించు బిట్టు_ముసుగు(n) ((n) ? ((~0UL) >> (బిట్లు_చొప్పున_మాట - (n))) : ~0UL)

vvp_bit4_t vvp_vector4_t::reduce_or() స్థిరరాశి
{
      గుర్తులేని దీర్ఘ ముసుగు = బిట్టు_ముసుగు(కొలత_2 % బిట్లు_చొప్పున_మాట);
      vvp_bit4_t res = BIT4_0;

      ఒకవేళ (కొలత_2 <= బిట్లు_చొప్పున_మాట) {
	    ఒకవేళ ((abits_val_ & ~bbits_val_ & ముసుగు) != 0UL)
		  తిరిగి BIT4_1;
	    ఒకవేళ ((bbits_val_ & ముసుగు) != 0UL)
		  తిరిగి BIT4_X;
      } లేకపోతే {
	    గుర్తులేని పదాలు = (కొలత_2 + బిట్లు_చొప్పున_మాట - 1) / బిట్లు_చొప్పున_మాట;
	    గుర్తులేని idx;
	    కొరకు (idx = 0; idx < పదాలు - 1; idx += 1) {
		  ఒకవేళ ((abits_ptr_[idx] & ~bbits_ptr_[idx]) != 0UL)
			తిరిగి BIT4_1;
		  ఒకవేళ (bbits_ptr_[idx] != 0UL)
			res = BIT4_X;
	    }
	    ఒకవేళ ((abits_ptr_[idx] & ~bbits_ptr_[idx] & ముసుగు) != 0UL)
		  తిరిగి BIT4_1;
	    ఒకవేళ ((bbits_ptr_[idx] & ముసుగు) != 0UL)
		  res = BIT4_X;
      }

      తిరిగి res;
}

vvp_bit4_t vvp_vector4_t::reduce_and() స్థిరరాశి
{
      గుర్తులేని దీర్ఘ ముసుగు = బిట్టు_ముసుగు(కొలత_2 % బిట్లు_చొప్పున_మాట);
      vvp_bit4_t res = BIT4_1;

      ఒకవేళ (కొలత_2 <= బిట్లు_చొప్పున_మాట) {
	    ఒకవేళ ((abits_val_ | bbits_val_ | ~ముసుగు) != ~0UL)
		  తిరిగి BIT4_0;
	    ఒకవేళ ((bbits_val_ & ముసుగు) != 0UL)
		  తిరిగి BIT4_X;
	} లేకపోతే {
	    గుర్తులేని పదాలు = (కొలత_2 + బిట్లు_చొప్పున_మాట - 1) / బిట్లు_చొప్పున_మాట;
	    గుర్తులేని idx;
	    కొరకు (idx = 0; idx < పదాలు - 1; idx += 1) {
		  ఒకవేళ ((abits_ptr_[idx] | bbits_ptr_[idx]) != ~0UL)
			తిరిగి BIT4_0;
		  ఒకవేళ (bbits_ptr_[idx] != 0UL)
			res = BIT4_X;
	    }
	    ఒకవేళ ((abits_ptr_[idx] | bbits_ptr_[idx] | ~ముసుగు) != ~0UL)
		  తిరిగి BIT4_0;
	    ఒకవేళ ((bbits_ptr_[idx] & ముసుగు) != 0UL)
		  res = BIT4_X;
      }

      తిరిగి res;
}

స్థిర గుర్తులేని దీర్ఘ parity(గుర్తులేని దీర్ఘ val)
{
#ఒకవేళ defined(__GNUC__)
      // The compiler builtin can use target-specific CPU instructions.
      తిరిగి __builtin_parityl(val);
#లేకపోతే
#ఒకవేళ ULONG_MAX > 0xffffffffUL
	val ^= val >> 32;
#అయితే_ముగింపు
	val ^= val >> 16;
	val ^= val >> 8;
	val ^= val >> 4;

      తిరిగి (0x6996 >> (val & 0xf)) & 1;
#అయితే_ముగింపు
}

vvp_bit4_t vvp_vector4_t::reduce_xor() స్థిరరాశి
{
      గుర్తులేని దీర్ఘ ముసుగు = బిట్టు_ముసుగు(కొలత_2 % బిట్లు_చొప్పున_మాట);

      ఒకవేళ (కొలత_2 <= బిట్లు_చొప్పున_మాట) {
	    ఒకవేళ ((bbits_val_ & ముసుగు) != 0UL)
		  తిరిగి BIT4_X;
	    తిరిగి parity(abits_val_ & ముసుగు) ? BIT4_1 : BIT4_0;
      } లేకపోతే {
	    గుర్తులేని పదాలు = (కొలత_2 + బిట్లు_చొప్పున_మాట - 1) / బిట్లు_చొప్పున_మాట;
	    గుర్తులేని దీర్ఘ val_a = 0UL;
	    గుర్తులేని idx;
	    కొరకు (idx = 0; idx < పదాలు - 1; idx += 1) {
		  ఒకవేళ (bbits_ptr_[idx] != 0UL)
			తిరిగి BIT4_X;
		  val_a ^= abits_ptr_[idx];
	    }
	    ఒకవేళ ((bbits_ptr_[idx] & ముసుగు) != 0UL)
		  తిరిగి BIT4_X;
	    val_a ^= abits_ptr_[idx] & ముసుగు;
	    తిరిగి parity(val_a) ? BIT4_1 : BIT4_0;
      }
}

vvp_vector4_t& vvp_vector4_t::క్రియాకారకం &= (స్థిరరాశి vvp_vector4_t&that)
{
	// The truth table is:
	//     00 01 11 10
	//  00 00 00 00 00
	//  01 00 01 11 11
	//  11 00 11 11 11
	//  10 00 11 11 11
      ఒకవేళ (కొలత_2 <= బిట్లు_చొప్పున_మాట) {
	    గుర్తులేని దీర్ఘ tmp1 = abits_val_ | bbits_val_;
	    గుర్తులేని దీర్ఘ tmp2 = that.abits_val_ | that.bbits_val_;
	    abits_val_ = tmp1 & tmp2;
	    bbits_val_ = (tmp1 & that.bbits_val_) | (tmp2 & bbits_val_);
      } లేకపోతే {
	    గుర్తులేని పదాలు = (కొలత_2 + బిట్లు_చొప్పున_మాట - 1) / బిట్లు_చొప్పున_మాట;
	    కొరకు (గుర్తులేని idx = 0; idx < పదాలు ; idx += 1) {
		  గుర్తులేని దీర్ఘ tmp1 = abits_ptr_[idx] | bbits_ptr_[idx];
		  గుర్తులేని దీర్ఘ tmp2 = that.abits_ptr_[idx] |
		                       that.bbits_ptr_[idx];
		  abits_ptr_[idx] = tmp1 & tmp2;
		  bbits_ptr_[idx] = (tmp1 & that.bbits_ptr_[idx]) |
		                    (tmp2 & bbits_ptr_[idx]);
	    }
      }

      తిరిగి *ఇది;
}

vvp_vector4_t& vvp_vector4_t::క్రియాకారకం |= (స్థిరరాశి vvp_vector4_t&that)
{
	// The truth table is:
	//     00 01 11 10
	//  00 00 01 11 11
	//  01 01 01 01 01
	//  11 11 01 11 11
	//  10 11 01 11 11
      ఒకవేళ (కొలత_2 <= బిట్లు_చొప్పున_మాట) {
	    గుర్తులేని దీర్ఘ tmp = abits_val_ | bbits_val_ |
	                        that.abits_val_ | that.bbits_val_;
	    bbits_val_ = ((~abits_val_ | bbits_val_) & that.bbits_val_) |
	                 ((~that.abits_val_ | that.bbits_val_) & bbits_val_);
	    abits_val_ = tmp;

      } లేకపోతే {
	    గుర్తులేని పదాలు = (కొలత_2 + బిట్లు_చొప్పున_మాట - 1) / బిట్లు_చొప్పున_మాట;
	    కొరకు (గుర్తులేని idx = 0; idx < పదాలు ; idx += 1) {
		  గుర్తులేని దీర్ఘ tmp = abits_ptr_[idx] | bbits_ptr_[idx] |
	                        that.abits_ptr_[idx] | that.bbits_ptr_[idx];
		  bbits_ptr_[idx] = ((~abits_ptr_[idx] | bbits_ptr_[idx]) &
		                     that.bbits_ptr_[idx]) |
		                    ((~that.abits_ptr_[idx] |
		                      that.bbits_ptr_[idx]) & bbits_ptr_[idx]);
		  abits_ptr_[idx] = tmp;
	    }
      }

      తిరిగి *ఇది;
}

vvp_vector4_t& vvp_vector4_t::క్రియాకారకం ^= (స్థిరరాశి vvp_vector4_t&that)
{
	// The truth table is:
	//     00 01 11 10
	//  00 00 01 11 11
	//  01 01 00 11 11
	//  11 11 11 11 11
	//  10 11 11 11 11
      ఒకవేళ (కొలత_2 <= బిట్లు_చొప్పున_మాట) {
	    గుర్తులేని దీర్ఘ bval = bbits_val_ | that.bbits_val_;
	    bbits_val_ = bval;
	    abits_val_ = (abits_val_ ^ that.abits_val_) | bval;
      } లేకపోతే {
	    గుర్తులేని పదాలు = (కొలత_2 + బిట్లు_చొప్పున_మాట - 1) / బిట్లు_చొప్పున_మాట;
	    కొరకు (గుర్తులేని idx = 0; idx < పదాలు ; idx += 1) {
		  గుర్తులేని దీర్ఘ bval = bbits_ptr_[idx] | that.bbits_ptr_[idx];
		  bbits_ptr_[idx] = bval;
		  abits_ptr_[idx] = (abits_ptr_[idx] ^ that.abits_ptr_[idx]) | bval;
	    }
      }

      తిరిగి *ఇది;
}

/*
* Add an integer to the vvp_vector4_t in place, bit by bit so that
* there is no size limitations.
*/
vvp_vector4_t& vvp_vector4_t::క్రియాకారకం += (int64_t that)
{
      vvp_bit4_t వాహకం = BIT4_0;
      గుర్తులేని idx;

      ఒకవేళ (has_xz()) {
	    vvp_vector4_t xxx (size(), BIT4_X);
	    *ఇది = xxx;
	    తిరిగి *ఇది;
      }

      కొరకు (idx = 0 ; idx < size() ; idx += 1) {
	    ఒకవేళ (that == 0 && వాహకం==BIT4_0)
		  ఆపు;

	    vvp_bit4_t that_బిట్టు = (that&1)? BIT4_1 : BIT4_0;
	    that >>= 1;

	    ఒకవేళ (that_బిట్టు==BIT4_0 && వాహకం==BIT4_0)
		  కొనసాగు;

	    vvp_bit4_t బిట్టు = value(idx);
	    బిట్టు = కూడు_with_వాహకం(బిట్టు, that_బిట్టు, వాహకం);

	    సమితి_బిట్టు(idx, బిట్టు);
      }

      తిరిగి *ఇది;
}

ostream& క్రియాకారకం<< (ostream&out, స్థిరరాశి vvp_vector4_t&that)
{
      out << that.size() << "'b";
      కొరకు (గుర్తులేని idx = 0 ;  idx < that.size() ;  idx += 1)
	    out << that.value(that.size()-idx-1);
      తిరిగి out;
}

మూస <తరగతి INT>తార్కిక vector4_గమ్యం_విలువ(స్థిరరాశి vvp_vector4_t&vec, INT&val,
					  తార్కిక is_signed, తార్కిక is_arithmetic)
{
      INT res = 0;
      INT msk = 1;
      తార్కిక rc_flag = నిజం;

      గుర్తులేని size = vec.size();
      ఒకవేళ (size > 8*పరిమాణం(val)) size = 8*పరిమాణం(val);
      కొరకు (గుర్తులేని idx = 0 ;  idx < size ;  idx += 1) {
	    ఎంపిక (vec.value(idx)) {
		స్థితి BIT4_0:
		  ఆపు;
		స్థితి BIT4_1:
		  res |= msk;
		  ఆపు;
		అప్రమేయ:
		  ఒకవేళ (is_arithmetic)
			తిరిగి అబద్ధం;
		  లేకపోతే
			rc_flag = అబద్ధం;
	    }

	    msk <<= 1;
      }

      ఒకవేళ (is_signed && vec.value(vec.size()-1) == BIT4_1) {
	    ఒకవేళ (vec.size() < 8*పరిమాణం(val))
		  res |= static_cast<INT>(-1ULL << vec.size());
      }

      val = res;
      తిరిగి rc_flag;
}

మూస తార్కిక vector4_గమ్యం_విలువ(స్థిరరాశి vvp_vector4_t&vec, int8_t&val,
			       తార్కిక is_signed, తార్కిక is_arithmetic);
మూస తార్కిక vector4_గమ్యం_విలువ(స్థిరరాశి vvp_vector4_t&vec, int16_t&val,
			       తార్కిక is_signed, తార్కిక is_arithmetic);
మూస తార్కిక vector4_గమ్యం_విలువ(స్థిరరాశి vvp_vector4_t&vec, int32_t&val,
			       తార్కిక is_signed, తార్కిక is_arithmetic);
మూస తార్కిక vector4_గమ్యం_విలువ(స్థిరరాశి vvp_vector4_t&vec, int64_t&val,
			       తార్కిక is_signed, తార్కిక is_arithmetic);
మూస తార్కిక vector4_గమ్యం_విలువ(స్థిరరాశి vvp_vector4_t&vec, uint8_t&val,
			       తార్కిక is_signed, తార్కిక is_arithmetic);
మూస తార్కిక vector4_గమ్యం_విలువ(స్థిరరాశి vvp_vector4_t&vec, uint16_t&val,
			       తార్కిక is_signed, తార్కిక is_arithmetic);
మూస తార్కిక vector4_గమ్యం_విలువ(స్థిరరాశి vvp_vector4_t&vec, uint32_t&val,
			       తార్కిక is_signed, తార్కిక is_arithmetic);
మూస తార్కిక vector4_గమ్యం_విలువ(స్థిరరాశి vvp_vector4_t&vec, uint64_t&val,
			       తార్కిక is_signed, తార్కిక is_arithmetic);

మూస <తరగతి T> తార్కిక vector4_గమ్యం_విలువ(స్థిరరాశి vvp_vector4_t&vec,
                                         తార్కిక&అధికప్రవాహం_flag, T&val)
{
      T res = 0;
      T msk = 1;

      అధికప్రవాహం_flag = అబద్ధం;
      గుర్తులేని size = vec.size();
      కొరకు (గుర్తులేని idx = 0 ;  idx < size ;  idx += 1) {
	    ఎంపిక (vec.value(idx)) {
		స్థితి BIT4_0:
		  ఆపు;
		స్థితి BIT4_1:
		  ఒకవేళ (msk == 0)
			అధికప్రవాహం_flag = నిజం;
		  లేకపోతే
			res |= msk;
		  ఆపు;
		అప్రమేయ:
		  తిరిగి అబద్ధం;
	    }

	    msk <<= static_cast<T>(1);
      }

      val = res;
      తిరిగి నిజం;
}

మూస తార్కిక vector4_గమ్యం_విలువ(స్థిరరాశి vvp_vector4_t&vec, తార్కిక&అధికప్రవాహం_flag,
                               గుర్తులేని దీర్ఘ&val);
#నిర్వచితం_కాకపోతే UL_AND_TIME64_SAME
మూస తార్కిక vector4_గమ్యం_విలువ(స్థిరరాశి vvp_vector4_t&vec, తార్కిక&అధికప్రవాహం_flag,
                               vvp_time64_t&val);
#అయితే_ముగింపు

తార్కిక vector4_గమ్యం_విలువ(స్థిరరాశి vvp_vector4_t&vec, ద్విగుణ&val, తార్కిక signed_flag)
{

      ఒకవేళ (vec.size() == 0) {
	    val = 0.0;
	    తిరిగి నిజం;
      }

      తార్కిక flag = నిజం;

      ఒకవేళ (vec.value(vec.size()-1) != BIT4_1) {
	    signed_flag = అబద్ధం;
      }

      ద్విగుణ res = 0.0;
      ఒకవేళ (signed_flag) {
	    vvp_bit4_t వాహకం = BIT4_1;
	    కొరకు (గుర్తులేని idx = 0 ;  idx < vec.size() ;  idx += 1) {
		  vvp_bit4_t a = ~vec.value(idx);
		  vvp_bit4_t x = కూడు_with_వాహకం(a, BIT4_0, వాహకం);
		  ఎంపిక (x) {
		      స్థితి BIT4_0:
			ఆపు;
		      స్థితి BIT4_1:
			res += ఘాతం(2.0, (పూర్ణాంకం)idx);
			ఆపు;
		      అప్రమేయ:
			flag = అబద్ధం;
		  }
	    }
	    res *= -1.0;
      } లేకపోతే {
	    కొరకు (గుర్తులేని idx = 0 ;  idx < vec.size() ;  idx += 1) {
		  ఎంపిక (vec.value(idx)) {
		      స్థితి BIT4_0:
			ఆపు;
		      స్థితి BIT4_1:
			res += ఘాతం(2.0, (పూర్ణాంకం)idx);
			ఆపు;
		      అప్రమేయ:
			flag = అబద్ధం;
		  }
	    }
      }
      val = res;
      తిరిగి flag;
}

తార్కిక vector2_గమ్యం_విలువ(స్థిరరాశి vvp_vector2_t&a, int32_t&val, తార్కిక is_signed)
{
      val = 0;
      గుర్తులేని idx;
      uint32_t ముసుగు;
      కొరకు (idx = 0, ముసుగు = 1 ; idx < a.size() && idx < 32 ; idx += 1, ముసుగు <<= 1) {
	    ఒకవేళ (a.value(idx)) val |= ముసుగు;
      }

      ఒకవేళ (is_signed && a.size() < 32 && a.value(a.size()-1)) {
	    ముసుగు = -1;
	    ముసుగు <<= a.size();
	    val |= ముసుగు;
      }

      తిరిగి a.size() <= 32;
}

vvp_vector4array_t::vvp_vector4array_t(గుర్తులేని వెడల్పు_3, గుర్తులేని పదాలు_2)
: వెడల్పు_2(వెడల్పు_3), పదాలు_3(పదాలు_2)
{
}

vvp_vector4array_t::~vvp_vector4array_t()
{
}

శూన్య vvp_vector4array_t::సమితి_మాట_2(v4cell*cell, స్థిరరాశి vvp_vector4_t&that)
{
      ప్రతిజ్ఞా(that.కొలత_2 == వెడల్పు_2);

      ఒకవేళ (వెడల్పు_2 <= vvp_vector4_t::బిట్లు_చొప్పున_మాట) {
	    cell->abits_val_ = that.abits_val_;
	    cell->bbits_val_ = that.bbits_val_;
	    తిరిగి;
      }

      గుర్తులేని cnt = (వెడల్పు_2 + vvp_vector4_t::బిట్లు_చొప్పున_మాట-1)/vvp_vector4_t::బిట్లు_చొప్పున_మాట;

      ఒకవేళ (cell->abits_ptr_ == 0) {
	    cell->abits_ptr_ = కొత్త గుర్తులేని దీర్ఘ[2*cnt];
	    cell->bbits_ptr_ = cell->abits_ptr_ + cnt;
      }

      కొరకు (గుర్తులేని idx = 0 ; idx < cnt ; idx += 1)
	    cell->abits_ptr_[idx] = that.abits_ptr_[idx];
      కొరకు (గుర్తులేని idx = 0 ; idx < cnt ; idx += 1)
	    cell->bbits_ptr_[idx] = that.bbits_ptr_[idx];
}

vvp_vector4_t vvp_vector4array_t::get_మాట_2(v4cell*cell) స్థిరరాశి
{
      ఒకవేళ (వెడల్పు_2 <= vvp_vector4_t::బిట్లు_చొప్పున_మాట) {
	    vvp_vector4_t res;
	    res.కొలత_2 = వెడల్పు_2;
	    res.abits_val_ = cell->abits_val_;
	    res.bbits_val_ = cell->bbits_val_;
	    తిరిగి res;
      }

      vvp_vector4_t res (వెడల్పు_2, BIT4_X);
      ఒకవేళ (cell->abits_ptr_ == 0)
	    తిరిగి res;

      గుర్తులేని cnt = (వెడల్పు_2 + vvp_vector4_t::బిట్లు_చొప్పున_మాట-1)/vvp_vector4_t::బిట్లు_చొప్పున_మాట;

      కొరకు (గుర్తులేని idx = 0 ; idx < cnt ; idx += 1)
	    res.abits_ptr_[idx] = cell->abits_ptr_[idx];
      కొరకు (గుర్తులేని idx = 0 ; idx < cnt ; idx += 1)
	    res.bbits_ptr_[idx] = cell->bbits_ptr_[idx];

      తిరిగి res;
}

vvp_vector4array_sa::vvp_vector4array_sa(గుర్తులేని వెడల్పు_3, గుర్తులేని పదాలు_2)
: vvp_vector4array_t(వెడల్పు_3, పదాలు_2)
{
      array_ = కొత్త v4cell[పదాలు_3];

      ఒకవేళ (వెడల్పు_2 <= vvp_vector4_t::బిట్లు_చొప్పున_మాట) {
	    కొరకు (గుర్తులేని idx = 0 ; idx < పదాలు_3 ; idx += 1) {
		  array_[idx].abits_val_ = vvp_vector4_t::మాట_X_ABITS;
		  array_[idx].bbits_val_ = vvp_vector4_t::మాట_X_BBITS;
	    }
      } లేకపోతే {
	    కొరకు (గుర్తులేని idx = 0 ; idx < పదాలు_3 ; idx += 1) {
		  array_[idx].abits_ptr_ = 0;
		  array_[idx].bbits_ptr_ = 0;
	    }
      }
}

vvp_vector4array_sa::~vvp_vector4array_sa()
{
      ఒకవేళ (array_) {
	    ఒకవేళ (వెడల్పు_2 > vvp_vector4_t::బిట్లు_చొప్పున_మాట) {
		  కొరకు (గుర్తులేని idx = 0 ; idx < పదాలు_3 ; idx += 1)
			ఒకవేళ (array_[idx].abits_ptr_)
			      తొలగించు[]array_[idx].abits_ptr_;
	    }
	    తొలగించు[]array_;
      }
}

శూన్య vvp_vector4array_sa::సమితి_మాట(గుర్తులేని సూచీ, స్థిరరాశి vvp_vector4_t&that)
{
      ప్రతిజ్ఞా(సూచీ < పదాలు_3);

      v4cell*cell = &array_[సూచీ];

      సమితి_మాట_2(cell, that);
}

vvp_vector4_t vvp_vector4array_sa::get_మాట(గుర్తులేని సూచీ) స్థిరరాశి
{
      ఒకవేళ (సూచీ >= పదాలు_3)
	    తిరిగి vvp_vector4_t(వెడల్పు_2, BIT4_X);

      ప్రతిజ్ఞా(సూచీ < పదాలు_3);

      v4cell*cell = &array_[సూచీ];

      తిరిగి get_మాట_2(cell);
}

vvp_vector4array_aa::vvp_vector4array_aa(గుర్తులేని వెడల్పు_3, గుర్తులేని పదాలు_2)
: vvp_vector4array_t(వెడల్పు_3, పదాలు_2)
{
      context_idx_ = vpip_కూడు_అంశం_గమ్యం_context(ఇది, vpip_peek_context_scope());
}

vvp_vector4array_aa::~vvp_vector4array_aa()
{
}

శూన్య vvp_vector4array_aa::alloc_instance(vvp_context_t context)
{
      v4cell*array = కొత్త v4cell[పదాలు_3];

      ఒకవేళ (వెడల్పు_2 <= vvp_vector4_t::బిట్లు_చొప్పున_మాట) {
	    కొరకు (గుర్తులేని idx = 0 ; idx < పదాలు_3 ; idx += 1) {
		  array[idx].abits_val_ = vvp_vector4_t::మాట_X_ABITS;
		  array[idx].bbits_val_ = vvp_vector4_t::మాట_X_BBITS;
	    }
      } లేకపోతే {
	    కొరకు (గుర్తులేని idx = 0 ; idx < పదాలు_3 ; idx += 1) {
		  array[idx].abits_ptr_ = 0;
		  array[idx].bbits_ptr_ = 0;
	    }
      }

      vvp_సమితి_context_అంశం(context, context_idx_, array);
}

శూన్య vvp_vector4array_aa::reset_instance(vvp_context_t context)
{
      v4cell*cell = static_cast<v4cell*>
            (vvp_get_context_అంశం(context, context_idx_));

      ఒకవేళ (వెడల్పు_2 <= vvp_vector4_t::బిట్లు_చొప్పున_మాట) {
	    కొరకు (గుర్తులేని idx = 0 ; idx < పదాలు_3 ; idx += 1) {
		  cell->abits_val_ = vvp_vector4_t::మాట_X_ABITS;
		  cell->bbits_val_ = vvp_vector4_t::మాట_X_BBITS;
                  cell++;
	    }
      } లేకపోతే {
            గుర్తులేని cnt = (వెడల్పు_2 + vvp_vector4_t::బిట్లు_చొప్పున_మాట-1)/vvp_vector4_t::బిట్లు_చొప్పున_మాట;
	    కొరకు (గుర్తులేని idx = 0 ; idx < పదాలు_3 ; idx += 1) {
		  ఒకవేళ (cell->abits_ptr_) {
                        కొరకు (గుర్తులేని n = 0 ; n < cnt ; n += 1) {
			      cell->abits_ptr_[n] = vvp_vector4_t::మాట_X_ABITS;
			      cell->bbits_ptr_[n] = vvp_vector4_t::మాట_X_BBITS;
                        }
                  }
                  cell++;
	    }
      }
}

#నిర్వచితమైతే CHECK_WITH_VALGRIND
శూన్య vvp_vector4array_aa::free_instance(vvp_context_t context)
{
      v4cell*cell = static_cast<v4cell*>
            (vvp_get_context_అంశం(context, context_idx_));
      తొలగించు [] cell;
}
#అయితే_ముగింపు

శూన్య vvp_vector4array_aa::సమితి_మాట(గుర్తులేని సూచీ, స్థిరరాశి vvp_vector4_t&that)
{
      ప్రతిజ్ఞా(సూచీ < పదాలు_3);

      v4cell*cell = static_cast<v4cell*>
            (vthread_get_wt_context_అంశం(context_idx_)) + సూచీ;

      సమితి_మాట_2(cell, that);
}

vvp_vector4_t vvp_vector4array_aa::get_మాట(గుర్తులేని సూచీ) స్థిరరాశి
{
      ఒకవేళ (సూచీ >= పదాలు_3)
	    తిరిగి vvp_vector4_t(వెడల్పు_2, BIT4_X);

      ప్రతిజ్ఞా(సూచీ < పదాలు_3);

      v4cell*cell = static_cast<v4cell*>
            (vthread_get_rd_context_అంశం(context_idx_)) + సూచీ;

      తిరిగి get_మాట_2(cell);
}

vvp_vector2_t::vvp_vector2_t()
{
      vec_ = 0;
      wid_ = 0;
}

vvp_vector2_t::vvp_vector2_t(uint64_t v, గుర్తులేని wid)
{
      wid_ = wid;
      స్థిరరాశి గుర్తులేని బిట్లు_చొప్పున_మాట_2 = 8 * పరిమాణం(vec_[0]);
      స్థిరరాశి గుర్తులేని పదాలు = (wid_ + బిట్లు_చొప్పున_మాట_2-1) / బిట్లు_చొప్పున_మాట_2;

      vec_ = కొత్త గుర్తులేని దీర్ఘ[పదాలు];
      కొరకు (గుర్తులేని idx = 0 ; idx < పదాలు ; idx += 1) {
	    స్థిరరాశి గుర్తులేని జరుపు = idx * బిట్లు_చొప్పున_మాట_2;
	    vec_[idx] = జరుపు < 64 ? v >> జరుపు : 0;
      }
}

vvp_vector2_t::vvp_vector2_t(vvp_vector2_t::fill_t fill, గుర్తులేని wid)
{
      wid_ = wid;
      స్థిరరాశి గుర్తులేని బిట్లు_చొప్పున_మాట_2 = 8 * పరిమాణం(vec_[0]);
      స్థిరరాశి గుర్తులేని పదాలు = (wid_ + బిట్లు_చొప్పున_మాట_2-1) / బిట్లు_చొప్పున_మాట_2;

      vec_ = కొత్త గుర్తులేని దీర్ఘ[పదాలు];
      కొరకు (గుర్తులేని idx = 0 ;  idx < పదాలు ;  idx += 1)
	    vec_[idx] = fill? -1 : 0;
}

vvp_vector2_t::vvp_vector2_t(స్థిరరాశి vvp_vector2_t&that, గుర్తులేని పునాది, గుర్తులేని wid)
{
      wid_ = wid;
      స్థిరరాశి గుర్తులేని పదాలు = (wid_ + బిట్లు_చొప్పున_మాట-1) / బిట్లు_చొప్పున_మాట;

      vec_ = కొత్త గుర్తులేని దీర్ఘ[పదాలు];

      కొరకు (గుర్తులేని idx = 0 ; idx < wid ; idx += 1) {
	    పూర్ణాంకం బిట్టు = that.value(పునాది+idx);
	    ఒకవేళ (బిట్టు == 0)
		  కొనసాగు;
	    గుర్తులేని మాట = idx / బిట్లు_చొప్పున_మాట;
	    గుర్తులేని దీర్ఘ ముసుగు = 1UL << (idx % బిట్లు_చొప్పున_మాట);
	    vec_[మాట] |= ముసుగు;
      }
}

శూన్య vvp_vector2_t::copy_from_that_(స్థిరరాశి vvp_vector4_t&that)
{
      wid_ = that.size();
      స్థిరరాశి గుర్తులేని పదాలు = (that.size() + బిట్లు_చొప్పున_మాట-1) / బిట్లు_చొప్పున_మాట;

      ఒకవేళ (పదాలు == 0) {
	    vec_ = 0;
	    wid_ = 0;
	    తిరిగి;
      }

	// Use the subarray method with the xz_to_0 flag set so that
	// we get values even when there are xz bits.
      vec_ = that.subarray(0, wid_, నిజం);
}

శూన్య vvp_vector2_t::copy_from_that_(స్థిరరాశి vvp_vector2_t&that)
{
      wid_ = that.wid_;
      స్థిరరాశి గుర్తులేని పదాలు = (wid_ + బిట్లు_చొప్పున_మాట-1) / బిట్లు_చొప్పున_మాట;

      ఒకవేళ (పదాలు == 0) {
	    vec_ = 0;
	    wid_ = 0;
	    తిరిగి;
      }

      vec_ = కొత్త గుర్తులేని దీర్ఘ[పదాలు];
      కొరకు (గుర్తులేని idx = 0 ;  idx < పదాలు ;  idx += 1)
	    vec_[idx] = that.vec_[idx];
}

vvp_vector2_t::vvp_vector2_t(స్థిరరాశి vvp_vector2_t&that, గుర్తులేని newsize)
{
      wid_ = newsize;
      ఒకవేళ (newsize == 0) {
	    vec_ = 0;
	    తిరిగి;
      }

      స్థిరరాశి గుర్తులేని పదాలు = (wid_ + బిట్లు_చొప్పున_మాట-1) / బిట్లు_చొప్పున_మాట;
      స్థిరరాశి గుర్తులేని twords = (that.wid_ + బిట్లు_చొప్పున_మాట-1) / బిట్లు_చొప్పున_మాట;

      vec_ = కొత్త గుర్తులేని దీర్ఘ[పదాలు];
      కొరకు (గుర్తులేని idx = 0 ;  idx < పదాలు ;  idx += 1) {
	    ఒకవేళ (idx < twords)
		  vec_[idx] = that.vec_[idx];
	    లేకపోతే
		  vec_[idx] = 0;
      }
}

vvp_vector2_t& vvp_vector2_t::క్రియాకారకం= (స్థిరరాశి vvp_vector2_t&that)
{
      ఒకవేళ (ఇది == &that)
	    తిరిగి *ఇది;

      తొలగించు[] vec_;
      vec_ = 0;

      copy_from_that_(that);
      తిరిగి *ఇది;
}

vvp_vector2_t& vvp_vector2_t::క్రియాకారకం= (స్థిరరాశి vvp_vector4_t&that)
{
      తొలగించు[]vec_;
      vec_ = 0;
      copy_from_that_(that);
      తిరిగి *ఇది;
}

vvp_vector2_t& vvp_vector2_t::క్రియాకారకం <<= (గుర్తులేని పూర్ణాంకం జరుపు)
{
      ఒకవేళ (wid_ == 0)
	    తిరిగి *ఇది;

      స్థిరరాశి గుర్తులేని పదాలు = (wid_ + బిట్లు_చొప్పున_మాట-1) / బిట్లు_చొప్పున_మాట;

	// Number of words to shift
      స్థిరరాశి గుర్తులేని wshift = జరుపు / బిట్లు_చొప్పున_మాట;
	// bits to shift within each word.
      స్థిరరాశి గుర్తులేని దీర్ఘ oshift = జరుపు % బిట్లు_చొప్పున_మాట;

	// If shifting the entire value away, then return zeros.
      ఒకవేళ (wshift >= పదాలు) {
	    కొరకు (గుర్తులేని idx = 0 ;  idx < పదాలు ;  idx += 1)
		  vec_[idx] = 0;

	    తిరిగి *ఇది;
      }

	// Do the word shift first.
      ఒకవేళ (wshift > 0) {
	    కొరకు (గుర్తులేని idx = 0 ;  idx < పదాలు-wshift ;  idx += 1) {
		  గుర్తులేని sel = పదాలు - idx - 1;
		  vec_[sel] = vec_[sel-wshift];
	    }

	    కొరకు (గుర్తులేని idx = 0 ;  idx < wshift ;  idx += 1)
		  vec_[idx] = 0;
      }

	// Do the fine shift.
      ఒకవేళ (oshift != 0) {
	    గుర్తులేని దీర్ఘ pad = 0;
	    కొరకు (గుర్తులేని idx = 0 ;  idx < పదాలు ;  idx += 1) {
		  గుర్తులేని దీర్ఘ తదుపరి_pad = vec_[idx] >> (బిట్లు_చొప్పున_మాట-oshift);
		  vec_[idx] = (vec_[idx] << oshift) | pad;
		  pad = తదుపరి_pad;
	    }

	      // Cleanup the tail bits.
	    గుర్తులేని దీర్ఘ ముసుగు = -1UL >> (బిట్లు_చొప్పున_మాట - wid_%బిట్లు_చొప్పున_మాట);
	    vec_[పదాలు-1] &= ముసుగు;
      }

      తిరిగి *ఇది;
}

vvp_vector2_t& vvp_vector2_t::క్రియాకారకం >>= (గుర్తులేని జరుపు)
{
      ఒకవేళ (wid_ == 0)
	    తిరిగి *ఇది;

      స్థిరరాశి గుర్తులేని పదాలు = (wid_ + బిట్లు_చొప్పున_మాట-1) / బిట్లు_చొప్పున_మాట;

	// Number of words to shift
      స్థిరరాశి గుర్తులేని wshift = జరుపు / బిట్లు_చొప్పున_మాట;
	// bits to shift within each word.
      స్థిరరాశి గుర్తులేని దీర్ఘ oshift = జరుపు % బిట్లు_చొప్పున_మాట;

	// If shifting the entire value away, then return zeros.
      ఒకవేళ (wshift >= పదాలు) {
	    కొరకు (గుర్తులేని idx = 0 ;  idx < పదాలు ;  idx += 1)
		  vec_[idx] = 0;

	    తిరిగి *ఇది;
      }

      ఒకవేళ (wshift > 0) {
	    కొరకు (గుర్తులేని idx = 0 ;  idx < పదాలు-wshift ;  idx += 1)
		  vec_[idx] = vec_[idx+wshift];

	    కొరకు (గుర్తులేని idx = పదాలు-wshift ;  idx < పదాలు ;  idx += 1)
		  vec_[idx] = 0;
      }

      ఒకవేళ (oshift > 0) {
	    గుర్తులేని దీర్ఘ pad = 0;
	    కొరకు (గుర్తులేని idx = పదాలు ;  idx > 0 ;  idx -= 1) {
		  గుర్తులేని దీర్ఘ new_pad = vec_[idx-1] <<(బిట్లు_చొప్పున_మాట-oshift);
		  vec_[idx-1] = pad | (vec_[idx-1] >> oshift);
		  pad = new_pad;
	    }

	      // Cleanup the tail bits.

	    గుర్తులేని use_పదాలు = పదాలు;
	      // Mask_shift is the number of high bits of the top word
	      // that are to be masked off. We start with the number
	      // of bits that are not included even in the original,
	      // then we include the bits of the shift, that are to be
	      // masked to zero.
	    గుర్తులేని దీర్ఘ ముసుగు_జరుపు = బిట్లు_చొప్పున_మాట - wid_%బిట్లు_చొప్పున_మాట;
	    ముసుగు_జరుపు %= బిట్లు_చొప్పున_మాట;
	    ముసుగు_జరుపు += oshift;
	    అయితే (ముసుగు_జరుపు >= బిట్లు_చొప్పున_మాట) {
		  vec_[use_పదాలు-1] = 0;
		  use_పదాలు -= 1;
		  ముసుగు_జరుపు -= బిట్లు_చొప్పున_మాట;
	    }
	    ఒకవేళ (ముసుగు_జరుపు > 0) {
		  ప్రతిజ్ఞా(use_పదాలు > 0);
		  గుర్తులేని దీర్ఘ ముసుగు = -1UL >> ముసుగు_జరుపు;
		  vec_[use_పదాలు-1] &= ముసుగు;
	    }
      }

      తిరిగి *ఇది;
}

స్థిర గుర్తులేని దీర్ఘ కూడు_వాహకం(గుర్తులేని దీర్ఘ a, గుర్తులేని దీర్ఘ b,
			       గుర్తులేని దీర్ఘ&వాహకం)
{
      గుర్తులేని దీర్ఘ out = వాహకం;
      వాహకం = 0;

      ఒకవేళ ((ULONG_MAX - out) < a)
	    వాహకం += 1;
      out += a;

      ఒకవేళ ((ULONG_MAX - out) < b)
	    వాహకం += 1;
      out += b;

      తిరిగి out;
}

vvp_vector2_t& vvp_vector2_t::క్రియాకారకం += (స్థిరరాశి vvp_vector2_t&that)
{
      ప్రతిజ్ఞా(wid_ == that.wid_);
      ఒకవేళ (wid_ == 0)
	    తిరిగి *ఇది;

      స్థిరరాశి గుర్తులేని పదాలు = (wid_ + బిట్లు_చొప్పున_మాట-1) / బిట్లు_చొప్పున_మాట;

      గుర్తులేని దీర్ఘ వాహకం = 0;
      కొరకు (గుర్తులేని idx = 0 ;  idx < పదాలు ;  idx += 1) {
	    vec_[idx] = కూడు_వాహకం(vec_[idx], that.vec_[idx], వాహకం);
      }


	// Cleanup the tail bits.
      గుర్తులేని దీర్ఘ ముసుగు = -1UL >> (బిట్లు_చొప్పున_మాట - wid_%బిట్లు_చొప్పున_మాట);
      vec_[పదాలు-1] &= ముసుగు;

      తిరిగి *ఇది;
}

vvp_vector2_t& vvp_vector2_t::క్రియాకారకం -= (స్థిరరాశి vvp_vector2_t&that)
{
      ప్రతిజ్ఞా(wid_ == that.wid_);
      ఒకవేళ (wid_ == 0)
	    తిరిగి *ఇది;

      స్థిరరాశి గుర్తులేని పదాలు = (wid_ + బిట్లు_చొప్పున_మాట-1) / బిట్లు_చొప్పున_మాట;

      గుర్తులేని దీర్ఘ వాహకం = 1;
      కొరకు (గుర్తులేని idx = 0 ;  idx < పదాలు ;  idx += 1) {
	    vec_[idx] = కూడు_వాహకం(vec_[idx], ~that.vec_[idx], వాహకం);
      }

      తిరిగి *ఇది;
}

శూన్య vvp_vector2_t::trim()
{
      అయితే (value(wid_-1) == 0 && wid_ > 1) wid_ -= 1;
}

/* This is a special trim that is used on numbers we know represent a
 * negative signed value (they came from a negative real value). */
శూన్య vvp_vector2_t::trim_neg()
{
      ఒకవేళ (value(wid_-1) == 1 && wid_ > 32) {
	    అయితే (value(wid_-2) == 1 && wid_ > 32) wid_ -= 1;
      }
}

పూర్ణాంకం vvp_vector2_t::value(గుర్తులేని idx) స్థిరరాశి
{
      ఒకవేళ (idx >= wid_)
	    తిరిగి 0;

      స్థిరరాశి గుర్తులేని బిట్లు_చొప్పున_మాట_2 = 8 * పరిమాణం(vec_[0]);
      గుర్తులేని addr = idx/బిట్లు_చొప్పున_మాట_2;
      గుర్తులేని ముసుగు = idx%బిట్లు_చొప్పున_మాట_2;

      ఒకవేళ (vec_[addr] & (1UL<<ముసుగు))
	    తిరిగి 1;
      లేకపోతే
	    తిరిగి 0;
}

శూన్య vvp_vector2_t::సమితి_బిట్టు(గుర్తులేని idx, పూర్ణాంకం బిట్టు)
{
      ప్రతిజ్ఞా(idx < wid_);

      స్థిరరాశి గుర్తులేని బిట్లు_చొప్పున_మాట_2 = 8 * పరిమాణం(vec_[0]);
      గుర్తులేని addr = idx/బిట్లు_చొప్పున_మాట_2;
      గుర్తులేని దీర్ఘ ముసుగు = idx%బిట్లు_చొప్పున_మాట_2;

      ఒకవేళ (బిట్టు)
	    vec_[addr] |= 1UL << ముసుగు;
      లేకపోతే
	    vec_[addr] &= ~(1UL << ముసుగు);
}

శూన్య vvp_vector2_t::సమితి_vec(గుర్తులేని adr, స్థిరరాశి vvp_vector2_t&that)
{
      ప్రతిజ్ఞా((adr + that.wid_) <= wid_);

      కొరకు (గుర్తులేని idx = 0 ; idx < that.wid_ ; idx += 1)
	    సమితి_బిట్టు(adr+idx, that.value(idx));
}

తార్కిక vvp_vector2_t::is_NaN() స్థిరరాశి
{
      తిరిగి wid_ == 0;
}

తార్కిక vvp_vector2_t::is_zero() స్థిరరాశి
{
      స్థిరరాశి గుర్తులేని పదాలు = (wid_ + బిట్లు_చొప్పున_మాట-1) / బిట్లు_చొప్పున_మాట;

      కొరకు (గుర్తులేని idx = 0; idx < పదాలు; idx += 1) {
	    ఒకవేళ (vec_[idx] == 0) కొనసాగు;
	    తిరిగి అబద్ధం;
      }

      తిరిగి నిజం;
}

/*
 * Basic idea from "Introduction to Programming using SML" by
 * Michael R. Hansen and Hans Rischel page 261 and "Seminumerical
 * Algorithms, Third Edition" by Donald E. Knuth section 4.6.3.
 */
vvp_vector2_t ఘాతం(స్థిరరాశి vvp_vector2_t&x, vvp_vector2_t&y)
{
        /* If we have a zero exponent just return 1. */
      ఒకవేళ (y == vvp_vector2_t(0L, 1)) {
	    తిరిగి vvp_vector2_t(1L, x.size());
      }

        /* Is the value odd? */
      ఒకవేళ (y.value(0) == 1) {
	    y.సమితి_బిట్టు(0, 0);  // A quick subtract by 1.
	    vvp_vector2_t res = x * ఘాతం(x, y);
	    తిరిగి res;
      }

      y >>= 1;  // A fast divide by two. We know the LSB is zero.
      vvp_vector2_t z = ఘాతం(x, y);
      vvp_vector2_t res = z * z;
      తిరిగి res;
}

స్థిర శూన్య multiply_long(గుర్తులేని దీర్ఘ a, గుర్తులేని దీర్ఘ b,
			  గుర్తులేని దీర్ఘ&దిగువ, గుర్తులేని దీర్ఘ&ఎగువ)
{
      ప్రతిజ్ఞా(పరిమాణం(గుర్తులేని దీర్ఘ) %2 == 0);

      స్థిరరాశి గుర్తులేని దీర్ఘ మాట_ముసుగు = (1UL << 4UL*పరిమాణం(a)) - 1UL;
      గుర్తులేని దీర్ఘ tmpa;
      గుర్తులేని దీర్ఘ tmpb;
      గుర్తులేని దీర్ఘ res[4];

      tmpa = a & మాట_ముసుగు;
      tmpb = b & మాట_ముసుగు;
      res[0] = tmpa * tmpb;
      res[1] = res[0] >> 4UL*పరిమాణం(గుర్తులేని దీర్ఘ);
      res[0] &= మాట_ముసుగు;

      tmpa = (a >> 4UL*పరిమాణం(గుర్తులేని దీర్ఘ)) & మాట_ముసుగు;
      res[1] += tmpa * tmpb;
      res[2] = res[1] >> 4UL*పరిమాణం(గుర్తులేని దీర్ఘ);
      res[1] &= మాట_ముసుగు;

      tmpa = a & మాట_ముసుగు;
      tmpb = (b >> 4UL*పరిమాణం(గుర్తులేని దీర్ఘ)) & మాట_ముసుగు;
      res[1] += tmpa * tmpb;
      res[2] += res[1] >> 4UL*పరిమాణం(గుర్తులేని దీర్ఘ);
      res[3]  = res[2] >> 4UL*పరిమాణం(గుర్తులేని దీర్ఘ);
      res[1] &= మాట_ముసుగు;
      res[2] &= మాట_ముసుగు;

      tmpa = (a >> 4UL*పరిమాణం(గుర్తులేని దీర్ఘ)) & మాట_ముసుగు;
      res[2] += tmpa * tmpb;
      res[3] += res[2] >> 4UL*పరిమాణం(గుర్తులేని దీర్ఘ);
      res[2] &= మాట_ముసుగు;

      ఎగువ = (res[3] << 4UL*పరిమాణం(గుర్తులేని దీర్ఘ)) | res[2];
      దిగువ  = (res[1] << 4UL*పరిమాణం(గుర్తులేని దీర్ఘ)) | res[0];
}

vvp_vector2_t క్రియాకారకం * (స్థిరరాశి vvp_vector2_t&a, స్థిరరాశి vvp_vector2_t&b)
{
      స్థిరరాశి గుర్తులేని బిట్లు_చొప్పున_మాట_2 = 8 * పరిమాణం(a.vec_[0]);

	// The compiler ensures that the two operands are of equal size.
      ప్రతిజ్ఞా(a.size() == b.size());
      vvp_vector2_t r (0, a.size());

      గుర్తులేని పదాలు = (r.wid_ + బిట్లు_చొప్పున_మాట_2 - 1) / బిట్లు_చొప్పున_మాట_2;

      కొరకు (గుర్తులేని bdx = 0 ;  bdx < పదాలు ;  bdx += 1) {
	    గుర్తులేని దీర్ఘ tmpb = b.vec_[bdx];
	    ఒకవేళ (tmpb == 0)
		  కొనసాగు;

	    కొరకు (గుర్తులేని adx = 0 ;  adx < పదాలు ;  adx += 1) {
		  గుర్తులేని దీర్ఘ tmpa = a.vec_[adx];
		  ఒకవేళ (tmpa == 0)
			కొనసాగు;

		  గుర్తులేని దీర్ఘ దిగువ, hig;
		  multiply_long(tmpa, tmpb, దిగువ, hig);

		  గుర్తులేని దీర్ఘ వాహకం = 0;
		  కొరకు (గుర్తులేని sdx = 0
			     ; (adx+bdx+sdx) < పదాలు
			     ;  sdx += 1) {

			r.vec_[adx+bdx+sdx] = కూడు_వాహకం(r.vec_[adx+bdx+sdx],
							దిగువ, వాహకం);
			దిగువ = hig;
			hig = 0;
		  }
	    }
      }

      తిరిగి r;
}

స్థిర శూన్య div_mod (vvp_vector2_t dividend, vvp_vector2_t భాజకం,
		     vvp_vector2_t&quotient, vvp_vector2_t&remainder)
{

      quotient = vvp_vector2_t(0, dividend.size());

      ఒకవేళ (భాజకం == quotient) {
	    cerr << "ERROR: division by zero, exiting." << పంక్తి_ముగింపు;
	    నిష్క్రమించు(255);
      }

      ఒకవేళ (dividend < భాజకం) {
	    remainder = dividend;
	    తిరిగి;
      }

      vvp_vector2_t ముసుగు (1, dividend.size());

	// Make the dividend 1 bit larger to prevent overflow of
	// divtmp in startup.
      dividend = vvp_vector2_t(dividend, dividend.size()+1);
      vvp_vector2_t divtmp (భాజకం, dividend.size());

      అయితే (divtmp < dividend) {
	    divtmp <<= 1;
	    ముసుగు <<= 1;
      }

      అయితే (dividend >= భాజకం) {
	    ఒకవేళ (divtmp <= dividend) {
		  dividend -= divtmp;
		  quotient += ముసుగు;
	    }

	    divtmp >>= 1;
	    ముసుగు >>= 1;
      }

      remainder = vvp_vector2_t(dividend, ముసుగు.size());
}

vvp_vector2_t క్రియాకారకం - (స్థిరరాశి vvp_vector2_t&that)
{
      vvp_vector2_t neg(that);
      ఒకవేళ (neg.wid_ == 0) తిరిగి neg;

      స్థిరరాశి గుర్తులేని పదాలు = (neg.wid_ + neg.బిట్లు_చొప్పున_మాట-1) /
                                         neg.బిట్లు_చొప్పున_మాట;
      కొరకు (గుర్తులేని idx = 0 ;  idx < పదాలు ;  idx += 1) {
	    neg.vec_[idx] = ~neg.vec_[idx];
      }
      neg += vvp_vector2_t(1, neg.wid_);

      తిరిగి neg;
}

vvp_vector2_t క్రియాకారకం / (స్థిరరాశి vvp_vector2_t&dividend,
			  స్థిరరాశి vvp_vector2_t&భాజకం)
{
      vvp_vector2_t quot, rem;
      div_mod(dividend, భాజకం, quot, rem);
      తిరిగి quot;
}

vvp_vector2_t క్రియాకారకం % (స్థిరరాశి vvp_vector2_t&dividend,
			  స్థిరరాశి vvp_vector2_t&భాజకం)
{
      vvp_vector2_t quot, rem;
      div_mod(dividend, భాజకం, quot, rem);
      తిరిగి rem;
}

తార్కిక క్రియాకారకం > (స్థిరరాశి vvp_vector2_t&a, స్థిరరాశి vvp_vector2_t&b)
{
      స్థిరరాశి గుర్తులేని awords = (a.wid_ + vvp_vector2_t::బిట్లు_చొప్పున_మాట-1) / vvp_vector2_t::బిట్లు_చొప్పున_మాట;
      స్థిరరాశి గుర్తులేని bwords = (b.wid_ + vvp_vector2_t::బిట్లు_చొప్పున_మాట-1) / vvp_vector2_t::బిట్లు_చొప్పున_మాట;

      స్థిరరాశి గుర్తులేని పదాలు = awords > bwords? awords : bwords;

      కొరకు (గుర్తులేని idx = పదాలు ;  idx > 0 ;  idx -= 1) {
	    గుర్తులేని దీర్ఘ aw = (idx <= awords)? a.vec_[idx-1] : 0;
	    గుర్తులేని దీర్ఘ bw = (idx <= bwords)? b.vec_[idx-1] : 0;

	    ఒకవేళ (aw > bw)
		  తిరిగి నిజం;
	    ఒకవేళ (aw < bw)
		  తిరిగి అబద్ధం;
      }

	// If the above loop finishes, then the vectors are equal.
      తిరిగి అబద్ధం;
}

తార్కిక క్రియాకారకం >= (స్థిరరాశి vvp_vector2_t&a, స్థిరరాశి vvp_vector2_t&b)
{
      స్థిరరాశి గుర్తులేని awords = (a.wid_ + vvp_vector2_t::బిట్లు_చొప్పున_మాట-1) / vvp_vector2_t::బిట్లు_చొప్పున_మాట;
      స్థిరరాశి గుర్తులేని bwords = (b.wid_ + vvp_vector2_t::బిట్లు_చొప్పున_మాట-1) / vvp_vector2_t::బిట్లు_చొప్పున_మాట;

      స్థిరరాశి గుర్తులేని పదాలు = awords > bwords? awords : bwords;

      కొరకు (గుర్తులేని idx = పదాలు ;  idx > 0 ;  idx -= 1) {
	    గుర్తులేని దీర్ఘ aw = (idx <= awords)? a.vec_[idx-1] : 0;
	    గుర్తులేని దీర్ఘ bw = (idx <= bwords)? b.vec_[idx-1] : 0;

	    ఒకవేళ (aw > bw)
		  తిరిగి నిజం;
	    ఒకవేళ (aw < bw)
		  తిరిగి అబద్ధం;
      }

	// If the above loop finishes, then the vectors are equal.
      తిరిగి నిజం;
}

తార్కిక క్రియాకారకం < (స్థిరరాశి vvp_vector2_t&a, స్థిరరాశి vvp_vector2_t&b)
{
      స్థిరరాశి గుర్తులేని awords = (a.wid_ + vvp_vector2_t::బిట్లు_చొప్పున_మాట-1) / vvp_vector2_t::బిట్లు_చొప్పున_మాట;
      స్థిరరాశి గుర్తులేని bwords = (b.wid_ + vvp_vector2_t::బిట్లు_చొప్పున_మాట-1) / vvp_vector2_t::బిట్లు_చొప్పున_మాట;

      గుర్తులేని పదాలు = awords;
      ఒకవేళ (bwords > పదాలు)
	    పదాలు = bwords;

      కొరకు (గుర్తులేని idx = పదాలు ;  idx > 0 ;  idx -= 1) {
	    గుర్తులేని దీర్ఘ aw = (idx <= awords)? a.vec_[idx-1] : 0;
	    గుర్తులేని దీర్ఘ bw = (idx <= bwords)? b.vec_[idx-1] : 0;

	    ఒకవేళ (aw < bw)
		  తిరిగి నిజం;
	    ఒకవేళ (aw > bw)
		  తిరిగి అబద్ధం;
      }

	// If the above loop finishes, then the vectors are equal.
      తిరిగి అబద్ధం;
}

తార్కిక క్రియాకారకం <= (స్థిరరాశి vvp_vector2_t&a, స్థిరరాశి vvp_vector2_t&b)
{
	// XXXX For now, only support equal width vectors.
      ప్రతిజ్ఞా(a.wid_ == b.wid_);

      స్థిరరాశి గుర్తులేని awords = (a.wid_ + vvp_vector2_t::బిట్లు_చొప్పున_మాట-1) / vvp_vector2_t::బిట్లు_చొప్పున_మాట;

      కొరకు (గుర్తులేని idx = awords ;  idx > 0 ;  idx -= 1) {
	    ఒకవేళ (a.vec_[idx-1] < b.vec_[idx-1])
		  తిరిగి నిజం;
	    ఒకవేళ (a.vec_[idx-1] > b.vec_[idx-1])
		  తిరిగి అబద్ధం;
      }

	// If the above loop finishes, then the vectors are equal.
      తిరిగి నిజం;
}

తార్కిక క్రియాకారకం == (స్థిరరాశి vvp_vector2_t&a, స్థిరరాశి vvp_vector2_t&b)
{
      స్థిరరాశి గుర్తులేని awords = (a.wid_ + vvp_vector2_t::బిట్లు_చొప్పున_మాట-1) / vvp_vector2_t::బిట్లు_చొప్పున_మాట;
      స్థిరరాశి గుర్తులేని bwords = (b.wid_ + vvp_vector2_t::బిట్లు_చొప్పున_మాట-1) / vvp_vector2_t::బిట్లు_చొప్పున_మాట;

      స్థిరరాశి గుర్తులేని పదాలు = awords > bwords? awords : bwords;

      కొరకు (గుర్తులేని idx = పదాలు ;  idx > 0 ;  idx -= 1) {
	    గుర్తులేని దీర్ఘ aw = (idx <= awords)? a.vec_[idx-1] : 0;
	    గుర్తులేని దీర్ఘ bw = (idx <= bwords)? b.vec_[idx-1] : 0;

	    ఒకవేళ (aw > bw)
		  తిరిగి అబద్ధం;
	    ఒకవేళ (aw < bw)
		  తిరిగి అబద్ధం;
      }

	// If the above loop finishes, then the vectors are equal.
      తిరిగి నిజం;
}


vvp_vector4_t vector2_గమ్యం_vector4(స్థిరరాశి vvp_vector2_t&that, గుర్తులేని wid)
{
      vvp_vector4_t res (wid);

      కొరకు (గుర్తులేని idx = 0 ;  idx < res.size() ;  idx += 1) {
	    vvp_bit4_t బిట్టు = BIT4_0;

	    ఒకవేళ (that.value(idx))
		  బిట్టు = BIT4_1;

	    res.సమితి_బిట్టు(idx, బిట్టు);
      }

      తిరిగి res;
}

స్థిర తార్కిక c4string_శీర్షిక_test(స్థిరరాశి అక్షరం*str)
{
      ఒకవేళ ((str[0] != 'C') && (str[0] != 'c'))
	    తిరిగి అబద్ధం;
      ఒకవేళ ((str[1] != '4') || (str[2] != '<'))
	    తిరిగి అబద్ధం;
      తిరిగి నిజం;
}

తార్కిక c4string_test(స్థిరరాశి అక్షరం*str)
{
      ఒకవేళ (!c4string_శీర్షిక_test(str))
	    తిరిగి అబద్ధం;
      size_t విలువ_కొలత = strspn(str+3, "01xz");
      ఒకవేళ (str[3+విలువ_కొలత] != '>')
	    తిరిగి అబద్ధం;
      ఒకవేళ (str[3+విలువ_కొలత+1] != 0)
	    తిరిగి అబద్ధం;

      తిరిగి నిజం;
}

vvp_vector4_t c4string_గమ్యం_vector4(స్థిరరాశి అక్షరం*str)
{
      ప్రతిజ్ఞా(c4string_శీర్షిక_test(str));

      str += 3;
      స్థిరరాశి అక్షరం*tp = str + strspn(str,"01xz");
      ప్రతిజ్ఞా(tp[0] == '>');

      vvp_vector4_t tmp (tp-str);

      కొరకు (గుర్తులేని idx = 0 ;  idx < tmp.size() ;  idx += 1) {
	    vvp_bit4_t బిట్టు;
	    ఎంపిక (str[idx]) {
		స్థితి '0':
		  బిట్టు = BIT4_0;
		  ఆపు;
		స్థితి '1':
		  బిట్టు = BIT4_1;
		  ఆపు;
		స్థితి 'x':
		  బిట్టు = BIT4_X;
		  ఆపు;
		స్థితి 'z':
		  బిట్టు = BIT4_Z;
		  ఆపు;
		అప్రమేయ:
		  fprintf(stderr, "Unsupported bit value %c(%d).\n", str[idx],
		          str[idx]);
		  ప్రతిజ్ఞా(0);
		  బిట్టు = BIT4_0;
		  ఆపు;
	    }
	    tmp.సమితి_బిట్టు(tmp.size()-idx-1, బిట్టు);
      }

      తిరిగి tmp;
}

ostream& క్రియాకారకం<< (ostream&out, స్థిరరాశి vvp_vector2_t&that)
{
      ఒకవేళ (that.is_NaN()) {
	    out << "NaN";

      } లేకపోతే {
	    out << vector2_గమ్యం_vector4(that, that.size());
      }
      తిరిగి out;
}

vvp_vector8_t::vvp_vector8_t(స్థిరరాశి vvp_vector8_t&that)
{
      కొలత_2 = that.కొలత_2;
      ఒకవేళ (కొలత_2 <= పరిమాణం(val_)) {
	    జ్ఞాపకం_నకలు(val_, that.val_, పరిమాణం val_);
      } లేకపోతే {
	    ptr_ = కొత్త గుర్తులేని అక్షరం[కొలత_2];
	    జ్ఞాపకం_నకలు(ptr_, that.ptr_, కొలత_2);
      }
}

vvp_vector8_t::vvp_vector8_t(స్థిరరాశి vvp_vector4_t&that,
			     గుర్తులేని str0, గుర్తులేని str1)
: కొలత_2(that.size())
{
      ఒకవేళ (కొలత_2 == 0)
	    తిరిగి;

      ఒకవేళ (కొలత_2 <= పరిమాణం(val_)) {
	    ptr_ = 0; // Prefill all val_ bytes
	    కొరకు (గుర్తులేని idx = 0 ; idx < కొలత_2 ; idx += 1)
		  val_[idx] = vvp_scalar_t(that.value(idx),str0, str1).raw();
      } లేకపోతే {
	    ptr_ = కొత్త గుర్తులేని అక్షరం[కొలత_2];
	    కొరకు (గుర్తులేని idx = 0 ;  idx < కొలత_2 ;  idx += 1)
		  ptr_[idx] = vvp_scalar_t(that.value(idx), str0, str1).raw();
      }
}

vvp_vector8_t::vvp_vector8_t(స్థిరరాశి vvp_vector2_t&that,
			     గుర్తులేని str0, గుర్తులేని str1)
: కొలత_2(that.size())
{
      ఒకవేళ (కొలత_2 == 0)
	    తిరిగి;

      ఒకవేళ (కొలత_2 <= పరిమాణం(val_)) {
	    ptr_ = 0;
	    కొరకు (గుర్తులేని idx = 0 ; idx < కొలత_2 ; idx += 1)
		  val_[idx] = vvp_scalar_t(that.value(idx)? BIT4_1:BIT4_0, str0, str1).raw();
      } లేకపోతే {
	    ptr_ = కొత్త గుర్తులేని అక్షరం[కొలత_2];
	    కొరకు (గుర్తులేని idx = 0 ; idx < కొలత_2 ; idx += 1)
		  ptr_[idx] = vvp_scalar_t(that.value(idx)? BIT4_1:BIT4_0, str0, str1).raw();
      }
}

స్థిరరాశి vvp_vector8_t vvp_vector8_t::nil;

vvp_vector8_t& vvp_vector8_t::క్రియాకారకం= (స్థిరరాశి vvp_vector8_t&that)
{
	// Assign to self.
      ఒకవేళ (ఇది == &that)
	    తిరిగి *ఇది;

      ఒకవేళ (కొలత_2 != that.కొలత_2) {
	    ఒకవేళ (కొలత_2 > పరిమాణం(val_))
		  తొలగించు[]ptr_;
	    కొలత_2 = 0;
      }

      ఒకవేళ (that.కొలత_2 == 0) {
	    ప్రతిజ్ఞా(కొలత_2 == 0);
	    తిరిగి *ఇది;
      }

      ఒకవేళ (that.కొలత_2 <= పరిమాణం(val_)) {
	    కొలత_2 = that.కొలత_2;
	    జ్ఞాపకం_నకలు(val_, that.val_, పరిమాణం(val_));
	    తిరిగి *ఇది;
      }

      ఒకవేళ (కొలత_2 == 0) {
	    కొలత_2 = that.కొలత_2;
	    ptr_ = కొత్త గుర్తులేని అక్షరం[కొలత_2];
      }

      జ్ఞాపకం_నకలు(ptr_, that.ptr_, కొలత_2);

      తిరిగి *ఇది;
}

vvp_vector8_t vvp_vector8_t::subvalue(గుర్తులేని పునాది, గుర్తులేని wid) స్థిరరాశి
{
      vvp_vector8_t tmp (wid);

      గుర్తులేని అక్షరం*tmp_ptr = tmp.కొలత_2 <= పరిమాణం(val_) ? tmp.val_ : tmp.ptr_;
      స్థిరరాశి గుర్తులేని అక్షరం*use_ptr = కొలత_2 <= పరిమాణం(val_) ? val_ : ptr_;

      గుర్తులేని idx = 0;
      అయితే ((idx < wid) && (పునాది+idx < కొలత_2)) {
	    tmp_ptr[idx] = use_ptr[పునాది+idx];
	    idx += 1;
      }

      తిరిగి tmp;
}

శూన్య vvp_vector8_t::సమితి_vec(గుర్తులేని పునాది, స్థిరరాశి vvp_vector8_t&that)
{
      ప్రతిజ్ఞా((పునాది+that.size()) <= size());
      కొరకు (గుర్తులేని idx = 0 ; idx < that.size() ; idx += 1)
	    సమితి_బిట్టు(పునాది+idx, that.value(idx));
}

vvp_vector8_t part_expand(స్థిరరాశి vvp_vector8_t&that, గుర్తులేని wid, గుర్తులేని off)
{
      ప్రతిజ్ఞా(off < wid);
      vvp_vector8_t tmp (wid);

      గుర్తులేని అక్షరం* tmp_ptr = tmp.కొలత_2<= పరిమాణం(tmp.val_) ?
                               tmp.val_ : tmp.ptr_;
      స్థిరరాశి గుర్తులేని అక్షరం* that_ptr = that.కొలత_2<= పరిమాణం(that.val_) ?
                                      that.val_ : that.ptr_;

      గుర్తులేని idx = off;

      అయితే (idx < wid && that.కొలత_2 > (idx-off)) {
	    tmp_ptr[idx] = that_ptr[idx-off];
	    idx += 1;
      }

      తిరిగి tmp;
}

స్థిర తార్కిక c8string_శీర్షిక_test(స్థిరరాశి అక్షరం*str)
{
      ఒకవేళ ((str[0] != 'C') && (str[0] != 'c'))
	    తిరిగి అబద్ధం;
      ఒకవేళ ((str[1] != '8') || (str[2] != '<'))
	    తిరిగి అబద్ధం;
      తిరిగి నిజం;
}

తార్కిక c8string_test(స్థిరరాశి అక్షరం*str)
{
      ఒకవేళ (!c8string_శీర్షిక_test(str))
	    తిరిగి అబద్ధం;

      స్థిరరాశి అక్షరం*cp = str+3;
      కొరకు (;; cp += 1) {
	    ఒకవేళ (cp[0] == '>' && cp[1] == 0) తిరిగి నిజం;
	    ఒకవేళ (cp[0] >= '0' && cp[0] <= '9') కొనసాగు;
	    ఒకవేళ (cp[0] == 'x') కొనసాగు;
	    ఒకవేళ (cp[0] == 'z') కొనసాగు;
	    తిరిగి అబద్ధం;
      }
      తిరిగి అబద్ధం;
}
/*
 * The format of a C8<> string is:
 *   C8<aaabbbccc...>
 * where aaa... is a 3 character bit descriptor.
 */
vvp_vector8_t c8string_గమ్యం_vector8(స్థిరరాశి అక్షరం*str)
{
      ప్రతిజ్ఞా(c8string_శీర్షిక_test(str));

      size_t vsize = అక్షరశ్రేణి_పొడవు(str)-4;
      ప్రతిజ్ఞా(vsize%3 == 0);
      vsize /= 3;
      vvp_vector8_t tmp (vsize);

      కొరకు (size_t idx = 0 ; idx < vsize ; idx += 1) {
	    స్థిరరాశి అక్షరం*cp = str+3+3*idx;
	    vvp_bit4_t బిట్టు = BIT4_X;
	    గుర్తులేని dr0 = cp[0]-'0';
	    గుర్తులేని dr1 = cp[1]-'0';
	    ఎంపిక (cp[2]) {
		స్థితి '0':
		  బిట్టు = BIT4_0;
		  ఆపు;
		స్థితి '1':
		  బిట్టు = BIT4_1;
		  ఆపు;
		స్థితి 'x':
		  బిట్టు = BIT4_X;
		  ఆపు;
		స్థితి 'z':
		  బిట్టు = BIT4_Z;
		  ఆపు;
	    }
	    tmp.సమితి_బిట్టు(vsize-idx-1, vvp_scalar_t(బిట్టు, dr0, dr1));
      }

      తిరిగి tmp;
}

ostream& క్రియాకారకం<<(ostream&out, స్థిరరాశి vvp_vector8_t&that)
{
      out << "C8<";
      కొరకు (గుర్తులేని idx = 0 ;  idx < that.size() ; idx += 1)
	    out << that.value(that.size()-idx-1);

      out << ">";
      తిరిగి out;
}

vvp_net_fun_t::vvp_net_fun_t()
{
      లెక్క_functors += 1;
}

vvp_net_fun_t::~vvp_net_fun_t()
{
}

శూన్య vvp_net_fun_t::recv_vec4(vvp_net_ptr_t, స్థిరరాశి vvp_vector4_t&,
                              vvp_context_t)
{
      fprintf(stderr, "internal error: %s: recv_vec4 not implemented\n",
	      typeid(*ఇది).name());
      ప్రతిజ్ఞా(0);
}

శూన్య vvp_net_fun_t::recv_vec4_pv_(vvp_net_ptr_t p, స్థిరరాశి vvp_vector4_t&బిట్టు,
                                  గుర్తులేని పునాది, గుర్తులేని vwid, vvp_context_t)
{
	// The majority of functors don't normally expect to receive part
	// values, because the primary operands of an expression will be
	// extended to the expression width. But in the case that a primary
	// operand is a wire that is only partly driven by a single driver,
	// the part value driven onto the wire propagates directly to the
	// inputs of any functors connected to that wire. In this case we
	// know that the remaining bits are undriven, so can simply build
	// the full width value from the part we have received.
	//
	// Note that this case is almost certainly a bug in the user's
	// code, but we still need to handle it correctly. See GitHub
	// issue #99 and br_gh99*.v in the test suite for examples.

      ప్రతిజ్ఞా(పునాది + బిట్టు.size() <= vwid);

      vvp_vector4_t tmp(vwid, BIT4_Z);
      tmp.సమితి_vec(పునాది, బిట్టు);
      recv_vec4(p, tmp, 0);
}

శూన్య vvp_net_fun_t::recv_vec4_pv(vvp_net_ptr_t, స్థిరరాశి vvp_vector4_t&బిట్టు,
                                 గుర్తులేని పునాది, గుర్తులేని vwid, vvp_context_t)
{
      cerr << "internal error: " << typeid(*ఇది).name() << ": "
	   << "recv_vec4_pv(" << బిట్టు << ", " << పునాది
	   << ", " << vwid << ") not implemented" << పంక్తి_ముగింపు;
      ప్రతిజ్ఞా(0);
}

శూన్య vvp_net_fun_t::recv_vec8(vvp_net_ptr_t పోర్టు, స్థిరరాశి vvp_vector8_t&బిట్టు)
{
      recv_vec4(పోర్టు, reduce4(బిట్టు), 0);
}

శూన్య vvp_net_fun_t::recv_vec8_pv_(vvp_net_ptr_t p, స్థిరరాశి vvp_vector8_t&బిట్టు,
				  గుర్తులేని పునాది, గుర్తులేని vwid)
{
	// This is the strength-aware version of recv_vec4_pv_.

      ప్రతిజ్ఞా(పునాది + బిట్టు.size() <= vwid);

      vvp_vector8_t tmp(vwid);
      tmp.సమితి_vec(పునాది, బిట్టు);
      recv_vec8(p, tmp);
}

శూన్య vvp_net_fun_t::recv_vec8_pv(vvp_net_ptr_t పోర్టు, స్థిరరాశి vvp_vector8_t&బిట్టు,
				 గుర్తులేని పునాది, గుర్తులేని vwid)
{
      recv_vec4_pv(పోర్టు, reduce4(బిట్టు), పునాది, vwid, 0);
}

శూన్య vvp_net_fun_t::recv_real(vvp_net_ptr_t, ద్విగుణ బిట్టు, vvp_context_t)
{
      fprintf(stderr, "internal error: %s: recv_real(%f) not implemented\n",
	      typeid(*ఇది).name(), బిట్టు);
      ప్రతిజ్ఞా(0);
}

శూన్య vvp_net_fun_t::recv_string(vvp_net_ptr_t, స్థిరరాశి std::string&బిట్టు, vvp_context_t)
{
      fprintf(stderr, "internal error: %s: recv_string(%s) not implemented\n",
	      typeid(*ఇది).name(), బిట్టు.c_str());
      ప్రతిజ్ఞా(0);
}

శూన్య vvp_net_fun_t::recv_object(vvp_net_ptr_t, vvp_object_t, vvp_context_t)
{
      fprintf(stderr, "internal error: %s: recv_object(...) not implemented\n",
	      typeid(*ఇది).name());
      ప్రతిజ్ఞా(0);
}

శూన్య vvp_net_fun_t::force_flag(తార్కిక)
{
}

/* **** vvp_fun_drive methods **** */

vvp_fun_drive::vvp_fun_drive(గుర్తులేని str0, గుర్తులేని str1)
{
      ప్రతిజ్ఞా(str0 < 8);
      ప్రతిజ్ఞా(str1 < 8);

      drive0_ = str0;
      drive1_ = str1;
}

vvp_fun_drive::~vvp_fun_drive()
{
}

శూన్య vvp_fun_drive::recv_vec4(vvp_net_ptr_t పోర్టు, స్థిరరాశి vvp_vector4_t&బిట్టు,
                              vvp_context_t)
{
      ప్రతిజ్ఞా(పోర్టు.పోర్టు() == 0);
      పోర్టు.ptr()->send_vec8(vvp_vector8_t(బిట్టు, drive0_, drive1_));
}

శూన్య vvp_fun_drive::recv_vec4_pv(vvp_net_ptr_t ptr, స్థిరరాశి vvp_vector4_t&బిట్టు,
				 గుర్తులేని పునాది, గుర్తులేని vwid, vvp_context_t ctx)
{
      recv_vec4_pv_(ptr, బిట్టు, పునాది, vwid, ctx);
}

/* **** vvp_wide_fun_* methods **** */

vvp_wide_fun_core::vvp_wide_fun_core(vvp_net_t*net, గుర్తులేని nports)
{
      ptr_ = net;
      nports_ = nports;
      పోర్టు_విలువలు = 0;
      పోర్టు_rvalues = 0;
}

vvp_wide_fun_core::~vvp_wide_fun_core()
{
      తొలగించు[] పోర్టు_విలువలు;
      తొలగించు[] పోర్టు_rvalues;
}

శూన్య vvp_wide_fun_core::propagate_vec4(స్థిరరాశి vvp_vector4_t&బిట్టు,
				       vvp_time64_t delay)
{
      ఒకవేళ (delay)
	    కాలపట్టిక_propagate_సదిశ(ptr_, delay, బిట్టు);
      లేకపోతే
	    ptr_->send_vec4(బిట్టు, 0);
}

శూన్య vvp_wide_fun_core::propagate_real(ద్విగుణ బిట్టు,
				       vvp_time64_t delay)
{
      ఒకవేళ (delay) {
	      // schedule_assign_vector(ptr_->out, bit, delay);
	    ప్రతిజ్ఞా(0); // Need a real-value version of assign_vector.
      } లేకపోతే {
	    ptr_->send_real(బిట్టు, 0);
      }
}


గుర్తులేని vvp_wide_fun_core::పోర్టు_లెక్క() స్థిరరాశి
{
      తిరిగి nports_;
}

vvp_vector4_t& vvp_wide_fun_core::value(గుర్తులేని idx)
{
      ప్రతిజ్ఞా(idx < nports_);
      ఒకవేళ (పోర్టు_విలువలు == 0)
	    పోర్టు_విలువలు = కొత్త vvp_vector4_t [nports_];
      తిరిగి పోర్టు_విలువలు[idx];
}

ద్విగుణ vvp_wide_fun_core::విలువ_r(గుర్తులేని idx)
{
      ప్రతిజ్ఞా(idx < nports_);
      తిరిగి పోర్టు_rvalues? పోర్టు_rvalues[idx] : 0.0;
}

శూన్య vvp_wide_fun_core::recv_real_from_inputs(గుర్తులేని)
{
      ప్రతిజ్ఞా(0);
}

శూన్య vvp_wide_fun_core::dispatch_vec4_from_ఆదానం(గుర్తులేని పోర్టు,
						  స్థిరరాశి vvp_vector4_t&బిట్టు)
{
      ప్రతిజ్ఞా(పోర్టు < nports_);
      ఒకవేళ (పోర్టు_విలువలు == 0) పోర్టు_విలువలు = కొత్త vvp_vector4_t [nports_];
      పోర్టు_విలువలు[పోర్టు] = బిట్టు;
      recv_vec4_from_inputs(పోర్టు);
}

శూన్య vvp_wide_fun_core::dispatch_real_from_ఆదానం(గుర్తులేని పోర్టు,
						  ద్విగుణ బిట్టు)
{
      ప్రతిజ్ఞా(పోర్టు < nports_);
      ఒకవేళ (పోర్టు_rvalues == 0) పోర్టు_rvalues = కొత్త ద్విగుణ[nports_];
      పోర్టు_rvalues[పోర్టు] = బిట్టు;
      recv_real_from_inputs(పోర్టు);
}

vvp_wide_fun_t::vvp_wide_fun_t(vvp_wide_fun_core*c, గుర్తులేని పునాది)
: core_(c), పోర్టు_పునాది_2(పునాది)
{
}

vvp_wide_fun_t::~vvp_wide_fun_t()
{
}

శూన్య vvp_wide_fun_t::recv_vec4(vvp_net_ptr_t పోర్టు, స్థిరరాశి vvp_vector4_t&బిట్టు,
                               vvp_context_t)
{
      గుర్తులేని pidx = పోర్టు_పునాది_2 + పోర్టు.పోర్టు();
      core_->dispatch_vec4_from_ఆదానం(pidx, బిట్టు);
}

శూన్య vvp_wide_fun_t::recv_vec4_pv(vvp_net_ptr_t ptr, స్థిరరాశి vvp_vector4_t&బిట్టు,
				  గుర్తులేని పునాది, గుర్తులేని vwid, vvp_context_t ctx)
{
      recv_vec4_pv_(ptr, బిట్టు, పునాది, vwid, ctx);
}

శూన్య vvp_wide_fun_t::recv_real(vvp_net_ptr_t పోర్టు, ద్విగుణ బిట్టు,
                               vvp_context_t)
{
      గుర్తులేని pidx = పోర్టు_పునాది_2 + పోర్టు.పోర్టు();
      core_->dispatch_real_from_ఆదానం(pidx, బిట్టు);
}

/* **** vvp_scalar_t methods **** */

/*
 * DRIVE STRENGTHS:
 *
 * The normal functor is not aware of strengths. It generates strength
 * simply by virtue of having strength specifications. The drive
 * strength specification includes a drive0 and drive1 strength, each
 * with 8 possible values (that can be represented in 3 bits) as given
 * in this table:
 *
 *    HiZ    = 0,
 *    SMALL  = 1,
 *    MEDIUM = 2,
 *    WEAK   = 3,
 *    LARGE  = 4,
 *    PULL   = 5,
 *    STRONG = 6,
 *    SUPPLY = 7
 *
 * The vvp_scalar_t value, however, is a combination of value and
 * strength, used in strength-aware contexts.
 *
 * OUTPUT STRENGTHS:
 *
 * The strength-aware values are specified as an 8 bit value, that is
 * two 4 bit numbers. The value is encoded with two drive strengths (0-7)
 * and two drive values (0 or 1). Each nibble contains three bits of
 * strength and one bit of value, like so: VSSS. The high nibble has
 * the strength-value closest to supply1, and the low nibble has the
 * strength-value closest to supply0.
 */

/*
 * A signal value is unambiguous if the top 4 bits and the bottom 4
 * bits are identical. This means that the VSSSvsss bits of the 8bit
 * value have V==v and SSS==sss.
 */
# define UNAMBIG(v)  (((v) & 0x0f) == (((v) >> 4) & 0x0f))

# define STREN1(v) (((v)&0x70) >> 4)
# define STREN0(v) ((v)&0x07)

గుర్తులేని vvp_scalar_t::strength0() స్థిరరాశి
{
      తిరిగి STREN0(విలువ_2);
}

గుర్తులేని vvp_scalar_t::strength1() స్థిరరాశి
{
      తిరిగి STREN1(విలువ_2);
}

ostream& క్రియాకారకం <<(ostream&out, vvp_scalar_t a)
{
      out << a.strength0() << a.strength1();
      ఎంపిక (a.value()) {
	  స్థితి BIT4_0:
	    out << "0";
	    ఆపు;
	  స్థితి BIT4_1:
	    out << "1";
	    ఆపు;
	  స్థితి BIT4_X:
	    out << "X";
	    ఆపు;
	  స్థితి BIT4_Z:
	    out << "Z";
	    ఆపు;
      }
      తిరిగి out;
}

/*
 * This function is only called if the actual interface function rules
 * out some of the easy cases. If we get here, we can assume that
 * neither of the values is HiZ, and the values are not exactly equal.
 */
vvp_scalar_t fully_featured_resolv_(vvp_scalar_t a, vvp_scalar_t b)
{

      ఒకవేళ (UNAMBIG(a.విలువ_2) && UNAMBIG(b.విలువ_2)) {

	      /* If both signals are unambiguous, simply choose
		 the stronger. If they have the same strength
		 but different values, then this becomes
		 ambiguous. */

	    ఒకవేళ ((b.విలువ_2&0x07) > (a.విలువ_2&0x07)) {

		    /* b value is stronger. Take it. */
		  తిరిగి b;

	    } లేకపోతే ఒకవేళ ((b.విలువ_2&0x77) == (a.విలువ_2&0x77)) {

		    // Strengths are the same. Since we know already
		    // that the values are not the same, Make value
		    // into "x".
		  vvp_scalar_t tmp (a);
		  tmp.విలువ_2 = (tmp.విలువ_2&0x77) | 0x80;
		  తిరిగి tmp;

	    } లేకపోతే {

		    /* Must be "a" is the stronger one. */
		  తిరిగి a;
	    }

      }

	/* If one of the signals is unambiguous, then it
	   will sweep up the weaker parts of the ambiguous
	   signal. The result may be ambiguous, or maybe not. */

      ఒకవేళ (UNAMBIG(a.విలువ_2)) {
	    vvp_scalar_t res;

	    ఒకవేళ ((a.విలువ_2&0x70) > (b.విలువ_2&0x70))
		  res.విలువ_2 |= a.విలువ_2&0xf0;
	    లేకపోతే
		  res.విలువ_2 |= b.విలువ_2&0xf0;

	    ఒకవేళ ((a.విలువ_2&0x07) > (b.విలువ_2&0x07))
		  res.విలువ_2 |= a.విలువ_2&0x0f;
	    లేకపోతే
		  res.విలువ_2 |= b.విలువ_2&0x0f;

	    తిరిగి res;

      } లేకపోతే ఒకవేళ (UNAMBIG(b.విలువ_2)) {

	    vvp_scalar_t res;

	    ఒకవేళ ((b.విలువ_2&0x70) > (a.విలువ_2&0x70))
		  res.విలువ_2 |= b.విలువ_2&0xf0;
	    లేకపోతే
		  res.విలువ_2 |= a.విలువ_2&0xf0;

	    ఒకవేళ ((b.విలువ_2&0x07) > (a.విలువ_2&0x07))
		  res.విలువ_2 |= b.విలువ_2&0x0f;
	    లేకపోతే
		  res.విలువ_2 |= a.విలువ_2&0x0f;

	    తిరిగి res;

      }


	/* If both signals are ambiguous, then the result
	   has an even wider ambiguity. */

      గుర్తులేని tmp = 0;
      పూర్ణాంకం sv1a = (a.విలువ_2 & 0x80) ? STREN1(a.విలువ_2) : - STREN1(a.విలువ_2);
      పూర్ణాంకం sv0a = (a.విలువ_2 & 0x08) ? STREN0(a.విలువ_2) : - STREN0(a.విలువ_2);
      పూర్ణాంకం sv1b = (b.విలువ_2 & 0x80) ? STREN1(b.విలువ_2) : - STREN1(b.విలువ_2);
      పూర్ణాంకం sv0b = (b.విలువ_2 & 0x08) ? STREN0(b.విలువ_2) : - STREN0(b.విలువ_2);

      పూర్ణాంకం sv1 = sv1a;
      పూర్ణాంకం sv0 = sv0a;

      ఒకవేళ (sv0a > sv1)
	    sv1 = sv0a;
      ఒకవేళ (sv1b > sv1)
	    sv1 = sv1b;
      ఒకవేళ (sv0b > sv1)
	    sv1 = sv0b;

      ఒకవేళ (sv1a < sv0)
	    sv0 = sv1a;
      ఒకవేళ (sv1b < sv0)
	    sv0 = sv1b;
      ఒకవేళ (sv0b < sv0)
	    sv0 = sv0b;

      ఒకవేళ (sv1 > 0) {
	    tmp |= 0x80;
	    tmp |= sv1 << 4;
      } లేకపోతే {
	      /* Set the MSB when both arguments MSBs are set. This
		 can only happen if both one strengths are zero. */
	    tmp |= (a.విలువ_2&b.విలువ_2)&0x80;
	    tmp |= (-sv1) << 4;
      }

      ఒకవేళ (sv0 > 0) {
	    tmp |= 0x08;
	    tmp |= sv0;
      } లేకపోతే {
	    tmp |= (-sv0);
      }

      vvp_scalar_t res;
      res.విలువ_2 = tmp;

	/* Canonicalize the HiZ value. */
      ఒకవేళ ((res.విలువ_2&0x77) == 0)
	    res.విలువ_2 = 0;

      తిరిగి res;
}

గుర్తులేని vvp_switch_strength_map[2][8] = {
      {  // non-resistive
	    0, /* High impedance   --> High impedance   */
	    1, /* Small capacitor  --> Small capacitor  */
	    2, /* Medium capacitor --> Medium capacitor */
	    3, /* Weak drive       --> Weak drive       */
	    4, /* Large capacitor  --> Large capacitor  */
	    5, /* Pull drive       --> Pull drive       */
	    6, /* Strong drive     --> Strong drive     */
	    6  /* Supply drive     --> Strong drive     */
      },
      {  // resistive
	    0, /* High impedance   --> High impedance   */
	    1, /* Small capacitor  --> Small capacitor  */
	    1, /* Medium capacitor --> Small capacitor  */
	    2, /* Weak drive       --> Medium capacitor */
	    2, /* Large capacitor  --> Medium capacitor */
	    3, /* Pull drive       --> Weak drive       */
	    5, /* Strong drive     --> Pull drive       */
	    5  /* Supply drive     --> Pull drive       */
      }
};

vvp_vector4_t reduce4(స్థిరరాశి vvp_vector8_t&that)
{
      vvp_vector4_t out (that.size());
      కొరకు (గుర్తులేని idx = 0 ;  idx < out.size() ;  idx += 1)
	    out.సమితి_బిట్టు(idx, that.value(idx).value());

      తిరిగి out;
}

vvp_bit4_t compare_gtge(స్థిరరాశి vvp_vector4_t&lef, స్థిరరాశి vvp_vector4_t&rig,
			vvp_bit4_t బహిర్గతం_if_equal)
{
      గుర్తులేని min_కొలత = lef.size();
      ఒకవేళ (rig.size() < min_కొలత)
	    min_కొలత = rig.size();

	// If one of the inputs is nil, treat is as all X values, and
	// that makes the result BIT4_X.
      ఒకవేళ (min_కొలత == 0)
	    తిరిగి BIT4_X;

	// As per the IEEE1364 definition of >, >=, < and <=, if there
	// are any X or Z values in either of the operand vectors,
	// then the result of the compare is BIT4_X.

	// Check for X/Z in the left operand
      ఒకవేళ (lef.has_xz())
	    తిరిగి BIT4_X;

	// Check for X/Z in the right operand
      ఒకవేళ (rig.has_xz())
	    తిరిగి BIT4_X;

      కొరకు (గుర్తులేని idx = lef.size() ; idx > rig.size() ;  idx -= 1) {
	    ఒకవేళ (lef.value(idx-1) == BIT4_1)
		  తిరిగి BIT4_1;
      }

      కొరకు (గుర్తులేని idx = rig.size() ; idx > lef.size() ;  idx -= 1) {
	    ఒకవేళ (rig.value(idx-1) == BIT4_1)
		  తిరిగి BIT4_0;
      }

      కొరకు (గుర్తులేని idx = min_కొలత ; idx > 0 ;  idx -= 1) {
	    vvp_bit4_t lv = lef.value(idx-1);
	    vvp_bit4_t rv = rig.value(idx-1);

	    ఒకవేళ (lv == rv)
		  కొనసాగు;

	    ఒకవేళ (lv == BIT4_1)
		  తిరిగి BIT4_1;
	    లేకపోతే
		  తిరిగి BIT4_0;
      }

      తిరిగి బహిర్గతం_if_equal;
}

vvp_bit4_t compare_gtge_signed(స్థిరరాశి vvp_vector4_t&a,
			       స్థిరరాశి vvp_vector4_t&b,
			       vvp_bit4_t బహిర్గతం_if_equal)
{
      ప్రతిజ్ఞా(a.size() == b.size());

      గుర్తులేని sign_idx = a.size()-1;
      vvp_bit4_t a_sign = a.value(sign_idx);
      vvp_bit4_t b_sign = b.value(sign_idx);

      ఒకవేళ (bit4_is_xz(a_sign))
	    తిరిగి BIT4_X;
      ఒకవేళ (bit4_is_xz(b_sign))
	    తిరిగి BIT4_X;

      ఒకవేళ (a_sign == b_sign)
	    తిరిగి compare_gtge(a, b, బహిర్గతం_if_equal);

      ఒకవేళ (a.has_xz())
	    తిరిగి BIT4_X;

      ఒకవేళ (b.has_xz())
	    తిరిగి BIT4_X;

      ఒకవేళ(a_sign == BIT4_0)
	    తిరిగి BIT4_1;
      లేకపోతే
	    తిరిగి BIT4_0;
}
