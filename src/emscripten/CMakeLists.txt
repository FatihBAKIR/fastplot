add_executable(emplot em_ctx.cpp)
target_link_libraries(emplot PUBLIC fastplot_gl)
set_target_properties(emplot PROPERTIES LINK_FLAGS "-s USE_WEBGL2=1 -s OFFSCREEN_FRAMEBUFFER=1")
set_target_properties(emplot PROPERTIES SUFFIX ".html")