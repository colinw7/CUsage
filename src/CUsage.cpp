#include <CUsage.h>
#include <CFileUtil.h>

#include <cstring>
#include <cstdio>
#include <algorithm>
#include <iostream>

/*------------------------------------------------------------------
 *
 * Displays a list of the largest/smallest and/or oldest/newest files and total space
 * usage, for each of a list of directories.
 *
 * Usage:
 *   CUsage [-h] [-o <l|s|o|n|d|c>] [-n <num_files>] [-nl <num_files>] [-ns <num_files>]
 *          [-no <num_files>] [-nn <num_files>] [-da] [-dc] [-dm] [-tg] [-tm] [-tk] [-tb]
 *          [-s] [-sl] [-S] [-L] [-H] [-mp <pattern>] [-mn <pattern>]
 *          [-p <days>][<dir> ...]
 *
 *   -h               Displays this help text.
 *   -o <l|s|o|n|d|c> Display the selected lists :-
 *                      l - Display Largest Files
 *                      s - Display Smallest Files
 *                      o - Display Oldest Files
 *                      n - Display Newest Files
 *                      d - Display Directories (largest first)
 *                      c - Display Count
 *                    These options can be used in combination e.g. '-o lo' would
 *                    display the largest and oldest files.
 *                    By default none of these lists will be displayed.
 *   -n  <num_files>  Sets the number of the files displayed for all lists <num_files>
 *                    instead of the default 20.
 *   -nl <num_files>  Sets the number of the largest files displayed to <num_files>
 *                    instead of the the default 20.
 *   -ns <num_files>  Sets the number of the smallest files displayed to <num_files>
 *                    instead of the the default 20.
 *   -no <num_files>  Sets the number of the oldest files displayed to <num_files>
 *                    instead of the the default 20.
 *   -nn <num_files>  Sets the number of the newest files displayed to <num_files>
 *                    instead of the the default 20.
 *   -da              Uses the Last Access Time for date comparison.
 *   -dc              Uses the Last Change Time for date comparison.
 *   -dm              Uses the Last Modify Time for date comparison. (Default)
 *   -tg              Output the Total as Gigabytes.
 *   -tm              Output the Total as Megabytes.
 *   -tk              Output the Total as Kilobytes.
 *   -tb              Output the Total as bytes.
 *   -s               Display Output in short form for easy batch processing.
 *   -sl              Display Output in short line form for easy batch processing.
 *   -S               Display Output in stream form for feeding into other commands
 *   -L               follow links
 *   -H               Ignore hidden (dot files)
 *   -mp <pattern>    Only display files matching pattern
 *   -mn <pattern>    Only display files not matching pattern
 *   -mt <type>       Only display files matching type :=
 *                      exe   - executable files
 *                      obj   - object files
 *                      core  - core files
 *                      image - image files
 *   -p <days>        Number of days in the past to check
 *   <dir> ...        List of directories to process instead of the default current directory.
 *
 * Notes:
 *   The '-tg', '-tm', '-tk' and '-tb' options can be combined to select any combination of
 *   totals, by default sizes are shown which is the same as specifying '-tg -tm -tk -tb'.
 *
 *------------------------------------------------------------------*/

int
main(int argc, char **argv)
{
  CUsage usage;

  usage.processOptions(argc, argv);

  usage.process();

  exit(0);
}

CUsage::
CUsage()
{
}

