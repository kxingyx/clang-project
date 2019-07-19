
#include "clang/Driver/Options.h"
#include "clang/AST/AST.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/ASTConsumers.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"

using namespace std;
using namespace clang;
using namespace clang::driver;
using namespace clang::tooling;
using namespace llvm;

static llvm::cl::OptionCategory StatSampleCategory("Stat Sample");

class IndexVisitor : public RecursiveASTVisitor<IndexVisitor> {
private:
    ASTContext *astContext;

public:
    explicit IndexVisitor(CompilerInstance *CI): astContext(&(CI->getASTContext())) {
    }

    virtual bool VisitFunctionDecl(FunctionDecl *func) {
        string funcName = func->getNameInfo().getName().getAsString();
        errs() << " function def: " << funcName << " @ " << func->getLocation().printToString(astContext->getSourceManager()) << "\n";
        return true;
    }

    virtual bool VisitStmt(Stmt *st) {
        if (CallExpr *call = dyn_cast<CallExpr>(st)) {
	    if (auto func = call->getDirectCallee()) {
                string s =  func->getLocation().printToString(astContext->getSourceManager());
                string funcName = func->getNameInfo().getName().getAsString();
                errs() << " function call: "<< funcName << ":"  << s << " @ " << call->getBeginLoc().printToString(astContext->getSourceManager()) << "\n";
	    }
        }
        return true;
    }
};

class IndexASTConsumer : public ASTConsumer {
private:
    IndexVisitor *visitor;

public:
    explicit IndexASTConsumer(CompilerInstance *CI): visitor(new IndexVisitor(CI)) {}

    virtual void HandleTranslationUnit(ASTContext &Context) {
        visitor->TraverseDecl(Context.getTranslationUnitDecl());
    }
};

class IndexFrontendAction : public ASTFrontendAction {
public:
    IndexFrontendAction() {}

    virtual std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI, StringRef file) {
        return  llvm::make_unique<IndexASTConsumer>(&CI); // pass CI pointer to ASTConsumer
    }
};

int main(int argc, const char **argv) {
    CommonOptionsParser op(argc, argv, StatSampleCategory);
    ClangTool Tool(op.getCompilations(), op.getSourcePathList());

    return Tool.run(newFrontendActionFactory<IndexFrontendAction>().get());
}
