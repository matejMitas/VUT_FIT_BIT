/**
 * Kostra programu pro 3. projekt IZP 2015/16
 *
 * Jednoducha shlukova analyza
 * Complete linkage
 * http://is.muni.cz/th/172767/fi_b/5739129/web/web/clsrov.html
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h> // sqrtf
#include <limits.h> // INT_MAX
#include <stdbool.h>

/*****************************************************************
 * Ladici makra. Vypnout jejich efekt lze definici makra
 * NDEBUG, napr.:
 *   a) pri prekladu argumentem prekladaci -DNDEBUG
 *   b) v souboru (na radek pred #include <assert.h>
 *      #define NDEBUG
 */
#ifdef NDEBUG
#define debug(s)
#define dfmt(s, ...)
#define dint(i)
#define dfloat(f)
#else

// vypise ladici retezec
#define debug(s) printf("- %s\n", s)

// vypise formatovany ladici vystup - pouziti podobne jako printf
#define dfmt(s, ...) printf(" - "__FILE__":%u: "s"\n",__LINE__,__VA_ARGS__)

// vypise ladici informaci o promenne - pouziti dint(identifikator_promenne)
#define dint(i) printf(" - " __FILE__ ":%u: " #i " = %d\n", __LINE__, i)

// vypise ladici informaci o promenne typu float - pouziti
// dfloat(identifikator_promenne)
#define dfloat(f) printf(" - " __FILE__ ":%u: " #f " = %g\n", __LINE__, f)

#endif

/*****************************************************************
 * Deklarace potrebnych datovych typu:
 *
 * TYTO DEKLARACE NEMENTE
 *
 *   struct obj_t - struktura objektu: identifikator a souradnice
 *   struct cluster_t - shluk objektu:
 *      pocet objektu ve shluku,
 *      kapacita shluku (pocet objektu, pro ktere je rezervovano
 *          misto v poli),
 *      ukazatel na pole shluku.
 */

struct obj_t {
    int id;
    float x;
    float y;
};

struct cluster_t {
    int size;
    int capacity;
    struct obj_t *obj;
};

/*****************************************************************
 * Deklarace potrebnych funkci.
 *
 * PROTOTYPY FUNKCI NEMENTE
 *
 * IMPLEMENTUJTE POUZE FUNKCE NA MISTECH OZNACENYCH 'TODO'
 *
 */

/*
 Inicializace shluku 'c'. Alokuje pamet pro cap objektu (kapacitu).
 Ukazatel NULL u pole objektu znamena kapacitu 0.
 */
void init_cluster(struct cluster_t *c, int cap)
{
    assert(c != NULL);
    assert(cap >= 0);
    
    // TODO
    
    /* ==========================================
     
     Create new cluster
     
     ========================================== */
    c->size = 0;
    c->capacity = cap;
    c->obj = malloc(sizeof(struct obj_t) * cap);
}

/*
 Odstraneni vsech objektu shluku a inicializace na prazdny shluk.
 */
void clear_cluster(struct cluster_t *c)
{
    // TODO
    c->capacity = 0;
    c->size = 0;
    free(c->obj);
}

/// Chunk of cluster objects. Value recommended for reallocation.
const int CLUSTER_CHUNK = 10;

/*
 Zmena kapacity shluku 'c' na kapacitu 'new_cap'.
 */
struct cluster_t *resize_cluster(struct cluster_t *c, int new_cap)
{
    // TUTO FUNKCI NEMENTE
    assert(c);
    assert(c->capacity >= 0);
    assert(new_cap >= 0);
    
    if (c->capacity >= new_cap)
        return c;
    
    size_t size = sizeof(struct obj_t) * new_cap;
    
    void *arr = realloc(c->obj, size);
    if (arr == NULL)
        return NULL;
    
    c->obj = arr;
    c->capacity = new_cap;
    
    return c;
}

/*
 Prida objekt 'obj' na konec shluku 'c'. Rozsiri shluk, pokud se do nej objekt
 nevejde.
 */
void append_cluster(struct cluster_t *c, struct obj_t obj)
{
    // TODO
    /* ==========================================
     
     Add object to the cluster
     
     ========================================== */
    int size = c->size;
    int cap = c->capacity;

    
    if (size == cap) {
        cap++;
        resize_cluster(c, cap);
    }
    
    c->obj[size] = obj;
    c->size++;
}

/*
 Seradi objekty ve shluku 'c' vzestupne podle jejich identifikacniho cisla.
 */
void sort_cluster(struct cluster_t *c);

