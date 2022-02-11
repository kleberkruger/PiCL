#include "nvm_perf_model_constant.h"
#include "simulator.h"
#include "config.h"
#include "config.hpp"
#include "stats.h"
#include "shmem_perf.h"

NvmPerfModelConstant::NvmPerfModelConstant(core_id_t core_id, UInt32 cache_block_size) :
      NvmPerfModel(core_id, cache_block_size),
      m_queue_model(nullptr),
      m_nvm_read_cost(NvmPerfModel::getReadLatency()),
      m_nvm_write_cost(NvmPerfModel::getWriteLatency()),
      m_nvm_log_cost(NvmPerfModel::getLogLatency()),
      m_nvm_bandwidth(8 * Sim()->getCfg()->getFloat("perf_model/dram/per_controller_bandwidth")),
      m_total_queueing_delay(SubsecondTime::Zero()),
      m_total_access_latency(SubsecondTime::Zero())
{
   if (Sim()->getCfg()->getBool("perf_model/dram/queue_model/enabled"))
   {
      m_queue_model = QueueModel::create("dram-queue", core_id,
                                         Sim()->getCfg()->getString("perf_model/dram/queue_model/type"),
                                         m_nvm_bandwidth.getRoundedLatency(8 * cache_block_size)); // bytes to bits
   }
   registerStatsMetric("dram", core_id, "total-access-latency", &m_total_access_latency);
   registerStatsMetric("dram", core_id, "total-queueing-delay", &m_total_queueing_delay);
}

NvmPerfModelConstant::~NvmPerfModelConstant()
{
   if (m_queue_model) {
      delete m_queue_model;
      m_queue_model = nullptr;
   }
}

SubsecondTime
NvmPerfModelConstant::getAccessLatency(SubsecondTime pkt_time, UInt64 pkt_size, core_id_t requester, IntPtr address,
                                       DramCntlrInterface::access_t access_type, ShmemPerf *perf)
{
   // pkt_size is in 'Bytes'
   // m_nvm_bandwidth is in 'Bits per clock cycle'
   if ((!m_enabled) || (requester >= (core_id_t) Config::getSingleton()->getApplicationCores())) {
      return SubsecondTime::Zero();
   }
   
   SubsecondTime processing_time = m_nvm_bandwidth.getRoundedLatency(8 * pkt_size); // bytes to bits

   // Compute Queue Delay
   SubsecondTime queue_delay = m_queue_model ? m_queue_model->computeQueueDelay(pkt_time, processing_time, requester)
                                             : SubsecondTime::Zero();

   SubsecondTime access_cost = (access_type == DramCntlrInterface::WRITE) ? m_nvm_write_cost : m_nvm_read_cost;
   SubsecondTime access_latency = queue_delay + processing_time + access_cost;

   // Added by Kleber Kruger
   // queue_delay + processing_time + access_cost
   // printf("- NVM access: %s | Latency: (%luq + %lup + %lua) = %lu ns\n", 
   //        access_type == DramCntlrInterface::READ ? "READ " :
   //        access_type == DramCntlrInterface::WRITE ? "WRITE" : "LOG",
   //        queue_delay.getNS(), processing_time.getNS(), access_cost.getNS(), access_latency.getNS());

   perf->updateTime(pkt_time);
   perf->updateTime(pkt_time + queue_delay, ShmemPerf::DRAM_QUEUE);
   perf->updateTime(pkt_time + queue_delay + processing_time, ShmemPerf::DRAM_BUS);
   perf->updateTime(pkt_time + queue_delay + processing_time + access_cost, ShmemPerf::DRAM_DEVICE);

   // Update Memory Counters
   m_num_accesses++;
   m_total_access_latency += access_latency;
   m_total_queueing_delay += queue_delay;

   return access_latency;
}
