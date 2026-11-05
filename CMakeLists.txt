FILE(GLOB_RECURSE lib_sources "./src/*.cpp")

set(priv_requires esp_hw_support esp_netif)
if(IDF_VERSION_MAJOR GREATER_EQUAL 5)
  list(APPEND priv_requires esp_app_format)
else()
  list(APPEND priv_requires app_update)
endif()

idf_component_register(COMPONENT_NAME "HomeAssistantEntities"
                        SRCS ${lib_sources}
                        INCLUDE_DIRS "./src/" "./src/entities/"
                        PRIV_REQUIRES ${priv_requires})

if(IDF_VERSION_MAJOR LESS 5) # 5+ compiles with c++23.
target_compile_options(${COMPONENT_LIB} PRIVATE -std=gnu++17)
endif()
