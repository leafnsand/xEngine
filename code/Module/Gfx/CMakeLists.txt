fips_begin_lib(Gfx)
    #fips_src(shader *.sc NO_RECURSE)
	#  bgfx_shaders(FILES
    #    fs_gfx_col.sc
    #    fs_gfx_tex.sc
    #    vs_gfx.sc
    #)
    fips_include_directories(.)
    fips_src(. GROUP_FOLDERS)
    fips_deps(AzCore)
fips_end_lib()