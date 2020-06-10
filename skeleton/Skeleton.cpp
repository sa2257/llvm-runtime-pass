#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/IRBuilder.h"
using namespace llvm;


namespace {
  struct SkeletonPass : public FunctionPass {
    static char ID;
    SkeletonPass() : FunctionPass(ID) {}

    virtual bool runOnFunction(Function &F) {
// Changes at instruction level
		errs() << "I saw a function called " << F.getName() << "!\n";
		for (auto &B: F) {
		    errs() << "a block\n";
		    for (auto &I: B) {
		        errs() << "an instruction: " << I << " \n";
		        BinaryOperator *bop = dyn_cast<BinaryOperator>(&I);
		        if (bop) {
		            errs() << "above is a binary operator!\n";
		            Value* lhs = bop->getOperand(0);
		            Value* rhs = bop->getOperand(1);
		            errs() << *lhs << "\n";
		            errs() << *rhs << "\n";
		
		            IRBuilder<> builder(bop);
		            Value* mul = builder.CreateMul(lhs, rhs);
		
		            for (auto& U : bop->uses()) {
		                User* user = U.getUser();  // A User is anything with operands.
		                user->setOperand(U.getOperandNo(), mul);
		            }
		        }
		    }
		}

// Get the function to call from our runtime library.
		LLVMContext& Ctx = F.getContext();
		FunctionCallee logFunc = F.getParent()->getOrInsertFunction(
		  "rtlib", Type::getVoidTy(Ctx), Type::getInt32Ty(Ctx)
		);
		
		for (auto& B : F) {
		  for (auto& I : B) {
		    if (auto* op = dyn_cast<BinaryOperator>(&I)) {
		      errs() << "Insert function call after " << op->getOpcode() << "!\n";
		      IRBuilder<> builder(op);
		      builder.SetInsertPoint(&B, ++builder.GetInsertPoint());
		
		      errs() << "Insert a call to our function!\n";
		      Value* args[] = {op};
		      builder.CreateCall(logFunc, args);
		
		      return true;
		    }
		  }
		}

      	return false;
    }
  };
}

char SkeletonPass::ID = 0;

// Register the pass so `opt -skeleton` runs it.
static RegisterPass<SkeletonPass> X("skeleton", "a useless pass");
