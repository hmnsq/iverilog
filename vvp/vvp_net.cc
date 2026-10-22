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
#که_تعریف_شوی CHECK_WITH_VALGRIND
# include  <valgrind/memcheck.h>
# include  <map>
# include  "sfunc.h"
# include  "udp.h"
# include  "ivl_alloc.h"
#د_که_پای

کارول نطاق std;

/* This is the size of an unsigned long in bits. This is just a
   convenience macro. */
# define CPU_کلمه_بټونه (8*حجم(بینښه اوږد))
# define سر_بټ (1UL << (CPU_کلمه_بټونه-1))

permaheap vvp_net_fun_t::ډېرۍ;
permaheap vvp_net_fil_t::ډېرۍ;

// Allocate around 1Megabyte/chunk.
ساكن ثابت size_t VVP_NET_CHUNK = 1024*1024/حجم(vvp_net_t);
ساكن vvp_net_t*vvp_net_alloc_جدول = عدم;
#که_تعریف_شوی CHECK_WITH_VALGRIND
ساكن vvp_net_t **vvp_net_pool = عدم;
ساكن بینښه vvp_net_pool_شمېر = 0;
#د_که_پای
ساكن size_t vvp_net_alloc_پاتې_شوی = 0;
// For statistics, count the vvp_nets allocated and the bytes of alloc
// chunks allocated.
بینښه اوږد شمېر_vvp_nets = 0;
size_t اندازه_vvp_nets = 0;

فراغ* vvp_net_t::چلوونکی جديد (size_t size)
{
      أكد(size == حجم(vvp_net_t));
      إذا (vvp_net_alloc_پاتې_شوی == 0) {
	    vvp_net_alloc_جدول = ::جديد vvp_net_t[VVP_NET_CHUNK];
	    vvp_net_alloc_پاتې_شوی = VVP_NET_CHUNK;
	    اندازه_vvp_nets += size*VVP_NET_CHUNK;
#که_تعریف_شوی CHECK_WITH_VALGRIND
	    VALGRIND_MAKE_MEM_NOACCESS(vvp_net_alloc_جدول, size*VVP_NET_CHUNK);
	    VALGRIND_CREATE_MEMPOOL(vvp_net_alloc_جدول, 0, 0);
	    vvp_net_pool_شمېر += 1;
	    vvp_net_pool = static_cast<vvp_net_t **>(realloc(vvp_net_pool,
	                   vvp_net_pool_شمېر*حجم(vvp_net_t **)));
	    vvp_net_pool[vvp_net_pool_شمېر-1] = vvp_net_alloc_جدول;
#د_که_پای
      }

      vvp_net_t*return_this = vvp_net_alloc_جدول;
#که_تعریف_شوی CHECK_WITH_VALGRIND
      VALGRIND_MEMPOOL_ALLOC(vvp_net_pool[vvp_net_pool_شمېر-1],
                             return_this, size);
      return_this->pool = vvp_net_pool[vvp_net_pool_شمېر-1];
#د_که_پای
      vvp_net_alloc_جدول += 1;
      vvp_net_alloc_پاتې_شوی -= 1;
      شمېر_vvp_nets += 1;
      أرجع return_this;
}

#که_تعریف_شوی CHECK_WITH_VALGRIND
ساكن map<vvp_net_t*, منطقي> vvp_net_map;
ساكن map<sfunc_core*, منطقي> sfunc_map;
ساكن map<vvp_udp_fun_core*, منطقي> udp_map;
ساكن map<resolv_core*, منطقي> resolv_map;
ساكن vvp_net_t **local_net_pool = 0;
ساكن بینښه local_net_pool_شمېر = 0;

فراغ pool_local_net(vvp_net_t*net)
{
      local_net_pool_شمېر += 1;
      local_net_pool = static_cast<vvp_net_t **>(realloc(local_net_pool,
                       local_net_pool_شمېر*حجم(vvp_net_t **)));
      local_net_pool[local_net_pool_شمېر-1] = net;
}

فراغ vvp_net_delete(vvp_net_t *توکی)
{
      vvp_net_map[توکی] = ریښتیا;
      إذا (sfunc_core*tmp = dynamic_cast<sfunc_core*> (توکی->fun)) {
	    sfunc_map[tmp] = ریښتیا;
      }
      إذا (vvp_udp_fun_core*tmp = dynamic_cast<vvp_udp_fun_core*> (توکی->fun)) {
	    udp_map[tmp] = ریښتیا;
      }
      إذا (resolv_core*tmp = dynamic_cast<resolv_core*> (توکی->fun)) {
	    resolv_map[tmp] = ریښتیا;
      }
	/* Only delete static object variables. */
      إذا (vvp_fun_signal_object_sa*tmp = dynamic_cast<vvp_fun_signal_object_sa*> (توکی->fun)) {
	    ړنګول tmp;
      }
	/* Only delete static string variables. */
      إذا (vvp_fun_signal_string_sa*tmp = dynamic_cast<vvp_fun_signal_string_sa*> (توکی->fun)) {
	    ړنګول tmp;
      }
}

فراغ vvp_net_pool_delete()
{
      بینښه اوږد vvp_nets_del = 0;

      لكل (بینښه idx = 0; idx < local_net_pool_شمېر; idx += 1) {
	    vvp_net_delete(local_net_pool[idx]);
      }
      حرر(local_net_pool);
      local_net_pool = 0;
      local_net_pool_شمېر = 0;

      map<vvp_net_t*, منطقي>::iterator iter;
      لكل (iter = vvp_net_map.begin(); iter != vvp_net_map.پای_ټکی(); ++ iter ) {
	    vvp_nets_del += 1;
	    VALGRIND_MEMPOOL_FREE(iter->لومړی->pool, iter->لومړی);
      }
      vvp_net_map.clear();

      map<sfunc_core*, منطقي>::iterator siter;
      لكل (siter = sfunc_map.begin(); siter != sfunc_map.پای_ټکی(); ++ siter ) {
	    ړنګول siter->لومړی;
      }
      sfunc_map.clear();

      map<vvp_udp_fun_core*, منطقي>::iterator uiter;
      لكل (uiter = udp_map.begin(); uiter != udp_map.پای_ټکی(); ++ uiter ) {
	    ړنګول uiter->لومړی;
      }
      udp_map.clear();

      map<resolv_core*, منطقي>::iterator riter;
      لكل (riter = resolv_map.begin(); riter != resolv_map.پای_ټکی(); ++ riter ) {
	    ړنګول riter->لومړی;
      }
      resolv_map.clear();

      إذا (RUNNING_ON_VALGRIND && (vvp_nets_del != شمېر_vvp_nets)) {
	    fflush(عدم);
	    VALGRIND_PRINTF("Error: vvp missed deleting %ld of %lu net(s).",
	                    (اوږد) شمېر_vvp_nets - vvp_nets_del,
	                    شمېر_vvp_nets);
      }

      لكل (بینښه idx = 0; idx < vvp_net_pool_شمېر; idx += 1) {
	    VALGRIND_DESTROY_MEMPOOL(vvp_net_pool[idx]);
	    ::ړنګول [] vvp_net_pool[idx];
      }
      حرر(vvp_net_pool);
      vvp_net_pool = عدم;
      vvp_net_pool_شمېر = 0;
}
#د_که_پای

فراغ vvp_net_t::چلوونکی ړنګول(فراغ*)
{
      أكد(0);
}

vvp_net_t::vvp_net_t()
: بهر_2(vvp_net_ptr_t(0,0))
{
      fun = 0;
      fil = 0;
}

فراغ vvp_net_t::link(vvp_net_ptr_t پورټ_منزل_link)
{
      vvp_net_t*net = پورټ_منزل_link.ptr();

	// Connect nodes with vvp_fun_modpath_src to the head of the
	// linked list, so that vvp_fun_modpath_src are always evaluated
	// before their respective vvp_fun_modpath
      إذا (dynamic_cast<vvp_fun_modpath_src*>(net->fun)) {
	    net->پورټ[پورټ_منزل_link.پورټ()] = بهر_2;
	    بهر_2 = پورټ_منزل_link;
	// All other nodes connect after the last vvp_fun_modpath_src
      } وإلا {
	      // List is empty or first node has no vvp_fun_modpath_src
	      // then just insert the node into the head of linked list
	    إذا (بهر_2.ptr() == تش_اشاره || ! dynamic_cast<vvp_fun_modpath_src*>(بهر_2.ptr()->fun)) {
		  net->پورټ[پورټ_منزل_link.پورټ()] = بهر_2;
		  بهر_2 = پورټ_منزل_link;
		  أرجع;
	    }

	    vvp_net_ptr_t cur = بهر_2;
	    vvp_net_ptr_t prev = vvp_net_ptr_t(تش_اشاره,0);

	      // Traverse through linked list
	    طالما (dynamic_cast<vvp_fun_modpath_src*>(cur.ptr()->fun)) {
		  prev = cur;
		  cur = cur.ptr()->پورټ[cur.پورټ()];
		  إذا (! cur.ptr()) اكسر; // End of list
	    }

	    أكد(prev.ptr());

	      // Insert after last vvp_fun_modpath_src (or end of list)
	    net->پورټ[پورټ_منزل_link.پورټ()] = cur;
	    prev.ptr()->پورټ[prev.پورټ()] = پورټ_منزل_link;
      }
}

/*
 * Unlink a ptr object from the driver. The input is the driver in the
 * form of a vvp_net_t pointer. The .out member of that object is the
 * driver. The dst_ptr argument is the receiver pin to be located and
 * removed from the fan-out list.
 */
فراغ vvp_net_t::unlink(vvp_net_ptr_t dst_ptr)
{
      vvp_net_t*net = dst_ptr.ptr();
      بینښه net_پورټ = dst_ptr.پورټ();

      إذا (بهر_2 == dst_ptr) {
	      /* If the drive fan-out list starts with this pointer,
		 then the unlink is easy. Pull the list forward. */
	    بهر_2 = net->پورټ[net_پورټ];
      } وإلا إذا (! بهر_2.nil()) {
	      /* Scan the linked list, looking for the net_ptr_t
		 pointer *before* the one we wish to remove. */
	    vvp_net_ptr_t cur = بهر_2;
	    أكد(!cur.nil());
	    vvp_net_t*cur_net = cur.ptr();
	    بینښه cur_پورټ = cur.پورټ();
	    طالما (cur_net && cur_net->پورټ[cur_پورټ] != dst_ptr) {
		  cur = cur_net->پورټ[cur_پورټ];
		  cur_net = cur.ptr();
		  cur_پورټ = cur.پورټ();
	    }
	      /* Unlink. */
	    إذا (cur_net) cur_net->پورټ[cur_پورټ] = net->پورټ[net_پورټ];
      }

      net->پورټ[net_پورټ] = vvp_net_ptr_t(0,0);
}

فراغ vvp_net_t::شمېر_drivers(بینښه idx, بینښه counts[4])
{
      counts[0] = 0;
      counts[1] = 0;
      counts[2] = 0;
      counts[3] = 0;

        /* $countdrivers can only be used on wires. */
      vvp_wire_بنسټ*wire=dynamic_cast<vvp_wire_بنسټ*>(fil);
      أكد(wire);

      إذا (wire->is_forced(idx))
            counts[3] = 1;

        /* If the net has multiple drivers, we need to interrogate the
           resolver network to get the driven values. */
      إذا (resolv_core*resolver = dynamic_cast<resolv_core*>(fun)) {
            resolver->شمېر_drivers(idx, counts);
            أرجع;
      }
      إذا (vvp_island_پورټ*resolver = dynamic_cast<vvp_island_پورټ*>(fun)) {
            resolver->شمېر_drivers(idx, counts);
            أرجع;
      }

        /* If the functor is not a resolver, there is only one driver, so
           we can just interrogate the filter to get the driven value. */
      update_driver_counts(wire->driven_ارزښت(idx), counts);
}

فراغ vvp_net_fun_t::چلوونکی ړنګول(فراغ*)
{
#که_نه_تعریف_شوی CHECK_WITH_VALGRIND
      أكد(0);
#د_که_پای
}


vvp_net_fil_t::vvp_net_fil_t()
{
      force_link_ = 0;
      force_propagate_ = خطأ;
      شمېر_filters += 1;
}

vvp_net_fil_t::~vvp_net_fil_t()
{
      أكد(force_link_ == 0);
}

vvp_net_fil_t::prop_t vvp_net_fil_t::filter_vec4(ثابت vvp_vector4_t&,
                                                 vvp_vector4_t&,
                                                 بینښه, بینښه)
{
      أرجع PROP;
}

vvp_net_fil_t::prop_t vvp_net_fil_t::filter_vec8(ثابت vvp_vector8_t&,
                                                 vvp_vector8_t&,
                                                 بینښه, بینښه)
{
      أرجع PROP;
}

vvp_net_fil_t::prop_t vvp_net_fil_t::filter_real(مزدوج&)
{
      أرجع PROP;
}

vvp_net_fil_t::prop_t vvp_net_fil_t::filter_string(ثابت string&)
{
      أرجع PROP;
}

vvp_net_fil_t::prop_t vvp_net_fil_t::filter_object(vvp_object_t&)
{
      أرجع PROP;
}

فراغ vvp_net_fil_t::force_ماسک(ثابت vvp_vector2_t&ماسک)
{
      إذا (force_ماسک_2.size() == 0)
	    force_ماسک_2 = vvp_vector2_t(vvp_vector2_t::FILL0, ماسک.size());

      أكد(force_ماسک_2.size() == ماسک.size());
      لكل (بینښه idx = 0 ; idx < ماسک.size() ; idx += 1) {
	    إذا (ماسک.value(idx) == 0)
		  استمر;

	    force_ماسک_2.سېټ_بټ(idx, 1);
	    force_propagate_ = ریښتیا;
      }
}