bool
CUsage::
processOptions(int argc, char **argv)
{
  for (int i = 1; i < argc; ++i) {
    if (argv[i][0] == '-') {
      switch (argv[i][1]) {
        case 'h': {
          int num_lines = sizeof(usage_str)/sizeof(char *);

          for (i = 0; i < num_lines; ++i)
            std::cerr << usage_str[i] << "\n";

          exit(1);

          break;
        }
        // -o <l|s|o|n|d|c>
        case 'o': {
          if (i < argc - 1) {
            for (int j = 0; j < (int) strlen(argv[i + 1]); j++) {
              switch (argv[i + 1][j]) {
                case 'l': display_largest  = true; break;
                case 's': display_smallest = true; break;
                case 'o': display_oldest   = true; break;
                case 'n': display_newest   = true; break;
                case 'd': display_dirs     = true; break;
                case 'c': display_count    = true; break;
                default:
                  error("Invalid Output List Specifier \'%c\'", argv[i + 1][j]);
                  break;
              }
            }

            ++i;
          }
          else
            error("Missing List Specifier for \'-o\' Option");

          break;
        }
        // n, nl, ns, no, nn
        case 'n': {
          if (i < argc - 1) {
            if (argv[i][2] == '\0' || argv[i][2] == 'l' ||
                argv[i][2] == 's'  || argv[i][2] == 'o' ||
                argv[i][2] == 'n') {
              int num_files1 = atoi(argv[i + 1]);

              if      (argv[i][2] == '\0') {
                num_largest  = num_files1;
                num_smallest = num_files1;
                num_oldest   = num_files1;
                num_newest   = num_files1;
              }
              else if (argv[i][2] == 'l') num_largest  = num_files1;
              else if (argv[i][2] == 's') num_smallest = num_files1;
              else if (argv[i][2] == 'o') num_oldest   = num_files1;
              else if (argv[i][2] == 'n') num_newest   = num_files1;

              ++i;
            }
            else
              error("Invalid List Specifier for \'%s\' Option", "-n[lson]");
          }
          else
            error("Invalid Option \'-%s\'", argv[i]);

          break;
        }
        // da, dc, dm
        case 'd': {
          if      (argv[i][2] == 'a') date_type = CUsageDateType::LAST_ACCESSED;
          else if (argv[i][2] == 'c') date_type = CUsageDateType::LAST_CHANGED;
          else if (argv[i][2] == 'm') date_type = CUsageDateType::LAST_MODIFIED;
          else
            error("Invalid Date Specifier for \'%s\' Option", "-d[a|c|m]");

          break;
        }
        // tg, tm, tk, tb
        case 't': {
          if      (argv[i][2] == 'g') total_output |= TOTAL_G;
          else if (argv[i][2] == 'm') total_output |= TOTAL_M;
          else if (argv[i][2] == 'k') total_output |= TOTAL_K;
          else if (argv[i][2] == 'b') total_output |= TOTAL_B;
          else
            error("Invalid Total Type Specifier for \'%s\' Option", "-t[m|k|b]");

          break;
        }
        // s, sl, S, L
        case 's':
          if (argv[i][2] == 'l')
            short_line_form = true;
          else
            short_form = true;

          break;
        case 'S': stream_form   = true; break;
        case 'L': follow_links  = true; break;
        case 'H': ignore_hidden = true; break;
        // mp, mn
        case 'm': {
          if      (argv[i][2] == 'p') {
            if (i < argc - 1)
              match_pattern = argv[++i];
            else
              error("Missing pattern for \'%s\' Option", argv[i]);
          }
          else if (argv[i][2] == 'n') {
            if (i < argc - 1)
              no_match_pattern = argv[++i];
            else
              error("Missing pattern for \'%s\' Option", argv[i]);
          }
          else if (argv[i][2] == 't') {
            if (i < argc - 1)
              match_type = argv[++i];
            else
              error("Missing pattern for \'%s\' Option", argv[i]);
          }
          else
            error("Invalid Match Type Specifier for \'%s\' Option", "-m[p|n]");

          break;
        }
        // p
        case 'p': {
          if (i < argc - 1)
            num_days = atoi(argv[++i]);
          else
            error("Missing value for \'%s\' Option", argv[i]);

          break;
        }
        default:
          error("Invalid Option \'%s\'", argv[i]);

          break;
      }
    }
    else
      directory_list.push_back(argv[i]);
  }

  if (total_output == 0)
    total_output = TOTAL_G | TOTAL_M | TOTAL_K | TOTAL_B;

  return true;
}

