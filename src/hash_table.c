#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_N 256
#define MAX_M 512
#define EMPTY -1

typedef struct {
    int key;
    int home;
    int probes;
} Slot;

static int songs[MAX_N], n_songs;
static int queries[MAX_N], n_queries;

static void load_input(const char *path)
{
    FILE *f = fopen(path, "r");
    if (!f) {
        fprintf(stderr, "error: cannot open input file \"%s\"\n", path);
        exit(EXIT_FAILURE);
    }

    char line[512];
    int section = 0;

    while (fgets(line, sizeof line, f)) {
        char *hash = strchr(line, '#');
        if (hash) {
            *hash = '\0';
        }

        char *p = line;
        while (*p == ' ' || *p == '\t') {
            p++;
        }

        if (*p == '[') {
            if (strstr(p, "SONGS")) {
                section = 1;
            } else if (strstr(p, "QUERIES")) {
                section = 2;
            }
            continue;
        }

        if (*p < '0' || *p > '9') {
            continue;
        }

        int v = atoi(p);
        if (section == 1 && n_songs < MAX_N) {
            songs[n_songs++] = v;
        } else if (section == 2 && n_queries < MAX_N) {
            queries[n_queries++] = v;
        }
    }

    fclose(f);

    if (!n_songs) {
        fprintf(stderr, "error: no song IDs found in \"%s\"\n", path);
        exit(EXIT_FAILURE);
    }
}

static int hash_div(int key, int m)
{
    return key % m;
}

static void table_init(Slot *t, int m)
{
    for (int i = 0; i < m; i++) {
        t[i].key = EMPTY;
        t[i].home = -1;
        t[i].probes = 0;
    }
}

static int insert_key(Slot *t, int m, int key, int *collision)
{
    int home = hash_div(key, m);
    int idx = home;

    for (int p = 1; p <= m; p++) {
        if (t[idx].key == EMPTY) {
            t[idx].key = key;
            t[idx].home = home;
            t[idx].probes = p;
            *collision = p > 1;
            return idx;
        }
        idx = (idx + 1) % m;
    }

    *collision = 1;
    return -1;
}

static int search_hash(const Slot *t, int m, int key, int *found)
{
    int idx = hash_div(key, m);

    for (int p = 1; p <= m; p++) {
        if (t[idx].key == EMPTY) {
            *found = 0;
            return p;
        }
        if (t[idx].key == key) {
            *found = 1;
            return p;
        }
        idx = (idx + 1) % m;
    }

    *found = 0;
    return m;
}

static int search_linear(const int *arr, int n, int key, int *found)
{
    for (int i = 0; i < n; i++) {
        if (arr[i] == key) {
            *found = 1;
            return i + 1;
        }
    }

    *found = 0;
    return n;
}

static void cluster_stats(const Slot *t, int m, int *clusters, int *longest)
{
    *clusters = 0;
    *longest = 0;
    int run = 0;

    for (int i = 0; i < m; i++) {
        if (t[i].key != EMPTY) {
            run++;
        } else if (run > 0) {
            (*clusters)++;
            if (run > *longest) {
                *longest = run;
            }
            run = 0;
        }
    }

    if (run > 0) {
        (*clusters)++;
        if (run > *longest) {
            *longest = run;
        }
    }
}

static void rule(const char *title)
{
    printf("\n");
    printf("================================================================================\n");
    if (title) {
        printf("  %s\n", title);
        printf("================================================================================\n");
    }
}

static void print_table(const Slot *t, int m, const char *caption)
{
    printf("     %s\n", caption);
    printf("     +------+------+--------+-------+\n");
    printf("     |  idx |  key |  home  | probe |\n");
    printf("     +------+------+--------+-------+\n");

    for (int i = 0; i < m; i++) {
        if (t[i].key == EMPTY) {
            printf("     |  %2d  |   -  |    -   |    -  |\n", i);
        } else {
            printf("     |  %2d  | %4d |   %2d   |   %d   |\n",
                   i, t[i].key, t[i].home, t[i].probes);
        }
    }

    printf("     +------+------+--------+-------+\n");
}

