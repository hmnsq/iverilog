/*
 * Copyright (c) 2001-2013 Stephen Williams (steve@icarus.com)
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

# include  "symbols.h"
# include  <cstring>
# include  <cstdlib>
# include  <cassert>

/*
 * The keys of the symbol table are null terminated strings. Keep them
 * in a string buffer, with the strings separated by a single null,
 * for compact use of memory. This also makes it easy to delete the
 * entire lot of keys, simply by deleting the heaps.
 *
 * The key_strdup() function below allocates the strings from this
 * buffer, possibly making a new buffer if needed.
 */
ರಚನೆ ಕೀಲಿ_ಅಕ್ಷರಸರಣಿಗಳು {
      ರಚನೆ ಕೀಲಿ_ಅಕ್ಷರಸರಣಿಗಳು*ಮುಂದಿನ_2;
      ಅಕ್ಷರ ದತ್ತಾಂಶ[64*1024 - ಗಾತ್ರ(ರಚನೆ ಕೀಲಿ_ಅಕ್ಷರಸರಣಿಗಳು*)];
};

ಅಕ್ಷರ*ಚಿಹ್ನೆ_ಕೋಷ್ಟಕ_s::ಕೀಲಿ_strdup(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*str)
{
      ಚಿಹ್ನೆರಹಿತ len = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(str);
      ಪ್ರತಿಜ್ಞಾ( (len+1) <= ಗಾತ್ರ str_chunk->ದತ್ತಾಂಶ );

      ಆದರೆ ( (len+1) > (ಗಾತ್ರ str_chunk->ದತ್ತಾಂಶ - str_ಬಳಸಿದ) ) {
	    ಕೀಲಿ_ಅಕ್ಷರಸರಣಿಗಳು*tmp = ಹೊಸ ಕೀಲಿ_ಅಕ್ಷರಸರಣಿಗಳು;
	    tmp->ಮುಂದಿನ_2 = str_chunk;
	    str_chunk = tmp;
	    str_ಬಳಸಿದ = 0;
      }

      ಅಕ್ಷರ*res = str_chunk->ದತ್ತಾಂಶ + str_ಬಳಸಿದ;
      str_ಬಳಸಿದ += len + 1;
      strcpy(res, str);
      ಹಿಂತಿರುಗಿ res;
}

/*
 * This is a B-Tree data structure, where there are nodes and
 * leaves.
 *
 * Nodes have a bunch of pointers to children. Each child pointer has
 * associated with it a key that is the largest key referenced by that
 * child. So, if the key being searched for has a value <= the first
 * key of a node, then the value is in the first child.
 *
 * leaves have a sorted table of key-value pairs. The search can use a
 * simple binary search to find an item. Each key represents an item.
 */

ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಎಲೆ_ಅಗಲ = 254;
ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಗಂಟು_ಅಗಲ = 508;

ರಚನೆ ವೃಕ್ಷ_ದತ್ತಾಂಶ {
      ಅಕ್ಷರ*ಕೀಲಿ;
      ಚಿಹ್ನೆ_ಮೌಲ್ಯ_t val;
};

ರಚನೆ ವೃಕ್ಷ_ಗಂಟು {
      ತಾರ್ಕಿಕ ಎಲೆ_flag;
      ಚಿಹ್ನೆರಹಿತ ಎಣಿಕೆ_2;
      ರಚನೆ ವೃಕ್ಷ_ಗಂಟು*ಪೋಷಕ;

      ಒಕ್ಕೂಟ {
	    ರಚನೆ ವೃಕ್ಷ_ದತ್ತಾಂಶ ಎಲೆ[ಎಲೆ_ಅಗಲ];
	    ರಚನೆ ವೃಕ್ಷ_ಗಂಟು *ಮಗು[ಗಂಟು_ಅಗಲ];
      };

};

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಅಕ್ಷರ* ಗಂಟು_ಕೊನೆಯ_ಕೀಲಿ(ರಚನೆ ವೃಕ್ಷ_ಗಂಟು*ಗಂಟು)
{
      ಆಗ (ಗಂಟು->ಎಲೆ_flag == ಸುಳ್ಳು)
	    ಗಂಟು = ಗಂಟು->ಮಗು[ಗಂಟು->ಎಣಿಕೆ_2-1];

      ಹಿಂತಿರುಗಿ ಗಂಟು->ಎಲೆ[ಗಂಟು->ಎಣಿಕೆ_2-1].ಕೀಲಿ;
}

/*
 * Allocate a new symbol table means creating the table structure and
 * a root node, and initializing the pointers and members of the root
 * node.
 */
