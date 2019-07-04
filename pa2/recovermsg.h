/****************************************************************************
  FileName     [ recovermsg.h ]
  PackageName  [ pa2 ]
  Author       [ Chen-Hao Hsu ]
  Purpose      [ 2017 Physical Design - Programming Assignment 2 
                 (Fixed-outline Floorplanning) ]
  Date         [ 2017/4/6 created ]
****************************************************************************/

#ifndef RECOVER_MSG_H
#define RECOVER_MSG_H

#include <vector>
#include <cassert>
#include "mydefine.h"
#include "block.h"
#include "btree.h"

using namespace std;

/*
<< RecoverMsg Protocol >>

[0] LEFT_ROTATE
      _blocks[0] = rootblock(after rotation) (i.e. the rotated node's parent after rotation)

[1] RIGHT_ROTATE 
      _blocks[0] = rootblock(after rotation) (i.e. the rotated node's parent after rotation)
      
[2] DELETE_INSERT 
      _blocks[0] = the chosenNullnode 
      _blocks[1] = deleteAndInsert Block
      
[3] SWAP_TWO_BLOCK
      _blocks[0] = block1
      _blocks[1] = block2

[4] ROTATE_BLOCK
      _blocks[0] = rotated block
      
*/

class RecoverMsg
{
public:
   RecoverMsg(){ reset(); }
   ~RecoverMsg() {}

   // Basic access functions
   OptType getType() const { return _type; }
   Block* getBlock(unsigned k) const
   {
      if(k < _blocks.size())
         return _blocks[k];
      else 
         return 0;
   }
   size_t getSize() const { return _blocks.size(); }

   // Basic assign functions
   void setType(OptType p) { _type = p; }
   void pushBlock(Block* b) { _blocks.push_back(b); }

   void reset() 
   { 
      _blocks.resize(0); 
      _type = OPT_TOT; 
      assert(_blocks.empty());
   }

private:
   OptType        _type;
   vector<Block*> _blocks;

};

#endif // RECOVER_MSG_H
