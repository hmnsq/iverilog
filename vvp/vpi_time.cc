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
स्थिर पूर्णांक vpi_time_precision = 0;

स्थिर संरचना __vpiSystemTime global_simtime;

शून्य vpip_time_to_timestruct(संरचना t_vpi_time*ts, vvp_time64_t ti)
{
      ts->खालचा  = ti & 0xFFFFFFFF;
      ts->वरचा = (ti >> 32) & 0xFFFFFFFF;
}

vvp_time64_t vpip_timestruct_to_time(स्थिरांक संरचना t_vpi_time*ts)
{
      vvp_time64_t ti = ts->वरचा;
      ti <<= 32;
      ti += ts->खालचा & 0xffffffff;
      परत ti;
}

द्विगुण vpip_time_to_scaled_real(vvp_time64_t ti, स्थिरांक __vpiScope*scope)
{
      द्विगुण val;
      पूर्णांक scale = 0;
      जर (scope) scale = vpi_time_precision - scope->time_units;

      जर (scale >= 0) val = (द्विगुण)ti * घात(10.0, scale);
      अन्यथा val = (द्विगुण)ti / घात(10.0, -scale);

      परत val;
}

/*
 * This routine does not currently support negative real delays and it
 * does not check for overflow. It is only used for modpath delays and
 * they are required to be non-negative.
 */
vvp_time64_t vpip_scaled_real_to_time64(द्विगुण val, स्थिरांक __vpiScope*scope)
{
      पूर्णांक shift = 0;
      जर (scope) shift = scope->time_units - scope->time_precision;
      प्रतिज्ञा(shift >= 0);

      प्रतिज्ञा(val >= 0);

	// Scale to the local precision and then round away from zero.
      द्विगुण scale = घात(10.0L, shift);
      val *= scale;

      vvp_time64_t delay = (vvp_time64_t) (val + 0.5);

	// If needed now scale the value to the simulator precision.
      जर (scope) {
	    shift = scope->time_precision - vpi_time_precision;
	    प्रतिज्ञा(shift >= 0);
	    साठी (पूर्णांक lp = 0; lp <  shift; lp += 1) delay *= 10;
      }

      परत delay;
}

