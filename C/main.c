/*
 * Scroll down past most of this to the main file for a comment explaining
 * the two modes of this project. Generally, to get a final image, you
 * run 3 steps:
 *
 * Chart -> Render -> Display (in Python)
 *
 * Skip past chart() and render() to read about main.
 */

#include "main_helper.h"
#include "path_generator.h"
#include "render.h"


void chart(const char *outfile, size_t history_len, uint8_t n, path_gen get_path, uint8_t depth) {
    paths_t paths = explore_paths(history_len, n, get_path);

    FILE *fp = fopen(outfile, "w");

    clock_t start, end;

    start = clock();
    uint64_t visited = iterate(&paths, depth, fp);
    end = clock();

    printf("It took %f seconds to visit %s points\n", ((double) (end - start)) / CLOCKS_PER_SEC, commafy(visited));

    fclose(fp);

    free_paths_t(&paths);
}


void render(const char *infile, const char *outfile, uint32_t size) {
    static uint8_t buf[8192];
    static uint32_t write;

    FILE *fpin = fopen(infile, "r");

    fseek(fpin, 0, SEEK_END);
    size_t num_bytes = ftell(fpin);
    fseek(fpin, 0, SEEK_SET);

    if (num_bytes < 2) {
        fputs("File is too smol, aborting\n", stderr);
        abort();
    }

    uint8_t n_points, depth;
    if (fread(&n_points, 1, 1, fpin) != 1) abort();
    if (fread(&depth, 1, 1, fpin) != 1) abort();

    renderer_t renderer = prepare_renderer(n_points, depth, size);
    uint64_t visited = 0;
    uint64_t update_schedule = 10;

    clock_t start = clock();

    while (!feof(fpin)) {
        size_t bytes_read = fread(buf, 1, sizeof(buf), fpin);

        for (size_t i = 0; i < bytes_read; i++) {
            dfs_count(&renderer, buf[i]);

            if (buf[i] != 0xff) {
                visited++;

                if (visited == update_schedule) {
                    printf("Visited %s points in %f seconds\n", commafy(visited), ((double) (clock() - start)) / CLOCKS_PER_SEC);
                    update_schedule *= 10;
                }
            }
        }
    }
    printf("Visited %s points in %f seconds\n", commafy(visited), ((double) (clock() - start)) / CLOCKS_PER_SEC);
    fclose(fpin);


    printf("Writing countfile to %s\n", outfile);
    start = clock();
    FILE *fpout = fopen(outfile, "w");
    write = htobe32(size);
    uint64_t bytes_written = 0;
    bytes_written += fwrite(&write, sizeof(uint32_t), 1, fpout);
    uint32_t max_visits = 0;

    for (size_t i = 0; i < size * size; i++) {
        max_visits = imax(max_visits, renderer.counts[i]);
        renderer.counts[i] = htobe32(renderer.counts[i]);
    }
    bytes_written += fwrite(renderer.counts, sizeof(uint32_t), size * size, fpout);
    fclose(fpout);
    printf("Saved countfile (%s B) in %f seconds\n", commafy(bytes_written), ((double) (clock() - start)) / CLOCKS_PER_SEC);

    free_renderer(&renderer);
}


/*
 * The main function runs one of the modes and quits. You'll need to run
 * this program twice to Chart and then Render a fractal. If I kept working
 * on this, then I would eventually support writing / reading from stdin / stdout,
 * letting you chain together invocations without needing to save to a ramfile.
 *
 * If there are 0 arguments, it "Charts" the fractal. That means generating
 * a depth-first tree of which corners get selected. The results are
 * saved to out.path.
 *
 * If there is *any* extra argument, it "Renders" the fractal. Rendering means
 * following the DFS tree of decisions to compute the actual XY coords visited,
 * saving the pixels those coords land on to a file, out.render.
 *
 * Last, after uncommenting some code in the Python project, you can read
 * out.render and get your final image.
 */
int main(int argc, char **argv) {
    ProgramMode mode;
    if (argc == 1)
        mode = Chart;
    else
        mode = Render;

    switch (mode) {
        case Chart:
            /*
             * As configured, this writes a ~17 MB file when rendering to a depth of 10.
             * Note that file size increases exponentially with render depth.
             */
            chart("/dev/shm/out.path", 2, 5, fancy, 10);
            break;
        case Render:
            /*
             * This reads out.path, writes the pixel counts to out.render, for an image
             * size of 5,000x5,000 px.
             */
            render("/dev/shm/out.path", "/dev/shm/out.render", 5000);
            break;
        default:
            fputs("Unknown mode type\n", stderr);
            abort();
    }

    return 0;
}
