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
#अगर_घोषित CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
# include  "ivl_alloc.h"
#पूर्ण_अगर

using namespace std;

अचिन्हित दीर्घ गिनती_assign_events = 0;
अचिन्हित दीर्घ गिनती_gen_events = 0;
अचिन्हित दीर्घ गिनती_thread_events = 0;
  // Count the time events (A time cell created)
अचिन्हित दीर्घ गिनती_time_events = 0;



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
काष्ठा event_s {
      काष्ठा event_s*अग्र_2;
      भव ~event_s() { }
      भव व्योम run_run(व्योम) =0;

	// Write something about the event to stderr
      भव व्योम single_step_display(व्योम);

	// Fallback new/delete
      अटल व्योम*चालक नया (माप_प्रकार size) { वापस ::नया अक्षर[size]; }
      अटल व्योम चालक मिटाओ(व्योम*ptr)  { ::मिटाओ[]( static_cast<अक्षर*>(ptr) ); }
};

व्योम event_s::single_step_display(व्योम)
{
      std::अ_त्रुटि << "event_s: Step into event " << typeid(*यह).name() << std::प_पूर्ण;
}

काष्ठा event_time_s {
      event_time_s() {
	    गिनती_time_events += 1;
	    delay = 0;
	    start = 0;
	    active = 0;
	    inactive = 0;
	    nbassign = 0;
	    rwsync = 0;
	    rosync = 0;
	    del_thr = 0;
	    अग्र_2 = शून्य;
      }
      vvp_time64_t delay;

      काष्ठा event_s*start;
      काष्ठा event_s*active;
      काष्ठा event_s*inactive;
      काष्ठा event_s*nbassign;
      काष्ठा event_s*rwsync;
      काष्ठा event_s*rosync;
      काष्ठा event_s*del_thr;

      काष्ठा event_time_s*अग्र_2;

      अटल व्योम* चालक नया (माप_प्रकार);
      अटल व्योम चालक मिटाओ(व्योम*obj, माप_प्रकार s);
};

vvp_gen_event_s::~vvp_gen_event_s()
{
}

व्योम vvp_gen_event_s::single_step_display(व्योम)
{
      अ_त्रुटि << "vvp_gen_event_s: Step into event " << typeid(*यह).name() << प_पूर्ण;
}

/*
 * Derived event types
 */
काष्ठा vthread_event_s : खुला event_s {
      vthread_t thr;
      व्योम run_run(व्योम) override;
      व्योम single_step_display(व्योम) override;

      अटल व्योम* चालक नया(माप_प्रकार);
      अटल व्योम चालक मिटाओ(व्योम*);
};

व्योम vthread_event_s::run_run(व्योम)
{
      गिनती_thread_events += 1;
      vthread_run(thr);
}

व्योम vthread_event_s::single_step_display(व्योम)
{
      __vpiScope*scope = vthread_scope(thr);
      अ_त्रुटि << "vthread_event: Resume thread"
	   << " scope=" << scope->vpi_get_str(vpiFullName)
	   << प_पूर्ण;
}

अटल स्थिर माप_प्रकार VTHR_CHUNK_गिनती = 8192 / माप(काष्ठा vthread_event_s);
अटल slab_t<माप(vthread_event_s),VTHR_CHUNK_गिनती> vthread_event_heap;

अंतरभूत व्योम* vthread_event_s::चालक नया(माप_प्रकार size)
{
      निश्चित(size == माप(vthread_event_s));
      वापस vthread_event_heap.alloc_slab();
}

व्योम vthread_event_s::चालक मिटाओ(व्योम*dptr)
{
      vthread_event_heap.free_slab(dptr);
}

काष्ठा del_thr_event_s : खुला event_s {
      vthread_t thr;
      व्योम run_run(व्योम) override;
      व्योम single_step_display(व्योम) override;
};

व्योम del_thr_event_s::run_run(व्योम)
{
      vthread_delete(thr);
}

व्योम del_thr_event_s::single_step_display(व्योम)
{
      __vpiScope*scope = vthread_scope(thr);
      अ_त्रुटि << "del_thr_event: Reap completed thread"
	   << " scope=" << scope->vpi_get_str(vpiFullName) << प_पूर्ण;
}

काष्ठा assign_vector4_event_s  : खुला event_s {
	/* The default constructor. */
      explicit assign_vector4_event_s(स्थिर vvp_vector4_t&that) : val(that) {
	    base = 0;
	    vwid = 0;
	    अग्र_2 = शून्य;
      }

	/* Where to do the assign. */
      vvp_net_ptr_t ptr;
	/* Value to assign. */
      vvp_vector4_t val;
	/* Offset of the part into the destination. */
      अचिन्हित base;
	/* Width of the destination vector. */
      अचिन्हित vwid;
      व्योम run_run(व्योम) override;
      व्योम single_step_display(व्योम) override;

      अटल व्योम* चालक नया(माप_प्रकार);
      अटल व्योम चालक मिटाओ(व्योम*);
};

व्योम assign_vector4_event_s::run_run(व्योम)
{
      गिनती_assign_events += 1;
      अगर (vwid > 0)
	    vvp_send_vec4_pv(ptr, val, base, vwid, 0);
      अन्यथा
	    vvp_send_vec4(ptr, val, 0);
}

