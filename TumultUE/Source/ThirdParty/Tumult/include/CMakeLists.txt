add_library(tumult INTERFACE)
add_library(tumult::core ALIAS tumult)
target_include_directories(tumult INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:include>
)
target_compile_features(tumult INTERFACE cxx_std_20)
target_link_libraries(tumult INTERFACE rpg::core)
