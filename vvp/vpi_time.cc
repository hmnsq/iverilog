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
      ts->तल्लो  = ti & 0xFFFFFFFF;
      ts->माथिल्लो = (ti >> 32) & 0xFFFFFFFF;
}

vvp_time64_t vpip_timestruct_to_time(अचर संरचना t_vpi_time*ts)
{
      vvp_time64_t ti = ts->माथिल्लो;
      ti <<= 32;
      ti += ts->तल्लो & 0xffffffff;
      फर्काऊ ti;
}

द्विगुण vpip_time_to_scaled_real(vvp_time64_t ti, अचर __vpiScope*scope)
{
      द्विगुण val;
      पूर्णाङ्क scale = 0;
      यदि (scope) scale = vpi_time_precision - scope->time_units;

      यदि (scale >= 0) val = (द्विगुण)ti * घात(10.0, scale);
      अन्यथा val = (द्विगुण)ti / घात(10.0, -scale);

      फर्काऊ val;
}

/*
 * This routine does not currently support negative real delays and it
 * does not check for overflow. It is only used for modpath delays and
 * they are required to be non-negative.
 */
vvp_time64_t vpip_scaled_real_to_time64(द्विगुण val, अचर __vpiScope*scope)
{
      पूर्णाङ्क shift = 0;
      यदि (scope) shift = scope->time_units - scope->time_precision;
      प्रतिज्ञा(shift >= 0);

      प्रतिज्ञा(val >= 0);

	// Scale to the local precision and then round away from zero.
      द्विगुण scale = घात(10.0L, shift);
      val *= scale;

      vvp_time64_t delay = (vvp_time64_t) (val + 0.5);

	// If needed now scale the value to the simulator precision.
      यदि (scope) {
	    shift = scope->time_precision - vpi_time_precision;
	    प्रतिज्ञा(shift >= 0);
	    लागि (पूर्णाङ्क lp = 0; lp <  shift; lp += 1) delay *= 10;
      }

      फर्काऊ delay;
}

स्थिर शून्य timevar_get_मान(vpiHandle ref, s_vpi_मान*vp, तार्किक is_int_func,
                              तार्किक is_stime)
{
	/* Keep a persistent structure for passing time values back to
	   the caller. */
      स्थिर संरचना t_vpi_time time_मान;

      संरचना __vpiSystemTime*rfp = dynamic_cast<__vpiSystemTime*>(ref);
      अचिन्हित दीर्घ अङ्क_bits;
      vvp_time64_t x, simtime = schedule_simtime();
      पूर्णाङ्क units = rfp->scope? rfp->scope->time_units : vpi_time_precision;

      स्थिर अचर माप_प्रकार RBUF_USE_SIZE = 128;
      वर्ण*rbuf = static_cast<वर्ण *>(need_result_buf(RBUF_USE_SIZE, RBUF_VAL));

	/* Calculate the divisor needed to scale the simulation time
	   (in time_precision units) to time units of the scope. */
      vvp_time64_t भाजक = 1;
      जबसम्म (units > vpi_time_precision) {
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
	    vp->value.समय = &time_मान;
	    vp->value.समय->type = vpiSimTime;
	    vpip_time_to_timestruct(vp->value.समय, simtime);
	    रोक;

	  पक्ष vpiRealVal:
	      /* If this is an integer based call (anything but $realtime)
	       * just return the value as a double. */
	    यदि (is_int_func)  vp->value.real = द्विगुण (simtime);
	      /* This is a call to $realtime to return a real value so
	       * scale this using the scaled real rules. */
	    अन्यथा vp->value.real = vpip_time_to_scaled_real(schedule_simtime(),
						           rfp->scope);
	    रोक;

	  पक्ष vpiBinStrVal:
	    x = simtime;
	    अङ्क_bits = 8 * परिमाण(vvp_time64_t);

	    rbuf[अङ्क_bits] = 0;
	    लागि (अचिन्हित i = 1; i <= अङ्क_bits; i++) {
	      rbuf[अङ्क_bits-i] = (x  & 1) ? '1' : '0';
	      x = x >> 1;
	    }

	    vp->value.str = rbuf;
	    रोक;

	  पक्ष vpiDecStrVal:
	    snprintf(rbuf, RBUF_USE_SIZE, "%" TIME_FMT_U, simtime);
	    vp->value.str = rbuf;
	    रोक;

	  पक्ष vpiOctStrVal:
	    snprintf(rbuf, RBUF_USE_SIZE, "%" TIME_FMT_O, simtime);
	    vp->value.str = rbuf;
	    रोक;

	  पक्ष vpiHexStrVal:
	    snprintf(rbuf, RBUF_USE_SIZE, "%" TIME_FMT_X, simtime);
	    vp->value.str = rbuf;
	    रोक;

	  स्वभाव:
	    ख_लिखो(मानक_त्रुटि, "vpi_time: unknown format: %d\n", (पूर्णाङ्क)vp->format);
	    प्रतिज्ञा(0);
      }
}

स्थिर शून्य timevar_get_ivalue(vpiHandle ref, s_vpi_मान*vp)
{
      timevar_get_मान(ref, vp, सत्य, असत्य);
}

स्थिर शून्य timevar_get_svalue(vpiHandle ref, s_vpi_मान*vp)
{
      timevar_get_मान(ref, vp, सत्य, सत्य);
}