व्योम assign_vector4_event_s::single_step_display(व्योम)
{
      अ_त्रुटि << "assign_vector4_event: Propagate val=" << val
	   << ", vwid=" << vwid << ", base=" << base << प_पूर्ण;
}

अटल स्थिर माप_प्रकार ASSIGN4_CHUNK_गिनती = 524288 / माप(काष्ठा assign_vector4_event_s);
अटल slab_t<माप(assign_vector4_event_s),ASSIGN4_CHUNK_गिनती> assign4_heap;

अंतरभूत व्योम* assign_vector4_event_s::चालक नया(माप_प्रकार size)
{
      निश्चित(size == माप(assign_vector4_event_s));
      वापस assign4_heap.alloc_slab();
}

व्योम assign_vector4_event_s::चालक मिटाओ(व्योम*dptr)
{
      assign4_heap.free_slab(dptr);
}

अचिन्हित दीर्घ गिनती_assign4_pool(व्योम) { वापस assign4_heap.pool; }

काष्ठा assign_vector8_event_s  : खुला event_s {
      vvp_net_ptr_t ptr;
      vvp_vector8_t val;
      व्योम run_run(व्योम) override;
      व्योम single_step_display(व्योम) override;

      अटल व्योम* चालक नया(माप_प्रकार);
      अटल व्योम चालक मिटाओ(व्योम*);
};

व्योम assign_vector8_event_s::run_run(व्योम)
{
      गिनती_assign_events += 1;
      vvp_send_vec8(ptr, val);
}

व्योम assign_vector8_event_s::single_step_display(व्योम)
{
      अ_त्रुटि << "assign_vector8_event: Propagate val=" << val << प_पूर्ण;
}

अटल स्थिर माप_प्रकार ASSIGN8_CHUNK_गिनती = 8192 / माप(काष्ठा assign_vector8_event_s);
अटल slab_t<माप(assign_vector8_event_s),ASSIGN8_CHUNK_गिनती> assign8_heap;

अंतरभूत व्योम* assign_vector8_event_s::चालक नया(माप_प्रकार size)
{
      निश्चित(size == माप(assign_vector8_event_s));
      वापस assign8_heap.alloc_slab();
}

व्योम assign_vector8_event_s::चालक मिटाओ(व्योम*dptr)
{
      assign8_heap.free_slab(dptr);
}

अचिन्हित दीर्घ गिनती_assign8_pool() { वापस assign8_heap.pool; }

काष्ठा assign_real_event_s  : खुला event_s {
      vvp_net_ptr_t ptr;
      द्विगुन val;
      व्योम run_run(व्योम) override;
      व्योम single_step_display(व्योम) override;

      अटल व्योम* चालक नया(माप_प्रकार);
      अटल व्योम चालक मिटाओ(व्योम*);
};

व्योम assign_real_event_s::run_run(व्योम)
{
      गिनती_assign_events += 1;
      vvp_send_real(ptr, val, 0);
}

व्योम assign_real_event_s::single_step_display(व्योम)
{
      अ_त्रुटि << "assign_real_event: Propagate val=" << val << प_पूर्ण;
}

अटल स्थिर माप_प्रकार ASSIGNR_CHUNK_गिनती = 8192 / माप(काष्ठा assign_real_event_s);
अटल slab_t<माप(assign_real_event_s),ASSIGNR_CHUNK_गिनती> assignr_heap;

अंतरभूत व्योम* assign_real_event_s::चालक नया (माप_प्रकार size)
{
      निश्चित(size == माप(assign_real_event_s));
      वापस assignr_heap.alloc_slab();
}

व्योम assign_real_event_s::चालक मिटाओ(व्योम*dptr)
{
      assignr_heap.free_slab(dptr);
}

अचिन्हित दीर्घ गिनती_assign_real_pool(व्योम) { वापस assignr_heap.pool; }

काष्ठा assign_array_word_s  : खुला event_s {
      vvp_array_t mem;
      अचिन्हित adr;
      vvp_vector4_t val;
      अचिन्हित off;
      व्योम run_run(व्योम) override;

      अटल व्योम* चालक नया(माप_प्रकार);
      अटल व्योम चालक मिटाओ(व्योम*);
};

व्योम assign_array_word_s::run_run(व्योम)
{
      गिनती_assign_events += 1;
      mem->set_word(adr, off, val);
}

अटल स्थिर माप_प्रकार ARRAY_W_CHUNK_गिनती = 8192 / माप(काष्ठा assign_array_word_s);
अटल slab_t<माप(assign_array_word_s),ARRAY_W_CHUNK_गिनती> array_w_heap;

अंतरभूत व्योम* assign_array_word_s::चालक नया (माप_प्रकार size)
{
      निश्चित(size == माप(assign_array_word_s));
      वापस array_w_heap.alloc_slab();
}

व्योम assign_array_word_s::चालक मिटाओ(व्योम*ptr)
{
      array_w_heap.free_slab(ptr);
}

अचिन्हित दीर्घ गिनती_assign_aword_pool(व्योम) { वापस array_w_heap.pool; }

