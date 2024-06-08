#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <string>
#include <ctime>
#include <algorithm>
#include <thread>
#include <chrono>
#include <iomanip>
#include <ctime>

const int BLOCK_SIZE = 1024;  // Each block is 1KB
const int TOTAL_BLOCKS = 16384;  // Total blocks (16MB total memory)
char storage[TOTAL_BLOCKS * BLOCK_SIZE];  // Simulated disk space

struct INode {
    bool isDirectory;
    int size;  // Size in bytes
    std::time_t created;
    int directBlocks[10];
    int usedBlocks;  // Number of used blocks

    INode() : isDirectory(false), size(0), created(std::time(nullptr)), usedBlocks(0) {
        std::fill(std::begin(directBlocks), std::end(directBlocks), -1);
    }
};

class FileSystem {
public:
    FileSystem();
    ~FileSystem();
    void run();
    void cat(const std::string& path); 

private:
    std::unordered_map<std::string, INode> inodes;
    std::string currentDir;

    void initialize();
    void printWelcomeScreen();
    void printLoadingScreen();
    void handleCommands();
    void createFile(const std::string& path, int size);
    void deleteFile(const std::string& path);
    void createDir(const std::string& path);
    void deleteDir(const std::string& path);
    void changeDir(const std::string& path);
    void listDir() const;
    void copyFile(const std::string& srcPath, const std::string& dstPath);
    void displayStorageUsage() const;
    void printHelp() const;
    void saveToDisk(const std::string& filename);
    void loadFromDisk(const std::string& filename);
    void clearMemory();
    int allocateBlock();
    void releaseBlock(int blockNum);
    bool pathExists(const std::string& path) const;
};

FileSystem::FileSystem() : currentDir("/") {
    inodes["/"] = INode();
    inodes["/"].isDirectory = true;
}

FileSystem::~FileSystem() {
    std::cout << "Shutting down the file system..." << std::endl;
    saveToDisk("filesystem.dat");
}

int FileSystem::allocateBlock() {
    for (int i = 0; i < TOTAL_BLOCKS; ++i) {
        if (storage[i * BLOCK_SIZE] == 0) {
            storage[i * BLOCK_SIZE] = 1;  // Mark block as used
            return i;
        }
    }
    return -1;  // No available blocks
}

void FileSystem::releaseBlock(int blockNum) {
    if (blockNum >= 0 && blockNum < TOTAL_BLOCKS) {
        storage[blockNum * BLOCK_SIZE] = 0;  // Mark block as free
    }
}

void FileSystem::initialize() {
    printLoadingScreen();
    loadFromDisk("filesystem.dat");
    printWelcomeScreen();
}

void FileSystem::printWelcomeScreen() {
    std::cout << "Welcome to the Unix-style file system!" << std::endl;
    std::cout << "This is a virtual file system implemented in C++." << std::endl;
    std::cout << "-----------------------------------------#########---------------------------------------------" << std::endl;
    std::cout <<                                 "Developed by [Sudia Sotra Dhar] "                                << std::endl;
     std::cout << "-----------------------------------------#########--------------------------------------------" << std::endl;
}

void FileSystem::printLoadingScreen() {
    const int totalDuration = 10; // Loading duration in seconds
    const int steps = 100; // Number of steps in the loading bar
    int durationPerStep = totalDuration * 1000 / steps; // Duration per step in milliseconds

    std::cout << "Initializing file system, please wait..." << std::endl;
    std::cout << "[";
    for (int i = 0; i <= steps; i++) {
        std::this_thread::sleep_for(std::chrono::milliseconds(durationPerStep));
        std::cout << "=" << std::flush;
        if (i % (steps / 10) == 0) {  // Update percentage every 10%
            std::cout << "] " << i << "%" << "\r[";
        }
    }
    std::cout << "] 100%\nLoading complete!\n\n";
}

void FileSystem::run() {
    initialize();
    handleCommands();
}

