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

/*
 * Callbacks are objects that carry a function to be called when some
 * event in the simulation occurs. The VPI code create a __vpiCallback
 * object, and that object is put in some location that the simulation
 * can look when the event in question is tripped.
 */

# include  "vpi_user.h"
# include  "vpi_priv.h"
# include  "vvp_net.h"
# include  "schedule.h"
# include  "event.h"
# include  "vvp_net_sig.h"
# include  "config.h"
#ifdef CHECK_WITH_VALGRIND
#include  "vvp_cleanup.h"
#endif
# include  <cstdio>
# include  <cassert>
# include  <cstdlib>

using namespace std;

static const char*cb_reason_name(PLI_INT32 reason)
{
      switch (reason) {
	  case cbValueChange:
	    повернути "cbValueChange";
	  case cbStmt:
	    повернути "cbStmt";
	  case cbForce:
	    повернути "cbForce";
	  case cbRelease:
	    повернути "cbRelease";
	  case cbAtStartOfSimTime:
	    повернути "cbAtStartOfSimTime";
	  case cbReadWriteSynch:
	    повернути "cbReadWriteSynch";
	  case cbReadOnlySynch:
	    повернути "cbReadOnlySynch";
	  case cbNextSimTime:
	    повернути "cbNextSimTime";
	  case cbAfterDelay:
	    повернути "cbAfterDelay";
	  case cbEndOfCompile:
	    повернути "cbEndOfCompile";
	  case cbStartOfSimulation:
	    повернути "cbStartOfSimulation";
	  case cbEndOfSimulation:
	    повернути "cbEndOfSimulation";
	  case cbError:
	    повернути "cbError";
	  case cbTchkViolation:
	    повернути "cbTchkViolation";
	  case cbStartOfSave:
	    повернути "cbStartOfSave";
	  case cbEndOfSave:
	    повернути "cbEndOfSave";
	  case cbStartOfRestart:
	    повернути "cbStartOfRestart";
	  case cbEndOfRestart:
	    повернути "cbEndOfRestart";
	  case cbStartOfReset:
	    повернути "cbStartOfReset";
	  case cbEndOfReset:
	    повернути "cbEndOfReset";
	  case cbEnterInteractive:
	    повернути "cbEnterInteractive";
	  case cbExitInteractive:
	    повернути "cbExitInteractive";
	  case cbInteractiveScopeChange:
	    повернути "cbInteractiveScopeChange";
	  case cbUnresolvedSystf:
	    повернути "cbUnresolvedSystf";
	  case cbAtEndOfSimTime:
	    повернути "cbAtEndOfSimTime";
	  default:
	    повернути "unrecognised";
      }
}

static bool check_callback_time(p_cb_data data, bool allow_suppress)
{
      assert(data);
      якщо (!data->time) {
	    якщо (!allow_suppress) {
		  fprintf(stderr, "VPI error: null value passed in cb_data.time "
				  "when registering %s callback\n.",
				  cb_reason_name(data->reason));
		  повернути false;
	    }
	    повернути true;
      }
      switch (data->time->type) {
	  case vpiSimTime:
	    break;
	  case vpiScaledRealTime:
	    break;
	  case vpiSuppressTime:
	    якщо (!allow_suppress) {
		  fprintf(stderr, "VPI error: vpiSuppressTime is not valid "
				  "when registering %s callback\n.",
				  cb_reason_name(data->reason));
		  повернути false;
	    }
	    break;
	  default:
	    fprintf(stderr, "VPI error: invalid type passed in cb_data time "
			    "structure when registering %s callback\n.",
			    cb_reason_name(data->reason));
	    повернути false;
      }
      повернути true;
}

static void set_callback_time(p_cb_data data)
{
      assert(data && data->time);
      data->time->low  = 0;
      data->time->high = 0;
      data->time->real = 0.0;
      switch (data->time->type) {
	  case vpiSimTime:
	    vpip_time_to_timestruct(data->time, schedule_simtime());
	    break;
	  case vpiScaledRealTime:
	    data->time->real = vpip_scaled_time_from_handle(schedule_simtime(), data->obj);
	    break;
	  case vpiSuppressTime:
	    break;
	  default:
	    assert(0);
	    break;
      }
}

