#ifndef CUsage_H
#define CUsage_H

#include <CRegExp.h>
#include <CFile.h>
#include <CDir.h>
#include <CDirFTW.h>
#include <CStrUtil.h>
#include <CFuncs.h>
#include <map>

#define TOTAL_G (1<<0)
#define TOTAL_M (1<<1)
#define TOTAL_K (1<<2)
#define TOTAL_B (1<<3)

#define DEFAULT_NUM_FILES 40
#define DEFAULT_DIRECTORY "."

enum class CUsageDateType {
  LAST_ACCESSED = 1,
  LAST_MODIFIED = 2,
  LAST_CHANGED  = 3
};

static const char *
usage_str[] = {
  "Description :-",
  "  Displays a list of the largest/smallest and/or oldest/newest files, and total space",
  "  usage, for each of a list of directories.",
  "",
  "Usage :-",
  "  CUsage [-h] [-o <l|s|o|n>] [-n <num_files>] [-nl <num_files>] [-ns <num_files>]",
  "         [-no <num_files>] [-nn <num_files>] [-da] [-dc] [-dm] [-tg] [-tm] [-tk] [-tb]",
  "         [-s] [-sl] [-S] [-L] [-H] [-mp <pattern>] [-mn <pattern>]",
  "         [-p <days>][<dir> ...]",
  "",
  "    -h               Displays this help text.",
  "    -o <l|s|o|n|d|c> Display the selected lists :-",
  "                       l - Display Largest Files",
  "                       s - Display Smallest Files",
  "                       o - Display Oldest Files",
  "                       n - Display Newest Files",
  "                       d - Display Directories",
  "                       c - Display Count",
  "                     These options can be used in combination e.g. \'-o lo\' would",
  "                     display the largest and oldest files.",
  "                     By default none of these lists will be displayed.",
  "    -n  <num_files>  Sets the number of the files displayed for all lists <num_files>",
  "                     instead of the default 40.",
  "    -nl <num_files>  Sets the number of the largest files displayed to <num_files>",
  "                     instead of the the default 40.",
  "    -ns <num_files>  Sets the number of the smallest files displayed to <num_files>",
  "                     instead of the the default 40.",
  "    -no <num_files>  Sets the number of the oldest files displayed to <num_files>",
  "                     instead of the the default 40.",
  "    -nn <num_files>  Sets the number of the newest files displayed to <num_files>",
  "                     instead of the the default 40.",
  "    -da              Uses the Last Access Time for date comparison.",
  "    -dc              Uses the Last Change Time for date comparison.",
  "    -dm              Uses the Last Modify Time for date comparison. (Default)",
  "    -tg              Output the Total as Gigabytes.",
  "    -tm              Output the Total as Megabytes.",
  "    -tk              Output the Total as Kilobytes.",
  "    -tb              Output the Total as bytes.",
  "    -s               Display Output in short form for easy batch processing.",
  "    -sl              Display Output in short line form for easy batch processing.",
  "    -S               Display Output in stream form for easy feeding to other commands.",
  "    -L               Follow links",
  "    -H               Ignore hidden (dot files)",
  "    -mp <pattern>    Only display files matching pattern",
  "    -mn <pattern>    Only display files not matching pattern",
  "    -mt <type>       Only display files matching type :=",
  "                       exe   - executable files",
  "                       obj   - object files",
  "                       core  - core files",
  "                       image - image files",
  "    -p <days>        Number of days in the past to check",
  "    <dir> ...        List of directories to process instead of the default current directory.",
  "",
  "Notes :-",
  "  The '-tg', '-tm', '-tk' and '-tb' options can be combined to select any combination of",
  "  totals, by default all sizes are shown which is the same as specifying '-tg -tm -tk -tb'.",
  "",
};

//---

class CUsage;

class CUsageDirWalk : public CDirFTW {
 public:
  CUsageDirWalk(CUsage *unix_usage, const std::string &dirname) :
   CDirFTW(dirname), unix_usage_(unix_usage) {
  }

  void process();

 private:
  CUsage *unix_usage_ { nullptr };
};

//---

struct CUsageFileSpec {
  std::string name;
  int         size { 0 };
  time_t      time { };
};

