#include "test.h"

#include <chrono>
#include <csignal>
#include <ctime>
#include <fstream>
#include <future>
#include <optional>
#include <ostream>
#include <sched.h>
#include <sys/wait.h>
#include <unistd.h>

#include "convert.h"
#include "epsilon.h"
#include "triangularize.h"

#include <flint/flint.h>

#include "BS_thread_pool.hpp"
#include "canonicalform.h"

namespace epsilon {

constexpr int MAX_PROCESSES = 1;

static std::map<__pid_t, std::string> process_polys_name;

void
alarm_handler(int signum)
{
  std::cout << process_polys_name[getpid()] << "超时" << std::endl;
  kill(getpid(), SIGTERM); // 发送SIGTERM信号给自己
}

void
child_func(const nlohmann::basic_json<>::iterator& data_json_iter)
{
  const auto& poly_name = data_json_iter.key();
  const auto& poly_sys_json = data_json_iter.value();

  auto polys_json = poly_sys_json.at(std::string("polys"));
  auto vars_json = poly_sys_json.at(std::string("vars"));

  auto PS = convJsonPSFactoryPS(polys_json, vars_json, false);
  auto vars = vars_json.get<std::vector<std::string>>();

  CFSet QS;
  if (PS) {
    std::chrono::duration<double> elapsed_seconds;

    // epsilon::Triangularize<TriSer> Solver(*PS, QS);
    // epsilon::TriangularizeWithTree<TriSer> Solver(*PS, QS);
    std::cout << poly_name << ": " << std::flush;

    auto start = std::chrono::steady_clock::now();

    epsilon::TriangularizeWithStl<TriSer> Solver(*PS, QS);
    Solver.Elimination();

    auto end = std::chrono::steady_clock::now();

    elapsed_seconds = std::chrono::duration<double>(end - start);
    std::cout << elapsed_seconds.count() << "s" << std::setw(20) << "\n"
              << std::flush;
    usleep(1000);
  }
}

int
Triangularize_Test(int argc, char** argv)
{

  // 得到 bpas_polys.json 文件位置
  // auto json_path = getDataPath("epsilon-cpp", "data/biology_polys.json");
  auto json_path = getDataPath("epsilon-cpp", "data/bpas_polys.json");

  // 反序列化
  json data_json = json::parse(std::ifstream{ json_path.value() });

  // 处理json
  nlohmann::basic_json<>::value_type poly_sys_json;
  nlohmann::basic_json<>::value_type polys_json;
  nlohmann::basic_json<>::value_type vars_json;

  int count = 0; // 当前已经运行的子进程数

  // int pipefd[data_size][2];

  auto data_json_iter = std::next(data_json.begin(), 0);
  for (; data_json_iter != data_json.end();) {

    if (count < MAX_PROCESSES) {
      pid_t pid = fork();
      if (pid < 0) {
        perror("创建子进程失败");
        exit(EXIT_FAILURE);
      } else if (pid == 0) { // 子进程
        setCharacteristic(0);
        On(SW_RATIONAL);

        signal(SIGALRM, alarm_handler); // 注册SIGALRM信号处理函数
        alarm(500); // 设置定时器，500秒后发送SIGALRM信号

        // 子进程的具体任务
        process_polys_name[getpid()] = data_json_iter.key();
        epsilon::child_func(data_json_iter);
        // child_func(data_json_iter, pipefd[index][0], pipefd[index][1]);

        exit(EXIT_SUCCESS);
      } else {
        std::advance(data_json_iter, 1);
        // index++;
        count++; // 已运行子进程数加1
      }
    } else { // 同时运行的子进程数已达到最大值，等待子进程退出
      int status;
      pid_t child_pid = waitpid(-1, &status, 0);
      if (child_pid > 0) {
        if (WIFEXITED(status)) { // 子进程正常退出
          //   std::cout << "子进程 " << child_pid << " 退出，返回状态为 "
          //             << WEXITSTATUS(status) << std::endl;
          count--;                        // 已运行子进程数减1
        } else if (WIFSIGNALED(status)) { // 子进程被信号中断
          if (WTERMSIG(status) != SIGTERM)
            std::cout << "子进程 " << child_pid << " 被信号中断, 信号为 "
                      << WTERMSIG(status) << ", 开启新进程" << std::endl;
          count--; // 已运行子进程数减1
        }
      }
    }
  }

  while (true) {
    int status;
    pid_t finished_pid = wait(&status);
    if (finished_pid > 0) {
      if (WIFEXITED(status)) { // 子进程正常退出
        //   std::cout << "子进程 " << child_pid << " 退出，返回状态为 "
        //             << WEXITSTATUS(status) << std::endl;
      } else if (WIFSIGNALED(status)) { // 子进程被信号中断
        std::cout << "子进程 " << finished_pid << " 被信号中断, 信号为 "
                  << WTERMSIG(status) << std::endl;
      }
    } else if (finished_pid < 0) { // 所有子进程已结束
      break;
    }
  }

  return 0;
}

}