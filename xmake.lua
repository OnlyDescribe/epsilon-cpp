-- ----------------------------------------------------------------- --
add_rules("mode.release","mode.debug","mode.profile")

if is_mode("debug") then
    add_undefines("NDEBUG") 
    set_symbols("debug")  -- enable debug symbols
    set_optimize("none")  -- disable optimization
end

if is_mode("release", "profile") then
    if is_mode("release") then
        set_symbols("hidden")  -- mark symbols visibility as hidden
        set_strip("all")  -- strip all symbols
        add_cxflags("-fomit-frame-pointer")  -- fomit frame pointer
        
    else
        set_symbols("debug")  -- enable debug symbols
        add_cxflags("-pg")
        add_ldflags("-pg")
    end
    add_defines("NDEBUG")
    -- add vectorexts
    -- add_vectorexts("mmx")
    add_vectorexts("neon")
    add_vectorexts("avx", "avx2")
    add_vectorexts("sse", "sse2", "sse3", "ssse3")
    set_optimize("fastest")
end

if is_plat("linux") then
    add_requires("gmp","ntl","flint","tbb")
    add_requires("Singular")
    add_cxflags("-m64")
    set_languages("cxx17")
else 
    add_requires("conan::gmp/6.2.1",{alias = "gmp"})
    add_requires("conan::flint/2.8.1",{alias = "flint"})
    add_requires("conan::tbb/2020.3",{alias = "tbb"})
end

target("main")
    if is_plat("linux") then
        set_enabled(true)
    else 
        set_enabled(false )
    end
    set_kind("binary")
    -- set_rundir("$(projectdir)")

    add_includedirs("src/","test/","dep/")
    add_includedirs("./dep/thread_pool")

    add_files("main.cc")
    add_files("src/*.cc","test/*.cc")

    -- -- factory
    -- add_deps("factory")
    add_includedirs("dep/factory/","dep/factory/templates/","dep/factory/include/")
    add_undefines("NOSTREAMIO")
    add_defines("HAVE_IOSTREAM")
    add_packages("Singular")

    -- tracy
    add_files("./dep/tracy/public/TracyClient.cpp")
    add_includedirs("./dep/tracy/public/tracy")
    if is_mode("profile") then
        add_defines("TRACY_ENABLE")
        add_syslinks("pthread","dl")
    end

    -- openmaple
    if is_plat("linux") then
        add_defines("WITH_MAPLE")
        add_includedirs("/home/oldcb/maple2022/extern/include")
        add_links("maplec","rt")
        add_linkdirs("/home/oldcb/maple2022/bin.X86_64_LINUX/")
        add_rpathdirs("/home/oldcb/maple2022/bin.X86_64_LINUX/")
        
        add_runenvs("LD_LIBRARY_PATH", "/home/oldcb/maple2022/bin.X86_64_LINUX/")
        add_runenvs("MAPLE", "/home/oldcb/maple2022/")
    end

    -- others
    add_packages("gmp","ntl","flint","tbb")


package("Singular")
    set_sourcedir(path.join(os.scriptdir(), "dep/Singular"))
    add_deps("autoconf", "automake", "libtool", "pkg-config")
    on_load(function (package)
        package:add("includedirs", "include/")
    end)
    on_install(function (package)
        local configs = {}
        table.insert(configs, "--enable-streamio")
        -- table.insert(configs, "--enable-debug")
        table.insert(configs, "--with-track-fl")
        table.insert(configs, "--with-track-backtrace")
        import("package.tools.autoconf").install(package, configs)
    end)

-- target("factory")
--     set_enabled(false)
--     set_kind("static")
--     add_undefines("NOSTREAMIO")
--     add_defines("HAVE_IOSTREAM")

--     add_includedirs("dep/","dep/Singular/factory/","dep/Singular/factory/","dep/Singular/factory/","dep/Singular/factory/templates/")
--     add_files("dep/Singular/factory/*.cc","dep/Singular/factory/ftmpl_inst.cc")

--     if is_mode("debug") then
--         add_undefines("NOASSERT","SING_NDEBUG")  -- FACTORY ASSERT
--     else 
--         add_defines("NOASSERT","SING_NDEBUG")
--     end
--     -- add_defines("TIMING")

--     add_includedirs("./dep/tracy/public/tracy")
--     if is_mode("profile") then
--         add_defines("TRACY_ENABLE")
--         add_syslinks("pthread","dl")
--     end

--     -- add_deps("omalloc")
--     add_packages("gmp","ntl","flint","tbb")

-- target("omalloc")
--     if is_plat("linux") then
--         set_enabled(true)
--     else 
--         set_enabled(false )
--     end
--     set_kind("static")
--     add_includedirs("./","omalloc/")
--     add_files("omalloc/*.c","omalloc/*.cc")
-- ----------------------------------------------------------------- --
