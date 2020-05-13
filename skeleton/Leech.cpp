// Initially taken from llvm_profiling/skeleton/Skeleton.cpp at https://github.com/neiladit/llvm_profiling.git and later modified by Sachille Atapattu

/* Leech: This selects certain instructions and replaces them with a runtime function call */

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

static cl::opt<bool> SwitchOn("select", cl::desc("Select to run pass"));

namespace {
  struct LeechPass : public ModulePass {
      static char ID;
      LeechPass() : ModulePass(ID) {}
      
      virtual bool runOnModule(Module &M); //when there is a Module
      virtual bool runtimeForInstruction(Function &F); //called by runOnModule
      virtual bool runtimeForBasicBlock(Function &F); //called by runOnModule
      virtual bool runtimeForFunction(Function &F); //called by runOnModule
  };
}

bool LeechPass::runOnModule(Module &M)
{
    bool modified = false;
    
    for (auto& F : M) {
        if (SwitchOn)
            //modified |= runtimeForInstruction(F);
            modified |= runtimeForBasicBlock(F);
            //modified |= runtimeForFunction(F);
    }
    
    return modified;
}

bool LeechPass::runtimeForFunction(Function &F) {
    bool modified = false;
    
    /* Add conditional to preventive recursive replacement */
    if (F.getName() == "ssadd") {
        /* Remove existing basic blocks */
        BasicBlock* workList = NULL; // To collect replaced instructions within iterator
	    
	    for (auto& B : F) {
            workList = &B;
        }

        if (workList != NULL)
            workList->eraseFromParent();
        
        int i = 0;
        Value* args[3];
        for(auto arg = F.arg_begin(); arg != F.arg_end(); ++arg) {
                args[i] = arg;
                i++;
        }
        
        /* Create new basic block */ 
	    LLVMContext& Ctx = F.getContext();
        BasicBlock *pb = BasicBlock::Create(Ctx, "simple", &F);

        // Create function call
        IRBuilder<> builder(pb);

        errs() << "Insert runtime function in " << F.getName() << "\n";
	    FunctionCallee rtFunc = F.getParent()->getOrInsertFunction(
	      "rtlib_double", Type::getDoubleTy(Ctx),
          Type::getDoublePtrTy(Ctx), Type::getDoublePtrTy(Ctx), Type::getInt32Ty(Ctx)
	    );
        // function call name and argument types have to be known
        
        Constant *i32_select = ConstantInt::get(Type::getInt32Ty(Ctx), 2, true);
        args[2] = i32_select;
	    Value* ret =  builder.CreateCall(rtFunc, args);
  	    
        // Create return
        Instruction *retInst = ReturnInst::Create(Ctx, ret, pb);
        modified = true;
    }

    return modified;
}

bool LeechPass::runtimeForBasicBlock(Function &F) {
    bool modified = false;
    
    // if function not selected, llvm goes crazy
    if (F.getName() == "vvadd") {
        /* Select a basic block */
        BasicBlock* workList = NULL; // To collect replaced instructions within iterator
	    for (auto& B : F) {
	      for (auto& I : B) {
	        if (auto* op = dyn_cast<BinaryOperator>(&I)) {
                if(op->getOpcode() == Instruction::FAdd)
                    workList = &B;
            }
          }
        }
        
        BasicBlock* predBB = workList->getSinglePredecessor();
        BasicBlock* succBB = workList->getSingleSuccessor();

	    if (workList != NULL) {
            workList->eraseFromParent();
        }
        
        /* Create new basic block */ 
	    LLVMContext& Ctx = F.getContext();
        //BasicBlock *pb = BasicBlock::Create(Ctx, "simple", &F, succBB);
        BasicBlock *pb = BasicBlock::Create(Ctx, "simple", &F);

        // Create function call
        IRBuilder<> builder(pb);

        errs() << "Insert runtime function in " << F.getName() << "\n";
	    FunctionCallee rtFunc = F.getParent()->getOrInsertFunction(
	      "rtlib_int", Type::getInt32Ty(Ctx),
          Type::getInt32Ty(Ctx), Type::getInt32Ty(Ctx), Type::getInt32Ty(Ctx)
	    );
        FunctionCallee logFunc = F.getParent()->getOrInsertFunction(
                  "log_val", Type::getVoidTy(Ctx), Type::getInt32Ty(Ctx)
                );
        // function call name and argument types have to be known
        
        Constant *i32_in1 = ConstantInt::get(Type::getInt32Ty(Ctx), 5, true);
        Constant *i32_in2 = ConstantInt::get(Type::getInt32Ty(Ctx), 6, true);
        Constant *i32_select = ConstantInt::get(Type::getInt32Ty(Ctx), 2, true);
        Value* args[] = {i32_in1, i32_in2, i32_select};
	    Value* ret =  builder.CreateCall(rtFunc, args);

        Value* argsl[] = {ret};
        builder.CreateCall(logFunc, argsl);
            
        // Create a jump
        Instruction *brInst = BranchInst::Create(succBB, pb);

        // Update entry
        Instruction *brPred = predBB->getTerminator();
        brPred->setSuccessor(0, pb);

        modified = true;
    }
	
  	return modified;
}

