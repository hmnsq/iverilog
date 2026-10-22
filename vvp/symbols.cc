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
بنية مبودي_strings {
      بنية مبودي_strings*next;
      حرف data[64*1024 - حجم(بنية مبودي_strings*)];
};

حرف*علام_table_s::مبودي_strdup(ثابت حرف*str)
{
      unsigned len = strlen(str);
      أكد( (len+1) <= حجم str_chunk->data );

      إذا ( (len+1) > (حجم str_chunk->data - str_used) ) {
	    مبودي_strings*tmp = جديد مبودي_strings;
	    tmp->next = str_chunk;
	    str_chunk = tmp;
	    str_used = 0;
      }

      حرف*res = str_chunk->data + str_used;
      str_used += len + 1;
      strcpy(res, str);
      أرجع res;
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

ثابت unsigned leaf_width = 254;
ثابت unsigned كلي_width = 508;

بنية بشيا_data {
      حرف*مبودي;
      علام_درج_t val;
};

بنية بشيا_كلي {
      منطقي leaf_flag;
      unsigned كدي;
      بنية بشيا_كلي*parent;

      اتحاد {
	    بنية بشيا_data leaf[leaf_width];
	    بنية بشيا_كلي *child[كلي_width];
      };

};

ساكن مضمن حرف* كلي_last_مبودي(بنية بشيا_كلي*كلي)
{
      طالما (كلي->leaf_flag == خطأ)
	    كلي = كلي->child[كلي->كدي-1];

      أرجع كلي->leaf[كلي->كدي-1].مبودي;
}

/*
 * Allocate a new symbol table means creating the table structure and
 * a root node, and initializing the pointers and members of the root
 * node.
 */
علام_table_s::علام_table_s()
{
      سيو = جديد بنية بشيا_كلي;
      سيو->leaf_flag = خطأ;
      سيو->كدي = 0;
      سيو->parent = 0;

      str_chunk = جديد مبودي_strings;
      str_chunk->next = 0;
      str_used = 0;
}

ساكن فراغ delete_علام_كلي(بنية بشيا_كلي*cur)
{
      إذا (! cur->leaf_flag) {
	    لكل (unsigned idx = 0 ;  idx < cur->كدي ;  idx += 1)
		  delete_علام_كلي(cur->child[idx]);
      }

      delete cur;
}

/* Do as split_leaf_ does, but for nodes. */
ساكن فراغ split_كلي(بنية بشيا_كلي*cur)
{
      أكد(!cur->leaf_flag);
      إذا (cur->parent) أكد(! cur->parent->leaf_flag);

      طالما (cur->كدي == كلي_width) {
	      /* Create a new node to hold half the data from cur. */
	    بنية بشيا_كلي *new_كلي = جديد بنية بشيا_كلي;
	    new_كلي->leaf_flag = خطأ;
	    new_كلي->كدي = cur->كدي / 2;
	      /* cur is not root; new_node becomes sibling. */
	    إذا (cur->parent) new_كلي->parent = cur->parent;

	      /* Move the last half of the data from the end of the old node
	       * to the beginning of the new node. At the same time, reduce
	       * the size of the old node. */
	    unsigned idx1 = new_كلي->كدي;
	    unsigned idx2 = cur->كدي;
	    طالما (idx1 > 0) {
		  idx1 -= 1;
		  idx2 -= 1;
		  new_كلي->child[idx1] = cur->child[idx2];
		  new_كلي->child[idx1]->parent = new_كلي;
		  cur->كدي -= 1;
	    }

	    أكد(new_كلي->كدي > 0);
	    أكد(cur->كدي > 0);

	    إذا (cur->parent == 0) {
		    /* cur is root. Move first half of children to another
		     * new node, and put the two new nodes in cur. The plan
		     * here is to make cur into the new root and split its
		     * contents into 2 children. */

		  new_كلي->parent = cur;
		  بنية بشيا_كلي*new2_كلي = جديد بنية بشيا_كلي;
		  new2_كلي->leaf_flag = خطأ;
		  new2_كلي->كدي = cur->كدي;
		  new2_كلي->parent = cur;
		  لكل (unsigned idx = 0; idx < cur->كدي; idx += 1) {
			new2_كلي->child[idx] = cur->child[idx];
			new2_كلي->child[idx]->parent = new2_كلي;
		  }
		  cur->child[0] = new2_كلي;
		  cur->child[1] = new_كلي;
		  cur->كدي = 2;
		    /* no more ancestors, stop the while loop */
		  اكسر;
	    }

	      /* cur is not root. hook new_node to cur->parent. */
	    unsigned idx = 0;
	    طالما (cur->parent->child[idx] != cur) {
		  أكد(idx < cur->parent->كدي);
		  idx += 1;
	    }
	    idx += 1;

	    لكل (unsigned tmp = cur->parent->كدي ;  tmp > idx ;  tmp -= 1) {
		  cur->parent->child[tmp] = cur->parent->child[tmp-1];
	    }

	    cur->parent->child[idx] = new_كلي;
	    cur->parent->كدي += 1;

	      /* check the ancestor */
	    cur = cur->parent;
      }
}

/*
 * This function takes a leaf node and splits in into two. Move half
 * the leaf keys into the new node, and add the new leaf into the
 * parent node.
 */
ساكن بنية بشيا_كلي* split_leaf_(بنية بشيا_كلي*cur)
{
      أكد(cur->leaf_flag);
      أكد(cur->parent);
      أكد(! cur->parent->leaf_flag);

	/* Create a new leaf to hold half the data from the old leaf. */
      بنية بشيا_كلي*new_leaf = جديد بنية بشيا_كلي;
      new_leaf->leaf_flag = true;
      new_leaf->كدي = cur->كدي / 2;
      new_leaf->parent = cur->parent;

	/* Move the last half of the data from the end of the old leaf
	   to the beginning of the new leaf. At the same time, reduce
	   the size of the old leaf. */
      unsigned idx1 = new_leaf->كدي;
      unsigned idx2 = cur->كدي;
      طالما (idx1 > 0) {
	    idx1 -= 1;
	    idx2 -= 1;
	    new_leaf->leaf[idx1] = cur->leaf[idx2];
	    cur->كدي -= 1;
      }

      أكد(new_leaf->كدي > 0);
      أكد(cur->كدي > 0);

      unsigned idx = 0;
      طالما (cur->parent->child[idx] != cur) {
	    أكد(idx < cur->parent->كدي);
	    idx += 1;
      }

      idx += 1;

      لكل (unsigned tmp = cur->parent->كدي ;  tmp > idx ;  tmp -= 1)
	    cur->parent->child[tmp] = cur->parent->child[tmp-1];

      cur->parent->child[idx] = new_leaf;
      cur->parent->كدي += 1;

      إذا (cur->parent->كدي == كلي_width)
	    split_كلي(cur->parent);

      أرجع new_leaf;
}


/*
 * This function searches tree recursively for the key. If the value
 * is not found (and we are at a leaf) then set the key with the given
 * value. If the key is found, set the value only if the force_flag is
 * true.
 */

علام_درج_t علام_table_s::find_درج(بنية بشيا_كلي*cur,
					   ثابت حرف*مبودي, علام_درج_t val,
					   منطقي force_flag)
{
      إذا (cur->leaf_flag) {

	    unsigned idx = 0;
	    لكل (;;) {
		    /* If we run out of keys in the leaf, then add
		       this at the end of the leaf. */
		  إذا (idx == cur->كدي) {
			cur->leaf[idx].مبودي = مبودي_strdup(مبودي);
			cur->leaf[idx].val = val;
			cur->كدي += 1;
			إذا (cur->كدي == leaf_width)
			      split_leaf_(cur);

			أرجع val;
		  }

		  صحيح rc = strcmp(مبودي, cur->leaf[idx].مبودي);

		    /* If we found the key already in the table, then
		       set the new value and break. */
		  إذا (rc == 0) {
			إذا (force_flag)
			      cur->leaf[idx].val = val;
			أرجع cur->leaf[idx].val;
		  }

		    /* If this key goes before the current key, then
		       push all the following entries back and add
		       this key here. */
		  إذا (rc < 0) {
			لكل (unsigned tmp = cur->كدي; tmp > idx; tmp -= 1)
			      cur->leaf[tmp] = cur->leaf[tmp-1];

			cur->leaf[idx].مبودي = مبودي_strdup(مبودي);
			cur->leaf[idx].val = val;
			cur->كدي += 1;
			إذا (cur->كدي == leaf_width)
			      split_leaf_(cur);

			أرجع val;
		  }

		  idx += 1;
	    }

      } وإلا {
	      /* Do a binary search within the inner node. */
	    unsigned min = 0;
	    unsigned max = cur->كدي;
	    unsigned idx = max/2;
	    لكل (;;) {
		  صحيح rc = strcmp(مبودي, كلي_last_مبودي(cur->child[idx]));
		  إذا (rc == 0) {
			أرجع find_درج(cur->child[idx],
					   مبودي, val, force_flag);
		  }

		  إذا (rc > 0) {
			min = idx + 1;
			إذا (min == cur->كدي)
			      أرجع find_درج(cur->child[idx],
						 مبودي, val, force_flag);
			إذا (min == max)
			      أرجع find_درج(cur->child[max],
						 مبودي, val, force_flag);

			idx = min + (max-min)/2;

		  } وإلا  {
			max = idx;
			إذا (idx == min)
			      أرجع find_درج(cur->child[idx],
						 مبودي, val, force_flag);
			idx = min + (max-min)/2;
		  }
	    }
      }

      أكد(0);
      { علام_درج_t tmp;
        tmp.ptr = 0;
        أرجع tmp;
      }
}

فراغ علام_table_s::sym_set_درج(ثابت حرف*مبودي, علام_درج_t val)
{
      إذا (سيو->كدي == 0) {
	      /* Handle the special case that this is the very first
		 value in the symbol table. Create the first leaf node
		 and initialize the pointers. */
	    بنية بشيا_كلي*cur = جديد بنية بشيا_كلي;
	    cur->leaf_flag = true;
	    cur->parent = سيو;
	    cur->كدي = 1;
	    cur->leaf[0].مبودي = مبودي_strdup(مبودي);
	    cur->leaf[0].val = val;

	    سيو->كدي = 1;
	    سيو->child[0] = cur;
      } وإلا {
	    find_درج(سيو, مبودي, val, true);
      }
}

علام_درج_t علام_table_s::sym_get_درج(ثابت حرف*مبودي)
{
      علام_درج_t def;
      def.ptr = 0;

      إذا (سيو->كدي == 0) {
	      /* Handle the special case that this is the very first
		 value in the symbol table. Create the first leaf node
		 and initialize the pointers. */
	    بنية بشيا_كلي*cur = جديد بنية بشيا_كلي;
	    cur->leaf_flag = true;
	    cur->parent = سيو;
	    cur->كدي = 1;
	    cur->leaf[0].مبودي = مبودي_strdup(مبودي);
	    cur->leaf[0].val = def;

	    سيو->كدي = 1;
	    سيو->child[0] = cur;
	    أرجع cur->leaf[0].val;
      } وإلا {
	    أرجع find_درج(سيو, مبودي, def, خطأ);
      }
}

علام_table_s::~علام_table_s()
{
      delete_علام_كلي(سيو);
      طالما (str_chunk) {
	    مبودي_strings*tmp = str_chunk;
	    str_chunk = tmp->next;
	    delete tmp;
      }
}