स्थिर शून्य timevar_get_मूल्य(vpiHandle ref, s_vpi_मूल्य*vp, तार्किक is_int_func,
                              तार्किक is_stime)
{
	/* Keep a persistent structure for passing time values back to
	   the caller. */
      स्थिर संरचना t_vpi_time time_मूल्य;

      संरचना __vpiSystemTime*rfp = dynamic_cast<__vpiSystemTime*>(ref);
      अचिन्हित दीर्घ अंक_bits;
      vvp_time64_t x, simtime = schedule_simtime();
      पूर्णांक units = rfp->scope? rfp->scope->time_units : vpi_time_precision;

      स्थिर स्थिरांक माप_प्रकार RBUF_USE_SIZE = 128;
      अक्षर*rbuf = static_cast<अक्षर *>(need_result_buf(RBUF_USE_SIZE, RBUF_VAL));

	/* Calculate the divisor needed to scale the simulation time
	   (in time_precision units) to time units of the scope. */
      vvp_time64_t भाजक = 1;
      पर्यंत (units > vpi_time_precision) {
	    भाजक *= 10;
	    units -= 1;
      }

	/* Scale the simtime, and use the modulus to round up if
	   appropriate. */
      vvp_time64_t simtime_fraction = simtime % भाजक;
      simtime /= भाजक;

      जर ((भाजक >= 10) && (simtime_fraction >= (भाजक/2)))
	    simtime += 1;

	/* If this is a call to $stime only return the lower 32 bits. */
      जर (is_stime) simtime &= 0xffffffff;

      निवड (vp->format) {
	  स्थिती vpiObjTypeVal:
	      /* The default format is vpiTimeVal. */
	    vp->format = vpiTimeVal;
	    // fallthrough
	  स्थिती vpiTimeVal:
	    vp->value.समय = &time_मूल्य;
	    vp->value.समय->type = vpiSimTime;
	    vpip_time_to_timestruct(vp->value.समय, simtime);
	    थांबा;

	  स्थिती vpiRealVal:
	      /* If this is an integer based call (anything but $realtime)
	       * just return the value as a double. */
	    जर (is_int_func)  vp->value.real = द्विगुण (simtime);
	      /* This is a call to $realtime to return a real value so
	       * scale this using the scaled real rules. */
	    अन्यथा vp->value.real = vpip_time_to_scaled_real(schedule_simtime(),
						           rfp->scope);
	    थांबा;

	  स्थिती vpiBinStrVal:
	    x = simtime;
	    अंक_bits = 8 * आकार(vvp_time64_t);

	    rbuf[अंक_bits] = 0;
	    साठी (अचिन्हित i = 1; i <= अंक_bits; i++) {
	      rbuf[अंक_bits-i] = (x  & 1) ? '1' : '0';
	      x = x >> 1;
	    }

	    vp->value.str = rbuf;
	    थांबा;

	  स्थिती vpiDecStrVal:
	    snprintf(rbuf, RBUF_USE_SIZE, "%" TIME_FMT_U, simtime);
	    vp->value.str = rbuf;
	    थांबा;

	  स्थिती vpiOctStrVal:
	    snprintf(rbuf, RBUF_USE_SIZE, "%" TIME_FMT_O, simtime);
	    vp->value.str = rbuf;
	    थांबा;

	  स्थिती vpiHexStrVal:
	    snprintf(rbuf, RBUF_USE_SIZE, "%" TIME_FMT_X, simtime);
	    vp->value.str = rbuf;
	    थांबा;

	  पूर्वनिर्धारित:
	    ख_लिखो(मानक_त्रुटि, "vpi_time: unknown format: %d\n", (पूर्णांक)vp->format);
	    प्रतिज्ञा(0);
      }
}

स्थिर शून्य timevar_get_ivalue(vpiHandle ref, s_vpi_मूल्य*vp)
{
      timevar_get_मूल्य(ref, vp, सत्य, असत्य);
}

स्थिर शून्य timevar_get_svalue(vpiHandle ref, s_vpi_मूल्य*vp)
{
      timevar_get_मूल्य(ref, vp, सत्य, सत्य);
}

स्थिर शून्य timevar_get_rvalue(vpiHandle ref, s_vpi_मूल्य*vp)
{
      timevar_get_मूल्य(ref, vp, असत्य, असत्य);
}

__vpiScopedTime::__vpiScopedTime()
{ }

अक्षर* __vpiScopedTime::vpi_get_str(पूर्णांक code)
{
      निवड (code) {
	  स्थिती vpiName:
	    परत simple_set_rbuf_str("$time");
	  पूर्वनिर्धारित:
	    ख_लिखो(मानक_त्रुटि, "Code: %d\n", code);
	    प्रतिज्ञा(0);
	    परत 0;
      }
}


शून्य __vpiScopedTime::vpi_get_मूल्य(p_vpi_मूल्य val)
{ timevar_get_ivalue(हे, val); }


__vpiScopedSTime::__vpiScopedSTime()
{ }

पूर्णांक __vpiScopedSTime::vpi_get(पूर्णांक code)
{
      निवड (code) {
          स्थिती vpiSize:
	    परत 32;

	  पूर्वनिर्धारित:
	    परत __vpiSystemTime::vpi_get(code);
      }
}


अक्षर* __vpiScopedSTime::vpi_get_str(पूर्णांक code)
{
      निवड (code) {
	  स्थिती vpiName:
	    परत simple_set_rbuf_str("$stime");
	  पूर्वनिर्धारित:
	    ख_लिखो(मानक_त्रुटि, "Code: %d\n", code);
	    प्रतिज्ञा(0);
	    परत 0;
      }
}


