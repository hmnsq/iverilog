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
#جے_تعریف_شدہ CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#جے_انت
# include  <cstring>
# include  <cassert>

/*
 * The code space is broken into chunks, to make for efficient
 * allocation of large amounts. Each chunk is an array of vvp_code_s
 * structures, with the last opcode loaded with an of_CHUNK_LINK
 * instruction to branch to the next chunk. This handles the case
 * where the program counter steps off the end of a chunk.
 */
ثابت بنانشانی کوڈ_chunk_ناپ = 1024;

ساكن بنية vvp_کوڈ_s *پہلا_chunk = 0;
ساكن بنية vvp_کوڈ_s *current_chunk = 0;
ساكن بنانشانی current_within_chunk = 0;

/*
 * This initializes the code space. It sets up the first code chunk,
 * and places at address 0 a ZOMBIE instruction.
 */
فراغ codespace_init(فراغ)
{
      أكد(current_chunk == 0);
      پہلا_chunk = جديد بنية vvp_کوڈ_s [کوڈ_chunk_ناپ];
      current_chunk = پہلا_chunk;

      current_chunk[0].opcode = &دا_ZOMBIE;

      current_chunk[کوڈ_chunk_ناپ-1].opcode = &دا_CHUNK_LINK;
      current_chunk[کوڈ_chunk_ناپ-1].cptr = 0;

      current_within_chunk = 1;

      گنتی_opcodes = 0;
      ناپ_opcodes += کوڈ_chunk_ناپ * حجم (بنية vvp_کوڈ_s);
}

vvp_کوڈ_t codespace_اگلا(فراغ)
{
      إذا (current_within_chunk == (کوڈ_chunk_ناپ-1)) {
	    current_chunk[کوڈ_chunk_ناپ-1].cptr
		  = جديد بنية vvp_کوڈ_s [کوڈ_chunk_ناپ];
	    current_chunk = current_chunk[کوڈ_chunk_ناپ-1].cptr;

	      /* Put a link opcode on the end of the chunk. */
	    current_chunk[کوڈ_chunk_ناپ-1].opcode = &دا_CHUNK_LINK;
	    current_chunk[کوڈ_chunk_ناپ-1].cptr   = 0;

	    current_within_chunk = 0;

	    ناپ_opcodes += کوڈ_chunk_ناپ * حجم (بنية vvp_کوڈ_s);
      }

      vvp_کوڈ_t res = current_chunk + current_within_chunk;
      أرجع res;
}

vvp_کوڈ_t codespace_allocate(فراغ)
{
      vvp_کوڈ_t res = codespace_اگلا();
      current_within_chunk += 1;
      گنتی_opcodes += 1;

      memset(res, 0, حجم(*res));

      أرجع res;
}

vvp_کوڈ_t codespace_null(فراغ)
{
      أرجع پہلا_chunk + 0;
}

#جے_تعریف_شدہ CHECK_WITH_VALGRIND
فراغ codespace_delete(فراغ)
{
      vvp_کوڈ_t cur = پہلا_chunk;

	/* If there are no opcodes then just delete the code space. */
      إذا (گنتی_opcodes == 0) {
	    مٹاؤ [] cur;
	    أرجع;
      }

      افعل {
	    vvp_کوڈ_t اگلا_2 = cur[کوڈ_chunk_ناپ-1].cptr;
	    لكل (بنانشانی idx = 0 ; idx < کوڈ_chunk_ناپ; idx += 1) {
		  گنتی_opcodes -= 1;
		  إذا ((cur+idx)->opcode == &دا_VPI_CALL) {
			vpi_call_delete((cur+idx)->handle);
		  } وإلا إذا (((cur+idx)->opcode == &دا_EXEC_UFUNC_REAL) ||
		             ((cur+idx)->opcode == &دا_EXEC_UFUNC_VEC4)) {
			exec_ufunc_delete((cur+idx));
		  } وإلا إذا ((cur+idx)->opcode == &دا_FILE_سطر) {
			مٹاؤ((cur+idx)->handle);
		  } وإلا إذا (((cur+idx)->opcode == &دا_CONCATI_STR) ||
		             ((cur+idx)->opcode == &دا_NEW_DARRAY) ||
		             ((cur+idx)->opcode == &دا_PUSHI_STR)) {
			مٹاؤ [] ((cur+idx)->متن);
		  }
		  إذا (گنتی_opcodes == 0) اكسر;
	    }
	      /* Don't count the &of_CHUNK_LINK opcode. */
	    إذا (گنتی_opcodes != 0) گنتی_opcodes += 1;
	    مٹاؤ [] cur;
	    cur = اگلا_2;
      } طالما (cur != 0);
}
#جے_انت
