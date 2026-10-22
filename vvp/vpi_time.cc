/*
 * Copyright (c) 2001-2025 Stephen Williams (steve@icarus.com)
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
# include  "vpi_priv.h"
# include  "schedule.h"
# include  <cstdio>
# include  <cmath>
# include  <cassert>

/*
 * The $time system function is supported in VPI contexts (i.e. an
 * argument to a system task/function) as a vpiSysFuncCall object. The
 * $display function divines that this is a function call and uses a
 * vpi_get_value to get the value.
 */

/*
 * vpi_time_precision is the precision of the simulation clock. It is
 * set by the :vpi_time_precision directive in the vvp source file.
 */
સ્થિર પૂર્ણાંક vpi_time_precision = 0;

સ્થિર સંરચના __vpiSystemTime global_simtime;

શૂન્ય vpip_time_to_timestruct(સંરચના t_vpi_time*ts, vvp_time64_t ti)
{
      ts->નીચલું  = ti & 0xFFFFFFFF;
      ts->ઉપલું = (ti >> 32) & 0xFFFFFFFF;
}

vvp_time64_t vpip_timestruct_to_time(અચલ સંરચના t_vpi_time*ts)
{
      vvp_time64_t ti = ts->ઉપલું;
      ti <<= 32;
      ti += ts->નીચલું & 0xffffffff;
      પાછા ti;
}

દ્વિગુણ vpip_time_to_scaled_real(vvp_time64_t ti, અચલ __vpiScope*scope)
{
      દ્વિગુણ val;
      પૂર્ણાંક scale = 0;
      જો (scope) scale = vpi_time_precision - scope->time_units;

      જો (scale >= 0) val = (દ્વિગુણ)ti * pow(10.0, scale);
      નહીંતર val = (દ્વિગુણ)ti / pow(10.0, -scale);

      પાછા val;
}

/*
 * This routine does not currently support negative real delays and it
 * does not check for overflow. It is only used for modpath delays and
 * they are required to be non-negative.
 */
vvp_time64_t vpip_scaled_real_to_time64(દ્વિગુણ val, અચલ __vpiScope*scope)
{
      પૂર્ણાંક shift = 0;
      જો (scope) shift = scope->time_units - scope->time_precision;
      પ્રતિજ્ઞા(shift >= 0);

      પ્રતિજ્ઞા(val >= 0);

	// Scale to the local precision and then round away from zero.
      દ્વિગુણ scale = pow(10.0L, shift);
      val *= scale;

      vvp_time64_t delay = (vvp_time64_t) (val + 0.5);

	// If needed now scale the value to the simulator precision.
      જો (scope) {
	    shift = scope->time_precision - vpi_time_precision;
	    પ્રતિજ્ઞા(shift >= 0);
	    માટે (પૂર્ણાંક lp = 0; lp <  shift; lp += 1) delay *= 10;
      }

      પાછા delay;
}

