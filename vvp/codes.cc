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
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#ਜੇ_ਅੰਤ
# include  <cstring>
# include  <cassert>

/*
 * The code space is broken into chunks, to make for efficient
 * allocation of large amounts. Each chunk is an array of vvp_code_s
 * structures, with the last opcode loaded with an of_CHUNK_LINK
 * instruction to branch to the next chunk. This handles the case
 * where the program counter steps off the end of a chunk.
 */
ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਕੋਡ_chunk_ਮਾਪ = 1024;

ਸਥਿਰ ਬਣਤਰ vvp_ਕੋਡ_s *ਪਹਿਲਾ_chunk = 0;
ਸਥਿਰ ਬਣਤਰ vvp_ਕੋਡ_s *current_chunk = 0;
ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ current_within_chunk = 0;

/*
 * This initializes the code space. It sets up the first code chunk,
 * and places at address 0 a ZOMBIE instruction.
 */
ਖਾਲੀ codespace_init(ਖਾਲੀ)
{
      ਪ੍ਰਤਿਜ੍ਞਾ(current_chunk == 0);
      ਪਹਿਲਾ_chunk = ਨਵਾਂ ਬਣਤਰ vvp_ਕੋਡ_s [ਕੋਡ_chunk_ਮਾਪ];
      current_chunk = ਪਹਿਲਾ_chunk;

      current_chunk[0].opcode = &ਦਾ_ZOMBIE;

      current_chunk[ਕੋਡ_chunk_ਮਾਪ-1].opcode = &ਦਾ_CHUNK_LINK;
      current_chunk[ਕੋਡ_chunk_ਮਾਪ-1].cptr = 0;

      current_within_chunk = 1;

      ਗਿਣਤੀ_opcodes = 0;
      ਮਾਪ_opcodes += ਕੋਡ_chunk_ਮਾਪ * ਆਕਾਰ (ਬਣਤਰ vvp_ਕੋਡ_s);
}

vvp_ਕੋਡ_t codespace_ਅਗਲਾ(ਖਾਲੀ)
{
      ਜੇ (current_within_chunk == (ਕੋਡ_chunk_ਮਾਪ-1)) {
	    current_chunk[ਕੋਡ_chunk_ਮਾਪ-1].cptr
		  = ਨਵਾਂ ਬਣਤਰ vvp_ਕੋਡ_s [ਕੋਡ_chunk_ਮਾਪ];
	    current_chunk = current_chunk[ਕੋਡ_chunk_ਮਾਪ-1].cptr;

	      /* Put a link opcode on the end of the chunk. */
	    current_chunk[ਕੋਡ_chunk_ਮਾਪ-1].opcode = &ਦਾ_CHUNK_LINK;
	    current_chunk[ਕੋਡ_chunk_ਮਾਪ-1].cptr   = 0;

	    current_within_chunk = 0;

	    ਮਾਪ_opcodes += ਕੋਡ_chunk_ਮਾਪ * ਆਕਾਰ (ਬਣਤਰ vvp_ਕੋਡ_s);
      }

      vvp_ਕੋਡ_t res = current_chunk + current_within_chunk;
      ਵਾਪਸ res;
}

vvp_ਕੋਡ_t codespace_allocate(ਖਾਲੀ)
{
      vvp_ਕੋਡ_t res = codespace_ਅਗਲਾ();
      current_within_chunk += 1;
      ਗਿਣਤੀ_opcodes += 1;

      memset(res, 0, ਆਕਾਰ(*res));

      ਵਾਪਸ res;
}

vvp_ਕੋਡ_t codespace_null(ਖਾਲੀ)
{
      ਵਾਪਸ ਪਹਿਲਾ_chunk + 0;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CHECK_WITH_VALGRIND
ਖਾਲੀ codespace_delete(ਖਾਲੀ)
{
      vvp_ਕੋਡ_t cur = ਪਹਿਲਾ_chunk;

	/* If there are no opcodes then just delete the code space. */
      ਜੇ (ਗਿਣਤੀ_opcodes == 0) {
	    ਮਿਟਾਓ [] cur;
	    ਵਾਪਸ;
      }

      ਕਰੋ {
	    vvp_ਕੋਡ_t ਅਗਲਾ_2 = cur[ਕੋਡ_chunk_ਮਾਪ-1].cptr;
	    ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ idx = 0 ; idx < ਕੋਡ_chunk_ਮਾਪ; idx += 1) {
		  ਗਿਣਤੀ_opcodes -= 1;
		  ਜੇ ((cur+idx)->opcode == &ਦਾ_VPI_CALL) {
			vpi_call_delete((cur+idx)->handle);
		  } ਨਹੀਂਤਾਂ ਜੇ (((cur+idx)->opcode == &ਦਾ_EXEC_UFUNC_REAL) ||
		             ((cur+idx)->opcode == &ਦਾ_EXEC_UFUNC_VEC4)) {
			exec_ufunc_delete((cur+idx));
		  } ਨਹੀਂਤਾਂ ਜੇ ((cur+idx)->opcode == &ਦਾ_FILE_ਸਤਰ) {
			ਮਿਟਾਓ((cur+idx)->handle);
		  } ਨਹੀਂਤਾਂ ਜੇ (((cur+idx)->opcode == &ਦਾ_CONCATI_STR) ||
		             ((cur+idx)->opcode == &ਦਾ_NEW_DARRAY) ||
		             ((cur+idx)->opcode == &ਦਾ_PUSHI_STR)) {
			ਮਿਟਾਓ [] ((cur+idx)->ਪਾਠ);
		  }
		  ਜੇ (ਗਿਣਤੀ_opcodes == 0) ਰੋਕੋ;
	    }
	      /* Don't count the &of_CHUNK_LINK opcode. */
	    ਜੇ (ਗਿਣਤੀ_opcodes != 0) ਗਿਣਤੀ_opcodes += 1;
	    ਮਿਟਾਓ [] cur;
	    cur = ਅਗਲਾ_2;
      } ਜਦੋਂ (cur != 0);
}
#ਜੇ_ਅੰਤ
