file(GLOB_RECURSE headers CONFIGURE_DEPENDS "*.hpp" "*.h")
target_sources(
    mb.cpp-app-template
    ${_mb_cpp_app_template_fs_scope}
    FILE_SET HEADERS
    FILES
    ${headers}
)
