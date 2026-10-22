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
ಸಂರಚನಾ ಕೀಲಿ_strings {
      ಸಂರಚನಾ ಕೀಲಿ_strings*next;
      ವರ್ಣ data[64*1024 - ಪರಿಮಾಣ(ಸಂರಚನಾ ಕೀಲಿ_strings*)];
};

ವರ್ಣ*ಚಿಹ್ನೆ_table_s::ಕೀಲಿ_strdup(ಧ್ರುವ ವರ್ಣ*str)
{
      unsigned len = strlen(str);
      ಪ್ರತಿಜ್ಞಾ( (len+1) <= ಪರಿಮಾಣ str_chunk->data );

      ಯದಿ ( (len+1) > (ಪರಿಮಾಣ str_chunk->data - str_used) ) {
	    ಕೀಲಿ_strings*tmp = ನೂತನ ಕೀಲಿ_strings;
	    tmp->next = str_chunk;
	    str_chunk = tmp;
	    str_used = 0;
      }

      ವರ್ಣ*res = str_chunk->data + str_used;
      str_used += len + 1;
      strcpy(res, str);
      ಪ್ರತ್ಯಾಗಮ res;
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

ಧ್ರುವ unsigned leaf_width = 254;
ಧ್ರುವ unsigned ಗಂಟು_width = 508;

ಸಂರಚನಾ ವೃಕ್ಷ_data {
      ವರ್ಣ*ಕೀಲಿ;
      ಚಿಹ್ನೆ_ಮೌಲ್ಯ_t val;
};

ಸಂರಚನಾ ವೃಕ್ಷ_ಗಂಟು {
      ತಾರ್ಕಿಕ leaf_flag;
      unsigned ಎಣಿಕೆ;
      ಸಂರಚನಾ ವೃಕ್ಷ_ಗಂಟು*parent;

      ಸಙ್ಘ {
	    ಸಂರಚನಾ ವೃಕ್ಷ_data leaf[leaf_width];
	    ಸಂರಚನಾ ವೃಕ್ಷ_ಗಂಟು *child[ಗಂಟು_width];
      };

};

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ವರ್ಣ* ಗಂಟು_last_ಕೀಲಿ(ಸಂರಚನಾ ವೃಕ್ಷ_ಗಂಟು*ಗಂಟು)
{
      ಯಾವತ್ (ಗಂಟು->leaf_flag == ಅಸತ್ಯ)
	    ಗಂಟು = ಗಂಟು->child[ಗಂಟು->ಎಣಿಕೆ-1];

      ಪ್ರತ್ಯಾಗಮ ಗಂಟು->leaf[ಗಂಟು->ಎಣಿಕೆ-1].ಕೀಲಿ;
}

/*
 * Allocate a new symbol table means creating the table structure and
 * a root node, and initializing the pointers and members of the root
 * node.
 */
ಚಿಹ್ನೆ_table_s::ಚಿಹ್ನೆ_table_s()
{
      ಬೇರು = ನೂತನ ಸಂರಚನಾ ವೃಕ್ಷ_ಗಂಟು;
      ಬೇರು->leaf_flag = ಅಸತ್ಯ;
      ಬೇರು->ಎಣಿಕೆ = 0;
      ಬೇರು->parent = 0;

      str_chunk = ನೂತನ ಕೀಲಿ_strings;
      str_chunk->next = 0;
      str_used = 0;
}

ಸ್ಥಿರ ಶೂನ್ಯ delete_ಚಿಹ್ನೆ_ಗಂಟು(ಸಂರಚನಾ ವೃಕ್ಷ_ಗಂಟು*cur)
{
      ಯದಿ (! cur->leaf_flag) {
	    ಪ್ರತಿ (unsigned idx = 0 ;  idx < cur->ಎಣಿಕೆ ;  idx += 1)
		  delete_ಚಿಹ್ನೆ_ಗಂಟು(cur->child[idx]);
      }

      delete cur;
}

/* Do as split_leaf_ does, but for nodes. */
ಸ್ಥಿರ ಶೂನ್ಯ split_ಗಂಟು(ಸಂರಚನಾ ವೃಕ್ಷ_ಗಂಟು*cur)
{
      ಪ್ರತಿಜ್ಞಾ(!cur->leaf_flag);
      ಯದಿ (cur->parent) ಪ್ರತಿಜ್ಞಾ(! cur->parent->leaf_flag);

      ಯಾವತ್ (cur->ಎಣಿಕೆ == ಗಂಟು_width) {
	      /* Create a new node to hold half the data from cur. */
	    ಸಂರಚನಾ ವೃಕ್ಷ_ಗಂಟು *new_ಗಂಟು = ನೂತನ ಸಂರಚನಾ ವೃಕ್ಷ_ಗಂಟು;
	    new_ಗಂಟು->leaf_flag = ಅಸತ್ಯ;
	    new_ಗಂಟು->ಎಣಿಕೆ = cur->ಎಣಿಕೆ / 2;
	      /* cur is not root; new_node becomes sibling. */
	    ಯದಿ (cur->parent) new_ಗಂಟು->parent = cur->parent;

	      /* Move the last half of the data from the end of the old node
	       * to the beginning of the new node. At the same time, reduce
	       * the size of the old node. */
	    unsigned idx1 = new_ಗಂಟು->ಎಣಿಕೆ;
	    unsigned idx2 = cur->ಎಣಿಕೆ;
	    ಯಾವತ್ (idx1 > 0) {
		  idx1 -= 1;
		  idx2 -= 1;
		  new_ಗಂಟು->child[idx1] = cur->child[idx2];
		  new_ಗಂಟು->child[idx1]->parent = new_ಗಂಟು;
		  cur->ಎಣಿಕೆ -= 1;
	    }

	    ಪ್ರತಿಜ್ಞಾ(new_ಗಂಟು->ಎಣಿಕೆ > 0);
	    ಪ್ರತಿಜ್ಞಾ(cur->ಎಣಿಕೆ > 0);

	    ಯದಿ (cur->parent == 0) {
		    /* cur is root. Move first half of children to another
		     * new node, and put the two new nodes in cur. The plan
		     * here is to make cur into the new root and split its
		     * contents into 2 children. */

		  new_ಗಂಟು->parent = cur;
		  ಸಂರಚನಾ ವೃಕ್ಷ_ಗಂಟು*new2_ಗಂಟು = ನೂತನ ಸಂರಚನಾ ವೃಕ್ಷ_ಗಂಟು;
		  new2_ಗಂಟು->leaf_flag = ಅಸತ್ಯ;
		  new2_ಗಂಟು->ಎಣಿಕೆ = cur->ಎಣಿಕೆ;
		  new2_ಗಂಟು->parent = cur;
		  ಪ್ರತಿ (unsigned idx = 0; idx < cur->ಎಣಿಕೆ; idx += 1) {
			new2_ಗಂಟು->child[idx] = cur->child[idx];
			new2_ಗಂಟು->child[idx]->parent = new2_ಗಂಟು;
		  }
		  cur->child[0] = new2_ಗಂಟು;
		  cur->child[1] = new_ಗಂಟು;
		  cur->ಎಣಿಕೆ = 2;
		    /* no more ancestors, stop the while loop */
		  ಭಙ್ಗ;
	    }

	      /* cur is not root. hook new_node to cur->parent. */
	    unsigned idx = 0;
	    ಯಾವತ್ (cur->parent->child[idx] != cur) {
		  ಪ್ರತಿಜ್ಞಾ(idx < cur->parent->ಎಣಿಕೆ);
		  idx += 1;
	    }
	    idx += 1;

	    ಪ್ರತಿ (unsigned tmp = cur->parent->ಎಣಿಕೆ ;  tmp > idx ;  tmp -= 1) {
		  cur->parent->child[tmp] = cur->parent->child[tmp-1];
	    }

	    cur->parent->child[idx] = new_ಗಂಟು;
	    cur->parent->ಎಣಿಕೆ += 1;

	      /* check the ancestor */
	    cur = cur->parent;
      }
}

/*
 * This function takes a leaf node and splits in into two. Move half
 * the leaf keys into the new node, and add the new leaf into the
 * parent node.
 */
ಸ್ಥಿರ ಸಂರಚನಾ ವೃಕ್ಷ_ಗಂಟು* split_leaf_(ಸಂರಚನಾ ವೃಕ್ಷ_ಗಂಟು*cur)
{
      ಪ್ರತಿಜ್ಞಾ(cur->leaf_flag);
      ಪ್ರತಿಜ್ಞಾ(cur->parent);
      ಪ್ರತಿಜ್ಞಾ(! cur->parent->leaf_flag);

	/* Create a new leaf to hold half the data from the old leaf. */
      ಸಂರಚನಾ ವೃಕ್ಷ_ಗಂಟು*new_leaf = ನೂತನ ಸಂರಚನಾ ವೃಕ್ಷ_ಗಂಟು;
      new_leaf->leaf_flag = ಸತ್ಯ;
      new_leaf->ಎಣಿಕೆ = cur->ಎಣಿಕೆ / 2;
      new_leaf->parent = cur->parent;

	/* Move the last half of the data from the end of the old leaf
	   to the beginning of the new leaf. At the same time, reduce
	   the size of the old leaf. */
      unsigned idx1 = new_leaf->ಎಣಿಕೆ;
      unsigned idx2 = cur->ಎಣಿಕೆ;
      ಯಾವತ್ (idx1 > 0) {
	    idx1 -= 1;
	    idx2 -= 1;
	    new_leaf->leaf[idx1] = cur->leaf[idx2];
	    cur->ಎಣಿಕೆ -= 1;
      }

      ಪ್ರತಿಜ್ಞಾ(new_leaf->ಎಣಿಕೆ > 0);
      ಪ್ರತಿಜ್ಞಾ(cur->ಎಣಿಕೆ > 0);

      unsigned idx = 0;
      ಯಾವತ್ (cur->parent->child[idx] != cur) {
	    ಪ್ರತಿಜ್ಞಾ(idx < cur->parent->ಎಣಿಕೆ);
	    idx += 1;
      }

      idx += 1;

      ಪ್ರತಿ (unsigned tmp = cur->parent->ಎಣಿಕೆ ;  tmp > idx ;  tmp -= 1)
	    cur->parent->child[tmp] = cur->parent->child[tmp-1];

      cur->parent->child[idx] = new_leaf;
      cur->parent->ಎಣಿಕೆ += 1;

      ಯದಿ (cur->parent->ಎಣಿಕೆ == ಗಂಟು_width)
	    split_ಗಂಟು(cur->parent);

      ಪ್ರತ್ಯಾಗಮ new_leaf;
}


/*
 * This function searches tree recursively for the key. If the value
 * is not found (and we are at a leaf) then set the key with the given
 * value. If the key is found, set the value only if the force_flag is
 * true.
 */

ಚಿಹ್ನೆ_ಮೌಲ್ಯ_t ಚಿಹ್ನೆ_table_s::find_ಮೌಲ್ಯ(ಸಂರಚನಾ ವೃಕ್ಷ_ಗಂಟು*cur,
					   ಧ್ರುವ ವರ್ಣ*ಕೀಲಿ, ಚಿಹ್ನೆ_ಮೌಲ್ಯ_t val,
					   ತಾರ್ಕಿಕ force_flag)
{
      ಯದಿ (cur->leaf_flag) {

	    unsigned idx = 0;
	    ಪ್ರತಿ (;;) {
		    /* If we run out of keys in the leaf, then add
		       this at the end of the leaf. */
		  ಯದಿ (idx == cur->ಎಣಿಕೆ) {
			cur->leaf[idx].ಕೀಲಿ = ಕೀಲಿ_strdup(ಕೀಲಿ);
			cur->leaf[idx].val = val;
			cur->ಎಣಿಕೆ += 1;
			ಯದಿ (cur->ಎಣಿಕೆ == leaf_width)
			      split_leaf_(cur);

			ಪ್ರತ್ಯಾಗಮ val;
		  }

		  ಪೂರ್ಣಾಙ್ಕ rc = strcmp(ಕೀಲಿ, cur->leaf[idx].ಕೀಲಿ);

		    /* If we found the key already in the table, then
		       set the new value and break. */
		  ಯದಿ (rc == 0) {
			ಯದಿ (force_flag)
			      cur->leaf[idx].val = val;
			ಪ್ರತ್ಯಾಗಮ cur->leaf[idx].val;
		  }

		    /* If this key goes before the current key, then
		       push all the following entries back and add
		       this key here. */
		  ಯದಿ (rc < 0) {
			ಪ್ರತಿ (unsigned tmp = cur->ಎಣಿಕೆ; tmp > idx; tmp -= 1)
			      cur->leaf[tmp] = cur->leaf[tmp-1];

			cur->leaf[idx].ಕೀಲಿ = ಕೀಲಿ_strdup(ಕೀಲಿ);
			cur->leaf[idx].val = val;
			cur->ಎಣಿಕೆ += 1;
			ಯದಿ (cur->ಎಣಿಕೆ == leaf_width)
			      split_leaf_(cur);

			ಪ್ರತ್ಯಾಗಮ val;
		  }

		  idx += 1;
	    }

      } ಅನ್ಯಥಾ {
	      /* Do a binary search within the inner node. */
	    unsigned min = 0;
	    unsigned max = cur->ಎಣಿಕೆ;
	    unsigned idx = max/2;
	    ಪ್ರತಿ (;;) {
		  ಪೂರ್ಣಾಙ್ಕ rc = strcmp(ಕೀಲಿ, ಗಂಟು_last_ಕೀಲಿ(cur->child[idx]));
		  ಯದಿ (rc == 0) {
			ಪ್ರತ್ಯಾಗಮ find_ಮೌಲ್ಯ(cur->child[idx],
					   ಕೀಲಿ, val, force_flag);
		  }

		  ಯದಿ (rc > 0) {
			min = idx + 1;
			ಯದಿ (min == cur->ಎಣಿಕೆ)
			      ಪ್ರತ್ಯಾಗಮ find_ಮೌಲ್ಯ(cur->child[idx],
						 ಕೀಲಿ, val, force_flag);
			ಯದಿ (min == max)
			      ಪ್ರತ್ಯಾಗಮ find_ಮೌಲ್ಯ(cur->child[max],
						 ಕೀಲಿ, val, force_flag);

			idx = min + (max-min)/2;

		  } ಅನ್ಯಥಾ  {
			max = idx;
			ಯದಿ (idx == min)
			      ಪ್ರತ್ಯಾಗಮ find_ಮೌಲ್ಯ(cur->child[idx],
						 ಕೀಲಿ, val, force_flag);
			idx = min + (max-min)/2;
		  }
	    }
      }

      ಪ್ರತಿಜ್ಞಾ(0);
      { ಚಿಹ್ನೆ_ಮೌಲ್ಯ_t tmp;
        tmp.ptr = 0;
        ಪ್ರತ್ಯಾಗಮ tmp;
      }
}

ಶೂನ್ಯ ಚಿಹ್ನೆ_table_s::sym_set_ಮೌಲ್ಯ(ಧ್ರುವ ವರ್ಣ*ಕೀಲಿ, ಚಿಹ್ನೆ_ಮೌಲ್ಯ_t val)
{
      ಯದಿ (ಬೇರು->ಎಣಿಕೆ == 0) {
	      /* Handle the special case that this is the very first
		 value in the symbol table. Create the first leaf node
		 and initialize the pointers. */
	    ಸಂರಚನಾ ವೃಕ್ಷ_ಗಂಟು*cur = ನೂತನ ಸಂರಚನಾ ವೃಕ್ಷ_ಗಂಟು;
	    cur->leaf_flag = ಸತ್ಯ;
	    cur->parent = ಬೇರು;
	    cur->ಎಣಿಕೆ = 1;
	    cur->leaf[0].ಕೀಲಿ = ಕೀಲಿ_strdup(ಕೀಲಿ);
	    cur->leaf[0].val = val;

	    ಬೇರು->ಎಣಿಕೆ = 1;
	    ಬೇರು->child[0] = cur;
      } ಅನ್ಯಥಾ {
	    find_ಮೌಲ್ಯ(ಬೇರು, ಕೀಲಿ, val, ಸತ್ಯ);
      }
}

ಚಿಹ್ನೆ_ಮೌಲ್ಯ_t ಚಿಹ್ನೆ_table_s::sym_get_ಮೌಲ್ಯ(ಧ್ರುವ ವರ್ಣ*ಕೀಲಿ)
{
      ಚಿಹ್ನೆ_ಮೌಲ್ಯ_t def;
      def.ptr = 0;

      ಯದಿ (ಬೇರು->ಎಣಿಕೆ == 0) {
	      /* Handle the special case that this is the very first
		 value in the symbol table. Create the first leaf node
		 and initialize the pointers. */
	    ಸಂರಚನಾ ವೃಕ್ಷ_ಗಂಟು*cur = ನೂತನ ಸಂರಚನಾ ವೃಕ್ಷ_ಗಂಟು;
	    cur->leaf_flag = ಸತ್ಯ;
	    cur->parent = ಬೇರು;
	    cur->ಎಣಿಕೆ = 1;
	    cur->leaf[0].ಕೀಲಿ = ಕೀಲಿ_strdup(ಕೀಲಿ);
	    cur->leaf[0].val = def;

	    ಬೇರು->ಎಣಿಕೆ = 1;
	    ಬೇರು->child[0] = cur;
	    ಪ್ರತ್ಯಾಗಮ cur->leaf[0].val;
      } ಅನ್ಯಥಾ {
	    ಪ್ರತ್ಯಾಗಮ find_ಮೌಲ್ಯ(ಬೇರು, ಕೀಲಿ, def, ಅಸತ್ಯ);
      }
}

ಚಿಹ್ನೆ_table_s::~ಚಿಹ್ನೆ_table_s()
{
      delete_ಚಿಹ್ನೆ_ಗಂಟು(ಬೇರು);
      ಯಾವತ್ (str_chunk) {
	    ಕೀಲಿ_strings*tmp = str_chunk;
	    str_chunk = tmp->next;
	    delete tmp;
      }
}
