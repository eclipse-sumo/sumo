/***************************************************************************
                         Indexsort.cpp

	Sorts a vector and its indices, based on quicksort algorithm for
	large number of data

			  usage		 : Indexsort(inputvector,indexvector,func_point,size)
			  inputvector: data to sort (int)
			  indexvector: index of data to sort (int)
			  func_point : pointer to function
			  size		 : number of data to sort
                             -------------------
    project              : SUMO		 :
	subproject           : OD2TRIPS
    begin                : Thu, 10 September 2002
	modified			 : Thu, 25 March 2003, from int to long int
    copyright            : (C) 2002 by DLR/IVF http://ivf.dlr.de/
    author               : Peter Mieth, based on quicksort
    email                : Peter.Mieth@dlr.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#define INSERTION_SORT_BOUND 16 /* boundary point to use insertion sort */

#define uint32 long int
//#define uint32 unsigned int

typedef int (*CMPFUN)(long int, long int);



/* explain function
 * Description:
 * indexsort::Qsort() is an internal subroutine that implements quick sort.
 *
 * Return Value: none
 */
void Qsort(long int *This, long int *iax, CMPFUN fun_ptr, uint32 first, uint32 last)
{
  uint32 stack_pointer = 0;
  long int first_stack[64];
  long int last_stack[64];
  long int cur_i,temp_i;

  for (;;)
  {
    if (last - first <= INSERTION_SORT_BOUND)
    {
      /* for small sort, use insertion sort */
      uint32 indx;
      long int prev_val = *(This+first);
      long int prev_i   = *(iax+first);
      long int cur_val,hilf;

      for (indx = first + 1; indx <= last; ++indx)
      {
        cur_val = *(This+indx);
        cur_i=*(iax+indx);
       if ((*fun_ptr)(prev_val, cur_val) > 0)
        {
          /* out of order: array[indx-1] > array[indx] */
          uint32 indx2;
          *(This+indx) = prev_val;
          *(iax+indx) = prev_i;
          /* move up the larger item first */
          /* find the insertion point for the smaller item */
          for (indx2 = indx - 1; indx2 > first; )
          {
            long int temp_val = *(This+indx2 - 1);
            temp_i = *(iax+indx2-1);
            if ((*fun_ptr)(temp_val, cur_val) > 0)
            {
             hilf = indx2;
             *(This+indx2--) = temp_val;
             *(iax+hilf--) = temp_i;
            }
            else
              break;
          }
          *(This+indx2) = cur_val; /* insert the smaller item right here */
          *(iax+indx2) = cur_i;
        }
        else
        {
          /* in order, advance to next element */
			prev_val = cur_val;
			prev_i=cur_i;
        }
      }
    }
    else
    {
      long int pivot;

      /* try quick sort */
      {
        long int temp;
        uint32 med = (first + last) >> 1;
        /* Choose pivot from first, last, and median position. */
        /* Sort the three elements. */
        temp = *(This+first);
        temp_i = *(iax+first);
        if ((*fun_ptr)(temp, *(This+last)) > 0)
        {
			*(This+first) = *(This+last); *(This+last) = temp;
			*(iax+first) = *(iax+last); *(iax+last)=temp_i;
        }
        temp = *(This+med);
        temp_i = *(iax+med);
        if ((*fun_ptr)(*(This+first), temp) > 0)
        {
			*(This+med) = *(This+first); *(This+first) = temp;
			*(iax+med) = *(iax+first); *(iax+first)=temp_i;
        }
        temp = *(This+last);
        temp_i = *(iax+last);
        if ((*fun_ptr)(*(This+med), temp) > 0)
        {
			*(This+last) = *(This+med); *(This+med) = temp;
			*(iax+last) = *(iax+med); *(iax+med)=temp_i;
        }
        pivot = *(This+med);
      }
      {
        uint32 up;
        {
	  uint32 down;
      /* First and last element will be loop stopper. */
	  /* Split array into two partitions. */
	  down = first;
	  up = last;
	  for (;;)
	  {
	    do
	    {
	      ++down;
	    } while ((*fun_ptr)(pivot, *(This+down)) > 0);

	    do
	    {
	      --up;
	    } while ((*fun_ptr)(*(This+up), pivot) > 0);

	    if (up > down)
	    {
	      long int temp;
	      /* interchange L[down] and L[up] */
	      temp = *(This+down);
		  *(This+down)= *(This+up);
		  *(This+up) = temp;
              temp_i=*(iax+down); *(iax+down)=*(iax+up);*(iax+up)=temp_i;
	    }
	    else
	      break;
	  }
	}
	{
	  uint32 len1; /* length of first segment */
	  uint32 len2; /* length of second segment */
	  len1 = up - first + 1;
	  len2 = last - up;
	  /* stack the partition that is larger */
	  if (len1 >= len2)
	  {
	    first_stack[stack_pointer] = first;
	    last_stack[stack_pointer++] = up;

	    first = up + 1;
	    /*  tail recursion elimination of
	     *  Qsort(This,fun_ptr,up + 1,last)
	     */
	  }
	  else
	  {
	    first_stack[stack_pointer] = up + 1;
	    last_stack[stack_pointer++] = last;

	    last = up;
	    /* tail recursion elimination of
	     * Qsort(This,fun_ptr,first,up)
	     */
	  }
	}
        continue;
      }
      /* end of quick sort */
    }
    if (stack_pointer > 0)
    {
      /* Sort segment from stack. */
      first = first_stack[--stack_pointer];
      last = last_stack[stack_pointer];
    }
    else
      break;
  } /* end for */
}



void IndexSort(long int *This, long int *iax, CMPFUN fun_ptr, uint32 the_len)
{
	Qsort(This, iax, fun_ptr, 0, the_len - 1);
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "ODIndexSort.icc"
//#endif

// Local Variables:
// mode:C++
// End:


