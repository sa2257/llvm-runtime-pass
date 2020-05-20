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
//#include <vector>
#include <queue>
#include <list>

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
      virtual bool checkInstrNotInList(list<Instruction*> List, Instruction &I);
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
    if (F.getName() == "vvadd") {
        /* Remove existing basic blocks */
        queue <BasicBlock*> workList; // To collect replaced instructions within iterator
	    
	    for (auto& B : F) {
            for (auto& I : B) {
                Instruction* ins = &I;
                ins->dropAllReferences();
            }
            workList.push(&B);
        }
        
        while (!workList.empty()) {
            workList.front()->eraseFromParent();
            workList.pop();
        }
        
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
        FunctionCallee pyFunc = F.getParent()->getOrInsertFunction(
            "func_replace", Type::getVoidTy(Ctx),
            Type::getDoublePtrTy(Ctx), Type::getDoublePtrTy(Ctx), Type::getDoublePtrTy(Ctx)
	    );
        // function call name and argument types have to be known
        
	    builder.CreateCall(pyFunc, args);
  	    
        // Create return
        Instruction *retInst = ReturnInst::Create(Ctx, pb);
        modified = true;
    }

    return modified;
}

bool LeechPass::runtimeForBasicBlock(Function &F) {
    bool modified = false;
    
    // if function not selected, llvm goes crazy
    if (F.getName() == "diamond") {
        /* Select a basic block */
        BasicBlock* workList = NULL; // To collect replaced instructions within iterator
        list <Instruction*> Used;
        bool select = false;
	    for (auto& B : F) {
	        for (auto& I : B) {
	            if (auto* op = dyn_cast<BinaryOperator>(&I)) {
                    if (op->getOpcode() == Instruction::FAdd ||
                        op->getOpcode() == Instruction::FSub ||
                        op->getOpcode() == Instruction::FMul ||
                        op->getOpcode() == Instruction::FDiv ||
                        op->getOpcode() == Instruction::FRem ) {
                        if (!select) {
                            workList = &B;
                            select = true;
                        }
                        if (checkInstrNotInList(Used, I)) {// currently handling one group of ins per block
                            Used.push_back(&I);
                        }
                    }
                }
            }
        }
        /* Done selecting a basic block */
        
        // Enter and exit to the basic block- single for now
        BasicBlock* predBB = workList->getSinglePredecessor();
        BasicBlock* succBB = workList->getSingleSuccessor();

	    if (workList != NULL) {
            for (auto& I : Used ) {
                for (operands)
                    if (outside)
                        Inputs.push_back(operand);
                    else
                        Link.push_back(p-c);
                        remove memory ops
                for (uses)
                    if (outside)
                        Outputs.push_back(use);
                    else
                        check in links
                I->eraseFromParent();
            }
        }

        // Take each ins, create arg from operands if they are not used in others in list.
        // Replace all uses with rets if not used in list
        // Remove ins produced and consumed by list
        
        /* Create new basic block */ 
	    LLVMContext& Ctx = F.getContext();

        // Create function call
        IRBuilder<> builder(workList);

        /* Enter instructions to the new basic block */
        errs() << "Insert runtime function to " << workList << "\n";
	    FunctionCallee rtFunc = F.getParent()->getOrInsertFunction(
	        "bb_replace", Type::getInt32Ty(Ctx),
            Type::getInt32Ty(Ctx), Type::getInt32Ty(Ctx), Type::getInt32Ty(Ctx)
	    );
        // function call name and argument types have to be known
        
        Constant *i32_in1 = ConstantInt::get(Type::getInt32Ty(Ctx), 5, true);
        Constant *i32_in2 = ConstantInt::get(Type::getInt32Ty(Ctx), 6, true);
        Constant *i32_select = ConstantInt::get(Type::getInt32Ty(Ctx), 2, true);
	    Value* args[] = {firstIns->getOperand(0), firstIns->getOperand(1), i32_select};
	    Value* ret =  builder.CreateCall(rtFunc, args);
        /* Done entering new instructions */
            
	    for (auto& U : lastIns->uses()) {
	        User* user = U.getUser();
	        user->setOperand(U.getOperandNo(), ret);
	    }

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
              //    func = 7; break;
              case Instruction::Add: // addition
                  func = 2; break;
              case Instruction::FAdd: // fp addition
                  func = 8; break;
              case Instruction::Sub: // subtraction
                  func = 3; break;
              case Instruction::FSub: // fp subtraction
                  func = 9; break;
              case Instruction::Mul: // multiplication
                  func = 4; break;
              case Instruction::FMul: // fp multiplication
                  func = 10; break;
              case Instruction::UDiv: // division unsigned
              case Instruction::SDiv: // division signed
                  func = 5; break;
              case Instruction::FDiv: // fp division
                  func = 11; break;
              case Instruction::URem: // remainder unsigned
              case Instruction::SRem: // remainder signed
                  func = 6; break;
              case Instruction::FRem: // fp remainder
                  func = 12; break;
              case Instruction::And: // and
                  func = 13; break;
              case Instruction::Or: // or
                  func = 14; break;
              case Instruction::Xor: // xor
                  func = 15; break;
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
          if (func < 6) {
	          rtFunc = F.getParent()->getOrInsertFunction(
	              "rtlib_int", Type::getInt32Ty(Ctx),
                  Type::getInt32Ty(Ctx), Type::getInt32Ty(Ctx), Type::getInt32Ty(Ctx)
	              );
          } else {
	          rtFunc = F.getParent()->getOrInsertFunction(
	              "rtlib_sim", Type::getDoubleTy(Ctx),
                  Type::getDoubleTy(Ctx), Type::getDoubleTy(Ctx), Type::getInt32Ty(Ctx)
	              );
          }
          // function call name and argument types have to be known
    
          /* insert runtime call */
          if(func > 6) {
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

bool LeechPass::checkInstrNotInList(list<Instruction*> List, Instruction &I) {
    for (auto L: List) {
        if (L == &I) {
            return false;
        }
    }
    return true;
}

char LeechPass::ID = 0;

// Register the pass so `opt -leech` runs it.
static RegisterPass<LeechPass> X("leech", "a profiling pass");
