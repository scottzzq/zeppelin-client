/*
 * "Copyright [2016] qihoo"
 */
#ifndef CLIENT_INCLUDE_ZP_CLUSTER_C_H_
#define CLIENT_INCLUDE_ZP_CLUSTER_C_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct zp_status_t      zp_status_t;
typedef struct zp_option_t      zp_option_t;
typedef struct zp_cluster_t     zp_cluster_t;
typedef struct zp_node_t        zp_node_t;
typedef struct zp_node_vec_t    zp_node_vec_t;
typedef struct zp_string_t      zp_string_t;
typedef struct zp_string_vec_t  zp_string_vec_t;

// struct's constructor and destructor
extern int zp_status_ok(const zp_status_t* s);
extern zp_string_t* zp_status_tostring(const zp_status_t* s);
extern void zp_status_destroy(zp_status_t* s);

extern zp_option_t* zp_option_create(zp_node_vec_t* metas, int op_timeout);
extern void zp_option_destroy(zp_option_t* option);

extern zp_cluster_t* zp_cluster_create(const zp_option_t* options);
extern void zp_cluster_destroy(zp_cluster_t* cluster);

extern zp_node_t* zp_node_create1(const char* ip, int port);
extern zp_node_t* zp_node_create();
extern void zp_node_destroy(zp_node_t* node);
extern char* zp_node_ip(zp_node_t* node);
extern int zp_node_port(zp_node_t* node);

extern zp_node_vec_t* zp_nodevec_create();
extern void zp_nodevec_destroy(zp_node_vec_t* vec);
extern void zp_nodevec_pushback(zp_node_vec_t* nodevec, const zp_node_t* node);
extern zp_node_t* zp_nodevec_popback(zp_node_vec_t* nodevec);

extern zp_string_t* zp_string_create1(const char* data, int length);
extern zp_string_t* zp_string_create();
extern void zp_string_destroy(zp_string_t* str);
extern char* zp_string_data(zp_string_t* str);
extern int zp_string_length(zp_string_t* str);

extern zp_string_vec_t* zp_strvec_create();
extern void zp_strvec_destroy(zp_string_vec_t* vec);
extern void zp_strvec_pushback(zp_string_vec_t* nodevec, zp_string_t* str);
extern zp_string_t* zp_strvec_popback(zp_string_vec_t* strvec);

// Zeppelin cluster interface
extern zp_status_t* zp_create_table(
    const zp_cluster_t* cluster,
    const char* table_name,
    int partition_num);

extern zp_status_t* zp_drop_table(
    const zp_cluster_t* cluster,
    const char* table_name);

extern zp_status_t* zp_pull(
    const zp_cluster_t* cluster,
    const char* table);

// statistical cmd
extern zp_status_t* zp_list_table(
    const zp_cluster_t* cluster,
    zp_string_vec_t* tables);

extern zp_status_t* zp_list_meta(
    const zp_cluster_t* cluster,
    zp_node_t* master,
    zp_node_vec_t* slaves);

extern zp_status_t* zp_metastatus(
    const zp_cluster_t* cluster,
    zp_string_t* status);

extern zp_status_t* zp_list_node(
    const zp_cluster_t* cluster,
    zp_node_vec_t* nodes,
    zp_string_vec_t* status);

// data operation
extern zp_status_t* zp_cluster_set(
    const zp_cluster_t* cluster,
    const char* table_name,
    const zp_string_t* key,
    const zp_string_t* value,
    int ttl);

extern zp_status_t* zp_cluster_get(
    const zp_cluster_t* cluster,
    const char* table_name,
    const zp_string_t* key,
    zp_string_t* value);

extern zp_status_t* zp_cluster_mget(
    const zp_cluster_t* cluster,
    const char* table_name,
    zp_string_vec_t* keys,
    zp_string_vec_t* res_keys,
    zp_string_vec_t* res_values);

extern zp_status_t* zp_cluster_delete(
    const zp_cluster_t* cluster,
    const char* table_name,
    const zp_string_t* key);


#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // CLIENT_INCLUDE_ZP_CLUSTER_H_