काष्ठा force_vector4_event_s  : खुला event_s {
	/* The default constructor. */
      explicit force_vector4_event_s(स्थिर vvp_vector4_t&that): val(that) {
	    net = शून्य;
	    base = 0;
	    vwid = 0;
	    अग्र_2 = शून्य;
      }
	/* Where to do the force. */
      vvp_net_t*net;
	/* Value to force. */
      vvp_vector4_t val;
	/* Offset of the part into the destination. */
      अचिन्हित base;
	/* Width of the destination vector. */
      अचिन्हित vwid;

      व्योम run_run(व्योम) override;
      व्योम single_step_display(व्योम) override;

      अटल व्योम* चालक नया(माप_प्रकार);
      अटल व्योम चालक मिटाओ(व्योम*);
};

व्योम force_vector4_event_s::run_run(व्योम)
{
      गिनती_assign_events += 1;

      अचिन्हित wid = val.size();
      अगर ((base + wid) > vwid)
	    wid = vwid - base;

	// Make a mask of which bits are to be forced, 0 for unforced
	// bits and 1 for forced bits.
      vvp_vector2_t mask (vvp_vector2_t::FILL0, vwid);
      क्रम (अचिन्हित idx = 0 ; idx < wid ; idx += 1)
	    mask.set_bit(base+idx, 1);

      vvp_vector4_t tmp (vwid, BIT4_Z);

	// vvp_net_t::force_vec4 propagates all the bits of the
	// forced vector value, regardless of the mask. This
	// ensures the unforced bits retain their current value.
      vvp_signal_मूल्य*sig = dynamic_cast<vvp_signal_मूल्य*>(net->fil);
      निश्चित(sig);
      sig->vec4_मूल्य(tmp);

      tmp.set_vec(base, val);
      net->force_vec4(tmp, mask);
}

व्योम force_vector4_event_s::single_step_display(व्योम)
{
      अ_त्रुटि << "force_vector4_event: Force val=" << val
	   << ", vwid=" << vwid << ", base=" << base << प_पूर्ण;
}

अटल स्थिर माप_प्रकार FORCE4_CHUNK_गिनती = 8192 / माप(काष्ठा force_vector4_event_s);
अटल slab_t<माप(force_vector4_event_s),FORCE4_CHUNK_गिनती> force4_heap;

अंतरभूत व्योम* force_vector4_event_s::चालक नया(माप_प्रकार size)
{
      निश्चित(size == माप(force_vector4_event_s));
      वापस force4_heap.alloc_slab();
}

व्योम force_vector4_event_s::चालक मिटाओ(व्योम*dptr)
{
      force4_heap.free_slab(dptr);
}

अचिन्हित दीर्घ गिनती_force4_pool(व्योम) { वापस force4_heap.pool; }

/*
 * This class supports the propagation of vec4 outputs from a
 * vvp_net_t object.
 */
काष्ठा propagate_vector4_event_s : खुला event_s {
	/* The default constructor. */
      explicit propagate_vector4_event_s(स्थिर vvp_vector4_t&that) : val(that) {
	    net = शून्य;
	    अग्र_2 = शून्य;
      }
	/* A constructor that makes the val directly. */
      propagate_vector4_event_s(स्थिर vvp_vector4_t&that, अचिन्हित adr, अचिन्हित wid)
      : val(that,adr,wid) {
	    net = शून्य;
	    अग्र_2 = शून्य;
      }

	/* Propagate the output of this net. */
      vvp_net_t*net;
	/* value to propagate */
      vvp_vector4_t val;
	/* Action */
      व्योम run_run(व्योम) override;
      व्योम single_step_display(व्योम) override;
};

व्योम propagate_vector4_event_s::run_run(व्योम)
{
      net->send_vec4(val, 0);
}

व्योम propagate_vector4_event_s::single_step_display(व्योम)
{
      अ_त्रुटि << "propagate_vector4_event: Propagate val=" << val << प_पूर्ण;
}

/*
 * This class supports the propagation of real outputs from a
 * vvp_net_t object.
 */
काष्ठा propagate_real_event_s : खुला event_s {
	/* Propagate the output of this net. */
      vvp_net_t*net;
	/* value to propagate */
      द्विगुन val;
	/* Action */
      व्योम run_run(व्योम) override;
      व्योम single_step_display(व्योम) override;
};

व्योम propagate_real_event_s::run_run(व्योम)
{
      net->send_real(val, 0);
}

व्योम propagate_real_event_s::single_step_display(व्योम)
{
      अ_त्रुटि << "propagate_real_event: Propagate val=" << val << प_पूर्ण;
}

काष्ठा assign_array_r_word_s  : खुला event_s {
      vvp_array_t mem;
      अचिन्हित adr;
      द्विगुन val;
      व्योम run_run(व्योम) override;

      अटल व्योम* चालक नया(माप_प्रकार);
      अटल व्योम चालक मिटाओ(व्योम*);
};

व्योम assign_array_r_word_s::run_run(व्योम)
{
      गिनती_assign_events += 1;
      mem->set_word(adr, val);
}
अटल स्थिर माप_प्रकार ARRAY_R_W_CHUNK_गिनती = 8192 / माप(काष्ठा assign_array_r_word_s);
अटल slab_t<माप(assign_array_r_word_s),ARRAY_R_W_CHUNK_गिनती> array_r_w_heap;

