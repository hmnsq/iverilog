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
بنية مفتاح_strings {
      بنية مفتاح_strings*next;
      حرف data[64*1024 - حجم(بنية مفتاح_strings*)];
};

حرف*رمز_table_s::مفتاح_strdup(ثابت حرف*str)
{
      unsigned len = strlen(str);
      أكد( (len+1) <= حجم str_chunk->data );

      إذا ( (len+1) > (حجم str_chunk->data - str_used) ) {
	    مفتاح_strings*tmp = جديد مفتاح_strings;
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
ثابت unsigned عقدة_width = 508;

بنية شجرة_data {
      حرف*مفتاح;
      رمز_قيمة_t val;
};

بنية شجرة_عقدة {
      منطقي leaf_flag;
      unsigned عدد;
      بنية شجرة_عقدة*parent;

      اتحاد {
	    بنية شجرة_data leaf[leaf_width];
	    بنية شجرة_عقدة *child[عقدة_width];
      };

};

ساكن مضمن حرف* عقدة_last_مفتاح(بنية شجرة_عقدة*عقدة)
{
      طالما (عقدة->leaf_flag == خطأ)
	    عقدة = عقدة->child[عقدة->عدد-1];

      أرجع عقدة->leaf[عقدة->عدد-1].مفتاح;
}

/*
 * Allocate a new symbol table means creating the table structure and
 * a root node, and initializing the pointers and members of the root
 * node.
 */
رمز_table_s::رمز_table_s()
{
      جذر = جديد بنية شجرة_عقدة;
      جذر->leaf_flag = خطأ;
      جذر->عدد = 0;
      جذر->parent = 0;

      str_chunk = جديد مفتاح_strings;
      str_chunk->next = 0;
      str_used = 0;
}

ساكن فراغ delete_رمز_عقدة(بنية شجرة_عقدة*cur)
{
      إذا (! cur->leaf_flag) {
	    لكل (unsigned idx = 0 ;  idx < cur->عدد ;  idx += 1)
		  delete_رمز_عقدة(cur->child[idx]);
      }

      delete cur;
}

/* Do as split_leaf_ does, but for nodes. */
ساكن فراغ split_عقدة(بنية شجرة_عقدة*cur)
{
      أكد(!cur->leaf_flag);
      إذا (cur->parent) أكد(! cur->parent->leaf_flag);

      طالما (cur->عدد == عقدة_width) {
	      /* Create a new node to hold half the data from cur. */
	    بنية شجرة_عقدة *new_عقدة = جديد بنية شجرة_عقدة;
	    new_عقدة->leaf_flag = خطأ;
	    new_عقدة->عدد = cur->عدد / 2;
	      /* cur is not root; new_node becomes sibling. */
	    إذا (cur->parent) new_عقدة->parent = cur->parent;

	      /* Move the last half of the data from the end of the old node
	       * to the beginning of the new node. At the same time, reduce
	       * the size of the old node. */
	    unsigned idx1 = new_عقدة->عدد;
	    unsigned idx2 = cur->عدد;
	    طالما (idx1 > 0) {
		  idx1 -= 1;
		  idx2 -= 1;
		  new_عقدة->child[idx1] = cur->child[idx2];
		  new_عقدة->child[idx1]->parent = new_عقدة;
		  cur->عدد -= 1;
	    }

	    أكد(new_عقدة->عدد > 0);
	    أكد(cur->عدد > 0);

	    إذا (cur->parent == 0) {
		    /* cur is root. Move first half of children to another
		     * new node, and put the two new nodes in cur. The plan
		     * here is to make cur into the new root and split its
		     * contents into 2 children. */

		  new_عقدة->parent = cur;
		  بنية شجرة_عقدة*new2_عقدة = جديد بنية شجرة_عقدة;
		  new2_عقدة->leaf_flag = خطأ;
		  new2_عقدة->عدد = cur->عدد;
		  new2_عقدة->parent = cur;
		  لكل (unsigned idx = 0; idx < cur->عدد; idx += 1) {
			new2_عقدة->child[idx] = cur->child[idx];
			new2_عقدة->child[idx]->parent = new2_عقدة;
		  }
		  cur->child[0] = new2_عقدة;
		  cur->child[1] = new_عقدة;
		  cur->عدد = 2;
		    /* no more ancestors, stop the while loop */
		  اكسر;
	    }

	      /* cur is not root. hook new_node to cur->parent. */
	    unsigned idx = 0;
	    طالما (cur->parent->child[idx] != cur) {
		  أكد(idx < cur->parent->عدد);
		  idx += 1;
	    }
	    idx += 1;

	    لكل (unsigned tmp = cur->parent->عدد ;  tmp > idx ;  tmp -= 1) {
		  cur->parent->child[tmp] = cur->parent->child[tmp-1];
	    }

	    cur->parent->child[idx] = new_عقدة;
	    cur->parent->عدد += 1;

	      /* check the ancestor */
	    cur = cur->parent;
      }
}

/*
 * This function takes a leaf node and splits in into two. Move half
 * the leaf keys into the new node, and add the new leaf into the
 * parent node.
 */
ساكن بنية شجرة_عقدة* split_leaf_(بنية شجرة_عقدة*cur)
{
      أكد(cur->leaf_flag);
      أكد(cur->parent);
      أكد(! cur->parent->leaf_flag);

	/* Create a new leaf to hold half the data from the old leaf. */
      بنية شجرة_عقدة*new_leaf = جديد بنية شجرة_عقدة;
      new_leaf->leaf_flag = true;
      new_leaf->عدد = cur->عدد / 2;
      new_leaf->parent = cur->parent;

	/* Move the last half of the data from the end of the old leaf
	   to the beginning of the new leaf. At the same time, reduce
	   the size of the old leaf. */
      unsigned idx1 = new_leaf->عدد;
      unsigned idx2 = cur->عدد;
      طالما (idx1 > 0) {
	    idx1 -= 1;
	    idx2 -= 1;
	    new_leaf->leaf[idx1] = cur->leaf[idx2];
	    cur->عدد -= 1;
      }

      أكد(new_leaf->عدد > 0);
      أكد(cur->عدد > 0);

      unsigned idx = 0;
      طالما (cur->parent->child[idx] != cur) {
	    أكد(idx < cur->parent->عدد);
	    idx += 1;
      }

      idx += 1;

      لكل (unsigned tmp = cur->parent->عدد ;  tmp > idx ;  tmp -= 1)
	    cur->parent->child[tmp] = cur->parent->child[tmp-1];

      cur->parent->child[idx] = new_leaf;
      cur->parent->عدد += 1;

      إذا (cur->parent->عدد == عقدة_width)
	    split_عقدة(cur->parent);

      أرجع new_leaf;
}


/*
 * This function searches tree recursively for the key. If the value
 * is not found (and we are at a leaf) then set the key with the given
 * value. If the key is found, set the value only if the force_flag is
 * true.
 */

رمز_قيمة_t رمز_table_s::find_قيمة(بنية شجرة_عقدة*cur,
					   ثابت حرف*مفتاح, رمز_قيمة_t val,
					   منطقي force_flag)
{
      إذا (cur->leaf_flag) {

	    unsigned idx = 0;
	    لكل (;;) {
		    /* If we run out of keys in the leaf, then add
		       this at the end of the leaf. */
		  إذا (idx == cur->عدد) {
			cur->leaf[idx].مفتاح = مفتاح_strdup(مفتاح);
			cur->leaf[idx].val = val;
			cur->عدد += 1;
			إذا (cur->عدد == leaf_width)
			      split_leaf_(cur);

			أرجع val;
		  }

		  صحيح rc = strcmp(مفتاح, cur->leaf[idx].مفتاح);

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
			لكل (unsigned tmp = cur->عدد; tmp > idx; tmp -= 1)
			      cur->leaf[tmp] = cur->leaf[tmp-1];

			cur->leaf[idx].مفتاح = مفتاح_strdup(مفتاح);
			cur->leaf[idx].val = val;
			cur->عدد += 1;
			إذا (cur->عدد == leaf_width)
			      split_leaf_(cur);

			أرجع val;
		  }

		  idx += 1;
	    }

      } وإلا {
	      /* Do a binary search within the inner node. */
	    unsigned min = 0;
	    unsigned max = cur->عدد;
	    unsigned idx = max/2;
	    لكل (;;) {
		  صحيح rc = strcmp(مفتاح, عقدة_last_مفتاح(cur->child[idx]));
		  إذا (rc == 0) {
			أرجع find_قيمة(cur->child[idx],
					   مفتاح, val, force_flag);
		  }

		  إذا (rc > 0) {
			min = idx + 1;
			إذا (min == cur->عدد)
			      أرجع find_قيمة(cur->child[idx],
						 مفتاح, val, force_flag);
			إذا (min == max)
			      أرجع find_قيمة(cur->child[max],
						 مفتاح, val, force_flag);

			idx = min + (max-min)/2;

		  } وإلا  {
			max = idx;
			إذا (idx == min)
			      أرجع find_قيمة(cur->child[idx],
						 مفتاح, val, force_flag);
			idx = min + (max-min)/2;
		  }
	    }
      }

      أكد(0);
      { رمز_قيمة_t tmp;
        tmp.ptr = 0;
        أرجع tmp;
      }
}

فراغ رمز_table_s::sym_set_قيمة(ثابت حرف*مفتاح, رمز_قيمة_t val)
{
      إذا (جذر->عدد == 0) {
	      /* Handle the special case that this is the very first
		 value in the symbol table. Create the first leaf node
		 and initialize the pointers. */
	    بنية شجرة_عقدة*cur = جديد بنية شجرة_عقدة;
	    cur->leaf_flag = true;
	    cur->parent = جذر;
	    cur->عدد = 1;
	    cur->leaf[0].مفتاح = مفتاح_strdup(مفتاح);
	    cur->leaf[0].val = val;

	    جذر->عدد = 1;
	    جذر->child[0] = cur;
      } وإلا {
	    find_قيمة(جذر, مفتاح, val, true);
      }
}

رمز_قيمة_t رمز_table_s::sym_get_قيمة(ثابت حرف*مفتاح)
{
      رمز_قيمة_t def;
      def.ptr = 0;

      إذا (جذر->عدد == 0) {
	      /* Handle the special case that this is the very first
		 value in the symbol table. Create the first leaf node
		 and initialize the pointers. */
	    بنية شجرة_عقدة*cur = جديد بنية شجرة_عقدة;
	    cur->leaf_flag = true;
	    cur->parent = جذر;
	    cur->عدد = 1;
	    cur->leaf[0].مفتاح = مفتاح_strdup(مفتاح);
	    cur->leaf[0].val = def;

	    جذر->عدد = 1;
	    جذر->child[0] = cur;
	    أرجع cur->leaf[0].val;
      } وإلا {
	    أرجع find_قيمة(جذر, مفتاح, def, خطأ);
      }
}

رمز_table_s::~رمز_table_s()
{
      delete_رمز_عقدة(جذر);
      طالما (str_chunk) {
	    مفتاح_strings*tmp = str_chunk;
	    str_chunk = tmp->next;
	    delete tmp;
      }
}
