const { addon: ov } = require('openvinojs-node');


const { cv } = require('opencv-wasm');
const { getImageData } = require('./helpers.js');

pathImg1 = 'path_to_img';
pathImg2 = 'path_to_img';
pathImg3 = 'path_to_img';
pathImg4 = 'path_to_img';

// // Correct results
// images = [ 
//     pathImg1,
//     pathImg2,
//     pathImg3,
//     pathImg4
//   ]

// Incorrect results
images = [ 
    pathImg1,
    pathImg1,
    pathImg2,
    pathImg2,
    pathImg1,
  ]

main( images);

async function main( images) {

  const [h, w] = [224,224];
  const imagesData = [];

  for (const imagePath of images) {
    console.log(imagePath);
    imagesData.push(await getImageData(imagePath));
  }


  const tData = [] // store tensor data to prevent gc
  const tensors = imagesData.map((imgData) => {
    const originalImage = cv.matFromImageData(imgData);
    const image = new cv.Mat();
    cv.cvtColor(originalImage, image, cv.COLOR_RGBA2RGB);
    cv.resize(image, image, new cv.Size(w, h));

    console.log(image.data[0], " 1st el of image.data")
    const tensorData = new Uint8Array(image.data);
    //tData.push(tensorData) // uncomment to fix 
    const shape = [1, h, w, 3];

    const tensor = new ov.Tensor(ov.element.u8, shape, tensorData);
    console.log(tensor.data[0], " 1st el of a new Tensor")
    return tensor;
  });
  console.log()

  
  const mapped_tensors = tensors.map((t, i) => {
    console.log(t.data[0], "1st el after the img processing");
    return t;
  });

//   // Uncomment to get totally random results
//   console.log("\nLoop over tensors")
//   for (t of tensors) 
//     console.log(t.data[0])

}
