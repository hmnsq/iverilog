/*
 * Copyright (c) 2001-2026 Stephen Williams (steve@icarus.com)
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
# include  "schedule.h"
# include  "vthread.h"
# include  "vpi_priv.h"
# include  "vvp_net_sig.h"
# include  "slab.h"
# include  "compile.h"
# include  <new>
# include  <typeinfo>
# include  <csignal>
# include  <cstdlib>
# include  <cassert>
# include  <iostream>
#यदि_परिभाषितम् CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
# include  "ivl_alloc.h"
#यदि_अन्तः

उपयुज्य नामाकाश std;

अचिह्नित दीर्घ गणनम्_नियोजय_events = 0;
अचिह्नित दीर्घ गणनम्_gen_events = 0;
अचिह्नित दीर्घ गणनम्_thread_events = 0;
  // Count the time events (A time cell created)
अचिह्नित दीर्घ गणनम्_time_events = 0;



/*
 * The event_s and event_time_s structures implement the Verilog
 * stratified event queue.
 *
 * The event_time_s objects are one per time step. Each time step in
 * turn contains a list of event_s objects that are the actual events.
 *
 * The event_s objects are base classes for the more specific sort of
 * event.
 */
संरचना event_s {
      संरचना event_s*अग्र_2;
      आभास ~event_s() { }
      आभास शून्य सततम्_सततम्(शून्य) =0;

	// Write something about the event to stderr
      आभास शून्य single_चरणः_display(शून्य);

	// Fallback new/delete
      स्थिर शून्य*संक्रियकः नूतन (size_t size) { प्रत्यागम ::नूतन वर्ण[size]; }
      स्थिर शून्य संक्रियकः विलोपय(शून्य*ptr)  { ::विलोपय[]( static_cast<वर्ण*>(ptr) ); }
};

शून्य event_s::single_चरणः_display(शून्य)
{
      std::cerr << "event_s: Step into event " << typeid(*एतत्).name() << std::पङ्क्त्यन्तः;
}

संरचना event_time_s {
      event_time_s() {
	    गणनम्_time_events += 1;
	    delay = 0;
	    start = 0;
	    सक्रिय = 0;
	    inactive = 0;
	    nbassign = 0;
	    rwsync = 0;
	    rosync = 0;
	    del_thr = 0;
	    अग्र_2 = रिक्त;
      }
      vvp_time64_t delay;

      संरचना event_s*start;
      संरचना event_s*सक्रिय;
      संरचना event_s*inactive;
      संरचना event_s*nbassign;
      संरचना event_s*rwsync;
      संरचना event_s*rosync;
      संरचना event_s*del_thr;

      संरचना event_time_s*अग्र_2;

      स्थिर शून्य* संक्रियकः नूतन (size_t);
      स्थिर शून्य संक्रियकः विलोपय(शून्य*obj, size_t s);
};

vvp_gen_event_s::~vvp_gen_event_s()
{
}

शून्य vvp_gen_event_s::single_चरणः_display(शून्य)
{
      cerr << "vvp_gen_event_s: Step into event " << typeid(*एतत्).name() << पङ्क्त्यन्तः;
}

/*
 * Derived event types
 */
संरचना vthread_event_s : सार्वजनिक event_s {
      vthread_t thr;
      शून्य सततम्_सततम्(शून्य) override;
      शून्य single_चरणः_display(शून्य) override;

      स्थिर शून्य* संक्रियकः नूतन(size_t);
      स्थिर शून्य संक्रियकः विलोपय(शून्य*);
};

शून्य vthread_event_s::सततम्_सततम्(शून्य)
{
      गणनम्_thread_events += 1;
      vthread_सततम्(thr);
}

शून्य vthread_event_s::single_चरणः_display(शून्य)
{
      __vpiScope*scope = vthread_scope(thr);
      cerr << "vthread_event: Resume thread"
	   << " scope=" << scope->vpi_get_str(vpi_पूर्णम्_Name)
	   << पङ्क्त्यन्तः;
}

स्थिर ध्रुव size_t VTHR_CHUNK_गणनम् = 8192 / परिमाण(संरचना vthread_event_s);
स्थिर slab_t<परिमाण(vthread_event_s),VTHR_CHUNK_गणनम्> vthread_event_heap;

अन्तःस्थ शून्य* vthread_event_s::संक्रियकः नूतन(size_t size)
{
      प्रतिज्ञा(size == परिमाण(vthread_event_s));
      प्रत्यागम vthread_event_heap.alloc_slab();
}

शून्य vthread_event_s::संक्रियकः विलोपय(शून्य*dptr)
{
      vthread_event_heap.free_slab(dptr);
}

संरचना del_thr_event_s : सार्वजनिक event_s {
      vthread_t thr;
      शून्य सततम्_सततम्(शून्य) override;
      शून्य single_चरणः_display(शून्य) override;
};

शून्य del_thr_event_s::सततम्_सततम्(शून्य)
{
      vthread_delete(thr);
}

शून्य del_thr_event_s::single_चरणः_display(शून्य)
{
      __vpiScope*scope = vthread_scope(thr);
      cerr << "del_thr_event: Reap completed thread"
	   << " scope=" << scope->vpi_get_str(vpi_पूर्णम्_Name) << पङ्क्त्यन्तः;
}

संरचना नियोजय_vector4_event_s  : सार्वजनिक event_s {
	/* The default constructor. */
      स्पष्ट नियोजय_vector4_event_s(ध्रुव vvp_vector4_t&that) : val(that) {
	    आधारः = 0;
	    vwid = 0;
	    अग्र_2 = रिक्त;
      }

	/* Where to do the assign. */
      vvp_net_ptr_t ptr;
	/* Value to assign. */
      vvp_vector4_t val;
	/* Offset of the part into the destination. */
      अचिह्नित आधारः;
	/* Width of the destination vector. */
      अचिह्नित vwid;
      शून्य सततम्_सततम्(शून्य) override;
      शून्य single_चरणः_display(शून्य) override;

      स्थिर शून्य* संक्रियकः नूतन(size_t);
      स्थिर शून्य संक्रियकः विलोपय(शून्य*);
};

