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
ସଂରଚନା ଚାବି_strings {
      ସଂରଚନା ଚାବି_strings*next;
      ଅକ୍ଷର data[64*1024 - ଆକାର(ସଂରଚନା ଚାବି_strings*)];
};

ଅକ୍ଷର*ପ୍ରତୀକ_table_s::ଚାବି_strdup(ଅଚଳ ଅକ୍ଷର*str)
{
      unsigned len = strlen(str);
      ପ୍ରତିଜ୍ଞା( (len+1) <= ଆକାର str_chunk->data );

      ଯଦି ( (len+1) > (ଆକାର str_chunk->data - str_used) ) {
	    ଚାବି_strings*tmp = ନୂତନ ଚାବି_strings;
	    tmp->next = str_chunk;
	    str_chunk = tmp;
	    str_used = 0;
      }

      ଅକ୍ଷର*res = str_chunk->data + str_used;
      str_used += len + 1;
      strcpy(res, str);
      ଫେରନ୍ତୁ res;
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

ଅଚଳ unsigned leaf_width = 254;
ଅଚଳ unsigned ଗଣ୍ଠି_width = 508;

ସଂରଚନା ବୃକ୍ଷ_data {
      ଅକ୍ଷର*ଚାବି;
      ପ୍ରତୀକ_ମୂଲ୍ୟ_t val;
};

ସଂରଚନା ବୃକ୍ଷ_ଗଣ୍ଠି {
      ତାର୍କିକ leaf_flag;
      unsigned ଗଣନା_2;
      ସଂରଚନା ବୃକ୍ଷ_ଗଣ୍ଠି*parent;

      ସଂଘ {
	    ସଂରଚନା ବୃକ୍ଷ_data leaf[leaf_width];
	    ସଂରଚନା ବୃକ୍ଷ_ଗଣ୍ଠି *child[ଗଣ୍ଠି_width];
      };

};

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଅକ୍ଷର* ଗଣ୍ଠି_last_ଚାବି(ସଂରଚନା ବୃକ୍ଷ_ଗଣ୍ଠି*ଗଣ୍ଠି)
{
      ଯେତେବେଳେ (ଗଣ୍ଠି->leaf_flag == ମିଥ୍ୟା)
	    ଗଣ୍ଠି = ଗଣ୍ଠି->child[ଗଣ୍ଠି->ଗଣନା_2-1];

      ଫେରନ୍ତୁ ଗଣ୍ଠି->leaf[ଗଣ୍ଠି->ଗଣନା_2-1].ଚାବି;
}

/*
 * Allocate a new symbol table means creating the table structure and
 * a root node, and initializing the pointers and members of the root
 * node.
 */
ପ୍ରତୀକ_table_s::ପ୍ରତୀକ_table_s()
{
      ମୂଳ = ନୂତନ ସଂରଚନା ବୃକ୍ଷ_ଗଣ୍ଠି;
      ମୂଳ->leaf_flag = ମିଥ୍ୟା;
      ମୂଳ->ଗଣନା_2 = 0;
      ମୂଳ->parent = 0;

      str_chunk = ନୂତନ ଚାବି_strings;
      str_chunk->next = 0;
      str_used = 0;
}

ସ୍ଥିର ଶୂନ୍ୟ delete_ପ୍ରତୀକ_ଗଣ୍ଠି(ସଂରଚନା ବୃକ୍ଷ_ଗଣ୍ଠି*cur)
{
      ଯଦି (! cur->leaf_flag) {
	    ପାଇଁ (unsigned idx = 0 ;  idx < cur->ଗଣନା_2 ;  idx += 1)
		  delete_ପ୍ରତୀକ_ଗଣ୍ଠି(cur->child[idx]);
      }

      delete cur;
}

/* Do as split_leaf_ does, but for nodes. */
ସ୍ଥିର ଶୂନ୍ୟ split_ଗଣ୍ଠି(ସଂରଚନା ବୃକ୍ଷ_ଗଣ୍ଠି*cur)
{
      ପ୍ରତିଜ୍ଞା(!cur->leaf_flag);
      ଯଦି (cur->parent) ପ୍ରତିଜ୍ଞା(! cur->parent->leaf_flag);

      ଯେତେବେଳେ (cur->ଗଣନା_2 == ଗଣ୍ଠି_width) {
	      /* Create a new node to hold half the data from cur. */
	    ସଂରଚନା ବୃକ୍ଷ_ଗଣ୍ଠି *new_ଗଣ୍ଠି = ନୂତନ ସଂରଚନା ବୃକ୍ଷ_ଗଣ୍ଠି;
	    new_ଗଣ୍ଠି->leaf_flag = ମିଥ୍ୟା;
	    new_ଗଣ୍ଠି->ଗଣନା_2 = cur->ଗଣନା_2 / 2;
	      /* cur is not root; new_node becomes sibling. */
	    ଯଦି (cur->parent) new_ଗଣ୍ଠି->parent = cur->parent;

	      /* Move the last half of the data from the end of the old node
	       * to the beginning of the new node. At the same time, reduce
	       * the size of the old node. */
	    unsigned idx1 = new_ଗଣ୍ଠି->ଗଣନା_2;
	    unsigned idx2 = cur->ଗଣନା_2;
	    ଯେତେବେଳେ (idx1 > 0) {
		  idx1 -= 1;
		  idx2 -= 1;
		  new_ଗଣ୍ଠି->child[idx1] = cur->child[idx2];
		  new_ଗଣ୍ଠି->child[idx1]->parent = new_ଗଣ୍ଠି;
		  cur->ଗଣନା_2 -= 1;
	    }

	    ପ୍ରତିଜ୍ଞା(new_ଗଣ୍ଠି->ଗଣନା_2 > 0);
	    ପ୍ରତିଜ୍ଞା(cur->ଗଣନା_2 > 0);

	    ଯଦି (cur->parent == 0) {
		    /* cur is root. Move first half of children to another
		     * new node, and put the two new nodes in cur. The plan
		     * here is to make cur into the new root and split its
		     * contents into 2 children. */

		  new_ଗଣ୍ଠି->parent = cur;
		  ସଂରଚନା ବୃକ୍ଷ_ଗଣ୍ଠି*new2_ଗଣ୍ଠି = ନୂତନ ସଂରଚନା ବୃକ୍ଷ_ଗଣ୍ଠି;
		  new2_ଗଣ୍ଠି->leaf_flag = ମିଥ୍ୟା;
		  new2_ଗଣ୍ଠି->ଗଣନା_2 = cur->ଗଣନା_2;
		  new2_ଗଣ୍ଠି->parent = cur;
		  ପାଇଁ (unsigned idx = 0; idx < cur->ଗଣନା_2; idx += 1) {
			new2_ଗଣ୍ଠି->child[idx] = cur->child[idx];
			new2_ଗଣ୍ଠି->child[idx]->parent = new2_ଗଣ୍ଠି;
		  }
		  cur->child[0] = new2_ଗଣ୍ଠି;
		  cur->child[1] = new_ଗଣ୍ଠି;
		  cur->ଗଣନା_2 = 2;
		    /* no more ancestors, stop the while loop */
		  ଭାଙ୍ଗ;
	    }

	      /* cur is not root. hook new_node to cur->parent. */
	    unsigned idx = 0;
	    ଯେତେବେଳେ (cur->parent->child[idx] != cur) {
		  ପ୍ରତିଜ୍ଞା(idx < cur->parent->ଗଣନା_2);
		  idx += 1;
	    }
	    idx += 1;

	    ପାଇଁ (unsigned tmp = cur->parent->ଗଣନା_2 ;  tmp > idx ;  tmp -= 1) {
		  cur->parent->child[tmp] = cur->parent->child[tmp-1];
	    }

	    cur->parent->child[idx] = new_ଗଣ୍ଠି;
	    cur->parent->ଗଣନା_2 += 1;

	      /* check the ancestor */
	    cur = cur->parent;
      }
}

/*
 * This function takes a leaf node and splits in into two. Move half
 * the leaf keys into the new node, and add the new leaf into the
 * parent node.
 */
ସ୍ଥିର ସଂରଚନା ବୃକ୍ଷ_ଗଣ୍ଠି* split_leaf_(ସଂରଚନା ବୃକ୍ଷ_ଗଣ୍ଠି*cur)
{
      ପ୍ରତିଜ୍ଞା(cur->leaf_flag);
      ପ୍ରତିଜ୍ଞା(cur->parent);
      ପ୍ରତିଜ୍ଞା(! cur->parent->leaf_flag);

	/* Create a new leaf to hold half the data from the old leaf. */
      ସଂରଚନା ବୃକ୍ଷ_ଗଣ୍ଠି*new_leaf = ନୂତନ ସଂରଚନା ବୃକ୍ଷ_ଗଣ୍ଠି;
      new_leaf->leaf_flag = ସତ୍ୟ;
      new_leaf->ଗଣନା_2 = cur->ଗଣନା_2 / 2;
      new_leaf->parent = cur->parent;

	/* Move the last half of the data from the end of the old leaf
	   to the beginning of the new leaf. At the same time, reduce
	   the size of the old leaf. */
      unsigned idx1 = new_leaf->ଗଣନା_2;
      unsigned idx2 = cur->ଗଣନା_2;
      ଯେତେବେଳେ (idx1 > 0) {
	    idx1 -= 1;
	    idx2 -= 1;
	    new_leaf->leaf[idx1] = cur->leaf[idx2];
	    cur->ଗଣନା_2 -= 1;
      }

      ପ୍ରତିଜ୍ଞା(new_leaf->ଗଣନା_2 > 0);
      ପ୍ରତିଜ୍ଞା(cur->ଗଣନା_2 > 0);

      unsigned idx = 0;
      ଯେତେବେଳେ (cur->parent->child[idx] != cur) {
	    ପ୍ରତିଜ୍ଞା(idx < cur->parent->ଗଣନା_2);
	    idx += 1;
      }

      idx += 1;

      ପାଇଁ (unsigned tmp = cur->parent->ଗଣନା_2 ;  tmp > idx ;  tmp -= 1)
	    cur->parent->child[tmp] = cur->parent->child[tmp-1];

      cur->parent->child[idx] = new_leaf;
      cur->parent->ଗଣନା_2 += 1;

      ଯଦି (cur->parent->ଗଣନା_2 == ଗଣ୍ଠି_width)
	    split_ଗଣ୍ଠି(cur->parent);

      ଫେରନ୍ତୁ new_leaf;
}


/*
 * This function searches tree recursively for the key. If the value
 * is not found (and we are at a leaf) then set the key with the given
 * value. If the key is found, set the value only if the force_flag is
 * true.
 */

ପ୍ରତୀକ_ମୂଲ୍ୟ_t ପ୍ରତୀକ_table_s::find_ମୂଲ୍ୟ(ସଂରଚନା ବୃକ୍ଷ_ଗଣ୍ଠି*cur,
					   ଅଚଳ ଅକ୍ଷର*ଚାବି, ପ୍ରତୀକ_ମୂଲ୍ୟ_t val,
					   ତାର୍କିକ force_flag)
{
      ଯଦି (cur->leaf_flag) {

	    unsigned idx = 0;
	    ପାଇଁ (;;) {
		    /* If we run out of keys in the leaf, then add
		       this at the end of the leaf. */
		  ଯଦି (idx == cur->ଗଣନା_2) {
			cur->leaf[idx].ଚାବି = ଚାବି_strdup(ଚାବି);
			cur->leaf[idx].val = val;
			cur->ଗଣନା_2 += 1;
			ଯଦି (cur->ଗଣନା_2 == leaf_width)
			      split_leaf_(cur);

			ଫେରନ୍ତୁ val;
		  }

		  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc = strcmp(ଚାବି, cur->leaf[idx].ଚାବି);

		    /* If we found the key already in the table, then
		       set the new value and break. */
		  ଯଦି (rc == 0) {
			ଯଦି (force_flag)
			      cur->leaf[idx].val = val;
			ଫେରନ୍ତୁ cur->leaf[idx].val;
		  }

		    /* If this key goes before the current key, then
		       push all the following entries back and add
		       this key here. */
		  ଯଦି (rc < 0) {
			ପାଇଁ (unsigned tmp = cur->ଗଣନା_2; tmp > idx; tmp -= 1)
			      cur->leaf[tmp] = cur->leaf[tmp-1];

			cur->leaf[idx].ଚାବି = ଚାବି_strdup(ଚାବି);
			cur->leaf[idx].val = val;
			cur->ଗଣନା_2 += 1;
			ଯଦି (cur->ଗଣନା_2 == leaf_width)
			      split_leaf_(cur);

			ଫେରନ୍ତୁ val;
		  }

		  idx += 1;
	    }

      } ଅନ୍ୟଥା {
	      /* Do a binary search within the inner node. */
	    unsigned min = 0;
	    unsigned max = cur->ଗଣନା_2;
	    unsigned idx = max/2;
	    ପାଇଁ (;;) {
		  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc = strcmp(ଚାବି, ଗଣ୍ଠି_last_ଚାବି(cur->child[idx]));
		  ଯଦି (rc == 0) {
			ଫେରନ୍ତୁ find_ମୂଲ୍ୟ(cur->child[idx],
					   ଚାବି, val, force_flag);
		  }

		  ଯଦି (rc > 0) {
			min = idx + 1;
			ଯଦି (min == cur->ଗଣନା_2)
			      ଫେରନ୍ତୁ find_ମୂଲ୍ୟ(cur->child[idx],
						 ଚାବି, val, force_flag);
			ଯଦି (min == max)
			      ଫେରନ୍ତୁ find_ମୂଲ୍ୟ(cur->child[max],
						 ଚାବି, val, force_flag);

			idx = min + (max-min)/2;

		  } ଅନ୍ୟଥା  {
			max = idx;
			ଯଦି (idx == min)
			      ଫେରନ୍ତୁ find_ମୂଲ୍ୟ(cur->child[idx],
						 ଚାବି, val, force_flag);
			idx = min + (max-min)/2;
		  }
	    }
      }

      ପ୍ରତିଜ୍ଞା(0);
      { ପ୍ରତୀକ_ମୂଲ୍ୟ_t tmp;
        tmp.ptr = 0;
        ଫେରନ୍ତୁ tmp;
      }
}

ଶୂନ୍ୟ ପ୍ରତୀକ_table_s::sym_set_ମୂଲ୍ୟ(ଅଚଳ ଅକ୍ଷର*ଚାବି, ପ୍ରତୀକ_ମୂଲ୍ୟ_t val)
{
      ଯଦି (ମୂଳ->ଗଣନା_2 == 0) {
	      /* Handle the special case that this is the very first
		 value in the symbol table. Create the first leaf node
		 and initialize the pointers. */
	    ସଂରଚନା ବୃକ୍ଷ_ଗଣ୍ଠି*cur = ନୂତନ ସଂରଚନା ବୃକ୍ଷ_ଗଣ୍ଠି;
	    cur->leaf_flag = ସତ୍ୟ;
	    cur->parent = ମୂଳ;
	    cur->ଗଣନା_2 = 1;
	    cur->leaf[0].ଚାବି = ଚାବି_strdup(ଚାବି);
	    cur->leaf[0].val = val;

	    ମୂଳ->ଗଣନା_2 = 1;
	    ମୂଳ->child[0] = cur;
      } ଅନ୍ୟଥା {
	    find_ମୂଲ୍ୟ(ମୂଳ, ଚାବି, val, ସତ୍ୟ);
      }
}

ପ୍ରତୀକ_ମୂଲ୍ୟ_t ପ୍ରତୀକ_table_s::sym_get_ମୂଲ୍ୟ(ଅଚଳ ଅକ୍ଷର*ଚାବି)
{
      ପ୍ରତୀକ_ମୂଲ୍ୟ_t def;
      def.ptr = 0;

      ଯଦି (ମୂଳ->ଗଣନା_2 == 0) {
	      /* Handle the special case that this is the very first
		 value in the symbol table. Create the first leaf node
		 and initialize the pointers. */
	    ସଂରଚନା ବୃକ୍ଷ_ଗଣ୍ଠି*cur = ନୂତନ ସଂରଚନା ବୃକ୍ଷ_ଗଣ୍ଠି;
	    cur->leaf_flag = ସତ୍ୟ;
	    cur->parent = ମୂଳ;
	    cur->ଗଣନା_2 = 1;
	    cur->leaf[0].ଚାବି = ଚାବି_strdup(ଚାବି);
	    cur->leaf[0].val = def;

	    ମୂଳ->ଗଣନା_2 = 1;
	    ମୂଳ->child[0] = cur;
	    ଫେରନ୍ତୁ cur->leaf[0].val;
      } ଅନ୍ୟଥା {
	    ଫେରନ୍ତୁ find_ମୂଲ୍ୟ(ମୂଳ, ଚାବି, def, ମିଥ୍ୟା);
      }
}

ପ୍ରତୀକ_table_s::~ପ୍ରତୀକ_table_s()
{
      delete_ପ୍ରତୀକ_ଗଣ୍ଠି(ମୂଳ);
      ଯେତେବେଳେ (str_chunk) {
	    ଚାବି_strings*tmp = str_chunk;
	    str_chunk = tmp->next;
	    delete tmp;
      }
}
