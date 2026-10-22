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
কাঠামো চাবি_strings {
      কাঠামো চাবি_strings*পরবর্তী_2;
      অক্ষর তথ্য[64*1024 - আকার(কাঠামো চাবি_strings*)];
};

অক্ষর*প্রতীক_table_s::চাবি_strdup(ধ্রুবক অক্ষর*str)
{
      unsigned len = strlen(str);
      প্রতিজ্ঞা( (len+1) <= আকার str_chunk->তথ্য );

      যদি ( (len+1) > (আকার str_chunk->তথ্য - str_used) ) {
	    চাবি_strings*tmp = নতুন চাবি_strings;
	    tmp->পরবর্তী_2 = str_chunk;
	    str_chunk = tmp;
	    str_used = 0;
      }

      অক্ষর*res = str_chunk->তথ্য + str_used;
      str_used += len + 1;
      strcpy(res, str);
      ফেরত res;
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

ধ্রুবক unsigned leaf_width = 254;
ধ্রুবক unsigned গ্রন্থি_width = 508;

কাঠামো বৃক্ষ_তথ্য {
      অক্ষর*চাবি;
      প্রতীক_মান_t val;
};

কাঠামো বৃক্ষ_গ্রন্থি {
      যৌক্তিক leaf_flag;
      unsigned গুনতি;
      কাঠামো বৃক্ষ_গ্রন্থি*parent;

      সংঘ {
	    কাঠামো বৃক্ষ_তথ্য leaf[leaf_width];
	    কাঠামো বৃক্ষ_গ্রন্থি *child[গ্রন্থি_width];
      };

};

স্থির অন্তঃরেখ অক্ষর* গ্রন্থি_last_চাবি(কাঠামো বৃক্ষ_গ্রন্থি*গ্রন্থি)
{
      যখন (গ্রন্থি->leaf_flag == মিথ্যা)
	    গ্রন্থি = গ্রন্থি->child[গ্রন্থি->গুনতি-1];

      ফেরত গ্রন্থি->leaf[গ্রন্থি->গুনতি-1].চাবি;
}

/*
 * Allocate a new symbol table means creating the table structure and
 * a root node, and initializing the pointers and members of the root
 * node.
 */
প্রতীক_table_s::প্রতীক_table_s()
{
      মূল = নতুন কাঠামো বৃক্ষ_গ্রন্থি;
      মূল->leaf_flag = মিথ্যা;
      মূল->গুনতি = 0;
      মূল->parent = 0;

      str_chunk = নতুন চাবি_strings;
      str_chunk->পরবর্তী_2 = 0;
      str_used = 0;
}

স্থির শূন্য delete_প্রতীক_গ্রন্থি(কাঠামো বৃক্ষ_গ্রন্থি*cur)
{
      যদি (! cur->leaf_flag) {
	    জন্য (unsigned idx = 0 ;  idx < cur->গুনতি ;  idx += 1)
		  delete_প্রতীক_গ্রন্থি(cur->child[idx]);
      }

      delete cur;
}

/* Do as split_leaf_ does, but for nodes. */
স্থির শূন্য split_গ্রন্থি(কাঠামো বৃক্ষ_গ্রন্থি*cur)
{
      প্রতিজ্ঞা(!cur->leaf_flag);
      যদি (cur->parent) প্রতিজ্ঞা(! cur->parent->leaf_flag);

      যখন (cur->গুনতি == গ্রন্থি_width) {
	      /* Create a new node to hold half the data from cur. */
	    কাঠামো বৃক্ষ_গ্রন্থি *new_গ্রন্থি = নতুন কাঠামো বৃক্ষ_গ্রন্থি;
	    new_গ্রন্থি->leaf_flag = মিথ্যা;
	    new_গ্রন্থি->গুনতি = cur->গুনতি / 2;
	      /* cur is not root; new_node becomes sibling. */
	    যদি (cur->parent) new_গ্রন্থি->parent = cur->parent;

	      /* Move the last half of the data from the end of the old node
	       * to the beginning of the new node. At the same time, reduce
	       * the size of the old node. */
	    unsigned idx1 = new_গ্রন্থি->গুনতি;
	    unsigned idx2 = cur->গুনতি;
	    যখন (idx1 > 0) {
		  idx1 -= 1;
		  idx2 -= 1;
		  new_গ্রন্থি->child[idx1] = cur->child[idx2];
		  new_গ্রন্থি->child[idx1]->parent = new_গ্রন্থি;
		  cur->গুনতি -= 1;
	    }

	    প্রতিজ্ঞা(new_গ্রন্থি->গুনতি > 0);
	    প্রতিজ্ঞা(cur->গুনতি > 0);

	    যদি (cur->parent == 0) {
		    /* cur is root. Move first half of children to another
		     * new node, and put the two new nodes in cur. The plan
		     * here is to make cur into the new root and split its
		     * contents into 2 children. */

		  new_গ্রন্থি->parent = cur;
		  কাঠামো বৃক্ষ_গ্রন্থি*new2_গ্রন্থি = নতুন কাঠামো বৃক্ষ_গ্রন্থি;
		  new2_গ্রন্থি->leaf_flag = মিথ্যা;
		  new2_গ্রন্থি->গুনতি = cur->গুনতি;
		  new2_গ্রন্থি->parent = cur;
		  জন্য (unsigned idx = 0; idx < cur->গুনতি; idx += 1) {
			new2_গ্রন্থি->child[idx] = cur->child[idx];
			new2_গ্রন্থি->child[idx]->parent = new2_গ্রন্থি;
		  }
		  cur->child[0] = new2_গ্রন্থি;
		  cur->child[1] = new_গ্রন্থি;
		  cur->গুনতি = 2;
		    /* no more ancestors, stop the while loop */
		  বিরতি;
	    }

	      /* cur is not root. hook new_node to cur->parent. */
	    unsigned idx = 0;
	    যখন (cur->parent->child[idx] != cur) {
		  প্রতিজ্ঞা(idx < cur->parent->গুনতি);
		  idx += 1;
	    }
	    idx += 1;

	    জন্য (unsigned tmp = cur->parent->গুনতি ;  tmp > idx ;  tmp -= 1) {
		  cur->parent->child[tmp] = cur->parent->child[tmp-1];
	    }

	    cur->parent->child[idx] = new_গ্রন্থি;
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
স্থির কাঠামো বৃক্ষ_গ্রন্থি* split_leaf_(কাঠামো বৃক্ষ_গ্রন্থি*cur)
{
      প্রতিজ্ঞা(cur->leaf_flag);
      প্রতিজ্ঞা(cur->parent);
      প্রতিজ্ঞা(! cur->parent->leaf_flag);

	/* Create a new leaf to hold half the data from the old leaf. */
      কাঠামো বৃক্ষ_গ্রন্থি*new_leaf = নতুন কাঠামো বৃক্ষ_গ্রন্থি;
      new_leaf->leaf_flag = সত্য;
      new_leaf->গুনতি = cur->গুনতি / 2;
      new_leaf->parent = cur->parent;

	/* Move the last half of the data from the end of the old leaf
	   to the beginning of the new leaf. At the same time, reduce
	   the size of the old leaf. */
      unsigned idx1 = new_leaf->গুনতি;
      unsigned idx2 = cur->গুনতি;
      যখন (idx1 > 0) {
	    idx1 -= 1;
	    idx2 -= 1;
	    new_leaf->leaf[idx1] = cur->leaf[idx2];
	    cur->গুনতি -= 1;
      }

      প্রতিজ্ঞা(new_leaf->গুনতি > 0);
      প্রতিজ্ঞা(cur->গুনতি > 0);

      unsigned idx = 0;
      যখন (cur->parent->child[idx] != cur) {
	    প্রতিজ্ঞা(idx < cur->parent->গুনতি);
	    idx += 1;
      }

      idx += 1;

      জন্য (unsigned tmp = cur->parent->গুনতি ;  tmp > idx ;  tmp -= 1)
	    cur->parent->child[tmp] = cur->parent->child[tmp-1];

      cur->parent->child[idx] = new_leaf;
      cur->parent->গুনতি += 1;

      যদি (cur->parent->গুনতি == গ্রন্থি_width)
	    split_গ্রন্থি(cur->parent);

      ফেরত new_leaf;
}


/*
 * This function searches tree recursively for the key. If the value
 * is not found (and we are at a leaf) then set the key with the given
 * value. If the key is found, set the value only if the force_flag is
 * true.
 */

প্রতীক_মান_t প্রতীক_table_s::find_মান(কাঠামো বৃক্ষ_গ্রন্থি*cur,
					   ধ্রুবক অক্ষর*চাবি, প্রতীক_মান_t val,
					   যৌক্তিক force_flag)
{
      যদি (cur->leaf_flag) {

	    unsigned idx = 0;
	    জন্য (;;) {
		    /* If we run out of keys in the leaf, then add
		       this at the end of the leaf. */
		  যদি (idx == cur->গুনতি) {
			cur->leaf[idx].চাবি = চাবি_strdup(চাবি);
			cur->leaf[idx].val = val;
			cur->গুনতি += 1;
			যদি (cur->গুনতি == leaf_width)
			      split_leaf_(cur);

			ফেরত val;
		  }

		  পূর্ণসংখ্যা rc = strcmp(চাবি, cur->leaf[idx].চাবি);

		    /* If we found the key already in the table, then
		       set the new value and break. */
		  যদি (rc == 0) {
			যদি (force_flag)
			      cur->leaf[idx].val = val;
			ফেরত cur->leaf[idx].val;
		  }

		    /* If this key goes before the current key, then
		       push all the following entries back and add
		       this key here. */
		  যদি (rc < 0) {
			জন্য (unsigned tmp = cur->গুনতি; tmp > idx; tmp -= 1)
			      cur->leaf[tmp] = cur->leaf[tmp-1];

			cur->leaf[idx].চাবি = চাবি_strdup(চাবি);
			cur->leaf[idx].val = val;
			cur->গুনতি += 1;
			যদি (cur->গুনতি == leaf_width)
			      split_leaf_(cur);

			ফেরত val;
		  }

		  idx += 1;
	    }

      } অন্যথা {
	      /* Do a binary search within the inner node. */
	    unsigned min = 0;
	    unsigned max = cur->গুনতি;
	    unsigned idx = max/2;
	    জন্য (;;) {
		  পূর্ণসংখ্যা rc = strcmp(চাবি, গ্রন্থি_last_চাবি(cur->child[idx]));
		  যদি (rc == 0) {
			ফেরত find_মান(cur->child[idx],
					   চাবি, val, force_flag);
		  }

		  যদি (rc > 0) {
			min = idx + 1;
			যদি (min == cur->গুনতি)
			      ফেরত find_মান(cur->child[idx],
						 চাবি, val, force_flag);
			যদি (min == max)
			      ফেরত find_মান(cur->child[max],
						 চাবি, val, force_flag);

			idx = min + (max-min)/2;

		  } অন্যথা  {
			max = idx;
			যদি (idx == min)
			      ফেরত find_মান(cur->child[idx],
						 চাবি, val, force_flag);
			idx = min + (max-min)/2;
		  }
	    }
      }

      প্রতিজ্ঞা(0);
      { প্রতীক_মান_t tmp;
        tmp.ptr = 0;
        ফেরত tmp;
      }
}

শূন্য প্রতীক_table_s::sym_set_মান(ধ্রুবক অক্ষর*চাবি, প্রতীক_মান_t val)
{
      যদি (মূল->গুনতি == 0) {
	      /* Handle the special case that this is the very first
		 value in the symbol table. Create the first leaf node
		 and initialize the pointers. */
	    কাঠামো বৃক্ষ_গ্রন্থি*cur = নতুন কাঠামো বৃক্ষ_গ্রন্থি;
	    cur->leaf_flag = সত্য;
	    cur->parent = মূল;
	    cur->গুনতি = 1;
	    cur->leaf[0].চাবি = চাবি_strdup(চাবি);
	    cur->leaf[0].val = val;

	    মূল->গুনতি = 1;
	    মূল->child[0] = cur;
      } অন্যথা {
	    find_মান(মূল, চাবি, val, সত্য);
      }
}

প্রতীক_মান_t প্রতীক_table_s::sym_get_মান(ধ্রুবক অক্ষর*চাবি)
{
      প্রতীক_মান_t def;
      def.ptr = 0;

      যদি (মূল->গুনতি == 0) {
	      /* Handle the special case that this is the very first
		 value in the symbol table. Create the first leaf node
		 and initialize the pointers. */
	    কাঠামো বৃক্ষ_গ্রন্থি*cur = নতুন কাঠামো বৃক্ষ_গ্রন্থি;
	    cur->leaf_flag = সত্য;
	    cur->parent = মূল;
	    cur->গুনতি = 1;
	    cur->leaf[0].চাবি = চাবি_strdup(চাবি);
	    cur->leaf[0].val = def;

	    মূল->গুনতি = 1;
	    মূল->child[0] = cur;
	    ফেরত cur->leaf[0].val;
      } অন্যথা {
	    ফেরত find_মান(মূল, চাবি, def, মিথ্যা);
      }
}

প্রতীক_table_s::~প্রতীক_table_s()
{
      delete_প্রতীক_গ্রন্থি(মূল);
      যখন (str_chunk) {
	    চাবি_strings*tmp = str_chunk;
	    str_chunk = tmp->পরবর্তী_2;
	    delete tmp;
      }
}
