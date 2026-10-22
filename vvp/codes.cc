/*
 * Copyright (c) 2001-2016 Stephen Williams (steve@icarus.com)
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

# include  "codes.h"
# include  "statistics.h"
# include  "config.h"
#నిర్వచితమైతే CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#అయితే_ముగింపు
# include  <cstring>
# include  <cassert>

/*
 * The code space is broken into chunks, to make for efficient
 * allocation of large amounts. Each chunk is an array of vvp_code_s
 * structures, with the last opcode loaded with an of_CHUNK_LINK
 * instruction to branch to the next chunk. This handles the case
 * where the program counter steps off the end of a chunk.
 */
స్థిరరాశి గుర్తులేని సంకేతం_chunk_కొలత = 1024;

స్థిర నిర్మాణం vvp_సంకేతం_s *మొదటి_chunk = 0;
స్థిర నిర్మాణం vvp_సంకేతం_s *current_chunk = 0;
స్థిర గుర్తులేని current_within_chunk = 0;

/*
 * This initializes the code space. It sets up the first code chunk,
 * and places at address 0 a ZOMBIE instruction.
 */
శూన్య codespace_init(శూన్య)
{
      ప్రతిజ్ఞా(current_chunk == 0);
      మొదటి_chunk = కొత్త నిర్మాణం vvp_సంకేతం_s [సంకేతం_chunk_కొలత];
      current_chunk = మొదటి_chunk;

      current_chunk[0].opcode = &యొక్క_ZOMBIE;

      current_chunk[సంకేతం_chunk_కొలత-1].opcode = &యొక్క_CHUNK_LINK;
      current_chunk[సంకేతం_chunk_కొలత-1].cptr = 0;

      current_within_chunk = 1;

      లెక్క_opcodes = 0;
      కొలత_opcodes += సంకేతం_chunk_కొలత * పరిమాణం (నిర్మాణం vvp_సంకేతం_s);
}

vvp_సంకేతం_t codespace_తదుపరి(శూన్య)
{
      ఒకవేళ (current_within_chunk == (సంకేతం_chunk_కొలత-1)) {
	    current_chunk[సంకేతం_chunk_కొలత-1].cptr
		  = కొత్త నిర్మాణం vvp_సంకేతం_s [సంకేతం_chunk_కొలత];
	    current_chunk = current_chunk[సంకేతం_chunk_కొలత-1].cptr;

	      /* Put a link opcode on the end of the chunk. */
	    current_chunk[సంకేతం_chunk_కొలత-1].opcode = &యొక్క_CHUNK_LINK;
	    current_chunk[సంకేతం_chunk_కొలత-1].cptr   = 0;

	    current_within_chunk = 0;

	    కొలత_opcodes += సంకేతం_chunk_కొలత * పరిమాణం (నిర్మాణం vvp_సంకేతం_s);
      }

      vvp_సంకేతం_t res = current_chunk + current_within_chunk;
      తిరిగి res;
}

vvp_సంకేతం_t codespace_allocate(శూన్య)
{
      vvp_సంకేతం_t res = codespace_తదుపరి();
      current_within_chunk += 1;
      లెక్క_opcodes += 1;

      memset(res, 0, పరిమాణం(*res));

      తిరిగి res;
}

vvp_సంకేతం_t codespace_null(శూన్య)
{
      తిరిగి మొదటి_chunk + 0;
}

#నిర్వచితమైతే CHECK_WITH_VALGRIND
శూన్య codespace_delete(శూన్య)
{
      vvp_సంకేతం_t cur = మొదటి_chunk;

	/* If there are no opcodes then just delete the code space. */
      ఒకవేళ (లెక్క_opcodes == 0) {
	    తొలగించు [] cur;
	    తిరిగి;
      }

      చేయి {
	    vvp_సంకేతం_t తదుపరి_2 = cur[సంకేతం_chunk_కొలత-1].cptr;
	    కొరకు (గుర్తులేని idx = 0 ; idx < సంకేతం_chunk_కొలత; idx += 1) {
		  లెక్క_opcodes -= 1;
		  ఒకవేళ ((cur+idx)->opcode == &యొక్క_VPI_CALL) {
			vpi_call_delete((cur+idx)->handle);
		  } లేకపోతే ఒకవేళ (((cur+idx)->opcode == &యొక్క_EXEC_UFUNC_REAL) ||
		             ((cur+idx)->opcode == &యొక్క_EXEC_UFUNC_VEC4)) {
			exec_ufunc_delete((cur+idx));
		  } లేకపోతే ఒకవేళ ((cur+idx)->opcode == &యొక్క_FILE_పంక్తి) {
			తొలగించు((cur+idx)->handle);
		  } లేకపోతే ఒకవేళ (((cur+idx)->opcode == &యొక్క_CONCATI_STR) ||
		             ((cur+idx)->opcode == &యొక్క_NEW_DARRAY) ||
		             ((cur+idx)->opcode == &యొక్క_PUSHI_STR)) {
			తొలగించు [] ((cur+idx)->పాఠ్యం);
		  }
		  ఒకవేళ (లెక్క_opcodes == 0) ఆపు;
	    }
	      /* Don't count the &of_CHUNK_LINK opcode. */
	    ఒకవేళ (లెక్క_opcodes != 0) లెక్క_opcodes += 1;
	    తొలగించు [] cur;
	    cur = తదుపరి_2;
      } అయితే (cur != 0);
}
#అయితే_ముగింపు