शून्य नियोजय_vector4_event_s::सततम्_सततम्(शून्य)
{
      गणनम्_नियोजय_events += 1;
      यदि (vwid > 0)
	    vvp_send_vec4_pv(ptr, val, आधारः, vwid, 0);
      अन्यथा
	    vvp_send_vec4(ptr, val, 0);
}

शून्य नियोजय_vector4_event_s::single_चरणः_display(शून्य)
{
      cerr << "assign_vector4_event: Propagate val=" << val
	   << ", vwid=" << vwid << ", base=" << आधारः << पङ्क्त्यन्तः;
}

स्थिर ध्रुव size_t ASSIGN4_CHUNK_गणनम् = 524288 / परिमाण(संरचना नियोजय_vector4_event_s);
स्थिर slab_t<परिमाण(नियोजय_vector4_event_s),ASSIGN4_CHUNK_गणनम्> assign4_heap;

अन्तःस्थ शून्य* नियोजय_vector4_event_s::संक्रियकः नूतन(size_t size)
{
      प्रतिज्ञा(size == परिमाण(नियोजय_vector4_event_s));
      प्रत्यागम assign4_heap.alloc_slab();
}

शून्य नियोजय_vector4_event_s::संक्रियकः विलोपय(शून्य*dptr)
{
      assign4_heap.free_slab(dptr);
}

अचिह्नित दीर्घ गणनम्_assign4_pool(शून्य) { प्रत्यागम assign4_heap.pool; }

संरचना नियोजय_vector8_event_s  : सार्वजनिक event_s {
      vvp_net_ptr_t ptr;
      vvp_vector8_t val;
      शून्य सततम्_सततम्(शून्य) override;
      शून्य single_चरणः_display(शून्य) override;

      स्थिर शून्य* संक्रियकः नूतन(size_t);
      स्थिर शून्य संक्रियकः विलोपय(शून्य*);
};

शून्य नियोजय_vector8_event_s::सततम्_सततम्(शून्य)
{
      गणनम्_नियोजय_events += 1;
      vvp_send_vec8(ptr, val);
}

शून्य नियोजय_vector8_event_s::single_चरणः_display(शून्य)
{
      cerr << "assign_vector8_event: Propagate val=" << val << पङ्क्त्यन्तः;
}

स्थिर ध्रुव size_t ASSIGN8_CHUNK_गणनम् = 8192 / परिमाण(संरचना नियोजय_vector8_event_s);
स्थिर slab_t<परिमाण(नियोजय_vector8_event_s),ASSIGN8_CHUNK_गणनम्> assign8_heap;

अन्तःस्थ शून्य* नियोजय_vector8_event_s::संक्रियकः नूतन(size_t size)
{
      प्रतिज्ञा(size == परिमाण(नियोजय_vector8_event_s));
      प्रत्यागम assign8_heap.alloc_slab();
}

शून्य नियोजय_vector8_event_s::संक्रियकः विलोपय(शून्य*dptr)
{
      assign8_heap.free_slab(dptr);
}

अचिह्नित दीर्घ गणनम्_assign8_pool() { प्रत्यागम assign8_heap.pool; }

संरचना नियोजय_real_event_s  : सार्वजनिक event_s {
      vvp_net_ptr_t ptr;
      द्विगुण val;
      शून्य सततम्_सततम्(शून्य) override;
      शून्य single_चरणः_display(शून्य) override;

      स्थिर शून्य* संक्रियकः नूतन(size_t);
      स्थिर शून्य संक्रियकः विलोपय(शून्य*);
};

शून्य नियोजय_real_event_s::सततम्_सततम्(शून्य)
{
      गणनम्_नियोजय_events += 1;
      vvp_send_real(ptr, val, 0);
}

शून्य नियोजय_real_event_s::single_चरणः_display(शून्य)
{
      cerr << "assign_real_event: Propagate val=" << val << पङ्क्त्यन्तः;
}

स्थिर ध्रुव size_t ASSIGNR_CHUNK_गणनम् = 8192 / परिमाण(संरचना नियोजय_real_event_s);
स्थिर slab_t<परिमाण(नियोजय_real_event_s),ASSIGNR_CHUNK_गणनम्> assignr_heap;

अन्तःस्थ शून्य* नियोजय_real_event_s::संक्रियकः नूतन (size_t size)
{
      प्रतिज्ञा(size == परिमाण(नियोजय_real_event_s));
      प्रत्यागम assignr_heap.alloc_slab();
}

शून्य नियोजय_real_event_s::संक्रियकः विलोपय(शून्य*dptr)
{
      assignr_heap.free_slab(dptr);
}

अचिह्नित दीर्घ गणनम्_नियोजय_real_pool(शून्य) { प्रत्यागम assignr_heap.pool; }

संरचना नियोजय_array_word_s  : सार्वजनिक event_s {
      vvp_array_t mem;
      अचिह्नित adr;
      vvp_vector4_t val;
      अचिह्नित off;
      शून्य सततम्_सततम्(शून्य) override;

      स्थिर शून्य* संक्रियकः नूतन(size_t);
      स्थिर शून्य संक्रियकः विलोपय(शून्य*);
};

शून्य नियोजय_array_word_s::सततम्_सततम्(शून्य)
{
      गणनम्_नियोजय_events += 1;
      mem->समुच्चयः_word(adr, off, val);
}

स्थिर ध्रुव size_t ARRAY_W_CHUNK_गणनम् = 8192 / परिमाण(संरचना नियोजय_array_word_s);
स्थिर slab_t<परिमाण(नियोजय_array_word_s),ARRAY_W_CHUNK_गणनम्> array_w_heap;

अन्तःस्थ शून्य* नियोजय_array_word_s::संक्रियकः नूतन (size_t size)
{
      प्रतिज्ञा(size == परिमाण(नियोजय_array_word_s));
      प्रत्यागम array_w_heap.alloc_slab();
}

