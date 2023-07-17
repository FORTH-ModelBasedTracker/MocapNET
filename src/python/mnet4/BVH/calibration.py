#This mimics the calibration files like ;
# https://github.com/AmmarkoV/RGBDAcquisition/blob/master/tools/Calibration/calibration.c

#This mimics the calibration files like ;
# https://github.com/AmmarkoV/RGBDAcquisition/blob/master/tools/Calibration/calibration.c
def readCalibrationFromFile(filename):
    calib = dict()
    if filename is None:
        return calib

    fp = None
    try:
        fp = open(filename, "r")
    except IOError:
        return calib

    # Our state
    # ----------------------------
    i = 0
    category = 0
    line_length = 0
    lines_at_current_category = 0
    # ----------------------------


    for line in fp:
        #--------------------------------------
        line = line.rstrip("\r\n")
        line_length = len(line)
        #--------------------------------------
        if line_length > 0:
            if line[line_length - 1] == '\n':
                line = line[:-1]
            if line[line_length - 1] == '\r':
                line = line[:-1]
        #--------------------------------------
        if line_length > 1:
            if line[line_length - 2] == '\n':
                line = line[:-2]
            if line[line_length - 2] == '\r':
                line = line[:-2]
        #--------------------------------------
        if line[0] == '%':
            lines_at_current_category = 0
        #--------------------------------------
        # ---------------------------- ---------------------------- ----------------------------  
        if line == "%I":
            category = 1
            calib["intrinsic"] = list()
        elif line == "%D":
            category = 2
        elif line == "%T":
            category = 3
            calib["extrinsicTranslation"] = list()
        elif line == "%R":
            category = 4
            calib["extrinsicRotationRodriguez"] = list()
        elif line == "%NF":
            category = 5
        elif line == "%UNIT":
            category = 6
        elif line == "%RT4*4":
            category = 7
            calib["extrinsic"] = list()
        elif line == "%Width":
            category = 8
        elif line == "%Height":
            category = 9
        else:
        # ---------------------------- ---------------------------- ----------------------------  
            if category == 1:
                calib["intrinsicParametersSet"] = 1
                lines_at_current_category = min(lines_at_current_category, 9)
                calib["intrinsic"].append(float(line))
                lines_at_current_category += 1
                if (lines_at_current_category==9):
                                                   category = 0
            elif category == 2:
                if lines_at_current_category == 0:
                    calib["k1"] = float(line)
                elif lines_at_current_category == 1:
                    calib["k2"] = float(line)
                elif lines_at_current_category == 2:
                    calib["p1"] = float(line)
                elif lines_at_current_category == 3:
                    calib["p2"] = float(line)
                elif lines_at_current_category == 4:
                    calib["k3"] = float(line)
                lines_at_current_category += 1
                if (lines_at_current_category==4):
                                                   category = 0
            elif category == 3:
                calib["extrinsicParametersSet"] = 1
                lines_at_current_category = min(lines_at_current_category, 3)
                calib["extrinsicTranslation"].append(float(line))
                lines_at_current_category += 1
                if (lines_at_current_category==3):
                                                   category = 0
            elif category == 4:
                lines_at_current_category = min(lines_at_current_category, 3)
                calib["extrinsicRotationRodriguez"].append(float(line))
                lines_at_current_category += 1
                if (lines_at_current_category==3):
                                                   category = 0
            elif category == 5:
                calib["nearPlane"] = float(line)
                category = 0
            elif category == 6:
                calib["farPlane"] = float(line)
                category = 0
            elif category == 7:
                lines_at_current_category = min(lines_at_current_category, 16)
                calib["extrinsic"].append(float(line))
                lines_at_current_category += 1
                category = 0
            elif category == 8:
                    calib["width"] = int(line)
                    category = 0
            elif category == 9:
                    calib["height"] = int(line)
                    category = 0
        # ---------------------------- ---------------------------- ----------------------------  

    fp.close()

    try:
        calib["fX"] = calib["intrinsic"][0]
        calib["fY"] = calib["intrinsic"][4]
        calib["cX"] = calib["intrinsic"][2]
        calib["cY"] = calib["intrinsic"][5] 
    except:
        print("No intrinsic matrix declared in ", filename)
        print("Cannot populate fX, fY, cX, cY")
   
  
    print("New calibration loaded : ",calib)

    return calib