સ્થિર શૂન્ય timevar_get_કિંમત(vpiHandle ref, s_vpi_કિંમત*vp, તાર્કિક is_int_func,
                              તાર્કિક is_stime)
{
	/* Keep a persistent structure for passing time values back to
	   the caller. */
      સ્થિર સંરચના t_vpi_time time_કિંમત;

      સંરચના __vpiSystemTime*rfp = dynamic_cast<__vpiSystemTime*>(ref);
      unsigned long અંક_bits;
      vvp_time64_t x, simtime = schedule_simtime();
      પૂર્ણાંક units = rfp->scope? rfp->scope->time_units : vpi_time_precision;

      સ્થિર અચલ size_t RBUF_USE_SIZE = 128;
      અક્ષર*rbuf = static_cast<અક્ષર *>(need_result_buf(RBUF_USE_SIZE, RBUF_VAL));

	/* Calculate the divisor needed to scale the simulation time
	   (in time_precision units) to time units of the scope. */
      vvp_time64_t ભાજક = 1;
      જ્યારે (units > vpi_time_precision) {
	    ભાજક *= 10;
	    units -= 1;
      }

	/* Scale the simtime, and use the modulus to round up if
	   appropriate. */
      vvp_time64_t simtime_fraction = simtime % ભાજક;
      simtime /= ભાજક;

      જો ((ભાજક >= 10) && (simtime_fraction >= (ભાજક/2)))
	    simtime += 1;

	/* If this is a call to $stime only return the lower 32 bits. */
      જો (is_stime) simtime &= 0xffffffff;

      પસંદગી (vp->format) {
	  સ્થિતિ vpiObjTypeVal:
	      /* The default format is vpiTimeVal. */
	    vp->format = vpiTimeVal;
	    // fallthrough
	  સ્થિતિ vpiTimeVal:
	    vp->value.time = &time_કિંમત;
	    vp->value.time->type = vpiSimTime;
	    vpip_time_to_timestruct(vp->value.time, simtime);
	    તોડો;

	  સ્થિતિ vpiRealVal:
	      /* If this is an integer based call (anything but $realtime)
	       * just return the value as a double. */
	    જો (is_int_func)  vp->value.real = દ્વિગુણ (simtime);
	      /* This is a call to $realtime to return a real value so
	       * scale this using the scaled real rules. */
	    નહીંતર vp->value.real = vpip_time_to_scaled_real(schedule_simtime(),
						           rfp->scope);
	    તોડો;

	  સ્થિતિ vpiBinStrVal:
	    x = simtime;
	    અંક_bits = 8 * કદ(vvp_time64_t);

	    rbuf[અંક_bits] = 0;
	    માટે (unsigned i = 1; i <= અંક_bits; i++) {
	      rbuf[અંક_bits-i] = (x  & 1) ? '1' : '0';
	      x = x >> 1;
	    }

	    vp->value.str = rbuf;
	    તોડો;

	  સ્થિતિ vpiDecStrVal:
	    snprintf(rbuf, RBUF_USE_SIZE, "%" TIME_FMT_U, simtime);
	    vp->value.str = rbuf;
	    તોડો;

	  સ્થિતિ vpiOctStrVal:
	    snprintf(rbuf, RBUF_USE_SIZE, "%" TIME_FMT_O, simtime);
	    vp->value.str = rbuf;
	    તોડો;

	  સ્થિતિ vpiHexStrVal:
	    snprintf(rbuf, RBUF_USE_SIZE, "%" TIME_FMT_X, simtime);
	    vp->value.str = rbuf;
	    તોડો;

	  મૂળભૂત:
	    fprintf(stderr, "vpi_time: unknown format: %d\n", (પૂર્ણાંક)vp->format);
	    પ્રતિજ્ઞા(0);
      }
}

સ્થિર શૂન્ય timevar_get_ivalue(vpiHandle ref, s_vpi_કિંમત*vp)
{
      timevar_get_કિંમત(ref, vp, સાચું, ખોટું);
}

સ્થિર શૂન્ય timevar_get_svalue(vpiHandle ref, s_vpi_કિંમત*vp)
{
      timevar_get_કિંમત(ref, vp, સાચું, સાચું);
}

સ્થિર શૂન્ય timevar_get_rvalue(vpiHandle ref, s_vpi_કિંમત*vp)
{
      timevar_get_કિંમત(ref, vp, ખોટું, ખોટું);
}

__vpiScopedTime::__vpiScopedTime()
{ }

અક્ષર* __vpiScopedTime::vpi_get_str(પૂર્ણાંક code)
{
      પસંદગી (code) {
	  સ્થિતિ vpiName:
	    પાછા simple_set_rbuf_str("$time");
	  મૂળભૂત:
	    fprintf(stderr, "Code: %d\n", code);
	    પ્રતિજ્ઞા(0);
	    પાછા 0;
      }
}


શૂન્ય __vpiScopedTime::vpi_get_કિંમત(p_vpi_કિંમત val)
{ timevar_get_ivalue(આ, val); }


__vpiScopedSTime::__vpiScopedSTime()
{ }

પૂર્ણાંક __vpiScopedSTime::vpi_get(પૂર્ણાંક code)
{
      પસંદગી (code) {
          સ્થિતિ vpiSize:
	    પાછા 32;

	  મૂળભૂત:
	    પાછા __vpiSystemTime::vpi_get(code);
      }
}


અક્ષર* __vpiScopedSTime::vpi_get_str(પૂર્ણાંક code)
{
      પસંદગી (code) {
	  સ્થિતિ vpiName:
	    પાછા simple_set_rbuf_str("$stime");
	  મૂળભૂત:
	    fprintf(stderr, "Code: %d\n", code);
	    પ્રતિજ્ઞા(0);
	    પાછા 0;
      }
}