void FileSystem::handleCommands() {
    std::string command;
    std::string input;
    std::string path;
    int size;
    

    while (true) {
        std::cout << "FileSystem> ";
        std::getline(std::cin, input);  // Read the whole line of input
        std::istringstream iss(input);
        iss >> command;  // Extract the command

        if (command == "exit") {
            break;
        } else if (command == "createFile") {
            iss >> path >> size;  // Extract path and size
            if (!iss.fail()) {
                createFile(path, size);
            } else {
                std::cout << "Invalid arguments for createFile." << std::endl;
            }
        } else if (command == "deleteFile") {
            iss >> path;  // Extract path
            deleteFile(path);
        } else if (command == "createDir") {
            path = iss.str().substr(command.length()); // Extract remaining input as path
            std::string trimmed = path.erase(0, path.find_first_not_of(" \t\n\r\f\v")); // Trim leading spaces
            createDir(trimmed);
        } else if (command == "deleteDir") {
            iss >> path;  // Extract path
            deleteDir(path);
        } else if (command == "changeDir") {
            iss >> path;  // Extract path
            changeDir(path);
        } else if (command == "listDir") {
            listDir();
        } else if (command == "cp") {
            std::string srcPath, dstPath;
            iss >> srcPath >> dstPath;  // Extract source and destination paths
            copyFile(srcPath, dstPath);
        } else if (command == "sum") {
            displayStorageUsage();
        } else if (command == "cat") {
            iss >> path;  // Extract path
            cat(path);
        } else if (command == "help") {
            printHelp();
        } else {
            std::cout << "Unknown command. Type 'help' for a list of commands." << std::endl;
        }
    }
}
void FileSystem::createFile(const std::string& path, int size) {
    // Fully qualify the path if not already
    std::string fullPath = (path.front() == '/') ? path : currentDir + "/" + path;
    
    if (pathExists(fullPath)) {
        std::cout << "Error: File already exists." << std::endl;
        return;
    }

    if (size <= 0 || size > BLOCK_SIZE * 10) {
        std::cout << "Error: Invalid file size. Must be between 1 and 10240 KB." << std::endl;
        return;
    }

    INode node;
    node.size = size;
    int blocksNeeded = (size + BLOCK_SIZE - 1) / BLOCK_SIZE;

    for (int i = 0; i < blocksNeeded; i++) {
        int blockNum = allocateBlock();
        if (blockNum == -1) {
            std::cout << "Error: Insufficient disk space." << std::endl;
            for (int j = 0; j < i; j++) {
                releaseBlock(node.directBlocks[j]);
            }
            return;
        }
        node.directBlocks[i] = blockNum;
    }

    node.usedBlocks = blocksNeeded;
    inodes[fullPath] = node;
    std::cout << "File created: " << fullPath << std::endl;
}

void FileSystem::deleteFile(const std::string& path) {
    std::string fullPath = (path.front() == '/') ? path : currentDir + "/" + path;
    auto it = inodes.find(fullPath);
    if (it == inodes.end() || it->second.isDirectory) {
        std::cout << "Error: File does not exist or is a directory." << std::endl;
        return;
    }

    for (int i = 0; i < it->second.usedBlocks; i++) {
        releaseBlock(it->second.directBlocks[i]);
    }

    inodes.erase(it);
    std::cout << "File deleted: " << fullPath << std::endl;
}

void FileSystem::createDir(const std::string& path) {
    // Construct full path relative to current directory if not an absolute path
    std::string fullPath = path;
    if (path.front() != '/') {
        fullPath = currentDir + "/" + path;
    }

    if (pathExists(fullPath)) {
        std::cout << "Error: Directory already exists." << std::endl;
        return;
    }

    INode dirNode;
    dirNode.isDirectory = true;
    dirNode.created = std::time(nullptr);  // Set current time as creation time
    inodes[fullPath] = dirNode;
    std::cout << "Directory created: " << fullPath << std::endl;
}