शून्य नियोजय_array_word_s::संक्रियकः विलोपय(शून्य*ptr)
{
      array_w_heap.free_slab(ptr);
}

अचिह्नित दीर्घ गणनम्_नियोजय_aword_pool(शून्य) { प्रत्यागम array_w_heap.pool; }

संरचना force_vector4_event_s  : सार्वजनिक event_s {
	/* The default constructor. */
      स्पष्ट force_vector4_event_s(ध्रुव vvp_vector4_t&that): val(that) {
	    net = रिक्त;
	    आधारः = 0;
	    vwid = 0;
	    अग्र_2 = रिक्त;
      }
	/* Where to do the force. */
      vvp_net_t*net;
	/* Value to force. */
      vvp_vector4_t val;
	/* Offset of the part into the destination. */
      अचिह्नित आधारः;
	/* Width of the destination vector. */
      अचिह्नित vwid;

      शून्य सततम्_सततम्(शून्य) override;
      शून्य single_चरणः_display(शून्य) override;

      स्थिर शून्य* संक्रियकः नूतन(size_t);
      स्थिर शून्य संक्रियकः विलोपय(शून्य*);
};

शून्य force_vector4_event_s::सततम्_सततम्(शून्य)
{
      गणनम्_नियोजय_events += 1;

      अचिह्नित wid = val.size();
      यदि ((आधारः + wid) > vwid)
	    wid = vwid - आधारः;

	// Make a mask of which bits are to be forced, 0 for unforced
	// bits and 1 for forced bits.
      vvp_vector2_t आवरणम् (vvp_vector2_t::FILL0, vwid);
      प्रति (अचिह्नित idx = 0 ; idx < wid ; idx += 1)
	    आवरणम्.समुच्चयः_bit(आधारः+idx, 1);

      vvp_vector4_t tmp (vwid, BIT4_Z);

	// vvp_net_t::force_vec4 propagates all the bits of the
	// forced vector value, regardless of the mask. This
	// ensures the unforced bits retain their current value.
      vvp_signal_मानम्*sig = dynamic_cast<vvp_signal_मानम्*>(net->fil);
      प्रतिज्ञा(sig);
      sig->vec4_मानम्(tmp);

      tmp.समुच्चयः_vec(आधारः, val);
      net->force_vec4(tmp, आवरणम्);
}

शून्य force_vector4_event_s::single_चरणः_display(शून्य)
{
      cerr << "force_vector4_event: Force val=" << val
	   << ", vwid=" << vwid << ", base=" << आधारः << पङ्क्त्यन्तः;
}

स्थिर ध्रुव size_t FORCE4_CHUNK_गणनम् = 8192 / परिमाण(संरचना force_vector4_event_s);
स्थिर slab_t<परिमाण(force_vector4_event_s),FORCE4_CHUNK_गणनम्> force4_heap;

अन्तःस्थ शून्य* force_vector4_event_s::संक्रियकः नूतन(size_t size)
{
      प्रतिज्ञा(size == परिमाण(force_vector4_event_s));
      प्रत्यागम force4_heap.alloc_slab();
}

शून्य force_vector4_event_s::संक्रियकः विलोपय(शून्य*dptr)
{
      force4_heap.free_slab(dptr);
}

अचिह्नित दीर्घ गणनम्_force4_pool(शून्य) { प्रत्यागम force4_heap.pool; }

/*
 * This class supports the propagation of vec4 outputs from a
 * vvp_net_t object.
 */
संरचना propagate_vector4_event_s : सार्वजनिक event_s {
	/* The default constructor. */
      स्पष्ट propagate_vector4_event_s(ध्रुव vvp_vector4_t&that) : val(that) {
	    net = रिक्त;
	    अग्र_2 = रिक्त;
      }
	/* A constructor that makes the val directly. */
      propagate_vector4_event_s(ध्रुव vvp_vector4_t&that, अचिह्नित adr, अचिह्नित wid)
      : val(that,adr,wid) {
	    net = रिक्त;
	    अग्र_2 = रिक्त;
      }

	/* Propagate the output of this net. */
      vvp_net_t*net;
	/* value to propagate */
      vvp_vector4_t val;
	/* Action */
      शून्य सततम्_सततम्(शून्य) override;
      शून्य single_चरणः_display(शून्य) override;
};

शून्य propagate_vector4_event_s::सततम्_सततम्(शून्य)
{
      net->send_vec4(val, 0);
}

शून्य propagate_vector4_event_s::single_चरणः_display(शून्य)
{
      cerr << "propagate_vector4_event: Propagate val=" << val << पङ्क्त्यन्तः;
}

/*
 * This class supports the propagation of real outputs from a
 * vvp_net_t object.
 */
संरचना propagate_real_event_s : सार्वजनिक event_s {
	/* Propagate the output of this net. */
      vvp_net_t*net;
	/* value to propagate */
      द्विगुण val;
	/* Action */
      शून्य सततम्_सततम्(शून्य) override;
      शून्य single_चरणः_display(शून्य) override;
};

शून्य propagate_real_event_s::सततम्_सततम्(शून्य)
{
      net->send_real(val, 0);
}

शून्य propagate_real_event_s::single_चरणः_display(शून्य)
{
      cerr << "propagate_real_event: Propagate val=" << val << पङ्क्त्यन्तः;
}

संरचना नियोजय_array_r_word_s  : सार्वजनिक event_s {
      vvp_array_t mem;
      अचिह्नित adr;
      द्विगुण val;
      शून्य सततम्_सततम्(शून्य) override;

      स्थिर शून्य* संक्रियकः नूतन(size_t);
      स्थिर शून्य संक्रियकः विलोपय(शून्य*);
};

