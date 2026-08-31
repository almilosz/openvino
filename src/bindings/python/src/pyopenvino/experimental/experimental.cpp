// Copyright (C) 2018-2026 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#include "pyopenvino/experimental/experimental.hpp"

#include "openvino/core/bound_evaluation_util.hpp"
#include "openvino/core/descriptor_tensor.hpp"
#include "openvino/core/validation_util.hpp"
#include "transformations/rt_info/disable_precision_conversion.hpp"

namespace py = pybind11;

void regmodule_experimental(py::module m) {
    py::module m_dev = m.def_submodule("experimental", "openvino.experimental submodule");

    m_dev.def("evaluate_as_partial_shape",
              &ov::util::evaluate_as_partial_shape,
              py::arg("output"),
              py::arg("partial_shape"),
              R"(
                    Evaluates lower and upper value estimations for the output tensor.
                    The estimation will be represented as a partial shape object,
                    using Dimension(min, max) for each element.

                    :param output: Node output pointing to the tensor for estimation.
                    :type output: openvino.Output
                    :param partial_shape: The resulting estimation will be stored in this PartialShape.
                    :type partial_shape: openvino.PartialShape
                    :return: True if estimation evaluation was successful, false otherwise.
                    :rtype: bool
                )");
    m_dev.def("evaluate_both_bounds",
              &ov::util::evaluate_both_bounds,
              py::arg("output"),
              R"(
                    Evaluates lower and upper value estimations of the output tensor.
                    It traverses the graph upwards to deduce the estimation.

                    :param output: Node output pointing to the tensor for estimation.
                    :type output: openvino.Output
                    :return: Tensors representing the lower and upper bound value estimations.
                    :rtype: tuple[openvino.Tensor, openvino.Tensor]
                )");
    m_dev.def("set_element_type",
              &ov::descriptor::set_element_type,
              py::arg("tensor"),
              py::arg("element_type"),
              R"(
                    Sets element type for a tensor descriptor in the OV model graph.

                    :param tensor: The tensor descriptor whose element type is to be set.
                    :type tensor: openvino.Tensor
                    :param element_type: A new element type of the tensor descriptor.
                    :type element_type: openvino.Type
                )");
    m_dev.def("set_tensor_type",
              &ov::descriptor::set_tensor_type,
              py::arg("tensor"),
              py::arg("element_type"),
              py::arg("partial_shape"),
              R"(
                    Changes element type and partial shape of a tensor descriptor in the OV model graph.

                    :param tensor: The tensor descriptor whose element type is to be set.
                    :type tensor: openvino.Tensor
                    :param element_type: A new element type of the tensor descriptor.
                    :type element_type: openvino.Type
                    :param partial_shape: A new partial shape of the tensor desriptor.
                    :type partial_shape: openvino.PartialShape
                )");
    m_dev.def(
        "disable_fp16_compression",
        [](const std::shared_ptr<ov::Node>& node) {
            ov::disable_conversion(node, ov::element::f16);
        },
        py::arg("node"),
        R"(
                    Marks a node as prohibited to convert to FP16, so it will be kept and
                    inferred in its original precision. This is the supported replacement
                    for manually setting the legacy ["precise_0"] rt_info key from Python.

                    :param node: Node to mark.
                    :type node: openvino.Node
                )");
    m_dev.def(
        "is_fp16_compression_disabled",
        [](const std::shared_ptr<const ov::Node>& node) {
            return ov::is_conversion_disabled(node, ov::element::f16);
        },
        py::arg("node"),
        R"(
                    Checks whether FP16 conversion is disabled for the given node.

                    :param node: Node to check.
                    :type node: openvino.Node
                    :return: True if conversion to FP16 is disabled for the node, False otherwise.
                    :rtype: bool
                )");
}
