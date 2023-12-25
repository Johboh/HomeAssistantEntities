FILE(GLOB_RECURSE lib_sources "./src/*.*" "./src/entities/*.*")

idf_component_register(COMPONENT_NAME "HomeAssistantEntities"
                        SRCS ${lib_sources}
                        INCLUDE_DIRS "./src/" "./src/entities/")

if(IDF_VERSION_MAJOR LESS 5) # 5+ compiles with c++23.
target_compile_options(${COMPONENT_LIB} PRIVATE -std=gnu++17)
endif()