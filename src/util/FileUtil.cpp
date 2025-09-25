#include "util/FileUtil.h"

#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

#include <string>
#include <vector>
#include <fstream>
#include <functional>

#include <Poco/Path.h>
#include <Poco/File.h>

using std::string;
using std::vector;
using std::function;
using namespace sn::FileUtil;

namespace sn {
namespace FileUtil{

std::string LoadFile(const std::string &file_path) {
    std::string content;

    std::ifstream fp(file_path);
    if (!fp)
        return content;
    string str;
    while (getline(fp, str)) {
        content += str;
        content += "\n";
    }
    fp.close();

    return content;
}

vector<string> LoadFileLines(const string &file_path, bool trim) {
    vector<string> lines;
    LoadFileLines(file_path, lines, trim);
    return lines;
}

bool LoadFile(const string &file_path, string &content) {
    std::ifstream fp(file_path);
    if (!fp)
        return false;
    content.clear();
    string str;
    while (getline(fp, str)) {
        content += str;
        content += "\n";
    }
    fp.close();

    return !content.empty();
}

bool LoadFileLines(const string &file_path, vector<string> &lines, bool trim) {
    lines.clear();

    std::fstream fp(file_path);
    if (!fp)
        return false;
    std::string trim_chrs = " \t\n\r";
    string str;
    // 按行读取,遇到换行符结束
    while (getline(fp, str)) {
        if (trim) {
            int sub_beg = 0, sub_end = str.size();
            while (sub_beg < sub_end) {
                if (trim_chrs.find(str[sub_beg]) == trim_chrs.npos)
                    break;
                ++sub_beg;
            }
            while (sub_beg < sub_end) {
                if (trim_chrs.find(str[sub_end - 1]) == trim_chrs.npos)
                    break;
                --sub_end;
            }
            str = str.substr(sub_beg, sub_end - sub_beg);
        }

        lines.emplace_back(std::move(str));
        str.clear();
    }
    fp.close();

    return !lines.empty();
}

bool SaveFile(const string &file_path, const string &content) {
    std::ofstream fp(file_path);
    if (!fp)
        return false;
    fp << content;
    bool ret = !!fp;
    fp.close();
    return ret;
}

bool SaveFileLines(const std::string &file_path, const std::vector<std::string> &lines) {
    std::ofstream fp(file_path);
    if (!fp)
        return false;
    for (auto &line : lines) {
        fp << line << "\n";
    }
    bool ret = !!fp;
    fp.close();
    return ret;
}

bool IsPathExist(const string &path) {
    if (path.empty())
        return false;
    Poco::File poco_file(path);
    return poco_file.exists();
}

bool IsFileExist(const string &file) {
    if (file.empty())
        return false;
    Poco::File poco_file(file);
    return poco_file.exists() && !poco_file.isDirectory();
}

bool IsFolderExist(const string &folder) {
    if (folder.empty())
        return false;
    Poco::File poco_folder(folder);
    return poco_folder.exists() && poco_folder.isDirectory();
}

bool RemoveFile(const string &file, bool recursive) {
    if (file.empty())
        return false;
    Poco::File poco_file(file);
    if (poco_file.exists())
        poco_file.remove(recursive);
    return !poco_file.exists();
}

bool RemoveFolder(const std::string &folder, bool recursive) {
    if (folder.empty())
        return false;
    Poco::File poco_folder(folder);
    if (poco_folder.exists())
        poco_folder.remove(recursive);
    return !poco_folder.exists();
}

bool CreateFolder(const string &folder, bool recursive) {
    if (folder.empty())
        return false;
    Poco::File poco_folder(folder);
    if (poco_folder.exists())
        return true;
    if (recursive) {
        if (!CreateFolder(GetParentFolder(folder), recursive))
            return false;
    }
    poco_folder.createDirectories();
    return poco_folder.exists();
}

std::string GetParentFolder(const std::string &path) {
    auto ret = path;
    auto last_pos = ret.size();
    while (last_pos > 0 && ret[last_pos - 1] == '/')
        --last_pos;
    while (last_pos > 0 && ret[last_pos - 1] != '/')
        --last_pos;
    ret = ret.substr(0, last_pos);
    return ret.empty() ? "." : ret;
}

std::pair<std::vector<std::string>, std::vector<std::string>> ListFolderFiles(const string &path, bool need_file, bool need_folder) {
    std::pair<std::vector<std::string>, std::vector<std::string>> ret;
    struct dirent *ptr = nullptr;
    DIR *dir = opendir(path.c_str());
    if (nullptr == dir)
        return ret;

    while ((ptr = readdir(dir)) != nullptr) {
        string filename = ptr->d_name;
        if (ptr->d_type != 4) {
            if (!need_file)
                continue;
            ret.first.emplace_back(filename);
        }
        else {
            if (filename == "." || filename == ".." || !need_folder)
                continue;
            ret.second.emplace_back(filename);
        }
    }

    closedir(dir);
    return ret;
}

PathCheckFunc GenIgnoreFunc(const std::string &prefix, const std::string &suffix, bool break_need, bool break_ignore) {
    auto len_prefix = prefix.size();
    auto len_suffix = suffix.size();
    auto need_rlt = break_need ? PathSearchResult::PATH_SEARCH_RLT_NEED_BREAK : PathSearchResult::PATH_SEARCH_RLT_NEED;
    auto ignore_rlt = break_ignore ? PathSearchResult::PATH_SEARCH_RLT_IGNORE_BREAK : PathSearchResult::PATH_SEARCH_RLT_IGNORE;
    return [len_prefix, len_suffix, prefix, suffix, need_rlt, ignore_rlt](const std::string &parent_path, const std::string &filename) {
        if (len_prefix > 0 && (filename.size() < len_prefix || filename.substr(0, len_prefix) != prefix))
            return need_rlt;
        if (len_suffix > 0 && (filename.size() < len_suffix || filename.substr(filename.size() - len_suffix) != suffix))
            return need_rlt;
        return ignore_rlt;
    };
}

PathCheckFunc GenNeedFunc(const std::string &prefix, const std::string &suffix, bool break_need, bool break_ignore) {
    auto len_prefix = prefix.size();
    auto len_suffix = suffix.size();
    auto need_rlt = break_need ? PathSearchResult::PATH_SEARCH_RLT_NEED_BREAK : PathSearchResult::PATH_SEARCH_RLT_NEED;
    auto ignore_rlt = break_ignore ? PathSearchResult::PATH_SEARCH_RLT_IGNORE_BREAK : PathSearchResult::PATH_SEARCH_RLT_IGNORE;
    return [len_prefix, len_suffix, prefix, suffix, need_rlt, ignore_rlt](const std::string &parent_path, const std::string &filename) {
        if (len_prefix > 0 && (filename.size() < len_prefix || filename.substr(0, len_prefix) != prefix))
            return ignore_rlt;
        if (len_suffix > 0 && (filename.size() < len_suffix || filename.substr(filename.size() - len_suffix) != suffix))
            return ignore_rlt;
        return need_rlt;
    };
}

std::vector<std::string> GetAllFiles(const std::string &root_path, PathCheckFunc file_need_func, PathCheckFunc folder_enter_func) {
    std::vector<std::string> ret;
    if (!IsFolderExist(root_path))
        return ret;

    auto files_folders = ListFolderFiles(root_path);
    bool is_breaked = false;
    for (auto &filename : files_folders.first) {
        if (file_need_func == nullptr) {
            auto path = root_path + "/" + filename;
            ret.emplace_back(path);
            continue;
        }
        auto rlt = file_need_func(root_path, filename);
        if (rlt == PathSearchResult::PATH_SEARCH_RLT_NEED || rlt == PathSearchResult::PATH_SEARCH_RLT_NEED_BREAK) {
            auto path = root_path + "/" + filename;
            ret.emplace_back(path);
        }
        if (rlt == PathSearchResult::PATH_SEARCH_RLT_NEED_BREAK || rlt == PathSearchResult::PATH_SEARCH_RLT_IGNORE_BREAK) {
            is_breaked = true;
            break;
        }
    }
    if (is_breaked)
        return ret;

    for (auto &folder_name : files_folders.second) {
        auto rlt = PathSearchResult::PATH_SEARCH_RLT_NEED;
        if (folder_enter_func != nullptr)
            rlt = folder_enter_func(root_path, folder_name);
        if (rlt == PathSearchResult::PATH_SEARCH_RLT_NEED || rlt == PathSearchResult::PATH_SEARCH_RLT_NEED_BREAK) {
            auto path = root_path + "/" + folder_name;
            auto sub_files = GetAllFiles(path, file_need_func, folder_enter_func);
            ret.insert(ret.end(), sub_files.begin(), sub_files.end());
        }
        if (rlt == PathSearchResult::PATH_SEARCH_RLT_NEED_BREAK || rlt == PathSearchResult::PATH_SEARCH_RLT_IGNORE_BREAK)
            break;
    }

    return ret;
}

std::vector<std::string> GetAllFolders(const std::string &root_path, PathCheckFunc file_need_parent_func, PathCheckFunc folder_enter_func) {
    std::vector<std::string> ret;    if (!IsFolderExist(root_path))
        return ret;

    auto files_folders = ListFolderFiles(root_path);
    bool is_breaked = false;
    for (auto &filename : files_folders.first) {
        if (file_need_parent_func == nullptr) {
            ret.emplace_back(root_path);
            break;
        }
        auto rlt = file_need_parent_func(root_path, filename);
        if (rlt == PathSearchResult::PATH_SEARCH_RLT_NEED || rlt == PathSearchResult::PATH_SEARCH_RLT_NEED_BREAK) {
            ret.emplace_back(root_path);
        }
        if (rlt == PathSearchResult::PATH_SEARCH_RLT_NEED_BREAK || rlt == PathSearchResult::PATH_SEARCH_RLT_IGNORE_BREAK) {
            is_breaked = true;
            break;
        }
    }
    if (is_breaked)
        return ret;

    for (auto &folder_name : files_folders.second) {
        auto rlt = PathSearchResult::PATH_SEARCH_RLT_NEED;
        if (folder_enter_func != nullptr)
            rlt = folder_enter_func(root_path, folder_name);
        if (rlt == PathSearchResult::PATH_SEARCH_RLT_NEED || rlt == PathSearchResult::PATH_SEARCH_RLT_NEED_BREAK) {
            auto path = root_path + "/" + folder_name;
            auto sub_folders = GetAllFolders(path, file_need_parent_func, folder_enter_func);
            ret.insert(ret.end(), sub_folders.begin(), sub_folders.end());
        }
        if (rlt == PathSearchResult::PATH_SEARCH_RLT_NEED_BREAK || rlt == PathSearchResult::PATH_SEARCH_RLT_IGNORE_BREAK)
            break;
    }

    return ret;
}

} /* namespace FileUtil */
} /* namespace sn */
