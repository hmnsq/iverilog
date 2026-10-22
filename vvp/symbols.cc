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
بنية ئاچقۇچ_strings {
      بنية ئاچقۇچ_strings*next;
      حرف data[64*1024 - حجم(بنية ئاچقۇچ_strings*)];
};

حرف*بەلگە_table_s::ئاچقۇچ_strdup(ثابت حرف*str)
{
      unsigned len = strlen(str);
      أكد( (len+1) <= حجم str_chunk->data );

      إذا ( (len+1) > (حجم str_chunk->data - str_used) ) {
	    ئاچقۇچ_strings*tmp = جديد ئاچقۇچ_strings;
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
ثابت unsigned تۈگۈن_width = 508;

بنية دەرەخ_data {
      حرف*ئاچقۇچ;
      بەلگە_قىممەت_t val;
};

بنية دەرەخ_تۈگۈنى {
      منطقي leaf_flag;
      unsigned سانى;
      بنية دەرەخ_تۈگۈنى*parent;

      اتحاد {
	    بنية دەرەخ_data leaf[leaf_width];
	    بنية دەرەخ_تۈگۈنى *child[تۈگۈن_width];
      };

};

ساكن مضمن حرف* تۈگۈن_last_ئاچقۇچ(بنية دەرەخ_تۈگۈنى*تۈگۈن)
{
      طالما (تۈگۈن->leaf_flag == خطأ)
	    تۈگۈن = تۈگۈن->child[تۈگۈن->سانى-1];

      أرجع تۈگۈن->leaf[تۈگۈن->سانى-1].ئاچقۇچ;
}

/*
 * Allocate a new symbol table means creating the table structure and
 * a root node, and initializing the pointers and members of the root
 * node.
 */
بەلگە_table_s::بەلگە_table_s()
{
      يىلتىز = جديد بنية دەرەخ_تۈگۈنى;
      يىلتىز->leaf_flag = خطأ;
      يىلتىز->سانى = 0;
      يىلتىز->parent = 0;

      str_chunk = جديد ئاچقۇچ_strings;
      str_chunk->next = 0;
      str_used = 0;
}

ساكن فراغ delete_بەلگە_تۈگۈن(بنية دەرەخ_تۈگۈنى*cur)
{
      إذا (! cur->leaf_flag) {
	    لكل (unsigned idx = 0 ;  idx < cur->سانى ;  idx += 1)
		  delete_بەلگە_تۈگۈن(cur->child[idx]);
      }

      delete cur;
}

/* Do as split_leaf_ does, but for nodes. */
ساكن فراغ split_تۈگۈن(بنية دەرەخ_تۈگۈنى*cur)
{
      أكد(!cur->leaf_flag);
      إذا (cur->parent) أكد(! cur->parent->leaf_flag);

      طالما (cur->سانى == تۈگۈن_width) {
	      /* Create a new node to hold half the data from cur. */
	    بنية دەرەخ_تۈگۈنى *new_تۈگۈن = جديد بنية دەرەخ_تۈگۈنى;
	    new_تۈگۈن->leaf_flag = خطأ;
	    new_تۈگۈن->سانى = cur->سانى / 2;
	      /* cur is not root; new_node becomes sibling. */
	    إذا (cur->parent) new_تۈگۈن->parent = cur->parent;

	      /* Move the last half of the data from the end of the old node
	       * to the beginning of the new node. At the same time, reduce
	       * the size of the old node. */
	    unsigned idx1 = new_تۈگۈن->سانى;
	    unsigned idx2 = cur->سانى;
	    طالما (idx1 > 0) {
		  idx1 -= 1;
		  idx2 -= 1;
		  new_تۈگۈن->child[idx1] = cur->child[idx2];
		  new_تۈگۈن->child[idx1]->parent = new_تۈگۈن;
		  cur->سانى -= 1;
	    }

	    أكد(new_تۈگۈن->سانى > 0);
	    أكد(cur->سانى > 0);

	    إذا (cur->parent == 0) {
		    /* cur is root. Move first half of children to another
		     * new node, and put the two new nodes in cur. The plan
		     * here is to make cur into the new root and split its
		     * contents into 2 children. */

		  new_تۈگۈن->parent = cur;
		  بنية دەرەخ_تۈگۈنى*new2_تۈگۈن = جديد بنية دەرەخ_تۈگۈنى;
		  new2_تۈگۈن->leaf_flag = خطأ;
		  new2_تۈگۈن->سانى = cur->سانى;
		  new2_تۈگۈن->parent = cur;
		  لكل (unsigned idx = 0; idx < cur->سانى; idx += 1) {
			new2_تۈگۈن->child[idx] = cur->child[idx];
			new2_تۈگۈن->child[idx]->parent = new2_تۈگۈن;
		  }
		  cur->child[0] = new2_تۈگۈن;
		  cur->child[1] = new_تۈگۈن;
		  cur->سانى = 2;
		    /* no more ancestors, stop the while loop */
		  اكسر;
	    }

	      /* cur is not root. hook new_node to cur->parent. */
	    unsigned idx = 0;
	    طالما (cur->parent->child[idx] != cur) {
		  أكد(idx < cur->parent->سانى);
		  idx += 1;
	    }
	    idx += 1;

	    لكل (unsigned tmp = cur->parent->سانى ;  tmp > idx ;  tmp -= 1) {
		  cur->parent->child[tmp] = cur->parent->child[tmp-1];
	    }

	    cur->parent->child[idx] = new_تۈگۈن;
	    cur->parent->سانى += 1;

	      /* check the ancestor */
	    cur = cur->parent;
      }
}

/*
 * This function takes a leaf node and splits in into two. Move half
 * the leaf keys into the new node, and add the new leaf into the
 * parent node.
 */
ساكن بنية دەرەخ_تۈگۈنى* split_leaf_(بنية دەرەخ_تۈگۈنى*cur)
{
      أكد(cur->leaf_flag);
      أكد(cur->parent);
      أكد(! cur->parent->leaf_flag);

	/* Create a new leaf to hold half the data from the old leaf. */
      بنية دەرەخ_تۈگۈنى*new_leaf = جديد بنية دەرەخ_تۈگۈنى;
      new_leaf->leaf_flag = true;
      new_leaf->سانى = cur->سانى / 2;
      new_leaf->parent = cur->parent;

	/* Move the last half of the data from the end of the old leaf
	   to the beginning of the new leaf. At the same time, reduce
	   the size of the old leaf. */
      unsigned idx1 = new_leaf->سانى;
      unsigned idx2 = cur->سانى;
      طالما (idx1 > 0) {
	    idx1 -= 1;
	    idx2 -= 1;
	    new_leaf->leaf[idx1] = cur->leaf[idx2];
	    cur->سانى -= 1;
      }

      أكد(new_leaf->سانى > 0);
      أكد(cur->سانى > 0);

      unsigned idx = 0;
      طالما (cur->parent->child[idx] != cur) {
	    أكد(idx < cur->parent->سانى);
	    idx += 1;
      }

      idx += 1;

      لكل (unsigned tmp = cur->parent->سانى ;  tmp > idx ;  tmp -= 1)
	    cur->parent->child[tmp] = cur->parent->child[tmp-1];

      cur->parent->child[idx] = new_leaf;
      cur->parent->سانى += 1;

      إذا (cur->parent->سانى == تۈگۈن_width)
	    split_تۈگۈن(cur->parent);

      أرجع new_leaf;
}


/*
 * This function searches tree recursively for the key. If the value
 * is not found (and we are at a leaf) then set the key with the given
 * value. If the key is found, set the value only if the force_flag is
 * true.
 */

بەلگە_قىممەت_t بەلگە_table_s::find_قىممەت(بنية دەرەخ_تۈگۈنى*cur,
					   ثابت حرف*ئاچقۇچ, بەلگە_قىممەت_t val,
					   منطقي force_flag)
{
      إذا (cur->leaf_flag) {

	    unsigned idx = 0;
	    لكل (;;) {
		    /* If we run out of keys in the leaf, then add
		       this at the end of the leaf. */
		  إذا (idx == cur->سانى) {
			cur->leaf[idx].ئاچقۇچ = ئاچقۇچ_strdup(ئاچقۇچ);
			cur->leaf[idx].val = val;
			cur->سانى += 1;
			إذا (cur->سانى == leaf_width)
			      split_leaf_(cur);

			أرجع val;
		  }

		  صحيح rc = strcmp(ئاچقۇچ, cur->leaf[idx].ئاچقۇچ);

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
			لكل (unsigned tmp = cur->سانى; tmp > idx; tmp -= 1)
			      cur->leaf[tmp] = cur->leaf[tmp-1];

			cur->leaf[idx].ئاچقۇچ = ئاچقۇچ_strdup(ئاچقۇچ);
			cur->leaf[idx].val = val;
			cur->سانى += 1;
			إذا (cur->سانى == leaf_width)
			      split_leaf_(cur);

			أرجع val;
		  }

		  idx += 1;
	    }

      } وإلا {
	      /* Do a binary search within the inner node. */
	    unsigned min = 0;
	    unsigned max = cur->سانى;
	    unsigned idx = max/2;
	    لكل (;;) {
		  صحيح rc = strcmp(ئاچقۇچ, تۈگۈن_last_ئاچقۇچ(cur->child[idx]));
		  إذا (rc == 0) {
			أرجع find_قىممەت(cur->child[idx],
					   ئاچقۇچ, val, force_flag);
		  }

		  إذا (rc > 0) {
			min = idx + 1;
			إذا (min == cur->سانى)
			      أرجع find_قىممەت(cur->child[idx],
						 ئاچقۇچ, val, force_flag);
			إذا (min == max)
			      أرجع find_قىممەت(cur->child[max],
						 ئاچقۇچ, val, force_flag);

			idx = min + (max-min)/2;

		  } وإلا  {
			max = idx;
			إذا (idx == min)
			      أرجع find_قىممەت(cur->child[idx],
						 ئاچقۇچ, val, force_flag);
			idx = min + (max-min)/2;
		  }
	    }
      }

      أكد(0);
      { بەلگە_قىممەت_t tmp;
        tmp.ptr = 0;
        أرجع tmp;
      }
}

فراغ بەلگە_table_s::sym_set_قىممەت(ثابت حرف*ئاچقۇچ, بەلگە_قىممەت_t val)
{
      إذا (يىلتىز->سانى == 0) {
	      /* Handle the special case that this is the very first
		 value in the symbol table. Create the first leaf node
		 and initialize the pointers. */
	    بنية دەرەخ_تۈگۈنى*cur = جديد بنية دەرەخ_تۈگۈنى;
	    cur->leaf_flag = true;
	    cur->parent = يىلتىز;
	    cur->سانى = 1;
	    cur->leaf[0].ئاچقۇچ = ئاچقۇچ_strdup(ئاچقۇچ);
	    cur->leaf[0].val = val;

	    يىلتىز->سانى = 1;
	    يىلتىز->child[0] = cur;
      } وإلا {
	    find_قىممەت(يىلتىز, ئاچقۇچ, val, true);
      }
}

بەلگە_قىممەت_t بەلگە_table_s::sym_get_قىممەت(ثابت حرف*ئاچقۇچ)
{
      بەلگە_قىممەت_t def;
      def.ptr = 0;

      إذا (يىلتىز->سانى == 0) {
	      /* Handle the special case that this is the very first
		 value in the symbol table. Create the first leaf node
		 and initialize the pointers. */
	    بنية دەرەخ_تۈگۈنى*cur = جديد بنية دەرەخ_تۈگۈنى;
	    cur->leaf_flag = true;
	    cur->parent = يىلتىز;
	    cur->سانى = 1;
	    cur->leaf[0].ئاچقۇچ = ئاچقۇچ_strdup(ئاچقۇچ);
	    cur->leaf[0].val = def;

	    يىلتىز->سانى = 1;
	    يىلتىز->child[0] = cur;
	    أرجع cur->leaf[0].val;
      } وإلا {
	    أرجع find_قىممەت(يىلتىز, ئاچقۇچ, def, خطأ);
      }
}

بەلگە_table_s::~بەلگە_table_s()
{
      delete_بەلگە_تۈگۈن(يىلتىز);
      طالما (str_chunk) {
	    ئاچقۇچ_strings*tmp = str_chunk;
	    str_chunk = tmp->next;
	    delete tmp;
      }
}
