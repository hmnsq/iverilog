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
#اگر_تعریف_شده CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#پایان_اگر
# include  <cstring>
# include  <cassert>

/*
 * The code space is broken into chunks, to make for efficient
 * allocation of large amounts. Each chunk is an array of vvp_code_s
 * structures, with the last opcode loaded with an of_CHUNK_LINK
 * instruction to branch to the next chunk. This handles the case
 * where the program counter steps off the end of a chunk.
 */
ثابت بیعلامت کد_chunk_اندازه = 1024;

ساكن بنية vvp_کد_s *اولی_chunk = 0;
ساكن بنية vvp_کد_s *current_chunk = 0;
ساكن بیعلامت current_within_chunk = 0;

/*
 * This initializes the code space. It sets up the first code chunk,
 * and places at address 0 a ZOMBIE instruction.
 */
فراغ codespace_init(فراغ)
{
      أكد(current_chunk == 0);
      اولی_chunk = جديد بنية vvp_کد_s [کد_chunk_اندازه];
      current_chunk = اولی_chunk;

      current_chunk[0].opcode = &از_ZOMBIE;

      current_chunk[کد_chunk_اندازه-1].opcode = &از_CHUNK_LINK;
      current_chunk[کد_chunk_اندازه-1].cptr = 0;

      current_within_chunk = 1;

      شمار_opcodes = 0;
      اندازه_opcodes += کد_chunk_اندازه * حجم (بنية vvp_کد_s);
}

vvp_کد_t codespace_بعدی(فراغ)
{
      إذا (current_within_chunk == (کد_chunk_اندازه-1)) {
	    current_chunk[کد_chunk_اندازه-1].cptr
		  = جديد بنية vvp_کد_s [کد_chunk_اندازه];
	    current_chunk = current_chunk[کد_chunk_اندازه-1].cptr;

	      /* Put a link opcode on the end of the chunk. */
	    current_chunk[کد_chunk_اندازه-1].opcode = &از_CHUNK_LINK;
	    current_chunk[کد_chunk_اندازه-1].cptr   = 0;

	    current_within_chunk = 0;

	    اندازه_opcodes += کد_chunk_اندازه * حجم (بنية vvp_کد_s);
      }

      vvp_کد_t res = current_chunk + current_within_chunk;
      أرجع res;
}

vvp_کد_t codespace_allocate(فراغ)
{
      vvp_کد_t res = codespace_بعدی();
      current_within_chunk += 1;
      شمار_opcodes += 1;

      memset(res, 0, حجم(*res));

      أرجع res;
}

vvp_کد_t codespace_null(فراغ)
{
      أرجع اولی_chunk + 0;
}

#اگر_تعریف_شده CHECK_WITH_VALGRIND
فراغ codespace_delete(فراغ)
{
      vvp_کد_t cur = اولی_chunk;

	/* If there are no opcodes then just delete the code space. */
      إذا (شمار_opcodes == 0) {
	    حذف [] cur;
	    أرجع;
      }

      افعل {
	    vvp_کد_t بعدی_2 = cur[کد_chunk_اندازه-1].cptr;
	    لكل (بیعلامت idx = 0 ; idx < کد_chunk_اندازه; idx += 1) {
		  شمار_opcodes -= 1;
		  إذا ((cur+idx)->opcode == &از_VPI_CALL) {
			vpi_call_delete((cur+idx)->handle);
		  } وإلا إذا (((cur+idx)->opcode == &از_EXEC_UFUNC_REAL) ||
		             ((cur+idx)->opcode == &از_EXEC_UFUNC_VEC4)) {
			exec_ufunc_delete((cur+idx));
		  } وإلا إذا ((cur+idx)->opcode == &از_FILE_سطر) {
			حذف((cur+idx)->handle);
		  } وإلا إذا (((cur+idx)->opcode == &از_CONCATI_STR) ||
		             ((cur+idx)->opcode == &از_NEW_DARRAY) ||
		             ((cur+idx)->opcode == &از_PUSHI_STR)) {
			حذف [] ((cur+idx)->متن);
		  }
		  إذا (شمار_opcodes == 0) اكسر;
	    }
	      /* Don't count the &of_CHUNK_LINK opcode. */
	    إذا (شمار_opcodes != 0) شمار_opcodes += 1;
	    حذف [] cur;
	    cur = بعدی_2;
      } طالما (cur != 0);
}
#پایان_اگر
