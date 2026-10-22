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
#اگر_تعریف_شدہ CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#اگر_ختم
# include  <cstring>
# include  <cassert>

/*
 * The code space is broken into chunks, to make for efficient
 * allocation of large amounts. Each chunk is an array of vvp_code_s
 * structures, with the last opcode loaded with an of_CHUNK_LINK
 * instruction to branch to the next chunk. This handles the case
 * where the program counter steps off the end of a chunk.
 */
ثابت غیرعلامتی رمز_chunk_سائز = 1024;

ساکن ساخت vvp_رمز_s *پہلا_chunk = 0;
ساکن ساخت vvp_رمز_s *current_chunk = 0;
ساکن غیرعلامتی current_within_chunk = 0;

/*
 * This initializes the code space. It sets up the first code chunk,
 * and places at address 0 a ZOMBIE instruction.
 */
خالی codespace_init(خالی)
{
      أكد(current_chunk == 0);
      پہلا_chunk = نیا ساخت vvp_رمز_s [رمز_chunk_سائز];
      current_chunk = پہلا_chunk;

      current_chunk[0].opcode = &کا_ZOMBIE;

      current_chunk[رمز_chunk_سائز-1].opcode = &کا_CHUNK_LINK;
      current_chunk[رمز_chunk_سائز-1].cptr = 0;

      current_within_chunk = 1;

      گنتی_opcodes = 0;
      سائز_opcodes += رمز_chunk_سائز * حجم (ساخت vvp_رمز_s);
}

vvp_رمز_t codespace_اگلا(خالی)
{
      اگر (current_within_chunk == (رمز_chunk_سائز-1)) {
	    current_chunk[رمز_chunk_سائز-1].cptr
		  = نیا ساخت vvp_رمز_s [رمز_chunk_سائز];
	    current_chunk = current_chunk[رمز_chunk_سائز-1].cptr;

	      /* Put a link opcode on the end of the chunk. */
	    current_chunk[رمز_chunk_سائز-1].opcode = &کا_CHUNK_LINK;
	    current_chunk[رمز_chunk_سائز-1].cptr   = 0;

	    current_within_chunk = 0;

	    سائز_opcodes += رمز_chunk_سائز * حجم (ساخت vvp_رمز_s);
      }

      vvp_رمز_t res = current_chunk + current_within_chunk;
      واپس res;
}

vvp_رمز_t codespace_allocate(خالی)
{
      vvp_رمز_t res = codespace_اگلا();
      current_within_chunk += 1;
      گنتی_opcodes += 1;

      memset(res, 0, حجم(*res));

      واپس res;
}

vvp_رمز_t codespace_null(خالی)
{
      واپس پہلا_chunk + 0;
}

#اگر_تعریف_شدہ CHECK_WITH_VALGRIND
خالی codespace_delete(خالی)
{
      vvp_رمز_t cur = پہلا_chunk;

	/* If there are no opcodes then just delete the code space. */
      اگر (گنتی_opcodes == 0) {
	    حذف [] cur;
	    واپس;
      }

      کرو {
	    vvp_رمز_t اگلا_2 = cur[رمز_chunk_سائز-1].cptr;
	    برائے (غیرعلامتی idx = 0 ; idx < رمز_chunk_سائز; idx += 1) {
		  گنتی_opcodes -= 1;
		  اگر ((cur+idx)->opcode == &کا_VPI_CALL) {
			vpi_call_delete((cur+idx)->handle);
		  } ورنہ اگر (((cur+idx)->opcode == &کا_EXEC_UFUNC_REAL) ||
		             ((cur+idx)->opcode == &کا_EXEC_UFUNC_VEC4)) {
			exec_ufunc_delete((cur+idx));
		  } ورنہ اگر ((cur+idx)->opcode == &کا_FILE_سطر) {
			حذف((cur+idx)->handle);
		  } ورنہ اگر (((cur+idx)->opcode == &کا_CONCATI_STR) ||
		             ((cur+idx)->opcode == &کا_NEW_DARRAY) ||
		             ((cur+idx)->opcode == &کا_PUSHI_STR)) {
			حذف [] ((cur+idx)->متن);
		  }
		  اگر (گنتی_opcodes == 0) توڑو;
	    }
	      /* Don't count the &of_CHUNK_LINK opcode. */
	    اگر (گنتی_opcodes != 0) گنتی_opcodes += 1;
	    حذف [] cur;
	    cur = اگلا_2;
      } جب (cur != 0);
}
#اگر_ختم
