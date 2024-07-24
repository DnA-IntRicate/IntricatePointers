project "Test-WeakRefMemoryLeak"
    kind "ConsoleApp"
    language "C++"

    debugdir(OUT_DIR)
    targetdir(OUT_DIR)
    objdir(INT_DIR)

    files
    {
        "./*.hpp",
        "./*.cpp"
    }

    includedirs
    {
        ".",
        "%{INTRICATE_POINTERS_HPP_INCLUDE}"
    }
