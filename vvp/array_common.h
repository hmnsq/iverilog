/*
 * Copyright (c) 2014-2026 Stephen Williams (steve@icarus.com)
 * Copyright (c) 2014 CERN
 * @author Maciej Suminski <maciej.suminski@cern.ch>
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

#నిర్వచితం_కాకపోతే ARRAY_COMMON_H
#నిర్వచించు ARRAY_COMMON_H

#అన్తర్భావ "vpi_priv.h"

నిర్మాణం __vpiArrayIterator : బహిరంగ __vpiHandle {
      పూర్ణాంకం get_type_సంకేతం(శూన్య) స్థిరరాశి override
      { తిరిగి vpiIterator; }

      vpiHandle vpi_సూచీ(పూర్ణాంకం idx) override;
      free_object_fun_t free_object_fun(శూన్య) override;

      నిర్మాణం __vpiArrayBase*array;
      గుర్తులేని తదుపరి_2;
};

నిర్మాణం __vpiArrayIndex : బహిరంగ __vpiHandle {
      పూర్ణాంకం get_type_సంకేతం(శూన్య) స్థిరరాశి override
      { తిరిగి vpiIterator; }

      vpiHandle vpi_iterate(పూర్ణాంకం సంకేతం_2) override;
      vpiHandle vpi_సూచీ(పూర్ణాంకం idx) override;
      free_object_fun_t free_object_fun(శూన్య) override;

      __vpiDecConst *సూచీ;
      గుర్తులేని పూర్తి;
};

/*
 * The vpiArrayWord is magic. It is used as the handle to return when
 * vpi code tries to index or scan an array of variable words. The
 * array word handle contains no actual data. It is just a hook for
 * the vpi methods and to point to the parent.
 *
 * How the point to the parent works is tricky. The vpiArrayWord
 * objects for an array are themselves allocated as an array. All the
 * ArrayWord objects in the array have a word0 that points to the base
 * of the array. Thus, the position into the array (and the index into
 * the memory) is calculated by subtracting word0 from the ArrayWord
 * pointer.
 *
 * To then get to the parent, use word0[-1].parent.
 *
 * The vpiArrayWord is also used as a handle for the index (vpiIndex)
 * for the word. To make that work, return the pointer to the as_index
 * member instead of the as_word member. The result is a different set
 * of vpi functions is bound to the same structure. All the details
 * for the word also apply when treating this as an index.
 */
నిర్మాణం __vpiArrayWord {
      నిర్మాణం as_మాట_t : బహిరంగ __vpiHandle {
	    పూర్ణాంకం get_type_సంకేతం(శూన్య) స్థిరరాశి override { తిరిగి vpi_జ్ఞాపకం_మాట; }
	    పూర్ణాంకం vpi_get(పూర్ణాంకం సంకేతం_2) override;
	    అక్షరం*vpi_get_str(పూర్ణాంకం సంకేతం_2) override;
	    శూన్య vpi_get_విలువ(p_vpi_విలువ vp) override;
	    vpiHandle vpi_put_విలువ(p_vpi_విలువ vp, పూర్ణాంకం flags) override;
	    vpiHandle vpi_handle(పూర్ణాంకం సంకేతం_2) override;
      } as_మాట;

      నిర్మాణం as_సూచీ_t : బహిరంగ __vpiHandle {
	    పూర్ణాంకం get_type_సంకేతం(శూన్య) స్థిరరాశి override { తిరిగి vpi_సూచీ_2; }
	    శూన్య vpi_get_విలువ(p_vpi_విలువ val) override;
      } as_సూచీ;

      సంఘం {
	    నిర్మాణం __vpiArrayBase*మాతృ;
	    నిర్మాణం __vpiArrayWord*word0;
      };

      అంతఃవరుస గుర్తులేని get_సూచీ() స్థిరరాశి { తిరిగి this - word0; }
      అంతఃవరుస నిర్మాణం __vpiArrayBase*get_మాతృ() స్థిరరాశి { తిరిగి (word0 - 1)->మాతృ; }
};

నిర్మాణం __vpiArrayWord*array_var_మాట_from_handle(vpiHandle ref);
నిర్మాణం __vpiArrayWord*array_var_సూచీ_from_handle(vpiHandle ref);

#అయితే_ముగింపు /* ARRAY_COMMON_H */
