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
#جيڪڏهن_وصف_ٿيل CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#جيڪڏهن_پورو
# include  <cstring>
# include  <cassert>

/*
 * The code space is broken into chunks, to make for efficient
 * allocation of large amounts. Each chunk is an array of vvp_code_s
 * structures, with the last opcode loaded with an of_CHUNK_LINK
 * instruction to branch to the next chunk. This handles the case
 * where the program counter steps off the end of a chunk.
 */
ثابت بنانشان ڪوڊ_chunk_سائيز = 1024;

ساكن بنية vvp_ڪوڊ_s *پهريون_chunk = 0;
ساكن بنية vvp_ڪوڊ_s *current_chunk = 0;
ساكن بنانشان current_within_chunk = 0;

/*
 * This initializes the code space. It sets up the first code chunk,
 * and places at address 0 a ZOMBIE instruction.
 */
فراغ codespace_init(فراغ)
{
      أكد(current_chunk == 0);
      پهريون_chunk = جديد بنية vvp_ڪوڊ_s [ڪوڊ_chunk_سائيز];
      current_chunk = پهريون_chunk;

      current_chunk[0].opcode = &جو_ZOMBIE;

      current_chunk[ڪوڊ_chunk_سائيز-1].opcode = &جو_CHUNK_LINK;
      current_chunk[ڪوڊ_chunk_سائيز-1].cptr = 0;

      current_within_chunk = 1;

      ڳڻپ_opcodes = 0;
      سائيز_opcodes += ڪوڊ_chunk_سائيز * حجم (بنية vvp_ڪوڊ_s);
}

vvp_ڪوڊ_t codespace_ايندڙ(فراغ)
{
      إذا (current_within_chunk == (ڪوڊ_chunk_سائيز-1)) {
	    current_chunk[ڪوڊ_chunk_سائيز-1].cptr
		  = جديد بنية vvp_ڪوڊ_s [ڪوڊ_chunk_سائيز];
	    current_chunk = current_chunk[ڪوڊ_chunk_سائيز-1].cptr;

	      /* Put a link opcode on the end of the chunk. */
	    current_chunk[ڪوڊ_chunk_سائيز-1].opcode = &جو_CHUNK_LINK;
	    current_chunk[ڪوڊ_chunk_سائيز-1].cptr   = 0;

	    current_within_chunk = 0;

	    سائيز_opcodes += ڪوڊ_chunk_سائيز * حجم (بنية vvp_ڪوڊ_s);
      }

      vvp_ڪوڊ_t res = current_chunk + current_within_chunk;
      أرجع res;
}

vvp_ڪوڊ_t codespace_allocate(فراغ)
{
      vvp_ڪوڊ_t res = codespace_ايندڙ();
      current_within_chunk += 1;
      ڳڻپ_opcodes += 1;

      memset(res, 0, حجم(*res));

      أرجع res;
}

vvp_ڪوڊ_t codespace_null(فراغ)
{
      أرجع پهريون_chunk + 0;
}

#جيڪڏهن_وصف_ٿيل CHECK_WITH_VALGRIND
فراغ codespace_delete(فراغ)
{
      vvp_ڪوڊ_t cur = پهريون_chunk;

	/* If there are no opcodes then just delete the code space. */
      إذا (ڳڻپ_opcodes == 0) {
	    ميٽيو [] cur;
	    أرجع;
      }

      افعل {
	    vvp_ڪوڊ_t ايندڙ_2 = cur[ڪوڊ_chunk_سائيز-1].cptr;
	    لكل (بنانشان idx = 0 ; idx < ڪوڊ_chunk_سائيز; idx += 1) {
		  ڳڻپ_opcodes -= 1;
		  إذا ((cur+idx)->opcode == &جو_VPI_CALL) {
			vpi_call_delete((cur+idx)->handle);
		  } وإلا إذا (((cur+idx)->opcode == &جو_EXEC_UFUNC_REAL) ||
		             ((cur+idx)->opcode == &جو_EXEC_UFUNC_VEC4)) {
			exec_ufunc_delete((cur+idx));
		  } وإلا إذا ((cur+idx)->opcode == &جو_FILE_سٽ) {
			ميٽيو((cur+idx)->handle);
		  } وإلا إذا (((cur+idx)->opcode == &جو_CONCATI_STR) ||
		             ((cur+idx)->opcode == &جو_NEW_DARRAY) ||
		             ((cur+idx)->opcode == &جو_PUSHI_STR)) {
			ميٽيو [] ((cur+idx)->متن);
		  }
		  إذا (ڳڻپ_opcodes == 0) اكسر;
	    }
	      /* Don't count the &of_CHUNK_LINK opcode. */
	    إذا (ڳڻپ_opcodes != 0) ڳڻپ_opcodes += 1;
	    ميٽيو [] cur;
	    cur = ايندڙ_2;
      } طالما (cur != 0);
}
#جيڪڏهن_پورو
