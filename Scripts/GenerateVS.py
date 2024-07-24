import os
import subprocess
from DeleteProjects import Delete

def Generate():
    subprocess.run(["Vendor/premake/premake5.exe", "vs2022"])

if __name__ == "__main__":
    os.chdir("../")
    Delete()
    print("\n")
    Generate()
    input("Press Enter to continue...")
