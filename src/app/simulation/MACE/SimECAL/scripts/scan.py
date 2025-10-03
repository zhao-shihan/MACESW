import yaml

nPoint = 0
for i in range(10, 20):
    for j in range(15, 25):
        nPoint += 1

        test = {
            "ECAL": {
                "InnerRadius": j * 10,
                "CrystalHypotenuse": i * 10,
                "UpstreamWindowRadius": 0,
                "DownstreamWindowRadius": 0,
                "fNSubdivision": 2,
            }
        }
        name = "test3_{}L_{}R".format(i, j)

        with open(name + ".yaml", "w") as f:
            yaml.safe_dump(test, f)

        # 生成.mac文件名和yaml文件名
        mac_filename = name + ".mac"
        yaml_filename = name + ".yaml"

        # 写入mac文件内容
        with open(mac_filename, "w") as f:
            f.write(f"""/control/verbose 0
//run/verbose 0
/event/verbose 0
/tracking/verbose 0

#/MACE/Physics/UseOpticalPhysics
/Mustard/Detector/Description/Import {yaml_filename}

/run/initialize

/MACE/Generator/SwitchToGPSX

/gps/particle e-
/gps/ene/mono 53 MeV
/gps/pos/centre 0. 0. 0. cm
/gps/direction -0.231259 0.883817 -0.386541

/MACE/Analysis/FilePath {name}.root
/MACE/Analysis/FileMode RECREATE
/run/beamOn 1000000
/MPIRun/PrintRunSummary
""")

print(f"{nPoint} points have been generated. Enjoy :)")
