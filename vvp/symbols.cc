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
સંરચના ચાવી_strings {
      સંરચના ચાવી_strings*next;
      અક્ષર data[64*1024 - કદ(સંરચના ચાવી_strings*)];
};

અક્ષર*પ્રતીક_table_s::ચાવી_strdup(અચલ અક્ષર*str)
{
      unsigned len = strlen(str);
      પ્રતિજ્ઞા( (len+1) <= કદ str_chunk->data );

      જો ( (len+1) > (કદ str_chunk->data - str_used) ) {
	    ચાવી_strings*tmp = નવું ચાવી_strings;
	    tmp->next = str_chunk;
	    str_chunk = tmp;
	    str_used = 0;
      }

      અક્ષર*res = str_chunk->data + str_used;
      str_used += len + 1;
      strcpy(res, str);
      પાછા res;
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

અચલ unsigned leaf_width = 254;
અચલ unsigned ગાંઠ_width = 508;

સંરચના વૃક્ષ_data {
      અક્ષર*ચાવી;
      પ્રતીક_કિંમત_t val;
};

સંરચના વૃક્ષ_ગાંઠ {
      તાર્કિક leaf_flag;
      unsigned ગણતરી;
      સંરચના વૃક્ષ_ગાંઠ*parent;

      સંઘ {
	    સંરચના વૃક્ષ_data leaf[leaf_width];
	    સંરચના વૃક્ષ_ગાંઠ *child[ગાંઠ_width];
      };

};

સ્થિર અંતઃરેખ અક્ષર* ગાંઠ_last_ચાવી(સંરચના વૃક્ષ_ગાંઠ*ગાંઠ)
{
      જ્યારે (ગાંઠ->leaf_flag == ખોટું)
	    ગાંઠ = ગાંઠ->child[ગાંઠ->ગણતરી-1];

      પાછા ગાંઠ->leaf[ગાંઠ->ગણતરી-1].ચાવી;
}

/*
 * Allocate a new symbol table means creating the table structure and
 * a root node, and initializing the pointers and members of the root
 * node.
 */
પ્રતીક_table_s::પ્રતીક_table_s()
{
      મૂળ = નવું સંરચના વૃક્ષ_ગાંઠ;
      મૂળ->leaf_flag = ખોટું;
      મૂળ->ગણતરી = 0;
      મૂળ->parent = 0;

      str_chunk = નવું ચાવી_strings;
      str_chunk->next = 0;
      str_used = 0;
}

સ્થિર શૂન્ય delete_પ્રતીક_ગાંઠ(સંરચના વૃક્ષ_ગાંઠ*cur)
{
      જો (! cur->leaf_flag) {
	    માટે (unsigned idx = 0 ;  idx < cur->ગણતરી ;  idx += 1)
		  delete_પ્રતીક_ગાંઠ(cur->child[idx]);
      }

      delete cur;
}

/* Do as split_leaf_ does, but for nodes. */
સ્થિર શૂન્ય split_ગાંઠ(સંરચના વૃક્ષ_ગાંઠ*cur)
{
      પ્રતિજ્ઞા(!cur->leaf_flag);
      જો (cur->parent) પ્રતિજ્ઞા(! cur->parent->leaf_flag);

      જ્યારે (cur->ગણતરી == ગાંઠ_width) {
	      /* Create a new node to hold half the data from cur. */
	    સંરચના વૃક્ષ_ગાંઠ *new_ગાંઠ = નવું સંરચના વૃક્ષ_ગાંઠ;
	    new_ગાંઠ->leaf_flag = ખોટું;
	    new_ગાંઠ->ગણતરી = cur->ગણતરી / 2;
	      /* cur is not root; new_node becomes sibling. */
	    જો (cur->parent) new_ગાંઠ->parent = cur->parent;

	      /* Move the last half of the data from the end of the old node
	       * to the beginning of the new node. At the same time, reduce
	       * the size of the old node. */
	    unsigned idx1 = new_ગાંઠ->ગણતરી;
	    unsigned idx2 = cur->ગણતરી;
	    જ્યારે (idx1 > 0) {
		  idx1 -= 1;
		  idx2 -= 1;
		  new_ગાંઠ->child[idx1] = cur->child[idx2];
		  new_ગાંઠ->child[idx1]->parent = new_ગાંઠ;
		  cur->ગણતરી -= 1;
	    }

	    પ્રતિજ્ઞા(new_ગાંઠ->ગણતરી > 0);
	    પ્રતિજ્ઞા(cur->ગણતરી > 0);

	    જો (cur->parent == 0) {
		    /* cur is root. Move first half of children to another
		     * new node, and put the two new nodes in cur. The plan
		     * here is to make cur into the new root and split its
		     * contents into 2 children. */

		  new_ગાંઠ->parent = cur;
		  સંરચના વૃક્ષ_ગાંઠ*new2_ગાંઠ = નવું સંરચના વૃક્ષ_ગાંઠ;
		  new2_ગાંઠ->leaf_flag = ખોટું;
		  new2_ગાંઠ->ગણતરી = cur->ગણતરી;
		  new2_ગાંઠ->parent = cur;
		  માટે (unsigned idx = 0; idx < cur->ગણતરી; idx += 1) {
			new2_ગાંઠ->child[idx] = cur->child[idx];
			new2_ગાંઠ->child[idx]->parent = new2_ગાંઠ;
		  }
		  cur->child[0] = new2_ગાંઠ;
		  cur->child[1] = new_ગાંઠ;
		  cur->ગણતરી = 2;
		    /* no more ancestors, stop the while loop */
		  તોડો;
	    }

	      /* cur is not root. hook new_node to cur->parent. */
	    unsigned idx = 0;
	    જ્યારે (cur->parent->child[idx] != cur) {
		  પ્રતિજ્ઞા(idx < cur->parent->ગણતરી);
		  idx += 1;
	    }
	    idx += 1;

	    માટે (unsigned tmp = cur->parent->ગણતરી ;  tmp > idx ;  tmp -= 1) {
		  cur->parent->child[tmp] = cur->parent->child[tmp-1];
	    }

	    cur->parent->child[idx] = new_ગાંઠ;
	    cur->parent->ગણતરી += 1;

	      /* check the ancestor */
	    cur = cur->parent;
      }
}

/*
 * This function takes a leaf node and splits in into two. Move half
 * the leaf keys into the new node, and add the new leaf into the
 * parent node.
 */
સ્થિર સંરચના વૃક્ષ_ગાંઠ* split_leaf_(સંરચના વૃક્ષ_ગાંઠ*cur)
{
      પ્રતિજ્ઞા(cur->leaf_flag);
      પ્રતિજ્ઞા(cur->parent);
      પ્રતિજ્ઞા(! cur->parent->leaf_flag);

	/* Create a new leaf to hold half the data from the old leaf. */
      સંરચના વૃક્ષ_ગાંઠ*new_leaf = નવું સંરચના વૃક્ષ_ગાંઠ;
      new_leaf->leaf_flag = સાચું;
      new_leaf->ગણતરી = cur->ગણતરી / 2;
      new_leaf->parent = cur->parent;

	/* Move the last half of the data from the end of the old leaf
	   to the beginning of the new leaf. At the same time, reduce
	   the size of the old leaf. */
      unsigned idx1 = new_leaf->ગણતરી;
      unsigned idx2 = cur->ગણતરી;
      જ્યારે (idx1 > 0) {
	    idx1 -= 1;
	    idx2 -= 1;
	    new_leaf->leaf[idx1] = cur->leaf[idx2];
	    cur->ગણતરી -= 1;
      }

      પ્રતિજ્ઞા(new_leaf->ગણતરી > 0);
      પ્રતિજ્ઞા(cur->ગણતરી > 0);

      unsigned idx = 0;
      જ્યારે (cur->parent->child[idx] != cur) {
	    પ્રતિજ્ઞા(idx < cur->parent->ગણતરી);
	    idx += 1;
      }

      idx += 1;

      માટે (unsigned tmp = cur->parent->ગણતરી ;  tmp > idx ;  tmp -= 1)
	    cur->parent->child[tmp] = cur->parent->child[tmp-1];

      cur->parent->child[idx] = new_leaf;
      cur->parent->ગણતરી += 1;

      જો (cur->parent->ગણતરી == ગાંઠ_width)
	    split_ગાંઠ(cur->parent);

      પાછા new_leaf;
}


/*
 * This function searches tree recursively for the key. If the value
 * is not found (and we are at a leaf) then set the key with the given
 * value. If the key is found, set the value only if the force_flag is
 * true.
 */

પ્રતીક_કિંમત_t પ્રતીક_table_s::find_કિંમત(સંરચના વૃક્ષ_ગાંઠ*cur,
					   અચલ અક્ષર*ચાવી, પ્રતીક_કિંમત_t val,
					   તાર્કિક force_flag)
{
      જો (cur->leaf_flag) {

	    unsigned idx = 0;
	    માટે (;;) {
		    /* If we run out of keys in the leaf, then add
		       this at the end of the leaf. */
		  જો (idx == cur->ગણતરી) {
			cur->leaf[idx].ચાવી = ચાવી_strdup(ચાવી);
			cur->leaf[idx].val = val;
			cur->ગણતરી += 1;
			જો (cur->ગણતરી == leaf_width)
			      split_leaf_(cur);

			પાછા val;
		  }

		  પૂર્ણાંક rc = strcmp(ચાવી, cur->leaf[idx].ચાવી);

		    /* If we found the key already in the table, then
		       set the new value and break. */
		  જો (rc == 0) {
			જો (force_flag)
			      cur->leaf[idx].val = val;
			પાછા cur->leaf[idx].val;
		  }

		    /* If this key goes before the current key, then
		       push all the following entries back and add
		       this key here. */
		  જો (rc < 0) {
			માટે (unsigned tmp = cur->ગણતરી; tmp > idx; tmp -= 1)
			      cur->leaf[tmp] = cur->leaf[tmp-1];

			cur->leaf[idx].ચાવી = ચાવી_strdup(ચાવી);
			cur->leaf[idx].val = val;
			cur->ગણતરી += 1;
			જો (cur->ગણતરી == leaf_width)
			      split_leaf_(cur);

			પાછા val;
		  }

		  idx += 1;
	    }

      } નહીંતર {
	      /* Do a binary search within the inner node. */
	    unsigned min = 0;
	    unsigned max = cur->ગણતરી;
	    unsigned idx = max/2;
	    માટે (;;) {
		  પૂર્ણાંક rc = strcmp(ચાવી, ગાંઠ_last_ચાવી(cur->child[idx]));
		  જો (rc == 0) {
			પાછા find_કિંમત(cur->child[idx],
					   ચાવી, val, force_flag);
		  }

		  જો (rc > 0) {
			min = idx + 1;
			જો (min == cur->ગણતરી)
			      પાછા find_કિંમત(cur->child[idx],
						 ચાવી, val, force_flag);
			જો (min == max)
			      પાછા find_કિંમત(cur->child[max],
						 ચાવી, val, force_flag);

			idx = min + (max-min)/2;

		  } નહીંતર  {
			max = idx;
			જો (idx == min)
			      પાછા find_કિંમત(cur->child[idx],
						 ચાવી, val, force_flag);
			idx = min + (max-min)/2;
		  }
	    }
      }

      પ્રતિજ્ઞા(0);
      { પ્રતીક_કિંમત_t tmp;
        tmp.ptr = 0;
        પાછા tmp;
      }
}

શૂન્ય પ્રતીક_table_s::sym_set_કિંમત(અચલ અક્ષર*ચાવી, પ્રતીક_કિંમત_t val)
{
      જો (મૂળ->ગણતરી == 0) {
	      /* Handle the special case that this is the very first
		 value in the symbol table. Create the first leaf node
		 and initialize the pointers. */
	    સંરચના વૃક્ષ_ગાંઠ*cur = નવું સંરચના વૃક્ષ_ગાંઠ;
	    cur->leaf_flag = સાચું;
	    cur->parent = મૂળ;
	    cur->ગણતરી = 1;
	    cur->leaf[0].ચાવી = ચાવી_strdup(ચાવી);
	    cur->leaf[0].val = val;

	    મૂળ->ગણતરી = 1;
	    મૂળ->child[0] = cur;
      } નહીંતર {
	    find_કિંમત(મૂળ, ચાવી, val, સાચું);
      }
}

પ્રતીક_કિંમત_t પ્રતીક_table_s::sym_get_કિંમત(અચલ અક્ષર*ચાવી)
{
      પ્રતીક_કિંમત_t def;
      def.ptr = 0;

      જો (મૂળ->ગણતરી == 0) {
	      /* Handle the special case that this is the very first
		 value in the symbol table. Create the first leaf node
		 and initialize the pointers. */
	    સંરચના વૃક્ષ_ગાંઠ*cur = નવું સંરચના વૃક્ષ_ગાંઠ;
	    cur->leaf_flag = સાચું;
	    cur->parent = મૂળ;
	    cur->ગણતરી = 1;
	    cur->leaf[0].ચાવી = ચાવી_strdup(ચાવી);
	    cur->leaf[0].val = def;

	    મૂળ->ગણતરી = 1;
	    મૂળ->child[0] = cur;
	    પાછા cur->leaf[0].val;
      } નહીંતર {
	    પાછા find_કિંમત(મૂળ, ચાવી, def, ખોટું);
      }
}

પ્રતીક_table_s::~પ્રતીક_table_s()
{
      delete_પ્રતીક_ગાંઠ(મૂળ);
      જ્યારે (str_chunk) {
	    ચાવી_strings*tmp = str_chunk;
	    str_chunk = tmp->next;
	    delete tmp;
      }
}