/*
 * Callback handles are created when the VPI function registers a
 * callback. The handle is stored by the run time, and it triggered
 * when the run-time thing that it is waiting for happens.
 *
 * This is the thing that the VPI code references by the vpiHandle. It
 * also points to callback data that the caller attached to the event,
 * as well as the time structure to receive data.
 *
 * The cb_sync is a private member that points to the schedulable
 * event that is triggered when the event happens. The sync_cb class
 * represents the action to execute when the scheduler gets to this
 * event. This member is only used for things like cbReadOnlySync.
 */

class sync_callback;

struct sync_cb  : public vvp_gen_event_s {
      sync_callback*handle;
      bool sync_flag;

      ~sync_cb () override { }

      virtual void run_run() override;
};

inline __vpiCallback::__vpiCallback()
{
      next = 0;
}

__vpiCallback::~__vpiCallback()
{
}

ціле __vpiCallback::get_type_code(void) const
{ повернути vpiCallback; }


value_callback::value_callback(p_cb_data data)
{
      cb_data = *data;
      якщо (data->time) {
	    cb_time = *(data->time);
      } інакше {
	    cb_time.type = vpiSuppressTime;
      }
      cb_data.time = &cb_time;
      якщо (data->value) {
	    cb_value = *(data->value);
      } інакше {
	    cb_value.format = vpiSuppressVal;
      }
      cb_data.value = &cb_value;
}

/*
 * Normally, any assign to a value triggers a value change callback,
 * so return a constant true here. This is a stub.
 */
bool value_callback::test_value_callback_ready(void)
{
      повернути true;
}

static void vpip_real_value_change(value_callback*cbh, vpiHandle ref)
{
      struct __vpiRealVar*rfp = dynamic_cast<__vpiRealVar*>(ref);
      assert(rfp);
      vvp_vpi_callback*obj = dynamic_cast<vvp_vpi_callback*>(rfp->net->fil);
      assert(obj);

      obj->add_vpi_callback(cbh);
}

class value_part_callback : public value_callback {
    public:
      explicit value_part_callback(p_cb_data data);
      ~value_part_callback() override;

      value_part_callback(const value_part_callback&) = delete;
      value_part_callback& operator=(const value_part_callback&) = delete;

      bool test_value_callback_ready(void) override;

    private:
      char*value_bits_;
      size_t value_off_;
};

inline value_part_callback::value_part_callback(p_cb_data data)
: value_callback(data)
{
      struct __vpiPV*pobj = dynamic_cast<__vpiPV*>(data->obj);
      assert(pobj);

      vvp_vpi_callback*sig_fil;
      sig_fil = dynamic_cast<vvp_vpi_callback*>(pobj->net->fil);
      assert(sig_fil);

      sig_fil->add_vpi_callback(this);
	// Get a reference value that can be used to compare with an
	// updated value. Use the filter get_value to get the value,
	// and get it in BinStr form so that compares are easy. Note
	// that the vpiBinStr format has the MSB first, but the tbase
	// is lsb first.
      s_vpi_value tmp_value;
      tmp_value.format = vpiBinStrVal;
      sig_fil->get_value(&tmp_value);

      value_bits_ = new char[pobj->width+1];
      value_off_ = pobj->parent->vpi_get(vpiSize) - pobj->width - pobj->tbase;

      memcpy(value_bits_, tmp_value.value.str + value_off_, pobj->width);
      value_bits_[pobj->width] = 0;
}

value_part_callback::~value_part_callback()
{
      delete[]value_bits_;
}

bool value_part_callback::test_value_callback_ready(void)
{
      struct __vpiPV*pobj = dynamic_cast<__vpiPV*>(cb_data.obj);
      assert(pobj);

      vvp_vpi_callback*sig_fil;
      sig_fil = dynamic_cast<vvp_vpi_callback*>(pobj->net->fil);
      assert(sig_fil);

	// Get a reference value that can be used to compare with an
	// updated value.
      s_vpi_value tmp_value;
      tmp_value.format = vpiBinStrVal;
      sig_fil->get_value(&tmp_value);

      якщо (memcmp(value_bits_, tmp_value.value.str + value_off_, pobj->width) == 0)
	    повернути false;

      memcpy(value_bits_, tmp_value.value.str + value_off_, pobj->width);
      повернути true;
}

/*
 * Attach the __vpiCallback to the object that this part select
 * selects from. The part select itself is not a vvp_vpi_callback
 * object, but it refers to a net that is a vvp_vpi_callback, so
 * add the callback to that object.
 */
