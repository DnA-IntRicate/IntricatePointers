import os

def DeleteFile(path: str):
    if os.path.isfile(path):
        os.remove(path)
        print(f"Deleted: {path}")

def DeleteExamples():
    DeleteFile("Examples/Examples.sln")

    DeleteFile("Examples/Example-Ref/Example-Ref.vcxproj")
    DeleteFile("Examples/Example-Ref/Example-Ref.vcxproj.filters")
    DeleteFile("Examples/Example-Ref/Example-Ref.vcxproj.user")

    DeleteFile("Examples/Example-Scope/Example-Scope.vcxproj")
    DeleteFile("Examples/Example-Scope/Example-Scope.vcxproj.filters")
    DeleteFile("Examples/Example-Scope/Example-Scope.vcxproj.user")

    DeleteFile("Examples/Example-WeakRef/Example-WeakRef.vcxproj")
    DeleteFile("Examples/Example-WeakRef/Example-WeakRef.vcxproj.filters")
    DeleteFile("Examples/Example-WeakRef/Example-WeakRef.vcxproj.user")

def DeleteTests():
    DeleteFile("Tests/Tests.sln")

    DeleteFile("Tests/Test-RefMemoryLeak/Test-RefMemoryLeak.vcxproj")
    DeleteFile("Tests/Test-RefMemoryLeak/Test-RefMemoryLeak.vcxproj.filters")
    DeleteFile("Tests/Test-RefMemoryLeak/Test-RefMemoryLeak.vcxproj.user")

def Delete():
    DeleteExamples()
    DeleteTests()

if __name__ == "__main__":
    os.chdir("../")
    print("Deleting Visual Studio projects...")
    Delete()
    input("Press Enter to continue...")