/*
 Do shluku 'c1' prida objekty 'c2'. Shluk 'c1' bude v pripade nutnosti rozsiren.
 Objekty ve shluku 'c1' budou serazny vzestupne podle identifikacniho cisla.
 Shluk 'c2' bude nezmenen.
 */
void merge_clusters(struct cluster_t *c1, struct cluster_t *c2)
{
    assert(c1 != NULL);
    assert(c2 != NULL);
    
    // TODO
    //int size1;
    int size2 = c2->size;
    
    for (int i = 0; i < size2; i++) {
        append_cluster(c1, c2->obj[i]);
    }
    
    // sort cluster for the bottom up
    sort_cluster(c1);
}

/**********************************************************************/
/* Prace s polem shluku */

/*
 Odstrani shluk z pole shluku 'carr'. Pole shluku obsahuje 'narr' polozek
 (shluku). Shluk pro odstraneni se nachazi na indexu 'idx'. Funkce vraci novy
 pocet shluku v poli.
 */
int remove_cluster(struct cluster_t *carr, int narr, int idx)
{
    assert(idx < narr);
    assert(narr > 0);
    
    // TODO
    // move items to the left
    int newNarr = narr - 1;
    
    clear_cluster(&carr[idx]);
    
    for (int i = idx; i < newNarr; i++) {
        *(carr + i) = *(carr + (i + 1));
    }
    
    //carr = realloc(carr, sizeof(struct cluster_t) * newNarr);
    
    return newNarr;
}

/*
 Pocita Euklidovskou vzdalenost mezi dvema objekty.
 */
float obj_distance(struct obj_t *o1, struct obj_t *o2)
{
    assert(o1 != NULL);
    assert(o2 != NULL);
    
    return sqrt((pow(o1->x - o2->x, 2)) + (pow(o1->y - o2->y, 2)));
}

/*
 Pocita vzdalenost dvou shluku.
 */
float cluster_distance(struct cluster_t *c1, struct cluster_t *c2)
{
    assert(c1 != NULL);
    assert(c1->size > 0);
    assert(c2 != NULL);
    assert(c2->size > 0);
    
    // TODO
    
    int size1 = c1->size;
    int size2 = c2->size;
    float maxDist = 0;
    
    
    for (int i = 0; i < size1; i++) {
        //printf("X-ova souradnice objektu %d z clusteru a: %f\n", i, c1->obj[i].x);
        
        for (int j = 0; j < size2; j++) {
            float currentDist = obj_distance(&c1->obj[i], &c2->obj[j]);
            
            if (currentDist > maxDist) {
                maxDist = currentDist;
            }
            
            //printf("Vzdalenost pro bod %d, z clusteru b je: %f\n", j, currentDist);
            //printf("X-ova souradnice objektu %d z clusteru b: %f\n", j, c2->obj[j].x);
        }
    }
    
    return maxDist;
}

/*
 Funkce najde dva nejblizsi shluky. V poli shluku 'carr' o velikosti 'narr'
 hleda dva nejblizsi shluky. Nalezene shluky identifikuje jejich indexy v poli
 'carr'. Funkce nalezene shluky (indexy do pole 'carr') uklada do pameti na
 adresu 'c1' resp. 'c2'.
 */
void find_neighbours(struct cluster_t *carr, int narr, int *c1, int *c2)
{
    assert(narr > 0);
    
    // TODO
    float minDist = 1000;
    int tempC2 = 1;
    // set indexes of closest clusters
    *c1 = 0;
    *c2 = 1;
    
    //printf("%d", *c1);
    
    for (int i = 0; i < narr; i++) {
        //printf("Cluster to be compared to: %d\n", i);
        
        float minDistOne = 1000;
        
        for (int j = 0; j < narr; j++) {
            // prevent comparing cluster to itself
            if (j != i) {
                float currentDist = cluster_distance(&carr[i], &carr[j]);
                
                if (currentDist < minDistOne) {
                    minDistOne = currentDist;
                    tempC2 = j;
                }
                
                //printf("Cluster to be compared with: %d, its distance %f\n", j, currentDist);
            }
        }
        
        if (minDistOne < minDist) {
            minDist = minDistOne;
            *c1 = i;
            *c2 = tempC2;
        }
    }
    //printf("Distance %f\n", minDist);
}

// pomocna funkce pro razeni shluku
static int obj_sort_compar(const void *a, const void *b)
{
    // TUTO FUNKCI NEMENTE
    const struct obj_t *o1 = a;
    const struct obj_t *o2 = b;
    if (o1->id < o2->id) return -1;
    if (o1->id > o2->id) return 1;
    return 0;
}

