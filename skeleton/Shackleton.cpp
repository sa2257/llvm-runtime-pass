// Initially taken from llvm_profiling/skeleton/Skeleton.cpp at https://github.com/neiladit/llvm_profiling.git and later modified by Sachille Atapattu

/*Shackleton: This profiles most of the instructions and reports the raw numbers of occurence*/

#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Instructions.h"
//#include "llvm/IR/InstrTypes.h"

#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"

#include "llvm/Analysis/CallGraph.h"
#include "llvm/Analysis/CallGraphSCCPass.h"

#include <string.h>
#include <vector>

#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/CommandLine.h"

using namespace std;
using namespace llvm;

static cl::opt<string> InputModule("function", cl::desc("Specify function to run pass"), cl::value_desc("module"));

namespace {
  struct ShackletonPass : public ModulePass {
      static char ID;
      ShackletonPass() : ModulePass(ID) {}
      
      virtual bool runOnModule(Module &M); //when there is a Module
      virtual bool runtimeOnFunction(Function &F); //called by runOnModule
  };
}

bool ShackletonPass::runOnModule(Module &M)
{
    bool modified = false;
    
    errs() << "Pass running on: " << InputModule << "\n";
    for (auto& func : M) {
        modified |= runtimeOnFunction(func);
    }
    
    return modified;
}

bool ShackletonPass::runtimeOnFunction(Function &F) {
    /* create the function call from runtime library */
	LLVMContext& Ctx = F.getContext();
	FunctionCallee rtFunc = F.getParent()->getOrInsertFunction(
	  "rtlib", Type::getInt32Ty(Ctx),
      Type::getInt32Ty(Ctx), Type::getInt32Ty(Ctx), Type::getInt32Ty(Ctx)
	);
    int func = 0;
    // function call name and argument types have to be known
	
	for (auto& B : F) {
	  for (auto& I : B) {
	    if (auto* op = dyn_cast<BinaryOperator>(&I)) {
	      errs() << "Insert function call after " << op->getOpcodeName() << "!\n";
          switch (op->getOpcode()) {
              //case Instruction::FNeg: // fp negation
              //    func = 3; continue;
              case Instruction::Add: // addition
                  func = 2; continue;
              case Instruction::FAdd: // fp addition
                  func = 5; continue;
              case Instruction::Sub: // subtraction
                  func = 2; continue;
              case Instruction::FSub: // fp subtraction
                  func = 5; continue;
              case Instruction::Mul: // multiplication
                  func = 2; continue;
              case Instruction::FMul: // fp multiplication
                  func = 5; continue;
              case Instruction::UDiv: // division unsigned
              case Instruction::SDiv: // division signed
                  func = 2; continue;
              case Instruction::FDiv: // fp division
                  func = 5; continue;
              case Instruction::URem: // remainder unsigned
              case Instruction::SRem: // remainder signed
                  func = 2; continue;
              case Instruction::FRem: // fp remainder
                  func = 5; continue;
              case Instruction::And: // and
                  func = 1; continue;
              case Instruction::Or: // or
                  func = 1; continue;
              case Instruction::Xor: // xor
                  func = 1; continue;
              //case Instruction::Load: // load
              //    func = 5; continue;
              //case Instruction::Store: // store
              //    func = 5; continue;
              //case Instruction::Trunc: // truncate
              //    func = 5; continue;
              //case Instruction::ZExt: // zero-extend
              //    func = 5; continue;
              //case Instruction::SExt: // sign-extend
              //    func = 5; continue;
              //case Instruction::FPTrunc: // fp truncate
              //    func = 5; continue;
              //case Instruction::FPExt: // fp extend
              //    func = 5; continue;
              //case Instruction::Shl: // shift left
              //    func = 5; continue;
              //case Instruction::LShr: // logic shift right
              //    func = 5; continue;
              //case Instruction::AShr: // arith shift right
              //    func = 5; continue;
              default:
                  continue;
          }
	      
          /* find the place to enter the runtime call */
          IRBuilder<> builder(op);
	      builder.SetInsertPoint(&B, ++builder.GetInsertPoint());
	
          /* insert runtime call */
	      errs() << "Insert a call to our function!\n";
          Constant *i32_select = ConstantInt::get(Type::getInt32Ty(Ctx), func, true);
	      Value* args[] = {op->getOperand(0), op->getOperand(1), i32_select};
	      Value* ret =  builder.CreateCall(rtFunc, args);
	
          /* forward return to users */
	      for (auto& U : op->uses()) {
	          User* user = U.getUser();
	          user->setOperand(U.getOperandNo(), ret);
	      }
	      
          return true;
	    }
	  }
	}

  	return false;
}


char ShackletonPass::ID = 0;

// Register the pass so `opt -shackleton` runs it.
static RegisterPass<ShackletonPass> X("shackleton", "a profiling pass");
