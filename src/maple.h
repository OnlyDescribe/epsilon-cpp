#ifndef MAPLE_H
#define MAPLE_H

#include "gmp.h"
#include "maplec.h"
// #include "mplshlib.h"

#include <atomic>
#include <csignal>
#include <fstream>

#include "convert.h"
#include "epsilon.h"

namespace epsilon {
#ifdef _MSC_VER
#define CDECL __cdecl
#else
#define CDECL
#endif

// AssignMapleStatement: string = string - > algeb
// ToMapleName: string -> algeb
// EvalMapleStatement: string
// EvalMapleProc: algeb algeb_args -> algeb
// MapleAssign: algeb algeb -> algeb
class Maple
{
public:
  Maple(const Maple&) = delete;
  Maple(Maple&&) = delete;
  Maple& operator=(const Maple&) = delete;
  Maple& operator=(Maple&&) = delete;

  static Maple& GetMaple()
  {
    static Maple mapleInstance;
    return mapleInstance;
  }
  MKernelVector& GetMKernelVector() { return kv; }

  void StartMapleKernel(int argc, char** argv);

  void RestartMapleKernel();

  void EvalMapleProc(const fs::path& maple_proc_path);

  ALGEB AssignMapleStatement(const std::string& name_str,
                             const std::string& statement_str);

  /**
   * Helper function for ALGEB to string.
   */
  std::string algebToString(MKernelVector kv, ALGEB in) const;

  /**
   * Compare an ALGEB and a string for equality.
   */
  bool testEquality(ALGEB test,
                    const std::string& other,
                    const std::string& verifyMethod) const;

  inline bool testEquality(ALGEB test, const std::string& other) const
  {
    return testEquality(test, other, "");
  }

  /**
   * Compare two ALGEBS for equality.
   */
  inline bool testEquality(ALGEB test, ALGEB other) const
  {
    return testEquality(test, other, "");
  }

  /**
   * Compare two ALGEBS for equality using some verification methods.
   */
  bool testEquality(ALGEB test,
                    ALGEB other,
                    const std::string& verifyMethod) const;

private:
  Maple();
  Maple(int argc, char** argv);
  ~Maple();

  MKernelVector kv; /* Maple kernel handle */

  inline static std::atomic_int Interrupted = 0;

  /* callback used for directing result output */
  static void M_DECL textCallBack(void* data, int tag, const char* output);
  static void M_DECL ErrorCallBack(void* data,
                                   long int tag,
                                   const char* output);
  /* interrupt callback: stops computation when `Interrupted' is true */
  static M_BOOL M_DECL queryInterrupt(void* data);
  /* interrupt signal handler: sets global variable when user hits Ctrl-C */
  static void CDECL catch_intr(int signo)
  {
    Interrupted = TRUE;
    signal(SIGINT, catch_intr);
#ifdef _MSC_VER
    signal(SIGBREAK, catch_intr);
#endif
  }
};

}

#endif