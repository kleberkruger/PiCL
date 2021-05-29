#!/usr/bin/env python

import os, sniper_lib
import pandas as pd


class ExecutionData:
  def __init__(self, exec_time, num_mem_access, num_mem_writes, num_mem_logs = 0):
    self.exec_time = exec_time
    self.num_mem_access = num_mem_access
    self.num_mem_writes = num_mem_writes
    self.num_mem_logs = num_mem_logs

  def __str__(self):
    return '{} {} {} {}'.format(self.exec_time, self.num_mem_access, self.num_mem_writes, self.num_mem_logs)
    


class App:
  def __init__(self, name, baseline, picl):
    self.name = name
    self.baseline = baseline
    self.picl = picl
  
  def __str__(self):
    return 'Name: {}\n\tBaseline: {}\n\tPiCL: {}'.format(self.name, self.baseline, self.picl)


def get_exec_time(res):
  results = res['results']
  config = res['config']
  ncores = int(config['general/total_cores'])
  
  if 'barrier.global_time_begin' in results:
    time0_begin = results['barrier.global_time_begin']
    time0_end = results['barrier.global_time_end']
  
  if 'barrier.global_time' in results:
    time0 = results['barrier.global_time'][0]
  else:
    time0 = time0_begin - time0_end

  results['performance_model.elapsed_time_fixed'] = [
    time0
    for c in range(ncores)
  ]
  results['performance_model.cycle_count_fixed'] = [
    results['performance_model.elapsed_time_fixed'][c] * results['fs_to_cycles_cores'][c]
    for c in range(ncores)
  ]
  return long(results['performance_model.cycle_count_fixed'][0])


def get_num_mem_access(res):
  results = res['results']
  return map(sum, zip(results['dram.reads'], results['dram.writes']))[0]


def get_num_mem_writes(res):
  results = res['results']
  return results['dram.writes'][0]


def get_num_mem_logs(res):
  results = res['results']
  return results['onchip_undo_buffer.log_writes'][0]



def get_results_from_resultsdir(name, resultsdir):
  res = sniper_lib.get_results(0, resultsdir, None)
  return ExecutionData(get_exec_time(res), get_num_mem_access(res), get_num_mem_writes(res), get_num_mem_logs(res))


apps = []
for app_dir in os.listdir("."):
  try:
    app = App(app_dir, get_results_from_resultsdir(app_dir, app_dir + '/base'), get_results_from_resultsdir(app_dir, app_dir + '/pikl'))
    apps.append(app)
  except:
    print('An exception occurred in application: {}'.format(app_dir))

df = pd.DataFrame({
  'Application': [ a.name for a in apps ],
  'ExecTime:Baseline': [ a.baseline.exec_time for a in apps ],
  'ExecTime:PiCL': [ a.picl.exec_time for a in apps ],
  # 'ExecTime:Overhead': [ a.name for a in apps ],
  'MemAccess:Baseline': [ a.baseline.num_mem_access for a in apps ],
  'MemAccess:PiCL': [ a.picl.num_mem_access for a in apps ],
  # 'MemAccess:Overhead': [ a.name for a in apps ],
  'MemWrites:Baseline': [ a.baseline.num_mem_writes for a in apps ],
  'MemWrites:PiCL': [ a.picl.num_mem_writes for a in apps ],
  'MemWrites:Logging': [ a.picl.num_mem_logs for a in apps ],
  # 'MemWrites:Perc Log': [ a.name for a in apps ],
  # 'MemWrites:Overhead': [ a.name for a in apps ],
})

print(df)

writer = pd.ExcelWriter('results_cpu2006.xlsx', engine='xlsxwriter')
df.to_excel(writer, sheet_name='SPEC CPU2006', index=False)
writer.save()


