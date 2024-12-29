#define _CRT_SECURE_NO_WARNINGS
#include <img_lib.h>
#include <jpeg_image.h>
#include <ppm_image.h>
#include <bmp_image.h>

#include <filesystem>
#include <string_view>
#include <iostream>

using namespace std;

enum class Format {
    UNKNOWN,
    JPEG,
    PPM,
    BMP
};

Format GetFormatByExtension(const img_lib::Path& input_file) {
    const string ext = input_file.extension().string();
    if (ext == ".jpg"sv || ext == ".jpeg"sv) {
        return Format::JPEG;
    }

    if (ext == ".ppm"sv) {
        return Format::PPM;
    }

    if (ext == ".bmp"sv) {
        return Format::BMP;
    }

    return Format::UNKNOWN;
}

class ImageFormatInterface {
public:
    virtual bool SaveImage(const img_lib::Path& file, const img_lib::Image& image) const = 0;
    virtual img_lib::Image LoadImage(const img_lib::Path& file) const = 0;
};

class PPM_Interface : ImageFormatInterface {
public:
    bool SaveImage(const img_lib::Path& file, const img_lib::Image& image) const override {
        return img_lib::SavePPM(file, image);
    }

    img_lib::Image LoadImage(const img_lib::Path& file) const override {
        return img_lib::LoadPPM(file);
    }
};

class JPEG_Interface : ImageFormatInterface {
public:
    bool SaveImage(const img_lib::Path& file, const img_lib::Image& image) const override {
        return img_lib::SaveJPEG(file, image);
    }

    img_lib::Image LoadImage(const img_lib::Path& file) const override {
        return img_lib::LoadJPEG(file);
    }
};

class BMP_Interface : ImageFormatInterface {
public:
    bool SaveImage(const img_lib::Path& file, const img_lib::Image& image) const override {
        return img_lib::SaveBMP(file, image);
    }

    img_lib::Image LoadImage(const img_lib::Path& file) const override {
        return img_lib::LoadBMP(file);
    }
};

ImageFormatInterface* GetFormatInterface(const img_lib::Path& path) {

    Format format = GetFormatByExtension(path);

    if (format == Format::PPM) {
        return reinterpret_cast<ImageFormatInterface*>(new PPM_Interface());
    }
    else if (format == Format::JPEG) {
        return reinterpret_cast<ImageFormatInterface*>(new JPEG_Interface());
    }
    else if (format == Format::BMP) {
        return reinterpret_cast<ImageFormatInterface*>(new BMP_Interface());
    }
    else /*(format == Format::UNKNOWN)*/ {
        return nullptr;
    }
}

int main(int argc, const char** argv) {
    if (argc != 3) {
        cerr << "Usage: "sv << argv[0] << " <in_file> <out_file>"sv << endl;
        return 1;
    }

    img_lib::Path in_path = argv[1];
    img_lib::Path out_path = argv[2];

    ImageFormatInterface* in_file = GetFormatInterface(in_path);
    if (in_file == nullptr) {
        return 2;
    }
    ImageFormatInterface* out_file = GetFormatInterface(out_path);
    if (in_file == nullptr) {
        return 3;
    }

    out_file->SaveImage(out_path, in_file->LoadImage(in_path));
    cout << "Successfully converted"sv << endl;
}