bool LeechPass::runtimeForInstruction(Function &F) {
    bool modified = false;
    Instruction* workList = NULL; // To collect replaced instructions within iterator
	
	for (auto& B : F) {
	  for (auto& I : B) {
          /* Add runtime function selection here */
	    if (auto* op = dyn_cast<BinaryOperator>(&I)) {
	      errs() << "Insert function call after " << op->getOpcodeName() << "!\n";
          int func = 0;
          switch (op->getOpcode()) {
              //case Instruction::FNeg: // fp negation
              //    func = 3; break;
              case Instruction::Add: // addition
                  func = 2; break;
              case Instruction::FAdd: // fp addition
                  func = 5; break;
              case Instruction::Sub: // subtraction
                  func = 2; break;
              case Instruction::FSub: // fp subtraction
                  func = 5; break;
              case Instruction::Mul: // multiplication
                  func = 2; break;
              case Instruction::FMul: // fp multiplication
                  func = 5; break;
              case Instruction::UDiv: // division unsigned
              case Instruction::SDiv: // division signed
                  func = 2; break;
              case Instruction::FDiv: // fp division
                  func = 5; break;
              case Instruction::URem: // remainder unsigned
              case Instruction::SRem: // remainder signed
                  func = 2; break;
              case Instruction::FRem: // fp remainder
                  func = 5; break;
              case Instruction::And: // and
                  func = 1; break;
              case Instruction::Or: // or
                  func = 1; break;
              case Instruction::Xor: // xor
                  func = 1; break;
              default:
                  break;
          }
	      /* End of selection algorithm */
          /* find the place to enter the runtime call */
          IRBuilder<> builder(op);
	      builder.SetInsertPoint(&B, ++builder.GetInsertPoint());
	
          /* create the function call from runtime library */
	      LLVMContext& Ctx = F.getContext();
	      FunctionCallee rtFunc;
          if (func < 4) {
	          rtFunc = F.getParent()->getOrInsertFunction(
	              "rtlib_int", Type::getInt32Ty(Ctx),
                  Type::getInt32Ty(Ctx), Type::getInt32Ty(Ctx), Type::getInt32Ty(Ctx)
	              );
          } else {
	          rtFunc = F.getParent()->getOrInsertFunction(
	              "rtlib_double", Type::getDoubleTy(Ctx),
                  Type::getDoubleTy(Ctx), Type::getDoubleTy(Ctx), Type::getInt32Ty(Ctx)
	              );
          }
          // function call name and argument types have to be known
    
          /* insert runtime call */
          if(func > 3) {
	      errs() << "Insert a call to our function!\n";
          Constant *i32_select = ConstantInt::get(Type::getInt32Ty(Ctx), func, true);
	      Value* args[] = {op->getOperand(0), op->getOperand(1), i32_select};
	      Value* ret =  builder.CreateCall(rtFunc, args);
	
          /* forward return to users */
	      for (auto& U : op->uses()) {
	          User* user = U.getUser();
	          user->setOperand(U.getOperandNo(), ret);
	      }

          workList = &I;

          modified = true;

          }
	      
	    }
	  }
	}

	if (workList != NULL)
        workList->eraseFromParent();

  	return modified;
}


char LeechPass::ID = 0;

// Register the pass so `opt -leech` runs it.
static RegisterPass<LeechPass> X("leech", "a profiling pass");
