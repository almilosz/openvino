const ov = require('./build/Release/ov_node_addon.node');


// node --trace-gc --max-old-space-size=50 index_debug.js
function reportMemoryUsage() {
    console.log(`Test heapUsed  ${process.memoryUsage().heapUsed/1000000}MB`);
    console.log(`Test heapTotal ${process.memoryUsage().heapTotal/1000000}MB`);
    console.log(`Test external  ${process.memoryUsage().external/1000000}MB`);
    console.log(`Test arrayBuffers  ${process.memoryUsage().arrayBuffers/1000000}MB`);
    console.log(`Test rss ${process.memoryUsage().rss/1000000}MB\n`);
}



const SIZE = 100_000;
const TIMEOUT = 10000;

async function times(number, fn, { name } = {}) {
    const transformedName = name ? `'${name}' ` : '';
    console.log(`Start ${transformedName} ${number} times`);
  
    for (let i = 0; i < number; i++) {
      fn(i);
    //   await new Promise(setImmediate); // Uncomment to fix memory managment issues
    }
  
    console.log(`Done ${transformedName}`);
}

function testBasicWrap(i) {
    const t_obj = new ov.Basic();
    if (i%10000==0) reportMemoryUsage();
}

function createAndReleaseCore(i) {
    const core = new ov.Core();
    if (i%10000==0) reportMemoryUsage();
    
}

function createAndReleaseTensor(i) {
    const largeArray = new Float32Array(SIZE).fill(1);
    new ov.Tensor("f32", [1, SIZE], largeArray);
    if (i%10_000==0) reportMemoryUsage();
}

function main() {
    console.log('Start main!');
  
    // times(300000, createAndReleaseCore, { name: 'Core' }); // FIXME: leaks!
    // times(300000, createAndReleaseTensor, { name: 'Tensor' }); // FIXME: leaks!
    times(1_00_000, testBasicWrap, { name: 'Basic' });
 
    console.log('Done main!');


}

main()