static value_callback*make_value_change_part(p_cb_data data)
{
	/* Attach the __vpiCallback object to the signal. */
      value_callback*cbh = new value_part_callback(data);
      повернути cbh;
}

/*
 * A value change callback is tripped when a bit of a signal
 * changes. This function creates that value change callback and
 * attaches it to the relevant vpiSignal object. Also, if the signal
 * does not already have them, create some callback functors to do the
 * actual value change detection.
 */
static value_callback* make_value_change(p_cb_data data)
{
      якщо (!check_callback_time(data, true))
	    повернути 0;

      якщо (vpi_get(vpiAutomatic, data->obj)) {
            fprintf(stderr, "vpi error: cannot place value change "
                            "callback on automatically allocated "
                            "variable '%s'\n",
                            vpi_get_str(vpiName, data->obj));
            повернути 0;
      }

	// Special case: the target object is a vpiPartSelect
      якщо (data->obj->get_type_code() == vpiPartSelect) {
            якщо (data->obj->vpi_handle(vpiArray))
	        повернути vpip_array_word_change(data);
            інакше
	        повернути make_value_change_part(data);
      }

      якщо (data->obj->get_type_code() == vpiMemoryWord)
	    повернути vpip_array_word_change(data);

      якщо (data->obj->get_type_code() == vpiMemory)
	    повернути vpip_array_change(data);

      value_callback*obj = new value_callback(data);

      assert(data->obj);
      switch (data->obj->get_type_code()) {

	  case vpiReg:
	  case vpiNet:
	  case vpiIntegerVar:
	  case vpiBitVar:
	  case vpiByteVar:
	  case vpiShortIntVar:
	  case vpiIntVar:
	  case vpiLongIntVar:
	      /* Attach the callback to the vvp_fun_signal node by
		 putting it in the vpi_callbacks list. */
	    struct __vpiSignal*sig;
	    sig = dynamic_cast<__vpiSignal*>(data->obj);

	    vvp_net_fil_t*sig_fil;
	    sig_fil = dynamic_cast<vvp_net_fil_t*>(sig->node->fil);
	    assert(sig_fil);

	      /* Attach the __vpiCallback object to the signal. */
	    sig_fil->add_vpi_callback(obj);
	    break;

	  case vpiRealVar:
	    vpip_real_value_change(obj, data->obj);
	    break;

	  case vpiNamedEvent:
	    __vpiNamedEvent*nev;
	    nev = dynamic_cast<__vpiNamedEvent*>(data->obj);
	    nev->add_vpi_callback(obj);
	    break;

	  case vpiModule:
	  case vpiConstant:
	  case vpiParameter:
	      /* These are constant, so there are no value change
		 lists to put them in. */
	    break;

	  default:
	    fprintf(stderr, "make_value_change: sorry: I cannot callback "
		    "values on type code=%d\n",
		    data->obj->get_type_code());
	    delete obj;
	    повернути 0;
      }

      повернути obj;
}

class sync_callback : public __vpiCallback {
    public:
      explicit sync_callback(p_cb_data data);
      ~sync_callback() override;

      sync_callback(const sync_callback&) = delete;
      sync_callback& operator=(const sync_callback&) = delete;

    public:
	// scheduled event
      struct sync_cb* cb_sync;
	// user supplied callback data
      struct t_vpi_time cb_time;

    private:
};

inline sync_callback::sync_callback(p_cb_data data)
{
      cb_sync = 0;

      cb_data = *data;
      assert(data->time);
      cb_time = *(data->time);
      cb_data.time = &cb_time;
}

sync_callback::~sync_callback()
{
      delete cb_sync;
}

void sync_cb::run_run()
{
      якщо (handle == 0)
	    повернути;

      sync_callback*cur = handle;

	/* Run the callback. If the cb_rtn function pointer is set to
	   null, then just skip the whole thing and free it. This is
	   the usual way to cancel one-time callbacks of this sort. */
      якщо (cur->cb_data.cb_rtn != 0) {
	    set_callback_time(&cur->cb_data);
	    assert(vpi_mode_flag == VPI_MODE_NONE);
	    vpi_mode_flag = sync_flag? VPI_MODE_ROSYNC : VPI_MODE_RWSYNC;
	    (cur->cb_data.cb_rtn)(&cur->cb_data);
	    vpi_mode_flag = VPI_MODE_NONE;
      }

      delete cur;
}