void FileSystem::deleteDir(const std::string& path) {
    std::string fullPath = (path.front() == '/') ? path : currentDir + "/" + path;

    if (fullPath == currentDir || fullPath == "/") {
        std::cout << "Error: Cannot delete the current or root directory." << std::endl;
        return;
    }

    auto it = inodes.find(fullPath);
    if (it == inodes.end() || !it->second.isDirectory) {
        std::cout << "Error: Directory does not exist or is not a directory." << std::endl;
        return;
    }

    // Check if directory is empty
    bool isEmpty = true;
    for (const auto& inode : inodes) {
        if (inode.first != fullPath && inode.first.find(fullPath + "/") == 0) {
            isEmpty = false;
            break;
        }
    }

    if (!isEmpty) {
        std::cout << "Error: Directory is not empty." << std::endl;
        return;
    }

    inodes.erase(it);
    std::cout << "Directory deleted: " << fullPath << std::endl;
}

void FileSystem::changeDir(const std::string& path) {
    // Check if the specified path exists and is a directory
    if (pathExists(path) && inodes[path].isDirectory) {
        currentDir = path;  // Change the current directory to the specified path
        std::cout << "Current directory changed to: " << currentDir << std::endl;
    } else {
        std::cout << "Error: Directory does not exist." << std::endl;
    }
}

void FileSystem::listDir() const {
    std::cout << "Contents of directory: " << currentDir << std::endl;

    for (const auto& inode : inodes) {
        if (inode.first.find(currentDir + "/") == 0 || inode.first == currentDir) {
            // Skip the current directory entry itself
            if (inode.first == currentDir) continue;

            // Format the creation time
            std::time_t creationTime = inode.second.created;
            std::string timeStr = std::asctime(std::localtime(&creationTime));
            timeStr.pop_back();  // Remove the newline character at the end of the asctime result

            // Print directory or file with its creation time
            if (inode.second.isDirectory) {
                std::cout << "[D] " << inode.first.substr(currentDir.length() + 1) << " - Created: " << timeStr << std::endl;
            } else {
                std::cout << "[F] " << inode.first.substr(currentDir.length() + 1) << " - Size: " << inode.second.size << " bytes - Created: " << timeStr << std::endl;
            }
        }
    }
}
void FileSystem::copyFile(const std::string& srcPath, const std::string& dstPath) {
    std::string fullSrcPath = (srcPath.front() == '/') ? srcPath : currentDir + "/" + srcPath;
    std::string fullDstPath = (dstPath.front() == '/') ? dstPath : currentDir + "/" + dstPath;

    auto srcIt = inodes.find(fullSrcPath);
    if (srcIt == inodes.end() || srcIt->second.isDirectory) {
        std::cout << "Error: Source file does not exist or is a directory." << std::endl;
        return;
    }

    if (pathExists(fullDstPath)) {
        std::cout << "Error: Destination file already exists." << std::endl;
        return;
    }

    INode newNode = srcIt->second;
    std::fill(std::begin(newNode.directBlocks), std::end(newNode.directBlocks), -1);

    for (int i = 0; i < srcIt->second.usedBlocks; i++) {
        int newBlock = allocateBlock();
        if (newBlock == -1) {
            std::cout << "Error: Insufficient disk space to copy file." << std::endl;
            for (int j = 0; j < i; j++) {
                releaseBlock(newNode.directBlocks[j]);
            }
            return;
        }
        newNode.directBlocks[i] = newBlock;
        std::copy(storage + srcIt->second.directBlocks[i] * BLOCK_SIZE,
                  storage + (srcIt->second.directBlocks[i] + 1) * BLOCK_SIZE,
                  storage + newBlock * BLOCK_SIZE);
    }

    inodes[fullDstPath] = newNode;
    std::cout << "File copied from " << fullSrcPath << " to " << fullDstPath << std::endl;
}

