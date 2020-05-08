// Initially taken from llvm_profiling/skeleton/Skeleton.cpp at https://github.com/neiladit/llvm_profiling.git and later modified by Sachille Atapattu

/*Shackleton: This profiles most of the instructions and reports the raw numbers of occurence*/

#include "llvm/Pass.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Support/raw_ostream.h"

#include "llvm/Analysis/CallGraph.h"
#include "llvm/Analysis/CallGraphSCCPass.h"

#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include <string.h>
#include <vector>

#include "llvm/Support/CommandLine.h"

using namespace std;
using namespace llvm;

static cl::opt<string> InputModule("function", cl::desc("Specify function to run pass"), cl::value_desc("module"));
static cl::opt<bool> SwitchOn("select", cl::desc("Select to run pass"));

namespace {
  struct ShackletonPass : public ModulePass {
      static char ID;
      ShackletonPass() : ModulePass(ID) {}
      
      virtual bool runOnModule(Module &M); //when there is a Module
      virtual bool runOnFunction(Function &F, Module &M); //called by runOnModule
      virtual bool runOnBasicBlock(BasicBlock &BB, Module &M); // called by runOnFunction
      
      bool initialize(Module &M); //create global variable
      bool finialize(Module &M); //print global variable
      void createInstr(BasicBlock &bb, Constant *counter_ptr, int num, bool loc);
      
      vector<string> atomicCounter = {  "instructionCounter", "basicBlockCounter", 
                                        "branchCounter", "switchCounter",
                                        "addCounter", "subCounter", "mulCounter", "divCounter", "remCounter",
                                        "faddCounter", "fsubCounter", "fmulCounter", "fdivCounter", "fremCounter",
                                        "fnegCounter", "andCounter", "orCounter", "xorCounter",
                                        "shiftCounter", "intCompCounter", "fpCompCounter", "phiCounter",
                                        "intConvCounter", "fpConvCounter", "typeConvCounter", "otherConvCounter",
                                        "loadCounter", "storeCounter", 
                                        "otherCount"
                                     }; //keep global variable names for profiling. e.g. instr counter
  };
}

bool ShackletonPass::runOnModule(Module &M)
{
    bool modified = initialize(M);
    
    errs() << "Pass run status: " << SwitchOn << "\n";
    errs() << "Pass running on: " << InputModule << "\n";
    for (auto func = M.begin(); func != M.end(); func++) {
        modified |= runOnFunction(*func, M);
    }
    modified |= finialize(M);
    //M.dump();
    
    return modified;
}

void ShackletonPass::createInstr(BasicBlock &bb, Constant *counter_ptr, int num, bool loc){
    Instruction *before  = loc ? bb.getTerminator() : bb.getFirstNonPHI();
    if(num){ // create atomic addition instruction
        new AtomicRMWInst(AtomicRMWInst::Add,
                      counter_ptr, // pointer to global variable
                      ConstantInt::get(Type::getInt64Ty(bb.getContext()), num), //create integer with value num
                      AtomicOrdering::SequentiallyConsistent, //operations may not be reordered
                      SyncScope::System, // synchronize to all threads
                      before); //insert right before block terminator
    }
}

bool ShackletonPass::runOnFunction(Function &F, Module &M)
{
    bool modified = false;
    //errs() << F.getName() << "\n";
    string funcName = F.getName();

    for (auto bb = F.begin(); bb != F.end(); bb++) {
        if (SwitchOn || funcName == InputModule) {
            modified |= runOnBasicBlock(*bb, M);
        }
    }

    return modified;
}