struct CUsageDirUsage {
  std::string name;
  int         len  { 0 };
  size_t      size { 0 };
  bool        leaf { true };
};

//---

struct CUsageDirUsageCmp {
  bool operator()(CUsageDirUsage *a, CUsageDirUsage *b);
};

//---

class CUsage {
 public:
  CUsage();

  bool processOptions(int, char**);
  void process();

  void processDirectory(const std::string &, int);

  void updateFileLists(const std::string &, const struct stat *, CFileType);

  void addLargestFileSpec(CUsageFileSpec *file_spec);
  void addSmallestFileSpec(CUsageFileSpec *file_spec);
  void addOldestFileSpec(CUsageFileSpec *file_spec);
  void addNewestFileSpec(CUsageFileSpec *file_spec);

  void addDirFileUsage(const std::string &, size_t);
  void addFileUsage   (const std::string &, size_t);
  void addDirUsage   (const std::string &, size_t, int);

  void deleteDirectory(char *);

  int addLargestFile(CUsageFileSpec *, CUsageFileSpec *);
  int addSmallestFile(CUsageFileSpec *, CUsageFileSpec *);
  int addOldestFile(CUsageFileSpec *, CUsageFileSpec *);
  int addNewestFile(CUsageFileSpec *, CUsageFileSpec *);

  void deleteFileSpec(CUsageFileSpec *);

  void printLargestFile(CUsageFileSpec *);
  void printSmallestFile(CUsageFileSpec *);
  void printOldestFile(CUsageFileSpec *);
  void printNewestFile(CUsageFileSpec *);

  void printDirUsages(void);

  void addDirUsageToArray(CUsageDirUsage *, CUsageDirUsage ***);

  void setFileSpecLength(CUsageFileSpec *);

  time_t statTime(const struct stat *);

  void error(const char *, ...);

 private:
  class UnitsNum {
   public:
    UnitsNum(size_t num) :
     num_(num) {
      num_k_ = num   /1024.0;
      num_m_ = num_k_/1024.0;
      num_g_ = num_m_/1024.0;
    }

    size_t num() const { return num_; }

    double k() const { return num_k_; }
    double m() const { return num_m_; }
    double g() const { return num_g_; }

   private:
    size_t num_   { 0 };
    double num_k_ { 0.0 };
    double num_m_ { 0.0 };
    double num_g_ { 0.0 };
  };

 private:
  using DirNameList  = std::vector<std::string>;
  using FileSpecList = std::list<CUsageFileSpec *>;
  using DirUsageMap  = std::map<std::string,CUsageDirUsage *>;
  using DirUsageList = std::list<CUsageDirUsage *>;

  CUsageDateType date_type            { CUsageDateType::LAST_MODIFIED };
  bool           display_largest      { false };
  bool           display_smallest     { false };
  bool           display_oldest       { false };
  bool           display_newest       { false };
  bool           display_dirs         { false };
  bool           display_count        { false };
  bool           short_form           { false };
  bool           short_line_form      { false };
  bool           stream_form          { false };
  bool           follow_links         { false };
  bool           ignore_hidden        { false };
  int            total_output         { 0 };
  DirNameList    directory_list;
  uint           num_largest          { DEFAULT_NUM_FILES };
  uint           num_smallest         { DEFAULT_NUM_FILES };
  uint           num_oldest           { DEFAULT_NUM_FILES };
  uint           num_newest           { DEFAULT_NUM_FILES };
  std::string    match_pattern;
  CRegExp*       match_regex          { nullptr };
  std::string    no_match_pattern;
  CRegExp*       no_match_regex       { nullptr };
  std::string    match_type;
  FileSpecList   largest_file_list;
  FileSpecList   smallest_file_list;
  FileSpecList   oldest_file_list;
  FileSpecList   newest_file_list;
  DirUsageMap    dir_usage_list;
  uint           max_directory_length { 0 };
  std::string    format_string;
  long           total_usage          { 0 };
  long           num_files            { 0 };
  long           num_dirs             { 0 };
  std::string    link_directory;
  int            num_days             { -1 };
  time_t         current_time         { };
  uint           max_name_length      { 0 };
};

#endif
