/*
 * "Copyright [2016] qihoo"
 */
#ifndef CLIENT_INCLUDE_ZP_CLUSTER_H_
#define CLIENT_INCLUDE_ZP_CLUSTER_H_

#include <map>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

#include "slash/include/slash_status.h"
#include "slash/include/slash_mutex.h"

#include "libzp/include/zp_entity.h"

namespace pink {
  class BGThread;
}

namespace client {
  class CmdRequest;
  class CmdResponse;
}

namespace ZPMeta {
  class MetaCmd;
  class MetaCmdResponse;
  class MetaCmdResponse_Pull;
}

namespace libzp {

class ZpCli;
class ConnectionPool;
struct CmdContext;

class Cluster {
public:
  explicit Cluster(const Options& options);
  Cluster(const std::string& ip, int port);
  virtual ~Cluster();
  Status Connect();

  uint64_t op_timeout() {
    return options_.op_timeout;
  }

  // data cmd
  Status Set(const std::string& table, const std::string& key,
      const std::string& value, int32_t ttl = -1);
  Status Delete(const std::string& table, const std::string& key);
  Status Get(const std::string& table, const std::string& key,
      std::string* value);
  Status Mget(const std::string& table, const std::vector<std::string>& keys,
      std::map<std::string, std::string>* values);
  Status FlushTable(const std::string& table);

  // async data cmd
  Status Aset(const std::string& table, const std::string& key,
      const std::string& value, zp_completion_t complietion, void* data,
      int32_t ttl = -1);
  Status Adelete(const std::string& table, const std::string& key,
      zp_completion_t complietion, void* data);
  Status Aget(const std::string& table, const std::string& key,
      zp_completion_t complietion, void* data);
  Status Amget(const std::string& table, const std::vector<std::string>& keys,
      zp_completion_t complietion, void* data);

  // meta cmd
  Status CreateTable(const std::string& table_name, int partition_num);
  Status DropTable(const std::string& table_name);
  Status Pull(const std::string& table);
  Status FetchMetaInfo(const std::string& table, Table* table_meta);
  Status SetMaster(const std::string& table, const int partition,
      const Node& ip_port);
  Status AddSlave(const std::string& table, const int partition,
      const Node& ip_port);
  Status RemoveSlave(const std::string& table, const int partition,
      const Node& ip_port);

  // statistical cmd
  Status ListTable(std::vector<std::string>* tables);
  Status ListMeta(Node* master, std::vector<Node>* nodes);
  Status MetaStatus(std::string* meta_status);
  Status ListNode(std::vector<Node>* nodes,
      std::vector<std::string>* status);

  Status InfoQps(const std::string& table, int32_t* qps, int64_t* total_query);
  Status InfoRepl(const Node& node,
      const std::string& table, std::map<int, PartitionView>* partitions);
  Status InfoSpace(const std::string& table,
      std::vector<std::pair<Node, SpaceInfo> >* nodes);
  Status InfoServer(const Node& node, ServerState* state);

  // local cmd
  Status DebugDumpPartition(const std::string& table, int partition_id = -1);
  int LocateKey(const std::string& table, const std::string& key);

 private:
  
  void Init();
  Status PullInternal(const std::string& table, uint64_t deadline);
  bool DeliverMget(CmdContext* context);
  bool Deliver(CmdContext* context);
  void DeliverAndPull(CmdContext* context);
  static void DoAsyncTask(void* arg);
  void AddAsyncTask(CmdContext* context);
  static void DoNodeTask(void* arg);
  void AddNodeTask(const Node& node, CmdContext* context);
  Status SubmitDataCmd(const Node& master,
      client::CmdRequest& req, client::CmdResponse *res,
      uint64_t deadline, int attempt = 0);
  Status SubmitMetaCmd(ZPMeta::MetaCmd& req, ZPMeta::MetaCmdResponse *res,
      uint64_t deadline, int attempt = 0);
  std::shared_ptr<ZpCli> GetMetaConnection(uint64_t deadline, Status* s);

  // options
  Options options_;
 
  // Protect meta info include epoch, tables
  // Which may be changed when meta updated
  pthread_rwlock_t meta_rw_;
  int64_t epoch_;
  std::unordered_map<std::string, Table*> tables_;
  void ResetMetaInfo(const std::string& table_name,
      const ZPMeta::MetaCmdResponse_Pull& pull);
  void PointUpdateMetaInfo(const std::string& table_name,
      const std::string& sample_key, const Node& target);

  Status GetTableMasters(const std::string& table_name,
    std::set<Node>* related_nodes);
  Status GetDataMasterById(const std::string& table,
    int partition_id, Node* master);
  Status GetDataMaster(const std::string& table,
      const std::string& key, Node* master);
  Status UpdateDataMaster(const std::string& table,
      const std::string& key, const Node& target);
  Status UpdateDataMasterById(const std::string& table_name,
      int partition_id, const Node& target);

  // connection pool
  ConnectionPool* meta_pool_;
  ConnectionPool* data_pool_;

  // Pb command for communication
  ZPMeta::MetaCmd *meta_cmd_;
  ZPMeta::MetaCmdResponse *meta_res_;
  CmdContext* context_;
  
  // BG worker
  slash::Mutex peer_mu_;
  std::map<Node, pink::BGThread*> peer_workers_;
  pink::BGThread* async_worker_;
};

} // namespace libzp

#endif  // CLIENT_INCLUDE_ZP_CLUSTER_H_
