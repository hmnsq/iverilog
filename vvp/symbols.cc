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
నిర్మాణం కీలకం_అక్షరశ్రేణులు {
      నిర్మాణం కీలకం_అక్షరశ్రేణులు*తదుపరి_2;
      అక్షరం దత్తాంశం[64*1024 - పరిమాణం(నిర్మాణం కీలకం_అక్షరశ్రేణులు*)];
};

అక్షరం*చిహ్నం_పట్టిక_s::కీలకం_strdup(స్థిరరాశి అక్షరం*str)
{
      గుర్తులేని len = అక్షరశ్రేణి_పొడవు(str);
      ప్రతిజ్ఞా( (len+1) <= పరిమాణం str_chunk->దత్తాంశం );

      ఒకవేళ ( (len+1) > (పరిమాణం str_chunk->దత్తాంశం - str_వాడిన) ) {
	    కీలకం_అక్షరశ్రేణులు*tmp = కొత్త కీలకం_అక్షరశ్రేణులు;
	    tmp->తదుపరి_2 = str_chunk;
	    str_chunk = tmp;
	    str_వాడిన = 0;
      }

      అక్షరం*res = str_chunk->దత్తాంశం + str_వాడిన;
      str_వాడిన += len + 1;
      strcpy(res, str);
      తిరిగి res;
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

స్థిరరాశి గుర్తులేని ఆకు_వెడల్పు = 254;
స్థిరరాశి గుర్తులేని కణుపు_వెడల్పు = 508;

నిర్మాణం వృక్షం_దత్తాంశం {
      అక్షరం*కీలకం;
      చిహ్నం_విలువ_t val;
};

నిర్మాణం వృక్ష_కణుపు {
      తార్కిక ఆకు_flag;
      గుర్తులేని లెక్క;
      నిర్మాణం వృక్ష_కణుపు*మాతృ;

      సంఘం {
	    నిర్మాణం వృక్షం_దత్తాంశం ఆకు[ఆకు_వెడల్పు];
	    నిర్మాణం వృక్ష_కణుపు *బిడ్డ[కణుపు_వెడల్పు];
      };

};

స్థిర అంతఃవరుస అక్షరం* కణుపు_చివరి_కీలకం(నిర్మాణం వృక్ష_కణుపు*కణుపు)
{
      అయితే (కణుపు->ఆకు_flag == అబద్ధం)
	    కణుపు = కణుపు->బిడ్డ[కణుపు->లెక్క-1];

      తిరిగి కణుపు->ఆకు[కణుపు->లెక్క-1].కీలకం;
}

/*
 * Allocate a new symbol table means creating the table structure and
 * a root node, and initializing the pointers and members of the root
 * node.
 */
చిహ్నం_పట్టిక_s::చిహ్నం_పట్టిక_s()
{
      మూలం = కొత్త నిర్మాణం వృక్ష_కణుపు;
      మూలం->ఆకు_flag = అబద్ధం;
      మూలం->లెక్క = 0;
      మూలం->మాతృ = 0;

      str_chunk = కొత్త కీలకం_అక్షరశ్రేణులు;
      str_chunk->తదుపరి_2 = 0;
      str_వాడిన = 0;
}

స్థిర శూన్య delete_చిహ్నం_కణుపు(నిర్మాణం వృక్ష_కణుపు*cur)
{
      ఒకవేళ (! cur->ఆకు_flag) {
	    కొరకు (గుర్తులేని idx = 0 ;  idx < cur->లెక్క ;  idx += 1)
		  delete_చిహ్నం_కణుపు(cur->బిడ్డ[idx]);
      }

      తొలగించు cur;
}

/* Do as split_leaf_ does, but for nodes. */
స్థిర శూన్య విభజించు_కణుపు(నిర్మాణం వృక్ష_కణుపు*cur)
{
      ప్రతిజ్ఞా(!cur->ఆకు_flag);
      ఒకవేళ (cur->మాతృ) ప్రతిజ్ఞా(! cur->మాతృ->ఆకు_flag);

      అయితే (cur->లెక్క == కణుపు_వెడల్పు) {
	      /* Create a new node to hold half the data from cur. */
	    నిర్మాణం వృక్ష_కణుపు *new_కణుపు = కొత్త నిర్మాణం వృక్ష_కణుపు;
	    new_కణుపు->ఆకు_flag = అబద్ధం;
	    new_కణుపు->లెక్క = cur->లెక్క / 2;
	      /* cur is not root; new_node becomes sibling. */
	    ఒకవేళ (cur->మాతృ) new_కణుపు->మాతృ = cur->మాతృ;

	      /* Move the last half of the data from the end of the old node
	       * to the beginning of the new node. At the same time, reduce
	       * the size of the old node. */
	    గుర్తులేని idx1 = new_కణుపు->లెక్క;
	    గుర్తులేని idx2 = cur->లెక్క;
	    అయితే (idx1 > 0) {
		  idx1 -= 1;
		  idx2 -= 1;
		  new_కణుపు->బిడ్డ[idx1] = cur->బిడ్డ[idx2];
		  new_కణుపు->బిడ్డ[idx1]->మాతృ = new_కణుపు;
		  cur->లెక్క -= 1;
	    }

	    ప్రతిజ్ఞా(new_కణుపు->లెక్క > 0);
	    ప్రతిజ్ఞా(cur->లెక్క > 0);

	    ఒకవేళ (cur->మాతృ == 0) {
		    /* cur is root. Move first half of children to another
		     * new node, and put the two new nodes in cur. The plan
		     * here is to make cur into the new root and split its
		     * contents into 2 children. */

		  new_కణుపు->మాతృ = cur;
		  నిర్మాణం వృక్ష_కణుపు*new2_కణుపు = కొత్త నిర్మాణం వృక్ష_కణుపు;
		  new2_కణుపు->ఆకు_flag = అబద్ధం;
		  new2_కణుపు->లెక్క = cur->లెక్క;
		  new2_కణుపు->మాతృ = cur;
		  కొరకు (గుర్తులేని idx = 0; idx < cur->లెక్క; idx += 1) {
			new2_కణుపు->బిడ్డ[idx] = cur->బిడ్డ[idx];
			new2_కణుపు->బిడ్డ[idx]->మాతృ = new2_కణుపు;
		  }
		  cur->బిడ్డ[0] = new2_కణుపు;
		  cur->బిడ్డ[1] = new_కణుపు;
		  cur->లెక్క = 2;
		    /* no more ancestors, stop the while loop */
		  ఆపు;
	    }

	      /* cur is not root. hook new_node to cur->parent. */
	    గుర్తులేని idx = 0;
	    అయితే (cur->మాతృ->బిడ్డ[idx] != cur) {
		  ప్రతిజ్ఞా(idx < cur->మాతృ->లెక్క);
		  idx += 1;
	    }
	    idx += 1;

	    కొరకు (గుర్తులేని tmp = cur->మాతృ->లెక్క ;  tmp > idx ;  tmp -= 1) {
		  cur->మాతృ->బిడ్డ[tmp] = cur->మాతృ->బిడ్డ[tmp-1];
	    }

	    cur->మాతృ->బిడ్డ[idx] = new_కణుపు;
	    cur->మాతృ->లెక్క += 1;

	      /* check the ancestor */
	    cur = cur->మాతృ;
      }
}

/*
 * This function takes a leaf node and splits in into two. Move half
 * the leaf keys into the new node, and add the new leaf into the
 * parent node.
 */
స్థిర నిర్మాణం వృక్ష_కణుపు* విభజించు_ఆకు(నిర్మాణం వృక్ష_కణుపు*cur)
{
      ప్రతిజ్ఞా(cur->ఆకు_flag);
      ప్రతిజ్ఞా(cur->మాతృ);
      ప్రతిజ్ఞా(! cur->మాతృ->ఆకు_flag);

	/* Create a new leaf to hold half the data from the old leaf. */
      నిర్మాణం వృక్ష_కణుపు*new_ఆకు = కొత్త నిర్మాణం వృక్ష_కణుపు;
      new_ఆకు->ఆకు_flag = నిజం;
      new_ఆకు->లెక్క = cur->లెక్క / 2;
      new_ఆకు->మాతృ = cur->మాతృ;

	/* Move the last half of the data from the end of the old leaf
	   to the beginning of the new leaf. At the same time, reduce
	   the size of the old leaf. */
      గుర్తులేని idx1 = new_ఆకు->లెక్క;
      గుర్తులేని idx2 = cur->లెక్క;
      అయితే (idx1 > 0) {
	    idx1 -= 1;
	    idx2 -= 1;
	    new_ఆకు->ఆకు[idx1] = cur->ఆకు[idx2];
	    cur->లెక్క -= 1;
      }

      ప్రతిజ్ఞా(new_ఆకు->లెక్క > 0);
      ప్రతిజ్ఞా(cur->లెక్క > 0);

      గుర్తులేని idx = 0;
      అయితే (cur->మాతృ->బిడ్డ[idx] != cur) {
	    ప్రతిజ్ఞా(idx < cur->మాతృ->లెక్క);
	    idx += 1;
      }

      idx += 1;

      కొరకు (గుర్తులేని tmp = cur->మాతృ->లెక్క ;  tmp > idx ;  tmp -= 1)
	    cur->మాతృ->బిడ్డ[tmp] = cur->మాతృ->బిడ్డ[tmp-1];

      cur->మాతృ->బిడ్డ[idx] = new_ఆకు;
      cur->మాతృ->లెక్క += 1;

      ఒకవేళ (cur->మాతృ->లెక్క == కణుపు_వెడల్పు)
	    విభజించు_కణుపు(cur->మాతృ);

      తిరిగి new_ఆకు;
}


/*
 * This function searches tree recursively for the key. If the value
 * is not found (and we are at a leaf) then set the key with the given
 * value. If the key is found, set the value only if the force_flag is
 * true.
 */

చిహ్నం_విలువ_t చిహ్నం_పట్టిక_s::కనుగొను_విలువ(నిర్మాణం వృక్ష_కణుపు*cur,
					   స్థిరరాశి అక్షరం*కీలకం, చిహ్నం_విలువ_t val,
					   తార్కిక force_flag)
{
      ఒకవేళ (cur->ఆకు_flag) {

	    గుర్తులేని idx = 0;
	    కొరకు (;;) {
		    /* If we run out of keys in the leaf, then add
		       this at the end of the leaf. */
		  ఒకవేళ (idx == cur->లెక్క) {
			cur->ఆకు[idx].కీలకం = కీలకం_strdup(కీలకం);
			cur->ఆకు[idx].val = val;
			cur->లెక్క += 1;
			ఒకవేళ (cur->లెక్క == ఆకు_వెడల్పు)
			      విభజించు_ఆకు(cur);

			తిరిగి val;
		  }

		  పూర్ణాంకం rc = strcmp(కీలకం, cur->ఆకు[idx].కీలకం);

		    /* If we found the key already in the table, then
		       set the new value and break. */
		  ఒకవేళ (rc == 0) {
			ఒకవేళ (force_flag)
			      cur->ఆకు[idx].val = val;
			తిరిగి cur->ఆకు[idx].val;
		  }

		    /* If this key goes before the current key, then
		       push all the following entries back and add
		       this key here. */
		  ఒకవేళ (rc < 0) {
			కొరకు (గుర్తులేని tmp = cur->లెక్క; tmp > idx; tmp -= 1)
			      cur->ఆకు[tmp] = cur->ఆకు[tmp-1];

			cur->ఆకు[idx].కీలకం = కీలకం_strdup(కీలకం);
			cur->ఆకు[idx].val = val;
			cur->లెక్క += 1;
			ఒకవేళ (cur->లెక్క == ఆకు_వెడల్పు)
			      విభజించు_ఆకు(cur);

			తిరిగి val;
		  }

		  idx += 1;
	    }

      } లేకపోతే {
	      /* Do a binary search within the inner node. */
	    గుర్తులేని min = 0;
	    గుర్తులేని max = cur->లెక్క;
	    గుర్తులేని idx = max/2;
	    కొరకు (;;) {
		  పూర్ణాంకం rc = strcmp(కీలకం, కణుపు_చివరి_కీలకం(cur->బిడ్డ[idx]));
		  ఒకవేళ (rc == 0) {
			తిరిగి కనుగొను_విలువ(cur->బిడ్డ[idx],
					   కీలకం, val, force_flag);
		  }

		  ఒకవేళ (rc > 0) {
			min = idx + 1;
			ఒకవేళ (min == cur->లెక్క)
			      తిరిగి కనుగొను_విలువ(cur->బిడ్డ[idx],
						 కీలకం, val, force_flag);
			ఒకవేళ (min == max)
			      తిరిగి కనుగొను_విలువ(cur->బిడ్డ[max],
						 కీలకం, val, force_flag);

			idx = min + (max-min)/2;

		  } లేకపోతే  {
			max = idx;
			ఒకవేళ (idx == min)
			      తిరిగి కనుగొను_విలువ(cur->బిడ్డ[idx],
						 కీలకం, val, force_flag);
			idx = min + (max-min)/2;
		  }
	    }
      }

      ప్రతిజ్ఞా(0);
      { చిహ్నం_విలువ_t tmp;
        tmp.ptr = 0;
        తిరిగి tmp;
      }
}

శూన్య చిహ్నం_పట్టిక_s::sym_సమితి_విలువ(స్థిరరాశి అక్షరం*కీలకం, చిహ్నం_విలువ_t val)
{
      ఒకవేళ (మూలం->లెక్క == 0) {
	      /* Handle the special case that this is the very first
		 value in the symbol table. Create the first leaf node
		 and initialize the pointers. */
	    నిర్మాణం వృక్ష_కణుపు*cur = కొత్త నిర్మాణం వృక్ష_కణుపు;
	    cur->ఆకు_flag = నిజం;
	    cur->మాతృ = మూలం;
	    cur->లెక్క = 1;
	    cur->ఆకు[0].కీలకం = కీలకం_strdup(కీలకం);
	    cur->ఆకు[0].val = val;

	    మూలం->లెక్క = 1;
	    మూలం->బిడ్డ[0] = cur;
      } లేకపోతే {
	    కనుగొను_విలువ(మూలం, కీలకం, val, నిజం);
      }
}

చిహ్నం_విలువ_t చిహ్నం_పట్టిక_s::sym_get_విలువ(స్థిరరాశి అక్షరం*కీలకం)
{
      చిహ్నం_విలువ_t def;
      def.ptr = 0;

      ఒకవేళ (మూలం->లెక్క == 0) {
	      /* Handle the special case that this is the very first
		 value in the symbol table. Create the first leaf node
		 and initialize the pointers. */
	    నిర్మాణం వృక్ష_కణుపు*cur = కొత్త నిర్మాణం వృక్ష_కణుపు;
	    cur->ఆకు_flag = నిజం;
	    cur->మాతృ = మూలం;
	    cur->లెక్క = 1;
	    cur->ఆకు[0].కీలకం = కీలకం_strdup(కీలకం);
	    cur->ఆకు[0].val = def;

	    మూలం->లెక్క = 1;
	    మూలం->బిడ్డ[0] = cur;
	    తిరిగి cur->ఆకు[0].val;
      } లేకపోతే {
	    తిరిగి కనుగొను_విలువ(మూలం, కీలకం, def, అబద్ధం);
      }
}

చిహ్నం_పట్టిక_s::~చిహ్నం_పట్టిక_s()
{
      delete_చిహ్నం_కణుపు(మూలం);
      అయితే (str_chunk) {
	    కీలకం_అక్షరశ్రేణులు*tmp = str_chunk;
	    str_chunk = tmp->తదుపరి_2;
	    తొలగించు tmp;
      }
}
