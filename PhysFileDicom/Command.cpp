#include <boost/program_options.hpp>
#include <boost/make_shared.hpp>
#include "Command.h"

// Include this to avoid having to type fully qualified names
using namespace Physio;
using namespace std;
namespace po = boost::program_options;
namespace fs = boost::filesystem;


bool ProcessCommandLine(const int argc, const char* const argv[], boost::filesystem::path& dicoms, std::vector<boost::filesystem::path>& files)
{
    po::options_description desc("Options");
    desc.add_options()
        ("help,h", "produce help message")
        ("dicoms,d", po::value<string>(), "Specify directory with seed Dicoms")
    ("text,t", po::value<vector<string>>()->multitoken(), "Specify the text files to encode");
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc),vm);
    po::notify(vm);
    
    if(vm.count("help"))
    {
        std::cout<<"Usage: options_description [options]\n";
        std::cout<<desc;
        return 1;
    }
    
    if(vm.count("dicoms") != 1)
    {
        cout<<"\n--dicom flag must be specified!\n"<<endl;
        std::cout<<"Usage: options_description [options]\n";
        std::cout<<desc;
        return 1;
    }
    
    if(vm.count("text") != 1)
    {
        cout<<"\nat least one(1) --text flag must be specified!\n"<<endl;
        std::cout<<"Usage: options_description [options]\n";
        std::cout<<desc;
        return 1;
    }

    //dicoms.assign(vm["dicoms"].as<string>());
    dicoms = fs::path(vm["dicoms"].as<string>());
    
    for(auto x: vm["text"].as<vector<string>>())
    {
        fs::path tmp2(x);
        files.push_back(tmp2);
    }
    return 0;
}

/********************
 ** Main function  **
 ********************/
int main(const int argc, const char* const argv[])
{
    fs::path dicoms;
    //(vm["dicoms"].as<string>());
    //dicoms->assign(vm["dicoms"].as<string>());
    //dicoms = fs::path(vm["dicoms"].as<string>());
    vector<fs::path> files;
    
    if(ProcessCommandLine(argc,argv,dicoms,files))
    {
        return 1;
    }
    
    std::cout<<"\nDicom Directory: "<<dicoms<<std::endl;
    std::cout<<"Attempting to process the following files:"<<std::endl;
    for(auto x: files)
    {
        std::cout<<"    "<<x<<std::endl;
    }
    Physio::EncodePhysioFiles(dicoms, files);
}
