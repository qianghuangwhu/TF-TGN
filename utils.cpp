#include "utils.h"

bool createDirectory(const std::string& path) {
    // Attempt to create the directory.
    int result = mkdir(path.c_str(), 0777);

    // Check if the directory was created successfully.
    if (result == 0) {
        std::cout << "Directory created successfully." << std::endl;
        return true; 
    } else if (errno == EEXIST) {
        // If the failure was because the directory already exists.
        std::cout << "Directory already exists." << std::endl;
        return true; // This is true in the sense that the desired end state (directory exists) is true.
    } else {
        // If directory creation failed for a reason other than it already existing.
        std::cerr << "Failed to create directory: " << strerror(errno) << std::endl;
        return false;
    }
}

string extractSecondLastFolder(const string &filePath) {
    size_t lastSlashPos = filePath.rfind('/'); // Find the last slash
    // Handle cases where there is no slash or only a single leading slash, indicating no parent directories
    if (lastSlashPos == string::npos || lastSlashPos == 0) {
        return "";
    }

    size_t secondLastSlashPos = filePath.rfind('/', lastSlashPos - 1); // Find the second last slash
    if (secondLastSlashPos == string::npos) {
        return "";
    }

    // Extract the folder name between the second last slash and the last slash
    string folder = filePath.substr(secondLastSlashPos + 1, lastSlashPos - secondLastSlashPos - 1);

    return folder;
}