#include <gtest/gtest.h>

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>

#include "all/titov_s_ImageFilter_HorizGaussian3x3/include/ops_all.hpp"
#include "boost/mpi/communicator.hpp"
#include "core/perf/include/perf.hpp"
#include "core/task/include/task.hpp"

TEST(titov_s_image_filter_horiz_gaussian3x3_all, test_pipeline_run) {
  boost::mpi::communicator world;
  constexpr size_t kWidth = 9000;
  constexpr size_t kHeight = 9000;
  std::vector<double> input(kWidth * kHeight, 0.0);
  std::vector<double> output(kWidth * kHeight, 0.0);
  std::vector<double> expected(kWidth * kHeight, 0.0);
  std::vector<int> kernel = {1, 2, 1};

  auto init_cell = [&](size_t i, size_t j) {
    const size_t idx = (i * kWidth) + j;
    input[idx] = (j % 3 == 0) ? 100.0 : 0.0;

    if (j == kWidth - 1) {
      expected[idx] = 0.0;
    } else if (j % 3 == 0) {
      expected[idx] = 50.0;
    } else {
      expected[idx] = 25.0;
    }
  };

  for (size_t i = 0; i < kHeight; ++i) {
    for (size_t j = 0; j < kWidth; ++j) {
      init_cell(i, j);
    }
  }

  auto task_data_seq = std::make_shared<ppc::core::TaskData>();
  task_data_seq->inputs.emplace_back(reinterpret_cast<uint8_t *>(input.data()));
  task_data_seq->inputs.emplace_back(reinterpret_cast<uint8_t *>(kernel.data()));
  task_data_seq->inputs_count.emplace_back(input.size());
  task_data_seq->outputs.emplace_back(reinterpret_cast<uint8_t *>(output.data()));
  task_data_seq->outputs_count.emplace_back(output.size());

  auto test_task_stl = std::make_shared<titov_s_image_filter_horiz_gaussian3x3_all::GaussianFilterALL>(task_data_seq);

  auto perf_attr = std::make_shared<ppc::core::PerfAttr>();
  perf_attr->num_running = 10;
  const auto t0 = std::chrono::high_resolution_clock::now();
  perf_attr->current_timer = [&] {
    auto current_time_point = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(current_time_point - t0).count();
    return static_cast<double>(duration) * 1e-9;
  };

  auto perf_results = std::make_shared<ppc::core::PerfResults>();
  auto perf_analyzer = std::make_shared<ppc::core::Perf>(test_task_stl);
  perf_analyzer->PipelineRun(perf_attr, perf_results);
  ppc::core::Perf::PrintPerfStatistic(perf_results);
  auto verify_cell = [&](size_t i, size_t j) {
    const size_t idx = (i * kWidth) + j;
    ASSERT_NEAR(output[idx], expected[idx], 1e-6) << "Mismatch at [" << i << "," << j << "]";
  };

  if (world.rank() == 0) {
    for (size_t i = 0; i < kHeight; ++i) {
      for (size_t j = 0; j < kWidth; ++j) {
        verify_cell(i, j);
      }
    }
  }
}

TEST(titov_s_image_filter_horiz_gaussian3x3_all, test_task_run) {
  boost::mpi::communicator world;
  constexpr size_t kWidth = 9000;
  constexpr size_t kHeight = 9000;
  std::vector<double> input(kWidth * kHeight, 0.0);
  std::vector<double> output(kWidth * kHeight, 0.0);
  std::vector<double> expected(kWidth * kHeight, 0.0);
  std::vector<int> kernel = {1, 2, 1};

  auto init_cell = [&](size_t i, size_t j) {
    const size_t idx = (i * kWidth) + j;
    input[idx] = (j % 3 == 0) ? 100.0 : 0.0;

    if (j == kWidth - 1) {
      expected[idx] = 0.0;
    } else if (j % 3 == 0) {
      expected[idx] = 50.0;
    } else {
      expected[idx] = 25.0;
    }
  };

  for (size_t i = 0; i < kHeight; ++i) {
    for (size_t j = 0; j < kWidth; ++j) {
      init_cell(i, j);
    }
  }

  auto task_data_seq = std::make_shared<ppc::core::TaskData>();
  task_data_seq->inputs.emplace_back(reinterpret_cast<uint8_t *>(input.data()));
  task_data_seq->inputs.emplace_back(reinterpret_cast<uint8_t *>(kernel.data()));
  task_data_seq->inputs_count.emplace_back(input.size());
  task_data_seq->outputs.emplace_back(reinterpret_cast<uint8_t *>(output.data()));
  task_data_seq->outputs_count.emplace_back(output.size());

  auto test_task_stl = std::make_shared<titov_s_image_filter_horiz_gaussian3x3_all::GaussianFilterALL>(task_data_seq);

  auto perf_attr = std::make_shared<ppc::core::PerfAttr>();
  perf_attr->num_running = 10;
  const auto t0 = std::chrono::high_resolution_clock::now();
  perf_attr->current_timer = [&] {
    auto current_time_point = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(current_time_point - t0).count();
    return static_cast<double>(duration) * 1e-9;
  };

  auto perf_results = std::make_shared<ppc::core::PerfResults>();
  auto perf_analyzer = std::make_shared<ppc::core::Perf>(test_task_stl);
  perf_analyzer->TaskRun(perf_attr, perf_results);
  ppc::core::Perf::PrintPerfStatistic(perf_results);

  auto verify_cell = [&](size_t i, size_t j) {
    const size_t idx = (i * kWidth) + j;
    ASSERT_NEAR(output[idx], expected[idx], 1e-6) << "Mismatch at [" << i << "," << j << "]";
  };

  if (world.rank() == 0) {
    for (size_t i = 0; i < kHeight; ++i) {
      for (size_t j = 0; j < kWidth; ++j) {
        verify_cell(i, j);
      }
    }
  }
}
