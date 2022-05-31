// Kevin Koch
// Medical College of Wisconsin
// December, 2015
// Gutted for my purposes here B. Swearingen, May 2022


#include "DicomDir.h"

//probably can get rid of some of this.
//#include "dcmtk/ofstd/ofstdinc.h"
//#include <dcmtk/dcmimgle/dcmimage.h>
#include <dcmtk/dcmdata/dctk.h>
//#include <dcmtk/dcmdata/dcfilefo.h>
//#include <dcmtk/dcmdata/dcdeftag.h>
//#include <dcmtk/dcmdata/dcuid.h>
//#include <dcmtk/dcmdata/dcdict.h>
//#include <dcmtk/dcmdata/dcvrfl.h>
//#include <dcmtk/dcmdata/dcdatset.h>

// Include this to avoid having to type fully qualified names
using namespace std;
using namespace Physio;
using namespace boost::filesystem;

DicomDir::DicomDir( const boost::filesystem::path dirPath){

    
    bool initLoop = false;
    bool initLoad = false;
    
    try  // for catching
    {
        if (exists(dirPath))    // does the directory actually exist?
        {
            // use an iterator so that we can sort the directory into the filePaths vector
            // filePaths is a vector of filename strings
            copy(directory_iterator(dirPath), directory_iterator(), back_inserter(filePaths));
            // sort the files
            sort(filePaths.begin(), filePaths.end()); // sort, since directory iteration
            numFiles = filePaths.size() ;
            
            DcmFileFormat fileformat;
            OFCondition status;
            vector<boost::filesystem::path>::iterator it = filePaths.begin();
            int counter = 0;
            long Rows;
            while(it != filePaths.end())
            {
                string s = it->string();
                status = fileformat.loadFile(s.c_str());
                if(status.good())
                {
                    status = fileformat.getDataset()->findAndGetLongInt(DCM_Rows, Rows);
                }
                //if either test fails, remove this filepath
                if(!status.good())
                {
                    it = filePaths.erase(it);
                }
                else
                {
                    if(!initLoad)
                    {
                        OFString sNumber ;
                        if (fileformat.getDataset()->findAndGetOFString(DCM_SeriesNumber,sNumber).good())
                        {
                            sscanf(sNumber.c_str(),"%d",&seriesNumber) ;
                        }
        
        
                        OFString sDesc ;
                        if (fileformat.getDataset()->findAndGetOFString(DCM_SeriesDescription,sDesc).good())
                        {
                            std::ostringstream sdBuffer ;
                            sdBuffer << sDesc ;
                            seriesDescription = sdBuffer.str() ;
                        }
                        
                        initLoad=true;
                    }
                    ++it;
                }

                counter++;
                printProgBar( (counter*100)/ numFiles );
            }
            printProgBar(100);
        }
    }

    catch (const filesystem_error& ex)
    {
        cout << ex.what() << '\n';
    }

    
}

void WritePrivateGroup(DcmDataset* ds, std::vector<float> dt, int group, std::string desc)
{
    
#define BASE_PRIVATE_ELEMENT 0x0010
#define PRIVATE_CREATOR_NAME "MCW"
    
    int pe = BASE_PRIVATE_ELEMENT;
    //adjust element if already occuppied
    while(pe < 0x00FF)
    {
        //std::cout<<"Checking pe "<<pe<<std::endl;
        if(ds->tagExists(DcmTag(group, pe)))
        {
            //cout<<"     Exists, incrementing"<<endl;
            pe++;
            continue;
        }
        else
        {
            break;
        }
    }
    if(pe == 0x00FF)
    {
        //TODO - throw an exception
        cout<<" Hit limit, ERROR"<<endl;
    }
    
    OFCondition res;
    std::string name = (std::string)PRIVATE_CREATOR_NAME + "_" + desc;
    cout<<"Inserting "<<name.c_str()<<endl; //<<" into "<<group<<","<<pe<<endl;
    res = ds->putAndInsertString(DcmTag(group, pe, EVR_LO), name.c_str());
    cout<<"      "<<res.text()<<endl;
    //basic case, need to adjust for longer arrays
    int offset = 0x00;
    
    
    //okay, maybe won't be too bad
    vector<string> reformat;
    string tmp = "";
    //std::ostringstream ss;
    for(auto x: dt)
    {
        //ss << x;
        //add to the string as long as the length isn't too long
        if(tmp=="")
        {
            tmp = boost::lexical_cast<std::string>(x);
        }
        else if(tmp.length() < 65000)  //should be okay to 65534, but why push it
        {
            tmp = tmp + "\\" + boost::lexical_cast<std::string>(x);
        }
        else
        {
            reformat.push_back(tmp);
            tmp=boost::lexical_cast<std::string>(x);
        }
    }
    //push the final string if there is one
    reformat.push_back(tmp);
    for(auto x: reformat)
    {
        res = ds->putAndInsertString(DcmTag(group, pe*0x100 + offset, EVR_DS), x.c_str());
        offset++;
        cout<<"      "<<res.text()<<endl;
    }
    
}