शून्य नियोजय_array_r_word_s::सततम्_सततम्(शून्य)
{
      गणनम्_नियोजय_events += 1;
      mem->समुच्चयः_word(adr, val);
}
स्थिर ध्रुव size_t ARRAY_R_W_CHUNK_गणनम् = 8192 / परिमाण(संरचना नियोजय_array_r_word_s);
स्थिर slab_t<परिमाण(नियोजय_array_r_word_s),ARRAY_R_W_CHUNK_गणनम्> array_r_w_heap;

अन्तःस्थ शून्य* नियोजय_array_r_word_s::संक्रियकः नूतन(size_t size)
{
      प्रतिज्ञा(size == परिमाण(नियोजय_array_r_word_s));
      प्रत्यागम array_r_w_heap.alloc_slab();
}

शून्य नियोजय_array_r_word_s::संक्रियकः विलोपय(शून्य*ptr)
{
      array_r_w_heap.free_slab(ptr);
}

अचिह्नित दीर्घ गणनम्_नियोजय_arword_pool(शून्य) { प्रत्यागम array_r_w_heap.pool; }

संरचना generic_event_s : सार्वजनिक event_s {
      vvp_gen_event_t obj;
      तार्किक delete_obj_when_समाप्तम्;
      शून्य सततम्_सततम्(शून्य) override;
      शून्य single_चरणः_display(शून्य) override;

      स्थिर शून्य* संक्रियकः नूतन(size_t);
      स्थिर शून्य संक्रियकः विलोपय(शून्य*);
};

शून्य generic_event_s::सततम्_सततम्(शून्य)
{
      गणनम्_gen_events += 1;
      यदि (obj) {
	    obj->सततम्_सततम्();
	    यदि (delete_obj_when_समाप्तम्)
		  विलोपय obj;
      }
}

शून्य generic_event_s::single_चरणः_display(शून्य)
{
      obj->single_चरणः_display();
}

स्थिर ध्रुव size_t GENERIC_CHUNK_गणनम् = 131072 / परिमाण(संरचना generic_event_s);
स्थिर slab_t<परिमाण(generic_event_s),GENERIC_CHUNK_गणनम्> generic_event_heap;

अन्तःस्थ शून्य* generic_event_s::संक्रियकः नूतन(size_t size)
{
      प्रतिज्ञा(size == परिमाण(generic_event_s));
      प्रत्यागम generic_event_heap.alloc_slab();
}

शून्य generic_event_s::संक्रियकः विलोपय(शून्य*ptr)
{
      generic_event_heap.free_slab(ptr);
}

अचिह्नित दीर्घ गणनम्_gen_pool(शून्य) { प्रत्यागम generic_event_heap.pool; }

/*
** These event_time_s will be required a lot, at high frequency.
** Once allocated, we never free them, but stash them away for next time.
*/


स्थिर ध्रुव size_t TIME_CHUNK_गणनम् = 8192 / परिमाण(संरचना event_time_s);
स्थिर slab_t<परिमाण(event_time_s),TIME_CHUNK_गणनम्> event_time_heap;

अन्तःस्थ शून्य* event_time_s::संक्रियकः नूतन (size_t size)
{
      प्रतिज्ञा(size == परिमाण(संरचना event_time_s));
      शून्य*ptr = event_time_heap.alloc_slab();
      प्रत्यागम ptr;
}

अन्तःस्थ शून्य event_time_s::संक्रियकः विलोपय(शून्य*ptr, size_t)
{
      event_time_heap.free_slab(ptr);
}

अचिह्नित दीर्घ गणनम्_time_pool(शून्य) { प्रत्यागम event_time_heap.pool; }

/*
 * This is the head of the list of pending events. This includes all
 * the events that have not been executed yet, and reaches into the
 * future.
 */
स्थिर संरचना event_time_s* sched_list = 0;

/*
 * This is a list of initialization events. The setup puts
 * initializations in this list so that they happen before the
 * simulation as a whole starts. This prevents time-0 triggers of
 * certain events.
 */
स्थिर संरचना event_s* कालसारणी_init_list = 0;

/*
 * This is the head of the list of final events.
 */
स्थिर संरचना event_s* कालसारणी_final_list = 0;

/*
 * This flag is true until a VPI task or function finishes the
 * simulation.
 */
स्थिर तार्किक कालसारणी_runnable = सत्य;
स्थिर तार्किक कालसारणी_stopped_flag  = असत्य;
स्थिर तार्किक कालसारणी_single_चरणः_flag = असत्य;
स्थिर तार्किक no_सङ्केताः_flag = असत्य;

शून्य कालसारणी_finish(पूर्णाङ्क)
{
      कालसारणी_runnable = असत्य;
}

शून्य कालसारणी_stop(पूर्णाङ्क)
{
      कालसारणी_stopped_flag = सत्य;
}

शून्य कालसारणी_single_चरणः(पूर्णाङ्क)
{
      कालसारणी_single_चरणः_flag = सत्य;
}

तार्किक कालसारणी_परिसमाप्तम्(शून्य)
{
      प्रत्यागम !कालसारणी_runnable;
}

तार्किक कालसारणी_stopped(शून्य)
{
      प्रत्यागम कालसारणी_stopped_flag;
}

बाह्य "C" शून्य vvp_no_सङ्केताः(शून्य)
{
      no_सङ्केताः_flag = सत्य;
}

/*
 * These are the signal handling infrastructure. The SIGINT signal
 * leads to an implicit $stop. The SIGHUP and SIGTERM signals lead
 * to an implicit $finish.
 */
बाह्य तार्किक stop_is_finish;

बाह्य "C" शून्य सङ्केताः_handler(पूर्णाङ्क signum)
{
#यदि_परिभाषितम् __MINGW32__
	// Windows implements the original UNIX semantics for signal,
	// so we have to re-establish the signal handler each time a
	// signal is caught.
      सङ्केत(signum, &सङ्केताः_handler);
#यदि_अन्तः
      यदि (signum != SIGINT)
	    stop_is_finish = सत्य;
      कालसारणी_stopped_flag = सत्य;
}

