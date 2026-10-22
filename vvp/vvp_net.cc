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
#اگر_تعریف_شدہ CHECK_WITH_VALGRIND
# include  <valgrind/memcheck.h>
# include  <map>
# include  "sfunc.h"
# include  "udp.h"
# include  "ivl_alloc.h"
#اگر_ختم

استعمال نامخلا std;

/* This is the size of an unsigned long in bits. This is just a
   convenience macro. */
# define CPU_لفظ_بٹ (8*حجم(غیرعلامتی طویل))
# define چوٹی_ثنائی_ہندسہ (1UL << (CPU_لفظ_بٹ-1))

permaheap vvp_net_fun_t::ڈھیر;
permaheap vvp_net_fil_t::ڈھیر;

// Allocate around 1Megabyte/chunk.
ساکن ثابت size_t VVP_NET_CHUNK = 1024*1024/حجم(vvp_net_t);
ساکن vvp_net_t*vvp_net_alloc_جدول = تہی;
#اگر_تعریف_شدہ CHECK_WITH_VALGRIND
ساکن vvp_net_t **vvp_net_pool = تہی;
ساکن غیرعلامتی vvp_net_pool_گنتی = 0;
#اگر_ختم
ساکن size_t vvp_net_alloc_بقایا = 0;
// For statistics, count the vvp_nets allocated and the bytes of alloc
// chunks allocated.
غیرعلامتی طویل گنتی_vvp_nets = 0;
size_t سائز_vvp_nets = 0;

خالی* vvp_net_t::عامل نیا (size_t size)
{
      أكد(size == حجم(vvp_net_t));
      اگر (vvp_net_alloc_بقایا == 0) {
	    vvp_net_alloc_جدول = ::نیا vvp_net_t[VVP_NET_CHUNK];
	    vvp_net_alloc_بقایا = VVP_NET_CHUNK;
	    سائز_vvp_nets += size*VVP_NET_CHUNK;
#اگر_تعریف_شدہ CHECK_WITH_VALGRIND
	    VALGRIND_MAKE_MEM_NOACCESS(vvp_net_alloc_جدول, size*VVP_NET_CHUNK);
	    VALGRIND_CREATE_MEMPOOL(vvp_net_alloc_جدول, 0, 0);
	    vvp_net_pool_گنتی += 1;
	    vvp_net_pool = static_cast<vvp_net_t **>(realloc(vvp_net_pool,
	                   vvp_net_pool_گنتی*حجم(vvp_net_t **)));
	    vvp_net_pool[vvp_net_pool_گنتی-1] = vvp_net_alloc_جدول;
#اگر_ختم
      }

      vvp_net_t*return_this = vvp_net_alloc_جدول;
#اگر_تعریف_شدہ CHECK_WITH_VALGRIND
      VALGRIND_MEMPOOL_ALLOC(vvp_net_pool[vvp_net_pool_گنتی-1],
                             return_this, size);
      return_this->pool = vvp_net_pool[vvp_net_pool_گنتی-1];
#اگر_ختم
      vvp_net_alloc_جدول += 1;
      vvp_net_alloc_بقایا -= 1;
      گنتی_vvp_nets += 1;
      واپس return_this;
}

#اگر_تعریف_شدہ CHECK_WITH_VALGRIND
ساکن map<vvp_net_t*, منطقی> vvp_net_map;
ساکن map<sfunc_core*, منطقی> sfunc_map;
ساکن map<vvp_udp_fun_core*, منطقی> udp_map;
ساکن map<resolv_core*, منطقی> resolv_map;
ساکن vvp_net_t **local_net_pool = 0;
ساکن غیرعلامتی local_net_pool_گنتی = 0;

خالی pool_local_net(vvp_net_t*net)
{
      local_net_pool_گنتی += 1;
      local_net_pool = static_cast<vvp_net_t **>(realloc(local_net_pool,
                       local_net_pool_گنتی*حجم(vvp_net_t **)));
      local_net_pool[local_net_pool_گنتی-1] = net;
}

خالی vvp_net_delete(vvp_net_t *شے)
{
      vvp_net_map[شے] = سچ;
      اگر (sfunc_core*tmp = dynamic_cast<sfunc_core*> (شے->fun)) {
	    sfunc_map[tmp] = سچ;
      }
      اگر (vvp_udp_fun_core*tmp = dynamic_cast<vvp_udp_fun_core*> (شے->fun)) {
	    udp_map[tmp] = سچ;
      }
      اگر (resolv_core*tmp = dynamic_cast<resolv_core*> (شے->fun)) {
	    resolv_map[tmp] = سچ;
      }
	/* Only delete static object variables. */
      اگر (vvp_fun_signal_object_sa*tmp = dynamic_cast<vvp_fun_signal_object_sa*> (شے->fun)) {
	    حذف tmp;
      }
	/* Only delete static string variables. */
      اگر (vvp_fun_signal_string_sa*tmp = dynamic_cast<vvp_fun_signal_string_sa*> (شے->fun)) {
	    حذف tmp;
      }
}

خالی vvp_net_pool_delete()
{
      غیرعلامتی طویل vvp_nets_del = 0;

      برائے (غیرعلامتی idx = 0; idx < local_net_pool_گنتی; idx += 1) {
	    vvp_net_delete(local_net_pool[idx]);
      }
      آزاد(local_net_pool);
      local_net_pool = 0;
      local_net_pool_گنتی = 0;

      map<vvp_net_t*, منطقی>::iterator iter;
      برائے (iter = vvp_net_map.begin(); iter != vvp_net_map.انتہا(); ++ iter ) {
	    vvp_nets_del += 1;
	    VALGRIND_MEMPOOL_FREE(iter->پہلا->pool, iter->پہلا);
      }
      vvp_net_map.clear();

      map<sfunc_core*, منطقی>::iterator siter;
      برائے (siter = sfunc_map.begin(); siter != sfunc_map.انتہا(); ++ siter ) {
	    حذف siter->پہلا;
      }
      sfunc_map.clear();

      map<vvp_udp_fun_core*, منطقی>::iterator uiter;
      برائے (uiter = udp_map.begin(); uiter != udp_map.انتہا(); ++ uiter ) {
	    حذف uiter->پہلا;
      }
      udp_map.clear();

      map<resolv_core*, منطقی>::iterator riter;
      برائے (riter = resolv_map.begin(); riter != resolv_map.انتہا(); ++ riter ) {
	    حذف riter->پہلا;
      }
      resolv_map.clear();

      اگر (RUNNING_ON_VALGRIND && (vvp_nets_del != گنتی_vvp_nets)) {
	    fflush(تہی);
	    VALGRIND_PRINTF("Error: vvp missed deleting %ld of %lu net(s).",
	                    (طویل) گنتی_vvp_nets - vvp_nets_del,
	                    گنتی_vvp_nets);
      }

      برائے (غیرعلامتی idx = 0; idx < vvp_net_pool_گنتی; idx += 1) {
	    VALGRIND_DESTROY_MEMPOOL(vvp_net_pool[idx]);
	    ::حذف [] vvp_net_pool[idx];
      }
      آزاد(vvp_net_pool);
      vvp_net_pool = تہی;
      vvp_net_pool_گنتی = 0;
}
#اگر_ختم

خالی vvp_net_t::عامل حذف(خالی*)
{
      أكد(0);
}

vvp_net_t::vvp_net_t()
: برآمد_2(vvp_net_ptr_t(0,0))
{
      fun = 0;
      fil = 0;
}

خالی vvp_net_t::link(vvp_net_ptr_t پورٹ_مقصد_link)
{
      vvp_net_t*net = پورٹ_مقصد_link.ptr();

	// Connect nodes with vvp_fun_modpath_src to the head of the
	// linked list, so that vvp_fun_modpath_src are always evaluated
	// before their respective vvp_fun_modpath
      اگر (dynamic_cast<vvp_fun_modpath_src*>(net->fun)) {
	    net->پورٹ[پورٹ_مقصد_link.پورٹ()] = برآمد_2;
	    برآمد_2 = پورٹ_مقصد_link;
	// All other nodes connect after the last vvp_fun_modpath_src
      } ورنہ {
	      // List is empty or first node has no vvp_fun_modpath_src
	      // then just insert the node into the head of linked list
	    اگر (برآمد_2.ptr() == خالی_اشاریہ || ! dynamic_cast<vvp_fun_modpath_src*>(برآمد_2.ptr()->fun)) {
		  net->پورٹ[پورٹ_مقصد_link.پورٹ()] = برآمد_2;
		  برآمد_2 = پورٹ_مقصد_link;
		  واپس;
	    }

	    vvp_net_ptr_t cur = برآمد_2;
	    vvp_net_ptr_t prev = vvp_net_ptr_t(خالی_اشاریہ,0);

	      // Traverse through linked list
	    جب (dynamic_cast<vvp_fun_modpath_src*>(cur.ptr()->fun)) {
		  prev = cur;
		  cur = cur.ptr()->پورٹ[cur.پورٹ()];
		  اگر (! cur.ptr()) توڑو; // End of list
	    }

	    أكد(prev.ptr());

	      // Insert after last vvp_fun_modpath_src (or end of list)
	    net->پورٹ[پورٹ_مقصد_link.پورٹ()] = cur;
	    prev.ptr()->پورٹ[prev.پورٹ()] = پورٹ_مقصد_link;
      }
}

/*
 * Unlink a ptr object from the driver. The input is the driver in the
 * form of a vvp_net_t pointer. The .out member of that object is the
 * driver. The dst_ptr argument is the receiver pin to be located and
 * removed from the fan-out list.
 */
خالی vvp_net_t::unlink(vvp_net_ptr_t dst_ptr)
{
      vvp_net_t*net = dst_ptr.ptr();
      غیرعلامتی net_پورٹ = dst_ptr.پورٹ();

      اگر (برآمد_2 == dst_ptr) {
	      /* If the drive fan-out list starts with this pointer,
		 then the unlink is easy. Pull the list forward. */
	    برآمد_2 = net->پورٹ[net_پورٹ];
      } ورنہ اگر (! برآمد_2.nil()) {
	      /* Scan the linked list, looking for the net_ptr_t
		 pointer *before* the one we wish to remove. */
	    vvp_net_ptr_t cur = برآمد_2;
	    أكد(!cur.nil());
	    vvp_net_t*cur_net = cur.ptr();
	    غیرعلامتی cur_پورٹ = cur.پورٹ();
	    جب (cur_net && cur_net->پورٹ[cur_پورٹ] != dst_ptr) {
		  cur = cur_net->پورٹ[cur_پورٹ];
		  cur_net = cur.ptr();
		  cur_پورٹ = cur.پورٹ();
	    }
	      /* Unlink. */
	    اگر (cur_net) cur_net->پورٹ[cur_پورٹ] = net->پورٹ[net_پورٹ];
      }

      net->پورٹ[net_پورٹ] = vvp_net_ptr_t(0,0);
}

خالی vvp_net_t::گنتی_drivers(غیرعلامتی idx, غیرعلامتی counts[4])
{
      counts[0] = 0;
      counts[1] = 0;
      counts[2] = 0;
      counts[3] = 0;

        /* $countdrivers can only be used on wires. */
      vvp_wire_بنیاد*wire=dynamic_cast<vvp_wire_بنیاد*>(fil);
      أكد(wire);

      اگر (wire->is_forced(idx))
            counts[3] = 1;

        /* If the net has multiple drivers, we need to interrogate the
           resolver network to get the driven values. */
      اگر (resolv_core*resolver = dynamic_cast<resolv_core*>(fun)) {
            resolver->گنتی_drivers(idx, counts);
            واپس;
      }
      اگر (vvp_island_پورٹ*resolver = dynamic_cast<vvp_island_پورٹ*>(fun)) {
            resolver->گنتی_drivers(idx, counts);
            واپس;
      }

        /* If the functor is not a resolver, there is only one driver, so
           we can just interrogate the filter to get the driven value. */
      update_driver_counts(wire->driven_قدر(idx), counts);
}

خالی vvp_net_fun_t::عامل حذف(خالی*)
{
#اگر_غیر_تعریف_شدہ CHECK_WITH_VALGRIND
      أكد(0);
#اگر_ختم
}


vvp_net_fil_t::vvp_net_fil_t()
{
      force_link_ = 0;
      force_propagate_ = جھوٹ;
      گنتی_filters += 1;
}

vvp_net_fil_t::~vvp_net_fil_t()
{
      أكد(force_link_ == 0);
}

vvp_net_fil_t::prop_t vvp_net_fil_t::filter_vec4(ثابت vvp_vector4_t&,
                                                 vvp_vector4_t&,
                                                 غیرعلامتی, غیرعلامتی)
{
      واپس PROP;
}

vvp_net_fil_t::prop_t vvp_net_fil_t::filter_vec8(ثابت vvp_vector8_t&,
                                                 vvp_vector8_t&,
                                                 غیرعلامتی, غیرعلامتی)
{
      واپس PROP;
}

vvp_net_fil_t::prop_t vvp_net_fil_t::filter_real(دوہرا&)
{
      واپس PROP;
}

vvp_net_fil_t::prop_t vvp_net_fil_t::filter_string(ثابت string&)
{
      واپس PROP;
}

vvp_net_fil_t::prop_t vvp_net_fil_t::filter_object(vvp_object_t&)
{
      واپس PROP;
}

خالی vvp_net_fil_t::force_نقاب(ثابت vvp_vector2_t&نقاب)
{
      اگر (force_نقاب_2.size() == 0)
	    force_نقاب_2 = vvp_vector2_t(vvp_vector2_t::FILL0, نقاب.size());

      أكد(force_نقاب_2.size() == نقاب.size());
      برائے (غیرعلامتی idx = 0 ; idx < نقاب.size() ; idx += 1) {
	    اگر (نقاب.value(idx) == 0)
		  جاری;

	    force_نقاب_2.سیٹ_ثنائی_ہندسہ(idx, 1);
	    force_propagate_ = سچ;
      }
}

