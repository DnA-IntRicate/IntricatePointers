import os

def Delete():
    if os.path.isfile("Examples.sln"):
        os.remove("Examples.sln")
        print("Deleted: Examples.sln")

    if os.path.isfile("Example-Ref/Example-Ref.vcxproj"):
        os.remove("Example-Ref/Example-Ref.vcxproj")
        print("Deleted: Example-Ref/Example-Ref.vcxproj")

    if os.path.isfile("Example-Ref/Example-Ref.vcxproj.filters"):
        os.remove("Example-Ref/Example-Ref.vcxproj.filters")
        print("Deleted: Example-Ref/Example-Ref.vcxproj.filters")

    if os.path.isfile("Example-Ref/Example-Ref.vcxproj.user"):
        os.remove("Example-Ref/Example-Ref.vcxproj.user")
        print("Deleted: Example-Ref/Example-Ref.vcxproj.user")

if __name__ == "__main__":
    os.chdir("../")
    print("Deleting Visual Studio projects...")
    Delete()
    input("Press Enter to continue...")
