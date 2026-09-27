/* ===========================================================================
 *  hash_table.c
 *
 *  DSA Assignment - Question 1 (15 marks)
 *  "A music application stores song IDs ..."
 *
 *  (a) Build a hash table with the DIVISION METHOD, insert the song IDs,
 *      print the table after every insertion and identify the collisions.
 *  (b) Search a set of song IDs twice - once with hashing (the table) and
 *      once with linear search (the raw, unsorted array) - counting the
 *      number of operations / key comparisons of each search.
 *  (c) Analyse how collisions affect search performance, compute the load
 *      factor and compare the measured work against the theoretical
 *      complexity of hashing and linear search.
 *
 *  Build : gcc -std=c11 -Wall -Wextra -O2 -o build/hash_table src/hash_table.c
 *  Run   : ./build/hash_table input/song_ids.txt
 *
 *  Hash function   : Division method, h(k) = k mod m
 *  Collisions      : resolved by linear probing (open addressing),
 *                    probe sequence  h, h+1, h+2, ... (mod m)
 *
 *  Counting rules used throughout (kept identical for every scenario):
 *    probe       - one examination of a table slot, whether or not the slot
 *                 is empty, occupied or a key match.  For an unsuccessful
 *                 search the empty slot that terminates the probe sequence
 *                 IS counted, because it is examined to end the search.
 *    comparison  - one comparison of a query against a key in the
 *                 unsorted array.
 * =========================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_N 256          /* maximum number of song IDs / queries          */
#define MAX_M 512          /* maximum table size                              */
#define EMPTY  (-1)        /* marks a free slot (song IDs are all positive)   */

/* -------------------------------------------------------------------------- */
/*  Data types                                                                 */
/* -------------------------------------------------------------------------- */

typedef struct {
    int key;      /* song ID stored in the slot, EMPTY if the slot is free */
    int home;     /* index the division method sent this key to             */
    int probes;   /* probes spent when the key was inserted                  */
} Slot;

static int songs [MAX_N];   /* song IDs, in the order supplied          */
static int n_songs         = 0;
static int queries[MAX_N];  /* IDs submitted to the two search routines */
static int n_queries       = 0;

/* -------------------------------------------------------------------------- */
/*  Input                                                                      */
/* -------------------------------------------------------------------------- */

/* Reads [SONGS] and [QUERIES] sections.  Comments (# ...) and blank lines
   are ignored, so the data file stays readable.                             */
static void load_input(const char *path)
{
    FILE *f = fopen(path, "r");
    if (f == NULL) {
        fprintf(stderr, "error: cannot open input file \"%s\"\n", path);
        exit(EXIT_FAILURE);
    }

    char line[512];
    int section = 0;                 /* 0 = none, 1 = songs, 2 = queries */

    while (fgets(line, sizeof line, f) != NULL) {
        char *comment = strchr(line, '#');
        if (comment != NULL) {
            *comment = '\0';
        }

        char *p = line;
        while (*p == ' ' || *p == '\t') {
            p++;
        }

        if (*p == '[') {                                  /* section header */
            if      (strstr(p, "SONGS")   != NULL) section = 1;
            else if (strstr(p, "QUERIES") != NULL) section = 2;
            continue;
        }
        if (*p < '0' || *p > '9') {                      /* not a number   */
            continue;
        }

        int value = atoi(p);
        if      (section == 1 && n_songs   < MAX_N) songs  [n_songs++]   = value;
        else if (section == 2 && n_queries < MAX_N) queries[n_queries++] = value;
    }

    fclose(f);

    if (n_songs == 0) {
        fprintf(stderr, "error: no song IDs found in \"%s\"\n", path);
        exit(EXIT_FAILURE);
    }
}

/* -------------------------------------------------------------------------- */
/*  PART (a): the hash table                                                   */
/* -------------------------------------------------------------------------- */

/* Division method.  m is the table size, keys are non-negative. */
static int hash_div(int key, int m)
{
    return key % m;
}

static void table_init(Slot *t, int m)
{
    for (int i = 0; i < m; i++) {
        t[i].key    = EMPTY;
        t[i].home   = -1;
        t[i].probes = 0;
    }
}

/* Linear-probing insertion.
   Returns the index used, sets *collision when the home slot was already
   taken, and returns -1 if the table is full.                              */
static int insert_key(Slot *t, int m, int key, int *collision)
{
    int home = hash_div(key, m);
    int idx  = home;

    for (int p = 1; p <= m; p++) {
        if (t[idx].key == EMPTY) {
            t[idx].key    = key;
            t[idx].home   = home;
            t[idx].probes = p;
            *collision    = (p > 1);
            return idx;
        }
        idx = (idx + 1) % m;          /* linear probing: next slot          */
    }

    *collision = 1;
    return -1;                        /* table full                          */
}