अंतरभूत व्योम* assign_array_r_word_s::चालक नया(माप_प्रकार size)
{
      निश्चित(size == माप(assign_array_r_word_s));
      वापस array_r_w_heap.alloc_slab();
}

व्योम assign_array_r_word_s::चालक मिटाओ(व्योम*ptr)
{
      array_r_w_heap.free_slab(ptr);
}

अचिन्हित दीर्घ गिनती_assign_arword_pool(व्योम) { वापस array_r_w_heap.pool; }

काष्ठा generic_event_s : खुला event_s {
      vvp_gen_event_t obj;
      bool delete_obj_when_done;
      व्योम run_run(व्योम) override;
      व्योम single_step_display(व्योम) override;

      अटल व्योम* चालक नया(माप_प्रकार);
      अटल व्योम चालक मिटाओ(व्योम*);
};

व्योम generic_event_s::run_run(व्योम)
{
      गिनती_gen_events += 1;
      अगर (obj) {
	    obj->run_run();
	    अगर (delete_obj_when_done)
		  मिटाओ obj;
      }
}

व्योम generic_event_s::single_step_display(व्योम)
{
      obj->single_step_display();
}

अटल स्थिर माप_प्रकार GENERIC_CHUNK_गिनती = 131072 / माप(काष्ठा generic_event_s);
अटल slab_t<माप(generic_event_s),GENERIC_CHUNK_गिनती> generic_event_heap;

अंतरभूत व्योम* generic_event_s::चालक नया(माप_प्रकार size)
{
      निश्चित(size == माप(generic_event_s));
      वापस generic_event_heap.alloc_slab();
}

व्योम generic_event_s::चालक मिटाओ(व्योम*ptr)
{
      generic_event_heap.free_slab(ptr);
}

अचिन्हित दीर्घ गिनती_gen_pool(व्योम) { वापस generic_event_heap.pool; }

/*
** These event_time_s will be required a lot, at high frequency.
** Once allocated, we never free them, but stash them away for next time.
*/


अटल स्थिर माप_प्रकार TIME_CHUNK_गिनती = 8192 / माप(काष्ठा event_time_s);
अटल slab_t<माप(event_time_s),TIME_CHUNK_गिनती> event_time_heap;

अंतरभूत व्योम* event_time_s::चालक नया (माप_प्रकार size)
{
      निश्चित(size == माप(काष्ठा event_time_s));
      व्योम*ptr = event_time_heap.alloc_slab();
      वापस ptr;
}

अंतरभूत व्योम event_time_s::चालक मिटाओ(व्योम*ptr, माप_प्रकार)
{
      event_time_heap.free_slab(ptr);
}

अचिन्हित दीर्घ गिनती_time_pool(व्योम) { वापस event_time_heap.pool; }

/*
 * This is the head of the list of pending events. This includes all
 * the events that have not been executed yet, and reaches into the
 * future.
 */
अटल काष्ठा event_time_s* sched_list = 0;

/*
 * This is a list of initialization events. The setup puts
 * initializations in this list so that they happen before the
 * simulation as a whole starts. This prevents time-0 triggers of
 * certain events.
 */
अटल काष्ठा event_s* schedule_init_list = 0;

/*
 * This is the head of the list of final events.
 */
अटल काष्ठा event_s* schedule_final_list = 0;

/*
 * This flag is true until a VPI task or function finishes the
 * simulation.
 */
अटल bool schedule_runnable = true;
अटल bool schedule_stopped_flag  = false;
अटल bool schedule_single_step_flag = false;
अटल bool no_signals_flag = false;

व्योम schedule_finish(पूर्णांक)
{
      schedule_runnable = false;
}

व्योम schedule_stop(पूर्णांक)
{
      schedule_stopped_flag = true;
}

व्योम schedule_single_step(पूर्णांक)
{
      schedule_single_step_flag = true;
}

bool schedule_finished(व्योम)
{
      वापस !schedule_runnable;
}

bool schedule_stopped(व्योम)
{
      वापस schedule_stopped_flag;
}

बाह्य "C" व्योम vvp_no_signals(व्योम)
{
      no_signals_flag = true;
}

/*
 * These are the signal handling infrastructure. The SIGINT signal
 * leads to an implicit $stop. The SIGHUP and SIGTERM signals lead
 * to an implicit $finish.
 */
बाह्य bool stop_is_finish;

बाह्य "C" व्योम signals_handler(पूर्णांक signum)
{
#अगर_घोषित __MINGW32__
	// Windows implements the original UNIX semantics for signal,
	// so we have to re-establish the signal handler each time a
	// signal is caught.
      संकेत(signum, &signals_handler);
#पूर्ण_अगर
      अगर (signum != संक_विघ्न)
	    stop_is_finish = true;
      schedule_stopped_flag = true;
}

अटल व्योम signals_capture(व्योम)
{
      अगर (no_signals_flag)
	   वापस;
#अगर_अघोषित __MINGW32__
      संकेत(SIGHUP,  &signals_handler);
#पूर्ण_अगर
      संकेत(संक_विघ्न,  &signals_handler);
      संकेत(संक_इति, &signals_handler);
}