स्थिर शून्य सङ्केताः_capture(शून्य)
{
      यदि (no_सङ्केताः_flag)
	   प्रत्यागम;
#यदि_अपरिभाषितम् __MINGW32__
      सङ्केत(SIGHUP,  &सङ्केताः_handler);
#यदि_अन्तः
      सङ्केत(SIGINT,  &सङ्केताः_handler);
      सङ्केत(SIGTERM, &सङ्केताः_handler);
}

स्थिर शून्य सङ्केताः_revert(शून्य)
{
      यदि (no_सङ्केताः_flag)
	   प्रत्यागम;
#यदि_अपरिभाषितम् __MINGW32__
      सङ्केत(SIGHUP,  SIG_DFL);
#यदि_अन्तः
      सङ्केत(SIGINT,  SIG_DFL);
      सङ्केत(SIGTERM, SIG_DFL);
}

/*
 * This function puts an event on the end of the pre-simulation event queue.
 */
स्थिर शून्य कालसारणी_init_event(संरचना event_s*cur)
{
      यदि (कालसारणी_init_list == 0) {
            cur->अग्र_2 = cur;
      } अन्यथा {
            cur->अग्र_2 = कालसारणी_init_list->अग्र_2;
            कालसारणी_init_list->अग्र_2 = cur;
      }
      कालसारणी_init_list = cur;
}

/*
 * This function puts an event on the end of the post-simulation event queue.
 */
स्थिर शून्य कालसारणी_final_event(संरचना event_s*cur)
{
      यदि (कालसारणी_final_list == 0) {
            cur->अग्र_2 = cur;
      } अन्यथा {
            cur->अग्र_2 = कालसारणी_final_list->अग्र_2;
            कालसारणी_final_list->अग्र_2 = cur;
      }
      कालसारणी_final_list = cur;
}

/*
 * This function does all the hard work of putting an event into the
 * event queue. The event delay is taken from the event structure
 * itself, and the structure is placed in the right place in the
 * queue.
 */
प्रकारनिर्देश गणना event_queue_e { SEQ_START, SEQ_सक्रिय, SEQ_INACTIVE, SEQ_NBASSIGN,
			     SEQ_RWSYNC, SEQ_ROSYNC, DEL_THREAD } event_queue_t;

स्थिर शून्य कालसारणी_event(संरचना event_s*cur, vvp_time64_t delay,
			    event_queue_t select_queue)
{
      cur->अग्र_2 = cur;
      संरचना event_time_s*ctim = sched_list;

      यदि (sched_list == 0) {
	      /* Is the event_time list completely empty? Create the
		 first event_time object. */
	    ctim = नूतन संरचना event_time_s;
	    ctim->delay = delay;
	    ctim->अग्र_2  = 0;
	    sched_list = ctim;

      } अन्यथा यदि (sched_list->delay > delay) {

	      /* Am I looking for an event before the first event_time?
		 If so, create a new event_time to go in front. */
	    संरचना event_time_s*tmp = नूतन संरचना event_time_s;
	    tmp->delay = delay;
	    tmp->अग्र_2 = ctim;
	    ctim->delay -= delay;
	    ctim = tmp;
	    sched_list = ctim;

      } अन्यथा {
	    संरचना event_time_s*prev = 0;

	    यावत् (ctim->अग्र_2 && (ctim->delay < delay)) {
		  delay -= ctim->delay;
		  prev = ctim;
		  ctim = ctim->अग्र_2;
	    }

	    यदि (ctim->delay > delay) {
		  संरचना event_time_s*tmp = नूतन संरचना event_time_s;
		  tmp->delay = delay;
		  tmp->अग्र_2  = prev->अग्र_2;
		  prev->अग्र_2 = tmp;

		  tmp->अग्र_2->delay -= delay;
		  ctim = tmp;

	    } अन्यथा यदि (ctim->delay == delay) {

	    } अन्यथा {
		  प्रतिज्ञा(ctim->अग्र_2 == 0);
		  संरचना event_time_s*tmp = नूतन संरचना event_time_s;
		  tmp->delay = delay - ctim->delay;
		  tmp->अग्र_2 = 0;
		  ctim->अग्र_2 = tmp;

		  ctim = tmp;
	    }
      }

	/* By this point, ctim is the event_time structure that is to
	   receive the event at hand. Put the event in to the
	   appropriate list for the kind of assign we have at hand. */

      संरचना event_s** q = 0;

      विकल्प (select_queue) {
	  पक्ष SEQ_START:
	    q = &ctim->start;
	    भङ्ग;

	  पक्ष SEQ_सक्रिय:
	    q = &ctim->सक्रिय;
	    भङ्ग;

	  पक्ष SEQ_INACTIVE:
	    प्रतिज्ञा(delay == 0);
	    q = &ctim->inactive;
	    भङ्ग;

	  पक्ष SEQ_NBASSIGN:
	    q = &ctim->nbassign;
	    भङ्ग;

	  पक्ष SEQ_RWSYNC:
	    q = &ctim->rwsync;
	    भङ्ग;

	  पक्ष SEQ_ROSYNC:
	    q = &ctim->rosync;
	    भङ्ग;

	  पक्ष DEL_THREAD:
	    q = &ctim->del_thr;
	    भङ्ग;
      }

      यदि (q) {
	    यदि (*q) {
		  /* Put the cur event on the end of the queue. */
		  cur->अग्र_2 = (*q)->अग्र_2;
		  (*q)->अग्र_2 = cur;
	    }
	    *q = cur;
      }
}

स्थिर शून्य कालसारणी_event_प्रक्षिप(संरचना event_s*cur)
{
      यदि ((sched_list == 0) || (sched_list->delay > 0)) {
	    कालसारणी_event(cur, 0, SEQ_सक्रिय);
	    प्रत्यागम;
      }

      संरचना event_time_s*ctim = sched_list;

      यदि (ctim->सक्रिय == 0) {
	    cur->अग्र_2 = cur;
	    ctim->सक्रिय = cur;
	    प्रत्यागम;
      }

      cur->अग्र_2 = ctim->सक्रिय->अग्र_2;
      ctim->सक्रिय->अग्र_2 = cur;
}

