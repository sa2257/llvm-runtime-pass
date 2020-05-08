// Initially taken from llvm-pass-skeleton/skeleton/Skeleton.cpp at https://github.com/sampsyo/llvm-pass-skeleton.git and later modified by Sachille Atapattu

/* Leech: Extends the rtlib in Skeleton to add a runtime function in place of an existing operation */

#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/IRBuilder.h"
using namespace llvm;


namespace {
  struct LeechPass : public FunctionPass {
    static char ID;
    LeechPass() : FunctionPass(ID) {}

    virtual bool runOnFunction(Function &F) {
// Get the function to call from our runtime library.
		LLVMContext& Ctx = F.getContext();
		FunctionCallee rtFunc = F.getParent()->getOrInsertFunction(
		  "rtlib", Type::getInt32Ty(Ctx),
          Type::getInt32Ty(Ctx), Type::getInt32Ty(Ctx),
          Type::getInt32Ty(Ctx), Type::getInt32Ty(Ctx)
		);
        /* create the function call */
		
		for (auto& B : F) {
		  for (auto& I : B) {
		    if (auto* op = dyn_cast<BinaryOperator>(&I)) {
		      errs() << "Insert function call after " << op->getOpcodeName() << "!\n";
		      IRBuilder<> builder(op);
		      builder.SetInsertPoint(&B, ++builder.GetInsertPoint());
              /* find the place to enter the simulator call */
		
		      errs() << "Insert a call to our function!\n";
              Constant *i32_ticks = ConstantInt::get(Type::getInt32Ty(Ctx), 5, true);
              Constant *i32_size = ConstantInt::get(Type::getInt32Ty(Ctx), 2, true);
		      Value* args[] = {op->getOperand(0), op->getOperand(1), i32_ticks, i32_size};
		      Value* ret =  builder.CreateCall(rtFunc, args);
		
		      for (auto& U : op->uses()) {
		          User* user = U.getUser();  // A User is anything with operands.
		          user->setOperand(U.getOperandNo(), ret);
		      }
		      
              return true;
		    }
		  }
		}

      	return false;
    }
  };
}

char LeechPass::ID = 0;

// Register the pass so `opt -leech` runs it.
static RegisterPass<LeechPass> X("leech", "a pass to insert a runtime insertion of a python call");