अटल व्योम signals_revert(व्योम)
{
      अगर (no_signals_flag)
	   वापस;
#अगर_अघोषित __MINGW32__
      संकेत(SIGHUP,  संक_पूर्व);
#पूर्ण_अगर
      संकेत(संक_विघ्न,  संक_पूर्व);
      संकेत(संक_इति, संक_पूर्व);
}

/*
 * This function puts an event on the end of the pre-simulation event queue.
 */
अटल व्योम schedule_init_event(काष्ठा event_s*cur)
{
      अगर (schedule_init_list == 0) {
            cur->अग्र_2 = cur;
      } अन्यथा {
            cur->अग्र_2 = schedule_init_list->अग्र_2;
            schedule_init_list->अग्र_2 = cur;
      }
      schedule_init_list = cur;
}

/*
 * This function puts an event on the end of the post-simulation event queue.
 */
अटल व्योम schedule_final_event(काष्ठा event_s*cur)
{
      अगर (schedule_final_list == 0) {
            cur->अग्र_2 = cur;
      } अन्यथा {
            cur->अग्र_2 = schedule_final_list->अग्र_2;
            schedule_final_list->अग्र_2 = cur;
      }
      schedule_final_list = cur;
}

/*
 * This function does all the hard work of putting an event into the
 * event queue. The event delay is taken from the event structure
 * itself, and the structure is placed in the right place in the
 * queue.
 */
प्रकार क्रमागत event_queue_e { SEQ_START, SEQ_ACTIVE, SEQ_INACTIVE, SEQ_NBASSIGN,
			     SEQ_RWSYNC, SEQ_ROSYNC, DEL_THREAD } event_queue_t;

अटल व्योम schedule_event_(काष्ठा event_s*cur, vvp_time64_t delay,
			    event_queue_t select_queue)
{
      cur->अग्र_2 = cur;
      काष्ठा event_time_s*ctim = sched_list;

      अगर (sched_list == 0) {
	      /* Is the event_time list completely empty? Create the
		 first event_time object. */
	    ctim = नया काष्ठा event_time_s;
	    ctim->delay = delay;
	    ctim->अग्र_2  = 0;
	    sched_list = ctim;

      } अन्यथा अगर (sched_list->delay > delay) {

	      /* Am I looking for an event before the first event_time?
		 If so, create a new event_time to go in front. */
	    काष्ठा event_time_s*tmp = नया काष्ठा event_time_s;
	    tmp->delay = delay;
	    tmp->अग्र_2 = ctim;
	    ctim->delay -= delay;
	    ctim = tmp;
	    sched_list = ctim;

      } अन्यथा {
	    काष्ठा event_time_s*prev = 0;

	    जबतक (ctim->अग्र_2 && (ctim->delay < delay)) {
		  delay -= ctim->delay;
		  prev = ctim;
		  ctim = ctim->अग्र_2;
	    }

	    अगर (ctim->delay > delay) {
		  काष्ठा event_time_s*tmp = नया काष्ठा event_time_s;
		  tmp->delay = delay;
		  tmp->अग्र_2  = prev->अग्र_2;
		  prev->अग्र_2 = tmp;

		  tmp->अग्र_2->delay -= delay;
		  ctim = tmp;

	    } अन्यथा अगर (ctim->delay == delay) {

	    } अन्यथा {
		  निश्चित(ctim->अग्र_2 == 0);
		  काष्ठा event_time_s*tmp = नया काष्ठा event_time_s;
		  tmp->delay = delay - ctim->delay;
		  tmp->अग्र_2 = 0;
		  ctim->अग्र_2 = tmp;

		  ctim = tmp;
	    }
      }

	/* By this point, ctim is the event_time structure that is to
	   receive the event at hand. Put the event in to the
	   appropriate list for the kind of assign we have at hand. */

      काष्ठा event_s** q = 0;

      चयन (select_queue) {
	  हाल SEQ_START:
	    q = &ctim->start;
	    अवरोध;

	  हाल SEQ_ACTIVE:
	    q = &ctim->active;
	    अवरोध;

	  हाल SEQ_INACTIVE:
	    निश्चित(delay == 0);
	    q = &ctim->inactive;
	    अवरोध;

	  हाल SEQ_NBASSIGN:
	    q = &ctim->nbassign;
	    अवरोध;

	  हाल SEQ_RWSYNC:
	    q = &ctim->rwsync;
	    अवरोध;

	  हाल SEQ_ROSYNC:
	    q = &ctim->rosync;
	    अवरोध;

	  हाल DEL_THREAD:
	    q = &ctim->del_thr;
	    अवरोध;
      }

      अगर (q) {
	    अगर (*q) {
		  /* Put the cur event on the end of the queue. */
		  cur->अग्र_2 = (*q)->अग्र_2;
		  (*q)->अग्र_2 = cur;
	    }
	    *q = cur;
      }
}

अटल व्योम schedule_event_धकेलो(काष्ठा event_s*cur)
{
      अगर ((sched_list == 0) || (sched_list->delay > 0)) {
	    schedule_event_(cur, 0, SEQ_ACTIVE);
	    वापस;
      }

      काष्ठा event_time_s*ctim = sched_list;

      अगर (ctim->active == 0) {
	    cur->अग्र_2 = cur;
	    ctim->active = cur;
	    वापस;
      }

      cur->अग्र_2 = ctim->active->अग्र_2;
      ctim->active->अग्र_2 = cur;
}