શૂન્ય __vpiScopedSTime::vpi_get_કિંમત(p_vpi_કિંમત val)
{ timevar_get_svalue(આ, val); }

__vpiSystemTime::__vpiSystemTime()
{
      scope = 0;
}

પૂર્ણાંક __vpiSystemTime::get_type_code(શૂન્ય) અચલ
{ પાછા vpiSysFuncCall; }

પૂર્ણાંક __vpiSystemTime::vpi_get(પૂર્ણાંક code)
{
      પસંદગી (code) {
          સ્થિતિ vpiSize:
	    પાછા 64;

          સ્થિતિ vpiSigned:
	    પાછા 0;

	  સ્થિતિ vpiFuncType:
	    પાછા vpiTimeFunc;

          સ્થિતિ vpiAutomatic:
	    પાછા 0;

	  મૂળભૂત:
	    fprintf(stderr, "Code: %d\n", code);
	    પ્રતિજ્ઞા(0);
	    પાછા 0;
      }
}


અક્ષર* __vpiSystemTime::vpi_get_str(પૂર્ણાંક code)
{
      પસંદગી (code) {
	  સ્થિતિ vpiName:
	    પાછા simple_set_rbuf_str("$simtime");
	  મૂળભૂત:
	    fprintf(stderr, "Code: %d\n", code);
	    પ્રતિજ્ઞા(0);
	    પાછા 0;
      }
}


શૂન્ય __vpiSystemTime::vpi_get_કિંમત(p_vpi_કિંમત val)
{ timevar_get_ivalue(આ, val); }

vpiHandle __vpiSystemTime::vpi_handle(પૂર્ણાંક code)
{
      પસંદગી (code) {
	  સ્થિતિ vpiScope:
	    પાછા scope;
	  મૂળભૂત:
	    પાછા 0;
      }
}


__vpiScopedRealtime::__vpiScopedRealtime()
{ }

પૂર્ણાંક __vpiScopedRealtime::vpi_get(પૂર્ણાંક code)
{
      પસંદગી (code) {
          સ્થિતિ vpiSize:
	    પાછા 1;

          સ્થિતિ vpiSigned:
	    પાછા 0;

	  સ્થિતિ vpiFuncType:
	    પાછા vpiRealFunc;

          સ્થિતિ vpiAutomatic:
	    પાછા 0;

	  મૂળભૂત:
	    fprintf(stderr, "Code: %d\n", code);
	    પ્રતિજ્ઞા(0);
	    પાછા 0;
      }
}


અક્ષર* __vpiScopedRealtime::vpi_get_str(પૂર્ણાંક code)
{
      પસંદગી (code) {
	  સ્થિતિ vpiName:
	    પાછા simple_set_rbuf_str("$realtime");
	  મૂળભૂત:
	    fprintf(stderr, "Code: %d\n", code);
	    પ્રતિજ્ઞા(0);
	    પાછા 0;
      }
}


શૂન્ય __vpiScopedRealtime::vpi_get_કિંમત(p_vpi_કિંમત val)
{ timevar_get_rvalue(આ, val); }

/*
 * Create a handle to represent a call to $time/$stime/$simtime. The
 * $time and $stime system functions return a value scaled to a scope,
 * and the $simtime returns the unscaled time.
 */
vpiHandle vpip_sim_time(__vpiScope*scope, તાર્કિક is_stime)
{
      જો (scope) {
	    જો (is_stime) {
		  scope->scoped_stime.scope = scope;
		  પાછા &scope->scoped_stime;
	    } નહીંતર {
		  scope->scoped_time.scope = scope;
		  પાછા &scope->scoped_time;
	    }
      } નહીંતર {
	    પાછા &global_simtime;
      }
}

vpiHandle vpip_sim_realtime(__vpiScope*scope)
{
      scope->scoped_realtime.scope = scope;
      પાછા &scope->scoped_realtime;
}

પૂર્ણાંક vpip_get_time_precision(શૂન્ય)
{
      પાછા vpi_time_precision;
}

શૂન્ય vpip_set_time_precision(પૂર્ણાંક pre)
{
      vpi_time_precision = pre;
}
