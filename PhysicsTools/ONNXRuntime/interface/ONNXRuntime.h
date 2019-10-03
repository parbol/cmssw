/*
 * ONNXRuntime.h
 *
 *  Created on: Jun 28, 2019
 *      Author: hqu
 */

#ifndef PHYSICSTOOLS_ONNXRUNTIME_INTERFACE_ONNXRUNTIME_H_
#define PHYSICSTOOLS_ONNXRUNTIME_INTERFACE_ONNXRUNTIME_H_

#include <vector>
#include <map>
#include <string>
#include <memory>

#include "onnxruntime/core/session/onnxruntime_cxx_api.h"

namespace Ort {

typedef std::vector<std::vector<float>> FloatArrays;

class ONNXRuntime {
public:

  ONNXRuntime();
  ONNXRuntime(const ONNXRuntime&) = delete;
  ONNXRuntime& operator=(const ONNXRuntime&) = delete;
  virtual ~ONNXRuntime();

  // Creates session
  void createSession(const std::string& model_path, const SessionOptions *session_options = nullptr);

  // Run inference and get outputs
  // input_names: list of the names of the input nodes.
  // input_values: list of input arrays for each input node. The order of `input_values` must match `input_names`.
  // output_names: names of the output nodes to get outputs from. Empty list means all output nodes.
  // batch_size: number of samples in the batch. Each array in `input_values` must have a shape layout of (batch_size, ...).
  // Returns: a std::vector<std::vector<float>>, with the order matched to `output_names`.
  // When `output_names` is empty, will return all outputs ordered as in `getOutputNames()`.
  FloatArrays run(const std::vector<std::string>& input_names, FloatArrays input_values,
      const std::vector<std::string>& output_names = {}, int64_t batch_size = 1) const;

  // Get a list of names of all the output nodes
  const std::vector<std::string>& getOutputNames() const;

  // Get the shape of a output node
  // The 0th dim depends on the batch size, therefore is set to -1
  const std::vector<int64_t>& getOutputShape(const std::string& output_name) const;

private:
  static Env& getEnv();
  std::unique_ptr<Session> session_;

  std::vector<std::string> input_node_strings_;
  std::vector<const char*> input_node_names_;
  std::map<std::string, std::vector<int64_t>> input_node_dims_;

  std::vector<std::string> output_node_strings_;
  std::vector<const char*> output_node_names_;
  std::map<std::string, std::vector<int64_t>> output_node_dims_;
};

} /* namespace onnxruntime */

#endif /* PHYSICSTOOLS_ONNXRUNTIME_INTERFACE_ONNXRUNTIME_H_ */