शून्य कालसारणी_vthread(vthread_t thr, vvp_time64_t delay, तार्किक प्रक्षिप_flag)
{
      संरचना vthread_event_s*cur = नूतन vthread_event_s;

      cur->thr = thr;
      vthread_mark_scheduled(thr);

      यदि (प्रक्षिप_flag && (delay == 0)) {
	      /* Special case: If the delay is 0, the push_flag means
		 I can push this event in front of everything. This is
		 used by the %fork statement, for example, to perform
		 task calls. */
	    कालसारणी_event_प्रक्षिप(cur);

      } अन्यथा {
	    कालसारणी_event(cur, delay, SEQ_सक्रिय);
      }
}

शून्य कालसारणी_t0_trigger(vvp_net_ptr_t ptr)
{
      vvp_vector4_t bit (1, BIT4_X);
      संरचना नियोजय_vector4_event_s*cur = नूतन संरचना नियोजय_vector4_event_s(bit);
      cur->ptr = ptr;
      कालसारणी_event(cur, 0, SEQ_INACTIVE);
}

शून्य कालसारणी_inactive(vthread_t thr)
{
      संरचना vthread_event_s*cur = नूतन vthread_event_s;

      cur->thr = thr;
      vthread_mark_scheduled(thr);
      कालसारणी_event(cur, 0, SEQ_INACTIVE);
}

शून्य कालसारणी_init_vthread(vthread_t thr)
{
      संरचना vthread_event_s*cur = नूतन vthread_event_s;

      cur->thr = thr;
      vthread_mark_scheduled(thr);

      कालसारणी_init_event(cur);
}

शून्य कालसारणी_final_vthread(vthread_t thr)
{
      संरचना vthread_event_s*cur = नूतन vthread_event_s;

      cur->thr = thr;
      vthread_mark_final(thr);
      vthread_mark_scheduled(thr);

      कालसारणी_final_event(cur);
}

शून्य कालसारणी_नियोजय_सदिशम्(vvp_net_ptr_t ptr,
			    अचिह्नित आधारः, अचिह्नित vwid,
			    ध्रुव vvp_vector4_t&bit,
			    vvp_time64_t delay)
{
      संरचना नियोजय_vector4_event_s*cur = नूतन संरचना नियोजय_vector4_event_s(bit);
      cur->ptr = ptr;
      cur->आधारः = आधारः;
      cur->vwid = vwid;
      कालसारणी_event(cur, delay, SEQ_NBASSIGN);
}

शून्य कालसारणी_force_सदिशम्(vvp_net_t*net,
			    अचिह्नित आधारः, अचिह्नित vwid,
			    ध्रुव vvp_vector4_t&bit,
			    vvp_time64_t delay)
{
      संरचना force_vector4_event_s*cur = नूतन संरचना force_vector4_event_s(bit);
      cur->net = net;
      cur->आधारः = आधारः;
      cur->vwid = vwid;
      कालसारणी_event(cur, delay, SEQ_NBASSIGN);
}

शून्य कालसारणी_propagate_सदिशम्(vvp_net_t*net,
			       vvp_time64_t delay,
			       ध्रुव vvp_vector4_t&src)
{
      संरचना propagate_vector4_event_s*cur
	    = नूतन संरचना propagate_vector4_event_s(src);
      cur->net = net;
      कालसारणी_event(cur, delay, SEQ_NBASSIGN);
}

// FIXME: This needs to create a non-blocking event, but only one per time slot.
//        Is schedule_event_ or execution actually filtering since the net is
//        already X because this is not triggering?
शून्य कालसारणी_propagate_event(vvp_net_t*net,
                              vvp_time64_t delay)
{
      vvp_vector4_t tmp (1, BIT4_X);
      संरचना propagate_vector4_event_s*cur
	    = नूतन संरचना propagate_vector4_event_s(tmp);
      cur->net = net;
      कालसारणी_event(cur, delay, SEQ_NBASSIGN);
}

शून्य कालसारणी_नियोजय_array_word(vvp_array_t mem,
				अचिह्नित word_addr,
				अचिह्नित off,
				ध्रुव vvp_vector4_t&val,
				vvp_time64_t delay)
{
      संरचना नियोजय_array_word_s*cur = नूतन संरचना नियोजय_array_word_s;
      cur->mem = mem;
      cur->adr = word_addr;
      cur->off = off;
      cur->val = val;
      कालसारणी_event(cur, delay, SEQ_NBASSIGN);
}

शून्य कालसारणी_नियोजय_array_word(vvp_array_t mem,
				अचिह्नित word_addr,
				द्विगुण val,
				vvp_time64_t delay)
{
      संरचना नियोजय_array_r_word_s*cur = नूतन संरचना नियोजय_array_r_word_s;
      cur->mem = mem;
      cur->adr = word_addr;
      cur->val = val;
      कालसारणी_event(cur, delay, SEQ_NBASSIGN);
}

शून्य कालसारणी_समुच्चयः_सदिशम्(vvp_net_ptr_t ptr, ध्रुव vvp_vector4_t&bit)
{
      संरचना नियोजय_vector4_event_s*cur = नूतन संरचना नियोजय_vector4_event_s(bit);
      cur->ptr = ptr;
      cur->आधारः = 0;
      cur->vwid = 0;
      कालसारणी_event(cur, 0, SEQ_सक्रिय);
}

शून्य कालसारणी_समुच्चयः_सदिशम्(vvp_net_ptr_t ptr, ध्रुव vvp_vector8_t&bit)
{
      संरचना नियोजय_vector8_event_s*cur = नूतन संरचना नियोजय_vector8_event_s;
      cur->ptr = ptr;
      cur->val = bit;
      कालसारणी_event(cur, 0, SEQ_सक्रिय);
}

शून्य कालसारणी_समुच्चयः_सदिशम्(vvp_net_ptr_t ptr, द्विगुण bit)
{
      संरचना नियोजय_real_event_s*cur = नूतन संरचना नियोजय_real_event_s;
      cur->ptr = ptr;
      cur->val = bit;
      कालसारणी_event(cur, 0, SEQ_सक्रिय);
}

