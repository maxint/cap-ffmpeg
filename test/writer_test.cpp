#include <windows.h>
#include <strsafe.h>
#include <ffmpeg_cap.h>

class DynamicLibraryLoader
{
public:
    DynamicLibraryLoader() : _hModule(NULL) {}
    DynamicLibraryLoader(const char* libname)
    {
        load(libname);
    }
    ~DynamicLibraryLoader()
    {
        free();
    }
    template <class T> T getProcAddress(const char* funcname) const
    {
        return (T) GetProcAddress(_hModule, funcname);
    }
    bool isLoaded() { return _hModule != NULL; }
    void load(const char* libname)
    {
        free();
        _hModule = LoadLibrary(libname);
    }
    void free()
    {
        if (_hModule)
        {
            FreeLibrary(_hModule);
            _hModule = NULL;
        }
    }
private:
    HMODULE _hModule;
};

#define CV_GET_PROC_ADDRESS(loader, func) (loader).getProcAddress<decltype(&func)>(#func)

static DynamicLibraryLoader s_dll("ffmpeg_cap.dll");

static auto ff_cap_create_f      = CV_GET_PROC_ADDRESS(s_dll, ff_cap_create);
static auto ff_cap_release_f     = CV_GET_PROC_ADDRESS(s_dll, ff_cap_release);
static auto ff_cap_get_f         = CV_GET_PROC_ADDRESS(s_dll, ff_cap_get);
static auto ff_cap_set_f         = CV_GET_PROC_ADDRESS(s_dll, ff_cap_set);
static auto ff_cap_grab_f        = CV_GET_PROC_ADDRESS(s_dll, ff_cap_grab);
static auto ff_cap_retrieve_f    = CV_GET_PROC_ADDRESS(s_dll, ff_cap_retrieve);

static auto ff_writer_create_f   = CV_GET_PROC_ADDRESS(s_dll, ff_writer_create);
static auto ff_writer_release_f  = CV_GET_PROC_ADDRESS(s_dll, ff_writer_release);
static auto ff_writer_write_f    = CV_GET_PROC_ADDRESS(s_dll, ff_writer_write);
static auto ff_get_buffer_size_f = CV_GET_PROC_ADDRESS(s_dll, ff_get_buffer_size);
static auto ff_get_pix_fmt_f     = CV_GET_PROC_ADDRESS(s_dll, ff_get_pix_fmt);
static auto ff_get_pix_fmt_name_f= CV_GET_PROC_ADDRESS(s_dll, ff_get_pix_fmt_name);


int main(int argc, char* argv[])
{
    if (ff_cap_create_f == 0 || ff_cap_release_f == 0 || ff_cap_get_f == 0 ||
        ff_cap_set_f == 0 || ff_cap_grab_f == 0 || ff_cap_retrieve_f == 0 ||
        ff_writer_create_f == 0 || ff_writer_release_f == 0 || ff_writer_write_f == 0 ||
        ff_get_buffer_size_f == 0 || ff_get_pix_fmt_f == 0 || ff_get_pix_fmt_name_f == 0)
    {
        MessageBox(NULL, TEXT("Can not find enough process address in ffmpeg_cap.dll"), TEXT("Error"), MB_OK | MB_ICONERROR);
        return -1;
    }

    if (argc != 2)
    {
        MessageBox(NULL, TEXT("usage: <EXE> <VIDEO FILE>"), TEXT("Help"), MB_OK | MB_ICONSTOP);
        return -1;
    }

    auto reader = ff_cap_create_f(argv[1]);
    const auto count = (int) ff_cap_get_f(reader, FFMPEG_PROP_FRAME_COUNT);
    const auto width = (int) ff_cap_get_f(reader, FFMPEG_PROP_FRAME_WIDTH);
    const auto height = (int) ff_cap_get_f(reader, FFMPEG_PROP_FRAME_HEIGHT);
    const auto pix_fmt = (int) ff_cap_get_f(reader, FFMPEG_PROP_PIXEL_FORMAT);
    const auto fourcc = (int) ff_cap_get_f(reader, FFMPEG_PROP_FOURCC);
    const auto fps = ff_cap_get_f(reader, FFMPEG_PROP_FPS);
    const auto pix_fmt_name = ff_get_pix_fmt_name_f(pix_fmt);

    char szDstFileName[MAX_PATH];
#if 1
    StringCchPrintf(szDstFileName, MAX_PATH, "%s_dump.mp4", argv[1]);
    auto writer = ff_writer_create_f(szDstFileName, 0, fps, width, height, pix_fmt);
    //auto writer = ff_writer_create_f(szDstFileName, FFMPEG_FCC('H264'), fps, width, height, pix_fmt);
#else
    StringCchPrintf(szDstFileName, MAX_PATH, "%s_dump.avi", argv[1]);
    auto writer = ff_writer_create_f(szDstFileName, FFMPEG_FCC('XVID'), fps, width, height, pix_fmt);
#endif

    if (writer)
    {
        const unsigned char* data[4];
        int step[4];
        for (int i = 0; i < count; ++i)
        {
            printf("writing %d/%d frame\n", i, count);
            if (ff_cap_grab_f(reader) && ff_cap_retrieve_f(reader, data, step))
        	    ff_writer_write_f(writer, data[0]);
            else
                printf("    no data retrieved\n");
        }
    }

    ff_writer_release_f(&writer);
    ff_cap_release_f(&reader);
}