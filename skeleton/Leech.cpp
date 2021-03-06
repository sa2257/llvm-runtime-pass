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
        virtual bool runtimeForFunction(Function &F);    // called by runOnModule
        virtual bool runtimeForBasicBlock(Function &F);  // called by runOnModule
        virtual bool runtimeForSubBlock(Function &F);    // called by runOnModule
        virtual bool runtimeForInstruction(Function &F); // called by runOnModule
        virtual bool checkNotInList(list<Instruction*> List, Value *V);
        virtual bool checkInstrIsFP(Instruction &I);
        virtual int  findInstrOp(Instruction &I);
        virtual bool checkForChains(Value *V);
    };
}

bool LeechPass::runOnModule(Module &M)
{
    bool modified = false;
    
    for (auto& F : M) {
        if (SwitchOn)
            modified |= runtimeForInstruction(F);
            //modified |= runtimeForSubBlock(F);
            //modified |= runtimeForBasicBlock(F);
            //modified |= runtimeForFunction(F);
    }
    
    return modified;
}

bool LeechPass::runtimeForFunction(Function &F) {
    bool modified = false;
    
    /* Add conditional to preventive recursive replacement */
    if (F.getName() == "kernel") {
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
    if (F.getName() == "kernel") {
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
        /* Done selecting a basic block */
        
        // Enter and exit to the basic block- single for now
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

        /* Enter instructions to the new basic block */
        errs() << "Insert runtime basic block as " << pb->getName() << "\n";
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
        /* Done entering new instructions */
            
        // Create a jump
        Instruction *brInst = BranchInst::Create(succBB, pb);

        // Update entry
        Instruction *brPred = predBB->getTerminator();
        brPred->setSuccessor(0, pb);

        modified = true;
    }
	
  	return modified;
}

bool LeechPass::runtimeForSubBlock(Function &F) {
    bool modified = false;
    LLVMContext& Ctx = F.getContext();
    FunctionCallee pyFunc = F.getParent()->getOrInsertFunction(
        "chain_replace", Type::getDoubleTy(Ctx),
        Type::getDoubleTy(Ctx), Type::getDoubleTy(Ctx),
        Type::getDoubleTy(Ctx), Type::getDoubleTy(Ctx)
	);
    
    // if function not selected, llvm goes crazy
    if (F.getName() == "kernel") {
		for (auto &B: F) {
            list <Instruction*> Selected;
        /* Select a basic block */
		    for (auto &I: B) {
		        if (auto* op = dyn_cast<BinaryOperator>(&I)) {
		            if (checkInstrIsFP(I)) {
                        bool select = false;
		                Value* lhs = op->getOperand(0);
                        select = select | checkForChains(lhs);
		                Value* rhs = op->getOperand(1);
                        select = select | checkForChains(rhs);
		                for (auto& U : op->uses()) {
		                    Value* user = U.getUser();
                            select = select | checkForChains(user);
		                }
                        if (select) {
                            Selected.push_back(&I);
		                    errs() << op->getOpcodeName() << " added to the list!\n";
                        }
                    }
		        }
		    }
        /* Done selecting a basic block */

            Value* args[4];
        /* Enter instructions to the new basic block */
            int idx = 0, i = 0;
            for (auto &I: Selected) {
                i++;
		        Value* lhs = I->getOperand(0);
                if (checkNotInList(Selected, lhs)) {
                    args[idx] = lhs;
                    idx++;
                }

		        Value* rhs = I->getOperand(1);
                if (checkNotInList(Selected, rhs)) {
                    args[idx] = rhs;
                    idx++;
                }
                if (i == Selected.size()) {
                    IRBuilder<> builder(I);
                    builder.SetInsertPoint(&B, ++builder.GetInsertPoint());
	                Value* ret =  builder.CreateCall(pyFunc, args);
	
                    /* forward return to users */
	                for (auto& U : I->uses()) {
	                    User* user = U.getUser();
	                    user->setOperand(U.getOperandNo(), ret);
	                }
                }
            }
        /* Done entering new instructions */
            
        /* Remove replaced instructions from the basic block */
            while (!Selected.empty()) {
                Selected.front()->eraseFromParent();
                Selected.pop_front();
            }
        /* Done removing replaced instructions */

            // Chain assumes to consume all the uses, a check is not done yet
            // This doesn't support multiple chains in the same BB
        }
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
                int func = findInstrOp(I);
                /* End of selection algorithm */
                /* find the place to enter the runtime call */
                IRBuilder<> builder(op);
                builder.SetInsertPoint(&B, ++builder.GetInsertPoint());
                
                /* create the function call from runtime library */
                LLVMContext& Ctx = F.getContext();
                FunctionCallee rtFunc;
                if (func > 6) {
                    rtFunc = F.getParent()->getOrInsertFunction(
                        "ins_replace", Type::getDoubleTy(Ctx),
                        Type::getDoubleTy(Ctx), Type::getDoubleTy(Ctx), Type::getInt32Ty(Ctx)
                        );
                }
                // function call name and argument types have to be known
                
                /* insert runtime call */
                if(func > 6) {
	                errs() << "Insert function call after " << op->getOpcodeName() << "!\n";
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

bool LeechPass::checkNotInList(list<Instruction*> List, Value *V) {
    if(isa<Instruction>(V)) {
        Instruction *I = cast<Instruction>(V);
        for (auto L: List) {
            if (L == I) {
                return false;
            }
        }
    }
    return true;
}

bool LeechPass::checkInstrIsFP(Instruction &I) {
    if (I.getOpcode() == Instruction::FAdd ||
        I.getOpcode() == Instruction::FSub ||
        I.getOpcode() == Instruction::FMul ||
        I.getOpcode() == Instruction::FDiv ||
        I.getOpcode() == Instruction::FRem ) {
        return true;
    }
    return false;
}

int LeechPass::findInstrOp(Instruction &I) {
    int func = 0;
    switch (I.getOpcode()) {
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
    return func;
}

bool LeechPass::checkForChains(Value *V) {
    if(isa<Instruction>(V)) {
        Instruction *I = cast<Instruction>(V);
        if (checkInstrIsFP(*I)) {
            return true;
        }
    }
    return false;
}

char LeechPass::ID = 0;

// Register the pass so `opt -leech` runs it.
static RegisterPass<LeechPass> X("leech", "a profiling pass");
