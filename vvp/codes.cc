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
#अगर_घोषित CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#पूर्ण_अगर
# include  <cstring>
# include  <cassert>

/*
 * The code space is broken into chunks, to make for efficient
 * allocation of large amounts. Each chunk is an array of vvp_code_s
 * structures, with the last opcode loaded with an of_CHUNK_LINK
 * instruction to branch to the next chunk. This handles the case
 * where the program counter steps off the end of a chunk.
 */
स्थिर अचिन्हित code_chunk_आकार = 1024;

अटल काष्ठा vvp_code_s *first_chunk = 0;
अटल काष्ठा vvp_code_s *current_chunk = 0;
अटल अचिन्हित current_within_chunk = 0;

/*
 * This initializes the code space. It sets up the first code chunk,
 * and places at address 0 a ZOMBIE instruction.
 */
व्योम codespace_init(व्योम)
{
      निश्चित(current_chunk == 0);
      first_chunk = नया काष्ठा vvp_code_s [code_chunk_आकार];
      current_chunk = first_chunk;

      current_chunk[0].opcode = &of_ZOMBIE;

      current_chunk[code_chunk_आकार-1].opcode = &of_CHUNK_LINK;
      current_chunk[code_chunk_आकार-1].cptr = 0;

      current_within_chunk = 1;

      गिनती_opcodes = 0;
      आकार_opcodes += code_chunk_आकार * माप (काष्ठा vvp_code_s);
}

vvp_code_t codespace_अग्र(व्योम)
{
      अगर (current_within_chunk == (code_chunk_आकार-1)) {
	    current_chunk[code_chunk_आकार-1].cptr
		  = नया काष्ठा vvp_code_s [code_chunk_आकार];
	    current_chunk = current_chunk[code_chunk_आकार-1].cptr;

	      /* Put a link opcode on the end of the chunk. */
	    current_chunk[code_chunk_आकार-1].opcode = &of_CHUNK_LINK;
	    current_chunk[code_chunk_आकार-1].cptr   = 0;

	    current_within_chunk = 0;

	    आकार_opcodes += code_chunk_आकार * माप (काष्ठा vvp_code_s);
      }

      vvp_code_t res = current_chunk + current_within_chunk;
      वापस res;
}

vvp_code_t codespace_allocate(व्योम)
{
      vvp_code_t res = codespace_अग्र();
      current_within_chunk += 1;
      गिनती_opcodes += 1;

      स_रखो(res, 0, माप(*res));

      वापस res;
}

vvp_code_t codespace_null(व्योम)
{
      वापस first_chunk + 0;
}

#अगर_घोषित CHECK_WITH_VALGRIND
व्योम codespace_delete(व्योम)
{
      vvp_code_t cur = first_chunk;

	/* If there are no opcodes then just delete the code space. */
      अगर (गिनती_opcodes == 0) {
	    मिटाओ [] cur;
	    वापस;
      }

      करो {
	    vvp_code_t अग्र_2 = cur[code_chunk_आकार-1].cptr;
	    क्रम (अचिन्हित idx = 0 ; idx < code_chunk_आकार; idx += 1) {
		  गिनती_opcodes -= 1;
		  अगर ((cur+idx)->opcode == &of_VPI_CALL) {
			vpi_call_delete((cur+idx)->handle);
		  } अन्यथा अगर (((cur+idx)->opcode == &of_EXEC_UFUNC_REAL) ||
		             ((cur+idx)->opcode == &of_EXEC_UFUNC_VEC4)) {
			exec_ufunc_delete((cur+idx));
		  } अन्यथा अगर ((cur+idx)->opcode == &of_FILE_पंक्ति) {
			मिटाओ((cur+idx)->handle);
		  } अन्यथा अगर (((cur+idx)->opcode == &of_CONCATI_STR) ||
		             ((cur+idx)->opcode == &of_NEW_DARRAY) ||
		             ((cur+idx)->opcode == &of_PUSHI_STR)) {
			मिटाओ [] ((cur+idx)->text);
		  }
		  अगर (गिनती_opcodes == 0) अवरोध;
	    }
	      /* Don't count the &of_CHUNK_LINK opcode. */
	    अगर (गिनती_opcodes != 0) गिनती_opcodes += 1;
	    मिटाओ [] cur;
	    cur = अग्र_2;
      } जबतक (cur != 0);
}
#पूर्ण_अगर
