#include "utilityFile.h"

#include "FilePath.h"
#include "FileSystem.h"

#include "utility.h"

std::vector<FilePath> utility::partitionFilePathsBySize(std::vector<FilePath> filePaths, int partitionCount)
{
	typedef std::pair<unsigned long long int, FilePath> PairType;
	std::vector<PairType> sourceFileSizesToCommands;
	for (const FilePath& path : filePaths)
	{
		if (path.exists())
		{
			sourceFileSizesToCommands.push_back(std::make_pair(FileSystem::getFileByteSize(path), path));
		}
		else
		{
			sourceFileSizesToCommands.push_back(std::make_pair(1, path));
		}
	}

	std::sort(sourceFileSizesToCommands.begin(), sourceFileSizesToCommands.end(), [](const PairType& p, const PairType& q) { return p.first > q.first; });

	if (0 < partitionCount && partitionCount < sourceFileSizesToCommands.size())
	{
		for (int i = 0; i < partitionCount; i++)
		{
			std::sort(
				sourceFileSizesToCommands.begin() + sourceFileSizesToCommands.size() * i / partitionCount,
				sourceFileSizesToCommands.begin() + sourceFileSizesToCommands.size() * (i + 1) / partitionCount,
				[](const PairType& p, const PairType& q) { return p.second.wstr() < q.second.wstr(); }
			);
		}
	}

	std::vector<FilePath> sortedFilePaths;
	for (const PairType &pair : sourceFileSizesToCommands)
	{
		sortedFilePaths.push_back(pair.second);
	}
	return sortedFilePaths;
}

std::vector<FilePath> utility::getTopLevelPaths(const std::vector<FilePath>& paths)
{
	return utility::getTopLevelPaths(utility::toSet(paths));
}

std::vector<FilePath> utility::getTopLevelPaths(const std::set<FilePath>& paths)
{
	// this works because the set contains the paths already in alphabetical order
	std::vector<FilePath> topLevelPaths;

	FilePath lastPath;
	for (const FilePath& path : paths)
	{
		if (lastPath.empty() || !lastPath.contains(path)) // don't add subdirectories of already added paths
		{
			lastPath = path;
			topLevelPaths.push_back(path);
		}
	}

	return topLevelPaths;
}

FilePath utility::getAsRelativeIfShorter(const FilePath& absolutePath, const FilePath& baseDirectory)
{
	if (!baseDirectory.empty())
	{
		const FilePath relativePath = absolutePath.getRelativeTo(baseDirectory);
		if (relativePath.wstr().size() < absolutePath.wstr().size())
		{
			return relativePath;
		}
	}
	return absolutePath;
}

std::vector<FilePath> utility::getAsRelativeIfShorter(const std::vector<FilePath>& absolutePaths, const FilePath& baseDirectory)
{
	return utility::convert<FilePath, FilePath>(absolutePaths, [&](const FilePath& path) { return getAsRelativeIfShorter(path, baseDirectory); });
}

