import os
import shutil

class SharedLib:
	binDirectory = "./Binaries"
	physXLibDirectory = "./Source/Vendor/PhysX/lib"

	@classmethod
	def SetupPhysX(cls):

		pConfig = [ "/debug", "/release" ]
		nConfig = [ "/Debug", "/Release", "/Dist" ]

		for i in range(len(nConfig)):
			
			binD = cls.binDirectory + nConfig[i]

			if i == 2:
				i = 1

			phxD = cls.physXLibDirectory + pConfig[i]

			if not os.path.exists(binD):
				os.makedirs(binD)

			files = os.listdir(phxD)

			for file in files:
				fp = os.path.splitext(file)	

				if fp[1] == ".dll" :
					src = os.path.abspath(phxD + "/" + file)
					shutil.copy(src,binD)

				
		
		print("Nvidia PhysX Setup Complete")

		return