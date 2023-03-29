#include "maple.h"

namespace epsilon {

/* callback used for directing result output */
void M_DECL
Maple::textCallBack(void* data, int tag, const char* output)
{
  std::cout << "Maple output: " << output << std::endl;
}

void M_DECL
Maple::ErrorCallBack(void* data, long int tag, const char* output)
{
  std::cerr << std::endl << "MAPLE ERROR: " << output << std::endl;
  exit(1);
}

/* interrupt callback: stops computation when `Interrupted' is true */
M_BOOL M_DECL
Maple::queryInterrupt(void* data)
{
  if (Interrupted) {
    Interrupted = 0;
    return (TRUE);
  }
  return (FALSE);
}

Maple::Maple()
{
#if (defined(WITH_MAPLE) && WITH_MAPLE)
  StartMapleKernel(0, nullptr);
#else
  kv = NULL;
#endif
}
Maple::Maple(int argc, char** argv)
{
#if (defined(WITH_MAPLE) && WITH_MAPLE)
  StartMapleKernel(argc, argv);
#else
  kv = NULL;
#endif
}

Maple::~Maple()
{
#if (defined(WITH_MAPLE) && WITH_MAPLE)
  StopMaple(kv);
#endif
}

void
Maple::StartMapleKernel(int argc, char** argv)
{
  MCallBackVectorDesc cb{
    textCallBack,   ErrorCallBack, nullptr, /* statusCallBack not used */
    nullptr,                                /* readLineCallBack not used */
    nullptr,                                /* redirectCallBack not used */
    nullptr,                                /* streamCallBack not used */
    queryInterrupt, nullptr                 /* callBackCallBack not used */
  };
  std::array<char, 2048> err; /* command input and error string buffers */

  /* initialize Maple */
  if ((kv = StartMaple(argc, argv, &cb, nullptr, nullptr, err.data())) ==
      nullptr) {
    std::cerr << "MapleTestTool ERROR: Cannot start maple kernel " << err
              << std::endl;
    exit(1);
  }
  /* catch ^C */
  signal(SIGINT, catch_intr);
}

void
Maple::RestartMapleKernel()
{
#if (defined(WITH_MAPLE) && WITH_MAPLE)
  std::array<char, 2048> err;
  if (RestartMaple(kv, err.data()) == 0) {
    std::cerr << "MapleTestTool ERROR: Cannot restart the marple kernel " << err
              << std::endl;
    exit(1);
  }
#endif
}

void
Maple::EvalMapleProc(const fs::path& maple_proc_path)
{
  // 读入Maple函数
  // auto maple_proc_path = getDataPath("epsilon-cpp", "src/maple.inc");
  std::string maple_proc_str;

  if (std::ifstream is{ maple_proc_path, // 读取整个文件到 string
                        std::ios::in | std::ios::ate }) {
    auto size = is.tellg();
    std::string str(size, '\0'); // 构造 string 为流大小
    is.seekg(0);
    if (!is.read(&str[0], size))
      std::cout << "error" << '\n';
    maple_proc_str = str;
  }

  // std::cout << maple_func_str << std::endl;
  EvalMapleStatement(kv, maple_proc_str.c_str());
}

ALGEB
Maple::AssignMapleStatement(const std::string& name_str,
                            const std::string& statement_str)
{
  ALGEB statement_maple, name_maple;
  auto statement = statement_str + ":";
  statement_maple = EvalMapleStatement(kv, statement.c_str());
  name_maple = ToMapleName(kv, name_str.c_str(), TRUE);
  MapleAssign(kv, name_maple, statement_maple);
  return name_maple;
}

std::string
Maple::algebToString(MKernelVector kv, ALGEB in) const
{
#if (defined(MAPLE_VALIDATE) && MAPLE_VALIDATE) ||                             \
  (defined(WITH_MAPLE) && WITH_MAPLE)
  std::array<char, 9> printStr{ "sprintf:" };
  ALGEB f = EvalMapleStatement(kv, printStr.data());
  ALGEB resStrObj = ::EvalMapleProc(kv, f, 2, ToMapleString(kv, "%a"), in);
  char* resStr = MapleToString(kv, resStrObj);
  return { resStr };
#else
  return "";
#endif
}

bool
Maple::testEquality(ALGEB test,
                    const std::string& other,
                    const std::string& verifyMethod) const
{
#if (defined(WITH_MAPLE) && WITH_MAPLE)
  std::string otherColon = other + ":";
  ALGEB otherALGEB =
    EvalMapleStatement(kv, const_cast<char*>(otherColon.c_str()));
  return testEquality(test, otherALGEB, verifyMethod);
#else
  return 1;
#endif
}

bool
Maple::testEquality(ALGEB test,
                    ALGEB other,
                    const std::string& verifyMethod) const
{
#if (defined(WITH_MAPLE) && WITH_MAPLE)
  std::array<char, 8> expandFunc{ "expand:" };
  ALGEB expF = EvalMapleStatement(kv, expandFunc.data());
  ALGEB expTest = ::EvalMapleProc(kv, expF, 1, test);
  ALGEB expOther = ::EvalMapleProc(kv, expF, 1, other);

  std::array<char, 8> compareFunc{ "verify:" };
  ALGEB cmpF = EvalMapleStatement(kv, compareFunc.data());

  if (verifyMethod != "") {
    std::string verStr = verifyMethod + ":";
    ALGEB verMthd = EvalMapleStatement(kv, const_cast<char*>(verStr.c_str()));
    ALGEB comp = ::EvalMapleProc(kv, cmpF, 3, expTest, expOther, verMthd);
    M_BOOL compBool = MapleToM_BOOL(kv, comp);
    return (bool)compBool;

  } else {
    ALGEB comp = ::EvalMapleProc(kv, cmpF, 2, expTest, expOther);
    M_BOOL compBool = MapleToM_BOOL(kv, comp);
    return (bool)compBool;
  }
#else
  return 1;
#endif
}

}