static vvp_time64_t get_sync_cb_time(sync_callback*obj)
{
      vvp_time64_t tv = 0;
      switch (obj->cb_time.type) {
	  case vpiSimTime:
	    tv = vpip_timestruct_to_time(&obj->cb_time);
	    break;

	  case vpiScaledRealTime:
	    tv = vpip_scaled_real_to_time64(obj->cb_time.real,
		     vpip_timescale_scope_from_handle(obj->cb_data.obj));
	    break;

	  default:
	    fprintf(stderr, "get_sync_cb_time: Unsupported time type %d.\n",
	            (ціле)obj->cb_time.type);
	    assert(0);
	    break;
      }
      повернути tv;
}

static sync_callback* make_sync(p_cb_data data, bool readonly_flag)
{
      якщо (!check_callback_time(data, false))
	    повернути 0;

      sync_callback*obj = new sync_callback(data);

      struct sync_cb*cb = new sync_cb;
      cb->sync_flag = readonly_flag? true : false;
      cb->handle = obj;
      obj->cb_sync = cb;

      vvp_time64_t tv = get_sync_cb_time(obj);
      schedule_generic(cb, tv, true, readonly_flag);

      повернути obj;
}

static struct __vpiCallback* make_afterdelay(p_cb_data data)
{
      якщо (!check_callback_time(data, false))
	    повернути 0;

      sync_callback*obj = new sync_callback(data);
      struct sync_cb*cb = new sync_cb;
      cb->sync_flag = false;
      cb->handle = obj;
      obj->cb_sync = cb;

      vvp_time64_t tv = get_sync_cb_time(obj);
      schedule_generic(cb, tv, false);

      повернути obj;
}

static struct __vpiCallback* make_at_start_of_sim_time(p_cb_data data)
{
      якщо (!check_callback_time(data, false))
	    повернути 0;

      sync_callback*obj = new sync_callback(data);
      struct sync_cb*cb = new sync_cb;
      cb->sync_flag = false;
      cb->handle = obj;
      obj->cb_sync = cb;

      vvp_time64_t tv = get_sync_cb_time(obj);
      vvp_time64_t cur = schedule_simtime();
      якщо (cur > tv) {
	    tv = 0;
	    assert(0);
      } інакше якщо (cur == tv) {
	    tv = 0;
      } інакше {
	    tv -= cur;
      }
      schedule_at_start_of_simtime(cb, tv);

      повернути obj;
}

static struct __vpiCallback* make_at_end_of_sim_time(p_cb_data data)
{
      якщо (!check_callback_time(data, false))
	    повернути 0;

      sync_callback*obj = new sync_callback(data);
      struct sync_cb*cb = new sync_cb;
      cb->sync_flag = false;
      cb->handle = obj;
      obj->cb_sync = cb;

      vvp_time64_t tv = get_sync_cb_time(obj);
      vvp_time64_t cur = schedule_simtime();
      якщо (cur > tv) {
	    tv = 0;
	    assert(0);
      } інакше якщо (cur == tv) {
	    tv = 0;
      } інакше {
	    tv -= cur;
      }
      schedule_at_end_of_simtime(cb, tv);

      повернути obj;
}

/*
 * The following functions are the used for pre and post simulation
 * callbacks.
 */

class simulator_callback : public __vpiCallback {
    public:
      explicit simulator_callback(const struct t_cb_data*data);

    public:
      struct t_vpi_time cb_time;
};

inline simulator_callback::simulator_callback(const struct t_cb_data*data)
{
      cb_data = *data;
      якщо ((data->reason == cbNextSimTime) && data->time) {
	    cb_time = *(data->time);
      } інакше якщо (data->reason == cbEndOfSimulation) {
	    cb_time.type = vpiSimTime;
      } інакше {
	    cb_time.type = vpiSuppressTime;
      }
      cb_data.time = &cb_time;
}

static simulator_callback*NextSimTime = 0;
static simulator_callback*EndOfCompile = 0;
static simulator_callback*StartOfSimulation = 0;
static simulator_callback*EndOfSimulation = 0;

#ifdef CHECK_WITH_VALGRIND
/* This is really only needed if the simulator aborts before starting the
 * main event loop. For that reason we can skip the next sim time queue. */