स्थिर शून्य timevar_get_rvalue(vpiHandle ref, s_vpi_मान*vp)
{
      timevar_get_मान(ref, vp, असत्य, असत्य);
}

__vpiScopedTime::__vpiScopedTime()
{ }

वर्ण* __vpiScopedTime::vpi_get_str(पूर्णाङ्क code)
{
      विकल्प (code) {
	  पक्ष vpiName:
	    फर्काऊ simple_set_rbuf_str("$time");
	  स्वभाव:
	    ख_लिखो(मानक_त्रुटि, "Code: %d\n", code);
	    प्रतिज्ञा(0);
	    फर्काऊ 0;
      }
}


शून्य __vpiScopedTime::vpi_get_मान(p_vpi_मान val)
{ timevar_get_ivalue(एतत्, val); }


__vpiScopedSTime::__vpiScopedSTime()
{ }

पूर्णाङ्क __vpiScopedSTime::vpi_get(पूर्णाङ्क code)
{
      विकल्प (code) {
          पक्ष vpiSize:
	    फर्काऊ 32;

	  स्वभाव:
	    फर्काऊ __vpiSystemTime::vpi_get(code);
      }
}


वर्ण* __vpiScopedSTime::vpi_get_str(पूर्णाङ्क code)
{
      विकल्प (code) {
	  पक्ष vpiName:
	    फर्काऊ simple_set_rbuf_str("$stime");
	  स्वभाव:
	    ख_लिखो(मानक_त्रुटि, "Code: %d\n", code);
	    प्रतिज्ञा(0);
	    फर्काऊ 0;
      }
}


शून्य __vpiScopedSTime::vpi_get_मान(p_vpi_मान val)
{ timevar_get_svalue(एतत्, val); }

__vpiSystemTime::__vpiSystemTime()
{
      scope = 0;
}

पूर्णाङ्क __vpiSystemTime::get_type_code(शून्य) अचर
{ फर्काऊ vpiSysFuncCall; }

पूर्णाङ्क __vpiSystemTime::vpi_get(पूर्णाङ्क code)
{
      विकल्प (code) {
          पक्ष vpiSize:
	    फर्काऊ 64;

          पक्ष vpiSigned:
	    फर्काऊ 0;

	  पक्ष vpiFuncType:
	    फर्काऊ vpiTimeFunc;

          पक्ष vpiAutomatic:
	    फर्काऊ 0;

	  स्वभाव:
	    ख_लिखो(मानक_त्रुटि, "Code: %d\n", code);
	    प्रतिज्ञा(0);
	    फर्काऊ 0;
      }
}


वर्ण* __vpiSystemTime::vpi_get_str(पूर्णाङ्क code)
{
      विकल्प (code) {
	  पक्ष vpiName:
	    फर्काऊ simple_set_rbuf_str("$simtime");
	  स्वभाव:
	    ख_लिखो(मानक_त्रुटि, "Code: %d\n", code);
	    प्रतिज्ञा(0);
	    फर्काऊ 0;
      }
}


शून्य __vpiSystemTime::vpi_get_मान(p_vpi_मान val)
{ timevar_get_ivalue(एतत्, val); }

vpiHandle __vpiSystemTime::vpi_handle(पूर्णाङ्क code)
{
      विकल्प (code) {
	  पक्ष vpiScope:
	    फर्काऊ scope;
	  स्वभाव:
	    फर्काऊ 0;
      }
}


__vpiScopedRealtime::__vpiScopedRealtime()
{ }

पूर्णाङ्क __vpiScopedRealtime::vpi_get(पूर्णाङ्क code)
{
      विकल्प (code) {
          पक्ष vpiSize:
	    फर्काऊ 1;

          पक्ष vpiSigned:
	    फर्काऊ 0;

	  पक्ष vpiFuncType:
	    फर्काऊ vpiRealFunc;

          पक्ष vpiAutomatic:
	    फर्काऊ 0;

	  स्वभाव:
	    ख_लिखो(मानक_त्रुटि, "Code: %d\n", code);
	    प्रतिज्ञा(0);
	    फर्काऊ 0;
      }
}


वर्ण* __vpiScopedRealtime::vpi_get_str(पूर्णाङ्क code)
{
      विकल्प (code) {
	  पक्ष vpiName:
	    फर्काऊ simple_set_rbuf_str("$realtime");
	  स्वभाव:
	    ख_लिखो(मानक_त्रुटि, "Code: %d\n", code);
	    प्रतिज्ञा(0);
	    फर्काऊ 0;
      }
}


शून्य __vpiScopedRealtime::vpi_get_मान(p_vpi_मान val)
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
		  फर्काऊ &scope->scoped_stime;
	    } अन्यथा {
		  scope->scoped_time.scope = scope;
		  फर्काऊ &scope->scoped_time;
	    }
      } अन्यथा {
	    फर्काऊ &global_simtime;
      }
}

vpiHandle vpip_sim_realtime(__vpiScope*scope)
{
      scope->scoped_realtime.scope = scope;
      फर्काऊ &scope->scoped_realtime;
}

पूर्णाङ्क vpip_get_time_precision(शून्य)
{
      फर्काऊ vpi_time_precision;
}

शून्य vpip_set_time_precision(पूर्णाङ्क pre)
{
      vpi_time_precision = pre;
}
