/*
 * Copyright (c) 2014-2026 Stephen Williams (steve@icarus.com)
 * Copyright (c) 2014 CERN
 * @author Maciej Suminski <maciej.suminski@cern.ch>
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

#ifndef ARRAY_COMMON_H
#define ARRAY_COMMON_H

#ଅନ୍ତର୍ଭାଵ "vpi_priv.h"

ସଂରଚନା __vpiArrayIterator : ସର୍ବସାଧାରଣ __vpiHandle {
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_type_code(ଶୂନ୍ୟ) ଅଚଳ override
      { ଫେରନ୍ତୁ vpiIterator; }

      vpiHandle vpi_index(ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx) override;
      free_object_fun_t free_object_fun(ଶୂନ୍ୟ) override;

      ସଂରଚନା __vpiArrayBase*array;
      unsigned next;
};

ସଂରଚନା __vpiArrayIndex : ସର୍ବସାଧାରଣ __vpiHandle {
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_type_code(ଶୂନ୍ୟ) ଅଚଳ override
      { ଫେରନ୍ତୁ vpiIterator; }

      vpiHandle vpi_iterate(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code) override;
      vpiHandle vpi_index(ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx) override;
      free_object_fun_t free_object_fun(ଶୂନ୍ୟ) override;

      __vpiDecConst *index;
      unsigned done;
};

/*
 * The vpiArrayWord is magic. It is used as the handle to return when
 * vpi code tries to index or scan an array of variable words. The
 * array word handle contains no actual data. It is just a hook for
 * the vpi methods and to point to the parent.
 *
 * How the point to the parent works is tricky. The vpiArrayWord
 * objects for an array are themselves allocated as an array. All the
 * ArrayWord objects in the array have a word0 that points to the base
 * of the array. Thus, the position into the array (and the index into
 * the memory) is calculated by subtracting word0 from the ArrayWord
 * pointer.
 *
 * To then get to the parent, use word0[-1].parent.
 *
 * The vpiArrayWord is also used as a handle for the index (vpiIndex)
 * for the word. To make that work, return the pointer to the as_index
 * member instead of the as_word member. The result is a different set
 * of vpi functions is bound to the same structure. All the details
 * for the word also apply when treating this as an index.
 */
ସଂରଚନା __vpiArrayWord {
      ସଂରଚନା as_word_t : ସର୍ବସାଧାରଣ __vpiHandle {
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_type_code(ଶୂନ୍ୟ) ଅଚଳ override { ଫେରନ୍ତୁ vpiMemoryWord; }
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା vpi_get(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code) override;
	    ଅକ୍ଷର*vpi_get_str(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code) override;
	    ଶୂନ୍ୟ vpi_get_ମୂଲ୍ୟ(p_vpi_ମୂଲ୍ୟ vp) override;
	    vpiHandle vpi_put_ମୂଲ୍ୟ(p_vpi_ମୂଲ୍ୟ vp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags) override;
	    vpiHandle vpi_handle(ପୂର୍ଣ୍ଣସଂଖ୍ୟା code) override;
      } as_word;

      ସଂରଚନା as_index_t : ସର୍ବସାଧାରଣ __vpiHandle {
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_type_code(ଶୂନ୍ୟ) ଅଚଳ override { ଫେରନ୍ତୁ vpiIndex; }
	    ଶୂନ୍ୟ vpi_get_ମୂଲ୍ୟ(p_vpi_ମୂଲ୍ୟ val) override;
      } as_index;

      ସଂଘ {
	    ସଂରଚନା __vpiArrayBase*parent;
	    ସଂରଚନା __vpiArrayWord*word0;
      };

      ଧାଡ଼ିଭିତରେ unsigned get_index() ଅଚଳ { ଫେରନ୍ତୁ this - word0; }
      ଧାଡ଼ିଭିତରେ ସଂରଚନା __vpiArrayBase*get_parent() ଅଚଳ { ଫେରନ୍ତୁ (word0 - 1)->parent; }
};

ସଂରଚନା __vpiArrayWord*array_var_word_from_handle(vpiHandle ref);
ସଂରଚନା __vpiArrayWord*array_var_index_from_handle(vpiHandle ref);

#endif /* ARRAY_COMMON_H */
