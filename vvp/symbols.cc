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
সংরচনা চাবি_strings {
      সংরচনা চাবি_strings*next;
      ৱর্ণ data[64*1024 - পরিমাণ(সংরচনা চাবি_strings*)];
};

ৱর্ণ*প্ৰতীক_table_s::চাবি_strdup(ধ্রুৱ ৱর্ণ*str)
{
      unsigned len = strlen(str);
      প্রতিজ্ঞা( (len+1) <= পরিমাণ str_chunk->data );

      যদি ( (len+1) > (পরিমাণ str_chunk->data - str_used) ) {
	    চাবি_strings*tmp = নূতন চাবি_strings;
	    tmp->next = str_chunk;
	    str_chunk = tmp;
	    str_used = 0;
      }

      ৱর্ণ*res = str_chunk->data + str_used;
      str_used += len + 1;
      strcpy(res, str);
      ঘূৰাই res;
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

ধ্রুৱ unsigned leaf_width = 254;
ধ্রুৱ unsigned গ্ৰন্থি_width = 508;

সংরচনা বৃক্ষ_data {
      ৱর্ণ*চাবি;
      প্ৰতীক_মান_t val;
};

সংরচনা বৃক্ষ_গ্ৰন্থি {
      তার্কিক leaf_flag;
      unsigned গুনতি;
      সংরচনা বৃক্ষ_গ্ৰন্থি*parent;

      সঙ্ঘ {
	    সংরচনা বৃক্ষ_data leaf[leaf_width];
	    সংরচনা বৃক্ষ_গ্ৰন্থি *child[গ্ৰন্থি_width];
      };

};

স্থির অন্তঃস্থ ৱর্ণ* গ্ৰন্থি_last_চাবি(সংরচনা বৃক্ষ_গ্ৰন্থি*গ্ৰন্থি)
{
      যেতিয়ালৈ (গ্ৰন্থি->leaf_flag == মিছা)
	    গ্ৰন্থি = গ্ৰন্থি->child[গ্ৰন্থি->গুনতি-1];

      ঘূৰাই গ্ৰন্থি->leaf[গ্ৰন্থি->গুনতি-1].চাবি;
}

/*
 * Allocate a new symbol table means creating the table structure and
 * a root node, and initializing the pointers and members of the root
 * node.
 */
প্ৰতীক_table_s::প্ৰতীক_table_s()
{
      মূল = নূতন সংরচনা বৃক্ষ_গ্ৰন্থি;
      মূল->leaf_flag = মিছা;
      মূল->গুনতি = 0;
      মূল->parent = 0;

      str_chunk = নূতন চাবি_strings;
      str_chunk->next = 0;
      str_used = 0;
}

স্থির শূন্য delete_প্ৰতীক_গ্ৰন্থি(সংরচনা বৃক্ষ_গ্ৰন্থি*cur)
{
      যদি (! cur->leaf_flag) {
	    বাবে (unsigned idx = 0 ;  idx < cur->গুনতি ;  idx += 1)
		  delete_প্ৰতীক_গ্ৰন্থি(cur->child[idx]);
      }

      delete cur;
}

/* Do as split_leaf_ does, but for nodes. */
স্থির শূন্য split_গ্ৰন্থি(সংরচনা বৃক্ষ_গ্ৰন্থি*cur)
{
      প্রতিজ্ঞা(!cur->leaf_flag);
      যদি (cur->parent) প্রতিজ্ঞা(! cur->parent->leaf_flag);

      যেতিয়ালৈ (cur->গুনতি == গ্ৰন্থি_width) {
	      /* Create a new node to hold half the data from cur. */
	    সংরচনা বৃক্ষ_গ্ৰন্থি *new_গ্ৰন্থি = নূতন সংরচনা বৃক্ষ_গ্ৰন্থি;
	    new_গ্ৰন্থি->leaf_flag = মিছা;
	    new_গ্ৰন্থি->গুনতি = cur->গুনতি / 2;
	      /* cur is not root; new_node becomes sibling. */
	    যদি (cur->parent) new_গ্ৰন্থি->parent = cur->parent;

	      /* Move the last half of the data from the end of the old node
	       * to the beginning of the new node. At the same time, reduce
	       * the size of the old node. */
	    unsigned idx1 = new_গ্ৰন্থি->গুনতি;
	    unsigned idx2 = cur->গুনতি;
	    যেতিয়ালৈ (idx1 > 0) {
		  idx1 -= 1;
		  idx2 -= 1;
		  new_গ্ৰন্থি->child[idx1] = cur->child[idx2];
		  new_গ্ৰন্থি->child[idx1]->parent = new_গ্ৰন্থি;
		  cur->গুনতি -= 1;
	    }

	    প্রতিজ্ঞা(new_গ্ৰন্থি->গুনতি > 0);
	    প্রতিজ্ঞা(cur->গুনতি > 0);

	    যদি (cur->parent == 0) {
		    /* cur is root. Move first half of children to another
		     * new node, and put the two new nodes in cur. The plan
		     * here is to make cur into the new root and split its
		     * contents into 2 children. */

		  new_গ্ৰন্থি->parent = cur;
		  সংরচনা বৃক্ষ_গ্ৰন্থি*new2_গ্ৰন্থি = নূতন সংরচনা বৃক্ষ_গ্ৰন্থি;
		  new2_গ্ৰন্থি->leaf_flag = মিছা;
		  new2_গ্ৰন্থি->গুনতি = cur->গুনতি;
		  new2_গ্ৰন্থি->parent = cur;
		  বাবে (unsigned idx = 0; idx < cur->গুনতি; idx += 1) {
			new2_গ্ৰন্থি->child[idx] = cur->child[idx];
			new2_গ্ৰন্থি->child[idx]->parent = new2_গ্ৰন্থি;
		  }
		  cur->child[0] = new2_গ্ৰন্থি;
		  cur->child[1] = new_গ্ৰন্থি;
		  cur->গুনতি = 2;
		    /* no more ancestors, stop the while loop */
		  ভঙ্গ;
	    }

	      /* cur is not root. hook new_node to cur->parent. */
	    unsigned idx = 0;
	    যেতিয়ালৈ (cur->parent->child[idx] != cur) {
		  প্রতিজ্ঞা(idx < cur->parent->গুনতি);
		  idx += 1;
	    }
	    idx += 1;

	    বাবে (unsigned tmp = cur->parent->গুনতি ;  tmp > idx ;  tmp -= 1) {
		  cur->parent->child[tmp] = cur->parent->child[tmp-1];
	    }

	    cur->parent->child[idx] = new_গ্ৰন্থি;
	    cur->parent->গুনতি += 1;

	      /* check the ancestor */
	    cur = cur->parent;
      }
}

/*
 * This function takes a leaf node and splits in into two. Move half
 * the leaf keys into the new node, and add the new leaf into the
 * parent node.
 */
স্থির সংরচনা বৃক্ষ_গ্ৰন্থি* split_leaf_(সংরচনা বৃক্ষ_গ্ৰন্থি*cur)
{
      প্রতিজ্ঞা(cur->leaf_flag);
      প্রতিজ্ঞা(cur->parent);
      প্রতিজ্ঞা(! cur->parent->leaf_flag);

	/* Create a new leaf to hold half the data from the old leaf. */
      সংরচনা বৃক্ষ_গ্ৰন্থি*new_leaf = নূতন সংরচনা বৃক্ষ_গ্ৰন্থি;
      new_leaf->leaf_flag = সঁচা;
      new_leaf->গুনতি = cur->গুনতি / 2;
      new_leaf->parent = cur->parent;

	/* Move the last half of the data from the end of the old leaf
	   to the beginning of the new leaf. At the same time, reduce
	   the size of the old leaf. */
      unsigned idx1 = new_leaf->গুনতি;
      unsigned idx2 = cur->গুনতি;
      যেতিয়ালৈ (idx1 > 0) {
	    idx1 -= 1;
	    idx2 -= 1;
	    new_leaf->leaf[idx1] = cur->leaf[idx2];
	    cur->গুনতি -= 1;
      }

      প্রতিজ্ঞা(new_leaf->গুনতি > 0);
      প্রতিজ্ঞা(cur->গুনতি > 0);

      unsigned idx = 0;
      যেতিয়ালৈ (cur->parent->child[idx] != cur) {
	    প্রতিজ্ঞা(idx < cur->parent->গুনতি);
	    idx += 1;
      }

      idx += 1;

      বাবে (unsigned tmp = cur->parent->গুনতি ;  tmp > idx ;  tmp -= 1)
	    cur->parent->child[tmp] = cur->parent->child[tmp-1];

      cur->parent->child[idx] = new_leaf;
      cur->parent->গুনতি += 1;

      যদি (cur->parent->গুনতি == গ্ৰন্থি_width)
	    split_গ্ৰন্থি(cur->parent);

      ঘূৰাই new_leaf;
}


/*
 * This function searches tree recursively for the key. If the value
 * is not found (and we are at a leaf) then set the key with the given
 * value. If the key is found, set the value only if the force_flag is
 * true.
 */

প্ৰতীক_মান_t প্ৰতীক_table_s::find_মান(সংরচনা বৃক্ষ_গ্ৰন্থি*cur,
					   ধ্রুৱ ৱর্ণ*চাবি, প্ৰতীক_মান_t val,
					   তার্কিক force_flag)
{
      যদি (cur->leaf_flag) {

	    unsigned idx = 0;
	    বাবে (;;) {
		    /* If we run out of keys in the leaf, then add
		       this at the end of the leaf. */
		  যদি (idx == cur->গুনতি) {
			cur->leaf[idx].চাবি = চাবি_strdup(চাবি);
			cur->leaf[idx].val = val;
			cur->গুনতি += 1;
			যদি (cur->গুনতি == leaf_width)
			      split_leaf_(cur);

			ঘূৰাই val;
		  }

		  পূর্ণাঙ্ক rc = strcmp(চাবি, cur->leaf[idx].চাবি);

		    /* If we found the key already in the table, then
		       set the new value and break. */
		  যদি (rc == 0) {
			যদি (force_flag)
			      cur->leaf[idx].val = val;
			ঘূৰাই cur->leaf[idx].val;
		  }

		    /* If this key goes before the current key, then
		       push all the following entries back and add
		       this key here. */
		  যদি (rc < 0) {
			বাবে (unsigned tmp = cur->গুনতি; tmp > idx; tmp -= 1)
			      cur->leaf[tmp] = cur->leaf[tmp-1];

			cur->leaf[idx].চাবি = চাবি_strdup(চাবি);
			cur->leaf[idx].val = val;
			cur->গুনতি += 1;
			যদি (cur->গুনতি == leaf_width)
			      split_leaf_(cur);

			ঘূৰাই val;
		  }

		  idx += 1;
	    }

      } অন্যথা {
	      /* Do a binary search within the inner node. */
	    unsigned min = 0;
	    unsigned max = cur->গুনতি;
	    unsigned idx = max/2;
	    বাবে (;;) {
		  পূর্ণাঙ্ক rc = strcmp(চাবি, গ্ৰন্থি_last_চাবি(cur->child[idx]));
		  যদি (rc == 0) {
			ঘূৰাই find_মান(cur->child[idx],
					   চাবি, val, force_flag);
		  }

		  যদি (rc > 0) {
			min = idx + 1;
			যদি (min == cur->গুনতি)
			      ঘূৰাই find_মান(cur->child[idx],
						 চাবি, val, force_flag);
			যদি (min == max)
			      ঘূৰাই find_মান(cur->child[max],
						 চাবি, val, force_flag);

			idx = min + (max-min)/2;

		  } অন্যথা  {
			max = idx;
			যদি (idx == min)
			      ঘূৰাই find_মান(cur->child[idx],
						 চাবি, val, force_flag);
			idx = min + (max-min)/2;
		  }
	    }
      }

      প্রতিজ্ঞা(0);
      { প্ৰতীক_মান_t tmp;
        tmp.ptr = 0;
        ঘূৰাই tmp;
      }
}

শূন্য প্ৰতীক_table_s::sym_set_মান(ধ্রুৱ ৱর্ণ*চাবি, প্ৰতীক_মান_t val)
{
      যদি (মূল->গুনতি == 0) {
	      /* Handle the special case that this is the very first
		 value in the symbol table. Create the first leaf node
		 and initialize the pointers. */
	    সংরচনা বৃক্ষ_গ্ৰন্থি*cur = নূতন সংরচনা বৃক্ষ_গ্ৰন্থি;
	    cur->leaf_flag = সঁচা;
	    cur->parent = মূল;
	    cur->গুনতি = 1;
	    cur->leaf[0].চাবি = চাবি_strdup(চাবি);
	    cur->leaf[0].val = val;

	    মূল->গুনতি = 1;
	    মূল->child[0] = cur;
      } অন্যথা {
	    find_মান(মূল, চাবি, val, সঁচা);
      }
}

প্ৰতীক_মান_t প্ৰতীক_table_s::sym_get_মান(ধ্রুৱ ৱর্ণ*চাবি)
{
      প্ৰতীক_মান_t def;
      def.ptr = 0;

      যদি (মূল->গুনতি == 0) {
	      /* Handle the special case that this is the very first
		 value in the symbol table. Create the first leaf node
		 and initialize the pointers. */
	    সংরচনা বৃক্ষ_গ্ৰন্থি*cur = নূতন সংরচনা বৃক্ষ_গ্ৰন্থি;
	    cur->leaf_flag = সঁচা;
	    cur->parent = মূল;
	    cur->গুনতি = 1;
	    cur->leaf[0].চাবি = চাবি_strdup(চাবি);
	    cur->leaf[0].val = def;

	    মূল->গুনতি = 1;
	    মূল->child[0] = cur;
	    ঘূৰাই cur->leaf[0].val;
      } অন্যথা {
	    ঘূৰাই find_মান(মূল, চাবি, def, মিছা);
      }
}

প্ৰতীক_table_s::~প্ৰতীক_table_s()
{
      delete_প্ৰতীক_গ্ৰন্থি(মূল);
      যেতিয়ালৈ (str_chunk) {
	    চাবি_strings*tmp = str_chunk;
	    str_chunk = tmp->next;
	    delete tmp;
      }
}