void FileSystem::displayStorageUsage() const {
    int usedBlocks = 0;
    for (int i = 0; i < TOTAL_BLOCKS; i++) {
        if (storage[i * BLOCK_SIZE] == 1) {
            usedBlocks++;
        }
    }
    std::cout << "Total Blocks: " << TOTAL_BLOCKS << std::endl;
    std::cout << "Used Blocks: " << usedBlocks << std::endl;
    std::cout << "Free Blocks: " << (TOTAL_BLOCKS - usedBlocks) << std::endl;
}

void FileSystem::cat(const std::string& path) {
    std::string fullPath = (path.front() == '/') ? path : currentDir + "/" + path;
    auto it = inodes.find(fullPath);
    if (it == inodes.end() || it->second.isDirectory) {
        std::cout << "Error: File does not exist or is a directory." << std::endl;
        return;
    }

    const INode& inode = it->second;
    std::cout << "Contents of " << fullPath << ":\n";
    for (int i = 0; i < inode.usedBlocks; i++) {
        int blockNum = inode.directBlocks[i];
        int sizeToRead = std::min(BLOCK_SIZE, inode.size - i * BLOCK_SIZE);
        std::cout.write(storage + blockNum * BLOCK_SIZE, sizeToRead);
    }
    std::cout << std::endl;
}

void FileSystem::printHelp() const {
    std::cout << "Commands available:" << std::endl;
    std::cout << "createFile <path> <size> - Create a new file" << std::endl;
    std::cout << "deleteFile <path> - Delete a file" << std::endl;
    std::cout << "createDir <path> - Create a new directory" << std::endl;
    std::cout << "deleteDir <path> - Delete a directory" << std::endl;
    std::cout << "changeDir <path> - Change the current directory" << std::endl;
    std::cout << "listDir - List directory contents" << std::endl;
    std::cout << "cp <source> <destination> - Copy a file" << std::endl;
    std::cout << "sum - Display storage usage" << std::endl;
    std::cout << "cat <path> - Display the contents of a file" << std::endl;
    std::cout << "help - Display this help message" << std::endl;
    std::cout << "exit - Exit the program" << std::endl;
}

void FileSystem::saveToDisk(const std::string& filename) {
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cout << "Failed to open file for saving." << std::endl;
        return;
    }

    // Save inodes
    size_t inodes_size = inodes.size();
    file.write(reinterpret_cast<const char*>(&inodes_size), sizeof(inodes_size));
    for (const auto& inode_pair : inodes) {
        size_t path_size = inode_pair.first.size();
        file.write(reinterpret_cast<const char*>(&path_size), sizeof(path_size));
        file.write(inode_pair.first.c_str(), path_size);
        const INode& inode = inode_pair.second;
        file.write(reinterpret_cast<const char*>(&inode_pair.second), sizeof(INode));
    }

    // Save storage
    file.write(storage, sizeof(storage));
    file.close();
    std::cout << "File system saved." << std::endl;
}

void FileSystem::loadFromDisk(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cout << "Failed to open file for loading." << std::endl;
        return;
    }

    clearMemory();

    // Load inodes
    size_t inodes_size;
    file.read(reinterpret_cast<char*>(&inodes_size), sizeof(inodes_size));
    for (size_t i = 0; i < inodes_size; i++) {
        size_t path_size;
        file.read(reinterpret_cast<char*>(&path_size), sizeof(path_size));
        std::string path(path_size, '\0');
        file.read(&path[0], path_size);
        INode inode;
        file.read(reinterpret_cast<char*>(&inode), sizeof(INode));
        inodes[path] = inode;
    }

    // Load storage
    file.read(storage, sizeof(storage));
    file.close();
    std::cout << "File system loaded." << std::endl;
}

void FileSystem::clearMemory() {
    inodes.clear();
    std::fill(std::begin(storage), std::end(storage), 0);
}

bool FileSystem::pathExists(const std::string& path) const {
    return inodes.find(path) != inodes.end();
}

int main() {
    FileSystem fs;
    fs.run();
    return 0;
}
