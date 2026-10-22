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
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECK_WITH_VALGRIND
# include  "vvp_cleanup.h"
#ಆದರೆ_ಅಂತ್ಯ
# include  <cstring>
# include  <cassert>

/*
 * The code space is broken into chunks, to make for efficient
 * allocation of large amounts. Each chunk is an array of vvp_code_s
 * structures, with the last opcode loaded with an of_CHUNK_LINK
 * instruction to branch to the next chunk. This handles the case
 * where the program counter steps off the end of a chunk.
 */
ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಸಂಕೇತ_chunk_ಗಾತ್ರ = 1024;

ಸ್ಥಿರ ರಚನೆ vvp_ಸಂಕೇತ_s *ಮೊದಲ_chunk = 0;
ಸ್ಥಿರ ರಚನೆ vvp_ಸಂಕೇತ_s *current_chunk = 0;
ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ current_within_chunk = 0;

/*
 * This initializes the code space. It sets up the first code chunk,
 * and places at address 0 a ZOMBIE instruction.
 */
ಶೂನ್ಯ codespace_init(ಶೂನ್ಯ)
{
      ಪ್ರತಿಜ್ಞಾ(current_chunk == 0);
      ಮೊದಲ_chunk = ಹೊಸ ರಚನೆ vvp_ಸಂಕೇತ_s [ಸಂಕೇತ_chunk_ಗಾತ್ರ];
      current_chunk = ಮೊದಲ_chunk;

      current_chunk[0].opcode = &ದ_ZOMBIE;

      current_chunk[ಸಂಕೇತ_chunk_ಗಾತ್ರ-1].opcode = &ದ_CHUNK_LINK;
      current_chunk[ಸಂಕೇತ_chunk_ಗಾತ್ರ-1].cptr = 0;

      current_within_chunk = 1;

      ಎಣಿಕೆ_opcodes = 0;
      ಗಾತ್ರ_opcodes += ಸಂಕೇತ_chunk_ಗಾತ್ರ * ಗಾತ್ರ (ರಚನೆ vvp_ಸಂಕೇತ_s);
}

vvp_ಸಂಕೇತ_t codespace_ಮುಂದಿನ(ಶೂನ್ಯ)
{
      ಆದರೆ (current_within_chunk == (ಸಂಕೇತ_chunk_ಗಾತ್ರ-1)) {
	    current_chunk[ಸಂಕೇತ_chunk_ಗಾತ್ರ-1].cptr
		  = ಹೊಸ ರಚನೆ vvp_ಸಂಕೇತ_s [ಸಂಕೇತ_chunk_ಗಾತ್ರ];
	    current_chunk = current_chunk[ಸಂಕೇತ_chunk_ಗಾತ್ರ-1].cptr;

	      /* Put a link opcode on the end of the chunk. */
	    current_chunk[ಸಂಕೇತ_chunk_ಗಾತ್ರ-1].opcode = &ದ_CHUNK_LINK;
	    current_chunk[ಸಂಕೇತ_chunk_ಗಾತ್ರ-1].cptr   = 0;

	    current_within_chunk = 0;

	    ಗಾತ್ರ_opcodes += ಸಂಕೇತ_chunk_ಗಾತ್ರ * ಗಾತ್ರ (ರಚನೆ vvp_ಸಂಕೇತ_s);
      }

      vvp_ಸಂಕೇತ_t res = current_chunk + current_within_chunk;
      ಹಿಂತಿರುಗಿ res;
}

vvp_ಸಂಕೇತ_t codespace_allocate(ಶೂನ್ಯ)
{
      vvp_ಸಂಕೇತ_t res = codespace_ಮುಂದಿನ();
      current_within_chunk += 1;
      ಎಣಿಕೆ_opcodes += 1;

      memset(res, 0, ಗಾತ್ರ(*res));

      ಹಿಂತಿರುಗಿ res;
}

vvp_ಸಂಕೇತ_t codespace_null(ಶೂನ್ಯ)
{
      ಹಿಂತಿರುಗಿ ಮೊದಲ_chunk + 0;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CHECK_WITH_VALGRIND
ಶೂನ್ಯ codespace_delete(ಶೂನ್ಯ)
{
      vvp_ಸಂಕೇತ_t cur = ಮೊದಲ_chunk;

	/* If there are no opcodes then just delete the code space. */
      ಆದರೆ (ಎಣಿಕೆ_opcodes == 0) {
	    ಅಳಿಸು [] cur;
	    ಹಿಂತಿರುಗಿ;
      }

      ಮಾಡು {
	    vvp_ಸಂಕೇತ_t ಮುಂದಿನ_2 = cur[ಸಂಕೇತ_chunk_ಗಾತ್ರ-1].cptr;
	    ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ; idx < ಸಂಕೇತ_chunk_ಗಾತ್ರ; idx += 1) {
		  ಎಣಿಕೆ_opcodes -= 1;
		  ಆದರೆ ((cur+idx)->opcode == &ದ_VPI_CALL) {
			vpi_call_delete((cur+idx)->handle);
		  } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (((cur+idx)->opcode == &ದ_EXEC_UFUNC_REAL) ||
		             ((cur+idx)->opcode == &ದ_EXEC_UFUNC_VEC4)) {
			exec_ufunc_delete((cur+idx));
		  } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((cur+idx)->opcode == &ದ_FILE_ಸಾಲು) {
			ಅಳಿಸು((cur+idx)->handle);
		  } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (((cur+idx)->opcode == &ದ_CONCATI_STR) ||
		             ((cur+idx)->opcode == &ದ_NEW_DARRAY) ||
		             ((cur+idx)->opcode == &ದ_PUSHI_STR)) {
			ಅಳಿಸು [] ((cur+idx)->ಪಠ್ಯ);
		  }
		  ಆದರೆ (ಎಣಿಕೆ_opcodes == 0) ನಿಲ್ಲಿಸು;
	    }
	      /* Don't count the &of_CHUNK_LINK opcode. */
	    ಆದರೆ (ಎಣಿಕೆ_opcodes != 0) ಎಣಿಕೆ_opcodes += 1;
	    ಅಳಿಸು [] cur;
	    cur = ಮುಂದಿನ_2;
      } ಆಗ (cur != 0);
}
#ಆದರೆ_ಅಂತ್ಯ
