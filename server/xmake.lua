add_rules("mode.debug", "mode.release")
add_rules("plugin.compile_commands.autoupdate", {outputdir = ".vscode"})

set_languages("c++20")
add_requires("nlohmann_json") 
add_requires("boost", {
        configs = {
            beast = true,
            system = true,
            asio = true,
        }
})

target("cserver")
    set_kind("binary")
    add_files("src/*.cpp")
    add_packages("boost")
    add_defines("BOOST_BEAST_USE_STD_STRING_VIEW=1")
    add_packages("nlohmann_json") 