void
CUsage::
process()
{
  uint num_directories = directory_list.size();

  if (num_directories == 0)
    directory_list.push_back(DEFAULT_DIRECTORY);

  if (num_largest <= 0 || num_largest > 1000) {
    error("Invalid value for number of largest files - %d", num_largest);
    exit(1);
  }

  if (num_smallest <= 0 || num_smallest > 1000) {
    error("Invalid value for number of smallest files - %d", num_smallest);
    exit(1);
  }

  if (num_oldest <= 0 || num_oldest > 1000) {
    error("Invalid value for number of oldest files - %d", num_oldest);
    exit(1);
  }

  if (num_newest <= 0 || num_newest > 1000) {
    error("Invalid value for number of newest files - %d", num_newest);
    exit(1);
  }

  if (match_pattern != "") {
    match_regex = new CRegExp(match_pattern);

    match_regex->setExtended(true);
    match_regex->setMatchBOL(false);
    match_regex->setMatchEOL(false);
  }
  else
    match_regex = nullptr;

  if (no_match_pattern != "") {
    no_match_regex = new CRegExp(no_match_pattern);

    no_match_regex->setExtended(true);
    no_match_regex->setMatchBOL(false);
    no_match_regex->setMatchEOL(false);
  }
  else
    no_match_regex = nullptr;

  //------------

  /* Get Max Directory Length */

  max_directory_length = 1;

  num_directories = directory_list.size();

  for (uint i = 0; i < num_directories; ++i)
    max_directory_length = std::max(max_directory_length, uint(directory_list[i].size()));

  //------------

  /* Get Current Time */

  current_time = time(0);

  //------------

  /* Process List of Directories */

  for (uint i = 0; i < num_directories; ++i)
    processDirectory(directory_list[i], i);
}