فراغ vvp_net_fil_t::release_ماسک(ثابت vvp_vector2_t&ماسک)
{
      إذا (force_ماسک_2.size() == 0)
	    أرجع;

      أكد(force_ماسک_2.size() == ماسک.size());
      لكل (بینښه idx = 0 ; idx < ماسک.size() ; idx += 1) {
	    إذا (ماسک.value(idx))
		  force_ماسک_2.سېټ_بټ(idx, 0);
      }

      إذا (force_ماسک_2.is_zero())
	    force_ماسک_2 = vvp_vector2_t();
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
فراغ vvp_net_fil_t::force_link(vvp_net_t*dst, vvp_net_t*src)
{
      أكد(dst->fil == هذا);

      إذا (force_link_ == 0) {
	    force_link_ = جديد vvp_net_t;
	      // Use port[3] to hold the force destination.
	    force_link_->پورټ[3] = vvp_net_ptr_t(dst, 0);
	    force_link_->پورټ[2] = vvp_net_ptr_t(0,0);
	    force_link_->fun = جديد vvp_fun_force;
      }

      force_unlink();
      أكد(force_link_->پورټ[2] == vvp_net_ptr_t(0,0));

	// Use port[2] to hold the force source.
      force_link_->پورټ[2] = vvp_net_ptr_t(src,0);

      vvp_net_ptr_t dst_ptr(force_link_, 0);
      src->link(dst_ptr);
}

فراغ vvp_net_fil_t::چلوونکی ړنګول(فراغ*)
{
      أكد(0);
}

فراغ vvp_net_fil_t::force_unlink(فراغ)
{
      إذا (force_link_ == 0) أرجع;
      vvp_net_t*src = force_link_->پورټ[2].ptr();
      إذا (src == 0) أرجع;

      src->unlink(vvp_net_ptr_t(force_link_,0));
      force_link_->پورټ[2] = vvp_net_ptr_t(0,0);
}

/* *** BIT operations *** */
vvp_bit4_t جمع_کړه_with_لېږد(vvp_bit4_t a, vvp_bit4_t b, vvp_bit4_t&c)
{
      إذا (bit4_is_xz(a) || bit4_is_xz(b) || bit4_is_xz(c)) {
	    c = BIT4_X;
	    أرجع BIT4_X;
      }

	// NOTE: This relies on the facts that XZ values have been
	// weeded out, and that BIT4_1 is 1 and BIT4_0 is 0.
      صحيح مجموعه = (صحيح)a + (صحيح)b + (صحيح)c;

      بدل (مجموعه) {
	  حالة 0:
	      // c must already be 0.
	    أرجع BIT4_0;
	  حالة 1:
	    c = BIT4_0;
	    أرجع BIT4_1;
	  حالة 2:
	    c = BIT4_1;
	    أرجع BIT4_0;
	  حالة 3:
	    c = BIT4_1;
	    أرجع BIT4_1;
      }
      fprintf(stderr, "Incorrect result %d.\n", مجموعه);
      أكد(0);
      أرجع BIT4_X;
}

vvp_bit4_t scalar_منزل_bit4(PLI_INT32 scalar)
{
      بدل(scalar) {
	  حالة vpi0:
	    أرجع BIT4_0;
	  حالة vpi1:
	    أرجع BIT4_1;
	  حالة vpiX:
	    أرجع BIT4_X;
	  حالة vpiZ:
	    أرجع BIT4_Z;
      }
      fprintf(stderr, "Unsupported scalar value %d.\n", (صحيح)scalar);
      أكد(0);
      أرجع BIT4_X;
}

vvp_bit4_t چلوونکی ^ (vvp_bit4_t a, vvp_bit4_t b)
{
      إذا (bit4_is_xz(a))
	    أرجع BIT4_X;
      إذا (bit4_is_xz(b))
	    أرجع BIT4_X;
      إذا (a == BIT4_0)
	    أرجع b;
      إذا (b == BIT4_0)
	    أرجع a;
      أرجع BIT4_0;
}

ostream& چلوونکی<<(ostream&out, vvp_bit4_t بټ)
{
      بدل (بټ) {
	  حالة BIT4_0:
	    out << "0";
	    اكسر;
	  حالة BIT4_1:
	    out << "1";
	    اكسر;
	  حالة BIT4_X:
	    out << "X";
	    اكسر;
	  حالة BIT4_Z:
	    out << "Z";
	    اكسر;
	  افتراضي:
	    out << "?";
	    اكسر;
      }
      أرجع out;
}

تعريفنوع بینښه لنډ څنډه_t;

مضمن څنډه_t VVP_څنډه(vvp_bit4_t from, vvp_bit4_t to)
{
      أرجع 1 << ((from << 2) | to);
}

ثابت څنډه_t vvp_څنډه_posedge
      = VVP_څنډه(BIT4_0,BIT4_1)
      | VVP_څنډه(BIT4_0,BIT4_X)
      | VVP_څنډه(BIT4_0,BIT4_Z)
      | VVP_څنډه(BIT4_X,BIT4_1)
      | VVP_څنډه(BIT4_Z,BIT4_1)
      ;

ثابت څنډه_t vvp_څنډه_negedge
      = VVP_څنډه(BIT4_1,BIT4_0)
      | VVP_څنډه(BIT4_1,BIT4_X)
      | VVP_څنډه(BIT4_1,BIT4_Z)
      | VVP_څنډه(BIT4_X,BIT4_0)
      | VVP_څنډه(BIT4_Z,BIT4_0)
      ;

صحيح څنډه(vvp_bit4_t from, vvp_bit4_t to)
{
      څنډه_t ماسک = VVP_څنډه(from, to);
      إذا (ماسک & vvp_څنډه_posedge)
	    أرجع 1;
      إذا (ماسک & vvp_څنډه_negedge)
	    أرجع -1;
      أرجع 0;
}

بینښه اوږد multiply_with_لېږد(بینښه اوږد a, بینښه اوږد b,
				  بینښه اوږد&لېږد)
{
      ثابت بینښه اوږد ماسک = (1UL << (CPU_کلمه_بټونه/2)) - 1;
      بینښه اوږد a0 = a & ماسک;
      بینښه اوږد a1 = (a >> (CPU_کلمه_بټونه/2)) & ماسک;
      بینښه اوږد b0 = b & ماسک;
      بینښه اوږد b1 = (b >> (CPU_کلمه_بټونه/2)) & ماسک;

      بینښه اوږد tmp = a0 * b0;

      بینښه اوږد r00 = tmp & ماسک;
      بینښه اوږد c00 = (tmp >> (CPU_کلمه_بټونه/2)) & ماسک;

      tmp = a0 * b1;

      بینښه اوږد r01 = tmp & ماسک;
      بینښه اوږد c01 = (tmp >> (CPU_کلمه_بټونه/2)) & ماسک;

      tmp = a1 * b0;

      بینښه اوږد r10 = tmp & ماسک;
      بینښه اوږد c10 = (tmp >> (CPU_کلمه_بټونه/2)) & ماسک;

      tmp = a1 * b1;

      بینښه اوږد r11 = tmp & ماسک;
      بینښه اوږد c11 = (tmp >> (CPU_کلمه_بټونه/2)) & ماسک;

      بینښه اوږد r1 = c00 + r01 + r10;
      بینښه اوږد r2 = (r1 >> (CPU_کلمه_بټونه/2)) & ماسک;
      r1 &= ماسک;
      r2 += c01 + c10 + r11;
      بینښه اوږد r3 = (r2 >> (CPU_کلمه_بټونه/2)) & ماسک;
      r2 &= ماسک;
      r3 += c11;
      r3 &= ماسک;

      لېږد = (r3 << (CPU_کلمه_بټونه/2)) + r2;
      أرجع (r1 << (CPU_کلمه_بټونه/2)) + r00;
}


فراغ vvp_send_vec8(vvp_net_ptr_t ptr, ثابت vvp_vector8_t&val)
{
      طالما (vvp_net_t*cur = ptr.ptr()) {
	    vvp_net_ptr_t بل_val = cur->پورټ[ptr.پورټ()];

	    إذا (cur->fun)
		  cur->fun->recv_vec8(ptr, val);

	    ptr = بل_val;
      }
}

فراغ vvp_send_real(vvp_net_ptr_t ptr, مزدوج val, vvp_context_t context)
{
      طالما (vvp_net_t*cur = ptr.ptr()) {
	    vvp_net_ptr_t بل_val = cur->پورټ[ptr.پورټ()];

	    إذا (cur->fun)
		  cur->fun->recv_real(ptr, val, context);

	    ptr = بل_val;
      }
}

ثابت vvp_vector4_t vvp_vector4_t::nil;

فراغ vvp_vector4_t::copy_بټونه(ثابت vvp_vector4_t&that)
{

      إذا (اندازه_2 == that.اندازه_2) {
	    إذا (اندازه_2 > بټونه_فی_کلمه) {
		  بینښه ټکي = (اندازه_2+بټونه_فی_کلمه-1) / بټونه_فی_کلمه;
		  لكل (بینښه idx = 0 ;  idx < ټکي ;  idx += 1)
			abits_ptr_[idx] = that.abits_ptr_[idx];
		  لكل (بینښه idx = 0 ;  idx < ټکي ;  idx += 1)
			bbits_ptr_[idx] = that.bbits_ptr_[idx];
	    } وإلا {
		  abits_val_ = that.abits_val_;
		  bbits_val_ = that.bbits_val_;
	    }
	    أرجع;
      }

	/* Now we know that the sizes of this and that are definitely
	   different. We can use that in code below. In any case, we
	   need to copy only the smaller of the sizes. */

	/* If source and destination are both short, then mask/copy
	   the bit values. */
      إذا (اندازه_2 <= بټونه_فی_کلمه && that.اندازه_2 <= بټونه_فی_کلمه) {
	    بینښه بټونه_منزل_copy = (that.اندازه_2 < اندازه_2) ? that.اندازه_2 : اندازه_2;
	    بینښه اوږد ماسک = ((بټونه_منزل_copy < بټونه_فی_کلمه) ? (1UL << بټونه_منزل_copy) :
	                                                           (0UL)) - 1UL;
	    abits_val_ &= ~ماسک;
	    bbits_val_ &= ~ماسک;
	    abits_val_ |= that.abits_val_&ماسک;
	    bbits_val_ |= that.bbits_val_&ماسک;
	    أرجع;
      }

	/* Now we know that either source or destination are long. If
	   the destination is short, then mask/copy from the low word
	   of the long source. */
      إذا (اندازه_2 <= بټونه_فی_کلمه) {
	    abits_val_ = that.abits_ptr_[0];
	    bbits_val_ = that.bbits_ptr_[0];
	    إذا (اندازه_2 < بټونه_فی_کلمه) {
		  بینښه اوږد ماسک = (1UL << اندازه_2) - 1UL;
		  abits_val_ &= ماسک;
		  bbits_val_ &= ماسک;
	    }
	    أرجع;
      }

	/* Now we know that the destination must be long. If the
	   source is short, then mask/copy from its value. */
      إذا (that.اندازه_2 <= بټونه_فی_کلمه) {
	    بینښه اوږد ماسک;
	    إذا (that.اندازه_2 < بټونه_فی_کلمه) {
		  ماسک = (1UL << that.اندازه_2) - 1UL;
		  abits_ptr_[0] &= ~ماسک;
		  bbits_ptr_[0] &= ~ماسک;
	    } وإلا {
		  ماسک = -1UL;
	    }
	    abits_ptr_[0] |= that.abits_val_&ماسک;
	    bbits_ptr_[0] |= that.bbits_val_&ماسک;
	    أرجع;
      }

	/* Finally, we know that source and destination are long. copy
	   words until we get to the last. */
      بینښه بټونه_منزل_copy = (that.اندازه_2 < اندازه_2) ? that.اندازه_2 : اندازه_2;
      بینښه کلمه = 0;
      طالما (بټونه_منزل_copy >= بټونه_فی_کلمه) {
	    abits_ptr_[کلمه] = that.abits_ptr_[کلمه];
	    bbits_ptr_[کلمه] = that.bbits_ptr_[کلمه];
	    بټونه_منزل_copy -= بټونه_فی_کلمه;
	    کلمه += 1;
      }
      إذا (بټونه_منزل_copy > 0) {
	    بینښه اوږد ماسک = (1UL << بټونه_منزل_copy) - 1UL;
	    abits_ptr_[کلمه] &= ~ماسک;
	    bbits_ptr_[کلمه] &= ~ماسک;
	    abits_ptr_[کلمه] |= that.abits_ptr_[کلمه] & ماسک;
	    bbits_ptr_[کلمه] |= that.bbits_ptr_[کلمه] & ماسک;
      }
}

/*
 * This function should ONLY BE CALLED FROM vvp_vector4_t::copy_from_,
 * as it performs part of that functions tasks.
 */
فراغ vvp_vector4_t::copy_from_big_(ثابت vvp_vector4_t&that)
{
      بینښه ټکي = (اندازه_2+بټونه_فی_کلمه-1) / بټونه_فی_کلمه;
      abits_ptr_ = جديد بینښه اوږد[2*ټکي];
      bbits_ptr_ = abits_ptr_ + ټکي;

      لكل (بینښه idx = 0 ;  idx < ټکي ;  idx += 1)
	    abits_ptr_[idx] = that.abits_ptr_[idx];
      لكل (بینښه idx = 0 ;  idx < ټکي ;  idx += 1)
	    bbits_ptr_[idx] = that.bbits_ptr_[idx];
}

/*
 * The copy_inverted_from_ method is just like the copy_from_ method,
 * except that we combine that with an invert. This allows the ~ and
 * the assignment to be blended in many common cases.
 */
فراغ vvp_vector4_t::copy_inverted_from_(ثابت vvp_vector4_t&that)
{
      اندازه_2 = that.اندازه_2;
      إذا (اندازه_2 > بټونه_فی_کلمه) {
	    بینښه ټکي = (اندازه_2+بټونه_فی_کلمه-1) / بټونه_فی_کلمه;
	    abits_ptr_ = جديد بینښه اوږد[2*ټکي];
	    bbits_ptr_ = abits_ptr_ + ټکي;

	    بینښه پاتې_شوی = اندازه_2;
	    بینښه idx = 0;
	    طالما (پاتې_شوی >= بټونه_فی_کلمه) {
		  abits_ptr_[idx] = that.bbits_ptr_[idx] | ~that.abits_ptr_[idx];
		  idx += 1;
		  پاتې_شوی -= بټونه_فی_کلمه;
	    }
	    إذا (پاتې_شوی > 0) {
		  بینښه اوږد ماسک = (1UL<<پاتې_شوی) - 1UL;
		  abits_ptr_[idx] = ماسک & (that.bbits_ptr_[idx] | ~that.abits_ptr_[idx]);
	    }

	    لكل (idx = 0 ;  idx < ټکي ;  idx += 1)
		  bbits_ptr_[idx] = that.bbits_ptr_[idx];

      } وإلا {
	    بینښه اوږد ماسک = (اندازه_2<بټونه_فی_کلمه)? (1UL<<اندازه_2)-1UL : -1UL;
	    abits_val_ = ماسک & (that.bbits_val_ | ~that.abits_val_);
	    bbits_val_ = that.bbits_val_;
      }
}

/* Make sure to set size_ before calling this routine. */
فراغ vvp_vector4_t::allocate_ټکي(بینښه اوږد inita, بینښه اوږد initb)
{
      إذا (اندازه_2 > بټونه_فی_کلمه) {
	    بینښه cnt = (اندازه_2 + بټونه_فی_کلمه - 1) / بټونه_فی_کلمه;
	    abits_ptr_ = جديد بینښه اوږد[2*cnt];
	    bbits_ptr_ = abits_ptr_ + cnt;
	    لكل (بینښه idx = 0 ;  idx < cnt ;  idx += 1)
		  abits_ptr_[idx] = inita;
	    لكل (بینښه idx = 0 ;  idx < cnt ;  idx += 1)
		  bbits_ptr_[idx] = initb;

      } وإلا {
	    abits_val_ = inita;
	    bbits_val_ = initb;
      }
}

vvp_vector4_t::vvp_vector4_t(بینښه اندازه_3, مزدوج val)
: اندازه_2(اندازه_3)
{
      منطقي is_neg = خطأ;
      مزدوج fraction;
      صحيح exponent;

	/* We return 'bx for a NaN or +/- infinity. */
      إذا (val != val || (val && (val == 0.5*val)))  {
	    allocate_ټکي(کلمه_X_ABITS, کلمه_X_BBITS);
	    أرجع;
      }

	/* Convert to a positive result. */
      إذا (val < 0.0) {
	    is_neg = ریښتیا;
	    val = -val;
      }
      allocate_ټکي(کلمه_0_ABITS, کلمه_0_BBITS);

	/* Get the exponent and fractional part of the number. */
      fraction = frexp(val, &exponent);

	/* If the value is small enough just use lround(). */
      إذا (exponent < بټونه_فی_کلمه-2) {
	    إذا (is_neg) هذا->invert();  // Invert the bits if negative.
	    اوږد sval = lround(val);
	    إذا (is_neg) sval = -sval;
	      /* This requires that 0 and 1 have the same bbit value. */
	    إذا (اندازه_2 > بټونه_فی_کلمه) {
		  abits_ptr_[0] = sval;
	    } وإلا {
		  abits_val_ = sval;
	    }
	    أرجع;
      }

      بینښه nwords = (exponent-1) / بټونه_فی_کلمه;
      بینښه my_ټکي = (اندازه_2 + بټونه_فی_کلمه - 1) / بټونه_فی_کلمه - 1;

      fraction = ldexp(fraction, (exponent-1) % بټونه_فی_کلمه + 1);

	/* Skip any leading bits. */
      لكل (صحيح idx = (نښهدار) nwords; idx > (نښهدار) my_ټکي; idx -=1) {
	    بینښه اوږد بټونه = (بینښه اوږد) fraction;
	    fraction = fraction - (مزدوج) بټونه;
	    fraction = ldexp(fraction, بټونه_فی_کلمه);
      }

	/* Convert the remaining bits as appropriate. */
      إذا (my_ټکي == 0) {
		  بینښه اوږد بټونه = (بینښه اوږد) fraction;
		  abits_val_ = بټونه;
		  fraction = fraction - (مزدوج) بټونه;
		    /* Round any fractional part up. */
		  إذا (fraction >= 0.5) *هذا += (int64_t) 1;
      } وإلا {
	    إذا (nwords < my_ټکي) my_ټکي = nwords;
	    لكل (صحيح idx = (نښهدار)my_ټکي; idx >= 0; idx -= 1) {
		  بینښه اوږد بټونه = (بینښه اوږد) fraction;
		  abits_ptr_[idx] = بټونه;
		  fraction = fraction - (مزدوج) بټونه;
		  fraction = ldexp(fraction, بټونه_فی_کلمه);
	    }
	      /* Round any fractional part up. */
	    إذا (fraction >= ldexp(0.5, بټونه_فی_کلمه)) *هذا += (int64_t) 1;
       }

	/* Convert to a negative number if needed. */
      إذا (is_neg) {
	    هذا->invert();
	    *هذا += (int64_t) 1;
      }
}

vvp_vector4_t::vvp_vector4_t(ثابت vvp_vector4_t&that,
			    بینښه adr, بینښه wid)
{
	// Set up and initialize the destination.
      اندازه_2 = wid;
      allocate_ټکي(کلمه_X_ABITS, کلمه_X_BBITS);

	// Special case: selecting from far beyond the source vector,
	// to the result is all X bits. We're done.
      إذا (adr >= that.اندازه_2)
	    أرجع;

	// Special case: The source is not quite big enough to supply
	// all bits, so get the bits that we can. The remainder will
	// be left at BIT4_X.
      إذا ((adr + wid) > that.اندازه_2) {
	    بینښه use_wid = that.اندازه_2 - adr;
	    لكل (بینښه idx = 0 ; idx < use_wid ; idx += 1)
		  سېټ_بټ(idx, that.value(adr+idx));

	    أرجع;
      }

	// At the point, we know that the source part is entirely
	// contained in the source vector.
	// assert((adr + wid) <= that.size_);

      إذا (wid > بټونه_فی_کلمه) {
	      /* In this case, the subvector and the source vector are
		 long. Do the transfer reasonably efficiently. */
	    بینښه ptr = adr / بټونه_فی_کلمه;
	    بینښه اوږد off = adr % بټونه_فی_کلمه;
	    بینښه اوږد noff = بټونه_فی_کلمه - off;
	    بینښه اوږد lmask = (1UL << off) - 1UL;
	    بینښه trans = 0;
	    بینښه dst = 0;
	    طالما (trans < wid) {
		    // The low bits of the result.
		  abits_ptr_[dst] = (that.abits_ptr_[ptr] & ~lmask) >> off;
		  bbits_ptr_[dst] = (that.bbits_ptr_[ptr] & ~lmask) >> off;
		  trans += noff;

		  إذا (trans >= wid)
			اكسر;

		  ptr += 1;

		    // The high bits of the result. Skip this if the
		    // source and destination are perfectly aligned.
		  إذا (noff != بټونه_فی_کلمه) {
			abits_ptr_[dst] |= (that.abits_ptr_[ptr]&lmask) << noff;
			bbits_ptr_[dst] |= (that.bbits_ptr_[ptr]&lmask) << noff;
			trans += off;
		  }

		  dst += 1;
	    }

      } وإلا إذا (that.اندازه_2 > بټونه_فی_کلمه) {
	      /* In this case, the subvector fits in a single word,
		 but the source is large. */
	    بینښه ptr = adr / بټونه_فی_کلمه;
	    بینښه اوږد off = adr % بټونه_فی_کلمه;
	    بینښه trans = بټونه_فی_کلمه - off;
	    إذا (trans > wid)
		  trans = wid;

	    إذا (trans == بټونه_فی_کلمه) {
		    // Very special case: Copy exactly 1 perfectly
		    // aligned word.
		  abits_val_ = that.abits_ptr_[ptr];
		  bbits_val_ = that.bbits_ptr_[ptr];

	    } وإلا {
		    // lmask is the low bits of the destination,
		    // masked into the source.
		  بینښه اوږد lmask = (1UL<<trans) - 1UL;
		  lmask <<= off;

		    // The low bits of the result.
		  abits_val_ = (that.abits_ptr_[ptr] & lmask) >> off;
		  bbits_val_ = (that.bbits_ptr_[ptr] & lmask) >> off;

		  إذا (trans < wid) {
			  // If there are more bits, then get them
			  // from the bottom of the next word of the
			  // source.
			بینښه اوږد hmask = (1UL << (wid-trans)) - 1UL;

			  // The high bits of the result.
			abits_val_ |= (that.abits_ptr_[ptr+1]&hmask) << trans;
			bbits_val_ |= (that.bbits_ptr_[ptr+1]&hmask) << trans;
		  }
	    }

      } وإلا إذا (اندازه_2 == بټونه_فی_کلمه) {
	      /* We know that source and destination are short. If the
		 destination is a full word, then we know the copy is
		 aligned and complete. */
	    abits_val_ = that.abits_val_;
	    bbits_val_ = that.bbits_val_;

      } وإلا {
	      /* Finally, the source and destination vectors are both
		 short, so there is a single mask/shift/copy. */
	    بینښه اوږد ماسک = (1UL << اندازه_2) - 1UL;
	    ماسک <<= adr;

	    abits_val_ = (that.abits_val_ & ماسک) >> adr;
	    bbits_val_ = (that.bbits_val_ & ماسک) >> adr;
      }

}

/*
 * Change the size of the vvp_vector4_t vector to the new size. Copy
 * the old values, as many as well fit, into the new vector.
 */
فراغ vvp_vector4_t::resize(بینښه newsize, vvp_bit4_t pad_بټ)
{
      إذا (اندازه_2 == newsize)
	    أرجع;

      بینښه cnt = (اندازه_2 + بټونه_فی_کلمه - 1) / بټونه_فی_کلمه;
      بینښه اوږد کلمه_pad_abits = 0;
      بینښه اوږد کلمه_pad_bbits = 0;
      بدل (pad_بټ) {
	  حالة BIT4_0:
	    کلمه_pad_abits = کلمه_0_ABITS;
	    کلمه_pad_bbits = کلمه_0_BBITS;
	    اكسر;
	  حالة BIT4_1:
	    کلمه_pad_abits = کلمه_1_ABITS;
	    کلمه_pad_bbits = کلمه_1_BBITS;
	    اكسر;
	  حالة BIT4_X:
	    کلمه_pad_abits = کلمه_X_ABITS;
	    کلمه_pad_bbits = کلمه_X_BBITS;
	    اكسر;
	  حالة BIT4_Z:
	    کلمه_pad_abits = کلمه_Z_ABITS;
	    کلمه_pad_bbits = کلمه_Z_BBITS;
	    اكسر;
      }

      إذا (newsize > بټونه_فی_کلمه) {
	    بینښه newcnt = (newsize + بټونه_فی_کلمه - 1) / بټونه_فی_کلمه;
	    إذا (newcnt == cnt) {
		    // If the word count doesn't change, then there is
		    // no need for re-allocation so we are done now.
		  إذا (newsize > اندازه_2) {
			إذا (بینښه fill = اندازه_2 % بټونه_فی_کلمه) {
			      abits_ptr_[cnt-1] &= ~((-1UL) << fill);
			      bbits_ptr_[cnt-1] &= ~((-1UL) << fill);
			      abits_ptr_[cnt-1] |= کلمه_pad_abits << fill;
			      bbits_ptr_[cnt-1] |= کلمه_pad_bbits << fill;
			}
		  }
		  اندازه_2 = newsize;
		  أرجع;
	    }

	    بینښه اوږد*newbits = جديد بینښه اوږد[2*newcnt];

	    إذا (cnt > 1) {
		  بینښه trans = cnt;
		  إذا (trans > newcnt)
			trans = newcnt;

		  لكل (بینښه idx = 0 ;  idx < trans ;  idx += 1)
			newbits[idx] = abits_ptr_[idx];
		  لكل (بینښه idx = 0 ;  idx < trans ;  idx += 1)
			newbits[newcnt+idx] = bbits_ptr_[idx];

		  ړنګول[]abits_ptr_;

	    } وإلا {
		  newbits[0] = abits_val_;
		  newbits[newcnt] = bbits_val_;
	    }

	    إذا (newsize > اندازه_2) {
		  إذا (بینښه fill = اندازه_2 % بټونه_فی_کلمه) {
			newbits[cnt-1] &= ~((-1UL) << fill);
			newbits[cnt-1] |= کلمه_pad_abits << fill;
			newbits[newcnt+cnt-1] &= ~((-1UL) << fill);
			newbits[newcnt+cnt-1] |= کلمه_pad_bbits << fill;
		  }
		  لكل (بینښه idx = cnt ;  idx < newcnt ;  idx += 1)
			newbits[idx] = کلمه_pad_abits;
		  لكل (بینښه idx = cnt ;  idx < newcnt ;  idx += 1)
			newbits[newcnt+idx] = کلمه_pad_bbits;
	    }

	    اندازه_2 = newsize;
	    abits_ptr_ = newbits;
	    bbits_ptr_ = newbits + newcnt;

      } وإلا {
	    إذا (cnt > 1) {
		  بینښه اوږد newvala = abits_ptr_[0];
		  بینښه اوږد newvalb = bbits_ptr_[0];
		  ړنګول[]abits_ptr_;
		  abits_val_ = newvala;
		  bbits_val_ = newvalb;
	    }

	    إذا (newsize > اندازه_2) {
		  abits_val_ &= ~((-1UL) << اندازه_2);
		  bbits_val_ &= ~((-1UL) << اندازه_2);
		  abits_val_ |= کلمه_pad_abits << اندازه_2;
		  bbits_val_ |= کلمه_pad_bbits << اندازه_2;
	    }

	    اندازه_2 = newsize;
      }
}


بینښه اوږد* vvp_vector4_t::subarray(بینښه adr, بینښه wid, منطقي xz_منزل_0) ثابت
{
      ثابت بینښه BIT2_فی_کلمه = 8*حجم(بینښه اوږد);
      بینښه awid = (wid + BIT2_فی_کلمه - 1) / (BIT2_فی_کلمه);
      بینښه اوږد*val = جديد بینښه اوږد[awid];

      لكل (بینښه idx = 0 ;  idx < awid ;  idx += 1)
	    val[idx] = 0;

      إذا (اندازه_2 <= بټونه_فی_کلمه) {
	      /* Handle the special case that the array is small. The
		 entire value of the vector4 is within the xbits_val_
		 so we know that the result is a single word, the
		 source is a single word, and we just have to loop
		 through that word. */
	    بینښه اوږد atmp = abits_val_ >> adr;
	    بینښه اوږد btmp = bbits_val_ >> adr;
	    إذا (wid < BIT2_فی_کلمه) {
		  atmp &= (1UL << wid) - 1;
		  btmp &= (1UL << wid) - 1;
	    }
	    إذا (btmp) {
		  إذا (xz_منزل_0)
			atmp &= ~btmp;
		  وإلا
			اذهب x_بهر;
	    }

	    val[0] = atmp;

      } وإلا {

	    بینښه val_ptr = 0;
	    بینښه val_off = 0;

	      /* Get the first word we are scanning. We may in fact be
		 somewhere in the middle of that word. */
	    طالما (wid > 0) {
		  بینښه اوږد atmp = abits_ptr_[adr/بټونه_فی_کلمه];
		  بینښه اوږد btmp = bbits_ptr_[adr/بټونه_فی_کلمه];
		  بینښه اوږد off = adr%بټونه_فی_کلمه;
		  atmp >>= off;
		  btmp >>= off;

		  بینښه اوږد trans = بټونه_فی_کلمه - off;
		  إذا (trans > (BIT2_فی_کلمه - val_off))
			trans = BIT2_فی_کلمه - val_off;
		  إذا (wid < trans)
			trans = wid;
		  إذا (trans < BIT2_فی_کلمه) {
			atmp &= (1UL << trans) - 1;
			btmp &= (1UL << trans) - 1;
		  }
		  إذا (btmp) {
			إذا (xz_منزل_0)
			      atmp &= ~btmp;
			وإلا
			      اذهب x_بهر;
		  }

		  val[val_ptr] |= atmp << val_off;
		  adr += trans;
		  wid -= trans;
		  val_off += trans;
		  إذا (val_off == BIT2_فی_کلمه) {
			val_ptr += 1;
			val_off = 0;
		  }
	    }
      }

      أرجع val;

 x_بهر:
      ړنګول[]val;
      أرجع 0;
}

فراغ vvp_vector4_t::setarray(بینښه adr, بینښه wid, ثابت بینښه اوږد*val)
{
      أكد(adr+wid <= اندازه_2);

      ثابت بینښه BIT2_فی_کلمه = 8*حجم(بینښه اوږد);

      إذا (اندازه_2 <= بټونه_فی_کلمه) {
	      // We know here that both the source and the target are
	      // within a single word. Write the bits into the
	      // abits_val_ directly.

	    أكد(BIT2_فی_کلمه <= بټونه_فی_کلمه);
	    بینښه اوږد lmask = (1UL << adr) - 1UL;
	    بینښه اوږد hmask = ((adr+wid) < بټونه_فی_کلمه)
		  ? -1UL << (adr+wid)
		  : 0;
	    بینښه اوږد ماسک = ~(hmask | lmask);

	    abits_val_ &= ~ماسک;
	    bbits_val_ &= ~ماسک;

	    abits_val_ |= ماسک & (val[0] << adr);

      } وإلا {
	      // The general case, there are multiple words of
	      // destination, and possibly multiple words of source
	      // data. Shift and mask as we go.
	    بینښه off = adr % بټونه_فی_کلمه;
	    بینښه ptr = adr / بټونه_فی_کلمه;
	    بینښه val_off = 0;
	    بینښه val_ptr = 0;
	    طالما (wid > 0) {
		  بینښه trans = wid;
		  إذا (trans > (BIT2_فی_کلمه-val_off))
			trans = BIT2_فی_کلمه-val_off;
		  إذا (trans > (بټونه_فی_کلمه-off))
			trans = بټونه_فی_کلمه-off;

		  بینښه اوږد lmask = (1UL << off) - 1UL;
		  بینښه اوږد hmask = ((off+trans) < بټونه_فی_کلمه)
			? -1UL << (off+trans)
			: 0;
		  بینښه اوږد ماسک = ~(hmask | lmask);

		  abits_ptr_[ptr] &= ~ماسک;
		  bbits_ptr_[ptr] &= ~ماسک;
		  إذا (val_off >= off)
			abits_ptr_[ptr] |= ماسک & (val[val_ptr] >> (val_off-off));
		  وإلا
			abits_ptr_[ptr] |= ماسک & (val[val_ptr] << (off-val_off));

		  wid -= trans;
		  val_off += trans;
		  إذا (val_off == BIT2_فی_کلمه) {
			val_off = 0;
			val_ptr += 1;
		  }
		  off += trans;
		  إذا (off == بټونه_فی_کلمه) {
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
منطقي vvp_vector4_t::سېټ_vec(بینښه adr, ثابت vvp_vector4_t&that)
{
      أكد(adr+that.اندازه_2  <= اندازه_2);
      منطقي diff_flag = خطأ;

      إذا (اندازه_2 <= بټونه_فی_کلمه) {

	      /* The destination vector (me!) is within a bits_val_
		 word, so the subvector is certainly within a
		 bits_val_ word. Therefore, the entire operation is a
		 matter of writing the bits of that into the addressed
		 bits of this. The mask below is calculated to be 1
		 for all the bits that are to come from that. Do the
		 job by some shifting, masking and OR. */

	    بینښه اوږد lmask = (1UL << adr) - 1;
	    بینښه اوږد hmask;
	    بینښه اوږد hshift = adr+that.اندازه_2;
	    إذا (hshift >= بټونه_فی_کلمه)
		  hmask = -1UL;
	    وإلا
		  hmask = (1UL << (adr+that.اندازه_2)) - 1;
	    بینښه اوږد ماسک = hmask & ~lmask;

	    بینښه اوږد tmp = (that.abits_val_<<adr)&ماسک;
	    إذا ((abits_val_&ماسک) != tmp) {
		  diff_flag = ریښتیا;
		  abits_val_ = (abits_val_ & ~ماسک) | tmp;
	    }
	    tmp = (that.bbits_val_<<adr) & ماسک;
	    إذا ((bbits_val_&ماسک) != tmp) {
		  diff_flag = ریښتیا;
		  bbits_val_ = (bbits_val_ & ~ماسک) | tmp;
	    }

      } وإلا إذا (that.اندازه_2 <= بټونه_فی_کلمه) {

	      /* This vector is more than a word, but that vector is
		 still small. Write into the destination, possibly
		 spanning two destination words, depending on whether
		 the source vector spans a word transition. */
	    بینښه اوږد dptr = adr / بټونه_فی_کلمه;
	    بینښه اوږد doff = adr % بټونه_فی_کلمه;

	    بینښه اوږد lmask = (1UL << doff) - 1;
	    بینښه اوږد hshift = doff+that.اندازه_2;
	    بینښه اوږد hmask;
	    إذا (hshift >= بټونه_فی_کلمه)
		  hmask = -1UL;
	    وإلا
		  hmask = (1UL << hshift) - 1UL;

	    بینښه اوږد ماسک = hmask & ~lmask;
	    بینښه اوږد tmp;

	    tmp = (that.abits_val_ << doff) & ماسک;
	    إذا ((abits_ptr_[dptr] & ماسک) != tmp) {
		  diff_flag = ریښتیا;
		  abits_ptr_[dptr] = (abits_ptr_[dptr] & ~ماسک) | tmp;
	    }
	    tmp = (that.bbits_val_ << doff) & ماسک;
	    إذا ((bbits_ptr_[dptr] & ماسک) != tmp) {
		  diff_flag = ریښتیا;
		  bbits_ptr_[dptr] = (bbits_ptr_[dptr] & ~ماسک) | tmp;
	    }

	    إذا ((doff + that.اندازه_2) > بټونه_فی_کلمه) {
		  بینښه لکۍ = doff + that.اندازه_2 - بټونه_فی_کلمه;
		  ماسک = (1UL << لکۍ) - 1;

		  dptr += 1;
		  tmp = (that.abits_val_ >> (that.اندازه_2-لکۍ)) & ماسک;
		  إذا ((abits_ptr_[dptr] & ماسک) != tmp) {
			diff_flag = ریښتیا;
			abits_ptr_[dptr] = (abits_ptr_[dptr] & ~ماسک) | tmp;
		  }
		  tmp = (that.bbits_val_ >> (that.اندازه_2-لکۍ)) & ماسک;
		  إذا ((bbits_ptr_[dptr] & ماسک) != tmp) {
			diff_flag = ریښتیا;
			bbits_ptr_[dptr] = (bbits_ptr_[dptr] & ~ماسک) | tmp;
		  }
	    }

      } وإلا إذا (adr%بټونه_فی_کلمه == 0) {

	      /* In this case, both vectors are long, but the
		 destination is neatly aligned. That means all but the
		 last word can be simply copied with no masking. */

	    بینښه remain = that.اندازه_2;
	    بینښه sptr = 0;
	    بینښه dptr = adr / بټونه_فی_کلمه;
	    طالما (remain >= بټونه_فی_کلمه) {
		  إذا (abits_ptr_[dptr] != that.abits_ptr_[sptr]) {
			diff_flag = ریښتیا;
			abits_ptr_[dptr] = that.abits_ptr_[sptr];
		  }
		  إذا (bbits_ptr_[dptr] != that.bbits_ptr_[sptr]) {
			diff_flag = ریښتیا;
			bbits_ptr_[dptr] = that.bbits_ptr_[sptr];
		  }
		  dptr += 1;
		  sptr += 1;
		  remain -= بټونه_فی_کلمه;
	    }

	    إذا (remain > 0) {
		  بینښه اوږد ماسک = (1UL << remain) - 1;
		  بینښه اوږد tmp;

		  tmp = that.abits_ptr_[sptr] & ماسک;
		  إذا ((abits_ptr_[dptr] & ماسک) != tmp) {
			diff_flag = ریښتیا;
			abits_ptr_[dptr] = (abits_ptr_[dptr] & ~ماسک) | tmp;
		  }
		  tmp = that.bbits_ptr_[sptr] & ماسک;
		  إذا ((bbits_ptr_[dptr] & ماسک) != tmp) {
			diff_flag = ریښتیا;
			bbits_ptr_[dptr] = (bbits_ptr_[dptr] & ~ماسک) | tmp;
		  }
	    }

      } وإلا {

	      /* We know that there are two long vectors, and we know
		 that the destination is definitely NOT aligned. */

	    بینښه remain = that.اندازه_2;
	    بینښه sptr = 0;
	    بینښه dptr = adr / بټونه_فی_کلمه;
	    بینښه doff = adr % بټونه_فی_کلمه;
	    بینښه اوږد lmask = (1UL << doff) - 1;
	    بینښه ndoff = بټونه_فی_کلمه - doff;
	    طالما (remain >= بټونه_فی_کلمه) {
		  بینښه اوږد tmp;

		  tmp = (that.abits_ptr_[sptr] << doff) & ~lmask;
		  إذا ((abits_ptr_[dptr] & ~lmask) != tmp) {
			diff_flag = ریښتیا;
			abits_ptr_[dptr] = (abits_ptr_[dptr] & lmask) | tmp;
		  }
		  tmp = (that.bbits_ptr_[sptr] << doff) & ~lmask;
		  إذا ((bbits_ptr_[dptr] & ~lmask) != tmp) {
			diff_flag = ریښتیا;
			bbits_ptr_[dptr] = (bbits_ptr_[dptr] & lmask) | tmp;
		  }
		  dptr += 1;

		  tmp = (that.abits_ptr_[sptr] >> ndoff) & lmask;
		  إذا ((abits_ptr_[dptr] & lmask) != tmp) {
			diff_flag = ریښتیا;
			abits_ptr_[dptr] = (abits_ptr_[dptr] & ~lmask) | tmp;
		  }
		  tmp = (that.bbits_ptr_[sptr] >> ndoff) & lmask;
		  إذا ((bbits_ptr_[dptr] & lmask) != tmp) {
			diff_flag = ریښتیا;
			bbits_ptr_[dptr] = (bbits_ptr_[dptr] & ~lmask) | tmp;
		  }

		  remain -= بټونه_فی_کلمه;
		  sptr += 1;
	    }

	    إذا (remain > 0) {
		  بینښه اوږد hshift = doff+remain;
		  بینښه اوږد hmask;
		  إذا (hshift >= بټونه_فی_کلمه)
			hmask = -1UL;
		  وإلا
			hmask = (1UL << (doff+remain)) - 1;

		  بینښه اوږد ماسک = hmask & ~lmask;
		  بینښه اوږد tmp;

		  tmp = (that.abits_ptr_[sptr] << doff) & ماسک;
		  إذا ((abits_ptr_[dptr] & ماسک) != tmp) {
			diff_flag = ریښتیا;
			abits_ptr_[dptr] = (abits_ptr_[dptr] & ~ماسک) | tmp;
		  }
		  tmp = (that.bbits_ptr_[sptr] << doff) & ماسک;
		  إذا ((bbits_ptr_[dptr] & ماسک) != tmp) {
			diff_flag = ریښتیا;
			bbits_ptr_[dptr] = (bbits_ptr_[dptr] & ~ماسک) | tmp;
		  }

		  إذا ((doff + remain) > بټونه_فی_کلمه) {
			بینښه لکۍ = doff + remain - بټونه_فی_کلمه;
			إذا (لکۍ >= بټونه_فی_کلمه)
			      ماسک = -1UL;
			وإلا
			      ماسک = (1UL << لکۍ) - 1;

			dptr += 1;

			tmp = (that.abits_ptr_[sptr] >> (remain-لکۍ))&ماسک;
			إذا ((abits_ptr_[dptr] & ماسک) != tmp) {
			      diff_flag = ریښتیا;
			      abits_ptr_[dptr] = (abits_ptr_[dptr] & ~ماسک) | tmp;
			}
			tmp = (that.bbits_ptr_[sptr] >> (remain-لکۍ))&ماسک;
			إذا ((bbits_ptr_[dptr] & ماسک) != tmp) {
			      diff_flag = ریښتیا;
			      bbits_ptr_[dptr] = (bbits_ptr_[dptr] & ~ماسک) | tmp;
			}
		  }
	    }
      }

      أرجع diff_flag;
}

/*
 * Add that vector to this vector. Do it in the Verilog way, which
 * means if we detect any X or Z bits, change the entire results to
 * all X.
 *
 * Assume both vectors are the same size.
 */
فراغ vvp_vector4_t::جمع_کړه(ثابت vvp_vector4_t&that)
{
      أكد(اندازه_2 == that.اندازه_2);

      إذا (اندازه_2 < بټونه_فی_کلمه) {
	    بینښه اوږد ماسک = ~(-1UL << اندازه_2);
	    إذا ((bbits_val_|that.bbits_val_) & ماسک) {
		  abits_val_ |= ماسک;
		  bbits_val_ |= ماسک;
		  أرجع;
	    }

	    abits_val_ += that.abits_val_;
	    abits_val_ &= ماسک;
	    أرجع;
      }

      إذا (اندازه_2 == بټونه_فی_کلمه) {
	    إذا (bbits_val_ | that.bbits_val_) {
		  abits_val_ = کلمه_X_ABITS;
		  bbits_val_ = کلمه_X_BBITS;
	    } وإلا {
		  abits_val_ += that.abits_val_;
	    }
	    أرجع;
      }

      صحيح cnt = اندازه_2 / بټونه_فی_کلمه;
      بینښه اوږد لېږد = 0;
      لكل (صحيح idx = 0 ; idx < cnt ; idx += 1) {
	    إذا (bbits_ptr_[idx] | that.bbits_ptr_[idx])
		  اذهب x_بهر;

	    abits_ptr_[idx] = جمع_کړه_with_لېږد(abits_ptr_[idx], that.abits_ptr_[idx], لېږد);
      }

      إذا (بینښه لکۍ = اندازه_2 % بټونه_فی_کلمه) {
	    بینښه اوږد ماسک = ~( -1UL << لکۍ );
	    إذا ((bbits_ptr_[cnt] | that.bbits_ptr_[cnt])&ماسک)
		  اذهب x_بهر;

	    abits_ptr_[cnt] = جمع_کړه_with_لېږد(abits_ptr_[cnt], that.abits_ptr_[cnt], لېږد);
	    abits_ptr_[cnt] &= ماسک;
      }

      أرجع;

 x_بهر:
      لكل (صحيح idx = 0 ; idx < cnt ; idx += 1) {
	    abits_ptr_[idx] = کلمه_X_ABITS;
	    bbits_ptr_[idx] = کلمه_X_BBITS;
      }
      إذا (بینښه لکۍ = اندازه_2%بټونه_فی_کلمه) {
	    بینښه اوږد ماسک = ~( -1UL << لکۍ );
	    abits_ptr_[cnt] = کلمه_X_ABITS&ماسک;
	    bbits_ptr_[cnt] = کلمه_X_BBITS&ماسک;
      }
}

فراغ vvp_vector4_t::sub(ثابت vvp_vector4_t&that)
{
      أكد(اندازه_2 == that.اندازه_2);

      إذا (اندازه_2 < بټونه_فی_کلمه) {
	    بینښه اوږد ماسک = ~(-1UL << اندازه_2);
	    إذا ((bbits_val_|that.bbits_val_) & ماسک) {
		  abits_val_ |= ماسک;
		  bbits_val_ |= ماسک;
		  أرجع;
	    }

	    abits_val_ -= that.abits_val_;
	    abits_val_ &= ماسک;
	    أرجع;
      }

      إذا (اندازه_2 == بټونه_فی_کلمه) {
	    إذا (bbits_val_ | that.bbits_val_) {
		  abits_val_ = کلمه_X_ABITS;
		  bbits_val_ = کلمه_X_BBITS;
	    } وإلا {
		  abits_val_ -= that.abits_val_;
	    }
	    أرجع;
      }

      صحيح cnt = اندازه_2 / بټونه_فی_کلمه;
      بینښه اوږد لېږد = 1;
      لكل (صحيح idx = 0 ; idx < cnt ; idx += 1) {
	    إذا (bbits_ptr_[idx] | that.bbits_ptr_[idx])
		  اذهب x_بهر;

	    abits_ptr_[idx] = جمع_کړه_with_لېږد(abits_ptr_[idx], ~that.abits_ptr_[idx], لېږد);
      }

      إذا (بینښه لکۍ = اندازه_2 % بټونه_فی_کلمه) {
	    بینښه اوږد ماسک = ~( -1UL << لکۍ );
	    إذا ((bbits_ptr_[cnt] | that.bbits_ptr_[cnt])&ماسک)
		  اذهب x_بهر;

	    abits_ptr_[cnt] = جمع_کړه_with_لېږد(abits_ptr_[cnt], ~that.abits_ptr_[cnt], لېږد);
	    abits_ptr_[cnt] &= ماسک;
      }

      أرجع;

 x_بهر:
      لكل (صحيح idx = 0 ; idx < cnt ; idx += 1) {
	    abits_ptr_[idx] = کلمه_X_ABITS;
	    bbits_ptr_[idx] = کلمه_X_BBITS;
      }
      إذا (بینښه لکۍ = اندازه_2%بټونه_فی_کلمه) {
	    بینښه اوږد ماسک = ~( -1UL << لکۍ );
	    abits_ptr_[cnt] = کلمه_X_ABITS&ماسک;
	    bbits_ptr_[cnt] = کلمه_X_BBITS&ماسک;
      }

}

فراغ vvp_vector4_t::mov(بینښه dst, بینښه src, بینښه cnt)
{
      أكد(dst+cnt <= اندازه_2);
      أكد(src+cnt <= اندازه_2);

      إذا (اندازه_2 <= بټونه_فی_کلمه) {
	    بینښه اوږد vmask = (1UL << cnt) - 1;
	    بینښه اوږد tmp;

	    tmp = (abits_val_ >> src) & vmask;
	    abits_val_ &= ~ (vmask << dst);
	    abits_val_ |= tmp << dst;

	    tmp = (bbits_val_ >> src) & vmask;
	    bbits_val_ &= ~ (vmask << dst);
	    bbits_val_ |= tmp << dst;

      } وإلا {
	    بینښه sptr = src / بټونه_فی_کلمه;
	    بینښه dptr = dst / بټونه_فی_کلمه;
	    بینښه soff = src % بټونه_فی_کلمه;
	    بینښه doff = dst % بټونه_فی_کلمه;

	    طالما (cnt > 0) {
		  بینښه trans = cnt;
		  إذا ((soff+trans) > بټونه_فی_کلمه)
			trans = بټونه_فی_کلمه - soff;

		  إذا ((doff+trans) > بټونه_فی_کلمه)
			trans = بټونه_فی_کلمه - doff;

		  إذا (trans == بټونه_فی_کلمه) {
			  // Special case: the transfer count is
			  // exactly an entire word. For this to be
			  // true, it must also be true that the
			  // pointers are aligned. The work is easy,
			abits_ptr_[dptr] = abits_ptr_[sptr];
			bbits_ptr_[dptr] = bbits_ptr_[sptr];
			dptr += 1;
			sptr += 1;
			cnt -= بټونه_فی_کلمه;
			استمر;
		  }

		    // Here we know that either the source or
		    // destination is unaligned, and also we know that
		    // the count is less than a full word.
		  بینښه اوږد vmask = (1UL << trans) - 1;
		  بینښه اوږد tmp;

		  tmp = (abits_ptr_[sptr] >> soff) & vmask;
		  abits_ptr_[dptr] &= ~ (vmask << doff);
		  abits_ptr_[dptr] |= tmp << doff;

		  tmp = (bbits_ptr_[sptr] >> soff) & vmask;
		  bbits_ptr_[dptr] &= ~ (vmask << doff);
		  bbits_ptr_[dptr] |= tmp << doff;

		  cnt -= trans;
		  soff += trans;
		  إذا (soff >= بټونه_فی_کلمه) {
			soff = 0;
			sptr += 1;
		  }
		  doff += trans;
		  إذا (doff >= بټونه_فی_کلمه) {
			doff = 0;
			dptr += 1;
		  }
	    }
      }
}

فراغ vvp_vector4_t::mul(ثابت vvp_vector4_t&that)
{
      أكد(اندازه_2 == that.اندازه_2);

      إذا (اندازه_2 < بټونه_فی_کلمه) {
	    بینښه اوږد ماسک = ~(-1UL << اندازه_2);
	    إذا ((bbits_val_|that.bbits_val_) & ماسک) {
		  abits_val_ |= ماسک;
		  bbits_val_ |= ماسک;
		  أرجع;
	    }

	    abits_val_ *= that.abits_val_;
	    abits_val_ &= ماسک;
	    أرجع;
      }

      إذا (اندازه_2 == بټونه_فی_کلمه) {
	    إذا (bbits_val_ || that.bbits_val_) {
		  abits_val_ = کلمه_X_ABITS;
		  bbits_val_ = کلمه_X_BBITS;
	    } وإلا {
		  abits_val_ *= that.abits_val_;
	    }
	    أرجع;
      }

      ثابت صحيح cnt = (اندازه_2+بټونه_فی_کلمه-1) / بټونه_فی_کلمه;

      بینښه اوږد ماسک;
      إذا (بینښه لکۍ = اندازه_2%بټونه_فی_کلمه) {
	    ماسک = ~( -1UL << لکۍ );
      } وإلا {
	    ماسک = ~0UL;
      }

	// Check for any XZ values ahead of time in a first pass. If
	// we find any, then force the entire result to be X and be
	// done.
      لكل (صحيح idx = 0 ; idx < cnt ; idx += 1) {
	    بینښه اوږد lval = bbits_ptr_[idx];
	    بینښه اوږد rval = that.bbits_ptr_[idx];
	    إذا (idx == (cnt-1)) {
		  lval &= ماسک;
		  rval &= ماسک;
	    }
	    إذا (lval || rval) {
		  لكل (صحيح xdx = 0 ; xdx < cnt-1 ; xdx += 1) {
			abits_ptr_[xdx] = کلمه_X_ABITS;
			bbits_ptr_[xdx] = کلمه_X_BBITS;
		  }
		  abits_ptr_[cnt-1] = کلمه_X_ABITS & ماسک;
		  bbits_ptr_[cnt-1] = کلمه_X_BBITS & ماسک;
		  أرجع;
	    }
      }

	// Calculate the result into a res array. We need to keep is
	// separate from the "this" array because we are making
	// multiple passes.
      بینښه اوږد*res = جديد بینښه اوږد[cnt];
      لكل (صحيح idx = 0 ; idx < cnt ; idx += 1)
	    res[idx] = 0;

      لكل (صحيح mul_a = 0 ; mul_a < cnt ; mul_a += 1) {
	    بینښه اوږد lval = abits_ptr_[mul_a];
	    إذا (mul_a == (cnt-1))
		  lval &= ماسک;

	    لكل (صحيح mul_b = 0 ; mul_b < (cnt-mul_a) ; mul_b += 1) {
		  بینښه اوږد rval = that.abits_ptr_[mul_b];
		  إذا (mul_b == (cnt-1))
			rval &= ماسک;

		  بینښه اوږد مجموعه;
		  بینښه اوږد tmp = multiply_with_لېږد(lval, rval, مجموعه);
		  صحيح بنسټ = mul_a + mul_b;
		  بینښه اوږد لېږد = 0;
		  res[بنسټ] = جمع_کړه_with_لېږد(res[بنسټ], tmp, لېږد);
		  لكل (صحيح جمع_کړه_idx = بنسټ+1 ; جمع_کړه_idx < cnt ; جمع_کړه_idx += 1) {
			res[جمع_کړه_idx] = جمع_کړه_with_لېږد(res[جمع_کړه_idx], مجموعه, لېږد);
			مجموعه = 0;
		  }
	    }
      }

	// Replace the "this" value with the calculated result. We
	// know a-priori that the bbits are zero and unchanged.
      res[cnt-1] &= ماسک;
      لكل (صحيح idx = 0 ; idx < cnt ; idx += 1)
	    abits_ptr_[idx] = res[idx];

      ړنګول[]res;
      أرجع;


}

منطقي vvp_vector4_t::eeq(ثابت vvp_vector4_t&that) ثابت
{
      إذا (اندازه_2 != that.اندازه_2)
	    أرجع خطأ;

      إذا (اندازه_2 < بټونه_فی_کلمه) {
	    بینښه اوږد ماسک = (1UL << اندازه_2) - 1;
	    أرجع (abits_val_&ماسک) == (that.abits_val_&ماسک)
		  && (bbits_val_&ماسک) == (that.bbits_val_&ماسک);
      }

      إذا (اندازه_2 == بټونه_فی_کلمه) {
	    أرجع (abits_val_ == that.abits_val_)
		  && (bbits_val_ == that.bbits_val_);
      }

      بینښه ټکي = اندازه_2 / بټونه_فی_کلمه;
      لكل (بینښه idx = 0 ;  idx < ټکي ;  idx += 1) {
	    إذا (abits_ptr_[idx] != that.abits_ptr_[idx])
		  أرجع خطأ;
	    إذا (bbits_ptr_[idx] != that.bbits_ptr_[idx])
		  أرجع خطأ;
      }

      بینښه اوږد ماسک = اندازه_2%بټونه_فی_کلمه;
      إذا (ماسک > 0) {
	    ماسک = (1UL << ماسک) - 1;
	    أرجع (abits_ptr_[ټکي]&ماسک) == (that.abits_ptr_[ټکي]&ماسک)
		  && (bbits_ptr_[ټکي]&ماسک) == (that.bbits_ptr_[ټکي]&ماسک);
      }

      أرجع ریښتیا;
}

منطقي vvp_vector4_t::eq_xz(ثابت vvp_vector4_t&that) ثابت
{
      إذا (اندازه_2 != that.اندازه_2)
	    أرجع خطأ;

      إذا (اندازه_2 < بټونه_فی_کلمه) {
	    بینښه اوږد ماسک = (1UL << اندازه_2) - 1;
	    أرجع ((abits_val_|bbits_val_)&ماسک) == ((that.abits_val_|that.bbits_val_)&ماسک)
		  && (bbits_val_&ماسک) == (that.bbits_val_&ماسک);
      }

      إذا (اندازه_2 == بټونه_فی_کلمه) {
	    أرجع ((abits_val_|bbits_val_) == (that.abits_val_|that.bbits_val_))
		  && (bbits_val_ == that.bbits_val_);
      }

      بینښه ټکي = اندازه_2 / بټونه_فی_کلمه;
      لكل (بینښه idx = 0 ;  idx < ټکي ;  idx += 1) {
	    إذا ((abits_ptr_[idx]|bbits_ptr_[idx]) != (that.abits_ptr_[idx]|that.bbits_ptr_[idx]))
		  أرجع خطأ;
	    إذا (bbits_ptr_[idx] != that.bbits_ptr_[idx])
		  أرجع خطأ;
      }

      بینښه اوږد ماسک = اندازه_2%بټونه_فی_کلمه;
      إذا (ماسک > 0) {
	    ماسک = (1UL << ماسک) - 1;
	    أرجع ((abits_ptr_[ټکي]|bbits_ptr_[ټکي])&ماسک) == ((that.abits_ptr_[ټکي]|that.bbits_ptr_[ټکي])&ماسک)
		  && (bbits_ptr_[ټکي]&ماسک) == (that.bbits_ptr_[ټکي]&ماسک);
      }

      أرجع ریښتیا;
}

منطقي vvp_vector4_t::has_xz() ثابت
{
      إذا (اندازه_2 < بټونه_فی_کلمه) {
	    بینښه اوږد ماسک = -1UL >> (بټونه_فی_کلمه - اندازه_2);
	    أرجع bbits_val_&ماسک;
      }

      إذا (اندازه_2 == بټونه_فی_کلمه) {
	    أرجع bbits_val_;
      }

      بینښه ټکي = اندازه_2 / بټونه_فی_کلمه;
      لكل (بینښه idx = 0 ; idx < ټکي ; idx += 1) {
	    إذا (bbits_ptr_[idx])
		  أرجع ریښتیا;
      }

      بینښه اوږد ماسک = اندازه_2%بټونه_فی_کلمه;
      إذا (ماسک > 0) {
	    ماسک = -1UL >> (بټونه_فی_کلمه - ماسک);
	    أرجع bbits_ptr_[ټکي]&ماسک;
      }

      أرجع خطأ;
}

فراغ vvp_vector4_t::change_z2x()
{
	// This method relies on the fact that both BIT4_X and BIT4_Z
	// have the bbit set in the vector4 encoding, and also that
	// the BIT4_X has abit set in the vector4 encoding. By simply
	// or-ing the bbit into the abit, BIT4_X and BIT4_Z both
	// become BIT4_X.

      إذا (اندازه_2 <= بټونه_فی_کلمه) {
	    abits_val_ |= bbits_val_;
      } وإلا {
	    بینښه ټکي = (اندازه_2+بټونه_فی_کلمه-1) / بټونه_فی_کلمه;
	    لكل (بینښه idx = 0 ;  idx < ټکي ;  idx += 1)
		  abits_ptr_[idx] |= bbits_ptr_[idx];
      }
}

فراغ vvp_vector4_t::fill_بټونه(vvp_bit4_t بټ)
{
	/* note: this relies on the bit encoding for the vvp_bit4_t. */
      ساكن ثابت بینښه اوږد init_atable[4] = {
	    کلمه_0_ABITS,
	    کلمه_1_ABITS,
	    کلمه_Z_ABITS,
	    کلمه_X_ABITS };
      ساكن ثابت بینښه اوږد init_btable[4] = {
	    کلمه_0_BBITS,
	    کلمه_1_BBITS,
	    کلمه_Z_BBITS,
	    کلمه_X_BBITS };

      إذا (اندازه_2 <= بټونه_فی_کلمه) {
	    abits_val_ = init_atable[بټ];
	    bbits_val_ = init_btable[بټ];
      } وإلا {
	    بینښه ټکي = (اندازه_2 + بټونه_فی_کلمه - 1) / بټونه_فی_کلمه;
	    لكل (بینښه idx = 0; idx < ټکي; idx += 1) {
		  abits_ptr_[idx] = init_atable[بټ];
		  bbits_ptr_[idx] = init_btable[بټ];
	    }
      }
}

حرف* vvp_vector4_t::as_string(حرف*buf, size_t buf_len) ثابت
{
      حرف*res = buf;
      *buf++ = 'C';
      *buf++ = '4';
      *buf++ = '<';
      buf_len -= 3;

      لكل (بینښه idx = 0 ;  idx < size() && buf_len >= 2 ;  idx += 1) {
	    بدل (value(size()-idx-1)) {
		حالة BIT4_0:
		  *buf++ = '0';
		  اكسر;
		حالة BIT4_1:
		  *buf++ = '1';
		  اكسر;
		حالة BIT4_X:
		  *buf++ = 'x';
		  اكسر;
		حالة BIT4_Z:
		  *buf++ = 'z';
	    }
	    buf_len -= 1;
      }

      *buf++ = '>';
      *buf++ = 0;
      أرجع res;
}

فراغ vvp_vector4_t::invert()
{
      إذا (اندازه_2 <= بټونه_فی_کلمه) {
	    بینښه اوږد ماسک = (اندازه_2<بټونه_فی_کلمه)? (1UL<<اندازه_2)-1UL : -1UL;
	    abits_val_ = ماسک & ~abits_val_;
	    abits_val_ |= bbits_val_;
      } وإلا {
	    بینښه پاتې_شوی = اندازه_2;
	    بینښه idx = 0;
	    طالما (پاتې_شوی >= بټونه_فی_کلمه) {
		  abits_ptr_[idx] = ~abits_ptr_[idx];
		  abits_ptr_[idx] |= bbits_ptr_[idx];
		  idx += 1;
		  پاتې_شوی -= بټونه_فی_کلمه;
	    }
	    إذا (پاتې_شوی > 0) {
		  بینښه اوږد ماسک = (1UL<<پاتې_شوی) - 1UL;
		  abits_ptr_[idx] = ماسک & ~abits_ptr_[idx];
		  abits_ptr_[idx] |= bbits_ptr_[idx];
	    }
      }
}

#تعریف_کړه بټ_ماسک(n) ((n) ? ((~0UL) >> (بټونه_فی_کلمه - (n))) : ~0UL)

vvp_bit4_t vvp_vector4_t::reduce_or() ثابت
{
      بینښه اوږد ماسک = بټ_ماسک(اندازه_2 % بټونه_فی_کلمه);
      vvp_bit4_t res = BIT4_0;

      إذا (اندازه_2 <= بټونه_فی_کلمه) {
	    إذا ((abits_val_ & ~bbits_val_ & ماسک) != 0UL)
		  أرجع BIT4_1;
	    إذا ((bbits_val_ & ماسک) != 0UL)
		  أرجع BIT4_X;
      } وإلا {
	    بینښه ټکي = (اندازه_2 + بټونه_فی_کلمه - 1) / بټونه_فی_کلمه;
	    بینښه idx;
	    لكل (idx = 0; idx < ټکي - 1; idx += 1) {
		  إذا ((abits_ptr_[idx] & ~bbits_ptr_[idx]) != 0UL)
			أرجع BIT4_1;
		  إذا (bbits_ptr_[idx] != 0UL)
			res = BIT4_X;
	    }
	    إذا ((abits_ptr_[idx] & ~bbits_ptr_[idx] & ماسک) != 0UL)
		  أرجع BIT4_1;
	    إذا ((bbits_ptr_[idx] & ماسک) != 0UL)
		  res = BIT4_X;
      }

      أرجع res;
}

vvp_bit4_t vvp_vector4_t::reduce_and() ثابت
{
      بینښه اوږد ماسک = بټ_ماسک(اندازه_2 % بټونه_فی_کلمه);
      vvp_bit4_t res = BIT4_1;

      إذا (اندازه_2 <= بټونه_فی_کلمه) {
	    إذا ((abits_val_ | bbits_val_ | ~ماسک) != ~0UL)
		  أرجع BIT4_0;
	    إذا ((bbits_val_ & ماسک) != 0UL)
		  أرجع BIT4_X;
	} وإلا {
	    بینښه ټکي = (اندازه_2 + بټونه_فی_کلمه - 1) / بټونه_فی_کلمه;
	    بینښه idx;
	    لكل (idx = 0; idx < ټکي - 1; idx += 1) {
		  إذا ((abits_ptr_[idx] | bbits_ptr_[idx]) != ~0UL)
			أرجع BIT4_0;
		  إذا (bbits_ptr_[idx] != 0UL)
			res = BIT4_X;
	    }
	    إذا ((abits_ptr_[idx] | bbits_ptr_[idx] | ~ماسک) != ~0UL)
		  أرجع BIT4_0;
	    إذا ((bbits_ptr_[idx] & ماسک) != 0UL)
		  res = BIT4_X;
      }

      أرجع res;
}

ساكن بینښه اوږد parity(بینښه اوږد val)
{
#إذا defined(__GNUC__)
      // The compiler builtin can use target-specific CPU instructions.
      أرجع __builtin_parityl(val);
#وإلا
#إذا ULONG_MAX > 0xffffffffUL
	val ^= val >> 32;
#د_که_پای
	val ^= val >> 16;
	val ^= val >> 8;
	val ^= val >> 4;

      أرجع (0x6996 >> (val & 0xf)) & 1;
#د_که_پای
}

vvp_bit4_t vvp_vector4_t::reduce_xor() ثابت
{
      بینښه اوږد ماسک = بټ_ماسک(اندازه_2 % بټونه_فی_کلمه);

      إذا (اندازه_2 <= بټونه_فی_کلمه) {
	    إذا ((bbits_val_ & ماسک) != 0UL)
		  أرجع BIT4_X;
	    أرجع parity(abits_val_ & ماسک) ? BIT4_1 : BIT4_0;
      } وإلا {
	    بینښه ټکي = (اندازه_2 + بټونه_فی_کلمه - 1) / بټونه_فی_کلمه;
	    بینښه اوږد val_a = 0UL;
	    بینښه idx;
	    لكل (idx = 0; idx < ټکي - 1; idx += 1) {
		  إذا (bbits_ptr_[idx] != 0UL)
			أرجع BIT4_X;
		  val_a ^= abits_ptr_[idx];
	    }
	    إذا ((bbits_ptr_[idx] & ماسک) != 0UL)
		  أرجع BIT4_X;
	    val_a ^= abits_ptr_[idx] & ماسک;
	    أرجع parity(val_a) ? BIT4_1 : BIT4_0;
      }
}

vvp_vector4_t& vvp_vector4_t::چلوونکی &= (ثابت vvp_vector4_t&that)
{
	// The truth table is:
	//     00 01 11 10
	//  00 00 00 00 00
	//  01 00 01 11 11
	//  11 00 11 11 11
	//  10 00 11 11 11
      إذا (اندازه_2 <= بټونه_فی_کلمه) {
	    بینښه اوږد tmp1 = abits_val_ | bbits_val_;
	    بینښه اوږد tmp2 = that.abits_val_ | that.bbits_val_;
	    abits_val_ = tmp1 & tmp2;
	    bbits_val_ = (tmp1 & that.bbits_val_) | (tmp2 & bbits_val_);
      } وإلا {
	    بینښه ټکي = (اندازه_2 + بټونه_فی_کلمه - 1) / بټونه_فی_کلمه;
	    لكل (بینښه idx = 0; idx < ټکي ; idx += 1) {
		  بینښه اوږد tmp1 = abits_ptr_[idx] | bbits_ptr_[idx];
		  بینښه اوږد tmp2 = that.abits_ptr_[idx] |
		                       that.bbits_ptr_[idx];
		  abits_ptr_[idx] = tmp1 & tmp2;
		  bbits_ptr_[idx] = (tmp1 & that.bbits_ptr_[idx]) |
		                    (tmp2 & bbits_ptr_[idx]);
	    }
      }

      أرجع *هذا;
}

vvp_vector4_t& vvp_vector4_t::چلوونکی |= (ثابت vvp_vector4_t&that)
{
	// The truth table is:
	//     00 01 11 10
	//  00 00 01 11 11
	//  01 01 01 01 01
	//  11 11 01 11 11
	//  10 11 01 11 11
      إذا (اندازه_2 <= بټونه_فی_کلمه) {
	    بینښه اوږد tmp = abits_val_ | bbits_val_ |
	                        that.abits_val_ | that.bbits_val_;
	    bbits_val_ = ((~abits_val_ | bbits_val_) & that.bbits_val_) |
	                 ((~that.abits_val_ | that.bbits_val_) & bbits_val_);
	    abits_val_ = tmp;

      } وإلا {
	    بینښه ټکي = (اندازه_2 + بټونه_فی_کلمه - 1) / بټونه_فی_کلمه;
	    لكل (بینښه idx = 0; idx < ټکي ; idx += 1) {
		  بینښه اوږد tmp = abits_ptr_[idx] | bbits_ptr_[idx] |
	                        that.abits_ptr_[idx] | that.bbits_ptr_[idx];
		  bbits_ptr_[idx] = ((~abits_ptr_[idx] | bbits_ptr_[idx]) &
		                     that.bbits_ptr_[idx]) |
		                    ((~that.abits_ptr_[idx] |
		                      that.bbits_ptr_[idx]) & bbits_ptr_[idx]);
		  abits_ptr_[idx] = tmp;
	    }
      }

      أرجع *هذا;
}

vvp_vector4_t& vvp_vector4_t::چلوونکی ^= (ثابت vvp_vector4_t&that)
{
	// The truth table is:
	//     00 01 11 10
	//  00 00 01 11 11
	//  01 01 00 11 11
	//  11 11 11 11 11
	//  10 11 11 11 11
      إذا (اندازه_2 <= بټونه_فی_کلمه) {
	    بینښه اوږد bval = bbits_val_ | that.bbits_val_;
	    bbits_val_ = bval;
	    abits_val_ = (abits_val_ ^ that.abits_val_) | bval;
      } وإلا {
	    بینښه ټکي = (اندازه_2 + بټونه_فی_کلمه - 1) / بټونه_فی_کلمه;
	    لكل (بینښه idx = 0; idx < ټکي ; idx += 1) {
		  بینښه اوږد bval = bbits_ptr_[idx] | that.bbits_ptr_[idx];
		  bbits_ptr_[idx] = bval;
		  abits_ptr_[idx] = (abits_ptr_[idx] ^ that.abits_ptr_[idx]) | bval;
	    }
      }

      أرجع *هذا;
}

/*
* Add an integer to the vvp_vector4_t in place, bit by bit so that
* there is no size limitations.
*/
vvp_vector4_t& vvp_vector4_t::چلوونکی += (int64_t that)
{
      vvp_bit4_t لېږد = BIT4_0;
      بینښه idx;

      إذا (has_xz()) {
	    vvp_vector4_t xxx (size(), BIT4_X);
	    *هذا = xxx;
	    أرجع *هذا;
      }

      لكل (idx = 0 ; idx < size() ; idx += 1) {
	    إذا (that == 0 && لېږد==BIT4_0)
		  اكسر;

	    vvp_bit4_t that_بټ = (that&1)? BIT4_1 : BIT4_0;
	    that >>= 1;

	    إذا (that_بټ==BIT4_0 && لېږد==BIT4_0)
		  استمر;

	    vvp_bit4_t بټ = value(idx);
	    بټ = جمع_کړه_with_لېږد(بټ, that_بټ, لېږد);

	    سېټ_بټ(idx, بټ);
      }

      أرجع *هذا;
}

ostream& چلوونکی<< (ostream&out, ثابت vvp_vector4_t&that)
{
      out << that.size() << "'b";
      لكل (بینښه idx = 0 ;  idx < that.size() ;  idx += 1)
	    out << that.value(that.size()-idx-1);
      أرجع out;
}

کينډۍ <صنف INT>منطقي vector4_منزل_ارزښت(ثابت vvp_vector4_t&vec, INT&val,
					  منطقي is_signed, منطقي is_arithmetic)
{
      INT res = 0;
      INT msk = 1;
      منطقي rc_flag = ریښتیا;

      بینښه size = vec.size();
      إذا (size > 8*حجم(val)) size = 8*حجم(val);
      لكل (بینښه idx = 0 ;  idx < size ;  idx += 1) {
	    بدل (vec.value(idx)) {
		حالة BIT4_0:
		  اكسر;
		حالة BIT4_1:
		  res |= msk;
		  اكسر;
		افتراضي:
		  إذا (is_arithmetic)
			أرجع خطأ;
		  وإلا
			rc_flag = خطأ;
	    }

	    msk <<= 1;
      }

      إذا (is_signed && vec.value(vec.size()-1) == BIT4_1) {
	    إذا (vec.size() < 8*حجم(val))
		  res |= static_cast<INT>(-1ULL << vec.size());
      }

      val = res;
      أرجع rc_flag;
}

کينډۍ منطقي vector4_منزل_ارزښت(ثابت vvp_vector4_t&vec, int8_t&val,
			       منطقي is_signed, منطقي is_arithmetic);
کينډۍ منطقي vector4_منزل_ارزښت(ثابت vvp_vector4_t&vec, int16_t&val,
			       منطقي is_signed, منطقي is_arithmetic);
کينډۍ منطقي vector4_منزل_ارزښت(ثابت vvp_vector4_t&vec, int32_t&val,
			       منطقي is_signed, منطقي is_arithmetic);
کينډۍ منطقي vector4_منزل_ارزښت(ثابت vvp_vector4_t&vec, int64_t&val,
			       منطقي is_signed, منطقي is_arithmetic);
کينډۍ منطقي vector4_منزل_ارزښت(ثابت vvp_vector4_t&vec, uint8_t&val,
			       منطقي is_signed, منطقي is_arithmetic);
کينډۍ منطقي vector4_منزل_ارزښت(ثابت vvp_vector4_t&vec, uint16_t&val,
			       منطقي is_signed, منطقي is_arithmetic);
کينډۍ منطقي vector4_منزل_ارزښت(ثابت vvp_vector4_t&vec, uint32_t&val,
			       منطقي is_signed, منطقي is_arithmetic);
کينډۍ منطقي vector4_منزل_ارزښت(ثابت vvp_vector4_t&vec, uint64_t&val,
			       منطقي is_signed, منطقي is_arithmetic);

کينډۍ <صنف T> منطقي vector4_منزل_ارزښت(ثابت vvp_vector4_t&vec,
                                         منطقي&ډکېدنه_flag, T&val)
{
      T res = 0;
      T msk = 1;

      ډکېدنه_flag = خطأ;
      بینښه size = vec.size();
      لكل (بینښه idx = 0 ;  idx < size ;  idx += 1) {
	    بدل (vec.value(idx)) {
		حالة BIT4_0:
		  اكسر;
		حالة BIT4_1:
		  إذا (msk == 0)
			ډکېدنه_flag = ریښتیا;
		  وإلا
			res |= msk;
		  اكسر;
		افتراضي:
		  أرجع خطأ;
	    }

	    msk <<= static_cast<T>(1);
      }

      val = res;
      أرجع ریښتیا;
}

کينډۍ منطقي vector4_منزل_ارزښت(ثابت vvp_vector4_t&vec, منطقي&ډکېدنه_flag,
                               بینښه اوږد&val);
#که_نه_تعریف_شوی UL_AND_TIME64_SAME
کينډۍ منطقي vector4_منزل_ارزښت(ثابت vvp_vector4_t&vec, منطقي&ډکېدنه_flag,
                               vvp_time64_t&val);
#د_که_پای

منطقي vector4_منزل_ارزښت(ثابت vvp_vector4_t&vec, مزدوج&val, منطقي signed_flag)
{

      إذا (vec.size() == 0) {
	    val = 0.0;
	    أرجع ریښتیا;
      }

      منطقي flag = ریښتیا;

      إذا (vec.value(vec.size()-1) != BIT4_1) {
	    signed_flag = خطأ;
      }

      مزدوج res = 0.0;
      إذا (signed_flag) {
	    vvp_bit4_t لېږد = BIT4_1;
	    لكل (بینښه idx = 0 ;  idx < vec.size() ;  idx += 1) {
		  vvp_bit4_t a = ~vec.value(idx);
		  vvp_bit4_t x = جمع_کړه_with_لېږد(a, BIT4_0, لېږد);
		  بدل (x) {
		      حالة BIT4_0:
			اكسر;
		      حالة BIT4_1:
			res += توان(2.0, (صحيح)idx);
			اكسر;
		      افتراضي:
			flag = خطأ;
		  }
	    }
	    res *= -1.0;
      } وإلا {
	    لكل (بینښه idx = 0 ;  idx < vec.size() ;  idx += 1) {
		  بدل (vec.value(idx)) {
		      حالة BIT4_0:
			اكسر;
		      حالة BIT4_1:
			res += توان(2.0, (صحيح)idx);
			اكسر;
		      افتراضي:
			flag = خطأ;
		  }
	    }
      }
      val = res;
      أرجع flag;
}

منطقي vector2_منزل_ارزښت(ثابت vvp_vector2_t&a, int32_t&val, منطقي is_signed)
{
      val = 0;
      بینښه idx;
      uint32_t ماسک;
      لكل (idx = 0, ماسک = 1 ; idx < a.size() && idx < 32 ; idx += 1, ماسک <<= 1) {
	    إذا (a.value(idx)) val |= ماسک;
      }

      إذا (is_signed && a.size() < 32 && a.value(a.size()-1)) {
	    ماسک = -1;
	    ماسک <<= a.size();
	    val |= ماسک;
      }

      أرجع a.size() <= 32;
}

vvp_vector4array_t::vvp_vector4array_t(بینښه پلنوالی_3, بینښه ټکي_2)
: پلنوالی_2(پلنوالی_3), ټکي_3(ټکي_2)
{
}

vvp_vector4array_t::~vvp_vector4array_t()
{
}

فراغ vvp_vector4array_t::سېټ_کلمه_2(v4cell*cell, ثابت vvp_vector4_t&that)
{
      أكد(that.اندازه_2 == پلنوالی_2);

      إذا (پلنوالی_2 <= vvp_vector4_t::بټونه_فی_کلمه) {
	    cell->abits_val_ = that.abits_val_;
	    cell->bbits_val_ = that.bbits_val_;
	    أرجع;
      }

      بینښه cnt = (پلنوالی_2 + vvp_vector4_t::بټونه_فی_کلمه-1)/vvp_vector4_t::بټونه_فی_کلمه;

      إذا (cell->abits_ptr_ == 0) {
	    cell->abits_ptr_ = جديد بینښه اوږد[2*cnt];
	    cell->bbits_ptr_ = cell->abits_ptr_ + cnt;
      }

      لكل (بینښه idx = 0 ; idx < cnt ; idx += 1)
	    cell->abits_ptr_[idx] = that.abits_ptr_[idx];
      لكل (بینښه idx = 0 ; idx < cnt ; idx += 1)
	    cell->bbits_ptr_[idx] = that.bbits_ptr_[idx];
}

vvp_vector4_t vvp_vector4array_t::get_کلمه_2(v4cell*cell) ثابت
{
      إذا (پلنوالی_2 <= vvp_vector4_t::بټونه_فی_کلمه) {
	    vvp_vector4_t res;
	    res.اندازه_2 = پلنوالی_2;
	    res.abits_val_ = cell->abits_val_;
	    res.bbits_val_ = cell->bbits_val_;
	    أرجع res;
      }

      vvp_vector4_t res (پلنوالی_2, BIT4_X);
      إذا (cell->abits_ptr_ == 0)
	    أرجع res;

      بینښه cnt = (پلنوالی_2 + vvp_vector4_t::بټونه_فی_کلمه-1)/vvp_vector4_t::بټونه_فی_کلمه;

      لكل (بینښه idx = 0 ; idx < cnt ; idx += 1)
	    res.abits_ptr_[idx] = cell->abits_ptr_[idx];
      لكل (بینښه idx = 0 ; idx < cnt ; idx += 1)
	    res.bbits_ptr_[idx] = cell->bbits_ptr_[idx];

      أرجع res;
}

vvp_vector4array_sa::vvp_vector4array_sa(بینښه پلنوالی_3, بینښه ټکي_2)
: vvp_vector4array_t(پلنوالی_3, ټکي_2)
{
      array_ = جديد v4cell[ټکي_3];

      إذا (پلنوالی_2 <= vvp_vector4_t::بټونه_فی_کلمه) {
	    لكل (بینښه idx = 0 ; idx < ټکي_3 ; idx += 1) {
		  array_[idx].abits_val_ = vvp_vector4_t::کلمه_X_ABITS;
		  array_[idx].bbits_val_ = vvp_vector4_t::کلمه_X_BBITS;
	    }
      } وإلا {
	    لكل (بینښه idx = 0 ; idx < ټکي_3 ; idx += 1) {
		  array_[idx].abits_ptr_ = 0;
		  array_[idx].bbits_ptr_ = 0;
	    }
      }
}

vvp_vector4array_sa::~vvp_vector4array_sa()
{
      إذا (array_) {
	    إذا (پلنوالی_2 > vvp_vector4_t::بټونه_فی_کلمه) {
		  لكل (بینښه idx = 0 ; idx < ټکي_3 ; idx += 1)
			إذا (array_[idx].abits_ptr_)
			      ړنګول[]array_[idx].abits_ptr_;
	    }
	    ړنګول[]array_;
      }
}

فراغ vvp_vector4array_sa::سېټ_کلمه(بینښه شاخص, ثابت vvp_vector4_t&that)
{
      أكد(شاخص < ټکي_3);

      v4cell*cell = &array_[شاخص];

      سېټ_کلمه_2(cell, that);
}

vvp_vector4_t vvp_vector4array_sa::get_کلمه(بینښه شاخص) ثابت
{
      إذا (شاخص >= ټکي_3)
	    أرجع vvp_vector4_t(پلنوالی_2, BIT4_X);

      أكد(شاخص < ټکي_3);

      v4cell*cell = &array_[شاخص];

      أرجع get_کلمه_2(cell);
}

vvp_vector4array_aa::vvp_vector4array_aa(بینښه پلنوالی_3, بینښه ټکي_2)
: vvp_vector4array_t(پلنوالی_3, ټکي_2)
{
      context_idx_ = vpip_جمع_کړه_توکی_منزل_context(هذا, vpip_peek_context_scope());
}

vvp_vector4array_aa::~vvp_vector4array_aa()
{
}

فراغ vvp_vector4array_aa::alloc_instance(vvp_context_t context)
{
      v4cell*array = جديد v4cell[ټکي_3];

      إذا (پلنوالی_2 <= vvp_vector4_t::بټونه_فی_کلمه) {
	    لكل (بینښه idx = 0 ; idx < ټکي_3 ; idx += 1) {
		  array[idx].abits_val_ = vvp_vector4_t::کلمه_X_ABITS;
		  array[idx].bbits_val_ = vvp_vector4_t::کلمه_X_BBITS;
	    }
      } وإلا {
	    لكل (بینښه idx = 0 ; idx < ټکي_3 ; idx += 1) {
		  array[idx].abits_ptr_ = 0;
		  array[idx].bbits_ptr_ = 0;
	    }
      }

      vvp_سېټ_context_توکی(context, context_idx_, array);
}

فراغ vvp_vector4array_aa::reset_instance(vvp_context_t context)
{
      v4cell*cell = static_cast<v4cell*>
            (vvp_get_context_توکی(context, context_idx_));

      إذا (پلنوالی_2 <= vvp_vector4_t::بټونه_فی_کلمه) {
	    لكل (بینښه idx = 0 ; idx < ټکي_3 ; idx += 1) {
		  cell->abits_val_ = vvp_vector4_t::کلمه_X_ABITS;
		  cell->bbits_val_ = vvp_vector4_t::کلمه_X_BBITS;
                  cell++;
	    }
      } وإلا {
            بینښه cnt = (پلنوالی_2 + vvp_vector4_t::بټونه_فی_کلمه-1)/vvp_vector4_t::بټونه_فی_کلمه;
	    لكل (بینښه idx = 0 ; idx < ټکي_3 ; idx += 1) {
		  إذا (cell->abits_ptr_) {
                        لكل (بینښه n = 0 ; n < cnt ; n += 1) {
			      cell->abits_ptr_[n] = vvp_vector4_t::کلمه_X_ABITS;
			      cell->bbits_ptr_[n] = vvp_vector4_t::کلمه_X_BBITS;
                        }
                  }
                  cell++;
	    }
      }
}

#که_تعریف_شوی CHECK_WITH_VALGRIND
فراغ vvp_vector4array_aa::free_instance(vvp_context_t context)
{
      v4cell*cell = static_cast<v4cell*>
            (vvp_get_context_توکی(context, context_idx_));
      ړنګول [] cell;
}
#د_که_پای

فراغ vvp_vector4array_aa::سېټ_کلمه(بینښه شاخص, ثابت vvp_vector4_t&that)
{
      أكد(شاخص < ټکي_3);

      v4cell*cell = static_cast<v4cell*>
            (vthread_get_wt_context_توکی(context_idx_)) + شاخص;

      سېټ_کلمه_2(cell, that);
}

vvp_vector4_t vvp_vector4array_aa::get_کلمه(بینښه شاخص) ثابت
{
      إذا (شاخص >= ټکي_3)
	    أرجع vvp_vector4_t(پلنوالی_2, BIT4_X);

      أكد(شاخص < ټکي_3);

      v4cell*cell = static_cast<v4cell*>
            (vthread_get_rd_context_توکی(context_idx_)) + شاخص;

      أرجع get_کلمه_2(cell);
}

vvp_vector2_t::vvp_vector2_t()
{
      vec_ = 0;
      wid_ = 0;
}

vvp_vector2_t::vvp_vector2_t(uint64_t v, بینښه wid)
{
      wid_ = wid;
      ثابت بینښه بټونه_فی_کلمه_2 = 8 * حجم(vec_[0]);
      ثابت بینښه ټکي = (wid_ + بټونه_فی_کلمه_2-1) / بټونه_فی_کلمه_2;

      vec_ = جديد بینښه اوږد[ټکي];
      لكل (بینښه idx = 0 ; idx < ټکي ; idx += 1) {
	    ثابت بینښه ښويدنه = idx * بټونه_فی_کلمه_2;
	    vec_[idx] = ښويدنه < 64 ? v >> ښويدنه : 0;
      }
}

vvp_vector2_t::vvp_vector2_t(vvp_vector2_t::fill_t fill, بینښه wid)
{
      wid_ = wid;
      ثابت بینښه بټونه_فی_کلمه_2 = 8 * حجم(vec_[0]);
      ثابت بینښه ټکي = (wid_ + بټونه_فی_کلمه_2-1) / بټونه_فی_کلمه_2;

      vec_ = جديد بینښه اوږد[ټکي];
      لكل (بینښه idx = 0 ;  idx < ټکي ;  idx += 1)
	    vec_[idx] = fill? -1 : 0;
}

vvp_vector2_t::vvp_vector2_t(ثابت vvp_vector2_t&that, بینښه بنسټ, بینښه wid)
{
      wid_ = wid;
      ثابت بینښه ټکي = (wid_ + بټونه_فی_کلمه-1) / بټونه_فی_کلمه;

      vec_ = جديد بینښه اوږد[ټکي];

      لكل (بینښه idx = 0 ; idx < wid ; idx += 1) {
	    صحيح بټ = that.value(بنسټ+idx);
	    إذا (بټ == 0)
		  استمر;
	    بینښه کلمه = idx / بټونه_فی_کلمه;
	    بینښه اوږد ماسک = 1UL << (idx % بټونه_فی_کلمه);
	    vec_[کلمه] |= ماسک;
      }
}

فراغ vvp_vector2_t::copy_from_that_(ثابت vvp_vector4_t&that)
{
      wid_ = that.size();
      ثابت بینښه ټکي = (that.size() + بټونه_فی_کلمه-1) / بټونه_فی_کلمه;

      إذا (ټکي == 0) {
	    vec_ = 0;
	    wid_ = 0;
	    أرجع;
      }

	// Use the subarray method with the xz_to_0 flag set so that
	// we get values even when there are xz bits.
      vec_ = that.subarray(0, wid_, ریښتیا);
}

فراغ vvp_vector2_t::copy_from_that_(ثابت vvp_vector2_t&that)
{
      wid_ = that.wid_;
      ثابت بینښه ټکي = (wid_ + بټونه_فی_کلمه-1) / بټونه_فی_کلمه;

      إذا (ټکي == 0) {
	    vec_ = 0;
	    wid_ = 0;
	    أرجع;
      }

      vec_ = جديد بینښه اوږد[ټکي];
      لكل (بینښه idx = 0 ;  idx < ټکي ;  idx += 1)
	    vec_[idx] = that.vec_[idx];
}

vvp_vector2_t::vvp_vector2_t(ثابت vvp_vector2_t&that, بینښه newsize)
{
      wid_ = newsize;
      إذا (newsize == 0) {
	    vec_ = 0;
	    أرجع;
      }

      ثابت بینښه ټکي = (wid_ + بټونه_فی_کلمه-1) / بټونه_فی_کلمه;
      ثابت بینښه twords = (that.wid_ + بټونه_فی_کلمه-1) / بټونه_فی_کلمه;

      vec_ = جديد بینښه اوږد[ټکي];
      لكل (بینښه idx = 0 ;  idx < ټکي ;  idx += 1) {
	    إذا (idx < twords)
		  vec_[idx] = that.vec_[idx];
	    وإلا
		  vec_[idx] = 0;
      }
}

vvp_vector2_t& vvp_vector2_t::چلوونکی= (ثابت vvp_vector2_t&that)
{
      إذا (هذا == &that)
	    أرجع *هذا;

      ړنګول[] vec_;
      vec_ = 0;

      copy_from_that_(that);
      أرجع *هذا;
}

vvp_vector2_t& vvp_vector2_t::چلوونکی= (ثابت vvp_vector4_t&that)
{
      ړنګول[]vec_;
      vec_ = 0;
      copy_from_that_(that);
      أرجع *هذا;
}

vvp_vector2_t& vvp_vector2_t::چلوونکی <<= (بینښه صحيح ښويدنه)
{
      إذا (wid_ == 0)
	    أرجع *هذا;

      ثابت بینښه ټکي = (wid_ + بټونه_فی_کلمه-1) / بټونه_فی_کلمه;

	// Number of words to shift
      ثابت بینښه wshift = ښويدنه / بټونه_فی_کلمه;
	// bits to shift within each word.
      ثابت بینښه اوږد oshift = ښويدنه % بټونه_فی_کلمه;

	// If shifting the entire value away, then return zeros.
      إذا (wshift >= ټکي) {
	    لكل (بینښه idx = 0 ;  idx < ټکي ;  idx += 1)
		  vec_[idx] = 0;

	    أرجع *هذا;
      }

	// Do the word shift first.
      إذا (wshift > 0) {
	    لكل (بینښه idx = 0 ;  idx < ټکي-wshift ;  idx += 1) {
		  بینښه sel = ټکي - idx - 1;
		  vec_[sel] = vec_[sel-wshift];
	    }

	    لكل (بینښه idx = 0 ;  idx < wshift ;  idx += 1)
		  vec_[idx] = 0;
      }

	// Do the fine shift.
      إذا (oshift != 0) {
	    بینښه اوږد pad = 0;
	    لكل (بینښه idx = 0 ;  idx < ټکي ;  idx += 1) {
		  بینښه اوږد بل_pad = vec_[idx] >> (بټونه_فی_کلمه-oshift);
		  vec_[idx] = (vec_[idx] << oshift) | pad;
		  pad = بل_pad;
	    }

	      // Cleanup the tail bits.
	    بینښه اوږد ماسک = -1UL >> (بټونه_فی_کلمه - wid_%بټونه_فی_کلمه);
	    vec_[ټکي-1] &= ماسک;
      }

      أرجع *هذا;
}

vvp_vector2_t& vvp_vector2_t::چلوونکی >>= (بینښه ښويدنه)
{
      إذا (wid_ == 0)
	    أرجع *هذا;

      ثابت بینښه ټکي = (wid_ + بټونه_فی_کلمه-1) / بټونه_فی_کلمه;

	// Number of words to shift
      ثابت بینښه wshift = ښويدنه / بټونه_فی_کلمه;
	// bits to shift within each word.
      ثابت بینښه اوږد oshift = ښويدنه % بټونه_فی_کلمه;

	// If shifting the entire value away, then return zeros.
      إذا (wshift >= ټکي) {
	    لكل (بینښه idx = 0 ;  idx < ټکي ;  idx += 1)
		  vec_[idx] = 0;

	    أرجع *هذا;
      }

      إذا (wshift > 0) {
	    لكل (بینښه idx = 0 ;  idx < ټکي-wshift ;  idx += 1)
		  vec_[idx] = vec_[idx+wshift];

	    لكل (بینښه idx = ټکي-wshift ;  idx < ټکي ;  idx += 1)
		  vec_[idx] = 0;
      }

      إذا (oshift > 0) {
	    بینښه اوږد pad = 0;
	    لكل (بینښه idx = ټکي ;  idx > 0 ;  idx -= 1) {
		  بینښه اوږد new_pad = vec_[idx-1] <<(بټونه_فی_کلمه-oshift);
		  vec_[idx-1] = pad | (vec_[idx-1] >> oshift);
		  pad = new_pad;
	    }

	      // Cleanup the tail bits.

	    بینښه use_ټکي = ټکي;
	      // Mask_shift is the number of high bits of the top word
	      // that are to be masked off. We start with the number
	      // of bits that are not included even in the original,
	      // then we include the bits of the shift, that are to be
	      // masked to zero.
	    بینښه اوږد ماسک_ښويدنه = بټونه_فی_کلمه - wid_%بټونه_فی_کلمه;
	    ماسک_ښويدنه %= بټونه_فی_کلمه;
	    ماسک_ښويدنه += oshift;
	    طالما (ماسک_ښويدنه >= بټونه_فی_کلمه) {
		  vec_[use_ټکي-1] = 0;
		  use_ټکي -= 1;
		  ماسک_ښويدنه -= بټونه_فی_کلمه;
	    }
	    إذا (ماسک_ښويدنه > 0) {
		  أكد(use_ټکي > 0);
		  بینښه اوږد ماسک = -1UL >> ماسک_ښويدنه;
		  vec_[use_ټکي-1] &= ماسک;
	    }
      }

      أرجع *هذا;
}

ساكن بینښه اوږد جمع_کړه_لېږد(بینښه اوږد a, بینښه اوږد b,
			       بینښه اوږد&لېږد)
{
      بینښه اوږد out = لېږد;
      لېږد = 0;

      إذا ((ULONG_MAX - out) < a)
	    لېږد += 1;
      out += a;

      إذا ((ULONG_MAX - out) < b)
	    لېږد += 1;
      out += b;

      أرجع out;
}

vvp_vector2_t& vvp_vector2_t::چلوونکی += (ثابت vvp_vector2_t&that)
{
      أكد(wid_ == that.wid_);
      إذا (wid_ == 0)
	    أرجع *هذا;

      ثابت بینښه ټکي = (wid_ + بټونه_فی_کلمه-1) / بټونه_فی_کلمه;

      بینښه اوږد لېږد = 0;
      لكل (بینښه idx = 0 ;  idx < ټکي ;  idx += 1) {
	    vec_[idx] = جمع_کړه_لېږد(vec_[idx], that.vec_[idx], لېږد);
      }


	// Cleanup the tail bits.
      بینښه اوږد ماسک = -1UL >> (بټونه_فی_کلمه - wid_%بټونه_فی_کلمه);
      vec_[ټکي-1] &= ماسک;

      أرجع *هذا;
}

vvp_vector2_t& vvp_vector2_t::چلوونکی -= (ثابت vvp_vector2_t&that)
{
      أكد(wid_ == that.wid_);
      إذا (wid_ == 0)
	    أرجع *هذا;

      ثابت بینښه ټکي = (wid_ + بټونه_فی_کلمه-1) / بټونه_فی_کلمه;

      بینښه اوږد لېږد = 1;
      لكل (بینښه idx = 0 ;  idx < ټکي ;  idx += 1) {
	    vec_[idx] = جمع_کړه_لېږد(vec_[idx], ~that.vec_[idx], لېږد);
      }

      أرجع *هذا;
}

فراغ vvp_vector2_t::trim()
{
      طالما (value(wid_-1) == 0 && wid_ > 1) wid_ -= 1;
}

/* This is a special trim that is used on numbers we know represent a
 * negative signed value (they came from a negative real value). */
فراغ vvp_vector2_t::trim_neg()
{
      إذا (value(wid_-1) == 1 && wid_ > 32) {
	    طالما (value(wid_-2) == 1 && wid_ > 32) wid_ -= 1;
      }
}

صحيح vvp_vector2_t::value(بینښه idx) ثابت
{
      إذا (idx >= wid_)
	    أرجع 0;

      ثابت بینښه بټونه_فی_کلمه_2 = 8 * حجم(vec_[0]);
      بینښه addr = idx/بټونه_فی_کلمه_2;
      بینښه ماسک = idx%بټونه_فی_کلمه_2;

      إذا (vec_[addr] & (1UL<<ماسک))
	    أرجع 1;
      وإلا
	    أرجع 0;
}

فراغ vvp_vector2_t::سېټ_بټ(بینښه idx, صحيح بټ)
{
      أكد(idx < wid_);

      ثابت بینښه بټونه_فی_کلمه_2 = 8 * حجم(vec_[0]);
      بینښه addr = idx/بټونه_فی_کلمه_2;
      بینښه اوږد ماسک = idx%بټونه_فی_کلمه_2;

      إذا (بټ)
	    vec_[addr] |= 1UL << ماسک;
      وإلا
	    vec_[addr] &= ~(1UL << ماسک);
}

فراغ vvp_vector2_t::سېټ_vec(بینښه adr, ثابت vvp_vector2_t&that)
{
      أكد((adr + that.wid_) <= wid_);

      لكل (بینښه idx = 0 ; idx < that.wid_ ; idx += 1)
	    سېټ_بټ(adr+idx, that.value(idx));
}

منطقي vvp_vector2_t::is_NaN() ثابت
{
      أرجع wid_ == 0;
}

منطقي vvp_vector2_t::is_zero() ثابت
{
      ثابت بینښه ټکي = (wid_ + بټونه_فی_کلمه-1) / بټونه_فی_کلمه;

      لكل (بینښه idx = 0; idx < ټکي; idx += 1) {
	    إذا (vec_[idx] == 0) استمر;
	    أرجع خطأ;
      }

      أرجع ریښتیا;
}

/*
 * Basic idea from "Introduction to Programming using SML" by
 * Michael R. Hansen and Hans Rischel page 261 and "Seminumerical
 * Algorithms, Third Edition" by Donald E. Knuth section 4.6.3.
 */
vvp_vector2_t توان(ثابت vvp_vector2_t&x, vvp_vector2_t&y)
{
        /* If we have a zero exponent just return 1. */
      إذا (y == vvp_vector2_t(0L, 1)) {
	    أرجع vvp_vector2_t(1L, x.size());
      }

        /* Is the value odd? */
      إذا (y.value(0) == 1) {
	    y.سېټ_بټ(0, 0);  // A quick subtract by 1.
	    vvp_vector2_t res = x * توان(x, y);
	    أرجع res;
      }

      y >>= 1;  // A fast divide by two. We know the LSB is zero.
      vvp_vector2_t z = توان(x, y);
      vvp_vector2_t res = z * z;
      أرجع res;
}

ساكن فراغ multiply_long(بینښه اوږد a, بینښه اوږد b,
			  بینښه اوږد&ټیټ, بینښه اوږد&لوړ)
{
      أكد(حجم(بینښه اوږد) %2 == 0);

      ثابت بینښه اوږد کلمه_ماسک = (1UL << 4UL*حجم(a)) - 1UL;
      بینښه اوږد tmpa;
      بینښه اوږد tmpb;
      بینښه اوږد res[4];

      tmpa = a & کلمه_ماسک;
      tmpb = b & کلمه_ماسک;
      res[0] = tmpa * tmpb;
      res[1] = res[0] >> 4UL*حجم(بینښه اوږد);
      res[0] &= کلمه_ماسک;

      tmpa = (a >> 4UL*حجم(بینښه اوږد)) & کلمه_ماسک;
      res[1] += tmpa * tmpb;
      res[2] = res[1] >> 4UL*حجم(بینښه اوږد);
      res[1] &= کلمه_ماسک;

      tmpa = a & کلمه_ماسک;
      tmpb = (b >> 4UL*حجم(بینښه اوږد)) & کلمه_ماسک;
      res[1] += tmpa * tmpb;
      res[2] += res[1] >> 4UL*حجم(بینښه اوږد);
      res[3]  = res[2] >> 4UL*حجم(بینښه اوږد);
      res[1] &= کلمه_ماسک;
      res[2] &= کلمه_ماسک;

      tmpa = (a >> 4UL*حجم(بینښه اوږد)) & کلمه_ماسک;
      res[2] += tmpa * tmpb;
      res[3] += res[2] >> 4UL*حجم(بینښه اوږد);
      res[2] &= کلمه_ماسک;

      لوړ = (res[3] << 4UL*حجم(بینښه اوږد)) | res[2];
      ټیټ  = (res[1] << 4UL*حجم(بینښه اوږد)) | res[0];
}

vvp_vector2_t چلوونکی * (ثابت vvp_vector2_t&a, ثابت vvp_vector2_t&b)
{
      ثابت بینښه بټونه_فی_کلمه_2 = 8 * حجم(a.vec_[0]);

	// The compiler ensures that the two operands are of equal size.
      أكد(a.size() == b.size());
      vvp_vector2_t r (0, a.size());

      بینښه ټکي = (r.wid_ + بټونه_فی_کلمه_2 - 1) / بټونه_فی_کلمه_2;

      لكل (بینښه bdx = 0 ;  bdx < ټکي ;  bdx += 1) {
	    بینښه اوږد tmpb = b.vec_[bdx];
	    إذا (tmpb == 0)
		  استمر;

	    لكل (بینښه adx = 0 ;  adx < ټکي ;  adx += 1) {
		  بینښه اوږد tmpa = a.vec_[adx];
		  إذا (tmpa == 0)
			استمر;

		  بینښه اوږد ټیټ, hig;
		  multiply_long(tmpa, tmpb, ټیټ, hig);

		  بینښه اوږد لېږد = 0;
		  لكل (بینښه sdx = 0
			     ; (adx+bdx+sdx) < ټکي
			     ;  sdx += 1) {

			r.vec_[adx+bdx+sdx] = جمع_کړه_لېږد(r.vec_[adx+bdx+sdx],
							ټیټ, لېږد);
			ټیټ = hig;
			hig = 0;
		  }
	    }
      }

      أرجع r;
}

ساكن فراغ div_mod (vvp_vector2_t dividend, vvp_vector2_t وېشونکی,
		     vvp_vector2_t&quotient, vvp_vector2_t&remainder)
{

      quotient = vvp_vector2_t(0, dividend.size());

      إذا (وېشونکی == quotient) {
	    cerr << "ERROR: division by zero, exiting." << کرښه_پای;
	    وتل(255);
      }

      إذا (dividend < وېشونکی) {
	    remainder = dividend;
	    أرجع;
      }

      vvp_vector2_t ماسک (1, dividend.size());

	// Make the dividend 1 bit larger to prevent overflow of
	// divtmp in startup.
      dividend = vvp_vector2_t(dividend, dividend.size()+1);
      vvp_vector2_t divtmp (وېشونکی, dividend.size());

      طالما (divtmp < dividend) {
	    divtmp <<= 1;
	    ماسک <<= 1;
      }

      طالما (dividend >= وېشونکی) {
	    إذا (divtmp <= dividend) {
		  dividend -= divtmp;
		  quotient += ماسک;
	    }

	    divtmp >>= 1;
	    ماسک >>= 1;
      }

      remainder = vvp_vector2_t(dividend, ماسک.size());
}

vvp_vector2_t چلوونکی - (ثابت vvp_vector2_t&that)
{
      vvp_vector2_t neg(that);
      إذا (neg.wid_ == 0) أرجع neg;

      ثابت بینښه ټکي = (neg.wid_ + neg.بټونه_فی_کلمه-1) /
                                         neg.بټونه_فی_کلمه;
      لكل (بینښه idx = 0 ;  idx < ټکي ;  idx += 1) {
	    neg.vec_[idx] = ~neg.vec_[idx];
      }
      neg += vvp_vector2_t(1, neg.wid_);

      أرجع neg;
}

vvp_vector2_t چلوونکی / (ثابت vvp_vector2_t&dividend,
			  ثابت vvp_vector2_t&وېشونکی)
{
      vvp_vector2_t quot, rem;
      div_mod(dividend, وېشونکی, quot, rem);
      أرجع quot;
}

vvp_vector2_t چلوونکی % (ثابت vvp_vector2_t&dividend,
			  ثابت vvp_vector2_t&وېشونکی)
{
      vvp_vector2_t quot, rem;
      div_mod(dividend, وېشونکی, quot, rem);
      أرجع rem;
}

منطقي چلوونکی > (ثابت vvp_vector2_t&a, ثابت vvp_vector2_t&b)
{
      ثابت بینښه awords = (a.wid_ + vvp_vector2_t::بټونه_فی_کلمه-1) / vvp_vector2_t::بټونه_فی_کلمه;
      ثابت بینښه bwords = (b.wid_ + vvp_vector2_t::بټونه_فی_کلمه-1) / vvp_vector2_t::بټونه_فی_کلمه;

      ثابت بینښه ټکي = awords > bwords? awords : bwords;

      لكل (بینښه idx = ټکي ;  idx > 0 ;  idx -= 1) {
	    بینښه اوږد aw = (idx <= awords)? a.vec_[idx-1] : 0;
	    بینښه اوږد bw = (idx <= bwords)? b.vec_[idx-1] : 0;

	    إذا (aw > bw)
		  أرجع ریښتیا;
	    إذا (aw < bw)
		  أرجع خطأ;
      }

	// If the above loop finishes, then the vectors are equal.
      أرجع خطأ;
}

منطقي چلوونکی >= (ثابت vvp_vector2_t&a, ثابت vvp_vector2_t&b)
{
      ثابت بینښه awords = (a.wid_ + vvp_vector2_t::بټونه_فی_کلمه-1) / vvp_vector2_t::بټونه_فی_کلمه;
      ثابت بینښه bwords = (b.wid_ + vvp_vector2_t::بټونه_فی_کلمه-1) / vvp_vector2_t::بټونه_فی_کلمه;

      ثابت بینښه ټکي = awords > bwords? awords : bwords;

      لكل (بینښه idx = ټکي ;  idx > 0 ;  idx -= 1) {
	    بینښه اوږد aw = (idx <= awords)? a.vec_[idx-1] : 0;
	    بینښه اوږد bw = (idx <= bwords)? b.vec_[idx-1] : 0;

	    إذا (aw > bw)
		  أرجع ریښتیا;
	    إذا (aw < bw)
		  أرجع خطأ;
      }

	// If the above loop finishes, then the vectors are equal.
      أرجع ریښتیا;
}

منطقي چلوونکی < (ثابت vvp_vector2_t&a, ثابت vvp_vector2_t&b)
{
      ثابت بینښه awords = (a.wid_ + vvp_vector2_t::بټونه_فی_کلمه-1) / vvp_vector2_t::بټونه_فی_کلمه;
      ثابت بینښه bwords = (b.wid_ + vvp_vector2_t::بټونه_فی_کلمه-1) / vvp_vector2_t::بټونه_فی_کلمه;

      بینښه ټکي = awords;
      إذا (bwords > ټکي)
	    ټکي = bwords;

      لكل (بینښه idx = ټکي ;  idx > 0 ;  idx -= 1) {
	    بینښه اوږد aw = (idx <= awords)? a.vec_[idx-1] : 0;
	    بینښه اوږد bw = (idx <= bwords)? b.vec_[idx-1] : 0;

	    إذا (aw < bw)
		  أرجع ریښتیا;
	    إذا (aw > bw)
		  أرجع خطأ;
      }

	// If the above loop finishes, then the vectors are equal.
      أرجع خطأ;
}

منطقي چلوونکی <= (ثابت vvp_vector2_t&a, ثابت vvp_vector2_t&b)
{
	// XXXX For now, only support equal width vectors.
      أكد(a.wid_ == b.wid_);

      ثابت بینښه awords = (a.wid_ + vvp_vector2_t::بټونه_فی_کلمه-1) / vvp_vector2_t::بټونه_فی_کلمه;

      لكل (بینښه idx = awords ;  idx > 0 ;  idx -= 1) {
	    إذا (a.vec_[idx-1] < b.vec_[idx-1])
		  أرجع ریښتیا;
	    إذا (a.vec_[idx-1] > b.vec_[idx-1])
		  أرجع خطأ;
      }

	// If the above loop finishes, then the vectors are equal.
      أرجع ریښتیا;
}

منطقي چلوونکی == (ثابت vvp_vector2_t&a, ثابت vvp_vector2_t&b)
{
      ثابت بینښه awords = (a.wid_ + vvp_vector2_t::بټونه_فی_کلمه-1) / vvp_vector2_t::بټونه_فی_کلمه;
      ثابت بینښه bwords = (b.wid_ + vvp_vector2_t::بټونه_فی_کلمه-1) / vvp_vector2_t::بټونه_فی_کلمه;

      ثابت بینښه ټکي = awords > bwords? awords : bwords;

      لكل (بینښه idx = ټکي ;  idx > 0 ;  idx -= 1) {
	    بینښه اوږد aw = (idx <= awords)? a.vec_[idx-1] : 0;
	    بینښه اوږد bw = (idx <= bwords)? b.vec_[idx-1] : 0;

	    إذا (aw > bw)
		  أرجع خطأ;
	    إذا (aw < bw)
		  أرجع خطأ;
      }

	// If the above loop finishes, then the vectors are equal.
      أرجع ریښتیا;
}


vvp_vector4_t vector2_منزل_vector4(ثابت vvp_vector2_t&that, بینښه wid)
{
      vvp_vector4_t res (wid);

      لكل (بینښه idx = 0 ;  idx < res.size() ;  idx += 1) {
	    vvp_bit4_t بټ = BIT4_0;

	    إذا (that.value(idx))
		  بټ = BIT4_1;

	    res.سېټ_بټ(idx, بټ);
      }

      أرجع res;
}

ساكن منطقي c4string_سرلیک_test(ثابت حرف*str)
{
      إذا ((str[0] != 'C') && (str[0] != 'c'))
	    أرجع خطأ;
      إذا ((str[1] != '4') || (str[2] != '<'))
	    أرجع خطأ;
      أرجع ریښتیا;
}

منطقي c4string_test(ثابت حرف*str)
{
      إذا (!c4string_سرلیک_test(str))
	    أرجع خطأ;
      size_t ارزښت_اندازه = strspn(str+3, "01xz");
      إذا (str[3+ارزښت_اندازه] != '>')
	    أرجع خطأ;
      إذا (str[3+ارزښت_اندازه+1] != 0)
	    أرجع خطأ;

      أرجع ریښتیا;
}

vvp_vector4_t c4string_منزل_vector4(ثابت حرف*str)
{
      أكد(c4string_سرلیک_test(str));

      str += 3;
      ثابت حرف*tp = str + strspn(str,"01xz");
      أكد(tp[0] == '>');

      vvp_vector4_t tmp (tp-str);

      لكل (بینښه idx = 0 ;  idx < tmp.size() ;  idx += 1) {
	    vvp_bit4_t بټ;
	    بدل (str[idx]) {
		حالة '0':
		  بټ = BIT4_0;
		  اكسر;
		حالة '1':
		  بټ = BIT4_1;
		  اكسر;
		حالة 'x':
		  بټ = BIT4_X;
		  اكسر;
		حالة 'z':
		  بټ = BIT4_Z;
		  اكسر;
		افتراضي:
		  fprintf(stderr, "Unsupported bit value %c(%d).\n", str[idx],
		          str[idx]);
		  أكد(0);
		  بټ = BIT4_0;
		  اكسر;
	    }
	    tmp.سېټ_بټ(tmp.size()-idx-1, بټ);
      }

      أرجع tmp;
}

ostream& چلوونکی<< (ostream&out, ثابت vvp_vector2_t&that)
{
      إذا (that.is_NaN()) {
	    out << "NaN";

      } وإلا {
	    out << vector2_منزل_vector4(that, that.size());
      }
      أرجع out;
}

vvp_vector8_t::vvp_vector8_t(ثابت vvp_vector8_t&that)
{
      اندازه_2 = that.اندازه_2;
      إذا (اندازه_2 <= حجم(val_)) {
	    حافظه_نقل(val_, that.val_, حجم val_);
      } وإلا {
	    ptr_ = جديد بینښه حرف[اندازه_2];
	    حافظه_نقل(ptr_, that.ptr_, اندازه_2);
      }
}

vvp_vector8_t::vvp_vector8_t(ثابت vvp_vector4_t&that,
			     بینښه str0, بینښه str1)
: اندازه_2(that.size())
{
      إذا (اندازه_2 == 0)
	    أرجع;

      إذا (اندازه_2 <= حجم(val_)) {
	    ptr_ = 0; // Prefill all val_ bytes
	    لكل (بینښه idx = 0 ; idx < اندازه_2 ; idx += 1)
		  val_[idx] = vvp_scalar_t(that.value(idx),str0, str1).raw();
      } وإلا {
	    ptr_ = جديد بینښه حرف[اندازه_2];
	    لكل (بینښه idx = 0 ;  idx < اندازه_2 ;  idx += 1)
		  ptr_[idx] = vvp_scalar_t(that.value(idx), str0, str1).raw();
      }
}

vvp_vector8_t::vvp_vector8_t(ثابت vvp_vector2_t&that,
			     بینښه str0, بینښه str1)
: اندازه_2(that.size())
{
      إذا (اندازه_2 == 0)
	    أرجع;

      إذا (اندازه_2 <= حجم(val_)) {
	    ptr_ = 0;
	    لكل (بینښه idx = 0 ; idx < اندازه_2 ; idx += 1)
		  val_[idx] = vvp_scalar_t(that.value(idx)? BIT4_1:BIT4_0, str0, str1).raw();
      } وإلا {
	    ptr_ = جديد بینښه حرف[اندازه_2];
	    لكل (بینښه idx = 0 ; idx < اندازه_2 ; idx += 1)
		  ptr_[idx] = vvp_scalar_t(that.value(idx)? BIT4_1:BIT4_0, str0, str1).raw();
      }
}

ثابت vvp_vector8_t vvp_vector8_t::nil;

vvp_vector8_t& vvp_vector8_t::چلوونکی= (ثابت vvp_vector8_t&that)
{
	// Assign to self.
      إذا (هذا == &that)
	    أرجع *هذا;

      إذا (اندازه_2 != that.اندازه_2) {
	    إذا (اندازه_2 > حجم(val_))
		  ړنګول[]ptr_;
	    اندازه_2 = 0;
      }

      إذا (that.اندازه_2 == 0) {
	    أكد(اندازه_2 == 0);
	    أرجع *هذا;
      }

      إذا (that.اندازه_2 <= حجم(val_)) {
	    اندازه_2 = that.اندازه_2;
	    حافظه_نقل(val_, that.val_, حجم(val_));
	    أرجع *هذا;
      }

      إذا (اندازه_2 == 0) {
	    اندازه_2 = that.اندازه_2;
	    ptr_ = جديد بینښه حرف[اندازه_2];
      }

      حافظه_نقل(ptr_, that.ptr_, اندازه_2);

      أرجع *هذا;
}

vvp_vector8_t vvp_vector8_t::subvalue(بینښه بنسټ, بینښه wid) ثابت
{
      vvp_vector8_t tmp (wid);

      بینښه حرف*tmp_ptr = tmp.اندازه_2 <= حجم(val_) ? tmp.val_ : tmp.ptr_;
      ثابت بینښه حرف*use_ptr = اندازه_2 <= حجم(val_) ? val_ : ptr_;

      بینښه idx = 0;
      طالما ((idx < wid) && (بنسټ+idx < اندازه_2)) {
	    tmp_ptr[idx] = use_ptr[بنسټ+idx];
	    idx += 1;
      }

      أرجع tmp;
}

فراغ vvp_vector8_t::سېټ_vec(بینښه بنسټ, ثابت vvp_vector8_t&that)
{
      أكد((بنسټ+that.size()) <= size());
      لكل (بینښه idx = 0 ; idx < that.size() ; idx += 1)
	    سېټ_بټ(بنسټ+idx, that.value(idx));
}

vvp_vector8_t part_expand(ثابت vvp_vector8_t&that, بینښه wid, بینښه off)
{
      أكد(off < wid);
      vvp_vector8_t tmp (wid);

      بینښه حرف* tmp_ptr = tmp.اندازه_2<= حجم(tmp.val_) ?
                               tmp.val_ : tmp.ptr_;
      ثابت بینښه حرف* that_ptr = that.اندازه_2<= حجم(that.val_) ?
                                      that.val_ : that.ptr_;

      بینښه idx = off;

      طالما (idx < wid && that.اندازه_2 > (idx-off)) {
	    tmp_ptr[idx] = that_ptr[idx-off];
	    idx += 1;
      }

      أرجع tmp;
}

ساكن منطقي c8string_سرلیک_test(ثابت حرف*str)
{
      إذا ((str[0] != 'C') && (str[0] != 'c'))
	    أرجع خطأ;
      إذا ((str[1] != '8') || (str[2] != '<'))
	    أرجع خطأ;
      أرجع ریښتیا;
}

منطقي c8string_test(ثابت حرف*str)
{
      إذا (!c8string_سرلیک_test(str))
	    أرجع خطأ;

      ثابت حرف*cp = str+3;
      لكل (;; cp += 1) {
	    إذا (cp[0] == '>' && cp[1] == 0) أرجع ریښتیا;
	    إذا (cp[0] >= '0' && cp[0] <= '9') استمر;
	    إذا (cp[0] == 'x') استمر;
	    إذا (cp[0] == 'z') استمر;
	    أرجع خطأ;
      }
      أرجع خطأ;
}
/*
 * The format of a C8<> string is:
 *   C8<aaabbbccc...>
 * where aaa... is a 3 character bit descriptor.
 */
vvp_vector8_t c8string_منزل_vector8(ثابت حرف*str)
{
      أكد(c8string_سرلیک_test(str));

      size_t vsize = مزي_اوږدوالی(str)-4;
      أكد(vsize%3 == 0);
      vsize /= 3;
      vvp_vector8_t tmp (vsize);

      لكل (size_t idx = 0 ; idx < vsize ; idx += 1) {
	    ثابت حرف*cp = str+3+3*idx;
	    vvp_bit4_t بټ = BIT4_X;
	    بینښه dr0 = cp[0]-'0';
	    بینښه dr1 = cp[1]-'0';
	    بدل (cp[2]) {
		حالة '0':
		  بټ = BIT4_0;
		  اكسر;
		حالة '1':
		  بټ = BIT4_1;
		  اكسر;
		حالة 'x':
		  بټ = BIT4_X;
		  اكسر;
		حالة 'z':
		  بټ = BIT4_Z;
		  اكسر;
	    }
	    tmp.سېټ_بټ(vsize-idx-1, vvp_scalar_t(بټ, dr0, dr1));
      }

      أرجع tmp;
}

ostream& چلوونکی<<(ostream&out, ثابت vvp_vector8_t&that)
{
      out << "C8<";
      لكل (بینښه idx = 0 ;  idx < that.size() ; idx += 1)
	    out << that.value(that.size()-idx-1);

      out << ">";
      أرجع out;
}

vvp_net_fun_t::vvp_net_fun_t()
{
      شمېر_functors += 1;
}

vvp_net_fun_t::~vvp_net_fun_t()
{
}

فراغ vvp_net_fun_t::recv_vec4(vvp_net_ptr_t, ثابت vvp_vector4_t&,
                              vvp_context_t)
{
      fprintf(stderr, "internal error: %s: recv_vec4 not implemented\n",
	      typeid(*هذا).name());
      أكد(0);
}

فراغ vvp_net_fun_t::recv_vec4_pv_(vvp_net_ptr_t p, ثابت vvp_vector4_t&بټ,
                                  بینښه بنسټ, بینښه vwid, vvp_context_t)
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

      أكد(بنسټ + بټ.size() <= vwid);

      vvp_vector4_t tmp(vwid, BIT4_Z);
      tmp.سېټ_vec(بنسټ, بټ);
      recv_vec4(p, tmp, 0);
}

فراغ vvp_net_fun_t::recv_vec4_pv(vvp_net_ptr_t, ثابت vvp_vector4_t&بټ,
                                 بینښه بنسټ, بینښه vwid, vvp_context_t)
{
      cerr << "internal error: " << typeid(*هذا).name() << ": "
	   << "recv_vec4_pv(" << بټ << ", " << بنسټ
	   << ", " << vwid << ") not implemented" << کرښه_پای;
      أكد(0);
}

فراغ vvp_net_fun_t::recv_vec8(vvp_net_ptr_t پورټ, ثابت vvp_vector8_t&بټ)
{
      recv_vec4(پورټ, reduce4(بټ), 0);
}

فراغ vvp_net_fun_t::recv_vec8_pv_(vvp_net_ptr_t p, ثابت vvp_vector8_t&بټ,
				  بینښه بنسټ, بینښه vwid)
{
	// This is the strength-aware version of recv_vec4_pv_.

      أكد(بنسټ + بټ.size() <= vwid);

      vvp_vector8_t tmp(vwid);
      tmp.سېټ_vec(بنسټ, بټ);
      recv_vec8(p, tmp);
}

فراغ vvp_net_fun_t::recv_vec8_pv(vvp_net_ptr_t پورټ, ثابت vvp_vector8_t&بټ,
				 بینښه بنسټ, بینښه vwid)
{
      recv_vec4_pv(پورټ, reduce4(بټ), بنسټ, vwid, 0);
}

فراغ vvp_net_fun_t::recv_real(vvp_net_ptr_t, مزدوج بټ, vvp_context_t)
{
      fprintf(stderr, "internal error: %s: recv_real(%f) not implemented\n",
	      typeid(*هذا).name(), بټ);
      أكد(0);
}

فراغ vvp_net_fun_t::recv_string(vvp_net_ptr_t, ثابت std::string&بټ, vvp_context_t)
{
      fprintf(stderr, "internal error: %s: recv_string(%s) not implemented\n",
	      typeid(*هذا).name(), بټ.c_str());
      أكد(0);
}

فراغ vvp_net_fun_t::recv_object(vvp_net_ptr_t, vvp_object_t, vvp_context_t)
{
      fprintf(stderr, "internal error: %s: recv_object(...) not implemented\n",
	      typeid(*هذا).name());
      أكد(0);
}

فراغ vvp_net_fun_t::force_flag(منطقي)
{
}

/* **** vvp_fun_drive methods **** */

vvp_fun_drive::vvp_fun_drive(بینښه str0, بینښه str1)
{
      أكد(str0 < 8);
      أكد(str1 < 8);

      drive0_ = str0;
      drive1_ = str1;
}

vvp_fun_drive::~vvp_fun_drive()
{
}

فراغ vvp_fun_drive::recv_vec4(vvp_net_ptr_t پورټ, ثابت vvp_vector4_t&بټ,
                              vvp_context_t)
{
      أكد(پورټ.پورټ() == 0);
      پورټ.ptr()->send_vec8(vvp_vector8_t(بټ, drive0_, drive1_));
}

فراغ vvp_fun_drive::recv_vec4_pv(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&بټ,
				 بینښه بنسټ, بینښه vwid, vvp_context_t ctx)
{
      recv_vec4_pv_(ptr, بټ, بنسټ, vwid, ctx);
}

/* **** vvp_wide_fun_* methods **** */

vvp_wide_fun_core::vvp_wide_fun_core(vvp_net_t*net, بینښه nports)
{
      ptr_ = net;
      nports_ = nports;
      پورټ_ارزښتونه = 0;
      پورټ_rvalues = 0;
}

vvp_wide_fun_core::~vvp_wide_fun_core()
{
      ړنګول[] پورټ_ارزښتونه;
      ړنګول[] پورټ_rvalues;
}

فراغ vvp_wide_fun_core::propagate_vec4(ثابت vvp_vector4_t&بټ,
				       vvp_time64_t delay)
{
      إذا (delay)
	    مهالوېش_propagate_ویکټور(ptr_, delay, بټ);
      وإلا
	    ptr_->send_vec4(بټ, 0);
}

فراغ vvp_wide_fun_core::propagate_real(مزدوج بټ,
				       vvp_time64_t delay)
{
      إذا (delay) {
	      // schedule_assign_vector(ptr_->out, bit, delay);
	    أكد(0); // Need a real-value version of assign_vector.
      } وإلا {
	    ptr_->send_real(بټ, 0);
      }
}


بینښه vvp_wide_fun_core::پورټ_شمېر() ثابت
{
      أرجع nports_;
}

vvp_vector4_t& vvp_wide_fun_core::value(بینښه idx)
{
      أكد(idx < nports_);
      إذا (پورټ_ارزښتونه == 0)
	    پورټ_ارزښتونه = جديد vvp_vector4_t [nports_];
      أرجع پورټ_ارزښتونه[idx];
}

مزدوج vvp_wide_fun_core::ارزښت_r(بینښه idx)
{
      أكد(idx < nports_);
      أرجع پورټ_rvalues? پورټ_rvalues[idx] : 0.0;
}

فراغ vvp_wide_fun_core::recv_real_from_inputs(بینښه)
{
      أكد(0);
}

فراغ vvp_wide_fun_core::dispatch_vec4_from_ننوت(بینښه پورټ,
						  ثابت vvp_vector4_t&بټ)
{
      أكد(پورټ < nports_);
      إذا (پورټ_ارزښتونه == 0) پورټ_ارزښتونه = جديد vvp_vector4_t [nports_];
      پورټ_ارزښتونه[پورټ] = بټ;
      recv_vec4_from_inputs(پورټ);
}

فراغ vvp_wide_fun_core::dispatch_real_from_ننوت(بینښه پورټ,
						  مزدوج بټ)
{
      أكد(پورټ < nports_);
      إذا (پورټ_rvalues == 0) پورټ_rvalues = جديد مزدوج[nports_];
      پورټ_rvalues[پورټ] = بټ;
      recv_real_from_inputs(پورټ);
}

vvp_wide_fun_t::vvp_wide_fun_t(vvp_wide_fun_core*c, بینښه بنسټ)
: core_(c), پورټ_بنسټ_2(بنسټ)
{
}

vvp_wide_fun_t::~vvp_wide_fun_t()
{
}

فراغ vvp_wide_fun_t::recv_vec4(vvp_net_ptr_t پورټ, ثابت vvp_vector4_t&بټ,
                               vvp_context_t)
{
      بینښه pidx = پورټ_بنسټ_2 + پورټ.پورټ();
      core_->dispatch_vec4_from_ننوت(pidx, بټ);
}

فراغ vvp_wide_fun_t::recv_vec4_pv(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&بټ,
				  بینښه بنسټ, بینښه vwid, vvp_context_t ctx)
{
      recv_vec4_pv_(ptr, بټ, بنسټ, vwid, ctx);
}

فراغ vvp_wide_fun_t::recv_real(vvp_net_ptr_t پورټ, مزدوج بټ,
                               vvp_context_t)
{
      بینښه pidx = پورټ_بنسټ_2 + پورټ.پورټ();
      core_->dispatch_real_from_ننوت(pidx, بټ);
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

بینښه vvp_scalar_t::strength0() ثابت
{
      أرجع STREN0(ارزښت_2);
}

بینښه vvp_scalar_t::strength1() ثابت
{
      أرجع STREN1(ارزښت_2);
}

ostream& چلوونکی <<(ostream&out, vvp_scalar_t a)
{
      out << a.strength0() << a.strength1();
      بدل (a.value()) {
	  حالة BIT4_0:
	    out << "0";
	    اكسر;
	  حالة BIT4_1:
	    out << "1";
	    اكسر;
	  حالة BIT4_X:
	    out << "X";
	    اكسر;
	  حالة BIT4_Z:
	    out << "Z";
	    اكسر;
      }
      أرجع out;
}

/*
 * This function is only called if the actual interface function rules
 * out some of the easy cases. If we get here, we can assume that
 * neither of the values is HiZ, and the values are not exactly equal.
 */
vvp_scalar_t fully_featured_resolv_(vvp_scalar_t a, vvp_scalar_t b)
{

      إذا (UNAMBIG(a.ارزښت_2) && UNAMBIG(b.ارزښت_2)) {

	      /* If both signals are unambiguous, simply choose
		 the stronger. If they have the same strength
		 but different values, then this becomes
		 ambiguous. */

	    إذا ((b.ارزښت_2&0x07) > (a.ارزښت_2&0x07)) {

		    /* b value is stronger. Take it. */
		  أرجع b;

	    } وإلا إذا ((b.ارزښت_2&0x77) == (a.ارزښت_2&0x77)) {

		    // Strengths are the same. Since we know already
		    // that the values are not the same, Make value
		    // into "x".
		  vvp_scalar_t tmp (a);
		  tmp.ارزښت_2 = (tmp.ارزښت_2&0x77) | 0x80;
		  أرجع tmp;

	    } وإلا {

		    /* Must be "a" is the stronger one. */
		  أرجع a;
	    }

      }

	/* If one of the signals is unambiguous, then it
	   will sweep up the weaker parts of the ambiguous
	   signal. The result may be ambiguous, or maybe not. */

      إذا (UNAMBIG(a.ارزښت_2)) {
	    vvp_scalar_t res;

	    إذا ((a.ارزښت_2&0x70) > (b.ارزښت_2&0x70))
		  res.ارزښت_2 |= a.ارزښت_2&0xf0;
	    وإلا
		  res.ارزښت_2 |= b.ارزښت_2&0xf0;

	    إذا ((a.ارزښت_2&0x07) > (b.ارزښت_2&0x07))
		  res.ارزښت_2 |= a.ارزښت_2&0x0f;
	    وإلا
		  res.ارزښت_2 |= b.ارزښت_2&0x0f;

	    أرجع res;

      } وإلا إذا (UNAMBIG(b.ارزښت_2)) {

	    vvp_scalar_t res;

	    إذا ((b.ارزښت_2&0x70) > (a.ارزښت_2&0x70))
		  res.ارزښت_2 |= b.ارزښت_2&0xf0;
	    وإلا
		  res.ارزښت_2 |= a.ارزښت_2&0xf0;

	    إذا ((b.ارزښت_2&0x07) > (a.ارزښت_2&0x07))
		  res.ارزښت_2 |= b.ارزښت_2&0x0f;
	    وإلا
		  res.ارزښت_2 |= a.ارزښت_2&0x0f;

	    أرجع res;

      }


	/* If both signals are ambiguous, then the result
	   has an even wider ambiguity. */

      بینښه tmp = 0;
      صحيح sv1a = (a.ارزښت_2 & 0x80) ? STREN1(a.ارزښت_2) : - STREN1(a.ارزښت_2);
      صحيح sv0a = (a.ارزښت_2 & 0x08) ? STREN0(a.ارزښت_2) : - STREN0(a.ارزښت_2);
      صحيح sv1b = (b.ارزښت_2 & 0x80) ? STREN1(b.ارزښت_2) : - STREN1(b.ارزښت_2);
      صحيح sv0b = (b.ارزښت_2 & 0x08) ? STREN0(b.ارزښت_2) : - STREN0(b.ارزښت_2);

      صحيح sv1 = sv1a;
      صحيح sv0 = sv0a;

      إذا (sv0a > sv1)
	    sv1 = sv0a;
      إذا (sv1b > sv1)
	    sv1 = sv1b;
      إذا (sv0b > sv1)
	    sv1 = sv0b;

      إذا (sv1a < sv0)
	    sv0 = sv1a;
      إذا (sv1b < sv0)
	    sv0 = sv1b;
      إذا (sv0b < sv0)
	    sv0 = sv0b;

      إذا (sv1 > 0) {
	    tmp |= 0x80;
	    tmp |= sv1 << 4;
      } وإلا {
	      /* Set the MSB when both arguments MSBs are set. This
		 can only happen if both one strengths are zero. */
	    tmp |= (a.ارزښت_2&b.ارزښت_2)&0x80;
	    tmp |= (-sv1) << 4;
      }

      إذا (sv0 > 0) {
	    tmp |= 0x08;
	    tmp |= sv0;
      } وإلا {
	    tmp |= (-sv0);
      }

      vvp_scalar_t res;
      res.ارزښت_2 = tmp;

	/* Canonicalize the HiZ value. */
      إذا ((res.ارزښت_2&0x77) == 0)
	    res.ارزښت_2 = 0;

      أرجع res;
}

بینښه vvp_switch_strength_map[2][8] = {
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

vvp_vector4_t reduce4(ثابت vvp_vector8_t&that)
{
      vvp_vector4_t out (that.size());
      لكل (بینښه idx = 0 ;  idx < out.size() ;  idx += 1)
	    out.سېټ_بټ(idx, that.value(idx).value());

      أرجع out;
}

vvp_bit4_t compare_gtge(ثابت vvp_vector4_t&lef, ثابت vvp_vector4_t&rig,
			vvp_bit4_t بهر_if_equal)
{
      بینښه min_اندازه = lef.size();
      إذا (rig.size() < min_اندازه)
	    min_اندازه = rig.size();

	// If one of the inputs is nil, treat is as all X values, and
	// that makes the result BIT4_X.
      إذا (min_اندازه == 0)
	    أرجع BIT4_X;

	// As per the IEEE1364 definition of >, >=, < and <=, if there
	// are any X or Z values in either of the operand vectors,
	// then the result of the compare is BIT4_X.

	// Check for X/Z in the left operand
      إذا (lef.has_xz())
	    أرجع BIT4_X;

	// Check for X/Z in the right operand
      إذا (rig.has_xz())
	    أرجع BIT4_X;

      لكل (بینښه idx = lef.size() ; idx > rig.size() ;  idx -= 1) {
	    إذا (lef.value(idx-1) == BIT4_1)
		  أرجع BIT4_1;
      }

      لكل (بینښه idx = rig.size() ; idx > lef.size() ;  idx -= 1) {
	    إذا (rig.value(idx-1) == BIT4_1)
		  أرجع BIT4_0;
      }

      لكل (بینښه idx = min_اندازه ; idx > 0 ;  idx -= 1) {
	    vvp_bit4_t lv = lef.value(idx-1);
	    vvp_bit4_t rv = rig.value(idx-1);

	    إذا (lv == rv)
		  استمر;

	    إذا (lv == BIT4_1)
		  أرجع BIT4_1;
	    وإلا
		  أرجع BIT4_0;
      }

      أرجع بهر_if_equal;
}

vvp_bit4_t compare_gtge_signed(ثابت vvp_vector4_t&a,
			       ثابت vvp_vector4_t&b,
			       vvp_bit4_t بهر_if_equal)
{
      أكد(a.size() == b.size());

      بینښه sign_idx = a.size()-1;
      vvp_bit4_t a_sign = a.value(sign_idx);
      vvp_bit4_t b_sign = b.value(sign_idx);

      إذا (bit4_is_xz(a_sign))
	    أرجع BIT4_X;
      إذا (bit4_is_xz(b_sign))
	    أرجع BIT4_X;

      إذا (a_sign == b_sign)
	    أرجع compare_gtge(a, b, بهر_if_equal);

      إذا (a.has_xz())
	    أرجع BIT4_X;

      إذا (b.has_xz())
	    أرجع BIT4_X;

      إذا(a_sign == BIT4_0)
	    أرجع BIT4_1;
      وإلا
	    أرجع BIT4_0;
}
