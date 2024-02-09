const testAddon = require('./build/Release/ov_node_addon.node');
console.log("Module initialized.\n\n")


function testMethodA() {
    const t = testAddon.methodA();
    if (t instanceof testAddon.Basic) {
        console.log("Yes, it's BasicWrap")
    }
    console.log("Exiting methodA test \n");
}
testMethodA()

// Check C++ custom class destructors. -------------------------------
function testBasicWrap(iter) {
    const t_obj = new testAddon.Basic();

    if(iter % 100_000 == 0) {
    console.log(`Exiting testBasicWrap ${t_obj.getMessage()}`);
    }
}

for (let i = 0; i<5_000_000; i++) {
    testBasicWrap(i);
    if (i % 1000 == 0) {
        console.log(`Test heapUsed using node-addon ObjectWrap ${process.memoryUsage().heapUsed/1000000} ${process.memoryUsage().heapTotal/1000000}MB\n`) 

    }
}
    
// -------------------------------------------------------------------



function testMethodB() {
    const t = testAddon.methodB();
    if (t instanceof testAddon.Basic) {
        console.log("Yes, it's BasicWrap")
    }
    console.log("Exiting methodB test \n");
}
testMethodB()
testMethodA()


module.exports = testAddon;
console.log("\n\nTHE END \n\n")
