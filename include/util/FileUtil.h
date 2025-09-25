#ifndef SN_SHORT_URL_SERVER_FILE_UTIL_H
#define SN_SHORT_URL_SERVER_FILE_UTIL_H

#include <string>
#include <vector>
#include <functional>

namespace sn {
namespace FileUtil {

extern std::string LoadFile(const std::string &file_path);
extern std::vector<std::string> LoadFileLines(const std::string &file_path, bool trim = false);
extern bool LoadFile(const std::string &file_path, std::string &content);
extern bool LoadFileLines(const std::string &file_path, std::vector<std::string> &lines, bool trim = false);
extern bool SaveFile(const std::string &file_path, const std::string &content);
extern bool SaveFileLines(const std::string &file_path, const std::vector<std::string> &lines);

extern bool IsPathExist(const std::string &path);
extern bool IsFileExist(const std::string &file);
extern bool IsFolderExist(const std::string &folder);

extern bool RemoveFile(const std::string &file);
extern bool RemoveFolder(const std::string &folder, bool recursive = true);
extern bool CreateFolder(const std::string &folder, bool recursive = true);
extern std::string GetParentFolder(const std::string &path);

extern std::pair<std::vector<std::string>, std::vector<std::string>> ListFolderFiles(const std::string &path,
    bool need_file = true, bool need_folder = true);

enum PathSearchResult {
    PATH_SEARCH_RLT_NEED = 0,
    PATH_SEARCH_RLT_IGNORE,
    PATH_SEARCH_RLT_NEED_BREAK,
    PATH_SEARCH_RLT_IGNORE_BREAK,
};
typedef std::function<PathSearchResult(const std::string &parent_path, const std::string &filename)> PathCheckFunc;
extern PathCheckFunc GenIgnoreFunc(const std::string &prefix, const std::string &suffix = "", bool break_need = false, bool break_ignore = false);
extern PathCheckFunc GenNeedFunc(const std::string &prefix, const std::string &suffix = "", bool break_need = false, bool break_ignore = false);
extern std::vector<std::string> GetAllFiles(const std::string &root_path, PathCheckFunc file_need_func, PathCheckFunc folder_enter_func);
extern std::vector<std::string> GetAllFolders(const std::string &root_path, PathCheckFunc file_need_parent_func, PathCheckFunc folder_enter_func);

} /* namespace FileUtil */
} /* namespace sn */

#endif // SN_SHORT_URL_SERVER_FILE_UTIL_H