// Process all files in the specified directory and produce a total space usage
// and update list of oldest, newest, largest and smallest files (if requested
// by the user).
void
CUsage::
processDirectory(const std::string &directory, int num_directories)
{
  // Initalise Total Usage
  total_usage = 0;

  //------------

  // Output Directory Header if more than one directory is being processed
  if (num_directories > 1) {
    if (! short_form && ! short_line_form && ! stream_form) {
      std::cout << "\n";
      std::cout << "--------------- Directory \'" << directory << "\' ---------------\n";
      std::cout << "\n";
    }
    else {
      if (display_largest || display_smallest || display_oldest  || display_newest) {
        std::cout << directory;

        if (! short_line_form)
          std::cout << "\n";
      }
      else {
        CStrUtil::sprintf(format_string, "%%-%ds", max_directory_length);

        std::cout << CStrUtil::strprintf(format_string.c_str(), directory.c_str());

        if (! short_line_form)
          std::cout << "\n";
      }
    }
  }

  //------------

  // Process all Files in the Directory
  CUsageDirWalk walk(this, directory);

  walk.walk();

  //------------

  /* Display Largest File List if Requested */

  if (display_largest) {
    max_name_length = 0;

    for (const auto &largest_file : largest_file_list)
      setFileSpecLength(largest_file);

    if      (! short_form && ! short_line_form && ! stream_form)
      CStrUtil::sprintf(format_string, "%%-%ds %%8d", max_name_length);
    else if (! stream_form)
      CStrUtil::sprintf(format_string, "  %%-%ds %%8d", max_name_length);
    else
      CStrUtil::sprintf(format_string, "%%-%ds", max_name_length);

    if      (! short_form && ! short_line_form && ! stream_form) {
      std::cout << "List of Top " << largest_file_list.size() << " Largest Files\n";
      std::cout << "\n";
    }
    else if (! stream_form)
      std::cout << "Largest " << largest_file_list.size() << "\n";

    for (const auto &largest_file : largest_file_list)
      printLargestFile(largest_file);

    if (! short_form && ! short_line_form && ! stream_form)
      std::cout << "\n";
  }

  //------------

  /* Display Smallest File List if Requested */

  if (display_smallest) {
    max_name_length = 0;

    for (const auto &smallest_file : smallest_file_list)
      setFileSpecLength(smallest_file);

    if      (! short_form && ! short_line_form && ! stream_form)
      CStrUtil::sprintf(format_string, "%%-%ds %%8d", max_name_length);
    else if (! stream_form)
      CStrUtil::sprintf(format_string, "  %%-%ds %%8d", max_name_length);
    else
      CStrUtil::sprintf(format_string, "%%-%ds", max_name_length);

    if      (! short_form && ! short_line_form && ! stream_form) {
      std::cout << "List of Top " << smallest_file_list.size() << " Smallest Files\n";
      std::cout << "\n";
    }
    else if (! stream_form)
      std::cout << "Smallest " << smallest_file_list.size() << "\n";

    for (const auto &smallest_file : smallest_file_list)
      printSmallestFile(smallest_file);

    if (! short_form && ! short_line_form && ! stream_form)
      std::cout << "\n";
  }

  //------------

  /* Display Oldest File List if Requested */

  if (display_oldest) {
    max_name_length = 0;

    for (const auto &oldest_file : oldest_file_list)
      setFileSpecLength(oldest_file);

    if      (! short_form && ! short_line_form && ! stream_form)
      CStrUtil::sprintf(format_string, "%%-%ds %%s", max_name_length);
    else if (! stream_form)
      CStrUtil::sprintf(format_string, "  %%-%ds %%s", max_name_length);
    else
      CStrUtil::sprintf(format_string, "%%-%ds", max_name_length);

    if (! short_form && ! short_line_form && ! stream_form) {
      std::cout << "List of Top " << oldest_file_list.size() << " Oldest Files\n";
      std::cout << "\n";
    }
    else if (! stream_form)
      std::cout << "Oldest " << oldest_file_list.size() << "\n";

    for (const auto &oldest_file : oldest_file_list)
      printOldestFile(oldest_file);

    if (! short_form && ! short_line_form && ! stream_form)
      std::cout << "\n";
  }

  //------------

  /* Display Newest File List if Requested */

  if (display_newest) {
    max_name_length = 0;

    for (const auto &newest_file : newest_file_list)
      setFileSpecLength(newest_file);

    if      (! short_form && ! short_line_form && ! stream_form)
      CStrUtil::sprintf(format_string, "%%-%ds %%s", max_name_length);
    else if (! stream_form)
      CStrUtil::sprintf(format_string, "  %%-%ds %%s", max_name_length);
    else
      CStrUtil::sprintf(format_string, "%%-%ds", max_name_length);

    if (! short_form && ! short_line_form && ! stream_form) {
      std::cout << "List of Top " << newest_file_list.size() << " Newest Files\n";
      std::cout << "\n";
    }
    else if (! stream_form)
      std::cout << "Newest " << newest_file_list.size() << "\n";

    for (const auto &newest_file : newest_file_list)
      printNewestFile(newest_file);

    if (! short_form && ! short_line_form && ! stream_form)
      printf("\n");
  }

  //------------

  /* Display Directories if Requested */

  if (display_dirs)
    printDirUsages();

  //------------

  if (display_count) {
    std::cout << "  " << CStrUtil::strprintf("%12d", num_files) << " Files\n";
    std::cout << "  " << CStrUtil::strprintf("%12d", num_dirs ) << " Dirs\n";
  }

  //------------

  // Display Total Usage in Bytes, Kilobytes, Megabytes and Gigabytes
  UnitsNum unitsTotal(total_usage);

  if      (! short_form && ! short_line_form && ! stream_form) {
    if (display_largest || display_smallest || display_oldest  || display_newest)
      std::cout << "Total :-\n";

    if (total_output & TOTAL_G)
      std::cout << "  " << CStrUtil::strprintf("%12.2lf", unitsTotal.g()) << " Gigabytes\n";

    if (total_output & TOTAL_M)
      std::cout << "  " << CStrUtil::strprintf("%12.2lf", unitsTotal.m()) << " Megabytes\n";

    if (total_output & TOTAL_K)
      std::cout << "  " << CStrUtil::strprintf("%12.2lf", unitsTotal.k()) << " Kilobytes\n";

    if (total_output & TOTAL_B)
      std::cout << "  " << CStrUtil::strprintf("%12.2ld", total_usage) << " Bytes\n";
  }
  else if (! stream_form) {
    if (display_largest || display_smallest || display_oldest  || display_newest)
      std::cout << "Total\n";

    if (total_output & TOTAL_G) {
      std::cout << "  " << CStrUtil::strprintf("%12.2lfMb", unitsTotal.g());
      if (! short_line_form) std::cout << "\n";
    }

    if (total_output & TOTAL_M) {
      std::cout << "  " << CStrUtil::strprintf("%12.2lfMb", unitsTotal.m());
      if (! short_line_form) std::cout << "\n";
    }

    if (total_output & TOTAL_K) {
      std::cout << "  " << CStrUtil::strprintf("%12.2lfKb", unitsTotal.k());
      if (! short_line_form) std::cout << "\n";
    }

    if (total_output & TOTAL_B) {
      std::cout << "  " << CStrUtil::strprintf("%12ld", total_usage);
      if (! short_line_form) std::cout << "\n";
    }

    std::cout << "\n";
  }
  else
    std::cout << " ";

  /*------*/

  for (auto &file : largest_file_list ) delete file;
  for (auto &file : smallest_file_list) delete file;
  for (auto &file : oldest_file_list  ) delete file;
  for (auto &file : newest_file_list  ) delete file;

  largest_file_list .clear();
  smallest_file_list.clear();
  oldest_file_list  .clear();
  newest_file_list  .clear();
}

