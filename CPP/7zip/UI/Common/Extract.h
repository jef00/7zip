// Extract.h

#ifndef ZIP7_INC_EXTRACT_H
#define ZIP7_INC_EXTRACT_H

#include "../../../Windows/FileFind.h"

#include "../../Archive/IArchive.h"

#include "ArchiveExtractCallback.h"
#include "ArchiveOpenCallback.h"
#include "ExtractMode.h"
#include "Property.h"

#include "../Common/LoadCodecs.h"

namespace NExtractOutDirMode {
enum EEnum
{
  k_Direct = 0,
  k_AddArcName,
  k_ReplaceAsterisk
};
}

struct CExtractOptionsBase
{
  CBoolPair ElimDup;

  bool ExcludeDirItems;
  bool ExcludeFileItems;

  bool PathMode_Force;
  bool OverwriteMode_Force;
  NExtract::NPathMode::EEnum PathMode;
  NExtract::NOverwriteMode::EEnum OverwriteMode;
  NExtract::NZoneIdMode::EEnum ZoneMode;
  NExtractOutDirMode::EEnum OutDirMode;

  CExtractNtOptions NtOptions;
  
  FString OutputDir; // normalized : with path separator at the end

  // -sdel for extract commands:
  //   - delete archive (and all its volumes) after it was extracted
  //     without errors and warnings,
  //   - if there were errors or warnings, delete all files that were
  //     created from that archive (we don't keep incomplete files).
  bool DeleteArcAfterExtract;
  UString HashDir;

  CExtractOptionsBase():
      ExcludeDirItems(false),
      ExcludeFileItems(false),
      PathMode_Force(false),
      OverwriteMode_Force(false),
      PathMode(NExtract::NPathMode::kFullPaths),
      OverwriteMode(NExtract::NOverwriteMode::kAsk),
      ZoneMode(NExtract::NZoneIdMode::kNone),
      OutDirMode(NExtractOutDirMode::k_ReplaceAsterisk),
      DeleteArcAfterExtract(false)
      {}
};

/* callback for (DeleteArcAfterExtract) mode.
   The UI knows whether the archive that was just processed is OK. */
struct IArcDeleteCallback
{
  // true, if last processed archive was extracted without errors and warnings
  virtual bool IsLastArcOk() = 0;
  // is called for each archive file (volume) after delete attempt
  virtual void ArcFile_DeleteResult(const UString &path, bool wasDeleted) = 0;
  // is called for each incomplete output file after delete attempt
  virtual void OutFile_DeleteResult(const UString &path, bool wasDeleted) = 0;
  virtual ~IArcDeleteCallback() {}
};

struct CExtractOptions: public CExtractOptionsBase
{
  IArcDeleteCallback *DeleteCallback; // it's required for DeleteArcAfterExtract mode
  bool StdInMode;
  bool StdOutMode;
  bool YesToAll;
  bool TestMode;
  
  // bool ShowDialog;
  // bool PasswordEnabled;
  // UString Password;
  #ifndef Z7_SFX
  CObjectVector<CProperty> Properties;
  #endif

  /*
  #ifdef Z7_EXTERNAL_CODECS
  CCodecs *Codecs;
  #endif
  */

  CExtractOptions():
      DeleteCallback(NULL),
      StdInMode(false),
      StdOutMode(false),
      YesToAll(false),
      TestMode(false)
      {}
};

struct CDecompressStat
{
  UInt64 NumArchives;
  UInt64 UnpackSize;
  UInt64 AltStreams_UnpackSize;
  UInt64 PackSize;
  UInt64 NumFolders;
  UInt64 NumFiles;
  UInt64 NumAltStreams;

  void Clear()
  {
    NumArchives = UnpackSize = AltStreams_UnpackSize = PackSize = NumFolders = NumFiles = NumAltStreams = 0;
  }
};

HRESULT Extract(
    // DECL_EXTERNAL_CODECS_LOC_VARS
    CCodecs *codecs,
    const CObjectVector<COpenType> &types,
    const CIntVector &excludedFormats,
    UStringVector &archivePaths, UStringVector &archivePathsFull,
    const NWildcard::CCensorNode &wildcardCensor,
    const CExtractOptions &options,
    IOpenCallbackUI *openCallback,
    IExtractCallbackUI *extractCallback,
    IFolderArchiveExtractCallback *faeCallback,
    #ifndef Z7_SFX
    IHashCalc *hash,
    #endif
    UString &errorMessage,
    CDecompressStat &st);

#endif