ಚಿಹ್ನೆ_ಕೋಷ್ಟಕ_s::ಚಿಹ್ನೆ_ಕೋಷ್ಟಕ_s()
{
      ಬೇರು = ಹೊಸ ರಚನೆ ವೃಕ್ಷ_ಗಂಟು;
      ಬೇರು->ಎಲೆ_flag = ಸುಳ್ಳು;
      ಬೇರು->ಎಣಿಕೆ_2 = 0;
      ಬೇರು->ಪೋಷಕ = 0;

      str_chunk = ಹೊಸ ಕೀಲಿ_ಅಕ್ಷರಸರಣಿಗಳು;
      str_chunk->ಮುಂದಿನ_2 = 0;
      str_ಬಳಸಿದ = 0;
}

ಸ್ಥಿರ ಶೂನ್ಯ delete_ಚಿಹ್ನೆ_ಗಂಟು(ರಚನೆ ವೃಕ್ಷ_ಗಂಟು*cur)
{
      ಆದರೆ (! cur->ಎಲೆ_flag) {
	    ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0 ;  idx < cur->ಎಣಿಕೆ_2 ;  idx += 1)
		  delete_ಚಿಹ್ನೆ_ಗಂಟು(cur->ಮಗು[idx]);
      }

      ಅಳಿಸು cur;
}

/* Do as split_leaf_ does, but for nodes. */
ಸ್ಥಿರ ಶೂನ್ಯ ವಿಭಜಿಸು_ಗಂಟು(ರಚನೆ ವೃಕ್ಷ_ಗಂಟು*cur)
{
      ಪ್ರತಿಜ್ಞಾ(!cur->ಎಲೆ_flag);
      ಆದರೆ (cur->ಪೋಷಕ) ಪ್ರತಿಜ್ಞಾ(! cur->ಪೋಷಕ->ಎಲೆ_flag);

      ಆಗ (cur->ಎಣಿಕೆ_2 == ಗಂಟು_ಅಗಲ) {
	      /* Create a new node to hold half the data from cur. */
	    ರಚನೆ ವೃಕ್ಷ_ಗಂಟು *new_ಗಂಟು = ಹೊಸ ರಚನೆ ವೃಕ್ಷ_ಗಂಟು;
	    new_ಗಂಟು->ಎಲೆ_flag = ಸುಳ್ಳು;
	    new_ಗಂಟು->ಎಣಿಕೆ_2 = cur->ಎಣಿಕೆ_2 / 2;
	      /* cur is not root; new_node becomes sibling. */
	    ಆದರೆ (cur->ಪೋಷಕ) new_ಗಂಟು->ಪೋಷಕ = cur->ಪೋಷಕ;

	      /* Move the last half of the data from the end of the old node
	       * to the beginning of the new node. At the same time, reduce
	       * the size of the old node. */
	    ಚಿಹ್ನೆರಹಿತ idx1 = new_ಗಂಟು->ಎಣಿಕೆ_2;
	    ಚಿಹ್ನೆರಹಿತ idx2 = cur->ಎಣಿಕೆ_2;
	    ಆಗ (idx1 > 0) {
		  idx1 -= 1;
		  idx2 -= 1;
		  new_ಗಂಟು->ಮಗು[idx1] = cur->ಮಗು[idx2];
		  new_ಗಂಟು->ಮಗು[idx1]->ಪೋಷಕ = new_ಗಂಟು;
		  cur->ಎಣಿಕೆ_2 -= 1;
	    }

	    ಪ್ರತಿಜ್ಞಾ(new_ಗಂಟು->ಎಣಿಕೆ_2 > 0);
	    ಪ್ರತಿಜ್ಞಾ(cur->ಎಣಿಕೆ_2 > 0);

	    ಆದರೆ (cur->ಪೋಷಕ == 0) {
		    /* cur is root. Move first half of children to another
		     * new node, and put the two new nodes in cur. The plan
		     * here is to make cur into the new root and split its
		     * contents into 2 children. */

		  new_ಗಂಟು->ಪೋಷಕ = cur;
		  ರಚನೆ ವೃಕ್ಷ_ಗಂಟು*new2_ಗಂಟು = ಹೊಸ ರಚನೆ ವೃಕ್ಷ_ಗಂಟು;
		  new2_ಗಂಟು->ಎಲೆ_flag = ಸುಳ್ಳು;
		  new2_ಗಂಟು->ಎಣಿಕೆ_2 = cur->ಎಣಿಕೆ_2;
		  new2_ಗಂಟು->ಪೋಷಕ = cur;
		  ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ idx = 0; idx < cur->ಎಣಿಕೆ_2; idx += 1) {
			new2_ಗಂಟು->ಮಗು[idx] = cur->ಮಗು[idx];
			new2_ಗಂಟು->ಮಗು[idx]->ಪೋಷಕ = new2_ಗಂಟು;
		  }
		  cur->ಮಗು[0] = new2_ಗಂಟು;
		  cur->ಮಗು[1] = new_ಗಂಟು;
		  cur->ಎಣಿಕೆ_2 = 2;
		    /* no more ancestors, stop the while loop */
		  ನಿಲ್ಲಿಸು;
	    }

	      /* cur is not root. hook new_node to cur->parent. */
	    ಚಿಹ್ನೆರಹಿತ idx = 0;
	    ಆಗ (cur->ಪೋಷಕ->ಮಗು[idx] != cur) {
		  ಪ್ರತಿಜ್ಞಾ(idx < cur->ಪೋಷಕ->ಎಣಿಕೆ_2);
		  idx += 1;
	    }
	    idx += 1;

	    ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ tmp = cur->ಪೋಷಕ->ಎಣಿಕೆ_2 ;  tmp > idx ;  tmp -= 1) {
		  cur->ಪೋಷಕ->ಮಗು[tmp] = cur->ಪೋಷಕ->ಮಗು[tmp-1];
	    }

	    cur->ಪೋಷಕ->ಮಗು[idx] = new_ಗಂಟು;
	    cur->ಪೋಷಕ->ಎಣಿಕೆ_2 += 1;

	      /* check the ancestor */
	    cur = cur->ಪೋಷಕ;
      }
}

