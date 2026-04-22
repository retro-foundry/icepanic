#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum {
    IAB_FLAG_ROW_INTERLEAVED = 1,
    IAB_FLAG_DELTA_XOR = 2,
};

static uint16_t read_u16_le(const uint8_t *p) {
    return (uint16_t)(p[0] | (p[1] << 8));
}

static uint32_t read_u32_le(const uint8_t *p) {
    return (uint32_t)(p[0] | (p[1] << 8) | (p[2] << 16) | (p[3] << 24));
}

static uint32_t fnv1a32(const uint8_t *data, size_t size) {
    uint32_t h = 2166136261u;
    for (size_t i = 0; i < size; ++i) {
        h ^= data[i];
        h *= 16777619u;
    }
    return h;
}

static int parse_int_arg(const char *text, int *out_value) {
    char *end = NULL;
    long v = strtol(text, &end, 10);
    if (end == text || *end != '\0') {
        return 0;
    }
    if (v < 0 || v > 1000000L) {
        return 0;
    }
    *out_value = (int)v;
    return 1;
}

static int write_file(const char *path, const uint8_t *data, size_t size) {
    FILE *f = fopen(path, "wb");
    if (f == NULL) {
        return 0;
    }
    if (fwrite(data, 1, size, f) != size) {
        fclose(f);
        return 0;
    }
    fclose(f);
    return 1;
}

int main(int argc, char **argv) {
    const char *bundle_path = NULL;
    const char *out_frame_path = NULL;
    int frame_index = 0;
    FILE *f = NULL;
    uint8_t *file_data = NULL;
    long file_size_long = 0;
    size_t file_size = 0;

    if (argc < 2) {
        fprintf(stderr, "Usage: %s <bundle.iab> [frame_index] [out_frame.bpl]\n", argv[0]);
        return 1;
    }

    bundle_path = argv[1];
    if (argc >= 3 && !parse_int_arg(argv[2], &frame_index)) {
        fprintf(stderr, "Invalid frame_index: %s\n", argv[2]);
        return 1;
    }
    if (argc >= 4) {
        out_frame_path = argv[3];
    }

    f = fopen(bundle_path, "rb");
    if (f == NULL) {
        fprintf(stderr, "Could not open bundle: %s\n", bundle_path);
        return 1;
    }
    if (fseek(f, 0, SEEK_END) != 0) {
        fclose(f);
        fprintf(stderr, "fseek failed\n");
        return 1;
    }
    file_size_long = ftell(f);
    if (file_size_long <= 0) {
        fclose(f);
        fprintf(stderr, "Empty or invalid bundle size\n");
        return 1;
    }
    file_size = (size_t)file_size_long;
    if (fseek(f, 0, SEEK_SET) != 0) {
        fclose(f);
        fprintf(stderr, "fseek reset failed\n");
        return 1;
    }

    file_data = (uint8_t *)malloc(file_size);
    if (file_data == NULL) {
        fclose(f);
        fprintf(stderr, "Out of memory\n");
        return 1;
    }
    if (fread(file_data, 1, file_size, f) != file_size) {
        free(file_data);
        fclose(f);
        fprintf(stderr, "Could not read full bundle\n");
        return 1;
    }
    fclose(f);

    if (file_size < 28u + 64u) {
        free(file_data);
        fprintf(stderr, "Bundle too small\n");
        return 1;
    }
    if (memcmp(file_data, "IAB1", 4) != 0) {
        free(file_data);
        fprintf(stderr, "Invalid magic; expected IAB1\n");
        return 1;
    }

    {
        const uint16_t version = read_u16_le(file_data + 4);
        const uint16_t flags = read_u16_le(file_data + 6);
        const uint16_t width = read_u16_le(file_data + 8);
        const uint16_t height = read_u16_le(file_data + 10);
        const uint16_t bitplanes = read_u16_le(file_data + 12);
        const uint32_t frame_count = read_u32_le(file_data + 16);
        const uint32_t frame_size = read_u32_le(file_data + 20);
        const uint16_t palette_count = read_u16_le(file_data + 24);
        const size_t header_size = 28u + (size_t)palette_count * 2u;
        const size_t data_size = (size_t)frame_count * (size_t)frame_size;
        const uint8_t *frame_base = file_data + header_size;
        uint8_t *frame_buf = NULL;
        size_t nonzero = 0;
        uint32_t hash = 0;

        if (version != 1) {
            free(file_data);
            fprintf(stderr, "Unsupported version: %u\n", (unsigned)version);
            return 1;
        }
        if (palette_count != 32) {
            free(file_data);
            fprintf(stderr, "Unsupported palette_count: %u\n", (unsigned)palette_count);
            return 1;
        }
        if (frame_count == 0 || frame_size == 0) {
            free(file_data);
            fprintf(stderr, "Invalid frame_count/frame_size\n");
            return 1;
        }
        if ((size_t)frame_index >= (size_t)frame_count) {
            free(file_data);
            fprintf(stderr, "frame_index out of range: %d (max %u)\n", frame_index, (unsigned)(frame_count - 1));
            return 1;
        }
        if (header_size + data_size > file_size) {
            free(file_data);
            fprintf(stderr, "Bundle truncated: expected at least %zu bytes, got %zu\n", header_size + data_size, file_size);
            return 1;
        }

        frame_buf = (uint8_t *)malloc(frame_size);
        if (frame_buf == NULL) {
            free(file_data);
            fprintf(stderr, "Out of memory for frame buffer\n");
            return 1;
        }

        memcpy(frame_buf, frame_base, frame_size);
        if ((flags & IAB_FLAG_DELTA_XOR) != 0) {
            for (int i = 1; i <= frame_index; ++i) {
                const uint8_t *delta = frame_base + (size_t)i * (size_t)frame_size;
                for (uint32_t j = 0; j < frame_size; ++j) {
                    frame_buf[j] ^= delta[j];
                }
            }
        } else {
            memcpy(frame_buf, frame_base + (size_t)frame_index * (size_t)frame_size, frame_size);
        }

        for (uint32_t i = 0; i < frame_size; ++i) {
            if (frame_buf[i] != 0) {
                ++nonzero;
            }
        }
        hash = fnv1a32(frame_buf, frame_size);

        printf("IAB1 bundle: %s\n", bundle_path);
        printf("  version=%u flags=%u (%s, %s)\n",
            (unsigned)version,
            (unsigned)flags,
            ((flags & IAB_FLAG_ROW_INTERLEAVED) != 0) ? "row-interleaved" : "plane-major",
            ((flags & IAB_FLAG_DELTA_XOR) != 0) ? "delta-xor" : "raw");
        printf("  width=%u height=%u bitplanes=%u\n", (unsigned)width, (unsigned)height, (unsigned)bitplanes);
        printf("  frame_count=%u frame_size=%u\n", (unsigned)frame_count, (unsigned)frame_size);
        printf("  selected_frame=%d nonzero_bytes=%zu fnv1a32=0x%08X\n", frame_index, nonzero, (unsigned)hash);

        if (out_frame_path != NULL) {
            if (!write_file(out_frame_path, frame_buf, frame_size)) {
                free(frame_buf);
                free(file_data);
                fprintf(stderr, "Could not write output frame: %s\n", out_frame_path);
                return 1;
            }
            printf("  wrote_frame=%s\n", out_frame_path);
        }

        free(frame_buf);
    }

    free(file_data);
    return 0;
}

