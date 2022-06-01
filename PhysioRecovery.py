import pydicom
import glob
import numpy as np
import argparse
from decimal import Decimal
pydicom.config.use_DS_numpy=True

args=[]

def parseArgs():
    global args
    parser = argparse.ArgumentParser(description='Argument parser for retrieving data from private tags')
    parser.add_argument('-f','--File',help="The file to recover the physio files from",required=True)
    args=parser.parse_args()
    return

def buildPrivateData(ds,group,element):
    x=0
    mydata=[]
    try:
        while (group,element + x) in ds:
            print("    {}   {}".format(hex(group),hex(element + x)))
            mv = ds[group,element+x].value
            if(type(mv) != np.ndarray):
                if(mv == None):
                    print("Empty data")
                    break
            
            for y in mv:
                s=str(y)
                mydata.append(s.rstrip('0').rstrip('.') if '.' in s else s)
            x += 1
    except:
        print("issue")
    return mydata

def outputPrivateGroup(ds,group,element):
    name = ds[group,element].value.replace("MCW_","")
    print("    {}".format(name))
    #open a file named name
    child=element*0x100
    mydata=buildPrivateData(ds,group,child)
    print(len(mydata))
    print("\n")
    with open(name,"w") as f:
        f.write("\n".join(mydata))
            
def extractPrivateGroup(ds,base=0x33): #, myData, group, desc):
    offset=0x10
    while (base,offset) in ds:
        print("{}   {}".format(hex(base),hex(offset)))
        outputPrivateGroup(ds,base,offset)
        offset += 1

def main():
     global args
     parseArgs()
     ds=pydicom.read_file(args.File)              
     extractPrivateGroup(ds)

#MAIN
if __name__ == '__main__':
    main()