static void print_hash_map(int m)
{
    printf("     Division method  h(k) = k mod %d\n", m);
    printf("     +------+--------+-------------+\n");
    printf("     |  key |  h(k)  |   status     |\n");
    printf("     +------+--------+-------------+\n");

    for (int i = 0; i < n_songs; i++) {
        printf("     | %4d |   %2d   | home slot   |\n",
               songs[i], hash_div(songs[i], m));
    }

    printf("     +------+--------+-------------+\n");
}

static void run_scenario(int m, int verbose, int *out_coll, int *out_hash,
                         int *out_linear, int *out_longest)
{
    Slot table[MAX_M];
    table_init(table, m);

    rule(NULL);
    printf("  TABLE SIZE m = %d          alpha = n/m = %d/%d = %.3f\n",
           m, n_songs, m, (double) n_songs / m);
    rule("PART (a)  INSERTION TRACE  (division method + linear probing)");

    print_hash_map(m);
    printf("\n");

    int collisions = 0;
    int total_probes = 0;

    for (int i = 0; i < n_songs; i++) {
        int key = songs[i];
        int home = hash_div(key, m);
        int collision = 0;
        int slot = insert_key(table, m, key, &collision);

        collisions += collision;
        total_probes += table[slot].probes;

        printf("  [%d] Insert %4d : h(%d) = %d mod %d = %2d  ->  slot %2d  |  probes = %d  |  %s\n",
               i + 1, key, key, key, m, home, slot, table[slot].probes,
               collision ? "*** COLLISION ***" : "no collision");

        if (verbose) {
            char caption[128];
            snprintf(caption, sizeof caption,
                     "table state after inserting %d  (inserted %d of %d)",
                     key, i + 1, n_songs);
            print_table(table, m, caption);
        }
    }

    int clusters, longest;
    cluster_stats(table, m, &clusters, &longest);

    if (!verbose) {
        printf("\n");
        print_table(table, m, "final table state");
    }

    printf("\n  Insertion summary for m = %d\n", m);
    printf("    keys inserted          : %d\n", n_songs);
    printf("    collisions             : %d %s\n", collisions,
           collisions == 0 ? "(perfect distribution)"
                            : "(keys needed a probe sequence)");
    printf("    total probes to build  : %d\n", total_probes);
    printf("    average probes / key   : %.2f\n", (double) total_probes / n_songs);
    printf("    primary clusters       : %d\n", clusters);
    printf("    longest cluster        : %d slots\n", longest);

    rule("PART (b)  SEARCH COMPARISON  (hashing  vs  linear search)");

    printf("     %-6s | %-9s | %-27s | %-25s\n",
           "key", "in data", "linear search (comparisons)", "hashing (probes)");
    printf("     %-6s-+-%-9s-+-%-27s-+-%-25s\n",
           "------", "---------", "---------------------------", "-------------------------");

    int linear_total = 0;
    int hash_total = 0;
    int agree = 0;

    for (int i = 0; i < n_queries; i++) {
        int key = queries[i];
        int fl = 0, fh = 0;
        int cmp = search_linear(songs, n_songs, key, &fl);
        int prb = search_hash(table, m, key, &fh);

        linear_total += cmp;
        hash_total += prb;
        if (fl == fh) {
            agree++;
        }

        char lbuf[32], hbuf[32];
        snprintf(lbuf, sizeof lbuf, "%2d  %s (idx %d)",
                 cmp, fl ? "FOUND " : "not found", fl ? cmp - 1 : -1);
        snprintf(hbuf, sizeof hbuf, "%2d  %s (home %2d)",
                 prb, fh ? "FOUND " : "not found", hash_div(key, m));

        printf("     %-6d | %-9s | %-27s | %-25s\n", key,
               fl ? "present" : "ABSENT", lbuf, hbuf);
    }

    printf("     %-6s-+-%-9s-+-%-27s-+-%-25s\n",
           "------", "---------", "---------------------------", "-------------------------");
    printf("     %-6s | %-9s | %-27s | %-25s\n", "TOTAL", "", "", "");
    printf("     %-6s | %-9s | %-27d | %-25d\n", "", "", linear_total, hash_total);
    printf("     %-6s | %-9s | %-27.2f | %-25.2f\n", "", "AVERAGE",
           (double) linear_total / n_queries, (double) hash_total / n_queries);
    printf("     %-6s | %-9s | %-27s | %-25d\n", "", "SAVING", "",
           linear_total - hash_total);

    if (agree == n_queries) {
        printf("\n  Both routines returned the same verdict for all %d queries.\n",
               n_queries);
    } else {
        printf("\n  WARNING: the two routines disagreed on %d query/queries!\n",
               n_queries - agree);
    }

    rule("PART (c)  COLLISION IMPACT AND LOAD FACTOR");

    printf("  load factor      alpha = n / m = %d / %d = %.3f\n",
           n_songs, m, (double) n_songs / m);
    printf("  slots filled     %d of %d (%.1f%% occupied)\n",
           n_songs, m, 100.0 * n_songs / m);
    printf("  collisions       %d  ->  %s\n", collisions,
           collisions == 0
               ? "every song ID landed in its own home slot"
               : "linear probing was invoked, lengthening the probe sequences");
    printf("  longest cluster  %d slots -> a search starting at the head of that\n", longest);
    printf("                    cluster needs up to %d probes.\n", longest);
    printf("  avg hash probes  %.2f per query  (ideal = 1.00, i.e. hit on the home slot)\n",
           (double) hash_total / n_queries);
    printf("  avg linear cmps  %.2f per query  (n/2 = %.2f for a random hit on an\n",
           (double) linear_total / n_queries, (n_songs + 1) / 2.0);
    printf("                    unsorted array of %d keys)\n", n_songs);
    printf("  speed-up         %.2fx fewer operations than linear search\n",
           (double) linear_total / hash_total);

    *out_coll = collisions;
    *out_hash = hash_total;
    *out_linear = linear_total;
    *out_longest = longest;
}