void
CUsageDirWalk::
process()
{
  unix_usage_->updateFileLists(getFileName(), getStat(), getType());
}

// Process each file updating the total usage and the largest, smallest, newest
// and oldest file lists.
void
CUsage::
updateFileLists(const std::string &filename, const struct stat *ftw_stat, CFileType type)
{
  if (   match_regex != nullptr &&  ! match_regex->find(filename))
    return;

  if (no_match_regex != nullptr && no_match_regex->find(filename))
    return;

  if (match_type != "" && type != CFILE_TYPE_INODE_DIR) {
    bool match = false;

    std::string filename1 = filename;

    auto pos = filename1.rfind('/');

    if (pos != std::string::npos)
      filename1 = filename1.substr(pos + 1);

    CFileType file_type = CFileUtil::getType(filename1);

    if (! match && match_type == "exe") {
      if (file_type & CFILE_TYPE_BIN && file_type & CFILE_TYPE_EXEC)
        match = true;
    }

    if (! match && match_type == "image") {
      if (file_type & CFILE_TYPE_IMAGE)
        match = true;
    }

    if (! match && match_type == "core") {
      if (file_type == CFILE_TYPE_APP_CORE)
        match = true;
    }

    if (! match)
      return;
  }

  //------------

  if (ignore_hidden) {
    std::string filename1 = filename;

    auto pos = filename1.rfind('/');

    while (pos != std::string::npos) {
      if (filename1[pos + 1] == '.')
        return;

      filename1 = filename1.substr(0, pos);

      pos = filename1.rfind('/');
    }
  }

  //------------

  // Process Directory
  if (type == CFILE_TYPE_INODE_DIR) {
    // If link add link size and set link directory ...
    if (CFile::isLink(filename)) {
      struct stat my_stat;

      lstat(filename.c_str(), &my_stat);

      addFileUsage(filename, my_stat.st_size);
    }

    // ... otherwise add directory node list size
    else {
      addDirFileUsage(filename, ftw_stat->st_size);
    }

    ++num_dirs;

    return;
  }

  //------------

  // If link add link size but don't include in file lists
  if (CFile::isLink(filename)) {
    struct stat my_stat;

    lstat(filename.c_str(), &my_stat);

    addFileUsage(filename, my_stat.st_size);

    ++num_files;

    return;
  }

  //------------

  // Ignore if older than specified days
  if (num_days >= 0) {
    double cmp = difftime(current_time, ftw_stat->st_ctime);

    int num_days1 = (int) (cmp/86400);

    if (num_days1 > num_days)
      return;
  }

  //------------

  // Update Total for Ordinary File
  addFileUsage(filename, ftw_stat->st_size);

  ++num_files;

  // Update Largest Files
  if (display_largest) {
    if (largest_file_list.size() < num_largest) {
      auto *file_spec = new CUsageFileSpec;

      file_spec->name = filename;
      file_spec->size = ftw_stat->st_size;
      file_spec->time = statTime(ftw_stat);

      addLargestFileSpec(file_spec);
    }
    else {
      CUsageFileSpec *file_spec = largest_file_list.back();

      if (ftw_stat->st_size > file_spec->size) {
        largest_file_list.remove(file_spec);

        auto *file_spec1 = new CUsageFileSpec;

        file_spec1->name = filename;
        file_spec1->size = ftw_stat->st_size;
        file_spec1->time = statTime(ftw_stat);

        addLargestFileSpec(file_spec1);
      }
    }
  }

  // Update Smallest Files
  if (display_smallest) {
    if (smallest_file_list.size() < num_smallest) {
      auto *file_spec = new CUsageFileSpec;

      file_spec->name = filename;
      file_spec->size = ftw_stat->st_size;
      file_spec->time = statTime(ftw_stat);

      addSmallestFileSpec(file_spec);
    }
    else {
      CUsageFileSpec *file_spec = smallest_file_list.back();

      if (ftw_stat->st_size < file_spec->size) {
        smallest_file_list.remove(file_spec);

        auto *file_spec1 = new CUsageFileSpec;

        file_spec1->name = filename;
        file_spec1->size = ftw_stat->st_size;
        file_spec1->time = statTime(ftw_stat);

        addSmallestFileSpec(file_spec1);
      }
    }
  }

  // Update Oldest Files
  if (display_oldest) {
    if (oldest_file_list.size() < num_oldest) {
      auto *file_spec = new CUsageFileSpec;

      file_spec->name = filename;
      file_spec->size = ftw_stat->st_size;
      file_spec->time = statTime(ftw_stat);

      addOldestFileSpec(file_spec);
    }
    else {
      CUsageFileSpec *file_spec = oldest_file_list.back();

      if (statTime(ftw_stat) < file_spec->time) {
        oldest_file_list.remove(file_spec);

        auto *file_spec1 = new CUsageFileSpec;

        file_spec1->name = filename;
        file_spec1->size = ftw_stat->st_size;
        file_spec1->time = statTime(ftw_stat);

        addOldestFileSpec(file_spec1);
      }
    }
  }

  // Update Newest Files
  if (display_newest) {
    if (newest_file_list.size() < num_newest) {
      auto *file_spec = new CUsageFileSpec;

      file_spec->name = filename;
      file_spec->size = ftw_stat->st_size;
      file_spec->time = statTime(ftw_stat);

      addNewestFileSpec(file_spec);
    }
    else {
      auto *file_spec = newest_file_list.back();

      if (statTime(ftw_stat) > file_spec->time) {
        newest_file_list.remove(file_spec);

        auto *file_spec1 = new CUsageFileSpec;

        file_spec1->name = filename;
        file_spec1->size = ftw_stat->st_size;
        file_spec1->time = statTime(ftw_stat);

        addNewestFileSpec(file_spec1);
      }
    }
  }
}

