add_lldb_library(lldbExpression
  DWARFExpression.cpp
  Expression.cpp
  ExpressionSourceCode.cpp
  ExpressionVariable.cpp
  FunctionCaller.cpp
  IRDynamicChecks.cpp
  IRExecutionUnit.cpp
  IRInterpreter.cpp
  IRMemoryMap.cpp
  LLVMUserExpression.cpp
  Materializer.cpp
  REPL.cpp
  UserExpression.cpp
  UtilityFunction.cpp
# +===== Scout ========
  Scout/ClangASTSource.cpp
# ====== Scout ========
)
