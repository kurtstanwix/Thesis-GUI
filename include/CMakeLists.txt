set(FILES "")
set(SUBDIRECTORIES "GraphicsTest")
set(RELATIVE_PATH "include")

foreach(VAR ${SUBDIRECTORIES})
    add_subdirectory(${VAR})
endforeach()

if(FILES)
    target_sources("${PROJECT_NAME}" PRIVATE ${FILES})
endif()