bool ShackletonPass::runOnBasicBlock(BasicBlock &bb, Module &M)
{
    // Get the global variable for atomic counter
    Type *I64Ty = Type::getInt64Ty(M.getContext());
    Constant *instrCounter[atomicCounter.size()];
    for (int i = 0; i < atomicCounter.size(); i++){
        instrCounter[i] = M.getOrInsertGlobal(atomicCounter[i], I64Ty);
        char message[1024];
        sprintf(message, "Could not declare or find one global var");
        //string message = string("Could not declare or find ") + atomicCounter[i] + " global";
        assert(instrCounter[i] && message);
    }
    
    // get instruction number and basic block number.
    int instr = 0;
    int basic_block = 1;
    int add_instr = 0;
    int sub_instr = 0;
    int mul_instr = 0;
    int div_instr = 0;
    int rem_instr = 0;
    int fadd_instr = 0;
    int fsub_instr = 0;
    int fmul_instr = 0;
    int fdiv_instr = 0;
    int frem_instr = 0;
    int fneg_instr = 0;
    int and_instr = 0;
    int or_instr = 0;
    int xor_instr = 0;
    int br_instr = 0;
    int sw_instr = 0;
    int str_instr = 0;
    int ld_instr = 0;
    int icmp_instr = 0;
    int fcmp_instr = 0;
    int phi_instr = 0;
    int shift_instr = 0;
    int iconv_instr = 0;
    int fconv_instr = 0;
    int tconv_instr = 0;
    int oconv_instr = 0;
    int other = 0;
    
    instr += bb.size(); // this includes atomicrmws introduced by this pass
    for (auto it = bb.begin(); it != bb.end(); it++) {
        switch (it->getOpcode()) {
            case Instruction::Ret: // return
                continue;
            case Instruction::Br: // branch
                br_instr++;
                continue;
            case Instruction::Switch: // switch
                sw_instr++;
                continue;
            case Instruction::FNeg: // fp negation
                fneg_instr++;
                continue;
            case Instruction::Add: // addition
                add_instr++;
                continue;
            case Instruction::FAdd: // fp addition
                fadd_instr++;
                continue;
            case Instruction::Sub: // subtraction
                sub_instr++;
                continue;
            case Instruction::FSub: // fp subtraction
                fsub_instr++;
                continue;
            case Instruction::Mul: // multiplication
                mul_instr++;
                continue;
            case Instruction::FMul: // fp multiplication
                fmul_instr++;
                continue;
            case Instruction::UDiv: // division unsigned
            case Instruction::SDiv: // division signed
                div_instr++;
                continue;
            case Instruction::FDiv: // fp division
                fdiv_instr++;
                continue;
            case Instruction::URem: // remainder unsigned
            case Instruction::SRem: // remainder signed
                rem_instr++;
                continue;
            case Instruction::FRem: // fp remainder
                frem_instr++;
                continue;
            case Instruction::And: // and
                and_instr++;
                continue;
            case Instruction::Or: // or
                or_instr++;
                continue;
            case Instruction::Xor: // xor
                xor_instr++;
                continue;
            case Instruction::Alloca: // allocate stack memory
            case Instruction::GetElementPtr: // address of subelement
                continue;
            case Instruction::Load: // load
                ld_instr++;
                continue;
            case Instruction::Store: // store
                str_instr++;
                continue;
            case Instruction::Trunc: // truncate
                iconv_instr++;
                continue;
            case Instruction::ZExt: // zero-extend
                iconv_instr++;
                continue;
            case Instruction::SExt: // sign-extend
                iconv_instr++;
                continue;
            case Instruction::FPTrunc: // fp truncate
                fconv_instr++;
                continue;
            case Instruction::FPExt: // fp extend
                fconv_instr++;
                continue;
            case Instruction::FPToUI: // fp to unsigned int
                tconv_instr++;
                continue;
            case Instruction::FPToSI: // fp to signed int
                tconv_instr++;
                continue;
            case Instruction::UIToFP: // unsigned int to fp
                tconv_instr++;
                continue;
            case Instruction::SIToFP: // signed int to fp
                tconv_instr++;
                continue;
            case Instruction::IntToPtr: // int to pointer
                oconv_instr++;
                continue;
            case Instruction::PtrToInt: // pointer to int
                oconv_instr++;
                continue;
            case Instruction::BitCast: // bitcast
                oconv_instr++;
                continue;
            case Instruction::ICmp: // int compare
                icmp_instr++;
                continue;
            case Instruction::FCmp: // fp compare
                fcmp_instr++;
                continue;
            case Instruction::PHI: // phi nodes
                phi_instr++;
                continue;
            case Instruction::Shl: // shift left
                shift_instr++;
                continue;
            case Instruction::LShr: // logic shift right
                shift_instr++;
                continue;
            case Instruction::AShr: // arith shift right
                shift_instr++;
                continue;
            case Instruction::Call: // function call
                continue;
            default:
                errs() << it->getOpcodeName() << "\n";
                other++;
                break;
        }
    }
    
    // create atomic addition instruction
    createInstr(bb, instrCounter[0] , instr      , true);
    createInstr(bb, instrCounter[1] , basic_block, true);
    createInstr(bb, instrCounter[2] , br_instr   , true);
    createInstr(bb, instrCounter[3] , sw_instr   , true);
    createInstr(bb, instrCounter[4] , add_instr  , true);
    createInstr(bb, instrCounter[5] , sub_instr  , true);
    createInstr(bb, instrCounter[6] , mul_instr  , true);
    createInstr(bb, instrCounter[7] , div_instr  , true);
    createInstr(bb, instrCounter[8] , rem_instr  , true);
    createInstr(bb, instrCounter[9] , fadd_instr , true);
    createInstr(bb, instrCounter[10], fsub_instr , true);
    createInstr(bb, instrCounter[11], fmul_instr , true);
    createInstr(bb, instrCounter[12], fdiv_instr , true);
    createInstr(bb, instrCounter[13], frem_instr , true);
    createInstr(bb, instrCounter[14], fneg_instr , true);
    createInstr(bb, instrCounter[15], and_instr  , true);
    createInstr(bb, instrCounter[16], or_instr   , true);
    createInstr(bb, instrCounter[17], xor_instr  , true);
    createInstr(bb, instrCounter[18], shift_instr, true);
    createInstr(bb, instrCounter[19], icmp_instr , true);
    createInstr(bb, instrCounter[20], fcmp_instr , true);
    createInstr(bb, instrCounter[21], phi_instr  , true);
    createInstr(bb, instrCounter[22], iconv_instr, true);
    createInstr(bb, instrCounter[23], fconv_instr, true);
    createInstr(bb, instrCounter[24], tconv_instr, true);
    createInstr(bb, instrCounter[25], oconv_instr, true);
    createInstr(bb, instrCounter[26], ld_instr   , true);
    createInstr(bb, instrCounter[27], str_instr  , true);
    createInstr(bb, instrCounter[28], other      , true);
    
    return true;
}

