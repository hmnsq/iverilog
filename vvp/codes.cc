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
#यदि_परिभाषितम् CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#यदि_अन्तः
# include  <cstring>
# include  <cassert>

/*
 * The code space is broken into chunks, to make for efficient
 * allocation of large amounts. Each chunk is an array of vvp_code_s
 * structures, with the last opcode loaded with an of_CHUNK_LINK
 * instruction to branch to the next chunk. This handles the case
 * where the program counter steps off the end of a chunk.
 */
ध्रुव अचिह्नित संहिता_chunk_आकार = 1024;

स्थिर संरचना vvp_संहिता_s *first_chunk = 0;
स्थिर संरचना vvp_संहिता_s *current_chunk = 0;
स्थिर अचिह्नित current_within_chunk = 0;

/*
 * This initializes the code space. It sets up the first code chunk,
 * and places at address 0 a ZOMBIE instruction.
 */
शून्य codespace_init(शून्य)
{
      प्रतिज्ञा(current_chunk == 0);
      first_chunk = नूतन संरचना vvp_संहिता_s [संहिता_chunk_आकार];
      current_chunk = first_chunk;

      current_chunk[0].opcode = &सम्बन्धि_ZOMBIE;

      current_chunk[संहिता_chunk_आकार-1].opcode = &सम्बन्धि_CHUNK_LINK;
      current_chunk[संहिता_chunk_आकार-1].cptr = 0;

      current_within_chunk = 1;

      गणनम्_opcodes = 0;
      आकार_opcodes += संहिता_chunk_आकार * परिमाण (संरचना vvp_संहिता_s);
}

vvp_संहिता_t codespace_अग्र(शून्य)
{
      यदि (current_within_chunk == (संहिता_chunk_आकार-1)) {
	    current_chunk[संहिता_chunk_आकार-1].cptr
		  = नूतन संरचना vvp_संहिता_s [संहिता_chunk_आकार];
	    current_chunk = current_chunk[संहिता_chunk_आकार-1].cptr;

	      /* Put a link opcode on the end of the chunk. */
	    current_chunk[संहिता_chunk_आकार-1].opcode = &सम्बन्धि_CHUNK_LINK;
	    current_chunk[संहिता_chunk_आकार-1].cptr   = 0;

	    current_within_chunk = 0;

	    आकार_opcodes += संहिता_chunk_आकार * परिमाण (संरचना vvp_संहिता_s);
      }

      vvp_संहिता_t res = current_chunk + current_within_chunk;
      प्रत्यागम res;
}

vvp_संहिता_t codespace_allocate(शून्य)
{
      vvp_संहिता_t res = codespace_अग्र();
      current_within_chunk += 1;
      गणनम्_opcodes += 1;

      स्मृतिं_पूरय(res, 0, परिमाण(*res));

      प्रत्यागम res;
}

vvp_संहिता_t codespace_null(शून्य)
{
      प्रत्यागम first_chunk + 0;
}

#यदि_परिभाषितम् CHECK_WITH_VALGRIND
शून्य codespace_delete(शून्य)
{
      vvp_संहिता_t cur = first_chunk;

	/* If there are no opcodes then just delete the code space. */
      यदि (गणनम्_opcodes == 0) {
	    विलोपय [] cur;
	    प्रत्यागम;
      }

      कुरु {
	    vvp_संहिता_t अग्र_2 = cur[संहिता_chunk_आकार-1].cptr;
	    प्रति (अचिह्नित idx = 0 ; idx < संहिता_chunk_आकार; idx += 1) {
		  गणनम्_opcodes -= 1;
		  यदि ((cur+idx)->opcode == &सम्बन्धि_VPI_CALL) {
			vpi_call_delete((cur+idx)->handle);
		  } अन्यथा यदि (((cur+idx)->opcode == &सम्बन्धि_EXEC_UFUNC_REAL) ||
		             ((cur+idx)->opcode == &सम्बन्धि_EXEC_UFUNC_VEC4)) {
			exec_ufunc_delete((cur+idx));
		  } अन्यथा यदि ((cur+idx)->opcode == &सम्बन्धि_FILE_पङ्क्तिः) {
			विलोपय((cur+idx)->handle);
		  } अन्यथा यदि (((cur+idx)->opcode == &सम्बन्धि_CONCATI_STR) ||
		             ((cur+idx)->opcode == &सम्बन्धि_NEW_DARRAY) ||
		             ((cur+idx)->opcode == &सम्बन्धि_PUSHI_STR)) {
			विलोपय [] ((cur+idx)->text);
		  }
		  यदि (गणनम्_opcodes == 0) भङ्ग;
	    }
	      /* Don't count the &of_CHUNK_LINK opcode. */
	    यदि (गणनम्_opcodes != 0) गणनम्_opcodes += 1;
	    विलोपय [] cur;
	    cur = अग्र_2;
      } यावत् (cur != 0);
}
#यदि_अन्तः