शून्य कालसारणी_init_सदिशम्(vvp_net_ptr_t ptr, ध्रुव vvp_vector4_t&bit)
{
      संरचना नियोजय_vector4_event_s*cur = नूतन संरचना नियोजय_vector4_event_s(bit);
      cur->ptr = ptr;
      cur->आधारः = 0;
      cur->vwid = 0;
      कालसारणी_init_event(cur);
}

शून्य कालसारणी_init_सदिशम्(vvp_net_ptr_t ptr, ध्रुव vvp_vector8_t&bit)
{
      संरचना नियोजय_vector8_event_s*cur = नूतन संरचना नियोजय_vector8_event_s;
      cur->ptr = ptr;
      cur->val = bit;
      कालसारणी_init_event(cur);
}

शून्य कालसारणी_init_सदिशम्(vvp_net_ptr_t ptr, द्विगुण bit)
{
      संरचना नियोजय_real_event_s*cur = नूतन संरचना नियोजय_real_event_s;
      cur->ptr = ptr;
      cur->val = bit;
      कालसारणी_init_event(cur);
}

शून्य कालसारणी_init_propagate(vvp_net_t*net, vvp_vector4_t bit)
{
      संरचना propagate_vector4_event_s*cur = नूतन संरचना propagate_vector4_event_s(bit);
      cur->net = net;
      कालसारणी_init_event(cur);
}

शून्य कालसारणी_init_propagate(vvp_net_t*net, द्विगुण bit)
{
      संरचना propagate_real_event_s*cur = नूतन संरचना propagate_real_event_s;
      cur->net = net;
      cur->val = bit;
      कालसारणी_init_event(cur);
}

शून्य कालसारणी_del_thr(vthread_t thr)
{
      संरचना del_thr_event_s*cur = नूतन del_thr_event_s;

      cur->thr = thr;

      कालसारणी_event(cur, 0, DEL_THREAD);
}

शून्य कालसारणी_generic(vvp_gen_event_t obj, vvp_time64_t delay,
		      तार्किक sync_flag, तार्किक ro_flag, तार्किक delete_when_समाप्तम्)
{
      संरचना generic_event_s*cur = नूतन generic_event_s;

      cur->obj = obj;
      cur->delete_obj_when_समाप्तम् = delete_when_समाप्तम्;
      कालसारणी_event(cur, delay,
		      sync_flag? (ro_flag?SEQ_ROSYNC:SEQ_RWSYNC) : SEQ_सक्रिय);

      यदि (sync_flag)
	    vthread_delay_delete();
}

स्थिर तार्किक sim_started;

शून्य कालसारणी_functor(vvp_gen_event_t obj)
{
      संरचना generic_event_s*cur = नूतन generic_event_s;

      cur->obj = obj;
      cur->delete_obj_when_समाप्तम् = असत्य;
      यदि (!sim_started) {
            कालसारणी_init_event(cur);
      } अन्यथा {
            कालसारणी_event(cur, 0, SEQ_सक्रिय);
      }
}

शून्य कालसारणी_at_start_सम्बन्धि_simtime(vvp_gen_event_t obj, vvp_time64_t delay)
{
      संरचना generic_event_s*cur = नूतन generic_event_s;

      cur->obj = obj;
      cur->delete_obj_when_समाप्तम् = असत्य;
      कालसारणी_event(cur, delay, SEQ_START);
}

/*
 * In the vvp runtime of Icarus Verilog, the SEQ_RWSYNC time step is
 * after all of the non-blocking assignments, so is effectively the
 * same as the ReadWriteSync time.
 */
शून्य कालसारणी_at_अन्तः_सम्बन्धि_simtime(vvp_gen_event_t obj, vvp_time64_t delay)
{
      संरचना generic_event_s*cur = नूतन generic_event_s;

      cur->obj = obj;
      cur->delete_obj_when_समाप्तम् = असत्य;
      कालसारणी_event(cur, delay, SEQ_RWSYNC);
}

स्थिर vvp_time64_t कालसारणी_time;
vvp_time64_t कालसारणी_simtime(शून्य)
{ प्रत्यागम कालसारणी_time; }

स्थिर तार्किक sim_at_rosync = असत्य;
तार्किक कालसारणी_at_rosync(शून्य)
{ प्रत्यागम sim_at_rosync; }

/*
 * The scheduler uses this function to drain the rosync events of the
 * current time. The ctim object is still in the event queue, because
 * it is legal for a rosync callback to create other rosync
 * callbacks. It is *not* legal for them to create any other kinds of
 * events, and that is why the rosync is treated specially.
 *
 * Once all the rosync callbacks are done we can safely delete any
 * threads that finished during this time step.
 */
स्थिर शून्य सततम्_rosync(संरचना event_time_s*ctim)
{
      sim_at_rosync = सत्य;
      यावत् (ctim->rosync) {
	    संरचना event_s*cur = ctim->rosync->अग्र_2;
	    यदि (cur->अग्र_2 == cur) {
		  ctim->rosync = 0;
	    } अन्यथा {
		  ctim->rosync->अग्र_2 = cur->अग्र_2;
	    }

	    cur->सततम्_सततम्();
	    विलोपय cur;
      }
      sim_at_rosync = असत्य;

      यावत् (ctim->del_thr) {
	    संरचना event_s*cur = ctim->del_thr->अग्र_2;
	    यदि (cur->अग्र_2 == cur) {
		  ctim->del_thr = 0;
	    } अन्यथा {
		  ctim->del_thr->अग्र_2 = cur->अग्र_2;
	    }

	    cur->सततम्_सततम्();
	    विलोपय cur;
      }

      यदि (ctim->सक्रिय || ctim->inactive || ctim->nbassign || ctim->rwsync) {
	    cerr << "SCHEDULER ERROR: read-only sync events "
		 << "created RW events!" << पङ्क्त्यन्तः;
      }
}