struct IsLargerFileSpec {
  CUsageFileSpec *spec_;

  IsLargerFileSpec(CUsageFileSpec *spec) :
   spec_(spec) {
  }

  bool operator()(CUsageFileSpec *spec) {
    return (spec_->size > spec->size);
  }
};

struct IsSmallerFileSpec {
  CUsageFileSpec *spec_;

  IsSmallerFileSpec(CUsageFileSpec *spec) :
   spec_(spec) {
  }

  bool operator()(CUsageFileSpec *spec) {
    return (spec_->size < spec->size);
  }
};

struct IsOlderFileSpec {
  CUsageFileSpec *spec_;

  IsOlderFileSpec(CUsageFileSpec *spec) :
   spec_(spec) {
  }

  bool operator()(CUsageFileSpec *spec) {
    return (spec_->time < spec->time);
  }
};

struct IsNewerFileSpec {
  CUsageFileSpec *spec_;

  IsNewerFileSpec(CUsageFileSpec *spec) :
   spec_(spec) {
  }

  bool operator()(CUsageFileSpec *spec) {
    return (spec_->time > spec->time);
  }
};

void
CUsage::
addLargestFileSpec(CUsageFileSpec *file_spec)
{
  auto p = find_if(largest_file_list.begin(), largest_file_list.end(),
                   IsLargerFileSpec(file_spec));

  if (p != largest_file_list.end())
    largest_file_list.insert(p, file_spec);
  else
    largest_file_list.push_back(file_spec);
}

