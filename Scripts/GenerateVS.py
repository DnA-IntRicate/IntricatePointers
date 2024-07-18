import os
import subprocess


def Generate():
    subprocess.run(["Vendor/premake/premake5.exe", "vs2022"])

def Delete():
    pass

if __name__ == "__main__":
    os.chdir("../")
    Delete()
    print("\n")
    Generate()

    input("Press Enter to continue...")