bool ShackletonPass::initialize(Module &M)
{
    IRBuilder<> Builder(M.getContext());
    Function *mainFunc = M.getFunction("main");
        
    // not the main module
    if (!mainFunc)
        return false;
    
    Type *I64Ty = Type::getInt64Ty(M.getContext());
    // Create atomic counter global variable;
    
    for (int i = 0; i < atomicCounter.size(); i++){
        new GlobalVariable(M, I64Ty, false, GlobalValue::CommonLinkage, ConstantInt::get(I64Ty, 0), atomicCounter[i]);
    }
    
    auto &functionList = M.getFunctionList();
    for (auto &function : functionList) {
        Value *atomic_counter = new GlobalVariable(M, I64Ty, false, GlobalValue::CommonLinkage, ConstantInt::get(I64Ty, 0), function.getName()+".glob");
    }
    
    return true;
}

bool ShackletonPass::finialize(Module &M){
    
    IRBuilder<> Builder(M.getContext());
    Function *mainFunc = M.getFunction("main");
    
    // not the main module
    if (!mainFunc)
        return false;
    
    // Build printf function handle
    std::vector<Type *> FTyArgs;
    FTyArgs.push_back(Type::getInt8PtrTy(M.getContext())); // specify the first argument, i8* is the return type of CreateGlobalStringPtr
    FunctionType *FTy = FunctionType::get(Type::getInt32Ty(M.getContext()), FTyArgs, true); 
    // create function type with return type, argument types and if const argument
    FunctionCallee printF = M.getOrInsertFunction("printf", FTy); // create function if not extern or defined
    
    //assert(printF != NULL);
    
    for (auto bb = mainFunc->begin(); bb != mainFunc->end(); bb++) {
        for(auto it = bb->begin(); it != bb->end(); it++) {
            
            // insert at the end of main function
            if ((std::string)it->getOpcodeName() == "ret") {
                
                // insert printf at the end of main function, before return function
                Builder.SetInsertPoint(&*bb, it);
                Value *format_empty;
                format_empty = Builder.CreateGlobalStringPtr("\n", "formatEmpty"); // create empty string
                std::vector<Value *> argVec;
                argVec.push_back(format_empty);
                CallInst::Create(printF, argVec, "printf", &*it); //create printf function for a new line
                
                for(int i = 0; i < atomicCounter.size(); i++) {
                    
                    // Build Arguments
                    Value *format_long;
                    //format_long = Builder.CreateGlobalStringPtr(atomicCounter[i] + ", %ld\n", "formatLong"); 
                    format_long = Builder.CreateGlobalStringPtr("%ld,", "formatLong"); 
                    // create global string variable formatLong, add suffix(.1/.2/...) if already exists
                    Value* atomic_counter = M.getGlobalVariable(atomicCounter[i]); // get pointer pointing to the global variable name
                    Value* finalVal = new LoadInst(atomic_counter, atomic_counter->getName()+".val", &*it); 
                    // atomic_counter only points to a string, but we want to print the number the string stores
                    
                    std::vector<Value *> argVec;
                    argVec.push_back(format_long);
                    argVec.push_back(finalVal);
                    CallInst::Create(printF, argVec, "printf", &*it); 
                    //create printf function with the return value name called printf (with suffix if already exists)
                    
                }
                
                argVec.push_back(format_empty);
                CallInst::Create(printF, argVec, "printf", &*it); //create printf function for a new line
                
            }
        }
    }
    return true;
    
}

char ShackletonPass::ID = 0;

// Register the pass so `opt -shackleton` runs it.
static RegisterPass<ShackletonPass> X("shackleton", "a profiling pass");
