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
#که_تعریف_شوی CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#د_که_پای
# include  <cstring>
# include  <cassert>

/*
 * The code space is broken into chunks, to make for efficient
 * allocation of large amounts. Each chunk is an array of vvp_code_s
 * structures, with the last opcode loaded with an of_CHUNK_LINK
 * instruction to branch to the next chunk. This handles the case
 * where the program counter steps off the end of a chunk.
 */
ثابت بینښه کوډ_chunk_اندازه = 1024;

ساكن بنية vvp_کوډ_s *لومړی_chunk = 0;
ساكن بنية vvp_کوډ_s *current_chunk = 0;
ساكن بینښه current_within_chunk = 0;

/*
 * This initializes the code space. It sets up the first code chunk,
 * and places at address 0 a ZOMBIE instruction.
 */
فراغ codespace_init(فراغ)
{
      أكد(current_chunk == 0);
      لومړی_chunk = جديد بنية vvp_کوډ_s [کوډ_chunk_اندازه];
      current_chunk = لومړی_chunk;

      current_chunk[0].opcode = &د_ZOMBIE;

      current_chunk[کوډ_chunk_اندازه-1].opcode = &د_CHUNK_LINK;
      current_chunk[کوډ_chunk_اندازه-1].cptr = 0;

      current_within_chunk = 1;

      شمېر_opcodes = 0;
      اندازه_opcodes += کوډ_chunk_اندازه * حجم (بنية vvp_کوډ_s);
}

vvp_کوډ_t codespace_بل(فراغ)
{
      إذا (current_within_chunk == (کوډ_chunk_اندازه-1)) {
	    current_chunk[کوډ_chunk_اندازه-1].cptr
		  = جديد بنية vvp_کوډ_s [کوډ_chunk_اندازه];
	    current_chunk = current_chunk[کوډ_chunk_اندازه-1].cptr;

	      /* Put a link opcode on the end of the chunk. */
	    current_chunk[کوډ_chunk_اندازه-1].opcode = &د_CHUNK_LINK;
	    current_chunk[کوډ_chunk_اندازه-1].cptr   = 0;

	    current_within_chunk = 0;

	    اندازه_opcodes += کوډ_chunk_اندازه * حجم (بنية vvp_کوډ_s);
      }

      vvp_کوډ_t res = current_chunk + current_within_chunk;
      أرجع res;
}

vvp_کوډ_t codespace_allocate(فراغ)
{
      vvp_کوډ_t res = codespace_بل();
      current_within_chunk += 1;
      شمېر_opcodes += 1;

      memset(res, 0, حجم(*res));

      أرجع res;
}

vvp_کوډ_t codespace_null(فراغ)
{
      أرجع لومړی_chunk + 0;
}

#که_تعریف_شوی CHECK_WITH_VALGRIND
فراغ codespace_delete(فراغ)
{
      vvp_کوډ_t cur = لومړی_chunk;

	/* If there are no opcodes then just delete the code space. */
      إذا (شمېر_opcodes == 0) {
	    ړنګول [] cur;
	    أرجع;
      }

      افعل {
	    vvp_کوډ_t بل_2 = cur[کوډ_chunk_اندازه-1].cptr;
	    لكل (بینښه idx = 0 ; idx < کوډ_chunk_اندازه; idx += 1) {
		  شمېر_opcodes -= 1;
		  إذا ((cur+idx)->opcode == &د_VPI_CALL) {
			vpi_call_delete((cur+idx)->handle);
		  } وإلا إذا (((cur+idx)->opcode == &د_EXEC_UFUNC_REAL) ||
		             ((cur+idx)->opcode == &د_EXEC_UFUNC_VEC4)) {
			exec_ufunc_delete((cur+idx));
		  } وإلا إذا ((cur+idx)->opcode == &د_FILE_کرښه) {
			ړنګول((cur+idx)->handle);
		  } وإلا إذا (((cur+idx)->opcode == &د_CONCATI_STR) ||
		             ((cur+idx)->opcode == &د_NEW_DARRAY) ||
		             ((cur+idx)->opcode == &د_PUSHI_STR)) {
			ړنګول [] ((cur+idx)->متن);
		  }
		  إذا (شمېر_opcodes == 0) اكسر;
	    }
	      /* Don't count the &of_CHUNK_LINK opcode. */
	    إذا (شمېر_opcodes != 0) شمېر_opcodes += 1;
	    ړنګول [] cur;
	    cur = بل_2;
      } طالما (cur != 0);
}
#د_که_پای