/* Builds the complete table for table size m and returns the number of
   collisions.  The table is cleared first, so the caller only has to supply
   the array.  insert_key() only reports whether the *current* key collided,
   so the caller has to accumulate - that is done here in one place.      */
static int build_table(Slot *t, int m)
{
    int collisions = 0;

    table_init(t, m);

    for (int i = 0; i < n_songs; i++) {
        int collision = 0;
        insert_key(t, m, songs[i], &collision);
        collisions += collision;
    }

    return collisions;
}

/* -------------------------------------------------------------------------- */
/*  PART (b): the two search routines                                         */
/* -------------------------------------------------------------------------- */

/* Search through the hash table.  Returns the number of probes; *found says
   whether the song ID was located.                                           */
static int search_hash(const Slot *t, int m, int key, int *found)
{
    int idx = hash_div(key, m);

    for (int p = 1; p <= m; p++) {
        if (t[idx].key == EMPTY) {     /* empty slot -> the key is absent    */
            *found = 0;
            return p;
        }
        if (t[idx].key == key) {
            *found = 1;
            return p;
        }
        idx = (idx + 1) % m;
    }

    *found = 0;                       /* table completely full              */
    return m;
}

/* Search through the raw unsorted array.  Returns the number of key
   comparisons performed.                                                    */
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

/* -------------------------------------------------------------------------- */
/*  PART (c): table statistics                                                 */
/* -------------------------------------------------------------------------- */

/* Longest run of consecutive occupied slots (the worst case a linear-probe
   search has to walk through) and the number of such runs.                  */
static void cluster_stats(const Slot *t, int m, int *clusters, int *longest)
{
    *clusters = 0;
    *longest  = 0;
    int run    = 0;

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
    if (run > 0) {                    /* cluster touching the end of table  */
        (*clusters)++;
        if (run > *longest) {
            *longest = run;
        }
    }
}

/* -------------------------------------------------------------------------- */
/*  Pretty printing                                                            */
/* -------------------------------------------------------------------------- */

static void rule(const char *title)
{
    printf("\n");
    printf("================================================================================\n");
    if (title != NULL) {
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
        int h = hash_div(songs[i], m);
        printf("     | %4d |   %2d   | home slot   |\n", songs[i], h);
    }

    printf("     +------+--------+-------------+\n");
}

/* -------------------------------------------------------------------------- */
/*  One complete experiment for a given table size                            */
/* -------------------------------------------------------------------------- */