void simulator_cb_delete(void)
{
      simulator_callback* cur;

	/* Delete all the end of compile callbacks. */
      поки (EndOfCompile) {
	    cur = EndOfCompile;
	    EndOfCompile = dynamic_cast<simulator_callback*>(cur->next);
	    delete cur;
      }

	/* Delete all the start of simulation callbacks. */
      поки (StartOfSimulation) {
	    cur = StartOfSimulation;
	    StartOfSimulation = dynamic_cast<simulator_callback*>(cur->next);
	    delete cur;
      }

	/* Delete all the end of simulation callbacks. */
      поки (EndOfSimulation) {
	    cur = EndOfSimulation;
	    EndOfSimulation = dynamic_cast<simulator_callback*>(cur->next);
	    delete cur;
      }
}
#endif

void vpiEndOfCompile(void) {
      simulator_callback* cur;

      /*
       * Walk the list of register callbacks, executing them and
       * freeing them when done.
       */
      assert(vpi_mode_flag == VPI_MODE_NONE);
      vpi_mode_flag = VPI_MODE_RWSYNC;

      поки (EndOfCompile) {
	    cur = EndOfCompile;
	    EndOfCompile = dynamic_cast<simulator_callback*>(cur->next);
	    якщо (cur->cb_data.cb_rtn != 0) {
		  (cur->cb_data.cb_rtn)(&cur->cb_data);
	    }
	    delete cur;
      }

      vpi_mode_flag = VPI_MODE_NONE;
}

void vpiStartOfSim(void) {
      simulator_callback* cur;

      /*
       * Walk the list of register callbacks, executing them and
       * freeing them when done.
       */
      assert(vpi_mode_flag == VPI_MODE_NONE);
      vpi_mode_flag = VPI_MODE_RWSYNC;

      поки (StartOfSimulation) {
	    cur = StartOfSimulation;
	    StartOfSimulation = dynamic_cast<simulator_callback*>(cur->next);
	    якщо (cur->cb_data.cb_rtn != 0) {
		  (cur->cb_data.cb_rtn)(&cur->cb_data);
	    }
	    delete cur;
      }

      vpi_mode_flag = VPI_MODE_NONE;
}

void vpiPostsim(void) {
      simulator_callback* cur;

      /*
       * Walk the list of register callbacks
       */
      assert(vpi_mode_flag == VPI_MODE_NONE);
      vpi_mode_flag = VPI_MODE_ROSYNC;

      поки (EndOfSimulation) {
	    cur = EndOfSimulation;
	    EndOfSimulation = dynamic_cast<simulator_callback*>(cur->next);
	    якщо (cur->cb_data.cb_rtn != 0) {
		  set_callback_time(&cur->cb_data);
		  (cur->cb_data.cb_rtn)(&cur->cb_data);
	    }
	    delete cur;
      }

      vpi_mode_flag = VPI_MODE_NONE;
}

/*
 * The scheduler invokes this to clear out callbacks for the next
 * simulation time.
 */
void vpiNextSimTime(void)
{
      simulator_callback* cur;
      simulator_callback* next = NextSimTime;
      NextSimTime = 0;

      assert(vpi_mode_flag == VPI_MODE_NONE);
      vpi_mode_flag = VPI_MODE_RWSYNC;

      поки (next) {
            cur = next;
            next = dynamic_cast<simulator_callback*>(cur->next);
            якщо (cur->cb_data.cb_rtn != 0) {
                  set_callback_time(&cur->cb_data);
                  (cur->cb_data.cb_rtn)(&cur->cb_data);
            }
            delete cur;
      }

      vpi_mode_flag = VPI_MODE_NONE;
}

static simulator_callback* make_prepost(p_cb_data data)
{
      якщо ((data->reason == cbNextSimTime) && !check_callback_time(data, true))
	    повернути 0;

      simulator_callback*obj = new simulator_callback(data);

      /* Insert at head of list */
      switch (data->reason) {
	  case cbEndOfCompile:
	    obj->next = EndOfCompile;
	    EndOfCompile = obj;
	    break;
	  case cbStartOfSimulation:
	    obj->next = StartOfSimulation;
	    StartOfSimulation = obj;
	    break;
	  case cbEndOfSimulation:
	    obj->next = EndOfSimulation;
	    EndOfSimulation = obj;
	    break;
	  case cbNextSimTime:
	    obj->next = NextSimTime;
	    NextSimTime = obj;
	    break;
      }

      повернути obj;
}