/*
 Razeni objektu ve shluku vzestupne podle jejich identifikatoru.
 */
void sort_cluster(struct cluster_t *c)
{
    // TUTO FUNKCI NEMENTE
    qsort(c->obj, c->size, sizeof(struct obj_t), &obj_sort_compar);
}

/*
 Tisk shluku 'c' na stdout.
 */
void print_cluster(struct cluster_t *c)
{
    // TUTO FUNKCI NEMENTE
    for (int i = 0; i < c->size; i++)
    {
        if (i) putchar(' ');
        printf("%d[%g,%g]", c->obj[i].id, c->obj[i].x, c->obj[i].y);
    }
    putchar('\n');
}

/*
 Ze souboru 'filename' nacte objekty. Pro kazdy objekt vytvori shluk a ulozi
 jej do pole shluku. Alokuje prostor pro pole vsech shluku a ukazatel na prvni
 polozku pole (ukalazatel na prvni shluk v alokovanem poli) ulozi do pameti,
 kam se odkazuje parametr 'arr'. Funkce vraci pocet nactenych objektu (shluku).
 V pripade nejake chyby uklada do pameti, kam se odkazuje 'arr', hodnotu NULL.
 */
int load_clusters(char *filename, struct cluster_t **arr)
{
    assert(arr != NULL);
    
    // TODO
    // variable for count of objects in file
    int count = 0;
    // variable for opened file
    FILE * fp;
    // set file mode
    fp = fopen (filename, "r");
    
    if (fp == NULL) {
        fprintf(stderr, ">File can't be opened\n");
    } else {
        // get count of objects
        fscanf(fp, "count=%d", &count);

        /* ==========================================
         
         Alloc memory and write there
         
         ========================================== */
        
        *arr = malloc(sizeof(struct cluster_t) * count);
        
        
        // loop through object to show
        for (int i = 0; i < count; i++) {
            
            /* ==========================================
             
             Create new cluster
             
             ========================================== */
            
            // create new cluster
            struct cluster_t newCluster;
            
            // pass new cluster to the function
            init_cluster(&newCluster, 1);
        
            /* ==========================================
             
             Create new object for cluster
             
             ========================================== */
            
            struct obj_t newObj;
            
            // read what's in input text file & asign values to the newly created object
            fscanf(fp, "%d %f %f", &newObj.id, &newObj.x, &newObj.y);
            
            
            
            // call function to append the object to the cluster
            append_cluster(&newCluster, newObj);
            
            /* ==========================================
             
             Append to the clusters array
             
             ========================================== */
            (*arr)[i] = newCluster;
        }
        fclose(fp);
    }
    
    return count;
}

/*
 Tisk pole shluku. Parametr 'carr' je ukazatel na prvni polozku (shluk).
 Tiskne se prvnich 'narr' shluku.
 */
void print_clusters(struct cluster_t *carr, int narr)
{
    printf("Clusters:\n");
    for (int i = 0; i < narr; i++)
    {
        printf("cluster %d: ", i);
        print_cluster(&carr[i]);
    }
}

int main(int argc, char *argv[])
{
    struct cluster_t *clusters = NULL;
    
    // TODO
    bool cont = false;
    int count = 0;
    int endNumber = 1;
    int status = EXIT_SUCCESS;
    
    /* ==========================================
     
     Arguments handling
     
     ========================================== */
    
    if (argc >= 2 && argc <= 3) {
        
        count = load_clusters(argv[1], &clusters);
        cont = true;
        
        if (argc == 3) {
            char *ptr;
            double ret;
            
            ret = strtod(argv[2], &ptr);
            
            if ((ptr == argv[2]) || (*ptr != '\0')) {
                fprintf(stderr, ">Second argument must be a number\n");
                status = EXIT_FAILURE;
            } else {
                cont = true;
                endNumber = ret;
            }
        }
    } else {
        fprintf(stderr, ">Wrong nubmer of valid arguments\n");
        status = EXIT_FAILURE;
    }
    
    
    /* ==========================================
     
     Testing
     
     ========================================== */
    
    if (cont) {
        
        int cToMerge1 = 0;
        int cToMerge2 = 0;
        
        while (count > endNumber) {
            find_neighbours(clusters, count, &cToMerge1, &cToMerge2);
            merge_clusters(&clusters[cToMerge1], &clusters[cToMerge2]);
            count = remove_cluster(clusters, count, cToMerge2);
        }
        
        print_clusters(clusters, count);
        
        for (int i = 0; i < count; i++) {
            clear_cluster(&clusters[i]);
        }
        
        free(clusters);
    }
    
    return status;
}
