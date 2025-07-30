/******************************************************************************
MIT License

Copyright (c) 2025 bwyeag

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
******************************************************************************/
#ifndef BL_UTIL_RESULT_TYPE_FILE
#include <chrono>
#include <print>
namespace blt {
//*****************************************************************************
// 计时类
//*****************************************************************************

/// @brief 标准计时器
class Timer {
  using Clock = std::chrono::high_resolution_clock;
  using TimePoint = Clock::time_point;
  using TimeDuration = Clock::duration;
  TimePoint m_TimeBegin, m_TimeEnd;

public:
  void begin() { m_TimeBegin = Clock::now(); }
  void end() { m_TimeEnd = Clock::now(); }
  double duration() const {
    auto Cast = [](TimeDuration dt) {
      return std::chrono::duration_cast<std::chrono::nanoseconds>(dt);
    };
    return Cast(m_TimeEnd - m_TimeBegin).count() * 1e-6;
  }
};
/// @brief 作用域计时器
class ScopedTimer : protected Timer {
  double &m_target;

public:
  ScopedTimer(double &tar) : m_target(tar) { this->begin(); }
  ~ScopedTimer() {
    this->end();
    m_target = this->duration();
  }
};
/// @brief 自动的作用域计时器，在开始和结束时自动输出计时结果
class AutoScopedTimer : protected Timer {
  const char *m_Name;

public:
  AutoScopedTimer(const char *name) : m_Name(name) {
    std::println(stdout, "Timer Begin [{}];", m_Name);
    this->begin();
  }
  ~AutoScopedTimer() {
    this->end();
    std::println(stdout, "Timer End [{}] {}ms;", m_Name, this->duration());
  }
};

} // namespace BLT
#endif // !BL_UTIL_RESULT_TYPE_FILE
