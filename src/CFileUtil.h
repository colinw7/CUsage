#ifndef CFileUtil_H
#define CFileUtil_H

#include <CFileType.h>

#include <sys/stat.h>

namespace CFileUtil {
  bool isLink(const std::string &filename) {
    struct stat file_stat;

    if (::stat(filename.c_str(), &file_stat) < 0)
      return false;

    return S_ISLNK(file_stat.st_mode);
  }

  CFileType getType(const std::string &filename) {
    struct stat file_stat;

    if (::stat(filename.c_str(), &file_stat) < 0)
      return CFILE_TYPE_NONE;

    if (S_ISFIFO(file_stat.st_mode)) return CFILE_TYPE_INODE_FIFO;
    if (S_ISCHR (file_stat.st_mode)) return CFILE_TYPE_INODE_CHR ;
    if (S_ISDIR (file_stat.st_mode)) return CFILE_TYPE_INODE_DIR ;
    if (S_ISBLK (file_stat.st_mode)) return CFILE_TYPE_INODE_BLK ;
    if (S_ISREG (file_stat.st_mode)) return CFILE_TYPE_INODE_REG ;
    if (S_ISLNK (file_stat.st_mode)) return CFILE_TYPE_INODE_LNK ;
    if (S_ISSOCK(file_stat.st_mode)) return CFILE_TYPE_INODE_SOCK;

    return CFILE_TYPE_NONE;
  }

  std::string getTypeMime(CFileType type) {
    std::string type_str = "??";

    switch (type) {
      case CFILE_TYPE_INODE_FIFO : type_str = "inode/fifo"               ; break;
      case CFILE_TYPE_INODE_CHR  : type_str = "inode/chardevice"         ; break;
      case CFILE_TYPE_INODE_DIR  : type_str = "inode/directory"          ; break;
      case CFILE_TYPE_INODE_BLK  : type_str = "inode/blockdevice"        ; break;
      case CFILE_TYPE_INODE_REG  : type_str = "text/plain"               ; break;
      case CFILE_TYPE_INODE_LNK  : type_str = "inode/link"               ; break;
      case CFILE_TYPE_INODE_SOCK : type_str = "inode/socket"             ; break;

      case CFILE_TYPE_TEXT_PLAIN : type_str = "text/plain"               ; break;
      case CFILE_TYPE_TEXT_BINARY: type_str = "application/octet-stream" ; break;
      case CFILE_TYPE_TEXT_HTML  : type_str = "text/html"                ; break;
      case CFILE_TYPE_TEXT_XML   : type_str = "text/xml"                 ; break;
      case CFILE_TYPE_TEXT_TAR   : type_str = "application/x-tar"        ; break;
      case CFILE_TYPE_TEXT_TGZ   : type_str = "application/x-tar-gz"     ; break;
      case CFILE_TYPE_TEXT_GZIP  : type_str = "application/x-compressed" ; break;

      case CFILE_TYPE_IMAGE_BMP  : type_str = "image/x-ms-bmp"           ; break;
      case CFILE_TYPE_IMAGE_EPS  : type_str = "application/postscript"   ; break;
      case CFILE_TYPE_IMAGE_GIF  : type_str = "image/gif"                ; break;
      case CFILE_TYPE_IMAGE_ICO  : type_str = "image/x-icon"             ; break;
      case CFILE_TYPE_IMAGE_IFF  : type_str = "image/iff"                ; break;
      case CFILE_TYPE_IMAGE_JPG  : type_str = "image/jpeg"               ; break;
      case CFILE_TYPE_IMAGE_PCX  : type_str = "image/pcx"                ; break;
      case CFILE_TYPE_IMAGE_PNG  : type_str = "image/png"                ; break;
      case CFILE_TYPE_IMAGE_PPM  : type_str = "image/x-portable-pixmap"  ; break;
      case CFILE_TYPE_IMAGE_PS   : type_str = "application/postscript"   ; break;
      case CFILE_TYPE_IMAGE_PSP  : type_str = "image/psp"                ; break;
      case CFILE_TYPE_IMAGE_SGI  : type_str = "image/x-rgb"              ; break;
      case CFILE_TYPE_IMAGE_SVG  : type_str = "image/svg+html"           ; break;
      case CFILE_TYPE_IMAGE_TGA  : type_str = "image/tga"                ; break;
      case CFILE_TYPE_IMAGE_TIF  : type_str = "image/tiff"               ; break;
      case CFILE_TYPE_IMAGE_XBM  : type_str = "image/x-xbitmap"          ; break;
      case CFILE_TYPE_IMAGE_XPM  : type_str = "image/x-xpixmap"          ; break;
      case CFILE_TYPE_IMAGE_XWD  : type_str = "image/x-xwindowdump"      ; break;

      case CFILE_TYPE_APP_EXEC   : type_str = "application/octet-stream" ; break;
      case CFILE_TYPE_APP_CORE   : type_str = "application/x-coredump"   ; break;
      case CFILE_TYPE_APP_SH     : type_str = "application/x-shellscript"; break;
      case CFILE_TYPE_APP_CSH    : type_str = "application/x-shellscript"; break;
      case CFILE_TYPE_APP_KSH    : type_str = "application/x-shellscript"; break;
      case CFILE_TYPE_APP_TCL    : type_str = "text/x-tcl"               ; break;

      // TODO: Python, ...

      case CFILE_TYPE_SOUND_WAV  : type_str = "audio/x-wav"              ; break;

      default: break;
    }

    return type_str;
  }
}

#endif
