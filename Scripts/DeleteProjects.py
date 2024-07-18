import os
import GenerateVS as VS

os.chdir("../")
print("Deleting Visual Studio projects...")
VS.Delete()

input("Press Enter to continue...")
