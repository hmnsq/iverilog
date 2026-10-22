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
بنية کونچي_strings {
      بنية کونچي_strings*next;
      حرف data[64*1024 - حجم(بنية کونچي_strings*)];
};

حرف*سيمبول_table_s::کونچي_strdup(ثابت حرف*str)
{
      unsigned len = strlen(str);
      أكد( (len+1) <= حجم str_chunk->data );

      إذا ( (len+1) > (حجم str_chunk->data - str_used) ) {
	    کونچي_strings*tmp = جديد کونچي_strings;
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
ثابت unsigned نود_width = 508;

بنية ڤوکوق_data {
      حرف*کونچي;
      سيمبول_نيلاي_t val;
};

بنية نود_ڤوکوق {
      منطقي leaf_flag;
      unsigned کيراءن;
      بنية نود_ڤوکوق*parent;

      اتحاد {
	    بنية ڤوکوق_data leaf[leaf_width];
	    بنية نود_ڤوکوق *child[نود_width];
      };

};

ساكن مضمن حرف* نود_last_کونچي(بنية نود_ڤوکوق*نود)
{
      طالما (نود->leaf_flag == خطأ)
	    نود = نود->child[نود->کيراءن-1];

      أرجع نود->leaf[نود->کيراءن-1].کونچي;
}

/*
 * Allocate a new symbol table means creating the table structure and
 * a root node, and initializing the pointers and members of the root
 * node.
 */
سيمبول_table_s::سيمبول_table_s()
{
      اکر = جديد بنية نود_ڤوکوق;
      اکر->leaf_flag = خطأ;
      اکر->کيراءن = 0;
      اکر->parent = 0;

      str_chunk = جديد کونچي_strings;
      str_chunk->next = 0;
      str_used = 0;
}

ساكن فراغ delete_سيمبول_نود(بنية نود_ڤوکوق*cur)
{
      إذا (! cur->leaf_flag) {
	    لكل (unsigned idx = 0 ;  idx < cur->کيراءن ;  idx += 1)
		  delete_سيمبول_نود(cur->child[idx]);
      }

      delete cur;
}

/* Do as split_leaf_ does, but for nodes. */
ساكن فراغ split_نود(بنية نود_ڤوکوق*cur)
{
      أكد(!cur->leaf_flag);
      إذا (cur->parent) أكد(! cur->parent->leaf_flag);

      طالما (cur->کيراءن == نود_width) {
	      /* Create a new node to hold half the data from cur. */
	    بنية نود_ڤوکوق *new_نود = جديد بنية نود_ڤوکوق;
	    new_نود->leaf_flag = خطأ;
	    new_نود->کيراءن = cur->کيراءن / 2;
	      /* cur is not root; new_node becomes sibling. */
	    إذا (cur->parent) new_نود->parent = cur->parent;

	      /* Move the last half of the data from the end of the old node
	       * to the beginning of the new node. At the same time, reduce
	       * the size of the old node. */
	    unsigned idx1 = new_نود->کيراءن;
	    unsigned idx2 = cur->کيراءن;
	    طالما (idx1 > 0) {
		  idx1 -= 1;
		  idx2 -= 1;
		  new_نود->child[idx1] = cur->child[idx2];
		  new_نود->child[idx1]->parent = new_نود;
		  cur->کيراءن -= 1;
	    }

	    أكد(new_نود->کيراءن > 0);
	    أكد(cur->کيراءن > 0);

	    إذا (cur->parent == 0) {
		    /* cur is root. Move first half of children to another
		     * new node, and put the two new nodes in cur. The plan
		     * here is to make cur into the new root and split its
		     * contents into 2 children. */

		  new_نود->parent = cur;
		  بنية نود_ڤوکوق*new2_نود = جديد بنية نود_ڤوکوق;
		  new2_نود->leaf_flag = خطأ;
		  new2_نود->کيراءن = cur->کيراءن;
		  new2_نود->parent = cur;
		  لكل (unsigned idx = 0; idx < cur->کيراءن; idx += 1) {
			new2_نود->child[idx] = cur->child[idx];
			new2_نود->child[idx]->parent = new2_نود;
		  }
		  cur->child[0] = new2_نود;
		  cur->child[1] = new_نود;
		  cur->کيراءن = 2;
		    /* no more ancestors, stop the while loop */
		  اكسر;
	    }

	      /* cur is not root. hook new_node to cur->parent. */
	    unsigned idx = 0;
	    طالما (cur->parent->child[idx] != cur) {
		  أكد(idx < cur->parent->کيراءن);
		  idx += 1;
	    }
	    idx += 1;

	    لكل (unsigned tmp = cur->parent->کيراءن ;  tmp > idx ;  tmp -= 1) {
		  cur->parent->child[tmp] = cur->parent->child[tmp-1];
	    }

	    cur->parent->child[idx] = new_نود;
	    cur->parent->کيراءن += 1;

	      /* check the ancestor */
	    cur = cur->parent;
      }
}

/*
 * This function takes a leaf node and splits in into two. Move half
 * the leaf keys into the new node, and add the new leaf into the
 * parent node.
 */
ساكن بنية نود_ڤوکوق* split_leaf_(بنية نود_ڤوکوق*cur)
{
      أكد(cur->leaf_flag);
      أكد(cur->parent);
      أكد(! cur->parent->leaf_flag);

	/* Create a new leaf to hold half the data from the old leaf. */
      بنية نود_ڤوکوق*new_leaf = جديد بنية نود_ڤوکوق;
      new_leaf->leaf_flag = true;
      new_leaf->کيراءن = cur->کيراءن / 2;
      new_leaf->parent = cur->parent;

	/* Move the last half of the data from the end of the old leaf
	   to the beginning of the new leaf. At the same time, reduce
	   the size of the old leaf. */
      unsigned idx1 = new_leaf->کيراءن;
      unsigned idx2 = cur->کيراءن;
      طالما (idx1 > 0) {
	    idx1 -= 1;
	    idx2 -= 1;
	    new_leaf->leaf[idx1] = cur->leaf[idx2];
	    cur->کيراءن -= 1;
      }

      أكد(new_leaf->کيراءن > 0);
      أكد(cur->کيراءن > 0);

      unsigned idx = 0;
      طالما (cur->parent->child[idx] != cur) {
	    أكد(idx < cur->parent->کيراءن);
	    idx += 1;
      }

      idx += 1;

      لكل (unsigned tmp = cur->parent->کيراءن ;  tmp > idx ;  tmp -= 1)
	    cur->parent->child[tmp] = cur->parent->child[tmp-1];

      cur->parent->child[idx] = new_leaf;
      cur->parent->کيراءن += 1;

      إذا (cur->parent->کيراءن == نود_width)
	    split_نود(cur->parent);

      أرجع new_leaf;
}


/*
 * This function searches tree recursively for the key. If the value
 * is not found (and we are at a leaf) then set the key with the given
 * value. If the key is found, set the value only if the force_flag is
 * true.
 */

سيمبول_نيلاي_t سيمبول_table_s::find_نيلاي(بنية نود_ڤوکوق*cur,
					   ثابت حرف*کونچي, سيمبول_نيلاي_t val,
					   منطقي force_flag)
{
      إذا (cur->leaf_flag) {

	    unsigned idx = 0;
	    لكل (;;) {
		    /* If we run out of keys in the leaf, then add
		       this at the end of the leaf. */
		  إذا (idx == cur->کيراءن) {
			cur->leaf[idx].کونچي = کونچي_strdup(کونچي);
			cur->leaf[idx].val = val;
			cur->کيراءن += 1;
			إذا (cur->کيراءن == leaf_width)
			      split_leaf_(cur);

			أرجع val;
		  }

		  صحيح rc = strcmp(کونچي, cur->leaf[idx].کونچي);

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
			لكل (unsigned tmp = cur->کيراءن; tmp > idx; tmp -= 1)
			      cur->leaf[tmp] = cur->leaf[tmp-1];

			cur->leaf[idx].کونچي = کونچي_strdup(کونچي);
			cur->leaf[idx].val = val;
			cur->کيراءن += 1;
			إذا (cur->کيراءن == leaf_width)
			      split_leaf_(cur);

			أرجع val;
		  }

		  idx += 1;
	    }

      } وإلا {
	      /* Do a binary search within the inner node. */
	    unsigned min = 0;
	    unsigned max = cur->کيراءن;
	    unsigned idx = max/2;
	    لكل (;;) {
		  صحيح rc = strcmp(کونچي, نود_last_کونچي(cur->child[idx]));
		  إذا (rc == 0) {
			أرجع find_نيلاي(cur->child[idx],
					   کونچي, val, force_flag);
		  }

		  إذا (rc > 0) {
			min = idx + 1;
			إذا (min == cur->کيراءن)
			      أرجع find_نيلاي(cur->child[idx],
						 کونچي, val, force_flag);
			إذا (min == max)
			      أرجع find_نيلاي(cur->child[max],
						 کونچي, val, force_flag);

			idx = min + (max-min)/2;

		  } وإلا  {
			max = idx;
			إذا (idx == min)
			      أرجع find_نيلاي(cur->child[idx],
						 کونچي, val, force_flag);
			idx = min + (max-min)/2;
		  }
	    }
      }

      أكد(0);
      { سيمبول_نيلاي_t tmp;
        tmp.ptr = 0;
        أرجع tmp;
      }
}

فراغ سيمبول_table_s::sym_set_نيلاي(ثابت حرف*کونچي, سيمبول_نيلاي_t val)
{
      إذا (اکر->کيراءن == 0) {
	      /* Handle the special case that this is the very first
		 value in the symbol table. Create the first leaf node
		 and initialize the pointers. */
	    بنية نود_ڤوکوق*cur = جديد بنية نود_ڤوکوق;
	    cur->leaf_flag = true;
	    cur->parent = اکر;
	    cur->کيراءن = 1;
	    cur->leaf[0].کونچي = کونچي_strdup(کونچي);
	    cur->leaf[0].val = val;

	    اکر->کيراءن = 1;
	    اکر->child[0] = cur;
      } وإلا {
	    find_نيلاي(اکر, کونچي, val, true);
      }
}

سيمبول_نيلاي_t سيمبول_table_s::sym_get_نيلاي(ثابت حرف*کونچي)
{
      سيمبول_نيلاي_t def;
      def.ptr = 0;

      إذا (اکر->کيراءن == 0) {
	      /* Handle the special case that this is the very first
		 value in the symbol table. Create the first leaf node
		 and initialize the pointers. */
	    بنية نود_ڤوکوق*cur = جديد بنية نود_ڤوکوق;
	    cur->leaf_flag = true;
	    cur->parent = اکر;
	    cur->کيراءن = 1;
	    cur->leaf[0].کونچي = کونچي_strdup(کونچي);
	    cur->leaf[0].val = def;

	    اکر->کيراءن = 1;
	    اکر->child[0] = cur;
	    أرجع cur->leaf[0].val;
      } وإلا {
	    أرجع find_نيلاي(اکر, کونچي, def, خطأ);
      }
}

سيمبول_table_s::~سيمبول_table_s()
{
      delete_سيمبول_نود(اکر);
      طالما (str_chunk) {
	    کونچي_strings*tmp = str_chunk;
	    str_chunk = tmp->next;
	    delete tmp;
      }
}
