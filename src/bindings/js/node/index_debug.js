const testAddon = require('./build/Debug/ov_node_addon.node');

const core = new testAddon.Core();
const model = core.readModel('absolute-path-to-model');

console.log(model.getName())

module.exports = testAddon;