व्योम schedule_vthread(vthread_t thr, vvp_time64_t delay, bool धकेलो_flag)
{
      काष्ठा vthread_event_s*cur = नया vthread_event_s;

      cur->thr = thr;
      vthread_mark_scheduled(thr);

      अगर (धकेलो_flag && (delay == 0)) {
	      /* Special case: If the delay is 0, the push_flag means
		 I can push this event in front of everything. This is
		 used by the %fork statement, for example, to perform
		 task calls. */
	    schedule_event_धकेलो(cur);

      } अन्यथा {
	    schedule_event_(cur, delay, SEQ_ACTIVE);
      }
}

व्योम schedule_t0_trigger(vvp_net_ptr_t ptr)
{
      vvp_vector4_t bit (1, BIT4_X);
      काष्ठा assign_vector4_event_s*cur = नया काष्ठा assign_vector4_event_s(bit);
      cur->ptr = ptr;
      schedule_event_(cur, 0, SEQ_INACTIVE);
}

व्योम schedule_inactive(vthread_t thr)
{
      काष्ठा vthread_event_s*cur = नया vthread_event_s;

      cur->thr = thr;
      vthread_mark_scheduled(thr);
      schedule_event_(cur, 0, SEQ_INACTIVE);
}

व्योम schedule_init_vthread(vthread_t thr)
{
      काष्ठा vthread_event_s*cur = नया vthread_event_s;

      cur->thr = thr;
      vthread_mark_scheduled(thr);

      schedule_init_event(cur);
}

व्योम schedule_final_vthread(vthread_t thr)
{
      काष्ठा vthread_event_s*cur = नया vthread_event_s;

      cur->thr = thr;
      vthread_mark_final(thr);
      vthread_mark_scheduled(thr);

      schedule_final_event(cur);
}

व्योम schedule_assign_vector(vvp_net_ptr_t ptr,
			    अचिन्हित base, अचिन्हित vwid,
			    स्थिर vvp_vector4_t&bit,
			    vvp_time64_t delay)
{
      काष्ठा assign_vector4_event_s*cur = नया काष्ठा assign_vector4_event_s(bit);
      cur->ptr = ptr;
      cur->base = base;
      cur->vwid = vwid;
      schedule_event_(cur, delay, SEQ_NBASSIGN);
}

व्योम schedule_force_vector(vvp_net_t*net,
			    अचिन्हित base, अचिन्हित vwid,
			    स्थिर vvp_vector4_t&bit,
			    vvp_time64_t delay)
{
      काष्ठा force_vector4_event_s*cur = नया काष्ठा force_vector4_event_s(bit);
      cur->net = net;
      cur->base = base;
      cur->vwid = vwid;
      schedule_event_(cur, delay, SEQ_NBASSIGN);
}

व्योम schedule_propagate_vector(vvp_net_t*net,
			       vvp_time64_t delay,
			       स्थिर vvp_vector4_t&src)
{
      काष्ठा propagate_vector4_event_s*cur
	    = नया काष्ठा propagate_vector4_event_s(src);
      cur->net = net;
      schedule_event_(cur, delay, SEQ_NBASSIGN);
}

// FIXME: This needs to create a non-blocking event, but only one per time slot.
//        Is schedule_event_ or execution actually filtering since the net is
//        already X because this is not triggering?
व्योम schedule_propagate_event(vvp_net_t*net,
                              vvp_time64_t delay)
{
      vvp_vector4_t tmp (1, BIT4_X);
      काष्ठा propagate_vector4_event_s*cur
	    = नया काष्ठा propagate_vector4_event_s(tmp);
      cur->net = net;
      schedule_event_(cur, delay, SEQ_NBASSIGN);
}

व्योम schedule_assign_array_word(vvp_array_t mem,
				अचिन्हित word_addr,
				अचिन्हित off,
				स्थिर vvp_vector4_t&val,
				vvp_time64_t delay)
{
      काष्ठा assign_array_word_s*cur = नया काष्ठा assign_array_word_s;
      cur->mem = mem;
      cur->adr = word_addr;
      cur->off = off;
      cur->val = val;
      schedule_event_(cur, delay, SEQ_NBASSIGN);
}

व्योम schedule_assign_array_word(vvp_array_t mem,
				अचिन्हित word_addr,
				द्विगुन val,
				vvp_time64_t delay)
{
      काष्ठा assign_array_r_word_s*cur = नया काष्ठा assign_array_r_word_s;
      cur->mem = mem;
      cur->adr = word_addr;
      cur->val = val;
      schedule_event_(cur, delay, SEQ_NBASSIGN);
}

व्योम schedule_set_vector(vvp_net_ptr_t ptr, स्थिर vvp_vector4_t&bit)
{
      काष्ठा assign_vector4_event_s*cur = नया काष्ठा assign_vector4_event_s(bit);
      cur->ptr = ptr;
      cur->base = 0;
      cur->vwid = 0;
      schedule_event_(cur, 0, SEQ_ACTIVE);
}

व्योम schedule_set_vector(vvp_net_ptr_t ptr, स्थिर vvp_vector8_t&bit)
{
      काष्ठा assign_vector8_event_s*cur = नया काष्ठा assign_vector8_event_s;
      cur->ptr = ptr;
      cur->val = bit;
      schedule_event_(cur, 0, SEQ_ACTIVE);
}

