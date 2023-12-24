FILE(GLOB_RECURSE lib_sources "./src/*.*" "./src/entities/*.*")

idf_component_register(COMPONENT_NAME "HomeAssistantEntities"
                        SRCS ${lib_sources}
                        INCLUDE_DIRS "./src/" "./src/entities/")
