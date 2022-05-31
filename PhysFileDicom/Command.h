
#pragma once

#include <boost/shared_ptr.hpp>
#include "DicomDir.h"

/**
 *
 * @author B. Swearingen
 */

namespace Physio
{
    
    
    void EncodePhysioFiles(boost::filesystem::path filePathDicom, std::vector<boost::filesystem::path> textFiles) ;
}
