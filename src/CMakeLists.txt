add_executable(app)
target_sources(app PRIVATE main.cpp)
target_link_libraries(app PRIVATE deps)
