find_path(FLINT_INCLUDE_DIR flint/flint.h)
find_library(FLINT_LIBRARIES NAMES flint)

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(FLINT DEFAULT_MSG FLINT_INCLUDE_DIR FLINT_LIBRARIES)

mark_as_advanced(FLINT_INCLUDE_DIR FLINT_LIBRARIES)

add_library(FLINT UNKNOWN IMPORTED)
set_target_properties(FLINT PROPERTIES
        IMPORTED_LOCATION ${FLINT_LIBRARIES}
        INTERFACE_INCLUDE_DIRECTORIES ${FLINT_INCLUDE_DIR}
        )
