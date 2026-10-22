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
struct key_strings {
      struct key_strings*next;
      char data[64*1024 - sizeof(struct key_strings*)];
};

char*symbol_table_s::key_strdup_(const char*str)
{
      unsigned len = strlen(str);
      assert( (len+1) <= sizeof str_chunk->data );

      ако ( (len+1) > (sizeof str_chunk->data - str_used) ) {
	    key_strings*tmp = new key_strings;
	    tmp->next = str_chunk;
	    str_chunk = tmp;
	    str_used = 0;
      }

      char*res = str_chunk->data + str_used;
      str_used += len + 1;
      strcpy(res, str);
      врати res;
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

const unsigned leaf_width = 254;
const unsigned node_width = 508;

struct tree_data_ {
      char*key;
      symbol_value_t val;
};

struct tree_node_ {
      bool leaf_flag;
      unsigned count;
      struct tree_node_*parent;

      union {
	    struct tree_data_ leaf[leaf_width];
	    struct tree_node_ *child[node_width];
      };

};

static inline char* node_last_key(struct tree_node_*node)
{
      док (node->leaf_flag == false)
	    node = node->child[node->count-1];

      врати node->leaf[node->count-1].key;
}

/*
 * Allocate a new symbol table means creating the table structure and
 * a root node, and initializing the pointers and members of the root
 * node.
 */
symbol_table_s::symbol_table_s()
{
      root = new struct tree_node_;
      root->leaf_flag = false;
      root->count = 0;
      root->parent = 0;

      str_chunk = new key_strings;
      str_chunk->next = 0;
      str_used = 0;
}

static void delete_symbol_node(struct tree_node_*cur)
{
      ако (! cur->leaf_flag) {
	    за (unsigned idx = 0 ;  idx < cur->count ;  idx += 1)
		  delete_symbol_node(cur->child[idx]);
      }

      delete cur;
}

/* Do as split_leaf_ does, but for nodes. */
static void split_node_(struct tree_node_*cur)
{
      assert(!cur->leaf_flag);
      ако (cur->parent) assert(! cur->parent->leaf_flag);

      док (cur->count == node_width) {
	      /* Create a new node to hold half the data from cur. */
	    struct tree_node_ *new_node = new struct tree_node_;
	    new_node->leaf_flag = false;
	    new_node->count = cur->count / 2;
	      /* cur is not root; new_node becomes sibling. */
	    ако (cur->parent) new_node->parent = cur->parent;

	      /* Move the last half of the data from the end of the old node
	       * to the beginning of the new node. At the same time, reduce
	       * the size of the old node. */
	    unsigned idx1 = new_node->count;
	    unsigned idx2 = cur->count;
	    док (idx1 > 0) {
		  idx1 -= 1;
		  idx2 -= 1;
		  new_node->child[idx1] = cur->child[idx2];
		  new_node->child[idx1]->parent = new_node;
		  cur->count -= 1;
	    }

	    assert(new_node->count > 0);
	    assert(cur->count > 0);

	    ако (cur->parent == 0) {
		    /* cur is root. Move first half of children to another
		     * new node, and put the two new nodes in cur. The plan
		     * here is to make cur into the new root and split its
		     * contents into 2 children. */

		  new_node->parent = cur;
		  struct tree_node_*new2_node = new struct tree_node_;
		  new2_node->leaf_flag = false;
		  new2_node->count = cur->count;
		  new2_node->parent = cur;
		  за (unsigned idx = 0; idx < cur->count; idx += 1) {
			new2_node->child[idx] = cur->child[idx];
			new2_node->child[idx]->parent = new2_node;
		  }
		  cur->child[0] = new2_node;
		  cur->child[1] = new_node;
		  cur->count = 2;
		    /* no more ancestors, stop the while loop */
		  break;
	    }

	      /* cur is not root. hook new_node to cur->parent. */
	    unsigned idx = 0;
	    док (cur->parent->child[idx] != cur) {
		  assert(idx < cur->parent->count);
		  idx += 1;
	    }
	    idx += 1;

	    за (unsigned tmp = cur->parent->count ;  tmp > idx ;  tmp -= 1) {
		  cur->parent->child[tmp] = cur->parent->child[tmp-1];
	    }

	    cur->parent->child[idx] = new_node;
	    cur->parent->count += 1;

	      /* check the ancestor */
	    cur = cur->parent;
      }
}

/*
 * This function takes a leaf node and splits in into two. Move half
 * the leaf keys into the new node, and add the new leaf into the
 * parent node.
 */
static struct tree_node_* split_leaf_(struct tree_node_*cur)
{
      assert(cur->leaf_flag);
      assert(cur->parent);
      assert(! cur->parent->leaf_flag);

	/* Create a new leaf to hold half the data from the old leaf. */
      struct tree_node_*new_leaf = new struct tree_node_;
      new_leaf->leaf_flag = true;
      new_leaf->count = cur->count / 2;
      new_leaf->parent = cur->parent;

	/* Move the last half of the data from the end of the old leaf
	   to the beginning of the new leaf. At the same time, reduce
	   the size of the old leaf. */
      unsigned idx1 = new_leaf->count;
      unsigned idx2 = cur->count;
      док (idx1 > 0) {
	    idx1 -= 1;
	    idx2 -= 1;
	    new_leaf->leaf[idx1] = cur->leaf[idx2];
	    cur->count -= 1;
      }

      assert(new_leaf->count > 0);
      assert(cur->count > 0);

      unsigned idx = 0;
      док (cur->parent->child[idx] != cur) {
	    assert(idx < cur->parent->count);
	    idx += 1;
      }

      idx += 1;

      за (unsigned tmp = cur->parent->count ;  tmp > idx ;  tmp -= 1)
	    cur->parent->child[tmp] = cur->parent->child[tmp-1];

      cur->parent->child[idx] = new_leaf;
      cur->parent->count += 1;

      ако (cur->parent->count == node_width)
	    split_node_(cur->parent);

      врати new_leaf;
}


/*
 * This function searches tree recursively for the key. If the value
 * is not found (and we are at a leaf) then set the key with the given
 * value. If the key is found, set the value only if the force_flag is
 * true.
 */

symbol_value_t symbol_table_s::find_value_(struct tree_node_*cur,
					   const char*key, symbol_value_t val,
					   bool force_flag)
{
      ако (cur->leaf_flag) {

	    unsigned idx = 0;
	    за (;;) {
		    /* If we run out of keys in the leaf, then add
		       this at the end of the leaf. */
		  ако (idx == cur->count) {
			cur->leaf[idx].key = key_strdup_(key);
			cur->leaf[idx].val = val;
			cur->count += 1;
			ако (cur->count == leaf_width)
			      split_leaf_(cur);

			врати val;
		  }

		  цео rc = strcmp(key, cur->leaf[idx].key);

		    /* If we found the key already in the table, then
		       set the new value and break. */
		  ако (rc == 0) {
			ако (force_flag)
			      cur->leaf[idx].val = val;
			врати cur->leaf[idx].val;
		  }

		    /* If this key goes before the current key, then
		       push all the following entries back and add
		       this key here. */
		  ако (rc < 0) {
			за (unsigned tmp = cur->count; tmp > idx; tmp -= 1)
			      cur->leaf[tmp] = cur->leaf[tmp-1];

			cur->leaf[idx].key = key_strdup_(key);
			cur->leaf[idx].val = val;
			cur->count += 1;
			ако (cur->count == leaf_width)
			      split_leaf_(cur);

			врати val;
		  }

		  idx += 1;
	    }

      } инако {
	      /* Do a binary search within the inner node. */
	    unsigned min = 0;
	    unsigned max = cur->count;
	    unsigned idx = max/2;
	    за (;;) {
		  цео rc = strcmp(key, node_last_key(cur->child[idx]));
		  ако (rc == 0) {
			врати find_value_(cur->child[idx],
					   key, val, force_flag);
		  }

		  ако (rc > 0) {
			min = idx + 1;
			ако (min == cur->count)
			      врати find_value_(cur->child[idx],
						 key, val, force_flag);
			ако (min == max)
			      врати find_value_(cur->child[max],
						 key, val, force_flag);

			idx = min + (max-min)/2;

		  } инако  {
			max = idx;
			ако (idx == min)
			      врати find_value_(cur->child[idx],
						 key, val, force_flag);
			idx = min + (max-min)/2;
		  }
	    }
      }

      assert(0);
      { symbol_value_t tmp;
        tmp.ptr = 0;
        врати tmp;
      }
}

void symbol_table_s::sym_set_value(const char*key, symbol_value_t val)
{
      ако (root->count == 0) {
	      /* Handle the special case that this is the very first
		 value in the symbol table. Create the first leaf node
		 and initialize the pointers. */
	    struct tree_node_*cur = new struct tree_node_;
	    cur->leaf_flag = true;
	    cur->parent = root;
	    cur->count = 1;
	    cur->leaf[0].key = key_strdup_(key);
	    cur->leaf[0].val = val;

	    root->count = 1;
	    root->child[0] = cur;
      } инако {
	    find_value_(root, key, val, true);
      }
}

symbol_value_t symbol_table_s::sym_get_value(const char*key)
{
      symbol_value_t def;
      def.ptr = 0;

      ако (root->count == 0) {
	      /* Handle the special case that this is the very first
		 value in the symbol table. Create the first leaf node
		 and initialize the pointers. */
	    struct tree_node_*cur = new struct tree_node_;
	    cur->leaf_flag = true;
	    cur->parent = root;
	    cur->count = 1;
	    cur->leaf[0].key = key_strdup_(key);
	    cur->leaf[0].val = def;

	    root->count = 1;
	    root->child[0] = cur;
	    врати cur->leaf[0].val;
      } инако {
	    врати find_value_(root, key, def, false);
      }
}

symbol_table_s::~symbol_table_s()
{
      delete_symbol_node(root);
      док (str_chunk) {
	    key_strings*tmp = str_chunk;
	    str_chunk = tmp->next;
	    delete tmp;
      }
}
