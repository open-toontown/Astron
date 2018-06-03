#pragma once
#include <unordered_map>

#include "core/Role.h"
#include "core/RoleFactory.h"
#include "core/global.h"
#include "DatabaseBackend.h"
#include "DBOperation.h"
#include "DBOperationQueue.h"

extern RoleConfigGroup dbserver_config;

class DatabaseServer : public Role
{
  public:
    DatabaseServer(RoleConfig);

    virtual void handle_datagram(DatagramHandle in_dg, DatagramIterator &dgi);

  private:
    void handle_operation(DBOperation *op);
    void clear_operation(const DBOperation *op);
    std::unordered_map<doid_t, DBOperationQueue> m_queues;
    std::recursive_mutex m_lock;

    void init_metrics();

    DatabaseBackend *m_db_backend;
    LogCategory *m_log;

    channel_t m_control_channel;
    doid_t m_min_id, m_max_id;
    bool m_broadcast;

    // Prometheus metrics.
    prometheus::Family<prometheus::Counter>* m_ops_completed_builder = nullptr;
    std::unordered_map<DBOperation::OperationType, prometheus::Counter*> m_ops_completed;
    prometheus::Family<prometheus::Counter>* m_ops_failed_builder = nullptr;
    std::unordered_map<DBOperation::OperationType, prometheus::Counter*> m_ops_failed;
    prometheus::Family<prometheus::Histogram>* m_completion_time_builder;
    std::unordered_map<DBOperation::OperationType, prometheus::Histogram*> m_completion_time;

    void report_failure(DBOperation *op);
    void report_success(DBOperation *op);

    friend class DBOperation;
    friend class DBOperationCreate;
    friend class DBOperationDelete;
    friend class DBOperationGet;
    friend class DBOperationSet;
    friend class DBOperationUpdate;

    friend class DBOperationQueue;
};
