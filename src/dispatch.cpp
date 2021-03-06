#include <map>
#include <vector>
#include <cassert>
#include <iostream>
#include <exception>

#include "llvm/Type.h"
#include "llvm/Module.h"
#include "llvm/Function.h"
#include "llvm/Constants.h"
#include "llvm/Instructions.h"
#include "llvm/Support/CFG.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/InstIterator.h"
#include "llvm/InstrTypes.h"
#include "llvm/ValueSymbolTable.h"

#include "llvm/Constants.h"
#include "llvm/DerivedTypes.h"
#include "llvm/InlineAsm.h"
#include "llvm/DerivedTypes.h"
#include "llvm/GlobalValue.h"

#include "dispatch.h"

// MakeDispatcherPass::MakeDispatcherPass()
// {
//   FunctionPass(ID);
// }
char MakeDispatcherPass::ID = 0;
const std::string MakeDispatcherPass::PassName = "MakeDispatcherPass";

bool MakeDispatcherPass::runOnFunction( Function& currFunction )
{
  BasicBlock* entryBB = &currFunction.getEntryBlock();
  bool madeChange = true;


  std::cout << ":Processing " << currFunction.getName().str() << std::endl;

  ConvertCmp(currFunction);

  return (true);
}

void ShowType(CmpInst* inst)
{
  switch(inst->getPredicate())
    {
    case CmpInst::ICMP_EQ:
      std::cout << "Equal" << std::endl;
      break;
    case CmpInst::ICMP_NE:
      std::cout << "Not Equal" << std::endl;
      break;
    case CmpInst::ICMP_UGT:
      std::cout << "Unsigned Greater Than" << std::endl;
      break;
    case CmpInst::ICMP_UGE:
      std::cout << "Unsigned Greater Or Equal" << std::endl;
      break;
    case CmpInst::ICMP_ULT:
      std::cout << "Unsigned Less Than" << std::endl;
      break;
    case CmpInst::ICMP_ULE:
      std::cout << "unsigned less or equal" << std::endl;
      break;
    case CmpInst::ICMP_SGT:
      std::cout << "signed greater than" << std::endl;
      break;
    case CmpInst::ICMP_SGE:
      std::cout << "signed greater or equal" << std::endl;
      break;
    case CmpInst::ICMP_SLT:
      std::cout << "signed less than" << std::endl;
      break;
    case CmpInst::ICMP_SLE:
      std::cout << "signed less or equal" << std::endl;
      break;
    default:
      std::cout << "default cmp" << std::endl;
    }
}

extern LLVMContext &Context;

void MakeDispatcherPass::CreateInt3(BasicBlock* block, Instruction* I)
{
  FunctionType *asm_Ftype = FunctionType::get(Type::getVoidTy(Context),
					      ArrayRef<Type*>(), false);
  InlineAsm* Iasm =
  InlineAsm::get(asm_Ftype,"int3","~{dirflag},~{fpsr},~{flags}", true);

  Function *NewF = Function::Create(asm_Ftype, Function::ExternalLinkage);

  // Instruction* newI = new Instruction(Iasm);
  // block->getInstList().push_back(dynamic_cast<Instruction*>(Iasm));
  // block.insert(Iasm);
  // ReplaceInstWithValue(block->getInstList(), block, Iasm);
  // block->getInstList().insert(I, Iasm);
}

void MakeDispatcherPass::ConvertCmp(Function& function)
{
  typedef std::vector< Instruction * > InstList;
  InstList insts;

  for (Function::iterator BB = function.begin(), bbE = function.end(); BB != bbE; ++BB)
    {
      for (BasicBlock::iterator I = BB->begin(), E = BB->end(); I != E;)
	{
	  if (isa< CmpInst >(I))
	    {
	      insts.push_back(I);
	    }
	  if (isa< BranchInst >(I))
	    {
	      BasicBlock::iterator save = I;

	      BranchInst* branchInst = dynamic_cast< BranchInst *>(&*I);
	      if (branchInst->isConditional() && !insts.empty())
		{
		  Value*	valbranch = NULL;

		  valbranch = branchInst->getCondition();
		  ShowType(dynamic_cast<CmpInst*>(insts[0]));
		  CreateInt3(BB, I);
		  I++;
		  save->eraseFromParent();
		  insts.pop_back();
		  continue;
		}
	    }
	  I++;
	}
    }
}

bool MakeDispatcherPass::IsUsedOutsideParentBlock( Instruction* value )
{
  for( Value::use_iterator i = value->use_begin(); i != value->use_end();
       i++ )
    {
      Instruction* user = dynamic_cast< Instruction* >( *i );
      if( user->getParent() != value->getParent() )
        {
	  return true;
        }
    }
  return false;
}


void MakeDispatcherPass::ConvertSwitch( Function& function )
{
  BasicBlock* entryBB = &function.getEntryBlock();

  std::cout << ":Processing " << function.getName().str() << std::endl;

  for( Function::iterator i = function.begin(); i != function.end(); i++ )
    {
      BasicBlock* basicBlock = &*i;
      Instruction* inst;

      TerminatorInst* terminator = basicBlock->getTerminator();
      assert( terminator && "Basic block is not well formed and has no terminator!" );

      if( isa< BranchInst >( terminator ) )
        {
	  // std::cout << "Branch Instruction !!!" << std::endl;
	  BranchInst* branchInst = dynamic_cast< BranchInst *>
	    (
	     basicBlock->getTerminator()
	     );
	  std::cout << "Branch Instruction, Opcode = " <<
	    branchInst->getOpcodeName();
	  if (branchInst->isConditional())
	    {
	      inst = dynamic_cast<Instruction *>(basicBlock->getTerminator());
	      std::cout << ", Is Conditional ";
	      // i->eraseFromParent();
	    }
	  else
	    {
	      std::cout << ", Is not conditionnal ";
	    }

	  // if (valbranch) // && !valbranch->getName().empty())
	  //   {
	  //     valbranch->getType()->dump();
	  //     std::cout << valbranch->getName().str();
	  //   }
	  // branchInst->eraseFromParent();


	  std::cout << std::endl;

	  // std::cout << branchInst->getCondition()->getName.str() << std::endl;
        }
    }
}