void
CUsage::
addSmallestFileSpec(CUsageFileSpec *file_spec)
{
  auto p = find_if(smallest_file_list.begin(), smallest_file_list.end(),
                   IsSmallerFileSpec(file_spec));

  if (p != smallest_file_list.end())
    smallest_file_list.insert(p, file_spec);
  else
    smallest_file_list.push_back(file_spec);
}

void
CUsage::
addOldestFileSpec(CUsageFileSpec *file_spec)
{
  auto p = find_if(oldest_file_list.begin(), oldest_file_list.end(),
                   IsOlderFileSpec(file_spec));

  if (p != oldest_file_list.end())
    oldest_file_list.insert(p, file_spec);
  else
    oldest_file_list.push_back(file_spec);
}

void
CUsage::
addNewestFileSpec(CUsageFileSpec *file_spec)
{
  auto p = find_if(newest_file_list.begin(), newest_file_list.end(),
                   IsNewerFileSpec(file_spec));

  if (p != newest_file_list.end())
    newest_file_list.insert(p, file_spec);
  else
    newest_file_list.push_back(file_spec);
}

void
CUsage::
addDirFileUsage(const std::string &, size_t size)
{
  // don't add dir node size
  total_usage += size;
}

void
CUsage::
addFileUsage(const std::string &filename, size_t size)
{
  if (display_dirs) {
    auto pos = filename.rfind('/');

    if (pos != std::string::npos)
      addDirUsage(filename.substr(0, pos), size, 0);
  }

  total_usage += size;
}

void
CUsage::
addDirUsage(const std::string &dirname, size_t size, int depth)
{
  auto pdir = dir_usage_list.find(dirname);

  CUsageDirUsage *dir_usage = nullptr;

  if (pdir == dir_usage_list.end()) {
    dir_usage = new CUsageDirUsage;

    dir_usage->name = dirname;
    dir_usage->len  = dirname.size();
    dir_usage->size = 0;

    dir_usage_list[dirname] = dir_usage;
  }
  else
    dir_usage = (*pdir).second;

  if (depth > 0)
    dir_usage->leaf = false;

  dir_usage->size += size;

  auto pos = dirname.rfind('/');

  if (pos != std::string::npos)
    addDirUsage(dirname.substr(0, pos), size, depth + 1);
}

// Routine used to Output a Largest File.
void
CUsage::
printLargestFile(CUsageFileSpec *file_spec)
{
  std::string file_name = file_spec->name;

  while (file_name.find("./") != std::string::npos)
    file_name = file_name.substr(2);

  std::cout << CStrUtil::strprintf(format_string.c_str(), file_name.c_str(), file_spec->size);

  CFileType type = CFileUtil::getType(file_name);

  if (! short_form && ! short_line_form && ! stream_form)
    std::cout << " " << CFileUtil::getTypeMime(type);

  std::cout << "\n";
}

// Routine used to Output a Smallest File.
void
CUsage::
printSmallestFile(CUsageFileSpec *file_spec)
{
  std::string file_name = file_spec->name;

  while (file_name.find("./") != std::string::npos)
    file_name = file_name.substr(2);

  std::cout << CStrUtil::strprintf(format_string.c_str(), file_name.c_str(), file_spec->size);

  // Small files don't often have type

  std::cout << "\n";
}

// Routine used to Output an Oldest File.
void
CUsage::
printOldestFile(CUsageFileSpec *file_spec)
{
  std::string file_name = file_spec->name;

  while (file_name.find("./") != std::string::npos)
    file_name = file_name.substr(2);

  struct tm *tm = localtime(&file_spec->time);

  char time_string[256];

  strftime(time_string, 256, "%a %h %e %H:%M:%S %Z %Y", tm);

  std::cout << CStrUtil::strprintf(format_string.c_str(), file_name.c_str(), time_string) << "\n";
}

