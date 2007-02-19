/* This is a C++-ified version of the Mersenne Twister pseudo-random number
   generator based on a recode by Shawn Cokus. I didn't touch the original 
   code, the only changes I made were related to the task of wrapping it into
   a C++ class (renaming of functions and variables, changing them to class
   members, etc.).
   Martin Hinsch (mhinsch@usf.uni-osnabrueck.de), April 4, 1999.
   
   5.6.99 - Changed macro names to begin with a '__MTRAND_'.
   
   This library is free software; you can redistribute it and/or modify it
   under the terms of the GNU Library General Public License as published by
   the Free Software Foundation (either version 2 of the License or, at your
   option, any later version).  This library is distributed in the hope that
   it will be useful, but WITHOUT ANY WARRANTY, without even the implied
   warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
   the GNU Library General Public License for more details.  You should have
   received a copy of the GNU Library General Public License along with this
   library; if not, write to the Free Software Foundation, Inc., 59 Temple
   
   Place, Suite 330, Boston, MA 02111-1307, USA.
*/


#include "mtrand.h"

MTRand::uint32 MTRand::lastSeed = 1;

MTRand::uint32 MTRand::State[__MTRAND_N+1];
MTRand::uint32 * MTRand::Next = 0;
int MTRand::Left = -1;

const MTRand::uint32 MTRand::min = 0;
const MTRand::uint32 MTRand::max = ~MTRand::min;