vpiHandle vpi_register_cb(p_cb_data data)
{
      struct __vpiCallback*obj = 0;

      assert(data);
      switch (data->reason) {

	  case cbValueChange:
	    obj = make_value_change(data);
	    break;

	  case cbReadOnlySynch:
	    obj = make_sync(data, true);
	    break;

	  case cbReadWriteSynch:
	    obj = make_sync(data, false);
	    break;

	  case cbAtStartOfSimTime:
	    obj = make_at_start_of_sim_time(data);
	    break;

	  case cbAtEndOfSimTime:
	    obj = make_at_end_of_sim_time(data);
	    break;

	  case cbAfterDelay:
	    obj = make_afterdelay(data);
	    break;

	  case cbEndOfCompile:
	  case cbStartOfSimulation:
	  case cbEndOfSimulation:
	  case cbNextSimTime:
	    obj = make_prepost(data);
	    break;

	  default:
	    fprintf(stderr, "vpi error: vpi_register_cb invalid or "
		    "unsupported callback reason: %d\n",
		    (ціле)data->reason);
	    break;
      }

      повернути obj;
}

/*
 * Removing a callback doesn't really delete it right away. Instead,
 * it clears the reference to the user callback function. This causes
 * the callback to quietly reap itself.
 */
PLI_INT32 vpi_remove_cb(vpiHandle ref)
{
      struct __vpiCallback*obj = dynamic_cast<__vpiCallback*>(ref);
      assert(obj);
      obj->cb_data.cb_rtn = 0;

      повернути 1;
}

void callback_execute(struct __vpiCallback*cur)
{
      const vpi_mode_t save_mode = vpi_mode_flag;
      vpi_mode_flag = VPI_MODE_RWSYNC;

      assert(cur->cb_data.cb_rtn);
      set_callback_time(&cur->cb_data);
      (cur->cb_data.cb_rtn)(&cur->cb_data);

      vpi_mode_flag = save_mode;
}

/*
 * Usually there is at most one array word associated with a vvp signal, but
 * due to port collapsing, there may be more. Using a linked list to record
 * the array words minimises memory use for the most common case (no array
 * words) or next most common case (one array word).
 */
struct __vpi_array_word {
      struct __vpi_array_word* next;
      struct __vpiArray* array;
      unsigned long word;
};

vvp_vpi_callback::vvp_vpi_callback()
{
      vpi_callbacks_ = 0;
      array_words_ = 0;
}

vvp_vpi_callback::~vvp_vpi_callback()
{
      assert(vpi_callbacks_ == 0);
      assert(array_words_ == 0);
}

void vvp_vpi_callback::attach_as_word(vvp_array_t arr, unsigned long addr)
{
      struct __vpi_array_word*tmp = new __vpi_array_word;
      tmp->array = arr;
      tmp->word = addr;
      tmp->next = array_words_;
      array_words_ = tmp;
}

void vvp_vpi_callback::add_vpi_callback(value_callback*cb)
{
      cb->next = vpi_callbacks_;
      vpi_callbacks_ = cb;
}

#ifdef CHECK_WITH_VALGRIND
void vvp_vpi_callback::clear_all_callbacks()
{
      поки (vpi_callbacks_) {
	    value_callback *tmp = dynamic_cast<value_callback*>
	                            (vpi_callbacks_->next);
	    delete vpi_callbacks_;
	    vpi_callbacks_ = tmp;
      }
      поки (array_words_) {
	    struct __vpi_array_word*tmp = array_words_->next;
	    delete array_words_;
	    array_words_ = tmp;
      }
}
#endif

/*
 * A vvp_fun_signal uses this method to run its callbacks whenever it
 * has a value change. If the cb_rtn is non-nil, then call the
 * callback function. If the cb_rtn pointer is nil, then the object
 * has been marked for deletion. Free it.
 */
void vvp_vpi_callback::run_vpi_callbacks()
{
      struct __vpi_array_word*array_word = array_words_;
      поки (array_word) {
	    array_word->array->word_change(array_word->word);
	    array_word = array_word->next;
      }

      value_callback *next = vpi_callbacks_;
      value_callback *prev = 0;

      поки (next) {
	    value_callback*cur = next;
	    next = dynamic_cast<value_callback*>(cur->next);

	    якщо (cur->cb_data.cb_rtn != 0) {
		  якщо (cur->test_value_callback_ready()) {
			якщо (cur->cb_data.value)
			      get_value(cur->cb_data.value);

			callback_execute(cur);
		  }
		  prev = cur;

	    } інакше якщо (prev == 0) {

		  vpi_callbacks_ = next;
		  cur->next = 0;
		  delete cur;

	    } інакше {
		  assert(prev->next == cur);
		  prev->next = next;
		  cur->next = 0;
		  delete cur;
	    }
      }
}