static void run_scenario(int m, int verbose)
{
    Slot table[MAX_M];
    table_init(table, m);

    rule(NULL);
    printf("  TABLE SIZE m = %d          alpha = n/m = %d/%d = %.3f\n",
           m, n_songs, m, (double) n_songs / m);
    rule("PART (a)  INSERTION TRACE  (division method + linear probing)");

    print_hash_map(m);
    printf("\n");

    int collisions   = 0;
    int total_probes = 0;

    for (int i = 0; i < n_songs; i++) {
        int key      = songs[i];
        int home     = hash_div(key, m);
        int collision = 0;
        int slot     = insert_key(table, m, key, &collision);

        collisions   += collision;
        total_probes += table[slot].probes;

        printf("  [%d] Insert %4d : h(%d) = %d mod %d = %2d  ->  slot %2d  |  probes = %d  |  %s\n",
               i + 1, key, key, key, m, home, slot, table[slot].probes,
               collision ? "*** COLLISION ***" : "no collision");

        if (verbose) {
            char caption[128];
            snprintf(caption, sizeof caption,
                     "table state after inserting %d  (inserted %d of %d)", key, i + 1, n_songs);
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
           collisions == 0 ? "(perfect distribution)" : "(keys needed a probe sequence)");
    printf("    total probes to build  : %d\n", total_probes);
    printf("    average probes / key   : %.2f\n", (double) total_probes / n_songs);
    printf("    primary clusters       : %d\n", clusters);
    printf("    longest cluster        : %d slots\n", longest);

    /* ------------------------------------------------------------------ */
    rule("PART (b)  SEARCH COMPARISON  (hashing  vs  linear search)");

    printf("     %-6s | %-9s | %-27s | %-25s\n",
           "key", "in data", "linear search (comparisons)", "hashing (probes)");
    printf("     %-6s-+-%-9s-+-%-27s-+-%-25s\n",
           "------", "---------", "---------------------------", "-------------------------");

    int linear_total = 0;
    int hash_total   = 0;
    int hits = 0;

    for (int i = 0; i < n_queries; i++) {
        int key = queries[i];
        int fl = 0, fh = 0;
        int cmp = search_linear(songs, n_songs, key, &fl);
        int prb = search_hash(table, m, key, &fh);

        linear_total += cmp;
        hash_total   += prb;
        if (fl == fh) {
            hits++;                        /* both agree on the answer     */
        }

        char lbuf[32], hbuf[32];
        snprintf(lbuf, sizeof lbuf, "%2d  %s (idx %d)", cmp, fl ? "FOUND " : "not found", fl ? cmp - 1 : -1);
        snprintf(hbuf, sizeof hbuf, "%2d  %s (home %2d)", prb, fh ? "FOUND " : "not found", hash_div(key, m));

        printf("     %-6d | %-9s | %-27s | %-25s\n", key,
               fl ? "present" : "ABSENT", lbuf, hbuf);
    }

    printf("     %-6s-+-%-9s-+-%-27s-+-%-25s\n",
           "------", "---------", "---------------------------", "-------------------------");
    printf("     %-6s | %-9s | %-27s | %-25s\n", "TOTAL", "", "", "");
    printf("     %-6s | %-9s | %-27d | %-25d\n", "", "", linear_total, hash_total);
    printf("     %-6s | %-9s | %-27.2f | %-25.2f\n", "", "AVERAGE",
           (double) linear_total / n_queries, (double) hash_total / n_queries);
    printf("     %-6s | %-9s | %-27s | %-25d\n", "", "SAVING", "", linear_total - hash_total);

    if (hits == n_queries) {
        printf("\n  Both routines returned the same verdict for all %d queries.\n", n_queries);
    } else {
        printf("\n  WARNING: the two routines disagreed on %d query/queries!\n", n_queries - hits);
    }

    /* ------------------------------------------------------------------ */
    rule("PART (c)  COLLISION IMPACT AND LOAD FACTOR");

    printf("  load factor      alpha = n / m = %d / %d = %.3f\n", n_songs, m, (double) n_songs / m);
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
}

/* -------------------------------------------------------------------------- */
/*  main                                                                       */
/* -------------------------------------------------------------------------- */

/*  m = 12  primary table   - a composite size, produces a realistic mix of
                              collisions and clusters for the trace tables
     m = 11  prime size      - the textbook recommendation; no collisions
     m = 21  multiple of 105 - every song ID hashes to slot 0, the worst
                              case for this particular key set                */
static const int sizes[] = { 12, 11, 21 };
#define N_SIZES ((int)(sizeof sizes / sizeof sizes[0]))

int main(int argc, char *argv[])
{
    const char *path = (argc > 1) ? argv[1] : "input/song_ids.txt";
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

    int hash_total   [N_SIZES];
    int linear_total [N_SIZES];
    int collisions   [N_SIZES];

    for (int s = 0; s < N_SIZES; s++) {
        /* remember the totals so the closing comparison can use them */
        Slot table[MAX_M];
        collisions[s] = build_table(table, sizes[s]);

        int lt = 0, ht = 0;
        for (int i = 0; i < n_queries; i++) {
            int fl = 0, fh = 0;
            lt += search_linear(songs, n_songs, queries[i], &fl);
            ht += search_hash(table, sizes[s], queries[i], &fh);
        }
        linear_total[s] = lt;
        hash_total[s]   = ht;

        run_scenario(sizes[s], s == 0);   /* full trace only for the primary */
    }

    /* ------------------------------------------------------------------ */
    rule("OVERALL COMPARISON OF THE THREE TABLE SIZES");

    printf("   %-5s | %-7s | %-8s | %-11s | %-9s | %-12s | %-10s\n",
           "m", "alpha", "collis.", "avg probes", "max clust", "avg linear", "speed-up");
    printf("   %-5s-+-%-7s-+-%-8s-+-%-11s-+-%-9s-+-%-12s-+-%-10s\n",
           "-----", "-------", "--------", "-----------", "---------",
           "------------", "----------");

    for (int s = 0; s < N_SIZES; s++) {
        int m = sizes[s];
        Slot table[MAX_M];
        build_table(table, m);
        int clusters, longest;
        cluster_stats(table, m, &clusters, &longest);

        printf("   %-5d | %-7.3f | %-8d | %-11.2f | %-9d | %-12.2f | %-9.2fx\n",
               m, (double) n_songs / m, collisions[s],
               (double) hash_total[s] / n_queries, longest,
               (double) linear_total[s] / n_queries,
               (double) linear_total[s] / hash_total[s]);
    }

    printf("   %-5s-+-%-7s-+-%-8s-+-%-11s-+-%-9s-+-%-12s-+-%-10s\n",
           "-----", "-------", "--------", "-----------", "---------",
           "------------", "----------");
    printf("\n   %-5s | linear search is the same for every table size: %d comparisons in total.\n",
           "", linear_total[0]);
    printf("   %-5s | hashing wins at every table size, but the winning margin depends on\n",
           "");
    printf("   %-5s | how the song IDs are distributed - not on the load factor alone.\n", "");

    return EXIT_SUCCESS;
}
