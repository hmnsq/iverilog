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
struct सुसि_strings {
      struct सुसि_strings*next;
      char data[64*1024 - sizeof(struct सुसि_strings*)];
};

char*सिम्बोलो_table_s::सुसि_strdup(const char*str)
{
      unsigned len = strlen(str);
      assert( (len+1) <= sizeof str_chunk->data );

      if ( (len+1) > (sizeof str_chunk->data - str_used) ) {
	    सुसि_strings*tmp = new सुसि_strings;
	    tmp->next = str_chunk;
	    str_chunk = tmp;
	    str_used = 0;
      }

      char*res = str_chunk->data + str_used;
      str_used += len + 1;
      strcpy(res, str);
      return res;
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
const unsigned बुहोल_width = 508;

struct पुनो_data {
      char*सुसि;
      सिम्बोलो_हालागा_t val;
};

struct पुनो_बुहोल {
      bool leaf_flag;
      unsigned बिलांग;
      struct पुनो_बुहोल*parent;

      union {
	    struct पुनो_data leaf[leaf_width];
	    struct पुनो_बुहोल *child[बुहोल_width];
      };

};

static inline char* बुहोल_last_सुसि(struct पुनो_बुहोल*बुहोल)
{
      while (बुहोल->leaf_flag == false)
	    बुहोल = बुहोल->child[बुहोल->बिलांग-1];

      return बुहोल->leaf[बुहोल->बिलांग-1].सुसि;
}

/*
 * Allocate a new symbol table means creating the table structure and
 * a root node, and initializing the pointers and members of the root
 * node.
 */
सिम्बोलो_table_s::सिम्बोलो_table_s()
{
      उगात = new struct पुनो_बुहोल;
      उगात->leaf_flag = false;
      उगात->बिलांग = 0;
      उगात->parent = 0;

      str_chunk = new सुसि_strings;
      str_chunk->next = 0;
      str_used = 0;
}

static void delete_सिम्बोलो_बुहोल(struct पुनो_बुहोल*cur)
{
      if (! cur->leaf_flag) {
	    for (unsigned idx = 0 ;  idx < cur->बिलांग ;  idx += 1)
		  delete_सिम्बोलो_बुहोल(cur->child[idx]);
      }

      delete cur;
}

/* Do as split_leaf_ does, but for nodes. */
static void split_बुहोल(struct पुनो_बुहोल*cur)
{
      assert(!cur->leaf_flag);
      if (cur->parent) assert(! cur->parent->leaf_flag);

      while (cur->बिलांग == बुहोल_width) {
	      /* Create a new node to hold half the data from cur. */
	    struct पुनो_बुहोल *new_बुहोल = new struct पुनो_बुहोल;
	    new_बुहोल->leaf_flag = false;
	    new_बुहोल->बिलांग = cur->बिलांग / 2;
	      /* cur is not root; new_node becomes sibling. */
	    if (cur->parent) new_बुहोल->parent = cur->parent;

	      /* Move the last half of the data from the end of the old node
	       * to the beginning of the new node. At the same time, reduce
	       * the size of the old node. */
	    unsigned idx1 = new_बुहोल->बिलांग;
	    unsigned idx2 = cur->बिलांग;
	    while (idx1 > 0) {
		  idx1 -= 1;
		  idx2 -= 1;
		  new_बुहोल->child[idx1] = cur->child[idx2];
		  new_बुहोल->child[idx1]->parent = new_बुहोल;
		  cur->बिलांग -= 1;
	    }

	    assert(new_बुहोल->बिलांग > 0);
	    assert(cur->बिलांग > 0);

	    if (cur->parent == 0) {
		    /* cur is root. Move first half of children to another
		     * new node, and put the two new nodes in cur. The plan
		     * here is to make cur into the new root and split its
		     * contents into 2 children. */

		  new_बुहोल->parent = cur;
		  struct पुनो_बुहोल*new2_बुहोल = new struct पुनो_बुहोल;
		  new2_बुहोल->leaf_flag = false;
		  new2_बुहोल->बिलांग = cur->बिलांग;
		  new2_बुहोल->parent = cur;
		  for (unsigned idx = 0; idx < cur->बिलांग; idx += 1) {
			new2_बुहोल->child[idx] = cur->child[idx];
			new2_बुहोल->child[idx]->parent = new2_बुहोल;
		  }
		  cur->child[0] = new2_बुहोल;
		  cur->child[1] = new_बुहोल;
		  cur->बिलांग = 2;
		    /* no more ancestors, stop the while loop */
		  break;
	    }

	      /* cur is not root. hook new_node to cur->parent. */
	    unsigned idx = 0;
	    while (cur->parent->child[idx] != cur) {
		  assert(idx < cur->parent->बिलांग);
		  idx += 1;
	    }
	    idx += 1;

	    for (unsigned tmp = cur->parent->बिलांग ;  tmp > idx ;  tmp -= 1) {
		  cur->parent->child[tmp] = cur->parent->child[tmp-1];
	    }

	    cur->parent->child[idx] = new_बुहोल;
	    cur->parent->बिलांग += 1;

	      /* check the ancestor */
	    cur = cur->parent;
      }
}

/*
 * This function takes a leaf node and splits in into two. Move half
 * the leaf keys into the new node, and add the new leaf into the
 * parent node.
 */
static struct पुनो_बुहोल* split_leaf_(struct पुनो_बुहोल*cur)
{
      assert(cur->leaf_flag);
      assert(cur->parent);
      assert(! cur->parent->leaf_flag);

	/* Create a new leaf to hold half the data from the old leaf. */
      struct पुनो_बुहोल*new_leaf = new struct पुनो_बुहोल;
      new_leaf->leaf_flag = true;
      new_leaf->बिलांग = cur->बिलांग / 2;
      new_leaf->parent = cur->parent;

	/* Move the last half of the data from the end of the old leaf
	   to the beginning of the new leaf. At the same time, reduce
	   the size of the old leaf. */
      unsigned idx1 = new_leaf->बिलांग;
      unsigned idx2 = cur->बिलांग;
      while (idx1 > 0) {
	    idx1 -= 1;
	    idx2 -= 1;
	    new_leaf->leaf[idx1] = cur->leaf[idx2];
	    cur->बिलांग -= 1;
      }

      assert(new_leaf->बिलांग > 0);
      assert(cur->बिलांग > 0);

      unsigned idx = 0;
      while (cur->parent->child[idx] != cur) {
	    assert(idx < cur->parent->बिलांग);
	    idx += 1;
      }

      idx += 1;

      for (unsigned tmp = cur->parent->बिलांग ;  tmp > idx ;  tmp -= 1)
	    cur->parent->child[tmp] = cur->parent->child[tmp-1];

      cur->parent->child[idx] = new_leaf;
      cur->parent->बिलांग += 1;

      if (cur->parent->बिलांग == बुहोल_width)
	    split_बुहोल(cur->parent);

      return new_leaf;
}


/*
 * This function searches tree recursively for the key. If the value
 * is not found (and we are at a leaf) then set the key with the given
 * value. If the key is found, set the value only if the force_flag is
 * true.
 */

सिम्बोलो_हालागा_t सिम्बोलो_table_s::find_हालागा(struct पुनो_बुहोल*cur,
					   const char*सुसि, सिम्बोलो_हालागा_t val,
					   bool force_flag)
{
      if (cur->leaf_flag) {

	    unsigned idx = 0;
	    for (;;) {
		    /* If we run out of keys in the leaf, then add
		       this at the end of the leaf. */
		  if (idx == cur->बिलांग) {
			cur->leaf[idx].सुसि = सुसि_strdup(सुसि);
			cur->leaf[idx].val = val;
			cur->बिलांग += 1;
			if (cur->बिलांग == leaf_width)
			      split_leaf_(cur);

			return val;
		  }

		  int rc = strcmp(सुसि, cur->leaf[idx].सुसि);

		    /* If we found the key already in the table, then
		       set the new value and break. */
		  if (rc == 0) {
			if (force_flag)
			      cur->leaf[idx].val = val;
			return cur->leaf[idx].val;
		  }

		    /* If this key goes before the current key, then
		       push all the following entries back and add
		       this key here. */
		  if (rc < 0) {
			for (unsigned tmp = cur->बिलांग; tmp > idx; tmp -= 1)
			      cur->leaf[tmp] = cur->leaf[tmp-1];

			cur->leaf[idx].सुसि = सुसि_strdup(सुसि);
			cur->leaf[idx].val = val;
			cur->बिलांग += 1;
			if (cur->बिलांग == leaf_width)
			      split_leaf_(cur);

			return val;
		  }

		  idx += 1;
	    }

      } else {
	      /* Do a binary search within the inner node. */
	    unsigned min = 0;
	    unsigned max = cur->बिलांग;
	    unsigned idx = max/2;
	    for (;;) {
		  int rc = strcmp(सुसि, बुहोल_last_सुसि(cur->child[idx]));
		  if (rc == 0) {
			return find_हालागा(cur->child[idx],
					   सुसि, val, force_flag);
		  }

		  if (rc > 0) {
			min = idx + 1;
			if (min == cur->बिलांग)
			      return find_हालागा(cur->child[idx],
						 सुसि, val, force_flag);
			if (min == max)
			      return find_हालागा(cur->child[max],
						 सुसि, val, force_flag);

			idx = min + (max-min)/2;

		  } else  {
			max = idx;
			if (idx == min)
			      return find_हालागा(cur->child[idx],
						 सुसि, val, force_flag);
			idx = min + (max-min)/2;
		  }
	    }
      }

      assert(0);
      { सिम्बोलो_हालागा_t tmp;
        tmp.ptr = 0;
        return tmp;
      }
}

void सिम्बोलो_table_s::sym_set_हालागा(const char*सुसि, सिम्बोलो_हालागा_t val)
{
      if (उगात->बिलांग == 0) {
	      /* Handle the special case that this is the very first
		 value in the symbol table. Create the first leaf node
		 and initialize the pointers. */
	    struct पुनो_बुहोल*cur = new struct पुनो_बुहोल;
	    cur->leaf_flag = true;
	    cur->parent = उगात;
	    cur->बिलांग = 1;
	    cur->leaf[0].सुसि = सुसि_strdup(सुसि);
	    cur->leaf[0].val = val;

	    उगात->बिलांग = 1;
	    उगात->child[0] = cur;
      } else {
	    find_हालागा(उगात, सुसि, val, true);
      }
}

सिम्बोलो_हालागा_t सिम्बोलो_table_s::sym_get_हालागा(const char*सुसि)
{
      सिम्बोलो_हालागा_t def;
      def.ptr = 0;

      if (उगात->बिलांग == 0) {
	      /* Handle the special case that this is the very first
		 value in the symbol table. Create the first leaf node
		 and initialize the pointers. */
	    struct पुनो_बुहोल*cur = new struct पुनो_बुहोल;
	    cur->leaf_flag = true;
	    cur->parent = उगात;
	    cur->बिलांग = 1;
	    cur->leaf[0].सुसि = सुसि_strdup(सुसि);
	    cur->leaf[0].val = def;

	    उगात->बिलांग = 1;
	    उगात->child[0] = cur;
	    return cur->leaf[0].val;
      } else {
	    return find_हालागा(उगात, सुसि, def, false);
      }
}

सिम्बोलो_table_s::~सिम्बोलो_table_s()
{
      delete_सिम्बोलो_बुहोल(उगात);
      while (str_chunk) {
	    सुसि_strings*tmp = str_chunk;
	    str_chunk = tmp->next;
	    delete tmp;
      }
}
