#include <ffmpeg_cap.h>
#include "dllloader.hpp"

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

    const int width = 640;
    const int height = 480;
    const int fps = 30;
    const int count = 256;
    const int pix_fmt = ff_get_pix_fmt_f("gray");
    unsigned char* data = new unsigned char[width*height];
    auto writer = ff_writer_create_f("gray.mp4", 0, fps, width, height, pix_fmt);

    if (writer)
    {
        for (int i = 0; i < count; ++i)
        {
            memset(data, i, width*height);
            ff_writer_write_f(writer, data);
        }
    }

    delete[] data;
    ff_writer_release_f(&writer);
}