void DicomDir::WriteVolume(std::vector<std::string> names,
                           std::vector<std::vector<float>> data,
                           std::string outputDirName,
                           int seriesNumber,
                           std::string seriesDesc){
    
    // create the new directory, if necessary
    //boost::filesystem::create_directory(outputDirName.c_str()) ;
    create_directory(path(outputDirName));
    std::cout<<"Writing out "<<seriesDesc<<" dicoms."<<std::endl;

    OFCondition res;
    DcmElement* x;

    // build a series UID that will be used for all of the new files
    char uid[100];
    char * seriesUID = dcmGenerateUniqueIdentifier(uid,SITE_SERIES_UID_ROOT) ;

    // run through full set of files in directory
    // Only need first valid file
    int thisFile=0;
//    if(validFiles(thisFile)){   // safety check, in case we found no valid files
    //with changes above, we only retain filePaths that are valid
    if(true)
    {
        //Grab the template file for editting
        std::string s = filePaths.at(thisFile).string() ;
        const char* c = s.c_str();
        DcmFileFormat fileformat;
        OFCondition status = fileformat.loadFile(c);
        DcmDataset* ds = fileformat.getDataset();

        //create new UIDs
        char SOPuid[100];
        char * SOPUID = dcmGenerateUniqueIdentifier(SOPuid,SITE_SERIES_UID_ROOT) ;
        res = fileformat.getMetaInfo()->findAndGetElement(DcmTagKey(0x0002,0x0003),x);
        res = x->putString(SOPUID);
        res = ds->findAndGetElement(DcmTagKey(0x0008,0x0018),x);
        res = x->putString(SOPUID);
        // modify the series UID tag
        res = ds->findAndGetElement(DcmTagKey(0x0020,0x000E),x);
        res = x->putString(seriesUID);
        //std::cout<<"Series UID Write: "<<res.text()<<std::endl;
        

        
        // write the new series description tag
        //fileformat.getDataset()->putAndInsertString(DcmTagKey(0x0008,0x103E), seriesDesc.c_str());
        res = ds->findAndGetElement(DcmTagKey(0x0008,0x103E),x);
        res = x->putString(seriesDesc.c_str());
        //std::cout<<"Series Desc Write: "<<res.text()<<std::endl;
        //std::cout<<"actual DcmElement data type: "<<typeid(x).name()<<std::endl;


        // build a buffer for the series number, write the number to the series number tag
        std::ostringstream seriesBuffer ;
        seriesBuffer << seriesNumber ;
        //fileformat.getDataset()->putAndInsertString(DcmTagKey(0x0020,0x0011), seriesBuffer.str().c_str(), OFTrue);
        res = ds->findAndGetElement(DcmTagKey(0x0020,0x0011),x);
        res = x->putString(seriesBuffer.str().c_str());
        //std::cout<<"Series Number Write: "<<res.text()<<std::endl;

        //update acquisition number
        //don't think I need to update this
        //res = ds->findAndGetElement(DcmTagKey(0x0020,0x0012),x);
        
        //instance - change this to 1
        res = ds->findAndGetElement(DcmTagKey(0x0020,0x0013),x);
        res = x->putString("1");
        
        //images in acquisition - change this to 1
        res = ds->findAndGetElement(DcmTagKey(0x0020,0x1002),x);
        res = x->putString("1");
        
        //Write the private groups
#define PRIVATE_GROUP 0x0033
        for(int x = 0; x < names.size(); x++)
        {
            //std::cout<<"Location message: "<<x<<std::endl;
            std::string nm = names.at(x);
            std::vector<float> dt = data.at(x);
            WritePrivateGroup(ds, dt, PRIVATE_GROUP, nm);
            
        }

        // build the filename
        std::ostringstream fileNameBuffer ;
        fileNameBuffer << outputDirName << "/IMG" << 0 << ".dcm" ;
        fileformat.saveFile(fileNameBuffer.str().c_str(), EXS_LittleEndianExplicit);

    }
}

void DicomDir::printProgBar( int percent ){
    std::string bar;
    
    for(int i = 0; i < 50; i++){
        if( i < (percent/2)){
            bar.replace(i,1,"=");
        }else if( i == (percent/2)){
            bar.replace(i,1,">");
        }else{
            bar.replace(i,1," ");
        }
    }
    
    std::cout<< "\r" "[" << bar << "] ";
    std::cout.width( 3 );
    std::cout<< percent << "%     " << std::flush;
    if(percent >= 100){
    	std::cout << "Files loaded" << std::endl;
    }
}


