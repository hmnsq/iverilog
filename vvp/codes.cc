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
#ifdef CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#endif
# include  <cstring>
# include  <cassert>

/*
 * The code space is broken into chunks, to make for efficient
 * allocation of large amounts. Each chunk is an array of vvp_code_s
 * structures, with the last opcode loaded with an of_CHUNK_LINK
 * instruction to branch to the next chunk. This handles the case
 * where the program counter steps off the end of a chunk.
 */
ଅଚଳ unsigned code_chunk_size = 1024;

ସ୍ଥିର ସଂରଚନା vvp_code_s *first_chunk = 0;
ସ୍ଥିର ସଂରଚନା vvp_code_s *current_chunk = 0;
ସ୍ଥିର unsigned current_within_chunk = 0;

/*
 * This initializes the code space. It sets up the first code chunk,
 * and places at address 0 a ZOMBIE instruction.
 */
ଶୂନ୍ୟ codespace_init(ଶୂନ୍ୟ)
{
      ପ୍ରତିଜ୍ଞା(current_chunk == 0);
      first_chunk = ନୂତନ ସଂରଚନା vvp_code_s [code_chunk_size];
      current_chunk = first_chunk;

      current_chunk[0].opcode = &of_ZOMBIE;

      current_chunk[code_chunk_size-1].opcode = &of_CHUNK_LINK;
      current_chunk[code_chunk_size-1].cptr = 0;

      current_within_chunk = 1;

      ଗଣନା_opcodes = 0;
      size_opcodes += code_chunk_size * ଆକାର (ସଂରଚନା vvp_code_s);
}

vvp_code_t codespace_next(ଶୂନ୍ୟ)
{
      ଯଦି (current_within_chunk == (code_chunk_size-1)) {
	    current_chunk[code_chunk_size-1].cptr
		  = ନୂତନ ସଂରଚନା vvp_code_s [code_chunk_size];
	    current_chunk = current_chunk[code_chunk_size-1].cptr;

	      /* Put a link opcode on the end of the chunk. */
	    current_chunk[code_chunk_size-1].opcode = &of_CHUNK_LINK;
	    current_chunk[code_chunk_size-1].cptr   = 0;

	    current_within_chunk = 0;

	    size_opcodes += code_chunk_size * ଆକାର (ସଂରଚନା vvp_code_s);
      }

      vvp_code_t res = current_chunk + current_within_chunk;
      ଫେରନ୍ତୁ res;
}

vvp_code_t codespace_allocate(ଶୂନ୍ୟ)
{
      vvp_code_t res = codespace_next();
      current_within_chunk += 1;
      ଗଣନା_opcodes += 1;

      memset(res, 0, ଆକାର(*res));

      ଫେରନ୍ତୁ res;
}

vvp_code_t codespace_null(ଶୂନ୍ୟ)
{
      ଫେରନ୍ତୁ first_chunk + 0;
}

#ifdef CHECK_WITH_VALGRIND
ଶୂନ୍ୟ codespace_delete(ଶୂନ୍ୟ)
{
      vvp_code_t cur = first_chunk;

	/* If there are no opcodes then just delete the code space. */
      ଯଦି (ଗଣନା_opcodes == 0) {
	    delete [] cur;
	    ଫେରନ୍ତୁ;
      }

      କର {
	    vvp_code_t next = cur[code_chunk_size-1].cptr;
	    ପାଇଁ (unsigned idx = 0 ; idx < code_chunk_size; idx += 1) {
		  ଗଣନା_opcodes -= 1;
		  ଯଦି ((cur+idx)->opcode == &of_VPI_CALL) {
			vpi_call_delete((cur+idx)->handle);
		  } ଅନ୍ୟଥା ଯଦି (((cur+idx)->opcode == &of_EXEC_UFUNC_REAL) ||
		             ((cur+idx)->opcode == &of_EXEC_UFUNC_VEC4)) {
			exec_ufunc_delete((cur+idx));
		  } ଅନ୍ୟଥା ଯଦି ((cur+idx)->opcode == &of_FILE_ଧାଡ଼ି) {
			delete((cur+idx)->handle);
		  } ଅନ୍ୟଥା ଯଦି (((cur+idx)->opcode == &of_CONCATI_STR) ||
		             ((cur+idx)->opcode == &of_NEW_DARRAY) ||
		             ((cur+idx)->opcode == &of_PUSHI_STR)) {
			delete [] ((cur+idx)->text);
		  }
		  ଯଦି (ଗଣନା_opcodes == 0) ଭାଙ୍ଗ;
	    }
	      /* Don't count the &of_CHUNK_LINK opcode. */
	    ଯଦି (ଗଣନା_opcodes != 0) ଗଣନା_opcodes += 1;
	    delete [] cur;
	    cur = next;
      } ଯେତେବେଳେ (cur != 0);
}
#endif
