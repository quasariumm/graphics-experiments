# For all FetchContent
set(BUILD_SHARED_LIBS OFF)

# Common compiler/preprocessor args
function(setup_common_compiler_args target)
	target_compile_options(${target} PUBLIC -march=native)
	if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
		include(CheckCXXCompilerFlag)
		check_cxx_compiler_flag(-Wno-TU-local-entity-exposure HAS_WNO_TU_LOCAL)
		if (HAS_WNO_TU_LOCAL)
			add_compile_options(-Wno-TU-local-entity-exposure)
		endif ()
		add_compile_options(-Wno-unknown-attributes)
	endif ()
	target_compile_definitions(${target} PUBLIC _SILENCE_CXX23_DENORM_DEPRECATION_WARNING)
	target_compile_definitions(${target} PUBLIC NOMINMAX WIN32_LEAN_AND_MEAN)
	target_compile_features(${target} PUBLIC cxx_std_23)
endfunction()