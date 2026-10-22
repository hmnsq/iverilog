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
स्थिर पूर्णाङ्क vpi_time_precision = 0;

स्थिर संरचना __vpiSystemTime global_simtime;

शून्य vpip_time_to_timestruct(संरचना t_vpi_time*ts, vvp_time64_t ti)
{
      ts->खालचा  = ti & 0xFFFFFFFF;
      ts->वरचा = (ti >> 32) & 0xFFFFFFFF;
}

vvp_time64_t vpip_timestruct_to_time(ध्रुव संरचना t_vpi_time*ts)
{
      vvp_time64_t ti = ts->वरचा;
      ti <<= 32;
      ti += ts->खालचा & 0xffffffff;
      प्रत्यागम ti;
}

द्विगुण vpip_time_to_scaled_real(vvp_time64_t ti, ध्रुव __vpiScope*scope)
{
      द्विगुण val;
      पूर्णाङ्क scale = 0;
      यदि (scope) scale = vpi_time_precision - scope->time_units;

      यदि (scale >= 0) val = (द्विगुण)ti * pow(10.0, scale);
      अन्यथा val = (द्विगुण)ti / pow(10.0, -scale);

      प्रत्यागम val;
}

/*
 * This routine does not currently support negative real delays and it
 * does not check for overflow. It is only used for modpath delays and
 * they are required to be non-negative.
 */
vvp_time64_t vpip_scaled_real_to_time64(द्विगुण val, ध्रुव __vpiScope*scope)
{
      पूर्णाङ्क shift = 0;
      यदि (scope) shift = scope->time_units - scope->time_precision;
      प्रतिज्ञा(shift >= 0);

      प्रतिज्ञा(val >= 0);

	// Scale to the local precision and then round away from zero.
      द्विगुण scale = pow(10.0L, shift);
      val *= scale;

      vvp_time64_t delay = (vvp_time64_t) (val + 0.5);

	// If needed now scale the value to the simulator precision.
      यदि (scope) {
	    shift = scope->time_precision - vpi_time_precision;
	    प्रतिज्ञा(shift >= 0);
	    प्रति (पूर्णाङ्क lp = 0; lp <  shift; lp += 1) delay *= 10;
      }

      प्रत्यागम delay;
}