/*
 * This function takes a leaf node and splits in into two. Move half
 * the leaf keys into the new node, and add the new leaf into the
 * parent node.
 */
ಸ್ಥಿರ ರಚನೆ ವೃಕ್ಷ_ಗಂಟು* ವಿಭಜಿಸು_ಎಲೆ(ರಚನೆ ವೃಕ್ಷ_ಗಂಟು*cur)
{
      ಪ್ರತಿಜ್ಞಾ(cur->ಎಲೆ_flag);
      ಪ್ರತಿಜ್ಞಾ(cur->ಪೋಷಕ);
      ಪ್ರತಿಜ್ಞಾ(! cur->ಪೋಷಕ->ಎಲೆ_flag);

	/* Create a new leaf to hold half the data from the old leaf. */
      ರಚನೆ ವೃಕ್ಷ_ಗಂಟು*new_ಎಲೆ = ಹೊಸ ರಚನೆ ವೃಕ್ಷ_ಗಂಟು;
      new_ಎಲೆ->ಎಲೆ_flag = ನಿಜ;
      new_ಎಲೆ->ಎಣಿಕೆ_2 = cur->ಎಣಿಕೆ_2 / 2;
      new_ಎಲೆ->ಪೋಷಕ = cur->ಪೋಷಕ;

	/* Move the last half of the data from the end of the old leaf
	   to the beginning of the new leaf. At the same time, reduce
	   the size of the old leaf. */
      ಚಿಹ್ನೆರಹಿತ idx1 = new_ಎಲೆ->ಎಣಿಕೆ_2;
      ಚಿಹ್ನೆರಹಿತ idx2 = cur->ಎಣಿಕೆ_2;
      ಆಗ (idx1 > 0) {
	    idx1 -= 1;
	    idx2 -= 1;
	    new_ಎಲೆ->ಎಲೆ[idx1] = cur->ಎಲೆ[idx2];
	    cur->ಎಣಿಕೆ_2 -= 1;
      }

      ಪ್ರತಿಜ್ಞಾ(new_ಎಲೆ->ಎಣಿಕೆ_2 > 0);
      ಪ್ರತಿಜ್ಞಾ(cur->ಎಣಿಕೆ_2 > 0);

      ಚಿಹ್ನೆರಹಿತ idx = 0;
      ಆಗ (cur->ಪೋಷಕ->ಮಗು[idx] != cur) {
	    ಪ್ರತಿಜ್ಞಾ(idx < cur->ಪೋಷಕ->ಎಣಿಕೆ_2);
	    idx += 1;
      }

      idx += 1;

      ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ tmp = cur->ಪೋಷಕ->ಎಣಿಕೆ_2 ;  tmp > idx ;  tmp -= 1)
	    cur->ಪೋಷಕ->ಮಗು[tmp] = cur->ಪೋಷಕ->ಮಗು[tmp-1];

      cur->ಪೋಷಕ->ಮಗು[idx] = new_ಎಲೆ;
      cur->ಪೋಷಕ->ಎಣಿಕೆ_2 += 1;

      ಆದರೆ (cur->ಪೋಷಕ->ಎಣಿಕೆ_2 == ಗಂಟು_ಅಗಲ)
	    ವಿಭಜಿಸು_ಗಂಟು(cur->ಪೋಷಕ);

      ಹಿಂತಿರುಗಿ new_ಎಲೆ;
}