व्योम schedule_set_vector(vvp_net_ptr_t ptr, द्विगुन bit)
{
      काष्ठा assign_real_event_s*cur = नया काष्ठा assign_real_event_s;
      cur->ptr = ptr;
      cur->val = bit;
      schedule_event_(cur, 0, SEQ_ACTIVE);
}

व्योम schedule_init_vector(vvp_net_ptr_t ptr, स्थिर vvp_vector4_t&bit)
{
      काष्ठा assign_vector4_event_s*cur = नया काष्ठा assign_vector4_event_s(bit);
      cur->ptr = ptr;
      cur->base = 0;
      cur->vwid = 0;
      schedule_init_event(cur);
}

व्योम schedule_init_vector(vvp_net_ptr_t ptr, स्थिर vvp_vector8_t&bit)
{
      काष्ठा assign_vector8_event_s*cur = नया काष्ठा assign_vector8_event_s;
      cur->ptr = ptr;
      cur->val = bit;
      schedule_init_event(cur);
}

व्योम schedule_init_vector(vvp_net_ptr_t ptr, द्विगुन bit)
{
      काष्ठा assign_real_event_s*cur = नया काष्ठा assign_real_event_s;
      cur->ptr = ptr;
      cur->val = bit;
      schedule_init_event(cur);
}

व्योम schedule_init_propagate(vvp_net_t*net, vvp_vector4_t bit)
{
      काष्ठा propagate_vector4_event_s*cur = नया काष्ठा propagate_vector4_event_s(bit);
      cur->net = net;
      schedule_init_event(cur);
}

व्योम schedule_init_propagate(vvp_net_t*net, द्विगुन bit)
{
      काष्ठा propagate_real_event_s*cur = नया काष्ठा propagate_real_event_s;
      cur->net = net;
      cur->val = bit;
      schedule_init_event(cur);
}

व्योम schedule_del_thr(vthread_t thr)
{
      काष्ठा del_thr_event_s*cur = नया del_thr_event_s;

      cur->thr = thr;

      schedule_event_(cur, 0, DEL_THREAD);
}

व्योम schedule_generic(vvp_gen_event_t obj, vvp_time64_t delay,
		      bool sync_flag, bool ro_flag, bool delete_when_done)
{
      काष्ठा generic_event_s*cur = नया generic_event_s;

      cur->obj = obj;
      cur->delete_obj_when_done = delete_when_done;
      schedule_event_(cur, delay,
		      sync_flag? (ro_flag?SEQ_ROSYNC:SEQ_RWSYNC) : SEQ_ACTIVE);

      अगर (sync_flag)
	    vthread_delay_delete();
}

अटल bool sim_started;

व्योम schedule_functor(vvp_gen_event_t obj)
{
      काष्ठा generic_event_s*cur = नया generic_event_s;

      cur->obj = obj;
      cur->delete_obj_when_done = false;
      अगर (!sim_started) {
            schedule_init_event(cur);
      } अन्यथा {
            schedule_event_(cur, 0, SEQ_ACTIVE);
      }
}

व्योम schedule_at_start_of_simtime(vvp_gen_event_t obj, vvp_time64_t delay)
{
      काष्ठा generic_event_s*cur = नया generic_event_s;

      cur->obj = obj;
      cur->delete_obj_when_done = false;
      schedule_event_(cur, delay, SEQ_START);
}

/*
 * In the vvp runtime of Icarus Verilog, the SEQ_RWSYNC time step is
 * after all of the non-blocking assignments, so is effectively the
 * same as the ReadWriteSync time.
 */
व्योम schedule_at_end_of_simtime(vvp_gen_event_t obj, vvp_time64_t delay)
{
      काष्ठा generic_event_s*cur = नया generic_event_s;

      cur->obj = obj;
      cur->delete_obj_when_done = false;
      schedule_event_(cur, delay, SEQ_RWSYNC);
}

अटल vvp_time64_t schedule_time;
vvp_time64_t schedule_simtime(व्योम)
{ वापस schedule_time; }

अटल bool sim_at_rosync = false;
bool schedule_at_rosync(व्योम)
{ वापस sim_at_rosync; }

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
अटल व्योम run_rosync(काष्ठा event_time_s*ctim)
{
      sim_at_rosync = true;
      जबतक (ctim->rosync) {
	    काष्ठा event_s*cur = ctim->rosync->अग्र_2;
	    अगर (cur->अग्र_2 == cur) {
		  ctim->rosync = 0;
	    } अन्यथा {
		  ctim->rosync->अग्र_2 = cur->अग्र_2;
	    }

	    cur->run_run();
	    मिटाओ cur;
      }
      sim_at_rosync = false;

      जबतक (ctim->del_thr) {
	    काष्ठा event_s*cur = ctim->del_thr->अग्र_2;
	    अगर (cur->अग्र_2 == cur) {
		  ctim->del_thr = 0;
	    } अन्यथा {
		  ctim->del_thr->अग्र_2 = cur->अग्र_2;
	    }

	    cur->run_run();
	    मिटाओ cur;
      }

      अगर (ctim->active || ctim->inactive || ctim->nbassign || ctim->rwsync) {
	    अ_त्रुटि << "SCHEDULER ERROR: read-only sync events "
		 << "created RW events!" << प_पूर्ण;
      }
}

