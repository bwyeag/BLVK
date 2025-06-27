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
#include "GLFW/glfw3.h"
#include <bl_contexts2.hpp>
namespace BLT {
//*****************************************************************************
// WindowContextBase_*** 类
//*****************************************************************************
CtxResult WindowContextBase_glfw::init_glfw() {
  static bool init_successful = false;
  std::call_once(s_InitOnce, [] {
    if (!glfwInit() || !glfwVulkanSupported()) {
      print_error("Context", "Failed to initialize GLFW!");
      return;
    }
    glfwSetErrorCallback([](int error_code, const char *description) {
      print_error("GLFW", "Error Code:", error_code, "; Desc:", description);
    });
    init_successful = true;
  });
  return init_successful ? CtxResult::Success:CtxResult::Failed;
}
void WindowContextBase_glfw::cleanup_glfw() noexcept {
  glfwTerminate();
}
CtxResult WindowContextBase_glfw::create(const WindowCreateInfo_glfw &info) {
  using State = WindowCreateState;
  if (CtxResult result = init_glfw(); result != CtxResult::Success)
    return result;
  // 1. 选取所用的监视器
  int monitor_count;
  GLFWmonitor **pMonitors = glfwGetMonitors(&monitor_count);
  if (!pMonitors)
    print_error(s_TypeName, "No monitors!"), abort();
  auto use_primary_monitor = [this]() {
    m_pMonitor = glfwGetPrimaryMonitor();
    if (!m_pMonitor)
      print_error(s_TypeName, "No primary monitor!"), abort();
  };
  if (info.m_InitState & State::use_primary_monitor) {
    use_primary_monitor();
  } else if (info.m_MonitorPred) {
    for (int i = 0; i < monitor_count; ++i)
      if (info.m_MonitorPred(pMonitors[i])) {
        m_pMonitor = pMonitors[i];
        break;
      }
    print_warning(s_TypeName, "No monitor selected! Use primary monitor.");
    use_primary_monitor();
  } else {
    print_error(s_TypeName, "No Monitor select function!");
    return CtxResult::ArgumentError;
  }
  // 2. 创建窗口
  m_Title = info.m_InitTitle;
  glfwDefaultWindowHints();
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE,
                 static_cast<bool>(info.m_InitState & State::resizable));
  glfwWindowHint(GLFW_DECORATED,
                 static_cast<bool>(info.m_InitState & State::decorated));
  glfwWindowHint(GLFW_AUTO_ICONIFY,
                 static_cast<bool>(info.m_InitState & State::auto_iconify));
  glfwWindowHint(GLFW_FLOATING,
                 static_cast<bool>(info.m_InitState & State::window_floating));
  glfwWindowHint(
      GLFW_CENTER_CURSOR,
      static_cast<bool>(info.m_InitState & State::init_mouse_centered));
  glfwWindowHint(GLFW_VISIBLE,
                 !static_cast<bool>(info.m_InitState & State::init_unvisiable));
  State size_state = State(info.m_InitState & State::size_mask);
  const GLFWvidmode *pMode = glfwGetVideoMode(m_pMonitor);
  if (!pMode) {
    print_error("WindowContext", "Get video mode failed!");
    return CtxResult::GetVideoModeFailed;
  }
  switch (size_state) {
  case State::full_screen:
    m_pWindow = glfwCreateWindow(pMode->width, pMode->height, m_Title.c_str(),
                                 m_pMonitor, nullptr);
    break;
  case State::maximized:
    glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);
    m_pWindow = glfwCreateWindow(pMode->width, pMode->height, m_Title.c_str(),
                                 nullptr, nullptr);
    break;
  case State::specified:
    m_pWindow = glfwCreateWindow(info.m_InitSizeX, info.m_InitSizeY,
                                 m_Title.c_str(), nullptr, nullptr);
    break;
  default:
    m_pMonitor = nullptr;
    m_Title.clear();
    return CtxResult::ArgumentError;
  }
  if (!m_pWindow) {
    m_pMonitor = nullptr, m_pWindow = nullptr;
    m_Title.clear();
    return CtxResult::WindowCreateFailed;
  }
  if (info.m_InitPosX != (~0u) && info.m_InitPosY != (~0u))
    glfwSetWindowPos(m_pWindow, info.m_InitPosX, info.m_InitPosY);
  glfwSetWindowSizeLimits(m_pWindow, info.m_MinSizeX, info.m_MinSizeY,
                          info.m_MaxSizeX, info.m_MaxSizeY);
  glfwSetWindowUserPointer(m_pWindow, this);
  print_log(s_TypeName,
            std::format("Window created! Title:{} Position:{},{} Size:{},{} "
                        "with Size limits:{},{}~{},{}\n",
                        m_Title, info.m_InitPosX, info.m_InitPosY,
                        info.m_InitSizeX, info.m_InitSizeY, info.m_MinSizeX,
                        info.m_MinSizeY, info.m_MaxSizeX, info.m_MaxSizeY));
  return CtxResult::Success;
}
void WindowContextBase_glfw::cleanup() noexcept {
  if (m_pWindow) glfwDestroyWindow(m_pWindow), m_pWindow = nullptr;
  m_pMonitor = nullptr,m_Title.clear();
}
//*****************************************************************************
// WindowContext 类
//*****************************************************************************

//*****************************************************************************
// Context 类
//*****************************************************************************
} // namespace BLT
