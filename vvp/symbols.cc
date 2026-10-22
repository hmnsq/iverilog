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
ساختار کلید_strings {
      ساختار کلید_strings*next;
      نویسه data[64*1024 - اندازه(ساختار کلید_strings*)];
};

نویسه*نماد_table_s::کلید_strdup(ثابت نویسه*str)
{
      unsigned len = strlen(str);
      أكد( (len+1) <= اندازه str_chunk->data );

      اگر ( (len+1) > (اندازه str_chunk->data - str_used) ) {
	    کلید_strings*tmp = جدید کلید_strings;
	    tmp->next = str_chunk;
	    str_chunk = tmp;
	    str_used = 0;
      }

      نویسه*res = str_chunk->data + str_used;
      str_used += len + 1;
      strcpy(res, str);
      بازگشت res;
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
ثابت unsigned گره_width = 508;

ساختار درخت_data {
      نویسه*کلید;
      نماد_مقدار_t val;
};

ساختار گره_درخت {
      منطقی leaf_flag;
      unsigned شمار;
      ساختار گره_درخت*parent;

      اجتماع {
	    ساختار درخت_data leaf[leaf_width];
	    ساختار گره_درخت *child[گره_width];
      };

};

ایستا درونخطی نویسه* گره_last_کلید(ساختار گره_درخت*گره)
{
      وقتی (گره->leaf_flag == نادرست)
	    گره = گره->child[گره->شمار-1];

      بازگشت گره->leaf[گره->شمار-1].کلید;
}

/*
 * Allocate a new symbol table means creating the table structure and
 * a root node, and initializing the pointers and members of the root
 * node.
 */
نماد_table_s::نماد_table_s()
{
      ریشه = جدید ساختار گره_درخت;
      ریشه->leaf_flag = نادرست;
      ریشه->شمار = 0;
      ریشه->parent = 0;

      str_chunk = جدید کلید_strings;
      str_chunk->next = 0;
      str_used = 0;
}

ایستا تهی delete_نماد_گره(ساختار گره_درخت*cur)
{
      اگر (! cur->leaf_flag) {
	    برای (unsigned idx = 0 ;  idx < cur->شمار ;  idx += 1)
		  delete_نماد_گره(cur->child[idx]);
      }

      delete cur;
}

/* Do as split_leaf_ does, but for nodes. */
ایستا تهی split_گره(ساختار گره_درخت*cur)
{
      أكد(!cur->leaf_flag);
      اگر (cur->parent) أكد(! cur->parent->leaf_flag);

      وقتی (cur->شمار == گره_width) {
	      /* Create a new node to hold half the data from cur. */
	    ساختار گره_درخت *new_گره = جدید ساختار گره_درخت;
	    new_گره->leaf_flag = نادرست;
	    new_گره->شمار = cur->شمار / 2;
	      /* cur is not root; new_node becomes sibling. */
	    اگر (cur->parent) new_گره->parent = cur->parent;

	      /* Move the last half of the data from the end of the old node
	       * to the beginning of the new node. At the same time, reduce
	       * the size of the old node. */
	    unsigned idx1 = new_گره->شمار;
	    unsigned idx2 = cur->شمار;
	    وقتی (idx1 > 0) {
		  idx1 -= 1;
		  idx2 -= 1;
		  new_گره->child[idx1] = cur->child[idx2];
		  new_گره->child[idx1]->parent = new_گره;
		  cur->شمار -= 1;
	    }

	    أكد(new_گره->شمار > 0);
	    أكد(cur->شمار > 0);

	    اگر (cur->parent == 0) {
		    /* cur is root. Move first half of children to another
		     * new node, and put the two new nodes in cur. The plan
		     * here is to make cur into the new root and split its
		     * contents into 2 children. */

		  new_گره->parent = cur;
		  ساختار گره_درخت*new2_گره = جدید ساختار گره_درخت;
		  new2_گره->leaf_flag = نادرست;
		  new2_گره->شمار = cur->شمار;
		  new2_گره->parent = cur;
		  برای (unsigned idx = 0; idx < cur->شمار; idx += 1) {
			new2_گره->child[idx] = cur->child[idx];
			new2_گره->child[idx]->parent = new2_گره;
		  }
		  cur->child[0] = new2_گره;
		  cur->child[1] = new_گره;
		  cur->شمار = 2;
		    /* no more ancestors, stop the while loop */
		  بشکن;
	    }

	      /* cur is not root. hook new_node to cur->parent. */
	    unsigned idx = 0;
	    وقتی (cur->parent->child[idx] != cur) {
		  أكد(idx < cur->parent->شمار);
		  idx += 1;
	    }
	    idx += 1;

	    برای (unsigned tmp = cur->parent->شمار ;  tmp > idx ;  tmp -= 1) {
		  cur->parent->child[tmp] = cur->parent->child[tmp-1];
	    }

	    cur->parent->child[idx] = new_گره;
	    cur->parent->شمار += 1;

	      /* check the ancestor */
	    cur = cur->parent;
      }
}

/*
 * This function takes a leaf node and splits in into two. Move half
 * the leaf keys into the new node, and add the new leaf into the
 * parent node.
 */
ایستا ساختار گره_درخت* split_leaf_(ساختار گره_درخت*cur)
{
      أكد(cur->leaf_flag);
      أكد(cur->parent);
      أكد(! cur->parent->leaf_flag);

	/* Create a new leaf to hold half the data from the old leaf. */
      ساختار گره_درخت*new_leaf = جدید ساختار گره_درخت;
      new_leaf->leaf_flag = درست;
      new_leaf->شمار = cur->شمار / 2;
      new_leaf->parent = cur->parent;

	/* Move the last half of the data from the end of the old leaf
	   to the beginning of the new leaf. At the same time, reduce
	   the size of the old leaf. */
      unsigned idx1 = new_leaf->شمار;
      unsigned idx2 = cur->شمار;
      وقتی (idx1 > 0) {
	    idx1 -= 1;
	    idx2 -= 1;
	    new_leaf->leaf[idx1] = cur->leaf[idx2];
	    cur->شمار -= 1;
      }

      أكد(new_leaf->شمار > 0);
      أكد(cur->شمار > 0);

      unsigned idx = 0;
      وقتی (cur->parent->child[idx] != cur) {
	    أكد(idx < cur->parent->شمار);
	    idx += 1;
      }

      idx += 1;

      برای (unsigned tmp = cur->parent->شمار ;  tmp > idx ;  tmp -= 1)
	    cur->parent->child[tmp] = cur->parent->child[tmp-1];

      cur->parent->child[idx] = new_leaf;
      cur->parent->شمار += 1;

      اگر (cur->parent->شمار == گره_width)
	    split_گره(cur->parent);

      بازگشت new_leaf;
}


/*
 * This function searches tree recursively for the key. If the value
 * is not found (and we are at a leaf) then set the key with the given
 * value. If the key is found, set the value only if the force_flag is
 * true.
 */

نماد_مقدار_t نماد_table_s::find_مقدار(ساختار گره_درخت*cur,
					   ثابت نویسه*کلید, نماد_مقدار_t val,
					   منطقی force_flag)
{
      اگر (cur->leaf_flag) {

	    unsigned idx = 0;
	    برای (;;) {
		    /* If we run out of keys in the leaf, then add
		       this at the end of the leaf. */
		  اگر (idx == cur->شمار) {
			cur->leaf[idx].کلید = کلید_strdup(کلید);
			cur->leaf[idx].val = val;
			cur->شمار += 1;
			اگر (cur->شمار == leaf_width)
			      split_leaf_(cur);

			بازگشت val;
		  }

		  صحیح rc = strcmp(کلید, cur->leaf[idx].کلید);

		    /* If we found the key already in the table, then
		       set the new value and break. */
		  اگر (rc == 0) {
			اگر (force_flag)
			      cur->leaf[idx].val = val;
			بازگشت cur->leaf[idx].val;
		  }

		    /* If this key goes before the current key, then
		       push all the following entries back and add
		       this key here. */
		  اگر (rc < 0) {
			برای (unsigned tmp = cur->شمار; tmp > idx; tmp -= 1)
			      cur->leaf[tmp] = cur->leaf[tmp-1];

			cur->leaf[idx].کلید = کلید_strdup(کلید);
			cur->leaf[idx].val = val;
			cur->شمار += 1;
			اگر (cur->شمار == leaf_width)
			      split_leaf_(cur);

			بازگشت val;
		  }

		  idx += 1;
	    }

      } وگرنه {
	      /* Do a binary search within the inner node. */
	    unsigned min = 0;
	    unsigned max = cur->شمار;
	    unsigned idx = max/2;
	    برای (;;) {
		  صحیح rc = strcmp(کلید, گره_last_کلید(cur->child[idx]));
		  اگر (rc == 0) {
			بازگشت find_مقدار(cur->child[idx],
					   کلید, val, force_flag);
		  }

		  اگر (rc > 0) {
			min = idx + 1;
			اگر (min == cur->شمار)
			      بازگشت find_مقدار(cur->child[idx],
						 کلید, val, force_flag);
			اگر (min == max)
			      بازگشت find_مقدار(cur->child[max],
						 کلید, val, force_flag);

			idx = min + (max-min)/2;

		  } وگرنه  {
			max = idx;
			اگر (idx == min)
			      بازگشت find_مقدار(cur->child[idx],
						 کلید, val, force_flag);
			idx = min + (max-min)/2;
		  }
	    }
      }

      أكد(0);
      { نماد_مقدار_t tmp;
        tmp.ptr = 0;
        بازگشت tmp;
      }
}

تهی نماد_table_s::sym_set_مقدار(ثابت نویسه*کلید, نماد_مقدار_t val)
{
      اگر (ریشه->شمار == 0) {
	      /* Handle the special case that this is the very first
		 value in the symbol table. Create the first leaf node
		 and initialize the pointers. */
	    ساختار گره_درخت*cur = جدید ساختار گره_درخت;
	    cur->leaf_flag = درست;
	    cur->parent = ریشه;
	    cur->شمار = 1;
	    cur->leaf[0].کلید = کلید_strdup(کلید);
	    cur->leaf[0].val = val;

	    ریشه->شمار = 1;
	    ریشه->child[0] = cur;
      } وگرنه {
	    find_مقدار(ریشه, کلید, val, درست);
      }
}

نماد_مقدار_t نماد_table_s::sym_get_مقدار(ثابت نویسه*کلید)
{
      نماد_مقدار_t def;
      def.ptr = 0;

      اگر (ریشه->شمار == 0) {
	      /* Handle the special case that this is the very first
		 value in the symbol table. Create the first leaf node
		 and initialize the pointers. */
	    ساختار گره_درخت*cur = جدید ساختار گره_درخت;
	    cur->leaf_flag = درست;
	    cur->parent = ریشه;
	    cur->شمار = 1;
	    cur->leaf[0].کلید = کلید_strdup(کلید);
	    cur->leaf[0].val = def;

	    ریشه->شمار = 1;
	    ریشه->child[0] = cur;
	    بازگشت cur->leaf[0].val;
      } وگرنه {
	    بازگشت find_مقدار(ریشه, کلید, def, نادرست);
      }
}

نماد_table_s::~نماد_table_s()
{
      delete_نماد_گره(ریشه);
      وقتی (str_chunk) {
	    کلید_strings*tmp = str_chunk;
	    str_chunk = tmp->next;
	    delete tmp;
      }
}
