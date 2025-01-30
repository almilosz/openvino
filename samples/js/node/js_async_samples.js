// import openvino-node module
// const { addon: ov } = require('openvino-node');
const { addon: ov } = require('../../../src/bindings/js/node');
const Image = require('./image.js');
const {argMax, findMax} = require('./helpers.js');
const imagenetClassesMap = require('../assets/datasets/imagenet_class_index.json');
const imagenetClasses = ['background', ...Object.values(imagenetClassesMap)];
assert = require('assert');
const imgPath = "../assets/images/coco.jpg"

// https://github.com/vishniakov-nikolai/openvino_build_deploy/commit/76cf270888dbeaea13419098c45b0715d8766fee

main()

async function simple_check(core, model, inputTensor) {
    const compiledModel = await core.compileModel(model, 'AUTO');
    const inferRequest = compiledModel.createInferRequest();
    inferRequest.infer([inputTensor]);

    const outputLayer = compiledModel.outputs[0];

    const resultInfer = inferRequest.getTensor(outputLayer);

    
    const classId = argMax(resultInfer.data);
    console.log(`${classId}\t${resultInfer.data[(classId)].toFixed(7)}\t${imagenetClasses[classId][1]}`)
    console.log(`------------`)
}

async function main() {
    const img = await Image.load(imgPath);
    inputTensor = new ov.Tensor(
        ov.element.u8,
        [1, img.height, img.width, 3],
        new Uint8ClampedArray(img.rgb),
      );
      console.log(inputTensor.data[20])

    const core = new ov.Core();
    modelPath = "../assets/models/v3-small_224_1.0_float.xml"
    const model = await core.readModel(modelPath);

    const _ppp = new ov.preprocess.PrePostProcessor(model);
    _ppp.input().tensor().setElementType(ov.element.u8).setShape(inputTensor.getShape()).setLayout('NHWC');
    _ppp.input().preprocess().resize(ov.preprocess.resizeAlgorithm.RESIZE_LINEAR);
    _ppp.input().model().setLayout('NHWC');
    _ppp.output().tensor().setElementType(ov.element.f32);
    _ppp.build();

    // Sanity check
    await simple_check(core, model, inputTensor)



    const ITER = 100;
    let start;
    let end;
    let time;
    // Test#1 100 async inference iterations
    const compiledModel_1 = await core.compileModel(model, 'AUTO');
    const inferRequest_1 = compiledModel_1.createInferRequest();
    
    start = new Date();
    for (let i = 0; i < ITER; i++) {
        console.log(inputTensor.data[20])
        var resultPromise = inferRequest_1.inferAsync([inputTensor]);
        resultPromise.then(() => {
            const resultInfer = inferRequest_1.getOutputTensor();
            const classId = argMax(resultInfer.data);
            // assert.deepStrictEqual(classId, 206); //fails
            console.log(`${classId}\t${resultInfer.data[(classId)].toFixed(7)}\t${imagenetClasses[classId][1]}`)
        });
        await resultPromise;
    }
    end = new Date();
    time = end - start;
    console.log(`Time ${time}. Time per iteration: ${time / ITER} ms`); // Original time per iteration: 7.4 ms

    await new Promise(r => setTimeout(r, 2000));
    // Test 100  inference iterations
    const dogImage = await Image.load(imgPath);
    dogTensor = new ov.Tensor(
        ov.element.u8,
        [1, dogImage.height, dogImage.width, 3],
        new Uint8ClampedArray(dogImage.rgb),
      );
      console.log(dogTensor.data[20])


    const compiledModel_2 = await core.compileModel(model, 'AUTO');
    const inferRequest_2 = compiledModel_2.createInferRequest();
    start = new Date();
    for (let i = 0; i < ITER; i++) {
        result = inferRequest_2.infer([dogTensor]);
        tensor = result['MobilenetV3/Predictions/Softmax:0'];
        const classId = argMax(tensor.data);
        assert.deepStrictEqual(classId, 206);
        // console.log(`${classId}\t${tensor.data[(classId)].toFixed(7)}\t${imagenetClasses[classId][1]}`)
    }
    end = new Date();
    time = end - start;
    console.log(`Time ${time}. Time per iteration: ${time / ITER} ms`);  // Original time per iteration: 1.21 ms.

    // After changes, but the results are incorrect for async inference
    // async ~2.52 ms
    // sync ~1.11 ms


    console.log('-- end --')
}

