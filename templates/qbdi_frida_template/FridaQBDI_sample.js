// QBDI
const qbdi = require('/usr/local/share/qbdi/frida-qbdi'); // import QBDI bindings
qbdi.import(); // Set bindings to global environment

// Initialize QBDI
var vm = new QBDI();
var state = vm.getGPRState();
var stack = vm.allocateVirtualStack(state, 0x100000);

// Instrument "Secret" function from demo.bin
var funcPtr = Module.findExportByName(null, "Secret");
if (!funcPtr) {
    funcPtr = DebugSymbol.fromName("Secret");
}
vm.addInstrumentedModuleFromAddr(funcPtr);

// Callback on every instruction
// This callback will print context and display current instruction address and dissassembly
// We choose to print only XOR instructions
var icbk = vm.newInstCallback(function(vm, gpr, fpr, data) {
    var inst = vm.getInstAnalysis();
    if (inst.mnemonic.search("XOR")){
        return VMAction.CONTINUE;
    }
    gpr.dump(); // Display context
    console.log("0x" + inst.address.toString(16) + " " + inst.disassembly); // Display instruction dissassembly
    return VMAction.CONTINUE;
});
var iid = vm.addCodeCB(InstPosition.PREINST, icbk);

// Allocate a string in remote process memory
var strP = Memory.allocUtf8String("Hello world !");
// Call the Secret function using QBDI and with our string as argument
vm.call(funcPtr, [strP]);
