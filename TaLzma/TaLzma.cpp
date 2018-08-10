// TaValidation.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include <string>

#include <base/file/file.h>
#include <base/file/filedata.h>
#include <common/Buffer.h>
#include <lzmahelper.h>
#include <time.h>

HANDLE g_hFile = INVALID_HANDLE_VALUE;

int _tmain(int argc, _TCHAR *argv[]) {
  if (argc < 2) {
    _tprintf(_T("Put the file in the programme!\n"));
    _tscanf(_T("%*c"));
    return 0;
  }

  for (int i = 1; i < argc; i++) {
    base::CFile file;
    base::CFileData fd;
    if (!file.Open(base::kFileRead, argv[i])) {
      _tprintf(_T("Open file \"%s\" error!\n"), argv[i]);
      continue;
    }
    if (!fd.Read(file)) {
      _tprintf(_T("Read file \"%s\" error!\n"), argv[i]);
      continue;
    }

    _tprintf(_T("Start compress \"%s\" !\n"), argv[i]);

    BYTE props[LZMA_PROPS_SIZE] = {0};
    size_t propsize = LZMA_PROPS_SIZE;
    size_t osize = fd.GetSize();
    size_t csize = osize * 3;
    BYTE *bcom = (BYTE *)malloc(csize);
    if (!bcom) {
      _tprintf(_T("malloc failed!\n"));
      continue;
    }

    int status;
    /* *outPropsSize must be = 5 */
    status = LzmaCompress(bcom, &csize, fd.GetData(), fd.GetSize(), props,
                          &propsize, 5, /* 0 <= level <= 9, default = 5 */
                          65536,        /* default = (1 << 24) */
                          3,            /* 0 <= lc <= 8, default = 3  */
                          0,            /* 0 <= lp <= 4, default = 0  */
                          2,            /* 0 <= pb <= 4, default = 2  */
                          32,           /* 5 <= fb <= 273, default = 32 */
                          1);           /* 1 or 2, default = 2 */
    if (status != SZ_OK) {
      free(bcom);
      _tprintf(_T("Compress \"%s\" error! Error code: %d\n"), argv[i], status);
      continue;
    }

    CBuffer buf;
    TCHAR path[MAX_PATH];
    buf.Write((unsigned char *)"TALZMA", 5);
    buf.Write(props, LZMA_PROPS_SIZE);
    buf.Write((unsigned char *)&osize, 4);
    buf.Write((unsigned char *)&csize, 4);
    buf.Write(bcom, csize);

    free(bcom);
    lstrcpy(path, argv[i]);
    lstrcat(path, _T(".talzma"));

    if (buf.FileWrite(path)) {
      _tprintf(_T("Finish \"%s\".\n"), argv[i]);
    } else {
      _tprintf(_T("Write file \"%s\" error!\n"), argv[i]);
    }

  }

  _tscanf(_T("%*c"));
  return 0;
}