خالی vvp_net_fil_t::release_نقاب(ثابت vvp_vector2_t&نقاب)
{
      اگر (force_نقاب_2.size() == 0)
	    واپس;

      أكد(force_نقاب_2.size() == نقاب.size());
      برائے (غیرعلامتی idx = 0 ; idx < نقاب.size() ; idx += 1) {
	    اگر (نقاب.value(idx))
		  force_نقاب_2.سیٹ_ثنائی_ہندسہ(idx, 0);
      }

      اگر (force_نقاب_2.is_zero())
	    force_نقاب_2 = vvp_vector2_t();
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
خالی vvp_net_fil_t::force_link(vvp_net_t*dst, vvp_net_t*src)
{
      أكد(dst->fil == یہ);

      اگر (force_link_ == 0) {
	    force_link_ = نیا vvp_net_t;
	      // Use port[3] to hold the force destination.
	    force_link_->پورٹ[3] = vvp_net_ptr_t(dst, 0);
	    force_link_->پورٹ[2] = vvp_net_ptr_t(0,0);
	    force_link_->fun = نیا vvp_fun_force;
      }

      force_unlink();
      أكد(force_link_->پورٹ[2] == vvp_net_ptr_t(0,0));

	// Use port[2] to hold the force source.
      force_link_->پورٹ[2] = vvp_net_ptr_t(src,0);

      vvp_net_ptr_t dst_ptr(force_link_, 0);
      src->link(dst_ptr);
}

خالی vvp_net_fil_t::عامل حذف(خالی*)
{
      أكد(0);
}

خالی vvp_net_fil_t::force_unlink(خالی)
{
      اگر (force_link_ == 0) واپس;
      vvp_net_t*src = force_link_->پورٹ[2].ptr();
      اگر (src == 0) واپس;

      src->unlink(vvp_net_ptr_t(force_link_,0));
      force_link_->پورٹ[2] = vvp_net_ptr_t(0,0);
}

/* *** BIT operations *** */
vvp_bit4_t جوڑو_with_حاصل(vvp_bit4_t a, vvp_bit4_t b, vvp_bit4_t&c)
{
      اگر (bit4_is_xz(a) || bit4_is_xz(b) || bit4_is_xz(c)) {
	    c = BIT4_X;
	    واپس BIT4_X;
      }

	// NOTE: This relies on the facts that XZ values have been
	// weeded out, and that BIT4_1 is 1 and BIT4_0 is 0.
      صحیح مجموع = (صحیح)a + (صحیح)b + (صحیح)c;

      انتخاب (مجموع) {
	  حالت 0:
	      // c must already be 0.
	    واپس BIT4_0;
	  حالت 1:
	    c = BIT4_0;
	    واپس BIT4_1;
	  حالت 2:
	    c = BIT4_1;
	    واپس BIT4_0;
	  حالت 3:
	    c = BIT4_1;
	    واپس BIT4_1;
      }
      fprintf(stderr, "Incorrect result %d.\n", مجموع);
      أكد(0);
      واپس BIT4_X;
}

vvp_bit4_t scalar_مقصد_bit4(PLI_INT32 scalar)
{
      انتخاب(scalar) {
	  حالت vpi0:
	    واپس BIT4_0;
	  حالت vpi1:
	    واپس BIT4_1;
	  حالت vpiX:
	    واپس BIT4_X;
	  حالت vpiZ:
	    واپس BIT4_Z;
      }
      fprintf(stderr, "Unsupported scalar value %d.\n", (صحیح)scalar);
      أكد(0);
      واپس BIT4_X;
}

vvp_bit4_t عامل ^ (vvp_bit4_t a, vvp_bit4_t b)
{
      اگر (bit4_is_xz(a))
	    واپس BIT4_X;
      اگر (bit4_is_xz(b))
	    واپس BIT4_X;
      اگر (a == BIT4_0)
	    واپس b;
      اگر (b == BIT4_0)
	    واپس a;
      واپس BIT4_0;
}

ostream& عامل<<(ostream&out, vvp_bit4_t ثنائی_ہندسہ)
{
      انتخاب (ثنائی_ہندسہ) {
	  حالت BIT4_0:
	    out << "0";
	    توڑو;
	  حالت BIT4_1:
	    out << "1";
	    توڑو;
	  حالت BIT4_X:
	    out << "X";
	    توڑو;
	  حالت BIT4_Z:
	    out << "Z";
	    توڑو;
	  طےشدہ:
	    out << "?";
	    توڑو;
      }
      واپس out;
}

قسمتعریف غیرعلامتی مختصر کنارہ_t;

اندرونی کنارہ_t VVP_کنارہ(vvp_bit4_t from, vvp_bit4_t to)
{
      واپس 1 << ((from << 2) | to);
}

ثابت کنارہ_t vvp_کنارہ_posedge
      = VVP_کنارہ(BIT4_0,BIT4_1)
      | VVP_کنارہ(BIT4_0,BIT4_X)
      | VVP_کنارہ(BIT4_0,BIT4_Z)
      | VVP_کنارہ(BIT4_X,BIT4_1)
      | VVP_کنارہ(BIT4_Z,BIT4_1)
      ;

ثابت کنارہ_t vvp_کنارہ_negedge
      = VVP_کنارہ(BIT4_1,BIT4_0)
      | VVP_کنارہ(BIT4_1,BIT4_X)
      | VVP_کنارہ(BIT4_1,BIT4_Z)
      | VVP_کنارہ(BIT4_X,BIT4_0)
      | VVP_کنارہ(BIT4_Z,BIT4_0)
      ;

صحیح کنارہ(vvp_bit4_t from, vvp_bit4_t to)
{
      کنارہ_t نقاب = VVP_کنارہ(from, to);
      اگر (نقاب & vvp_کنارہ_posedge)
	    واپس 1;
      اگر (نقاب & vvp_کنارہ_negedge)
	    واپس -1;
      واپس 0;
}

غیرعلامتی طویل multiply_with_حاصل(غیرعلامتی طویل a, غیرعلامتی طویل b,
				  غیرعلامتی طویل&حاصل)
{
      ثابت غیرعلامتی طویل نقاب = (1UL << (CPU_لفظ_بٹ/2)) - 1;
      غیرعلامتی طویل a0 = a & نقاب;
      غیرعلامتی طویل a1 = (a >> (CPU_لفظ_بٹ/2)) & نقاب;
      غیرعلامتی طویل b0 = b & نقاب;
      غیرعلامتی طویل b1 = (b >> (CPU_لفظ_بٹ/2)) & نقاب;

      غیرعلامتی طویل tmp = a0 * b0;

      غیرعلامتی طویل r00 = tmp & نقاب;
      غیرعلامتی طویل c00 = (tmp >> (CPU_لفظ_بٹ/2)) & نقاب;

      tmp = a0 * b1;

      غیرعلامتی طویل r01 = tmp & نقاب;
      غیرعلامتی طویل c01 = (tmp >> (CPU_لفظ_بٹ/2)) & نقاب;

      tmp = a1 * b0;

      غیرعلامتی طویل r10 = tmp & نقاب;
      غیرعلامتی طویل c10 = (tmp >> (CPU_لفظ_بٹ/2)) & نقاب;

      tmp = a1 * b1;

      غیرعلامتی طویل r11 = tmp & نقاب;
      غیرعلامتی طویل c11 = (tmp >> (CPU_لفظ_بٹ/2)) & نقاب;

      غیرعلامتی طویل r1 = c00 + r01 + r10;
      غیرعلامتی طویل r2 = (r1 >> (CPU_لفظ_بٹ/2)) & نقاب;
      r1 &= نقاب;
      r2 += c01 + c10 + r11;
      غیرعلامتی طویل r3 = (r2 >> (CPU_لفظ_بٹ/2)) & نقاب;
      r2 &= نقاب;
      r3 += c11;
      r3 &= نقاب;

      حاصل = (r3 << (CPU_لفظ_بٹ/2)) + r2;
      واپس (r1 << (CPU_لفظ_بٹ/2)) + r00;
}


خالی vvp_send_vec8(vvp_net_ptr_t ptr, ثابت vvp_vector8_t&val)
{
      جب (vvp_net_t*cur = ptr.ptr()) {
	    vvp_net_ptr_t اگلا_val = cur->پورٹ[ptr.پورٹ()];

	    اگر (cur->fun)
		  cur->fun->recv_vec8(ptr, val);

	    ptr = اگلا_val;
      }
}

خالی vvp_send_real(vvp_net_ptr_t ptr, دوہرا val, vvp_context_t context)
{
      جب (vvp_net_t*cur = ptr.ptr()) {
	    vvp_net_ptr_t اگلا_val = cur->پورٹ[ptr.پورٹ()];

	    اگر (cur->fun)
		  cur->fun->recv_real(ptr, val, context);

	    ptr = اگلا_val;
      }
}

ثابت vvp_vector4_t vvp_vector4_t::nil;

خالی vvp_vector4_t::copy_بٹ(ثابت vvp_vector4_t&that)
{

      اگر (سائز_2 == that.سائز_2) {
	    اگر (سائز_2 > بٹ_فی_لفظ) {
		  غیرعلامتی الفاظ = (سائز_2+بٹ_فی_لفظ-1) / بٹ_فی_لفظ;
		  برائے (غیرعلامتی idx = 0 ;  idx < الفاظ ;  idx += 1)
			abits_ptr_[idx] = that.abits_ptr_[idx];
		  برائے (غیرعلامتی idx = 0 ;  idx < الفاظ ;  idx += 1)
			bbits_ptr_[idx] = that.bbits_ptr_[idx];
	    } ورنہ {
		  abits_val_ = that.abits_val_;
		  bbits_val_ = that.bbits_val_;
	    }
	    واپس;
      }

	/* Now we know that the sizes of this and that are definitely
	   different. We can use that in code below. In any case, we
	   need to copy only the smaller of the sizes. */

	/* If source and destination are both short, then mask/copy
	   the bit values. */
      اگر (سائز_2 <= بٹ_فی_لفظ && that.سائز_2 <= بٹ_فی_لفظ) {
	    غیرعلامتی بٹ_مقصد_copy = (that.سائز_2 < سائز_2) ? that.سائز_2 : سائز_2;
	    غیرعلامتی طویل نقاب = ((بٹ_مقصد_copy < بٹ_فی_لفظ) ? (1UL << بٹ_مقصد_copy) :
	                                                           (0UL)) - 1UL;
	    abits_val_ &= ~نقاب;
	    bbits_val_ &= ~نقاب;
	    abits_val_ |= that.abits_val_&نقاب;
	    bbits_val_ |= that.bbits_val_&نقاب;
	    واپس;
      }

	/* Now we know that either source or destination are long. If
	   the destination is short, then mask/copy from the low word
	   of the long source. */
      اگر (سائز_2 <= بٹ_فی_لفظ) {
	    abits_val_ = that.abits_ptr_[0];
	    bbits_val_ = that.bbits_ptr_[0];
	    اگر (سائز_2 < بٹ_فی_لفظ) {
		  غیرعلامتی طویل نقاب = (1UL << سائز_2) - 1UL;
		  abits_val_ &= نقاب;
		  bbits_val_ &= نقاب;
	    }
	    واپس;
      }

	/* Now we know that the destination must be long. If the
	   source is short, then mask/copy from its value. */
      اگر (that.سائز_2 <= بٹ_فی_لفظ) {
	    غیرعلامتی طویل نقاب;
	    اگر (that.سائز_2 < بٹ_فی_لفظ) {
		  نقاب = (1UL << that.سائز_2) - 1UL;
		  abits_ptr_[0] &= ~نقاب;
		  bbits_ptr_[0] &= ~نقاب;
	    } ورنہ {
		  نقاب = -1UL;
	    }
	    abits_ptr_[0] |= that.abits_val_&نقاب;
	    bbits_ptr_[0] |= that.bbits_val_&نقاب;
	    واپس;
      }

	/* Finally, we know that source and destination are long. copy
	   words until we get to the last. */
      غیرعلامتی بٹ_مقصد_copy = (that.سائز_2 < سائز_2) ? that.سائز_2 : سائز_2;
      غیرعلامتی لفظ = 0;
      جب (بٹ_مقصد_copy >= بٹ_فی_لفظ) {
	    abits_ptr_[لفظ] = that.abits_ptr_[لفظ];
	    bbits_ptr_[لفظ] = that.bbits_ptr_[لفظ];
	    بٹ_مقصد_copy -= بٹ_فی_لفظ;
	    لفظ += 1;
      }
      اگر (بٹ_مقصد_copy > 0) {
	    غیرعلامتی طویل نقاب = (1UL << بٹ_مقصد_copy) - 1UL;
	    abits_ptr_[لفظ] &= ~نقاب;
	    bbits_ptr_[لفظ] &= ~نقاب;
	    abits_ptr_[لفظ] |= that.abits_ptr_[لفظ] & نقاب;
	    bbits_ptr_[لفظ] |= that.bbits_ptr_[لفظ] & نقاب;
      }
}

/*
 * This function should ONLY BE CALLED FROM vvp_vector4_t::copy_from_,
 * as it performs part of that functions tasks.
 */
خالی vvp_vector4_t::copy_from_big_(ثابت vvp_vector4_t&that)
{
      غیرعلامتی الفاظ = (سائز_2+بٹ_فی_لفظ-1) / بٹ_فی_لفظ;
      abits_ptr_ = نیا غیرعلامتی طویل[2*الفاظ];
      bbits_ptr_ = abits_ptr_ + الفاظ;

      برائے (غیرعلامتی idx = 0 ;  idx < الفاظ ;  idx += 1)
	    abits_ptr_[idx] = that.abits_ptr_[idx];
      برائے (غیرعلامتی idx = 0 ;  idx < الفاظ ;  idx += 1)
	    bbits_ptr_[idx] = that.bbits_ptr_[idx];
}

/*
 * The copy_inverted_from_ method is just like the copy_from_ method,
 * except that we combine that with an invert. This allows the ~ and
 * the assignment to be blended in many common cases.
 */
خالی vvp_vector4_t::copy_inverted_from_(ثابت vvp_vector4_t&that)
{
      سائز_2 = that.سائز_2;
      اگر (سائز_2 > بٹ_فی_لفظ) {
	    غیرعلامتی الفاظ = (سائز_2+بٹ_فی_لفظ-1) / بٹ_فی_لفظ;
	    abits_ptr_ = نیا غیرعلامتی طویل[2*الفاظ];
	    bbits_ptr_ = abits_ptr_ + الفاظ;

	    غیرعلامتی بقایا = سائز_2;
	    غیرعلامتی idx = 0;
	    جب (بقایا >= بٹ_فی_لفظ) {
		  abits_ptr_[idx] = that.bbits_ptr_[idx] | ~that.abits_ptr_[idx];
		  idx += 1;
		  بقایا -= بٹ_فی_لفظ;
	    }
	    اگر (بقایا > 0) {
		  غیرعلامتی طویل نقاب = (1UL<<بقایا) - 1UL;
		  abits_ptr_[idx] = نقاب & (that.bbits_ptr_[idx] | ~that.abits_ptr_[idx]);
	    }

	    برائے (idx = 0 ;  idx < الفاظ ;  idx += 1)
		  bbits_ptr_[idx] = that.bbits_ptr_[idx];

      } ورنہ {
	    غیرعلامتی طویل نقاب = (سائز_2<بٹ_فی_لفظ)? (1UL<<سائز_2)-1UL : -1UL;
	    abits_val_ = نقاب & (that.bbits_val_ | ~that.abits_val_);
	    bbits_val_ = that.bbits_val_;
      }
}

/* Make sure to set size_ before calling this routine. */
خالی vvp_vector4_t::allocate_الفاظ(غیرعلامتی طویل inita, غیرعلامتی طویل initb)
{
      اگر (سائز_2 > بٹ_فی_لفظ) {
	    غیرعلامتی cnt = (سائز_2 + بٹ_فی_لفظ - 1) / بٹ_فی_لفظ;
	    abits_ptr_ = نیا غیرعلامتی طویل[2*cnt];
	    bbits_ptr_ = abits_ptr_ + cnt;
	    برائے (غیرعلامتی idx = 0 ;  idx < cnt ;  idx += 1)
		  abits_ptr_[idx] = inita;
	    برائے (غیرعلامتی idx = 0 ;  idx < cnt ;  idx += 1)
		  bbits_ptr_[idx] = initb;

      } ورنہ {
	    abits_val_ = inita;
	    bbits_val_ = initb;
      }
}

vvp_vector4_t::vvp_vector4_t(غیرعلامتی سائز_3, دوہرا val)
: سائز_2(سائز_3)
{
      منطقی is_neg = جھوٹ;
      دوہرا fraction;
      صحیح exponent;

	/* We return 'bx for a NaN or +/- infinity. */
      اگر (val != val || (val && (val == 0.5*val)))  {
	    allocate_الفاظ(لفظ_X_ABITS, لفظ_X_BBITS);
	    واپس;
      }

	/* Convert to a positive result. */
      اگر (val < 0.0) {
	    is_neg = سچ;
	    val = -val;
      }
      allocate_الفاظ(لفظ_0_ABITS, لفظ_0_BBITS);

	/* Get the exponent and fractional part of the number. */
      fraction = frexp(val, &exponent);

	/* If the value is small enough just use lround(). */
      اگر (exponent < بٹ_فی_لفظ-2) {
	    اگر (is_neg) یہ->invert();  // Invert the bits if negative.
	    طویل sval = lround(val);
	    اگر (is_neg) sval = -sval;
	      /* This requires that 0 and 1 have the same bbit value. */
	    اگر (سائز_2 > بٹ_فی_لفظ) {
		  abits_ptr_[0] = sval;
	    } ورنہ {
		  abits_val_ = sval;
	    }
	    واپس;
      }

      غیرعلامتی nwords = (exponent-1) / بٹ_فی_لفظ;
      غیرعلامتی my_الفاظ = (سائز_2 + بٹ_فی_لفظ - 1) / بٹ_فی_لفظ - 1;

      fraction = ldexp(fraction, (exponent-1) % بٹ_فی_لفظ + 1);

	/* Skip any leading bits. */
      برائے (صحیح idx = (علامتی) nwords; idx > (علامتی) my_الفاظ; idx -=1) {
	    غیرعلامتی طویل بٹ = (غیرعلامتی طویل) fraction;
	    fraction = fraction - (دوہرا) بٹ;
	    fraction = ldexp(fraction, بٹ_فی_لفظ);
      }

	/* Convert the remaining bits as appropriate. */
      اگر (my_الفاظ == 0) {
		  غیرعلامتی طویل بٹ = (غیرعلامتی طویل) fraction;
		  abits_val_ = بٹ;
		  fraction = fraction - (دوہرا) بٹ;
		    /* Round any fractional part up. */
		  اگر (fraction >= 0.5) *یہ += (int64_t) 1;
      } ورنہ {
	    اگر (nwords < my_الفاظ) my_الفاظ = nwords;
	    برائے (صحیح idx = (علامتی)my_الفاظ; idx >= 0; idx -= 1) {
		  غیرعلامتی طویل بٹ = (غیرعلامتی طویل) fraction;
		  abits_ptr_[idx] = بٹ;
		  fraction = fraction - (دوہرا) بٹ;
		  fraction = ldexp(fraction, بٹ_فی_لفظ);
	    }
	      /* Round any fractional part up. */
	    اگر (fraction >= ldexp(0.5, بٹ_فی_لفظ)) *یہ += (int64_t) 1;
       }

	/* Convert to a negative number if needed. */
      اگر (is_neg) {
	    یہ->invert();
	    *یہ += (int64_t) 1;
      }
}

vvp_vector4_t::vvp_vector4_t(ثابت vvp_vector4_t&that,
			    غیرعلامتی adr, غیرعلامتی wid)
{
	// Set up and initialize the destination.
      سائز_2 = wid;
      allocate_الفاظ(لفظ_X_ABITS, لفظ_X_BBITS);

	// Special case: selecting from far beyond the source vector,
	// to the result is all X bits. We're done.
      اگر (adr >= that.سائز_2)
	    واپس;

	// Special case: The source is not quite big enough to supply
	// all bits, so get the bits that we can. The remainder will
	// be left at BIT4_X.
      اگر ((adr + wid) > that.سائز_2) {
	    غیرعلامتی use_wid = that.سائز_2 - adr;
	    برائے (غیرعلامتی idx = 0 ; idx < use_wid ; idx += 1)
		  سیٹ_ثنائی_ہندسہ(idx, that.value(adr+idx));

	    واپس;
      }

	// At the point, we know that the source part is entirely
	// contained in the source vector.
	// assert((adr + wid) <= that.size_);

      اگر (wid > بٹ_فی_لفظ) {
	      /* In this case, the subvector and the source vector are
		 long. Do the transfer reasonably efficiently. */
	    غیرعلامتی ptr = adr / بٹ_فی_لفظ;
	    غیرعلامتی طویل off = adr % بٹ_فی_لفظ;
	    غیرعلامتی طویل noff = بٹ_فی_لفظ - off;
	    غیرعلامتی طویل lmask = (1UL << off) - 1UL;
	    غیرعلامتی trans = 0;
	    غیرعلامتی dst = 0;
	    جب (trans < wid) {
		    // The low bits of the result.
		  abits_ptr_[dst] = (that.abits_ptr_[ptr] & ~lmask) >> off;
		  bbits_ptr_[dst] = (that.bbits_ptr_[ptr] & ~lmask) >> off;
		  trans += noff;

		  اگر (trans >= wid)
			توڑو;

		  ptr += 1;

		    // The high bits of the result. Skip this if the
		    // source and destination are perfectly aligned.
		  اگر (noff != بٹ_فی_لفظ) {
			abits_ptr_[dst] |= (that.abits_ptr_[ptr]&lmask) << noff;
			bbits_ptr_[dst] |= (that.bbits_ptr_[ptr]&lmask) << noff;
			trans += off;
		  }

		  dst += 1;
	    }

      } ورنہ اگر (that.سائز_2 > بٹ_فی_لفظ) {
	      /* In this case, the subvector fits in a single word,
		 but the source is large. */
	    غیرعلامتی ptr = adr / بٹ_فی_لفظ;
	    غیرعلامتی طویل off = adr % بٹ_فی_لفظ;
	    غیرعلامتی trans = بٹ_فی_لفظ - off;
	    اگر (trans > wid)
		  trans = wid;

	    اگر (trans == بٹ_فی_لفظ) {
		    // Very special case: Copy exactly 1 perfectly
		    // aligned word.
		  abits_val_ = that.abits_ptr_[ptr];
		  bbits_val_ = that.bbits_ptr_[ptr];

	    } ورنہ {
		    // lmask is the low bits of the destination,
		    // masked into the source.
		  غیرعلامتی طویل lmask = (1UL<<trans) - 1UL;
		  lmask <<= off;

		    // The low bits of the result.
		  abits_val_ = (that.abits_ptr_[ptr] & lmask) >> off;
		  bbits_val_ = (that.bbits_ptr_[ptr] & lmask) >> off;

		  اگر (trans < wid) {
			  // If there are more bits, then get them
			  // from the bottom of the next word of the
			  // source.
			غیرعلامتی طویل hmask = (1UL << (wid-trans)) - 1UL;

			  // The high bits of the result.
			abits_val_ |= (that.abits_ptr_[ptr+1]&hmask) << trans;
			bbits_val_ |= (that.bbits_ptr_[ptr+1]&hmask) << trans;
		  }
	    }

      } ورنہ اگر (سائز_2 == بٹ_فی_لفظ) {
	      /* We know that source and destination are short. If the
		 destination is a full word, then we know the copy is
		 aligned and complete. */
	    abits_val_ = that.abits_val_;
	    bbits_val_ = that.bbits_val_;

      } ورنہ {
	      /* Finally, the source and destination vectors are both
		 short, so there is a single mask/shift/copy. */
	    غیرعلامتی طویل نقاب = (1UL << سائز_2) - 1UL;
	    نقاب <<= adr;

	    abits_val_ = (that.abits_val_ & نقاب) >> adr;
	    bbits_val_ = (that.bbits_val_ & نقاب) >> adr;
      }

}

/*
 * Change the size of the vvp_vector4_t vector to the new size. Copy
 * the old values, as many as well fit, into the new vector.
 */
خالی vvp_vector4_t::resize(غیرعلامتی newsize, vvp_bit4_t pad_ثنائی_ہندسہ)
{
      اگر (سائز_2 == newsize)
	    واپس;

      غیرعلامتی cnt = (سائز_2 + بٹ_فی_لفظ - 1) / بٹ_فی_لفظ;
      غیرعلامتی طویل لفظ_pad_abits = 0;
      غیرعلامتی طویل لفظ_pad_bbits = 0;
      انتخاب (pad_ثنائی_ہندسہ) {
	  حالت BIT4_0:
	    لفظ_pad_abits = لفظ_0_ABITS;
	    لفظ_pad_bbits = لفظ_0_BBITS;
	    توڑو;
	  حالت BIT4_1:
	    لفظ_pad_abits = لفظ_1_ABITS;
	    لفظ_pad_bbits = لفظ_1_BBITS;
	    توڑو;
	  حالت BIT4_X:
	    لفظ_pad_abits = لفظ_X_ABITS;
	    لفظ_pad_bbits = لفظ_X_BBITS;
	    توڑو;
	  حالت BIT4_Z:
	    لفظ_pad_abits = لفظ_Z_ABITS;
	    لفظ_pad_bbits = لفظ_Z_BBITS;
	    توڑو;
      }

      اگر (newsize > بٹ_فی_لفظ) {
	    غیرعلامتی newcnt = (newsize + بٹ_فی_لفظ - 1) / بٹ_فی_لفظ;
	    اگر (newcnt == cnt) {
		    // If the word count doesn't change, then there is
		    // no need for re-allocation so we are done now.
		  اگر (newsize > سائز_2) {
			اگر (غیرعلامتی fill = سائز_2 % بٹ_فی_لفظ) {
			      abits_ptr_[cnt-1] &= ~((-1UL) << fill);
			      bbits_ptr_[cnt-1] &= ~((-1UL) << fill);
			      abits_ptr_[cnt-1] |= لفظ_pad_abits << fill;
			      bbits_ptr_[cnt-1] |= لفظ_pad_bbits << fill;
			}
		  }
		  سائز_2 = newsize;
		  واپس;
	    }

	    غیرعلامتی طویل*newbits = نیا غیرعلامتی طویل[2*newcnt];

	    اگر (cnt > 1) {
		  غیرعلامتی trans = cnt;
		  اگر (trans > newcnt)
			trans = newcnt;

		  برائے (غیرعلامتی idx = 0 ;  idx < trans ;  idx += 1)
			newbits[idx] = abits_ptr_[idx];
		  برائے (غیرعلامتی idx = 0 ;  idx < trans ;  idx += 1)
			newbits[newcnt+idx] = bbits_ptr_[idx];

		  حذف[]abits_ptr_;

	    } ورنہ {
		  newbits[0] = abits_val_;
		  newbits[newcnt] = bbits_val_;
	    }

	    اگر (newsize > سائز_2) {
		  اگر (غیرعلامتی fill = سائز_2 % بٹ_فی_لفظ) {
			newbits[cnt-1] &= ~((-1UL) << fill);
			newbits[cnt-1] |= لفظ_pad_abits << fill;
			newbits[newcnt+cnt-1] &= ~((-1UL) << fill);
			newbits[newcnt+cnt-1] |= لفظ_pad_bbits << fill;
		  }
		  برائے (غیرعلامتی idx = cnt ;  idx < newcnt ;  idx += 1)
			newbits[idx] = لفظ_pad_abits;
		  برائے (غیرعلامتی idx = cnt ;  idx < newcnt ;  idx += 1)
			newbits[newcnt+idx] = لفظ_pad_bbits;
	    }

	    سائز_2 = newsize;
	    abits_ptr_ = newbits;
	    bbits_ptr_ = newbits + newcnt;

      } ورنہ {
	    اگر (cnt > 1) {
		  غیرعلامتی طویل newvala = abits_ptr_[0];
		  غیرعلامتی طویل newvalb = bbits_ptr_[0];
		  حذف[]abits_ptr_;
		  abits_val_ = newvala;
		  bbits_val_ = newvalb;
	    }

	    اگر (newsize > سائز_2) {
		  abits_val_ &= ~((-1UL) << سائز_2);
		  bbits_val_ &= ~((-1UL) << سائز_2);
		  abits_val_ |= لفظ_pad_abits << سائز_2;
		  bbits_val_ |= لفظ_pad_bbits << سائز_2;
	    }

	    سائز_2 = newsize;
      }
}


غیرعلامتی طویل* vvp_vector4_t::subarray(غیرعلامتی adr, غیرعلامتی wid, منطقی xz_مقصد_0) ثابت
{
      ثابت غیرعلامتی BIT2_فی_لفظ = 8*حجم(غیرعلامتی طویل);
      غیرعلامتی awid = (wid + BIT2_فی_لفظ - 1) / (BIT2_فی_لفظ);
      غیرعلامتی طویل*val = نیا غیرعلامتی طویل[awid];

      برائے (غیرعلامتی idx = 0 ;  idx < awid ;  idx += 1)
	    val[idx] = 0;

      اگر (سائز_2 <= بٹ_فی_لفظ) {
	      /* Handle the special case that the array is small. The
		 entire value of the vector4 is within the xbits_val_
		 so we know that the result is a single word, the
		 source is a single word, and we just have to loop
		 through that word. */
	    غیرعلامتی طویل atmp = abits_val_ >> adr;
	    غیرعلامتی طویل btmp = bbits_val_ >> adr;
	    اگر (wid < BIT2_فی_لفظ) {
		  atmp &= (1UL << wid) - 1;
		  btmp &= (1UL << wid) - 1;
	    }
	    اگر (btmp) {
		  اگر (xz_مقصد_0)
			atmp &= ~btmp;
		  ورنہ
			جاؤ x_برآمد;
	    }

	    val[0] = atmp;

      } ورنہ {

	    غیرعلامتی val_ptr = 0;
	    غیرعلامتی val_off = 0;

	      /* Get the first word we are scanning. We may in fact be
		 somewhere in the middle of that word. */
	    جب (wid > 0) {
		  غیرعلامتی طویل atmp = abits_ptr_[adr/بٹ_فی_لفظ];
		  غیرعلامتی طویل btmp = bbits_ptr_[adr/بٹ_فی_لفظ];
		  غیرعلامتی طویل off = adr%بٹ_فی_لفظ;
		  atmp >>= off;
		  btmp >>= off;

		  غیرعلامتی طویل trans = بٹ_فی_لفظ - off;
		  اگر (trans > (BIT2_فی_لفظ - val_off))
			trans = BIT2_فی_لفظ - val_off;
		  اگر (wid < trans)
			trans = wid;
		  اگر (trans < BIT2_فی_لفظ) {
			atmp &= (1UL << trans) - 1;
			btmp &= (1UL << trans) - 1;
		  }
		  اگر (btmp) {
			اگر (xz_مقصد_0)
			      atmp &= ~btmp;
			ورنہ
			      جاؤ x_برآمد;
		  }

		  val[val_ptr] |= atmp << val_off;
		  adr += trans;
		  wid -= trans;
		  val_off += trans;
		  اگر (val_off == BIT2_فی_لفظ) {
			val_ptr += 1;
			val_off = 0;
		  }
	    }
      }

      واپس val;

 x_برآمد:
      حذف[]val;
      واپس 0;
}

خالی vvp_vector4_t::setarray(غیرعلامتی adr, غیرعلامتی wid, ثابت غیرعلامتی طویل*val)
{
      أكد(adr+wid <= سائز_2);

      ثابت غیرعلامتی BIT2_فی_لفظ = 8*حجم(غیرعلامتی طویل);

      اگر (سائز_2 <= بٹ_فی_لفظ) {
	      // We know here that both the source and the target are
	      // within a single word. Write the bits into the
	      // abits_val_ directly.

	    أكد(BIT2_فی_لفظ <= بٹ_فی_لفظ);
	    غیرعلامتی طویل lmask = (1UL << adr) - 1UL;
	    غیرعلامتی طویل hmask = ((adr+wid) < بٹ_فی_لفظ)
		  ? -1UL << (adr+wid)
		  : 0;
	    غیرعلامتی طویل نقاب = ~(hmask | lmask);

	    abits_val_ &= ~نقاب;
	    bbits_val_ &= ~نقاب;

	    abits_val_ |= نقاب & (val[0] << adr);

      } ورنہ {
	      // The general case, there are multiple words of
	      // destination, and possibly multiple words of source
	      // data. Shift and mask as we go.
	    غیرعلامتی off = adr % بٹ_فی_لفظ;
	    غیرعلامتی ptr = adr / بٹ_فی_لفظ;
	    غیرعلامتی val_off = 0;
	    غیرعلامتی val_ptr = 0;
	    جب (wid > 0) {
		  غیرعلامتی trans = wid;
		  اگر (trans > (BIT2_فی_لفظ-val_off))
			trans = BIT2_فی_لفظ-val_off;
		  اگر (trans > (بٹ_فی_لفظ-off))
			trans = بٹ_فی_لفظ-off;

		  غیرعلامتی طویل lmask = (1UL << off) - 1UL;
		  غیرعلامتی طویل hmask = ((off+trans) < بٹ_فی_لفظ)
			? -1UL << (off+trans)
			: 0;
		  غیرعلامتی طویل نقاب = ~(hmask | lmask);

		  abits_ptr_[ptr] &= ~نقاب;
		  bbits_ptr_[ptr] &= ~نقاب;
		  اگر (val_off >= off)
			abits_ptr_[ptr] |= نقاب & (val[val_ptr] >> (val_off-off));
		  ورنہ
			abits_ptr_[ptr] |= نقاب & (val[val_ptr] << (off-val_off));

		  wid -= trans;
		  val_off += trans;
		  اگر (val_off == BIT2_فی_لفظ) {
			val_off = 0;
			val_ptr += 1;
		  }
		  off += trans;
		  اگر (off == بٹ_فی_لفظ) {
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
منطقی vvp_vector4_t::سیٹ_vec(غیرعلامتی adr, ثابت vvp_vector4_t&that)
{
      أكد(adr+that.سائز_2  <= سائز_2);
      منطقی diff_flag = جھوٹ;

      اگر (سائز_2 <= بٹ_فی_لفظ) {

	      /* The destination vector (me!) is within a bits_val_
		 word, so the subvector is certainly within a
		 bits_val_ word. Therefore, the entire operation is a
		 matter of writing the bits of that into the addressed
		 bits of this. The mask below is calculated to be 1
		 for all the bits that are to come from that. Do the
		 job by some shifting, masking and OR. */

	    غیرعلامتی طویل lmask = (1UL << adr) - 1;
	    غیرعلامتی طویل hmask;
	    غیرعلامتی طویل hshift = adr+that.سائز_2;
	    اگر (hshift >= بٹ_فی_لفظ)
		  hmask = -1UL;
	    ورنہ
		  hmask = (1UL << (adr+that.سائز_2)) - 1;
	    غیرعلامتی طویل نقاب = hmask & ~lmask;

	    غیرعلامتی طویل tmp = (that.abits_val_<<adr)&نقاب;
	    اگر ((abits_val_&نقاب) != tmp) {
		  diff_flag = سچ;
		  abits_val_ = (abits_val_ & ~نقاب) | tmp;
	    }
	    tmp = (that.bbits_val_<<adr) & نقاب;
	    اگر ((bbits_val_&نقاب) != tmp) {
		  diff_flag = سچ;
		  bbits_val_ = (bbits_val_ & ~نقاب) | tmp;
	    }

      } ورنہ اگر (that.سائز_2 <= بٹ_فی_لفظ) {

	      /* This vector is more than a word, but that vector is
		 still small. Write into the destination, possibly
		 spanning two destination words, depending on whether
		 the source vector spans a word transition. */
	    غیرعلامتی طویل dptr = adr / بٹ_فی_لفظ;
	    غیرعلامتی طویل doff = adr % بٹ_فی_لفظ;

	    غیرعلامتی طویل lmask = (1UL << doff) - 1;
	    غیرعلامتی طویل hshift = doff+that.سائز_2;
	    غیرعلامتی طویل hmask;
	    اگر (hshift >= بٹ_فی_لفظ)
		  hmask = -1UL;
	    ورنہ
		  hmask = (1UL << hshift) - 1UL;

	    غیرعلامتی طویل نقاب = hmask & ~lmask;
	    غیرعلامتی طویل tmp;

	    tmp = (that.abits_val_ << doff) & نقاب;
	    اگر ((abits_ptr_[dptr] & نقاب) != tmp) {
		  diff_flag = سچ;
		  abits_ptr_[dptr] = (abits_ptr_[dptr] & ~نقاب) | tmp;
	    }
	    tmp = (that.bbits_val_ << doff) & نقاب;
	    اگر ((bbits_ptr_[dptr] & نقاب) != tmp) {
		  diff_flag = سچ;
		  bbits_ptr_[dptr] = (bbits_ptr_[dptr] & ~نقاب) | tmp;
	    }

	    اگر ((doff + that.سائز_2) > بٹ_فی_لفظ) {
		  غیرعلامتی دم = doff + that.سائز_2 - بٹ_فی_لفظ;
		  نقاب = (1UL << دم) - 1;

		  dptr += 1;
		  tmp = (that.abits_val_ >> (that.سائز_2-دم)) & نقاب;
		  اگر ((abits_ptr_[dptr] & نقاب) != tmp) {
			diff_flag = سچ;
			abits_ptr_[dptr] = (abits_ptr_[dptr] & ~نقاب) | tmp;
		  }
		  tmp = (that.bbits_val_ >> (that.سائز_2-دم)) & نقاب;
		  اگر ((bbits_ptr_[dptr] & نقاب) != tmp) {
			diff_flag = سچ;
			bbits_ptr_[dptr] = (bbits_ptr_[dptr] & ~نقاب) | tmp;
		  }
	    }

      } ورنہ اگر (adr%بٹ_فی_لفظ == 0) {

	      /* In this case, both vectors are long, but the
		 destination is neatly aligned. That means all but the
		 last word can be simply copied with no masking. */

	    غیرعلامتی remain = that.سائز_2;
	    غیرعلامتی sptr = 0;
	    غیرعلامتی dptr = adr / بٹ_فی_لفظ;
	    جب (remain >= بٹ_فی_لفظ) {
		  اگر (abits_ptr_[dptr] != that.abits_ptr_[sptr]) {
			diff_flag = سچ;
			abits_ptr_[dptr] = that.abits_ptr_[sptr];
		  }
		  اگر (bbits_ptr_[dptr] != that.bbits_ptr_[sptr]) {
			diff_flag = سچ;
			bbits_ptr_[dptr] = that.bbits_ptr_[sptr];
		  }
		  dptr += 1;
		  sptr += 1;
		  remain -= بٹ_فی_لفظ;
	    }

	    اگر (remain > 0) {
		  غیرعلامتی طویل نقاب = (1UL << remain) - 1;
		  غیرعلامتی طویل tmp;

		  tmp = that.abits_ptr_[sptr] & نقاب;
		  اگر ((abits_ptr_[dptr] & نقاب) != tmp) {
			diff_flag = سچ;
			abits_ptr_[dptr] = (abits_ptr_[dptr] & ~نقاب) | tmp;
		  }
		  tmp = that.bbits_ptr_[sptr] & نقاب;
		  اگر ((bbits_ptr_[dptr] & نقاب) != tmp) {
			diff_flag = سچ;
			bbits_ptr_[dptr] = (bbits_ptr_[dptr] & ~نقاب) | tmp;
		  }
	    }

      } ورنہ {

	      /* We know that there are two long vectors, and we know
		 that the destination is definitely NOT aligned. */

	    غیرعلامتی remain = that.سائز_2;
	    غیرعلامتی sptr = 0;
	    غیرعلامتی dptr = adr / بٹ_فی_لفظ;
	    غیرعلامتی doff = adr % بٹ_فی_لفظ;
	    غیرعلامتی طویل lmask = (1UL << doff) - 1;
	    غیرعلامتی ndoff = بٹ_فی_لفظ - doff;
	    جب (remain >= بٹ_فی_لفظ) {
		  غیرعلامتی طویل tmp;

		  tmp = (that.abits_ptr_[sptr] << doff) & ~lmask;
		  اگر ((abits_ptr_[dptr] & ~lmask) != tmp) {
			diff_flag = سچ;
			abits_ptr_[dptr] = (abits_ptr_[dptr] & lmask) | tmp;
		  }
		  tmp = (that.bbits_ptr_[sptr] << doff) & ~lmask;
		  اگر ((bbits_ptr_[dptr] & ~lmask) != tmp) {
			diff_flag = سچ;
			bbits_ptr_[dptr] = (bbits_ptr_[dptr] & lmask) | tmp;
		  }
		  dptr += 1;

		  tmp = (that.abits_ptr_[sptr] >> ndoff) & lmask;
		  اگر ((abits_ptr_[dptr] & lmask) != tmp) {
			diff_flag = سچ;
			abits_ptr_[dptr] = (abits_ptr_[dptr] & ~lmask) | tmp;
		  }
		  tmp = (that.bbits_ptr_[sptr] >> ndoff) & lmask;
		  اگر ((bbits_ptr_[dptr] & lmask) != tmp) {
			diff_flag = سچ;
			bbits_ptr_[dptr] = (bbits_ptr_[dptr] & ~lmask) | tmp;
		  }

		  remain -= بٹ_فی_لفظ;
		  sptr += 1;
	    }

	    اگر (remain > 0) {
		  غیرعلامتی طویل hshift = doff+remain;
		  غیرعلامتی طویل hmask;
		  اگر (hshift >= بٹ_فی_لفظ)
			hmask = -1UL;
		  ورنہ
			hmask = (1UL << (doff+remain)) - 1;

		  غیرعلامتی طویل نقاب = hmask & ~lmask;
		  غیرعلامتی طویل tmp;

		  tmp = (that.abits_ptr_[sptr] << doff) & نقاب;
		  اگر ((abits_ptr_[dptr] & نقاب) != tmp) {
			diff_flag = سچ;
			abits_ptr_[dptr] = (abits_ptr_[dptr] & ~نقاب) | tmp;
		  }
		  tmp = (that.bbits_ptr_[sptr] << doff) & نقاب;
		  اگر ((bbits_ptr_[dptr] & نقاب) != tmp) {
			diff_flag = سچ;
			bbits_ptr_[dptr] = (bbits_ptr_[dptr] & ~نقاب) | tmp;
		  }

		  اگر ((doff + remain) > بٹ_فی_لفظ) {
			غیرعلامتی دم = doff + remain - بٹ_فی_لفظ;
			اگر (دم >= بٹ_فی_لفظ)
			      نقاب = -1UL;
			ورنہ
			      نقاب = (1UL << دم) - 1;

			dptr += 1;

			tmp = (that.abits_ptr_[sptr] >> (remain-دم))&نقاب;
			اگر ((abits_ptr_[dptr] & نقاب) != tmp) {
			      diff_flag = سچ;
			      abits_ptr_[dptr] = (abits_ptr_[dptr] & ~نقاب) | tmp;
			}
			tmp = (that.bbits_ptr_[sptr] >> (remain-دم))&نقاب;
			اگر ((bbits_ptr_[dptr] & نقاب) != tmp) {
			      diff_flag = سچ;
			      bbits_ptr_[dptr] = (bbits_ptr_[dptr] & ~نقاب) | tmp;
			}
		  }
	    }
      }

      واپس diff_flag;
}

/*
 * Add that vector to this vector. Do it in the Verilog way, which
 * means if we detect any X or Z bits, change the entire results to
 * all X.
 *
 * Assume both vectors are the same size.
 */
خالی vvp_vector4_t::جوڑو(ثابت vvp_vector4_t&that)
{
      أكد(سائز_2 == that.سائز_2);

      اگر (سائز_2 < بٹ_فی_لفظ) {
	    غیرعلامتی طویل نقاب = ~(-1UL << سائز_2);
	    اگر ((bbits_val_|that.bbits_val_) & نقاب) {
		  abits_val_ |= نقاب;
		  bbits_val_ |= نقاب;
		  واپس;
	    }

	    abits_val_ += that.abits_val_;
	    abits_val_ &= نقاب;
	    واپس;
      }

      اگر (سائز_2 == بٹ_فی_لفظ) {
	    اگر (bbits_val_ | that.bbits_val_) {
		  abits_val_ = لفظ_X_ABITS;
		  bbits_val_ = لفظ_X_BBITS;
	    } ورنہ {
		  abits_val_ += that.abits_val_;
	    }
	    واپس;
      }

      صحیح cnt = سائز_2 / بٹ_فی_لفظ;
      غیرعلامتی طویل حاصل = 0;
      برائے (صحیح idx = 0 ; idx < cnt ; idx += 1) {
	    اگر (bbits_ptr_[idx] | that.bbits_ptr_[idx])
		  جاؤ x_برآمد;

	    abits_ptr_[idx] = جوڑو_with_حاصل(abits_ptr_[idx], that.abits_ptr_[idx], حاصل);
      }

      اگر (غیرعلامتی دم = سائز_2 % بٹ_فی_لفظ) {
	    غیرعلامتی طویل نقاب = ~( -1UL << دم );
	    اگر ((bbits_ptr_[cnt] | that.bbits_ptr_[cnt])&نقاب)
		  جاؤ x_برآمد;

	    abits_ptr_[cnt] = جوڑو_with_حاصل(abits_ptr_[cnt], that.abits_ptr_[cnt], حاصل);
	    abits_ptr_[cnt] &= نقاب;
      }

      واپس;

 x_برآمد:
      برائے (صحیح idx = 0 ; idx < cnt ; idx += 1) {
	    abits_ptr_[idx] = لفظ_X_ABITS;
	    bbits_ptr_[idx] = لفظ_X_BBITS;
      }
      اگر (غیرعلامتی دم = سائز_2%بٹ_فی_لفظ) {
	    غیرعلامتی طویل نقاب = ~( -1UL << دم );
	    abits_ptr_[cnt] = لفظ_X_ABITS&نقاب;
	    bbits_ptr_[cnt] = لفظ_X_BBITS&نقاب;
      }
}

خالی vvp_vector4_t::sub(ثابت vvp_vector4_t&that)
{
      أكد(سائز_2 == that.سائز_2);

      اگر (سائز_2 < بٹ_فی_لفظ) {
	    غیرعلامتی طویل نقاب = ~(-1UL << سائز_2);
	    اگر ((bbits_val_|that.bbits_val_) & نقاب) {
		  abits_val_ |= نقاب;
		  bbits_val_ |= نقاب;
		  واپس;
	    }

	    abits_val_ -= that.abits_val_;
	    abits_val_ &= نقاب;
	    واپس;
      }

      اگر (سائز_2 == بٹ_فی_لفظ) {
	    اگر (bbits_val_ | that.bbits_val_) {
		  abits_val_ = لفظ_X_ABITS;
		  bbits_val_ = لفظ_X_BBITS;
	    } ورنہ {
		  abits_val_ -= that.abits_val_;
	    }
	    واپس;
      }

      صحیح cnt = سائز_2 / بٹ_فی_لفظ;
      غیرعلامتی طویل حاصل = 1;
      برائے (صحیح idx = 0 ; idx < cnt ; idx += 1) {
	    اگر (bbits_ptr_[idx] | that.bbits_ptr_[idx])
		  جاؤ x_برآمد;

	    abits_ptr_[idx] = جوڑو_with_حاصل(abits_ptr_[idx], ~that.abits_ptr_[idx], حاصل);
      }

      اگر (غیرعلامتی دم = سائز_2 % بٹ_فی_لفظ) {
	    غیرعلامتی طویل نقاب = ~( -1UL << دم );
	    اگر ((bbits_ptr_[cnt] | that.bbits_ptr_[cnt])&نقاب)
		  جاؤ x_برآمد;

	    abits_ptr_[cnt] = جوڑو_with_حاصل(abits_ptr_[cnt], ~that.abits_ptr_[cnt], حاصل);
	    abits_ptr_[cnt] &= نقاب;
      }

      واپس;

 x_برآمد:
      برائے (صحیح idx = 0 ; idx < cnt ; idx += 1) {
	    abits_ptr_[idx] = لفظ_X_ABITS;
	    bbits_ptr_[idx] = لفظ_X_BBITS;
      }
      اگر (غیرعلامتی دم = سائز_2%بٹ_فی_لفظ) {
	    غیرعلامتی طویل نقاب = ~( -1UL << دم );
	    abits_ptr_[cnt] = لفظ_X_ABITS&نقاب;
	    bbits_ptr_[cnt] = لفظ_X_BBITS&نقاب;
      }

}

خالی vvp_vector4_t::mov(غیرعلامتی dst, غیرعلامتی src, غیرعلامتی cnt)
{
      أكد(dst+cnt <= سائز_2);
      أكد(src+cnt <= سائز_2);

      اگر (سائز_2 <= بٹ_فی_لفظ) {
	    غیرعلامتی طویل vmask = (1UL << cnt) - 1;
	    غیرعلامتی طویل tmp;

	    tmp = (abits_val_ >> src) & vmask;
	    abits_val_ &= ~ (vmask << dst);
	    abits_val_ |= tmp << dst;

	    tmp = (bbits_val_ >> src) & vmask;
	    bbits_val_ &= ~ (vmask << dst);
	    bbits_val_ |= tmp << dst;

      } ورنہ {
	    غیرعلامتی sptr = src / بٹ_فی_لفظ;
	    غیرعلامتی dptr = dst / بٹ_فی_لفظ;
	    غیرعلامتی soff = src % بٹ_فی_لفظ;
	    غیرعلامتی doff = dst % بٹ_فی_لفظ;

	    جب (cnt > 0) {
		  غیرعلامتی trans = cnt;
		  اگر ((soff+trans) > بٹ_فی_لفظ)
			trans = بٹ_فی_لفظ - soff;

		  اگر ((doff+trans) > بٹ_فی_لفظ)
			trans = بٹ_فی_لفظ - doff;

		  اگر (trans == بٹ_فی_لفظ) {
			  // Special case: the transfer count is
			  // exactly an entire word. For this to be
			  // true, it must also be true that the
			  // pointers are aligned. The work is easy,
			abits_ptr_[dptr] = abits_ptr_[sptr];
			bbits_ptr_[dptr] = bbits_ptr_[sptr];
			dptr += 1;
			sptr += 1;
			cnt -= بٹ_فی_لفظ;
			جاری;
		  }

		    // Here we know that either the source or
		    // destination is unaligned, and also we know that
		    // the count is less than a full word.
		  غیرعلامتی طویل vmask = (1UL << trans) - 1;
		  غیرعلامتی طویل tmp;

		  tmp = (abits_ptr_[sptr] >> soff) & vmask;
		  abits_ptr_[dptr] &= ~ (vmask << doff);
		  abits_ptr_[dptr] |= tmp << doff;

		  tmp = (bbits_ptr_[sptr] >> soff) & vmask;
		  bbits_ptr_[dptr] &= ~ (vmask << doff);
		  bbits_ptr_[dptr] |= tmp << doff;

		  cnt -= trans;
		  soff += trans;
		  اگر (soff >= بٹ_فی_لفظ) {
			soff = 0;
			sptr += 1;
		  }
		  doff += trans;
		  اگر (doff >= بٹ_فی_لفظ) {
			doff = 0;
			dptr += 1;
		  }
	    }
      }
}

خالی vvp_vector4_t::mul(ثابت vvp_vector4_t&that)
{
      أكد(سائز_2 == that.سائز_2);

      اگر (سائز_2 < بٹ_فی_لفظ) {
	    غیرعلامتی طویل نقاب = ~(-1UL << سائز_2);
	    اگر ((bbits_val_|that.bbits_val_) & نقاب) {
		  abits_val_ |= نقاب;
		  bbits_val_ |= نقاب;
		  واپس;
	    }

	    abits_val_ *= that.abits_val_;
	    abits_val_ &= نقاب;
	    واپس;
      }

      اگر (سائز_2 == بٹ_فی_لفظ) {
	    اگر (bbits_val_ || that.bbits_val_) {
		  abits_val_ = لفظ_X_ABITS;
		  bbits_val_ = لفظ_X_BBITS;
	    } ورنہ {
		  abits_val_ *= that.abits_val_;
	    }
	    واپس;
      }

      ثابت صحیح cnt = (سائز_2+بٹ_فی_لفظ-1) / بٹ_فی_لفظ;

      غیرعلامتی طویل نقاب;
      اگر (غیرعلامتی دم = سائز_2%بٹ_فی_لفظ) {
	    نقاب = ~( -1UL << دم );
      } ورنہ {
	    نقاب = ~0UL;
      }

	// Check for any XZ values ahead of time in a first pass. If
	// we find any, then force the entire result to be X and be
	// done.
      برائے (صحیح idx = 0 ; idx < cnt ; idx += 1) {
	    غیرعلامتی طویل lval = bbits_ptr_[idx];
	    غیرعلامتی طویل rval = that.bbits_ptr_[idx];
	    اگر (idx == (cnt-1)) {
		  lval &= نقاب;
		  rval &= نقاب;
	    }
	    اگر (lval || rval) {
		  برائے (صحیح xdx = 0 ; xdx < cnt-1 ; xdx += 1) {
			abits_ptr_[xdx] = لفظ_X_ABITS;
			bbits_ptr_[xdx] = لفظ_X_BBITS;
		  }
		  abits_ptr_[cnt-1] = لفظ_X_ABITS & نقاب;
		  bbits_ptr_[cnt-1] = لفظ_X_BBITS & نقاب;
		  واپس;
	    }
      }

	// Calculate the result into a res array. We need to keep is
	// separate from the "this" array because we are making
	// multiple passes.
      غیرعلامتی طویل*res = نیا غیرعلامتی طویل[cnt];
      برائے (صحیح idx = 0 ; idx < cnt ; idx += 1)
	    res[idx] = 0;

      برائے (صحیح mul_a = 0 ; mul_a < cnt ; mul_a += 1) {
	    غیرعلامتی طویل lval = abits_ptr_[mul_a];
	    اگر (mul_a == (cnt-1))
		  lval &= نقاب;

	    برائے (صحیح mul_b = 0 ; mul_b < (cnt-mul_a) ; mul_b += 1) {
		  غیرعلامتی طویل rval = that.abits_ptr_[mul_b];
		  اگر (mul_b == (cnt-1))
			rval &= نقاب;

		  غیرعلامتی طویل مجموع;
		  غیرعلامتی طویل tmp = multiply_with_حاصل(lval, rval, مجموع);
		  صحیح بنیاد = mul_a + mul_b;
		  غیرعلامتی طویل حاصل = 0;
		  res[بنیاد] = جوڑو_with_حاصل(res[بنیاد], tmp, حاصل);
		  برائے (صحیح جوڑو_idx = بنیاد+1 ; جوڑو_idx < cnt ; جوڑو_idx += 1) {
			res[جوڑو_idx] = جوڑو_with_حاصل(res[جوڑو_idx], مجموع, حاصل);
			مجموع = 0;
		  }
	    }
      }

	// Replace the "this" value with the calculated result. We
	// know a-priori that the bbits are zero and unchanged.
      res[cnt-1] &= نقاب;
      برائے (صحیح idx = 0 ; idx < cnt ; idx += 1)
	    abits_ptr_[idx] = res[idx];

      حذف[]res;
      واپس;


}

منطقی vvp_vector4_t::eeq(ثابت vvp_vector4_t&that) ثابت
{
      اگر (سائز_2 != that.سائز_2)
	    واپس جھوٹ;

      اگر (سائز_2 < بٹ_فی_لفظ) {
	    غیرعلامتی طویل نقاب = (1UL << سائز_2) - 1;
	    واپس (abits_val_&نقاب) == (that.abits_val_&نقاب)
		  && (bbits_val_&نقاب) == (that.bbits_val_&نقاب);
      }

      اگر (سائز_2 == بٹ_فی_لفظ) {
	    واپس (abits_val_ == that.abits_val_)
		  && (bbits_val_ == that.bbits_val_);
      }

      غیرعلامتی الفاظ = سائز_2 / بٹ_فی_لفظ;
      برائے (غیرعلامتی idx = 0 ;  idx < الفاظ ;  idx += 1) {
	    اگر (abits_ptr_[idx] != that.abits_ptr_[idx])
		  واپس جھوٹ;
	    اگر (bbits_ptr_[idx] != that.bbits_ptr_[idx])
		  واپس جھوٹ;
      }

      غیرعلامتی طویل نقاب = سائز_2%بٹ_فی_لفظ;
      اگر (نقاب > 0) {
	    نقاب = (1UL << نقاب) - 1;
	    واپس (abits_ptr_[الفاظ]&نقاب) == (that.abits_ptr_[الفاظ]&نقاب)
		  && (bbits_ptr_[الفاظ]&نقاب) == (that.bbits_ptr_[الفاظ]&نقاب);
      }

      واپس سچ;
}

منطقی vvp_vector4_t::eq_xz(ثابت vvp_vector4_t&that) ثابت
{
      اگر (سائز_2 != that.سائز_2)
	    واپس جھوٹ;

      اگر (سائز_2 < بٹ_فی_لفظ) {
	    غیرعلامتی طویل نقاب = (1UL << سائز_2) - 1;
	    واپس ((abits_val_|bbits_val_)&نقاب) == ((that.abits_val_|that.bbits_val_)&نقاب)
		  && (bbits_val_&نقاب) == (that.bbits_val_&نقاب);
      }

      اگر (سائز_2 == بٹ_فی_لفظ) {
	    واپس ((abits_val_|bbits_val_) == (that.abits_val_|that.bbits_val_))
		  && (bbits_val_ == that.bbits_val_);
      }

      غیرعلامتی الفاظ = سائز_2 / بٹ_فی_لفظ;
      برائے (غیرعلامتی idx = 0 ;  idx < الفاظ ;  idx += 1) {
	    اگر ((abits_ptr_[idx]|bbits_ptr_[idx]) != (that.abits_ptr_[idx]|that.bbits_ptr_[idx]))
		  واپس جھوٹ;
	    اگر (bbits_ptr_[idx] != that.bbits_ptr_[idx])
		  واپس جھوٹ;
      }

      غیرعلامتی طویل نقاب = سائز_2%بٹ_فی_لفظ;
      اگر (نقاب > 0) {
	    نقاب = (1UL << نقاب) - 1;
	    واپس ((abits_ptr_[الفاظ]|bbits_ptr_[الفاظ])&نقاب) == ((that.abits_ptr_[الفاظ]|that.bbits_ptr_[الفاظ])&نقاب)
		  && (bbits_ptr_[الفاظ]&نقاب) == (that.bbits_ptr_[الفاظ]&نقاب);
      }

      واپس سچ;
}

منطقی vvp_vector4_t::has_xz() ثابت
{
      اگر (سائز_2 < بٹ_فی_لفظ) {
	    غیرعلامتی طویل نقاب = -1UL >> (بٹ_فی_لفظ - سائز_2);
	    واپس bbits_val_&نقاب;
      }

      اگر (سائز_2 == بٹ_فی_لفظ) {
	    واپس bbits_val_;
      }

      غیرعلامتی الفاظ = سائز_2 / بٹ_فی_لفظ;
      برائے (غیرعلامتی idx = 0 ; idx < الفاظ ; idx += 1) {
	    اگر (bbits_ptr_[idx])
		  واپس سچ;
      }

      غیرعلامتی طویل نقاب = سائز_2%بٹ_فی_لفظ;
      اگر (نقاب > 0) {
	    نقاب = -1UL >> (بٹ_فی_لفظ - نقاب);
	    واپس bbits_ptr_[الفاظ]&نقاب;
      }

      واپس جھوٹ;
}

خالی vvp_vector4_t::change_z2x()
{
	// This method relies on the fact that both BIT4_X and BIT4_Z
	// have the bbit set in the vector4 encoding, and also that
	// the BIT4_X has abit set in the vector4 encoding. By simply
	// or-ing the bbit into the abit, BIT4_X and BIT4_Z both
	// become BIT4_X.

      اگر (سائز_2 <= بٹ_فی_لفظ) {
	    abits_val_ |= bbits_val_;
      } ورنہ {
	    غیرعلامتی الفاظ = (سائز_2+بٹ_فی_لفظ-1) / بٹ_فی_لفظ;
	    برائے (غیرعلامتی idx = 0 ;  idx < الفاظ ;  idx += 1)
		  abits_ptr_[idx] |= bbits_ptr_[idx];
      }
}

خالی vvp_vector4_t::fill_بٹ(vvp_bit4_t ثنائی_ہندسہ)
{
	/* note: this relies on the bit encoding for the vvp_bit4_t. */
      ساکن ثابت غیرعلامتی طویل init_atable[4] = {
	    لفظ_0_ABITS,
	    لفظ_1_ABITS,
	    لفظ_Z_ABITS,
	    لفظ_X_ABITS };
      ساکن ثابت غیرعلامتی طویل init_btable[4] = {
	    لفظ_0_BBITS,
	    لفظ_1_BBITS,
	    لفظ_Z_BBITS,
	    لفظ_X_BBITS };

      اگر (سائز_2 <= بٹ_فی_لفظ) {
	    abits_val_ = init_atable[ثنائی_ہندسہ];
	    bbits_val_ = init_btable[ثنائی_ہندسہ];
      } ورنہ {
	    غیرعلامتی الفاظ = (سائز_2 + بٹ_فی_لفظ - 1) / بٹ_فی_لفظ;
	    برائے (غیرعلامتی idx = 0; idx < الفاظ; idx += 1) {
		  abits_ptr_[idx] = init_atable[ثنائی_ہندسہ];
		  bbits_ptr_[idx] = init_btable[ثنائی_ہندسہ];
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

      برائے (غیرعلامتی idx = 0 ;  idx < size() && buf_len >= 2 ;  idx += 1) {
	    انتخاب (value(size()-idx-1)) {
		حالت BIT4_0:
		  *buf++ = '0';
		  توڑو;
		حالت BIT4_1:
		  *buf++ = '1';
		  توڑو;
		حالت BIT4_X:
		  *buf++ = 'x';
		  توڑو;
		حالت BIT4_Z:
		  *buf++ = 'z';
	    }
	    buf_len -= 1;
      }

      *buf++ = '>';
      *buf++ = 0;
      واپس res;
}

خالی vvp_vector4_t::invert()
{
      اگر (سائز_2 <= بٹ_فی_لفظ) {
	    غیرعلامتی طویل نقاب = (سائز_2<بٹ_فی_لفظ)? (1UL<<سائز_2)-1UL : -1UL;
	    abits_val_ = نقاب & ~abits_val_;
	    abits_val_ |= bbits_val_;
      } ورنہ {
	    غیرعلامتی بقایا = سائز_2;
	    غیرعلامتی idx = 0;
	    جب (بقایا >= بٹ_فی_لفظ) {
		  abits_ptr_[idx] = ~abits_ptr_[idx];
		  abits_ptr_[idx] |= bbits_ptr_[idx];
		  idx += 1;
		  بقایا -= بٹ_فی_لفظ;
	    }
	    اگر (بقایا > 0) {
		  غیرعلامتی طویل نقاب = (1UL<<بقایا) - 1UL;
		  abits_ptr_[idx] = نقاب & ~abits_ptr_[idx];
		  abits_ptr_[idx] |= bbits_ptr_[idx];
	    }
      }
}

#تعریف_کرو ثنائی_ہندسہ_نقاب(n) ((n) ? ((~0UL) >> (بٹ_فی_لفظ - (n))) : ~0UL)

vvp_bit4_t vvp_vector4_t::reduce_or() ثابت
{
      غیرعلامتی طویل نقاب = ثنائی_ہندسہ_نقاب(سائز_2 % بٹ_فی_لفظ);
      vvp_bit4_t res = BIT4_0;

      اگر (سائز_2 <= بٹ_فی_لفظ) {
	    اگر ((abits_val_ & ~bbits_val_ & نقاب) != 0UL)
		  واپس BIT4_1;
	    اگر ((bbits_val_ & نقاب) != 0UL)
		  واپس BIT4_X;
      } ورنہ {
	    غیرعلامتی الفاظ = (سائز_2 + بٹ_فی_لفظ - 1) / بٹ_فی_لفظ;
	    غیرعلامتی idx;
	    برائے (idx = 0; idx < الفاظ - 1; idx += 1) {
		  اگر ((abits_ptr_[idx] & ~bbits_ptr_[idx]) != 0UL)
			واپس BIT4_1;
		  اگر (bbits_ptr_[idx] != 0UL)
			res = BIT4_X;
	    }
	    اگر ((abits_ptr_[idx] & ~bbits_ptr_[idx] & نقاب) != 0UL)
		  واپس BIT4_1;
	    اگر ((bbits_ptr_[idx] & نقاب) != 0UL)
		  res = BIT4_X;
      }

      واپس res;
}

vvp_bit4_t vvp_vector4_t::reduce_and() ثابت
{
      غیرعلامتی طویل نقاب = ثنائی_ہندسہ_نقاب(سائز_2 % بٹ_فی_لفظ);
      vvp_bit4_t res = BIT4_1;

      اگر (سائز_2 <= بٹ_فی_لفظ) {
	    اگر ((abits_val_ | bbits_val_ | ~نقاب) != ~0UL)
		  واپس BIT4_0;
	    اگر ((bbits_val_ & نقاب) != 0UL)
		  واپس BIT4_X;
	} ورنہ {
	    غیرعلامتی الفاظ = (سائز_2 + بٹ_فی_لفظ - 1) / بٹ_فی_لفظ;
	    غیرعلامتی idx;
	    برائے (idx = 0; idx < الفاظ - 1; idx += 1) {
		  اگر ((abits_ptr_[idx] | bbits_ptr_[idx]) != ~0UL)
			واپس BIT4_0;
		  اگر (bbits_ptr_[idx] != 0UL)
			res = BIT4_X;
	    }
	    اگر ((abits_ptr_[idx] | bbits_ptr_[idx] | ~نقاب) != ~0UL)
		  واپس BIT4_0;
	    اگر ((bbits_ptr_[idx] & نقاب) != 0UL)
		  res = BIT4_X;
      }

      واپس res;
}

ساکن غیرعلامتی طویل parity(غیرعلامتی طویل val)
{
#اگر defined(__GNUC__)
      // The compiler builtin can use target-specific CPU instructions.
      واپس __builtin_parityl(val);
#ورنہ
#اگر ULONG_MAX > 0xffffffffUL
	val ^= val >> 32;
#اگر_ختم
	val ^= val >> 16;
	val ^= val >> 8;
	val ^= val >> 4;

      واپس (0x6996 >> (val & 0xf)) & 1;
#اگر_ختم
}

vvp_bit4_t vvp_vector4_t::reduce_xor() ثابت
{
      غیرعلامتی طویل نقاب = ثنائی_ہندسہ_نقاب(سائز_2 % بٹ_فی_لفظ);

      اگر (سائز_2 <= بٹ_فی_لفظ) {
	    اگر ((bbits_val_ & نقاب) != 0UL)
		  واپس BIT4_X;
	    واپس parity(abits_val_ & نقاب) ? BIT4_1 : BIT4_0;
      } ورنہ {
	    غیرعلامتی الفاظ = (سائز_2 + بٹ_فی_لفظ - 1) / بٹ_فی_لفظ;
	    غیرعلامتی طویل val_a = 0UL;
	    غیرعلامتی idx;
	    برائے (idx = 0; idx < الفاظ - 1; idx += 1) {
		  اگر (bbits_ptr_[idx] != 0UL)
			واپس BIT4_X;
		  val_a ^= abits_ptr_[idx];
	    }
	    اگر ((bbits_ptr_[idx] & نقاب) != 0UL)
		  واپس BIT4_X;
	    val_a ^= abits_ptr_[idx] & نقاب;
	    واپس parity(val_a) ? BIT4_1 : BIT4_0;
      }
}

vvp_vector4_t& vvp_vector4_t::عامل &= (ثابت vvp_vector4_t&that)
{
	// The truth table is:
	//     00 01 11 10
	//  00 00 00 00 00
	//  01 00 01 11 11
	//  11 00 11 11 11
	//  10 00 11 11 11
      اگر (سائز_2 <= بٹ_فی_لفظ) {
	    غیرعلامتی طویل tmp1 = abits_val_ | bbits_val_;
	    غیرعلامتی طویل tmp2 = that.abits_val_ | that.bbits_val_;
	    abits_val_ = tmp1 & tmp2;
	    bbits_val_ = (tmp1 & that.bbits_val_) | (tmp2 & bbits_val_);
      } ورنہ {
	    غیرعلامتی الفاظ = (سائز_2 + بٹ_فی_لفظ - 1) / بٹ_فی_لفظ;
	    برائے (غیرعلامتی idx = 0; idx < الفاظ ; idx += 1) {
		  غیرعلامتی طویل tmp1 = abits_ptr_[idx] | bbits_ptr_[idx];
		  غیرعلامتی طویل tmp2 = that.abits_ptr_[idx] |
		                       that.bbits_ptr_[idx];
		  abits_ptr_[idx] = tmp1 & tmp2;
		  bbits_ptr_[idx] = (tmp1 & that.bbits_ptr_[idx]) |
		                    (tmp2 & bbits_ptr_[idx]);
	    }
      }

      واپس *یہ;
}

vvp_vector4_t& vvp_vector4_t::عامل |= (ثابت vvp_vector4_t&that)
{
	// The truth table is:
	//     00 01 11 10
	//  00 00 01 11 11
	//  01 01 01 01 01
	//  11 11 01 11 11
	//  10 11 01 11 11
      اگر (سائز_2 <= بٹ_فی_لفظ) {
	    غیرعلامتی طویل tmp = abits_val_ | bbits_val_ |
	                        that.abits_val_ | that.bbits_val_;
	    bbits_val_ = ((~abits_val_ | bbits_val_) & that.bbits_val_) |
	                 ((~that.abits_val_ | that.bbits_val_) & bbits_val_);
	    abits_val_ = tmp;

      } ورنہ {
	    غیرعلامتی الفاظ = (سائز_2 + بٹ_فی_لفظ - 1) / بٹ_فی_لفظ;
	    برائے (غیرعلامتی idx = 0; idx < الفاظ ; idx += 1) {
		  غیرعلامتی طویل tmp = abits_ptr_[idx] | bbits_ptr_[idx] |
	                        that.abits_ptr_[idx] | that.bbits_ptr_[idx];
		  bbits_ptr_[idx] = ((~abits_ptr_[idx] | bbits_ptr_[idx]) &
		                     that.bbits_ptr_[idx]) |
		                    ((~that.abits_ptr_[idx] |
		                      that.bbits_ptr_[idx]) & bbits_ptr_[idx]);
		  abits_ptr_[idx] = tmp;
	    }
      }

      واپس *یہ;
}

vvp_vector4_t& vvp_vector4_t::عامل ^= (ثابت vvp_vector4_t&that)
{
	// The truth table is:
	//     00 01 11 10
	//  00 00 01 11 11
	//  01 01 00 11 11
	//  11 11 11 11 11
	//  10 11 11 11 11
      اگر (سائز_2 <= بٹ_فی_لفظ) {
	    غیرعلامتی طویل bval = bbits_val_ | that.bbits_val_;
	    bbits_val_ = bval;
	    abits_val_ = (abits_val_ ^ that.abits_val_) | bval;
      } ورنہ {
	    غیرعلامتی الفاظ = (سائز_2 + بٹ_فی_لفظ - 1) / بٹ_فی_لفظ;
	    برائے (غیرعلامتی idx = 0; idx < الفاظ ; idx += 1) {
		  غیرعلامتی طویل bval = bbits_ptr_[idx] | that.bbits_ptr_[idx];
		  bbits_ptr_[idx] = bval;
		  abits_ptr_[idx] = (abits_ptr_[idx] ^ that.abits_ptr_[idx]) | bval;
	    }
      }

      واپس *یہ;
}

/*
* Add an integer to the vvp_vector4_t in place, bit by bit so that
* there is no size limitations.
*/
vvp_vector4_t& vvp_vector4_t::عامل += (int64_t that)
{
      vvp_bit4_t حاصل = BIT4_0;
      غیرعلامتی idx;

      اگر (has_xz()) {
	    vvp_vector4_t xxx (size(), BIT4_X);
	    *یہ = xxx;
	    واپس *یہ;
      }

      برائے (idx = 0 ; idx < size() ; idx += 1) {
	    اگر (that == 0 && حاصل==BIT4_0)
		  توڑو;

	    vvp_bit4_t that_ثنائی_ہندسہ = (that&1)? BIT4_1 : BIT4_0;
	    that >>= 1;

	    اگر (that_ثنائی_ہندسہ==BIT4_0 && حاصل==BIT4_0)
		  جاری;

	    vvp_bit4_t ثنائی_ہندسہ = value(idx);
	    ثنائی_ہندسہ = جوڑو_with_حاصل(ثنائی_ہندسہ, that_ثنائی_ہندسہ, حاصل);

	    سیٹ_ثنائی_ہندسہ(idx, ثنائی_ہندسہ);
      }

      واپس *یہ;
}

ostream& عامل<< (ostream&out, ثابت vvp_vector4_t&that)
{
      out << that.size() << "'b";
      برائے (غیرعلامتی idx = 0 ;  idx < that.size() ;  idx += 1)
	    out << that.value(that.size()-idx-1);
      واپس out;
}

سانچہ <جماعت INT>منطقی vector4_مقصد_قدر(ثابت vvp_vector4_t&vec, INT&val,
					  منطقی is_signed, منطقی is_arithmetic)
{
      INT res = 0;
      INT msk = 1;
      منطقی rc_flag = سچ;

      غیرعلامتی size = vec.size();
      اگر (size > 8*حجم(val)) size = 8*حجم(val);
      برائے (غیرعلامتی idx = 0 ;  idx < size ;  idx += 1) {
	    انتخاب (vec.value(idx)) {
		حالت BIT4_0:
		  توڑو;
		حالت BIT4_1:
		  res |= msk;
		  توڑو;
		طےشدہ:
		  اگر (is_arithmetic)
			واپس جھوٹ;
		  ورنہ
			rc_flag = جھوٹ;
	    }

	    msk <<= 1;
      }

      اگر (is_signed && vec.value(vec.size()-1) == BIT4_1) {
	    اگر (vec.size() < 8*حجم(val))
		  res |= static_cast<INT>(-1ULL << vec.size());
      }

      val = res;
      واپس rc_flag;
}

سانچہ منطقی vector4_مقصد_قدر(ثابت vvp_vector4_t&vec, int8_t&val,
			       منطقی is_signed, منطقی is_arithmetic);
سانچہ منطقی vector4_مقصد_قدر(ثابت vvp_vector4_t&vec, int16_t&val,
			       منطقی is_signed, منطقی is_arithmetic);
سانچہ منطقی vector4_مقصد_قدر(ثابت vvp_vector4_t&vec, int32_t&val,
			       منطقی is_signed, منطقی is_arithmetic);
سانچہ منطقی vector4_مقصد_قدر(ثابت vvp_vector4_t&vec, int64_t&val,
			       منطقی is_signed, منطقی is_arithmetic);
سانچہ منطقی vector4_مقصد_قدر(ثابت vvp_vector4_t&vec, uint8_t&val,
			       منطقی is_signed, منطقی is_arithmetic);
سانچہ منطقی vector4_مقصد_قدر(ثابت vvp_vector4_t&vec, uint16_t&val,
			       منطقی is_signed, منطقی is_arithmetic);
سانچہ منطقی vector4_مقصد_قدر(ثابت vvp_vector4_t&vec, uint32_t&val,
			       منطقی is_signed, منطقی is_arithmetic);
سانچہ منطقی vector4_مقصد_قدر(ثابت vvp_vector4_t&vec, uint64_t&val,
			       منطقی is_signed, منطقی is_arithmetic);

سانچہ <جماعت T> منطقی vector4_مقصد_قدر(ثابت vvp_vector4_t&vec,
                                         منطقی&حد_سے_تجاوز_flag, T&val)
{
      T res = 0;
      T msk = 1;

      حد_سے_تجاوز_flag = جھوٹ;
      غیرعلامتی size = vec.size();
      برائے (غیرعلامتی idx = 0 ;  idx < size ;  idx += 1) {
	    انتخاب (vec.value(idx)) {
		حالت BIT4_0:
		  توڑو;
		حالت BIT4_1:
		  اگر (msk == 0)
			حد_سے_تجاوز_flag = سچ;
		  ورنہ
			res |= msk;
		  توڑو;
		طےشدہ:
		  واپس جھوٹ;
	    }

	    msk <<= static_cast<T>(1);
      }

      val = res;
      واپس سچ;
}

سانچہ منطقی vector4_مقصد_قدر(ثابت vvp_vector4_t&vec, منطقی&حد_سے_تجاوز_flag,
                               غیرعلامتی طویل&val);
#اگر_غیر_تعریف_شدہ UL_AND_TIME64_SAME
سانچہ منطقی vector4_مقصد_قدر(ثابت vvp_vector4_t&vec, منطقی&حد_سے_تجاوز_flag,
                               vvp_time64_t&val);
#اگر_ختم

منطقی vector4_مقصد_قدر(ثابت vvp_vector4_t&vec, دوہرا&val, منطقی signed_flag)
{

      اگر (vec.size() == 0) {
	    val = 0.0;
	    واپس سچ;
      }

      منطقی flag = سچ;

      اگر (vec.value(vec.size()-1) != BIT4_1) {
	    signed_flag = جھوٹ;
      }

      دوہرا res = 0.0;
      اگر (signed_flag) {
	    vvp_bit4_t حاصل = BIT4_1;
	    برائے (غیرعلامتی idx = 0 ;  idx < vec.size() ;  idx += 1) {
		  vvp_bit4_t a = ~vec.value(idx);
		  vvp_bit4_t x = جوڑو_with_حاصل(a, BIT4_0, حاصل);
		  انتخاب (x) {
		      حالت BIT4_0:
			توڑو;
		      حالت BIT4_1:
			res += قوت(2.0, (صحیح)idx);
			توڑو;
		      طےشدہ:
			flag = جھوٹ;
		  }
	    }
	    res *= -1.0;
      } ورنہ {
	    برائے (غیرعلامتی idx = 0 ;  idx < vec.size() ;  idx += 1) {
		  انتخاب (vec.value(idx)) {
		      حالت BIT4_0:
			توڑو;
		      حالت BIT4_1:
			res += قوت(2.0, (صحیح)idx);
			توڑو;
		      طےشدہ:
			flag = جھوٹ;
		  }
	    }
      }
      val = res;
      واپس flag;
}

منطقی vector2_مقصد_قدر(ثابت vvp_vector2_t&a, int32_t&val, منطقی is_signed)
{
      val = 0;
      غیرعلامتی idx;
      uint32_t نقاب;
      برائے (idx = 0, نقاب = 1 ; idx < a.size() && idx < 32 ; idx += 1, نقاب <<= 1) {
	    اگر (a.value(idx)) val |= نقاب;
      }

      اگر (is_signed && a.size() < 32 && a.value(a.size()-1)) {
	    نقاب = -1;
	    نقاب <<= a.size();
	    val |= نقاب;
      }

      واپس a.size() <= 32;
}

vvp_vector4array_t::vvp_vector4array_t(غیرعلامتی چوڑائی_3, غیرعلامتی الفاظ_2)
: چوڑائی_2(چوڑائی_3), الفاظ_3(الفاظ_2)
{
}

vvp_vector4array_t::~vvp_vector4array_t()
{
}

خالی vvp_vector4array_t::سیٹ_لفظ_2(v4cell*cell, ثابت vvp_vector4_t&that)
{
      أكد(that.سائز_2 == چوڑائی_2);

      اگر (چوڑائی_2 <= vvp_vector4_t::بٹ_فی_لفظ) {
	    cell->abits_val_ = that.abits_val_;
	    cell->bbits_val_ = that.bbits_val_;
	    واپس;
      }

      غیرعلامتی cnt = (چوڑائی_2 + vvp_vector4_t::بٹ_فی_لفظ-1)/vvp_vector4_t::بٹ_فی_لفظ;

      اگر (cell->abits_ptr_ == 0) {
	    cell->abits_ptr_ = نیا غیرعلامتی طویل[2*cnt];
	    cell->bbits_ptr_ = cell->abits_ptr_ + cnt;
      }

      برائے (غیرعلامتی idx = 0 ; idx < cnt ; idx += 1)
	    cell->abits_ptr_[idx] = that.abits_ptr_[idx];
      برائے (غیرعلامتی idx = 0 ; idx < cnt ; idx += 1)
	    cell->bbits_ptr_[idx] = that.bbits_ptr_[idx];
}

vvp_vector4_t vvp_vector4array_t::get_لفظ_2(v4cell*cell) ثابت
{
      اگر (چوڑائی_2 <= vvp_vector4_t::بٹ_فی_لفظ) {
	    vvp_vector4_t res;
	    res.سائز_2 = چوڑائی_2;
	    res.abits_val_ = cell->abits_val_;
	    res.bbits_val_ = cell->bbits_val_;
	    واپس res;
      }

      vvp_vector4_t res (چوڑائی_2, BIT4_X);
      اگر (cell->abits_ptr_ == 0)
	    واپس res;

      غیرعلامتی cnt = (چوڑائی_2 + vvp_vector4_t::بٹ_فی_لفظ-1)/vvp_vector4_t::بٹ_فی_لفظ;

      برائے (غیرعلامتی idx = 0 ; idx < cnt ; idx += 1)
	    res.abits_ptr_[idx] = cell->abits_ptr_[idx];
      برائے (غیرعلامتی idx = 0 ; idx < cnt ; idx += 1)
	    res.bbits_ptr_[idx] = cell->bbits_ptr_[idx];

      واپس res;
}

vvp_vector4array_sa::vvp_vector4array_sa(غیرعلامتی چوڑائی_3, غیرعلامتی الفاظ_2)
: vvp_vector4array_t(چوڑائی_3, الفاظ_2)
{
      array_ = نیا v4cell[الفاظ_3];

      اگر (چوڑائی_2 <= vvp_vector4_t::بٹ_فی_لفظ) {
	    برائے (غیرعلامتی idx = 0 ; idx < الفاظ_3 ; idx += 1) {
		  array_[idx].abits_val_ = vvp_vector4_t::لفظ_X_ABITS;
		  array_[idx].bbits_val_ = vvp_vector4_t::لفظ_X_BBITS;
	    }
      } ورنہ {
	    برائے (غیرعلامتی idx = 0 ; idx < الفاظ_3 ; idx += 1) {
		  array_[idx].abits_ptr_ = 0;
		  array_[idx].bbits_ptr_ = 0;
	    }
      }
}

vvp_vector4array_sa::~vvp_vector4array_sa()
{
      اگر (array_) {
	    اگر (چوڑائی_2 > vvp_vector4_t::بٹ_فی_لفظ) {
		  برائے (غیرعلامتی idx = 0 ; idx < الفاظ_3 ; idx += 1)
			اگر (array_[idx].abits_ptr_)
			      حذف[]array_[idx].abits_ptr_;
	    }
	    حذف[]array_;
      }
}

خالی vvp_vector4array_sa::سیٹ_لفظ(غیرعلامتی اشاریہ, ثابت vvp_vector4_t&that)
{
      أكد(اشاریہ < الفاظ_3);

      v4cell*cell = &array_[اشاریہ];

      سیٹ_لفظ_2(cell, that);
}

vvp_vector4_t vvp_vector4array_sa::get_لفظ(غیرعلامتی اشاریہ) ثابت
{
      اگر (اشاریہ >= الفاظ_3)
	    واپس vvp_vector4_t(چوڑائی_2, BIT4_X);

      أكد(اشاریہ < الفاظ_3);

      v4cell*cell = &array_[اشاریہ];

      واپس get_لفظ_2(cell);
}

vvp_vector4array_aa::vvp_vector4array_aa(غیرعلامتی چوڑائی_3, غیرعلامتی الفاظ_2)
: vvp_vector4array_t(چوڑائی_3, الفاظ_2)
{
      context_idx_ = vpip_جوڑو_شے_مقصد_context(یہ, vpip_peek_context_scope());
}

vvp_vector4array_aa::~vvp_vector4array_aa()
{
}

خالی vvp_vector4array_aa::alloc_instance(vvp_context_t context)
{
      v4cell*array = نیا v4cell[الفاظ_3];

      اگر (چوڑائی_2 <= vvp_vector4_t::بٹ_فی_لفظ) {
	    برائے (غیرعلامتی idx = 0 ; idx < الفاظ_3 ; idx += 1) {
		  array[idx].abits_val_ = vvp_vector4_t::لفظ_X_ABITS;
		  array[idx].bbits_val_ = vvp_vector4_t::لفظ_X_BBITS;
	    }
      } ورنہ {
	    برائے (غیرعلامتی idx = 0 ; idx < الفاظ_3 ; idx += 1) {
		  array[idx].abits_ptr_ = 0;
		  array[idx].bbits_ptr_ = 0;
	    }
      }

      vvp_سیٹ_context_شے(context, context_idx_, array);
}

خالی vvp_vector4array_aa::reset_instance(vvp_context_t context)
{
      v4cell*cell = static_cast<v4cell*>
            (vvp_get_context_شے(context, context_idx_));

      اگر (چوڑائی_2 <= vvp_vector4_t::بٹ_فی_لفظ) {
	    برائے (غیرعلامتی idx = 0 ; idx < الفاظ_3 ; idx += 1) {
		  cell->abits_val_ = vvp_vector4_t::لفظ_X_ABITS;
		  cell->bbits_val_ = vvp_vector4_t::لفظ_X_BBITS;
                  cell++;
	    }
      } ورنہ {
            غیرعلامتی cnt = (چوڑائی_2 + vvp_vector4_t::بٹ_فی_لفظ-1)/vvp_vector4_t::بٹ_فی_لفظ;
	    برائے (غیرعلامتی idx = 0 ; idx < الفاظ_3 ; idx += 1) {
		  اگر (cell->abits_ptr_) {
                        برائے (غیرعلامتی n = 0 ; n < cnt ; n += 1) {
			      cell->abits_ptr_[n] = vvp_vector4_t::لفظ_X_ABITS;
			      cell->bbits_ptr_[n] = vvp_vector4_t::لفظ_X_BBITS;
                        }
                  }
                  cell++;
	    }
      }
}

#اگر_تعریف_شدہ CHECK_WITH_VALGRIND
خالی vvp_vector4array_aa::free_instance(vvp_context_t context)
{
      v4cell*cell = static_cast<v4cell*>
            (vvp_get_context_شے(context, context_idx_));
      حذف [] cell;
}
#اگر_ختم

خالی vvp_vector4array_aa::سیٹ_لفظ(غیرعلامتی اشاریہ, ثابت vvp_vector4_t&that)
{
      أكد(اشاریہ < الفاظ_3);

      v4cell*cell = static_cast<v4cell*>
            (vthread_get_wt_context_شے(context_idx_)) + اشاریہ;

      سیٹ_لفظ_2(cell, that);
}

vvp_vector4_t vvp_vector4array_aa::get_لفظ(غیرعلامتی اشاریہ) ثابت
{
      اگر (اشاریہ >= الفاظ_3)
	    واپس vvp_vector4_t(چوڑائی_2, BIT4_X);

      أكد(اشاریہ < الفاظ_3);

      v4cell*cell = static_cast<v4cell*>
            (vthread_get_rd_context_شے(context_idx_)) + اشاریہ;

      واپس get_لفظ_2(cell);
}

vvp_vector2_t::vvp_vector2_t()
{
      vec_ = 0;
      wid_ = 0;
}

vvp_vector2_t::vvp_vector2_t(uint64_t v, غیرعلامتی wid)
{
      wid_ = wid;
      ثابت غیرعلامتی بٹ_فی_لفظ_2 = 8 * حجم(vec_[0]);
      ثابت غیرعلامتی الفاظ = (wid_ + بٹ_فی_لفظ_2-1) / بٹ_فی_لفظ_2;

      vec_ = نیا غیرعلامتی طویل[الفاظ];
      برائے (غیرعلامتی idx = 0 ; idx < الفاظ ; idx += 1) {
	    ثابت غیرعلامتی سرکاؤ = idx * بٹ_فی_لفظ_2;
	    vec_[idx] = سرکاؤ < 64 ? v >> سرکاؤ : 0;
      }
}

vvp_vector2_t::vvp_vector2_t(vvp_vector2_t::fill_t fill, غیرعلامتی wid)
{
      wid_ = wid;
      ثابت غیرعلامتی بٹ_فی_لفظ_2 = 8 * حجم(vec_[0]);
      ثابت غیرعلامتی الفاظ = (wid_ + بٹ_فی_لفظ_2-1) / بٹ_فی_لفظ_2;

      vec_ = نیا غیرعلامتی طویل[الفاظ];
      برائے (غیرعلامتی idx = 0 ;  idx < الفاظ ;  idx += 1)
	    vec_[idx] = fill? -1 : 0;
}

vvp_vector2_t::vvp_vector2_t(ثابت vvp_vector2_t&that, غیرعلامتی بنیاد, غیرعلامتی wid)
{
      wid_ = wid;
      ثابت غیرعلامتی الفاظ = (wid_ + بٹ_فی_لفظ-1) / بٹ_فی_لفظ;

      vec_ = نیا غیرعلامتی طویل[الفاظ];

      برائے (غیرعلامتی idx = 0 ; idx < wid ; idx += 1) {
	    صحیح ثنائی_ہندسہ = that.value(بنیاد+idx);
	    اگر (ثنائی_ہندسہ == 0)
		  جاری;
	    غیرعلامتی لفظ = idx / بٹ_فی_لفظ;
	    غیرعلامتی طویل نقاب = 1UL << (idx % بٹ_فی_لفظ);
	    vec_[لفظ] |= نقاب;
      }
}

خالی vvp_vector2_t::copy_from_that_(ثابت vvp_vector4_t&that)
{
      wid_ = that.size();
      ثابت غیرعلامتی الفاظ = (that.size() + بٹ_فی_لفظ-1) / بٹ_فی_لفظ;

      اگر (الفاظ == 0) {
	    vec_ = 0;
	    wid_ = 0;
	    واپس;
      }

	// Use the subarray method with the xz_to_0 flag set so that
	// we get values even when there are xz bits.
      vec_ = that.subarray(0, wid_, سچ);
}

خالی vvp_vector2_t::copy_from_that_(ثابت vvp_vector2_t&that)
{
      wid_ = that.wid_;
      ثابت غیرعلامتی الفاظ = (wid_ + بٹ_فی_لفظ-1) / بٹ_فی_لفظ;

      اگر (الفاظ == 0) {
	    vec_ = 0;
	    wid_ = 0;
	    واپس;
      }

      vec_ = نیا غیرعلامتی طویل[الفاظ];
      برائے (غیرعلامتی idx = 0 ;  idx < الفاظ ;  idx += 1)
	    vec_[idx] = that.vec_[idx];
}

vvp_vector2_t::vvp_vector2_t(ثابت vvp_vector2_t&that, غیرعلامتی newsize)
{
      wid_ = newsize;
      اگر (newsize == 0) {
	    vec_ = 0;
	    واپس;
      }

      ثابت غیرعلامتی الفاظ = (wid_ + بٹ_فی_لفظ-1) / بٹ_فی_لفظ;
      ثابت غیرعلامتی twords = (that.wid_ + بٹ_فی_لفظ-1) / بٹ_فی_لفظ;

      vec_ = نیا غیرعلامتی طویل[الفاظ];
      برائے (غیرعلامتی idx = 0 ;  idx < الفاظ ;  idx += 1) {
	    اگر (idx < twords)
		  vec_[idx] = that.vec_[idx];
	    ورنہ
		  vec_[idx] = 0;
      }
}

vvp_vector2_t& vvp_vector2_t::عامل= (ثابت vvp_vector2_t&that)
{
      اگر (یہ == &that)
	    واپس *یہ;

      حذف[] vec_;
      vec_ = 0;

      copy_from_that_(that);
      واپس *یہ;
}

vvp_vector2_t& vvp_vector2_t::عامل= (ثابت vvp_vector4_t&that)
{
      حذف[]vec_;
      vec_ = 0;
      copy_from_that_(that);
      واپس *یہ;
}

vvp_vector2_t& vvp_vector2_t::عامل <<= (غیرعلامتی صحیح سرکاؤ)
{
      اگر (wid_ == 0)
	    واپس *یہ;

      ثابت غیرعلامتی الفاظ = (wid_ + بٹ_فی_لفظ-1) / بٹ_فی_لفظ;

	// Number of words to shift
      ثابت غیرعلامتی wshift = سرکاؤ / بٹ_فی_لفظ;
	// bits to shift within each word.
      ثابت غیرعلامتی طویل oshift = سرکاؤ % بٹ_فی_لفظ;

	// If shifting the entire value away, then return zeros.
      اگر (wshift >= الفاظ) {
	    برائے (غیرعلامتی idx = 0 ;  idx < الفاظ ;  idx += 1)
		  vec_[idx] = 0;

	    واپس *یہ;
      }

	// Do the word shift first.
      اگر (wshift > 0) {
	    برائے (غیرعلامتی idx = 0 ;  idx < الفاظ-wshift ;  idx += 1) {
		  غیرعلامتی sel = الفاظ - idx - 1;
		  vec_[sel] = vec_[sel-wshift];
	    }

	    برائے (غیرعلامتی idx = 0 ;  idx < wshift ;  idx += 1)
		  vec_[idx] = 0;
      }

	// Do the fine shift.
      اگر (oshift != 0) {
	    غیرعلامتی طویل pad = 0;
	    برائے (غیرعلامتی idx = 0 ;  idx < الفاظ ;  idx += 1) {
		  غیرعلامتی طویل اگلا_pad = vec_[idx] >> (بٹ_فی_لفظ-oshift);
		  vec_[idx] = (vec_[idx] << oshift) | pad;
		  pad = اگلا_pad;
	    }

	      // Cleanup the tail bits.
	    غیرعلامتی طویل نقاب = -1UL >> (بٹ_فی_لفظ - wid_%بٹ_فی_لفظ);
	    vec_[الفاظ-1] &= نقاب;
      }

      واپس *یہ;
}

vvp_vector2_t& vvp_vector2_t::عامل >>= (غیرعلامتی سرکاؤ)
{
      اگر (wid_ == 0)
	    واپس *یہ;

      ثابت غیرعلامتی الفاظ = (wid_ + بٹ_فی_لفظ-1) / بٹ_فی_لفظ;

	// Number of words to shift
      ثابت غیرعلامتی wshift = سرکاؤ / بٹ_فی_لفظ;
	// bits to shift within each word.
      ثابت غیرعلامتی طویل oshift = سرکاؤ % بٹ_فی_لفظ;

	// If shifting the entire value away, then return zeros.
      اگر (wshift >= الفاظ) {
	    برائے (غیرعلامتی idx = 0 ;  idx < الفاظ ;  idx += 1)
		  vec_[idx] = 0;

	    واپس *یہ;
      }

      اگر (wshift > 0) {
	    برائے (غیرعلامتی idx = 0 ;  idx < الفاظ-wshift ;  idx += 1)
		  vec_[idx] = vec_[idx+wshift];

	    برائے (غیرعلامتی idx = الفاظ-wshift ;  idx < الفاظ ;  idx += 1)
		  vec_[idx] = 0;
      }

      اگر (oshift > 0) {
	    غیرعلامتی طویل pad = 0;
	    برائے (غیرعلامتی idx = الفاظ ;  idx > 0 ;  idx -= 1) {
		  غیرعلامتی طویل new_pad = vec_[idx-1] <<(بٹ_فی_لفظ-oshift);
		  vec_[idx-1] = pad | (vec_[idx-1] >> oshift);
		  pad = new_pad;
	    }

	      // Cleanup the tail bits.

	    غیرعلامتی use_الفاظ = الفاظ;
	      // Mask_shift is the number of high bits of the top word
	      // that are to be masked off. We start with the number
	      // of bits that are not included even in the original,
	      // then we include the bits of the shift, that are to be
	      // masked to zero.
	    غیرعلامتی طویل نقاب_سرکاؤ = بٹ_فی_لفظ - wid_%بٹ_فی_لفظ;
	    نقاب_سرکاؤ %= بٹ_فی_لفظ;
	    نقاب_سرکاؤ += oshift;
	    جب (نقاب_سرکاؤ >= بٹ_فی_لفظ) {
		  vec_[use_الفاظ-1] = 0;
		  use_الفاظ -= 1;
		  نقاب_سرکاؤ -= بٹ_فی_لفظ;
	    }
	    اگر (نقاب_سرکاؤ > 0) {
		  أكد(use_الفاظ > 0);
		  غیرعلامتی طویل نقاب = -1UL >> نقاب_سرکاؤ;
		  vec_[use_الفاظ-1] &= نقاب;
	    }
      }

      واپس *یہ;
}

ساکن غیرعلامتی طویل جوڑو_حاصل(غیرعلامتی طویل a, غیرعلامتی طویل b,
			       غیرعلامتی طویل&حاصل)
{
      غیرعلامتی طویل out = حاصل;
      حاصل = 0;

      اگر ((ULONG_MAX - out) < a)
	    حاصل += 1;
      out += a;

      اگر ((ULONG_MAX - out) < b)
	    حاصل += 1;
      out += b;

      واپس out;
}

vvp_vector2_t& vvp_vector2_t::عامل += (ثابت vvp_vector2_t&that)
{
      أكد(wid_ == that.wid_);
      اگر (wid_ == 0)
	    واپس *یہ;

      ثابت غیرعلامتی الفاظ = (wid_ + بٹ_فی_لفظ-1) / بٹ_فی_لفظ;

      غیرعلامتی طویل حاصل = 0;
      برائے (غیرعلامتی idx = 0 ;  idx < الفاظ ;  idx += 1) {
	    vec_[idx] = جوڑو_حاصل(vec_[idx], that.vec_[idx], حاصل);
      }


	// Cleanup the tail bits.
      غیرعلامتی طویل نقاب = -1UL >> (بٹ_فی_لفظ - wid_%بٹ_فی_لفظ);
      vec_[الفاظ-1] &= نقاب;

      واپس *یہ;
}

vvp_vector2_t& vvp_vector2_t::عامل -= (ثابت vvp_vector2_t&that)
{
      أكد(wid_ == that.wid_);
      اگر (wid_ == 0)
	    واپس *یہ;

      ثابت غیرعلامتی الفاظ = (wid_ + بٹ_فی_لفظ-1) / بٹ_فی_لفظ;

      غیرعلامتی طویل حاصل = 1;
      برائے (غیرعلامتی idx = 0 ;  idx < الفاظ ;  idx += 1) {
	    vec_[idx] = جوڑو_حاصل(vec_[idx], ~that.vec_[idx], حاصل);
      }

      واپس *یہ;
}

خالی vvp_vector2_t::trim()
{
      جب (value(wid_-1) == 0 && wid_ > 1) wid_ -= 1;
}

/* This is a special trim that is used on numbers we know represent a
 * negative signed value (they came from a negative real value). */
خالی vvp_vector2_t::trim_neg()
{
      اگر (value(wid_-1) == 1 && wid_ > 32) {
	    جب (value(wid_-2) == 1 && wid_ > 32) wid_ -= 1;
      }
}

صحیح vvp_vector2_t::value(غیرعلامتی idx) ثابت
{
      اگر (idx >= wid_)
	    واپس 0;

      ثابت غیرعلامتی بٹ_فی_لفظ_2 = 8 * حجم(vec_[0]);
      غیرعلامتی addr = idx/بٹ_فی_لفظ_2;
      غیرعلامتی نقاب = idx%بٹ_فی_لفظ_2;

      اگر (vec_[addr] & (1UL<<نقاب))
	    واپس 1;
      ورنہ
	    واپس 0;
}

خالی vvp_vector2_t::سیٹ_ثنائی_ہندسہ(غیرعلامتی idx, صحیح ثنائی_ہندسہ)
{
      أكد(idx < wid_);

      ثابت غیرعلامتی بٹ_فی_لفظ_2 = 8 * حجم(vec_[0]);
      غیرعلامتی addr = idx/بٹ_فی_لفظ_2;
      غیرعلامتی طویل نقاب = idx%بٹ_فی_لفظ_2;

      اگر (ثنائی_ہندسہ)
	    vec_[addr] |= 1UL << نقاب;
      ورنہ
	    vec_[addr] &= ~(1UL << نقاب);
}

خالی vvp_vector2_t::سیٹ_vec(غیرعلامتی adr, ثابت vvp_vector2_t&that)
{
      أكد((adr + that.wid_) <= wid_);

      برائے (غیرعلامتی idx = 0 ; idx < that.wid_ ; idx += 1)
	    سیٹ_ثنائی_ہندسہ(adr+idx, that.value(idx));
}

منطقی vvp_vector2_t::is_NaN() ثابت
{
      واپس wid_ == 0;
}

منطقی vvp_vector2_t::is_zero() ثابت
{
      ثابت غیرعلامتی الفاظ = (wid_ + بٹ_فی_لفظ-1) / بٹ_فی_لفظ;

      برائے (غیرعلامتی idx = 0; idx < الفاظ; idx += 1) {
	    اگر (vec_[idx] == 0) جاری;
	    واپس جھوٹ;
      }

      واپس سچ;
}

/*
 * Basic idea from "Introduction to Programming using SML" by
 * Michael R. Hansen and Hans Rischel page 261 and "Seminumerical
 * Algorithms, Third Edition" by Donald E. Knuth section 4.6.3.
 */
vvp_vector2_t قوت(ثابت vvp_vector2_t&x, vvp_vector2_t&y)
{
        /* If we have a zero exponent just return 1. */
      اگر (y == vvp_vector2_t(0L, 1)) {
	    واپس vvp_vector2_t(1L, x.size());
      }

        /* Is the value odd? */
      اگر (y.value(0) == 1) {
	    y.سیٹ_ثنائی_ہندسہ(0, 0);  // A quick subtract by 1.
	    vvp_vector2_t res = x * قوت(x, y);
	    واپس res;
      }

      y >>= 1;  // A fast divide by two. We know the LSB is zero.
      vvp_vector2_t z = قوت(x, y);
      vvp_vector2_t res = z * z;
      واپس res;
}

ساکن خالی multiply_long(غیرعلامتی طویل a, غیرعلامتی طویل b,
			  غیرعلامتی طویل&کم, غیرعلامتی طویل&بلند)
{
      أكد(حجم(غیرعلامتی طویل) %2 == 0);

      ثابت غیرعلامتی طویل لفظ_نقاب = (1UL << 4UL*حجم(a)) - 1UL;
      غیرعلامتی طویل tmpa;
      غیرعلامتی طویل tmpb;
      غیرعلامتی طویل res[4];

      tmpa = a & لفظ_نقاب;
      tmpb = b & لفظ_نقاب;
      res[0] = tmpa * tmpb;
      res[1] = res[0] >> 4UL*حجم(غیرعلامتی طویل);
      res[0] &= لفظ_نقاب;

      tmpa = (a >> 4UL*حجم(غیرعلامتی طویل)) & لفظ_نقاب;
      res[1] += tmpa * tmpb;
      res[2] = res[1] >> 4UL*حجم(غیرعلامتی طویل);
      res[1] &= لفظ_نقاب;

      tmpa = a & لفظ_نقاب;
      tmpb = (b >> 4UL*حجم(غیرعلامتی طویل)) & لفظ_نقاب;
      res[1] += tmpa * tmpb;
      res[2] += res[1] >> 4UL*حجم(غیرعلامتی طویل);
      res[3]  = res[2] >> 4UL*حجم(غیرعلامتی طویل);
      res[1] &= لفظ_نقاب;
      res[2] &= لفظ_نقاب;

      tmpa = (a >> 4UL*حجم(غیرعلامتی طویل)) & لفظ_نقاب;
      res[2] += tmpa * tmpb;
      res[3] += res[2] >> 4UL*حجم(غیرعلامتی طویل);
      res[2] &= لفظ_نقاب;

      بلند = (res[3] << 4UL*حجم(غیرعلامتی طویل)) | res[2];
      کم  = (res[1] << 4UL*حجم(غیرعلامتی طویل)) | res[0];
}

vvp_vector2_t عامل * (ثابت vvp_vector2_t&a, ثابت vvp_vector2_t&b)
{
      ثابت غیرعلامتی بٹ_فی_لفظ_2 = 8 * حجم(a.vec_[0]);

	// The compiler ensures that the two operands are of equal size.
      أكد(a.size() == b.size());
      vvp_vector2_t r (0, a.size());

      غیرعلامتی الفاظ = (r.wid_ + بٹ_فی_لفظ_2 - 1) / بٹ_فی_لفظ_2;

      برائے (غیرعلامتی bdx = 0 ;  bdx < الفاظ ;  bdx += 1) {
	    غیرعلامتی طویل tmpb = b.vec_[bdx];
	    اگر (tmpb == 0)
		  جاری;

	    برائے (غیرعلامتی adx = 0 ;  adx < الفاظ ;  adx += 1) {
		  غیرعلامتی طویل tmpa = a.vec_[adx];
		  اگر (tmpa == 0)
			جاری;

		  غیرعلامتی طویل کم, hig;
		  multiply_long(tmpa, tmpb, کم, hig);

		  غیرعلامتی طویل حاصل = 0;
		  برائے (غیرعلامتی sdx = 0
			     ; (adx+bdx+sdx) < الفاظ
			     ;  sdx += 1) {

			r.vec_[adx+bdx+sdx] = جوڑو_حاصل(r.vec_[adx+bdx+sdx],
							کم, حاصل);
			کم = hig;
			hig = 0;
		  }
	    }
      }

      واپس r;
}

ساکن خالی div_mod (vvp_vector2_t dividend, vvp_vector2_t مقسوم_علیہ,
		     vvp_vector2_t&quotient, vvp_vector2_t&remainder)
{

      quotient = vvp_vector2_t(0, dividend.size());

      اگر (مقسوم_علیہ == quotient) {
	    cerr << "ERROR: division by zero, exiting." << سطر_ختم;
	    باہر_نکلو(255);
      }

      اگر (dividend < مقسوم_علیہ) {
	    remainder = dividend;
	    واپس;
      }

      vvp_vector2_t نقاب (1, dividend.size());

	// Make the dividend 1 bit larger to prevent overflow of
	// divtmp in startup.
      dividend = vvp_vector2_t(dividend, dividend.size()+1);
      vvp_vector2_t divtmp (مقسوم_علیہ, dividend.size());

      جب (divtmp < dividend) {
	    divtmp <<= 1;
	    نقاب <<= 1;
      }

      جب (dividend >= مقسوم_علیہ) {
	    اگر (divtmp <= dividend) {
		  dividend -= divtmp;
		  quotient += نقاب;
	    }

	    divtmp >>= 1;
	    نقاب >>= 1;
      }

      remainder = vvp_vector2_t(dividend, نقاب.size());
}

vvp_vector2_t عامل - (ثابت vvp_vector2_t&that)
{
      vvp_vector2_t neg(that);
      اگر (neg.wid_ == 0) واپس neg;

      ثابت غیرعلامتی الفاظ = (neg.wid_ + neg.بٹ_فی_لفظ-1) /
                                         neg.بٹ_فی_لفظ;
      برائے (غیرعلامتی idx = 0 ;  idx < الفاظ ;  idx += 1) {
	    neg.vec_[idx] = ~neg.vec_[idx];
      }
      neg += vvp_vector2_t(1, neg.wid_);

      واپس neg;
}

vvp_vector2_t عامل / (ثابت vvp_vector2_t&dividend,
			  ثابت vvp_vector2_t&مقسوم_علیہ)
{
      vvp_vector2_t quot, rem;
      div_mod(dividend, مقسوم_علیہ, quot, rem);
      واپس quot;
}

vvp_vector2_t عامل % (ثابت vvp_vector2_t&dividend,
			  ثابت vvp_vector2_t&مقسوم_علیہ)
{
      vvp_vector2_t quot, rem;
      div_mod(dividend, مقسوم_علیہ, quot, rem);
      واپس rem;
}

منطقی عامل > (ثابت vvp_vector2_t&a, ثابت vvp_vector2_t&b)
{
      ثابت غیرعلامتی awords = (a.wid_ + vvp_vector2_t::بٹ_فی_لفظ-1) / vvp_vector2_t::بٹ_فی_لفظ;
      ثابت غیرعلامتی bwords = (b.wid_ + vvp_vector2_t::بٹ_فی_لفظ-1) / vvp_vector2_t::بٹ_فی_لفظ;

      ثابت غیرعلامتی الفاظ = awords > bwords? awords : bwords;

      برائے (غیرعلامتی idx = الفاظ ;  idx > 0 ;  idx -= 1) {
	    غیرعلامتی طویل aw = (idx <= awords)? a.vec_[idx-1] : 0;
	    غیرعلامتی طویل bw = (idx <= bwords)? b.vec_[idx-1] : 0;

	    اگر (aw > bw)
		  واپس سچ;
	    اگر (aw < bw)
		  واپس جھوٹ;
      }

	// If the above loop finishes, then the vectors are equal.
      واپس جھوٹ;
}

منطقی عامل >= (ثابت vvp_vector2_t&a, ثابت vvp_vector2_t&b)
{
      ثابت غیرعلامتی awords = (a.wid_ + vvp_vector2_t::بٹ_فی_لفظ-1) / vvp_vector2_t::بٹ_فی_لفظ;
      ثابت غیرعلامتی bwords = (b.wid_ + vvp_vector2_t::بٹ_فی_لفظ-1) / vvp_vector2_t::بٹ_فی_لفظ;

      ثابت غیرعلامتی الفاظ = awords > bwords? awords : bwords;

      برائے (غیرعلامتی idx = الفاظ ;  idx > 0 ;  idx -= 1) {
	    غیرعلامتی طویل aw = (idx <= awords)? a.vec_[idx-1] : 0;
	    غیرعلامتی طویل bw = (idx <= bwords)? b.vec_[idx-1] : 0;

	    اگر (aw > bw)
		  واپس سچ;
	    اگر (aw < bw)
		  واپس جھوٹ;
      }

	// If the above loop finishes, then the vectors are equal.
      واپس سچ;
}

منطقی عامل < (ثابت vvp_vector2_t&a, ثابت vvp_vector2_t&b)
{
      ثابت غیرعلامتی awords = (a.wid_ + vvp_vector2_t::بٹ_فی_لفظ-1) / vvp_vector2_t::بٹ_فی_لفظ;
      ثابت غیرعلامتی bwords = (b.wid_ + vvp_vector2_t::بٹ_فی_لفظ-1) / vvp_vector2_t::بٹ_فی_لفظ;

      غیرعلامتی الفاظ = awords;
      اگر (bwords > الفاظ)
	    الفاظ = bwords;

      برائے (غیرعلامتی idx = الفاظ ;  idx > 0 ;  idx -= 1) {
	    غیرعلامتی طویل aw = (idx <= awords)? a.vec_[idx-1] : 0;
	    غیرعلامتی طویل bw = (idx <= bwords)? b.vec_[idx-1] : 0;

	    اگر (aw < bw)
		  واپس سچ;
	    اگر (aw > bw)
		  واپس جھوٹ;
      }

	// If the above loop finishes, then the vectors are equal.
      واپس جھوٹ;
}

منطقی عامل <= (ثابت vvp_vector2_t&a, ثابت vvp_vector2_t&b)
{
	// XXXX For now, only support equal width vectors.
      أكد(a.wid_ == b.wid_);

      ثابت غیرعلامتی awords = (a.wid_ + vvp_vector2_t::بٹ_فی_لفظ-1) / vvp_vector2_t::بٹ_فی_لفظ;

      برائے (غیرعلامتی idx = awords ;  idx > 0 ;  idx -= 1) {
	    اگر (a.vec_[idx-1] < b.vec_[idx-1])
		  واپس سچ;
	    اگر (a.vec_[idx-1] > b.vec_[idx-1])
		  واپس جھوٹ;
      }

	// If the above loop finishes, then the vectors are equal.
      واپس سچ;
}

منطقی عامل == (ثابت vvp_vector2_t&a, ثابت vvp_vector2_t&b)
{
      ثابت غیرعلامتی awords = (a.wid_ + vvp_vector2_t::بٹ_فی_لفظ-1) / vvp_vector2_t::بٹ_فی_لفظ;
      ثابت غیرعلامتی bwords = (b.wid_ + vvp_vector2_t::بٹ_فی_لفظ-1) / vvp_vector2_t::بٹ_فی_لفظ;

      ثابت غیرعلامتی الفاظ = awords > bwords? awords : bwords;

      برائے (غیرعلامتی idx = الفاظ ;  idx > 0 ;  idx -= 1) {
	    غیرعلامتی طویل aw = (idx <= awords)? a.vec_[idx-1] : 0;
	    غیرعلامتی طویل bw = (idx <= bwords)? b.vec_[idx-1] : 0;

	    اگر (aw > bw)
		  واپس جھوٹ;
	    اگر (aw < bw)
		  واپس جھوٹ;
      }

	// If the above loop finishes, then the vectors are equal.
      واپس سچ;
}


vvp_vector4_t vector2_مقصد_vector4(ثابت vvp_vector2_t&that, غیرعلامتی wid)
{
      vvp_vector4_t res (wid);

      برائے (غیرعلامتی idx = 0 ;  idx < res.size() ;  idx += 1) {
	    vvp_bit4_t ثنائی_ہندسہ = BIT4_0;

	    اگر (that.value(idx))
		  ثنائی_ہندسہ = BIT4_1;

	    res.سیٹ_ثنائی_ہندسہ(idx, ثنائی_ہندسہ);
      }

      واپس res;
}

ساکن منطقی c4string_سرخی_test(ثابت حرف*str)
{
      اگر ((str[0] != 'C') && (str[0] != 'c'))
	    واپس جھوٹ;
      اگر ((str[1] != '4') || (str[2] != '<'))
	    واپس جھوٹ;
      واپس سچ;
}

منطقی c4string_test(ثابت حرف*str)
{
      اگر (!c4string_سرخی_test(str))
	    واپس جھوٹ;
      size_t قدر_سائز = strspn(str+3, "01xz");
      اگر (str[3+قدر_سائز] != '>')
	    واپس جھوٹ;
      اگر (str[3+قدر_سائز+1] != 0)
	    واپس جھوٹ;

      واپس سچ;
}

vvp_vector4_t c4string_مقصد_vector4(ثابت حرف*str)
{
      أكد(c4string_سرخی_test(str));

      str += 3;
      ثابت حرف*tp = str + strspn(str,"01xz");
      أكد(tp[0] == '>');

      vvp_vector4_t tmp (tp-str);

      برائے (غیرعلامتی idx = 0 ;  idx < tmp.size() ;  idx += 1) {
	    vvp_bit4_t ثنائی_ہندسہ;
	    انتخاب (str[idx]) {
		حالت '0':
		  ثنائی_ہندسہ = BIT4_0;
		  توڑو;
		حالت '1':
		  ثنائی_ہندسہ = BIT4_1;
		  توڑو;
		حالت 'x':
		  ثنائی_ہندسہ = BIT4_X;
		  توڑو;
		حالت 'z':
		  ثنائی_ہندسہ = BIT4_Z;
		  توڑو;
		طےشدہ:
		  fprintf(stderr, "Unsupported bit value %c(%d).\n", str[idx],
		          str[idx]);
		  أكد(0);
		  ثنائی_ہندسہ = BIT4_0;
		  توڑو;
	    }
	    tmp.سیٹ_ثنائی_ہندسہ(tmp.size()-idx-1, ثنائی_ہندسہ);
      }

      واپس tmp;
}

ostream& عامل<< (ostream&out, ثابت vvp_vector2_t&that)
{
      اگر (that.is_NaN()) {
	    out << "NaN";

      } ورنہ {
	    out << vector2_مقصد_vector4(that, that.size());
      }
      واپس out;
}

vvp_vector8_t::vvp_vector8_t(ثابت vvp_vector8_t&that)
{
      سائز_2 = that.سائز_2;
      اگر (سائز_2 <= حجم(val_)) {
	    حافظہ_نقل(val_, that.val_, حجم val_);
      } ورنہ {
	    ptr_ = نیا غیرعلامتی حرف[سائز_2];
	    حافظہ_نقل(ptr_, that.ptr_, سائز_2);
      }
}

vvp_vector8_t::vvp_vector8_t(ثابت vvp_vector4_t&that,
			     غیرعلامتی str0, غیرعلامتی str1)
: سائز_2(that.size())
{
      اگر (سائز_2 == 0)
	    واپس;

      اگر (سائز_2 <= حجم(val_)) {
	    ptr_ = 0; // Prefill all val_ bytes
	    برائے (غیرعلامتی idx = 0 ; idx < سائز_2 ; idx += 1)
		  val_[idx] = vvp_scalar_t(that.value(idx),str0, str1).raw();
      } ورنہ {
	    ptr_ = نیا غیرعلامتی حرف[سائز_2];
	    برائے (غیرعلامتی idx = 0 ;  idx < سائز_2 ;  idx += 1)
		  ptr_[idx] = vvp_scalar_t(that.value(idx), str0, str1).raw();
      }
}

vvp_vector8_t::vvp_vector8_t(ثابت vvp_vector2_t&that,
			     غیرعلامتی str0, غیرعلامتی str1)
: سائز_2(that.size())
{
      اگر (سائز_2 == 0)
	    واپس;

      اگر (سائز_2 <= حجم(val_)) {
	    ptr_ = 0;
	    برائے (غیرعلامتی idx = 0 ; idx < سائز_2 ; idx += 1)
		  val_[idx] = vvp_scalar_t(that.value(idx)? BIT4_1:BIT4_0, str0, str1).raw();
      } ورنہ {
	    ptr_ = نیا غیرعلامتی حرف[سائز_2];
	    برائے (غیرعلامتی idx = 0 ; idx < سائز_2 ; idx += 1)
		  ptr_[idx] = vvp_scalar_t(that.value(idx)? BIT4_1:BIT4_0, str0, str1).raw();
      }
}

ثابت vvp_vector8_t vvp_vector8_t::nil;

vvp_vector8_t& vvp_vector8_t::عامل= (ثابت vvp_vector8_t&that)
{
	// Assign to self.
      اگر (یہ == &that)
	    واپس *یہ;

      اگر (سائز_2 != that.سائز_2) {
	    اگر (سائز_2 > حجم(val_))
		  حذف[]ptr_;
	    سائز_2 = 0;
      }

      اگر (that.سائز_2 == 0) {
	    أكد(سائز_2 == 0);
	    واپس *یہ;
      }

      اگر (that.سائز_2 <= حجم(val_)) {
	    سائز_2 = that.سائز_2;
	    حافظہ_نقل(val_, that.val_, حجم(val_));
	    واپس *یہ;
      }

      اگر (سائز_2 == 0) {
	    سائز_2 = that.سائز_2;
	    ptr_ = نیا غیرعلامتی حرف[سائز_2];
      }

      حافظہ_نقل(ptr_, that.ptr_, سائز_2);

      واپس *یہ;
}

vvp_vector8_t vvp_vector8_t::subvalue(غیرعلامتی بنیاد, غیرعلامتی wid) ثابت
{
      vvp_vector8_t tmp (wid);

      غیرعلامتی حرف*tmp_ptr = tmp.سائز_2 <= حجم(val_) ? tmp.val_ : tmp.ptr_;
      ثابت غیرعلامتی حرف*use_ptr = سائز_2 <= حجم(val_) ? val_ : ptr_;

      غیرعلامتی idx = 0;
      جب ((idx < wid) && (بنیاد+idx < سائز_2)) {
	    tmp_ptr[idx] = use_ptr[بنیاد+idx];
	    idx += 1;
      }

      واپس tmp;
}

خالی vvp_vector8_t::سیٹ_vec(غیرعلامتی بنیاد, ثابت vvp_vector8_t&that)
{
      أكد((بنیاد+that.size()) <= size());
      برائے (غیرعلامتی idx = 0 ; idx < that.size() ; idx += 1)
	    سیٹ_ثنائی_ہندسہ(بنیاد+idx, that.value(idx));
}

vvp_vector8_t part_expand(ثابت vvp_vector8_t&that, غیرعلامتی wid, غیرعلامتی off)
{
      أكد(off < wid);
      vvp_vector8_t tmp (wid);

      غیرعلامتی حرف* tmp_ptr = tmp.سائز_2<= حجم(tmp.val_) ?
                               tmp.val_ : tmp.ptr_;
      ثابت غیرعلامتی حرف* that_ptr = that.سائز_2<= حجم(that.val_) ?
                                      that.val_ : that.ptr_;

      غیرعلامتی idx = off;

      جب (idx < wid && that.سائز_2 > (idx-off)) {
	    tmp_ptr[idx] = that_ptr[idx-off];
	    idx += 1;
      }

      واپس tmp;
}

ساکن منطقی c8string_سرخی_test(ثابت حرف*str)
{
      اگر ((str[0] != 'C') && (str[0] != 'c'))
	    واپس جھوٹ;
      اگر ((str[1] != '8') || (str[2] != '<'))
	    واپس جھوٹ;
      واپس سچ;
}

منطقی c8string_test(ثابت حرف*str)
{
      اگر (!c8string_سرخی_test(str))
	    واپس جھوٹ;

      ثابت حرف*cp = str+3;
      برائے (;; cp += 1) {
	    اگر (cp[0] == '>' && cp[1] == 0) واپس سچ;
	    اگر (cp[0] >= '0' && cp[0] <= '9') جاری;
	    اگر (cp[0] == 'x') جاری;
	    اگر (cp[0] == 'z') جاری;
	    واپس جھوٹ;
      }
      واپس جھوٹ;
}
/*
 * The format of a C8<> string is:
 *   C8<aaabbbccc...>
 * where aaa... is a 3 character bit descriptor.
 */
vvp_vector8_t c8string_مقصد_vector8(ثابت حرف*str)
{
      أكد(c8string_سرخی_test(str));

      size_t vsize = لڑی_لمبائی(str)-4;
      أكد(vsize%3 == 0);
      vsize /= 3;
      vvp_vector8_t tmp (vsize);

      برائے (size_t idx = 0 ; idx < vsize ; idx += 1) {
	    ثابت حرف*cp = str+3+3*idx;
	    vvp_bit4_t ثنائی_ہندسہ = BIT4_X;
	    غیرعلامتی dr0 = cp[0]-'0';
	    غیرعلامتی dr1 = cp[1]-'0';
	    انتخاب (cp[2]) {
		حالت '0':
		  ثنائی_ہندسہ = BIT4_0;
		  توڑو;
		حالت '1':
		  ثنائی_ہندسہ = BIT4_1;
		  توڑو;
		حالت 'x':
		  ثنائی_ہندسہ = BIT4_X;
		  توڑو;
		حالت 'z':
		  ثنائی_ہندسہ = BIT4_Z;
		  توڑو;
	    }
	    tmp.سیٹ_ثنائی_ہندسہ(vsize-idx-1, vvp_scalar_t(ثنائی_ہندسہ, dr0, dr1));
      }

      واپس tmp;
}

ostream& عامل<<(ostream&out, ثابت vvp_vector8_t&that)
{
      out << "C8<";
      برائے (غیرعلامتی idx = 0 ;  idx < that.size() ; idx += 1)
	    out << that.value(that.size()-idx-1);

      out << ">";
      واپس out;
}

vvp_net_fun_t::vvp_net_fun_t()
{
      گنتی_functors += 1;
}

vvp_net_fun_t::~vvp_net_fun_t()
{
}

خالی vvp_net_fun_t::recv_vec4(vvp_net_ptr_t, ثابت vvp_vector4_t&,
                              vvp_context_t)
{
      fprintf(stderr, "internal error: %s: recv_vec4 not implemented\n",
	      typeid(*یہ).name());
      أكد(0);
}

خالی vvp_net_fun_t::recv_vec4_pv_(vvp_net_ptr_t p, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                                  غیرعلامتی بنیاد, غیرعلامتی vwid, vvp_context_t)
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

      أكد(بنیاد + ثنائی_ہندسہ.size() <= vwid);

      vvp_vector4_t tmp(vwid, BIT4_Z);
      tmp.سیٹ_vec(بنیاد, ثنائی_ہندسہ);
      recv_vec4(p, tmp, 0);
}

خالی vvp_net_fun_t::recv_vec4_pv(vvp_net_ptr_t, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                                 غیرعلامتی بنیاد, غیرعلامتی vwid, vvp_context_t)
{
      cerr << "internal error: " << typeid(*یہ).name() << ": "
	   << "recv_vec4_pv(" << ثنائی_ہندسہ << ", " << بنیاد
	   << ", " << vwid << ") not implemented" << سطر_ختم;
      أكد(0);
}

خالی vvp_net_fun_t::recv_vec8(vvp_net_ptr_t پورٹ, ثابت vvp_vector8_t&ثنائی_ہندسہ)
{
      recv_vec4(پورٹ, reduce4(ثنائی_ہندسہ), 0);
}

خالی vvp_net_fun_t::recv_vec8_pv_(vvp_net_ptr_t p, ثابت vvp_vector8_t&ثنائی_ہندسہ,
				  غیرعلامتی بنیاد, غیرعلامتی vwid)
{
	// This is the strength-aware version of recv_vec4_pv_.

      أكد(بنیاد + ثنائی_ہندسہ.size() <= vwid);

      vvp_vector8_t tmp(vwid);
      tmp.سیٹ_vec(بنیاد, ثنائی_ہندسہ);
      recv_vec8(p, tmp);
}

خالی vvp_net_fun_t::recv_vec8_pv(vvp_net_ptr_t پورٹ, ثابت vvp_vector8_t&ثنائی_ہندسہ,
				 غیرعلامتی بنیاد, غیرعلامتی vwid)
{
      recv_vec4_pv(پورٹ, reduce4(ثنائی_ہندسہ), بنیاد, vwid, 0);
}

خالی vvp_net_fun_t::recv_real(vvp_net_ptr_t, دوہرا ثنائی_ہندسہ, vvp_context_t)
{
      fprintf(stderr, "internal error: %s: recv_real(%f) not implemented\n",
	      typeid(*یہ).name(), ثنائی_ہندسہ);
      أكد(0);
}

خالی vvp_net_fun_t::recv_string(vvp_net_ptr_t, ثابت std::string&ثنائی_ہندسہ, vvp_context_t)
{
      fprintf(stderr, "internal error: %s: recv_string(%s) not implemented\n",
	      typeid(*یہ).name(), ثنائی_ہندسہ.c_str());
      أكد(0);
}

خالی vvp_net_fun_t::recv_object(vvp_net_ptr_t, vvp_object_t, vvp_context_t)
{
      fprintf(stderr, "internal error: %s: recv_object(...) not implemented\n",
	      typeid(*یہ).name());
      أكد(0);
}

خالی vvp_net_fun_t::force_flag(منطقی)
{
}

/* **** vvp_fun_drive methods **** */

vvp_fun_drive::vvp_fun_drive(غیرعلامتی str0, غیرعلامتی str1)
{
      أكد(str0 < 8);
      أكد(str1 < 8);

      drive0_ = str0;
      drive1_ = str1;
}

vvp_fun_drive::~vvp_fun_drive()
{
}

خالی vvp_fun_drive::recv_vec4(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                              vvp_context_t)
{
      أكد(پورٹ.پورٹ() == 0);
      پورٹ.ptr()->send_vec8(vvp_vector8_t(ثنائی_ہندسہ, drive0_, drive1_));
}

خالی vvp_fun_drive::recv_vec4_pv(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&ثنائی_ہندسہ,
				 غیرعلامتی بنیاد, غیرعلامتی vwid, vvp_context_t ctx)
{
      recv_vec4_pv_(ptr, ثنائی_ہندسہ, بنیاد, vwid, ctx);
}

/* **** vvp_wide_fun_* methods **** */

vvp_wide_fun_core::vvp_wide_fun_core(vvp_net_t*net, غیرعلامتی nports)
{
      ptr_ = net;
      nports_ = nports;
      پورٹ_اقدار = 0;
      پورٹ_rvalues = 0;
}

vvp_wide_fun_core::~vvp_wide_fun_core()
{
      حذف[] پورٹ_اقدار;
      حذف[] پورٹ_rvalues;
}

خالی vvp_wide_fun_core::propagate_vec4(ثابت vvp_vector4_t&ثنائی_ہندسہ,
				       vvp_time64_t delay)
{
      اگر (delay)
	    جدول_وقت_propagate_سمتیہ(ptr_, delay, ثنائی_ہندسہ);
      ورنہ
	    ptr_->send_vec4(ثنائی_ہندسہ, 0);
}

خالی vvp_wide_fun_core::propagate_real(دوہرا ثنائی_ہندسہ,
				       vvp_time64_t delay)
{
      اگر (delay) {
	      // schedule_assign_vector(ptr_->out, bit, delay);
	    أكد(0); // Need a real-value version of assign_vector.
      } ورنہ {
	    ptr_->send_real(ثنائی_ہندسہ, 0);
      }
}


غیرعلامتی vvp_wide_fun_core::پورٹ_گنتی() ثابت
{
      واپس nports_;
}

vvp_vector4_t& vvp_wide_fun_core::value(غیرعلامتی idx)
{
      أكد(idx < nports_);
      اگر (پورٹ_اقدار == 0)
	    پورٹ_اقدار = نیا vvp_vector4_t [nports_];
      واپس پورٹ_اقدار[idx];
}

دوہرا vvp_wide_fun_core::قدر_r(غیرعلامتی idx)
{
      أكد(idx < nports_);
      واپس پورٹ_rvalues? پورٹ_rvalues[idx] : 0.0;
}

خالی vvp_wide_fun_core::recv_real_from_inputs(غیرعلامتی)
{
      أكد(0);
}

خالی vvp_wide_fun_core::dispatch_vec4_from_داخلہ(غیرعلامتی پورٹ,
						  ثابت vvp_vector4_t&ثنائی_ہندسہ)
{
      أكد(پورٹ < nports_);
      اگر (پورٹ_اقدار == 0) پورٹ_اقدار = نیا vvp_vector4_t [nports_];
      پورٹ_اقدار[پورٹ] = ثنائی_ہندسہ;
      recv_vec4_from_inputs(پورٹ);
}

خالی vvp_wide_fun_core::dispatch_real_from_داخلہ(غیرعلامتی پورٹ,
						  دوہرا ثنائی_ہندسہ)
{
      أكد(پورٹ < nports_);
      اگر (پورٹ_rvalues == 0) پورٹ_rvalues = نیا دوہرا[nports_];
      پورٹ_rvalues[پورٹ] = ثنائی_ہندسہ;
      recv_real_from_inputs(پورٹ);
}

vvp_wide_fun_t::vvp_wide_fun_t(vvp_wide_fun_core*c, غیرعلامتی بنیاد)
: core_(c), پورٹ_بنیاد_2(بنیاد)
{
}

vvp_wide_fun_t::~vvp_wide_fun_t()
{
}

خالی vvp_wide_fun_t::recv_vec4(vvp_net_ptr_t پورٹ, ثابت vvp_vector4_t&ثنائی_ہندسہ,
                               vvp_context_t)
{
      غیرعلامتی pidx = پورٹ_بنیاد_2 + پورٹ.پورٹ();
      core_->dispatch_vec4_from_داخلہ(pidx, ثنائی_ہندسہ);
}

خالی vvp_wide_fun_t::recv_vec4_pv(vvp_net_ptr_t ptr, ثابت vvp_vector4_t&ثنائی_ہندسہ,
				  غیرعلامتی بنیاد, غیرعلامتی vwid, vvp_context_t ctx)
{
      recv_vec4_pv_(ptr, ثنائی_ہندسہ, بنیاد, vwid, ctx);
}

خالی vvp_wide_fun_t::recv_real(vvp_net_ptr_t پورٹ, دوہرا ثنائی_ہندسہ,
                               vvp_context_t)
{
      غیرعلامتی pidx = پورٹ_بنیاد_2 + پورٹ.پورٹ();
      core_->dispatch_real_from_داخلہ(pidx, ثنائی_ہندسہ);
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

غیرعلامتی vvp_scalar_t::strength0() ثابت
{
      واپس STREN0(قدر_2);
}

غیرعلامتی vvp_scalar_t::strength1() ثابت
{
      واپس STREN1(قدر_2);
}

ostream& عامل <<(ostream&out, vvp_scalar_t a)
{
      out << a.strength0() << a.strength1();
      انتخاب (a.value()) {
	  حالت BIT4_0:
	    out << "0";
	    توڑو;
	  حالت BIT4_1:
	    out << "1";
	    توڑو;
	  حالت BIT4_X:
	    out << "X";
	    توڑو;
	  حالت BIT4_Z:
	    out << "Z";
	    توڑو;
      }
      واپس out;
}

/*
 * This function is only called if the actual interface function rules
 * out some of the easy cases. If we get here, we can assume that
 * neither of the values is HiZ, and the values are not exactly equal.
 */
vvp_scalar_t fully_featured_resolv_(vvp_scalar_t a, vvp_scalar_t b)
{

      اگر (UNAMBIG(a.قدر_2) && UNAMBIG(b.قدر_2)) {

	      /* If both signals are unambiguous, simply choose
		 the stronger. If they have the same strength
		 but different values, then this becomes
		 ambiguous. */

	    اگر ((b.قدر_2&0x07) > (a.قدر_2&0x07)) {

		    /* b value is stronger. Take it. */
		  واپس b;

	    } ورنہ اگر ((b.قدر_2&0x77) == (a.قدر_2&0x77)) {

		    // Strengths are the same. Since we know already
		    // that the values are not the same, Make value
		    // into "x".
		  vvp_scalar_t tmp (a);
		  tmp.قدر_2 = (tmp.قدر_2&0x77) | 0x80;
		  واپس tmp;

	    } ورنہ {

		    /* Must be "a" is the stronger one. */
		  واپس a;
	    }

      }

	/* If one of the signals is unambiguous, then it
	   will sweep up the weaker parts of the ambiguous
	   signal. The result may be ambiguous, or maybe not. */

      اگر (UNAMBIG(a.قدر_2)) {
	    vvp_scalar_t res;

	    اگر ((a.قدر_2&0x70) > (b.قدر_2&0x70))
		  res.قدر_2 |= a.قدر_2&0xf0;
	    ورنہ
		  res.قدر_2 |= b.قدر_2&0xf0;

	    اگر ((a.قدر_2&0x07) > (b.قدر_2&0x07))
		  res.قدر_2 |= a.قدر_2&0x0f;
	    ورنہ
		  res.قدر_2 |= b.قدر_2&0x0f;

	    واپس res;

      } ورنہ اگر (UNAMBIG(b.قدر_2)) {

	    vvp_scalar_t res;

	    اگر ((b.قدر_2&0x70) > (a.قدر_2&0x70))
		  res.قدر_2 |= b.قدر_2&0xf0;
	    ورنہ
		  res.قدر_2 |= a.قدر_2&0xf0;

	    اگر ((b.قدر_2&0x07) > (a.قدر_2&0x07))
		  res.قدر_2 |= b.قدر_2&0x0f;
	    ورنہ
		  res.قدر_2 |= a.قدر_2&0x0f;

	    واپس res;

      }


	/* If both signals are ambiguous, then the result
	   has an even wider ambiguity. */

      غیرعلامتی tmp = 0;
      صحیح sv1a = (a.قدر_2 & 0x80) ? STREN1(a.قدر_2) : - STREN1(a.قدر_2);
      صحیح sv0a = (a.قدر_2 & 0x08) ? STREN0(a.قدر_2) : - STREN0(a.قدر_2);
      صحیح sv1b = (b.قدر_2 & 0x80) ? STREN1(b.قدر_2) : - STREN1(b.قدر_2);
      صحیح sv0b = (b.قدر_2 & 0x08) ? STREN0(b.قدر_2) : - STREN0(b.قدر_2);

      صحیح sv1 = sv1a;
      صحیح sv0 = sv0a;

      اگر (sv0a > sv1)
	    sv1 = sv0a;
      اگر (sv1b > sv1)
	    sv1 = sv1b;
      اگر (sv0b > sv1)
	    sv1 = sv0b;

      اگر (sv1a < sv0)
	    sv0 = sv1a;
      اگر (sv1b < sv0)
	    sv0 = sv1b;
      اگر (sv0b < sv0)
	    sv0 = sv0b;

      اگر (sv1 > 0) {
	    tmp |= 0x80;
	    tmp |= sv1 << 4;
      } ورنہ {
	      /* Set the MSB when both arguments MSBs are set. This
		 can only happen if both one strengths are zero. */
	    tmp |= (a.قدر_2&b.قدر_2)&0x80;
	    tmp |= (-sv1) << 4;
      }

      اگر (sv0 > 0) {
	    tmp |= 0x08;
	    tmp |= sv0;
      } ورنہ {
	    tmp |= (-sv0);
      }

      vvp_scalar_t res;
      res.قدر_2 = tmp;

	/* Canonicalize the HiZ value. */
      اگر ((res.قدر_2&0x77) == 0)
	    res.قدر_2 = 0;

      واپس res;
}

غیرعلامتی vvp_switch_strength_map[2][8] = {
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
      برائے (غیرعلامتی idx = 0 ;  idx < out.size() ;  idx += 1)
	    out.سیٹ_ثنائی_ہندسہ(idx, that.value(idx).value());

      واپس out;
}

vvp_bit4_t compare_gtge(ثابت vvp_vector4_t&lef, ثابت vvp_vector4_t&rig,
			vvp_bit4_t برآمد_if_equal)
{
      غیرعلامتی min_سائز = lef.size();
      اگر (rig.size() < min_سائز)
	    min_سائز = rig.size();

	// If one of the inputs is nil, treat is as all X values, and
	// that makes the result BIT4_X.
      اگر (min_سائز == 0)
	    واپس BIT4_X;

	// As per the IEEE1364 definition of >, >=, < and <=, if there
	// are any X or Z values in either of the operand vectors,
	// then the result of the compare is BIT4_X.

	// Check for X/Z in the left operand
      اگر (lef.has_xz())
	    واپس BIT4_X;

	// Check for X/Z in the right operand
      اگر (rig.has_xz())
	    واپس BIT4_X;

      برائے (غیرعلامتی idx = lef.size() ; idx > rig.size() ;  idx -= 1) {
	    اگر (lef.value(idx-1) == BIT4_1)
		  واپس BIT4_1;
      }

      برائے (غیرعلامتی idx = rig.size() ; idx > lef.size() ;  idx -= 1) {
	    اگر (rig.value(idx-1) == BIT4_1)
		  واپس BIT4_0;
      }

      برائے (غیرعلامتی idx = min_سائز ; idx > 0 ;  idx -= 1) {
	    vvp_bit4_t lv = lef.value(idx-1);
	    vvp_bit4_t rv = rig.value(idx-1);

	    اگر (lv == rv)
		  جاری;

	    اگر (lv == BIT4_1)
		  واپس BIT4_1;
	    ورنہ
		  واپس BIT4_0;
      }

      واپس برآمد_if_equal;
}

vvp_bit4_t compare_gtge_signed(ثابت vvp_vector4_t&a,
			       ثابت vvp_vector4_t&b,
			       vvp_bit4_t برآمد_if_equal)
{
      أكد(a.size() == b.size());

      غیرعلامتی sign_idx = a.size()-1;
      vvp_bit4_t a_sign = a.value(sign_idx);
      vvp_bit4_t b_sign = b.value(sign_idx);

      اگر (bit4_is_xz(a_sign))
	    واپس BIT4_X;
      اگر (bit4_is_xz(b_sign))
	    واپس BIT4_X;

      اگر (a_sign == b_sign)
	    واپس compare_gtge(a, b, برآمد_if_equal);

      اگر (a.has_xz())
	    واپس BIT4_X;

      اگر (b.has_xz())
	    واپس BIT4_X;

      اگر(a_sign == BIT4_0)
	    واپس BIT4_1;
      ورنہ
	    واپس BIT4_0;
}