static const int sizes[] = {12, 11, 21};
#define NS (int)(sizeof sizes / sizeof sizes[0])

int main(int argc, char *argv[])
{
    const char *path = argc > 1 ? argv[1] : "input/song_ids.txt";
    load_input(path);

    printf("================================================================================\n");
    printf("  HASH TABLE USING THE DIVISION METHOD      h(k) = k mod m\n");
    printf("  Collision resolution: linear probing (open addressing)\n");
    printf("================================================================================\n");
    printf("  data file   : %s\n", path);
    printf("  song IDs    :");
    for (int i = 0; i < n_songs; i++) {
        printf(" %d", songs[i]);
    }
    printf("\n  query IDs   :");
    for (int i = 0; i < n_queries; i++) {
        printf(" %d", queries[i]);
    }

    int coll[NS], hsh[NS], lin[NS], clus[NS];

    for (int s = 0; s < NS; s++) {
        run_scenario(sizes[s], s == 0, &coll[s], &hsh[s], &lin[s], &clus[s]);
    }

    rule("OVERALL COMPARISON OF THE THREE TABLE SIZES");

    printf("   %-5s | %-7s | %-8s | %-11s | %-9s | %-12s | %-10s\n",
           "m", "alpha", "collis.", "avg probes", "max clust", "avg linear", "speed-up");
    printf("   %-5s-+-%-7s-+-%-8s-+-%-11s-+-%-9s-+-%-12s-+-%-10s\n",
           "-----", "-------", "--------", "-----------", "---------",
           "------------", "----------");

    for (int s = 0; s < NS; s++) {
        printf("   %-5d | %-7.3f | %-8d | %-11.2f | %-9d | %-12.2f | %-9.2fx\n",
               sizes[s], (double) n_songs / sizes[s], coll[s],
               (double) hsh[s] / n_queries, clus[s],
               (double) lin[s] / n_queries, (double) lin[s] / hsh[s]);
    }

    printf("   %-5s-+-%-7s-+-%-8s-+-%-11s-+-%-9s-+-%-12s-+-%-10s\n",
           "-----", "-------", "--------", "-----------", "---------",
           "------------", "----------");
    printf("\n   %-5s | linear search is the same for every table size: %d comparisons in total.\n",
           "", lin[0]);
    printf("   %-5s | hashing wins at every table size, but the winning margin depends on\n", "");
    printf("   %-5s | how the song IDs are distributed - not on the load factor alone.\n", "");

    return EXIT_SUCCESS;
}
