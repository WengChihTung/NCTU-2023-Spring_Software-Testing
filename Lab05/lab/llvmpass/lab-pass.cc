/*
  Ref:
  * https://llvm.org/doxygen/
  * https://llvm.org/docs/GettingStarted.html
  * https://llvm.org/docs/WritingAnLLVMPass.html
  * https://llvm.org/docs/ProgrammersManual.html
 */
#include "lab-pass.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Constants.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/Error.h"
#include "llvm/TableGen/DirectiveEmitter.h"

using namespace llvm;

char LabPass::ID = 0;

bool LabPass::doInitialization(Module &M) {
  return true;
}
static void dumpIR(Function &F)
{
  for (auto &BB : F) {
    errs() << "BB: " << "\n";
    errs() << BB << "\n";
  }
}

static Constant* getI8StrVal(Module &M, char const *str, Twine const &name) {
  LLVMContext &ctx = M.getContext();

  Constant *strConstant = ConstantDataArray::getString(ctx, str);

  GlobalVariable *gvStr = new GlobalVariable(M, strConstant->getType(), true,
    GlobalValue::InternalLinkage, strConstant, name);

  Constant *zero = Constant::getNullValue(IntegerType::getInt32Ty(ctx));
  Constant *indices[] = { zero, zero };
  Constant *strVal = ConstantExpr::getGetElementPtr(Type::getInt8PtrTy(ctx),
    gvStr, indices, true);

  return strVal;
}

static FunctionCallee printfPrototype(Module &M) {
  LLVMContext &ctx = M.getContext();

  FunctionType *printfType = FunctionType::get(
    Type::getInt32Ty(ctx),
    { Type::getInt8PtrTy(ctx) },
    true);

  FunctionCallee printfCallee = M.getOrInsertFunction("printf", printfType);

  return printfCallee;
}



bool LabPass::runOnModule(Module &M) {
  errs() << "runOnModule\n";

  LLVMContext &llvm_context = M.getContext();
  Type *get_int_ty = Type::getInt32Ty(llvm_context);

  GlobalVariable *depth_var = M.getGlobalVariable("depthVar");
  if (!depth_var) {
      depth_var = new GlobalVariable(M, get_int_ty, false, GlobalValue::LinkageTypes::CommonLinkage, nullptr, "depthVar");
      depth_var->setInitializer(ConstantInt::get(get_int_ty, 0));
  }
  errs() << *depth_var << "\n";

  FunctionCallee the_callee = printfPrototype(M);

  for (auto &F : M) {
    if (F.empty()) {
      continue;
    }
    
    errs() << F.getName() << "\n";

    Constant *space = getI8StrVal(M, " ", "space");
    Constant *one = Constant::getIntegerValue(Type::getInt32Ty(llvm_context), APInt(32, 1));
    
    BasicBlock &first_block = F.front();
    BasicBlock &last_block = F.back();
    
    Instruction &last_ins = *(++last_block.rend());
    BasicBlock *new_bb = last_block.splitBasicBlock(&last_ins, "last_ins");

    BasicBlock *depth_block = BasicBlock::Create(llvm_context, "depth", &F, &first_block);
    BasicBlock *space_block = BasicBlock::Create(llvm_context, "space", &F, &first_block);
    BasicBlock *show_block = BasicBlock::Create(llvm_context, "show", &F, &first_block);
    
    GlobalVariable *the_depth = M.getNamedGlobal("depthVar");

    IRBuilder<> IR_depth(depth_block);
    Value *va = IR_depth.CreateLoad(the_depth->getValueType(), the_depth);
    Value *va2 = IR_depth.CreateAdd(va, one);
    IR_depth.CreateStore(va2, the_depth);
    Value *count = IR_depth.CreateAlloca(Type::getInt32Ty(llvm_context));
    IR_depth.CreateStore(ConstantInt::get(Type::getInt32Ty(llvm_context), 1), count);
    Value *noSpace = IR_depth.CreateICmpEQ(IR_depth.CreateLoad(Type::getInt32Ty(llvm_context), the_depth), one);
    IR_depth.CreateCondBr(noSpace, show_block, space_block);
    
    IRBuilder<> IR_space(space_block);
    IR_space.CreateCall(the_callee, {space});
    Value *count_Plus = IR_space.CreateAdd(IR_space.CreateLoad(Type::getInt32Ty(llvm_context), count), one);
    IR_space.CreateStore(count_Plus, count);
    Value *eq = IR_space.CreateICmpEQ(IR_space.CreateLoad(the_depth->getValueType(), the_depth), IR_space.CreateLoad(Type::getInt32Ty(llvm_context), count));
    IR_space.CreateCondBr(eq, show_block, space_block);

    IRBuilder<> IR_show(show_block);
    Value *funcName = IR_show.CreateGlobalStringPtr(F.getName().str());
    Value *funcPtr = IR_show.CreatePointerCast(&F, Type::getInt8PtrTy(llvm_context));

    Constant *formatString = ConstantDataArray::getString(llvm_context, "%s: %p\n");
    GlobalVariable *formatStringVar = new GlobalVariable(M, formatString->getType(), true, GlobalValue::InternalLinkage, formatString, "formatString");
    errs() << funcName << " " << funcPtr << "\n";
    std::vector<Value *> printfArgs = {formatStringVar, funcName, funcPtr};

    IR_show.CreateCall(the_callee, printfArgs);
    IR_show.CreateBr(&first_block);

    BasicBlock &final_block = F.back(); 
    Instruction &final_ins = *(++final_block.rend());

    IRBuilder<> IR_sub(&final_ins);
    Value *val3 = IR_sub.CreateLoad(the_depth->getValueType(), the_depth);
    Value *v4 = IR_sub.CreateSub(val3, one);
    IR_sub.CreateStore(v4, the_depth);

    dumpIR(F);
  }
  return true;
}
static RegisterPass<LabPass> X("labpass", "Lab Pass", false, false);