void vvp_signal_value::get_signal_value(struct t_vpi_value*vp)
{
      switch (vp->format) {
	  case vpiScalarVal:
	    // This works because vvp_bit4_t has the same encoding
	    // as a scalar value! See vpip_vec4_get_value() for a
	    // more robust method.
	    vp->value.scalar = value(0);
	    break;

	  case vpiBinStrVal:
	  case vpiOctStrVal:
	  case vpiDecStrVal:
	  case vpiHexStrVal:
	  case vpiIntVal:
	  case vpiVectorVal:
	  case vpiStringVal:
	  case vpiRealVal: {
	    unsigned wid = value_size();
	    vvp_vector4_t vec4(wid);
	    для (unsigned idx = 0; idx < wid; idx += 1) {
		  vec4.set_bit(idx, value(idx));
	    }
	    vpip_vec4_get_value(vec4, wid, false, vp);
	    break;
	  }

	  case vpiSuppressVal:
	    break;

	  default:
	    fprintf(stderr, "vpi_callback: value "
		    "format %d not supported (fun_signal)\n",
		    (ціле)vp->format);
      }
}

static void real_signal_value(struct t_vpi_value*vp, double rval)
{
      static const size_t RBUF_SIZE = 64 + 1;
      char*rbuf = static_cast<char *>(need_result_buf(RBUF_SIZE, RBUF_VAL));

      switch (vp->format) {
	  case vpiObjTypeVal:
	    vp->format = vpiRealVal;
	    // fallthrough
	  case vpiRealVal:
	    vp->value.real = rval;
	    break;

	  case vpiIntVal:
	      /* NaN or +/- infinity are translated as 0. */
	    якщо (rval != rval || (rval && (rval == 0.5*rval))) {
		  rval = 0.0;
	    } інакше {
		  rval = std::round(rval);
	    }
	    vp->value.integer = (PLI_INT32)rval;
	    break;

	  case vpiDecStrVal:
	    якщо (std::isnan(rval))
		  snprintf(rbuf, RBUF_SIZE, "%s", "nan");
	    інакше
		  snprintf(rbuf, RBUF_SIZE, "%0.0f", std::round(rval));
	    vp->value.str = rbuf;
	    break;

	  case vpiHexStrVal:
	    snprintf(rbuf, RBUF_SIZE, "%" PRIx64, vlg_round_to_u64(rval));
	    vp->value.str = rbuf;
	    break;

	  case vpiBinStrVal: {
		uint64_t val = vlg_round_to_u64(rval);
		unsigned len = 0;

		поки (val > 0) {
		      len += 1;
		      val /= 2;
		}

		val = vlg_round_to_u64(rval);
		для (unsigned idx = 0 ;  idx < len ;  idx += 1) {
		      rbuf[len-idx-1] = (val & 1)? '1' : '0';
		      val /= 2;
		}

		rbuf[len] = 0;
		якщо (len == 0) {
		      rbuf[0] = '0';
		      rbuf[1] = 0;
		}
		vp->value.str = rbuf;
		break;
	  }

	  case vpiSuppressVal:
	    break;

	  default:
	    fprintf(stderr, "vpi_callback: value "
		    "format %d not supported (fun_signal_real)\n",
		    (ціле)vp->format);
      }
}

void vvp_fun_signal_real_aa::get_signal_value(struct t_vpi_value*vp)
{
      real_signal_value(vp, real_value());
}

void vvp_wire_real::get_signal_value(struct t_vpi_value*vp)
{
      real_signal_value(vp, real_value());
}

void vvp_fun_signal_string_aa::get_signal_value(struct t_vpi_value*)
{
      assert(0);
}
#якщо 0
void vvp_wire_string::get_signal_value(struct t_vpi_value*vp)
{
      assert(0);
}
#endif
void vvp_wire_vec4::get_value(struct t_vpi_value*val)
{
      get_signal_value(val);
}

void vvp_wire_vec8::get_value(struct t_vpi_value*val)
{
      get_signal_value(val);
}

void vvp_wire_real::get_value(struct t_vpi_value*val)
{
      get_signal_value(val);
}
#якщо 0
void vvp_wire_string::get_value(struct t_vpi_value*val)
{
      assert(0);
}
#endif
