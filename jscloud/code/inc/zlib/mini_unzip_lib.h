#ifndef mini_unzip_lib_914DA9AB_0265_4205_B59E_7D7B950CE839
#define mini_unzip_lib_914DA9AB_0265_4205_B59E_7D7B950CE839

#ifdef __cplusplus
extern "C" {
#endif

// unzip a .zip file to an existing folder. return 0 when succeeded, but non zero when failed.
// to use this lib, you have to include zconf.h & zlib.h, and link zlib.lib.
int mini_unzip_lib( const char * zipfilename, const char * directroy );

#ifdef __cplusplus
};
#endif

#endif