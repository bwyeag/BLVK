CPMAddPackage(
        NAME GLFW
        GITHUB_REPOSITORY glfw/glfw
        GIT_TAG 3.4
        OPTIONS
          "GLFW_BUILD_TESTS OFF"
          "GLFW_BUILD_EXAMPLES OFF"
          "GLFW_BUILD_DOCS OFF"
)

# CPMAddPackage(
#   NAME freetype
#   GIT_REPOSITORY https://github.com/aseprite/freetype2.git
#   GIT_TAG VER-2-10-0
#   VERSION 2.10.0
# )
# if (freetype_ADDED)
#   add_library(Freetype::Freetype ALIAS freetype)
# endif()

# CPMAddPackage(
#   NAME FreeImage
#   GITHUB_REPOSITORY Kanma/FreeImage
#   GIT_TAG 6bb613d9402926b3846dea1b417f20b6c596c5dc
# )
# if(FreeImage_ADDED)
#   target_include_directories(freeimage INTERFACE "${FreeImage_SOURCE_DIR}")
# endif()