व्योम schedule_simulate(व्योम)
{
      bool run_finals;
      sim_started = false;

      schedule_time = 0;

      अगर (verbose_flag) {
	    vpi_mcd_printf(1, " ...execute EndOfCompile callbacks\n");
      }

      // Execute end of compile callbacks
      vpiEndOfCompile();

      अगर (verbose_flag) {
	    vpi_mcd_printf(1, " ...propagate initialization events\n");
      }

	// Execute initialization events.
      जबतक (schedule_init_list) {
	    काष्ठा event_s*cur = schedule_init_list->अग्र_2;
	    अगर (cur->अग्र_2 == cur) {
		  schedule_init_list = 0;
	    } अन्यथा {
		  schedule_init_list->अग्र_2 = cur->अग्र_2;
	    }
	    cur->run_run();
	    मिटाओ cur;
      }

      sim_started = true;

      अगर (verbose_flag) {
	    vpi_mcd_printf(1, " ...execute StartOfSim callbacks\n");
      }

      // Execute start of simulation callbacks
      vpiStartOfSim();

      signals_capture();

      अगर (verbose_flag) {
	    vpi_mcd_printf(1, " ...run scheduler\n");
      }

      // If there were no compiletf, etc. errors then we are going to
      // process events and when done run the final blocks.
      run_finals = schedule_runnable;

      अगर (schedule_runnable) जबतक (sched_list) {

	    अगर (schedule_stopped_flag) {
		  schedule_stopped_flag = false;
		  stop_handler(0);
		  // You can finish from the debugger without a time change.
		  अगर (!schedule_runnable) अवरोध;
		  जारी;
	    }

	      /* ctim is the current time step. */
	    काष्ठा event_time_s* ctim = sched_list;

	      /* If the time is advancing, then first run the
		 postponed sync events. Run them all. */
	    अगर (ctim->delay > 0) {

		  अगर (!schedule_runnable) अवरोध;
		  schedule_time += ctim->delay;
		    /* When the design is being traced (we are emitting
		     * file/line information) also print any time changes. */
		  अगर (show_file_पंक्ति) {
			अ_त्रुटि << "Advancing to simulation time: "
			     << schedule_time << प_पूर्ण;
		  }
		  ctim->delay = 0;

		  vpi_अग्र_Sim_Time();
		    // Process the cbAtStartOfSimTime callbacks.
		  जबतक (ctim->start) {
			काष्ठा event_s*cur = ctim->start->अग्र_2;
			अगर (cur->अग्र_2 == cur) {
			      ctim->start = 0;
			} अन्यथा {
			      ctim->start->अग्र_2 = cur->अग्र_2;
			}
			cur->run_run();
			मिटाओ (cur);
		  }
	    }


	      /* If there are no more active events, advance the event
		 queues. If there are not events at all, then release
		 the event_time object. */
	    अगर (ctim->active == 0) {
		  ctim->active = ctim->inactive;
		  ctim->inactive = 0;

		  अगर (ctim->active == 0) {
			ctim->active = ctim->nbassign;
			ctim->nbassign = 0;

			अगर (ctim->active == 0) {
			      ctim->active = ctim->rwsync;
			      ctim->rwsync = 0;

				/* If out of rw events, then run the rosync
				   events and delete this time step. This also
				   deletes threads as needed. */
			      अगर (ctim->active == 0) {
				    run_rosync(ctim);
				    sched_list = ctim->अग्र_2;
				    मिटाओ ctim;
				    जारी;
			      }
			}
		  }
	    }

	      /* Pull the first item off the list. If this is the last
		 cell in the list, then clear the list. Execute that
		 event type, and delete it. */
	    काष्ठा event_s*cur = ctim->active->अग्र_2;
	    अगर (cur->अग्र_2 == cur) {
		  ctim->active = 0;
	    } अन्यथा {
		  ctim->active->अग्र_2 = cur->अग्र_2;
	    }

	    अगर (schedule_single_step_flag) {
		  cur->single_step_display();
		  schedule_stopped_flag = true;
		  schedule_single_step_flag = false;
	    }

	    cur->run_run();

	    मिटाओ (cur);
      }

	// Execute final events.
      schedule_runnable = run_finals;
      जबतक (schedule_runnable && schedule_final_list) {
	    काष्ठा event_s*cur = schedule_final_list->अग्र_2;
	    अगर (cur->अग्र_2 == cur) {
		  schedule_final_list = 0;
	    } अन्यथा {
		  schedule_final_list->अग्र_2 = cur->अग्र_2;
	    }
	    cur->run_run();
	    मिटाओ cur;
      }

      signals_revert();

      अगर (verbose_flag) {
	    vpi_mcd_printf(1, " ...execute Postsim callbacks\n");
      }

      // Execute post-simulation callbacks
      vpiPostsim();
#अगर_घोषित CHECK_WITH_VALGRIND
      schedule_delete();
#पूर्ण_अगर
}

#अगर_घोषित CHECK_WITH_VALGRIND
व्योम schedule_delete(व्योम)
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
#पूर्ण_अगर
