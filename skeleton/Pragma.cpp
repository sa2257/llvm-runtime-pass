#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Constants.h"
using namespace llvm;


namespace {
  struct PragmaPass : public FunctionPass {
    static char ID;
    PragmaPass() : FunctionPass(ID) {}

    virtual bool runOnFunction(Function &F) {
        bool modified = false;
    
		errs() << "Go into function " << F.getName() << "!\n";
        /* Add conditional to preventive recursive replacement */
        if (F.getName() == "foo") {
            int i = 0;
            Value* args[5];
            for(auto arg = F.arg_begin(); arg != F.arg_end(); ++arg) {
                    args[i] = arg;
                    i++;
            }

		    LLVMContext& Ctx = F.getContext();
		    FunctionCallee logFunc = F.getParent()->getOrInsertFunction(
		    //    "log_val", Type::getVoidTy(Ctx), Type::getDoubleTy(Ctx)
		        "matmul_accel", Type::getVoidTy(Ctx),
                Type::getDoublePtrTy(Ctx), Type::getDoublePtrTy(Ctx), Type::getDoublePtrTy(Ctx),
                Type::getInt32Ty(Ctx), Type::getInt32Ty(Ctx)
		    );
            Constant *i32_size1 = ConstantInt::get(Type::getInt32Ty(Ctx), 64);
            Constant *i32_size2 = ConstantInt::get(Type::getInt32Ty(Ctx), 64);
            args[3] = i32_size1;
            args[4] = i32_size2;

		    for (auto &B: F) {
		        for (auto &I: B) {
		            //errs() << "an instruction: " << I << " \n";
                    if(auto* callInst = dyn_cast<CallInst>(&I)) {
                        Function* fn = callInst->getCalledFunction();
                        if(fn->getName() == "llvm.var.annotation") {
                            errs() << "Found pragma " << fn->getName() << "!\n";
                            IRBuilder<> builder(&I);
                            builder.SetInsertPoint(&B, ++builder.GetInsertPoint());
                            
                            errs() << "Insert a call to our function!\n";

                            //Constant *double_in = ConstantFP::get(Type::getDoubleTy(Ctx), 5.0);
                            //Value* args[] = {double_in};
                            builder.CreateCall(logFunc, args);
                            
                            modified = true;
                        }
                    }
		        }
		    }
        }

      	return modified;
    }
  };
}

char PragmaPass::ID = 0;

// Register the pass so `opt -pragma` runs it.
static RegisterPass<PragmaPass> X("pragma", "a useless pass");
