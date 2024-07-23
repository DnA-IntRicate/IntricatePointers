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

    if os.path.isfile("Example-Scope/Example-Scope.vcxproj"):
        os.remove("Example-Scope/Example-Scope.vcxproj")
        print("Deleted: Example-Scope/Example-Scope.vcxproj")

    if os.path.isfile("Example-Scope/Example-Scope.vcxproj.filters"):
        os.remove("Example-Scope/Example-Scope.vcxproj.filters")
        print("Deleted: Example-Scope/Example-Scope.vcxproj.filters")

    if os.path.isfile("Example-Scope/Example-Scope.vcxproj.user"):
        os.remove("Example-Scope/Example-Scope.vcxproj.user")
        print("Deleted: Example-Scope/Example-Scope.vcxproj.user")

    if os.path.isfile("Example-WeakRef/Example-WeakRef.vcxproj"):
        os.remove("Example-WeakRef/Example-WeakRef.vcxproj")
        print("Deleted: Example-WeakRef/Example-WeakRef.vcxproj")

    if os.path.isfile("Example-WeakRef/Example-WeakRef.vcxproj.filters"):
        os.remove("Example-WeakRef/Example-WeakRef.vcxproj.filters")
        print("Deleted: Example-WeakRef/Example-WeakRef.vcxproj.filters")

    if os.path.isfile("Example-WeakRef/Example-WeakRef.vcxproj.user"):
        os.remove("Example-WeakRef/Example-WeakRef.vcxproj.user")
        print("Deleted: Example-WeakRef/Example-WeakRef.vcxproj.user")

    if os.path.isfile("Test-RefMemoryLeak/Test-RefMemoryLeak.vcxproj"):
        os.remove("Test-RefMemoryLeak/Test-RefMemoryLeak.vcxproj")
        print("Deleted: Test-RefMemoryLeak/Test-RefMemoryLeak.vcxproj")

    if os.path.isfile("Test-RefMemoryLeak/Test-RefMemoryLeak.vcxproj.filters"):
        os.remove("Test-RefMemoryLeak/Test-RefMemoryLeak.vcxproj.filters")
        print("Deleted: Test-RefMemoryLeak/Test-RefMemoryLeak.vcxproj.filters")

    if os.path.isfile("Test-RefMemoryLeak/Test-RefMemoryLeak.vcxproj.user"):
        os.remove("Test-RefMemoryLeak/Test-RefMemoryLeak.vcxproj.user")
        print("Deleted: Test-RefMemoryLeak/Test-RefMemoryLeak.vcxproj.user")

if __name__ == "__main__":
    os.chdir("../")
    print("Deleting Visual Studio projects...")
    Delete()
    input("Press Enter to continue...")
