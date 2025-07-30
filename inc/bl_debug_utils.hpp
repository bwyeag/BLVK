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
#ifndef _BL_DEBUG_UTILS_HPP_
#define _BL_DEBUG_UTILS_HPP_
#ifdef DEBUG
// 本地库include
#include <bl_contexts.hpp>
// 标准库include
#include <iomanip>
namespace blt::debug {
template <class BaseCtx> struct FpsTitle {
  BaseCtx *m_WindowCtx{nullptr};
  ContextBase *m_Context{nullptr};
  double m_LastTime{-1.0};
  int m_FrameCount{0};

  void init(BaseCtx &windowCtx, ContextBase &context);
  void title_fps();
};
template <class BaseCtx>
void FpsTitle<BaseCtx>::init(BaseCtx &windowCtx, ContextBase &context) {
  m_WindowCtx = &windowCtx, m_Context = &context;
  m_Context->m_CallbackUpdate.insert([this](ContextBase *ctx) { title_fps(); });
}
template <class BaseCtx> void FpsTitle<BaseCtx>::title_fps() {
  if (double dt = m_Context->m_CurrentTime - m_LastTime; dt >= 1.0) {
    if (m_LastTime > 0.0) {
      auto &sstm = acquire_local_data().m_LocalSstream;
      double fps = m_FrameCount / dt;
      sstm << m_WindowCtx->get_window_title();
      sstm.setf(std::ios::fixed);
      sstm << std::setprecision(2) << fps;
      sstm.unsetf(std::ios::fixed);
      m_WindowCtx->set_window_title(sstm.str());
      sstm.str("");
      m_FrameCount = 0;
    }
    m_LastTime = m_Context->m_CurrentTime;
  } else
    ++m_FrameCount;
}
} // namespace blt::debug
#endif // DEBUG
#endif // !_BL_DEBUG_UTILS_HPP_
