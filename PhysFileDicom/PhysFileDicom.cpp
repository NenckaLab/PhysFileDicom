#include "Command.h"
#include <unistd.h>
#include <fstream>

// Include this to avoid having to type fully qualified names
using namespace Physio;
using namespace std;

#define VERSION_MAJOR 1
#define VERSION_MINOR 0
#define VERSION_REVISION 0



vector<float> getFileData(boost::filesystem::path pt, char delimiter)
{
    //probably need some safety checks in here
    vector<float> result;
    //ifstream in(pt.string<string>());
    ifstream in;
    in.open(pt.string<string>());
    
    
    for (std::string f; getline(in, f, delimiter);)
    {
        try
        {
            result.push_back(std::stof(f));
        }
        catch(exception& e)
        {
            //do nothing with this item
        }
    }
    return result;
        
}

/**
 * Function that takes passed text files and encodes them into a dicom under the private tags.
 *
 *
 * @author Brad Swearingen, Medical College of Wisconsin, 2022
 *
 */
void Physio::EncodePhysioFiles(boost::filesystem::path filePathDicom, std::vector<boost::filesystem::path> textFiles){
    
    std::cout<<"Physical file encoding into dicom"<<endl;
    std::cout<<"Version "<< VERSION_MAJOR<<"."<< VERSION_MINOR<<"."<< VERSION_REVISION<<endl;
    
    std::cout<<"Loading dicom images"<<endl;
    // declare DicomDir object
    boost::shared_ptr<DicomDir> myDicomObject ;
    myDicomObject.reset(new DicomDir(filePathDicom));
    
    std::vector<std::string> datapointNames;
    std::vector<std::vector<float>> datapoints;
    //so, load each file from textfiles
    for(auto x: textFiles)
    {
        std::vector<float> dps;
        dps = getFileData(x,'\n');
        datapoints.push_back(dps);
        //get the "stem" -> filename without final extension
        string myname=x.stem().string();
        //std::string myname=x.stem().string<string>();
        std::string abbr = myname;
        //crude way of removing any time stamps - somewhat specific to MCW
        while(abbr[abbr.length() - 1] == '_' ||
              abbr[abbr.length() - 1] == '0' ||
              abbr[abbr.length() - 1] == '1' ||
              abbr[abbr.length() - 1] == '2' ||
              abbr[abbr.length() - 1] == '3' ||
              abbr[abbr.length() - 1] == '4' ||
              abbr[abbr.length() - 1] == '5' ||
              abbr[abbr.length() - 1] == '6' ||
              abbr[abbr.length() - 1] == '7' ||
              abbr[abbr.length() - 1] == '8' ||
              abbr[abbr.length() - 1] == '9'
              )
        {
            abbr.erase(abbr.length() - 1);
        }
        if(abbr.length() > 0)
        {
            myname = abbr;
        }
        datapointNames.push_back(myname);
    }
    
    
    //Call the function to create the new dicom
    myDicomObject->WriteVolume(datapointNames, datapoints, "PhysioData", myDicomObject->seriesNumber * 100 ,"PhysioData");
    
}