शून्य __vpiScopedSTime::vpi_get_मूल्य(p_vpi_मूल्य val)
{ timevar_get_svalue(हे, val); }

__vpiSystemTime::__vpiSystemTime()
{
      scope = 0;
}

पूर्णांक __vpiSystemTime::get_type_code(शून्य) स्थिरांक
{ परत vpiSysFuncCall; }

पूर्णांक __vpiSystemTime::vpi_get(पूर्णांक code)
{
      निवड (code) {
          स्थिती vpiSize:
	    परत 64;

          स्थिती vpiSigned:
	    परत 0;

	  स्थिती vpiFuncType:
	    परत vpiTimeFunc;

          स्थिती vpiAutomatic:
	    परत 0;

	  पूर्वनिर्धारित:
	    ख_लिखो(मानक_त्रुटि, "Code: %d\n", code);
	    प्रतिज्ञा(0);
	    परत 0;
      }
}


अक्षर* __vpiSystemTime::vpi_get_str(पूर्णांक code)
{
      निवड (code) {
	  स्थिती vpiName:
	    परत simple_set_rbuf_str("$simtime");
	  पूर्वनिर्धारित:
	    ख_लिखो(मानक_त्रुटि, "Code: %d\n", code);
	    प्रतिज्ञा(0);
	    परत 0;
      }
}


शून्य __vpiSystemTime::vpi_get_मूल्य(p_vpi_मूल्य val)
{ timevar_get_ivalue(हे, val); }

vpiHandle __vpiSystemTime::vpi_handle(पूर्णांक code)
{
      निवड (code) {
	  स्थिती vpiScope:
	    परत scope;
	  पूर्वनिर्धारित:
	    परत 0;
      }
}


__vpiScopedRealtime::__vpiScopedRealtime()
{ }

पूर्णांक __vpiScopedRealtime::vpi_get(पूर्णांक code)
{
      निवड (code) {
          स्थिती vpiSize:
	    परत 1;

          स्थिती vpiSigned:
	    परत 0;

	  स्थिती vpiFuncType:
	    परत vpiRealFunc;

          स्थिती vpiAutomatic:
	    परत 0;

	  पूर्वनिर्धारित:
	    ख_लिखो(मानक_त्रुटि, "Code: %d\n", code);
	    प्रतिज्ञा(0);
	    परत 0;
      }
}


अक्षर* __vpiScopedRealtime::vpi_get_str(पूर्णांक code)
{
      निवड (code) {
	  स्थिती vpiName:
	    परत simple_set_rbuf_str("$realtime");
	  पूर्वनिर्धारित:
	    ख_लिखो(मानक_त्रुटि, "Code: %d\n", code);
	    प्रतिज्ञा(0);
	    परत 0;
      }
}


शून्य __vpiScopedRealtime::vpi_get_मूल्य(p_vpi_मूल्य val)
{ timevar_get_rvalue(हे, val); }

/*
 * Create a handle to represent a call to $time/$stime/$simtime. The
 * $time and $stime system functions return a value scaled to a scope,
 * and the $simtime returns the unscaled time.
 */
vpiHandle vpip_sim_time(__vpiScope*scope, तार्किक is_stime)
{
      जर (scope) {
	    जर (is_stime) {
		  scope->scoped_stime.scope = scope;
		  परत &scope->scoped_stime;
	    } अन्यथा {
		  scope->scoped_time.scope = scope;
		  परत &scope->scoped_time;
	    }
      } अन्यथा {
	    परत &global_simtime;
      }
}

vpiHandle vpip_sim_realtime(__vpiScope*scope)
{
      scope->scoped_realtime.scope = scope;
      परत &scope->scoped_realtime;
}

पूर्णांक vpip_get_time_precision(शून्य)
{
      परत vpi_time_precision;
}

शून्य vpip_set_time_precision(पूर्णांक pre)
{
      vpi_time_precision = pre;
}
