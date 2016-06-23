#ifndef CUsage_H
#define CUsage_H

#include <CRegExp.h>
#include <CFile.h>
#include <CDir.h>
#include <CDirFTW.h>
#include <CStrUtil.h>
#include <CFuncs.h>
#include <map>

#define TOTAL_M (1<<0)
#define TOTAL_K (1<<1)
#define TOTAL_B (1<<2)

#define DEFAULT_NUM_FILES 20
#define DEFAULT_DIRECTORY "."

enum CUsageDateType {
  DATE_LAST_ACCESSED = 1,
  DATE_LAST_MODIFIED = 2,
  DATE_LAST_CHANGED  = 3
};

static const char *
usage[] = {
  "Description :-",
  "  Displays a list of the largest/smallest and/or oldest/newest",
  "  files, and total space usage, for each of a list of",
  "  directories.",
  "",
  "Usage :-",
  "",
  "  CUsage [-h] [-o <l|s|o|n>] [-n <num_files>]",
  "         [-nl <num_files>] [-ns <num_files>]",
  "         [-no <num_files>] [-nn <num_files>]",
  "         [-da] [-dc] [-dm] [-tm] [-tk] [-tb]",
  "         [-s] [-L] [-mp <pattern>] [-mn <pattern>]",
  "         [<dir> ...]",
  "",
  "    -h              Displays this help text.",
  "    -o <l|s|o|n>    Display the selected lists :-",
  "",
  "                      l - Display Largest Files",
  "                      s - Display Smallest Files",
  "                      o - Display Oldest Files",
  "                      n - Display Newest Files",
  "",
  "                    These options can be used in combination",
  "                    e.g. \'-o lo\' would display the largest and",
  "                    oldest files. By default none of these lists",
  "                    will be displayed.",
  "    -n  <num_files> Sets the number of the files displayed for",
  "                    all lists <num_files> instead of the default",
  "                    20.",
  "    -nl <num_files> Sets the number of the largest files",
  "                    displayed to <num_files> instead of the",
  "                    the default 20.",
  "    -ns <num_files> Sets the number of the smallest files",
  "                    displayed to <num_files> instead of the",
  "                    the default 20.",
  "    -no <num_files> Sets the number of the oldest files",
  "                    displayed to <num_files> instead of the",
  "                    the default 20.",
  "    -nn <num_files> Sets the number of the newest files",
  "                    displayed to <num_files> instead of the",
  "                    the default 20.",
  "    -da             Uses the Last Access Time for date",
  "                    comparison.",
  "    -dc             Uses the Last Change Time for date",
  "                    comparison.",
  "    -dm             Uses the Last Modify Time for date",
  "                    comparison. (Default)",
  "    -tm             Output the Total as Megabytes.",
  "    -tk             Output the Total as Kilobytes.",
  "    -tb             Output the Total as bytes.",
  "    -s              Display Output in short form for",
  "                    easy batch processing.",
  "    -S              Display Output in stream form for",
  "                    easy feeding to other commands.",
  "    -L              Follow links",
  "    -mp <pattern>   only display files matching pattern",
  "    -mn <pattern>   only display files not matching pattern",
  "    -pd <days>      Number of days in the past to check",
  "    <dir> ...       List of directories to process instead",
  "                    of the default current directory.",
  "",
  "Notes :-",
  "  The '-tm', '-tk' and '-tb' options can be combined",
  "  to select any combination of totals, by default all",
  "  three are shown which is the same as specifying",
  "  '-tm -tk -tb'.",
  "",
};

class CUsage;

class CUsageDirWalk : public CDirFTW {
 public:
  CUsageDirWalk(CUsage *unix_usage, const std::string &dirname) :
   CDirFTW(dirname), unix_usage_(unix_usage) {
  }

  void process();

 private:
  CUsage *unix_usage_;
};

struct CUsageFileSpec {
  std::string name;
  int         size;
  time_t      time;
};

struct CUsageDirUsage {
  std::string name;
  int         len;
  int         size;
};

struct CUsageDirUsageCmp {
  int operator()(CUsageDirUsage *a, CUsageDirUsage *b);
};

class CUsage {
 public:
  CUsage();

  bool processOptions(int, char**);
  void process();

  void   processDirectory(const std::string &, int);

  void   updateFileLists(const std::string &, const struct stat *, CFileType);

  void   addLargestFileSpec(CUsageFileSpec *file_spec);
  void   addSmallestFileSpec(CUsageFileSpec *file_spec);
  void   addOldestFileSpec(CUsageFileSpec *file_spec);
  void   addNewestFileSpec(CUsageFileSpec *file_spec);

  void   addFileUsage(const std::string &, size_t);

  void   addDirUsage(const std::string &, size_t);

  void   deleteDirectory(char *);

  int    addLargestFile(CUsageFileSpec *, CUsageFileSpec *);
  int    addSmallestFile(CUsageFileSpec *, CUsageFileSpec *);
  int    addOldestFile(CUsageFileSpec *, CUsageFileSpec *);
  int    addNewestFile(CUsageFileSpec *, CUsageFileSpec *);

  void   deleteFileSpec(CUsageFileSpec *);

  void   printLargestFile(CUsageFileSpec *);
  void   printSmallestFile(CUsageFileSpec *);
  void   printOldestFile(CUsageFileSpec *);
  void   printNewestFile(CUsageFileSpec *);

  void   printDirUsages(void);

  void   addDirUsageToArray(CUsageDirUsage *, CUsageDirUsage ***);

  void   setFileSpecLength(CUsageFileSpec *);

  time_t statTime(const struct stat *);

  void   error(const char *, ...);

 private:
  typedef std::vector<std::string>               DirNameList;
  typedef std::list<CUsageFileSpec *>            FileSpecList;
  typedef std::map<std::string,CUsageDirUsage *> DirUsageMap;
  typedef std::list<CUsageDirUsage *>            DirUsageList;

  CUsageDateType  date_type;
  bool            display_largest;
  bool            display_smallest;
  bool            display_oldest;
  bool            display_newest;
  bool            display_dirs;
  bool            display_count;
  bool            short_form;
  bool            short_line_form;
  bool            stream_form;
  bool            follow_links;
  bool            ignore_hidden;
  int             total_output;
  DirNameList     directory_list;
  uint            num_largest;
  uint            num_smallest;
  uint            num_oldest;
  uint            num_newest;
  std::string     match_pattern;
  CRegExp        *match_regex;
  std::string     no_match_pattern;
  CRegExp        *no_match_regex;
  std::string     match_type;
  FileSpecList    largest_file_list;
  FileSpecList    smallest_file_list;
  FileSpecList    oldest_file_list;
  FileSpecList    newest_file_list;
  DirUsageMap     dir_usage_list;
  uint            max_directory_length;
  std::string     format_string;
  long            total_usage;
  long            num_files, num_dirs;
  std::string     link_directory;
  int             num_days;
  time_t          current_time;
  uint            max_name_length;
};

#endif