शून्य कालसारणी_simulate(शून्य)
{
      तार्किक सततम्_finals;
      sim_started = असत्य;

      कालसारणी_time = 0;

      यदि (verbose_flag) {
	    vpi_mcd_printf(1, " ...execute EndOfCompile callbacks\n");
      }

      // Execute end of compile callbacks
      vpi_अन्तः_सम्बन्धि_Compile();

      यदि (verbose_flag) {
	    vpi_mcd_printf(1, " ...propagate initialization events\n");
      }

	// Execute initialization events.
      यावत् (कालसारणी_init_list) {
	    संरचना event_s*cur = कालसारणी_init_list->अग्र_2;
	    यदि (cur->अग्र_2 == cur) {
		  कालसारणी_init_list = 0;
	    } अन्यथा {
		  कालसारणी_init_list->अग्र_2 = cur->अग्र_2;
	    }
	    cur->सततम्_सततम्();
	    विलोपय cur;
      }

      sim_started = सत्य;

      यदि (verbose_flag) {
	    vpi_mcd_printf(1, " ...execute StartOfSim callbacks\n");
      }

      // Execute start of simulation callbacks
      vpi_Start_सम्बन्धि_Sim();

      सङ्केताः_capture();

      यदि (verbose_flag) {
	    vpi_mcd_printf(1, " ...run scheduler\n");
      }

      // If there were no compiletf, etc. errors then we are going to
      // process events and when done run the final blocks.
      सततम्_finals = कालसारणी_runnable;

      यदि (कालसारणी_runnable) यावत् (sched_list) {

	    यदि (कालसारणी_stopped_flag) {
		  कालसारणी_stopped_flag = असत्य;
		  stop_handler(0);
		  // You can finish from the debugger without a time change.
		  यदि (!कालसारणी_runnable) भङ्ग;
		  अनुवर्त;
	    }

	      /* ctim is the current time step. */
	    संरचना event_time_s* ctim = sched_list;

	      /* If the time is advancing, then first run the
		 postponed sync events. Run them all. */
	    यदि (ctim->delay > 0) {

		  यदि (!कालसारणी_runnable) भङ्ग;
		  कालसारणी_time += ctim->delay;
		    /* When the design is being traced (we are emitting
		     * file/line information) also print any time changes. */
		  यदि (show_file_पङ्क्तिः) {
			cerr << "Advancing to simulation time: "
			     << कालसारणी_time << पङ्क्त्यन्तः;
		  }
		  ctim->delay = 0;

		  vpi_अग्र_Sim_Time();
		    // Process the cbAtStartOfSimTime callbacks.
		  यावत् (ctim->start) {
			संरचना event_s*cur = ctim->start->अग्र_2;
			यदि (cur->अग्र_2 == cur) {
			      ctim->start = 0;
			} अन्यथा {
			      ctim->start->अग्र_2 = cur->अग्र_2;
			}
			cur->सततम्_सततम्();
			विलोपय (cur);
		  }
	    }


	      /* If there are no more active events, advance the event
		 queues. If there are not events at all, then release
		 the event_time object. */
	    यदि (ctim->सक्रिय == 0) {
		  ctim->सक्रिय = ctim->inactive;
		  ctim->inactive = 0;

		  यदि (ctim->सक्रिय == 0) {
			ctim->सक्रिय = ctim->nbassign;
			ctim->nbassign = 0;

			यदि (ctim->सक्रिय == 0) {
			      ctim->सक्रिय = ctim->rwsync;
			      ctim->rwsync = 0;

				/* If out of rw events, then run the rosync
				   events and delete this time step. This also
				   deletes threads as needed. */
			      यदि (ctim->सक्रिय == 0) {
				    सततम्_rosync(ctim);
				    sched_list = ctim->अग्र_2;
				    विलोपय ctim;
				    अनुवर्त;
			      }
			}
		  }
	    }

	      /* Pull the first item off the list. If this is the last
		 cell in the list, then clear the list. Execute that
		 event type, and delete it. */
	    संरचना event_s*cur = ctim->सक्रिय->अग्र_2;
	    यदि (cur->अग्र_2 == cur) {
		  ctim->सक्रिय = 0;
	    } अन्यथा {
		  ctim->सक्रिय->अग्र_2 = cur->अग्र_2;
	    }

	    यदि (कालसारणी_single_चरणः_flag) {
		  cur->single_चरणः_display();
		  कालसारणी_stopped_flag = सत्य;
		  कालसारणी_single_चरणः_flag = असत्य;
	    }

	    cur->सततम्_सततम्();

	    विलोपय (cur);
      }

	// Execute final events.
      कालसारणी_runnable = सततम्_finals;
      यावत् (कालसारणी_runnable && कालसारणी_final_list) {
	    संरचना event_s*cur = कालसारणी_final_list->अग्र_2;
	    यदि (cur->अग्र_2 == cur) {
		  कालसारणी_final_list = 0;
	    } अन्यथा {
		  कालसारणी_final_list->अग्र_2 = cur->अग्र_2;
	    }
	    cur->सततम्_सततम्();
	    विलोपय cur;
      }

      सङ्केताः_revert();

      यदि (verbose_flag) {
	    vpi_mcd_printf(1, " ...execute Postsim callbacks\n");
      }

      // Execute post-simulation callbacks
      vpiPostsim();
#यदि_परिभाषितम् CHECK_WITH_VALGRIND
      कालसारणी_delete();
#यदि_अन्तः
}

#यदि_परिभाषितम् CHECK_WITH_VALGRIND
शून्य कालसारणी_delete(शून्य)
{
      vthread_event_heap.delete_pool();
      assign4_heap.delete_pool();
      assign8_heap.delete_pool();
      assignr_heap.delete_pool();
      array_w_heap.delete_pool();
      array_r_w_heap.delete_pool();
      generic_event_heap.delete_pool();
      event_time_heap.delete_pool();
}
#यदि_अन्तः
