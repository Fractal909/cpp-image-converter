#include "bmp_image.h"
#include "pack_defines.h"

#include <array>
#include <fstream>
#include <string_view>

using namespace std;

namespace img_lib {

    PACKED_STRUCT_BEGIN BitmapFileHeader{
        // поля заголовка Bitmap File Header
        //const string_view sign = "BM";
        char sign1 = 'B';
        char sign2 = 'M';

        unsigned int total_file_size; //data_size with padding + header size (54)
        const int reserved = 0x00000000;
        unsigned int data_begin_pad_size = 54; //54
    }
        PACKED_STRUCT_END

        PACKED_STRUCT_BEGIN BitmapInfoHeader{
        // поля заголовка Bitmap Info Header
        unsigned int sec_head_size = 40; // 40 second header size
        int image_width;
        int image_height;
        const short num_of_planes = 1;
        const short num_of_bit_pixel = 24;
        const unsigned int compress_type = 0;
        unsigned int data_size;
        const int h_res = 11811; //11811
        const int v_res = 11811; //11811
        const int num_of_colors = 0;
        const int num_of_Important_colors = 0x1000000;
    }
        PACKED_STRUCT_END

        // функция вычисления отступа по ширине
        static int GetBMPStride(int w) {
        return 4 * ((w * 3 + 3) / 4);
    }

    // напишите эту функцию
    bool SaveBMP(const Path& file, const Image& image) {
        ofstream out(file, ios::binary);

        const int w = image.GetWidth();
        const int h = image.GetHeight();
        int stride = GetBMPStride(w);

        BitmapFileHeader bfh;
        bfh.total_file_size = stride * h + 54;
        out.write(reinterpret_cast<char*>(&bfh), sizeof(bfh));

        BitmapInfoHeader bih;
        bih.image_width = w;
        bih.image_height = h;
        bih.data_size = stride * h;
        out.write(reinterpret_cast<char*>(&bih), sizeof(bih));


        std::vector<char> buff(stride);

        for (int y = h - 1; y >= 0; --y) {
            const Color* line = image.GetLine(y);
            for (int x = 0; x < w; ++x) {
                buff[x * 3 + 0] = static_cast<char>(line[x].b);
                buff[x * 3 + 1] = static_cast<char>(line[x].g);
                buff[x * 3 + 2] = static_cast<char>(line[x].r);
            }
            out.write(buff.data(), stride/*w * 3*/);
        }

        return out.good();
    }

    // напишите эту функцию
    Image LoadBMP(const Path& file) {

        ifstream ifs(file, ios::binary);

        if (!ifs) {
            return Image();
        }

        int w, h, data_size;

        char sign1;
        char sign2;

        ifs.read(&sign1, 1);
        ifs.read(&sign2, 1);

        if (sign1 != 'B' || sign2 != 'M') {
            return Image();
        }

        ifs.seekg(18);
        ifs.read(reinterpret_cast<char*>(&w), sizeof(w));
        ifs.read(reinterpret_cast<char*>(&h), sizeof(h));

        ifs.seekg(34);
        ifs.read(reinterpret_cast<char*>(&data_size), sizeof(data_size));

        ifs.seekg(54);

        Image result(w, h, Color::Black());

        int stride = GetBMPStride(w);
        std::vector<char> buff(stride);

        for (int y = h - 1; y >= 0; --y) {
            Color* line = result.GetLine(y);

            ifs.read(buff.data(), stride);

            for (int x = 0; x < w; ++x) {
                line[x].b = static_cast<byte>(buff[x * 3 + 0]);
                line[x].g = static_cast<byte>(buff[x * 3 + 1]);
                line[x].r = static_cast<byte>(buff[x * 3 + 2]);
            }
        }
        return result;
    }

}  // namespace img_lib