// Routine used to Output an Newest File.
void
CUsage::
printNewestFile(CUsageFileSpec *file_spec)
{
  std::string file_name = file_spec->name;

  while (file_name.find("./") != std::string::npos)
    file_name = file_name.substr(2);

  char time_string[256];

  struct tm *tm = localtime(&file_spec->time);

  strftime(time_string, 256, "%a %h %e %H:%M:%S %Z %Y", tm);

  std::cout << CStrUtil::strprintf(format_string.c_str(), file_name.c_str(), time_string) << "\n";
}

void
CUsage::
printDirUsages()
{
  // sort by usage (largest frst)
  auto pdir1 = dir_usage_list.begin();
  auto pdir2 = dir_usage_list.end  ();

  DirUsageList dir_usage_list1;

  for ( ; pdir1 != pdir2; ++pdir1)
    dir_usage_list1.push_back((*pdir1).second);

  dir_usage_list1.sort(CUsageDirUsageCmp());

  //---

  // get largest name length
  auto psdir1 = dir_usage_list1.begin();
  auto psdir2 = dir_usage_list1.end  ();

  uint max_len = 0;

  for ( ; psdir1 != psdir2; ++psdir1)
    if ((*psdir1)->name.size() > max_len)
      max_len = (*psdir1)->name.size();

  //---

  std::cout << "\n";
  std::cout << "Directory Usages :-\n";
  std::cout << "\n";

  psdir1 = dir_usage_list1.begin();

  for ( ; psdir1 != psdir2; ++psdir1) {
    if (! (*psdir1)->leaf) continue;

    int len1 = max_len - (*psdir1)->len;

    UnitsNum unitsSize((*psdir1)->size);

    std::string fmt;

    if      (total_output & TOTAL_G)
      fmt = CStrUtil::strprintf("%s%*.*s  %12.2lfG", (*psdir1)->name.c_str(),
                                len1, len1, "", unitsSize.g());
    else if (total_output & TOTAL_M)
      fmt = CStrUtil::strprintf("%s%*.*s  %12.2lfM", (*psdir1)->name.c_str(),
                                len1, len1, "", unitsSize.m());
    else if (total_output & TOTAL_K)
      fmt = CStrUtil::strprintf("%s%*.*s  %12.2lfK", (*psdir1)->name.c_str(),
                                len1, len1, "", unitsSize.k());
    else if (total_output & TOTAL_B)
      fmt = CStrUtil::strprintf("%s%*.*s  %-10d", (*psdir1)->name.c_str(),
                                len1, len1, "", (*psdir1)->size);

    std::cout << fmt << "\n";
  }

  std::cout << "\n";
}

//---

bool
CUsageDirUsageCmp::
operator()(CUsageDirUsage *dir_usage1, CUsageDirUsage *dir_usage2)
{
  return (dir_usage1->size > dir_usage2->size);
}

//---

// Routine used to update the maximum length of the filenames in a list to be output.
void
CUsage::
setFileSpecLength(CUsageFileSpec *file_spec)
{
  uint name_length = file_spec->name.size();

  if (name_length > max_name_length)
    max_name_length = name_length;
}

// Get the required file date dependant on whether the user wishes to see Access,
// Modified or Changed time.
//
// Uses the global variable 'date_type' to determine which time to return.
time_t
CUsage::
statTime(const struct stat *stat)
{
  if      (date_type == CUsageDateType::LAST_ACCESSED)
    return stat->st_atime;
  else if (date_type == CUsageDateType::LAST_MODIFIED)
    return stat->st_mtime;
  else if (date_type == CUsageDateType::LAST_CHANGED)
    return stat->st_ctime;
  else
    return stat->st_atime;
}

// Output an error Message.
//
// The formatted string will be preceded by the string ' Unix Usage : ' and have
// a newline added to the end before final output.
void
CUsage::
error(const char *format, ...)
{
  va_list args;

  va_start(args, format);

  std::cerr << "Unix Usage : " << CStrUtil::vstrprintf(format, &args) << "\n";

  va_end(args);
}
