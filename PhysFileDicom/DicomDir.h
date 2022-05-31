
#pragma once

#include <string>
#include <iostream>

#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>

#include <stdio.h>
#include <vector>



namespace Physio
{
        class DicomDir
        {
        public:
            /**
             * Constructor
             */
            DicomDir(const boost::filesystem::path dirPath);
            
            
            // Writing method
            void WriteVolume(std::vector<std::string> names,
                             std::vector<std::vector<float>> data,
                                std::string outputDirName,
                                int seriesNumber,
                                std::string seriesDesc);
            
            boost::filesystem::path thisDirPath ;   // the directory path  (from the constructor)
            
            typedef std::vector<boost::filesystem::path> vec;
            vec filePaths;  // vector of filenames in the directory
            
            int numFiles ;  // number of files in the directory
            
            int seriesNumber ;
            std::string seriesDescription ;
            
            
        private:
            void printProgBar( int percent ) ;
            
        } ;
    
   
}