स्थिर शून्य timevar_get_मूल्य(vpiHandle ref, s_vpi_मूल्य*vp, तार्किक is_int_func,
                              तार्किक is_stime)
{
	/* Keep a persistent structure for passing time values back to
	   the caller. */
      स्थिर संरचना t_vpi_time time_मूल्य;

      संरचना __vpiSystemTime*rfp = dynamic_cast<__vpiSystemTime*>(ref);
      unsigned long अंक_bits;
      vvp_time64_t x, simtime = schedule_simtime();
      पूर्णाङ्क units = rfp->scope? rfp->scope->time_units : vpi_time_precision;

      स्थिर ध्रुव size_t RBUF_USE_SIZE = 128;
      वर्ण*rbuf = static_cast<वर्ण *>(need_result_buf(RBUF_USE_SIZE, RBUF_VAL));

	/* Calculate the divisor needed to scale the simulation time
	   (in time_precision units) to time units of the scope. */
      vvp_time64_t भाजक = 1;
      यावत् (units > vpi_time_precision) {
	    भाजक *= 10;
	    units -= 1;
      }

	/* Scale the simtime, and use the modulus to round up if
	   appropriate. */
      vvp_time64_t simtime_fraction = simtime % भाजक;
      simtime /= भाजक;

      यदि ((भाजक >= 10) && (simtime_fraction >= (भाजक/2)))
	    simtime += 1;

	/* If this is a call to $stime only return the lower 32 bits. */
      यदि (is_stime) simtime &= 0xffffffff;

      विकल्प (vp->format) {
	  पक्ष vpiObjTypeVal:
	      /* The default format is vpiTimeVal. */
	    vp->format = vpiTimeVal;
	    // fallthrough
	  पक्ष vpiTimeVal:
	    vp->value.time = &time_मूल्य;
	    vp->value.time->type = vpiSimTime;
	    vpip_time_to_timestruct(vp->value.time, simtime);
	    भङ्ग;

	  पक्ष vpiRealVal:
	      /* If this is an integer based call (anything but $realtime)
	       * just return the value as a double. */
	    यदि (is_int_func)  vp->value.real = द्विगुण (simtime);
	      /* This is a call to $realtime to return a real value so
	       * scale this using the scaled real rules. */
	    अन्यथा vp->value.real = vpip_time_to_scaled_real(schedule_simtime(),
						           rfp->scope);
	    भङ्ग;

	  पक्ष vpiBinStrVal:
	    x = simtime;
	    अंक_bits = 8 * परिमाण(vvp_time64_t);

	    rbuf[अंक_bits] = 0;
	    प्रति (unsigned i = 1; i <= अंक_bits; i++) {
	      rbuf[अंक_bits-i] = (x  & 1) ? '1' : '0';
	      x = x >> 1;
	    }

	    vp->value.str = rbuf;
	    भङ्ग;

	  पक्ष vpiDecStrVal:
	    snprintf(rbuf, RBUF_USE_SIZE, "%" TIME_FMT_U, simtime);
	    vp->value.str = rbuf;
	    भङ्ग;

	  पक्ष vpiOctStrVal:
	    snprintf(rbuf, RBUF_USE_SIZE, "%" TIME_FMT_O, simtime);
	    vp->value.str = rbuf;
	    भङ्ग;

	  पक्ष vpiHexStrVal:
	    snprintf(rbuf, RBUF_USE_SIZE, "%" TIME_FMT_X, simtime);
	    vp->value.str = rbuf;
	    भङ्ग;

	  स्वभाव:
	    fprintf(stderr, "vpi_time: unknown format: %d\n", (पूर्णाङ्क)vp->format);
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

वर्ण* __vpiScopedTime::vpi_get_str(पूर्णाङ्क code)
{
      विकल्प (code) {
	  पक्ष vpiName:
	    प्रत्यागम simple_set_rbuf_str("$time");
	  स्वभाव:
	    fprintf(stderr, "Code: %d\n", code);
	    प्रतिज्ञा(0);
	    प्रत्यागम 0;
      }
}


शून्य __vpiScopedTime::vpi_get_मूल्य(p_vpi_मूल्य val)
{ timevar_get_ivalue(एतत्, val); }


__vpiScopedSTime::__vpiScopedSTime()
{ }

पूर्णाङ्क __vpiScopedSTime::vpi_get(पूर्णाङ्क code)
{
      विकल्प (code) {
          पक्ष vpiSize:
	    प्रत्यागम 32;

	  स्वभाव:
	    प्रत्यागम __vpiSystemTime::vpi_get(code);
      }
}


वर्ण* __vpiScopedSTime::vpi_get_str(पूर्णाङ्क code)
{
      विकल्प (code) {
	  पक्ष vpiName:
	    प्रत्यागम simple_set_rbuf_str("$stime");
	  स्वभाव:
	    fprintf(stderr, "Code: %d\n", code);
	    प्रतिज्ञा(0);
	    प्रत्यागम 0;
      }
}


शून्य __vpiScopedSTime::vpi_get_मूल्य(p_vpi_मूल्य val)
{ timevar_get_svalue(एतत्, val); }

__vpiSystemTime::__vpiSystemTime()
{
      scope = 0;
}

पूर्णाङ्क __vpiSystemTime::get_type_code(शून्य) ध्रुव
{ प्रत्यागम vpiSysFuncCall; }

पूर्णाङ्क __vpiSystemTime::vpi_get(पूर्णाङ्क code)
{
      विकल्प (code) {
          पक्ष vpiSize:
	    प्रत्यागम 64;

          पक्ष vpiSigned:
	    प्रत्यागम 0;

	  पक्ष vpiFuncType:
	    प्रत्यागम vpiTimeFunc;

          पक्ष vpiAutomatic:
	    प्रत्यागम 0;

	  स्वभाव:
	    fprintf(stderr, "Code: %d\n", code);
	    प्रतिज्ञा(0);
	    प्रत्यागम 0;
      }
}


वर्ण* __vpiSystemTime::vpi_get_str(पूर्णाङ्क code)
{
      विकल्प (code) {
	  पक्ष vpiName:
	    प्रत्यागम simple_set_rbuf_str("$simtime");
	  स्वभाव:
	    fprintf(stderr, "Code: %d\n", code);
	    प्रतिज्ञा(0);
	    प्रत्यागम 0;
      }
}


शून्य __vpiSystemTime::vpi_get_मूल्य(p_vpi_मूल्य val)
{ timevar_get_ivalue(एतत्, val); }

vpiHandle __vpiSystemTime::vpi_handle(पूर्णाङ्क code)
{
      विकल्प (code) {
	  पक्ष vpiScope:
	    प्रत्यागम scope;
	  स्वभाव:
	    प्रत्यागम 0;
      }
}


__vpiScopedRealtime::__vpiScopedRealtime()
{ }

पूर्णाङ्क __vpiScopedRealtime::vpi_get(पूर्णाङ्क code)
{
      विकल्प (code) {
          पक्ष vpiSize:
	    प्रत्यागम 1;

          पक्ष vpiSigned:
	    प्रत्यागम 0;

	  पक्ष vpiFuncType:
	    प्रत्यागम vpiRealFunc;

          पक्ष vpiAutomatic:
	    प्रत्यागम 0;

	  स्वभाव:
	    fprintf(stderr, "Code: %d\n", code);
	    प्रतिज्ञा(0);
	    प्रत्यागम 0;
      }
}


वर्ण* __vpiScopedRealtime::vpi_get_str(पूर्णाङ्क code)
{
      विकल्प (code) {
	  पक्ष vpiName:
	    प्रत्यागम simple_set_rbuf_str("$realtime");
	  स्वभाव:
	    fprintf(stderr, "Code: %d\n", code);
	    प्रतिज्ञा(0);
	    प्रत्यागम 0;
      }
}


शून्य __vpiScopedRealtime::vpi_get_मूल्य(p_vpi_मूल्य val)
{ timevar_get_rvalue(एतत्, val); }

/*
 * Create a handle to represent a call to $time/$stime/$simtime. The
 * $time and $stime system functions return a value scaled to a scope,
 * and the $simtime returns the unscaled time.
 */
vpiHandle vpip_sim_time(__vpiScope*scope, तार्किक is_stime)
{
      यदि (scope) {
	    यदि (is_stime) {
		  scope->scoped_stime.scope = scope;
		  प्रत्यागम &scope->scoped_stime;
	    } अन्यथा {
		  scope->scoped_time.scope = scope;
		  प्रत्यागम &scope->scoped_time;
	    }
      } अन्यथा {
	    प्रत्यागम &global_simtime;
      }
}

vpiHandle vpip_sim_realtime(__vpiScope*scope)
{
      scope->scoped_realtime.scope = scope;
      प्रत्यागम &scope->scoped_realtime;
}

पूर्णाङ्क vpip_get_time_precision(शून्य)
{
      प्रत्यागम vpi_time_precision;
}

शून्य vpip_set_time_precision(पूर्णाङ्क pre)
{
      vpi_time_precision = pre;
}
