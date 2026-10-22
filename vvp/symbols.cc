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
கட்டமைப்பு சாவி_strings {
      கட்டமைப்பு சாவி_strings*next;
      எழுத்து data[64*1024 - அளவு(கட்டமைப்பு சாவி_strings*)];
};

எழுத்து*குறியீடு_table_s::சாவி_strdup(மாறிலி எழுத்து*str)
{
      unsigned len = strlen(str);
      ப்ரதிஜ்ஞா( (len+1) <= அளவு str_chunk->data );

      எனில் ( (len+1) > (அளவு str_chunk->data - str_used) ) {
	    சாவி_strings*tmp = புதிய சாவி_strings;
	    tmp->next = str_chunk;
	    str_chunk = tmp;
	    str_used = 0;
      }

      எழுத்து*res = str_chunk->data + str_used;
      str_used += len + 1;
      strcpy(res, str);
      திரும்பு res;
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

மாறிலி unsigned leaf_width = 254;
மாறிலி unsigned முனை_width = 508;

கட்டமைப்பு மரம்_data {
      எழுத்து*சாவி;
      குறியீடு_மதிப்பு_t val;
};

கட்டமைப்பு மர_முனை {
      தருக்க leaf_flag;
      unsigned மொத்தம்;
      கட்டமைப்பு மர_முனை*parent;

      ஒன்றிப்பு {
	    கட்டமைப்பு மரம்_data leaf[leaf_width];
	    கட்டமைப்பு மர_முனை *child[முனை_width];
      };

};

நிலையான உள்வரி எழுத்து* முனை_last_சாவி(கட்டமைப்பு மர_முனை*முனை)
{
      போது (முனை->leaf_flag == பொய்)
	    முனை = முனை->child[முனை->மொத்தம்-1];

      திரும்பு முனை->leaf[முனை->மொத்தம்-1].சாவி;
}

/*
 * Allocate a new symbol table means creating the table structure and
 * a root node, and initializing the pointers and members of the root
 * node.
 */
குறியீடு_table_s::குறியீடு_table_s()
{
      வேர் = புதிய கட்டமைப்பு மர_முனை;
      வேர்->leaf_flag = பொய்;
      வேர்->மொத்தம் = 0;
      வேர்->parent = 0;

      str_chunk = புதிய சாவி_strings;
      str_chunk->next = 0;
      str_used = 0;
}

நிலையான வெறுமை delete_குறியீடு_முனை(கட்டமைப்பு மர_முனை*cur)
{
      எனில் (! cur->leaf_flag) {
	    க்கு (unsigned idx = 0 ;  idx < cur->மொத்தம் ;  idx += 1)
		  delete_குறியீடு_முனை(cur->child[idx]);
      }

      delete cur;
}

/* Do as split_leaf_ does, but for nodes. */
நிலையான வெறுமை split_முனை(கட்டமைப்பு மர_முனை*cur)
{
      ப்ரதிஜ்ஞா(!cur->leaf_flag);
      எனில் (cur->parent) ப்ரதிஜ்ஞா(! cur->parent->leaf_flag);

      போது (cur->மொத்தம் == முனை_width) {
	      /* Create a new node to hold half the data from cur. */
	    கட்டமைப்பு மர_முனை *new_முனை = புதிய கட்டமைப்பு மர_முனை;
	    new_முனை->leaf_flag = பொய்;
	    new_முனை->மொத்தம் = cur->மொத்தம் / 2;
	      /* cur is not root; new_node becomes sibling. */
	    எனில் (cur->parent) new_முனை->parent = cur->parent;

	      /* Move the last half of the data from the end of the old node
	       * to the beginning of the new node. At the same time, reduce
	       * the size of the old node. */
	    unsigned idx1 = new_முனை->மொத்தம்;
	    unsigned idx2 = cur->மொத்தம்;
	    போது (idx1 > 0) {
		  idx1 -= 1;
		  idx2 -= 1;
		  new_முனை->child[idx1] = cur->child[idx2];
		  new_முனை->child[idx1]->parent = new_முனை;
		  cur->மொத்தம் -= 1;
	    }

	    ப்ரதிஜ்ஞா(new_முனை->மொத்தம் > 0);
	    ப்ரதிஜ்ஞா(cur->மொத்தம் > 0);

	    எனில் (cur->parent == 0) {
		    /* cur is root. Move first half of children to another
		     * new node, and put the two new nodes in cur. The plan
		     * here is to make cur into the new root and split its
		     * contents into 2 children. */

		  new_முனை->parent = cur;
		  கட்டமைப்பு மர_முனை*new2_முனை = புதிய கட்டமைப்பு மர_முனை;
		  new2_முனை->leaf_flag = பொய்;
		  new2_முனை->மொத்தம் = cur->மொத்தம்;
		  new2_முனை->parent = cur;
		  க்கு (unsigned idx = 0; idx < cur->மொத்தம்; idx += 1) {
			new2_முனை->child[idx] = cur->child[idx];
			new2_முனை->child[idx]->parent = new2_முனை;
		  }
		  cur->child[0] = new2_முனை;
		  cur->child[1] = new_முனை;
		  cur->மொத்தம் = 2;
		    /* no more ancestors, stop the while loop */
		  நிறுத்து;
	    }

	      /* cur is not root. hook new_node to cur->parent. */
	    unsigned idx = 0;
	    போது (cur->parent->child[idx] != cur) {
		  ப்ரதிஜ்ஞா(idx < cur->parent->மொத்தம்);
		  idx += 1;
	    }
	    idx += 1;

	    க்கு (unsigned tmp = cur->parent->மொத்தம் ;  tmp > idx ;  tmp -= 1) {
		  cur->parent->child[tmp] = cur->parent->child[tmp-1];
	    }

	    cur->parent->child[idx] = new_முனை;
	    cur->parent->மொத்தம் += 1;

	      /* check the ancestor */
	    cur = cur->parent;
      }
}

/*
 * This function takes a leaf node and splits in into two. Move half
 * the leaf keys into the new node, and add the new leaf into the
 * parent node.
 */
நிலையான கட்டமைப்பு மர_முனை* split_leaf_(கட்டமைப்பு மர_முனை*cur)
{
      ப்ரதிஜ்ஞா(cur->leaf_flag);
      ப்ரதிஜ்ஞா(cur->parent);
      ப்ரதிஜ்ஞா(! cur->parent->leaf_flag);

	/* Create a new leaf to hold half the data from the old leaf. */
      கட்டமைப்பு மர_முனை*new_leaf = புதிய கட்டமைப்பு மர_முனை;
      new_leaf->leaf_flag = உண்மை;
      new_leaf->மொத்தம் = cur->மொத்தம் / 2;
      new_leaf->parent = cur->parent;

	/* Move the last half of the data from the end of the old leaf
	   to the beginning of the new leaf. At the same time, reduce
	   the size of the old leaf. */
      unsigned idx1 = new_leaf->மொத்தம்;
      unsigned idx2 = cur->மொத்தம்;
      போது (idx1 > 0) {
	    idx1 -= 1;
	    idx2 -= 1;
	    new_leaf->leaf[idx1] = cur->leaf[idx2];
	    cur->மொத்தம் -= 1;
      }

      ப்ரதிஜ்ஞா(new_leaf->மொத்தம் > 0);
      ப்ரதிஜ்ஞா(cur->மொத்தம் > 0);

      unsigned idx = 0;
      போது (cur->parent->child[idx] != cur) {
	    ப்ரதிஜ்ஞா(idx < cur->parent->மொத்தம்);
	    idx += 1;
      }

      idx += 1;

      க்கு (unsigned tmp = cur->parent->மொத்தம் ;  tmp > idx ;  tmp -= 1)
	    cur->parent->child[tmp] = cur->parent->child[tmp-1];

      cur->parent->child[idx] = new_leaf;
      cur->parent->மொத்தம் += 1;

      எனில் (cur->parent->மொத்தம் == முனை_width)
	    split_முனை(cur->parent);

      திரும்பு new_leaf;
}


/*
 * This function searches tree recursively for the key. If the value
 * is not found (and we are at a leaf) then set the key with the given
 * value. If the key is found, set the value only if the force_flag is
 * true.
 */

குறியீடு_மதிப்பு_t குறியீடு_table_s::find_மதிப்பு(கட்டமைப்பு மர_முனை*cur,
					   மாறிலி எழுத்து*சாவி, குறியீடு_மதிப்பு_t val,
					   தருக்க force_flag)
{
      எனில் (cur->leaf_flag) {

	    unsigned idx = 0;
	    க்கு (;;) {
		    /* If we run out of keys in the leaf, then add
		       this at the end of the leaf. */
		  எனில் (idx == cur->மொத்தம்) {
			cur->leaf[idx].சாவி = சாவி_strdup(சாவி);
			cur->leaf[idx].val = val;
			cur->மொத்தம் += 1;
			எனில் (cur->மொத்தம் == leaf_width)
			      split_leaf_(cur);

			திரும்பு val;
		  }

		  முழு rc = strcmp(சாவி, cur->leaf[idx].சாவி);

		    /* If we found the key already in the table, then
		       set the new value and break. */
		  எனில் (rc == 0) {
			எனில் (force_flag)
			      cur->leaf[idx].val = val;
			திரும்பு cur->leaf[idx].val;
		  }

		    /* If this key goes before the current key, then
		       push all the following entries back and add
		       this key here. */
		  எனில் (rc < 0) {
			க்கு (unsigned tmp = cur->மொத்தம்; tmp > idx; tmp -= 1)
			      cur->leaf[tmp] = cur->leaf[tmp-1];

			cur->leaf[idx].சாவி = சாவி_strdup(சாவி);
			cur->leaf[idx].val = val;
			cur->மொத்தம் += 1;
			எனில் (cur->மொத்தம் == leaf_width)
			      split_leaf_(cur);

			திரும்பு val;
		  }

		  idx += 1;
	    }

      } இல்லையெனில் {
	      /* Do a binary search within the inner node. */
	    unsigned min = 0;
	    unsigned max = cur->மொத்தம்;
	    unsigned idx = max/2;
	    க்கு (;;) {
		  முழு rc = strcmp(சாவி, முனை_last_சாவி(cur->child[idx]));
		  எனில் (rc == 0) {
			திரும்பு find_மதிப்பு(cur->child[idx],
					   சாவி, val, force_flag);
		  }

		  எனில் (rc > 0) {
			min = idx + 1;
			எனில் (min == cur->மொத்தம்)
			      திரும்பு find_மதிப்பு(cur->child[idx],
						 சாவி, val, force_flag);
			எனில் (min == max)
			      திரும்பு find_மதிப்பு(cur->child[max],
						 சாவி, val, force_flag);

			idx = min + (max-min)/2;

		  } இல்லையெனில்  {
			max = idx;
			எனில் (idx == min)
			      திரும்பு find_மதிப்பு(cur->child[idx],
						 சாவி, val, force_flag);
			idx = min + (max-min)/2;
		  }
	    }
      }

      ப்ரதிஜ்ஞா(0);
      { குறியீடு_மதிப்பு_t tmp;
        tmp.ptr = 0;
        திரும்பு tmp;
      }
}

வெறுமை குறியீடு_table_s::sym_set_மதிப்பு(மாறிலி எழுத்து*சாவி, குறியீடு_மதிப்பு_t val)
{
      எனில் (வேர்->மொத்தம் == 0) {
	      /* Handle the special case that this is the very first
		 value in the symbol table. Create the first leaf node
		 and initialize the pointers. */
	    கட்டமைப்பு மர_முனை*cur = புதிய கட்டமைப்பு மர_முனை;
	    cur->leaf_flag = உண்மை;
	    cur->parent = வேர்;
	    cur->மொத்தம் = 1;
	    cur->leaf[0].சாவி = சாவி_strdup(சாவி);
	    cur->leaf[0].val = val;

	    வேர்->மொத்தம் = 1;
	    வேர்->child[0] = cur;
      } இல்லையெனில் {
	    find_மதிப்பு(வேர், சாவி, val, உண்மை);
      }
}

குறியீடு_மதிப்பு_t குறியீடு_table_s::sym_get_மதிப்பு(மாறிலி எழுத்து*சாவி)
{
      குறியீடு_மதிப்பு_t def;
      def.ptr = 0;

      எனில் (வேர்->மொத்தம் == 0) {
	      /* Handle the special case that this is the very first
		 value in the symbol table. Create the first leaf node
		 and initialize the pointers. */
	    கட்டமைப்பு மர_முனை*cur = புதிய கட்டமைப்பு மர_முனை;
	    cur->leaf_flag = உண்மை;
	    cur->parent = வேர்;
	    cur->மொத்தம் = 1;
	    cur->leaf[0].சாவி = சாவி_strdup(சாவி);
	    cur->leaf[0].val = def;

	    வேர்->மொத்தம் = 1;
	    வேர்->child[0] = cur;
	    திரும்பு cur->leaf[0].val;
      } இல்லையெனில் {
	    திரும்பு find_மதிப்பு(வேர், சாவி, def, பொய்);
      }
}

குறியீடு_table_s::~குறியீடு_table_s()
{
      delete_குறியீடு_முனை(வேர்);
      போது (str_chunk) {
	    சாவி_strings*tmp = str_chunk;
	    str_chunk = tmp->next;
	    delete tmp;
      }
}
