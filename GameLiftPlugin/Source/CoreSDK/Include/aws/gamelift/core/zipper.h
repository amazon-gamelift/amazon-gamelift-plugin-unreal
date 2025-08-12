// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.

#pragma once

// GameLift
#include <aws/gamelift/core/api.h>

namespace GameLift
{
    static const int MAX_ZIPPER_FILE_SIZE_IN_BYTES = 500000;
    // Wrapper for public-domain miniz ZIP writer - see miniz.inc
    // NOTE: filenames and paths use UTF-8 encoding on all platforms
    class GAMELIFT_API Zipper
    {
    private:
        void* m_zipFile;
        std::string m_sourcePath;
        FuncLogCallback m_logCb = nullptr;
        std::set<std::string> m_addedFiles;  // Keep track of files we've added

        void SetSourcePath(const std::string& sourcePath);
        const std::string& GetSourcePath() const;

    public:
        Zipper(const std::string& sourcePath, const std::string& zipFileName, FuncLogCallback logCb);
        ~Zipper();

        bool AddDirectoryToZipFile(const std::string& directoryPath);
        bool AddDirectoryToZipFile(const std::string& directoryPath, const std::string& archivePath);
        bool AddFileToZipFile(const std::string& fileName, const std::string& archivePath);
        bool AddFileToZipFile(const std::string& fileName);        
        bool CloseZipFile();
        static void NormalizePathInZip(std::string& inOutPathInZip, const std::string& relativeSoucePath);
    };
}