/*
 * This function searches tree recursively for the key. If the value
 * is not found (and we are at a leaf) then set the key with the given
 * value. If the key is found, set the value only if the force_flag is
 * true.
 */

ಚಿಹ್ನೆ_ಮೌಲ್ಯ_t ಚಿಹ್ನೆ_ಕೋಷ್ಟಕ_s::ಕಂಡುಹಿಡಿ_ಮೌಲ್ಯ(ರಚನೆ ವೃಕ್ಷ_ಗಂಟು*cur,
					   ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*ಕೀಲಿ, ಚಿಹ್ನೆ_ಮೌಲ್ಯ_t val,
					   ತಾರ್ಕಿಕ force_flag)
{
      ಆದರೆ (cur->ಎಲೆ_flag) {

	    ಚಿಹ್ನೆರಹಿತ idx = 0;
	    ಗಾಗಿ (;;) {
		    /* If we run out of keys in the leaf, then add
		       this at the end of the leaf. */
		  ಆದರೆ (idx == cur->ಎಣಿಕೆ_2) {
			cur->ಎಲೆ[idx].ಕೀಲಿ = ಕೀಲಿ_strdup(ಕೀಲಿ);
			cur->ಎಲೆ[idx].val = val;
			cur->ಎಣಿಕೆ_2 += 1;
			ಆದರೆ (cur->ಎಣಿಕೆ_2 == ಎಲೆ_ಅಗಲ)
			      ವಿಭಜಿಸು_ಎಲೆ(cur);

			ಹಿಂತಿರುಗಿ val;
		  }

		  ಪೂರ್ಣಾಂಕ rc = strcmp(ಕೀಲಿ, cur->ಎಲೆ[idx].ಕೀಲಿ);

		    /* If we found the key already in the table, then
		       set the new value and break. */
		  ಆದರೆ (rc == 0) {
			ಆದರೆ (force_flag)
			      cur->ಎಲೆ[idx].val = val;
			ಹಿಂತಿರುಗಿ cur->ಎಲೆ[idx].val;
		  }

		    /* If this key goes before the current key, then
		       push all the following entries back and add
		       this key here. */
		  ಆದರೆ (rc < 0) {
			ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ tmp = cur->ಎಣಿಕೆ_2; tmp > idx; tmp -= 1)
			      cur->ಎಲೆ[tmp] = cur->ಎಲೆ[tmp-1];

			cur->ಎಲೆ[idx].ಕೀಲಿ = ಕೀಲಿ_strdup(ಕೀಲಿ);
			cur->ಎಲೆ[idx].val = val;
			cur->ಎಣಿಕೆ_2 += 1;
			ಆದರೆ (cur->ಎಣಿಕೆ_2 == ಎಲೆ_ಅಗಲ)
			      ವಿಭಜಿಸು_ಎಲೆ(cur);

			ಹಿಂತಿರುಗಿ val;
		  }

		  idx += 1;
	    }

      } ಇಲ್ಲದಿದ್ದರೆ {
	      /* Do a binary search within the inner node. */
	    ಚಿಹ್ನೆರಹಿತ min = 0;
	    ಚಿಹ್ನೆರಹಿತ max = cur->ಎಣಿಕೆ_2;
	    ಚಿಹ್ನೆರಹಿತ idx = max/2;
	    ಗಾಗಿ (;;) {
		  ಪೂರ್ಣಾಂಕ rc = strcmp(ಕೀಲಿ, ಗಂಟು_ಕೊನೆಯ_ಕೀಲಿ(cur->ಮಗು[idx]));
		  ಆದರೆ (rc == 0) {
			ಹಿಂತಿರುಗಿ ಕಂಡುಹಿಡಿ_ಮೌಲ್ಯ(cur->ಮಗು[idx],
					   ಕೀಲಿ, val, force_flag);
		  }

		  ಆದರೆ (rc > 0) {
			min = idx + 1;
			ಆದರೆ (min == cur->ಎಣಿಕೆ_2)
			      ಹಿಂತಿರುಗಿ ಕಂಡುಹಿಡಿ_ಮೌಲ್ಯ(cur->ಮಗು[idx],
						 ಕೀಲಿ, val, force_flag);
			ಆದರೆ (min == max)
			      ಹಿಂತಿರುಗಿ ಕಂಡುಹಿಡಿ_ಮೌಲ್ಯ(cur->ಮಗು[max],
						 ಕೀಲಿ, val, force_flag);

			idx = min + (max-min)/2;

		  } ಇಲ್ಲದಿದ್ದರೆ  {
			max = idx;
			ಆದರೆ (idx == min)
			      ಹಿಂತಿರುಗಿ ಕಂಡುಹಿಡಿ_ಮೌಲ್ಯ(cur->ಮಗು[idx],
						 ಕೀಲಿ, val, force_flag);
			idx = min + (max-min)/2;
		  }
	    }
      }

      ಪ್ರತಿಜ್ಞಾ(0);
      { ಚಿಹ್ನೆ_ಮೌಲ್ಯ_t tmp;
        tmp.ptr = 0;
        ಹಿಂತಿರುಗಿ tmp;
      }
}

