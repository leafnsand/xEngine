fips_begin_module(FairyGUI)
	fips_vs_disable_warnings(4800)
    fips_src(shader *.sc NO_RECURSE)
    bgfx_shaders(FILES
        vs_fairy.sc
        fs_fairy_normal.sc
        fs_font_normal.sc
    )
    fips_vs_warning_level(3)
    fips_vs_disable_warnings(4244 4996 4307 4018 4267)	
    fips_include_directories(.)
    fips_include_directories(fairy)
    fips_src(. GROUP_FOLDERS)
    fips_deps(AzCore Renderer Font Window lua)
fips_end_module()

if (FIPS_CLANG)
    set_target_properties(FairyGUI PROPERTIES COMPILE_FLAGS "-Wno-sign-compare -Wno-unused-variable -Wno-reorder -Wno-unused-private-field")
endif()