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

  FunctionCallee printfCallee = printfPrototype(M); // 取得 printf 函式的 FunctionCallee
  for (auto &F : M) {
    if (F.empty()) { // 如果函式為空，則跳過
      continue;
    }
    
    errs() << F.getName() << "\n";


    Constant *space = getI8StrVal(M, " ", "space");
    Constant *one = Constant::getIntegerValue(Type::getInt32Ty(llvm_context), APInt(32, 1));
    BasicBlock &Bstart = F.front(); // 取得函式的第一個 Basic Block
    BasicBlock &Bend = F.back(); // 取得函式的最後一個 Basic Block
    Instruction &ret = *(++Bend.rend()); // 取得最後一個指令，即 "ret" 指令
    BasicBlock *Bret = Bend.splitBasicBlock(&ret, "ret"); // 把 "ret" 指令替換成一個新的 Basic Block，並將它命名為 "ret"

    BasicBlock *Bdepth = BasicBlock::Create(llvm_context, "depth", &F, &Bstart);
    BasicBlock *Bspace = BasicBlock::Create(llvm_context, "space", &F, &Bstart);
    BasicBlock *Bshow = BasicBlock::Create(llvm_context, "show", &F, &Bstart);
    
    GlobalVariable *depth = M.getNamedGlobal("depthVar");

    IRBuilder<> BuilderDepth(Bdepth);
    Value *i0 = BuilderDepth.CreateLoad(depth->getValueType(), depth);
    Value *i1 = BuilderDepth.CreateAdd(i0, one);
    BuilderDepth.CreateStore(i1, depth);

    Value *cnt = BuilderDepth.CreateAlloca(Type::getInt32Ty(llvm_context));
    BuilderDepth.CreateStore(ConstantInt::get(Type::getInt32Ty(llvm_context), 1), cnt);

    Value *noSpace = BuilderDepth.CreateICmpEQ(BuilderDepth.CreateLoad(Type::getInt32Ty(llvm_context), depth), one);
    BuilderDepth.CreateCondBr(noSpace, Bshow, Bspace);


    
    IRBuilder<> BuilderSpace(Bspace);
   
    BuilderSpace.CreateCall(printfCallee, {space});
    Value *cntPlus = BuilderSpace.CreateAdd(BuilderSpace.CreateLoad(Type::getInt32Ty(llvm_context), cnt), one);
    BuilderSpace.CreateStore(cntPlus, cnt);
    Value *eq = BuilderSpace.CreateICmpEQ(BuilderSpace.CreateLoad(depth->getValueType(), depth), BuilderSpace.CreateLoad(Type::getInt32Ty(llvm_context), cnt));
    BuilderSpace.CreateCondBr(eq, Bshow, Bspace);


    IRBuilder<> BuilderShow(Bshow);
    Value *funcName = BuilderShow.CreateGlobalStringPtr(F.getName().str());
    Value *funcPtr = BuilderShow.CreatePointerCast(&F, Type::getInt8PtrTy(llvm_context));
    Constant *formatString = ConstantDataArray::getString(llvm_context, "%s: %p\n");
    GlobalVariable *formatStringVar = new GlobalVariable(M, formatString->getType(), true, GlobalValue::InternalLinkage, formatString, "formatString");
    
    errs() << funcName << " " << funcPtr << "\n";
    std::vector<Value *> printfArgs = {formatStringVar, funcName, funcPtr};
    BuilderShow.CreateCall(printfCallee, printfArgs);

    BuilderShow.CreateBr(&Bstart);

    BasicBlock &Bfinal = F.back(); 
    Instruction &final_ret = *(++Bfinal.rend());
    IRBuilder<> BuilderSub(&final_ret);
    Value *i2 = BuilderSub.CreateLoad(depth->getValueType(), depth);
    Value *i3 = BuilderSub.CreateSub(i2, one);
    BuilderSub.CreateStore(i3, depth);

    dumpIR(F);
  }

  return true;
}
static RegisterPass<LabPass> X("labpass", "Lab Pass", false, false);
