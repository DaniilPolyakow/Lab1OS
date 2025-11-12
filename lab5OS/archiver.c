#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <utime.h>
#include <errno.h>

#define BUF_SZ 4096
#define FNAME_MAX 256

typedef struct {
    char fname[FNAME_MAX];
    off_t fsize;
    mode_t fmode;
    uid_t fuid;
    gid_t fgid;
    struct timespec acc_time;
    struct timespec mod_time;
} FileHeader;

static void show_help() {
    puts("Usage:");
    puts("  ./archiver archive_name -i file       Add file");
    puts("  ./archiver archive_name -e file       Extract and remove file");
    puts("  ./archiver archive_name -s            Show archive info");
    puts("  ./archiver -h                         Show help");
}

static int read_hdr(int fd, FileHeader *hdr) {
    ssize_t r = read(fd, hdr, sizeof(*hdr));
    if (r == 0) return 0;
    if (r != sizeof(*hdr)) {
        perror("header read error");
        return -1;
    }
    return 1;
}

static int write_hdr(int fd, const FileHeader *hdr) {
    if (write(fd, hdr, sizeof(*hdr)) != sizeof(*hdr)) {
        perror("header write error");
        return -1;
    }
    return 0;
}

static int archive_add(const char *arch, const char *fname) {
    struct stat st;
    if (stat(fname, &st) < 0) {
        perror("stat file");
        return 1;
    }

    int fd_arch = open(arch, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (fd_arch < 0) {
        perror("open archive");
        return 1;
    }

    FileHeader h = {0};
    strncpy(h.fname, fname, FNAME_MAX - 1);
    h.fsize = st.st_size;
    h.fmode = st.st_mode;
    h.fuid = st.st_uid;
    h.fgid = st.st_gid;
    h.acc_time = st.st_atim;
    h.mod_time = st.st_mtim;

    if (write_hdr(fd_arch, &h) < 0) {
        close(fd_arch);
        return 1;
    }

    int fd_src = open(fname, O_RDONLY);
    if (fd_src < 0) {
        perror("open input");
        close(fd_arch);
        return 1;
    }

    char buffer[BUF_SZ];
    ssize_t n;
    while ((n = read(fd_src, buffer, BUF_SZ)) > 0) {
        if (write(fd_arch, buffer, n) != n) {
            perror("write data");
            close(fd_src);
            close(fd_arch);
            return 1;
        }
    }

    if (n < 0) perror("read source");

    close(fd_src);
    close(fd_arch);
    return 0;
}

static int archive_extract(const char *arch, const char *fname) {
    int fd_a = open(arch, O_RDWR);
    if (fd_a < 0) {
        perror("open archive");
        return 1;
    }

    struct stat st_a;
    if (fstat(fd_a, &st_a) < 0) {
        perror("fstat");
        close(fd_a);
        return 1;
    }

    if (st_a.st_size == 0) {
        puts("Archive empty");
        close(fd_a);
        return 1;
    }

    char tmp_path[] = "/tmp/arch_tempXXXXXX";
    int fd_tmp = mkstemp(tmp_path);
    if (fd_tmp < 0) {
        perror("temp file");
        close(fd_a);
        return 1;
    }

    off_t offset = 0;
    int found = 0;
    FileHeader hdr;

    while (offset < st_a.st_size) {
        ssize_t got = pread(fd_a, &hdr, sizeof(hdr), offset);
        if (got != sizeof(hdr)) break;

        off_t data_start = offset + sizeof(hdr);
        off_t data_end = data_start + hdr.fsize;

        if (!strcmp(hdr.fname, fname)) {
            int fd_out = open(fname, O_CREAT | O_TRUNC | O_WRONLY, hdr.fmode);
            if (fd_out < 0) {
                perror("open output");
                break;
            }

            char buf[BUF_SZ];
            off_t left = hdr.fsize;
            while (left > 0) {
                ssize_t chunk = (left > BUF_SZ) ? BUF_SZ : left;
                ssize_t rd = pread(fd_a, buf, chunk, data_start);
                if (rd <= 0) break;
                if (write(fd_out, buf, rd) != rd) {
                    perror("write extracted");
                    break;
                }
                left -= rd;
                data_start += rd;
            }
            close(fd_out);

            struct timespec ts[2] = {hdr.acc_time, hdr.mod_time};
            utimensat(AT_FDCWD, fname, ts, 0);
            chmod(fname, hdr.fmode);
            found = 1;
            printf("Extracted: %s\n", fname);
        } else {
            write_hdr(fd_tmp, &hdr);
            char buf[BUF_SZ];
            off_t remain = hdr.fsize;
            while (remain > 0) {
                ssize_t blk = (remain > BUF_SZ) ? BUF_SZ : remain;
                ssize_t rd = pread(fd_a, buf, blk, data_start);
                if (rd <= 0) break;
                write(fd_tmp, buf, rd);
                data_start += rd;
                remain -= rd;
            }
        }
        offset = data_end;
    }

    close(fd_a);
    close(fd_tmp);

    if (!found) {
        printf("File '%s' not found\n", fname);
        unlink(tmp_path);
        return 1;
    }

    if (rename(tmp_path, arch) < 0) {
        perror("rename failed, manual copy");
        int new_a = open(arch, O_WRONLY | O_TRUNC | O_CREAT, 0644);
        int t = open(tmp_path, O_RDONLY);
        if (new_a < 0 || t < 0) {
            perror("open copy files");
            return 1;
        }
        char b[BUF_SZ];
        ssize_t n;
        while ((n = read(t, b, BUF_SZ)) > 0)
            write(new_a, b, n);
        close(new_a);
        close(t);
        unlink(tmp_path);
    }

    return 0;
}

static int archive_list(const char *arch) {
    int fd = open(arch, O_RDONLY);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    struct stat st;
    if (fstat(fd, &st) < 0) {
        perror("fstat");
        close(fd);
        return 1;
    }

    if (st.st_size == 0) {
        printf("Archive '%s' is empty\n", arch);
        close(fd);
        return 0;
    }

    printf("Contents of '%s':\n", arch);
    printf("%-25s %-10s %-6s\n", "Name", "Size", "Mode");

    off_t off = 0;
    FileHeader hdr;
    while (off < st.st_size) {
        if (pread(fd, &hdr, sizeof(hdr), off) != sizeof(hdr))
            break;
        printf("%-25s %-10ld %04o\n", hdr.fname, (long)hdr.fsize, hdr.fmode & 0777);
        off += sizeof(hdr) + hdr.fsize;
    }

    close(fd);
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        show_help();
        return 1;
    }

    if (!strcmp(argv[1], "-h") || !strcmp(argv[1], "--help")) {
        show_help();
        return 0;
    }

    if (argc < 3) {
        show_help();
        return 1;
    }

    const char *arch = argv[1];
    const char *opt = argv[2];

    if ((!strcmp(opt, "-i") || !strcmp(opt, "--input")) && argc > 3)
        return archive_add(arch, argv[3]);
    if ((!strcmp(opt, "-e") || !strcmp(opt, "--extract")) && argc > 3)
        return archive_extract(arch, argv[3]);
    if (!strcmp(opt, "-s") || !strcmp(opt, "--stat"))
        return archive_list(arch);

    show_help();
    return 1;
}
