// Copyright (C) 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

const { addon: ov } = require('../../../../src/bindings/js/node');
const path = require('path');

function generateImage(shape = [1, 3, 32, 32]) {
  const lemm = shape.reduce(
    (accumulator, currentValue) => accumulator * currentValue,
    1,
  );

  const epsilon = 0.5; // To avoid very small numbers
  const tensorData = Float32Array.from(
    { length: lemm },
    () => Math.random() + epsilon,
  );

  return tensorData;
}

function benchmark(model, datasetSize) {
  console.log(`Running benchmark for ${datasetSize} iterations...`);
  const shape = model.inputs[0].getShape();
  const dataset = Array.from(
    { length: datasetSize + 10 }, () => generateImage(shape),
  );
  const inferRequest = model.createInferRequest();

  // Warm up
  for (let i = 0; i < 10; i++) {
    inferRequest.infer([dataset[i]]);
  }

  let times = [];
  for (let i = 1; i < datasetSize; i++) {
    const startTime = process.hrtime();
    inferRequest.infer([dataset[i]]);
    let endTime = process.hrtime(startTime);
    const duration = (endTime[0] * 1e3 + endTime[1] / 1e6);
    times.push(duration);
  }

  return times;
}

async function main() {
  let deviceName = 'CPU';
  const args = process.argv;

  if (args.length === 4) {
    deviceName = args[2];
  } else if (args.length !== 3) {
    console.log(`Usage: ${path.basename(args[1])} 
      <path_to_model> <device_name>(default: CPU)`);

    return 1;
  }

  const latency = {'PERFORMANCE_HINT': 'LATENCY'};

  const core = new ov.Core();
  // Reads and compiles the model with one input and one output.
  const compiledModel = await core.compileModel(args[2], deviceName, latency);

  const datasetSize = 1000;
  const results = benchmark(compiledModel, datasetSize);

  const avgTime = results.reduce((a, b) => a + b, 0) / results.length;
  const minTime = Math.min(...results);
  const maxTime = Math.max(...results);
  const sortedTimes = [...results].sort((a, b) => a - b);
  const mid = Math.floor(sortedTimes.length / 2);
  const medianTime = sortedTimes.length % 2 !== 0
    ? sortedTimes[mid]
    : (sortedTimes[mid - 1] + sortedTimes[mid]) / 2;

  console.log(`Average inference time: ${avgTime.toFixed(2)} ms`);
  console.log(`Min inference time: ${minTime.toFixed(2)} ms`);
  console.log(`Max inference time: ${maxTime.toFixed(2)} ms`);
  console.log(`Median inference time: ${medianTime.toFixed(2)} ms`);

}

main();
