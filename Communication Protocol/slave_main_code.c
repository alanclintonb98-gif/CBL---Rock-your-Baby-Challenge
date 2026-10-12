    if (strcmp(s->status, "PASS: reply = sent + 1") == 0 ||
        strcmp(s->status, "COMPLETE: ALL PASSED") == 0) colors[5] = RGB_GREEN;
    if (strncmp(s->status, "FAIL", 4) == 0) colors[5] = RGB_RED;
    snprintf(rows[6], sizeof(rows[6]), "%s: %" PRIu32,
             is_master ? "Verified" : "Processed", s->count);
    snprintf(rows[7], sizeof(rows[7]), "%s", is_master ? "Checks actual I2C reply" : "Reply = received + 1");
    snprintf(rows[8], sizeof(rows[8]), "%s", is_master ? "Ctrl+C to stop" : "Waiting for master...");
    for (unsigned i = 0; i < 9; ++i) {
        if (strcmp(previous[i], rows[i]) == 0) continue;
        char padded[29];
        snprintf(padded, sizeof(padded), "%-28s", rows[i]);
        displayDrawString(&screen, font, 8, (uint16_t)(22 + 25 * i),
                          (uint8_t *)padded, colors[i]);
        memcpy(previous[i], rows[i], sizeof(previous[i]));
    }
}

/* Send a local snapshot to the LCD process without blocking I2C. */
static void update_display(const char *status)
{
    snprintf(screen_state.status, sizeof(screen_state.status), "%s", status);
    if (screen_pipe < 0) return;

    ssize_t n = write(screen_pipe, &screen_state, sizeof(screen_state));
    if (n < 0 && errno != EAGAIN && errno != EWOULDBLOCK && errno != EINTR) {
        perror("[DISPLAY] update failed; terminal/I2C continue");
        close(screen_pipe);
        screen_pipe = -1;
    }
}

/* The child draws the LCD; the parent continues handling I2C. */
static int start_display(int is_master, unsigned address, int lock_fd)
{
    const char *path = getenv("RYB_FONT");
    if (!path || !*path)
        path = "/home/student/libpynq-5EWC0-2023-v0.2.6/fonts/ILGH16XB.FNT";
    if (access(path, R_OK) != 0) {
        fprintf(stderr, "[DISPLAY] Font missing: %s\n", path);
        return 1;
    }
    InitFontx(font, path, "");
    uint8_t glyph[FontxGlyphBufSize], width = 0, height = 0;
    if (!GetFontx(font, 'A', glyph, &width, &height) || width != 8 || height != 16) {
        fprintf(stderr, "[DISPLAY] Need the 8x16 ILGH16XB.FNT font.\n");
        CloseFontx(&font[0]);
        return 1;
    }
    int pipe_fd[2];
    if (pipe(pipe_fd) != 0) { perror("display pipe"); CloseFontx(&font[0]); return 1; }
    if (fcntl(pipe_fd[1], F_SETFL, O_NONBLOCK) == -1) {
        perror("display nonblocking pipe");
        close(pipe_fd[0]); close(pipe_fd[1]); CloseFontx(&font[0]); return 1;
    }
    printf("[DISPLAY] Initializing the TU/e LCD...\n");
    display_init(&screen);
    displaySetFontDirection(&screen, TEXT_DIRECTION0);
    displaySetFontFill(&screen, RGB_BLACK);
    displayFillScreen(&screen, RGB_BLACK);
    screen_state.address = address;
    snprintf(screen_state.status, sizeof(screen_state.status), "STARTING");
    draw_display(&screen_state, is_master);
    signal(SIGPIPE, SIG_IGN);
    pid_t parent_pid = getpid();
    screen_pid = fork();
    if (screen_pid == 0) {
        signal(SIGINT, SIG_DFL);
        signal(SIGTERM, SIG_DFL);
        if (prctl(PR_SET_PDEATHSIG, SIGTERM) != 0 || getppid() != parent_pid) _exit(1);
        close(lock_fd);
        close(pipe_fd[1]);
        screen_state_t update;
        for (;;) {
            size_t used = 0;
            while (used < sizeof(update)) {
                ssize_t n = read(pipe_fd[0], (char *)&update + used, sizeof(update) - used);
                if (n < 0 && errno == EINTR) continue;
                if (n <= 0) goto finished;
                used += (size_t)n;
            }
            draw_display(&update, is_master);
        }
finished:
        CloseFontx(&font[0]);
        display_destroy(&screen);
        close(pipe_fd[0]);

        _exit(0);
    }
    close(pipe_fd[0]);

    CloseFontx(&font[0]);
    display_destroy(&screen);
    if (screen_pid < 0) { perror("display fork"); close(pipe_fd[1]); return 1; }
    screen_pipe = pipe_fd[1];
    printf("[DISPLAY] Renderer running (pid=%ld).\n", (long)screen_pid);
    return 0;
}

static void stop_display(void)
{
    if (screen_pipe >= 0) { close(screen_pipe); screen_pipe = -1; }
    if (screen_pid <= 0) return;

    for (unsigned i = 0; i < 100; ++i) {
        pid_t r = waitpid(screen_pid, NULL, WNOHANG);
        if (r == screen_pid || (r < 0 && errno == ECHILD)) { screen_pid = -1; return; }
        sleep_msec(20);
    }
    kill(screen_pid, SIGKILL);
    while (waitpid(screen_pid, NULL, 0) < 0 && errno == EINTR) {}
    screen_pid = -1;
}