ಶೂನ್ಯ ಚಿಹ್ನೆ_ಕೋಷ್ಟಕ_s::sym_ಗಣ_ಮೌಲ್ಯ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*ಕೀಲಿ, ಚಿಹ್ನೆ_ಮೌಲ್ಯ_t val)
{
      ಆದರೆ (ಬೇರು->ಎಣಿಕೆ_2 == 0) {
	      /* Handle the special case that this is the very first
		 value in the symbol table. Create the first leaf node
		 and initialize the pointers. */
	    ರಚನೆ ವೃಕ್ಷ_ಗಂಟು*cur = ಹೊಸ ರಚನೆ ವೃಕ್ಷ_ಗಂಟು;
	    cur->ಎಲೆ_flag = ನಿಜ;
	    cur->ಪೋಷಕ = ಬೇರು;
	    cur->ಎಣಿಕೆ_2 = 1;
	    cur->ಎಲೆ[0].ಕೀಲಿ = ಕೀಲಿ_strdup(ಕೀಲಿ);
	    cur->ಎಲೆ[0].val = val;

	    ಬೇರು->ಎಣಿಕೆ_2 = 1;
	    ಬೇರು->ಮಗು[0] = cur;
      } ಇಲ್ಲದಿದ್ದರೆ {
	    ಕಂಡುಹಿಡಿ_ಮೌಲ್ಯ(ಬೇರು, ಕೀಲಿ, val, ನಿಜ);
      }
}

ಚಿಹ್ನೆ_ಮೌಲ್ಯ_t ಚಿಹ್ನೆ_ಕೋಷ್ಟಕ_s::sym_get_ಮೌಲ್ಯ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*ಕೀಲಿ)
{
      ಚಿಹ್ನೆ_ಮೌಲ್ಯ_t def;
      def.ptr = 0;

      ಆದರೆ (ಬೇರು->ಎಣಿಕೆ_2 == 0) {
	      /* Handle the special case that this is the very first
		 value in the symbol table. Create the first leaf node
		 and initialize the pointers. */
	    ರಚನೆ ವೃಕ್ಷ_ಗಂಟು*cur = ಹೊಸ ರಚನೆ ವೃಕ್ಷ_ಗಂಟು;
	    cur->ಎಲೆ_flag = ನಿಜ;
	    cur->ಪೋಷಕ = ಬೇರು;
	    cur->ಎಣಿಕೆ_2 = 1;
	    cur->ಎಲೆ[0].ಕೀಲಿ = ಕೀಲಿ_strdup(ಕೀಲಿ);
	    cur->ಎಲೆ[0].val = def;

	    ಬೇರು->ಎಣಿಕೆ_2 = 1;
	    ಬೇರು->ಮಗು[0] = cur;
	    ಹಿಂತಿರುಗಿ cur->ಎಲೆ[0].val;
      } ಇಲ್ಲದಿದ್ದರೆ {
	    ಹಿಂತಿರುಗಿ ಕಂಡುಹಿಡಿ_ಮೌಲ್ಯ(ಬೇರು, ಕೀಲಿ, def, ಸುಳ್ಳು);
      }
}

ಚಿಹ್ನೆ_ಕೋಷ್ಟಕ_s::~ಚಿಹ್ನೆ_ಕೋಷ್ಟಕ_s()
{
      delete_ಚಿಹ್ನೆ_ಗಂಟು(ಬೇರು);
      ಆಗ (str_chunk) {
	    ಕೀಲಿ_ಅಕ್ಷರಸರಣಿಗಳು*tmp = str_chunk;
	    str_chunk = tmp->ಮುಂದಿನ_2;
	    ಅಳಿಸು tmp;
      }
}
